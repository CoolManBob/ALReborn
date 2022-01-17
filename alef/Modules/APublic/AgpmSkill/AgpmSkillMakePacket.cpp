#include "AgpmSkill.h"

PVOID AgpmSkill::MakeBasePacket(ApBase *pcsBase)
{
	if (!pcsBase)
		return NULL;

	INT8	cType = pcsBase->m_eType;
	INT16	nPacketLength = 0;

	return m_csPacketBase.MakePacket(FALSE, &nPacketLength, 0,
										&cType,
										&pcsBase->m_lID);
}

PVOID AgpmSkill::MakeActionPacket(INT8 cActionType, INT32 lSkillLevel, ApBase *pcsBase, PVOID pvResultFactorPacket, AuPOS *pcsPos, BOOL bForceAttack, UINT32 ulCastDelay, UINT32 ulDuration, UINT32 ulRecastDelay, CHAR *szTargetCharID, INT16 nLenTargetCharID, BOOL bIsFactorNotQueueing, UINT32 ulAdditionalEffect)
{
	PVOID	pvPacketTargetBase = MakeBasePacket(pcsBase);

	INT16	nPacketLength;
	INT8	cForceAttack = (INT8) bForceAttack;
	UINT8	cSkillLevel = (UINT8) lSkillLevel;

	PVOID	pvPacketAction = m_csPacketAction.MakePacket(FALSE, &nPacketLength, 0,
										&cActionType,
										pvPacketTargetBase,
										pvResultFactorPacket,
										pcsPos,
										(bForceAttack) ? &cForceAttack : NULL,
										(ulCastDelay > 0) ? &ulCastDelay : NULL,
										(ulDuration > 0) ? &ulDuration : NULL,
										(ulRecastDelay > 0) ? &ulRecastDelay : NULL,
										(cSkillLevel > 0) ? &cSkillLevel : NULL,
										(bIsFactorNotQueueing) ? &bIsFactorNotQueueing : NULL,
										(ulAdditionalEffect != 0) ? &ulAdditionalEffect : NULL,
										szTargetCharID, &nLenTargetCharID
										);

	if (pvPacketTargetBase)
		m_csPacketBase.FreePacket(pvPacketTargetBase);

	return pvPacketAction;
}

PVOID AgpmSkill::MakeCastPacket(INT16 nActionType, INT32 *plSkillID, INT32 *plSkillTID, ApBase *pcsOwnerBase, INT32 lSkillLevel, ApBase *pcsTargetBase, AuPOS *pcsTargetPos, BOOL bForceAttack, UINT32 ulCastDelay, UINT32 ulDuration, UINT32 ulRecastDelay, CHAR *szTargetCharID, INT16 nLenTargetCharID, INT16 *pnPacketLength)
{
	if (!pcsOwnerBase || !pnPacketLength)
		return NULL;

	PVOID	pvPacketOwnerBase = MakeBasePacket(pcsOwnerBase);
	if (!pvPacketOwnerBase)
		return NULL;

	PVOID	pvPacketAction = NULL;

	if (nActionType == AGPMSKILL_ACTION_CAST_SKILL)
		pvPacketAction = MakeActionPacket((INT8) nActionType, lSkillLevel, pcsTargetBase, NULL, pcsTargetPos, bForceAttack, ulCastDelay, ulDuration, ulRecastDelay, szTargetCharID, nLenTargetCharID);
	else if (nActionType == AGPMSKILL_ACTION_START_CAST_SKILL)
		pvPacketAction = MakeActionPacket((INT8) nActionType, lSkillLevel, pcsTargetBase, NULL, pcsTargetPos, bForceAttack, ulCastDelay, ulDuration, ulRecastDelay, szTargetCharID, nLenTargetCharID);
	else if (nActionType == AGPMSKILL_ACTION_END_CAST_SKILL || nActionType == AGPMSKILL_ACTION_CANCEL_CAST_SKILL)
		pvPacketAction = MakeActionPacket((INT8) nActionType, lSkillLevel, pcsTargetBase, NULL, NULL, bForceAttack, ulCastDelay, ulDuration, ulRecastDelay, NULL, 0);
	else if (nActionType == AGPMSKILL_ACTION_MISS_CAST_SKILL)
		pvPacketAction = MakeActionPacket((INT8) nActionType, lSkillLevel, pcsTargetBase, NULL, NULL, bForceAttack, ulCastDelay, ulDuration, ulRecastDelay, NULL, 0);
	else if (nActionType == AGPMSKILL_INVALID_TARGET_TO_GATHER)
		pvPacketAction = MakeActionPacket((INT8) nActionType, lSkillLevel, pcsTargetBase, NULL, NULL, bForceAttack, ulCastDelay, ulDuration, ulRecastDelay, NULL, 0);
	else if (nActionType == AGPMSKILL_ACTION_TARGET_NUM_ZERO)
		pvPacketAction = MakeActionPacket((INT8) nActionType, lSkillLevel, pcsTargetBase, NULL, NULL, bForceAttack, ulCastDelay, ulDuration, ulRecastDelay, NULL, 0);


	if (!pvPacketAction)
	{
		m_csPacketBase.FreePacket(pvPacketOwnerBase);
		return NULL;
	}

	PVOID	pvPacketFactor = NULL;
	
	INT8	cOperation		= AGPMSKILL_PACKET_OPERATION_CAST_SKILL;
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
												&cOperation,
												plSkillID,
												pvPacketOwnerBase,
												NULL,
												plSkillTID,
												NULL,
												pvPacketAction,
												NULL,
												NULL,
												NULL);

	m_csPacketBase.FreePacket(pvPacketOwnerBase);
	m_csPacketAction.FreePacket(pvPacketAction);
	if (pvPacketFactor)
		m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	return pvPacket;
}

