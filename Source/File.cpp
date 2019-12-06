#include "File.h"
#include <iostream>
#pragma warning(disable: 4996)
#include <ctime>

#define MAX_BYTE 1024 * 1024

File::File() {
	filePathIn = filePathOut = "";
	compress = bitSize = 0;
}

File::File(string name) {
	filePathIn = name;
}

File::File(string pathIn, string pathOut, int cp) {
	bitSize = 0;
	filePathIn = pathIn;
	filePathOut = pathOut;
	compress = cp;
}

void File::setCompress(int cp) {
	compress = cp;
}

int File::getCompress() {
	return compress;
}

void File::readFileEncode() {
	//Mở file
	FILE* fi = fopen(filePathIn.c_str(), "rb");
	
	if (fi == NULL) {
		cout << "Cannot open this file to compress!! Please check your file and path file";
		return;
	}
	
	unsigned char c;
	unsigned long long freq[256];

	//Gán toàn bộ phần tử trong bảng tần số = 0
	memset(freq, 0, 256 * sizeof(unsigned long long));
	
	//Đọc toàn bộ byte trong file nén
	unsigned char* fileContent = new unsigned char[MAX_BYTE];

	fseek(fi, 0, SEEK_END);

	unsigned long long len = ftell(fi);

	fseek(fi, 0, SEEK_SET);

	while (len >= MAX_BYTE) {
		fread(fileContent, MAX_BYTE, 1, fi);

		for (unsigned int i = 0; i < MAX_BYTE; ++i)
			freq[fileContent[i]]++;

		len -= MAX_BYTE;
	}

	if (len > 0) {
		fread(fileContent, len, 1, fi);

		for (unsigned int i = 0; i < len; ++i)
			freq[fileContent[i]]++;
	}

	//Gán bảng tần số vào cây huffman

	huffTree.setCharFreq(freq);
	fclose(fi);

	delete[] fileContent;
}

void File::writeFileEncode() {
	FILE* fo = fopen(filePathOut.c_str(), "wb");

	//Lấy ra tên file để ghi lên file nén
	char name[50];
	int i = filePathIn.length() - 1;
	int j = 0;
	while (filePathIn[i] != '\\') {
		name[j++] = filePathIn[i--];
	}
	name[j] = '\0';
	strrev(name);

	//ghi tên file lên file nén
	fwrite(&name, 50, 1, fo);

	//Lấy ra bảng tần số để ghi lên file nén, làm tư liệu cho việc giải nén
	unsigned long long* charFreq = huffTree.getCharFreq();

	//Mảng một chiều để lưu lại độ dài mã code
	int* cnt = new int[256];
	for (int i = 0; i < 256; ++i)
		* (cnt + i) = huffTree.getCharCode(i).length();

	for (int i = 0; i < 256; ++i) 
		bitSize += charFreq[i] * (*(cnt + i));
	

	//Ghi bảng tần số lên file nén
	fwrite(charFreq, sizeof(unsigned long long) * 256, 1, fo);

	//Lấy ra toàn bộ mã code của từng kí tự
	char** code = huffTree.getAllCharCode();

	//convert từ char sang bool
	bool** bitCode = new bool* [256];

	for (int i = 0; i < 256; ++i) {
		bitCode[i] = new bool[*(cnt + i)];

		for (int j = 0; j < *(cnt + i); ++j) {
			*(*(bitCode + i) + j) = *(*(code + i) + j) - '0';
		}
	}
	
	//Tính toán padding
	char padding = 0;

	while (bitSize % 8 != 0) {
		++bitSize;
		++padding;
	}

	//Ghi padding và độ dài chuỗi bit đã cộng padding lên file nén
	fwrite(&bitSize, sizeof(bitSize), 1, fo);
	fwrite(&padding, sizeof(padding), 1, fo);
	
	//Duyệt lại file để thay thế các kí tự thành mã code tương ứng
	FILE* fi = fopen(filePathIn.c_str(), "rb");

	//Duyệt 
	int index = 0;
	unsigned char c;

	bool temp[8];

	unsigned char* fileContent = new unsigned char[MAX_BYTE];

	fseek(fi, 0, SEEK_END);

	unsigned long long len = ftell(fi);
	unsigned char* compress = new unsigned char[MAX_BYTE];
	unsigned long long d = 0;

	fseek(fi, 0, SEEK_SET);

	//Đọc lại file cần nén và thay thành mã code tương ứng
	while (len >= MAX_BYTE) {
		fread(fileContent, MAX_BYTE, 1, fi);

		for (unsigned int idx = 0; idx < MAX_BYTE; ++idx) {
			c = fileContent[idx];

			for (int i = 0; i < *(cnt + c); ++i) {
				*(temp + index++) = *(*(bitCode + c) + i);

				if (index % 8 == 0) {
					unsigned char ch = 0;

					for (int j = 0; j < 8; ++j)
						if (temp[j])
							ch |= (128 >> j);
					
					compress[d++] = ch;
					index = 0;

					if (d == MAX_BYTE) {
						fwrite(compress, d, 1, fo);
						d = 0;
					}

				}
			}
		}
		
		len -= MAX_BYTE;
	}

	if (len > 0 && len < MAX_BYTE) {
		fread(fileContent, len, 1, fi);

		for (unsigned int idx = 0; idx < len; ++idx) {
			c = fileContent[idx];
			
			for (int i = 0; i < *(cnt + c); ++i) {
				*(temp + index++) = *(*(bitCode + c) + i);
				
				if (index % 8 == 0) {
					unsigned char ch = 0;

					for (int j = 0; j < 8; ++j)
						if (temp[j])
							ch |= (128 >> j);
					
					compress[d++] = ch;
					index = 0;

					if (d == MAX_BYTE) {
						fwrite(compress, d, 1, fo);
						d = 0;
					}
				}
			}
		}
	}

	if (padding != 0) {
		//Cộng các kí tự 0 cho đủ chia hết cho 8
		for (int i = 0; i < padding; ++i) {
			temp[index++] = 0;
		}

		unsigned char ch = 0;

		for (int j = 0; j < 8; ++j)
			if (temp[j])
				ch |= (128 >> j);
		
		compress[d++] = ch;

		if (d == MAX_BYTE) {
			fwrite(compress, d, 1, fo);
			d = 0;
		}
	}

	if (d > 0)
		fwrite(compress, d, 1, fo);

	fclose(fo);

	delete[] fileContent;
	delete[] cnt;
	delete[] compress;

	for (int i = 0; i < 256; ++i) {
		delete[] code[i];
		delete[] bitCode[i];
	}

	delete[] code;
	delete[] bitCode;
}

