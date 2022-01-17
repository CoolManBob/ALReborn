#include "AgsmSkillEffect.h"
#include "AgsmSkill.h"
#include "AuMath.h"
#include "AgsdDatabase.h"
#include "AgpmItem.h"
#include "AgpmAI2.h"

typedef std::multimap<INT32, INT32>						MapMob;
typedef std::multimap<INT32, INT32>::iterator			MapIterMob;
typedef std::multimap<INT32, INT32>::reverse_iterator	MapRIterMob;

extern INT32	g_alFactorTable[AGPMSKILL_CONST_PERCENT_END + 1][3];

AgsmSkillEffect::AgsmSkillEffect()
	: m_pcsApModuleManager(NULL)
{
	m_pagpmSkill = NULL;
	m_pagsmSkill = NULL;
	m_pagpmCharacter = NULL;
	m_pagsmCharacter = NULL;
	m_pagpmFactors = NULL;
	m_pagpmCombat = NULL;
	m_pagsmParty = NULL;
	m_pagpmSummons = NULL;
	m_pagsmSummons = NULL;
	m_pagpmEventNature = NULL;
	m_pagsmEventNature = NULL;
	m_pagsmFactors = NULL;
	m_pagpmSiegeWar = NULL;
	m_pagpmWantedCriminal = NULL;
	m_pagsmCombat = NULL;
	m_pagpmItem = NULL;
	m_pagpmTimer = NULL;
	m_pagsmTimer = NULL;
	m_papmEventManager = NULL;
	m_pagpmPvP = NULL;
	m_pagpmEventSkillMaster = NULL;
	m_pagsmItem	= NULL;
	m_pagpmAI2 = NULL;

	m_csRandom = NULL;
}

AgsmSkillEffect::~AgsmSkillEffect()
{
	if(m_csRandom)
		delete m_csRandom;
}

BOOL AgsmSkillEffect::Initialize(ApModuleManager* pModuleManager)
{
	m_pcsApModuleManager = pModuleManager;
	if(!m_pcsApModuleManager)
		return FALSE;

	m_pagpmSkill = (AgpmSkill*)m_pcsApModuleManager->GetModule("AgpmSkill");
	m_pagsmSkill = (AgsmSkill*)m_pcsApModuleManager->GetModule("AgsmSkill");
	m_pagpmCharacter = (AgpmCharacter*)m_pcsApModuleManager->GetModule("AgpmCharacter");
	m_pagsmCharacter = (AgsmCharacter*)m_pcsApModuleManager->GetModule("AgsmCharacter");
	m_pagpmFactors = (AgpmFactors*)m_pcsApModuleManager->GetModule("AgpmFactors");
	m_pagsmFactors = (AgsmFactors*)m_pcsApModuleManager->GetModule("AgsmFactors");
	m_pagpmCombat = (AgpmCombat*)m_pcsApModuleManager->GetModule("AgpmCombat");
	m_pagsmCombat = (AgsmCombat*)m_pcsApModuleManager->GetModule("AgsmCombat");
	m_pagpmItem = (AgpmItem*)m_pcsApModuleManager->GetModule("AgpmItem");
	m_pagsmParty = (AgsmParty*)m_pcsApModuleManager->GetModule("AgsmParty");
	m_pagpmSummons = (AgpmSummons*)m_pcsApModuleManager->GetModule("AgpmSummons");
	m_pagsmSummons = (AgsmSummons*)m_pcsApModuleManager->GetModule("AgsmSummons");
	m_pagpmEventNature = (AgpmEventNature*)m_pcsApModuleManager->GetModule("AgpmEventNature");
	m_pagsmEventNature = (AgsmEventNature*)m_pcsApModuleManager->GetModule("AgsmEventNature");
	m_pagpmSiegeWar = (AgpmSiegeWar*)m_pcsApModuleManager->GetModule("AgpmSiegeWar");
	m_pagpmWantedCriminal = (AgpmWantedCriminal*)m_pcsApModuleManager->GetModule("AgpmWantedCriminal");
	m_pagpmTimer = (AgpmTimer*)m_pcsApModuleManager->GetModule("AgpmTimer");
	m_pagsmTimer = (AgsmTimer*)m_pcsApModuleManager->GetModule("AgsmTimer");
	m_papmEventManager = (ApmEventManager*)m_pcsApModuleManager->GetModule("ApmEventManager");
	m_pagpmPvP = (AgpmPvP*)m_pcsApModuleManager->GetModule("AgpmPvP");
	m_pagpmEventSkillMaster = (AgpmEventSkillMaster*)m_pcsApModuleManager->GetModule("AgpmEventSkillMaster");
	m_pagsmItem = (AgsmItem*)m_pcsApModuleManager->GetModule("AgsmItem");
	m_pagpmAI2 = (AgpmAI2*)m_pcsApModuleManager->GetModule("AgpmAI2");

	m_csRandom = new MTRand;

	return TRUE;
}

// 3. process skill effect functions
///////////////////////////////////////////////////////////////////////////////
//		ProcessSkillEffect
//	Functions
//		- pcsSkill 의 skill effect type에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	: 처리할 skill type index
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffect(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	PROFILE("AgsmSkill::ProcessSkillEffect");

	if (!pcsSkill || !pcsSkill->m_pcsTemplate || !pcsTarget)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32	nRepeatCount = 1;

	BOOL	bRetval	= FALSE;

	for (int i = 0; i < nRepeatCount; ++i)
	{
		if (bProcessInterval)
		{
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET)
				bRetval |= ProcessSkillEffectTransformTarget(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG)
				bRetval |= ProcessSkillEffectMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG)
				bRetval |= ProcessSkillEffectMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			//if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MOVE_TARGET)
			//	bRetval |= ProcessSkillEffectMoveTarget(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR)
				bRetval |= ProcessSkillEffectUpdateFactor(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_MELEE_ATTACK)
				bRetval |= ProcessSkillEffectReflectMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_MAGIC_ATTACK)
				bRetval |= ProcessSkillEffectReflectMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_HEROIC_ATTACK)
				bRetval |= ProcessSkillEffectReflectHeroicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);
			
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK)
				bRetval |= ProcessSkillEffectDefenseMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DEFENSE_MAGIC_ATTACK)
				bRetval |= ProcessSkillEffectDefenseMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_RESIST_LEVEL_UP)
				bRetval |= ProcessSkillEffectMagicResistLevelUp(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DISPEL_MAGIC)
				bRetval |= ProcessSkillEffectDispelMagic(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_SPECIAL_STATUS)
				bRetval |= ProcessSkillEffectSpecialStatus(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_LIFE_PROTECTION)
				bRetval |= ProcessSkillEffectLifeProtection(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE)
				bRetval |= ProcessSkillEffectDOT(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DMG_ADJUST)
				bRetval |= ProcessSkillEffectDMGAdjust(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST)
				bRetval |= ProcessSkillEffectSkillFactorAdjust(pcsSkill, pcsTarget, nIndex, bProcessInterval);	
		
			// Skill Effect 2
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT)
				bRetval |= ProcessSkillEffect2UpdateCombatPoint(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_REGEN_HP)
				bRetval |= ProcessSkillEffect2RegenHP(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_REGEN_MP)
				bRetval |= ProcessSkillEffect2RegenMP(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_CONVERT)
				bRetval |= ProcessSkillEffect2Convert(pcsSkill, pcsTarget, nIndex, bProcessInterval);
			
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_CHARGE)
				bRetval |= ProcessSkillEffect2Charge(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_SKILL_LEVELUP)
				bRetval |= ProcessSkillEffect2SkillLevelUp(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_PRODUCT)
				bRetval |= ProcessSkillEffectProduct(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION)
			{
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1)
					bRetval = TRUE;	// 1타입이라면 걍 TRUE 준다.
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3)
					bRetval |= ProcessSkillEffect2ActionOnActionType3(pcsSkill, pcsTarget, bProcessInterval);
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4)
					bRetval |= ProcessSkillEffect2ActionOnActionType4(pcsSkill, pcsTarget, bProcessInterval);
			}

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_AT_FIELD)
				bRetval |= ProcessSkillEffect2ATField(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_SUMMONS)
				bRetval |= ProcessSkillEffect2Summons(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_ACTION_PASSIVE)
				bRetval |= ProcessSkillActionPassive(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_GAME_BONUS)
				bRetval |= ProcessSkillEffect2GameBonus(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_DETECT)
				bRetval |= ProcessSkillEffect2Detect(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_RIDE)
				bRetval |= ProcessSkillEffect2Ride(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_GAME_EFFECT)
				bRetval |= ProcessSkillEffect2GameEffect(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_DIVIDE)
				bRetval |= ProcessSkillEffect2Divide(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_RESURRECTION)
				bRetval |= ProcessSkillEffect2Resurrection(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_SKILL_UNION)
				bRetval |= ProcessSkillEffect2Union(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_MOVE_POS)
				bRetval |= ProcessSkillEffect2MovePos(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_DISTURB_CHARACTER_ACTION)
				bRetval |= ProcessSkillEffect2DisturbCharacterAction(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_TOLERANCE_DISTURB_CHARACTER_ACTION)
				bRetval |= ProcessSkillEffect2ToleranceDisturbCharacterAction(pcsSkill, pcsTarget, nIndex, bProcessInterval);
		}
		else
		{
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET)
				bRetval |= ProcessSkillEffectTransformTarget(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG)
				bRetval |= ProcessSkillEffectMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG)
				bRetval |= ProcessSkillEffectMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			//if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MOVE_TARGET)
			//	bRetval |= ProcessSkillEffectMoveTarget(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR)
				bRetval |= ProcessSkillEffectUpdateFactor(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_MELEE_ATTACK)
				bRetval |= ProcessSkillEffectReflectMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_MAGIC_ATTACK)
				bRetval |= ProcessSkillEffectReflectMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_HEROIC_ATTACK)
				bRetval |= ProcessSkillEffectReflectHeroicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK)
				bRetval |= ProcessSkillEffectDefenseMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DEFENSE_MAGIC_ATTACK)
				bRetval |= ProcessSkillEffectDefenseMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_RESIST_LEVEL_UP)
				bRetval |= ProcessSkillEffectMagicResistLevelUp(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DISPEL_MAGIC)
				bRetval |= ProcessSkillEffectDispelMagic(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_SPECIAL_STATUS)
				bRetval |= ProcessSkillEffectSpecialStatus(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_LIFE_PROTECTION)
				bRetval |= ProcessSkillEffectLifeProtection(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE)
				bRetval |= ProcessSkillEffectDOT(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DMG_ADJUST)
				bRetval |= ProcessSkillEffectDMGAdjust(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST)
				bRetval |= ProcessSkillEffectSkillFactorAdjust(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			// Skill Effect 2
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT)
				bRetval |= ProcessSkillEffect2UpdateCombatPoint(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_REGEN_HP)
				bRetval |= ProcessSkillEffect2RegenHP(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_REGEN_MP)
				bRetval |= ProcessSkillEffect2RegenMP(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_CONVERT)
				bRetval |= ProcessSkillEffect2Convert(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_CHARGE)
				bRetval |= ProcessSkillEffect2Charge(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_SKILL_LEVELUP)
				bRetval |= ProcessSkillEffect2SkillLevelUp(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_PRODUCT)
				bRetval |= ProcessSkillEffectProduct(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION)
			{
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1)
					bRetval = TRUE;	// 1타입이라면 걍 TRUE 준다.
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3)
					bRetval |= ProcessSkillEffect2ActionOnActionType3(pcsSkill, pcsTarget, bProcessInterval);
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4)
					bRetval |= ProcessSkillEffect2ActionOnActionType4(pcsSkill, pcsTarget, bProcessInterval);
			}

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_AT_FIELD)
				bRetval |= ProcessSkillEffect2ATField(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_SUMMONS)
				bRetval |= ProcessSkillEffect2Summons(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_ACTION_PASSIVE)
				bRetval |= ProcessSkillActionPassive(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_GAME_BONUS)
				bRetval |= ProcessSkillEffect2GameBonus(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_DETECT)
				bRetval |= ProcessSkillEffect2Detect(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_RIDE)
				bRetval |= ProcessSkillEffect2Ride(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_GAME_EFFECT)
				bRetval |= ProcessSkillEffect2GameEffect(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_DIVIDE)
				bRetval |= ProcessSkillEffect2Divide(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_RESURRECTION)
				bRetval |= ProcessSkillEffect2Resurrection(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_SKILL_UNION)
				bRetval |= ProcessSkillEffect2Union(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_MOVE_POS)
				bRetval |= ProcessSkillEffect2MovePos(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_DISTURB_CHARACTER_ACTION)
				bRetval |= ProcessSkillEffect2DisturbCharacterAction(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_TOLERANCE_DISTURB_CHARACTER_ACTION)
				bRetval |= ProcessSkillEffect2ToleranceDisturbCharacterAction(pcsSkill, pcsTarget, nIndex, bProcessInterval);
		}
	}

	if(!bRetval)
	{
		if(pcsAgsdSkill->m_eMissedReason == AGSDSKILL_MISSED_REASON_NORMAL)
			m_pagsmSkill->SendMissCastSkill(pcsSkill, pcsTarget);
	}

	return bRetval;
}

BOOL AgsmSkillEffect::ProcessSkillEffectCancel(AgpdSkill *pcsSkill, AgpdCharacter* pcsCharacter)
{
	if(!pcsSkill || !pcsCharacter)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	for (INT32 i = 0; i < AGPMSKILL_MAX_SKILL_CONDITION; ++i)
	{
		if (pcsSkillTemplate->m_lEffectType[i] & AGPMSKILL_EFFECT_TRANSFORM_TARGET)
		{
			if (pcsSkillTemplate->m_lEffectType[i] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE5)
			{
				bResult = ProcessSkillEffectTransformTargetType5Cancel(pcsSkill, pcsCharacter);
				//if(pcsSkill->m_pSaveSkillData.eStep != SAVESKILL_NONE)
				{
					pcsSkill->m_pSaveSkillData.m_pSaveData.SkillTID = pcsSkillTemplate->m_lID;
					m_pagsmSkill->ProcessSkillSave(pcsSkill, pcsCharacter, AGSMDATABASE_OPERATION_DELETE);
				}
			}
		}
	}

	return bResult;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTarget(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	PROFILE("AgsmSkill::ProcessSkillEffectTransformTarget");

	if (!pcsSkill || !pcsTarget)
		return FALSE;

	BOOL	bRetval = TRUE;

	if (bProcessInterval)
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE1)
			bRetval &= ProcessSkillEffectTransformTargetType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE2)
			bRetval &= ProcessSkillEffectTransformTargetType2(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE3)
			bRetval &= ProcessSkillEffectTransformTargetType3(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE4)
			bRetval &= ProcessSkillEffectTransformTargetType4(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE5)
		{
			bRetval &= ProcessSkillEffectTransformTargetType5(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			// 진화 스킬은 저장한다. - arycoat 2008.10
			if(pcsSkill->m_pSaveSkillData.eStep == SAVESKILL_NONE)
			{
				pcsSkill->m_pSaveSkillData.m_pSaveData.SkillTID = pcsSkill->m_pcsTemplate->m_lID;
				m_pagsmSkill->ProcessSkillSave(pcsSkill, (AgpdCharacter*)pcsTarget, AGSMDATABASE_OPERATION_INSERT);
			}
		}
	}
	else
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE1)
			bRetval &= ProcessSkillEffectTransformTargetType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE2)
			bRetval &= ProcessSkillEffectTransformTargetType2(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE3)
			bRetval &= ProcessSkillEffectTransformTargetType3(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE4)
			bRetval &= ProcessSkillEffectTransformTargetType4(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE5)
		{
			bRetval &= ProcessSkillEffectTransformTargetType5(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			// 진화 스킬은 저장한다. - arycoat 2008.10
			if(pcsSkill->m_pSaveSkillData.eStep == SAVESKILL_NONE)
			{
				pcsSkill->m_pSaveSkillData.m_pSaveData.SkillTID = pcsSkill->m_pcsTemplate->m_lID;
				m_pagsmSkill->ProcessSkillSave(pcsSkill, (AgpdCharacter*)pcsTarget, AGSMDATABASE_OPERATION_INSERT);
			}
		}
	}

	return bRetval;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType1(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
		INT32	lSkillLevel	= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
		if(!lSkillLevel)
			return FALSE;

		AgpdSkillTemplate		*pcsSkillTemplate		= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;

		AgpdCharacterTemplate	*pcsMonsterTemplate		= m_pagpmCharacter->GetCharacterTemplate((INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CHANGE_MONSTER][lSkillLevel]);
		if (!pcsMonsterTemplate)
			return FALSE;

		INT32					lRace					= m_pagpmFactors->GetRace(&((AgpdCharacter *) pcsTarget)->m_csFactor);
		INT32					lGender					= m_pagpmFactors->GetGender(&((AgpdCharacter *) pcsTarget)->m_csFactor);

		INT32					lIndex					= 0;
		AgpdCharacterTemplate	*pcsCharacterTemplate	= NULL;

		pcsCharacterTemplate	= m_pagpmCharacter->GetTemplateSequence(&lIndex);
		while (pcsCharacterTemplate)
		{
			INT32	lTemplateRace	= m_pagpmFactors->GetRace(&pcsCharacterTemplate->m_csFactor);
			//INT32	lTemplateGender	= m_pagpmFactors->GetGender(&pcsCharacterTemplate->m_csFactor);
			INT32	lTemplateClass	= m_pagpmFactors->GetClass(&pcsCharacterTemplate->m_csFactor);

			if (lTemplateRace == lRace &&
				//lTemplateGender == lGender &&
				lTemplateClass == AUCHARCLASS_TYPE_KNIGHT &&
				m_pagpmFactors->IsUseFactor(&pcsCharacterTemplate->m_csLevelFactor[(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_WARRIOR_LEVEL][lSkillLevel]]))
				break;

			pcsCharacterTemplate	= m_pagpmCharacter->GetTemplateSequence(&lIndex);
		}

		if (!pcsCharacterTemplate)
			return FALSE;

		return m_pagpmCharacter->TransformCharacter((AgpdCharacter *) pcsTarget,
												AGPMCHAR_TRANSFORM_TYPE_APPEAR_STATUS_ALL,
												pcsMonsterTemplate,
												&pcsCharacterTemplate->m_csLevelFactor[(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_WARRIOR_LEVEL][lSkillLevel]],
												TRUE);
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType2(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType3(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
	}

	return TRUE;
}

// 2005.09.01. steeple
// Type1 과 비슷한데, Race, Gender 는 무시하고, Type1 에서 TargetFactor 얻는 부분을 빼고 단순화 시킴.
// 원래는 빈 코드였음.
BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType4(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// 몬스터인데 바꿀 수 없는 놈이라면 걍 return FALSE. 이 건 원래 Cast 시점에서 Fail 떨어져야 한다.
	if(m_pagpmCharacter->IsMonster((AgpdCharacter*)pcsTarget) &&
		((AgpdCharacter*)pcsTarget)->m_pcsCharacterTemplate->m_eTamableType != AGPDCHAR_TAMABLE_TYPE_BY_FORMULA)
		return FALSE;

	// 2005.10.24. steeple
	// 공식 비교 들어간다.
	INT32 lCharacterLevel = m_pagpmCharacter->GetLevel((AgpdCharacter*)pcsSkill->m_pcsBase);
	INT32 lTargetLevel = m_pagpmCharacter->GetLevel((AgpdCharacter*)pcsTarget);

	// 이건 모야!!!
	if(lCharacterLevel + lTargetLevel == 0)
		return FALSE;

	INT32 lRandom = m_csRandom->randInt(100);
	if(lRandom > (INT32)(((FLOAT)lCharacterLevel / (FLOAT)(lCharacterLevel + lTargetLevel)) * 100.0f))
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;
	
	INT32 lCreatureTID = m_pagpmSkill->GetRandomCreatureTID(pcsSkillTemplate, lSkillLevel);

	AgpdCharacterTemplate* pcsTransformTemplate = m_pagpmCharacter->GetCharacterTemplate(lCreatureTID);
	if(!pcsTransformTemplate)
		return FALSE;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
	if(ulDuration == 0)
		return FALSE;

	if(!m_pagpmCharacter->TransformCharacter((AgpdCharacter*)pcsTarget,
											AGPMCHAR_TRANSFORM_TYPE_APPEAR_ONLY,
											pcsTransformTemplate,
											&pcsTransformTemplate->m_csFactor,
											TRUE))
		return FALSE;

	// 누구한테 걸린건지 세팅한다. 2005.10.24. steeple
	((AgpdCharacter*)pcsTarget)->m_lTransformedByCID = pcsSkill->m_pcsBase->m_lID;

	// 풀리는 시간 세팅해준다.
	m_pagsmCharacter->SetTransformTimeout((AgpdCharacter*)pcsTarget, ulDuration);

	// 일단 정지 시킨다. 2005.10.03. steeple
	m_pagpmCharacter->StopCharacter((AgpdCharacter*)pcsTarget, NULL);

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType5(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pcsTarget;

	if(!m_pagsmSkill->CheckEnableEvolution(pcsCharacter, pcsSkill))
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lCreatureTID = m_pagpmSkill->GetRandomCreatureTID(pcsSkillTemplate, lSkillLevel);

	AgpdCharacterTemplate* pcsTransformTemplate = m_pagpmCharacter->GetCharacterTemplate(lCreatureTID);
	if(!pcsTransformTemplate)
		return FALSE;

	m_pagpmCharacter->EvolutionCharacter(pcsCharacter, pcsTransformTemplate);

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType5Cancel(AgpdSkill* pcsSkill, AgpdCharacter* pcsCharacter)
{
	if(!pcsSkill || !pcsCharacter)
		return FALSE;

	INT32 lPrevTID = m_pagpmEventSkillMaster->m_vcSkillMasteryEvolution.GetBeforeEvolutionTID(pcsCharacter->m_lTID1);
	if(!lPrevTID)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdCharacterTemplate* pcsTransformTemplate = m_pagpmCharacter->GetCharacterTemplate(lPrevTID);
	if(!pcsTransformTemplate)
		return FALSE;

	m_pagpmCharacter->RestoreEvolutionCharacter(pcsCharacter, pcsTransformTemplate);

	return TRUE;
}

//		ProcessSkillEffectMeleeAttack
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	PROFILE("AgsmSkill::ProcessSkillEffectMeleeAttack");

	if (!pcsSkill || !pcsTarget)
		return FALSE;

	BOOL	bRetval = TRUE;

	if (bProcessInterval)
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1)
			bRetval &= ProcessSkillEffectMeleeAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE2)
			bRetval &= ProcessSkillEffectMeleeAttackType2(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE3)
			bRetval &= ProcessSkillEffectMeleeAttackType3(pcsSkill, pcsTarget, nIndex, bProcessInterval);

//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE4)
//			bRetval &= ProcessSkillEffectMeleeAttackType4(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE5)
//			bRetval &= ProcessSkillEffectMeleeAttackType5(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL)
			bRetval &= ProcessSkillEffectMeleeAttackCritical(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DEATH)
			bRetval &= ProcessSkillEffectMeleeAttackDeath(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE6)
			bRetval &= ProcessSkillEffectMeleeAttackType6(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DURABILITY)
			// 여기에 Durability 함수 추가
			bRetval &= ProcessSkillEffectMeleeAttackDurability(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		// 여기는 Heroic Melee Attack 관련 스킬 처리
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_HEROIC)
			bRetval &= ProcessSkillEffectMeleeAttackHeroic(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}
	else
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1)
			bRetval &= ProcessSkillEffectMeleeAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE2)
			bRetval &= ProcessSkillEffectMeleeAttackType2(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE3)
			bRetval &= ProcessSkillEffectMeleeAttackType3(pcsSkill, pcsTarget, nIndex, bProcessInterval);

