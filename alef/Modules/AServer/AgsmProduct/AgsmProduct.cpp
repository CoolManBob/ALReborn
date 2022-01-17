/*====================================================================

	AgsmProduct.cpp

====================================================================*/

#include "AgsmProduct.h"
#include "AgsaProduct.h"




/****************************************************/
/*		The Implementation of AgsmProduct class		*/
/****************************************************/
//
AgsmProduct::AgsmProduct()
	{
	SetModuleName("AgsmProduct");
	SetModuleType(APMODULE_TYPE_SERVER);
	
	EnableIdle(FALSE);
	SetModuleData(sizeof(AgsdProductItemSet), AGSMPRODUCT_DATATYPE_ITEM_SET);

	m_pAgpmCharacter	= NULL;
	m_pAgpmFactors		= NULL;
	m_pAgpmSkill		= NULL;
	m_pAgpmProduct		= NULL;
	m_pAgsmCharacter	= NULL;
	m_pAgsmItem			= NULL;
	m_pAgsmItemManager	= NULL;
	m_pAgsmSkill		= NULL;
	m_pAgpmTitle		= NULL;
	}


AgsmProduct::~AgsmProduct()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgsmProduct::OnAddModule()
	{
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmFactors		= (AgpmFactors *) GetModule("AgpmFactors");
	m_pAgpmSkill		= (AgpmSkill *) GetModule("AgpmSkill");
	m_pAgpmProduct		= (AgpmProduct *) GetModule("AgpmProduct");
	m_pAgsmCharacter	= (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pAgsmItem			= (AgsmItem *) GetModule("AgsmItem");
	m_pAgsmItemManager	= (AgsmItemManager *) GetModule("AgsmItemManager");
	m_pAgsmSkill		= (AgsmSkill *) GetModule("AgsmSkill");
	

	if (!m_pAgpmCharacter || !m_pAgpmFactors || !m_pAgpmSkill ||
		!m_pAgpmProduct || !m_pAgsmCharacter || !m_pAgsmItem ||
		!m_pAgsmItemManager || !m_pAgsmSkill
	    )
		return FALSE;

	if (!m_pAgsmCharacter->SetCallbackSendCharacterAllInfo(CBSendCharacterAllInfo, this))
		return FALSE;

	if (!m_pAgpmProduct->SetCallbackCompose(CBCompose, this))
		return FALSE;

	if (!m_pAgsmSkill->SetCallbackParseSkillComposeString(CBDecodeCompose, this))
		return FALSE;

	if (!m_pAgsmSkill->SetCallbackProductSkill(CBGatherPrepare, this))
		return FALSE;

	if (!m_pAgpmProduct->SetCallbackGather(CBGather, this))
		return FALSE;

	return TRUE;
	}


BOOL AgsmProduct::OnInit()
	{
	if (!m_csAdminItemSet.InitializeObject(sizeof(AgsdProductItemSet *), m_csAdminItemSet.GetCount()))
		return FALSE;

	m_pAgpmTitle = (AgpmTitle *) GetModule("AgpmTitle");

	return TRUE;
	}


BOOL AgsmProduct::OnIdle(UINT32 ulClockCount)
	{
	return TRUE;
	}


BOOL AgsmProduct::OnDestroy()
	{
	INT32 lIndex = 0;
	AgsdProductItemSet **ppAgsdProductItemSet = NULL;

	for (ppAgsdProductItemSet = (AgsdProductItemSet **) m_csAdminItemSet.GetObjectSequence(&lIndex);
		 ppAgsdProductItemSet; 
		 ppAgsdProductItemSet = (AgsdProductItemSet **) m_csAdminItemSet.GetObjectSequence(&lIndex))
		{
		DestroyModuleData(*ppAgsdProductItemSet, AGSMPRODUCT_DATATYPE_ITEM_SET);
		}

	m_csAdminItemSet.Reset();

	return TRUE;
	}




//	Admin
//=========================================
//
BOOL AgsmProduct::SetMaxItemSet(INT32 lCount)
	{
	return m_csAdminItemSet.SetCount(lCount);
	}


AgsdProductItemSet* AgsmProduct::GetItemSet(INT32 lID)
	{
	return m_csAdminItemSet.Get(lID);
	}




//	Stream
//===================================
//
BOOL AgsmProduct::StreamReadProductItemSet(CHAR *pszFile, BOOL bDecryption)
	{
	if (!pszFile || !strlen(pszFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;
	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
		return FALSE;

	AgsdProductItemSet *pTemp = NULL;
	INT32 lSetID = 0;
	INT32 lLevel = 0;
	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)
		{
		AgsdProductItemSet *pAgsdProductItemSet = NULL;

		// if not exist create else add to previous one.
		CHAR *psz = csExcelTxtLib.GetData(AGSMPRODUCT_ITEM_SET_EXCEL_COLUMN_SET_ID, lRow);
		if (psz && strlen(psz) > 0)
			{
			lSetID = atoi(psz);
			lLevel = 0;				// new set id, initialize skill level.
			}
		
		if (0 == lSetID)
			continue;
		
		pAgsdProductItemSet = m_csAdminItemSet.Get(lSetID);
		if (!pAgsdProductItemSet)
			{
			// create
			pTemp = (AgsdProductItemSet *) CreateModuleData(AGSMPRODUCT_DATATYPE_ITEM_SET);
			if (!pTemp)
				return FALSE;

			pTemp->m_lID = lSetID;
			pTemp->m_LevelSet.MemSetAll();
			m_csAdminItemSet.Add(pTemp);
			pAgsdProductItemSet = pTemp;
			}

		// level
		psz = csExcelTxtLib.GetData(AGSMPRODUCT_ITEM_SET_EXCEL_COLUMN_SKILL_LEVEL, lRow);
		if (psz && strlen(psz) > 0)
			{
			lLevel = atoi(psz);
			}
	
		INT32 lLevelIndex = lLevel - 1;
		if (0 > lLevelIndex || lLevelIndex >= AGPMPRODUCT_MAX_LEVEL)
			continue;

		// item element
		AgsdProductItemElement ProductItem;

		// item tid
		psz = csExcelTxtLib.GetData(AGSMPRODUCT_ITEM_SET_EXCEL_COLUMN_ITEM_TID, lRow);
		if (!psz || !strlen(psz))
			continue;
		ProductItem.m_lItemTID = atoi(psz);	
		
		// probability
		psz = csExcelTxtLib.GetData(AGSMPRODUCT_ITEM_SET_EXCEL_COLUMN_PROB, lRow);
		if (!psz || !strlen(psz))
			return FALSE;

		// accmulate previous values.
		// eg) 20, 30, 40, 10	=>	20, 50, 90, 100
		INT32 lProb = atoi(psz);
		if (0 < pAgsdProductItemSet->m_LevelSet[lLevelIndex].size())
			ProductItem.m_lRange = pAgsdProductItemSet->m_LevelSet[lLevelIndex][pAgsdProductItemSet->m_LevelSet[lLevelIndex].size() - 1].m_lRange + lProb;
		else
			ProductItem.m_lRange = lProb;

		pAgsdProductItemSet->m_LevelSet[lLevelIndex].push_back(ProductItem);
		}

  #ifdef _DEBUG
	// assert that the sum of all item prob. in a item set is 100%
	INT32 lIndex = 0;
	AgsdProductItemSet **ppAgsdProductItemSet = NULL;

	for (ppAgsdProductItemSet = (AgsdProductItemSet **) m_csAdminItemSet.GetObjectSequence(&lIndex);
		 ppAgsdProductItemSet; 
		 ppAgsdProductItemSet = (AgsdProductItemSet **) m_csAdminItemSet.GetObjectSequence(&lIndex))
		{
		for (INT32 i = 0; i < AGPMPRODUCT_MAX_LEVEL; i++)
			ASSERT((100 == (*ppAgsdProductItemSet)->m_LevelSet[i][(*ppAgsdProductItemSet)->m_LevelSet[i].size()-1].m_lRange));
		}
  #endif

	return TRUE;
	}




//	Callbacks
//============================================
//
BOOL AgsmProduct::CBCompose(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;
		
	AgsmProduct		*pThis = (AgsmProduct *) pClass;
	PVOID			*ppvBuffer = (PVOID *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;

	return pThis->OnCompose(pAgpdCharacter, (AgpdSkill *) ppvBuffer[0],
						    (AgpdComposeTemplate *) ppvBuffer[1], (AgpdItem *) ppvBuffer[2]);
	}


BOOL AgsmProduct::CBGatherPrepare(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmProduct		*pThis	= (AgsmProduct *) pClass;
	AgpdSkill		*pAgpdSkill = (AgpdSkill *) ((PVOID *) pData)[0];
	ApBase			*pTarget = (ApBase *) ((PVOID *) pData)[1];
	BOOL			*pbResult = (BOOL *) pCustData;

	if (!pAgpdSkill || !pTarget)
		return FALSE;

	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pAgpdSkill->m_pcsBase;
	*pbResult = pThis->OnGatherPrepare(pAgpdCharacter, pAgpdSkill, pTarget);

	return TRUE;
	}


BOOL AgsmProduct::CBGather(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmProduct		*pThis	= (AgsmProduct *) pClass;
	AgpdSkill		*pAgpdSkill = (AgpdSkill *) ((PVOID *) pData)[0];
	ApBase			*pTarget = (ApBase *) ((PVOID *) pData)[1];
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;

	return pThis->OnGather(pAgpdCharacter, pAgpdSkill, pTarget);
	}


BOOL AgsmProduct::CBDecodeCompose(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmProduct		*pThis			= (AgsmProduct *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *) pData;
	CHAR			*pszCompose		= (CHAR *) pCustData;

	return pThis->DecodeCompose(pAgpdCharacter, pszCompose);
	}


BOOL AgsmProduct::CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmProduct		*pThis			= (AgsmProduct *)	pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	return pThis->SendPacketComposeAll(pAgpdCharacter, ulNID);
	}
	



//	Operation (after callback)
//====================================================
//
BOOL AgsmProduct::OnCompose(AgpdCharacter *pAgpdCharacter, AgpdSkill *pAgpdSkill,
						    AgpdComposeTemplate *pAgpdComposeTemplate, AgpdItem *pReceipe)
	{
	if (!pAgpdCharacter || !pAgpdSkill || !pAgpdComposeTemplate)
		return FALSE;

	// validation
	INT32 lResult = AGPMPRODUCT_RESULT_FAIL;
	if (!m_pAgpmProduct->IsValidStatus(pAgpdCharacter, pAgpdComposeTemplate, pAgpdSkill, &lResult))
		{
		return SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
		}

	// success probability
	INT32 lSuccessProb = pAgpdComposeTemplate->m_lSuccessProb;

	if(m_pAgpmTitle)
	{
		INT32 nEffectSet = 0;
		INT32 nEffectValue1 = 0;
		INT32 nEffectValue2 = 0;

		if(pAgpdComposeTemplate->m_eCategory == AGPMPRODUCT_CATEGORY_ALCHEMY)
		{
			m_pAgpmTitle->GetEffectValue(pAgpdCharacter, AGPMTITLE_TITLE_EFFECT_ALCHEMIST_BONUS_RATE, &nEffectSet, &nEffectValue1, &nEffectValue2);
			lSuccessProb += nEffectValue1;
		}
		else if(pAgpdComposeTemplate->m_eCategory == AGPMPRODUCT_CATEGORY_COOK && pAgpdComposeTemplate->m_lNeedTitleTid != 0)
		{
			if(!pAgpdCharacter->m_csTitle || !pAgpdCharacter->m_csTitle->IsUsingTitle(pAgpdComposeTemplate->m_lNeedTitleTid))
				return FALSE;
		}
	}

	// delete source items
	if (!DeleteComposeSourceItem(pAgpdCharacter, pAgpdComposeTemplate, pReceipe))
		return FALSE;
	
	// additive probability when uses receipe
	if (pReceipe)
		lSuccessProb += pAgpdComposeTemplate->m_lAdditionalProb;

	// additive probability
	INT32 lSkillLevel = m_pAgpmSkill->GetSkillLevel(pAgpdSkill);
	if(lSkillLevel < 1 ||  lSkillLevel > AGPMPRODUCT_MAX_LEVEL)
		return FALSE;
	lSkillLevel = min(lSkillLevel, AGPMPRODUCT_MAX_LEVEL);
	AgpdProductSkillFactor *pAgpdProductSkillFactor = m_pAgpmProduct->GetProductSkillFactor(((AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate)->m_szName);
	if (pAgpdProductSkillFactor)
		lSuccessProb += pAgpdProductSkillFactor->m_lFactors[lSkillLevel][AGPMPRODUCT_SKILL_FACTOR_ADD_PROB];
	
	// evaluate
	if (lSuccessProb < m_csRandom.randInt(100))
		{
		lResult = AGPMPRODUCT_RESULT_FAIL;
		return SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
		}

	// success
	lResult = AGPMPRODUCT_RESULT_SUCCESS;

	// give skill experience.
	INT32 lExp = m_pAgpmProduct->AddSkillExp(pAgpdSkill);
	
	// update DB
	if (lExp > 0)
		UpdateDB(pAgpdCharacter);
	

	// give item.
	AgpdItemADChar* pAgpdItemADChar = m_pAgpmProduct->m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	if (!pAgpdItemADChar)
		return FALSE;

	if (m_pAgpmProduct->m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid))
		{
		lResult = AGPMPRODUCT_RESULT_FAIL_INVENTORY_FULL;
		return SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, pAgpdComposeTemplate->m_lComposeID, 0);
		}

	INT32 lItemCount = 1;
	AgpdItem* pAgpdItem = m_pAgsmItemManager->CreateItem(pAgpdComposeTemplate->m_lComposeID, pAgpdCharacter, lItemCount);
	if(!pAgpdItem)
		return FALSE;

	pAgpdItem->m_eStatus = AGPDITEM_STATUS_NOTSETTING;
	if (!m_pAgpmProduct->m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem))
	{
		m_pAgpmProduct->m_pAgpmItem->RemoveItem(pAgpdItem, TRUE);
		return FALSE;
	}

	// log
	m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_PDSKILL_REWARD, pAgpdCharacter->m_lID,
							pAgpdItem,
							1
							);	

	// send result.
	BOOL bResult = SendPakcetProductResult(pAgpdCharacter->m_lID,
										   pAgpdSkill->m_lID,
										   lResult,
										   pAgpdComposeTemplate->m_lComposeID,
										   lExp,
										   lItemCount
										   );

	return TRUE;
	}


