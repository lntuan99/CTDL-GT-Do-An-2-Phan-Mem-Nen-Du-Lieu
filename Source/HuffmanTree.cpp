#include "HuffmanTree.h"
#include <ctime>
#include <iostream>

void HuffmanTree::createMinHeap() {
	//Kí tự nào có xuất hiện trong file thì thêm vào cây min heap
	for (int i = 0; i < 256; ++i)
		if (charFreq[i] != 0)
			minHeap.push(new HuffNode(i, charFreq[i], NULL, NULL));

	HuffNode* l, * r;

	//Thực hiện việc tạo cây. đến khi nào cây chỉ còn 1 nút gốc k có nút con thì dừng 
	while (minHeap.size() != 1) {
		l = minHeap.top();
		minHeap.pop();

		r = minHeap.top();
		minHeap.pop();

		minHeap.push(new HuffNode('#', l->nFreq + r->nFreq, l, r));
	}
}

void HuffmanTree::countCharFreq() {
	for (unsigned long long i = 0; i < fileContent.size(); ++i)
		charFreq[fileContent[i]]++;
}

void HuffmanTree::setCharCode(HuffNode* root, string str) {
	//Nút rỗng thì dừng 
	if (!root)
		return;

	//Nếu nút là nút lá thì gán mã code cho kí tự 
	if (root->left == NULL && root->right == NULL) {
		codes[root->c] = str;
	}
	
	//Duyệt sang trái + thêm 0
	setCharCode(root->left, str + "0");

	//Duyệt sang phải + thêm 1
	setCharCode(root->right, str + "1");
}

void HuffmanTree::setFileContent(vector<unsigned char> fct) {
	fileContent = fct;
}

vector<unsigned char> HuffmanTree::getFileContent() {
	return fileContent;
}

priority_queue<HuffNode*, vector<HuffNode*>, Compare> HuffmanTree::getMinHeap() {
	return minHeap;
}

unsigned long long* HuffmanTree::getCharFreq() {
	return charFreq;
}

string HuffmanTree::getCharCode(unsigned char c) {
	return codes[c];
}

char** HuffmanTree::getAllCharCode() {
	char** res = new char* [256];

	for (int i = 0; i < 256; ++i) {
		int len = codes[i].length();
		res[i] = new char[len + 1];

		for (int j = 0; j < len; ++j) {
			res[i][j] = codes[i][j];
		}

		res[i][len] = '\0';
	}
	return res;
}

void HuffmanTree::setCharFreq(unsigned long long p[256]) {
	for (int i = 0; i < 256; ++i)
		charFreq[i] = p[i];
}