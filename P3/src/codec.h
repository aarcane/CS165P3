/*
 * codec.h
 *
 *  Created on: May 10, 2014
 *      Author: aarcane
 */

#ifndef CODEC_H_
#define CODEC_H_
#include <fstream>
#include <ostream>

class codec {
public:
	codec(std::istream &in, std::ostream &out);
	virtual void compress(void) = 0;
	virtual void decompress(void) = 0;
protected:
	std::istream &i;
	std::ostream &o;
};

#endif /* CODEC_H_ */
