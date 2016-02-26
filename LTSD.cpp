/*
 * LTSD.cpp
 *
 *  Created on: 2015/11/18
 *      Author: Shunsuke Aihara (aihara@argmax.jp)
 */

#include "LTSD.h"

//#include <android/log.h>
//#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, "vaddsp-jni", __VA_ARGS__))


LTSD::LTSD(int winsize, int samprate, int order, double e0, double e1, double lambda0, double lambda1){
	windowsize = winsize;
	fftsize = winsize / 2 + 1;
	freqsize = fftsize / 2.5;
	samplingrate = samprate;
	m_order = order;
	m_e0 = e0;
	m_e1 = e1;
	fft_errors = 0;
	m_lambda0 = lambda0;
	m_lambda1 = lambda1;
	fft_in = new float[windowsize];
	ltse = new float[fftsize];
	noise_profile = new float[fftsize];
	for(int i=0; i< fftsize; i++){
		noise_profile[i] = 0.0;
	}

	estimated = false;
	createWindow();
    context = CkFftInit(windowsize, kCkFftDirection_Both, NULL, NULL);
    forwardOutput = new CkFftComplex[windowsize/2 + 1];
    
    mmse = NULL;
}

LTSD::~LTSD() {
	for (std::deque<short*>::iterator its = signal_history.begin(); its != signal_history.end(); its++){
		delete[] (*its);
	}
	for (std::deque<float*>::iterator ita = amp_history.begin(); ita != amp_history.end(); ita++){
		delete[] (*ita);
	}
	if (window != NULL){
		delete[] window;
	}
	delete[] fft_in;
	delete[] ltse;
	delete[] noise_profile;
    CkFftShutdown(context);

	if (mmse != NULL){
		delete mmse;
	}
}

bool LTSD::process(char *input){
    short *signal = (short *)input;
	for(int i=0; i<windowsize; i++){
		fft_in[i]=(float(signal[i]) / 32767.0) * window[i];
	}

    CkFftRealForward(context, windowsize, fft_in, forwardOutput);
    
	float *amp = new float[fftsize];
	for(int i=0; i<fftsize; i++) {
      if (forwardOutput != NULL){
        amp[i] = sqrtf(powf(forwardOutput[i].real, 2.0) + powf(forwardOutput[i].imag, 2.0)) + 0.0000001;
		  if (std::isnan(amp[i]) || std::isinf(amp[i])){
			  amp[i] = 0.0000001;
			  fft_errors++;
		  }else if (amp[i] > 100){
			  fft_errors++;
		  };
	  }
	}

	short* sig = new short[windowsize];
	memcpy(sig, signal, sizeof(short) * windowsize);
	signal_history.push_back(sig);
	amp_history.push_back(amp);

	if (signal_history.size() > m_order){
		if(!estimated){
			createNoiseProfile();
			estimated = true;
			mmse = new MmseBasedNpe(fftsize, noise_profile);
		}
		if (mmse != NULL){
			mmse->process(amp);
			mmse->updateNoiseProfile(noise_profile);
		}
		//履歴長が指定以上なので、先頭を削除してltsd判定
		delete[] signal_history[0];
		signal_history.pop_front();
		delete[] amp_history[0];
		amp_history.pop_front();

		return isSignal();
	}else{
		return false;
	}
}

bool LTSD::isSignal(){
	calcLTSE();
	float ltsd = calcLTSD();
	float e = calcPower();
    float e2 = calcNoisePower();
	//float sn = fabs(e - e2);
    float lamb = (m_lambda0 - m_lambda1) / (m_e0 - m_e1) * e2 + m_lambda0 -
                  (m_lambda0 - m_lambda1) / (1.0 - (m_e1 / m_e0));

    //LOGE("signal: %f, noise: %f, ltsd: %f, lambda:%f, e0:%f", e, e2, ltsd, lamb, m_e0);
	//LOGE("e0: %f, e1: %f, lam0: %f, lam1:%f", m_e0, m_e1, m_lambda0, m_lambda1);

	if (e2 < m_e0){
		if(ltsd > m_lambda0){
			return true;
		}else{
			return false;
		}
	}else if (e2 > m_e1){
		if(ltsd > m_lambda1){
			return true;
		}else{
			return false;
		}
	}else {
        if (ltsd > lamb) {
            return true;
        } else {
            return false;
        }
    }
}

float LTSD::calcPower(){
	float* amp = amp_history.at(amp_history.size() - 1);
	float sum = 0.0;
	for(int i = 0; i < freqsize; i++){
		sum += amp[i] * amp[i];
	}
	return 10 * log10f((sum / float(freqsize)) / ((1.0e-5 * 2.0) * (1.0e-5 * 2.0)));
}

float LTSD::calcNoisePower(){
    float s = 0.0;
    for(int i = 0; i < freqsize; i++) {
        s += noise_profile[i];
    }
    return 10 * log10f((s / float(freqsize)) / ((1.0e-5 * 2.0) * (1.0e-5 * 2.0)));
}

char* LTSD::getSignal(){
	if (signal_history.size() != m_order){
		return NULL;
	}else{
		short* src = signal_history.at(signal_history.size() - 1);
		short* dest = new short[windowsize];
		memcpy(dest, src, sizeof(short) * windowsize);
		return (char *)dest;
	}
}

void LTSD::calcLTSE(){
	int i = 0;
	float amp;
	for(i=0;i < freqsize; i++){
		ltse[i] = 0.0;
	}
	for (std::deque<float*>::iterator ita = amp_history.begin(); ita != amp_history.end(); ita++){
		for(i=0;i < freqsize; i++){
			amp = (*ita)[i];
			if (ltse[i] < amp){
				ltse[i] = amp;
			}
		}
	}
	for(i=0;i < freqsize; i++){
		ltse[i] = ltse[i] * ltse[i];
	}
}

float LTSD::calcLTSD(){
	float sum = 0.0;
	for(int i = 0; i < freqsize; i++){
		sum += ltse[i] / noise_profile[i];
	}
	return 10 * log10(sum / freqsize);
}

void LTSD::createNoiseProfile(){
	int i = 0;
	float s = (float)amp_history.size();
	for (std::deque<float*>::iterator ita = amp_history.begin(); ita != amp_history.end(); ita++){
        float *x = (*ita);
		for(i=0;i < fftsize; i++){
			noise_profile[i] += x[i];
		}
	}
    float sum = 0.0;
	for(i=0;i < fftsize; i++){
        noise_profile[i] = powf(noise_profile[i] / s, 2);
	}
}

void LTSD::createWindow(){
	window = new float[windowsize];
	if (windowsize == 1){
		window[0] = 1.0;
	}else{
		float n = windowsize -1;
		float coef = M_PI * 2 / float(n);
		for (int i = 0; i < n; i++){
			window[i] = 0.54 - 0.46 * cos(coef * float(i));
		}
	}
}

void LTSD::updateParams(double e0, double e1, double lambda0, double lambda1){
 	m_e0 = e0;
	m_e1 = e1;
	m_lambda0 = lambda0;
	m_lambda1 = lambda1; 
}

int LTSD::fftErrors() {
	return fft_errors;
}

void LTSD::initFFT() {
	CkFftShutdown(context);
	context = CkFftInit(windowsize, kCkFftDirection_Both, NULL, NULL);
}