/*
 * MmseBasedNpe.h
 *
 *  Created on: 2015/11/27
 *      Author: aihara
 */

#ifndef MMSEBASEDNPE_H_
#define MMSEBASEDNPE_H_

#include "utils.h"
#include "string.h"
#include "math.h"

class MmseBasedNpe {
public:
	MmseBasedNpe(int size, float *noiseProfile);
	virtual ~MmseBasedNpe();
	void process(float *amp);
	void updateNoiseProfile(float *noise);
private:
	int fftsize;
	float* PH1mean;
	float alphaPH1mean;
	float alphaPSD;
	float q;
	float priorFact;
	float xiOptDb;
	float xiOpt;
	float logGLRFact;
	float GLRexp;

	float* noisePow;
	float* noisyPer;
	float* snrPost1;
	float* estimate;
	float* GLR;
	float* PH1;

};

#endif /* MMSEBASEDNPE_H_ */
