/*
 * huff.cpp
 *
 *  Created on: May 20, 2014
 *      Author: aarcane
 */

#include "huff.h"
#include <limits>
#include <iostream>
#include <queue>
#include <stack>
#include <stdexcept>

class huff::treeNodeStarCompare
{private:
	bool reverse;
public:
	treeNodeStarCompare(const bool& rev=false);
	bool operator() (const huff::treeNode* l, const huff::treeNode* r) const;
};

class huff::mapChar
{public:
	unsigned char c;
	bool isEOF;
	mapChar(const unsigned char ch);
	mapChar();
	friend bool operator<(const mapChar& l, const mapChar& r);
	friend std::ostream& operator<< (std::ostream& o, const mapChar& m);
};

class huff::treeNode
{public:
	long long unsigned int count;
	bool hasEOF;
	treeNode(long long unsigned int cnt, bool e = false);
	virtual ~treeNode(){};
	friend bool operator<(const treeNode& l, const treeNode& r);
	friend bool operator>(const treeNode& l, const treeNode& r);
	friend bool operator<=(const treeNode& l, const treeNode& r);
	friend bool operator>=(const treeNode& l, const treeNode& r);
	friend bool operator==(const treeNode& l, const treeNode& r);
	//friend std::ostream& operator<< (std::ostream& o, const treeNode& r);
	//virtual void doPrint(std::ostream& o) const;
	virtual void walk(std::map<mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const = 0;
};

class huff::treeEOF: public huff::treeNode
{public:
	std::string c = "EOF";
	treeEOF();
	//virtual void doPrint(std::ostream& o) const override;
	virtual void walk(std::map<mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const override;
};

class huff::treeLeaf: public huff::treeNode
{public:
	unsigned char c;
	treeLeaf(long long unsigned int cnt, unsigned char ch);
	//virtual void doPrint(std::ostream& o) const override;
	virtual void walk(std::map<mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const override;
};

class huff::treeInternal: public huff::treeNode
{public:
	const treeNode* left;
	const treeNode* right;
	treeInternal(const treeNode* l, const treeNode* r);
	treeInternal();
	virtual ~treeInternal();
	//virtual void doPrint(std::ostream& o) const override;
	virtual void walk(std::map<mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const override;
};

inline size_t count1s(std::vector<bool> in);

huff::huff(std::istream &in, std::ostream &out):
	codec(in, out)
{	byteCount.resize(std::numeric_limits<unsigned char>::max()+1, 0ULL);
}

huff::~huff()
{	delete tree;
}

unsigned char huff::magic()
{	return MAGIC;
}

void huff::compress(void)
{	countBytes();
	resetStream();
	buildTree();
	buildMap();
	compressBytes();
}

void huff::decompress(void)
{	rebuildTree();
	buildMap();
	decompressBytes();
}

void huff::countBytes()
{//	std::cerr << "Huffman Coding: Counting bytes" << std::endl;
	unsigned char c;
	while(i.read(reinterpret_cast<char*>(&c), 1)) ++byteCount[c];
}

void huff::resetStream()
{//	std::cerr << "Huffman Coding: Resetting stream" << std::endl;
	i.clear();
	i.seekg(0);
}

const huff::treeNode* huff::treeMin(std::priority_queue<treeNode*, std::vector<treeNode*>, treeNodeStarCompare>& t1, std::priority_queue<treeNode*, std::vector<treeNode*>, treeNodeStarCompare>& t2)
{	const huff::treeNode* ret;
	if(t2.empty() && t1.empty()) throw std::runtime_error("huff::treeMin: Unable to produce nodes from two empty trees");
	else if(t2.empty())
	{	ret = t1.top();
		t1.pop();
	}
	else if(t1.empty())
	{	ret = t2.top();
		t2.pop();
	}
	else
	{	if(*(t1.top()) <= *(t2.top()))
		{	ret = t1.top();
			t1.pop();
		}
		else
		{	ret = t2.top();
			t2.pop();
	}	}
	return ret;
}

void huff::buildTree()
{	//std::cerr << "Huffman Coding: Building Tree" << std::endl;
	std::priority_queue<treeNode*, std::vector<treeNode*>, treeNodeStarCompare> bare;
	std::priority_queue<treeNode*, std::vector<treeNode*>, treeNodeStarCompare> trees;
	for(unsigned d = 0; d < byteCount.size(); ++d) if(byteCount.at(d) > 0) bare.push(new treeLeaf(byteCount.at(d),(unsigned char)d));
	bare.push(new treeEOF());
	if(bare.size()==1)
	{	tree = bare.top();
		bare.pop();
	}
	else
	{	const treeNode* t1 = treeMin(bare, trees);
		const treeNode* t2 = treeMin(bare, trees);
		trees.push(new treeInternal(t1, t2));
		while(bare.size() > 0 || trees.size() > 1)
		{	t1 = treeMin(bare, trees);
			t2 = treeMin(bare, trees);
			trees.push(new treeInternal(t1, t2));
		}
		tree = trees.top();
		trees.pop();
}	}

void huff::buildMap()
{//	std::cerr << "Huffman Coding: Building Map" << std::endl;
	std::vector<bool> root;
	root.push_back(false);
	tree->walk(forwardMap, reverseMap, nodeList, charList, root);
}

void huff::compressBytes()
{	//std::cerr << "Huffman Coding: Compressing ByteStream" << std::endl;
	std::vector<bool> os;
	unsigned char m = magic();
	o.write(reinterpret_cast<char*>(&m), 1);
	size_t s;
	s = nodeList.size();
	o.write(reinterpret_cast<char*>(&s), sizeof(size_t));
	printVectorBool(o, nodeList, 0);
	o.write(reinterpret_cast<char*>(charList.data()), charList.size());
	char c;
	while(i.read(&c, 1))
	{	os << forwardMap.at(huff::mapChar(c));
		printVectorBool(o, os);//print it only if longer than default flush size
	}
	os << forwardMap.at(huff::mapChar());
	printVectorBool(o, os, 0); //flush everything
	o.flush();
}

void huff::rebuildTree()
{	//std::cerr << "Huffman Coding: Rebuilding Tree" << std::endl;
	size_t nodeListSize;
	i.read(reinterpret_cast<char*>(&nodeListSize), sizeof(size_t));
	readVectorBool(i, nodeList, nodeListSize);
	size_t charListSize = count1s(nodeList);
	size_t charListPos = 0;
	unsigned char* chars = new unsigned char[charListSize];
	i.read(reinterpret_cast<char*>(chars), charListSize);
	std::vector<treeNode*> tr;
	for(bool b: nodeList)
	{	if(b && charListPos < charListSize) tr.push_back(new treeLeaf(0, chars[charListPos++]));
		else if(b) tr.push_back(new treeEOF());
		else tr.push_back(new treeInternal());
	}
	delete[] chars;
	nodeList.resize(0);
	std::vector<treeNode*>::iterator t = tr.begin();
	std::stack<treeInternal*> ts;
	treeInternal* current = static_cast<treeInternal*>(*t);
	ts.push(current);
	++t;
	while(t != tr.end())
	{	current = ts.top();
		if(current->left == nullptr) current->left = *t;
		else if(current->right == nullptr)
		{	current->right = *t;
			ts.pop();
		}
		else throw std::runtime_error("Rebuild Tree: Unable to rebuild tree.  Something has gone terriby wrong.  Both pointers on current are != nullptr.");
		if(treeInternal* tn = dynamic_cast<treeInternal*>(*t)) //do we have a treeInternal?
			ts.push(tn);
		++t;
	}
	tree = tr.at(0);
}

void huff::decompressBytes()
{	//std::cerr << "Huffman Coding: Decompressing ByteStream" << std::endl;
	std::vector<bool> working;
	std::vector<bool> w;
	bool eof = false;
	size_t read;
	mapChar* c;
	while(!eof) //read until end of literal or virtual input.
	{	working.resize(0);
		working.reserve(8192*8);
		read = readVectorBool(i, working, 8192*8);
		eof=(read < 8192);
		for(bool j: working)
		{	w.push_back(j);
			if(reverseMap.count(w) > 0)
			{	c = &(reverseMap.at(w));
				if(c->isEOF) //END OF FILE
				{	eof = true;
					o.flush();
					return;
					break;
				}
				else o.write(reinterpret_cast<char*>(&(c->c)), 1);
				w.resize(0);
	}	}	}
	std::cerr << "EOF Marker not found.  Please verify file integrity manually." << std::endl;
	o.flush();
}

//huff::treeNodeStarCompare::
huff::treeNodeStarCompare::treeNodeStarCompare(const bool& rev): reverse(rev)
{
}

bool huff::treeNodeStarCompare::operator() (const huff::treeNode* l, const huff::treeNode* r) const
{	return (reverse?(*r < *l):(*l < *r));
}

//huff::mapChar::
huff::mapChar::mapChar(const unsigned char ch): c(ch), isEOF(false){}

huff::mapChar::mapChar(): c(0xFF), isEOF(true) {}

bool operator<( const huff::mapChar& l, const huff::mapChar& r )
{	if(r.isEOF) return false;
	if(l.isEOF && r.isEOF) return false;// This quirky condition handles the < > compare for reflexive inequality in std::map
	if(l.isEOF) return true;
	return l.c < r.c;
}

/* std::ostream& operator<< (std::ostream& o, const huff::mapChar& m)
{	if(m.isEOF) o << "EOF";
	else o << m.c;
	return o;
}
*/

//huff::treeNode
huff::treeNode::treeNode(long long unsigned int cnt, bool e):
	count(cnt),
	hasEOF(e)
{
}

bool operator<( const huff::treeNode& l, const huff::treeNode& r )
{	return l.count > r.count;
}

bool operator>( const huff::treeNode& l, const huff::treeNode& r )
{	return l.count < r.count;
}

bool operator<=( const huff::treeNode& l, const huff::treeNode& r )
{	return l.count >= r.count;
}

bool operator>=( const huff::treeNode& l, const huff::treeNode& r )
{	return l.count <= r.count;
}

bool operator==( const huff::treeNode& l, const huff::treeNode& r )
{	return l.count == r.count;
}

/* std::ostream& operator<< (std::ostream& o, const huff::treeNode& r)
{	r.doPrint(o);
	return o;
}
*/

/* void huff::treeNode::doPrint(std::ostream& o) const
{	o << count;
}
*/

//huff::treeEOF::
huff::treeEOF::treeEOF():
	treeNode(1, true)
{
}

/* void huff::treeEOF::doPrint(std::ostream& o) const
{	static_cast<const huff::treeNode*>(this)->doPrint(o);
	o << ": " << c;
}
*/

void huff::treeEOF::walk(std::map<huff::mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, huff::mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const
{	forwardMap.insert(std::pair<huff::mapChar, std::vector<bool>>(huff::mapChar(), toMe));
	reverseMap.insert(std::pair<std::vector<bool>, huff::mapChar>(toMe, huff::mapChar()));
	nodeList.push_back(1);
	(void)charList;
}

//huff::treeLeaf::
huff::treeLeaf::treeLeaf(long long unsigned int cnt, unsigned char ch):
	huff::treeNode(cnt),
	c(ch)
{
}

/* void huff::treeLeaf::doPrint(std::ostream& o) const
{	static_cast<const huff::treeNode*>(this)->doPrint(o);
	o << ": " << c;
}
*/

void huff::treeLeaf::walk(std::map<mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const
{	//std::cerr << "(" << count << "," << c << "):" << toMe << std::endl;
	forwardMap.insert(std::pair<mapChar, std::vector<bool>>(mapChar(c), toMe));
	reverseMap.insert(std::pair<std::vector<bool>, mapChar>(toMe, mapChar(c)));
	nodeList.push_back(1);
	charList.push_back(c);
}

//huff::treeInternal::
huff::treeInternal::treeInternal(const huff::treeNode* l, const huff::treeNode* r):
	huff::treeNode(l->count+r->count)
{	if(l->hasEOF)
	{	left = r;
		right = l;
	}
	else
	{	left = l;
		right = r;
	}
	hasEOF = right->hasEOF;
}

huff::treeInternal::treeInternal():
	huff::treeNode(1), left(nullptr), right(nullptr)
{}

huff::treeInternal::~treeInternal()
{	if(left != nullptr)
	{	delete left;
		left = nullptr;
	}
	if(right != nullptr)
	{	delete right;
		right = nullptr;
	}

}

/* void huff::treeInternal::doPrint(std::ostream& o) const
{	static_cast<const huff::treeNode*>(this)->doPrint(o);
	o << ": " << "Internal Node.  {left: ";
	if(left != nullptr) o << *left;
	else o << "NULL";
	o << " right: ";
	if(right != nullptr) o << *right;
	else o << "NULL";
	o << "}";
}
*/

void huff::treeInternal::walk(std::map<huff::mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, huff::mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const
{	nodeList.push_back(false); //0 for internal node.

	toMe.push_back(0);
	left->walk(forwardMap, reverseMap, nodeList, charList, toMe);
	toMe.back() = 1;
	right->walk(forwardMap, reverseMap, nodeList, charList, toMe);
}

std::vector<bool>& operator<< (std::vector<bool>& o, const bool& v)
{	o.push_back(v);
	return o;
}

inline size_t count1s(std::vector<bool> in)
{	size_t ret = 0;
	for(bool i: in) if(i) ++ret;
	return --ret;
}
