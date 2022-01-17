#ifndef __AGPDAI2_H__
#define __AGPDAI2_H__

#define AGPDAI2_MAX_USABLE_ITEM_COUNT		10
#define AGPDAI2_MAX_USABLE_SKILL_COUNT		10
#define AGPDAI2_MAX_RACE_CLASS_ADJUST		10
#define AGPDAI2_MAX_ACTION_ADJUST			10
#define AGPDAI2_MAX_FELLOW_WORKER_COUNT		5
#define AGPDAI2_MAX_FOLLOWER_COUNT			10
#define AGPDAI2_MAX_PREEMPTIVECONDITON		3

#include "SpecializeListPathFind.h"
#include "AgpmCharacter.h"
#include "AgpmPathFind.h"

typedef enum
{
	AGPDAI2_ACTION_ADJUST_NONE = 0,
	AGPDAI2_ACTION_ADJUST_MELEE_ATTACK,
	AGPDAI2_ACTION_ADJUST_MELEE_ATTACK_TRY,
	AGPDAI2_ACTION_ADJUST_BUFF,
	AGPDAI2_ACTION_ADJUST_BUFF_TRY,
	AGPDAI2_ACTION_ADJUST_DEBUFF,
	AGPDAI2_ACTION_ADJUST_DEBUFF_TRY,
} eAgpdAI2ActionAdjust;

enum eSearchEnemy
{
	AGPDAI2_SEARCH_ENEMY_NONE = 0,
	AGPDAI2_SEARCH_ENEMY_RUSH1,
	AGPDAI2_SEARCH_ENEMY_RUSH2,
	AGPDAI2_SEARCH_ENEMY_IN_CASTLE,
	AGPDAI2_SEARCH_ENEMY_MAX
};

enum eStopFight
{
	AGPDAI2_STOP_FIGHT_NONE = 0,
	AGPDAI2_STOP_FIGHT_NEAR,
	AGPDAI2_STOP_FIGHT_NORMAL,
	AGPDAI2_STOP_FIGHT_MAX
};

enum eEscape
{
	AGPDAI2_ESCAPE_NONE = 0,
	AGPDAI2_ESCAPE_ESCAPE,
	AGPDAI2_ESCAPE_RESPONSIVE_ATTACK,
	AGPDAI2_ESCAPE_MAX
};

enum eAgpmAI2StatePCAI
{
	AGPDAI2_STATE_PC_AI_NONE = 0,
	AGPDAI2_STATE_PC_AI_HOLD,
	AGPDAI2_STATE_PC_AI_CONFUSION,
	AGPDAI2_STATE_PC_AI_FEAR,
	AGPDAI2_STATE_PC_AI_BERSERK,
	AGPDAI2_STATE_PC_AI_SHRINK,
	AGPDAI2_STATE_PC_AI_DISEASE,
	AGPDAI2_STATE_PC_AI_MAX
};

class AgpdAI2ConditionTable
{
public:
	INT32			m_lAggroParameter;
	BOOL			m_bAggroAccept;

	INT32			m_lTargetParameter;
	INT32			m_lParameter;
	BOOL			m_bPercent;
	INT32			m_lOperator;
	INT32			m_lConditionCheck;
	INT32			m_lPrevProcessTime;
	INT32			m_lTimerCount;
	INT32			m_lProbable;
	
	INT32			m_lMaxUsableCount;

	AgpdAI2ConditionTable()
	{
		m_lAggroParameter = 0;
		m_bAggroAccept = FALSE;

		m_lTargetParameter = 0;
		m_lParameter = 0;
		m_bPercent = 0;
		m_lOperator = 0;
		m_lConditionCheck = 0;
		m_lPrevProcessTime = 0;
		m_lTimerCount = 0;
		m_lProbable = 0;

		m_lMaxUsableCount = 0;
	}
};

//AI사용 결과를 가지고 있는 클래스
class AgpdAI2UseResult
{
public:
	//아이템, 스킬 사용에 대한 몬스터 로그.
	INT32				m_lUsedCount;
	INT32				m_lClock;

	AgpdAI2UseResult()
	{
		m_lUsedCount = 0;
		m_lClock = 0;
	}
};

