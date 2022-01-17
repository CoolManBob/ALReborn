//----------------------------------------------------------------------------
// File: server.cpp
//
// Desc: see main.cpp
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include "ApBase.h"
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <math.h>
#include <tchar.h>
#include <conio.h>
#include <dplay8.h>
#include <dpaddr.h>
#include <dxerr9.h>
#include "DXUtil.h"
#include "AsDP9.h"
//#include "StressMazeGUID.h"




//-----------------------------------------------------------------------------
// Name: AsDP9Server
// Desc: Constructor
//-----------------------------------------------------------------------------
AsDP9Server::AsDP9Server()
{
    m_pDPlay    = NULL;
    m_pServer   = NULL;

    // Initialise the player list
    ZeroMemory( m_PlayerDatas, sizeof(m_PlayerDatas) );
    m_pFirstActivePlayerData = NULL;
    m_pFirstFreePlayerData = m_PlayerDatas;
    for( DWORD i = 1; i < MAX_PLAYER_OBJECTS-1; ++i )
    {
        m_PlayerDatas[i].pNext = &m_PlayerDatas[i+1];
        m_PlayerDatas[i].pPrevious = &m_PlayerDatas[i-1];
    }

    m_PlayerDatas[0].pNext = &m_PlayerDatas[1];
    m_PlayerDatas[MAX_PLAYER_OBJECTS-1].pPrevious = &m_PlayerDatas[MAX_PLAYER_OBJECTS-2];
    m_dwActivePlayerDataCount = 0;
    m_dwPlayerDataUniqueValue = 0;

	m_dwPlayerCount			= 0;
	m_dwPeakPlayerCount		= 0;

	m_wActiveThreadCount	= 0;
	m_wMaxThreadCount		= 0;
	m_fAvgThreadCount		= 0.0;
	m_fAvgThreadTime		= 0.0;
	m_fMaxThreadTime		= 0.0;

	m_lSelfCID	= AP_INVALID_CID;

	ZeroMemory(m_szLocalAddr, sizeof(CHAR) * MAX_DEVICES * 23);
	m_nNumAddr	= 0;

	ZeroMemory(m_pstIDHashBucket, NUM_ID_HASH_BUCKETS * sizeof(PlayerData *));
}


AsDP9Server::~AsDP9Server()
{

}

BOOL AsDP9Server::Initialize(UINT32 nPort, GUID *pguidApp, INT16 *pnServerStatus, ApIOCPDispatcher *pcsIOCPDispatcher,
								   DWORD dwConnectTimeout, DWORD dwConnectRetries, DWORD dwTimeoutUntilKeepAlive)
{
	m_dwPort					= nPort;

	m_pguidApp					= pguidApp;
	m_pnServerStatus			= pnServerStatus;
	m_pcsIOCPDispatcher			= pcsIOCPDispatcher;

	m_dwConnectTimeout			= dwConnectTimeout;
	m_dwConnectRetries			= dwConnectRetries;
	m_dwTimeoutUntilKeepAlive	= dwTimeoutUntilKeepAlive;

	return TRUE;
}

HRESULT	AsDP9Server::StartServer()
{
	SYSTEM_INFO		csSystemInfo;

	GetSystemInfo(&csSystemInfo);

	return Start(csSystemInfo.dwNumberOfProcessors);
}

VOID AsDP9Server::StopServer()
{
	Shutdown();
}

INT16 AsDP9Server::SendTo(DWORD dpnid, PVOID pvData, INT16 nLength)
{
	if (dpnid== 0 || !pvData || !nLength)
		return (-1);

	HRESULT	hr = SendPacket(dpnid, pvData, nLength, TRUE, 0);

	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX( TEXT("SendPacket"), hr);
		return (-1);
	}

	return nLength;
}


INT32 AsDP9Server::GetActivePlayers()
{
	return m_dwActivePlayerDataCount;
}

INT16 AsDP9Server::GetConnectionInfo(DWORD dpnid, UINT32 *pulRoundTripLatencyMS, UINT32 *pulThroughputBPS)
{
	if (dpnid == ASDP_INVALID_DPNID)
		return FALSE;

	DPN_CONNECTION_INFO dpnConnectionInfo;
	dpnConnectionInfo.dwSize = sizeof(dpnConnectionInfo);

	HRESULT hr = m_pDPlay->GetConnectionInfo(dpnid, &dpnConnectionInfo, 0);
	if (FAILED(hr))
	{
#ifdef	_DEBUG
		DXTRACE_ERR_MSGBOX(TEXT("GetConnectionInfo"), hr);
#endif	_DEBUG

		return FALSE;
	}

	if (pulRoundTripLatencyMS)
		*pulRoundTripLatencyMS = dpnConnectionInfo.dwRoundTripLatencyMS;

	if (pulThroughputBPS)
		*pulThroughputBPS = dpnConnectionInfo.dwThroughputBPS;

	return TRUE;
}

IDirectPlay8Server*	AsDP9Server::GetDP8Server()
{
	return m_pDPlay;
}

DPNID AsDP9Server::GetDPNIDClients()
{
	return m_dpnidClients;
}

DPNID AsDP9Server::GetDPNIDServers()
{
	return m_dpnidServers;
}

BOOL AsDP9Server::GetLocalIPAddr(CHAR **pszBuffer, INT16 *pnCount)
{
	if (!pszBuffer || !pnCount)
		return FALSE;

	*pnCount = m_nNumAddr;
	*pszBuffer = (CHAR *)m_szLocalAddr;

	return TRUE;
}

BOOL AsDP9Server::DestroyClient(DWORD dpnid, HRESULT hrReason)
{
	if (dpnid == ASDP_INVALID_DPNID)
		return FALSE;

	HRESULT hr = m_pDPlay->DestroyClient(dpnid, &hrReason, sizeof(hrReason), 0);

	if (FAILED(hr))
	{
		if (hr == DPNERR_NOTHOST || hr == DPNERR_INVALIDPLAYER)
		{
			//WriteLog(AS_LOG_DEBUG, "AsDP8 : DestroyClient()");
			return TRUE;
		}

		//DXTRACE_ERR_MSGBOX(TEXT("DestroyClient"), hr);
		return FALSE;
	}

	//WriteLog(AS_LOG_DEBUG, "AsDP8 : DestroyClient()");

	return TRUE;
}

GUID* AsDP9Server::GetGUID()
{
	return m_pguidApp;
}

