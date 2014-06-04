/*
 * lz.cpp
 *
 *  Created on: Jun 1, 2014
 *      Author: aarcane
 */

#include "lz.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <limits>
#include <map>
//include "circularCounter.h"

#include <random>
#include <functional>
#include <algorithm>

class token
{public:
	unsigned char s;
	token(unsigned char size): s(size) {if(s > 16) throw std::out_of_range("Invalid size");};
	virtual ~token(){};
	virtual std::vector<bool> getVector(void)
	{	std::vector<bool> ret;
		ret.resize(4,false);
		ret[0] = (s-1) & bits[4];
		ret[1] = (s-1) & bits[5];
		ret[2] = (s-1) & bits[6];
		ret[3] = (s-1) & bits[7];
		return ret;
	};
};

class tokenOffset: public token
{public:
	static size_t M;
	unsigned o;
	tokenOffset(unsigned char size, unsigned offset): token(size), o(offset) {/*if(o > M) throw std::out_of_range("Invalid range");*/};
	virtual ~tokenOffset(){};
	virtual std::vector<bool> getVector(void) override
	{	std::vector<bool> ret = token::getVector();
		if(M == 12)	ret.push_back(reinterpret_cast<char*>(&o)[1] & bits[4]);
		ret.push_back(reinterpret_cast<char*>(&o)[1] & bits[5]);
		ret.push_back(reinterpret_cast<char*>(&o)[1] & bits[6]);
		ret.push_back(reinterpret_cast<char*>(&o)[1] & bits[7]);

		ret.push_back(reinterpret_cast<char*>(&o)[0] & bits[0]);
		ret.push_back(reinterpret_cast<char*>(&o)[0] & bits[1]);
		ret.push_back(reinterpret_cast<char*>(&o)[0] & bits[2]);
		ret.push_back(reinterpret_cast<char*>(&o)[0] & bits[3]);
		ret.push_back(reinterpret_cast<char*>(&o)[0] & bits[4]);
		ret.push_back(reinterpret_cast<char*>(&o)[0] & bits[5]);
		ret.push_back(reinterpret_cast<char*>(&o)[0] & bits[6]);
		ret.push_back(reinterpret_cast<char*>(&o)[0] & bits[7]);
		return ret;
	};
};

size_t tokenOffset::M = 0;

class tokenSequence: public token
{public:
	unsigned char l;
	std::vector<unsigned char> ch;
	tokenSequence(std::vector<unsigned char> charSeq ): token(0), l(charSeq.size()), ch(charSeq)
	{	if(l > 16) throw std::out_of_range("sequence too long");
	};
	virtual ~tokenSequence(){}
	virtual std::vector<bool> getVector(void) override
	{	std::vector<bool> ret = token::getVector();
		ret.push_back(l & bits[4]);
		ret.push_back(l & bits[5]);
		ret.push_back(l & bits[6]);
		ret.push_back(l & bits[7]);
		ret << ch;
		return ret;
	};
};

lz::lz(std::istream &in, std::ostream &out, size_t windowSize):
	codec(in, out),
	N(windowSize),
	M(HighOrderBit(windowSize))
{
	tokenOffset::M = M;
}

lz::~lz(){}

void resizeBuffer(std::vector<unsigned char>& buf, size_t size)
{	if(size < buf.size()) buf.erase(buf.begin(), buf.begin()+(buf.size()-size));
}

void dumpTokens(std::vector<token*>& tok, size_t N, size_t M, size_t F, std::vector<bool>& out, std::ostream& o, bool flush = false);

void lz::compress(void)
{	std::cerr << "LZ compression.  Window size: " << N << " Gap size: " << M << std::endl;
	unsigned char m = magic();
	o.write(reinterpret_cast<char*>(&m), 1);
	std::cerr << int(m) << std::endl;
	std::vector<unsigned char> buffer;
	buffer.resize(2*N);
	unsigned char in[16];
	size_t current = 0;
	size_t avail = 0;
	size_t j;
	std::vector<token*> tok;
	std::vector<bool> out;
	std::vector<unsigned char> tokenStr;
	std::vector<unsigned char> testStr;

	bool done = false;
	do
	{	if(i)
		{	i.read(reinterpret_cast<char*>(in), 16-avail);
			for(std::streamsize j = 0; j < i.gcount(); ++j)
			{	buffer[(current+avail+j)%(2*N)] = in[j];
				pushInv(in[j]);
				pushMap(current+avail+j);
			}
			avail+=i.gcount();
		}

		j = 1;
		size_t tokenPos = current;
		bool found = false;
		for(size_t m = 0; m < avail && !found; ++m)
		{	testStr.resize((avail-m));
			for(size_t k = 0; k <= (avail-m); ++k) testStr[k] = buffer[(tokenPos+k) % (2*N)];
			std::pair<std::multimap<std::vector<unsigned char>,size_t>::iterator, std::multimap<std::vector<unsigned char>,size_t>::iterator> inMap = posMap.equal_range(testStr);
			std::multimap<std::vector<unsigned char>, size_t>::iterator it = inMap.first;
			while(it != inMap.second && !found)
			{	if(current > (N - 16) && it->second < current - (N - 16)) it = posMap.erase(it);
				else if(it->second < current)
				{	j = avail-m;
					tokenPos = it->second;
					found = true;
				}
				else ++it;
			}
		}

		//tokenStr.resize(j);
		//for(size_t k = 0; k <= j; ++k) tokenStr[k] = buffer[(tokenPos+k) % (2*N)];
		//tokenPos = current-tokenPos;
		if((current - tokenPos)) //positive seek back
		{	tok.push_back(new tokenOffset(j, (current - tokenPos)));
		}
		else
		{	tokenStr.resize(j);
			for(size_t k = 0; k <= j; ++k) tokenStr[k] = buffer[(tokenPos+k) % (2*N)];
			tok.push_back(new tokenSequence(tokenStr));
		}
		dumpTokens(tok, N, M, F, out, o);
		//std::cout << tokenStr;
		avail -= j;
		current += j;
		done = (avail == 0 && !i);
	} while (!done);
	dumpTokens(tok, N, M, F, out, o, true);
}

