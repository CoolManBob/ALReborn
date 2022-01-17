// AgsmAdmin.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 04.

#include "AgsmAdmin.h"
#include <list>
#include "ApMemoryTracker.h"
#include "AgppAdmin.h"
#include "AgsmTitle.h"
#include "AuGameEnv.h"

#define AGSMADMIN_MAX_CURRENT_USER			30000		// 한서버 최대 접속자 임시.
#define AGSMADMIN_EVENT_ITEM_FILE_NAME		_T("ini\\EventItems.ini")

INT32 AgsmAdmin::s_lCharTID[AGSMADMIN_CHARTID_MAX] = 
{
	96,
	1,
	6,
	4,
	3,
	8
};

AgsmAdmin::AgsmAdmin():m_papmMap( NULL )
{
	SetModuleName("AgsmAdmin");

	SetModuleType(APMODULE_TYPE_SERVER);
	
	EnableIdle(TRUE);

	// 패킷 설정
	SetPacketType(AGSMADMIN_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operator
							AUTYPE_INT32,			1,	// Admin CID
							AUTYPE_INT32,			1,	// Connectable Account Level
							AUTYPE_PACKET,			1,	// Help Packet
							AUTYPE_END,				0
							);

	m_pagpmLog = NULL;

	m_papmEventManager = NULL;
	m_pagpmFactors = NULL;
	m_pagpmCharacter = NULL;
	m_pagpmChatting = NULL;
	m_pagpmItem = NULL;
	m_pagpmGrid = NULL;
	m_pagpmItemConvert = NULL;
	m_pagpmParty = NULL;
	m_pagpmGuild = NULL;
	//m_pagpmItemLog = NULL;
	m_pagpmSkill = NULL;
	m_pagpmEventSkillMaster = NULL;
	m_pagpmEventSpawn = NULL;
	m_pagpmAdmin = NULL;
	//m_pagpmScript = NULL;
	m_pagpmCashMall = NULL;
	m_pagpmProduct = NULL;
	m_pagpmSiegeWar = NULL;
	m_pagpmSummons = NULL;
	m_pagpmWantedCriminal = NULL;
	m_pagpmMailBox = NULL;

	m_pagsmAOIFilter = NULL;
	m_pagsmCharacter = NULL;
	m_pagsmCharManager = NULL;
	m_pagsmChatting = NULL;
	m_pagsmFactors = NULL;
	m_pagsmItem = NULL;
	m_pagsmItemManager = NULL;
	m_pagsmSkill = NULL;
	m_pagsmEventSkillMaster = NULL;
	m_pagsmParty = NULL;
	m_pagsmDeath = NULL;
	m_pagsmEventSpawn = NULL;
	m_pagsmGuild = NULL;
	m_pagsmRide	= NULL;
	m_pagsmSummons = NULL;
	m_pagsmWantedCriminal = NULL;
	m_pagsmEventNPCTrade = NULL;
	m_pagsmEventBank = NULL;
	m_pagpmBattleGround = NULL;
	m_pagsmBattleGround = NULL;
	
	m_pAgsmServerManager = NULL;
	m_pAgsmInterServerLink = NULL;

	m_pagpmConfig = NULL;
	m_pagsmConfig = NULL;
	m_pagsmNpcManager = NULL;

	m_pagpmPvP = NULL;
	m_pagsmAreaChatting = NULL;
	m_pagpmBuddy = NULL;

	m_pagpmEpicZone = NULL;
	m_pagsmEpicZone = NULL;

	m_pagpmTitle = NULL;
	m_pagsmTitle = NULL;

	memset(&m_stTotalUser, 0, sizeof(m_stTotalUser));
	m_dwLastWriteLogCurrentUserTickCount = 0;
	m_lThisServerUserCount = 0;
	
	m_dwLastWriteLogCurrentUserTickCountForExcel = 0;
	m_lLastMinuteWriteLogCurrentUserForExcel = -1;

	m_ulLastSendPingClient = 0;

	m_lConnectableAccountLevel = 0;
}

AgsmAdmin::~AgsmAdmin()
{
}

BOOL AgsmAdmin::OnAddModule()
{
	m_pagpmLog = (AgpmLog*)GetModule("AgpmLog");

	m_papmMap = (ApmMap *) GetModule("ApmMap");
	m_papmEventManager = (ApmEventManager*)GetModule("ApmEventManager");
	m_pagpmFactors = (AgpmFactors*)GetModule("AgpmFactors");
	m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagpmChatting = (AgpmChatting*)GetModule("AgpmChatting");
	m_pagpmItem = (AgpmItem*)GetModule("AgpmItem");
	m_pagpmGrid = (AgpmGrid*)GetModule("AgpmGrid");
	m_pagpmItemConvert = (AgpmItemConvert*)GetModule("AgpmItemConvert");
	m_pagpmParty = (AgpmParty*)GetModule("AgpmParty");
	m_pagpmGuild = (AgpmGuild*)GetModule("AgpmGuild");
	//m_pagpmItemLog = (AgpmItemLog*)GetModule("AgpmItemLog");
	m_pagpmSkill = (AgpmSkill*)GetModule("AgpmSkill");
	m_pagpmEventSkillMaster = (AgpmEventSkillMaster*)GetModule("AgpmEventSkillMaster");
	m_pagpmEventSpawn = (AgpmEventSpawn*)GetModule("AgpmEventSpawn");
	m_pagpmAdmin = (AgpmAdmin*)GetModule("AgpmAdmin");
	m_pagpmAI2 = (AgpmAI2*)GetModule("AgpmAI2");
	m_pagpmScript = (AgpmScript*)GetModule("AgpmScript");
	m_pagpmCashMall = (AgpmCashMall*)GetModule("AgpmCashMall");
	m_pagpmProduct = (AgpmProduct*)GetModule("AgpmProduct");
	m_pagpmSiegeWar = (AgpmSiegeWar*)GetModule("AgpmSiegeWar");
	m_pagpmSummons = (AgpmSummons*)GetModule("AgpmSummons");
	m_pagpmWantedCriminal = (AgpmWantedCriminal*)GetModule("AgpmWantedCriminal");
	m_pagpmMailBox = (AgpmMailBox*)GetModule("AgpmMailBox");

	m_pagsmAOIFilter = (AgsmAOIFilter*)GetModule("AgsmAOIFilter");
	m_pagsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pagsmCharManager = (AgsmCharManager*)GetModule("AgsmCharManager");
	m_pagsmChatting = (AgsmChatting*)GetModule("AgsmChatting");
	m_pagsmFactors = (AgsmFactors*)GetModule("AgsmFactors");
	m_pagsmItem = (AgsmItem*)GetModule("AgsmItem");
	m_pagsmItemManager = (AgsmItemManager*)GetModule("AgsmItemManager");
	m_pagsmSkill = (AgsmSkill*)GetModule("AgsmSkill");
	m_pagsmEventSkillMaster = (AgsmEventSkillMaster*)GetModule("AgsmEventSkillMaster");
	m_pagsmParty = (AgsmParty*)GetModule("AgsmParty");
	m_pagsmDeath = (AgsmDeath*)GetModule("AgsmDeath");
	m_pagsmEventSpawn = (AgsmEventSpawn*)GetModule("AgsmEventSpawn");
	m_pagsmGuild = (AgsmGuild*)GetModule("AgsmGuild");
	m_pagsmRide = (AgsmRide*)GetModule("AgsmRide");
	m_pagsmQuest = (AgsmQuest*)GetModule("AgsmQuest");
	m_pagsmSummons = (AgsmSummons*)GetModule("AgsmSummons");
	m_pagsmRemission = (AgsmRemission*)GetModule("AgsmRemission");
	m_pagsmSystemMessage = (AgsmSystemMessage*)GetModule("AgsmSystemMessage");
	m_pagsmWantedCriminal = (AgsmWantedCriminal*)GetModule("AgsmWantedCriminal");
	m_pagsmEventNPCTrade = (AgsmEventNPCTrade*)GetModule("AgsmEventNPCTrade");
	m_pagsmEventBank = (AgsmEventBank*)GetModule("AgsmEventBank");

	m_pAgsmServerManager = (AgsmServerManager*)GetModule("AgsmServerManager2");
	m_pAgsmInterServerLink = (AgsmInterServerLink*)GetModule("AgsmInterServerLink");

	m_pagpmConfig = (AgpmConfig *)GetModule("AgpmConfig");

	//m_csCurrentUserAdmin.InitializeObject(sizeof(stAgsdAdminCurrentUser*), AGSMADMIN_MAX_CURRENT_USER);

	//if(!m_pagpmFactors ||
	//	!m_pagpmCharacter ||
	//	!m_pagpmChatting ||
	//	!m_pagpmItem ||
	//	!m_pagpmGrid ||
	//	!m_pagpmItemConvert ||
	//	!m_pagpmParty ||
	//	!m_pagpmGuild ||
	//	!m_pagpmItemLog ||
	//	!m_pagpmSkill ||
	//	!m_pagpmEventSkillMaster ||
	//	!m_pagpmAdmin ||
	//	!m_pagsmAOIFilter ||
	//	!m_pagsmCharacter ||
	//	!m_pagsmCharManager ||
	//	!m_pagsmChatting ||
	//	!m_pagsmFactors ||
	//	!m_pagsmItem ||
	//	!m_pagsmItemManager ||
	//	!m_pagsmSkill ||
	//	!m_pagsmEventSkillMaster ||
	//	!m_pagsmParty ||
	//	!m_pagsmDeath ||
	//	!m_pagsmGuild ||
	//	!m_pAgsmServerManager ||
	//	!m_pagsmAdminDB)
	//	return FALSE;

	// 다른 건 몰라도 얘는 있어야 겠다. -_-
	if(!m_pagpmAdmin || !m_pagpmCharacter)
		return FALSE;

	// CallBack 등록
	m_pagpmAdmin->SetCallbackAddAdmin(CBAddAdmin, this);	// Admin 접속 Callback 을 AgpmAdmin 에 등록으로 변경. 2004.03.05
	m_pagpmAdmin->SetCallbackRemoveAdmin(CBRemoveAdmin, this);	// Admin 로그아웃 Callback 을 AgpmAdmin 에도 추가. 2004.03.08
	m_pagpmAdmin->SetCallbackConnect(CBConnect, this);
	
	//m_pagpmAdmin->SetCallbackAddCharacter(CBAddCharacter, this);
	//m_pagsmCharManager->SetCallbackEnterGameWorld(CBAddCharacter, this);	// 콜백 이제 일루 변경. 초간단모드로. 2006.06.04.
	m_pagpmAdmin->SetCallbackRemoveCharacter(CBRemoveCharacter, this);
	m_pagpmAdmin->SetCallbackNumCurrentUser(CBNumCurrentUser, this);
	m_pagpmAdmin->SetCallbackUserPosition(CBUserPosition, this);

	//m_pagpmChatting->SetCallbackRecvNormalMsg(CBRecvNormalMsg, this);

	m_pagpmAdmin->SetCallbackSearchCharacter(CBSearchCharacter, this);
	m_pagpmAdmin->SetCallbackCharItem(CBSearchCharItem, this);
	m_pagpmAdmin->SetCallbackCharSkill(CBSearchCharSkill, this);
	//m_pagpmAdmin->SetCallbackCharParty(CBSearchCharParty, this);
	m_pagpmAdmin->SetCallbackCharMove(CBCharMove, this);
	m_pagpmAdmin->SetCallbackCharEdit(CBCharEdit, this);
	m_pagpmAdmin->SetCallbackBan(CBBan, this);

	m_pagpmAdmin->SetCallbackGuild(CBGuildOperation, this);

	m_pagpmAdmin->SetCallbackItemCreate(CBItemOperation, this);

	m_pagpmAdmin->SetCallbackPing(CBPingOperation, this);	

	m_pagpmAdmin->SetCallbackAdminClientLogin(CBOnAdminClientLogin, this);

	m_pagpmAdmin->SetCallbackTitleEdit(CBOnTitleEdit, this);

	if(m_pagsmCharManager)
	{
		m_pagsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this);
		m_pagsmCharManager->SetCallbackDisconnectCharacter(CBDisconnectAdmin, this);
	}

	if(m_pagsmChatting)
	{
		m_pagsmChatting->SetCallbackCheckAdminCommand(CBCheckAdminCommand, this);
		m_pagsmChatting->SetCallbackEventMessage(CBReceiveEventMessage, this);
	}

	if(m_pAgsmInterServerLink)
	{
		m_pAgsmInterServerLink->SetCallbackConnect(CBOnConnectServer, this);
		m_pAgsmInterServerLink->SetCallbackDisconnect(CBOnDisconnectServer, this);
	}

