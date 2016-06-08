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
	PARADE(int winsize, int asize, float *window);
	virtual ~PARADE();
	float process(float *power, float avg_pow);

private:
	float eta;
	int analysissize;
	int winsize;
	int fftsize;
	float last_score;
};

#endif /* PARADE_H_ */