//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE4)
//			bRetval &= ProcessSkillEffectMeleeAttackType4(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE5)
//			bRetval &= ProcessSkillEffectMeleeAttackType5(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL)
			bRetval &= ProcessSkillEffectMeleeAttackCritical(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DEATH)
			bRetval &= ProcessSkillEffectMeleeAttackDeath(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE6)
			bRetval &= ProcessSkillEffectMeleeAttackType6(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DURABILITY)
			// 여기에 Durability 함수 추가
			bRetval &= ProcessSkillEffectMeleeAttackDurability(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		// 여기는 Heroic Melee Attack 관련 스킬 처리
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_HEROIC)
			bRetval &= ProcessSkillEffectMeleeAttackHeroic(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	return bRetval;
}

//		ProcessSkillEffectMagicAttack
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_MAGIC_ATTACK_DMG) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMagicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	PROFILE("AgsmSkill::ProcessSkillEffectMagicAttack");

	if (!pcsSkill || !pcsTarget)
		return FALSE;

	BOOL	bRetval	= TRUE;

	if (bProcessInterval)
	{
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_MAGIC)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_FIRE)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_EARTH)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_AIR)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_WATER)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL)
			bRetval &= ProcessSkillEffectMagicAttackCritical(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}
	else
	{
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_MAGIC)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_FIRE)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_EARTH)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_AIR)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_WATER)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL)
			bRetval &= ProcessSkillEffectMagicAttackCritical(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	return bRetval;
}

//		ProcessSkillEffectUpdateFactor
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_UPDATE_FACTOR) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactor(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	PROFILE("AgsmSkill::ProcessSkillEffectUpdateFactor");

	if (!pcsSkill || !pcsTarget)
		return FALSE;

	BOOL	bRetval = TRUE;

	// 2006.01.18. steeple
	// Attack Damage 만 세팅되어 있는 스킬이라면, Factor 올리는 작업을 하지 않는다.
	if(m_pagsmSkill->IsSetAttackDamageOnly(pcsSkill))
		return TRUE;

	if (bProcessInterval)
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_HOT)
			bRetval &= ProcessSkillEffectUpdateFactorHOT(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_MAGNIFY)
			bRetval &= ProcessSkillEffectUpdateFactorMagnify(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_FOR_A_WHILE)
			bRetval &= ProcessSkillEffectUpdateFactorAdd(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_ITEM)
			bRetval &= ProcessSkillEffectUpdateFactorItem(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_TIME)
			bRetval &= ProcessSkillEffectUpdateFactorTime(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}
	else
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_HOT)
			bRetval &= ProcessSkillEffectUpdateFactorHOT(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_MAGNIFY)
			bRetval &= ProcessSkillEffectUpdateFactorMagnify(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_FOR_A_WHILE)
			bRetval &= ProcessSkillEffectUpdateFactorAdd(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_ITEM)
			bRetval &= ProcessSkillEffectUpdateFactorItem(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_TIME)
			bRetval &= ProcessSkillEffectUpdateFactorTime(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	return bRetval;
}

//		ProcessSkillEffectReflectMeleeAttack
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_REFLECT_MELEE_ATTACK) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectReflectMeleeAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget) return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	//////////////////////////////////////////////////////////////////////////
	// 2004.09.21. steeple 변경
	
	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	UINT64 ullEffectType = 0;
	if(bProcessInterval)
		ullEffectType = pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex];
	else
		ullEffectType = pcsSkillTemplate->m_lEffectType[nIndex];

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	// AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE 는 Damage 를 반격하고,
	if(ullEffectType & AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE)
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectProbability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectAmount = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectProbability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectAmount += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];
		}

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamageReflectProbability += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectProbability;
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamageReflectAmount += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectAmount;
	}

	// AGPMSKILL_EFFECT_REFLECT_DAMAGE_SHIELD 는 Damage 를 무시한다.
	if(ullEffectType & AGPMSKILL_EFFECT_REFLECT_DAMAGE_SHIELD)
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageIgnoreProbability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		if(bCheckConnection)
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageIgnoreProbability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamageIgnoreProbability += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageIgnoreProbability;
	}

	// 
	// Effect_Detail_Reflect_Melee_Attack을 처리해준다. 2008.06.11. iluvs
	if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK])
	{
		ProcessSkillEffect2ReflectMeleeAtk(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	return TRUE;
}

//		ProcessSkillEffectReflectMagicAttack
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_REFLECT_MAGIC_ATTACK) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectReflectMagicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget) return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	pcsAttachData->m_nDefenseTypeEffect |= AGPMSKILL_EFFECT_REFLECT_MAGIC_ATTACK;

	if (bProcessInterval)
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE)
			pcsAttachData->m_nDefenseTypeEffect |= AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE;
		else
		{
			pcsAgsdSkill->m_nMagicReflectPoint 
				= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];
			
			if(bCheckConnection)
			{
				pcsAgsdSkill->m_nMagicReflectPoint 
					+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];
			}

			pcsAttachData->m_nMagicReflectPoint += pcsAgsdSkill->m_nMagicReflectPoint;
		}
	}
	else
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE)
			pcsAttachData->m_nDefenseTypeEffect |= AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE;
		else
		{
			pcsAgsdSkill->m_nMagicReflectPoint 
				= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];

			if(bCheckConnection)
			{
				pcsAgsdSkill->m_nMagicReflectPoint 
					+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];
			}

			pcsAttachData->m_nMagicReflectPoint += pcsAgsdSkill->m_nMagicReflectPoint;
		}
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectReflectHeroicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget || NULL == pcsSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	// AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE 는 Damage 를 반격하고,
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicProbability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicAmount = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_HEROIC_REFLECT_MAX][lSkillLevel];

	if(bCheckConnection)
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicProbability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicAmount += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_HEROIC_REFLECT_MAX][lSkillLevel];
	}

	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicProbability += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectProbability;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicAmount += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectAmount;

	return TRUE;
}

//		ProcessSkillEffectDefenseMeleeAttack
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectDefenseMeleeAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget) return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	UINT64				ullEffectType	= 0;

	if (bProcessInterval)
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex];
	else
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex];

	pcsAttachData->m_ullBuffedSkillCombatEffect				|= AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// 물리 공격을 100% 막고 Melee Damage만큼 데미지를 준다.
	if (ullEffectType & AGPMSKILL_EFFECT_DEFENSE_COUNTER_ATTACK)
	{
		pcsAttachData->m_ullBuffedSkillCombatEffect				|= AGPMSKILL_EFFECT_DEFENSE_COUNTER_ATTACK;
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lCounterAttackProbability	+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lCounterAttackProbability	= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
	}

	// 2005.03.27. steeple
	// 상대방의 공격을 회피한다. arg1 이 0이면 EVADE. 1이면 DODGE 임.
	//
	// 상대방의 모든 물리/장거리 공격을 arg1의 확률로 회피하여 무효화한다.
	// arg1 이 0 이면 Const 에서 꺼내온다. - 2004.09.15. steeple
	else if (ullEffectType & AGPMSKILL_EFFECT_DEFENSE_EVADE)
	{
		// 2005.12.22. steeple
		// Evade 체크는 바뀌었다.
		//
		//pcsAttachData->m_ullBuffedSkillCombatEffect						|= AGPMSKILL_EFFECT_DEFENSE_EVADE;

		//switch(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1)
		//{
		//	case 0:		// Evade 만
		//		pcsAttachData->m_stBuffedSkillCombatEffectArg.lEvadeProbability	+= ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_EVADE_RATE][lSkillLevel];
		//		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lEvadeProbability = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_EVADE_RATE][lSkillLevel];
		//		break;

		//	case 1:		// Dodge 만
		//		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDodgeProbability	+= ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DODGE_RATE][lSkillLevel];
		//		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDodgeProbability = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DODGE_RATE][lSkillLevel];
		//		break;

		//	case 2:		// 둘다
		//		pcsAttachData->m_stBuffedSkillCombatEffectArg.lEvadeProbability	+= ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_EVADE_RATE][lSkillLevel];
		//		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lEvadeProbability = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_EVADE_RATE][lSkillLevel];
		//		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDodgeProbability	+= ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DODGE_RATE][lSkillLevel];
		//		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDodgeProbability = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DODGE_RATE][lSkillLevel];
		//		break;
		//}
	}

	//defense_max 만큼 physical 데미지를 막는다.
	else if (ullEffectType & AGPMSKILL_EFFECT_CRITICAL_DEFENCE_PERCENT) 
	{
		pcsAttachData->m_ullBuffedSkillCombatEffect	|= AGPMSKILL_EFFECT_CRITICAL_DEFENCE_PERCENT;
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDefenceCriticalAttack	+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CRITICAL_DEFENCE_PERCENT][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDefenceCriticalAttack	= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CRITICAL_DEFENCE_PERCENT][lSkillLevel]; 
	}
	else 
	{
		pcsAttachData->m_nDefenseTypeEffect |= AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK;

		pcsAgsdSkill->m_nMeleeDefensePoint 
			= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DEFENSE_MAX][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAgsdSkill->m_nMeleeDefensePoint 
				+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DEFENSE_MAX][lSkillLevel];
		}

		pcsAttachData->m_nMeleeDefensePoint += pcsAgsdSkill->m_nMeleeDefensePoint;
	}

	return TRUE;
}

//		ProcessSkillEffectDefenseMagicAttack
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_DEFENSE_MAGIC_ATTACK) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectDefenseMagicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget) return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	UINT64				ullEffectType	= 0;

	if (bProcessInterval)
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex];
	else
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex];

	pcsAttachData->m_ullBuffedSkillCombatEffect				|= AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK;

	// 상대방의 모든 매직공격을 arg1의 확률로 회피하여 무효화한다.
	if (ullEffectType & AGPMSKILL_EFFECT_DEFENSE_EVADE)
	{
		// 2005.12.22. steeple
		// Evade 체크는 바뀌었다.
		//
		//pcsAttachData->m_ullBuffedSkillCombatEffect						|= AGPMSKILL_EFFECT_DEFENSE_EVADE;
		//pcsAttachData->m_stBuffedSkillCombatEffectArg.lEvadeProbability	+= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
		//pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lEvadeProbability	= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
	}

	// defense_max 만큼 magic 데미지를 막는다.
	else
	{
		pcsAttachData->m_nDefenseTypeEffect |= AGPMSKILL_EFFECT_DEFENSE_MAGIC_ATTACK;

		pcsAgsdSkill->m_nMagicDefensePoint 
			= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DEFENSE_MAX][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAgsdSkill->m_nMagicDefensePoint 
				+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DEFENSE_MAX][lSkillLevel];
		}

		pcsAttachData->m_nMagicDefensePoint += pcsAgsdSkill->m_nMagicDefensePoint;
	}

	return TRUE;
}

//		ProcessSkillEffectMagicResistLevelUp
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_MAGIC_RESIST_LEVEL_UP) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMagicResistLevelUp(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	return m_pagsmSkill->SetModifiedCharLevel(pcsTarget, pcsSkill);
}

//		ProcessSkillEffectDispelMagic
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_DISPEL_MAGIC) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectDispelMagic(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget || !pcsSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkillADBase* pcsADBase = m_pagsmSkill->GetADBase((ApBase*)pcsTarget);
	if(!pcsADBase)
		return TRUE;

	AgpdSkillTemplate* pcsTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	UINT64 ullEffectType = 0;
	if(bProcessInterval)
		ullEffectType = pcsTemplate->m_lProcessIntervalEffectType[nIndex];
	else
		ullEffectType = pcsTemplate->m_lEffectType[nIndex];

	if (ullEffectType & AGPMSKILL_EFFECT_ADD_SPELLCOUNT_MASTERY_POINT)
	{
		INT32	lMasteryPoint	= lSkillLevel;
		if (lMasteryPoint)
		{
			INT32	lDispelSkillIndex	= (-1);
			m_pagsmSkill->ReduceSpellCount(pcsTarget, lMasteryPoint, &lDispelSkillIndex);

			if (lDispelSkillIndex >= 0)
			{
				// spell count가 0이 되었다. 스킬을 없애야 한다.
				m_pagsmSkill->FreeSkillEffect(pcsADBase->m_csBuffedSkillProcessInfo[lDispelSkillIndex].m_pcsTempSkill, pcsTarget, lDispelSkillIndex);
			}
		}
	}

	// 2004. 09. 21. steeple
	if(ullEffectType & AGPMSKILL_EFFECT_DISPEL_MAGIC_CURE)
	{
		m_pagsmSkill->EndAllDebuffSkill((ApBase*)pcsTarget, TRUE, ( INT32 ) pcsTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel]);
	}

	// Effect Detail 처리해준다. 2007.07.03. steeple
	if(pcsTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL])
	{
		ProcessSkillEffect2Dispel(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	return TRUE;
}

//		ProcessSkillEffectSpecialStatus
//	Functions
//		- pcsSkill 의 skill effect type () 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatus(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32	lAtkPenalty		= m_pagpmCombat->CalcAtkPenaltyLevelGap((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget);

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
		UINT64 lSpecialStatus = (UINT64)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_lSpecialStatus;

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_STUN)
		{
			INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 lStunProbability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				lStunProbability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// 먼저 공격자의 스턴 확률을 체크한다.
			if(lStunProbability - lAtkPenalty < m_csRandom->randInt(100))
				return FALSE;

			// 다른 모듈에서 특수상태와 관련된 적용해야할 값이 있는지 가져온다.

			UINT64	ulSpecialStatus		= (UINT64)	AGPDCHAR_SPECIAL_STATUS_STUN;
			UINT32	ulAdjustDuration	= 0;
			INT32	lAdjustProbability	= 0;

			PVOID	pvBuffer[4];
			pvBuffer[0]	= (PVOID)	&ulSpecialStatus;
			pvBuffer[1]	= (PVOID)	pcsSkill;
			pvBuffer[2]	= (PVOID)	&ulAdjustDuration;
			pvBuffer[3]	= (PVOID)	&lAdjustProbability;

			m_pagsmSkill->EnumCallback(AGSMSKILL_CB_CHECK_SPECIAL_STATUS_FACTOR, pcsTarget, pvBuffer);

			// 2004.12.16. steeple
			// 방어자가 Stun Protect 상태이면 Stun 안 걸릴 확률을 높여준다.
			if(((AgpdCharacter*)pcsTarget)->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT)
			{
				AgsdCharacter* pcsAgsdTarget = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
				if(pcsAgsdTarget)
					lAdjustProbability += pcsAgsdTarget->m_stInvincibleInfo.lStunProbability;
			}

			if (lAdjustProbability > 0)
			{
				// 스턴 실패...
				if (m_csRandom->randInt(100) < lAdjustProbability)
					return FALSE;
			}

			UINT32	ulStunDuration	= (UINT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_STUN_TIME][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				ulStunDuration += (UINT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_STUN_TIME][lSkillLevel];

			if (ulStunDuration == 0)
				ulStunDuration	= 3000;			// 기본은 3초로 한다.

			ulStunDuration -= ulAdjustDuration;
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_STUN, ulStunDuration);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_FREEZE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_FREEZE, ulDuration);
		}
		
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SLOW)
		{
			// Slow 같은 경우는 UpdateFactor로 movement를 감소시키는 스킬이다.

			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_SLOW, ulDuration);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_INVINCIBLE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_INVINCIBLE, ulDuration);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE, ulDuration);

			// 확률을 세팅해준다. - 2007.06.30. steeple
			AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
			if(pcsAgsdSkill && pcsAgsdCharacter)
			{
				pcsAgsdSkill->m_stInvincibleInfo.lAttrProbability = m_pagsmSkill->GetSkillRate(pcsSkill);
				pcsAgsdCharacter->m_stInvincibleInfo.lAttrProbability += pcsAgsdSkill->m_stInvincibleInfo.lAttrProbability;
			}
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_NOT_ADD_AGRO)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_NOT_ADD_AGRO, ulDuration);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_HIDE_AGRO)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO, ulDuration);
		}

		// 2004.12.13. steeple 추가
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_STUN_PROTECT)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT, ulDuration);

			// 확률을 세팅해준다. - 2007.06.30. steeple
			AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
			if(pcsAgsdSkill && pcsAgsdCharacter)
			{
				pcsAgsdSkill->m_stInvincibleInfo.lStunProbability = m_pagsmSkill->GetSkillRate(pcsSkill);
				pcsAgsdCharacter->m_stInvincibleInfo.lStunProbability += pcsAgsdSkill->m_stInvincibleInfo.lStunProbability;
			}
		}

		// 2005.07.07. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_TRANSPARENT)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT, ulDuration);
		}

		// 2005.10.17. steeple 추가
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_HALT)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_HALT, ulDuration);
		}

		// 2006.09.20. steeple 추가
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_HALF_TRANSPARENT)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT, ulDuration);
		}

		// 2007.06.01. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE, ulDuration);

			// 확률을 세팅해준다. - 2007.06.30. steeple
			AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
			if(pcsAgsdSkill && pcsAgsdCharacter)
			{
				pcsAgsdSkill->m_stInvincibleInfo.lNormalATKProbability = m_pagsmSkill->GetSkillRate(pcsSkill);
				pcsAgsdCharacter->m_stInvincibleInfo.lNormalATKProbability += pcsAgsdSkill->m_stInvincibleInfo.lNormalATKProbability;
			}
		}

		// 2007.06.01. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE, ulDuration);

			// 확률을 세팅해준다. - 2007.06.30. steeple
			AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
			if(pcsAgsdSkill && pcsAgsdCharacter)
			{
				pcsAgsdSkill->m_stInvincibleInfo.lSkillATKProbability = m_pagsmSkill->GetSkillRate(pcsSkill);
				pcsAgsdCharacter->m_stInvincibleInfo.lSkillATKProbability += pcsAgsdSkill->m_stInvincibleInfo.lSkillATKProbability;
			}
		}

		// 2007.06.01. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISABLE_SKILL)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISABLE_SKILL, ulDuration);
		}

		// 2007.06.01. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISABLE_NORMAL_ATK)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISABLE_NORMAL_ATK, ulDuration);
		}

		// 2007.06.01. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SLEEP)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_SLEEP, ulDuration);
		}

		// 2007.06.30. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SLOW_INVINCIBLE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_SLOW_INVINCIBLE, ulDuration);

			// 확률을 세팅해준다. - 2007.06.30. steeple
			AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
			if(pcsAgsdSkill && pcsAgsdCharacter)
			{
				pcsAgsdSkill->m_stInvincibleInfo.lSlowProbability = m_pagsmSkill->GetSkillRate(pcsSkill);
				pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability += pcsAgsdSkill->m_stInvincibleInfo.lSlowProbability;
			}
		}

		// 2008.11.03. iluvs
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISARMAMENT)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			INT32 lPart = 0;

			if(!ProcessSkillEffectSpecialStatusDisArmament(pcsSkill, pcsTarget, &lPart))
				return FALSE;

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISARMAMENT, ulDuration);

			// 캐릭터의 Special Status 에 Set 해준다.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISARMAMENT, ulDuration, lPart);
		}

		// 2009.04.23. iluvs
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISABLE_CHATTING)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			m_pagpmCharacter->UpdateSetSpecialStatus((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISABLE_CHATTING);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_HOLD)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// 먼저 공격자의 스턴 확률을 체크한다.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;
		
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_HOLD, ulDuration);

			//Implementation process about special_status_hold 
			ProcessSkillEffectSpecialStatusHold(pcsSkill, pcsTarget);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_CONFUSION)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// 먼저 공격자의 스턴 확률을 체크한다.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_CONFUSION, ulDuration);

			//Implementation process about special_status_confusion
			ProcessSkillEffectSpecialStatusConfusion(pcsSkill, pcsTarget);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_FEAR)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// 먼저 공격자의 스턴 확률을 체크한다.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_FEAR, ulDuration);

			//Implementation process about special_status_fear
			ProcessSkillEffectSpecialStatusFear(pcsSkill, pcsTarget);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISEASE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// 먼저 공격자의 스턴 확률을 체크한다.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISEASE, ulDuration);

			//Implementation process about special_status_disease
			ProcessSkillEffectSpecialStatusDisease(pcsSkill, pcsTarget);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_BERSERK)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// 먼저 공격자의 스턴 확률을 체크한다.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_BERSERK, ulDuration);

			//Implementation process about special_status_berserk
			ProcessSkillEffectSpecialStatusBerserk(pcsSkill, pcsTarget);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SHRINK)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// 먼저 공격자의 스턴 확률을 체크한다.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_SHRINK, ulDuration);

			//Implementation process about special_status_shrink
			ProcessSkillEffectSpecialStatusShrink(pcsSkill, pcsTarget);
		}
	}

	return TRUE;
}