#ifndef _WIN64	// disable on win64
	if (m_pagpmScript)
	{
		m_pagpmScript->SetCallbackIsAdmin(CBIsAdminCharacter, this);
	}
#endif

	return TRUE;
}

BOOL AgsmAdmin::OnInit()
{
	m_pagpmBattleGround = (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagsmBattleGround = (AgsmBattleGround*)GetModule("AgsmBattleGround");
	m_pagsmConfig = (AgsmConfig*)GetModule("AgsmConfig");
	m_pagsmNpcManager = (AgsmNpcManager*)GetModule("AgsmNpcManager");
	m_pagpmBuddy = (AgpmBuddy*)GetModule("AgpmBuddy");

	m_pagpmPvP = (AgpmPvP*)GetModule("AgpmPvP");
	m_pagsmAreaChatting = (AgsmAreaChatting *)GetModule("AgsmAreaChatting");

	m_pagpmEpicZone = (AgpmEpicZone*)GetModule("AgpmEpicZone");
	m_pagsmEpicZone = (AgsmEpicZone*)GetModule("AgsmEpicZone");

	m_pagpmTitle = (AgpmTitle *)GetModule("AgpmTitle");
	m_pagsmTitle = (AgsmTitle *)GetModule("AgsmTitle");

	InitCommandString();

	return TRUE;
}

BOOL AgsmAdmin::OnIdle(UINT32 ulClockCount)
{
	//STOPWATCH2(GetModuleName(), _T("OnIdle"));

	// Write Log Current User Count
	WriteLogCurrentUserCount();

	// Write Log Current User Count For Excel - 2004.05.24.
	WriteLogCurrentUserCountForExcel();

	// Ping To Relay
	ProcessPingRelaySend();

	// Ping to Client
	ProcessPingClient(ulClockCount);

	// Notice Repeat
	ProcessNoticeRepeat();

	return TRUE;
}

BOOL AgsmAdmin::OnDestroy()
{
	//stAgsdAdminCurrentUser** ppcsCurrentUser = NULL;
	//INT32 lIndex = 0;
	//for(ppcsCurrentUser = (stAgsdAdminCurrentUser**)m_csCurrentUserAdmin.GetObjectSequence(&lIndex); ppcsCurrentUser;
	//	ppcsCurrentUser = (stAgsdAdminCurrentUser**)m_csCurrentUserAdmin.GetObjectSequence(&lIndex))
	//{
	//	if(!*ppcsCurrentUser)
	//		break;

	//	delete *ppcsCurrentUser;
	//}
	//m_csCurrentUserAdmin.RemoveObjectAll();

	return TRUE;
}

BOOL AgsmAdmin::InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag)
{
	//m_csGenerateHelpCount.Initialize(ulStartValue, ulServerFlag, nSizeServerFlag);

	return TRUE;
}













//////////////////////////////////////////////////////////////////////////
// Get Server Properties
INT32 AgsmAdmin::GetThisServerID()
{
	if(!m_pAgsmServerManager)
		return 0;

	AgsdServer* pcsThisServer = m_pAgsmServerManager->GetThisServer();
	if(!pcsThisServer)
		return 0;

	return pcsThisServer->m_lServerID;
}

INT32 AgsmAdmin::GetRelayServerID()
{
	if(!m_pAgsmServerManager)
		return 0;

	AgsdServer* pcsRelayServer = m_pAgsmServerManager->GetRelayServer();
	if(!pcsRelayServer)
		return 0;

	return pcsRelayServer->m_lServerID;
}

UINT32 AgsmAdmin::GetGameServerNID(INT32 nGameServerID)
{
	if(!m_pAgsmServerManager)
		return 0;

	AgsdServer* pcsServer = m_pAgsmServerManager->GetServer(nGameServerID);
	if(!pcsServer)
		return 0;

	return pcsServer->m_dpnidServer;
}

UINT32 AgsmAdmin::GetRelayServerNID()
{
	if(!m_pAgsmServerManager)
		return 0;

	AgsdServer* pcsRelayServer = m_pAgsmServerManager->GetRelayServer();
	if(!pcsRelayServer)
		return 0;

	return pcsRelayServer->m_dpnidServer;
}

UINT32 AgsmAdmin::GetLoginServerNID(INT16 nIndex)
{
	if(!m_pAgsmServerManager)
		return 0;

	UINT32 ulNID = 0;
	nIndex = 0;

	AgsdServer* pcsThisServer = m_pAgsmServerManager->GetThisServer();
	if(!pcsThisServer)
		return ulNID;

	for(AgsdServer* pcsLoginServer = m_pAgsmServerManager->GetLoginServers(&nIndex); pcsLoginServer;
			pcsLoginServer = m_pAgsmServerManager->GetLoginServers(&nIndex))
	{
		if(!pcsLoginServer)
			break;

		/*
		if(strcmp(pcsLoginServer->m_szGroup, pcsThisServer->m_szGroup) == 0)
		{
			ulNID = pcsLoginServer->m_dpnidServer;
			break;
		}
		*/
	}

	return ulNID;
}

