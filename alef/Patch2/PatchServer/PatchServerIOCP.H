#ifndef __IOCP_PATCHSERVER_LIB_H__
#define __IOCP_PATCHSERVER_LIB_H__

//#include "PatchInfo.h"
#include "AuPacket.h"

#include "AuPatchCheckCode.h"
#include "AuPackingManager.h"

#include <stdio.h>
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>

#include "PatchPacketTypeCS.h"
#include "PatchPacketTypeSC.h"

#include "../../include/Queue.h"

#include "PatchServerWinSockLib.h"
#include "PatchServerOverlapped.h"
#include "ApAdmin.h"
//#include "AgsmIPBlockingManger.h"

//IOCP Command( Key )
#define		IOCP_OP_DESTROY				0xFFFFFFFF
#define		IOCP_OP_ACTIVE				0xFFFFFFFE
#define		IOCP_ACCEPTEX				0xFFFFFFFD

#define		MAX_IP						5

typedef unsigned (WINAPI *PBEGINTHREADEX_THREADFUNC)(
    LPVOID lpThreadParameter
    );
typedef unsigned *PBEGINTHREADEX_THREADID;

class CSocketIOContext
{
public:
	SOCKET			m_hSocket;

	//PatchFile List를 얻기위한 리스트.
	CDList< CJZPFileInfo * > *m_pcPatchFileInfo;
	CListNode< CJZPFileInfo *>	 *m_pcPatchFileInfoNode;

	//m_cWSABuf에 대한 Race-Condition방지용.
	CRITICAL_SECTION	m_cSendCriticalSection;
	CRITICAL_SECTION	m_cRecvCriticalSection;

	CSocketIOContext();
	CSocketIOContext(SOCKET hSocket, int iSendBufferSize, bool bSendEvent, int iRecvBufferSize, bool bRecvEvent );
	~CSocketIOContext();
	void init( int iSendBufferSize, bool bSendEvent, int iRecvBufferSize, bool bRecvEvent );
	void SendLock();
	void SendUnlock();
	void RecvLock();
	void RecvUnlock();
	void cleanupContext();
};

class CQueueUnit
{
public:
	CSocketIOContext *m_pcContext;
	char			*m_pstrPacket;
	int				m_iPacketSize;
	SOCKET			m_hSocket;
};

class CIOCP
{
public:
	HANDLE			m_hIOCP;

	bool CreateIOCP( int iThreadNum );
	bool AssociateIOCP( HANDLE hAssociateHandle, void *pvKey );
	bool CleanupIOCP();
};

class CLockMgr
{
public:
	CLockMgr();
	~CLockMgr();

	void Lock();
	void Unlock();

	bool AddObject( void *pvData, int iKey );
	bool RemoveObject( int iKey );
	void *GetObject( int iKey );

private:
	ApAdmin					m_csContextTree;
	CRITICAL_SECTION		m_cCriticalSection;

	void Init();
	void CleanUp();
};

class CIOCPNetworkModule : public CIOCP
{
	char					m_strServerIP[PATCH_DOWNLOAD_SERVER_LENGTH];
	unsigned short			m_iServerPort;

public:
	CLockMgr				m_cLockMgr;

	unsigned int			m_iStatus;

	CWinSockLib				m_cWinSockLib;
	CAcceptOverlappedEx		m_AcceptOverlappedEx;
	CQueue<CQueueUnit *>	m_cSendQueue;
	CQueue<CQueueUnit *>	m_cRecvQueue;

//	AgsmIPBlockingManager	m_cIPBlockingManager;

	CQueue<unsigned int (CIOCPNetworkModule::*)(void *)>	m_cRegisteredThreadLlist;

	HANDLE				m_hSendQueueEvent;
	HANDLE				m_hRecvQueueEvent;

	//IP블럭킹 체크
	bool				m_bIPBlockingCheck;

	//패치용
	AuPackingManager	m_cAuPackingManager;
	AuPatchCheckCode	m_cAuPatchCheckCode;

	int					m_iVersion;
	int					m_iCurrentUsers;

	int					m_nNumAddr;
	char				m_szLocalAddr[MAX_IP][64];

	void InitLocalIPAddr();

	//IP블럭킹 체크
	void SetIPBlocking( bool bUseIPBlocking );

	//PatchCode
	int GetPatchFiles( int iVersion, unsigned int iPatchCheckCode, CDList<CJZPFileInfo *> *pcList );

	//실제 사용하지는 않으나 확장성을 위해 둠.
	bool registerThread( unsigned int ( CIOCPNetworkModule::*pf )( void * ) );
	void beginRegisteredThread();
	static DWORD WINAPI executeRegistedThread( LPVOID param);

	//Download Server Set
	bool SetDownloadServer( char *pstrServerIP, unsigned short iServerPort );
	char *GetDownloadServerIP();
	unsigned short GetDownloadServerPort();

	//PatchCheckCode Load;
	bool LoadPatchCode( char *pstrFileName );

	//Netwotk Accept
	int postAccept();
	//Network Recv
	int postRecv( CSocketIOContext *pContext, COverlappedRecv *pcOverlappedRecv );
	//sendPacket;
	int sendPacket( CSocketIOContext *pContext, char *pstrPacket, int iPacketSize );
	//int sendPatchFileBlock( CSocketIOContext *pContext );
	int sendPatchFileInfo( CSocketIOContext *pContext );
	//Network Send
	int postSend( CSocketIOContext *pContext );
	//Packet Parsing
	int parsePacket( CSocketIOContext *pcContext, COverlappedRecv *pCOverlappedRecv, int iTransferedBytes );

	int cleanup();
	//Thread for this class
	virtual unsigned int IOCPGQCSThread( void *pArg );
	virtual unsigned int IOCPGQCSThreadSM( void *pArg );
	virtual unsigned int IOCPSendThread( void *pArg );
	virtual unsigned int IOCPRecvThread( void *pArg );

	CIOCPNetworkModule();
	~CIOCPNetworkModule();

public :
	unsigned long								m_ulClientFileCRC;

public :
	BOOL 				SendClientFileCRC		( CSocketIOContext *pContext );

private :
	unsigned long		_GetClientFileCRC		( char* pFolderName, char* pFileName );
};

#endif