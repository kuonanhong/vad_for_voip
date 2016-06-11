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
#include <cmath>

class MmseBasedNpe {
 public:
  MmseBasedNpe(int size, float* __restrict noiseProfile);
  virtual ~MmseBasedNpe();
  void process(float* __restrict amp);
  void updateNoiseProfile(float* __restrict noise);
 private:
  int fftsize;
  float* __restrict PH1mean;
  float alphaPH1mean;
  float alphaPSD;
  float q;
  float priorFact;
  float xiOptDb;
  float xiOpt;
  float logGLRFact;
  float GLRexp;

  float* __restrict noisePow;
  float* __restrict noisyPer;
  float* __restrict snrPost1;
  float* __restrict estimate;
  float* __restrict GLR;
  float* __restrict PH1;

};

#endif /* MMSEBASEDNPE_H_ */
