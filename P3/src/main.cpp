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
#include "tempFile.h"

void testStream(std::fstream &in)
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
void testRead(std::fstream &in)
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
int main(void)
{	//itfstream itf(std::cin);
	tempFile itf(std::cin);
	//std::fstream &in = itf;
	testRead(itf);
	testStream(itf);

	std::cout << std::endl;
	return EXIT_SUCCESS;
}
