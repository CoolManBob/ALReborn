/******************************************************************************
Module:  AsDPModule.cpp
Notices: Copyright (c) 2002 netong
Purpose: 
Last Update: 2002. 04. 01
******************************************************************************/

#include <stdio.h>
#include <tchar.h>

#include "windows.h"
#include "AsDP8.h"
#include "AsCommonLib.h"
#include "AsDefine.h"

//#include "Winsock2.h"

//#include "stdio.h"

//-----------------------------------------------------------------------------
// Player context locking defines
//-----------------------------------------------------------------------------
CRITICAL_SECTION		g_csPlayerContext;
#define PLAYER_LOCK()                   EnterCriticalSection( &g_csPlayerContext ); 
#define PLAYER_ADDREF( pPlayerInfo )    if( pPlayerInfo ) pPlayerInfo->lRefCount++;
#define PLAYER_RELEASE( pPlayerInfo )   if( pPlayerInfo ) { pPlayerInfo->lRefCount--; if( pPlayerInfo->lRefCount <= 0 ) SAFE_DELETE( pPlayerInfo ); } pPlayerInfo = NULL;
#define PLAYER_UNLOCK()                 LeaveCriticalSection( &g_csPlayerContext );

IDirectPlay8Server*	g_pDPServer;				// DirectPlay server object
LONG				g_lNumberOfActivePlayers;	// Number of players currently in game

//AsQueueLIFO			g_queueWaitThread;
//AsCMDQueue			g_queueDP8;

AsDP8::AsDP8()
{
}

AsDP8::AsDP8(UINT32 nPort, GUID *pguidApp, INT16 *pnServerStatus, AsCMDQueue *pqueueRecv, AsCMDQueue *pqueueRecvSvr)
{
	m_pguidApp = pguidApp;
	m_pnServerStatus = pnServerStatus;
	m_pqueueRecv = pqueueRecv;
	m_pqueueRecvSvr = pqueueRecvSvr;

	m_bDPServerStarted = FALSE;

	m_dwPort = nPort;

	//g_queueWaitThread.Initialize(AGSMP_POOL_CLASSIFYPROC, sizeof(AsIOCP));

	//g_queueDP8.Initialize(AGSCP_CMD_QUEUE_SIZE);

	m_lSelfCID	= AP_INVALID_CID;
}

AsDP8::~AsDP8()
{
}

BOOL AsDP8::Initialize(UINT32 nPort, GUID *pguidApp, INT16 *pnServerStatus, AsCMDQueue *pqueueRecv, AsCMDQueue *pqueueRecvSvr)
{
	m_pguidApp = pguidApp;
	m_pnServerStatus = pnServerStatus;
	m_pqueueRecv = pqueueRecv;
	m_pqueueRecvSvr = pqueueRecvSvr;

	m_bDPServerStarted = FALSE;

	m_dwPort = nPort;

	return TRUE;
}

