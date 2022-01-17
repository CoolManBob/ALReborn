/******************************************************************************
Module:  AgsmSkill.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 03. 24
******************************************************************************/

#if !defined(__AGSMSKILL_H__)
#define __AGSMSKILL_H__

#include "ApBase.h"

#include "AgpmGrid.h"

#include "ApmMap.h"

#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgpmSkill.h"
#include "AgpmParty.h"
#include "AgpmShrine.h"
#include "AgpmLog.h"
#include "AgpmSummons.h"
#include "AgpmPvP.h"
#include "AgpmAI2.h"
#include "AgpmEventSpawn.h"
#include "AgpmEventSkillMaster.h"
#include "AgpmGuild.h"
#include "AgpmSiegeWar.h"
#include "AgpmArchlord.h"
#include "AgpmWantedCriminal.h"
#include "AgpmBillInfo.h"
#include "AgpmEpicZone.h"

#include "ApmEventManager.h"

#include "AsDefine.h"
#include "AgsEngine.h"
#include "AgsdSkill.h"
#include "AgsmFactors.h"
#include "AgsmAOIFilter.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmItem.h"
#include "AgsmCombat.h"
#include "AgsmParty.h"
#include "AgsmSummons.h"
#include "AgsmTimer.h"
#include "AuDatabase2.h"
//#include "AgsmDBStream.h"
//#include "AgsmServerManager.h"
#include "AgsmServerManager2.h"
#include "AgsmSystemMessage.h"

#include "AgsmEventNature.h"
#include "AgsmEpicZone.h"

//#include "AgsmEventSpawn.h"

#include "AuRandomNumber.h"
#include "AuPacket.h"

#include "AgpmOptimizedPacket2.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmSkillD" )
#else
#pragma comment ( lib , "AgsmSkill" )
#endif
#endif


/*
#define	AGSMSKILL_SKILL_NAME_BLOW_OF_DESTRUCTION				"파멸의 일격"
#define AGSMSKILL_SKILL_NAME_BUCKLER_ATTACK						"방패 공격"
#define AGSMSKILL_SKILL_NAME_PROVOCATION						"도발"
#define AGSMSKILL_SKILL_NAME_COUNTERATTACK						"반격"
#define AGSMSKILL_SKILL_NAME_CUT_A_CIRCLE						"원형베기"
#define AGSMSKILL_SKILL_NAME_ILSUM								"일섬"
#define AGSMSKILL_SKILL_NAME_RUSH								"러쉬"
#define AGSMSKILL_SKILL_NAME_ARROW_RAIN							"화살비"
#define AGSMSKILL_SKILL_NAME_PENETRATION						"관통"
#define AGSMSKILL_SKILL_NAME_POWER_BLOW							"파워블로우"
#define	AGSMSKILL_SKILL_NAME_ENERGY_WIND						"장풍"
#define AGSMSKILL_SKILL_NAME_ACCEL_ATTACK						"가속공격"
#define	AGSMSKILL_SKILL_NAME_IMMORTAL_ZIAN						"불멸의 지안"
#define	AGSMSKILL_SKILL_NAME_DEBUFF								"디버프"
#define	AGSMSKILL_SKILL_NAME_BATTLE_CRY							"전쟁의 함성"
*/


const int	AGSMSKILL_CHECKRESULT_FAILED_CAST					= 0x00000001;
const int	AGSMSKILL_CHECKRESULT_EVADE_SKILL					= 0x00000002;
const int	AGSMSKILL_CHECKRESULT_SPELL_CRITICAL_STRIKE			= 0x00000004;

const int	AGSMSKILL_MAX_TARGET_NUM							= 16;

const int	AGSMSKILL_DISARMAMENT_VALUE							= 100;

typedef enum	_eAgsmSkillCB {

	AGSMSKILL_CB_MELEE_ATTACK_CHECK				= 0,
	AGSMSKILL_CB_MAGIC_ATTACK_CHECK,

	AGSMSKILL_CB_PRE_CHECK_CAST,
	AGSMSKILL_CB_POST_CHECK_CAST,

	AGSMSKILL_CB_ATTACK,

	/*
	AGSMSKILL_CB_DB_STREAM_INSERT,
	AGSMSKILL_CB_DB_STREAM_SELECT,
	AGSMSKILL_CB_DB_STREAM_UPDATE,
	AGSMSKILL_CB_DB_STREAM_DELETE,
	*/

	AGSMSKILL_CB_GET_NEW_SKILLID,

	AGSMSKILL_CB_INSERT_SKILL_MASTERY_TO_DB,
	AGSMSKILL_CB_UPDATE_SKILL_MASTERY_TO_DB,
	AGSMSKILL_CB_DELETE_SKILL_MASTERY_TO_DB,

	AGSMSKILL_DB_PARSE_SKILL_TREE_STIRNG,

	AGSMSKILL_CB_CHECK_SPECIAL_STATUS_FACTOR,
	AGSMSKILL_CB_CHECK_POISON_STATUS_FACTOR,

	AGSMSKILL_CB_PRODUCT_SKILL,
	AGSMSKILL_DB_PARSE_SKILL_COMPOSE_STIRNG,

	AGSMSKILL_CB_AFFECTED_SKILL,

	AGSMSKILL_CB_DAMAGE_ADJUST_SIEGEWAR,

	AGSMSKILL_CB_CAST_SKILL,

	AGSMSKILL_CB_RIDE,
} eAgsmSkillCB;


