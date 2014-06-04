/*
 * codec.cpp
 *
 *  Created on: May 10, 2014
 *      Author: aarcane
 */

#include "codec.h"
#include "circularCounter.h"
#include <vector>
#include <stdexcept>

codec::codec(std::istream &in, std::ostream &out):
	i(in),
	o(out)
{
}

std::vector<bool>& operator<< (std::vector<bool>& o, const unsigned char& vs)
{	for(unsigned char c: bits) o.push_back((vs&c));
	return o;
}

std::vector<bool>& operator<< (std::vector<bool>& o, const char& vs)
{	for(unsigned char c: bits) o.push_back((vs&c));
	return o;
}

std::vector<bool>& operator<< (std::vector<bool>& o, std::vector<bool>& v)
{	o.insert(o.end(), v.begin(), v.end());
	return o;
}

std::vector<bool>& operator>> (std::vector<bool>& i, bool& v)
{	v = i.front();
	i.erase(i.begin());
	return i;
}

std::vector<bool>& operator>> (std::vector<bool>& i, std::vector<bool>& v)
{	size_t count = std::min(i.size(), v.size());
	for(size_t j = 0; j < count; ++j) v[j] = i[j];
	v.resize(count);
	i.erase(i.begin(), i.begin()+count);
	return i;
}

std::vector<bool>& operator>> (std::vector<bool>& i, unsigned char& vs)
{	//for(unsigned char c: bits) i.push_back((vs&c));
	for(size_t j = 0; j < 8; ++j) if(i[j]) vs |= bits[j];
	return i;
}
std::vector<bool>& operator>> (std::vector<bool>& i, std::vector<unsigned char>& cs)
{	for(unsigned char c : cs) i >> c;
	return i;
}


std::vector<bool>& operator<< (std::vector<bool>& o, std::vector<unsigned char>& cs)
{	for(const unsigned char c : cs) o << c;
	return o;
}

std::vector<unsigned char>& operator<< (std::vector<unsigned char>& o, const unsigned char& c)
{	o.insert(o.end(), c);
	return o;
}
std::vector<unsigned char>& operator<< (std::vector<unsigned char>& o, std::vector<unsigned char>& cs)
{	o.insert(o.end(), cs.begin(), cs.end());
	return o;
}

std::ostream&  operator<<(std::ostream& o, const std::vector<bool>& v)
{	for(bool i: v) o << (i?'1':'0');
	return o;
}

std::ostream&  operator<<(std::ostream& o, const std::vector<unsigned char>& v)
{	o.write(reinterpret_cast<const char*>(v.data()), v.size());
	//for(unsigned char i: v) o.write(reinterpret_cast<char*>(&i), 1);
	return o;
}

std::ostream&  operator<<(std::ostream& o, const std::vector<char>& v)
{	for(char i: v) o.write(&i, 1);
	return o;
}

size_t printVectorBool(std::ostream& o, std::vector<bool>&v, size_t threshold)
{	if(v.size() < threshold) return 0;
	circularCounter<unsigned> j(8);
	unsigned char c = 0x00;
	size_t written = 0;
	bool b;
	std::vector<bool>::iterator i = v.begin();
	std::vector<bool>::iterator e;
	if(threshold > 0) e = v.begin()+threshold;
	else e = v.end();

	while(i != e)
	{	b = *i;
		if(b) c |= bits[*j];
		++i;
		++j;
		if(*j == 0U)
		{	o.write(reinterpret_cast<char*>(&c), 1);
			c = 0x00;
			++written;
		}
	}
	if(*j != 0U) o.write(reinterpret_cast<char*>(&c), 1);
	v.erase(v.begin(), e);
	return written;
}

size_t readVectorBool(std::istream& i, std::vector<bool>&v, size_t count)
{	circularCounter<unsigned> j(8);
	bool b;
	size_t currentChar = 0;
	size_t bytesRead;
	size_t toRead = count/8;
	size_t extraByte = ((count%8)!=0);
	toRead += extraByte;
	unsigned char *c = new unsigned char[toRead];
	i.read(reinterpret_cast<char*>(c), toRead);
	bytesRead = i.gcount();
	while(count > 0 && currentChar < bytesRead)// std::numeric_limits<size_t>::max())
	{	b = (c[currentChar] & bits[*j]);
		v.push_back(b);
		++j;
		--count;
		if(*j == 0U)
			++currentChar;
	}
	delete[] c;
	currentChar += extraByte;
	if(currentChar != bytesRead) throw std::runtime_error("Unexpected mismatch between bytes read and bytes processed");
	return currentChar;
}
