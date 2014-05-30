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
#include "circularCounter.h"

inline std::vector<bool>& operator<< (std::vector<bool>& o, const bool& v);
inline std::vector<bool>& operator<< (std::vector<bool>& o, const unsigned char& vs);
inline std::vector<bool>& operator<< (std::vector<bool>& o, const char& vs);
inline std::vector<bool>& operator<< (std::vector<bool>& o, std::vector<bool>& v);
inline std::vector<bool>& operator<< (std::vector<bool>& o, std::vector<unsigned char>& cs);

inline std::ostream&  operator<<(std::ostream& o, const std::vector<bool>& v);
inline std::ostream&  operator<<(std::ostream& o, const std::vector<unsigned char>& v);
inline std::ostream&  operator<<(std::ostream& o, const std::vector<char>& v);
inline void printVectorBool(std::ostream& o, std::vector<bool>&v, size_t threshold = 8192);
inline void readVectorBool(std::istream& i, std::vector<bool>&v, size_t count = 0);
inline size_t count1s(std::vector<bool> in);

huff::huff(std::istream &in, std::ostream &out):
	codec(in, out)
{	std::cerr << "Huffman Coding" << std::endl;
	byteCount.resize(std::numeric_limits<unsigned char>::max(), 0ULL);
}

huff::~huff() {
	// TODO Auto-generated destructor stub
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
{	std::cerr << "Huffman Coding: Counting bytes" << std::endl;
	char c;
	while(i.read(&c, 1)) ++byteCount[c];
}

void huff::resetStream()
{	std::cerr << "Huffman Coding: Resetting stream" << std::endl;
	i.clear();
	i.seekg(0);
}

const huff::treeNode* huff::treeMin(std::priority_queue<treeNode*, std::vector<treeNode*>, treeNodeStarCompare>& t1, std::priority_queue<treeNode*, std::vector<treeNode*>, treeNodeStarCompare>& t2)
{	const huff::treeNode* ret;
	if(t1.empty())
	{	ret = t2.top();
		t2.pop();
	}
	else
	{	if(t1.top() <= t2.top())
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
{	std::cerr << "Huffman Coding: Building Tree" << std::endl;
	std::priority_queue<treeNode*, std::vector<treeNode*>, treeNodeStarCompare> bare;
	std::priority_queue<treeNode*, std::vector<treeNode*>, treeNodeStarCompare> trees;
	for(unsigned d = 0; d < byteCount.size(); ++d)
		if(byteCount.at(d) > 0) bare.push(new treeLeaf(byteCount.at(d),(unsigned char)d));
	bare.push(new treeEOF());

	//std::cerr << "sizes: bare: " << bare.size() << ", trees: " << trees.size() << std::endl;
	if(bare.size()==1)
	{	tree = bare.top();
		bare.pop();
	}
	else
	{	const treeNode* t1 = bare.top();
		bare.pop();
		const treeNode* t2 = bare.top();
		bare.pop();
		//std::cerr << *t1 << " " << *t2 << std::endl;
		trees.push(new treeInternal(t1, t2));
		while(bare.size() > 0 || trees.size() > 1)
		{	t1 = treeMin(bare, trees);
			t2 = treeMin(bare,trees);
			//std::cerr << *t1 << " " << *t2 << std::endl;
			trees.push(new treeInternal(t1, t2));
			//std::cerr << "  sizes: bare: " << bare.size() << ", trees: " << trees.size() << std::endl;
		}
		tree = trees.top();
		trees.pop();
	}
	//std::cerr << *tree;
}

void huff::buildMap()
{	std::cerr << "Huffman Coding: Building Map" << std::endl;
	std::vector<bool> root;
	root.push_back(false);
	tree->walk(forwardMap, reverseMap, nodeList, charList, root);

	std::cerr <<  forwardMap.size() << " "
			<< reverseMap.size() << " "
			<< nodeList.size() << " "
			<< charList.size() << " "
			<< root.size() << std::endl;
}

void huff::compressBytes()
{	std::cerr << "Huffman Coding: Compressing ByteStream" << std::endl;
	std::cerr << nodeList << std::endl;
	std::vector<bool> os;
	char m[1];
	m[0] = (char)magic();
	o.write(m, 1);
	size_t s[1];
	s[0] = nodeList.size();
	o.write((char *) s, sizeof(size_t));
	//o << nodeList.size();
	printVectorBool(o, nodeList, 0);
	//o << charList;
	o.write((char *)charList.data(), charList.size());
	char c;
	while(i.read(&c, 1))
	{	os<<forwardMap.at(huff::mapChar(c));
		std::cerr <<"\"" << c << "\"";// std::endl;
		printVectorBool(o, os);//print it only if longer than default flush size
	}
	os << forwardMap.at(huff::mapChar());
	std::cerr << "\"EOF\"";
	printVectorBool(o, os, 0); //flush everything
	o.flush();
}

void huff::rebuildTree()
{	std::cerr << "Huffman Coding: Rebuilding Tree" << std::endl;
	size_t nodeListSize[1];
	i.read((char*)nodeListSize, sizeof(size_t));
	std::cerr << "reading nodeList" << std::endl;
	readVectorBool(i, nodeList, nodeListSize[0]);
	std::cerr << nodeList << std::endl;
	size_t charListSize = count1s(nodeList);
	size_t charListPos = 0;
	char* chars = new char[charListSize];
	i.read(chars, charListSize);
	//std::cerr << charListSize << std::endl;
	std::vector<treeNode*> tr;
	for(bool b: nodeList)
	{	if(b && charListPos < charListSize)
		{	tr.push_back(new treeLeaf(0, (unsigned char)chars[charListPos]));
			++charListPos;
		}
		else if(b)
		{	tr.push_back(new treeEOF());
		}
		else
		{	tr.push_back(new treeInternal());
		}
	}
	delete[] chars;
	nodeList.resize(0);

	std::vector<treeNode*>::iterator t = tr.begin();
	//std::vector<bool>::iterator b = nodeList.begin();
	std::stack<treeInternal*> ts;
	treeInternal* current = static_cast<treeInternal*>(*t);
	ts.push(current);
	//++b;
	++t;
	while(t != tr.end())
	{	current = ts.top();
		if(current->left == nullptr)
		{	current->left = *t;
		}
		else if(current->right == nullptr)
		{	current->right = *t;
			ts.pop();
		}
		else
		{	std::cerr << "Unexpected endgame behavior";
		}
		if(treeInternal* tn = dynamic_cast<treeInternal*>(*t)) //do e have a treeInternal?
			ts.push(tn);
		++t;
	}
	tree = tr.at(0);
}

void huff::decompressBytes()
{	std::cerr << "Huffman Coding: Decompressing ByteStream" << std::endl;
	std::vector<bool> working;
	std::vector<bool> w;
	char ch[8192];// = new char[8192];
	bool eof = false;
	mapChar* c;
	size_t read;
	while(!eof) //read until end of literal or virtual input.
	{	i.read(ch, 8192);
		read  = i.gcount();
		for(size_t i = 0; i < read; ++i)
		{	working << ch[i];
		}
		eof=(read < 8192);
		for(bool j: working)
		{	w.push_back(j);
			try
			{	c = &reverseMap.at(w);
				if(c->c[1] == 0xFF) //END OF FILE
				{	eof = false;
					break;
				}
				else o.write(c->c, 1);
				w.resize(0);
			}
			catch (std::out_of_range& e)
			{	continue;
			}
		}
	}
}

//huff::treeNodeStarCompare::
huff::treeNodeStarCompare::treeNodeStarCompare(const bool& rev): reverse(rev)
{
}

bool huff::treeNodeStarCompare::operator() (const huff::treeNode* l, const huff::treeNode* r) const
{	return (reverse?(*r < *l):(*l < *r));
}

//huff::mapChar::
huff::mapChar::mapChar(const char ch)
{	c[0] = ch;
	c[1] = 0;
}

huff::mapChar::mapChar()
{	c[0] = 0xFF;
	c[1] = 0xFF;
}

bool operator<( const huff::mapChar& l, const huff::mapChar& r )
{	if(r.c[1] == 0xFF) return true;
	if(l.c[1] == 0xFF) return false;
	return l.c[0] < r.c[0];
}

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

std::ostream& operator<< (std::ostream& o, const huff::treeNode& r)
{	r.doPrint(o);
	return o;
}

void huff::treeNode::doPrint(std::ostream& o) const
{	o << count;
}

//huff::treeEOF::
huff::treeEOF::treeEOF():
	treeNode(1, true)
{
}

void huff::treeEOF::doPrint(std::ostream& o) const
{	static_cast<huff::treeNode>(*this).doPrint(o);
	o << ": " << c;
}

void huff::treeEOF::walk(std::map<huff::mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, huff::mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const
{	std::cerr << "(" << count << "," << c << "):" << toMe << std::endl;
	forwardMap.insert(std::pair<huff::mapChar, std::vector<bool>>(huff::mapChar(), toMe));
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

void huff::treeLeaf::doPrint(std::ostream& o) const
{	static_cast<huff::treeNode>(*this).doPrint(o);
	o << ": " << c;
}

void huff::treeLeaf::walk(std::map<huff::mapChar, std::vector<bool>>& forwardMap, std::map<std::vector<bool>, huff::mapChar>& reverseMap, std::vector<bool>& nodeList, std::vector<unsigned char>& charList, std::vector<bool> toMe) const
{	std::cerr << "(" << count << "," << c << "):" << toMe << std::endl;
	forwardMap.insert(std::pair<huff::mapChar, std::vector<bool>>(huff::mapChar(c), toMe));
	reverseMap.insert(std::pair<std::vector<bool>, huff::mapChar>(toMe, huff::mapChar(c)));
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
	huff::treeNode(0), left(nullptr), right(nullptr)
{}

void huff::treeInternal::doPrint(std::ostream& o) const
{	static_cast<huff::treeNode>(*this).doPrint(o);
	o << ": " << "Internal Node.  {left:" << *left << " right: " << *right << "}";
}

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

std::vector<bool>& operator<< (std::vector<bool>& o, const unsigned char& vs)
{	bool v;
	unsigned char bits[] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
	for(unsigned char c: bits)
	{	v = (vs&c);
		o.push_back(v);
	}
	return o;
}

std::vector<bool>& operator<< (std::vector<bool>& o, const char& vs)
{	bool v;
	unsigned char bits[] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
	for(unsigned char c: bits)
	{	v = (vs&c);
		o.push_back(v);
	}
	return o;
}


std::vector<bool>& operator<< (std::vector<bool>& o, std::vector<bool>& v)
{	o.insert(o.end(), v.begin(), v.end());
	return o;
}

std::vector<bool>& operator<< (std::vector<bool>& o, std::vector<unsigned char>& cs)
{	for(const unsigned char c : cs)
	{	o << c;
	}
	return o;
}

inline std::ostream&  operator<<(std::ostream& o, const std::vector<bool>& v)
{	for(bool i: v)
	{	o << (i?'1':'0');
	}
	return o;
}

inline std::ostream&  operator<<(std::ostream& o, const std::vector<unsigned char>& v)
{	for(unsigned char i: v)
	{	char j[1] = {static_cast<char>(i)};
		o.write(j, 1);
	}
	return o;
}

inline std::ostream&  operator<<(std::ostream& o, const std::vector<char>& v)
{	for(char i: v)
	{	char j[1] = {i};
		o.write(j, 1);
	}
	return o;
}

inline void printVectorBool(std::ostream& o, std::vector<bool>&v, size_t threshold)
{	if(v.size() < threshold) return;
	circularCounter<unsigned> j(8);
	unsigned char bits[] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
	char c[] = {0x0};
	bool b;
	std::vector<bool>::iterator i = v.begin();
	std::vector<bool>::iterator e;// = v.begin();
	if(threshold > 0)
		e = v.begin()+threshold;
	else
		e = v.end();

	while(i != e)
	{	b = *i;
		if(b)
			c[0] += bits[*j];

		++i;
		++j;
		if(*j == 0U)
		{	o.write(c, 1);
			c[0] = 0x0;
		}
	}
	if(*j != 0U)
		o.write(c, 1);
	v.erase(v.begin(), e);
}

inline void readVectorBool(std::istream& i, std::vector<bool>&v, size_t count)
{	circularCounter<unsigned> j(8);
	unsigned char bits[] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
	char c[] = {0x0};
	bool b;

	i.read(c, 1);
	while(count > 0)// std::numeric_limits<size_t>::max())
	{	b = (c[0] & bits[*j]);
		v.push_back(b);
		++j;
		--count;
		if(*j == 0U) i.read(c, 1);
}	}

inline size_t count1s(std::vector<bool> in)
{	size_t ret = 0;
	for(bool i: in) if(i) ++ret;
	return --ret;
}
