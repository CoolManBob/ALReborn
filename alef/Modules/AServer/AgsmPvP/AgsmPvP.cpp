// AgsmPvP.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2004. 12. 26.


#include "AgsmPvP.h"
#include <list>
#include "AgsmAI2.h"
#include "AgpmBattleGround.h"
#include "AgsmBattleGround.h"

using namespace std;

////////////////////////////////////////////////////////////////////////

const TCHAR	AgsmPvP::s_szCharStatus[AGSMPVP_CHAR_STATUS_MAX][30] = 
	{
	_T("normal"),
	_T("assailant"),
	_T("villain_step01"),
	_T("villain_step02"),
	_T("villain_step03")
	};


const INT32	AgsmPvP::s_lDropEquipItem[AGSMPVP_DROP_EQUIP_PART_MAX] = 
	{
	AGPMITEM_PART_HEAD,					//	3
	AGPMITEM_PART_BODY,					//	2
	AGPMITEM_PART_LEGS,					//	6
	AGPMITEM_PART_HANDS,				//	4
	AGPMITEM_PART_FOOT,					//	7
	AGPMITEM_PART_HAND_LEFT,			//	9
	AGPMITEM_PART_HAND_RIGHT,			//	10					
	};

const INT32 AgsmPvP::s_lNextEvalEquip[AGSMPVP_DROP_EQUIP_PART_MAX] = 
	{
	AGSMPVP_DROP_EQUIP_PART_SLEEVE,		//AGSMPVP_DROP_EQUIP_PART_HEAD
	AGSMPVP_DROP_EQUIP_PART_LEG,		//AGSMPVP_DROP_EQUIP_PART_BREAST,
	AGSMPVP_DROP_EQUIP_PART_HEAD,		//AGSMPVP_DROP_EQUIP_PART_LEG,
	AGSMPVP_DROP_EQUIP_PART_FOOT,		//AGSMPVP_DROP_EQUIP_PART_SLEEVE,
	AGSMPVP_DROP_EQUIP_PART_SHIELD,		//AGSMPVP_DROP_EQUIP_PART_FOOT,
	AGSMPVP_DROP_EQUIP_PART_WEAPON,		//AGSMPVP_DROP_EQUIP_PART_SHIELD,
	AGSMPVP_DROP_EQUIP_PART_BREAST		//AGSMPVP_DROP_EQUIP_PART_HAND,
	};

const TCHAR AgsmPvP::s_szRaceBattleStatus[AGPMPVP_RACE_BATTLE_STATUS_MAX][20] = 
	{
	"none",			// AGPMPVP_RACE_BATTLE_STATUS_NONE = 0,
	"ready",		// 	AGPMPVP_RACE_BATTLE_STATUS_READY,
	"start",		// AGPMPVP_RACE_BATTLE_STATUS_ING,
	"end_not_use",	// AGPMPVP_RACE_BATTLE_STATUS_ENDED,
	"reward",		// AGPMPVP_RACE_BATTLE_STATUS_REWARD,
	"rewardend",	// AGPMPVP_RACE_BATTLE_STATUS_MAX
	};

AgsmPvP::AgsmPvP()
{
	SetModuleName("AgsmPvP");
	SetModuleType(APMODULE_TYPE_SERVER);

	m_papmMap = NULL;
	m_papmEventManager = NULL;
	m_pagpmCharacter = NULL;
	m_pagpmFactors = NULL;
	m_pagpmParty = NULL;
	m_pagpmGuild = NULL;
	m_pagpmGrid = NULL;
	m_pagpmItem = NULL;
	m_pagpmEventTeleport = NULL;
	m_pagpmSummons = NULL;
	m_pagpmSkill = NULL;
	m_pagpmPvP = NULL;
	m_pagpmLog = NULL;
	m_pagpmConfig = NULL;
	m_pagpmSiegeWar = NULL;

	m_pagsmAOIFilter = NULL;
	m_pagsmCharacter = NULL;
	m_pagsmCharManager = NULL;
	m_pagsmCombat = NULL;
	m_pagsmDeath = NULL;
	m_pagsmSummons = NULL;
	m_pagsmSkill = NULL;
	m_pagsmItem = NULL;
	m_pagsmItemManager = NULL;
	m_pagsmGuild = NULL;
	
	m_lEquipDropRate.MemSetAll();
	m_lBagDropRate.MemSetAll();
	m_ItemDropRate.MemSetAll();

	// 하드코딩으로 일단 TID 넣어준다. ㅜㅜ
	m_vcPCTID.push_back(96);
	m_vcPCTID.push_back(1);
	m_vcPCTID.push_back(6);
	m_vcPCTID.push_back(4);
	m_vcPCTID.push_back(8);
	m_vcPCTID.push_back(3);
	m_vcPCTID.push_back(460);
	m_vcPCTID.push_back(9);

	m_vcSummonsTID.push_back(543);
	m_vcSummonsTID.push_back(613);
	m_vcSummonsTID.push_back(464);
	m_vcSummonsTID.push_back(463);
	m_vcSummonsTID.push_back(462);
	m_vcSummonsTID.push_back(461);
	m_vcSummonsTID.push_back(541);
	m_vcSummonsTID.push_back(542);
	m_vcSummonsTID.push_back(468);
	m_vcSummonsTID.push_back(639);

	m_lTotalCharismaPoint = 0;
	m_CharismaPoint.MemSetAll();	

	m_RaceBattlePointLock.Init();
	m_KilledCharMapLock.Init();
	ZeroMemory(m_szIniFile, sizeof(m_szIniFile));
	m_ulRaceBattleTimeOfSec.MemSetAll();
	m_lSiegeDayOfYear = -1;
	m_lNotSiegeDayOfYear = -1;
	
	m_ulPreviousClock = 0;
	m_lRemained = 0;

	EnableIdle(TRUE);
}

AgsmPvP::~AgsmPvP()
{
	m_RaceBattlePointLock.Destroy();
	m_KilledCharMapLock.Destroy();
}

BOOL AgsmPvP::OnAddModule()
{
	m_papmMap = (ApmMap*)GetModule("ApmMap");
	m_papmEventManager = (ApmEventManager*)GetModule("ApmEventManager");
	m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagpmFactors = (AgpmFactors*)GetModule("AgpmFactors");
	m_pagpmParty = (AgpmParty*)GetModule("AgpmParty");
	m_pagpmGuild = (AgpmGuild*)GetModule("AgpmGuild");
	m_pagpmGrid = (AgpmGrid*)GetModule("AgpmGrid");
	m_pagpmItem = (AgpmItem*)GetModule("AgpmItem");
	m_pagpmEventTeleport = (AgpmEventTeleport *)GetModule("AgpmEventTeleport");
	m_pagpmSummons = (AgpmSummons*)GetModule("AgpmSummons");
	m_pagpmSkill = (AgpmSkill*)GetModule("AgpmSkill");
	m_pagpmPvP = (AgpmPvP*)GetModule("AgpmPvP");
	m_pagpmLog = (AgpmLog*)GetModule("AgpmLog");
	m_pagpmConfig = (AgpmConfig*)GetModule("AgpmConfig");
	m_pagpmSiegeWar = (AgpmSiegeWar*)GetModule("AgpmSiegeWar");

	m_pagsmAOIFilter = (AgsmAOIFilter*)GetModule("AgsmAOIFilter");
	m_pagsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pagsmCharManager = (AgsmCharManager*)GetModule("AgsmCharManager");
	m_pagsmCombat = (AgsmCombat*)GetModule("AgsmCombat");
	m_pagsmDeath = (AgsmDeath*)GetModule("AgsmDeath");
	m_pagsmSummons = (AgsmSummons*)GetModule("AgsmSummons");
	m_pagsmSkill = (AgsmSkill*)GetModule("AgsmSkill");
	m_pagsmItem = (AgsmItem*)GetModule("AgsmItem");
	m_pagsmItemManager = (AgsmItemManager*)GetModule("AgsmItemManager");
	m_pagsmGuild = (AgsmGuild*)GetModule("AgsmGuild");
	m_pagsmAI2 = (AgsmAI2*)GetModule("AgsmAI2");

	if(!m_papmEventManager ||!m_pagpmCharacter || !m_pagpmFactors ||
		!m_pagpmParty || !m_pagpmGuild ||
		!m_pagpmPvP || !m_pagpmEventTeleport ||
		!m_pagpmGrid || !m_pagpmItem ||
		!m_pagsmAOIFilter || !m_pagsmCharacter ||
		!m_pagsmCharManager || !m_pagsmCombat ||
		!m_pagsmDeath || !m_pagsmSkill ||
		!m_pagsmItem || !m_pagsmItemManager ||
		!m_pagsmGuild || !m_pagpmConfig ||
		!m_pagpmSiegeWar
		)
		return FALSE;

	if(!m_pagpmPvP->SetCallbackEnterSafePvPArea(CBEnterSafePvPArea, this))
		return FALSE;
	if(!m_pagpmPvP->SetCallbackEnterFreePvPArea(CBEnterFreePvPArea, this))
		return FALSE;
	if(!m_pagpmPvP->SetCallbackEnterCombatPvPArea(CBEnterCombatPvPArea, this))
		return FALSE;
	if(!m_pagpmPvP->SetCallbackLeaveFreePvPArea(CBLeaveFreePvPArea, this))
		return FALSE;
	if(!m_pagpmPvP->SetCallbackLeaveCombatPvPArea(CBLeaveCombatPvPArea, this))
		return FALSE;
	if(!m_pagpmPvP->SetCallbackSystemMessage(CBProcessSystemMessage, this))
		return FALSE;
	if(!m_pagpmPvP->SetCallbackRequestDeadType(CBRequestDeadType, this))
		return FALSE;
	if(!m_pagpmPvP->SetCallbackResponseDeadType(CBResponseDeadType, this))
		return FALSE;

	//if(!m_pagpmCharacter->SetCallbackBindingRegionChange(CBRegionChange, this))
	//	return FALSE;
	if(!m_pagpmCharacter->SetCallbackUpdateChar(CBUpdateCharacter, this))
		return FALSE;
	//if(!m_pagpmCharacter->SetCallbackCheckValidNormalAttack(CBIsAttackable, this))
	//	return FALSE;

	if(!m_pagsmCharacter->SetCallbackSendCharView(CBSendCharView, this))
		return FALSE;
	if(!m_pagsmCharacter->SetCallbackSendCharacterAllInfo(CBSendCharacterAllInfo, this))
		return FALSE;
	if(!m_pagsmCharacter->SetCallbackSendCharacterNewID(CBSendCharacterAllInfo, this))
		return FALSE;

	if(!m_pagsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this))
		return FALSE;
	if(!m_pagsmCharManager->SetCallbackDisconnectCharacter(CBDisconnect, this))
		return FALSE;

	if(!m_pagsmAOIFilter->SetCallbackMoveChar(CBMoveCell, this))
		return FALSE;
	
	if(!m_pagpmEventTeleport->SetCallbackCheckUsableTeleport(CBIsUsableTeleport, this))
		return FALSE;

	if(!m_pagsmCombat->SetCallbackAttackStart(CBOnAttack, this))
		return FALSE;

	if(!m_pagsmDeath->SetCallbackCharacterDead(CBOnDead, this))
		return FALSE;

	if(!m_pagsmSkill->SetCallbackAttack(CBOnSkill, this))
		return FALSE;
	//if(!m_pagsmSkill->SetCallbackCheckSkillPvP(CBIsSkillEnable, this))
	//	return FALSE;

	if(!m_pagsmGuild->SetCallbackDBMemberDelete(CBLeaveGuild, this))
		return FALSE;
	if(!m_pagsmGuild->SetCallbackAddEnemyGuildByBattle(CBAddEnemyGuildByBattle, this))
		return FALSE;
	if(!m_pagsmGuild->SetCallbackRemoveEnemyGuildByBattle(CBRemoveEnemyGuildByBattle, this))
		return FALSE;
	if(!m_pagsmGuild->SetCallbackAddEnemyGuildBySurviveBattle(CBAddEnemyPrivateGuildByBattle, this))
		return FALSE;
	if(!m_pagsmGuild->SetCallbackRemoveEnemyGuildBySurviveBattle(CBRemoveEnemyPrivateGuildByBattle, this))
		return FALSE;

	if(!m_pagpmParty->SetCBRemoveMember(CBLeaveParty, this))
		return FALSE;
	if(!m_pagpmParty->SetCBRemoveParty(CBDestroyParty, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmPvP::OnInit()
{
#ifdef REMOVE_RACEPVP_CHN //중국 종족전 삭제
	if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
	{
		if (!m_PacketSender.Set(1024 * 1024, m_pagsmAOIFilter, "RBPSender")
			|| !m_PacketSender.Start())
			return FALSE;
	}
#endif
	m_pagpmBattleGround		= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagsmBattleGround		= (AgsmBattleGround*)GetModule("AgsmBattleGround");
	m_pagsmTitle			= (AgsmTitle*)GetModule("AgsmTitle");
	
	return TRUE;
}

BOOL AgsmPvP::OnIdle(UINT32 ulClockCount)
{
#ifdef REMOVE_RACEPVP_CHN //중국 종족전 삭제
	if (AP_SERVICE_AREA_CHINA == g_eServiceArea &&
		!m_pagpmSiegeWar->IsDayOfSiegeWar() &&
		ulClockCount > m_ulPreviousClock + AGSMPVP_IDLE_CLOCK_INTERVAL)
	{
		ProcessRaceBattle();
		m_ulPreviousClock = ulClockCount;
	}	
#endif 
	return TRUE;
}

BOOL AgsmPvP::OnDestroy()
{
#ifdef REMOVE_RACEPVP_CHN //중국 종족전 삭제
	if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
		m_PacketSender.Stop();
#endif
	return TRUE;
}







// pcsCharacter 가 pcsTarget 을 때렸다
BOOL AgsmPvP::OnAttack(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	if(pcsCharacter == pcsTarget)
		return TRUE;

	// 
	if (m_pagsmAI2)
	{
		m_pagsmAI2->CheckHideMonster(pcsTarget, GetClockCount());
		m_pagsmAI2->CheckRunaway(pcsTarget, GetClockCount());
	}

	// 모두 PC 일 때만 하면 된다.
	if(!m_pagpmCharacter->IsPC(pcsCharacter) || !m_pagpmCharacter->IsPC(pcsTarget))
	{
		// 둘중 하나라도 소환수 관련이라면 소환수 관련 루틴을 탄다. 2005.10.08. steeple
		if(m_pagpmCharacter->IsStatusSummoner(pcsCharacter) || m_pagpmCharacter->IsStatusTame(pcsCharacter) ||
			m_pagpmCharacter->IsStatusSummoner(pcsTarget) || m_pagpmCharacter->IsStatusTame(pcsTarget))
            return OnAttackSummons(pcsCharacter, pcsTarget);

		return TRUE;
	}

	// 다시한번 PC 가 아니면 루틴을 타지 않는다. 2005.10.08. steeple
	if(!m_pagpmCharacter->IsPC(pcsCharacter) || !m_pagpmCharacter->IsPC(pcsTarget))
		return TRUE;

	// 가드일 때는 빠진다.
	if(strlen(pcsCharacter->m_szID) == 0 || strlen(pcsTarget->m_szID) == 0)
		return TRUE;
		
	// BattleGround
	if(m_pagpmBattleGround->IsAttackable(pcsCharacter, pcsTarget))
		return TRUE;

	// 적이 아니고(먼저 때린놈이 아니고) 나한테 선공불가 페널티가 있으면
	if (FALSE == m_pagpmPvP->IsEnemy(pcsCharacter, pcsTarget) &&
		m_pagpmCharacter->HasPenalty(pcsCharacter, AGPMCHAR_PENALTY_FIRST_ATTACK)
		)
		return TRUE;

	// 공격자가 무적상태라면 풀어준다.
	if(m_pagpmPvP->IsInvincible(pcsCharacter))
	{
		UnsetInvincible(pcsCharacter);
	}

	UINT32 ulCurrentClock = GetClockCount();

	INT32 lCount = 0;
	lCount = ProcessEnemyGuild(pcsCharacter, pcsTarget, ulCurrentClock);
	if (0 == lCount)
		lCount = ProcessEnemy(pcsCharacter, pcsTarget, ulCurrentClock);

	return TRUE;
}

// 공격가능한 상대에게 스킬을 썼는지 체크해서 OnAttack 으로 바로 해준다.
BOOL AgsmPvP::OnSkill(AgpdSkill* pcsSkill, AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsSkill || !pcsCharacter || !pcsTarget)
		return FALSE;

	if(m_pagpmPvP->IsSkillEnable(pcsSkill, pcsTarget))
	{
		// 공격 or 디버프 스킬일때만 걸어준다.
		if(m_pagpmPvP->IsAttackSkill(pcsSkill))
			OnAttack(pcsCharacter, pcsTarget);
	}

	return TRUE;
}

// 캐릭터가 죽었다.
// 둘다 PC 일 때만 처리해주고, pcsCharacter 가 마지막 때린놈, pcsTarget 이 죽은놈이다.
BOOL AgsmPvP::OnDead(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bDisconnect)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	AgsdDeath* pcsAgsdDeath = m_pagsmDeath->GetADCharacter(pcsTarget);
	if(!pcsAgsdDeath)
		return FALSE;

	// 2005.07.28. steeple
	// 경험치 처리 때문에 넣음.
	pcsAgsdDeath->m_lDeadTargetCID = pcsCharacter->m_lID;

	// CombatArea 에서 PC 가 죽었을 때는 아래 값을 세팅한다.
	if(m_pagpmPvP->IsCombatPvPMode(pcsTarget) && m_pagpmCharacter->IsPC(pcsTarget) &&
		strlen(pcsCharacter->m_szID) > 0)
	{
		m_pagpmPvP->SetDeadInCombatArea(pcsTarget, TRUE);
	}

	// 2005.07.28. steeple
	// 때린 놈은 몬스터이고, 죽은 놈은 사람이다. 경험치 처리 때문에 넣음.
	if(m_pagpmCharacter->IsMonster(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
		pcsAgsdDeath->m_cDeadType = (INT8)AGPMPVP_TARGET_TYPE_MOB;

	// 2005.10.10. steeple
	// 때린 놈이 소환수이면 주인으로 바꿔치기 한다.
	if(m_pagpmCharacter->IsStatusSummoner(pcsCharacter) || m_pagpmCharacter->IsStatusTame(pcsCharacter) ||
		m_pagpmCharacter->IsStatusFixed(pcsCharacter))
	{
		AuAutoLock Lock;
		AgpdCharacter* pcsOwner = NULL;
		INT32 lOwnerCID = m_pagpmSummons->GetOwnerCID(pcsCharacter);
		if(lOwnerCID)
		{
			pcsOwner = m_pagpmCharacter->GetCharacter(lOwnerCID);

			// Owner 를 구했으면 pcsCharacter 를 바꿔치기 한다.
			if(pcsOwner)
			{
				Lock.SetMutualEx(pcsOwner->m_Mutex);
				Lock.Lock();
				if (Lock.Result())
					pcsCharacter = pcsOwner;
			}
		}
	}

	if(!m_pagpmCharacter->IsPC(pcsCharacter) || !m_pagpmCharacter->IsPC(pcsTarget))
	{
		if (pcsAgsdDeath->m_cDeadType == (INT8)AGPMPVP_TARGET_TYPE_MOB)
			WriteDeathLog(pcsCharacter, pcsTarget, pcsAgsdDeath->m_cDeadType);
		return TRUE;
	}
	
	// 가드일 때는 빠진다.
	if(strlen(pcsCharacter->m_szID) == 0 || strlen(pcsTarget->m_szID) == 0)
		return TRUE;

	//////////////////////////////////////////////////////////////////////////
	//
	// 모두 PC 일 때만 하면 된다.
	//
	pcsAgsdDeath->m_cDeadType = (INT8)AGPMPVP_TARGET_TYPE_NORMAL_PC;
	
	AgpdPvPADChar* pcsAttachedPvP1 = m_pagpmPvP->GetADCharacter(pcsCharacter);
	AgpdPvPADChar* pcsAttachedPvP2 = m_pagpmPvP->GetADCharacter(pcsTarget);
	if(!pcsAttachedPvP1 || !pcsAttachedPvP2)
		return TRUE;
		
	//////////////////////////////////////////////////////////////////////////
	// 배틀그라운드 - arycoat 2008.02
	if(m_pagsmBattleGround->OnDead(pcsCharacter, pcsTarget))
		return TRUE;
	
	//////////////////////////////////////////////////////////////////////////
	//
	// 서로가 적인지 확인한다.
	if(!m_pagpmPvP->IsEnemy(pcsCharacter, pcsTarget) && !m_pagpmPvP->IsEnemyGuild(pcsCharacter, m_pagpmGuild->GetJoinedGuildID(pcsTarget)))
		return TRUE;

	if(!m_pagpmPvP->IsEnemy(pcsTarget, pcsCharacter) && !m_pagpmPvP->IsEnemyGuild(pcsTarget, m_pagpmGuild->GetJoinedGuildID(pcsCharacter)))
		return TRUE;

	//////////////////////////////////////////////////////////////////////////
	
	if(m_pagsmGuild->CalcBattleScore(pcsCharacter, pcsTarget)) // 길드전일때
	{
		pcsAgsdDeath->m_cDeadType = (INT8)AGPMPVP_TARGET_TYPE_ENEMY_GUILD;
	}
	else 
	{
		if(m_pagpmGuild->IsHostileGuild(pcsCharacter, pcsTarget)) // 길드전이 아니고, Hostile 길드에게 죽어도 길드전 처리와 같게 해준다.
			pcsAgsdDeath->m_cDeadType = (INT8)AGPMPVP_TARGET_TYPE_ENEMY_GUILD;
		
		// 길드전이 아니고 둘다 위험전투지역이 아닌 경우 악당 포인트 처리
		if( !m_pagpmPvP->IsCombatPvPMode(pcsCharacter) && !m_pagpmPvP->IsCombatPvPMode(pcsTarget) && !bDisconnect )	// 죽지 않았는데 디스커넥트로 인해서 죽은 처리 되면서 머더러 포인트가 올라갔었음. 2006.04.18. steeple
		{
			ProcessMurderer(pcsCharacter, pcsTarget);
			/*ProcessCharisma(pcsCharacter, pcsTarget);*/ //배틀그라운드 - arycoat 2008.7
		}
		
		EnumCallback(AGSMPVP_CB_PVPDEAD, pcsCharacter, pcsTarget); // 현상금 처리
	}

	// 공통적인 부분을 처리한다.
	SendSystemMessage(AGPMPVP_SYSTEM_CODE_KILL_PLAYER, m_pagsmCharacter->GetCharDPNID(pcsCharacter), pcsTarget->m_szID);
	SendSystemMessage(AGPMPVP_SYSTEM_CODE_DEAD, m_pagsmCharacter->GetCharDPNID(pcsTarget), pcsCharacter->m_szID);

	// 이긴놈에게 승리를 언져주고 진놈에겐 패배를 언져주자
	pcsAttachedPvP1->m_lWin++;
	pcsAttachedPvP2->m_lLose++;

	// Log
	WriteDeathLog(pcsCharacter, pcsTarget, pcsAgsdDeath->m_cDeadType);
	WriteKillLog(pcsCharacter, pcsTarget, pcsAgsdDeath->m_cDeadType);

	// 타겟과 싸움이 어디서 이루어졌는 지에 따라 구분해서 처리한다.
	// 위험 전투 지역에서 싸웠는지 검사
	if(m_pagpmPvP->IsCombatPvPMode(pcsCharacter) && m_pagpmPvP->IsCombatPvPMode(pcsTarget))
	{
		ProcessDeadInCombatArea(pcsCharacter, pcsTarget);
	}
	else
	{
		ProcessDeadInFreeArea(pcsCharacter, pcsTarget);
	}

	if(AGPMPVP_TARGET_TYPE_ENEMY_GUILD != pcsAgsdDeath->m_cDeadType)
	{
		// 서로 적에서 빼준다.
		if(m_pagpmPvP->RemoveEnemy(pcsCharacter, pcsTarget))
			SendRemoveEnemy(pcsCharacter, pcsTarget->m_lID);
		if(m_pagpmPvP->RemoveEnemy(pcsTarget, pcsCharacter))
			SendRemoveEnemy(pcsTarget, pcsCharacter->m_lID);
	}

	// 결과 패킷을 보내준다.
	SendPvPResult(pcsCharacter, pcsTarget);

	UpdatePvPStatus(pcsCharacter);
	UpdatePvPStatus(pcsTarget);

	// 길드, 파티 처리
	NotifyDeadToFriendGuildMembers(pcsCharacter, pcsTarget);
	NotifyDeadToPartyMembers(pcsCharacter, pcsTarget);

	return TRUE;
}

// Idle 처리
BOOL AgsmPvP::UpdateCharacter(AgpdCharacter* pcsCharacter, UINT32 ulClockCount)
{
	if(!pcsCharacter)
		return FALSE;

	if(!m_pagpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	// 가드일 경우 돌 필요없다.
	if(strlen(pcsCharacter->m_szID) == 0)
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;

	if(!pcsAttachedPvP->m_bInit)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("UpdateCharacter"));

	// Idle Interval Check
	if(!m_pagsmCharacter->IsIdleProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_PVP, ulClockCount))
		return TRUE;

	m_pagsmCharacter->ResetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_PVP);
	m_pagsmCharacter->SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_PVP, ulClockCount);

	// 무적상태를 확인해준다.
	if(pcsAttachedPvP->m_cPvPStatus & AGPDPVP_STATUS_INVINCIBLE)
	{
		AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(pcsAgsdCharacter)
		{
			// 시간이 지나면 해제시켜준다.
			if(ulClockCount > pcsAgsdCharacter->m_ulReleasePvPInvincibleTime)
			{
				UnsetInvincible(pcsCharacter);
			}
			else
			{
				SetPvPIdleInterval(pcsCharacter);
			}
		}
	}


	if (pcsAttachedPvP->m_pEnemyVector && !pcsAttachedPvP->m_pEnemyVector->empty())
	{
		// 적을 돌면서 ClockCount 를 비교해서 처리해준다.
		PvPCharIter iterEnemy = pcsAttachedPvP->m_pEnemyVector->begin();
		INT32 lTempEnemyCID = 0;			// 클라이언트에게 보내줄 CID 백업용

		INT32 lSizeVt = pcsAttachedPvP->m_pEnemyVector->size();

		//for (iterEnemy = pcsAttachedPvP->m_pEnemyVector->begin(); iterEnemy != pcsAttachedPvP->m_pEnemyVector->end(); iterEnemy++)
		for(int i = 0; i < lSizeVt; i++,iterEnemy++)
		{
			// 아직 해제 시간이 되지 않았다.
			if((ulClockCount - iterEnemy->m_ulLastCombatClock) <= AGPMPVP_RELEASE_PVP_TIME)
				continue;

			// 적이 아니라고 해준다.
			lTempEnemyCID = iterEnemy->m_lCID;	// 해제되는 적 CID 백업해두고
			if(m_pagpmPvP->RemoveEnemy(pcsCharacter, iterEnemy->m_lCID))
			{
				SendRemoveEnemy(pcsCharacter, lTempEnemyCID);
				UpdatePvPStatus(pcsCharacter);
			}
			lTempEnemyCID = 0;
		}
	}
	
	if (pcsAttachedPvP->m_pEnemyGuildVector)
	{
		// 적 길드를 돌면서 ClockCount 를 비교한다.
		CHAR* szJoinedGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);

		PvPGuildIter iterGuild = pcsAttachedPvP->m_pEnemyGuildVector->begin();
		CHAR szTempGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1] = {0, };		// 클라이언트에게 보내줄 GuildID 백업용

		INT32 lSizeVt = pcsAttachedPvP->m_pEnemyGuildVector->size();

		//for (iterGuild = pcsAttachedPvP->m_pEnemyGuildVector->begin(); iterGuild != pcsAttachedPvP->m_pEnemyGuildVector->end(); iterGuild++)
		for(int i = 0; i < lSizeVt; i++,iterGuild++)
		{
			// 2005.04.18. steeple
			// 적 길드가 현재 배틀중인 길드라면 빼지 않고 지나간다.
			if(szJoinedGuildID && m_pagpmGuild->IsEnemyGuild(szJoinedGuildID, iterGuild->m_szGuildID))
				continue;

			// 2006.08.16. steeple
			// 적대 길드도 일단 Pass
			if(szJoinedGuildID && m_pagpmGuild->IsHostileGuild(szJoinedGuildID, iterGuild->m_szGuildID))
				continue;

			if((ulClockCount - iterGuild->m_ulLastCombatClock) <= AGPMPVP_RELEASE_PVP_TIME)
				continue;

			sprintf_s(szTempGuildID, sizeof(szTempGuildID), iterGuild->m_szGuildID);
			if(m_pagpmPvP->RemoveEnemyGuild(pcsCharacter, iterGuild->m_szGuildID))
			{
				SendRemoveEnemyGuild(pcsCharacter, szTempGuildID);
				UpdatePvPStatus(pcsCharacter);
			}
			ZeroMemory(szTempGuildID, sizeof(szTempGuildID));
		}
	}

	// 적이 있으면 다음 Idle 시간을 준다.
	if( ( pcsAttachedPvP->m_pEnemyVector && !pcsAttachedPvP->m_pEnemyVector->empty() ) || 
		( pcsAttachedPvP->m_pEnemyGuildVector && !pcsAttachedPvP->m_pEnemyGuildVector->empty() ) )
		SetPvPIdleInterval(pcsCharacter);

	return TRUE;
}