PVOID AgpmSkill::MakeCastResultPacket(INT16 nActionType, INT32 *plSkillID, INT32 *plSkillTID, ApBase *pcsOwnerBase, INT32 lSkillLevel, ApBase *pcsTargetBase, PVOID pvPacketResultFactor, INT16 *pnPacketLength, BOOL bIsFactorNotQueueing, UINT32 ulAdditionalEffect)
{
	if (!pcsOwnerBase || !pnPacketLength)
		return NULL;

	PVOID	pvPacketOwnerBase	= MakeBasePacket(pcsOwnerBase);
	if (!pvPacketOwnerBase)
		return NULL;

	PVOID	pvPacketAction		= MakeActionPacket((INT8) nActionType, lSkillLevel, pcsTargetBase, pvPacketResultFactor, NULL, FALSE, 0, 0, 0, NULL, 0, bIsFactorNotQueueing, ulAdditionalEffect);

	if (!pvPacketAction)
	{
		m_csPacketBase.FreePacket(pvPacketOwnerBase);
		return NULL;
	}

	INT8	cOperation		= AGPMSKILL_PACKET_OPERATION_CAST_SKILL_RESULT;
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
												&cOperation,
												plSkillID,
												pvPacketOwnerBase,
												NULL,
												plSkillTID,
												NULL,
												pvPacketAction,
												NULL,
												NULL,
												NULL);

	m_csPacketBase.FreePacket(pvPacketOwnerBase);
	m_csPacketAction.FreePacket(pvPacketAction);

	return pvPacket;
}

// 2004.11.09. steeple
// Twice Packet Skill 사용시 실제 공격시점에서 불린다.
// 클라이언트에서 불리게 된다. (아직까지는..)
PVOID AgpmSkill::MakePacketRealHit(INT32* plSkillID, ApBase* pcsOwnerBase, INT16* pnPacketLength)
{
	if(!pcsOwnerBase || !pnPacketLength)
		return NULL;

	PVOID pvPacketOwnerBase = MakeBasePacket(pcsOwnerBase);
	if(!pvPacketOwnerBase)
		return NULL;

	INT8 cOperation	= AGPMSKILL_PACKET_OPERATION_REAL_HIT;
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
												&cOperation,
												plSkillID,
												pvPacketOwnerBase,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL);

	m_csPacketBase.FreePacket(pvPacketOwnerBase);

	return pvPacket;
}

