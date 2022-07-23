#include "AgcmCharacter.h"
#include "AgcmItem.h"

#include <rtquat.h>

//@{ 2006/06/21 burumal
//#define HANIMMATRIXSTACKHEIGHT	32
#define HANIMMATRIXSTACKHEIGHT		128
//@}


BOOL AgcmCharacter::SetAttackStatus(AgcdCharacter *pstAgcdCharacter)
{
	pstAgcdCharacter->m_lLastAttackTime	= 0;

	return TRUE;
}


BOOL AgcmCharacter::AdminChangeAnim(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter)
{
	PROFILE("AgcmCharacter::AdminChangeAnim");

	if (!pstAgcdCharacter)
	{
		pstAgcdCharacter = GetCharacterData(pstAgpdCharacter);
		if (!pstAgcdCharacter) return FALSE;
	}

	{
		PROFILE("AGCMCHAR_CB_ID_CHANGE_ANIM");
		EnumCallback(AGCMCHAR_CB_ID_CHANGE_ANIM, pstAgpdCharacter, pstAgcdCharacter);
	}

	if (pstAgpdCharacter == m_pcsSelfCharacter &&
		pstAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_ATTACK)
		m_bStartAttack = FALSE;

	switch(pstAgcdCharacter->m_eCurAnimType)
	{
	case AGCMCHAR_ANIM_TYPE_WAIT:
		{
			PROFILE("AGCMCHAR_CB_ID_CHANGE_ANIM_WAIT");
			EnumCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_WAIT, pstAgpdCharacter, pstAgcdCharacter);
		}
		break;

	case AGCMCHAR_ANIM_TYPE_WALK:
	case AGCMCHAR_ANIM_TYPE_BACKWARD_STEP:				// 뒷걸음	
		{
			PROFILE("AGCMCHAR_CB_ID_CHANGE_ANIM_WALK");
			EnumCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_WALK, pstAgpdCharacter, pstAgcdCharacter);
		}
		break;

	case AGCMCHAR_ANIM_TYPE_RUN:
	case AGCMCHAR_ANIM_TYPE_LEFTSIDE_STEP:
	case AGCMCHAR_ANIM_TYPE_RIGHTSIDE_STEP:
		{
			PROFILE("AGCMCHAR_CB_ID_CHANGE_ANIM_RUN");
			EnumCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_RUN, pstAgpdCharacter, pstAgcdCharacter);
		}
		break;

	case AGCMCHAR_ANIM_TYPE_ATTACK:
		{
			PROFILE("AGCMCHAR_ANIM_TYPE_ATTACK");
			if (!SetStruckAnimation(pstAgpdCharacter, pstAgcdCharacter)) return FALSE;
			EnumCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_ATTACK, pstAgpdCharacter, pstAgcdCharacter);
		}
		break;

	case AGCMCHAR_ANIM_TYPE_STRUCK:
		{
			//ProcessActionQueue(pstAgcdCharacter->m_pstAttacker, pstAgpdCharacter, AGPDCHAR_ACTION_TYPE_ATTACK);

			PROFILE("AGCMCHAR_CB_ID_CHANGE_ANIM_STRUCK");

			//SetSubStruckAnimation(pstAgcdCharacter);

			EnumCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_STRUCK, pstAgpdCharacter, pstAgcdCharacter);
		}
		break;

	case AGCMCHAR_ANIM_TYPE_DEAD:
		{
			PROFILE("AGCMCHAR_CB_ID_CHANGE_ANIM_DEAD");
			//			AdminDeadAnimCB(pstAgpdCharacter, pstAgcdCharacter);
			EnumCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_DEAD, pstAgpdCharacter, pstAgcdCharacter);
		}
		break;

	case AGCMCHAR_ANIM_TYPE_SKILL:
		{
			PROFILE("AGCMCHAR_CB_ID_CHANGE_ANIM_SKILL");
			SetSkillStruckAnimation(pstAgpdCharacter, pstAgcdCharacter);
			EnumCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_SKILL, pstAgpdCharacter, pstAgcdCharacter);
		}
		break;

	case AGCMCHAR_ANIM_TYPE_SOCIAL:
	//@{ Jaewon 20051115
	// Support continuant social animations.
	case AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT:
	//@} Jaewon
		{
			PROFILE("AGCMCHAR_CB_ID_CHANGE_ANIM_SOCIAL");
			EnumCallback(AGCMCHAR_CB_ID_CHANGE_ANIM_SOCIAL, pstAgpdCharacter, pstAgcdCharacter);
		}
		break;
	}

	return TRUE;
}


BOOL AgcmCharacter::SetSubAnimation(AgcdCharacter *pstAgcdCharacter, RpHAnimHierarchy *pstHierarchy, AgcmCharacterAnimType eType)
{
	AgcdCharacterAnimation *pcsAnimation	=
		pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[eType][pstAgcdCharacter->m_lCurAnimType2];

	if (!pcsAnimation)
		pcsAnimation	= pstAgcdCharacter->m_pstAgcdCharacterTemplate->GetAnimation( eType , 0 );
	if (!pcsAnimation)
		return FALSE;

	if (!pcsAnimation->m_pcsAnimation)
		return FALSE;

	if (!pcsAnimation->m_pcsAnimation->m_pcsHead)
		return FALSE;

	ACA_AttachedData	*pcsAttachedData	=
		(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
		AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
		pcsAnimation->m_pcsAnimation->m_pcsHead				));
	if (!pcsAttachedData)
	{
		ASSERT(!"!pstAgcdCharacter->m_pcsCurAnimData");
		return FALSE;
	}

	if (!pcsAttachedData->m_pcsSubData)
		return FALSE;

	if (!pcsAttachedData->m_pcsSubData->m_pcsRtAnim)
		return FALSE;

	if (!pcsAttachedData->m_pcsSubData->m_pcsRtAnim->m_pstAnimation)
		return FALSE;

	if (!pstHierarchy)
		return FALSE;

	RpHAnimHierarchySetCurrentAnim(
		pstHierarchy,
		pcsAttachedData->m_pcsSubData->m_pcsRtAnim->m_pstAnimation	);

	return TRUE;
}


BOOL AgcmCharacter::SetSubStruckAnimation(AgcdCharacter *pstAgcdCharacter)
{
	if (!SetSubAnimation(pstAgcdCharacter, pstAgcdCharacter->m_csSubAnim.m_pInHierarchy, AGCMCHAR_ANIM_TYPE_STRUCK))
		return FALSE;

	pstAgcdCharacter->m_csSubAnim.SetComponent(
		TRUE,
		500,
		pstAgcdCharacter->m_csSubAnim.m_pInHierarchy->currentAnim->pCurrentAnim->duration	);

	return TRUE;
}

BOOL AgcmCharacter::SetStruckAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, FLOAT fForceDelayTime, BOOL bMakeAnimPoint)
{
	if ((!pstAgpdCharacter->m_alTargetCID[0]) || (pstAgpdCharacter->m_alTargetCID[0] == pstAgpdCharacter->m_lID))
		return TRUE; // skip

	AgpdCharacter *pcsAgpdTarget = m_pcsAgpmCharacter->GetCharacter(pstAgpdCharacter->m_alTargetCID[0]);
	if (!pcsAgpdTarget) return FALSE;

	AgcdCharacter *pcsAgcdTarget = GetCharacterData(pcsAgpdTarget);
	if (!pcsAgcdTarget) return FALSE;

	//@{ 2006/03/15 burumal
	// 멀티픽킹 아토믹을 사용하는 경우 모델툴에서 자동으로 next 선택되도록
#ifdef USE_MFC
	if ( pcsAgcdTarget->m_pPickAtomic )
		if ( pcsAgcdTarget->m_pPickAtomic != pcsAgcdTarget->m_pPickAtomic->next )
			pcsAgcdTarget->m_pPickAtomic = pcsAgcdTarget->m_pPickAtomic->next;
#endif
	//@}

	AgcdCharacterTemplate *pcsAgcdTargetTemplate = pcsAgcdTarget->m_pstAgcdCharacterTemplate;
	if (!pcsAgcdTargetTemplate) return FALSE;

	if (!pstAgcdCharacter)
	{
		pstAgcdCharacter = GetCharacterData(pstAgpdCharacter);
		if(!pstAgcdCharacter) return FALSE;
	}

	pcsAgcdTarget->m_lAttackerID						= pstAgpdCharacter->m_lID;
	pcsAgcdTarget->m_lLastAttackTime					= 0;

	/*AgcdCharacterAnimationAttachedData	*pstCharAnimAttachedData	=
	(AgcdCharacterAnimationAttachedData *)(m_csAnimation.GetAttachedData(pstAgcdCharacter->m_pstCurAnim, AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY));
	if (!pstCharAnimAttachedData)
	return FALSE;*/

	ACA_AttachedData	*pcsAnimAttachedData	=	NULL;

	if (pstAgcdCharacter->m_pcsCurAnimData)
		pcsAnimAttachedData		= (ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
		AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
		pstAgcdCharacter->m_pcsCurAnimData				));

	AgcdAnimationCallbackData		csData;

	/*	BOOL							bSkipStruckAnim		= TRUE;
	AgpdCharacterActionResultType	eActionResultType	= GetNextReceivedActionResult(pstAgpdCharacter, pcsAgpdTarget, AGPDCHAR_ACTION_TYPE_ATTACK);
	if ((eActionResultType != AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS) &&
	(eActionResultType != AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_EVADE) &&
	(eActionResultType != AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_BLOCK) &&
	(pcsAgcdTarget->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_SKILL)			)
	bSkipStruckAnim									= FALSE;

	if (!bSkipStruckAnim)*/
	{
		csData.m_pavData[AGCMCHAR_ACDA_ANIM_TYPE]		= (PVOID)(AGCMCHAR_ANIM_TYPE_STRUCK);
		if (pcsAnimAttachedData)
			csData.m_pavData[AGCMCHAR_ACDA_CUST_ANIM_TYPE]	= (PVOID)(pcsAnimAttachedData->m_nCustType);
		//(PVOID)(pstCharAnimAttachedData->m_alCustAnimType[pstAgcdCharacter->m_lCurAnimIndex]);
		//csData.m_pavData[AGCMCHAR_ACDA_ANIMATION]		= ((PVOID)(pcsAgcdTargetTemplate->m_paAnim[AGCMCHAR_ANIM_TYPE_STRUCK][pcsAgcdTarget->m_lAnimHandEquipType]));
	}

	csData.m_pavData[AGCMCHAR_ACDA_START_ANIM_CB_POINT]	= (PVOID)(AGCMCHAR_START_ANIM_CB_POINT_DEFAULT);
	//	csData.m_pavData[AGCMCHAR_AGCD_CUST_CALLBACK]		= EnumCallbackStruckChar;
	csData.m_pavData[AGCMCHAR_AGCD_CUST_CALLBACK]		= NULL;
	csData.m_pavData[AGCMCHAR_ACDA_CHARACTER_ID]		= (PVOID)(pcsAgpdTarget->m_lID);
	csData.m_pavData[AGCMCHAR_AGCD_INIT_ANIM]			= (PVOID)(FALSE);

	csData.m_pfCallback									= OnAnimCB;

	csData.m_bInfluenceNextAnimation					= TRUE;

	RwReal	fAnimPoint;
	if (fForceDelayTime > -1.0f)
	{
		fAnimPoint = pstAgcdCharacter->m_csAnimation.m_fCurrentTime + fForceDelayTime;

		csData.m_fTime	= fAnimPoint;
		AddAnimCallback(pstAgcdCharacter, &csData);
	}
	/*else if (	(pstCharAnimAttachedData->m_paszAnimPoint[pstAgcdCharacter->m_lCurAnimIndex]) &&
	(pstCharAnimAttachedData->m_paszAnimPoint[pstAgcdCharacter->m_lCurAnimIndex][0] >= '0') &&
	(pstCharAnimAttachedData->m_paszAnimPoint[pstAgcdCharacter->m_lCurAnimIndex][0] <= '9')		)*/
	else if ((pcsAnimAttachedData) && (pcsAnimAttachedData->m_pszPoint))
	{
		INT32	lCount, alTemp[AGCD_MAX_ANIMATION_ANIM_POINT];
		memset(alTemp, 0, sizeof(INT32) * AGCD_MAX_ANIMATION_ANIM_POINT);

		INT32	lParse = pstAgcdCharacter->m_csAnimation.ParseAnimPoint(
			//pstCharAnimAttachedData->m_paszAnimPoint[pstAgcdCharacter->m_lCurAnimIndex],
			pcsAnimAttachedData->m_pszPoint,
			alTemp																				);

		if (lParse < 1)
			return FALSE;

		for (lCount = 0; lCount < lParse; ++lCount)
		{
			csData.m_fTime	= (FLOAT)(alTemp[lCount]) / 100.0f;
			AddAnimCallback(pstAgcdCharacter, &csData);
		}

		fAnimPoint															= (FLOAT)(alTemp[0]) / 100.0f;
	}
	else
	{
		if (!bMakeAnimPoint)
			return TRUE; // skip

		fAnimPoint = (RwReal)(pstAgcdCharacter->m_csAnimation.GetDuration() * AGCMCHAR_ATTACK_ANIM_POINT_RATIO);

		csData.m_fTime	= fAnimPoint;
		AddAnimCallback(pstAgcdCharacter, &csData);
	}

	//	INT32 lIdleEventTime = m_ulCurTick + (INT32)(fAnimPoint * 1000.0f);
	//	AddIdleEvent(lIdleEventTime, pcsAgpdTarget->m_lID, this, ProcessStruckIdleEvent, (PVOID) pstAgpdCharacter->m_lID);

	return TRUE;
}