BOOL AgsmProduct::OnGatherPrepare(AgpdCharacter *pAgpdCharacter, AgpdSkill *pAgpdSkill, ApBase *pTarget)
	{
	if (!pAgpdCharacter || !pAgpdSkill || !pTarget)
		return FALSE;

	INT32 lResult;
	INT32 lSuccessProb;
	INT32 lItemSetID = 0;
	
	// validate
	if (!m_pAgpmProduct->IsValidStatus(pAgpdCharacter, pAgpdSkill, pTarget->m_eType, pTarget->m_lID, &lResult, &lSuccessProb, &lItemSetID))
		{
		if (AGPMPRODUCT_RESULT_FAIL_TARGET_TOO_FAR == lResult)
			{
			pAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
			pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_SKILL;
			pAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pAgpdSkill->m_csTargetBase.m_eType;
			pAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pAgpdSkill->m_csTargetBase.m_lID;
			pAgpdCharacter->m_stNextAction.m_lUserData[0] = pAgpdSkill->m_lID;
			pAgpdCharacter->m_stNextAction.m_lUserData[1] = (INT_PTR) pAgpdSkill->m_pcsBase;
			pAgpdCharacter->m_stNextAction.m_lUserData[2] = pAgpdSkill->m_pcsTemplate->m_lID;
			pAgpdCharacter->m_stNextAction.m_lUserData[3] = m_pAgpmSkill->GetSkillLevel(pAgpdSkill);
			pAgpdCharacter->m_stNextAction.m_lUserData[4] = FALSE;

			m_pAgpmCharacter->MoveCharacterFollow(pAgpdCharacter, (AgpdCharacter *) pTarget, 250, TRUE);	
			return FALSE;
			}
		
		// send result direct
		SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
		return FALSE;
		}

	// check target has product(remaining item, ...)
	AgpdGatherCharacterAD *pAgpdGatherCharacterAD = m_pAgpmProduct->GetGatherCharacterAD(pTarget);
	if (!pAgpdGatherCharacterAD || pAgpdGatherCharacterAD->m_bIsGathering)
		{
		lResult = AGPMPRODUCT_RESULT_FAIL_INVALID_TARGET;
		SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
		return FALSE;
		}

	// reserve
	AuAutoLock Lock1(pAgpdGatherCharacterAD->m_Mutex);
	if (!Lock1.Result()) return FALSE;

	pAgpdGatherCharacterAD->m_bIsGathering = TRUE;
	if (pAgpdGatherCharacterAD->m_nRemained <= 0)
		{
		pAgpdGatherCharacterAD->m_bIsGathering = FALSE;
		lResult = AGPMPRODUCT_RESULT_FAIL_NO_REMAINED_ITEM;
		SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
		return FALSE;
		}
	
	pAgpdGatherCharacterAD->m_lCharacterID = pAgpdCharacter->m_lID;
	--pAgpdGatherCharacterAD->m_nRemained;

	AgpdGatherCharacterAD *pAgpdGatherAttachData = m_pAgpmProduct->GetGatherCharacterAD(pAgpdCharacter);
	if (pAgpdGatherAttachData)
	{
		pAgpdGatherAttachData->m_lGatherTargetCID		= pTarget->m_lID;
		pAgpdGatherAttachData->m_ulStartGatherTimeMSec	= GetClockCount();

		m_pAgpmCharacter->SetActionBlockTime(pAgpdCharacter, AGPMPRODUCT_GATHER_DURATION_MSEC, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL);
	}

	// add 10 seconds to disappear
	m_pAgsmCharacter->AddReservedTimeForDestroy((AgpdCharacter *)pTarget, 10000);

	return TRUE;
	}


