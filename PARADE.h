/*
 * PARADE.h
 *
 *  Created on: 2016/05/24
 *      Author: aihara
 */

#ifndef PARADE_H_
#define PARADE_H_

class PARADE {
public:
	PARADE(int winsize, float *window);
	virtual ~PARADE();
	float process(float *power, float avg_pow);

private:
	float eta;
	int winsize;
	int fftsize;
};

#endif /* PARADE_H_ */