HRESULT AsDP8::StartServer()
{
	HRESULT				hr;
	PDIRECTPLAY8ADDRESS pDP8AddrLocal = NULL;
	PDIRECTPLAY8ADDRESSIP pDP8AddrLocalIP = NULL;

	InitializeCriticalSection(&g_csPlayerContext);

	// Create IDirectPlay8Server
	if (FAILED(hr = CoCreateInstance(CLSID_DirectPlay8Server, NULL,
									 CLSCTX_INPROC_SERVER,
									 IID_IDirectPlay8Server, 
									 (LPVOID*) &g_pDPServer)))
		return DXTRACE_ERR(TEXT("CoCreateInstance"), hr);

	// Init IDirectPlay8Server
	if (FAILED( hr = g_pDPServer->Initialize(this, DPMessageHandler, 0)))
		return DXTRACE_ERR(TEXT("Initialize"), hr);

	hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL,
						  CLSCTX_ALL, IID_IDirectPlay8Address,
						  (LPVOID*) &pDP8AddrLocal);
	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("CoCreateInstance"), hr);
		goto LCleanup;
	}

	DPN_CAPS	dpnCaps;
	ZeroMemory(&dpnCaps, sizeof(dpnCaps));
	dpnCaps.dwSize = sizeof(dpnCaps);
	dpnCaps.dwConnectTimeout = ASDP_CONNECT_TIMEOUT;
	dpnCaps.dwConnectRetries = ASDP_CONNECT_RETRIES;
	dpnCaps.dwTimeoutUntilKeepAlive = ASDP_TIMEOUT_KEEPALIVE;

	hr = g_pDPServer->SetCaps(&dpnCaps, 0);
	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("SetCaps"), hr);
		goto LCleanup;
	}

	hr = pDP8AddrLocal->SetSP(&CLSID_DP8SP_TCPIP);
	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("SetSP"), hr);
		goto LCleanup;
	}

    // Add the port to pDP8AddrLocal, if the port is non-zero.
    // If the port is 0, then DirectPlay will pick a port, 
    // Games will typically hard code the port so the 
    // user need not know it
	if (m_dwPort != 0)
	{
		if (FAILED(hr = pDP8AddrLocal->AddComponent(DPNA_KEY_PORT,
													&m_dwPort, sizeof(m_dwPort),
													DPNA_DATATYPE_DWORD)))
			return DXTRACE_ERR(TEXT("AddComponent"), hr);
	}

	/*
	hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL,
						  CLSCTX_ALL, IID_IDirectPlay8AddressIP,
						  (LPVOID*) &pDP8AddrLocalIP);
	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("CoCreateInstance"), hr);
		goto LCleanup;
	}

	//SOCKADDR_IN	stSockAddr;
	SOCKADDR	stSockAddr;
	DWORD		dwBufferSize;
	hr = pDP8AddrLocalIP->GetSockAddress((SOCKADDR *) &stSockAddr, &dwBufferSize);
	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("GetSockAddress"), hr);
		goto LCleanup;
	}

	//sprintf(m_szIPv4Addr, "%s:%s", inet_ntoa(stSockAddr.sin_addr), m_dwPort);
	*/

    WSADATA WSAData;
    _tcscpy( m_szIPv4Addr, TEXT("") );
    if( WSAStartup (MAKEWORD(1,0), &WSAData) == 0) 
    {
        CHAR strLocalHostName[MAX_PATH];
        gethostname( strLocalHostName, MAX_PATH );
        HOSTENT* pHostEnt = gethostbyname( strLocalHostName );
        if( pHostEnt )
        {
            in_addr* pInAddr = (in_addr*) pHostEnt->h_addr_list[0];
            char* strLocalIP = inet_ntoa( *pInAddr );
            if( strLocalIP )
                DXUtil_ConvertAnsiStringToGeneric( m_szIPv4Addr, strLocalIP );
        }

        WSACleanup();
    }

	DXUtil_ConvertGenericStringToWide(m_szSessionName, "ArchLordServer", -1);

	DPN_APPLICATION_DESC dpnAppDesc;
	ZeroMemory(&dpnAppDesc, sizeof(DPN_APPLICATION_DESC));
	dpnAppDesc.dwSize			= sizeof(DPN_APPLICATION_DESC);
	dpnAppDesc.dwFlags			= DPNSESSION_CLIENT_SERVER;
	dpnAppDesc.guidApplication	= *m_pguidApp;
	dpnAppDesc.pwszSessionName	= m_szSessionName;

	// Creates a new client/server session, hosted by the local computer.
	hr = g_pDPServer->Host(&dpnAppDesc, &pDP8AddrLocal, 1, NULL, NULL, NULL, 0);
	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("Host"), hr);
		goto LCleanup;
	}

	m_bDPServerStarted = TRUE;

	// client, server dpnid group 생성
	DPN_GROUP_INFO	dpnGroupInfo;
	ZeroMemory(&dpnGroupInfo, sizeof(DPN_GROUP_INFO));
	dpnGroupInfo.dwSize = sizeof(DPN_GROUP_INFO);
	dpnGroupInfo.dwInfoFlags = DPNINFO_DATA;
	dpnGroupInfo.pvData = NULL;
	dpnGroupInfo.dwDataSize = 0;
	DPNHANDLE		hAsync;

	hr = g_pDPServer->CreateGroup(&dpnGroupInfo, &m_dpnidClients, NULL, &hAsync, 0);

	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("CreateGroup"), hr);
		goto LCleanup;
	}

	hr = g_pDPServer->CreateGroup(&dpnGroupInfo, &m_dpnidServers, NULL, &hAsync, 0);

	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("CreateGroup"), hr);
		goto LCleanup;
	}
	// client, server dpnid group 생성

LCleanup:
	SAFE_RELEASE(pDP8AddrLocal);

	return hr;
}

VOID AsDP8::StopServer()
{
	if (g_pDPServer)
	{
		HRESULT			hr;
		DPNHANDLE		hAsync;

		hr = g_pDPServer->DestroyGroup(m_dpnidClients, NULL, &hAsync, 0);
		if (FAILED(hr))
		{
			DXTRACE_ERR(TEXT("AddPlayerToGroup"), hr);
		}

		hr = g_pDPServer->DestroyGroup(m_dpnidServers, NULL, &hAsync, 0);
		if (FAILED(hr))
		{
			DXTRACE_ERR(TEXT("AddPlayerToGroup"), hr);
		}

		g_pDPServer->Close(0);
		SAFE_RELEASE(g_pDPServer);
	}

	DeleteCriticalSection(&g_csPlayerContext);

	m_bDPServerStarted = FALSE;
}