INT16* AsDP9Server::GetServerStatus()
{
	return m_pnServerStatus;
}

BOOL AsDP9Server::SetIDToPlayerContext(INT32 lID, DWORD dpnid)
{
	if (dpnid == ASDP_INVALID_DPNID)
		return FALSE;

	PlayerData	*pPlayerData;

	pPlayerData = GetPlayerDataForID(dpnid);
	if (!pPlayerData)
	{
		//WriteLog(AS_LOG_DEBUG, "AsDP9 : SetIDToPlayerContext() 에서 GetPlayerDataForID() 실패. NULL 리턴");
		return FALSE;
	}

	pPlayerData->lID = lID;

	return TRUE;
}

BOOL AsDP9Server::SetAccountNameToPlayerContext(CHAR *szAccountName, DWORD dpnid)
{
	if (!szAccountName || dpnid == ASDP_INVALID_DPNID)
		return FALSE;

	PlayerData	*pPlayerData;

	pPlayerData = GetPlayerDataForID(dpnid);
	if (!pPlayerData)
	{
		//WriteLog(AS_LOG_DEBUG, "AsDP9 : SetIDToPlayerContext() 에서 GetPlayerDataForID() 실패. NULL 리턴");
		return FALSE;
	}

	strncpy(pPlayerData->szAccountName, szAccountName, 32);

	return TRUE;
}

DPNID AsDP9Server::CreateGroup()
{
	/*
	// dpnid group 생성
	DPN_GROUP_INFO	dpnGroupInfo;
	ZeroMemory(&dpnGroupInfo, sizeof(DPN_GROUP_INFO));
	dpnGroupInfo.dwSize = sizeof(DPN_GROUP_INFO);
	dpnGroupInfo.dwInfoFlags = DPNINFO_DATA;
	dpnGroupInfo.pvData = NULL;
	dpnGroupInfo.dwDataSize = 0;
	DPNHANDLE		hAsync;

	DWORD			dpnidGroup;

	HRESULT hr = m_pDPlay->CreateGroup(&dpnGroupInfo, &dpnidGroup, NULL, &hAsync, 0);

	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(TEXT("CreateGroup"), hr);
		return 0;
	}

	return dpnidGroup;
	*/
	return 0;
}

BOOL AsDP9Server::DestroyGroup(DWORD dpnidGroup)
{
	/*
	if (dpnidGroup == ASDP_INVALID_DPNID)
		return FALSE;

	DPNHANDLE	hAsync;

	HRESULT hr = m_pDPlay->DestroyGroup(dpnidGroup, NULL, &hAsync, 0);
	if (FAILED(hr))
	{
		//DXTRACE_ERR_MSGBOX(TEXT("DestroyGroup"), hr);
		return FALSE;
	}

	return TRUE;
	*/
	return FALSE;
}

BOOL AsDP9Server::AddPlayerToGroup(DWORD dpnidGroup, DWORD dpnidPlayer)
{
	/*
	if (dpnidGroup == ASDP_INVALID_DPNID || dpnidPlayer == ASDP_INVALID_DPNID)
		return FALSE;

	DPNHANDLE	hAsync;

	HRESULT hr = m_pDPlay->AddPlayerToGroup(dpnidGroup, dpnidPlayer, (void *) 1, &hAsync, 0);

	if (FAILED(hr))
	{
		//DXTRACE_ERR_MSGBOX(TEXT("AddPlayerToGroup"), hr);
		return FALSE;
	}

	return TRUE;
	*/
	return FALSE;
}

BOOL AsDP9Server::RemovePlayerFromGroup(DWORD dpnidGroup, DWORD dpnidPlayer)
{
	/*
	if (dpnidGroup == ASDP_INVALID_DPNID || dpnidPlayer == ASDP_INVALID_DPNID)
		return FALSE;

	DPNHANDLE	hAsync;

	HRESULT hr = m_pDPlay->RemovePlayerFromGroup(dpnidGroup, dpnidPlayer, (void *) 2, &hAsync, 0);

	if (FAILED(hr))
	{
		//DXTRACE_ERR_MSGBOX(TEXT("RemovePlayerFromGroup"), hr);
		return FALSE;
	}

	return TRUE;
	*/
	return FALSE;
}

BOOL AsDP9Server::SetSelfCID(INT32 lServerID)
{
	m_lSelfCID = lServerID;

	return TRUE;
}


//-----------------------------------------------------------------------------
// Name: Start
// Desc: Create and initialize the DirectPlay server object
//-----------------------------------------------------------------------------
HRESULT AsDP9Server::Start(DWORD dwNumSPThreads)
{
    HRESULT hr;
    PDIRECTPLAY8ADDRESS   pDP8AddrLocal = NULL;

    // Turn off parameter validation in release builds
#ifdef _DEBUG
    const DWORD dwInitFlags = 0;
#else
    const DWORD dwInitFlags = DPNINITIALIZE_DISABLEPARAMVAL;
#endif // _DEBUG

    // Create DirectPlay object
    hr = CoCreateInstance( CLSID_DirectPlay8Server, NULL, 
                           CLSCTX_INPROC_SERVER, IID_IDirectPlay8Server,
                           (LPVOID*) &m_pDPlay );
    if( FAILED(hr) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("CoCreateInstance"), hr );
        goto LCleanup;
    }

    hr = CoCreateInstance( CLSID_DirectPlay8Address, NULL, 
                           CLSCTX_ALL, IID_IDirectPlay8Address, 
                           (LPVOID*) &pDP8AddrLocal );
    if( FAILED(hr) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("CoCreateInstance"), hr );
        goto LCleanup;
    }

    hr = pDP8AddrLocal->SetSP( &CLSID_DP8SP_TCPIP );
    if( FAILED(hr) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("BuildLocalAddress"), hr );
        goto LCleanup;
    }

    hr = pDP8AddrLocal->AddComponent( DPNA_KEY_PORT, 
                                      &m_dwPort, sizeof(m_dwPort),
                                      DPNA_DATATYPE_DWORD );
    if( FAILED(hr) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("BuildLocalAddress"), hr );
        goto LCleanup;
    }

    hr = m_pDPlay->Initialize( this, StaticReceiveHandler, dwInitFlags );
    if( FAILED(hr) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("Initialize"), hr );
        goto LCleanup;
    }