// 2005.01.13. steeple. AgpmPvP 로 옮겨갔음
// Attack 가능한 캐릭터인지 검사
// pcsCharacter 가 pcsTarget 을 패는 것임.
BOOL AgsmPvP::IsAttackable(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bForce)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// PvP 가 아니라면 return TRUE
	if(!m_pagpmCharacter->IsPC(pcsCharacter) || !m_pagpmCharacter->IsPC(pcsTarget))
		return TRUE;

	// 일단 적이라면 return TRUE
	if(m_pagpmPvP->IsEnemyCharacter(pcsCharacter, pcsTarget))
		return TRUE;

	// 적이 아니라면 지역 체크

	// 위험 전투 지역에서 같은 편이면 return FALSE
	if(m_pagpmPvP->IsCombatPvPMode(pcsCharacter) && m_pagpmPvP->IsFriendCharacter(pcsCharacter, pcsTarget))
		return FALSE;

	return TRUE;
}

// Skill 쓸 수 있는 상대인지 검사
// pcsCharacter 가 pcsTarget 에게 쓰는 것임
BOOL AgsmPvP::IsSkillEnable(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget)
{
	//if(!pcsSkill || !pcsTarget)
	//	return FALSE;

	//AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	//if(!pcsSkillTemplate)
	//	return FALSE;

	//AgpdCharacter* pcsCharacter = (AgpdCharacter*)pcsSkill->m_pcsBase;
	//if(!pcsCharacter)
	//	return FALSE;

	//// PvP 가 아니라면 return TRUE
	//if(!m_pagpmCharacter->IsPC(pcsCharacter) || !m_pagpmCharacter->IsPC(pcsTarget))
	//	return TRUE;

	//// 공격 or 디버프 스킬인지 확인한다.
	//BOOL bAttackSkill = FALSE;
	//INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
 //   if(pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_BUFF_TYPE][lSkillLevel] == 1)
	//	bAttackSkill = TRUE;
	//else if(m_pagpmSkill->IsAttackSkill(pcsSkillTemplate))
	//	bAttackSkill = TRUE;

	//// Attack Skill 을 사용할 때 같은 편에게는 쓸 수 없다.
	//if(bAttackSkill && m_pagpmPvP->IsFriendCharacter(pcsCharacter, pcsTarget))
	//	return FALSE;

	return TRUE;
}

// 2006.06.02. steeple
// History 에러든, FinalAttacker 가 로그아웃이던 어떻게든 Exception 이 발생한 경우
BOOL AgsmPvP::ProcessExceptionNonExistFinalAttackerOnDead(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	AgsdDeath* pcsAgsdDeath = m_pagsmDeath->GetADCharacter(pcsCharacter);
	if(!pcsAgsdDeath)
		return FALSE;

	// Death 쪽에 적절하게 값을 설정해준다.
	// 현재 문제는 길드적은 처리할 수 없고 일반 PC 만 가능하다.

	TIDIter iterPD = std::find(m_vcPCTID.begin(), m_vcPCTID.end(), pcsAgsdCharacter->m_stHistory.m_lLastEnemyTID);
	if(iterPD != m_vcPCTID.end())
	{
		pcsAgsdDeath->m_cDeadType = (INT8)AGPMPVP_TARGET_TYPE_NORMAL_PC;
		return TRUE;
	}

	iterPD = std::find(m_vcSummonsTID.begin(), m_vcSummonsTID.end(), pcsAgsdCharacter->m_stHistory.m_lLastEnemyTID);
	if(iterPD != m_vcSummonsTID.end())
	{
		pcsAgsdDeath->m_cDeadType = (INT8)AGPMPVP_TARGET_TYPE_NORMAL_PC;
		return TRUE;
	}

	// 이도 저도 없으면 몬스터한테 뒤진것이삼.
	pcsAgsdDeath->m_cDeadType = (INT8)AGPMPVP_TARGET_TYPE_MOB;

	return TRUE;
}

// 2005.10.08. steeple
// Summons 관련된 놈들과 전투
BOOL AgsmPvP::OnAttackSummons(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!m_pagpmSummons || !m_pagsmSummons)
		return FALSE;

	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// 둘다 PC 면. 여기 와선 안된다.
	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar1 = m_pagpmSummons->GetADCharacter(pcsCharacter);
	AgpdSummonsADChar* pcsSummonsADChar2 = m_pagpmSummons->GetADCharacter(pcsTarget);
	if(!pcsSummonsADChar1 || !pcsSummonsADChar2)
		return FALSE;

	// 둘다 소환수 관련
	if(pcsSummonsADChar1->m_lOwnerCID != 0 && pcsSummonsADChar2->m_lOwnerCID != 0)
	{
		AgpdCharacter* pcsOwner1 = m_pagpmCharacter->GetCharacter(pcsSummonsADChar1->m_lOwnerCID);
		AgpdCharacter* pcsOwner2 = m_pagpmCharacter->GetCharacter(pcsSummonsADChar2->m_lOwnerCID);
		if(!pcsOwner1 || !pcsOwner2)
			return FALSE;

		AuAutoLock csLock1(pcsOwner1->m_Mutex);
		AuAutoLock csLock2(pcsOwner2->m_Mutex);

		if (csLock1.Result() && csLock2.Result())
			return OnAttack(pcsOwner1, pcsOwner2);
		else
			return FALSE;
	}
	// 공격자만 소환수 관련.
	else if(pcsSummonsADChar1->m_lOwnerCID != 0 && pcsSummonsADChar2->m_lOwnerCID == 0)
	{
		AgpdCharacter* pcsOwner = m_pagpmCharacter->GetCharacter(pcsSummonsADChar1->m_lOwnerCID);
		if(!pcsOwner)
			return FALSE;

		AuAutoLock csLock(pcsOwner->m_Mutex);

		if (!csLock.Result()) return FALSE;

		return OnAttack(pcsOwner, pcsTarget);
	}
	// 맞는 놈만 소환수 관련.
	else if(pcsSummonsADChar1->m_lOwnerCID == 0 && pcsSummonsADChar2->m_lOwnerCID != 0)
	{
		AgpdCharacter* pcsOwner = m_pagpmCharacter->GetCharacter(pcsSummonsADChar2->m_lOwnerCID);	// Target 의 Owner 를 얻는다.
		if(!pcsOwner)
			return FALSE;

		AuAutoLock csLock(pcsOwner->m_Mutex);
		if (!csLock.Result()) return FALSE;

		return OnAttack(pcsCharacter, pcsOwner);
	}

	return FALSE;
}

template<typename AgpdPvPCharInfo> 
struct SendAddPvPCharInfo : public unary_function<AgpdPvPCharInfo, void>
{
	enum
	{
		SEND_ADD_FRIEND = 0,
		SEND_ADD_ENEMY,
	};

	SendAddPvPCharInfo(INT32 lType, AgsmPvP* pAgsmPvP, AgpdCharacter* pcsCharacter) :
		m_lType(lType), m_pAgsmPvP(pAgsmPvP), m_pcsCharacter(pcsCharacter) {}

	void operator() (AgpdPvPCharInfo csPvPCharInfo)
	{
		switch(m_lType)
		{
			case SEND_ADD_FRIEND:
				m_pAgsmPvP->SendAddFriend(m_pcsCharacter, csPvPCharInfo.m_lCID);
				break;

			case SEND_ADD_ENEMY:
				m_pAgsmPvP->SendAddEnemy(m_pcsCharacter, csPvPCharInfo.m_lCID);
				break;
		}
	}

	INT32 m_lType;
	AgsmPvP* m_pAgsmPvP;
	AgpdCharacter* m_pcsCharacter;
};

template<typename AgpdPvPGuildInfo>
struct SendAddPvPGuildInfo : public unary_function<AgpdPvPGuildInfo, void>
{
	enum
	{
		SEND_ADD_FRIEND_GUILD = 0,
		SEND_ADD_ENEMY_GUILD,
	};

	SendAddPvPGuildInfo(INT32 lType, AgsmPvP* pAgsmPvP, AgpdCharacter* pcsCharacter) :
		m_lType(lType), m_pAgsmPvP(pAgsmPvP), m_pcsCharacter(pcsCharacter) {}

	void operator() (AgpdPvPGuildInfo csPvPGuildInfo)
	{
		switch(m_lType)
		{
			case SEND_ADD_FRIEND_GUILD:
				m_pAgsmPvP->SendAddFriendGuild(m_pcsCharacter, csPvPGuildInfo.m_szGuildID);
				break;

			case SEND_ADD_ENEMY_GUILD:
				m_pAgsmPvP->SendAddEnemyGuild(m_pcsCharacter, csPvPGuildInfo.m_szGuildID);
				break;
		}
	}

	INT32 m_lType;
	AgsmPvP* m_pAgsmPvP;
	AgpdCharacter* m_pcsCharacter;
};

// pcsSource 의 모든 Friend, Enemy 리스트를 pcsTarget 으로 복사한다.
// bPacket 이 TRUE 이면 Add 패킷을 보내준다.
INT32 AgsmPvP::CopyFriendEnemy(AgpdCharacter* pcsSource, AgpdCharacter* pcsTarget, BOOL bPacket)
{
	if(!pcsSource || !pcsTarget)
		return 0;

	AgpdPvPADChar* pcsSourcePvP = m_pagpmPvP->GetADCharacter(pcsSource);
	AgpdPvPADChar* pcsTargetPvP = m_pagpmPvP->GetADCharacter(pcsTarget);
	if(!pcsSourcePvP || !pcsTargetPvP)
		return 0;

	INT32 lCount = 0;

	// 복사
	pcsTargetPvP->m_pFriendVector->assign(pcsSourcePvP->m_pFriendVector->begin(), pcsSourcePvP->m_pFriendVector->end());
	if(bPacket)
	{
		std::for_each(pcsTargetPvP->m_pFriendVector->begin(), pcsTargetPvP->m_pFriendVector->end(), 
						SendAddPvPCharInfo<AgpdPvPCharInfo>(0, this, pcsTarget));
	}
	lCount += (INT32)pcsTargetPvP->m_pFriendVector->size();

	pcsTargetPvP->m_pEnemyVector->assign(pcsSourcePvP->m_pEnemyVector->begin(), pcsSourcePvP->m_pEnemyVector->end());
	if(bPacket)
	{
		std::for_each(pcsTargetPvP->m_pEnemyVector->begin(), pcsTargetPvP->m_pEnemyVector->end(), 
						SendAddPvPCharInfo<AgpdPvPCharInfo>(1, this, pcsTarget));
	}
	lCount += (INT32)pcsTargetPvP->m_pEnemyVector->size();

	pcsTargetPvP->m_pFriendGuildVector->assign(pcsSourcePvP->m_pFriendGuildVector->begin(), pcsSourcePvP->m_pFriendGuildVector->end());
	if(bPacket)
	{
		std::for_each(pcsTargetPvP->m_pFriendGuildVector->begin(), pcsTargetPvP->m_pFriendGuildVector->end(), 
						SendAddPvPGuildInfo<AgpdPvPGuildInfo>(0, this, pcsTarget));
	}
	lCount += (INT32)pcsTargetPvP->m_pFriendGuildVector->size();

	pcsTargetPvP->m_pEnemyGuildVector->assign(pcsSourcePvP->m_pEnemyGuildVector->begin(), pcsSourcePvP->m_pEnemyGuildVector->end());
	if(bPacket)
	{
		std::for_each(pcsTargetPvP->m_pEnemyGuildVector->begin(), pcsTargetPvP->m_pEnemyGuildVector->end(), 
						SendAddPvPGuildInfo<AgpdPvPGuildInfo>(1, this, pcsTarget));
	}
	lCount += (INT32)pcsTargetPvP->m_pEnemyGuildVector->size();

	//INT32 lCount = 0, lIndex = 0;

	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	//AgpdPvPCharInfo* pcsNewCharInfo = NULL;


	//// 같은 편 복사
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsSourcePvP->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsSourcePvP->m_pFriendList->GetObjectSequence(&lIndex))
	//{
	//	m_pagpmPvP->AddFriend(pcsTarget, pcsPvPCharInfo->m_lCID);
	//	pcsNewCharInfo = m_pagpmPvP->GetFriendInfo(pcsTarget, pcsPvPCharInfo->m_lCID);
	//	if(!pcsNewCharInfo)
	//		continue;

	//	pcsNewCharInfo->m_ePvPMode = pcsPvPCharInfo->m_ePvPMode;
	//	pcsNewCharInfo->m_ulLastCombatClock = pcsPvPCharInfo->m_ulLastCombatClock;

	//	//pcsNewCharInfo = (AgpdPvPCharInfo*)m_pagpmPvP->CreateModuleData(AGPMPVP_DATA_TYPE_CHAR_INFO);
	//	//if(!pcsNewCharInfo)
	//	//	break;

	//	//memcpy(pcsNewCharInfo, pcsPvPCharInfo, sizeof(AgpdPvPCharInfo));

	//	//pcsTargetPvP->m_pFriendList->AddObject((PVOID)pcsNewCharInfo, pcsNewCharInfo->m_lCID);
	//	if(bPacket)
	//		SendAddFriend(pcsTarget, pcsNewCharInfo->m_lCID);

	//	lCount++;
	//}

	//pcsPvPCharInfo = NULL;
	//pcsNewCharInfo = NULL;
	//lIndex = 0;

	//// 적 복사
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsSourcePvP->m_pEnemyList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsSourcePvP->m_pEnemyList->GetObjectSequence(&lIndex))
	//{
	//	m_pagpmPvP->AddEnemy(pcsTarget, pcsPvPCharInfo->m_lCID);
	//	pcsNewCharInfo = m_pagpmPvP->GetEnemyInfo(pcsTarget, pcsPvPCharInfo->m_lCID);
	//	if(!pcsNewCharInfo)
	//		continue;

	//	pcsNewCharInfo->m_ePvPMode = pcsPvPCharInfo->m_ePvPMode;
	//	pcsNewCharInfo->m_ulLastCombatClock = pcsPvPCharInfo->m_ulLastCombatClock;

	//	//pcsNewCharInfo = (AgpdPvPCharInfo*)m_pagpmPvP->CreateModuleData(AGPMPVP_DATA_TYPE_CHAR_INFO);
	//	//if(!pcsNewCharInfo)
	//	//	break;

	//	//memcpy(pcsNewCharInfo, *ppcsPvPCharInfo, sizeof(AgpdPvPCharInfo));

	//	//pcsTargetPvP->m_pEnemyList->AddObject((PVOID)&pcsNewCharInfo, pcsNewCharInfo->m_lCID);
	//	if(bPacket)
	//		SendAddEnemy(pcsTarget, pcsNewCharInfo->m_lCID);

	//	lCount++;
	//}

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	//AgpdPvPGuildInfo* pcsNewPvPGuildInfo = NULL;
	//INT32 lGuildIDLength = 0;
	//lIndex = 0;

	//// 같은 편 길드 복사
	//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsSourcePvP->m_pFriendGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsSourcePvP->m_pFriendGuildList->GetObjectSequence(&lIndex))
	//{
	//	m_pagpmPvP->AddFriendGuild(pcsTarget, pcsPvPGuildInfo->m_szGuildID);
	//	pcsNewPvPGuildInfo = m_pagpmPvP->GetFriendGuildInfo(pcsTarget, pcsPvPGuildInfo->m_szGuildID);
	//	if(!pcsNewPvPGuildInfo)
	//		continue;

	//	pcsNewPvPGuildInfo->m_ePvPMode = pcsPvPGuildInfo->m_ePvPMode;
	//	pcsNewPvPGuildInfo->m_ulLastCombatClock = pcsPvPGuildInfo->m_ulLastCombatClock;
	//	
	//	//AgpdPvPGuildInfo* pcsPvPGuildInfo = (AgpdPvPGuildInfo*)m_pagpmPvP->CreateModuleData(AGPMPVP_DATA_TYPE_GUILD_INFO);
	//	//if(!pcsPvPGuildInfo)
	//	//	break;

	//	//memcpy(pcsPvPGuildInfo, *ppcsPvPGuildInfo, sizeof(AgpdPvPGuildInfo));

	//	//pcsTargetPvP->m_pFriendGuildList->AddObject((PVOID)&pcsPvPGuildInfo, pcsPvPGuildInfo->m_szGuildID);
	//	if(bPacket)
	//		SendAddFriendGuild(pcsTarget, pcsPvPGuildInfo->m_szGuildID);

	//	lCount++;
	//}

	//pcsPvPGuildInfo = NULL;
	//pcsNewPvPGuildInfo = NULL;
	//lGuildIDLength = 0;
	//lIndex = 0;

	//// 적 길드 복사
	//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsSourcePvP->m_pEnemyGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsSourcePvP->m_pEnemyGuildList->GetObjectSequence(&lIndex))
	//{
	//	m_pagpmPvP->AddEnemyGuild(pcsTarget, pcsPvPGuildInfo->m_szGuildID);
	//	pcsNewPvPGuildInfo = m_pagpmPvP->GetEnemyGuildInfo(pcsTarget, pcsPvPGuildInfo->m_szGuildID);
	//	if(!pcsNewPvPGuildInfo)
	//		continue;

	//	pcsNewPvPGuildInfo->m_ePvPMode = pcsPvPGuildInfo->m_ePvPMode;
	//	pcsNewPvPGuildInfo->m_ulLastCombatClock = pcsPvPGuildInfo->m_ulLastCombatClock;
	//	
	//	//AgpdPvPGuildInfo* pcsPvPGuildInfo = (AgpdPvPGuildInfo*)m_pagpmPvP->CreateModuleData(AGPMPVP_DATA_TYPE_GUILD_INFO);
	//	//if(!pcsPvPGuildInfo)
	//	//	break;

	//	//memcpy(pcsPvPGuildInfo, *ppcsPvPGuildInfo, sizeof(AgpdPvPGuildInfo));

	//	//pcsTargetPvP->m_pEnemyGuildList->AddObject((PVOID)&pcsPvPGuildInfo, pcsPvPGuildInfo->m_szGuildID);
	//	if(bPacket)
	//		SendAddEnemyGuild(pcsTarget, pcsPvPGuildInfo->m_szGuildID);

	//	lCount++;
	//}

	return lCount;
}

// 전투 지역에 들어왔다는 것을 같은 편에게 알린다.
INT32 AgsmPvP::NotifyEnterCombatAreaToFriend(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	INT32 lCount = 0;
	AgpdCharacter* pcsFriend = NULL;

	PvPCharVector vcTmp(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end());
	PvPCharIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		pcsFriend = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsFriend)
		{
			++iter;
			continue;
		}

		if(m_pagpmPvP->AddFriend(pcsFriend, pcsCharacter->m_lID))
			SendAddFriend(pcsFriend, pcsCharacter->m_lID);

		pcsFriend->m_Mutex.Release();		

		++lCount;
		++iter;
	}

	//INT32 lCount = 0, lIndex = 0;
	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	//AgpdCharacter* pcsFriend = NULL;

	//// 같은 편 복사
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex))
	//{
	//	pcsFriend = m_pagpmCharacter->GetCharacterLock(pcsPvPCharInfo->m_lCID);
	//	if(!pcsFriend)
	//		continue;

	//	if(m_pagpmPvP->AddFriend(pcsFriend, pcsCharacter->m_lID))
	//		SendAddFriend(pcsFriend, pcsCharacter->m_lID);

	//	pcsFriend->m_Mutex.Release();		

	//	lCount++;
	//}

	// 같은 길드에게는 안해두 된다.
	// 정말???

	return lCount;
}

// 전투 지역에 들어왔다는 것을 적에게 알린다.
INT32 AgsmPvP::NotifyEnterCombatAreaToEnemy(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	AgpdCharacter* pcsEnemy = NULL;
	AgpdPvPADChar* pcsPvPEnemy = NULL;

	UINT32 ulClockCount = GetClockCount();
	INT32 lCount = 0/*, lIndex = 0*/;
	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;

	// 적들에게 알린다.
	PvPCharVector vcTmp(pcsAttachedPvP->m_pEnemyVector->begin(), pcsAttachedPvP->m_pEnemyVector->end());
	PvPCharIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		pcsEnemy = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsEnemy)
		{
			++iter;
			continue;
		}

		if(m_pagpmPvP->IsCombatPvPMode(pcsEnemy))
		{
			lCount += AddEnemyToCharacter(pcsEnemy, pcsCharacter, (INT8)pcsAttachedPvP->m_ePvPMode, ulClockCount);
		}

		pcsEnemy->m_Mutex.Release();
		++iter;
	}

	//// 적들에게 알린다.
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObjectSequence(&lIndex))
	//{
	//	pcsEnemy = m_pagpmCharacter->GetCharacterLock(pcsPvPCharInfo->m_lCID);
	//	if(!pcsEnemy)
	//		continue;

	//	if(m_pagpmPvP->IsCombatPvPMode(pcsEnemy))
	//	{
	//		lCount += AddEnemyToCharacter(pcsEnemy, pcsCharacter, (INT8)pcsAttachedPvP->m_ePvPMode, ulClockCount);
	//	}

	//	pcsEnemy->m_Mutex.Release();
	//}

	// 길드원이 아니라면 적 길드에게 알린다.
	if(!m_pagpmGuild->IsAnyGuildMember(pcsCharacter))
	{
		list<INT32> cMemberIDList;
		PvPGuildIter iter = pcsAttachedPvP->m_pEnemyGuildVector->begin();
		while(iter != pcsAttachedPvP->m_pEnemyGuildVector->end())
		{
			m_pagpmGuild->GetMemberCIDList(iter->m_szGuildID, &cMemberIDList);
			++iter;
		}

		//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
		//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
		//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex))
		//{
		//	m_pagpmGuild->GetMemberCIDList(pcsPvPGuildInfo->m_szGuildID, &cMemberIDList);
		//}

		if(!cMemberIDList.empty())
		{
			list<INT32>::iterator iterData = cMemberIDList.begin();
			while(iterData != cMemberIDList.end())
			{
				pcsEnemy = m_pagpmCharacter->GetCharacterLock(*iterData);
				if(!pcsEnemy)
				{
					++iterData;
					continue;
				}

				if(m_pagpmPvP->IsCombatPvPMode(pcsEnemy))
				{
					lCount += AddEnemyToCharacter(pcsEnemy, pcsCharacter, (INT8)pcsAttachedPvP->m_ePvPMode, ulClockCount);
				}

                pcsEnemy->m_Mutex.Release();
				++iterData;
			}
		}
	}

	return lCount;
}