class AgpdAI2FellowWorkerInfo
{
public:
	INT32					m_lFellowWorkerTID;			//동종 업종 몹 TID
	char					m_strFellowWorkerName[80];	//동종 업종 몹 이름

	AgpdAI2FellowWorkerInfo()
	{
		m_lFellowWorkerTID = 0;
		memset( m_strFellowWorkerName, 0, sizeof(m_strFellowWorkerName) );
	}
};

// 주변에서 같이 일할 몬스터들을 불러드릴때 사용하는 클래스
// 그러므로 꼭 몬스터 TID를 지정해서 할필요는 없다.
class AgpdAI2FellowWorker
{
public:
	INT32					m_lFellowWorkerTotalCount;
	INT32					m_lRange;					//스크림 거리
	INT32					m_lCount;					//스크림시 몬스터의 수

	AgpdAI2FellowWorkerInfo	m_csFellowWorker[AGPDAI2_MAX_FELLOW_WORKER_COUNT];
	AgpdAI2FellowWorkerInfo	m_csFellowWorkerInfo;

	AgpdAI2FellowWorker()
	{
		m_lFellowWorkerTotalCount = 0;
		m_lRange = 0;
		m_lCount = 0;
	}
};

class AgpdAI2FollowerInfo
{
public:
	INT32					m_lFollowerTID;
	INT32					m_lMaxFollowerCount;

	AgpdAI2FollowerInfo()
	{
		m_lFollowerTID = 0;
		m_lMaxFollowerCount = 0;
	}
};

class AgpdAI2Follower
{
public:
	INT32					m_lFollowerTotalCount;
	//AgpdAI2FollowerInfo		m_lFollowerInfo[AGPDAI2_MAX_FOLLOWER_COUNT];
	ApSafeArray<AgpdAI2FollowerInfo, AGPDAI2_MAX_FOLLOWER_COUNT>	m_lFollowerInfo;
	AgpdAI2FollowerInfo		m_csFollowerInfo;

	INT32					m_lMaxCumulativeFollower;

	INT32					m_lMinUpkeepFollower;

	AgpdAI2Follower()
	{
		m_lFollowerTotalCount = 0;
		m_lMaxCumulativeFollower = 0;
		m_lMinUpkeepFollower = 0;

		m_lFollowerInfo.MemSetAll();
	}
};

class AgpdAI2ActionAdjustInfo
{
public:
	eAgpdAI2ActionAdjust	m_eAction;
	INT32					m_lHateRate;

	AgpdAI2ActionAdjustInfo()
	{
		m_eAction = AGPDAI2_ACTION_ADJUST_NONE;
		m_lHateRate = 0;
	}
};

class AgpdAI2ActionAdjust
{
public:
	ApSafeArray<AgpdAI2ActionAdjustInfo, AGPDAI2_MAX_ACTION_ADJUST>	m_csInfo;

	AgpdAI2ActionAdjust()
	{
		m_csInfo.MemSetAll();
	}
};

class AgpdAI2RaceClassAdjustInfo
{
public:
	INT32			m_lRace;
	INT32			m_lClass;
	INT32			m_lHateRate;
	INT32			m_lHateClassRate;

	AgpdAI2RaceClassAdjustInfo()
	{
		m_lRace = 0;
		m_lClass = 0;
		m_lHateRate = 100;
		m_lHateClassRate = 100;
	}
};

class AgpdAI2RaceClassAdjust
{
public:
	ApSafeArray<AgpdAI2RaceClassAdjustInfo, AGPDAI2_MAX_RACE_CLASS_ADJUST>	m_csInfo;

	AgpdAI2RaceClassAdjust()
	{
		m_csInfo.MemSetAll();
	}
};

class AgpdAI2ClassAdjust
{
public:
	INT32			m_lWarrior;
	INT32			m_lArcher;
	INT32			m_lMage;
	INT32			m_lMonk;

	AgpdAI2ClassAdjust()
	{
		m_lWarrior = 100;
		m_lArcher = 100;
		m_lMage = 100;
		m_lMonk = 100;
	}
};

class AgpdAI2AgroActionAdjust
{
public:
	INT32			m_lDamage;
	INT32			m_lTryAttack;
	INT32			m_lDebuff;
	INT32			m_lTryDebuff;
	INT32			m_lP2PHeal;
	INT32			m_lP2PBuff;

