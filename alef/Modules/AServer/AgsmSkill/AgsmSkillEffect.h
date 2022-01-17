#ifndef _AGSM_SKILLEFFECT
#define _AGSM_SKILLEFFECT

#include "ApDefine.h"

class ApModuleManager;
class AgpmSkill;
class AgsmSkill;
class AgpdSkill;
class ApBase;
class AgpmCharacter;
class AgsmCharacter;
class AgpmFactors;
class AgsmFactors;
class AgpmCombat;
class AgsmCombat;
class AgsmParty;
class AgpmSummons;
class AgsmSummons;
class AgpmEventNature;
class AgsmEventNature;
class AgpmSiegeWar;
class AgpmWantedCriminal;
class ApmEventManager;
class AgpmItem;
class AgpmTimer;
class AgsmTimer;
class AgpmPvP;
class AgpmEventSkillMaster;
class AgsmItem;
class AgpmAI2;

class MTRand;
class AgpdCharacter;
struct AgpdFactorAttribute;

// 2008.11.13. iluvs
// Kind of Durability Skill Define
const INT32 AGSMSKILL_EFFECT_NONCHECK_DURABILITY		= -1;
const INT32 AGSMSKILL_EFFECT_LIMIT_DURABILITY			= 1;

class AgsmSkillEffect
{
public:
	AgsmSkillEffect();
	virtual ~AgsmSkillEffect();

	ApModuleManager* m_pcsApModuleManager;
	AgpmSkill* m_pagpmSkill;
	AgsmSkill* m_pagsmSkill;
	AgpmCharacter* m_pagpmCharacter;
	AgsmCharacter* m_pagsmCharacter;
	AgpmFactors* m_pagpmFactors;
	AgsmFactors* m_pagsmFactors;
	AgpmCombat* m_pagpmCombat;
	AgsmCombat* m_pagsmCombat;
	AgpmItem* m_pagpmItem;
	AgsmParty* m_pagsmParty;
	AgpmPvP* m_pagpmPvP;
	AgpmSummons* m_pagpmSummons;
	AgsmSummons* m_pagsmSummons;
	AgpmEventNature* m_pagpmEventNature;
	AgsmEventNature* m_pagsmEventNature;
	AgpmSiegeWar* m_pagpmSiegeWar;
	AgpmWantedCriminal* m_pagpmWantedCriminal;
	ApmEventManager* m_papmEventManager;
	AgpmTimer* m_pagpmTimer;
	AgsmTimer* m_pagsmTimer;
	AgpmEventSkillMaster* m_pagpmEventSkillMaster;
	AgsmItem* m_pagsmItem;

	AgpmAI2* m_pagpmAI2;

	MTRand* m_csRandom;

	BOOL Initialize(ApModuleManager* pModuleManager);

	//////////////////////////////////////////////////////////////////////////
	//
	BOOL ProcessSkillEffect(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval = FALSE);
	BOOL ProcessSkillEffectCancel(AgpdSkill *pcsSkill, AgpdCharacter* pcsCharacter);