// 2008.11.04. iluvs
// DisArmament Effect인 Skill을 처리한다.
// (이 경우는 캐릭터에 적용되는 것이아닌 아이템에 적용되므로 따로 처리한다.)
BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusDisArmament(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT32 *lParts)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;
/*		
	// 이 스킬의 확률을 구해서 성공일때만 적용시킨다.
	INT32 lRandom	 = m_csRandom->randInt(100);
	INT32 nSkillRate = m_pagsmSkill->GetSkillRate(pcsSkill);
	INT16 nSize		 = 0;
	
	if(nSkillRate < lRandom) 
		return FALSE;
*/
	// 현재 Parts가 빠져있는 상태면 일단 중복으로는 안되게 한다.
	if(((AgpdCharacter*)pcsTarget)->m_ulSpecialStatusReserved != 0)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	AgpmItemPart eParts = (AgpmItemPart)((INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_ITEM_PARTS][lSkillLevel]);

	AgpdItem *pcsEquipItem = NULL;

	BOOL bDefaultParts = TRUE;

	// eParts에 아무것도 세팅이 안되어있으면 default로 무기를 떨군다.
	if(eParts < AGPMITEM_PART_BODY || eParts > AGPMITEM_PART_V_LANCER || eParts == AGPMITEM_PART_HAND_LEFT || eParts == AGPMITEM_PART_HAND_RIGHT)
	{
		pcsEquipItem = m_pagpmItem->GetEquipWeapon((AgpdCharacter*)pcsTarget);
		if(!pcsEquipItem)
			return FALSE;
	}
	else
	{
		pcsEquipItem = m_pagpmItem->GetEquipSlotItem((AgpdCharacter*)pcsTarget, eParts);
		if(NULL == pcsEquipItem)
			return FALSE;

		bDefaultParts = FALSE;

		*lParts = (INT32)eParts;
	}

	BOOL bEquipDefaultItem = TRUE;

	if(m_pagpmItem->GetWeaponType(pcsEquipItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER)
	{
		AgpdItem* pcsLeftItem = m_pagpmItem->GetEquipSlotItem((AgpdCharacter*)pcsTarget, AGPMITEM_PART_HAND_LEFT);
		if(pcsLeftItem && pcsLeftItem->m_pcsItemTemplate && pcsLeftItem->m_lID != pcsEquipItem->m_lID &&
			m_pagpmItem->GetWeaponType(pcsLeftItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
		{
			if(pcsLeftItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
				m_pagpmItem->EnumCallback(ITEM_CB_ID_CHAR_UNEQUIP, pcsLeftItem, &bEquipDefaultItem);

			m_pagpmItem->EnumCallback(ITEM_CB_ID_REMOVE_FOR_NEAR_CHARACTER, pcsLeftItem, NULL);
		}
	}

	if(pcsEquipItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
		m_pagpmItem->EnumCallback(ITEM_CB_ID_CHAR_UNEQUIP, pcsEquipItem, &bEquipDefaultItem);

	m_pagpmItem->EnumCallback(ITEM_CB_ID_REMOVE_FOR_NEAR_CHARACTER, pcsEquipItem, NULL);

	// 어떤 Part가 빠졌는제 저장
	if(bDefaultParts)
	{
		((AgpdCharacter*)pcsTarget)->m_ulSpecialStatusReserved = (INT32)AGPMITEM_PART_HAND_RIGHT;
	}
	else
	{
		((AgpdCharacter*)pcsTarget)->m_ulSpecialStatusReserved = (INT32)eParts;
	}
	
	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusConfusion(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_CONFUSION;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusHold(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_HOLD;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusFear(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_FEAR;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusDisease(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget || NULL == pcsSkill->m_pcsTemplate)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_DISEASE;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here
	INT32 lSkillLevel		= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	INT32 lStunDurationTime = static_cast<INT32>(((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_STUN_TIME][lSkillLevel]);
	INT32 lStunGenerateTime = static_cast<INT32>(((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_STUN_GENERATE_TIME][lSkillLevel]);

	if(lStunDurationTime < 0 || lStunGenerateTime < 0)
		return FALSE;

	pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam1 = lStunDurationTime;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam2 = lStunGenerateTime;

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusBerserk(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_BERSERK;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusShrink(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_SHRINK;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectLifeProtection(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
		AgsdCharacter	*pcsAgsdCharacter		= m_pagsmCharacter->GetADCharacter((AgpdCharacter *) pcsTarget);
		if (!pcsAgsdCharacter)
			return FALSE;

		INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
		if(!lSkillLevel)
			return FALSE;

		pcsAgsdCharacter->m_bResurrectWhenDie	= TRUE;
		pcsAgsdCharacter->m_lResurrectHP		= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_HP_RECOVERY][lSkillLevel];
	}

	return TRUE;
}

//		ProcessSkillEffectDOT
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_DOT_DAMAGE) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectDOT(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
		AgsdCharacter	*pcsAgsdTarget	= m_pagsmCharacter->GetADCharacter((AgpdCharacter *) pcsTarget);
		if (pcsAgsdTarget && pcsAgsdTarget->m_bIsMaxDefence)
			return TRUE;
		if (pcsAgsdTarget && pcsAgsdTarget->m_bIsSuperMan)
			return TRUE;
	}

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	if(!bProcessInterval)
	{
		// 총 적용되야 하는 횟수를 구한다.
		// 1을 배주는 이유는, DOT 스킬은 Duration 을 Interval 만큼 더 더했다. 그래서 해야할 계산보다 1번 더 들어가기 때문에 1 빼준다.
		UINT32 ulSkillInterval = m_pagpmSkill->GetSkillInterval(pcsSkill, lSkillLevel);
		if(ulSkillInterval != 0)
			pcsAgsdSkill->m_nTotalDOTCount = (INT16)(m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel) / ulSkillInterval) - 1;

		return TRUE;
	}


	// 새로 구현된 부분
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	AgpdSkillTemplate	*pcsSkillTemplate = (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	INT32	nDamage		= 0;
	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	BOOL	bIsPoisonDamage	= FALSE;

	AgpdFactorAttribute	stAttrDamage;
	ZeroMemory(&stAttrDamage, sizeof(AgpdFactorAttribute));

	if (bProcessInterval)
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_PHYSICAL)
		{
			nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
			stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];

			// 2007.10.24. steeple
			if(bCheckConnection)
			{
				nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
				stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
			}

			// 파티에 따라 보너스 데미지가 있는지 검사한다.
			//////////////////////////////////////////////////////////////////////////////////
//			if (nDamage > 0 && !pcsSkill->m_bCloneObject)
//				nDamage += (INT32) (nDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));
		}

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_HEROIC)
		{
			nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
			stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];

			// 2007.10.24. steeple
			if(bCheckConnection)
			{
				nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
				stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
			}		
		}


		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_ATTRIBUTE)
		{
			INT32 lSpiritConstIndex = 0;
			// 여기에서 Poison 값이 0보다 크면 bIsPoisonDamage를 TRUE로 세팅해줘야 한다.
			for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL + 1; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
			{
				lSpiritConstIndex = m_pagsmSkill->GetSpiritDOTConstIndexByFactor((AgpdFactorAttributeType)i);
				if(lSpiritConstIndex < AGPMSKILL_CONST_DOT_DMG_MAGIC || 
					lSpiritConstIndex > AGPMSKILL_CONST_DOT_DMG_THUNDER)
					continue;

				nDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor[lSpiritConstIndex][lSkillLevel];
				stAttrDamage.lValue[i] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[lSpiritConstIndex][lSkillLevel];

				// 2007.10.24. steeple
				if(bCheckConnection)
				{
					nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[lSpiritConstIndex][lSkillLevel];
					stAttrDamage.lValue[i] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[lSpiritConstIndex][lSkillLevel];
				}

				if (i == AGPD_FACTORS_ATTRIBUTE_TYPE_POISON && stAttrDamage.lValue[i] > 0)
					bIsPoisonDamage	= TRUE;
			}
		}

		// 다른 모듈에서 Poison과 관련된 적용해야할 값이 있는지 가져온다.
		if (bIsPoisonDamage)
		{
			UINT64	ulSpecialStatus		= (UINT64)	AGPDCHAR_SPECIAL_STATUS_SLOW;
			UINT32	ulAdjustDuration	= 0;
			INT32	lAdjustProbability	= 0;

			PVOID	pvBuffer[2];
			pvBuffer[0]	= (PVOID)	&lAdjustProbability;
			pvBuffer[1]	= (PVOID)	&ulAdjustDuration;

			m_pagsmSkill->EnumCallback(AGSMSKILL_CB_CHECK_POISON_STATUS_FACTOR, pcsTarget, pvBuffer);

			if (ulAdjustDuration > 0)
			{
				pcsSkill->m_ulEndTime	-= ulAdjustDuration;
			}
		}
	}
	else
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_PHYSICAL)
		{
			nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
			stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];

			// 2007.10.24. steeple
			if(bCheckConnection)
			{
				nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
				stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
			}

			// 파티에 따라 보너스 데미지가 있는지 검사한다.
			//////////////////////////////////////////////////////////////////////////////////
//			if (nDamage > 0 && !pcsSkill->m_bCloneObject)
//				nDamage += (INT32) (nDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));
		}

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_HEROIC)
		{
			nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
			stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];

			// 2007.10.24. steeple
			if(bCheckConnection)
			{
				nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
				stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
			}		
		}

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_ATTRIBUTE)
		{
			INT32 lSpiritConstIndex = 0;
			// 여기에서 Poison 값이 0보다 크면 bIsPoisonDamage를 TRUE로 세팅해줘야 한다.
			for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL + 1; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
			{
				lSpiritConstIndex = m_pagsmSkill->GetSpiritDOTConstIndexByFactor((AgpdFactorAttributeType)i);
				if(lSpiritConstIndex < AGPMSKILL_CONST_DOT_DMG_MAGIC || 
					lSpiritConstIndex > AGPMSKILL_CONST_DOT_DMG_THUNDER)
					continue;

				nDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor[lSpiritConstIndex][lSkillLevel];
				stAttrDamage.lValue[i] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[lSpiritConstIndex][lSkillLevel];

				// 2007.10.24. steeple
				if(bCheckConnection)
				{
					nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[lSpiritConstIndex][lSkillLevel];
					stAttrDamage.lValue[i] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[lSpiritConstIndex][lSkillLevel];
				}

				if (i == AGPD_FACTORS_ATTRIBUTE_TYPE_POISON && stAttrDamage.lValue[i] > 0)
					bIsPoisonDamage	= TRUE;
			}
		}

		// 다른 모듈에서 Poison과 관련된 적용해야할 값이 있는지 가져온다.
		if (bIsPoisonDamage)
		{
			UINT64	ulSpecialStatus		= (UINT64)	AGPDCHAR_SPECIAL_STATUS_SLOW;
			UINT32	ulAdjustDuration	= 0;
			INT32	lAdjustProbability	= 0;

			PVOID	pvBuffer[2];
			pvBuffer[0]	= (PVOID)	&lAdjustProbability;
			pvBuffer[1]	= (PVOID)	&ulAdjustDuration;

			m_pagsmSkill->EnumCallback(AGSMSKILL_CB_CHECK_POISON_STATUS_FACTOR, pcsTarget, pvBuffer);

			if (ulAdjustDuration > 0)
			{
				pcsSkill->m_ulEndTime	-= ulAdjustDuration;
			}
		}
	}

	if (nDamage < 1)
		return TRUE;

	/*
	// defense나 reflect를 하는지 검사한다.
	stAgsmCombatAttackResult	stAttackResult;
	stAttackResult.pAttackChar = (AgpdCharacter *) pcsSkill->m_pcsBase;
	stAttackResult.pTargetChar = (AgpdCharacter *) pcsTarget;
	stAttackResult.nDamage = nDamage;

	INT32	lReflectDamage = 0;

	EnumCallback(AGSMSKILL_CB_MELEE_ATTACK_CHECK, &stAttackResult, &lReflectDamage);

	nDamage = stAttackResult.nDamage;
	*/

	// Damage를 반영한다.
	//		실제 캐릭터에 반영하고 History에도 반영한다.
	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (!pcsFactorCharPoint)
			return FALSE;
	}

	// 2005.04.01. steeple
	// 지속형으로 BuffedSkill 로 적용되는 거라면 데미지를 그냥 대입하고 아니면 더해준다.
	if(bProcessInterval)
	{
		pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] = (-nDamage);
		for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
			pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL + i] = (-stAttrDamage.lValue[i]);	// 요기도 그냥 대입

		// Heroic Damage는 따로 계산
		if(stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] > 0)
			pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC] = (-stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC]);

		// 이거 하나 증가시켜준다. 2006.11.14. steeple
		++pcsAgsdSkill->m_nAffectedDOTCount;
	}
	else
	{
		//pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-nDamage);
		//for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
		//	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL + i] += (-stAttrDamage.lValue[i]);

	}

	/*
	if (lReflectDamage > 0)
	{
		// 공격한 넘한테 lReflectDamage 를 되돌려준다.
		ProcessReflectDamage(pcsTarget, pcsSkill->m_pcsBase, lReflectDamage);
	}
	*/

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectDMGAdjust(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsSkill->m_pcsBase || !pcsTarget || !pcsSkill->m_pcsTemplate)
		return FALSE;

	AgsdSkillADBase	*pcsADBase		= m_pagsmSkill->GetADBase(pcsTarget);
	if (!pcsADBase)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;

	INT32	lSkillLevel				= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32	lDMGAdjust				= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DMG_ADJUST][lSkillLevel];

	//pcsADBase->m_lDamageAdjustRate	= (INT32) (100 / (1 + lDMGAdjust / 100.0));
	pcsADBase->m_lDamageAdjustRate	= (INT32) (1 + lDMGAdjust / 100.0);

	return TRUE;
}

//		ProcessSkillEffectSkillFactorAdjust
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			(arg1의 확률로 아래 값들(arg2)중 하나를 모든 스킬의 사용시에 체크하여 적용한다. (변화 단위 : %))
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_HP
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_MP
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_SP
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_RANGE
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_CAST_TIME
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_DURATION_TIME	20057.09. steeple
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectSkillFactorAdjust(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	UINT64				ullEffectType	= 0;

	if (bProcessInterval)
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex];
	else
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex];

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData)
		return FALSE;

	INT32	lSkillLevel	= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	//	cost hp
	if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_HP)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_HP;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostHP				+= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg2;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostHPProbability	+= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;

		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCostHP				= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg2;
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCostHPProbability		= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
	}

	//	cost mp
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_MP)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_MP;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostMP				+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_COST_MP_DECREASE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCostMP				= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_COST_MP_DECREASE][lSkillLevel];

		// 2007.10.25. steeple
		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostMP += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_COST_MP_DECREASE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCostMP += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_COST_MP_DECREASE][lSkillLevel];
		}

		// 2005.04.28. steeple
		// 옵티멈 스킬 때문에 lCostMP 약간 변화 생김.

		// 2005.01.31. steeple
		// 아래 값이 세팅되어 있다면 SKILL_RATE 의 확률로 COSTMP 를 0 으로 한다.
		if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1)
		{
			pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostMPProbability = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCostMPProbability  = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		}
		
		//pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostMPProbability	= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
	}

	/*
	//	cost sp
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_SP)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_SP;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostSP				= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg2;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostSPProbability	= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
	}
	*/

	//	range
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_RANGE)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect									|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_RANGE;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lRangeAdjustPercent			+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RANGE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lRangeAdjustPercent			= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RANGE][lSkillLevel];

		// 2007.10.25. steeple
		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillFactorEffectArg.lRangeAdjustPercent += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RANGE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lRangeAdjustPercent += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RANGE][lSkillLevel];
		}

		/*
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lMagicSkillRange				= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg2;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lMagicSkillRangeProbability	= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
		*/
	}

	//	cast time
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_CAST_TIME)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_CAST_TIME;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCastTime				+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_CAST_TIME_DECREASE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCastTime				= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_CAST_TIME_DECREASE][lSkillLevel];

		// 2007.10.25. steeple
		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillFactorEffectArg.lCastTime += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CAST_TIME_DECREASE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCastTime += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CAST_TIME_DECREASE][lSkillLevel];
		}

		//pcsAttachData->m_stBuffedSkillFactorEffectArg.lCastTimeProbability	= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
	}

	//	Duration Time 2005.07.09. steeple
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_DURATION_TIME)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_DURATION_TIME;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lDurationTime			+= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_ADDITIONAL_DURATION][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lDurationTime			= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_ADDITIONAL_DURATION][lSkillLevel];

		// 2007.10.25. steeple
		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillFactorEffectArg.lDurationTime += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_ADDITIONAL_DURATION][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lDurationTime += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_ADDITIONAL_DURATION][lSkillLevel];
		}
	}

	// Buffed Skill Duration Time
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_BUFFED_SKILL_DURATION)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_BUFFED_SKILL_DURATION;

		ProcessSkillEffectBuffedSkillDurationTime(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	else
		return FALSE;

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectBuffedSkillDurationTime(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget || NULL == pcsSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillAttachData *pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(NULL == pcsAttachData)
		return FALSE;

	AgsdSkillADBase *pcsADBase = m_pagsmSkill->GetADBase(pcsTarget);
	if(NULL == pcsADBase)
		return FALSE;

	// Check Target has buffedSkill
	if(pcsAttachData->m_astBuffSkillList[0].lSkillID == AP_INVALID_SKILLID)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	for(int i=0; i < AGPMSKILL_MAX_SKILL_BUFF; ++i)
	{
		if(pcsAttachData->m_astBuffSkillList[i].lSkillID == AP_INVALID_SKILLID ||
			!pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill)
			break;

		AgpdSkillTemplate *pcsBuffedSkillTemplate = pcsAttachData->m_astBuffSkillList[i].pcsSkillTemplate;
		if(NULL == pcsBuffedSkillTemplate)
			continue;

		if(m_pagpmSkill->IsDebuffSkill(pcsBuffedSkillTemplate) == FALSE)
			continue;

		BOOL bAdjust = FALSE;

		// Check All or Not Debuffed Skill Update Duration Time
		if(((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_bSpecialStatusMatchingAll == TRUE)
		{
			bAdjust = TRUE;
		}
		else
		{
			if(((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_lSpecialStatusMatching & pcsBuffedSkillTemplate->m_lSpecialStatus)
			{
				bAdjust = TRUE;
			}
		}

		// Adjust Buffed Skill Duration at the Buffed Skill
		//////////////////////////////////////////////////////////////////////////////////////////
		if(bAdjust)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			INT32 lAdjustTime = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_TIME_CONTROL][lSkillLevel];

			pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill->m_ulEndTime += lAdjustTime;

			if(bCheckConnection)
			{
				lAdjustTime = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_TIME_CONTROL][lSkillLevel];
				pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill->m_ulEndTime += lAdjustTime;
			}
		}
	}

	return TRUE;
}

//		ProcessSkillEffect2UpdateCombatPoint
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_UPDATE_COMBAT_POINT) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			ullEffectType 을 잘 보고 값을 세팅해준다.
//						AGPMSKILL_EFFECT_UPDATE_COMBAT_POINT_TYPE1
//						AGPMSKILL_EFFECT_UPDATE_COMBAT_POINT_TYPE2
//						AGPMSKILL_EFFECT_UPDATE_COMBAT_POINT_TYPE3
//						AGPMSKILL_EFFECT_UPDATE_COMBAT_POINT_TYPE4
//
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffect2UpdateCombatPoint(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	//UINT64				ullEffectType	= 0;

	//if (bProcessInterval)
	//	ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex];
	//else
	//	ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex];

	UINT32 ulEffectDetailType = 0;
	if(bProcessInterval)
		ulEffectDetailType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT];
	else
		ulEffectDetailType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT];

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// 2005.12.22. steeple
	// Hit Rate 가 Factor 로 변경되었다. 
	//
	//
	//// Hit Rate
	//if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE1)
	//{
	//	pcsAttachData->m_stBuffedSkillCombatEffectArg.lHitRate += ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_HIT_RATE][lSkillLevel];
	//	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lHitRate = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_HIT_RATE][lSkillLevel];

	//	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT;
	//	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE1;
	//}

	// 무기 데미지 최대 데미지로 고정
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE2)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lWeaponDamageType = AGPMSKILL_WEAPON_DAMAGE_TYPE_MAX;
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lWeaponDamageType = AGPMSKILL_WEAPON_DAMAGE_TYPE_MAX;

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE2;
	}

	// 무기 데미지 최소 데미지로 고정
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE3)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lWeaponDamageType = AGPMSKILL_WEAPON_DAMAGE_TYPE_MIN;
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lWeaponDamageType = AGPMSKILL_WEAPON_DAMAGE_TYPE_MIN;

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE3;
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2RegenHP(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lHPRegen = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REGEN_HP][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lHPRegen += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_REGEN_HP][lSkillLevel];

	pcsAttachData->m_stBuffedSkillFactorEffectArg.lHPRegen += pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lHPRegen;

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2RegenMP(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lMPRegen = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REGEN_MP][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lMPRegen += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_REGEN_MP][lSkillLevel];

	pcsAttachData->m_stBuffedSkillFactorEffectArg.lMPRegen += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REGEN_MP][lSkillLevel];

	return TRUE;
}

