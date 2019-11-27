#include "HuffNode.h"
#include <cstdlib>

HuffNode::HuffNode() {
	c = nFreq = 0;
	left = right = NULL;
}

HuffNode::HuffNode(unsigned char ch, unsigned long long freq, HuffNode* l, HuffNode* r) {
	c = ch;
	nFreq = freq;
	left = l;
	right = r;
}