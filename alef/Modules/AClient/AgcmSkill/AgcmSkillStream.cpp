#include "AgcmSkill.h"
#include "ApModuleStream.h"
#include "ApUtil.h"

BOOL AgcmSkill::StreamReadTemplateSkill( CHAR *szFile, CHAR *pszErrorMessage, BOOL bDecryption )
{
	if( szFile == NULL )
		return FALSE;

	ApModuleStream			csStream;
	UINT16					nNumKeys;
	INT32					i;
	INT32					lTID;

	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );

	// szFile을 읽는다.
	csStream.Open(szFile, 0, bDecryption);
	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for( i = 0; i < nNumKeys; ++i )
	{
		// Section Name은 TID 이다.
		lTID = atoi( csStream.ReadSectionName(i) );

		AgpdCharacterTemplate* pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( lTID );

		if( pcsAgpdCharacterTemplate == NULL ) {
			TRACE( "AgcmSkill::StreamReadTemplateSkill 템플릿을 찾을 수 없습니다.\n" );
			continue;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if( csStream.EnumReadCallback( AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL, pcsAgpdCharacterTemplate, this ) == NULL )
		{
			if(pszErrorMessage)
				sprintf(pszErrorMessage, "ERROR:[TID : %d]%s가 잘못되었습니다!", pcsAgpdCharacterTemplate->m_lID, pcsAgpdCharacterTemplate->m_szTName);

			TRACE( "AgcmSkill::StreamReadTemplateSkill Callback returns FALSE!.\n" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmSkill::StreamWriteTemplateSkill(CHAR *szFile, BOOL bEncryption)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szTID[AGPACHARACTERT_MAX_TEMPLATE_NAME + 1];
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	if( bEncryption )	csStream.SetType(AUINIMANAGER_TYPE_KEYINDEX);

	// 등록된 모든 Character Template에 대해서...
	for( pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex); 
		pcsAgpdCharacterTemplate; 
		pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		sprintf(szTID, "%d", pcsAgpdCharacterTemplate->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL, pcsAgpdCharacterTemplate, this))
			return FALSE;
	}
	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL AgcmSkill::StreamReadTemplateSkillSound( CHAR *szFile, CHAR *pszErrorMessage, BOOL bDecryption )
{
	if( szFile == NULL )
		return FALSE;

	ApModuleStream			csStream;
	UINT16					nNumKeys;
	INT32					i;
	INT32					lTID;

	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );

	// szFile을 읽는다.
	csStream.Open(szFile, 0, bDecryption);
	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for( i = 0; i < nNumKeys; ++i )
	{
		// Section Name은 TID 이다.
		lTID = atoi( csStream.ReadSectionName(i) );

		AgpdCharacterTemplate* pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( lTID );

		if( pcsAgpdCharacterTemplate == NULL ) {
			TRACE( "AgcmSkill::StreamReadTemplateSkill 템플릿을 찾을 수 없습니다.\n" );
			continue;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if( csStream.EnumReadCallback( AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL_SOUND, pcsAgpdCharacterTemplate, this ) == NULL )
		{
			if(pszErrorMessage)
				sprintf(pszErrorMessage, "ERROR:[TID : %d]%s가 잘못되었습니다!", pcsAgpdCharacterTemplate->m_lID, pcsAgpdCharacterTemplate->m_szTName);

			TRACE( "AgcmSkill::StreamReadTemplateSkill Callback returns FALSE!.\n" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmSkill::StreamWriteTemplateSkillSound(CHAR *szFile, BOOL bEncryption)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szTID[AGPACHARACTERT_MAX_TEMPLATE_NAME + 1];
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	if( bEncryption )	csStream.SetType(AUINIMANAGER_TYPE_KEYINDEX);

	// 등록된 모든 Character Template에 대해서...
	for( pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex); 
		pcsAgpdCharacterTemplate; 
		pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		sprintf(szTID, "%d", pcsAgpdCharacterTemplate->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL_SOUND, pcsAgpdCharacterTemplate, this))
			return FALSE;
	}
	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL AgcmSkill::AttachedTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgcmSkill	*pThis				= (AgcmSkill *)	pClass;
	ApBase		*pcsBaseTemplate	= (ApBase *)	pData;

	AgcdCharacterTemplate		*pcsAgcdCharacterTemplate	=
		pThis->m_pcsAgcmCharacter->GetTemplateData(
		(AgpdCharacterTemplate *)(pcsBaseTemplate)	);
	if (!pcsAgcdCharacterTemplate)
		return FALSE;

	AgcdSkillAttachTemplateData *pcsTemplateAttachData		= pThis->GetAttachTemplateData(pcsBaseTemplate);
	if (!pcsTemplateAttachData)
		return FALSE;

	CHAR	szValue[256], szTemp[256];
	INT32	lVisualInfoIndex, lAnimType2, lEffectIndex, lTemp1, lTemp2;
	AgcdUseEffectSetData	*pcsEData;
	//AgcdCharacterAnimationAttachedData	*pstAttachedData;
	ACA_AttachedData		*pcsAttachedData	= NULL;
	eAcReturnType			eRt					= E_AC_RETURN_TYPE_NONE;
	//값을 읽어들인다.
	const CHAR	*szValueName = NULL;
	
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strncmp(szValueName, AGCMSKILL_INI_NAME_ANIMATION_NAME, strlen(AGCMSKILL_INI_NAME_ANIMATION_NAME)))
		{
			if (!pStream->GetValue(szValue, 256))
				return FALSE;

			sscanf(szValue, "%d:%d:%s", &lVisualInfoIndex, &lAnimType2, szTemp);

			if (!strcmp(szTemp, ""))
				continue;

			eRt	= pThis->CheckVisualInfo(pcsAgcdCharacterTemplate, pcsTemplateAttachData, lVisualInfoIndex);

			//. 파싱된 lVisualInfoIndex을 저장. ModelTool에서 사용
			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_iVisualIndex = lVisualInfoIndex;

			if (eRt == E_AC_RETURN_TYPE_FAILURE)
			{
				ASSERT(!"!CheckVisualInfo()");
				return FALSE;
			}
			else if (eRt == E_AC_RETURN_TYPE_SKIP)
				continue;

			lAnimType2	= pThis->m_pcsAgcmCharacter->ConvertAnimType2(pcsAgcdCharacterTemplate->m_lAnimType2, lAnimType2);

			if (lAnimType2 == -2)
			{
				continue;
			}
			else if (lAnimType2 == -1)
			{
				ASSERT(!"ERROR!");
				return FALSE;
			}
			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2])
				pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2] = new AgcdCharacterAnimation();

			pThis->m_pcsAgcmCharacter->GetAgcaAnimation2()->AddAnimation(
				&pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation,
				szTemp																											);
		}
		else if (!strncmp(szValueName, AGCMSKILL_INI_NAME_ANIMATION_FLAGS, strlen(AGCMSKILL_INI_NAME_ANIMATION_FLAGS)))
		{
			if (!pStream->GetValue(szValue, 256))
				return FALSE;

			sscanf(szValue, "%d:%d:%d:%d", &lVisualInfoIndex, &lAnimType2, &lTemp1, &lTemp2);

			eRt	= pThis->CheckVisualInfo(pcsAgcdCharacterTemplate, pcsTemplateAttachData, lVisualInfoIndex);

			//. 파싱된 lVisualInfoIndex을 저장. ModelTool에서 사용
			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_iVisualIndex = lVisualInfoIndex;

			if (eRt == E_AC_RETURN_TYPE_FAILURE)
			{
				ASSERT(!"!CheckVisualInfo()");
				return FALSE;
			}
			else if (eRt == E_AC_RETURN_TYPE_SKIP)
				continue;


			lAnimType2	= pThis->m_pcsAgcmCharacter->ConvertAnimType2(pcsAgcdCharacterTemplate->m_lAnimType2, lAnimType2);
			if (lAnimType2 == -2)
			{
				continue;
			}
			else if (lAnimType2 == -1)
			{
				ASSERT(!"ERROR!");
				return FALSE;
			}

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2])
			{
				ASSERT(!"!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]");
				return FALSE;
			}

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pstAnimFlags)
				pThis->m_pcsAgcmCharacter->GetAgcaAnimation2()->AddFlags(
				&pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pstAnimFlags	);

			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pstAnimFlags->m_unAnimFlag	= lTemp1;
			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pstAnimFlags->m_unPreference	= lTemp2;
		}
		else if (!strncmp(szValueName, AGCMSKILL_INI_NAME_ANIMATION_POINT, strlen(AGCMSKILL_INI_NAME_ANIMATION_POINT)))
		{
			if (!pStream->GetValue(szValue, 256))
				return FALSE;

			sscanf(szValue, "%d:%d:%s", &lVisualInfoIndex, &lAnimType2, szTemp);

			if (!strcmp(szTemp, ""))
				continue;

			eRt	= pThis->CheckVisualInfo(pcsAgcdCharacterTemplate, pcsTemplateAttachData, lVisualInfoIndex);

			//. 파싱된 lVisualInfoIndex을 저장. ModelTool에서 사용
			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_iVisualIndex = lVisualInfoIndex;

			if (eRt == E_AC_RETURN_TYPE_FAILURE)
			{
				ASSERT(!"!CheckVisualInfo()");
				return FALSE;
			}
			else if (eRt == E_AC_RETURN_TYPE_SKIP)
				continue;

			lAnimType2	= pThis->m_pcsAgcmCharacter->ConvertAnimType2(pcsAgcdCharacterTemplate->m_lAnimType2, lAnimType2);
			if (lAnimType2 == -2)
			{
				continue;
			}
			else if (lAnimType2 == -1)
			{
				ASSERT(!"ERROR!");
				return FALSE;
			}

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2])
			{
				ASSERT(!"!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]");
				return FALSE;
			}

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation)
			{
				ASSERT(!"!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation");
				return FALSE;
			}

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead)
			{
				ASSERT(!"!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead");
				return FALSE;
			}

			pcsAttachedData	=
				(ACA_AttachedData *)(pThis->m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData(
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
				pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pacsAnimation[lAnimType2]->m_pcsAnimation->m_pcsHead	));
			if (!pcsAttachedData)
			{
				ASSERT(!"!pcsAttachedData");
				return FALSE;
			}

			if (pcsAttachedData->m_pszPoint)
			{
				ASSERT(!"pcsAttachedData->m_pszPoint");
				return FALSE;
			}

			pcsAttachedData->m_pszPoint	= new CHAR [strlen(szTemp) + 1];
			strcpy(pcsAttachedData->m_pszPoint, szTemp);
		}
		else if (!strncmp(szValueName, AGCMSKILL_INI_NAME_EFFECT_DATA, strlen(AGCMSKILL_INI_NAME_EFFECT_DATA)))
		{
			if (!pStream->GetValue(szValue, 256))
				return FALSE;

			eRt	= pThis->SetSkillEffectData(pcsAgcdCharacterTemplate, pcsTemplateAttachData, szValue);
			if (eRt == E_AC_RETURN_TYPE_FAILURE)
				return FALSE;
			else if (eRt == E_AC_RETURN_TYPE_SKIP)
				continue;
		}
		else if (!strncmp(szValueName, AGCMSKILL_INI_NAME_SKILL_INFO, strlen(AGCMSKILL_INI_NAME_SKILL_INFO)))
		{
			if (!pStream->GetValue(szValue, 256))
				return FALSE;

			INT32	eOptionShow = ( INT32 ) AgcmSkillVisualInfo::DEFEND_ON_OPTION;

			CGetArg2	arg( szValue , ":");

			if( arg.GetArgCount() == 3 )
			{
				sscanf(szValue, "%d:%d:%s", &lVisualInfoIndex, &lAnimType2, szTemp  );
			}
			else if( arg.GetArgCount() == 4 )
			{
				lVisualInfoIndex	= atoi( arg.GetParam( 0 ) );
				lAnimType2			= atoi( arg.GetParam( 1 ) );

				strncpy( szTemp , arg.GetParam( 2 ) , 255 );

				eOptionShow			= atoi( arg.GetParam( 3 ) );
			}
			else
			{
				// Error
			}
			
			if (!strcmp(szTemp, ""))
				continue;

			eRt	= pThis->CheckVisualInfo(pcsAgcdCharacterTemplate, pcsTemplateAttachData, lVisualInfoIndex);

			//. 파싱된 lVisualInfoIndex을 저장. ModelTool에서 사용
			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_iVisualIndex	= lVisualInfoIndex;
			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_eShowOption	= ( AgcmSkillVisualInfo::OPTION ) eOptionShow;

			if (eRt == E_AC_RETURN_TYPE_FAILURE)
			{
				ASSERT(!"!CheckVisualInfo()");
				return FALSE;
			}
			else if (eRt == E_AC_RETURN_TYPE_SKIP)
				continue;


			lAnimType2	= pThis->m_pcsAgcmCharacter->ConvertAnimType2(pcsAgcdCharacterTemplate->m_lAnimType2, lAnimType2);

			if (lAnimType2 == -2)
			{
				continue;
			}
			else if (lAnimType2 == -1)
			{
				ASSERT(!"ERROR!");
				return FALSE;
			}
			if (pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszInfo[lAnimType2])
			{
				ASSERT(!"pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszInfo[lAnimType2]");
				return FALSE;
			}

			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszInfo[lAnimType2] =
				new CHAR [strlen(szTemp) + 1];
			strcpy(pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszInfo[lAnimType2], szTemp);
		}
		else if (!strncmp(szValueName, AGCMSKILL_INI_NAME_EFFECT_CUST_DATA, strlen(AGCMSKILL_INI_NAME_EFFECT_CUST_DATA)))
		{
			if (!pStream->GetValue(szValue, 256))
				return FALSE;

			sscanf(szValue, "%d:%d:%d:%s", &lVisualInfoIndex, &lAnimType2, &lEffectIndex, szTemp);

			if (!strcmp(szTemp, ""))
				continue;

			eRt	= pThis->CheckVisualInfo(pcsAgcdCharacterTemplate, pcsTemplateAttachData, lVisualInfoIndex);

			//. 파싱된 lVisualInfoIndex을 저장. ModelTool에서 사용
			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_iVisualIndex = lVisualInfoIndex;

			if (eRt == E_AC_RETURN_TYPE_FAILURE)
			{
				ASSERT(!"!CheckVisualInfo()");
				return FALSE;
			}
			else if (eRt == E_AC_RETURN_TYPE_SKIP)
				continue;


			lAnimType2	= pThis->m_pcsAgcmCharacter->ConvertAnimType2(pcsAgcdCharacterTemplate->m_lAnimType2, lAnimType2);

			if (lAnimType2 == -2)
			{
				continue;
			}
			else if (lAnimType2 == -1)
			{
				ASSERT(!"ERROR!");
				return FALSE;
			}

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2])
				pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2] =
				new AgcdUseEffectSet();

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2])
			{
				ASSERT(!"!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2]");
				return FALSE;
			}

			pcsEData = pThis->m_csEffectDataAdmin2.GetData(
				pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2],
				lEffectIndex,
				TRUE																					);

			if (!pcsEData)
			{
				ASSERT(!"!pcsEData");
				return FALSE;
			}

			if (pcsEData->m_pszCustData)
			{
				ASSERT(!"pcsEData->m_pszCustData");
				return FALSE;
			}


			pcsEData->m_pszCustData		= new CHAR[strlen(szTemp) + 1];
			//			strcpy(pcsTemplateAttachData->m_astSkillVisualInfo[lUsableIndex].m_astEffect[lEquipType].m_astData[lEffectIndex]->m_szCustData, szTemp);
			strcpy(pcsEData->m_pszCustData, szTemp);
		}
		else if (!strncmp(szValueName, AGCMSKILL_INI_NAME_EFFECT_CONDITION, strlen(AGCMSKILL_INI_NAME_EFFECT_CONDITION)))
		{
			if (!pStream->GetValue(szValue, 256))
				return FALSE;

			sscanf(szValue, "%d:%d:%d:%d", &lVisualInfoIndex, &lAnimType2, &lEffectIndex, &lTemp1);

			eRt	= pThis->CheckVisualInfo(pcsAgcdCharacterTemplate, pcsTemplateAttachData, lVisualInfoIndex);

			//. 파싱된 lVisualInfoIndex을 저장. ModelTool에서 사용
			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_iVisualIndex = lVisualInfoIndex;

			if (eRt == E_AC_RETURN_TYPE_FAILURE)
			{
				ASSERT(!"!CheckVisualInfo()");
				return FALSE;
			}
			else if (eRt == E_AC_RETURN_TYPE_SKIP)
				continue;


			lAnimType2	= pThis->m_pcsAgcmCharacter->ConvertAnimType2(pcsAgcdCharacterTemplate->m_lAnimType2, lAnimType2);

			if (lAnimType2 == -2)
			{
				continue;
			}
			else if (lAnimType2 == -1)
			{
				ASSERT(!"ERROR!");
				return FALSE;
			}

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2])
				pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2] =
				new AgcdUseEffectSet();

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2])
			{
				ASSERT(!"!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2]");
				return FALSE;
			}

			pcsEData = pThis->m_csEffectDataAdmin2.GetData(
				pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2],
				lEffectIndex,
				TRUE																					);

			if (!pcsEData)
			{
				ASSERT(!"!pcsEData");
				return FALSE;
			}

			pcsEData->m_ulConditionFlags																			|=	lTemp1;
			pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2]->m_ulConditionFlags	|=	lTemp1;
		}
	}

	return TRUE;
}


