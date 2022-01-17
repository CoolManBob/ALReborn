// AgpmPvP.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2004. 12. 16.


#include "AgpmPvP.h"
#include "ApAutoLockCharacter.h"
#include "AgpmBattleGround.h"
#include "AgpmConfig.h"

//////////////////////////////////////////////////////////////////////////

AgpmPvP::AgpmPvP()
{
	SetModuleName("AgpmPvP");
	SetModuleType(APMODULE_TYPE_PUBLIC);
	
	SetModuleData(sizeof(AgpdPvPCharInfo), AGPMPVP_DATA_TYPE_CHAR_INFO);
	SetModuleData(sizeof(AgpdPvPGuildInfo), AGPMPVP_DATA_TYPE_GUILD_INFO);
	SetModuleData(sizeof(AgpdPvPAreaDrop), AGPMPVP_DATA_TYPE_AREA_DROP);
	SetModuleData(sizeof(AgpdPvPItemDrop), AGPMPVP_DATA_TYPE_ITEM_DROP);
	SetModuleData(sizeof(AgpdPvPSkullDrop), AGPMPVP_DATA_TYPE_SKULL_DROP);

	EnableIdle(FALSE);
	
	m_papmMap = NULL;
	m_pagpmCharacter = NULL;
	m_pagpmFactors = NULL;
	m_pagpmGuild = NULL;
	m_pagpmSkill = NULL;
	m_pagpmSummons = NULL;
	m_pagpmParty = NULL;
	m_pagpmBattleGround	= NULL;
	m_pagpmConfig = NULL;
	m_nIndexADCharacter = -1;

	m_lSkullDropMinLevelGap = 0;
	m_lSkullDropMaxLevelGap = 0;
    memset(&m_alSkullDropLevelBranch, 0, sizeof(CHAR) * AGPMPVP_MAX_SKULL_LEVEL_BRANCH);
	memset(&m_alSkullDropLevel, 0, sizeof(CHAR)* AGPMPVP_MAX_SKULL_LEVEL_BRANCH);
	
	m_ulLastClock = 0;

	SetPacketType(AGPMPVP_PACKET_TYPE); 

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operation
							AUTYPE_INT32,			1,	// CID
							AUTYPE_INT32,			1,	// Target CID
							AUTYPE_INT32,			1,	// Win
							AUTYPE_INT32,			1,	// Lose
							AUTYPE_INT8,			1,	// PvPMode
							AUTYPE_INT8,			1,	// PvPStatus
							AUTYPE_INT8,			1,	// Win
							AUTYPE_CHAR,			AGPMGUILD_MAX_GUILD_ID_LENGTH + 1,	// GuildID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// For System Message
							AUTYPE_END,				0
							);

	//if (g_eServiceArea == AP_SERVICE_AREA_KOREA || AP_SERVICE_AREA_JAPAN == g_eServiceArea)
	//	m_lMinPvPLevel = 30;
	//else
	//	m_lMinPvPLevel = 15;
	//if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
	//	m_lMinPvPLevel = 15;
	//else
		m_lMinPvPLevel = AGPMPVP_MINIMUM_ABLE_LEVEL;
	
	m_eRaceBattleStatus = AGPMPVP_RACE_BATTLE_STATUS_NONE;
	m_lRaceBattleWinner = AURACE_TYPE_NONE;
}

AgpmPvP::~AgpmPvP()
{
}

