/*===================================================================

	AgpmProduct.cpp

===================================================================*/

#include "AgpmProduct.h"
#include "AuExcelTxtLib.h"


CHAR* g_szProductCategory[AGPMPRODUCT_CATEGORY_MAX] =
{
	"Hunt",
	"Cook",
	"Alchemy"
};


/****************************************************/
/*		The Implementation of AgpmProduct class		*/
/****************************************************/
//
AgpmProduct::AgpmProduct()
	{
	SetModuleName("AgpmProduct");
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPMPRODUCT_PACKET_TYPE);	
	
	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_INT8,			1,	// Operation
							AUTYPE_INT32,			1,	// CID
							AUTYPE_INT32,			1,	// Skill ID
							AUTYPE_INT32,			1,	// Item TID/Target ID
							AUTYPE_INT32,			1,	// Receipe ID
							AUTYPE_INT32,			1,	// Result
							AUTYPE_INT32,			1,	// Result Exp.
							AUTYPE_MEMORY_BLOCK,	1,	// Compose Info
							AUTYPE_END,				0
							);

	SetModuleData(sizeof(AgpdProductSkillFactor), AGPMPRODUCT_DATATYPE_SKILL_FACTOR);
	SetModuleData(sizeof(AgpdComposeTemplate), AGPMPRODUCT_DATATYPE_COMPOSE_TEMPLATE);

	m_Category.MemSetAll();
	for (INT32 i=0; i<AGPMPRODUCT_CATEGORY_ARRAY_MAX; ++i)
		m_Category[i].m_eCategory = AGPMPRODUCT_CATEGORY_NONE;

	m_pAgpmItem			= NULL;
	m_pAgpmGrid			= NULL;
	m_pAgpmFactors		= NULL;
	m_pAgpmCharacter	= NULL;
	m_pAgpmSkill		= NULL;
	m_pApmEventManager	= NULL;

	EnableIdle(FALSE);
	}


AgpmProduct::~AgpmProduct()
	{
	}




//	ApModule inherited
//============================================
//
BOOL AgpmProduct::OnIdle(UINT32 ulClockCount)
	{
	return TRUE;
	}


