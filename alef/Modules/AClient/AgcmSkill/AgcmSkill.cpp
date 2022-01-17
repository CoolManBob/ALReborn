/******************************************************************************
Module:  AgcmSkill.cpp
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 25
******************************************************************************/

#include "AgcmSkill.h"
#include "AgpdProduct.h"
#include "ApMemoryTracker.h"
#include "AgcmUIConsole.h"
#include "ApModuleStream.h"
#include "AgcmUIConsole.h"
#include "AgcmItem.h"
#include "AgcmUICharacter.h"

#include <functional>

AgcmSkill::AgcmSkill()
{
	SetModuleName("AgcmSkill");

	m_lTempSkillID		= 0x7FFFFFFF;

	ZeroMemory(m_lTempCreatedSkillID, sizeof(INT32) * AGCMSKILL_MAX_TEMP_CREATED_SKILL);

	EnableIdle(TRUE);

	m_bIsProcessUpdateMastery	= FALSE;
	m_ulStartProcessUpdateMasteryTime	= 0;
	
	ZeroMemory( &m_posSelectedPos , sizeof( m_posSelectedPos ) );

	m_nIndexADCharacterTemplate	= 0;
	m_nIndexADSkillTemplate		= 0;
	m_nIndexADSpecialize		= 0;
	m_nIndexADCharacter			= 0;
	m_nIndexADSkill				= 0;

	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmSkill			= NULL;
	m_pcsAgpmGrid			= NULL;
	m_pcsAgpmPvP			= NULL;
	m_pcsApmEventManager	= NULL;
	m_pcsAgcmCharacter		= NULL;
	m_pcsAgpmFactors		= NULL;
	m_pcsAgcmUIControl		= NULL;
	
	m_ulNextSendAddSPTime	= 0;

	m_szIconTexturePath[0] = 0;

	m_bFollowCastLock		= FALSE;
	m_lFollowCastTargetID	= AP_INVALID_CID;
	m_lFollowCastSkillID	= AP_INVALID_SKILLID;

	m_lGroundTargetSkillID	= 0;

	m_bDbgSkill				= FALSE;

	m_bShowAffectedTargetCount = FALSE;
}

AgcmSkill::~AgcmSkill()
{
}

