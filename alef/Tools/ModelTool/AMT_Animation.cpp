#include "stdafx.h"
#include "ModelTool.h"

BOOL CModelToolApp::StartAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	// 해야할 것덜...
	// 1. 강제로 AnimEquip 설정, 원상복귀
	// 2. 강제로 AnimIndex 확률 100% 설정, 원상복귀
/*
	CModelToolApp						*pThis	= (CModelToolApp *)(pClass);
	AgcmAnimationDlgCharStartAnimSet	*pstSet	= (AgcmAnimationDlgCharStartAnimSet *)(pData);

	AgpmCharacter	*pcsAgpmCharacter	= pThis->m_csAgcEngine.GetAgpmCharacterModule();
	AgcmCharacter	*pcsAgcmCharacter	= pThis->m_csAgcEngine.GetAgcmCharacterModule();

	AgpdCharacter	*pcsAgpdCharacter	= pcsAgpmCharacter->GetCharacter(pThis->GetCurrentID());
	if (!pcsAgpdCharacter)
		return FALSE;

	AgcdCharacter	*pcsAgcdCharacter	= pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);
	if (!pcsAgcdCharacter)
		return FALSE;

	if ((!pstSet->m_pcsAgcdAnimation) && (pstSet->m_lAnimType > -1))
	{
//		if (!pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_paSubAnim[AGCMCHAR_ANIM_TYPE_STRUCK][0][0])
//			return FALSE;
//
//		RtAnimInterpolatorSetCurrentAnim(pcsAgcdCharacter->m_astSubAnimInfo[0].m_pInHierarchy->currentAnim, pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_paSubAnim[AGCMCHAR_ANIM_TYPE_STRUCK][0][0]->m_ppastAnimData[0]->m_pstAnim);
//		RpHAnimHierarchySetCurrentAnimTime(pcsAgcdCharacter->m_astSubAnimInfo[0].m_pInHierarchy, pcsAgcdCharacter->m_astSubAnimInfo[0].m_pInHierarchy->currentAnim->pCurrentAnim->duration);
//		RpHAnimUpdateHierarchyMatrices(pcsAgcdCharacter->m_astSubAnimInfo[0].m_pInHierarchy);
//
//		pcsAgcdCharacter->m_bStop = TRUE;

		pThis->m_pstStartAnimation = NULL;

		if (	(!pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_paAnim[pstSet->m_lAnimType][AGCDITEM_EQUIP_ANIMATION_TYPE_HANDATTACK]) ||
				(!pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_paAnim[pstSet->m_lAnimType][AGCDITEM_EQUIP_ANIMATION_TYPE_HANDATTACK]->m_ppastAnimData[0])	)
		{
			CModelToolDlg::GetInstance()->MessageBox("Animation이 설정되어 있지 않습니다!", "WARNNING", MB_OK);
			return FALSE;
		}

		if (pstSet->m_lAnimType == AGCMCHAR_ANIM_TYPE_DEAD)
		{
			pcsAgpdCharacter->m_unActionStatus = AGPDCHAR_STATUS_DEAD;
		}
		else
		{
			pcsAgpdCharacter->m_unActionStatus = AGPDCHAR_STATUS_NORMAL;

			if (pstSet->m_lAnimType == AGCMCHAR_ANIM_TYPE_ATTACK)
				pcsAgcmCharacter->SetAttackingStatus(pcsAgcdCharacter);
		}

		pcsAgcmCharacter->StartAnimation(pcsAgpdCharacter, pcsAgcdCharacter, (AgcmCharacterAnimType)(pstSet->m_lAnimType));

		return TRUE;
	}

//	pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_paAnim[pstSet->m_nAnimType][]

	INT32 lIndex;
	AgcdCharacterAnimationAttachedData *pstCharAnimAttachedData = (AgcdCharacterAnimationAttachedData *)(
		pThis->GetEngine()->GetAgcmCharacterModule()->GetAnimationAttachedData(
		pstSet->m_pcsAgcdAnimation,
		AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY								)							);

	if (pstSet->m_lSubAnimType > -1)
	{
//		pThis->m_pstStartAnimation = NULL;
//
//		INT32 alTemp[AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA], lEquipType;
//
//		for (lIndex = 0; lIndex < AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA; ++lIndex)
//		{
//			if (pstSet->m_pcsAgcdAnimation->m_ppastAnimData[lIndex])
//			{
////				alTemp[lIndex] = pstSet->m_pcsAgcdAnimation->m_alActiveRate[lIndex];
////				pstSet->m_pcsAgcdAnimation->m_alActiveRate[lIndex] = 0;
//				alTemp[lIndex] = pstCharAnimAttachedData->m_alActiveRate[lIndex];
//				pstCharAnimAttachedData->m_alActiveRate[lIndex] = 0;
//
//			}
//			else
//				break;
//		}
//		lEquipType															= pcsAgcdCharacter->m_lAnimHandEquipType;
//
////		pstSet->m_pcsAgcdAnimation->m_alActiveRate[pstSet->m_lAnimIndex]	= 100;
//		pstCharAnimAttachedData->m_alActiveRate[pstSet->m_lAnimIndex]		= 100;
//		pcsAgcdCharacter->m_lAnimHandEquipType								= pstSet->m_lAnimEquipType;
//
//		pcsAgcmCharacter->StartSubAnimation(pcsAgcdCharacter,
//											(AgcmCharacterAnimType)(pstSet->m_lAnimType),
//											(AgcmCharacterSubAnimType)(pstSet->m_lSubAnimType),
//											pstSet->m_pcsAgcdAnimation								);
//
//		for (lIndex = 0; lIndex < AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA; ++lIndex)
//		{
//			if(pstSet->m_pcsAgcdAnimation->m_ppastAnimData[lIndex])
//			{
////				pstSet->m_pcsAgcdAnimation->m_alActiveRate[lIndex] = alTemp[lIndex];
//				pstCharAnimAttachedData->m_alActiveRate[lIndex] = alTemp[lIndex];
//			}
//			else
//				break;
//		}
//
//		pcsAgcdCharacter->m_lAnimHandEquipType								= lEquipType;
	}
	else
	{
		for (lIndex = 0; lIndex < AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA; ++lIndex)
		{
			if (pstSet->m_pcsAgcdAnimation->m_ppastAnimData[lIndex])
			{
//				pThis->m_alTempAnimActiveRate[lIndex] = pstSet->m_pcsAgcdAnimation->m_alActiveRate[lIndex];
//				pstSet->m_pcsAgcdAnimation->m_alActiveRate[lIndex] = 0;
				pThis->m_alTempAnimActiveRate[lIndex] = pstCharAnimAttachedData->m_alActiveRate[lIndex];
				pstCharAnimAttachedData->m_alActiveRate[lIndex] = 0;
			}
			else
				break;
		}

		pThis->m_lTempAnimHandEquipType										=	pcsAgcdCharacter->m_lAnimHandEquipType;		
		pThis->m_pstStartAnimation											=	pstSet->m_pcsAgcdAnimation;
//		memcpy(&pThis->m_stTempAnimFlag, &pThis->m_pstStartAnimation->m_stAnimFlag, sizeof(stAgcdAnimationFlag));

		pcsAgcdCharacter->m_lAnimHandEquipType								=	pstSet->m_lAnimEquipType;
//		pstSet->m_pcsAgcdAnimation->m_alActiveRate[pstSet->m_lAnimIndex]	=	100;
		pstCharAnimAttachedData->m_alActiveRate[pstSet->m_lAnimIndex]		=	100;
//		pstSet->m_pcsAgcdAnimation->m_stAnimFlag.m_ulAnimFlag				&=	~AGCD_ANIMATION_FLAG_LINK;

		if (pstSet->m_lAnimType == AGCMCHAR_ANIM_TYPE_DEAD)
		{
			pcsAgpdCharacter->m_unActionStatus = AGPDCHAR_STATUS_DEAD;
		}
		else
		{
			pcsAgpdCharacter->m_unActionStatus = AGPDCHAR_STATUS_NORMAL;
		}

		pcsAgcmCharacter->StartAnimation(	pcsAgpdCharacter, pcsAgcdCharacter,
											(AgcmCharacterAnimType)(pstSet->m_lAnimType),											
											pstSet->m_pcsAgcdAnimation						);
	}

	//if(CModelToolDlg::GetInstance())
	//{
	//	CModelToolDlg::GetInstance()->SetAnimRange(0, pThis->GetCharacterCurrentAnimDuration());
	//}
*/
	return TRUE;
}