/*
    DPN_CAPS_EX	dpnCaps;
	ZeroMemory(&dpnCaps, sizeof(DPN_CAPS_EX));
	dpnCaps.dwSize						= sizeof(DPN_CAPS_EX);
	dpnCaps.dwConnectTimeout			= m_dwConnectTimeout;
	dpnCaps.dwConnectRetries			= m_dwConnectRetries;
	dpnCaps.dwTimeoutUntilKeepAlive		= m_dwTimeoutUntilKeepAlive;
	dpnCaps.dwMaxRecvMsgSize			= 4096;
	dpnCaps.dwNumSendRetries			= 5;
	dpnCaps.dwMaxSendRetryInterval		= 3000;
	dpnCaps.dwDropThresholdRate			= 10;
	dpnCaps.dwThrottleRate				= 80;
	dpnCaps.dwNumHardDisconnectSends	= 5;
	dpnCaps.dwMaxHardDisconnectPeriod	= 1000;
*/

	DPN_CAPS	dpnCaps;
	ZeroMemory(&dpnCaps, sizeof(dpnCaps));
	dpnCaps.dwSize						= sizeof(DPN_CAPS);
	dpnCaps.dwFlags						= 0;
	dpnCaps.dwConnectTimeout			= m_dwConnectTimeout;
	dpnCaps.dwConnectRetries			= m_dwConnectRetries;
	dpnCaps.dwTimeoutUntilKeepAlive		= m_dwTimeoutUntilKeepAlive;


	hr = m_pDPlay->SetCaps((PDPN_CAPS) &dpnCaps, 0);
	if (FAILED(hr))
	{
		DXTRACE_ERR_MSGBOX(TEXT("SetCaps"), hr);
		goto LCleanup;
	}

// If greater than 0, and less than 128, attempt to set number of threads.
    if((dwNumSPThreads >= MIN_SP_THREADS) && (dwNumSPThreads <= MAX_SP_THREADS))
    {
        // Set the number of SP Threads
        SetNumSPThreads(dwNumSPThreads);

    }

    DPN_APPLICATION_DESC dpnAppDesc;
    ZeroMemory( &dpnAppDesc, sizeof(DPN_APPLICATION_DESC) );
    dpnAppDesc.dwSize = sizeof( DPN_APPLICATION_DESC );
    dpnAppDesc.dwFlags = DPNSESSION_CLIENT_SERVER | DPNSESSION_NODPNSVR;
    dpnAppDesc.guidApplication = *m_pguidApp;
    dpnAppDesc.pwszSessionName = L"ArchLord Server";

    // Set host player context to non-NULL so we can determine which player indication is 
    // the host's.
    hr = m_pDPlay->Host( &dpnAppDesc, &pDP8AddrLocal, 1, NULL, NULL, (void *) 1, 0  );
    if( FAILED(hr) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("Host"), hr );
        goto LCleanup;
    }

	// local ip address setting
    WSADATA WSAData;
    if( WSAStartup (MAKEWORD(1,0), &WSAData) == 0) 
    {
        CHAR strLocalHostName[MAX_PATH];
        gethostname( strLocalHostName, MAX_PATH );
        HOSTENT* pHostEnt = gethostbyname( strLocalHostName );
        if( pHostEnt )
        {
			for (int i = 0; i < MAX_DEVICES; i++)
			{
				_tcscpy( m_szLocalAddr[i], TEXT("") );

				in_addr* pInAddr = (in_addr*) pHostEnt->h_addr_list[i];
				if (!pInAddr)
					break;

				char* strLocalIP = inet_ntoa( *pInAddr );
				if( strLocalIP )
					DXUtil_ConvertAnsiStringToGeneric( m_szLocalAddr[i], strLocalIP, (int)strlen(strLocalIP) + 1 );

				sprintf(m_szLocalAddr[i], "%s:%d", m_szLocalAddr[i], m_dwPort);

				m_nNumAddr++;
			}
        }

        WSACleanup();
    }
	// local ip address setting

	m_dpnidClients = CreateGroup();
	m_dpnidServers = CreateGroup();

LCleanup:
    SAFE_RELEASE( pDP8AddrLocal );

    return hr;
}



//-----------------------------------------------------------------------------
// Name: GetNumSPThreads
// Desc: Get the number of threads for the current service provider
//-----------------------------------------------------------------------------
DWORD AsDP9Server::GetNumSPThreads()
{

    HRESULT             hr = DPN_OK;

    DPN_SP_CAPS         dnSPCaps;
    DWORD               dwFlags = NULL;
    DWORD               dwNumSPThreads = NULL;

    DXTRACE_MSG( TEXT("Attempting to Get SP Thread Count.") );

    dnSPCaps.dwSize = sizeof(DPN_SP_CAPS);
    
    hr = m_pDPlay->GetSPCaps(&CLSID_DP8SP_TCPIP, &dnSPCaps, dwFlags);
    if(hr != DPN_OK)
    {
        DXTRACE_ERR_MSGBOX( TEXT("GetSPCaps error"), hr );
        dwNumSPThreads = 0xffffffff;
    }
    else
    {
        dwNumSPThreads = dnSPCaps.dwNumThreads;
    }

    return dwNumSPThreads;

}


//-----------------------------------------------------------------------------
// Name: SetNumSPThreads
// Desc: Set the number of threads for the current service provider
//-----------------------------------------------------------------------------
void AsDP9Server::SetNumSPThreads(DWORD dwNumSPThreads)
{
    HRESULT             hr = DPN_OK;
    
    DPN_SP_CAPS         dnSPCaps;
    DWORD               dwFlags = NULL;

    dnSPCaps.dwSize = sizeof(DPN_SP_CAPS);
    hr = m_pDPlay->GetSPCaps(&CLSID_DP8SP_TCPIP, &dnSPCaps, dwFlags);
    if(hr == DPN_OK)
    {
        if((dwNumSPThreads >= MIN_SP_THREADS) && (dwNumSPThreads <= MAX_SP_THREADS))
        {
            dnSPCaps.dwNumThreads = dwNumSPThreads;
            
            // Attempt to set the number of SP Threads.
            hr = m_pDPlay->SetSPCaps(&CLSID_DP8SP_TCPIP, &dnSPCaps, dwFlags);
            if(hr != DPN_OK)
            {
                DXTRACE_ERR_MSGBOX( TEXT("SetSPCaps error."), hr );
            }
        }
    }
    else
    {
        DXTRACE_ERR_MSGBOX( TEXT("GetSPCaps error."), hr );
    }

}

