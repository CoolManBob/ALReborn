// AgpmAI.cpp: implementation of the AgpmAI class.
//
//////////////////////////////////////////////////////////////////////

#include "AgpmAI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//INT32	g_lAIFactorDataSize[AGPDAI_FACTOR_MAX_TYPE] = 
//{
//	0,
//	sizeof(AgpdAIFactorCharFactor),
//	sizeof(AgpdAIFactorCharStatus),
//	sizeof(AgpdAIFactorCharItem),
//	sizeof(AgpdAIFactorActionHistory),
//	sizeof(AgpdAIFactorEnvPosition),
//	sizeof(AgpdAIFactorEmotion)
//};

ApSafeArray<INT32, AGPDAI_FACTOR_MAX_TYPE>	g_lAIFactorDataSize;

AgpmAI::AgpmAI()
{
	SetModuleName("AgpmAI");

	SetModuleData(sizeof(AgpdAITemplate), AGPMAI_DATA_AITEMPLATE);

	m_lTemplates = AGPMAI_DEFAULT_TEMPLATE_NUMBER;

	m_lMaxTID = 0;

	SetPacketType(AGPMAI_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT32,			1,				// character id
							AUTYPE_INT8,			1,				// m_bUseAI
							AUTYPE_INT32,			1,				// m_lTID
							AUTYPE_UINT32,			1,				// m_ulPrevProcessTime
							AUTYPE_END,				0
							);

	g_lAIFactorDataSize[0]		= 0;
	g_lAIFactorDataSize[1]		= sizeof(AgpdAIFactorCharFactor);
	g_lAIFactorDataSize[2]		= sizeof(AgpdAIFactorCharStatus);
	g_lAIFactorDataSize[3]		= sizeof(AgpdAIFactorCharItem);
	g_lAIFactorDataSize[4]		= sizeof(AgpdAIFactorActionHistory);
	g_lAIFactorDataSize[5]		= sizeof(AgpdAIFactorEnvPosition);
	g_lAIFactorDataSize[6]		= sizeof(AgpdAIFactorEmotion);
}

AgpmAI::~AgpmAI()
{

}

BOOL				AgpmAI::OnAddModule()
{
	// Parent Module들 가져오기
	m_pcsApmMap = (ApmMap *) GetModule("ApmMap");
	m_pcsAgpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmEventSpawn = (AgpmEventSpawn *) GetModule("AgpmEventSpawn");

	// Character모듈만 있으면 일단 OK
	if (!m_pcsAgpmCharacter)
		return FALSE;

	// Character와 Template에 Data 붙인다.
	m_nCharacterTemplateAttachIndex = m_pcsAgpmCharacter->AttachCharacterTemplateData(this, sizeof(AgpdAIADChar), CBCharacterTemplateConstructor, CBCharacterTemplateDestructor);
	if (m_nCharacterTemplateAttachIndex < 0)
		return FALSE;

	m_nCharacterAttachIndex = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdAIADChar), CBCharacterConstructor, CBCharacterDestructor);
	if (m_nCharacterAttachIndex < 0)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackInitChar(CBCharacterInit, this))
		return FALSE;

	// Streaming도 등록하자.
	//if(!AddStreamCallback(AGPMCHAR_DATA_TYPE_TEMPLATE, CBCharacterTemplateRead, CBCharacterTemplateWrite, this))
	//	return FALSE;

	if(!AddStreamCallback(AGPMAI_DATA_AITEMPLATE, CBAITemplateRead, CBAITemplateWrite, this))
		return FALSE;

	if (m_pcsAgpmEventSpawn)
	{
		m_nSpawnAttachIndex = m_pcsAgpmEventSpawn->AttachSpawnData(this, sizeof(AgpdAIADSpawn), CBSpawnConstructor, CBSpawnDestructor);
		if (m_nSpawnAttachIndex < 0)
			return FALSE;

		if (!m_pcsAgpmEventSpawn->AddStreamCallback(AGPMSPAWN_DATA_SPAWN, CBSpawnRead, CBSpawnWrite, this))
			return FALSE;
	}

	return TRUE;
}	

BOOL				AgpmAI::OnInit()
{
	if (!m_csAITemplates.InitializeObject(sizeof(AgpdAITemplate *), m_lTemplates, NULL, NULL, NULL))
		return FALSE;

	return TRUE;
}

BOOL				AgpmAI::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL				AgpmAI::OnDestroy()
{
	return TRUE;
}

VOID				AgpmAI::SetMaxTemplate(INT32 lCount)
{
	m_lTemplates = lCount;
}

AgpdAITemplate *	AgpmAI::GetTemplateSequence(INT32 *plIndex)
{
	AgpdAITemplate **ppcsTemplate = (AgpdAITemplate **) m_csAITemplates.GetObjectSequence(plIndex);

	if (!ppcsTemplate)
		return NULL;

	return  *ppcsTemplate;
}

AgpdAIADChar *		AgpmAI::GetCharacterData(AgpdCharacter *pcsCharacter)
{
	// Module Data 가져오기
	return (AgpdAIADChar *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nCharacterAttachIndex, (void *) pcsCharacter);
}

AgpdAIADChar *		AgpmAI::GetCharacterTemplateData(AgpdCharacterTemplate *pcsCharacterTemplate)
{
	// Module Data 가져오기
	return (AgpdAIADChar *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nCharacterTemplateAttachIndex, (void *) pcsCharacterTemplate);
}

AgpdAIADSpawn *		AgpmAI::GetSpawnData(AgpdSpawn *pstSpawn)
{
	// Module Data 가져오기
	return (AgpdAIADSpawn *) m_pcsAgpmEventSpawn->GetAttachedModuleData(m_nSpawnAttachIndex, (void *) pstSpawn);
}