BOOL AgsmProduct::OnGather(AgpdCharacter *pAgpdCharacter, AgpdSkill *pAgpdSkill, ApBase *pTarget)
	{
	if (!pAgpdCharacter || !pAgpdSkill || !pTarget)
		return FALSE;

	INT32 lResult;
	INT32 lSuccessProb;
	INT32 lItemSetID;

	// validate
	if (!m_pAgpmProduct->IsValidStatus(pAgpdCharacter, pAgpdSkill, pTarget->m_eType, pTarget->m_lID, &lResult, &lSuccessProb, &lItemSetID))
		{
		// send result direct
		return SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
		}

	// validate duration
	AgpdGatherCharacterAD *pAgpdGatherAttachData = m_pAgpmProduct->GetGatherCharacterAD(pAgpdCharacter);
	if (pAgpdGatherAttachData)
		{
		if (pAgpdGatherAttachData->m_lGatherTargetCID == AP_INVALID_CID ||
			pAgpdGatherAttachData->m_ulStartGatherTimeMSec == 0 ||
			pAgpdGatherAttachData->m_lGatherTargetCID != pTarget->m_lID ||
			pAgpdGatherAttachData->m_ulStartGatherTimeMSec + AGPMPRODUCT_GATHER_DURATION_MSEC > GetClockCount())
			{
			pAgpdGatherAttachData->m_lGatherTargetCID		= AP_INVALID_CID;
			pAgpdGatherAttachData->m_ulStartGatherTimeMSec	= 0;

			lResult = AGPMPRODUCT_RESULT_FAIL;
			return SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
			}
		}

	// check target has product(remaining item, ...)
	AgpdGatherCharacterAD *pAgpdGatherCharacterAD = m_pAgpmProduct->GetGatherCharacterAD(pTarget);
	if (!pAgpdGatherCharacterAD)
		{
		lResult = AGPMPRODUCT_RESULT_FAIL;
		return SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
		}
	
	// it's not for me
	if (pAgpdGatherCharacterAD->m_lCharacterID != pAgpdCharacter->m_lID)
		{
		lResult = AGPMPRODUCT_RESULT_FAIL;
		return SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
		}	

	AuAutoLock Lock1(pAgpdGatherCharacterAD->m_Mutex);
	if (!Lock1.Result()) return FALSE;

	// additive probalility
	INT32 lSkillLevel = m_pAgpmSkill->GetSkillLevel(pAgpdSkill);
	if(lSkillLevel < 1 ||  lSkillLevel > AGPMPRODUCT_MAX_LEVEL)
		return FALSE;
	lSkillLevel = min(lSkillLevel, AGPMPRODUCT_MAX_LEVEL);
	AgpdProductSkillFactor *pAgpdProductSkillFactor = m_pAgpmProduct->GetProductSkillFactor(((AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate)->m_szName);
	if (pAgpdProductSkillFactor)
		lSuccessProb += pAgpdProductSkillFactor->m_lFactors[lSkillLevel][AGPMPRODUCT_SKILL_FACTOR_ADD_PROB];

	if(m_pAgpmTitle)
	{
		INT32 nEffectSet = 0;
		INT32 nEffectValue1 = 0;
		INT32 nEffectValue2 = 0;
		m_pAgpmTitle->GetEffectValue(pAgpdCharacter, AGPMTITLE_TITLE_EFFECT_GATHER_BONUS_RATE, &nEffectSet, &nEffectValue1, &nEffectValue2);
		lSuccessProb += nEffectValue1;
	}
	
	// evaluate
	if (lSuccessProb < m_csRandom.randInt(100))
		{
		pAgpdGatherCharacterAD->m_lCharacterID = AP_INVALID_CID;
		pAgpdGatherCharacterAD->m_bIsGathering = FALSE;
		
		lResult = AGPMPRODUCT_RESULT_FAIL;
		return SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
		}	

	// determine item to present
	INT32 lItemTID = EvaluateItem(lItemSetID, lSkillLevel);
	if (!lItemTID)
		{
		lResult = AGPMPRODUCT_RESULT_FAIL;
		return SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, 0, 0);
		}

	// success
	lResult = AGPMPRODUCT_RESULT_SUCCESS;

	// give skill experience.
	INT32 lExp = m_pAgpmProduct->AddSkillExp(pAgpdSkill);

	// update db
	if (lExp > 0)
		UpdateDB(pAgpdCharacter);

	// give item.
	AgpdItemADChar* pAgpdItemADChar = m_pAgpmProduct->m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	if (!pAgpdItemADChar)
		{
		pAgpdGatherCharacterAD->m_lCharacterID = AP_INVALID_CID;
		pAgpdGatherCharacterAD->m_bIsGathering = FALSE;
		return FALSE;
		}

	if (m_pAgpmProduct->m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid))
		{
		pAgpdGatherCharacterAD->m_lCharacterID = AP_INVALID_CID;
		pAgpdGatherCharacterAD->m_bIsGathering = FALSE;
		
		lResult = AGPMPRODUCT_RESULT_FAIL_INVENTORY_FULL;
		return SendPakcetProductResult(pAgpdCharacter->m_lID, pAgpdSkill->m_lID, lResult, lItemTID, 0);
		}

	INT32 nEffectSet = 0;
	INT32 nEffectValue1 = 0;
	INT32 nEffectValue2 = 0;

	if(m_pAgpmTitle)
		m_pAgpmTitle->GetEffectValue(pAgpdCharacter, AGPMTITLE_TITLE_EFFECT_GATHER_BONUS_ITEM, &nEffectSet, &nEffectValue1, &nEffectValue2);

	if(!(nEffectValue1 != 0  && nEffectValue1 == ((AgpdCharacter *)pTarget)->m_pcsCharacterTemplate->m_lID))
		nEffectValue2 = 1;

	AgpdItem* pAgpdItem = m_pAgsmItemManager->CreateItem(lItemTID, pAgpdCharacter, nEffectValue2);
	if(!pAgpdItem)
		return FALSE;

	pAgpdItem->m_eStatus = AGPDITEM_STATUS_NOTSETTING;
	if (!m_pAgpmProduct->m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem))
		{
		m_pAgpmProduct->m_pAgpmItem->RemoveItem(pAgpdItem, TRUE);
		return FALSE;
		}

	pAgpdGatherCharacterAD->m_lCharacterID = AP_INVALID_CID;
	pAgpdGatherCharacterAD->m_bIsGathering = FALSE;

	// log
	m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_PDSKILL_REWARD, pAgpdCharacter->m_lID,
							pAgpdItem,
							nEffectValue2
							);	

	// send result.
	BOOL bResult = SendPakcetProductResult(pAgpdCharacter->m_lID,
										   pAgpdSkill->m_lID,
										   lResult,
										   lItemTID,
										   lExp,
										   nEffectValue2
										   );
	
	return TRUE;
	}




