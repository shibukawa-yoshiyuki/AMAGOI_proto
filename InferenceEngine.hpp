#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H
//
// Filename :   InferenceEngine.hpp
// Abstruct :   Class definition for inference engine
// Author   :   application_division@atit.jp
// Update   :   2025/09/20	New Creation
#include <math.h>

namespace AMAGOI {
//
// Class    :   InferenceEngine
// Abstruct :   Class definition for inference engine
class InferenceEngine {
    // Definition of constant
private:
    const uint32_t ONEHOUR_MSEC    = ( 60 * 60 * 1000 );	                // 1時間をミリ秒に換算
    const uint32_t OBS_INTERVAL    = ( 5 * 1000 );                          // 計測処理実行間隔
    const uint32_t EST_INTERVAL    = ( 5 * 60 * 1000 );                     // 推定処理実行間隔
    const uint16_t EST_CALC_CNT    = ( ONEHOUR_MSEC / OBS_INTERVAL);        // 推定時フィルタ更新実行回数
    const uint8_t  OBS_REC_CNT_MAX = ( ONEHOUR_MSEC / EST_INTERVAL + 1);    // 観測値記録最大数
    const uint8_t  EST_REC_CNT_MAX = ( ONEHOUR_MSEC / EST_INTERVAL );       // 推定値記録最大数
    const uint8_t  EST_REC_CNT     = ( EST_CALC_CNT / EST_REC_CNT_MAX );    // 推定時記録実行間隔
    const uint8_t  EST_ARRAY_MAX   = ( OBS_REC_CNT_MAX + EST_REC_CNT_MAX ); // 記録配列データ長
	// Definition of variable
private:
    double G;               // カルマンゲイン
    double P;               // 誤差共分散
    double Q;               // システムノイズ
    double R;               // 観測ノイズ
    double estVal[25];      // 観測値および推定値記憶域
    int    observCnt;       // 観測回数カウンタ
    int    estValCnt;       // 観測値データ数
    double inferredValue;   // 最新の推定値
    double inclination;     // 傾き
private:
    // Definition of method
private:
    void calcInferredValue( double, int );
    void calcPredictedValue( double*, double, double*, double* );
    void updatePrediction( double*, int );
    void arraySlide( double* );
    double addNoise2Observ( double );
public:
    InferenceEngine( double, double );
    bool updateObservations( double );
    double getInferredValue();
    double getInclination();
};
}
#endif // #ifndef INFERENCE_ENGINE_H
