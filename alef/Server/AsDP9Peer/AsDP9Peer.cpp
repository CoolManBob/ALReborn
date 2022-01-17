//----------------------------------------------------------------------------
// File: ChatPeer.cpp
//
// Desc: The main game file for the ChatPeer sample.  It connects 
//       players together with two dialog boxes to prompt users on the 
//       connection settings to join or create a session. After the user 
//       connects to a session, the sample displays a multiplayer stage. 
// 
//       After a new game has started the sample begins a very simplistic 
//       chat session where users can send text to each other.
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------

#include "AsDP9Peer.h"
#include "DxUtil.h"
#include <dxerr9.h>
#include <tchar.h>
#include <stdio.h>

#include "objbase.h"

#include "AsDefine.h"

//-----------------------------------------------------------------------------
// Player context locking defines
//-----------------------------------------------------------------------------
CRITICAL_SECTION g_csPlayerContext;
#define PLAYER_LOCK()                   EnterCriticalSection( &g_csPlayerContext ); 
#define PLAYER_ADDREF( pPlayerInfo )    if( pPlayerInfo ) pPlayerInfo->lRefCount++;
#define PLAYER_RELEASE( pPlayerInfo )   if( pPlayerInfo ) { pPlayerInfo->lRefCount--; if( pPlayerInfo->lRefCount <= 0 ) SAFE_DELETE( pPlayerInfo ); } pPlayerInfo = NULL;
#define PLAYER_UNLOCK()                 LeaveCriticalSection( &g_csPlayerContext );


//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
#define MAX_PLAYER_NAME         12
#define SERVER_PORT           9999


// This GUID allows DirectPlay to find other instances of the same game on
// the network.  So it must be unique for every game, and the same for 
// every instance of that game.  // {876A3036-FFD7-46bc-9209-B42F617B9BE7}
GUID *g_pguidApp;

struct APP_PLAYER_INFO
{
    LONG  lRefCount;                        // Ref count so we can cleanup when all threads 
                                            // are done w/ this object
    DPNID dpnidPlayer;                      // DPNID of player
    //TCHAR strPlayerName[MAX_PLAYER_NAME];   // Player name

	INT32 lServerID;						// server id
};



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
IDirectPlay8Peer*  g_pDP[ASDP9_PEER_MAX_CONNECTION];    // DirectPlay peer object
INT16			   g_nNumDPInstance				 = 0;
//CNetConnectWizard* g_pNetConnectWizard           = NULL;    // Connection wizard
//CSessionInfo*      g_pSessionInfo                = NULL;    // Session Information
//IDirectPlay8LobbiedApplication* g_pLobbiedApp    = NULL;    // DirectPlay lobbied app 
//BOOL               g_bWasLobbyLaunched           = FALSE;   // TRUE if lobby launched
HINSTANCE          g_hInst                       = NULL;    // HINST of app
//HWND               g_hDlg                        = NULL;    // HWND of main dialog
DPNID              g_dpnidLocalPlayer            = 0;       // DPNID of local player
DPNID              g_dpnidHostPlayer             = 0;       // DPNID of host player
LONG               g_lNumberOfActivePlayers      = 0;       // Number of players currently in game
TCHAR              g_strAppName[256]             = TEXT("ArchLord Server");
TCHAR              g_strLocalPlayerName[MAX_PATH];          // Local player name
TCHAR              g_strSessionName[MAX_PATH];              // Session name
TCHAR              g_strPreferredProvider[MAX_PATH];        // Provider string

HANDLE				g_hConnectCompleteEvent;
HRESULT				g_hrConnectComplete;


AsCMDQueue*			g_pqueueRecvPeer;
INT32				g_lSelfCID;


// Change compiler pack alignment to be BYTE aligned, and pop the current value
#pragma pack( push, 1 )

// Pop the old pack alignment
#pragma pack( pop )


//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------
HRESULT WINAPI DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
HRESULT        InitDirectPlay();
HRESULT        GetHostnamePortString( TCHAR* str, LPDIRECTPLAY8ADDRESS pAddress, DWORD dwBufferLen );


