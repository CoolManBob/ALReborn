#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <WinInet.h>

//-----------------------------------------------------------------------
//

class HttpDownLoader
{
	HINTERNET internetHandle_;
	HINTERNET urlHandle_;
	int fileSize_;

public:
	HttpDownLoader( char * url );

	~HttpDownLoader();

	int FileSize();

	bool DownLoad(char *path);
};

//-----------------------------------------------------------------------