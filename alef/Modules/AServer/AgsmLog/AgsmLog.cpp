// AgsmLog.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 04. 26.

//
// 2005.03.15. steeple
// 리뉴얼~ 
// 패킷을 그냥 구조체 통째로 넘겨버림.
//

#include "AgsmLog.h"

AgsmLog::AgsmLog()
{
	SetModuleName("AgsmLog");
	SetModuleType(APMODULE_TYPE_SERVER);

	EnableIdle(FALSE);

	m_pagpmLog = NULL;
	m_pAgsmServerManager = NULL;

	// Packet
	SetPacketType(AGSMLOG_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operation
							AUTYPE_MEMORY_BLOCK,	1,	// 각 Operation 에 대응하는 구조체가 온다.
							AUTYPE_END,				0
							);

	//m_csPacket.SetFlagLength(sizeof(INT16));
	//m_csPacket.SetFieldType(
	//						AUTYPE_INT8,				1,	// Operation
	//						AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// CharName
	//						AUTYPE_INT32,			1,	// CharTID
	//						AUTYPE_INT32,			1,	// CharLevel
	//						AUTYPE_INT32,			1,	// TimeStamp
	//						AUTYPE_PACKET,			1,	// Loginout Packet
	//						AUTYPE_PACKET,			1,	// Combat Packet
	//						AUTYPE_PACKET,			1,	// Skill Packet
	//						AUTYPE_PACKET,			1,	// Item Packet
	//						AUTYPE_PACKET,			1,	// LevelUp Packet
	//						AUTYPE_END,			0
	//						);

	//m_csLoginPacket.SetFlagLength(sizeof(INT8));
	//m_csLoginPacket.SetFieldType(
	//						AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Acc Name
	//						AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// World
	//						AUTYPE_INT8,			1,	// Flag - 0:Account, 1:Login, 2:Logout
	//						AUTYPE_INT32,			1,	// PlayTime
	//						AUTYPE_END,			0
	//						);

	//m_csCombatPacket.SetFlagLength(sizeof(INT16));
	//m_csCombatPacket.SetFieldType(
	//						AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Target Name
	//						AUTYPE_INT32,			1,	// TargetTID
	//						AUTYPE_INT32,			1,	// Target Level
	//						AUTYPE_INT32,			1,	// EXP
	//						AUTYPE_INT8,			1,	// Win
	//						AUTYPE_INT8,			1,	// Party
	//						AUTYPE_INT32,			1,	// Attack Count
	//						AUTYPE_INT32,			1,	// Skill Use Count
	//						AUTYPE_INT32,			1,	// Combat Time
	//						AUTYPE_MEMORY_BLOCK,	1,	// HP Potion (INT32, 3개 짜리 배열. 12Bytes)
	//						AUTYPE_MEMORY_BLOCK,	1,	// MP Potion (INT32, 3개 짜리 배열. 12Bytes)
	//						AUTYPE_MEMORY_BLOCK,	1,	// SP Potion (INT32, 3개 짜리 배열. 12Bytes)
	//						AUTYPE_END,			0
	//						);

	//m_csSkillPacket.SetFlagLength(sizeof(INT8));
	//m_csSkillPacket.SetFieldType(
	//						AUTYPE_INT32,			1,	// RemainPoint
	//						AUTYPE_MEMORY_BLOCK,	1,	// Mastery Point (INT32, 6개 짜리 배열이 온다. 24Bytes)
	//						AUTYPE_INT32,			1,	// SkillTID
	//						AUTYPE_INT32,			1,	// Count
	//						AUTYPE_END,			0
	//						);

	//m_csItemPacket.SetFlagLength(sizeof(INT16));
	//m_csItemPacket.SetFieldType(
	//						AUTYPE_INT64,			1,	// Item DB ID
	//						AUTYPE_INT32,			1,	// Item TID
	//						AUTYPE_INT32,			1,	// Item Count
	//						AUTYPE_INT8,			1,	// Flag - 0:Pickup, 1:Drop, 2:Use
	//						AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Buyer Name
	//						AUTYPE_INT8,			1,	// Convert Level
	//						AUTYPE_INT32,			1,	// Convert TID
	//						AUTYPE_INT8,			1,	// Convert Result
	//						AUTYPE_INT32,			1,	// Money
	//						AUTYPE_END,			0
	//						);

	//m_csLevelUpPacket.SetFlagLength(sizeof(INT8));
	//m_csLevelUpPacket.SetFieldType(
	//						AUTYPE_INT32,			1,	// Level From
	//						AUTYPE_INT32,			1,	// Level To
	//						AUTYPE_INT32,			1,	// PartyTime
	//						AUTYPE_INT32,			1,	// SoloTime
	//						AUTYPE_INT32,			1,	// Hunt Count
	//						AUTYPE_INT32,			1,	// Dead Count
	//						AUTYPE_END,			0
	//						);
}