BOOL AsDP9Peer::Initialize(GUID *pguidApp, AsCMDQueue *pqueueRecv)
{
	if (!pguidApp || !pqueueRecv)
		return FALSE;

	g_pqueueRecvPeer = pqueueRecv;
	g_pguidApp = pguidApp;

    InitializeCriticalSection( &g_csPlayerContext );

	for (int i = 0; i < ASDP9_PEER_MAX_CONNECTION; i++)
		g_pDP[i] = NULL;

	g_lSelfCID	= AP_INVALID_SERVERID;

	return TRUE;
}

INT16 AsDP9Peer::Open(CHAR *szIPAddress)
{
	HRESULT	hr;
    DPN_APPLICATION_DESC   dpnAppDesc;
    IDirectPlay8Address*   pDP8AddressHost  = NULL;
    IDirectPlay8Address*   pDP8AddressLocal = NULL;
    WCHAR*                 wszHostName      = NULL;

    BOOL    bConnectSuccess = FALSE;

    if( szIPAddress != NULL && szIPAddress[0] != 0 )
	{
		return (-1);
	}

    if( FAILED( hr = InitDirectPlay() ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("InitDirectPlay"), hr );
        goto LCleanup;
    }

	g_hConnectCompleteEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

	DPN_CAPS	dpnCaps;
	ZeroMemory(&dpnCaps, sizeof(dpnCaps));
	dpnCaps.dwSize = sizeof(dpnCaps);
	dpnCaps.dwConnectTimeout = ASDP_CONNECT_TIMEOUT;
	dpnCaps.dwConnectRetries = ASDP_CONNECT_RETRIES;
	dpnCaps.dwTimeoutUntilKeepAlive = ASDP_TIMEOUT_KEEPALIVE;

	hr = g_pDP[g_nNumDPInstance]->SetCaps(&dpnCaps, 0);
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(TEXT("SetCaps"), hr);
		goto LCleanup;
	}

    // Create the local device address object
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Address, NULL, 
                                       CLSCTX_ALL, IID_IDirectPlay8Address,
                                       (LPVOID*) &pDP8AddressLocal ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("CoCreateInstance"), hr );
        goto LCleanup;
    }

    // Set IP service provider
    if( FAILED( hr = pDP8AddressLocal->SetSP( &CLSID_DP8SP_TCPIP ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("SetSP"), hr );
        goto LCleanup;
    }

    // Create the remote host address object
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Address, NULL, 
                                       CLSCTX_ALL, IID_IDirectPlay8Address,
                                       (LPVOID*) &pDP8AddressHost ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("CoCreateInstance"), hr );
        goto LCleanup;
    }

    // Set IP service provider
    if( FAILED( hr = pDP8AddressHost->SetSP( &CLSID_DP8SP_TCPIP ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("SetSP"), hr );
        goto LCleanup;
    }

    // Set the remote host name (if provided)
	CHAR szTempAddress[23];

	CopyMemory(szTempAddress, szIPAddress, sizeof(CHAR) * 23);

    DWORD dwPort;
	dwPort = 0;

    // Parse out port if it exists (expected form of "xxx.xxx.xxx.xxx:port")
    TCHAR* strPort;
	strPort = _tcschr( szTempAddress, TEXT(':') );
    if( NULL != strPort )
    {
        // Chop off :port from end of strIPAddress
        TCHAR* strEndOfIP = strPort;
        *strEndOfIP = 0;

        // Get port number from strPort
        strPort++;
        dwPort = _ttoi( strPort );
    }

    wszHostName = new WCHAR[_tcslen(szTempAddress)+1];
    DXUtil_ConvertGenericStringToWide( wszHostName, szTempAddress, -1);

    hr = pDP8AddressHost->AddComponent( DPNA_KEY_HOSTNAME, wszHostName, 
                                        (wcslen(wszHostName)+1)*sizeof(WCHAR), 
                                        DPNA_DATATYPE_STRING );
    if( FAILED(hr) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("AddComponent"), hr );
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
            DXTRACE_ERR_MSGBOX( TEXT("AddComponent"), hr );
            goto LCleanup;
        }
    }

    // Set the peer info
    //WCHAR wszPeerName[MAX_PATH];
    //DXUtil_ConvertGenericStringToWideCch( wszPeerName, m_strLocalPlayerName, MAX_PATH );

    DPN_PLAYER_INFO dpPlayerInfo;
    ZeroMemory( &dpPlayerInfo, sizeof(DPN_PLAYER_INFO) );
    dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
    //dpPlayerInfo.pwszName = wszPeerName;

    // Set the peer info, and use the DPNOP_SYNC since by default this
    // is an async call.  If it is not DPNOP_SYNC, then the peer info may not
    // be set by the time we call Connect() below.
    if( FAILED( hr = g_pDP[g_nNumDPInstance]->SetPeerInfo( &dpPlayerInfo, NULL, NULL, DPNOP_SYNC ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("SetPeerInfo"), hr );
        goto LCleanup;
    }

    ZeroMemory( &dpnAppDesc, sizeof( DPN_APPLICATION_DESC ) );
    dpnAppDesc.dwSize = sizeof( DPN_APPLICATION_DESC );
    dpnAppDesc.guidApplication = *g_pguidApp;

    ResetEvent( g_hConnectCompleteEvent );

	DPNHANDLE	hConnectAsyncOp;

    // Connect to an existing session. DPNCONNECT_OKTOQUERYFORADDRESSING allows
    // DirectPlay to prompt the user using a dialog box for any device address
    // or host address information that is missing
    // We also pass in copies of the app desc and host addr, since pDPHostEnumSelected
    // might be deleted from another thread that calls SessionsDlgExpireOldHostEnums().
    // This process could also be done using reference counting instead.
    hr = g_pDP[g_nNumDPInstance]->Connect( &dpnAppDesc,       // the application desc
                         pDP8AddressHost,					  // address of the host of the session
                         pDP8AddressLocal,					  // address of the local device the enum responses were received on
                         NULL, NULL,                          // DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
                         NULL, 0,                             // user data, user data size
                         NULL,                                // player context,
                         NULL, &hConnectAsyncOp,              // async context, async handle,
                         DPNCONNECT_OKTOQUERYFORADDRESSING ); // flags
    if( FAILED(hr) && hr != E_PENDING )
    {
        DXTRACE_ERR_MSGBOX( TEXT("Connect"), hr );
        goto LCleanup;
    }

    WaitForSingleObject(g_hConnectCompleteEvent, 5000);

    if (FAILED(g_hrConnectComplete))
    {
		bConnectSuccess = FALSE;
    }
	else
	{
		bConnectSuccess = TRUE;
	}

    CloseHandle( g_hConnectCompleteEvent );

LCleanup:
	if (bConnectSuccess)
	{
		g_nNumDPInstance++;
		return (g_nNumDPInstance - 1);
	}
    else if (g_pDP[g_nNumDPInstance])
    {
        g_pDP[g_nNumDPInstance]->Close(0);
        SAFE_RELEASE( g_pDP[g_nNumDPInstance] );
    }

	return (-1);
}