// 전투 지역에서 나간다는 것을 같은 편에게 알린다.
INT32 AgsmPvP::NotifyLeaveCombatAreaToFriend(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	AgpdCharacter* pcsFriend = NULL;
	AgpdPvPADChar* pcsPvPFriend = NULL;

	INT32 lCount = 0/*, lIndex = 0*/;
	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;

	PvPCharVector vcTmp(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end());
	PvPCharIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		pcsFriend = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsFriend)
		{
			++iter;
			continue;
		}

		if(m_pagpmPvP->RemoveFriend(pcsFriend, pcsCharacter->m_lID))
			SendRemoveFriend(pcsFriend, pcsCharacter->m_lID);

		pcsFriend->m_Mutex.Release();		

		++lCount;
		++iter;
	}
	
	//// 같은 편 에게 알린다.
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex))
	//{
	//	pcsFriend = m_pagpmCharacter->GetCharacterLock(pcsPvPCharInfo->m_lCID);
	//	if(!pcsFriend)
	//		continue;

	//	if(m_pagpmPvP->RemoveFriend(pcsFriend, pcsCharacter->m_lID))
	//		SendRemoveFriend(pcsFriend, pcsCharacter->m_lID);

	//	pcsFriend->m_Mutex.Release();		

	//	lCount++;
	//}

	// 같은 길드에게는 안해두 된다.
	// 정말???

	return lCount;
}

// 전투 지역에서 나간다는 것을 적에게 알린다.
INT32 AgsmPvP::NotifyLeaveCombatAreaToEnemy(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	INT32 lCount = 0;
	
	// 2005.04.18. steeple
	// 길드전 중인 길드는 스킵해준다.
	// 무조건 리셋하게 변경!!! 2005.01.29. steeple
	lCount += NotifyDisconnectToEnemy(pcsCharacter);
	lCount += NotifyDisconnectToEnemyGuildMembers(pcsCharacter, NULL, TRUE);

	// 죽어서 나갈 때는 리셋하고
	//if(pcsAttachedPvP->m_bDeadInCombatArea)
	//{
	//	lCount += NotifyDisconnectToEnemy(pcsCharacter);
	//	lCount += NotifyDisconnectToEnemyGuildMembers(pcsCharacter);
	//}
	//// 살아서 나갈때는 적 길드를 일반적으로 바꿔준다.
	//else
	//{
	//	lCount += pcsAttachedPvP->m_pEnemyList->GetObjectCount();
	//	lCount += ConvertEnemyGuildMemberToEnemy(pcsCharacter);
	//}

	return lCount;
}

// 위험 전투 지역에서 나올 때, 적길드의 멤버들을 일반 적으로 바꿔준다.
INT32 AgsmPvP::ConvertEnemyGuildMemberToEnemy(AgpdCharacter* pcsCharacter, INT8 cNewPvPMode)
{
	if(!pcsCharacter)
		return 0;

	if(!m_pagpmCharacter->IsPC(pcsCharacter))
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;
    
	eAgpdPvPMode eNewPvPMode = cNewPvPMode == -1 ? AGPDPVP_MODE_FREE : (eAgpdPvPMode)cNewPvPMode;
	UINT32 ulLastCombatClock = 0;

	AgpdCharacter* pcsEnemy = NULL;
	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	INT32 /*lIndex = 0,*/ lCount = 0;
	list<INT32> cMemberIDList;

	// 2005.04.18. steeple
	// 캐릭터가 가입한 길드아이디를 구한다.
	CHAR* szJoinedGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);

	PvPGuildVector vcTmp(pcsAttachedPvP->m_pEnemyGuildVector->begin(), pcsAttachedPvP->m_pEnemyGuildVector->end());
	PvPGuildIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
	//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex))
	//{

		// 2005.04.18. steeple
		// 길드전 중인 길드라면 빼지 않고 가지고 있는다.
		if(szJoinedGuildID && m_pagpmGuild->IsEnemyGuild(szJoinedGuildID, iter->m_szGuildID))
		{
			++iter;
			continue;
		}

		// 2006.08.16. steeple
		// 적대 길드도 빼지 않는다.
		if(szJoinedGuildID && m_pagpmGuild->IsHostileGuild(szJoinedGuildID, iter->m_szGuildID))
		{
			++iter;
			continue;
		}

		cMemberIDList.clear();
		m_pagpmGuild->GetMemberCIDList(iter->m_szGuildID, &cMemberIDList);

		ulLastCombatClock = iter->m_ulLastCombatClock;

		// Member 를 돌면서 일반 적으로 넣어준다.
		list<INT32>::iterator iterData = cMemberIDList.begin();
		while(iterData != cMemberIDList.end())
		{
			// Enemy 가 Combat Area 에 있을 때만이다.
			pcsEnemy = m_pagpmCharacter->GetCharacterLock(*iterData);
			if(!pcsEnemy)
			{
				++iterData;
				continue;
			}

			if(m_pagpmPvP->IsCombatPvPMode(pcsEnemy))
			{
				// 양쪽 다 해준다.
				AddEnemyToCharacter(pcsCharacter, pcsEnemy, (INT8)eNewPvPMode, ulLastCombatClock);
				AddEnemyToCharacter(pcsEnemy, pcsCharacter, (INT8)cNewPvPMode, ulLastCombatClock);

				++lCount;
			}

			pcsEnemy->m_Mutex.Release();
			++iterData;
		}

		++iter;
	}

	return lCount;
}

// 접속 종료를 같은 편에게 알린다.
INT32 AgsmPvP::NotifyDisconnectToFriend(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	INT32 lCount = 0/*, lIndex = 0*/;
	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	AgpdCharacter* pcsFriend = NULL;

	PvPCharVector vcTmp(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end());
	PvPCharIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		pcsFriend = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsFriend)
		{
			++iter;
			continue;
		}

		if(m_pagpmPvP->RemoveFriend(pcsFriend, pcsCharacter->m_lID))
			SendRemoveFriend(pcsFriend, pcsCharacter->m_lID);

		pcsFriend->m_Mutex.Release();		

		++lCount;
		++iter;
	}
	
	//// 같은 편 에게 알린다.
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex))
	//{
	//	pcsFriend = m_pagpmCharacter->GetCharacterLock(pcsPvPCharInfo->m_lCID);
	//	if(!pcsFriend)
	//		continue;

	//	if(m_pagpmPvP->RemoveFriend(pcsFriend, pcsCharacter->m_lID))
	//		SendRemoveFriend(pcsFriend, pcsCharacter->m_lID);

	//	pcsFriend->m_Mutex.Release();		

	//	lCount++;
	//}

	return lCount;
}

// 접속 종료를 같은 편 길드 애들에게 알린다.
INT32 AgsmPvP::NotifyDisconnectToFriendGuildMembers(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	// 길드원이라면 안해두 된다.
	if(m_pagpmGuild->IsAnyGuildMember(pcsCharacter))
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	list<INT32> cMemberIDList;
	m_pagpmGuild->GetMemberCIDList(pcsCharacter, &cMemberIDList);	// 먼저 자기 길드원들을 가져온다.

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	INT32 /*lIndex = 0,*/ lCount = 0;

	PvPGuildIter iter = pcsAttachedPvP->m_pFriendGuildVector->begin();
	while(iter != pcsAttachedPvP->m_pFriendGuildVector->end())
	{
		m_pagpmGuild->GetMemberCIDList(iter->m_szGuildID, &cMemberIDList);
		++iter;
	}

	// 같은 편 길드의 멤버 아이디를 얻어온다.
	//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex))
	//{
	//	m_pagpmGuild->GetMemberCIDList(pcsPvPGuildInfo->m_szGuildID, &cMemberIDList);
	//}

	AgpdCharacter* pcsFriend = NULL;
	list<INT32>::iterator iterData = cMemberIDList.begin();
	while(iterData != cMemberIDList.end())
	{
		if(pcsCharacter->m_lID == *iterData)
		{
			++iterData;
			continue;
		}

		pcsFriend = m_pagpmCharacter->GetCharacterLock(*iterData);
		if(!pcsFriend)
		{
			++iterData;
			continue;
		}

		if(m_pagpmPvP->RemoveFriend(pcsFriend, pcsCharacter->m_lID))
			SendRemoveFriend(pcsFriend, pcsCharacter->m_lID);

		++lCount;

		pcsFriend->m_Mutex.Release();
		++iterData;
	}

	return lCount;
}

// 접속 종료를 적에게 알린다.
INT32 AgsmPvP::NotifyDisconnectToEnemy(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	BOOL bIsAnyGuildMember = m_pagpmGuild->IsAnyGuildMember(pcsCharacter);
	CHAR* szGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);

	AgpdCharacter* pcsEnemy = NULL;
	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	INT32 /*lIndex = 0,*/ lCount = 0;

	PvPCharVector vcTmp(pcsAttachedPvP->m_pEnemyVector->begin(), pcsAttachedPvP->m_pEnemyVector->end());
	PvPCharIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		pcsEnemy = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsEnemy)
		{
			++iter;
			continue;
		}

		if(m_pagpmPvP->RemoveEnemy(pcsEnemy, pcsCharacter->m_lID))
		{
			SendRemoveEnemy(pcsEnemy, pcsCharacter->m_lID);
			UpdatePvPStatus(pcsEnemy);
			++lCount;
		}

		pcsEnemy->m_Mutex.Release();
		++iter;
	}

	//// 적들에게 알린다.
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObjectSequence(&lIndex))
	//{
	//	pcsEnemy = m_pagpmCharacter->GetCharacterLock(pcsPvPCharInfo->m_lCID);
	//	if(!pcsEnemy)
	//		continue;

	//	if(m_pagpmPvP->RemoveEnemy(pcsEnemy, pcsCharacter->m_lID))
	//	{
	//		SendRemoveEnemy(pcsEnemy, pcsCharacter->m_lID);
	//		UpdatePvPStatus(pcsEnemy);
	//		lCount++;
	//	}

	//	pcsEnemy->m_Mutex.Release();
	//}

	return lCount;
}

// 접속 종료를 적길드에게 알린다.
INT32 AgsmPvP::NotifyDisconnectToEnemyGuildMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsMostDamager, BOOL bLeaveCombatArea)
{
	if(!pcsCharacter)
		return 0;

	// 2005.04.17. steeple 변경.
	// 길드원이라면 AgsmGuild 의 함수를 불러서 길드전 중일때의 점수 계산을 해준다.
	CHAR* szGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	if(szGuildID && _tcslen(szGuildID) > 0)
	{
		// 2005.04.22. steeple
		// 포탈이나 이동으로 나간거라면 불러주지 않는다.
		if(!bLeaveCombatArea)
			m_pagsmGuild->CalcBattleScoreByDisconnect(pcsCharacter, pcsMostDamager);
		//return 0;
	}

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	INT32 lGuildIDLength = 0;
	INT32 /*lIndex = 0,*/ lCount = 0;;

	list<INT32> cMemberIDList;

	PvPGuildIter iter = pcsAttachedPvP->m_pEnemyGuildVector->begin();
	while(iter != pcsAttachedPvP->m_pEnemyGuildVector->end())
	{
		m_pagpmGuild->GetMemberCIDList(iter->m_szGuildID, &cMemberIDList);
		++iter;
	}

	// 적 길드
	//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex))
	//{
	//	m_pagpmGuild->GetMemberCIDList(pcsPvPGuildInfo->m_szGuildID, &cMemberIDList);
	//}

	AgpdCharacter* pcsEnemy = NULL;
	list<INT32>::iterator iterData = cMemberIDList.begin();
	while(iterData != cMemberIDList.end())
	{
		if(pcsCharacter->m_lID == *iterData)
		{
			++iterData;
			continue;
		}

		pcsEnemy = m_pagpmCharacter->GetCharacterLock(*iterData);
		if(!pcsEnemy)
		{
			++iterData;
			continue;
		}

		// 2007.01.04. steeple
		if(szGuildID && _tcslen(szGuildID) > 0)
		{
			if(m_pagpmPvP->RemoveEnemyGuild(pcsEnemy, szGuildID))
			{
				SendRemoveEnemyGuild(pcsEnemy, szGuildID);
				UpdatePvPStatus(pcsEnemy);
				++lCount;
			}
		}
		else
		{
			if(m_pagpmPvP->RemoveEnemy(pcsEnemy, pcsCharacter->m_lID))
			{
				SendRemoveEnemy(pcsEnemy, pcsCharacter->m_lID);
				UpdatePvPStatus(pcsEnemy);
				++lCount;
			}
		}

		pcsEnemy->m_Mutex.Release();
		++iterData;
	}

	return lCount;
}

// 적이 추가 되었음을 서로에게 알린다.
// 기준은 pcsCharacter 가 된다.
INT32 AgsmPvP::ProcessEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, UINT32 ulClockCount)
{
	if(!pcsCharacter || !pcsEnemy)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	AgpdPvPADChar* pcsAttachedPvPEnemy = m_pagpmPvP->GetADCharacter(pcsEnemy);
	if(!pcsAttachedPvP || !pcsAttachedPvPEnemy)
		return 0;

	// 둘다 위험전투지역이 아닌 경우 선공처리
	// 2007.01.30 선타기준이었는데 막타로 바꾼다. 대략 패킷 이빠이 간다. -_-;
	if(!m_pagpmPvP->IsCombatPvPMode(pcsCharacter) && !m_pagpmPvP->IsCombatPvPMode(pcsEnemy))
		ProcessCriminalAttack(pcsCharacter, pcsEnemy);

	// 이미 서로 적이라면 Update 를 하고 나간다.
	if(m_pagpmPvP->IsEnemy(pcsCharacter, pcsEnemy->m_lID) && m_pagpmPvP->IsEnemy(pcsEnemy, pcsCharacter->m_lID))
	{
		m_pagpmPvP->UpdateEnemy(pcsCharacter, pcsEnemy->m_lID, -1, ulClockCount);
		m_pagpmPvP->UpdateEnemy(pcsEnemy, pcsCharacter->m_lID, -1, ulClockCount);

		if(m_pagpmPvP->IsCombatPvPMode(pcsCharacter))
			UpdateEnemyToFriends(pcsCharacter, pcsEnemy, -1, ulClockCount);
		if(m_pagpmPvP->IsCombatPvPMode(pcsEnemy))
			UpdateEnemyToFriends(pcsEnemy, pcsCharacter, -1, ulClockCount);

		return 1;
	}

	// 서로 적이 되었음을 추가한다.
	AddEnemyToCharacter(pcsCharacter, pcsEnemy, (INT8)pcsAttachedPvP->m_ePvPMode, ulClockCount);
	AddEnemyToCharacter(pcsEnemy, pcsCharacter, (INT8)pcsAttachedPvPEnemy->m_ePvPMode, ulClockCount);
    
	// 위험 전투 지역이 아니라면 여기서 나간다.
	if(!m_pagpmPvP->IsCombatPvPMode(pcsCharacter) || !m_pagpmPvP->IsCombatPvPMode(pcsEnemy))
		return 1;

	INT32 lCount = 1;
	// 적의 같은편을 적으로 추가해준다.
	lCount += AddEnemyFriendsToCharacter(pcsCharacter, pcsEnemy, (INT8)pcsAttachedPvP->m_ePvPMode, ulClockCount);
	lCount += AddEnemyFriendsToCharacter(pcsEnemy, pcsCharacter, (INT8)pcsAttachedPvPEnemy->m_ePvPMode, ulClockCount);

	// 그 다음은 서로 같은 편 애들한테 적이 추가 되었음을 알린다.
	lCount += AddEnemyToFriends(pcsCharacter, pcsEnemy, (INT8)pcsAttachedPvP->m_ePvPMode, ulClockCount);
	lCount += AddEnemyToFriends(pcsEnemy, pcsCharacter, (INT8)pcsAttachedPvPEnemy->m_ePvPMode, ulClockCount);

	return lCount;
}

// 적이 추가 되었을 때 길드인지 체크해서 그쪽 처리를 해준다.
INT32 AgsmPvP::ProcessEnemyGuild(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, UINT32 ulClockCount)
{
	if(!pcsCharacter || !pcsEnemy)
		return 0;

	CHAR* szGuildID1 = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	CHAR* szGuildID2 = m_pagpmGuild->GetJoinedGuildID(pcsEnemy);

	// 2005.04.20. steeple
	// 위험전투지역이 아니라도 길드전 중일 때라면 리턴을 1로 해준다.
	// 위험전투지역이 아니면 그냥 나간다.
	if(!m_pagpmPvP->IsCombatPvPMode(pcsCharacter) || !m_pagpmPvP->IsCombatPvPMode(pcsEnemy))
	{
		if(szGuildID1 && szGuildID2 && strlen(szGuildID1) > 0 && strlen(szGuildID2) > 0)
		{
			if((m_pagpmGuild->IsEnemyGuild(szGuildID1, szGuildID2) &&
				m_pagpmGuild->IsEnemyGuild(szGuildID2, szGuildID1)) ||
				m_pagpmGuild->IsHostileGuild(szGuildID1, szGuildID2))		// 적대길드인 경우에도 리턴 1
				return 1;
		}

		return 0;
	}

	// 이미 적 길드에 추가되어 있으면 업데이트 해준다.
	if(m_pagpmPvP->IsEnemyGuild(pcsCharacter, szGuildID2) && m_pagpmPvP->IsEnemyGuild(pcsEnemy, szGuildID1))
	{
		m_pagpmPvP->UpdateEnemyGuild(pcsCharacter, szGuildID2, -1, ulClockCount);
		UpdateEnemyGuildToFriends(pcsCharacter, pcsEnemy, -1, ulClockCount);
		UpdateEnemyGuildToFriendGuildMembers(pcsCharacter, pcsEnemy, -1, ulClockCount);

		m_pagpmPvP->UpdateEnemyGuild(pcsEnemy, szGuildID1, -1, ulClockCount);
		UpdateEnemyGuildToFriends(pcsEnemy, pcsCharacter, -1, ulClockCount);
		UpdateEnemyGuildToFriendGuildMembers(pcsEnemy, pcsCharacter, -1, ulClockCount);

		return 1;
	}

	BOOL bIsAnyGuildMember1 = m_pagpmGuild->IsAnyGuildMember(pcsCharacter);
	BOOL bIsAnyGuildMember2 = m_pagpmGuild->IsAnyGuildMember(pcsEnemy);

	if(bIsAnyGuildMember1 && !bIsAnyGuildMember2 &&
		m_pagpmPvP->IsEnemy(pcsCharacter, pcsEnemy->m_lID) && m_pagpmPvP->IsEnemyGuild(pcsEnemy, szGuildID1))
	{
		// 때린 놈은 길드원이고 맞은 놈은 길드원이 아니다.
		m_pagpmPvP->UpdateEnemy(pcsCharacter, pcsEnemy->m_lID, -1, ulClockCount);
		UpdateEnemyToFriends(pcsCharacter, pcsEnemy, -1, ulClockCount);
		UpdateEnemyToFriendGuildMembers(pcsCharacter, pcsEnemy, -1, ulClockCount);

		m_pagpmPvP->UpdateEnemyGuild(pcsEnemy, szGuildID1, -1, ulClockCount);
		UpdateEnemyGuildToFriends(pcsEnemy, pcsCharacter, -1, ulClockCount);

		return 1;
	}

	if(!bIsAnyGuildMember1 && bIsAnyGuildMember2 &&
		m_pagpmPvP->IsEnemyGuild(pcsCharacter, szGuildID2) && m_pagpmPvP->IsEnemy(pcsEnemy, pcsCharacter->m_lID))
	{
		// 때린 놈은 길드원이 아니고 맞은 놈은 길드원이다.
		m_pagpmPvP->UpdateEnemyGuild(pcsCharacter, szGuildID2, -1, ulClockCount);
		UpdateEnemyGuildToFriends(pcsCharacter, pcsEnemy, -1, ulClockCount);

		m_pagpmPvP->UpdateEnemy(pcsEnemy, pcsCharacter->m_lID, -1, ulClockCount);
		UpdateEnemyToFriends(pcsEnemy, pcsCharacter, -1, ulClockCount);
		UpdateEnemyToFriendGuildMembers(pcsEnemy, pcsCharacter, -1, ulClockCount);

		return 1;
	}


	///////////////////////////////////////////////////////////////////////////////
	// 여기까지 왔으면 둘다 적이나 적 길드가 아니여서 새로 추가하는 것이다.

	INT32 lCount = 0;
	if(bIsAnyGuildMember1)
	{
		// pcsCharacter 가 길드이기 때문에, pcsEnemy 에 적 길드를 추가한다.
		lCount += AddEnemyFriendGuildsToCharacter(pcsEnemy, pcsCharacter, AGPDPVP_MODE_COMBAT, ulClockCount);

		if(bIsAnyGuildMember2)
		{
			// pcsCharacter, pcsEnemy 둘다 길드임
			lCount += AddEnemyFriendGuildsToCharacter(pcsCharacter, pcsEnemy, AGPDPVP_MODE_COMBAT, ulClockCount);
			lCount += AddEnemyToFriendGuildMembers(pcsCharacter, pcsEnemy, AGPDPVP_MODE_COMBAT, ulClockCount);

			lCount += AddEnemyToFriendGuildMembers(pcsEnemy, pcsCharacter, AGPDPVP_MODE_COMBAT, ulClockCount);
		}
		else
		{
			// pcsCharacter 만 길드, pcsEnemy 는 길드 아님
			lCount += AddEnemyToCharacter(pcsCharacter, pcsEnemy, AGPDPVP_MODE_COMBAT, ulClockCount);
			lCount += AddEnemyFriendsToCharacter(pcsCharacter, pcsEnemy, AGPDPVP_MODE_COMBAT, ulClockCount);
			lCount += AddEnemyToFriendGuildMembers(pcsCharacter, pcsEnemy, AGPDPVP_MODE_COMBAT, ulClockCount);

			lCount += AddEnemyToFriends(pcsEnemy, pcsCharacter, AGPDPVP_MODE_COMBAT, ulClockCount);
		}
	}
	else if(bIsAnyGuildMember2)
	{
		// pcsCharacter 는 길드가 아니고 pcsEnemy 만 길드일 때
		lCount += AddEnemyFriendGuildsToCharacter(pcsCharacter, pcsEnemy, AGPDPVP_MODE_COMBAT, ulClockCount);
		lCount += AddEnemyToFriends(pcsCharacter, pcsEnemy, AGPDPVP_MODE_COMBAT, ulClockCount);
		
		lCount += AddEnemyToCharacter(pcsEnemy, pcsCharacter, AGPDPVP_MODE_COMBAT, ulClockCount);
		lCount += AddEnemyFriendsToCharacter(pcsEnemy, pcsCharacter, AGPDPVP_MODE_COMBAT, ulClockCount);
		lCount += AddEnemyToFriendGuildMembers(pcsEnemy, pcsCharacter, AGPDPVP_MODE_COMBAT, ulClockCount);
	}

	return lCount;
}

// 선공 처리
BOOL AgsmPvP::ProcessCriminalAttack(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTarget)
{
	if (!pcsAttacker || !pcsTarget)
		return FALSE;

	// 2006.10.17. parn
	// 중국은 다른 종족 때려도 선공 아니다.
	//if (g_eServiceArea == AP_SERVICE_AREA_CHINA && !m_pagpmCharacter->IsSameRace(pcsAttacker, pcsTarget))
	//	return TRUE;

	// 2006.08.06. steeple
	// 적대길드이면 선공자 해주지 않는다.
	if (m_pagpmGuild->IsHostileGuild(pcsAttacker, pcsTarget))
		return TRUE;

	// 공성 지역인지 체크해준다.
	BOOL bIsInSiegeWarIngArea = FALSE;
	PVOID pvBuffer[2];
	pvBuffer[0] = &bIsInSiegeWarIngArea;
	pvBuffer[1] = NULL;
	m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pcsTarget, pvBuffer);
	if (bIsInSiegeWarIngArea)
		return TRUE;

	// 공성적인지 체크해준다. 2006.08.09. steeple
	if (m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pcsAttacker, pcsTarget))
		return TRUE;

	// 맞은놈이 나쁜놈이다(말이 이상하군 -_-;)
	if (AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED == pcsTarget->m_unCriminalStatus
		|| m_pagpmCharacter->IsMurderer(pcsTarget))
		return FALSE;
	
	BOOL bCriminal = FALSE;
	EnumCallback(AGSMPVP_CB_CHECK_CRIMINAL, pcsTarget, &bCriminal);
	
	if (bCriminal)
		return FALSE;
	
	m_pagpmCharacter->UpdateCriminalStatus(pcsAttacker, AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED);
	
	return TRUE;	
}

