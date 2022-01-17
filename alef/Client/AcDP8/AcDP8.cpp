/******************************************************************************
Module:  AcDP8.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 12. 09
******************************************************************************/


#define STRICT
#include "AcDP8.h"

#include <basetsd.h>
#include <dplay8.h>
#include <dpaddr.h>
//#include <dplobby8.h>
#include <dxerr8.h>
#include <tchar.h>
//#include "DXUtil.h"
//#include "SimpleClientServer.h"
//#include "NetClient.h"
//#include "resource.h"

#include "MagDebug.h"

//-----------------------------------------------------------------------------
// player struct locking defines
//-----------------------------------------------------------------------------
CRITICAL_SECTION g_csPlayerContext;
#define PLAYER_LOCK()                   EnterCriticalSection( &g_csPlayerContext ); 
#define PLAYER_ADDREF( pPlayerInfo )    if( pPlayerInfo ) pPlayerInfo->lRefCount++;
#define PLAYER_RELEASE( pPlayerInfo )   if( pPlayerInfo ) { pPlayerInfo->lRefCount--; if( pPlayerInfo->lRefCount <= 0 ) DestoryPlayerStruct( pPlayerInfo ); } pPlayerInfo = NULL;
#define PLAYER_UNLOCK()                 LeaveCriticalSection( &g_csPlayerContext );

GUID	g_guidApp;

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
struct APP_PLAYER_INFO
{
    LONG  lRefCount;                        // Ref count so we can cleanup when all threads 
                                            // are done w/ this object
    DPNID dpnidPlayer;                      // DPNID of player
    //TCHAR strPlayerName[MAX_PLAYER_NAME];   // Player name

	INT16	nNID;

    APP_PLAYER_INFO* pNext;
    APP_PLAYER_INFO* pPrev;
};


struct APP_HANDLER_CONTEXT
{
	PVOID	pvThisClass;
	INT16	nConnectionIndex;
};


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
IDirectPlay8Client*     g_pDPClient[ACDP_MAX_DPCLIENT_INSTANCE];   // DirectPlay peer objects
eAcDPServerType			g_eConnectionServerType[ACDP_MAX_DPCLIENT_INSTANCE];
INT16					g_nNumDPInstance			= 0;		   // # of DirectPlay peer objects
//CNetClientWizard*       g_pNetClientWizard            = NULL;    // Connection wizard
//IDirectPlay8LobbiedApplication* g_pLobbiedApp         = NULL;    // DirectPlay lobbied app 
//BOOL                    g_bWasLobbyLaunched           = FALSE;   // TRUE if lobby launched
//HINSTANCE               g_hInst                       = NULL;    // HINST of app
//HWND                    g_hDlg                        = NULL;    // HWND of main dialog
LONG                    g_lNumberOfActivePlayers      = 0;       // Number of players currently in game
DPNID                   g_dpnidLocalPlayer            = 0;
APP_PLAYER_INFO         g_playerHead;
TCHAR                   g_strAppName[256]             = TEXT("ArchLordClient");
//HRESULT                 g_hrDialog;                              // Exit code for app 
TCHAR                   g_strLocalPlayerName[MAX_PATH];          // Local player name


UINT32					g_dpnidConnected				= 0;


//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------
//HRESULT WINAPI   DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
//HRESULT WINAPI   DirectPlayLobbyMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
//INT_PTR CALLBACK GreetingDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
//HRESULT  InitDirectPlay();
//HRESULT  WaveToAllPlayers();
//VOID     AppendTextToEditControl( HWND hDlg, TCHAR* strNewLogLine );
//HRESULT  GetPlayerStruct( DPNID dpnidPlayer, APP_PLAYER_INFO** ppPlayerInfo );
//VOID     DestoryPlayerStruct( APP_PLAYER_INFO* pPlayerInfo );
//VOID     AddPlayerStruct( APP_PLAYER_INFO* pPlayerInfo );


AcDP8::AcDP8()
{
	m_bDPConnected	= FALSE;

	m_queueRecv.Initialize(30000);

	m_hEnumAsyncOp	= NULL;
	m_hConnectAsyncOp	= NULL;

	for (int i = 0; i < ACDP_MAX_DPCLIENT_INSTANCE; ++i)
		g_pDPClient[i] = NULL;

	m_bStartDP = FALSE;

	m_lSelfCID = AP_INVALID_CID;
}