BOOL AgcmCharacter::SetSkillStruckAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter)
{
	for (INT32 lSkillTarget = 0; lSkillTarget < AGPDCHARACTER_MAX_TARGET; ++lSkillTarget)
	{
		//		if (!pstAgcdCharacter->m_alSkillTargetCID[lSkillTarget])
		if (!pstAgpdCharacter->m_alTargetCID[lSkillTarget])
			break;

		AgpdCharacter *pcsAgpdTarget = m_pcsAgpmCharacter->GetCharacter(pstAgpdCharacter->m_alTargetCID[lSkillTarget]);
		if (!pcsAgpdTarget) return FALSE;

		AgcdCharacter *pcsAgcdTarget = GetCharacterData(pcsAgpdTarget);
		if (!pcsAgcdTarget)	return FALSE;

		AgcdCharacterTemplate *pcsAgcdTargetTemplate = pcsAgcdTarget->m_pstAgcdCharacterTemplate;
		if (!pcsAgcdTargetTemplate) return FALSE;

		if (!pstAgcdCharacter)
		{
			pstAgcdCharacter = GetCharacterData(pstAgpdCharacter);
			if(!pstAgcdCharacter) return FALSE;
		}

		pcsAgcdTarget->m_lAttackerID				= pstAgpdCharacter->m_lID;

		ACA_AttachedData	*pcsAnimAttachedData	=
			(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
			AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
			pstAgcdCharacter->m_pcsCurAnimData				));

		AgcdAnimationCallbackData		csData;

		//		if (pcsAgcdTarget->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_SKILL)
		{
			csData.m_pavData[AGCMCHAR_ACDA_ANIM_TYPE]		= (PVOID)(AGCMCHAR_ANIM_TYPE_STRUCK);
			//csData.m_pavData[AGCMCHAR_ACDA_CUST_ANIM_TYPE]	= (PVOID)(pstCharAnimAttachedData->m_alCustAnimType[pstAgcdCharacter->m_lCurAnimIndex]);
			if (pcsAnimAttachedData)
				csData.m_pavData[AGCMCHAR_ACDA_CUST_ANIM_TYPE]	= (PVOID)(pcsAnimAttachedData->m_nCustType);
			//csData.m_pavData[AGCMCHAR_ACDA_ANIMATION]		= 
			//	((PVOID)(pcsAgcdTargetTemplate->m_paAnim[AGCMCHAR_ANIM_TYPE_STRUCK][pcsAgcdTarget->m_lAnimHandEquipType]));
		}

		csData.m_pavData[AGCMCHAR_ACDA_START_ANIM_CB_POINT]	= (PVOID)(AGCMCHAR_START_ANIM_CB_POINT_DEFAULT);
		csData.m_pavData[AGCMCHAR_AGCD_CUST_CALLBACK]		= EnumCallbackStruckChar;
		csData.m_pavData[AGCMCHAR_ACDA_CHARACTER_ID]		= (PVOID)(pcsAgpdTarget->m_lID);
		csData.m_pavData[AGCMCHAR_AGCD_INIT_ANIM]			= (PVOID)(FALSE);

		csData.m_pfCallback									= OnAnimCB;

		RwReal fAnimPoint = 0.0f;
		/*if (	(pstCharAnimAttachedData->m_paszAnimPoint[pstAgcdCharacter->m_lCurAnimIndex]) &&
		(pstCharAnimAttachedData->m_paszAnimPoint[pstAgcdCharacter->m_lCurAnimIndex][0] >= '0') &&
		(pstCharAnimAttachedData->m_paszAnimPoint[pstAgcdCharacter->m_lCurAnimIndex][0] <= '9')		)*/
		if ((pcsAnimAttachedData) && (pcsAnimAttachedData->m_pszPoint))
		{
			INT32	lCount, alTemp[AGCD_MAX_ANIMATION_ANIM_POINT];
			memset(alTemp, 0, sizeof(INT32) * AGCD_MAX_ANIMATION_ANIM_POINT);

			INT32	lParse = pstAgcdCharacter->m_csAnimation.ParseAnimPoint(
				//pstCharAnimAttachedData->m_paszAnimPoint[pstAgcdCharacter->m_lCurAnimIndex],
				pcsAnimAttachedData->m_pszPoint,
				alTemp															);

			if (lParse < 1)
				return FALSE;

			for (lCount = 0; lCount < lParse; ++lCount)
			{
				csData.m_fTime													= (FLOAT)(alTemp[lCount]) / 100.0f;

				AddAnimCallback(pstAgcdCharacter, &csData);
			}

			fAnimPoint															= (FLOAT)(alTemp[0]) / 100.0f;
		}

		if (fAnimPoint > 0.0f)
		{
			INT32 lIdleEventTime = m_ulCurTick + (INT32)(fAnimPoint * 1000.0f);
			AddIdleEvent(lIdleEventTime, pcsAgpdTarget->m_lID, this, ProcessSkillStruckIdleEvent, (PVOID) pstAgpdCharacter->m_lID);
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::SetDeadAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter)
{
	if (!pstAgcdCharacter->m_pInHierarchy)
		return FALSE;

	if (!pstAgcdCharacter->m_pstAgcdCharacterTemplate)
		return FALSE;

	AgcdCharacterAnimation	*pcsDeadAnimation;
	if (pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_DEAD][pstAgcdCharacter->m_lCurAnimType2])
		pcsDeadAnimation	= pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_DEAD][pstAgcdCharacter->m_lCurAnimType2];
	else
		pcsDeadAnimation	= pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_DEAD][0];

	if (	(!pcsDeadAnimation) ||
		(!pcsDeadAnimation->m_pcsAnimation) ||
		(!pcsDeadAnimation->m_pcsAnimation->m_pcsHead) ||
		(!pcsDeadAnimation->m_pcsAnimation->m_pcsHead->m_pcsRtAnim) ||
		(!pcsDeadAnimation->m_pcsAnimation->m_pcsHead->m_pcsRtAnim->m_pstAnimation)	)
		return FALSE;

	RpHAnimHierarchySetCurrentAnim(
		pstAgcdCharacter->m_pInHierarchy,
		pcsDeadAnimation->m_pcsAnimation->m_pcsHead->m_pcsRtAnim->m_pstAnimation	);

	RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, SetHierarchyForSkinAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));
	pstAgcdCharacter->m_lStatus	|= AGCMCHAR_STATUS_FLAG_ATTACHED_HIERARCHY;

	RpHAnimHierarchySetCurrentAnimTime(pstAgcdCharacter->m_pInHierarchy, pstAgcdCharacter->m_pInHierarchy->currentAnim->pCurrentAnim->duration);

	LockFrame();
	RpHAnimHierarchyUpdateMatrices(pstAgcdCharacter->m_pInHierarchy);
	UnlockFrame();

	pstAgcdCharacter->m_bStop	= TRUE;

	return TRUE;
}

BOOL AgcmCharacter::StopAnimation(INT32 lCID)
{
	AgpdCharacter *pstAgpdCharacter = m_pcsAgpmCharacter ? m_pcsAgpmCharacter->GetCharacter(lCID) : NULL;

	return StopAnimation(pstAgpdCharacter);
}

BOOL AgcmCharacter::StopAnimation(AgpdCharacter *pstCharacter)
{
	return StopAnimation(GetCharacterData(pstCharacter));
}

BOOL AgcmCharacter::StopAnimation(AgcdCharacter *pstCharacter)
{
	pstCharacter->m_bStop = TRUE;

	return TRUE;
}

VOID AgcmCharacter::SkipSearchSituation(BOOL bSkipSearchSituation)
{
	if (bSkipSearchSituation)
		m_ulModuleFlags	|= E_AGCM_CHAR_FLAGS_SKIP_SEARCH_SITUATION;
	else
		m_ulModuleFlags	&= ~E_AGCM_CHAR_FLAGS_SKIP_SEARCH_SITUATION;
}

eAcReturnType AgcmCharacter::SearchSituation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, AgcmCharacterAnimType eNewAnimType)
{
	if (m_ulModuleFlags & E_AGCM_CHAR_FLAGS_SKIP_SEARCH_SITUATION)
	{
		if (eNewAnimType == AGCMCHAR_ANIM_TYPE_STRUCK)
		{
			AgpdCharacter	*pcsAgpdAttacker	= m_pcsAgpmCharacter->GetCharacter(pstAgcdCharacter->m_lAttackerID);
			if (pcsAgpdAttacker)
				EnumCallback(AGCMCHAR_CB_ID_ATTACK_SUCESS, pcsAgpdAttacker, pstAgpdCharacter);
		}

		return E_AC_RETURN_TYPE_SUCCESS;
	}

	//@{ 2006/12/26 burumal
	/*
	// Character에 hierarchy가 설정되어 있지 않으면...(thread 관련!)
	if (!(pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_INIT_HIERARCHY))
	{
		pstAgcdCharacter->m_eCurAnimType = eNewAnimType;
		return E_AC_RETURN_TYPE_SKIP;
	}
	*/
	// 스킬이 없는 캐릭터인 경우 hierarchy가 없으면 skip 시킨다
	if ( !(pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_INIT_HIERARCHY) )
	{
		AgpdSkillTemplateAttachData* pcsTemplateAttachData = NULL;
		if ( m_pcsAgpmSkill )
			pcsTemplateAttachData = m_pcsAgpmSkill->GetAttachSkillTemplateData(pstAgpdCharacter->m_pcsCharacterTemplate);
		
		if ( (pcsTemplateAttachData == NULL) || (pcsTemplateAttachData->m_aszUsableSkillTName[0] == NULL) )
		{
			pstAgcdCharacter->m_eCurAnimType = eNewAnimType;
			return E_AC_RETURN_TYPE_SKIP;
		}
	}
	//@}

	// 뒈져 있으면 애니메이션을 할 수 없당! -_-;;
	if ((pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_DEAD) &&
		(pstAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)		)
	{
		//		StopAnimation(pstAgcdCharacter);
		return E_AC_RETURN_TYPE_SKIP; // skip!
	}
	//탈것 처리
	if( pstAgcdCharacter->m_pOwner && (pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_DEAD) )
	{
		AgpdCharacter *pstAgpdCharacterOwner = GetCharacter( pstAgcdCharacter->m_pOwner );
		if( pstAgpdCharacterOwner->m_unActionStatus == AGPDCHAR_STATUS_NOT_ACTION ||
			pstAgpdCharacterOwner->m_unActionStatus == AGPDCHAR_STATUS_DEAD  ) {
			return E_AC_RETURN_TYPE_SKIP; // skip!
		}
	}


	//@{ Jaewon 20051115
	// Support continuant social animations.
#define IS_SOCIAL_TYPE(type) ((type == AGCMCHAR_ANIM_TYPE_SOCIAL) || (type == AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT))
	if (	IS_SOCIAL_TYPE(pstAgcdCharacter->m_eCurAnimType) &&
		!IS_SOCIAL_TYPE(eNewAnimType)							)
	//@} Jaewon
	{
		EnumCallback(AGCMCHAR_CB_ID_END_SOCIAL_ANIM, pstAgpdCharacter, pstAgcdCharacter);
	}

	switch (eNewAnimType)
	{
	case AGCMCHAR_ANIM_TYPE_ATTACK:
		{
			AgpdCharacter	*pcsAgpdTarget	= m_pcsAgpmCharacter->GetCharacter(pstAgpdCharacter->m_alTargetCID[0]);
			if (pcsAgpdTarget)
			{
				AgcdCharacter	*pcsAgcdTarget	= GetCharacterData(pcsAgpdTarget);
				if (pcsAgcdTarget)
				{
					if (pcsAgcdTarget->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_DEAD)
					{
						if (!StartAnimation(pstAgpdCharacter, pstAgcdCharacter, AGCMCHAR_ANIM_TYPE_WAIT))
							return E_AC_RETURN_TYPE_FAILURE;

						return E_AC_RETURN_TYPE_SKIP;
					}
				}
			}
		}
		break;

	case AGCMCHAR_ANIM_TYPE_STRUCK:
		{
			AgpdCharacter	*pcsAgpdAttacker	= m_pcsAgpmCharacter->GetCharacter(pstAgcdCharacter->m_lAttackerID);
			if (!pcsAgpdAttacker)
				return E_AC_RETURN_TYPE_FAILURE;

			AgcdCharacter	*pcsAgcdAttacker	= GetCharacterData(pcsAgpdAttacker);
			if (!pcsAgcdAttacker)
				return E_AC_RETURN_TYPE_FAILURE;

			if(pstAgcdCharacter->m_bForceAnimation)
				return E_AC_RETURN_TYPE_SUCCESS;

			//dobal
			//몬스터가 스킬중 공격당하면 무시한다.
			if( m_pcsAgpmCharacter->IsMonster(pstAgpdCharacter) )
			{
				if( pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SKILL ) {
					return E_AC_RETURN_TYPE_SKIP;
				}
			}

			if (pcsAgcdAttacker->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SKILL)
			{
				// 2005.08.24. steeple
				// 이거 체크하는 거 몬스터가 쓰는 스킬 때문에 SkillTID 로 변경
				BOOL bProcessRightNow = TRUE;
				if(m_pcsAgpmSkill)
				{
					AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(pcsAgcdAttacker->m_lLastCastSkillTID);
					if(pcsSkillTemplate && pcsSkillTemplate->m_bTwicePacket)
						bProcessRightNow = FALSE;
				}

				if(bProcessRightNow)
				{
					EnumCallback(AGCMCHAR_CB_ID_ATTACK_SUCESS, pcsAgpdAttacker, pstAgpdCharacter);

					// 바로 불러주는 거 뺐었다가 다시 추가. 2005.08.24. steeple
					ProcessActionQueue(pcsAgpdAttacker, pstAgpdCharacter, AGPDCHAR_ACTION_TYPE_SKILL, pcsAgcdAttacker->m_lLastCastSkillTID);
				}
				else
					return E_AC_RETURN_TYPE_SKIP;
			}
			else
			{
				AgpdCharacterActionResultType	eActionResultType	= GetNextReceivedActionResult(pcsAgpdAttacker, pstAgpdCharacter, AGPDCHAR_ACTION_TYPE_ATTACK);

				switch (eActionResultType)
				{
				case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS:
				case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_CRITICAL:
				case AGPDCHAR_ACTION_RESULT_TYPE_COUNTER_ATTACK:
				case AGPDCHAR_ACTION_RESULT_TYPE_DEATH_STRIKE:
					{
						EnumCallback(AGCMCHAR_CB_ID_ATTACK_SUCESS, pcsAgpdAttacker, pstAgpdCharacter);

						if (pstAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_WAIT)
						{
							if ((pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_RUN) ||
								(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_WALK			) ||
								(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_LEFTSIDE_STEP	) ||
								(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_RIGHTSIDE_STEP	) ||
								(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_BACKWARD_STEP	) )
							{
								ProcessActionQueue( pcsAgpdAttacker, pstAgpdCharacter, AGPDCHAR_ACTION_TYPE_ATTACK);
								return E_AC_RETURN_TYPE_SKIP;
							}

							if (m_pcsAgpmCharacter->IsPC(pstAgpdCharacter))
							{
								if (m_csRandom.randInt(AGCMCHAR_RAND_PERCENTAGE) > AGCMCHAR_PC_STRUCK_ANIM_RATE)
								{
									ProcessActionQueue(pcsAgpdAttacker, pstAgpdCharacter, AGPDCHAR_ACTION_TYPE_ATTACK);
									return E_AC_RETURN_TYPE_SKIP;
								}
							}
						}
					}
					break;

				default:
					{
						ProcessActionQueue(pcsAgpdAttacker, pstAgpdCharacter, AGPDCHAR_ACTION_TYPE_ATTACK);
						return E_AC_RETURN_TYPE_SKIP; // skip
					}
					break;
				}

				ProcessActionQueue(pcsAgpdAttacker, pstAgpdCharacter, AGPDCHAR_ACTION_TYPE_ATTACK);

				/*				if (m_pcsAgpmCharacter->IsPC(pstAgpdCharacter))
				{
				if (pstAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_WAIT)
				return E_AC_RETURN_TYPE_SKIP;
				}*/
			}

			if (pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SKILL)
				return E_AC_RETURN_TYPE_SKIP;
		}
		break;

		// Social 관련 처리는 AgcmCharacter에서 처리하지 않고, AgcmEventEffect(실제로 불러주는 모듈)에서 처리한다.(BOB, 271004)
		/*	case AGCMCHAR_ANIM_TYPE_SOCIAL:
		{
		if (pstAgpdCharacter->m_bIsTrasform)
		return E_AC_RETURN_TYPE_SKIP;

		if (pstAgcdCharacter->m_lLastAttackTime > -1)
		return E_AC_RETURN_TYPE_SKIP;
		}
		break;*/
	}

	return E_AC_RETURN_TYPE_SUCCESS;
}

