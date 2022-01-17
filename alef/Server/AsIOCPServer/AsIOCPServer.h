/*
	Notices: Copyright (c) NHN Studio 2003
	Created by: Bryan Jeong (2003/12/24)
 */

// AsIOCPServer.h: interface for the AsIOCPServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASIOCPSERVER_H__5EBC67AE_3DE9_45C2_9104_E438870B924A__INCLUDED_)
#define AFX_ASIOCPSERVER_H__5EBC67AE_3DE9_45C2_9104_E438870B924A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <WINSOCK2.H>
#include "ApIOCP.h"
#include "ApMutualEx.h"
#include "AsListener.h"
#include "AsTimer.h"
#include "AsServerSocketManager.h"
#include "AuIPBlockingManager.h"

#pragma comment (lib, "ws2_32")
#pragma comment (lib, "mswsock")

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AsIOCPServerD" )
#else
#pragma comment ( lib , "AsIOCPServer" )
#endif
#endif

typedef BOOL (*AsDispatchCallBack) (PVOID pData, PVOID pParam, PVOID pSocket);
#define MAX_DEVICES				5

#define	DISCONNNECT_REASON_CLIENT_NORMAL		2
#define	DISCONNNECT_REASON_CLIENT_OUT_OF_DATE	1

struct AsDispatchParam
{
	PVOID					pvModule;
	AsDispatchCallBack		fpDispatch;
	AsDispatchCallBack		fpDisconnect;

	AsDispatchParam() : pvModule(NULL), fpDispatch(NULL), fpDisconnect(NULL) {}
};


class AsIOCPServer : public ApIOCP
{
private:
	INT32					m_lNIDClients;
	INT32					m_lNIDServers;
	INT32					m_lSelfCID;

	INT16					m_nNumAddr;
	CHAR					m_szLocalAddr[MAX_DEVICES][23];

	EnumSocketType			m_eDefaultSocketType;

#ifdef __PROFILE__
	DWORD			m_dwAcceptableDelay;
#endif

public:
	//ApMutualEx				m_Mutex;
	ApCriticalSection		m_Mutex;			// 이 객체를 잠그고 AsTimer에 이벤트를 추가하면 DeadLock 걸릴 수 있다
	
	BOOL					m_bExecuteBlockIP;
	BOOL					m_bOpenPCRoomIP;

	AsListener				m_csListener;
	AsServerSocketManager	m_csSocketManager;
	AsTimer					m_csTimer;

	AuIPBlockingManager		m_cIPBlockingManager;

	PVOID					m_pClass;
	AsDispatchCallBack		m_fpLoginDisconnect;
	AsDispatchCallBack		m_fpConnect;

	AsDispatchParam			m_DispatchArchlord;
	AsDispatchParam			m_DispatchLKServer;
	AsDispatchParam			m_DispatchBilling;
	AsDispatchParam			m_DispatchPatch;

	HANDLE		m_hThread;
	UINT		m_ulThreadID;

	VOID	NetworkInitialize();
	VOID	NetworkUnInitialize();

public:
	AsIOCPServer();
	virtual ~AsIOCPServer();

	BOOL ProcessListener(AsObject *pObject, AsOverlapped* pOverlapped);
	BOOL ProcessSocket(AsObject *pObject, DWORD dwTransferred);
	BOOL ProcessTimer(AsObject *pObject);
	BOOL DisconnectSocket(AsObject* pObject, AsOverlapped* pOverlapped);

	BOOL SetSelfCID(INT32 lCID) {m_lSelfCID = lCID; return TRUE;}
	BOOL GetLocalIPAddr(CHAR **pszBuffer, INT16 *pnCount);

	INT32 GetDPNIDClients() {return m_lNIDClients;}
	INT32 GetDPNIDServers() {return m_lNIDServers;}

	PlayerData* GetPlayerDataForID(INT32 lNID);
	BOOL SetIDToPlayerContext(INT32 lID, INT32 lNID);
	BOOL SetAccountNameToPlayerContext(CHAR *szAccountName, INT32 lNID);

	BOOL SetCheckValidation(INT32 lNID);
	BOOL ResetCheckValidation(INT32 lNID);

	BOOL SetServerFlagToPlayerContext(INT32 lNID);

	virtual BOOL Initialize(INT32 lThreadCount, INT32 lPort, INT32 lMaxConnectionCount = 3000, INT32 lMaxSendBufferMB = 100);
	VOID InitLocalIPAddr(INT32 lPort);

	BOOL LoadIPBlockRes( char *pstrFileName, char *pstrPCRoomIPFileName );
	VOID IsCheckBlockIP( BOOL bCheck );
	BOOL CheckBlockIP(AsServerSocket *pSocket);	
	BOOL CheckPCRoomIP(AsServerSocket *pSocket);	

	void SetDispatchGameModule(PVOID pvModule, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect);
	void SetDispatchLKModule(PVOID pvModule, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect);
	void SetDispatchBillingModule(PVOID pvModule, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect);
	void SetDispatchPatchModule(PVOID pvModule, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect);

	void SetConnectFunction(AsDispatchCallBack fpConnect);
	
	BOOL DestroyClient(INT32 lNID, INT32 lReason = DISCONNNECT_REASON_CLIENT_OUT_OF_DATE, CHAR* Buffer = NULL);
	BOOL DestroyClient(AsServerSocket *pSocket, INT32 lReason = DISCONNNECT_REASON_CLIENT_OUT_OF_DATE, CHAR* Buffer = NULL);

	BOOL DestroyAllClient();

	BOOL ReUseSocket(AsServerSocket *pSocket);

	void SetDefaultSocketType(EnumSocketType eSocketType);

#ifdef __PROFILE__
	DWORD GetAcceptableDelay();
	void SetAcceptableDelay(DWORD  dwDelayTime);
#endif

	static UINT WINAPI ConnectionChecker(PVOID pvParam);
	BOOL StartConnectionChecker();
};

#endif // !defined(AFX_ASIOCPSERVER_H__5EBC67AE_3DE9_45C2_9104_E438870B924A__INCLUDED_)