BOOL AgcmSkill::AttachedTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	ApBase		*pcsBaseTemplate	= (ApBase *)	pData;
	AgcmSkill	*pThis				= (AgcmSkill *)	pClass;

	AgpdSkillTemplateAttachData *pcsAgpdSkillData = pThis->m_pcsAgpmSkill->GetAttachSkillTemplateData(pcsBaseTemplate);
	AgcdSkillAttachTemplateData *pcsAgcdSkillData = pThis->GetAttachTemplateData(pcsBaseTemplate);
	if ((!pcsAgpdSkillData) || (!pcsAgcdSkillData))
		return FALSE;

	if (!pcsAgcdSkillData->m_pacsSkillVisualInfo)
		return TRUE; // skip!

	CHAR szValueName[512], szValue[1024];
	//ACA_AttachedData		*pcsAttachedData			= NULL;
	AgcdCharacterTemplate	*pcsAgcdCharacterTemplate	= pThis->m_pcsAgcmCharacter->GetTemplateData((AgpdCharacterTemplate *)(pcsBaseTemplate));
	if (!pcsAgcdCharacterTemplate)
		return FALSE;

	INT32					lNumVisualInfo				= pThis->GetNumVisualInfo(pcsAgcdCharacterTemplate);
	INT32					lNumAnimType2				= pThis->m_pcsAgcmCharacter->GetAnimType2Num(pcsAgcdCharacterTemplate);

	//for (INT32 lUsableIndex = 0; lUsableIndex < AGPMSKILL_MAX_SKILL_OWN; ++lUsableIndex)
	for (INT32 lVisualInfoIndex = 0; lVisualInfoIndex < lNumVisualInfo; ++lVisualInfoIndex)
	{
		//if ((!pcsAgpdSkillData->m_aszUsableSkillTName[lUsableIndex]) || (!pcsAgpdSkillData->m_aszUsableSkillTName[lUsableIndex][0]))
		//	break;
		if (!pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfoIndex])
			continue;

		for (INT32 lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2)
		{
			if (pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszInfo[lAnimType2])
			{
				sprintf(szValueName,	"%s%d%d",				AGCMSKILL_INI_NAME_SKILL_INFO, lVisualInfoIndex, lAnimType2);

				if( pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_eShowOption != AgcmSkillVisualInfo::DEFEND_ON_OPTION )
				{
					sprintf(szValue,		"%d:%d:%s:%d",			lVisualInfoIndex,
						lAnimType2,
						//pcsAgcdSkillData->m_astSkillVisualInfo[lUsableIndex].m_paszInfoName[lEquipType]	);
						pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszInfo[lAnimType2]	,
						( INT32 ) pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_eShowOption	);
				}
				else
				{
					sprintf(szValue,		"%d:%d:%s",			lVisualInfoIndex,
						lAnimType2,
						//pcsAgcdSkillData->m_astSkillVisualInfo[lUsableIndex].m_paszInfoName[lEquipType]	);
						pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_paszInfo[lAnimType2]	);
				}

				pStream->WriteValue(szValueName, szValue);
			}

			pThis->AttachedTemplateAnimationWrite(
				pStream,
				pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfoIndex],
				lVisualInfoIndex,
				lAnimType2													);

			pThis->AttachedTemplateEffectWrite(
				pStream,
				pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfoIndex],
				lVisualInfoIndex,
				lAnimType2													);
		}
	}

	return TRUE;
}