BOOL AgsmAdmin::IsGameServer()
{
	if(!m_pAgsmServerManager)
		return FALSE;

	if(m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
		return FALSE;

	//if(((AgsdServerTemplate*)pcsThisServer->m_pcsTemplate)->m_cType != AGSMSERVER_TYPE_GAME_SERVER)
	//	return FALSE;
		
	return TRUE;
}

BOOL AgsmAdmin::IsRelayServer()
{
	if(!m_pAgsmServerManager)
		return FALSE;

	if(m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_RELAY_SERVER)
		return FALSE;

	//if(((AgsdServerTemplate*)pcsThisServer->m_pcsTemplate)->m_cType != AGSMSERVER_TYPE_RELAY_SERVER)
	//	return FALSE;

	return TRUE;
}

BOOL AgsmAdmin::IsThisServerCharacter(AgpdCharacter* pcsAgpdCharacter)
{
	if(!pcsAgpdCharacter)
		return FALSE;

	if(!m_pagsmCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	if(pcsAgsdCharacter->m_ulServerID == GetThisServerID())
		return TRUE;
	else
		return FALSE;
}

UINT32 AgsmAdmin::GetCharacterNID(AgpdCharacter* pcsAgpdCharacter)
{
	if(!pcsAgpdCharacter)
		return FALSE;

	if(!m_pagsmCharacter)
		return 0;

	return (UINT32)m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);
}

INT16 AgsmAdmin::GetCharacterServerID(AgpdCharacter* pcsAgpdCharacter)
{
	if(!pcsAgpdCharacter)
		return FALSE;

	if(!m_pagsmCharacter)
		return 0;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	return pcsAgsdCharacter->m_ulServerID;
}

INT32 AgsmAdmin::GetNumCurrentUser()
{
	INT32 lCount = 0;

	UserLock();
	lCount = m_stTotalUser.m_lTotalUserCount;
	UserUnlock();

	return lCount;
}

void AgsmAdmin::UserLock()
{
	m_csUserLock.WLock();
}

void AgsmAdmin::UserUnlock()
{
	m_csUserLock.Release();
}







////////////////////////////////////////////////////////////////////////////////////////
// Operation

// Callback 이 AgsmCharacter 에서 불리다 보니 FALSE 로 리턴하면 안된다.
//
// 2004.03.04. Callback 을 AgsmCharManager 에서 호출하기로 변경. (기존은 AgsmCharacter)
//
// 2004.03.05. Callback 을 AgpmAdmin 에서만 호출하기로 다시 변경
// 별다른 작업은 하지 않고, 새로운 Admin 의 접속을 Admin 들에게 알린다.
BOOL AgsmAdmin::AddAdmin(AgpdCharacter* pcsAgpdCharacter)
{
	if(/*!m_pagsmAdminDB || */!m_pagsmCharacter)
		return FALSE;

	// Admin Character 가 아니라면 여기서 나간다. 2004.03.04
	if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
		return TRUE;
	
	AgpdAdmin* pcsAttachedAdmin = m_pagpmAdmin->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAttachedAdmin)
		return TRUE;

	// Relay 서버라면 Private Help List 로딩 작업을 한다.
	if(IsRelayServer())
	{
		return TRUE;
	}

	stAgpdAdminInfo stAdminInfo;
	ZeroMemory(&stAdminInfo, sizeof(stAdminInfo));
	
	stAdminInfo.m_lCID = pcsAgpdCharacter->m_lID;
	stAdminInfo.m_lAdminLevel = pcsAttachedAdmin->m_lAdminLevel;
	strncpy(stAdminInfo.m_szAdminName, pcsAgpdCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
	if(pcsAgsdCharacter)
	{
		strncpy(stAdminInfo.m_szAccName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
	}
	
	// 새로운 Admin 에게 기존에 접속해 있는 Admin List 를 보낸다.
	// 2004.03.08. 여기서 이걸 보낼 필요는 없다.
	//SendAdminList(pcsAgpdCharacter);

	// 다른 Admin 들에게 새로운 Admin 의 접속을 알린다.
	SendAddAdminToAllAdmin(&stAdminInfo, pcsAgpdCharacter);
	
	return TRUE;
}

// AgsmCharManager::EnterGameWorld(...) 에서 호출된다.
// AddAdmin 과 다른 점은, AddAdmin 은 AgpmAdmin 에서 패킷을 받으면 호출되지만,
// EnterGameWorld 가 된 시점에서 호출된다. 고로 List 에 Add 같은 건 하지 않고, 걍 해당 캐릭터에 Admin 정보만 보낸다.
BOOL AgsmAdmin::EnterGameWorld(AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	// NID 가 0 이 아닐때만 한다.
	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgsdCharacter || pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	AgpdAdmin* pcsAttachedAdmin = m_pagpmAdmin->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAttachedAdmin)
		return TRUE;

	// PC 일 경우 동접올려주고 LoginCheck 1 로 해준다.
	if(m_pagpmCharacter->IsPC(pcsAgpdCharacter) && pcsAttachedAdmin->m_lLoginCheck == 0)
	{
		UserLock();
		++m_stTotalUser.m_lTotalUserCount;
		++m_lThisServerUserCount;
		UserUnlock();

		pcsAttachedAdmin->m_lLoginCheck = 1;
	}

	// Admin Character 가 아니라면 여기서 나간다. 2004.03.04
	if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
		return TRUE;
	
	stAgpdAdminInfo stAdminInfo;
	ZeroMemory(&stAdminInfo, sizeof(stAdminInfo));
	
	stAdminInfo.m_lCID = pcsAgpdCharacter->m_lID;
	stAdminInfo.m_lAdminLevel = pcsAttachedAdmin->m_lAdminLevel;
	strncpy(stAdminInfo.m_szAdminName, pcsAgpdCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);

	if(pcsAgsdCharacter)
	{
		strncpy(stAdminInfo.m_szAccName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
	}
	
	// 새로운 Admin 에게 기존에 접속해 있는 Admin List 를 보낸다.
	SendAdminList(pcsAgpdCharacter);

	// 다른 Admin 들에게 새로운 Admin 의 접속을 알린다.
	SendAddAdminToAllAdmin(&stAdminInfo, pcsAgpdCharacter);

	// 새로 접속한 Admin 에게 ServerInfo 를 보낸다. - 2004.03.09.
	ProcessSendServerInfo(pcsAgpdCharacter);
	
	return TRUE;
}

// AgpmAdmin 에서 불린 것이다. AgpmAdmin 의 Admin List 에서는 이미 빠져있다.
BOOL AgsmAdmin::RemoveAdmin(INT32 lRemoveCID)
{
	if(!lRemoveCID)
		return FALSE;

	stAgpdAdminInfo stAdminInfo;
	ZeroMemory(&stAdminInfo, sizeof(stAdminInfo));
	
	stAdminInfo.m_lCID = lRemoveCID;

	// 다른 Admin 들에게 새로운 Admin 의 로그아웃을 알린다.
	SendRemoveAdminToAllAdmin(&stAdminInfo, NULL);

	return TRUE;
}

// 실패한다 쳐도 그냥 Skip 으로 처리한다. return TRUE
// 2004.03.08. AgsmCharManager::DisconnectCharacter 에서 불림.
BOOL AgsmAdmin::DisconnectAdmin(AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	AgpdAdmin* pcsAttachedAdmin = m_pagpmAdmin->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAttachedAdmin)
		return TRUE;

	// Admin 이 아니면 나간다.
	if(pcsAttachedAdmin->m_lAdminLevel < AGPMADMIN_LEVEL_ADMIN)
		return TRUE;

	stAgpdAdminInfo stAdminInfo;
	ZeroMemory(&stAdminInfo, sizeof(stAdminInfo));
	
	stAdminInfo.m_lCID = pcsAgpdCharacter->m_lID;

	// AgpmAdmin 의 List 를 비운다.
	m_pagpmAdmin->OnOperationRemoveAdmin(pcsAgpdCharacter, pcsAgpdCharacter->m_lID);

	// 다른 Admin 들에게 새로운 Admin 의 로그아웃을 알린다.
	SendRemoveAdminToAllAdmin(&stAdminInfo, pcsAgpdCharacter);
	return TRUE;
}

// 2004.03.10.
// 다른 서버에 접속한 Admin 이다.
// 현재 서버에서 관리하는 Character 가 아니라면 NID 를 세팅해준다.
BOOL AgsmAdmin::Connect(AgpdCharacter* pcsAgpdCharacter, UINT32 ulNID)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pcsAgpdCharacter || !ulNID)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// 현재 서버에서 컨트롤 하는 놈이 아니라면 NID 세팅
	if(pcsAgsdCharacter->m_ulServerID != GetThisServerID())
	{
		if(pcsAgsdCharacter->m_dpnidCharacter == 0)
			pcsAgsdCharacter->m_dpnidCharacter = ulNID;
	}

	// 서버 컨트롤에 상관없이 Attached 에다가 세팅
	AgpdAdmin* pcsAgpdAdmin = m_pagpmAdmin->GetADCharacter(pcsAgpdCharacter);
	if(pcsAgpdAdmin)
		pcsAgpdAdmin->m_ulNID = ulNID;
	
	return TRUE;
}

// 2006.06.02. steeple
// 여기선 아무것도 하지 않는다.
//
// 2004.03.25.
// 접속자 수 관리를 위해서
// AgsmCharacter 에서 AGSMCHAR_PACKET_OPERATION_UPDATE 패킷을 받았을 때 불린다.
BOOL AgsmAdmin::AddCharacter(AgpdCharacter* pcsAgpdCharacter)
{
	return TRUE;
}

// 2004.03.25.
// 접속자 수 관리를 위해서
BOOL AgsmAdmin::RemoveCharacter(AgpdCharacter* pcsAgpdCharacter)
{
	if(!pcsAgpdCharacter)
		return FALSE;

	// 20006.06.04. steeple
	// 동접관리 간단하게 변경
	AgpdAdmin* pcsAttachedAdmin = m_pagpmAdmin->GetADCharacter(pcsAgpdCharacter);
	if(pcsAttachedAdmin && m_pagpmCharacter->IsPC(pcsAgpdCharacter) && pcsAttachedAdmin->m_lLoginCheck == 1)
	{
		UserLock();
		--m_stTotalUser.m_lTotalUserCount;
		--m_lThisServerUserCount;
		UserUnlock();

		pcsAttachedAdmin->m_lLoginCheck = 0;
	}

	return TRUE;
}









































// 해당 Admin 에게 모든 Server Info 를 보낸다. - 2004.03.09
BOOL AgsmAdmin::ProcessSendServerInfo(AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagsmCharacter || !m_pAgsmServerManager)
		return FALSE;

	if(!pcsAgpdCharacter)
		return FALSE;

	stAgpdAdminServerInfo stServerInfo;
	AgsdServer* pcsAgsdServer = NULL;

	UINT32 ulNID = (UINT32)m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);

	// 먼저 게임 서버 정보
	INT16 lIndex = 0;
	while((pcsAgsdServer = m_pAgsmServerManager->GetGameServers(&lIndex)) != NULL)
	{
		memset(&stServerInfo, 0, sizeof(stServerInfo));

		// IP 와 Port 다 얻어온다.
		GetIPAddressFromAgsdServer(&stServerInfo, pcsAgsdServer);
		
		SendServerInfo(&stServerInfo, pcsAgpdCharacter->m_lID, ulNID);
	}

	// Relay Server 에도 보낸다.
	pcsAgsdServer = m_pAgsmServerManager->GetRelayServer();
	if(pcsAgsdServer)
	{
		memset(&stServerInfo, 0, sizeof(stServerInfo));
		GetIPAddressFromAgsdServer(&stServerInfo, pcsAgsdServer);
		SendServerInfo(&stServerInfo, pcsAgpdCharacter->m_lID, ulNID);
	}

	return TRUE;
}

BOOL AgsmAdmin::GetIPAddressFromAgsdServer(stAgpdAdminServerInfo* pstServerInfo, AgsdServer* pcsAgsdServer)
{
	if(!pstServerInfo || !pcsAgsdServer)
		return FALSE;

	//AgsdServerTemplate* pcsServerTemplate = (AgsdServerTemplate*)pcsAgsdServer->m_pcsTemplate;
	//if(!pcsServerTemplate)
	//	return FALSE;

	BOOL bFound = FALSE;
	INT16 nIndex = 0;

	for(nIndex; nIndex < (INT16)strlen(pcsAgsdServer->m_szIP); nIndex++)
	{
		if(pcsAgsdServer->m_szIP[nIndex] == ':')
		{
			bFound = TRUE;
			break;
		}
	}

	if(bFound)
	{
		if(nIndex <= AGPMADMIN_MAX_IP_STRING)
		{
			memcpy(pstServerInfo->m_szServerIP, pcsAgsdServer->m_szIP, nIndex);
			pstServerInfo->m_szServerIP[nIndex] = '\0';

			if(nIndex < (INT16)strlen(pcsAgsdServer->m_szIP))
				pstServerInfo->m_lPort = (INT16)atoi(&pcsAgsdServer->m_szIP[nIndex+1]);
		}
		else
			bFound = FALSE;
	}

	/*
	for(nIndex; nIndex < strlen(pcsServerTemplate->m_szIPv4Addr); nIndex++)
	{
		if(pcsServerTemplate->m_szIPv4Addr[nIndex] == ':')
		{
			bFound = TRUE;
			break;
		}
	}

	if(bFound)
	{
		if(nIndex <= AGPMADMIN_MAX_IP_STRING)
		{
			memcpy(pstServerInfo->m_szServerIP, pcsServerTemplate->m_szIPv4Addr, nIndex);
			pstServerInfo->m_szServerIP[nIndex] = '\0';

			if(nIndex < strlen(pcsServerTemplate->m_szIPv4Addr))
				pstServerInfo->m_lPort = (INT16)atoi(&pcsServerTemplate->m_szIPv4Addr[nIndex+1]);
		}
		else
			bFound = FALSE;
	}
	*/

	return bFound;
}

