//============================================================================
// Name        : P3.cpp
// Author      : Christ Schlacta
// Version     :
// Copyright   : 
// Description : Hello World in C, Ansi-style
//============================================================================

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include "tempFile.h"
#include "codec.h"
#include "huff.h"
#include "lz1.h"
#include "lz2.h"

int main(int argc, char ** argv)
{	std::vector<std::string> args(argv, argv+argc);
	std::istream *in;
	std::string filename;
	bool usingCin = false;
	bool usingTempFile = false;
	tempFile f1;
	std::fstream f2;
	size_t pos = std::string::npos;
	bool compress = false;
	codec *c = nullptr;

	filename = (args.size() == 2)?args.at(1):"-";
	usingCin = (filename == "-");

	if(!usingCin)
	{	f2.open(filename, std::ios_base::in|std::ios_base::binary);
		in = &f2;
	}
	else
		in = &std::cin;

	if((pos = args.at(0).rfind("EXPAND")) != std::string::npos && args.at(0).substr(pos) == "EXPAND")
		{	char ch;
			compress = false;
			in->read(&ch, 1);
			switch(ch)
			{	case 13: //std::cerr << "Expanding Huffman coding" << std::endl;
					c = new huff(*in, std::cout);
					break;
				case 17: //std::cerr << "Expanding LZ1 coding" << std::endl;
					c = new lz1(*in, std::cout);
					break;
				case 19: //std::cerr << "Expanding LZ2 coding" << std::endl;
					c = new lz2(*in, std::cout);
					break;
				default: std::cerr << "Cowardly refusing to expand unknown coding" << std::endl;
					break;
			}

		}
	else if((pos = args.at(0).rfind("HUFF")) != std::string::npos && args.at(0).substr(pos) == "HUFF")
	{	compress = true;
		if (usingCin)
		{	in = new tempFile(*in);
			usingTempFile = true;
		}
		c = new huff(*in, std::cout);
	}
	else if((pos = args.at(0).rfind("LZ1")) != std::string::npos && args.at(0).substr(pos) == "LZ1")
	{	compress = true;
		c = new lz1(*in, std::cout);
		//std::cerr << "LZ1 coding" << std::endl;
	}
	else if((pos = args.at(0).rfind("LZ2")) != std::string::npos && args.at(0).substr(pos) == "LZ2")
	{	compress = true;
		c = new lz2(*in, std::cout);
		//std::cerr << "LZ2 coding" << std::endl;
	}
	else
	{	std::cerr << "Huh?  What do you want me to do?" << std::endl;
	}
	if(c)
	{	if(compress) c->compress();
		else c->decompress();
		delete c;
	}
	if(usingTempFile)
		delete in;
	return EXIT_SUCCESS;
}