BOOL AgpmProduct::OnAddModule()
	{
	m_pAgpmItem			= (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmGrid			= (AgpmGrid *) GetModule("AgpmGrid");
	m_pAgpmFactors		= (AgpmFactors *) GetModule("AgpmFactors");
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmSkill		= (AgpmSkill *) GetModule("AgpmSkill");
	m_pApmEventManager	= (ApmEventManager *) GetModule("ApmEventManager");
	
	if (!m_pAgpmItem || !m_pAgpmCharacter || 
		!m_pAgpmGrid || !m_pAgpmSkill || !m_pApmEventManager
		)
		return FALSE;

	m_nIndexGatherCharacterTAD = m_pAgpmCharacter->AttachCharacterTemplateData(this,
								 sizeof(AgpdGatherCharacterTAD), ConAgpdGatherCharacterTAD, DesAgpdGatherCharacterTAD);
	if (m_nIndexGatherCharacterTAD < 0)
		return FALSE;

	m_nIndexGatherCharacterAD = m_pAgpmCharacter->AttachCharacterData(this,
								 sizeof(AgpdGatherCharacterAD), ConAgpdGatherCharacterAD, DesAgpdGatherCharacterAD);
	if (m_nIndexGatherCharacterAD < 0)
		return FALSE;

	m_nIndexComposeCharacterAD = m_pAgpmCharacter->AttachCharacterData(this,
								 sizeof(AgpdComposeCharacterAD), ConAgpdComposeCharacterAD, DesAgpdComposeCharacterAD);
	if (m_nIndexComposeCharacterAD < 0)
		return FALSE;

	if (!m_pAgpmCharacter->SetCallbackResetMonster(CBResetMonster, this))
		return FALSE;

	return TRUE;
	}


BOOL AgpmProduct::OnInit()
	{
	if (!m_csAdminSkillFactor.InitializeObject(sizeof(AgpdProductSkillFactor *), m_csAdminSkillFactor.GetCount()))
		return FALSE;

	if (!m_csAdminComposeTemplate.InitializeObject(sizeof(AgpdComposeTemplate *), m_csAdminComposeTemplate.GetCount()))
		return FALSE;

	return TRUE;
	}


BOOL AgpmProduct::OnDestroy()
	{
	INT32 lIndex = 0;
	AgpdProductSkillFactor** ppAgpdProductSkillFactor = NULL;

	for (ppAgpdProductSkillFactor = (AgpdProductSkillFactor **) m_csAdminSkillFactor.GetObjectSequence(&lIndex); ppAgpdProductSkillFactor; 
		 ppAgpdProductSkillFactor = (AgpdProductSkillFactor **) m_csAdminSkillFactor.GetObjectSequence(&lIndex))
		{
		DestroyModuleData(*ppAgpdProductSkillFactor, AGPMPRODUCT_DATATYPE_SKILL_FACTOR);
		}

	m_csAdminSkillFactor.Reset();

	lIndex = 0;
	AgpdComposeTemplate** ppAgpdComposeTemplate = NULL;

	for (ppAgpdComposeTemplate = (AgpdComposeTemplate **) m_csAdminComposeTemplate.GetObjectSequence(&lIndex); ppAgpdComposeTemplate; 
		 ppAgpdComposeTemplate = (AgpdComposeTemplate **) m_csAdminComposeTemplate.GetObjectSequence(&lIndex))
		{
		DestroyModuleData(*ppAgpdComposeTemplate, AGPMPRODUCT_DATATYPE_COMPOSE_TEMPLATE);
		}

	m_csAdminComposeTemplate.Reset();

	return TRUE;
	}


BOOL AgpmProduct::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8		cOperation		= (-1);
	INT32		lCID			= 0;
	INT32		lSkillID		= 0;
	INT32		lID		= 0;
	INT32		lReceipeID		= 0;
	INT32		lResult			= 0;
	INT32		lExp			= 0;
	PVOID		pvComposeInfo	= NULL;
	INT16		nComposeInfo	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&lSkillID,
						&lID,
						&lReceipeID,
						&lResult,
						&lExp,
						&pvComposeInfo,
						&nComposeInfo
						);

	AgpdCharacter* pAgpdCharacter = m_pAgpmCharacter->GetCharacterLock(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
	{
		pAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (cOperation)
		{
		case AGPMPRODUCT_PACKET_OPERATION_COMPOSE:
			OnOperationCompose(pAgpdCharacter, lSkillID, lID, lReceipeID);
			break;

		case AGPMPRODUCT_PACKET_OPERATION_GATHER:
			OnOperationGather(pAgpdCharacter, lSkillID, lID);
			break;

		case AGPMPRODUCT_PACKET_OPERATION_RESULT:
			{
			PVOID pvBuffer[4];
			pvBuffer[0] = IntToPtr(lSkillID);		// Skill ID
			pvBuffer[1] = IntToPtr(lID);			// Result Item TID
			pvBuffer[2] = IntToPtr(lExp);			// Exp.
			pvBuffer[3] = IntToPtr(lReceipeID);		// Result item Count
			EnumCallback(AGPMPRODUCT_CB_RESULT, pvBuffer, &lResult);
			}
			break;
			
		case AGPMPRODUCT_PACKET_OPERATION_COMPOSE_INFO:
			OnOperationComposeInfo(pAgpdCharacter, pvComposeInfo, nComposeInfo);
			break;
			
		default:
			break;
		}

	pAgpdCharacter->m_Mutex.Release();
		
	return TRUE;
	}




//	Admin
//==========================================
//
BOOL AgpmProduct::SetMaxSkillFactor(INT32 lCount)
	{
	return m_csAdminSkillFactor.SetCount(lCount);
	}


BOOL AgpmProduct::SetMaxComposeTemplate(INT32 lCount)
	{
	return m_csAdminComposeTemplate.SetCount(lCount);
	}


AgpdProductSkillFactor* AgpmProduct::GetProductSkillFactor(CHAR *pszSkill)
	{
	return m_csAdminSkillFactor.Get(pszSkill);
	}


AgpdComposeTemplate* AgpmProduct::GetComposeTemplate(INT32 lComposeID)
	{
	return m_csAdminComposeTemplate.Get(lComposeID);
	}


INT32 AgpmProduct::GetComposeList(INT32 eCategory, INT32 lComposeList[], INT32 lBufferSize)
	{
	INT32 lCount = 0;
	INT32 lIndex = 0;
	AgpdComposeTemplate **ppAgpdComposeTemplate = NULL;

	while (ppAgpdComposeTemplate = (AgpdComposeTemplate **) m_csAdminComposeTemplate.GetObjectSequence(&lIndex))
		{
		AgpdComposeTemplate	*pAgpdComposeTemplate = *ppAgpdComposeTemplate;
		if (lBufferSize <= lCount)
			return (lBufferSize + 1);

		if (eCategory == pAgpdComposeTemplate->m_eCategory)
			lComposeList[lCount++] = pAgpdComposeTemplate->m_lComposeID;
		}

	return lCount;
	}


INT32 AgpmProduct::GetComposeList(AgpdCharacter *pAgpdCharacter, INT32 eCategory, INT32 lComposeList[], INT32 lBufferSize)
	{
	INT32 lCount = 0;
	INT32 lIndex = 0;
	AgpdComposeTemplate *pAgpdComposeTemplate = NULL;
	AgpdComposeCharacterAD *pAgpdComposeCharacterAD = GetComposeCharacterAD(pAgpdCharacter);
	if (!pAgpdComposeCharacterAD)
		return 0;

	for (INT32 i=0; i<pAgpdComposeCharacterAD->m_lCount; ++i)
		{
		if (lBufferSize <= lCount)
			return (lBufferSize + 1);
					
		pAgpdComposeTemplate = GetComposeTemplate(pAgpdComposeCharacterAD->m_OwnComposes[i]);
		if (pAgpdComposeTemplate && pAgpdComposeTemplate->m_eCategory == eCategory)
			{
			lComposeList[lCount++] = pAgpdComposeTemplate->m_lComposeID;
			}
		}
	
	return lCount;	
	}


INT32 AgpmProduct::GetSkillList(INT32 eCategory, INT32 lSkillList[], INT32 lBufferSize)
	{
	INT32 lCount = 0;
	INT32 lIndex = 0;

	// 
	for (INT32 i=0; i<AGPMPRODUCT_CATEGORY_ARRAY_MAX; ++i)
		{
		if (lBufferSize <= lCount)
			return (lBufferSize + 1);

		if (AGPMPRODUCT_CATEGORY_NONE != m_Category[i].m_eCategory &&
			eCategory == m_Category[i].m_eCategory)
			lSkillList[lCount++] = m_Category[i].m_lSkillTID;
		}

	return lCount;	
	
	}


BOOL AgpmProduct::IsComposeSkill(INT32 lSkillTID, INT32 *plCategory)
	{
	for (INT32 i=0; i<AGPMPRODUCT_CATEGORY_ARRAY_MAX; ++i)
		{
		if (m_Category[i].m_eCategory >= AGPMPRODUCT_CATEGORY_COOK && m_Category[i].m_lSkillTID == lSkillTID)
			{
			if (plCategory)
				*plCategory = m_Category[i].m_eCategory;
			return TRUE;
			}
		}

	return FALSE;	
	}




//	Attached data
//=====================================================
//
BOOL AgpmProduct::ConAgpdGatherCharacterTAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmProduct	*pThis		= (AgpmProduct *) pClass;
	ApBase		*pcsBase	= (ApBase *) pData;

	AgpdGatherCharacterTAD	*pAgpdGatherCharacterTAD = pThis->GetGatherCharacterTAD(pcsBase);
	if (pAgpdGatherCharacterTAD)
		{
		pAgpdGatherCharacterTAD->m_ClassElement.MemSetAll();
		}
	else
		return FALSE;

	return TRUE;
	}