//		ProcessSkillEffectAbsorb
//	Functions
//		- HP, MP 의 변환을 처리
//
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffect2Convert(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	//UINT64 ullEffectType = 0;

	//if(bProcessInterval)
	//	ullEffectType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex];
	//else
	//	ullEffectType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex];

	UINT32 ulEffectDetailType = 0;
	if(bProcessInterval)
		ulEffectDetailType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT];
	else
		ulEffectDetailType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT];

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	// 데미지를 HP 로 흡수
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_HP)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[0] = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[0] = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
		}

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_CONVERT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_HP;
	}

	// 데미지를 MP 로 흡수
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_MP)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[0] = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[0] = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
		}

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_CONVERT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_MP;
	}

	// 2005.03.30. steeple 약간 변경
	// CONST 수치를 MP 까지 않고 걍 HP 에 더해준다.
	//
	// MP 를 HP 로 흡수
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_MP_TO_HP)
	{
		AgpdCharacter* pcsAgpdTarget = (AgpdCharacter*)pcsTarget;
		INT32 lChangeValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_MP_CONVERT_HP][lSkillLevel];
		
		if(bCheckConnection)
			lChangeValue += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_MP_CONVERT_HP][lSkillLevel];

		// 현재 HP, MP, MAX HP 를 얻는다.
		INT32 lCurrentHP, lCurrentMP, lMaxHP;
		lCurrentHP = lCurrentMP = lMaxHP = 0;

		m_pagpmFactors->GetValue(&pcsAgpdTarget->m_csFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pagpmFactors->GetValue(&pcsAgpdTarget->m_csFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->GetValue(&pcsAgpdTarget->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

		//if(lChangeValue > lCurrentMP)
		//	lChangeValue = lCurrentMP;

		if((lChangeValue + lCurrentHP) > lMaxHP)
			lChangeValue = lMaxHP - lCurrentHP;

		//lCurrentMP -= lChangeValue;		// MP 는 까주고,
		lCurrentHP += lChangeValue;		// HP 는 더해준다.

		// 다시 세팅
		PVOID pvPacket = NULL;
		m_pagsmFactors->UpdateCharPoint(&pcsAgpdTarget->m_csFactor, &pvPacket, lCurrentHP, lCurrentMP, 0);

		// 변경 했다면 패킷 보내버린다.
		if(pvPacket)
		{
			m_pagsmCharacter->SendPacketFactor(pvPacket, pcsAgpdTarget, PACKET_PRIORITY_4);
			m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);
		}
	}

	// 2005.07.12. steeple
	// 공격 데미지를 HP 로 변환
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_HP)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[1] = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[1] = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
		}

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_CONVERT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_HP;
	}

	// 2005.07.12. steeple
	// 공격 데미지를 MP 로 변환
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_MP)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[1] = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[1] = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
		}

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_CONVERT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_MP;
	}

	// 2007.07.04. steeple
	// 공격 데미지를 HP 로 바로 변환. 한방형
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_HP_INSTANT)
	{
		// 여기서 크게 할 일은 없다.
		// 실 데미지가 들어가는 부분에서 처리해준다.
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2Charge(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// Charge 를 세팅해준다. 첫 단계는 NONE
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_CHARGE;

	pcsAttachData->m_stBuffedSkillCombatEffectArg.lChargeLevel = AGPMSKILL_CHARGE_NONE;
	pcsAttachData->m_lMeleeComboInCharge = 0;

	// AgsdSkill 의 첫 단계도 NONE, 그러나 Damage 는 값을 주자.
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lChargeLevel = AGPMSKILL_CHARGE_NONE;
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lChargeDamage = 1;

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2SkillLevelUp(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE1)
			bResult |= ProcessSkillEffect2SkillLevelUPType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE2)
			bResult |= ProcessSkillEffect2SkillLevelUPType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE3)
			bResult |= ProcessSkillEffect2SkillLevelUPType3(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE1)
			bResult |= ProcessSkillEffect2SkillLevelUPType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE2)
			bResult |= ProcessSkillEffect2SkillLevelUPType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE3)
			bResult |= ProcessSkillEffect2SkillLevelUPType3(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

BOOL AgsmSkillEffect::ProcessSkillEffectProduct(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	// do nothing
	return TRUE;
}

// 2005.07.18. steeple
// Action on Action Type3 
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType3(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	// 그냥 일단 세팅만 해놓는다.
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lActionOnActionType3Count++;
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lActionOnActionType3Count = 1;

	return TRUE;
}

// 2005.12.13. steeple
// Action on Action Type4. ex) Lens Stone
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType4(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	// 그냥 일단 세팅만 해놓는다.
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lActionOnActionType4Count = 1;

	return TRUE;
}

// 2005.07.07. steeple
// AT Field 형 스킬을 처리
BOOL AgsmSkillEffect::ProcessSkillEffect2ATField(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK)
			bResult |= ProcessSkillEffect2ATFieldAttack(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK2)
			bResult |= ProcessSkillEffect2ATFieldAttack2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_AURA)
			bResult |= ProcessSkillEffect2ATFieldAura(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK)
			bResult |= ProcessSkillEffect2ATFieldAttack(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK2)
			bResult |= ProcessSkillEffect2ATFieldAttack2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_AURA)
			bResult |= ProcessSkillEffect2ATFieldAura(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2005.07.15. steeple
// 소환쪽 관련
BOOL AgsmSkillEffect::ProcessSkillEffect2Summons(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE1)
			bResult |= ProcessSkillEffect2SummonsType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE2)
			bResult |= ProcessSkillEffect2SummonsType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE3)
			bResult |= ProcessSkillEffect2SummonsType3(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE4)
			bResult |= ProcessSkillEffect2SummonsType4(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE5)
			bResult |= ProcessSkillEffect2SummonsType5(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE6)
			bResult |= ProcessSkillEffect2SummonsType6(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE7)
			bResult |= ProcessSkillEffect2SummonsType7(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE8)
			bResult |= ProcessSkillEffect2SummonsType8(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE9)
			bResult |= ProcessSkillEffect2SummonsType9(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE10)
			bResult |= ProcessSkillEffect2SummonsType10(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE1)
			bResult |= ProcessSkillEffect2SummonsType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE2)
			bResult |= ProcessSkillEffect2SummonsType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE3)
			bResult |= ProcessSkillEffect2SummonsType3(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE4)
			bResult |= ProcessSkillEffect2SummonsType4(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE5)
			bResult |= ProcessSkillEffect2SummonsType5(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE6)
			bResult |= ProcessSkillEffect2SummonsType6(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE7)
			bResult |= ProcessSkillEffect2SummonsType7(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE8)
			bResult |= ProcessSkillEffect2SummonsType8(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE9)
			bResult |= ProcessSkillEffect2SummonsType9(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE10)
			bResult |= ProcessSkillEffect2SummonsType10(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2005.07.15. steeple
// Action Passive Skill
BOOL AgsmSkillEffect::ProcessSkillActionPassive(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// 여기서는 그다지 할일 없다.
	// Post Attack 에서 Passive Skill 을 한번씩 돌면서 체크해야 할 듯 싶다.

	return TRUE;
}

// 2005.11.25. steeple
// Game Bonus Skill
BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonus(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_EXP)
			bResult |= ProcessSkillEffect2GameBonusExp(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_MONEY)
			bResult |= ProcessSkillEffect2GameBonusMoney(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE)
			bResult |= ProcessSkillEffect2GameBonusDropRate(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE2)
			bResult |= ProcessSkillEffect2GameBonusDropRate2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_CHARISMA_RATE)
			bResult |= ProcessSkillEffect2GameBonusCharismaRate(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_EXP)
			bResult |= ProcessSkillEffect2GameBonusExp(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_MONEY)
			bResult |= ProcessSkillEffect2GameBonusMoney(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE)
			bResult |= ProcessSkillEffect2GameBonusDropRate(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE2)
			bResult |= ProcessSkillEffect2GameBonusDropRate2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_CHARISMA_RATE)
			bResult |= ProcessSkillEffect2GameBonusCharismaRate(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2007.07.04. steeple
// Detect Type 이 늘어나면서 추가 코딩 했다.
BOOL AgsmSkillEffect::ProcessSkillEffect2Detect(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] & AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE1)
			bResult |= ProcessSkillEffect2DetectType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] & AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE2)
			bResult |= ProcessSkillEffect2DetectType2(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] & AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE1)
			bResult |= ProcessSkillEffect2DetectType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] & AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE2)
			bResult |= ProcessSkillEffect2DetectType2(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2006.10.10. steeple
// Ride Skill
// 스킬 사용시 탈것에 타고, 한번 더 사용하면 내려온다.
// 그러므로, 쿨타임이 거의 없어야 함.
BOOL AgsmSkillEffect::ProcessSkillEffect2Ride(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// 2007.04.24. steeple
	// 아크로드 스킬이고, 현재 아크로드 공성이 진행중이면 쓸 수 없다.
	if(m_pagpmSkill->IsArchlordSkill(pcsSkillTemplate) && m_pagpmCharacter->IsArchlord((AgpdCharacter*)pcsTarget))
	{
		AgpdSiegeWar* pcsSiegeWar = m_pagpmSiegeWar->GetArchlordCastle();
		if(pcsSiegeWar && m_pagpmSiegeWar->IsStarted(pcsSiegeWar))
			return TRUE;
	}

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lRideTID = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];

	m_pagsmSkill->EnumCallback(AGSMSKILL_CB_RIDE, static_cast<AgpdCharacter*>(pcsTarget), &lRideTID);

	return TRUE;
}

// 2006.10.10. steeple
// Game Effect. Take some effect to the game environment.
BOOL AgsmSkillEffect::ProcessSkillEffect2GameEffect(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_DAY_NIGHT)
			bResult |= ProcessSkillEffect2GameEffectDayNight(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_RAIN)
			bResult |= ProcessSkillEffect2GameEffectRain(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_SNOW)
			bResult |= ProcessSkillEffect2GameEffectSnow(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_DAY_NIGHT)
			bResult |= ProcessSkillEffect2GameEffectDayNight(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_RAIN)
			bResult |= ProcessSkillEffect2GameEffectRain(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_SNOW)
			bResult |= ProcessSkillEffect2GameEffectSnow(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2007.06.26. steeple
// 데미지를 나눈다.
BOOL AgsmSkillEffect::ProcessSkillEffect2Divide(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] & AGPMSKILL_EFFECT_DETAIL_DIVIDE_ATTR)
			bResult |= ProcessSkillEffect2DivideAttr(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] & AGPMSKILL_EFFECT_DETAIL_DIVIDE_NORMAL)
			bResult |= ProcessSkillEffect2DivideNormal(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] & AGPMSKILL_EFFECT_DETAIL_DIVIDE_ATTR)
			bResult |= ProcessSkillEffect2DivideAttr(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] & AGPMSKILL_EFFECT_DETAIL_DIVIDE_NORMAL)
			bResult |= ProcessSkillEffect2DivideNormal(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2007.08.23. steeple
// 대상을 부활시키려는 메시지를 보내야 한다.
BOOL AgsmSkillEffect::ProcessSkillEffect2Resurrection(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!m_pagpmWantedCriminal || !pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	if(m_pagpmCharacter->IsPC((AgpdCharacter*)pcsTarget) == FALSE || _tcslen(((AgpdCharacter*)pcsTarget)->m_szID) == 0)
		return FALSE;

	// 현상 수배범은 부활 시킬 수 없다.
	if(m_pagpmWantedCriminal->GetWantedCriminal(((AgpdCharacter*)pcsTarget)->m_szID))
		return FALSE;

	AgsdCharacter* pcsAgsdTarget = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
	if(pcsAgsdTarget)
		_tcsncpy(pcsAgsdTarget->m_szResurrectionCaster, ((AgpdCharacter*)pcsSkill->m_pcsBase)->m_szID, _tcslen(((AgpdCharacter*)pcsSkill->m_pcsBase)->m_szID));

	// 타겟에게 부활할거냐는 확인창을 띄워준다.
	m_pagsmCharacter->SendPacketResurrectionByOther((AgpdCharacter*)pcsTarget, ((AgpdCharacter*)pcsSkill->m_pcsBase)->m_szID, 0);

	return TRUE;
}

// 2007.10.29. steeple
// Union Type 처리
BOOL AgsmSkillEffect::ProcessSkillEffect2Union(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] & AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE1)
			bResult |= ProcessSkillEffect2UnionType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] & AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE2)
			bResult |= ProcessSkillEffect2UnionType2(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] & AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE1)
			bResult |= ProcessSkillEffect2UnionType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] & AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE2)
			bResult |= ProcessSkillEffect2UnionType2(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}


//		ProcessSkillEffectMeleeAttackType1
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType1(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	// 2004.12.29. steeple
	// 데미지 공식 새로 적용하고, 코드 정리하였음. 불필요한 주석은 삭제 했음.

	if(!pcsSkill || !pcsSkill->m_pcsBase || !pcsTarget)
		return FALSE;

	INT32 lSkillLevel =  m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// 모든 스킬에서 정령 데미지를 검사하여야 한다.
	AgpdFactorAttribute	stAttrDamage;
	ZeroMemory(&stAttrDamage, sizeof(AgpdFactorAttribute));

	INT32 lNormalSkillDamage = m_pagsmSkill->CalcNormalSkillDamage(pcsSkill, pcsTarget);
	INT32 lSpiritSkillDamage = m_pagsmSkill->CalcSpiritSkillDamage(pcsSkill, pcsTarget, &stAttrDamage);
	INT32 lHeroicSkillDamage = m_pagsmSkill->CalcNormalHeroicSkillDamage(pcsSkill, pcsTarget);	

	if(lNormalSkillDamage <= 0)
		lNormalSkillDamage = 1;

	if(lSpiritSkillDamage <= 0)
		lSpiritSkillDamage = 1;

	if(lHeroicSkillDamage <= 0)
		lHeroicSkillDamage = 1;

	INT32 lDamage = lNormalSkillDamage;
	if(lDamage <= 0)
		lDamage	= 1;

	// 파티에 따라 보너스 데미지가 있는지 검사한다.
	if(lDamage > 0 && !pcsSkill->m_bCloneObject)
		lDamage += (INT32) (lDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));

	// Divide Damage 처리해준다. 2007.06.27. steeple
	if(!bProcessInterval)
	{
		m_pagsmSkill->ProcessDivideDamage(pcsSkill, (AgpdCharacter*)pcsTarget, &stAttrDamage, &lDamage, &lSpiritSkillDamage);
	}

	// 높이에 따른 데미지 변화. 2006.06.23. steeple
	if(!bProcessInterval)		// 버프에서 들어온 건 하지 않는다.
	{
		lDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lDamage);
		lSpiritSkillDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lSpiritSkillDamage);
		lHeroicSkillDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lHeroicSkillDamage);
	}

	// 총 물리 데미지를 세팅해준다.
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] = lDamage;
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] = lHeroicSkillDamage;
	lDamage += lSpiritSkillDamage;
	lDamage += lHeroicSkillDamage;

	// Damage를 반영한다.
	// 실제 캐릭터에 반영하고 History에도 반영한다.
	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor* pcsFactorResult = (AgpdFactor*)m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if(!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint* pcsFactorCharPoint = (AgpdFactorCharPoint*)m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if(!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint*) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if(!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-lDamage);
	// 정령 데미지 적용
	for(int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; i++)
		pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL + i] += (-stAttrDamage.lValue[i]);

	// Heroic 데미지 적용
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC] += (-stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC]);

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackType2
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE2) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//		
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType2(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	// 2004.12.29. steeple
	// 불필요한 주석은 삭제 했음.

	if (!pcsSkill || !pcsTarget)
		return FALSE;

	INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate = (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	INT32	lDamage	= (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_A][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
	{
		lDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_A][lSkillLevel];
	}

	// Divide Damage 처리해준다. 2007.06.27. steeple
	if(!bProcessInterval)
	{
		m_pagsmSkill->ProcessDivideDamage(pcsSkill, (AgpdCharacter*)pcsTarget, NULL, &lDamage, NULL);
	}

	// 높이에 따른 데미지 변화. 2006.06.23. steeple
	if(!bProcessInterval)		// 버프에서 들어온 건 하지 않는다.
	{
		lDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lDamage);
	}

	// Damage를 반영한다.
	// 실제 캐릭터에 반영하고 History에도 반영한다.
	AgsdSkill	*pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-lDamage);
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL] += (-lDamage);

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackType3
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE3) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType3(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	// 2004.12.29. steeple
	// MeleeAttackType3 는 MeleeAttackType1 과 동일해졌다.
	return ProcessSkillEffectMeleeAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
}

//		ProcessSkillEffectMeleeAttackType4
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE4) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			공식 : {데미지 상수 * (Air 원소 방어력) + 데미지 상수 * (1-((1-(400/Defense))/2)) (방어력)}
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType4(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdFactor	*pcsAttackFactorResult	= m_pagsmSkill->GetOwnerFactorResult(pcsSkill);
	AgpdFactor	*pcsTargetFactorResult	= m_pagsmSkill->GetResultFactor(pcsTarget);

	if (!pcsAttackFactorResult || !pcsTargetFactorResult)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate =  (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	INT32	lAttackMAP			= 0;
	FLOAT	fTargetAirResist	= 0.0f;

	m_pagpmFactors->GetValue(pcsAttackFactorResult, &lAttackMAP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAP);
	m_pagpmFactors->GetValue(pcsTargetFactorResult, &fTargetAirResist, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);

	BOOL	bIsEquipWeapon		= m_pagpmItem->IsEquipWeapon((AgpdCharacter *) pcsSkill->m_pcsBase);

	// 2004.12.29. steeple 마법사 때문에..
	INT32	lNormalAttackDamage	= m_pagpmCombat->CalcPhysicalAttackForSkill((AgpdCharacter *) pcsSkill->m_pcsBase, (AgpdCharacter *) pcsTarget, bIsEquipWeapon);
	//INT32	lNormalAttackDamage	= m_pagpmCombat->CalcPhysicalAttack((AgpdCharacter *) pcsSkill->m_pcsBase, (AgpdCharacter *) pcsTarget, bIsEquipWeapon);
	if (lNormalAttackDamage < 0)
		lNormalAttackDamage	= 0;

	FLOAT	fAirDefense	= (FLOAT)(lNormalAttackDamage * (lAttackMAP / (fTargetAirResist + 0.0)));

	INT32	nDamage = (INT32)((FLOAT)m_pagsmSkill->GetConstDamageA(pcsSkill, pcsTarget) * fAirDefense);

	FLOAT	fDefensePoint	= 0.0f;
	m_pagpmFactors->GetValue(pcsAttackFactorResult, &fDefensePoint, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

	FLOAT	fTemp			= 0;
	if (fDefensePoint != 0) fTemp = (FLOAT)(400.0 / (fDefensePoint + 0.0));

	INT32 lConstDamage = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_B][lSkillLevel];
	
	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
	{
		lConstDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_B][lSkillLevel];
	}

	nDamage	+= (INT32) (lConstDamage * (1 - ((1 - fTemp) / 2.0)));

	if (nDamage <= 0)
		return FALSE;

	// 파티에 따라 보너스 데미지가 있는지 검사한다.
	//////////////////////////////////////////////////////////////////////////////////
	if (nDamage > 0 && !pcsSkill->m_bCloneObject)
		nDamage += (INT32) (nDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));

	// Divide Damage 처리해준다. 2007.06.27. steeple
	if(!bProcessInterval)
	{
		m_pagsmSkill->ProcessDivideDamage(pcsSkill, (AgpdCharacter*)pcsTarget, NULL, &nDamage, NULL);
	}

	// 높이에 따른 데미지 변화. 2006.06.23. steeple
	if(!bProcessInterval)		// 버프에서 들어온 건 하지 않는다.
	{
		nDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, nDamage);
	}

	/*
	// defense나 reflect를 하는지 검사한다.
	stAgsmCombatAttackResult	stAttackResult;
	stAttackResult.pAttackChar = (AgpdCharacter *) pcsSkill->m_pcsBase;
	stAttackResult.pTargetChar = (AgpdCharacter *) pcsTarget;
	stAttackResult.nDamage = nDamage;

	INT32	lReflectDamage = 0;

	EnumCallback(AGSMSKILL_CB_MELEE_ATTACK_CHECK, &stAttackResult, &lReflectDamage);

	nDamage = stAttackResult.nDamage;
	*/

	// Damage를 반영한다.
	AgsdSkill	*pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-nDamage);
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL] += (-nDamage);

	/*
	if (lReflectDamage > 0)
	{
		// 공격한 넘한테 lReflectDamage 를 되돌려준다.
		ProcessReflectDamage(pcsTarget, pcsSkill->m_pcsBase, lReflectDamage);
	}
	*/

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackType5
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE5) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			공식 : 일반데미지 공식 + DOT_DMG (Damage On Time)
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType5(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdFactor	*pcsAttackFactorResult	= m_pagsmSkill->GetOwnerFactorResult(pcsSkill);
	AgpdFactor	*pcsTargetFactorResult	= m_pagsmSkill->GetResultFactor(pcsTarget);

	if (!pcsAttackFactorResult || !pcsTargetFactorResult)
		return FALSE;

	INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate =  (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	INT32	nDamage	= 0;

	if (!bProcessInterval)
	{
		// physical damage를 구한다.
		BOOL	bIsEquipWeapon	= m_pagpmItem->IsEquipWeapon((AgpdCharacter *) pcsSkill->m_pcsBase);

		// 2004.12.29. steeple 마법사 때문에...
		nDamage = m_pagpmCombat->CalcPhysicalAttackForSkill((AgpdCharacter *) pcsSkill->m_pcsBase, (AgpdCharacter *) pcsTarget, bIsEquipWeapon);
		//nDamage = m_pagpmCombat->CalcPhysicalAttack((AgpdCharacter *) pcsSkill->m_pcsBase, (AgpdCharacter *) pcsTarget, bIsEquipWeapon);
	}

	//nDamage += pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG][lSkillLevel];

	if (nDamage <= 0)
		return FALSE;

	// 파티에 따라 보너스 데미지가 있는지 검사한다.
	//////////////////////////////////////////////////////////////////////////////////
	if (nDamage > 0 && !pcsSkill->m_bCloneObject)
		nDamage += (INT32) (nDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));

	// Divide Damage 처리해준다. 2007.06.27. steeple
	if(!bProcessInterval)
	{
		m_pagsmSkill->ProcessDivideDamage(pcsSkill, (AgpdCharacter*)pcsTarget, NULL, &nDamage, NULL);
	}

	// 높이에 따른 데미지 변화. 2006.06.23. steeple
	if(!bProcessInterval)		// 버프에서 들어온 건 하지 않는다.
	{
		nDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, nDamage);
	}
	/*
	// defense나 reflect를 하는지 검사한다.
	stAgsmCombatAttackResult	stAttackResult;
	stAttackResult.pAttackChar = (AgpdCharacter *) pcsSkill->m_pcsBase;
	stAttackResult.pTargetChar = (AgpdCharacter *) pcsTarget;
	stAttackResult.nDamage = nDamage;

	INT32	lReflectDamage = 0;

	EnumCallback(AGSMSKILL_CB_MELEE_ATTACK_CHECK, &stAttackResult, &lReflectDamage);

	nDamage = stAttackResult.nDamage;
	*/

	// Damage를 반영한다.
	AgsdSkill	*pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-nDamage);
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL] += (-nDamage);

	/*
	if (lReflectDamage > 0)
	{
		// 공격한 넘한테 lReflectDamage 를 되돌려준다.
		ProcessReflectDamage(pcsTarget, pcsSkill->m_pcsBase, lReflectDamage);
	}
	*/

	return TRUE;
}