HRESULT WINAPI AsDP8::DPMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
{
	AsDP8 *pThis = (AsDP8 *) pvUserContext;
	//AsIOCP	csIOCP;

	switch (dwMessageId)
	{
		case DPN_MSGID_CREATE_PLAYER:
		{
			PDPNMSG_CREATE_PLAYER pCreatePlayerMsg;
			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER) pMsgBuffer;

			WriteLog(AS_LOG_DEBUG, "AsDP8 : DPN_MSGID_CREATE_PLAYER");

			HRESULT hr;

			// Get the peer info and extract its name
			DWORD dwSize = 0;
			DPN_PLAYER_INFO* pdpPlayerInfo = NULL;
			hr = g_pDPServer->GetClientInfo(pCreatePlayerMsg->dpnidPlayer, 
											pdpPlayerInfo, &dwSize, 0 );
			if (FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL)
			{
				if (hr == DPNERR_INVALIDPLAYER)
				{
					// Ignore this message if this is for the host
					WriteLog(AS_LOG_DEBUG, "AsDP8 : invalid player");

					break;
				}

				return DXTRACE_ERR(TEXT("GetClientInfo"), hr);
			}
			pdpPlayerInfo = (DPN_PLAYER_INFO *) new BYTE[dwSize];
			ZeroMemory(pdpPlayerInfo, dwSize);
			pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
			hr = g_pDPServer->GetClientInfo(pCreatePlayerMsg->dpnidPlayer, 
									   pdpPlayerInfo, &dwSize, 0);
			if (FAILED(hr))
				return DXTRACE_ERR(TEXT("GetClientInfo"), hr);

			// Create a new and fill in a APP_PLAYER_INFO
			AS_APP_PLAYER_INFO* pPlayerInfo = new AS_APP_PLAYER_INFO;
			ZeroMemory(pPlayerInfo, sizeof(AS_APP_PLAYER_INFO));
			pPlayerInfo->lRefCount = 1;
			pPlayerInfo->dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;
			pPlayerInfo->lID = AP_INVALID_CID;
			//pPlayerInfo->bReady = FALSE;

			// This stores a extra TCHAR copy of the player name for 
			// easier access.  This will be redundent copy since DPlay 
			// also keeps a copy of the player name in GetClientInfo()
			DXUtil_ConvertWideStringToGeneric(pPlayerInfo->strPlayerName, 
											  pdpPlayerInfo->pwszName, AGPACHARACTER_MAX_ID_STRING);

			DPNHANDLE	hAsync;

			// 접속한 client가 서버인지 검사
			if (strncmp(pPlayerInfo->strPlayerName, "Server", 6) == 0)
			{
				pPlayerInfo->bServer = TRUE;

				/*
				// 접속 서버 정보를 푸쉬한다.
				stCOMMANDSVR	stCommandSvr;

				stCommandSvr.unType = ASDP_CK_CREATE_PLAYER;
				stCommandSvr.dpnid = pPlayerInfo->dpnidPlayer;
				stCommandSvr.pvServer = pPlayerInfo;

				pThis->m_pqueueRecvSvr->PushCommand(&stCommandSvr);
				*/

				// 서버 그룹에 추가한다.
				hr = g_pDPServer->AddPlayerToGroup(pThis->m_dpnidServers, pPlayerInfo->dpnidPlayer, NULL, &hAsync, 0);

				if (FAILED(hr))
				{
					DXTRACE_ERR(TEXT("AddPlayerToGroup"), hr);
				}
			}
			else
			{
				if (*(pThis->m_pnServerStatus) != GF_SERVER_START)
				{
					// 서버가 완벽한 상태가 아니다. 이런 상태로는 클라이언트의 접속을 받아들일 수 없다.
					WriteLog(AS_LOG_RELEASE, "AsDP8 : server is not ready. cannot accept client connection");

					SAFE_DELETE_ARRAY(pdpPlayerInfo);

					InterlockedIncrement(&g_lNumberOfActivePlayers);

					pThis->DestroyClient(pPlayerInfo->dpnidPlayer);

					break;
				}

				pPlayerInfo->bServer = FALSE;

				hr = g_pDPServer->AddPlayerToGroup(pThis->m_dpnidClients, pPlayerInfo->dpnidPlayer, (void *) 1, &hAsync, 0);

				if (FAILED(hr))
				{
					DXTRACE_ERR(TEXT("AddPlayerToGroup"), hr);
				}

				WriteLog(AS_LOG_DEBUG, "AsDP8 : create client session");
			}

			SAFE_DELETE_ARRAY(pdpPlayerInfo);

			// Tell DirectPlay to store this pPlayerInfo 
			// pointer in the pvPlayerContext.
			pCreatePlayerMsg->pvPlayerContext = pPlayerInfo;

			// 접속한 player의 정보를 connect_queue에 넣는다.

			//MessageBox(NULL, "Create new player", "DP8", 1);

			// 접속한 player의 정보를 connect_queue에 넣는다.

			// Update the number of active players, and 
			// post a message to the dialog thread to update the 
			// UI.  This keeps the DirectPlay message handler 
			// from blocking
			InterlockedIncrement(&g_lNumberOfActivePlayers);

#ifdef	_DEBUG
			DPN_CONNECTION_INFO	dpnConnectionInfo;
			dpnConnectionInfo.dwSize = sizeof(dpnConnectionInfo);

			hr = g_pDPServer->GetConnectionInfo(pPlayerInfo->dpnidPlayer, &dpnConnectionInfo, 0);
			if (FAILED(hr))
			{
				DXTRACE_ERR(TEXT("GetConnectionInfo"), hr);
			}
			else
			{
				WriteLog(AS_LOG_DEBUG, "AsDP8 : client connection information");

				CHAR	szBuffer[64];
				wsprintf(szBuffer, "	RoundTripLatency : %d(msec)", dpnConnectionInfo.dwRoundTripLatencyMS);
				WriteLog(AS_LOG_DEBUG, szBuffer);
			}
#endif	_DEBUG
			
			break;
		}

		case DPN_MSGID_ASYNC_OP_COMPLETE:
		{
			PDPNMSG_ASYNC_OP_COMPLETE pAsyncOpMsg = (PDPNMSG_ASYNC_OP_COMPLETE) pMsgBuffer;

#ifdef	_DEBUG
			if (pAsyncOpMsg->pvUserContext == (void *) 1)
			{
				fprintf(stdout, "AsDP8 : complete AddPlayerToGroup()\n");
				fflush(stdout);
			}
			else if (pAsyncOpMsg->pvUserContext == (void *) 2)
			{
				fprintf(stdout, "AsDP8 : complete RemovePlayerFromGroup()\n");
				fflush(stdout);
			}
#endif	_DEBUG

			break;
		}

		case DPN_MSGID_DESTROY_PLAYER:
		{
			WriteLog(AS_LOG_DEBUG, "AsDP8 : DPN_MSGID_DESTROY_PLAYER");

			PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg;
			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER) pMsgBuffer;
			AS_APP_PLAYER_INFO* pPlayerInfo = (AS_APP_PLAYER_INFO *) pDestroyPlayerMsg->pvPlayerContext;

			// Ignore this message if this is the host player
			if (pPlayerInfo == NULL)
				break; 

			//HRESULT	hr;
			//DPNHANDLE	hAsync;

			// player 데이타를 삭제한다.

			// 접속이 종료된다는 command를 큐에 넣는다.

			if (pPlayerInfo->bServer)
			{
				stCOMMANDSVR		stCommand;

				stCommand.unType = ASDP_CK_DELETE_PLAYER;
				stCommand.pvServer = pPlayerInfo;
				stCommand.dpnid = pPlayerInfo->dpnidPlayer;
				stCommand.unDataSize = sizeof(INT32);
				CopyMemory(stCommand.szData, &pPlayerInfo->lID, sizeof(INT32));

				pThis->m_pqueueRecvSvr->PushCommand(&stCommand);

				/*
				hr = g_pDPServer->RemovePlayerFromGroup(pThis->m_dpnidServers, pPlayerInfo->dpnidPlayer, NULL, &hAsync, 0);

				if (FAILED(hr))
				{
					DXTRACE_ERR(TEXT("RemovePlayerFromGroup"), hr);
				}
				*/
			}
			else
			{
				stCOMMAND			stCommand;

				stCommand.unType = ASDP_CK_DELETE_PLAYER;
				stCommand.pvCharacter = pPlayerInfo;
				stCommand.dpnid = pPlayerInfo->dpnidPlayer;
				stCommand.unDataSize = sizeof(INT32);
				CopyMemory(stCommand.szData, &pPlayerInfo->lID, sizeof(INT32));

				pThis->m_pqueueRecv->PushCommand(&stCommand);

				/*
				hr = g_pDPServer->RemovePlayerFromGroup(pThis->m_dpnidClients, pPlayerInfo->dpnidPlayer, (void *) 2, &hAsync, 0);

				if (FAILED(hr))
				{
					switch (hr)
					{
					case DPNSUCCESS_PENDING:
						break;
					case DPNERR_INVALIDFLAGS:
						break;
					case DPNERR_INVALIDGROUP:
						break;
					case DPNERR_INVALIDPLAYER:
						break;
					default:
						break;
					}

					DXTRACE_ERR(TEXT("RemovePlayerFromGroup"), hr);
				}
				*/

				WriteLog(AS_LOG_DEBUG, "AsDP8 : destroy client session");
			}

			// player 데이타를 삭제한다.

			PLAYER_LOCK();                  // enter player context CS
			PLAYER_RELEASE(pPlayerInfo);	// Release player and cleanup if needed
			PLAYER_UNLOCK();                // leave player context CS


			// Update the number of active players, and 
			// post a message to the dialog thread to update the 
			// UI.  This keeps the DirectPlay message handler 
			// from blocking

			InterlockedDecrement(&g_lNumberOfActivePlayers);

			break;
			}

		case DPN_MSGID_TERMINATE_SESSION:
			WriteLog(AS_LOG_DEBUG, "AsDP8 : DPN_MSGID_TERMINATE_SESSION");
			break;

		case DPN_MSGID_RECEIVE:
			{
			/*
#ifdef	_DEBUG
			fprintf(stdout, "AsDP8 : DPN_MSGID_RECEIVE\n");
			fflush(stdout);
#endif	_DEBUG
			*/

			PDPNMSG_RECEIVE pReceiveMsg;
			pReceiveMsg = (PDPNMSG_RECEIVE) pMsgBuffer;
			AS_APP_PLAYER_INFO* pPlayerInfo = (AS_APP_PLAYER_INFO *) pReceiveMsg->pvPlayerContext;

			//MessageBox(NULL, "Receive Msg", "DP8", 1);

			if (!pPlayerInfo)
			{
#ifdef	_DEBUG
				fprintf(stdout, "AsDP8 : player context is null\n");
				fflush(stdout);
#endif	_DEBUG
				pThis->DestroyClient(pReceiveMsg->dpnidSender);
				break;
			}

			if (!pThis->CheckValidSource(pReceiveMsg->pReceiveData, pPlayerInfo))
			{
				WriteLog(AS_LOG_DEBUG, "AsDP8 : 받은 패킷의 아뒤가 잘못되어 있다. 이넘은 도대체 뭐하는 넘일까.. 걍 무시해버린다.");

				break;
			}

			if (pPlayerInfo->bServer)
			{
				stCOMMANDSVR		stCommand;

				stCommand.unType = ASDP_CK_RECEIVE_DATA;
				stCommand.pvServer = pPlayerInfo;
				stCommand.dpnid = pPlayerInfo->dpnidPlayer;
				stCommand.unDataSize = (UINT16) pReceiveMsg->dwReceiveDataSize;
				CopyMemory(stCommand.szData, pReceiveMsg->pReceiveData, stCommand.unDataSize);

				// setting DPNID (패킷 젤 끝에 DPNID를 붙여서 큐에 넣는다. 나중에 필요한 모듈에서 뽑아서 쓴다)
				//CopyMemory(stCommand.szData + stCommand.unDataSize, &stCommand.dpnid, sizeof(DPNID));
				//stCommand.unDataSize += sizeof(DPNID);

				pThis->m_pqueueRecvSvr->PushCommand(&stCommand);

			/*
#ifdef	_DEBUG
				fprintf(stdout, "AsDP8 : receive message from server\n");
				fflush(stdout);
#endif	_DEBUG
			*/
			}
			else
			{
				if (*(pThis->m_pnServerStatus) != GF_SERVER_START)
				{
					break;
				}

				if ((UINT16) pReceiveMsg->dwReceiveDataSize > APPACKET_MAX_PACKET_SIZE)
				{
					WriteLog(AS_LOG_DEBUG, "AsDP8 : Received Packet Size > APPACKET_MAX_PACKET_SIZE");

					break;
				}



				stCOMMAND		stCommand;

				stCommand.unType = ASDP_CK_RECEIVE_DATA;
				stCommand.pvCharacter = pPlayerInfo;
				stCommand.dpnid = pPlayerInfo->dpnidPlayer;
				stCommand.unDataSize = (UINT16) pReceiveMsg->dwReceiveDataSize;
				CopyMemory(stCommand.szData, pReceiveMsg->pReceiveData, stCommand.unDataSize);

				// setting DPNID (패킷 젤 끝에 DPNID를 붙여서 큐에 넣는다. 나중에 필요한 모듈에서 뽑아서 쓴다)
				//CopyMemory(stCommand.szData + stCommand.unDataSize, &stCommand.dpnid, sizeof(DPNID));
				//stCommand.unDataSize += sizeof(DPNID);

				pThis->m_pqueueRecv->PushCommand(&stCommand);

/*
#ifdef	_DEBUG
				//MessageBox(NULL, "Receive message", "AsDP8", 1);
				fprintf(stdout, "AsDP8 : receive message from client\n");
				fflush(stdout);
#endif	_DEBUG
*/
			}

/*
#ifdef	_DEBUG
			DPN_CONNECTION_INFO	dpnConnectionInfo;
			dpnConnectionInfo.dwSize = sizeof(dpnConnectionInfo);

			HRESULT hr = g_pDPServer->GetConnectionInfo(pPlayerInfo->dpnidPlayer, &dpnConnectionInfo, 0);
			if (FAILED(hr))
			{
				DXTRACE_ERR(TEXT("GetConnectionInfo"), hr);
			}
			else
			{
				fprintf(stdout, "AsDP8 : client connection information\n");
				fprintf(stdout, "		RoundTripLatency : %d(msec)\n", dpnConnectionInfo.dwRoundTripLatencyMS);
				fprintf(stdout, "		Throughput : %d(bps)\n", dpnConnectionInfo.dwThroughputBPS);
				fflush(stdout);
			}
#endif	_DEBUG
*/

			break;
		}

		case DPN_MSGID_CREATE_GROUP:
		{
/*
#ifdef	_DEBUG
			fprintf(stdout, "AsDP8 : DPN_MSGID_CREATE_GROUP\n");
			fflush(stdout);
#endif	_DEBUG
*/

			PDPNMSG_CREATE_GROUP	pCreateGroupMsg;
			pCreateGroupMsg = (PDPNMSG_CREATE_GROUP) pMsgBuffer;

			// 새로 생성된 group의 dpnidGroup을 지정된 포인터에 넣는다.
			DWORD *pUserContext = (DWORD *) pCreateGroupMsg->pvGroupContext;

			*((DPNID *) pUserContext) = pCreateGroupMsg->dpnidGroup;

/*
#ifdef	_DEBUG
			fprintf(stdout, "AsDP8 : create group (group dpnid : %l)\n", pCreateGroupMsg->dpnidGroup);
			fflush(stdout);
#endif	_DEBUG
*/

			break;
		}

		case DPN_MSGID_INDICATE_CONNECT:
		{
			if (*(pThis->m_pnServerStatus) == GF_SERVER_FULL)
			{
				// 서버가 풀이다... 클라이언트에게 적당한 메시지를 보낸후 접속을 종료한다.
				WriteLog(AS_LOG_RELEASE, "AsDP8 : server full");
				return (-1);
				break;
			}
			if (*(pThis->m_pnServerStatus) == GF_SERVER_STOP)
			{
				// 서버가 다운된다. 새로 접속하는넘들 모두 무시한다. 종료하는 마당에 누굴 접속 시키랴...
				WriteLog(AS_LOG_RELEASE, "AsDP8 : server down (stop)");
				return (-1);
				break;
			}

			break;
		}

		case DPN_MSGID_INDICATED_CONNECT_ABORTED:
		{
			break;
		}

			/*
		default:
			{
			MessageBox(NULL, "unknown message id", "AsDP8", 0);
			break;
			}
			*/
	}

	return S_OK;
}

