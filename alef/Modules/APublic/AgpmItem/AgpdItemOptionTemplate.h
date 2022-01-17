#if !defined(__AGPDITEMOPTIONTEMPLATE_H__)
#define __AGPDITEMOPTIONTEMPLATE_H__

#include "ApBase.h"
#include "AgpmFactors.h"

typedef enum
{
	AGPDITEM_OPTION_PART_BODY	= 0x00,
	AGPDITEM_OPTION_PART_LEGS,
	AGPDITEM_OPTION_PART_WEAPON,
	AGPDITEM_OPTION_PART_SHIELD,
	AGPDITEM_OPTION_PART_HEAD,
	AGPDITEM_OPTION_PART_RING,
	AGPDITEM_OPTION_PART_NECKLACE,
	AGPDITEM_OPTION_PART_FOOTS,
	AGPDITEM_OPTION_PART_HANDS,
    AGPDITEM_OPTION_PART_REFINERY,
    AGPDITEM_OPTION_PART_GACHA,
	AGPDITEM_OPTION_MAX_PART,
} AgpdItemOptionPart;

typedef enum
{
	AGPDITEM_OPTION_SET_DROP		= 0,
	AGPDITEM_OPTION_SET_REFINERY,
	AGPDITEM_OPTION_SET_ALL,
} AgpdItemOptionSetType;

#define	AGPDITEM_OPTION_SET_TYPE_BODY				(1 << AGPDITEM_OPTION_PART_BODY		)
#define	AGPDITEM_OPTION_SET_TYPE_LEGS				(1 << AGPDITEM_OPTION_PART_LEGS		)
#define	AGPDITEM_OPTION_SET_TYPE_WEAPON				(1 << AGPDITEM_OPTION_PART_WEAPON	)
#define	AGPDITEM_OPTION_SET_TYPE_SHIELD				(1 << AGPDITEM_OPTION_PART_SHIELD	)
#define	AGPDITEM_OPTION_SET_TYPE_HEAD				(1 << AGPDITEM_OPTION_PART_HEAD		)
#define	AGPDITEM_OPTION_SET_TYPE_RING				(1 << AGPDITEM_OPTION_PART_RING		)
#define	AGPDITEM_OPTION_SET_TYPE_NECKLACE			(1 << AGPDITEM_OPTION_PART_NECKLACE	)
#define	AGPDITEM_OPTION_SET_TYPE_FOOTS				(1 << AGPDITEM_OPTION_PART_FOOTS	)
#define	AGPDITEM_OPTION_SET_TYPE_HANDS				(1 << AGPDITEM_OPTION_PART_HANDS	)
#define	AGPDITEM_OPTION_SET_TYPE_REFINERY			(1 << AGPDITEM_OPTION_PART_REFINERY	)
#define	AGPDITEM_OPTION_SET_TYPE_GACHA				(1 << AGPDITEM_OPTION_PART_GACHA	)


#define	AGPDITEM_OPTION_MAX_NUM						5
#define AGPDITEM_LINK_MAX_NUM						10
const INT8 AGPDITEM_OPTION_RUNE_MAX_NUM	= AGPDITEM_OPTION_MAX_NUM *AGPDITEM_LINK_MAX_NUM;

#define AGPMITEM_OPTION_MAX_DESCRIPTION				64

typedef enum
{
	AGPDITEM_OPTION_SKILL_TYPE_CRITICAL = 1,
	AGPDITEM_OPTION_SKILL_TYPE_STUN,
	AGPDITEM_OPTION_SKILL_TYPE_DMG_CONVERT_HP,
	AGPDITEM_OPTION_SKILL_TYPE_REGEN_HP,
	AGPDITEM_OPTION_SKILL_TYPE_DOT,
	AGPDITEM_OPTION_SKILL_TYPE_EVADE,
	AGPDITEM_OPTION_SKILL_TYPE_MAX,
} AgpdItemOptionSkillType;

struct stOptionSkillData
{
	// 2006.12.01. steeple
	// Skill Data
	INT32				m_lCriticalRate;
	INT32				m_lCritical;

	INT32				m_lStunRate;
	UINT32				m_ulStunTime;

	INT32				m_lDamageConvertHPRate;
	INT32				m_lDamageConvertHP;

	INT32				m_lRegenHP;
	INT32				m_lRegenMP;

	INT32				m_lDotDamageRate;
	UINT32				m_ulDotDamageTime;
	INT32				m_lDotDamage;

	INT32				m_lSkillLevelUp;
	INT32				m_lBonusExp;
	INT32				m_lBonusMoney;
	INT32				m_lBonusDropRate;
	INT32				m_lBonusDropRate2;
	INT32				m_lBonusCharismaRate;

	INT32				m_lIgnorePhysicalDefenseRate;
	INT32				m_lIgnoreAttributeDefenseRate;
	INT32				m_lCriticalDefenseRate;

