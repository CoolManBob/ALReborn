/******************************************************************************
Module:  AgsmDeath.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 11
******************************************************************************/

#include "AgsmDeath.h"
#include "AgpmBattleGround.h"
#include "AgsmBattleGround.h"
#include "AgpmEpicZone.h"
#include "AgsmEpicZone.h"
#include "AgpmAI2.h"

const INT32		AGSMDEATH_EXP_LEVEL_DIFF = 12;

AgsmDeath::AgsmDeath()
{
	SetModuleName("AgsmDeath");

	//EnableIdle2(TRUE);

	SetModuleType(APMODULE_TYPE_SERVER);

	SetPacketType(AGSMDEATH_PACKET_TYPE);

	m_nIdleCount	= 0;

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_PACKET,			1,			// m_csPacketDeath
							AUTYPE_PACKET,			1,			// m_csPacketDeathADItem
							AUTYPE_END,				0
							);

	m_csPacketDeath.SetFlagLength(sizeof(INT8));
	m_csPacketDeath.SetFieldType(
							AUTYPE_INT32,			1,			// character id
							AUTYPE_UINT32,			1,			// m_ulDeadTime
							AUTYPE_INT64,			1,			// m_lLastLoseExp
							AUTYPE_PACKET,			1,			// m_csLastHitBase
							AUTYPE_UINT32,			1,			// current clock count
							AUTYPE_END,				0
							);

	m_csPacketDeathADItem.SetFlagLength(sizeof(INT8));
	m_csPacketDeathADItem.SetFieldType(
							AUTYPE_INT32,			1,			// item id
							AUTYPE_INT32,			1,			// m_lDropRate
							AUTYPE_END,				0
							);

	m_pagpmLog = NULL;
	m_pagpmPvP = NULL;
	m_pagpmBattleGround = NULL;
	m_pagsmBattleGround = NULL;

	//m_partyBonus = g_eServiceArea == AP_SERVICE_AREA_JAPAN ? 0.2 : 0.1;
	m_partyBonus = 0.2;
}

AgsmDeath::~AgsmDeath()
{
}

