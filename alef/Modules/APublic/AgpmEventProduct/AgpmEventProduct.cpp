/*===================================================================

	AgpmEventProduct.cpp
	
===================================================================*/

#include "AgpmEventProduct.h"

#include "ApMemoryTracker.h"
#include "ApModuleStream.h"


/********************************************************/
/*	The Implementation of AgpmEventProduct class		*/
/********************************************************/
//
AgpmEventProduct::AgpmEventProduct()
	{
	SetModuleName("AgpmEventProduct");
	SetPacketType(AGPMEVENT_PRODUCT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT8,		1,			// Operation
							AUTYPE_PACKET,		1,			// Event Base Packet
							AUTYPE_INT32,		1,			// CID
							AUTYPE_INT32,		1,			// ID (SkillTID or ComposeID)
							AUTYPE_INT32,		1,			// Result
							AUTYPE_END,			0
							);
							
	m_csPacketEmb.SetFlagLength(sizeof(INT8));
	m_csPacketEmb.SetFieldType(AUTYPE_INT32,		1,	// product skill category
							   AUTYPE_END,		0
							   );

	m_pAgpmCharacter		= NULL;
	m_pAgpmProduct			= NULL;
	m_pApmEventManager		= NULL;
	m_pAgpmEventSkillMaster = NULL;
	}

AgpmEventProduct::~AgpmEventProduct()
	{
	}


