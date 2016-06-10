/*
 * Mmsebasednpe.cpp
 *
 *  Created on: 2015/11/27
 *      Author: aihara
 */

#include <MmseBasedNpe.h>

#include "LTSD.h"


MmseBasedNpe::MmseBasedNpe(int size, float* __restrict noiseProfile) {
	// TODO Auto-generated constructor stub
	fftsize = size;
	alphaPH1mean = 0.9;
	alphaPSD = 0.8;

	q = 0.5; // a priori probability of speech presence:
	priorFact = q / (1 - q);
	xiOptDb = 15.0; // optimal fixed a priori SNR for SPP estimation
	xiOpt = powf(10.0, (xiOptDb / 10.0));
	logGLRFact = logf(1.0 / (1.0 + xiOpt));
	GLRexp = xiOpt / (1.0 + xiOpt);

	PH1mean = makeVector(fftsize, (float)0.5);
	noisePow = new float[fftsize];
	memcpy(noisePow, noiseProfile, sizeof(float) * fftsize);
	noisyPer = new float[fftsize];
	snrPost1 = new float[fftsize];
	estimate = new float[fftsize];
	GLR = new float[fftsize];
	PH1 = new float[fftsize];
}

MmseBasedNpe::~MmseBasedNpe() {
	// TODO Auto-generated destructor stub
	delete[] PH1mean;
	delete[] noisePow;
	delete[] noisyPer;
	delete[] snrPost1;
	delete[] estimate;
	delete[] GLR;
	delete[] PH1;
}


void MmseBasedNpe::process(float* __restrict amp) {
	int i = 0;
	float tmp;
	for(i = 0; i< fftsize; i++) {
        noisyPer[i] = amp[i] * amp[i];
        snrPost1[i] = noisyPer[i] / noisePow[i];

        tmp = logGLRFact + GLRexp * snrPost1[i];
        if (tmp > 200.0) {
            tmp = 200.0;
        }
        GLR[i] = priorFact * expf(tmp);
        PH1[i] = GLR[i] / (1.0 + GLR[i]);
        PH1mean[i] = alphaPH1mean * PH1mean[i] + (1.0 - alphaPH1mean) * PH1[i];
        if (PH1mean[i] > 0.99) {
            if (PH1[i] > 0.99) {
                PH1[i] = 0.99;
            }
        }
        estimate[i] = PH1[i] * noisePow[i] + (1.0 - alphaPSD) * noisyPer[i];
        noisePow[i] = alphaPSD * noisePow[i] + (1.0 - alphaPSD) * estimate[i];
        if (std::isnan(noisePow[i])) {
            noisePow[i] = 0.0000001;
        }
	}
}

void MmseBasedNpe::updateNoiseProfile(float* __restrict noise){
	memcpy(noise, noisePow, sizeof(float) * fftsize);
}