BOOL AgcmSkill::AttachedSoundTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgcmSkill	*pThis				= (AgcmSkill *)	pClass;
	ApBase		*pcsBaseTemplate	= (ApBase *)	pData;

	AgcdCharacterTemplate		*pcsAgcdCharacterTemplate	=
		pThis->m_pcsAgcmCharacter->GetTemplateData(
		(AgpdCharacterTemplate *)(pcsBaseTemplate)	);
	if (!pcsAgcdCharacterTemplate)
		return FALSE;

	AgcdSkillAttachTemplateData *pcsTemplateAttachData		= pThis->GetAttachTemplateData(pcsBaseTemplate);
	if (!pcsTemplateAttachData)
		return FALSE;

	CHAR	szValue[256], szTemp[256];
	INT32	lVisualInfoIndex, lAnimType2, lEffectIndex;
	AgcdUseEffectSetData	*pcsEData;
	//AgcdCharacterAnimationAttachedData	*pstAttachedData;
	ACA_AttachedData		*pcsAttachedData	= NULL;
	eAcReturnType			eRt					= E_AC_RETURN_TYPE_NONE;

	while (pStream->ReadNextValue())
	{
		const CHAR	*szValueName = pStream->GetValueName();
		if (!strncmp(szValueName, AGCMSKILL_INI_NAME_EFFECT_SOUND, strlen(AGCMSKILL_INI_NAME_EFFECT_SOUND)))
		{
			if (!pStream->GetValue(szValue, 256))
				return FALSE;

			sscanf(szValue, "%d:%d:%d:%s", &lVisualInfoIndex, &lAnimType2, &lEffectIndex, szTemp);

			if (!strcmp(szTemp, ""))
				continue;

			eRt	= pThis->CheckVisualInfo(pcsAgcdCharacterTemplate, pcsTemplateAttachData, lVisualInfoIndex);
			if (eRt == E_AC_RETURN_TYPE_FAILURE)
			{
				ASSERT(!"!CheckVisualInfo()");
				return FALSE;
			}
			else if (eRt == E_AC_RETURN_TYPE_SKIP)
				continue;

			lAnimType2	= pThis->m_pcsAgcmCharacter->ConvertAnimType2(pcsAgcdCharacterTemplate->m_lAnimType2, lAnimType2);
			if (lAnimType2 == -2)
			{
				continue;
			}
			else if (lAnimType2 == -1)
			{
				ASSERT(!"ERROR!");
				return FALSE;
			}

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2])
				pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2] =
				new AgcdUseEffectSet();

			if (!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2])
			{
				ASSERT(!"!pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2]");
				return FALSE;
			}

			pcsEData = pThis->m_csEffectDataAdmin2.GetData(
				pcsTemplateAttachData->m_pacsSkillVisualInfo[lVisualInfoIndex]->m_pastEffect[lAnimType2],
				lEffectIndex,
				TRUE																					);

			if (!pcsEData)
			{
				ASSERT(!"!pcsEData");
				return FALSE;
			}

			if (pcsEData->m_pszSoundName)
			{
				ASSERT(!"pcsEData->m_pszSoundName");
				return FALSE;
			}

			pcsEData->m_pszSoundName	= new CHAR[strlen(szTemp) + 1];			
			strcpy(pcsEData->m_pszSoundName, szTemp);
		}
	}
	return TRUE;
}