//	2004.09.02. steeple
//		ProcessSkillEffectMeleeAttackType6
//	Functions
//		- ProcessSkillEffectMeleeAttackType1 과 공식 등은 같고, 
//		- 주위 다른 타겟에게 추가 데미지가 들어간다.
//
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType6(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	// 2004.12.29. steeple
	// 데미지 공식 변경으로 인해서 살짝 변경

	if(!pcsSkill || !pcsSkill->m_pcsBase || !pcsTarget)
		return FALSE;

	INT32 lSkillLevel =  m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// 모든 스킬에서 정령 데미지를 검사하여야 한다.
	AgpdFactorAttribute	stAttrDamage;
	ZeroMemory(&stAttrDamage, sizeof(AgpdFactorAttribute));

	INT32 lNormalSkillDamage = m_pagsmSkill->CalcNormalSkillDamage(pcsSkill, pcsTarget);
	INT32 lSpiritSkillDamage = m_pagsmSkill->CalcSpiritSkillDamage(pcsSkill, pcsTarget, &stAttrDamage);
	INT32 lHeroicSkillDamage = m_pagsmSkill->CalcNormalHeroicSkillDamage(pcsSkill, pcsTarget);

	if(lNormalSkillDamage <= 0)
		lNormalSkillDamage = 1;

	if(lSpiritSkillDamage <= 0)
		lSpiritSkillDamage = 1;

	if(lHeroicSkillDamage <= 0)
		lHeroicSkillDamage = 1;

	INT32 lDamage = lNormalSkillDamage;
	if(lDamage <= 0)
		lDamage	= 1;

	// 파티에 따라 보너스 데미지가 있는지 검사한다.
	if(lDamage > 0 && !pcsSkill->m_bCloneObject)
		lDamage += (INT32) (lDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));

	// 2004.09.02. steeple
	if(pcsSkill->m_csTargetBase.m_lID != pcsTarget->m_lID)
	{
		// 타겟이 주위 다른 적이다.
		// 이 때 추가 데미지를 준다.
		lDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_A][lSkillLevel];

		// 2007.10.25. steeple
		if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		{
			lDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_A][lSkillLevel];
		}
	}
	
	// Divide Damage 처리해준다. 2007.06.27. steeple
	if(!bProcessInterval)
	{
		m_pagsmSkill->ProcessDivideDamage(pcsSkill, (AgpdCharacter*)pcsTarget, &stAttrDamage, &lDamage, &lSpiritSkillDamage);
	}

	// 높이에 따른 데미지 변화. 2006.06.23. steeple
	if(!bProcessInterval)		// 버프에서 들어온 건 하지 않는다.
	{
		lDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lDamage);
		lSpiritSkillDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lSpiritSkillDamage);
		lHeroicSkillDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lHeroicSkillDamage);
	}

	// 총 물리 데미지를 세팅해준다.
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] = lDamage;
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] = lHeroicSkillDamage;
	lDamage += lSpiritSkillDamage;
	lDamage += lHeroicSkillDamage;

	// Damage를 반영한다.
	// 실제 캐릭터에 반영하고 History에도 반영한다.
	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor* pcsFactorResult = (AgpdFactor*)m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if(!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint* pcsFactorCharPoint = (AgpdFactorCharPoint*)m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if(!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint*) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if(!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-lDamage);
	// 정령 데미지 적용
	for(int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; i++)
		pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL + i] += (-stAttrDamage.lValue[i]);

	// Heroic Damage적용
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC] += (-stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC]);

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackCritical
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//					(일정 확률로 물리 데미지 증가 (발생확률:arg1, 증가 데미지(%): arg2))
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackCritical(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData)
		return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	INT32	lSkillLevel	= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if (lSkillLevel < 1 || lSkillLevel >= AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	// Attack Damage 만 세팅되어 있는 스킬이라면, Factor 올리는 작업을 하지 않는다.
	if(m_pagsmSkill->IsSetAttackDamageOnly(pcsSkill))
		return TRUE;

	pcsAttachData->m_ullBuffedSkillCombatEffect								|= AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeCriticalProbability	= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeDamageAdjustRate		= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_CRITICAL][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeCriticalProbability	+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeDamageAdjustRate		+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CRITICAL][lSkillLevel];
	}

	pcsAttachData->m_stBuffedSkillCombatEffectArg.lMeleeCriticalProbability	+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeCriticalProbability;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lMeleeDamageAdjustRate	+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeDamageAdjustRate;

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackDeath
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_MELEE_ATTACK_DEATH) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//					(크리티컬 공격(melee attack 106) 이 들어간 경우 arg1의 확률로 타켓을 즉사시킨다.)
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackDeath(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData)
		return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAttachData->m_ullBuffedSkillCombatEffect								|= AGPMSKILL_EFFECT_MELEE_ATTACK_DEATH;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathProbability			= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathAmount				= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DEATH][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathProbability		+= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathAmount			+= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DEATH][lSkillLevel];
	}

	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDeathProbability			+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathProbability;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDeathAmount				+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathAmount;

	return TRUE;
}

//		ProcessSkillEffectMagicAttackCritical
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			세팅된 값들을 factor point에 더한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMagicAttackCritical(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData)
		return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	INT32	lSkillLevel	= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if (lSkillLevel < 1 || lSkillLevel >= AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	pcsAttachData->m_ullBuffedSkillCombatEffect								|= AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicCriticalProbability	= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicDamageAdjustRate		= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_CRITICAL][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicCriticalProbability	+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicDamageAdjustRate		+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CRITICAL][lSkillLevel];
	}

	pcsAttachData->m_stBuffedSkillCombatEffectArg.lMagicCriticalProbability	+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicCriticalProbability;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lMagicDamageAdjustRate	+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicDamageAdjustRate;

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackDurability
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_MELEE_ATTACK_DURABILITY) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			세팅된 값들을 factor point에 더한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackDurability(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// 이 스킬의 확률을 구해서 성공일때만 적용시킨다.
	INT32 lRandom	 = m_csRandom->randInt(100);
	INT32 nSkillRate = m_pagsmSkill->GetSkillRate(pcsSkill);

	if(nSkillRate < lRandom) 
		return FALSE;

	INT32	  ArrEquipItem[AGPM_DURABILITY_EQUIP_PART_MAX] = { -1, };

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(lSkillLevel < 1) return FALSE;

	INT32 lCount		 = 0;
	INT32 lMax			 = m_pagpmItem->GetEquipItems((AgpdCharacter *)pcsTarget, ArrEquipItem, (INT32)AGPM_DURABILITY_EQUIP_PART_MAX);
	INT32 lDurabilityNum = (INT32)((AgpdSkillTemplate*)(pcsSkill->m_pcsTemplate))->m_fUsedConstFactor2[AGPMSKILL_CONST_ARMOR_DURABILITY_NUM][lSkillLevel];
	FLOAT lDurabilityPer = ((AgpdSkillTemplate*)(pcsSkill->m_pcsTemplate))->m_fUsedConstFactor2[AGPMSKILL_CONST_ARMOR_DURABILITY_PERCENT][lSkillLevel];

	if(lMax < lDurabilityNum) lDurabilityNum = lMax;

	if(lMax < 1) return FALSE;

	for(INT32 lCount = 0; lCount < lDurabilityNum; ++lCount)
	{
		AgpdItem* pEquipItem = NULL;
		INT32 lRandPart = m_csRandom->randInt(lMax);

		if(ArrEquipItem[lRandPart] > AGSMSKILL_EFFECT_NONCHECK_DURABILITY && ArrEquipItem[lRandPart] < AGPM_DURABILITY_EQUIP_PART_MAX)
		{
			pEquipItem = m_pagpmItem->GetEquipSlotItem((AgpdCharacter*)pcsTarget, (AgpmItemPart)AgpmItem::s_lBeDurabilityEquipItem[ArrEquipItem[lRandPart]]);
		}

		if(pEquipItem)
		{
			INT32 lMaxDurability	 = m_pagpmItem->GetItemDurabilityMax(pEquipItem);
			INT32 lCurrentDurability = m_pagpmItem->GetItemDurabilityCurrent(pEquipItem);
			INT32 lChangeDurability  = (INT32)((lMaxDurability * (FLOAT)(1 + lDurabilityPer / 100.0)) - lMaxDurability);

			if(lCurrentDurability + lChangeDurability < AGSMSKILL_EFFECT_LIMIT_DURABILITY) 
			{
				lChangeDurability = AGSMSKILL_EFFECT_LIMIT_DURABILITY - lCurrentDurability;
			}
										
			if(!m_pagsmItem->UpdateItemDurability(pEquipItem, lChangeDurability))
			{
				//
			}

			ArrEquipItem[lRandPart] = AGSMSKILL_EFFECT_NONCHECK_DURABILITY;			
		}
	}

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackHeroic
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_MELEE_ATTACK_HEROIC) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			세팅된 값들을 factor point에 더한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackHeroic(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget || NULL == pcsSkill->m_pcsBase)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate *pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(NULL == pcsSkillTemplate)
		return FALSE;

	AgpdFactorAttribute stAttrDamage;
	ZeroMemory(&stAttrDamage, sizeof(AgpdFactorAttribute));

	INT32 lHeroicDamage		= m_pagsmSkill->CalcNormalHeroicSkillDamage(pcsSkill, pcsTarget);

	INT32 lDamage = lHeroicDamage;
	if(lDamage <= 0)
		lDamage = 1;

	// 파티에 따라 보너스 데미지가 있는지 검사한다.
	if(lDamage > 0 && !pcsSkill->m_bCloneObject)
		lDamage += (INT32) (lDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));

	// 총 물리 데미지를 세팅해준다
	// 어차피 HeroicDamage는 Heroic이 전부 다이므로 물리 = 히로익이다.
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] = lDamage;
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] = lDamage;

	// Damage를 반영
	AgsdSkill *pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(NULL == pcsAgsdSkill)
		return FALSE;
	
	if(!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor *pcsFactorResult = (AgpdFactor*)m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if(NULL == pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint*)m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if(!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint*)m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if(!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-lDamage);

	// Heroic Facotr Setting
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC] += (-lDamage);

	return TRUE;
}

//		ProcessSkillEffectUpdateFactorHOT
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_UPDATE_FACTOR_HOT) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			세팅된 값들을 union factor point에 더한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorHOT(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill			*pcsAgsdSkill		= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	// 2006.11.27. steeple
	// 한번만 팩터 적용되는 스킬인지 체크한다.
	if(bProcessInterval && m_pagsmSkill->CheckOnceAffected(pcsSkillTemplate))
		return TRUE;

	INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor			*pcsFactorResult	= (AgpdFactor *)			m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint	*pcsFactorPoint		= (AgpdFactorCharPoint *)	m_pagpmFactors->SetFactor(pcsFactorResult, NULL, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorPoint)
		return FALSE;

	pcsFactorPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_HOT][lSkillLevel];
	
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		pcsFactorPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_HOT][lSkillLevel];

	return TRUE;
}

//		ProcessSkillEffectUpdateFactorMagnify
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_UPDATE_FACTOR_MAGNIFY) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			세팅된 값들을 union factor point에 더한다. ((현재 FactorValue)*(1+(FactorValue/100)))
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorMagnify(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;
	
	AgsdSkill			*pcsAgsdSkill		= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	// 2006.11.27. steeple
	// 한번만 팩터 적용되는 스킬인지 체크한다.
	if(bProcessInterval && m_pagsmSkill->CheckOnceAffected(pcsSkillTemplate))
		return TRUE;

	AgpdFactorDIRT		*pcsFactorDIRT		= (AgpdFactorDIRT *)	m_pagpmFactors->GetFactor(&pcsSkill->m_csFactor, AGPD_FACTORS_TYPE_DIRT);
	if (!pcsFactorDIRT)
		return FALSE;

	INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	for (int i = AGPMSKILL_CONST_MAGNIFY_START + 1; i < AGPMSKILL_CONST_MAGNIFY_END; ++i)
	{
		// '0' 인지 아닌지를 검사해서 '0'이 아니면 target의 factor point에 더한다.
		if (pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0)
		{
			if (!m_pagsmSkill->SetUpdateFactorPoint(pcsAgsdSkill))
				continue;

			AgpdFactorCharStatus	*pcsFactorTemp  = (AgpdFactorCharStatus *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorPoint, NULL, g_alFactorTable[i][0]);
			if (!pcsFactorTemp)
				continue;

			if (g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
			{
				FLOAT	fPrevValue	= 0.0f;

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
				if (fPrevValue > 0.0f)
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, (FLOAT) (fPrevValue * (1 + fPrevValue / 100.0)), (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
			}
			else
			{
				FLOAT	fPrevValue	= 0.0f;

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
				if (fPrevValue > 0.0f)
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, (FLOAT) (fPrevValue * (1 + fPrevValue / 100.0)), (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
			}
		}
	}

	return TRUE;
}

//		ProcessSkillEffectUpdateFactorAdd
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_UPDATE_FACTOR) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			세팅된 값들을 factor point에 더한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorAdd(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill			*pcsAgsdSkill		= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	// 2006.11.27. steeple
	// 한번만 팩터 적용되는 스킬인지 체크한다.
	if(bProcessInterval && m_pagsmSkill->CheckOnceAffected(pcsSkillTemplate))
		return TRUE;

	AgpdFactorDIRT		*pcsFactorDIRT		= (AgpdFactorDIRT *)	m_pagpmFactors->GetFactor(&pcsSkill->m_csFactor, AGPD_FACTORS_TYPE_DIRT);
	if (!pcsFactorDIRT)
		return FALSE;

	INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL	bIsUpdateMovement	= FALSE;
	BOOL	bCheckConnection	= FALSE;

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		bCheckConnection = TRUE;

	for (int i = AGPMSKILL_CONST_POINT_START + 1; i < AGPMSKILL_CONST_POINT_END; ++i)
	{
		//////////////////////////////////////////////////////////////////////////
		//
		if(i == AGPMSKILL_CONST_POINT_CHA) continue; // 카리스마는 스탯이 아님. arycoat - 2008.7

		//////////////////////////////////////////////////////////////////////////
		// '0' 인지 아닌지를 검사해서 '0'이 아니면 target의 factor point에 더한다.
		if (pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0 || pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel] != 0)
		{
			if (!m_pagsmSkill->SetUpdateFactorPoint(pcsAgsdSkill))
				continue;

			AgpdFactorCharStatus	*pcsFactorTemp  = (AgpdFactorCharStatus *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorPoint, NULL, g_alFactorTable[i][0]);
			if (!pcsFactorTemp)
				continue;

			if (g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + fConstValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
			}
			else
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);

				if ((eAgpdFactorsType) g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_CHAR_STATUS &&
					(g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT || g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST))
				{
					// pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] 가 0 보다 작을 때 해야 한다.
					AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
					if(fConstValue < 0.0f && pcsAgsdCharacter && pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > 0)
					{
						// 아래와 같은 상황이라면 슬로우 주지 않는다.
						INT32 lRandom = m_csRandom->randInt(100);
						if(pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > lRandom)
							continue;
					}

					bIsUpdateMovement	= TRUE;
				}

				// AttackRange 는 Factor 에서 cm 단위로 저장된다. Skill_Const 에서는 m 단위로 온다. 2006.03.28. steeple
				if ((eAgpdFactorsType) g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_ATTACK && g_alFactorTable[i][1] == AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE)
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + (fConstValue * 100.0f), (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
				else
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + fConstValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
			}
		}
	}

	for (int i = AGPMSKILL_CONST_PERCENT_START + 1; i < AGPMSKILL_CONST_PERCENT_END; ++i)
	{
		//////////////////////////////////////////////////////////////////////////
		//
		if(i == AGPMSKILL_CONST_PERCENT_CHA) continue; // 카리스마는 스탯이 아님. arycoat - 2008.7

		// '0' 인지 아닌지를 검사해서 '0'이 아니면 target의 factor percent에 더한다.
		if (pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0)
		{
			if (!m_pagsmSkill->SetUpdateFactorPercent(pcsAgsdSkill))
				continue;

			AgpdFactorCharStatus	*pcsFactorTemp  = (AgpdFactorCharStatus *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorPercent, NULL, g_alFactorTable[i][0]);
			if (!pcsFactorTemp)
				continue;

			if (g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fPrevValue + fConstValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
			}
			else
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);

				if ((eAgpdFactorsType) g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_CHAR_STATUS &&
					(g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT || g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST))
				{
					// pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] 가 0 보다 작을 때 해야 한다.
					AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
					if(fConstValue < 0.0f && pcsAgsdCharacter && pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > 0)
					{
						// 아래와 같은 상황이라면 슬로우 주지 않는다.
						INT32 lRandom = m_csRandom->randInt(100);
						if(pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > lRandom)
							continue;
					}

					bIsUpdateMovement	= TRUE;
				}

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fPrevValue + fConstValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
			}
		}
	}

	if (bIsUpdateMovement)
	{
		// 다른 모듈에서 Slow와 관련된 적용해야할 값이 있는지 가져온다.

		UINT64	ulSpecialStatus		= (UINT64)	AGPDCHAR_SPECIAL_STATUS_SLOW;
		UINT32	ulAdjustDuration	= 0;
		INT32	lAdjustProbability	= 0;

		PVOID	pvBuffer[4];
		pvBuffer[0]	= (PVOID)	&ulSpecialStatus;
		pvBuffer[1]	= (PVOID)	pcsSkill;
		pvBuffer[2]	= (PVOID)	&ulAdjustDuration;
		pvBuffer[3]	= (PVOID)	&lAdjustProbability;

		m_pagsmSkill->EnumCallback(AGSMSKILL_CB_CHECK_SPECIAL_STATUS_FACTOR, pcsTarget, pvBuffer);

		if (ulAdjustDuration > 0)
		{
			pcsSkill->m_ulEndTime	-= ulAdjustDuration;
		}
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorItem(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	BOOL retval = m_pagsmSkill->ApplyUpdateFactorItem(pcsSkill, pcsTarget, nIndex, TRUE, TRUE, FALSE);
	
	return retval;
}

// 2005.07.07. steeple
// 시간에 따라 올려줄 Factor 가 바뀐다.
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorTime(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate	= (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdFactorDIRT* pcsFactorDIRT = (AgpdFactorDIRT*)m_pagpmFactors->GetFactor(&pcsSkill->m_csFactor, AGPD_FACTORS_TYPE_DIRT);
	if(!pcsFactorDIRT)
		return FALSE;

	// 현재 시간을 구한다.
	UINT32 ulHour = m_pagpmEventNature->GetHour(m_pagpmEventNature->GetGameTime());
	BOOL bIsNight = FALSE;
	if(ulHour < 6 || ulHour >= 18)
		bIsNight = TRUE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bIsUpdateMovement = FALSE;
	BOOL bCheckConnection = FALSE;
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		bCheckConnection = TRUE;

	for(INT32 i = AGPMSKILL_CONST_POINT_START + 1; i < AGPMSKILL_CONST_POINT_END; i++)
	{
		// '0' 인지 아닌지를 검사해서 '0'이 아니면 target의 factor point에 더한다.
		if(pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0 || pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel] != 0)
		{
			if(pcsSkillTemplate->m_stConditionArg[nIndex].lArg1 == 1)	// 낮에는 방어력. 밤에는 공격력 증가.
			{
				if(!bIsNight && g_alFactorTable[i][0] != AGPD_FACTORS_TYPE_DEFENSE)
					continue;		// 낮인데 Factor Table 이 AGPD_FACTORS_TYPE_DEFENSE 가 아니면 Skip

				if(bIsNight && g_alFactorTable[i][0] != AGPD_FACTORS_TYPE_DAMAGE)
					continue;		// 밤인데 Factor Table 이 AGPD_FACTORS_TYPE_DAMAGE 가 아니면 Skip
			}

			if (!m_pagsmSkill->SetUpdateFactorPoint(pcsAgsdSkill))
				continue;

			AgpdFactorCharStatus* pcsFactorTemp = (AgpdFactorCharStatus*)m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorPoint, NULL, g_alFactorTable[i][0]);
			if(!pcsFactorTemp)
				continue;

			if(g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
			{
				FLOAT fPrevValue = 0.0f;
				FLOAT fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + fConstValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
			}
			else
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue	= pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);

				if ((eAgpdFactorsType) g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_CHAR_STATUS &&
					(g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT || g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST))
				{
					// pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] 가 0 보다 작을 때 해야 한다.
					AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
					if(fConstValue < 0.0f && pcsAgsdCharacter && pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > 0)
					{
						// 아래와 같은 상황이라면 슬로우 주지 않는다.
						INT32 lRandom = m_csRandom->randInt(100);
						if(pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > lRandom)
							continue;
					}

					bIsUpdateMovement	= TRUE;
				}

				// AttackRange 는 Factor 에서 cm 단위로 저장된다. Skill_Const 에서는 m 단위로 온다. 2006.03.28. steeple
				if (g_alFactorTable[i][1] == AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE)
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + (fConstValue * 100.0f), (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
				else
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + fConstValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
			}
		}
	}

	for(INT32 i = AGPMSKILL_CONST_PERCENT_START + 1; i < AGPMSKILL_CONST_PERCENT_END; i++)
	{
		// '0' 인지 아닌지를 검사해서 '0'이 아니면 target의 factor percent에 더한다.
		if(pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0)
		{
			if(pcsSkillTemplate->m_stConditionArg[nIndex].lArg1 == 1)	// 낮에는 방어력. 밤에는 공격력 증가.
			{
				if(!bIsNight && g_alFactorTable[i][0] != AGPD_FACTORS_TYPE_DEFENSE)
					continue;		// 낮인데 Factor Table 이 AGPD_FACTORS_TYPE_DEFENSE 가 아니면 Skip

				if(bIsNight && g_alFactorTable[i][0] != AGPD_FACTORS_TYPE_DAMAGE)
					continue;		// 밤인데 Factor Table 이 AGPD_FACTORS_TYPE_DAMAGE 가 아니면 Skip
			}

			if (!m_pagsmSkill->SetUpdateFactorPercent(pcsAgsdSkill))
				continue;

			AgpdFactorCharStatus* pcsFactorTemp  = (AgpdFactorCharStatus*)m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorPercent, NULL, g_alFactorTable[i][0]);
			if(!pcsFactorTemp)
				continue;

			if(g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fPrevValue + fConstValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
			}
			else
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);

				if ((eAgpdFactorsType) g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_CHAR_STATUS &&
					(g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT || g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST))
				{
					// pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] 가 0 보다 작을 때 해야 한다.
					AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
					if(fConstValue < 0.0f && pcsAgsdCharacter && pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > 0)
					{
						// 아래와 같은 상황이라면 슬로우 주지 않는다.
						INT32 lRandom = m_csRandom->randInt(100);
						if(pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > lRandom)
							continue;
					}

					bIsUpdateMovement	= TRUE;
				}

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fPrevValue + fConstValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1]);
			}
		}
	}

	if (bIsUpdateMovement)
	{
		// 다른 모듈에서 Slow와 관련된 적용해야할 값이 있는지 가져온다.

		UINT64	ulSpecialStatus		= (UINT64)	AGPDCHAR_SPECIAL_STATUS_SLOW;
		UINT32	ulAdjustDuration	= 0;
		INT32	lAdjustProbability	= 0;

		PVOID	pvBuffer[4];
		pvBuffer[0]	= (PVOID)	&ulSpecialStatus;
		pvBuffer[1]	= (PVOID)	pcsSkill;
		pvBuffer[2]	= (PVOID)	&ulAdjustDuration;
		pvBuffer[3]	= (PVOID)	&lAdjustProbability;

		m_pagsmSkill->EnumCallback(AGSMSKILL_CB_CHECK_SPECIAL_STATUS_FACTOR, pcsTarget, pvBuffer);

		if (ulAdjustDuration > 0)
		{
			pcsSkill->m_ulEndTime	-= ulAdjustDuration;
		}
	}

	return TRUE;
}