AcDP8::~AcDP8()
{
}

DWORD WINAPI AcDP8::CleanUpThread(PVOID pvParam)
{
	if (!pvParam)
		return FALSE;

	AcDP8 *pThis = (AcDP8 *) pvParam;

	while (pThis->m_bStartDP)
	{
		WaitForSingleObject(pThis->m_hCleanUpEvent, INFINITE);

		pThis->Close(pThis->nCleanUpInstance);

		ResetEvent(pThis->m_hCleanUpEvent);
	}

	return TRUE;
}

BOOL AcDP8::SetSelfCID(INT32 lCID)
{
	m_lSelfCID = lCID;

	return TRUE;
}

BOOL AcDP8::StartDP()
{
	InitializeCriticalSection(&g_csPlayerContext);

	m_bStartDP = TRUE;

	m_hCleanUpEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	ResetEvent(m_hCleanUpEvent);

	m_hCleanUpThread = 
		CreateThread(NULL,
						0,
						CleanUpThread,
						this, 					// &i -> this (Edited by Parn)
						0,
						&m_ulCleanUpThreadID);

	if (m_hCleanUpThread == NULL)
	{
		// thread creation failed...
		CloseHandle(m_hCleanUpEvent);
		return FALSE;
	}
	else
	{
		// success thread creation

	}

	return TRUE;
}