typedef enum	_AgsmSkillPacketOperation 
{
	AGSMSKILL_PACKET_OPERATION_UPDATE_AGSD_SKILL				= 0,
	AGSMSKILL_PACKET_OPERATION_UPDATE_ATTACH_BASE_DATA,
	AGSMSKILL_PACKET_OPERATION_ADD_DB_DATA,
	AGSMSKILL_PACKET_OPERATION_EQIP_LIST,
} AgsmSkillPacketOperation;

class AgpmBattleGround;
class AgsmBattleGround;
class AgsmSkillEffect;
class AgsmTitle;

class AgsmSkill : public AgsModule {
private:
	AgpmGrid*			m_pagpmGrid;

	ApmMap*				m_papmMap;
	ApmObject*			m_papmObject;
	AgpmFactors*		m_pagpmFactors;
	AgpmCharacter*		m_pagpmCharacter;
	AgpmItem*			m_pagpmItem;
	AgpmItemConvert*	m_pagpmItemConvert;
	AgpmCombat*			m_pagpmCombat;
	AgpmSkill*			m_pagpmSkill;
	ApmEventManager*	m_papmEventManager;
	AgpmParty*			m_pagpmParty;
	AgpmShrine*			m_pagpmShrine;
	AgpmLog*			m_pagpmLog;
	AgpmEventNature*	m_pagpmEventNature;
	AgpmSummons*		m_pagpmSummons;
	AgpmPvP*			m_pagpmPvP;
	AgpmEventSpawn*		m_pagpmEventSpawn;
	AgpmAI2*			m_pagpmAI2;
	AgpmEventSkillMaster* m_pagpmEventSkillMaster;
	AgpmGuild*			m_pagpmGuild;
	AgpmSiegeWar*		m_pagpmSiegeWar;
	AgpmArchlord*		m_pagpmArchlord;
	AgpmTimer*			m_pagpmTimer;
	AgpmOptimizedPacket2*	m_pagpmOptimizedPacket2;
	AgpmWantedCriminal* m_pagpmWantedCriminal;
	AgpmBillInfo*		m_pagpmBillInfo;
	AgpmEpicZone*		m_pagpmEpicZone;

	AgsmSystemMessage*	m_pagsmSystemMessage;
	//AgsmDBStream*		m_pagsmDBStream;
	AgsmFactors*		m_pagsmFactors;
	AgsmAOIFilter*		m_pagsmAOIFilter;
	AgsmCharacter*		m_pagsmCharacter;
	AgsmCharManager*	m_pagsmCharManager;
	AgsmItem*			m_pagsmItem;
	AgsmCombat*			m_pagsmCombat;
	AgsmServerManager*	m_pAgsmServerManager;
	AgsmParty*			m_pagsmParty;
	AgsmSummons*		m_pagsmSummons;
	AgsmTimer*			m_pagsmTimer;
	AgsmEventNature*	m_pagsmEventNature;
	//AgsmEventSpawn*		m_pagsmEventSpawn;
	AgpmBattleGround*	m_pagpmBattleGround;
	AgsmBattleGround*	m_pagsmBattleGround;
	AgsmSkillEffect*	m_pagsmSkillEffect;
	AgsmTitle*			m_pagsmTitle;

	INT16				m_nIndexADSkill;
	INT16				m_nIndexADSkillTemplate;
	INT16				m_nIndexADCharacter;
	INT16				m_nIndexADObject;

	MTRand				m_csRandom;

	INT32				m_lPointFactorTypeIndex;
	INT32				m_lPercentFactorTypeIndex;
	INT32				m_lResultFactorTypeIndex;
	INT32				m_lItemPointFactorTypeIndex;
	INT32				m_lItemPercentFactorTypeIndex;

public:
	AuPacket			m_csPacket;

	AuPacket			m_csPacketAttachBaseData;
	AuPacket			m_csPacketBuffedSkillProcessInfo;
	AuPacket			m_csPacketTempSkillInfo;

	AuPacket			m_csPacketDBData;
	AuPacket			m_csPacketMasteryDBData;

	vector<AgpdSkill*>	m_vcArchlordSkill;
	ApString<AGPDCHARACTER_MAX_ID_LENGTH> m_szArchlord;

private:
	BOOL				IsValidSkillCast(AgpdSkill *pcsSkill);

	//AgpdFactor*			GetTargetFactorResult(AgpdSkill *pcsSkill);

	BOOL				RestoreDisturbState(AgpdSkill *pcsSkill, ApBase *pcsTarget);
	BOOL				RestoreSkillFactor(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nBuffedIndex = (-1));
	//BOOL				RestoreSkillFactor(ApBase *pcsBase, INT16 nBuffedIndex);
	BOOL				ProcessReflectDamage(ApBase *pcsAttackBase, ApBase *pcsTargetBase, INT32 lDamage);

	UINT32				GetBaseDPNID(ApBase *pcsBase);
	ApBase*				GetBaseLock(ApBase *pcsBase);
	ApBase*				GetBase(ApBase *pcsBase);

	BOOL				FreeSkillEffect(AgpdSkill *pcsSkill, ApBase *pcsBase);

	INT32				GetSpellCount(AgpdSkill *pcsSkill);