BOOL CModelToolApp::StartSkillAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
/*	CModelToolApp					*pThis	= (CModelToolApp *)(pClass);
	AgcmSkillDlgStartAnimationSet	*pcsSet	= (AgcmSkillDlgStartAnimationSet *)(pData);

	AgpdCharacter	*pcsAgpdCharacter		= pThis->m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(pThis->GetCurrentID());
	if (!pcsAgpdCharacter)
		return FALSE;

	AgcdCharacter	*pcsAgcdCharacter		= pThis->m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
	if (!pcsAgcdCharacter)
		return FALSE;

	AgcdSkill		*pcsAgcdSkill			= pThis->m_csAgcEngine.GetAgcmSkillModule()->GetCharacterAttachData((ApBase *)(pcsAgpdCharacter));
	if (!pcsAgcdSkill)
		return FALSE;

	pThis->GetEngine()->GetAgcmEventEffectModule()->RemoveAllEffect((ApBase *)(pcsAgpdCharacter));

	INT32 lOffset = pThis->m_lStartTargetID;
	for (INT32 lCount = 0; lCount < pThis->m_lNumTargets; ++lCount, --lOffset)
	{
		AgpdCharacter				*pcsAgpdTarget		= pThis->GetEngine()->GetAgpmCharacterModule()->GetCharacter(lOffset);
		if (!pcsAgpdTarget)
			return FALSE;

		AgcdEventEffectCharacter	*pcsAttachedData	= (AgcdEventEffectCharacter *)(pThis->GetEngine()->GetAgcmEventEffectModule()->GetEventEffectData((ApBase *)(pcsAgpdTarget)));
		
		pThis->GetEngine()->GetAgcmEventEffectModule()->RemoveAllEffect((ApBase *)(pcsAgpdTarget));

		// 지속형 스킬.
		{
			if (pThis->m_csCharacterOffset.m_bCharacterTarget)
			{
				if (lCount > 0)
					continue;

				pcsAgpdTarget							= pcsAgpdCharacter;
			}

			AgcdSkillAttachTemplateData	*pstSkillData		= pThis->GetEngine()->GetAgcmSkillModule()->GetAttachTemplateData((ApBase *)(pcsAgpdCharacter->m_pcsCharacterTemplate));
			if (!pstSkillData)
				return FALSE;

			AgcmEventEffect				*pcsAgcmEventEffect	= pThis->GetEngine()->GetAgcmEventEffectModule();
			if (!pcsAgcmEventEffect)
				return FALSE;

			AgcmStartEventEffectParams	csParams;
			if (pcsAgcmEventEffect->SetPassiveSkill(
				pcsAgpdTarget,
				&(pstSkillData->m_astSkillVisualInfo[pcsSet->m_lSkillUsableIndex].m_astEffect[AGCDITEM_EQUIP_ANIMATION_TYPE_HANDATTACK]),
				&csParams,
				E_AGCD_EVENT_EFFECT_CDI_EXT_TYPE_CONTINUATIVE_SKILL_EFFECT,
				0																															)	)
			{
				AgcdEventEffectCharacter	*pcsAttachedData	= (AgcdEventEffectCharacter *)(pcsAgcmEventEffect->GetEventEffectData((ApBase *)(pcsAgpdTarget)));
				if (!pcsAttachedData)
					return FALSE;

//				for (INT32 lCount = 0; lCount < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++lCount)
				AES_EFFCTRL_SET_LIST		*pcsCurrent	= csParams.m_pcsNodeInfo;
				while (pcsCurrent)
				{
//					if (!csParams.m_pastNodeInfo[lCount])
//						break;

					//if (!pcsAttachedData->m_csContinuativeSkillList.AddNode(pcsSet->m_lSkillID, csParams.m_pastNodeInfo[lCount]))
						pcsAttachedData->m_csContinuativeSkillList.AddNode(pcsSet->m_lSkillID, pcsCurrent->m_pstNode)

					pcsCurrent	= pcsCurrent->m_pcsNext;
				}
			}
	//		if (!pThis->GetEngine()->GetAgcmEventEffectModule()->SetIndependentSkillEventEffect(pcsAgpdTarget, &(pstSkillData->m_astSkillVisualInfo[pcsSet->m_lSkillUsableIndex].m_astEffect[AGCDITEM_EQUIP_ANIMATION_TYPE_HANDATTACK]), pcsSet->m_lSkillID, E_AGCD_EVENT_EFFECT_CDI_EXT_TYPE_CONTINUATIVE_SKILL_EFFECT, 0))
	//			return FALSE;
		}
	}

//	pcsAgcdCharacter->m_lSkillUsableIndex	= pcsSet->m_lSkillUsableIndex;
	pcsAgcdSkill->m_lSkillUsableIndex		= pcsSet->m_lSkillUsableIndex;

	pThis->m_pstStartAnimation				= pcsSet->m_pstAgcdAnimation;

	pThis->m_lTempAnimHandEquipType			= pcsSet->m_lEquipType;
//	pThis->m_lTempAnimHandEquipType			= pcsAgcdCharacter->m_lAnimHandEquipType;
//	pcsAgcdCharacter->m_lAnimHandEquipType	= pcsSet->m_lEquipType;

	pThis->m_csAgcEngine.GetAgcmCharacterModule()->StartAnimation(pcsAgpdCharacter, AGCMCHAR_ANIM_TYPE_SKILL, pcsSet->m_pstAgcdAnimation);

//	if(CModelToolDlg::GetInstance())
//	{
//		CModelToolDlg::GetInstance()->SetAnimRange(0, pThis->GetCharacterCurrentAnimDuration());
//	}
*/
	return TRUE;
}

