// RecvDebugInfoSvr.cpp : Defines the entry point for the console application.
//

#include <afx.h>

#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <Psapi.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>
#include <tchar.h>
#include <string>

using namespace std;

BOOL SafeTerminateProcess( HANDLE hProcess, UINT uExitCode )
{
	DWORD dwTID, dwCode, dwErr = 0;
	HANDLE hProcessDup = INVALID_HANDLE_VALUE;
	HANDLE hCurProcess = GetCurrentProcess();
	BOOL bDup = DuplicateHandle( hCurProcess, hProcess, hCurProcess, &hProcessDup, PROCESS_ALL_ACCESS, FALSE, 0 );
	if( !bDup )
	{
		CloseHandle( hProcessDup );
		return FALSE;
	}

	HANDLE hRT = NULL;
	if( GetExitCodeProcess( bDup? hProcessDup : hProcess, &dwCode ) && STILL_ACTIVE == dwCode  )
	{
		FARPROC pfnExitProc = GetProcAddress( GetModuleHandle( _T("Kernel32" ) ), "ExitProcess" );
		hRT = CreateRemoteThread( bDup ? hProcessDup : hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnExitProc, (PVOID)uExitCode, 0, &dwTID);
	}

	if ( hRT )
	{
		WaitForSingleObject( bDup ? hProcessDup : hProcess, INFINITE );
		CloseHandle( hProcessDup );
		CloseHandle( hRT );

		return TRUE;
	}

	CloseHandle( hProcessDup );
	CloseHandle( hRT );

	return FALSE;
}

void ExitProcessName( string strProcessName, DWORD dwMyProcessID ) 
{
	DWORD aProcesses[1024], cbNeeded;
	if( !EnumProcesses( aProcesses, sizeof(DWORD) * 1024, &cbNeeded ) )	return;

	DWORD dwProcessesCount = cbNeeded / sizeof(DWORD);

	for( DWORD i = 0; i < dwProcessesCount; i++ )
	{
		TCHAR szProcessName[MAX_PATH] = _T( "<unknown>" );
		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i] );
		if( hProcess )
		{
			HMODULE	hMod;
			DWORD	cbNeeded;
			if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
				GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
		}

		if( !_tcscmp( szProcessName, strProcessName.c_str() ) && aProcesses[i] != dwMyProcessID )
		{
			HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, aProcesses[i]);
			if( hProcess )
			{        
				DWORD    ExitCode = 0;
				GetExitCodeProcess(hProcess, &ExitCode);
				if( SafeTerminateProcess( hProcess, ExitCode ) )
					::WaitForSingleObject( hProcess, INFINITE );

				::CloseHandle( hProcess );
			}

			// 다름 프로세스를 발견하면 죽이고 로그를 남김.
			printf( "Debug process found in memory .. kill them now\n" );
		}

		::CloseHandle( hProcess );
	}
}