	INT32				AddSpellCount(AgpdSkill *pcsSkill, ApBase *pcsBase, INT32 lSpellCount);
public:
	BOOL				UpdateFactorPermanently(AgpdSkill *pcsSkill, ApBase *pcsTarget, eAgpmSkillUsedConstFactor eConstIndex);
	BOOL				UpdateSkillFactor(AgpdSkill *pcsSkill, ApBase *pcsTarget, BOOL bIsFactorNotQueueing = FALSE, BOOL bProcessInterval = FALSE);
	INT32				ReduceSpellCount(ApBase *pcsBase, INT32 lSpellCount, INT32 *plDispelSkillIndex, AgpdSkill *pcsSkill = NULL);
	BOOL				FreeSkillEffect(AgpdSkill *pcsSkill, ApBase *pcsBase, INT32 lBuffedListIndex);
	INT32				GetSkillRate(AgpdSkill* pcsSkill);
	BOOL				SetUpdateFactorResult(AgsdSkill *pcsAgsdSkill);
	BOOL				SetUpdateFactorPoint(AgsdSkill *pcsAgsdSkill);
	BOOL				SetUpdateFactorPercent(AgsdSkill *pcsAgsdSkill);
	AgpdFactor*			GetOwnerFactorResult(AgpdSkill *pcsSkill);
	AgpdFactor*			GetResultFactor(ApBase *pcsBase);

private:
	BOOL				SetSkillProcessInfo(AgpdSkill *pcsSkill, AgsdSkillProcessInfo *pcsSkillProcessInfo);

	INT32				TargetFiltering(AgpdSkill *pcsSkill, ApSafeArray<INT32, AGSMSKILL_MAX_TARGET_NUM>& lCIDBuffer, INT32 lNumTarget);
	INT32				DynamicTargetFiltering(AgpdSkill* pcsSkill, ApSafeArray<INT32, AGSMSKILL_MAX_TARGET_NUM>& lCIDBuffer, INT32 lNumTarget, BOOL bIsTargetSelfCharacter);
	
	BOOL				SkillRateCheck(AgpdSkill* pcsSkill);
	
	BOOL				HaltStatusCheck(AgpdSkill* pcsSkill);
	BOOL				IsDebuffSkillEnable(AgpdSkill* pcsSkill, ApBase* pcsTarget);
	BOOL				SetForceSkill(AgpdSkill* pcsSkill);
	BOOL				SkillInvincibleCheck(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget);
	BOOL				CheckUnionSkill(AgpdSkill* pcsSkill);
	
	AgpdSkill*			GetTempSkill(ApBase *pcsBase, INT32 lSkillTID);

	// 1. range check functions
	///////////////////////////////////////////////////////////////////////////////
	INT32	RangeCheck								(AgpdSkill *pcsSkill, INT_PTR *plID, INT16 nMaxBuffer, INT32* plID2, INT16 nMaxBuffer2, ApdEvent *pcsEvent);

	INT32	RangeCheckSelfOnly						(AgpdSkill *pcsSkill, INT_PTR *plID, INT16 nMaxBuffer, INT32* plID2, INT16 nMaxBuffer2, ApdEvent *pcsEvent);
	INT32	RangeCheckTargetOnly					(AgpdSkill *pcsSkill, INT_PTR *plID, INT16 nMaxBuffer, INT32* plID2, INT16 nMaxBuffer2, ApdEvent *pcsEvent);
	INT32	RangeCheckSphere						(AgpdSkill *pcsSkill, INT_PTR *plID, INT16 nMaxBuffer, INT32* plID2, INT16 nMaxBuffer2, ApdEvent *pcsEvent);
public:	
	INT32	RangeCheckSphereForATField				(AgpdSkill *pcsSkill, ApBase* pcsTarget, INT_PTR *plID, INT16 nMaxBuffer, INT32* plID2, INT16 nMaxBuffer2, ApdEvent *pcsEvent);
private:
	INT32	RangeCheckBox							(AgpdSkill *pcsSkill, INT_PTR *plID, INT16 nMaxBuffer, INT32* plID2, INT16 nMaxBuffer2, ApdEvent *pcsEvent);
	INT32	RangeCheckSummonsOnly					(AgpdSkill *pcsSkill, INT_PTR *plID, INT16 nMaxBuffer, INT32* plID2, INT16 nMaxBuffer2, ApdEvent *pcsEvent);

	INT32	RangeCheckBaseTargetToFirst				(INT_PTR* plID, INT16 nMaxBuffer, INT32* plID2, INT16 nMaxBuffer2, INT_PTR lBaseTarget, INT32 lBaseTargetCID);

	// 2. condition check functions
	///////////////////////////////////////////////////////////////////////////////
	BOOL	ConditionCheck							(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex);

	BOOL	ConditionCheckMeleeAttack				(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex);
	BOOL	ConditionCheckMagicAttack				(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex);
	BOOL	ConditionCheckDeBuff					(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex);
	BOOL	ConditionCheckFriendlyUnit				(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex);
	BOOL	ConditionCheckLevel						(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex);
	//BOOL	ConditionCheckKillGuardian				(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex);
	BOOL	ConditionCheckCharType					(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex);
	BOOL	ConditionCheckOwnItem					(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex);
	BOOL	ConditionCheckPartyMember				(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex);

	// 2. 1. calculate & apply skill cost
	///////////////////////////////////////////////////////////////////////////////
	BOOL	CostApply								(AgpdSkill *pcsSkill);

	BOOL	CostConsumeHP							(AgpdSkill *pcsSkill);
	BOOL	CostConsumeMP							(AgpdSkill *pcsSkill);
	BOOL	CostConsumeSP							(AgpdSkill *pcsSkill);
	BOOL	CostConsumeArrow						(AgpdSkill *pcsSkill);