BOOL AgcmSkill::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmSkill			= (AgpmSkill *)			GetModule("AgpmSkill");
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pcsAgpmGrid			= (AgpmGrid *)			GetModule("AgpmGrid");
	m_pcsAgpmPvP			= (AgpmPvP *)			GetModule("AgpmPvP");
	
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgpmFactors		= (AgpmFactors * )		GetModule("AgpmFactors");

	m_pcsAgcmUIControl		= (AgcmUIControl *)		GetModule("AgcmUIControl");

	m_pcsAgcmResourceLoader	= (AgcmResourceLoader *)GetModule("AgcmResourceLoader");

	m_pcsAgpmOptimizedPacket2	= (AgpmOptimizedPacket2 *)	GetModule("AgpmOptimizedPacket2");

	if (!m_pcsAgpmFactors ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmSkill ||
		!m_pcsAgpmGrid ||
		!m_pcsAgpmPvP ||
		!m_pcsAgcmCharacter || 
		!m_pcsApmEventManager || 
		!m_pcsAgcmUIControl)
		return FALSE;

	if (!m_pcsAgpmSkill->SetCallbackReceiveAction(CBReceiveAction, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackReceiveActionResult(CBActionSkillResult, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackCheckNowUpdateActionFactor(CBCheckNowUpdateActionFactor, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionSkill(CBActionSkill, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackActionProductSkill(CBActionProductSkill, this))
		return FALSE;

	m_nIndexADCharacter	= m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgcdSkillADChar), ConAgcdSkillAttachData, DesAgcdSkillAttachData);
	if (m_nIndexADCharacter < 0)
		return FALSE;

	m_nIndexADCharacterTemplate = m_pcsAgpmCharacter->AttachCharacterTemplateData(this, sizeof(AgcdSkillAttachTemplateData), ConTemplateAttachData, DesTemplateAttachData);
	if (m_nIndexADCharacterTemplate < 0)
		return FALSE;

	if (!AddStreamCallback( AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL, AttachedTemplateReadCB, AttachedTemplateWriteCB, this) )
		return FALSE;

	if (!AddStreamCallback( AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL_SOUND, AttachedSoundTemplateReadCB, AttachedSoundTemplateWriteCB, this) )
		return FALSE;

	m_nIndexADSkillTemplate = m_pcsAgpmSkill->AttachSkillTemplateData(this, sizeof(AgcdSkillTemplate), ConAgcdSkillTemplate, DesAgcdSkillTemplate);
	if (m_nIndexADSkillTemplate < 0)
		return FALSE;

	m_nIndexADSpecialize	= m_pcsAgpmSkill->AttachSpecializeData(this, sizeof(AgcdSkillSpecializeTemplate), ConAgcdSkillSpecialize, DesAgcdSkillSpecialize);
	if (m_nIndexADSpecialize < 0)
		return FALSE;

	m_nIndexADSkill			= m_pcsAgpmSkill->AttachSkillData(this, sizeof(AgcdSkill), ConAgcdSkill, DesAgcdSkill);
	if (m_nIndexADSkill < 0)
		return FALSE;

	if (!m_pcsAgpmSkill->AddStreamCallback(AGPMSKILL_DATA_TYPE_TEMPLATE, AgcdSkillTemplateReadCB, AgcdSkillTemplateWriteCB, this))
		return FALSE;
	if (!m_pcsAgpmSkill->AddStreamCallback(AGPMSKILL_DATA_TYPE_SPECIALIZE, AgcdSkillSpecializeReadCB, AgcdSkillSpecializeWriteCB, this))
		return FALSE;

//#ifndef	__NEW_MASTERY__
//	if (!m_pcsAgpmSkill->SetCallbackMasterySpecialize(CBMasterySpecialize, this))
//		return FALSE;
//#endif	//__NEW_MASTERY__
//	if (!m_pcsAgpmSkill->SetCallbackMasterySpecializeResult(CBMasterySpecializeResult, this))
//		return FALSE;
//#ifndef	__NEW_MASTERY__
//	if (!m_pcsAgpmSkill->SetCallbackMasteryChangeResult(CBMasteryChangeResult, this))
//		return FALSE;
//#endif	//__NEW_MASTERY__
	if (!m_pcsAgpmSkill->SetCallbackLearnSkill(CBLearnSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackForgetSkill(CBForgetSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackInitSkill(CBInitSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackRemoveSkill(CBRemoveSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackInitSkillTemplate(CBInitSkillTemplate, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackInitSpecialize(CBInitSpecialize, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackUpdateSkill(CBUpdateSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackUpdateSkillPoint(CBUpdateSkillPoint, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackAddBuffedList(CBAddBuffedSkillList, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackRemoveBuffedList(CBRemoveBuffedSkillList, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackReceiveAdditionalEffect(CBAdditionalEffect, this))
		return FALSE;


//	if (!m_pcsAgpmSkill->SetCallbackMasteryRollbackResult(CBMasteryRollbackResult, this))
//		return FALSE;
//
//#ifdef	__NEW_MASTERY__
//	if (!m_pcsAgpmSkill->SetCallbackAddSPResult(CBAddSPToMasteryResult, this))
//		return FALSE;
//	if (!m_pcsAgpmSkill->SetCallbackMasteryNodeUpdate(CBMasteryNodeUpdate, this))
//		return FALSE;
//#endif	//__NEW_MASTERY__

	if (!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSkillStruckCharacter(CBSkillStruckAnimation, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackProcessSkillStruckCharacter(CBProcessSkillStruck, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackActionSkill(CBReservedActionSkill, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackLoadTemplate(CBLoadCharacterTemplate, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackReleaseTemplate(CBReleaseCharacterTemplate, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSelfStopCharacter(CBStopSelfCharacter, this))
		return FALSE;

	if (m_pcsAgpmOptimizedPacket2)
	{
		if (!m_pcsAgpmOptimizedPacket2->SetCallbackMoveActionSkill(CBMoveActionSkill, this))
			return FALSE;
		if (!m_pcsAgpmOptimizedPacket2->SetCallbackMoveActionRelease(CBMoveActionRelease, this))
			return FALSE;
	}

	// Seong Yon - jun 
	
	return TRUE;
}


//@{ kday 20050823
// ;)
#include "ApmMap.h"
void GetMonsterList_Ready(const ApWorldSector* pSector, const AuMATRIX* pMat, const AuBOX* pBox, const AuPOS* pPos );
void GetMonsterList_push_Box(const AuPOS& suf, const AuPOS& inf);
void GetMonsterList_push_Pos(const AuPOS& pos);
//@} kday

BOOL AgcmSkill::OnInit()
{
	
	//@{ kday 20050822
	// ;)
	AS_REGISTER_TYPE_BEGIN(AgcmSkill, AgcmSkill);
	AS_REGISTER_METHOD0(void, DebugSkill_On);
	AS_REGISTER_METHOD0(void, DebugSkill_Off);

	AS_REGISTER_METHOD0(void, ToggleAffectedTargetCount);
	AS_REGISTER_TYPE_END;
	//@} kday

	//@{ kday 20050823
	// ;)
	ApmMap*	pApmMap = static_cast<ApmMap*>( GetModule("ApmMap") );
	if( pApmMap )
	{
		pApmMap->SetSkillDbgCB( GetMonsterList_Ready, GetMonsterList_push_Box, GetMonsterList_push_Pos );
	}
	//@} kday

	m_pcsAgpmCharacter->SetCallbackUpdateSpecialStatus( CBSkillStatusUpdate , this );


	return TRUE;
}

BOOL AgcmSkill::OnDestroy()
{
//	m_csEffectDataAdmin.RemoveAll();

	return TRUE;
}

AgcdSkillTemplate*	AgcmSkill::GetADSkillTemplate(PVOID pvData)
{
	return (AgcdSkillTemplate *) m_pcsAgpmSkill->GetAttachedModuleData(m_nIndexADSkillTemplate, pvData);
}

AgcdSkillSpecializeTemplate* AgcmSkill::GetADSpecializeTemplate(PVOID pvData)
{
	return (AgcdSkillSpecializeTemplate *) m_pcsAgpmSkill->GetAttachedModuleData(m_nIndexADSpecialize, pvData);
}

BOOL AgcmSkill::SetSelectedBase(INT32 lType, INT32 lID, AuPOS *pcsPos)
{
	if (!pcsPos)
		return FALSE;

	m_csSelectedBase.m_eType	= (ApBaseType) lType;
	m_csSelectedBase.m_lID		= lID;

	CopyMemory(&m_posSelectedPos, pcsPos, sizeof(AuPOS));

	return TRUE;
}

BOOL AgcmSkill::SetSelectedPos(AuPOS *pcsPos)
{
	if (!pcsPos)
		return FALSE;

	m_csSelectedBase.m_eType	= APBASE_TYPE_NONE;
	m_csSelectedBase.m_lID		= 0;

	CopyMemory(&m_posSelectedPos, pcsPos, sizeof(AuPOS));

	return TRUE;
}

BOOL AgcmSkill::CBReceiveAction(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill					*pThis									= (AgcmSkill *)				pClass;
	pstAgpmSkillActionData		pstActionData							= (pstAgpmSkillActionData)	pData;

	// 자기 자신인 경우 UI 상에서 표시해주기 위해 스킬 액션에 대한 결과를 콜백으로 넘겨준다.
	AgpdCharacter				*pcsSelfCharacter						= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (pcsSelfCharacter &&
		pcsSelfCharacter->m_eType == pstActionData->pcsOwnerBase->m_eType &&
		pcsSelfCharacter->m_lID == pstActionData->pcsOwnerBase->m_lID)
	{
		pThis->EnumCallback(AGCMSKILL_CB_ID_CHECK_CAST_SKILL_RESULT, (PVOID) pstActionData->nActionType, NULL);
	}

	switch (pstActionData->nActionType) {
	case AGPMSKILL_ACTION_START_CAST_SKILL:
		break;

	case AGPMSKILL_ACTION_CAST_SKILL:
		{
			return pThis->ProcessReceiveCastSkill(pstActionData);
		}
		break;

	case AGPMSKILL_ACTION_END_CAST_SKILL:
		break;

	case AGPMSKILL_ACTION_CANCEL_CAST_SKILL:

		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);

		pThis->EnumCallback(AGCMSKILL_CB_ID_MISS_CAST_SKILL,
							pstActionData->pcsOwnerBase,
							pThis->m_pcsApmEventManager->GetBase(pstActionData->csTargetBase[0].m_eType, pstActionData->csTargetBase[0].m_lID));

		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		
		break;

	case AGPMSKILL_ACTION_MISS_CAST_SKILL:

		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);

		pThis->EnumCallback(AGCMSKILL_CB_ID_MISS_CAST_SKILL,
							pstActionData->pcsOwnerBase,
							pThis->m_pcsApmEventManager->GetBase(pstActionData->csTargetBase[0].m_eType, pstActionData->csTargetBase[0].m_lID));

		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);

		break;

	case AGPMSKILL_ACTION_FAILED_CAST:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		break;

	case AGPMSKILL_ACTION_INVALID_TARGET:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		break;

	case AGPMSKILL_ACTION_NEED_REQUIREMENT:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		break;

	case AGPMSKILL_ACTION_NOT_ENOUGH_HP:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		break;

	case AGPMSKILL_ACTION_NOT_ENOUGH_MP:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		break;

	case AGPMSKILL_ACTION_NOT_ENOUGH_SP:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		break;

	case AGPMSKILL_ACTION_NOT_ENOUGH_ARROW:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		break;

	case AGPMSKILL_ACTION_ALREADY_USE_SKILL:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		break;

	case AGPMSKILL_ACTION_NOT_READY_CAST:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		break;

		// 2005.09.21. steeple
		// 타겟이 없다. 이럴 땐 Action Block 을 해제해준다.
	case AGPMSKILL_ACTION_TARGET_NUM_ZERO:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		break;

	default:
		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		pThis->m_pcsAgcmCharacter->ResetCastSkill(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter);
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL AgcmSkill::ProcessReceiveCastSkill(pstAgpmSkillActionData pstActionData)
{
	if (!pstActionData)
		return FALSE;

	// Animation 시켜준다.(041103, Bob)
	// 주인공이 없으면 안돼~~~
	if(!pstActionData->pcsOwnerBase)
	{
		m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		return FALSE;
	}

	AgpdCharacter				*pcsOwner						= (AgpdCharacter *)(pstActionData->pcsOwnerBase);
	AgpdCharacterTemplate		*pcsOwnerTemplate				= (AgpdCharacterTemplate *)(pcsOwner->m_pcsCharacterTemplate);
	if(!pcsOwnerTemplate)
	{
		m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		return FALSE;
	}

	AgpdSkill					*pcsSkill						= m_pcsAgpmSkill->GetSkill(pstActionData->lSkillID);
	if (pcsSkill &&
		((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
	{
		AgpdCharacter	*pcsSourceChar	= m_pcsAgpmCharacter->GetCharacter(pstActionData->csTargetBase[0].m_lID);
		if (pcsSourceChar)
		{
			AuPOS *pBasePos		= m_pcsApmEventManager->GetBasePos(pcsSkill->m_pcsBase, NULL);

			AuPOS *pSourcePos	= m_pcsApmEventManager->GetBasePos((ApBase *) pcsSourceChar, NULL);

			AuPOS pCalcPos;

			if (!m_pcsAgpmFactors->IsInRange(pBasePos, pSourcePos, AGPMPRODUCT_MAX_RANGE, 0, &pCalcPos))
			{
				m_pcsAgpmCharacter->MoveCharacter((AgpdCharacter *) pcsSkill->m_pcsBase, &pCalcPos, MD_NODIRECTION , FALSE, TRUE);

				m_stQueuedSkillActionData	= *pstActionData;

				pcsOwner->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_PRODUCT_SKILL;
				pcsOwner->m_stNextAction.m_lUserData[0]	= pstActionData->lSkillID;

				return TRUE;
			}
		}
	}

	// 타겟을 향해 턴한다.
	if (pstActionData->pcsOwnerBase->m_eType == APBASE_TYPE_CHARACTER)
	{
		m_pcsAgpmCharacter->StopCharacter(pcsOwner, NULL);

		if (pcsOwner->m_lID != pstActionData->csTargetBase[0].m_lID)
			m_pcsAgpmCharacter->TurnCharacter(pcsOwner, 0, m_pcsAgpmCharacter->GetSelfCharacterTurnAngle(pcsOwner, &pstActionData->csTargetPos));
	}

	// 일대일 대칭이므로 걍 가져온당...
	AgcdSkillAttachTemplateData	*pstAgcdSkillAttachTemplateData	= GetAttachTemplateData((ApBase *)(pcsOwnerTemplate));
	if(!pstAgcdSkillAttachTemplateData)
	{
		m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		return FALSE;
	}

	INT32						lUsableIndex					= GetUsableSkillTNameIndex((ApBase *)(pcsOwner->m_pcsCharacterTemplate), pstActionData->lSkillTID);
	if(lUsableIndex < 0)
	{
		m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		return FALSE;
	}

	if (pcsSkill)
	{
		pcsSkill->m_ulStartTime		= m_pcsAgcmCharacter->m_ulCurTick;
		pcsSkill->m_ulEndTime		= pcsSkill->m_ulStartTime + pstActionData->ulDuration;
		pcsSkill->m_ulRecastDelay	= /*pcsSkill->m_ulEndTime*/ m_pcsAgcmCharacter->m_ulCurTick + pstActionData->ulRecastDelay;
	}

	AgcdCharacter				*pcsAgcdOwner					= m_pcsAgcmCharacter->GetCharacterData(pcsOwner);
	if (!pcsAgcdOwner)
	{
		m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		return FALSE;
	}

	pcsAgcdOwner->m_pstSkillActionData = pstActionData;

	AgcdSkillADChar				*pcsAgcdSkillADChar				= GetCharacterAttachData(pstActionData->pcsOwnerBase);
	if (!pcsAgcdSkillADChar)
	{
		m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
		return FALSE;
	}

	pcsAgcdSkillADChar->m_lSkillUsableIndex	= lUsableIndex;
	pcsAgcdSkillADChar->m_lSkillID			= pstActionData->lSkillID;

	// 2005.08.24. steeple
	// SkillTID 로 변경했다. (기존에는 SkillID 였음)
	// 2005.06.01. steeple
	// AgcdCharacter 에다가도 넣어준다.
	pcsAgcdOwner->m_lLastCastSkillTID	= pstActionData->lSkillTID;

	UINT32	ulCurrentClock	= GetClockCount();

	if (pcsSkill && m_pcsAgpmSkill->IsAttackSkill((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate))
		m_pcsAgpmCharacter->SetCombatMode((AgpdCharacter *) pcsOwner, ulCurrentClock);

	m_pcsAgcmCharacter->SetAttackStatus(m_pcsAgcmCharacter->GetCharacterData((AgpdCharacter *) pcsOwner));

	if (!pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo)
	{
		ASSERT(!"!pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo");
		return FALSE;
	}

	FLOAT	fSkillAnimationDuration	= 0.0f;

	if( !pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lUsableIndex] ) return FALSE;

	if (pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lUsableIndex]->m_pacsAnimation[pcsAgcdOwner->m_lCurAnimType2])
	{
		if (m_pcsAgcmCharacter->GetSelfCID() == pcsOwner->m_lID)
			//fSkillAnimationDuration	= pThis->m_pcsAgcmCharacter->GetAnimDuration(pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[pcsAgcdOwner->m_lAnimHandEquipType]);
			fSkillAnimationDuration	=
				m_pcsAgcmCharacter->GetAnimDuration(
					pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lUsableIndex]->m_pacsAnimation[pcsAgcdOwner->m_lCurAnimType2]->m_pcsAnimation	);
	}
	//else if(pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[AGCDITEM_EQUIP_ANIMATION_TYPE_HANDATTACK])
	else if (pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lUsableIndex]->m_pacsAnimation[AGCMCHAR_AT2_COMMON_DEFAULT])
	{
		if (m_pcsAgcmCharacter->GetSelfCID() == pcsOwner->m_lID)
			//fSkillAnimationDuration	= pThis->m_pcsAgcmCharacter->GetAnimDuration(pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[AGCDITEM_EQUIP_ANIMATION_TYPE_HANDATTACK]);
			fSkillAnimationDuration	=
				m_pcsAgcmCharacter->GetAnimDuration(
					pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lUsableIndex]->m_pacsAnimation[AGCMCHAR_AT2_COMMON_DEFAULT]->m_pcsAnimation	);
	}

	AgcdSkill* pcsAgcdSkill = NULL;
	if(pcsSkill)
		pcsAgcdSkill = GetAgcdSkill(pcsSkill);


	// 2005.12.13. steeple
	// Animation Duration 을 조정해준다.
	FLOAT fAdjustCastDelay = fSkillAnimationDuration * (FLOAT)m_pcsAgpmSkill->GetAdjustCastDelay(pcsOwner) / 100.0f;
	fSkillAnimationDuration += fAdjustCastDelay;

	INT32	lNumTargetChar	= 0;
	ApBase	*pcsTargetBase;
//			memset(pcsAgcdOwner->m_alSkillTargetCID, 0, sizeof(INT32) * AGCD_CHARACTER_SKILL_MAX_TARGET);
	memset(pcsOwner->m_alTargetCID, 0, sizeof (INT32) * AGPDCHARACTER_MAX_TARGET);
	for(INT16 nSkillTarget = 0; nSkillTarget < AGPDCHARACTER_MAX_TARGET; ++nSkillTarget)
	{
		pcsTargetBase	= m_pcsApmEventManager->GetBase(pstActionData->csTargetBase[nSkillTarget].m_eType, pstActionData->csTargetBase[nSkillTarget].m_lID);
		if (!pcsTargetBase)
			break;

		if (fSkillAnimationDuration > 0)
		{
			EnumCallback(AGCMSKILL_CB_ID_ADD_HPBAR,
					(PVOID)pcsTargetBase,(PVOID)((INT32) (fSkillAnimationDuration * 1000)));
			//pThis->m_pcsAgcmTextBoard->EnableHpBar(pcsTargetBase,TRUE,(INT32) (fSkillAnimationDuration * 1000));
		}
		else
		{
			EnumCallback(AGCMSKILL_CB_ID_ADD_HPBAR,
					(PVOID)pcsTargetBase,(PVOID)1000);
			//pThis->m_pcsAgcmTextBoard->EnableHpBar(pcsTargetBase,TRUE,1000);
		}

		if (pcsSkill && m_pcsAgpmSkill->IsAttackSkill((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate))
			m_pcsAgpmCharacter->SetCombatMode((AgpdCharacter *) pcsTargetBase, ulCurrentClock);

		m_pcsAgcmCharacter->SetAttackStatus(m_pcsAgcmCharacter->GetCharacterData((AgpdCharacter *) pcsTargetBase));

		if (pcsTargetBase->m_eType == APBASE_TYPE_CHARACTER)
			pcsOwner->m_alTargetCID[lNumTargetChar] = pcsTargetBase->m_lID;
		else
			break;

//				if(!pcsAgcdOwner->m_alSkillTargetCID[lNumTargetChar++])
		if (!pcsOwner->m_alTargetCID[lNumTargetChar])
			break;

		// 2005.09.22. steeple
		// AgcdSkill 에 타겟을 저장한다.
		if(pcsAgcdSkill)
			pcsAgcdSkill->m_alAffectedTargetCID[lNumTargetChar] = pstActionData->csTargetBase[nSkillTarget].m_lID;

		lNumTargetChar++;
	}

	pcsAgcdOwner->m_bForceAnimation	= TRUE;

	//if(pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[pcsAgcdOwner->m_lAnimHandEquipType])
	if (pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lUsableIndex]->m_pacsAnimation[pcsAgcdOwner->m_lCurAnimType2])
	{
		// 스킬애니메이션 발동~~~
		m_pcsAgcmCharacter->StartAnimation(
			(AgpdCharacter *)(pstActionData->pcsOwnerBase),
			AGCMCHAR_ANIM_TYPE_SKILL,
			//pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[pcsAgcdOwner->m_lAnimHandEquipType]	);
			pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lUsableIndex]->m_pacsAnimation[pcsAgcdOwner->m_lCurAnimType2]	);
	}
	//else if(pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[AGCDITEM_EQUIP_ANIMATION_TYPE_HANDATTACK])
	else if (pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lUsableIndex]->m_pacsAnimation[AGCMCHAR_AT2_COMMON_DEFAULT])
	{
		// 스킬애니메이션 발동~~~
		m_pcsAgcmCharacter->StartAnimation(
			(AgpdCharacter *)(pstActionData->pcsOwnerBase),
			AGCMCHAR_ANIM_TYPE_SKILL,
			//pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[AGCDITEM_EQUIP_ANIMATION_TYPE_HANDATTACK]	);
			pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[lUsableIndex]->m_pacsAnimation[AGCMCHAR_AT2_COMMON_DEFAULT]	);
	}
	else
	{
		EnumCallbackNoAnimSkill((PVOID)(pstActionData->pcsOwnerBase), (PVOID) pstActionData);
	}

	pcsAgcdOwner->m_bForceAnimation	= FALSE;

	if (pcsOwner == m_pcsAgcmCharacter->m_pcsSelfCharacter && fSkillAnimationDuration > 0.0f)
	{
		// fSkillAnimationDuration 은 초 단위이다.

		// 2005.06.16. steeple
		// Action Block Time. 애니메이션 체크만 (맨 처음 것과 같음) 하는 걸로 돌림.
		UINT32 ulActionBlockTime = (UINT32) (fSkillAnimationDuration * 1000);
		m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, ulActionBlockTime, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);

		// 2004.11.09. steeple
		// 패킷을 두번 보내야 하는 스킬이라면 fSkillAnimationDuration 만큼의 Interval 후에 더 보내게 한다.
		if(pcsSkill && pcsSkill->m_pcsTemplate && ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_bTwicePacket)
		{
			// 2005.05.23. steeple. 발사체가 들어갔기에 다시 이거 들어감.
			// 2005.01.16. steeple. Interval 추가
			FLOAT fTwicePacketInterval = GetTwicePacketInteval(pcsSkill, pcsOwner, fSkillAnimationDuration);
			AddIdleEventTwicePacketSkill(pcsOwner, pcsSkill, ulCurrentClock + (UINT32)(fTwicePacketInterval));

			//ulActionBlockTime += (UINT32)(fTwicePacketInterval / 2.0f);
		}

		// 2005.06.02. steeple
		// TwicePacket 보내는 시간도 ActionBlock 타임에 넣는다.
		//m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, ulActionBlockTime, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);

		// 2005.01.16. steeple. 태형아저씨의 요청으로 인해서 추가.
		INT8 cSkillType = 1;
		if(pcsSkill && !m_pcsAgpmSkill->IsAttackSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			cSkillType = 0;
		EnumCallback(AGCMSKILL_CB_ID_START_CAST_SELF_CHARACTER, &fSkillAnimationDuration, &cSkillType);
	}
	else
	{
		m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
	}

	m_pcsAgcmCharacter->ResetCastSkill(m_pcsAgcmCharacter->m_pcsSelfCharacter);

	/*
	if (pcsOwner == pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter)
	{
		INT32	lSkillBlockTime	= pThis->m_pcsAgpmSkill->GetCastDelay(pcsSkill);

		pThis->m_pcsAgpmCharacter->SetActionBlockTime(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, lSkillBlockTime, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
	}
	*/

	return TRUE;
}

BOOL AgcmSkill::CBActionSkillResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill				*pThis				= (AgcmSkill *)				pClass;
	pstAgpmSkillActionData	pstActionData		= (pstAgpmSkillActionData)	pData;

	// 자기 자신인 경우 UI 상에서 표시해주기 위해 스킬 액션에 대한 결과를 콜백으로 넘겨준다.
	AgpdCharacter				*pcsSelfCharacter						= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (pcsSelfCharacter &&
		pcsSelfCharacter->m_eType == pstActionData->pcsOwnerBase->m_eType &&
		pcsSelfCharacter->m_lID == pstActionData->pcsOwnerBase->m_lID)
	{
		pThis->EnumCallback(AGCMSKILL_CB_ID_CHECK_CAST_SKILL_RESULT, (PVOID) pstActionData->nActionType, NULL);
	}

	switch (pstActionData->nActionType) {
	case AGPMSKILL_ACTION_CAST_SKILL_RESULT:
		{
			// 여기서 받은 결과를 보여줘야 한다.
			// ActionQueue에 집어넣는다.
			//

			AgcmCharacterActionQueueData		stActionData;
			ZeroMemory(&stActionData, sizeof(stActionData));

			stActionData.eActionType			= AGPDCHAR_ACTION_TYPE_SKILL;
			//stActionData.eActionResultType		= (AgpdCharacterActionResultType) pstActionData->nActionType;
			stActionData.eActionResultType		= (AgpdCharacterActionResultType) AGPMSKILL_ACTION_CAST_SKILL_RESULT;
			stActionData.lActorID				= pstActionData->pcsOwnerBase->m_lID;
			stActionData.lSkillTID				= pstActionData->lSkillTID;
			stActionData.ulProcessTime			= pThis->m_pcsAgcmCharacter->m_ulCurTick + pThis->GetMaxSkillPreserveTime(pstActionData->lSkillTID, pstActionData->lSkillLevel);
			stActionData.bDeath					= FALSE;
			stActionData.bIsNowUpdate			= pstActionData->bIsNowUpdate;
			stActionData.ulAdditionalEffect		= pstActionData->ulAdditionalEffect;

			if (pstActionData->pvPacketFactor)
			{
				stActionData.bIsSetPacketFactor	= TRUE;
				//CopyMemory(stActionData.szPacketFactor, pstActionData->pvPacketFactor, sizeof(CHAR) * (*((UINT16 *) pstActionData->pvPacketFactor) + sizeof(UINT16)));
				// 2004.11.30. steeple
				UINT16 nPacketSize = (*((UINT16 *) pstActionData->pvPacketFactor) + sizeof(UINT16));
				stActionData.pszPacketFactor = new CHAR[nPacketSize];
				memcpy(stActionData.pszPacketFactor, pstActionData->pvPacketFactor, nPacketSize);
				//TRACEFILE2("steeple_memory.log", "AgcmSkill.cpp::CBActionSkillResult, pszPacketFactor(0x%08x) new", stActionData.pszPacketFactor);
			}

			ApBase	*pcsTargetBase	= NULL;

			if (pstActionData->csTargetBase[0].m_lID != 0)
				pcsTargetBase = pThis->m_pcsApmEventManager->GetBase(pstActionData->csTargetBase[0].m_eType, pstActionData->csTargetBase[0].m_lID);
			else
				pcsTargetBase = pstActionData->pcsOwnerBase;

			if (pThis->m_pcsAgpmFactors && pstActionData->pcsResultFactor)
				pThis->m_pcsAgpmFactors->CopyFactor(&stActionData.csFactorDamage, pstActionData->pcsResultFactor, TRUE, FALSE);

			// 2005.08.24. steeple
			// 몬스터 일때라면 Cast 없이 바로 오기도 하니깐, 여기서 마지막에 무슨 스킬을 사용했는 지 체크해주어야 한다.
			AgpdCharacter* pcsAttacker = pThis->m_pcsAgpmCharacter->GetCharacter(pstActionData->pcsOwnerBase->m_lID);
			if(pcsAttacker && pThis->m_pcsAgpmCharacter->IsMonster(pcsAttacker))
			{
				AgcdCharacter* pcsAgcdAttacker = pThis->m_pcsAgcmCharacter->GetCharacterData(pcsAttacker);
				if(pcsAgcdAttacker)
					pcsAgcdAttacker->m_lLastCastSkillTID = pstActionData->lSkillTID;
			}

			// 2005.09.20. steeple
			// 거리, 속도에 상관없이 지정된 시간후에 시전한 스킬이라면 Not Queueing 이다.
			AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(pstActionData->lSkillTID);
			if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET] &
				AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET_BY_STATIC_TIME)
			{
				pstActionData->bIsFactorNotQueueing = TRUE;
			}

			BOOL	bApplyActionData	= FALSE;
			if (pstActionData->bIsFactorNotQueueing)
			{
				if (pcsTargetBase && pcsTargetBase->m_eType == APBASE_TYPE_CHARACTER)
					pThis->m_pcsAgcmCharacter->ApplyActionData((AgpdCharacter *) pcsTargetBase, &stActionData);
			}
			else
			{
				if (pThis->IsExistCastQueue(pcsTargetBase, stActionData.lActorID))
				{
					pThis->m_pcsAgcmCharacter->ApplyActionData((AgpdCharacter *) pcsTargetBase, &stActionData);
					bApplyActionData	= TRUE;

					pThis->RemoveCastQueue(pcsTargetBase, stActionData.lActorID);
				}

				if (!bApplyActionData)
				{
					if (pThis->m_pcsAgpmFactors)
					{
						INT32	lNumDivideForShow			= pThis->m_pcsAgpmSkill->GetDivideDMGNumForShow(pstActionData->lSkillTID, pstActionData->lSkillLevel);

						if (lNumDivideForShow > 1 && pThis->m_pcsAgpmFactors)
						{
							// 뎀쥐를 나눠서 표시해줘야 하는데.. 음냐.. 이걸 오째야 하나..
							// factor를 나눠 보여줘야 하는만큼 나눈다.
							// 나눠서 보여줘야 하는 인터벌을 세팅한다. (AgcmCharacter에서 ActionData를 처리할때 이 인터벌을 사용해 타이머 이벤트에 등록한다.)

							if (pThis->m_pcsAgpmFactors->DivideFactor(&stActionData.csFactorDamage, lNumDivideForShow))
							{
								stActionData.lNumDivideForShow		= lNumDivideForShow;
								stActionData.ulDivideIntervalMSec	= AGCDSKILL_SHOW_DAMAGE_INTERVAL;
							}
						}
					}

					pThis->m_pcsAgcmCharacter->AddReceivedAction((AgpdCharacter *) pcsTargetBase, &stActionData);
				}
			}

			// 2004.11.30. steeple. Queue 에 넣지 않고 이미 다 처리해버린 상태라면
			if(pstActionData->bIsFactorNotQueueing || bApplyActionData)
			{
				if(stActionData.bIsSetPacketFactor && stActionData.pszPacketFactor)
				{
					//TRACEFILE2("steeple_memory.log", "AgcmSkill.cpp::CBActionSkillResult, pszPacketFactor(0x%08x) delete", stActionData.pszPacketFactor);
					delete stActionData.pszPacketFactor;
					stActionData.pszPacketFactor = NULL;
				}
			}
		}
		break;

	case AGPMSKILL_ACTION_MISS_CAST_SKILL:
		{
			AgcmCharacterActionQueueData		stActionData;
			ZeroMemory(&stActionData, sizeof(stActionData));

			stActionData.eActionType			= AGPDCHAR_ACTION_TYPE_SKILL;
			//stActionData.eActionResultType		= (AgpdCharacterActionResultType) pstActionData->nActionType;
			stActionData.eActionResultType		= (AgpdCharacterActionResultType) AGPMSKILL_ACTION_MISS_CAST_SKILL;
			stActionData.lActorID				= pstActionData->pcsOwnerBase->m_lID;
			stActionData.ulProcessTime			= pThis->m_pcsAgcmCharacter->m_ulCurTick + pThis->GetMaxSkillPreserveTime(pstActionData->lSkillTID, pstActionData->lSkillLevel);
			stActionData.bDeath					= FALSE;

			if (pThis->m_pcsAgpmFactors)
				pThis->m_pcsAgpmFactors->CopyFactor(&stActionData.csFactorDamage, pstActionData->pcsResultFactor, TRUE, FALSE);

			ApBase	*pcsTargetBase	= NULL;

			if (pstActionData->csTargetBase[0].m_lID != 0)
				pcsTargetBase = pThis->m_pcsApmEventManager->GetBase(pstActionData->csTargetBase[0].m_eType, pstActionData->csTargetBase[0].m_lID);
			else
				pcsTargetBase = pstActionData->pcsOwnerBase;

			if (pstActionData->bIsFactorNotQueueing)
			{
				if (pcsTargetBase && pcsTargetBase->m_eType == APBASE_TYPE_CHARACTER)
					pThis->m_pcsAgcmCharacter->ApplyActionData((AgpdCharacter *) pcsTargetBase, &stActionData);
			}
			else
			{
				BOOL	bApplyActionData	= FALSE;

				if (pThis->IsExistCastQueue(pcsTargetBase, stActionData.lActorID))
				{
					pThis->m_pcsAgcmCharacter->ApplyActionData((AgpdCharacter *) pcsTargetBase, &stActionData);
					bApplyActionData	= TRUE;

					pThis->RemoveCastQueue(pcsTargetBase, stActionData.lActorID);
				}

				if (!bApplyActionData)
					pThis->m_pcsAgcmCharacter->AddReceivedAction((AgpdCharacter *) pcsTargetBase, &stActionData);
			}

			/*
			ApBase	*pcsTargetBase	= NULL;

			if (pstActionData->csTargetBase[0].m_lID != 0)
				pcsTargetBase = pThis->m_pcsApmEventManager->GetBase(pstActionData->csTargetBase[0].m_eType, pstActionData->csTargetBase[0].m_lID);
			else
				pcsTargetBase = pstActionData->pcsOwnerBase;

			pThis->EnumCallback(AGCMSKILL_CB_ID_MISS_CAST_SKILL,
								pstActionData->pcsOwnerBase,
								pcsTargetBase);
			*/
		}
		break;

	case AGPMSKILL_ACTION_BLOCK_CAST_SKILL:
		{
			ApBase	*pcsTargetBase	= NULL;

			if (pstActionData->csTargetBase[0].m_lID != 0)
				pcsTargetBase = pThis->m_pcsApmEventManager->GetBase(pstActionData->csTargetBase[0].m_eType, pstActionData->csTargetBase[0].m_lID);
			else
				pcsTargetBase = pstActionData->pcsOwnerBase;

			pThis->EnumCallback(AGCMSKILL_CB_ID_BLOCK_CAST_SKILL, pcsTargetBase, NULL);
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL AgcmSkill::CBActionProductSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill			*pThis				= (AgcmSkill *)			pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	AgpdCharacterAction	*pstAction			= (AgpdCharacterAction *)	pCustData;

	if (pThis->m_stQueuedSkillActionData.lSkillID == pstAction->m_lUserData[0])
	{
		pThis->ProcessReceiveCastSkill(&pThis->m_stQueuedSkillActionData);

		ZeroMemory(&pThis->m_stQueuedSkillActionData, sizeof(pThis->m_stQueuedSkillActionData));
	}

	return TRUE;
}

// 2005.11.16. steeple
BOOL AgcmSkill::CBAdditionalEffect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill* pThis = static_cast<AgcmSkill*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	PVOID* ppvBuffer = static_cast<PVOID*>(pCustData);

	INT32 lEffectID = ppvBuffer[0] ? *(INT32*)ppvBuffer[0] : 0;
	INT32 lTargetCID = ppvBuffer[1] ? *(INT32*)ppvBuffer[1] : 0;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();

	// 여타 다른 Effect 들은 AgcmEventEffect 에서 처리한다. 
	switch(lEffectID)
	{
		case AGPMSKILL_ADDITIONAL_EFFECT_RELEASE_TARGET:
		{
			// 자기가 아닐 때만 해준다.
			if(pcsSelfCharacter == pcsCharacter)
				return TRUE;

			AgcdCharacter* pcsAgcdCharacter = pThis->m_pcsAgcmCharacter->GetCharacterData(pcsSelfCharacter);
			if(!pcsAgcdCharacter)
				return TRUE;

			// 타겟을 해제해준다.
			if(pcsAgcdCharacter->m_lLockTargetID == pcsCharacter->m_lID)
				pThis->m_pcsAgcmCharacter->UnLockTarget(pcsSelfCharacter);
			if(pcsAgcdCharacter->m_lSelectTargetID == pcsCharacter->m_lID)
				pThis->m_pcsAgcmCharacter->UnSelectTarget(pcsSelfCharacter);

			break;
		}
	}

	return TRUE;
}

BOOL AgcmSkill::CBCheckNowUpdateActionFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill			*pThis				= (AgcmSkill *)			pClass;
	ApBase				*pcsTarget			= (ApBase *)			pData;
	BOOL				*pbIsNowUpdate		= (BOOL *)				pCustData;

	if (pcsTarget->m_eType	== APBASE_TYPE_CHARACTER &&
		pcsTarget->m_lID	== pThis->m_pcsAgcmCharacter->GetSelfCID())
		*pbIsNowUpdate	= TRUE;
	else
		*pbIsNowUpdate	= FALSE;

	return TRUE;
}

UINT32 AgcmSkill::GetMaxSkillPreserveTime(INT32 lSkillTID, INT32 lSkillLevel)
{
	if (lSkillTID == AP_INVALID_SKILLID || lSkillLevel <= 0)
		return AGCDSKILL_MAX_ACTION_SKILL_PRESERVE_TIME;

	AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(lSkillTID);
	if (!pcsSkillTemplate)
		return AGCDSKILL_MAX_ACTION_SKILL_PRESERVE_TIME;

	//UINT32	ulPreserveTime	= AGCDSKILL_MAX_ACTION_SKILL_PRESERVE_TIME;

	//ulPreserveTime += (UINT32) ((pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_RANGE][lSkillLevel] / 100.0) * AGCDSKILL_BASE_PRESERVE_TIME_PER_METER);

	//ulPreserveTime += (UINT32) (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CAST_TIME][lSkillLevel]);

	// 2005.03.14. steeple
	// 보스 몹 이 주인공을 쳤을 때 데미지가 너무 늦게 뜨길래 이렇게, 다른 시간은 다 빼고 스킬 액션 리절트를 받은 시간에서
	// 캐스트 시간만 더하게끔 함.
	UINT32 ulPreserveTime = (UINT32)(pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CAST_TIME][lSkillLevel]);

	return ulPreserveTime;
}

BOOL AgcmSkill::ProcessNoticeEffect(stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pvClass)
{
	if (!pvClass)
		return FALSE;

	AgcmSkill			*pThis				= (AgcmSkill *)			pvClass;

	switch (stProcessData.lEffectProcessType) {
	case AGCMEFF2_PROCESS_TYPE_EFFECT_START:
		{
			return pThis->ProcessStartEffect(stProcessData.lOwnerCID, stProcessData.lTargetCID, stProcessData.bMissile);
		}
		break;

	// 2005.09.20. steeple 다시 주석. Static 시간으로 보내는 Twice Packet 이 추가되었으므로 얘는 필요없어졌다.
	//case AGCMEFF2_PROCESS_TYPE_EFFECT_END:			// 2005.03.29. steeple 추가.
	case AGCMEFF2_PROCESS_TYPE_EFFECT_MISSLEEND:
		{
			if(stProcessData.lCustID == 0)	// 이곳에 SkillID 가 온다. (그냥 SkillID)
				break;

			INT32 lSkillTID = 0;
			AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(stProcessData.lCustID);
			lSkillTID = pcsSkill ? pcsSkill->m_pcsTemplate->m_lID : 0;

			if(lSkillTID == 0)
			{
//				ASSERT(lSkillTID != 0);
				break;
			}

			pThis->ProcessEndEffect(stProcessData.lOwnerCID, stProcessData.lTargetCID, stProcessData.bMissile, lSkillTID);

			// 2005.09.22. steeple
			// 타겟이 다중이라면 한놈이 맞을 때 다같이 ProcessEndEffect 불러주자.
			if(pcsSkill && pcsSkill->m_pcsTemplate)
				pThis->ProcessAffectedTarget(pcsSkill, stProcessData);	// 2007.02.13. steeple
		}
		break;

	default:
		return TRUE;
		break;
	}

	return TRUE;
}

BOOL AgcmSkill::ProcessStartEffect(INT32 lOwner, INT32 lTarget, BOOL bIsMissile)
{
	return TRUE;

	if (lOwner == AP_INVALID_CID)
	{
		return FALSE;
	}

	// 발사체는 끝날때 처리한다.
	if (bIsMissile)
	{
		return TRUE;
	}

	AgpdCharacter	*pcsActor		= m_pcsAgpmCharacter->GetCharacter(lOwner);
	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacter(lTarget);

	if (!pcsActor)
	{
		return FALSE;
	}

	if (!pcsTarget)
		pcsTarget	= pcsActor;

	//m_pcsAgcmCharacter->ProcessActionQueue(pcsActor, pcsTarget, AGPDCHAR_ACTION_TYPE_SKILL);

	// Idle Event를 등록한다.
	if (!AddIdleEvent(m_pcsAgcmCharacter->m_ulCurTick + AGCDSKILL_START_SKILL_EFFECT_DELAY,
						pcsTarget->m_lID,
						this,
						ProcessSkillEffectIdleEvent,
						(PVOID) pcsActor->m_lID))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmSkill::ProcessEndEffect(INT32 lOwner, INT32 lTarget, BOOL bIsMissile, INT32 lSkillTID)
{
	if (lOwner == AP_INVALID_CID)
		return FALSE;

	// 2005.03.29. steeple
	// 발사체가 아닌경우데도 한번 체크를 해본다.
	// 발사체가 아닌경우는 이미 처리가 되었다.
	//if (!bIsMissile)
	//	return TRUE;

	AgpdCharacter	*pcsActor		= m_pcsAgpmCharacter->GetCharacter(lOwner);
	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacter(lTarget);

	if (!pcsActor)
		return FALSE;

	if (!pcsTarget)
		pcsTarget	= pcsActor;

	if (!m_pcsAgcmCharacter->ProcessActionQueue(pcsActor, pcsTarget, AGPDCHAR_ACTION_TYPE_SKILL, lSkillTID))
		AddCastQueue(pcsTarget, pcsActor->m_lID);

	return TRUE;
}

BOOL AgcmSkill::ProcessSkillEffectIdleEvent(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	if (!pClass || !pvData)
	{
		return FALSE;
	}

	AgcmSkill		*pThis				= (AgcmSkill *)	pClass;

	AgpdCharacter	*pcsCharacter		= pThis->m_pcsAgpmCharacter->GetCharacter(lCID);
	if (!pcsCharacter)
	{
		return TRUE;
	}

	INT32			lCasterID			= (INT32)	pvData;

	if (!pThis->m_pcsAgcmCharacter->ProcessActionQueue(pThis->m_pcsAgpmCharacter->GetCharacter(lCasterID), pcsCharacter, AGPDCHAR_ACTION_TYPE_SKILL) )
	{
		if ( !pThis->AddCastQueue((ApBase *) pcsCharacter, lCasterID) )
		{
			;
		}
	}

	return TRUE;
}

BOOL AgcmSkill::CastSkillTest(INT32 lSkillNum, INT32 lTargetCID, BOOL bForceAttack)
{
	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsCharacter)
		return FALSE;

	AgpdSkillAttachData *pcsAttachData = m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);
	if (!pcsAttachData)
		return FALSE;

	return CastSkill(pcsAttachData->m_alSkillID[lSkillNum], lTargetCID, bForceAttack);
}

BOOL AgcmSkill::CastSkillTest(INT32 lSkillNum, AuPOS *pposTarget, BOOL bForceAttack)
{
	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsCharacter)
		return FALSE;

	AgpdSkillAttachData *pcsAttachData = m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);
	if (!pcsAttachData)
		return FALSE;

	return CastSkill(pcsAttachData->m_alSkillID[lSkillNum], pposTarget, bForceAttack);
}

BOOL AgcmSkill::PreProcessSkill(INT32 lSkillID, INT32 lTargetCID, BOOL bForceAttack)
{
	// 이 함수에서 TRUE가 리턴되면 CastSkill()을 하지 않는다.
	///////////////////////////////////////////////////////////////
	
	// 자동 공격 스킬인지 본다. 자동 공격스킬이라면 걍 자동공격 해버린다.
	AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->GetSkillByTID( m_pcsAgcmCharacter->GetSelfCharacter(), lSkillID );
	if (!pcsSkill || !pcsSkill->m_pcsTemplate)
		return TRUE;

	if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_bIsAutoAttack)
	{
		//@{ 2006/03/06 burumal
		//m_pcsAgcmCharacter->LockTarget(m_pcsAgcmCharacter->GetSelfCharacter(), lTargetCID, bForceAttack);
		m_pcsAgcmCharacter->LockTarget(m_pcsAgcmCharacter->GetSelfCharacter(), lTargetCID, NULL, bForceAttack);
		//@}

		m_pcsAgcmCharacter->ActionSelfCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, lTargetCID, 0, bForceAttack);

		return TRUE;
	}

	BOOL	bIsPreProcess	= FALSE;

	PVOID	pvBuffer[3];
	pvBuffer[0]	= (PVOID) lSkillID;
	pvBuffer[1]	= (PVOID) lTargetCID;
	pvBuffer[2]	= (PVOID) bForceAttack;

	EnumCallback(AGCMSKILL_CB_ID_PRE_PROCESS_SKILL, pvBuffer, &bIsPreProcess);

	return bIsPreProcess;
}

BOOL AgcmSkill::CastSkill(INT32 lTargetCID, BOOL bForceAttack)
{
	return TRUE;
}

BOOL AgcmSkill::CastSkill(ApBase *pcsTargetBase, BOOL bForceAttack)
{
	return TRUE;
}

BOOL AgcmSkill::CastSkill(AuPOS *pposTarget, BOOL bForceAttack)
{
	return TRUE;
}

BOOL AgcmSkill::CastSkill(ApBase *pcsTargetBase, AuPOS *pposTarget, BOOL bForceAttack)
{
	return TRUE;
}

BOOL AgcmSkill::CastSkill(INT32 lSkillID, INT32 lTargetCID, BOOL bForceAttack)
{
	return CastSkill(lSkillID, (ApBase *) m_pcsAgpmCharacter->GetCharacter(lTargetCID), NULL, bForceAttack);
}

BOOL AgcmSkill::CastSkill(INT32 lSkillID, ApBase *pcsTargetBase, BOOL bForceAttack)
{
	return CastSkill(lSkillID, pcsTargetBase, NULL, bForceAttack);
}

BOOL AgcmSkill::CastSkill(INT32 lSkillID, AuPOS *pposTarget, BOOL bForceAttack)
{
	return CastSkill(lSkillID, NULL, pposTarget, bForceAttack);
}

//		CastSkill
//	Functions
//		- cast lSkillID
//			1. packet을 맹근다.
//			2. 서버로 보낸다.
//	Arguments
//		- lSkillID : 사용할 스킬 아뒤
//		- pcsTargetBase : 타겟 베이쑤 
//		- pposTarget : 타겟 포지션
//		- bForceAttack : 강제 공격 여부 (Ctrl + skill)
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
//@{ kday 20050822
// ;)
// for debugging skill
RwMatrix& GetMatrixShowBox(RwMatrix& out, const RwBBox& box, const RwMatrix& matrot, const RwV3d& pos)
{
	RwMatrixSetIdentity ( &out );
	RwV3d scale = { box.sup.x-box.inf.x,
					box.sup.y-box.inf.y,
					box.sup.z-box.inf.z};		
	RwV3d center= { (box.sup.x+box.inf.x)*0.5f,
					(box.sup.y+box.inf.y)*0.5f,
					(box.sup.z+box.inf.z)*0.5f};
	
	
	RwMatrixTranslate ( &out, &center, rwCOMBINEPOSTCONCAT );
	RwMatrixScale ( &out, &scale, rwCOMBINEPRECONCAT );
	RwMatrixTransform ( &out, &matrot, rwCOMBINEPOSTCONCAT );
	RwMatrixTranslate ( &out, &pos, rwCOMBINEPOSTCONCAT );

	return out;
}
RwMatrix& GetMatrixShowBox(RwMatrix& out, const RwBBox& box)
{ 
	RwMatrixSetIdentity( &out );
	RwV3d scale = { box.sup.x-box.inf.x,
					box.sup.y-box.inf.y,
					box.sup.z-box.inf.z};	
	RwV3d	center	= { (box.sup.x+box.inf.x)*0.5f,
						(box.sup.y+box.inf.y)*0.5f,
						(box.sup.z+box.inf.z)*0.5f};
	RwMatrixScale ( &out, &scale, rwCOMBINEPRECONCAT );
	RwMatrixTranslate ( &out, &center, rwCOMBINEPOSTCONCAT );

	return out;
}

std::list<RwMatrix>	g_BoxContainer;
std::list<RwV3d>	g_PosContainer;
RwMatrix			g_matCharacter;

#include "AgcuVtxType.h"
USING_AGCUVTX;
#include "AgcuBasisGeo.h"
USING_AGCUGEOUD;
AgcuGeo_Sphere<VTX_PD>	g_sphere(0.5f, 24, "sphere");
AgcuGeo_Box<VTX_PD>		g_box(1.f,1.f,1.f,"box");
class functrBox
{
public:
	void operator () (RwMatrix& mat)
	{
		g_box.bRenderW(&mat);
	}
};
class functrPos
{
public:
	void operator () (RwV3d& pos)
	{
		static RwSphere sphere = { {0.f,0.f,0.f,}, 100.f };
		sphere.center = pos;
		g_sphere.bRwRenderW( sphere );
	}
};
void AgcmSkill::DebugSkill_Render()
{
	if( m_bDbgSkill )
	{
		RwMatrix	mat;
		RwMatrixSetIdentity ( &mat );
		RwD3D9SetTransformWorld( &mat );

		std::for_each( g_BoxContainer.begin(), g_BoxContainer.end(), functrBox() );
		std::for_each( g_PosContainer.begin(), g_PosContainer.end(), functrPos() );
	}
};
//@} kday

void GetMonsterList_Ready(const ApWorldSector* pSector, const AuMATRIX* pMat, const AuBOX* pBox, const AuPOS* pPos )
{
	RwBBox tmpbox = {
		{pSector->GetXEnd(), g_matCharacter.pos.y+200.f, pSector->GetZEnd(), },
		{pSector->GetXStart(), g_matCharacter.pos.y-200.f , pSector->GetZStart(), }, };

	RwMatrix	mat;
	GetMatrixShowBox(mat, tmpbox);
	g_BoxContainer.push_back(mat);

	//@{ kday 20051108
	// ;)
	//AXISVIEW::ClearAxis();
	//AXISVIEW::AddAxis(g_matCharacter, 0xffffff00);
	//@} kday

	g_PosContainer.push_back( *(RwV3d*)(pPos) );

	tmpbox.inf = *(RwV3d*)(&pBox->inf);
	tmpbox.sup = *(RwV3d*)(&pBox->sup);
	GetMatrixShowBox(mat, tmpbox, *(RwMatrix*)(pMat), *(RwV3d*)(pPos) );
	g_BoxContainer.push_back( mat );
}

void GetMonsterList_push_Box(const AuPOS& suf, const AuPOS& inf)
{
	RwBBox tmpbox = {
		{suf.x, g_matCharacter.pos.y+suf.y, suf.z, },
		{inf.x, g_matCharacter.pos.y+inf.y , inf.z, }, };

	RwMatrix	mat;
	GetMatrixShowBox(mat, tmpbox);
	g_BoxContainer.push_back(mat);
}
void GetMonsterList_push_Pos(const AuPOS& pos)
{
	g_PosContainer.push_back( *(RwV3d*)(&pos) );
}

BOOL AgcmSkill::CastSkillCheck(INT32 lSkillID, ApBase *pcsTargetBase, AuPOS *pposTarget, BOOL bForceAttack)
{
	// 스킬이 사용 가능한지 확인.
	// return TRUE 면 바로 사용 가능.
	AgpdSkill *pcsSkill = m_pcsAgpmSkill->GetSkill(lSkillID);
	if (!pcsSkill)
		return FALSE;

	if (!m_pcsAgcmCharacter->m_pcsSelfCharacter)
		return FALSE;

	if (!m_pcsAgpmCharacter->IsArchlord(m_pcsAgcmCharacter->m_pcsSelfCharacter) &&
		m_pcsAgcmCharacter->m_pcsSelfCharacter->m_bIsTrasform)
	{
		return FALSE;
	}

	if (!((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_bIsAutoAttack &&
		!m_pcsAgpmSkill->IsRideSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
		m_pcsAgcmCharacter->m_pcsSelfCharacter->m_bRidable)
	{
		// 탈것은 UI Module이 없기 때문에 AgcmUIEventItemRepair에서 신세진다~~ kermi..
		return FALSE;
	}

	// 그라운드 타겟 스킬체크 2006.10.27. steeple
	if (m_pcsAgpmSkill->IsGroundTargetSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
	{
		// 받아온 좌표를 pcsSkill 에 세팅해준다.
		if(pposTarget)
		{
			m_posSelectedPos = *pposTarget;
			pcsSkill->m_posTarget = *pposTarget;
		}
	}

	SetGroundTargetSkillID(0);

	if (m_pcsAgpmCharacter->IsActionBlockCondition(m_pcsAgcmCharacter->m_pcsSelfCharacter, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL))
	{
		return FALSE;
	}

	if (m_bFollowCastLock && pcsTargetBase &&
		m_lFollowCastTargetID == pcsTargetBase->m_lID &&
		m_lFollowCastSkillID == pcsSkill->m_lID)
		return FALSE;

	//ApBase *pcsBase = m_pcsApmEventManager->GetBase(m_csSelectedBase.m_eType, m_csSelectedBase.m_lID);
	ApBase *pcsBase = NULL;
	
	if (pcsTargetBase)
		pcsBase = m_pcsApmEventManager->GetBase(pcsTargetBase->m_eType, pcsTargetBase->m_lID);

	if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lTargetType & AGPMSKILL_TARGET_SELF_ONLY)
	{

		// 2005.01.22. steeple 변경 ENEMY 검사 추가
		if(!pcsTargetBase || !(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lTargetType & AGPMSKILL_TARGET_ENEMY_UNITS))
			pcsBase	= (ApBase *) m_pcsAgcmCharacter->GetSelfCharacter();

	}

	// 2006.08.23. steeple 
	// 테스트용 >> 공성 관련 스킬이라면 일단 pcsBase 를 자기 자신을 넣어준다.
	if (!pcsBase && m_pcsAgpmSkill->IsUnlimitedTargetSiegeWarSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
		pcsBase = (ApBase *) m_pcsAgcmCharacter->GetSelfCharacter();

	//eAgpmSkillActionType	eCheckCastSkillResult;

	if (!m_pcsAgpmSkill->IsSatisfyRequirement(pcsSkill))
	{
		return FALSE;
	}

	// 스킬을 씀으로 소모되는 hp, mp, sp 등이 충분히 있는지 1차 검사한다.

	eAgpmSkillActionType	eActionType	= AGPMSKILL_ACTION_NOT_ENOUGH_HP;
	if (!m_pcsAgpmSkill->IsSufficientCost(pcsSkill, &eActionType))
	{
		return FALSE;
	}

	// 사용 가능 시간이 되었는지 검사한다.
	if (pcsSkill->m_ulRecastDelay > m_pcsAgcmCharacter->m_ulCurTick)
	{
		return FALSE;
	}

	// 2005.04.08. steeple
	// 강제 공격 스킬 개념 추가
	pcsSkill->m_bForceAttack = bForceAttack;

	// 2005.01.14. steeple
	// PvP 가능 여부부터 검사
	if (!m_pcsAgpmSkill->CheckSkillPvP(pcsSkill, pcsBase))
	{
		// 별달리 처리 해주지 말고 걍 리턴 FALSE 해주자.
		return FALSE;
	}

	// 타겟이 제대로 된 넘인지 검사한다.

	return TRUE;
}

BOOL AgcmSkill::CastSkill(INT32 lSkillID, ApBase *pcsTargetBase, AuPOS *pposTarget, BOOL bForceAttack)
{
	AgpdSkill *pcsSkill = m_pcsAgpmSkill->GetSkillByTID( m_pcsAgcmCharacter->m_pcsSelfCharacter, lSkillID);
	if (!pcsSkill)
		return FALSE;

	if (!m_pcsAgcmCharacter->m_pcsSelfCharacter)
		return FALSE;

	// 독핸드 스킬에 적중당해 있는 경우에는 스킬사용 불가능 하다.
	if( m_pcsAgcmCharacter->m_pcsSelfCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISARMAMENT ) return TRUE;

	if (!m_pcsAgpmCharacter->IsArchlord(m_pcsAgcmCharacter->m_pcsSelfCharacter) &&
		m_pcsAgcmCharacter->m_pcsSelfCharacter->m_bIsTrasform)
	{
		EnumCallback(AGCMSKILL_CB_ID_CAST_FAIL_FOR_TRANSFORM, NULL, NULL);
		return FALSE;
	}

	if (!((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_bIsAutoAttack &&
		!m_pcsAgpmSkill->IsRideSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
		m_pcsAgcmCharacter->m_pcsSelfCharacter->m_bRidable)
	{
		// 탈것은 UI Module이 없기 때문에 AgcmUIEventItemRepair에서 신세진다~~ kermi..
		EnumCallback(AGCMSKILL_CB_ID_CAST_FAIL_FOR_RIDE, NULL, NULL);
		return FALSE;
	}

	// DragonScion ToDo : 현재 캐릭터가 아크로드 이고 이 스킬이 진화스킬인경우 실패해야 한다.

	// 그라운드 타겟 스킬체크 2006.10.27. steeple
	if (m_pcsAgpmSkill->IsGroundTargetSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
	{
		if (GetGroundTargetSkillID() == 0)
		{
			SetGroundTargetSkillID(pcsSkill->m_lID);

			// 인자로는 범위를 넘겨준다.
			INT32 lSkillLevel = m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
			FLOAT fArea = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_TARGET_AREA_R][lSkillLevel];
			//fArea = 30.0f * 100.0f;
			EnumCallback(AGCMSKILL_CB_ID_START_GROUND_TARGET, &fArea, NULL);
			return FALSE;
		}
		else
		{
			// 받아온 좌표를 pcsSkill 에 세팅해준다.
			if(pposTarget)
			{
				m_posSelectedPos = *pposTarget;
				pcsSkill->m_posTarget = *pposTarget;
			}
		}
	}

	SetGroundTargetSkillID(0);

	if (m_pcsAgpmCharacter->IsActionBlockCondition(m_pcsAgcmCharacter->m_pcsSelfCharacter, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL))
	{
		/*
		AgcdCharacter	*pcsAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(m_pcsAgcmCharacter->m_pcsSelfCharacter);
		if (pcsAgcdCharacter)
		{
			pcsAgcdCharacter->m_stNextAction.m_eActionType		= AGPDCHAR_ACTION_TYPE_SKILL;

			if (pcsTargetBase)
			{
				pcsAgcdCharacter->m_stNextAction.m_csTargetBase.m_eType	= pcsTargetBase->m_eType;
				pcsAgcdCharacter->m_stNextAction.m_csTargetBase.m_lID	= pcsTargetBase->m_lID;
			}

			if (pposTarget)
			{
				pcsAgcdCharacter->m_stNextAction.m_stTargetPos			= *pposTarget;
			}

			pcsAgcdCharacter->m_stNextAction.m_bForceAction		= bForceAttack;

			pcsAgcdCharacter->m_stNextAction.m_lUserData[0]		= lSkillID;
		}
		*/

		AgpdCharacterAction	stNextAction;
		ZeroMemory(&stNextAction, sizeof(AgpdCharacterAction));

		stNextAction.m_eActionType			= AGPDCHAR_ACTION_TYPE_SKILL;

		if (pcsTargetBase)
		{
			stNextAction.m_csTargetBase.m_eType	= pcsTargetBase->m_eType;
			stNextAction.m_csTargetBase.m_lID	= pcsTargetBase->m_lID;
		}
		
		if (pposTarget)
			stNextAction.m_stTargetPos			= *pposTarget;

		stNextAction.m_lUserData[0]			= lSkillID;
		stNextAction.m_lUserData[1]			= (INT32) bForceAttack;

		m_pcsAgcmCharacter->SetNextAction(&stNextAction);

		return FALSE;
	}

	if (m_bFollowCastLock && pcsTargetBase &&
		m_lFollowCastTargetID == pcsTargetBase->m_lID &&
		m_lFollowCastSkillID == pcsSkill->m_lID)
		return TRUE;

	//ApBase *pcsBase = m_pcsApmEventManager->GetBase(m_csSelectedBase.m_eType, m_csSelectedBase.m_lID);
	ApBase *pcsBase = NULL;
	
	if (pcsTargetBase)
		pcsBase = m_pcsApmEventManager->GetBase(pcsTargetBase->m_eType, pcsTargetBase->m_lID);

	if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lTargetType & AGPMSKILL_TARGET_SELF_ONLY)
	//if (!pcsBase)
	{
		//if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lTargetType & AGPMSKILL_TARGET_SELF_ONLY)
		//	lTargetID	= pThis->m_pcsAgcmCharacter->GetSelfCID();


		// 2005.01.22. steeple 변경 ENEMY 검사 추가
		if(!pcsTargetBase || !(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lTargetType & AGPMSKILL_TARGET_ENEMY_UNITS))
			pcsBase	= (ApBase *) m_pcsAgcmCharacter->GetSelfCharacter();

		
		
		//m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
		//return FALSE;
	}

	// 2006.08.23. steeple 
	// 테스트용 >> 공성 관련 스킬이라면 일단 pcsBase 를 자기 자신을 넣어준다.
	if (!pcsBase && m_pcsAgpmSkill->IsUnlimitedTargetSiegeWarSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
		pcsBase = (ApBase *) m_pcsAgcmCharacter->GetSelfCharacter();

	eAgpmSkillActionType	eCheckCastSkillResult;

	if (!m_pcsAgpmSkill->IsSatisfyRequirement(pcsSkill))
	{
		eCheckCastSkillResult	= AGPMSKILL_ACTION_NEED_REQUIREMENT;

		EnumCallback(AGCMSKILL_CB_ID_CHECK_CAST_SKILL_RESULT, (PVOID) eCheckCastSkillResult, NULL);

		//m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

		return FALSE;
	}

	// 스킬을 씀으로 소모되는 hp, mp, sp 등이 충분히 있는지 1차 검사한다.

	eAgpmSkillActionType	eActionType	= AGPMSKILL_ACTION_NOT_ENOUGH_HP;
	if (!m_pcsAgpmSkill->IsSufficientCost(pcsSkill, &eActionType))
	{
		/*
		AgpdItem	*pcsPotion	= NULL;

		switch (eActionType) {
		case AGPMSKILL_ACTION_NOT_ENOUGH_MP:
			{
//				pcsPotion	= m_pcsAgpmItem->GetMPPotionInInventory((AgpdCharacter *) pcsSkill->m_pcsBase);

				INT32	lMPPotionTID	= AP_INVALID_IID;
				EnumCallback(AGCMSKILL_CB_ID_GET_REGISTERED_MP_POTION_TID, &lMPPotionTID, NULL);

				pcsPotion	= m_pcsAgpmItem->GetInventoryItemByTID((AgpdCharacter *) pcsSkill->m_pcsBase, lMPPotionTID);
			}
			break;

//		case AGPMSKILL_ACTION_NOT_ENOUGH_SP:
//			{
//				pcsPotion	= m_pcsAgpmItem->GetSPPotionInInventory((AgpdCharacter *) pcsSkill->m_pcsBase);
//			}
//			break;
		}

		if (!pcsPotion)
		*/
		{
			// eActionType에 따라 어떤넘이 부족해서 스킬을 사용할 수 없는지 표시한다.
			EnumCallback(AGCMSKILL_CB_ID_CHECK_CAST_SKILL_RESULT, (PVOID) eActionType, NULL);

			//m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

			return FALSE;
		}
	}

	// 사용 가능 시간이 되었는지 검사한다.
	if (pcsSkill->m_ulRecastDelay > m_pcsAgcmCharacter->m_ulCurTick)
	{
		// 아직 사용 가능한 스킬이 아니다. 시간이 더 지나야 한다.
		eCheckCastSkillResult	= AGPMSKILL_ACTION_NOT_READY_CAST;

		EnumCallback(AGCMSKILL_CB_ID_CHECK_CAST_SKILL_RESULT, (PVOID) eCheckCastSkillResult, NULL);

		//m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

		return FALSE;
	}

	if (PreProcessSkill(lSkillID, pcsTargetBase ? pcsTargetBase->m_lID : 0, bForceAttack))
		return TRUE;

	// 2005.04.08. steeple
	// 강제 공격 스킬 개념 추가
	pcsSkill->m_bForceAttack = bForceAttack;

	// 2005.01.14. steeple
	// PvP 가능 여부부터 검사
	if (!m_pcsAgpmSkill->CheckSkillPvP(pcsSkill, pcsBase))
	{
		// 별달리 처리 해주지 말고 걍 리턴 FALSE 해주자.
		return FALSE;
	}

	// 타겟이 제대로 된 넘인지 검사한다.
	AuPOS	pstCalcPos;
	AgpmSkillCheckTargetResult	eCheckTargetResult = m_pcsAgpmSkill->IsValidTarget(pcsSkill, pcsBase, bForceAttack, &pstCalcPos);

	switch (eCheckTargetResult) {
	case AGPMSKILL_CHECK_TARGET_INVALID:
		{
			eCheckCastSkillResult	= AGPMSKILL_ACTION_INVALID_TARGET;

			EnumCallback(AGCMSKILL_CB_ID_CHECK_CAST_SKILL_RESULT, (PVOID) eCheckCastSkillResult, NULL);

			//m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

			return FALSE;
		}
		break;

	case AGPMSKILL_CHECK_TARGET_NOT_IN_RANGE:
		// pstCalcPos로 이동한다.
		//
		//return TRUE;
		break;
	}

	//@{ kday 20050822
	// ;)
	if( m_bDbgSkill )
	{
		FLOAT	x = (FLOAT) m_pcsAgpmSkill->GetTargetArea2(pcsSkill);
		FLOAT	z = (FLOAT) m_pcsAgpmSkill->GetTargetArea1(pcsSkill);

		AuBOX	stBox;

		stBox.inf.x = (-(x / 2.0f));
		stBox.inf.y = (-1000);
		stBox.inf.z = (0);

		stBox.sup.x = x / 2.0f;
		stBox.sup.y = 1000;
		stBox.sup.z = z;

		INT32 alCID[32];
		memset(alCID, 0, sizeof(alCID));
		
		g_BoxContainer.clear();
		g_PosContainer.clear();

		g_matCharacter = *(RwMatrix*)(&m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stMatrix);
		g_matCharacter.pos = *(RwV3d*)(&m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stPos);

		ApmMap* pcsApmMap = (ApmMap*)GetModule("ApmMap");
		pcsApmMap->GetCharList(
			m_pcsAgcmCharacter->m_pcsSelfCharacter->m_nDimension
			, APMMAP_CHAR_TYPE_MONSTER
			, m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stPos
			, &m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stMatrix
			, &stBox
			, alCID
			, 32
			, NULL
			, 0
			, m_bDbgSkill);
	}
	//@} kday

	INT16	nPacketLength = 0;

	/*
	// pcsSkill을 cast 한다는 패킷을 만들어 서버로 보낸다.
	PVOID	pvPacket = m_pcsAgpmSkill->MakeCastPacket(AGPMSKILL_ACTION_START_CAST_SKILL, &pcsSkill->m_lID, NULL, (ApBase *) m_pcsAgcmCharacter->m_pcsSelfCharacter,
													  NULL, &m_csSelectedBase, &m_posSelectedPos, bForceAttack, NULL, 0, &nPacketLength);
	*/
	// pcsSkill을 cast 한다는 패킷을 만들어 서버로 보낸다.
	PVOID	pvPacket = m_pcsAgpmSkill->MakeCastPacket(AGPMSKILL_ACTION_START_CAST_SKILL, &pcsSkill->m_lID, &pcsSkill->m_pcsTemplate->m_lID, (ApBase *) m_pcsAgcmCharacter->m_pcsSelfCharacter,
													  0, pcsBase, &m_posSelectedPos, bForceAttack, 0, 0, 0, NULL, 0, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
	{
		//m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
		return FALSE;
	}

	BOOL bRetval = TRUE;

	//m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, 1000, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);

	// pvPacket을 서버로 보낸다.
	if (!SendPacket(pvPacket, nPacketLength))
	{
		m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);
		bRetval = FALSE;

		//m_pcsAgcmCharacter->m_pcsSelfCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

		//m_pcsAgpmCharacter->SetActionBlockTime(m_pcsAgcmCharacter->m_pcsSelfCharacter, 0, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
	}

	// 2007.02.13. steeple
	AgcdSkill* pcsAgcdSkill = GetAgcdSkill(pcsSkill);
	if(pcsAgcdSkill)
		pcsAgcdSkill->m_alAffectedTargetCID.MemSetAll();

	m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);

	return bRetval;
}

AgcdSkillADChar* AgcmSkill::GetCharacterAttachData(ApBase *pcsBase)
{
	if (!pcsBase)
		return NULL;

	switch (pcsBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		return (AgcdSkillADChar *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, (PVOID) pcsBase);
		break;
	}

	return NULL;
}

AgcdSkillAttachTemplateData* AgcmSkill::GetAttachTemplateData(ApBase *pcsBase)
{
	if (!pcsBase)
		return NULL;

	switch (pcsBase->m_eType) {
	case APBASE_TYPE_CHARACTER_TEMPLATE:
		return (AgcdSkillAttachTemplateData *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nIndexADCharacterTemplate, (PVOID) pcsBase);
		break;

	case APBASE_TYPE_OBJECT_TEMPLATE:
		return NULL;
		break;
	}

	return NULL;
}

AgpdCharacterTemplate *AgcmSkill::GetCharacterTemplate(AgcdSkillAttachTemplateData *pcsAgcdSkillAttachTemplateData)
{
	return (AgpdCharacterTemplate *)(m_pcsAgpmCharacter->GetParentModuleData(m_nIndexADCharacterTemplate, (PVOID)(pcsAgcdSkillAttachTemplateData)));
}

BOOL AgcmSkill::ConAgcdSkillAttachData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgcdSkillADChar	*pcsAgcdSkillADChar	= pThis->GetCharacterAttachData((ApBase *) pcsCharacter);

	if (pcsAgcdSkillADChar)
	{
		pcsAgcdSkillADChar->m_lNumCastSkill		= 0;

		ZeroMemory(&pcsAgcdSkillADChar->m_stCastQueue, sizeof(AgcmSkillCastQueue) * AGCDSKILL_MAX_ACTION_QUEUE_SIZE);
	}

	return TRUE;
}

BOOL AgcmSkill::DesAgcdSkillAttachData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmSkill::ConTemplateAttachData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	ApBase			*pcsBase		= (ApBase	*)		pData;

	AgcdSkillAttachTemplateData	*pcsAttachData	= pThis->GetAttachTemplateData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	ZeroMemory(pcsAttachData, sizeof (AgcdSkillAttachTemplateData));

	AgcdCharacterTemplate *pstAgcdCharacterTemplate =
		pThis->m_pcsAgcmCharacter->GetTemplateData(
			(AgpdCharacterTemplate *)(pcsBase)		);
	if (!pstAgcdCharacterTemplate)
		return FALSE;

	/*INT32	lNumVisualInfo	= pThis->GetNumVisualInfo(pstAgcdCharacterTemplate);
	if (lNumVisualInfo > 0)
		pcsAttachData->m_pacsSkillVisualInfo	= new AgcmSkillVisualInfo* [lNumVisualInfo];*/

	return TRUE;
}

BOOL AgcmSkill::DesTemplateAttachData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	ApBase			*pcsBase		= (ApBase	*)		pData;

	AgcdSkillAttachTemplateData	*pcsAttachData	= pThis->GetAttachTemplateData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	AgcdCharacterTemplate	*pstAgcdCharacterTemplate	=
		pThis->m_pcsAgcmCharacter->GetTemplateData(	(AgpdCharacterTemplate *)(pcsBase)	);
	if (!pstAgcdCharacterTemplate)
		return FALSE;

	INT32	lNumVisualInfo	= pThis->GetNumVisualInfo(pstAgcdCharacterTemplate);
	INT32	lNumAnimType2	= pThis->m_pcsAgcmCharacter->GetAnimType2Num(pstAgcdCharacterTemplate);
	//AgcdUseEffectSetList	*pcsEListPrev;
	//AgcdUseEffectSetList	*pcsEList;

	if (pcsAttachData->m_pacsSkillVisualInfo)
	{
		//for (INT32 lSkillIndex = 0; lSkillIndex < AGPMSKILL_MAX_SKILL_OWN; ++lSkillIndex)
		for (INT32 lVisualInfoIndex = 0; lVisualInfoIndex < lNumVisualInfo; ++lVisualInfoIndex)
		{
			if (!pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex])
				continue;

			for (INT32 lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2)
			{
				/*if (pcsAttachData->m_astSkillVisualInfo[lSkillIndex].m_paszInfoName[lEquipType])
				{
					delete [] pcsAttachData->m_astSkillVisualInfo[lSkillIndex].m_paszInfoName[lEquipType];
					pcsAttachData->m_astSkillVisualInfo[lSkillIndex].m_paszInfoName[lEquipType] = NULL;
				}

				if (pcsAttachData->m_astSkillVisualInfo[lSkillIndex].m_paszAnimName[lEquipType])
				{
					delete [] pcsAttachData->m_astSkillVisualInfo[lSkillIndex].m_paszAnimName[lEquipType];
					pcsAttachData->m_astSkillVisualInfo[lSkillIndex].m_paszAnimName[lEquipType] = NULL;
				}

				pThis->m_csEffectDataAdmin2.DeleteAll(pcsAttachData->m_astSkillVisualInfo[lSkillIndex].m_astEffect + lEquipType);*/

				if (pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszInfo[lAnimType2])
				{
					delete [] pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszInfo[lAnimType2];
					pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszInfo[lAnimType2]	= NULL;
				}

				/*if (pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszAnimName[lAnimType2])
				{
					delete [] pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszAnimName[lAnimType2];
					pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszAnimName[lAnimType2]	= NULL;
				}*/

				if (pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2])
				{
					if (pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation)
					{
						ACA_AttachedData	*pcsAttachedAnimData	=
							(ACA_AttachedData *)(pThis->m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData(
								AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
								pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead	));
						if (!pcsAttachedAnimData)
						{
							ASSERT(!"!pcsAttachedAnimData");
							return FALSE;
						}

						if (pcsAttachedAnimData->m_pszPoint)
						{
							delete [] pcsAttachedAnimData->m_pszPoint;
							pcsAttachedAnimData->m_pszPoint = NULL;
						}
					}

					pThis->m_pcsAgcmCharacter->GetAgcaAnimation2()->RemoveAllAnimation(
						&pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation	);

					delete pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2];
					pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2] = NULL;
				}

				if (pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2])
				{
					pThis->m_csEffectDataAdmin2.DeleteAll(
						pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2]	);

					delete pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2];
					pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2] = NULL;
				}
			}

			pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->FreePArray();

			delete pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex];
			pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex] = NULL;
		}

		delete [] pcsAttachData->m_pacsSkillVisualInfo;
	}

	return TRUE;
}