BOOL AgsmDeath::OnAddModule()
{
	m_papmMap			= (ApmMap *)		GetModule("ApmMap");
	m_pagpmGrid			= (AgpmGrid *)		GetModule("AgpmGrid");

	m_pagpmFactors		= (AgpmFactors *)	GetModule("AgpmFactors");
	m_pagpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmUnion		= (AgpmUnion *)		GetModule("AgpmUnion");
	m_pagpmItem			= (AgpmItem *)		GetModule("AgpmItem");
	m_pagpmSkill		= (AgpmSkill *)		GetModule("AgpmSkill");
	m_pagpmItemConvert	= (AgpmItemConvert *)	GetModule("AgpmItemConvert");
	//m_pagpmShrine		= (AgpmShrine *)	GetModule("AgpmShrine");
	m_pagpmParty		= (AgpmParty *)		GetModule("AgpmParty");
	m_pagpmPvP			= (AgpmPvP *)		GetModule("AgpmPvP");
	m_pagpmSummons		= (AgpmSummons *)	GetModule("AgpmSummons");
	m_pagpmConfig		= (AgpmConfig *)	GetModule("AgpmConfig");

	m_papmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pagpmEventBinding	= (AgpmEventBinding *)	GetModule("AgpmEventBinding");
	m_pagpmLog = (AgpmLog *)	GetModule("AgpmLog");
	m_pagpmBillInfo		= (AgpmBillInfo *)	GetModule("AgpmBillInfo");

	m_pagsmAOIFilter	= (AgsmAOIFilter *) GetModule("AgsmAOIFilter");
	m_pagsmCharacter	= (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pagsmCharManager	= (AgsmCharManager *)	GetModule("AgsmCharManager");
	m_pagsmFactors		= (AgsmFactors *)	GetModule("AgsmFactors");
	m_pagsmCombat		= (AgsmCombat *)	GetModule("AgsmCombat");
	m_pagsmSummons		= (AgsmSummons *)	GetModule("AgsmSummons");
	m_pagsmSkill		= (AgsmSkill *)		GetModule("AgsmSkill");
	m_pagsmItem			= (AgsmItem *)		GetModule("AgsmItem");
	m_pagsmItemManager	= (AgsmItemManager *)	GetModule("AgsmItemManager");
	m_pagsmDropItem2	= (AgsmDropItem2 *)	GetModule("AgsmDropItem2");
	m_pagsmParty		= (AgsmParty *)		GetModule("AgsmParty");
	m_pcsAgsmUsedCharDataPool	= (AgsmUsedCharDataPool *)	GetModule("AgsmUsedCharDataPool");
	m_pcsAgpmEventSpawn	= (AgpmEventSpawn *)	GetModule("AgpmEventSpawn");
	m_pcsAgpmSiegeWar	= (AgpmSiegeWar *)	GetModule("AgpmSiegeWar");

	if (!m_papmMap ||
		!m_pagpmFactors ||
		!m_pagpmCharacter ||
		!m_pagpmUnion ||
		!m_pagpmSkill ||
		//!m_pagpmShrine ||
		!m_pagsmAOIFilter ||
		!m_pagsmCharacter ||
		!m_pagsmFactors ||
		!m_pagsmCombat ||
		!m_pagsmSkill ||
		!m_pagpmItem ||
		!m_pagpmItemConvert ||
		!m_pagpmEventBinding ||
		!m_pagpmBillInfo ||
		!m_pagpmConfig ||
		!m_pcsAgsmUsedCharDataPool ||
		!m_pcsAgpmEventSpawn ||
		!m_pcsAgpmSiegeWar
		)
		return FALSE;

	if (!m_pagpmParty || !m_papmEventManager || !m_pagsmItemManager || !m_pagsmParty || !m_pagsmItem || !m_pagsmCharManager)
		return FALSE;

	// setting attached data(m_nKeeping) in character module
	m_nIndexADCharacter =
		m_pagpmCharacter->AttachCharacterData(this, sizeof(AgsdDeath), ConAgsdDeath, DesAgsdDeath);

	m_nIndexADItem = 
		m_pagpmItem->AttachItemData(this, sizeof(AgsdDeathADItem), NULL, NULL);

	if (m_nIndexADCharacter < 0 || m_nIndexADItem < 0)
		return FALSE;

	if (!m_pagsmCombat->SetCallbackAttack(CBAttackCharacter, this))
		return FALSE;

	if (!m_pagsmSkill->SetCallbackAttack(CBAttackCharacter, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackUpdateChar(CBUpdateCharacter, this))
		return FALSE;
	if (!m_pagpmCharacter->SetCallbackRequestResurrection(CBRequestResurrection, this))
		return FALSE;
	if (!m_pagpmCharacter->SetCallbackResurrectionByOther(CBResurrectionByOther, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackSendCharacterAllServerInfo(CBSendCharacterAllServerInfo, this))
		return FALSE;

	if (!m_pagsmCharManager->SetCallbackDisconnectCharacter(CBDisconnectCharacter, this))
		return FALSE;

	if (m_pagsmSummons)
	{
		if(!m_pagsmSummons->SetCallbackEndPeriodStart(CBEndPeriodStart, this))
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmDeath::OnInit()
{
	m_pagpmBattleGround		= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagsmBattleGround		= (AgsmBattleGround*)GetModule("AgsmBattleGround");
	m_pagpmEpicZone			= (AgpmEpicZone*)GetModule("AgpmEpicZone");
	m_pagsmEpicZone			= (AgsmEpicZone*)GetModule("AgsmEpicZone");
	m_pagpmTitle			= (AgpmTitle*) GetModule("AgpmTitle");
	m_pagpmAI2				= (AgpmAI2*) GetModule("AgpmAI2");
	
	//if(!m_pagpmBattleGround || !m_pagsmBattleGround)
	//	return FALSE;
		
	return TRUE;
}

BOOL AgsmDeath::OnDestroy()
{
	return TRUE;
}

BOOL AgsmDeath::OnIdle2(UINT32 ulClockCount)
{
	PROFILE("AgsmDeath::OnIdle2");

	/*
	if (m_ulRecoveryPointTime > ulClockCount)
		return TRUE;

	m_ulRecoveryPointTime = ulClockCount + AGSMDEATH_RECOVERY_POINT_INTERVAL;
	*/

	return TRUE;
}

AgsdDeath* AgsmDeath::GetADCharacter(PVOID pData)
{
	return (AgsdDeath *) m_pagpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, pData);
}

AgsdDeathADItem* AgsmDeath::GetADItem(PVOID pData)
{
	return (AgsdDeathADItem *) m_pagpmItem->GetAttachedModuleData(m_nIndexADItem, pData);
}

//		ProcessIdleDeath
//	Functions
//		- 죽은걸 처리한다.
//	Arguments
//		- lCID : 처리할 캐릭터 아뒤
//		- pClass : this module pointer
//		- ulClockCount : 현재 system clock count
//	Return value
//		- none
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::ProcessIdleDeath(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	if (!pClass || lCID == AP_INVALID_CID)
		return FALSE;

	AgsmDeath *pThis = (AgsmDeath *) pClass;

	AgpdCharacter*	pCharacter = pThis->m_pagpmCharacter->GetCharacterLock(lCID);
	if (pCharacter == NULL)
		return FALSE;

	BOOL	bIsRemoved	= FALSE;

	pThis->ProcessIdleDeath(pCharacter, &bIsRemoved);

	if (!bIsRemoved)
		pCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmDeath::ProcessIdleDeath(AgpdCharacter *pCharacter, BOOL *pbIsRemoved)
{
	PROFILE("AgsmDeath::ProcessIdleDeath");

	if (!pCharacter)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessIdleDeatch"));

	switch (pCharacter->m_unActionStatus) {
	case AGPDCHAR_STATUS_NORMAL:
		// 살아있음 암것도 안한다
		break;

	case AGPDCHAR_STATUS_PREDEAD:
		{
			// AGSDCHAR_FLAG_PREDEAD 상태
			if (m_pagsmFactors->IsDead(&pCharacter->m_csFactor))
			{
				AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pCharacter);
				if (pcsAgsdCharacter->m_bResurrectWhenDie && pcsAgsdCharacter->m_lResurrectHP > 0)
				{
					// 죽은 넘이지만 스킬이나 뭐 기타 등등의 이유로 다시 그자리에서 살아나는 경우이다.
					ResurrectNow(pCharacter);
				}
				else if (!SetDead(pCharacter))
				{
					return FALSE;
				}
			}
			else
			{
				// 이넘 살았네... RoundTripLatency 지나기 전에 물약을 먹었던지 암튼... HP를 회복했다. 다시 살려준다.
				if (!NotDead(pCharacter))
				{
					return FALSE;
				}
			}
		}

		break;

	case AGPDCHAR_STATUS_DEAD:
		// AGSDCHAR_FLAG_DEAD 상태
		// 언제 어디서 살아날지 보고 조건에 부합하면 살려낸다.

		BOOL	bDestroyed;
		if (!Resurrection(pCharacter, &bDestroyed))
		{
			return FALSE;
		}

		if (bDestroyed && pbIsRemoved)
		{
			*pbIsRemoved = TRUE;
		}

		break;
	};

	return TRUE;
}

BOOL AgsmDeath::ProcessDeathResult(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	PROFILE("AgsmDeath::ProcessDeathResult");

	AgsmDeath		*pThis				= (AgsmDeath *)	pClass;

	AgpdCharacter	*pcsCharacter		= pThis->m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return TRUE;

	AgsdDeath		*pcsAgsdDeath		= pThis->GetADCharacter(pcsCharacter);
	if (!pcsAgsdDeath)
	{
		pcsCharacter->m_Mutex.Release();
		return TRUE;
	}

	if (!pcsAgsdDeath->m_bIsNeedProcessDeath)
	{
		pcsCharacter->m_Mutex.Release();
		return TRUE;
	}

	/*
	BOOL	bAddIdleEvent	= TRUE;

	if (pThis->m_pagpmCharacter->IsPC(pcsCharacter))
		bAddIdleEvent = FALSE;

	pThis->Dead(pcsCharacter, bAddIdleEvent);
	*/

	pThis->Dead(pcsCharacter, TRUE);

	pcsAgsdDeath->m_bIsNeedProcessDeath	= FALSE;

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmDeath::SetDead(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("SetDead"));

	AgsdDeath	*pcsAgsdDeath	= GetADCharacter(pcsCharacter);
	if (!pcsAgsdDeath)
		return FALSE;
		
	// 리버스오브 쿨타임 동기화 - arycoat 2008.05.19
	m_pagsmItem->SendPacketUpdateReverseOrbReuseTime(pcsCharacter, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	// 죽기 전에 사용하는 스킬 써준다. 2007.07.10. steeple
	m_pagsmSkill->ProcessActionOnActionType6(pcsCharacter);

	pcsAgsdDeath->m_ulDeadTime = GetClockCount();

	pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

	m_pagpmCharacter->StopCharacter(pcsCharacter, NULL);

	m_pagpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_DEAD);

	m_pagsmSkill->EndAllBuffedSkillExceptTitleSkill((ApBase *) pcsCharacter, TRUE);

	// 2005.07.07. steeple
	// 걸려있는 모든 SpecialStatus 를 풀어준다.
	m_pagsmCharacter->CheckSpecialStatusIdleTime(pcsCharacter, 0xFFFFFFFF);		// 끝나는 시간을 크게 줘버려서 끝나게 해버린다.

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (pcsAgsdCharacter)
		pcsAgsdCharacter->m_DetailInfo.bRemoveByDead = true;

	// 소환수 소환한 상태라면 다 없애뿐진다. 2005.10.10. steeple
	if(m_pagsmSummons)
	{
		if(m_pagpmCharacter->IsPC(pcsCharacter))
		{
			m_pagsmSummons->RemoveAllSummons(pcsCharacter, 
							AGSMSUMMONS_CB_REMOVE_TYPE_SUMMONS | AGSMSUMMONS_CB_REMOVE_TYPE_TAME | AGSMSUMMONS_CB_REMOVE_TYPE_FIXED);

			// 죽은 후에는 Max Summons 개수 초기화 시켜준다.
			m_pagpmSummons->SetMaxSummonsCount(pcsCharacter, 1);
		}
	}

	// 2007.07.25. steeple
	// PC 라면 Idle 에서 처리가 아니라 즉시 처리해준다.
	if(m_pagpmCharacter->IsPC(pcsCharacter) && _tcslen(pcsCharacter->m_szID) > 0)
	{
		Dead(pcsCharacter, TRUE);
		m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_TEN_SECONDS);
	}
	else if (pcsCharacter->m_pcsCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_SUMMON)
	{
		Dead(pcsCharacter, TRUE);
		m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_HALF_SECOND);
	}
	else
	{
		pcsAgsdDeath->m_bIsNeedProcessDeath = TRUE;
		m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_ONE_SECOND);
	}

	return TRUE;
}

//		Dead
//	Functions
//		- 이넘 진따로 죽었다. 처리를 한다.
//			1. 죽은 정보를 주변 넘들에게 보낸다.
//			2. 어떤 넘한테 죽었는지 보고 그에 해당하는 처리를 한다.
//			3. 살아날 시간을 OnIdle event에 등록한다. 
//	Arguments
//		- pCharacter : 죽은 넘
//	Return value
//		- none
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::Dead(AgpdCharacter *pCharacter, BOOL bAddIdleEvent)
{
	PROFILE("AgsmDeath::Dead");

	if (!pCharacter)
		return FALSE;

	AgsdDeath *		pcsDeath = GetADCharacter(pCharacter);
	if (!pcsDeath)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("Dead"));

	EnumCallback(AGSMDEATH_CB_PRE_PROCESS_CHARACTER_DEAD, pCharacter, NULL);

	pcsDeath->m_bIsNeedProcessDeath	= FALSE;

	// 죽은넘에 대한 Exp, Penalty 등등, PK, UvU 같은거 다 처리한다.
	if (!ProcessCombatResult(pCharacter))
		return FALSE;

	INT32	lMaxHP	= 0;
	m_pagpmFactors->GetValue(&pCharacter->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	if (lMaxHP < 1)
		return FALSE;

	ApBase	*pcsMostAttackerBase	= NULL;
	ApBase	*pcsRealAttackerBase	= NULL;
	AgsdCharacterHistoryEntry *pcsEntry = m_pagsmCharacter->GetMostDamager(pCharacter);
	if (pcsEntry)
	{
		pcsMostAttackerBase	= m_papmEventManager->GetBase(pcsEntry->m_csSource.m_eType, pcsEntry->m_csSource.m_lID);

		// 2005.01.06. steeple
		// pcsMostAttackerBase 가 Lock 안 되어 있길래 Lock 을 해줌
		if(pcsMostAttackerBase)
		{
			if (!pcsMostAttackerBase->m_Mutex.WLock())
				pcsMostAttackerBase = NULL;

			if(pcsMostAttackerBase && pcsMostAttackerBase->m_eType == APBASE_TYPE_CHARACTER)
			{
				AgpdCharacter *pcsAttacker = m_pagpmCharacter->GetCharacter(pcsMostAttackerBase->m_lID);

				if(pcsAttacker && (m_pagpmCharacter->IsStatusTame(pcsAttacker) || m_pagpmCharacter->IsStatusSummoner(pcsAttacker)))
				{
					INT32 lOwnerCID = m_pagpmSummons->GetOwnerCID(pcsAttacker);
					if(0 != lOwnerCID)
						pcsRealAttackerBase = (ApBase*) m_pagpmCharacter->GetCharacterLock(lOwnerCID);
				}
			}

			if(!pcsRealAttackerBase)
				pcsRealAttackerBase = pcsMostAttackerBase;
		}
	}
	
	if(pcsEntry)
	{
		AgpdCharacter* pcsMostDamager = NULL;

		// 캐릭터라면
		if(pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
		{
			pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
		}
		else if(pcsEntry->m_csSource.m_eType == APBASE_TYPE_PARTY)
		{
			pcsEntry = m_pagsmCharacter->GetMostDamagerInParty(pCharacter, pcsEntry->m_csSource.m_lID);
			if(pcsEntry && pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
				pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
		}
	
		//if(m_pagsmBattleGround && m_pagpmBattleGround->IsInBattleGround(pCharacter))
		if(m_pagsmBattleGround)// 히로익 존에서 카리스마 포인트 획득을 위하여..OnDead 안에서 카리스마 포인트를 처리허고 배틀그라운드 인지 여부를 확인 한다..
			m_pagsmBattleGround->OnDead(pcsMostDamager, pCharacter);

		if(m_pagsmEpicZone && m_pagpmEpicZone->IsInEpicZone(pCharacter))
			m_pagsmEpicZone->OnDeadProcess(pCharacter, pcsMostDamager);
	}

	// 2005.07.28. steeple
	// 누구한테 어떻게 죽었는 지는 초기화 한다.
	// 아래 콜백 중 AgsmPvP 에서 처리된다.
	pcsDeath->m_lDeadTargetCID = 0;
	pcsDeath->m_cDeadType = AGPMPVP_TARGET_TYPE_MOB;

	EnumCallback(AGSMDEATH_CB_CHARACTER_DEAD, pCharacter, pcsRealAttackerBase);
	
	m_pagsmDropItem2->DropItem( pCharacter, pcsRealAttackerBase );

	// 2005.01.06. steeple
	// pcsRealAttackerBase 의 Lock 을 풀어줌.

	if(pcsMostAttackerBase && pcsRealAttackerBase != pcsMostAttackerBase)
		pcsMostAttackerBase->m_Mutex.Release();

	if(pcsRealAttackerBase)
		pcsRealAttackerBase->m_Mutex.Release();

	AgsdCharacterHistory	*pcsHistory = m_pagsmCharacter->GetHistory(pCharacter);
	if (pcsHistory)
	{
		for (int i = 0; i < pcsHistory->m_lEntryNum; i++)
		{
			if (pcsHistory->m_astEntry[i].m_lPartyID != AP_INVALID_PARTYID)
				continue;

			if (pcsHistory->m_astEntry[i].m_csSource.m_eType == APBASE_TYPE_CHARACTER)
			{
				AgpdCharacter	*pcsHistoryTarget	= m_pagpmCharacter->GetCharacterLock(pcsHistory->m_astEntry[i].m_csSource.m_lID);
				if (pcsHistoryTarget)
				{
					if (m_pagsmCharacter->RemoveHistory(pcsHistoryTarget, (ApBase *) pCharacter))
						--i;

					pcsHistoryTarget->m_Mutex.Release();
				}
			}
		}
	}

	m_pagsmCharacter->InitHistory(pCharacter);

	// 2005.04.27. steeple
	m_pagsmCharacter->InitPartyHistory(pCharacter);

	// 2005.11.29. steeple
	m_pagsmCharacter->InitTargetInfoArray(pCharacter);


	// player character 인 경우만 다쉬 살린다.
	//if (((AgpdCharacterTemplate *) pCharacter->m_pcsCharacterTemplate)->m_lID < 7 || 
	//	((AgpdCharacterTemplate *) pCharacter->m_pcsCharacterTemplate)->m_lID > 13)
	//{
	/*
		if (bAddIdleEvent && !RegisterDeath(pCharacter))
			return FALSE;
	*/
	//}

	return TRUE;
}

//		CheckKiller
//	Functions
//		- pCharacter 를 때린 pcsAttack가 어떤넘인지 검사한다.
//			AGSMDEATH_KILLER_PK		: PK 당했다.
//			AGSMDEATH_KILLER_NPC	: 쪽팔리다. 말도 못하겠다.
//			AGSMDEATH_KILLER_UNION	: 상대편 유니온한테 맞아 죽었다.
//	Arguments
//		- pCharacter : 죽은 넘
//		- pcsAttacker : pCharacter를 때린 넘
//	Return value
//		- killer type
///////////////////////////////////////////////////////////////////////////////
INT32 AgsmDeath::CheckKiller(AgpdCharacter *pCharacter, ApBase *pcsAttacker)
{
	if (!pCharacter || !pcsAttacker)
		return (-1);

	AgpdCharacter	*pcsAttackCharacter = NULL;

	if (pcsAttacker->m_eType == APBASE_TYPE_CHARACTER)
	{
		pcsAttackCharacter = (AgpdCharacter *) pcsAttacker;
	}
	else if (pcsAttacker->m_eType == APBASE_TYPE_PARTY)
	{
		pcsAttackCharacter = m_pagpmCharacter->GetCharacter(((AgpdParty *) pcsAttacker)->m_lMemberListID[0]);
	}

	if (!pcsAttackCharacter)
		return (-1);

	// npc 한테 죽었다.
	if (m_pagsmCharacter->GetCharDPNID(pcsAttackCharacter) == 0)
		return AGSMDEATH_KILLER_NPC;

	AgsdDeath	*pcsAgsdDeath = GetADCharacter((PVOID) pCharacter);
	if (!pcsAgsdDeath)
		return (-1);

	INT32	lKilledUnion = m_pagpmCharacter->GetUnion(pCharacter);

	INT32	lAttackerUnion = m_pagpmCharacter->GetUnion(pcsAttackCharacter);

	if (lKilledUnion == lAttackerUnion)
		return AGSMDEATH_KILLER_PK;
	else
		return AGSMDEATH_KILLER_UNION;

	return (-1);
}

BOOL AgsmDeath::ProcessCombatResult(AgpdCharacter *pCharacter)
{
	PROFILE("AgsmDeath::ProcessCombatResult");

	if (!pCharacter)
		return FALSE;

	if (!m_pagpmCharacter->IsMonster(pCharacter))
		return TRUE;

	// 2005.10.26. steeple
    // 소환수, 테이밍 이 죽었을 때는 걍 나간다.
	if(m_pagpmCharacter->IsStatusSummoner(pCharacter) ||
		m_pagpmCharacter->IsStatusTame(pCharacter) ||
		m_pagpmCharacter->IsStatusFixed(pCharacter))
		return TRUE;

	AgpdCharacter* pcsMostDamager = NULL;
	AgsdCharacterHistoryEntry* pcsEntry = m_pagsmCharacter->GetMostDamager(pCharacter);
	if(pcsEntry)
	{
		// 캐릭터라면
		if(pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
		{
			pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
		}
		else if(pcsEntry->m_csSource.m_eType == APBASE_TYPE_PARTY)
		{
			pcsEntry = m_pagsmCharacter->GetMostDamagerInParty(pCharacter, pcsEntry->m_csSource.m_lID);
			if(pcsEntry && pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
				pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
		}
	}

	// 죽은넘이 몬스터인 경우이다.
	// 이 경우엔 항상 PlayerVsMonster이다

	INT32	lTotalExp	= GetMonsterExp(pCharacter, pcsMostDamager);
	if (lTotalExp < 1)
		return FALSE;

	AgsdCharacterHistory	*pcsHistory = m_pagsmCharacter->GetHistory(pCharacter);
	if (!pcsHistory)
		return FALSE;
		
	for (int i = 0; i < pcsHistory->m_lEntryNum; i++)
	{
		if (pcsHistory->m_astEntry[i].m_lPartyID != AP_INVALID_PARTYID)
			continue;

		// exp 계산한다.
		GetBonusExp(lTotalExp, pCharacter, &pcsHistory->m_astEntry[i]);
	}

	pcsHistory = m_pagsmCharacter->GetPartyHistory(pCharacter);
	if (pcsHistory)
	{
		for (int i = 0; i < pcsHistory->m_lEntryNum; i++)
		{
			// exp 계산한다.
			GetBonusExpToParty(lTotalExp, pCharacter, &pcsHistory->m_astEntry[i]);
		}
	}

	return TRUE;
}

//		RegisterDeath
//	Functions
//		- 캐릭터가 죽었다. 설정된 시간후에 부활 장소에서 살아나도록 OnIdle Event에 등록한다.
//			캐릭터가 죽었다는 정보를 주변에 보낸다.
//	Arguments
//		- pCharacter : 죽은 캐릭터
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::RegisterDeath(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return FALSE;

	// idle event에 등록한다.
	AgsdDeath	*pcsDeath = GetADCharacter(pCharacter);

	return AddIdleEvent2(pcsDeath->m_ulDeadTime + AGSMDEATH_RES_INTERVAL * 1000, pCharacter->m_lID, this, ProcessIdleDeath, NULL);
}

//		NotDead
//	Functions
//		- PreDead 상태에서 죽지않고 살아났다. 다시 원상복귀 시켜준다.
//	Arguments
//		- pCharacter : 살아난 캐릭터
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::NotDead(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return FALSE;

	// 캐릭터의 상태를 보통 상태로 바꾼다.
	m_pagpmCharacter->UpdateActionStatus(pCharacter, AGPDCHAR_STATUS_NORMAL);

	return TRUE;
}

// ProcessPinchCharacterDeath
// Functions
//		- Pinch Character의 죽음을 진행한다.
// Arguments
//		- pcsCharacter : 죽은 캐릭터
// Return value
//		- BOOL :성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::ProcessPinchCharacterDeath(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar)
{
	if(NULL == pTargetChar || NULL == pAttackChar)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData(pTargetChar);
	if(pcsAgpdAI2ADChar && pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
	{
		if(pcsAgpdAI2ADChar->m_csPinchInfo.ePinchType == AGPMAI2_TYPE_PINCH_WANTED)
		{
			if(pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter)
			{
				AgpdAI2ADChar *pcsAgpdAI2PinchChar = m_pagpmAI2->GetCharacterData(pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter);
				if(pcsAgpdAI2PinchChar)
				{
					if(pcsAgpdAI2PinchChar->m_csPinchInfo.ePinchType == AGPMAI2_TYPE_PINCH_MONSTER)
					{
						vector<AgpdCharacter*>::iterator it = pcsAgpdAI2PinchChar->m_csPinchInfo.pRequestMobVector.begin(); 
						while(it != pcsAgpdAI2PinchChar->m_csPinchInfo.pRequestMobVector.end())
						{
							AgpdCharacter *pcsListCharacter = *it;
							if(pcsListCharacter)
							{
								if(pcsListCharacter->m_lID == pTargetChar->m_lID)
								{
									it = pcsAgpdAI2PinchChar->m_csPinchInfo.pRequestMobVector.erase(it);
									if(pcsAgpdAI2PinchChar->m_csPinchInfo.pRequestMobVector.empty() == TRUE)
									{
										pcsAgpdAI2PinchChar->m_csPinchInfo.pRequestMobVector.clear();
										pcsAgpdAI2PinchChar->m_csPinchInfo.ePinchType = AGPMAI2_TYPE_PINCH_NONE;

										// 강제로 HP를 0을 만들어서 죽인다.
										m_pagpmFactors->SetValue(&pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter->m_csFactor, 0.0f, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
										PrepareDeath(pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter, pAttackChar);
									}
									break;
								}
							}

							it++;
						}

						pcsAgpdAI2ADChar->m_csPinchInfo.ePinchType = AGPMAI2_TYPE_PINCH_NONE;
						pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter = NULL;
					}					
				}
			}
		}
	}

	return TRUE;
}

//		Resurrection
//	Functions
//		- 죽은 캐릭을 부활시킨다.
//	Arguments
//		- pCharacter : 죽은 캐릭터
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::Resurrection(AgpdCharacter *pCharacter, BOOL *pbDestroyed)
{
	*pbDestroyed = FALSE;

	if (pCharacter == NULL)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("Resurrection"));

	// 현재 버프된 모든 스킬을 종료시킨다.
	//m_pagsmSkill->EndAllBuffedSkill((ApBase *) pCharacter);

	/*
	// Criminal Flag가 세팅되어 있다면 없애준다.
	if (pCharacter->m_unCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED)
	{
		m_pagpmCharacter->UpdateCriminalStatus(pCharacter, AGPDCHAR_CRIMINAL_STATUS_INNOCENT);
	}

	// 지정되어 있는 위치에서 부활시킨다.
	// 위치정보, 캐릭터 Factor 등등의 데이타를 업데이트 한다.
	//
	PVOID pvPacketFactor;
	*/

	if (m_pagsmCharacter->GetADCharacter(pCharacter)->m_bDestroyWhenDie)
	{
		INT32	lCID	= pCharacter->m_lID;

		//if (m_pagpmCharacter->IsNPC(pCharacter) || pCharacter->m_bIsProtectedNPC)
		//	AuLogFile("RemoveNPC.log", "Removed by Resurrection()\n");

		pCharacter->m_Mutex.Release();

#ifndef	_USE_CHARDATAPOOL_
		m_pagpmCharacter->RemoveCharacter(lCID);
#else
		if (m_pagpmCharacter->IsPC(pCharacter) ||
			m_pagpmCharacter->IsStatusSummoner(pCharacter) || 
			m_pagpmCharacter->IsStatusFixed(pCharacter) ||
			m_pagpmCharacter->IsStatusTame(pCharacter) ||
			m_pagpmCharacter->m_csMemoryPool.GetRemainCount() < 500)
            m_pagpmCharacter->RemoveCharacter(lCID);
		else
		{
			if (pCharacter->m_Mutex.WLock())
			{
				if (m_pcsAgsmUsedCharDataPool->RemoveWorld(pCharacter))
					m_pcsAgsmUsedCharDataPool->PushUsedData(pCharacter);

				pCharacter->m_Mutex.Release();
			}
		}
#endif

		*pbDestroyed = TRUE;

		return TRUE;
	}

	/*
	if (!m_pagsmFactors->Resurrection(&pCharacter->m_csFactor, &pvPacketFactor))
		return FALSE;

	// 다시 살아났다는 패킷을 보낸다.
	BOOL bSendResult = m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pCharacter);

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	// update character status
	m_pagpmCharacter->UpdateActionStatus(pCharacter, AGPDCHAR_STATUS_NORMAL);

	AgsdDeath	*pcsAgsdDeath	= GetADCharacter(pCharacter);
	if (pcsAgsdDeath)
		pcsAgsdDeath->m_bDropItem	= FALSE;

	// 테스트를 위해 시작위치를 부활 위치를 설정해 놓는다.
	AuPOS	stResPos;
	stResPos.x			= -353236;
	stResPos.y			= 6946.60;
	stResPos.z			= 107239;
	*/

	/*
	AuPOS	stResPos;

	if (!m_pagpmEventBinding->GetBindingPositionForResurrection(pCharacter, &stResPos))
	{
		// 이게 실패한 경우는 정말 일어나면 안되는 경우이나.. 혹시 일어나게되면 아무데로나 텔레포트 시킬 수 없으니
		// 걍 이전 위치에서 부활시킨다.
		stResPos	= pCharacter->m_stPos;
	}
	*/

	/*
	// update position
	m_pagpmCharacter->UpdatePosition(pCharacter, &stResPos, FALSE, TRUE);

	return bSendResult;
	*/

	return TRUE;
}

BOOL AgsmDeath::ResurrectNow(AgpdCharacter *pcsCharacter, BOOL bIsHPFull, BOOL bSetDead)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ResurrectNow"));

	if (bSetDead)
	{
		// 부활시키기 전에 일단 죽인다. 실제로는 걍 패킷만 보내서 클라이언트에서 애니메이션 등을 하게 한다.
		/////////////////////////////////////////////////////////////////////////////////////////////////
		m_pagpmCharacter->StopCharacter(pcsCharacter, NULL);
		m_pagpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_DEAD);
	}

	// 부활시킨다. 바로 시켜야 하나.. 아니면 Delay를 줘야 하나는 기획에서 결정이 나면 그때 처리한다.
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// HP를 lResurrectHP만큼 회복시켜준다.
	PVOID	pvPacketFactor	= NULL;

	if (bIsHPFull)
	{
		if (!m_pagsmFactors->Resurrection(&pcsCharacter->m_csFactor, &pvPacketFactor))
			return FALSE;
	}
	else
		m_pagsmFactors->UpdateCharPoint(&pcsCharacter->m_csFactor, &pvPacketFactor, pcsAgsdCharacter->m_lResurrectHP, 0, 0);

	// factor 패킷을 보낸다.
	BOOL bSendResult = m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_3);
	m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	// 캐릭터의 상태를 보통 상태로 바꾼다.
	m_pagpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_NORMAL);
	
	// 값들을 초기화한다.
	pcsAgsdCharacter->m_bResurrectWhenDie	= FALSE;
	pcsAgsdCharacter->m_lResurrectHP	= 0;

	AgsdDeath	*pcsAgsdDeath	= GetADCharacter(pcsCharacter);
	if (pcsAgsdDeath)
		pcsAgsdDeath->m_bDropItem	= FALSE;

	return bSendResult;
}

// 2007.09.06. steeple
BOOL AgsmDeath::ResurrectionNowNoPenalty(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->IsDead() == FALSE)
		return TRUE;

	if(m_pagpmCharacter->IsAllBlockStatus(pcsCharacter))
		return TRUE;

	BOOL bIsTransform = pcsCharacter->m_bIsTrasform;

	// 변신중이면 원래로 되돌린다.
	if(pcsCharacter->m_bIsTrasform)
		m_pagpmCharacter->RestoreTransformCharacter(pcsCharacter);

	// 지정되어 있는 위치에서 부활시킨다.
	// 위치정보, 캐릭터 Factor 등등의 데이타를 업데이트 한다.
	PVOID pvPacketFactor	= NULL;
	if(!m_pagsmFactors->Resurrection(&pcsCharacter->m_csFactor, &pvPacketFactor))
		return FALSE;

	// 다시 살아났다는 패킷을 보낸다.
	BOOL bSendResult = m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_3);

	if(pvPacketFactor)
		m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	// update character status
	m_pagpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_NORMAL);

	AgsdDeath* pcsAgsdDeath = GetADCharacter(pcsCharacter);
	if(pcsAgsdDeath)
		pcsAgsdDeath->m_bDropItem = FALSE;

	AuPOS stResPos = pcsCharacter->m_stPos;

	// 2007.07.25. steeple
	// DB 저장하기 전에 UpdatePosition 부터 한다.
	//
	// update position
	EnumCallback(AGSMDEATH_CB_RESURRECTION, pcsCharacter, &stResPos);

	// 2005.12.16. steeple
	// 현재 사용중으로 체크 되어 있는 스킬 스크롤 캐쉬 아이템을 사용시켜준다.
	m_pagsmItem->UseAllEnableCashItem(pcsCharacter,
										0,
										AGSDITEM_PAUSE_REASON_NONE,
										bIsTransform);	// 부활 전 상태가 변신 중이었다면, '일시 정지' 상태의 아이템도 사용해준다.
	m_pagsmSkill->RecastSaveSkill(pcsCharacter);
	// 현재 캐릭터 데이타를 모두 DB에 저장한다.
	m_pagsmCharacter->BackupCharacterData(pcsCharacter);
	m_pagsmCharacter->EnumCallback(AGSMCHARACTER_CB_UPDATE_ALL_TO_DB, pcsCharacter, NULL);

	return bSendResult;
}

