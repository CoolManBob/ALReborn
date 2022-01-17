/******************************************************************************
Module:  AgsmDeath.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 11
******************************************************************************/

#if !defined(__AGSMDEATH_H__)
#define __AGSMDEATH_H__

#include "ApBase.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmUnion.h"
#include "AgpmShrine.h"
#include "AgpmParty.h"
#include "ApmEventManager.h"
#include "AgpmEventBinding.h"
#include "AgpmLog.h"
#include "AgpmBillInfo.h"
#include "AgpmPvP.h"
#include "AgpmRide.h"
#include "AgpmConfig.h"

#include "AuRandomNumber.h"
#include "AuPacket.h"

#include "AsDefine.h"
#include "AgsEngine.h"
#include "AgsmAOIFilter.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmItemManager.h"
#include "AgsmDropItem2.h"
#include "AgsmFactors.h"
#include "AgsmCombat.h"
#include "AgsmParty.h"
#include "AgsmSummons.h"
#include "AgsmSkill.h"

#include "AgsdDeath.h"

#include "AgpmGrid.h"
#include "AgpmItemConvert.h"
#include "AgsmUsedCharDataPool.h"

#include "AgpmSiegeWar.h"

#include "AgpmTitle.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmDeathD" )
#else
#pragma comment ( lib , "AgsmDeath" )
#endif
#endif

//const int	AGSMDEATH_MAX_HIT_PRESERVE_TIME		= 30;			// 최대 HitHistory 저장 시간 (초단위)
const int	AGSMDEATH_DROP_ITEM_INTERVAL		= 1000;			// 죽고 난후 아템 떨구고 경험치 계산하고 할때까지의 시간 (밀리초단위)
const int	AGSMDEATH_RES_INTERVAL				= 15000;		// 죽은후 다시 부활할때까지의 시간 (밀리초단위)

const int	AGSMDEATH_RECOVERY_POINT_INTERVAL	= 5000;			// HP, SP, MP가 최대치보다 적을때 자동으로 채워지는 간격 (milisecond)

typedef enum AgsmDeathKillerType {
	AGSMDEATH_KILLER_PK				= 1,
	AGSMDEATH_KILLER_UNION,
	AGSMDEATH_KILLER_NPC
} AgsmDeathKillerType;


typedef enum _AgsmDeathCBID {
	AGSMDEATH_CB_CHARACTER_DEAD			= 0,
	AGSMDEATH_CB_RESURRECTION,
	AGSMDEATH_CB_PRE_PROCESS_CHARACTER_DEAD,
} AgsmDeathCBID;


typedef struct	_stAgsmDeathDropItemInfo {
	PVOID		pvClass;
	ApBase*		pcsAttackerBase;
} stAgsmDeathDropItemInfo;

class AgpmBattleGround;
class AgsmBattleGround;
class AgpmEpicZone;
class AgsmEpicZone;
class AgpmAI2;
class AgsmDropItem2;

class AgsmDeath : public AgsModule {
private:
	ApmMap				*m_papmMap;
	AgpmGrid			*m_pagpmGrid;

	AgpmFactors			*m_pagpmFactors;
	AgpmCharacter		*m_pagpmCharacter;
	AgpmUnion			*m_pagpmUnion;
	AgpmItem			*m_pagpmItem;
	AgpmSkill			*m_pagpmSkill;
	AgpmItemConvert		*m_pagpmItemConvert;
	AgpmShrine			*m_pagpmShrine;
	AgpmParty			*m_pagpmParty;
	ApmEventManager		*m_papmEventManager;
	AgpmEventBinding	*m_pagpmEventBinding;
	AgpmLog				*m_pagpmLog;
	AgpmBillInfo		*m_pagpmBillInfo;
	AgpmPvP				*m_pagpmPvP;
	AgpmSummons			*m_pagpmSummons;
	AgpmConfig			*m_pagpmConfig;

	AgsmAOIFilter		*m_pagsmAOIFilter;
	AgsmCharacter		*m_pagsmCharacter;
	AgsmCharManager		*m_pagsmCharManager;
	AgsmItem			*m_pagsmItem;
	AgsmItemManager		*m_pagsmItemManager;
	AgsmDropItem2		*m_pagsmDropItem2;
	AgsmFactors			*m_pagsmFactors;
	AgsmCombat			*m_pagsmCombat;
	AgsmParty			*m_pagsmParty;
	AgsmSummons			*m_pagsmSummons;
	AgsmSkill			*m_pagsmSkill;
	AgsmUsedCharDataPool	*m_pcsAgsmUsedCharDataPool;
	AgpmEventSpawn		*m_pcsAgpmEventSpawn;
	AgpmSiegeWar		*m_pcsAgpmSiegeWar;
	AgpmBattleGround*	m_pagpmBattleGround;
	AgsmBattleGround*	m_pagsmBattleGround;
	AgpmEpicZone*		m_pagpmEpicZone;
	AgsmEpicZone*		m_pagsmEpicZone;
	AgpmAI2*			m_pagpmAI2;
	AgpmTitle*			m_pagpmTitle;

	INT16				m_nIndexADCharacter;
	INT16				m_nIndexADItem;

	//INT16				m_nMaxHitHistory;
	INT16				m_lMaxPreserveTime;

	//UINT32				m_ulRecoveryPointTime;

	INT16				m_nIdleCount;

