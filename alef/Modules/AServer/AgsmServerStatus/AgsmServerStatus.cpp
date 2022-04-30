// AgsmServerStatus.h
// (C) NHN - ArchLord Development Team
// steeple, 2005. 03. 31.

#include "AgsmServerStatus.h"
#include "AuStrTable.h"
#include <psapi.h>

#pragma comment(lib, "psapi")


#define ASSM_SEND_CURRENT_USER_COUNT_INTERVAL			2000
#define IP_ADDR_LENGTH									15

AgsmServerStatus::AgsmServerStatus()
{
	// 모듈 이름만 설정하고, 패킷 타입은 설정하지 않아도 된다.
	SetModuleName("AgsmServerStatus");
	SetModuleType(APMODULE_TYPE_SERVER);
	EnableIdle(TRUE);

	m_csIOCPServer.SetDispatchGameModule(this, DispatchThread, DisconnectCB);

	m_pagsmServerManager = NULL;
	m_pagsmAdmin = NULL;
	m_pagsmConfig = NULL;

	m_pAgsmInterServerLink = NULL;
	m_pAgsmDatabasePool = NULL;

	m_pAgpmSiegeWar = NULL;
	
	m_lServerType = ASSM_SERVER_TYPE_GAME_SERVER;
	m_nPort = (INT16)ASSM_PORT_GAME_SERVER;

	m_lConnectedSocketIndex = -1;

	memset(m_szProcessFileLastModifiedTime, 0, sizeof(m_szProcessFileLastModifiedTime));

	m_pServerMainClass = NULL;
	m_pfServerShutdown = NULL;

	m_lLiveTick = 0;

	m_lPatchVersion = 0;
}

AgsmServerStatus::~AgsmServerStatus()
{
}

BOOL AgsmServerStatus::OnAddModule()
{
	m_pagsmServerManager = (AgsmServerManager*)GetModule("AgsmServerManager2");
	m_pagsmAdmin = (AgsmAdmin*)GetModule("AgsmAdmin");
	m_pagpmConfig = (AgpmConfig*)GetModule("AgpmConfig");
	m_pagsmConfig = (AgsmConfig*)GetModule("AgsmConfig");
	m_pagsmAccountManager = (AgsmAccountManager*)GetModule("AgsmAccountManager");
	m_pAgsmInterServerLink = (AgsmInterServerLink*)GetModule("AgsmInterServerLink");
	m_pAgsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pAgsmDatabasePool = (AgsmDatabasePool*)GetModule("AgsmDatabasePool");
	m_pAgpmSiegeWar = (AgpmSiegeWar*)GetModule("AgpmSiegeWar");

	/*
	if (!m_pagsmServerManager || !m_pagsmAdmin || !m_pagpmConfig || !m_pagsmConfig)
	{
		return FALSE;
	}
	*/

	// Module 을 다 가져왔으면 서버의 실행시간을 구한다.
	CHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);

	HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if(hFile)
	{
		FILETIME Cr, Ac, Wr;
		FILETIME lCr, lAc, lWr;
		SYSTEMTIME sCr, sAc, sWr;

		GetFileTime(hFile, &Cr, &Ac, &Wr);
		FileTimeToLocalFileTime(&Cr, &lCr);
		FileTimeToLocalFileTime(&Ac, &lAc);
		FileTimeToLocalFileTime(&Wr, &lWr);

		FileTimeToSystemTime(&lCr, &sCr);
		FileTimeToSystemTime(&lAc, &sAc);
		FileTimeToSystemTime(&lWr, &sWr);

		sprintf(m_szProcessFileLastModifiedTime, "%4d-%02d-%02d %02d:%02d:%02d",
			sWr.wYear, sWr.wMonth, sWr.wDay, sWr.wHour, sWr.wMinute, sWr.wSecond);
	}
    
	return TRUE;
}

BOOL AgsmServerStatus::OnInit()
{
	//m_WISMObj.Open();//JK_WISM연동

	return TRUE;
}