eAcReturnType AgcmCharacter::CheckStartAnimCallback(	AgpdCharacter						*pstAgpdCharacter,
													AgcdCharacter						*pstAgcdCharacter,
													AgcmCharacterAnimType				eNextAnimType,
													AgcdCharacterAnimation				*pstNextAnim,
													BOOL								*pbDestIdenticalAnim,
													BOOL								*pbDestEndThenChange,
													AgcmCharacterStartAnimCallbackPoint	ePoint,
													INT32								lCustAnimType			)
{
	if (!pstNextAnim)
		return E_AC_RETURN_TYPE_FAILURE;

	//@{ 2006/09/07 burumal	
	ACA_AttachedData* pstAttachedData = 
		(ACA_AttachedData*) (AgcmCharacter::m_pThisAgcmCharacter->m_csAnimation2.GetAttachedData(AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY, pstAgcdCharacter->m_pcsCurAnimData));

	if ( pstAttachedData )
	{
		if ( pstAttachedData->m_lClumpShowOffsetTime * 0.001f <= pstAgcdCharacter->m_csAnimation.m_fCurrentTime )
		{			
			if ( pstAttachedData->m_unCustFlags & AGCMCHAR_ACF_CLUMP_SHOW_START_TIME )
			{
				if ( pstAgcdCharacter->m_pClump && (pstAgcdCharacter->m_cTransparentType == (INT8) AGCMCHAR_TRANSPARENT_FULL) )
					AgcmCharacter::m_pThisAgcmCharacter->ShowClumpForced(pstAgpdCharacter->m_lID, TRUE, pstAttachedData->m_uClumpFadeInTime);
			}
		}
	}
	//@}
	
	//. 2006. 6. 28. Nonstopdj
	//. PolyMorph의 경우 pstNextAnim와 pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[eNextAnimType][pstAgcdCharacter->m_lCurAnimType2]
	//. 다를 경우가 있다. 어디에선가 원본의 TID로 AgcdCharacterTemplate를 이용하여 설정하는데 그곳은 어딘지 찾지 못했음.
	//. 재설정하여 문제 해결. -_-;;
	if(eNextAnimType > 0)
	{
		AgcdCharacterAnimation * pAnimation = pstAgcdCharacter->m_pstAgcdCharacterTemplate->GetAnimation( eNextAnimType , pstAgcdCharacter->m_lCurAnimType2 );
		if( NULL == pAnimation ) pAnimation = pstAgcdCharacter->m_pstAgcdCharacterTemplate->GetAnimation( eNextAnimType , 0 );
		if(pstNextAnim != pAnimation)
			pstNextAnim = pAnimation;
	}

	if (!pstNextAnim)
		return E_AC_RETURN_TYPE_FAILURE;

	// 기본으로 불린거라면...(애니메이션 콜백등으로 호출된게 아니라면...)
	if (ePoint == AGCMCHAR_START_ANIM_CB_POINT_DEFAULT)
	{
		// 다음 애니매이션이 링크 애니매이션이라면...
		//if ((pstNextAnimAttachedData) && (pstNextAnimAttachedData->m_stAnimFlag.m_ulAnimFlag & AGCD_ANIMATION_FLAG_LINK))
		if (	(pstNextAnim->m_pstAnimFlags) &&
			(pstNextAnim->m_pstAnimFlags->m_unAnimFlag & AGCD_ANIMATION_FLAG_LINK)	)
		{ 
			if ((pstAgcdCharacter->m_eCurAnimType == eNextAnimType) && (!(pstAgcdCharacter->m_ulAnimationFlags & AGCMCHAR_AF_BLEND)))
			{
				//if (SetEndThenChangeAnimation(pstAgpdCharacter, pstAgcdCharacter, pstNextAnim, eNextAnimType))

				pstAgcdCharacter->m_pcsNextAnimData	= pstAgcdCharacter->m_pcsCurAnimData;

				if (SetEndThenChangeAnimation(pstAgpdCharacter, pstAgcdCharacter, eNextAnimType))
					return E_AC_RETURN_TYPE_SKIP;
				else
					return E_AC_RETURN_TYPE_FAILURE;
			}
			else
			{
				//				InitLinkAnimData(pstAgcdCharacter);
				InitAnimData(pstAgcdCharacter, pstNextAnim);
			}
		}
		else
		{
			// 다음 에니메이션의 인덱스를 구한다.
			//pstAgcdCharacter->m_lNextAnimIndex = GenerateAnimIndex(pstAgcdCharacter, eNextAnimType, pstNextAnim, lCustAnimType);
			// 다음 애니메이션을 설정한다.
			if (!SetNextAnimData(pstAgcdCharacter, eNextAnimType, pstNextAnim, lCustAnimType))
				return E_AC_RETURN_TYPE_FAILURE;

			// 현재 캐릭터가 블랜딩이 아니고... 현재 에니메이션과 다음 에니메이션이 같다면... skip한다.
			/*			if (pstAgcdCharacter->m_pstCurAnim)
			if (!(pstAgcdCharacter->m_ulAnimationFlags & AGCMCHAR_AF_BLEND) && !pstAgcdCharacter->m_bForceAnimation) // added by netong
			if (pstAgcdCharacter->m_pstCurAnim->m_ppastAnimData[pstAgcdCharacter->m_lCurAnimIndex] == pstNextAnim->m_ppastAnimData[pstAgcdCharacter->m_lNextAnimIndex])*/
			if (pstAgcdCharacter->m_pcsCurAnimData)
				if (!(pstAgcdCharacter->m_ulAnimationFlags & AGCMCHAR_AF_BLEND) && !(pstAgcdCharacter->m_bForceAnimation))
					if (pstAgcdCharacter->m_pcsCurAnimData == pstAgcdCharacter->m_pcsNextAnimData)
						return E_AC_RETURN_TYPE_SKIP;

			// 같은 타입의 애니메이션이고 공격 애니메이션인 경우 현재 애니메이션이 끝난 후에 플레이 할 수 있도록 한다.
			if (	(pstAgcdCharacter->m_eCurAnimType == eNextAnimType) &&
				(eNextAnimType == AGCMCHAR_ANIM_TYPE_ATTACK)			)
			{
				if (pbDestEndThenChange)
					*pbDestEndThenChange	= TRUE;
			}
		}
	}
	else if (ePoint == AGCMCHAR_START_ANIM_CB_POINT_ANIMEND)
	{
		//		pstAgcdCharacter->m_lNextAnimIndex = GenerateAnimIndex(pstAgcdCharacter, eNextAnimType, pstNextAnim, lCustAnimType);
		// 다음 애니메이션을 설정한다.
		if (!SetNextAnimData(pstAgcdCharacter, eNextAnimType, pstNextAnim, lCustAnimType))
			return E_AC_RETURN_TYPE_FAILURE;
		// 현재 애니매이션과 다음 애니매이션이 같냐?
		//if (pstAgcdCharacter->m_pstCurAnim->m_ppastAnimData[pstAgcdCharacter->m_lCurAnimIndex] == pstNextAnim->m_ppastAnimData[pstAgcdCharacter->m_lNextAnimIndex])
		if (pstAgcdCharacter->m_pcsCurAnimData == pstAgcdCharacter->m_pcsNextAnimData)
			*(pbDestIdenticalAnim) = TRUE;
	}
	else if (ePoint == AGCMCHAR_START_ANIM_CB_POINT_ANIMENDTHENCHANGE) 
	{
		// 다음 애니메이션의 flag가 AGCD_ANIMATION_FLAG_LINK라면...
		//		if ((pstNextAnimAttachedData) && (pstNextAnimAttachedData->m_stAnimFlag.m_ulAnimFlag & AGCD_ANIMATION_FLAG_LINK))
		if (	(pstNextAnim->m_pstAnimFlags) &&
			(pstNextAnim->m_pstAnimFlags->m_unAnimFlag & AGCD_ANIMATION_FLAG_LINK)	)
		{
			// 다음 인덱스로 넘어간다.
			//			if (!(pstNextAnimAttachedData->m_aulCustFlags[pstAgcdCharacter->m_lNextAnimIndex] & AGCMCHAR_ACF_LOCK_LINK_ANIM))
			//++pstAgcdCharacter->m_lNextAnimIndex;

			if (pstAgcdCharacter->m_pcsNextAnimData)
			{
				ACA_AttachedData	*pstAttachedData	= 
					(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
					AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
					pstAgcdCharacter->m_pcsNextAnimData				));
				if (!pstAttachedData)
				{
					ASSERT(!"!pstAttachedData");
					return E_AC_RETURN_TYPE_FAILURE;
				}

				if (!(pstAttachedData->m_unCustFlags & AGCMCHAR_ACF_LOCK_LINK_ANIM))
					pstAgcdCharacter->m_pcsNextAnimData	= pstAgcdCharacter->m_pcsNextAnimData->m_pcsNext;
			}
			else
			{
				if (	(!pstNextAnim) ||
					(!pstNextAnim->m_pcsAnimation)	||
					(!pstNextAnim->m_pcsAnimation->m_pcsHead)	)
				{
					ASSERT(!"!pstNextAnim");
					return E_AC_RETURN_TYPE_FAILURE;
				}

				pstAgcdCharacter->m_pcsNextAnimData	= pstNextAnim->m_pcsAnimation->m_pcsHead;
			}

			//			pstAgcdCharacter->m_fLinkAnimSpeed		= 0.0f;

			// 다음 인덱스의 애니메이션이 없으면 reset한다.
			//if(!pstNextAnim->m_ppastAnimData[pstAgcdCharacter->m_lNextAnimIndex])
			if (!pstAgcdCharacter->m_pcsNextAnimData)
			{
				InitAnimData(pstAgcdCharacter, NULL);
				return E_AC_RETURN_TYPE_SKIP;
				/*				if (StopLinkAnimation(pstAgpdCharacter, pstAgcdCharacter))
				return E_AC_RETURN_TYPE_SKIP;
				else
				return E_AC_RETURN_TYPE_FAILURE;*/
			}
		}
	}
	else if (ePoint == AGCMCHAR_START_ANIM_CB_POINT_BLEND)
	{
	}

	return E_AC_RETURN_TYPE_SUCCESS;
}

VOID AgcmCharacter::SetAnimOption(	BOOL								*pbDestBlend,
								  BOOL								*pbDestEndThenChange,
								  AgcdCharacter						*pstAgcdCharacter,
								  //AgcdCharacterAnimationAttachedData	*pstNextAnimAttachedData,
								  AgcdAnimationFlag					*pstFlags,
								  BOOL								bIdenticalAnimation			)
{
	//if ((pstAgcdCharacter->m_ulAnimationFlags & AGCMCHAR_AF_BLEND) || (!pstAgcdCharacter->m_pstCurAnim) || (bIdenticalAnimation))
	if ((pstAgcdCharacter->m_ulAnimationFlags & AGCMCHAR_AF_BLEND) || (!pstAgcdCharacter->m_pcsCurAnimData) || (bIdenticalAnimation))
	{
		*(pbDestBlend)			= FALSE; // 캐릭터가 블랜딩중이거나 현재 캐릭터에 애니메이션이 설정되어 있지 않거나 같은 애니매이션이라면 블랜딩을 할 수 없다.
		*(pbDestEndThenChange)	= FALSE; // 캐릭터가 블랜딩중이거나 현재 캐릭터에 애니메이션이 설정되어 있지 않거나 같은 애니매이션이라면 이녀석을 쓸 수 없다.
	}
	else if (!(*pbDestEndThenChange)) // && (!pstAgcdCharacter->m_bBlending) && (pstAgcdCharacter->m_pstCurAnim)
	{
		// 다음 애니메이션의 플래그에 따라서 블랜딩을 결정한다.
		*(pbDestBlend)			=
			//((pstNextAnimAttachedData) && (pstNextAnimAttachedData->m_stAnimFlag.m_ulAnimFlag & AGCD_ANIMATION_FLAG_BLEND)) ?
			((pstFlags) && (pstFlags->m_unAnimFlag & AGCD_ANIMATION_FLAG_BLEND)) ?
			(TRUE) :
		(FALSE);

		//@{ Jaewon 20051124
		// If the previous animation was a social type, do the blending always.
		if(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SOCIAL
			|| pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT)
			*(pbDestBlend) = TRUE;
		//@} Jaewon
	}
}

