/******************************************************************************
Module:  AgsmServerManager.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 12. 10
******************************************************************************/

#include <stdio.h>
#include <time.h>

#include "AgsmServerManager.h"

INT32	g_lNumGetDBID	= 0;

AgsmServerManager::AgsmServerManager()
{
	SetModuleName("AgsmServerManager");

	SetModuleType(APMODULE_TYPE_SERVER);

	SetModuleData(sizeof(AgsdServer), AGSMSERVER_DATA_TYPE_SERVER);
	SetModuleData(sizeof(AgsdServerTemplate), AGSMSERVER_DATA_TYPE_TEMPLATE);

	//SetPacketType(AGSMSERVER_PACKET_TYPE);

	// flag length = 16bits
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,		// operation
							AUTYPE_INT32,	1,		// server id
							AUTYPE_CHAR,	12,		// server name
							AUTYPE_INT8,	1,		// server status
							AUTYPE_END,		0
							);

	m_bIsConnect	= FALSE;

	m_nTotalConnect	= 0;
	m_nNumAccept	= 0;

	m_pcsLoginServer = NULL;
	m_pcsDealServer	= NULL;
	m_pcsRelayServer = NULL;
	m_pcsRecruitServer = NULL;

	m_lServerID		= AP_INVALID_SERVERID;
	m_lServerTID	= AP_INVALID_SERVERID;

    m_hGetItemDBIDEvent		= CreateEvent( NULL, FALSE, FALSE, NULL );
    m_hGetSkillDBIDEvent	= CreateEvent( NULL, FALSE, FALSE, NULL );
}

AgsmServerManager::~AgsmServerManager()
{
	CloseHandle(m_hGetItemDBIDEvent);
	CloseHandle(m_hGetSkillDBIDEvent);
}

BOOL AgsmServerManager::OnAddModule()
{
	//m_papmMap = (ApmMap *) GetModule("ApmMap");
	m_pagsmDBStream		= (AgsmDBStream *)	GetModule("AgsmDBStream");
	m_pagsmAOIFilter	= (AgsmAOIFilter *)	GetModule("AgsmAOIFilter");

	// JNY TODO : Relay 서버 개발을 위해 m_pagsmDBStream를 체크하는 부분을 
	// 잠시 삭제합니다. 
	// 2004.2.16
	// 다시 복구 하거나 DB모듈 변경에 의한 변경작업을 해야합니다.

	if (/*!m_pagsmDBStream || */
		!m_pagsmAOIFilter)
		return FALSE;

	//if (!m_papmMap)
	//	return FALSE;

	//m_nIndexADMap = m_papmMap->AttachSectorData(this, sizeof(AgsdControlServer), ConAgsdControlServer, NULL);

	if (!AddStreamCallback(AGSMSERVER_DATA_TYPE_TEMPLATE, TemplateReadCB, TemplateWriteCB, this))
	{
		OutputDebugString("AgsmServerManager::OnAddModule() Error (1) !!!\n");
		return FALSE;
	}

	if (m_pagsmDBStream)
	{
		if (!m_pagsmDBStream->SetCallbackMaxIDResult(CBStreamDB, this))
			return FALSE;
	}

	return TRUE;
}	

BOOL AgsmServerManager::OnIdle50Times()
{
	// 게임서버인경우 로긴서버로 이 서버의 상태를 보낸다.
	//
	//

	return TRUE;
}

BOOL AgsmServerManager::OnInit()
{
	if (!m_csServerAdmin.InitializeObject(sizeof(AgsdServer *), m_csServerAdmin.GetCount(), NULL, NULL, this))
		return FALSE;

	if (!m_csTemplateAdmin.InitializeObject(sizeof(AgsdServerTemplate *), m_csTemplateAdmin.GetCount(), NULL, NULL, this))
		return FALSE;

	m_dpnidGameServers = m_pagsmAOIFilter->CreateGroup(20);

	return TRUE;
}

BOOL AgsmServerManager::OnDestroy()
{
	m_pagsmAOIFilter->DestroyGroup(m_dpnidGameServers);

	INT32	lIndex	= 0;
	AgsdServerTemplate	**ppcsServerTemplate = (AgsdServerTemplate **) m_csTemplateAdmin.GetObjectSequence(&lIndex);

	while (ppcsServerTemplate)
	{
		DestroyTemplate(*ppcsServerTemplate);

		ppcsServerTemplate = (AgsdServerTemplate **) m_csTemplateAdmin.GetObjectSequence(&lIndex);
	}

	lIndex	= 0;
	AgsdServer	**ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);

	while (ppcsServer)
	{
		DestroyServer(*ppcsServer);

		ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);
	}

	return TRUE;
}

BOOL AgsmServerManager::OnValid(CHAR* szData, INT16 nSize)
{
	return TRUE;
}

BOOL AgsmServerManager::SetMaxServer(INT16 nCount)
{
	return m_csServerAdmin.SetCount(nCount);
}

BOOL AgsmServerManager::SetMaxTemplate(INT16 nCount)
{
	return m_csTemplateAdmin.SetCount(nCount);
}

/*
BOOL AgsmServerManager::ConAgsdControlServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmServerManager *pThis = (AgsmServerManager *) pClass;
	AgsdControlServer *pcsControlServer = pThis->GetADMap(pData);

	if (!pcsControlServer)
		return FALSE;

	pcsControlServer->m_lServerID = AP_INVALID_CID;
	pcsControlServer->m_pcsServer = NULL;

	return TRUE;
}
*/

AgsdServer* AgsmServerManager::CreateServer()
{
	AgsdServer *pcsServer = (AgsdServer *) CreateModuleData(AGSMSERVER_DATA_TYPE_SERVER);

	if (pcsServer)
	{
		pcsServer->m_Mutex.Init((PVOID) pcsServer);
		pcsServer->m_eType = APBASE_TYPE_SERVER;

		pcsServer->m_dpnidServer	= 0;
		pcsServer->m_lServerID		= AP_INVALID_SERVERID;
		pcsServer->m_nidServer		= 0;
		pcsServer->m_nStatus		= GF_SERVER_STOP;

		pcsServer->m_bIsAccepted	= FALSE;
		pcsServer->m_bIsConnected	= FALSE;
		pcsServer->m_bIsAuth		= FALSE;

		pcsServer->m_pcsTemplate	= NULL;
	}

	return pcsServer;
}

BOOL AgsmServerManager::DestroyServer(AgsdServer *pcsServer)
{
	pcsServer->m_Mutex.Destroy();

	return DestroyModuleData((PVOID) pcsServer, AGSMSERVER_DATA_TYPE_SERVER);
}

AgsdServerTemplate* AgsmServerManager::CreateTemplate()
{
	AgsdServerTemplate *pcsTemplate = (AgsdServerTemplate *) CreateModuleData(AGSMSERVER_DATA_TYPE_TEMPLATE);

	if (pcsTemplate)
	{
		pcsTemplate->m_Mutex.Init((PVOID) pcsTemplate);
		pcsTemplate->m_eType = APBASE_TYPE_SERVER_TEMPLATE;
		pcsTemplate->m_bIsConnectLoginServer = FALSE;

		pcsTemplate->m_nServerFlag = 0;
		pcsTemplate->m_nFlagSize = 0;

		pcsTemplate->m_ullItemDBIDStart		= 0;
		pcsTemplate->m_ullSkillDBIDStart	= 0;
	}

	return pcsTemplate;
}