//	Send Packet
//===============================================
//
BOOL AgsmProduct::SendPakcetProductResult(INT32 lCID, INT32 lSkillID, INT32 eResult, INT32 lItemTID, INT32 lExp, INT32 lItemCount)
	{
	INT16	nPacketLength = 0;
	INT8	cOperation = AGPMPRODUCT_PACKET_OPERATION_RESULT;

	PVOID pvPacket = m_pAgpmProduct->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRODUCT_PACKET_TYPE,
														   &cOperation,
														   &lCID,
														   &lSkillID,
														   &lItemTID,
														   &lItemCount,
														   &eResult,
														   &lExp,
														   NULL
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmCharacter->GetCharDPNID(lCID));
	m_pAgpmProduct->m_csPacket.FreePacket(pvPacket);

	return bResult;	
	}


BOOL AgsmProduct::SendPacketComposeAll(AgpdCharacter *pAgpdCharacter, UINT32 ulNID)
	{
	AgpdComposeCharacterAD *pAgpdComposeCharacterAD = m_pAgpmProduct->GetComposeCharacterAD(pAgpdCharacter);
	if (!pAgpdComposeCharacterAD)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGPMPRODUCT_PACKET_OPERATION_COMPOSE_INFO;
	INT16	nLength = (INT16)pAgpdComposeCharacterAD->m_lCount * sizeof(INT32);

	PVOID pvPacket = m_pAgpmProduct->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRODUCT_PACKET_TYPE,
														   &cOperation,
														   &pAgpdCharacter->m_lID,
														   NULL,
														   NULL,
														   NULL,
														   NULL,
														   NULL,
														   &(pAgpdComposeCharacterAD->m_OwnComposes[0]),
														   &nLength
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pAgpmProduct->m_csPacket.FreePacket(pvPacket);
		
	return TRUE;
	}