//-----------------------------------------------------------------------------
// Name: GetSPBuffer
// Desc: Get the buffer size for the current service provider
//-----------------------------------------------------------------------------
DWORD AsDP9Server::GetSPBuffer()
{

    HRESULT             hr = DPN_OK;

    DPN_SP_CAPS         dnSPCaps;
    DWORD               dwFlags = NULL;
    DWORD               dwSPBufferSize = NULL;

    DXTRACE_MSG( TEXT("Attempting to Get SP Buffer Size.") );

    dnSPCaps.dwSize = sizeof(DPN_SP_CAPS);
    
    hr = m_pDPlay->GetSPCaps(&CLSID_DP8SP_TCPIP, &dnSPCaps, dwFlags);
    if(hr != DPN_OK)
    {
        DXTRACE_ERR_MSGBOX( TEXT("GetSPCaps error"), hr );
        dwSPBufferSize = 0xffffffff;
    }
    else
    {
        dwSPBufferSize = dnSPCaps.dwSystemBufferSize;
    }

    return dwSPBufferSize;

}


//-----------------------------------------------------------------------------
// Name: SetSPBuffer
// Desc: Set the buffer size for the current service provider
//-----------------------------------------------------------------------------
void AsDP9Server::SetSPBuffer(DWORD dwSPBufferSize)
{
    HRESULT             hr = DPN_OK;
    
    DPN_SP_CAPS         dnSPCaps;
    DWORD               dwFlags = NULL;

    dnSPCaps.dwSize = sizeof(DPN_SP_CAPS);
    hr = m_pDPlay->GetSPCaps(&CLSID_DP8SP_TCPIP, &dnSPCaps, dwFlags);
    if(hr == DPN_OK)
    {
        if(dwSPBufferSize <= MAX_SP_BUFFER)
            dnSPCaps.dwSystemBufferSize = dwSPBufferSize;

        // Attempt to set the number of SP Threads.
        hr = m_pDPlay->SetSPCaps(&CLSID_DP8SP_TCPIP, &dnSPCaps, dwFlags);
        if(hr != DPN_OK)
        {
            DXTRACE_ERR_MSGBOX( TEXT("SetSPCaps error."), hr );
        }
    }
    else   
    {
        DXTRACE_ERR_MSGBOX( TEXT("GetSPCaps error."), hr );
    }

}

//-----------------------------------------------------------------------------
// Name: Shutdown
// Desc: Close and release DirectPlay objects
//-----------------------------------------------------------------------------
void AsDP9Server::Shutdown()
{
    if( m_pDPlay != NULL )
        m_pDPlay->Close(0);

    SAFE_RELEASE( m_pDPlay );
}




//-----------------------------------------------------------------------------
// Name: RejectClient
// Desc: Reject the given client's connection request
//-----------------------------------------------------------------------------
HRESULT AsDP9Server::RejectClient( DWORD dwID, HRESULT hrReason )
{
    HRESULT hrRet = S_OK;

    if( m_pDPlay )
    {
        hrRet = m_pDPlay->DestroyClient( dwID, &hrReason, sizeof(hrReason), 0 );
    }

    return hrRet;
}




//-----------------------------------------------------------------------------
// Name: StaticReceiveHandler
// Desc: Static helper for ReceiveHandler
//-----------------------------------------------------------------------------
HRESULT AsDP9Server::StaticReceiveHandler( void *pvContext, DWORD dwMessageType, 
                                             void *pvMessage )
{
    AsDP9Server* pThisObject = (AsDP9Server*) pvContext;

    return pThisObject->ReceiveHandler( pvContext, dwMessageType, pvMessage );
}



