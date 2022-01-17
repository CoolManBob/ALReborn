/*==============================================================

	AgsmEventProduct.cpp
	
==============================================================*/

#include "AgsmEventProduct.h"


/************************************************************/
/*		The Implementation of AgsmEventProduct class		*/
/************************************************************/
//
AgsmEventProduct::AgsmEventProduct()
	{
	SetModuleName("AgsmEventProduct");

	m_pAgpmCharacter = NULL;
	m_pAgpmSkill = NULL;
	m_pAgpmProduct = NULL;
	m_pAgpmEventProduct = NULL;
	m_pAgsmCharacter = NULL;
	m_pAgsmProduct = NULL;
	}

AgsmEventProduct::~AgsmEventProduct()
	{
	}


//	ApModule inherited
//=========================================
//
BOOL AgsmEventProduct::OnAddModule()
	{
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmSkill = (AgpmSkill *) GetModule("AgpmSkill");
	m_pAgpmProduct = (AgpmProduct *) GetModule("AgpmProduct");
	m_pAgpmEventProduct = (AgpmEventProduct *) GetModule("AgpmEventProduct");
	m_pAgsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pAgsmProduct = (AgsmProduct *) GetModule("AgsmProduct");
	m_pAgpmLog = (AgpmLog *) GetModule("AgpmLog");

	if (!m_pAgpmCharacter || !m_pAgpmSkill ||
		!m_pAgpmProduct || !m_pAgpmEventProduct ||
		!m_pAgsmCharacter || !m_pAgsmProduct
	    )
		return FALSE;

	if (!m_pAgpmEventProduct->SetCallbackRequest(CBProductEventRequest, this))
		return FALSE;

	if (!m_pAgpmEventProduct->SetCallbackLearnSkill(CBLearnSkill, this))
		return FALSE;
		
	if (!m_pAgpmEventProduct->SetCallbackBuyCompose(CBBuyCompose, this))
		return FALSE;
	
	return TRUE;
	}


//	Callback
//=========================================
//
BOOL AgsmEventProduct::CBProductEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pApdEvent = (ApdEvent *) pData;
	AgsmEventProduct	*pThis = (AgsmEventProduct *) pClass;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pAgpmEventProduct->MakePacketEventGrant(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	BOOL bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter));
	pThis->m_pAgpmEventProduct->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
	}
	
BOOL AgsmEventProduct::CBLearnSkill(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;	

	ApdEvent			*pApdEvent = (ApdEvent *) pData;
	AgsmEventProduct	*pThis = (AgsmEventProduct *) pClass;
	PVOID				*ppvBuffer = (PVOID *) pCustData;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) ppvBuffer[0];
	INT32				lSkillTID = PtrToInt(ppvBuffer[1]);

	INT32 lResult = AGPMEVENT_PRODUCT_RESULT_SUCCESS;

	INT64 llPrice = 0;
	INT32 lTax = 0;
	if (!pThis->m_pAgpmEventProduct->IsValidLearnSkill(pAgpdCharacter, lSkillTID, &lResult, &llPrice, &lTax))
		{
		return pThis->SendPacketLearnSkillResult(pAgpdCharacter, lSkillTID, lResult);
		}

	// learn skill
	AgpdSkill *pAgpdSkill = pThis->m_pAgpmSkill->LearnSkill(pAgpdCharacter, lSkillTID, 0);
	if (!pAgpdSkill)
		{
		lResult = AGPMEVENT_PRODUCT_RESULT_FAIL;
		return pThis->SendPacketLearnSkillResult(pAgpdCharacter, lSkillTID, lResult);
		}
	
	// sub inven money
	pThis->m_pAgpmCharacter->SubMoney(pAgpdCharacter, llPrice);
	pThis->m_pAgpmCharacter->PayTax(pAgpdCharacter, lTax);
	
	// update db
	pThis->m_pAgsmProduct->UpdateDB(pAgpdCharacter);
	
	// log
	if (pThis->m_pAgpmLog)
		{
		AgsdCharacter *pAgsdCharacter = pThis->m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
		if (pAgsdCharacter)
			{
			INT32 lLevel = pThis->m_pAgpmCharacter->GetLevel(pAgpdCharacter);
			INT64 llExp = pThis->m_pAgpmCharacter->GetExp(pAgpdCharacter);

			pThis->m_pAgpmLog->WriteLog_SkillBuy(0, &pAgsdCharacter->m_strIPAddress[0],
												pAgsdCharacter->m_szAccountID,
												pAgsdCharacter->m_szServerName,
												pAgpdCharacter->m_szID,
												((AgpdCharacterTemplate *)(pAgpdCharacter->m_pcsCharacterTemplate))->m_lID,
												lLevel,
												llExp,
												pAgpdCharacter->m_llMoney,
												pAgpdCharacter->m_llBankMoney,
												lSkillTID,
												NULL,
												(INT32)llPrice
												);
			}
		}

	// send result packet
	return pThis->SendPacketLearnSkillResult(pAgpdCharacter, lSkillTID, lResult);
	}
	