//	ApModule inherited
//=============================================
//
BOOL AgpmEventProduct::OnAddModule()
	{
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmProduct = (AgpmProduct *) GetModule("AgpmProduct");
	m_pApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pAgpmEventSkillMaster = (AgpmEventSkillMaster *) GetModule("AgpmEventSkillMaster");
	
	if (!m_pAgpmCharacter || !m_pAgpmProduct || !m_pApmEventManager)
		return FALSE;

	if (!m_pAgpmCharacter->SetCallbackActionEventProduct(CBActionProduct, this))
		return FALSE;

	if (!m_pApmEventManager->RegisterEvent(APDEVENT_FUNCTION_PRODUCT, CBEventConstructor, CBEventDestructor, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
		return FALSE;

	if (!m_pApmEventManager->RegisterPacketFunction(CBEmbeddedPacketEncode, CBEmbeddedPacketDecode, this, APDEVENT_FUNCTION_PRODUCT))
		return FALSE;

	return TRUE;
	}

BOOL AgpmEventProduct::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation = -1;
	PVOID	pvPacketEvent = NULL;
	INT32	lCID = -1;
	INT32	lID = -1;
	INT32	lResult = AGPMEVENT_PRODUCT_RESULT_FAIL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvPacketEvent,
						&lCID,
						&lID,
						&lResult
						);
	
	ApdEvent *pApdEvent = NULL;

	if(pvPacketEvent)
		{
		pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEvent);
		if (!pApdEvent || APDEVENT_FUNCTION_PRODUCT != pApdEvent->m_eFunction)
			return FALSE;
		}

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacterLock(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
	{
		pAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch(cOperation)
		{
		case AGPMEVENT_PRODUCT_PACKET_REQUEST:
			OnOperationRequest(pApdEvent, pAgpdCharacter);
			break;

		case AGPMEVENT_PRODUCT_PACKET_GRANT:
			OnOperationGrant(pApdEvent, pAgpdCharacter);
			break;

		case AGPMEVENT_PRODUCT_PACKET_LEARN_SKILL:
			OnOperationLearnSkill(pApdEvent, pAgpdCharacter, lID);
			break;
			
		case AGPMEVENT_PRODUCT_PACKET_LEARN_SKILL_RESULT:
			OnOperationLearnSkillResult(pAgpdCharacter, lID, lResult);
			break;
			
		case AGPMEVENT_PRODUCT_PACKET_BUY_COMPOSE:
			OnOperationBuyCompose(pApdEvent, pAgpdCharacter, lID);
			break;
			
		case AGPMEVENT_PRODUCT_PACKET_BUY_COMPOSE_RESULT:
			OnOperationBuyComposeResult(pAgpdCharacter, lID, lResult);
			break;
		}

	pAgpdCharacter->m_Mutex.Release();
	return TRUE;
	}


//	Validation
//======================================
//
BOOL AgpmEventProduct::IsValidLearnSkill(AgpdCharacter *pAgpdCharacter, AgpdSkillTemplate *pAgpdSkillTemplate, INT32 *plResult, INT64 *pllPrice, INT32 *plTax)
	{
	INT32 lResult = AGPMEVENT_PRODUCT_RESULT_FAIL;
	INT64 llNeed = 0;
	INT64 llMoney = 0;
	INT32 lLevel = 0;
	
	if (!pAgpdCharacter || !pAgpdSkillTemplate)
		goto my_result1;

	// check level
	lLevel = m_pAgpmCharacter->GetLevel(pAgpdCharacter);
	if (lLevel < GetLearnableLevel(pAgpdSkillTemplate))
		{
		lResult = AGPMEVENT_PRODUCT_RESULT_FAIL_INSUFFICIENT_LEVEL;
		goto my_result1;		
		}

	// check money
	m_pAgpmCharacter->GetMoney(pAgpdCharacter, &llMoney);
	llNeed = (INT64) GetBuyCost(pAgpdSkillTemplate, pAgpdCharacter, plTax);
	if (pllPrice)
		*pllPrice = llNeed;
	
	if (llMoney < llNeed)
		{
		lResult = AGPMEVENT_PRODUCT_RESULT_FAIL_NOT_ENOUGH_MONEY;
		goto my_result1;
		}

	lResult = AGPMEVENT_PRODUCT_RESULT_SUCCESS;
	
  my_result1:
	if (plResult)
		*plResult = lResult;

	return (AGPMEVENT_PRODUCT_RESULT_SUCCESS == lResult);	
	}


BOOL AgpmEventProduct::IsValidLearnSkill(AgpdCharacter *pAgpdCharacter, INT32 lSkillTID, INT32 *plResult, INT64 *pllPrice, INT32 *plTax)
	{
	AgpdSkillTemplate *pAgpdSkillTemplate = m_pAgpmProduct->m_pAgpmSkill->GetSkillTemplate(lSkillTID);
	
	return IsValidLearnSkill(pAgpdCharacter, pAgpdSkillTemplate, plResult, pllPrice, plTax);
	}

	
BOOL AgpmEventProduct::IsValidBuyCompose(AgpdCharacter *pAgpdCharacter, AgpdComposeTemplate *pAgpdComposeTemplate, INT32 *plResult)
	{
	INT32 lResult = AGPMEVENT_PRODUCT_RESULT_FAIL;
	INT64 llNeed = 0;
	INT64 llMoney = 0;
	
	if (!pAgpdCharacter || !pAgpdComposeTemplate)
		goto my_result2;

	// if already bought
	if (m_pAgpmProduct->IsOwnCompose(pAgpdCharacter, pAgpdComposeTemplate))
		{
		lResult = AGPMEVENT_PRODUCT_RESULT_FAIL_ALREADY_BOUGHT;
		goto my_result2;
		}
	
	// check prerequisites
	if (!m_pAgpmProduct->IsSatisfyPrerequisites(pAgpdCharacter, pAgpdComposeTemplate))
		{
		lResult = AGPMEVENT_PRODUCT_RESULT_FAIL_NEED_PREREQUISITES;
		goto my_result2;
		}

	// check level
	if (!m_pAgpmProduct->IsValidComposeLevel(pAgpdCharacter, pAgpdComposeTemplate, NULL))
		{
		lResult = AGPMEVENT_PRODUCT_RESULT_FAIL_INSUFFICIENT_LEVEL;
		goto my_result2;
		}

	// check money	
	m_pAgpmCharacter->GetMoney(pAgpdCharacter, &llMoney);
	llNeed = (INT64) pAgpdComposeTemplate->m_lPrice;
	
	if (llMoney < llNeed)
		{
		lResult = AGPMEVENT_PRODUCT_RESULT_FAIL_NOT_ENOUGH_MONEY;
		goto my_result2;
		}

	lResult = AGPMEVENT_PRODUCT_RESULT_SUCCESS;
	
  my_result2:
	if (plResult)
		*plResult = lResult;

	return (AGPMEVENT_PRODUCT_RESULT_SUCCESS == lResult);		
	}


//
//=====================================
//
INT32 AgpmEventProduct::GetBuyCost(INT32 lSkillTID, AgpdCharacter *pAgpdCharacter, INT32* plTax)
	{
	if (!m_pAgpmEventSkillMaster)
		return 0;
	
	return m_pAgpmEventSkillMaster->GetBuyCost(lSkillTID, pAgpdCharacter, plTax);
	}
	
INT32 AgpmEventProduct::GetBuyCost(AgpdSkillTemplate *pAgpdSkillTemplate, AgpdCharacter *pAgpdCharacter, INT32* plTax)
	{
	if (!m_pAgpmEventSkillMaster)
		return 0;
	
	return m_pAgpmEventSkillMaster->GetBuyCost(pAgpdSkillTemplate, pAgpdCharacter, plTax);	
	}
	
INT32 AgpmEventProduct::GetLearnableLevel(INT32 lSkillTID)
	{
	if (!m_pAgpmEventSkillMaster)
		return 0;
	
	return m_pAgpmEventSkillMaster->GetLearnableLevel(lSkillTID);	
	}
	
INT32 AgpmEventProduct::GetLearnableLevel(AgpdSkillTemplate *pAgpdSkillTemplate)
	{
	if (!m_pAgpmEventSkillMaster)
		return 0;
	
	return m_pAgpmEventSkillMaster->GetLearnableLevel(pAgpdSkillTemplate);	
	}




//	Operations
//=====================================
//
BOOL AgpmEventProduct::OnOperationRequest(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if(!pApdEvent || !pAgpdCharacter)
		return FALSE;

	AuPOS stTargetPos;
	memset(&stTargetPos, 0, sizeof(stTargetPos));
	
	if (m_pApmEventManager->CheckValidRange(pApdEvent, &pAgpdCharacter->m_stPos, AGPMEVENT_PRODUCT_MAX_USE_RANGE, &stTargetPos))
		{
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
		if (pAgpdCharacter->m_bMove)
			m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);

		EnumCallback(AGPMEVENT_PRODUCT_CB_REQUEST, pApdEvent, pAgpdCharacter);
		}
	else
		{
		// move to <stTargetPos>
		pAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_PRODUCT;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pApdEvent->m_pcsSource->m_eType;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pApdEvent->m_pcsSource->m_lID;

		m_pAgpmCharacter->MoveCharacter(pAgpdCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
		}

	return TRUE;
	}

BOOL AgpmEventProduct::OnOperationGrant(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	EnumCallback(AGPMEVENT_PRODUCT_CB_GRANT, pApdEvent, pAgpdCharacter);
	return TRUE;
	}

BOOL AgpmEventProduct::OnOperationBuyCompose(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter, INT32 lComposeID)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = (PVOID) pAgpdCharacter;
	pvBuffer[1] = IntToPtr(lComposeID);

	EnumCallback(AGPMEVENT_PRODUCT_CB_BUY_COMPOSE, pApdEvent, pvBuffer);
	return TRUE;	
	}

