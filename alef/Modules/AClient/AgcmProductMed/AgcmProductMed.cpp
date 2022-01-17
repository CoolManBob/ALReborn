/*===================================================================

	AgcmProductMed.cpp

===================================================================*/

#include "AgcmProductMed.h"
#include "AuStrTable.h"


/********************************************************/
/*		The Implementation of AgcmProductMed class		*/
/********************************************************/
//
AgcmProductMed::AgcmProductMed()
	{
	SetModuleName("AgcmProductMed");
	SetModuleType(APMODULE_TYPE_CLIENT);

	EnableIdle(FALSE);

	m_pAgpmSkill = NULL;
	m_pAgcmSkill = NULL;
	m_pAgpmProduct = NULL;
	m_pAgcmCharacter = NULL;
	m_pAgcmProduct = NULL;
	m_pAgcmUIProduct = NULL;
	m_pAgcmUIProductSkill = NULL;
	m_pAgcmUICooldown = NULL;
	
	m_lCooldownID = AGCMUICOOLDOWN_INVALID_ID;
	m_pAgpdSkill = NULL;
	m_lTargetID = AP_INVALID_CID;
	//m_bHold = FALSE;
	}

AgcmProductMed::~AgcmProductMed()
	{
	}


//	ApModule inherited
//========================================
//
BOOL AgcmProductMed::OnAddModule()
	{
	m_pAgpmSkill = (AgpmSkill *) GetModule("AgpmSkill");
	m_pAgcmSkill = (AgcmSkill *) GetModule("AgcmSkill");
	m_pAgpmProduct = (AgpmProduct *) GetModule("AgpmProduct");
	m_pAgcmCharacter = (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pAgcmProduct = (AgcmProduct *) GetModule("AgcmProduct");
	m_pAgcmUIProduct = (AgcmUIProduct *) GetModule("AgcmUIProduct");
	m_pAgcmUIProductSkill = (AgcmUIProductSkill *) GetModule("AgcmUIProductSkill");
	m_pAgcmUICooldown = (AgcmUICooldown *) GetModule("AgcmUICooldown");

	if (!m_pAgpmSkill || !m_pAgcmSkill || !m_pAgpmProduct ||
		!m_pAgcmCharacter || !m_pAgcmProduct || !m_pAgcmUIProduct ||
		!m_pAgcmUIProductSkill
		)
		return FALSE;

	if (!m_pAgpmSkill->SetCallbackReceiveAction(CBActionSkillResult, this))
		return FALSE;

	if (!m_pAgcmSkill->SetCallbackPreProcessSkill(CBPreProcessSkill, this))
		return FALSE;

	if (!m_pAgpmProduct->SetCallbackResult(CBProductResult, this))
		return FALSE;

	if (m_pAgcmUICooldown)
		{
		m_lCooldownID = m_pAgcmUICooldown->RegisterCooldown(CBAfterCooldown, this);
		if (AGCMUICOOLDOWN_INVALID_ID == m_lCooldownID)
			return FALSE;
		}

	return TRUE;
	}


//	Callback
//=============================================
//
BOOL AgcmProductMed::CBPreProcessSkill(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmProductMed *pThis = (AgcmProductMed *) pClass;

	INT32 lSkillTID = (INT32) ((PVOID *) pData)[0];
	INT32 lTargetID = (INT32) ((PVOID *) pData)[1];
	BOOL bForceAttack = (BOOL) ((PVOID *) pData)[2];
	BOOL *pbProcessed	= (BOOL *) pCustData;

	AgpdSkill *pAgpdSkill = pThis->m_pAgpmSkill->GetSkillByTID( pThis->m_pAgcmCharacter->GetSelfCharacter(), lSkillTID );
	if (!pAgpdSkill || !pAgpdSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillTemplate *pAgpdSkillTemplate = (AgpdSkillTemplate *) pAgpdSkill->m_pcsTemplate;

	// is product skill?
	if (pAgpdSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
		{
		INT32 eCategory = AGPMPRODUCT_CATEGORY_NONE;
		CHAR sz[256];
		
		// compose(need UI)
		if (pThis->m_pAgpmProduct->IsComposeSkill(pAgpdSkillTemplate->m_lID, &eCategory))
			{
			// message
			sprintf(sz, ClientStr().GetStr(STI_USE_SKILL), pAgpdSkillTemplate->m_szName);
			pThis->m_pAgcmUIProduct->AddSystemMessage(sz);
			
			// invoke UI
			pThis->m_pAgcmUIProduct->Open(eCategory, pAgpdSkill->m_lID);
			*pbProcessed = TRUE;
			
			return TRUE;
			}
		
		// gather
		else
			{
			INT32 lResult;
			
			/*
			if (pThis->m_bHold)
				{
				//pThis->OnResult(lResult, lSkillID, 0, 0);
				*pbProcessed = TRUE;
				return TRUE;
				}
			*/
				
			// validation
			if (!pThis->m_pAgpmProduct->IsValidStatus(pThis->m_pAgcmCharacter->GetSelfCharacter(), pAgpdSkill, APBASE_TYPE_CHARACTER, lTargetID, &lResult))
				{
				// if range check failed, cast skill (server will move charcter to target position)
				if (AGPMPRODUCT_RESULT_FAIL_TARGET_TOO_FAR != lResult)
					{
					pThis->OnResult(lResult, lSkillTID, 0, 0, 0);
					*pbProcessed = TRUE;
					return TRUE;
					}
				}

			// message
			sprintf(sz, ClientStr().GetStr(STI_USE_SKILL), pAgpdSkillTemplate->m_szName);
			pThis->m_pAgcmUIProduct->AddSystemMessage(sz);
				
			// save
			pThis->m_pAgpdSkill = pAgpdSkill;
			pThis->m_lTargetID = lTargetID;
			}
		}

	return TRUE;
	}


BOOL AgcmProductMed::CBActionSkillResult(PVOID pData, PVOID pClass, PVOID pCustData)	
	{
	if (!pData || !pClass)
		return FALSE;
		
	AgcmProductMed			*pThis			= (AgcmProductMed *)				pClass;
	pstAgpmSkillActionData	pstActionData	= (pstAgpmSkillActionData)	pData;

	if (AGPMSKILL_ACTION_CAST_SKILL != pstActionData->nActionType)
		return FALSE;
	
	AgpdSkill *pAgpdSkill = pThis->m_pAgpmSkill->GetSkill(pstActionData->lSkillID);
	if (!pAgpdSkill || !pAgpdSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillTemplate *pAgpdSkillTemplate = (AgpdSkillTemplate *) pAgpdSkill->m_pcsTemplate;

	// if saved?
	if (pThis->m_pAgpdSkill != pAgpdSkill)
		return FALSE;

	//pThis->m_bHold = TRUE;

	if (pThis->m_pAgcmUICooldown && AGCMUICOOLDOWN_INVALID_ID != pThis->m_lCooldownID)
		return pThis->m_pAgcmUICooldown->StartCooldown(pThis->m_lCooldownID, AGPMPRODUCT_GATHER_DURATION_MSEC);
	else
		return pThis->SendPacketGather(pThis->m_pAgpdSkill->m_lID, pThis->m_lTargetID);
	}


BOOL AgcmProductMed::CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;
		
	AgcmProductMed *pThis = (AgcmProductMed *) pClass;
	
	if (!pThis->m_pAgpdSkill || AP_INVALID_CID == pThis->m_lTargetID)
		return FALSE;
	
	return pThis->SendPacketGather(pThis->m_pAgpdSkill->m_lID, pThis->m_lTargetID);
	}


BOOL AgcmProductMed::CBProductResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmProductMed *pThis = (AgcmProductMed *) pClass;
	INT32 lResult = *(INT32 *) pCustData;
	INT32 lSkillID = (INT32) ((PVOID *)pData)[0];
	INT32 lItemTID = (INT32) ((PVOID *)pData)[1];
	INT32 lExp = (INT32) ((PVOID *)pData)[2];

	int nCount = ( int )( ( ( void** )pData )[ 3 ] );
	return pThis->OnResult( lResult, lSkillID, lItemTID, lExp, nCount );
	}


//	OnResult
//============================================
//
BOOL AgcmProductMed::OnResult( INT32 lResult, INT32 lSkillID, INT32 lItemTID, INT32 lExp, int nCount )
	{
	m_pAgpdSkill = NULL;
	m_lTargetID = AP_INVALID_CID;
	//m_bHold = FALSE;
	
	AgpdSkill *pAgpdSkill = m_pAgpmSkill->GetSkill(lSkillID);
	if (!pAgpdSkill || !pAgpdSkill->m_pcsTemplate)
		return FALSE;

	INT32 eCategory = AGPMPRODUCT_CATEGORY_NONE;
	if (m_pAgpmProduct->IsComposeSkill(((AgpdSkillTemplate *)pAgpdSkill->m_pcsTemplate)->m_lID, &eCategory))
		{
		m_pAgcmUIProduct->OnResult( pAgpdSkill, lResult, lItemTID, lExp, nCount );
		}
	else
		{
		m_pAgcmUIProductSkill->OnResult( pAgpdSkill, lResult, lItemTID, lExp, nCount );
		}

	return TRUE;
	}


BOOL AgcmProductMed::SendPacketGather(INT32 lSkillID, INT32 lTargetID)
	{
	INT16	nPacketLength = 0;
	INT8	cOperation = AGPMPRODUCT_PACKET_OPERATION_GATHER;

	if(!m_pAgcmCharacter->GetSelfCharacter())
		return FALSE;

	INT32 lCID = m_pAgcmCharacter->GetSelfCharacter()->m_lID;
	PVOID pvPacket = m_pAgpmProduct->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRODUCT_PACKET_TYPE,
														   &cOperation,
														   &lCID,
														   &lSkillID,
														   &lTargetID,
														   NULL,
														   NULL,
														   NULL,
														   NULL
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmProduct->m_csPacket.FreePacket(pvPacket);

	return bResult;	
	
	}