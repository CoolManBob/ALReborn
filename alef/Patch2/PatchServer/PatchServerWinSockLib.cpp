#include "PatchServerWinSockLib.h"
#include <stdio.h>

CWinSockLib::CWinSockLib()
{
	m_bBind = false;
	m_bConnect = false;
}

CWinSockLib::~CWinSockLib()
{
	cleanupWinsock();
}

bool CWinSockLib::startupWinsock( unsigned short iVersion )
{
	bool		bResult;

	if( WSAStartup (0x0202, &m_cWsaData) == SOCKET_ERROR )
	{
		bResult = false;
	}
	else
	{
		bResult = true;
	}

	return bResult;
}

bool CWinSockLib::createSocket( bool bOverlapped )
{
	SOCKET			hTempSocket;

	bool			bResult;
	DWORD			iFlag;

	iFlag = 0;

	if( bOverlapped == true )
	{
		iFlag = WSA_FLAG_OVERLAPPED;
	}

    hTempSocket = WSASocket(AF_INET, SOCK_STREAM,0, NULL, 0, iFlag );

	if( hTempSocket != INVALID_SOCKET )
	{
		m_hSocket = hTempSocket;
		bResult = true;
	}
	else
	{
		//Socket 생성 실패....
        WSACleanup();
		bResult = false;
	}

	return bResult;
}

bool CWinSockLib::bindSocket( char *pstrAddress, unsigned short iPort )
{
	bool			bResult;

	m_cBindAddress.sin_family = AF_INET;
    m_cBindAddress.sin_port = htons( iPort );  //포트번호

	if( pstrAddress == NULL )
	{
		m_cBindAddress.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		m_cBindAddress.sin_addr.s_addr = inet_addr( pstrAddress );
	}

    if( bind(m_hSocket,(struct sockaddr*)&m_cBindAddress,sizeof( m_cBindAddress) ) == SOCKET_ERROR ) 
    {
        closesocket( m_hSocket );
        WSACleanup();
		bResult = false;
    }
	else
	{
		bResult = true;
	}

	return bResult;
}

bool CWinSockLib::listenSocket( int iPendingConnection = 0 )
{
	bool		bResult;

	if( iPendingConnection == 0 )
	{
		iPendingConnection = SOMAXCONN;
	}

    if( listen(m_hSocket,iPendingConnection) == SOCKET_ERROR ) 
    {
        closesocket( m_hSocket );
        WSACleanup();
		bResult = false;
    }
	else
	{
		bResult = true;
	}

	return bResult;
}

bool CWinSockLib::connectSocket( char *pstrAddress, unsigned short iPort )
{
	bool		bResult;

	m_cConnectAddress.sin_family = AF_INET;
    m_cConnectAddress.sin_addr.s_addr = inet_addr( pstrAddress ); 
    m_cConnectAddress.sin_port = htons( iPort );

	if( connect( m_hSocket, (struct sockaddr*)&m_cConnectAddress, sizeof(m_cConnectAddress) ) != SOCKET_ERROR )
	{
		bResult = true;
	}
	else
	{
		bResult = false;
	}

	return bResult;
}


bool CWinSockLib::initBind( bool bOverlapped, char *pstrAddress, unsigned short iPort, int iPendingConnection )
{
	bool		bResult;

	bResult = createSocket( bOverlapped );

	if( bResult == false )
		printf( "Create Socker Error!\n" );

	if( bResult == true )
	{
		bResult = bindSocket( pstrAddress, iPort );

		if( bResult == false )
			printf( "Bind Error. IP:%s, Port:%d!\n", pstrAddress, iPort );
	}

	if( bResult == true )
	{
		bResult = listenSocket( iPendingConnection );

		if( bResult == false )
		{
			printf( "ListenSocket Error. %d\n", iPendingConnection );
		}
	}

	return bResult;
}

bool CWinSockLib::initConnect( unsigned short iVer, bool bOverlapped, char *pstrAddress, unsigned short iPort )
{
	bool		bResult;

	bResult = startupWinsock( iVer );

	if( bResult == true )
		bResult = createSocket( bOverlapped );

	if( bResult == true )
		bResult = connectSocket( pstrAddress, iPort );

	return bResult;
}

SOCKET CWinSockLib::acceptSocket()
{
	SOCKET			hClientSocket;
	int				iAddressSize;

	iAddressSize = sizeof( m_cAcceptClientAddress );

	hClientSocket = accept( m_hSocket,(struct sockaddr*)&m_cAcceptClientAddress, &iAddressSize );

	return hClientSocket;
}

void CWinSockLib::cleanupWinsock()
{
	closesocket( m_hSocket );
	WSACleanup();
}
