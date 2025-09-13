//
// Filename :   EnviroSensor.cpp
// Abstruct :   Method for EnviroSensor class
// Author   :   application_division@atit.jp
// Update   :   2025/09/13  New Creation
#include "EnviroSensor.hpp"

namespace AMAGOI {
//
// Method   :   EnviroSensor
// Abstruct :   コンストラクタ
// Argument :   TwoWire* wireAddr   : [I]I2C通信クラスインスタンスへの参照
EnviroSensor::EnviroSensor( TwoWire* wireAddr ) {
    // I2C通信クラスインスタンス
    this->myWire        = wireAddr;
    // 初期設定値の生成
    uint8_t osrs_t      = EnviroSensor::OVER_SAMPLING;    // Temperature oversampling x 1
    uint8_t osrs_p      = EnviroSensor::OVER_SAMPLING;    // Pressure oversampling x 1
    uint8_t osrs_h      = EnviroSensor::OVER_SAMPLING;    // Humidity oversampling x 1
    uint8_t mode        = EnviroSensor::MODE;             // Normal mode
    uint8_t t_sb        = EnviroSensor::T_STANDBY;        // Timer Stand-by 1000ms
    uint8_t filter      = EnviroSensor::FILTER;           // Filter off
    uint8_t spi3w_en    = EnviroSensor::SPI3W;            // 3-wire SPI Disable

    // 初期設定値をレジスタに書き込み
    uint8_t ctrlMeas    = (osrs_t << 5) | (osrs_p << 2) | mode;
    writeRegister( REG_ADDR_CTRLMEAS, ctrlMeas );
    uint8_t config      = (t_sb << 5) | (filter << 2) | spi3w_en;
    writeRegister( REG_ADDR_CONFIG,   config );
    uint8_t ctrlHum     = osrs_h;    
    writeRegister( REG_ADDR_CTRLHUM,  ctrlHum );

    // 補正データをレジスタから読み込み
    readCorrectionValue();

    return;
}

//
// Method   :   readCorrectionValue
// Abstruct :   補正データをレジスタから読み出す
// Argument :   n/a
// Return   :   n/a
void EnviroSensor::readCorrectionValue() {
    uint8_t data[32] = { 0 };   // 読み出しデータ用テンポラリ
    char    i        = 0;       // カウンタ

    // レジスタから補正値を取得(0x88番地から0xA0番地)
    this->myWire->beginTransmission( I2C_ADDR );
    this->myWire->write( REG_ADDR_CTRLCORR1 );
    this->myWire->endTransmission();
    this->myWire->requestFrom( I2C_ADDR, 24 );
    while( this->myWire->available() ){
        data[i] = this->myWire->read();
        i++;
    }
    
    // レジスタから補正値を取得(0xA1番地)
    this->myWire->beginTransmission( I2C_ADDR );
    this->myWire->write( REG_ADDR_CTRLCORR2 );
    this->myWire->endTransmission();
    this->myWire->requestFrom( I2C_ADDR, 1 );
    data[i] = this->myWire->read();
    i++;
    
    // レジスタから補正値を取得(0xE1番地から0xE7番地)
    this->myWire->beginTransmission( I2C_ADDR );
    this->myWire->write( REG_ADDR_CTRLCORR3 );
    this->myWire->endTransmission();
    this->myWire->requestFrom( I2C_ADDR, 7 );
    while( this->myWire->available()){
        data[i] = this->myWire->read();
        i++;    
    }

    // レジスタから取得したデータを補正値として記憶する
    // 気温算出用補正値(T1/T2/T3)
    this->dig_T1 = (data[1]  << 8) | data[0];
    this->dig_T2 = (data[3]  << 8) | data[2];
    this->dig_T3 = (data[5]  << 8) | data[4];

    // 気圧算出用補正値(P1/P2/P3/P4/P5/P6/P7/P8/P9)
    this->dig_P1 = (data[7]  << 8) | data[6];
    this->dig_P2 = (data[9]  << 8) | data[8];
    this->dig_P3 = (data[11] << 8) | data[10];
    this->dig_P4 = (data[13] << 8) | data[12];
    this->dig_P5 = (data[15] << 8) | data[14];
    this->dig_P6 = (data[17] << 8) | data[16];
    this->dig_P7 = (data[19] << 8) | data[18];
    this->dig_P8 = (data[21] << 8) | data[20];
    this->dig_P9 = (data[23] << 8) | data[22];

    // 湿度算出用補正値(H1/H2/H3/H4/H5/H6)
    this->dig_H1 = data[24];
    this->dig_H2 = (data[26] << 8) | data[25];
    this->dig_H3 = data[27];
    this->dig_H4 = (data[28] << 4) | (0x0F & data[29]);
    this->dig_H5 = (data[30] << 4) | ((data[29] >> 4) & 0x0F);
    this->dig_H6 = data[31];   

    return;
}

//
// Method   :   writeRegister
// Abstruct :   レジスタ指定アドレスへの書きこみ
// Argument :   uint8_t reg_address : [I]レジスタへ書き込むアドレス
//          :   uint8_t data        : [I]書きこむデータ
// Return   :   n/a
void EnviroSensor::writeRegister( uint8_t reg_address, uint8_t data ) {
    // レジスタへの書き込み
    this->myWire->beginTransmission( I2C_ADDR );
    this->myWire->write( reg_address );
    this->myWire->write( data );
    this->myWire->endTransmission();   

    return;
}

//
// Method   :   getObservations
// Abstruct :   観測値をレジスタから読み込む
// Argument :   unsigned long int* temp_raw : [O]補正前観測値(気温)
//          :   unsigned long int* pres_raw : [O]補正前観測値(気圧)
//          :   unsigned long int* hum_raw  : [O]補正前観測値(湿度)
// Return   :   n/a
void EnviroSensor::getObservations( unsigned long int* temp_raw, unsigned long int* pres_raw, unsigned long int* hum_raw ) {
    uint32_t data[8] = { 0 };   // 読み出しデータ用テンポラリ
    char     i       = 0;       // カウンタ

    // レジスタから観測値を取得(0xF7番地から8byte分)
    this->myWire->beginTransmission( I2C_ADDR );
    this->myWire->write( REG_ADDR_OBSERV );
    this->myWire->endTransmission();
    this->myWire->requestFrom( I2C_ADDR, 8 );
    while( this->myWire->available() ) {
        data[i] = this->myWire->read();
        i++;
    }

    // レジスタから読み出したデータを補正前観測値に変換
    *pres_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    *temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    *hum_raw  = (data[6] <<  8) | (data[7]);

    return;
}

//
// Method   :   correctTemperature
// Abstruct :   気温観測データを補正する
// Argument :   signed long int adc_T   : [I]補正前データ(気温)
// Return   :   signed long int
//              補正後の観測データ(気温)
//              ※整数値なので実値の100倍になっていることに注意
signed long int EnviroSensor::correctTemperature( signed long int adc_T ) {
    signed long int var1 = 0L;  // 途中項1
    signed long int var2 = 0L;  // 途中項2
    signed long int T    = 0L;  // 補正後の観測データ

    // 補正後観測データの算出
    var1 = ((((adc_T >> 3) - ((signed long int)dig_T1<<1))) * ((signed long int)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((signed long int)dig_T1)) * ((adc_T>>4) - ((signed long int)dig_T1))) >> 12) * ((signed long int)dig_T3)) >> 14;
    this->t_fine = var1 + var2;
    T = (this->t_fine * 5 + 128) >> 8;

    return T;
}