INT16 AgsmAdmin::GetPortFromAgsdServer(AgsdServer* pcsAgsdServer)
{
	if(!pcsAgsdServer)
		return 0;

	INT16 nPort = 0;

	BOOL bFound = FALSE;
	INT16 nIndex = 0;
	for(nIndex; nIndex < (INT16)strlen(pcsAgsdServer->m_szIP); nIndex++)
	{
		if(pcsAgsdServer->m_szIP[nIndex] == ':')
		{
			bFound = TRUE;
			break;
		}
	}

	if(bFound)
		nPort = (INT16)atoi(&pcsAgsdServer->m_szIP[nIndex]);
	/*
	AgsdServerTemplate* pcsServerTemplate = (AgsdServerTemplate*)pcsAgsdServer->m_pcsTemplate;
	if(!pcsServerTemplate)
		return FALSE;

	BOOL bFound = FALSE;
	INT16 nIndex = 0;
	for(nIndex; nIndex < strlen(pcsServerTemplate->m_szIPv4Addr); nIndex++)
	{
		if(pcsServerTemplate->m_szIPv4Addr[nIndex] == ':')
		{
			bFound = TRUE;
			break;
		}
	}

	if(bFound)
		nPort = (INT16)atoi(&pcsServerTemplate->m_szIPv4Addr[nIndex]);
	*/
	
	return nPort;
}

// Ping to Relay
void AgsmAdmin::ProcessPingRelaySend()
{
	if(!IsGameServer())
		return;

	// 쓰레드가 여러개가 될수 있으므로 Lock 해준다.
	AuAutoLock pLock(m_csPingRelayLock);
	if(!pLock.Result())
		return;

	DWORD dwNowTickCount = GetTickCount();
	if(dwNowTickCount - m_csPingRelay.m_dwLastPingTickCount < AGSMADMIN_TIMEINTERVAL_PING)
		return;

	m_csPingRelay.m_dwLastPingTickCount = dwNowTickCount;

	// Queue 에 넣는다.
	if(AGSMADMIN_MAX_PING_QUEUE_COUNT <= m_csPingRelay.m_lPingCurrentQueueCount)
	{
		// Queue 가 다 찼으면 마지막을 냅두고 앞으로 땡긴다.
		m_csPingRelay.m_arrPingSendQueue.MemCopy(0, &m_csPingRelay.m_arrPingSendQueue[1], AGSMADMIN_MAX_PING_QUEUE_COUNT - 1);

		m_csPingRelay.m_lPingCurrentQueueCount--;
	}

	// 마지막에 넣는다.
	m_csPingRelay.m_arrPingSendQueue[m_csPingRelay.m_lPingCurrentQueueCount++] = dwNowTickCount;

	// Send Count 증가
	m_csPingRelay.m_lPingSendCount++;

	// 마지막에 받은 TickCount 와 보낸 TickCount 를 비교해서 우울하면 우울하다고 세팅해준다.
	if(m_csPingRelay.m_dwLastPingTickCount - m_csPingRelay.m_dwLastReceivePingTickCount > (AGSMADMIN_TIMEINTERVAL_PING * AGSMADMIN_MAX_PING_QUEUE_COUNT))
		m_csPingRelay.m_lPingTime = AGSMADMIN_TIMEINTERVAL_PING * AGSMADMIN_MAX_PING_QUEUE_COUNT;

#ifdef _DEBUG
	if( m_csPingRelay.m_lPingTime > 1000 )
		printf_s("[AgsmAdmin] Ping to Relay : %d (ms)\n", m_csPingRelay.m_dwLastPingTickCount - m_csPingRelay.m_dwLastReceivePingTickCount);
#endif

	// 릴레이로 Ping Packet 쏜다.
	EnumCallback(AGSMADMIN_CB_PING_RELAY_SERVER, &dwNowTickCount, NULL);
}

// Ping Receive From Relay
void AgsmAdmin::ProcessPingRelayReceive(DWORD dwSendTickCount)
{
	if(dwSendTickCount == 0)
		return;

	DWORD dwNowTickCount = GetTickCount();

	m_csPingRelayLock.WLock();

	// Receive Count 증가
	m_csPingRelay.m_lPingReceiveCount++;

	// 마지막에 받은 놈 세팅
	m_csPingRelay.m_dwLastReceivePingTickCount = dwNowTickCount;

	// 미리 이렇세 세팅한다.
	m_csPingRelay.m_lPingTime = AGSMADMIN_TIMEINTERVAL_PING * AGSMADMIN_MAX_PING_QUEUE_COUNT;

	// Send Queue 에서 보낸 시간이 같은 놈을 찾는다.
	for(INT32 i = 0; i < m_csPingRelay.m_lPingCurrentQueueCount; i++)
	{
		if(m_csPingRelay.m_arrPingSendQueue[i] == dwSendTickCount)
		{
			// TickCount 를 비교해서 Ping Time 을 넣어준다.
			m_csPingRelay.m_lPingTime = dwNowTickCount - dwSendTickCount;

			// Queue 를 땡긴다.
			if(i != AGSMADMIN_MAX_PING_QUEUE_COUNT - 1)
				m_csPingRelay.m_arrPingSendQueue.MemCopy(i, &m_csPingRelay.m_arrPingSendQueue[i + 1], AGSMADMIN_MAX_PING_QUEUE_COUNT - i - 1);

			m_csPingRelay.m_arrPingSendQueue.MemSet(AGSMADMIN_MAX_PING_QUEUE_COUNT - 1, 1);
			m_csPingRelay.m_lPingCurrentQueueCount--;

			break;
		}
	}

	m_csPingRelayLock.Release();
}

// Ping Data 를 얻는다.
void AgsmAdmin::GetPingRelayInfo(AgsdAdminPing* pcsAgsdAdminPing)
{
	if(!pcsAgsdAdminPing)
		return;

	m_csPingRelayLock.WLock();

	pcsAgsdAdminPing->m_dwLastPingTickCount = m_csPingRelay.m_dwLastPingTickCount;
	pcsAgsdAdminPing->m_dwLastReceivePingTickCount = m_csPingRelay.m_dwLastReceivePingTickCount;
	pcsAgsdAdminPing->m_arrPingSendQueue.MemCopy(0, &m_csPingRelay.m_arrPingSendQueue[0], AGSMADMIN_MAX_PING_QUEUE_COUNT);
	pcsAgsdAdminPing->m_lPingCurrentQueueCount = m_csPingRelay.m_lPingCurrentQueueCount;
	pcsAgsdAdminPing->m_lPingTime = m_csPingRelay.m_lPingTime;
    pcsAgsdAdminPing->m_lPingSendCount = m_csPingRelay.m_lPingSendCount;
	pcsAgsdAdminPing->m_lPingReceiveCount = m_csPingRelay.m_lPingReceiveCount;

	m_csPingRelayLock.Release();
}

// 클라이언트에게 Ping Send
void AgsmAdmin::ProcessPingClient(UINT32 ulClockCount)
{
	// 로그인 서버만 한다.
	AgsdServer* pcsThisServer = m_pAgsmServerManager->GetThisServer();
	if(!pcsThisServer)
		return;

	if(pcsThisServer->m_cType != AGSMSERVER_TYPE_LOGIN_SERVER)
		return;

	AgsEngine* pEngine = static_cast<AgsEngine*>(GetModuleManager());
	if(!pEngine)
		return;

	// Interval 체크
	if(m_ulLastSendPingClient != 0 && ulClockCount - m_ulLastSendPingClient < AGSMADMIN_TIMEINTERVAL_PING)
		return;

	m_ulLastSendPingClient = ulClockCount;

	// 소켓 연결된 모든 클라이언트에게 Ping 전송을 한다.
	stAgpdAdminPing stPing;
	stPing.m_ulTimeStamp = AuTimeStamp::GetCurrentTimeStamp();
	for(int i = 0; i < pEngine->m_AsIOCPServer.m_csSocketManager.GetMaxConnectionCount(); ++i)
	{
		if(pEngine->m_AsIOCPServer.m_csSocketManager.IsValidArrayIndex(i) == FALSE)
			continue;

		// 서버에는 Ping 보내지 않는다.
		std::set<UINT32>::iterator iter = m_setServerNID.find(i);
		if(iter != m_setServerNID.end())
			continue;

		AsServerSocket* pSocket = pEngine->m_AsIOCPServer.m_csSocketManager[i];
		if(!pSocket)
			continue;

		// 소켓이 유효한지 확인한다.
		if(!pSocket->IsActive() ||
			pSocket->m_ulConnectedTimeMSec + 60000 > ::timeGetTime())		// 60초가 지나지 않은 애들은 보내지 않는다.
			continue;

		// 시간 체크해서 우울한 놈들은 끊어버린다.
		if(pSocket->m_ulLastSendTimeStamp - pSocket->m_ulLastReceiveTimeStamp > (UINT32)AGSMADMIN_DISCONNECT_LIMIT_TIME)
		{
			pEngine->m_AsIOCPServer.DestroyClient(pSocket);
			continue;
		}	

		// 2007.08.02. steeple
		// 처음 보낼 때는 받는 쪽에도 데이터 세팅해주고 보낸다.
		if(pSocket->m_ulLastSendTimeStamp == 0)
			pSocket->m_ulLastReceiveTimeStamp = stPing.m_ulTimeStamp;

		pSocket->m_ulLastSendTimeStamp = stPing.m_ulTimeStamp;
		SendPing(stPing, i);
	}
}

// 클라이언트에게 받은 Ping 처리
void AgsmAdmin::ProcessPingClientReceive(stAgpdAdminPing& stPing, UINT32 ulNID)
{
	if(!ulNID)
		return;

	AgsEngine* pEngine = static_cast<AgsEngine*>(GetModuleManager());
	if(!pEngine)
		return;

	// 그냥 대입하고 끝.
	if(pEngine->m_AsIOCPServer.m_csSocketManager.IsValidArrayIndex(ulNID) == FALSE)
		return;
	
	if(pEngine->m_AsIOCPServer.m_csSocketManager[ulNID] == FALSE)
		return;

	pEngine->m_AsIOCPServer.m_csSocketManager[ulNID]->m_ulLastReceiveTimeStamp = stPing.m_ulTimeStamp;
}











//////////////////////////////////////////////////////////////////////////
// Packet Make
PVOID AgsmAdmin::MakePacket(INT16* pnPacketLength, INT8* pcOperation, INT32* plCID, INT32* plConnectedAccountLevel, PVOID pvHelp)
{
	if(!pnPacketLength)
		return FALSE;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMADMIN_PACKET_TYPE, pcOperation, plCID, plConnectedAccountLevel, pvHelp);
}









///////////////////////////////////////////////////////////////////////////
// Packet Receive - Server Packet
BOOL AgsmAdmin::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgsmAdmin::OnReceive");

	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = AGSMADMIN_PACKET_MAX;
	INT32 lCID = -1;
	INT32 lLevel = ACCOUNT_LEVEL_NONE;
	PVOID pvHelp = NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&lLevel,
						&pvHelp
						);

	printf("[AgsmAdmin] :: OnReceive(...) | cOperation=%d, lCID=%d\n", cOperation, lCID);

	switch(cOperation)
	{
		case AGSMADMIN_PACKET_CONNECTABLE_ACCOUNT_LEVEL:
			OnReceiveConnectableAccountLevel(lCID, lLevel);
			break;
			
	}

	printf("[AgsmAdmin] :: OnReceive(...) End\n", cOperation, lCID);

	return TRUE;
}