//BOOL AcDP8::StartDP(CHAR *pszServerAddr)
INT16 AcDP8::Open(CHAR *pszServerAddr, INT16 nServerType)
{
	if (g_nNumDPInstance == ACDP_MAX_DPCLIENT_INSTANCE || !pszServerAddr || !strlen(pszServerAddr))
		return ACDP_CONNECT_ERR;

    HRESULT hr;

	m_bDPConnected	= FALSE;

	EnterCriticalSection(&g_csPlayerContext);
	INT16	nIndexDPInstance;
	for (int i = 0; i < ACDP_MAX_DPCLIENT_INSTANCE; ++i)
	{
		if (!g_pDPClient[i])
			break;
	}

	if (i == ACDP_MAX_DPCLIENT_INSTANCE)
		return (-1);

	nIndexDPInstance = i;

    DPN_APPLICATION_DESC   dpnAppDesc;
    IDirectPlay8Address*   pDP8AddressHost  = NULL;
    IDirectPlay8Address*   pDP8AddressLocal = NULL;
    WCHAR*                 wszHostName      = NULL;

    m_hConnectCompleteEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	strncpy(m_szServerAddr, pszServerAddr, sizeof(CHAR) * 23);

	g_eConnectionServerType[nIndexDPInstance] = (eAcDPServerType) nServerType;

    // Create IDirectPlay8Client
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Client, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectPlay8Client, 
                                       (LPVOID*) &g_pDPClient[nIndexDPInstance] ) ) )
	{
        DXTRACE_ERR( TEXT("CoCreateInstance"), hr );
		LeaveCriticalSection(&g_csPlayerContext);
		return ACDP_CONNECT_ERR;
	}
	LeaveCriticalSection(&g_csPlayerContext);

	APP_HANDLER_CONTEXT	*pstHandlerContext = new(APP_HANDLER_CONTEXT);
	pstHandlerContext->pvThisClass = this;
	pstHandlerContext->nConnectionIndex = nIndexDPInstance + 1;
	
    // Init IDirectPlay8Client
    if( FAILED( hr = g_pDPClient[nIndexDPInstance]->Initialize( pstHandlerContext, DirectPlayMessageHandler, 0 ) ) )
	{
        DXTRACE_ERR( TEXT("Initialize"), hr );
		return ACDP_CONNECT_ERR;
	}

	// Connnect DPServer

    // Create the local device address object
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Address, NULL, 
                                       CLSCTX_ALL, IID_IDirectPlay8Address,
                                       (LPVOID*) &pDP8AddressLocal ) ) )
    {
        DXTRACE_ERR( TEXT("CoCreateInstance"), hr );
        goto LCleanup;
    }

	DPN_CAPS	dpnCaps;
	ZeroMemory(&dpnCaps, sizeof(dpnCaps));
	dpnCaps.dwSize = sizeof(dpnCaps);
	dpnCaps.dwConnectTimeout = ACDP_CONNECT_TIMEOUT;
	dpnCaps.dwConnectRetries = ACDP_CONNECT_RETRIES;
	dpnCaps.dwTimeoutUntilKeepAlive = ACDP_TIMEOUT_KEEPALIVE;

	hr = g_pDPClient[nIndexDPInstance]->SetCaps(&dpnCaps, 0);
	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("SetCaps"), hr);
		goto LCleanup;
	}

    // Set IP service provider
    if( FAILED( hr = pDP8AddressLocal->SetSP( &CLSID_DP8SP_TCPIP ) ) )
    {
        DXTRACE_ERR( TEXT("SetSP"), hr );
        goto LCleanup;
    }

    // Create the remote host address object
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Address, NULL, 
                                       CLSCTX_ALL, IID_IDirectPlay8Address,
                                       (LPVOID*) &pDP8AddressHost ) ) )
    {
        DXTRACE_ERR( TEXT("CoCreateInstance"), hr );
        goto LCleanup;
    }

    // Set IP service provider
    if( FAILED( hr = pDP8AddressHost->SetSP( &CLSID_DP8SP_TCPIP ) ) )
    {
        DXTRACE_ERR( TEXT("SetSP"), hr );
        goto LCleanup;
    }

    // Set the remote host name (if provided)
    //TCHAR strIPAddress[23];
    //GetDlgItemText( hDlg, IDC_IP_ADDRESS, strIPAddress, MAX_PATH );

	//strncpy(strIPAddress, "61.96.38.110:9998", 22);

    if( m_szServerAddr != NULL && m_szServerAddr[0] != 0 )
    {
        DWORD dwPort = 0;

        // Parse out port if it exists (expected form of "xxx.xxx.xxx.xxx:port")
        TCHAR* strPort = _tcschr( m_szServerAddr, TEXT(':') );
        if( NULL != strPort )
        {
            // Chop off :port from end of strIPAddress
            TCHAR* strEndOfIP = strPort;
            *strEndOfIP = 0;

            // Get port number from strPort
            strPort++;
            dwPort = _ttoi( strPort );
        }

        wszHostName = new WCHAR[_tcslen(m_szServerAddr)+1];
        DXUtil_ConvertGenericStringToWide( wszHostName, m_szServerAddr, -1);

        hr = pDP8AddressHost->AddComponent( DPNA_KEY_HOSTNAME, wszHostName, 
                                            (wcslen(wszHostName)+1)*sizeof(WCHAR), 
                                            DPNA_DATATYPE_STRING );
        if( FAILED(hr) )
        {
            DXTRACE_ERR( TEXT("AddComponent"), hr );
            goto LCleanup;
        }

        // If a port was specified in the IP string, then add it.
        // Games will typically hard code the port so the user need not know it
        if( dwPort != 0 )
        {
            hr = pDP8AddressHost->AddComponent( DPNA_KEY_PORT, 
                                                &dwPort, sizeof(dwPort),
                                                DPNA_DATATYPE_DWORD );
            if( FAILED(hr) )
            {
                DXTRACE_ERR( TEXT("AddComponent"), hr );
                goto LCleanup;
            }
        }
    }

	/*
    ZeroMemory( &dpnAppDesc, sizeof( DPN_APPLICATION_DESC ) );
    dpnAppDesc.dwSize = sizeof( DPN_APPLICATION_DESC );
    dpnAppDesc.guidApplication = g_guidApp;

    // Enumerate all StressMazeApp hosts running on IP service providers
    hr = g_pDPClient->EnumHosts( &dpnAppDesc, pDP8AddressHost, 
                                 pDP8AddressLocal, NULL, 
                                 0, INFINITE, 0, INFINITE, NULL, 
                                 &m_hEnumAsyncOp, 0 );
    if( FAILED(hr) )
    {
        if( hr != DPNERR_INVALIDDEVICEADDRESS && 
            hr != DPNERR_ADDRESSING ) // This will be returned if the ip address is is invalid. 
            DXTRACE_ERR( TEXT("EnumHosts"), hr );
        goto LCleanup;
	}
	*/

    // Set the peer info
    WCHAR wszPeerName[128];
    DXUtil_ConvertGenericStringToWide(wszPeerName, "ArchLord Client", -1);

    DPN_PLAYER_INFO dpPlayerInfo;
    ZeroMemory( &dpPlayerInfo, sizeof(DPN_PLAYER_INFO) );
    dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
    dpPlayerInfo.pwszName = wszPeerName;

    // Set the peer info, and use the DPNOP_SYNC since by default this
    // is an async call.  If it is not DPNOP_SYNC, then the peer info may not
    // be set by the time we call Connect() below.
    if( FAILED( hr = g_pDPClient[nIndexDPInstance]->SetClientInfo( &dpPlayerInfo, NULL, NULL, DPNOP_SYNC ) ) )
	{
        DXTRACE_ERR( TEXT("SetPeerInfo"), hr );
		goto LCleanup;
	}

    ResetEvent( m_hConnectCompleteEvent );

	ZeroMemory( &dpnAppDesc, sizeof( DPN_APPLICATION_DESC ) );
    dpnAppDesc.dwSize = sizeof( DPN_APPLICATION_DESC );
    dpnAppDesc.guidApplication = g_guidApp;

	BOOL	bServer;
	bServer = FALSE;

    // Connect to an existing session. DPNCONNECT_OKTOQUERYFORADDRESSING allows
    // DirectPlay to prompt the user using a dialog box for any device address
    // or host address information that is missing
    // We also pass in copies of the app desc and host addr, since pDPHostEnumSelected
    // might be deleted from another thread that calls SessionsDlgExpireOldHostEnums().
    // This process could also be done using reference counting instead.
    hr = g_pDPClient[nIndexDPInstance]->Connect( &dpnAppDesc,       // the application desc
											     pDP8AddressHost,      // address of the host of the session
								                 pDP8AddressLocal,    // address of the local device the enum responses were received on
												 NULL, NULL,                          // DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
												 &bServer, sizeof(bServer),                             // user data, user data size
												 NULL, &m_hConnectAsyncOp,            // async context, async handle,
												 NULL ); // flags
    if( hr != E_PENDING && FAILED(hr) )
	{
        DXTRACE_ERR( TEXT("Connect"), hr );
		goto LCleanup;
	}

    WaitForSingleObject(m_hConnectCompleteEvent, INFINITE);

	

    if (FAILED(m_hrConnectComplete))
    {
        DXTRACE_ERR( TEXT("DPN_MSGID_CONNECT_COMPLETE"), m_hrConnectComplete );
#ifdef	_DEBUG
        MessageBox(NULL, TEXT("Unable to join game."), "AcDP8", 1);
        hr = m_hrConnectComplete;
#endif	_DEBUG

		m_bDPConnected = FALSE;
    }
	else
		m_bDPConnected = TRUE;

    CloseHandle( m_hConnectCompleteEvent );