	stOptionSkillData()
	{
		init();
	}

	void init()
	{
		m_lCriticalRate = m_lCritical = 0;
		m_lStunRate = m_lDamageConvertHPRate = m_lDamageConvertHP = 0;
		
		m_lRegenHP = 0;
		m_lRegenMP = 0;
		
		m_lDotDamageRate = m_lDotDamage = 0;

		m_ulStunTime = m_ulDotDamageTime = 0;

		m_lSkillLevelUp = m_lBonusExp = m_lBonusMoney = m_lBonusDropRate = 0;

		m_lIgnorePhysicalDefenseRate = m_lIgnoreAttributeDefenseRate = m_lCriticalDefenseRate = 0;
	}

	stOptionSkillData& operator += (const stOptionSkillData& data)
	{
		m_lCriticalRate += data.m_lCriticalRate;
		m_lCritical += data.m_lCritical;

		m_lStunRate += data.m_lStunRate;
		m_ulStunTime += data.m_ulStunTime;

		m_lDamageConvertHPRate += data.m_lDamageConvertHPRate;
		m_lDamageConvertHP += data.m_lDamageConvertHP;

		m_lRegenHP += data.m_lRegenHP;
		m_lRegenMP += data.m_lRegenMP;

		m_lDotDamageRate += data.m_lDotDamageRate;
		m_ulDotDamageTime += data.m_ulDotDamageTime;
		m_lDotDamage += data.m_lDotDamage;

		m_lSkillLevelUp += data.m_lSkillLevelUp;
		m_lBonusExp += data.m_lBonusExp;
		m_lBonusMoney += data.m_lBonusMoney;
		m_lBonusDropRate += data.m_lBonusDropRate;
		m_lBonusCharismaRate += data.m_lBonusCharismaRate;

		m_lIgnorePhysicalDefenseRate += data.m_lIgnorePhysicalDefenseRate;
		m_lIgnoreAttributeDefenseRate += data.m_lIgnoreAttributeDefenseRate;
		m_lCriticalDefenseRate += data.m_lCriticalDefenseRate;

		return *this;
	}

	stOptionSkillData& operator -= (const stOptionSkillData& data)
	{
		m_lCriticalRate -= data.m_lCriticalRate;
		m_lCritical -= data.m_lCritical;

		m_lStunRate -= data.m_lStunRate;
		m_ulStunTime -= data.m_ulStunTime;

		m_lDamageConvertHPRate -= data.m_lDamageConvertHPRate;
		m_lDamageConvertHP -= data.m_lDamageConvertHP;

		m_lRegenHP -= data.m_lRegenHP;
		m_lRegenMP -= data.m_lRegenMP;

		m_lDotDamageRate -= data.m_lDotDamageRate;
		m_ulDotDamageTime -= data.m_ulDotDamageTime;
		m_lDotDamage -= data.m_lDotDamage;

		m_lSkillLevelUp -= data.m_lSkillLevelUp;
		m_lBonusExp -= data.m_lBonusExp;
		m_lBonusMoney -= data.m_lBonusMoney;
		m_lBonusDropRate -= data.m_lBonusDropRate;
		m_lBonusCharismaRate -= data.m_lBonusCharismaRate;

		m_lIgnorePhysicalDefenseRate -= data.m_lIgnorePhysicalDefenseRate;
		m_lIgnoreAttributeDefenseRate -= data.m_lIgnoreAttributeDefenseRate;
		m_lCriticalDefenseRate -= data.m_lCriticalDefenseRate;

		return *this;
	}
};

class AgpdItemOptionTemplate : public ApBase {
public:
	AgpdFactor			m_csFactor;
	AgpdFactor			m_csFactorPercent;

	AgpdFactor			m_csSkillFactor;
	AgpdFactor			m_csSkillFactorPercent;

	INT32				m_lSkillTID;
	INT32				m_lSkillLevel;

	UINT32				m_ulSetPart;

	INT32				m_lType;
	INT32				m_lPointType;

	INT32				m_lLevelLimit;
	INT32				m_lRankLimit;
	INT32				m_lProbability;

	BOOL				m_bIsVisible;

	AgpdItemOptionSetType	m_eOptionSetType;

	CHAR				m_szDescription[AGPMITEM_OPTION_MAX_DESCRIPTION];

	AgpdItemOptionSkillType	m_eSkillType;
	stOptionSkillData	m_stSkillData;

	INT32				m_lLevelMin;
	INT32				m_lLevelMax;
	INT32				m_lLinkID;
};

//////////////////////////////////////////////////////////////////////////
//
#include <map>

typedef map<INT32, AgpdItemOptionTemplate*> AgpaItemOptionTemplate;


#endif	//__AGPDITEMOPTIONTEMPLATE_H__