//
// Method   :   correctPressure
// Abstruct :   気圧観測データを補正する
// Argument :   signed long int adc_P   : [I]補正前データ(気圧)
// Return   :   unsigned long int
//              補正後の観測データ(気圧)
//              ※整数値なので実値の100倍になっていることに注意
unsigned long int EnviroSensor::correctPressure( signed long int adc_P ) {
    signed long int var1 = 0L;
    signed long int var2 = 0L;
    unsigned long int P  = 0UL;

    // 補正後観測データの算出
    var1 = (((signed long int)this->t_fine)>>1) - (signed long int)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11) * ((signed long int)dig_P6);
    var2 = var2 + ((var1*((signed long int)dig_P5))<<1);
    var2 = (var2>>2)+(((signed long int)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((signed long int)dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((signed long int)dig_P1))>>15);
    if (var1 == 0)
    {
        return 0;
    }    
    P = (((unsigned long int)(((signed long int)1048576)-adc_P)-(var2>>12)))*3125;
    if(P<0x80000000)
    {
       P = (P << 1) / ((unsigned long int) var1);   
    }
    else
    {
        P = (P / (unsigned long int)var1) * 2;    
    }
    var1 = (((signed long int)dig_P9) * ((signed long int)(((P>>3) * (P>>3))>>13)))>>12;
    var2 = (((signed long int)(P>>2)) * ((signed long int)dig_P8))>>13;
    P = (unsigned long int)((signed long int)P + ((var1 + var2 + dig_P7) >> 4));

    return P;
}

//
// Method   :   correctHumidity
// Abstruct :   湿度観測データを補正する
// Argument :   signed long int adc_P   : [I]補正前データ(湿度)
// Return   :   unsigned long int
//              補正後の観測データ(湿度)
//              ※整数値なので実値の1024倍になっていることに注意
unsigned long int EnviroSensor::correctHumidity( signed long int adc_H ) {
    signed long int v_x1;
    
    v_x1 = (this->t_fine - ((signed long int)76800));
    v_x1 = (((((adc_H << 14) -(((signed long int)dig_H4) << 20) - (((signed long int)dig_H5) * v_x1)) + 
              ((signed long int)16384)) >> 15) * (((((((v_x1 * ((signed long int)dig_H6)) >> 10) * 
              (((v_x1 * ((signed long int)dig_H3)) >> 11) + ((signed long int) 32768))) >> 10) + (( signed long int)2097152)) * 
              ((signed long int) dig_H2) + 8192) >> 14));
    v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((signed long int)dig_H1)) >> 4));
    v_x1 = (v_x1 < 0 ? 0 : v_x1);
    v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);

    return (unsigned long int)(v_x1 >> 12);
}

//
// Method   :   performObservations
// Abstruct :   観測値を取得して補正値を返す
// Argument :   double* temp_act    : [O]補正後の気温
//          :   double* press_act   : [O]補正後の気圧
//          :   double* hum_act     : [O]補正後の湿度
// Return   :   n/a
void EnviroSensor::performObservations( double* temp_act, double* press_act, double* hum_act ) {
    // レジスタからの読み出し値
    unsigned long int temp_raw  = 0UL;
    unsigned long int pres_raw  = 0UL;
    unsigned long int hum_raw   = 0UL;

    // レジスタから観測値を取得
    getObservations( &temp_raw, &pres_raw, &hum_raw );
    
    // 取得した観測値を補正
    signed long int   temp_cal  = correctTemperature( temp_raw );
    unsigned long int press_cal = correctPressure( pres_raw );
    unsigned long int hum_cal   = correctHumidity( hum_raw );

    // 補正値から気温/気圧/湿度を算出して返却する
    *temp_act                   = (double)temp_cal  / 100.0;
    *press_act                  = (double)press_cal / 100.0;
    *hum_act                    = (double)hum_cal   / 1024.0;

    return;
}
}
