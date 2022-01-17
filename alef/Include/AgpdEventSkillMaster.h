#ifndef	__AGPDEVENTSKILLMASTER_H__
#define	__AGPDEVENTSKILLMASTER_H__

#include "ApBase.h"

const int	AGPMEVENT_SKILL_MAX_MASTERY				= 5;	// ## Product
const int	AGPMEVENT_SKILL_MAX_MASTERY_NAME		= 16;
const int	AGPMEVENT_SKILL_MAX_MASTERY_SKILL		= 64;
const int	AGPMEVENT_SKILL_MAX_HIGHLEVEL_CONDITION = 4;
const int	AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL		= 11;
const int	AGPMEVENT_SKILL_MAX_HEROIC_CONDITION	= 4;

const int	AGPMEVENT_SKILL_MAX_LEVEL				= 16;

const int	AGPMEVENT_SKILL_ARCHLORD_MASTERY_INDEX	= 100;
const int	AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX = 6;
const int	AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX	= 7;

typedef struct	_AgpmEventSkillMastery {
	//INT32	lSkillTID[AGPMEVENT_SKILL_MAX_MASTERY_SKILL];
	ApSafeArray<INT32, AGPMEVENT_SKILL_MAX_MASTERY_SKILL>	lSkillTID;
} AgpdEventSkillMastery;

typedef struct	_AgpmEventSkillCost {
	INT32	lCostMoney;
	INT32	lCostSkillPoint;
	INT32   lCostHeroicPoint;
	INT32   lCostCharismaPoint;
} AgpdEventSkillCost;

class AgpdEventSkillAttachTemplateData {
public:
	//AgpdEventSkillCost	m_stSkillCost[AGPMEVENT_SKILL_MAX_LEVEL];
	ApSafeArray<AgpdEventSkillCost, AGPMEVENT_SKILL_MAX_LEVEL>	m_stSkillCost;

	INT32	lMaxUpgradeLevel;
	INT32	lMasteryIndex;
};

class AgpdSkillEventAttachData {
public:
	AuRaceType			eRaceType;
	AuCharClassType		eClassType;
};

class AgpdEventSkillAttachCharTemplateData {
public:
	INT32				m_lNumDefaultSkill;
	AgpdSkillTemplate**	m_apcsDefaultSkillTemplate;
};

struct AgpdEventSkillHighLevelCondition
{
	INT32		m_lSkillTID;
	INT32		m_lSkillLevel;
	INT32		m_lSkillPoint;
	INT32		m_lSkillTab;
	INT32		m_lSkillTotalPoint;
};

struct AgpdEventSkillHighLevel
{
	INT32		m_lCharTID;
	INT32		m_lSkillTID;
	INT32		m_lCharRace;
	AgpdEventSkillHighLevelCondition	m_stCondition[AGPMEVENT_SKILL_MAX_HIGHLEVEL_CONDITION];

	bool operator== (INT32 lSkillTID)
	{
		if(lSkillTID == m_lSkillTID)
			return true;
		
		return false;
	}
};

struct AgpdEventSkillHeroicCondition
{
	INT32		m_lSkillTID;
	INT32		m_lSkillLevel;
	INT32		m_lUsedTotalHeroicPoint;
	
	AgpdEventSkillHeroicCondition()
	{
		m_lSkillTID = 0;
		m_lSkillLevel = 0;
		m_lUsedTotalHeroicPoint = 0;
	}
};

struct AgpdEventSkillHeroic
{
	INT32		m_lCharTID;
	INT32		m_lSkillTID;
	AgpdEventSkillHeroicCondition	m_stCondition[AGPMEVENT_SKILL_MAX_HEROIC_CONDITION];

	bool operator== (INT32 lSkillTID)
	{
		if(lSkillTID == m_lSkillTID)
			return true;

		return false;
	}
};
#endif	//__AGPMEVENTSKILLMASTER_H__