BOOL AgpmProduct::DesAgpdGatherCharacterTAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	return TRUE;
	}


BOOL AgpmProduct::CBResetMonster(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmProduct	*pThis		= (AgpmProduct *) pClass;
	ApBase		*pcsBase	= (ApBase *) pData;

	pThis->InitGatherData(pcsBase);
	pThis->InitComposeData(pcsBase);

	return TRUE;
	}


BOOL AgpmProduct::InitGatherData(ApBase *pcsBase)
	{
	AgpdGatherCharacterAD	*pAgpdGatherCharacterAD = GetGatherCharacterAD(pcsBase);
	if (pAgpdGatherCharacterAD)
		{
		pAgpdGatherCharacterAD->m_bIsGathering = FALSE;
		pAgpdGatherCharacterAD->m_lCharacterID = AP_INVALID_CID;
		pAgpdGatherCharacterAD->m_nRemained = 1;			// hard-coded
		}
	else
		return FALSE;

	return TRUE;
	}


BOOL AgpmProduct::InitComposeData(ApBase *pcsBase)
	{
	AgpdComposeCharacterAD	*pAgpdComposeCharacterAD = GetComposeCharacterAD(pcsBase);
	if (pAgpdComposeCharacterAD)
		{
		pAgpdComposeCharacterAD->m_lCount = 0;
		pAgpdComposeCharacterAD->m_OwnComposes.MemSetAll();
		}
	else
		return FALSE;

	return TRUE;
	}


AgpdGatherCharacterTAD*	AgpmProduct::GetGatherCharacterTAD(ApBase *pcsBase)
	{
	if (!pcsBase || APBASE_TYPE_CHARACTER_TEMPLATE != pcsBase->m_eType)
		return NULL;

	return (AgpdGatherCharacterTAD *) m_pAgpmCharacter->GetAttachedModuleData(m_nIndexGatherCharacterTAD, (PVOID) pcsBase);
	}


BOOL AgpmProduct::ConAgpdGatherCharacterAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmProduct	*pThis		= (AgpmProduct *) pClass;
	ApBase		*pcsBase	= (ApBase *) pData;

	AgpdGatherCharacterAD	*pAgpdGatherCharacterAD = pThis->GetGatherCharacterAD(pcsBase);
	if (pAgpdGatherCharacterAD)
		{
		pAgpdGatherCharacterAD->m_Mutex.Init();
		pThis->InitGatherData(pcsBase);
		}
	else
		return FALSE;

	return TRUE;
	}


BOOL AgpmProduct::DesAgpdGatherCharacterAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmProduct	*pThis		= (AgpmProduct *) pClass;
	ApBase		*pcsBase	= (ApBase *) pData;

	AgpdGatherCharacterAD	*pAgpdGatherCharacterAD = pThis->GetGatherCharacterAD(pcsBase);
	if (pAgpdGatherCharacterAD)
		{
		pAgpdGatherCharacterAD->m_bIsGathering = FALSE;
		pAgpdGatherCharacterAD->m_lCharacterID = AP_INVALID_CID;
		pAgpdGatherCharacterAD->m_nRemained = 1;			// hard-coded		
		pAgpdGatherCharacterAD->m_Mutex.Destroy();
		}
	else
		return FALSE;

	return TRUE;
	}


