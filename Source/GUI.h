#pragma once
#include "dirent.h"
#include "File.h"
#include "Folder.h"
#include "HuffmanTree.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <ctime>
#include <conio.h>

using namespace std;

class GUI
{
public:
	void brand();
	
	//Màn hình chính
	void mainScene();

	void compressFile();

	void extractFile();

	void compressFolder();

	void extractFolder();
};