/*
HRESULT AsDP8::RejectClient(DWORD dwID, HRESULT bReason)
{
    BOOL hrRet = TRUE;

    if (g_pDPServer)
    {
        if( (hrRet = g_pDPServer->DestroyClient( dwID, &bReason, sizeof(bReason), 0 ) ) == S_OK)
			return TRUE;
    }

    return FALSE;
}
*/

INT16 AsDP8::SendTo(DPNID dpnid, PVOID pvData, INT16 nLength)
{
	HRESULT	hr;

	// server id setting
	*((INT32 *) (((CHAR *) pvData) + sizeof(INT8) /*type size*/)) = m_lSelfCID;

	DPN_BUFFER_DESC bufferDesc;
	bufferDesc.dwBufferSize = nLength;
	bufferDesc.pBufferData  = (BYTE*) pvData;

	DPNHANDLE hAsync;
	hr = g_pDPServer->SendTo(dpnid, &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_GUARANTEED | DPNSEND_NOLOOPBACK);

	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("SendTo"), hr);
		return (-1);
	}

	return nLength;
}

BOOL AsDP8::DestroyClient(DPNID dpnid)
{
	HRESULT hr = g_pDPServer->DestroyClient(dpnid, NULL, 0, 0);

	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("DestroyClient"), hr);
		return FALSE;
	}

	WriteLog(AS_LOG_DEBUG, "AsDP8 : DestroyClient()");

	return TRUE;
}