AgpdAITemplate *	AgpmAI::AddAITemplate(INT32 lTID)
{
	AgpdAITemplate *	pstTemplate;

	pstTemplate = (AgpdAITemplate *) CreateModuleData();
	if (!pstTemplate)
		return NULL;

	pstTemplate->m_lID = lTID;

	return AddAITemplate(pstTemplate);
}

AgpdAITemplate *	AgpmAI::AddAITemplate(AgpdAITemplate *pstTemplate)
{
	if (m_lMaxTID < pstTemplate->m_lID)
		m_lMaxTID = pstTemplate->m_lID;

	return m_csAITemplates.AddAITemplate(pstTemplate);
}

AgpdAITemplate *	AgpmAI::GetAITemplate(INT32 lTID)
{
	return m_csAITemplates.GetAITemplate(lTID);
}

BOOL				AgpmAI::RemoveAITemplate(INT32 lTID)
{
	return m_csAITemplates.RemoveAITemplate(lTID);
}

AgpdAIFactor *		AgpmAI::AddAIFactor(AgpdCharacter *pcsCharacter, AgpdAIFactorType eType)
{
	return AddAIFactor(&GetCharacterData(pcsCharacter)->m_stAI, eType);
}

AgpdAIFactor *		AgpmAI::AddAIFactor(AgpdCharacterTemplate *pcsCharacterTemplate, AgpdAIFactorType eType)
{
	return AddAIFactor(&GetCharacterTemplateData(pcsCharacterTemplate)->m_stAI, eType);
}

AgpdAIFactor *		AgpmAI::AddAIFactor(AgpdAI *pstAI, AgpdAIFactorType eType)
{
	if (!pstAI || eType < AGPDAI_FACTOR_TYPE_DEFAULT || eType >= AGPDAI_FACTOR_MAX_TYPE)
		return FALSE;
	
	INT32			lIndex;
	AgpdAIFactor *	pstTailAIFactor = (AgpdAIFactor *) pstAI->m_listAIFactors.GetTailNode();
	AgpdAIFactor *	pstAIFactor = (AgpdAIFactor *) GlobalAlloc(GMEM_FIXED, sizeof(AgpdAIFactor));
	if (!pstAIFactor)
		return NULL;

	memset(pstAIFactor, 0, sizeof(AgpdAIFactor));
	for (lIndex = 0; lIndex < AGPDAI_ACTION_MAX_TYPE; ++lIndex)
	{
		pstAIFactor->m_astActions[lIndex].m_eActionType = (AgpdAIActionType) lIndex;
	}

	// 만약 List에 이미 들어가 있는 Node가 있으면, 그 마지막 Node의 FactorID에 1을 더해서 Factor ID를 생성하자. 없으면, 1
	if (pstTailAIFactor)
		pstAIFactor->m_lFactorID = pstTailAIFactor->m_lFactorID + 1;
	else
		pstAIFactor->m_lFactorID = 1;

	// Type 넣고
	pstAIFactor->m_eType = eType;

	// Type에 따라 알맞는 Data형을 할당해준다.
	if (g_lAIFactorDataSize[eType])
	{
		pstAIFactor->m_uoData.m_pvData = malloc(g_lAIFactorDataSize[eType]);
	}

	// 마지막으로 AddTail()해서 List에 넣고, 넣은 Data를 return한다.
	if (pstAI->m_listAIFactors.AddTail(*pstAIFactor))
	{
		GlobalFree(pstAIFactor);
		return &pstAI->m_listAIFactors.GetTail();
	}

	GlobalFree(pstAIFactor);
	return NULL;
}

AgpdAIFactor *		AgpmAI::GetAIFactor(AgpdCharacter *pcsCharacter, INT32 lFactorID)
{
	return GetAIFactor(&GetCharacterData(pcsCharacter)->m_stAI, lFactorID);
}

AgpdAIFactor *		AgpmAI::GetAIFactor(AgpdCharacterTemplate *pcsCharacterTemplate, INT32 lFactorID)
{
	return GetAIFactor(&GetCharacterTemplateData(pcsCharacterTemplate)->m_stAI, lFactorID);
}

AgpdAIFactor *		AgpmAI::GetAIFactor(AgpdAI *pstAI, INT32 lFactorID)
{
	AgpdAIFactor *				pstAIFactor;
	AuNode< AgpdAIFactor > *	pNode;

	// Head Node부터 마지막까지 Traversing
	pNode = pstAI->m_listAIFactors.GetHeadNode();
	while (pNode)
	{
		pstAIFactor	= &pNode->GetData();

		// 해당 FactorID를 return한다.
		if (pstAIFactor->m_lFactorID == lFactorID)
		{
			return pstAIFactor;
		}

		pstAI->m_listAIFactors.GetNext(pNode);
	}

	return NULL;
}

AgpdAIFactor *		AgpmAI::RemoveAIFactor(AgpdCharacter *pcsCharacter, INT32 lFactorID)
{
	return RemoveAIFactor(&GetCharacterData(pcsCharacter)->m_stAI, lFactorID);
}

AgpdAIFactor *		AgpmAI::RemoveAIFactor(AgpdCharacterTemplate *pcsCharacterTemplate, INT32 lFactorID)
{
	return RemoveAIFactor(&GetCharacterTemplateData(pcsCharacterTemplate)->m_stAI, lFactorID);
}