BOOL AgsmServerStatus::OnIdle(UINT32 ulClockCount)
{
	if (m_eModuleStatus != APMODULE_STATUS_READY)
		return FALSE;

	if(ulClockCount - m_lLastSendClockCount < ASSM_SEND_CURRENT_USER_COUNT_INTERVAL)
		return TRUE;

	SendServerInfoPacket(m_lConnectedSocketIndex);

	m_lLastSendClockCount = ulClockCount;

/////////////////////////////////////////////////////////////////
	if(m_lServerType != ASSM_SERVER_TYPE_GAME_SERVER) return TRUE;

	if(ulClockCount - m_lLastSendServerMonitorInfo < 30 * 1000) 
		return TRUE;	

	
	INT32 nPremiumPCRoom = m_pAgsmCharacter->GetNumOfPremiumPCRoomPlayers();
	INT32 nNormalPCRoom = m_pAgsmCharacter->GetNumOfPCRoomPlayers() - nPremiumPCRoom;


	//WISM::CNotificationNumber aNumber;
	//aNumber.mSystemId = GetServerSystemID();
	//aNumber.mActivityId = 1005;//동접
	//aNumber.mNumber[WISM::eNotificationNumberValue] = m_pAgsmCharacter->GetNumOfPlayers();
	//
	//m_WISMObj.NotifyNumber(aNumber);
	//
	//aNumber.mSystemId = GetServerSystemID();
	//aNumber.mActivityId = 1007;//유료pc방
	//aNumber.mNumber[WISM::eNotificationNumberValue] = nPremiumPCRoom;
	//
	//m_WISMObj.NotifyNumber(aNumber);

	//aNumber.mSystemId = GetServerSystemID();
	//aNumber.mActivityId = 1008;//무료pc방
	//aNumber.mNumber[WISM::eNotificationNumberValue] = nNormalPCRoom;

	//m_WISMObj.NotifyNumber(aNumber);


	m_lLastSendServerMonitorInfo = ulClockCount;
/////////////////////////////////////////////////////////////////////////////////
	return TRUE;
}

BOOL AgsmServerStatus::OnDestroy()
{
	Stop();

	//m_WISMObj.Close();

	return TRUE;
}
//JK_WISM연동
INT32 AgsmServerStatus::GetServerSystemID()
{
	AgsdServer2* pThisServer = NULL;
	INT32 nSystemID;

	pThisServer = m_pagsmServerManager->GetThisServer();

	if(!pThisServer) return 0;
	
	switch(pThisServer->m_lServerID)
	{
	case 15:		//9번 14번이 동합되어 15번이 되었다..
		nSystemID = 601;
		break;
	case 11:
		nSystemID = 602;
		break;
	case 99:		//JK_특성화서버
		nSystemID = 603;
		break;
//	case 14:
//		nSystemID = 603;
//		break;
//  12번 13번이 통합되어 14번이 되었다..
//	case 12:
//		nSystemID = 603;
//		break;
//	case 13:
//		nSystemID = 604;
//		break;
	default:
		nSystemID = 0; //테스트 서버
		break;
	}
	return nSystemID;

}

BOOL AgsmServerStatus::SetServerTypeLoginServer()
{
	return SetServerType(ASSM_SERVER_TYPE_LOGIN_SERVER, ASSM_PORT_LOGIN_SERVER);
}

BOOL AgsmServerStatus::SetServerTypeGameServer()
{
	return SetServerType(ASSM_SERVER_TYPE_GAME_SERVER, ASSM_PORT_GAME_SERVER);
}

BOOL AgsmServerStatus::SetServerTypeRelayServer()
{
	return SetServerType(ASSM_SERVER_TYPE_RELAY_SERVER, ASSM_PORT_RELAY_SERVER);
}

BOOL AgsmServerStatus::SetServerTypePatchServer()
{
	return SetServerType(ASSM_SERVER_TYPE_PATCH_SERVER, ASSM_PORT_PATCH_SERVER);
}

BOOL AgsmServerStatus::SetServerType(INT32 lServerType, INT16 nPort)
{
	m_lServerType = lServerType;
	m_nPort = nPort;
	return TRUE;
}

BOOL AgsmServerStatus::Start()
{
	return m_csIOCPServer.Initialize(ASSM_MAX_THREAD, m_nPort, ASSM_MAX_CONNECTION, 10);
}

BOOL AgsmServerStatus::Stop()
{
	m_csIOCPServer.DestroyAllClient();
	return TRUE;
}

BOOL AgsmServerStatus::DispatchThread(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	AgsmServerStatus* pThis = (AgsmServerStatus*)pvParam;
	AsServerSocket* pSocket = (AsServerSocket*)pvSocket;

	if(!pThis || !pSocket)
		return FALSE;

	pThis->ParsePacket(pvPacket, ((PPACKET_HEADER) pvPacket)->unPacketLength, pSocket->GetIndex());

	return TRUE;
}

BOOL AgsmServerStatus::DisconnectCB(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	AgsmServerStatus* pThis = (AgsmServerStatus*)pvParam;
	AsServerSocket* pSocket = (AsServerSocket*)pvSocket;

	if(!pThis || !pSocket)
		return FALSE;

	if(pSocket->GetIndex() == pThis->m_lConnectedSocketIndex)
		pThis->m_lConnectedSocketIndex = -1;

	return TRUE;
}

BOOL AgsmServerStatus::ParsePacket(PVOID pvPacket, INT16 nSize, INT32 lIndex)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	PPACKET_HEADER pstPacket = (PPACKET_HEADER)pvPacket;
	switch(pstPacket->cType)
	{
		case ASSM_PACKET_SERVER_INFO:
			ParseServerInfoPacket(pvPacket, nSize, lIndex);
			break;

		case ASSM_PACKET_SERVER_CONTROL:
			ParseServerControlPacket(pvPacket, nSize, lIndex);
			break;
	}

	return TRUE;
}