BOOL AgpmPvP::OnAddModule()
{
	m_papmMap = (ApmMap*)GetModule("ApmMap");
	m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagpmFactors = (AgpmFactors*)GetModule("AgpmFactors");
	m_pagpmGuild = (AgpmGuild*)GetModule("AgpmGuild");
	m_pagpmSkill = (AgpmSkill*)GetModule("AgpmSkill");
	m_pagpmSummons = (AgpmSummons*)GetModule("AgpmSummons");
	m_pagpmParty = (AgpmParty*)GetModule("AgpmParty");

	if(!m_papmMap || !m_pagpmCharacter || !m_pagpmGuild || !m_pagpmSkill || !m_pagpmSummons || !m_pagpmParty)
		return FALSE;

	m_nIndexADCharacter = m_pagpmCharacter->AttachCharacterData(this, sizeof(AgpdPvPADChar), ConAgpdPvPADChar, DesAgpdPvPADChar);
	if(m_nIndexADCharacter < 0)
		return FALSE;

	if(!m_pagpmCharacter->SetCallbackInitChar(CBInitCharacter, this))
		return FALSE;
	if(!m_pagpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;
	if(!m_pagpmCharacter->SetCallbackCheckValidNormalAttack(CBIsAttackable, this))
		return FALSE;
	if(!m_pagpmCharacter->SetCallbackBindingRegionChange(CBRegionChange, this))
		return FALSE;

	// 2005.06.01. steeple
	if(!m_pagpmCharacter->SetCallbackAddCharacterToMap(CBRegionChange, this))
		return FALSE;

	if(!m_pagpmSkill->SetCallbackCheckSkillPvP(CBIsSkillEnable, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmPvP::OnInit()
{
	m_pagpmBattleGround	= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagpmConfig = (AgpmConfig*)GetModule("AgpmConfig");
	
	if(!m_pagpmBattleGround || !m_pagpmConfig)
		return FALSE;
	
	return TRUE;
}

const INT32 AGPMPVP_INTERVAL = (1000 * 30);	// 30 seconds

BOOL AgpmPvP::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmPvP::OnDestroy()
{
	// Probability 저장해놨던 거 지운다.
	INT32 lIndex = 0;

	// Area Drop
	for(AgpdPvPAreaDrop** ppcsPvPAreaDrop = (AgpdPvPAreaDrop**)m_csAreaDropAdmin.GetObjectSequence(&lIndex); ppcsPvPAreaDrop;
			ppcsPvPAreaDrop = (AgpdPvPAreaDrop**)m_csAreaDropAdmin.GetObjectSequence(&lIndex))
	{
		DestroyModuleData((PVOID)*ppcsPvPAreaDrop, AGPMPVP_DATA_TYPE_AREA_DROP);
	}
	m_csAreaDropAdmin.RemoveObjectAll();		

	lIndex = 0;

	// Item Drop
	for(AgpdPvPItemDrop** ppcsPvPItemDrop = (AgpdPvPItemDrop**)m_csItemDropAdmin.GetObjectSequence(&lIndex); ppcsPvPItemDrop;
			ppcsPvPItemDrop = (AgpdPvPItemDrop**)m_csItemDropAdmin.GetObjectSequence(&lIndex))
	{
		DestroyModuleData((PVOID)*ppcsPvPItemDrop, AGPMPVP_DATA_TYPE_ITEM_DROP);
	}
	m_csItemDropAdmin.RemoveObjectAll();	

	lIndex = 0;

	// Skull Drop
	for(AgpdPvPSkullDrop** ppcsPvPSkullDrop = (AgpdPvPSkullDrop**)m_csSkullDropAdmin.GetObjectSequence(&lIndex); ppcsPvPSkullDrop;
			ppcsPvPSkullDrop = (AgpdPvPSkullDrop**)m_csSkullDropAdmin.GetObjectSequence(&lIndex))
	{
		DestroyModuleData((PVOID)*ppcsPvPSkullDrop, AGPMPVP_DATA_TYPE_SKULL_DROP);
	}
	m_csSkullDropAdmin.RemoveObjectAll();		
	
	return TRUE;
}

AgpdPvPADChar* AgpmPvP::GetADCharacter(AgpdCharacter* pData)
{
	AgpdPvPADChar* pcsPvPADChar = NULL;
	if(m_pagpmCharacter)
		pcsPvPADChar = (AgpdPvPADChar*)m_pagpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, (PVOID)pData);

	if(!pcsPvPADChar)
		return NULL;

	// 2005.01.12. PC 일 때는 검사를 세분화한다.
	// PC 일 때만 Valid 한 값이 리턴될 수 있다.
	if(m_pagpmCharacter && m_pagpmCharacter->IsPC(pData))
	{
		// 아직 초기화가 안된 상태라면 즉시 리턴해준다.
		if(!pcsPvPADChar->m_bInit)
			return pcsPvPADChar;

		// 같은편, 적, 같은편 길드, 적길드 리스트가 모두 Valid 할 때만 return 잘해준다.
		if(pcsPvPADChar->m_pFriendVector&& pcsPvPADChar->m_pFriendGuildVector &&
			pcsPvPADChar->m_pEnemyVector && pcsPvPADChar->m_pEnemyGuildVector)
			return pcsPvPADChar;
	}

	return NULL;
}

BOOL AgpmPvP::ConAgpdPvPADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgpmPvP::DesAgpdPvPADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgpmPvP::InitCharacter(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// PC 가 아니라면 걍 리턴
	if(!IsPC(pcsCharacter))
		return TRUE;

	// 가드라면 리턴
	if(strlen(pcsCharacter->m_szID) == 0)
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	pcsAttachedPvP->m_bInit = TRUE;

	pcsAttachedPvP->m_lWin = 0;
	pcsAttachedPvP->m_lLose = 0;
	pcsAttachedPvP->m_bDeadInCombatArea = FALSE;

	// 2007.01.30. steeple
	// AgpmPvPArray 에 문제가 있어서 vector 로 다 바꿨다.
	pcsAttachedPvP->m_pFriendVector = new PvPCharVector;
	pcsAttachedPvP->m_pEnemyVector = new PvPCharVector;
	pcsAttachedPvP->m_pFriendGuildVector = new PvPGuildVector;
	pcsAttachedPvP->m_pEnemyGuildVector = new PvPGuildVector;

	// Memory Pool 을 안쓰는 데신 서버에서는 Reserve 해놓자
	// 클라이언트는 하지 말자. -_-;
	// 고로 InitCharacter 콜백을 받아서 AgsmPvP 에서나 하자.

	InitFriendList(pcsCharacter);
	InitEnemyList(pcsCharacter);
	InitFriendGuildList(pcsCharacter);
	InitEnemyGuildList(pcsCharacter);

	return TRUE;
}

BOOL AgpmPvP::RemoveCharacter(AgpdCharacter* pcsCharacter)
{
	// PC 가 아니라면 걍 리턴
	if(!IsPC(pcsCharacter))
		return TRUE;

	// 가드라면 리턴
	if(strlen(pcsCharacter->m_szID) == 0)
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	// 초기화 안되어 있으면 걍 나간다.
	if(!pcsAttachedPvP->m_bInit)
		return TRUE;

	InitFriendList(pcsCharacter);
	InitEnemyList(pcsCharacter);
	InitFriendGuildList(pcsCharacter);
	InitEnemyGuildList(pcsCharacter);

	if(pcsAttachedPvP->m_pFriendVector)
	{
		pcsAttachedPvP->m_pFriendVector->clear();
		delete pcsAttachedPvP->m_pFriendVector;
		pcsAttachedPvP->m_pFriendVector = NULL;
	}

	if(pcsAttachedPvP->m_pEnemyVector)
	{
		pcsAttachedPvP->m_pEnemyVector->clear();
		delete pcsAttachedPvP->m_pEnemyVector;
		pcsAttachedPvP->m_pEnemyVector = NULL;
	}

	if(pcsAttachedPvP->m_pFriendGuildVector)
	{
		pcsAttachedPvP->m_pFriendGuildVector->clear();
		delete pcsAttachedPvP->m_pFriendGuildVector;
		pcsAttachedPvP->m_pFriendGuildVector = NULL;
	}

	if(pcsAttachedPvP->m_pEnemyGuildVector)
	{
		pcsAttachedPvP->m_pEnemyGuildVector->clear();
		delete pcsAttachedPvP->m_pEnemyGuildVector;
		pcsAttachedPvP->m_pEnemyGuildVector = NULL;
	}

	return TRUE;
}

BOOL AgpmPvP::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgpmPvP::OnReceive");
	
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	INT32 lCID = -1;
	INT32 lTargetCID = -1;
	INT32 lWin = -1;
	INT32 lLose = -1;
	INT8 cPvPMode = -1;
	INT8 cPvPStatus = -1;
	INT8 cWin = -1;
	CHAR* szGuildID = NULL;
	CHAR* szMessage = NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&lTargetCID,
						&lWin,
						&lLose,
						&cPvPMode,
						&cPvPStatus,
						&cWin,
						&szGuildID,
						&szMessage);

	switch(cOperation)
	{
		case AGPMPVP_PACKET_PVP_INFO:
			OnReceivePvPInfo(lCID, lTargetCID, lWin, lLose, cPvPMode, cPvPStatus, cWin);
			break;

		case AGPMPVP_PACKET_ADD_FRIEND:
			OnReceiveAddFriend(lCID, lTargetCID);
			break;

		case AGPMPVP_PACKET_ADD_ENEMY:
			OnReceiveAddEnemy(lCID, lTargetCID, cPvPMode);
			break;

		case AGPMPVP_PACKET_REMOVE_FRIEND:
			OnReceiveRemoveFriend(lCID, lTargetCID);
			break;

		case AGPMPVP_PACKET_REMOVE_ENEMY:
			OnReceiveRemoveEnemy(lCID, lTargetCID);
			break;

		case AGPMPVP_PACKET_INIT_FRIEND:
			OnReceiveInitFriend(lCID);
			break;

		case AGPMPVP_PACKET_INIT_ENEMY:
			OnReceiveInitEnemy(lCID);
			break;

		case AGPMPVP_PACKET_UPDATE_FRIEND:
			OnReceiveUpdateFriend(lCID, lTargetCID);
			break;

		case AGPMPVP_PACKET_UPDATE_ENEMY:
			OnReceiveUpdateEnemy(lCID, lTargetCID, cPvPMode);
			break;

		case AGPMPVP_PACKET_ADD_FRIEND_GUILD:
			OnReceiveAddFriendGuild(lCID, szGuildID);
			break;

		case AGPMPVP_PACKET_ADD_ENEMY_GUILD:
			OnReceiveAddEnemyGuild(lCID, szGuildID, cPvPMode);
			break;

		case AGPMPVP_PACKET_REMOVE_FRIEND_GUILD:
			OnReceiveRemoveFriendGuild(lCID, szGuildID);
			break;

		case AGPMPVP_PACKET_REMOVE_ENEMY_GUILD:
			OnReceiveRemoveEnemyGuild(lCID, szGuildID);
			break;

		case AGPMPVP_PACKET_INIT_FRIEND_GUILD:
			OnReceiveInitFriendGuild(lCID);
			break;

		case AGPMPVP_PACKET_INIT_ENEMY_GUILD:
			OnReceiveInitEnemyGuild(lCID);
			break;

		case AGPMPVP_PACKET_UPDATE_FRIEND_GUILD:
			OnReceiveUpdateFriendGuild(lCID, szGuildID);
			break;

		case AGPMPVP_PACKET_UPDATE_ENEMY_GUILD:
			OnReceiveUpdateEnemyGuild(lCID, szGuildID, cPvPMode);
			break;

		case AGPMPVP_PACKET_CANNOT_USE_TELEPORT:
			OnReceiveCannotUseTeleport(lCID);
			break;

		case AGPMPVP_PACKET_SYSTEM_MESSAGE:
			OnReceiveSystemMessage(lCID, szGuildID, szMessage, lWin, lLose);
			break;

		case AGPMPVP_PACKET_REQUEST_DEAD_TYPE:
			OnReceiveRequestDeadType(lCID, lTargetCID);
			break;

		case AGPMPVP_PACKET_RESPONSE_DEAD_TYPE:
			if (pstCheckArg->bReceivedFromServer)
				OnReceiveResponseDeadType(lCID, lTargetCID, lWin, lLose);
			break;
		
		case AGPMPVP_PACKET_RACE_BATTLE:
			if (pstCheckArg->bReceivedFromServer)
				OnReceiveRaceBattle(cPvPStatus,		// race battle status
								 lCID,				// remain time
								 lTargetCID,		// point	human
								 lWin,				//			orc
								 lLose				//			moonelf
								 );
			break;
	}

	return TRUE;
}

// 여러가지 경우에 이리로 온다.
// + 처음 로그인 시 lWin, lLose, cPvPMode, cPvPStatus 를 받아올 때.
// + 전투 지역을 들어가거나 나올 때 cPvPMode 를 받음.
// + 전투 시작하면 lTargetCID, cPvPStatus 를 받음.
// + 전투가 끝나면 lTargetCID, lWin, lLose, cPvPStatus, cWin 을 받음.
BOOL AgpmPvP::OnReceivePvPInfo(INT32 lCID, INT32 lTargetCID, INT32 lWin, INT32 lLose, INT8 cPvPMode, INT8 cPvPStatus, INT8 cWin)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(lWin != -1)
		pcsAttachedPvP->m_lWin = lWin;
	if(lLose != -1)
		pcsAttachedPvP->m_lLose = lLose;
	if(cPvPMode != -1)
		pcsAttachedPvP->m_ePvPMode = (eAgpdPvPMode)cPvPMode;
	if(cPvPStatus != -1)
		pcsAttachedPvP->m_cPvPStatus = cPvPStatus;

	EnumCallback(AGPMPVP_CB_PVP_INFO, pcsCharacter, NULL);

	// cWin 이 세팅되어 있다면 결과가 온 것이다.
	if(cWin != -1)
	{
		PVOID pBuffer[2];
		pBuffer[0] = &lTargetCID;
		pBuffer[1] = &cWin;
		EnumCallback(AGPMPVP_CB_PVP_RESULT, pcsCharacter, pBuffer);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 같은 편 추가
BOOL AgpmPvP::OnReceiveAddFriend(INT32 lCID, INT32 lTargetCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(AddFriend(pcsCharacter, lTargetCID))
		EnumCallback(AGPMPVP_CB_ADD_FRIEND, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();
	return TRUE;
}

// 적 추가
BOOL AgpmPvP::OnReceiveAddEnemy(INT32 lCID, INT32 lTargetCID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(ulLastCombatClock == 0)
		ulLastCombatClock = GetClockCount();

	if(AddEnemy(pcsCharacter, lTargetCID, cPvPMode, ulLastCombatClock))
		EnumCallback(AGPMPVP_CB_ADD_ENEMY, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 같은 편 삭제
BOOL AgpmPvP::OnReceiveRemoveFriend(INT32 lCID, INT32 lTargetCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(RemoveFriend(pcsCharacter, lTargetCID))
		EnumCallback(AGPMPVP_CB_REMOVE_FRIEND, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 적 삭제
BOOL AgpmPvP::OnReceiveRemoveEnemy(INT32 lCID, INT32 lTargetCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(RemoveEnemy(pcsCharacter, lTargetCID))
		EnumCallback(AGPMPVP_CB_REMOVE_ENEMY, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 같은 편 리스트 비우기.
// 보통 전투지역을 입장/퇴장 시 이전에 있던 것들을 비우기 위해서 받는다.
BOOL AgpmPvP::OnReceiveInitFriend(INT32 lCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(InitFriendList(pcsCharacter))
	{
		INT8 cInitType = (INT8)AGPMPVP_INIT_TYPE_FRIEND;
		EnumCallback(AGPMPVP_CB_INIT_FRIEND, pcsCharacter, &cInitType);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 적 리스트 비우기.
// 보통 전투지역을 입장/퇴장 시 이전에 있던 것들을 비우기 위해서 받는다.
BOOL AgpmPvP::OnReceiveInitEnemy(INT32 lCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(InitEnemyList(pcsCharacter))
	{
		INT8 cInitType = (INT8)AGPMPVP_INIT_TYPE_ENEMY;
		EnumCallback(AGPMPVP_CB_INIT_ENEMY, pcsCharacter, &cInitType);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 같은 편 한놈의 정보를 업데이트
BOOL AgpmPvP::OnReceiveUpdateFriend(INT32 lCID, INT32 lTargetCID)
{
	if(lCID == AP_INVALID_CID || lTargetCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(UpdateFriend(pcsCharacter, lTargetCID))
		EnumCallback(AGPMPVP_CB_UPDATE_FRIEND, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 적 한놈의 정보를 업데이트
BOOL AgpmPvP::OnReceiveUpdateEnemy(INT32 lCID, INT32 lTargetCID, INT8 cPvPMode, UINT32 ulClockCount)
{
	if(lCID == AP_INVALID_CID || lTargetCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(UpdateEnemy(pcsCharacter, lTargetCID, cPvPMode, ulClockCount))
		EnumCallback(AGPMPVP_CB_UPDATE_ENEMY, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}


BOOL AgpmPvP::OnReceiveAddFriendGuild(INT32 lCID, CHAR* szGuildID)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(AddFriendGuild(pcsCharacter, szGuildID))
		EnumCallback(AGPMPVP_CB_ADD_FRIEND_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveAddEnemyGuild(INT32 lCID, CHAR* szGuildID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(AddEnemyGuild(pcsCharacter, szGuildID, cPvPMode, ulLastCombatClock))
		EnumCallback(AGPMPVP_CB_ADD_ENEMY_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveRemoveFriendGuild(INT32 lCID, CHAR* szGuildID)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	//if(RemoveFriendGuild(pcsCharacter, szGuildID))
	// 자기 길드가 올 수도 있으므로 무조건 콜백 부른다.
	RemoveFriendGuild(pcsCharacter, szGuildID);
	EnumCallback(AGPMPVP_CB_REMOVE_FRIEND_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveRemoveEnemyGuild(INT32 lCID, CHAR* szGuildID)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(RemoveEnemyGuild(pcsCharacter, szGuildID))
		EnumCallback(AGPMPVP_CB_REMOVE_ENEMY_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveInitFriendGuild(INT32 lCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(InitFriendGuildList(pcsCharacter))
	{
		INT8 cInitType = (INT8)AGPMPVP_INIT_TYPE_FRIEND_GUILD;
		EnumCallback(AGPMPVP_CB_INIT_FRIEND_GUILD, pcsCharacter, &cInitType);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveInitEnemyGuild(INT32 lCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(InitEnemyGuildList(pcsCharacter))
	{
		INT8 cInitType = (INT8)AGPMPVP_INIT_TYPE_ENEMY_GUILD;
		EnumCallback(AGPMPVP_CB_INIT_ENEMY_GUILD, pcsCharacter, &cInitType);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveUpdateFriendGuild(INT32 lCID, CHAR* szGuildID)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(UpdateFriendGuild(pcsCharacter, szGuildID))
		EnumCallback(AGPMPVP_CB_UPDATE_FRIEND_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveUpdateEnemyGuild(INT32 lCID, CHAR* szGuildID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(UpdateEnemyGuild(pcsCharacter, szGuildID, cPvPMode, ulLastCombatClock))
		EnumCallback(AGPMPVP_CB_UPDATE_ENEMY_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveCannotUseTeleport(INT32 lCID)
{
	AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	EnumCallback(AGPMPVP_CB_RECV_CANNOT_USE_TELEPORT, pcsCharacter, NULL);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveSystemMessage(INT32 lCode, CHAR* szData1, CHAR* szData2, INT32 lData1, INT32 lData2)
{
	return ProcessSystemMessage(lCode, szData1, szData2, lData1, lData2);
}

// 2005.07.28. steeple
BOOL AgpmPvP::OnReceiveRequestDeadType(INT32 lCID, INT32 lResurrectionType)
{
	if(lCID < 1 || lResurrectionType < 0)
		return FALSE;

	EnumCallback(AGPMPVP_CB_REQUEST_DEAD_TYPE, &lCID, &lResurrectionType);
	return TRUE;
}

// 2005.07.28. steeple
BOOL AgpmPvP::OnReceiveResponseDeadType(INT32 lCID, INT32 lTargetCID, INT32 lDropExpRate, INT32 lTargetType)
{
	if(lCID < 1)
		return FALSE;

	INT32 alTmp[3];
	alTmp[0] = lTargetCID;
	alTmp[1] = lDropExpRate;
	alTmp[2] = lTargetType;

	AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if (!pcsAttachedPvP)
		return FALSE;

	pcsAttachedPvP->m_cDeadType	= (INT8) lTargetType;

	EnumCallback(AGPMPVP_CB_RESPONSE_DEAD_TYPE, pcsCharacter, alTmp);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveRaceBattle(INT8 cStatus, INT32 lRemainTime, INT32 lHumanPoint, INT32 lOrcPoint, INT32 lMoonelfPoint)
{
	INT8 cPrevStatus = m_eRaceBattleStatus;
	m_eRaceBattleStatus = (eAGPMPVP_RACE_BATTLE_STATUS) cStatus;

	m_lRaceBattlePoint[AURACE_TYPE_HUMAN]	= lHumanPoint;
	m_lRaceBattlePoint[AURACE_TYPE_ORC]		= lOrcPoint;
	m_lRaceBattlePoint[AURACE_TYPE_MOONELF]	= lMoonelfPoint;
	
	if (AGPMPVP_RACE_BATTLE_STATUS_ENDED == m_eRaceBattleStatus)
		SetRaceBattleResult();
	
	EnumCallback(AGPMPVP_CB_RACE_BATTLE_STATUS, &cPrevStatus, &lRemainTime);
	
	return TRUE;
}

// 이미 FriendList 에 대한 ApAdmin 은 메모리 생성해 놓은 상태이고,
// 리스트만 비워준다.
BOOL AgpmPvP::InitFriendList(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP || !pcsAttachedPvP->m_pFriendVector)
		return FALSE;

	PvPCharIter iter = pcsAttachedPvP->m_pFriendVector->begin();
	while(iter != pcsAttachedPvP->m_pFriendVector->end())
	{
		EnumCallback(AGPMPVP_CB_REMOVE_FRIEND, pcsCharacter, &iter->m_lCID);
		++iter;
	}

	pcsAttachedPvP->m_pFriendVector->clear();
	return TRUE;

	//INT32 lIndex = 0;
	//for(AgpdPvPCharInfo* pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex))
	//{
	//	EnumCallback(AGPMPVP_CB_REMOVE_FRIEND, pcsCharacter, &pcsPvPCharInfo->m_lCID);

	//	if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//		m_csMemoryPoolCharInfo.Free(pcsPvPCharInfo);
	//	else
	//		DestroyModuleData((PVOID)pcsPvPCharInfo, AGPMPVP_DATA_TYPE_CHAR_INFO);
	//}

	//pcsAttachedPvP->m_pFriendList->Initialize();		
	//return TRUE;
}

BOOL AgpmPvP::AddFriend(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend)
{
	if(!pcsCharacter || !pcsFriend)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return AddFriend(pcsCharacter, pcsFriend->m_lID);
}

BOOL AgpmPvP::AddFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	// 이미 있는 지 검사
	if(IsFriend(pcsCharacter, lFriendCID))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	// 최대치 검사
	// 굳이 할 필요 있을까?
	//if(pcsAttachedPvP->m_pFriendVector->size() >= AGPMPVP_MAX_FRIEND_COUNT)
	//	return FALSE;

	AgpdPvPCharInfo csPvPCharInfo;
	csPvPCharInfo.m_lCID = lFriendCID;
	pcsAttachedPvP->m_pFriendVector->push_back(csPvPCharInfo);
	return TRUE;

	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	//
	//if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//	pcsPvPCharInfo	= (AgpdPvPCharInfo *) m_csMemoryPoolCharInfo.Alloc();
	//else
	//	pcsPvPCharInfo	= (AgpdPvPCharInfo*)CreateModuleData(AGPMPVP_DATA_TYPE_CHAR_INFO);

	//if(!pcsPvPCharInfo)
	//	return FALSE;

	//pcsPvPCharInfo->m_lCID = lFriendCID;
	//pcsPvPCharInfo->m_ulLastCombatClock = 0;		// 같은 편에겐 필요없다.

	//return pcsAttachedPvP->m_pFriendList->AddObject((PVOID)pcsPvPCharInfo, lFriendCID) ? TRUE : FALSE;
}

BOOL AgpmPvP::RemoveFriend(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend)
{
	if(!pcsCharacter || !pcsFriend)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return RemoveFriend(pcsCharacter, pcsFriend->m_lID);
}

BOOL AgpmPvP::RemoveFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	//// 같은 편 아니라면 걍 나간다.
	//if(!IsFriend(pcsCharacter, lFriendCID))
	//	return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	PvPCharIter iter = std::find(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end(), lFriendCID);
	if(iter == pcsAttachedPvP->m_pFriendVector->end())
		return FALSE;

	pcsAttachedPvP->m_pFriendVector->erase(iter);
	return TRUE;

	//AgpdPvPCharInfo* pcsPvPCharInfo = GetFriendInfo(pcsCharacter, lFriendCID);
	//if(!pcsPvPCharInfo)
	//	return FALSE;

	//pcsAttachedPvP->m_pFriendList->RemoveObject(lFriendCID);

	//if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//	m_csMemoryPoolCharInfo.Free((PVOID)pcsPvPCharInfo);
	//else
	//	DestroyModuleData((PVOID)pcsPvPCharInfo, AGPMPVP_DATA_TYPE_CHAR_INFO);
	
	//return TRUE;
}

inline BOOL AgpmPvP::UpdateFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPCharInfo* pcsCharInfo = GetFriendInfo(pcsCharacter, lFriendCID);
	if(!pcsCharInfo)
		return FALSE;

	// 같은 편 정보는 별달리 할 게 없다.
	return TRUE;
}

inline BOOL AgpmPvP::IsFriend(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend)
{
	if(!pcsCharacter || !pcsFriend)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return IsFriend(pcsCharacter, pcsFriend->m_lID);
}

inline BOOL AgpmPvP::IsFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return GetFriendInfo(pcsCharacter, lFriendCID) ? TRUE : FALSE;
}

inline AgpdPvPCharInfo* AgpmPvP::GetFriendInfo(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend)
{
	if(!pcsCharacter || !pcsFriend)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return GetFriendInfo(pcsCharacter, pcsFriend->m_lID);
}

// 그냥 구조체를 반환하고 싶으나...
// 일단은 기존의 함수를 그대로 따라간다.
inline AgpdPvPCharInfo* AgpmPvP::GetFriendInfo(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return NULL;

	if(!IsPC(pcsCharacter))
		return NULL;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return NULL;

	PvPCharIter iter = std::find(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end(), lFriendCID);
	if(iter == pcsAttachedPvP->m_pFriendVector->end())
		return NULL;

	return &(*iter);
	//return (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObject(lFriendCID);
}

// 이미 EnemyList 에 대한 ApAdmin 은 메모리 생성해 놓은 상태이고,
// 리스트만 비워준다.
BOOL AgpmPvP::InitEnemyList(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP || !pcsAttachedPvP->m_pEnemyVector)
		return FALSE;
	
	PvPCharIter iter = pcsAttachedPvP->m_pEnemyVector->begin();
	while(iter != pcsAttachedPvP->m_pEnemyVector->end())
	{
		EnumCallback(AGPMPVP_CB_REMOVE_ENEMY, pcsCharacter, &iter->m_lCID);
		++iter;
	}

	pcsAttachedPvP->m_pEnemyVector->clear();
	return TRUE;

	//INT32 lIndex = 0;
	//for(AgpdPvPCharInfo* pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObjectSequence(&lIndex))
	//{
	//	EnumCallback(AGPMPVP_CB_REMOVE_ENEMY, pcsCharacter, &pcsPvPCharInfo->m_lCID);

	//	if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//		m_csMemoryPoolCharInfo.Free((PVOID)pcsPvPCharInfo);
	//	else
	//		DestroyModuleData((PVOID)pcsPvPCharInfo, AGPMPVP_DATA_TYPE_CHAR_INFO);
	//}

	//pcsAttachedPvP->m_pEnemyList->Initialize();		
	//return TRUE;
}

BOOL AgpmPvP::AddEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(!pcsCharacter || !pcsEnemy)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return AddEnemy(pcsCharacter, pcsEnemy->m_lID, ulLastCombatClock);
}

BOOL AgpmPvP::AddEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	// 이미 있는 지 검사
	if(IsEnemy(pcsCharacter, lEnemyCID))
	{
		UpdateEnemy(pcsCharacter, lEnemyCID, cPvPMode, ulLastCombatClock);
		return FALSE;
	}

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	AgpdPvPCharInfo csPvPCharInfo;
	csPvPCharInfo.m_lCID = lEnemyCID;
	csPvPCharInfo.m_ePvPMode = cPvPMode == -1 ? pcsAttachedPvP->m_ePvPMode : (eAgpdPvPMode)cPvPMode;
	csPvPCharInfo.m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;

	pcsAttachedPvP->m_pEnemyVector->push_back(csPvPCharInfo);
	return TRUE;

	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	//
	//if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//	pcsPvPCharInfo	= (AgpdPvPCharInfo *) m_csMemoryPoolCharInfo.Alloc();
	//else
 //       pcsPvPCharInfo	= (AgpdPvPCharInfo*)CreateModuleData(AGPMPVP_DATA_TYPE_CHAR_INFO);

	//if(!pcsPvPCharInfo)
	//	return FALSE;

	//pcsPvPCharInfo->m_lCID = lEnemyCID;
	//pcsPvPCharInfo->m_ePvPMode = cPvPMode == -1 ? pcsAttachedPvP->m_ePvPMode : (eAgpdPvPMode)cPvPMode;
	//pcsPvPCharInfo->m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;

	//return pcsAttachedPvP->m_pEnemyList->AddObject((PVOID)pcsPvPCharInfo, lEnemyCID) ? TRUE : FALSE;
}

BOOL AgpmPvP::RemoveEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy)
{
	if(!pcsCharacter || !pcsEnemy)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return RemoveEnemy(pcsCharacter, pcsEnemy->m_lID);
}

BOOL AgpmPvP::RemoveEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	PvPCharIter iter = std::find(pcsAttachedPvP->m_pEnemyVector->begin(), pcsAttachedPvP->m_pEnemyVector->end(), lEnemyCID);
	if(iter == pcsAttachedPvP->m_pEnemyVector->end())
		return FALSE;

	pcsAttachedPvP->m_pEnemyVector->erase(iter);
	return TRUE;

	//AgpdPvPCharInfo* pcsPvPCharInfo = GetEnemyInfo(pcsCharacter, lEnemyCID);
	//if(!pcsPvPCharInfo)
	//	return FALSE;

	//pcsAttachedPvP->m_pEnemyList->RemoveObject(lEnemyCID);

	//if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//	m_csMemoryPoolCharInfo.Free((PVOID)pcsPvPCharInfo);
	//else
	//	DestroyModuleData(pcsPvPCharInfo, AGPMPVP_DATA_TYPE_CHAR_INFO);

	//return TRUE;
}

inline BOOL AgpmPvP::UpdateEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPCharInfo* pcsCharInfo = GetEnemyInfo(pcsCharacter, lEnemyCID);
	if(!pcsCharInfo)
		return FALSE;

	if(cPvPMode != -1)
		pcsCharInfo->m_ePvPMode = (eAgpdPvPMode)cPvPMode;
	pcsCharInfo->m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;
	return TRUE;
}

BOOL AgpmPvP::IsEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy)
{
	if(!pcsCharacter || !pcsEnemy)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return IsEnemy(pcsCharacter, pcsEnemy->m_lID);
}

BOOL AgpmPvP::IsEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return GetEnemyInfo(pcsCharacter, lEnemyCID) ? TRUE : FALSE;
}

inline AgpdPvPCharInfo* AgpmPvP::GetEnemyInfo(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy)
{
	if(!pcsCharacter || !pcsEnemy)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return GetEnemyInfo(pcsCharacter, pcsEnemy->m_lID);
}

inline AgpdPvPCharInfo* AgpmPvP::GetEnemyInfo(AgpdCharacter* pcsCharacter, INT32 lEnemyCID)
{
	if(!pcsCharacter || !lEnemyCID)
		return NULL;

	if(!IsPC(pcsCharacter))
		return NULL;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return NULL;

	PvPCharIter iter = std::find(pcsAttachedPvP->m_pEnemyVector->begin(), pcsAttachedPvP->m_pEnemyVector->end(), lEnemyCID);
	if(iter == pcsAttachedPvP->m_pEnemyVector->end())
		return NULL;

	return &(*iter);
	//return (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObject(lEnemyCID);
}

BOOL AgpmPvP::InitFriendGuildList(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP || !pcsAttachedPvP->m_pFriendGuildVector)
		return FALSE;

	PvPGuildIter iter = pcsAttachedPvP->m_pFriendGuildVector->begin();
	while(iter != pcsAttachedPvP->m_pFriendGuildVector->end())
	{
		EnumCallback(AGPMPVP_CB_REMOVE_FRIEND_GUILD, pcsCharacter, iter->m_szGuildID);
		++iter;
	}

	pcsAttachedPvP->m_pFriendGuildVector->clear();
	return TRUE;

	//INT32 lIndex = 0;
	//for(AgpdPvPGuildInfo* pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex))
	//{
	//	EnumCallback(AGPMPVP_CB_REMOVE_FRIEND_GUILD, pcsCharacter, pcsPvPGuildInfo->m_szGuildID);

	//	if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//		m_csMemoryPoolGuildInfo.Free(pcsPvPGuildInfo);
	//	else
	//		DestroyModuleData(pcsPvPGuildInfo, AGPMPVP_DATA_TYPE_GUILD_INFO);
	//}

	//pcsAttachedPvP->m_pFriendGuildList->Initialize();		
	//return TRUE;
}

BOOL AgpmPvP::AddFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	if(IsFriendGuild(pcsCharacter, szGuildID))
		return FALSE;

	AgpdPvPGuildInfo csGuildInfo;
	_tcscpy(csGuildInfo.m_szGuildID, szGuildID);
	csGuildInfo.m_ulLastCombatClock = GetClockCount();

	pcsAttachedPvP->m_pFriendGuildVector->push_back(csGuildInfo);
	return TRUE;

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	//
	//if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//	pcsPvPGuildInfo	= (AgpdPvPGuildInfo *)	m_csMemoryPoolGuildInfo.Alloc();
	//else
	//	pcsPvPGuildInfo	= (AgpdPvPGuildInfo*)CreateModuleData(AGPMPVP_DATA_TYPE_GUILD_INFO);

	//if(!pcsPvPGuildInfo)
	//	return FALSE;

	//strcpy(pcsPvPGuildInfo->m_szGuildID, szGuildID);
	//pcsPvPGuildInfo->m_ulLastCombatClock = GetClockCount();
	//pcsAttachedPvP->m_pFriendGuildList->AddObject((PVOID)pcsPvPGuildInfo, pcsPvPGuildInfo->m_szGuildID);
 //   
	//return TRUE;
}

BOOL AgpmPvP::RemoveFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	PvPGuildIter iter = std::find(pcsAttachedPvP->m_pFriendGuildVector->begin(), pcsAttachedPvP->m_pFriendGuildVector->end(), szGuildID);
	if(iter == pcsAttachedPvP->m_pFriendGuildVector->end())
		return FALSE;

	pcsAttachedPvP->m_pFriendGuildVector->erase(iter);
	return TRUE;

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = GetFriendGuildInfo(pcsCharacter, szGuildID);
	//if(!pcsPvPGuildInfo)
	//	return FALSE;

	//pcsAttachedPvP->m_pFriendGuildList->RemoveObject(szGuildID);

	//if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//	m_csMemoryPoolGuildInfo.Free(pcsPvPGuildInfo);
	//else
	//	DestroyModuleData(pcsPvPGuildInfo, AGPMPVP_DATA_TYPE_GUILD_INFO);

	//return TRUE;
}

inline BOOL AgpmPvP::UpdateFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	AgpdPvPGuildInfo* pcsPvPGuildInfo = GetFriendGuildInfo(pcsCharacter, szGuildID);
	if(!pcsPvPGuildInfo)
		return FALSE;

	// Update 가 날라오면 현재(2005.01.12) 로서는 마지막 CombatClock 만 업데이트 해주면 된다.
	pcsPvPGuildInfo->m_ulLastCombatClock = GetClockCount();

	return TRUE;
}

inline BOOL AgpmPvP::IsFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	// 자기 길드 체크는 하지 않는다. -0-;;; 2005.01.19. steeple
	// 자기 길드이면 return TRUE
	//CHAR* szSelfGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	//if(szSelfGuildID && strlen(szSelfGuildID) > 0 && strcmp(szSelfGuildID, szGuildID) == 0)
	//	return TRUE;

	return GetFriendGuildInfo(pcsCharacter, szGuildID) ? TRUE : FALSE;
}

inline AgpdPvPGuildInfo* AgpmPvP::GetFriendGuildInfo(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return NULL;

	if(!IsPC(pcsCharacter))
		return NULL;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return NULL;

	PvPGuildIter iter = std::find(pcsAttachedPvP->m_pFriendGuildVector->begin(), pcsAttachedPvP->m_pFriendGuildVector->end(), szGuildID);
	if(iter == pcsAttachedPvP->m_pFriendGuildVector->end())
		return NULL;

	return &(*iter);

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObject(szGuildID);
	//return pcsPvPGuildInfo;
}

BOOL AgpmPvP::InitEnemyGuildList(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP || !pcsAttachedPvP->m_pEnemyGuildVector)
		return FALSE;

	// 2005.04.18. steeple
	// 길드전 중일때는 길드적을 빼면 안된다.
	CHAR* szJoinedGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);

	PvPGuildIter iter = pcsAttachedPvP->m_pEnemyGuildVector->begin();
	while(iter != pcsAttachedPvP->m_pEnemyGuildVector->end())
	{
		// 2005.04.18. steeple
		// 길드전 중일때는 길드적을 빼면 안된다.
		if(szJoinedGuildID && m_pagpmGuild->IsEnemyGuild(szJoinedGuildID, iter->m_szGuildID))
		{
			++iter;
			continue;
		}

		EnumCallback(AGPMPVP_CB_REMOVE_ENEMY_GUILD, pcsCharacter, iter->m_szGuildID);

		iter = pcsAttachedPvP->m_pEnemyGuildVector->erase(iter);
	}

	// 얘는 vector clear 하지 않는다.
	return TRUE;

	//INT32 lIndex = 0;
	//for(AgpdPvPGuildInfo* pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex))
	//{
	//	// 2005.04.18. steeple
	//	// 길드전 중일때는 길드적을 빼면 안된다.
	//	if(szJoinedGuildID && m_pagpmGuild->IsEnemyGuild(szJoinedGuildID, pcsPvPGuildInfo->m_szGuildID))
	//		continue;

	//	EnumCallback(AGPMPVP_CB_REMOVE_ENEMY_GUILD, pcsCharacter, pcsPvPGuildInfo->m_szGuildID);

	//	// 2005.04.18. steeple
	//	// Remove 를 해준다.
	//	pcsAttachedPvP->m_pEnemyGuildList->RemoveObject(pcsPvPGuildInfo->m_szGuildID);
	//	//lIndex--;		// 이거 없으면 계속 건너뛰게 됨.

	//	if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//		m_csMemoryPoolGuildInfo.Free(pcsPvPGuildInfo);
	//	else
	//		DestroyModuleData(pcsPvPGuildInfo, AGPMPVP_DATA_TYPE_GUILD_INFO);

	//}

	//// 2005.04.18. steeple
	//// 이렇게 하면 안되고
	////pcsAttachedPvP->m_pEnemyGuildList->Initialize();		
	//return TRUE;
}

BOOL AgpmPvP::AddEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	if(IsEnemyGuild(pcsCharacter, szGuildID))
	{
		UpdateEnemyGuild(pcsCharacter, szGuildID, cPvPMode, ulLastCombatClock);
		return TRUE;
	}

	AgpdPvPGuildInfo csPvPGuildInfo;
	_tcscpy(csPvPGuildInfo.m_szGuildID, szGuildID);
	csPvPGuildInfo.m_ePvPMode = cPvPMode == -1 ? pcsAttachedPvP->m_ePvPMode : (eAgpdPvPMode)cPvPMode;
	csPvPGuildInfo.m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;

	pcsAttachedPvP->m_pEnemyGuildVector->push_back(csPvPGuildInfo);
	return TRUE;

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	//
	//if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//	pcsPvPGuildInfo	= (AgpdPvPGuildInfo *) m_csMemoryPoolGuildInfo.Alloc();
	//else
	//	pcsPvPGuildInfo	= (AgpdPvPGuildInfo*)CreateModuleData(AGPMPVP_DATA_TYPE_GUILD_INFO);

	//if(!pcsPvPGuildInfo)
	//	return FALSE;

	//strcpy(pcsPvPGuildInfo->m_szGuildID, szGuildID);
	//pcsPvPGuildInfo->m_ePvPMode = cPvPMode == -1 ? pcsAttachedPvP->m_ePvPMode : (eAgpdPvPMode)cPvPMode;
	//pcsPvPGuildInfo->m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;
	//pcsAttachedPvP->m_pEnemyGuildList->AddObject((PVOID)pcsPvPGuildInfo, pcsPvPGuildInfo->m_szGuildID);
 
	//return TRUE;
}

BOOL AgpmPvP::RemoveEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	// 2007.01.04. steeple
	// 길드전 중이라면 지우지 않는다.
	CHAR* szJoinedGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	if(szJoinedGuildID && m_pagpmGuild->IsEnemyGuild(szJoinedGuildID, szGuildID, TRUE))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	PvPGuildIter iter = std::find(pcsAttachedPvP->m_pEnemyGuildVector->begin(), pcsAttachedPvP->m_pEnemyGuildVector->end(), szGuildID);
	if(iter == pcsAttachedPvP->m_pEnemyGuildVector->end())
		return FALSE;

	pcsAttachedPvP->m_pEnemyGuildVector->erase(iter);
	return TRUE;

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = GetEnemyGuildInfo(pcsCharacter, szGuildID);
	//if(!pcsPvPGuildInfo)
	//	return FALSE;

	//pcsAttachedPvP->m_pEnemyGuildList->RemoveObject(szGuildID);

	//if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//	m_csMemoryPoolGuildInfo.Free(pcsPvPGuildInfo);
	//else
	//	DestroyModuleData(pcsPvPGuildInfo, AGPMPVP_DATA_TYPE_GUILD_INFO);

	//return TRUE;
}

inline BOOL AgpmPvP::UpdateEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(!pcsCharacter || !szGuildID)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	AgpdPvPGuildInfo* pcsPvPGuildInfo = GetEnemyGuildInfo(pcsCharacter, szGuildID);
	if(!pcsPvPGuildInfo)
		return FALSE;

	if(cPvPMode != -1)
		pcsPvPGuildInfo->m_ePvPMode = (eAgpdPvPMode)cPvPMode;
	pcsPvPGuildInfo->m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;

	return TRUE;
}

inline BOOL AgpmPvP::IsEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return GetEnemyGuildInfo(pcsCharacter, szGuildID) ? TRUE : FALSE;
}

inline AgpdPvPGuildInfo* AgpmPvP::GetEnemyGuildInfo(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return NULL;

	if(!IsPC(pcsCharacter))
		return NULL;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return NULL;

	PvPGuildIter iter = std::find(pcsAttachedPvP->m_pEnemyGuildVector->begin(), pcsAttachedPvP->m_pEnemyGuildVector->end(), szGuildID);
	if(iter == pcsAttachedPvP->m_pEnemyGuildVector->end())
		return NULL;

	return &(*iter);

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObject(szGuildID);
	//return pcsPvPGuildInfo;
}







//////////////////////////////////////////////////////////////////////////
// Drop Probability

// pcsCharacter 가 이겼다. pcsTarget 의 확률을 체크한다.
INT32 AgpmPvP::GetItemDropProbability(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return -1;

	if(!IsPC(pcsCharacter) || !IsPC(pcsTarget))
		return -1;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	AgpdPvPADChar* pcsAttachedTarget = GetADCharacter(pcsTarget);
	if(!pcsAttachedPvP || !pcsAttachedTarget)
		return -1;

	INT32 lProbability = -1;
	INT32 lRand = 0;

	// 먼저 지역에 따른 확률을 구한다.
	switch(pcsAttachedPvP->m_ePvPMode)
	{
		case AGPDPVP_MODE_SAFE:
			break;

		case AGPDPVP_MODE_FREE:
		case AGPDPVP_MODE_COMBAT:
		{
			AgpdPvPAreaDrop* pcsPvPAreaDrop = GetAreaDrop(pcsAttachedPvP->m_ePvPMode);
			if(pcsPvPAreaDrop)
			{
				lRand = m_csRandom.randInt(100);
				if(lRand < pcsPvPAreaDrop->m_nAreaProbability)	// 먼저 지역 확률 체크
				{
					// Equip 인지 Inven 인지 결정
					lRand = m_csRandom.randInt(100);
					if(lRand < pcsPvPAreaDrop->m_nEquipProbability)	// Equip
						lProbability = AGPMPVP_ITEM_DROP_POS_EQUIP;
					else if(lRand < (pcsPvPAreaDrop->m_nEquipProbability + pcsPvPAreaDrop->m_nInvenProbability))	// Inven
						lProbability = AGPMPVP_ITEM_DROP_POS_INVEN;
				}
			}
			break;
		}
	}

	// Drop 안하면 리턴
	if(lProbability < 0)
		return -1;

	// 아이템 Drop 할 Equip 또는 Inven 의 위치 확률 계산
	lRand = m_csRandom.randInt(100);
	AgpdPvPItemDrop** ppcsPvPItemDrop = NULL;
	AgpdPvPItemDrop* pcsPvPItemDrop = NULL;
	INT32 lIndex = 0, lPrevProbabilitySum = 0;
	for(ppcsPvPItemDrop = (AgpdPvPItemDrop**)m_csItemDropAdmin.GetObjectSequence(&lIndex); ppcsPvPItemDrop;
		ppcsPvPItemDrop = (AgpdPvPItemDrop**)m_csItemDropAdmin.GetObjectSequence(&lIndex))
	{
		pcsPvPItemDrop = *ppcsPvPItemDrop;
		if(!pcsPvPItemDrop)
			break;

		if(lProbability & AGPMPVP_ITEM_DROP_POS_EQUIP)		// Equip 일 때
		{
			if(pcsPvPItemDrop->m_nEquipProbability == 0)
				continue;

			if(lRand < (lPrevProbabilitySum + pcsPvPItemDrop->m_nEquipProbability))
			{
				lProbability |= pcsPvPItemDrop->m_lIndex;
				break;
			}

			// 확률에 안 걸렸으면
			lPrevProbabilitySum += pcsPvPItemDrop->m_nEquipProbability;
		}
		else if(lProbability & AGPMPVP_ITEM_DROP_POS_INVEN)	// Inven 일 때
		{
			if(pcsPvPItemDrop->m_nInvenProbability == 0)
				continue;

			if(lRand < (lPrevProbabilitySum + pcsPvPItemDrop->m_nInvenProbability))
			{
				lProbability |= pcsPvPItemDrop->m_lIndex;
				break;
			}

			// 확률에 안 걸렸으면
			lPrevProbabilitySum += pcsPvPItemDrop->m_nInvenProbability;
		}
	}

	return lProbability;
}

// pcsCharacter 가 이겼다. 둘의 레벨 차이로 확률 계산 후 해골 레벨을 리턴.
INT32 AgpmPvP::GetSkullDropLevel(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!m_pagpmFactors)
		return -1;

	if(!pcsCharacter || !pcsTarget)
		return -1;

	if(!IsPC(pcsCharacter) || !IsPC(pcsTarget))
		return -1;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	AgpdPvPADChar* pcsAttachedTarget = GetADCharacter(pcsTarget);
	if(!pcsAttachedPvP || !pcsAttachedTarget)
		return -1;

	// 먼저 양 캐릭터의 레벨을 구한다.
	INT32 lLevel, lTargetLevel;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	m_pagpmFactors->GetValue(&pcsTarget->m_csFactor, &lTargetLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	// 레벨 차이를 구하고
	INT32 lLevelGap = lLevel - lTargetLevel;
	if(lLevelGap < m_lSkullDropMinLevelGap)
		lLevelGap = m_lSkullDropMinLevelGap;
	if(lLevelGap > m_lSkullDropMaxLevelGap)
		lLevelGap = m_lSkullDropMaxLevelGap;

	AgpdPvPSkullDrop* pcsPvPSkullDrop = GetSkullDrop(lLevelGap);
	if(!pcsPvPSkullDrop)
		return -1;

	INT32 lRand = m_csRandom.randInt(100);
	if(lRand >= pcsPvPSkullDrop->m_lSkullProbability)
		return -1;

	INT32 lSkullLevel = -1;
	// 진사람의 레벨을 검사해서 얻는다.
	for(int i = 0; i < AGPMPVP_MAX_SKULL_LEVEL_BRANCH; i++)
	{
		if(lTargetLevel <= m_alSkullDropLevelBranch[i])
		{
			lSkullLevel = m_alSkullDropLevel[i];
			break;
		}
	}

	return lSkullLevel;
}

AgpdPvPAreaDrop* AgpmPvP::GetAreaDrop(eAgpdPvPMode ePvPMode)
{
	AgpdPvPAreaDrop** ppcsAreaDrop = (AgpdPvPAreaDrop**)m_csAreaDropAdmin.GetObject((INT32)ePvPMode);
	if(ppcsAreaDrop && *ppcsAreaDrop)
		return *ppcsAreaDrop;
	else
		return NULL;
}

AgpdPvPItemDrop* AgpmPvP::GetItemDrop(INT32 lIndex)
{
	AgpdPvPItemDrop** ppcsItemDrop = (AgpdPvPItemDrop**)m_csItemDropAdmin.GetObject(lIndex);
	if(ppcsItemDrop && *ppcsItemDrop)
		return *ppcsItemDrop;
	else
		return NULL;
}

AgpdPvPSkullDrop* AgpmPvP::GetSkullDrop(INT32 lLevelGap)
{
	if(lLevelGap == 0)
		lLevelGap = AGPMPVP_LEVEL_GAP_ZERO;

	AgpdPvPSkullDrop** ppcsSkullDrop = (AgpdPvPSkullDrop**)m_csSkullDropAdmin.GetObject(lLevelGap);
	if(ppcsSkullDrop && *ppcsSkullDrop)
		return *ppcsSkullDrop;
	else
		return NULL;
}

// Region 이 바뀔 때 불린다.
BOOL AgpmPvP::RegionChange(AgpdCharacter* pcsCharacter, INT16 nPrevRegionIndex)
{
	if(!pcsCharacter)
		return FALSE;

	// 몬스터일 때는 나간다.
	if(!m_pagpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	// 가드일때도 나간다.
	if(strlen(pcsCharacter->m_szID) == 0)
		return TRUE;

    AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	eAgpdPvPMode ePrevPvPMode = AGPDPVP_MODE_NONE;

	// 2005.06.01. steeple
	// ADD_CHARACTER_TO_MAP 에서 불렸다면, 새 RegionIndex 를 세팅해준다.
	// 그리고 이전 PvPMode 는 AttachedPvP 데이터에서 가져온다.
	if(nPrevRegionIndex == -1)
	{
		//pcsCharacter->m_nBindingRegionIndex = m_papmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);
		ePrevPvPMode = pcsAttachedPvP->m_ePvPMode;
	}
	else
		ePrevPvPMode = GetPvPModeByRegion(nPrevRegionIndex);

	eAgpdPvPMode eNowPvPMode = GetPvPModeByPosition(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);

	// 그리고 새로운 놈 대입
	pcsAttachedPvP->m_ePvPMode = eNowPvPMode;
	
	if(eNowPvPMode == AGPDPVP_MODE_SAFE && ePrevPvPMode != AGPDPVP_MODE_SAFE)
	{
		// 안전지역에 들어갔음
		EnumCallback(AGPMPVP_CB_ENTER_SAFE_PVP_AREA, pcsCharacter, NULL);
	}
	
	if(eNowPvPMode != AGPDPVP_MODE_SAFE && ePrevPvPMode == AGPDPVP_MODE_SAFE)
	{
		// 안전지역에서 나왔음
		EnumCallback(AGPMPVP_CB_LEAVE_SAFE_PVP_AREA, pcsCharacter, NULL);
	}
	
	if(eNowPvPMode == AGPDPVP_MODE_FREE && ePrevPvPMode != AGPDPVP_MODE_FREE)
	{
		// 자유지역에 들어갔음
		EnumCallback(AGPMPVP_CB_ENTER_FREE_PVP_AREA, pcsCharacter, NULL);
	}
	
	if(eNowPvPMode != AGPDPVP_MODE_FREE && ePrevPvPMode == AGPDPVP_MODE_FREE)
	{
		// 자유지역에서 나왔음
		EnumCallback(AGPMPVP_CB_LEAVE_FREE_PVP_AREA, pcsCharacter, NULL);
	}
	
	if(eNowPvPMode == AGPDPVP_MODE_COMBAT && ePrevPvPMode != AGPDPVP_MODE_COMBAT)
	{
		// 전투지역에 들어갔음
		EnumCallback(AGPMPVP_CB_ENTER_COMBAT_PVP_AREA, pcsCharacter, NULL);
	}
	
	if(eNowPvPMode != AGPDPVP_MODE_COMBAT && ePrevPvPMode == AGPDPVP_MODE_COMBAT)
	{
		// 전투지역에서 나왔음
		EnumCallback(AGPMPVP_CB_LEAVE_COMBAT_PVP_AREA, pcsCharacter, NULL);
	}

	return TRUE;
}

// 현재 모드를 비교해서 리턴한다.
inline BOOL AgpmPvP::IsSafePvPMode(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	return pcsAttachedPvP->m_ePvPMode == AGPDPVP_MODE_SAFE ? TRUE : FALSE;
}

// 현재 모드를 비교해서 리턴한다.
inline BOOL AgpmPvP::IsFreePvPMode(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	return pcsAttachedPvP->m_ePvPMode == AGPDPVP_MODE_FREE ? TRUE : FALSE;
}
// 현재 모드를 비교해서 리턴한다.
inline BOOL AgpmPvP::IsCombatPvPMode(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	return pcsAttachedPvP->m_ePvPMode == AGPDPVP_MODE_COMBAT ? TRUE : FALSE;
}

// X, Z 좌표로 현재 리전을 얻어서 PvP 모드를 리턴한다.
inline eAgpdPvPMode AgpmPvP::GetPvPModeByPosition(FLOAT fX, FLOAT fZ)
{
	INT16 nRegionIndex = m_papmMap->GetRegion(fX, fZ);
	return GetPvPModeByRegion(nRegionIndex);
}

// Region Index 로부터 현재 리전을 얻어서 PvP 모드를 리턴한다.
inline eAgpdPvPMode AgpmPvP::GetPvPModeByRegion(INT16 nRegionIndex)
{
	ApmMap::RegionTemplate* pRegionTemplate = m_papmMap->GetTemplate(nRegionIndex);
	if(!pRegionTemplate)
		return AGPDPVP_MODE_NONE;

	switch(pRegionTemplate->ti.stType.uSafetyType)
	{
		case ApmMap::ST_SAFE:
			return AGPDPVP_MODE_SAFE;
		case ApmMap::ST_FREE:
			return AGPDPVP_MODE_FREE;
		case ApmMap::ST_DANGER:
			return AGPDPVP_MODE_COMBAT;
	}

	return AGPDPVP_MODE_NONE;
}

// 현재 PvP 하고 있는지 리턴
BOOL AgpmPvP::IsNowPvP(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	// Status 가 NOWPVP 이면
	if(pcsAttachedPvP->m_cPvPStatus & AGPDPVP_STATUS_NOWPVP)
		return TRUE;

	// 적이 있으면
	if(!pcsAttachedPvP->m_pEnemyVector->empty() || !pcsAttachedPvP->m_pEnemyGuildVector->empty())
		return TRUE;

	return FALSE;
}

// 현재 무적 상태인지 리턴
inline BOOL AgpmPvP::IsInvincible(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	if(pcsAttachedPvP->m_cPvPStatus & AGPDPVP_STATUS_INVINCIBLE)
		return TRUE;

	return FALSE;
}

// PvP 상태를 새로 체크한다.
BOOL AgpmPvP::UpdatePvPStatus(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	// 적이 있으면
	if(!pcsAttachedPvP->m_pEnemyVector->empty() || !pcsAttachedPvP->m_pEnemyGuildVector->empty())
		pcsAttachedPvP->m_cPvPStatus |= AGPDPVP_STATUS_NOWPVP;
	else
	{
		if(pcsAttachedPvP->m_cPvPStatus & AGPDPVP_STATUS_NOWPVP)
			pcsAttachedPvP->m_cPvPStatus -= AGPDPVP_STATUS_NOWPVP;
	}

	return TRUE;
}

// CombatArea 에서 죽었으면 세팅한다.
BOOL AgpmPvP::SetDeadInCombatArea(AgpdCharacter* pcsCharacter, BOOL bDead)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	pcsAttachedPvP->m_bDeadInCombatArea = bDead;

	return TRUE;
}


eAGPMPVP_RACE_BATTLE_STATUS AgpmPvP::GetRaceBattleStatus()
{
	return m_eRaceBattleStatus;
}

void AgpmPvP::SetRaceBattleStatus(eAGPMPVP_RACE_BATTLE_STATUS eStatus)
{
	m_eRaceBattleStatus = eStatus;
}

BOOL AgpmPvP::IsInRaceBattle()
{
	eAGPMPVP_RACE_BATTLE_STATUS eStatus = GetRaceBattleStatus();
	if (AGPMPVP_RACE_BATTLE_STATUS_ING == eStatus)
		return TRUE;
	
	return FALSE;
}

BOOL AgpmPvP::IsInRaceBattleReward()
{
	eAGPMPVP_RACE_BATTLE_STATUS eStatus = GetRaceBattleStatus();
	if (eStatus == AGPMPVP_RACE_BATTLE_STATUS_REWARD)
		return TRUE;
	
	return FALSE;
}


void AgpmPvP::SetRaceBattleResult()
{
	INT32 lRace = AURACE_TYPE_NONE;
	INT32 lMax = 0;
	for (INT32 i = AURACE_TYPE_HUMAN; i < AURACE_TYPE_MAX; i++)
	{
		if (m_lRaceBattlePoint[i] > lMax)
		{
			lMax = m_lRaceBattlePoint[i];
			lRace = i;
		}
	}
	m_lRaceBattleWinner = lRace;
}


INT32 AgpmPvP::GetRaceBattleWinner()
{
	if (IsInRaceBattleReward())
		return m_lRaceBattleWinner;
	
	return AURACE_TYPE_NONE;
}




//////////////////////////////////////////////////////////////////////////
// Etc
inline BOOL AgpmPvP::IsPC(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(m_pagpmCharacter->IsPC(pcsCharacter) &&
		strlen(pcsCharacter->m_szID) > 0)	// 이름 체크 여기다 걍 넣어버렸다. 2005.09.30. steeple
		return TRUE;
	else
		return FALSE;
}

// pcsTarget 이 pcsCharacter 와 같은편인지 확인. 일발+길드
inline BOOL AgpmPvP::IsFriendCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// 둘다 PC 여야 함
	if(!IsPC(pcsCharacter) || !IsPC(pcsTarget))
		return FALSE;

	if(IsFriend(pcsCharacter, pcsTarget->m_lID))
		return TRUE;

	CHAR* szGuildID1 = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	CHAR* szGuildID2 = m_pagpmGuild->GetJoinedGuildID(pcsTarget);
	if(szGuildID1 && szGuildID2 &&
		strlen(szGuildID1) > 0 && strlen(szGuildID2) > 0 && strcmp(szGuildID1, szGuildID2) == 0)
		return TRUE;

	if(szGuildID2 && strlen(szGuildID2) > 0 && IsFriendGuild(pcsCharacter, szGuildID2))
		return TRUE;

	//if(szGuildID1 && szGuildID2 && _tcslen(szGuildID1) > 0 && _tcslen(szGuildID2) > 0)
	//{
	//	// 연대길드 끼리도 길드전 할 수 있다.
 //       if(m_pagpmGuild->IsJointGuild(szGuildID1, szGuildID2))
	//		return TRUE;
	//}

	return FALSE;
}

// pcsTarget 이 pcsCharacter 의 적인지 확인. 일반 적 + 길드 적
inline BOOL AgpmPvP::IsEnemyCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bServer )
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// 둘다 PC 여야 함
	if(!IsPC(pcsCharacter) || !IsPC(pcsTarget))
		return FALSE;

	if(IsEnemy(pcsCharacter, pcsTarget->m_lID))
		return TRUE;

	// EnemyGuild 이면 return TRUE
	CHAR* szGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	CHAR* szTargetGuildID = m_pagpmGuild->GetJoinedGuildID(pcsTarget);
	if(szTargetGuildID && _tcslen(szTargetGuildID) > 0)
	{
		// 2007.01.03. steeple
		// 길드전 중이라면 장소와 관계없이 리턴 TRUE
		if(szGuildID && _tcslen(szGuildID) > 0 )
		{
			// 현재 상태가 길드전 중인지 다시 한번확인.
			// 락을 해야하기 때문에 늦게 처리한다.
			// 하지만 그냥 일단 락 하지 않았다. 큰 문제가 생길 건 같지 않아서.
			AgpdGuild* pcsGuild			= m_pagpmGuild->GetGuild(szGuildID);
			AgpdGuild* pcsTargetGuild	= m_pagpmGuild->GetGuild( szTargetGuildID );

			if( m_pagpmGuild->IsBattleIngStatus(pcsGuild) && m_pagpmGuild->IsBattleIngStatus(pcsTargetGuild) 
				&& m_pagpmGuild->IsEnemyGuild(szGuildID, szTargetGuildID) 
				&& m_pagpmGuild->IsEnemyGuild(szTargetGuildID, szGuildID))
			{
				if( bServer )
				{
					if( pcsGuild->IsOnBattle( pcsCharacter->m_szID ) && pcsTargetGuild->IsOnBattle( pcsTarget->m_szID ) )
					{
						/*if( m_pagpmCharacter->IsInSiege(pcsCharacter) || m_pagpmCharacter->IsInSiege(pcsTarget) ||
							m_pagpmCharacter->IsInDungeon(pcsCharacter) || m_pagpmCharacter->IsInDungeon(pcsTarget))
							return FALSE;
						else*/
							return TRUE;
					}
				}
				else
					return FALSE;
			}
		}

		// 2007.01.03. steeple
		// 수정.
		//
		// 마을을 제외하고, 적길드 또는 적대길드라면 싸워진다.
		if(!IsSafePvPMode(pcsCharacter) && !IsSafePvPMode(pcsTarget) &&
			(IsEnemyGuild(pcsCharacter, szTargetGuildID) ||
			m_pagpmGuild->IsHostileGuild(szGuildID, szTargetGuildID)))
			return TRUE;
	}

	return FALSE;
}

// Attack 가능한 캐릭터인지 검사
// pcsCharacter 가 pcsTarget 을 패는 것임.
BOOL AgpmPvP::IsAttackable(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bForce)
{
	//STOPWATCH2(GetModuleName(), _T("IsAttackable"));

	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	if(pcsCharacter == pcsTarget)
		return TRUE;

	// PvP인 경우인데 PvP를 못하게 설정되어있는지 검사한다.
	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
	{
		if(m_pagpmConfig->IsEnablePvP() == FALSE)
			return FALSE;
	}

	// 어떤 전투도 일어나면 안되는 때인지 검사한다.
	if(!EnumCallback(AGPMPVP_CB_IS_ATTACKABLE_TIME, pcsCharacter, pcsTarget))
		return FALSE;

	// 공격자가 공격 불가능 상태이다. 2007.06.27. steeple
	if(m_pagpmCharacter->IsStatusDisableNormalATK(pcsCharacter))
	{
		if(m_pagpmCharacter->IsPC(pcsCharacter))
			ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_DISABLE_NORMAL_ATTACK, NULL, NULL, 0, 0, pcsCharacter);

		return FALSE;
	}

	// Disable 상태이면 공격할 수 없다. 2007.07.18. steeple
	if(m_pagpmCharacter->IsDisableCharacter(pcsCharacter) || m_pagpmCharacter->IsDisableCharacter(pcsTarget))
		return FALSE;

	// 2005.08.29. steeple
	// 둘 중 하나라도 Summons 이라면 다음체크를 간다. Taming 포함.
	if(m_pagpmCharacter->IsStatusSummoner(pcsCharacter) || m_pagpmCharacter->IsStatusSummoner(pcsTarget) ||
		m_pagpmCharacter->IsStatusTame(pcsCharacter) || m_pagpmCharacter->IsStatusTame(pcsTarget))
		return IsAttackableSummons(pcsCharacter, pcsTarget, bForce);

	// 때리는 놈이 PC 가 아니면
	if(!m_pagpmCharacter->IsPC(pcsCharacter) || m_pagpmCharacter->IsGuard(pcsCharacter))
	{
		// 서로 몬스터이면 못 때림
		if(m_pagpmCharacter->IsMonster(pcsCharacter) && m_pagpmCharacter->IsMonster(pcsTarget))
			return FALSE;
			
		// 배틀 그라운드 - arycoat 2008. 01.
		// 몹이 유저를 공격할때...
		if( m_pagpmBattleGround->IsInBattleGround(pcsCharacter) && m_pagpmBattleGround->IsInBattleGround(pcsTarget) )
		{
			return (!m_pagpmCharacter->IsSameRace(pcsCharacter, pcsTarget));
		}
		
		return TRUE;
	}

	// 2006.09.19. steeple
	// 변신상태 크리쳐인지 검사
	if((pcsCharacter->m_bIsTrasform && m_pagpmCharacter->IsCreature(pcsCharacter->m_pcsCharacterTemplate)) ||
		(pcsTarget->m_bIsTrasform && m_pagpmCharacter->IsCreature(pcsTarget->m_pcsCharacterTemplate)))
	{
		return FALSE;
	}

	// 맞는 놈이 PC 가 아니면 여러 체크를 하자.
	if (!m_pagpmCharacter->IsPC(pcsTarget))
	{
		// 타겟이 공성 오브젝트인지 보고 공성 오브젝트라면 공성, 수성을 잘 따져서 공격 가능 여부를 확인한다.
		BOOL	bIsSiegeWarObject	= FALSE;
		BOOL	bCheckResult		= FALSE;

		PVOID	pvBuffer[4];
		pvBuffer[0]	= pcsCharacter;
		pvBuffer[1]	= pcsTarget;
		pvBuffer[2]	= &bIsSiegeWarObject;
		pvBuffer[3]	= &bCheckResult;

		EnumCallback(AGPMPVP_CB_CHECK_NPC_ATTACKABLE_TARGET, pvBuffer, NULL);

		if (bIsSiegeWarObject)
			return bCheckResult;

		// 트랩 추가 2006.12.27. steeple
		if (!m_pagpmCharacter->IsAttackable(pcsTarget) || m_pagpmCharacter->IsTrap(pcsTarget))
			return FALSE;
			
		// 배틀 그라운드 - arycoat 2008. 01.
		// 유저가 몹을 공격할때..
		if( m_pagpmBattleGround->IsInBattleGround(pcsCharacter) && m_pagpmBattleGround->IsInBattleGround(pcsTarget) )
		{
			return (!m_pagpmCharacter->IsSameRace(pcsCharacter, pcsTarget));
		}
		
		return TRUE;
	}
	else if(strlen(pcsTarget->m_szID) == 0)
	{
		// 이경우엔 가드나 NPC 임
		return FALSE;
	}

	// 스킬 시전자가 자고 있으면 불가. 2007.06.27. steeple
	if(m_pagpmCharacter->IsStatusSleep(pcsCharacter))
		return FALSE;

	// 파티중이고 타겟이 파티원이면 빠진다.
	AgpdParty *pcsParty = m_pagpmParty->GetParty(pcsCharacter);
	if (NULL != pcsParty)
	{
		AuAutoLock LockParty(pcsParty->m_Mutex);
		if (LockParty.Result())
		{
			if (IsFreePvPMode(pcsCharacter) && m_pagpmParty->IsMember(pcsParty, pcsTarget->m_lID))
				return FALSE;
		}
	}

	// 상대방이 무적상태라면 return FALSE
	if(IsInvincible(pcsTarget) || m_pagpmCharacter->IsStatusInvincible(pcsTarget))
	{

		// 배틀 그라운드일 경우 시스템 메세지에서 캐릭터명을
		// ******* 로 바꾼다
		if(m_pagpmCharacter->IsPC(pcsCharacter))
		{

			string	strName		=	pcsTarget->m_szID;

			if( m_pagpmBattleGround->IsInBattleGround( pcsTarget ) || 
				APMMAP_PECULIARITY_RETURN_DISABLE_USE == m_papmMap->CheckRegionPerculiarity( pcsTarget->m_nBindingRegionIndex , APMMAP_PECULIARITY_SHOWNAME ) )
			{
				strName.replace( strName.begin() , strName.end() , strName.size() , '*' );
			}

			ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_TARGET_INVINCIBLE, (char*)strName.c_str(), NULL, 0, 0, pcsCharacter);
		}

		return FALSE;
	}

	// 상대방이 투명이면 공격하지 못한다.
	if(m_pagpmCharacter->IsStatusFullTransparent(pcsTarget))
		return FALSE;
		
	// 배틀 그라운드 - arycoat 2008. 01.
	// 유저가 유저를 공격할때...
	if( m_pagpmBattleGround->IsInBattleGround(pcsCharacter) && m_pagpmBattleGround->IsInBattleGround(pcsTarget) )
	{
		return (!m_pagpmCharacter->IsSameRace(pcsCharacter, pcsTarget));
	}
		
	// 공성 진행중인 지역에서는 일단 모두 공격 가능. 같은 지역일테니 한놈만 조사한다.
	BOOL bIsInSiegeWarIngArea = FALSE;
	BOOL bIsParticipantSiegeWar = FALSE;
	PVOID pvBuffer[2];
	pvBuffer[0] = &bIsInSiegeWarIngArea;
	pvBuffer[1] = &bIsParticipantSiegeWar;
	EnumCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pcsCharacter, pvBuffer);
	if (!bIsInSiegeWarIngArea)
	{
		// 2005.10.04. steeple
		if(IsEnemyCharacter(pcsCharacter, pcsTarget))
			return TRUE;

		// 최소 PvP 레벨보다 낮으면 공격 불가
		// 하지만, 중국인 경우 다른 종족은 공격 가능하다는 거..
		INT32 lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
		if (m_lMinPvPLevel > lLevel)
		{
			ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_PVP_BY_LEVEL, NULL, NULL, m_lMinPvPLevel, 0, pcsCharacter);
			return FALSE;
		}

		
		lLevel = m_pagpmCharacter->GetLevel(pcsTarget);
		if (m_lMinPvPLevel > lLevel)
		{
			ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_PVP_BY_LEVEL, NULL, NULL, m_lMinPvPLevel, 0, pcsCharacter);
			return FALSE;
		}
	}
	// 공성 진행 지역
	else
	{
		// 2006.06.20. steeple
		// 공성전일 때 공격가능한 지 체크
		// 클라이언트에는 적 데이터가 cm 모듈에 있어서 부득이하게 이렇게 처리.
		if(EnumCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pcsCharacter, pcsTarget))
			return TRUE;
		else if(m_pagpmGuild->IsSameGuild(pcsCharacter, pcsTarget))
			return FALSE;
		else if(EnumCallback(AGPMPVP_CB_IS_FRIEND_SIEGEWAR, pcsCharacter, pcsTarget) && TRUE != bForce)
			return FALSE;
		
		// 때리는놈은 참가자 맞는놈은 어중이.
		if (bIsParticipantSiegeWar)
			return TRUE;

		return FALSE;
	}
	
	// 적이 아니라면 지역 체크

	// 둘 중 하나라도 안전지역이면 공격불가
	// 하지만, 중국은 다른 종족이면 공격할 수 있다는 거... -_-;
	if(IsSafePvPMode(pcsCharacter) || IsSafePvPMode(pcsTarget))
		return FALSE;

	// ------------  Friend Character 도 공격가능한 Region이면 공격가능하게 
	INT nTargetResult	=	m_papmMap->CheckRegionPerculiarity( pcsTarget->m_nBindingRegionIndex	, APMMAP_PECULIARITY_GUILD_PVP );
	INT nSelfResult		=	m_papmMap->CheckRegionPerculiarity( pcsCharacter->m_nBindingRegionIndex	, APMMAP_PECULIARITY_GUILD_PVP );

	if( nTargetResult	==	APMMAP_PECULIARITY_RETURN_DISABLE_USE &&
		nSelfResult		==	APMMAP_PECULIARITY_RETURN_DISABLE_USE	)
	{
		return TRUE;
	}
	// -------------------------------------------------------------------------

	// 위험 전투 지역에서 같은 편이면 return FALSE
	if(IsCombatPvPMode(pcsCharacter) && IsFriendCharacter(pcsCharacter, pcsTarget))
	{
		ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_ATTACK_FRIEND, NULL, NULL, NULL, NULL, pcsCharacter);
		return FALSE;
	}

	// 2005.04.08. steeple
	// 위험전투지역에서도 강제공격아니면 적이 아닌 캐릭터를 공격 못함.
	// 자유전투지역에서 적이 아닐 때 강제공격이 아니면 실패
	if(/*IsFreePvPMode(pcsCharacter) && */!IsEnemyCharacter(pcsCharacter, pcsTarget) && bForce != TRUE) 
		return FALSE;

	if (!IsEnemyCharacter(pcsCharacter, pcsTarget) &&
		m_pagpmCharacter->HasPenalty(pcsCharacter, AGPMCHAR_PENALTY_FIRST_ATTACK))
		return FALSE;

	return TRUE;
}