AgsmLog::~AgsmLog()
{
}

BOOL AgsmLog::OnAddModule()
{
	m_pagpmLog = (AgpmLog*)GetModule("AgpmLog");
	m_pAgsmServerManager = (AgsmServerManager*)GetModule("AgsmServerManager2");
	
	if(!m_pagpmLog ||
		!m_pAgsmServerManager)
		return FALSE;

	if(!m_pagpmLog->SetCallbackWriteLog(CBWriteLog, this))
		return FALSE;
	
	return TRUE;
}

BOOL AgsmLog::IsLoginServer()
{
	if(m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_LOGIN_SERVER)
		return FALSE;

	return TRUE;
}

BOOL AgsmLog::IsGameServer()
{
	if(m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
		return FALSE;

	return TRUE;
}

BOOL AgsmLog::IsRelayServer()
{
	if(m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_RELAY_SERVER)
		return FALSE;

	return TRUE;
}

UINT32 AgsmLog::GetRelayServerNID()
{
	AgsdServer* pcsRelayServer = m_pAgsmServerManager->GetRelayServer();
	if(!pcsRelayServer)
		return 0;

	return pcsRelayServer->m_dpnidServer;
}

BOOL AgsmLog::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgsmLog::OnReceive");
	
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	PVOID pvDetailPacket = NULL;
	INT16 nLength = 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvDetailPacket, &nLength
						);

	if(!pvDetailPacket || nLength < 1)
		return FALSE;

	if(cOperation < AGPDLOG_CATEGORY_PLAY || cOperation >= AGPDLOG_CATEGORY_MAX)
		return FALSE;

	// 이거 최고~
	WriteLog(pvDetailPacket, nLength);

	return TRUE;

	//switch(cOperation)
	//{
	//	case AGPDLOGTYPE_LOGINOUT:
	//		OnReceiveLogin(&cLog, pvLoginPacket);
	//		break;

	//	case AGPDLOGTYPE_COMBAT:
	//		OnReceiveCombat(&cLog, pvCombatPacket);
	//		break;

	//	case AGPDLOGTYPE_SKILL_POINT:
	//	case AGPDLOGTYPE_SKILL_USE:
	//		OnReceiveSkill(&cLog, pvSkillPacket);
	//		break;

	//	case AGPDLOGTYPE_ITEM_PICKUSE:
	//	case AGPDLOGTYPE_ITEM_TRADE:
	//	case AGPDLOGTYPE_ITEM_CONVERT:
	//	case AGPDLOGTYPE_ITEM_REPAIR:
	//	case AGPDLOGTYPE_ITEM_OWNERCHANGE:
	//		OnReceiveItem(&cLog, pvItemPacket);
	//		break;

	//	case AGPDLOGTYPE_LEVEL_UP:
	//		OnReceiveLevelUp(&cLog, pvLevelUpPacket);
	//		break;
	//}
	//
	//return TRUE;
}

BOOL AgsmLog::WriteLog(PVOID pvLog, INT16 nSize)
{
	if(!pvLog || nSize < 1)
		return FALSE;

	AgpdLog* pcsLog = reinterpret_cast<AgpdLog*>(pvLog);
	if(!pcsLog)
		return FALSE;

	//로그인 서버면 게임 서버로 포스팅
	if (IsLoginServer())
	{
		if (AGPDLOG_CATEGORY_PLAY == pcsLog->m_cLogType
			|| AGPDLOG_CATEGORY_ETC == pcsLog->m_cLogType
			)
			return FALSE;
	
		AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pcsLog->m_szWorld);
		if (pAgsdServer)
		{
			pcsLog->m_lServerID = pAgsdServer->m_lServerID;
			SendLogPacket(pcsLog->m_cLogType, pvLog, nSize, pAgsdServer->m_dpnidServer);
		}
		return TRUE;
	}		

	//게임 서버면 릴레이 서버로 포스팅
	if(IsGameServer())
	{
		AgsdServer *pAgsdServer = m_pAgsmServerManager->GetThisServer();
		pcsLog->m_lServerID = pAgsdServer ? pAgsdServer->m_lServerID : 0;
		SendLogPacket(pcsLog->m_cLogType, pvLog, nSize, GetRelayServerNID());
		return TRUE;
	}

	if(!IsRelayServer())
		return FALSE;

	// 릴레이 서버면 AgsmRelay2 를 통해 DB로...
	EnumCallback(AGSMLOG_CB_WRITE_LOG, pcsLog, pvLog);

	return TRUE;
}