AgpdAIFactor *		AgpmAI::RemoveAIFactor(AgpdAI *pstAI, INT32 lFactorID)
{
	AgpdAIFactor *				pstAIFactor;
	AuNode< AgpdAIFactor > *	pNode;

	// Head Node부터 마지막까지 Traversing
	pNode = pstAI->m_listAIFactors.GetHeadNode();
	while (pNode)
	{
		pstAIFactor	= &pNode->GetData();

		// 해당 FactorID를 만나면
		if (pstAIFactor->m_lFactorID == lFactorID)
		{
			// Data가 있으면
			if (pstAIFactor->m_uoData.m_pvData)
			{
				free(pstAIFactor->m_uoData.m_pvData);
				pstAIFactor->m_uoData.m_pvData = NULL;
			}

			// Node를 삭제하고
			if (!pstAI->m_listAIFactors.RemoveNode(pNode))
				return NULL;

			// 해당 AIFactor를 return
			return pstAIFactor;
		}

		pstAI->m_listAIFactors.GetNext(pNode);
	}

	return NULL;
}

BOOL				AgpmAI::DestroyAI(AgpdAI *pstAI)
{
	AgpdAIFactor *				pstAIFactor;
	AuNode< AgpdAIFactor > *	pNode;

	// Head Node부터 마지막까지 Traversing
	pNode = pstAI->m_listAIFactors.GetHeadNode();
	while (pNode)
	{
		pstAIFactor	= &pNode->GetData();

		// Data가 있으면
		if (pstAIFactor->m_uoData.m_pvData)
		{
			free(pstAIFactor->m_uoData.m_pvData);
		}

		pstAI->m_listAIFactors.GetNext(pNode);
	}

	// List에서 모두 삭제
	pstAI->m_listAIFactors.RemoveAll();

	return TRUE;
}

BOOL				AgpmAI::CopyAI(AgpdAI *pstDstAI, AgpdAI *pstSrcAI, BOOL bAddFactor)
{
	AuNode< AgpdAIFactor > *	pNode;
	AgpdAIFactor *				pstDstAIFactor;
	AgpdAIFactor *				pstSrcAIFactor;

	if (!bAddFactor)
		DestroyAI(pstDstAI);

	pstDstAI->m_fVisibility			= pstSrcAI->m_fVisibility;
	pstDstAI->m_ulProcessInterval	= pstSrcAI->m_ulProcessInterval;

	// Head Node부터 마지막까지 Traversing
	pNode = pstSrcAI->m_listAIFactors.GetHeadNode();
	while (pNode)
	{
		pstSrcAIFactor	= &pNode->GetData();

		// Source의 AIFactor를 Destination에도 추가
		pstDstAIFactor = AddAIFactor(pstDstAI, pstSrcAIFactor->m_eType);

		// Action은 Memory를 Copy한다.
		//memcpy(pstDstAIFactor->m_astActions, pstSrcAIFactor->m_astActions, sizeof(AgpdAIAction) * AGPDAI_ACTION_MAX_TYPE);
		pstDstAIFactor->m_astActions.MemCopy(0, &pstSrcAIFactor->m_astActions[0], AGPDAI_ACTION_MAX_TYPE);

		if (pstSrcAIFactor->m_eType >= AGPDAI_FACTOR_TYPE_DEFAULT && pstSrcAIFactor->m_eType < AGPDAI_FACTOR_MAX_TYPE)
		{
			// Source의 Data를 Copy한다.
			if (pstSrcAIFactor->m_uoData.m_pvData && pstDstAIFactor->m_uoData.m_pvData)
				memcpy(pstDstAIFactor->m_uoData.m_pvData, pstSrcAIFactor->m_uoData.m_pvData, g_lAIFactorDataSize[pstSrcAIFactor->m_eType]);
		}

		// AIFactor ID도 복사
		pstDstAIFactor->m_lFactorID = pstSrcAIFactor->m_lFactorID;

		pstSrcAI->m_listAIFactors.GetNext(pNode);
	}

	return TRUE;
}

BOOL				AgpmAI::StreamWriteTemplate(CHAR *szFile)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szTID[32];
	AgpdAITemplate *	pcsTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// 등록된 모든 Character Template에 대해서...
	for (pcsTemplate = GetTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = GetTemplateSequence(&lIndex))
	{
		sprintf(szTID, "%d", pcsTemplate->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMAI_DATA_AITEMPLATE, pcsTemplate, this))
			return FALSE;
	}

	csStream.Write(szFile);

	return TRUE;
}

BOOL				AgpmAI::StreamReadTemplate(CHAR *szFile)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	INT32				lTID;
	AgpdAITemplate *	pcsTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile);

	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; i++)
	{
		// Section Name은 TID 이다.
		lTID = atoi(csStream.ReadSectionName(i));

		// Template을 추가한다.
		pcsTemplate = AddAITemplate(lTID);
		if (!pcsTemplate)
			return FALSE;

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMAI_DATA_AITEMPLATE, pcsTemplate, this))
			return FALSE;
	}

	return TRUE;
}