INT32 AsDP8::GetActivePlayers()
{
	return (g_lNumberOfActivePlayers);
}

INT16 AsDP8::GetConnectionInfo(DPNID dpnid, UINT32 *pulRoundTripLatencyMS, UINT32 *pulThroughputBPS)
{
	DPN_CONNECTION_INFO dpnConnectionInfo;
	dpnConnectionInfo.dwSize = sizeof(dpnConnectionInfo);

	HRESULT hr = g_pDPServer->GetConnectionInfo(dpnid, &dpnConnectionInfo, 0);
	if (FAILED(hr))
	{
#ifdef	_DEBUG
		DXTRACE_ERR(TEXT("DestroyClient"), hr);
#endif	_DEBUG

		return FALSE;
	}

	if (pulRoundTripLatencyMS)
		*pulRoundTripLatencyMS = dpnConnectionInfo.dwRoundTripLatencyMS;

	if (pulThroughputBPS)
		*pulThroughputBPS = dpnConnectionInfo.dwThroughputBPS;

	return TRUE;
}

IDirectPlay8Server* AsDP8::GetDP8Server()
{
	return g_pDPServer;
}

GUID* AsDP8::GetGUID()
{
	return m_pguidApp;
}

AsCMDQueue*	AsDP8::GetQueueRecv()
{
	return m_pqueueRecv;
}

