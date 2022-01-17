/******************************************************************************
Module:  AgsdSkill.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 03. 24
******************************************************************************/

#if !defined(__AGSDSKILL_H__)
#define __AGSDSKILL_H__

#include "ApBase.h"

#include "AsDefine.h"
#include <atltime.h>

#define	AGSMSKILL_MAX_TEMPLATE_SKILL_BUFFER			10
#define AGSMSKILL_MAX_SKILLTREE_LENGTH				512

typedef struct _stMasteryInfo {
	CHAR		szSkillTree[AGSMSKILL_MAX_SKILLTREE_LENGTH + 1];
	CHAR		szSpecialize[33];
	CHAR		szProductCompose[AGSMSKILL_MAX_SKILLTREE_LENGTH + 1];	// compose list of product skill
} stMasteryInfo;

// 2005.09.15. steeple
typedef enum _eAgsdSkillMissedReason
{
	AGSDSKILL_MISSED_REASON_NORMAL = 0,
	AGSDSKILL_MISSED_REASON_DURATION_UNLIMITED,
	AGSDSKILL_MISSED_REASON_TARGET_NUM_ZERO,
	AGSDSKILL_MISSED_REASON_CANNOT_TRANSPARENT_FOR_COMBAT,
	AGSDSKILL_MISSED_REASON_TAME_FAILURE_ABNORMAL,
} eAgsdSkillMissedReason;

// 2007.10.23. steeple
typedef enum _eAgsdSkillConnectionType
{
	AGSDSKILL_CONNECTION_TYPE_NONE			= 0x00,
	AGSDSKILL_CONNECTION_TYPE_DAMAGE		= 0x01,
	AGSDSKILL_CONNECTION_TYPE_BUFF			= 0x02,
} eAgsdSkillConnectionType;

typedef enum _eAgsdSkillConnectionConditionType
{
	AGSDSKILL_CONNECTION_CONDITION_TYPE_NONE		= 0x00,
	AGSDSKILL_CONNECTION_CONDITION_TYPE_BUFF		= 0x01,
	AGSDSKILL_CONNECTION_CONDITION_TYPE_TRANSPARENT	= 0x02,
} eAgsdSkillConnectionConditionType;

// 2007.10.23. steeple
struct AgsdSkillConnectionInfo
{
	BOOL m_bNeedCheck;
	BOOL m_bAuthorized;		// 이미 체크를 통과해서 버프중에도 계속 써주어야 한다.
	eAgsdSkillConnectionType m_eType;

	eAgsdSkillConnectionConditionType m_eConditionType;
	INT32 m_lConditionTID;

	void init()
	{
		m_bNeedCheck = FALSE;
		m_bAuthorized = FALSE;
		m_eType = AGSDSKILL_CONNECTION_TYPE_NONE;
		m_eConditionType = AGSDSKILL_CONNECTION_CONDITION_TYPE_NONE;
		m_lConditionTID = 0;
	}
};

// 2007.10.26. steeple
// Union Skill 을 Control 하는 Skill 정보
struct stUnionControlInfo
{
	INT32 m_lUnionID;
	INT32 m_lMaxCount;
	INT32 m_lCurrentCount;

	void init()
	{
		m_lUnionID = 0;
		m_lMaxCount = 1;	// 기본값은 1이다.
		m_lCurrentCount = 0;
	}

	bool operator == (INT32 lUnionID)
	{
		return m_lUnionID == lUnionID;
	}
};

#define AGSMSKILL_MAX_UNION_CONTROL_COUNT		5		// 5개면 충분할듯.

struct UnionControlArray
{
	typedef ApSafeArray<stUnionControlInfo, AGSMSKILL_MAX_UNION_CONTROL_COUNT>::iterator	iterator;

	ApSafeArray<stUnionControlInfo, AGSMSKILL_MAX_UNION_CONTROL_COUNT>		m_astUnionControlInfo;
};

class AgsdSkill {
public:
	/*
	UINT32					m_ulStartTime;
	UINT32					m_ulEndTime;
	UINT32					m_ulInterval;
	UINT32					m_ulNextProcessTime;
	UINT32					m_ulRecastDelay;
	*/

	AgpdFactor				*m_pcsUpdateFactorResult;
	AgpdFactor				*m_pcsUpdateFactorPoint;		// factor를 변경시키는 스킬인 경우 변경되는 값을 계산해 넣는다.
	AgpdFactor				*m_pcsUpdateFactorPercent;
	
	AgpdFactor				*m_pcsUpdateFactorItemPoint;
	AgpdFactor				*m_pcsUpdateFactorItemPercent;

														// 나중에 이값을 빼서 원래 값으로 되돌린다.

	//AgpdFactor				m_csModifyFactorPoint[AGPMSKILL_MAX_AFFECTED_BASE];		// factor를 변경시키는 스킬인 경우 변경되는 값을 계산해 넣는다.
	//AgpdFactor				m_csModifyFactorPercent[AGPMSKILL_MAX_AFFECTED_BASE];
														// 나중에 이값을 빼서 원래 값으로 되돌린다.