#define _TEXTEND	_T('\0')
#define _DELIM		_T(':')

BOOL AgsmProduct::EncodeCompose(AgpdCharacter *pAgpdCharacter, CHAR *pszCompose, INT32 lBufferSize)
	{
	if (!pAgpdCharacter || !pszCompose || 0 >= lBufferSize)
		return FALSE;

	ZeroMemory(pszCompose, lBufferSize);

	// get compose AD
	AgpdComposeCharacterAD *pAgpdComposeCharacterAD = m_pAgpmProduct->GetComposeCharacterAD(pAgpdCharacter);
	if (pAgpdComposeCharacterAD)
		{
		CHAR *psz = pszCompose;
		
		// check buffer size. compose(4) + delimiter(1)
		ASSERT(lBufferSize > (pAgpdComposeCharacterAD->m_lCount * 5));
		for (INT32 i=0; i<pAgpdComposeCharacterAD->m_lCount; ++i)
			{
			psz += sprintf(psz, _T("%d%c"), pAgpdComposeCharacterAD->m_OwnComposes[i], _DELIM);
			}
		}

	return TRUE;
	}


BOOL AgsmProduct::DecodeCompose(AgpdCharacter *pAgpdCharacter, CHAR *pszCompose)
	{
	if (!pAgpdCharacter || !pszCompose)
		return FALSE;

	// get compose AD
	AgpdComposeCharacterAD *pAgpdComposeCharacterAD = m_pAgpmProduct->GetComposeCharacterAD(pAgpdCharacter);
	if (!pAgpdComposeCharacterAD)
		return FALSE;
	
	CHAR sz[10];
	CHAR *psz = sz;
	for (;_TEXTEND != *pszCompose; pszCompose++)
		{
		if (_DELIM != *pszCompose)
			*psz++ = *pszCompose;
		else
			{
			*psz = _TEXTEND;
			INT32 lComposeID = atoi(sz);
			if (0 != lComposeID)
				pAgpdComposeCharacterAD->m_OwnComposes[pAgpdComposeCharacterAD->m_lCount++] = lComposeID;
			psz = sz;
			}
		}

	return TRUE;
	}