LCleanup:
    SAFE_RELEASE( pDP8AddressHost);
    SAFE_RELEASE( pDP8AddressLocal );
    SAFE_DELETE( wszHostName );

    if( hr == DPNERR_PENDING )
        hr = DPN_OK;

	if (m_bDPConnected)
	{
		++g_nNumDPInstance;
		return (nIndexDPInstance + 1);
	}

    return ACDP_CONNECT_ERR;
}

BOOL AcDP8::StopDP()
{
	if( m_bStartDP )
	{
		m_bStartDP = FALSE;

		CloseHandle(m_hCleanUpThread);
		CloseHandle(m_hCleanUpEvent);

		EnterCriticalSection(&g_csPlayerContext);
		// Cleanup DirectPlay and helper classes
		for (int i = 0; i < g_nNumDPInstance; ++i)
		{
			if (g_pDPClient[i])
			{
				g_pDPClient[i]->Close(0);
				SAFE_RELEASE(g_pDPClient[i]);
			}
		}
		LeaveCriticalSection(&g_csPlayerContext);

		DeleteCriticalSection(&g_csPlayerContext);

		CoUninitialize();
	}

    return TRUE;

}

BOOL AcDP8::Close(INT16 nServerID)
{
	BOOL	bResult = FALSE;

	EnterCriticalSection(&g_csPlayerContext);

	if (nServerID < ACDP_MAX_DPCLIENT_INSTANCE && g_pDPClient[nServerID])
	{
		g_pDPClient[nServerID]->Close(0);
		SAFE_RELEASE(g_pDPClient[nServerID]);

		//CopyMemory(g_pDPClient + nServerID, g_pDPClient + nServerID + 1, 
		//	sizeof(IDirectPlay8Client *) * (g_nNumDPInstance - nServerID - 1));

		g_pDPClient[nServerID] = NULL;

		--g_nNumDPInstance;

		bResult = TRUE;
	}

	LeaveCriticalSection(&g_csPlayerContext);

	return bResult;
}