BOOL AgpmEventProduct::OnOperationBuyComposeResult(AgpdCharacter *pAgpdCharacter, INT32 lComposeID, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	if (AGPMEVENT_PRODUCT_RESULT_SUCCESS == lResult)
		m_pAgpmProduct->LearnCompose(pAgpdCharacter, lComposeID);

	PVOID pvBuffer[3];
	pvBuffer[0] = (PVOID) pAgpdCharacter;
	pvBuffer[1] = IntToPtr(lResult);
	pvBuffer[2] = IntToPtr(lComposeID);

	EnumCallback(AGPMEVENT_PRODUCT_CB_BUY_COMPOSE_RESULT, NULL, pvBuffer);
	return TRUE;		
	}

BOOL AgpmEventProduct::OnOperationLearnSkill(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter, INT32 lSkillTID)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = (PVOID) pAgpdCharacter;
	pvBuffer[1] = IntToPtr(lSkillTID);

	EnumCallback(AGPMEVENT_PRODUCT_CB_LEARN_SKILL, pApdEvent, pvBuffer);
	return TRUE;		
	}

BOOL AgpmEventProduct::OnOperationLearnSkillResult(AgpdCharacter *pAgpdCharacter, INT32 lSkillTID, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	PVOID pvBuffer[3];
	pvBuffer[0] = (PVOID) pAgpdCharacter;
	pvBuffer[1] = IntToPtr(lResult);
	pvBuffer[2] = IntToPtr(lSkillTID);

	EnumCallback(AGPMEVENT_PRODUCT_CB_LEARN_SKILL_RESULT, NULL, pvBuffer);
	return TRUE;		
	}

//	Callback setting
//==========================================
//
BOOL AgpmEventProduct::SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMEVENT_PRODUCT_CB_REQUEST, pfCallback, pClass);
	}

BOOL AgpmEventProduct::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMEVENT_PRODUCT_CB_GRANT, pfCallback, pClass);
	}