// 2005.09.22. steeple
BOOL AgcmSkill::ConAgcdSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdSkill* pcsSkill = static_cast<AgpdSkill*>(pData);
	AgcmSkill* pThis = static_cast<AgcmSkill*>(pClass);

	AgcdSkill* pcsAgcdSkill = pThis->GetAgcdSkill(pcsSkill);
	if(!pcsAgcdSkill)
		return FALSE;

	pcsAgcdSkill->m_alAffectedTargetCID.MemSetAll();

	return TRUE;
}

// 2005.09.22. steeple
BOOL AgcmSkill::DesAgcdSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

/*BOOL AddCustData(CHAR *pszDestCustData, CHAR *pszAddCustData)
{
	if (!pszDestCustData[0])
		strcpy(pszDestCustData, "0()");
//		strcpy(pszDestCustData, "0[]");

	CHAR	szTemp[256], szTemp2[256];
	INT32	lProgress, lProgress2;
	INT32	lCustDataNum	= atoi(pszDestCustData);
	INT32	lCustDataLen	= strlen(pszDestCustData);
	INT32	lAddDataLen		= strlen(pszAddCustData);
	INT32	lOffset			= (lCustDataNum > 9) ? (2) : (1);

	strcpy(szTemp, pszDestCustData + lOffset);

	for (lProgress = 0; lProgress < lCustDataLen; ++lProgress)
	{
		if (szTemp[lProgress] == pszAddCustData[0])
		{
			for (lProgress2 = lProgress; lProgress2 < lCustDataLen; ++lProgress2)
			{
				if ((szTemp[lProgress2] == '|') || (szTemp[lProgress2] == ')'))
				{
					strcpy(szTemp2, pszDestCustData);
					szTemp2[lOffset + lProgress] = '\0';
					sprintf(pszDestCustData, "%s%s%s", szTemp2, pszAddCustData, szTemp + lProgress2);
					return TRUE;
				}
			}

			return FALSE;
		}

		if (szTemp[lProgress] == ')')
		{
			if (lCustDataNum > 0)
			{
				szTemp[lProgress] = '|';
				++lProgress;
			}

			strcpy(szTemp + lProgress, pszAddCustData);
			szTemp[lProgress + (lAddDataLen + 0)] = ')';
			szTemp[lProgress + (lAddDataLen + 1)] = '\0';
			break;
		}
	}

	sprintf(pszDestCustData, "%d%s", ++lCustDataNum, szTemp);

	return TRUE;
}*/

