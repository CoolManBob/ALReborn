#include "stdafx.h"
#include <Imagehlp.h>
#include <Mmsystem.h>



#include "AuPackingManager.h"
#include "ClientNetworkLib.h"
#include "AutoDetectMemoryLeak.h"

extern wchar_t* g_szMessageTitle;

bool		CWinSockLib::m_bStartup	= false;
WSADATA		CWinSockLib::m_cWsaData;
int			CWinSockLib::m_lSockNum = 0;

CWinSockLib::CWinSockLib()
{
	m_lErrorCode	= 0;
	m_bBind			= false;
	m_bConnect		= false;
	m_hSocket		= NULL;
}

CWinSockLib::~CWinSockLib()
{
	cleanupWinsock();
}

bool CWinSockLib::startupWinsock( unsigned short iVersion )
{	
	if ( m_bStartup )		return true;

	if( SOCKET_ERROR == WSAStartup(0x0202, &m_cWsaData)  )
		return false;

	m_bStartup	= true;

	return true;
}

bool CWinSockLib::createSocket( bool bOverlapped )
{
	SOCKET	hTempSocket = WSASocket(AF_INET, SOCK_STREAM,0, NULL, 0, bOverlapped ? WSA_FLAG_OVERLAPPED : 0 );
	if( hTempSocket != INVALID_SOCKET )
	{
		m_hSocket	= hTempSocket;
		++m_lSockNum;

		return true;
	}

    WSACleanup();
	return false;
}

bool CWinSockLib::bindSocket( char *pstrAddress, unsigned short iPort )
{
	m_cBindAddress.sin_family		= AF_INET;
    m_cBindAddress.sin_port			= htons( iPort );  //포트번호
	m_cBindAddress.sin_addr.s_addr	= pstrAddress ? inet_addr( pstrAddress ) : INADDR_ANY;

    if( SOCKET_ERROR != bind( m_hSocket,(sockaddr*)&m_cBindAddress, sizeof(m_cBindAddress) ) ) 
		return true;

	cleanupWinsock();
	return false;
}

bool CWinSockLib::listenSocket( int iPendingConnection = 0 )
{
	if( SOCKET_ERROR != listen( m_hSocket, iPendingConnection ? iPendingConnection : SOMAXCONN ) ) 
		return true;

	cleanupWinsock();
	return false;
}

int CWinSockLib::connectSocket( char *pstrAddress, unsigned short iPort )
{
	m_cConnectAddress.sin_family		= AF_INET;
	m_cConnectAddress.sin_port			= htons( iPort );
	m_cConnectAddress.sin_addr.s_addr	= inet_addr(pstrAddress);

	if (INADDR_NONE == m_cConnectAddress.sin_addr.s_addr)
		m_cConnectAddress.sin_addr.s_addr = inet_addr(GetIPFromHost(pstrAddress));

	if( INADDR_NONE == m_cConnectAddress.sin_addr.s_addr )
		return g_lConnectError_DNS;

	if( SOCKET_ERROR != connect( m_hSocket, (struct sockaddr*)&m_cConnectAddress, sizeof(m_cConnectAddress) ) )
	{
		m_bConnect = true;
		return g_lConnectSucceed;
	}
		

	return g_lConnectError_Firewall;
}

bool CWinSockLib::initBind( unsigned short iVer, bool bOverlapped, char *pstrAddress, unsigned short iPort, int iPendingConnection )
{
	if( startupWinsock( iVer ) &&
		createSocket( bOverlapped ) &&
		bindSocket( pstrAddress, iPort ) &&
		listenSocket( iPendingConnection ) )
		return true;

	return false;
}

bool CWinSockLib::initConnect( unsigned short iVer, bool bOverlapped, char *pstrAddress, unsigned short iPort )
{
	if( !startupWinsock( iVer ) )
	{
		m_lErrorCode = 1;
		return false;
	}

	if( !createSocket( bOverlapped ) )
	{
		m_lErrorCode = 2;
		return false;
	}

	int	iResult = connectSocket( pstrAddress, iPort );
	switch( iResult )
	{
	case g_lConnectError_DNS:
		m_lErrorCode = 4;
		return false;
	case g_lConnectError_Firewall:
		m_lErrorCode = 8;
		return false;
	case g_lConnectSucceed:
		m_hRecvEvent = WSACreateEvent();
		WSAEventSelect( m_hSocket, m_hRecvEvent, FD_CONNECT | FD_READ | FD_CLOSE );
		m_lErrorCode = 0;
		return true;
	}

	return false;
}

SOCKET CWinSockLib::acceptSocket()
{
	int	nAddrSize = sizeof( m_cAcceptClientAddress );
	return accept( m_hSocket, (sockaddr*)&m_cAcceptClientAddress, &nAddrSize );
}

void CWinSockLib::cleanupWinsock()
{
	if ( m_hSocket )
	{
		--m_lSockNum;
		closesocket( m_hSocket );
		m_hSocket	= NULL;
		m_bConnect	= false;
	}

	if ( m_bStartup && !m_lSockNum )
	{
		WSACleanup();
		m_bStartup = false;
	}
}


bool CWinSockLib::SendPacket( char *pstrPacket, int iPacketSize )
{
	int		iStartPosition = 0;
	int		iTotalSendSize = 0;

	while( 1 )
	{
		int iSendSize = send( m_hSocket, &pstrPacket[iStartPosition], iPacketSize-iTotalSendSize, 0 );
		if (iSendSize < 0)
		{
			delete [] pstrPacket;
			return false;
		}

		iTotalSendSize += iSendSize;
		iStartPosition += iSendSize;

		if( iTotalSendSize == iPacketSize )
		{
			delete [] pstrPacket;
			return true;
		}
	}
	
	return false;
}

char* CWinSockLib::GetIPFromHost( char *pstrHostName )
{
	in_addr iaHost;
	iaHost.s_addr = inet_addr( pstrHostName );

	LPHOSTENT lpHostEntry = INADDR_NONE == iaHost.s_addr ? gethostbyname( pstrHostName ) : gethostbyaddr( (const char *)&iaHost, sizeof(struct in_addr), AF_INET );
	if( !lpHostEntry )		return NULL;

	for (int i = 0; ; ++i)
	{
		in_addr* pinAddr = ((LPIN_ADDR)lpHostEntry->h_addr_list[i]);
		if ( !pinAddr )		break;
		
		return inet_ntoa( *pinAddr );
	}

	return NULL;
}