//@{ Jaewon 20051129
// 'pfCustCallback' parameter added.
//@{ Jaewon 20051129
// 'fBlendingDuration' parameter added.
BOOL AgcmCharacter::SetBlendingAnimation(	AgpdCharacter			*pstAgpdCharacter,
										 AgcdCharacter			*pstAgcdCharacter,
										 AgcmCharacterAnimType	eNextAnimType,
										 FLOAT					fBlendingDuration,
										 AcCallbackData3			pfCustCallback		)
{
	if (!pstAgcdCharacter->m_pcsNextAnimData) return FALSE;

	ACA_AttachedData						*pcsAttachedData	= NULL;

	if (	(pstAgcdCharacter->m_eCurAnimType > AGCMCHAR_ANIM_TYPE_NONE)	&&
		(pstAgcdCharacter->m_eCurAnimType < AGCMCHAR_MAX_ANIM_TYPE)			)
	{
		pcsAttachedData	=
			(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
			AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
			pstAgcdCharacter->m_pcsCurAnimData				));
	}

	if(	(eNextAnimType == AGCMCHAR_ANIM_TYPE_WAIT) &&
		(pcsAttachedData) &&
		(pcsAttachedData->m_pcsBlendingData) &&
		(pcsAttachedData->m_pcsBlendingData->m_pcsRtAnim) &&
		(pcsAttachedData->m_pcsBlendingData->m_pcsRtAnim->m_pstAnimation)	)
	{
		pstAgcdCharacter->m_csAnimation.InitializeAnimation(
			pcsAttachedData->m_pcsBlendingData->m_pcsRtAnim->m_pstAnimation->duration	);

		pstAgcdCharacter->m_ulAnimationFlags |= AGCMCHAR_AF_BLEND_EX;
	}
	else
	{
		pstAgcdCharacter->m_csAnimation.InitializeAnimation(fBlendingDuration);
	}

	AgcdAnimationCallbackData	csData;
	SetAnimCBData(
		&csData,
		OnAnimCB,
		pstAgcdCharacter->m_csAnimation.GetDuration(),
		FALSE,
		pstAgpdCharacter->m_lID,
		eNextAnimType,
		AGCMCHAR_START_ANIM_CB_POINT_BLEND,
		TRUE,
		pfCustCallback	);
	AddAnimCallback(pstAgcdCharacter, &csData);

	// 다음 애니메이션이 무엇인지 설정한다.
	//pstAgcdCharacter->m_pstNextAnim		= pstNextAnim;

	// blend 관련 flag 설정.
	pstAgcdCharacter->m_ulAnimationFlags	|= AGCMCHAR_AF_BLEND;

	return TRUE;
}
//@} Jaewon
//@} Jaewon