// 2008.06.11. iluvs.
// Detail Reflect_Melee_Attack
BOOL AgsmSkillEffect::ProcessSkillEffect2ReflectMeleeAtk(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(lSkillLevel < 1)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] & AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL)
		{
			pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL;

			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_IGNORE_PHYSICAL_DEFENCE_PERCENT][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] & AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE)
		{
			pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE;

			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_IGNORE_ATTRIBUTE_DEFENCE_PERCENT][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence;
		}

	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] & AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL)
		{
			pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL;

			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_IGNORE_PHYSICAL_DEFENCE_PERCENT][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] & AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE)
		{	
			pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE;

			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_IGNORE_ATTRIBUTE_DEFENCE_PERCENT][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence;
		}
		
	}

	return bResult;
}

// 2007.07.03. steeple
// Dispel 이 늘어났다.
BOOL AgsmSkillEffect::ProcessSkillEffect2Dispel(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_STUN)
			bResult |= ProcessSkillEffect2DispelStun(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_SLOW)
			bResult |= ProcessSkillEffect2DispelSlow(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_ALL_BUFF)
			bResult |= ProcessSkillEffect2DispelAllBuff(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_TRANSPARENT)
			bResult |= ProcessSkillEffect2DispelTransparent(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_SPECIAL_STATUS)
			bResult |= ProcessSkillEffect2DispelSpecialStatus(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_STUN)
			bResult |= ProcessSkillEffect2DispelStun(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_SLOW)
			bResult |= ProcessSkillEffect2DispelSlow(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_ALL_BUFF)
			bResult |= ProcessSkillEffect2DispelAllBuff(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_TRANSPARENT)
			bResult |= ProcessSkillEffect2DispelTransparent(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_SPECIAL_STATUS)
			bResult |= ProcessSkillEffect2DispelSpecialStatus(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2SkillLevelUPType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// Level Up 수치를 세팅해준다.
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lSkillLevelUpPoint += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_LEVELUP][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lSkillLevelUpPoint = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_LEVELUP][lSkillLevel];

	// 2007.02.06. steeple
	// Modified Skill Level 을 다시 계산해준다.
	m_pagpmSkill->UpdateModifiedSkillLevel(pcsSkill->m_pcsBase);

	return TRUE;
}

// 2007.07.07. steeple
// 캐릭터 레벨을 올려준다.
BOOL AgsmSkillEffect::ProcessSkillEffect2SkillLevelUPType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lCharLevel = m_pagpmCharacter->GetLevel((AgpdCharacter*)pcsTarget);
	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_LEVELUP][lSkillLevel];
	//INT32 lValue = m_pagpmCharacter->GetLevel((AgpdCharacter*)pcsTarget);

	// 2008.04.08. steeple
	// 타겟이 레벨 제한 지역에 있고, 현재 레벨과 올라가는 레벨의 합이 레벨 제한을 넘어간다면 그냥 나간다.
	// 타겟에는 문제가 좀 있어서, 시전자로 변경.
	INT32 lLimitedLevel = m_pagpmCharacter->GetCurrentRegionLevelLimit((AgpdCharacter*)pcsSkill->m_pcsBase);
	if(lLimitedLevel != 0 && (lCharLevel + lValue) > lLimitedLevel)
		return FALSE;

	// Level Up 수치를 세팅해준다.
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lCharLevelUpPoint += lValue;
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lCharLevelUpPoint = lValue;

	if(m_pagpmCharacter->IsPC((AgpdCharacter*)pcsTarget))
	{
		// PC 라면 UpdateCharLevel 불러주고
		//
		// 나중에 작업.
	}
	else
	{
		// Mob 이라면 그냥 팩터에 세팅한다.
		if(m_pagsmSkill->SetUpdateFactorPoint(pcsAgsdSkill))
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	}

	return TRUE;
}

// 2007.11.14. steeple
// 특정 스킬의 레벨을 1 올려준다. 물론 배운 스킬들만.
BOOL AgsmSkillEffect::ProcessSkillEffect2SkillLevelUPType3(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsSkill->m_pcsTemplate || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);

	int j = 0;
	pcsAgsdSkill->m_alLevelUpSkillTID.MemSetAll();

	for(int i = 0; i < AGPMSKILL_MAX_SKILL_LEVELUP_TID; ++i)
	{
		if(pcsSkillTemplate->m_allLevelUpSkillTID[lSkillLevel][i] == 0)
			break;

		AgpdSkill* pcsOwnSkill = m_pagpmSkill->GetSkillByTID(static_cast<AgpdCharacter*>(pcsSkill->m_pcsBase),
															pcsSkillTemplate->m_allLevelUpSkillTID[lSkillLevel][i]);
		if(pcsOwnSkill)
		{
			++pcsOwnSkill->m_lModifiedLevel;
			pcsAgsdSkill->m_alLevelUpSkillTID[j++] = pcsOwnSkill->m_pcsTemplate->m_lID;

			// 패킷을 보낸다.
			m_pagsmSkill->SendSpecificSkillLevel(pcsOwnSkill);

			// Passive Skill 이면 다시 캐스팅한다.
			if(m_pagpmSkill->IsPassiveSkill(pcsOwnSkill))
			{
				m_pagsmSkill->EndPassiveSkill(pcsOwnSkill);
				m_pagsmSkill->CastPassiveSkill(pcsOwnSkill);
			}
		}
	}

	return TRUE;
}

// 2005.07.07. steeple
// AT Field Attack 처리
BOOL AgsmSkillEffect::ProcessSkillEffect2ATFieldAttack(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	if(bProcessInterval)
	{
		// 버프 상태에서 불렸다.
		// 이 때에는 주위의 캐릭터를 얻어와서 공격할 수 있는 애들 공격을 해준다.
		//
		if(!pcsTarget)
			return FALSE;

		ApdEvent* pcsEvent = NULL;
		pcsEvent = m_papmEventManager->GetEvent(pcsTarget, APDEVENT_FUNCTION_SKILL);
		if(!pcsEvent)
			pcsEvent = m_papmEventManager->AddEvent(m_papmEventManager->GetEventData(pcsTarget), APDEVENT_FUNCTION_SKILL, pcsTarget, FALSE, 0, m_pagsmSkill->GetClockCount());

		if(!pcsEvent)
			return FALSE;

		ApSafeArray<INT_PTR, AGSMSKILL_MAX_TARGET_NUM> lCIDBuffer;
		lCIDBuffer.MemSetAll();
		ApSafeArray<INT32, AGSMSKILL_MAX_TARGET_NUM> lCIDBuffer2;
		lCIDBuffer2.MemSetAll();

		// 먼저 range conditon을 검사해 타겟을 가져온다.
		INT32 lNumTarget = m_pagsmSkill->RangeCheckSphereForATField(pcsSkill, pcsTarget, &lCIDBuffer[0], AGSMSKILL_MAX_TARGET_NUM - 1, &lCIDBuffer2[0], AGSMSKILL_MAX_TARGET_NUM - 1, pcsEvent);
		AgpdCharacter* pcsTargetCharacter = NULL;
		ApBase* pcsTargetBase = NULL;

		// 루프를 돌면서 공격해준다.
		for(INT32 i = 0; i < lNumTarget; i++)
		{
			// 공격이니깐 자기는 넘어가고
			if(pcsTarget->m_lID == lCIDBuffer2[i])
				continue;

			pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(lCIDBuffer2[i]);
			if(!pcsTargetCharacter)
				continue;

			if(pcsTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			{
				pcsTargetCharacter->m_Mutex.Release();
				continue;
			}

			// 이조건이면 이 녀석은 펫이다 
			if(m_pagpmCharacter->IsPet(pcsTargetCharacter))
			{
				pcsTargetCharacter->m_Mutex.Release();
				continue;
			}

			m_pagsmSkill->ProcessATFieldAttack(pcsSkill, (AgpdCharacter*)pcsTarget, pcsTargetCharacter);

			pcsTargetCharacter->m_Mutex.Release();

			// 데미지 및 팩터들을 초기화 한다.
			m_pagsmSkill->DestroyUpdateFactorPoint(pcsAgsdSkill);
			m_pagsmSkill->DestroyUpdateFactorPercent(pcsAgsdSkill);
			m_pagsmSkill->DestroyUpdateFactorResult(pcsAgsdSkill);
			m_pagsmSkill->DestroyUpdateFactorItemPoint(pcsAgsdSkill);
			m_pagsmSkill->DestroyUpdateFactorItemPercent(pcsAgsdSkill);
		}

		m_papmEventManager->RemoveEvent(pcsEvent);
	}
	else
	{
		// 그다지 할 건 없다. -0-;;
	}

	return TRUE;
}

// 2005.10.06. steeple
// AT Field Attack2 처리
BOOL AgsmSkillEffect::ProcessSkillEffect2ATFieldAttack2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	if(bProcessInterval)
	{
		// 버프 상태에서 불렸다.
		// 이 때에는 주위의 캐릭터를 얻어와서 공격할 수 있는 애들 공격을 해준다.
		//
		if(!pcsTarget)
			return FALSE;

		ApdEvent* pcsEvent = NULL;
		pcsEvent = m_papmEventManager->GetEvent(pcsTarget, APDEVENT_FUNCTION_SKILL);
		if(!pcsEvent)
			pcsEvent = m_papmEventManager->AddEvent(m_papmEventManager->GetEventData(pcsTarget), APDEVENT_FUNCTION_SKILL, pcsTarget, FALSE, 0, m_pagsmSkill->GetClockCount());

		if(!pcsEvent)
			return FALSE;

		ApSafeArray<INT_PTR, AGSMSKILL_MAX_TARGET_NUM> lCIDBuffer;
		lCIDBuffer.MemSetAll();
		ApSafeArray<INT32, AGSMSKILL_MAX_TARGET_NUM> lCIDBuffer2;
		lCIDBuffer2.MemSetAll();

		// 먼저 range conditon을 검사해 타겟을 가져온다.
		INT32 lNumTarget = m_pagsmSkill->RangeCheckSphereForATField(pcsSkill, pcsTarget, &lCIDBuffer[0], AGSMSKILL_MAX_TARGET_NUM - 1, &lCIDBuffer2[0], AGSMSKILL_MAX_TARGET_NUM - 1, pcsEvent);
		AgpdCharacter* pcsTargetCharacter = NULL;
		ApBase* pcsTargetBase = NULL;

		// 루프를 돌면서 공격해준다.
		for(INT32 i = 0; i < lNumTarget; i++)
		{
			pcsTargetBase = (ApBase*)lCIDBuffer[i];
			if(!pcsTargetBase)
				break;

			// 공격이니깐 자기는 넘어가고
			if(pcsTarget->m_lID == pcsTargetBase->m_lID)
				continue;

			pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(pcsTargetBase->m_lID);
			if(!pcsTargetCharacter)
				continue;

			// 공격할 캐릭터에게 바로 공격 호출
			m_pagpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, (AgpdCharacter*)pcsTarget, (ApBase*)pcsTargetCharacter, NULL);

			pcsTargetCharacter->m_Mutex.Release();
		}

		m_papmEventManager->RemoveEvent(pcsEvent);
	}
	else
	{
		// 그다지 할 건 없다. -0-;;
	}

	return TRUE;
}

// 2005.07.07. steeple
// AT Field Aura 처리
BOOL AgsmSkillEffect::ProcessSkillEffect2ATFieldAura(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// 이 스킬은 Additional SkillTID 가 없으면 쓰지 못한다.
	if(pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel) == 0)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	if(bProcessInterval)
	{
		// 버프 상태에서 불렸다.
		//
		if(!pcsTarget)
			return FALSE;

		// 그냥 타겟의 현재 좌표로 주위에다가 거시기 한다.
		AuPOS stPos = ((AgpdCharacter*)pcsTarget)->m_stPos;
		m_pagsmSkill->CastSkill(pcsTarget,
					pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel),
					lSkillLevel,
					&stPos);


		//ApdEvent* pcsEvent = NULL;
		//pcsEvent = m_papmEventManager->GetEvent(pcsTarget, APDEVENT_FUNCTION_SKILL);
		//if(!pcsEvent)
		//	pcsEvent = m_papmEventManager->AddEvent(m_papmEventManager->GetEventData(pcsTarget), APDEVENT_FUNCTION_SKILL, pcsTarget, FALSE, 0, GetClockCount());

		//if(!pcsEvent)
		//	return FALSE;

		//ApSafeArray<INT32, 33> lCIDBuffer;
		//lCIDBuffer.MemSetAll();

		//// 먼저 range conditon을 검사해 타겟을 가져온다.
		//INT32 lNumTarget = RangeCheckSphereForATField(pcsSkill, pcsTarget, &lCIDBuffer[0], 32, pcsEvent);

		//// 얘는 ATFieldAttack 과는 다르게 여기서 TargetFiltering 한번 해줘야 한다.
		//// pcsSkill->m_pcsBase 를 pcsTarget 으로 잠시 바꿔준다.
		//ApBase* pcsOriginalSkillBase = pcsSkill->m_pcsBase;
		//pcsSkill->m_pcsBase = pcsTarget;

		//BOOL bIsTargetSelfCharacter	= FALSE;

		//ApSafeArray<AgpdCharacter*, 33>	pacsCharacter;
		//pacsCharacter.MemSetAll();

		//if(lNumTarget > 32)
		//	lNumTarget	= 32;

		//if(lNumTarget > 0)
		//{
		//	for(INT32 i = 0; i < lNumTarget; i++)
		//	{
		//		if(lCIDBuffer[i] == (INT32)pcsSkill->m_pcsBase)
		//		{
		//			bIsTargetSelfCharacter	= TRUE;
		//			break;
		//		}
		//	}

		//	if(!bIsTargetSelfCharacter)
		//	{
		//		lCIDBuffer[lNumTarget++] = (INT32)pcsSkill->m_pcsBase;
		//	}

		//	if(!(lNumTarget == 1 && bIsTargetSelfCharacter))
		//	{
		//		for(INT32 j = 0; j < lNumTarget; j++)
		//			pacsCharacter[j] = (AgpdCharacter*)lCIDBuffer[j];
		//	}
		//	else
		//	{
		//		pacsCharacter[0] = (AgpdCharacter*)pcsSkill->m_pcsBase;
		//	}
		//}

		//ApSafeArray<AgpdCharacter*, 33> pacsRemoveTarget;
		//pacsRemoveTarget.MemSetAll();

		//lNumTarget = TargetFiltering(pcsSkill, &pacsCharacter[0], lNumTarget, &pacsRemoveTarget[0]);

		//lCIDBuffer.MemSetAll();
		//for(INT32 i = 0; i < lNumTarget; ++i)
		//	lCIDBuffer[i] = pacsCharacter[i]->m_lID;

		//pcsSkill->m_pcsBase = pcsOriginalSkillBase;

		//// 타겟이 없으면 걍 나간다.
		//if(lNumTarget < 0)
		//	return TRUE;

		//AgpdCharacter* pcsTargetCharacter = NULL;

		//// 루프를 돌면서 버프 스킬을 걸어준다.
		//// 이미 버프가 걸려있으면 중간에 실패하기 때문에 걍 걸면 된다.
		//for(INT32 i = 0; i < lNumTarget; i++)
		//{
		//	pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(lCIDBuffer[i]);
		//	if(!pcsTargetCharacter)
		//		continue;

		//	if(pcsTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		//	{
		//		pcsTargetCharacter->m_Mutex.Release();
		//		continue;
		//	}

		//	m_pagsmSkill->CastSkill(static_cast<ApBase*>(pcsTarget),
		//		pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_ADDITIONAL_SKILLTID][lSkillLevel],
		//		lSkillLevel,
		//		static_cast<ApBase*>(pcsTargetCharacter));

		//	pcsTargetCharacter->m_Mutex.Release();
		//}

		//m_papmEventManager->RemoveEvent(pcsEvent);
	}
	else
	{
		// 그다지 할 건 없다. -0-;;
	}

	return TRUE;
}

// 2005.07.15. steeple
// 소환 타입1. 뭔가를 소환한다. 움직이는 애로.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// 기존 개수를 체크해서 맥스 소환 수치에 다달았다면 
	// 먼저 번 거를 없애주고 새로 소환하던가 소환이 안되게 처리하자.

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lSummonsTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lSummonsCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lSummonsCount == 0)
		lSummonsCount = 1;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
	
	INT32 lCreateNum = m_pagsmSummons->ProcessSummons(static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID, ulDuration, lSummonsCount, lSkillLevel,
													AGPMSUMMONS_TYPE_ELEMENTAL, AGPMSUMMONS_PROPENSITY_DEFENSE, pcsSkill->m_bForceAttack);
	if(lCreateNum < 1)
	{
		// 에러 메시지를 보내줘야 하는데
		return FALSE;
	}

	// 소환될 때 CBEnterGameWorld 를 타고 Passive 스킬들은 이 때 Cast 된다.
	// 그럼 딱히 할 거 없다???? -_-

	return TRUE;
}

// 2005.10.05. steeple
// Fixed 방식의 추가로 인해서 기존의 소환 타입1 과 차이가 없던 점을 완전 수정.
//
// 2005.07.15. steeple
// 소환 타입2. 땅에다가 심는다. ex) 라이트닝 트랩, 홀리 그라운드
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagpmSummons || !m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lFixedTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lFixedCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lFixedCount == 0)
		lFixedCount = 1;

	// 고정형은 각 타입별로 Max 치가 스킬에 저장되어 있다.
	INT32 lMaxEachCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_LIMIT_QUANTITY][lSkillLevel];

	// Owner 의 Fixed List 에서 해당 TID 의 개수 체크를 한다.
	INT32 lCurrentEachCount = m_pagpmSummons->GetEachFixedCount(static_cast<AgpdCharacter*>(pcsTarget), lFixedTID);
	if(lCurrentEachCount >= lMaxEachCount)
		return FALSE;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);

	INT32 lCreateNum = m_pagsmSummons->ProcessFixed(static_cast<AgpdCharacter*>(pcsTarget), lFixedTID,
											ulDuration, lFixedCount, lSkillLevel, pcsSkill->m_bForceAttack);
	if(lCreateNum < 1)
	{
		// 에러 메시지를 보내줘야 하는데
		return FALSE;
	}

	return TRUE;
}

// 2005.07.15. steeple
// 소환 타입3. 타겟을 나의 편으로 만든다. 이때 타겟은 only 몬스터.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType3(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	if(!m_pagpmCharacter->IsMonster(static_cast<AgpdCharacter*>(pcsTarget)))
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);

	// 2005.10.05. steeple
	// Taming 은 각 스킬별로 Max 개수가 정해진다. 
	// 현재는 스킬이 하나여서 이렇게 처리해도 된다지만......
	// 스킬이 여러개가 생기면 각 Taming 한 애들의 SkillTID 를 주고, 그거에 따른 개수 비교를 해야할 듯 싶다.
	//
    
	INT32 lMaxTameCountSkill = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_LIMIT_QUANTITY][lSkillLevel];
	if(m_pagpmSummons->GetTameCount(static_cast<AgpdCharacter*>(pcsTarget)) >= lMaxTameCountSkill)
		return FALSE;

	INT32 lTamingNum = m_pagsmSummons->ProcessTaming(static_cast<AgpdCharacter*>(pcsSkill->m_pcsBase),
													static_cast<AgpdCharacter*>(pcsTarget),
													ulDuration,
													lSkillLevel,
													pcsSkill->m_bForceAttack);
	if(lTamingNum < 1)
	{
		// 에러메시지를 대략 보내주고
		AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
		if(pcsAgsdSkill)
			pcsAgsdSkill->m_eMissedReason = AGSDSKILL_MISSED_REASON_TAME_FAILURE_ABNORMAL;

		return FALSE;
	}

	return TRUE;
}