	//////////////////////////////////////////////////////////////////////////
	//
	BOOL ProcessSkillEffectTransformTarget(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectTransformTargetType1(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectTransformTargetType2(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectTransformTargetType3(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectTransformTargetType4(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectTransformTargetType5(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectTransformTargetType5Cancel(AgpdSkill* pcsSkill, AgpdCharacter* pcsCharacter);
	
	BOOL ProcessSkillEffectMeleeAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMagicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMagicAttackType1(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectReflectMeleeAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectReflectMagicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectReflectHeroicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectDefenseMeleeAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectDefenseMagicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);

	BOOL ProcessSkillEffectMagicResistLevelUp(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectDispelMagic(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectSpecialStatus(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectSpecialStatusDisArmament(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT32 *lParts);
	BOOL ProcessSkillEffectSpecialStatusConfusion(AgpdSkill *pcsSkill, ApBase *pcsTarget);
	BOOL ProcessSkillEffectSpecialStatusHold(AgpdSkill *pcsSkill, ApBase *pcsTarget);
	BOOL ProcessSkillEffectSpecialStatusFear(AgpdSkill *pcsSkill, ApBase *pcsTarget);
	BOOL ProcessSkillEffectSpecialStatusDisease(AgpdSkill *pcsSkill, ApBase *pcsTarget);
	BOOL ProcessSkillEffectSpecialStatusBerserk(AgpdSkill *pcsSkill, ApBase *pcsTarget);
	BOOL ProcessSkillEffectSpecialStatusShrink(AgpdSkill *pcsSkill, ApBase *pcsTarget);
	BOOL ProcessSkillEffectLifeProtection(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectDOT(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectDMGAdjust(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectSkillFactorAdjust(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);

	// detail ProcessSkillEffectSkillFactorAdjust
	BOOL ProcessSkillEffectBuffedSkillDurationTime(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);

	BOOL ProcessSkillEffectProduct(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillActionPassive(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);

	BOOL ProcessSkillEffect2UpdateCombatPoint(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */);
	BOOL ProcessSkillEffect2RegenHP(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */);
	BOOL ProcessSkillEffect2RegenMP(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */);
	BOOL ProcessSkillEffect2Convert(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */);
	BOOL ProcessSkillEffect2Charge(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */);
	BOOL ProcessSkillEffect2SkillLevelUp(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */);
	BOOL ProcessSkillEffect2ActionOnActionType3(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ActionOnActionType4(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ATField(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2Summons(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2GameBonus(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2Detect(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2Ride(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2GameEffect(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2Divide(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2Resurrection(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2Union(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ReflectMeleeAtk(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */);
	BOOL ProcessSkillEffect2Dispel(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SkillLevelUPType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SkillLevelUPType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SkillLevelUPType3(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ATFieldAttack(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ATFieldAttack2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ATFieldAura(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SummonsType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SummonsType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SummonsType3(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SummonsType4(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SummonsType5(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SummonsType6(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SummonsType7(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SummonsType8(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SummonsType9(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2SummonsType10(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2GameBonusExp(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2GameBonusMoney(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2GameBonusDropRate(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2GameBonusDropRate2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2GameBonusCharismaRate(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2DetectType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2DetectType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2GameEffectDayNight(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2GameEffectRain(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2GameEffectSnow(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2DivideAttr(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2DivideNormal(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2UnionType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2UnionType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2DispelStun(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2DispelSlow(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2DispelAllBuff(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2DispelTransparent(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2DispelSpecialStatus(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ActionOnAction(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT32 lActionOnActionType, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ActionOnActionType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ActionOnActionType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ActionOnActionType5(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ActionOnActionType6(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2MovePos(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2MovePosTarget(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2MovePosSelf(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2MovePosTargetToMe(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval);

	BOOL ProcessSkillEffectMeleeAttackType1(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMeleeAttackType2(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMeleeAttackType3(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMeleeAttackType4(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMeleeAttackType5(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMeleeAttackType6(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMeleeAttackCritical(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMeleeAttackDeath(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMagicAttackCritical(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMeleeAttackDurability(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectMeleeAttackHeroic(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);

	BOOL ProcessSkillEffectUpdateFactor(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectUpdateFactorHOT(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectUpdateFactorMagnify(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectUpdateFactorAdd(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectUpdateFactorItem(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */);
	BOOL ProcessSkillEffectUpdateFactorTime(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectUpdateFactorPermanently(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffectUpdateFactorPermanentlyRandomStatus(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	
	BOOL ProcessSkillEffectMoveTarget(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2DisturbCharacterAction(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
	BOOL ProcessSkillEffect2ToleranceDisturbCharacterAction(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval);
};
#endif //_AGSM_SKILLEFFECT