//		AddBonusExpToChar
//	Functions
//		- Bonus Exp를 반영한다. 즉, 펙터에 반영한다.
//	Arguments
//		- lCID : 보너스를 받을 캐릭터
//		- nBonusExp : 보너스 Exp
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::AddBonusExpToChar(INT32 lCID, INT64 llBonusExp, BOOL bIsLock, BOOL bIsPCBangBonus, BOOL bIsQuest)
{
	AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	BOOL	bRetval = AddBonusExpToChar(pcsCharacter, NULL, llBonusExp, TRUE, bIsPCBangBonus, bIsQuest);

	pcsCharacter->m_Mutex.Release();

	return bRetval;
}

BOOL AgsmDeath::AddBonusExpToChar(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsKilledCharacter, INT64 llBonusExp, BOOL bIsLock, BOOL bIsPCBangBonus, BOOL bIsQuest)
{
	PROFILE("AgsmDeath::AddBonusExpToChar");

	if (!pcsCharacter || llBonusExp == 0)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("AddBonusExpToChar"));

	/*
	AgpdBillInfo	*pcsBillInfo	= m_pagpmBillInfo->GetADCharacter(pcsCharacter);

	// pc방인경우 보너스로 10%를 더준다.
	// 이벤트가 끝나면 빼버린다.
	if (bIsPCBangBonus &&
		pcsBillInfo->m_bIsPCRoom)
	{
		nBonusExp = (INT32)	(nBonusExp * 1.10);
	}
	*/

	INT32 lTitleBonusExp = 0;

	if(pcsKilledCharacter)
	{

		INT32 nEffectSet = 0;
		INT32 nEffectValue1 = 0;
		INT32 nEffectValue2 = 0;

		m_pagpmTitle->GetEffectValue(pcsCharacter, AGPMTITLE_TITLE_EFFECT_HUNT_BONUS_EXP, &nEffectSet, &nEffectValue1, &nEffectValue2);

		if(nEffectValue1 == pcsKilledCharacter->m_pcsCharacterTemplate->m_lID && nEffectSet == 0)
			lTitleBonusExp = (INT32)((FLOAT)llBonusExp * (FLOAT)(nEffectValue2) / 100.0f);
	}

	// 2005.12.02. steeple
	// 경험치 증가용 유료아이템을 사용하고 있다면 증가율을 구해서 곱해준다.
	if(TRUE && !bIsQuest)	// 해당 아이템이 사용중인지 아닌지 체크한다.
	{
		INT64 llBonusExpByCash = m_pagpmCharacter->GetGameBonusExp(pcsCharacter);

		AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(pcsAgsdCharacter)
			llBonusExpByCash += pcsAgsdCharacter->m_stOptionSkillData.m_lBonusExp;
		
		llBonusExp = llBonusExp + (INT32)((FLOAT)llBonusExp * (FLOAT)(llBonusExpByCash) / 100.0f);
	}

	if ( AP_SERVICE_AREA_CHINA == g_eServiceArea )
	{
		if ( pcsCharacter->m_lAddictStatus == 1 )
			llBonusExp = llBonusExp / 2;

		if ( pcsCharacter->m_lAddictStatus >= 2 )
			llBonusExp = 0;
	}

	llBonusExp += lTitleBonusExp;

	INT64	llOriginalBonusExp	= llBonusExp;
	BOOL	bLevelUp		= FALSE;

	// 레벨이 오를 수 있는지 본다.
	INT32	lCharacterLevel	= m_pagpmCharacter->GetLevel(pcsCharacter);

	BOOL	bSkipLevelUp = FALSE;

	if(llBonusExp > 0)
	{
		switch( m_pagpmCharacter->GetCharacterLevelLimit( pcsCharacter ) )
		{
		case	AgpmCharacter::LLS_NONE	:	// 블럭 없음.. 노멀 상황
		case	AgpmCharacter::LLS_LIMITED	:	// 레벨 제한걸림.. 걸린 레벨보다 낮음. 경험치&레벨업 정상획득
			break;
		case	AgpmCharacter::LLS_BLOCKED	:	// 레벨 제한 걸림.. 경험치는 얻으나 레벨은 못올라감.
			bSkipLevelUp = TRUE;
			break;
		case	AgpmCharacter::LLS_DOWNED	:	// 레벨 제한걸림 .. 레벨이 다운되어 있음.
			return FALSE;	// 레벨제한 걸려서 경험치 없음
		}
	}

	// 레벨이 올랐는지 본다.
	INT64	llCharacterExp	= m_pagpmCharacter->GetExp(pcsCharacter);

	INT32	lNumLevelUp		= 0;

	//JK_특성화서버
	INT32	lLimitLevel = m_pagpmConfig->GetLimitLevel();

	if (llBonusExp > 0)
	{
		while (1)
		{
			INT64	llLevelUpExp		= m_pagpmCharacter->GetLevelUpExp(lCharacterLevel);

			if (llLevelUpExp <= llCharacterExp + llBonusExp)
			{
				if( bSkipLevelUp )
				{
					// 레벨업 경험치에 1이 모자라게.
					llBonusExp = llLevelUpExp - llCharacterExp - 1;
					break;
				}
				//JK_특성화서버
				if(lLimitLevel > 0 && lCharacterLevel >= lLimitLevel)
				{
					llBonusExp = llLevelUpExp - llCharacterExp;
					if(llBonusExp < 0)
						return FALSE;

					break;
				}

				if (lCharacterLevel >= (AGPMCHAR_MAX_LEVEL - 1))
				{
					// 마지막이다 여기서 더 올라갈데는 없다. ㅡ.ㅡ...........
					llBonusExp = llLevelUpExp - llCharacterExp;
					if (llBonusExp < 0)
						return FALSE;

					break;
				}
				else
				{
					// 레벨 업 한 경우이다.
					llBonusExp = llCharacterExp + llBonusExp - llLevelUpExp;
					++lCharacterLevel;
					++lNumLevelUp;
					llCharacterExp	= 0;

					llBonusExp	= 0;

					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		if (llCharacterExp + llBonusExp < 0)
			llBonusExp = (-llCharacterExp);
		/*
		while (1)
		{
			if (lCharacterExp + nBonusExp < 0)
			{
				if (lCharacterLevel == 1)
				{
					nBonusExp = (-lCharacterExp);
					break;
				}

				nBonusExp += lCharacterExp;
				--lCharacterLevel;
				--lNumLevelUp;
				lCharacterExp	= m_pagpmCharacter->GetLevelUpExp(lCharacterLevel);
			}
			else
			{
				break;
			}
		}
		*/
	}

	AgpdFactor		csUpdateFactor;
	ZeroMemory(&csUpdateFactor, sizeof(AgpdFactor));

	AgpdFactor	*pcsUpdateResultFactor = (AgpdFactor *) m_pagpmFactors->SetFactor(&csUpdateFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsUpdateResultFactor)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
		return FALSE;
	}

	AgpdFactorCharPoint	*pcsFactorCharPoint = NULL;

	if (lNumLevelUp > 0)
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitUpdateFactor(pcsUpdateResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	else
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsUpdateResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT);

	if (!pcsFactorCharPoint)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
		return FALSE;
	}

	// UpdateFactor에 보너스 Exp를 세팅한다.

	PVOID	pvPacketFactor	= NULL;

	FLOAT	fOriginalBonusExp	= (FLOAT) llOriginalBonusExp;

	//if (bIsPCBangBonus)
	//	fOriginalBonusExp	+= 0.5f;

	// 펙터에 반영하고 변경된 값에 대한 펙터를 받는다.
	if (lNumLevelUp != 0)
	{
		// UpdateFactor에 보너스 Exp를 세팅한다.
		m_pagpmFactors->SetExp((AgpdFactor *) m_pagpmFactors->GetFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_RESULT), llBonusExp);
		m_pagpmFactors->SetValue(&csUpdateFactor, fOriginalBonusExp, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_BONUS_EXP);

		pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactor, &csUpdateFactor);
	}
	else
	{
		// UpdateFactor에 보너스 Exp를 세팅한다.
		m_pagpmFactors->SetExp((AgpdFactor *) m_pagpmFactors->GetFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_RESULT), llBonusExp);
		m_pagpmFactors->SetValue(&csUpdateFactor, fOriginalBonusExp, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_BONUS_EXP);

		pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, &csUpdateFactor, TRUE, TRUE, TRUE, FALSE);
	}

	m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, 0, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_BONUS_EXP);

	m_pagpmFactors->DestroyFactor(&csUpdateFactor);

	if (!pvPacketFactor)
		return FALSE;

	BOOL bSendResult = m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_4);

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	if (lNumLevelUp != 0)
		m_pagpmCharacter->UpdateLevel(pcsCharacter, lNumLevelUp);

	//else
	//	m_pagpmSkill->AdjustSkillPoint(pcsCharacter);


	// 2004.05.18. steeple
	if(lNumLevelUp != 0)
	{
		m_pagsmCharacter->WritePlayLog(pcsCharacter, lNumLevelUp);
	}

	return bSendResult;
}