AgpdGatherCharacterAD*	AgpmProduct::GetGatherCharacterAD(ApBase *pcsBase)
	{
	if (!pcsBase || APBASE_TYPE_CHARACTER != pcsBase->m_eType)
		return NULL;

	return (AgpdGatherCharacterAD *) m_pAgpmCharacter->GetAttachedModuleData(m_nIndexGatherCharacterAD, (PVOID) pcsBase);
	}


BOOL AgpmProduct::ConAgpdComposeCharacterAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmProduct	*pThis		= (AgpmProduct *) pClass;
	ApBase		*pcsBase	= (ApBase *) pData;

	AgpdComposeCharacterAD	*pAgpdComposeCharacterAD = pThis->GetComposeCharacterAD(pcsBase);
	if (pAgpdComposeCharacterAD)
		{
		pAgpdComposeCharacterAD->m_Mutex.Init();
		pThis->InitComposeData(pcsBase);
		}
	else
		return FALSE;

	return TRUE;	
	}

	
BOOL AgpmProduct::DesAgpdComposeCharacterAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmProduct	*pThis		= (AgpmProduct *) pClass;
	ApBase		*pcsBase	= (ApBase *) pData;

	AgpdComposeCharacterAD	*pAgpdComposeCharacterAD = pThis->GetComposeCharacterAD(pcsBase);
	if (pAgpdComposeCharacterAD)
		{
		pAgpdComposeCharacterAD->m_lCount = 0;
		pAgpdComposeCharacterAD->m_OwnComposes.MemSetAll();
		pAgpdComposeCharacterAD->m_Mutex.Destroy();
		}
	else
		return FALSE;

	return TRUE;		
	}

	
AgpdComposeCharacterAD* AgpmProduct::GetComposeCharacterAD(ApBase *pcsBase)
	{
	if (!pcsBase || APBASE_TYPE_CHARACTER != pcsBase->m_eType)
		return NULL;

	return (AgpdComposeCharacterAD *) m_pAgpmCharacter->GetAttachedModuleData(m_nIndexComposeCharacterAD, (PVOID) pcsBase);
	}




//	Compose Validation
//==========================================
//
BOOL AgpmProduct::IsOwnCompose(AgpdCharacter *pAgpdCharacter, AgpdComposeTemplate *pAgpdComposeTemplate)
	{
	if (!pAgpdCharacter || !pAgpdComposeTemplate)
		return FALSE;
		
	AgpdComposeCharacterAD *pAgpdComposeCharacterAD = GetComposeCharacterAD(pAgpdCharacter);
	if (!pAgpdComposeCharacterAD)
		return FALSE;
		
	for (INT32 i=0; i<pAgpdComposeCharacterAD->m_lCount; ++i)
		{
		if (pAgpdComposeCharacterAD->m_OwnComposes[i] == pAgpdComposeTemplate->m_lComposeID)
			return TRUE;
		}
		
	return FALSE;
	}


BOOL AgpmProduct::IsSatisfyPrerequisites(AgpdCharacter *pAgpdCharacter, AgpdComposeTemplate *pAgpdComposeTemplate)
	{
	if (!pAgpdCharacter || !pAgpdComposeTemplate)
		return FALSE;
		
	INT32 lSkillList[10];
	INT32 lSkill = GetSkillList(pAgpdComposeTemplate->m_eCategory, lSkillList, 10);

	for (INT i=0; i<lSkill; ++i)
		{
		AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkillByTID(pAgpdCharacter, lSkillList[i]);
		if (!pAgpdSkill)
			{
			return FALSE;
			}
		}
		
	return TRUE;
	}


BOOL AgpmProduct::IsValidStatus(AgpdCharacter* pAgpdCharacter, AgpdComposeTemplate* pAgpdComposeTemplate,
								AgpdSkill *pAgpdSkill, INT32 *plResult)
	{
	// check inventory full
	AgpdItemADChar* pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	if (m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid))
		return FALSE;
	
	INT32 lResult = AGPMPRODUCT_RESULT_SUCCESS;
	int i = 0;
	
	if (!IsOwnCompose(pAgpdCharacter, pAgpdComposeTemplate))
		{
		lResult = AGPMPRODUCT_RESULT_FAIL_HAS_NO_SKILL;
		goto my_result1;
		}
	
	// check required item and quantity
	for (i = 0; i < pAgpdComposeTemplate->m_lRequiredItems; ++i)
		{
		if (!IsValidItemStatus(pAgpdCharacter, pAgpdComposeTemplate->m_RequiredItems[i].m_pItemTemplate,
			 pAgpdComposeTemplate->m_RequiredItems[i].m_lCount))
			{
			lResult = AGPMPRODUCT_RESULT_FAIL_INSUFFICIENT_SOURCE_ITEM;
			goto my_result1;
			}
		}

	// check skill level
	if (!IsValidComposeLevel(pAgpdCharacter, pAgpdComposeTemplate, pAgpdSkill))
		{
		lResult = AGPMPRODUCT_RESULT_FAIL_INSUFFICIENT_LEVEL;
		goto my_result1;
		}

	// check price
	/*
	if (pAgpdCharacter->m_llMoney < pAgpdComposeTemplate->m_lPrice)
		{
		lResult = AGPMPRODUCT_RESULT_FAIL_INSUFFICIENT_MONEY;
		goto my_result1;
		}
	*/

  my_result1:
	if (plResult)
		*plResult = lResult;

	return (AGPMPRODUCT_RESULT_SUCCESS == lResult);
	}