// 2005.08.29. steeple
// Summons 계열 때문에 몇가지 추가 및 체크 방법 변화.
// Skill 쓸 수 있는 상대인지 검사
// pcsCharacter 가 pcsTarget 에게 쓰는 것임
BOOL AgpmPvP::IsSkillEnable(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget, AgpdCharacter* pcsSummonsOwner)
{
	//STOPWATCH2(GetModuleName(), _T("IsSkillEnable"));

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pcsSkill->m_pcsBase;
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter == pcsTarget)
		return TRUE;

	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
	{
		if(m_pagpmConfig->IsEnablePvP() == FALSE)
			return FALSE;
	}

	// 2005.08.29. steeple
	// Owner 가 NULL 이고 둘중 하나라도 Summons 이라면 다음 체크로 간다.
	if(!pcsSummonsOwner && (m_pagpmCharacter->IsStatusSummoner(pcsCharacter) || m_pagpmCharacter->IsStatusSummoner(pcsTarget) ||
		m_pagpmCharacter->IsStatusTame(pcsCharacter) || m_pagpmCharacter->IsStatusTame(pcsTarget)))
		return IsSkillEnableSummons(pcsSkill, pcsTarget);

	// 2005.08.29. steeple
	// Owner 가 있고, pcsCharacter 가 Owner 의 Summons 라면 pcsCharacter 를 pcsSummonsOwner 로 바꿔서 체크한다.
	if(pcsSummonsOwner && pcsCharacter != pcsSummonsOwner &&
		(m_pagpmCharacter->IsStatusSummoner(pcsCharacter) || m_pagpmCharacter->IsStatusTame(pcsCharacter)) &&
		m_pagpmSummons->GetOwnerCID(pcsCharacter) == pcsSummonsOwner->m_lID)
		pcsCharacter = pcsSummonsOwner;

	// 다시한번 체크
	if(!pcsCharacter)
		return FALSE;

	// 2006.09.19. steeple
	// 변신상태 크리쳐인지 검사
	if((pcsCharacter->m_bIsTrasform && m_pagpmCharacter->IsCreature(pcsCharacter->m_pcsCharacterTemplate)) ||
		(pcsTarget->m_bIsTrasform && m_pagpmCharacter->IsCreature(pcsTarget->m_pcsCharacterTemplate)))
	{
		return FALSE;
	}

	// 스킬 시전자가 불가능 상태이다. 2007.06.27. steeple
	if(m_pagpmCharacter->IsStatusDisableSkill(pcsCharacter))
	{
		if(m_pagpmCharacter->IsPC(pcsCharacter))
			ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_DISABLE_SKILL_CAST, NULL, NULL, 0, 0, pcsCharacter);

		return FALSE;
	}

	// Disable 상태이면 캐스팅할 수 없다. 2007.07.18. steeple
	if(m_pagpmCharacter->IsDisableCharacter(pcsCharacter) || m_pagpmCharacter->IsDisableCharacter(pcsTarget))
		return FALSE;

	// 공격스킬인지
	BOOL bAttackSkill = IsAttackSkill(pcsSkill);

	// 스킬 쓰는 놈이 PC 가 아니면
	if(!m_pagpmCharacter->IsPC(pcsCharacter))
	{
		// 서로 몬스터이면 못 때림
		if(m_pagpmCharacter->IsMonster(pcsCharacter) && m_pagpmCharacter->IsMonster(pcsTarget))
			return FALSE;

		// 공격스킬이고, 맞는 놈이 PC 이고 무적이면 못 때림
		if(bAttackSkill && m_pagpmCharacter->IsPC(pcsTarget) && m_pagpmCharacter->IsStatusInvincible(pcsTarget))
			return FALSE;

		return TRUE;
	}

	// 스킬 맞는 놈이 PC 가 아니면 여러 체크를 하자.
	if(!m_pagpmCharacter->IsPC(pcsTarget))
	{
		// 타겟이 공성 오브젝트인지 보고 공성 오브젝트라면 공성, 수성을 잘 따져서 공격 가능 여부를 확인한다.
		BOOL	bIsSiegeWarObject	= FALSE;
		BOOL	bCheckResult		= FALSE;

		PVOID	pvBuffer[4];
		pvBuffer[0]	= pcsCharacter;
		pvBuffer[1]	= pcsTarget;
		pvBuffer[2]	= &bIsSiegeWarObject;
		pvBuffer[3]	= &bCheckResult;

		EnumCallback(AGPMPVP_CB_CHECK_NPC_ATTACKABLE_TARGET, pvBuffer, NULL);

		if (bIsSiegeWarObject)
		{
			return bCheckResult;
		}
		else
		{
			// 트랩 추가 2006.12.27. steeple
			if (!m_pagpmCharacter->IsAttackable(pcsTarget) || m_pagpmCharacter->IsTrap(pcsTarget))
				return FALSE;
			else
				return TRUE;
		}
	}
	else if(strlen(pcsTarget->m_szID) == 0)
	{
		// 이경우엔 가드나 NPC 임
		return FALSE;
	}

	// 스킬 시전자가 자고 있으면 불가. 2007.06.27. steeple
	if(m_pagpmCharacter->IsStatusSleep(pcsCharacter))
		return FALSE;

	// 적이어도 투명, 무적이 아니어야 한다. 2005.10.04. steeple
	if(IsEnemyCharacter(pcsCharacter, pcsTarget) &&
		m_pagpmCharacter->IsStatusFullTransparent(pcsTarget) == FALSE &&
		IsInvincible(pcsTarget) == FALSE &&
		m_pagpmCharacter->IsStatusInvincible(pcsTarget) == FALSE)
		return TRUE;

	if(bAttackSkill)
	{
		// 2005.10.04. steeple
		// 공격 스킬일 때, 무적인 애들에게는 써지면 안된다.
		if(m_pagpmCharacter->IsStatusInvincible(pcsTarget))
		{
			if(m_pagpmCharacter->IsPC(pcsCharacter))
			{
				string	strName		=	pcsTarget->m_szID;

				if( m_pagpmBattleGround->IsInBattleGround( pcsTarget ) ||
					APMMAP_PECULIARITY_RETURN_DISABLE_USE == m_papmMap->CheckRegionPerculiarity( pcsTarget->m_nBindingRegionIndex , APMMAP_PECULIARITY_SHOWNAME ) )
				{
					strName.replace( strName.begin() , strName.end() , strName.size() , '*' );
				}

				ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_TARGET_INVINCIBLE, (char*)strName.c_str(), NULL, 0, 0, pcsCharacter);
			}

			return FALSE;
		}

		// 디텍트 스킬 추가 2006.09.21. steeple
		if(!m_pagpmSkill->IsDetectSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) && 
			m_pagpmCharacter->IsStatusFullTransparent(pcsTarget))
			return FALSE;

		// 2006.06.20. steeple
		// 공성전일 때 공격가능한 지 체크
		// 클라이언트에는 적 데이터가 cm 모듈에 있어서 부득이하게 이렇게 처리.
		if(EnumCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pcsCharacter, pcsTarget))
			return TRUE;
		else if(EnumCallback(AGPMPVP_CB_IS_FRIEND_SIEGEWAR, pcsCharacter, pcsTarget))
			return FALSE;

		// Attack Skill 을 사용할 때 자유전투, 위험전투 지역이 아니면 return FALSE
		if(!IsCombatPvPMode(pcsCharacter) && !IsFreePvPMode(pcsCharacter))
			return FALSE;

		// 둘 중 하나라도 안전지역이면 공격불가
		if(IsSafePvPMode(pcsCharacter) || IsSafePvPMode(pcsTarget))
			return FALSE;

		// Attack Skill 을 사용할 때 같은 편에게는 쓸 수 없다.
		// 위험전투 지역에서만 체크해주면 된다.
		// pcsCharacter와 pcsTarget이 둘다 배틀그라운드 안이면 같은 길드끼리도 싸움 가능
		if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter) == FALSE && m_pagpmBattleGround->IsInBattleGround(pcsTarget) == FALSE)
		{
			if(IsFriendCharacter(pcsCharacter, pcsTarget) && IsCombatPvPMode(pcsCharacter) && IsCombatPvPMode(pcsTarget))
			{	
				ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_ATTACK_FRIEND, NULL, NULL, NULL, NULL, pcsCharacter);
				return FALSE;
			}
		}

		// 공격 스킬일 때
		// 파티중이고 타겟이 파티원이면 빠진다.
		AgpdParty *pcsParty = m_pagpmParty->GetParty(pcsCharacter);
		if (NULL != pcsParty)
		{
			AuAutoLock LockParty(pcsParty->m_Mutex);
			if (LockParty.Result())
			{
				if (IsFreePvPMode(pcsCharacter) && m_pagpmParty->IsMember(pcsParty, pcsTarget->m_lID))
					return FALSE;
			}
		}

		// 2008.6.18 arycoat - battleground
		if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
		{
			if(!m_pagpmBattleGround->IsAttackable(pcsCharacter, pcsTarget))
				return FALSE;
		} 
		else if(/*IsFreePvPMode(pcsCharacter) &&*/ !IsEnemyCharacter(pcsCharacter, pcsTarget) && pcsSkill->m_bForceAttack != TRUE)
			return FALSE;

		BOOL bIsInSiegeWarIngArea = FALSE;
		BOOL bIsParticipantSiegeWar = FALSE;
		PVOID pvBuffer[2];
		pvBuffer[0] = &bIsInSiegeWarIngArea;
		pvBuffer[1] = &bIsParticipantSiegeWar;
		EnumCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pcsCharacter, pvBuffer);
		if (!bIsInSiegeWarIngArea)
		{
			// 때리는 놈이나 맞는 놈이나 레벨 6미만은 PK불가.
			INT32 lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
			if (m_lMinPvPLevel > lLevel)
			{
				ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_PVP_BY_LEVEL, NULL, NULL, m_lMinPvPLevel, 0, pcsCharacter);
				return FALSE;
			}
			
			lLevel = m_pagpmCharacter->GetLevel(pcsTarget);
			if (m_lMinPvPLevel > lLevel)
			{
				ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_PVP_BY_LEVEL, NULL, NULL, m_lMinPvPLevel, 0, pcsCharacter);
				return FALSE;
			}
		}
		else
		{
			// 워케하는겨? 나두 몰러...
		}

		// 지역스킬인지
		BOOL bAreaSkill = m_pagpmSkill->IsAreaSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate);


		// 2005.04.08. steeple
		// 위험전투지역에서도 강제공격아니면 적이 아닌 캐릭터를 공격 못함.
		//
		// 지역스킬이고 공격스킬이고 자유전투지역일때는
		if(bAreaSkill)// && bAttackSkill && IsFreePvPMode(pcsCharacter) && IsFreePvPMode(pcsTarget))
		{
			// 2006.10.24. steeple
			// 디텍트 스킬 관련 추가
			if(m_pagpmSkill->IsDetectSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
				{
					if(!m_pagpmBattleGround->IsAttackable(pcsCharacter, pcsTarget))
						return FALSE;
				}
				else
				{
					// 위험지역도 아니고 적도 아니라면
					if(!IsEnemyCharacter(pcsCharacter, pcsTarget) && !IsCombatPvPMode(pcsCharacter) && !IsCombatPvPMode(pcsTarget))
						return FALSE;
				}
			}
			else
			{
				if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
				{
					if(!m_pagpmBattleGround->IsAttackable(pcsCharacter, pcsTarget))
						return FALSE;
				}
				else
				{
					// 스킬의 직접적인 타겟과 같지 않고 적이 아니면 실패
					if(pcsSkill->m_csTargetBase.m_lID != AP_INVALID_CID &&
						pcsSkill->m_csTargetBase.m_lID != pcsTarget->m_lID &&
						!IsEnemyCharacter(pcsCharacter, pcsTarget))
						return FALSE;
				}
			}
		}

		if(!m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
		{
			if (!IsEnemyCharacter(pcsCharacter, pcsTarget) &&
				m_pagpmCharacter->HasPenalty(pcsCharacter, AGPMCHAR_PENALTY_FIRST_ATTACK))
				return FALSE;
		}
	}

	return TRUE;
}