//		AddBonusExpToParty
//	Functions
//		- Bonus Exp를 반영한다. 즉, 펙터에 반영한다.
//	Arguments
//		- lPartyID : 보너스를 받을 파티
//		- nBonusExp : 보너스 Exp
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::AddBonusExpToParty(AgpdParty *pcsParty, INT64 llBonusExp, INT32 lMemberTotalLevel, INT32 lNumCombatMember, AgpdCharacter **pacsCombatMember)
{
	PROFILE("AgsmDeath::AddBonusExpToParty");

	if (!pcsParty ||
		llBonusExp < 1 ||
		lMemberTotalLevel < 1 ||
		lNumCombatMember < 1 ||
		!pacsCombatMember)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("AddBonusExpToParty"));

	for (int i = 0; i < lNumCombatMember; i++)
	{
		if (!pacsCombatMember[i])
			continue;

		if (pacsCombatMember[i]->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			continue;

		INT32	lMemberLevel	= m_pagpmCharacter->GetLevel(pacsCombatMember[i]);

		INT64	llExp			= (INT64) (llBonusExp * (lMemberLevel / (float) lMemberTotalLevel));
		if (llExp < 1)
			continue;

		if (pacsCombatMember[i]->m_Mutex.WLock())
		{
			AddBonusExpToChar(pacsCombatMember[i], NULL, llExp, TRUE, TRUE);
			pacsCombatMember[i]->m_Mutex.Release();
		}
	}

	/*
	if (nBonusExp < 1 || nMonsterExp < 1 || !pcsKilledChar)
		return FALSE;

	AgpdParty	*pcsParty = m_pagpmParty->GetPartyLock(lPartyID);
	if (!pcsParty)
		return FALSE;

	AgpdCharacter	*pcsCombatMember[AGPMPARTY_MAX_PARTY_MEMBER];
	ZeroMemory(pcsCombatMember, sizeof(AgpdCharacter *) * AGPMPARTY_MAX_PARTY_MEMBER);

	INT32	lMemberTotalLevel	= 0;

	INT32	lNumCombatMember = m_pagsmParty->GetNearMember(pcsParty, pcsKilledChar, pcsCombatMember, &lMemberTotalLevel);
	if (lNumCombatMember < 1 || lMemberTotalLevel < 1)
	{
		pcsParty->m_Mutex.Release();
		return FALSE;
	}

	switch (lNumCombatMember) {
	case 4:
		nBonusExp += nBonusExp * 1.30;
		break;
	case 5:
		nBonusExp += nBonusExp * 1.50;
		break;
	case 6:
		nBonusExp += nBonusExp * 1.75;
		break;
	case 7:
		nBonusExp += nBonusExp * 2.05;
		break;
	case 3:
		nBonusExp += nBonusExp * 1.15;
		break;
	case 2:
		nBonusExp += nBonusExp * 1.05;
		break;
	case 1:
		break;
	default:
		nBonusExp += nBonusExp * 2.40;
		break;
	}

	nBonusExp += nMonsterExp / 10;

	INT32	lMaximumExp	= nMonsterExp * 2;

	// pcsKilledChar와의 거리를 계산해서 넘 멀리 떨어져있는넘들 (지금은 시야내에 있는지 여부)은 제외시킨다.

	for (int i = 0; i < lNumCombatMember; i++)
	{
		if (!pcsCombatMember[i])
			continue;

		INT32	lMemberLevel	= m_pagpmFactors->GetLevel(&pcsCombatMember[i]->m_csFactor);

		INT32	lExp			= (INT32) (nBonusExp * (lMemberLevel / (float) lMemberTotalLevel));
		if (lExp < 1)
			continue;

		if (lExp > lMaximumExp)
			lExp = lMaximumExp;

		AddBonusExpToChar(pcsCombatMember[i], lExp);
	}

	pcsParty->m_Mutex.Release();
	*/

	return TRUE;
}

//		ConAgsdDeath
//	Functions
//		- AgsdCharacter constructor
//	Arguments
//		- pData : character data pointer
//		- pClass : this class pointer
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::ConAgsdDeath(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (pData == NULL || pClass == NULL)
	{
		// invalid constructor parameter
		return FALSE;
	}

	AgsmDeath     *pThis			= (AgsmDeath *) pClass;
	AgpdCharacter *pCharacter		= (AgpdCharacter *) pData;
	AgsdDeath     *pagsdDeath		= pThis->GetADCharacter(pCharacter);

	pagsdDeath->m_ulDeadTime		= 0;

	pagsdDeath->m_bDropItem			= FALSE;

	// 이 값은 DB에서 읽어들인다. 음냐... 음냐...
	pagsdDeath->m_llLastLoseExp		= 0;

	pagsdDeath->m_bIsNeedProcessDeath	= FALSE;

	pagsdDeath->m_ulRecoveryHPPointTime = 0;
	pagsdDeath->m_ulRecoveryMPPointTime = 0;
	pagsdDeath->m_tCharismaGiveTime = 0;

	return TRUE;
}

//		DesAgsdDeath
//	Functions
//		- AgsdCharacter destructor
//	Arguments
//		- pData : character data pointer
//		- pClass : this class pointer
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::DesAgsdDeath(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (pData == NULL || pClass == NULL)
	{
		// invalid constructor parameter
		return FALSE;
	}

	AgsmDeath     *pThis			= (AgsmDeath *) pClass;
	AgpdCharacter *pCharacter		= (AgpdCharacter *) pData;
	AgsdDeath     *pagsdDeath		= pThis->GetADCharacter(pCharacter);

	return TRUE;
}

//		CBAttackCharacter
//	Functions
//		- 캐릭터간 공격이 있는경우 Combat Module에서 처리한후 그 결과를 받아서 공격 결과와 관련된 처리들을 한다.
//			1. HitHistory에 추가한다.
//			2. 타겟이 죽었는지 검사해서 죽었다면 PrepareDeath()를 호출한다.
//	Arguments
//		- pData : attack character
//		- pClass : this module pointer
//		- pCustData : target character
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::CBAttackCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmDeath					*pThis			= (AgsmDeath *) pClass;
	pstAgsmCombatAttackResult	pstAttackResult = (pstAgsmCombatAttackResult) pData;

	if (pThis->m_pagsmFactors->IsDead(&pstAttackResult->pTargetChar->m_csFactor))
		pThis->PrepareDeath(pstAttackResult->pTargetChar, pstAttackResult->pAttackChar);
	
	// 2006.09.14. steeple
	// 자폭 몬스터라면 여기서 죽여준다.
	if(pThis->m_pagpmCharacter->IsMonster(pstAttackResult->pAttackChar) &&
			pstAttackResult->pAttackChar->m_pcsCharacterTemplate->m_bSelfDestructionAttackType)
	{
		pThis->SetDead(pstAttackResult->pAttackChar);
	}

	return TRUE;
}

//		PrepareDeath
//	Functions
//		- 캐릭터가 죽은경우 RoundTripLatency 만큼 기다린다. 그 후에도 죽은상태면 그때 진짜로 죽은넘으로 간주하고 처리한다.
//			1. 캐릭터 상태를 AGPDCHAR_STATUS_PREDEAD로 세팅한다.
//			2. OnIdle() 이벤트에 RoundTripLatency 시간후에 이벤트가 발생하도록 세팅한다.
//				(즉, RoundTripLatency 시간후에 진짜 죽었는지 감별한당)
//			3. AGPDCHAR_STATUS_PREDEAD로 된 시간을 저장한다.
//	Arguments
//		- pTargetChar : 죽은 캐릭터 (HP가 0이된 캐릭터)
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::PrepareDeath(AgpdCharacter *pTargetChar, AgpdCharacter *pAttackChar)
{
	if (!pTargetChar)
		return FALSE;

	if (pTargetChar->m_unActionStatus == AGPDCHAR_STATUS_PREDEAD ||
		pTargetChar->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return TRUE;

	// 캐릭터 상태 AGPDCHAR_STATUS_PREDEAD로 변경
	m_pagpmCharacter->UpdateActionStatus(pTargetChar, AGPDCHAR_STATUS_PREDEAD);
	
	AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pTargetChar);

	// Pinch 설정되어있는 Monster Death를 처리해준다.
	ProcessPinchCharacterDeath(pAttackChar, pTargetChar);

	// 마지막으로 때린넘을 세팅한다.
	if (pAttackChar)
	{
		AgsdDeath *pcsDeath = GetADCharacter((PVOID) pTargetChar);

		pcsDeath->m_csLastHitBase.m_eType	= pAttackChar->m_eType;
		pcsDeath->m_csLastHitBase.m_lID		= pAttackChar->m_lID;
	}

	//if (pcsAgsdCharacter->m_ulRoundTripLatencyMS)		// Latency가 0인지 아닌지 검사
	//{
	/*
		if (!AddIdleEvent2(GetClockCount() + (UINT32) (pcsAgsdCharacter->m_ulRoundTripLatencyMS / 2), pTargetChar->m_lID, this, ProcessIdleDeath, NULL))
		{
			// 변경된 캐릭터 상태 원상복귀
			//

			return FALSE;
		}
	*/
	//}
	//else											// Latency가 0이라면 바로 죽은넘 처리로 들어간다. (NPC 인경우 빼고는 PC는 이런경우는 없다)
	//{
	return ProcessIdleDeath(pTargetChar);
	//}

	//return TRUE;
}

BOOL AgsmDeath::CBUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmDeath		*pThis			= (AgsmDeath *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulClockCount	= PtrToUint(pCustData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBUpdateCharacter"));

	/*
	if (!pThis->m_pagsmCharacter->IsIdleProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, ulClockCount))
		return TRUE;
	*/

	pThis->m_pagsmCharacter->ResetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH);
	pThis->m_pagsmCharacter->SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, ulClockCount);

	pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);

	// 죽은지 1, 2초 후에 아템을 떨구는 등등의 작업을 한다.
	/////////////////////////////////////////////////////////////////////
	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		AgsdDeath		*pcsAgsdDeath	= pThis->GetADCharacter(pcsCharacter);

		if (pcsAgsdDeath->m_bIsNeedProcessDeath)
		{
			if (pcsAgsdDeath->m_ulDeadTime + AGSMDEATH_DROP_ITEM_INTERVAL <= ulClockCount)
			{
				pThis->Dead(pcsCharacter);

				pcsAgsdDeath->m_bIsNeedProcessDeath	= FALSE;

				if (pThis->m_pagpmCharacter->IsPC(pcsCharacter))
				{
					pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_TEN_SECONDS);
				}
				else
				{
					pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
				}
			}
			else
			{
				pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_ONE_SECOND);
			}
		}
		else if ((((pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter) == AGPD_SIEGE_MONSTER_TYPE_NONE || 
					pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter) == AGPD_SIEGE_MONSTER_SUMMON) &&
			     pcsAgsdDeath->m_ulDeadTime
			   + pThis->m_pagsmCharacter->GetReservedTimeForDestory(pcsCharacter)
			   + AGSMDEATH_RES_INTERVAL <= ulClockCount)) ||
			   pcsCharacter->m_ulCharType & AGPMCHAR_TYPE_SUMMON)
		{
			pThis->ProcessIdleDeath(pcsCharacter);
		}
		else
		{
			if (!pThis->m_pagpmCharacter->IsPC(pcsCharacter))
				pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
		}

		/*
		AgsdDeath		*pcsAgsdDeath	= pThis->GetADCharacter(pcsCharacter);
		if (pcsAgsdDeath &&
			!pcsAgsdDeath->m_bDropItem &&
			pcsAgsdDeath->m_ulDeadTime + 1700 < ulClockCount)
		{
			pThis->ProcessDropItem(pcsCharacter);

			pcsAgsdDeath->m_bDropItem	= TRUE;
		}
		*/
	}
	else
	{
		if (pThis->m_pagpmCharacter->IsPC(pcsCharacter))
		{
			// 각종 Point를 회복시킨다.

			if (pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD ||
				pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_PREDEAD)
			{
				return TRUE;
			}

			AgsdDeath	*pcsAgsdDeath	= pThis->GetADCharacter(pcsCharacter);
			AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

			BOOL	bIsRecoveryHP	= FALSE;
			BOOL	bIsRecoveryMP	= FALSE;

			if (pcsAgsdDeath->m_ulRecoveryHPPointTime <= ulClockCount)
				bIsRecoveryHP	= TRUE;

			if (pcsAgsdDeath->m_ulRecoveryMPPointTime <= ulClockCount)
				bIsRecoveryMP	= TRUE;

			if (!bIsRecoveryHP && !bIsRecoveryMP)
				return TRUE;

			INT32	lCon	= 0;
			INT32	lInt	= 0;

			pThis->m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCon, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
			pThis->m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lInt, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

			if (bIsRecoveryHP)
				pcsAgsdDeath->m_ulRecoveryHPPointTime	= ulClockCount + max(AGSMDEATH_RECOVERY_POINT_INTERVAL, (INT32) (10.5f - (lCon / 100.0f) * 1000));
			if (bIsRecoveryMP)
				pcsAgsdDeath->m_ulRecoveryMPPointTime	= ulClockCount + max(AGSMDEATH_RECOVERY_POINT_INTERVAL, (INT32) (10.5f - (lInt / 100.0f) * 1000));

			INT32	lModifiedPointHP	= 0;
			INT32	lModifiedPointMP	= 0;

			if (bIsRecoveryHP)
				lModifiedPointHP	= pThis->m_pagpmSkill->GetModifiedHPRegen(pcsCharacter)
									+ pcsAgsdCharacter->m_stOptionSkillData.m_lRegenHP;
			if (bIsRecoveryMP)
				lModifiedPointMP	= pThis->m_pagpmSkill->GetModifiedMPRegen(pcsCharacter)
									+ pcsAgsdCharacter->m_stOptionSkillData.m_lRegenMP;

			PVOID	pvPacketFactor = NULL;
			if (!pThis->m_pagsmFactors->RecoveryPointInterval(&pcsCharacter->m_csFactor, &pvPacketFactor, lModifiedPointHP, lModifiedPointMP, bIsRecoveryHP, bIsRecoveryMP) || !pvPacketFactor)
			{
				return TRUE;
			}

			INT16	nPacketLength = 0;
			INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;

			PVOID	pvPacket = pThis->m_pagpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																		&cOperation,							// Operation
																		&pcsCharacter->m_lID,					// Character ID
																		NULL,									// Character Template ID
																		NULL,									// Game ID
																		NULL,									// Character Status
																		NULL,									// Move Packet
																		NULL,									// Action Packet
																		pvPacketFactor,							// Factor Packet
																		NULL,									// llMoney
																		NULL,									// bank money
																		NULL,									// cash
																		NULL,									// character action status
																		NULL,									// character criminal status
																		NULL,									// attacker id (정당방위 설정에 필요)
																		NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																		NULL,									// region index
																		NULL,									// social action index
																		NULL,									// special status
																		NULL,									// is transform status
																		NULL,									// skill initialization text
																		NULL,									// face index
																		NULL,									// hair index
																		NULL,									// Option Flag
																		NULL,									// bank size
																		NULL,									// event status flag
																		NULL,									// remained criminal status time
																		NULL,									// remained murderer point time
																		NULL,									// nick name
																		NULL,									// gameguard
																		NULL									// last killed time in battlesquare
																		);

			if(pvPacketFactor)
				pThis->m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

			if(pvPacket)
			{
				pThis->m_pagpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

				if(!pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5))
				{
					//TRACEFILE("AgsmDeath::CBUpdateCharacter() 에서 SendPacketNear() 실패");
				}

				pThis->m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return TRUE;
}