BOOL AsDP9Peer::Close(INT16 nDPIndex)
{
	if (nDPIndex >= ASDP9_PEER_MAX_CONNECTION)
		return FALSE;

	if (g_pDP[nDPIndex])
	{
		g_pDP[nDPIndex]->Close(0);
		SAFE_RELEASE(g_pDP[nDPIndex]);
	}

	return FALSE;
}

HRESULT AsDP9Peer::Start()
{
	return S_OK;
}

BOOL AsDP9Peer::Stop()
{
	for (int i = 0; i < ASDP9_PEER_MAX_CONNECTION; i++)
	{
		if (g_pDP[i])
		{
			g_pDP[i]->Close(0);
			SAFE_RELEASE(g_pDP[i]);
		}
	}

    DeleteCriticalSection( &g_csPlayerContext );

    CoUninitialize();

    return TRUE;
}


BOOL AsDP9Peer::IsConnected(INT16 nServerID)
{
	if (nServerID >= ASDP9_PEER_MAX_CONNECTION)
		return FALSE;

	if (g_pDP[nServerID])
		return TRUE;

	return FALSE;
}

INT16 AsDP9Peer::Send(CHAR* pData, INT16 nDataSize, INT16 nServerID)
{
	if (nServerID >= ASDP9_PEER_MAX_CONNECTION || !g_pDP[nServerID])
		return (-1);

	HRESULT	hr;

	// server id setting
	*((INT32 *) (((CHAR *) pData) + sizeof(INT8) /*type size*/)) = g_lSelfCID;

	DPN_BUFFER_DESC bufferDesc;
	bufferDesc.dwBufferSize = nDataSize;
	bufferDesc.pBufferData  = (BYTE*) pData;

	DPNHANDLE hAsync;
	hr = g_pDP[nServerID]->SendTo(DPNID_ALL_PLAYERS_GROUP , &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);

	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("AcDP8::Send()"), hr);
		return (-1);
	}

	return nDataSize;
}