BOOL AgcmSkill::AttachedSoundTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	ApBase		*pcsBaseTemplate	= (ApBase *)	pData;
	AgcmSkill	*pThis				= (AgcmSkill *)	pClass;

	AgpdSkillTemplateAttachData *pcsAgpdSkillData = pThis->m_pcsAgpmSkill->GetAttachSkillTemplateData(pcsBaseTemplate);
	AgcdSkillAttachTemplateData *pcsAgcdSkillData = pThis->GetAttachTemplateData(pcsBaseTemplate);
	if ((!pcsAgpdSkillData) || (!pcsAgcdSkillData))
		return FALSE;

	if (!pcsAgcdSkillData->m_pacsSkillVisualInfo)
		return TRUE; // skip!

	AgcdCharacterTemplate	*pcsAgcdCharacterTemplate	= pThis->m_pcsAgcmCharacter->GetTemplateData((AgpdCharacterTemplate *)(pcsBaseTemplate));
	if (!pcsAgcdCharacterTemplate)
		return FALSE;

	INT32					lNumVisualInfo				= pThis->GetNumVisualInfo(pcsAgcdCharacterTemplate);
	INT32					lNumAnimType2				= pThis->m_pcsAgcmCharacter->GetAnimType2Num(pcsAgcdCharacterTemplate);

	for (INT32 lVisualInfoIndex = 0; lVisualInfoIndex < lNumVisualInfo; ++lVisualInfoIndex)
	{
		if (!pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfoIndex])
			continue;

		for (INT32 lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2)
		{
			pThis->AttachedTemplateEffectSoundWrite(
				pStream,
				pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfoIndex],
				lVisualInfoIndex,
				lAnimType2													);
		}
	}

	return TRUE;
}