BOOL AgcmCharacter::SetAnimSpeed(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter)
{
	// 공격속도를 설정한다.
	if (pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_ATTACK)
	{
		// 일단 속도를 초기화한다.
		pstAgcdCharacter->m_afAnimSpeedRate[AGCMCHAR_ANIM_TYPE_ATTACK]	= 1.0f;

		if (m_pcsAgpmCharacter->IsPC(pstAgpdCharacter))
		{
			if (!pstAgcdCharacter->m_pcsCurAnimData)
				return FALSE;
			if (!pstAgcdCharacter->m_pcsCurAnimData->m_pcsRtAnim)
				return FALSE;
			if (!pstAgcdCharacter->m_pcsCurAnimData->m_pcsRtAnim->m_pstAnimation)
				return FALSE;

			INT32	lNumAttackPerMin = 0;

			m_pcsAgpmFactors->GetValue(
				&pstAgpdCharacter->m_csFactor,
				&lNumAttackPerMin,
				AGPD_FACTORS_TYPE_RESULT,
				AGPD_FACTORS_TYPE_ATTACK,
				AGPD_FACTORS_ATTACK_TYPE_SPEED	);

			if (lNumAttackPerMin < 1)
				return TRUE; // 무시! -_-;;

			const FLOAT	fMinToSec		= 60.f;
			// 서버에 설정된 공격속도(초단위)
			FLOAT	fAttackTime			= fMinToSec / (FLOAT)(lNumAttackPerMin);
			// 데이터의 공격속도(초단위)
			//FLOAT	fDataAttackTime		= (FLOAT)(pstAgcdCharacter->m_pstCurAnim->m_ppastAnimData[pstAgcdCharacter->m_lCurAnimIndex]->m_pstAnim->duration);
			FLOAT	fDataAttackTime		= (FLOAT)(pstAgcdCharacter->m_pcsCurAnimData->m_pcsRtAnim->m_pstAnimation->duration);
			// 속도 비율(1.0보다 크면 데이터에 설정된 공격속도보다 빨라지고, 작으면 느려진다.)
			FLOAT	fSpeedRate			= fDataAttackTime / fAttackTime;

			// 속도 비율을 설정한다.
			pstAgcdCharacter->m_afAnimSpeedRate[AGCMCHAR_ANIM_TYPE_ATTACK]		= fSpeedRate;
		}
	}
	else if (pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_STRUCK)
	{
		pstAgcdCharacter->m_afAnimSpeedRate[AGCMCHAR_ANIM_TYPE_STRUCK]	= 1.0f;

		if (m_pcsAgpmCharacter->IsPC(pstAgpdCharacter))
		{
			pstAgcdCharacter->m_afAnimSpeedRate[AGCMCHAR_ANIM_TYPE_STRUCK]	= 1.3f;
		}
	}
	//@{ Jaewon 20050727
	// Speed up the running animation if the current factors tell it.
	else if (pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_RUN)
	{
		pstAgcdCharacter->m_afAnimSpeedRate[AGCMCHAR_ANIM_TYPE_RUN] = 1.0f;
		if (m_pcsAgpmCharacter->IsPC(pstAgpdCharacter))
		{
			INT32 lOriginalMoveFast = 0;
			if (pstAgpdCharacter->m_bIsTrasform)
			{
				AgpdCharacterTemplate *pcsOriginalTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(pstAgpdCharacter->m_lOriginalTID);
				if (pcsOriginalTemplate)
					m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csFactor, &lOriginalMoveFast, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
			}
			else if( pstAgcdCharacter->m_pRide )
			{
				AgpdCharacter* pCharacterRide = GetCharacter( pstAgcdCharacter->m_pRide );
				m_pcsAgpmFactors->GetValue(&pCharacterRide->m_pcsCharacterTemplate->m_csFactor, &lOriginalMoveFast, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
			}
			else
			{
				m_pcsAgpmFactors->GetValue(&pstAgpdCharacter->m_pcsCharacterTemplate->m_csFactor, &lOriginalMoveFast, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
			}

			INT32 lMoveFast = 0;
			m_pcsAgpmFactors->GetValue(&pstAgpdCharacter->m_csFactor, &lMoveFast, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			if( lMoveFast < 1.f )
				lMoveFast = lOriginalMoveFast;

			if( lOriginalMoveFast ) {
				pstAgcdCharacter->m_afAnimSpeedRate[AGCMCHAR_ANIM_TYPE_RUN] = (FLOAT)lMoveFast/(FLOAT)lOriginalMoveFast;
			}
		}
	}
	else if( pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SKILL ) {
		pstAgcdCharacter->m_fSkillSpeedRate	= 1.0f;

		pstAgcdCharacter->m_fSkillSpeedRate	-= m_pcsAgpmSkill->GetAdjustCastDelay( pstAgpdCharacter )/100.f;
//		pstAgcdCharacter->m_fSkillSpeedRate = 5.0f;
	}
	//@} Jaewon

	// dobal
	int i;
	for( i = 0; i < AGCMCHAR_ANIM_TYPE_ATTACK; i++ )
	{
		if( pstAgcdCharacter->m_pRide ) {
			pstAgcdCharacter->m_afAnimSpeedRate[i] *= pstAgcdCharacter->m_pRide->m_afDefaultAnimSpeedRate[i];
		}
		else {
			pstAgcdCharacter->m_afAnimSpeedRate[i] *= pstAgcdCharacter->m_afDefaultAnimSpeedRate[i];
		}
	}
	if( pstAgcdCharacter->m_pRide ) {
		for( i = 0; i < AGCMCHAR_ANIM_TYPE_ATTACK; i++ )
		{
			pstAgcdCharacter->m_pRide->m_afAnimSpeedRate[i] = pstAgcdCharacter->m_afAnimSpeedRate[i];
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::SetNextAnimation(	AgpdCharacter						*pstAgpdCharacter,
										AgcdCharacter						*pstAgcdCharacter,
										AgcmCharacterAnimType				eNextAnimType,
										BOOL								bLoop,
										AcCallbackData3						pfCustCallback )
{
	// Check validation
	if(	(!pstAgcdCharacter->m_pcsNextAnimData) ||
		(!pstAgcdCharacter->m_pcsNextAnimData->m_pcsRtAnim) ||
		(!pstAgcdCharacter->m_pcsNextAnimData->m_pcsRtAnim->m_pstAnimation)	) return FALSE;

	pstAgcdCharacter->m_csAnimation.InitializeAnimation(pstAgcdCharacter->m_pcsNextAnimData->m_pcsRtAnim->m_pstAnimation->duration);

	AgcdAnimationCallbackData	csData;
	AgcmCharacterAnimType		eCBDataAnimType;
	if( bLoop ) {
		eCBDataAnimType		= eNextAnimType;
	}
	else {
		eCBDataAnimType		= AGCMCHAR_ANIM_TYPE_WAIT;
	}

	SetAnimCBData(
		&csData,
		OnAnimCB,
		pstAgcdCharacter->m_csAnimation.GetDuration(),
		FALSE,
		pstAgpdCharacter->m_lID,
		eCBDataAnimType,
		AGCMCHAR_START_ANIM_CB_POINT_ANIMEND,
		TRUE,
		pfCustCallback );

	AddAnimCallback(pstAgcdCharacter, &csData);

	// 현재 애니메이션을 설정한다.
	pstAgcdCharacter->m_pcsCurAnimData	= pstAgcdCharacter->m_pcsNextAnimData;
	pstAgcdCharacter->m_pcsNextAnimData	= NULL;

	// 현재 애님 타입을 설정한다.
	pstAgcdCharacter->m_eCurAnimType	= eNextAnimType;

	// blend 관련 flag 설정.
	pstAgcdCharacter->m_ulAnimationFlags	&= ~AGCMCHAR_AF_BLEND;

	// animation 속도 설정.
	SetAnimSpeed(pstAgpdCharacter, pstAgcdCharacter);

	// 애니메이션 타입 변화에 따른 각종 콜백을 불러준다.
	AdminChangeAnim(pstAgpdCharacter, pstAgcdCharacter);
	return TRUE;
}

BOOL AgcmCharacter::StartAnimation(	AgpdCharacter *pstAgpdCharacter,
								   AgcdCharacter *pstAgcdCharacter,
								   AgcmCharacterAnimType eType,
								   AgcdCharacterAnimation *pstNextAnim,
								   BOOL bEndThenChange,
								   AgcmCharacterStartAnimCallbackPoint ePoint,
								   INT32 lCustAnimType							)
{
	if( pstAgcdCharacter->m_pRide ) {
		AgpdCharacter *pstAgpdCharacterRide = GetCharacter( pstAgcdCharacter->m_pRide );
		AgcdCharacter *pstAgcdCharacterRide = pstAgcdCharacter->m_pRide;
		StartAnimation(pstAgpdCharacterRide, pstAgcdCharacterRide, eType, bEndThenChange, ePoint, lCustAnimType);
	}
	
	eAcReturnType	eRt	= SearchSituation(pstAgpdCharacter, pstAgcdCharacter, eType);
	if (eRt == E_AC_RETURN_TYPE_SKIP)
		return TRUE; // skip!
	else if (eRt == E_AC_RETURN_TYPE_FAILURE)
		return FALSE;

	//. 2006. 3. 28. nonstopdj
	//. AGCMCHAR_ANIM_TYPE_RUN인 상태에서 STRUCK Animation은 무시.
	if(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_RUN && eType == AGCMCHAR_ANIM_TYPE_STRUCK)
		return TRUE;

	// 같은 애니매이션에 대한 체크
	BOOL bIdenticalAnimation = FALSE;

	eRt	= CheckStartAnimCallback( pstAgpdCharacter,	pstAgcdCharacter, eType, pstNextAnim, &bIdenticalAnimation,	&bEndThenChange, ePoint, lCustAnimType );
	if (eRt == E_AC_RETURN_TYPE_SKIP) return TRUE;
	else if (eRt == E_AC_RETURN_TYPE_FAILURE) return FALSE;

	// 블랜드를 할것인지?
	BOOL	bBlend = FALSE;

	SetAnimOption(
		&bBlend,
		&bEndThenChange,
		pstAgcdCharacter,
		(pstNextAnim) ? (pstNextAnim->m_pstAnimFlags) : (NULL),
		bIdenticalAnimation			);

	if( !bEndThenChange ) {// bEndThenChange가 TRUE인 경우는 현재 애니메이션이 끝난 후에 셋팅한다.
		pstAgcdCharacter->m_ulAnimationFlags	= AGCMCHAR_AF_NONE;

		// 블랜딩 설정
		if( bBlend ) {
			if (!SetBlendingAnimation(pstAgpdCharacter, pstAgcdCharacter, eType)) return FALSE;
		}
		else {
			BOOL bLoop = FALSE;
			if(	(pstNextAnim) && (pstNextAnim->m_pstAnimFlags) && (pstNextAnim->m_pstAnimFlags->m_unAnimFlag & AGCD_ANIMATION_FLAG_LOOP) ) {
				bLoop = TRUE;
			}

			//.if(((*(ApdCharacter*)(&*pstAgpdCharacter))).m_lTID1 == 1258)
			//.	bLoop = TRUE;

			if( !SetNextAnimation(pstAgpdCharacter, pstAgcdCharacter, eType, bLoop) ) return FALSE;
		}

		// 기타 설정을 한다.
		pstAgcdCharacter->m_bStop				= FALSE;
	}
	else {
		SetEndThenChangeAnimation(pstAgpdCharacter, pstAgcdCharacter, eType);
	}
	return TRUE;
}

BOOL AgcmCharacter::StartAnimation(INT32 lCID, AgcmCharacterAnimType eType, BOOL bEndThenChange, AgcmCharacterStartAnimCallbackPoint ePoint, INT32 lCustAnimType)
{
	AgpdCharacter *pstAgpdCharacter = m_pcsAgpmCharacter ? m_pcsAgpmCharacter->GetCharacter(lCID) : NULL;
	if(!pstAgpdCharacter) return FALSE;
	return StartAnimation(pstAgpdCharacter, eType, bEndThenChange, ePoint, lCustAnimType);
}

BOOL AgcmCharacter::StartAnimation(AgpdCharacter *pstAgpdCharacter, AgcmCharacterAnimType eType, BOOL bEndThenChange, AgcmCharacterStartAnimCallbackPoint ePoint, INT32 lCustAnimType)
{
	AgcdCharacter *pstAgcdCharacter = GetCharacterData(pstAgpdCharacter);
	if( !pstAgcdCharacter ) return FALSE;

	BOOL bResult = StartAnimation(pstAgpdCharacter, pstAgcdCharacter, eType, bEndThenChange, ePoint, lCustAnimType);
	if( bResult == FALSE )
		return FALSE;

	return bResult;
}

BOOL AgcmCharacter::StartAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, AgcmCharacterAnimType eType, BOOL bEndThenChange, AgcmCharacterStartAnimCallbackPoint ePoint, INT32 lCustAnimType)
{
	if(!pstAgcdCharacter->m_pstAgcdCharacterTemplate)
		return FALSE;

	// netong 추가
	//	eType이 음수일경우 체크를 안하고 m_paAnim[eType] 이런식의 사용을 하고 있음
	if (eType < 0 || pstAgcdCharacter->m_lCurAnimType2 < 0) return FALSE;
	if (!pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData)	return FALSE;

	//@{ 2006/09/29 burumal
	if ( eType == AGCMCHAR_ANIM_TYPE_WALK && pstAgcdCharacter->m_lCurAnimType2 == 0 )
	{
		if ( m_pcsAgpmCharacter->IsPC(pstAgpdCharacter) && pstAgcdCharacter->m_pRide )
		{
			AgcmItem* pAgcmItem = (AgcmItem*) GetModule("AgcmItem");
			if ( pAgcmItem )
			{
				if ( pAgcmItem->GetAgpmItem() )
				{
					pAgcmItem->SetEquipAnimType(pstAgpdCharacter, pstAgcdCharacter);
				}
			}
		}
	}	
	//@}

	AgcdCharacterAnimation	*pcsNextAnim = pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[eType][pstAgcdCharacter->m_lCurAnimType2];
	if( (!pcsNextAnim) || 
		(!pcsNextAnim->m_pcsAnimation) ||
		(!pcsNextAnim->m_pcsAnimation->m_pcsHead) ||
		(!pcsNextAnim->m_pcsAnimation->m_pcsHead->m_pcsRtAnim) ||
		(!pcsNextAnim->m_pcsAnimation->m_pcsHead->m_pcsRtAnim->m_pstAnimation) )
	{
		//@{ 2006/09/29 burumal
		INT32 lTempAnimType2 = 0;
		/*
		pcsNextAnim	= pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[eType][lTempAnimType2];
		*/
		if ( eType == AGCMCHAR_ANIM_TYPE_WALK && m_pcsAgpmCharacter->IsPC(pstAgpdCharacter) )
		{
			eType = AGCMCHAR_ANIM_TYPE_RUN;
			pcsNextAnim = pstAgcdCharacter->m_pstAgcdCharacterTemplate->GetAnimation( eType , pstAgcdCharacter->m_lCurAnimType2 );

			if( (!pcsNextAnim) || 
				(!pcsNextAnim->m_pcsAnimation) ||
				(!pcsNextAnim->m_pcsAnimation->m_pcsHead) ||
				(!pcsNextAnim->m_pcsAnimation->m_pcsHead->m_pcsRtAnim) ||
				(!pcsNextAnim->m_pcsAnimation->m_pcsHead->m_pcsRtAnim->m_pstAnimation) )
			{
				pcsNextAnim	= pstAgcdCharacter->m_pstAgcdCharacterTemplate->GetAnimation( eType , lTempAnimType2 );
			}
			else
			{
				lTempAnimType2 = pstAgcdCharacter->m_lCurAnimType2;
			}
		}
		else
		{
			pcsNextAnim	= pstAgcdCharacter->m_pstAgcdCharacterTemplate->GetAnimation( eType , lTempAnimType2 );
		}
		//@}
		
		if(	(!pcsNextAnim) || 
			(!pcsNextAnim->m_pcsAnimation) ||
			(!pcsNextAnim->m_pcsAnimation->m_pcsHead) ||
			(!pcsNextAnim->m_pcsAnimation->m_pcsHead->m_pcsRtAnim) ||
			(!pcsNextAnim->m_pcsAnimation->m_pcsHead->m_pcsRtAnim->m_pstAnimation)	)
		{
			return FALSE;
		}

		// pstAgcdCharacter->m_lCurAnimType2 = lTempAnimType2;
	}

	if( StartAnimation(pstAgpdCharacter, pstAgcdCharacter, eType, pcsNextAnim, bEndThenChange, ePoint, lCustAnimType) == FALSE ) {
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmCharacter::StartAnimation(AgpdCharacter *pstAgpdCharacter, AgcmCharacterAnimType eType, AgcdCharacterAnimation *pstNextAnim, BOOL bEndThenChange, AgcmCharacterStartAnimCallbackPoint ePoint, INT32 lCustAnimType)
{
	AgcdCharacter *pstAgcdCharacter = GetCharacterData(pstAgpdCharacter);
	if(!pstAgcdCharacter) return FALSE;
	return StartAnimation(pstAgpdCharacter, pstAgcdCharacter, eType, pstNextAnim, bEndThenChange, ePoint, lCustAnimType);
}

BOOL AgcmCharacter::UpdateAnimationTimeForAllCharacter()
{
	AgpdCharacter		*pstAgpdCharacter;
	AgcdCharacter		*pstAgcdCharacter;
	INT32				lIndex = 0;
	UINT32				ulDeltaTime;
	RwReal				fOffset, fAlpha;

	for (	pstAgpdCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
		pstAgpdCharacter;
		pstAgpdCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex)					)
	{
		pstAgcdCharacter		= GetCharacterData(pstAgpdCharacter);
		if (	(!pstAgcdCharacter) ||
			(pstAgcdCharacter->m_bStop) ||
			//				(!pstAgcdCharacter->m_pstCurAnim) ||
			(!pstAgcdCharacter->m_pcsCurAnimData) ||
			(pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_NONE) ||
			(pstAgcdCharacter->m_csSubAnim.m_bStopMainAnimation)				)
		{ // error
			continue;
		}

		ulDeltaTime = m_ulCurTick - pstAgcdCharacter->m_ulPrevTime;

		if (pstAgcdCharacter->m_ulPrevTime == 0)
			ulDeltaTime = 0;
		/*		else if (	(pstAgcdCharacter->m_pInHierarchy) &&
		(pstAgcdCharacter->m_pInHierarchy->currentAnim) && 
		(pstAgcdCharacter->m_pInHierarchy->currentAnim->pCurrentAnim) &&
		(ulDeltaTime > pstAgcdCharacter->m_pInHierarchy->currentAnim->pCurrentAnim->duration * 2000)	)
		ulDeltaTime = 0;*/

		if (pstAgcdCharacter->m_lLastAttackTime > -1)
		{
			pstAgcdCharacter->m_lLastAttackTime += ulDeltaTime;

			if (pstAgcdCharacter->m_lLastAttackTime >= AGCMCHAR_MAX_ATTACK_TIME)
			{
				pstAgcdCharacter->m_lLastAttackTime	= -1;
			}
		}

		if (	(pstAgcdCharacter->m_ulAnimationFlags & AGCMCHAR_AF_BLEND) &&
			(!(pstAgcdCharacter->m_ulAnimationFlags & AGCMCHAR_AF_BLEND_EX))	)
		{
			//			pstAgcdCharacter->m_ulBlendingTime += ulDeltaTime;

			if( pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SKILL ) {
				fOffset = ((RwReal)((AGCMCHAR_DEFAULT_ANIM_BLEND_DURATION + 0.0f) / pstAgcdCharacter->m_fSkillSpeedRate));
			}
			else {
				fOffset =
					(pstAgcdCharacter->m_eCurAnimType < AGCMCHAR_ANIM_TYPE_CUST_START) ?
					(AGCMCHAR_DEFAULT_ANIM_BLEND_DURATION + 0.0f) :
				((RwReal)((AGCMCHAR_DEFAULT_ANIM_BLEND_DURATION + 0.0f) / pstAgcdCharacter->m_afAnimSpeedRate[pstAgcdCharacter->m_eCurAnimType]));
				//				(pstAgcdCharacter->m_ulBlendingTime) ? ((RwReal)((pstAgcdCharacter->m_ulBlendingTime + 0.0f) / fOffset)) : (0.0f);
			}
			fAlpha = (ulDeltaTime) ? ((RwReal)((ulDeltaTime + 0.0f) / fOffset)) : (0.0f);
		}
		else
		{
			if( pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_SKILL ) {
				fOffset	=((RwReal)(pstAgcdCharacter->m_fSkillSpeedRate));
			}
			else {
				fOffset	=
					(pstAgcdCharacter->m_eCurAnimType < AGCMCHAR_ANIM_TYPE_CUST_START) ?
					(1.0f) :
				((RwReal)(pstAgcdCharacter->m_afAnimSpeedRate[pstAgcdCharacter->m_eCurAnimType]/* + pstAgcdCharacter->m_fLinkAnimSpeed*/));
			}
			fAlpha	= (ulDeltaTime + 0.0f) / (1000.0f / fOffset);
		}

		{
			PROFILE("Animation.AddTime");
			pstAgcdCharacter->m_csAnimation.AddTime(fAlpha);
		}

		pstAgcdCharacter->m_ulPrevTime =  m_ulCurTick;
	}

	return TRUE;
}

BOOL AgcmCharacter::UpdateMainAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, UINT32 ulDeltaTime)
{
	if (pstAgcdCharacter->m_bStop || !pstAgcdCharacter->m_pClump || !pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pClump)
		return TRUE; // skip

	if (!pstAgcdCharacter->m_pInHierarchy || IsBadReadPtr( pstAgcdCharacter->m_pInHierarchy , sizeof ( RpHAnimHierarchy ) ) )
		return TRUE; // skip

	RpHAnimHierarchy * pHierarchy = GetHierarchy( pstAgcdCharacter->m_pClump );
	if( pstAgcdCharacter->m_pInHierarchy != pHierarchy ||
		pstAgcdCharacter->m_pOutHierarchy->parentFrame != pstAgcdCharacter->m_pInHierarchy->parentFrame ||
		pstAgcdCharacter->m_pInHierarchy2->parentFrame != pstAgcdCharacter->m_pInHierarchy->parentFrame )
	{
		// Hierarchy를 설정한다~
		pstAgcdCharacter->m_pInHierarchy = pHierarchy;
		if (!pstAgcdCharacter->m_pInHierarchy)
		{
			return FALSE;
		}

		RpHAnimHierarchySetFlags(pstAgcdCharacter->m_pInHierarchy,
								 (RpHAnimHierarchyFlag)
								 ( RpHAnimHierarchyGetFlags(pstAgcdCharacter->m_pInHierarchy) | 
								   rpHANIMHIERARCHYUPDATELTMS |
								   rpHANIMHIERARCHYUPDATEMODELLINGMATRICES ) );// | rpHANIMHIERARCHYLOCALSPACEMATRICES) );

		RpHAnimHierarchyFlag flags = (RpHAnimHierarchyFlag) (pstAgcdCharacter->m_pInHierarchy)->flags;

		LockFrame();

		pstAgcdCharacter->m_pOutHierarchy = RpHAnimHierarchyCreateFromHierarchy(pstAgcdCharacter->m_pInHierarchy,
			flags, AGCMCHAR_HIERARCHY_MAX_KEYFRAME);
		pstAgcdCharacter->m_pInHierarchy2 = RpHAnimHierarchyCreateFromHierarchy(pstAgcdCharacter->m_pInHierarchy,
			flags, AGCMCHAR_HIERARCHY_MAX_KEYFRAME);

		pstAgcdCharacter->m_pOutHierarchy->parentFrame = pstAgcdCharacter->m_pInHierarchy->parentFrame;
		pstAgcdCharacter->m_pInHierarchy2->parentFrame = pstAgcdCharacter->m_pInHierarchy->parentFrame;

		RpHAnimHierarchySetKeyFrameCallBacks(pstAgcdCharacter->m_pOutHierarchy, rpHANIMSTDKEYFRAMETYPEID);

		RpHAnimHierarchyAttach(pstAgcdCharacter->m_pInHierarchy);
		RpHAnimHierarchyAttach(pstAgcdCharacter->m_pInHierarchy2);
		RpHAnimHierarchyAttach(pstAgcdCharacter->m_pOutHierarchy);

		UnlockFrame();
	}

	if (!pstAgcdCharacter->m_pcsCurAnimData)
	{
		if (pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_DEAD)
		{
			return SetDeadAnimation(pstAgpdCharacter, pstAgcdCharacter);
		}
		else
		{
			return FALSE;
		}
	}

	if (pstAgcdCharacter->m_ulAnimationFlags & AGCMCHAR_AF_BLEND)			
	{
		if (pstAgcdCharacter->m_ulAnimationFlags & AGCMCHAR_AF_BLEND_EX)
		{
			ACA_AttachedData	*pstAnimAttachedData	=
				(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
				pstAgcdCharacter->m_pcsCurAnimData					));

			if (	(!pstAnimAttachedData) ||
				(!pstAnimAttachedData->m_pcsBlendingData) ||
				(!pstAnimAttachedData->m_pcsBlendingData->m_pcsRtAnim)	)
				return FALSE;

			if (	pstAnimAttachedData->m_pcsBlendingData->m_pcsRtAnim->m_pstAnimation !=
				RpHAnimHierarchyGetCurrentAnim(pstAgcdCharacter->m_pInHierarchy)		)
			{
				RpHAnimHierarchySetCurrentAnim(
					pstAgcdCharacter->m_pInHierarchy,
					pstAnimAttachedData->m_pcsBlendingData->m_pcsRtAnim->m_pstAnimation	);

				RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, SetHierarchyForSkinAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));

				pstAgcdCharacter->m_lStatus				|= AGCMCHAR_STATUS_FLAG_ATTACHED_HIERARCHY;
			}

			RpHAnimHierarchySetCurrentAnimTime(pstAgcdCharacter->m_pInHierarchy, pstAgcdCharacter->m_csAnimation.m_fCurrentTime);
			LockFrame();
			RpHAnimHierarchyUpdateMatrices(pstAgcdCharacter->m_pInHierarchy);
			UnlockFrame();
		}
		else
		{
			if (	(!pstAgcdCharacter->m_pcsCurAnimData->m_pcsRtAnim->m_pstAnimation) ||
				(!pstAgcdCharacter->m_pcsNextAnimData) ||
				(!pstAgcdCharacter->m_pcsNextAnimData->m_pcsRtAnim) ||
				(!pstAgcdCharacter->m_pcsNextAnimData->m_pcsRtAnim->m_pstAnimation)		)
			{
				DropAnimation(pstAgcdCharacter);
				return FALSE;
			}

			RtAnimAnimation	*pstCurAnim1, *pstCurAnim2;

			pstCurAnim1	= RpHAnimHierarchyGetCurrentAnim(pstAgcdCharacter->m_pInHierarchy);
			pstCurAnim2 = RpHAnimHierarchyGetCurrentAnim(pstAgcdCharacter->m_pInHierarchy2);

			if (pstCurAnim1 != pstAgcdCharacter->m_pcsCurAnimData->m_pcsRtAnim->m_pstAnimation)
			{
				RpHAnimHierarchySetCurrentAnim(pstAgcdCharacter->m_pInHierarchy,
					pstAgcdCharacter->m_pcsCurAnimData->m_pcsRtAnim->m_pstAnimation	);
				RpHAnimHierarchySetCurrentAnimTime(pstAgcdCharacter->m_pInHierarchy, 
					pstAgcdCharacter->m_pcsCurAnimData->m_pcsRtAnim->m_pstAnimation->duration	);
			}

			if (pstCurAnim2 != pstAgcdCharacter->m_pcsNextAnimData->m_pcsRtAnim->m_pstAnimation)
			{
				RpHAnimHierarchySetCurrentAnim(pstAgcdCharacter->m_pInHierarchy2,
					pstAgcdCharacter->m_pcsNextAnimData->m_pcsRtAnim->m_pstAnimation	);
				RpHAnimHierarchySetCurrentAnimTime(pstAgcdCharacter->m_pInHierarchy2, 0.0f);
			}

			RpHAnimHierarchyBlend(
				pstAgcdCharacter->m_pOutHierarchy,
				pstAgcdCharacter->m_pInHierarchy,
				pstAgcdCharacter->m_pInHierarchy2,
				//@{ Jaewon 20051129
				// The duration may not be 1.0f, so we should divide the current time by the duration.
				pstAgcdCharacter->m_csAnimation.m_fCurrentTime/pstAgcdCharacter->m_csAnimation.m_fDuration	);
				//@} Jaewon

			if (!pstAgcdCharacter->m_bSetBlendingHierarchy)
			{
				RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, SetHierarchyForSkinAtomic, (void *)(pstAgcdCharacter->m_pOutHierarchy));

				pstAgcdCharacter->m_bSetBlendingHierarchy = TRUE;

				pstAgcdCharacter->m_lStatus				|= AGCMCHAR_STATUS_FLAG_ATTACHED_HIERARCHY;
			}

			LockFrame();
			RpHAnimHierarchyUpdateMatrices(pstAgcdCharacter->m_pOutHierarchy);
			UnlockFrame();
		}
	}
	else
	{
		if ( pstAgcdCharacter->m_pcsCurAnimData										&&	
			pstAgcdCharacter->m_pcsCurAnimData ->m_pcsRtAnim						&&
			(pstAgcdCharacter->m_pcsCurAnimData->m_pcsRtAnim->m_pstAnimation !=
			RpHAnimHierarchyGetCurrentAnim(pstAgcdCharacter->m_pInHierarchy)	)	||
			(pstAgcdCharacter->m_bSetBlendingHierarchy)									)
		{
			RpHAnimHierarchySetCurrentAnim(
				pstAgcdCharacter->m_pInHierarchy,
				pstAgcdCharacter->m_pcsCurAnimData->m_pcsRtAnim->m_pstAnimation	);

			RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, SetHierarchyForSkinAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));

			pstAgcdCharacter->m_bSetBlendingHierarchy = FALSE;

			pstAgcdCharacter->m_lStatus				|= AGCMCHAR_STATUS_FLAG_ATTACHED_HIERARCHY;
		}

		if( pstAgcdCharacter->m_pInHierarchy->currentAnim->pCurrentAnim )
		{
			if(	!IsBadReadPtr( pstAgcdCharacter->m_pInHierarchy->currentAnim->pCurrentAnim->pFrames , sizeof RwFrame			) &&
				!IsBadReadPtr( pstAgcdCharacter->m_pInHierarchy->currentAnim->pCurrentAnim->interpInfo , sizeof RtAnimInterpolatorInfo			)&&
				pstAgcdCharacter->m_pInHierarchy->currentAnim->currentTime <= pstAgcdCharacter->m_csAnimation.m_fDuration )
			{
				RpHAnimHierarchySetCurrentAnimTime(pstAgcdCharacter->m_pInHierarchy, pstAgcdCharacter->m_csAnimation.m_fCurrentTime);

				LockFrame();
				RpHAnimHierarchyUpdateMatrices(pstAgcdCharacter->m_pInHierarchy);
				UnlockFrame();
			}
		}
	}

	return TRUE;
}