BOOL AgcmSkill::AttachedTemplateAnimationWrite(	ApModuleStream *pStream,
												AgcmSkillVisualInfo *pcsInfo,
												INT32 lVisualInfoIndex,
												INT32 lAnimType2				)
{
	if (	(pcsInfo->m_pacsAnimation[lAnimType2]) &&
			(pcsInfo->m_pacsAnimation[lAnimType2]->m_pcsAnimation)	)
	{
		if (!pcsInfo->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead)
		{
			ASSERT(!"Invalid - pcsInfo->m_pacsAnimation[lAnimTyp2]->m_pcsAnimation->m_pcsHead");
			return FALSE;
		}

		CHAR szValueName[512], szValue[1024];
		memset(szValueName, 0, sizeof (CHAR) * 512);
		memset(szValue, 0, sizeof (CHAR) * 1024);

		if (pcsInfo->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName)
		{
			sprintf(
				szValueName,
				"%s%d%d",
				AGCMSKILL_INI_NAME_ANIMATION_NAME,
				lVisualInfoIndex,
				lAnimType2							);

			sprintf(
				szValue,
				"%d:%d:%s",
				lVisualInfoIndex,
				lAnimType2,
				//pcsInfo->m_paszAnimName[lAnimType2]	);
				pcsInfo->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName	);

			pStream->WriteValue(szValueName, szValue);
		}		

		ACA_AttachedData	*pcsAttachedData	=
			(ACA_AttachedData *)(m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData(
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
				pcsInfo->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead				));

		if (	(pcsAttachedData) &&
				(pcsAttachedData->m_pszPoint)	)
		{
			sprintf(szValueName,	"%s%d%d",	AGCMSKILL_INI_NAME_ANIMATION_POINT, lVisualInfoIndex, lAnimType2);
			sprintf(szValue,		"%d:%d:%s",	lVisualInfoIndex, lAnimType2, pcsAttachedData->m_pszPoint);
			
			pStream->WriteValue(szValueName, szValue);
		}

		if (pcsInfo->m_pacsAnimation[lAnimType2]->m_pstAnimFlags)
		{
			sprintf(szValueName, "%s%d%d", AGCMSKILL_INI_NAME_ANIMATION_FLAGS, lVisualInfoIndex, lAnimType2);
			sprintf(szValue,
				"%d:%d:%d:%d",
				lVisualInfoIndex,
				lAnimType2,
				pcsInfo->m_pacsAnimation[lAnimType2]->m_pstAnimFlags->m_unAnimFlag,
				pcsInfo->m_pacsAnimation[lAnimType2]->m_pstAnimFlags->m_unPreference	);

			pStream->WriteValue(szValueName, szValue);
		}
	}

	return TRUE;
}

BOOL AgcmSkill::AttachedTemplateEffectWrite(	ApModuleStream *pStream,
												AgcmSkillVisualInfo *pcsInfo,
												INT32 lVisualInfoIndex,
												INT32 lAnimType2				)
{
	if (!pcsInfo->m_pastEffect[lAnimType2])
		return TRUE; // skip

	CHAR szValueName[512], szValue[1024];
	memset(szValueName, 0, sizeof (CHAR) * 512);
	memset(szValue, 0, sizeof (CHAR) * 1024);

	AgcdUseEffectSetList	*pcsCurrent		= pcsInfo->m_pastEffect[lAnimType2]->m_pcsHead;
	while (pcsCurrent)
	{
		AgcdUseEffectSetData *pstEData = &pcsCurrent->m_csData;
		if (!pstEData)
			break;

		if ((!pstEData->m_ulEID) && (!pstEData->m_pszSoundName))
		{
			pcsCurrent	=	pcsCurrent->m_pcsNext;
			continue;
		}

		// Condition flags
		if (pstEData->m_ulConditionFlags)
		{
			sprintf(szValueName,		"%s%d%d%d",					AGCMSKILL_INI_NAME_EFFECT_CONDITION, lVisualInfoIndex, lAnimType2, pstEData->m_ulIndex);
			sprintf(szValue,			"%d:%d:%d:%d",				lVisualInfoIndex, lAnimType2, pstEData->m_ulIndex, pstEData->m_ulConditionFlags);

			pStream->WriteValue(szValueName, szValue);
		}

		// Use effect set
		{
			sprintf(szValueName,		"%s%d%d%d",							AGCMSKILL_INI_NAME_EFFECT_DATA, lVisualInfoIndex, lAnimType2, pstEData->m_ulIndex);
			print_compact_format(szValue,"%d:%d:%d:%d:%f:%f:%f:%f:%d:%d",	lVisualInfoIndex,
																			lAnimType2,
																			pstEData->m_ulIndex,
																			pstEData->m_ulEID,
																			(pstEData->m_pv3dOffset) ? (pstEData->m_pv3dOffset->x) : (0.0f),
																			(pstEData->m_pv3dOffset) ? (pstEData->m_pv3dOffset->y) : (0.0f),
																			(pstEData->m_pv3dOffset) ? (pstEData->m_pv3dOffset->z) : (0.0f),
																			pstEData->m_fScale,
																			pstEData->m_lParentNodeID,
																			pstEData->m_ulStartGap		);
			pStream->WriteValue(szValueName, szValue);
		}

		// CUSTOM DATA
		if (pstEData->m_pszCustData)
		{
			sprintf(szValueName,		"%s%d%d%d",					AGCMSKILL_INI_NAME_EFFECT_CUST_DATA, lVisualInfoIndex, lAnimType2, pstEData->m_ulIndex);
			sprintf(szValue,			"%d:%d:%d:%s",				lVisualInfoIndex, lAnimType2, pstEData->m_ulIndex, pstEData->m_pszCustData);

			pStream->WriteValue(szValueName, szValue);
		}

		pcsCurrent	=	pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

BOOL AgcmSkill::AttachedTemplateEffectSoundWrite( ApModuleStream *pStream,
											AgcmSkillVisualInfo *pcsInfo,
											INT32 lVisualInfoIndex,
											INT32 lAnimType2				)
{
	if (!pcsInfo->m_pastEffect[lAnimType2])
		return TRUE; // skip

	CHAR szValueName[512], szValue[1024];
	memset(szValueName, 0, sizeof (CHAR) * 512);
	memset(szValue, 0, sizeof (CHAR) * 1024);

	AgcdUseEffectSetList	*pcsCurrent		= pcsInfo->m_pastEffect[lAnimType2]->m_pcsHead;
	while (pcsCurrent)
	{
		AgcdUseEffectSetData *pstEData = &pcsCurrent->m_csData;
		if (!pstEData)
			break;

		if ((!pstEData->m_ulEID) && (!pstEData->m_pszSoundName))
		{
			pcsCurrent	=	pcsCurrent->m_pcsNext;
			continue;
		}

		// Sound
		if (pstEData->m_pszSoundName)
		{
			sprintf(szValueName,		"%s%d%d%d",					AGCMSKILL_INI_NAME_EFFECT_SOUND, lVisualInfoIndex, lAnimType2, pstEData->m_ulIndex);
			sprintf(szValue,			"%d:%d:%d:%s",				lVisualInfoIndex, lAnimType2, pstEData->m_ulIndex, pstEData->m_pszSoundName);

			pStream->WriteValue(szValueName, szValue);
		}

		pcsCurrent	=	pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

eAcReturnType AgcmSkill::CheckVisualInfo(AgcdCharacterTemplate *pstAgcdCharacterTemplate, AgcdSkillAttachTemplateData *pcsAttachedData, INT32 lVisualIndex)
{
	if (!pcsAttachedData->m_pacsSkillVisualInfo)
	{
		INT32	lNumVisualInfo	= GetNumVisualInfo(pstAgcdCharacterTemplate);
		if (lNumVisualInfo > 0)
		{
			pcsAttachedData->m_pacsSkillVisualInfo	=
				new AgcmSkillVisualInfo* [lNumVisualInfo];
			memset(pcsAttachedData->m_pacsSkillVisualInfo, 0, sizeof (AgcmSkillVisualInfo *) * lNumVisualInfo);
		}
		else
		{
			return E_AC_RETURN_TYPE_SKIP;
		}
	}

	if (!pcsAttachedData->m_pacsSkillVisualInfo[lVisualIndex])
	{
		pcsAttachedData->m_pacsSkillVisualInfo[lVisualIndex]	= new AgcmSkillVisualInfo();
		if (!pcsAttachedData->m_pacsSkillVisualInfo[lVisualIndex])
			return E_AC_RETURN_TYPE_FAILURE;

		INT32	lNumAnimType2	= m_pcsAgcmCharacter->GetAnimType2Num(pstAgcdCharacterTemplate);
		if (lNumAnimType2 < 1)
			return E_AC_RETURN_TYPE_FAILURE;

		if (!pcsAttachedData->m_pacsSkillVisualInfo[lVisualIndex]->AllocatePArray(lNumAnimType2))
			return E_AC_RETURN_TYPE_FAILURE;
	} 

	return E_AC_RETURN_TYPE_SUCCESS;
}


BOOL AgcmSkill::ParseTemplateString(CHAR *szString, CHAR *szFrontString, INT16 nFrontLength, CHAR *szBackString, INT16 nBackLength)
{
	int	i;

	if (!szString || !szFrontString || !szBackString || nFrontLength < 1 || nBackLength < 1)
		return FALSE;

	ZeroMemory(szFrontString, sizeof(CHAR) * nFrontLength);
	ZeroMemory(szBackString, sizeof(CHAR) * nBackLength);

	INT16	nStringLength = strlen(szString);
	if (nStringLength < 1)
		return FALSE;

	for (i = 0; i < nStringLength; ++i)
	{
		if (szString[i] == ':')
			break;
	}

	if (i == nStringLength)
		return FALSE;

	if (nFrontLength < i || nBackLength < nStringLength - i - 1)
		return FALSE;

	strncpy(szFrontString, szString, i);

	strncpy(szBackString, szString + i + 1, nStringLength - i - 1);

	return TRUE;
}

INT16 AgcmSkill::GetUsableSkillTNameIndex(ApBase *pcsBase, CHAR *szTemplateName)
{
	if (!pcsBase || !szTemplateName)
		return (-1);

	// AgpdSkillTemplateAttachData의 m_aszUsableSkillTName에서 szTemplateName 이름을 찾는다.
	// 못찾으면 에러다.
	AgpdSkillTemplateAttachData	*pcsAgpdTemplateAttachData = m_pcsAgpmSkill->GetAttachSkillTemplateData(pcsBase);
	if (!pcsAgpdTemplateAttachData)
		return (-1);

	int i;
	for (i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		if(!pcsAgpdTemplateAttachData->m_aszUsableSkillTName[i])
			return -1;

		if (!strncmp(szTemplateName, pcsAgpdTemplateAttachData->m_aszUsableSkillTName[i], AGPMSKILL_MAX_SKILL_NAME))
		{
			if( _CheckVisualInfo( (AgpdCharacterTemplate*)pcsBase , i ) )
				break;
		}
	}

	if (i == AGPMSKILL_MAX_SKILL_OWN)
		return (-1);
	
	return i;
}

BOOL	AgcmSkill::_CheckVisualInfo( AgpdCharacterTemplate* pcsCharacterTemplate , INT32 nIndex )
{
	AgcdSkillAttachTemplateData	*pstAgcdSkillAttachTemplateData	= GetAttachTemplateData((ApBase *)(pcsCharacterTemplate));

	if( pstAgcdSkillAttachTemplateData && pstAgcdSkillAttachTemplateData->m_pacsSkillVisualInfo[ nIndex ] )
		return TRUE;

	return FALSE;
}

INT16 AgcmSkill::GetUsableSkillTNameIndex(ApBase *pcsBase, INT32 lTID)
{
	if (!pcsBase || lTID == AP_INVALID_SKILLID)
		return (-1);

	AgpdSkillTemplate *pcsTemplate = m_pcsAgpmSkill->GetSkillTemplate(lTID);
	if (!pcsTemplate) return (-1);

	return GetUsableSkillTNameIndex(pcsBase, pcsTemplate->m_szName);
}

/******************************************************************************
* Purpose :
*
* 041103. Bob Jung
******************************************************************************/
/*BOOL AgcmSkill::SetSkillAnimation(INT32 lTID, pstAgcmSkillVisualInfo pstInfo, CHAR *szData)
{
	ASSERT( NULL != szData );

	INT32	lEquipType, lUsableIndex, lFlag, lPreference;
	CHAR	szAnimationName[256];
	CHAR	szAnimKeyName[256];

// 마고자 (2004-03-09 오전 10:25:48) : 아규먼트 갯수체크하는거 그냥 간단히 만들어놨는데 필요하면 쓰세용.
//	INT32	nArgCount = 1;
//	for( int i = 0 ; i < strlen( szData ) ; ++ i ) szData[ i ] == ':' ? nArgCount++ : 0 ;
//	if( nArgCount != 5 ) return FALSE; 

	sscanf(szData, "%d:%d:%d:%d:%s", &lUsableIndex, &lEquipType, &lFlag, &lPreference, szAnimationName);

	sprintf(szAnimKeyName, "SKILL_%d_%d_%d", lTID, lUsableIndex, lEquipType);

	pstInfo[lUsableIndex].m_pastAnim[lEquipType] = m_pcsAgcmCharacter->GetAnimation(szAnimKeyName, TRUE);
	if (!pstInfo[lUsableIndex].m_pastAnim[lEquipType])
	{
		OutputDebugString("AgcmSkill::SetSkillAnimation() Error (1) !!!\n");
		return FALSE;
	}

	if (!pstInfo[lUsableIndex].m_paszAnimName[lEquipType])
		pstInfo[lUsableIndex].m_paszAnimName[lEquipType] = new CHAR[AGCDSKILL_ANIMATION_NAME_LENGTH];

	strcpy(pstInfo[lUsableIndex].m_paszAnimName[lEquipType], szAnimationName);

	if (!pstInfo[lUsableIndex].m_pastAnim[lEquipType]->m_ppastAnimData)
	{
		OutputDebugString("AgcmSkill::SetSkillAnimation() Error (2) !!!\n");
		return FALSE;
	}

	pstInfo[lUsableIndex].m_pastAnim[lEquipType]->m_ppastAnimData[0] = m_pcsAgcmCharacter->GetAnimData(szAnimationName, m_pcsAgcmCharacter->GetAnimationPath());
	if (!pstInfo[lUsableIndex].m_pastAnim[lEquipType]->m_ppastAnimData[0])
	{
		OutputDebugString("AgcmSkill::SetSkillAnimation() Error (3) !!!\n");
		return FALSE;
	}

	AgcdCharacterAnimationAttachedData	*pstAttachedData = (AgcdCharacterAnimationAttachedData *)(
				m_pcsAgcmCharacter->GetAnimationAttachedData(
				pstInfo[lUsableIndex].m_pastAnim[lEquipType],
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY				)							);

	if (pstAttachedData)
	{
		pstAttachedData->m_stAnimFlag.m_ulAnimFlag		= lFlag;
		pstAttachedData->m_stAnimFlag.m_ulPreference	= lPreference;

		pstAttachedData->m_alActiveRate[0]				= 100;
	}

//		pstInfo[lUsableIndex].m_pastAnim[lEquipType]->m_stAnimFlag.m_ulAnimFlag		= lFlag;
//		pstInfo[lUsableIndex].m_pastAnim[lEquipType]->m_stAnimFlag.m_ulPreference	= lPreference;

//		pstInfo[lUsableIndex].m_pastAnim[lEquipType]->m_alActiveRate[0]				= 100; // 무조건 100이당...

	return TRUE;
}*/

/******************************************************************************
* Purpose : 
*
* 071003. Bob Jung
******************************************************************************/
/*AgcdAnimation *AgcmSkill::GetAnimation(INT32 lTID, INT32 lDataIndex, INT32 lEquipType, BOOL bAdd)
{
	CHAR	szAnimKeyName[256];
	sprintf(szAnimKeyName, "SKILL_%d_%d_%d", lTID, lDataIndex, lEquipType);

	AgcdAnimation *pstAnim = m_pcsAgcmCharacter->GetAnimation(szAnimKeyName, bAdd);

	return pstAnim;
}*/

/******************************************************************************
* Purpose :
*
* 041103. Bob Jung
******************************************************************************/
eAcReturnType AgcmSkill::SetSkillEffectData(AgcdCharacterTemplate *pstAgcdCharacterTemplate, AgcdSkillAttachTemplateData *pcsAttachedData, CHAR *szData)
{
	INT32				lVisualInfoIndex, lEquipType, lEffectIndex, lParentNodeID;
	UINT32				ulEID, ulStartGap;
	FLOAT				fOffsetX, fOffsetY, fOffsetZ, fScale;

	sscanf(szData, "%d:%d:%d:%d:%f:%f:%f:%f:%d:%d",	&lVisualInfoIndex,
													&lEquipType,
													&lEffectIndex,
													&ulEID,
													&fOffsetX,
													&fOffsetY,
													&fOffsetZ,
													&fScale,
													&lParentNodeID,
													&ulStartGap		);

	eAcReturnType	eRt		= CheckVisualInfo(pstAgcdCharacterTemplate, pcsAttachedData, lVisualInfoIndex);
	if (eRt != E_AC_RETURN_TYPE_SUCCESS)
		return eRt;

	lEquipType	= m_pcsAgcmCharacter->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lEquipType);
	if (lEquipType == -2)
	{
		return E_AC_RETURN_TYPE_SKIP;
	}
	else if (lEquipType == -1)
	{
		ASSERT(!"ERROR!");
		return E_AC_RETURN_TYPE_FAILURE;
	}

/*	if (!pstInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex])
	{
		pstInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex] = m_csEffectDataAdmin.CreateData();

		if(!pstInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex])
		{
			OutputDebugString("AgcmSkill::SetSkillEffectData() Error (1) !!!\n");
			return FALSE;
		}
	}

	pstInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex]->m_ulEID			= ulEID;
	pstInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex]->m_v3dOffset.x	= fOffsetX;
	pstInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex]->m_v3dOffset.y	= fOffsetY;
	pstInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex]->m_v3dOffset.z	= fOffsetZ;
	pstInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex]->m_fScale			= fScale;
	pstInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex]->m_lParentNodeID	= lParentNodeID;
	pstInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex]->m_ulStartGap		= ulStartGap;*/

	/*AgcdUseEffectSetData	*pcsEData	= m_csEffectDataAdmin2.GetData(pstInfo[lUsableIndex].m_astEffect + lEquipType, lEffectIndex, TRUE);
	if (!pcsEData)
		return FALSE;*/

	if (!pcsAttachedData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lEquipType])
		pcsAttachedData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lEquipType] = new AgcdUseEffectSet();

	if (!pcsAttachedData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lEquipType])
	{
		ASSERT(!"!pcsAttachedData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lEquipType]");
		return E_AC_RETURN_TYPE_FAILURE;
	}

	AgcdUseEffectSetData	*pcsEData =
		m_csEffectDataAdmin2.GetData(
			pcsAttachedData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lEquipType],
			lEffectIndex,
			TRUE																				);

	if (!pcsEData)
	{
		ASSERT(!"!pcsEData");
		return E_AC_RETURN_TYPE_FAILURE;
	}

	pcsEData->m_ulEID			= ulEID;
	pcsEData->m_fScale			= fScale;
	pcsEData->m_lParentNodeID	= lParentNodeID;
	pcsEData->m_ulStartGap		= ulStartGap;

	if ((fOffsetX != 0.0f) || (fOffsetY != 0.0f) || (fOffsetZ != 0.0f))
	{
		pcsEData->m_pv3dOffset			= new RwV3d();

		pcsEData->m_pv3dOffset->x		= fOffsetX;
		pcsEData->m_pv3dOffset->y		= fOffsetY;
		pcsEData->m_pv3dOffset->z		= fOffsetZ;
	}

	return E_AC_RETURN_TYPE_SUCCESS;
}

