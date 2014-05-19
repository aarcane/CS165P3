/*
 * tempFile.h
 *
 *  Created on: May 19, 2014
 *      Author: aarcane
 */

#ifndef TEMPFILE_H_
#define TEMPFILE_H_
#include <fstream>

class tempFile : public std::fstream
{private:
	std::string filename;
	void newTempFile();
public:
	tempFile();
	tempFile(std::istream &);
	~tempFile();
	void open(std::istream &);
	void close();

};

#endif /* TEMPFILE_H_ */
