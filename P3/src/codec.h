/*
 * codec.h
 *
 *  Created on: May 10, 2014
 *      Author: aarcane
 */

#ifndef CODEC_H_
#define CODEC_H_
#include <ostream>
#include <istream>
#include <map>
#include <vector>


class codec {
public:
	codec(std::istream &in, std::ostream &out);
	virtual ~codec() {}
	virtual void compress(void) = 0;
	virtual void decompress(void) = 0;
	virtual unsigned char magic(void) = 0;

protected:
	std::istream &i;
	std::ostream &o;
};

/* Codec shared utility functions */

std::vector<bool>& operator<< (std::vector<bool>& o, const bool& v);
std::vector<bool>& operator<< (std::vector<bool>& o, const unsigned char& vs);
std::vector<bool>& operator<< (std::vector<bool>& o, const char& vs);
std::vector<bool>& operator<< (std::vector<bool>& o, std::vector<bool>& v);
std::vector<bool>& operator<< (std::vector<bool>& o, std::vector<unsigned char>& cs);

std::vector<unsigned char>& operator<< (std::vector<unsigned char>& o, const unsigned char& vs);
std::vector<unsigned char>& operator<< (std::vector<unsigned char>& o, std::vector<unsigned char>& cs);

std::ostream&  operator<<(std::ostream& o, const std::vector<bool>& v);
std::ostream&  operator<<(std::ostream& o, const std::vector<unsigned char>& v);
std::ostream&  operator<<(std::ostream& o, const std::vector<char>& v);
size_t printVectorBool(std::ostream& o, std::vector<bool>&v, size_t threshold = (8192*8));
size_t readVectorBool(std::istream& i, std::vector<bool>&v, size_t count);

static const unsigned char bits[] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};

#endif /* CODEC_H_ */