BOOL	AgcmSkill::AgcdSkillTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass)	return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass;
	ApBase			*pcsBase		= (ApBase	*)		pData;

	AgcdSkillTemplate	*pcsAgcdSkillTemplate = pThis->GetADSkillTemplate(pcsBase);
	if (!pcsAgcdSkillTemplate)	return FALSE;

	CHAR			szTemp[256];
	memset(szTemp, 0, sizeof (CHAR) * 256);

	//@{ 2006/05/04 burumal
	pcsAgcdSkillTemplate->m_nDNF = 0;
	//@}

	const CHAR	*szValueName = NULL;
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGCMSKILL_INI_NAME_SKILL_TEXTURE))
		{
			pStream->GetValue(szTemp, 256);

			if (!strcmp(szTemp, ""))
				continue;

			if (pcsAgcdSkillTemplate->m_pszTextureName)
			{
				ASSERT(!"pcsAgcdSkillTemplate->m_pszTextureName");
				return FALSE;
			}

			pcsAgcdSkillTemplate->m_pszTextureName	= new CHAR [strlen(szTemp) + 1];
			strcpy(pcsAgcdSkillTemplate->m_pszTextureName, szTemp); 

			//pStream->GetValue(pcsAgcdSkillTemplate->m_szTextureName, AGCDSKILL_TEXTURE_NAME_LENGTH);
			//pcsAgcdSkillTemplate->m_pSkillTexture = RwTextureRead( pcsAgcdSkillTemplate->m_szTextureName, NULL );
		}
		else if (!strcmp(szValueName, AGCMSKILL_INI_NAME_SMALL_SKILL_TEXTURE))
		{
			pStream->GetValue(szTemp, 256);

			if (!strcmp(szTemp, ""))
				continue;

			if (pcsAgcdSkillTemplate->m_pszSmallTextureName)
			{
				ASSERT(!"pcsAgcdSkillTemplate->m_pszSmallTextureName");
				return FALSE;
			}

			pcsAgcdSkillTemplate->m_pszSmallTextureName	= new CHAR [strlen(szTemp) + 1];
			strcpy(pcsAgcdSkillTemplate->m_pszSmallTextureName, szTemp); 

			//pStream->GetValue(pcsAgcdSkillTemplate->m_szSmallTextureName, AGCDSKILL_TEXTURE_NAME_LENGTH);
			//pcsAgcdSkillTemplate->m_pSmallSkillTexture = RwTextureRead( pcsAgcdSkillTemplate->m_szSmallTextureName, NULL );
		}
		else if (!strcmp(szValueName, AGCMSKILL_INI_NAME_UNABLE_SKILL_TEXTURE))
		{
			pStream->GetValue(szTemp, 256);

			if (!strcmp(szTemp, ""))
				continue;

			if (pcsAgcdSkillTemplate->m_pszUnableTextureName)
			{
				ASSERT(!"pcsAgcdSkillTemplate->m_pszUnableTextureName");
				return FALSE;
			}

			pcsAgcdSkillTemplate->m_pszUnableTextureName	= new CHAR [strlen(szTemp) + 1];
			strcpy(pcsAgcdSkillTemplate->m_pszUnableTextureName, szTemp); 

			//pStream->GetValue(pcsAgcdSkillTemplate->m_szUnableTextureName, AGCDSKILL_TEXTURE_NAME_LENGTH);
			//pcsAgcdSkillTemplate->m_pUnableTexture = RwTextureRead( pcsAgcdSkillTemplate->m_szUnableTextureName, NULL );
		}
		//@{ 2006/05/03 burumal
		else if ( !strcmp(szValueName, AGCMSKILL_INI_NAME_DNF_1) )
		{			
			INT32 nDNF = 0;
			pStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 1;

			if ( nDNF > 0 )
				pcsAgcdSkillTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(szValueName, AGCMSKILL_INI_NAME_DNF_2) )
		{			
			INT32 nDNF = 0;
			pStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 2;

			if ( nDNF > 0 )
				pcsAgcdSkillTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(szValueName, AGCMSKILL_INI_NAME_DNF_3) )
		{			
			INT32 nDNF = 0;
			pStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 3;

			if ( nDNF > 0 )
				pcsAgcdSkillTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(szValueName, AGCMSKILL_INI_NAME_DNF_4) )
		{
			INT32 nDNF = 0;
			pStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 4;

			if ( nDNF > 0 )
				pcsAgcdSkillTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		//@}
	}

	return TRUE;
}

