
#include "HttpDownloader.h"

//-----------------------------------------------------------------------
//

HttpDownLoader::HttpDownLoader( char * url ) : internetHandle_(0)
, urlHandle_(0)
, fileSize_(-1)
{
	if( url )
	{
		// 1. set internet open
		internetHandle_ = InternetOpen ("AZITROAPPDOWN",
			INTERNET_OPEN_TYPE_PRECONFIG,
			NULL,
			NULL,
			0);

		if (internetHandle_)
		{
			// 2. open URL
			urlHandle_ = InternetOpenUrl (internetHandle_,
				url,
				NULL,
				0,
				INTERNET_FLAG_RELOAD,
				0);
		}
	}
}

//-----------------------------------------------------------------------
//

HttpDownLoader::~HttpDownLoader()
{
	if( urlHandle_ )
		InternetCloseHandle(urlHandle_);
	if( internetHandle_ )
		InternetCloseHandle(internetHandle_);
}

//-----------------------------------------------------------------------
//

int HttpDownLoader::FileSize()
{
	if( fileSize_ < 0 )
	{
		char szLengthBuf[32] = {0,};
		DWORD dwLengthBufQuery = 0;

		if( urlHandle_ )
		{
			dwLengthBufQuery = sizeof(szLengthBuf);
			HttpQueryInfo (urlHandle_, HTTP_QUERY_CONTENT_LENGTH, szLengthBuf, &dwLengthBufQuery, NULL);
			fileSize_ = atoi (szLengthBuf);
		}
	}

	return fileSize_;
}

//-----------------------------------------------------------------------
//

bool HttpDownLoader::DownLoad( char *path )
{
	bool result = false;

	if( !urlHandle_ || !path )
		return false;

	HANDLE hFile;
	DWORD dwSize;
	DWORD dwRead, dwWritten, dwRecv = 0;

	char *pDownBuf = NULL;

	if (!(pDownBuf = (char *)malloc (sizeof(char) * FileSize())))
		return 1;

	// 4. create file
	hFile = CreateFile (path,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return result;

	result = true;

	do {
		if (!InternetQueryDataAvailable (urlHandle_, &dwSize, 0, 0)) {
			result = false;
			break;
		}

		if (!InternetReadFile (urlHandle_, pDownBuf, dwSize, &dwRead)) {
			result = false;
			break;
		}

		if (!WriteFile (hFile, pDownBuf, dwRead, &dwWritten, NULL)) {
			result = false;
			break;
		}

	} while (dwRead != 0);

	CloseHandle (hFile);

	return result;
}

//-----------------------------------------------------------------------