	AgpdAI2AgroActionAdjust()
	{
		m_lDamage = 100;
		m_lTryAttack = 100;
		m_lDebuff = 100;
		m_lTryDebuff = 100;
		m_lP2PHeal = 100;
		m_lP2PBuff = 100;
	}
};

class AgpdAI2Agro
{
public:
	INT32			m_lTargetCID;

	INT32			m_lStaticAgro;
	INT32			m_lFastAgro;
	INT32			m_lSlowAgro;
	INT32			m_lBuffAgro;

	AgpdAI2Agro()
	{
		m_lTargetCID = 0;

		m_lStaticAgro = 0;
		m_lFastAgro = 0;
		m_lSlowAgro = 0;
		m_lBuffAgro = 0;
	}
};

class AgpdAI2UseItem
{
public:
	AgpdAI2ConditionTable	m_csTable;

	INT32			m_lItemID;

	AgpdAI2UseItem()
	{
		m_lItemID = 0;
	}
};

class AgpdAI2SkillSimpleCondition
{
public:
	INT32		m_lHPLimit;
	UINT32		m_lSkillCount;

	AgpdAI2SkillSimpleCondition()
	{
		m_lHPLimit = 0;
		m_lSkillCount = 0;
	}
};

class AgpdAI2UseSkill
{
public:
	AgpdAI2ConditionTable	m_csTable;
	AgpdAI2SkillSimpleCondition m_csSkillCondition;

	INT32			m_lSkillID;
	INT32			m_lSkillLevel;

	AgpdAI2UseSkill()
	{
		m_lSkillID = 0;
		m_lSkillLevel = 0;
	}
};

class AgpdAI2Preemptive
{
public:
	AgpdAI2ConditionTable	m_csTable;

	BOOL					m_bSet;

	AgpdAI2Preemptive()
	{
		m_bSet = FALSE;
	}
};

class AgpdAI2RPreemptive
{
public:
	INT32				m_lPreemptiveCondition1;
	INT32				m_lPreemptiveCondition2;
	INT32				m_lPreemptiveCondition3;

	AgpdAI2RPreemptive()
	{
		m_lPreemptiveCondition1 = 0;
		m_lPreemptiveCondition2 = 0;
		m_lPreemptiveCondition3 = 0;
	}
};

class AgpdAI2Summon
{
public:
	ApVector<INT32, 5> m_vtTID;
	INT32			m_lMaxCount;

	AgpdAI2Summon()
	{
		m_vtTID.clear();
		m_lMaxCount		= 0;
	}
};

class AgpdAI2HideInfo
{
public:
	UINT32					m_lHideCount;
	UINT32					m_lHideMonsterHP;
	UINT32					m_uIHideMonsterTime;

	AgpdAI2HideInfo()
	{
		m_lHideCount =0;
		m_lHideMonsterHP = 0;
		m_uIHideMonsterTime = 0;
	}
};

class AgpdAI2EscapeInfo
{
public:
	UINT32					m_lEscapeHP;
	UINT32					m_uIEscapeTime;
	UINT32					m_ulEscapeCount;

	AgpdAI2EscapeInfo()
	{
		m_lEscapeHP = 0;
		m_uIEscapeTime = 0;
		m_ulEscapeCount = 0;
	}
};

class AgpdAI2LootItemInfo
{
public:
	float					m_fLootItemRange;
	UINT32					m_uILootItemTime;

	AgpdAI2LootItemInfo()
	{
		m_fLootItemRange = 0.0f;
		m_uILootItemTime = 0;
	}
};

class AgpdAI2LootItem
{
public:
	UINT32					m_ulItemID;
	UINT32					m_ulGetItemTime;

	AgpdAI2LootItem()
	{
		m_ulItemID = 0;
		m_ulGetItemTime = 0;
	}
};

typedef enum eAgpmAI2Type
{
	AGPMAI2_TYPE_MOB = 0,
	AGPMAI2_TYPE_FIXED_NPC,
	AGPMAI2_TYPE_PATROL_NPC
} AgpmAI2Type;