BOOL AgpmEventProduct::SetCallbackBuyCompose(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMEVENT_PRODUCT_CB_BUY_COMPOSE, pfCallback, pClass);
	}
	
BOOL AgpmEventProduct::SetCallbackBuyComposeResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMEVENT_PRODUCT_CB_BUY_COMPOSE_RESULT, pfCallback, pClass);
	}
	
BOOL AgpmEventProduct::SetCallbackLearnSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMEVENT_PRODUCT_CB_LEARN_SKILL, pfCallback, pClass);
	}
	
BOOL AgpmEventProduct::SetCallbackLearnSkillResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMEVENT_PRODUCT_CB_LEARN_SKILL_RESULT, pfCallback, pClass);
	}
	
//	Packet
//============================================
//
PVOID AgpmEventProduct::MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_PRODUCT_PACKET_REQUEST;
	
	PVOID pvPacketBase = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_PRODUCT_PACKET_TYPE,
										   &cOperation,
										   pvPacketBase,
										   &lCID,
										   NULL,
										   NULL
										   );
	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
	}

PVOID AgpmEventProduct::MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_PRODUCT_PACKET_GRANT;
	PVOID pvPacketBase = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_PRODUCT_PACKET_TYPE,
										   &cOperation,
										   pvPacketBase,
										   &lCID,
										   NULL,
										   NULL
										   );
	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
	}

PVOID AgpmEventProduct::MakePacketBuyCompose(ApdEvent *pApdEvent, INT32 lCID, INT32 lComposeID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_PRODUCT_PACKET_BUY_COMPOSE;
	PVOID pvPacketBase = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_PRODUCT_PACKET_TYPE,
										   &cOperation,
										   pvPacketBase,
										   &lCID,
										   &lComposeID,
										   NULL
										   );
	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;	
	}
	
PVOID AgpmEventProduct::MakePacketBuyComposeResult(INT32 lCID, INT32 lComposeID, INT32 lResult, INT16 *pnPacketLength)
	{
	if (!pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_PRODUCT_PACKET_BUY_COMPOSE_RESULT;
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_PRODUCT_PACKET_TYPE,
										   &cOperation,
										   NULL,
										   &lCID,
										   &lComposeID,
										   &lResult
										   );
	return pvPacket;		
	}
	
PVOID AgpmEventProduct::MakePacketLearnSkill(ApdEvent *pApdEvent, INT32 lCID, INT32 lSkillTID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_PRODUCT_PACKET_LEARN_SKILL;
	PVOID pvPacketBase = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_PRODUCT_PACKET_TYPE,
										   &cOperation,
										   pvPacketBase,
										   &lCID,
										   &lSkillTID,
										   NULL
										   );
	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;		
	}
	
PVOID AgpmEventProduct::MakePacketLearnSkillResult(INT32 lCID, INT32 lSkillTID, INT32 lResult, INT16 *pnPacketLength)
	{
	if (!pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_PRODUCT_PACKET_LEARN_SKILL_RESULT;
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_PRODUCT_PACKET_TYPE,
										   &cOperation,
										   NULL,
										   &lCID,
										   &lSkillTID,
										   &lResult
										   );
	return pvPacket;
	}


//	Callback
//=========================================
//
BOOL AgpmEventProduct::CBActionProduct(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpmEventProduct *pThis = (AgpmEventProduct *) pClass;
	AgpdCharacterAction *pstAction = (AgpdCharacterAction *) pCustData;

	if (pThis->m_pAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
		return FALSE;

	pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	if(pAgpdCharacter->m_bMove)
		pThis->m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);
	
	ApdEvent *pApdEvent = pThis->m_pApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType, pstAction->m_csTargetBase.m_lID, APDEVENT_FUNCTION_PRODUCT);

	pThis->EnumCallback(AGPMEVENT_PRODUCT_CB_REQUEST, pApdEvent, pAgpdCharacter);
	
	return TRUE;
	}


//	Event data
//=======================================================
//
BOOL AgpmEventProduct::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pApdEvent	= (ApdEvent *)			pData;
	AgpmEventProduct	*pThis		= (AgpmEventProduct *)	pClass;
	ApModuleStream		*pStream	= (ApModuleStream *)	pCustData;

	AgpdEventProduct *pAgpdEventProduct = (AgpdEventProduct *)	pApdEvent->m_pvData;
	if (!pAgpdEventProduct)
		return FALSE;

	if (!pStream->WriteValue(AGPMEVENT_PRODUCT_STREAM_NAME_CATEGORY, (INT32) pAgpdEventProduct->m_eCategory))
		{
		OutputDebugString("AgpmEventProduct::CBStreamWriteEvent() Error (1) !!!\n");
		return FALSE;
		}

	if (!pStream->WriteValue(AGPMEVENT_PRODUCT_STREAM_NAME_END, 0))
		return FALSE;

	return TRUE;
	}

