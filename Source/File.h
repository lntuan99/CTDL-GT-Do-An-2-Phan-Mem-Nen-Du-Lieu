#pragma once

#include <string>
#include <fstream>
#include "HuffmanTree.h"

using namespace std;

class File
{
private:
	HuffmanTree huffTree;

	string filePathIn;
	string filePathOut;

	char nameFile[50];
	unsigned long long bitSize;
	int compress; //1: File để nén, 2: File để giải nén

public:
	File();
	File(string, string, int);
	File(string);

	void setCompress(int);
	int getCompress();

	//Đọc file cần nén
	void readFileEncode();

	//Ghi file nén
	void writeFileEncode();

	//ĐỌc file nén
	void readFileDecode(unsigned char*&, char&);

	//Ghi file giải nén
	void writeFileDecode(unsigned char*, char);

	//Thực thi
	void process();
	
};