// 2005.08.22. steeple
// TargetCID 추가요~
// 2005.07.07. steeple
// 스킬이 사용되는 어떤 시점 시점에 부가적인 Effect 가 필요할 때 불린다.
PVOID AgpmSkill::MakePacketAdditionalEffect(INT32* plEffectID, ApBase* pcsBase, INT32* plTargetCID, INT16* pnPacketLength)
{
	if(!pcsBase || !pnPacketLength)
		return NULL;

	PVOID pvPacketBase = MakeBasePacket(pcsBase);
	if(!pvPacketBase)
		return NULL;

	INT8 cOperation = AGPMSKILL_PACKET_OPERATION_ADDITIONAL_EFFECT;
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
												&cOperation,
												plEffectID,			// SkillID 에다가 보내버린다.
												pvPacketBase,
												NULL,
												plTargetCID,		// SkillTID 에다가 보내버린다.
												NULL,
												NULL,
												NULL,
												NULL,
												NULL);
	m_csPacketBase.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmSkill::MakePacketBuffedList(ApBase *pcsBase, INT32 lBuffedTID, INT32 lCasterTID, BOOL bAdd, UINT32 ulDurationTimeMSec, UINT8 ucChargeLevel, INT16 *pnPacketLength, UINT32 ulExpiredTime)
{
	if (!pcsBase || lBuffedTID == AP_INVALID_SKILLID || !pnPacketLength)
		return NULL;

	PVOID	pvBasePacket = MakeBasePacket(pcsBase);
	if (!pvBasePacket)
		return NULL;

	INT8	cOperation;

//	PVOID	pvPacketBuff[AGPMSKILL_MAX_SKILL_BUFF];
//	ZeroMemory(pvPacketBuff, sizeof(PVOID) * AGPMSKILL_MAX_SKILL_BUFF);

	INT32 lPacketSize = 0;

	// BuffedSkillCombatArg를 위해서 Packet추가
	AgpdSkillAttachData *pcsSkillAttachData = GetAttachSkillData(pcsBase);
	if(pcsSkillAttachData)
	{
		lPacketSize = sizeof(pcsSkillAttachData->m_stBuffedSkillCombatEffectArg);
	}
	
	ApSafeArray<PVOID, AGPMSKILL_MAX_SKILL_BUFF>	pvPacketBuff;
	pvPacketBuff.MemSetAll();

	if (bAdd)
	{
		pvPacketBuff[0]	= m_csPacketBuff.MakePacket(FALSE, NULL, 0,
													&lBuffedTID, 
													&ulDurationTimeMSec,
													&lCasterTID,
													(ucChargeLevel > 0) ? &ucChargeLevel : NULL,
													(ulExpiredTime > 0) ? &ulExpiredTime : NULL,
													pcsSkillAttachData ? &pcsSkillAttachData->m_stBuffedSkillCombatEffectArg : NULL,
													lPacketSize ? &lPacketSize : NULL);

		if (!pvPacketBuff[0])
			return FALSE;

		cOperation = AGPMSKILL_PACKET_OPERATION_ADD_BUFFED_LIST;
	}
	else
	{
		pvPacketBuff[0]	= m_csPacketBuff.MakePacket(FALSE, NULL, 0,
													&lBuffedTID, 
													NULL,
													NULL,
													NULL,
													NULL,
													pcsSkillAttachData ? &pcsSkillAttachData->m_stBuffedSkillCombatEffectArg : NULL,
													lPacketSize ? &lPacketSize : NULL);

		if (!pvPacketBuff[0])
			return FALSE;

		cOperation = AGPMSKILL_PACKET_OPERATION_REMOVE_BUFFED_LIST;
	}

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
												&cOperation,
												NULL,
												pvBasePacket,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												&pvPacketBuff[0],
												NULL);

	m_csPacketBuff.FreePacket(pvPacketBuff[0]);
	m_csPacketBase.FreePacket(pvBasePacket);

	return pvPacket;
}

PVOID AgpmSkill::MakePacketSkillRemove(AgpdSkill *pcsSkill, INT16 *pnPacketLength)
{
	if (!pcsSkill || !pcsSkill->m_pcsBase || !pnPacketLength)
		return NULL;

	PVOID	pvBasePacket	= MakeBasePacket(pcsSkill->m_pcsBase);
	if (!pvBasePacket)
		return NULL;

	INT8	cOperation		= AGPMSKILL_PACKET_OPERATION_REMOVE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
												&cOperation,
												&pcsSkill->m_lID,
												pvBasePacket,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL);

	m_csPacket.FreePacket(pvBasePacket);

	return pvPacket;
}

PVOID AgpmSkill::MakePacketAllBuffedList(ApBase *pcsBase, INT16 *pnPacketLength)
{
	if (!pcsBase || !pnPacketLength)
		return NULL;

	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);

	if (pcsAttachData->m_astBuffSkillList[0].lSkillTID == AP_INVALID_SKILLID)
		return NULL;

	PVOID	pvBasePacket	= MakeBasePacket(pcsBase);
	if (!pvBasePacket)
		return NULL;

	INT8	cOperation		= AGPMSKILL_PACKET_OPERATION_ALL_BUFFED_SKILL;

