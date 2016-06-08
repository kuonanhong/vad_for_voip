/*
 * LPCResidual.h
 *
 *  Created on: 2016/06/08
 *      Author: aihara
 */

#ifndef LPCRESIDUAL_H_
#define LPCRESIDUAL_H_

class LPCResidual {
public:
	LPCResidual(int windowsize, int lpcorder);
	virtual ~LPCResidual();

	float process(float *signal);
private:

	void calcResiduals(float *windowed);
	float calcKurtosis();

	int winsize;
	int ressize;
	float last_k;
	unsigned int order;
	float *R;
	float *K;
	float *A;
	float *Residuals;
};

#endif /* LPCRESIDUAL_H_ */