BOOL CModelToolApp::StartSkillEffectCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	/*CModelToolApp					*pThis				= (CModelToolApp *)(pClass);
	INT32							lSkillUsableIndex	= *((INT32 *)(pData));
	INT32							lEquipType			= *((INT32 *)(pCustData));

	AgpdCharacter					*pcsAgpdCharacter	= pThis->m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(pThis->GetCurrentID());
	if (!pcsAgpdCharacter)
		return FALSE;

	AgcdCharacter					*pcsAgcdCharacter	= pThis->m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
	if (!pcsAgcdCharacter)
		return FALSE;

	AgcdSkill						*pcsAgcdSkill		= pThis->m_csAgcEngine.GetAgcmSkillModule()->GetCharacterAttachData((ApBase *)(pcsAgpdCharacter));
	if (!pcsAgcdSkill)
		return FALSE;

//	pcsAgcdCharacter->m_lSkillUsableIndex				= lSkillUsableIndex;
	pcsAgcdSkill->m_lSkillUsableIndex					= lSkillUsableIndex;

//	memset(pcsAgcdCharacter->m_alSkillTargetCID, 0, sizeof(INT32) * AGCD_CHARACTER_SKILL_MAX_TARGET);
//	pcsAgcdCharacter->m_alSkillTargetCID[0] = pcsAgpdCharacter->m_lID;

	INT32							lTempEquipType		= pcsAgcdCharacter->m_lAnimHandEquipType;
	pcsAgcdCharacter->m_lAnimHandEquipType				= lEquipType;
	if (!pThis->m_csAgcEngine.GetAgcmSkillModule()->EnumCallbackNoAnimSkill((PVOID)(pcsAgpdCharacter)))
	{
		pcsAgcdCharacter->m_lAnimHandEquipType			= lTempEquipType;
		return FALSE;
	}

	pcsAgcdCharacter->m_lAnimHandEquipType				= lTempEquipType;*/

	return TRUE;
}