// Attack 스킬인지 확인한다.
// Debuf 스킬도 Attack 으로 간주
inline BOOL AgpmPvP::IsAttackSkill(AgpdSkill* pcsSkill)
{
	if(!pcsSkill || !pcsSkill->m_pcsTemplate)
		return FALSE;

	return IsAttackSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate);
}

inline BOOL AgpmPvP::IsAttackSkill(AgpdSkillTemplate* pcsSkillTemplate)
{
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bAttackSkill = FALSE;
    if(pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_BUFF_TYPE][1] == 1)
		bAttackSkill = TRUE;
	else if(m_pagpmSkill->IsAttackSkill(pcsSkillTemplate))
		bAttackSkill = TRUE;
	else if(m_pagpmSkill->GetActionOnActionType(pcsSkillTemplate) == 1)	// Action On Action Type1 (ex. Time-Attack)
		bAttackSkill = TRUE;

	return bAttackSkill;
}

// 2005.08.29. steeple
// pcsCharacter 나 pcsTarget 이 하나라도 Summons 이면 일단 이리로 온다.
BOOL AgpmPvP::IsAttackableSummons(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bForce)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// 둘다 PC 면. 여기 와선 안된다.
	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
		return FALSE;

	// 2005.11.09. steeple
	// Fixed 는 맞지 않는다.
	if(m_pagpmCharacter->IsStatusFixed(pcsTarget))
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar1 = m_pagpmSummons->GetADCharacter(pcsCharacter);
	AgpdSummonsADChar* pcsSummonsADChar2 = m_pagpmSummons->GetADCharacter(pcsTarget);
	if(!pcsSummonsADChar1 || !pcsSummonsADChar2)
		return FALSE;

	// 둘다 Summons 이다.
	if(pcsSummonsADChar1->m_lOwnerCID != 0 && pcsSummonsADChar2->m_lOwnerCID != 0)
	{
		// 주인이 같을 땐 Pass
		if(pcsSummonsADChar1->m_lOwnerCID == pcsSummonsADChar2->m_lOwnerCID)
			return FALSE;

		// 이럴 땐 주인을 얻어서 IsAttackable 을 불러주자.
		ApAutoLockCharacter LockOwner1(m_pagpmCharacter, pcsSummonsADChar1->m_lOwnerCID);
		AgpdCharacter* pcsOwner1 = LockOwner1.GetCharacterLock();
		if(!pcsOwner1)
			return FALSE;

		ApAutoLockCharacter LockOwner2(m_pagpmCharacter, pcsSummonsADChar2->m_lOwnerCID);
		AgpdCharacter* pcsOwner2 = LockOwner2.GetCharacterLock();
		if(!pcsOwner2)
			return FALSE;

		// 그냥 IsAttackable 부르면 될듯
		return IsAttackable(pcsOwner1, pcsOwner2, bForce);
	}
	// 때리는 놈만 Summons 이다.
	else if(pcsSummonsADChar1->m_lOwnerCID != 0 && pcsSummonsADChar2->m_lOwnerCID == 0)
	{
		// 주인이 타겟이면 Pass
		if(pcsSummonsADChar1->m_lOwnerCID == pcsTarget->m_lID)
			return FALSE;

		// 때리는 놈의 주인만 얻자
		ApAutoLockCharacter LockOwner1(m_pagpmCharacter, pcsSummonsADChar1->m_lOwnerCID);
		AgpdCharacter* pcsOwner1 = LockOwner1.GetCharacterLock();
		if(!pcsOwner1)
			return FALSE;

		return IsAttackable(pcsOwner1, pcsTarget, bForce);
	}
	// 맞는 놈만 Summons 이다.
	else if(pcsSummonsADChar1->m_lOwnerCID == 0 && pcsSummonsADChar2->m_lOwnerCID != 0)
	{
		// 자기 Summons 이라면 Pass
		if(pcsCharacter->m_lID == pcsSummonsADChar2->m_lOwnerCID)
			return FALSE;

		// 맞는 놈의 주인만 얻자.
		ApAutoLockCharacter LockOwner2(m_pagpmCharacter, pcsSummonsADChar2->m_lOwnerCID);
		AgpdCharacter* pcsOwner2 = LockOwner2.GetCharacterLock();
		if(!pcsOwner2)
			return FALSE;

		return IsAttackable(pcsCharacter, pcsOwner2, bForce);
	}
	// 2005.10.08. steeple
	// 이 루틴을 탔는데, 소유주가 둘다 0 이면 클라이언트에서 불린거다.
	// 클라이언트는 다른 소환수의 주인을 모르므로(2005.10.08 현재. 나중에 알게 될 수도 있음)
	// 일단 return TRUE 로 해주고, 서버에서 체크하게 해준다.
	else if(pcsSummonsADChar1->m_lOwnerCID == 0 && pcsSummonsADChar2->m_lOwnerCID == 0)
	{
		return TRUE;
	}

	return FALSE;
}

