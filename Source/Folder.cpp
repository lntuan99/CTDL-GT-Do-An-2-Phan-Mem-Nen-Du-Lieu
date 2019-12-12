#include "Folder.h"
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <direct.h>
#pragma warning(disable: 4996)

#define MAX_BYTE 1024 * 1024

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
	char name[1024];
	int i = folderPath.length() - 1;
	int j = 0;
	while (folderPath[i] != '\\') {
		name[j++] = folderPath[i--];
	}
	name[j] = '\0';
	strrev(name);

	//Ghi tên folder lên file nén
	fwrite(&name, 1024, 1, fo);

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

		unsigned long long len = s.length();
		char name[1024];

		for (int i = 0; i < len; ++i)
			name[i] = s[i];

		name[len] = '\0';
		fwrite(&name, 1024, 1, fo);

		//Đọc toàn bộ byte trong file nén
		unsigned char* fileContent = new unsigned char[MAX_BYTE];

		fseek(file, 0, SEEK_END);

		len = ftell(file);

		fseek(file, 0, SEEK_SET);

		while (len >= MAX_BYTE) {
			fread(fileContent, MAX_BYTE, 1, file);

			for (unsigned int i = 0; i < MAX_BYTE; ++i)
				charFreq[fileContent[i]]++;

			len -= MAX_BYTE;
		}

		if (len > 0) {
			fread(fileContent, len, 1, file);

			for (unsigned int i = 0; i < len; ++i)
				charFreq[fileContent[i]]++;
		}

		HuffmanTree huffTree;

		huffTree.setCharFreq(charFreq);
		huffTree.createMinHeap();
		if (huffTree.getMinHeap().size() != 0)
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

		for (int i = 0; i < 256; ++i)
			bitSize += charFreq[i] * (*(cnt + i));

		fwrite(charFreq, sizeof(unsigned long long) * 256, 1, fo);

		char padding = 0;

		while (bitSize % 8 != 0) {
			++bitSize;
			++padding;
		}

		fwrite(&bitSize, sizeof(bitSize), 1, fo);
		fwrite(&padding, sizeof(padding), 1, fo);

		//Duyệt 
		int index = 0;
		unsigned char c;

		bool temp[8];

		len = ftell(file);
		unsigned char* compress = new unsigned char[MAX_BYTE];
		unsigned long long d = 0;

		fseek(file, 0, SEEK_SET);

		if (len > 0) {
			//Đọc lại file cần nén và thay thành mã code tương ứng
			while (len >= MAX_BYTE) {
				fread(fileContent, MAX_BYTE, 1, file);

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
				fread(fileContent, len, 1, file);

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

			if (d != 0)
				fwrite(compress, d, 1, fo);
		}

		fclose(file);

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
	char folderName[1024];
	int nFile, nFolder;

	//Đọc tên folder, số lượng file trong folder, số lượng folder trong folder
	fread(&folderName, 1024, 1, fileUnCompress);
	fread(&nFile, sizeof(nFile), 1, fileUnCompress);
	fread(&nFolder, sizeof(nFolder), 1, fileUnCompress);

	//Tạo folder mới 
	mkdir((uncompressPathFile + "\\" + folderName).c_str());

	//Giải nén từng file trong folder
	for (int i = 0; i < nFile; ++i) {
		char name[1024];
		fread(&name, 1024, 1, fileUnCompress);

		FILE* file = fopen((uncompressPathFile + "\\" + folderName + "\\" + name).c_str(), "wb");

		unsigned long long freq[256];

		fread(freq, sizeof(unsigned long long) * 256, 1, fileUnCompress);

		//Tạo cây huffman
		HuffmanTree huffTree;

		huffTree.setCharFreq(freq);
		huffTree.createMinHeap();

		if (huffTree.getMinHeap().size() != 0)
			huffTree.setCharCode(huffTree.getMinHeap().top(), "");

		unsigned long long bitSize;
		char padding;

		//đọc bitsize và padding
		fread(&bitSize, sizeof(bitSize), 1, fileUnCompress);
		fread(&padding, sizeof(padding), 1, fileUnCompress);

		//Lấy ra cây min heap
		priority_queue<HuffNode*, vector<HuffNode*>, Compare> minHeap = huffTree.getMinHeap();

		//Node gốc
		HuffNode* curr = NULL;
		if (minHeap.size() != 0)
			curr = minHeap.top();

		unsigned char* arrByte = new unsigned char[MAX_BYTE];

		unsigned long long len = bitSize / 8;

		unsigned char* fileContent = new unsigned char[MAX_BYTE];

		unsigned long long d = 0;

		if (len > 0) {
			//Đọc lại file cần nén và thay thành mã code tương ứng
			while (len >= MAX_BYTE) {
				fread(arrByte, MAX_BYTE, 1, fileUnCompress);

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
								fwrite(fileContent, MAX_BYTE, 1, file);
								d = 0;
							}

							curr = minHeap.top();
						}
					}
				}

				len -= MAX_BYTE;
			}

			if (d > 0)
				fwrite(fileContent, d, 1, file);
			d = 0;

			if (len > 1) {
				fread(arrByte, len - 1, 1, fileUnCompress);

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
								fwrite(fileContent, len - 1, 1, file);
								d = 0;
							}

							curr = minHeap.top();
						}
					}
				}
			}

			if (d > 0)
				fwrite(fileContent, d, 1, file);

			unsigned char byte;
			fread(&byte, 1, 1, fileUnCompress);

			if (huffTree.getFlag()) {
				fwrite(&(curr->c), 1, 1, file);
			}

			else
				for (int j = 0; j < 8 - padding; ++j) {
					if ((byte >> (7 - j)) & 1)
						curr = curr->right;
					else
						curr = curr->left;

					if (curr->left == NULL && curr->right == NULL) {

						fwrite(&(curr->c), 1, 1, file);

						curr = minHeap.top();
					}
				}
		}

		fclose(file);

		delete[] arrByte;
		delete[] fileContent;
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