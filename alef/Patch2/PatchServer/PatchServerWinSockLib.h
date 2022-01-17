#ifndef __WINSOCKLIB_H__
#define __WINSOCKLIB_H__

#include <winsock2.h>
#include <windows.h>

class CWinSockLib
{
	SOCKADDR_IN		m_cBindAddress;
	SOCKADDR_IN		m_cConnectAddress;

public:
	SOCKET			m_hSocket;
	WSADATA			m_cWsaData;
	SOCKADDR_IN		m_cAcceptClientAddress;

	bool			m_bBind;
	bool			m_bConnect;

	CWinSockLib();
	~CWinSockLib();

	bool startupWinsock(unsigned short iVersion);
	bool createSocket( bool bOverlapped );
	bool bindSocket( char *pstrAddress, unsigned short iPort );
	bool listenSocket( int iPendingConnection );
	bool connectSocket( char *pstrAddress, unsigned short iPort );
	bool initBind( bool bOverlapped, char *pstrAddress, unsigned short iPort, int iPendingConnection );
	bool initConnect( unsigned short iVer, bool bOverlapped, char *pstrAddress, unsigned short iPort );
	SOCKET acceptSocket();
	void cleanupWinsock();
};

#endif