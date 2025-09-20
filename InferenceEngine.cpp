//
// Filename :   InferenceEngine.cpp
// Abstruct :   Method for InferenceEngine class
// Author   :   application_division@atit.jp
// Update   :   2025/09/20	New Creation
#include <cstdlib>
#include <ctime>
#include "InferenceEngine.hpp"

using namespace AMAGOI;
//
// Method   :   InferenceEngine
// Abstruct :   コンストラクタ
// Argument :   double Q : システムノイズ
//			:	double R : 観測ノイズ
InferenceEngine::InferenceEngine( double Q, double R ) 
	: G( 0.0)
	, P( 1.0 )
	, Q( Q )
	, R( R )
	, estVal{ 0.0 }
	, observCnt( 0 )
	, estValCnt( 0 )
	, inclination( 0.0 )
	, inferredValue( 0.0 )
{
	srand((unsigned int)time( NULL ));
}

//
// Method   :   updateObservationsobservCnt
// Abstruct :   観測値を取り込んでフィルタステップを進める
// Argument :   double x : [I]観測値
// Return   :   bool
//              推定値算出を実施した場合 true
bool InferenceEngine::updateObservations( double x ) {
	static double xhat = 0.0;	// 推定値
	bool isEstimation  = false;	// 返却値

	// y初期値設定
	double y = pow( x, 3.0 );

	// xhat初期値設定(初回のみ)
	if( this->observCnt == 0 && this->estValCnt == 0 ) {
		xhat = x + 1.0;
	}

	// フィルタ更新実行
	calcPredictedValue( &xhat, y, &(this->G), &(this->P) );
	this->observCnt++;

	if( this->observCnt == 1 && this->estValCnt == 0 ) {
		// 観測値を記憶(初回)
		this->estValCnt++;
		this->estVal[this->estValCnt - 1] = x;
		isEstimation = false;
	} else if( this->observCnt > EST_REC_CNT ) {
		// 観測値を記憶
		if( this->estValCnt == OBS_REC_CNT_MAX ) {
			// 記憶域がいっぱいの場合前にずらす
			arraySlide( this->estVal );
		}
		if( this->estValCnt < OBS_REC_CNT_MAX ) {
			this->estValCnt++;
		}
		this->estVal[this->estValCnt - 1] = x;
		
		// 推定値を算出
		calcInferredValue( xhat, this->estValCnt );
		
		// 最小二乗法にて傾きを算出
		updatePrediction( this->estVal, this->estValCnt + EST_REC_CNT_MAX );
		
		// 推論フラグon
		isEstimation = true;
		this->observCnt = 0;
	} else {
		isEstimation = false;
	}

	return isEstimation;
}

//
// Method   :   calcInferredValue
// Abstruct :   規定時間経過後の状態推定値を算出する
// Argument :   double xhat  : [I]推定値初期値
//   		:	int dataHead : [I]記録初期位置
// Return   :   n/a
void InferenceEngine::calcInferredValue( double xhat, int dataHead ) {
	// 予測用のゲイン・誤差共分散・疑似観測値
	double Ghat = this->G;
	double Phat = this->P;
	double x    = xhat;

	for( int i = 1; i <= EST_CALC_CNT; i++ ) {
		double yhat = pow( addNoise2Observ( x ), 3.0 );
		calcPredictedValue( &xhat, yhat, &Ghat, &Phat );
		if( i % 60 == 0 ) {
			// 記憶域に格納
			this->estVal[dataHead] = xhat;
			dataHead++;
		}
	}
	this-> inferredValue = xhat;

	return;
}

//
// Method   :   calcPredictedValue
// Abstruct :   推定ステップを進める
// Argument :   double xhat : [IO]推定値
//			:	double y    : [I]
// 			:	double *G	: [IO]カルマンゲイン
//			: 	double *P	: [IO]誤差共分散
// Return   :   n/a
void InferenceEngine::calcPredictedValue( double *xhat, double y, double *G, double *P ) {
	// 事前推定値の算出
    double xhatM = *xhat + 3.0 * cos( *xhat / 10.0 );
    double PM = ( 1.0 - 3.0 / 10.0 * sin( *xhat / 10 )) * (*P) * ( 1.0 - 3.0 / 10.0 * sin( *xhat / 10 )) + ( 1 ) * this->Q * ( 1 );

	// カルマンゲインの更新
    *G = PM * ( 3.0 * pow( xhatM, 2.0 ))  / (( 3.0 * pow( xhatM, 2.0 )) * PM * ( 3.0 * pow( xhatM, 2.0 ) ) + this->R);

	// 事後推定値の算出
	*xhat = xhatM + (*G) * ( y - pow( xhatM, 3.0 ));
    *P = ( 1.0 ) - (*G) * ( 3.0 * pow( xhatM, 2.0 )) * PM;

	return;
}

//
// Method   :   updatePrediction
// Abstruct :   最小二乗法を用いて傾きを算出する
// Argument :   double y   : [I]観測及び推定値の記録配列
//			:	int    cnt : [I]データ個数
// Return   :   n/a
void InferenceEngine::updatePrediction( double* y, int cnt ) {
    double sum_xx = 0.0;
    double sum_xy = 0.0;
    double sum_x  = 0.0;
    double sum_y  = 0.0;
    
	// 傾きを算出
	// note : 横軸は時間(秒単位)
	for( int i = 0; i < cnt; i++) {
		sum_xx += ( ((double)OBS_INTERVAL/1000.0) * (double)i ) * ( ((double)OBS_INTERVAL/1000.0) * (double)i );
		sum_xy += ( ((double)OBS_INTERVAL/1000.0) * (double)i ) * y[i];
		sum_x  += ( ((double)OBS_INTERVAL/1000.0) * (double)i );
		sum_y  += y[i];
    }
    this->inclination = ( cnt * sum_xy - sum_x * sum_y ) / ( cnt * sum_xx - sum_x * sum_x );

	return;
}

//
// Method   :   arraySlide
// Abstruct :   配列データをずらす
// Argument :   double* array : [I]処理対象配列
// Return   :   n/a
void InferenceEngine::arraySlide( double* array ) {
	array[0] = array[1];
	for( int i = 1; i <= ( OBS_REC_CNT_MAX - 1 ) - 1; i++ ) {
		array[i] = array[i + 1];
	}
	array[12] = 0.0;
	return;
}

//
// Method   :   addNoise2Observ
// Abstruct :   観測値にランダムな観測ノイズを与える
// Argument :   double* x : [IO]ノイズを与える観測値
// Return   :   n/a
double InferenceEngine::addNoise2Observ( double x ) {
	double rd = ((double)((int)rand() % 100 - 50) / 50.0) * this->R; 
	return x += rd;
}

//
// Method   :   getInferredValue
// Abstruct :   ゲッタ(推定値)
// Argument :   n/a
// Return   :   double
//				メンバ inferredValue の値
double InferenceEngine::getInferredValue() {
	return this->inferredValue;
}

//
// Method   :   getInclination
// Abstruct :   ゲッタ(傾き)
// Argument :   n/a
// Return   :   double
//				メンバ inclination の値
double InferenceEngine::getInclination() {
	return this->inclination;
}