BOOL				AgpmAI::CBCharacterInit(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpmAI *					pThis = (AgpmAI *) pvClass;
	AgpdCharacter *				pcsCharacter = (AgpdCharacter *) pvData;
	AgpdCharacterTemplate *		pcsCharacterTemplate = (AgpdCharacterTemplate *) pvCustData;
	AgpdAIADChar *				pstAIADChar = pThis->GetCharacterData(pcsCharacter);
	AgpdAIADChar *				pstAIADCharTemplate = pThis->GetCharacterTemplateData(pcsCharacterTemplate);

	// AI 생성

	if (pcsCharacterTemplate)
	{
		pThis->CopyAI(&pstAIADChar->m_stAI, &pstAIADCharTemplate->m_stAI);
	}

	// Test입니다.
	AgpdAIFactor *				pstAIFactor = pThis->AddAIFactor(&pstAIADChar->m_stAI, AGPDAI_FACTOR_TYPE_DEFAULT);

	pstAIFactor->m_astActions[AGPDAI_ACTION_TYPE_HOLD].m_lActionRate = 10;
	pstAIFactor->m_astActions[AGPDAI_ACTION_TYPE_WANDER].m_lActionRate = 40;

	pstAIFactor->m_astActions[AGPDAI_ACTION_TYPE_ATTACK].m_lActionRate = 100;
	pstAIFactor->m_astActions[AGPDAI_ACTION_TYPE_ATTACK].m_stTarget.m_csTargetBase.m_eType = APBASE_TYPE_CHARACTER;
	pstAIFactor->m_astActions[AGPDAI_ACTION_TYPE_ATTACK].m_stTarget.m_lTargetFlags = AGPDAI_TARGET_FLAG_OTHER | AGPDAI_TARGET_FLAG_HISTORY_DAMAGE;
	pstAIFactor->m_astActions[AGPDAI_ACTION_TYPE_ATTACK].m_stTarget.m_stTargetWeight.m_lHistoryDamage = 10;
	pstAIFactor->m_astActions[AGPDAI_ACTION_TYPE_ATTACK].m_stTarget.m_stTargetWeight.m_lNear = 10;

	return TRUE;
}

BOOL				AgpmAI::CBCharacterConstructor(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	return TRUE;
}

BOOL				AgpmAI::CBCharacterDestructor(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpmAI *					pThis = (AgpmAI *) pvClass;
	AgpdCharacter *				pcsCharacter = (AgpdCharacter *) pvData;
	AgpdAIADChar *				pstAIADChar = pThis->GetCharacterData(pcsCharacter);

	// AI 파괴
	return pThis->DestroyAI(&pstAIADChar->m_stAI);
}

BOOL				AgpmAI::CBCharacterTemplateConstructor(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdCharacterTemplate *		pcsCharacterTemplate = (AgpdCharacterTemplate *) pvData;
	AgpmAI *					pThis = (AgpmAI *) pvClass;
	AgpdAIADChar *				pstAIADChar = pThis->GetCharacterTemplateData(pcsCharacterTemplate);

	pstAIADChar->m_bUseAI = FALSE;

	pstAIADChar->m_stAI.m_ulProcessInterval = AGPMAI_DEFAULT_PROCESS_INTERVAL;
	pstAIADChar->m_stAI.m_fVisibility = AGPMAI_DEFAULT_VISIBILITY;

	// AI 생성

	return TRUE;
}

BOOL				AgpmAI::CBCharacterTemplateDestructor(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdCharacterTemplate *		pcsCharacterTemplate = (AgpdCharacterTemplate *) pvData;
	AgpmAI *					pThis = (AgpmAI *) pvClass;
	AgpdAIADChar *				pstAIADChar = pThis->GetCharacterTemplateData(pcsCharacterTemplate);

	// AI 파괴
	return pThis->DestroyAI(&pstAIADChar->m_stAI);
}

BOOL				AgpmAI::CBSpawnConstructor(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdSpawn *					pstSpawn = (AgpdSpawn *) pvData;
	AgpmAI *					pThis = (AgpmAI *) pvClass;
	AgpdAIADSpawn *				pstAIADSpawn = pThis->GetSpawnData(pstSpawn);
	INT32						lIndex;

	for (lIndex = 0; lIndex < AGPDSPAWN_MAX_CHAR_NUM; ++lIndex)
	{
		pstAIADSpawn->m_astAIADChar[lIndex].m_bUseAI = TRUE;

		pstAIADSpawn->m_astAIADChar[lIndex].m_stAI.m_ulProcessInterval = AGPMAI_DEFAULT_PROCESS_INTERVAL;
		pstAIADSpawn->m_astAIADChar[lIndex].m_stAI.m_fVisibility = AGPMAI_DEFAULT_VISIBILITY;
	}

	// AI 생성

	return TRUE;
}

BOOL				AgpmAI::CBSpawnDestructor(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdSpawn *					pstSpawn = (AgpdSpawn *) pvData;
	AgpmAI *					pThis = (AgpmAI *) pvClass;
	AgpdAIADSpawn *				pstAIADSpawn = pThis->GetSpawnData(pstSpawn);
	INT32						lIndex;

	for (lIndex = 0; lIndex < AGPDSPAWN_MAX_CHAR_NUM; ++lIndex)
	{
		pThis->DestroyAI(&pstAIADSpawn->m_astAIADChar[lIndex].m_stAI);
	}

	// AI 파괴
	return TRUE;
}