BOOL AgsmServerStatus::ParseServerInfoPacket(PVOID pvPacket, INT16 nSize, INT32 lIndex)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	BOOL bResult = SendServerInfoPacket(lIndex);
	m_lConnectedSocketIndex = lIndex;

	return bResult;
}

BOOL AgsmServerStatus::ParseServerControlPacket(PVOID pvPacket, INT16 nSize, INT32 lIndex)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT32 lServerControlType = -1;
	INT32 lClientType = -1;
	INT32 lServerNumber = -1;
	CHAR* pszServerName = NULL;
	INT32 lServerType = -1;
	INT32 lParam1 = -1;
	INT32 lParam2 = -1;
	INT32 lParam3 = -1;
	CHAR* pszAccountName = NULL;
	CHAR* pszPassword = NULL;
	INT32 lServerManagerVersion = -1;
	CHAR* pszConsoleIP = NULL;
	CHAR* pszParamString = NULL;

	m_csPacket.m_csApsmServerControlPacket.GetField(TRUE, pvPacket, nSize,
				/*  1 */			&lServerControlType,
				/*  2 */			&lClientType,
				/*  3 */			&lServerNumber,
				/*  4 */			&pszServerName,
				/*  5 */			&lServerType,
				/*  6 */			&lParam1,
				/*  7 */			&lParam2,
				/*  8 */			&lParam3,
				/*  9 */			&pszAccountName,
				/* 10 */			&pszPassword,
				/* 11 */			&lServerManagerVersion,
				/* 12 */			&pszConsoleIP,
				/* 13 */			&pszParamString
							);

	switch(lServerControlType)
	{
		case ASSM_SERVER_CONTROL_TYPE_STOP:		// 현재 쓰이는 건 이놈하나.
			{
				ServerControlStop(lIndex);
			}
			break;
		case ASSM_SERVER_CONTROL_TYPE_RELOAD_SERVER_CONFIG:
			{
				/*if (m_pagsmConfig)
				{
					m_pagsmConfig->LoadConfig();

					char szBuf[ASSM_MAX_GENERAL_PARAM_STRING];
					sprintf(szBuf, "Exp: %3.2f, Drop: %3.2f, Adult: %d, Admin: %d",
								m_pagpmConfig->GetExpAdjustmentRatio(),
								m_pagpmConfig->GetDropAdjustmentRatio(),
								m_pagpmConfig->IsAdultServer(),
								m_pagpmConfig->IsAllAdmin());

					SendGeneralParamStringPacket(lIndex, pszConsoleIP, szBuf);
				}*/
			}
			break;
		case ASSM_SERVER_CONTROL_TYPE_CRASH:
			{
				// Crash를 내자, Crash를 내자. 0으로 나누자.. ㅋㅋ by Parn
				CHAR *	pAddr = NULL;

				*pAddr = 'a';
			}
			break;
		case ASSM_SERVER_CONTROL_TYPE_RECONNECT_LOGIN:
			{
				if (m_lServerType == ASSM_SERVER_TYPE_GAME_SERVER && m_pAgsmInterServerLink)
				{
					// 2008.03.12. steeple
					// 그냥 로그인 서버를 끊으라는 세팅만 해 놓는다.
					m_pAgsmInterServerLink->SetDisconnectLoginServer(TRUE);
				}
			}
			break;
		case ASSM_SERVER_CONTROL_TYPE_DISPLAY_NOTICE:
			{
				if (m_pagsmAdmin)
				{
					m_pagsmAdmin->EnumNotice(CBEnumNotice, this, &lIndex);
				}
			}
			break;
		case ASSM_SERVER_CONTROL_TYPE_ADMIN_COMMAND:
			{
				if (m_pagsmAdmin)
				{
					AgpdChatData	stChatData;

					ZeroMemory(&stChatData, sizeof(stChatData));

					stChatData.eChatType = AGPDCHATTING_TYPE_NORMAL;
					stChatData.szMessage = new CHAR [strlen(pszParamString) + 1];
					strcpy(stChatData.szMessage, pszParamString);
					stChatData.lMessageLength = ( UINT32 ) strlen(stChatData.szMessage);

					m_pagsmAdmin->ParseCommand(&stChatData, FALSE);

					delete [] stChatData.szMessage;
				}
			}
			break;
		case ASSM_SERVER_CONTROL_TYPE_NOTICE_LIST:
			{
				if (m_pagsmAdmin)
				{
					AssmNoticeVector *	pvtNotice = m_pagsmAdmin->LockNotice();
					AssmNoticeIter		iter;
					CHAR				szTemp[ASSM_MAX_GENERAL_PARAM_STRING];

					sprintf(szTemp, "=== Notice List (%s) ===", pszServerName ? pszServerName : "");
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);

					for (iter = pvtNotice->begin(); iter != pvtNotice->end(); ++iter)
					{
						sprintf(szTemp, "%d : %s", (*iter).m_lID, (*iter).m_szMessage);
						SendGeneralParamStringPacket(lIndex, NULL, szTemp);

					}

					sprintf(szTemp, "===================");
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);

					m_pagsmAdmin->UnlockNotice();
				}
			}
			// 여기다가 break; 를 넣지 않음으로써, 아래 애들도 다 보여준다.
		case ASSM_SERVER_CONTROL_TYPE_SIEGE_INFO:
			{
				if (m_pAgpmSiegeWar)
				{
					INT32			i;
					AgpdSiegeWar *	pcsSiegeWar;
					CHAR			szTemp[ASSM_MAX_GENERAL_PARAM_STRING];

					sprintf(szTemp, "=== Castle Info (%s) ===", pszServerName ? pszServerName : "");
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);

					for (i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
					{
						pcsSiegeWar = m_pAgpmSiegeWar->GetSiegeWarInfo(i);
						if (!pcsSiegeWar)
							continue;

						sprintf(szTemp, "%s : %s (%s)", pcsSiegeWar->m_strCastleName, pcsSiegeWar->m_strOwnerGuildMasterName, pcsSiegeWar->m_strOwnerGuildName);
						SendGeneralParamStringPacket(lIndex, NULL, szTemp);
					}

					sprintf(szTemp, "===================");
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);
				}
			}
			// 여기다가 break; 를 넣지 않음으로써, 아래 애들도 다 보여준다.
		case ASSM_SERVER_CONTROL_TYPE_GAME_INFO:
			{
				if(m_pagpmConfig)
				{
					CHAR szTemp[ASSM_MAX_GENERAL_PARAM_STRING];

					sprintf(szTemp, "=== Game Info (%s) ===", pszServerName ? pszServerName : "");
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);

					// Exp
					sprintf(szTemp, "%s Ratio : %3.2f", ServerStr().GetStr(STI_EXP), m_pagpmConfig->GetExpAdjustmentRatio());
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);

					// Drop
					sprintf(szTemp, "Drop Ratio : %3.2f", m_pagpmConfig->GetDropAdjustmentRatio());
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);

					// Ghelld
					sprintf(szTemp, "%s Ratio : %3.2f", ServerStr().GetStr(STI_GELD), m_pagpmConfig->GetGheldDropAdjustmentRatio());
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);

					// Charisma
					sprintf(szTemp, "Charisma Ratio : %3.2f", m_pagpmConfig->GetCharismaDropAdjustmentRatio());
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);

					// Chatting Event
					sprintf(szTemp, "Chat Event : %d", m_pagpmConfig->IsEventChatting());
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);

					sprintf(szTemp, "===================");
					SendGeneralParamStringPacket(lIndex, NULL, szTemp);
				}
			}
			break;
		default:
			break;
	}

	return TRUE;
	//return SendServerControlPacket(lIndex);
}