//-----------------------------------------------------------------------------
// Name: ReceiveHandler
// Desc: DirectPlay message handler
//-----------------------------------------------------------------------------
HRESULT AsDP9Server::ReceiveHandler( void *pvContext, DWORD dwMessageType, 
                                       void *pvMessage )
{
    switch( dwMessageType )
    {
        case DPN_MSGID_CREATE_PLAYER:
        {
			if (*m_pnServerStatus != GF_SERVER_START)
				return (-1);

            PDPNMSG_CREATE_PLAYER pCreatePlayer = (PDPNMSG_CREATE_PLAYER) pvMessage;

            if( pCreatePlayer->pvPlayerContext == NULL )
			{
				HRESULT	hr;

				// Get the peer info and extract its name
				DWORD dwSize = 0;
				DPN_PLAYER_INFO* pdpPlayerInfo = NULL;
				hr = m_pDPlay->GetClientInfo(pCreatePlayer->dpnidPlayer, 
												pdpPlayerInfo, &dwSize, 0 );
				if (FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL)
				{
					if (hr == DPNERR_INVALIDPLAYER)
					{
						// Ignore this message if this is for the host
						//WriteLog(AS_LOG_DEBUG, "AsDP8 : invalid player");

						break;
					}

					return DXTRACE_ERR_MSGBOX(TEXT("GetClientInfo"), hr);
				}
				pdpPlayerInfo = (DPN_PLAYER_INFO *) new BYTE[dwSize];
				ZeroMemory(pdpPlayerInfo, dwSize);
				pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
				hr = m_pDPlay->GetClientInfo(pCreatePlayer->dpnidPlayer, 
										   pdpPlayerInfo, &dwSize, 0);
				if (FAILED(hr))
					return DXTRACE_ERR_MSGBOX(TEXT("GetClientInfo"), hr);

				OnAddConnection( pCreatePlayer->dpnidPlayer );

				PlayerData *pPlayerData = GetPlayerDataForID(pCreatePlayer->dpnidPlayer);
				if (!pPlayerData)
				{
					delete pdpPlayerInfo;

					return (-1);			// player data 생성 실패
				}

				CHAR	szPlayerName[AGPACHARACTER_MAX_ID_STRING + 1];
				// This stores a extra TCHAR copy of the player name for 
				// easier access.  This will be redundent copy since DPlay 
				// also keeps a copy of the player name in GetClientInfo()
				DXUtil_ConvertWideStringToGeneric(szPlayerName, pdpPlayerInfo->pwszName, AGPACHARACTER_MAX_ID_STRING);

				delete pdpPlayerInfo;

				if (strncmp(szPlayerName, "Server", 6) == 0)
				{
					pPlayerData->bServer = TRUE;

					/*
					if (!AddPlayerToGroup(m_dpnidServers, pCreatePlayer->dpnidPlayer))
					{
						WriteLog(AS_LOG_DEBUG, "AsDP9 : AddPlayerToGroup() failed");
					}
					*/
				}
				else
				{
					pPlayerData->bServer = FALSE;

					/*
					if (!AddPlayerToGroup(m_dpnidClients, pCreatePlayer->dpnidPlayer))
					{
						WriteLog(AS_LOG_DEBUG, "AsDP9 : AddPlayerToGroup() failed");
					}
					*/
				}
			}
            break;
        }
        
        case DPN_MSGID_DESTROY_PLAYER:
        {
            PDPNMSG_DESTROY_PLAYER pDestroyPlayer = (PDPNMSG_DESTROY_PLAYER) pvMessage;

            if( pDestroyPlayer->pvPlayerContext == NULL )
                OnRemoveConnection( pDestroyPlayer->dpnidPlayer );
            break;
        }

        case DPN_MSGID_RECEIVE:
        {
            PDPNMSG_RECEIVE pRecvData = (PDPNMSG_RECEIVE) pvMessage;

            OnPacket( pRecvData->dpnidSender, pRecvData->pReceiveData, pRecvData->dwReceiveDataSize );
            break;
        }

        case DPN_MSGID_TERMINATE_SESSION:
        {
            //OnSessionLost( DISCONNNECT_REASON_UNKNOWN );
            break;
        }
    
		case DPN_MSGID_INDICATE_CONNECT:
		{
			PDPNMSG_INDICATE_CONNECT pIndicateConnect = (PDPNMSG_INDICATE_CONNECT) pvMessage;

			if (!pIndicateConnect->pvUserConnectData || !pIndicateConnect->dwUserConnectDataSize)
				return (-1);

			if (*((BOOL *) pIndicateConnect->pvUserConnectData) == TRUE)	// 서버가 접속한 경우이다.
				break;

			if (*m_pnServerStatus == GF_SERVER_FULL)
			{
				// 서버가 풀이다... 클라이언트에게 적당한 메시지를 보낸후 접속을 종료한다.
				//WriteLog(AS_LOG_RELEASE, "AsDP9 : server full");
				return (-1);
				break;
			}
			if (*m_pnServerStatus != GF_SERVER_START)
			{
				// 서버가 준비 상태가 아니다. 접속을 허용하지 않는다.
				//WriteLog(AS_LOG_RELEASE, "AsDP9 : server not ready (reject connection)");
				return (-1);
				break;
			}

			break;
		}

		case DPN_MSGID_CREATE_GROUP:
		{
			PDPNMSG_CREATE_GROUP	pCreateGroupMsg;
			pCreateGroupMsg = (PDPNMSG_CREATE_GROUP) pvMessage;

			// 새로 생성된 group의 dpnidGroup을 지정된 포인터에 넣는다.
			DWORD *pUserContext = (DWORD *) pCreateGroupMsg->pvGroupContext;

			*((DPNID *) pUserContext) = pCreateGroupMsg->dpnidGroup;

			break;
		}
	}
    
    return DPN_OK;
}