BOOL CModelToolApp::GetAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	/*CModelToolApp				*pThis	= (CModelToolApp *)(pClass);
	AgcmAnimationDlgGetAnimSet	*pSet	= (AgcmAnimationDlgGetAnimSet *)(pData);

	AgpmCharacter	*pcsAgpmCharacter	= pThis->m_csAgcEngine.GetAgpmCharacterModule();
	AgcmCharacter	*pcsAgcmCharacter	= pThis->m_csAgcEngine.GetAgcmCharacterModule();

	if (pSet->m_ppcsAgcdAnimData)
	{
		*(pSet->m_ppcsAgcdAnimData)		= pcsAgcmCharacter->GetAnimData(pSet->m_szAnimName, AMT_CHARACTER_ANIMATION_DATA_PATH);
		return (*(pSet->m_ppcsAgcdAnimData)) ? (TRUE) : (FALSE);
	}

	AgpdCharacter	*pcsAgpdCharacter	= pcsAgpmCharacter->GetCharacter(pThis->GetCurrentID());
	if((!pcsAgpdCharacter) || (!pcsAgpdCharacter->m_pcsCharacterTemplate))
		return FALSE;

	AgcdCharacterTemplate *pcsAgcdCharacterTemplate = pcsAgcmCharacter->GetTemplateData(pcsAgpdCharacter->m_pcsCharacterTemplate);
	if(!pcsAgcdCharacterTemplate)
		return FALSE;

	if(!(*pSet->m_ppcsAgcdAnimation))
	{
		*pSet->m_ppcsAgcdAnimation = pcsAgcmCharacter->GetAnimation(pcsAgpdCharacter->m_pcsCharacterTemplate->m_lID,
																	pSet->m_lAnimType,
																	pSet->m_lAnimEquipType,
																	pSet->m_lSubAnimType,
																	TRUE												);
	}

	if(!(*pSet->m_ppcsAgcdAnimation))
		return FALSE;

	(*pSet->m_ppcsAgcdAnimation)->m_ppastAnimData[pSet->m_lAnimIndex] = pcsAgcmCharacter->GetAnimData(	pSet->m_szAnimName,
																										AMT_CHARACTER_ANIMATION_DATA_PATH	);
	if(!(*pSet->m_ppcsAgcdAnimation)->m_ppastAnimData[pSet->m_lAnimIndex])
		return FALSE;*/

	return TRUE;
}