BOOL				AgpmAI::CBAITemplateWrite(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpmAI *					pThis = (AgpmAI *) pClass;
	AgpdAITemplate *			pstTemplate = (AgpdAITemplate *) pData;
	AgpdAIFactor *				pstAIFactor;
	AuNode< AgpdAIFactor > *	pNode;
	INT32						lCounter;
	INT32						lIndex;
	INT32						lUsableItemCount;
	INT32						lUsableSkillCount;
	CHAR						szTemp[256];
	AgpdAITargetWeight *		pstWeight;

	lUsableItemCount = 0;
	lUsableSkillCount = 0;

	pStream->WriteValue(AGPMAI_STREAM_NAME_NAME, pstTemplate->m_szName);
	pStream->WriteValue(AGPMAI_STREAM_NAME_VISIBILITY, pstTemplate->m_stAI.m_fVisibility);
	pStream->WriteValue(AGPMAI_STREAM_NAME_INTERVAL, (INT32) pstTemplate->m_stAI.m_ulProcessInterval);

	for( lCounter=0; lCounter<AGPDAI_MAX_USABLE_ITEM_COUNT; lCounter++ )
	{
		if( pstTemplate->m_stAI.m_csAIUseItem.m_alTID[lCounter] && pstTemplate->m_stAI.m_csAIUseItem.m_alRate[lCounter] )
		{
			lUsableItemCount++;
		}
	}

	//사용가능 아이템 정보를 넣자.
	pStream->WriteValue(AGPMAI_STREAM_NAME_USABLE_ITEM_COUNT, lUsableItemCount );

	if( lUsableItemCount != 0 )
	{
		for( lCounter=0; lCounter<lUsableItemCount; lCounter++ )
		{
			pStream->WriteValue(AGPMAI_STREAM_NAME_USABLE_ITEM_TID, pstTemplate->m_stAI.m_csAIUseItem.m_alTID[lCounter] );
			pStream->WriteValue(AGPMAI_STREAM_NAME_USABLE_ITEM_RATE, pstTemplate->m_stAI.m_csAIUseItem.m_alRate[lCounter] );
		}
	}

	//사용가능 스킬 정보를 넣자.
	pStream->WriteValue(AGPMAI_STREAM_NAME_USABLE_SKILL_COUNT, lUsableSkillCount );

	if( lUsableSkillCount != 0 )
	{
		if (lUsableSkillCount >= AGPDAI_MAX_USABLE_ITEM_COUNT)
			lUsableSkillCount	= AGPDAI_MAX_USABLE_ITEM_COUNT;

		for( lCounter=0; lCounter<lUsableSkillCount; lCounter++ )
		{
			pStream->WriteValue(AGPMAI_STREAM_NAME_USABLE_SKILL_TID, pstTemplate->m_stAI.m_csAIUseSkill.m_alTID[lCounter] );
			pStream->WriteValue(AGPMAI_STREAM_NAME_USABLE_SKILL_RATE, pstTemplate->m_stAI.m_csAIUseSkill.m_alRate[lCounter] );
		}
	}

	//스크림 정보를 넣자.
	if( pstTemplate->m_stAI.m_csAIScream.m_bUseScream )
	{
		//사용하므로 1
		pStream->WriteValue( AGPMAI_STREAM_NAME_SCREAM_USE, 1 );

		if( pstTemplate->m_stAI.m_csAIScream.m_bHelpAll )
		{
			pStream->WriteValue( AGPMAI_STREAM_NAME_SCREAM_HELP_ALL, 1 );
		}
		else
		{
			int			iHelpCharTypeCount;
			iHelpCharTypeCount = 0;

			for( lCounter=0; lCounter<AGPDAI_MAX_HELP_CHARTYPE_COUNT; lCounter++ )
			{
				if( pstTemplate->m_stAI.m_csAIScream.m_alHelpCharTID[lCounter] )
				{
					iHelpCharTypeCount++;
				}
			}

			pStream->WriteValue( AGPMAI_STREAM_NAME_SCREAM_HELP_CHAR_TYPE_COUNT, iHelpCharTypeCount );

			for( lCounter=0; lCounter<iHelpCharTypeCount; lCounter++ )
			{
				pStream->WriteValue(AGPMAI_STREAM_NAME_SCREAM_HELP_CHAR_TID, pstTemplate->m_stAI.m_csAIScream.m_alHelpCharTID[lCounter] );
			}
		}

		pStream->WriteValue( AGPMAI_STREAM_NAME_SCREAM_FEARLV1, pstTemplate->m_stAI.m_csAIScream.m_lFearLV1Agro );
		pStream->WriteValue( AGPMAI_STREAM_NAME_SCREAM_FEARLV2, pstTemplate->m_stAI.m_csAIScream.m_lFearLV2HP );
		pStream->WriteValue( AGPMAI_STREAM_NAME_SCREAM_FEARLV3, pstTemplate->m_stAI.m_csAIScream.m_bFearLV3 );
		pStream->WriteValue( AGPMAI_STREAM_NAME_SCREAM_HELPLV1, pstTemplate->m_stAI.m_csAIScream.m_lHelpLV1CopyAgro );
		pStream->WriteValue( AGPMAI_STREAM_NAME_SCREAM_HELPLV2, pstTemplate->m_stAI.m_csAIScream.m_lHelpLV2Heal );
		pStream->WriteValue( AGPMAI_STREAM_NAME_SCREAM_HELPLV3, pstTemplate->m_stAI.m_csAIScream.m_lHelpLV3 );
	}
	else
	{
		//사용하지 않으므로 0
		pStream->WriteValue( AGPMAI_STREAM_NAME_SCREAM_USE, 0 );
	}

	// Head Node부터 마지막까지 Traversing
	pNode = pstTemplate->m_stAI.m_listAIFactors.GetHeadNode();
	while (pNode)
	{
		pstAIFactor	= &pNode->GetData();

		pStream->WriteValue(AGPMAI_STREAM_NAME_AIFACTOR_ID, pstAIFactor->m_lFactorID);
		pStream->WriteValue(AGPMAI_STREAM_NAME_AIFACTOR_TYPE, (INT32) pstAIFactor->m_eType);
		switch (pstAIFactor->m_eType)
		{
		case AGPDAI_FACTOR_TYPE_CHAR_FACTOR:
			pStream->WriteValue(AGPMAI_STREAM_NAME_AIFACTOR_CHAR_FACTOR_COMPARE, pstAIFactor->m_uoData.m_pstCharFactor->m_lCompareValue);
			pThis->m_pcsAgpmFactors->StreamWrite(&pstAIFactor->m_uoData.m_pstCharFactor->m_stFactor, pStream);
			break;
		case AGPDAI_FACTOR_TYPE_CHAR_STATUS:
			pStream->WriteValue(AGPMAI_STREAM_NAME_AIFACTOR_CHAR_STATUS, pstAIFactor->m_uoData.m_pstCharStatus->m_eStatus);
			break;
		case AGPDAI_FACTOR_TYPE_CHAR_ITEM:
			pStream->WriteValue(AGPMAI_STREAM_NAME_AIFACTOR_CHAR_ITEM, pstAIFactor->m_uoData.m_pstCharItem->m_lItemTID);
			break;
		case AGPDAI_FACTOR_TYPE_ACTION_HISTORY:
		case AGPDAI_FACTOR_TYPE_ENV_POSITION:
		case AGPDAI_FACTOR_TYPE_EMOTION:
			break;
		}

		// Stream actions
		for (lIndex = 0; lIndex < AGPDAI_ACTION_MAX_TYPE; ++lIndex)
		{
			if (pstAIFactor->m_astActions[lIndex].m_lActionRate)
			{
				pStream->WriteValue(AGPMAI_STREAM_NAME_ACTION_TYPE, pstAIFactor->m_astActions[lIndex].m_eActionType);
				pStream->WriteValue(AGPMAI_STREAM_NAME_ACTION_RATE, pstAIFactor->m_astActions[lIndex].m_lActionRate);
				pStream->WriteValue(AGPMAI_STREAM_NAME_TARGET_BASE_TYPE, pstAIFactor->m_astActions[lIndex].m_stTarget.m_csTargetBase.m_eType);
				pStream->WriteValue(AGPMAI_STREAM_NAME_TARGET_BASE_ID, pstAIFactor->m_astActions[lIndex].m_stTarget.m_csTargetBase.m_lID);
				pStream->WriteValue(AGPMAI_STREAM_NAME_TARGET_POS, &pstAIFactor->m_astActions[lIndex].m_stTarget.m_stTargetPos);
				pStream->WriteValue(AGPMAI_STREAM_NAME_TARGET_RADIUS, pstAIFactor->m_astActions[lIndex].m_stTarget.m_fRadius);
				pStream->WriteValue(AGPMAI_STREAM_NAME_TARGET_FLAGS, pstAIFactor->m_astActions[lIndex].m_stTarget.m_lTargetFlags);

				pstWeight = &pstAIFactor->m_astActions[lIndex].m_stTarget.m_stTargetWeight;
				wsprintf(szTemp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
					, pstWeight->m_lRelationAggressive
					, pstWeight->m_lRelationFriendly
					, pstWeight->m_lAgro
					, pstWeight->m_lHistoryDamage
					, pstWeight->m_lHistoryBuffer
					, pstWeight->m_lNear
					, pstWeight->m_lFar
					, pstWeight->m_lBoss
					, pstWeight->m_lFollower
					, pstWeight->m_lStrong
					, pstWeight->m_lWeak
					, pstWeight->m_lSelf
					, pstWeight->m_lOther
					);
				pStream->WriteValue(AGPMAI_STREAM_NAME_TARGET_WEIGHT, szTemp);
			}
		}

		pstTemplate->m_stAI.m_listAIFactors.GetNext(pNode);
	}

	return TRUE;
}