#undef _TEXTEND
#undef _DELIM




//	DB Operation
//=============================================
//
BOOL AgsmProduct::UpdateDB(AgpdCharacter *pAgpdCharacter)
	{
	return EnumCallback(AGSMPRODUCT_CB_UPDATE_DB, pAgpdCharacter, NULL);
	}
	
	
BOOL AgsmProduct::SetCallbackUpdateDB(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMPRODUCT_CB_UPDATE_DB, pfCallback, pClass);
	}




//	Helper
//==============================================
//
INT32 AgsmProduct::EvaluateItem(INT32 lItemSetID, INT32 lLevel)
	{
	AgsdProductItemSet *pAgsdProductItemSet = GetItemSet(lItemSetID);
	if (!pAgsdProductItemSet)
		return 0;

	//lLevel = min(lLevel, 1);	// at least 1 level
	//lLevel = max(lLevel, AGPMPRODUCT_MAX_LEVEL);	// at most AGPMPRODUCT_MAX_LEVEL level
	INT32 lIndex = lLevel - 1;

	if (0 > lIndex || lIndex >= AGPMPRODUCT_MAX_LEVEL)		// out of range
		return 0;

	INT32 lRange = m_csRandom.randInt(100);
	for (ProductResultItemsIter Iter = pAgsdProductItemSet->m_LevelSet[lIndex].begin(); Iter != pAgsdProductItemSet->m_LevelSet[lIndex].end(); Iter++)
		{
		if (lRange <= Iter->m_lRange)
			return Iter->m_lItemTID;
		}

	return 0;
	}


