#pragma once

#include "HuffNode.h"
#include <vector>
#include <queue>
#include <map>
#include <string>

using namespace std;

class HuffmanTree
{
private:
	//Cây Huffman Tree
	priority_queue<HuffNode*, vector<HuffNode*>, Compare> minHeap;

	//Số lần xuất hiện của từng kí tự trong bảng mã ascii
	unsigned long long charFreq[256];

	//Mã code tương ứng của từng kí tự trong bảng mã ascii
	string codes[256];
	
	//Nội dung của file được đọc vào 
	vector<unsigned char> fileContent;
	
public:
	//Tạo cây MinHeap
	void createMinHeap();

	//Đếm số lần xuất hiện của từng kí tự
	void countCharFreq();

	//Tạo mã code cho từng kí tự.
	void setCharCode(HuffNode*, string);

	//Gán nội dung file
	void setFileContent(vector<unsigned char>);

	//Lấy ra mã code của một kí tự
	string getCharCode(unsigned char);

	//Lấy ra nội dung file
	vector<unsigned char> getFileContent();

	//Lấy ra cây min heap
	priority_queue<HuffNode*, vector<HuffNode*>, Compare> getMinHeap();

	//Lấy ra số bảng tần số số lần xuất hiện
	unsigned long long* getCharFreq();

	//gán bảng tần số.
	void setCharFreq(unsigned long long p[256]);

	//Lấy toàn bộ mã code của từng kí tự, đưa về char* 
	char** getAllCharCode();
};