void dumpTokens(std::vector<token*>& tok, size_t N, size_t M, size_t F, std::vector<bool>& out, std::ostream& o, bool flush)
{	tokenSequence* pending = nullptr;
	while(tok.size() > 0)
	{	token* T;
		tokenSequence* S;
		//tokenOffset* O;
		T = tok.front();
		std::vector<bool> tempVec;
		tok.erase(tok.begin());
		if((S = dynamic_cast<tokenSequence*>(T)))
		{	if(pending == nullptr) pending = S;
			else if(pending->l < 16)
			{	std::vector<unsigned char> c = pending->ch;
				delete pending;
				pending = nullptr;
				c << S->ch;
				delete S;
				pending = new tokenSequence(c);
			}
			else if(pending->l == 16)
			{	tempVec = pending->getVector();
				out << tempVec;
				delete pending;
				pending = S;
			}
			else throw std::logic_error("This is bad.  Okay, that should never have happened.  I'm sorry.");
		}
		else //if(O = dynamic_cast<tokenOffset*>(T))
		{	if(pending != nullptr)
			{	tempVec = pending->getVector();
				out << tempVec;
				delete pending;
				pending = nullptr;
			}
			tempVec = T->getVector();
			out << tempVec;
		}
	}
	printVectorBool(o, out);
	(void)N;
	(void)M;
	(void)F;
	if(flush) printVectorBool(o, out, 0);
}

token* readToken( std::istream& i, std::vector<bool>& in);

void lz::decompress(void)
{	std::vector<bool> in;
	std::vector<unsigned char> buffer;
	size_t current = 0;
	size_t lastWrite = 0;
	buffer.resize(2*N);
	token * T = nullptr;
	tokenSequence * S = nullptr;
	tokenOffset * O = nullptr;
	while((T = readToken(i, in)))
	{	if((S = dynamic_cast<tokenSequence*>(T)))
		{	std::cerr << "Sequence of length " << S->ch.size() << ": " << S->ch << std::endl;
			for(size_t i = 0; i < S->ch.size(); ++i) buffer[(current+i)%(2*N)] = S->ch[i];
			current += S->ch.size();
		}
		if((O = dynamic_cast<tokenOffset*>(T)))
		{	for(size_t i = 0; i < O->s; ++i) buffer[(current+i)%(2*N)] = buffer[((current-(O->o))+i)%(2*N)];
			current += O->s;
		}
		if((lastWrite + N) < current)
		{	o.write(reinterpret_cast<char*>(buffer.data())+(lastWrite%(2*N)), N);
			lastWrite += N;
		}
		T = nullptr;
		S = nullptr;
		O = nullptr;
		std::cerr << "At position: " << current << std::endl;
	}
	if(lastWrite < current)
	{	o.write(reinterpret_cast<char*>(buffer.data())+(lastWrite%(2*N)), (current - lastWrite));
	}
	o.flush();
}

unsigned char readCode(std::vector<bool>);

token* readToken( std::istream& i, std::vector<bool>& in)
{	if(in.size() < 136) /* 4*4*136 */ readVectorBool(i, in, 8192);
	token* ret = nullptr;
	std::vector<bool> test;
	unsigned char c, p;
	test.resize(4);
	in >> test;
	c = readCode(test);
	if(c == 0) // Sequence
	{	in >> test;
		p = readCode(test);
		if(in.size() < (p * 8)) return ret;
		std::vector<unsigned char> ch;
		ch.resize(p);
		in >> ch;
		ret = new tokenSequence(ch);
	}
	else
	{	c += 1;
		unsigned pos = 0;
		bool t;
		if(in.size() < tokenOffset::M) return ret;
		if(tokenOffset::M == 12)
		{	in >> t;
			if(t) pos |= 1 << 11;
		}
		in >> t;
		if(t) pos |= 1 << 10;
		in >> t;
		if(t) pos |= 1 << 9;
		in >> t;
		if(t) pos |= 1 << 8;
		in >> test;
		p = readCode(test);
		pos |= (unsigned(p) << 4);
		in >> test;
		p = readCode(test);
		pos |= (unsigned(p) << 0);
		ret = new tokenOffset(c, p);
	}
	return ret;
}

unsigned char readCode(std::vector<bool> code)
{	unsigned char ret = 0;
	if(code.size() == 4)
	{	if(code[0]) ret |= bits[4];
		if(code[1]) ret |= bits[5];
		if(code[2]) ret |= bits[6];
		if(code[3]) ret |= bits[7];
	}
	return ret;
}

void lz::pushInv(unsigned char c)
{	for(size_t i = 0; i < 16; ++i)
	{	inv[i].push_back(c);
		if(inv[i].size() > i+1) inv[i].erase(inv[i].begin());
}	}

void lz::pushMap(size_t pos)
{	for(size_t i = 1; i < 16; ++i) if(i <= pos) posMap.insert(std::pair<std::vector<unsigned char>, size_t>(inv[i], pos-i));
}

size_t lz::HighOrderBit(size_t t) // O(1).  Always takes at most 64 passes.
{	size_t ret = std::numeric_limits<size_t>::digits;
	size_t one = 1;
	size_t test;
	do test = (one << --ret); while(!(test & t));
	return ret;
}