// 2005.08.29. steeple
// pcsCharacter 나 pcsTarget 이 하나라도 Summons 이면 일단 이리로 온다.
BOOL AgpmPvP::IsSkillEnableSummons(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget)
{
	if(!pcsSkill || !pcsSkill->m_pcsBase)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pcsSkill->m_pcsBase;
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// 둘다 PC 면. 여기 와선 안된다.
	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
		return FALSE;

	// 2005.11.09. steeple
	// Fixed 는 맞지 않는다.
	if(m_pagpmCharacter->IsStatusFixed(pcsTarget))
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar1 = m_pagpmSummons->GetADCharacter(pcsCharacter);
	AgpdSummonsADChar* pcsSummonsADChar2 = m_pagpmSummons->GetADCharacter(pcsTarget);
	if(!pcsSummonsADChar1 || !pcsSummonsADChar2)
		return FALSE;

	// 둘다 Summons 이다.
	if(pcsSummonsADChar1->m_lOwnerCID != 0 && pcsSummonsADChar2->m_lOwnerCID != 0)
	{
		// 주인이 같을 때
		if(pcsSummonsADChar1->m_lOwnerCID == pcsSummonsADChar2->m_lOwnerCID)
		{
			if(IsAttackSkill(pcsSkill))		// 공격스킬이면 Pass
				return FALSE;
			else							// 아니면 사용가능
				return TRUE;
		}

		// 이 스킬의 주인은 Summons 이다 보니, 주인으로 
		// 이럴 땐 주인을 얻어서 IsSkillEnable 을 불러주자.
		ApAutoLockCharacter LockOwner1(m_pagpmCharacter, pcsSummonsADChar1->m_lOwnerCID);
		AgpdCharacter* pcsOwner1 = LockOwner1.GetCharacterLock();
		if(!pcsOwner1)
			return FALSE;

		ApAutoLockCharacter LockOwner2(m_pagpmCharacter, pcsSummonsADChar2->m_lOwnerCID);
		AgpdCharacter* pcsOwner2 = LockOwner2.GetCharacterLock();
		if(!pcsOwner2)
			return FALSE;

		// 그냥 IsSkillEnable 부르면 될듯
		return IsSkillEnable(pcsSkill, pcsOwner2, pcsOwner1);
	}
	// 때리는 놈만 Summons 이다.
	else if(pcsSummonsADChar1->m_lOwnerCID != 0 && pcsSummonsADChar2->m_lOwnerCID == 0)
	{
		// 주인이 타겟이면
		if(pcsSummonsADChar1->m_lOwnerCID == pcsTarget->m_lID)
		{
			if(IsAttackSkill(pcsSkill))		// 공격스킬이면 Pass
				return FALSE;
			else							// 아니면 사용가능
				return TRUE;
		}

		// 때리는 놈의 주인만 얻자
		ApAutoLockCharacter LockOwner1(m_pagpmCharacter, pcsSummonsADChar1->m_lOwnerCID);
		AgpdCharacter* pcsOwner1 = LockOwner1.GetCharacterLock();
		if(!pcsOwner1)
			return FALSE;

		return IsSkillEnable(pcsSkill, pcsTarget, pcsOwner1);
	}
	// 맞는 놈만 Summons 이다.
	else if(pcsSummonsADChar1->m_lOwnerCID == 0 && pcsSummonsADChar2->m_lOwnerCID != 0)
	{
		// 자기 Summons 이라면
		if(pcsCharacter->m_lID == pcsSummonsADChar2->m_lOwnerCID)
		{
			if(IsAttackSkill(pcsSkill))		// 공격스킬이면 Pass
				return FALSE;
			else							// 아니면 사용가능
				return TRUE;
		}

		// 맞는 놈의 주인만 얻자.
		ApAutoLockCharacter LockOwner2(m_pagpmCharacter, pcsSummonsADChar2->m_lOwnerCID);
		AgpdCharacter* pcsOwner2 = LockOwner2.GetCharacterLock();
		if(!pcsOwner2)
			return FALSE;

		return IsSkillEnable(pcsSkill, pcsOwner2, pcsCharacter);	// Owner 인자에 그냥 Summons 를 넘겨버린다.
	}
	// 2005.10.08. steeple
	// 이 루틴을 탔는데, 소유주가 둘다 0 이면 클라이언트에서 불린거다.
	// 클라이언트는 다른 소환수의 주인을 모르므로(2005.10.08 현재. 나중에 알게 될 수도 있음)
	// 일단 return TRUE 로 해주고, 서버에서 체크하게 해준다.
	else if(pcsSummonsADChar1->m_lOwnerCID == 0 && pcsSummonsADChar2->m_lOwnerCID == 0)
	{
		return TRUE;
	}

	return FALSE;
}

