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


class codec {
public:
	codec(std::istream &in, std::ostream &out);
	virtual ~codec() {}
	virtual void compress(void) = 0;
	virtual void decompress(void) = 0;
	virtual unsigned char magic(void) = 0;
	//static std::map<unsigned char, std::string> magic_map;
	//static std::map<std::string, codec*> name_map;

protected:
	std::istream &i;
	std::ostream &o;
};



#endif /* CODEC_H_ */