BOOL CModelToolApp::GetSkillAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
/*	CModelToolApp				*pThis				= (CModelToolApp *)(pClass);
	AgcmSkillDlgGetAnimationSet	*pSet				= (AgcmSkillDlgGetAnimationSet *)(pData);
//	AgcdAnimation	**ppstAgcdAnimation = (AgcdAnimation **)(pData);
//	CHAR			*pszAnimName		= (CHAR *)(pCustData);
//
//	AgpmCharacter	*pcsAgpmCharacter	= pThis->GetAgpmCharacterModule();
//	AgcmCharacter	*pcsAgcmCharacter	= pThis->GetAgcmCharacterModule();
//	AgcmSkill		*pcsAgcmSkill		= pThis->GetAgcmSkillModule();
//
//	AgpdCharacter	*pcsAgpdCharacter	= pcsAgpmCharacter->GetCharacter(pThis->GetCurrentID());
//	if((!pcsAgpdCharacter) || (!pcsAgpdCharacter->m_pcsCharacterTemplate))
//		return FALSE;
//
//	AgcdCharacterTemplate *pcsAgcdCharacterTemplate = pcsAgcmCharacter->GetTemplateData(pcsAgpdCharacter->m_pcsCharacterTemplate);
//	if(!pcsAgcdCharacterTemplate)
//		return FALSE;
//
//	*(ppstAgcdAnimation) = pcsAgcmCharacter->GetAnimation(pszAnimName, AMT_CHARACTER_ANIMATION_DATA_PATH);

	AgcmCharacter				*pcsAgcmCharacter	= pThis->m_csAgcEngine.GetAgcmCharacterModule();
	AgcmSkill					*pcsAgcmSkill		= pThis->m_csAgcEngine.GetAgcmSkillModule();

	if(!(*pSet->m_ppcsAgcdAnimation))
	{
		*pSet->m_ppcsAgcdAnimation = pcsAgcmSkill->GetAnimation(pSet->m_lTID, pSet->m_lDataIndex, pSet->m_lEquipType, TRUE);
		if(!(*pSet->m_ppcsAgcdAnimation))
			return FALSE;
	}

	(*pSet->m_ppcsAgcdAnimation)->m_ppastAnimData[0] = pcsAgcmCharacter->GetAnimData(pSet->m_szAnimName, AMT_CHARACTER_ANIMATION_DATA_PATH);
	if(!(*pSet->m_ppcsAgcdAnimation)->m_ppastAnimData[0])
		return FALSE;*/

	return TRUE;
}

BOOL CModelToolApp::CharacterChangeAnimCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
/* 	if(!pClass)
		return FALSE;

	CModelToolApp	*pThis				= (CModelToolApp *)(pClass);

	if (pThis->m_pstStartAnimation)
	{
		AgcdCharacterAnimationAttachedData *pstCharAnimAttachedData = (AgcdCharacterAnimationAttachedData *)(
			pThis->GetEngine()->GetAgcmCharacterModule()->GetAnimationAttachedData(
			pThis->m_pstStartAnimation,
			AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY								)							);

		AgpdCharacter *pstAgpdCharacter = pThis->m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(pThis->GetCurrentID());
		if (!pstAgpdCharacter)
			return FALSE;

		AgcdCharacter *pstAgcdCharacter = pThis->m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pstAgpdCharacter);
		if (!pstAgcdCharacter)
			return FALSE;

		if (pThis->m_pstStartAnimation != pstAgcdCharacter->m_pstCurAnim)
			return TRUE; // skip

		for (INT32 lIndex = 0; lIndex < AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA; ++lIndex)
		{
			if(pThis->m_pstStartAnimation->m_ppastAnimData[lIndex])
			{
//				pThis->m_pstStartAnimation->m_alActiveRate[lIndex] = pThis->m_alTempAnimActiveRate[lIndex];
				pstCharAnimAttachedData->m_alActiveRate[lIndex] = pThis->m_alTempAnimActiveRate[lIndex];
			}
			else
				break;
		}

	//	memcpy(&pThis->m_pstStartAnimation->m_stAnimFlag, &pThis->m_stTempAnimFlag, sizeof(stAgcdAnimationFlag));	

		pstAgcdCharacter->m_lAnimHandEquipType = pThis->m_lTempAnimHandEquipType;
	}

	if (CModelToolDlg::GetInstance()->IsOpenAnimationOptionDlg())
	{
		if (CModelToolDlg::GetInstance())
		{
			CModelToolDlg::GetInstance()->SetAnimRange(0, pThis->GetCharacterCurrentAnimDuration());
		}
	}*/

	return TRUE;
}

