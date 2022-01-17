// MD5ConvertCmd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../Alef/Modules/AUtility/AuMD5EnCrypt/AuMD5Encrypt.h"

#include <fstream>
using namespace std;

#define HashKey "1111"

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		printf("input file name...\r\n");
		return 0;
	}

	printf("%s", argv[1]);

	fstream file;
	string filename = argv[1];
	file.open(filename.c_str(), ios::in | ios::out | ios::binary);
	if(!file) return NULL;

	file.seekg(0, ios::end);
	size_t size = file.tellg();
	char* buffer = new char[size];

	file.seekg(0, ios::beg);
	file.read(buffer, (unsigned int)size);

	AuMD5Encrypt crypt;
	crypt.DecryptString(HashKey, buffer, (unsigned int)size);

	file.seekp(0, ios::beg);
	file.write(buffer, (unsigned int)size);
	file.close();

	if(buffer)
		delete[] buffer;

	return 0;
}