BOOL AgsmAdmin::OnReceiveConnectableAccountLevel(INT32 lCID, INT32 lLevel)
{
	// Member 변수에 대입하고 끝
	SetConnectableAccountLevel(lLevel);
	return TRUE;
}

BOOL AgsmAdmin::SendConnectableAccountLevel(INT32 lLevel, INT32 lCID, UINT32 ulNID)
{
	if(!lCID || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGSMADMIN_PACKET_CONNECTABLE_ACCOUNT_LEVEL;

	pvPacket = MakePacket(&nPacketLength, &cOperation, &lCID, &lLevel, NULL);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}












///////////////////////////////////////////////////////////////////////////
// Packet Send - Public Packet
BOOL AgsmAdmin::SendAddAdmin(stAgpdAdminInfo* pstAdminInfo, INT32 lCID, UINT32 ulNID)
{
	if(!pstAdminInfo || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvLoginPacket = NULL;
	INT16 nPacketLength = 0;

	INT8 cLoginOperation = AGPMADMIN_LOGIN;

	pvLoginPacket = m_pagpmAdmin->MakeLoginPacket(&nPacketLength,
														&cLoginOperation, 
														&pstAdminInfo->m_lCID,
														pstAdminInfo->m_szAdminName,
														&pstAdminInfo->m_lAdminLevel,
														&pstAdminInfo->m_lServerID,
														pstAdminInfo->m_szAccName);

	if(!pvLoginPacket)
	{
		OutputDebugString("AgsmAdmin::SendAddAdmin(...) pvLoginPacket is Invalid\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeLoginPacket(&nPacketLength, &lCID, pvLoginPacket);

	m_pagpmAdmin->m_csLoginPacket.FreePacket(pvLoginPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendAddAdmin(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
}

BOOL AgsmAdmin::SendRemoveAdmin(stAgpdAdminInfo* pstAdminInfo, INT32 lCID, UINT32 ulNID)
{
	if(!pstAdminInfo || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvLoginPacket = NULL;
	INT16 nPacketLength = 0;

	INT8 cLoginOperation = AGPMADMIN_LOGOUT;

	pvLoginPacket = m_pagpmAdmin->MakeLoginPacket(&nPacketLength,
														&cLoginOperation,
														&pstAdminInfo->m_lCID,
														NULL,
														NULL,
														NULL,
														NULL);

	if(!pvLoginPacket)
	{
		OutputDebugString("AgsmAdmin::SendRemoveAdmin(...) pvLoginPacket is Invalid\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeLoginPacket(&nPacketLength, &lCID, pvLoginPacket);

	m_pagpmAdmin->m_csLoginPacket.FreePacket(pvLoginPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendRemoveAdmin(...) pvPacket is Invalid\n");
		return FALSE;
	}
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2004.02.26.
// 다른 서버로에도 전송하는 것으로 바꿈.
BOOL AgsmAdmin::SendAddAdminToAllAdmin(stAgpdAdminInfo* pstAdminInfo, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pstAdminInfo || !pcsAgpdCharacter)
		return FALSE;

	// Game Server 가 아니라면 이짓을 하지 않는다.
	//if(!IsGameServer())
	//	return FALSE;

	UINT32 lThisServerID = GetThisServerID();

	// Admin Login 을 알린다.
	// 현재 서버에 있는 Admin 에게만 보낸다.
	UINT32 ulNID = 0, lServerID = 0;
	AgpdCharacter* pcsTmpCharacter = NULL;
	AgsdCharacter* pcsAgsdCharacter = NULL;

	list<INT32> listAdminID;
	m_pagpmAdmin->AdminLock();
	m_pagpmAdmin->GetAdminIDList(&listAdminID);
	m_pagpmAdmin->AdminUnlock();
	list<INT32>::iterator iterData = listAdminID.begin();
	while(iterData != listAdminID.end())
	{
		ulNID = lServerID = 0;

		if(*iterData == pcsAgpdCharacter->m_lID)
		{
			ulNID = m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);
			pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
			if(pcsAgsdCharacter)
				lServerID = pcsAgsdCharacter->m_ulServerID;
		}
		else
		{
			pcsTmpCharacter = m_pagpmCharacter->GetCharacterLock(*iterData);
			if(pcsTmpCharacter)
			{
				pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsTmpCharacter);
				if(pcsAgsdCharacter)
				{
					ulNID = pcsAgsdCharacter->m_dpnidCharacter;
					lServerID = pcsAgsdCharacter->m_ulServerID;
				}

				pcsTmpCharacter->m_Mutex.Release();
			}
		}

		// 현재 서버에 있는 Admin 에게만 보낸다.
		if(ulNID > 0 && lServerID == lThisServerID)
			SendAddAdmin(pstAdminInfo, *iterData, ulNID);
		
		iterData++;
	}

	return TRUE;
}

// 뒤의 pcsAgpdCharacter 는 NULL 이 될 수 있다.
// 고로 NULL 체크해서 해야 한다.
BOOL AgsmAdmin::SendRemoveAdminToAllAdmin(stAgpdAdminInfo* pstAdminInfo, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pstAdminInfo)
		return FALSE;

	// Game Server 가 아니라면 이짓을 하지 않는다.
	//if(!IsGameServer())
	//	return FALSE;

	UINT32 lThisServerID = GetThisServerID();

	// Admin Logout 을 알린다.
	// 현재 서버에 있는 Admin 에게만 보낸다.
	UINT32 ulNID = 0, lServerID = 0;
	AgpdCharacter* pcsTmpCharacter = NULL;
	AgsdCharacter* pcsAgsdCharacter = NULL;

	list<INT32> listAdminID;
	m_pagpmAdmin->AdminLock();
	m_pagpmAdmin->GetAdminIDList(&listAdminID);
	m_pagpmAdmin->AdminUnlock();
	list<INT32>::iterator iterData = listAdminID.begin();
	while(iterData != listAdminID.end())
	{
		ulNID = lServerID = 0;

		if(*iterData == pstAdminInfo->m_lCID)
		{
			if(pcsAgpdCharacter)
			{
				ulNID = m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);
				pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
				if(pcsAgsdCharacter)
					lServerID = pcsAgsdCharacter->m_ulServerID;
			}
		}
		else
		{
			pcsTmpCharacter = m_pagpmCharacter->GetCharacterLock(*iterData);
			if(pcsTmpCharacter)
			{
				pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsTmpCharacter);
				if(pcsAgsdCharacter)
				{
					ulNID = pcsAgsdCharacter->m_dpnidCharacter;
					lServerID = pcsAgsdCharacter->m_ulServerID;
				}

				pcsTmpCharacter->m_Mutex.Release();
			}
		}

		// 현재 서버에 있는 Admin 에게만 보낸다.
		if(ulNID > 0 && lServerID == lThisServerID)
			SendRemoveAdmin(pstAdminInfo, *iterData, ulNID);
		
		iterData++;
	}

	return TRUE;
}

// 인자로 넘어온 Character 에게 Admin List 를 보낸다.
BOOL AgsmAdmin::SendAdminList(AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmCharacter || !m_pagsmCharacter)
		return FALSE;

	//list<stAgpdAdminInfo*> listAdminInfo;
	//m_pagpmAdmin->GetAdminInfoList(&listAdminInfo);
	DPNID dpnID = m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);

	m_pagpmAdmin->AdminLock();
	list<stAgpdAdminInfo*>::iterator iterData = m_pagpmAdmin->GetAdminInfoList()->begin();
	while(iterData != m_pagpmAdmin->GetAdminInfoList()->end())
	{
		SendAddAdmin(*iterData, pcsAgpdCharacter->m_lID, dpnID);
		//delete *iterData;
		iterData++;
	}
	m_pagpmAdmin->AdminUnlock();

	//if(listAdminInfo.size() != 0)
	//	listAdminInfo.clear();

	return TRUE;
}

// Admin 에게 현재 접속자 수를 알린다.
BOOL AgsmAdmin::SendNumCurrentUser(INT32 lCurrentUser, INT32 lCID, UINT32 ulNID)
{
	if(!lCurrentUser || !lCID || !ulNID)
		return FALSE;

	// Login Packet 을 사용한다.
	PVOID pvPacket = NULL;
	PVOID pvLoginPacket = NULL;
	INT16 nPacketLength = 0;

	pvLoginPacket = m_pagpmAdmin->MakeLoginPacket(&nPacketLength,
														NULL,
														&lCurrentUser,
														NULL,
														NULL,
														NULL,
														NULL);

	if(!pvLoginPacket)
	{
		OutputDebugString("AgsmAdmin::SendNumCurrentUser(...) pvLoginPacket is Invalid\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeNumCurrentUserPacket(&nPacketLength, &lCID, pvLoginPacket);

	m_pagpmAdmin->m_csLoginPacket.FreePacket(pvLoginPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendNumCurrentUser(...) pvPacket is Invalid\n");
		return FALSE;
	}
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}






















//////////////////////////////////////////////////////////////////////////
// Server Info
BOOL AgsmAdmin::SendServerInfo(stAgpdAdminServerInfo* pstServerInfo, INT32 lCID, UINT32 ulNID)
{
	if(!pstServerInfo || !lCID || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvServerInfoPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_SERVERINFO;

	pvServerInfoPacket = m_pagpmAdmin->MakeServerInfoPacket(&nPacketLength, pstServerInfo->m_szServerIP, &pstServerInfo->m_lPort);
	if(!pvServerInfoPacket)
	{
		OutputDebugString("AgsmAdmin::SendServerInfo(...) pvServerInfoPacket is NULL\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeServerInfoPacket(&nPacketLength, &lCID, pvServerInfoPacket);

	m_pagpmAdmin->m_csServerInfoPacket.FreePacket(pvServerInfoPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendServerInfo(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
}

BOOL AgsmAdmin::SendAdminClientLoginOK(AgpdCharacter* pstCharacter, UINT32 ulNID)
{
	if(!ulNID || !pstCharacter)
		return FALSE;

	stAgpdAdminServerInfo stClientInfo;
	stClientInfo.m_lPort = 0;

	strncpy(stClientInfo.m_szServerIP, "LoginOK", AGPMADMIN_MAX_IP_STRING);

	PVOID pvPacket = NULL;
	PVOID pvClientInfoPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_ADMIN_CLIENT_LOGIN;

	pvClientInfoPacket = m_pagpmAdmin->MakeServerInfoPacket(&nPacketLength, stClientInfo.m_szServerIP, &stClientInfo.m_lPort);

	if(!pvClientInfoPacket)
		return FALSE;

	pvPacket = m_pagpmAdmin->MakeAdminPacket(TRUE, &nPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, &pstCharacter->m_lID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvClientInfoPacket, NULL);

	m_pagpmAdmin->m_csPacket.FreePacket(pvClientInfoPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendAdminClientLoginOK(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}




// Account Level
BOOL AgsmAdmin::SetConnectableAccountLevel(INT32 lLevel)
{
	m_lConnectableAccountLevel = lLevel;
	m_pagpmAdmin->m_lConnectableAccountLevel = lLevel;	// AgpmAdmin 도 맞춰준다.
	printf("\nConnectable Account Level : %d\n", m_lConnectableAccountLevel);
	return TRUE;
}

INT32 AgsmAdmin::GetConnectableAccountLevel()
{
	return m_lConnectableAccountLevel;
}










//////////////////////////////////////////////////////////////////////////
// Callback Registration Function

// For AgsmMakSQL
BOOL AgsmAdmin::SetCallbackUpdateCharacterBan(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_BAN_UPDATE_DB, pfCallback, pClass);
}

// For AgsmAdmin
BOOL AgsmAdmin::SetCallbackSearchAccount(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_SEARCH_ACCOUNT, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackPingSend(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_PING_RELAY_SERVER, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelaySearchAccount(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_SEARCH_ACCOUNT, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelaySearchAccountBank(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_SEARCH_ACCOUNT_BANK, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelaySearchCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_SEARCH_CHARACTER, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelayUpdateCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_UPDATE_CHARACTER, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelayUpdateCharacter2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_UPDATE_CHARACTER2, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelayUpdateBankMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_UPDATE_BANK_MONEY, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelayUpdateCharismaPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_UPDATE_CHARISMA_POINT, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelayItemInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_ITEM_INSERT, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelayItemDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_ITEM_DELETE, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelayItemConvertUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_ITEM_CONVERT_UPDATE, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelayItemConvertUpdate2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_ITEM_CONVERT_UPDATE2, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelayItemUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_ITEM_UPDATE, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackRelaySkillUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_RELAY_SKILL_UPDATE, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackConcurrentUser(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_CONCURRENT_USER, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackStartSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_START_SIEGEWAR, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackEndSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_END_SIEGEWAR, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackEndArchlordBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_END_ARCHLORD_BATTLE, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackArchlordSetGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_ARCHLORD_SET_GUARD, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackArchlordCancelGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_ARCHLORD_CANCEL_GUARD, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackArchlordGuardInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_ARCHLORD_GUARD_INFO, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackArchlordSetArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_ARCHLORD_SET_ARCHLORD, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackArchlordCancelArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_ARCHLORD_CANCEL_ARCHLORD, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackArchlordStart(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_ARCHLORD_START, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackArchlordEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_ARCHLORD_END, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackArchlordNextStep(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_ARCHLORD_NEXT_STEP, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackSetCastleOwner(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_SET_CASTLE_OWNER, pfCallback, pClass);
}

BOOL AgsmAdmin::SetCallbackCancelCastleOwner(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMADMIN_CB_CANCEL_CASTLE_OWNER, pfCallback, pClass);
}

///////////////////////////////////////////////////////////////////////////
// Callback Function

BOOL AgsmAdmin::CBAddAdmin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!pcsAgpdCharacter || !pThis)
		return FALSE;

	return pThis->AddAdmin(pcsAgpdCharacter);
}

BOOL AgsmAdmin::CBIsAdminCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	INT32 *pCID = (INT32*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!pCID || !pThis)
		return FALSE;

	return pThis->m_pagpmAdmin->IsAdminCharacter(*pCID);
}

BOOL AgsmAdmin::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!pcsAgpdCharacter || !pThis)
		return FALSE;

	return pThis->EnterGameWorld(pcsAgpdCharacter);
}

// AgpmAdmin 에서 호출되도록 변경. 2004.03.09.
BOOL AgsmAdmin::CBRemoveAdmin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	INT32* plRemoveCID = (INT32*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!plRemoveCID || !pThis)
		return FALSE;

	return pThis->RemoveAdmin(*plRemoveCID);
}

BOOL AgsmAdmin::CBDisconnectAdmin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!pcsAgpdCharacter || !pThis)
		return FALSE;

	return pThis->DisconnectAdmin(pcsAgpdCharacter);
}

BOOL AgsmAdmin::CBConnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	UINT32* pNID = (UINT32*)pCustData;

	if(!pcsAgpdCharacter || !pThis || !pNID)
		return FALSE;

	return pThis->Connect(pcsAgpdCharacter, *pNID);
}

// Agpmadmin 에서 호출
BOOL AgsmAdmin::CBAddCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!pcsAgpdCharacter || !pThis)
		return FALSE;

	return pThis->AddCharacter(pcsAgpdCharacter);
}

// AgpmAdmin 에서 호출
BOOL AgsmAdmin::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!pcsAgpdCharacter || !pThis)
		return FALSE;

	return pThis->RemoveCharacter(pcsAgpdCharacter);
}

// AgpmAdmin 에서 호출
BOOL AgsmAdmin::CBNumCurrentUser(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsAgpdCharacter= (AgpdCharacter*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!pcsAgpdCharacter || !pThis)
		return FALSE;

	if(pThis->m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
		return FALSE;

	return pThis->SendNumCurrentUser(pThis->m_pagsmCharacter->GetNumOfPlayers(), pcsAgpdCharacter->m_lID, pThis->GetCharacterNID(pcsAgpdCharacter));
}

// AgpmAdmin 에서 호출
BOOL AgsmAdmin::CBUserPosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	
	if(!pcsAgpdCharacter || !pThis)
		return FALSE;

	INT32 stTempPos[AGSMAREACHATTING_MAX_REGIONTEMPLATE];
	ZeroMemory(stTempPos, sizeof(stTempPos));

	ApAdmin *pApRegionAdmin = pThis->m_pagsmAreaChatting->GetRegionAdmin();
	if(!pApRegionAdmin)
		return FALSE;

	for(int i=0; i<AGSMAREACHATTING_MAX_REGIONTEMPLATE; ++i)
	{
		stTempPos[i] = pApRegionAdmin[i].GetObjectCount(); 
	}

	PACKET_AGPMADMIN_USER_POSITON pPacket(AGSMAREACHATTING_MAX_REGIONTEMPLATE, stTempPos, pcsAgpdCharacter->m_lID);
	AgsEngine::GetInstance()->SendPacket(pPacket, pThis->m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}

// AgsmLoginDB 에서 호출
BOOL AgsmAdmin::CBIsConnectableAccountLevel(PVOID pData, PVOID pClass, PVOID pCustData)
{
	INT32* plLevel = (INT32*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	BOOL* pbConnectable = (BOOL*)pCustData;

	if(!plLevel || !pThis || !pbConnectable)
		return FALSE;

	if(pThis->m_lConnectableAccountLevel > *plLevel)
		*pbConnectable = FALSE;
	else
		*pbConnectable = TRUE;

	return TRUE;
}

// Log
BOOL AgsmAdmin::WriteLog_Char(eAGPDLOGTYPE_PLAY eLogType, AgpdCharacter *pcsAdmin, AgpdCharacter *pcsTarget)
{
	if (!m_pagpmLog || !pcsAdmin || !pcsTarget)
		return FALSE;

	AgsdCharacter *pcsAgsdAdmin = m_pagsmCharacter->GetADCharacter(pcsAdmin);
	AgsdCharacter *pcsAgsdTarget = m_pagsmCharacter->GetADCharacter(pcsTarget);
	if (NULL == pcsAgsdAdmin || NULL == pcsAgsdTarget)
		return FALSE;
	
	// target
	m_pagpmLog->WriteLog_Play(eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],			// IP만 GM의 IP로...
							pcsAgsdTarget->m_szAccountID,
							pcsAgsdTarget->m_szServerName,
							pcsTarget->m_szID,
							pcsTarget->m_pcsCharacterTemplate ? pcsTarget->m_pcsCharacterTemplate->m_lID : 0,
							m_pagpmCharacter->GetLevel(pcsTarget),
							m_pagpmCharacter->GetExp(pcsTarget),
							pcsTarget->m_llMoney,
							pcsTarget->m_llBankMoney,
							m_pagpmCharacter->GetLevel(pcsTarget),
							0,
							0,
							0,
							0,
							0,
							0
							);
	// GM
	m_pagpmLog->WriteLog_Play(eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],
							pcsAgsdAdmin->m_szAccountID,
							pcsAgsdAdmin->m_szServerName,
							pcsAdmin->m_szID,
							pcsAdmin->m_pcsCharacterTemplate ? pcsAdmin->m_pcsCharacterTemplate->m_lID : 0,
							m_pagpmCharacter->GetLevel(pcsAdmin),
							m_pagpmCharacter->GetExp(pcsAdmin),
							pcsAdmin->m_llMoney,
							pcsAdmin->m_llBankMoney,
							m_pagpmCharacter->GetLevel(pcsAdmin),
							0,
							0,
							0,
							0,
							0,
							0
							);
	return TRUE;
}

BOOL AgsmAdmin::WriteLog_Char(eAGPDLOGTYPE_PLAY eLogType, AgpdCharacter *pcsAdmin, CHAR *pszCharName, INT32 lTID, INT32 lLevel, INT64 llExp, INT64 llMoney)
{
	if (!m_pagpmLog || !pcsAdmin || !pszCharName)
		return FALSE;

	AgsdCharacter *pcsAgsdAdmin = m_pagsmCharacter->GetADCharacter(pcsAdmin);
	if (NULL == pcsAgsdAdmin)
		return FALSE;
	
	// target
	m_pagpmLog->WriteLog_Play(eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],			// IP만 GM의 IP로...
							"",
							"",
							pszCharName,
							lTID,
							lLevel,
							llExp,
							llMoney,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							0
							);
	// GM
	m_pagpmLog->WriteLog_Play(eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],
							pcsAgsdAdmin->m_szAccountID,
							pcsAgsdAdmin->m_szServerName,
							pcsAdmin->m_szID,
							pcsAdmin->m_pcsCharacterTemplate ? pcsAdmin->m_pcsCharacterTemplate->m_lID : 0,
							m_pagpmCharacter->GetLevel(pcsAdmin),
							m_pagpmCharacter->GetExp(pcsAdmin),
							pcsAdmin->m_llMoney,
							pcsAdmin->m_llBankMoney,
							m_pagpmCharacter->GetLevel(pcsAdmin),
							0,
							0,
							0,
							0,
							0,
							0
							);
	return TRUE;
}