INT16 AcDP8::Send(CHAR* pData, INT16 nDataSize, INT16 nSendType, INT16 nServerID)
{
	if (nDataSize > APPACKET_MAX_PACKET_SIZE || nSendType < 0)
		return (-1);

	HRESULT	hr;

	// SelfCID 세팅
	*((INT32 *) (((CHAR *) pData) + sizeof(UINT16) + sizeof(INT8))) = m_lSelfCID;

	DPN_BUFFER_DESC bufferDesc;
	bufferDesc.dwBufferSize = nDataSize;
	bufferDesc.pBufferData  = (BYTE*) pData;
	
	DWORD dwFlags = 0;
	dwFlags = DPNSEND_NOLOOPBACK;

	DPNHANDLE hAsync;
	if (nServerID > 0) 
	{
		if (g_pDPClient[nServerID - 1])
		{
			hr = g_pDPClient[nServerID - 1]->Send(&bufferDesc, 1, 0, NULL, &hAsync, dwFlags);

			if (FAILED(hr))
			{
				DXTRACE_ERR(TEXT("AcDP8::Send()"), hr);
				return (-1);
			}
		}

		return nDataSize;
	}

	switch (nSendType) {
	case ACDP_SEND_GAMESERVERS:
		{
			for (int i = 0; i < ACDP_MAX_DPCLIENT_INSTANCE; ++i)
			{
				if (g_pDPClient[i] && g_eConnectionServerType[i] == ACDP_SERVER_TYPE_GAMESERVER)
				{
					hr = g_pDPClient[i]->Send(&bufferDesc, 1, 0, NULL, &hAsync, dwFlags);

					if (FAILED(hr))
					{
						DXTRACE_ERR(TEXT("AcDP8::Send()"), hr);
						return (-1);
					}
				}
			}
		}
		break;

	case ACDP_SEND_SERVER:
		{
			hr = g_pDPClient[nServerID - 1]->Send(&bufferDesc, 1, 0, NULL, &hAsync, dwFlags);

			if (FAILED(hr))
			{
				DXTRACE_ERR(TEXT("AcDP8::Send()"), hr);
				return (-1);
			}
		}
		break;

	case ACDP_SEND_DEALSERVER:
		{
			for (int i = 0; i < ACDP_MAX_DPCLIENT_INSTANCE; ++i)
			{
				if (g_pDPClient[i] && g_eConnectionServerType[i] == ACDP_SERVER_TYPE_DEALSERVER)
				{
					hr = g_pDPClient[i]->Send(&bufferDesc, 1, 0, NULL, &hAsync, dwFlags);

					if (FAILED(hr))
					{
						DXTRACE_ERR(TEXT("AcDP8::Send()"), hr);
						return (-1);
					}
				}
			}
		}
		break;

	case ACDP_SEND_RECRUITSERVER:
		{
			for (int i = 0; i < ACDP_MAX_DPCLIENT_INSTANCE; ++i)
			{
				if (g_pDPClient[i] && g_eConnectionServerType[i] == ACDP_SERVER_TYPE_RECRUITSERVER)
				{
					hr = g_pDPClient[i]->Send(&bufferDesc, 1, 0, NULL, &hAsync, dwFlags);

					if (FAILED(hr))
					{
						DXTRACE_ERR(TEXT("AcDP8::Send()"), hr);
						return (-1);
					}
				}
			}
		}
		break;

	case ACDP_SEND_AUCTIONSERVER:
		{
			for (int i = 0; i < ACDP_MAX_DPCLIENT_INSTANCE; ++i)
			{
				if (g_pDPClient[i] && g_eConnectionServerType[i] == ACDP_SERVER_TYPE_AUCTIONSERVER)
				{
					hr = g_pDPClient[i]->Send(&bufferDesc, 1, 0, NULL, &hAsync, dwFlags);

					if (FAILED(hr))
					{
						DXTRACE_ERR(TEXT("AcDP8::Send()"), hr);
						return (-1);
					}
				}
			}
		}
		break;

	case ACDP_SEND_LOGINSERVER:
		{
			for (int i = 0; i < ACDP_MAX_DPCLIENT_INSTANCE; ++i)
			{
				if (g_pDPClient[i] && g_eConnectionServerType[i] == ACDP_SERVER_TYPE_LOGINSERVER)
				{
					hr = g_pDPClient[i]->Send(&bufferDesc, 1, 0, NULL, &hAsync, dwFlags);

					if (FAILED(hr))
					{
						DXTRACE_ERR(TEXT("AcDP8::Send()"), hr);
						return (-1);
					}
				}
			}
		}
		break;
	}

	/*
	MessageBox(NULL, "success send data", "AcDP8", 1);
	*/

	return nDataSize;
}