void File::readFileDecode(FILE*& fi, char& padding) {
	fi = fopen(filePathIn.c_str() , "rb");

	if (fi == NULL) {
		cout << "Cannot open this file extract. Please check your file and file path";
		return;
	}

	//Đọc tên của file trước khi nén
	fread(&nameFile, 50, 1, fi);

	//Đọc bảng tần số
	unsigned long long freq[256];
	fread(freq, sizeof(unsigned long long) * 256, 1, fi);

	//Gán bảng tần số vô cây huffman
	huffTree.setCharFreq(freq);

	//đọc bit size và padding
	fread(&bitSize, sizeof(bitSize), 1, fi);
	fread(&padding, sizeof(padding), 1, fi);
}

void File::writeFileDecode(FILE* fi, char padding) {
	//Mở file để giải nén
	FILE* fo = fopen((filePathOut + "\\" + nameFile).c_str(), "wb");

	//Lấy ra cây min heap
	priority_queue<HuffNode*, vector<HuffNode*>, Compare> minHeap = huffTree.getMinHeap();

	//Node gốc
	HuffNode* curr = minHeap.top();

	unsigned char* arrByte = new unsigned char[MAX_BYTE];

	unsigned long long len = bitSize / 8;

	unsigned char* fileContent = new unsigned char[MAX_BYTE];

	unsigned long long d = 0;

	//Đọc lại file cần nén và thay thành mã code tương ứng
	while (len >= MAX_BYTE) {
		fread(arrByte, MAX_BYTE, 1, fi);

		for (unsigned int idx = 0; idx < MAX_BYTE; ++idx) {
			for (int j = 0; j < 8; ++j) {
				if ((arrByte[idx] >> (7 - j)) & 1)
					curr = curr->right;
				else
					curr = curr->left;

				if (curr->left == NULL && curr->right == NULL) {
					//fwrite(&(curr->c), sizeof(curr->c), 1, fo);

					fileContent[d++] = curr->c;

					if (d == MAX_BYTE) {
						fwrite(fileContent, MAX_BYTE, 1, fo);
						d = 0;
					}

					curr = minHeap.top();
				}
			}
		}

		len -= MAX_BYTE;
	}

	if (d > 0)
		fwrite(fileContent, d, 1, fo);
	d = 0;

	if (len > 1) {
		fread(arrByte, len - 1, 1, fi);

		for (unsigned int idx = 0; idx < len - 1; ++idx) {
			for (int j = 0; j < 8; ++j) {
				if ((arrByte[idx] >> (7 - j)) & 1)
					curr = curr->right;
				else
					curr = curr->left;

				if (curr->left == NULL && curr->right == NULL) {
					//fwrite(&(curr->c), sizeof(curr->c), 1, fo);

					fileContent[d++] = curr->c;

					if (d == len - 1) {
						fwrite(fileContent, len - 1, 1, fo);
						d = 0;
					}

					curr = minHeap.top();
				}
			}
		}
	}

	if (d > 0) 
		fwrite(fileContent, d, 1, fo);

	unsigned char byte;
	fread(&byte, 1, 1, fi);
	
	for (int j = 0; j < 8 - padding; ++j) {
		if ((byte >> (7 - j)) & 1)
			curr = curr->right;
		else
			curr = curr->left;

		if (curr->left == NULL && curr->right == NULL) {

			fwrite(&(curr->c), 1, 1, fo);

			curr = minHeap.top();
		}
	}
	
	fclose(fo);

	delete[] arrByte;
	delete[] fileContent;
}

void File::process() {
	//Nén file
	if (compress == 1) {
		//Đọc file cần nén
		readFileEncode();

		//Tạo cây
		huffTree.createMinHeap();

		//Tạo mã code 
		huffTree.setCharCode(huffTree.getMinHeap().top(), "");

		//Ghi file nén
		writeFileEncode();
	}

	//Giải nén
	else {
		char padding;
		FILE* fi;
		//Đọc file nén
		readFileDecode(fi, padding);

		//Tạo cây
		huffTree.createMinHeap();

		//Tạo mã code
		huffTree.setCharCode(huffTree.getMinHeap().top(), "");
		
		//Ghi file nén
		writeFileDecode(fi, padding);

		fclose(fi);
	}
}
