#include "Folder.h"
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <direct.h>
#pragma warning(disable: 4996)

Folder::Folder() {
	folderPath = compressName = uncompressName = "";
	nFile = nFolder = 0;
}

Folder::Folder(string path) {
	folderPath = path;
	nFile = nFolder = 0;
}

void Folder::getFolderPath() {
	string path;
	
	//Lấy đường dẫn đến folder cần nén
	rewind(stdin);
	cout << "Enter file path to the folder needs to be compressed: ";
	getline(cin, path);

	if (path[path.length() - 1] == '\\')
		path = path.substr(0, path.length() - 1);

	//Check đường dẫn đúng hay sai
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(path.c_str())) == NULL) {
		cout << "Cannot open this folder!!!!";
		return;
	}

	folderPath = path;
}

void Folder::readFolder() {
	//Đọc toàn bộ folder
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(folderPath.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			fstream f(folderPath + "\\" + ent->d_name, ios::in);

			if (f.is_open()) {
				listFileName.push_back((ent->d_name));
				++nFile;
				f.close();
			}
			else {
				string temp(ent->d_name);
				if (temp.compare(".") != 0 && temp.compare("..") != 0) {
					listFolder.push_back(new Folder(folderPath + "\\" + ent->d_name));
					++nFolder;
				}
			}
			
		}
		closedir(dir);
	}
}

void Folder::writeHeader(FILE* fo) {
	//Lấy ra tên folder
	char name[50];
	int i = folderPath.length() - 1;
	int j = 0;
	while (folderPath[i] != '\\') {
		name[j++] = folderPath[i--];
	}
	name[j] = '\0';
	strrev(name);

	//Ghi tên folder lên file nén
	fwrite(&name, 50, 1, fo);
	
	//Ghi số lượng file trong folder lên file nén
	fwrite(&nFile, sizeof(nFile), 1, fo);

	//Ghi số lượng folder trong folder lên file nén
	fwrite(&nFolder, sizeof(nFolder), 1, fo);
}

void Folder::compress(FILE* fo) {
	//Nén toàn bộ file trong folder
	for (auto s : listFileName) {
		unsigned long long charFreq[256];
		memset(charFreq, 0, 256 * sizeof(unsigned long long));

		FILE* file = fopen((folderPath + "\\" + s).c_str(), "rb");
	
		int len = s.length();
		char name[50];

		for (int i = 0; i < len; ++i)
			name[i] = s[i];

		name[len] = '\0';
		fwrite(&name, 50, 1, fo);

		unsigned long long size = 0;

		unsigned char c;
		fread(&c, 1, 1, file);
		while (!feof(file)) {
			++size;
			++charFreq[c];
			fread(&c, 1, 1, file);
		}

		HuffmanTree huffTree;

		huffTree.setCharFreq(charFreq);
		huffTree.createMinHeap();
		huffTree.setCharCode(huffTree.getMinHeap().top(), "");

		int* cnt = new int[256];

		for (int i = 0; i < 256; ++i)
			* (cnt + i) = huffTree.getCharCode(i).length();

		char** code = huffTree.getAllCharCode();

		bool** bitCode = new bool* [256];

		for (int i = 0; i < 256; ++i) {
			bitCode[i] = new bool[*(cnt + i)];

			for (int j = 0; j < *(cnt + i); ++j) {
				*(*(bitCode + i) + j) = *(*(code + i) + j) - '0';
			}
		}

		unsigned long long bitSize = 0;

		for (int i = 0; i < 256; ++i) {
			unsigned long long w = charFreq[i];
			bitSize += w * (*(cnt + i));
			fwrite(&w, sizeof(unsigned long long), 1, fo);
		}

 		char padding = 0;

		while (bitSize % 8 != 0) {
			++bitSize;
			++padding;
		}

		bool* temp = new bool[bitSize];

		fwrite(&bitSize, sizeof(bitSize), 1, fo);
		fwrite(&padding, sizeof(padding), 1, fo);

		int index = 0;

		fseek(file, 0, SEEK_SET);
		fread(&c, 1, 1, file);
		while (!feof(file)) {
			for (int i = 0; i < *(cnt + c); ++i)
				* (temp + index++) = *(*(bitCode + c) + i);

			fread(&c, 1, 1, file);
		}

		fclose(file);

		for (int i = 0; i < padding; ++i)
			* (temp + index++) = 0;

		for (unsigned long long i = 0; i < bitSize; i += 8) {
			unsigned char c = 0;

			for (int j = 0; j < 8; ++j)
				if (temp[i + j])
					c |= (128 >> j);

			fwrite(&c, sizeof(c), 1, fo);
		}

		delete[] cnt;
		delete[] temp;

		for (int i = 0; i < 256; ++i) {
			delete code[i];
			delete bitCode[i];
		}

		delete[] code;
		delete[] bitCode;
	}
	
	//Đệ quy đọc folder trong folder r ghi lên file nén
	for (auto folder : listFolder) {
		folder->readFolder();
		folder->writeHeader(fo);
		folder->compress(fo);
	}
}