HRESULT WINAPI AcDP8::DirectPlayMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
{
    // Try not to stay in this message handler for too long, otherwise
    // there will be a backlog of data.  The best solution is to 
    // queue data as it comes in, and then handle it on other threads.
    
    // This function is called by the DirectPlay message handler pool of 
    // threads, so be careful of thread synchronization problems with shared memory

	HRESULT	hr = S_OK;

	APP_HANDLER_CONTEXT *pHandlerContext = (APP_HANDLER_CONTEXT *) pvUserContext;

	AcDP8	*pThis = (AcDP8 *) pHandlerContext->pvThisClass;

    switch (dwMessageId)
    {
        case DPN_MSGID_TERMINATE_SESSION:
        {
            PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
            pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;

			// 접속 종료...
			// 적당한 처리를 한다.

			if (pHandlerContext->nConnectionIndex < 1)
				break;

			pThis->nCleanUpInstance = pHandlerContext->nConnectionIndex - 1;
			SetEvent(pThis->m_hCleanUpEvent);

			if (g_nNumDPInstance > 0)
			{
				// 마고자 수정.
				TRACE( "disconnected by server , AcDP8\n" );
				//MessageBox(NULL, "disconnected by server", "AcDP8", 1);
			}

			if (pTerminateSessionMsg->pvTerminateData != NULL)
			{
                DWORD* pdw = (DWORD*) pTerminateSessionMsg->pvTerminateData;
                if (*pdw != DISCONNNECT_REASON_CLIENT_NORMAL)
					MessageBox(NULL, "disconnected by server", "AcDP8", 1);
			}
			else
			{
				MessageBox(NULL, "disconnected by server", "AcDP8", 1);
			}

            break;
        }

		case DPN_MSGID_DESTROY_PLAYER:
		{
			PDPNMSG_DESTROY_PLAYER	pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pMsgBuffer;
		}

        case DPN_MSGID_RECEIVE:
        {
            PDPNMSG_RECEIVE pReceiveMsg;
            pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;
			
			if (!pReceiveMsg)
				return FALSE;

			

			// 데이타를 받았다.
			// 음냐... 받은데이타를 처리한다. 큐에 넣던 뭘 하던...

			if (pThis->m_queueRecv.Push(pReceiveMsg->pReceiveData, pReceiveMsg->dwReceiveDataSize, pHandlerContext->nConnectionIndex) == FALSE)
			{
				// 큐에 넣는거 실패... 에러 리턴한다.

				// hr = 에러코드

				MessageBox(NULL, "Received Queue is fulled", "AcDP8", 1);
			}

            break;
        }

		case DPN_MSGID_CONNECT_COMPLETE:
		{
            PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg;
            pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE)pMsgBuffer;

            // Set m_hrConnectComplete, then set an event letting
            // everyone know that the DPN_MSGID_CONNECT_COMPLETE msg
            // has been handled

            pThis->m_hrConnectComplete = pConnectCompleteMsg->hResultCode;
            SetEvent( pThis->m_hConnectCompleteEvent );
		}

			break;
    }

	return hr;
}

VOID AcDP8::DXUtil_ConvertGenericStringToWide(WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar)
{
    if( wstrDestination==NULL || tstrSource==NULL )
        return;

#ifdef _UNICODE
    if( cchDestChar == -1 )
	    wcscpy( wstrDestination, tstrSource );
    else
	    wcsncpy( wstrDestination, tstrSource, cchDestChar );
#else
    DXUtil_ConvertAnsiStringToWide( wstrDestination, tstrSource, cchDestChar );
#endif
}