BOOL				AgpmAI::CBAITemplateRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpmAI *					pThis = (AgpmAI *) pClass;
	AgpdAITemplate *			pstTemplate = (AgpdAITemplate *) pData;
	AgpdAIFactor *				pstAIFactor = NULL;
	AgpdAIAction *				pstAIAction = NULL;
	const CHAR *				szValueName;
	const CHAR *				szValue;
	INT32						lAIFactorID;
	INT32						lAIActionType;
	AgpdAIFactorType			eAIFactorType;
	AgpdAITargetWeight *		pstWeight;

	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPMAI_STREAM_NAME_NAME))
		{
			pStream->GetValue(pstTemplate->m_szName, AGPDAI_TEMPLATE_NAME_LENGTH);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_VISIBILITY))
		{
			pStream->GetValue(&pstTemplate->m_stAI.m_fVisibility);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_INTERVAL))
		{
			pStream->GetValue((INT32 *) &pstTemplate->m_stAI.m_ulProcessInterval);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_AIFACTOR_ID))
		{
			pStream->GetValue(&lAIFactorID);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_AIFACTOR_TYPE))
		{
			pStream->GetValue((INT32 *) &eAIFactorType);
			pstAIFactor = pThis->AddAIFactor(&pstTemplate->m_stAI, eAIFactorType);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_AIFACTOR_CHAR_FACTOR_COMPARE))
		{
			pStream->GetValue(&pstAIFactor->m_uoData.m_pstCharFactor->m_lCompareValue);
		}
		else if (!strcmp(szValueName, AGPM_FACTORS_INI_START))
		{
			pThis->m_pcsAgpmFactors->StreamRead(&pstAIFactor->m_uoData.m_pstCharFactor->m_stFactor, pStream);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_AIFACTOR_CHAR_STATUS))
		{
			pStream->GetValue((INT32 *) &pstAIFactor->m_uoData.m_pstCharStatus->m_eStatus);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_AIFACTOR_CHAR_ITEM))
		{
			pStream->GetValue(&pstAIFactor->m_uoData.m_pstCharItem->m_lItemTID);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_ACTION_TYPE))
		{
			pStream->GetValue(&lAIActionType);
			pstAIAction = &pstAIFactor->m_astActions[lAIActionType];
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_ACTION_RATE))
		{
			pStream->GetValue(&pstAIAction->m_lActionRate);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_TARGET_BASE_TYPE))
		{
			pStream->GetValue((INT32 *)&pstAIAction->m_stTarget.m_csTargetBase.m_eType);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_TARGET_BASE_ID))
		{
			pStream->GetValue(&pstAIAction->m_stTarget.m_csTargetBase.m_lID);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_TARGET_POS))
		{
			pStream->GetValue(&pstAIAction->m_stTarget.m_stTargetPos);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_TARGET_RADIUS))
		{
			pStream->GetValue(&pstAIAction->m_stTarget.m_fRadius);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_TARGET_FLAGS))
		{
			pStream->GetValue(&pstAIAction->m_stTarget.m_lTargetFlags);
		}
		else if (!strcmp(szValueName, AGPMAI_STREAM_NAME_TARGET_WEIGHT))
		{
			szValue = pStream->GetValue();
			pstWeight = &pstAIAction->m_stTarget.m_stTargetWeight;
			sscanf(szValue, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
				, &pstWeight->m_lRelationAggressive
				, &pstWeight->m_lRelationFriendly
				, &pstWeight->m_lAgro
				, &pstWeight->m_lHistoryDamage
				, &pstWeight->m_lHistoryBuffer
				, &pstWeight->m_lNear
				, &pstWeight->m_lFar
				, &pstWeight->m_lBoss
				, &pstWeight->m_lFollower
				, &pstWeight->m_lStrong
				, &pstWeight->m_lWeak
				, &pstWeight->m_lSelf
				, &pstWeight->m_lOther
				);
		}
		else if( !strcmp( szValueName, AGPMAI_STREAM_NAME_USABLE_ITEM_COUNT ) )
		{
			INT32				lItemCount;
			INT32				lItemTID;
			INT32				lItemRate;
			INT32				lCount;

			pStream->GetValue(&lItemCount);

			if (lItemCount > AGPDAI_MAX_USABLE_ITEM_COUNT)
				lItemCount	= AGPDAI_MAX_USABLE_ITEM_COUNT;

			for( lCount=0; lCount<lItemCount; lCount++ )
			{
				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_USABLE_ITEM_TID ) )
				{
					pStream->GetValue(&lItemTID);
				}

				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_USABLE_ITEM_RATE ) )
				{
					pStream->GetValue(&lItemRate);
				}

				for( int lTemp=0; lTemp<lItemCount; lTemp++ )
				{
					if( pstTemplate->m_stAI.m_csAIUseItem.m_alTID[lTemp] == 0 )
					{
						pstTemplate->m_stAI.m_csAIUseItem.m_alTID[lTemp] = lItemTID;
						pstTemplate->m_stAI.m_csAIUseItem.m_alRate[lTemp] = lItemRate;
						break;
					}
				}
			}
		}
		else if( !strcmp( szValueName, AGPMAI_STREAM_NAME_USABLE_SKILL_COUNT ) )
		{
			INT32				lSkillCount;
			INT32				lSkillTID;
			INT32				lSkillRate;
			INT32				lCount;

			pStream->GetValue(&lSkillCount);

			if (lSkillCount > AGPDAI_MAX_USABLE_ITEM_COUNT)
				lSkillCount	= AGPDAI_MAX_USABLE_ITEM_COUNT;

			for( lCount=0; lCount<lSkillCount; lCount++ )
			{
				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_USABLE_SKILL_TID ) )
				{
					pStream->GetValue(&lSkillTID);
				}

				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_USABLE_SKILL_RATE ) )
				{
					pStream->GetValue(&lSkillRate);
				}

				for( int lTemp=0; lTemp<lSkillCount; lTemp++ )
				{
					if( pstTemplate->m_stAI.m_csAIUseSkill.m_alTID[lTemp] == 0 )
					{
						pstTemplate->m_stAI.m_csAIUseSkill.m_alTID[lTemp] = lSkillTID;
						pstTemplate->m_stAI.m_csAIUseSkill.m_alRate[lTemp] = lSkillRate;
						break;
					}
				}
			}
		}
		else if( !strcmp( szValueName, AGPMAI_STREAM_NAME_SCREAM_USE ) )
		{
			INT32				lScreamUse;

			pStream->GetValue(&lScreamUse);

			if( lScreamUse != 0 )
			{
				INT32				lHelpAll;
				INT32				lFearLV1, lFearLV2, lFearLV3;
				INT32				lHelpLV1, lHelpLV2, lHelpLV3;

				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_SCREAM_HELP_ALL ) )
				{
					pStream->GetValue(&lHelpAll);
				}

				//모두다 도와준다.
				if( lHelpAll != 0 )
				{
					pstTemplate->m_stAI.m_csAIScream.m_bHelpAll = true;
				}
				//어떤 TID를 가진 캐릭을 도와주는지 본다.
				else
				{
					INT32			lCharTypeCount;
					INT32			lCount;
					INT32			lCharTID;

					if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_SCREAM_HELP_CHAR_TYPE_COUNT ) )
					{
						pStream->GetValue(&lCharTypeCount);

						if (lCharTypeCount > AGPDAI_MAX_HELP_CHARTYPE_COUNT)
							lCharTypeCount	= AGPDAI_MAX_HELP_CHARTYPE_COUNT;

						for( lCount=0; lCount<lCharTypeCount; lCount++ )
						{
							if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_SCREAM_HELP_CHAR_TID ) )
							{
								pStream->GetValue(&lCharTID);

								for( int iTempCount=0; iTempCount<lCharTypeCount; iTempCount++ )
								{
									if( pstTemplate->m_stAI.m_csAIScream.m_alHelpCharTID[iTempCount] == 0 )
									{
										pstTemplate->m_stAI.m_csAIScream.m_alHelpCharTID[iTempCount] = lCharTID;
										break;
									}
								}
							}
						}
					}
				}

				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_SCREAM_FEARLV1 ) )
				{
					pStream->GetValue(&lFearLV1);

					pstTemplate->m_stAI.m_csAIScream.m_lFearLV1Agro = lFearLV1;
				}
				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_SCREAM_FEARLV2 ) )
				{
					pStream->GetValue(&lFearLV2);
					pstTemplate->m_stAI.m_csAIScream.m_lFearLV2HP = lFearLV2;
				}
				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_SCREAM_FEARLV3 ) )
				{
					pStream->GetValue(&lFearLV3);

					if( lFearLV3 != 0 )
					{
						pstTemplate->m_stAI.m_csAIScream.m_bFearLV3 = true;
					}
					else
					{
						pstTemplate->m_stAI.m_csAIScream.m_bFearLV3 = false;
					}
				}
				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_SCREAM_HELPLV1 ) )
				{
					pStream->GetValue(&lHelpLV1);
					pstTemplate->m_stAI.m_csAIScream.m_lHelpLV1CopyAgro = lHelpLV1;
				}
				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_SCREAM_HELPLV2 ) )
				{
					pStream->GetValue(&lHelpLV2);
					pstTemplate->m_stAI.m_csAIScream.m_lHelpLV2Heal = lHelpLV2;
				}
				if( !strcmp( pStream->GetValueName(), AGPMAI_STREAM_NAME_SCREAM_HELPLV3 ) )
				{
					pStream->GetValue(&lHelpLV3);
					pstTemplate->m_stAI.m_csAIScream.m_lHelpLV3 = lHelpLV3;
				}
			}
		}
	}

	return TRUE;
}