// 2005.04.09. steeple
BOOL AgsmDeath::SendUpdateRecoveryPoint(AgpdCharacter* pcsCharacter, PVOID pvPacketFactor)
{
	if(!pcsCharacter || !pvPacketFactor)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;

	PVOID	pvPacket = m_pagpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,							// Operation
																&pcsCharacter->m_lID,					// Character ID
																NULL,									// Character Template ID
																NULL,									// Game ID
																NULL,									// Character Status
																NULL,									// Move Packet
																NULL,									// Action Packet
																pvPacketFactor,							// Factor Packet
																NULL,									// llMoney
																NULL,									// bank money
																NULL,									// cash
																NULL,									// character action status
																NULL,									// character criminal status
																NULL,									// attacker id (정당방위 설정에 필요)
																NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,									// region index
																NULL,									// social action index
																NULL,									// special status
																NULL,									// is transform status
																NULL,									// skill initialization text
																NULL,									// face index
																NULL,									// hair index
																NULL,									// Option Flag
																NULL,									// bank size
																NULL,									// event status flag
																NULL,									// remained criminal status time
																NULL,									// remained murderer point time
																NULL,									// nick name
																NULL,									// gameguard
																NULL									// last killed time in battlesquare
																);

	if(pvPacketFactor)
		m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	if(pvPacket)
	{
		m_pagpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		if(!m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5))
		{
			//TRACEFILE("AgsmDeath::CBUpdateCharacter() 에서 SendPacketNear() 실패");
		}

		m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}

BOOL AgsmDeath::CBSendCharacterAllServerInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	return TRUE;

	AgsmDeath		*pThis			= (AgsmDeath *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			*pulBuffer		= (UINT32 *)		pCustData;

	BOOL	bSendResult = pThis->SendPacketDeath(pcsCharacter, pulBuffer[0], (BOOL) pulBuffer[1]);
	bSendResult &= pThis->SendPacketDeathADItem(pcsCharacter, pulBuffer[0], (BOOL) pulBuffer[1]);

	return bSendResult;
}

BOOL AgsmDeath::CBDisconnectCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmDeath		*pThis			= (AgsmDeath *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgsdDeath		*pcsAgsdDeath	= pThis->GetADCharacter(pcsCharacter);
	if (!pcsAgsdDeath)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		if (pcsAgsdDeath->m_bIsNeedProcessDeath)
		{
			pThis->Dead(pcsCharacter, FALSE);
		}

		/*
		pThis->m_pagsmFactors->Resurrection(&pcsCharacter->m_csFactor, NULL);

		AuPOS	stResPos;
		ZeroMemory(&stResPos, sizeof(AuPOS));

		if (pThis->m_pagpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &stResPos))
		{
			pThis->m_pagpmCharacter->UpdatePosition(pcsCharacter, &stResPos, FALSE, TRUE);
		}
		*/
	}

	return TRUE;
}

VOID AgsmDeath::SetItemDropRate(AgpdItem *pcsItem, INT32 lDropRate)
{
	SetItemDropRate(GetADItem(pcsItem), lDropRate);
}

VOID AgsmDeath::SetItemDropRate(AgsdDeathADItem *pstDeathItem, INT32 lDropRate)
{
	pstDeathItem->m_lDropRate = lDropRate;
}

BOOL AgsmDeath::DropItem(AgpdCharacter *pcsCharacter, ApBase *pcsAttacker)
{
	PROFILE("AgsmDeath::DropItem");

	if (!pcsCharacter)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("DropItem"));

	stAgsmDeathDropItemInfo			stDropItemInfo;

	stDropItemInfo.pvClass			= this;
	stDropItemInfo.pcsAttackerBase	= pcsAttacker;

	AgpdItem	*pcsDropItem	= NULL;
	INT32		lDropRate		= 0;
	BOOL		bRetval			= TRUE;
	if (m_pagpmCharacter->IsMurderer(pcsCharacter))
	{
		// 살인자라면 가방에 있는 아템 하나를 골라서 그 아템을 떨굴 확률을 발생시킨다.
		//
		if (m_csRandomNumber.randInt(100) < m_pagpmCharacter->GetMurdererLevel(pcsCharacter) * 10)
		{
			// 아템 하나를 골라서 떨궈야 한다.
			AgpdItemADChar	*pcsItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
			INT32	lItemCount = m_pagpmGrid->GetItemCount(&pcsItemADChar->m_csInventoryGrid);
			if (lItemCount > 0)
			{
				// 인벤토리에 있는 넘중에 하나를 골라낸다.
				INT32			lRandomNumber = m_csRandomNumber.randInt(lItemCount - 1);
				INT32			i;
				AgpdGridItem *	pcsGridItem;

				i = 0;
				for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i);
					 pcsGridItem;
					 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i))
				{
					 if (--lRandomNumber == 0)
						 break;
				}

				if (pcsGridItem)
					pcsDropItem = m_pagpmItem->GetItem(pcsGridItem);
				{
					if (pcsDropItem)
					{
						// 고른넘을 떨굴확률 100%로 세팅한다.
						AgsdDeathADItem	*pcsDeathADItem	= GetADItem(pcsDropItem);

						// 떨굴 확률을 발생시킨다.
						lDropRate = pcsDeathADItem->m_lDropRate;

						////////////////////////////////////////////////////////////
						SetItemDropRate(pcsDeathADItem, 100000);
						////////////////////////////////////////////////////////////

						bRetval = m_pagpmItem->CharacterForAllItems(pcsCharacter, CBDropItem, &stDropItemInfo);
					}
				}
			}
		}
	}
	else if (!m_pagsmCharacter->GetCharDPNID(pcsCharacter))
	{
		bRetval = m_pagpmItem->CharacterForAllItems(pcsCharacter, CBDropItem, &stDropItemInfo);

		// 필요하다면 여기서 돈을 떨궈야 한다.
		// 먼저 돈 아템을 만들고 바닦에 떨군다.
		//
		//	1. AddItemMoney(INT32 lIID, INT32 lMoney, AuPOS pos) 이걸로 아템을 만든다.
		//	2. Drop Rate를 100%로 세팅한다.
		//	3. CBDropItem(pcsItem, &stDropItemInfo, pcsCharacter) 을 호출한다.
		//
	}

	if (pcsDropItem)
	{
		// 확률을 원래되로 되돌린다.
		SetItemDropRate(pcsDropItem, lDropRate);
	}

	return bRetval;
}

BOOL AgsmDeath::CBDropItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmDeath::CBDropItem");

	if (!pData || !pClass)
		return FALSE;

	stAgsmDeathDropItemInfo		*pstDropItemInfo	= (stAgsmDeathDropItemInfo *)	pClass;

	if (!pstDropItemInfo->pvClass)
		return FALSE;

	AgsmDeath *					pThis = (AgsmDeath *) pstDropItemInfo->pvClass;
	AgpdItem *					pcsItem = (AgpdItem *) pData;
	AgpdCharacter *				pcsCharacter = (AgpdCharacter *) pCustData;
	AgsdDeathADItem *			pstDeathItem = pThis->GetADItem(pcsItem);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBDropItem"));

	if (pThis->m_csRandomNumber.randInt(100000) <= pstDeathItem->m_lDropRate)
	{
		//pThis->m_pagpmItem->ReleaseItem(pcsItem);

		// drop item을 looting 하는 권한에 대한 정보를 세팅한다. (선 루팅권 보장에 관한 세팅이다.)
		if (pstDropItemInfo->pcsAttackerBase)
		{
			AgsdItem	*pcsAgsdItem	= pThis->m_pagsmItem->GetADItem(pcsItem);
			pcsAgsdItem->m_csFirstLooterBase.m_eType	= pstDropItemInfo->pcsAttackerBase->m_eType;
			pcsAgsdItem->m_csFirstLooterBase.m_lID		= pstDropItemInfo->pcsAttackerBase->m_lID;

			pcsAgsdItem->m_ulDropTime = pThis->GetClockCount();
		}
		//////////////////////////////////////////////////////////////////////////////////////////

		pcsItem->m_posItem = pcsCharacter->m_stPos;

		pThis->m_pagpmItem->AddItemToField(pcsItem);

		pstDeathItem->m_lDropRate = 0;
	}

	return TRUE;
}

BOOL AgsmDeath::DropSkull(AgpdCharacter *pcsCharacter, AgpmItemSkullInfo *pstSkullInfo)
{
	if (!pcsCharacter || !pstSkullInfo)
		return FALSE;

	if (strlen(pstSkullInfo->szKiller) < 1 && pstSkullInfo->lPartyMemberID[0] == AP_INVALID_CID)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("DropSkull"));

	// pcsCharacter의 union rank point를 저장하고 있는 유골을 떨군다.
	// 유골에 죽기전 rank point를 저장한다.

	AgpdItem	*pcsItemSkull = m_pagsmItemManager->CreateItemSkull();
	if (!pcsItemSkull)
		return FALSE;

	// 죽은 캐릭터, 죽인 캐릭터의 아뒤, union rank를 SkullItem에 저장한다.
	//////////////////////////////////////////////////////////////////////////////
	if (!pcsItemSkull->m_pstSkullInfo)
	{
		ZeroMemory(pcsItemSkull->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsItemSkull->m_szDeleteReason, "Skull information error", AGPMITEM_MAX_DELETE_REASON);

		m_pagpmItem->RemoveItem(pcsItemSkull->m_lID, TRUE);
		return FALSE;
	}

	pcsItemSkull->m_nCount	= 1;

	CopyMemory(pcsItemSkull->m_pstSkullInfo, pstSkullInfo, sizeof(AgpmItemSkullInfo));

	CopyMemory(pcsItemSkull->m_pstSkullInfo->szSkullOwner, pcsCharacter->m_szID, sizeof(CHAR) * AGPACHARACTER_MAX_ID_STRING);
	pcsItemSkull->m_pstSkullInfo->lSkullUnionRank = m_pagpmUnion->GetUnionRank(pcsCharacter);
	if (pcsItemSkull->m_pstSkullInfo->lSkullUnionRank <= 0 || pcsItemSkull->m_pstSkullInfo->lSkullUnionRank > AGPMUNION_MAX_UNION_RANK)
	{
		ZeroMemory(pcsItemSkull->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsItemSkull->m_szDeleteReason, "Skull information error 2", AGPMITEM_MAX_DELETE_REASON);

		m_pagpmItem->RemoveItem(pcsItemSkull->m_lID, TRUE);
		return FALSE;
	}

	pcsItemSkull->m_posItem = pcsCharacter->m_stPos;

	return m_pagpmItem->AddItemToField(pcsItemSkull);
}

BOOL AgsmDeath::DropSkull(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT32		lRace			= m_pagpmFactors->GetRace(&pcsCharacter->m_csFactor);

	AgpdItem	*pcsSkullItem	= NULL;

	if (lRace == AURACE_TYPE_HUMAN)
	{
		pcsSkullItem	= m_pagsmItemManager->CreateItem(m_pagpmItem->GetHumanSkullTID());
	}
	else
	{
		pcsSkullItem	= m_pagsmItemManager->CreateItem(m_pagpmItem->GetOrcSkullTID());
	}

	if (!pcsSkullItem)
		return FALSE;

	pcsSkullItem->m_nCount	= 1;

	if (!m_papmEventManager->GetRandomPos(&pcsCharacter->m_stPos, &pcsSkullItem->m_posItem, 50, 100, TRUE))
		pcsSkullItem->m_posItem	= pcsCharacter->m_stPos;

	return m_pagpmItem->AddItemToField(pcsSkullItem);
}