BOOL CModelToolApp::ObjectGetAnimDataCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	/*if((!pData) || (!pClass) || (!pCustData))
		return FALSE;

	CModelToolApp	*pcsThis		= (CModelToolApp *)(pClass);
	AgcdAnimData	**ppAnimData	= (AgcdAnimData **)(pData);
	CHAR			*pszName		= (CHAR *)(pCustData);

	*(ppAnimData) = pcsThis->m_csAgcEngine.GetAgcmObjectModule()->GetAnimData(pszName, pcsThis->m_csAgcEngine.GetAgcmObjectModule()->GetAnimationPath());*/

	return TRUE;
}

BOOL CModelToolApp::ReadRtAnimCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ( !pData || !pClass )	return FALSE;

	CModelToolApp* pcsThis = (CModelToolApp*)(pClass);
	AAD_ReadRtAnimParams* pcsParams	= (AAD_ReadRtAnimParams*)(pData);

	AgcaAnimation2	*pcsAgcaAnimation2	=
		(pcsParams->m_bObject) ?
		(pcsThis->GetEngine()->GetAgcmObjectModule()->GetAgcaAnimation2()) :
		(pcsThis->GetEngine()->GetAgcmCharacterModule()->GetAgcaAnimation2());

	if (pcsAgcaAnimation2->ReadRtAnim(pcsParams->m_pcsSrcAnimData))
		pcsParams->m_bRead	= TRUE;

	return TRUE;
}

BOOL CModelToolApp::AddAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ( !pData || !pClass )	return FALSE;

	CModelToolApp* pcsThis = (CModelToolApp *)(pClass);
	AAD_AddAnimationParams* pcsParams = (AAD_AddAnimationParams *)(pData);

	AgcaAnimation2	*pcsAgcaAnimation2	=
		(pcsParams->m_bObject) ?
		(pcsThis->GetEngine()->GetAgcmObjectModule()->GetAgcaAnimation2()) :
		(pcsThis->GetEngine()->GetAgcmCharacterModule()->GetAgcaAnimation2());

	pcsParams->m_pDestData = pcsAgcaAnimation2->AddAnimation( pcsParams->m_ppSrcAnimation, pcsParams->m_pszSrcAnimation );
	return TRUE;
}

BOOL CModelToolApp::AddAnimDataCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ( !pData || !pClass )	return FALSE;

	CModelToolApp* pcsThis = (CModelToolApp *)(pClass);
	AAD_AddAnimDataParams* pcsParams = (AAD_AddAnimDataParams *)(pData);

	pcsParams->m_pDestAnimData = pcsThis->GetEngine()->GetAgcmCharacterModule()->GetAgcaAnimation2()->AddAnimData( pcsParams->m_pszSrcAnimData, FALSE );

	return TRUE;
}

BOOL CModelToolApp::RemoveAnimDataCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ( !pData || !pClass )	return FALSE;

	CModelToolApp* pcsThis = (CModelToolApp *)(pClass);

	AAD_RemoveAnimDataParams* pcsParams	= (AAD_RemoveAnimDataParams *)(pData);

	if ( pcsThis->GetEngine()->GetAgcmCharacterModule()->GetAgcaAnimation2()->RemoveAnimData( pcsParams->m_ppSrcAnimData ))
		pcsParams->m_bRemoved	= TRUE;

	pcsThis->ApplyObject();

	return TRUE;
}

BOOL CModelToolApp::RemoveAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ( !pData || !pClass )	return FALSE;

	CModelToolApp* pcsThis = (CModelToolApp *)(pClass);
	AAD_RemoveAnimationParams* pcsParams	= (AAD_RemoveAnimationParams *)(pData);

	AgcaAnimation2	*pcsAgcaAnimation2	=
		(pcsParams->m_bObject) ?
		(pcsThis->GetEngine()->GetAgcmObjectModule()->GetAgcaAnimation2()) :
		(pcsThis->GetEngine()->GetAgcmCharacterModule()->GetAgcaAnimation2());

	if (pcsAgcaAnimation2->RemoveAnimation( pcsParams->m_pSrcAnimation, pcsParams->m_szName ) )
		pcsParams->m_bRemovedAnimation	= TRUE;

	if ( pcsParams->m_bApply )
		pcsThis->ApplyObject();

	return TRUE;
}

BOOL CModelToolApp::RemoveAllAnimationCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ( !pData || !pClass )	return FALSE;

	CModelToolApp* pcsThis = (CModelToolApp *)(pClass);
	AAD_RemoveAllAnimationParams* pcsParams	= (AAD_RemoveAllAnimationParams *)(pData);

	AgcaAnimation2	*pcsAgcaAnimation2	=
		(pcsParams->m_bObject) ?
		(pcsThis->GetEngine()->GetAgcmObjectModule()->GetAgcaAnimation2()) :
		(pcsThis->GetEngine()->GetAgcmCharacterModule()->GetAgcaAnimation2());

	if (pcsAgcaAnimation2->RemoveAllAnimation( pcsParams->m_ppSrcAnimation	))
		pcsParams->m_bRemovedAnimation	= TRUE;

	if ( pcsParams->m_bApply )
		pcsThis->ApplyObject();

	return TRUE;
}