// 2005.08.31. steeple
// 소환 타입4. 맥스 소환수 늘리삼.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType4(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagpmSummons || !m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// 바로 추가해준다.
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIncreaseSummonsNum = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_LIMIT_QUANTITY][lSkillLevel];
	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIncreaseSummonsNum);

	// Max Count 패킷 보내준다.
	m_pagsmSummons->SendSetMaxSummons((AgpdCharacter*)pcsTarget, m_pagpmSummons->GetMaxSummonsCount((AgpdCharacter*)pcsTarget));

	return TRUE;
}

// 2005.09.18. steeple
// 소환 타입5. 소환수에게 버프를 걸어주는데, 이미 소환되어 있는 상태이면 걔들에게 걸어주고,
// 새로 소환되는 애들에게도 자동으로 걸어줘야 한다.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType5(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagpmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// 이 스킬은 PC 전용이다!
	if(!m_pagpmCharacter->IsPC((AgpdCharacter*)pcsTarget))
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// ADDITIONAL_SKILL_TID 가 없으면 GG
	//INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	INT32 lSkillLevel = m_pagpmSkill->GetSkillLevel(pcsSkill);
	if(pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel) == 0.0f)
		return FALSE;

	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_SUMMONS;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE5;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lSummonsType5Count++;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lSummonsType5Count = 1;

	// 현재 소환된 소환수가 있다면 돌면서 추가 효과를 Cast 해준다.
	if(m_pagpmSummons->GetSummonsCount((AgpdCharacter*)pcsTarget) < 1)
		return TRUE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pagpmSummons->GetADCharacter((AgpdCharacter*)pcsTarget);
	if(!pcsSummonsADChar)
		return TRUE;

	AgpdCharacter* pcsSummons = NULL;

	// 돈다.
	
	if(!pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	AgpdSummonsArray::SummonsVector tmpVector(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
												pcsSummonsADChar->m_SummonsArray.m_pSummons->end());
	AgpdSummonsArray::iterator iter = tmpVector.begin();
	while(iter != tmpVector.end())
	{
		// 소환수만 돈다.
		if(iter->m_eType != AGPMSUMMONS_TYPE_ELEMENTAL)
		{
			++iter;
			continue;
		}

		pcsSummons = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsSummons)
		{
			++iter;
			continue;
		}

		m_pagsmSkill->CastSkill(	pcsTarget,
					pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel),
					lSkillLevel,
					pcsSummons);

		pcsSummons->m_Mutex.Release();

		++iter;
	}

	return TRUE;
}

// 2006.10.31. steeple
// bForce 를 추가하면서 이렇게 따로 갈 필요가 없어졌다.
// 그냥 Type1 호출한다.
//
// 2006.08.22. steeple
// SiegeWar 용 Summons. 임시로 MaxSummonsCount 를 하나 늘려줘야 한다.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType6(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	return ProcessSkillEffect2SummonsType1(pcsSkill, pcsTarget, bProcessInterval);
	//if(!m_pagsmSummons)
	//	return FALSE;

	//if(!pcsSkill || !pcsTarget)
	//	return FALSE;

	//if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
	//	return FALSE;

	//// 임시로 하나 늘려준다. 버프가 꺼질 때 하나 줄여야 한다.
	//m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, 1);
	//m_pagsmSummons->SendSetMaxSummons((AgpdCharacter*)pcsTarget, m_pagpmSummons->GetMaxSummonsCount((AgpdCharacter*)pcsTarget));

	//if(ProcessSkillEffect2SummonsType1(pcsSkill, pcsTarget, bProcessInterval) == FALSE)
	//{
	//	// 롤백
	//	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, -1);
	//	m_pagsmSummons->SendSetMaxSummons((AgpdCharacter*)pcsTarget, m_pagpmSummons->GetMaxSummonsCount((AgpdCharacter*)pcsTarget));
	//	return FALSE;
	//}

	//return TRUE;
}

// 2006.09.28. steeple
// 캐스터를 주인으로 하는 몬스터를 스폰한다. 일반적인 스폰을 사용한다.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType7(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lSummonsTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lSummonsCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lSummonsCount == 0)
		return FALSE;	// 얘는 개수 지정 안되어 있으면 나간다.
		//lSummonsCount = 10;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
	if(ulDuration == 0)
		return FALSE;

	if(lSummonsTID > 0)
	{
		// 마지막 인자를 FALSE 를 주어서 패킷은 보내지 않는다.
		INT32 lCount = m_pagsmSummons->ProcessSummons(static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID,
										ulDuration, lSummonsCount, lSkillLevel, AGPMSUMMONS_TYPE_ELEMENTAL, AGPMSUMMONS_PROPENSITY_DEFENSE,/*AGPMSUMMONS_PROPENSITY_ATTACK*///JK_소환수선공수정
										pcsSkill->m_bForceAttack, FALSE);
		if(1 < lCount)
			return FALSE;
	}
	else
	{
		// Caster 의 레벨 +-5 사이의 몬스터를 각각 Random 하게 만든다.
		INT32 lCasterLevel = m_pagpmCharacter->GetLevel((AgpdCharacter*)pcsSkill->m_pcsBase);

		// 캐스터의 레벨에 맞는 몬스터 TID 를 벡터에 저장한다
		static MapMob MapTemplate;
		if(MapTemplate.size() == 0)
		{
			INT32 lIndex = 0;
			for(AgpdCharacterTemplate* pcsTemplate = m_pagpmCharacter->GetTemplateSequence(&lIndex);
				pcsTemplate;
				pcsTemplate = m_pagpmCharacter->GetTemplateSequence(&lIndex))
			{
				// 몬스터가 아니면 지나간다.
				if(!(pcsTemplate->m_ulCharType & AGPMCHAR_TYPE_MONSTER))
					continue;

				// 몬스터는 Race 가 11 로 세팅되어 있다. (CharacterDataTable 참고)
				if(m_pagpmCharacter->GetRaceFromTemplate(pcsTemplate) != 11)
					continue;

				// Taming 가능 한 애들만 소환한다.
				if(pcsTemplate->m_eTamableType == AGPDCHAR_TAMABLE_TYPE_NONE)
					continue;

				INT32 lMonsterLevel = m_pagpmCharacter->GetLevelFromTemplate(pcsTemplate);

				MapTemplate.insert(std::make_pair(lMonsterLevel, pcsTemplate->m_lID));
			}
		}

		if(MapTemplate.size() == 0)
			return FALSE;

		// 2007.09.12. steeple
		// 레벨 차이가 많이 난다 해도 전범위에서 구할 수 있게 한다.
		INT32 lMinLevel, lMaxLevel, lMaxMobLevel;
		lMinLevel = lMaxLevel = lMaxMobLevel = 0;

		MapRIterMob riter = MapTemplate.rbegin();
		if(riter != MapTemplate.rend())
			lMaxMobLevel = riter->first;

		if(lMaxMobLevel < lCasterLevel + 5)
			lMaxLevel = lMaxMobLevel;
		else
			lMaxLevel = lCasterLevel + 5;

		lMinLevel = lMaxLevel - 10;

		MapIterMob iterStart	= MapTemplate.lower_bound(lMinLevel);
		MapIterMob iterEnd		= MapTemplate.upper_bound(lMaxLevel);

		if(iterStart == MapTemplate.end() || iterEnd == MapTemplate.begin())
			return FALSE;

		std::vector<INT32> vcTemplate;
		for(iterStart; iterStart != iterEnd; ++iterStart)
		{
			vcTemplate.push_back(iterStart->second);
		}
		
		if(vcTemplate.size() == 0)
			return FALSE;

		for(INT32 i = 0; i < lSummonsCount; ++i)
		{
			INT32 lRand = m_csRandom->randInt((int)vcTemplate.size());
			if(lRand >= (int)vcTemplate.size())
				lRand = (int)vcTemplate.size() - 1;
			lSummonsTID = vcTemplate[lRand];

			// 얘들은 공격형이다. 후후훗.
			// 마지막 인자를 FALSE 를 주어서 패킷은 보내지 않는다.
			// 여기서는 에러처리 하지 않는다.
			m_pagsmSummons->ProcessSummons(static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID, ulDuration, 1, lSkillLevel,
											AGPMSUMMONS_TYPE_ELEMENTAL, AGPMSUMMONS_PROPENSITY_DEFENSE,/*AGPMSUMMONS_PROPENSITY_ATTACK*/ pcsSkill->m_bForceAttack, FALSE);//JK_소환수선공수정
		}
	}

	return TRUE;
}

// 2007.03.20. steeple
// Pet 을 소환한다.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType8(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// 기존 개수를 체크해서 맥스 소환 수치에 다달았다면 
	// 먼저 번 거를 없애주고 새로 소환하던가 소환이 안되게 처리하자.

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lSummonsTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lSummonsCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lSummonsCount == 0)
		lSummonsCount = 1;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
	
	INT32 lCreateNum = m_pagsmSummons->ProcessSummons(static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID, ulDuration, lSummonsCount, lSkillLevel,
													AGPMSUMMONS_TYPE_PET, AGPMSUMMONS_PROPENSITY_SHADOW, pcsSkill->m_bForceAttack);
	if(lCreateNum < 1)
	{
		// 에러 메시지를 보내줘야 하는데
		return FALSE;
	}

	// 소환될 때 CBEnterGameWorld 를 타고 Passive 스킬들은 이 때 Cast 된다.
	// 그럼 딱히 할 거 없다???? -_-

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType9(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// 기존 개수를 체크해서 맥스 소환 수치에 다달았다면 
	// 먼저 번 거를 없애주고 새로 소환하던가 소환이 안되게 처리하자.

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lSummonsTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lSummonsCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lSummonsCount == 0)
		lSummonsCount = 1;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);

	AuPOS stSummonPosition = m_pagsmSummons->GetFrontofCharacterPos((AgpdCharacter*)pcsTarget);

	// 소환할수 있는 Max치를 임시로 하나 올려준다.
	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, 1);

	INT32 lCreateNum = m_pagsmSummons->ProcessSummons(stSummonPosition, static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID, ulDuration, lSummonsCount, lSkillLevel,
														AGPMSUMMONS_TYPE_ELEMENTAL, AGPMSUMMONS_PROPENSITY_DEFENSE, pcsSkill->m_bForceAttack);
	if(lCreateNum < 1)
	{
		// 에러 메시지를 보내줘야 하는데
		return FALSE;
	}

	// 올려놓았던 Max치를 다시 하나 내려준다.
	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, -1);

	// 소환될 때 CBEnterGameWorld 를 타고 Passive 스킬들은 이 때 Cast 된다.
	// 그럼 딱히 할 거 없다???? -_-

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType10(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// 기존 개수를 체크해서 맥스 소환 수치에 다달았다면 
	// 먼저 번 거를 없애주고 새로 소환하던가 소환이 안되게 처리하자.

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	INT32 lSummonsTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lSummonsCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lSummonsCount == 0)
		lSummonsCount = 1;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);

	AuPOS stSummonPosition = m_pagsmSummons->GetFrontofCharacterPos((AgpdCharacter*)pcsTarget);

	// 소환할수 있는 Max치를 임시로 하나 올려준다.
	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, 1);

	INT32 lCreateNum = m_pagsmSummons->ProcessSummons(static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID, ulDuration, lSummonsCount, lSkillLevel,
														AGPMSUMMONS_TYPE_MONSTER, AGPMSUMMONS_PROPENSITY_DEFENSE, pcsSkill->m_bForceAttack);
	if(lCreateNum < 1)
	{
		// 에러 메시지를 보내줘야 하는데
		return FALSE;
	}

	// 올려놓았던 Max치를 다시 하나 내려준다.
	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, -1);

	// 소환될 때 CBEnterGameWorld 를 타고 Passive 스킬들은 이 때 Cast 된다.
	// 그럼 딱히 할 거 없다???? -_-

	return TRUE;
}

// 2005.11.25. steeple
// Game Bonus Exp Skill
BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonusExp(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_EXP][lSkillLevel];

	// PC 방 아이템인지 체크한다.
	double lPCBangBonusRate =  m_pagsmSkill->GetPCBangItemSkillBonusRate(pcsSkill);
	if(lPCBangBonusRate != 0)
		lValue = (INT32)((double)lValue * lPCBangBonusRate);

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lBonusExpRate = lValue;
	pcsAttachData->m_stBuffedSkillFactorEffectArg.lBonusExpRate += lValue;
	
	m_pagpmCharacter->AddGameBonusExp((AgpdCharacter*)pcsTarget, lValue);

	return TRUE;
}

// 2005.11.25. steeple
// Game Bonus Money Skill
BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonusMoney(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_MONEY][lSkillLevel];

	// PC 방 아이템인지 체크한다.
	double lPCBangBonusRate =  m_pagsmSkill->GetPCBangItemSkillBonusRate(pcsSkill);
	if(lPCBangBonusRate != 0)
		lValue = (INT32)((double)lValue * lPCBangBonusRate);

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lBonusMoneyRate = lValue;
	pcsAttachData->m_stBuffedSkillFactorEffectArg.lBonusMoneyRate += lValue;

	m_pagpmCharacter->AddGameBonusMoney((AgpdCharacter*)pcsTarget, lValue);

	return TRUE;
}

// 2006.06.01. steeple
// Game Bonus Drop Rate Skill
BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonusDropRate(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_DROP_RATE][lSkillLevel];

	// PC 방 아이템인지 체크한다.
	double lPCBangBonusRate =  m_pagsmSkill->GetPCBangItemSkillBonusRate(pcsSkill);
	if(lPCBangBonusRate != 0)
		lValue = (INT32)((double)lValue * lPCBangBonusRate);

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lBonusDropRate = lValue;
	pcsAttachData->m_stBuffedSkillFactorEffectArg.lBonusDropRate += lValue;

	m_pagpmCharacter->AddGameBonusDropRate((AgpdCharacter*)pcsTarget, lValue);

	return TRUE;
}

// 2008.07.10. iluvs
// Game Bonus Drop Rate Skill2
BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonusDropRate2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_BONUS_DROP_RATE2][lSkillLevel];

	// PC 방 아이템인지 체크한다.
	double lPCBangBonusRate =  m_pagsmSkill->GetPCBangItemSkillBonusRate(pcsSkill);
	if(lPCBangBonusRate != 0)
		lValue = (INT32)((double)lValue * lPCBangBonusRate);

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lBonusDropRate2 = lValue;
	pcsAttachData->m_stBuffedSkillFactorEffectArg.lBonusDropRate2 += lValue;

	m_pagpmCharacter->AddGameBonusDropRate2((AgpdCharacter*)pcsTarget, lValue);

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonusCharismaRate(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_CHARISMA_RATE][lSkillLevel];

	// PC 방 아이템인지 체크한다.
	double lPCBangBonusRate =  m_pagsmSkill->GetPCBangItemSkillBonusRate(pcsSkill);
	if(lPCBangBonusRate != 0)
		lValue = (INT32)((double)lValue * lPCBangBonusRate);

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lBonusDropCharismaRate = lValue;
	pcsAttachData->m_stBuffedSkillFactorEffectArg.lBonusDropCharismaRate += lValue;

	m_pagpmCharacter->AddGameBonusCharismaRate((AgpdCharacter*)pcsTarget, lValue);

	return TRUE;
}

// 2007.07.04. steeple
// Detect Type1. 투명을 반투명으로 만든다.
BOOL AgsmSkillEffect::ProcessSkillEffect2DetectType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// 이곳에서 딱히 할 건 없다.

	return TRUE;
}

// 2007.07.04. steeple
// Detect Type2. 그냥 투명을  다 날린다.
BOOL AgsmSkillEffect::ProcessSkillEffect2DetectType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// 그냥 풀어준다.
	m_pagsmSkill->EndTransparentBuffedSkill(pcsTarget);

	// 혹시 모르니 체크 한번 더 한다.
	if(m_pagpmCharacter->IsStatusFullTransparent((AgpdCharacter*)pcsTarget))
		m_pagpmCharacter->UpdateUnsetSpecialStatus((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT);
	if(m_pagpmCharacter->IsStatusHalfTransparent((AgpdCharacter*)pcsTarget))
		m_pagpmCharacter->UpdateUnsetSpecialStatus((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT);

	return TRUE;
}

// 2006.10.10. steeple
// Game Effect. Change day to night or night to day.
BOOL AgsmSkillEffect::ProcessSkillEffect2GameEffectDayNight(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagpmTimer || !m_pagsmTimer)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// 시간을 바꿔준다. 그냥 12시간 늘려버린다.
	UINT8 ucHour = m_pagpmTimer->GetCurHour();
	UINT8 ucHourConverted;
	ucHourConverted = ( ucHour + 12 ) % 24;

	m_pagpmTimer->SetTime((INT32)ucHourConverted, 0, 0, NULL);
	
	// 전체 유저에게 뿌린다.
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmTimer->MakePacketTime(&nPacketLength);
	if(!pvPacket || nPacketLength < 1)
	{
		m_pagpmTimer->SetTime((INT32)ucHour, 0, 0, NULL);
		return FALSE;
	}

	m_pagsmSkill->SendPacketAllUser(pvPacket, nPacketLength, PACKET_PRIORITY_6);
	m_pagpmTimer->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

// 2006.11.08. steeple
BOOL AgsmSkillEffect::ProcessSkillEffect2GameEffectRain(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmEventNature)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill);
	m_pagsmEventNature->SetWeatherWithDuration(AGPDNATURE_WEATHER_TYPE_RAINY, ulDuration);

	return TRUE;
}

// 2006.11.08. steeple
BOOL AgsmSkillEffect::ProcessSkillEffect2GameEffectSnow(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmEventNature)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill);
	m_pagsmEventNature->SetWeatherWithDuration(AGPDNATURE_WEATHER_TYPE_SNOWY, ulDuration);

	return TRUE;
}

// 2007.06.26. steeple
BOOL AgsmSkillEffect::ProcessSkillEffect2DivideAttr(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideAttrRate = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DIVIDE_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideAttrCaster = pcsSkill->m_pcsBase->m_lID;

	// 얘는 무조건 엎어친다.
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_DIVIDE;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_DIVIDE] |= AGPMSKILL_EFFECT_DETAIL_DIVIDE_ATTR;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDivideAttrRate = pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideAttrRate;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDivideAttrCaster = pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideAttrCaster;

	return TRUE;
}

// 2007.06.26. steeple
BOOL AgsmSkillEffect::ProcessSkillEffect2DivideNormal(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideNormalRate = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DIVIDE_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideNormalCaster = pcsSkill->m_pcsBase->m_lID;

	// 얘는 무조건 엎어친다.
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_DIVIDE;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_DIVIDE] |= AGPMSKILL_EFFECT_DETAIL_DIVIDE_NORMAL;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDivideNormalRate = pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideNormalRate;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDivideNormalCaster = pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideNormalCaster;

	return TRUE;
}

// 2007.10.29. steeple
// Union Type1 처리
BOOL AgsmSkillEffect::ProcessSkillEffect2UnionType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgsdSkillADBase* pcsAgsdADBase = m_pagsmSkill->GetADBase(pcsTarget);
	if(!pcsAgsdADBase)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stUnionInfo.m_lUnionID = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_UNION][lSkillLevel];
	
	UnionControlArray::iterator iter = std::find(pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.begin(),
												pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end(),
												pcsAgsdSkill->m_stUnionInfo.m_lUnionID);
	if(iter != pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end())
	{
		++iter->m_lCurrentCount;
	}
	else
	{
		iter = std::find(pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.begin(),
						pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end(),
						0);
		if(iter == pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end())
			return FALSE;	// 이렇게 되면 다 차있는 거라서 배열의 크기를 늘리던가 해야한다.

		iter->m_lUnionID = pcsAgsdSkill->m_stUnionInfo.m_lUnionID;
		iter->m_lCurrentCount = 1;
	}

	return TRUE;
}

// 2007.10.29. steeple
// Union Type2 처리
BOOL AgsmSkillEffect::ProcessSkillEffect2UnionType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgsdSkillADBase* pcsAgsdADBase = m_pagsmSkill->GetADBase(pcsTarget);
	if(!pcsAgsdADBase)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stUnionInfo.m_lUnionID = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_UNION_CONTROL][lSkillLevel];
	pcsAgsdSkill->m_stUnionInfo.m_lMaxCount = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_MAX_SKILL_UNION][lSkillLevel];

	UnionControlArray::iterator iter = std::find(pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.begin(),
												pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end(),
												pcsAgsdSkill->m_stUnionInfo.m_lUnionID);
	if(iter != pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end())
	{
		// 기존에 Union Max Control 치가 있다면 더해주고 끝이다.
		iter->m_lMaxCount += pcsAgsdSkill->m_stUnionInfo.m_lMaxCount;
	}
	else
	{
		iter = std::find(pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.begin(),
						pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end(),
						0);
		if(iter == pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end())
			return FALSE;	// 이렇게 되면 다 차있는 거라서 배열의 크기를 늘리던가 해야한다.

		iter->m_lUnionID = pcsAgsdSkill->m_stUnionInfo.m_lUnionID;
		iter->m_lMaxCount += pcsAgsdSkill->m_stUnionInfo.m_lMaxCount;
	}

	return TRUE;
}