BOOL AgsmEventProduct::CBBuyCompose(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;	

	ApdEvent			*pApdEvent = (ApdEvent *) pData;
	AgsmEventProduct	*pThis = (AgsmEventProduct *) pClass;
	PVOID				*ppvBuffer = (PVOID *) pCustData;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) ppvBuffer[0];
	INT32				lComposeID = PtrToInt(ppvBuffer[1]);
	
	INT32 lResult = AGPMEVENT_PRODUCT_RESULT_SUCCESS;
	
	AgpdComposeTemplate *pAgpdComposeTemplate = pThis->m_pAgpmProduct->GetComposeTemplate(lComposeID);
	if (!pAgpdComposeTemplate)
		{
		lResult = AGPMEVENT_PRODUCT_RESULT_FAIL;
		return pThis->SendPacketBuyComposeResult(pAgpdCharacter, lComposeID, lResult);		
		}

	if (!pThis->m_pAgpmEventProduct->IsValidBuyCompose(pAgpdCharacter, pAgpdComposeTemplate, &lResult))
		{
		return pThis->SendPacketBuyComposeResult(pAgpdCharacter, lComposeID, lResult);		
		}
		
	// learn compose
	pThis->m_pAgpmProduct->LearnCompose(pAgpdCharacter, pAgpdComposeTemplate);
	
	// sub inven money
	pThis->m_pAgpmCharacter->SubMoney(pAgpdCharacter, pAgpdComposeTemplate->m_lPrice);

	// update db
	pThis->m_pAgsmProduct->UpdateDB(pAgpdCharacter);

	// log
	if (pThis->m_pAgpmLog)
		{
		AgsdCharacter *pAgsdCharacter = pThis->m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
		if (pAgsdCharacter)
			{
			INT32 lLevel = pThis->m_pAgpmCharacter->GetLevel(pAgpdCharacter);
			INT64 llExp = pThis->m_pAgpmCharacter->GetExp(pAgpdCharacter);

			pThis->m_pAgpmLog->WriteLog_PDSkillBuy(0, &pAgsdCharacter->m_strIPAddress[0],
												pAgsdCharacter->m_szAccountID,
												pAgsdCharacter->m_szServerName,
												pAgpdCharacter->m_szID,
												((AgpdCharacterTemplate *)(pAgpdCharacter->m_pcsCharacterTemplate))->m_lID,
												lLevel,
												llExp,
												pAgpdCharacter->m_llMoney,
												pAgpdCharacter->m_llBankMoney,
												lComposeID,
												NULL,
												pAgpdComposeTemplate->m_lPrice
												);
			}
		}

	// send result packet
	return pThis->SendPacketBuyComposeResult(pAgpdCharacter, lComposeID, lResult);
	}

	
//	Packet
//=========================================================
//
BOOL AgsmEventProduct::SendPacketLearnSkillResult(AgpdCharacter *pAgpdCharacter, INT32 lSkillTID, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT16 nPacketLength	= 0;			
	PVOID pvPacket = m_pAgpmEventProduct->MakePacketLearnSkillResult(pAgpdCharacter->m_lID, lSkillTID, lResult, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter));

	m_pAgpmEventProduct->m_csPacket.FreePacket(pvPacket);

	return	bResult;
	}
	
BOOL AgsmEventProduct::SendPacketBuyComposeResult(AgpdCharacter *pAgpdCharacter, INT32 lComposeID, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT16 nPacketLength	= 0;			
	PVOID pvPacket = m_pAgpmEventProduct->MakePacketBuyComposeResult(pAgpdCharacter->m_lID, lComposeID, lResult, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter));

	m_pAgpmEventProduct->m_csPacket.FreePacket(pvPacket);

	return	bResult;	
	}