//-----------------------------------------------------------------------------
// Name: SendPacket
// Desc: Send the provided data to the given client
//-----------------------------------------------------------------------------
HRESULT AsDP9Server::SendPacket( DWORD dwTo, void* pData, 
                                   DWORD dwSize, BOOL bGuaranteed,
                                   DWORD dwTimeout )
{
	if (dwTo == ASDP_INVALID_DPNID)
		return E_INVALIDARG;

    DPNHANDLE       hAsync;
    DPNHANDLE*      phAsync;
    DWORD           dwFlags = 0;
    DPN_BUFFER_DESC dpnBufferDesc;

    if( bGuaranteed )
    {
        // If we are guaranteed then we must specify
        // DPNSEND_NOCOMPLETE and pass in non-null for the 
        // pvAsyncContext
//        dwFlags = DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK;
        dwFlags = DPNSEND_NOLOOPBACK;
    }
    else
    {
        // If we aren't guaranteed then we can
        // specify DPNSEND_NOCOMPLETE.  And when 
        // DPNSEND_NOCOMPLETE is on pvAsyncContext
        // must be NULL.
//        dwFlags = DPNSEND_NOCOMPLETE | DPNSEND_NOLOOPBACK;
        dwFlags = DPNSEND_NOCOMPLETE;
    }
    // Must define an async handle for the SendTo call. 
    phAsync = &hAsync;

    dpnBufferDesc.dwBufferSize = dwSize;
    dpnBufferDesc.pBufferData = (PBYTE) pData;

    // DirectPlay will tell via the message handler 
    // if there are any severe errors, so ignore any errors 
    m_pDPlay->SendTo( dwTo, &dpnBufferDesc, 1, dwTimeout, NULL, phAsync, dwFlags );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: GetConnectionInfo
// Desc: Get the current connection information for the given client
//-----------------------------------------------------------------------------
HRESULT AsDP9Server::GetConnectionInfo( DWORD dwID, TCHAR* strConnectionInfo, DWORD dwBufLength )
{
	if (dwID == ASDP_INVALID_DPNID)
		return E_INVALIDARG;

    // Validate parameters
    if( strConnectionInfo == NULL || dwBufLength == 0 )
        return E_INVALIDARG;

    HRESULT hr;

    // Call GetConnectionInfo and display results
    DPN_CONNECTION_INFO dpnConnectionInfo;
    ZeroMemory( &dpnConnectionInfo, sizeof(DPN_CONNECTION_INFO) );
    dpnConnectionInfo.dwSize = sizeof(DPN_CONNECTION_INFO);
    hr = m_pDPlay->GetConnectionInfo( dwID, &dpnConnectionInfo, 0 );

    if( SUCCEEDED(hr) )
    {
        DWORD dwHighPriMessages, dwHighPriBytes;
        DWORD dwNormalPriMessages, dwNormalPriBytes;
        DWORD dwLowPriMessages, dwLowPriBytes;

        hr = m_pDPlay->GetSendQueueInfo( dwID,
                                      &dwHighPriMessages, &dwHighPriBytes, 
                                      DPNGETSENDQUEUEINFO_PRIORITY_HIGH );

        hr = m_pDPlay->GetSendQueueInfo( dwID,
                                      &dwNormalPriMessages, &dwNormalPriBytes, 
                                      DPNGETSENDQUEUEINFO_PRIORITY_NORMAL );

        hr = m_pDPlay->GetSendQueueInfo( dwID,
                                      &dwLowPriMessages, &dwLowPriBytes, 
                                      DPNGETSENDQUEUEINFO_PRIORITY_LOW );

        int nCharsCopied = 0;
        nCharsCopied = _sntprintf( strConnectionInfo, dwBufLength-1,
                           TEXT("     Round Trip Latency MS=%dms\n")                      \
                           TEXT("     Throughput BPS: Current=%d Peak=%d\n")              \
                                                                                    \
                           TEXT("     Messages Received=%d\n")                            \
                                                                                    \
                           TEXT("     Sent: GB=%d GP=%d NGB=%d NGP=%d\n")                 \
                           TEXT("     Received: GB=%d GP=%d NGB=%d NGP=%d\n")             \
                                                                                    \
                           TEXT("     Messages Transmitted: HP=%d NP=%d LP=%d\n")         \
                           TEXT("     Messages Timed Out: HP=%d NP=%d LP=%d\n")           \
                                                                                    \
                           TEXT("     Retried: GB=%d GP=%d\n")                            \
                           TEXT("     Dropped: NGB=%d NGP=%d\n")                          \
                                                                                    \
                           TEXT("     Send Queue Messages: HP=%d NP=%d LP=%d\n")          \
                           TEXT("     Send Queue Bytes: HP=%d NP=%d LP=%d\n"),            \
                                                                                    \
                                                                                    \
                           dpnConnectionInfo.dwRoundTripLatencyMS, 
                           dpnConnectionInfo.dwThroughputBPS, 
                           dpnConnectionInfo.dwPeakThroughputBPS,

                           dpnConnectionInfo.dwMessagesReceived,

                           dpnConnectionInfo.dwBytesSentGuaranteed,
                           dpnConnectionInfo.dwPacketsSentGuaranteed,
                           dpnConnectionInfo.dwBytesSentNonGuaranteed,
                           dpnConnectionInfo.dwPacketsSentNonGuaranteed,

                           dpnConnectionInfo.dwBytesReceivedGuaranteed,
                           dpnConnectionInfo.dwPacketsReceivedGuaranteed,
                           dpnConnectionInfo.dwBytesReceivedNonGuaranteed,
                           dpnConnectionInfo.dwPacketsReceivedNonGuaranteed,

                           dpnConnectionInfo.dwMessagesTransmittedHighPriority,
                           dpnConnectionInfo.dwMessagesTransmittedNormalPriority,
                           dpnConnectionInfo.dwMessagesTransmittedLowPriority,

                           dpnConnectionInfo.dwMessagesTimedOutHighPriority,
                           dpnConnectionInfo.dwMessagesTimedOutNormalPriority,
                           dpnConnectionInfo.dwMessagesTimedOutLowPriority,

                           dpnConnectionInfo.dwBytesRetried,
                           dpnConnectionInfo.dwPacketsRetried,

                           dpnConnectionInfo.dwBytesDropped,
                           dpnConnectionInfo.dwPacketsDropped,

                           dwHighPriMessages, dwNormalPriMessages, dwLowPriMessages, 
                           dwHighPriBytes, dwNormalPriBytes, dwLowPriBytes

                           );

        // Null terminate
        strConnectionInfo[ dwBufLength-1 ] = 0;

        if( nCharsCopied < 0 )
            return DPNERR_BUFFERTOOSMALL;

    }
    else
    {
        _tcsncpy( strConnectionInfo, TEXT("DPNID not found.\n"), dwBufLength-1 );
        strConnectionInfo[ dwBufLength-1 ] = 0;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: OnAddConnection
// Desc: Handle a new client connection
//-----------------------------------------------------------------------------
void AsDP9Server::OnAddConnection( DWORD dpnid )
{
	if (dpnid == ASDP_INVALID_DPNID)
		return;

	m_csAddRemoveLock.Enter();

	// Increment our count of players
	m_dwPlayerCount++;

	CHAR	szBuffer[128];
	sprintf(szBuffer, "AsDP9 : Adding Player DPNID %0.8x", dpnid);
	//WriteLog(AS_LOG_DEBUG, szBuffer);
	sprintf(szBuffer, "Players connected = %d, Peak players connected = %d", m_dwPlayerCount, m_dwPeakPlayerCount);
	//WriteLog(AS_LOG_DEBUG, szBuffer);

    if( m_dwPlayerCount > m_dwPeakPlayerCount )
        m_dwPeakPlayerCount = m_dwPlayerCount;

    // Create a player for this client
    PlayerData* pPlayerData = CreatePlayerData();
    if( pPlayerData == NULL )
    {
		//WriteLog(AS_LOG_DEBUG, "AsDP9 : ERROR! Unable to create new PlayerData for client!");
        DXTRACE_ERR_MSGBOX( TEXT("CreatePlayerData"), E_FAIL );
		m_csAddRemoveLock.Leave();
        return;
    }

    // Store that pointer as local player data
    SetPlayerDataForID( dpnid, pPlayerData );

	m_csAddRemoveLock.Leave();

	return;
}




//-----------------------------------------------------------------------------
// Name: OnRemoveConnection
// Desc: Handle a client disconnection
//-----------------------------------------------------------------------------
void AsDP9Server::OnRemoveConnection( DWORD dpnid )
{
	if (dpnid == ASDP_INVALID_DPNID)
		return;

	m_csAddRemoveLock.Enter();

    // Decrement count of players
    m_dwPlayerCount--;

	CHAR	szBuffer[128];
	sprintf(szBuffer, "Removing player DPNID %0.8x", dpnid);
	//WriteLog(AS_LOG_DEBUG, szBuffer);
	sprintf(szBuffer, "Players connected = %d, Peak players connected = %d", m_dwPlayerCount, m_dwPeakPlayerCount);
	//WriteLog(AS_LOG_DEBUG, szBuffer);

    // Find playerdata for this client
    PlayerData* pPlayerData = GetPlayerDataForID( dpnid );
    if( pPlayerData != NULL )
    {
		// 여기서 삭제한다고 알려준다.
		//	걍 큐에 넣는다.
		if (pPlayerData->bServer)
		{
			pstCOMMAND	pstCommand = new stCOMMAND;

			pstCommand->stCheckArg.bReceivedFromServer	= TRUE;
			pstCommand->stCheckArg.lSocketOwnerID		= pPlayerData->lID;
			pstCommand->unType = ASDP_CK_DELETE_PLAYER;
			pstCommand->pvObject = NULL;
			pstCommand->dpnid = pPlayerData->NetID;
			pstCommand->unDataSize = sizeof(INT32);
			CopyMemory(pstCommand->szData, &pPlayerData->lID, sizeof(INT32));

			if (!m_pcsIOCPDispatcher->Dispatch(pstCommand))
			{
				// 큐에 넣는거 실패... 에러 리턴한다.
				//WriteLog(AS_LOG_DEBUG, "ASDP9 : m_pcsIOCPDispatcher->Dispatch 실패함(Server)");

				// hr = 에러코드
			}
		}
		else
		{
			pstCOMMAND	pstCommand = new stCOMMAND;
			pstCommand->stCheckArg.bReceivedFromServer	= TRUE;
			pstCommand->stCheckArg.lSocketOwnerID		= pPlayerData->lID;

			pstCommand->unType = ASDP_CK_DELETE_PLAYER;
			pstCommand->pvObject = NULL;
			pstCommand->dpnid = pPlayerData->NetID;
			pstCommand->unDataSize = sizeof(INT32) + sizeof(DPNID) + sizeof(CHAR) * 32;
			CopyMemory(pstCommand->szData, &pPlayerData->lID, sizeof(INT32));
			CopyMemory((CHAR *) pstCommand->szData + sizeof(INT32), &pPlayerData->NetID, sizeof(DPNID));
			CopyMemory((CHAR *) pstCommand->szData + sizeof(INT32) + sizeof(DPNID), &pPlayerData->szAccountName, sizeof(CHAR) * 32);

			if (!m_pcsIOCPDispatcher->Dispatch(pstCommand))
			{
				// 큐에 넣는거 실패... 에러 리턴한다.
				//WriteLog(AS_LOG_DEBUG, "ASDP9 : m_pcsIOCPDispatcher->Dispatch 실패함(Client)");

				// hr = 에러코드
			}
		}

        // Destroy it
        RemovePlayerDataID( pPlayerData );
        DestroyPlayerData( pPlayerData );
    }

	m_csAddRemoveLock.Leave();
}




//-----------------------------------------------------------------------------
// Name: OnPacket
// Desc: Receive handler
//-----------------------------------------------------------------------------
HRESULT AsDP9Server::OnPacket( DWORD dpnidFrom, void* pData, DWORD size )
{
    BOOL fFoundSize = FALSE;

	/*
	if (!CheckValidSource(pData, dpnidFrom))
	{
		WriteLog(AS_LOG_DEBUG, "AsDP9 : Invalid Srouce. drop packet");

		return (-1);
	}
	*/

	if (size > APPACKET_MAX_PACKET_SIZE)
	{
		//WriteLog(AS_LOG_DEBUG, "AsDP9 : Received Packet Size > APPACKET_MAX_PACKET_SIZE");

		return (-1);
	}

	FLOAT   fStartTime;

	{
		AuAutoLock _lock(m_csThreadCountLock);
		//Get the start time of when we entered the message handler.
		fStartTime = DXUtil_Timer( TIMER_GETAPPTIME );

		m_wActiveThreadCount++;
		if(m_wActiveThreadCount > m_wMaxThreadCount)
			m_wMaxThreadCount = m_wActiveThreadCount;
    
		// Calculate and average.
		FLOAT fdiff = m_wActiveThreadCount - m_fAvgThreadCount;
		m_fAvgThreadCount += fdiff/32;
	}
    
	// 여기서 받은 패킷을 큐에 넣는다.
	pstCOMMAND pstCommand = new stCOMMAND;

	pstCommand->stCheckArg.bReceivedFromServer	= FALSE;

    PlayerData* pPlayerData = GetPlayerDataForID( dpnidFrom );
	if (pPlayerData)
		pstCommand->stCheckArg.lSocketOwnerID		= pPlayerData->lID;
	else
		pstCommand->stCheckArg.lSocketOwnerID		= 0;

	pstCommand->unType = ASDP_CK_RECEIVE_DATA;
	pstCommand->pvObject = NULL;
	pstCommand->dpnid = dpnidFrom;
	pstCommand->unDataSize = (UINT16) size;
	CopyMemory(pstCommand->szData, pData, pstCommand->unDataSize);

	if (!m_pcsIOCPDispatcher->Dispatch(pstCommand))
	{
		// 큐에 넣는거 실패... 에러 리턴한다.
		//WriteLog(AS_LOG_DEBUG, "ASDP9 : 받은 데이타 큐에 넣는거 실패함");

		// hr = 에러코드
	}
	// 여기서 받은 패킷을 큐에 넣는다.

    // Retrieve thread data for this process.
	{
		AuAutoLock _Lock(m_csThreadCountLock);
		m_wActiveThreadCount--;

		FLOAT fDiffTime = (DXUtil_Timer( TIMER_GETAPPTIME ) - fStartTime) - m_fAvgThreadTime;
		m_fAvgThreadTime += fDiffTime/32;

		//Get the Max time in the thread.
		if ( fDiffTime > m_fMaxThreadTime )
		{
			m_fMaxThreadTime = fDiffTime;
		}
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CreatePlayerData
// Desc: Activate the player at the top of the free player list
//-----------------------------------------------------------------------------
PlayerData* AsDP9Server::CreatePlayerData()
{
    m_csPlayerDataListLock.Enter();

    // Grab first free player in the list
    PlayerData* pPlayerData = m_pFirstFreePlayerData;

    if( pPlayerData )
    {
        LockPlayerData( pPlayerData );

        // Got one, so remove it from the free list
        if( pPlayerData->pPrevious )
            pPlayerData->pPrevious->pNext = pPlayerData->pNext;
        if( pPlayerData->pNext )
            pPlayerData->pNext->pPrevious = pPlayerData->pPrevious;
        m_pFirstFreePlayerData = pPlayerData->pNext;

        // Add it to the active list
        if( m_pFirstActivePlayerData )
            m_pFirstActivePlayerData->pPrevious = pPlayerData;
        pPlayerData->pNext = m_pFirstActivePlayerData;
        pPlayerData->pPrevious = NULL;
        m_pFirstActivePlayerData = pPlayerData;

        // Update count of players
        m_dwActivePlayerDataCount++;

        // Generate the ID for this player
        m_dwPlayerDataUniqueValue++;
        pPlayerData->dwID = (DWORD) ((pPlayerData-m_PlayerDatas)|(m_dwPlayerDataUniqueValue<<PLAYER_OBJECT_SLOT_BITS));

        pPlayerData->pNextInIDHashBucket = NULL;
        pPlayerData->NetID = 0;

        // Mark as active
        pPlayerData->bActive = TRUE;

        UnlockPlayerData( pPlayerData );
    }

    m_csPlayerDataListLock.Leave();

    return pPlayerData;
}




//-----------------------------------------------------------------------------
// Name: DestroyPlayerData
// Desc: Deactivate the given player
//-----------------------------------------------------------------------------
void AsDP9Server::DestroyPlayerData( PlayerData* pPlayerData )
{
    m_csPlayerDataListLock.Enter();

    LockPlayerData( pPlayerData );

    // Mark as inactive
    pPlayerData->bActive = FALSE;

	pPlayerData->lID = AP_INVALID_CID;
	pPlayerData->bServer = FALSE;

    // Remove player from active list
    if( pPlayerData->pPrevious )
        pPlayerData->pPrevious->pNext = pPlayerData->pNext;
    if( pPlayerData->pNext )
        pPlayerData->pNext->pPrevious = pPlayerData->pPrevious;

    if( m_pFirstActivePlayerData == pPlayerData )
        m_pFirstActivePlayerData = pPlayerData->pNext;

    // Add it to the free list
    if( m_pFirstFreePlayerData )
        m_pFirstFreePlayerData->pPrevious = pPlayerData;
    pPlayerData->pNext = m_pFirstFreePlayerData;
    pPlayerData->pPrevious = NULL;
    m_pFirstFreePlayerData = pPlayerData;

    // Update count of players
    m_dwActivePlayerDataCount--;

    UnlockPlayerData( pPlayerData );

    m_csPlayerDataListLock.Leave();
}

//-----------------------------------------------------------------------------
// Name: IDHash
// Desc: Hash the current id for hashtable storage
//-----------------------------------------------------------------------------
DWORD   AsDP9Server::IDHash( DWORD id )
{
    DWORD   hash = ((id) + (id>>8) + (id>>16) + (id>>24)) & (NUM_ID_HASH_BUCKETS-1);
    return hash;
}




//-----------------------------------------------------------------------------
// Name: RemovePlayerDataID
// Desc: Remove the given player ID from the hashtable
//-----------------------------------------------------------------------------
void AsDP9Server::RemovePlayerDataID( PlayerData* pPlayerData )
{
    // Hash the ID to a bucket number
    DWORD   bucket = IDHash( pPlayerData->NetID );

    // Lock that hash bucket
    const   DWORD   buckets_per_lock = NUM_ID_HASH_BUCKETS / NUM_ID_HASH_BUCKET_LOCKS;
    m_csIDHashBucketLocks[bucket/buckets_per_lock].Enter();

    // Loop though players in bucket until we find the right one
    PlayerData* pPt = m_pstIDHashBucket[bucket];
    PlayerData* pPrev = NULL;
    while( pPt )
    {
        if( pPt == pPlayerData )
            break;
        pPrev = pPt;
        pPt = pPt->pNextInIDHashBucket;
    }

    if( pPt )
    {
        if( pPrev )
            pPrev->pNextInIDHashBucket = pPt->pNextInIDHashBucket;
        else
            m_pstIDHashBucket[bucket] = pPt->pNextInIDHashBucket;
        pPt->pNextInIDHashBucket = NULL;
    }

    // Unlock the hash bucket
    m_csIDHashBucketLocks[bucket/buckets_per_lock].Leave();
}




//-----------------------------------------------------------------------------
// Name: SetPlayerDataForID
// Desc: Store the data for the given player id in the hashtable
//-----------------------------------------------------------------------------
void AsDP9Server::SetPlayerDataForID( DWORD id, PlayerData* pPlayerData )
{
    // Make sure this player isn't added twice to the m_pstIDHashBucket[]
    // otherwise there will be a circular reference
    PlayerData* pSearch = GetPlayerDataForID( id );
    if( pSearch != NULL )
        return;

    // Hash the ID to a bucket number
    DWORD   bucket = IDHash( id );

    // Lock that hash bucket
    const   DWORD   buckets_per_lock = NUM_ID_HASH_BUCKETS / NUM_ID_HASH_BUCKET_LOCKS;
    m_csIDHashBucketLocks[bucket/buckets_per_lock].Enter();

    // Add player onto hash bucket chain
    pPlayerData->pNextInIDHashBucket = m_pstIDHashBucket[bucket];
    m_pstIDHashBucket[bucket] = pPlayerData;

    // Store net id in player
    pPlayerData->NetID = id;

    // Unlock the hash bucket
    m_csIDHashBucketLocks[bucket/buckets_per_lock].Leave();
}




//-----------------------------------------------------------------------------
// Name: GetPlayerDataForID
// Desc: Retrieve the assocated player data for the given ID
//-----------------------------------------------------------------------------
PlayerData* AsDP9Server::GetPlayerDataForID( DWORD id )
{
    // Hash the ID to a bucket number
    DWORD   bucket = IDHash( id );

    // Lock that hash bucket
    const   DWORD   buckets_per_lock = NUM_ID_HASH_BUCKETS / NUM_ID_HASH_BUCKET_LOCKS;
    m_csIDHashBucketLocks[bucket/buckets_per_lock].Enter();

    // Loop though players in bucket until we find the right one
    PlayerData* pPlayerData = m_pstIDHashBucket[bucket];
    while ( pPlayerData )
    {
        if( pPlayerData->NetID == id )
            break;
        pPlayerData = pPlayerData->pNextInIDHashBucket;
    }

    // Unlock the hash bucket
    m_csIDHashBucketLocks[bucket/buckets_per_lock].Leave();

    // Return the player we found (will be NULL if we couldn't find it)
    return pPlayerData;
}

BOOL AsDP9Server::CheckValidSource(PVOID pvData, DWORD dpnid)
{
	if (!pvData)
		return FALSE;

	PlayerData	*pPlayerData = GetPlayerDataForID(dpnid);
	if (!pPlayerData)
		return FALSE;

	if (pPlayerData->lID == AP_INVALID_CID)
	{
		return TRUE;
	}
	else
	{
		if (*(INT32 *)(((CHAR *) pvData) + sizeof(UINT8)) == pPlayerData->lID)	// 세팅된 아뒤랑 패킷 아뒤랑 비교한다.
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	return FALSE;
}