BOOL AgsmAdmin::WriteLog_Item(eAGPDLOGTYPE_ITEM eLogType, AgpdCharacter *pcsAdmin, AgpdCharacter *pcsTarget, AgpdItem *pcsItem, INT32 lQuantity)
{
	if (!m_pagpmLog || !pcsAdmin || !pcsTarget || !pcsItem)
		return FALSE;

	AgsdCharacter *pcsAgsdAdmin = m_pagsmCharacter->GetADCharacter(pcsAdmin);
	AgsdCharacter *pcsAgsdTarget = m_pagsmCharacter->GetADCharacter(pcsTarget);
	if (NULL == pcsAgsdAdmin || NULL == pcsAgsdTarget)
		return FALSE;
	
	AgsdItem *pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
	if (NULL == pcsAgsdItem)
		return FALSE;
	
	CHAR szConvert[AGPDLOG_MAX_ITEM_CONVERT_STRING+1];
	ZeroMemory(szConvert, sizeof(szConvert));
	AgpdItemConvertADItem *pcsItemConvertADItem = m_pagpmItemConvert->GetADItem(pcsItem);
	m_pagpmItemConvert->EncodeConvertHistory(pcsItemConvertADItem, szConvert, AGPDLOG_MAX_ITEM_CONVERT_STRING);

	CHAR szOption[AGPDLOG_MAX_ITEM_FULL_OPTION + 1];
	ZeroMemory(szOption, sizeof(szOption));
	CHAR szNormalOption[AGPDLOG_MAX_ITEM_OPTION+1];
	ZeroMemory(szNormalOption, sizeof(szNormalOption));
	m_pagsmItem->EncodingOption(pcsItem, szNormalOption, AGPDLOG_MAX_ITEM_OPTION);

	CHAR szPosition[33];
	ZeroMemory(szPosition, sizeof(szPosition));
	m_pagsmCharacter->EncodingPosition(&pcsTarget->m_stPos, szPosition, 32);	

	//######################
	CHAR szSkillPlus[AGPDLOG_MAX_ITEM_OPTION+1];
	ZeroMemory(szSkillPlus, sizeof(szSkillPlus));
	m_pagsmItem->EncodingSkillPlus(pcsItem, szSkillPlus, AGPDLOG_MAX_ITEM_OPTION);

	sprintf(szOption, "%s::%s", szNormalOption, szSkillPlus);
	
	BOOL bIsCashItem = IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType);
	
	// target
	m_pagpmLog->WriteLog_Item(bIsCashItem,
							eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],			// IP만 GM의 IP로...
							pcsAgsdTarget->m_szAccountID,
							pcsAgsdTarget->m_szServerName,
							pcsTarget->m_szID,
							pcsTarget->m_pcsCharacterTemplate ? pcsTarget->m_pcsCharacterTemplate->m_lID : 0,
							m_pagpmCharacter->GetLevel(pcsTarget),
							m_pagpmCharacter->GetExp(pcsTarget),
							pcsTarget->m_llMoney,
							pcsTarget->m_llBankMoney,
							pcsAgsdItem->m_ullDBIID,
							pcsItem->m_pcsItemTemplate ? pcsItem->m_pcsItemTemplate->m_lID : 0,
							lQuantity,
							szConvert,
							szOption,
							0,
							pcsAdmin->m_szID,
							pcsItem->m_nInUseItem,
							pcsItem->m_lCashItemUseCount,
							pcsItem->m_lRemainTime,
							pcsItem->m_lExpireTime,
							pcsItem->m_llStaminaRemainTime,
							m_pagpmItem->GetItemDurabilityCurrent(pcsItem),
							m_pagpmItem->GetItemDurabilityMax(pcsItem),
							szPosition
							);
	// GM
	m_pagpmLog->WriteLog_Item(bIsCashItem,
							eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],
							pcsAgsdAdmin->m_szAccountID,
							pcsAgsdAdmin->m_szServerName,
							pcsAdmin->m_szID,
							pcsAdmin->m_pcsCharacterTemplate ? pcsAdmin->m_pcsCharacterTemplate->m_lID : 0,
							m_pagpmCharacter->GetLevel(pcsAdmin),
							m_pagpmCharacter->GetExp(pcsAdmin),
							pcsAdmin->m_llMoney,
							pcsAdmin->m_llBankMoney,
							pcsAgsdItem->m_ullDBIID,
							pcsItem->m_pcsItemTemplate ? pcsItem->m_pcsItemTemplate->m_lID : 0,
							lQuantity,
							szConvert,
							szOption,
							0,
							pcsTarget->m_szID,
							pcsItem->m_nInUseItem,
							pcsItem->m_lCashItemUseCount,
							pcsItem->m_lRemainTime,
							pcsItem->m_lExpireTime,
							pcsItem->m_llStaminaRemainTime,
							m_pagpmItem->GetItemDurabilityCurrent(pcsItem),
							m_pagpmItem->GetItemDurabilityMax(pcsItem),
							szPosition
							);
	return TRUE;
}