//	PVOID	pvPacketBuff[AGPMSKILL_MAX_SKILL_BUFF];
//
//	ZeroMemory(pvPacketBuff, sizeof(PVOID) * AGPMSKILL_MAX_SKILL_BUFF);

	ApSafeArray<PVOID, AGPMSKILL_MAX_SKILL_BUFF>		pvPacketBuff;
	pvPacketBuff.MemSetAll();

	int i = 0;
	for (i = 0; i < AGPMSKILL_MAX_SKILL_BUFF; ++i)
	{
		if (pcsAttachData->m_astBuffSkillList[i].lSkillTID == AP_INVALID_SKILLID)
			break;

		UINT8	cChargeLevel	= (UINT8) pcsAttachData->m_astBuffSkillList[i].lChargeLevel;

		pvPacketBuff[i]	= m_csPacketBuff.MakePacket(FALSE, NULL, 0,
													&pcsAttachData->m_astBuffSkillList[i].lSkillTID, 
													NULL,
													&pcsAttachData->m_astBuffSkillList[i].lCasterTID,
													(cChargeLevel > 0) ? &cChargeLevel : NULL,
													NULL, NULL);

		if (!pvPacketBuff[i])
			break;
	}

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
												&cOperation,
												NULL,
												pvBasePacket,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												&pvPacketBuff[0],
												NULL);

	for (i = 0; i < AGPMSKILL_MAX_SKILL_BUFF; ++i)
	{
		if (pvPacketBuff[i])
			m_csPacketBuff.FreePacket(pvPacketBuff[i]);
	}

	m_csPacketBase.FreePacket(pvBasePacket);

	return pvPacket;
}

PVOID AgpmSkill::MakePacketSkill(AgpdSkill *pcsSkill, INT16 *pnPacketLength)
{
	if (!pcsSkill || !pnPacketLength)
		return NULL;

	PVOID	pvBasePacket = MakeBasePacket(pcsSkill->m_pcsBase);
	if (!pvBasePacket)
		return NULL;

	PVOID	pvFactorPacket = m_pagpmFactors->MakePacketFactors(&pcsSkill->m_csFactor);
	if (!pvFactorPacket)
	{
		m_csPacketBase.FreePacket(pvBasePacket);
		return NULL;
	}

	INT32 lPacketSize = 0;

	// BuffedSkillCombatArg를 위해서 Packet추가
	AgpdSkillAttachData *pcsSkillAttachData = GetAttachSkillData(pcsSkill->m_pcsBase);
	if(pcsSkillAttachData)
	{
		lPacketSize = sizeof(pcsSkillAttachData->m_stBuffedSkillCombatEffectArg);
	}

	INT8	cOperation	= AGPMSKILL_PACKET_OPERATION_ADD;
	INT8	cStatus		= pcsSkill->m_eStatus;

	INT8	cMasteryIndex	= (INT8) pcsSkill->m_lMasteryIndex;
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
												&cOperation,
												&pcsSkill->m_lID,
												pvBasePacket,
												pvFactorPacket,
												&pcsSkill->m_pcsTemplate->m_lID,
												&cStatus,
												NULL,
												NULL,
												NULL,
												pcsSkillAttachData ? &pcsSkillAttachData->m_stBuffedSkillCombatEffectArg : NULL,
												lPacketSize ? &lPacketSize : NULL);

	m_csPacketBase.FreePacket(pvBasePacket);
	m_pagpmFactors->m_csPacket.FreePacket(pvFactorPacket);

	return pvPacket;
}

PVOID AgpmSkill::MakePacketSkillDIRT(AgpdSkill *pcsSkill, INT16 *pnPacketLength)
{
	if (!pcsSkill || !pnPacketLength)
		return NULL;

	PVOID	pvFactorPacket = m_pagpmFactors->MakePacketFactorsOneType(&pcsSkill->m_csFactor, AGPD_FACTORS_TYPE_DIRT);
	if (!pvFactorPacket)
		return NULL;

	PVOID	pvPacketBase	= MakeBasePacket(pcsSkill->m_pcsBase);
	if (!pvPacketBase)
	{
		m_pagpmFactors->m_csPacket.FreePacket(pvFactorPacket);
		return NULL;
	}

	INT8	cOperation	= AGPMSKILL_PACKET_OPERATION_UPDATE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
												&cOperation,
												&pcsSkill->m_lID,
												pvPacketBase,
												pvFactorPacket,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL);

	m_csPacketBase.FreePacket(pvPacketBase);
	m_pagpmFactors->m_csPacket.FreePacket(pvFactorPacket);

	return pvPacket;
}

