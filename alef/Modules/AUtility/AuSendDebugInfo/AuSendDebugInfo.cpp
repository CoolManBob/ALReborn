
#include "AuSendDebugInfo.h"

#include "ApMemoryTracker.h"



AuSendDebugInfo::AuSendDebugInfo()
{
}

AuSendDebugInfo::~AuSendDebugInfo()
{
}

//@{ Jaewon 20041026
// send memory data version
BOOL AuSendDebugInfo::SendDebugInfoMemory(CHAR *szServerIPAddr, UINT32 ulPort, UINT8 *pData, UINT32 ulSize)
{
	if (!szServerIPAddr ||
		!strlen(szServerIPAddr) ||
		!pData ||
		!ulSize)
		return FALSE;

	WORD		wVersionRequested;
	WSADATA		wsaData;
	int			err;
 
	wVersionRequested = MAKEWORD( 2, 2 );
 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return FALSE;
	}
 
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
 
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
			HIBYTE( wsaData.wVersion ) != 2 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup( );
		return FALSE; 
	}
 
	/* The WinSock DLL is acceptable. Proceed. */

	SOCKET	socket	= ::socket(AF_INET, SOCK_STREAM, 0);
	if (socket == INVALID_SOCKET)
	{
		WSACleanup( );
		return FALSE;
	}

	SOCKADDR_IN		stServerAddr;

	stServerAddr.sin_family			= AF_INET;
	stServerAddr.sin_addr.s_addr	= inet_addr(szServerIPAddr);
	stServerAddr.sin_port			= htons(ulPort);

	INT32 lResult = ::connect(socket, (struct sockaddr*)&stServerAddr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == lResult)
	{
		INT32 lError = ::WSAGetLastError();
		if (WSAEWOULDBLOCK != lError) 
		{
			WSACleanup( );
			return FALSE;
		}
	}

	if (!SendData(socket, &ulSize, sizeof(ulSize)))
	{
		WSACleanup();
		return FALSE;
	}

	if (!SendData(socket, (PVOID) pData, ulSize))
	{
		WSACleanup();
		return FALSE;
	}

	::closesocket(socket);

	WSACleanup();

	return TRUE;
}

BOOL AuSendDebugInfo::SendDebugInfoFile(CHAR *szServerIPAddr, UINT32 ulPort, CHAR *szDebugFileName)
{
	if (!szServerIPAddr ||
		!strlen(szServerIPAddr) ||
		!szDebugFileName ||
		!strlen(szDebugFileName))
		return FALSE;

	OFSTRUCT	stOpenFile;
	ZeroMemory(&stOpenFile, sizeof(stOpenFile));

	HANDLE	hFile		= (HANDLE) OpenFile(szDebugFileName, &stOpenFile, OF_READ);
	if ((HFILE) hFile == HFILE_ERROR)
	{
		return FALSE;
	}

	UINT32	lFileSize	= GetFileSize(hFile, NULL);
	if (lFileSize <= 0)
	{
		::_lclose((HFILE) hFile);
		return FALSE;
	}

	byte*	data	= new byte[lFileSize];
	ZeroMemory(data, sizeof(byte) * lFileSize);

	DWORD	dwReadBytes	= 0;

	if (!ReadFile(hFile, (LPVOID) data, lFileSize, &dwReadBytes, NULL))
	{
		::_lclose((HFILE) hFile);
		return FALSE;
	}

	::_lclose((HFILE) hFile);

	return SendDebugInfoMemory(szServerIPAddr, ulPort, data, lFileSize);
}
//@} Jaewon

BOOL AuSendDebugInfo::SendData(SOCKET socket, PVOID pvData, UINT32 ulLength)
{
	if (!pvData || ulLength < 1)
		return FALSE;

	UINT32	lSendLength	= 0;

	char*	pBufferOffset	= (char *) pvData;

	while (lSendLength < ulLength)
	{
		UINT32 lLength	= ::send(socket, (const char *) pBufferOffset, ulLength - lSendLength, 0);

		if (lLength == 0)
		{
			WSACleanup();
			return FALSE;
		}

		lSendLength += lLength;

		if (lSendLength < ulLength)
			pBufferOffset	= (char *) pvData + lSendLength;
	}

	return TRUE;
}