typedef enum eAgpmAI2PinchType
{
	AGPMAI2_TYPE_PINCH_NONE = 0,
	AGPMAI2_TYPE_PINCH_MONSTER,
	AGPMAI2_TYPE_PINCH_WANTED
}AgpmAI2PinchType;

class AgpdAI2Pinch
{
public:
	AgpmAI2PinchType		ePinchType;
	AgpdCharacter			*pcsPinchCharcter;
	vector<AgpdCharacter*>	pRequestMobVector;

	AgpdAI2Pinch()
	{
		ePinchType = AGPMAI2_TYPE_PINCH_NONE;
		pcsPinchCharcter = NULL;
		pRequestMobVector.clear();
	}
};

class AgpdAI2Template
{
public:
	char					m_strTemplateName[80];
	BOOL					m_bUseNormalAttack;
	BOOL					m_bDontMove;

	//AI Type. 고정형 NPC, 패트롤 NPC, 몬스터
	eAgpmAI2Type			m_eNPCAI2Type;

	INT32					m_lTID;
	INT32					m_lAgressivePoint;

	INT32					m_lPreemptiveType;
	float					m_fPreemptiveRange;

	AgpdAI2FellowWorker		m_csFellowWorker;
	AgpdAI2Follower			m_csFollower;
	AgpdAI2ActionAdjust		m_csActionAdjust;
	AgpdAI2RaceClassAdjust	m_csRaceClassAdjust;

	eSearchEnemy			m_eSearchEnemy;
	eStopFight				m_eStopFight;
	eEscape					m_eEscape;

	//아래는 안쓰게 될것이다. 시간나는대로 삭제!
	AgpdAI2ClassAdjust		m_csClassAdjust;

	AgpdAI2AgroActionAdjust	m_csAgroActionAdjust;

	/* 요 세가지가 문제*/
	ApSafeArray<AgpdAI2UseItem, AGPDAI2_MAX_USABLE_ITEM_COUNT>		m_csUseItem;
	
	ApSafeArray<AgpdAI2UseSkill, AGPDAI2_MAX_USABLE_SKILL_COUNT>	m_csUseSkill;

	ApSafeArray<AgpdAI2Preemptive, AGPDAI2_MAX_PREEMPTIVECONDITON>	m_csPreemptiveCondition; 

	AgpdAI2RPreemptive		m_csRPreemptiveCondition;	// Preemptive Condition은 이걸로 간단하게 변경

	AgpdAI2HideInfo			m_csMonsterHideInfo;
	AgpdAI2EscapeInfo		m_csMonsterEscapeInfo;
	AgpdAI2LootItemInfo		m_csLootItemInfo;

	INT32					m_lLinkMonsterTID;
	float					m_fLinkMonsterSight;

	float					m_fMaintenanceRange;
	BOOL					m_bRandomTarget;
	
	// 스크림 재사용 시간 설정
	UINT32					m_ulFellowWorkerRefreshTime;

	UINT32					m_ulPinchMonsterTID;
	float					m_fVisibility;

	AgpdAI2Summon			m_csSummon;

	AgpdAI2Template()
	{
		memset( m_strTemplateName, 0, sizeof( m_strTemplateName ) );

		m_bUseNormalAttack = FALSE;
		m_bDontMove = FALSE;
		m_eNPCAI2Type = AGPMAI2_TYPE_MOB;
		m_lTID = 0;
		m_lAgressivePoint = 0;
		m_ulPinchMonsterTID = 0;

		m_lPreemptiveType = 0;
		m_fPreemptiveRange = 0.0f;

		m_csUseItem.MemSetAll();
		m_csUseSkill.MemSetAll();
		m_csPreemptiveCondition.MemSetAll();

		m_eSearchEnemy = AGPDAI2_SEARCH_ENEMY_NONE;
		m_eStopFight = AGPDAI2_STOP_FIGHT_NONE;
		m_eEscape = AGPDAI2_ESCAPE_NONE;

		m_lLinkMonsterTID = 0;
		m_fLinkMonsterSight = 0.0f;
		m_fMaintenanceRange = 0.0f;
		m_fVisibility = 0.0f;
		m_bRandomTarget = FALSE;
	}
};

