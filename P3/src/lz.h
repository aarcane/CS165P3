/*
 * lz.h
 *
 *  Created on: Jun 1, 2014
 *      Author: aarcane
 */

#ifndef LZ_H_
#define LZ_H_

#include "codec.h"

class lz: public codec
{public:
	lz(std::istream &in, std::ostream &out, size_t windowSize);
	virtual ~lz() override;
	void compress(void) override;
	void decompress(void) override;
	virtual unsigned char magic(void) = 0;

private:
	const size_t N; // Window size
	//class window;
	const size_t M; // Gap size.  Function of Window size
	const size_t F = 16;

	class window;
	size_t HighOrderBit(size_t);
	size_t findInWindow(window&, std::vector<unsigned char>&, size_t);
	bool isMatch(window&, std::vector<unsigned char>&, size_t);
	std::multimap<std::vector<unsigned char>, size_t> posMap; // Map of former invariants and where to find them.
	std::vector<unsigned char> inv[16]; //List of invariants
	void pushInv(unsigned char);
	void pushMap(size_t);
};

#endif /* LZ_H_ */