// 서버를 내리라는 패킷이 왔다
// 패킷이 로컬IP 에서 온 것인지 확인해보고 (Agent 에서 온 것인지)
// IP 가 맞다면 정상 종료 처리를 해준다.
BOOL AgsmServerStatus::ServerControlStop(INT32 lSocketIndex)
{
	if(lSocketIndex < 0)
		return FALSE;

	CHAR szRemoteIPAddr[IP_ADDR_LENGTH+1];
	memset(szRemoteIPAddr, 0, sizeof(szRemoteIPAddr));

	if(!m_csIOCPServer.m_csSocketManager[lSocketIndex])
		return FALSE;

	strncpy(szRemoteIPAddr, inet_ntoa(m_csIOCPServer.m_csSocketManager[lSocketIndex]->GetRemoteInfo()->sin_addr), IP_ADDR_LENGTH);

	// 로컬 IP 를 구한다.
	CHAR szLocalIPAddr[IP_ADDR_LENGTH+1];
	CHAR szHostName[255];
	PHOSTENT pHostInfo;

	if(gethostname(szHostName, sizeof(szHostName)) == 0)
	{
		if((pHostInfo = gethostbyname(szHostName)) != NULL)
			strncpy(szLocalIPAddr, inet_ntoa(*(struct in_addr*)*pHostInfo->h_addr_list), IP_ADDR_LENGTH);
	}

	if((strcmp(szRemoteIPAddr, szLocalIPAddr) == 0 && strlen(szRemoteIPAddr) > 0 && strlen(szLocalIPAddr) > 0) ||
		strcmp(szRemoteIPAddr, "127.0.0.1") == 0)
	{
		// IP 가 맞는다!!!!
		// 종료를 해버리자~

		if(m_pServerMainClass && m_pfServerShutdown)
		{
			// 1초 후에 내린다.
			m_csIOCPServer.m_csTimer.AddTimer(1000, 1, m_pServerMainClass, m_pfServerShutdown, NULL);
		}
	}

	return TRUE;
}