	MTRand				m_csRandomNumber;
	double				m_partyBonus;

public:
	AuPacket			m_csPacket;
	AuPacket			m_csPacketDeath;
	AuPacket			m_csPacketDeathADItem;

public:
	AgsmDeath();
	~AgsmDeath();

	BOOL OnAddModule();

	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle2(UINT32 ulClockCount);
	//BOOL OnIdle5Times(UINT32 ulClockCount);

	static BOOL ConAgsdDeath(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesAgsdDeath(PVOID pData, PVOID pClass, PVOID pCustData);

	AgsdDeath* GetADCharacter(PVOID pData);
	AgsdDeathADItem* GetADItem(PVOID pData);

	BOOL PrepareDeath(AgpdCharacter *pTargetChar, AgpdCharacter *pAttackChar);

	// 죽은 처리 함수들...
	static BOOL ProcessIdleDeath(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);
	BOOL ProcessIdleDeath(AgpdCharacter *pCharacter, BOOL *pbIsRemoved = NULL);

	BOOL SetDead(AgpdCharacter *pcsCharacter);

	static BOOL ProcessDeathResult(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);

	BOOL Dead(AgpdCharacter *pCharacter, BOOL bAddIdleEvent = TRUE);

	INT32 CheckKiller(AgpdCharacter *pCharacter, ApBase *pcsAttacker);
	BOOL ProcessCombatResult(AgpdCharacter *pCharacter);

	BOOL ProcessDropItem(AgpdCharacter *pcsCharacter);

	BOOL RegisterDeath(AgpdCharacter *pCharacter);

	BOOL NotDead(AgpdCharacter *pCharacter);

	BOOL ProcessPinchCharacterDeath(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar);

	//BOOL KilledGuardian(AgpdCharacter *pCharacter, ApBase *pcsAttacker);

	// exp 계산 관련 함수들
	INT32	GetRacialPoint(AgpdCharacter *pcsCharacter);
	INT32	GetMonsterExp(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsMostDamager);
	INT32	GetAttackerLevel(ApBase *pcsAttackerBase);

	INT32	GetBonusExp(INT32 lTotalBonusExp, AgpdCharacter *pcsKilledChar, AgsdCharacterHistoryEntry *pstHistoryEntry);
	INT32	GetBonusExpToParty(INT32 lTotalBonusExp, AgpdCharacter *pcsKilledChar, AgsdCharacterHistoryEntry *pstHistoryEntry);

	INT64	GetPenaltyExp(AgpdCharacter *pcsCharacter, BOOL bIsResurrectionTown);
	INT32	GetPenaltyExpRate(AgpdCharacter* pcsCharacter, BOOL bIsResurrectionTown);

	INT32	GetMaxHP(AgpdCharacter *pcsCharacter);

	INT32	GetTotalDamageCurrentMember(AgpdParty* pcsParty, AgpdCharacter* pcsDeadCharacter);

	BOOL Resurrection(AgpdCharacter *pCharacter, BOOL *pbDestroyed);
	BOOL ResurrectNow(AgpdCharacter *pCharacter, BOOL bIsHPFull = FALSE, BOOL bSetDead = TRUE);
	BOOL ResurrectionNowNoPenalty(AgpdCharacter* pcsCharacter);

	VOID SetItemDropRate(AgpdItem *pcsItem, INT32 lDropRate);
	VOID SetItemDropRate(AgsdDeathADItem *pstDeathItem, INT32 lDropRate);

	BOOL DropItem(AgpdCharacter *pcsCharacter, ApBase *pcsAttacker);

	BOOL SetSkullInfo(ApBase *pcsBase, AgpmItemSkullInfo *pstSkullInfo, AgpdCharacter *pcsTargetChar);

	BOOL DropSkull(AgpdCharacter *pcsCharacter, AgpmItemSkullInfo *pstSkullInfo);
	BOOL DropSkull(AgpdCharacter *pcsCharacter);

	static BOOL CBDropItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAttackCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBSendCharacterAllServerInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBDisconnectCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBRequestResurrection(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBResurrectionByOther(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBEndPeriodStart(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL			AddBonusExpToChar(INT32 lCID, INT64 llBonusExp, BOOL bIsLock = TRUE, BOOL bIsPCBangBonus = FALSE, BOOL bIsQuest = FALSE);
	BOOL			AddBonusExpToChar(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsKilledCharacter, INT64 llBonusExp, BOOL bIsLock = TRUE, BOOL bIsPCBangBonus = FALSE, BOOL bIsQuest = FALSE);

	BOOL			AddBonusExpToParty(AgpdParty *pcsParty, INT64 llBonusExp, INT32 lMemberTotalLevel, INT32 lNumCombatMember, AgpdCharacter **pacsCombatMember);

	INT32			GetUnionRank(ApBase *pcsBase);

	PVOID			MakePacketDeath(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, BOOL bLogin = TRUE);
	PVOID			MakePacketDeathADItem(AgpdItem *pcsItem, INT16 *pnPacketLength, BOOL bLogin = TRUE);

	BOOL			SendPacketDeath(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bLogin = TRUE);
	BOOL			SendPacketDeathADItem(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bLogin = TRUE);

	BOOL			OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL			SendUpdateRecoveryPoint(AgpdCharacter* pcsCharacter, PVOID pvPacketFactor);
	
	// Log - 2004.04.29. steeple
	BOOL			WriteDeathLog(AgpdCharacter* pcsCharacter);

	BOOL			SetCallbackCharacterDead(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackPreProcessCharacterDead(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackResurrection(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif //__AGSMDEATH_H__
