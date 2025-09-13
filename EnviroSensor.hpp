#ifndef ENVIRO_SENSOR_H
#define ENVIRO_SENSOR_H
//
// Filename :   EnviroSensor.hpp
// Abstruct :   Class definition for Enviro Sensor
// Author   :   application_division@atit.jp
// Update   :   2025/09/13  New Creation
#include <Wire.h>

namespace AMAGOI {
//
// Class    :   EnviroSensor
// Abstruct :   Class definition for Enviro Sensor
class EnviroSensor {
    // Definition of constant
private:
    enum registerAddress {
        REG_ADDR_CTRLMEAS           = 0xF4, // コンフィグ設定(1)先頭アドレス
        REG_ADDR_CONFIG             = 0xF5, // コンフィグ設定(2)先頭アドレス
        REG_ADDR_CTRLHUM            = 0xF2, // コンフィグ設定(3)先頭アドレス
        REG_ADDR_CTRLCORR1          = 0x88, // 補正データ(1)先頭アドレス
        REG_ADDR_CTRLCORR2          = 0xA1, // 補正データ(2)先頭アドレス
        REG_ADDR_CTRLCORR3          = 0xE1, // 補正データ(3)先頭アドレス
        REG_ADDR_OBSERV             = 0xF7  // 観測データ先頭アドレス
    };
    const int       I2C_ADDR        = 0x76; // I2Cアドレス(規定値)
    const uint8_t   OVER_SAMPLING   = 0x01; // オーバーサンプリング:×1(規定値)
    const uint8_t   MODE            = 0x03; // モード:ノーマル(規定値)
    const uint8_t   SPI3W           = 0x00; // 3線式SPI:未使用(固定値)
    const uint8_t   FILTER          = 0x00; // フィルタ:OFF(規定値)
    const uint8_t   T_STANDBY       = 0x05; // スタンバイ時間:1000(ms)
public:
    // Definition of variable
private:
    TwoWire*        myWire;                 // I2C通信クラスインスタンスへの参照
    int             i2cAddr;                // I2Cアドレス
    double          measureTemp;            // 観測値(気温)
    double          measurePress;           // 観測値(気圧)
    double          measureHum;             // 観測値(湿度)
    signed long int t_fine;                 // 補正用気温

    uint16_t        dig_T1;                 // 補正パラメータ1(気温)
    int16_t         dig_T2;                 // 補正パラメータ2(気温)
    int16_t         dig_T3;                 // 補正パラメータ3(気温)
    uint16_t        dig_P1;                 // 補正パラメータ1(気圧)
    int16_t         dig_P2;                 // 補正パラメータ2(気圧)
    int16_t         dig_P3;                 // 補正パラメータ3(気圧)
    int16_t         dig_P4;                 // 補正パラメータ4(気圧)
    int16_t         dig_P5;                 // 補正パラメータ5(気圧)
    int16_t         dig_P6;                 // 補正パラメータ6(気圧)
    int16_t         dig_P7;                 // 補正パラメータ7(気圧)
    int16_t         dig_P8;                 // 補正パラメータ8(気圧)
    int16_t         dig_P9;                 // 補正パラメータ9(気圧)
    uint8_t         dig_H1;                 // 補正パラメータ1(湿度)
    int16_t         dig_H2;                 // 補正パラメータ2(湿度)
    uint8_t         dig_H3;                 // 補正パラメータ3(湿度)
    int16_t         dig_H4;                 // 補正パラメータ4(湿度)
    int16_t         dig_H5;                 // 補正パラメータ5(湿度)
    int8_t          dig_H6;                 // 補正パラメータ6(湿度)
    // Definition of method
private:
    void readCorrectionValue();
    void writeRegister( uint8_t, uint8_t );
    void getObservations( unsigned long int*, unsigned long int*, unsigned long int* );
    signed long int correctTemperature( signed long int );
    unsigned long int correctPressure( signed long int );
    unsigned long int correctHumidity( signed long int );
public:
    EnviroSensor( TwoWire* );
    void performObservations( double*, double*, double* );
};
}
#endif