BOOL AgsmAdmin::WriteLog_Item(eAGPDLOGTYPE_ITEM eLogType, AgpdCharacter *pcsAdmin, CHAR *pszAccountID, CHAR *pszCharName, AgpdItem *pcsItem, INT32 lQuantity)
{
	if (!m_pagpmLog || !pcsAdmin || !pcsItem || !pszCharName)
		return FALSE;

	AgsdCharacter *pcsAgsdAdmin = m_pagsmCharacter->GetADCharacter(pcsAdmin);
	if (NULL == pcsAgsdAdmin)
		return FALSE;
	
	AgsdItem *pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
	if (NULL == pcsAgsdItem)
		return FALSE;
	
	CHAR szConvert[AGPDLOG_MAX_ITEM_CONVERT_STRING+1];
	ZeroMemory(szConvert, sizeof(szConvert));
	AgpdItemConvertADItem *pcsItemConvertADItem = m_pagpmItemConvert->GetADItem(pcsItem);
	m_pagpmItemConvert->EncodeConvertHistory(pcsItemConvertADItem, szConvert, AGPDLOG_MAX_ITEM_CONVERT_STRING);

	CHAR szOption[AGPDLOG_MAX_ITEM_FULL_OPTION + 1];
	ZeroMemory(szOption, sizeof(szOption));

	CHAR szNormalOption[AGPDLOG_MAX_ITEM_OPTION+1];
	ZeroMemory(szNormalOption, sizeof(szNormalOption));
	m_pagsmItem->EncodingOption(pcsItem, szNormalOption, AGPDLOG_MAX_ITEM_OPTION);
	
	//##########################
	CHAR szSkillPlus[AGPDLOG_MAX_ITEM_OPTION+1];
	ZeroMemory(szSkillPlus, sizeof(szSkillPlus));
	m_pagsmItem->EncodingSkillPlus(pcsItem, szSkillPlus, AGPDLOG_MAX_ITEM_OPTION);	

	sprintf(szOption, "%s::%s", szNormalOption, szSkillPlus);

	CHAR szPosition[33];
	ZeroMemory(szPosition, sizeof(szPosition));
	m_pagsmCharacter->EncodingPosition(&pcsAdmin->m_stPos, szPosition, 32);
	
	BOOL bIsCashItem = IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType);
	
	// target
	m_pagpmLog->WriteLog_Item(bIsCashItem,
							eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],			// IP만 GM의 IP로...
							pszAccountID ? pszAccountID : "",
							"",
							pszCharName,
							0,
							0,
							0,
							0,
							0,
							pcsAgsdItem->m_ullDBIID,
							pcsItem->m_pcsItemTemplate ? pcsItem->m_pcsItemTemplate->m_lID : 0,
							lQuantity,
							szConvert,
							szOption,
							0,
							pcsAdmin->m_szID,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							szPosition
							);
	// GM
	m_pagpmLog->WriteLog_Item(bIsCashItem,
							eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],			// IP만 GM의 IP로...
							pcsAgsdAdmin->m_szAccountID,
							pcsAgsdAdmin->m_szServerName,
							pcsAdmin->m_szID,
							pcsAdmin->m_pcsCharacterTemplate ? pcsAdmin->m_pcsCharacterTemplate->m_lID : 0,
							m_pagpmCharacter->GetLevel(pcsAdmin),
							m_pagpmCharacter->GetExp(pcsAdmin),
							pcsAdmin->m_llMoney,
							pcsAdmin->m_llBankMoney,
							pcsAgsdItem->m_ullDBIID,
							pcsItem->m_pcsItemTemplate ? pcsItem->m_pcsItemTemplate->m_lID : 0,
							lQuantity,
							szConvert,
							szOption,
							0,
							pszCharName,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							szPosition
							);
	return TRUE;
}

BOOL AgsmAdmin::WriteLog_Item(eAGPDLOGTYPE_ITEM eLogType, AgpdCharacter *pcsAdmin, CHAR *pszAccountID, CHAR *pszCharName, UINT64 ullItemSeq, CHAR *pszConvert, CHAR *pszOption, CHAR* pszPosition)
{
	if (!m_pagpmLog || !pcsAdmin || !pszCharName)
		return FALSE;

	AgsdCharacter *pcsAgsdAdmin = m_pagsmCharacter->GetADCharacter(pcsAdmin);
	if (NULL == pcsAgsdAdmin)
		return FALSE;

	// target	
	m_pagpmLog->WriteLog_Item(FALSE,
							eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],			// IP만 GM의 IP로...
							pszAccountID ? pszAccountID : "",
							"",
							pszCharName,
							0,
							0,
							0,
							0,
							0,
							ullItemSeq,
							0, //itemtid
							1,
							pszConvert ? pszConvert : "",
							pszOption ? pszOption : "",
							0,
							pcsAdmin->m_szID,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							(pszPosition) ? pszOption : ""
							);
	// GM
	m_pagpmLog->WriteLog_Item(FALSE,
							eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],			// IP만 GM의 IP로...
							pcsAgsdAdmin->m_szAccountID,
							pcsAgsdAdmin->m_szServerName,
							pcsAdmin->m_szID,
							pcsAdmin->m_pcsCharacterTemplate ? pcsAdmin->m_pcsCharacterTemplate->m_lID : 0,
							m_pagpmCharacter->GetLevel(pcsAdmin),
							m_pagpmCharacter->GetExp(pcsAdmin),
							pcsAdmin->m_llMoney,
							pcsAdmin->m_llBankMoney,
							ullItemSeq,
							0, //itemtid
							1,
							pszConvert ? pszConvert : "",
							pszOption ? pszOption : "",
							0,
							pszCharName,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							(pszPosition) ? pszOption : ""
							);
	return TRUE;
}

