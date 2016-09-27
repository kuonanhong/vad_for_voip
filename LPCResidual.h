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
  float process(const float* __restrict signal);
  bool hasError();
  
 private:

  void calcResiduals(const float* __restrict windowed);
  float calcKurtosis();

  int winsize;
  int ressize;
  float last_k;
  bool error;
  unsigned int order;
  float* __restrict R;
  float* __restrict K;
  float* __restrict A;
  float* __restrict Residuals;
};

#endif /* LPCRESIDUAL_H_ */
