/*
 * huff.h
 *
 *  Created on: May 20, 2014
 *      Author: aarcane
 */

#ifndef HUFF_H_
#define HUFF_H_

#include "codec.h"
#include <vector>
#include <utility>
#include <queue>
#include <string>
#include <map>

class huff: public codec
{public:
	huff(std::istream &, std::ostream &);
	virtual ~huff();
	virtual void compress(void) override;
	virtual void decompress(void) override;
	virtual unsigned char magic() override;

private:
	class mapChar;
	class treeNodeStarCompare;
	class treeNode;
	static const unsigned char MAGIC = 13;
	std::vector<long long unsigned int> byteCount;
	treeNode* tree = nullptr;
	std::map<mapChar, std::vector<bool>> forwardMap;
	std::map<std::vector<bool>, mapChar> reverseMap;
	std::vector<bool> nodeList;
	std::vector<unsigned char> charList;
	void countBytes();
	void resetStream();
	const huff::treeNode* treeMin(std::priority_queue<treeNode*, std::vector<treeNode*>, treeNodeStarCompare>& t1, std::priority_queue<treeNode*, std::vector<treeNode*>, treeNodeStarCompare>& t2);
	void buildTree();
	void buildMap();
	void compressBytes();
	void rebuildTree();
	void decompressBytes();

	//Forward declarations of nested classes and friend functions
	class treeLeaf;
	class treeEOF;
	class treeInternal;
	friend bool operator<(const mapChar& l, const mapChar& r);
	friend bool operator<(const treeNode& l, const treeNode& r);
	friend bool operator>(const treeNode& l, const treeNode& r);
	friend bool operator<=(const treeNode& l, const treeNode& r);
	friend bool operator>=(const treeNode& l, const treeNode& r);
	friend bool operator==(const treeNode& l, const treeNode& r);

};

#endif /* HUFF_H_ */