PVOID AgpmSkill::MakePacketUpdateSkillPoint(AgpdSkill *pcsSkill, INT16 *pnPacketLength, INT32 lPoint)
{
	if (!pcsSkill || !pnPacketLength || lPoint == 0)
		return NULL;

	PVOID	pvPacketBase	= MakeBasePacket(pcsSkill->m_pcsBase);

	INT8	cOperation	= AGPMSKILL_PACKET_OPERATION_UPDATE_SKILL_POINT;

	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
													&cOperation,
													&pcsSkill->m_lID,
													pvPacketBase,
													NULL,
													NULL,
													NULL,
													NULL,
													&lPoint,
													NULL,
													NULL);

	if (pvPacketBase)
		m_csPacketBase.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmSkill::MakePacketUpdateDIRTPoint(AgpdSkill *pcsSkill, INT16 *pnPacketLength, INT32 lDuration, INT32 lIntensity, INT32 lRange, INT32 lTarget)
{
	if (!pcsSkill || !pnPacketLength)
		return NULL;

	AgpdFactor	csUpdateFactor;
	m_pagpmFactors->InitFactor(&csUpdateFactor);

	AgpdFactorDIRT	*pcsFactorDIRT = (AgpdFactorDIRT *) m_pagpmFactors->SetFactor(&csUpdateFactor, NULL, AGPD_FACTORS_TYPE_DIRT);
	if (!pcsFactorDIRT)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
		return NULL;
	}

	m_pagpmFactors->SetValue(&csUpdateFactor, lDuration, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_DURATION);
	m_pagpmFactors->SetValue(&csUpdateFactor, lIntensity, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_INTENSITY);
	m_pagpmFactors->SetValue(&csUpdateFactor, lRange, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_RANGE);
	m_pagpmFactors->SetValue(&csUpdateFactor, lTarget, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_TARGET);

	PVOID	pvFactorPacket = m_pagpmFactors->MakePacketFactors(&csUpdateFactor);

	m_pagpmFactors->DestroyFactor(&csUpdateFactor);

	if (!pvFactorPacket)
		return NULL;

	PVOID	pvPacketBase	= MakeBasePacket(pcsSkill->m_pcsBase);

	INT8	cOperation	= AGPMSKILL_PACKET_OPERATION_UPDATE_DIRT_POINT;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
												&cOperation,
												&pcsSkill->m_lID,
												pvPacketBase,
												pvFactorPacket,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL);

	if (pvPacketBase)
		m_csPacketBase.FreePacket(pvPacketBase);

	m_pagpmFactors->m_csPacket.FreePacket(pvFactorPacket);

	return pvPacket;
}

//PVOID AgpmSkill::MakePacketMasteryRollback(ApBase *pcsBase, INT16 *pnPacketLength)
//{
//	if (!pcsBase || !pnPacketLength)
//		return NULL;
//
//	INT8	cOperation	= AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK;
//
//	PVOID	pvPacketBase = MakeBasePacket(pcsBase);
//	if (!pvPacketBase)
//		return NULL;
//
//	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
//											&cOperation,
//											NULL,
//											pvPacketBase,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL);
//
//	m_csPacketBase.FreePacket(pvPacketBase);
//
//	return pvPacket;
//}
//
//PVOID AgpmSkill::MakePacketMasteryRollbackResult(ApBase *pcsBase, INT16 *pnPacketLength, eAgpmSkillPacketOperation eResult)
//{
//	if (!pcsBase || !pnPacketLength)
//		return NULL;
//
//	INT8	cOperation	= eResult;
//
//	PVOID	pvPacketBase = MakeBasePacket(pcsBase);
//	if (!pvPacketBase)
//		return NULL;
//
//	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
//											&cOperation,
//											NULL,
//											pvPacketBase,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL,
//											NULL);
//
//	m_csPacketBase.FreePacket(pvPacketBase);
//
//	return pvPacket;
//}

