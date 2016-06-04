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

float calc_nullhypotes(float pp, float pa, float alpha){

	return (1.0 / (std::sqrt(M_PI * 2.0) * alpha * pa)) *
			std::exp( (-1.0 / (2 * std::pow(alpha, 2))) * std::pow((pp / pa), 2));
}

float calc_hypotes(float pp, float pa, float beta){

	return (1.0 / (std::sqrt(M_PI * 2.0) * beta * pp)) *
			std::exp( (-1.0 / (2 * std::pow(beta, 2))) * std::pow((pa / pp), 2));
}

PARADE::PARADE(int windowsize, float *window) {
	// TODO Auto-generated constructor stub
	winsize = windowsize;
	fftsize = winsize / 2;
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
	for(int i = 3; i < 40; i++){
		// searching f0 index in freqbin with maximizing estimated power of periodic component
		c = 0;
		for (int j = 0; j < fftsize; j += i + 1){
			s += power[j];
			c++;
		}
		s = s - avg_pow * c;
		if (s > smax){
			smax = s;
			lenmax = c;
		}
	}
	float pp = (smax / (1.0 - eta * lenmax)) * eta;
	float pa = avg_pow - pp;
	float ll = calc_hypotes(pa, pp, 1.0) / calc_nullhypotes(pa, pp, 1.0);

	return std::log(ll + 0.00001) - std::log(0.00001);
}