BOOL AgcmCharacter::UpdateSubAnimation(AgcdCharacter* pstAgcdCharacter, UINT32 ulDeltaTime)
{
	// 캐릭터 애니메이션이 정지되어 있는지 검사.
	if (pstAgcdCharacter->m_bStop)
		return TRUE; // skip

	// 서브 애니메이션이 비활성 되어 있는지 검사.
	if (pstAgcdCharacter->m_csSubAnim.m_ulDuration == 0)
		return TRUE; // skip

	pstAgcdCharacter->m_csSubAnim.m_ulCurrentTime	+= ulDeltaTime;
	if (pstAgcdCharacter->m_csSubAnim.m_ulCurrentTime > pstAgcdCharacter->m_csSubAnim.m_ulDuration)
	{
		pstAgcdCharacter->m_csSubAnim.InitComponent();
	}
	else
	{
		if (pstAgcdCharacter->m_csSubAnim.m_fForceTime >= 0.0f)
		{
			RpHAnimHierarchySetCurrentAnimTime(
				pstAgcdCharacter->m_csSubAnim.m_pInHierarchy,
				pstAgcdCharacter->m_csSubAnim.m_fForceTime		);
		}
		else
		{
			RpHAnimHierarchySetCurrentAnimTime(
				pstAgcdCharacter->m_csSubAnim.m_pInHierarchy,
				pstAgcdCharacter->m_csSubAnim.m_ulCurrentTime / 1000.0f	);
		}

		RpHAnimHierarchyUpdateMatrices(pstAgcdCharacter->m_csSubAnim.m_pInHierarchy);
	}

	return TRUE;
}


BOOL AgcmCharacter::DropAnimation(AgcdCharacter *pstAgcdCharacter)
{
	pstAgcdCharacter->m_pcsCurAnimData		= NULL;
	pstAgcdCharacter->m_eCurAnimType		= AGCMCHAR_ANIM_TYPE_NONE;
	pstAgcdCharacter->m_pcsNextAnimData		= NULL;

	pstAgcdCharacter->m_bStop				= TRUE;
	pstAgcdCharacter->m_ulAnimationFlags	= AGCMCHAR_AF_NONE;

	if (pstAgcdCharacter->m_pClump)
		RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, SetHierarchyForSkinAtomic, NULL);

	pstAgcdCharacter->m_lStatus				&= ~AGCMCHAR_STATUS_FLAG_ATTACHED_HIERARCHY;

	return TRUE;
}

BOOL AgcmCharacter::SetEndThenChangeAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, AgcmCharacterAnimType eType)
{
	AgcdAnimationCallbackData	csData;
	SetAnimCBData(
		&csData,
		OnAnimCB,
		pstAgcdCharacter->m_csAnimation.GetDuration(),
		FALSE,
		pstAgpdCharacter->m_lID,
		eType,
		//		pstNextAnim,
		AGCMCHAR_START_ANIM_CB_POINT_ANIMENDTHENCHANGE,
		TRUE											);

	return AddAnimCallback(pstAgcdCharacter, &csData);//pstAgcdCharacter->m_csAnimation.AddAnimCB(&csData);
}

VOID AgcmCharacter::InitAnimData(AgcdCharacter *pstAgcdCharacter, AgcdCharacterAnimation *pcsAnimation)
{
	pstAgcdCharacter->m_pcsNextAnimData	=
		((pcsAnimation) && (pcsAnimation->m_pcsAnimation)) ?
		(pcsAnimation->m_pcsAnimation->m_pcsHead) :
	(NULL);
}

// 초단위로 넘겨준다! (실패시 -1.0f를 넘겨준다.)
FLOAT AgcmCharacter::GetCurrentAnimDuration(AgcdCharacter *pstAgcdCharacter)
{
	return pstAgcdCharacter->m_csAnimation.GetDuration();
}

FLOAT AgcmCharacter::GetAnimDuration(AgcdAnimation2 *pstAnim, INT32 lAnimIndex)
{
	AgcdAnimData2	*pcsData	= m_csAnimation2.GetAnimData(pstAnim, lAnimIndex);
	if (	(pcsData) &&
		(pcsData->m_pcsRtAnim) &&
		(pcsData->m_pcsRtAnim->m_pstAnimation)	)
	{
		return pcsData->m_pcsRtAnim->m_pstAnimation->duration;
	}

	return 0.0f;
}

INT32 AgcmCharacter::GetAnimType2Num(AgcdCharacterTemplate *pstAgcdCharacterTemplate)
{
	switch (pstAgcdCharacterTemplate->m_lAnimType2)
	{
	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		return AGCMCHAR_AT2_BASE_NUM;

	case AGCMCHAR_AT2_WARRIOR:
		return AGCMCHAR_AT2_WARRIR_NUM;

	case AGCMCHAR_AT2_ARCHER:
		return AGCMCHAR_AT2_ARCHER_NUM;

	case AGCMCHAR_AT2_WIZARD:
		return AGCMCHAR_AT2_WIZARD_NUM;

	//@{ 2006/06/07 burumal
	case AGCMCHAR_AT2_ARCHLORD :
		return AGCMCHAR_AT2_ARCHLORD_NUM;
	//@}
	}

	return -1;
}

BOOL AgcmCharacter::AllocateAnimationData(AgcdCharacterTemplate *pstAgcdCharacterTemplate)
{
	if (pstAgcdCharacterTemplate->m_pacsAnimationData)
	{
		ASSERT(!"pstAgcdCharacterTemplate->m_pacsAnimationData");
		return FALSE;
	}

	INT32	lNumAnimType2	= GetAnimType2Num(pstAgcdCharacterTemplate);

	pstAgcdCharacterTemplate->m_pacsAnimationData	= new AgcdCharacterAnimation** [AGCMCHAR_MAX_ANIM_TYPE];
	for (INT32 lAnimType1 = 0; lAnimType1 < AGCMCHAR_MAX_ANIM_TYPE; ++lAnimType1)
	{
		pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType1]	= new AgcdCharacterAnimation* [lNumAnimType2];
		memset(
			pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType1],
			0,
			sizeof (AgcdCharacterAnimation *) * lNumAnimType2			);
	}

	return TRUE;
}

VOID AgcmCharacter::SetAnimCBData(	AgcdAnimationCallbackData *pcsData,
								  __AnimationCallback pfCallback,
								  FLOAT fTime,
								  BOOL bLoop,
								  INT32 lCID,
								  AgcmCharacterAnimType eAnimType,
								  AgcmCharacterStartAnimCallbackPoint eStartAnimCBPoint,
								  BOOL bInitAnim,
								  AcCallbackData3 pfCustCallbak,
								  INT32 lCustAnimType )
{
	pcsData->m_pfCallback									= pfCallback;
	pcsData->m_fTime										= fTime;
	pcsData->m_bLoop										= bLoop;

	pcsData->m_pavData[AGCMCHAR_ACDA_CHARACTER_ID]			= (PVOID)(lCID);
	pcsData->m_pavData[AGCMCHAR_ACDA_ANIM_TYPE]				= (PVOID)(eAnimType);
	pcsData->m_pavData[AGCMCHAR_ACDA_START_ANIM_CB_POINT]	= (PVOID)(eStartAnimCBPoint);
	pcsData->m_pavData[AGCMCHAR_AGCD_CUST_CALLBACK]			= (PVOID)(pfCustCallbak);
	pcsData->m_pavData[AGCMCHAR_ACDA_CUST_ANIM_TYPE]		= (PVOID)(lCustAnimType);
	pcsData->m_pavData[AGCMCHAR_AGCD_INIT_ANIM]				= (PVOID)(bInitAnim);
}

BOOL AgcmCharacter::AddAnimCallback(AgcdCharacter *pstAgcdCharacter, AgcdAnimationCallbackData *pcsData)
{
	return pstAgcdCharacter->m_csAnimation.AddAnimCB(pcsData);
}