BOOL				AgpmAI::CBCharacterTemplateWrite(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	return TRUE;
}

BOOL				AgpmAI::CBCharacterTemplateRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	return TRUE;
}

BOOL				AgpmAI::CBSpawnWrite(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpmAI *					pThis = (AgpmAI *) pClass;
	AgpdSpawn *					pstSpawn = (AgpdSpawn *) pData;
	AgpdAIADSpawn *				pstAIADSpawn = pThis->GetSpawnData(pstSpawn);
	INT32						lIndex;

	INT32	lSpawnChar	= pstSpawn->m_stConfig.m_lSpawnChar;
	if (lSpawnChar > AGPDSPAWN_MAX_CHAR_NUM)
		lSpawnChar	= AGPDSPAWN_MAX_CHAR_NUM;

	for (lIndex = 0; lIndex < lSpawnChar; ++lIndex)
	{
		pStream->WriteValue(AGPMAI_STREAM_NAME_TEMPLATE, pstAIADSpawn->m_astAIADChar[lIndex].m_lTID);
	}

	pStream->WriteValue(AGPMAI_STREAM_NAME_END, 0);

	return TRUE;
}

BOOL				AgpmAI::CBSpawnRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpmAI *					pThis = (AgpmAI *) pClass;
	AgpdSpawn *					pstSpawn = (AgpdSpawn *) pData;
	AgpdAIADSpawn *				pstAIADSpawn = pThis->GetSpawnData(pstSpawn);
	INT32						lIndex = 0;

	while (pStream->ReadNextValue() && lIndex < AGPDSPAWN_MAX_CHAR_NUM)
	{
		if (!strcmp(pStream->GetValueName(), AGPMAI_STREAM_NAME_TEMPLATE))
		{
			pStream->GetValue(&pstAIADSpawn->m_astAIADChar[lIndex].m_lTID);
			++lIndex;
		}
		else if (!strcmp(pStream->GetValueName(), AGPMAI_STREAM_NAME_END))
		{
			pStream->ReadNextValue();
			return TRUE;
		}
	}

	return TRUE;
}