	// 2. 2. check and process that pcsSkill is already buffed
	///////////////////////////////////////////////////////////////////////////////
	BOOL	ProcessAlreadyBuffedSkill				(AgpdSkill *pcsSkill, ApBase *pcsTarget);
	INT32	ProcessUnionSkill						(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT32 lBuffedIndex);


	// 4. end skill effect functions
	///////////////////////////////////////////////////////////////////////////////
	BOOL	EndSkillEffect							(AgpdSkill *pcsSkill, ApBase *pcsTarget);

	BOOL	EndSkillEffectConsumeHP					(AgpdSkill *pcsSkill, ApBase* pcsTarget);

	///////////////////////////////////////////////////////////////////////////////
	BOOL	EndDefenseSkill							(AgpdSkill *pcsSkill, ApBase* pcsTarget);
	BOOL	RestoreCombatSkillArg					(AgpdSkill *pcsSkill, ApBase* pcsTarget);
	BOOL	RestoreSkillFactorArg					(AgpdSkill *pcsSkill, ApBase* pcsTarget);

	BOOL	RestoreOriginalStatus					(AgpdSkill *pcsSkill, ApBase *pcsTarget);
	
	BOOL	RestoreSpecialStatus					(AgpdSkill* pcsSkill, ApBase* pcsTarget);

	BOOL	RestoreSummons							(AgpdSkill* pcsSkill, ApBase* pcsTarget);

	BOOL	RestoreUnionType1						(AgpdSkill* pcsSkill, ApBase* pcsTarget);
	BOOL	RestoreUnionType2						(AgpdSkill* pcsSkill, ApBase* pcsTarget);

	BOOL	ResotreSpecificLevelUp					(AgpdSkill* pcsSkill, ApBase* pcsTarget);

	BOOL	CalcUpdateFactorItem(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bInit = TRUE);

	BOOL	PreCheckProductSkill(AgpdSkill* pcsSkill, ApBase* pcsTarget);

	BOOL	SetUpdateFactorItemPoint(AgsdSkill *pcsAgsdSkill);
	BOOL	SetUpdateFactorItemPercent(AgsdSkill *pcsAgsdSkill);
public:
	BOOL	ApplyUpdateFactorItem(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bFirst, BOOL bAdd, BOOL bTimeOut = TRUE);
	BOOL	DestroyUpdateFactorPoint(AgsdSkill *pcsAgsdSkill);
	BOOL	DestroyUpdateFactorPercent(AgsdSkill *pcsAgsdSkill);
	BOOL	DestroyUpdateFactorResult(AgsdSkill *pcsAgsdSkill);
	BOOL	DestroyUpdateFactorItemPoint(AgsdSkill *pcsAgsdSkill);
	BOOL	DestroyUpdateFactorItemPercent(AgsdSkill *pcsAgsdSkill);

	BOOL	EndUpdateCharacterConfig(AgpdCharacter* pcsCharacter, AgpdSkill* pcsSkill);

private:
	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

public:
	AgsmSkill();
	~AgsmSkill();

	BOOL				OnAddModule();

	BOOL				OnInit();
	BOOL				OnDestroy();

	static BOOL			ProcessIdleEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				ProcessUseSkill(ApdEvent *pcsEvent, UINT32 ulClockCount);

	BOOL				ProcessBuffedSkill(ApBase *pcsBase, UINT32 ulClockCount);