BOOL AgsmDeath::SetSkullInfo(ApBase *pcsBase, AgpmItemSkullInfo *pstSkullInfo, AgpdCharacter *pcsTargetChar)
{
	if (!pcsBase || !pstSkullInfo || !pcsTargetChar)
		return FALSE;

	switch (pcsBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			INT32	lUnionRank = GetUnionRank(pcsBase);

			if (lUnionRank <= 0 || lUnionRank > AGPMUNION_MAX_UNION_RANK)
				return FALSE;

			CopyMemory(pstSkullInfo->szKiller, ((AgpdCharacter *) pcsBase)->m_szID, sizeof(CHAR) * AGPACHARACTER_MAX_ID_STRING);
			pstSkullInfo->lKillerUnionRank = lUnionRank;
		}
		break;

	case APBASE_TYPE_PARTY:
		{
			INT32	lUnionRank = GetUnionRank(pcsBase);

			if (lUnionRank <= 0 || lUnionRank > AGPMUNION_MAX_UNION_RANK)
				return FALSE;

//			AgpdCharacter	*pcsCombatMember[AGPMPARTY_MAX_PARTY_MEMBER];
//			ZeroMemory(pcsCombatMember, sizeof(AgpdCharacter *) * AGPMPARTY_MAX_PARTY_MEMBER);

			ApSafeArray<AgpdCharacter *, AGPMPARTY_MAX_PARTY_MEMBER>	pcsCombatMember;
			pcsCombatMember.MemSetAll();

			INT32	lMemberTotalLevel	= 0;

//			INT32	lNumCombatMember = m_pagsmParty->GetNearMember((AgpdParty *) pcsBase, pcsCombatMember, &lMemberTotalLevel);
			INT32	lNumCombatMember = m_pagsmParty->GetNearMember((AgpdParty *) pcsBase, pcsTargetChar, &pcsCombatMember[0], &lMemberTotalLevel);
			if (lNumCombatMember < 1)
			{
				return FALSE;
			}

			for (int i = 0; i < lNumCombatMember; ++i)
			{
				pstSkullInfo->lPartyMemberID[i] = pcsCombatMember[i]->m_lID;
			}

			pstSkullInfo->lKillerUnionRank = lUnionRank;
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

/*
BOOL AgsmDeath::KilledGuardian(AgpdCharacter *pCharacter, ApBase *pcsAttacker)
{
	if (!pCharacter || !pcsAttacker)
		return FALSE;

	if (!m_pagpmShrine->IsGuardian(pCharacter))
		return FALSE;

	// 죽은넘이 가디언이다. history를 보고 어느넘이 이넘을 죽였는지 알아본다.

	// 기본적인 방식은...
	//		1. 50% 이상의 데미지를 준넘 (개인이건 파티원이건 상관없음)
	//		2. 만약 50% 이상의 데미지가 파티에 의한거라면 해당 파티원에게 모두 kill 수를 올려준다.

	UINT32	ulCurrentClock = GetClockCount();

	// character, party에 따라 가디언을 죽였다는 정보를 추가한다.
	switch (pcsAttacker->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			m_pagpmShrine->AddKillGuardian((AgpdCharacter *) pcsAttacker, pCharacter, ulCurrentClock);
		}
		break;

	case APBASE_TYPE_PARTY:
		{
			AgpdParty		*pcsParty	= (AgpdParty *) pcsAttacker;
			if (!pcsParty)
				return FALSE;

			for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
			{
				// 죽은넘과의 거리를 측정해서 시야를 벗어나면 추가해주지 않는다.
				if (m_pagsmParty->CheckValidDistance(pcsParty->m_pcsMemberList[i], pCharacter) > 1)
					continue;

				if (!pcsParty->m_pcsMemberList[i])
				{
					pcsParty->m_pcsMemberList[i] = m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
					if (!pcsParty->m_pcsMemberList[i])
						continue;
				}

				m_pagpmShrine->AddKillGuardian(pcsParty->m_pcsMemberList[i], pCharacter, ulCurrentClock);
			}
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}
*/

INT32 AgsmDeath::GetRacialPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	// 지금 데이타가 없다. 그냥 1만 리턴한다.
	//
	//
	//
	//
	//

	return 1;
}

INT32 AgsmDeath::GetMonsterExp(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsMostDamager)
{
	if (!pcsCharacter)
		return FALSE;

	INT32	lBaseEXP	= 0;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lBaseEXP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_BASE_EXP);

	BOOL  bTPackUser	= FALSE;		
	INT32 ulPCRoomType	= 0;

	m_pagpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pcsMostDamager, &ulPCRoomType);
	if(ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK)
		bTPackUser = TRUE;

	return (INT32)(m_pagpmConfig->GetExpAdjustmentRatio(bTPackUser) * (float)lBaseEXP);

	/*
	AgpdFactorCharStatus	*pcsCharStatus = 
		(AgpdFactorCharStatus *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);

	if (!pcsCharStatus)
		return 0;

	return (INT32) ((100 + (pcsCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL] * 10)) * GetRacialPoint(pcsCharacter));
	*/
}

INT32 AgsmDeath::GetAttackerLevel(ApBase *pcsAttackerBase)
{
	if (!pcsAttackerBase)
		return 0;

	switch (pcsAttackerBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			AgpdCharacter	*pcsAttacker = m_pagpmCharacter->GetCharacter(pcsAttackerBase->m_lID);
			if (pcsAttacker)
			{
				return m_pagpmCharacter->GetLevel(pcsAttacker);
			}
		}
		break;

	case APBASE_TYPE_PARTY:
		{
			AgpdParty	*pcsParty	= m_pagpmParty->GetParty(pcsAttackerBase->m_lID);
			if (!pcsParty)
				return 0;

			return m_pagpmParty->GetPartyTotalMemberLevel(pcsParty) / pcsParty->m_nCurrentMember;
		}
		break;
	}

	return 0;
}