BOOL AgpmProduct::IsValidItemStatus(AgpdCharacter* pAgpdCharacter, AgpdItemTemplate* pItemTemplate, INT32 lCount)
	{
	if (!pItemTemplate)
		return FALSE;

	INT32	lIndex = 0;
	INT32	lStackCount = 0;	// for stackable item
	BOOL	bFound = FALSE;
	AgpdGridItem *pAgpdGridItem;
	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);

	for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex);
		 pAgpdGridItem;
		 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex))
		{
		// Check TID
		if (pItemTemplate->m_lID == pAgpdGridItem->m_lItemTID)
			{
			// Found
			bFound = TRUE;

			if (!pItemTemplate->m_bStackable)
				return TRUE;

			// Stackable
			AgpdItem* pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem);
			if (!pAgpdItem)
				return FALSE;

			lStackCount += pAgpdItem->m_nCount;
			}
		}

	// not found or found stackable but insufficient
	if (!bFound || (0 == lStackCount) || (lCount > lStackCount))
		return FALSE;

	return TRUE;
	}


BOOL AgpmProduct::IsValidComposeLevel(AgpdCharacter *pAgpdCharacter, AgpdComposeTemplate *pAgpdComposeTemplate, AgpdSkill *pAgpdSkill)
	{
	if (!pAgpdComposeTemplate)
		return FALSE;
	
	if (pAgpdSkill)
		{
		if (0 >= pAgpdComposeTemplate->m_lLevel)
			return TRUE;

		INT32 lLevel = m_pAgpmSkill->GetSkillLevel(pAgpdSkill);
		return (lLevel >= pAgpdComposeTemplate->m_lLevel);
		}

	else
		{
		INT32 lSkillList[10];
		INT32 lSkill = GetSkillList(pAgpdComposeTemplate->m_eCategory, lSkillList, 10);

		for (INT i=0; i<lSkill; ++i)
			{
			AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkillByTID(pAgpdCharacter, lSkillList[i]);
			if (!pAgpdSkill)
				return FALSE;

			INT32 lLevel = m_pAgpmSkill->GetSkillLevel(pAgpdSkill);
			if (lLevel < pAgpdComposeTemplate->m_lLevel)
				return FALSE;
			}
		
		return TRUE;
		}
	}




//	Gather Validation
//==================================================
//
BOOL AgpmProduct::IsValidStatus(AgpdCharacter *pAgpdCharacter, AgpdSkill *pAgpdSkill, ApBaseType eBaseType, INT32 lTargetID,
								INT32 *plResult, INT32 *plSuccessProb, INT32 *plItemSetID)
	{
	if (!pAgpdCharacter || !pAgpdSkill || !pAgpdSkill->m_pcsTemplate)
		return FALSE;

	INT32 lResult = AGPMPRODUCT_RESULT_SUCCESS;
	AgpdSkillTemplate *pAgpdSkillTemplate = (AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate;
	ApBase *pTarget = m_pApmEventManager->GetBase(eBaseType, lTargetID);
	INT32 lRange = 0;

	// requirements(item, ...)
	if (!m_pAgpmSkill->IsSatisfyRequirement(pAgpdSkill))
		{
		lResult = AGPMPRODUCT_RESULT_FAIL_NO_TOOL;
		goto my_result2;
		}

	// target type & status
	if (!CheckTargetStatus(pTarget))
		{
		lResult = AGPMPRODUCT_RESULT_FAIL_INVALID_TARGET;
		goto my_result2;
		}

	// skill applicable
	INT32 lClass = (INT32) AUCHARCLASS_TYPE_NONE;
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);
	
	if (!CheckTargetGatherApplicable(pTarget, pAgpdSkillTemplate->m_lID, lClass, plSuccessProb, plItemSetID))
		{
		lResult = AGPMPRODUCT_RESULT_FAIL_INVALID_TARGET;
		goto my_result2;
		}

	// range
	lRange = AGPMPRODUCT_MAX_RANGE;//m_pAgpmSkill->GetRange(pAgpdSkill);
	if (lRange > 0)
		{
		AuPOS *pBasePos = m_pApmEventManager->GetBasePos(pAgpdSkill->m_pcsBase, NULL);
		AuPOS *pSourcePos = m_pApmEventManager->GetBasePos(pTarget, NULL);
		AuPOS pCalcPos;
		if (!m_pAgpmFactors->IsInRange(pBasePos, pSourcePos, lRange, 0, &pCalcPos))
			{
			lResult = AGPMPRODUCT_RESULT_FAIL_TARGET_TOO_FAR;
			goto my_result2;
			}
		}

  my_result2:
	if (plResult)
		*plResult = lResult;

	return (AGPMPRODUCT_RESULT_SUCCESS == lResult);
	}


