#ifndef	__AGPMCOMBAT_H__
#define	__AGPMCOMBAT_H__

#include "ApBase.h"

#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgpmSkill.h"

#include "AuRandomNumber.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmCombatD" )
#else
#pragma comment ( lib , "AgpmCombat" )
#endif
#endif
//@} Jaewon

typedef enum _eAgpdPenaltyLevelGap
{
	AGPM_COMBAT_PENALTY_LEVEL_GAP_ONE	= 3,
	AGPM_COMBAT_PENALTY_LEVEL_GAP_TWO	= 5,
	AGPM_COMBAT_PENALTY_LEVEL_GAP_THREE = 7,
	AGPM_COMBAT_PENALTY_LEVEL_GAP_FOUR	= 9,
	AGPM_COMBAT_PENALTY_LEVEL_GAP_FIVE	= 11,
	AGPM_COMBAT_PENALTY_LEVEL_GAP_SIX	= 15,
	AGPM_COMBAT_PENALTY_LEVEL_GAP_SEVEN	= 19,
	AGPM_COMBAT_PENALTY_LEVEL_GAP_EIGHT	= 20,
}eAgpdPenaltyLevelGap;

// Penalty Result For LevelGap
const INT32	AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_ONE	= 2;
const INT32	AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_TWO	= 4;
const INT32	AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_THREE	= 6;
const INT32	AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_FOUR	= 8;
const INT32	AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_FIVE	= 10;
const INT32	AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_SIX	= 12;
const INT32	AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_SEVEN	= 16;
const INT32	AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_EIGHT	= 20;


// Precheck Combat Result Bit Mask
const int AGPMCOMBAT_PROCESS_COMBAT_ARG_IGNORE_PHYSICAL_DEFENCE		= 0x00000001;
const int AGPMCOMBAT_PROCESS_COMBAT_ARG_IGNORE_ATTRIBUTE_DEFENCE	= 0x00000002;

class AgpmCombat : public ApModule
{
private:
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmItem			*m_pcsAgpmItem;
	AgpmItemConvert		*m_pcsAgpmItemConvert;
	AgpmSkill			*m_pcsAgpmSkill;

public:
	MTRand				m_csRandom;

public:
	AgpmCombat();
	~AgpmCombat();

	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	INT32				ProcessCombat(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, AgpdFactor *pcsUpdateFactor, BOOL *pabIsSuccessSecondSpirit, AgpdCharacterActionResultType *pstResult, BOOL bIsEquipWeapon, BOOL bIsAttrInvincible, INT32 *pHeroicDamage, int nCombatPreCheck = 0);
	AgpdCharacterActionResultType ProcessEvade(AgpdCharacter* pcsAttackChar, AgpdCharacter* pcsTargetChar);

	INT32				CalcPhysicalAttack(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, BOOL bIsEquipWeapon, BOOL bIsSpec = FALSE, BOOL bIsMin = FALSE, int nCombatPrecheck = 0);
	INT32				CalcBasePhysicalAttack(AgpdCharacter *pcsAttackChar, BOOL bIsMin = FALSE);
	INT32				CalcPhysicalAttackForSkill(AgpdCharacter* pcsAttackChar, AgpdCharacter* pcsTargetChar, BOOL bIsEquipWeapon, BOOL bIsSpec = FALSE);
	INT32				CalcBasePhysicalAttackForSkill(AgpdCharacter* pcsAttackChar);
	INT32				CalcFirstSpiritAttack(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, INT32 lAttributeType, BOOL bIsSpec = FALSE, BOOL bIsMin = FALSE, BOOL bForSkill = FALSE, int nCombatPreCheck = 0);
	INT32				CalcSecondSpiritAttack(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, INT32 lAttributeType, BOOL bIsSpec = FALSE, BOOL bIsMin = FALSE, BOOL bForSkill = FALSE);

	INT32				CalcHeroicAttack(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, BOOL bIsEquipWeapon, BOOL bIsSpec = FALSE, BOOL bIsMin = FALSE, int nCombatPrecheck = 0);
	INT32				CalcHeroicAttackForSkill(AgpdCharacter* pcsAttackChar, AgpdCharacter* pcsTargetChar, BOOL bIsEquipWeapon, BOOL bIsSpec = FALSE);

	INT32				CalcItemFirstSpiritAttack(AgpdItem *pcsItem, INT32 lAttributeType, BOOL bIsMin);
	INT32				CalcItemFirstSpiritDefense(AgpdItem *pcsItem, INT32 lAttributeType);

	INT32				CalcItemSecondSpiritAttack(AgpdItem *pcsItem, INT32 lAttributeType, BOOL bIsMin);

	INT32				CalcFirstSpiritDefense(AgpdCharacter *pcsTargetChar, INT32 lAttributeType);

	FLOAT				GetWeaponTypeValue(AgpdItem *pcsItem);
	FLOAT				GetCharacterTypeValue(AgpdCharacter *pcsCharacter);

	BOOL				GetArmourAttributeInfo(AgpdCharacter *pcsCharacter, INT32 *plMinRequireLevel, INT32 *plNumConvertAttr, INT32 lAttributeType);

	BOOL				IsShieldDefenseSuccess(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTargetChar);
	BOOL				IsAttackSuccess(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTargetChar);
	BOOL				IsFirstSpiritAttackSuccess(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTargetChar, INT32 lAttributeType);
	BOOL				IsSecondSpiritAttackSuccess(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, INT32 lAttributeType);

	FLOAT				GetAR(AgpdCharacter *pcsCharacter, INT32 lAttackerClass);
	FLOAT				GetBaseAR(AgpdCharacter *pcsCharacter, INT32 lAttackerClass);
	FLOAT				GetDR(AgpdCharacter *pcsCharacter, INT32 lAttackerClass);
	FLOAT				GetBaseDR(AgpdCharacter *pcsCharacter, INT32 lAttackerClass);
	FLOAT				GetPhysicalDefense(AgpdCharacter* pcsCharacter);
	FLOAT				GetPhysicalResistance(AgpdCharacter* pcsCharacter);
	INT32				GetPhysicalBlockRate(AgpdCharacter* pcsCharacter);
	INT32				GetSkillBlockRate(AgpdCharacter* pcsCharacter);

	FLOAT				GetHeroicDefense(AgpdCharacter* pcsCharacter);
	FLOAT				GetHeroicDamageMax(AgpdCharacter* pcsCharacter);
	FLOAT				GetHeroicDamageMin(AgpdCharacter* pcsCharacter);
	FLOAT				GetHeroicMeleeResistance(AgpdCharacter* pcsCharacter);
	FLOAT				GetHeroicRangedResistance(AgpdCharacter* pcsCharacter);
	FLOAT				GetHeroicMagicResistance(AgpdCharacter* pcsCharacter);
	FLOAT				GetHeroicResistanceByHeroicClass(AgpdCharacter* pcsCharacter);
	FLOAT				GetHeroicResistanceByAttackerHeroicClass(AgpdCharacter* pcsAttacker, AgpdCharacter* pcsCharacter);
	
	INT32				GetAdditionalHitRate(AgpdCharacter* pcsCharacter);
	INT32				GetAdditionalEvadeRate(AgpdCharacter* pcsCharacter);
	INT32				GetAdditionalDodgeRate(AgpdCharacter* pcsCharacter);

	INT16				CalcPenaltyExp(AgpdFactor *pcsFactor, PVOID *pvPacket);
	INT32				CalcDefPenaltyLevelGap(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTargetChar);
	INT32				CalcAtkPenaltyLevelGap(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTargetChar);
};

#endif	//__AGPMCOMBAT_H__