PVOID AgcmCharacter::RemoveAnimAttachedDataCB(PVOID pvData1, PVOID pvData2)
{
	return NULL;
}

PVOID AgcmCharacter::OnAnimCB(PVOID pvData)
{
	if (!pvData)
		return NULL;

	PVOID								*ppvData			= (PVOID *)(pvData);

	INT32								lCID				= (INT32)(ppvData[AGCMCHAR_ACDA_CHARACTER_ID]);
	AgcmCharacterAnimType				eAnimType			= (AgcmCharacterAnimType)((INT32)(ppvData[AGCMCHAR_ACDA_ANIM_TYPE]));
	AgcmCharacterStartAnimCallbackPoint	eStartAnimCBPoint	= (AgcmCharacterStartAnimCallbackPoint)((INT32)(ppvData[AGCMCHAR_ACDA_START_ANIM_CB_POINT]));
	AcCallbackData3						pvCustCallback		= (AcCallbackData3)(ppvData[AGCMCHAR_AGCD_CUST_CALLBACK]);
	INT32								lCustAnimType		= (INT32)(ppvData[AGCMCHAR_ACDA_CUST_ANIM_TYPE]);
	BOOL								bInitAnim			= (BOOL)(ppvData[AGCMCHAR_AGCD_INIT_ANIM]);

	AgpdCharacter	*pstAgpdCharacter	= AgcmCharacter::m_pThisAgcmCharacter->m_pcsAgpmCharacter->GetCharacter(lCID);
	if (!pstAgpdCharacter)
		return (PVOID)(bInitAnim);

	AgcdCharacter	*pstAgcdCharacter	= AgcmCharacter::m_pThisAgcmCharacter->GetCharacterData(pstAgpdCharacter);
	if (!pstAgcdCharacter)
		return (PVOID)(bInitAnim);
	
	if (pstAgcdCharacter->m_csAnimation.GetDuration() == pstAgcdCharacter->m_csAnimation.m_fCurrentTime)	
	{
		// 현재 애니매이션이 죽는 애니매이션이었다면, 캐릭터 애니매이션을 정지시킨다.	
		if ((pstAgcdCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_DEAD) && (pstAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD))
		{	
			return (PVOID)(bInitAnim);
		}
	}

	//@{ 2006/09/07 burumal	
	ACA_AttachedData* pstAttachedData = 
		(ACA_AttachedData*) (AgcmCharacter::m_pThisAgcmCharacter->m_csAnimation2.GetAttachedData(AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY, pstAgcdCharacter->m_pcsCurAnimData));

	if ( pstAttachedData )
	{
		if ( pstAgcdCharacter->m_csAnimation.GetDuration() + pstAttachedData->m_lClumpHideOffsetTime * 0.001f <= pstAgcdCharacter->m_csAnimation.m_fCurrentTime )
		{			
			if ( pstAttachedData->m_unCustFlags & AGCMCHAR_ACF_CLUMP_HIDE_END_TIME )
			{
				//if ( pstAgcdCharacter->m_pClump && (pstAgcdCharacter->m_cTransparentType == (INT8) AGCMCHAR_TRANSPARENT_NONE) )
				if (pstAgcdCharacter->m_pClump && (pstAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_FULL))
					AgcmCharacter::m_pThisAgcmCharacter->ShowClumpForced(lCID, FALSE, pstAttachedData->m_uClumpFadeOutTime);
			}
		}
	}
	//@}

	// 기본 애니매이션을 불러준다.
	if ((eAnimType > AGCMCHAR_ANIM_TYPE_NONE) && (eAnimType < AGCMCHAR_MAX_ANIM_TYPE))
	{
		AgcmCharacter::m_pThisAgcmCharacter->StartAnimation(
			pstAgpdCharacter,
			pstAgcdCharacter,
			eAnimType,
			FALSE,
			eStartAnimCBPoint,
			lCustAnimType										);
	}
	//@{ Jaewon 20051115
	// In case of a repeating social animation, reset the animation timer. 
	else if(eAnimType == AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT)
	{
		pstAgcdCharacter->m_csAnimation.m_fCurrentTime = 0.0f;
	}
	//@} Jaewon

	if (pvCustCallback)
		pvCustCallback(AgcmCharacter::m_pThisAgcmCharacter, pstAgpdCharacter, pstAgcdCharacter);

	return (PVOID)(bInitAnim);
}

RtAnimInterpolator *AgcmCharacter::OnAnimEndCB(RtAnimInterpolator *animInstance, PVOID pvData)
{
	return animInstance;
}

RtAnimInterpolator *AgcmCharacter::OnAnimEndThenChangeCB(RtAnimInterpolator *animInstance, PVOID pvData)
{
	return animInstance;
}

RtAnimInterpolator	*AgcmCharacter::OnSubAnimEndCB(RtAnimInterpolator *animInstance, PVOID pvData)
{
	*((BOOL *)(pvData))		= TRUE;	

	return animInstance;
}

RtAnimInterpolator	*AgcmCharacter::OnSubAnimEndThenChangeCB(RtAnimInterpolator *animInstance, PVOID pvData)
{
	return animInstance;
}

BOOL AgcmCharacter::IntervalMainAnimCB(PVOID pvData)
{
	return TRUE;
}

BOOL AgcmCharacter::IntervalSubAnimCB(PVOID pvData)
{
	return TRUE;
}

BOOL AgcmCharacter::CopyAnimAttachedData(AgcdAnimation2 *pcsSrc, AgcdAnimation2 *pcsDest)
{
	if (	(!pcsSrc)	||
		(!pcsDest)		)
		return FALSE;

	AgcdAnimData2	*pcsSrcData		= pcsSrc->m_pcsHead;
	AgcdAnimData2	*pcsDestData	= pcsDest->m_pcsHead;

	ACA_AttachedData	*pcsSrcAttachedData		= NULL;
	ACA_AttachedData	*pcsDestAttachedData	= NULL;

	while (pcsSrcData)
	{
		if (!pcsDestData)
			return FALSE;

		pcsSrcAttachedData	=
			(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
			AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
			pcsSrcData										));

		pcsDestAttachedData	=
			(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
			AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
			pcsDestData										));

		if (	(!pcsSrcAttachedData)	||
			(!pcsDestAttachedData)		)
			return FALSE;

		pcsDestAttachedData->m_nCustType		= pcsSrcAttachedData->m_nCustType;
		pcsDestAttachedData->m_unActiveRate		= pcsSrcAttachedData->m_unActiveRate;
		pcsDestAttachedData->m_unCustFlags		= pcsSrcAttachedData->m_unCustFlags;
		//@{ 2006/09/08 burumal
		pcsDestAttachedData->m_uClumpFadeOutTime= pcsSrcAttachedData->m_uClumpFadeOutTime;
		//@}

		{
			if (pcsDestAttachedData->m_pszPoint)
				delete [] pcsDestAttachedData->m_pszPoint;

			if (pcsSrcAttachedData->m_pszPoint)
			{
				pcsDestAttachedData->m_pszPoint	= new CHAR[strlen(pcsSrcAttachedData->m_pszPoint) + 1];
				strcpy(pcsDestAttachedData->m_pszPoint, pcsSrcAttachedData->m_pszPoint);
			}
		}

		{
			if (pcsDestAttachedData->m_pcsBlendingData)
				m_csAnimation2.RemoveAnimData(&pcsDestAttachedData->m_pcsBlendingData);

			if (pcsSrcAttachedData->m_pcsBlendingData)
			{
				if (pcsDestAttachedData->m_pcsBlendingData)
					return FALSE;

				pcsDestAttachedData->m_pcsBlendingData	=
					m_csAnimation2.AddAnimData(pcsSrcAttachedData->m_pcsBlendingData->m_pszRtAnimName, FALSE);
				if (!pcsDestAttachedData->m_pcsBlendingData)
					return FALSE;
			}
		}

		{
			if (pcsDestAttachedData->m_pcsSubData)
				m_csAnimation2.RemoveAnimData(&pcsDestAttachedData->m_pcsSubData);

			if (pcsSrcAttachedData->m_pcsSubData)
			{
				if (pcsDestAttachedData->m_pcsSubData)
					return FALSE;

				pcsDestAttachedData->m_pcsSubData	=
					m_csAnimation2.AddAnimData(pcsSrcAttachedData->m_pcsSubData->m_pszRtAnimName, FALSE);
				if (!pcsDestAttachedData->m_pcsSubData)
					return FALSE;
			}
		}

		pcsSrcData	= pcsSrcData->m_pcsNext;
		pcsDestData	= pcsDestData->m_pcsNext;
	}

	return TRUE;
}


PVOID AgcmCharacter::EnumCallbackStruckChar(PVOID pvClass, PVOID pvData1, PVOID pvData2)
{
	AgcmCharacter	*pcsThis	= (AgcmCharacter *)(pvClass);

	return (PVOID)(pcsThis->EnumCallback(AGCMCHAR_CB_ID_STRUCK_CHARACTER, pvData1, pvData2));
}