BOOL AgsmAdmin::WriteLog_Gheld(eAGPDLOGTYPE_GHELD eLogType, AgpdCharacter *pcsAdmin, AgpdCharacter *pcsTarget)
{
	if (!m_pagpmLog || !pcsAdmin || !pcsTarget)
		return FALSE;

	AgsdCharacter *pcsAgsdAdmin = m_pagsmCharacter->GetADCharacter(pcsAdmin);
	AgsdCharacter *pcsAgsdTarget = m_pagsmCharacter->GetADCharacter(pcsTarget);
	if (NULL == pcsAgsdAdmin || NULL == pcsAgsdTarget)
		return FALSE;
	
	// target
	m_pagpmLog->WriteLog_Gheld(eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],			// IP만 GM의 IP로...
							pcsAgsdTarget->m_szAccountID,
							pcsAgsdTarget->m_szServerName,
							pcsTarget->m_szID,
							pcsTarget->m_pcsCharacterTemplate ? pcsTarget->m_pcsCharacterTemplate->m_lID : 0,
							m_pagpmCharacter->GetLevel(pcsTarget),
							m_pagpmCharacter->GetExp(pcsTarget),
							pcsTarget->m_llMoney,
							pcsTarget->m_llBankMoney,
							0,
							pcsAdmin->m_szID
							);
	// GM
	m_pagpmLog->WriteLog_Gheld(eLogType, 0,
							&pcsAgsdAdmin->m_strIPAddress[0],			// IP만 GM의 IP로...
							pcsAgsdAdmin->m_szAccountID,
							pcsAgsdAdmin->m_szServerName,
							pcsAdmin->m_szID,
							pcsAdmin->m_pcsCharacterTemplate ? pcsAdmin->m_pcsCharacterTemplate->m_lID : 0,
							m_pagpmCharacter->GetLevel(pcsAdmin),
							m_pagpmCharacter->GetExp(pcsAdmin),
							pcsAdmin->m_llMoney,
							pcsAdmin->m_llBankMoney,
							0,
							pcsTarget->m_szID
							);
	return TRUE;
}
	
void AgsmAdmin::ProcessNoticeRepeat()
{
	UINT32				ulClockCount = GetClockCount();
	AssmNoticeIter		iter;
	AssmNoticeRepeat *	pcsNotice;

	AuAutoLock lock(m_csNoticeCS);
	if (!lock.Result()) return;

	iter = m_vtNotice.begin();
	while (iter != m_vtNotice.end())
	{
		if ((*iter).m_ulDuration && (*iter).m_ulStartTime + (*iter).m_ulDuration <= ulClockCount)
		{
			if ((*iter).m_szMessage)
			{
				delete [] (*iter).m_szMessage;
				(*iter).m_szMessage = NULL;
			}

			pcsNotice = iter;

			m_vtNotice.erase(iter);
			iter = m_vtNotice.begin();

			continue;
		}

		if ((*iter).m_ulLastNoticeTime + (*iter).m_ulGap <= ulClockCount)
		{
			m_pagsmChatting->SendMessageAll(AGPDCHATTING_TYPE_WHOLE_WORLD, -1, (*iter).m_szMessage,(INT32) strlen((*iter).m_szMessage));
			(*iter).m_ulLastNoticeTime = ulClockCount;
		}

		++iter;
	}
}

void AgsmAdmin::ClearNoticeRepeat()
{
	AssmNoticeIter	iter;

	AuAutoLock lock(m_csNoticeCS);
	if (!lock.Result()) return;

	iter = m_vtNotice.begin();
	while (iter != m_vtNotice.end())
	{
		if ((*iter).m_szMessage)
		{
			delete [] (*iter).m_szMessage;
			(*iter).m_szMessage = NULL;
		}
		++iter;
	}

	m_vtNotice.clear();
}

INT32 AgsmAdmin::GetNoticeCount()
{
	AuAutoLock lock(m_csNoticeCS);
	if (!lock.Result()) return 0;

	return (INT32) m_vtNotice.size();
}

BOOL AgsmAdmin::EnumNotice(ApModuleDefaultCallBack pfCallback, PVOID pClass, PVOID pCustData)
{
	AssmNoticeIter	iter;

	AuAutoLock lock(m_csNoticeCS);
	if (!lock.Result()) return 0;

	iter = m_vtNotice.begin();
	while (iter != m_vtNotice.end())
	{
		if (pfCallback)
		{
			pfCallback(&(*iter), pClass, pCustData);
		}

		++iter;
	}

	return TRUE;
}

AssmNoticeVector *AgsmAdmin::LockNotice()
{
	m_csNoticeCS.Lock();
	return &m_vtNotice;
}

VOID AgsmAdmin::UnlockNotice()
{
	m_csNoticeCS.Unlock();
}

// 2007.07.30. steeple
BOOL AgsmAdmin::SendPing(stAgpdAdminPing& stPing, UINT32 ulNID)
{
	if(!ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmAdmin->MakePingPacket(&nPacketLength, &stPing, sizeof(stPing));
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	return bResult;
}

// 2007.07.30. steeple
BOOL AgsmAdmin::CBPingOperation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmAdmin* pThis = static_cast<AgsmAdmin*>(pClass);
	stAgpdAdminPing* pstPing = static_cast<stAgpdAdminPing*>(pData);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	pThis->ProcessPingClientReceive(*pstPing, ulNID);
	return TRUE;
}

// 2007.07.30. steeple
BOOL AgsmAdmin::CBOnConnectServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pCustData)
		return FALSE;

	AgsmAdmin* pThis = static_cast<AgsmAdmin*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	// Set 에 추가한다.
	pThis->m_setServerNID.insert(ulNID);
	return TRUE;
}

// 2007.07.30. steeple
BOOL AgsmAdmin::CBOnDisconnectServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pCustData)
		return FALSE;

	AgsmAdmin* pThis = static_cast<AgsmAdmin*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	// Set 에서 제거한다.
	pThis->m_setServerNID.erase(ulNID);
	return TRUE;
}

BOOL AgsmAdmin::CBOnAdminClientLogin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pCustData)
		return FALSE;

	AgsmAdmin* pThis = static_cast<AgsmAdmin*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pCustData);

	DPNID dpnID = pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	CHAR* szIP = pThis->GetPlayerIPAddress(dpnID);

	if(pThis->IsAllowedIP(szIP))
	{
		// 클라이언트에 접속해도 된다고 보낸다.
		pThis->SendAdminClientLoginOK(pcsCharacter, dpnID);
	}

	return TRUE;
}

BOOL AgsmAdmin::CBOnTitleEdit(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pData || !pCustData)
		return FALSE;

	AgsmAdmin* pThis = static_cast<AgsmAdmin*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pCustData);
	stAgpdAdminTitleOperation* pstTitle = static_cast<stAgpdAdminTitleOperation*>(pData);

	return pThis->ProcessTitleOperation(pstTitle, pcsCharacter);
}

// Event Item 2008.05.20. steeple
void AgsmAdmin::LoadEventItem()
{
	AuIniManagerA csIniFile;
	csIniFile.SetMode((AuIniManagerMode)APMODULE_STREAM_MODE_NAME_OVERWRITE);
	csIniFile.SetPath(AGSMADMIN_EVENT_ITEM_FILE_NAME);
	if(!csIniFile.ReadFile(0 , FALSE))
		return;

	m_vtEventItems.clear();

	INT32 lNumSections = csIniFile.GetNumSection();
	for(int i = 0; i < lNumSections; ++i)
	{
		AgsdAdminEventItem stEventItem;

		// Key - 0 ; ItemTID
		// Key - 1 ; ItemCount
		stEventItem.m_lTID = atoi(csIniFile.GetValue(i, 0, "0"));
		stEventItem.m_lCount = atoi(csIniFile.GetValue(i, 1, "0"));
		stEventItem.m_ISet = atoi(csIniFile.GetValue(i, 2, "0"));

		if(stEventItem.m_lTID > 0 && stEventItem.m_lCount > 0 && stEventItem.m_ISet > 0)
			m_vtEventItems.push_back(stEventItem);
	}
}

// Event Item 2008.05.20. steeple
BOOL AgsmAdmin::ProcessEventItem(INT32 m_ISet)
{
	if(!m_pagpmConfig)
		return FALSE;

	if(m_pagpmConfig->GetIsAdminEventItem() == FALSE)
		return FALSE;

	stAgpdAdminItemOperation stItemOperation;
	AgpdItemTemplate* pcsItemTemplate = NULL;

	// 접속해 있는 모든 캐릭터들에게 아이템을 일괄 지급한다.
	INT32 lIndex = 0;
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharSequence(&lIndex);
	while(pcsCharacter)
	{
		if(m_pagpmCharacter->IsPC(pcsCharacter) && pcsCharacter->m_szID[0] != '\0' && m_pagsmCharacter->GetCharDPNID(pcsCharacter) != 0)
		{
			memset(&stItemOperation, 0, sizeof(stItemOperation));

			AdminEventItemIter iter = m_vtEventItems.begin();
			while(iter != m_vtEventItems.end())
			{
				if(iter->m_ISet == m_ISet)
				{
					pcsItemTemplate = m_pagpmItem->GetItemTemplate(iter->m_lTID);
					if(pcsItemTemplate)
					{
						stItemOperation.m_lTID = iter->m_lTID;
						stItemOperation.m_lCount = iter->m_lCount;

						stItemOperation.m_nInUseItem = AGPDITEM_CASH_ITEM_UNUSE;
						stItemOperation.m_lRemainTime = pcsItemTemplate->m_lRemainTime;
						//stItemOperation.m_lExpireTime = pcsItemTemplate->m_lExpireTime;//JK_일괄지급버그 수정..

						ProcessItemCreate(&stItemOperation, NULL, FALSE, FALSE, pcsCharacter);
					}
				}

				++iter;
			}
		}

		pcsCharacter = m_pagpmCharacter->GetCharSequence(&lIndex);
	}

	return TRUE;
}

BOOL AgsmAdmin::LoadAllowedAdminIPList()
{
	ApModuleStream csStream;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	BOOL bRet = csStream.Open("INI\\adminClientIP.ini", 0, FALSE);

	m_csAllowedAdminIPList.clear();

	if(bRet < 0)
		return FALSE;

	while(csStream.ReadNextValue())
	{
		char tempIP[21];
		csStream.GetValue(tempIP, 21);
		m_csAllowedAdminIPList.push_back(tempIP);
	}

	return TRUE;
}

BOOL AgsmAdmin::IsAllowedIP(const char *szIPAddress)
{
	if(AuGameEnv().IsAlpha())
		return TRUE;

	if (m_csAllowedAdminIPList.size() == 0)
	{
		return TRUE;
	}

	for (int i = 0; i < m_csAllowedAdminIPList.size(); i++)
	{
		const char *szTmpIP = m_csAllowedAdminIPList[i].c_str();
		if (strncmp(szTmpIP, szIPAddress, strlen(szTmpIP)) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}