AsCMDQueue* AsDP8::GetQueueRecvSvr()
{
	return m_pqueueRecv;
}

INT16* AsDP8::GetServerStatus()
{
	return m_pnServerStatus;
}

//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertAnsiStringToGeneric()
// Desc: This is a UNICODE conversion utility to convert a WCHAR string into a
//       TCHAR string. cchDestChar defaults -1 which means it 
//       assumes strDest is large enough to store strSource
//-----------------------------------------------------------------------------
VOID DXUtil_ConvertWideStringToGeneric(TCHAR* tstrDestination, const WCHAR* wstrSource, int cchDestChar)
{
    if (tstrDestination==NULL || wstrSource==NULL)
        return;

#ifdef _UNICODE
    if (cchDestChar == -1)
	    wcscpy(tstrDestination, wstrSource);
    else
	    wcsncpy(tstrDestination, wstrSource, cchDestChar);
#else
    DXUtil_ConvertWideStringToAnsi(tstrDestination, wstrSource, cchDestChar);
#endif
}

//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertWideStringToAnsi()
// Desc: This is a UNICODE conversion utility to convert a WCHAR string into a
//       CHAR string. cchDestChar defaults -1 which means it 
//       assumes strDest is large enough to store strSource
//-----------------------------------------------------------------------------
VOID DXUtil_ConvertWideStringToAnsi(CHAR* strDestination, const WCHAR* wstrSource, int cchDestChar)
{
    if (strDestination==NULL || wstrSource==NULL)
        return;

    if (cchDestChar == -1)
        cchDestChar = wcslen(wstrSource) + 1;

    WideCharToMultiByte(CP_ACP, 0, wstrSource, -1, strDestination, cchDestChar - 1, NULL, NULL);

    strDestination[cchDestChar - 1] = 0;
}