BOOL AgsmPvP::ProcessMurderer(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTarget)
{
	if (!pcsAttacker || !pcsTarget)
		return FALSE;

	// 2006.10.17. parn
	// 중국은 다른 종족 죽여도 살인자 아니다.
	//if (g_eServiceArea == AP_SERVICE_AREA_CHINA && !m_pagpmCharacter->IsSameRace(pcsAttacker, pcsTarget))
	//	return TRUE;

	// 2006.08.06. steeple
	// 적대길드이면 살인자 처리하지 않는다.
	if (m_pagpmGuild->IsHostileGuild(pcsAttacker, pcsTarget))
		return TRUE;

	// 공성 지역인지 체크해준다.
	BOOL bIsInSiegeWarIngArea = FALSE;
	PVOID pvBuffer[2];
	pvBuffer[0] = &bIsInSiegeWarIngArea;
	pvBuffer[1] = NULL;
	m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pcsTarget, pvBuffer);
	if (bIsInSiegeWarIngArea)
		return TRUE;

	// 공성적인지 체크해준다. 2006.08.09. steeple
	if (m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pcsAttacker, pcsTarget))
		return TRUE;

	// 맞은놈이 나쁜놈이다(말이 이상하군 -_-;)
	if (AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED == pcsTarget->m_unCriminalStatus
		|| m_pagpmCharacter->IsMurderer(pcsTarget))
		return FALSE;

	BOOL bCriminal = FALSE;
	EnumCallback(AGSMPVP_CB_CHECK_CRIMINAL, pcsTarget, &bCriminal);
	if (bCriminal)
		return FALSE;
	
	INT32 lAttackerLevel = m_pagpmCharacter->GetLevel(pcsAttacker);
	INT32 lTargetLevel = m_pagpmCharacter->GetLevel(pcsTarget);
	INT32 lPoint = 0;
	
	if (lAttackerLevel >= lTargetLevel + 8)
		lPoint = 2;
	else if (lAttackerLevel >= lTargetLevel + 4)
		lPoint = 1;
	else
		lPoint = 0;

	if(m_pagsmTitle)
		m_pagsmTitle->OnPK(pcsAttacker);
	
	if (lPoint > 0)
	{
		m_pagpmCharacter->AddMurdererPoint(pcsAttacker, lPoint);
		if (m_pagpmLog)
			{
			AgsdCharacter* pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAttacker);
			if(!pAgsdCharacter)
				return FALSE;
	
			INT32 lLevel = m_pagpmCharacter->GetLevel(pcsAttacker);
			INT64 llExp = m_pagpmCharacter->GetExp(pcsAttacker);	
			
			m_pagpmLog->WriteLog_MurdererUp(0, &pAgsdCharacter->m_strIPAddress[0], pAgsdCharacter->m_szAccountID,
								pAgsdCharacter->m_szServerName, pcsAttacker->m_szID,
								((AgpdCharacterTemplate*)pcsAttacker->m_pcsCharacterTemplate)->m_lID,
								lLevel, llExp, pcsAttacker->m_llMoney, pcsAttacker->m_llBankMoney,
								lPoint, m_pagpmCharacter->GetMurdererPoint(pcsAttacker), pcsTarget->m_szID
								);
			}
	}
	
	return TRUE;
}

BOOL AgsmPvP::ProcessCharisma(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTarget)
{
	if (!pcsAttacker || !pcsTarget)
		return FALSE;

	// 공성 지역인지 체크해준다.
	BOOL bIsInSiegeWarIngArea = FALSE;
	PVOID pvBuffer[2];
	pvBuffer[0] = &bIsInSiegeWarIngArea;
	pvBuffer[1] = NULL;
	m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pcsTarget, pvBuffer);
	if (bIsInSiegeWarIngArea)
		return TRUE;

	// 공성적인지 체크해준다. 2006.08.09. steeple
	if (m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pcsAttacker, pcsTarget))
		return TRUE;

	// 맞은놈이 나쁜놈이다(말이 이상하군 -_-;)
	//if (AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED == pcsTarget->m_unCriminalStatus
	//	|| m_pagpmCharacter->IsMurderer(pcsTarget))
	//	return FALSE;

	//BOOL bCriminal = FALSE;
	//EnumCallback(AGSMPVP_CB_CHECK_CRIMINAL, pcsTarget, &bCriminal);
	//if (bCriminal)
	//	return FALSE;
	
	INT32 lAttackerLevel = m_pagpmCharacter->GetLevel(pcsAttacker);
	INT32 lTargetLevel = m_pagpmCharacter->GetLevel(pcsTarget);
	INT32 lPoint = 0;
	INT32 lLevelDiff = lTargetLevel - lAttackerLevel;
	INT32 lKillableCount = 0;
	time_t CurrentTime;
	time(&CurrentTime);
	tm *pCurrentTime = localtime(&CurrentTime);
	INT32 lDayOfYear = pCurrentTime ? pCurrentTime->tm_yday : -1;
	
	for (INT32 i=0; i<m_lTotalCharismaPoint; i++)
	{
		if (lLevelDiff >= m_CharismaPoint[i].m_lDiff)
		{
			if (m_pagpmPvP->IsInRaceBattle())
				lPoint = m_CharismaPoint[i].m_lRaceBattlePoint;
			else
				lPoint = m_CharismaPoint[i].m_lNormalPoint;
				
			lKillableCount = m_CharismaPoint[i].m_lKillableCount;
			break;
		}
	}

	if (lPoint > 0 && IsEnableCharismaPoint(pcsAttacker->m_szID, pcsTarget->m_szID, lDayOfYear, lKillableCount))
	{
		if (m_pagpmPvP->IsInRaceBattle())
		{
			INT32 lRace = this->m_pagpmFactors->GetRace(&pcsAttacker->m_csFactor);
			
			if (lRace > AURACE_TYPE_NONE && AURACE_TYPE_MAX > lRace)
			{
				m_RaceBattlePointLock.Lock();
				m_pagpmPvP->m_lRaceBattlePoint[lRace] += lPoint;
				m_RaceBattlePointLock.Unlock();
			}
		}
	
		m_pagpmCharacter->AddCharismaPoint(pcsAttacker, lPoint);
		SendSystemMessage(AGPMPVP_SYSTEM_CODE_CHARISMA_UP, m_pagsmCharacter->GetCharDPNID(pcsAttacker), NULL, NULL,
					lPoint, lLevelDiff);
		
		if (m_pagpmLog)
			{
			AgsdCharacter* pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAttacker);
			if(!pAgsdCharacter)
				return FALSE;
	
			INT32 lLevel = m_pagpmCharacter->GetLevel(pcsAttacker);
			INT64 llExp = m_pagpmCharacter->GetExp(pcsAttacker);	
			
			m_pagpmLog->WriteLog_CharismaUp(0, &pAgsdCharacter->m_strIPAddress[0], pAgsdCharacter->m_szAccountID,
								pAgsdCharacter->m_szServerName, pcsAttacker->m_szID,
								((AgpdCharacterTemplate*)pcsAttacker->m_pcsCharacterTemplate)->m_lID,
								lLevel, llExp, pcsAttacker->m_llMoney, pcsAttacker->m_llBankMoney,
								lPoint, m_pagpmCharacter->GetCharismaPoint(pcsAttacker), pcsTarget->m_szID
								);
			}
	}
	
	return TRUE;
}

// pcsCharacter 의 같은 편에게 pcsEnemy, pcsEnemy 의 같은편, pcsEnemy 의 같은편 길드를 적으로 추가해준다.
INT32 AgsmPvP::AddEnemyToFriends(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode, UINT32 ulClockCount)
{
	if(!pcsCharacter || !pcsEnemy)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	INT32 lCount = 0/*, lIndex = 0*/;
	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	AgpdCharacter* pcsFriend = NULL;

	PvPCharVector vcTmp(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end());
	PvPCharIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		pcsFriend = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsFriend)
		{
			++iter;
			continue;
		}

		if(m_pagpmPvP->IsCombatPvPMode(pcsFriend))
		{
			if(m_pagpmGuild->IsAnyGuildMember(pcsEnemy))
			{
				lCount += AddEnemyFriendGuildsToCharacter(pcsFriend, pcsEnemy, cNewPvPMode, ulClockCount);
			}
			else
			{
				lCount += AddEnemyToCharacter(pcsFriend, pcsEnemy, cNewPvPMode, ulClockCount);
				lCount += AddEnemyFriendsToCharacter(pcsFriend, pcsEnemy, cNewPvPMode, ulClockCount);
			}
		}

		pcsFriend->m_Mutex.Release();
		++iter;
	}

	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex))
	//{
	//	pcsFriend = m_pagpmCharacter->GetCharacterLock(pcsPvPCharInfo->m_lCID);
	//	if(!pcsFriend)
	//		continue;

	//	if(m_pagpmPvP->IsCombatPvPMode(pcsFriend))
	//	{
	//		if(m_pagpmGuild->IsAnyGuildMember(pcsEnemy))
	//		{
	//			lCount += AddEnemyFriendGuildsToCharacter(pcsFriend, pcsEnemy, cNewPvPMode, ulClockCount);
	//		}
	//		else
	//		{
	//			lCount += AddEnemyToCharacter(pcsFriend, pcsEnemy, cNewPvPMode, ulClockCount);
	//			lCount += AddEnemyFriendsToCharacter(pcsFriend, pcsEnemy, cNewPvPMode, ulClockCount);
	//		}
	//	}

	//	pcsFriend->m_Mutex.Release();
	//}

	return lCount;
}

// pcsCharacter 의 같은편 길드원들에게 pcsEnemy, pcsEnemy 의 같은편, pcsEnemy 의 같은편 길드를 적으로 추가해준다.
INT32 AgsmPvP::AddEnemyToFriendGuildMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode, UINT32 ulClockCount)
{
	if(!pcsCharacter || !pcsEnemy)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	list<INT32> cMemberIDList;
	m_pagpmGuild->GetMemberCIDList(pcsCharacter, &cMemberIDList);

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	INT32 /*lIndex = 0,*/ lCount = 0;

	PvPGuildIter iter = pcsAttachedPvP->m_pFriendGuildVector->begin();
	while(iter != pcsAttachedPvP->m_pFriendGuildVector->end())
	{
		m_pagpmGuild->GetMemberCIDList(iter->m_szGuildID, &cMemberIDList);
		++iter;
	}

	// 같은 편 길드의 멤버 아이디를 얻어온다.
	//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex))
	//{
	//	m_pagpmGuild->GetMemberCIDList(pcsPvPGuildInfo->m_szGuildID, &cMemberIDList);
	//}

	AgpdCharacter* pcsFriend = NULL;
	list<INT32>::iterator iterData = cMemberIDList.begin();
	while(iterData != cMemberIDList.end())
	{
		if(*iterData == pcsCharacter->m_lID)
		{
			++iterData;
			continue;
		}

		pcsFriend = m_pagpmCharacter->GetCharacterLock(*iterData);
		if(!pcsFriend)
		{
			++iterData;
			continue;
		}

		if(m_pagpmPvP->IsCombatPvPMode(pcsFriend))
		{
			if(m_pagpmGuild->IsAnyGuildMember(pcsEnemy))
			{
				lCount += AddEnemyFriendGuildsToCharacter(pcsFriend, pcsEnemy, cNewPvPMode, ulClockCount);
			}
			else
			{
				lCount += AddEnemyToCharacter(pcsFriend, pcsEnemy, cNewPvPMode, ulClockCount);
				lCount += AddEnemyFriendsToCharacter(pcsFriend, pcsEnemy, cNewPvPMode, ulClockCount);
			}
		}

		pcsFriend->m_Mutex.Release();

		++iterData;
	}

	return lCount;
}

// pcsCharacter 에다가 pcsEnemy 를 적으로 추가한다.
INT32 AgsmPvP::AddEnemyToCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode, UINT32 ulClockCount)
{
	if(!pcsCharacter || !pcsEnemy)
		return 0;

	INT32 lCount = 0;
	if(m_pagpmPvP->AddEnemy(pcsCharacter, pcsEnemy->m_lID, cNewPvPMode, ulClockCount))
	{
		SendAddEnemy(pcsCharacter, pcsEnemy->m_lID, cNewPvPMode, ulClockCount);
		UpdatePvPStatus(pcsCharacter);
		SetPvPIdleInterval(pcsCharacter);
		lCount++;
	}

	return lCount;
}

// pcsCharacter 에다가 pcsEnemy 의 같은편을 적으로 추가한다.
INT32 AgsmPvP::AddEnemyFriendsToCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode, UINT32 ulClockCount)
{
	if(!pcsCharacter || !pcsEnemy)
		return 0;

	AgpdPvPADChar* pcsAttachedPvPEnemy = m_pagpmPvP->GetADCharacter(pcsEnemy);
	if(!pcsAttachedPvPEnemy)
		return 0;

	INT32 lCount = 0/*, lIndex = 0*/;

	PvPCharIter iter = pcsAttachedPvPEnemy->m_pFriendVector->begin();
	while(iter != pcsAttachedPvPEnemy->m_pFriendVector->end())
	{
		if(m_pagpmPvP->AddEnemy(pcsCharacter, iter->m_lCID, cNewPvPMode, ulClockCount))
		{
			SendAddEnemy(pcsCharacter, iter->m_lCID, cNewPvPMode, ulClockCount);
			UpdatePvPStatus(pcsCharacter);
			SetPvPIdleInterval(pcsCharacter);
			++lCount;
		}

		++iter;
	}

	//// pcsCharacter 에다가 pcsEnemy 의 같은편을 적으로 추가해준다.
	//for(AgpdPvPCharInfo* pcsPvPEnemyInfo = (AgpdPvPCharInfo*)pcsAttachedPvPEnemy->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPEnemyInfo;
	//		pcsPvPEnemyInfo = (AgpdPvPCharInfo*)pcsAttachedPvPEnemy->m_pFriendList->GetObjectSequence(&lIndex))
	//{
	//	if(m_pagpmPvP->AddEnemy(pcsCharacter, pcsPvPEnemyInfo->m_lCID, cNewPvPMode, ulClockCount))
	//	{
	//		SendAddEnemy(pcsCharacter, pcsPvPEnemyInfo->m_lCID, cNewPvPMode, ulClockCount);
	//		UpdatePvPStatus(pcsCharacter);
	//		SetPvPIdleInterval(pcsCharacter);
	//		lCount++;
	//	}
	//}

	return lCount;
}

// pcsCharacter 에다가 pcsEnemy 의 같은편길드를 적으로 추가한다.
INT32 AgsmPvP::AddEnemyFriendGuildsToCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode, UINT32 ulClockCount)
{
	if(!pcsCharacter || !pcsEnemy)
		return 0;

	AgpdPvPADChar* pcsAttachedPvPEnemy = m_pagpmPvP->GetADCharacter(pcsEnemy);
	if(!pcsAttachedPvPEnemy)
		return 0;

	INT32 lCount = 0;
	CHAR* szEnemyGuildID = m_pagpmGuild->GetJoinedGuildID(pcsEnemy);
	if(szEnemyGuildID && strlen(szEnemyGuildID) > 0)
	{
		if(m_pagpmPvP->AddEnemyGuild(pcsCharacter, szEnemyGuildID, cNewPvPMode, ulClockCount))
		{
			SendAddEnemyGuild(pcsCharacter, szEnemyGuildID, cNewPvPMode, ulClockCount);
			UpdatePvPStatus(pcsCharacter);
			SetPvPIdleInterval(pcsCharacter);
			++lCount;
		}
	}

	PvPGuildIter iter = pcsAttachedPvPEnemy->m_pFriendGuildVector->begin();
	while(iter != pcsAttachedPvPEnemy->m_pFriendGuildVector->end())
	{
		if(m_pagpmPvP->AddEnemyGuild(pcsCharacter, iter->m_szGuildID, cNewPvPMode, ulClockCount))
		{
			SendAddEnemyGuild(pcsCharacter, iter->m_szGuildID, cNewPvPMode, ulClockCount);
			UpdatePvPStatus(pcsCharacter);
			SetPvPIdleInterval(pcsCharacter);
			++lCount;
		}

		++iter;
	}

	// pcsEnemy 의 같은편 길드를 적으로 해준다.
	//INT32 lIndex = 0;
	//for(AgpdPvPGuildInfo* pcsEnemyGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvPEnemy->m_pFriendGuildList->GetObjectSequence(&lIndex); pcsEnemyGuildInfo;
	//	pcsEnemyGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvPEnemy->m_pFriendGuildList->GetObjectSequence(&lIndex))
	//{
	//	if(m_pagpmPvP->AddEnemyGuild(pcsCharacter, pcsEnemyGuildInfo->m_szGuildID, cNewPvPMode, ulClockCount))
	//	{
	//		SendAddEnemyGuild(pcsCharacter, pcsEnemyGuildInfo->m_szGuildID, cNewPvPMode, ulClockCount);
	//		UpdatePvPStatus(pcsCharacter);
	//		SetPvPIdleInterval(pcsCharacter);
	//		lCount++;
	//	}
	//}

	return lCount;
}

INT32 AgsmPvP::RemoveEnemyToCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy )
{
	if( !pcsCharacter || !pcsEnemy )	return 0;

	if( m_pagpmPvP->RemoveEnemy( pcsCharacter, pcsEnemy ) )
	{
		SendRemoveEnemy( pcsCharacter, pcsEnemy->m_lID );
		UpdatePvPStatus( pcsCharacter );
	}

	return 0;
}

// 적 정보가 업데이트 된 것을 같은 편 애들한테 알린다.
INT32 AgsmPvP::UpdateEnemyToFriends(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode, UINT32 ulClockCount)
{
	if(!pcsCharacter || !pcsEnemy)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	INT32 lCount = 0/*, lIndex = 0*/;
	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	AgpdCharacter* pcsFriend = NULL;

	// pcsCharacter 같은 편에다가 pcsEnemy 의 정보를 업데이트 해준다.
	PvPCharVector vcTmp(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end());
	PvPCharIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		pcsFriend = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsFriend)
		{
			++iter;
			continue;
		}

		if(m_pagpmPvP->IsCombatPvPMode(pcsFriend))
		{
			if(m_pagpmPvP->UpdateEnemy(pcsFriend, pcsEnemy->m_lID, cNewPvPMode, ulClockCount))
			{
				++lCount;
			}

			// 양쪽 다 해준다.
			if(m_pagpmPvP->UpdateEnemy(pcsEnemy, pcsFriend->m_lID, cNewPvPMode, ulClockCount))
			{
			}
		}

		pcsFriend->m_Mutex.Release();
		++iter;
	}

	// pcsCharacter 같은 편에다가 pcsEnemy 의 정보를 업데이트 해준다.
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex))
	//{
	//	pcsFriend = m_pagpmCharacter->GetCharacterLock(pcsPvPCharInfo->m_lCID);
	//	if(!pcsFriend)
	//		continue;

	//	if(m_pagpmPvP->IsCombatPvPMode(pcsFriend))
	//	{
	//		if(m_pagpmPvP->UpdateEnemy(pcsFriend, pcsEnemy->m_lID, cNewPvPMode, ulClockCount))
	//		{
	//			lCount++;
	//		}

	//		// 양쪽 다 해준다.
	//		if(m_pagpmPvP->UpdateEnemy(pcsEnemy, pcsFriend->m_lID, cNewPvPMode, ulClockCount))
	//		{
	//		}
	//	}

	//	pcsFriend->m_Mutex.Release();
	//}

	return lCount;
}

// 적 정보가 업데이트 된 것을 같은 편 길드에게 알린다.
INT32 AgsmPvP::UpdateEnemyToFriendGuildMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode, UINT32 ulClockCount)
{
    if(!pcsCharacter || !pcsEnemy)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	list<INT32> cMemberIDList;
	m_pagpmGuild->GetMemberCIDList(pcsCharacter, &cMemberIDList);	// 먼저 자기 길드원들을 가져온다.

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	INT32 /*lIndex = 0,*/ lCount = 0;

	PvPGuildIter iter = pcsAttachedPvP->m_pFriendGuildVector->begin();
	while(iter != pcsAttachedPvP->m_pFriendGuildVector->end())
	{
		m_pagpmGuild->GetMemberCIDList(iter->m_szGuildID, &cMemberIDList);
		++iter;
	}

	// 같은 편 길드의 멤버 아이디를 얻어온다.
	//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex))
	//{
	//	m_pagpmGuild->GetMemberCIDList(pcsPvPGuildInfo->m_szGuildID, &cMemberIDList);
	//}


	AgpdCharacter* pcsFriend = NULL;
	list<INT32>::iterator iterData = cMemberIDList.begin();
	while(iterData != cMemberIDList.end())
	{
		if(pcsCharacter->m_lID == *iterData)
		{
			++iterData;
			continue;
		}

		pcsFriend = m_pagpmCharacter->GetCharacterLock(*iterData);
		if(!pcsFriend)
		{
			++iterData;
			continue;
		}

		if(m_pagpmPvP->IsCombatPvPMode(pcsFriend))
		{
			if(m_pagpmPvP->UpdateEnemy(pcsFriend, pcsEnemy->m_lID, cNewPvPMode, ulClockCount))
			{
				++lCount;
			}
		}

		pcsFriend->m_Mutex.Release();
		++iterData;
	}

	return lCount;
}

// 적 길드 정보가 업데이트 된 것을, 같은 편 애들한테 알린다.
INT32 AgsmPvP::UpdateEnemyGuildToFriends(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode, UINT32 ulClockCount)
{
	if(!pcsCharacter || !pcsEnemy)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	CHAR* szEnemyGuildID = m_pagpmGuild->GetJoinedGuildID(pcsEnemy);
	if(!szEnemyGuildID || strlen(szEnemyGuildID) == 0 || strlen(szEnemyGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return 0;

	INT32 lCount = 0/*, lIndex = 0*/;
	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	AgpdCharacter* pcsFriend = NULL;

	PvPCharVector vcTmp(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end());
	PvPCharIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		pcsFriend = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsFriend)
		{
			++iter;
			continue;
		}

		if(m_pagpmPvP->IsCombatPvPMode(pcsFriend))
		{
			if(m_pagpmPvP->UpdateEnemyGuild(pcsFriend, szEnemyGuildID, cNewPvPMode, ulClockCount))
			{
				//SendUpdateEnemyGuild(pcsFriend, szEnemyGuildID, cNewPvPMode, ulClockCount);
				++lCount;
			}
		}

		pcsFriend->m_Mutex.Release();
		++iter;
	}

	// pcsCharacter 같은 편의 적 길드 정보를 업데이트 해준다.
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex))
	//{
	//	pcsFriend = m_pagpmCharacter->GetCharacterLock(pcsPvPCharInfo->m_lCID);
	//	if(!pcsFriend)
	//		continue;

	//	if(m_pagpmPvP->IsCombatPvPMode(pcsFriend))
	//	{
	//		if(m_pagpmPvP->UpdateEnemyGuild(pcsFriend, szEnemyGuildID, cNewPvPMode, ulClockCount))
	//		{
	//			//SendUpdateEnemyGuild(pcsFriend, szEnemyGuildID, cNewPvPMode, ulClockCount);
	//			lCount++;
	//		}
	//	}

	//	pcsFriend->m_Mutex.Release();
	//}

	return lCount;
}

