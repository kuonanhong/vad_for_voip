/*
 * LTSD.h

 *
 *  Created on: 2015/11/18
 *      Author: Shunsuke Aihara (aihara@argmax.jp)
 */

#ifndef LTSD_H_
#define LTSD_H_

#include <vector>
#include <deque>
#include "string.h"
#include <cmath>
#include "math.h"
#include "utils.h"
//#include "ffft/FFTReal.h"
#include "ckfft/ckfft.h"
//#include "MinimumStatistics.h"
#include "MmseBasedNpe.h"
#include "PARADE.h"
#include "LPCResidual.h"

class LTSD {
public:
	// windowsizeは2の冪乗(256以上)で必ず偶数サイズにすること。
	// この値は、short型で換算した場合のサンプル数(つまりbyte配列の長さ/2)。
	// orderは奇数(5, 7, 11)が望ましい
	// sampling rateは、録音時のサンプリングレートを用いる
	LTSD(int winsize, int samprate, int order = 7,double e0 = 40.0, double e1 = 80.0, double lambda0 = 20.0, double lambda1 = 10.0);
	virtual ~LTSD();
	bool process(char* __restrict input);
	char* getSignal(); // 取得したsignalは必ず利用後deleteすること。byteで返しているが、アーキテクチャのエンディアンでのshort型の配列になっている
    void updateParams(double e0, double e1, double lambda0, double lambda1);
	int fftErrors();
private:
	void createWindow();
	void calcLTSE();
	float calcLTSD();
	bool isSignal();
	void createNoiseProfile();
	float calcPower();
	float calcNoisePower();
	void updateVadHistories(bool decision);
	bool vadDecision();
	void initFFT();
	int windowsize;
	int analysissize;
	int fftsize;
	int freqsize;
	int samplingrate;
	int m_order;
	int vad_history_size;
	float m_e0;
	float m_e1;
	float m_lambda0;
	float m_lambda1;
	float* __restrict window;
	float* __restrict ltse;
	float* __restrict noise_profile;
	bool* vad_histories;
	float* __restrict power_spectrum;
	float avg_pow;
	bool estimated;

	MmseBasedNpe *mmse;
	PARADE *parade;
	LPCResidual *lpcr;
	float* __restrict fft_in;
	std::deque<float*> amp_history;
	std::deque<short*> signal_history;

    CkFftContext* context;
    CkFftComplex* forwardOutput;

	int fft_errors;
};

#endif /* LTSD_H_ */