BOOL	AgcmSkill::AgcdSkillTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)	return FALSE;

	AgcmSkill		*pThis			= (AgcmSkill *)		pClass	;
	ApBase			*pcsBase		= (ApBase	*)		pData	;

	AgcdSkillTemplate	*pcsAgcdSkillTemplate = pThis->GetADSkillTemplate(pcsBase);
	if (!pcsAgcdSkillTemplate)	return FALSE;

	/*pStream->WriteValue(AGCMSKILL_INI_NAME_SKILL_TEXTURE, pcsAgcdSkillTemplate->m_szTextureName );

	pStream->WriteValue(AGCMSKILL_INI_NAME_SMALL_SKILL_TEXTURE, pcsAgcdSkillTemplate->m_szSmallTextureName );

	pStream->WriteValue(AGCMSKILL_INI_NAME_UNABLE_SKILL_TEXTURE, pcsAgcdSkillTemplate->m_szUnableTextureName );*/

	if (pcsAgcdSkillTemplate->m_pszTextureName)
		pStream->WriteValue(AGCMSKILL_INI_NAME_SKILL_TEXTURE, pcsAgcdSkillTemplate->m_pszTextureName);

	if (pcsAgcdSkillTemplate->m_pszSmallTextureName)
		pStream->WriteValue(AGCMSKILL_INI_NAME_SMALL_SKILL_TEXTURE, pcsAgcdSkillTemplate->m_pszSmallTextureName);

	if (pcsAgcdSkillTemplate->m_pszUnableTextureName)
		pStream->WriteValue(AGCMSKILL_INI_NAME_UNABLE_SKILL_TEXTURE, pcsAgcdSkillTemplate->m_pszUnableTextureName);

	//@{ 2006/05/03 burumal	
	pStream->WriteValue(AGCMSKILL_INI_NAME_DNF_1, (pcsAgcdSkillTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_KOREA		)) ? 1 : 0);
	pStream->WriteValue(AGCMSKILL_INI_NAME_DNF_2, (pcsAgcdSkillTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_CHINA		)) ? 1 : 0);
	pStream->WriteValue(AGCMSKILL_INI_NAME_DNF_3, (pcsAgcdSkillTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_WESTERN	)) ? 1 : 0);
	pStream->WriteValue(AGCMSKILL_INI_NAME_DNF_4, (pcsAgcdSkillTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_JAPAN		)) ? 1 : 0);
	//@}

	return TRUE;
}