/*BOOL AgcmSkill::SetSkillEffectSound(pstAgcmSkillVisualInfo pstInfo, CHAR *szData)
{
	INT32				lEquipType, lEffecIndex, lCustData;
	UINT32				ulConditionFlags;
	CHAR				szSoundName[256];

	sscanf(szData, "%d:%d:%d:%d:%s", &lEquipType, &lEffecIndex, &ulConditionFlags, &lCustData, szSoundName);

	if(!pstInfo->m_astEffect[lEquipType].m_astData[lEffecIndex])
	{
		pstInfo->m_astEffect[lEquipType].m_astData[lEffecIndex] = m_csEffectDataAdmin.CreateData();
		if(!pstInfo->m_astEffect[lEquipType].m_astData[lEffecIndex])
		{
			OutputDebugString("AgcmSkill::SetSkillEffectSound() Error (1) !!!\n");
			return FALSE;
		}
	}

	strcpy(pstInfo->m_astEffect[lEquipType].m_astData[lEffecIndex]->m_szSoundName, szSoundName);

	pstInfo->m_astEffect[lEquipType].m_astData[lEffecIndex]->m_ulConditionFlags	= ulConditionFlags;
//	pstInfo->m_astEffect[lEquipType].m_astData[lEffecIndex]->m_lCustData		= lCustData;

	return TRUE;
}*/

/******************************************************************************
* Purpose :
*
* 061203. Bob Jung
******************************************************************************/
/*BOOL AgcmSkill::SetLevelInfo(pstAgcmSkillVisualInfo pstInfo, CHAR *szData)
{
	INT32 lLevelIndex, lStartLevel;

	sscanf(szData, "%d:%d", &lLevelIndex, &lStartLevel);

	pstInfo->m_alLevel[lLevelIndex] = lStartLevel;

	return TRUE;
}*/

BOOL AgcmSkill::CBActionSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData) || (!pCustData))
		return FALSE;

	AgcmSkill				*pThis				= (AgcmSkill *)(pClass);
	AgpdCharacter			*pcsAgpdCharacter	= (AgpdCharacter *)(pData);
	AgpdCharacterAction		*pstAction			= (AgpdCharacterAction *) pCustData;

	// Cast Skill을 호출해준다.

	ApBase					*pcsBase			= pThis->m_pcsAgpmSkill->GetBase(pstAction->m_csTargetBase.m_eType, pstAction->m_csTargetBase.m_lID);
	if (!pcsBase)
		return FALSE;

	return pThis->CastSkill(pstAction->m_lUserData[0], pcsBase, NULL, (BOOL) pstAction->m_lUserData[1]);
}

/******************************************************************************
* Purpose : Callback을 등록한다.
*
* 061503. Bob Jung
******************************************************************************/
BOOL AgcmSkill::SetCallbackNoAnimSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_NO_ANIM_SKILL, pfCallback, pClass);
}

BOOL AgcmSkill::SetCallbackMissCastSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_MISS_CAST_SKILL, pfCallback, pClass);
}

/*BOOL AgcmSkill::SetCallbackTargetSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_TARGET_SKILL, pfCallback, pClass);
}*/

BOOL AgcmSkill::SetCallbackCheckCastSkillResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_CHECK_CAST_SKILL_RESULT, pfCallback, pClass);
}

//BOOL AgcmSkill::SetCallbackMasteryUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(AGCMSKILL_CB_ID_MASTERY_UPDATE, pfCallback, pClass);
//}
//
//BOOL AgcmSkill::SetCallbackMasteryRollbackSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(AGCMSKILL_CB_ID_MASTERY_ROLLBACK_SUCCESS, pfCallback, pClass);
//}
//
//BOOL AgcmSkill::SetCallbackMasteryRollbackFail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(AGCMSKILL_CB_ID_MASTERY_ROLLBACK_FAILED, pfCallback, pClass);
//}

BOOL AgcmSkill::SetCallbackAddBuffedSkillList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_ADD_BUFFED_SKILL_LIST, pfCallback, pClass);
}

BOOL AgcmSkill::SetCallbackRemoveBuffedSkillList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_REMOVE_BUFFED_SKILL_LIST, pfCallback, pClass);
}

BOOL AgcmSkill::SetCallbackAddtionalHitEffect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_ADDITIONAL_HITEFFECT, pfCallback, pClass);
};

BOOL AgcmSkill::SetCallbackAddHpBar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_ADD_HPBAR, pfCallback, pClass);
}

BOOL AgcmSkill::SetCallbackCastFailForTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_CAST_FAIL_FOR_TRANSFORM, pfCallback, pClass);
}

BOOL AgcmSkill::SetCallbackCastFailForRide(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_CAST_FAIL_FOR_RIDE, pfCallback, pClass);
}

BOOL AgcmSkill::SetCallbackPreProcessSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_PRE_PROCESS_SKILL, pfCallback, pClass);
}

BOOL AgcmSkill::SetCallbackGetRegisteredMPPotionTID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_GET_REGISTERED_MP_POTION_TID, pfCallback, pClass);
}

BOOL AgcmSkill::SetCallbackBlockCastSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_BLOCK_CAST_SKILL, pfCallback, pClass);
}

BOOL AgcmSkill::SetCallbackStartGroundTargetSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_START_GROUND_TARGET, pfCallback, pClass);
}

BOOL AgcmSkill::UpdateSkillPoint(INT32 lSkillID, INT32 lPoint)
{
	return UpdateSkillPoint(m_pcsAgpmSkill->GetSkill(lSkillID), lPoint);
}

BOOL AgcmSkill::UpdateSkillPoint(AgpdSkill *pcsSkill, INT32 lPoint)
{
	if (!pcsSkill || lPoint == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket = m_pcsAgpmSkill->MakePacketUpdateSkillPoint(pcsSkill, &nPacketLength, lPoint);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmSkill::UpdateDIRTPoint(INT32 lSkillID, INT32 lDuration, INT32 lIntensity, INT32 lRange, INT32 lTarget)
{
	return UpdateDIRTPoint(m_pcsAgpmSkill->GetSkill(lSkillID), lDuration, lIntensity, lRange, lTarget);
}

BOOL AgcmSkill::UpdateDIRTPoint(AgpdSkill *pcsSkill, INT32 lDuration, INT32 lIntensity, INT32 lRange, INT32 lTarget)
{
	if (!pcsSkill)
		return FALSE;

	// 다 합친게 100% 여야 한당. (음수도 있음 안된당)
	INT32	lTotalPoint = abs(lDuration) + abs(lIntensity) + abs(lRange) + abs(lTarget);
	if (lTotalPoint != 100)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pcsAgpmSkill->MakePacketUpdateDIRTPoint(pcsSkill, &nPacketLength, lDuration, lIntensity, lRange, lTarget);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL	AgcmSkill::ConAgcdSkillTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)	return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	ApBase			*pcsBase		= (ApBase	*)		pData;

	AgcdSkillTemplate	*pcsAgcdSkillTemplate = pThis->GetADSkillTemplate(pcsBase);
	if (!pcsAgcdSkillTemplate)	return FALSE;

	memset(pcsAgcdSkillTemplate, 0, sizeof (AgcdSkillTemplate));

	return TRUE;
}

BOOL	AgcmSkill::DesAgcdSkillTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)	return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	ApBase			*pcsBase		= (ApBase	*)		pData;

	AgcdSkillTemplate	*pcsAgcdSkillTemplate = pThis->GetADSkillTemplate(pcsBase);
	if (!pcsAgcdSkillTemplate)	return FALSE;

	if (pcsAgcdSkillTemplate->m_pSkillTexture)
	{
		RwTextureDestroy(pcsAgcdSkillTemplate->m_pSkillTexture);
		pcsAgcdSkillTemplate->m_pSkillTexture = NULL;
	}

	if (pcsAgcdSkillTemplate->m_pSmallSkillTexture)
	{
		RwTextureDestroy(pcsAgcdSkillTemplate->m_pSmallSkillTexture);
		pcsAgcdSkillTemplate->m_pSmallSkillTexture = NULL;
	}

	if (pcsAgcdSkillTemplate->m_pUnableTexture)
	{
		RwTextureDestroy(pcsAgcdSkillTemplate->m_pUnableTexture);
		pcsAgcdSkillTemplate->m_pUnableTexture = NULL;
	}

	if (pcsAgcdSkillTemplate->m_pszTextureName)
	{
		delete [] pcsAgcdSkillTemplate->m_pszTextureName;
		pcsAgcdSkillTemplate->m_pszTextureName	= NULL;
	}

	if (pcsAgcdSkillTemplate->m_pszSmallTextureName)
	{
		delete [] pcsAgcdSkillTemplate->m_pszSmallTextureName;
		pcsAgcdSkillTemplate->m_pszSmallTextureName	= NULL;
	}

	if (pcsAgcdSkillTemplate->m_pszUnableTextureName)
	{
		delete [] pcsAgcdSkillTemplate->m_pszUnableTextureName;
		pcsAgcdSkillTemplate->m_pszUnableTextureName	= NULL;
	}

	return TRUE;
}

BOOL	AgcmSkill::ConAgcdSkillSpecialize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill					*pThis				= (AgcmSkill *)						pClass;
	AgpdSkillSpecializeTemplate	*pcsSpecialize		= (AgpdSkillSpecializeTemplate *)	pData;

	AgcdSkillSpecializeTemplate	*pcsAgcdSpecialize	= pThis->GetADSpecializeTemplate(pcsSpecialize);
	if (!pcsAgcdSpecialize)
		return FALSE;

	memset(pcsAgcdSpecialize, 0, sizeof (AgcdSkillSpecializeTemplate));

	/*pcsAgcdSpecialize->m_pTexture					= NULL;
	pcsAgcdSpecialize->m_pszTextureName				= NULL;*/
	//ZeroMemory(pcsAgcdSpecialize->m_szTextureName, sizeof(CHAR) * AGCDSKILL_TEXTURE_NAME_LENGTH);

	return TRUE;
}

BOOL	AgcmSkill::DesAgcdSkillSpecialize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill					*pThis				= (AgcmSkill *)						pClass;
	AgpdSkillSpecializeTemplate	*pcsSpecialize		= (AgpdSkillSpecializeTemplate *)	pData;

	AgcdSkillSpecializeTemplate	*pcsAgcdSpecialize	= pThis->GetADSpecializeTemplate(pcsSpecialize);
	if (!pcsAgcdSpecialize)
		return FALSE;

	if (pcsAgcdSpecialize->m_pTexture)
	{
		RwTextureDestroy(pcsAgcdSpecialize->m_pTexture);
		pcsAgcdSpecialize->m_pTexture = NULL;
	}

	if (pcsAgcdSpecialize->m_pszTextureName)
	{
		delete [] pcsAgcdSpecialize->m_pszTextureName;
		pcsAgcdSpecialize->m_pszTextureName	= NULL;
	}

	return TRUE;
}

/*****************************************************************
*   Function : SetDIRTWindow
*   Comment  : SetDIRTWindow
*   Date&Time : 2003-05-13, 오후 6:45
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
/*void	AgcmSkill::SetDIRTWindow( INT32 lSkillID )	
{
	if ( AP_INVALID_SKILLID == lSkillID ) return;
	
	// Skill Rater Set
	m_clSkill.m_clSkillDIRT.m_clSkillRaster.SetWindowRaster( GetSkillRaster( lSkillID ), 
		ACUISKILLDIRT_SKILL_WIDTH, ACUISKILLDIRT_SKILL_HEIGHT );

	// Init Buffer
	m_clSkill.m_clSkillDIRT.m_clBuffer.SetNumber( 0 );

	// DIRT Set
	m_clSkill.m_clSkillDIRT.SetDIRT(
		GetSkillFactor( lSkillID, AGPD_FACTORS_DIRT_TYPE_DURATION), 
		GetSkillFactor( lSkillID, AGPD_FACTORS_DIRT_TYPE_INTENSITY), 
		GetSkillFactor( lSkillID, AGPD_FACTORS_DIRT_TYPE_RANGE), 
		GetSkillFactor( lSkillID, AGPD_FACTORS_DIRT_TYPE_TARGET)	);
	
	m_clSkill.m_clSkillDIRT.SetDIRT( 100, 0, 0, 0 );

	// DIRT Window Open
	m_clSkill.BeExpand( TRUE );
}	*/

/*****************************************************************
*   Function  : SetSkillUIInfo
*   Comment   : Skill UI 에 Info를 Set한다. 
*   Date&Time : 2003-05-13 오전 11:08:55
*   Code By   : Seong Yon-jun@NHN Studio 
*****************************************************************/
/*void	AgcmSkill::SetSkillUIInfo()
{
	// Valid Check
	if ( NULL == m_pcsAgcmCharacter || NULL == m_pcsAgpmSkill ) return;
	else if ( NULL == m_pcsAgcmCharacter->m_pcsSelfCharacter ) return;
	
	AgpdSkillAttachData* pADSkillData = m_pcsAgpmSkill->GetAttachSkillData( (ApBase*)(m_pcsAgcmCharacter->m_pcsSelfCharacter) );
	if ( NULL == pADSkillData ) return;

	// Skill Object를 초기화 해준다. 
	m_clSkill.InitSkillObject();
	
	INT32 lSkillPoint = 0;

	// Layer 0을 그려준다. 
	if ( 0 == m_clSkill.m_cLayer )
	{
		for ( UINT8 i = 0 ; i < ACUISKILL_SKILLOBJECT_NUM ; i++ )
		{
			if ( AP_INVALID_SKILLID == pADSkillData->m_alSkillID[i] ) break;
			
			m_clSkill.SetSkillObject( i, pADSkillData->m_alSkillID[i], 
				GetSkillTexture( pADSkillData->m_alSkillID[i] ), GetSkillTotalPoint( pADSkillData->m_alSkillID[i] ) );	
		}
	}
	else if ( 1 == m_clSkill.m_cLayer )
	{
		for ( UINT8 i = ACUISKILL_SKILLOBJECT_NUM ; i < (ACUISKILL_SKILLOBJECT_NUM * 2) ; i++ )
		{
			if ( AP_INVALID_SKILLID == pADSkillData->m_alSkillID[i] ) break;
		
			m_clSkill.SetSkillObject( i - ACUISKILL_SKILLOBJECT_NUM, pADSkillData->m_alSkillID[i], 
				GetSkillTexture( pADSkillData->m_alSkillID[i] ), GetSkillTotalPoint( pADSkillData->m_alSkillID[i] ) );	
		}
	}

	// 남은 포인트 써주기 
	SetSkillPoint();
}	*/

/*****************************************************************
*   Function : SetSkillPoint
*   Comment  : SetSkillPoint
*   Date&Time : 2003-05-13, 오후 3:22
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
/*void		AgcmSkill::SetSkillPoint()								
{
	if ( NULL == m_pcsAgcmCharacter->m_pcsSelfCharacter )	return;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(
		&(m_pcsAgcmCharacter->m_pcsSelfCharacter->m_csFactor), AGPD_FACTORS_TYPE_RESULT);

	INT32 lSkillPoint = 0;

	m_pcsAgpmFactors->GetValue( pcsFactorResult, &lSkillPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_POINT );
	
	m_clSkill.SetSkillPoint( lSkillPoint );
}	*/

/*****************************************************************
*   Function : GetSkillRaster
*   Comment  : GetSkillRaster
*   Date&Time : 2003-05-13, 오후 2:36
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
RwRaster*	AgcmSkill::GetSkillRaster( INT32 lSkillID )
{
	AgcdSkillTemplate	*pcsAttachTemplateData = GetAgcdSkillTemplate( lSkillID );

	if ( NULL == pcsAttachTemplateData || NULL == pcsAttachTemplateData->m_pSkillTexture ) return NULL;

	return pcsAttachTemplateData->m_pSkillTexture->raster;
}

/*****************************************************************
*   Function  : GetSmallSkillRaster
*   Comment   : GetSmallSkillRaster
*   Date&Time : 2003-05-13 오후 3:01:47
*   Code By   : Seong Yon-jun@NHN Studio 
*****************************************************************/
RwRaster* AgcmSkill::GetSmallSkillRaster( INT32 lSkillID )
{
	AgcdSkillTemplate	*pcsAttachTemplateData = GetAgcdSkillTemplate( lSkillID );

	if ( NULL == pcsAttachTemplateData || NULL == pcsAttachTemplateData->m_pSmallSkillTexture ) return NULL;

	return pcsAttachTemplateData->m_pSmallSkillTexture->raster;
}

RwTexture*	AgcmSkill::GetSkillTexture( INT32 lSkillID )
{
	AgcdSkillTemplate	*pcsAttachTemplateData = GetAgcdSkillTemplate( lSkillID );

	if ( NULL == pcsAttachTemplateData || NULL == pcsAttachTemplateData->m_pSkillTexture ) return NULL;

	return pcsAttachTemplateData->m_pSkillTexture;
}

RwTexture*	AgcmSkill::GetSmallSkillTexture( AgpdSkillTemplate* pcsAgpdSkillTemplate )
{
	AgcdSkillTemplate	*pcsAttachTemplateData = GetADSkillTemplate( pcsAgpdSkillTemplate );
	if ( !pcsAttachTemplateData || !pcsAttachTemplateData->m_pSmallSkillTexture )		return NULL;

	return pcsAttachTemplateData->m_pSmallSkillTexture;
}

RwTexture*	AgcmSkill::GetBuffSkillSmallTexture( AgpdCharacter* pdCharacter, INT32 nNum, BOOL bBattleGround )
{
	if( !pdCharacter )			return NULL;
	if( 0 > nNum || nNum >= AGPMSKILL_MAX_SKILL_BUFF )	return NULL;
	if( !pdCharacter )			return NULL;

	AgpdSkillAttachData* pcsSkillAttachData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase*)pdCharacter);
	if( !pcsSkillAttachData )	return NULL;

	if( bBattleGround )
	{
		if( 954 != pcsSkillAttachData->m_astBuffSkillList[nNum].lSkillTID &&
			955 != pcsSkillAttachData->m_astBuffSkillList[nNum].lSkillTID &&
			956 != pcsSkillAttachData->m_astBuffSkillList[nNum].lSkillTID )
			return NULL;
	}

	AgpdSkillTemplate* pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pcsSkillAttachData->m_astBuffSkillList[nNum].lSkillTID);
	if( !pcsSkillTemplate )		return NULL;

	return GetSmallSkillTexture( pcsSkillTemplate );
}