void Folder::getFileUnCompress() {
	rewind(stdin);
	cout << "Enter file path to the folder contains the folder has just extracted: ";
	getline(cin, uncompressPathFile);
}

void Folder::uncompress(FILE* fileUnCompress) {
	char folderName[50];
	int nFile, nFolder;
	
	//Đọc tên folder, số lượng file trong folder, số lượng folder trong folder
	fread(&folderName, 50, 1, fileUnCompress);
	fread(&nFile, sizeof(nFile), 1, fileUnCompress);
	fread(&nFolder, sizeof(nFolder), 1, fileUnCompress);

	//Tạo folder mới 
	mkdir((uncompressPathFile + "\\" + folderName).c_str());

	//Giải nén từng file trong folder
	for (int i = 0; i < nFile; ++i) {
		char name[50];
		fread(&name, 50, 1, fileUnCompress);
		
		FILE* file = fopen((uncompressPathFile + "\\" + folderName + "\\" + name).c_str(), "wb");

		unsigned long long freq[256];
		for (int i = 0; i < 256; ++i) {
			unsigned long long w;
			fread(&w, sizeof(unsigned long long), 1, fileUnCompress);
			freq[i] = w;
		}

		HuffmanTree huffTree;

		huffTree.setCharFreq(freq);
		huffTree.createMinHeap();
		huffTree.setCharCode(huffTree.getMinHeap().top(), "");

		unsigned long long bitSize;
		char padding;

		fread(&bitSize, sizeof(bitSize), 1, fileUnCompress);
		fread(&padding, sizeof(padding), 1, fileUnCompress);

		unsigned char* arrByte = new unsigned char[bitSize / 8];

		fread(arrByte, bitSize / 8, 1, fileUnCompress);

		bool* res = new bool[bitSize - padding];
		int index = 0;
		for (int i = 0; i < bitSize / 8 - 1; ++i)
			for (int j = 0; j < 8; ++j)
				* (res + index++) = (arrByte[i] >> (7 - j)) & 1;

		int temp = 8 - padding;
		for (int i = 0; i < temp; ++i)
			* (res + index++) = (arrByte[bitSize / 8 - 1] >> (7 - i)) & 1;

		priority_queue<HuffNode*, vector<HuffNode*>, Compare> minHeap = huffTree.getMinHeap();
		HuffNode* curr = minHeap.top();

		for (int i = 0; i < bitSize - padding; ++i) {
			if (*(res + i))
				curr = curr->right;
			else
				curr = curr->left;

			if (curr->left == NULL && curr->right == NULL) {
				fwrite(&(curr->c), sizeof(curr->c), 1, file);
				curr = minHeap.top();
			}
		}
		
		fclose(file);
		delete[] arrByte;
		delete[] res;
	}
	
	//Đệ quy để giải nén folder trong folder
	string temp = uncompressPathFile;
	uncompressPathFile += "\\";
	uncompressPathFile += folderName;

	for (int i = 0; i < nFolder; ++i) {
		uncompress(fileUnCompress); 
	}

	uncompressPathFile = temp;
}