BOOL	AgcmSkill::AgcdSkillSpecializeReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	const CHAR					*szValueName		= NULL;
	AgcmSkill					*pThis				= (AgcmSkill *)						pClass;
	AgpdSkillSpecializeTemplate	*pcsSpecialize		= (AgpdSkillSpecializeTemplate *)	pData;

	AgcdSkillSpecializeTemplate	*pcsAgcdSpecialize	= pThis->GetADSpecializeTemplate(pcsSpecialize);
	if (!pcsAgcdSpecialize)
		return FALSE;

	CHAR						szTemp[256];
	memset(szTemp, 0, sizeof (CHAR) * 256);

	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGCMSKILL_INI_NAME_SPECIALIZE_TEXTURE))
		{
			if (pcsAgcdSpecialize->m_pszTextureName)
			{
				ASSERT(!"pcsAgcdSpecialize->m_pszTextureName");
				return FALSE;
			}

			pStream->GetValue(szTemp, AGCDSKILL_TEXTURE_NAME_LENGTH);

			if (!strcmp(szTemp, ""))
				continue;

			pcsAgcdSpecialize->m_pszTextureName	= new CHAR [strlen(szTemp) + 1];
			strcpy(pcsAgcdSpecialize->m_pszTextureName, szTemp);

			//pStream->GetValue(pcsAgcdSpecialize->m_szTextureName, AGCDSKILL_TEXTURE_NAME_LENGTH);
			//pcsAgcdSpecialize->m_pTexture = RwTextureRead( pcsAgcdSpecialize->m_szTextureName, NULL );
		}
	}

	return TRUE;
}

BOOL	AgcmSkill::AgcdSkillSpecializeWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgcmSkill					*pThis				= (AgcmSkill *)						pClass;
	AgpdSkillSpecializeTemplate	*pcsSpecialize		= (AgpdSkillSpecializeTemplate *)	pData;

	AgcdSkillSpecializeTemplate	*pcsAgcdSpecialize	= pThis->GetADSpecializeTemplate(pcsSpecialize);
	if (!pcsAgcdSpecialize)
		return FALSE;

	//if (!pStream->WriteValue(AGCMSKILL_INI_NAME_SPECIALIZE_TEXTURE, pcsAgcdSpecialize->m_szTextureName))
	//	return FALSE;
	if (pcsAgcdSpecialize->m_pszTextureName)
		pStream->WriteValue(AGCMSKILL_INI_NAME_SPECIALIZE_TEXTURE, pcsAgcdSpecialize->m_pszTextureName);

	return TRUE;
}