//. 2005. 8. 31 Nonstopdj
//. Animation과 Skill에서 호출되는 StartAnimCB를 구분.
//. Animation Dialog에서는 EventEffect가 필요없으므로
BOOL CModelToolApp::StartPureAnimCB( PVOID pData, PVOID pClass, PVOID pCustData )
{
if ( !pData || !pClass )	return FALSE;

	CModelToolApp*				pcsThis		= (CModelToolApp *)(pClass);
	AAD_StartAnimationParams*	pcsParams	= (AAD_StartAnimationParams *)(pData);

	AgpdCharacter	*pcsAgpdCharacter = pcsThis->GetEngine()->GetAgpmCharacterModule()->GetCharacter( pcsThis->GetCurrentID() );
	if( !pcsAgpdCharacter )
		return FALSE;

	AgcdCharacter	*pcsAgcdCharacter = pcsThis->GetEngine()->GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter )
		return FALSE;

	pcsAgcdCharacter->m_pcsNextAnimData	= pcsParams->m_pcsAnimData;

	if( pcsParams->m_lAnimType2 != -1 )
		pcsAgcdCharacter->m_lCurAnimType2 = pcsParams->m_lAnimType2;


	BOOL bLoop = FALSE;
	if( pcsParams->m_lAnimType > 0 && pcsParams->m_lAnimType2 != -1 )
	{
		AgcdCharacterAnimation* pacsAnimationData = pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[pcsParams->m_lAnimType][pcsParams->m_lAnimType2];
		if( (pacsAnimationData) && (pacsAnimationData->m_pstAnimFlags) && (pacsAnimationData->m_pstAnimFlags->m_unAnimFlag & AGCD_ANIMATION_FLAG_LOOP) )
			bLoop = TRUE;
	}

	AgcmCharacter* pAgcmCharacter = pcsThis->GetEngine()->GetAgcmCharacterModule();
	BOOL bRet = pAgcmCharacter->SetNextAnimation( pcsAgpdCharacter, pcsAgcdCharacter, (AgcmCharacterAnimType)(pcsParams->m_lAnimType), bLoop );
	if( !bRet )		return FALSE;

	if( pcsAgcdCharacter->m_pRide ) {
		AgpdCharacter* pAgpdCharacterRide = pAgcmCharacter->GetCharacter( pcsAgcdCharacter->m_pRide );
		AgcdCharacterAnimation* pacsRideAnimationData = pcsAgcdCharacter->m_pRide->m_pstAgcdCharacterTemplate->m_pacsAnimationData[pcsParams->m_lAnimType][0];
		if ( pacsRideAnimationData )
		{
			pcsAgcdCharacter->m_pRide->m_pcsNextAnimData = pacsRideAnimationData->m_pcsAnimation->m_pcsHead;
			bRet = pAgcmCharacter->SetNextAnimation( pAgpdCharacterRide, pcsAgcdCharacter->m_pRide, (AgcmCharacterAnimType)(pcsParams->m_lAnimType), bLoop );
		}
	}

	return bRet;
}


