/******************************************************************************
Module:  AsDPModule.h
Notices: Copyright (c) 2002 netong
Purpose: 
Last Update: 2002. 04. 01
******************************************************************************/

#if !defined(__DPSERVER_MODULE_H__)
#define __DPSERVER_MODULE_H__

#include <dplay8.h>
#include <dpaddr.h>
#include <dxerr8.h>

#include "ApBase.h"

#include "AsCMDQueue.h"


#ifdef	_DEBUG
#pragma comment ( lib , "AsDP8D" )
#else
#pragma comment ( lib , "AsDP8" )
#endif


/*
#define ASDP_CONNECT_TIMEOUT		1500	// milisecond
#define ASDP_CONNECT_RETRIES		3
#define	ASDP_TIMEOUT_KEEPALIVE		5000	// milisecond
*/

//interface IDirectPlay8Server;


//-----------------------------------------------------------------------------
// Miscellaneous helper functions
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//class AS_APP_PLAYER_INFO : public OVERLAPPED {
typedef struct _AS_APP_PLAYER_INFO {
//public:
    LONG			lRefCount;									// Ref count so we can cleanup when all threads 
																// are done w/ this object
    DPNID			dpnidPlayer;								// DPNID of player
	//BOOL			bReady;										// 처리할 thread가 정해졌는지 여부...
	//UINT32		ulThreadIndex;								// process thread pool index
    TCHAR			strPlayerName[AGPACHARACTER_MAX_ID_STRING];	// Player name
	INT32			lID;										// ID (CID or ServerID)
	BOOL			bServer;									// 서버와의 연결인지 여부
//};
} AS_APP_PLAYER_INFO, *pAS_APP_PLAYER_INFO;

class AS_DPN_BUFFER_DESC : public OVERLAPPED {
public:
	DPN_BUFFER_DESC	bufferDesc;
};

/*
interface   IOutboundServer
{
public:
    //virtual HRESULT SendPacket( DWORD dwTo, void* dwData, DWORD dwSize, 
    //                           BOOL dwGuaranteed, DWORD dwTimeout ) = 0;
    //virtual HRESULT GetConnectionInfo( DWORD dwID, TCHAR* strConnectionInfo ) = 0;
    virtual HRESULT RejectClient( DWORD dwID, HRESULT hrReason ) = 0;
};
*/

class AsDP8 /*: public IOutboundServer*/ {
private:
	GUID*		m_pguidApp;
	INT16*		m_pnServerStatus;
	AsCMDQueue* m_pqueueRecv;
	AsCMDQueue* m_pqueueRecvSvr;

private:
	DWORD					m_dwPort;
	BOOL					m_bDPServerStarted;

	WCHAR					m_szSessionName[40];

	DPNID					m_dpnidClients;
	DPNID					m_dpnidServers;

	CHAR					m_szIPv4Addr[23];

	BOOL					CheckValidSource(PVOID pvData, AS_APP_PLAYER_INFO *pPlayerInfo);

	INT32					m_lSelfCID;

public:
	AsDP8();
	AsDP8(UINT32 nPort, GUID *pguidApp, INT16 *pnServerStatus, AsCMDQueue *pqueueRecv, AsCMDQueue *pqueueRecvSvr);
	~AsDP8();

	BOOL	Initialize(UINT32 nPort, GUID *pguidApp, INT16 *pnServerStatus, AsCMDQueue *pqueueRecv, AsCMDQueue *pqueueRecvSvr);

	HRESULT	StartServer();
	VOID	StopServer();

	static HRESULT WINAPI DPMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer);

	//virtual HRESULT RejectClient( DWORD dwID, HRESULT hrReason );

	INT16	SendTo(DPNID dpnid, PVOID pvData, INT16 nLength);				// Send Data

	INT32	GetActivePlayers();		// Get Number of players currently in game

	INT16	GetConnectionInfo(DPNID dpnid, UINT32 *pulRoundTripLatencyMS, UINT32 *pulThroughputBPS);

	IDirectPlay8Server*		GetDP8Server();

	DPNID	GetDPNIDClients();
	DPNID	GetDPNIDServers();

	BOOL	GetLocalIPAddr(CHAR* pszBuffer, INT16 *pnLength);

	BOOL	DestroyClient(DPNID dpnid);

	GUID*	GetGUID();

	AsCMDQueue*	GetQueueRecv();
	AsCMDQueue* GetQueueRecvSvr();

	INT16*	GetServerStatus();

	BOOL	SetIDToPlayerContext(INT32 lID, DPNID dpnid);
	//BOOL	SetAccountIDToPlayerContext(INT32 lAccountID, DPNID dpnid);

	DPNID	CreateGroup();
	BOOL	DestroyGroup(DPNID dpnidGroup);
	BOOL	AddPlayerToGroup(DPNID dpnidGroup, DPNID dpnidPlayer);
	BOOL	RemovePlayerFromGroup(DPNID dpnidGroup, DPNID dpnidPlayer);

	BOOL	SetSelfCID(INT32 lServerID);
};

VOID DXUtil_ConvertWideStringToGeneric(TCHAR* tstrDestination, const WCHAR* wstrSource, int cchDestChar);
VOID DXUtil_ConvertWideStringToAnsi(CHAR* strDestination, const WCHAR* wstrSource, int cchDestChar);
VOID DXUtil_ConvertGenericStringToWide(WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar);
VOID DXUtil_ConvertAnsiStringToWide(WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar);
VOID DXUtil_ConvertAnsiStringToGeneric(TCHAR* tstrDestination, const CHAR* strSource, int cchDestChar = (-1));

#endif