BOOL AgpmEventProduct::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ApdEvent			*pApdEvent	= (ApdEvent *)			pData;
	AgpmEventProduct	*pThis		= (AgpmEventProduct *)	pClass;
	ApModuleStream		*pStream	= (ApModuleStream *)	pCustData;

	AgpdEventProduct *pAgpdEventProduct = (AgpdEventProduct *) pApdEvent->m_pvData;
	if (!pAgpdEventProduct)
		return FALSE;

	const CHAR *pszValueName = NULL;

	while (pStream->ReadNextValue())
		{
		pszValueName = pStream->GetValueName();
		if (!strcmp(pszValueName, AGPMEVENT_PRODUCT_STREAM_NAME_CATEGORY))
			{
			INT32 eCategory = 0;
			if (!pStream->GetValue(&eCategory))
				{
				TRACE("AgpmEventProduct::CBStreamReadEvent() GetCategory Failed\n"); 
				ASSERT(!"AgpmEventProduct::CBStreamReadEvent() GetCategory Failed");
				}

			pAgpdEventProduct->m_eCategory = eCategory;
			}
		else if (!strcmp(pszValueName, AGPMEVENT_PRODUCT_STREAM_NAME_END))
			break;
		}
	
	return TRUE;
	}

BOOL AgpmEventProduct::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventProduct	*pThis		= (AgpmEventProduct *) pClass;
	ApdEvent			*pApdEvent	= (ApdEvent *) pData;

	pApdEvent->m_pvData	= (PVOID) new BYTE[sizeof(AgpdEventProduct)];
	if (!pApdEvent->m_pvData)
		return FALSE;

	ZeroMemory(pApdEvent->m_pvData, sizeof(AgpdEventProduct));

	((AgpdEventProduct *) pApdEvent->m_pvData)->m_eCategory = 0;

	return TRUE;	
	}

BOOL AgpmEventProduct::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventProduct	*pThis		= (AgpmEventProduct *) pClass;
	ApdEvent			*pApdEvent	= (ApdEvent *) pData;

	if (pApdEvent->m_pvData)
		{
		delete [] (BYTE *) pApdEvent->m_pvData;
		pApdEvent->m_pvData	= NULL;
		}

	return TRUE;
	}

PVOID AgpmEventProduct::MakePacketEventData(ApdEvent *pApdEvent)
	{
	if (!pApdEvent || !pApdEvent->m_pvData)
		return NULL;

	AgpdEventProduct *pAgpdEventProduct = (AgpdEventProduct *) pApdEvent->m_pvData;
	INT32 eCategory = (INT32) pAgpdEventProduct->m_eCategory;

	return m_csPacketEmb.MakePacket(FALSE, NULL, 0,
									&eCategory
									);
	}

BOOL AgpmEventProduct::ParsePacketEventData(ApdEvent *pApdEvent, PVOID pvPacketEmb)
	{
	if (!pApdEvent || !pApdEvent->m_pvData || !pvPacketEmb)
		return FALSE;

	INT32 eCategory = 0;
	m_csPacketEmb.GetField(FALSE, pvPacketEmb, 0,
						   &eCategory
						   );

	AgpdEventProduct *pAgpdEventProduct = (AgpdEventProduct *) pApdEvent->m_pvData;
	pAgpdEventProduct->m_eCategory = eCategory;

	return TRUE;	
	}

BOOL AgpmEventProduct::CBEmbeddedPacketEncode(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventProduct	*pThis			= (AgpmEventProduct *) pClass;
	ApdEvent			*pApdEvent		= (ApdEvent *) pData;
	PVOID				*ppvPacketEmb	= (PVOID *) pCustData;

	*ppvPacketEmb = pThis->MakePacketEventData(pApdEvent);	

	return TRUE;
	}

BOOL AgpmEventProduct::CBEmbeddedPacketDecode(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventProduct	*pThis		= (AgpmEventProduct *)	pClass;
	ApdEvent			*pApdEvent	= (ApdEvent *)	pData;
	PVOID				pvPacketEmb	= (PVOID)		pCustData;

	return pThis->ParsePacketEventData(pApdEvent, pvPacketEmb);
	}