BOOL AgsmProduct::DeleteComposeSourceItem(AgpdCharacter* pAgpdCharacter, AgpdComposeTemplate *pAgpdComposeTemplate, AgpdItem* pReceipe)
	{
	AgpdItemADChar* pAgpdItemADChar = m_pAgpmProduct->m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	if (!pAgpdItemADChar)
		return FALSE;

	// if use receipe, remove it
	if (pReceipe)
		{
		// log. 아이템이 사라질 수 있어 미리 남긴다.
		m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_PDSKILL_PAY, pAgpdCharacter->m_lID,
								pReceipe,
								1
								);
														
		m_pAgpmProduct->m_pAgpmItem->SubItemStackCount(pReceipe, 1);
		}

	// check item and quantity
	for (int i = 0; i < pAgpdComposeTemplate->m_lRequiredItems; ++i)
		{
		INT32 lIndex	= 0;
		INT32 lCount	= pAgpdComposeTemplate->m_RequiredItems[i].m_lCount;
		BOOL bFind		= FALSE;

		AgpdGridItem *pAgpdGridItem;
		for (pAgpdGridItem = m_pAgpmProduct->m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex);
			 pAgpdGridItem;
			 pAgpdGridItem = m_pAgpmProduct->m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex))
			{
			if (pAgpdComposeTemplate->m_RequiredItems[i].m_pItemTemplate->m_lID == pAgpdGridItem->m_lItemTID)
				{
				// TID matched
				bFind = TRUE;

				AgpdItem* pAgpdItem = m_pAgpmProduct->m_pAgpmItem->GetItem(pAgpdGridItem);
				if (!pAgpdItem)
					return FALSE;

				// is stackable?
				if (pAgpdComposeTemplate->m_RequiredItems[i].m_pItemTemplate->m_bStackable)
					{
					// if insufficient quantity, find another item of same TID.
					if (pAgpdItem->m_nCount >= lCount)
						{
						// log. 아이템이 사라질 수 있어 미리 남긴다.
						m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_PDSKILL_PAY, pAgpdCharacter->m_lID,
												pAgpdItem,
												lCount
												);
									
						m_pAgpmProduct->m_pAgpmItem->SubItemStackCount(pAgpdItem, lCount);
						lCount = 0;
						break;
						}
					else
						{
						lCount -= pAgpdItem->m_nCount;

						// log. 아이템이 사라질 수 있어 미리 남긴다.
						m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_PDSKILL_PAY, pAgpdCharacter->m_lID,
												pAgpdItem,
												pAgpdItem->m_nCount
												);

						m_pAgpmProduct->m_pAgpmItem->SubItemStackCount(pAgpdItem, pAgpdItem->m_nCount);
						}
					}
				else
					{
					// log. 아이템이 사라질 수 있어 미리 남긴다.
					m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_PDSKILL_PAY, pAgpdCharacter->m_lID,
											pAgpdItem,
											1
											);
																	
					// not stackable, remove item
					ZeroMemory(pAgpdItem->m_szDeleteReason, sizeof(pAgpdItem->m_szDeleteReason));
					strncpy(pAgpdItem->m_szDeleteReason, "Product material item", AGPMITEM_MAX_DELETE_REASON);
					if (!m_pAgpmProduct->m_pAgpmItem->RemoveItem(pAgpdItem, TRUE))
						return FALSE;
					
					break;
					}
				}
			}			// for loop - Item

		if (lCount || !bFind)
			return FALSE;
		}				// for loop - Product
	
	// pay
	//m_pAgpmCharacter->SubMoney(pAgpdCharacter, pAgpdComposeTemplate->m_lPrice);
	
	return TRUE;
	}