struct stAgpdAI2PCAI
{
	INT32	lTargetID;			// 시전자 TargetID (AI를 걸려들게 만든 장본인)
	INT32	lAICount;				// AI실행한 횟수		
	AuPOS	stFirstPOS;			// 최초 스킬 걸린 위치
	eAgpmAI2StatePCAI eStateAI;	// 어떤 State의 AI인지

	INT32	lReserveParam1;		// AI에 필요한 Reserve Param 1
	INT32	lReserveParam2;		// AI에 필요한 Reserve Param 2
	UINT32	lReserveParam3;		// AI에 필요한 Reserve Param 3 (보통 시간에 관련된거에 사용)

	stAgpdAI2PCAI()
		: lTargetID(0), lAICount(0), lReserveParam1(0), lReserveParam2(0), lReserveParam3(0)
	{
		ZeroMemory(&stFirstPOS, sizeof(stFirstPOS));
		eStateAI = AGPDAI2_STATE_PC_AI_NONE;
	}
};

typedef std::vector<AgpdAI2LootItem> CLootItemVector;

class AgpdAI2ADChar
{
public:
	AgpdAI2Template		*m_pcsAgpdAI2Template;

	//PathFinding 결과를 저장할 List
	CSpecializeListPathFind<int, AgpdPathFindPoint *> *m_pclPath;

	UINT32				m_ulLastAttackTime;
	INT32				m_lAttackInterval;

	UINT32				m_ulPrevProcessTime;
	UINT32				m_ulProcessInterval;
	UINT32				m_ulLastPathFindTime;

	UINT32				m_ulNextWanderingTime;
	UINT32				m_ulNextDialogTargetPC;

	UINT32				m_ulStartRunawayTime;
	UINT32				m_ulEscapeCount;

	UINT32				m_ulStartHideTime;
	UINT32				m_ulHideCount;

	//아이템 사용 결과
	ApSafeArray<AgpdAI2UseResult, AGPDAI2_MAX_USABLE_ITEM_COUNT>	m_csItemResult;

	//스킬 사용 결과
	ApSafeArray<AgpdAI2UseResult, AGPDAI2_MAX_USABLE_SKILL_COUNT>	m_csSkillResult;

	//스크림 사용결과
	BOOL				m_bScreamUsed;
	UINT32				m_ulScreamStartTime;

	//시간이 지남에 따라 적대치를 줄이기위한 타이머.
	UINT32				m_ulNextDecStaticTime;
	UINT32				m_ulNextDecSlowTime;
	UINT32				m_ulNextDecFastTime;

	AuPOS				m_csTargetPos;

	AuPOS				m_csFirstSpawnPos;

	BOOL				m_bUseAI2;
	AuPOS				m_csSpawnBasePos;
	float				m_fSpawnRadius;

	//적ID
	INT32				m_lTargetID;
	INT32				m_lReadyTargetID;	// 2005.08.30. steeple

	BOOL				m_bIsNeedPathFind;

	//적을 둘러쌀때 위치
	INT32				m_lWrapPosition;

	//보스(교주)몹의 포인터.
	AgpdCharacter		*m_pcsBossCharacter;

	//신도몹의 포인터
	ApSafeArray<AgpdCharacter *, AGPDAI2_MAX_FOLLOWER_COUNT>	m_pcsFollower;

	INT32				m_lCurrentFollowers;			//현재 내 부하의 수.
	INT32				m_lCurrentCumulativeFollower;	//현재까지 부하로 만든 몬스터의 수.

	INT32				m_lCastSkillLevel;		// Spawn 될 때 Cast 하는 Passive 나 Self Buff Skill 의 Level. 2005.09.06. steeple
	CLootItemVector		m_vLootItem;

	AgpdAI2Pinch		m_csPinchInfo;

	stAgpdAI2PCAI		m_stPCAIInfo;			// Heroic Skill로 인한 PC AI를 위한 간단한 구조체

	AgpdAI2ADChar()
	{
		m_lCurrentFollowers = 0;
		m_lCurrentCumulativeFollower = 0;
		m_ulStartRunawayTime = 0;
		m_ulEscapeCount = 0;
		m_ulStartHideTime = 0;
		m_ulHideCount = 0;
	}
	~AgpdAI2ADChar()
	{
	}
};

#endif