BOOL AgsmServerManager::DestroyTemplate(AgsdServerTemplate *pcsTemplate)
{
	pcsTemplate->m_Mutex.Destroy();

	return DestroyModuleData((PVOID) pcsTemplate, AGSMSERVER_DATA_TYPE_TEMPLATE);
}

AgsdServer* AgsmServerManager::AddServer(INT32 lTID, INT16 nServerType)
{
	AgsdServer *pcsServer = CreateServer();
	if (!pcsServer)
		return NULL;

	pcsServer->m_pcsTemplate = GetServerTemplate(lTID);

	if (!pcsServer->m_pcsTemplate)
	{
		DestroyServer(pcsServer);
		return NULL;
	}

	switch (((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType) {
	case AGSMSERVER_TYPE_LOGIN_SERVER:
		m_pcsLoginServer = pcsServer;
		break;

	case AGSMSERVER_TYPE_DEAL_SERVER:
		m_pcsDealServer = pcsServer;
		break;

	case AGSMSERVER_TYPE_RELAY_SERVER:
		m_pcsRelayServer = pcsServer;
		break;
	}

	pcsServer->m_lServerID = lTID;
	pcsServer->m_lTID = lTID;

	if (!m_csServerAdmin.AddServer(pcsServer, pcsServer->m_lServerID, nServerType))
	{
		DestroyServer(pcsServer);
		return NULL;
	}

	return pcsServer;
}

AgsdServer* AgsmServerManager::GetServer(INT32 lServerID)
{
	return m_csServerAdmin.GetServer(lServerID);
}

AgsdServer*	AgsmServerManager::GetServerName(CHAR *szServerName)
{
	if (!szServerName)
		return NULL;

	return m_csServerAdmin.GetServer(szServerName);
}

AgsdServer* AgsmServerManager::GetServer(CHAR *szAddress)
{
	if (!szAddress || !strlen(szAddress))
		return NULL;

	INT16	nIndex = 0;
	AgsdServer *pcsServer = m_csServerAdmin.GetGameServers(&nIndex);
	while (pcsServer)
	{
		if (!strncmp(((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr, szAddress, 23))
		{
			return pcsServer;
		}

		pcsServer = m_csServerAdmin.GetGameServers(&nIndex);
	}

	return NULL;
}

//게임서버 그룹이름들을 얻어내는 함수이다. 현재는 그냥 존재한다. -_-; 이광준 대리님께서 바꿔주신다고
//하셨으니 믿고 그냥 쓰자~ 2003/4/28
char *AgsmServerManager::GetGameServerGroupName()
{
	return NULL;
}

BOOL AgsmServerManager::RemoveServer(INT32 lServerID)
{
	AgsdServer *pcsServer = GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	if (!m_csServerAdmin.RemoveServer(lServerID))
		return FALSE;

	return DestroyServer(pcsServer);
}

AgsdServerTemplate* AgsmServerManager::AddServerTemplate(INT32 lTID)
{
	AgsdServerTemplate *pcsServerTemplate = CreateTemplate();
	if (!pcsServerTemplate)
		return NULL;

	pcsServerTemplate->m_lTID = lTID;
	
	if (!m_csTemplateAdmin.AddServerTemplate(pcsServerTemplate, lTID))
	{
		DestroyTemplate(pcsServerTemplate);
		return NULL;
	}

	return pcsServerTemplate;
}

AgsdServerTemplate* AgsmServerManager::GetServerTemplate(INT32 lTID)
{
	return m_csTemplateAdmin.GetServerTemplate(lTID);
}

BOOL AgsmServerManager::RemoveServerTemplate(INT32 lTID)
{
	AgsdServerTemplate *pcsServerTemplate = GetServerTemplate(lTID);
	if (!pcsServerTemplate)
		return FALSE;

	if (!m_csTemplateAdmin.RemoveServerTemplate(lTID))
		return FALSE;

	return DestroyTemplate(pcsServerTemplate);
}

BOOL AgsmServerManager::StreamReadTemplate(CHAR *szFile)
{
	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	INT32				lTID;
	AgsdServerTemplate	*pcsServerTemplate;

	// szFile을 읽는다.
	csStream.Open(szFile);

	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; i++)
	{
		// Section Name은 TID 이다.
		lTID = atoi(csStream.ReadSectionName(i));

		// Template을 추가한다.
		pcsServerTemplate = AddServerTemplate(lTID);
		if (!pcsServerTemplate)
			return FALSE;

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGSMSERVER_DATA_TYPE_TEMPLATE, pcsServerTemplate, this))
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmServerManager::StreamWriteTemplate(CHAR *szFile)
{
	return TRUE;
}

BOOL AgsmServerManager::TemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	const CHAR *szValueName;
	CHAR szValue[64];
	AgsmServerManager *pThis = (AgsmServerManager *) pClass;
	AgsdServerTemplate *pcsServerTemplate = (AgsdServerTemplate *) pData;

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGSMSERVER_INI_NAME_GROUP_NAME))
		{
			pStream->GetValue(pcsServerTemplate->m_szGroupName, AGSM_MAX_SERVER_NAME);
		}
		else if (!strcmp(szValueName, AGSMSERVER_INI_NAME_SERVER_TYPE))
		{
			pStream->GetValue(szValue, 64);

			switch (szValue[0]) {
			case '0':
				pcsServerTemplate->m_cType = AGSMSERVER_TYPE_GAME_SERVER;
				break;
			case '1':
				pcsServerTemplate->m_cType = AGSMSERVER_TYPE_MASTER_SERVER;
				break;
			case '2':
				pcsServerTemplate->m_cType = AGSMSERVER_TYPE_BACKUP_SERVER;
				break;
			case '3':
				pcsServerTemplate->m_cType = AGSMSERVER_TYPE_LOGIN_SERVER;
				break;
			case '4':
				pcsServerTemplate->m_cType = AGSMSERVER_TYPE_CHARACTER_SERVER;
				break;
			case '5':
				pcsServerTemplate->m_cType = AGSMSERVER_TYPE_ITEM_SERVER;
				break;
			case '6':
				pcsServerTemplate->m_cType = AGSMSERVER_TYPE_DEAL_SERVER;
				break;
			case '7':
				pcsServerTemplate->m_cType = AGSMSERVER_TYPE_RECRUIT_SERVER;
				break;
			case '8':
				pcsServerTemplate->m_cType = AGSMSERVER_TYPE_RELAY_SERVER;
				break;
			}
		}
		else if (!strcmp(szValueName, AGSMSERVER_INI_NAME_SERVER_NAME))
		{
			pStream->GetValue(pcsServerTemplate->m_szName, AGSM_MAX_SERVER_NAME);
		}
		else if (!strcmp(szValueName, AGSMSERVER_INI_NAME_SERVER_ADDRESS))
		{
			pStream->GetValue(pcsServerTemplate->m_szIPv4Addr, 23);
		}
		else if (!strncmp(szValueName, AGSMSERVER_INI_NAME_CONTROL_AREA, strlen(AGSMSERVER_INI_NAME_CONTROL_AREA)))
		{
			pStream->GetValue(szValue, 64);

			if (pcsServerTemplate->m_nControlArea == AGSMSERVER_MAX_CONTROL_AREA)
			{
				WriteLog(AS_LOG_RELEASE, "AgsmServerManager : overflow control area");

				return FALSE;
			}

			sscanf(szValue, "%f,%f,%f,%f", &pcsServerTemplate->m_csControlArea[pcsServerTemplate->m_nControlArea].m_stControlArea.posLeftTop.x,
										   &pcsServerTemplate->m_csControlArea[pcsServerTemplate->m_nControlArea].m_stControlArea.posLeftTop.z,
										   &pcsServerTemplate->m_csControlArea[pcsServerTemplate->m_nControlArea].m_stControlArea.posRightBottom.x,
										   &pcsServerTemplate->m_csControlArea[pcsServerTemplate->m_nControlArea].m_stControlArea.posRightBottom.z);

			pcsServerTemplate->m_nControlArea++;
		}
		else if (!strncmp(szValueName, AGSMSERVER_INI_NAME_IS_CONNECT_LOGIN, strlen(AGSMSERVER_INI_NAME_IS_CONNECT_LOGIN)))
		{
			pStream->GetValue(szValue, 64);

			INT16	nTemp;
			sscanf(szValue, "%d", &nTemp);

			if (nTemp == 0)
				pcsServerTemplate->m_bIsConnectLoginServer = FALSE;
			else
				pcsServerTemplate->m_bIsConnectLoginServer = TRUE;
		}
		else if (!strncmp(szValueName, AGSMSERVER_INI_NAME_ID_SERVER_FLAG, strlen(AGSMSERVER_INI_NAME_ID_SERVER_FLAG)))
		{
			pStream->GetValue(szValue, 64);

			pcsServerTemplate->m_nServerFlag = atoi(szValue);
		}
		else if (!strncmp(szValueName, AGSMSERVER_INI_NAME_ID_FLAG_SIZE, strlen(AGSMSERVER_INI_NAME_ID_FLAG_SIZE)))
		{
			pStream->GetValue(szValue, 64);

			pcsServerTemplate->m_nFlagSize = atoi(szValue);
		}
		else if (!strncmp(szValueName, AGSMSERVER_INI_NAME_ITEM_DBID_SERVER_FLAG, strlen(AGSMSERVER_INI_NAME_ITEM_DBID_SERVER_FLAG)))
		{
			pStream->GetValue(szValue, 64);

			pcsServerTemplate->m_nItemDBIDServerFlag = atoi(szValue);
		}
		else if (!strncmp(szValueName, AGSMSERVER_INI_NAME_ITEM_DBID_FLAG_SIZE, strlen(AGSMSERVER_INI_NAME_ITEM_DBID_FLAG_SIZE)))
		{
			pStream->GetValue(szValue, 64);

			pcsServerTemplate->m_nItemDBIDFlagSize = atoi(szValue);
		}
		else if (!strncmp(szValueName, AGSMSERVER_INI_NAME_SKILL_DBID_SERVER_FLAG, strlen(AGSMSERVER_INI_NAME_SKILL_DBID_SERVER_FLAG)))
		{
			pStream->GetValue(szValue, 64);

			pcsServerTemplate->m_nSkillDBIDServerFlag = atoi(szValue);
		}
		else if (!strncmp(szValueName, AGSMSERVER_INI_NAME_SKILL_DBID_FLAG_SIZE, strlen(AGSMSERVER_INI_NAME_SKILL_DBID_FLAG_SIZE)))
		{
			pStream->GetValue(szValue, 64);

			pcsServerTemplate->m_nSkillDBIDFlagSize = atoi(szValue);
		}
	}

	return TRUE;
}