// System Message 를 받아서 처리해준다.
BOOL AgpmPvP::ProcessSystemMessage(INT32 lCode, CHAR* szData1, CHAR* szData2, INT32 lData1, INT32 lData2, AgpdCharacter* pcsCharacter)
{
	AgpdPvPSystemMessage stSystemMessage;
	memset(&stSystemMessage, 0, sizeof(AgpdPvPSystemMessage));

	stSystemMessage.m_lCode = lCode;
	stSystemMessage.m_aszData[0] = szData1;
	stSystemMessage.m_aszData[1] = szData2;
	stSystemMessage.m_alData[0] = lData1;
    stSystemMessage.m_alData[1] = lData2;
    
	EnumCallback(AGPMPVP_CB_SYSTEM_MESSAGE, &stSystemMessage, pcsCharacter);
	return TRUE;
}









//////////////////////////////////////////////////////////////////////////
// Callback
BOOL AgpmPvP::CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmPvP* pThis = (AgpmPvP*)pClass;

	return pThis->InitCharacter(pcsCharacter);
}

BOOL AgpmPvP::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmPvP* pThis = (AgpmPvP*)pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	return pThis->RemoveCharacter(pcsCharacter);
}

BOOL AgpmPvP::CBIsAttackable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmPvP* pThis = (AgpmPvP*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;
	
	AgpdCharacter* pcsTarget = (AgpdCharacter*)ppvBuffer[0];
	BOOL bForce = PtrToInt(ppvBuffer[1]);

	return  pThis->IsAttackable(pcsCharacter, pcsTarget, bForce);
}