PVOID AgpmSkill::MakePacketOperation(ApBase *pcsBase, INT8 cOperation, INT16 *pnPacketLength)
{
	if (!pcsBase || !pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= MakeBasePacket(pcsBase);
	if (!pvPacketBase)
		return NULL;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketBase,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketBase.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmSkill::MakePacketRequestRollback(ApBase *pcsBase, INT32 lSkillID, INT16 *pnPacketLength)
{
	if (!pcsBase || !pnPacketLength || lSkillID == AP_INVALID_SKILLID)
		return NULL;

	PVOID	pvPacketBase	= MakeBasePacket(pcsBase);
	if (!pvPacketBase)
		return NULL;

	INT8	cOperation	= AGPMSKILL_PACKET_OPERATION_REQUEST_SKILL_ROLLBACK;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
											&cOperation,
											&lSkillID,
											pvPacketBase,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketBase.FreePacket(pvPacketBase);

	return pvPacket;
}

// 2007.02.06. steeple
PVOID AgpmSkill::MakePacketModifiedSkillLevel(ApBase* pcsBase, INT16* pnPacketLength)
{
	if(!pcsBase || !pnPacketLength)
		return NULL;

	AgpdSkillAttachData* pcsAttachData = GetAttachSkillData(pcsBase);
	if(!pcsAttachData)
		return NULL;
	
	PVOID pvPacketBase = MakeBasePacket(pcsBase);
	if(!pvPacketBase)
		return NULL;

	INT8 cOperation = AGPMSKILL_PACKET_OPERATION_MODIFIED_SKILL_LEVEL;
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
											&cOperation,
											&pcsAttachData->m_lModifiedSkillLevel,		// SkillID 에 넣어서 보낸다.
											pvPacketBase,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	m_csPacketBase.FreePacket(pvPacketBase);

	return pvPacket;
}

// 2007.07.20. steeple
PVOID AgpmSkill::MakePacketInitCoolTime(ApBase* pcsBase, INT16* pnPacketLength, INT32 lSkillTID)
{
	if(!pcsBase || !pnPacketLength)
		return NULL;

	AgpdSkillAttachData* pcsAttachData = GetAttachSkillData(pcsBase);
	if(!pcsAttachData)
		return NULL;
	
	PVOID pvPacketBase = MakeBasePacket(pcsBase);
	if(!pvPacketBase)
		return NULL;

	INT8 cOperation = AGPMSKILL_PACKET_OPERATION_INIT_COOLTIME;
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
											&cOperation,
											&lSkillTID,		// SkillID 에 넣어서 보낸다.
											pvPacketBase,
											NULL,
											&lSkillTID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	m_csPacketBase.FreePacket(pvPacketBase);

	return pvPacket;
}

// 2007.11.14. steeple
PVOID AgpmSkill::MakePacketSpecificSkillLevel(AgpdSkill* pcsSkill, INT16* pnPacketLength)
{
	if(!pcsSkill || !pcsSkill->m_pcsBase || !pnPacketLength)
		return NULL;

	PVOID pvPacketBase = MakeBasePacket(pcsSkill->m_pcsBase);
	if(!pvPacketBase)
		return NULL;

	INT8 cOperation = AGPMSKILL_PACKET_OPERATION_SPECIFIC_SKILL_LEVEL;
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
											&cOperation,
											&pcsSkill->m_lID,
											pvPacketBase,
											NULL,
											&pcsSkill->m_lModifiedLevel,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	m_csPacketBase.FreePacket(pvPacketBase);

	return pvPacket;
}

//PVOID AgpmSkill::MakePacketAddSPToMastery(ApBase *pcsBase, INT8 cMasteryIndex, INT32 *plSelectSkillTID, INT16 nArraySize, INT16 *pnPacketLength)
//{
//	if (!pcsBase ||
//		cMasteryIndex < 0 ||
//		cMasteryIndex >= AGPMSKILL_MAX_MASTERY ||
//		!pnPacketLength)
//		return NULL;
//
//	PVOID	pvPacketBase	= MakeBasePacket(pcsBase);
//	if (!pvPacketBase)
//		return NULL;
//
//	INT8	cOperation		= AGPMSKILL_PACKET_OPERATION_MASTERY_ADD_SP;
//
//	PVOID	pvPacket		= NULL;
//
//	nArraySize	= nArraySize * sizeof(INT32);
//
//	if (plSelectSkillTID)
//	{
//		pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
//													&cOperation,
//													NULL,
//													pvPacketBase,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													&cMasteryIndex,
//													NULL,
//													plSelectSkillTID, &nArraySize,
//													NULL,
//													NULL,
//													NULL,
//													NULL);
//	}
//	else
//	{
//		pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
//													&cOperation,
//													NULL,
//													pvPacketBase,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													&cMasteryIndex,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL);
//	}
//
//	m_csPacketBase.FreePacket(pvPacketBase);
//
//	return pvPacket;
//}
//
//PVOID AgpmSkill::MakePacketAddSPToMasteryResult(ApBase *pcsBase, INT8 cMasteryIndex, INT8 cResult, INT32 *plSelectSkillTID, INT16 nArraySize, INT16 *pnPacketLength)
//{
//	if (!pcsBase ||
//		cMasteryIndex < 0 ||
//		cMasteryIndex >= AGPMSKILL_MAX_MASTERY ||
//		!pnPacketLength)
//		return NULL;
//
//	PVOID	pvPacketBase	= MakeBasePacket(pcsBase);
//	if (!pvPacketBase)
//		return NULL;
//
//	INT8	cOperation		= AGPMSKILL_PACKET_OPERATION_MASTERY_ADD_SP_RESULT;
//
//	PVOID	pvPacket		= NULL;
//
//	nArraySize	= sizeof(INT32) * nArraySize;
//
//	if (plSelectSkillTID)
//	{
//		pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
//													&cOperation,
//													NULL,
//													pvPacketBase,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													&cMasteryIndex,
//													&cResult,
//													plSelectSkillTID, &nArraySize,
//													NULL,
//													NULL,
//													NULL,
//													NULL);
//	}
//	else
//	{
//		pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
//													&cOperation,
//													NULL,
//													pvPacketBase,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													&cMasteryIndex,
//													&cResult,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL);
//	}
//
//	m_csPacketBase.FreePacket(pvPacketBase);
//
//	return pvPacket;
//}

//PVOID AgpmSkill::MakePacketMasteryUpdate(ApBase *pcsBase, INT8 cMasteryIndex, INT16 *pnPacketLength)
//{
//	if (!pcsBase ||
//		cMasteryIndex < 0 ||
//		cMasteryIndex >= AGPMSKILL_MAX_MASTERY ||
//		!pnPacketLength)
//		return NULL;
//
//	PVOID	pvPacketBase	= MakeBasePacket(pcsBase);
//	if (!pvPacketBase)
//		return NULL;
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//	{
//		m_csPacketBase.FreePacket(pvPacketBase);
//
//		return FALSE;
//	}
//
//	PVOID	pvPacketMastery	= m_csPacketMastery.MakePacket(FALSE, NULL, 0,
//													&cMasteryIndex,
//													&pcsAttachData->m_csMastery[cMasteryIndex].m_ucTotalInputSP,
//													NULL,
//													NULL
//													);
//
//	if (!pvPacketMastery)
//	{
//		m_csPacketBase.FreePacket(pvPacketBase);
//
//		return NULL;
//	}
//
//	INT8	cOperation		= AGPMSKILL_PACKET_OPERATION_MASTERY_UPDATE;
//
//	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
//													&cOperation,
//													NULL,
//													pvPacketBase,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													pvPacketMastery,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL);
//
//	m_csPacketMastery.FreePacket(pvPacketMastery);
//	m_csPacketBase.FreePacket(pvPacketBase);
//
//	return pvPacket;
//}
//
//PVOID AgpmSkill::MakePacketMasteryAllUpdate(ApBase *pcsBase, INT16 *pnPacketLength)
//{
//	if (!pcsBase || !pnPacketLength)
//		return NULL;
//
//	PVOID	pvPacketBase	= MakeBasePacket(pcsBase);
//	if (!pvPacketBase)
//		return NULL;
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//	{
//		m_csPacketBase.FreePacket(pvPacketBase);
//
//		return FALSE;
//	}
//
//	UINT8	ucTotalInputSP[AGPMSKILL_MAX_MASTERY];
//	ZeroMemory(ucTotalInputSP, sizeof(UINT8) * AGPMSKILL_MAX_MASTERY);
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		ucTotalInputSP[i]	= pcsAttachData->m_csMastery[i].m_ucTotalInputSP;
//	}
//
//	PVOID	pvPacketMastery	= m_csPacketMastery.MakePacket(FALSE, NULL, 0,
//													NULL,
//													NULL,
//													ucTotalInputSP,
//													NULL
//													);
//
//	if (!pvPacketMastery)
//	{
//		m_csPacketBase.FreePacket(pvPacketBase);
//
//		return NULL;
//	}
//
//	INT8	cOperation		= AGPMSKILL_PACKET_OPERATION_MASTERY_UPDATE;
//
//	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
//													&cOperation,
//													NULL,
//													pvPacketBase,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													pvPacketMastery,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL);
//
//	m_csPacketMastery.FreePacket(pvPacketMastery);
//	m_csPacketBase.FreePacket(pvPacketBase);
//
//	return pvPacket;
//}
//
//PVOID AgpmSkill::MakePacketMasteryNodeUpdate(ApBase *pcsBase, INT8 cMasteryIndex, INT32 *plNodeList, INT32 lNumNode, INT16 *pnPacketLength)
//{
//	if (!pcsBase ||
//		cMasteryIndex < 0 ||
//		cMasteryIndex >= AGPMSKILL_MAX_MASTERY ||
//		!plNodeList ||
//		lNumNode < 1 ||
//		!pnPacketLength)
//		return NULL;
//
//	PVOID	pvPacketBase	= MakeBasePacket(pcsBase);
//	if (!pvPacketBase)
//		return NULL;
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//	{
//		m_csPacketBase.FreePacket(pvPacketBase);
//
//		return FALSE;
//	}
//
//	INT32	lNodeList[20][2];
//	ZeroMemory(lNodeList, sizeof(INT32) * 20 * 2);
//
//	CopyMemory(lNodeList, plNodeList, sizeof(INT32) * 2 * lNumNode);
//
//	PVOID	pvPacketMasteryNode[128];
//	ZeroMemory(pvPacketMasteryNode, sizeof(PVOID) * 128);
//
//	for (int i = 0; i < lNumNode; ++i)
//	{
//		INT8	cSkillTreeIndex	= lNodeList[i][0];
//		INT8	cNodeIndex		= lNodeList[i][1];
//
//		pvPacketMasteryNode[i]	= m_csPacketMasteryNode.MakePacket(FALSE, NULL, 0,
//														&cMasteryIndex,
//														&cSkillTreeIndex,
//														&cNodeIndex,
//														&pcsAttachData->m_csMasteryTemplate.m_csMastery[cMasteryIndex].m_csSkillTree[cSkillTreeIndex][cNodeIndex].m_lSkillID,
//														&pcsAttachData->m_csMasteryTemplate.m_csMastery[cMasteryIndex].m_csSkillTree[cSkillTreeIndex][cNodeIndex].m_ucSPForActive
//														);
//	}
//
//	PVOID	pvPacketMastery	= m_csPacketMastery.MakePacket(FALSE, NULL, 0,
//													&cMasteryIndex,
//													NULL,
//													NULL,
//													pvPacketMasteryNode
//													);
//
//	for (i = 0; i < lNumNode; ++i)
//	{
//		if (pvPacketMasteryNode[i])
//			m_csPacketMasteryNode.FreePacket(pvPacketMasteryNode[i]);
//	}
//
//	if (!pvPacketMastery)
//	{
//		m_csPacketBase.FreePacket(pvPacketBase);
//
//		return NULL;
//	}
//
//	INT8	cOperation		= AGPMSKILL_PACKET_OPERATION_MASTERY_NODE_UPDATE;
//
//	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
//													&cOperation,
//													NULL,
//													pvPacketBase,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													pvPacketMastery,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL);
//
//	m_csPacketMastery.FreePacket(pvPacketMastery);
//	m_csPacketBase.FreePacket(pvPacketBase);
//
//	return pvPacket;
//}
//
//PVOID AgpmSkill::MakePacketMasteryChangedNode(ApBase *pcsBase, INT16 *pnPacketLength)
//{
//	if (!pcsBase ||
//		!pnPacketLength)
//		return NULL;
//
//	PVOID	pvPacketBase	= MakeBasePacket(pcsBase);
//	if (!pvPacketBase)
//		return NULL;
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//	{
//		m_csPacketBase.FreePacket(pvPacketBase);
//
//		return FALSE;
//	}
//
//	PVOID	pvPacketMasteryNode[128];
//	ZeroMemory(pvPacketMasteryNode, sizeof(PVOID) * 128);
//
//	INT32	lPacketIndex	= 0;
//
//	for (INT8 i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		for (INT8 j = 0; j < AGPMSKILL_MAX_NUM_SKILL_NODE; ++j)
//		{
//			for (INT8 k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
//			{
//				if (pcsAttachData->m_csMasteryTemplate.m_csMastery[i].m_csSkillTree[j][k].m_bIsChanged)
//					pvPacketMasteryNode[lPacketIndex]	= m_csPacketMasteryNode.MakePacket(FALSE, NULL, 0,
//																	&i,
//																	&j,
//																	&k,
//																	&pcsAttachData->m_csMasteryTemplate.m_csMastery[i].m_csSkillTree[j][k].m_lSkillID,
//																	&pcsAttachData->m_csMasteryTemplate.m_csMastery[i].m_csSkillTree[j][k].m_ucSPForActive);
//
//				if (pvPacketMasteryNode[lPacketIndex])
//					++lPacketIndex;
//																	
//			}
//		}
//	}
//
//	if (lPacketIndex < 1)
//		return NULL;
//
//	PVOID	pvPacketMastery	= m_csPacketMastery.MakePacket(FALSE, NULL, 0,
//													NULL,
//													NULL,
//													NULL,
//													pvPacketMasteryNode
//													);
//
//	for (i = 0; i < lPacketIndex; ++i)
//	{
//		m_csPacketMasteryNode.FreePacket(pvPacketMasteryNode[i]);
//	}
//
//	if (!pvPacketMastery)
//	{
//		m_csPacketBase.FreePacket(pvPacketBase);
//
//		return NULL;
//	}
//
//	INT8	cOperation		= AGPMSKILL_PACKET_OPERATION_MASTERY_NODE_UPDATE;
//
//	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSKILL_PACKET_TYPE,
//													&cOperation,
//													NULL,
//													pvPacketBase,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													pvPacketMastery,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL,
//													NULL);
//
//	m_csPacketMastery.FreePacket(pvPacketMastery);
//	m_csPacketBase.FreePacket(pvPacketBase);
//
//	return pvPacket;
//}