//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertGenericStringToWide()
// Desc: This is a UNICODE conversion utility to convert a TCHAR string into a
//       WCHAR string. cchDestChar defaults -1 which means it 
//       assumes strDest is large enough to store strSource
//-----------------------------------------------------------------------------
VOID DXUtil_ConvertGenericStringToWide( WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar )
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

//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertAnsiStringToWide()
// Desc: This is a UNICODE conversion utility to convert a CHAR string into a
//       WCHAR string. cchDestChar defaults -1 which means it 
//       assumes strDest is large enough to store strSource
//-----------------------------------------------------------------------------
VOID DXUtil_ConvertAnsiStringToWide( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar )
{
    if( wstrDestination==NULL || strSource==NULL )
        return;

    if( cchDestChar == -1 )
        cchDestChar = strlen(strSource)+1;

    MultiByteToWideChar( CP_ACP, 0, strSource, -1, 
                         wstrDestination, cchDestChar-1 );

    wstrDestination[cchDestChar-1] = 0;
}

//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertAnsiStringToGeneric()
// Desc: This is a UNICODE conversion utility to convert a CHAR string into a
//       TCHAR string. cchDestChar defaults -1 which means it 
//       assumes strDest is large enough to store strSource
//-----------------------------------------------------------------------------
VOID DXUtil_ConvertAnsiStringToGeneric( TCHAR* tstrDestination, const CHAR* strSource, 
                                        int cchDestChar )
{
    if( tstrDestination==NULL || strSource==NULL )
        return;
        
#ifdef _UNICODE
    DXUtil_ConvertAnsiStringToWide( tstrDestination, strSource, cchDestChar );
#else
    if( cchDestChar == -1 )
    	strcpy( tstrDestination, strSource );
    else
    	strncpy( tstrDestination, strSource, cchDestChar );
#endif
}