// 적 길드 정보가 업데이트 된 것을, 같은 편 길드 멤버에게 알린다.
INT32 AgsmPvP::UpdateEnemyGuildToFriendGuildMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cPvPMode, UINT32 ulClockCount)
{
	if(!pcsCharacter || !pcsEnemy)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	CHAR* szEnemyGuildID = m_pagpmGuild->GetJoinedGuildID(pcsEnemy);
	if(!szEnemyGuildID || strlen(szEnemyGuildID) == 0 || strlen(szEnemyGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return 0;

	list<INT32> cMemberIDList;
	m_pagpmGuild->GetMemberCIDList(pcsCharacter, &cMemberIDList);	// 먼저 자기 길드원들을 가져온다.

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	INT32 /*lIndex = 0,*/ lCount = 0;

	PvPGuildIter iter = pcsAttachedPvP->m_pFriendGuildVector->begin();
	while(iter != pcsAttachedPvP->m_pFriendGuildVector->end())
	{
		m_pagpmGuild->GetMemberCIDList(iter->m_szGuildID, &cMemberIDList);
		++iter;
	}

	// 같은 편 길드의 멤버 아이디를 얻어온다.
	//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex))
	//{
	//	m_pagpmGuild->GetMemberCIDList(pcsPvPGuildInfo->m_szGuildID, &cMemberIDList);
	//}


	AgpdCharacter* pcsFriend = NULL;
	list<INT32>::iterator iterData = cMemberIDList.begin();
	while(iterData != cMemberIDList.end())
	{
		if(pcsCharacter->m_lID == *iterData)
		{
			++iterData;
			continue;
		}

		pcsFriend = m_pagpmCharacter->GetCharacterLock(*iterData);
		if(!pcsFriend)
		{
			++iterData;
			continue;
		}

		if(m_pagpmPvP->IsCombatPvPMode(pcsFriend))
		{
			if(m_pagpmPvP->UpdateEnemyGuild(pcsFriend, szEnemyGuildID, cPvPMode, ulClockCount))
			{
				//SendUpdateEnemyGuild(pcsFriend, szEnemyGuildID, cPvPMode, ulClockCount);
				++lCount;
			}
		}

		pcsFriend->m_Mutex.Release();
		++iterData;
	}

	return lCount;
}






// 넘어온 캐릭터의 길드원 중 위험전투지역에 있는 멤버중 한명을 리턴한다.
// 해당 캐릭터는 Lock 상태로 리턴된다.
AgpdCharacter* AgsmPvP::GetOneGuildMemberInCombatArea(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return NULL;

	AgpdCharacter* pcsMember = NULL;
	list<INT32> cMemberIDList;

	m_pagpmGuild->GetMemberCIDList(pcsCharacter, &cMemberIDList);

	list<INT32>::iterator iterData = cMemberIDList.begin();
	while(iterData != cMemberIDList.end())
	{
		if(pcsCharacter->m_lID == *iterData)
		{
			++iterData;
			continue;
		}

		pcsMember = m_pagpmCharacter->GetCharacterLock(*iterData);
		if(!pcsMember)
		{
			++iterData;
			continue;
		}

		if(m_pagpmPvP->IsCombatPvPMode(pcsMember))
			return pcsMember;

		pcsMember->m_Mutex.Release();
		++iterData;
	}

	return NULL;
}

// 넘어온 캐릭터의 파티원 중 위험전투지역에 있는 멤버 한명을 리턴한다.
// 해당 캐릭터는 Lock 상태로 리턴된다.
AgpdCharacter* AgsmPvP::GetOnePartyMemberInCombatArea(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return NULL;

	// 파티원인지 확인
    AgpdPartyADChar* pcsAttachedParty = m_pagpmParty->GetADCharacter(pcsCharacter);
	if(!pcsAttachedParty)
		return 0;

	if(pcsAttachedParty->lPID == AP_INVALID_PARTYID)
		return 0;

	AgpdParty* pcsParty = m_pagpmParty->GetPartyLock(pcsAttachedParty->lPID);
	if(!pcsParty)
		return 0;

	INT32 lFoundCID = 0;
	AgpdCharacter* pcsPartyMember = NULL;

	// 루프를 돌 때 캐릭터 락을 하지는 말자.
	for(INT32 i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		if(pcsParty->m_lMemberListID[i] == pcsCharacter->m_lID)
			continue;

		pcsPartyMember = m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
		if (!pcsPartyMember)
			break;

		// 해당 파티원이 위험전투지역에 이미 들어와 있다.
		if(m_pagpmPvP->IsCombatPvPMode(pcsPartyMember))
		{
			lFoundCID = pcsPartyMember->m_lID;
			break;
		}
	}

	pcsParty->m_Mutex.Release();

	return lFoundCID ? m_pagpmCharacter->GetCharacterLock(lFoundCID) : NULL;
}

// 해당 캐릭터가 길드에서 탈퇴했다. (강퇴포함)
// 고로 해당 캐릭터를 길드적으로 가지고 있던 애들에게 길드적이 아닌 일반적이라고 알려줘야 한다.
INT32 AgsmPvP::ConvertEnemyGuildMeToEnemyMe(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID)
		return 0;

	if(strlen(szGuildID) == 0 || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return 0;

	AgpdCharacter* pcsEnemy = NULL;
	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	INT32 /*lIndex = 0,*/ lCount = 0;

	eAgpdPvPMode ePvPMode = AGPDPVP_MODE_NONE;
	UINT32 ulClockCount = 0;

	PvPCharVector vcTmp(pcsAttachedPvP->m_pEnemyVector->begin(), pcsAttachedPvP->m_pEnemyVector->end());
	PvPCharIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	{
		pcsEnemy = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsEnemy)
		{
			++iter;
			continue;
		}

		ePvPMode = iter->m_ePvPMode;
		ulClockCount = iter->m_ulLastCombatClock;

		// 일반 적으로 추가한다.
		AddEnemyToCharacter(pcsEnemy, pcsCharacter, ePvPMode, ulClockCount);

		pcsEnemy->m_Mutex.Release();
		++lCount;
		++iter;
	}

	// 적들에게 알린다.
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObjectSequence(&lIndex))
	//{
	//	pcsEnemy = m_pagpmCharacter->GetCharacterLock(pcsPvPCharInfo->m_lCID);
	//	if(!pcsEnemy)
	//		continue;

	//	ePvPMode = pcsPvPCharInfo->m_ePvPMode;
	//	ulClockCount = pcsPvPCharInfo->m_ulLastCombatClock;

	//	// 일반 적으로 추가한다.
	//	AddEnemyToCharacter(pcsEnemy, pcsCharacter, ePvPMode, ulClockCount);
	//	lCount++;

	//	pcsEnemy->m_Mutex.Release();
	//}
	
	// 적 길드원들에게도 알려준다.

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	INT32 lGuildIDLength = 0;
	//lIndex = 0;

	list<INT32> cMemberIDList;

	PvPGuildVector vcTmp2(pcsAttachedPvP->m_pEnemyGuildVector->begin(), pcsAttachedPvP->m_pEnemyGuildVector->end());
	PvPGuildIter iter2 = vcTmp2.begin();

	// 적 길드
	while(iter2 != vcTmp2.end())
	{
		cMemberIDList.clear();
		m_pagpmGuild->GetMemberCIDList(iter2->m_szGuildID, &cMemberIDList);

		//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
		//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex))

		list<INT32>::iterator iterData = cMemberIDList.begin();
		while(iterData != cMemberIDList.end())
		{
			if(pcsCharacter->m_lID == *iterData)
			{
				++iterData;
				continue;
			}

			pcsEnemy = m_pagpmCharacter->GetCharacterLock(*iterData);
			if(!pcsEnemy)
			{
				++iterData;
				continue;
			}

			if(m_pagpmPvP->IsCombatPvPMode(pcsEnemy))
			{
				ePvPMode = iter2->m_ePvPMode;
				ulClockCount = iter2->m_ulLastCombatClock;

				// 일반 적으로 추가한다.
				AddEnemyToCharacter(pcsEnemy, pcsCharacter, ePvPMode, ulClockCount);

				// 양쪽 다 해준다.
				AddEnemyToCharacter(pcsCharacter, pcsEnemy, ePvPMode, ulClockCount);

				++lCount;
			}

			pcsEnemy->m_Mutex.Release();
			++iterData;
		}
		++iter2;
	}

	return lCount;
}

// Guild Battle - Add Enemy GuildID To Member
// 2005.04.17. steeple
BOOL AgsmPvP::AddEnemyGuildByBattle(AgpdCharacter* pcsCharacter, CHAR* szEnemyGuildID)
{
	if(!pcsCharacter || !szEnemyGuildID)
		return FALSE;

	INT8 cNewPvPMode = (INT8)AGPDPVP_MODE_BATTLE;
	UINT32 ulClockCount = GetClockCount();

	if(m_pagpmPvP->AddEnemyGuild(pcsCharacter, szEnemyGuildID, cNewPvPMode, ulClockCount))
	{
		// 안에서 이미 적길드였다면 Update 자동으로 된다. - 2005.04.17. steeple
		SendAddEnemyGuild(pcsCharacter, szEnemyGuildID, cNewPvPMode, ulClockCount);
		UpdatePvPStatus(pcsCharacter);
		SetPvPIdleInterval(pcsCharacter);
	}

	return TRUE;
}

// Guild Battle - Remove Enemy GuildID To Member
// 2005.04.17. steeple
BOOL AgsmPvP::RemoveEnemyGuildByBattle(AgpdCharacter* pcsCharacter, CHAR* szEnemyGuildID)
{
	if(!pcsCharacter || !szEnemyGuildID)
		return FALSE;

	if(m_pagpmPvP->RemoveEnemyGuild(pcsCharacter, szEnemyGuildID))
	{
		SendRemoveEnemyGuild(pcsCharacter, szEnemyGuildID);
		UpdatePvPStatus(pcsCharacter);
	}

	return TRUE;
}

// Free Area 에서 발생한 전투가 끝났다.
// 승-패 는 이 함수 호출 전에 처리해주었고, 여기서는 적, 아이템 떨구기 정도를 처리해주면 된다.
// pcsCharacter 가 가장 많이 때린놈. pcsTarget 은 진놈
BOOL AgsmPvP::ProcessDeadInFreeArea(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP1 = m_pagpmPvP->GetADCharacter(pcsCharacter);
	AgpdPvPADChar* pcsAttachedPvP2 = m_pagpmPvP->GetADCharacter(pcsTarget);
	if(!pcsAttachedPvP1 || !pcsAttachedPvP2)
		return TRUE;

	eAgpdPvPMode eBakPvPMode1 = pcsAttachedPvP1->m_ePvPMode;
	eAgpdPvPMode eBakPvPMode2 = pcsAttachedPvP2->m_ePvPMode;

	// Item Drop 계산하기 전에 지역을 FREE_AREA 로 맞춰준다.
	pcsAttachedPvP1->m_ePvPMode = AGPDPVP_MODE_FREE;
	pcsAttachedPvP2->m_ePvPMode = AGPDPVP_MODE_FREE;

	ProcessItemDrop(pcsCharacter, pcsTarget);

	// Item Drop 했으면 원래대로 돌려놓는다.
	pcsAttachedPvP1->m_ePvPMode = eBakPvPMode1;
	pcsAttachedPvP2->m_ePvPMode = eBakPvPMode2;

	return TRUE;
}

// Combat Area 에서 발생한 전투가 끝났다.
// 승-패 는 이 함수 호출 전에 처리해주었고, 여기서는 적, 아이템, 해골 떨구기 정도를 처리해주면 된다.
// pcsCharacter 가 가장 많이 때린놈. pcsTarget 은 진놈
BOOL AgsmPvP::ProcessDeadInCombatArea(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP1 = m_pagpmPvP->GetADCharacter(pcsCharacter);
	AgpdPvPADChar* pcsAttachedPvP2 = m_pagpmPvP->GetADCharacter(pcsTarget);
	if(!pcsAttachedPvP1 || !pcsAttachedPvP2)
		return TRUE;

	eAgpdPvPMode eBakPvPMode1 = pcsAttachedPvP1->m_ePvPMode;
	eAgpdPvPMode eBakPvPMode2 = pcsAttachedPvP2->m_ePvPMode;

	// Item Drop 계산하기 전에 지역을 COMBAT_AREA 로 맞춰준다.
	pcsAttachedPvP1->m_ePvPMode = AGPDPVP_MODE_COMBAT;
	pcsAttachedPvP2->m_ePvPMode = AGPDPVP_MODE_COMBAT;

	ProcessItemDrop(pcsCharacter, pcsTarget);
	ProcessSkullDrop(pcsCharacter, pcsTarget);

	// Item Drop 했으면 원래대로 돌려놓는다.
	pcsAttachedPvP1->m_ePvPMode = eBakPvPMode1;
	pcsAttachedPvP2->m_ePvPMode = eBakPvPMode2;

	return TRUE;
}

// PvP 이후 진 사람의 Item 을 드랍한다.
// pcsCharacter 가 이긴놈. pcsTarget 이 진놈.
BOOL AgsmPvP::ProcessItemDrop(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// 성인서버가 아니면 드랍하지 않는다.
	if (FALSE == m_pagpmConfig->IsAdultServer() ||
		TRUE == m_pagpmConfig->IsEventItemDrop())
		return FALSE;

	// ItemDrop을 하면 안되는 지역이라면 드랍하지 않는다. (Check RegionPeculiarity)
	if(m_papmMap->CheckRegionPerculiarity(pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_ITEM_DROP) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
	{
		return FALSE;
	}

	if(m_papmMap->CheckRegionPerculiarity(pcsTarget->m_nBindingRegionIndex, APMMAP_PECULIARITY_ITEM_DROP) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
	{
		return FALSE;
	}

	// 공성 지역인지 체크해준다.
	BOOL bIsInSiegeWarIngArea = FALSE;
	PVOID pvBuffer[2];
	pvBuffer[0] = &bIsInSiegeWarIngArea;
	pvBuffer[1] = NULL;
	m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pcsTarget, pvBuffer);
	if (bIsInSiegeWarIngArea)
		return TRUE;

	// 공성적인지 체크해준다. 2006.08.09. steeple
	if (m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pcsCharacter, pcsTarget))
		return TRUE;

	if (m_papmMap)
	{
		ApmMap::RegionTemplate *	pcsRegion;
		pcsRegion = m_papmMap->GetTemplate(m_papmMap->GetRegion(pcsTarget->m_stPos.x, pcsTarget->m_stPos.z));
		if (pcsRegion)
		{
			if (pcsRegion->ti.stType.uFieldType == ApmMap::FT_TOWN)
				return TRUE;
		}
	}

	AgsdDeath	*pcsAgsdDeath	= m_pagsmDeath->GetADCharacter(pcsTarget);
	if (!pcsAgsdDeath ||
		pcsAgsdDeath->m_cDeadType == AGPMPVP_TARGET_TYPE_ENEMY_GUILD)
		return FALSE;

	// check dead character status
	INT32 lMurdererLevel = m_pagpmCharacter->GetMurdererLevel(pcsTarget);
	BOOL bCriminal = (AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED == pcsTarget->m_unCriminalStatus) ? TRUE : FALSE;
	INT32 lCharStatus = AGSMPVP_CHAR_STATUS_NORMAL;

	if (lMurdererLevel >= AGPMCHAR_MURDERER_LEVEL3_POINT)
		lCharStatus = AGSMPVP_CHAR_STATUS_MURDERER3;
	else if (lMurdererLevel >= AGPMCHAR_MURDERER_LEVEL2_POINT)
		lCharStatus = AGSMPVP_CHAR_STATUS_MURDERER2;
	else if (lMurdererLevel >= AGPMCHAR_MURDERER_LEVEL1_POINT)
		lCharStatus = AGSMPVP_CHAR_STATUS_MURDERER1;
	else if (bCriminal)
		lCharStatus = AGSMPVP_CHAR_STATUS_ASSAILANT;
	else
		lCharStatus = AGSMPVP_CHAR_STATUS_NORMAL;

	INT32 lDropCount = 0;
	INT32 lProb = 0;
	AgpdItem *pAgpdItem = NULL;
	BOOL bNothing = TRUE;
		
	// equip item drop
	for (lDropCount = 0; lDropCount < m_ItemDropRate[lCharStatus].m_lDropEquip; ++lDropCount)
	{
		bNothing = TRUE;

		INT32 lPart = AGSMPVP_DROP_EQUIP_PART_BREAST;
		// evaluate part
		while (TRUE)
		{
			// 해당 파트에 아이템이 있는지 우선 체크
			pAgpdItem = GetEquipItemByDropPart(pcsTarget, lPart);
			if (pAgpdItem && m_pagpmItem->IsNotBound(pAgpdItem))
			{
				bNothing = FALSE;
				lProb = m_csRandom.randInt(99);
				if (lProb <= m_lEquipDropRate[lPart])	// hit
				{
					break;
				}
			}
			lPart = s_lNextEvalEquip[lPart];
			pAgpdItem = NULL;
			
			// 한바퀴돌았는데 equip아이템이 하나도 없다. 이런 거지쉑히~~
			if (AGSMPVP_DROP_EQUIP_PART_BREAST == lPart && bNothing)
				break;
		}

		lProb = m_csRandom.randInt(99);
		if (NULL != pAgpdItem && lProb <= m_ItemDropRate[lCharStatus].m_lRateEquip[lPart])
		{
			if (!UseAntiDrop(pcsTarget, pAgpdItem->m_pcsItemTemplate->m_lID))
			{
				// drop	
				m_pagsmItem->WriteDropLog(pcsTarget->m_lID, pAgpdItem, pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1);
				AddItemToField(pcsTarget, NULL, pAgpdItem);
				// 죽은 놈한테 시스템 메시지를 뿌린다.
				SendSystemMessage(AGPMPVP_SYSTEM_CODE_ITEM_DROP, m_pagsmCharacter->GetCharDPNID(pcsTarget), NULL, NULL,
					pAgpdItem->m_pcsItemTemplate->m_lID);
			}
		}
	}

	// bag item drop
	for (lDropCount = 0; lDropCount < m_ItemDropRate[lCharStatus].m_lDropBag; ++lDropCount)
	{
		bNothing = TRUE;
		BOOL bContinue = TRUE;
		INT32 lBag = 0;
		BOOL BagItemExist[AGPMITEM_MAX_INVENTORY] = {TRUE, TRUE, TRUE, TRUE};
		
		// evaluate bag
		while (bContinue)
		{
			for (INT32 l = 0; l < AGPMITEM_MAX_INVENTORY; ++l)
			{
				// 해당 인벤에 아이템이 있는지 체크
				if (BagItemExist[l])
				{
					GetInvenItemByDropPart(pcsTarget, l, &BagItemExist[l]);
					if (BagItemExist[l])
					{
						bNothing = FALSE;
						lProb = m_csRandom.randInt(99);
						if (lProb <= m_lEquipDropRate[l])	// hit
						{
							lBag = l;
							bContinue = FALSE;
							break;
						}
					}
				}
			}
			
			// 인벤에 아이템이 하나도 없다. 이런 진짜 거렁뱅이쉐이
			if (bNothing)
			{
				bContinue = FALSE;
			}
		}

		lProb = m_csRandom.randInt(99);
		if (FALSE == bNothing && lProb <= m_ItemDropRate[lCharStatus].m_lRateBag[lBag])
		{	
			// drop	
			pAgpdItem = GetInvenItemByDropPart(pcsTarget, lBag);
			if (pAgpdItem)
			{
				if (!UseAntiDrop(pcsTarget, pAgpdItem->m_pcsItemTemplate->m_lID))
				{
					// drop
					m_pagsmItem->WriteDropLog(pcsTarget->m_lID, pAgpdItem, pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1);
					AddItemToField(pcsTarget, NULL, pAgpdItem);
					// send message to dead character
					SendSystemMessage(AGPMPVP_SYSTEM_CODE_ITEM_DROP, m_pagsmCharacter->GetCharDPNID(pcsTarget), NULL, NULL,
						pAgpdItem->m_pcsItemTemplate->m_lID);
				}
			}
		}
	}

	return TRUE;
}

BOOL AgsmPvP::UseAntiDrop(AgpdCharacter *pAgpdCharacter, INT32 lTID)
{
	if (!pAgpdCharacter)
		return FALSE;
	
	// 수호부를 찾자.
	AgpdGridItem *pAgpdGridItem = m_pagpmItem->GetUsingCashItemUsableByType(pAgpdCharacter, AGPMITEM_USABLE_TYPE_ANTI_DROP, 0);
	if (NULL == pAgpdGridItem)
		return FALSE;
	
	AgpdItem *pAgpdItem = m_pagpmItem->GetItem(pAgpdGridItem->m_lItemID);
	if (NULL == pAgpdItem)
		return FALSE;

	// log
	m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_USE, pAgpdCharacter->m_lID, pAgpdItem, 1);

	// sub item
	m_pagpmItem->SubItemStackCount(pAgpdItem, 1);
	
	// send message
	SendSystemMessage(AGPMPVP_SYSTEM_CODE_ITEM_ANTI_DROP, m_pagsmCharacter->GetCharDPNID(pAgpdCharacter), NULL, NULL, lTID);
	
	return TRUE;
}

// PvP 이후 Skull Drop 을 한다.
BOOL AgsmPvP::ProcessSkullDrop(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	if(m_papmMap->CheckRegionPerculiarity(pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_SKUL_DROP) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
		return FALSE;

	if(m_papmMap->CheckRegionPerculiarity(pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_DROP_SKUL_RESTRICTION) == APMMAP_PECULIARITY_RETURN_ENABLE_USE)
	{
		UINT32 ulCurrentTime = AuTimeStamp::GetCurrentTimeStamp();

		if(ulCurrentTime < m_pagpmCharacter->GetLastKilledTimeInBattleSquare(pcsTarget) + m_papmMap->LoadRegioinPerculiaritySkulRestrictionTime(pcsCharacter->m_nBindingRegionIndex))
			return FALSE;
		else
			m_pagpmCharacter->SetLastKilledTimeInBattleSquare(pcsTarget, ulCurrentTime);
	}

	// 공성 지역인지 체크해준다.
	BOOL bIsInSiegeWarIngArea = FALSE;
	PVOID pvBuffer[2];
	pvBuffer[0] = &bIsInSiegeWarIngArea;
	pvBuffer[1] = NULL;
	m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pcsTarget, pvBuffer);
	if (bIsInSiegeWarIngArea)
		return TRUE;

	// 공성적인지 체크해준다. 2006.08.09. steeple
	if (m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pcsCharacter, pcsTarget))
		return TRUE;

	INT32 lSkulTid = m_papmMap->LoadRegioinPerculiaritySkulTid(pcsCharacter->m_nBindingRegionIndex); // 특수한 해골 대신 떨어뜨릴 아이템 지정 되어있는지.
	INT32 lGiveItemTid = 0;

	if(lSkulTid <= 0)
	{
		INT32 lSkullLevel = m_pagpmPvP->GetSkullDropLevel(pcsCharacter, pcsTarget);
		if(lSkullLevel < 1)
			return FALSE;

		lGiveItemTid = m_pagpmItem->GetSkullTID(lSkullLevel);
		if(lGiveItemTid < 1)
			return FALSE;
	}
	else
	{
		lGiveItemTid = lSkulTid;
	}
	
	// 해골 TID 를 얻어서 하나 만든다.
	AgpdItemTemplate* pcsItemTemplate = m_pagpmItem->GetItemTemplate(lGiveItemTid);
	if(!pcsItemTemplate)
		return FALSE;

	AgpdItem* pcsSkull = NULL;
	if(pcsItemTemplate->m_bStackable)
		pcsSkull = m_pagsmItemManager->CreateItem(lGiveItemTid, NULL, 1);
	else
		pcsSkull = m_pagsmItemManager->CreateItem(lGiveItemTid, NULL);

	if(!pcsSkull)
		return FALSE;

	// 해골 레벨을 세팅해주고
	//pcsSkull->m_pstSkullInfo->lSkullUnionRank = lSkullLevel;

	// 귀속 아이템으로 세팅
	//m_pagpmItem->SetBoundType(pcsSkull, E_AGPMITEM_BIND_ON_ACQUIRE);
	//m_pagpmItem->SetBoundOnOwner(pcsSkull, pcsCharacter);

	AddItemToField(pcsCharacter, pcsTarget, pcsSkull);

	return TRUE;
}

// Equip 에서, 두번째 인자로 넘어온 위치를 바탕으로 드랍할 아이템을 리턴한다.
AgpdItem* AgsmPvP::GetEquipItemByDropPart(AgpdCharacter* pcsCharacter, INT32 lPart)
{
	if(!pcsCharacter)
		return NULL;

	AgpdItem* pcsItem = NULL;
	BOOL bFound = FALSE;

	pcsItem = m_pagpmItem->GetEquipSlotItem(pcsCharacter, (AgpmItemPart) s_lDropEquipItem[lPart]);
	
	return pcsItem;
}

// Inven 에서, 두번째 인자로 넘어온 위치를 바탕으로 드랍할 아이템을 리턴한다.
AgpdItem* AgsmPvP::GetInvenItemByDropPart(AgpdCharacter* pcsCharacter, INT32 lLayer, BOOL *pbExist)
{
	if(!pcsCharacter)
		return NULL;

	AgpdItem* pcsReturnItem = NULL;
	AgpdItemADChar* pcsItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
	if(!pcsItemADChar)
		return NULL;

	// 그리드에 아이템이 있는경우 어레이에 넣어놓고 있는 아이템 중 랜덤하게 선택한다.
	// pbExist가 NULL이 아니면 해당 레이어에 아이템이 있는지 없는지 체크만 하고 나간다.

	ApSafeArray<AgpdItem *, AGPMITEM_INVENTORY_ROW * AGPMITEM_INVENTORY_COLUMN> InvenItems;
	InvenItems.MemSetAll();
	INT32 lItemCount = 0;
	
	for (INT32 r = 0; r < pcsItemADChar->m_csInventoryGrid.m_nRow; r++)
	{
		for(int c = 0; c < pcsItemADChar->m_csInventoryGrid.m_nColumn; c++)
		{
			AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItem(&pcsItemADChar->m_csInventoryGrid, lLayer, r, c);
			if(pcsGridItem)
			{
				AgpdItem* pcsItem = m_pagpmItem->GetItem(pcsGridItem);
				if(pcsItem && pcsItem->m_pcsItemTemplate && m_pagpmItem->IsNotBound(pcsItem))
				{
					if (pbExist)
					{
						*pbExist = TRUE;
						return NULL;
					}
					
					InvenItems[lItemCount++] = pcsItem;
				}
			}
		}
	}

	if (pbExist)
	{
		*pbExist = FALSE;
		return NULL;
	}

	if (lItemCount > 0)
	{
		INT32 lProb = m_csRandom.randInt(lItemCount - 1);
		pcsReturnItem = InvenItems[lProb];
	}
	
	return pcsReturnItem;
}

// pcsDeadCharacter 를 죽인 파티 멤버중 가장 데미지를 많이 준놈을 리턴한다.
AgpdCharacter* AgsmPvP::GetMostAttackerInParty(AgpdParty* pcsParty, AgpdCharacter* pcsDeadCharacter)
{
	if(!pcsParty || !pcsDeadCharacter)
		return NULL;

	AgsdCharacterHistoryEntry* pcsEntry = m_pagsmCharacter->GetMostDamagerInParty(pcsDeadCharacter, pcsParty->m_lID);
	if(!pcsEntry)
		return NULL;

	AgpdCharacter* pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
    return pcsMostDamager;
}