BOOL AgcmSkill::SetGridSkillAttachedTexture( AgpdSkill* pcsAgpdSkill )
{
	if ( NULL == m_pcsAgcmUIControl || pcsAgpdSkill == NULL ) return FALSE;

	RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdSkill->m_pcsGridItem);
	if (!ppRwTexture)
		return FALSE;

	AgcdSkillTemplate	*pcsAgcdSkillTemplate	= GetADSkillTemplate((AgpdSkillTemplate *) pcsAgpdSkill->m_pcsTemplate);

	if (!pcsAgcdSkillTemplate->m_pSkillTexture)
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->SetTexturePath(m_szIconTexturePath);

		//if (!pcsAgcdSkillTemplate->m_pSkillTexture && pcsAgcdSkillTemplate->m_szTextureName[0])
		//	pcsAgcdSkillTemplate->m_pSkillTexture = RwTextureRead(pcsAgcdSkillTemplate->m_szTextureName, NULL);
		if ((pcsAgcdSkillTemplate->m_pszTextureName))
		{
			pcsAgcdSkillTemplate->m_pSkillTexture	= RwTextureRead(pcsAgcdSkillTemplate->m_pszTextureName, NULL);
			if (pcsAgcdSkillTemplate->m_pSkillTexture)
			{
				RwTextureSetFilterMode( pcsAgcdSkillTemplate->m_pSkillTexture, rwFILTERNEAREST );
				RwTextureSetAddressing( pcsAgcdSkillTemplate->m_pSkillTexture, rwTEXTUREADDRESSCLAMP );
			}
		}
	}

	if (!pcsAgcdSkillTemplate->m_pSkillTexture)
		return FALSE;

	*ppRwTexture				= pcsAgcdSkillTemplate->m_pSkillTexture;

	return TRUE;
}

BOOL AgcmSkill::SetGridSkillAttachedUnableTexture( AgpdSkill* pcsAgpdSkill )
{
	if ( NULL == m_pcsAgcmUIControl || pcsAgpdSkill == NULL ) return FALSE;

	RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdSkill->m_pcsGridItem);
	if (!ppRwTexture)
		return FALSE;

	AgcdSkillTemplate	*pcsAgcdSkillTemplate	= GetADSkillTemplate((AgpdSkillTemplate *) pcsAgpdSkill->m_pcsTemplate);

	if (!pcsAgcdSkillTemplate->m_pUnableTexture)
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->SetTexturePath(m_szIconTexturePath);

		//if (!pcsAgcdSkillTemplate->m_pSkillTexture && pcsAgcdSkillTemplate->m_szTextureName[0])
		//	pcsAgcdSkillTemplate->m_pSkillTexture = RwTextureRead(pcsAgcdSkillTemplate->m_szTextureName, NULL);
		if ((pcsAgcdSkillTemplate->m_pszUnableTextureName))
		{
			pcsAgcdSkillTemplate->m_pUnableTexture	= RwTextureRead(pcsAgcdSkillTemplate->m_pszUnableTextureName, NULL);
			if (pcsAgcdSkillTemplate->m_pUnableTexture)
			{
				RwTextureSetFilterMode( pcsAgcdSkillTemplate->m_pUnableTexture, rwFILTERNEAREST );
				RwTextureSetAddressing( pcsAgcdSkillTemplate->m_pUnableTexture, rwTEXTUREADDRESSCLAMP );
			}
		}
	}

	if (!pcsAgcdSkillTemplate->m_pUnableTexture)
		return FALSE;

	*ppRwTexture				= pcsAgcdSkillTemplate->m_pUnableTexture;

	return TRUE;
}

BOOL AgcmSkill::SetGridSkillAttachedUnableTexture( AgpdSkillTemplate* pcsAgpdSkillTemplate )
{
	if ( NULL == m_pcsAgcmUIControl || pcsAgpdSkillTemplate == NULL ) return FALSE;

	RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdSkillTemplate->m_pcsGridItem);
	if (!ppRwTexture)
		return FALSE;

	AgcdSkillTemplate	*pcsAgcdSkillTemplate	= GetADSkillTemplate(pcsAgpdSkillTemplate);

	if (!pcsAgcdSkillTemplate->m_pUnableTexture)
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->SetTexturePath(m_szIconTexturePath);

		//if (!pcsAgcdSkillTemplate->m_pSkillTexture && pcsAgcdSkillTemplate->m_szTextureName[0])
		//	pcsAgcdSkillTemplate->m_pSkillTexture = RwTextureRead(pcsAgcdSkillTemplate->m_szTextureName, NULL);
		if ((pcsAgcdSkillTemplate->m_pszUnableTextureName))
		{
			pcsAgcdSkillTemplate->m_pUnableTexture	= RwTextureRead(pcsAgcdSkillTemplate->m_pszUnableTextureName, NULL);
			if (pcsAgcdSkillTemplate->m_pUnableTexture)
			{
				RwTextureSetFilterMode( pcsAgcdSkillTemplate->m_pUnableTexture, rwFILTERNEAREST );
				RwTextureSetAddressing( pcsAgcdSkillTemplate->m_pUnableTexture, rwTEXTUREADDRESSCLAMP );
			}
		}
	}

	if (!pcsAgcdSkillTemplate->m_pUnableTexture)
		return FALSE;

	*ppRwTexture				= pcsAgcdSkillTemplate->m_pUnableTexture;

	return TRUE;
}

BOOL AgcmSkill::SetGridSkillAttachedSmallTexture( AgpdSkillTemplate* pcsAgpdSkillTemplate )
{
	if (!pcsAgpdSkillTemplate || !m_pcsAgcmUIControl)
		return FALSE;

	RwTexture**	ppRwTexture		= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdSkillTemplate->m_pcsGridItemAlarm);
	if (!ppRwTexture)
		return FALSE;

	AgcdSkillTemplate	*pcsAgcdSkillTemplate	= GetADSkillTemplate(pcsAgpdSkillTemplate);

	if (!pcsAgcdSkillTemplate->m_pSmallSkillTexture)
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->SetTexturePath(m_szIconTexturePath);

		//if (!pcsAgcdSkillTemplate->m_pSmallSkillTexture && pcsAgcdSkillTemplate->m_szSmallTextureName[0])
		//	pcsAgcdSkillTemplate->m_pSmallSkillTexture = RwTextureRead(pcsAgcdSkillTemplate->m_szSmallTextureName, NULL);
		if ((pcsAgcdSkillTemplate->m_pszSmallTextureName))
		{
			pcsAgcdSkillTemplate->m_pSmallSkillTexture	= RwTextureRead(pcsAgcdSkillTemplate->m_pszSmallTextureName, NULL);
			if (pcsAgcdSkillTemplate->m_pSmallSkillTexture)
			{
				RwTextureSetFilterMode( pcsAgcdSkillTemplate->m_pSmallSkillTexture, rwFILTERNEAREST );
				RwTextureSetAddressing( pcsAgcdSkillTemplate->m_pSmallSkillTexture, rwTEXTUREADDRESSCLAMP );
			}
		}
	}

	if (!pcsAgcdSkillTemplate->m_pSmallSkillTexture)
		return FALSE;
		
	*ppRwTexture				= pcsAgcdSkillTemplate->m_pSmallSkillTexture;

	return TRUE;
}

BOOL AgcmSkill::SetGridSkillAttachedTexture( AgpdSkillTemplate* pcsAgpdSkillTemplate )
{
	if ( NULL == m_pcsAgcmUIControl || pcsAgpdSkillTemplate == NULL ) return FALSE;

	RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdSkillTemplate->m_pcsGridItem);
	if (!ppRwTexture)
		return FALSE;
	
	AgcdSkillTemplate	*pcsAgcdSkillTemplate	= GetADSkillTemplate(pcsAgpdSkillTemplate);

	if (!pcsAgcdSkillTemplate->m_pSkillTexture)
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->SetTexturePath(m_szIconTexturePath);

		//if (!pcsAgcdSkillTemplate->m_pSkillTexture && pcsAgcdSkillTemplate->m_szTextureName[0])
		//	pcsAgcdSkillTemplate->m_pSkillTexture = RwTextureRead(pcsAgcdSkillTemplate->m_szTextureName, NULL);

		if ((pcsAgcdSkillTemplate->m_pszTextureName))
		{
			pcsAgcdSkillTemplate->m_pSkillTexture = RwTextureRead(pcsAgcdSkillTemplate->m_pszTextureName, NULL);
			if (pcsAgcdSkillTemplate->m_pSkillTexture)
			{
				RwTextureSetFilterMode( pcsAgcdSkillTemplate->m_pSkillTexture, rwFILTERNEAREST );
				RwTextureSetAddressing( pcsAgcdSkillTemplate->m_pSkillTexture, rwTEXTUREADDRESSCLAMP );
			}
		}
	}

	if (!pcsAgcdSkillTemplate->m_pSkillTexture)
		return FALSE;	

	*ppRwTexture			= pcsAgcdSkillTemplate->m_pSkillTexture;

	return TRUE;		
}

BOOL AgcmSkill::SetGridSpecializeAttachedTexture( AgpdSkillSpecializeTemplate *pcsSpecialize )
{
	if (!m_pcsAgcmUIControl || !pcsSpecialize) return FALSE;

	RwTexture**	ppRwTexture		= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsSpecialize->m_pcsGridItem);
	if (!ppRwTexture)
		return FALSE;

	AgcdSkillSpecializeTemplate	*pcsAgcdSpecialize	= GetADSpecializeTemplate(pcsSpecialize);
	if (!pcsAgcdSpecialize)
		return FALSE;

	/*if (!pcsAgcdSpecialize->m_pTexture && pcsAgcdSpecialize->m_szTextureName[0])
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->SetTexturePath(m_szIconTexturePath);

		pcsAgcdSpecialize->m_pTexture = RwTextureRead(pcsAgcdSpecialize->m_szTextureName, NULL);
	}*/

	if ((pcsAgcdSpecialize->m_pszTextureName))
	{
		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->SetTexturePath(m_szIconTexturePath);

		pcsAgcdSpecialize->m_pTexture = RwTextureRead(pcsAgcdSpecialize->m_pszTextureName, NULL);
		if (pcsAgcdSpecialize->m_pTexture)
		{
			RwTextureSetFilterMode( pcsAgcdSpecialize->m_pTexture, rwFILTERNEAREST );
			RwTextureSetAddressing( pcsAgcdSpecialize->m_pTexture, rwTEXTUREADDRESSCLAMP );
		}
	}

	*ppRwTexture				= pcsAgcdSpecialize->m_pTexture;

	return TRUE;
}

/*****************************************************************
*   Function  : GetSkillFactor
*   Comment   : GetSkillFactor
*   Date&Time : 2003-05-13 오후 3:01:54
*   Code By   : Seong Yon-jun@NHN Studio 
*****************************************************************/
INT32	AgcmSkill::GetSkillFactor( INT32 lSkillID, AgpdFactorDIRTType eDIRT )
{
	AgpdSkill* ppdSkill = m_pcsAgpmSkill->GetSkill( lSkillID );

	if ( NULL == ppdSkill ) return 0;

	INT32 lValue = 0;

	m_pcsAgpmFactors->GetValue( &(ppdSkill->m_csFactor), &lValue, AGPD_FACTORS_TYPE_DIRT, eDIRT );

	return lValue;
}
/*****************************************************************
*   Function  : GetSkillTotalPoint
*   Comment   : GetSkillTotalPoint
*   Date&Time : 2003-05-13 오후 3:10:48
*   Code By   : Seong Yon-jun@NHN Studio 
*****************************************************************/
INT32	AgcmSkill::GetSkillTotalPoint( INT32 lSkillID	)	
{
	AgpdSkill* ppdSkill = m_pcsAgpmSkill->GetSkill( lSkillID );

	if ( NULL == ppdSkill ) return 0;
	
	INT32 lSkillLevel = 0;
	INT32 lSkillPoint = 0;

	m_pcsAgpmFactors->GetValue( &(ppdSkill->m_csFactor), &lSkillLevel, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_LEVEL );
	m_pcsAgpmFactors->GetValue( &(ppdSkill->m_csFactor), &lSkillPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_POINT );

	for ( INT32 i = 0 ; i < lSkillLevel ; ++i )
		lSkillPoint += ( i + 1 );

	return lSkillPoint;
}

/*BOOL	AgcmSkill::CBUpdateSkillUIInfo(PVOID pData, PVOID pClass, PVOID pCustData)						
{
	AgcmSkill* pThis = (AgcmSkill*)pClass;

	pThis->SetSkillUIInfo();

	return TRUE;
}

BOOL	AgcmSkill::CBUpdateSkillPoint( PVOID pData, PVOID pClass, PVOID pCustData )						
{
	AgcmSkill*	pThis = (AgcmSkill*)pClass;

	pThis->SetSkillPoint();
	
	return TRUE;
}

BOOL	AgcmSkill::CSUpdateSkillDIRT( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmSkill*	pThis = (AgcmSkill*)pClass;
	
	return TRUE;
}	*/

BOOL AgcmSkill::CBLearnSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	ApBase			*pcsBase		= (ApBase *)		pData;
	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate((CHAR *) ppvBuffer[0]);
	if (!pcsSkillTemplate)
		return FALSE;

	AgpdSkill		*pcsSkill		= pThis->m_pcsAgpmSkill->CreateSkill();
	if (!pcsSkill)
		return FALSE;

	pcsSkill->m_lID					= pThis->m_lTempSkillID--;
	pcsSkill->m_pcsBase				= pcsBase;
	pcsSkill->m_pcsTemplate			= pcsSkillTemplate;

//	pcsSkill->m_lMasteryIndex		= *(INT32 *) ppvBuffer[2];
	pcsSkill->m_lActivedSkillPoint	= *(INT32 *) ppvBuffer[2];

	pcsSkill->m_pcsGridItem->m_eType				= AGPDGRID_ITEM_TYPE_SKILL;
	pcsSkill->m_pcsGridItem->m_lItemID				= pcsSkill->m_lID;
	pcsSkill->m_pcsGridItem->m_lItemTID				= pcsSkill->m_pcsTemplate->m_lID;

	if (!pThis->m_pcsAgpmSkill->AddSkill(pcsSkill))
	{
		pThis->m_pcsAgpmSkill->RemoveSkill(pcsSkill->m_lID);

		if (!pThis->m_pcsAgpmSkill->AddSkill(pcsSkill))
			return FALSE;
	}

	pThis->m_pcsAgpmSkill->UpdateSkillPoint(pcsSkill, pcsSkill->m_lActivedSkillPoint);

	pThis->m_pcsAgpmSkill->InitSkill(pcsSkill);

	/*
	AgpdSkillAttachData	*pcsAttachData	= pThis->m_pcsAgpmSkill->GetAttachSkillData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	for (int i = 0; i < AGPMSKILL_MAX_MASTERY_SKILL; ++i)
	{
		AgpdGridItem	*pcsTemplateItem
			= pThis->m_pcsAgpmGrid->GetItemByTemplate(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i],
													  AGPDGRID_ITEM_TYPE_SKILL,
													  pcsSkill->m_pcsTemplate->m_lID);
		if (pcsTemplateItem)
		{
			pThis->m_pcsAgpmGrid->DeleteItem(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], pcsTemplateItem);

			pThis->m_pcsAgpmGrid->Add(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], 0, ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nGridRow, ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nGridColumn, pcsSkill->m_pcsGridItem, 1, 1);

			break;
		}
	}
	*/

	ppvBuffer[1]					= (PVOID) pcsSkill;

	for (int i = 0; i < AGCMSKILL_MAX_TEMP_CREATED_SKILL; ++i)
	{
		if (pThis->m_lTempCreatedSkillID[i] == 0)
		{
			pThis->m_lTempCreatedSkillID[i] = pcsSkill->m_lID;

			break;
		}
	}

	return TRUE;
}

BOOL AgcmSkill::CBForgetSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	ApBase			*pcsBase		= (ApBase *)		pData;
	AgpdSkill		*pcsSkill		= (AgpdSkill *)		pCustData;

	/*
	AgpdSkillAttachData	*pcsAttachData	= pThis->m_pcsAgpmSkill->GetAttachSkillData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	for (int i = 0; i < AGPMSKILL_MAX_MASTERY_SKILL; ++i)
	{
		INT16	lLayer;
		INT16	lRow;
		INT16	lColumn;

		AgpdGridItem	*pcsTemplateItem
			= pThis->m_pcsAgpmGrid->GetItemByTemplate(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i],
													  AGPDGRID_ITEM_TYPE_SKILL,
													  pcsSkill->m_pcsTemplate->m_lID);
		if (pcsTemplateItem)
		{
			pThis->m_pcsAgpmGrid->DeleteItem(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], pcsTemplateItem);

			pThis->m_pcsAgpmGrid->Add(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], 0, ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nGridRow, ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nGridColumn, ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_pcsGridItem, 1, 1);

			break;
		}
	}
	*/

	for (int i = 0; i < AGCMSKILL_MAX_TEMP_CREATED_SKILL; ++i)
	{
		if (pThis->m_lTempCreatedSkillID[i] == pcsSkill->m_lID)
		{
			pThis->m_lTempCreatedSkillID[i] = 0;

			break;
		}
	}

	pThis->m_pcsAgpmSkill->RemoveSkill(pcsSkill->m_lID);

	return TRUE;
}

BOOL AgcmSkill::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdSkillAttachData			*	pcsAttachSkillData	= pThis->m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);
//	AgpdSkillMasteryTemplate	*	pcsMasteryTemplate	= pThis->m_pcsAgpmSkill->GetMasteryTemplate((ApBase *) pcsCharacter);
//	if (!pcsMasteryTemplate)
//		return FALSE;

	AgpdSkillTemplateAttachData *	pstAttachTemplateData = pThis->m_pcsAgpmSkill->GetAttachSkillTemplateData(pcsCharacter->m_pcsCharacterTemplate);
	AgpdSkillTemplate *				pcsSkillTemplate;
	AgcdSkillTemplate *				pstAgcdSkillTemplate;
	INT32	lIndex;

	for (lIndex = 0; lIndex < AGPMSKILL_MAX_SKILL_OWN; ++lIndex)
	{
		pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(pstAttachTemplateData->m_aszUsableSkillTName[lIndex]);
		if (!pcsSkillTemplate)
			continue;

		pstAgcdSkillTemplate = pThis->GetADSkillTemplate(pcsSkillTemplate);

		if (pThis->m_pcsAgcmResourceLoader)
			pThis->m_pcsAgcmResourceLoader->SetTexturePath(pThis->m_szIconTexturePath);

		// Read Textures if skill is self skill
		/*if (!pstAgcdSkillTemplate->m_pSkillTexture && pstAgcdSkillTemplate->m_szTextureName[0])
			pstAgcdSkillTemplate->m_pSkillTexture = RwTextureRead(pstAgcdSkillTemplate->m_szTextureName, NULL);

		if (!pstAgcdSkillTemplate->m_pSmallSkillTexture && pstAgcdSkillTemplate->m_szSmallTextureName[0])
			pstAgcdSkillTemplate->m_pSmallSkillTexture = RwTextureRead(pstAgcdSkillTemplate->m_szSmallTextureName, NULL);

		if (!pstAgcdSkillTemplate->m_pUnableTexture && pstAgcdSkillTemplate->m_szUnableTextureName[0])
			pstAgcdSkillTemplate->m_pUnableTexture = RwTextureRead(pstAgcdSkillTemplate->m_szUnableTextureName, NULL);*/

		if (	(!pstAgcdSkillTemplate->m_pSkillTexture) &&
				(pstAgcdSkillTemplate->m_pszTextureName)	)
		{
			pstAgcdSkillTemplate->m_pSkillTexture = RwTextureRead(pstAgcdSkillTemplate->m_pszTextureName, NULL);
			if (pstAgcdSkillTemplate->m_pSkillTexture)
			{
				RwTextureSetFilterMode( pstAgcdSkillTemplate->m_pSkillTexture, rwFILTERNEAREST );
				RwTextureSetAddressing( pstAgcdSkillTemplate->m_pSkillTexture, rwTEXTUREADDRESSCLAMP );
			}
		}

		if (	(!pstAgcdSkillTemplate->m_pSmallSkillTexture) &&
				(pstAgcdSkillTemplate->m_pszSmallTextureName)		)
		{
			pstAgcdSkillTemplate->m_pSmallSkillTexture = RwTextureRead(pstAgcdSkillTemplate->m_pszSmallTextureName, NULL);
			if (pstAgcdSkillTemplate->m_pSmallSkillTexture)
			{
				RwTextureSetFilterMode( pstAgcdSkillTemplate->m_pSmallSkillTexture, rwFILTERNEAREST );
				RwTextureSetAddressing( pstAgcdSkillTemplate->m_pSmallSkillTexture, rwTEXTUREADDRESSCLAMP );
			}
		}

		if (	(!pstAgcdSkillTemplate->m_pUnableTexture) &&
				(pstAgcdSkillTemplate->m_pszUnableTextureName)	)
		{
			pstAgcdSkillTemplate->m_pUnableTexture = RwTextureRead(pstAgcdSkillTemplate->m_pszUnableTextureName, NULL);
			if (pstAgcdSkillTemplate->m_pUnableTexture)
			{
				RwTextureSetFilterMode( pstAgcdSkillTemplate->m_pUnableTexture, rwFILTERNEAREST );
				RwTextureSetAddressing( pstAgcdSkillTemplate->m_pUnableTexture, rwTEXTUREADDRESSCLAMP );
			}
		}

		pThis->SetGridSkillAttachedTexture(pcsSkillTemplate);
	}