	static BOOL			ConAgsdSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgsdSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			ConAgsdSkillADBase(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgsdSkillADBase(PVOID pData, PVOID pClass, PVOID pCustData);

	AgsdSkill*			GetADSkill(AgpdSkill *pcsSkill);
	AgsdSkillADBase*	GetADBase(ApBase *pcsBase);

	BOOL				CastSkill(AgpdSkill *pcsSkill, ApBase csTargetBase, AuPOS posTarget, INT32 lTargetUnion, BOOL bForceAttack);
	BOOL				CastSkill(ApBase *pcsBase, INT32 lSkillTID, INT32 lSkillLevel, ApBase *pcsTargetBase = NULL, BOOL bForceAttack = FALSE, BOOL bMonsterUse = FALSE, INT32 lSkillScrollIID = AP_INVALID_IID);
	BOOL				CastSkill(ApBase *pcsBase, INT32 lSkillTID, INT32 lSkillLevel, AuPOS *posDest = NULL, BOOL bForceAttack = FALSE);
	BOOL				CastSkill(AgsdCastSkill& pAgsdCastSkill);
	BOOL				CastSkill(AgpdSkill *pcsSkill, BOOL bMonsterUse = FALSE);

	BOOL				CancelSkill(AgpdSkill *pcsSkill);

	BOOL				CastPassiveSkill(AgpdSkill *pcsSkill);
	BOOL				ReCastAllPassiveSkill(ApBase* pcsBase);
	BOOL				CastSiegeWarSkillOnActive(AgpdCharacter* pcsCharacter);
	BOOL				CastCondition2Skill(AgpdCharacter* pcsCharacter, INT32 lConditionType, BOOL bCash);
	BOOL				CastSkillUnlimitedTarget(AgsdCastSkill pcsCastSkill);
	BOOL				CastPartySkill(AgsdCastSkill pcsCastSkill);
	BOOL				CastInvolveParentRegionSkill(AgsdCastSkill pcsCastSkill);

	BOOL				EndSkill(AgpdSkill *pcsSkill);
	BOOL				EndPassiveSkill(AgpdSkill *pcsSkill, BOOL bBelowSkillActive = FALSE);
	BOOL				EndAllPassiveSkill(ApBase *pcsBase);
	BOOL				EndBuffedSkill(ApBase *pcsBase, INT32 lBuffedIndex, BOOL bCreateSkillTemplate = TRUE, const TCHAR* szReason = NULL);
	BOOL				EndBuffedSkill(ApBase *pcsBase, AgpdSkill *pcsSkill);
	BOOL				EndBuffedSkillByTID(ApBase *pcsBase, INT32 lSkillTID, BOOL bInitCoolTime = FALSE);
	BOOL				EndAllBuffedSkill(ApBase *pcsBase, BOOL bCreateTempSkill = TRUE);
	BOOL				EndAllBuffedSkillExceptTitleSkill(ApBase *pcsBase, BOOL bCreateTempSkill = TRUE);
	BOOL				EndAllBuffedSkillExceptDebuff(ApBase* pcsBase, BOOL bCreateTempSkill = TRUE);
	BOOL				EndAllBuffedSkillExceptCashSkill(ApBase* pcsBase, BOOL bCreateTempSkill = TRUE);
	BOOL				EndAllBuffedSkillExceptCash_SummonsSkill(ApBase* pcsBase, BOOL bCreateTempSkill = TRUE, BOOL bInitCoolTime = FALSE, INT32 lMaxCount = 0);
	BOOL				EndAllDebuffSkill(ApBase *pcsBase, BOOL bCreateTempSkill = TRUE, INT32 lRate = 100);
	BOOL				EndMovementBuffedSkill(ApBase* pcsBase, BOOL bCreateTempSkill = TRUE);	// 2005.08.25. steeple
	BOOL				EndSummonsBuffedSkill(ApBase* pcsBase, BOOL bCreateTempSkill = TRUE, INT32 lTID = 0);	// 2005.10.09. steeple
	BOOL				EndTameBuffedSkill(ApBase* pcsBase, BOOL bCreateTempSkill = TRUE);	// 2005.09.30. steeple
	BOOL				EndTransparentBuffedSkill(ApBase* pcsBase, BOOL bCreateTempSkill = TRUE);	// 2005.09.30. steeple
	BOOL				EndCashItemBuffedSkill(ApBase* pcsBase, INT32 lSkillTID, BOOL bCreateTempSkill = TRUE);	// 2005.12.09. steeple
	BOOL				EndBuffedSkillOnEvolution(ApBase* pcsBase);

	BOOL				ApplySkill(AgpdSkill *pcsSkill, ApdEvent *pcsEvent);
	BOOL				ApplySkillSetTwicePacket(AgpdSkill* pcsSkill, ApdEvent* pcsEvent, ApSafeArray<INT32, AGSMSKILL_MAX_TARGET_NUM>& lCIDBuffer, INT32 lNumTarget);	// 2004.11.08. steeple
	BOOL				ApplySkillTwicePacket(AgpdSkill* pcsSkill, ApdEvent* pcsEvent);	// 2004.11.08. steeple
	BOOL				ApplySkillFinal(AgpdSkill* pcsSkill, ApdEvent* pcsEvent, ApSafeArray<INT32, AGSMSKILL_MAX_TARGET_NUM>& lCIDBuffer, INT32 lNumTarget, BOOL bIsTargetSelfCharacter);	// 2004.11.08. steeple
	BOOL				ApplySkillByBuffedList(AgpdSkill *pcsSkill, ApBase *pcsTarget);
	BOOL				ApplySkillAttack(AgpdSkill* pcsSkill, ApBase* pcsTargetBase);
	BOOL				ApplySkillUnlimitedTargetSiegeWar(AgpdSkill* pcsSkill, ApSafeArray<INT32, AGSMSKILL_MAX_TARGET_NUM>& lCIDBuffer, INT32 lNumTarget, BOOL bIsTargetSelfCharacter);	// 2006.08.11. steeple

	// Skill 로 인한 추가적인 Attack 함수들.
	BOOL				ProcessChargeAttack(pstAgsmCombatAttackResult pstCombatResult);
	BOOL				ProcessATFieldAttack(AgpdSkill* pcsSkill, AgpdCharacter* pcsAttacker, AgpdCharacter* pcsTarget);
	BOOL				ProcessActionOnActionType3(pstAgsmCombatAttackResult pstCombatResult);
	BOOL				ProcessActionOnActionType6(AgpdCharacter* pcsCharacter);
	BOOL				ProcessActionPassiveAttack(pstAgsmCombatAttackResult pstCombatResult);
	BOOL				ProcessDivideDamage(pstAgsmCombatAttackResult pstCombatResult);
	BOOL				ProcessDivideDamage(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget, AgpdFactorAttribute* pstAttrDamage, INT32* plDamage = NULL, INT32* plSpiritDamage = NULL);
	
	BOOL				AddBuffedList(AgpdSkill* pcsSkill, ApBase* pcsTarget);
	BOOL				IsEnableSkillByBuffedList(AgpdSkill* pcsSkill, ApBase* pcsTarget);
	BOOL				IsEnableSkillByBuffedList(AgpdItem* pcsItem, ApBase* pcsTarget);

	BOOL				CheckSkillDurationByDistance(AgpdSkill* pcsSkill, ApBase* pcsTarget);
	BOOL				CheckCashSkillDuration(AgpdSkill* pcsSkill, ApBase* pcsTarget);
	BOOL				CheckCasterStatus(AgpdSkill* pcsSkill, ApBase* pcsTarget);
	BOOL				CheckDOTCount(AgpdSkill* pcsSkill, ApBase* pcsTarget);
	
	BOOL				CheckLevelDiff(AgpdSkill* pcsSkill, ApBase* pcsTarget);	// 2006.06.27. steeple
	inline BOOL			CheckOnceAffected(AgpdSkillTemplate* pcsSkillTemplate);	// 2006.11.27. steeple

	BOOL				SetConnectionInfo(AgpdSkill* pcsSkill);							// 2007.10.23. steeple
	BOOL				CheckConnectionInfo(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval = FALSE);	// 2007.10.23. steeple

	BOOL				IsTwicePacketSkill(AgpdSkill* pcsSkill);		// 2004.11.08. steeple
	BOOL				IsSetTwicePacketSkill(AgpdSkill* pcsSkill);		// 2004.11.08. steeple
	BOOL				InitTwicePacketSkill(AgpdSkill* pcsSkill);		// 2004.11.08. steeple

	BOOL				IsEnableCashItemSkillOnRide(AgpdSkill* pcsSkill);	// 2005.12.28. steeple

	BOOL				SetAttackDamageOnly(AgpdSkill* pcsSkill);		// Attack Damage 만 준다고 세팅한다.
	BOOL				IsSetAttackDamageOnly(AgpdSkill* pcsSkill);		// Attack Damage 만 준다고 세팅되어 있는 지 확인.

	INT32				GetConstDamageA(AgpdSkill *pcsSkill, ApBase* pcsTarget);

	INT32				GetRealSkillLevel(AgpdSkill *pcsSkill);
	INT32				GetModifiedSkillLevel(AgpdSkill *pcsSkill);
	INT32				SetModifiedSkillLevel(AgpdSkill *pcsSkill);
	INT32				RestoreModifiedSkillLevel(AgpdSkill *pcsSkill);
	INT32				GetSpecificLevelUpSkillFromBuffedList(ApBase* pcsBase, INT32 lSkillTID);

	INT32				GetRealCharLevel(ApBase *pcsBase);
	INT32				GetModifiedCharLevel(ApBase *pcsBase);
	INT32				SetModifiedCharLevel(ApBase *pcsBase, AgpdSkill *pcsSkill);
	INT32				RestoreModifiedCharLevel(ApBase *pcsBase, AgpdSkill *pcsSkill);

	INT32				CalcNormalSkillDamage(AgpdSkill* pcsSkill, ApBase* pcsTarget);
	INT32				CalcSpiritSkillDamage(AgpdSkill* pcsSkill, ApBase* pcsTarget, AgpdFactorAttribute* pstAttrDamage);
	INT32				CalcNormalHeroicSkillDamage(AgpdSkill* pcsSkill, ApBase* pcsTarget);
	INT32				GetSpiritConstIndexByFactor(AgpdFactorAttributeType eAttributeType);
	INT32				GetSpiritDOTConstIndexByFactor(AgpdFactorAttributeType eAttributeType);
	INT32				GetSpiritEAConstIndexByFactor(AgpdFactorAttributeType eAttributeType);

	BOOL				ProcessSummonsType5InBuffedList(ApBase* pcsOwner, ApBase* pcsSummons);
	BOOL				ProcessSummonsType5InPassiveList(ApBase* pcsOwner, ApBase* pcsSummons);

	BOOL				LearnArchlordSkill(AgpdCharacter* pcsCharacter);
	BOOL				RemoveArchlordSkill(AgpdCharacter* pcsCharacter);

	BOOL				BuffRaceSkill(INT32 lRace);
	BOOL				BuffRaceSkill(AgpdCharacter* pcsCharacter);
	BOOL				EndRaceSkill(INT32 lRace);

	BOOL				ProcessOptionStun(AgpdCharacter* pcsAttacker, AgpdCharacter* pcsTarget);
	BOOL				ProcessDamageToHPInstant(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT32 lDamage);
	BOOL				ProcessInitCoolTime(AgpdCharacter* pcsCharacter, INT32 lSkillTID);

	BOOL				SetReleaseTransparent(AgpdSkill* pcsSkill);
	BOOL				IsReleaseTransparentSkillTID(ApBase* pcsBase, INT32 lSkillTID);

	double				GetPCBangItemSkillBonusRate(AgpdSkill* pcsSkill);

	static BOOL			CBInitSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRemoveSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBActionSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBCreateCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveAction(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReceiveRealHit(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBMeleeAttack(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBMagicAttack(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBPreCheckCombat(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBPostCheckCombat(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBPreCheckCast(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBPostCheckCast(PVOID pData, PVOID pClass, PVOID pCustData);
	
	static BOOL			CBConvertDamage(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBAttackFromCombat(PVOID pData, PVOID pClass, PVOID pCustData);

	//static BOOL			CBDamageAdjust(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBUpdateDIRTPoint(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBEquipItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBUnEquipItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBSendCharacterViewInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBSendCharacterNewID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBSendCharacterAllServerInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBReCalcFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBTransformStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBTransformRestore(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBEvolution(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRestoreEvolution(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBNewSummons(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEndSummons(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEndTaming(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEndFixed(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUpdateSpecialStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUnUseCashItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBPreCheckEnableSkillScroll(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBSetArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBModifiedSkillLevel(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBGetModifiedSkillLevel(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBEndBuffedSkillByItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUseItemReverseOrb(PVOID pData, PVOID pClass, PVOID pCustData);
	
//#ifdef	__NEW_MASTERY__
//	static BOOL			CBMasteryUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL			CBMasteryNodeUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL			CBMasteryAddSPResult(PVOID pData, PVOID pClass, PVOID pCustData);
//#else
//	//static BOOL			CBMasteryResult(PVOID pData, PVOID pClass, PVOID pCustData);
//	//static BOOL			CBMasteryUpdateTotalSP(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL			CBMasteryChange(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL			CBMasterySpecialize(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL			CBMasteryUnSpecialize(PVOID pData, PVOID pClass, PVOID pCustData);
//#endif	//__NEW_MASTERY__

//	static BOOL			CBMasteryRollback(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUpdateAllToDB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData);

//#ifndef	__NEW_MASTERY__
//	BOOL				GetFieldChangeMasteryPacket(ApBase *pcsBase, PVOID pvPacketMastery);
//#endif	//__NEW_MASTERY__
//	INT32				GetTotalSP(ApBase *pcsBase);

//#ifndef	__NEW_MASTERY__
//	BOOL				ChangeMastery(ApBase *pcsBase, PVOID pvPacketMastery);
//	BOOL				CheckReceiveMastery(ApBase *pcsBase);
//	BOOL				UpdateReceiveMastery(ApBase *pcsBase);
//#endif	//__NEW_MASTERY__

	BOOL				SetCallbackMeleeAttackCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackMagicAttackCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackPreCheckCast(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackPostCheckCast(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackInsertSkillMasteryToDB(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackUpdateSkillMasteryToDB(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackDeleteSkillMasteryToDB(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackParseSkillTreeString(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackParseSkillComposeString(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackCheckSpecialStatusFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackCheckPoisonStatusFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackDamageAdjustSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackCastSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SendCastSkill(AgpdSkill *pcsSkill, INT16 nActionType, INT32* palTargetCID, INT16 nLenTargetCID, AgpdCharacter **apcsCharacter = NULL, INT16 nNumTarget = 0);
	BOOL				SendMissCastSkill(AgpdSkill *pcsSkill, ApBase *pcsTargetBase, UINT32 ulAdditionalEffect = 0);

	BOOL				SendSkillAll(AgpdCharacter *pcsCharacter, UINT32 ulDPNID);
	
	BOOL				SendPassiveSkillAll(AgpdCharacter *pcsCharacter, UINT32 ulDPNID);//JK_패시브스킬 표시 오류
	
	BOOL				SendPacketDBData(AgpdCharacter *pcsCharacter, UINT32 ulNID);

	BOOL				SendEquipSkill(AgpdCharacter* pcsCharacter, UINT32 ulNID);

	BOOL				SendUpdateBuffedList(ApBase *pcsBase, INT32 lTID, INT32 lCasterTID, BOOL bAdd, UINT32 ulEndTimeMSec, UINT8 ucChargeLevel, UINT32 ulDPNID, UINT32 ulExpiredTime = 0);

	BOOL				SendAllBuffedSkill(ApBase *pcsBase, UINT32 ulNID, BOOL bGroupNID, UINT32 ulSelfNID);

//	BOOL				SendMasteryInfo(ApBase *pcsBase);
//	BOOL				SendMasteryInfo(ApBase *pcsBase, UINT32 ulNID);

	BOOL				SendCastResult(AgpdSkill *pcsSkill, ApBase *pcsBase, ApBase *pcsTarget, PVOID pvPacketFactor, eAgpmSkillActionType eResultType, BOOL bIsFactorNotQueueing = FALSE, BOOL bIsSyncHP = FALSE, UINT32 ulAdditionalEffect = 0);
	BOOL				SendAdditionalEffect(ApBase* pcsBase, INT32 lEffectID, INT32 lTargetCID = 0, UINT32 ulNID = 0);

	BOOL				SendModifiedSkillLevel(ApBase* pcsBase);
	BOOL				SendInitCoolTime(ApBase* pcsBase, INT32 lSkillTID);
	BOOL				SendSpecificSkillLevel(AgpdSkill* pcsSkill);

//	BOOL				SendMasteryRollbackResult(ApBase *pcsBase, BOOL bResult, UINT32 ulNID);

	/*
	BOOL				SendMasteryNeedSelect(ApBase *pcsBase, INT32 lMasteryIndex);
	BOOL				SendMasteryNeedSelect(ApBase *pcsBase, INT32 lMasteryIndex, UINT32 ulNID);
	*/

	/*
	BOOL				SetCallbackDBStreamInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackDBStreamDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackDBStreamSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackDBStreamUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	*/

	BOOL				SetCallbackGetNewSkillID(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackProductSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackAffectedSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackRide(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	/*
	// Get Query Result;
	AgpdSkill			*GetSelectResult( COLEDB *pcOLEDB );

	// get DB Query String.
	BOOL				GetInsertQuery( AgpdSkill *pcsSkill, char *pstrQuery );
	BOOL				GetSelectQuery( char *szCharName, char *pstrQuery );
	BOOL				GetUpdateQuery( AgpdSkill *pcsSkill, char *pstrQuery );
	BOOL				GetDeleteQueryByOwner( char *szCharName, char *pstrQuery );
	BOOL				GetDeleteQueryByDBID( AgpdSkill *pcsSkill, char *pstrQuery );

	// db operation
	BOOL				StreamInsertDB(AgpdSkill *pcsSkill, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamSelectDB(CHAR *szCharName, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamUpdateDB(AgpdSkill *pcsSkill, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamDeleteDB(AgpdSkill *pcsSkill, ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SaveSkillDIRT(AgpdSkill *pcsSkill);

	static BOOL			CBSaveCharacterDB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBStreamDB(PVOID pData, PVOID pClass, PVOID pCustData);
	*/

	PVOID				MakePacketAgsdSkill(AgpdSkill *pcsSkill, INT16 *pnPacketLength, BOOL bLogin = TRUE);
	PVOID				MakePacketBuffedSkillProcessInfo(AgsdSkillProcessInfo *pcsSkillProcessInfo);
	PVOID				MakePacketTempSkillInfo(AgpdSkill *pcsSkill);

	PVOID				MakePacketAddDBData(ApBase *pcsBase, INT16 *pnPacketLength);

	BOOL				UseSkill(CHAR *szSkillName, ApBase *pcsOwner, ApBase *pcsTarget);

	/*
	// get DB2 Query String.
	BOOL				GetInsertQuery2(AgpdCharacter *pcsCharacter, INT32 lMasteryIndex, CHAR *pstrQuery);
	BOOL				GetSelectQuery2(CHAR *szCharName, CHAR *pstrQuery);
	BOOL				GetDeleteQueryByOwner2(CHAR *szCharName, CHAR *pstrQuery);
	*/

	// get DB3 Query String.
	BOOL				GetInsertQuery3(AgpdCharacter *pcsCharacter, char *pstrWorldDBName, CHAR *pstrSkillSeq, CHAR *pstrQuery, INT32 lQueryLength);
	BOOL				GetSelectQuery3(CHAR *szCharName, char *pstrWorldDBName, CHAR *pstrQuery, INT32 lQueryLength);
	BOOL				GetDeleteQueryByOwner3(CHAR *szCharName, CHAR *pstrQuery, INT32 lQueryLength);

	//BOOL				GetInsertQuery4( CHAR *pstrQuery, INT32 lQueryLength);
	//BOOL				SetParamInsertQuery4( COLEDB *pcOLEDB, AgpdCharacter *pcsCharacter, CHAR *pstrSkillSeq );
	//BOOL				GetSelectQuery4( CHAR *pstrQuery, INT32 lQueryLength);

	BOOL				SetParamInsertQuery5(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter, CHAR *pstrSkillSeq);

//	BOOL				SendRelayUpdate(ApBase *pcsBase, INT32 lMasteryIndex);
//	BOOL				SendRelayInsert(ApBase *pcsBase, INT32 lMasteryIndex);
//	BOOL				SendRelayDelete(ApBase *pcsBase, INT32 lMasteryIndex);
	BOOL				SendRelayUpdate(ApBase *pcsBase);
	BOOL				SendRelayInsert(ApBase *pcsBase);
	BOOL				SendRelayDelete(ApBase *pcsBase);

	BOOL				IsMaxBuffedSkill(ApBase *pcsBase, AgpdSkill *pcsSkill);

//	BOOL				MakeStringMasteryActiveNodeSequence(ApBase *pcsBase, INT32 lMasteryIndex, CHAR *szStringBuffer, INT32 lStringBufferSize);
//	BOOL				ParseStringMasteryActiveNodeSequence(ApBase *pcsBase, INT32 lMasteryIndex, CHAR *szStringBuffer, INT32 lTotalMasteryPoint);

//#ifndef	__NEW_MASTERY__
//	INT32				SortPrintActiveNode(AgpdMasterySkillNode *pcsSkillNode, CHAR *szStringBuffer, INT32 lStringBufferSize);
//#endif	//__NEW_MASTERY__

//	BOOL				EncodingMasterySpecialize(ApBase *pcsBase, INT32 lMasteryIndex, CHAR *szStringBuffer, INT32 lStringSize);
//	BOOL				DecodingMasterySpecialize(ApBase *pcsBase, INT32 lMasteryIndex, CHAR *szStringBuffer);
	
	//////////////////////////////////////////////////////////////////////////
	// Log 관련 - 2004.05.03. steeple
//	BOOL				WriteMasteryLog(ApBase* pcsBase);
	//BOOL				WriteSkillUseLog(AgpdSkill* pcsSkill);	// 이거는 여기서 쓸지는 미정
	
	//////////////////////////////////////////////////////////////////////////
	//
	BOOL EnterGameWorld(AgpdCharacter* pcsCharacter);

	// skill save - arycoat 2008.7
	BOOL ReceiveSkillEquip(PACKET_HEADER* pvPacket);
	void RecastSaveSkill(AgpdCharacter* pcsCharacter);
	void RemoveSaveSkill(AgpdCharacter* pcsCharacter, UINT64 Attribute);
	BOOL ProcessSkillSave(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget, INT16 Operation);
	AgpdSkill* FindBuffedSkill(AgpdCharacter *pcsCharacter, INT32 lSkillTID);

	//////////////////////////////////////////////////////////////////////////
	// Evolution
	BOOL CheckEnableEvolution(AgpdCharacter* pcsCharacter, AgpdSkill* pcsSkill);
	void RecastSaveEvolutionSkill(AgpdCharacter* pcsCharacter);
	BOOL RemoveBuffedSkillEvolution(AgpdCharacter* pcsCharacter);
};

#endif //__AGSMSKILL_H__