PVOID AgsmServerStatus::MakeServerInfoPacket(INT16* pnPacketLength)
{
	if(!pnPacketLength)
		return NULL;

	switch(m_lServerType)
	{
		case ASSM_SERVER_TYPE_GAME_SERVER:
			return MakeServerInfoPacketForGameServer(pnPacketLength);

		case ASSM_SERVER_TYPE_LOGIN_SERVER:
			return MakeServerInfoPacketForLoginServer(pnPacketLength);

		case ASSM_SERVER_TYPE_RELAY_SERVER:
			return MakeServerInfoPacketForRelayServer(pnPacketLength);

		case ASSM_SERVER_TYPE_PATCH_SERVER:
			return MakeServerInfoPacketForPatchServer(pnPacketLength);
	}

	return NULL;
}

PVOID AgsmServerStatus::MakeServerInfoPacketForGameServer(INT16* pnPacketLength)
{
	if(!pnPacketLength)
		return NULL;

	INT32 lCurrentUserCount = 0;
	INT32 lMaxUserCount = 0;
	AgsdAdminPing csRelayPing;
	AgsdServer2* pThisServer = NULL;
	INT32 lServerType = (INT32)m_lServerType;
	INT32 lServerStatus = GetServerStatusForSM();
	INT32 lServerNumber = 0;
	INT32 lNoticeCount = 0;

	CHAR* szServerName = NULL;
	CHAR* szServerIP = NULL;

	INT32 lConnectedLoginServerCount = 0;
	INT32 lConnectedRelayServerCount = 0;

	/* by iluvs*/
	FLOAT lExpAdjustmentRatio = 0.0f;
	FLOAT lDropAdjustmentRatio = 0.0f;
	FLOAT lGheldAdjustmentRatio = 0.0f;

	lCurrentUserCount = m_pAgsmCharacter->GetNumOfPlayers();
	if(m_pagsmAdmin)
	{
		//lCurrentUserCount = m_pagsmAdmin->GetNumCurrentUser();
		m_pagsmAdmin->GetPingRelayInfo(&csRelayPing);
		lNoticeCount = m_pagsmAdmin->GetNoticeCount();
	}

	if(m_pagsmServerManager)
	{
		pThisServer = m_pagsmServerManager->GetThisServer();

		if(pThisServer)
		{
			// 2005.07.21. comment out by laki
			// !!!!! DB user명이 alef#이 아니라 다른 형태로 변경될 수 있다.
			// szServerName = pThisServer->m_szGroup;	// Group 이 DB 상의 World Field
			
			szServerName = pThisServer->m_szWorld;		// 2005.09.08. steeple 주석되어 있는 거 품.
			szServerIP = pThisServer->m_szIP;
			lServerNumber = pThisServer->m_lServerID;

			//if(strlen(pThisServer->m_szDBUser) > 4)
			//	lServerNumber = atoi(&pThisServer->m_szDBUser[4]);	// ALEF1 이런 형식이므로 ALEF 뒤의 숫자만 떼어온다.

			lMaxUserCount = pThisServer->m_lMaxUserCount;

			INT16 nIndex = 0;
			// LoginServer 돌면서 연결개수를 구한다.
			for(AgsdServer2* pLoginServer = m_pagsmServerManager->GetLoginServers(&nIndex); pLoginServer;
				pLoginServer = m_pagsmServerManager->GetLoginServers(&nIndex))
			{
				if(pLoginServer->m_bIsConnected)
					lConnectedLoginServerCount++;
			}

			nIndex = 0;
			AgsdServer2* pRelayServer = m_pagsmServerManager->GetRelayServer();
			if(pRelayServer)
			{
				if(pRelayServer->m_bIsConnected)
					lConnectedRelayServerCount++;
			}

			/* Get ExpRatio, DropRatio, Gheld Ratio by iluvs*/
			if(m_pagpmConfig)
			{
				lExpAdjustmentRatio = m_pagpmConfig->GetExpAdjustmentRatio();
				lDropAdjustmentRatio = m_pagpmConfig->GetDropAdjustmentRatio();
				lGheldAdjustmentRatio = m_pagpmConfig->GetGheldDropAdjustmentRatio();
			}
		}
	}

	return m_csPacket.m_csApsmServerInfoPacket.MakePacket(TRUE, pnPacketLength, ASSM_PACKET_SERVER_INFO,
						/*  1 */			&lServerType,
						/*  2 */			&lServerNumber,
						/*  3 */			NULL,				// Group 은 비우기로 했음.
						/*  4 */			szServerName,
						/*  5 */			szServerIP,
						/*  6 */			&lServerStatus,
						/*  7 */			&lCurrentUserCount,
						/*  8 */			NULL,
						/*  9 */			m_szProcessFileLastModifiedTime,
						/* 10 */			&lConnectedLoginServerCount,
						/* 11 */			&lConnectedRelayServerCount,
						/* 12 */			&csRelayPing.m_lPingTime,
						/* 13 */			NULL,
						/* 14 */			&csRelayPing.m_lPingSendCount,
						/* 15 */			&csRelayPing.m_lPingReceiveCount,
						/* 16 */			&lNoticeCount,
						/* 17 */			NULL,
						/* 18 */			NULL,
						/* 19 */			NULL,
						/* 20 */			&lExpAdjustmentRatio,
						/* 21 */			&lDropAdjustmentRatio,
						/* 22 */			&lGheldAdjustmentRatio
									);
}

