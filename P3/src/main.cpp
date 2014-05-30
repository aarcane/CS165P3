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

void testStream(std::istream &in)
{	std::cout << "First pass" << std::endl;
	std::string s;
	in >> s;
	std::cout << s << std::endl;
	in.seekg(0);
	std::cout << "Second pass" << std::endl;
	in >> s;
	std::cout << s << std::endl;
	in.seekg(0);
	std::cout.flush();
}
void testRead(std::istream &in)
{	std::cout << "First pass" << std::endl;
	char c[100];
	for(in.read(c, 100); in.gcount() > 0; in.read(c, 100))
		std::cout.write(c, in.gcount());
	in.clear();
	in.seekg(0);
	std::cout << "Second pass" << std::endl;
	for(in.read(c, 100); in.gcount() > 0; in.read(c, 100))
		std::cout.write(c, in.gcount());
	in.clear();
	in.seekg(0);
	std::cout.flush();
}
int main(int argc, char ** argv)
{	std::vector<std::string> args(argv, argv+argc);
	std::istream *in;
	std::string filename;
	bool usingCin = false;
	tempFile f1;
	std::fstream f2;
	size_t pos = std::string::npos;
	bool compress = false;
	codec *c = 0;

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
			in->read(&ch, 1);
			switch(ch)
			{	case 13: std::cerr << "Expanding Huffman coding" << std::endl;
					compress = false;
					c = new huff(*in, std::cout);
					break;
				case 17: std::cerr << "Expanding LZ1 coding" << std::endl;
					break;
				case 19: std::cerr << "Expanding LZ2 coding" << std::endl;
					break;
				default: std::cerr << "Cowardly refusing to expand unknown coding" << std::endl;
					break;
			}

		}
	else if((pos = args.at(0).rfind("HUFF")) != std::string::npos && args.at(0).substr(pos) == "HUFF")
	{	compress = true;
		if (usingCin) in = new tempFile(*in);
		c = new huff(*in, std::cout);
	}
	else if((pos = args.at(0).rfind("LZ1")) != std::string::npos && args.at(0).substr(pos) == "LZ1")
	{	compress = true;
		std::cerr << "LZ1 coding" << std::endl;
	}
	else if((pos = args.at(0).rfind("LZ2")) != std::string::npos && args.at(0).substr(pos) == "LZ2")
	{	compress = true;
		std::cerr << "LZ2 coding" << std::endl;
	}
	else
	{
		std::cerr << "Huh?  What do you want me to do?" << std::endl;
	}
	if(compress) c->compress();
	else c->decompress();
	std::cout << std::endl;
	return EXIT_SUCCESS;
}