BOOL AsDP8::GetLocalIPAddr(CHAR* pszBuffer, INT16 *pnLength)
{
	*pnLength = strlen(m_szIPv4Addr);

	strncpy(pszBuffer, m_szIPv4Addr, *pnLength);

	return TRUE;
}

DPNID AsDP8::GetDPNIDClients()
{
	return m_dpnidClients;
}

DPNID AsDP8::GetDPNIDServers()
{
	return m_dpnidServers;
}

BOOL AsDP8::SetIDToPlayerContext(INT32 lID, DPNID dpnid)
{
	AS_APP_PLAYER_INFO	*pPlayerContext;

	HRESULT	hr = g_pDPServer->GetPlayerContext(dpnid, (void **) &pPlayerContext, 0);
	if (FAILED(hr))
		return FALSE;

	pPlayerContext->lID = lID;

	return TRUE;
}

/*
BOOL AsDP8::SetAccountIDToPlayerContext(INT32 lAccountID, DPNID dpnid)
{
	AS_APP_PLAYER_INFO	*pPlayerContext;

	HRESULT	hr = g_pDPServer->GetPlayerContext(dpnid, (void **) &pPlayerContext, 0);
	if (FAILED(hr))
		return FALSE;

	pPlayerContext->lAccountID = lAccountID;

	return TRUE;
}
*/

DPNID AsDP8::CreateGroup()
{
	// dpnid group 생성
	DPN_GROUP_INFO	dpnGroupInfo;
	ZeroMemory(&dpnGroupInfo, sizeof(DPN_GROUP_INFO));
	dpnGroupInfo.dwSize = sizeof(DPN_GROUP_INFO);
	dpnGroupInfo.dwInfoFlags = DPNINFO_DATA;
	dpnGroupInfo.pvData = NULL;
	dpnGroupInfo.dwDataSize = 0;
	dpnGroupInfo.dwGroupFlags = DPNGROUP_AUTODESTRUCT;
	DPNHANDLE		hAsync;

	DPNID	dpnidGroup;

	HRESULT hr = g_pDPServer->CreateGroup(&dpnGroupInfo, &dpnidGroup, NULL, &hAsync, 0);

	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("CreateGroup"), hr);
		return 0;
	}

	return dpnidGroup;
}

BOOL AsDP8::DestroyGroup(DPNID dpnidGroup)
{
	DPNHANDLE	hAsync;

	HRESULT hr = g_pDPServer->DestroyGroup(dpnidGroup, NULL, &hAsync, 0);
	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("AddPlayerToGroup"), hr);
		return FALSE;
	}

	return TRUE;
}

BOOL AsDP8::AddPlayerToGroup(DPNID dpnidGroup, DPNID dpnidPlayer)
{
	DPNHANDLE	hAsync;

	HRESULT hr = g_pDPServer->AddPlayerToGroup(dpnidGroup, dpnidPlayer, (void *) 1, &hAsync, 0);

	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("AddPlayerToGroup"), hr);
		return FALSE;
	}

	return TRUE;
}

BOOL AsDP8::RemovePlayerFromGroup(DPNID dpnidGroup, DPNID dpnidPlayer)
{
	DPNHANDLE	hAsync;

	HRESULT hr = g_pDPServer->RemovePlayerFromGroup(dpnidGroup, dpnidPlayer, (void *) 2, &hAsync, 0);

	if (FAILED(hr))
	{
		DXTRACE_ERR(TEXT("RemovePlayerFromGroup"), hr);
		return FALSE;
	}

	return TRUE;
}

BOOL AsDP8::SetSelfCID(INT32 lServerID)
{
	m_lSelfCID = lServerID;

	return TRUE;
}

BOOL AsDP8::CheckValidSource(PVOID pvData, AS_APP_PLAYER_INFO *pPlayerInfo)
{
	if (!pvData || !pPlayerInfo)
		return FALSE;

	if (pPlayerInfo->bServer)
	{
		if (pPlayerInfo->lID == AP_INVALID_SERVERID)		// 아직 아뒤가 세팅 안되었다. 좀 위험하지만 여기선 어쩔 수 없이 걍 넘긴다.
		{
			return TRUE;
		}
		else
		{
			if (*(INT32 *)(((CHAR *) pvData) + sizeof(UINT8)) == pPlayerInfo->lID)	// 세팅된 아뒤랑 패킷 아뒤랑 비교한다.
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	else
	{
		if (pPlayerInfo->lID == AP_INVALID_CID)
		{
			return TRUE;
		}
		else
		{
			if (*(INT32 *)(((CHAR *) pvData) + sizeof(UINT8)) == pPlayerInfo->lID)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	
	return FALSE;
}