// 누가 누구에게 죽었다는 것을 죽은놈의 길드원들에게 알린다.
// pcsCharacter 가 이긴놈. pcsTarget 이 죽은놈.
INT32 AgsmPvP::NotifyDeadToFriendGuildMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return 0;

	if(!m_pagpmCharacter->IsPC(pcsCharacter) || !m_pagpmCharacter->IsPC(pcsTarget))
		return 0;

	// 죽은 놈이 길드원일 때만 한다.
	if(!m_pagpmGuild->IsAnyGuildMember(pcsTarget))
		return 0;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsTarget);
	if(!pcsAttachedPvP)
		return 0;

	list<INT32> cMemberIDList;
	m_pagpmGuild->GetMemberCIDList(pcsTarget, &cMemberIDList);	// 먼저 자기 길드원들을 가져온다.

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	INT32 /*lIndex = 0,*/ lCount = 0;

	// 같은 편 길드의 멤버 아이디를 얻어온다.
	PvPGuildIter iter = pcsAttachedPvP->m_pFriendGuildVector->begin();
	while(iter != pcsAttachedPvP->m_pFriendGuildVector->end())
	{
		m_pagpmGuild->GetMemberCIDList(iter->m_szGuildID, &cMemberIDList);
		++iter;
	}

	//for(pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex))
	//{
	//	m_pagpmGuild->GetMemberCIDList(pcsPvPGuildInfo->m_szGuildID, &cMemberIDList);
	//}

	AgpdCharacter* pcsFriend = NULL;
	list<INT32>::iterator iterData = cMemberIDList.begin();
	while(iterData != cMemberIDList.end())
	{
		if(pcsTarget->m_lID == *iterData)
		{
			++iterData;
			continue;
		}

		pcsFriend = m_pagpmCharacter->GetCharacterLock(*iterData);
		if(!pcsFriend)
		{
			++iterData;
			continue;
		}

		//if(m_pagpmPvP->IsCombatPvPMode(pcsFriend))
		//{
			SendSystemMessage(AGPMPVP_SYSTEM_CODE_DEAD_GUILD_MEMBER, m_pagsmCharacter->GetCharDPNID(pcsFriend), pcsTarget->m_szID, pcsCharacter->m_szID);
		//}

		pcsFriend->m_Mutex.Release();
		++iterData;
	}

	return lCount;
}

// 누가 누구에게 죽었다는 것을 죽은놈의 파티원들에게 알린다.
// pcsCharacter 가 이긴놈. pcsTarget 이 죽은놈.
INT32 AgsmPvP::NotifyDeadToPartyMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return 0;

	if(!m_pagpmCharacter->IsPC(pcsCharacter) || !m_pagpmCharacter->IsPC(pcsTarget))
		return 0;

	AgpdPartyADChar* pcsPartyADChar = m_pagpmParty->GetADCharacter(pcsTarget);
	if(!pcsPartyADChar || pcsPartyADChar->lPID == AP_INVALID_PARTYID)
		return 0;

	AgpdParty* pcsParty = m_pagpmParty->GetPartyLock(pcsPartyADChar->lPID);
	if(!pcsParty)
		return 0;

	AgpdCharacter* pcsMember = NULL;
	INT32 lCount = 0;
	for(int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		if(pcsParty->m_lMemberListID[i] == pcsTarget->m_lID)
			continue;

		pcsMember = m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if(!pcsMember)
			continue;

		if(m_pagpmPvP->IsCombatPvPMode(pcsMember))
		{
			SendSystemMessage(AGPMPVP_SYSTEM_CODE_DEAD_PARTY_MEMBER, m_pagsmCharacter->GetCharDPNID(pcsMember), pcsTarget->m_szID,
							pcsCharacter->m_szID);
		}

		pcsMember->m_Mutex.Release();
	}

	pcsParty->m_Mutex.Release();
	
	return lCount;
}

// AgsmGuild 에서 불리고 szCharID 의 길드 탈퇴를 모두 처리한 후에 불린다.
// 전투 지역에서 길드를 탈퇴하면, 같은 편에서 해당이름의 길드를 없애준다.
// 만약 캐릭터가 파티도 맺고 있다면 (이 파티는 해당이름의 길드원들밖에 되지 않는다.)
// 파티도 해제 시켜준다.
BOOL AgsmPvP::ProcessLeaveGuild(CHAR* szGuildID, CHAR* szCharID)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	// 해당 캐릭터를 얻는다.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szCharID);
	if(!pcsCharacter)
		return FALSE;

	// 해당 캐릭터가 CombatPvP 일 때만 한다.
	if(!m_pagpmPvP->IsCombatPvPMode(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return TRUE;
	}

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
	{
		pcsCharacter->m_Mutex.Release();
		return TRUE;
	}

	// 캐릭터의 적+적길드 에게 캐릭터를 길드적에서 일반적으로 바꿔준다.
	ConvertEnemyGuildMeToEnemyMe(pcsCharacter, szGuildID);

	// 파티에 가입되어 있다면 빼준다.
	AgpdPartyADChar* pcsPartyADChar = m_pagpmParty->GetADCharacter(pcsCharacter);

	if(pcsPartyADChar && pcsPartyADChar->lPID != AP_INVALID_PARTYID)
	{
		AgpdParty* pcsParty = m_pagpmParty->GetPartyLock(pcsPartyADChar->lPID);
		if(pcsParty)
		{
			BOOL bDestroyParty = FALSE;

			m_pagpmParty->RemoveMember(pcsParty, pcsCharacter->m_lID, &bDestroyParty);

			if(!bDestroyParty)
				pcsParty->m_Mutex.Release();
		}
	}

	// 자기 길드를 빼준다.
	SendRemoveFriendGuild(pcsCharacter, szGuildID);

	if(!UpdatePvPStatus(pcsCharacter))
	{
		SendPvPInfo(pcsCharacter);
		SendPvPInfoToNear(pcsCharacter);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// AgpmParty 에서 불리고 캐릭터가 파티를 탈퇴한다. (강퇴포함)
// 같은편 중에서 파티인 사람을 빼주면 된다.
BOOL AgsmPvP::ProcessLeaveParty(AgpdCharacter* pcsCharacter, AgpdParty* pcsParty)
{
	if(!pcsCharacter || !pcsParty)
		return FALSE;

	// 위험 전투지역에서만 해주면 된다.
	if(!m_pagpmPvP->IsCombatPvPMode(pcsCharacter))
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;

	INT32 lCount = 0/*, lIndex = 0*/;
	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	AgpdCharacter* pcsFriend = NULL;
	AgpdPartyADChar	*pcsPartyAttachData	= NULL;

	// pcsCharacter 같은 편 중 PartyID 를 가진 놈을 뺀다.
	PvPCharVector vcTmp(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end());
	PvPCharIter iter = vcTmp.begin();
	while(iter != vcTmp.end())
	//for(pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex))
	{
		pcsFriend = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsFriend)
		{
			++iter;
			continue;
		}

		pcsPartyAttachData = m_pagpmParty->GetADCharacter((PVOID)pcsFriend);
        if(!pcsPartyAttachData)
		{
			pcsFriend->m_Mutex.Release();
			++iter;
			continue;
		}

		// 같은 파티원
		if(pcsPartyAttachData->lPID == pcsParty->m_lID)
		{
			if(m_pagpmPvP->RemoveFriend(pcsCharacter, pcsFriend->m_lID))
			{
				SendRemoveFriend(pcsCharacter, pcsFriend->m_lID);
				SendPvPInfo(pcsCharacter);
				SendPvPInfoToNear(pcsCharacter);
			}

			if(m_pagpmPvP->RemoveFriend(pcsFriend, pcsCharacter->m_lID))
			{
				SendRemoveFriend(pcsFriend, pcsCharacter->m_lID);
				SendPvPInfo(pcsFriend);
				SendPvPInfoToNear(pcsFriend);
			}

			++lCount;
		}

		pcsFriend->m_Mutex.Release();
		++iter;
	}

	return lCount > 0 ? TRUE : FALSE;
}

// 파티가 없어진다. 해당 파티원들은 서로 같은 편을 빼주면 된다.
BOOL AgsmPvP::ProcessDestroyParty(AgpdParty* pcsParty)
{
	if(!pcsParty)
		return FALSE;

	INT32 lFoundCID = 0;
	AgpdCharacter* pcsPartyMember = NULL;

	for(INT32 i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		pcsPartyMember = m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if(!pcsPartyMember)
			break;

		ProcessLeaveParty(pcsPartyMember, pcsParty);

		pcsPartyMember->m_Mutex.Release();
	}

	return TRUE;
}

// PvP Status 를 업데이트 한다.
// 변경시에만 패킷 보낸다.
BOOL AgsmPvP::UpdatePvPStatus(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!m_pagpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;

	INT8 cPrevStatus = pcsAttachedPvP->m_cPvPStatus;
	m_pagpmPvP->UpdatePvPStatus(pcsCharacter);
	INT8 cNowStatus = pcsAttachedPvP->m_cPvPStatus;

	BOOL bChanged = FALSE;

	// Status 가 바뀌었을 때만 처리해준다.
	// INVINCIBLE 걸리고 풀리는 건 따로 처리해준다.
	if(cPrevStatus != cNowStatus)
	{
		bChanged = TRUE;

		SendPvPInfo(pcsCharacter);
		SendPvPInfoToNear(pcsCharacter);

		UINT32 ulNID = (UINT32)m_pagsmCharacter->GetCharDPNID(pcsCharacter);

		if(!(cPrevStatus & AGPDPVP_STATUS_NOWPVP) && (cNowStatus & AGPDPVP_STATUS_NOWPVP))
		{
			SendSystemMessage(AGPMPVP_SYSTEM_CODE_NOW_PVP_STATUS, ulNID);
		}
		else if((cPrevStatus & AGPDPVP_STATUS_NOWPVP) && !(cNowStatus & AGPDPVP_STATUS_NOWPVP))
		{
			SendSystemMessage(AGPMPVP_SYSTEM_CODE_NONE_PVP_STATUS, ulNID);
		}
	}
    
	return bChanged;
}

// 무적 상태로 해준다.
BOOL AgsmPvP::SetInvincible(AgpdCharacter* pcsCharacter, UINT32 ulClockCount)
{
	if(!pcsCharacter)
		return FALSE;

	// PvP Mode 세팅
	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;
	
	if(ulClockCount == 0)
		ulClockCount = GetClockCount();

	pcsAttachedPvP->m_cPvPStatus |= AGPDPVP_STATUS_INVINCIBLE;		// 무적 상태로 세팅
	pcsAgsdCharacter->m_ulReleasePvPInvincibleTime = ulClockCount + AGPMPVP_INVINCIBLE_TIME;
	SetPvPIdleInterval(pcsCharacter);
	SendSystemMessage(AGPMPVP_SYSTEM_CODE_START_INVINCIBLE, pcsAgsdCharacter->m_dpnidCharacter);

	return TRUE;
}

// 무적 상태를 풀어준다.
BOOL AgsmPvP::UnsetInvincible(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(pcsAgsdCharacter && pcsAttachedPvP)
	{
		pcsAttachedPvP->m_cPvPStatus &= ~AGPDPVP_STATUS_INVINCIBLE;
		pcsAgsdCharacter->m_ulReleasePvPInvincibleTime = 0;
		SendSystemMessage(AGPMPVP_SYSTEM_CODE_END_INVINCIBLE, pcsAgsdCharacter->m_dpnidCharacter);

		SendPvPInfo(pcsCharacter);
		SendPvPInfoToNear(pcsCharacter);
	}

	return TRUE;
}

// 해당 캐릭터가 Idle 을 돌게 해준다.
BOOL AgsmPvP::SetPvPIdleInterval(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_PVP, AGSDCHAR_IDLE_INTERVAL_TEN_SECONDS);
	return TRUE;
}

// 인자로 넘어온 Item 을 pcsTarget 위치에 떨군다.
// 루팅 권한은 가장 많은 데미지를 준 놈에게 준다.
BOOL AgsmPvP::AddItemToField(AgpdCharacter* pcsDead, AgpdCharacter* pcsKiller, AgpdItem* pcsItem)
{
	if(!pcsDead || !pcsItem)
		return FALSE;

	// 죽은놈 위치에 떨군다.
	pcsItem->m_posItem = pcsDead->m_stPos;

	INT32 lRandomX = m_csRandom.randInt(100);
	INT32 lRandomZ = m_csRandom.randInt(100);

	if (lRandomX > 50)
		pcsItem->m_posItem.x += lRandomX;
	else
		pcsItem->m_posItem.x -= (lRandomX + 50);

	if (lRandomZ > 50)
		pcsItem->m_posItem.z += lRandomZ;
	else
		pcsItem->m_posItem.z -= (lRandomZ + 50);

	AgsdItem* pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
	if (pcsAgsdItem)
	{
		// 선루팅 없다.
		//pcsAgsdItem->m_csFirstLooterBase.m_eType = pcsKiller->m_eType;
		//pcsAgsdItem->m_csFirstLooterBase.m_lID = pcsKiller->m_lID;

		pcsAgsdItem->m_ulDropTime = GetClockCount();
	}

	m_pagpmItem->AddItemToField(pcsItem);

	return TRUE;
}