PVOID AgsmServerStatus::MakeServerInfoPacketForRelayServer(INT16* pnPacketLength)
{
	if(!pnPacketLength)
		return NULL;

	AgsdServer2* pThisServer = NULL;
	INT32 lServerType = (INT32)m_lServerType;
	INT32 lServerStatus = GetServerStatusForSM();
	INT32 lServerNumber = 0;
	INT32 lRemained = 0;
	INT32 lFailed = 0;
	INT32 lPagedMemorySize = 0;
	INT32 lNonPagedMemorySize = 0;

	CHAR* szServerName = NULL;
	CHAR* szServerIP = NULL;

	if(m_pagsmServerManager)
	{
		pThisServer = m_pagsmServerManager->GetThisServer();

		if(pThisServer)
		{
			// 2005.07.21. comment out by laki
			//szServerName = pThisServer->m_szGroup;	// Group 이 DB 상의 World Field

			szServerName = pThisServer->m_szWorld;		// 2005.09.08. steeple -_-;; Game Server 만 하고 Relay 는 안했다. -0-;
			szServerIP = pThisServer->m_szIP;
			lServerNumber = pThisServer->m_lServerID;
		}
	}
	
	if(m_pAgsmDatabasePool)
	{
		INT32 lTempPosted = m_pAgsmDatabasePool->GetPosted();
		INT32 lTempExecuted = m_pAgsmDatabasePool->GetExecuted();
		INT32 lTempFailed = m_pAgsmDatabasePool->GetFailed();
		
		lRemained = lTempPosted - lTempExecuted;
		lFailed = lTempFailed;
	}

	PROCESS_MEMORY_COUNTERS	stMemory;
	if (GetProcessMemoryInfo(GetCurrentProcess(), &stMemory, sizeof(stMemory)))
	{
		lNonPagedMemorySize = (INT32 ) stMemory.QuotaNonPagedPoolUsage;
		lPagedMemorySize = (INT32 ) stMemory.QuotaPagedPoolUsage;
	}

	return m_csPacket.m_csApsmServerInfoPacket.MakePacket(TRUE, pnPacketLength, ASSM_PACKET_SERVER_INFO,
						/*  1 */			&lServerType,
						/*  2 */			&lServerNumber,
						/*  3 */			NULL,				// Group 은 비우기로 했음.
						/*  4 */			szServerName,
						/*  5 */			szServerIP,
						/*  6 */			&lServerStatus,
						/*  7 */			NULL,
						/*  8 */			NULL,
						/*  9 */			m_szProcessFileLastModifiedTime,
						/* 10 */			NULL,
						/* 11 */			NULL,
						/* 12 */			NULL,
						/* 13 */			&lFailed,			// 여기에 쿼리 실패 숫자 들어가면 된다.
						/* 14 */			&lRemained,
						/* 15 */			&lNonPagedMemorySize,
						/* 16 */			&lPagedMemorySize,
						/* 17 */			NULL,
						/* 18 */			NULL,
						/* 19 */			NULL,
						/* 20 */			NULL,
						/* 21 */			NULL,
						/* 22 */			NULL
									);
}