//#ifdef	__NEW_MASTERY__
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		pThis->m_pcsAgpmGrid->Init(&pcsAttachSkillData->m_csMasterySpecializeGrid[i], 1, 1, AGPMSKILL_MAX_CONST_SPECIALIZE_IN_MASTERY + AGPMSKILL_MAX_INSTANT_SPECIALIZE_IN_MASTERY);
//		pcsAttachSkillData->m_csMasterySpecializeGrid[i].m_lGridType	= AGPDGRID_ITEM_TYPE_SPECIALIZE;
//
//		BOOL	bInit	= FALSE;
//		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
//		{
//			INT32	lMaxDepth	= 0;
//			INT32	lMaxWidth	= 0;
//
//			INT32	lCurrentDepth	= 0;
//			INT32	lCurrentWidth	= 0;
//
//			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
//			{
//				if (pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_ucDepth == 0 &&
//					pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_lSkillTID == AP_INVALID_SKILLID)
//					break;
//
//				if (lMaxDepth < pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_ucDepth)
//					lMaxDepth	= pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_ucDepth;
//
//				if (lCurrentDepth == pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_ucDepth)
//				{
//					++lCurrentWidth;
//				}
//				else
//				{
//					if (lMaxWidth < lCurrentWidth)
//						lMaxWidth	= lCurrentWidth;
//
//					lCurrentDepth	= pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_ucDepth;
//					lCurrentWidth	= 1;
//				}
//			}
//
//			if (lMaxDepth > 0 &&
//				lMaxWidth > 0)
//			{
//				pThis->m_pcsAgpmGrid->Init(&pcsAttachSkillData->m_csMasteryGrid[i][j], 1, lMaxWidth, lMaxDepth + 1);
//				pcsAttachSkillData->m_csMasteryGrid[i][j].m_lGridType		= AGPDGRID_ITEM_TYPE_SKILL;
//			}
//		}
//	}
//
//	for (i = 0; i < pcsMasteryTemplate->m_ucNumMastery; ++i)
//	{
//		INT8	cSpecialize = pcsAttachSkillData->m_csMastery[i].cConstSpecialization | pcsAttachSkillData->m_csMastery[i].cInstantSpecialization;
//		INT32	lNumSpecialize	= AGPMSKILL_MAX_CONST_SPECIALIZE_IN_MASTERY + AGPMSKILL_MAX_INSTANT_SPECIALIZE_IN_MASTERY - 1;
//		for (int j = 1; j < AGPMSKILL_MASTERY_SPECIALIZED_MAX; j = j << 1)
//		{
//			if (lNumSpecialize < 0)
//				break;
//
//			if (cSpecialize & j)
//			{
//				AgpdSkillSpecializeTemplate	*pcsSpecialize	= pThis->m_pcsAgpmSkill->GetSpecialize(j);
//				if (!pcsSpecialize)
//					continue;
//
//				pThis->m_pcsAgpmGrid->Add(&pcsAttachSkillData->m_csMasterySpecializeGrid[i], 0, 0, lNumSpecialize, pcsSpecialize->m_pcsGridItem, 1, 1);
//
//				--lNumSpecialize;
//			}
//		}
//
//		for (j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
//		{
//			if (pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][0].m_lSkillTID == AP_INVALID_SKILLID)
//				break;
//
//			INT32	lCurrentDepth	= 0;
//			INT32	lCurrentWidth	= 0;
//
//			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
//			{
//				if (lCurrentDepth != pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_ucDepth)
//				{
//					lCurrentDepth	= pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_ucDepth;
//					lCurrentWidth	= 0;
//				}
//
//				AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_lSkillTID);
//				if (!pcsSkillTemplate || !pcsSkillTemplate->m_pcsGridItem)
//					continue;
//
//				pThis->m_pcsAgpmGrid->Add(&pcsAttachSkillData->m_csMasteryGrid[i][j], 0, lCurrentWidth, lCurrentDepth, pcsSkillTemplate->m_pcsGridItem, 1, 1);
//
//				pcsSkillTemplate->m_nGridRow	= lCurrentWidth;
//				pcsSkillTemplate->m_nGridColumn	= lCurrentDepth;
//
//				++lCurrentWidth;
//			}
//		}
//	}
//
//#else
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		pThis->m_pcsAgpmGrid->Init(&pcsAttachSkillData->m_csMasterySpecializeGrid[i], 1, 1, AGPMSKILL_MAX_CONST_SPECIALIZE_IN_MASTERY + AGPMSKILL_MAX_INSTANT_SPECIALIZE_IN_MASTERY);
//		pcsAttachSkillData->m_csMasterySpecializeGrid[i].m_lGridType	= AGPDGRID_ITEM_TYPE_SPECIALIZE;
//
//		BOOL	bInit	= FALSE;
//		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
//		{
//			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
//			{
//				if (pcsMasteryTemplate->m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k].m_lNumSkill == 0)
//				{
//					if (k > 0)
//					{
//						pThis->m_pcsAgpmGrid->Init(&pcsAttachSkillData->m_csMasteryGrid[i][j], 1, pcsMasteryTemplate->m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k - 1].m_lNumSkill, k);
//						pcsAttachSkillData->m_csMasteryGrid[i][j].m_lGridType		= AGPDGRID_ITEM_TYPE_SKILL;
//					}
//
//					break;
//				}
//			}
//		}
//	}
//
//	for (i = 0; i < pcsMasteryTemplate->m_lNumMastery; ++i)
//	{
//		INT8	cSpecialize = pcsAttachSkillData->m_csMastery[i].cConstSpecialization | pcsAttachSkillData->m_csMastery[i].cInstantSpecialization;
//		INT32	lNumSpecialize	= AGPMSKILL_MAX_CONST_SPECIALIZE_IN_MASTERY + AGPMSKILL_MAX_INSTANT_SPECIALIZE_IN_MASTERY - 1;
//		for (int j = 1; j < AGPMSKILL_MASTERY_SPECIALIZED_MAX; j = j << 1)
//		{
//			if (lNumSpecialize < 0)
//				break;
//
//			if (cSpecialize & j)
//			{
//				AgpdSkillSpecializeTemplate	*pcsSpecialize	= pThis->m_pcsAgpmSkill->GetSpecialize(j);
//				if (!pcsSpecialize)
//					continue;
//
//				pThis->m_pcsAgpmGrid->Add(&pcsAttachSkillData->m_csMasterySpecializeGrid[i], 0, 0, lNumSpecialize, pcsSpecialize->m_pcsGridItem, 1, 1);
//
//				--lNumSpecialize;
//			}
//		}
//
//		for (j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
//		{
//			if (pcsMasteryTemplate->m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[0].m_lNumSkill == 0)
//				break;
//
//			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
//			{
//				for (int l = 0; l < pcsMasteryTemplate->m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k].m_lNumSkill; ++l)
//				{
//					AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(pcsMasteryTemplate->m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k].m_szSkillName[l]);
//					if (!pcsSkillTemplate || !pcsSkillTemplate->m_pcsGridItem)
//						continue;
//
//					pThis->m_pcsAgpmGrid->Add(&pcsAttachSkillData->m_csMasteryGrid[i][j], 0, l, k, pcsSkillTemplate->m_pcsGridItem, 1, 1);
//
//					pcsSkillTemplate->m_nGridRow	= l;
//					pcsSkillTemplate->m_nGridColumn	= k;
//				}
//			}
//		}
//	}
//
//#endif	//__NEW_MASTERY__
	
//	for (lIndex = 0; lIndex < AGPMSKILL_MAX_SKILL_OWN; ++lIndex)
//	{
//		AgpdSkill	*pcsSkill	= pThis->m_pcsAgpmSkill->GetSkill(pcsAttachSkillData->m_alSkillID[lIndex]);
//		if (!pcsSkill)
//			continue;
//
//		AgpdSkillTemplate	*pcsTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
//
//		for (int i = 0; i < AGPMSKILL_MAX_MASTERY_SKILL; ++i)
//		{
//			if (pThis->m_pcsAgpmGrid->IsExistItemTemplate(&pcsAttachSkillData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], AGPDGRID_ITEM_TYPE_SKILL, pcsTemplate->m_lID))
//			{
//				pThis->m_pcsAgpmGrid->Clear(&pcsAttachSkillData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], 0, pcsTemplate->m_nGridRow, pcsTemplate->m_nGridColumn, 1, 1);
//
//				pThis->m_pcsAgpmGrid->Add(&pcsAttachSkillData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], 0, pcsTemplate->m_nGridRow, pcsTemplate->m_nGridColumn, pcsSkill->m_pcsGridItem, 1, 1);
//
//				break;
//			}
//		}
//	}

//	return pThis->RefreshNeedSkillPoint((ApBase *) pcsCharacter);

	return TRUE;
}

//BOOL AgcmSkill::CBMasteryRollbackResult(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
//	ApBase			*pcsBase		= (ApBase *)		pData;
//	INT8			cResult			= *(INT8 *)			pCustData;
//
//	switch (cResult) {
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK_SUCCESS:
//		{
//			pThis->m_pcsAgpmSkill->ResetAllMastery(pcsBase);
//			pThis->EnumCallback(AGCMSKILL_CB_ID_MASTERY_UPDATE, pcsBase, NULL);
//			pThis->EnumCallback(AGCMSKILL_CB_ID_MASTERY_ROLLBACK_SUCCESS, pcsBase, NULL);
//		}
//		break;
//
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK_FAIL:
//		{
//			pThis->EnumCallback(AGCMSKILL_CB_ID_MASTERY_ROLLBACK_FAILED, pcsBase, NULL);
//		}
//		break;
//	}
//
//	return TRUE;
//}
//
//BOOL AgcmSkill::RefreshNeedSkillPoint(ApBase *pcsBase)
//{
//	if (!pcsBase)
//		return FALSE;
//
//	AgpdSkillAttachData		*pcsAttachData	= m_pcsAgpmSkill->GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	AgpdSkillMasteryTemplate	*pcsMasteryTemplate	= &pcsAttachData->m_csMasteryTemplate;
//	if (!pcsMasteryTemplate)
//		return FALSE;
//
//#ifdef	__NEW_MASTERY__
//
//	for (int i = 0; i < pcsMasteryTemplate->m_ucNumMastery; ++i)
//	{
//		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
//		{
//			if (pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][0].m_lSkillTID == AP_INVALID_SKILLID)
//				break;
//
//			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
//			{
//				AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_lSkillTID);
//				if (!pcsSkillTemplate || !pcsSkillTemplate->m_pcsGridItem)
//					continue;
//
//				CHAR	szRightString[8];
//				ZeroMemory(szRightString, sizeof(CHAR) * 8);
//				sprintf(szRightString, "%d", pcsMasteryTemplate->m_csMastery[i].m_csSkillTree[j][k].m_ucSPForActive);
//
//				pcsSkillTemplate->m_pcsGridItem->SetRightBottomString(szRightString);
//			}
//		}
//	}
//
//#else
//
//	for (int i = 0; i < pcsMasteryTemplate->m_lNumMastery; ++i)
//	{
//		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
//		{
//			if (pcsMasteryTemplate->m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[0].m_lNumSkill == 0)
//				break;
//
//			for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
//			{
//				for (int l = 0; l < pcsMasteryTemplate->m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k].m_lNumSkill; ++l)
//				{
//					AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pcsMasteryTemplate->m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k].m_szSkillName[l]);
//					if (!pcsSkillTemplate || !pcsSkillTemplate->m_pcsGridItem)
//						continue;
//
//					CHAR	szRightString[8];
//					ZeroMemory(szRightString, sizeof(CHAR) * 8);
//					sprintf(szRightString, "%d", pcsMasteryTemplate->m_csMastery[i].m_stMasterySkillTree[j].m_csSkillNode[k].m_lNeedSP[l]);
//
//					pcsSkillTemplate->m_pcsGridItem->SetRightBottomString(szRightString);
//				}
//			}
//		}
//	}
//
//#endif	//__NEW_MASTERY__
//
//	return TRUE;
//}

BOOL AgcmSkill::CBInitSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill			*pThis			= (AgcmSkill *)			pClass;
	AgpdSkill			*pcsSkill		= (AgpdSkill *)			pData;

	if (!pcsSkill->m_pcsTemplate || !pcsSkill->m_pcsBase)
		return FALSE;

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (pcsSelfCharacter)
	{
		if (pcsSkill->m_pcsBase->m_eType	!= pcsSelfCharacter->m_eType ||
			pcsSkill->m_pcsBase->m_lID		!= pcsSelfCharacter->m_lID)
			return TRUE;
	}

	AgcdSkillTemplate *				pstAgcdSkillTemplate;

	pstAgcdSkillTemplate = pThis->GetADSkillTemplate(pcsSkill->m_pcsTemplate);

	if (pThis->m_pcsAgcmResourceLoader)
		pThis->m_pcsAgcmResourceLoader->SetTexturePath(pThis->m_szIconTexturePath);

	// Read Textures if skill is self skill
	/*if (!pstAgcdSkillTemplate->m_pSkillTexture && pstAgcdSkillTemplate->m_szTextureName[0])
		pstAgcdSkillTemplate->m_pSkillTexture = RwTextureRead(pstAgcdSkillTemplate->m_szTextureName, NULL);

	if (!pstAgcdSkillTemplate->m_pSmallSkillTexture && pstAgcdSkillTemplate->m_szSmallTextureName[0])
		pstAgcdSkillTemplate->m_pSmallSkillTexture = RwTextureRead(pstAgcdSkillTemplate->m_szSmallTextureName, NULL);

	if (!pstAgcdSkillTemplate->m_pUnableTexture && pstAgcdSkillTemplate->m_szUnableTextureName[0])
		pstAgcdSkillTemplate->m_pUnableTexture = RwTextureRead(pstAgcdSkillTemplate->m_szUnableTextureName, NULL);*/

	if (	(!pstAgcdSkillTemplate->m_pSkillTexture) &&
			(pstAgcdSkillTemplate->m_pszTextureName)	)
	{
		pstAgcdSkillTemplate->m_pSkillTexture = RwTextureRead(pstAgcdSkillTemplate->m_pszTextureName, NULL);
		if (pstAgcdSkillTemplate->m_pSkillTexture)
		{
			RwTextureSetFilterMode( pstAgcdSkillTemplate->m_pSkillTexture, rwFILTERNEAREST );
			RwTextureSetAddressing( pstAgcdSkillTemplate->m_pSkillTexture, rwTEXTUREADDRESSCLAMP );
		}
	}

	if (	(!pstAgcdSkillTemplate->m_pSmallSkillTexture) &&
			(pstAgcdSkillTemplate->m_pszSmallTextureName)		)
	{
		pstAgcdSkillTemplate->m_pSmallSkillTexture = RwTextureRead(pstAgcdSkillTemplate->m_pszSmallTextureName, NULL);
		if (pstAgcdSkillTemplate->m_pSmallSkillTexture)
		{
			RwTextureSetFilterMode( pstAgcdSkillTemplate->m_pSmallSkillTexture, rwFILTERNEAREST );
			RwTextureSetAddressing( pstAgcdSkillTemplate->m_pSmallSkillTexture, rwTEXTUREADDRESSCLAMP );
		}
	}

	if (	(!pstAgcdSkillTemplate->m_pUnableTexture) &&
			(pstAgcdSkillTemplate->m_pszUnableTextureName)	)
	{
		pstAgcdSkillTemplate->m_pUnableTexture = RwTextureRead(pstAgcdSkillTemplate->m_pszUnableTextureName, NULL);
		if (pstAgcdSkillTemplate->m_pUnableTexture)
		{
			RwTextureSetFilterMode( pstAgcdSkillTemplate->m_pUnableTexture, rwFILTERNEAREST );
			RwTextureSetAddressing( pstAgcdSkillTemplate->m_pUnableTexture, rwTEXTUREADDRESSCLAMP );
		}
	}

	pThis->SetGridSkillAttachedTexture(pcsSkill);
	pThis->SetGridSkillAttachedSmallTexture((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);

	CHAR	szLeftString[8];
	ZeroMemory(szLeftString, sizeof(CHAR) * 8);
	sprintf(szLeftString, "%d", pThis->m_pcsAgpmSkill->GetSkillPoint(pcsSkill));

	pcsSkill->m_pcsGridItem->SetLeftBottomString(szLeftString);

	//pThis->m_pcsAgpmSkill->AddOwnSkillList(pcsSkill);

	if (pThis->m_pcsAgpmSkill->IsPassiveSkill(pcsSkill))
	{
		pThis->m_pcsAgpmSkill->AddUsePassiveList(pcsSkill, pcsSkill->m_lID);

		pcsSkill->m_pcsGridItem->m_bMoveable		= FALSE;
	}
	else
		pcsSkill->m_pcsGridItem->m_bMoveable		= TRUE;

	AgpdSkillAttachData	*pcsAttachData	= pThis->m_pcsAgpmSkill->GetAttachSkillData(pcsSkill->m_pcsBase);
	if (!pcsAttachData)
		return FALSE;

	AgpdSkillTemplate	*pcsTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;

//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY_SKILL; ++i)
//	{
//		if (pThis->m_pcsAgpmGrid->IsExistItemTemplate(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], AGPDGRID_ITEM_TYPE_SKILL, pcsTemplate->m_lID))
//		{
//			pThis->m_pcsAgpmGrid->Clear(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], 0, pcsTemplate->m_nGridRow, pcsTemplate->m_nGridColumn, 1, 1);
//
//			pThis->m_pcsAgpmGrid->Add(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], 0, pcsTemplate->m_nGridRow, pcsTemplate->m_nGridColumn, pcsSkill->m_pcsGridItem, 1, 1);
//
//			break;
//		}
//	}

//	pThis->RefreshNeedSkillPoint(pcsSkill->m_pcsBase);

	return TRUE;
}

BOOL AgcmSkill::CBRemoveSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill			*pThis			= (AgcmSkill *)			pClass;
	AgpdSkill			*pcsSkill		= (AgpdSkill *)			pData;

	if (!pcsSkill->m_pcsTemplate || !pcsSkill->m_pcsBase)
		return FALSE;

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return TRUE;

	if (pcsSkill->m_pcsBase->m_eType	!= pcsSelfCharacter->m_eType ||
		pcsSkill->m_pcsBase->m_lID		!= pcsSelfCharacter->m_lID)
		return TRUE;

	pThis->m_pcsAgpmSkill->RemoveOwnSkillList(pcsSkill);

	if (pThis->m_pcsAgpmSkill->IsPassiveSkill(pcsSkill))
	{
		pThis->m_pcsAgpmSkill->RemoveUsePassiveList(pcsSkill, pcsSkill->m_lID);
	}

	AgpdSkillAttachData	*pcsAttachData	= pThis->m_pcsAgpmSkill->GetAttachSkillData(pcsSkill->m_pcsBase);
	if (!pcsAttachData)
		return FALSE;

//	AgpdSkillTemplate	*pcsTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY_SKILL; ++i)
//	{
//		if (pThis->m_pcsAgpmGrid->IsExistItem(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], AGPDGRID_ITEM_TYPE_SKILL, pcsSkill->m_lID))
//		{
//			pThis->m_pcsAgpmGrid->Clear(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], 0, pcsTemplate->m_nGridRow, pcsTemplate->m_nGridColumn, 1, 1);
//
//			pThis->m_pcsAgpmGrid->Add(&pcsAttachData->m_csMasteryGrid[pcsSkill->m_lMasteryIndex][i], 0, pcsTemplate->m_nGridRow, pcsTemplate->m_nGridColumn, pcsTemplate->m_pcsGridItem, 1, 1);
//
//			break;
//		}
//	}

	return TRUE;
}

BOOL AgcmSkill::CBInitSkillTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill			*pThis			= (AgcmSkill *)			pClass;
	AgpdSkillTemplate	*pcsTemplate	= (AgpdSkillTemplate *)	pData;

	//pThis->SetGridSkillAttachedTexture(pcsTemplate);

	return TRUE;
}

BOOL AgcmSkill::CBInitSpecialize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill					*pThis			= (AgcmSkill *)						pClass;
	AgpdSkillSpecializeTemplate	*pcsSpecialize	= (AgpdSkillSpecializeTemplate *)	pData;
	
	return pThis->SetGridSpecializeAttachedTexture(pcsSpecialize);
}

BOOL AgcmSkill::CBAddBuffedSkillList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill					*pThis				= (AgcmSkill *)						pClass;
	ApBase						*pcsBase			= (ApBase *)						pData;
	PVOID						*ppvBuffer			= (PVOID *)							pCustData;

	INT32						lBuffedTID			= (INT32)							ppvBuffer[0];
	INT32						lCasterTID			= (INT32)							ppvBuffer[1];
	BOOL						bIsOnlyUpdate		= (BOOL)							ppvBuffer[2];

	if (!bIsOnlyUpdate)
	{
		// 스킬 이펙트를 보여준다.
		pThis->EnumCallback(AGCMSKILL_CB_ID_ADD_BUFFED_SKILL_LIST, pData, pCustData);
	}

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	if (pcsBase->m_eType	!= ppdCharacter->m_eType ||
		pcsBase->m_lID		!= ppdCharacter->m_lID)
		return TRUE;

	AgpdSkillAttachData			*pcsAttachData		= pThis->m_pcsAgpmSkill->GetAttachSkillData(pcsBase);

	for (int i = 0; i < AGPMSKILL_MAX_SKILL_BUFF; ++i)
	{
		AgpdSkillTemplate		*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(pcsAttachData->m_astBuffSkillList[i].lSkillTID);
		if (!pcsSkillTemplate)
			break;

		if (pcsAttachData->m_astBuffSkillList[i].ulDurationTimeMSec > 0 &&
			pcsAttachData->m_astBuffSkillList[i].ulEndTimeMSec == 0)
		{
			pcsAttachData->m_astBuffSkillList[i].ulEndTimeMSec = pThis->GetPrevClockCount() + pcsAttachData->m_astBuffSkillList[i].ulDurationTimeMSec;
		}
	}

	return TRUE;
}

BOOL AgcmSkill::CBRemoveBuffedSkillList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill					*pThis				= (AgcmSkill *)						pClass;
	ApBase						*pcsBase			= (ApBase *)						pData;
	INT32						lBuffedTID			= (INT32)							pCustData;

	// 지속형 스킬 이펙트를 끈다.
	pThis->EnumCallback(AGCMSKILL_CB_ID_REMOVE_BUFFED_SKILL_LIST, pData, pCustData);

	return TRUE;
}

BOOL AgcmSkill::CBUpdateSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill					*pThis			= (AgcmSkill *)						pClass;
	AgpdSkill					*pcsSkill		= (AgpdSkill *)						pData;

	CHAR	szLeftString[8];
	ZeroMemory(szLeftString, sizeof(CHAR) * 8);
	sprintf(szLeftString, "%d", pThis->m_pcsAgpmSkill->GetSkillPoint(pcsSkill));

	pcsSkill->m_pcsGridItem->SetLeftBottomString(szLeftString);

	return TRUE;
}

BOOL AgcmSkill::CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill					*pThis			= (AgcmSkill *)						pClass;
	AgpdSkill					*pcsSkill		= (AgpdSkill *)						pData;

	CHAR	szLeftString[8];
	ZeroMemory(szLeftString, sizeof(CHAR) * 8);
	sprintf(szLeftString, "%d", pThis->m_pcsAgpmSkill->GetSkillPoint(pcsSkill));

	pcsSkill->m_pcsGridItem->SetLeftBottomString(szLeftString);

	return TRUE;
}

AgcdSkillTemplate*	AgcmSkill::GetAgcdSkillTemplate( INT32 lSkillID )
{
	AgpdSkill* pSkill = m_pcsAgpmSkill->GetSkill( lSkillID );

	if ( NULL == pSkill ) return NULL;
	
	AgpdSkillTemplate* pSkillTemplate = (AgpdSkillTemplate*)pSkill->m_pcsTemplate;

	if ( NULL == pSkillTemplate ) return NULL;

	AgcdSkillTemplate	*pcsAttachTemplateData = GetADSkillTemplate( pSkillTemplate );

	if ( NULL == pcsAttachTemplateData ) return NULL;

	return pcsAttachTemplateData;
}

// 2005.09.22. steeple
AgcdSkill* AgcmSkill::GetAgcdSkill(AgpdSkill* pcsSkill)
{
	if(!pcsSkill)
		return NULL;

	return (AgcdSkill*)m_pcsAgpmSkill->GetAttachedModuleData(m_nIndexADSkill, pcsSkill);
}

BOOL AgcmSkill::SkillTest(INT32 lSTID, INT32 lTCID, INT32 lLID)
{
/*	AgpdCharacter				*pcsOwner						= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsOwner)
		return FALSE;

	AgpdCharacterTemplate		*pcsOwnerTemplate				= (AgpdCharacterTemplate *)(pcsOwner->m_pcsCharacterTemplate);
	if (!pcsOwnerTemplate)
		return FALSE;

	AgcdSkillAttachTemplateData	*pstAgcdSkillAttachTemplateData	= GetAttachTemplateData((ApBase *)(pcsOwnerTemplate));
	if (!pstAgcdSkillAttachTemplateData)
		return FALSE;

	INT32						lUsableIndex					= GetUsableSkillTNameIndex((ApBase *)(pcsOwner->m_pcsCharacterTemplate), lSTID);
	if (lUsableIndex < 0)
		return FALSE;

	AgcdCharacter				*pcsAgcdOwner					= m_pcsAgcmCharacter->GetCharacterData(pcsOwner);
	if (!pcsAgcdOwner)
		return FALSE;

	AgcdSkill					*pcsAgcdSkill					= GetCharacterAttachData((ApBase *)(pcsOwner));
	if (!pcsAgcdSkill)
		return FALSE;

	// 스킬 인덱스를 저장한당...
//	pcsAgcdOwner->m_lSkillUsableIndex	= lUsableIndex;
//	pcsAgcdOwner->m_lSkillID			= 0;
//	pcsAgcdOwner->m_lSkillLevelIndex	= lLevelIndex;
	pcsAgcdSkill->m_lSkillUsableIndex	= lUsableIndex;
	pcsAgcdSkill->m_lSkillID			= 0;

//	memset(pcsAgcdOwner->m_alSkillTargetCID, 0, sizeof (INT32) * AGCD_CHARACTER_SKILL_MAX_TARGET);	
//	pcsAgcdOwner->m_alSkillTargetCID[0]	= ((ApBase *)(m_pcsAgcmCharacter->GetAgpmCharacter()->GetCharacter(lTCID)))->m_lID;
//	if(!pcsAgcdOwner->m_alSkillTargetCID[0])
//		return FALSE;

	ApBase	*pcsTarget					= m_pcsAgpmCharacter->GetCharacter(lTCID);
	if (!pcsTarget)
		return FALSE;

	memset(pcsOwner->m_alTargetCID, 0, sizeof (INT32) * AGPDCHARACTER_MAX_TARGET);
	pcsOwner->m_alTargetCID[0]			= pcsTarget->m_lID;

	if(pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[pcsAgcdOwner->m_lAnimHandEquipType])
	{
		// 스킬애니메이션 발동~~~
		m_pcsAgcmCharacter->StartAnimation(
			pcsOwner,
			AGCMCHAR_ANIM_TYPE_SKILL,
			pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[pcsAgcdOwner->m_lAnimHandEquipType]	);
	}
	else if(pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[AGCDITEM_EQUIP_ANIMATION_TYPE_HANDATTACK])
	{
		m_pcsAgcmCharacter->StartAnimation(
			pcsOwner,
			AGCMCHAR_ANIM_TYPE_SKILL,
			pstAgcdSkillAttachTemplateData->m_astSkillVisualInfo[lUsableIndex].m_pastAnim[AGCDITEM_EQUIP_ANIMATION_TYPE_HANDATTACK]	);
	}
	else
	{
		EnumCallbackNoAnimSkill((PVOID)(pcsOwner));
	}*/

	return TRUE;
}

