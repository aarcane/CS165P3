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
	static const unsigned char MAGIC = 13;
	std::vector<long long unsigned int> byteCount;
	class mapChar;
	class treeNode;
	class treeNodeStarCompare;
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

	class treeNodeStarCompare
	{private:
		bool reverse;
	public:
		treeNodeStarCompare(const bool& rev=false);
		bool operator() (const huff::treeNode* l, const huff::treeNode* r) const;
	};

	class mapChar
	{public:
		char c[2];
		mapChar(const char ch);
		mapChar();
		friend bool operator<( const mapChar& l, const mapChar& r );
	};

	class treeNode
	{public:
		long long unsigned int count;
		bool hasEOF;
		treeNode(long long unsigned int cnt, bool e = false);
		friend bool operator<( const treeNode& l, const treeNode& r );
		friend bool operator>( const treeNode& l, const treeNode& r );
		friend bool operator<=( const treeNode& l, const treeNode& r );
		friend bool operator>=( const treeNode& l, const treeNode& r );
		friend bool operator==( const treeNode& l, const treeNode& r );
		friend std::ostream& operator<< (std::ostream& o, const treeNode& r);
		virtual void doPrint(std::ostream& o) const;
		virtual void walk(std::map<mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const = 0;
	};
	class treeEOF: public treeNode
	{public:
		std::string c = "EOF";
		treeEOF();
		virtual void doPrint(std::ostream& o) const override;
		virtual void walk(std::map<mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const override;
	};
	class treeLeaf: public treeNode
	{public:
		unsigned char c;
		treeLeaf(long long unsigned int cnt, unsigned char ch);
		virtual void doPrint(std::ostream& o) const override;
		virtual void walk(std::map<mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const override;
	};
	class treeInternal: public treeNode
	{public:
		const treeNode* left;
		const treeNode* right;
		treeInternal(const treeNode* l, const treeNode* r);
		treeInternal();
		virtual void doPrint(std::ostream& o) const override;
		virtual void walk(std::map<mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const override;
};	};


#endif /* HUFF_H_ */