int main(int argc, char* argv[])
{
	// 2005.03.19. steeple
	// For Client, For Server 변경
	BOOL bForServer = FALSE;

	INT32 lPort = 10001;
	CHAR* szBaseDirectory = "\\DebugInfo";
	if(bForServer)
	{
		lPort = 10002;
		szBaseDirectory = "\\ServerDebugInfo";
		printf( "Debug Info Server , for Server\n" );
	}
	else
	{
		printf( "Debug Info Server , for Client\n" );
	}

	printf( "------------------------------\n" );

	// 기존에 떠있으면 날리는 코드 적용.
	{
		TCHAR szModuleName[MAX_PATH];
		GetModuleFileName( NULL, szModuleName, MAX_PATH );

		string strModuleName = szModuleName;
		string strNewName( strModuleName.begin() + strModuleName.find_last_of( _T( "\\" ) ) + 1, strModuleName.end() );

		ExitProcessName( strNewName, GetCurrentProcessId() );
	}

	printf( "------------------------------\n" );

   // Initialize Winsock.
    WSADATA wsaData;
    int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
    if ( iResult != NO_ERROR )
        printf("Error at WSAStartup()\n");

    // Create a socket.
    SOCKET m_socket;
    m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( m_socket == INVALID_SOCKET ) {
        printf( "Error at socket(): %ld\n", WSAGetLastError() );
        WSACleanup();
        return 0;
    }

    // Bind the socket.
    sockaddr_in service;

    service.sin_family		= AF_INET;
    service.sin_addr.s_addr	= htonl(INADDR_ANY);
    service.sin_port		= htons(lPort);		// 10001 : For Client. 10002 : For Server

    if ( bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) {
        printf( "bind() failed.\n" );
        closesocket(m_socket);
		getch();
        return 0;
    }
    
    // Listen on the Socket.
    if ( listen( m_socket, 1 ) == SOCKET_ERROR )
        printf( "Error listening on socket.\n");

    // Accept connections.
    SOCKET AcceptSocket;

    printf( "Waiting for a client to connect...\n" );

	for ( ; ; ) {
		while (1) {
			AcceptSocket = SOCKET_ERROR;
			while ( AcceptSocket == SOCKET_ERROR ) {
				AcceptSocket = accept( m_socket, NULL, NULL );
			}
			printf( "Client Connected.\n");
			//m_socket = AcceptSocket; 
			break;
		}
    
		int bytesRecv = 0;

		int dataLength = 0;
		int totalRecvBytes = 0;

		while (totalRecvBytes < 4)
		{
			bytesRecv = recv( AcceptSocket, (char *) &dataLength + totalRecvBytes, 4 - totalRecvBytes, 0 );

			if (bytesRecv <= 0)
				break;
			else
				totalRecvBytes += bytesRecv;
		}

		printf("	Received DebugInfo (size : %d)\n", dataLength);
		
		char* data = new char[dataLength];

		bytesRecv = 0;

		totalRecvBytes = 0;

		while (totalRecvBytes < dataLength)
		{
			bytesRecv = recv( AcceptSocket, (char *) data + totalRecvBytes, dataLength - totalRecvBytes, 0 );

			if (bytesRecv <= 0)
				break;
			else
				totalRecvBytes += bytesRecv;
		}

		CHAR	szDestFileName[512], szBuf[64];
		ZeroMemory(szDestFileName, sizeof(CHAR) * 512);
		ZeroMemory(szBuf, sizeof(CHAR) * 64);

		SYSTEMTIME	stSystemTime;
		GetSystemTime(&stSystemTime);

		FILETIME	stFileTime;
		SystemTimeToFileTime(&stSystemTime, &stFileTime);

		sprintf(szDestFileName, "%s", "C:\\Archlord");
		::CreateDirectory(szDestFileName, NULL);

		sprintf(szBuf, "%s", szBaseDirectory);
		strcat(szDestFileName, szBuf);

		//::CreateDirectory("C:\\Archlord\\ServerDebugInfo", NULL);
		//sprintf(szDestFileName, "C:\\Archlord\\ServerDebugInfo\\%d%02d%02d", stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay);
		sprintf(szBuf, "\\%d%02d%02d", stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay);
		strcat(szDestFileName, szBuf);
		::CreateDirectory(szDestFileName, NULL);

		// 2005.03.20. steeple
		// 다시 없앴다.
		// 2005.03.19. steeple
		// 시간대별로 남긴다.
		//time_t timeval;
		//(void)time(&timeval);
		//
		//struct tm* tm_ptr;
		//tm_ptr = localtime(&timeval);

		//strftime(szBuf, 32, "\\%H", tm_ptr);

		//strcat(szDestFileName, szBuf);
  //      ::CreateDirectory(szDestFileName, NULL);

		sockaddr_in	stSockAddr;
		int	nAddressSize	= sizeof(stSockAddr);
		getpeername(AcceptSocket, (struct sockaddr *) &stSockAddr, &nAddressSize);

		//sprintf(szDestFileName, "C:\\Archlord\\ServerDebugInfo\\%d%02d%02d\\%d.%d.%d.%d", stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay, stSockAddr.sin_addr.S_un.S_un_b.s_b1, stSockAddr.sin_addr.S_un.S_un_b.s_b2, stSockAddr.sin_addr.S_un.S_un_b.s_b3, stSockAddr.sin_addr.S_un.S_un_b.s_b4);
		sprintf(szBuf, "\\%d.%d.%d.%d", stSockAddr.sin_addr.S_un.S_un_b.s_b1, stSockAddr.sin_addr.S_un.S_un_b.s_b2, stSockAddr.sin_addr.S_un.S_un_b.s_b3, stSockAddr.sin_addr.S_un.S_un_b.s_b4);
		strcat(szDestFileName, szBuf);
		::CreateDirectory(szDestFileName, NULL);

		//sprintf(szDestFileName, "C:\\Archlord\\ServerDebugInfo\\%d%02d%02d\\%d.%d.%d.%d\\d%d.txt", stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay, stSockAddr.sin_addr.S_un.S_un_b.s_b1, stSockAddr.sin_addr.S_un.S_un_b.s_b2, stSockAddr.sin_addr.S_un.S_un_b.s_b3, stSockAddr.sin_addr.S_un.S_un_b.s_b4, (UINT32) stFileTime.dwLowDateTime, (UINT32) stFileTime.dwHighDateTime);
		sprintf(szBuf, "\\d%d.txt", (UINT32) stFileTime.dwLowDateTime, (UINT32) stFileTime.dwHighDateTime);
		strcat(szDestFileName, szBuf);

		//printf("%s\n", szDestFileName);

		HANDLE	hFile;

		hFile	= ::CreateFile(szDestFileName, 
								GENERIC_WRITE,                // open for writing 
								0,                            // do not share 
								NULL,                         // no security 
								OPEN_ALWAYS,                  // open or create 
								FILE_ATTRIBUTE_NORMAL,        // normal file 
								NULL);                        // no attr. template 

		DWORD	dwLastErrorCode = GetLastError();

		if (hFile == INVALID_HANDLE_VALUE) 
		{ 
			printf("Could not create log file.\n");    // process error 
		} 
 
		DWORD	dwBytesWritten = 0;
		while (dwBytesWritten < (DWORD) dataLength)
		{
			::WriteFile(hFile, data, dataLength, &dwBytesWritten, NULL);
		}

		::CloseHandle(hFile);

		delete data;

		//::closesocket(m_socket);
	}

    return 0;
}