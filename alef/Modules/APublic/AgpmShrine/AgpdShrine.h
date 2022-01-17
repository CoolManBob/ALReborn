/******************************************************************************
Module:  AgpdShrine.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 22
******************************************************************************/

#if !defined(__AGPDSHRINE_H__)
#define __AGPDSHRINE_H__

#include "ApBase.h"
#include "AgpmSkill.h"
#include "AgpmEventSpawn.h"

const int	AGPMSHRINE_MAX_POSITION				= 20;
const int	AGPMSHRINE_MAX_NAME					= 32;

const int	AGPMSHRINE_MAX_LEVEL				= 5;
const int	AGPMSHRINE_MAX_SHRINE				= 40;
const int	AGPMSHRINE_MAX_KILL_GUARDIAN		= 10;

typedef enum _eAgpmShrineUseCondition {
	AGPMSHRINE_USE_ONCE							= 1,
	AGPMSHRINE_USE_WHEN_ACTIVE,
	AGPMSHRINE_USE_TIME_INTERVAL
} eAgpmShrineUseCondition;

typedef enum _eAgpmShrineActiveCondition {
	AGPMSHRINE_ACTIVE_ALWAYS					= 1,
	AGPMSHRINE_ACTIVE_DAY_ONLY,
	AGPMSHRINE_ACTIVE_SPAWN
} eAgpmShrineActiveCondition;

typedef enum _eAgpmShrineReActiveCondition {
	AGPMSHRINE_REACTIVE_NONE					= 1,
	AGPMSHRINE_REACTIVE_DAY_ONLY,
	AGPMSHRINE_REACTIVE_TWICE_LAST_ACTIVE_TIME,
} eAgpmShrineReActiveCondition;

class AgpdShrineTemplate : public ApBase {
public:
	CHAR						m_szShrineName[AGPMSHRINE_MAX_NAME];

	// 이 사원의 능력 (스킬)
	CHAR						m_szSkillName[AGPMSHRINE_MAX_LEVEL][AGPMSKILL_MAX_SKILL_NAME + 1];

	// 랜덤하게 스폰되는 경우인지 아닌지
	BOOL						m_bIsRandomPosition;
	// shrine의 위치들... 정해져있는경우 m_posShrine[0], 그렇지 않은경우는  m_nNumShrinePos 만큼..
	AuPOS						m_posShrine[AGPMSHRINE_MAX_POSITION];
	INT32						m_nNumShrinePos;

	// 이 사원을 사용하는 방법.. (한번인지, 활성화 될때마다인지, 주어진 시간 간격인지)
	eAgpmShrineUseCondition		m_eUseCondition;

	UINT32						m_ulUseIntervalTimeHR;

	// 활성화 조건.. (항상, 낮에만, 스폰)
	eAgpmShrineActiveCondition	m_eActiveCondition;

	UINT32						m_ulMinActiveTimeHR;
	UINT32						m_ulMaxActiveTimeHR;

	// 다시 활성활될 조건
	eAgpmShrineReActiveCondition	m_eReActiveCondition;

	UINT32						m_ulReActiveTimeHR;

	// 이 사원의 가디언 스폰 이름
	CHAR						m_szGuardianSpawnName[AGPMSHRINE_MAX_LEVEL][AGPDSPAWN_SPAWN_NAME_LENGTH];

	// sigil data들...
	//		m_csSigilAttrFactor	: status, magic defense 중 이 sigil이 갖는 값을 세팅한다.
	//			- 0이 아닌값인경우 %의 확률로 해당 속성값을 갖는다.
	//		m_csMinValueFactor : 위 확률에 의해 속성을 갖는다고 계산이 되었을때 속성값의 최소치
	//		m_csMaxValueFactor : 위 확률에 의해 속성을 갖는다고 계산이 되었을때 속성값의 최대치
	BOOL						m_bIsSigil;
	AgpdFactor					m_csSigilAttrFactor;
	AgpdFactor					m_csMinValueFactor;
	AgpdFactor					m_csMaxValueFactor;
};

class AgpdShrine : public ApBase {
public:
	AuPOS						m_posShrine;

	BOOL						m_bIsActive;

	UINT32						m_ulLastActiveTimeMSec;
	UINT32						m_ulLastInactiveTimeMSec;

	UINT32						m_ulLastActiveDurationMSec;

	AgpdShrineTemplate*			m_pcsTemplate;

	ApBase*						m_pcsBase;

	INT32						m_lSkillID[AGPMSHRINE_MAX_LEVEL];

	AgpdFactor					m_csSigilFactor;
	INT32						m_lSigilOwnerUnion;
	ApBase						m_csSigilOwnerBase;
};

class AgpdShrineADChar {
public:
	BOOL						m_bIsGuardian;

	INT32						m_lShrineID;
	INT32						m_lGuardianLevel;

	// 가드언 죽인 정보
	INT32						m_lKillGuardianNum[AGPMSHRINE_MAX_SHRINE][AGPMSHRINE_MAX_LEVEL];
	UINT32						m_ulKillGuardianTime[AGPMSHRINE_MAX_SHRINE][AGPMSHRINE_MAX_LEVEL][AGPMSHRINE_MAX_KILL_GUARDIAN];

	// shrine 사용 정보
	BOOL						m_bUseShrineNum[AGPMSHRINE_MAX_SHRINE][AGPMSHRINE_MAX_LEVEL];
	UINT32						m_ulUseShrineTimeMSec[AGPMSHRINE_MAX_SHRINE][AGPMSHRINE_MAX_LEVEL];
};

class AgpdShrineADObject {
public:
	CHAR						m_szShrineName[AGPMSHRINE_MAX_NAME];
	INT32						m_lShrineID;
};

class AgpdShrineADSpawn {
public:
	// spawn 되는 사원 정보
	CHAR						m_szShrineName[AGPMSHRINE_MAX_NAME];
	INT32						m_lShrineID;

	// 위 사원에서 스폰이 되는데 어느 레벨로 스폰이 되는지...
	INT32						m_lShrineLevel;
};

#endif //__AGPDSHRINE_H__
