//----------------------------------------------------------------------------
// File: server.h
//
// Desc: see main.cpp
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _DPLAY8_SERVER_H
#define _DPLAY8_SERVER_H



#define MIN_SP_THREADS		1
#define MAX_SP_THREADS		128
#define MAX_SP_BUFFER       1024000

#define MAX_PACK_SIZE		2048
#define MAX_THREAD_WAIT     60000

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
#include "Windows.h"
#include "NetAbstract.h"

#include "ApBase.h"

//#include "AsCMDQueue.h"
#include "ApMutualEx.h"
#include "ApIOCPDispatcher.h"

interface IDirectPlay8Server;

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AsDP9D" )
#else
#pragma comment ( lib , "AsDP9" )
#endif
#endif

#define	DISCONNNECT_REASON_CLIENT_NORMAL		2
#define	DISCONNNECT_REASON_CLIENT_OUT_OF_DATE	1

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
struct PlayerData
{
    DWORD           dwID;                   // Client ID
    DWORD           NetID;                  // NetID (DPNID) for owning player (0 if none)
    DWORD           dwVersion;              // Version of the owning player
    BOOL            bAllow;                 // If FALSE, then we should drop this player

    // Links for the various lists
    PlayerData*     pNext;                  // Free/active PlayerData list (double link)
    PlayerData*     pPrevious;
    PlayerData*     pNextInCell;            // Cell list (single link)
    PlayerData*     pNextInIDHashBucket;    // ID hash bucket (single link)

	BOOL			bActive;				// active data 인지 여부

	INT32			lID;					// ID (CID or ServerID)
	CHAR			szAccountName[32];		// account name
	BOOL			bServer;				// 서버와의 연결인지 여부
};

enum { NUM_PLAYER_OBJECT_LOCKS		= 16 };
enum { NUM_ID_HASH_BUCKETS			= 1024 };
enum { NUM_ID_HASH_BUCKET_LOCKS		= 16 };


//-----------------------------------------------------------------------------
// Name: 
// Desc: Client IDs are 32-bit values that refer to a particular Client. They are 
//       broken up into two bitfields, one of which can be used into an index 
//       of a list of Client 'slots', the other bitfield is a "uniqueness" value 
//       that is incremented each time a new Client is created. Hence, although
//       the same slot may be reused by different Clients are different times, 
//       it's possible to distinguish between the two by comparing uniqueness 
//       values (you can just compare the whole 32-bit id).
//-----------------------------------------------------------------------------
typedef DWORD   ClientID;

#define PLAYER_OBJECT_SLOT_BITS 13
#define MAX_PLAYER_OBJECTS      (1<<PLAYER_OBJECT_SLOT_BITS)
#define PLAYER_OBJECT_SLOT_MASK (MAX_PLAYER_OBJECTS-1)

#define MAX_DEVICES				5

//-----------------------------------------------------------------------------
// Name: 
// Desc: Simple wrapper for critical section
//-----------------------------------------------------------------------------
class CCriticalSection
{
public:
    CCriticalSection( DWORD spincount = 2000 )
    {
        InitializeCriticalSection( &m_CritSec );
    };

    ~CCriticalSection()
    {
        DeleteCriticalSection( &m_CritSec );
    };

    void    Enter()
    {
        EnterCriticalSection( &m_CritSec );
    };

    void    Leave()
    {
        LeaveCriticalSection( &m_CritSec );
    };

private:
    CRITICAL_SECTION    m_CritSec;
};

//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
class AsDP9Server : public IOutboundServer
{
private:
	GUID*				m_pguidApp;
	INT16*				m_pnServerStatus;
//	AsCMDQueue*			m_pqueueRecv;
//	AsCMDQueue*			m_pqueueRecvSvr;
	ApIOCPDispatcher	*m_pcsIOCPDispatcher;

	DWORD				m_dpnidClients;
	DWORD				m_dpnidServers;

	DWORD				m_dwPort;

	INT32				m_lSelfCID;

	INT16				m_nNumAddr;
	CHAR				m_szLocalAddr[MAX_DEVICES][23];

	BOOL				CheckValidSource(PVOID pvData, DWORD dpnid);

	DWORD				m_dwConnectTimeout;
	DWORD				m_dwConnectRetries;
	DWORD				m_dwTimeoutUntilKeepAlive;


	void				OnAddConnection( DWORD dpnid );
	void				OnRemoveConnection( DWORD dpnid );
	HRESULT				OnPacket( DWORD dpnidFrom, void* pData, DWORD size );

    CCriticalSection    m_csAddRemoveLock;
    CCriticalSection    m_csPlayerDataListLock;
    CCriticalSection    m_csPlayerDataLocks[NUM_PLAYER_OBJECT_LOCKS];
    ApCriticalSection	m_csThreadCountLock;    
    CCriticalSection    m_csIDHashBucketLocks[NUM_ID_HASH_BUCKET_LOCKS];
    CCriticalSection    m_csClientNetConfigLock;

