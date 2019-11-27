#pragma once
class HuffNode
{
public:
	unsigned char c;
	unsigned long long nFreq;
	HuffNode* left;
	HuffNode* right;

	HuffNode();
	HuffNode(unsigned char, unsigned long long, HuffNode*, HuffNode*);
};

struct Compare {
	bool operator()(HuffNode* l, HuffNode* r) {
		return l->nFreq > r->nFreq;
	};
};