// 2007.07.03. steeple
// Dispel Stun
BOOL AgsmSkillEffect::ProcessSkillEffect2DispelStun(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// 그냥 풀어주면 된다.
	m_pagpmCharacter->UpdateUnsetSpecialStatus((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_STUN);

	return TRUE;
}

// 2007.07.03. steeple
// Dispel Slow
// 버프된 스킬에서 Slow 속성을 다 0으로 바꿔주고, RecalcFactor 부른다.
BOOL AgsmSkillEffect::ProcessSkillEffect2DispelSlow(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkillADBase* pcsADBase = m_pagsmSkill->GetADBase(pcsTarget);
	if(!pcsADBase)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = NULL;
	BOOL bUpdated = FALSE;

	for(INT32 i = 0; i < AGPMSKILL_MAX_SKILL_BUFF; i++)
	{
		if(!pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill ||
			!pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill->m_pcsTemplate)
			break;

		pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill);
		if(!pcsAgsdSkill)
			break;

		// Factor 를 찾아본다.
		FLOAT fValue = 0.0f;
		m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		if((INT32)fValue < 0)
		{
			bUpdated = TRUE;

			// 일단 팩터를 빼고
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPoint, pcsAgsdSkill->m_pcsUpdateFactorPoint, FALSE, FALSE, FALSE, FALSE);

			// 0 으로 세팅한 후
			fValue = 0.0f;
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			// 다시 더한다.
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPoint, pcsAgsdSkill->m_pcsUpdateFactorPoint, FALSE, FALSE, TRUE, FALSE);
		}

		fValue = 0.0f;
		m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		if((INT32)fValue < 0)
		{
			bUpdated = TRUE;

			// 일단 팩터를 빼고
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPercent, pcsAgsdSkill->m_pcsUpdateFactorPercent, FALSE, FALSE, FALSE, FALSE);

			// 0 으로 세팅한 후
			fValue = 0.0f;
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			// 다시 더한다.
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPercent, pcsAgsdSkill->m_pcsUpdateFactorPercent, FALSE, FALSE, TRUE, FALSE);
		}

		fValue = 0.0f;
		m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPoint, &fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		if((INT32)fValue < 0)
		{
			bUpdated = TRUE;

			// 일단 팩터를 빼고
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPoint, pcsAgsdSkill->m_pcsUpdateFactorItemPoint, FALSE, FALSE, FALSE, FALSE);

			// 0 으로 세팅한 후
			fValue = 0.0f;
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPoint, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPoint, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			// 다시 더한다.
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPoint, pcsAgsdSkill->m_pcsUpdateFactorItemPoint, FALSE, FALSE, TRUE, FALSE);
		}

		fValue = 0.0f;
		m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPercent, &fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		if((INT32)fValue > 0)
		{
			bUpdated = TRUE;

			// 일단 팩터를 빼고
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPercent, pcsAgsdSkill->m_pcsUpdateFactorItemPercent, FALSE, FALSE, FALSE, FALSE);

			// 0 으로 세팅한 후
			fValue = 0.0f;
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPercent, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPercent, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			// 다시 더한다.
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPercent, pcsAgsdSkill->m_pcsUpdateFactorItemPercent, FALSE, FALSE, TRUE, FALSE);
		}
	}

	if(bUpdated)
		m_pagsmCharacter->ReCalcCharacterResultFactors((AgpdCharacter*)pcsTarget, TRUE);

	return TRUE;
}

// 2007.07.03. steeple
// Dispel All Buff / Debuff except cash, summons buff
BOOL AgsmSkillEffect::ProcessSkillEffect2DispelAllBuff(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// 그냥 풀어준다. -0-;
	BOOL bInitCoolTime = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_stConditionArg[0].lArg1;
	INT32 lMaxCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_LIMIT_QUANTITY][lSkillLevel];

	m_pagsmSkill->EndAllBuffedSkillExceptCash_SummonsSkill(pcsTarget, TRUE, bInitCoolTime, lMaxCount);

	return TRUE;
}

// 2007.07.03. steeple
// Dispel Transparent
BOOL AgsmSkillEffect::ProcessSkillEffect2DispelTransparent(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// 그냥 풀어준다.
	m_pagsmSkill->EndTransparentBuffedSkill(pcsTarget);

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2DispelSpecialStatus(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AgpdSkillTemplate *pcsSkillTemplate = (AgpdSkillTemplate *)pcsSkill->m_pcsTemplate;
	if(NULL == pcsSkillTemplate)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	// If AIState is none, there is nothing to do dispel specialstatus
	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_NONE)
		return TRUE;

	BOOL bInitState = FALSE;

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_CONFUSION)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_CONFUSION)
		{
			bInitState = TRUE;
		}
	}

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_HOLD)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_HOLD)
		{
			bInitState = TRUE;
		}
	}

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_FEAR)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_FEAR)
		{
			bInitState = TRUE;
		}
	}

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISEASE)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_DISEASE)
		{
			bInitState = TRUE;
		}
	}

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_BERSERK)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_BERSERK)
		{
			bInitState = TRUE;
		}
	}

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SHRINK)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_SHRINK)
		{
			bInitState = TRUE;
		}
	}

	if(bInitState)
		m_pagpmAI2->InitPCAIInfo((AgpdCharacter*)pcsTarget);

	return TRUE;
}

// 2005.07.05. steeple
// 어떤 Action 이 일어나면서 일어나야 하는 Action 처리
// 이 함수는 ProcessSkillEffect 에서 처리하는 것이 아니라 필요한 곳에서 아무렇게나 부르면 된다.
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnAction(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT32 lActionOnActionType, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1)
			bResult |= ProcessSkillEffect2ActionOnActionType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2)
			bResult |= ProcessSkillEffect2ActionOnActionType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3)
			bResult |= ProcessSkillEffect2ActionOnActionType3(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4)
			bResult |= ProcessSkillEffect2ActionOnActionType4(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE5 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE5)
			bResult |= ProcessSkillEffect2ActionOnActionType5(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6)
			bResult |= ProcessSkillEffect2ActionOnActionType6(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1)
			bResult |= ProcessSkillEffect2ActionOnActionType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2)
			bResult |= ProcessSkillEffect2ActionOnActionType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3)
			bResult |= ProcessSkillEffect2ActionOnActionType3(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4)
			bResult |= ProcessSkillEffect2ActionOnActionType4(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE5 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE5)
			bResult |= ProcessSkillEffect2ActionOnActionType5(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6)
			bResult |= ProcessSkillEffect2ActionOnActionType6(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2005.07.05. steeple
// 버프가 끝나는 시점에 불리게 되며, 불리면 데미지를 준다.
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// 혹시 pcsSkill->m_pcsBase 가 NULL 이면 구해준다.
	BOOL bIsBaseNULL = FALSE;
	if(!pcsSkill->m_pcsBase)
	{
		pcsSkill->m_pcsBase = (ApBase*)m_pagpmCharacter->GetCharacterLock(pcsSkill->m_csBase.m_lID);
		bIsBaseNULL = TRUE;
	}

	// 못 구했다면 나가자.
	if(!pcsSkill->m_pcsBase)
		return FALSE;

	// 2005.11.01. steeple
	// 이 타입의 스킬은 지금 시점에 어그로 쌓이고, 적대관계되고, 선공되고 그래야 한다.
	// Attack 스킬도 아니고 디버프 스킬도 아니게 세팅되어 있는 타입.
	// 그러므로 지금 임의로 불러줘야 한다.
	stAgsmCombatAttackResult	stAttackResult;
	memset(&stAttackResult, 0, sizeof(stAttackResult));
	stAttackResult.pAttackChar	= (AgpdCharacter*)pcsSkill->m_pcsBase;
	stAttackResult.pTargetChar	= (AgpdCharacter*)pcsTarget;
	m_pagsmCombat->EnumCallback(AGSMCOMBAT_CB_ID_ATTACK_START, &stAttackResult, NULL);

	// Melee Attack 1 번으로 데미지를 계산하다. 물론 Const 에 세팅 되어 있어야 한다.
	ProcessSkillEffectMeleeAttackType1(pcsSkill, pcsTarget, 0, bProcessInterval);

	// Additional Effect 를 먼저 보내고
	m_pagsmSkill->SendAdditionalEffect(pcsTarget, AGPMSKILL_ADDITIONAL_EFFECT_END_BUFF_EXPLOSION);

	// factor를 보고 업데이트 시킨다.
	m_pagsmSkill->UpdateSkillFactor(pcsSkill, pcsTarget, TRUE, bProcessInterval);

	// 2005.01.28. steeple. Attack 후의 죽은 것을 체크한다.
	m_pagsmSkill->ApplySkillAttack(pcsSkill, pcsTarget);

	// pcsBase 가 NULL 이어서 새로 구한 거라면
	if(bIsBaseNULL)
	{
		pcsSkill->m_pcsBase->m_Mutex.Release();
		pcsSkill->m_pcsBase = NULL;
	}

	return TRUE;
}

// 2005.07.05. steeple
// 캐스트가 끝나는 시점에 불리게 되며, 스킬 시전자를 Target 에게 점프 시키고 공격들어간다.
//
// 2005.07.05. steeple
// Skill_Spec 문서에서 공격으로 세팅하고 Melee Attack 을 세팅하고
// Action On Action 을 2로 주면 여기서는 텔레포트만 시켜주면 될 것 같다. 나중에 실제로 돌려보고 체크하자.
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsSkill->m_pcsBase)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// 공성 오브젝트에는 사용불가. 2007.05.07. steeple
	AgpdSiegeWarMonsterType eType = m_pagpmSiegeWar->GetSiegeWarMonsterType((AgpdCharacter*)pcsTarget);
	if(eType >= AGPD_SIEGE_MONSTER_GUARD_TOWER && eType <= AGPD_SIEGE_MONSTER_CATAPULT)
		return FALSE;

	// 먼저 시전자를 정지 시킨다.
	m_pagpmCharacter->StopCharacter((AgpdCharacter*)pcsSkill->m_pcsBase, NULL);

	AuPOS stTargetPos = ((AgpdCharacter*)pcsTarget)->m_stPos;

	if(m_pagpmCharacter->UpdatePosition((AgpdCharacter*)pcsSkill->m_pcsBase, &stTargetPos, TRUE, TRUE))
	{
		// 텔레포트 했다는 이펙트를 보내준다.
		m_pagsmSkill->SendAdditionalEffect(pcsSkill->m_pcsBase, AGPMSKILL_ADDITIONAL_EFFECT_TELEPORT);

		//// Melee Attack 1 번으로 데미지를 계산하다. 물론 Const 에 세팅 되어 있어야 한다.
		//ProcessSkillEffectMeleeAttackType1(pcsSkill, pcsTarget, 0, bProcessInterval);

		//// factor를 보고 업데이트 시킨다.
		//UpdateSkillFactor(pcsSkill, pcsTarget, TRUE, bProcessInterval);

		//// 2005.01.28. steeple. Attack 후의 죽은 것을 체크한다.
		//ApplySkillAttack(pcsSkill, pcsTarget);
	}

	return TRUE;
}

// 2006.08.31. steeple
// Action on Action Type5. ex) 공성용 생명의탑. (버프가 걸린 유저는 시전자의 상태를 계속 체크한다.)
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType5(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	// 이 안에선 할 게 별로 없다. Pass
	return TRUE;
}

// 2007.07.10. steeple
// Action on Action Type6. 죽기 직전에 Additional Skill TID 를 시전해준다.
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType6(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	// 그냥 일단 세팅만 해놓는다.
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6;

	return TRUE;
}

//		ProcessSkillEffectMoveTarget
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_MOVE_TARGET) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			target을 수평이동
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMoveTarget(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsSkill->m_pcsBase || !pcsSkill->m_bCloneObject || !pcsSkill->m_pcsTemplate || !pcsTarget)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32	lMoveDistance = (INT32) ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_MOVE_DISTANCE][lSkillLevel];

	// AGSMSKILL_CONST_MOVE_DISTANCE 만큼 옮긴다. 수평으로..
	////////////////////////////////////////////////////////////////////////////
	return m_pagpmCharacter->MoveCharacterHorizontal((AgpdCharacter *) pcsTarget, m_papmEventManager->GetBasePos(pcsSkill->m_pcsBase, NULL), lMoveDistance);
}

//		ProcessSkillEffectMagicAttackType1
//	Functions
//		- pcsSkill 의 skill effect type (AGSMSKILL_EFFECT_MAGIC_ATTACK_DMG_TYPE1) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMagicAttackType1(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	return TRUE;
}

//		ProcessSkillEffectUpdateFactorPermanently
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_UPDATE_FACTOR_PERMANENTLY) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			세팅된 값들을 factor point에 더한다. 영구적으로 더하는 값이다. result factor가 아니라 character factor에 더한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorPermanently(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate =  (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	// target의 레벨을 구한다.
	INT32	lTargetLevel = 0;

	switch (pcsTarget->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			lTargetLevel	= m_pagpmCharacter->GetLevel((AgpdCharacter *) pcsTarget);
		}
		break;

	default:
		break;
	}

	if (lTargetLevel <= 0)
		return FALSE;

	BOOL	bIsUpdated = FALSE;

	// factor를 올려준다.
	for (int i = AGPMSKILL_CONST_POINT_START; i < AGPMSKILL_CONST_PERCENT_END; ++i)
	{
		if (pcsSkillTemplate->m_fUsedConstFactor[i][lTargetLevel] != 0 || pcsSkillTemplate->m_fUsedConstFactor2[i][lTargetLevel] != 0)
			if (m_pagsmSkill->UpdateFactorPermanently(pcsSkill, pcsTarget, (eAgpmSkillUsedConstFactor) i))
				bIsUpdated = TRUE;
	}

	if (bIsUpdated)
	{
		// 베이쑤 factor가 바뀌었으니 result factor를 다시 계산해 줘야 한다.
		m_pagsmCharacter->ReCalcCharacterResultFactors((AgpdCharacter *) pcsTarget);
	}

	return TRUE;
}

//		ProcessSkillEffectUpdateFactorPermanentlyRandomStatus
//	Functions
//		- pcsSkill 의 skill effect type (AGPMSKILL_EFFECT_UPDATE_FACTOR_PERMANENTLY_RANDOM_STATUS) 에 따른 함수를 호출해 실제 스킬 effect를 적용한다.
//			세팅된 값들을 factor point에 더한다. 영구적으로 더하는 값이다. result factor가 아니라 character factor에 더한다.
//	Arguments
//		- pcsSkill	: 처리할 AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorPermanentlyRandomStatus(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate =  (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	// target의 레벨을 구한다.
	INT32	lTargetLevel = 0;

	switch (pcsTarget->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			lTargetLevel	= m_pagpmCharacter->GetLevel((AgpdCharacter *) pcsTarget);
		}
		break;

	default:
		break;
	}

	if (lTargetLevel <= 0)
		return FALSE;

	// factor status중 하나를 고른다.
	INT32	lRandomStatus = m_csRandom->randInt(5);

	BOOL	bIsUpdated	= FALSE;

	// 고른넘 UsedConstFactor 가 세팅되어있는지 본다.
	if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_CON + lRandomStatus][lTargetLevel] != 0 ||
		pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_POINT_CON + lRandomStatus][lTargetLevel] != 0 )
	{
		if (m_pagsmSkill->UpdateFactorPermanently(pcsSkill, pcsTarget, (eAgpmSkillUsedConstFactor) (AGPMSKILL_CONST_POINT_CON + lRandomStatus)))
			bIsUpdated = TRUE;
	}
	else if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_CON + lRandomStatus][lTargetLevel] != 0 ||
				pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_PERCENT_CON + lRandomStatus][lTargetLevel] != 0)
	{
		if (m_pagsmSkill->UpdateFactorPermanently(pcsSkill, pcsTarget, (eAgpmSkillUsedConstFactor) (AGPMSKILL_CONST_PERCENT_CON + lRandomStatus)))
			bIsUpdated = TRUE;
	}

	if (bIsUpdated)
	{
		// 베이쑤 factor가 바뀌었으니 result factor를 다시 계산해 줘야 한다.
		m_pagsmCharacter->ReCalcCharacterResultFactors((AgpdCharacter *) pcsTarget);
	}

	return TRUE;
}


// 2007.12.05. steeple
// Move Position
BOOL AgsmSkillEffect::ProcessSkillEffect2MovePos(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET)
			bResult |= ProcessSkillEffect2MovePosTarget(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_SELF)
			bResult |= ProcessSkillEffect2MovePosSelf(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET_TO_ME)
			bResult |= ProcessSkillEffect2MovePosTargetToMe(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET)
			bResult |= ProcessSkillEffect2MovePosTarget(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_SELF)
			bResult |= ProcessSkillEffect2MovePosSelf(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET_TO_ME)
			bResult |= ProcessSkillEffect2MovePosTargetToMe(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2007.12.05. steeple
// Move Position Target
BOOL AgsmSkillEffect::ProcessSkillEffect2MovePosTarget(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsSkill->m_pcsTemplate || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bResult = FALSE;
	if(pcsSkill->m_pcsBase)
	{
		AuPOS stFrom = ((AgpdCharacter*)pcsTarget)->m_stPos;
		AuPOS stDest, stDirection;

		AuPOS stDelta = ((AgpdCharacter*)pcsTarget)->m_stPos - ((AgpdCharacter*)pcsSkill->m_pcsBase)->m_stPos;
		FLOAT fDistance = pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_MOVE_DISTANCE][lSkillLevel];

		// 방향 벡터 노멀라이즈
		AuMath::V3DNormalize(&stDirection, &stDelta);

		stDest.x = stFrom.x + stDirection.x * fDistance;
		stDest.y = stFrom.y + stDirection.y * fDistance;
		stDest.z = stFrom.z + stDirection.z * fDistance;

		AuPOS stValidDest = stDest;
		ApmMap::BLOCKINGTYPE eType = m_pagpmCharacter->GetBlockingType((AgpdCharacter*)pcsTarget);

		m_pagpmCharacter->GetValidDestination(&stFrom, &stDest, &stValidDest, eType);

		bResult = m_pagpmCharacter->UpdatePosition((AgpdCharacter*)pcsTarget, &stValidDest);
	}
	else
	{
		bResult = m_pagpmCharacter->MoveCharacterHorizontal((AgpdCharacter*)pcsTarget, &((AgpdCharacter*)pcsTarget)->m_stPos, (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_MOVE_DISTANCE][lSkillLevel]);
	}

	return bResult;
}

// 2007.12.05. steeple
// Move Position Self
BOOL AgsmSkillEffect::ProcessSkillEffect2MovePosSelf(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsSkill->m_pcsTemplate || !pcsSkill->m_pcsBase || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pcsSkill->m_pcsBase);
	AgpdCharacter* pcsTargetCharacter = static_cast<AgpdCharacter*>(pcsTarget);

	AuPOS stFrom = pcsCharacter->m_stPos;
	AuPOS stDest, stDirection;

	AuPOS stDelta = pcsTargetCharacter->m_stPos - pcsCharacter->m_stPos;
	FLOAT fDistance = pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_MOVE_DISTANCE][lSkillLevel];

	// 방향 벡터 노멀라이즈
	AuMath::V3DNormalize(&stDirection, &stDelta);

	stDest.x = stFrom.x + stDirection.x * fDistance;
	stDest.y = stFrom.y + stDirection.y * fDistance;
	stDest.z = stFrom.z + stDirection.z * fDistance;

	AuPOS stValidDest = stDest;
	ApmMap::BLOCKINGTYPE eType = m_pagpmCharacter->GetBlockingType((AgpdCharacter*)pcsTarget);

	m_pagpmCharacter->GetValidDestination(&stFrom, &stDest, &stValidDest, eType);

	// 일단 UpdatePosition 으로 처리하지만, 카메라 워크를 동반한 Express Move 같은 처리가 필요하다.
	BOOL bResult = m_pagpmCharacter->UpdatePosition(pcsCharacter, &stValidDest);
	return bResult;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2MovePosTargetToMe(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsSkill->m_pcsTemplate || NULL == pcsSkill->m_pcsBase || NULL == pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pcsSkill->m_pcsBase);
	AgpdCharacter* pcsTargetCharacter = static_cast<AgpdCharacter*>(pcsTarget);

	AuAutoLock Lock(pcsTargetCharacter->m_Mutex);

	AuPOS stCastCharacterPos = pcsCharacter->m_stPos;
	AuPOS stDest, stDirection;

	AuPOS stDelta = pcsTargetCharacter->m_stPos - pcsCharacter->m_stPos;
	
	// 방향 벡터 노멀라이즈
	AuMath::V3DNormalize(&stDirection, &stDelta);

	FLOAT fDistance = 10.0f;

	stDest.x = stCastCharacterPos.x + stDirection.x * fDistance;
	stDest.y = stCastCharacterPos.y + stDirection.y * fDistance;
	stDest.z = stCastCharacterPos.z + stDirection.z * fDistance;

	AuPOS stValidDest = stDest;
	ApmMap::BLOCKINGTYPE eType = m_pagpmCharacter->GetBlockingType((AgpdCharacter*)pcsTarget);

	m_pagpmCharacter->GetValidDestination(&stCastCharacterPos, &stDest, &stValidDest, eType);

	// 일단 UpdatePosition 으로 처리하지만, 카메라 워크를 동반한 Express Move 같은 처리가 필요하다.
	BOOL bResult = m_pagpmCharacter->UpdatePosition(pcsTargetCharacter, &stValidDest, FALSE, TRUE);

	// 소환수도 옮겨준다.
	if(m_pagpmSummons)
		m_pagpmSummons->UpdateAllSummonsPosToOwner(pcsTargetCharacter);
	
	return bResult;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2DisturbCharacterAction(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget || NULL == pcsSkill->m_pcsBase || NULL == pcsSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillTemplate *pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(NULL == pcsSkillTemplate)
		return FALSE;

	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_ATTACK;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_MOVE;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_USE_ITEM;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_SKILL;
		}	
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_ATTACK;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_MOVE;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_USE_ITEM;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_SKILL;
		}
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2ToleranceDisturbCharacterAction(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AgpdSkillTemplate *pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(NULL == pcsSkillTemplate)
		return FALSE;

	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_ATTACK)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_ATTACK;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_MOVE)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_MOVE;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_USE_ITEM)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_USE_ITEM;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_SKILL)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_SKILL;
		}	
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_ATTACK)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_ATTACK;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_MOVE)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_MOVE;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_USE_ITEM)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_USE_ITEM;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_SKILL)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_SKILL;
		}
	}

	return TRUE;
}