/*BOOL AgcmSkill::SetMaxEffectNum(INT32 lMax)
{
	return m_csEffectDataAdmin.Initialize(lMax);
}*/

//BOOL AgcmSkill::CBMasteryChangeResult(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	return TRUE;
//}

BOOL AgcmSkill::EnumCallbackNoAnimSkill(PVOID pData, PVOID pCustData)
{
	return EnumCallback(AGCMSKILL_CB_ID_NO_ANIM_SKILL, pData, pCustData);
}

BOOL AgcmSkill::IsExistCastQueue(ApBase *pcsBase, INT32 lCasterID)
{
	if (!pcsBase || lCasterID == 0)
		return FALSE;

	AgcdSkillADChar		*pcsAgcdSkillADChar		= GetCharacterAttachData(pcsBase);
	if (!pcsAgcdSkillADChar)
		return FALSE;

	if (pcsAgcdSkillADChar->m_lNumCastSkill > 0)
	{
		for (int i = 0; i < pcsAgcdSkillADChar->m_lNumCastSkill; ++i)
		{
			if (pcsAgcdSkillADChar->m_stCastQueue[i].lCasterID == lCasterID)
				return TRUE;
		}
	}

	return FALSE;
}

BOOL AgcmSkill::AddCastQueue(ApBase *pcsBase, INT32 lCasterID)
{
	if (!pcsBase || lCasterID == 0)
		return FALSE;

	AgcdSkillADChar		*pcsAgcdSkillADChar		= GetCharacterAttachData(pcsBase);
	if (!pcsAgcdSkillADChar)
		return FALSE;

	if (pcsAgcdSkillADChar->m_lNumCastSkill >= AGCDSKILL_MAX_ACTION_QUEUE_SIZE - 1)
		return FALSE;

	pcsAgcdSkillADChar->m_stCastQueue[pcsAgcdSkillADChar->m_lNumCastSkill].lCasterID	= lCasterID;

	++pcsAgcdSkillADChar->m_lNumCastSkill;

	return TRUE;
}

BOOL AgcmSkill::RemoveCastQueue(ApBase *pcsBase, INT32 lCasterID)
{
	if (!pcsBase || lCasterID == 0)
		return FALSE;

	AgcdSkillADChar		*pcsAgcdSkillADChar		= GetCharacterAttachData(pcsBase);
	if (!pcsAgcdSkillADChar)
		return FALSE;

	if (pcsAgcdSkillADChar->m_lNumCastSkill <= 0)
		return FALSE;

	int i;
	for (i = 0; i < pcsAgcdSkillADChar->m_lNumCastSkill; ++i)
	{
		if (pcsAgcdSkillADChar->m_stCastQueue[i].lCasterID == lCasterID)
			break;
	}

	if (i == pcsAgcdSkillADChar->m_lNumCastSkill)
		return FALSE;

	CopyMemory(pcsAgcdSkillADChar->m_stCastQueue + i, pcsAgcdSkillADChar->m_stCastQueue + i + 1, sizeof(AgcmSkillCastQueue) * (AGCDSKILL_MAX_ACTION_QUEUE_SIZE - i - 1));

	--pcsAgcdSkillADChar->m_lNumCastSkill;

	return TRUE;
}

BOOL AgcmSkill::CBSkillStruckAnimation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	AgpdCharacter	*pcsAttacker	= (AgpdCharacter *)	pData;
	AgpdCharacter	*pcsTargetChar	= (AgpdCharacter *)	pCustData;

	return pThis->AddCastQueue((ApBase *) pcsTargetChar, pcsAttacker->m_lID);
}

BOOL AgcmSkill::CBProcessSkillStruck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	AgpdCharacter	*pcsAttacker	= (AgpdCharacter *)	pData;
	AgpdCharacter	*pcsTargetChar	= (AgpdCharacter *)	pCustData;

	return pThis->RemoveCastQueue((ApBase *) pcsTargetChar, pcsAttacker->m_lID);
}

VOID				AgcmSkill::SetIconTexturePath(CHAR *szPath)
{
	m_szIconTexturePath[0] = 0;

	if (szPath)
		strncat(m_szIconTexturePath, szPath, AGCMSKILL_PATH_LENGTH - 1);
}

BOOL AgcmSkill::CBReservedActionSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	/*
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgcdCharacter	*pcsAgcdCharacter	= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

	pThis->CastSkill(pcsAgcdCharacter->m_stNextAction.m_lUserData[0], (ApBase *) &pcsAgcdCharacter->m_stNextAction.m_csTargetBase, &pcsAgcdCharacter->m_stNextAction.m_stTargetPos, pcsAgcdCharacter->m_stNextAction.m_bForceAction);
	*/

	if (!pClass)
		return FALSE;

	AgcmSkill				*pThis				= (AgcmSkill *) pClass;

	AgpdCharacterAction		*pstNextAction		= pThis->m_pcsAgcmCharacter->GetNextAction();

	// Cast Skill을 호출해준다.
	ApBase					*pcsBase			= pThis->m_pcsAgpmSkill->GetBase(pstNextAction->m_csTargetBase.m_eType, pstNextAction->m_csTargetBase.m_lID);
	if (!pcsBase)
		return FALSE;

	return pThis->CastSkill(pstNextAction->m_lUserData[0], pcsBase, &pstNextAction->m_stTargetPos, (BOOL) pstNextAction->m_lUserData[1]);
}

BOOL AgcmSkill::SetCallbackStartCastSelfCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILL_CB_ID_START_CAST_SELF_CHARACTER, pfCallback, pClass);
}

// 2004.11.09. steeple
// Twice Packet Skill 이므로 두번 보내야 한다. 그 Event 를 Idle 에 넣는다.
BOOL AgcmSkill::AddIdleEventTwicePacketSkill(ApBase* pcsOwner, AgpdSkill* pcsSkill, UINT32 ulClock)
{
	if(!pcsOwner || !pcsSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;
	
	if(!pcsSkillTemplate->m_bTwicePacket)
		return FALSE;

	if(pcsOwner != (ApBase*)m_pcsAgcmCharacter->m_pcsSelfCharacter)
		return FALSE;

	AddIdleEvent(ulClock,//pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CAST_TIME][lSkillLevel],
				pcsSkill->m_lID,
				this,
				CBProcessTwicePacketSkill,
				NULL
				);

	return TRUE;
}

// 2004.11.09. steeple
// Cast 후 공격할 시점에 패킷을 한번 더 보낸다. AddIdleEvent 를 했기때문에 Idle 에서 불린다.
BOOL AgcmSkill::CBProcessTwicePacketSkill(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	if(!pClass)
		return FALSE;

	AgcmSkill* pThis = (AgcmSkill*)pClass;
	INT32 lSkillID = lCID;	// lCID 에 SkillID 가 넘어오게 된다.

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmSkill->MakePacketRealHit(&lSkillID,
								(ApBase*)pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter,
								&nPacketLength);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bRetval = pThis->SendPacket(pvPacket, nPacketLength);
	pThis->m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);
	
	return bRetval;
}

// 2005.08.24. steeple
// AnimationDuration 을 받아서 한방에 처리 후 리턴하는 것으로 변경.
//
// 2005.08.19. steeple
// Twice Packet 의 타입이 하나 늘어났다.
// 여태까지는 적과의 거리와 발사체의 속도에 따라서 결정이 되었으나,
// 일정 시간 (Velocity 에 쓰인 시간) 후에 바로 Twice Packet 이 날라가야 하는 것도 있다.
//
// 2005.03.08. steeple
// Skill_Const 에 veolocity 수치 세팅 되었음. 가져와서 하면 된다. 발사체의 속도에 따라서 달라지게 된다.
//
// 2005.01.16. steeple
// Twice Packet 보내는 시간 계산하기.
// 나중에 이거 Const 라던가 실제 값으로 얻어와야 한다.
FLOAT AgcmSkill::GetTwicePacketInteval(AgpdSkill* pcsSkill, ApBase* pcsOwner, FLOAT fAnimationDuration)
{
	if(!pcsSkill || !pcsSkill->m_pcsTemplate || !pcsOwner)
		return 0.0f;

	if(pcsOwner->m_eType != APBASE_TYPE_CHARACTER)
		return 0.0f;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return 0.0f;

	// 자기 자신이 아니면 안한다.
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pcsOwner;
	if(pcsCharacter != m_pcsAgcmCharacter->GetSelfCharacter())
		return 0.0f;

	AgcdCharacter* pcsAgcdCharacter = m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);
	if(!pcsAgcdCharacter)
		return 0.0f;

	INT32 lSkillLevel = m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if(lSkillLevel < 1 || lSkillLevel >= AGPMSKILL_MAX_SKILL_CAP)
		return 0.0f;

	// 2005.08.19. steeple
	// 거리, 속도에 상관없이 시전된 후 일정 시간 후에 패킷보내는 방식
	if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET] & AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET_BY_STATIC_TIME)
	{
		// 이러면 애니메이션 시간을 리턴.
		if(pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_VELOCITY][lSkillLevel] == 0.0f)
			return fAnimationDuration * 750.0f;
		else
			return pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_VELOCITY][lSkillLevel];
	}
	// ActionOnActionType2 일 때 이렇게 한다.
	else if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2)
		return pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_VELOCITY][lSkillLevel];

	// 2005.09.20. steeple
	// 그냥 Twice Packet 이다.
	return fAnimationDuration * 600.0f;


	//// 여기서 부터는 그냥 일반적으로 거리/속도 에 관련있다.
	////
	////
	//if(pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_VELOCITY][lSkillLevel] == 0.0f)
	//	return fAnimationDuration * 500.0f;		// 거의 바로 보낸다. 2005.09.19. steeple

	//// Target 의 포지션을 얻어온다.
	//AuPOS auTargetPos;
	//FLOAT fDistance = 2000.0f;	// 기본적으로 20 미터라고 치고
	//ApBase* pcsTargetBase = m_pcsApmEventManager->GetBase(APBASE_TYPE_CHARACTER, pcsAgcdCharacter->m_lSelectTargetID);
	//if(pcsTargetBase && pcsTargetBase->m_eType == APBASE_TYPE_CHARACTER)
	//{
	//	// 거리를 구한다.
	//	auTargetPos = ((AgpdCharacter*)pcsTargetBase)->m_stPos;

	//	FLOAT fx = pcsCharacter->m_stPos.x - auTargetPos.x;
	//	FLOAT fy = pcsCharacter->m_stPos.y - auTargetPos.y;
	//	FLOAT fz = pcsCharacter->m_stPos.z - auTargetPos.z;

	//	fDistance = (FLOAT)sqrt((fx * fx) + (fz * fz));
	//}

	//// 2005.03.08. steeple
	//FLOAT fVelocityPerSec = pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_VELOCITY][lSkillLevel];	// 초당 가는 센티미터!!!
	//if(fVelocityPerSec == 0.0f)
	//	fVelocityPerSec = 2000.0f;
	//FLOAT fInterval = fDistance / fVelocityPerSec;	// 미터로 변환

	//return fAnimationDuration * 750.0f + fInterval * 800.0f;	// 0.0f 가 리턴되도 된다.
}

INT32 AgcmSkill::GetNumVisualInfo(AgcdCharacterTemplate *pstAgcdCharacterTemplate)
{
	switch (pstAgcdCharacterTemplate->m_lAnimType2)
	{
	case AGCMCHAR_AT2_WARRIOR:
	case AGCMCHAR_AT2_ARCHER:
	case AGCMCHAR_AT2_WIZARD:
	//@{ 2006/06/07 burumal
	case AGCMCHAR_AT2_ARCHLORD:
	//@}
		{
			return AGCDSKILL_MAX_VISUAL_INFO_PC;
		}
		break;

	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		{
			return AGCDSKILL_MAX_VISUAL_INFO_BOSS_MONSTER;
		}
		break;
	}

	return 0;
}

BOOL AgcmSkill::CBLoadCharacterTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmSkill	*pcsThis			= (AgcmSkill *)(pClass);
	ApBase		*pcsBaseTemplate	= (ApBase *)(pData);

	if (pcsThis->m_pcsAgcmCharacter->GetTemplateReadType() == AGCMCHAR_READ_TYPE_WAIT_ANIM_ONLY)
		return TRUE;

	AgcdCharacterTemplate *pstAgcdCharacterTemplate	= (AgcdCharacterTemplate *)(pCustData);

	AgcdSkillAttachTemplateData	*pcsAttachData	= pcsThis->GetAttachTemplateData(pcsBaseTemplate);
	if (!pcsAttachData)
		return FALSE;

	if (pcsAttachData->m_pacsSkillVisualInfo)
	{
		INT32	lNumVisualInfo	= pcsThis->GetNumVisualInfo(pstAgcdCharacterTemplate);
		INT32	lNumAnimType2	= pcsThis->m_pcsAgcmCharacter->GetAnimType2Num(pstAgcdCharacterTemplate);

		for (INT32 lVisualInfoIndex = 0; lVisualInfoIndex < lNumVisualInfo; ++lVisualInfoIndex)
		{
			if (!pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex])
				continue;

			for (INT32 lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2)
			{
				if (pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2])
				{
					if (pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation)
					{
						pcsThis->m_pcsAgcmCharacter->GetAgcaAnimation2()->ReadRtAnim(
							pcsAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation	);
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmSkill::CBReleaseCharacterTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmSkill				*pcsThis		= (AgcmSkill *)(pClass);
	ApBase					*pcsBaseTemplate	= (ApBase *)(pData);
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate	= (AgcdCharacterTemplate *)(pCustData);

	AgcdSkillAttachTemplateData	*pcsSkillAttachData	= pcsThis->GetAttachTemplateData(pcsBaseTemplate);
	if (!pcsSkillAttachData)
		return FALSE;

	if (pcsSkillAttachData->m_pacsSkillVisualInfo)
	{
		INT32	lNumVisualInfo	= pcsThis->GetNumVisualInfo(pstAgcdCharacterTemplate);
		INT32	lNumAnimType2	= pcsThis->m_pcsAgcmCharacter->GetAnimType2Num(pstAgcdCharacterTemplate);

		for (INT32 lVisualInfoIndex = 0; lVisualInfoIndex < lNumVisualInfo; ++lVisualInfoIndex)
		{
			if (!pcsSkillAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex])
				continue;

			for (INT32 lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2)
			{
				if (pcsSkillAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2])
				{
					if (!pcsSkillAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation)
					{
						continue;
					}

					pcsThis->m_pcsAgcmCharacter->GetAgcaAnimation2()->RemoveRtAnim(
						pcsSkillAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation	);
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmSkill::CBMoveActionSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	INT32			*plSkillID		= (INT32 *)			pCustData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	pThis->m_lFollowCastTargetID	= pcsCharacter->m_lFollowTargetID;
	pThis->m_lFollowCastSkillID		= *plSkillID;
	pThis->m_bFollowCastLock		= TRUE;

	return TRUE;
}

BOOL AgcmSkill::CBMoveActionRelease(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmSkill	*pThis	= (AgcmSkill *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	pThis->m_bFollowCastLock		= FALSE;
	pThis->m_lFollowCastTargetID	= AP_INVALID_CID;
	pThis->m_lFollowCastSkillID		= AP_INVALID_SKILLID;

	return TRUE;
}

BOOL AgcmSkill::CBStopSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmSkill	*pThis	= (AgcmSkill *)	pClass;

	pThis->m_bFollowCastLock		= FALSE;
	pThis->m_lFollowCastTargetID	= AP_INVALID_CID;
	pThis->m_lFollowCastSkillID		= AP_INVALID_SKILLID;

	return TRUE;
}

BOOL AgcmSkill::SetCurrentSkill(AgpdCharacter *pcsCharacter, INT32 lSkillTID)
{
	INT32						lUsableIndex					= GetUsableSkillTNameIndex((ApBase *)(pcsCharacter->m_pcsCharacterTemplate), lSkillTID);
	if(lUsableIndex < 0)
		return FALSE;

	return SetCurrentSkillUsableIndex(pcsCharacter, lSkillTID);
}

BOOL AgcmSkill::SetCurrentSkillUsableIndex(AgpdCharacter *pcsCharacter, INT32 lUsableIndex)
{
	AgcdSkillADChar					*pcsAgcdSkillADChar					= GetCharacterAttachData(pcsCharacter);
	if (!pcsAgcdSkillADChar)
		return FALSE;

	pcsAgcdSkillADChar->m_lSkillUsableIndex	= lUsableIndex;

	return TRUE;
}

BOOL AgcmSkill::SendRequestRollback(INT32 lSkillID)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSkill->MakePacketRequestRollback((ApBase *) m_pcsAgcmCharacter->GetSelfCharacter(), lSkillID, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmSkill->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

// 2006.10.27. steeple
BOOL AgcmSkill::CastGroundTargetSkill(AuPOS stPos, BOOL bForce)
{
	if(m_lGroundTargetSkillID == 0)
		return FALSE;

	AgpdSkill* ppdSkill = m_pcsAgpmSkill->GetSkill( m_lGroundTargetSkillID );
	if( !ppdSkill ) return FALSE;

	BOOL bResult = CastSkill(ppdSkill->m_pcsTemplate->m_lID, m_pcsAgcmCharacter->GetSelfCharacter(), &stPos, bForce);
	SetGroundTargetSkillID(0);

	return bResult;
}

// 2007.02.13. steeple
BOOL AgcmSkill::ProcessAffectedTarget(AgpdSkill* pcsSkill, stAgcmEffectNoticeEffectProcessData& stProcessData)
{
	if(!pcsSkill || !pcsSkill->m_pcsTemplate)
		return FALSE;

	AgcdSkill* pcsAgcdSkill = GetAgcdSkill(pcsSkill);
	if(!pcsAgcdSkill)
		return TRUE;

	// 자기 것만 한다.
	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter || pcsSelfCharacter->m_lID != stProcessData.lOwnerCID)
		return TRUE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);

	RwInt32 num	= 0;
	RwInt32	arrCID[AGPMSKILL_MAX_TARGET] = {0,};

	for(INT32 i = 0; i < AGPMSKILL_MAX_TARGET; ++i)
	{
		if(pcsAgcdSkill->m_alAffectedTargetCID[i] == 0)
			break;

		if(pcsAgcdSkill->m_alAffectedTargetCID[i] == stProcessData.lTargetCID ||
			pcsAgcdSkill->m_alAffectedTargetCID[i] == stProcessData.lOwnerCID)
			continue;

		arrCID[num++] = pcsAgcdSkill->m_alAffectedTargetCID[i];

		ProcessEndEffect(stProcessData.lOwnerCID, pcsAgcdSkill->m_alAffectedTargetCID[i], stProcessData.bMissile, pcsSkillTemplate->m_lID);
	}

	if(num > 0)
		EnumCallback(AGCMSKILL_CB_ID_ADDITIONAL_HITEFFECT, &num, arrCID);

	return TRUE;
}

// 2007.02.13. steeple
void AgcmSkill::ShowAffectedTargetCount(AgcdSkill* pcsAgcdSkill)
{
	if(!m_bShowAffectedTargetCount || !pcsAgcdSkill)
		return;

	AgcmUIConsole* pcsAgcmUIConsole = (AgcmUIConsole*)GetModule("AgcmUIConsole");
	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(pcsAgcmUIConsole && pcsSelfCharacter)
	{
		CHAR szTmp[64];
		INT32 iCount = 0;
		std::count_if(pcsAgcdSkill->m_alAffectedTargetCID.begin(),
						pcsAgcdSkill->m_alAffectedTargetCID.end(),
						bind2nd(not_equal_to<INT32>(), 0));

		sprintf(szTmp, "Affected count : %d", iCount);
		pcsAgcmUIConsole->getConsole().print(szTmp);
	}
}

BOOL AgcmSkill::CBSkillStatusUpdate( PVOID pData , PVOID pClass , PVOID pCustData )
{
	AgcmSkill*		pAgcmSkill		=	static_cast< AgcmSkill* >(pClass);
	UINT64			nOldStatus		=	*( UINT64*	)((PVOID*)pCustData)[0];
	INT32			lSpecialPart	=	*( INT32*	)((PVOID*)pCustData)[1];
	UINT64			nNewStatus		=	0;
	AgpdCharacter*	pSelfCharacter	=	static_cast< AgpdCharacter* >(pData);

	
	if( !pAgcmSkill || !pSelfCharacter )
		return FALSE;

	nNewStatus			=	pSelfCharacter->m_ulSpecialStatus;

	// ----- 무장해제 스킬 처리 ------- Start //

	// 무장해제 스킬에 걸려있는데 끝났을경우
	if( DEF_FLAG_CHK( nOldStatus , AGPDCHAR_SPECIAL_STATUS_DISARMAMENT ) &&  !DEF_FLAG_CHK( nNewStatus , AGPDCHAR_SPECIAL_STATUS_DISARMAMENT ) )
	{
		AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )pAgcmSkill->GetModule( "AgcmUICharacter" );
		if( pcmUICharacter )
		{
			// 방어구 , 무기
			for( INT i = 0 ; i <= AGPMITEM_PART_HAND_RIGHT ; ++i )
			{
				pcmUICharacter->OnHideBlindEquipSlot( pSelfCharacter , i );
			}
		}

		AgcmItem* pcmItem = ( AgcmItem* )pAgcmSkill->GetModule( "AgcmItem" );
		if( pcmItem )
		{
			// 방어구
			for( INT i = 0 ; i < AGPMITEM_PART_HAND_LEFT ; ++i )
				pcmItem->OnAttachPart( pSelfCharacter , (AgpmItemPart)i );

			// 무기
			pcmItem->OnAttachWeapon( pSelfCharacter );
		}
	}
	
	// 무장해제 스킬에 안 걸려있는데 걸렸을경우
	else if( !DEF_FLAG_CHK( nOldStatus , AGPDCHAR_SPECIAL_STATUS_DISARMAMENT ) && DEF_FLAG_CHK( nNewStatus , AGPDCHAR_SPECIAL_STATUS_DISARMAMENT ) )
	{
		AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )pAgcmSkill->GetModule( "AgcmUICharacter" );
		if( pcmUICharacter )
		{

			// 무기
			if( !lSpecialPart )
			{
				pcmUICharacter->OnShowBlindEquipSlot( pSelfCharacter, AGPMITEM_PART_HAND_LEFT );
				pcmUICharacter->OnShowBlindEquipSlot( pSelfCharacter, AGPMITEM_PART_HAND_RIGHT );
			}

			// 방어구
			else
			{
				pcmUICharacter->OnShowBlindEquipSlot( pSelfCharacter , lSpecialPart );
			}
		}

		AgcmItem* pcmItem = ( AgcmItem* )pAgcmSkill->GetModule( "AgcmItem" );
		if( pcmItem )
		{
			if( !lSpecialPart )
				pcmItem->OnDetachWeapon( pSelfCharacter );

			else
				pcmItem->OnDetachPart( pSelfCharacter , (AgpmItemPart)lSpecialPart );

		}

	}

	// ----- 무장해제 스킬 처리 ------- End //

	

	return TRUE;
}

/******************************************************************************
******************************************************************************/