PVOID				AgpmAI::MakePacketAIADChar(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	AgpdAIADChar		*pcsAIADChar	= GetCharacterData(pcsCharacter);
	if (!pcsAIADChar)
		return NULL;

	INT8				cUseAI			= pcsAIADChar->m_bUseAI;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMAI_PACKET_TYPE,
										&pcsCharacter->m_lID,
										&cUseAI,
										&pcsAIADChar->m_lTID,
										&pcsAIADChar->m_ulPrevProcessTime);
}

BOOL				AgpmAI::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket ||
		nSize <= 0 ||
		ulNID == 0)
		return FALSE;

	INT32				lCID				= AP_INVALID_CID;
	INT8				cUseAI				= (-1);
	INT32				lAITID				= (-1);
	UINT32				ulPrevProcessTime	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&lCID,
						&cUseAI,
						&lAITID,
						&ulPrevProcessTime
						);

	AgpdCharacter		*pcsCharacter		= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
	{
		OutputDebugString("AgpmAI::OnReceive() Error !!! - GetCharacterLock() return NULL \n");
		return FALSE;
	}

	AgpdAIADChar		*pcsAgpdAIADChar	= GetCharacterData(pcsCharacter);
	if (!pcsAgpdAIADChar)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if (cUseAI != (-1))
		pcsAgpdAIADChar->m_bUseAI				= (BOOL) cUseAI;

	if (ulPrevProcessTime != 0)
		pcsAgpdAIADChar->m_ulPrevProcessTime	= ulPrevProcessTime;

	if (lAITID != (-1))
	{
		AgpdAITemplate		*pcsAITemplate			= GetAITemplate(lAITID);
		if (pcsAITemplate)
		{
			if (!CopyAI(&pcsAgpdAIADChar->m_stAI, &pcsAITemplate->m_stAI))
			{
				OutputDebugString("AgpmAI::OnReceive() Error !!! - CopyAI() return FALSE \n");
			}
		}
		else
		{
			OutputDebugString("AgpmAI::OnReceive() Error !!! - GetAITemplate() return NULL \n");
		}
	}

	if (!pcsCharacter->m_Mutex.Release())
		return FALSE;

	return TRUE;
}