	DWORD				m_dwPlayerCount;
	DWORD				m_dwPeakPlayerCount;

	WORD				m_wActiveThreadCount;
	WORD				m_wMaxThreadCount;
    FLOAT				m_fAvgThreadCount;
    FLOAT				m_fAvgThreadTime;
    FLOAT				m_fMaxThreadTime;

    PlayerData*			CreatePlayerData();
    void				DestroyPlayerData( PlayerData* pPlayerData );

    // The PlayerData lists
    PlayerData          m_PlayerDatas[MAX_PLAYER_OBJECTS];
    PlayerData*         m_pFirstActivePlayerData;
    PlayerData*         m_pFirstFreePlayerData;
    DWORD               m_dwActivePlayerDataCount;
    DWORD               m_dwPlayerDataUniqueValue;

    // Hashing DPIDs to PlayerData pointers
    void                SetPlayerDataForID( DWORD dwID, PlayerData* pPlayerData );
    PlayerData*         GetPlayerDataForID( DWORD dwID );
    void                RemovePlayerDataID( PlayerData* pPlayerData );
    DWORD               IDHash( DWORD dwID );
    enum { NUM_ID_HASH_BUCKETS = 1024 };
    enum { NUM_ID_HASH_BUCKET_LOCKS = 16 };
    PlayerData*         m_pstIDHashBucket[NUM_ID_HASH_BUCKETS];

    void                LockPlayerData( PlayerData* pPlayerData ) { m_csPlayerDataLocks[((pPlayerData-m_PlayerDatas) & (NUM_PLAYER_OBJECT_LOCKS-1))].Enter(); };
    void                UnlockPlayerData( PlayerData* pPlayerData ) { m_csPlayerDataLocks[((pPlayerData-m_PlayerDatas) & (NUM_PLAYER_OBJECT_LOCKS-1))].Leave(); };

public:
    AsDP9Server();
	~AsDP9Server();

	BOOL				Initialize(UINT32 nPort, GUID *pguidApp, INT16 *pnServerStatus, ApIOCPDispatcher *pcsIOCPDispatcher,
								   DWORD dwConnectTimeout, DWORD dwConnectRetries, DWORD dwTimeoutUntilKeepAlive);

	HRESULT				StartServer();
	VOID				StopServer();

	INT16				SendTo(DWORD dpnid, PVOID pvData, INT16 nLength);				// Send Data

	INT32				GetActivePlayers();		// Get Number of players currently in game

	INT16				GetConnectionInfo(DWORD dpnid, UINT32 *pulRoundTripLatencyMS, UINT32 *pulThroughputBPS);

	IDirectPlay8Server*	GetDP8Server();

	DWORD				GetDPNIDClients();
	DWORD				GetDPNIDServers();

	BOOL				GetLocalIPAddr(CHAR **pszBuffer, INT16 *pnLength);

	BOOL				DestroyClient(DWORD dpnid, HRESULT hrReason = DISCONNNECT_REASON_CLIENT_OUT_OF_DATE);

	GUID*				GetGUID();

//	AsCMDQueue*			GetQueueRecv();
//	AsCMDQueue*			GetQueueRecvSvr();

	INT16*				GetServerStatus();

	BOOL				SetIDToPlayerContext(INT32 lID, DWORD dpnid);
	BOOL				SetAccountNameToPlayerContext(CHAR *szAccountName, DWORD dpnid);

	DWORD				CreateGroup();
	BOOL				DestroyGroup(DWORD dpnidGroup);
	BOOL				AddPlayerToGroup(DWORD dpnidGroup, DWORD dpnidPlayer);
	BOOL				RemovePlayerFromGroup(DWORD dpnidGroup, DWORD dpnidPlayer);

	BOOL				SetSelfCID(INT32 lServerID);


    HRESULT				Start(DWORD dwNumSPThreads);
    void				Shutdown();
    void				SetServer( INetServer* pServer ) { m_pServer = pServer; };

    DWORD				GetNumSPThreads();
    void				SetNumSPThreads(DWORD dwNumSPThreads);

    DWORD				GetSPBuffer();
    void				SetSPBuffer(DWORD dwSPBufferSize);

    // From IOutboundServer
    virtual HRESULT SendPacket( DWORD dwTo, void* pData, DWORD dwSize, BOOL bGuaranteed, DWORD dwTimeout );
    virtual HRESULT GetConnectionInfo( DWORD dwID, TCHAR* strConnectionInfo, DWORD dwBufLength );
    virtual HRESULT RejectClient( DWORD dwID, HRESULT hrReason );


protected:
    IDirectPlay8Server*     m_pDPlay;
    INetServer*             m_pServer;

    static HRESULT WINAPI StaticReceiveHandler( void *pvContext, DWORD dwMessageType, void *pvMessage );
    HRESULT WINAPI ReceiveHandler( void *pvContext, DWORD dwMessageType, void *pvMessage );
};




#endif
