/*
 * circularCounter.h
 *
 *  Created on: May 29, 2014
 *      Author: aarcane
 */

#ifndef CIRCULARCOUNTER_H_
#define CIRCULARCOUNTER_H_

template <class T>
class circularCounter
{private:
	T i;
	T j;
	T offset;
	T mod;
public:
	circularCounter(T end, T start = 0, T step = 1): i(0), j(step), offset(start), mod(end-start) {};
	void operator++() {i = (i+j)%mod;};
	void operator++(int){i = (i+j)%mod;};
	T operator*(){return i+offset;};
	T operator()(void){return i+offset;};
};





#endif /* CIRCULARCOUNTER_H_ */