BOOL AgsmLog::SendLogPacket(INT8 cOperation, PVOID pvLog, INT16 nSize, UINT32 ulNID)
{
	if(!pvLog || nSize < 1 || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeLogPacket(&nPacketLength, cOperation, pvLog, nSize);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_csPacket.FreePacket(pvPacket);
	
	return bSendResult;
}

PVOID AgsmLog::MakeLogPacket(INT16* pnPacketLength, INT8 cOperation, PVOID pvLog, INT16 nSize)
{
	if(!pnPacketLength || !pvLog || nSize < 1)
		return NULL;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
								&cOperation,
								pvLog, &nSize);
}

BOOL AgsmLog::CBWriteLog(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmLog* pThis = reinterpret_cast<AgsmLog*>(pClass);
	INT16* pnLength = reinterpret_cast<INT16*>(pCustData);

	return pThis->WriteLog(pData, *pnLength);
}

BOOL AgsmLog::SetCallbackWriteLog(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMLOG_CB_WRITE_LOG, pfCallback, pClass);
}








/*
BOOL AgsmLog::OnReceiveLogin(AgpdLog* pcsAgpdLog, PVOID pvLoginPacket)
{
	if(!pcsAgpdLog || !pvLoginPacket)
		return FALSE;

	CHAR* szAccName = NULL;
	CHAR* szWorld = NULL;
	INT8 cFlag = 0;
	INT32 lPlayTime = 0;

	m_csLoginPacket.GetField(FALSE, pvLoginPacket, 0,
							&szAccName,
							&szWorld,
							&cFlag,
							&lPlayTime);
	
	AgpdLog_Loginout cLog;
	
	// 기본 데이터 세팅
	cLog.SetBasicData(pcsAgpdLog);

	// Packet 에서 받은 데이터 세팅
	if(szAccName)
		strncpy(cLog.m_szAccName, szAccName, AGPACHARACTER_MAX_ID_STRING);
	if(szWorld)
		strncpy(cLog.m_szWorld, szWorld, AGPACHARACTER_MAX_ID_STRING);
	cLog.m_cFlag = cFlag;
	cLog.m_lPlayTime = lPlayTime;

	WriteLog(&cLog);

	return TRUE;
}

BOOL AgsmLog::OnReceiveCombat(AgpdLog* pcsAgpdLog, PVOID pvCombatPacket)
{
	if(!pcsAgpdLog || !pvCombatPacket)
		return FALSE;

	CHAR* szTargetName = NULL;
	INT32 lTargetID = 0;
	INT32 lTargetLevel = 0;
	INT32 lExp = 0;
	INT8 cWin = 0;
	INT8 cParty = 0;
	INT32 lAttackCount = 0;
	INT32 lSkillUseCount = 0;
	INT32 lCombatTime = 0;
	INT32* parrHPPotion = NULL;
	INT32* parrMPPotion = NULL;
	INT32* parrSPPotion = NULL;
	INT16 nHPPotionArrayLength = 0;
	INT16 nMPPotionArrayLength = 0;
	INT16 nSPPotionArrayLength = 0;

	m_csCombatPacket.GetField(FALSE, pvCombatPacket, 0,
							&szTargetName,
							&lTargetID,
							&lTargetLevel,
							&lExp,
							&cWin,
							&cParty,
							&lAttackCount,
							&lSkillUseCount,
							&lCombatTime,
							&parrHPPotion, &nHPPotionArrayLength,
							&parrMPPotion, &nMPPotionArrayLength,
							&parrSPPotion, &nSPPotionArrayLength);

	AgpdLog_Combat cLog;

	// 기본 데이터 세팅
	cLog.SetBasicData(pcsAgpdLog);

	// 패킷에서 받은 데이터 세팅
	if(szTargetName)
		strncpy(cLog.m_szTargetName, szTargetName, AGPACHARACTER_MAX_ID_STRING);
	cLog.m_lTargetTID = lTargetID;
	cLog.m_lTargetLevel = lTargetLevel;
	cLog.m_lExp = lExp;
	cLog.m_bWin = (BOOL)cWin;
	cLog.m_bParty = (BOOL)cParty;
	cLog.m_lAttackCount = lAttackCount;
	cLog.m_lSkillUseCount = lSkillUseCount;
	cLog.m_lCombatTime = lCombatTime;

	for(int i = 0; i < 3; i++)
	{
		cLog.m_nArrHPPotion[i] = parrHPPotion[i];
		cLog.m_nArrMPPotion[i] = parrMPPotion[i];
		cLog.m_nArrSPPotion[i] = parrSPPotion[i];
	}
	
	WriteLog(&cLog);

	return TRUE;
}

BOOL AgsmLog::OnReceiveSkill(AgpdLog* pcsAgpdLog, PVOID pvSkillPacket)
{
	if(!pcsAgpdLog || !pvSkillPacket)
		return FALSE;

	INT32 lRemainPoint = 0;
	INT32* parrMasteryPoint = NULL;
	INT16 lArraySize = -1;
	INT32 lSkillTID = 0;
	INT32 lCount = 0;

	m_csSkillPacket.GetField(FALSE, pvSkillPacket, 0,
							&lRemainPoint,
							&parrMasteryPoint, &lArraySize,
							&lSkillTID,
							&lCount);

	switch(pcsAgpdLog->m_cLogType)
	{
		case AGPDLOGTYPE_SKILL_POINT:
		{
			if(!parrMasteryPoint || lArraySize < 0)
				break;

			if(lArraySize != sizeof(INT32) * AGPDLOG_MAX_SKILL_MASTERY)
				break;
			
			AgpdLog_SkillPoint cLog;
			
			// 기본 데이터 세팅
			cLog.SetBasicData(pcsAgpdLog);

			// 패킷 데이터 세팅
			cLog.m_lRemainPoint = lRemainPoint;
			for(int i = 0; i < AGPDLOG_MAX_SKILL_MASTERY; i++)
				cLog.m_arrMasteryPoint[i] = parrMasteryPoint[i];

			WriteLog(&cLog);
			
			break;
		}

		case AGPDLOGTYPE_SKILL_USE:
		{
			AgpdLog_SkillUse cLog;

			// 기본 데이터 세팅
			cLog.SetBasicData(pcsAgpdLog);

			// 패킷 데이터 세팅
			cLog.m_lSkillTID = lSkillTID;
			cLog.m_lCount = lCount;

			WriteLog(&cLog);
			
			break;
		}
	}

	return TRUE;
}

BOOL AgsmLog::OnReceiveItem(AgpdLog* pcsAgpdLog, PVOID pvItemPacket)
{
	if(!pcsAgpdLog || !pvItemPacket)
		return FALSE;

	INT64 lItemDBID = 0;
	INT32 lItemTID = 0;
	INT32 lItemCount = 0;
	INT8 cFlag = -1;
	CHAR* szBuyerName = NULL;
	INT8 cConvertLevel = 0;
	INT32 lConvertTID = 0;
	INT8 cConvertResult = 0;
	INT32 lMoney = 0;

	m_csItemPacket.GetField(FALSE, pvItemPacket, 0,
						&lItemDBID,
						&lItemTID,
						&lItemCount,
						&cFlag,
						&szBuyerName,
						&cConvertLevel,
						&lConvertTID,
						&cConvertResult,
						&lMoney);

	switch(pcsAgpdLog->m_cLogType)
	{
		case AGPDLOGTYPE_ITEM_PICKUSE:
		{
			AgpdLog_ItemPickUse cLog;

			// 기본 데이터 세팅
			cLog.SetBasicData(pcsAgpdLog);

			// 패킷 데이터 세팅
			cLog.m_lItemDBID = lItemDBID;
			cLog.m_lItemTID = lItemTID;
			cLog.m_lCount = lItemCount;
			cLog.m_cFlag = cFlag;

			WriteLog(&cLog);

			break;
		}

		case AGPDLOGTYPE_ITEM_TRADE:
		{
			AgpdLog_ItemTrade cLog;

			// 기본 데이터 세팅
			cLog.SetBasicData(pcsAgpdLog);

			// 패킷 데이터 세팅
			cLog.m_lItemDBID = lItemDBID;
			cLog.m_lItemTID = lItemTID;
			if(szBuyerName)
				strncpy(cLog.m_szBuyerName, szBuyerName, AGPACHARACTER_MAX_ID_STRING);
			cLog.m_lCount = lItemCount;

			WriteLog(&cLog);

			break;
		}

		case AGPDLOGTYPE_ITEM_CONVERT:
		{
			AgpdLog_ItemConvert cLog;

			// 기본 데이터 세팅
			cLog.SetBasicData(pcsAgpdLog);

			// 패킷 데이터 세팅
			cLog.m_lItemDBID = lItemDBID;
			cLog.m_lItemTID = lItemTID;
			cLog.m_cConvertLevel = cConvertLevel;
			cLog.m_lConvertTID = lConvertTID;
			cLog.m_cResult = cConvertResult;

			WriteLog(&cLog);
			
			break;
		}

		case AGPDLOGTYPE_ITEM_REPAIR:
		{
			AgpdLog_ItemRepair cLog;

			// 기본 데이터 세팅
			cLog.SetBasicData(pcsAgpdLog);

			// 패킷 데이터 세팅
			cLog.m_lItemDBID = lItemDBID;
			cLog.m_lItemTID = lItemTID;
			cLog.m_lMoney = lMoney;

			WriteLog(&cLog);
			
			break;
		}

		case AGPDLOGTYPE_ITEM_OWNERCHANGE:
		{
			AgpdLog_ItemOwnerChange cLog;

			// 기본 데이터 세팅
			cLog.SetBasicData(pcsAgpdLog);

			// 패킷 데이터 세팅
			cLog.m_llItemDBID = lItemDBID;
			cLog.m_lItemTID = lItemTID;
			if(szBuyerName)
				strncpy(cLog.m_szPrevOwner, szBuyerName, AGPACHARACTER_MAX_ID_STRING);
			cLog.m_lCount = lItemCount;

			WriteLog(&cLog);

			break;
		}
	}

	return TRUE;
}

BOOL AgsmLog::OnReceiveLevelUp(AgpdLog* pcsAgpdLog, PVOID pvLevelPacket)
{
	if(!pcsAgpdLog || !pvLevelPacket)
		return FALSE;

	INT32 lLevelFrom = 0;
	INT32 lLevelTo = 0;
	INT32 lPartyTime = 0;
	INT32 lSoloTime = 0;
	INT32 lHunCount = 0;
	INT32 lDeadCount = 0;

	m_csLevelUpPacket.GetField(FALSE, pvLevelPacket, 0,
						&lLevelFrom,
						&lLevelTo,
						&lPartyTime,
						&lSoloTime,
						&lHunCount,
						&lDeadCount
						);				

	AgpdLog_LevelUp cLog;
	
	// 기본 데이터 세팅
	cLog.SetBasicData(pcsAgpdLog);

	// 패킷 데이터 세팅
	cLog.m_lLevelFrom = lLevelFrom;
	cLog.m_lLevelTo = lLevelTo;
	cLog.m_lPartyTime = lPartyTime;
	cLog.m_lSoloTime = lSoloTime;
	cLog.m_lHuntCount = lHunCount;
	cLog.m_lDeathCount = lDeadCount;

	WriteLog(&cLog);

	return TRUE;
}

BOOL AgsmLog::SendLogPacket(AgpdLog* pcsAgpdLog, UINT32 ulNID)
{
	if(!pcsAgpdLog || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeLogPacket(&nPacketLength, pcsAgpdLog);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_csPacket.FreePacket(pvPacket);
	
	return bSendResult;
}

PVOID AgsmLog::MakeLogPacket(INT16* pnPacketLength, AgpdLog* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	PVOID pvPacket = NULL;
	switch(pcsAgpdLog->m_cLogType)
	{
		case AGPDLOGTYPE_LOGINOUT:
			pvPacket = MakeLoginPacket(pnPacketLength, (AgpdLog_Loginout*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_COMBAT:
			//pvPacket = MakeCombatPacket(pnPacketLength, (AgpdLog_Combat*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_SKILL_POINT:
			pvPacket = MakeSkillPacket(pnPacketLength, (AgpdLog_SkillPoint*)pcsAgpdLog);
			break;
			
		case AGPDLOGTYPE_SKILL_USE:
			pvPacket = MakeSkillPacket(pnPacketLength, (AgpdLog_SkillUse*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_ITEM_PICKUSE:
			pvPacket = MakeItemPacket(pnPacketLength, (AgpdLog_ItemPickUse*)pcsAgpdLog);
			break;
			
		case AGPDLOGTYPE_ITEM_TRADE:
			pvPacket = MakeItemPacket(pnPacketLength, (AgpdLog_ItemTrade*)pcsAgpdLog);
			break;
			
		case AGPDLOGTYPE_ITEM_CONVERT:
			pvPacket = MakeItemPacket(pnPacketLength, (AgpdLog_ItemConvert*)pcsAgpdLog);
			break;
			
		case AGPDLOGTYPE_ITEM_REPAIR:
			pvPacket = MakeItemPacket(pnPacketLength, (AgpdLog_ItemRepair*)pcsAgpdLog);
			break;

		case AGPDLOGTYPE_LEVEL_UP:
			pvPacket = MakeLevelUpPacket(pnPacketLength, (AgpdLog_LevelUp*)pcsAgpdLog);
			break;
			
		case AGPDLOGTYPE_ITEM_OWNERCHANGE:
			pvPacket = MakeItemPacket(pnPacketLength, (AgpdLog_ItemOwnerChange*)pcsAgpdLog);
			break;
	}
	
	return pvPacket;
}

PVOID AgsmLog::MakeLoginPacket(INT16* pnPacketLength, AgpdLog_Loginout* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	PVOID pvLoginPacket = m_csLoginPacket.MakePacket(FALSE, pnPacketLength, 0,
									pcsAgpdLog->m_szAccName,
									pcsAgpdLog->m_szWorld,
									&pcsAgpdLog->m_cFlag,
									&pcsAgpdLog->m_lPlayTime);

	if(!pvLoginPacket)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
									&pcsAgpdLog->m_cLogType,
									pcsAgpdLog->m_szCharName,
									&pcsAgpdLog->m_lCharTID,
									&pcsAgpdLog->m_lCharLevel,
									&pcsAgpdLog->m_lTimeStamp,
									pvLoginPacket,
									NULL,
									NULL,
									NULL,
									NULL);

	m_csLoginPacket.FreePacket(pvLoginPacket);

	return pvPacket;
}

PVOID AgsmLog::MakeCombatPacket(INT16* pnPacketLength, AgpdLog_Combat* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	INT16 nHPPotionArrLength = sizeof(INT32) * 3;
	INT16 nMPPotionArrLength = sizeof(INT32) * 3;
	INT16 nSPPotionArrLength = sizeof(INT32) * 3;
	PVOID pvCombatPacket = m_csCombatPacket.MakePacket(FALSE, pnPacketLength, 0,
									pcsAgpdLog->m_szTargetName,
									&pcsAgpdLog->m_lTargetTID,
									&pcsAgpdLog->m_lTargetLevel,
									&pcsAgpdLog->m_lExp,
									&pcsAgpdLog->m_bWin,
									&pcsAgpdLog->m_bParty,
									&pcsAgpdLog->m_lAttackCount,
									&pcsAgpdLog->m_lSkillUseCount,
									&pcsAgpdLog->m_lCombatTime,
									&pcsAgpdLog->m_nArrHPPotion[0], &nHPPotionArrLength,
									&pcsAgpdLog->m_nArrMPPotion[0], &nMPPotionArrLength,
									&pcsAgpdLog->m_nArrSPPotion[0], &nSPPotionArrLength
									);

	if(!pvCombatPacket)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
									&pcsAgpdLog->m_cLogType,
									pcsAgpdLog->m_szCharName,
									&pcsAgpdLog->m_lCharTID,
									&pcsAgpdLog->m_lCharLevel,
									&pcsAgpdLog->m_lTimeStamp,
									NULL,
									pvCombatPacket,
									NULL,
									NULL,
									NULL);

	m_csCombatPacket.FreePacket(pvCombatPacket);

	return pvPacket;
}

PVOID AgsmLog::MakeSkillPacket(INT16* pnPacketLength, AgpdLog_SkillPoint* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	INT16 lArraySize = sizeof(INT32) * AGPDLOG_MAX_SKILL_MASTERY;
	PVOID pvSkillPacket = m_csSkillPacket.MakePacket(FALSE, pnPacketLength, 0,
									&pcsAgpdLog->m_lRemainPoint,
									&pcsAgpdLog->m_arrMasteryPoint[0], &lArraySize,
									NULL,
									NULL);

	if(!pvSkillPacket)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
									&pcsAgpdLog->m_cLogType,
									pcsAgpdLog->m_szCharName,
									&pcsAgpdLog->m_lCharTID,
									&pcsAgpdLog->m_lCharLevel,
									&pcsAgpdLog->m_lTimeStamp,
									NULL,
									NULL,
									pvSkillPacket,
									NULL,
									NULL);

	m_csSkillPacket.FreePacket(pvSkillPacket);

	return pvPacket;
}

PVOID AgsmLog::MakeSkillPacket(INT16* pnPacketLength, AgpdLog_SkillUse* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	INT16 lArraySize = sizeof(INT32) * AGPDLOG_MAX_SKILL_MASTERY;
	PVOID pvSkillPacket = m_csSkillPacket.MakePacket(FALSE, pnPacketLength, 0,
									NULL,
									NULL,
									&pcsAgpdLog->m_lSkillTID,
									&pcsAgpdLog->m_lCount);

	if(!pvSkillPacket)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
									&pcsAgpdLog->m_cLogType,
									pcsAgpdLog->m_szCharName,
									&pcsAgpdLog->m_lCharTID,
									&pcsAgpdLog->m_lCharLevel,
									&pcsAgpdLog->m_lTimeStamp,
									NULL,
									NULL,
									pvSkillPacket,
									NULL,
									NULL);

	m_csSkillPacket.FreePacket(pvSkillPacket);

	return pvPacket;
}

PVOID AgsmLog::MakeItemPacket(INT16* pnPacketLength, AgpdLog_ItemPickUse* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	PVOID pvItemPacket = m_csItemPacket.MakePacket(FALSE, pnPacketLength, 0,
									&pcsAgpdLog->m_lItemDBID,
									&pcsAgpdLog->m_lItemTID,
									&pcsAgpdLog->m_lCount,
									&pcsAgpdLog->m_cFlag,
									NULL,
									NULL,
									NULL,
									NULL,
									NULL);

	if(!pvItemPacket)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
									&pcsAgpdLog->m_cLogType,
									pcsAgpdLog->m_szCharName,
									&pcsAgpdLog->m_lCharTID,
									&pcsAgpdLog->m_lCharLevel,
									&pcsAgpdLog->m_lTimeStamp,
									NULL,
									NULL,
									NULL,
									pvItemPacket,
									NULL);

	m_csItemPacket.FreePacket(pvItemPacket);

	return pvPacket;
}

PVOID AgsmLog::MakeItemPacket(INT16* pnPacketLength, AgpdLog_ItemTrade* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	PVOID pvItemPacket = m_csItemPacket.MakePacket(FALSE, pnPacketLength, 0,
									&pcsAgpdLog->m_lItemDBID,
									&pcsAgpdLog->m_lItemTID,
									&pcsAgpdLog->m_lCount,
									NULL,
									pcsAgpdLog->m_szBuyerName,
									NULL,
									NULL,
									NULL,
									NULL);

	if(!pvItemPacket)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
									&pcsAgpdLog->m_cLogType,
									pcsAgpdLog->m_szCharName,
									&pcsAgpdLog->m_lCharTID,
									&pcsAgpdLog->m_lCharLevel,
									&pcsAgpdLog->m_lTimeStamp,
									NULL,
									NULL,
									NULL,
									pvItemPacket,
									NULL);

	m_csItemPacket.FreePacket(pvItemPacket);

	return pvPacket;
}

PVOID AgsmLog::MakeItemPacket(INT16* pnPacketLength, AgpdLog_ItemConvert* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	PVOID pvItemPacket = m_csItemPacket.MakePacket(FALSE, pnPacketLength, 0,
									&pcsAgpdLog->m_lItemDBID,
									&pcsAgpdLog->m_lItemTID,
									NULL,
									NULL,
									NULL,
									&pcsAgpdLog->m_cConvertLevel,
									&pcsAgpdLog->m_lConvertTID,
									&pcsAgpdLog->m_cResult,
									NULL);

	if(!pvItemPacket)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
									&pcsAgpdLog->m_cLogType,
									pcsAgpdLog->m_szCharName,
									&pcsAgpdLog->m_lCharTID,
									&pcsAgpdLog->m_lCharLevel,
									&pcsAgpdLog->m_lTimeStamp,
									NULL,
									NULL,
									NULL,
									pvItemPacket,NULL);

	m_csItemPacket.FreePacket(pvItemPacket);

	return pvPacket;
}

PVOID AgsmLog::MakeItemPacket(INT16* pnPacketLength, AgpdLog_ItemRepair* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	PVOID pvItemPacket = m_csItemPacket.MakePacket(FALSE, pnPacketLength, 0,
									&pcsAgpdLog->m_lItemDBID,
									&pcsAgpdLog->m_lItemTID,
									NULL,
									NULL,
									NULL,
									NULL,
									NULL,
									NULL,
									&pcsAgpdLog->m_lMoney);

	if(!pvItemPacket)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
									&pcsAgpdLog->m_cLogType,
									pcsAgpdLog->m_szCharName,
									&pcsAgpdLog->m_lCharTID,
									&pcsAgpdLog->m_lCharLevel,
									&pcsAgpdLog->m_lTimeStamp,
									NULL,
									NULL,
									NULL,
									pvItemPacket,
									NULL);

	m_csItemPacket.FreePacket(pvItemPacket);

	return pvPacket;
}

PVOID AgsmLog::MakeLevelUpPacket(INT16* pnPacketLength, AgpdLog_LevelUp* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	PVOID pvLevelUpPacket = m_csLevelUpPacket.MakePacket(FALSE, pnPacketLength, 0,
									&pcsAgpdLog->m_lLevelFrom,
									&pcsAgpdLog->m_lLevelTo,
									&pcsAgpdLog->m_lPartyTime,
									&pcsAgpdLog->m_lSoloTime,
									&pcsAgpdLog->m_lHuntCount,
									&pcsAgpdLog->m_lDeathCount);

	if(!pvLevelUpPacket)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
									&pcsAgpdLog->m_cLogType,
									pcsAgpdLog->m_szCharName,
									&pcsAgpdLog->m_lCharTID,
									&pcsAgpdLog->m_lCharLevel,
									&pcsAgpdLog->m_lTimeStamp,
									NULL,
									NULL,
									NULL,
									NULL,
									pvLevelUpPacket);

	m_csLevelUpPacket.FreePacket(pvLevelUpPacket);

	return pvPacket;
}

PVOID AgsmLog::MakeItemPacket(INT16* pnPacketLength, AgpdLog_ItemOwnerChange* pcsAgpdLog)
{
	if(!pnPacketLength || !pcsAgpdLog)
		return NULL;

	PVOID pvItemPacket = m_csItemPacket.MakePacket(FALSE, pnPacketLength, 0,
									&pcsAgpdLog->m_llItemDBID,
									&pcsAgpdLog->m_lItemTID,
									&pcsAgpdLog->m_lCount,
									NULL,
									pcsAgpdLog->m_szPrevOwner,
									NULL,
									NULL,
									NULL,
									NULL);

	if(!pvItemPacket)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMLOG_PACKET_TYPE,
									&pcsAgpdLog->m_cLogType,
									pcsAgpdLog->m_szCharName,
									&pcsAgpdLog->m_lCharTID,
									&pcsAgpdLog->m_lCharLevel,
									&pcsAgpdLog->m_lTimeStamp,
									NULL,
									NULL,
									NULL,
									pvItemPacket,
									NULL);

	m_csItemPacket.FreePacket(pvItemPacket);

	return pvPacket;
}

BOOL AgsmLog::WriteLog(AgpdLog* pcsAgpdLog)
{
	if(!pcsAgpdLog)
		return FALSE;

	if(IsGameServer())
	{
		AgsdServer *pAgsdServer = m_pAgsmServerManager->GetThisServer();
		pcsAgpdLog->m_lServerID = pAgsdServer ? pAgsdServer->m_lServerID : 0;
		SendLogPacket(pcsAgpdLog, GetRelayServerNID());
		return TRUE;
	}

	if(!IsRelayServer())
		return FALSE;

	EnumCallback(AGSMLOG_CB_WRITE_LOG, pcsAgpdLog, NULL);

	return TRUE;
}

BOOL AgsmLog::CBWriteLog(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdLog* pcsAgpdLog =  (AgpdLog*)pData;
	AgsmLog* pThis = (AgsmLog*)pClass;

	if(!pcsAgpdLog || !pThis)
		return FALSE;

	return pThis->WriteLog(pcsAgpdLog);
}

BOOL AgsmLog::SetCallbackWriteLog(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMLOG_CB_WRITE_LOG, pfCallback, pClass);
}
*/