VOID AcDP8::DXUtil_ConvertAnsiStringToWide( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar )
{
    if( wstrDestination==NULL || strSource==NULL )
        return;

    if( cchDestChar == -1 )
        cchDestChar = strlen(strSource)+1;

    MultiByteToWideChar( CP_ACP, 0, strSource, -1, 
                         wstrDestination, cchDestChar-1 );

    wstrDestination[cchDestChar-1] = 0;
}

BOOL AcDP8::IsStartDP()
{
	return m_bStartDP;
}

//	AcuQueueFIFO class member functions
///////////////////////////////////////////////////////////////////////////////

//		constructor
//	Functions
//		- none
///////////////////////////////////////////////////////////////////////////////
AcuQueueFIFO::AcuQueueFIFO()
{
	m_nHead	= 0;
	m_nTail	= 0;

	m_Mutex.Init();
}

//		destructor
//	Functions
//		- 처음 큐에 할당된 메모리를 해제한다.
///////////////////////////////////////////////////////////////////////////////
AcuQueueFIFO::~AcuQueueFIFO()
{
	if (m_pQueueData != NULL)
	{
		GlobalFree(m_pQueueData);
	}

	m_Mutex.Destroy();
}

//		Initialize
//	Functions
//		- initialize queue member variables
//		- memory allocate
//	Arguments
//		- nQueueSize : 
//		- nDataSize :
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL AcuQueueFIFO::Initialize(INT16 nQueueSize)
{
	//m_Mutex.WLock();

	m_nQueueSize = nQueueSize;

	m_pQueueData = (ACU_QUEUE_DATA *) GlobalAlloc(GMEM_FIXED, sizeof(ACU_QUEUE_DATA) * nQueueSize);
	if (m_pQueueData == NULL)
	{
		// memory allocation error. log error
		return FALSE;
	}

	m_nNumData = 0;

	//m_Mutex.Release();

	return TRUE;
}

//		Pop
//	Functions
//		- 큐에있는 데이타중 젤 앞에 있는 넘을 꺼낸다.
//	Arguments
//		- pBuffer : 꺼낸 데이타 저장할 포인터
//	Return value
//		- ULONG : 꺼낸 데이타 크기
///////////////////////////////////////////////////////////////////////////////
ULONG AcuQueueFIFO::Pop(PVOID pBuffer)
{
	m_Mutex.WLock();

	if (m_nNumData == 0)
	{
		// queue is empty
		m_Mutex.Release();
		return 0;
	}

	CopyMemory(pBuffer, m_pQueueData + m_nTail, sizeof(ACU_QUEUE_DATA));
	ULONG ulBufferSize = m_pQueueData[m_nTail].ulBufferSize;

	--m_nNumData;
	++m_nTail;

	if (m_nTail == (m_nQueueSize - 1))
		m_nTail = 0;

	m_Mutex.Release();

	return ulBufferSize;
}

//		Push
//	Functions
//		- queue에 데이타를 집어 넣는다.
//	Arguments
//		- pBuffer : 집어넣을 데이터
//		- ulDataSize : 데이타 크기
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
INT16 AcuQueueFIFO::Push(PVOID pBuffer, ULONG ulDataSize, UINT32 ulDPNID)
{
	m_Mutex.WLock();

	if (m_nNumData == m_nQueueSize)
	{
		// queue is full
		m_Mutex.Release();
		return FALSE;
	}

	CopyMemory(m_pQueueData[m_nHead].cBufferData, pBuffer, ulDataSize);
	m_pQueueData[m_nHead].ulBufferSize = ulDataSize;
	m_pQueueData[m_nHead].ulDPNID = ulDPNID;

	++m_nNumData;
	++m_nHead;

	if (m_nHead == (m_nQueueSize - 1))
		m_nHead = 0;

	m_Mutex.Release();

	return TRUE;
}

//		Reset
//	Functions
//		- 큐를 초기화한다.
//	Arguments
//		- none
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
INT16 AcuQueueFIFO::Reset()
{
	m_Mutex.WLock();

	if (m_pQueueData != NULL)
	{
		ZeroMemory(m_pQueueData, sizeof(ACU_QUEUE_DATA) * m_nQueueSize);
	}

	m_nNumData = 0;
	m_nHead = 0;
	m_nTail = 0;

	m_Mutex.Release();

	return TRUE;
}