//@{ Jaewon
void AgcmCharacter::UpdateLookAt(UINT32 dt)
{		
	AgpdCharacter* ppdCharacter = GetSelfCharacter();
	if( !ppdCharacter ) return;

	// 서머너는 제외!
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &ppdCharacter->m_csFactor );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return;

	AgcdCharacter* pcdCharacter = GetCharacterData( ppdCharacter );
	if( !pcdCharacter || !pcdCharacter->m_pClump || !pcdCharacter->m_pClump->atomicList ) return;

	// Get the hierarchy of the player character.
	RpHAnimHierarchy* pHierarchy = RpSkinAtomicGetHAnimHierarchy( pcdCharacter->m_pClump->atomicList );
	
	//. 2005. 1. 9. Nonstopdj
	//. ModelTool에서 default로 잡힌 애니메이션이 동작 중일때 그 애니메이션파일(rws)를 remove하면
	//. crash되므로 RpHAnimHierarchy가 없을 시에는 그냥 return.
	if(pHierarchy == NULL)
		return;	
	
	//@{ 2006/06/21 burumal
	// Get Head node index
	INT32 headNodeId = 1;
	RwInt32 headNodeIndex = RpHAnimIDGetIndex(pHierarchy, headNodeId);
	if ( headNodeIndex == -1 )
		return;

	RwFrame* pHeadFrame = pHierarchy->pNodeInfo[headNodeIndex].pFrame;
	RwMatrix* pHeadMatrix = RwFrameGetMatrix(pHeadFrame);
	
	// The first-time initialization
	if ( m_bLookAtFirstUpdateFlag )
	{
		RwMatrixCopy(&m_headMatrix, pHeadMatrix);
		SetPlayerLookAtTarget(pHeadMatrix->at);
		m_lookAtCurrent = m_lookAtTarget;
		m_bLookAtFirstUpdateFlag = false;
		return;
	}
	
	if ( m_lookAtFactor > 0.0f )
	{
		RtQuat rotKeyframe, rotLookAt, rotSlerped;

		if ( m_lookAtFactor < 1.0f )
		// We have to slerp the animation result with the look-at result later.
			RtQuatConvertFromMatrix(&rotKeyframe, pHeadMatrix);

		// Reset the modelling matrix to the previous result.
		// (It may be modified by an animation.)
		RwMatrixCopy(pHeadMatrix, &m_headMatrix);

		// Feedback control
		RwV3d lookAtCurrent = m_lookAtCurrent;

		RwV3d delta;
		RwV3dSub(&delta, &m_lookAtTarget, &m_lookAtCurrent);

		if(RwV3dDotProduct(&delta, &delta) > 0.001f)
		{
			RwV3dIncrementScaled(&lookAtCurrent, &delta, min(FLOAT(dt)/500.0f, 1.0f));

			// Transform into the local space.
			RwMatrix LTMInverse;
			RwMatrixInvert(&LTMInverse, RwFrameGetLTM(RwFrameGetParent(pHeadFrame)));
			RwV3dTransformVector(&lookAtCurrent, &lookAtCurrent, &LTMInverse);
			RwV3dNormalize(&lookAtCurrent, &lookAtCurrent);

			// Compute the x-axis & y-axis rotation angles(no z-axis rotation).
			
			// calc y-rotation value
			RwReal rotY = 0;
			RwV3d onYZ = lookAtCurrent;
			onYZ.x = 0;
			rotY = lookAtCurrent.x>0 ? acos(min(RwV3dLength(&onYZ), 1.0f)) : -acos(min(RwV3dLength(&onYZ), 1.0f));
			rotY *= 180.0f/3.141592f;
			
			// calc x-rotation value
			RwReal rotX = 0;
			if(RwV3dNormalize(&onYZ, &onYZ) > 0.001f)
			{
				onYZ.z = min(max(onYZ.z, -1.0f), 1.0f);
				rotX = lookAtCurrent.y>0 ? -acos(onYZ.z) : acos(onYZ.z);
				rotX *= 180.0f/3.141592f;
			}
			
			rotX = min(max(rotX, -60.0f), 60.0f);
			rotY = min(max(rotY, -30.0f), 30.0f);			

		#ifdef _DEBUG
			char pDbgMsg[256];
			sprintf(pDbgMsg, "xRot:%f  yRot:%f", rotX, rotY);
			TRACE(pDbgMsg);
		#endif

			// Set the frame matrix.
			RwV3d axisX; axisX.x = 1.0f; axisX.y = axisX.z = 0.0f;
			RwV3d axisY; axisY.y = 1.0f; axisY.x = axisY.z = 0.0f;
			RwV3d pos = pHeadMatrix->pos;
			pHeadMatrix->pos.x = pHeadMatrix->pos.y = pHeadMatrix->pos.z = 0.0f;			
			
			RwMatrixRotate(pHeadMatrix, &axisY, rotY, rwCOMBINEREPLACE);
			RwMatrixRotate(pHeadMatrix, &axisX, rotX, rwCOMBINEPOSTCONCAT);
			
			pHeadMatrix->pos = pos;

			//// Compute the x-axis & y-axis rotation angles(no z-axis rotation).
			//RwReal rotX = 0;
			//RwV3d onXZ = lookAtCurrent;
			//onXZ.y = 0;
			//rotX = lookAtCurrent.y>0?-acos(min(RwV3dLength(&onXZ), 1.0f)):acos(min(RwV3dLength(&onXZ), 1.0f));
			//rotX *= 180.0f/3.141592;
			//RwReal rotY = 0;	
			//if(RwV3dNormalize(&onXZ, &onXZ) > 0.001f)
			//{
			//	onXZ.z = min(max(onXZ.z, -1.0f), 1.0f);
			//	rotY = lookAtCurrent.x>0?acos(onXZ.z):-acos(onXZ.z);
			//	rotY *= 180.0f/3.141592;
			//}

			//// Apply constraints.
			//rotX = min(max(rotX, -60.0f), 60.0f);
			//rotY = min(max(rotY, -30.0f), 20.0f);

			//// Set the frame matrix.
			//RwV3d axisX; axisX.x = 1.0f; axisX.y = axisX.z = 0.0f;
			//RwV3d axisY; axisY.y = 1.0f; axisY.x = axisY.z = 0.0f;
			//RwV3d pos = pHeadMatrix->pos;
			//pHeadMatrix->pos.x = pHeadMatrix->pos.y = pHeadMatrix->pos.z = 0.0f;
			//RwMatrixRotate(pHeadMatrix, &axisX, rotX, rwCOMBINEREPLACE);
			//RwMatrixRotate(pHeadMatrix, &axisY, rotY, rwCOMBINEPOSTCONCAT);
			//pHeadMatrix->pos = pos;
		}

		if ( m_lookAtFactor < 1.0f )
		{
			// Now slerp the animation result with the look-at result.
			RtQuatConvertFromMatrix(&rotLookAt, pHeadMatrix);
			RtQuatScale(&rotKeyframe, &rotKeyframe, 1.0f - m_lookAtFactor);
			RtQuatScale(&rotLookAt, &rotLookAt, m_lookAtFactor);
			RtQuatAdd(&rotSlerped, &rotKeyframe, &rotLookAt);
			// Convert the quaternion back to the matrix.
			RwV3d pos = pHeadMatrix->pos;
			RtQuatConvertToMatrix(&rotSlerped, pHeadMatrix);
			pHeadMatrix->pos = pos;
		}

		RwFrameUpdateObjects(pHeadFrame);
		RwMatrixMultiply(&(pHierarchy->pMatrixArray[headNodeIndex]), &m_headMatrix, &(pHierarchy->pMatrixArray[headNodeIndex-1]));
		
		// The following code is based on RpHAnimHierarchyUpdateMatrices() function code in 'rphanim.c' file.
		RwMatrix *pParentMatrix = &(pHierarchy->pMatrixArray[headNodeIndex]);
		RwMatrix *parentStack[HANIMMATRIXSTACKHEIGHT];
		
		//@{ 2006/06/21 burumal
		parentStack[0] = NULL;
		//@}

		RwMatrix **stackTop = parentStack + 1; // Allow 1 place for last pop

		// If the head node has a hierarchy, update them also.
		for ( RwInt32 nodeIndex = headNodeIndex + 1;
			(nodeIndex < pHierarchy->numNodes) && (RwFrameGetParent(pHierarchy->pNodeInfo[nodeIndex].pFrame) != RwFrameGetParent(pHeadFrame));
			++nodeIndex )
		{
			//@{ 2006/06/21 burumal
			/*
			RwMatrixMultiply(&(pHierarchy->pMatrixArray[nodeIndex]), 
				RwFrameGetMatrix(pHierarchy->pNodeInfo[nodeIndex].pFrame),
				pParentMatrix);
			*/
			if ( pParentMatrix != NULL )
			{
				RwMatrixMultiply(&(pHierarchy->pMatrixArray[nodeIndex]), 
					RwFrameGetMatrix(pHierarchy->pNodeInfo[nodeIndex].pFrame),
					pParentMatrix);
			}
			//@}

			switch ( pHierarchy->pNodeInfo[nodeIndex].flags & (rpHANIMPOPPARENTMATRIX | rpHANIMPUSHPARENTMATRIX) )
			{
			case 0:
				// We have children but no siblings, so no need to save our parent.
				pParentMatrix = &(pHierarchy->pMatrixArray[nodeIndex]);
				break;

			case (rpHANIMPOPPARENTMATRIX):
				// We are childless, next node is back up in the tree, so recover parent.
				//@{ 2006/06/21 burumal
				/*
				pParentMatrix = *(--stackTop);
				*/
				if ( stackTop > parentStack )
					pParentMatrix = *(--stackTop);
				else
					pParentMatrix = NULL;
				//@}
				break;

			case (rpHANIMPUSHPARENTMATRIX):
				// We have children but other siblings need our parent, so save it.
				ASSERT(stackTop < &parentStack[HANIMMATRIXSTACKHEIGHT]);
				*(stackTop++) = pParentMatrix;
				pParentMatrix = &(pHierarchy->pMatrixArray[nodeIndex]);
				break;

			case (rpHANIMPOPPARENTMATRIX|rpHANIMPUSHPARENTMATRIX):
				// We are childless, next is sibling, same parent, so do nothing.
				break;
			}
		}
 	}
	
	// Set the current look-at.
	RwV3dTransformVector(&m_lookAtCurrent, &pHeadMatrix->at, RwFrameGetLTM(RwFrameGetParent(pHeadFrame)));

	// Keep the matrix.
	RwMatrixCopy(&m_headMatrix, pHeadMatrix);
}
//@} Jaewon

void	AgcmCharacter::UpdateBendOver(UINT32 uDeltaTime, AgpdCharacter* pAgpdCharacter, AgpdCharacter* pAgpdTargetCharacter)
{	
	if ( pAgpdCharacter == NULL )
		return;

	FLOAT fDegree = 0.0f;

	AgcdCharacter* pAgcdCharacter = GetCharacterData(pAgpdCharacter);
	AgcdCharacter* pAgcdTargetCharacter = GetCharacterData(pAgpdTargetCharacter);

	BOOL bIsEnabled = FALSE;	
	
	if ( !m_bBendingDisabled )
	{
		if ( pAgpdTargetCharacter
			&& (pAgpdTargetCharacter != pAgpdCharacter)
			&& !pAgpdCharacter->m_bIsTrasform
			&& !pAgcdCharacter->m_bTransforming
			&& pAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_DEAD 
			&& pAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_RUN
			&& !pAgpdTargetCharacter->IsDead()
			&& !(pAgcdTargetCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_DEAD)
			&& pAgcdCharacter->m_pstAgcdCharacterTemplate->m_bUseBending )
		{
			bIsEnabled = TRUE;

			if ( pAgcdTargetCharacter->m_pClump == NULL )
				return;

			if ( pAgcdTargetCharacter->m_pClump->atomicList == NULL )
				return;
		}
	}
	
	if ( !bIsEnabled )
	{
		#define fBENDING_DECREASE_DEGREE_FACTOR	1.2f

		if ( pAgcdCharacter->m_fLastBendingDegree != 0.0f )
		{
			if ( pAgcdCharacter->m_fLastBendingDegree > 0.0f )
				pAgcdCharacter->m_fLastBendingDegree -= fBENDING_DECREASE_DEGREE_FACTOR;
			else
			if ( pAgcdCharacter->m_fLastBendingDegree < 0.0f )
				pAgcdCharacter->m_fLastBendingDegree += fBENDING_DECREASE_DEGREE_FACTOR;

			if ( fabs(pAgcdCharacter->m_fLastBendingDegree) <= fBENDING_DECREASE_DEGREE_FACTOR )
				pAgcdCharacter->m_fLastBendingDegree = 0.0f;
		}
		else
		{
			return;
		}

		fDegree = pAgcdCharacter->m_fLastBendingDegree;
	}
	
	if ( pAgcdCharacter->m_pClump->atomicList == NULL )
		return;

	// Get the hierarchy of the player character.
	RpHAnimHierarchy* pHierarchy = RpSkinAtomicGetHAnimHierarchy(pAgcdCharacter->m_pClump->atomicList);
	if ( pHierarchy == NULL )
		return;	
		
	INT32 nBendNodeID = GetTemplateData(pAgpdCharacter->m_pcsCharacterTemplate)->m_lLookAtNode;

	RwInt32 nBendNodeIndex = RpHAnimIDGetIndex(pHierarchy, nBendNodeID);
	if ( nBendNodeIndex == -1 )
		return;

	RwFrame* pBendFrame = pHierarchy->pNodeInfo[nBendNodeIndex].pFrame;
	RpClump* pClump = pAgcdCharacter->m_pClump;

	if ( bIsEnabled )
	{	
		RpClump* pTargetClump = pAgcdTargetCharacter->m_pClump;
		if ( pClump == NULL || pTargetClump == NULL )
			return;
			
		RwV3d vPos;
		vPos = RwFrameGetLTM(RpClumpGetFrame(pClump))->pos;
		vPos.y += pClump->stType.boundingSphere.radius;

		RwV3d vTargetPos;
		vTargetPos = RwFrameGetLTM(RpClumpGetFrame(pTargetClump))->pos;
		vTargetPos.y += pTargetClump->stType.boundingSphere.radius;	
			
		FLOAT fDistance = AUPOS_DISTANCE_XZ(vPos, vTargetPos);

		if ( fDistance < 700.0f )
		{
			fDegree = atan((vTargetPos.y - vPos.y) / fDistance);
			fDegree = (fDegree * 180.0f) / 3.141592f;
			pAgcdCharacter->m_fLastBendingDegree = fDegree;
		}
		else
		{
			fDegree = pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree;
		}
	}	
	
	#define	fMAX_BENDING_OFFSET_UP	25.0f
	#define	fMAX_BENDING_OFFSET_DN	60.0f

#ifdef USE_MFC
	fDegree = pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree;

	if ( fDegree < 0.0f )
	{
		if ( fDegree < -fMAX_BENDING_OFFSET_UP )
			pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree = -fMAX_BENDING_OFFSET_UP;
	}
	else
	if ( fDegree > 0.0f )
	{
		if ( fDegree > fMAX_BENDING_OFFSET_DN )
			pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree = fMAX_BENDING_OFFSET_DN;
	}
#endif

	fDegree *= -pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingFactor;

	if ( fDegree < 0.0f )
	{
		if ( fDegree < -fMAX_BENDING_OFFSET_UP )
			fDegree = -fMAX_BENDING_OFFSET_UP;
	}
	else
	if ( fDegree > 0.0f )
	{
		if ( fDegree > fMAX_BENDING_OFFSET_DN )
			fDegree = fMAX_BENDING_OFFSET_DN;
	}

	if ( fDegree != 0 )
	{
		RwMatrix* pBendMatrix = RwFrameGetLTM(pBendFrame);

		RwFrame* pClumpFrame = RpClumpGetFrame(pClump);
		if ( pClumpFrame == NULL )
			return;		
		
		RwMatrix matRotX;		
		RwMatrixRotate(&matRotX, &RwFrameGetLTM(pClumpFrame)->right, fDegree, rwCOMBINEREPLACE);
		
		RwMatrix matTemp;
		RwV3d vPosBackup;

		RwMatrix* pModelMatrix = RwFrameGetMatrix(pBendFrame);
		
		vPosBackup = pModelMatrix->pos;
		pModelMatrix->pos.x = pModelMatrix->pos.y = pModelMatrix->pos.z = 0;

		RwMatrix* pLTM = RwFrameGetLTM(RwFrameGetParent(pBendFrame));
		
		RwMatrix matLTMInv;
		RwMatrixInvert(&matLTMInv, pLTM);

		RwMatrix matX;
		RwMatrixMultiply(&matTemp, pLTM, &matRotX);
		RwMatrixMultiply(&matX, &matTemp, &matLTMInv);

		RwMatrixCopy(&matTemp, pModelMatrix);
		RwMatrixMultiply(pModelMatrix, &matTemp, &matX);

		pModelMatrix->pos = vPosBackup;
		
		RwFrameUpdateObjects(pBendFrame);
		RwMatrixMultiply(&(pHierarchy->pMatrixArray[nBendNodeIndex]), pModelMatrix, &(pHierarchy->pMatrixArray[nBendNodeIndex-1]));

		RwMatrix* pParentMatrix = &(pHierarchy->pMatrixArray[nBendNodeIndex]);
		RwMatrix* parentStack[HANIMMATRIXSTACKHEIGHT];
		
		parentStack[0] = NULL;

		RwMatrix **stackTop = parentStack + 1; // Allow 1 place for last pop

		// If the head node has a hierarchy, update them also.
		for ( RwInt32 nodeIndex = nBendNodeIndex + 1;
			(nodeIndex < pHierarchy->numNodes) && (RwFrameGetParent(pHierarchy->pNodeInfo[nodeIndex].pFrame) != RwFrameGetParent(pBendFrame));
			++nodeIndex )
		{			
			if ( pParentMatrix != NULL )
			{
				RwMatrixMultiply(&(pHierarchy->pMatrixArray[nodeIndex]), 
					RwFrameGetMatrix(pHierarchy->pNodeInfo[nodeIndex].pFrame),
					pParentMatrix);
			}

			switch ( pHierarchy->pNodeInfo[nodeIndex].flags & (rpHANIMPOPPARENTMATRIX | rpHANIMPUSHPARENTMATRIX) )
			{
			case 0:
				// We have children but no siblings, so no need to save our parent.
				pParentMatrix = &(pHierarchy->pMatrixArray[nodeIndex]);
				break;

			case (rpHANIMPOPPARENTMATRIX):
				// We are childless, next node is back up in the tree, so recover parent.				
				if ( stackTop > parentStack )
					pParentMatrix = *(--stackTop);
				else
					pParentMatrix = NULL;
				break;

			case (rpHANIMPUSHPARENTMATRIX):
				// We have children but other siblings need our parent, so save it.
				ASSERT(stackTop < &parentStack[HANIMMATRIXSTACKHEIGHT]);
				*(stackTop++) = pParentMatrix;
				pParentMatrix = &(pHierarchy->pMatrixArray[nodeIndex]);
				break;

			case (rpHANIMPOPPARENTMATRIX|rpHANIMPUSHPARENTMATRIX):
				// We are childless, next is sibling, same parent, so do nothing.
				break;
			}
		}
	}	
}