BOOL AgsmPvP::EnterGameWorld(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// 몬스터일 때는 나간다.
	if(!m_pagpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	// 가드일 때도 나간다.
	if(strlen(pcsCharacter->m_szID) == 0)
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;

	eAgpdPvPMode ePvPMode = m_pagpmPvP->GetPvPModeByPosition(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);
	pcsAttachedPvP->m_ePvPMode = ePvPMode;

	if(ePvPMode == AGPDPVP_MODE_FREE)
	{
		EnterFreePvPArea(pcsCharacter);
	}
	else if(ePvPMode == AGPDPVP_MODE_COMBAT)
	{
		EnterCombatPvPArea(pcsCharacter);
	}

	//if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
	//{
	//	eAGPMPVP_RACE_BATTLE_STATUS eStatus = m_pagpmPvP->GetRaceBattleStatus();
	//	switch (eStatus)
	//	{
	//		case AGPMPVP_RACE_BATTLE_STATUS_READY:
	//		case AGPMPVP_RACE_BATTLE_STATUS_ING:
	//		case AGPMPVP_RACE_BATTLE_STATUS_REWARD:
	//			SendRaceBattleInfo(pcsCharacter, (INT8) m_pagpmPvP->GetRaceBattleStatus());
	//			break;
	//		default:
	//			break;
	//	}

	//	if(eStatus == AGPMPVP_RACE_BATTLE_STATUS_REWARD)
	//	{
	//		if(m_pagpmPvP->GetRaceBattleWinner() == m_pagpmFactors->GetRace(&pcsCharacter->m_csFactor))
	//		{
	//			m_pagsmSkill->BuffRaceSkill(pcsCharacter);
	//		}
	//	}
	//}

	return TRUE;
}

BOOL AgsmPvP::Disconnect(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacterHistoryEntry* pcsEntry = m_pagsmCharacter->GetMostDamager(pcsCharacter);
	if(!pcsEntry)
		return TRUE;

	AgpdCharacter* pcsMostDamager = NULL;

	// 캐릭터라면
    if(pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
	{
		pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
	}
	else if(pcsEntry->m_csSource.m_eType == APBASE_TYPE_PARTY)
	{
		pcsEntry = m_pagsmCharacter->GetMostDamagerInParty(pcsCharacter, pcsEntry->m_csSource.m_lID);
		if(pcsEntry && pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
			pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
	}

	if(pcsMostDamager)
	{
		if (pcsMostDamager->m_Mutex.WLock())
			// 죽지 않았는데 디스커넥트로 인해서 죽은 처리 되면서 머더러 포인트가 올라갔었음. 2006.04.18. steeple
			// 그래서 디스커넥트로 죽은 것 처리할 때는 아래처럼 bDisconnect = TRUE 로 해준다.
			OnDead(pcsMostDamager, pcsCharacter, TRUE);
		else
			pcsMostDamager	= NULL;
	}

	// Dead 처리를 다 했다면 같은편, 적 들에게 캐릭터의 Disconnect 를 알린다.
	NotifyDisconnectToFriend(pcsCharacter);
	NotifyDisconnectToFriendGuildMembers(pcsCharacter);
	NotifyDisconnectToEnemy(pcsCharacter);
	NotifyDisconnectToEnemyGuildMembers(pcsCharacter, pcsMostDamager);

	if(pcsMostDamager)
		pcsMostDamager->m_Mutex.Release();

	return TRUE;
}

// Region 이 바뀔 때 불린다.
//BOOL AgsmPvP::RegionChange(AgpdCharacter* pcsCharacter, INT16 nPrevRegionIndex)
//{
//	if(!pcsCharacter)
//		return FALSE;
//
//	// 몬스터일 때는 나간다.
//	if(!m_pagpmCharacter->IsPC(pcsCharacter))
//		return TRUE;
//
//	eAgpdPvPMode ePrevPvPMode = m_pagpmPvP->GetPvPModeByRegion(nPrevRegionIndex);
//	eAgpdPvPMode eNowPvPMode = m_pagpmPvP->GetPvPModeByPosition(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);	
//	
//	// 자유지역에 들어갔음
//	if(eNowPvPMode == AGPDPVP_MODE_FREE && ePrevPvPMode != AGPDPVP_MODE_FREE)
//	{
//		EnterFreePvPArea(pcsCharacter);
//	}
//
//	// 자유지역에서 나왔음
//	if(eNowPvPMode != AGPDPVP_MODE_FREE && ePrevPvPMode == AGPDPVP_MODE_FREE)
//	{
//		LeaveFreePvPArea(pcsCharacter);
//	}
//
//	// 전투지역에 들어갔음
//	if(eNowPvPMode == AGPDPVP_MODE_COMBAT && ePrevPvPMode != AGPDPVP_MODE_COMBAT)
//	{
//		EnterCombatPvPArea(pcsCharacter);
//	}
//
//	// 전투지역에서 나왔음
//	if(eNowPvPMode != AGPDPVP_MODE_COMBAT && ePrevPvPMode == AGPDPVP_MODE_COMBAT)
//	{
//		LeaveCombatPvPArea(pcsCharacter);
//	}
//
//	return TRUE;
//}

BOOL AgsmPvP::EnterSafePvPArea(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// PvP Mode 세팅
	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;
	
	// 일단 자기편, 적 들에게 알린다.
	NotifyLeaveCombatAreaToFriend(pcsCharacter);
	NotifyLeaveCombatAreaToEnemy(pcsCharacter);

	//// 같은 편 리스트를 비우고
	m_pagpmPvP->InitFriendList(pcsCharacter);
	SendInitFriend(pcsCharacter);

	// 나갈 때 모조리 리셋하게 변경. 2005.01.29. steeple
	m_pagpmPvP->InitEnemyList(pcsCharacter);
	SendInitEnemy(pcsCharacter);

	// 같은 편 리스트도 추가로 비운다.
	m_pagpmPvP->InitFriendGuildList(pcsCharacter);
	SendInitFriendGuild(pcsCharacter);

	// 2005.04.18. steeple
	// 적길드를 비울 때는 주의. 길드전 중인 길드는 비우면 안된다.
	// AgpmPvP 모듈에서 처리해주자.
	m_pagpmPvP->InitEnemyGuildList(pcsCharacter);
	SendInitEnemyGuild(pcsCharacter);

	// 무적이었으면 해제해주고
	if(m_pagpmPvP->IsInvincible(pcsCharacter))
	{
		UnsetInvincible(pcsCharacter);
	}

	if(!UpdatePvPStatus(pcsCharacter))
	{
		SendPvPInfo(pcsCharacter);
		SendPvPInfoToNear(pcsCharacter);
	}

	return TRUE;
}

BOOL AgsmPvP::EnterFreePvPArea(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// PvP Mode 세팅
	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;

	// 자기 정보 업데이트 해주고
	//pcsAttachedPvP->m_ePvPMode = AGPDPVP_MODE_FREE;
	SendPvPInfo(pcsCharacter);

	// 주위 사람들에게 PvPStatus 를 뿌린다.
	//SendPvPInfoToNear(pcsCharacter);

	return TRUE;
}

BOOL AgsmPvP::EnterCombatPvPArea(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// PvP Mode 세팅
	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// 자기 정보 업데이트 해주고
	//pcsAttachedPvP->m_ePvPMode = AGPDPVP_MODE_COMBAT;
	pcsAttachedPvP->m_bDeadInCombatArea = FALSE;
	SetInvincible(pcsCharacter);

	//SendPvPInfo(pcsCharacter);

	// 같은 편 리스트만 비워야 한다.
	// 일반 적 리스트는 비우지 말자. - 2005.01.28. steeple
	// 다시 비우는 걸로 바뀜.... -_-;  2005.01.29. steeple
	m_pagpmPvP->InitFriendList(pcsCharacter);
	SendInitFriend(pcsCharacter);
	m_pagpmPvP->InitEnemyList(pcsCharacter);
	SendInitEnemy(pcsCharacter);

	// 길드 정보도 비우고
	m_pagpmPvP->InitFriendGuildList(pcsCharacter);
	SendInitFriendGuild(pcsCharacter);
	m_pagpmPvP->InitEnemyGuildList(pcsCharacter);
	SendInitEnemyGuild(pcsCharacter);

	AgpdCharacter* pcsSource = NULL;

	// 소속 길드가 있는 지 확인한다.
	// 아래에서 pcsSource 는 Lock 되어서 넘어오게 된다.
	if(m_pagpmGuild->IsAnyGuildMember(pcsCharacter))
	{
		pcsSource = GetOneGuildMemberInCombatArea(pcsCharacter);
	}
	else
	{
		pcsSource = GetOnePartyMemberInCombatArea(pcsCharacter);
		if(pcsSource)
		{
			// 해당 파티원부터 같은 편으로 넣어준다.
			if(m_pagpmPvP->AddFriend(pcsCharacter, pcsSource->m_lID))
				SendAddFriend(pcsCharacter, pcsSource->m_lID);
		}
	}

	// 길드 또는 파티원이라면, 먼저 들어가 있는 놈의 것을 그대로 복사한다.
	if(pcsSource)
	{
		CopyFriendEnemy(pcsSource, pcsCharacter);

		// 복사를 했으면 하나하나에 알린다.
		NotifyEnterCombatAreaToFriend(pcsCharacter);
		NotifyEnterCombatAreaToEnemy(pcsCharacter);

		pcsSource->m_Mutex.Release();
	}

	// 길드도 아니고 파티도 아니면 암것도 안해주면 된다.

	if(!UpdatePvPStatus(pcsCharacter))
	{
		// 만약에 Status 가 업데이트 안되더라도 자기 정보를 자신+주위에 뿌린다.
		SendPvPInfo(pcsCharacter);
		SendPvPInfoToNear(pcsCharacter);

		// Idle 도 돌려준다.
		SetPvPIdleInterval(pcsCharacter);
	}

	return TRUE;
}

BOOL AgsmPvP::LeaveFreePvPArea(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// PvP Mode 세팅
	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;

	// 자기 정보 업데이트 해주고
	//pcsAttachedPvP->m_ePvPMode = AGPDPVP_MODE_NONE;
	SendPvPInfo(pcsCharacter);

	// 주위 사람들에게 PvPStatus 를 뿌린다.
	SendPvPInfoToNear(pcsCharacter);

	return TRUE;
}

BOOL AgsmPvP::LeaveCombatPvPArea(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// PvP Mode 세팅
	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;
	
	// 일단 자기편, 적 들에게 알린다.
	NotifyLeaveCombatAreaToFriend(pcsCharacter);
	NotifyLeaveCombatAreaToEnemy(pcsCharacter);

	//// 같은 편 리스트를 비우고
	m_pagpmPvP->InitFriendList(pcsCharacter);
	SendInitFriend(pcsCharacter);

	// 나갈 때 모조리 리셋하게 변경. 2005.01.29. steeple
	m_pagpmPvP->InitEnemyList(pcsCharacter);
	SendInitEnemy(pcsCharacter);

	//// 죽어서 나갈때는 리셋하고
	//if(pcsAttachedPvP->m_bDeadInCombatArea)
	//{
	//	m_pagpmPvP->InitEnemyList(pcsCharacter);
	//	SendInitEnemy(pcsCharacter);
	//}
	//// 살아서 나갈때는 적을 그대로 유지해주어야 한다.
	//else
	//{
	//}

	// 같은 편 리스트도 추가로 비운다.
	m_pagpmPvP->InitFriendGuildList(pcsCharacter);
	SendInitFriendGuild(pcsCharacter);

	// 2005.04.18. steeple
	// 적길드를 비울 때는 주의. 길드전 중인 길드는 비우면 안된다.
	// AgpmPvP 모듈에서 처리해주자.
	m_pagpmPvP->InitEnemyGuildList(pcsCharacter);
	SendInitEnemyGuild(pcsCharacter);

	// 무적이었으면 해제해주고
	if(m_pagpmPvP->IsInvincible(pcsCharacter))
	{
		UnsetInvincible(pcsCharacter);
	}

	// 자기 정보 업데이트 해주고
	//pcsAttachedPvP->m_ePvPMode = AGPDPVP_MODE_NONE;

	if(!UpdatePvPStatus(pcsCharacter))
	{
		SendPvPInfo(pcsCharacter);
		SendPvPInfoToNear(pcsCharacter);
	}

	return TRUE;
}







BOOL AgsmPvP::SendPvPInfo(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	INT8 cPvPMode = (INT8)pcsAttachedPvP->m_ePvPMode;
	INT8 cPvPStatus = (INT8)pcsAttachedPvP->m_cPvPStatus;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakePvPInfoPacket(&nPacketLength, 
										&pcsCharacter->m_lID,
										&pcsAttachedPvP->m_lWin,
										&pcsAttachedPvP->m_lLose,
										&cPvPMode,
										&cPvPStatus);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// pcsCharacter 가 이긴놈. pcsTarget 진놈.
BOOL AgsmPvP::SendPvPResult(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvPTarget = m_pagpmPvP->GetADCharacter(pcsTarget);
	if(!pcsAttachedPvPTarget)
		return FALSE;

	// 이긴 놈 한테는 이렇게 보내고
	INT8 cWin = 1;
	INT8 cPvPMode = (INT8)pcsAttachedPvP->m_ePvPMode;
	INT8 cPvPStatus = (INT8)pcsAttachedPvP->m_cPvPStatus;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakePvPResultPacket(&nPacketLength,
										&pcsCharacter->m_lID,
										&pcsTarget->m_lID,
										&pcsAttachedPvP->m_lWin,
										&pcsAttachedPvP->m_lLose,
										&cPvPMode,
										&cPvPStatus,
										&cWin);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	// 진 놈한테는 이렇게 보내고
	cWin = 0;
	cPvPMode = (INT8)pcsAttachedPvPTarget->m_ePvPMode;
	cPvPStatus = (INT8)pcsAttachedPvPTarget->m_cPvPStatus;

	nPacketLength = 0;
	PVOID pvPacket2 = m_pagpmPvP->MakePvPResultPacket(&nPacketLength,
										&pcsTarget->m_lID,
										&pcsCharacter->m_lID,
										&pcsAttachedPvPTarget->m_lWin,
										&pcsAttachedPvPTarget->m_lLose,
										&cPvPMode,
										&cPvPStatus,
										&cWin);
	if(!pvPacket2)
		return FALSE;

	bResult = SendPacket(pvPacket2, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsTarget));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket2);

	return bResult;
}

// 한놈의 PvP정보를 주위에 뿌린다.
BOOL AgsmPvP::SendPvPInfoToNear(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	INT8 cPvPMode = (INT8)pcsAttachedPvP->m_ePvPMode;
	INT8 cPvPStatus = (INT8)pcsAttachedPvP->m_cPvPStatus;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakePvPInfoPacket(&nPacketLength, 
										&pcsCharacter->m_lID,
										&pcsAttachedPvP->m_lWin,
										&pcsAttachedPvP->m_lLose,
										&cPvPMode,
										&cPvPStatus);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = m_pagsmAOIFilter->SendPacketNearExceptSelf(pvPacket, nPacketLength, pcsCharacter->m_stPos,
										m_pagpmCharacter->GetRealRegionIndex(pcsCharacter),
										m_pagsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_6);
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 싸움 결과를 주위에 뿌린다.
// 이걸 굳이 뿌릴 필요는 없어서 현재(2004.12.28.) 부르는 곳은 없다.
BOOL AgsmPvP::SendPvPResultToNear(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvPTarget = m_pagpmPvP->GetADCharacter(pcsTarget);
	if(!pcsAttachedPvPTarget)
		return FALSE;

	// 이긴 놈 데이터를 보내고
	INT8 cWin = 1;
	INT8 cPvPMode = (INT8)pcsAttachedPvP->m_ePvPMode;
	INT8 cPvPStatus = (INT8)pcsAttachedPvP->m_cPvPStatus;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakePvPResultPacket(&nPacketLength,
										&pcsCharacter->m_lID,
										&pcsTarget->m_lID,
										&pcsAttachedPvP->m_lWin,
										&pcsAttachedPvP->m_lLose,
										&cPvPMode,
										&cPvPStatus,
										&cWin);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = m_pagsmAOIFilter->SendPacketNearExceptSelf(pvPacket, nPacketLength, pcsCharacter->m_stPos,
										m_pagpmCharacter->GetRealRegionIndex(pcsCharacter),
										m_pagsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_6);
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	// 진 놈 데이터를 보내고
	cWin = 0;
	cPvPMode = (INT8)pcsAttachedPvPTarget->m_ePvPMode;
	cPvPStatus = (INT8)pcsAttachedPvPTarget->m_cPvPStatus;

	nPacketLength = 0;
	PVOID pvPacket2 = m_pagpmPvP->MakePvPResultPacket(&nPacketLength,
										&pcsTarget->m_lID,
										&pcsCharacter->m_lID,
										&pcsAttachedPvPTarget->m_lWin,
										&pcsAttachedPvPTarget->m_lLose,
										&cPvPMode,
										&cPvPStatus,
										&cWin);
	if(!pvPacket2)
		return FALSE;

	bResult = m_pagsmAOIFilter->SendPacketNearExceptSelf(pvPacket2, nPacketLength, pcsTarget->m_stPos,
										m_pagpmCharacter->GetRealRegionIndex(pcsCharacter),
										m_pagsmCharacter->GetCharDPNID(pcsTarget), PACKET_PRIORITY_6);
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket2);

	return bResult;
}

BOOL AgsmPvP::SendAddFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeAddFriendPacket(&nPacketLength,
										&pcsCharacter->m_lID,
										&lFriendCID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendAddEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID, INT8 cNewPvPMode, UINT32 ulCombatClock)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeAddEnemyPacket(&nPacketLength,
										&pcsCharacter->m_lID,
										&lEnemyCID,
										cNewPvPMode == -1 ? NULL : &cNewPvPMode);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendRemoveFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeRemoveFriendPacket(&nPacketLength,
										&pcsCharacter->m_lID,
										&lFriendCID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendRemoveEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeRemoveEnemyPacket(&nPacketLength,
										&pcsCharacter->m_lID,
										&lEnemyCID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendInitFriend(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeInitFriendPacket(&nPacketLength, &pcsCharacter->m_lID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendInitEnemy(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeInitEnemyPacket(&nPacketLength, &pcsCharacter->m_lID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendUpdateFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeUpdateFriendPacket(&nPacketLength,
										&pcsCharacter->m_lID,
										&lFriendCID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendUpdateEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID, INT8 cNewPvPMode, UINT32 ulCombatClock)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeUpdateEnemyPacket(&nPacketLength,
										&pcsCharacter->m_lID,
										&lEnemyCID,
										cNewPvPMode == -1 ? NULL : &cNewPvPMode);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendAddFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeAddFriendGuildPacket(&nPacketLength, &pcsCharacter->m_lID, szGuildID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendAddEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT8 cNewPvPMode, UINT32 ulClockCount)
{
	if(!pcsCharacter || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeAddEnemyGuildPacket(&nPacketLength, &pcsCharacter->m_lID, szGuildID,
											cNewPvPMode == -1 ? NULL : &cNewPvPMode);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendRemoveFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeRemoveFriendGuildPacket(&nPacketLength, &pcsCharacter->m_lID, szGuildID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendRemoveEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeRemoveEnemyGuildPacket(&nPacketLength, &pcsCharacter->m_lID, szGuildID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendInitFriendGuild(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeInitFriendPacket(&nPacketLength, &pcsCharacter->m_lID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendInitEnemyGuild(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeInitEnemyGuildPacket(&nPacketLength, &pcsCharacter->m_lID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendUpdateFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeUpdateFriendGuildPacket(&nPacketLength, &pcsCharacter->m_lID, szGuildID);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendUpdateEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT8 cNewPvPMode, UINT32 ulClockCount)
{
	if(!pcsCharacter || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeUpdateEnemyGuildPacket(&nPacketLength, &pcsCharacter->m_lID, szGuildID,
											cNewPvPMode == -1 ? NULL : &cNewPvPMode);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendSystemMessage(INT32 lCode, UINT32 ulNID, CHAR* szData1, CHAR* szData2, INT32 lData1, INT32 lData2)
{
	if(lCode < 0 || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeSystemMessagePacket(&nPacketLength,
									&lCode,
									szData1,
									szData2,
									&lData1,
									&lData2
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendResponseDeadType(AgpdCharacter* pcsCharacter, INT32 lTargetCID, INT32 lDropExpRate, INT32 lTargetType)
{
	if(!pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeResponseDeadTypePacket(&nPacketLength, pcsCharacter->m_lID,
															lTargetCID, lDropExpRate, lTargetType);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::SendRaceBattleInfo(AgpdCharacter *pcsCharacter, INT8 cStatus)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmPvP->MakeRaceBattlePacket(&nPacketLength,
													  cStatus,
													  m_lRemained,
													  m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_HUMAN],
													  m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_ORC],
													  m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_MOONELF]
													  );
	if (!pvPacket)
		return FALSE;

	BOOL bResult = FALSE;
	if (!pcsCharacter)	// all user
		bResult = m_PacketSender.Push(pvPacket, nPacketLength);
	else
		bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	
	m_pagpmPvP->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
}

BOOL AgsmPvP::SetCallbackPvPDead(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMPVP_CB_PVPDEAD, pfCallback, pClass);
}


BOOL AgsmPvP::SetCallbackCheckCriminal(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMPVP_CB_CHECK_CRIMINAL, pfCallback, pClass);
}


BOOL AgsmPvP::CBEnterSafePvPArea(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	
	return pThis->EnterSafePvPArea(pcsCharacter);
}


BOOL AgsmPvP::CBEnterFreePvPArea(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;

	return pThis->EnterFreePvPArea(pcsCharacter);
}

BOOL AgsmPvP::CBEnterCombatPvPArea(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;

	return pThis->EnterCombatPvPArea(pcsCharacter);
}

BOOL AgsmPvP::CBLeaveFreePvPArea(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;

	return pThis->LeaveFreePvPArea(pcsCharacter);
}

BOOL AgsmPvP::CBLeaveCombatPvPArea(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;

	return pThis->LeaveCombatPvPArea(pcsCharacter);
}

BOOL AgsmPvP::CBProcessSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdPvPSystemMessage* pstSystemMessage = (AgpdPvPSystemMessage*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;

	AgpdCharacter* pcsCharacter = NULL;
	if(pCustData)
		pcsCharacter = (AgpdCharacter*)pCustData;

	return pThis->SendSystemMessage(pstSystemMessage->m_lCode,
									pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter),
									pstSystemMessage->m_aszData[0],
									pstSystemMessage->m_aszData[1],
									pstSystemMessage->m_alData[0],
									pstSystemMessage->m_alData[1]
									);
}

// 2005.07.28. steeple
BOOL AgsmPvP::CBRequestDeadType(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	INT32* plCID = static_cast<INT32*>(pData);
	AgsmPvP* pThis = static_cast<AgsmPvP*>(pClass);
	INT32* plResurrectionType = static_cast<INT32*>(pCustData);

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacterLock(*plCID);
	if(!pcsCharacter)
		return FALSE;

	AgsdDeath* pcsAgsdDeath = pThis->m_pagsmDeath->GetADCharacter(pcsCharacter);
	if(pcsAgsdDeath)
		pThis->SendResponseDeadType(pcsCharacter,
						pcsAgsdDeath->m_lDeadTargetCID,
						pThis->m_pagsmDeath->GetPenaltyExpRate(pcsCharacter,
												*plResurrectionType == AGPMPVP_RESURRECTION_TYPE_TOWN_REBIRTH ? TRUE : FALSE),
						pcsAgsdDeath->m_cDeadType);

	pcsCharacter->m_Mutex.Release();
	return TRUE;
}

BOOL AgsmPvP::CBResponseDeadType(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmPvP* pThis = (AgsmPvP*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	AgpdPvPADChar* pcsAttachedPvP = pThis->m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	AgsdDeath	*pcsAgsdDeath	= pThis->m_pagsmDeath->GetADCharacter(pcsCharacter);
	if(!pcsAgsdDeath)
		return FALSE;

	pcsAgsdDeath->m_cDeadType	= pcsAttachedPvP->m_cDeadType;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pagpmPvP->MakeResponseDeadTypePacket(&nPacketLength, pcsCharacter->m_lID,
															AP_INVALID_CID, 0, pcsAttachedPvP->m_cDeadType);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	pThis->m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

// 캐릭터가 Cell 을 이동할 때, 자기의 정보를 주위 사람들에게 뿌린다.
BOOL AgsmPvP::CBMoveCell(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmPvP* pThis = (AgsmPvP*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	if(!pThis->m_pagpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	pThis->SendPvPInfoToNear(pcsCharacter);
	return TRUE;
}

BOOL AgsmPvP::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;

	return pThis->EnterGameWorld(pcsCharacter);
}

BOOL AgsmPvP::CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;

	return pThis->Disconnect(pcsCharacter);
}

//BOOL AgsmPvP::CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if(!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
//	AgsmPvP* pThis = (AgsmPvP*)pClass;
//	INT16 nPrevRegionIndex = *(INT16*)pCustData;
//
//    return pThis->RegionChange(pcsCharacter, nPrevRegionIndex);
//}

BOOL AgsmPvP::CBUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	UINT32 ulClockCount = PtrToUint(pCustData);

	return pThis->UpdateCharacter(pcsCharacter, ulClockCount);
}

// 한 캐릭터가 이동할 때, 그 주위에 있는 캐릭터의 정보를 이동하는 캐릭터에게 보내준다.
// pcsCharacter 는 주위에 있는 캐릭터이고, 넘어온 NID 가 이동한 캐릭터이다.
// 즉, pcsCharacter 의 PvP 정보를 NID 로 보내는 것이다.
BOOL AgsmPvP::CBSendCharView(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	// PC 가 아니면 나간다.
	if(!pThis->m_pagpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = pThis->m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;
	
	UINT_PTR ulNID = UINT_PTR(ppvBuffer[0]);
	BOOL bGroup = PtrToInt(ppvBuffer[1]);
	BOOL bIsExceptSelf = PtrToInt(ppvBuffer[2]);

	INT8 cPvPMode = (INT8)pcsAttachedPvP->m_ePvPMode;
	INT8 cPvPStatus = (INT8)pcsAttachedPvP->m_cPvPStatus;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pagpmPvP->MakePvPInfoPacket(&nPacketLength, 
											&pcsCharacter->m_lID,
											&pcsAttachedPvP->m_lWin,
											&pcsAttachedPvP->m_lLose,
											&cPvPMode,
											&cPvPStatus);
	if(!pvPacket || nPacketLength < 1)
		return TRUE;

	BOOL bSendResult = FALSE;
	if(bGroup)
	{
		if(bIsExceptSelf)
		{
			bSendResult = pThis->m_pagsmAOIFilter->SendPacketGroupExceptSelf(pvPacket, nPacketLength, ulNID, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_4);
		}
		else
		{
			bSendResult = pThis->m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);
		}
	}
	else
		bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, (UINT32)ulNID, PACKET_PRIORITY_4);

	pThis->m_pagpmPvP->m_csPacket.FreePacket(pvPacket);
	
	return TRUE;
}

// 2007.01.31. steeple
// PvP 의 개인별 데이터를 Vector 로 변경했는데,
// 그것 때문에 Memory Pool 을 사용할 수 없으므로,
// Vector 의 Reserve 를 이용해서 메모리 확보를 한다.
BOOL AgsmPvP::CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmPvP* pThis = static_cast<AgsmPvP*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);

	// PC 가 아니라면 리턴
	if(!pThis->m_pagpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	// 가드라면 리턴
	if(_tcslen(pcsCharacter->m_szID) == 0)
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = pThis->m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	if(!pcsAttachedPvP->m_bInit)
		return TRUE;

	// PvP Vector Reserve
	pcsAttachedPvP->m_pFriendVector->reserve(AGPMPVP_MAX_FRIEND_COUNT);
	pcsAttachedPvP->m_pEnemyVector->reserve(AGPMPVP_MAX_ENEMY_COUNT);
	pcsAttachedPvP->m_pFriendGuildVector->reserve(AGPMPVP_MAX_FRIEND_COUNT);
	pcsAttachedPvP->m_pEnemyGuildVector->reserve(AGPMPVP_MAX_ENEMY_COUNT);

	return TRUE;
}

BOOL AgsmPvP::CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmPvP			*pThis			= (AgsmPvP *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	if (!pThis->m_pagpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = pThis->m_pagpmPvP->GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return TRUE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pagpmPvP->MakeResponseDeadTypePacket(&nPacketLength, pcsCharacter->m_lID,
															AP_INVALID_CID, 0, pcsAttachedPvP->m_cDeadType);
	if(!pvPacket)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, ulNID);
	pThis->m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPvP::CBOnAttack(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	stAgsmCombatAttackResult* pstAttackResult = (stAgsmCombatAttackResult*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;

    return pThis->OnAttack((AgpdCharacter*)pstAttackResult->pAttackChar, (AgpdCharacter*)pstAttackResult->pTargetChar);
}

BOOL AgsmPvP::CBOnSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	stAgsmCombatAttackResult* pstAttackResult = (stAgsmCombatAttackResult*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	AgpdSkill* pcsSkill = (AgpdSkill*)pCustData;

    return pThis->OnSkill(pcsSkill, (AgpdCharacter*)pstAttackResult->pAttackChar, (AgpdCharacter*)pstAttackResult->pTargetChar);
}

BOOL AgsmPvP::CBIsAttackable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;
	
	AgpdCharacter* pcsTarget = (AgpdCharacter*)ppvBuffer[0];
	BOOL bForce = PtrToInt(ppvBuffer[1]);

	return  pThis->IsAttackable(pcsCharacter, pcsTarget, bForce);
}

BOOL AgsmPvP::CBIsSkillEnable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdSkill* pcsSkill = (AgpdSkill*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	AgpdCharacter* pcsTarget = (AgpdCharacter*)pCustData;

	return pThis->IsSkillEnable(pcsSkill, pcsTarget);
}

BOOL AgsmPvP::CBIsUsableTeleport(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmPvP	*pThis	= (AgsmPvP *)	pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pData;
	PVOID *ppvBuffer	= (PVOID *) pCustData;

	BOOL	*pbIsUsableTeleport	= (BOOL *)	ppvBuffer[0];
	AgpdTeleportPoint	*pcsTeleportPoint	= (AgpdTeleportPoint *)	ppvBuffer[1];

	if (!pcsTeleportPoint ||
		!pbIsUsableTeleport)
		return FALSE;

	if (pcsTeleportPoint->m_eRegionType == AGPDTELEPORT_REGION_TYPE_PVP)
	{
		// check guild, party member
		//if (pThis->m_pagpmGuild->IsAnyGuildMember(pcsCharacter))
		//{
		//	AgpdPartyADChar	*pcsPartyAttachData	= pThis->m_pagpmParty->GetADCharacter((PVOID) pcsCharacter);
		//	if (pcsPartyAttachData->lPID != AP_INVALID_PARTYID)
		//	{
		//		*pbIsUsableTeleport	= FALSE;

		//		INT16	nPacketLength	= 0;
		//		PVOID	pvPacket		= pThis->m_pagpmPvP->MakeCannotUseTeleportPacket(&nPacketLength, pcsCharacter->m_lID);

		//		if (pvPacket && nPacketLength > 0)
		//		{
		//			pThis->m_pagpmPvP->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);
		//			pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_3);

		//			pThis->m_pagpmPvP->m_csPacket.FreePacket(pvPacket);
		//		}
		//	}
		//}

		INT32 lSystemMessageCode = -1;

		// Check Level
		INT32 lLevel = 0;
		pThis->m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		if(lLevel < AGPMPVP_BATTLE_SQAURE_REQUIRE_LEVEL)
		{
			lSystemMessageCode = AGPMPVP_SYSTEM_CODE_NOT_ENOUGH_LEVEL;
			*pbIsUsableTeleport = FALSE;
		}

		// 캐릭터가 길드에 가입되어 있다면
		CHAR* szGuildID = pThis->m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
		if(*pbIsUsableTeleport && szGuildID && strlen(szGuildID) > 0)
		{
			// 파티를 찾는다.
			AgpdPartyADChar	*pcsPartyAttachData	= pThis->m_pagpmParty->GetADCharacter((PVOID) pcsCharacter);
			if (pcsPartyAttachData && pcsPartyAttachData->lPID != AP_INVALID_PARTYID)
			{
				AgpdParty* pcsParty = pThis->m_pagpmParty->GetPartyLock(pcsPartyAttachData->lPID);
				if(pcsParty)
				{
					// 파티원중에서 같은 길드원이 아닌 사람을 찾는다.
					for(int i = 0; i < pcsParty->m_nCurrentMember; i++)
					{
						if (pcsParty->m_lMemberListID[i] == pcsCharacter->m_lID)
							continue;

						AgpdCharacter	*pcsMember	= pThis->m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
						if (!pcsMember)
							continue;

						CHAR* szPartyMemberGuildID = pThis->m_pagpmGuild->GetJoinedGuildID(pcsMember);
						if(!szPartyMemberGuildID || strlen(szPartyMemberGuildID) == 0 || strcmp(szGuildID, szPartyMemberGuildID) != 0)
						{
							*pbIsUsableTeleport = FALSE;
							lSystemMessageCode = AGPMPVP_SYSTEM_CODE_LEAVE_GUILD_OR_PARTY;
							break;
						}
					}

					pcsParty->m_Mutex.Release();
				}
				//*pbIsUsableTeleport	= FALSE;

			}	// Party Valid
		}	// szGuildID 

		if(!*pbIsUsableTeleport)
		{
			INT16	nPacketLength	= 0;
			PVOID	pvPacket		= pThis->m_pagpmPvP->MakeCannotUseTeleportPacket(&nPacketLength, pcsCharacter->m_lID);

			if (pvPacket && nPacketLength > 0)
			{
				pThis->m_pagpmPvP->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);
				pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_3);

				pThis->m_pagpmPvP->m_csPacket.FreePacket(pvPacket);

				if(lSystemMessageCode != -1)
					pThis->SendSystemMessage(lSystemMessageCode, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
			}
		}

	}	// TeleportPoint is PVP

	return TRUE;
}

// 2005.08.23. steeple 완전 바꼈음.
// OnDead 에서 MostDamager 로 넘겨주는 거 완전 쌩까고, 무조건 FinalAttacker 로 한다.
//
// 2005.04.22. steeple 추가 수정
// OnDead 에서 MostDamager 로 부르긴 하나 없을 때는 FinalAttacker 로 구해본다.
BOOL AgsmPvP::CBOnDead(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsDeadCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	ApBase* pcsAttackerBase = pCustData ? (ApBase*)pCustData : NULL;

	// final attacker based
	ApBase* pcsFinalAttacker = NULL;
	AgsdCharacterHistoryEntry* pcsEntry = pThis->m_pagsmCharacter->GetFinalAttacker(pcsDeadCharacter);
	if(pcsEntry)
	{
		pcsFinalAttacker = pThis->m_papmEventManager->GetBase(pcsEntry->m_csSource.m_eType, pcsEntry->m_csSource.m_lID);
		if(pcsFinalAttacker && pcsFinalAttacker != pcsAttackerBase)
		{
			if (!pcsFinalAttacker->m_Mutex.WLock())
				pcsFinalAttacker	= NULL;
		}
	}

	if(!pcsFinalAttacker)
	{
		// 2006.06.02. steeple 땜빵용추가
		pThis->ProcessExceptionNonExistFinalAttackerOnDead(pcsDeadCharacter);
		return TRUE;
	}

	// 2005.08.23. steeple
	// 이제 캐릭만 오게 된다. 파티 따윈 KIN 이다.
	// 캐릭터나 파티일 때만 한다.
	if(pcsFinalAttacker->m_eType == APBASE_TYPE_CHARACTER)
		pThis->OnDead((AgpdCharacter*)pcsFinalAttacker, pcsDeadCharacter);

	// 2005.08.23. steeple
	// FinalAttacker 와 pcsAttackerBase 가 다르다면 Lock 푼다.
	if(pcsFinalAttacker != pcsAttackerBase)
	{
		pcsFinalAttacker->m_Mutex.Release();
	}

	// send dead type
	//////////////////////////////////////////////////////////
	if (pThis->m_pagpmCharacter->IsPC(pcsDeadCharacter))
	{
		AgsdDeath* pcsAgsdDeath = pThis->m_pagsmDeath->GetADCharacter(pcsDeadCharacter);
		if(pcsAgsdDeath)
			pThis->SendResponseDeadType(pcsDeadCharacter,
							pcsAgsdDeath->m_lDeadTargetCID,
							0,
							pcsAgsdDeath->m_cDeadType);
	}

	return TRUE;
}

BOOL AgsmPvP::CBLeaveGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	CHAR* szCharID = (CHAR*)pCustData;

	pThis->ProcessLeaveGuild(szGuildID, szCharID);

	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmPvP::CBAddEnemyGuildByBattle(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	CHAR* szEnemyGuildID = (CHAR*)pCustData;

	return pThis->AddEnemyGuildByBattle(pcsCharacter, szEnemyGuildID);
}

// 2005.04.17. steeple
BOOL AgsmPvP::CBRemoveEnemyGuildByBattle(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	CHAR* szEnemyGuildID = (CHAR*)pCustData;

	return pThis->RemoveEnemyGuildByBattle(pcsCharacter, szEnemyGuildID);
}

BOOL AgsmPvP::CBAddEnemyPrivateGuildByBattle(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	AgpdCharacter* pcsEnemy = (AgpdCharacter*)pCustData;

	return pThis->AddEnemyToCharacter(pcsCharacter, pcsEnemy);
}

BOOL AgsmPvP::CBRemoveEnemyPrivateGuildByBattle(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	AgpdCharacter* pcsEnemy = (AgpdCharacter*)pCustData;

	return pThis->RemoveEnemyToCharacter( pcsCharacter, pcsEnemy );
}

BOOL AgsmPvP::CBLeaveParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdParty* pcsParty = (AgpdParty*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	pThis->ProcessLeaveParty(pcsCharacter, pcsParty);

	return TRUE;
}

BOOL AgsmPvP::CBDestroyParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdParty* pcsParty = (AgpdParty*)pData;
	AgsmPvP* pThis = (AgsmPvP*)pClass;

	pThis->ProcessDestroyParty(pcsParty);

	return TRUE;
}


BOOL AgsmPvP::StreamReadItemDropRate(CHAR *pszFile, BOOL bDecryption)
	{
	if (!pszFile || !strlen(pszFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;
	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
		return FALSE;

	m_lEquipDropRate[AGSMPVP_DROP_EQUIP_PART_HEAD]		=	16;
	m_lEquipDropRate[AGSMPVP_DROP_EQUIP_PART_BREAST]	=	25;
	m_lEquipDropRate[AGSMPVP_DROP_EQUIP_PART_LEG]		=	21;
	m_lEquipDropRate[AGSMPVP_DROP_EQUIP_PART_SLEEVE]	=	14;
	m_lEquipDropRate[AGSMPVP_DROP_EQUIP_PART_FOOT]		=	13;
	m_lEquipDropRate[AGSMPVP_DROP_EQUIP_PART_SHIELD]	=	20;
	m_lEquipDropRate[AGSMPVP_DROP_EQUIP_PART_WEAPON]	=	10;

	m_lBagDropRate[0]	= 25;
	m_lBagDropRate[1]	= 25;
	m_lBagDropRate[2]	= 25;
	m_lBagDropRate[3]	= 25;

	m_ItemDropRate.MemSetAll();
	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)
		{
		CHAR *psz = csExcelTxtLib.GetData(AGSMPVP_EXCEL_COLUM_TYPE, lRow);

		INT32 lType = -1;		
		for (INT32 l = 0; l < AGSMPVP_CHAR_STATUS_MAX; ++l)
			{
			if (0 == _tcsicmp(psz, s_szCharStatus[l]))
				{
				lType = l;
				break;
				}
			}
		
		if (-1 == lType)
			continue;
		
		// equip drop?
		psz = csExcelTxtLib.GetData(AGSMPVP_EXCEL_COLUM_EQUIP_DROP, lRow);
		if (NULL == psz)
			m_ItemDropRate[lType].m_lDropEquip = 0;
		else
			m_ItemDropRate[lType].m_lDropEquip = _ttoi(psz);
		
		// bag drop
		psz = csExcelTxtLib.GetData(AGSMPVP_EXCEL_COLUM_BAG_DROP, lRow);
		if (NULL == psz)
			m_ItemDropRate[lType].m_lDropBag = 0;
		else
			m_ItemDropRate[lType].m_lDropBag = _ttoi(psz);
	
		// rate per equip part
		for (INT32 lEquip = 0; lEquip < AGSMPVP_DROP_EQUIP_PART_MAX; ++lEquip)
			{
			psz = csExcelTxtLib.GetData(AGSMPVP_EXCEL_COLUM_EQUIP_DROP_RATE_START + lEquip, lRow);
			if (NULL == psz)
				(m_ItemDropRate[lType]).m_lRateEquip[lEquip] = 0;
			else
				(m_ItemDropRate[lType]).m_lRateEquip[lEquip] = _ttoi(psz);
			}			
		
		// rate per bag
		for (INT32 lBag = 0; lBag < AGPMITEM_MAX_INVENTORY; ++lBag)
			{
			psz = csExcelTxtLib.GetData(AGSMPVP_EXCEL_COLUM_EQUIP_BAG_RATE_START + lBag, lRow);
			if (NULL == psz)
				(m_ItemDropRate[lType]).m_lRateBag[lBag] = 0;
			else
				(m_ItemDropRate[lType]).m_lRateBag[lBag] = _ttoi(psz);			
			}
		}
	
	return TRUE;
	}

BOOL AgsmPvP::StreamReadCharismaPoint(CHAR *pszFile, BOOL bDecryption)
{
	if (!pszFile || !strlen(pszFile))
		return FALSE;

	m_lTotalCharismaPoint = 0;
	m_CharismaPoint.MemSetAll();

	AuExcelTxtLib	csExcelTxtLib;
	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
	{
		m_CharismaPoint[0].m_lDiff = 10;
		m_CharismaPoint[0].m_lNormalPoint = 50;
		m_CharismaPoint[0].m_lKillableCount = 4;
		m_CharismaPoint[1].m_lDiff = 1;
		m_CharismaPoint[1].m_lNormalPoint = 20;
		m_CharismaPoint[1].m_lKillableCount = 3;
		m_CharismaPoint[2].m_lDiff = 0;
		m_CharismaPoint[2].m_lNormalPoint = 10;
		m_CharismaPoint[0].m_lKillableCount = 2;
		m_CharismaPoint[3].m_lDiff = -9;
		m_CharismaPoint[3].m_lNormalPoint = 5;	
		m_CharismaPoint[0].m_lKillableCount = 1;
		m_lTotalCharismaPoint = 4;
		return TRUE;
	}

	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)
	{
		CHAR *pszLevelDiff = csExcelTxtLib.GetData(0, lRow);
		CHAR *pszNormalPoint = csExcelTxtLib.GetData(1, lRow);
		CHAR *pszRaceBattlePoint = csExcelTxtLib.GetData(2, lRow);
		CHAR *pszKillableCount = csExcelTxtLib.GetData(3, lRow);
		
		if (!pszLevelDiff || !pszNormalPoint || !pszRaceBattlePoint || !pszKillableCount)
			continue;

		INT32 lLevelDiff = atoi(pszLevelDiff);
		INT32 lNormalPoint = atoi(pszNormalPoint);
		INT32 lRaceBattlePoint = atoi(pszRaceBattlePoint);
		INT32 lKillableCount = atoi(pszKillableCount);
		
		m_CharismaPoint[m_lTotalCharismaPoint].m_lDiff = lLevelDiff;
		m_CharismaPoint[m_lTotalCharismaPoint].m_lNormalPoint = lNormalPoint;
		m_CharismaPoint[m_lTotalCharismaPoint].m_lRaceBattlePoint = lRaceBattlePoint;
		m_CharismaPoint[m_lTotalCharismaPoint].m_lKillableCount = lKillableCount;
		m_lTotalCharismaPoint++;
	}
	
	return TRUE;
}


BOOL AgsmPvP::WriteDeathLog(AgpdCharacter *pAgpdCharacterAttack, AgpdCharacter *pAgpdCharacterDead, INT8 cDeadType)
	{
	if (!m_pagpmLog)
		return FALSE;
	
	AgsdCharacter *pAgsdCharacterDead = m_pagsmCharacter->GetADCharacter(pAgpdCharacterDead);
	if (!pAgsdCharacterDead)
		return FALSE;
		
	INT32 lLevel = m_pagpmCharacter->GetLevel(pAgpdCharacterDead);
	INT64 llExp = m_pagpmCharacter->GetExp(pAgpdCharacterDead);

	CHAR szEnemy[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	CHAR szDesc[128];
	CHAR szPosition[33];
	ZeroMemory(szEnemy, sizeof(szEnemy));
	ZeroMemory(szDesc, sizeof(szDesc));
	ZeroMemory(szPosition, sizeof(szPosition));
	
	m_pagsmCharacter->EncodingPosition(&pAgpdCharacterDead->m_stPos, szPosition, 32);

	switch (cDeadType)
		{
		case AGPMPVP_TARGET_TYPE_MOB :
			_tcscpy(szEnemy, pAgpdCharacterAttack->m_pcsCharacterTemplate->m_szTName);
			_stprintf(szDesc, _T("Monster %s"), szPosition);
			break;			

		case AGPMPVP_TARGET_TYPE_NORMAL_PC :
			_tcscpy(szEnemy, pAgpdCharacterAttack->m_szID);
			_stprintf(szDesc, _T("Player  %s"), szPosition);
			break;			

		case AGPMPVP_TARGET_TYPE_ENEMY_GUILD :
			_tcscpy(szEnemy, pAgpdCharacterAttack->m_szID);
			_stprintf(szDesc, _T("Guild  %s"), szPosition);
			break;
		}

	m_pagpmLog->WriteLog_CharDeath(0, &pAgsdCharacterDead->m_strIPAddress[0],
										pAgsdCharacterDead->m_szAccountID,
										pAgsdCharacterDead->m_szServerName,
										pAgpdCharacterDead->m_szID,
										pAgpdCharacterDead->m_pcsCharacterTemplate->m_lID,
										lLevel,
										llExp,
										pAgpdCharacterDead->m_llMoney,
										pAgpdCharacterDead->m_llBankMoney,
										szEnemy,
										szDesc,
										m_pagpmGuild->GetJoinedGuildID(pAgpdCharacterDead)
										);

	return TRUE;
}

BOOL AgsmPvP::WriteKillLog(AgpdCharacter *pAgpdCharacterAttack, AgpdCharacter *pAgpdCharacterDead, INT8 cDeadType)
	{
	if (!m_pagpmLog)
		return FALSE;
	
	AgsdCharacter *pAgsdCharacterAttack = m_pagsmCharacter->GetADCharacter(pAgpdCharacterAttack);
	if (!pAgsdCharacterAttack)
		return FALSE;
		
	INT32 lLevel = m_pagpmCharacter->GetLevel(pAgpdCharacterAttack);
	INT64 llExp = m_pagpmCharacter->GetExp(pAgpdCharacterAttack);

	CHAR szEnemy[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	CHAR szDesc[128];
	ZeroMemory(szEnemy, sizeof(szEnemy));
	ZeroMemory(szDesc, sizeof(szDesc));

	switch (cDeadType)
		{
		case AGPMPVP_TARGET_TYPE_NORMAL_PC :
			_tcscpy(szEnemy, pAgpdCharacterDead->m_szID);
			_stprintf(szDesc, _T("Player (%d,%d,%d)"), pAgpdCharacterAttack->m_stPos.x,
													   pAgpdCharacterAttack->m_stPos.y,
													   pAgpdCharacterAttack->m_stPos.z);
			break;			
		case AGPMPVP_TARGET_TYPE_ENEMY_GUILD :
			_tcscpy(szEnemy, pAgpdCharacterDead->m_szID);
			_stprintf(szDesc, _T("Guild (%d,%d,%d)"), pAgpdCharacterAttack->m_stPos.x,
													  pAgpdCharacterAttack->m_stPos.y,
													  pAgpdCharacterAttack->m_stPos.z);
		
		default :
			return FALSE;
			break;
		}

	m_pagpmLog->WriteLog_CharKill(0, &pAgsdCharacterAttack->m_strIPAddress[0],
										pAgsdCharacterAttack->m_szAccountID,
										pAgsdCharacterAttack->m_szServerName,
										pAgpdCharacterAttack->m_szID,
										pAgpdCharacterAttack->m_pcsCharacterTemplate->m_lID,
										lLevel,
										llExp,
										pAgpdCharacterAttack->m_llMoney,
										pAgpdCharacterAttack->m_llBankMoney,
										szEnemy,
										szDesc,
										m_pagpmGuild->GetJoinedGuildID(pAgpdCharacterAttack)
										);

	return TRUE;
	}




//	Race Battle
//==============================================
//
BOOL AgsmPvP::StreamReadRaceBattleStatus(CHAR *pszFile)
	{
	BOOL		bResult = FALSE;
	eAGPMPVP_RACE_BATTLE_STATUS eReadStatus = AGPMPVP_RACE_BATTLE_STATUS_NONE;
	ApModuleStream		csStream;
	UINT16				nNumKeys;

	if (NULL == pszFile || _T('\0') == *pszFile)
		return FALSE;

	strcpy(m_szIniFile, pszFile);

	if (!csStream.Open(pszFile))
		return FALSE;

	nNumKeys = csStream.GetNumSections();
	for (INT32 i = 0; i < nNumKeys; i++)
		{
		const CHAR *pszSection = csStream.ReadSectionName(i);
		
		if (!pszSection || 0 != _tcsicmp(pszSection, AGSMPVP_INI_SECTION_NAME_RACEBATTLE))
			continue;
		
		bResult = TRUE;
		INT32 lNumValue = 0;
		while (csStream.ReadNextValue())
			{
			const CHAR *pszName = csStream.GetValueName();

			// status
			if (0 == _tcsicmp(pszName, AGSMPVP_INI_RACE_BATTLE_STATUS))
				{
				if (csStream.GetValue(&lNumValue))
					eReadStatus = (eAGPMPVP_RACE_BATTLE_STATUS) lNumValue;
				}
			else if (0 == _tcsicmp(pszName, AGSMPVP_INI_RACE_BATTLE_HUMAN_SCORE))
				{
				if (csStream.GetValue(&lNumValue))
					m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_HUMAN] = lNumValue;
				}
			else if (0 == _tcsicmp(pszName, AGSMPVP_INI_RACE_BATTLE_ORC_SCORE))
				{
				if (csStream.GetValue(&lNumValue))
					m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_ORC] = lNumValue;
				}
			else if (0 == _tcsicmp(pszName, AGSMPVP_INI_RACE_BATTLE_MOONELF_SCORE))
				{
				if (csStream.GetValue(&lNumValue))
					m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_MOONELF] = lNumValue;
				}
			}
		}	

	return TRUE;
	}


BOOL AgsmPvP::StreamReadRaceBattleTime(CHAR *pszFile, BOOL bDecryption)
	{
	// 못읽어도 TRUE
	if (!pszFile || !strlen(pszFile))
		return TRUE;

	AuExcelTxtLib csExcelTxtLib;
	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
		return TRUE;

	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)
		{
		CHAR *pszStatus = csExcelTxtLib.GetData(0, lRow);
		if (!pszStatus || 0 >= strlen(pszStatus))
			continue;
		
		CHAR *pszTime = csExcelTxtLib.GetData(1, lRow);
		if (!pszTime || 0 >= strlen(pszTime))
			continue;
		
		UINT32 ulHour, ulMin;
		
		if (2 != sscanf(pszTime, "%2d:%2d", &ulHour, &ulMin))
			continue;
		
		UINT32 ulTime = ulHour * 60 * 60 + ulMin * 60;
		for (INT32 i = 0; i < AGPMPVP_RACE_BATTLE_STATUS_MAX; i++)
			{
			if (0 == stricmp(pszStatus, s_szRaceBattleStatus[i]))
				{
				m_ulRaceBattleTimeOfSec[i] = ulTime;
				break;
				}
			}
		}
	
	return TRUE;
	}


BOOL AgsmPvP::StreamWriteRaceBattleStatus()
	{
	if (0 >= strlen(m_szIniFile))
		return FALSE;
	
	ApModuleStream		csStream;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	csStream.SetSection(AGSMPVP_INI_SECTION_NAME_RACEBATTLE);
	
	csStream.WriteValue(AGSMPVP_INI_RACE_BATTLE_STATUS, (INT32) m_pagpmPvP->GetRaceBattleStatus());
	csStream.WriteValue(AGSMPVP_INI_RACE_BATTLE_HUMAN_SCORE, (INT32) m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_HUMAN]);
	csStream.WriteValue(AGSMPVP_INI_RACE_BATTLE_ORC_SCORE, (INT32) m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_ORC]);
	csStream.WriteValue(AGSMPVP_INI_RACE_BATTLE_MOONELF_SCORE, (INT32) m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_MOONELF]);
	
	return csStream.Write(m_szIniFile);
	}


void AgsmPvP::WriteLogRaceBattleResult()
	{
	SYSTEMTIME st;
	CHAR szLog[MAX_PATH];
	GetLocalTime(&st);
	sprintf(szLog, "LOG\\Wars_%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "Human Point = %d\nOrc Point = %d\nMoolelf Point = %d",
						m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_HUMAN],
						m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_ORC],
						m_pagpmPvP->m_lRaceBattlePoint[AURACE_TYPE_MOONELF]);
	AuLogFile_s(szLog, strCharBuff);
	}


void AgsmPvP::ProcessRaceBattle()
	{
	BOOL	bNotify = FALSE;
	time_t	CurrentTime;
	tm		*pCurrentLocalTime;
		
	time(&CurrentTime);
	pCurrentLocalTime = localtime(&CurrentTime);
	if (!pCurrentLocalTime)
		return;

	UINT32 ulSec = 60 * 60 * pCurrentLocalTime->tm_hour + 60 * pCurrentLocalTime->tm_min + pCurrentLocalTime->tm_sec;
	eAGPMPVP_RACE_BATTLE_STATUS ePrevStatus = m_pagpmPvP->GetRaceBattleStatus();
	
	if (ulSec >= m_ulRaceBattleTimeOfSec[AGPMPVP_RACE_BATTLE_STATUS_REWARD_END])
		{

		INT32 lRace = m_pagpmPvP->GetRaceBattleWinner();

		// None
		m_pagpmPvP->SetRaceBattleStatus(AGPMPVP_RACE_BATTLE_STATUS_NONE);
		m_lRemained = 0;
		if (AGPMPVP_RACE_BATTLE_STATUS_REWARD == ePrevStatus)
			{
			bNotify = TRUE;

			// 버프 시켜주었던 스킬을 없앤다. 2006.12.18. steeple
			m_pagsmSkill->EndRaceSkill(lRace);
			}
		}
	else if (ulSec >= m_ulRaceBattleTimeOfSec[AGPMPVP_RACE_BATTLE_STATUS_REWARD])
		{
		// Reward
		m_pagpmPvP->SetRaceBattleStatus(AGPMPVP_RACE_BATTLE_STATUS_REWARD);
		if (AGPMPVP_RACE_BATTLE_STATUS_ING == ePrevStatus)
			{
			m_pagpmPvP->SetRaceBattleResult();
			m_ulTickCount = 0;
			WriteLogRaceBattleResult();

			// 스킬 버프 시켜준다. 2006.12.18. steeple
			m_pagsmSkill->BuffRaceSkill(m_pagpmPvP->GetRaceBattleWinner());
			}
		
		m_lRemained = m_ulRaceBattleTimeOfSec[AGPMPVP_RACE_BATTLE_STATUS_REWARD_END] - ulSec;
		if (0 == m_ulTickCount % 60)
			bNotify = TRUE;
		}
	else if (ulSec >= m_ulRaceBattleTimeOfSec[AGPMPVP_RACE_BATTLE_STATUS_ING])
		{
		// Ing
		m_pagpmPvP->SetRaceBattleStatus(AGPMPVP_RACE_BATTLE_STATUS_ING);
		if (AGPMPVP_RACE_BATTLE_STATUS_READY == ePrevStatus)
			{
			m_pagpmPvP->m_lRaceBattlePoint.MemSetAll();
			m_ulTickCount = 0;
			}

		m_lRemained = m_ulRaceBattleTimeOfSec[AGPMPVP_RACE_BATTLE_STATUS_REWARD] - ulSec;
		if (0 == m_ulTickCount % 60)
			bNotify = TRUE;
		}
	else if (ulSec >= m_ulRaceBattleTimeOfSec[AGPMPVP_RACE_BATTLE_STATUS_READY])
		{
		// Ready
		m_pagpmPvP->SetRaceBattleStatus(AGPMPVP_RACE_BATTLE_STATUS_READY);
		if (AGPMPVP_RACE_BATTLE_STATUS_NONE == ePrevStatus)
			m_ulTickCount = 0;
		
		m_lRemained = m_ulRaceBattleTimeOfSec[AGPMPVP_RACE_BATTLE_STATUS_ING] - ulSec;			
		if (0 == m_ulTickCount % 30)
			bNotify = TRUE;
		}
	else
		{
		// None
		m_lRemained = 0;
		m_pagpmPvP->SetRaceBattleStatus(AGPMPVP_RACE_BATTLE_STATUS_NONE);
		}

	if (bNotify)
		{
		INT8	cStatus = (INT8) m_pagpmPvP->GetRaceBattleStatus();
		if (AGPMPVP_RACE_BATTLE_STATUS_REWARD == m_pagpmPvP->GetRaceBattleStatus() &&
			AGPMPVP_RACE_BATTLE_STATUS_ING == ePrevStatus)
			cStatus = (INT8) AGPMPVP_RACE_BATTLE_STATUS_ENDED;
		else
			cStatus = (INT8) m_pagpmPvP->GetRaceBattleStatus();

		SendRaceBattleInfo(NULL, cStatus);
		StreamWriteRaceBattleStatus();
		}
		
	m_ulTickCount++;
	}


BOOL AgsmPvP::IsEnableCharismaPoint(CHAR *pszCharID, CHAR *pszKilledCharID, INT32 lDayOfYear, INT32 lCount)
	{
	if (0 > lDayOfYear)
		return FALSE;
	
	AuAutoLock Lock(m_KilledCharMapLock);
	if (!Lock.Result())
		return FALSE;
	
	KilledCharMapIter Iter;
	Iter = m_KilledCharMap.find(pszCharID);
	if (Iter == m_KilledCharMap.end())
		{
		KilledCharInfoMap	InfoMap;
		m_KilledCharMap.insert(KilledCharMapPair(pszCharID, InfoMap));
		Iter = m_KilledCharMap.find(pszCharID);
		}

	KilledCharInfoMapIter Iter2 = Iter->second.find(pszKilledCharID);
	if (Iter2 == Iter->second.end())
		{
		AgsdPvPKilledChar	CharInfo;
		Iter->second.insert(KilledCharInfoMapPair(pszKilledCharID, CharInfo));
		Iter2 = Iter->second.find(pszKilledCharID);
		}
	
	if (lCount >= Iter2->second.AddKilledCount(lDayOfYear))
		return TRUE;
	
	return FALSE;
	}




/****************************************************/
/*		The Implementation of Relay Sender Thread	*/
/****************************************************/
//
RaceBattlePacketSender::RaceBattlePacketSender()
	{
	m_pAgsmAOIFilter	= NULL;
	m_bStop				= TRUE;
	m_bInit				= FALSE;
	m_csLock.Init();
	}


RaceBattlePacketSender::~RaceBattlePacketSender()
	{
	m_bInit	= FALSE;
	m_csLock.Destroy();
	}




//	zzThread inherited
//====================================================
//
BOOL RaceBattlePacketSender::OnCreate()
	{
	m_bStop = FALSE;
	return TRUE;
	}


void RaceBattlePacketSender::OnTerminate()
	{
	// 보내고 있음 좀 기다린다.
	// 남은 놈들 다 보낸다.
	if (!Send())
		{
		// 에러에 의해 전송이 불가능한 경우다.
		AuAutoLock Lock(m_csLock);
		if (!Lock.Result()) return;

		INT32 lRemain = (INT32)m_QueueItem.size();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Remained Queue Size=[%d] in RaceBattlePacketSender::OnTerminate()\n",
				  lRemain);
		AuLogFile_s("LOG\\RaceBattleError.log", strCharBuff);
		}	
	}


INT32 g_lRaceBattleSenderSleep = 10;
DWORD RaceBattlePacketSender::Do()
	{
	while (!m_bStop)
		{
		if (!m_bInit)
			return 12;

		DWORD dwWait = WaitForSingleObject(m_hSendEvent, INFINITE);
		if (WAIT_OBJECT_0 == dwWait)
			Send();

		Sleep(g_lRaceBattleSenderSleep);
		}

	return 0;
	}




//	Internal
//=====================================================
//
void RaceBattlePacketSender::Remove(PVOID pvPacket)
	{
	m_QueueItem.pop_front();
	m_QueueLength.pop_front();
	}


PVOID RaceBattlePacketSender::Get(INT16 &nLength)
	{
	if (!m_bInit || m_QueueItem.size() <= 0)
		{
		nLength = 0;
		return NULL;
		}
	
	ASSERT(m_QueueItem.size() == m_QueueLength.size());

	nLength = m_QueueLength.front();
	return 	m_QueueItem.front();
	}


BOOL RaceBattlePacketSender::Send()
	{
	AuAutoLock Lock(m_csLock);
	if (!Lock.Result()) return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = NULL;	
	INT32 lRemain = (INT32)m_QueueItem.size();
	for (INT32 lSended = 0; lSended < lRemain; ++lSended)
		{
		pvPacket = Get(nPacketLength);
		if (!pvPacket || 1 > nPacketLength)
			{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "Invalid Pakcet of Size[%d] in Send()\n", nPacketLength);
			AuLogFile_s("LOG\\RaceBattleError.log", strCharBuff);

			Remove(pvPacket);
			continue;
			}

		if (!m_pAgsmAOIFilter->SendPacketAllGroups(pvPacket, nPacketLength, PACKET_PRIORITY_2))
			{
			Remove(pvPacket);
			return FALSE;
			}

		Remove(pvPacket);
		}

	return TRUE;
	}


void RaceBattlePacketSender::Stop()
	{
	m_bStop = TRUE;
	}




//	Set(Init)
//=====================================================
//
BOOL RaceBattlePacketSender::Set(INT32 lBufferSize, AgsmAOIFilter *pAgsmAOIFilter, CHAR *pszName)
	{
	if (!pAgsmAOIFilter || !m_csBuffer.Init(lBufferSize))
		return FALSE;

	m_pAgsmAOIFilter = pAgsmAOIFilter;
	m_hSendEvent = CreateEvent(NULL, FALSE, FALSE, "RBPSEV");	
	
	strcpy(m_szName, pszName ? pszName : _T(""));
	
	m_bInit = TRUE;

	return TRUE;
	}




//	Push(to send)
//=====================================================
//
BOOL RaceBattlePacketSender::Push(PVOID pvPacket, INT16 nPacketLength)
	{
	if (!m_bInit)
		return FALSE;

	AuAutoLock Lock(m_csLock);
	if (!Lock.Result()) return FALSE;

	PVOID pvBuffer = m_csBuffer.Alloc(nPacketLength);
	::CopyMemory(pvBuffer, pvPacket, nPacketLength);
	m_QueueItem.push_back(pvBuffer);
	m_QueueLength.push_back(nPacketLength);
	
	SetEvent(m_hSendEvent);

	return TRUE;
	}