BOOL AgpmPvP::CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmPvP* pThis = (AgpmPvP*)pClass;
	INT16 nPrevRegionIndex = pCustData ? *(INT16*)pCustData : -1;	// NULL 로 올때는 ADD_CHARACTER_TO_MAP 에서 불린것임.

    return pThis->RegionChange(pcsCharacter, nPrevRegionIndex);
}

BOOL AgpmPvP::CBIsSkillEnable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdSkill* pcsSkill = (AgpdSkill*)pData;
	AgpmPvP* pThis = (AgpmPvP*)pClass;
	ApBase* pcsTarget = (ApBase*)pCustData;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return TRUE;

	return pThis->IsSkillEnable(pcsSkill, (AgpdCharacter*)pcsTarget);
}







//////////////////////////////////////////////////////////////////////////
// Callback Registration
BOOL AgpmPvP::SetCallbackPvPInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_PVP_INFO, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackAddFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ADD_FRIEND, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackAddEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ADD_ENEMY, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRemoveFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_REMOVE_FRIEND, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRemoveEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_REMOVE_ENEMY, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackPvPResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_PVP_RESULT, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackInitFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_INIT_FRIEND, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackInitEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_INIT_ENEMY, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackUpdateFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_UPDATE_FRIEND, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackUpdateEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_UPDATE_ENEMY, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackAddFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ADD_FRIEND_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackAddEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ADD_ENEMY_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRemoveFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_REMOVE_FRIEND_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRemoveEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_REMOVE_ENEMY_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackInitFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_INIT_FRIEND_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackInitEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_INIT_ENEMY_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackUpdateFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_UPDATE_FRIEND_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackUpdateEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_UPDATE_ENEMY_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRecvCannotUseTeleport(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_RECV_CANNOT_USE_TELEPORT, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackEnterSafePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ENTER_SAFE_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackEnterFreePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ENTER_FREE_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackEnterCombatPvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ENTER_COMBAT_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackLeaveSafePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_LEAVE_SAFE_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackLeaveFreePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_LEAVE_FREE_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackLeaveCombatPvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_LEAVE_COMBAT_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackSystemMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_SYSTEM_MESSAGE, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRequestDeadType(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_REQUEST_DEAD_TYPE, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackResponseDeadType(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_RESPONSE_DEAD_TYPE, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackIsFriendSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_IS_FRIEND_SIEGEWAR, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackIsEnemySiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackIsInSiegeWarIngArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackCheckNPCAttackableTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_CHECK_NPC_ATTACKABLE_TARGET, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackIsAttackableTime(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_IS_ATTACKABLE_TIME, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRaceBattleStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_RACE_BATTLE_STATUS, pfCallback, pClass);
}






//////////////////////////////////////////////////////////////////////////
// AgpmPvPArray 
AgpmPvPArray::AgpmPvPArray()
{
	lObjectCount	= 0;
}

AgpmPvPArray::~AgpmPvPArray()
{
}

BOOL	AgpmPvPArray::Initialize()
{
	m_csArrayID.MemSetAll();
	m_csArrayData.MemSetAll();

	ZeroMemory(m_csArrayName, sizeof(m_csArrayName));

	lObjectCount	= 0;

	return TRUE;
}

PVOID	AgpmPvPArray::AddObject(PVOID pObject, INT32 nKey)
{
	if (!pObject || nKey == 0)
		return NULL;

	if (lObjectCount == AGPMPVP_MAX_FRIEND_COUNT)
		return NULL;

	int i = 0;

	// 먼저 이미 있는 놈인지 본다.
	for (i = 0; i < lObjectCount; ++i)
	{
		if (m_csArrayID[i] == nKey)
			return NULL;
	}

	m_csArrayID[i]		= nKey;
	m_csArrayData[i]	= pObject;

	++lObjectCount;

	return pObject;
}

PVOID	AgpmPvPArray::AddObject(PVOID pObject, CHAR *szName)
{
	if (!pObject || !szName || !szName[0])
		return NULL;

	if (lObjectCount == AGPMPVP_MAX_FRIEND_COUNT)
		return NULL;

	int i = 0;

	// 먼저 이미 있는 놈인지 본다.
	for (i = 0; i < lObjectCount; ++i)
	{
		if (strncmp(m_csArrayName[i], szName, AGPMGUILD_MAX_GUILD_ID_LENGTH) == 0)
			return NULL;
	}

	strncpy(m_csArrayName[i], szName, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	m_csArrayData[i]	= pObject;

	++lObjectCount;

	return pObject;
}

BOOL	AgpmPvPArray::RemoveObject(INT32 nKey)
{
	if (nKey == 0)
		return FALSE;

	int i = 0;
	for (i = 0; i < lObjectCount; ++i)
	{
		if (m_csArrayID[i] == nKey)
			break;
	}

	if (i == lObjectCount)
		return FALSE;

	m_csArrayID.MemCopy(i, &m_csArrayID[i + 1], lObjectCount - i - 1);
	m_csArrayData.MemCopy(i, &m_csArrayData[i + 1], lObjectCount - i - 1);

	--lObjectCount;

	m_csArrayID[lObjectCount]		= 0;
	m_csArrayData[lObjectCount]		= NULL;

	return TRUE;
}

BOOL	AgpmPvPArray::RemoveObject(CHAR *szName)
{
	if (!szName || !szName[0])
		return FALSE;

	int i = 0;
	for (i = 0; i < lObjectCount; ++i)
	{
		if (strncmp(m_csArrayName[i], szName, AGPMGUILD_MAX_GUILD_ID_LENGTH) == 0)
			break;
	}

	if (i == lObjectCount)
		return FALSE;

	CopyMemory(m_csArrayName[i], m_csArrayName[i + 1], (sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1)) * (lObjectCount - i - 1));
	m_csArrayData.MemCopy(i, &m_csArrayData[i + 1], lObjectCount - i - 1);

	--lObjectCount;

	ZeroMemory(m_csArrayName[lObjectCount], sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1));
	m_csArrayData[lObjectCount]		= NULL;

	return TRUE;
}

PVOID	AgpmPvPArray::GetObject(INT32 nKey)
{
	for (int i = 0; i < lObjectCount; ++i)
	{
		if (m_csArrayID[i] == nKey)
			return m_csArrayData[i];
	}

	return NULL;
}

PVOID	AgpmPvPArray::GetObject(CHAR *szName)
{
	if (!szName || !szName[0])
		return FALSE;

	for (int i = 0; i < lObjectCount; ++i)
	{
		if (strncmp(m_csArrayName[i], szName, AGPMGUILD_MAX_GUILD_ID_LENGTH) == 0)
			return m_csArrayData[i];
	}

	return NULL;
}

PVOID	AgpmPvPArray::GetObjectSequence(INT32* plIndex)
{
	if (!plIndex)
		return NULL;

	PVOID	pvData	= m_csArrayData[*plIndex];

	++(*plIndex);

	return pvData;
}

INT32	AgpmPvPArray::GetObjectCount()
{
	return lObjectCount;
}
