#ifndef LZ2_H_
#define LZ2_H_

#include "lz.h"

class lz2: public lz
{public:
	lz2(std::istream& in, std::ostream& out):
		lz(in, out, 4096)
	{};

	unsigned char magic() { return MAGIC; };

private:
	static const unsigned char MAGIC = 19;

};

#endif /* LZ2_H_ */
