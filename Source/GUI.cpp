#include "GUI.h"
#pragma warning(disable: 4996)

void GUI::brand() {
	cout << "********************************************************\n";
	cout << "*      DO AN 2 - CTDL & GT - PHAN MEM NEN DU LIEU      *\n";
	cout << "*              CLASS      : 18CTT5C                    *\n";
	cout << "*              AUTHOR     : LE NHAT TUAN               *\n";
	cout << "*              STUDENT ID : 18120632                   *\n";
	cout << "*              INSTRUCTOR : MR. NGUYEN THANH AN        *\n";
	cout << "********************************************************\n";
}

void GUI::mainScene() {
	while (1) {
		system("cls");
		brand();
		int choice = -1;

		cout << "\t\t\tMENU\n";

		cout << "\t1: Compess file" << endl;
		cout << "\t2: Compres folder" << endl;
		cout << "\t3: Extract file" << endl;
		cout << "\t4: Extract folder" << endl;
		cout << "\t0: Exit" << endl;

		do {
			cout << "\n\tEnter your selection: ";
			cin >> choice;
		} while (choice < 0 || choice > 4);

		if (choice != 0) {
			switch (choice) {
			case 1:
				system("cls");
				brand();
				compressFile();
				choice = -1;
				break;

			case 2:
				system("cls");
				brand();
				compressFolder();
				choice = -1;
				break;

			case 3:
				system("cls");
				brand();
				extractFile();
				choice = -1;
				break;

			case 4:
				system("cls");
				brand();
				extractFolder();
				choice = -1;
				break;
			}
		}
		else
			break;
	}
}

void GUI::compressFile() {
	time_t t;

	string file, compress;

	rewind(stdin);
	cout << "Enter file path to the file needs to be compressed: ";
	getline(cin, file);

	rewind(stdin);
	cout << "Enter file path to the folder contains file has just compressed: ";
	getline(cin, compress);

	cout << "Processing... please wait a minute!\n";
	File fileEncode(file, compress, 1);
	t = clock();
	fileEncode.process();
	cout << "Time compress: " << (double)(clock() - t) / CLOCKS_PER_SEC << endl;

	cout << "Press Enter to continue!!";
	while (_getch() != 13);
}

void GUI::compressFolder() {
	Folder folder;

	folder.getFolderPath();
	folder.readFolder();

	string path;
	rewind(stdin);
	cout << "Enter file path to the file compress result: ";
	getline(cin, path);

	FILE* fo = fopen(path.c_str(), "wb");

	time_t t;
	cout << "Processing... please wait a minute!\n";
	t = clock();
	folder.writeHeader(fo);
	folder.compress(fo);
	fclose(fo);
	cout << "Time compress: " << (double)(clock() - t) / CLOCKS_PER_SEC << endl;

	cout << "Press Enter to continue!!";
	while (_getch() != 13);
}

void GUI::extractFile() {
	time_t t;

	string file, extract;

	rewind(stdin);
	cout << "Enter file path to the file needs to be extracted: ";
	getline(cin, extract);

	rewind(stdin);
	cout << "Enter file path to the folder contains file has just extracted: ";
	getline(cin, file);

	cout << "Processing... please wait a minute!\n";

	t = clock();
	File fileDecode(extract, file, 2);

	fileDecode.process();
	cout << "Time extract: " << (double)(clock() - t) / CLOCKS_PER_SEC << endl;

	cout << "Press Enter to continue!!";
	while (_getch() != 13);
}

void GUI::extractFolder() {
	Folder folder;

	string uncompressName;
	rewind(stdin);
	cout << "Enter file path to the file needs to be extracted: ";
	getline(cin, uncompressName);

	FILE* fileUnCompress = fopen(uncompressName.c_str(), "rb");
	if (fileUnCompress == NULL) {
		cout << "Cannot open this file!!";
		return;
	}

	time_t t;
	
	folder.getFileUnCompress();
	cout << "Processing... please wait a minute!\n";

	t = clock();
	folder.uncompress(fileUnCompress);
	fclose(fileUnCompress);
	cout << "Time extract: " << (double)(clock() - t) / CLOCKS_PER_SEC << endl;
	cout << "Press Enter to continue!!";
	while (_getch() != 13);
}