BOOL AgpmProduct::CheckTargetStatus(ApBase *pTarget)
	{
	if (!pTarget)
		return FALSE;

	switch (pTarget->m_eType)
		{
		case APBASE_TYPE_CHARACTER :
			{
			AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pTarget;
			if (m_pAgpmCharacter->IsMonster(pAgpdCharacter) &&
				AGPDCHAR_STATUS_DEAD == pAgpdCharacter->m_unActionStatus &&
				m_pAgpmCharacter->IsTargetable((AgpdCharacter *) pTarget)
				)
				return TRUE;
			}
			break;

		//case APBASE_TYPE_OBJECT :
		default:
			break;
		}
	
	return FALSE;
	}


BOOL AgpmProduct::CheckTargetGatherApplicable(ApBase *pTarget, INT32 lSkillTID, INT32 lClass, INT32 *plSuccessProb, INT32 *plItemSetID)
	{
	if (plSuccessProb)
		*plSuccessProb = 0;
	if (plItemSetID)
		*plItemSetID = 0;

	if (APBASE_TYPE_CHARACTER != pTarget->m_eType || AUCHARCLASS_TYPE_NONE == lClass || lClass >= AUCHARCLASS_TYPE_MAX)
		return FALSE;

	AgpdGatherCharacterTAD *pAgpdGatherCharacterTAD = GetGatherCharacterTAD(((AgpdCharacter *) pTarget)->m_pcsCharacterTemplate);
	if (!pAgpdGatherCharacterTAD)
		return FALSE;

	if (pAgpdGatherCharacterTAD->m_ClassElement[lClass].m_lSkillTID == lSkillTID)
		{
		if (plSuccessProb)
			*plSuccessProb = pAgpdGatherCharacterTAD->m_ClassElement[lClass].m_lProbability;
		if (plItemSetID)
			*plItemSetID = pAgpdGatherCharacterTAD->m_ClassElement[lClass].m_lItemSetID;

		return TRUE;
		}

	return FALSE;
	}


BOOL AgpmProduct::CheckTargetHasProduct(ApBase *pTarget, INT32 lSkillTID)
	{
	AgpdGatherCharacterAD *pAgpdGatherCharacterAD = GetGatherCharacterAD(pTarget);
	if (!pAgpdGatherCharacterAD)
		return FALSE;

	if (pAgpdGatherCharacterAD->m_nRemained > 0)
		return TRUE;

	return FALSE;
	}




