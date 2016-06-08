/*
 * PARADE.cpp
 *
 *  Created on: 2016/05/24
 *      Author: aihara
 */

#include <PARADE.h>
#include <cmath>
#include <cfloat>
#include <iostream>

//#include <android/log.h>
//#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, "vaddsp-jni", __VA_ARGS__))

float calc_nullhypotes(float pp, float pa, float alpha){

	return (1.0 / (std::sqrt(M_PI * 2.0) * alpha * pa)) *
			std::exp( (-1.0 / (2 * std::pow(alpha, 2))) * std::pow((pp / pa), 2));
}

float calc_hypotes(float pp, float pa, float beta){

	return (1.0 / (std::sqrt(M_PI * 2.0) * beta * pp)) *
			std::exp( (-1.0 / (2 * std::pow(beta, 2))) * std::pow((pa / pp), 2));
}

PARADE::PARADE(int windowsize, int asize, float *window) {
	// TODO Auto-generated constructor stub
	analysissize = asize;
	winsize = windowsize;
	fftsize = analysissize / 2 + 1;
	last_score = 1.0;
	double a = 0.0;
	double b = 0.0;
	for (int i=0; i<winsize; i++){
		a += std::pow(window[i], 2);
		b += window[i];
	}
	eta = 2.0 * (a / std::pow(b, 2));
}

PARADE::~PARADE() {
	// TODO Auto-generated destructor stub
}


float PARADE::process(float *power, float avg_pow){
	double smax = -FLT_MAX;
	int lenmax = 0;
	int c = 0;
	float s = 0.0;
	for(int i = 5; i < 300; i++){
		// searching f0 index in freqbin with maximizing estimated power of periodic component
		c = 0;
		for (int j = i; j < fftsize; j += i + 1){
			s += power[j];
			c++;
		}
		s = s - avg_pow * c;
		//s = s / c;
		if (s > smax){
			smax = s;
			lenmax = c;
		}
	}
	float pp = (smax / (1.0 - eta * lenmax)) * eta;
	if (pp < 0){
		pp = 0.001;
	}
	float pa = avg_pow - pp;

	if (pa < 0){
		pa = 0.001;
	}

	//float ll = std::log(calc_hypotes(pa, pp, 1.0)) - std::log(calc_nullhypotes(pa, pp, 1.0));
	//LOGE("pp:%f, pa:%f, ratio:%f, ll:%f", pp, pa, pp/pa, ll);
	float score = pp/pa;
	float ret = last_score * 0.5 + score * 0.5;
	last_score = score;
	//LOGE("pp:%f, pa:%f, ratio:%f, ret:%f", pp, pa, pp/pa, ret);
	return ret;
}
