#ifndef LZ1_H_
#define LZ1_H_

#include "lz.h"

class lz1: public lz
{public:
	lz1(std::istream& in, std::ostream& out):
		lz(in, out, 2048)
	{};

	unsigned char magic() { return MAGIC; };

private:
	static const unsigned char MAGIC = 17;

};

#endif /* LZ1_H_ */
