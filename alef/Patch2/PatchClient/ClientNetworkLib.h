#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <afxinet.h>

#include "LinkedList.h"

#include "PatchPacketTypeCS.h"
#include "PatchPacketTypeSC.h"
#include "PatchClientRegistry.h"
#include "PatchCompress.h"
#include "AuMD5Encrypt.h"
#include "CustomProgressBar.h"
#include "AuPackingManager.h"

#include "AuPatchCheckCode.h"

enum ePatchResult
{
	g_lError				= 0x00,
	g_lAlreadyPatched,
	g_lNotEnoughDiskSpace,
	g_lCannotConnectToDownloadServer,
	g_lDisconnectFromDownloadServer,
	g_lCannotOpenFileFromDownload,
	g_lCannotDownloadFileFromDownload,
};

enum ePatchConnectResult
{
	g_lConnectError_DNS		= 0x00,
	g_lConnectError_Firewall,
	g_lConnectSucceed,
};

class CWinSockLib
{
public:
	CWinSockLib();
	~CWinSockLib();

	bool	startupWinsock(unsigned short iVersion);
	bool	createSocket( bool bOverlapped );
	bool	bindSocket( char *pstrAddress, unsigned short iPort );
	bool	listenSocket( int iPendingConnection );
	int		connectSocket( char *pstrAddress, unsigned short iPort );
	bool	initBind( unsigned short iVer, bool bOverlapped, char *pstrAddress, unsigned short iPort, int iPendingConnection );
	bool	initConnect( unsigned short iVer, bool bOverlapped, char *pstrAddress, unsigned short iPort );
	SOCKET	acceptSocket();
	void	cleanupWinsock();

	bool	SendPacket( char *pstrPacket, int iPacketSize );

	char*	GetIPFromHost( char *pstrHostName );

public:
	SOCKET				m_hSocket;
	SOCKADDR_IN			m_cAcceptClientAddress;
	WSANETWORKEVENTS	m_event;
	WSAEVENT			m_hRecvEvent;

	bool				m_bBind;
	bool				m_bConnect;

	int					m_lErrorCode;

	static WSADATA	m_cWsaData;
	static bool		m_bStartup;
	static int		m_lSockNum;

private:
	SOCKADDR_IN		m_cBindAddress;
	SOCKADDR_IN		m_cConnectAddress;
};