BOOL AgsmServerManager::TemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	return TRUE;
}

AgsdServer* AgsmServerManager::GetControlServer(AuPOS posDestArea, AgsdControlArea *pstCurrentControlArea)
{
	if (pstCurrentControlArea)
	{
		for (int i = 0; i < pstCurrentControlArea->m_nNumAroundUnit; i++)
		{
			if (pstCurrentControlArea->m_pcsAroundUnit[i])
			{
				for (int j = 0; j < ((AgsdServerTemplate *) ((AgsdServer *) pstCurrentControlArea->m_pcsAroundUnit[i])->m_pcsTemplate)->m_nControlArea; j++)
				{
					if (IsInArea(((AgsdServerTemplate *) ((AgsdServer *) pstCurrentControlArea->m_pcsAroundUnit[i])->m_pcsTemplate)->m_csControlArea[j].m_stControlArea, posDestArea))
					{
						return (AgsdServer *) pstCurrentControlArea->m_pcsAroundUnit[i];
					}
				}
			}
		}
	}
	else
	{
		INT16 nIndex = 0;

		AgsdServer *pcsServer = m_csServerAdmin.GetGameServers(&nIndex);

		while (pcsServer)
		{
			for (int i = 0; i < ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_nControlArea; i++)
			{
				if (IsInArea(((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_csControlArea[i].m_stControlArea, posDestArea))
					return pcsServer;
			}

			pcsServer = m_csServerAdmin.GetGameServers(&nIndex);
		}
	}

	return NULL;
}

BOOL AgsmServerManager::IsInArea(AuAREA csArea, AuPOS csPos)
{
	if (csArea.posLeftTop.x <= csPos.x &&
		csArea.posLeftTop.z <= csPos.z &&
		csArea.posRightBottom.x >= csPos.x &&
		csArea.posRightBottom.z >= csPos.z)
		return TRUE;

	return FALSE;
}

BOOL AgsmServerManager::Connect(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	AgsdServerTemplate *pcsTemplate = (AgsdServerTemplate *) pcsServer->m_pcsTemplate;

	pcsServer->m_nidServer = ConnectServer(pcsTemplate->m_szIPv4Addr);
	if (pcsServer->m_nidServer < 0)
	{
		//pcsServer->m_bIsConnected = FALSE;
		//pcsServer->m_bIsAuth		= FALSE;
		//pcsServer->m_bIsAccepted	= FALSE;

		return FALSE;
	}

	pcsServer->m_bIsConnected = TRUE;

	if (!SendConnectPacket(pcsServer))
	{
		Disconnect(pcsServer, AGSMSERVER_CONNECT_TYPE_REQUEST);

		return FALSE;
	}

	pcsServer->m_bIsAuth = FALSE;

	CHAR	szBuffer[128];
	sprintf(szBuffer, "request connection : server(%s)", pcsTemplate->m_szIPv4Addr);
	WriteLog(AS_LOG_RELEASE, szBuffer);

	return TRUE;
}


BOOL AgsmServerManager::SendConnectPacket(AgsdServer *pcsServer)
{
	INT16	nPacketLength;
	INT8	cOperation = AGSMSERVER_PACKET_OPERATION_CONNECT;
	INT8	cServerStatus = (INT8) GetServerStatus();
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE, 
											&cOperation,
											&m_lServerID,
											m_szServerName,
											&cServerStatus);

	if (!pvPacket)
	{
		return FALSE;
	}

	if (!SendPacketSvr(pvPacket, nPacketLength, pcsServer->m_nidServer))
	{
		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	//AddIdleEvent(GetClockCount() + AGSMSERVER_MAX_WAIT_CONNECT_RESULT_CLOCK, pcsServer->m_lServerID, this, CheckConnect);

	return TRUE;
}

BOOL AgsmServerManager::SendConnectResultPacket(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	INT16	nPacketLength;
	INT8	cOperation = AGSMSERVER_PACKET_OPERATION_CONNECT_RESULT;
	INT8	cServerStatus = (INT8) GetServerStatus();
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMSERVER_PACKET_TYPE, 
											&cOperation,
											&m_lServerID,
											m_szServerName,
											&cServerStatus);

	if (!pvPacket)
	{
		return FALSE;
	}

	if (!SendPacket(pvPacket, nPacketLength, pcsServer->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmServerManager::CheckConnect(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	AgsmServerManager	*pThis = (AgsmServerManager *) pClass;

	AgsdServer			*pServer = pThis->GetServer(lCID);
	if (!pServer)
		return FALSE;

	pServer->m_Mutex.WLock();

	// 연결은 되어있지만 인증을 안받은경우가 있는지 검사한다.
	if (pServer->m_bIsAuth == FALSE)
	{
		// 제대로 응답 패킷을 못받았다.	 다쉬 연결한다.
		if (pServer->m_bIsConnected)
			pThis->Disconnect(pServer, AGSMSERVER_CONNECT_TYPE_REQUEST);

		if (pThis->Connect(pServer) && pThis->IsConnectComplete())
		{
			pServer->m_Mutex.Release();

			return TRUE;
		}

		pThis->AddIdleEvent(pThis->GetClockCount() + AGSMSERVER_MAX_WAIT_CONNECT_RESULT_CLOCK, pServer->m_lServerID, pThis, CheckConnect, NULL);
	}

	pThis->IsConnectComplete();

	pServer->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmServerManager::Disconnect(AgsdServer *pcsServer, INT16 nConnectType)
{
	if (!pcsServer)
		return FALSE;

	switch (nConnectType) {
	case AGSMSERVER_CONNECT_TYPE_REQUEST:
		{
			if (pcsServer->m_bIsConnected)
				DisconnectServer(pcsServer->m_nidServer);

			pcsServer->m_bIsConnected	= FALSE;
			pcsServer->m_bIsAuth		= FALSE;
		}
		break;

	case AGSMSERVER_CONNECT_TYPE_ACCEPT:
		{
			if (pcsServer->m_bIsAccepted)
			{
				m_pagsmAOIFilter->RemovePlayerFromGroup(m_dpnidGameServers, pcsServer->m_dpnidServer);
				DestroyClient(pcsServer->m_dpnidServer);
			}

			pcsServer->m_bIsAccepted	= FALSE;
		}
		break;
	};

	CHAR	szBuffer[128];
	sprintf(szBuffer, "disconnect server(%s)", ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
	WriteLog(AS_LOG_RELEASE, szBuffer);

	return TRUE;
}

BOOL AgsmServerManager::TryConnection()
{
	BOOL	bResult = TRUE;

	INT32	lIndex = 0;

	AgsdServer **ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);
	if (!ppcsServer)
	{
		SetServerStatus(GF_SERVER_COMPLETE_CONNECTION);

		return TRUE;	// Parn님 작업, 원래 FALSE 였음.
	}

	AgsdServerTemplate *pcsTemplate = (AgsdServerTemplate *) (*ppcsServer)->m_pcsTemplate;

	while (ppcsServer)
	{
		if ((*ppcsServer)->m_lServerID == m_lServerID)	// 자기 자신인 경우 넘어간다.
		{
			ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);

			if (ppcsServer)
			{
				pcsTemplate = (AgsdServerTemplate *) (*ppcsServer)->m_pcsTemplate;
			}

			continue;
		}

		(*ppcsServer)->m_Mutex.WLock();

		if ((*ppcsServer)->m_bIsConnected == TRUE)		// 이미 접속을 한넘이다.
		{
			(*ppcsServer)->m_Mutex.Release();

			ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);
			if (ppcsServer)
				pcsTemplate = (AgsdServerTemplate *) (*ppcsServer)->m_pcsTemplate;

			continue;
		}

		if (!Connect(*ppcsServer))
		{
			bResult = FALSE;

			//CHAR szLog[128];
			//sprintf(szLog, "AgsmServerManager: connect Server (%s) failed", pcsTemplate->m_szIPv4Addr);
			//WriteLog(AS_LOG_RELEASE, szLog);
		}

		AddIdleEvent(GetClockCount() + AGSMSERVER_MAX_WAIT_CONNECT_RESULT_CLOCK, (*ppcsServer)->m_lServerID, this, CheckConnect, NULL);

		(*ppcsServer)->m_Mutex.Release();

		ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);
		if (ppcsServer)
			pcsTemplate = (AgsdServerTemplate *) (*ppcsServer)->m_pcsTemplate;
	}

	return bResult;
}

BOOL AgsmServerManager::SetupConnection()
{
//	if (!InitServerData())
//		return FALSE;

	AgsdServer **ppcsServer;

	INT32	lIndex = 0;

	SetServerStatus(GF_SERVER_SETUP_CONNECTION);

	//나 자신을 찾아서 시작시간을 넣는다.
	while (ppcsServer)
	{
		ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);

		if ((*ppcsServer)->m_lServerID == m_lServerID)
		{
			(*ppcsServer)->m_lTime = time( NULL );
			break;
		}
	}

	if (m_nTotalConnect == 0)
	{
		SetServerStatus(GF_SERVER_COMPLETE_CONNECTION);

		return TRUE;
	}

	TryConnection();

	IsConnectComplete();

	return TRUE;
}

BOOL AgsmServerManager::InitServerData()
{
	// Initialize Template Data

	// Initialize Template Data

	// Initialize Server Data
	INT32	lIndex = 0;
	AgsdServerTemplate **ppcsThisServer = NULL;
	AgsdServerTemplate **ppcsTemplate = (AgsdServerTemplate **) m_csTemplateAdmin.GetObjectSequence(&lIndex);
	if (!ppcsTemplate)
		return FALSE;

	CHAR	*szLocalIPAddr = NULL;
	INT16	nNumAddr;
	GetLocalIPAddr(&szLocalIPAddr, &nNumAddr);

	while (ppcsTemplate)
	{
		for (int i = 0; i < nNumAddr; i++)
		{
			if (!strcmp((*ppcsTemplate)->m_szIPv4Addr, szLocalIPAddr + 23 * i))
			{
				// 자기 자신이당.
				m_lServerID = m_lServerTID = (*ppcsTemplate)->m_lTID;
				strncpy(m_szGroupName, (*ppcsTemplate)->m_szGroupName, AGSM_MAX_SERVER_NAME);
				strncpy(m_szServerName, (*ppcsTemplate)->m_szName, AGSM_MAX_SERVER_NAME);

				SetSelfCID(m_lServerID);

				ppcsThisServer = ppcsTemplate;

				break;
			}
		}

		if (i < nNumAddr)
			break;

		ppcsTemplate = (AgsdServerTemplate **) m_csTemplateAdmin.GetObjectSequence(&lIndex);
	}

	if (!ppcsThisServer)
	{
		WriteLog(AS_LOG_RELEASE, "AgsmServerManager : 이 서버 템플릿이 존재하지 않는다... ");

		return FALSE;
	}

	lIndex = 0;
	ppcsTemplate = (AgsdServerTemplate **) m_csTemplateAdmin.GetObjectSequence(&lIndex);
	if (!ppcsTemplate)
		return FALSE;
	while (ppcsTemplate)
	{
		AgsdServer *pcsServer = AddServer((*ppcsTemplate)->m_lTID, (*ppcsTemplate)->m_cType);
		if (!pcsServer)
			return FALSE;

		// 같은 서버군내에 있는 서버들 데이타를 추가한다.
		if (!strncmp((*ppcsTemplate)->m_szGroupName, m_szGroupName, AGSM_MAX_SERVER_NAME))
		{
			if ((*ppcsTemplate)->m_cType == AGSMSERVER_TYPE_DEAL_SERVER)
			{
				m_pcsDealServer = pcsServer;
			}
			else if ((*ppcsTemplate)->m_cType == AGSMSERVER_TYPE_RECRUIT_SERVER)
			{
				m_pcsRecruitServer = pcsServer;
			}
			else if ((*ppcsTemplate)->m_cType == AGSMSERVER_TYPE_RELAY_SERVER)
			{
				m_pcsRelayServer = pcsServer;
			}
		}

		ppcsTemplate = (AgsdServerTemplate **) m_csTemplateAdmin.GetObjectSequence(&lIndex);
	}

	/*
	if ((*ppcsThisServer)->m_cType == AGSM_TYPE_LOGIN_SERVER)
	{
		AgsdServer *pcsServer = AddServer((*ppcsTemplate)->m_lTID, (*ppcsTemplate)->m_cType);
		if (!pcsServer)
			return FALSE;
	}
	else
	{
		lIndex = 0;
		ppcsTemplate = (AgsdServerTemplate **) m_csTemplateAdmin.GetObjectSequence(&lIndex);
		if (!ppcsTemplate)
			return FALSE;
		while (ppcsTemplate)
		{
			//if ((*ppcsThisServer)->m_cType == AGSM_TYPE_LOGIN_SERVER)
			//{
			//	AgsdServer *pcsServer = AddServer((*ppcsTemplate)->m_lTID, (*ppcsTemplate)->m_cType);
			//	if (!pcsServer)
			//		return FALSE;
			//}
			// 같은 서버군내에 있는 서버들 데이타를 추가한다.
			if (!strncmp((*ppcsTemplate)->m_szGroupName, m_szGroupName, AGSM_MAX_SERVER_NAME))
			{
				AgsdServer *pcsServer = AddServer((*ppcsTemplate)->m_lTID, (*ppcsTemplate)->m_cType);
				if (!pcsServer)
					return FALSE;

				if ((*ppcsTemplate)->m_cType == AGSMSERVER_TYPE_DEAL_SERVER)
				{
					m_pcsDealServer = pcsServer;
				}
				else if ((*ppcsTemplate)->m_cType == AGSMSERVER_TYPE_RECRUIT_SERVER)
				{
					m_pcsRecruitServer = pcsServer;
				}
				EnumCallback(AGSMSERVER_CB_ADD_SERVER, pcsServer, NULL);
			}
			// 로그인 서버 데이타를 추가한다.
			else if ((*ppcsTemplate)->m_cType == AGSM_TYPE_LOGIN_SERVER)
			{
				AgsdServer *pcsServer = AddServer((*ppcsTemplate)->m_lTID, (*ppcsTemplate)->m_cType);
				if (!pcsServer)
					return FALSE;
			}

			if ((*ppcsTemplate)->m_lTID != m_lServerTID)
			{
				m_nTotalConnect++;
			}

			ppcsTemplate = (AgsdServerTemplate **) m_csTemplateAdmin.GetObjectSequence(&lIndex);
		}
	}
	*/

	// Initialize Server Data

	return TRUE;
}

BOOL AgsmServerManager::ConnectionLostToLoginServer(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	AgsdServer	*pcsThisServer	= GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	switch (((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_cType) {
	case AGSMSERVER_TYPE_GAME_SERVER:
		{
			UpdateServerStatus(pcsServer, GF_SERVER_LOST_CONNECTION);

			EnumCallback(AGSMSERVER_CB_RECONNECT_SERVER, pcsServer, NULL);
		}
		break;

	case AGSMSERVER_TYPE_LOGIN_SERVER:
		{
		}
		break;

	case AGSMSERVER_TYPE_RELAY_SERVER:
		{
		}
		break;

	default:
		return TRUE;
		break;
	}

	return TRUE;
}

BOOL AgsmServerManager::ConnectionLostToGameServer(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	AgsdServer	*pcsThisServer	= GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	switch (((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_cType) {
	case AGSMSERVER_TYPE_GAME_SERVER:
		{
			UpdateServerStatus(pcsThisServer, GF_SERVER_LOST_CONNECTION);
			
			UpdateServerStatus(pcsServer, GF_SERVER_LOST_CONNECTION);

			EnumCallback(AGSMSERVER_CB_RECONNECT_SERVER, pcsServer, NULL);
		}
		break;

	case AGSMSERVER_TYPE_LOGIN_SERVER:
		{
			pcsServer->m_bIsAccepted		= FALSE;
			pcsServer->m_bIsAuth			= FALSE;
			pcsServer->m_bIsConnected		= FALSE;

			DisconnectServer(pcsServer->m_nidServer);
			DestroyClient(pcsServer->m_dpnidServer);

			UpdateServerStatus(pcsServer, GF_SERVER_LOST_CONNECTION);
		}
		break;

	case AGSMSERVER_TYPE_RELAY_SERVER:
		{
			pcsServer->m_bIsAccepted		= FALSE;
			pcsServer->m_bIsAuth			= FALSE;
			pcsServer->m_bIsConnected		= FALSE;

			DisconnectServer(pcsServer->m_nidServer);
			DestroyClient(pcsServer->m_dpnidServer);

			UpdateServerStatus(pcsServer, GF_SERVER_LOST_CONNECTION);
		}
		break;

	default:
		return TRUE;
		break;
	}

	return TRUE;
}

BOOL AgsmServerManager::ConnectionLostToRelayServer(AgsdServer *pcsServer)
{
	if (!pcsServer)
		return FALSE;

	AgsdServer	*pcsThisServer	= GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	switch (((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_cType) {
	case AGSMSERVER_TYPE_GAME_SERVER:
		{
			pcsServer->m_dpnidServer	= 0;

			UpdateServerStatus(pcsThisServer, GF_SERVER_LOST_CONNECTION);
			
			UpdateServerStatus(pcsServer, GF_SERVER_LOST_CONNECTION);

			EnumCallback(AGSMSERVER_CB_RECONNECT_SERVER, pcsServer, NULL);
		}
		break;

	case AGSMSERVER_TYPE_LOGIN_SERVER:
		{
		}
		break;

	case AGSMSERVER_TYPE_RELAY_SERVER:
		{
		}
		break;

	default:
		return TRUE;
		break;
	}

	return TRUE;
}

BOOL AgsmServerManager::OnDisconnectSvr(INT32 lServerID)
{
	//WriteLog(AS_LOG_DEBUG, "call AgsmServerManager::OnDisconnectSvr()");

	INT16	nServerStatus = GetServerStatus();
	if (nServerStatus == GF_SERVER_STOP)
		return TRUE;

	AgsdServer *pcsServer = GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	pcsServer->m_Mutex.WLock();

	switch (((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType) {
	case AGSMSERVER_TYPE_GAME_SERVER:
		{
			ConnectionLostToGameServer(pcsServer);
		}
		break;

	case AGSMSERVER_TYPE_LOGIN_SERVER:
		{
			ConnectionLostToLoginServer(pcsServer);
		}
		break;

	case AGSMSERVER_TYPE_RELAY_SERVER:
		{
			ConnectionLostToRelayServer(pcsServer);
		}
		break;

	default:
		break;
	}

	pcsServer->m_Mutex.Release();

	CHAR	szBuffer[128];
	sprintf(szBuffer, "disconnected server(%s)", ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
	WriteLog(AS_LOG_RELEASE, szBuffer);

	return TRUE;
}

BOOL AgsmServerManager::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
{
	if (!pvPacket || nSize == 0)
		return FALSE;

	if (!bReceivedFromServer)
		return FALSE;

	INT8		cOperation;
	INT32		lServerID;
	CHAR*		szServerAddress;
	INT8		cServerStatus;

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&cOperation,
						&lServerID,
						&szServerAddress,
						&cServerStatus);

	switch (cOperation) {
	case AGSMSERVER_PACKET_OPERATION_CONNECT:
		{
			AgsdServer *pcsServer = GetServer(lServerID);
			if (!pcsServer)
			{
				WriteLog(AS_LOG_DEBUG, "AgsmServerManager::OnReceive() 에서 GetServer() 에러");
				break;
			}

			if (pcsServer->m_bIsAccepted)
			{
				WriteLog(AS_LOG_RELEASE, "AgsmServerManager : this request is already accepted connection");

				Disconnect(pcsServer, AGSMSERVER_CONNECT_TYPE_ACCEPT);

				break;
			}

			pcsServer->m_Mutex.WLock();

			UpdateServerStatus(pcsServer, (eSERVERSTATUS) cServerStatus);

			pcsServer->m_dpnidServer = (DPNID) ulNID;
			pcsServer->m_bIsAccepted = TRUE;

			SetIDToPlayerContext(lServerID, ulNID);

			if (((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType == AGSMSERVER_TYPE_GAME_SERVER)
				m_pagsmAOIFilter->AddPlayerToGroup(m_dpnidGameServers, pcsServer->m_dpnidServer);

			CHAR	szBuffer[128];
			sprintf(szBuffer, "accept server(%s) connect request", ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
			WriteLog(AS_LOG_RELEASE, szBuffer);

			if (!SendConnectResultPacket(pcsServer))
			{
				WriteLog(AS_LOG_RELEASE, "AgsmServerManager::OnReceive() 에서 SendConnectResultPacket() 에러... ");

				Disconnect(pcsServer, AGSMSERVER_CONNECT_TYPE_ACCEPT);
			}

			WriteLog(AS_LOG_DEBUG, "SendConnectResultPacket()");

			if (((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType == AGSMSERVER_TYPE_GAME_SERVER)
				m_pagsmAOIFilter->AddPlayerToGroup(m_dpnidGameServers, ulNID);

			pcsServer->m_Mutex.Release();

			IsConnectComplete();
		}
		break;

	case AGSMSERVER_PACKET_OPERATION_CONNECT_RESULT:
		{
			AgsdServer *pcsServer = GetServer(lServerID);
			if (!pcsServer)
			{
				WriteLog(AS_LOG_DEBUG, "AgsmServerManager::OnReceive() 에서 GetServer() 에러 - AGSMSERVER_PACKET_OPERATION_CONNECT_RESULT");
				break;
			}

			WriteLog(AS_LOG_DEBUG, "AgsmServerManager: receive connect result");

			pcsServer->m_Mutex.WLock();

			if (!pcsServer->m_bIsConnected)
			{
				pcsServer->m_Mutex.Release();
				break;
			}

			pcsServer->m_bIsAuth = TRUE;

			UpdateServerStatus(pcsServer, (eSERVERSTATUS) cServerStatus);

			pcsServer->m_Mutex.Release();

			CHAR	szBuffer[128];
			sprintf(szBuffer, "AgsmServerManager : success connection (%s)", ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szIPv4Addr);
			WriteLog(AS_LOG_DEBUG, szBuffer);

			IsConnectComplete();
		}
		break;
		
	case AGSMSERVER_PACKET_OPERATION_DISCONNECT:
		{
		}
		break;

	case AGSMSERVER_PACKET_OPERATION_SERVER_DOWN:
		{
		}
		break;
	}

	return TRUE;
}

AgsdServer* AgsmServerManager::GetLoginServers(INT16 *pnIndex)
{
	return m_csServerAdmin.GetLoginServers(pnIndex);
}

AgsdServer* AgsmServerManager::GetDealServer()
{
	return m_pcsDealServer;
}

AgsdServer* AgsmServerManager::GetRecruitServer()
{
	return m_pcsRecruitServer;
}

AgsdServer* AgsmServerManager::GetRelayServer()
{
	return m_pcsRelayServer;
}

AgsdServer* AgsmServerManager::GetThisServer()
{
	return GetServer(m_lServerID);
}

UINT8 AgsmServerManager::GetServerType(AgsdServer *pcsServer)
{
	if (!pcsServer || !pcsServer->m_pcsTemplate)
		return AGSMSERVER_TYPE_NONE;

	return ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType;
}

UINT8 AgsmServerManager::GetThisServerType()
{
	return GetServerType(GetThisServer());
}

BOOL AgsmServerManager::DisconnectAllServers()
{
	INT32	lIndex	= 0;

	AgsdServer **ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);
	if (!ppcsServer)
		return TRUE;	// Parn님 작업, 원래 FALSE 였음.

	while (ppcsServer)
	{
		if ((*ppcsServer)->m_lServerID == m_lServerID)	// 자기 자신인 경우 넘어간다.
		{
			ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);
			continue;
		}

		Disconnect(*ppcsServer, AGSMSERVER_CONNECT_TYPE_REQUEST);
		Disconnect(*ppcsServer, AGSMSERVER_CONNECT_TYPE_ACCEPT);

		ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);
	}
	return TRUE;
}

/*
AgsdControlServer* AgsmServerManager::GetADMap(PVOID pData)
{
	return (AgsdControlServer *) m_papmMap->GetAttachedModuleData(m_nIndexADMap, pData);
}

AgsdServer* AgsmServerManager::IsNeedZoning(AuPOS posDest)
{
	ApWorldSector *pSector = (ApWorldSector *) m_papmMap->GetSector(posDest);

	return IsNeedZoning(pSector);
}

AgsdServer* AgsmServerManager::IsNeedZoning(ApWorldSector *pSector)
{
	if (!pSector)
		return NULL;

	AgsdControlServer *pControlServer = GetADMap(pSector);
	if (!pControlServer)
		return NULL;

	if (pControlServer->m_lServerID == AP_INVALID_IID)
	{
		AuPOS	posTemp;
		posTemp.x = pSector->GetXStart() + 1;
		posTemp.z = pSector->GetZStart() + 1;

		AgsdServer *pServer = GetControlServer(posTemp);

		if (!pServer)
			return NULL;

		pControlServer->m_lServerID = pServer->m_lServerID;
		pControlServer->m_pcsServer = (PVOID) pServer;

		return pServer;
	}
	else if (pControlServer->m_lServerID == m_lServerID)
	{
		return NULL;		// 조닝할 필요가 없다
	}
	else
	{
		return (AgsdServer *) pControlServer->m_pcsServer;
	}

	return NULL;
}
*/

AgsdServer* AgsmServerManager::GetGameServers(CHAR *szGroupName, INT16 *pnIndex)
{
	if (!szGroupName || !pnIndex)
		return NULL;

	AgsdServer *pcsServer = m_csServerAdmin.GetGameServers(pnIndex);
	while (pcsServer)
	{
		if (strncmp(((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szGroupName, szGroupName, AGSM_MAX_SERVER_NAME) == 0)
			return pcsServer;

		pcsServer = m_csServerAdmin.GetGameServers(pnIndex);
	}

	return NULL;
}

AgsdServer* AgsmServerManager::GetGameServers(INT16 *pnIndex)
{
	if (!pnIndex)
		return NULL;

	return m_csServerAdmin.GetGameServers(pnIndex);
}

BOOL AgsmServerManager::IsConnectComplete()
{
	INT32	lIndex = 0;

	AgsdServer **ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);
	if (!ppcsServer)
		return TRUE;	// Parn님 작업, 원래 FALSE 였음.

	BOOL	bResult = TRUE;
	while (ppcsServer)
	{
		if ((*ppcsServer)->m_lServerID == m_lServerID)	// 자기 자신인 경우 넘어간다.
		{
			ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);
			continue;
		}

		if (!(*ppcsServer)->m_bIsConnected || !(*ppcsServer)->m_bIsAccepted || !(*ppcsServer)->m_bIsAuth)
			return FALSE;

		ppcsServer = (AgsdServer **) m_csServerAdmin.GetObjectSequence(&lIndex);
	}

	if (bResult)
		SetServerStatus(GF_SERVER_START);

	return bResult;
}

DPNID AgsmServerManager::GetGameServersDPNID()
{
	return m_dpnidGameServers;
}

BOOL AgsmServerManager::SetCallbackAddServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSERVER_CB_ADD_SERVER, pfCallback, pClass);
}

BOOL AgsmServerManager::SetCallbackReconnectServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSERVER_CB_RECONNECT_SERVER, pfCallback, pClass);
}

BOOL AgsmServerManager::SetCallbackUpdateServerStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSERVER_CB_UPDATE_SERVER_STATUS, pfCallback, pClass);
}

BOOL AgsmServerManager::SendPacketGameServers(PVOID pvPacket, INT16 nSize)
{
	if (!pvPacket || !nSize)
		return FALSE;

	return m_pagsmAOIFilter->SendPacketGroup(pvPacket, nSize, GetGameServersDPNID());
}

BOOL AgsmServerManager::GetServerFlag(INT16 *pnServerFlag, INT16 *pnFlagSize)
{
	if (!pnServerFlag || !pnFlagSize)
		return FALSE;

	AgsdServer	*pcsServer = m_csServerAdmin.GetServer(m_lServerID);
	if (!pcsServer)
	{
		WriteLog(AS_LOG_RELEASE, "AgsmServerManager : 이 서버 템플릿이 존재하지 않는다... ");

		return FALSE;
	}

	((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_nServerFlag;

	*pnServerFlag	= ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_nServerFlag;
	*pnFlagSize		= ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_nFlagSize;

	return TRUE;
}

BOOL AgsmServerManager::GetItemDBIDServerFlag(INT16 *pnServerFlag, INT16 *pnFlagSize, UINT64 *pullDBID)
{
	if (!pnServerFlag || !pnFlagSize || !pullDBID)
		return FALSE;

	AgsdServer	*pcsServer = m_csServerAdmin.GetServer(m_lServerID);
	if (!pcsServer)
	{
		WriteLog(AS_LOG_RELEASE, "AgsmServerManager : 이 서버 템플릿이 존재하지 않는다... ");

		return FALSE;
	}

	((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_nServerFlag;

	GetItemDBID(&((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_ullItemDBIDStart);

	++((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_ullItemDBIDStart;

	*pnServerFlag	= ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_nItemDBIDServerFlag;
	*pnFlagSize		= ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_nItemDBIDFlagSize;
	*pullDBID		= ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_ullItemDBIDStart;

	return TRUE;
}

BOOL AgsmServerManager::GetSkillDBIDServerFlag(INT16 *pnServerFlag, INT16 *pnFlagSize, UINT64 *pullDBID)
{
	if (!pnServerFlag || !pnFlagSize || !pullDBID)
		return FALSE;

	AgsdServer	*pcsServer = m_csServerAdmin.GetServer(m_lServerID);
	if (!pcsServer)
	{
		WriteLog(AS_LOG_RELEASE, "AgsmServerManager : 이 서버 템플릿이 존재하지 않는다... ");

		return FALSE;
	}

	((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_nServerFlag;

	GetSkillDBID(&((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_ullSkillDBIDStart);

	++((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_ullSkillDBIDStart;

	*pnServerFlag	= ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_nSkillDBIDServerFlag;
	*pnFlagSize		= ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_nSkillDBIDFlagSize;
	*pullDBID		= ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_ullSkillDBIDStart;

	return TRUE;
}

BOOL AgsmServerManager::GetServerDBIDRange( INT16 nServerFlag, INT16 nServerFlagSize, UINT64 &ullMinRange, UINT64 &ullMaxRange )
{
	BOOL			bResult;

	bResult = FALSE;

	if (nServerFlagSize > 0)
	{
		UINT64	ullServerFlag	= ((UINT64) nServerFlag) << (63 - nServerFlagSize);

		UINT64	ullMask			= ((UINT64) 0x7FFFFFFFFFFFFFFF) >> nServerFlagSize;

		ullMinRange = ullServerFlag;
		ullMaxRange = ullServerFlag | ullMask;

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgsmServerManager::GetItemDBID(UINT64 *pllDBID)
{
	if (!pllDBID)
		return FALSE;

	ResetEvent(m_hGetItemDBIDEvent);

	if (!StreamSelectItemDBID())
		return FALSE;

    DWORD	dwResult = WaitForSingleObject(m_hGetItemDBIDEvent, 20000);

	if (dwResult == WAIT_TIMEOUT ||
		dwResult == WAIT_FAILED)
		return FALSE;

	return TRUE;
}

BOOL AgsmServerManager::GetSkillDBID(UINT64 *pllDBID)
{
	if (!pllDBID)
		return FALSE;

	ResetEvent(m_hGetSkillDBIDEvent);

	if (!StreamSelectSkillDBID())
		return FALSE;

    DWORD	dwResult = WaitForSingleObject(m_hGetSkillDBIDEvent, 20000);

	if (dwResult == WAIT_TIMEOUT ||
		dwResult == WAIT_FAILED)
		return FALSE;

	return TRUE;
}

BOOL AgsmServerManager::StreamSelectItemDBID()
{
	BOOL	bResult	= FALSE;

	AgsdServer	*pcsThisServer	= GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	pstAgsmDBOperationArg	pstArg = (pstAgsmDBOperationArg) GlobalAlloc(GMEM_FIXED, sizeof(stAgsmDBOperationArg));
	if (!pstArg)	return bResult;

	ZeroMemory(pstArg, sizeof(stAgsmDBOperationArg));
	
	pstArg->nOperation	= AGSMDB_OPERATION_SELECT;
	pstArg->nDataType	= AGSMDB_DATATYPE_MAXITEMID;
	pstArg->lID			= AP_INVALID_CID;
	pstArg->pfCallback	= CBStreamDB;
	pstArg->pClass		= this;

	UINT64	ullMinDBID	= 0;
	UINT64	ullMaxDBID	= 0;

	if (!GetServerDBIDRange(((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_nItemDBIDServerFlag, 
							((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_nItemDBIDFlagSize,
							ullMinDBID,
							ullMaxDBID))
	{
		GlobalFree(pstArg);
		return FALSE;
	}

	sprintf(pstArg->szQuery, "select max(itemseq) from charitem where itemseq >= %I64d AND itemseq < %I64d order by itemseq desc", ullMinDBID, ullMaxDBID);

	bResult = m_pagsmDBStream->ExecuteQuery(pstArg, __FILE__, __LINE__ );

	if (!bResult)
	{
		GlobalFree(pstArg);
		return FALSE;
	}

	pstAgsmDBOperationArg	pstArg2 = (pstAgsmDBOperationArg) GlobalAlloc(GMEM_FIXED, sizeof(stAgsmDBOperationArg));
	if (!pstArg2)	return bResult;

	ZeroMemory(pstArg2, sizeof(stAgsmDBOperationArg));
	
	pstArg2->nOperation	= AGSMDB_OPERATION_SELECT;
	pstArg2->nDataType	= AGSMDB_DATATYPE_MAXITEMID;
	pstArg2->lID		= AP_INVALID_CID;
	pstArg2->pfCallback	= CBStreamDB;
	pstArg2->pClass		= this;

	sprintf(pstArg2->szQuery, "select max(itemseq) from bakcharitem where itemseq >= %I64d AND itemseq < %I64d order by itemseq desc", ullMinDBID, ullMaxDBID);

	bResult = m_pagsmDBStream->ExecuteQuery(pstArg2, __FILE__, __LINE__ );

	if (!bResult)
	{
		GlobalFree(pstArg2);
		return FALSE;
	}

	return bResult;
}

BOOL AgsmServerManager::StreamSelectSkillDBID()
{
	BOOL	bResult	= FALSE;

	AgsdServer	*pcsThisServer	= GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	pstAgsmDBOperationArg	pstArg = (pstAgsmDBOperationArg) GlobalAlloc(GMEM_FIXED, sizeof(stAgsmDBOperationArg));
	if (!pstArg)	return bResult;

	ZeroMemory(pstArg, sizeof(stAgsmDBOperationArg));
	
	pstArg->nOperation	= AGSMDB_OPERATION_SELECT;
	pstArg->nDataType	= AGSMDB_DATATYPE_MAXSKILLID;
	pstArg->lID			= AP_INVALID_CID;
	pstArg->pfCallback	= CBStreamDB;
	pstArg->pClass		= this;

	UINT64	ullMinDBID	= 0;
	UINT64	ullMaxDBID	= 0;

	if (!GetServerDBIDRange(((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_nSkillDBIDServerFlag, 
							((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_nSkillDBIDFlagSize,
							ullMinDBID,
							ullMaxDBID))
	{
		GlobalFree(pstArg);
		return FALSE;
	}

	sprintf(pstArg->szQuery, "select max(id) from skill where id >= %I64d AND id < %I64d order by id desc", ullMinDBID, ullMaxDBID);

	bResult = m_pagsmDBStream->ExecuteQuery(pstArg, __FILE__, __LINE__ );

	if (!bResult)
	{
		GlobalFree(pstArg);
		return FALSE;
	}

	return bResult;
}

BOOL AgsmServerManager::CBStreamDB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmServerManager		*pThis			= (AgsmServerManager *)		pClass;
	COLEDB					*pcOLEDB		= (COLEDB *)				pData;
	pstAgsmDBOperationArg	pstOperationArg	= (pstAgsmDBOperationArg)	pCustData;

	//if (!pstOperationArg->bSuccess)
	//	return FALSE;

	switch (pstOperationArg->nDataType) {
	case AGSMDB_DATATYPE_MAXITEMID:
		{
			AgsdServer	*pcsThisServer			= pThis->GetThisServer();

			UINT64		ullMaxDBID				= _atoi64(pcOLEDB->GetQueryResult(0));

			if (((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_ullItemDBIDStart < ullMaxDBID)
				((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_ullItemDBIDStart	= ullMaxDBID;

			++g_lNumGetDBID;

			if (g_lNumGetDBID > 1)
				SetEvent(pThis->m_hGetItemDBIDEvent);
		}
		break;

	case AGSMDB_DATATYPE_MAXSKILLID:
		{
			AgsdServer	*pcsThisServer			= pThis->GetThisServer();

			UINT64		ullMaxDBID				= _atoi64(pcOLEDB->GetQueryResult(0));

			if (((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_ullSkillDBIDStart < ullMaxDBID)
				((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_ullSkillDBIDStart	= ullMaxDBID;

			SetEvent(pThis->m_hGetSkillDBIDEvent);
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL AgsmServerManager::UpdateServerStatus(AgsdServer *pcsServer, eSERVERSTATUS eNewStatus)
{
	if (!pcsServer)
		return FALSE;

	INT16	nOldStatus	= pcsServer->m_nStatus;

	pcsServer->m_nStatus = (INT16) eNewStatus;

	return EnumCallback(AGSMSERVER_CB_UPDATE_SERVER_STATUS, pcsServer, &nOldStatus);
}