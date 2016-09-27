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
  PARADE(int winsize, int asize, const float *window);
  virtual ~PARADE();
  double process(const float *power, float avg_pow);
  bool hasError();

 private:
  double eta;
  bool error;
  int analysissize;
  int winsize;
  int fftsize;
  double last_score;
};

#endif /* PARADE_H_ */