BOOL CModelToolApp::StartAnimCB( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if(	!pData || !pClass )		return FALSE;

	CModelToolApp* pcsThis = (CModelToolApp *)(pClass);
	AAD_StartAnimationParams* pcsParams	= (AAD_StartAnimationParams *)(pData);

	AgpdCharacter* pcsAgpdCharacter = pcsThis->GetEngine()->GetAgpmCharacterModule()->GetCharacter( pcsThis->GetCurrentID() );
	if( !pcsAgpdCharacter )		return FALSE;

	AgcdCharacter* pcsAgcdCharacter = pcsThis->GetEngine()->GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter )		return FALSE;

	AgcdSkillADChar* pcsAgcdSkillADChar = pcsThis->m_csAgcEngine.GetAgcmSkillModule()->GetCharacterAttachData( (ApBase *)(pcsAgpdCharacter)	);
	if( pcsAgcdSkillADChar )
		pcsAgcdSkillADChar->m_lSkillUsableIndex	= pcsParams->m_lSkillIndex;

	pcsAgcdCharacter->m_pcsNextAnimData	= pcsParams->m_pcsAnimData;

	if( pcsParams->m_lAnimType2 != -1 )
		pcsAgcdCharacter->m_lCurAnimType2 = pcsParams->m_lAnimType2;

	INT32 lOffset = pcsThis->m_lStartTargetID;

	for( INT32 lCount = 0; lCount < pcsThis->m_lNumTargets; ++lCount, --lOffset )
	{
		AgpdCharacter* pcsAgpdTarget = pcsThis->GetEngine()->GetAgpmCharacterModule()->GetCharacter( lOffset );
		if( !pcsAgpdTarget )
			return FALSE;

		AgcdEventEffectCharacter	*pcsAttachedData	= (AgcdEventEffectCharacter *)(pcsThis->GetEngine()->GetAgcmEventEffectModule()->GetEventEffectData((ApBase *)(pcsAgpdTarget)));

		pcsThis->GetEngine()->GetAgcmEventEffectModule()->RemoveAllEffect((ApBase *)(pcsAgpdTarget));

		if( pcsThis->m_csCharacterOffset.m_bCharacterTarget ) {
			if (lCount > 0)
				continue;

			pcsAgpdTarget = pcsAgpdCharacter;
		}

		AgcdSkillAttachTemplateData	*pstSkillData = pcsThis->GetEngine()->GetAgcmSkillModule()->GetAttachTemplateData((ApBase *)(pcsAgpdCharacter->m_pcsCharacterTemplate));
		if( !pstSkillData )
			return FALSE;

		// 지속형 스킬.
		if ( pstSkillData->m_pacsSkillVisualInfo && pstSkillData->m_pacsSkillVisualInfo[pcsParams->m_lSkillIndex] && pstSkillData->m_pacsSkillVisualInfo[pcsParams->m_lSkillIndex]->m_pastEffect[AGCMCHAR_AT2_COMMON_DEFAULT] )
		{
			AgcmEventEffect				*pcsAgcmEventEffect	= pcsThis->GetEngine()->GetAgcmEventEffectModule();
			if (!pcsAgcmEventEffect)
				return FALSE;

			AgcmStartEventEffectParams	csParams;
			if (pcsAgcmEventEffect->SetPassiveSkill(
				pcsAgpdTarget,
				pstSkillData->m_pacsSkillVisualInfo[pcsParams->m_lSkillIndex]->m_pastEffect[AGCMCHAR_AT2_COMMON_DEFAULT],
				&csParams,
				E_AGCD_EVENT_EFFECT_CDI_EXT_TYPE_CONTINUATIVE_SKILL_EFFECT,
				0 )	)
			{
				AgcdEventEffectCharacter	*pcsAttachedData	= (AgcdEventEffectCharacter *)(pcsAgcmEventEffect->GetEventEffectData((ApBase *)(pcsAgpdTarget)));
				if (!pcsAttachedData)
					return FALSE;

				AES_EFFCTRL_SET_LIST		*pcsCurrent	= csParams.m_pcsNodeInfo;
				while( pcsCurrent )
				{
					pcsAttachedData->m_csContinuativeSkillList.AddNode(pcsParams->m_lSkillIndex, pcsCurrent->m_pstNode);
					pcsCurrent	= pcsCurrent->m_pcsNext;
				}
			}
		}
	}

	BOOL bLoop = FALSE;
	if( pcsParams->m_lAnimType > 0 && pcsParams->m_lAnimType2 != -1 ) {
		AgcdCharacterAnimation* pacsAnimationData = pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_pacsAnimationData[pcsParams->m_lAnimType][pcsParams->m_lAnimType2];
		if( (pacsAnimationData) && (pacsAnimationData->m_pstAnimFlags) && (pacsAnimationData->m_pstAnimFlags->m_unAnimFlag & AGCD_ANIMATION_FLAG_LOOP) ) {
			bLoop		= TRUE;
		}
	}

	AgcmCharacter* pAgcmCharacter = pcsThis->GetEngine()->GetAgcmCharacterModule();
	BOOL bRet = pAgcmCharacter->SetNextAnimation( pcsAgpdCharacter, pcsAgcdCharacter, (AgcmCharacterAnimType)(pcsParams->m_lAnimType), bLoop );
	if( bRet == FALSE )
		return FALSE;

	if( pcsAgcdCharacter->m_pRide ) {
		AgpdCharacter* pAgpdCharacterRide = pAgcmCharacter->GetCharacter( pcsAgcdCharacter->m_pRide );
		AgcdCharacterAnimation* pacsRideAnimationData = pcsAgcdCharacter->m_pRide->m_pstAgcdCharacterTemplate->m_pacsAnimationData[pcsParams->m_lAnimType][0];
		pcsAgcdCharacter->m_pRide->m_pcsNextAnimData = pacsRideAnimationData->m_pcsAnimation->m_pcsHead;
		bRet = pAgcmCharacter->SetNextAnimation( pAgpdCharacterRide, pcsAgcdCharacter->m_pRide, (AgcmCharacterAnimType)(pcsParams->m_lAnimType), bLoop );
	}

	return bRet;
}