	INT32					m_lModifySkillLevel;
	INT32					m_lModifyCharLevel;

	INT32					m_nMeleeDefensePoint;			// 실제 방어할 수 있는 데미지
	INT32					m_nMeleeReflectPoint;			// 실제 반사시킬 수 있는 데미지
	INT32					m_nMagicDefensePoint;			// 실제 방어할 수 있는 데미지
	INT32					m_nMagicReflectPoint;			// 실제 반사시킬 수 있는 데미지
	
	INT32					m_lDamageShield;

	AgpdSkillBuffedCombatEffectArg	m_stBuffedSkillCombatEffectArg;				// 이 스킬로 인해 변경된 값들을 저장한다.
	AgpdSkillBuffedFactorEffectArg	m_stBuffedSkillFactorEffectArg;

	BOOL					m_bIsCastSkill;
	INT32					m_lSkillScrollIID;

	UINT32					m_ulLastCastTwiceSkillClock;	// 마지막 캐스트한 스킬 Clock
	ApSafeArray<INT32, 33>	m_alLastTwiceSkillTargetCID;	// 마지막 캐스트할 때 지정한 타겟 배열
	INT32					m_lLastTwiceSkillNumTarget;		// 위의 배열에 들어간 타겟 수

	eAgsdSkillMissedReason	m_eMissedReason;				// 스킬이 실패한 이유 2005.09.15. steeple
	BOOL					m_bApplyDamageOnly;				// 데미지와 디버프가 같이 있는 스킬인데, 데미지만 주는 경우 TRUE

	INT16					m_nAffectedDOTCount;			// 버프 기간동안 적용된 DOT 횟수
	INT16					m_nTotalDOTCount;				// 총 적용해야할 DOT 횟수

	InvincibleInfo			m_stInvincibleInfo;				// 항목별 무적 확률 정보.

	AgsdSkillConnectionInfo	m_stConnectionInfo;				// 어떤 스킬과 연관되어서 추가 효과가 주어지는 정보.
	stUnionControlInfo		m_stUnionInfo;					// 

	ApSafeArray<INT32, AGPMSKILL_MAX_SKILL_LEVELUP_TID>		m_alLevelUpSkillTID;
};

class AgsdSkillProcessInfo {
public:
	/*
	INT32					m_lSkillID;
	AgpdSkillTemplate*		m_pcsTemplate;

	INT32					m_lSkillLevel;

	UINT32					m_tmEnd;
	UINT32					m_tmInterval;
	UINT32					m_tmNextProcess;

	AgpdFactor				m_csModifyFactorPoint;
	AgpdFactor				m_csModifyFactorPercent;
	*/

	ApBaseExLock			m_csSkillOwner;
	INT32					m_lMasteryPoint;

	AgpdSkill*				m_pcsTempSkill;
};

typedef vector<AgsdEquipSkill> CEquipSkillArray;

class AgsdSkillADBase {
public:
//	AgsdSkillProcessInfo	m_csBuffedSkillProcessInfo[AGPMSKILL_MAX_SKILL_BUFF];
	ApSafeArray<AgsdSkillProcessInfo, AGPMSKILL_MAX_SKILL_BUFF>		m_csBuffedSkillProcessInfo;

	INT32					m_lDamageAdjustRate;				// damage adjust skill에 의한 DamageAdjust rate

	stMasteryInfo			*m_pstMasteryInfo;

	// 아이템으로 사용되는.. 혹은 걍 TID와 레벨만으로 사용되는 스킬들을 여기에 세팅해서 사용한다.
//	AgpdSkill				*m_ppcsSkillItem[AGSMSKILL_MAX_TEMPLATE_SKILL_BUFFER];
	ApSafeArray<AgpdSkill *, AGSMSKILL_MAX_TEMPLATE_SKILL_BUFFER>	m_ppcsSkillItem;

	UnionControlArray		m_UnionControlArray;

	UINT32					m_ulLastReleasedTransparentClock;	// 마지막으로 스킬때문에 투명이 플린 클락.
	INT32					m_lReleaseTransparentSkillTID;		// 마지막으로 투명을 풀게한 Skill TID

	CEquipSkillArray		m_csEquipSkillArray;
};

/*
class AgsdSkillTemplate {
public:
	INT32					m_lUsedConstFactor[AGSMSKILL_CONST_MAX][AGSMSKILL_MAX_SKILL_CAP];
														// 스킬 계산에 사용되는 상수들
};
*/

struct AgsdCastSkill
{
	ApBase *pcsBase;
	ApBase *pcsTargetBase;
	INT32 lSkillTID;
	INT32 lSkillLevel;
	INT32 lSkillScrollIID;
	BOOL bForceAttack;
	BOOL bMonsterUse;
	SaveSkillData m_pSaveSkillData;

	AgsdCastSkill()
		: pcsBase(NULL), lSkillTID(0), lSkillLevel(0)
		, pcsTargetBase(NULL), bForceAttack(FALSE), bMonsterUse(FALSE)
		, lSkillScrollIID(AP_INVALID_IID)
	{
		m_pSaveSkillData.Clear();
	}
};

#endif	//__AGSDSKILL_H__