BOOL AsDP9Peer::SetSelfCID(INT32 lServerID)
{
	g_lSelfCID = lServerID;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: InitDirectPlay()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT InitDirectPlay()
{
    DPNHANDLE hLobbyLaunchedConnection = NULL;
    HRESULT hr;

    // Create IDirectPlay8Peer
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay8Peer, NULL, 
                                       CLSCTX_INPROC_SERVER,
                                       IID_IDirectPlay8Peer, 
                                       (LPVOID*) &g_pDP[g_nNumDPInstance] ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("CoCreateInstance"), hr );

    // Turn off parameter validation in release builds
#ifdef _DEBUG
    const DWORD dwInitFlags = 0;
#else
    const DWORD dwInitFlags = DPNINITIALIZE_DISABLEPARAMVAL;
#endif // _DEBUG

	INT16 *pnNumDPInstance = new (INT16);
	if (!pnNumDPInstance)
	{
		SAFE_RELEASE(g_pDP[g_nNumDPInstance]);

		return (-1);
	}

	*pnNumDPInstance = g_nNumDPInstance;

    // Init IDirectPlay8Peer
    if( FAILED( hr = g_pDP[g_nNumDPInstance]->Initialize( pnNumDPInstance, DirectPlayMessageHandler, dwInitFlags ) ) )
	{
		SAFE_RELEASE(g_pDP[g_nNumDPInstance]);

		delete pnNumDPInstance;

        return DXTRACE_ERR_MSGBOX( TEXT("Initialize"), hr );
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DirectPlayMessageHandler
// Desc: Handler for DirectPlay messages.  This function is called by
//       the DirectPlay message handler pool of threads, so be care of thread
//       synchronization problems with shared memory
//-----------------------------------------------------------------------------
HRESULT WINAPI DirectPlayMessageHandler( PVOID pvUserContext, 
                                         DWORD dwMessageId, 
                                         PVOID pMsgBuffer )
{
    // Try not to stay in this message handler for too long, otherwise
    // there will be a backlog of data.  The best solution is to 
    // queue data as it comes in, and then handle it on other threads
    // as this sample shows

    // This function is called by the DirectPlay message handler pool of 
    // threads, so be care of thread synchronization problems with shared memory
    
	INT16	nNumDPInstance = *(INT16 *)pvUserContext;
    HRESULT hReturn = S_OK;

    switch( dwMessageId )
    {
        case DPN_MSGID_CREATE_PLAYER:
        {
            HRESULT hr;
            PDPNMSG_CREATE_PLAYER pCreatePlayerMsg;
            pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;

            // Create a new and fill in a APP_PLAYER_INFO
            APP_PLAYER_INFO* pPlayerInfo = new APP_PLAYER_INFO;
            if( NULL == pPlayerInfo )
                break;

            ZeroMemory( pPlayerInfo, sizeof(APP_PLAYER_INFO) );
            pPlayerInfo->dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;
            pPlayerInfo->lRefCount   = 1;

            // Get the peer info and extract its name
            DWORD dwSize = 0;
            DPN_PLAYER_INFO* pdpPlayerInfo = NULL;
            hr = DPNERR_CONNECTING;
            
            // GetPeerInfo might return DPNERR_CONNECTING when connecting, 
            // so just keep calling it if it does
            while( hr == DPNERR_CONNECTING ) 
                hr = g_pDP[nNumDPInstance]->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );                                
                
            if( hr == DPNERR_BUFFERTOOSMALL )
            {
                pdpPlayerInfo = (DPN_PLAYER_INFO*) new BYTE[ dwSize ];
                if( NULL == pdpPlayerInfo )
                {
                    // Out of memory
                    SAFE_DELETE( pPlayerInfo )
                    break;
                }

                ZeroMemory( pdpPlayerInfo, dwSize );
                pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
                
                hr = g_pDP[nNumDPInstance]->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
                if( SUCCEEDED(hr) )
                {
                    // This stores a extra TCHAR copy of the player name for 
                    // easier access.  This will be redundent copy since DPlay 
                    // also keeps a copy of the player name in GetPeerInfo()
                    //DXUtil_ConvertWideStringToGenericCch( pPlayerInfo->strPlayerName, 
                    //                                  pdpPlayerInfo->pwszName, MAX_PLAYER_NAME );    
                                                       
                    if( pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_LOCAL )
                        g_dpnidLocalPlayer = pCreatePlayerMsg->dpnidPlayer;
                    if( pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_HOST )
                        g_dpnidHostPlayer = pCreatePlayerMsg->dpnidPlayer;
                }

                SAFE_DELETE_ARRAY( pdpPlayerInfo );
            }
                
            // Tell DirectPlay to store this pPlayerInfo 
            // pointer in the pvPlayerContext.
            pCreatePlayerMsg->pvPlayerContext = pPlayerInfo;

            // Update the number of active players, and 
            // post a message to the dialog thread to update the 
            // UI.  This keeps the DirectPlay message handler 
            // from blocking
            InterlockedIncrement( &g_lNumberOfActivePlayers );

            break;
        }

        case DPN_MSGID_DESTROY_PLAYER:
        {
            PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg;
            pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pDestroyPlayerMsg->pvPlayerContext;

            PLAYER_LOCK();                  // enter player context CS
            PLAYER_RELEASE( pPlayerInfo );  // Release player and cleanup if needed
            PLAYER_UNLOCK();                // leave player context CS

            // Update the number of active players, and 
            // post a message to the dialog thread to update the 
            // UI.  This keeps the DirectPlay message handler 
            // from blocking
            InterlockedDecrement( &g_lNumberOfActivePlayers );

			delete (pvUserContext);

            break;
        }

        case DPN_MSGID_HOST_MIGRATE:
        {
            PDPNMSG_HOST_MIGRATE pHostMigrateMsg;
            pHostMigrateMsg = (PDPNMSG_HOST_MIGRATE)pMsgBuffer;

            //if( pHostMigrateMsg->dpnidNewHost == g_dpnidLocalPlayer )
            //    SetWindowText( g_hDlg, TEXT("ChatPeer (Host)") );

            g_dpnidHostPlayer = pHostMigrateMsg->dpnidNewHost;

            break;
        }

        case DPN_MSGID_TERMINATE_SESSION:
        {
            PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
            pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;

            break;
        }

        case DPN_MSGID_RECEIVE:
        {
            PDPNMSG_RECEIVE pReceiveMsg;
            pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pReceiveMsg->pvPlayerContext;           
            if( NULL == pPlayerInfo )
                break;

			stCOMMAND	stRecvCommand;

			stRecvCommand.unType = ASDP_CK_RECEIVE_DATA;
			stRecvCommand.dpnid = nNumDPInstance;
			stRecvCommand.pvCharacter = pPlayerInfo;
			stRecvCommand.unDataSize = (UINT16) pReceiveMsg->dwReceiveDataSize;
			CopyMemory(stRecvCommand.szData, pReceiveMsg->pReceiveData, stRecvCommand.unDataSize);

			g_pqueueRecvPeer->PushCommand(&stRecvCommand);

            // Validate incoming data: A malicious user could modify or create an application
            // to send bogus information; to help guard against logical errors and denial 
            // of service attacks, the size of incoming data should be checked against what
            // is expected.
            //if( pReceiveMsg->dwReceiveDataSize < sizeof(GAMEMSG_GENERIC) )
            //    break;

            //GAMEMSG_GENERIC* pMsg = (GAMEMSG_GENERIC*) pReceiveMsg->pReceiveData;
            //if( pMsg->nType == GAME_MSGID_CHAT )
            //{
                // Validate incoming data
                //if( pReceiveMsg->dwReceiveDataSize > sizeof(GAMEMSG_CHAT) )
                //    break;

                // This message is sent when a player has send a chat message to us, so 
                // post a message to the dialog thread to update the UI.  
                // This keeps the DirectPlay threads from blocking, and also
                // serializes the recieves since DirectPlayMessageHandler can
                // be called simultaneously from a pool of DirectPlay threads.
                //GAMEMSG_CHAT* pChatMessage = (GAMEMSG_CHAT*) pMsg;

                // Make certain the passed string is null terminated
                //DWORD dwStringLength = pReceiveMsg->dwReceiveDataSize - sizeof(GAMEMSG_GENERIC);
                //pChatMessage->strChatString[ dwStringLength - 1 ] = 0;

                // Record the buffer handle so the buffer can be returned later 
                //APP_QUEUED_DATA* pQueuedData = new APP_QUEUED_DATA;
                //if( NULL == pQueuedData )
                //   break;

                //pQueuedData->hBufferHandle = pReceiveMsg->hBufferHandle;
                //pQueuedData->pChatMsg      = pChatMessage;

                // Pass the APP_QUEUED_DATA to the main dialog thread, so it can
                // process it.  It will also cleanup the struct
                //PostMessage( g_hDlg, WM_APP_CHAT, 
                //             pPlayerInfo->dpnidPlayer, (LPARAM) pQueuedData );

                // Tell DirectPlay to assume that ownership of the buffer 
                // has been transferred to the application, and so it will 
                // neither free nor modify it until ownership is returned 
                // to DirectPlay through the ReturnBuffer() call.
                //hReturn = DPNSUCCESS_PENDING;
            //}
            //break;
        }

		case DPN_MSGID_CONNECT_COMPLETE:
		{
            PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg;
            pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE)pMsgBuffer;

            // Set m_hrConnectComplete, then set an event letting
            // everyone know that the DPN_MSGID_CONNECT_COMPLETE msg
            // has been handled
            g_hrConnectComplete = pConnectCompleteMsg->hResultCode;
            SetEvent( g_hConnectCompleteEvent );

			break;
		}
    }

    // Make sure the DirectPlay MessageHandler calls the CNetConnectWizard handler, 
    // so it can be informed of messages such as DPN_MSGID_ENUM_HOSTS_RESPONSE.
    //if( hReturn != DPNSUCCESS_PENDING && SUCCEEDED(hReturn) && g_pNetConnectWizard )
    //    hReturn = g_pNetConnectWizard->MessageHandler( pvUserContext, dwMessageId, pMsgBuffer );
    
    return hReturn;
}