PVOID AgsmServerStatus::MakeServerInfoPacketForLoginServer(INT16* pnPacketLength)
{
	if(!pnPacketLength)
		return NULL;

	AgsdServer2* pThisServer = NULL;
	INT32 lServerType = (INT32)m_lServerType;
	INT32 lServerStatus = GetServerStatusForSM();
	INT32 lServerNumber = 0;
	INT32 lCurrentUserCount = 0;
	INT32 lIPBlock = 0;
	INT32 lConnectedGameServerCount = 0;

	CHAR* szServerName = NULL;
	CHAR* szServerIP = NULL;

	if(m_pagsmAccountManager)
	{
		lCurrentUserCount = m_pagsmAccountManager->GetAccountNum();
	}

	++m_lLiveTick;

	if(m_pagsmServerManager)
	{
		INT16 nIndex = 0;
		// GameServer 돌면서 연결개수를 구한다.
		for(AgsdServer2* pGameServer = m_pagsmServerManager->GetGameServers(&nIndex); pGameServer;
			pGameServer = m_pagsmServerManager->GetGameServers(&nIndex))
		{
			if(pGameServer->m_bIsConnected)
				lConnectedGameServerCount++;
		}

		pThisServer = m_pagsmServerManager->GetThisServer();

		if(pThisServer)
		{
			szServerName = pThisServer->m_szWorld;
			szServerIP = pThisServer->m_szIP;

			lServerNumber = pThisServer->m_lServerID;
		}
	}

	return m_csPacket.m_csApsmServerInfoPacket.MakePacket(TRUE, pnPacketLength, ASSM_PACKET_SERVER_INFO,
						/*  1 */			&lServerType,
						/*  2 */			&lServerNumber,
						/*  3 */			NULL,				// Group 은 비우기로 했음.
						/*  4 */			szServerName,
						/*  5 */			szServerIP,
						/*  6 */			&lServerStatus,
						/*  7 */			&lCurrentUserCount,
						/*  8 */			NULL,
						/*  9 */			m_szProcessFileLastModifiedTime,
						/* 10 */			NULL,
						/* 11 */			NULL,
						/* 12 */			NULL,
						/* 13 */			NULL,
						/* 14 */			&lIPBlock,				// GeneralParam1 에 IPBlock 인지를 넣자
						/* 15 */			&m_lLiveTick,			// GeneralParam1 에 LiveTick 인자를 넣자
						/* 16 */			&lConnectedGameServerCount,
						/* 17 */			NULL,
						/* 18 */			NULL,
						/* 19 */			NULL,
						/* 20 */			NULL,
						/* 21 */			NULL,
						/* 22 */			NULL
									);
}

PVOID AgsmServerStatus::MakeServerInfoPacketForPatchServer(INT16* pnPacketLength)
{
	if(!pnPacketLength)
		return NULL;

	AgsdServer2* pThisServer = NULL;
	INT32 lServerType = (INT32)m_lServerType;
	INT32 lServerStatus = GetServerStatusForSM();
	INT32 lServerNumber = 0;
	INT32 lCurrentUserCount = 0;
	INT32 lConnectedGameServerCount = 0;

	CHAR* szServerIP = NULL;
	INT16 nIPNum;

	GetLocalIPAddr(&szServerIP, &nIPNum);

	return m_csPacket.m_csApsmServerInfoPacket.MakePacket(TRUE, pnPacketLength, ASSM_PACKET_SERVER_INFO,
						/*  1 */			&lServerType,
						/*  2 */			&lServerNumber,
						/*  3 */			NULL,				// Group 은 비우기로 했음.
						/*  4 */			szServerIP,
						/*  5 */			szServerIP,
						/*  6 */			&lServerStatus,
						/*  7 */			NULL,
						/*  8 */			NULL,
						/*  9 */			m_szProcessFileLastModifiedTime,
						/* 10 */			NULL,
						/* 11 */			NULL,
						/* 12 */			NULL,
						/* 13 */			NULL,
						/* 14 */			&m_lPatchVersion,
						/* 15 */			NULL,
						/* 16 */			NULL,
						/* 17 */			NULL,
						/* 18 */			NULL,
						/* 19 */			NULL,
						/* 20 */			NULL,
						/* 21 */			NULL,
						/* 22 */			NULL
									);
}