//	Value related
//==================================================
//
INT32 AgpmProduct::AddSkillExp(AgpdSkill *pAgpdSkill)
	{
	if (!pAgpdSkill || !pAgpdSkill->m_pcsTemplate)
		return 0;

	AgpdProductSkillFactor *pAgpdProductSkillFactor =
			GetProductSkillFactor(((AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate)->m_szName);
	// non-growable skill
	if (!pAgpdProductSkillFactor)
		return 0;

	// get current exp
	INT32 lSkillExp = m_pAgpmSkill->GetSkillExp(pAgpdSkill);
	if (lSkillExp < 0)
		lSkillExp = 0;
	if (lSkillExp > pAgpdProductSkillFactor->m_lFactors[AGPMPRODUCT_MAX_LEVEL][AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP])
		lSkillExp = pAgpdProductSkillFactor->m_lFactors[AGPMPRODUCT_MAX_LEVEL][AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP];

	// evaluate give exp
	INT32 lCurrLevel = m_pAgpmSkill->GetSkillLevel(pAgpdSkill);
	if(lCurrLevel < 1 || lCurrLevel > AGPMPRODUCT_MAX_LEVEL) 
		return 0;

	INT32 lAdd = 0;

	INT32 lDiff = pAgpdProductSkillFactor->m_lFactors[lCurrLevel][AGPMPRODUCT_SKILL_FACTOR_MAX_EXP]
				  - pAgpdProductSkillFactor->m_lFactors[lCurrLevel][AGPMPRODUCT_SKILL_FACTOR_MIN_EXP];
	INT32 lBonus = m_csRandom.randInt(lDiff);
	lAdd = pAgpdProductSkillFactor->m_lFactors[lCurrLevel][AGPMPRODUCT_SKILL_FACTOR_MIN_EXP] + lBonus;

	// add exp
	if (lSkillExp + lAdd > pAgpdProductSkillFactor->m_lFactors[AGPMPRODUCT_MAX_LEVEL][AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP])
		lAdd = pAgpdProductSkillFactor->m_lFactors[AGPMPRODUCT_MAX_LEVEL][AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP] - lSkillExp;
	
	lSkillExp += lAdd;
	
	m_pAgpmSkill->SetSkillExp(pAgpdSkill, lSkillExp);

	// check level up
	INT32 lNextLevel = GetLevelOfExp(pAgpdProductSkillFactor, lSkillExp, lCurrLevel);
	m_pAgpmSkill->UpdateSkillLevelAndExp(pAgpdSkill, lNextLevel, lSkillExp);
	
	return lAdd;
	}


INT32 AgpmProduct::GetLevelOfExp(AgpdSkill *pAgpdSkill, INT32 lExp)
	{
	if (!pAgpdSkill || !pAgpdSkill->m_pcsTemplate)
		return FALSE;

	AgpdProductSkillFactor *pAgpdProductSkillFactor = GetProductSkillFactor(((AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate)->m_szName);
	if (!pAgpdProductSkillFactor) 	// non-growable skill
		return 0;

	return GetLevelOfExp(pAgpdProductSkillFactor, lExp);
	}


INT32 AgpmProduct::GetLevelOfExp(AgpdProductSkillFactor *pAgpdProductSkillFactor, INT32 lExp, INT32 lCurrLevel)
	{
	// check level up
	INT32 lLevel = lCurrLevel;
	INT32 lMax = min(pAgpdProductSkillFactor->m_lMaxLevel, AGPMPRODUCT_MAX_LEVEL);
	for (; lLevel <= lMax; lLevel++)
		{
		// find level of given exp
		if (lExp <= pAgpdProductSkillFactor->m_lFactors[lLevel][AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP])
			break;
		}
	
	lLevel = min(AGPMPRODUCT_MAX_LEVEL, lLevel);
	return lLevel;
	}


INT32	AgpmProduct::GetExpOfCurrLevel(AgpdSkill *pAgpdSkill)
	{
	if (!pAgpdSkill || !pAgpdSkill->m_pcsTemplate)
		return 0;

	AgpdProductSkillFactor *pAgpdProductSkillFactor =
			GetProductSkillFactor(((AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate)->m_szName);
	// non-growable skill
	if (!pAgpdProductSkillFactor)
		return 0;

	// get current exp
	INT32 lSkillExp = m_pAgpmSkill->GetSkillExp(pAgpdSkill);
	if (lSkillExp < 0)
		lSkillExp = 0;

	INT32 lLevel = m_pAgpmSkill->GetSkillLevel(pAgpdSkill);
	if(lLevel < 1 || lLevel > AGPMPRODUCT_MAX_LEVEL)
		return 0;

	INT32 lExp = (lSkillExp - pAgpdProductSkillFactor->m_lFactors[lLevel-1][AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP]);
	
	return ((lExp > 0) ? lExp : 0);
	}


INT32 AgpmProduct::GetMaxExpOfCurrLevel(AgpdSkill *pAgpdSkill)
	{
	if (!pAgpdSkill || !pAgpdSkill->m_pcsTemplate)
		return 0;

	AgpdProductSkillFactor *pAgpdProductSkillFactor =
			GetProductSkillFactor(((AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate)->m_szName);
	// non-growable skill
	if (!pAgpdProductSkillFactor)
		return 0;

	INT32 lLevel = m_pAgpmSkill->GetSkillLevel(pAgpdSkill);
	if(lLevel < 1 || lLevel > AGPMPRODUCT_MAX_LEVEL)
		return 0;

	return pAgpdProductSkillFactor->m_lFactors[lLevel][AGPMPRODUCT_SKILL_FACTOR_LEVELUP_EXP];
	}


BOOL AgpmProduct::SetSkillLevel(AgpdCharacter *pAgpdCharacter, INT32 lSkillTID, INT32 lLevel)
	{
	if (0 >= lLevel)
		return FALSE;
	
	AgpdSkillTemplate *pAgpdSkillTemplate = m_pAgpmSkill->GetSkillTemplate(lSkillTID);
	if (!pAgpdSkillTemplate)
		return FALSE;
	
	AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkill((ApBase *) pAgpdCharacter, pAgpdSkillTemplate->m_szName);
	if (!pAgpdSkill)
		return FALSE;

	AgpdProductSkillFactor *pAgpdProductSkillFactor =
			GetProductSkillFactor(((AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate)->m_szName);
	// non-growable skill
	if (!pAgpdProductSkillFactor)
		return FALSE;

	INT32 lSkillExp = 10 + pAgpdProductSkillFactor->m_lFactors[lLevel-1][AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP];
	
	m_pAgpmSkill->SetSkillExp(pAgpdSkill, lSkillExp);

	// check level up
	INT32 lLevel2 = GetLevelOfExp(pAgpdSkill, lSkillExp);
	m_pAgpmSkill->UpdateSkillLevelAndExp(pAgpdSkill, lLevel2, lSkillExp);
	
	return TRUE;
	}


BOOL AgpmProduct::LearnCompose(AgpdCharacter *pAgpdCharacter, AgpdComposeTemplate *pAgpdComposeTemplate)
	{
	if (!pAgpdCharacter || !pAgpdComposeTemplate)
		return FALSE;
		
	if (!IsOwnCompose(pAgpdCharacter, pAgpdComposeTemplate))
		{
		AgpdComposeCharacterAD *pAgpdComposeCharacterAD = GetComposeCharacterAD(pAgpdCharacter);
		if (!pAgpdComposeCharacterAD)
			return FALSE;
		
		pAgpdComposeCharacterAD->m_OwnComposes[pAgpdComposeCharacterAD->m_lCount++] = pAgpdComposeTemplate->m_lComposeID;
		}
		
	return TRUE;
	}

	
BOOL AgpmProduct::LearnCompose(AgpdCharacter *pAgpdCharacter, INT32 lComposeID)
	{
	if (!pAgpdCharacter)
		return FALSE;
		
	AgpdComposeTemplate *pAgpdComposeTemplate = GetComposeTemplate(lComposeID);
	return LearnCompose(pAgpdCharacter, pAgpdComposeTemplate);
	}




//	Callback setting
//==================================================
//
BOOL AgpmProduct::SetCallbackCompose(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMPRODUCT_CB_COMPOSE, pfCallback, pClass);
	}


BOOL AgpmProduct::SetCallbackResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMPRODUCT_CB_RESULT, pfCallback, pClass);
	}


BOOL AgpmProduct::SetCallbackGather(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMPRODUCT_CB_GATHER, pfCallback, pClass);
	}


BOOL AgpmProduct::SetCallbackComposeInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMPRODUCT_CB_COMPOSE_INFO, pfCallback, pClass);
	}




