#pragma once
#include <string>
#include <vector>
#include "dirent.h"
#include "File.h"
#include "HuffmanTree.h"

using namespace std;

class Folder
{
private:
	string folderPath;
	string compressName;
	string uncompressName;
	string uncompressPathFile;
	
	vector<string> listFileName; //Danh sách File trong folder. lưu tên làm đặc trưng 
	vector<Folder*> listFolder; //Danh sách folder trong folder. lưu tên làm đặc trưng

	int nFile; //Số lượng file trong folder
	int nFolder; //Số lượng folder trong folder

public:
	Folder();
	Folder(string);

	//Ghi phần header của file nén
	void writeHeader(FILE*);

	//Lấy đường dẫn đến folder
	void getFolderPath();

	//Đọc folder
	void readFolder();

	//Lấy đường dẫn đến nơi chứa file nén
	void getFileUnCompress();

	//Nén folder
	void compress(FILE*);

	//Giải nén folder
	void uncompress(FILE*);
};