BOOL AgsmServerStatus::SendServerInfoPacket(INT32 lIndex)
{
	AsServerSocket* pSocket = NULL;

	if (lIndex >= 0)
	{
		pSocket = m_csIOCPServer.m_csSocketManager[lIndex];
		if(!pSocket)
			return FALSE;
	}

	INT16 nPacketSize = 0;
	PVOID pvPacket = MakeServerInfoPacket(&nPacketSize);
	if(!pvPacket || nPacketSize < 1)
		return FALSE;

	BOOL bSendResult = TRUE;

	if (lIndex >= 0)
		bSendResult = pSocket->AsyncSend((CHAR*)pvPacket, (INT32)nPacketSize, PACKET_PRIORITY_1);
	else
		m_csIOCPServer.m_csSocketManager.SendPacketToAll((CHAR*)pvPacket, (INT32)nPacketSize, PACKET_PRIORITY_1);

	m_csPacket.m_csApsmServerInfoPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmServerStatus::SendServerControlPacket(INT32 lIndex)
{
	return TRUE;
}

BOOL AgsmServerStatus::SendGeneralParamStringPacket(INT32 lIndex, char *pszConsoleIP, char *pszString)
{
	AsServerSocket* pSocket = NULL;

	if (lIndex >= 0)
	{
		pSocket = m_csIOCPServer.m_csSocketManager[lIndex];
		if(!pSocket)
			return FALSE;
	}

	int lServerType = ASSM_SERVER_TYPE_GENERAL_PARAM_STRING;

	INT16 nPacketSize = 0;
	PVOID pvPacket
		= m_csPacket.m_csApsmServerInfoPacket.MakePacket(TRUE, &nPacketSize, ASSM_PACKET_SERVER_INFO,
						/*  1 */			&lServerType,
						/*  2 */			NULL,
						/*  3 */			NULL,
						/*  4 */			NULL,
						/*  5 */			NULL,
						/*  6 */			NULL,
						/*  7 */			NULL,
						/*  8 */			NULL,
						/*  9 */			NULL,
						/* 10 */			NULL,
						/* 11 */			NULL,
						/* 12 */			NULL,
						/* 13 */			NULL,
						/* 14 */			NULL,
						/* 15 */			NULL,
						/* 16 */			NULL,
						/* 17 */			pszString,
						/* 18 */			pszConsoleIP,
						/* 19 */			NULL,
						/* 20 */			NULL,
						/* 21 */			NULL,
						/* 22 */			NULL
								);

	if(!pvPacket || nPacketSize < 1)
		return FALSE;

	BOOL bSendResult = TRUE;

	if (lIndex >= 0)
		bSendResult = pSocket->AsyncSend((CHAR*)pvPacket, (INT32)nPacketSize, PACKET_PRIORITY_1);
	else
		m_csIOCPServer.m_csSocketManager.SendPacketToAll((CHAR*)pvPacket, (INT32)nPacketSize, PACKET_PRIORITY_1);

	m_csPacket.m_csApsmServerInfoPacket.FreePacket(pvPacket);

	return bSendResult;
}

INT32 AgsmServerStatus::GetServerStatusForSM()
{
	INT16	nStatus = GetServerStatus();

	switch (nStatus)
	{
	case GF_SERVER_START:
	case GF_SERVER_FULL:
		return ASSM_SERVER_STATE_STARTED;
	case GF_SERVER_STOP:
		return ASSM_SERVER_STATE_NOT_ACTIVE;
	case GF_SERVER_LOST_CONNECTION:
		return ASSM_SERVER_STATE_ABNORMAL;
	default:
		return ASSM_SERVER_STATE_LOADING;
	/*
	GF_SERVER_SETUP_CONNECTION
	GF_SERVER_COMPLETE_CONNECTION
	*/
	}

	return ASSM_SERVER_STATE_NOT_ACTIVE;
}

BOOL AgsmServerStatus::CBEnumNotice(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgsmServerStatus *	pThis = (AgsmServerStatus *) pvClass;
	INT32				lSocket = *(INT32 *) pvCustData;
	AssmNoticeRepeat *	pcsNotice = (AssmNoticeRepeat *) pvData;
	CHAR				szMessage[256];

	sprintf(szMessage, "%d : Dur(%d), Gap(%d), \"%s\"",
						pcsNotice->m_lID,
						pcsNotice->m_ulDuration,
						pcsNotice->m_ulGap,
						pcsNotice->m_szMessage);

	pThis->SendGeneralParamStringPacket(lSocket, NULL, szMessage);

	return TRUE;
}

BOOL AgsmServerStatus::MakeAndSendGameInfoPacket(INT32 lIndex, CHAR* _KillCharacterName, CHAR* _KillRace, CHAR* _KilledCharacter, CHAR* _KilledRace)
{
	AsServerSocket* pSocket = NULL;

	AgsdServer2 *pThisServer = m_pagsmServerManager->GetThisServer();

	if(!pThisServer)
		return FALSE;

	if (lIndex >= 0)
	{
		pSocket = m_csIOCPServer.m_csSocketManager[lIndex];
		if(!pSocket)
			return FALSE;
	}

	PACKET_ASSM_BATTLEGROUND_INFO pPacket(pThisServer->m_szWorld, _KillCharacterName, _KillRace, _KilledCharacter, _KilledRace);

	UINT16	nLength = pPacket.unPacketLength + 1; // end guard byte

	PVOID pvPacketRaw = g_AuCircularBuffer.Alloc(nLength);		// 패킷 앞뒤로 가드바이트를 붙인다.

	if (!pvPacketRaw) return FALSE;

	CopyMemory(pvPacketRaw, &pPacket, nLength);

	// set guard byte
	*((BYTE *) pvPacketRaw)					= APPACKET_FRONT_GUARD_BYTE;
	*((BYTE *) pvPacketRaw + nLength - 1)	= APPACKET_REAR_GUARD_BYTE;

	((PACKET_HEADER *) pvPacketRaw)->unPacketLength		= nLength;
	((PACKET_HEADER *) pvPacketRaw)->lOwnerID			= 0;
	((PACKET_HEADER *) pvPacketRaw)->Flag.Compressed	= 0;
	((PACKET_HEADER *) pvPacketRaw)->cType				= pPacket.cType;

	BOOL bSendResult = TRUE;

	if (pSocket)
		bSendResult = pSocket->AsyncSend((CHAR*)pvPacketRaw, (INT32)nLength, PACKET_PRIORITY_1);
	else
		m_csIOCPServer.m_csSocketManager.SendPacketToAll((CHAR*)pvPacketRaw, (INT32)nLength, PACKET_PRIORITY_1);

	g_AuCircularBuffer.Free(pvPacketRaw);

	return bSendResult;
}