//	Operation
//===============================================
//
BOOL AgpmProduct::OnOperationCompose(AgpdCharacter* pAgpdCharacter, INT32 lSkillID, INT32 lComposeID, INT32 lReceipeID)
	{
	if (!pAgpdCharacter)
		return FALSE;

	AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkill(lSkillID);
	if (!pAgpdSkill)
		return FALSE;

	AgpdComposeTemplate *pAgpdComposeTemplate = m_csAdminComposeTemplate.Get(lComposeID);
	if (!pAgpdComposeTemplate)
		return FALSE;

	AgpdItem *pAgpdItemReceipe = NULL;
	if (lReceipeID)
		{
		pAgpdItemReceipe = m_pAgpmItem->GetItem(lReceipeID);

		if (!pAgpdItemReceipe
			|| pAgpdItemReceipe->m_ulCID != pAgpdCharacter->m_lID
			|| AGPDITEM_STATUS_INVENTORY != pAgpdItemReceipe->m_eStatus)
			return FALSE;
		}

	PVOID pvBuffer[3];
	pvBuffer[0] = pAgpdSkill;
	pvBuffer[1] = pAgpdComposeTemplate;
	pvBuffer[2] = pAgpdItemReceipe;
	EnumCallback(AGPMPRODUCT_CB_COMPOSE, pvBuffer, pAgpdCharacter);

	return TRUE;
	}


BOOL AgpmProduct::OnOperationGather(AgpdCharacter* pAgpdCharacter, INT32 lSkillID, INT32 lTargetID)
	{
	if (!pAgpdCharacter)
		return FALSE;

	AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkill(lSkillID);
	if (!pAgpdSkill)
		return FALSE;
		
	if (AP_INVALID_CID == lTargetID)
		return FALSE;

	AgpdCharacter *pTarget = m_pAgpmCharacter->GetCharacterLock(lTargetID);	
	PVOID pvBuffer[2];
	pvBuffer[0] = pAgpdSkill;
	pvBuffer[1] = pTarget;

	EnumCallback(AGPMPRODUCT_CB_GATHER, pvBuffer, pAgpdCharacter);
	
	if (pTarget)
		pTarget->m_Mutex.Release();
	return TRUE;		
	}


BOOL AgpmProduct::OnOperationComposeInfo(AgpdCharacter *pAgpdCharacter, PVOID pvComposeInfo, INT16 nComposeInfo)
	{
	if (!pAgpdCharacter)
		return FALSE;
		
	AgpdComposeCharacterAD *pAgpdComposeCharacterAD = GetComposeCharacterAD(pAgpdCharacter);
	if (!pAgpdComposeCharacterAD)
		return FALSE;
	
	if (!pvComposeInfo || 0 == nComposeInfo)
		{
		pAgpdComposeCharacterAD->m_lCount = 0;
		pAgpdComposeCharacterAD->m_OwnComposes.MemSetAll();
		}
	else
		{
		pAgpdComposeCharacterAD->m_lCount = nComposeInfo / sizeof(INT32);
		pAgpdComposeCharacterAD->m_OwnComposes.MemCopy(0, (INT32 *)pvComposeInfo, nComposeInfo / sizeof(INT32));
		}
	
	return EnumCallback(AGPMPRODUCT_CB_COMPOSE_INFO, pAgpdComposeCharacterAD, pAgpdCharacter);
	}


BOOL AgpmProduct::IsActiveRequireTitle( int nComposeID, int nCurrTitleID )
{
	AgpdComposeTemplate* ppdComposeTemplate = GetComposeTemplate( nComposeID );
	if( !ppdComposeTemplate ) return FALSE;

	if( ppdComposeTemplate->m_lNeedTitleTid == 0 || ppdComposeTemplate->m_lNeedTitleTid == nCurrTitleID ) return TRUE;
	return FALSE;
}