INT32 AgsmDeath::GetBonusExp(INT32 lTotalBonusExp, AgpdCharacter *pcsKilledChar, AgsdCharacterHistoryEntry *pstHistoryEntry)
{
	PROFILE("AgsmDeath::GetBonusExp");

	if (!pcsKilledChar || !pstHistoryEntry)
		return 0;

	AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(pstHistoryEntry->m_csSource.m_lID);
	if (!pcsCharacter)
		return 0;

	AgpdPartyADChar	*pcsPartyAttachData	= m_pagpmParty->GetADCharacter(pcsCharacter);
	if (!pcsPartyAttachData || pcsPartyAttachData->lPID != AP_INVALID_PARTYID)
		return 0;

	// damage를 가져온다.

	INT32	lDamage	= 0;

	if (pstHistoryEntry->m_pcsUpdateFactor)
		m_pagpmFactors->GetValue(pstHistoryEntry->m_pcsUpdateFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	lDamage	= -lDamage;
	if (lDamage < 1)
	{
		pcsCharacter->m_Mutex.Release();
		return 0;
	}

	// 죽은넘 HP가 몇이었는지 알아온다.
	INT32	lTargetMaxHP	= GetMaxHP(pcsKilledChar);

	if (lDamage > lTargetMaxHP)
		lDamage = lTargetMaxHP;

	// (Mob Base EXP * 0.9) + Ran ((Mob Base EXP * 1.1) - (Mob Base EXP * 0.9))

	// 2007.01.30. steeple
	// FirstHit 공식에 데미지 / 맥스HP 비율 넣었다.
	FLOAT	fFirstAttackBonus	= 0.0f;
	FLOAT	fRandBonus = (FLOAT) m_csRandomNumber.rand((double)(lTotalBonusExp * 0.02f));
	if (pstHistoryEntry->m_bFirstHit)
		fFirstAttackBonus = ((FLOAT)lDamage / (FLOAT)lTargetMaxHP) * (((FLOAT)lTotalBonusExp * 0.99f + fRandBonus) * 0.1f);

	FLOAT	fBonusExp	= 0.0f;

	FLOAT	fDMGExp			= (FLOAT)lDamage / (FLOAT) lTargetMaxHP * ((((FLOAT)lTotalBonusExp * 0.99f + fRandBonus) * 0.9f));
	if (fDMGExp < 1.0f)
		fDMGExp	= 1.0f;

	// 죽인넘과 죽은넘 레벨을 가져온다.
	INT32	lAttackLevel	= GetAttackerLevel((ApBase *) &pstHistoryEntry->m_csSource);
	INT32	lTargetLevel	= m_pagpmCharacter->GetLevel(pcsKilledChar);

	if (lAttackLevel < 1 || lTargetLevel < 1)
	{
		pcsCharacter->m_Mutex.Release();
		return 0;
	}

	if (lTargetLevel >= lAttackLevel)
		fBonusExp += ((fDMGExp + fFirstAttackBonus) * (1.0f + pow((FLOAT)(lTargetLevel - lAttackLevel) * 2.0f, 2.0f) / 500.0f));
	else
		fBonusExp += ((fDMGExp + fFirstAttackBonus) * (1.0f - pow((FLOAT)(lAttackLevel - lTargetLevel) * 2.0f, 2.0f) / 500.0f));

	// 공격자보다 렙이 낮은 몹을 잡았을때 보정하기
	if ( fBonusExp < 1.0f) // 0.0f 에서 1.0f로 상향조정 - arycoat 2008.9.25
		fBonusExp = 1.0f;

	// 공격자보다 렙이 높은 몹을 잡았을때 보정하기
	if ( (lTargetLevel - lAttackLevel) >= AGSMDEATH_EXP_LEVEL_DIFF)
		fBonusExp = 1.0f;

	AddBonusExpToChar(pcsCharacter, pcsKilledChar, (INT32) fBonusExp, TRUE, TRUE);

	pcsCharacter->m_Mutex.Release();

	return (INT32) fBonusExp;
}

INT32 AgsmDeath::GetBonusExpToParty(INT32 lTotalBonusExp, AgpdCharacter *pcsKilledChar, AgsdCharacterHistoryEntry *pstHistoryEntry)
{
	PROFILE("AgsmDeath::GetBonusExpToParty");

	if (!pcsKilledChar || !pstHistoryEntry)
		return 0;

	//STOPWATCH2(GetModuleName(), _T("GetBonusExpToParty"));

	// 죽인넘과 죽은넘 레벨을 가져온다.
	AgpdParty	*pcsParty	= m_pagpmParty->GetPartyLock(pstHistoryEntry->m_csSource.m_lID);
	if (!pcsParty)
		return 0;

	// damage를 가져온다.
	// 2007.03.09. steeple
	// Damage 가져오는 거 변경했다.
	INT32 lDamage = GetTotalDamageCurrentMember(pcsParty, pcsKilledChar);
	
	//if (pstHistoryEntry->m_pcsUpdateFactor)
	//	m_pagpmFactors->GetValue(pstHistoryEntry->m_pcsUpdateFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);


	//lDamage	= -lDamage;
	if (lDamage < 1)
	{
		pcsParty->m_Mutex.Release();
		return 0;
	}

	// 죽은넘 HP가 몇이었는지 알아온다.
	INT32	lTargetMaxHP	= GetMaxHP(pcsKilledChar);

	if (lDamage > lTargetMaxHP)
		lDamage = lTargetMaxHP;

	// (Mob Base EXP * 0.9) + Ran ((Mob Base EXP * 1.1) - (Mob Base EXP * 0.9))
	FLOAT	fBonusExp		= 0.0f;
	FLOAT	fRandBonus		= (FLOAT) m_csRandomNumber.rand((double)((FLOAT)lTotalBonusExp * 0.02f));
	if (pstHistoryEntry->m_bFirstHit)
		fBonusExp = ((FLOAT)lDamage / (FLOAT)lTargetMaxHP) * (((FLOAT)lTotalBonusExp * 0.99f + fRandBonus) * 0.1f);

//	AgpdCharacter	*pcsCombatMember[AGPMPARTY_MAX_PARTY_MEMBER];
//	ZeroMemory(pcsCombatMember, sizeof(AgpdCharacter *) * AGPMPARTY_MAX_PARTY_MEMBER);

	ApSafeArray<AgpdCharacter *, AGPMPARTY_MAX_PARTY_MEMBER>	pcsCombatMember;
	pcsCombatMember.MemSetAll();

	INT32	lMemberTotalLevel	= 0;

	//INT32	lNumCombatMember = m_pagsmParty->GetNearMember(pcsParty, pcsCombatMember, &lMemberTotalLevel);
	INT32	lNumCombatMember = m_pagsmParty->GetNearMember(pcsParty, pcsKilledChar, &pcsCombatMember[0], &lMemberTotalLevel);

	// 2007.03.09. steeple
	// 파티의 레벨 합은 거리에 상관없이 전체 레벨합을 구한다.
	lMemberTotalLevel = m_pagpmParty->GetPartyTotalMemberLevel(pcsParty);
	if (lNumCombatMember < 1 || lMemberTotalLevel < 1)
	{
		pcsParty->m_Mutex.Release();
		return FALSE;
	}

	AgpmPartyCalcExpType	eCalcExpType	= pcsParty->m_eCalcExpType;

	pcsParty->m_Mutex.Release();

	INT32	lHighestMemberLevel	= 0;
	INT32	lLowerMemberLevel = 0x00FFFFFF;
	for (int i = 0; i < lNumCombatMember; ++i)
	{
		 INT32	lCombatMemberLevel	= m_pagpmCharacter->GetLevel(pcsCombatMember[i]);

		 if (lCombatMemberLevel > lHighestMemberLevel)
			 lHighestMemberLevel	= lCombatMemberLevel;
		 if (lCombatMemberLevel < lLowerMemberLevel)
			 lLowerMemberLevel = lCombatMemberLevel;
	}

	INT32	lAttackLevel	= lMemberTotalLevel / lNumCombatMember;
	INT32	lTargetLevel	= m_pagpmCharacter->GetLevel(pcsKilledChar);

	if (lAttackLevel < 1 || lTargetLevel < 1)
	{
		pcsParty->m_Mutex.Release();
		return 0;
	}

	FLOAT	fDMGExp			= (FLOAT)lDamage / (FLOAT) lTargetMaxHP * ((((FLOAT)lTotalBonusExp * 0.99f + fRandBonus) * 0.9f));

	if (fDMGExp < 1.0f)
		fDMGExp	= 1.0f;

//	FLOAT	fLeaderCharisma	= 0.0f;
//	m_pagpmFactors->GetValue(&pcsParty->m_pcsMemberList[0]->m_csFactor, &fLeaderCharisma, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);

	//BOOL	bAvailableLevelDiff = ( abs(lHighestMemberLevel - lLowerMemberLevel) <= AGPMPARTY_AVAILABLE_LEVEL_DIFF ) ? TRUE : FALSE;
	FLOAT	fFinalPartyExp	= 0.0f;

	if (lTargetLevel >= lHighestMemberLevel)
	{
		fFinalPartyExp = (fDMGExp + fBonusExp) * (1.0f + pow((FLOAT)(lTargetLevel - lHighestMemberLevel) * 2.0f, 2.0f) / 500.0f);
		//if (bAvailableLevelDiff == TRUE)
			fFinalPartyExp = (FLOAT) ( fFinalPartyExp * (1.0f + (m_partyBonus * (FLOAT)(lNumCombatMember - 1))) );
	}
	else
	{
		fFinalPartyExp = (fDMGExp + fBonusExp) * (1.0f - pow((FLOAT)(lTargetLevel - lHighestMemberLevel) * 2.0f, 2.0f) / 500.0f);
		//if (bAvailableLevelDiff == TRUE)
			fFinalPartyExp = (FLOAT) (fFinalPartyExp * (1.0f + (m_partyBonus * (FLOAT)(lNumCombatMember - 1))) );
	}


	for (int i = 0; i < lNumCombatMember; ++i)
	{
		if (!pcsCombatMember[i] ||
			pcsCombatMember[i]->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			continue;

		if (!pcsCombatMember[i]->m_Mutex.WLock())
			continue;

		INT32	lMemberLevel		= m_pagpmCharacter->GetLevel(pcsCombatMember[i]);

		FLOAT	fBonusExpBaseDamage	= 0.0f;
		FLOAT	fBonusExpBaseLevel	= 0.0f;
		

		if (eCalcExpType == AGPMPARTY_EXP_TYPE_BY_DAMAGE )		// 2005.05.17. By SungHoon
//			||	eCalcExpType == AGPMPARTY_EXP_TYPE_BY_COMPLEX)
		{
			// 사냥 기여도 (현재는 데미지)에 따른 분배
			INT32	lMemberDamage		= 0;

			AgsdCharacterHistory	*pcsHistory = m_pagsmCharacter->GetHistory(pcsKilledChar);
			if (pcsHistory)
			{
				for (int j = 0; j < pcsHistory->m_lEntryNum; j++)
				{
					if (pcsHistory->m_astEntry[j].m_csSource.m_lID == pcsCombatMember[i]->m_lID &&
						pcsHistory->m_astEntry[j].m_lPartyID != AP_INVALID_PARTYID)
					{
						if (pcsHistory->m_astEntry[j].m_pcsUpdateFactor)
							m_pagpmFactors->GetValue(pcsHistory->m_astEntry[j].m_pcsUpdateFactor, &lMemberDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

						lMemberDamage	= -lMemberDamage;
						if (lMemberDamage < 1)
							lMemberDamage	= 0;

						break;
					}
				}

				if (lMemberDamage > 0)
				{
					if (lMemberDamage > lTargetMaxHP)
						lMemberDamage = lTargetMaxHP;

					if (lDamage < lMemberDamage)
						lMemberDamage	= lDamage;

					fBonusExpBaseDamage	= fFinalPartyExp * ((float) lMemberDamage / (float) lDamage);
				}
			}
	
		}

		if (eCalcExpType == AGPMPARTY_EXP_TYPE_BY_LEVEL )	// 2005.05.17. By SungHoon
//			|| eCalcExpType == AGPMPARTY_EXP_TYPE_BY_COMPLEX)
		{
			// 레벨에 따른 균등 분배
			fBonusExpBaseLevel	= fFinalPartyExp * ((float) lMemberLevel / (float) lMemberTotalLevel);
		}
		FLOAT fMemberBonusExp = 0.0f;

		if (eCalcExpType == AGPMPARTY_EXP_TYPE_BY_DAMAGE)
			fMemberBonusExp	= fBonusExpBaseDamage;
		else if (eCalcExpType == AGPMPARTY_EXP_TYPE_BY_LEVEL)
			fMemberBonusExp	= fBonusExpBaseLevel;

		if (fMemberBonusExp <= 0.0f)
		{
			fMemberBonusExp	= 1.0f;
		}

		// 마을에 있는 놈도 뺀다.
		BOOL	bIsSafeArea	= FALSE;
		ApmMap::RegionTemplate	*pcsRegionTemplate	= m_papmMap->GetTemplate(m_papmMap->GetRegion(pcsCombatMember[i]->m_stPos.x , pcsCombatMember[i]->m_stPos.z));
		if (pcsRegionTemplate &&
			pcsRegionTemplate->ti.stType.uSafetyType == ApmMap::ST_SAFE)
			bIsSafeArea	= TRUE;

		// 장퀘에서는 이거 체크안함. 2007.02.02. steeple
		if (g_eServiceArea != AP_SERVICE_AREA_CHINA)
		{
			// 2007.04.09. steeple
			// 레벨차이가 많이 나면 경치 1 준다.
			if ((lTargetLevel - lMemberLevel) >= AGSMDEATH_EXP_LEVEL_DIFF)
				fMemberBonusExp = 1.0f;

			if ((lHighestMemberLevel - lMemberLevel) >= AGPMPARTY_AVAILABLE_LEVEL_DIFF)
				fMemberBonusExp = 0.0f;
		}

		if (fMemberBonusExp > 0.0f && !bIsSafeArea)
			AddBonusExpToChar(pcsCombatMember[i], NULL, (INT32) fMemberBonusExp, TRUE, TRUE);

		//AddBonusExpToParty(pcsParty, lBonusExp, lMemberTotalLevel, lNumCombatMember, pcsCombatMember);


		pcsCombatMember[i]->m_Mutex.Release();
	}

	return (INT32) fFinalPartyExp;
}

INT32 AgsmDeath::GetMaxHP(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	INT32	lMaxHP	= 0;

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

	return lMaxHP;
}

// 2007.03.09. steeple
// 현재 파티원들의 데미지를 구해온다. 탈퇴한 놈의 것은 안구한다.
// pcsParty 는 Lock 되어서 들어온다.
INT32 AgsmDeath::GetTotalDamageCurrentMember(AgpdParty* pcsParty, AgpdCharacter* pcsDeadCharacter)
{
	if(!pcsParty || !pcsDeadCharacter)
		return 0;

	AgsdCharacterHistory* pcsHistory = m_pagsmCharacter->GetHistory(pcsDeadCharacter);
	if(!pcsHistory)
		return 0;

	INT32 lTotalDamage, lEachDamage;
	lTotalDamage = lEachDamage = 0;

	for(INT32 i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		for(INT32 j = 0; j < pcsHistory->m_lEntryNum; ++j)
		{
			if(pcsHistory->m_astEntry[j].m_csSource.m_lID == pcsParty->m_lMemberListID[i] &&
				pcsHistory->m_astEntry[j].m_lPartyID == pcsParty->m_lID)
			{
				if(pcsHistory->m_astEntry[j].m_pcsUpdateFactor)
					m_pagpmFactors->GetValue(pcsHistory->m_astEntry[j].m_pcsUpdateFactor, &lEachDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

				lEachDamage = -lEachDamage;
				if(lEachDamage < 1)
					lEachDamage	= 0;

				lTotalDamage += lEachDamage;
			}
		}
	}

	return lTotalDamage;
}

INT64 AgsmDeath::GetPenaltyExp(AgpdCharacter *pcsCharacter, BOOL bIsResurrectionTown)
{
	// 중국버전은 경험치 패널티 빼달란다.
	if (!m_pagpmConfig->DoesExpPenaltyOnDeath())
		return 0;

	if (!pcsCharacter)
		return 0;

	// murder인지 아닌지... 등등을 고려해야한다.

	INT32	lLevel	= m_pagpmCharacter->GetLevel(pcsCharacter);

	if (lLevel < 6)
		return 0;

	INT64	llMaxExp	= m_pagpmFactors->GetMaxExp((AgpdFactor *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
	
	if( pcsCharacter->GetSpecialState( AGPDCHAR_SPECIAL_STATUS_LEVELLIMIT ) )
	{
		AgpdCharacterTemplate	*pcsTemplate	= pcsCharacter->m_pcsCharacterTemplate;
		INT32 lBeforeLevel = m_pagpmCharacter->GetLevelBefore(pcsCharacter);
		llMaxExp = m_pagpmFactors->GetMaxExp(&pcsTemplate->m_csLevelFactor[lBeforeLevel]);
	}

	if (llMaxExp < 1)
		return (-1);

	//JK_레벨별 경험치 패널티
	//INT64	llPenaltyExp = (INT32) ((FLOAT)(llMaxExp * GetPenaltyExpRate(pcsCharacter, bIsResurrectionTown)) / 100.0f);
	INT64 llPenaltyExp = m_pagpmCharacter->GetDownExpPerLevel(lLevel);

	if (m_pagpmCharacter->HasPenalty(pcsCharacter, AGPMCHAR_PENALTY_EXP_LOSE))
	{
		llPenaltyExp = (INT64) ( llPenaltyExp * 1.1f );
	}

	return llPenaltyExp;
}

// 2005.07.28. steeple
// Penalty Exp Rate 추가. 기본적으로 10% 까는 걸로 한다.
INT32 AgsmDeath::GetPenaltyExpRate(AgpdCharacter* pcsCharacter, BOOL bIsResurrectionTown)
{
	if(!pcsCharacter)
		return 0;

	AgsdDeath* pcsAgsdDeath = GetADCharacter(pcsCharacter);
	if(!pcsAgsdDeath)
		return 0;

	INT32 lRate = 5;	// 기본적으로 10% 까는 걸로 한다.
	switch((eAgpmPvPTargetType)pcsAgsdDeath->m_cDeadType)
	{
		case AGPMPVP_TARGET_TYPE_MOB:
		{
			if(bIsResurrectionTown)		lRate = 3;
			else						lRate = 5;

			break;
		}

		case AGPMPVP_TARGET_TYPE_NORMAL_PC:
		{
			if(bIsResurrectionTown)		lRate = 0;
			else
			{
				//중국 종족전시에 경험치 까는거 제거 ;; supertj@081013
				//if (g_eServiceArea == AP_SERVICE_AREA_CHINA && m_pagpmPvP->IsInRaceBattle())
				//	lRate = 0;
				//else
					lRate = 2;
			}

			break;
		}

		case AGPMPVP_TARGET_TYPE_ENEMY_GUILD:
		{
			lRate = 0;
			break;
		}
	}

	return lRate;
}


INT32 AgsmDeath::GetUnionRank(ApBase *pcsBase)
{
	if (!pcsBase)
		return 0;

	switch (pcsBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			return m_pagpmUnion->GetUnionRank((AgpdCharacter *) pcsBase);
		}
		break;

	case APBASE_TYPE_PARTY:
		{
			return m_pagpmParty->GetPartyHighestUnionRank((AgpdParty *) pcsBase);
		}
		break;

	default:
		return 0;
		break;
	}

	return 0;
}

PVOID AgsmDeath::MakePacketDeath(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, BOOL bLogin)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	AgsdDeath		*pcsDeath	= GetADCharacter(pcsCharacter);
	if (!pcsDeath)
		return NULL;

	PVOID	pvPacketLastHitBase = NULL;
	
	if (!bLogin)
		pvPacketLastHitBase = m_pagsmCharacter->MakePacketBase((ApBase *) &pcsDeath->m_csLastHitBase);

	UINT32	ulClockCount	= GetClockCount();

	PVOID	pvPacketDeath = m_csPacketDeath.MakePacket(FALSE, pnPacketLength, 0,
														&pcsCharacter->m_lID,
														&pcsDeath->m_ulDeadTime,
														&pcsDeath->m_llLastLoseExp,
														pvPacketLastHitBase,
														&ulClockCount);

	if (pvPacketLastHitBase)
		m_csPacket.FreePacket(pvPacketLastHitBase);

	if (!pvPacketDeath)
		return NULL;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMDEATH_PACKET_TYPE,
												pvPacketDeath,
												NULL);

	m_csPacket.FreePacket(pvPacketDeath);

	return pvPacket;
}

PVOID AgsmDeath::MakePacketDeathADItem(AgpdItem *pcsItem, INT16 *pnPacketLength, BOOL bLogin)
{
	if (!pcsItem || !pnPacketLength)
		return NULL;

	AgsdDeathADItem		*pcsDeathADItem = GetADItem(pcsItem);
	if (!pcsDeathADItem)
		return NULL;

	PVOID	pvPacketDeathADItem = m_csPacketDeathADItem.MakePacket(FALSE, pnPacketLength, 0,
																	&pcsItem->m_lID,
																	&pcsDeathADItem->m_lDropRate);

	if (!pvPacketDeathADItem)
		return NULL;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMDEATH_PACKET_TYPE,
												NULL,
												pvPacketDeathADItem);

	m_csPacket.FreePacket(pvPacketDeathADItem);

	return pvPacket;
}

BOOL AgsmDeath::SendPacketDeath(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bLogin)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = MakePacketDeath(pcsCharacter, &nPacketLength, bLogin);

	if (!pvPacket || nPacketLength <= 0)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmDeath::SendPacketDeathADItem(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bLogin)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	INT16	nPacketLength = 0;
	AgpdGridItem	*pcsGridItem;
	AgpdItemADChar	*pcsItemADChar = m_pagpmItem->GetADCharacter( pcsCharacter );

	int i = 0;
	for (i = 0; i < AGPMITEM_PART_NUM; i++)
	{
		pcsGridItem = m_pagpmItem->GetEquipItem( pcsCharacter, i );

		if ( pcsGridItem == NULL )
			continue;

		AgpdItem *pcsItem = m_pagpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		PVOID	pvPacket = MakePacketDeathADItem(pcsItem, &nPacketLength, bLogin);
		if (!pvPacket || nPacketLength <= 0)
			continue;

		SendPacket(pvPacket, nPacketLength, ulNID);

		m_csPacket.FreePacket(pvPacket);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		PVOID	pvPacket = MakePacketDeathADItem(pcsItem, &nPacketLength, bLogin);
		if (!pvPacket || nPacketLength <= 0)
			continue;

		SendPacket(pvPacket, nPacketLength, ulNID);

		m_csPacket.FreePacket(pvPacket);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		PVOID	pvPacket = MakePacketDeathADItem(pcsItem, &nPacketLength, bLogin);
		if (!pvPacket || nPacketLength <= 0)
			continue;

		SendPacket(pvPacket, nPacketLength, ulNID);

		m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}

BOOL AgsmDeath::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket ||
		nSize <= 0 ||
		ulNID == 0)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	PVOID		pvPacketDeath			= NULL;
	PVOID		pvPacketDeathADItem		= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&pvPacketDeath,
						&pvPacketDeathADItem);

	if (pvPacketDeath)
	{
		INT32		lCID					= AP_INVALID_CID;
		UINT32		ulDeadTime				= 0;
		INT64		llLastLoseExp			= 0;
		PVOID		pvPacketLastHitBase		= NULL;
		UINT32		ulReceivedClockCount	= 0;

		m_csPacketDeath.GetField(FALSE, pvPacketDeath, 0,
									&lCID,
									&ulDeadTime,
									&llLastLoseExp,
									&pvPacketLastHitBase,
									&ulReceivedClockCount);

		UINT32		ulCurrentClockCount		= GetClockCount();

		AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
		if (pcsCharacter)
		{
			AgsdDeath	*pcsDeath	= GetADCharacter(pcsCharacter);
			if (!pcsDeath)
			{
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

			pcsDeath->m_ulDeadTime = ulCurrentClockCount + (ulDeadTime - ulReceivedClockCount);
			pcsDeath->m_llLastLoseExp = llLastLoseExp;

			if (pvPacketLastHitBase)
			{
				INT8	cBaseType = 0;
				m_pagsmCharacter->m_csPacketBase.GetField(FALSE, pvPacketLastHitBase, 0,
															&cBaseType,
															&pcsDeath->m_csLastHitBase.m_lID);

				pcsDeath->m_csLastHitBase.m_eType = (ApBaseType) cBaseType;
			}

			if (!pcsCharacter->m_Mutex.Release())
				return FALSE;
		}
	}

	if (pvPacketDeathADItem)
	{
		INT32		lIID					= AP_INVALID_IID;
		INT32		lDropRate				= 0;

		m_csPacketDeathADItem.GetField(FALSE, pvPacketDeathADItem, 0,
									&lIID,
									&lDropRate);

		AgpdItem	*pcsItem	= m_pagpmItem->GetItemLock(lIID);
		if (pcsItem)
		{
			AgpdCharacter	*pcsCharacter = NULL;

			if (pcsItem->m_ulCID != AP_INVALID_CID)
			{
				pcsCharacter = m_pagpmCharacter->GetCharacterLock(pcsItem->m_ulCID);
				if (!pcsCharacter)
				{
					pcsItem->m_Mutex.Release();
					return FALSE;
				}
			}

			AgsdDeathADItem	*pcsDeathADItem = GetADItem(pcsItem);
			if (!pcsDeathADItem)
			{
				if (pcsCharacter)
					pcsCharacter->m_Mutex.Release();
				pcsItem->m_Mutex.Release();
				return FALSE;
			}

			pcsDeathADItem->m_lDropRate = lDropRate;

			if (pcsCharacter)
				pcsCharacter->m_Mutex.Release();

			if (!pcsItem->m_Mutex.Release())
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgsmDeath::ProcessDropItem(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	m_pagsmDropItem2->DropItem(pcsCharacter);

	return TRUE;
}

BOOL AgsmDeath::CBRequestResurrection(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmDeath::CBRequestResurrection");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmDeath		*pThis			= (AgsmDeath *)			pClass;
	AgpdCharacter	*pCharacter		= (AgpdCharacter *)		pData;
	INT32			lResurrcet		= *((BOOL *)			pCustData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRequestResurrection"));

	if (pCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
		return TRUE;

	if (pThis->m_pagpmCharacter->IsAllBlockStatus(pCharacter))
		return TRUE;

	BOOL	bSiegeWar	= FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pCharacter);

	if (pcsSiegeWar &&
		pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar) &&
		(pThis->m_pcsAgpmSiegeWar->IsAttackGuild(pCharacter, pcsSiegeWar) || pThis->m_pcsAgpmSiegeWar->IsDefenseGuild(pCharacter, pcsSiegeWar)))
		bSiegeWar	= TRUE;

	if (bSiegeWar && lResurrcet == AGPMCHAR_RESURRECT_NOW)
		return FALSE;

	// apply penalty exp
	INT64	llPenaltyExp = (bSiegeWar) ? 0 : pThis->GetPenaltyExp(pCharacter, lResurrcet == AGPMCHAR_RESURRECT_TOWN ? TRUE : FALSE);

	AgsdDeath	*pcsAgsdDeath	= pThis->GetADCharacter(pCharacter);
	if (pcsAgsdDeath->m_cDeadType == (INT8)AGPMPVP_TARGET_TYPE_MOB)
	{
		if (llPenaltyExp < 0)
			return FALSE;
	}

	if (lResurrcet == AGPMCHAR_RESURRECT_NOW)
	{
		ApmMap::RegionTemplate	*pcsRegionTemplate	= pThis->m_papmMap->GetTemplate(pCharacter->m_nBindingRegionIndex);
		if (pcsRegionTemplate)
		{
			if (pcsRegionTemplate->ti.stType.uSafetyType == ApmMap::ST_DANGER)
			{
				return FALSE;
			}
		}

		if (pThis->m_pagpmCharacter->GetExp(pCharacter) <= 0)
			return FALSE;
	}

	// 공성 부활 가능 여부 확인
	if ((lResurrcet == AGPMCHAR_RESURRECT_SIEGE_INNER ||
		 lResurrcet == AGPMCHAR_RESURRECT_SIEGE_OUTER) &&
		!pThis->m_pcsAgpmSiegeWar->IsResurrectableInCastle(pCharacter))
		return FALSE;

	// 현재 버프된 모든 스킬을 종료시킨다.
	//pThis->m_pagsmSkill->EndAllBuffedSkill((ApBase *) pCharacter, TRUE);

	// Criminal Flag가 세팅되어 있다면 없애준다.
	/*
	if (pCharacter->m_unCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED)
	{
		pThis->m_pagpmCharacter->UpdateCriminalStatus(pCharacter, AGPDCHAR_CRIMINAL_STATUS_INNOCENT);
	}
	*/

	BOOL bIsTransform = pCharacter->m_bIsTrasform;

	// 변신중이면 원래로 되돌린다.
	if (pCharacter->m_bIsTrasform)
		pThis->m_pagpmCharacter->RestoreTransformCharacter(pCharacter);

	// 지정되어 있는 위치에서 부활시킨다.
	// 위치정보, 캐릭터 Factor 등등의 데이타를 업데이트 한다.
	//
	PVOID pvPacketFactor	= NULL;

	// 기본 무적 걸어준다. 2006.01.09. steeple
	pThis->m_pagsmCharacter->SetDefaultInvincible(pCharacter);

	if (!pThis->m_pagsmFactors->Resurrection(&pCharacter->m_csFactor, &pvPacketFactor))
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pCharacter);
	pcsAgsdCharacter->m_bIsResurrectingNow = TRUE;

	// 다시 살아났다는 패킷을 보낸다.
	BOOL bSendResult = pThis->m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pCharacter, PACKET_PRIORITY_3);

	if (pvPacketFactor)
		pThis->m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	// update character status
	pThis->m_pagpmCharacter->UpdateActionStatus(pCharacter, AGPDCHAR_STATUS_NORMAL);

	if (pcsAgsdDeath)
		pcsAgsdDeath->m_bDropItem	= FALSE;

	AuPOS	stResPos;
	ZeroMemory(&stResPos, sizeof(AuPOS));

	if (lResurrcet == AGPMCHAR_RESURRECT_TOWN)
	{
		if (!pThis->m_pagpmEventBinding->GetBindingPositionForResurrection(pCharacter, &stResPos))
		{
			// 이게 실패한 경우는 정말 일어나면 안되는 경우이나.. 혹시 일어나게되면 아무데로나 텔레포트 시킬 수 없으니
			// 걍 이전 위치에서 부활시킨다.
			stResPos	= pCharacter->m_stPos;
		}

		INT64 llNowExp = pThis->m_pagpmCharacter->GetExp(pCharacter);
		if (llNowExp > 0 && llNowExp - llPenaltyExp < 0)
			llPenaltyExp = llNowExp;

		if (!pThis->m_pagpmConfig->IsEventResurrect() && llNowExp > 0 && llPenaltyExp > 0)
		{
			AgsdDeath	*pcsAgsdDeath = pThis->GetADCharacter((PVOID) pCharacter);
			pcsAgsdDeath->m_llLastLoseExp = llPenaltyExp;

			pThis->m_pagsmCharacter->SubExp(pCharacter, llPenaltyExp);
		}
	}
	else if (lResurrcet == AGPMCHAR_RESURRECT_SIEGE_INNER ||
			 lResurrcet == AGPMCHAR_RESURRECT_SIEGE_OUTER)
	{
		// 2007.01.25. laki
		BOOL bInner = (lResurrcet == AGPMCHAR_RESURRECT_SIEGE_INNER) ? TRUE : FALSE;
		
		if (!pThis->m_pcsAgpmSiegeWar->SetGuildTargetPosition(pCharacter, &stResPos, bInner))
		{
			// unknown exception, so restore orginal position
			stResPos = pCharacter->m_stPos;
		}
	}
	else
	{
		stResPos	= pCharacter->m_stPos;

		//if (!pcsAgsdDeath->m_bIsDeadByPvP)
		//{
			INT64 llNowExp = pThis->m_pagpmCharacter->GetExp(pCharacter);
			if (llNowExp > 0 && llNowExp - llPenaltyExp < 0)
				llPenaltyExp = llNowExp;

			if (!pThis->m_pagpmConfig->IsEventResurrect() && llNowExp > 0 && llPenaltyExp > 0)
			{
				AgsdDeath	*pcsAgsdDeath = pThis->GetADCharacter((PVOID) pCharacter);
				pcsAgsdDeath->m_llLastLoseExp = llPenaltyExp;

				pThis->m_pagsmCharacter->SubExp(pCharacter, llPenaltyExp);
			}
		//}
	}

	// 2007.07.25. steeple
	// DB 저장하기 전에 UpdatePosition 부터 한다.
	//
	// update position
	pThis->EnumCallback(AGSMDEATH_CB_RESURRECTION, pCharacter, &stResPos);

	// 2005.12.16. steeple
	// 현재 사용중으로 체크 되어 있는 스킬 스크롤 캐쉬 아이템을 사용시켜준다.
	pThis->m_pagsmItem->UseAllEnableCashItem(pCharacter,
											0,
											AGSDITEM_PAUSE_REASON_NONE,
											bIsTransform);	// 부활 전 상태가 변신 중이었다면, '일시 정지' 상태의 아이템도 사용해준다.
	pThis->m_pagsmSkill->RecastSaveSkill(pCharacter);
	// 현재 캐릭터 데이타를 모두 DB에 저장한다.
	pThis->m_pagsmCharacter->BackupCharacterData(pCharacter);
	pThis->m_pagsmCharacter->EnumCallback(AGSMCHARACTER_CB_UPDATE_ALL_TO_DB, pCharacter, NULL);

	pcsAgsdCharacter->m_bIsResurrectingNow = FALSE;

	return bSendResult;
}

// 2007.08.24. steeple
BOOL AgsmDeath::CBResurrectionByOther(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AgsmDeath* pThis = static_cast<AgsmDeath*>(pClass);
	PVOID* ppvBuffer = static_cast<PVOID*>(pCustData);

	CHAR* szName = static_cast<CHAR*>(ppvBuffer[0]);
	INT32 lFlag = *static_cast<INT32*>(ppvBuffer[1]);

	AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// lFlag 가 1이면 부활하겠다고 승낙한 것임.
	// 부활 처리 해준다.
	if(lFlag == 1)
	{
		// 클라이언트에서 보낸 이름과 서버에 저장된 이름이 같아야 한다.
		if(_tcsncmp(pcsAgsdCharacter->m_szResurrectionCaster, szName, _tcslen(pcsAgsdCharacter->m_szResurrectionCaster)) == 0)
		{
			pThis->ResurrectionNowNoPenalty(pcsCharacter);
		}
	}

	ZeroMemory(pcsAgsdCharacter->m_szResurrectionCaster, sizeof(pcsAgsdCharacter->m_szResurrectionCaster));
	return TRUE;
}

// 2005.10.14. steeple
BOOL AgsmDeath::CBEndPeriodStart(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsSummons = (AgpdCharacter*)pData;
	AgsmDeath* pThis = (AgsmDeath*)pClass;

	pThis->SetDead(pcsSummons);

	return TRUE;
}

// 2004.04.29. steeple
BOOL AgsmDeath::WriteDeathLog(AgpdCharacter* pAgpdCharacter)
{
	// final attacker based
	if (!m_pagpmLog)
		return FALSE;
	
	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	INT32 lLevel = m_pagpmCharacter->GetLevel(pAgpdCharacter);
	INT64 llExp = m_pagpmCharacter->GetExp(pAgpdCharacter);

	CHAR szEnemy[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	CHAR szDesc[128];
	
	ZeroMemory(szEnemy, sizeof(szEnemy));
	ZeroMemory(szDesc, sizeof(szDesc));
	
	AgpdCharacter *pEnemyCharacter = NULL;
	AgsdCharacterHistoryEntry *pcsEntry = m_pagsmCharacter->GetFinalAttacker(pAgpdCharacter);
	if (pcsEntry)
	{
		ApBase *pcsFinalAttacker = m_papmEventManager->GetBase(pcsEntry->m_csSource.m_eType, pcsEntry->m_csSource.m_lID);
		if (pcsFinalAttacker && pcsFinalAttacker->m_eType == APBASE_TYPE_CHARACTER)
		{
			pEnemyCharacter = (AgpdCharacter *) pcsFinalAttacker;

			if (m_pagpmCharacter->IsStatusSummoner(pEnemyCharacter)
				|| m_pagpmCharacter->IsStatusTame(pEnemyCharacter)
				|| m_pagpmCharacter->IsStatusFixed(pEnemyCharacter))
			{
				AgpdCharacter *pTemp = NULL;
				if (m_pagpmSummons)
					pTemp = m_pagpmCharacter->GetCharacter(m_pagpmSummons->GetOwnerCID(pEnemyCharacter));
				pEnemyCharacter = pTemp ? pTemp : pEnemyCharacter;
			}

			if (m_pagpmCharacter->IsPC(pEnemyCharacter))
			{
				_tcscpy(szEnemy, pEnemyCharacter->m_szID);
			}
			else
			{
				_tcscpy(szEnemy, pEnemyCharacter->m_pcsCharacterTemplate ? 
								pEnemyCharacter->m_pcsCharacterTemplate->m_szTName :
								_T("@Unknown"));
			}
		}
	}

	CHAR szPosition[33];
	ZeroMemory(szPosition, sizeof(szPosition));
	this->m_pagsmCharacter->EncodingPosition(&pAgpdCharacter->m_stPos, szPosition, 32);	

	AgsdDeath *pAgsdDeath = GetADCharacter(pAgpdCharacter);
	if (pAgsdDeath)
	{
		switch (pAgsdDeath->m_cDeadType)
		{
			case AGPMPVP_TARGET_TYPE_MOB :
				_stprintf(szDesc, _T("Monster (%s)"), szPosition);
				break;			
			case AGPMPVP_TARGET_TYPE_NORMAL_PC :
				_stprintf(szDesc, _T("Player  (%s)"), szPosition);
				break;			
			case AGPMPVP_TARGET_TYPE_ENEMY_GUILD :
				_stprintf(szDesc, _T("Guild  (%s)"), szPosition);
				break;
		}
	}		

	m_pagpmLog->WriteLog_CharDeath(0, &pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pAgpdCharacter->m_szID,
									pAgpdCharacter->m_pcsCharacterTemplate->m_lID,
									lLevel,
									llExp,
									pAgpdCharacter->m_llMoney,
									pAgpdCharacter->m_llBankMoney,
									szEnemy,
									szDesc,
									NULL // guild
									);

	return TRUE;
}


BOOL AgsmDeath::SetCallbackCharacterDead(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMDEATH_CB_CHARACTER_DEAD, pfCallback, pClass);
}

BOOL AgsmDeath::SetCallbackPreProcessCharacterDead(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMDEATH_CB_PRE_PROCESS_CHARACTER_DEAD, pfCallback, pClass);
}

BOOL AgsmDeath::SetCallbackResurrection(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMDEATH_CB_RESURRECTION, pfCallback, pClass);
}
