/*
 * tempFile.cpp
 *
 *  Created on: May 19, 2014
 *      Author: aarcane
 */

#include "tempFile.h"
#include <cstdio>
#include <iostream>
#include <stdexcept>

tempFile::tempFile():
	std::fstream(),
	filename("")
{
}
tempFile::tempFile(std::istream &in):
		tempFile()
{	open(in);
}

tempFile::~tempFile()
{	close();
}

void tempFile::open(std::istream &in)
{	newTempFile();
	static const size_t buf_size = 8192;
	char ch[buf_size];
	for(in.read(ch, buf_size); in.gcount() > 0; in.read(ch, buf_size))
		std::fstream::write(ch, in.gcount());
	//std::fstream::flush(); //flush buffers to output
	//std::fstream::sync(); //sync input and output
	std::fstream::seekg(0); //reset to beginning.
}
void tempFile::close()
{	if(filename == "") return;
	std::fstream::close();
	remove(filename.c_str());
	filename = "";
}
void tempFile::newTempFile()
{	close();
	filename = tmpnam(NULL);
	std::fstream::open(filename, std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	if(!is_open())
	{	throw new std::runtime_error("Failure to open file.");
	}
}