//-----------------------------------------------------------------------------
// Name: GetHostnamePortString
// Desc: Stores the hostname and port number of the given DirectPlay 
//       address in the provided string.
//-----------------------------------------------------------------------------
HRESULT GetHostnamePortString( TCHAR* str, LPDIRECTPLAY8ADDRESS pAddress, DWORD dwBufferLen )
{
    HRESULT hr = S_OK;

    // Sanity check
    if( NULL == str || NULL == pAddress )
        return E_FAIL;

    // Get the hostname string from the DirectPlay address
    WCHAR wstrHostname[ 256 ] = {0};
    DWORD dwSize = sizeof(wstrHostname);
    DWORD dwDataType = 0;
    hr = pAddress->GetComponentByName( L"hostname", wstrHostname, &dwSize, &dwDataType );
    if( FAILED(hr) )
        return hr;

    // Convert from wide character to generic
    TCHAR strHostname[ 256 ] = {0};
    DXUtil_ConvertWideStringToGenericCch( strHostname, wstrHostname, 256 );

    // Get the port value from the DirectPlay address
    DWORD dwPort = 0;
    dwSize = sizeof(DWORD);
    hr = pAddress->GetComponentByName( L"port", &dwPort, &dwSize, &dwDataType );
    if( FAILED(hr) )
        return hr;

    // Copy the address string and null terminate the result
    _sntprintf( str, dwBufferLen, TEXT("%s:%d"), strHostname, dwPort );
    str[ dwBufferLen-1 ] = TEXT('\0');

    return S_OK;
}