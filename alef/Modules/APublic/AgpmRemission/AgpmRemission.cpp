/*===========================================================================

		AgpmRemission.cpp

===========================================================================*/


#include "AgpmRemission.h"


/********************************************************/
/*		The Implementation of AgpmRemission class		*/
/********************************************************/
//
AgpmRemission::AgpmRemission()
	{
	SetModuleName("AgpmRemission");
	SetPacketType(AGPMREMISSION_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT8,		1,			// Operation
							AUTYPE_INT32,		1,			// CID
							AUTYPE_PACKET,		1,			// Event Packet/Remit Pakcet
							AUTYPE_END,			0
							);

	m_csPacketRemit.SetFlagLength(sizeof(INT8));
	m_csPacketRemit.SetFieldType(AUTYPE_INT8,	1,		// Remission Type
								 AUTYPE_INT32,	1,		// Result
								 AUTYPE_END,	0
								 );

	m_pAgpmCharacter = NULL;
	m_pAgpmItem = NULL;
	m_pApmEventManager = NULL;
	m_pAgpmGrid = NULL;
	}


AgpmRemission::~AgpmRemission()
	{
	}




//	ApModule inherited
//=======================================
//
BOOL AgpmRemission::OnAddModule()
	{
	m_pApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmGrid = (AgpmGrid *) GetModule("AgpmGrid");

	if (!m_pApmEventManager || !m_pAgpmCharacter || !m_pAgpmItem || !m_pAgpmGrid)
		return FALSE;

	if (!m_pAgpmCharacter->SetCallbackActionEventRemission(CBEventAction, this))
		return FALSE;

	if (!m_pApmEventManager->RegisterEvent(APDEVENT_FUNCTION_REMISSION, CBEventConstructor, CBEventDestructor, NULL, NULL, NULL, this))
		return FALSE;

	return TRUE;
	}


BOOL AgpmRemission::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation = -1;
	PVOID	pvPacketEmb = NULL;
	INT32	lCID = -1;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&pvPacketEmb
						);

	if (!pvPacketEmb)
		return FALSE;
	
	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacterLock(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
	{
		pAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (cOperation)
		{
		case AGPMREMISSION_OPERATION_EVENT_REQUEST :
			{
			ApdEvent* pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEmb);
			if(!pApdEvent)
			{
				pAgpdCharacter->m_Mutex.Release();
				return FALSE;
			}

			if (APDEVENT_FUNCTION_REMISSION != pApdEvent->m_eFunction)
			{
				pAgpdCharacter->m_Mutex.Release();
				return FALSE;
			}
					
			OnOperationEventRequest(pApdEvent, pAgpdCharacter);
			}
			break;

		case AGPMREMISSION_OPERATION_EVENT_GRANT :
			{
			ApdEvent* pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEmb);
			if(!pApdEvent)
			{
				pAgpdCharacter->m_Mutex.Release();
				return FALSE;
			}

			if (APDEVENT_FUNCTION_REMISSION != pApdEvent->m_eFunction)
			{
				pAgpdCharacter->m_Mutex.Release();
				return FALSE;
			}
					
			OnOperationEventGrant(pApdEvent, pAgpdCharacter);
			}
			break;
			
		case AGPMREMISSION_OPERATION_REMIT :
			{
			AgpdRemitArg	cAgpdRemitArg;
	
			m_csPacketRemit.GetField(FALSE, pvPacketEmb, 0,
							 &cAgpdRemitArg.m_cType,
							 &cAgpdRemitArg.m_lResult
							 );
			
			EnumCallback(AGPMREMISSION_CB_REMIT, &cAgpdRemitArg, pAgpdCharacter);
			}
			break;

		default :
			break;
		}

	pAgpdCharacter->m_Mutex.Release();

	return TRUE;
	}




//	Operation
//==================================================
//
BOOL AgpmRemission::OnOperationEventRequest(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	AuPOS stTargetPos;
	memset(&stTargetPos, 0, sizeof(stTargetPos));
	
	if (m_pApmEventManager->CheckValidRange(pApdEvent, &pAgpdCharacter->m_stPos, AGPMREMISSION_MAX_USE_RANGE, &stTargetPos))
		{
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if(pAgpdCharacter->m_bMove)
			m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);

		EnumCallback(AGPMREMISSION_CB_EVENT_REQUEST, pApdEvent, pAgpdCharacter);
		}
	else
		{
		// move to target position
		pAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_REMISSION;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pApdEvent->m_pcsSource->m_eType;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pApdEvent->m_pcsSource->m_lID;

		m_pAgpmCharacter->MoveCharacter(pAgpdCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
		}

	return TRUE;
	}


BOOL AgpmRemission::OnOperationEventGrant(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	EnumCallback(AGPMREMISSION_CB_EVENT_GRANT, pApdEvent, pAgpdCharacter);

	return TRUE;
	}




//	Validation
//===================================
//
BOOL AgpmRemission::IsValidStatus(AgpdCharacter *pAgpdCharacter, INT8 cType, INT32 *plResult)
	{
	INT32 lResult = AGPMREMISSION_RESULT_UNKNOWN;

	if (NULL == pAgpdCharacter)
		goto remit_result;
	
	INT32 lPoint = 0;
	// 악당 포인트 확인. 1이상이면 무조건 경감할 수 있다.
	//if (FALSE == m_pAgpmCharacter->IsMurderer(pAgpdCharacter, &lPoint))
	lPoint = m_pAgpmCharacter->GetMurdererPoint(pAgpdCharacter);
	if (0 >= lPoint)
		{
		lResult = AGPMREMISSION_RESULT_FAIL_NOT_MURDERER;
		goto remit_result;
		}		

	switch (cType)
		{
		case AGPMREMISSION_TYPE_GHELD :
			{
			INT64 llMoney = 0;
			m_pAgpmCharacter->GetMoney(pAgpdCharacter, &llMoney);
			
			if (AGPMREMISSION_PAY_GHELD > llMoney)
				{
				lResult = AGPMREMISSION_RESULT_FAIL_INSUFFICIENT_MONEY;
				goto remit_result;
				}
			}
			break;
			
		case AGPMREMISSION_TYPE_SKELETON :
			if (FALSE == IsValidItem(pAgpdCharacter, AGPMREMISSION_PAY_SKELETON_TID, 0, 0, AGPMREMISSION_PAY_SKELETON_QTY))
				{
				lResult = AGPMREMISSION_RESULT_FAIL_INSUFFICIENT_SKELETON;
				goto remit_result;
				}
			
			break;
			
		case AGPMREMISSION_TYPE_INDULGENCE :
			if (FALSE == IsValidItem(pAgpdCharacter, 0, AGPMITEM_TYPE_OTHER, AGPMITEM_OTHER_TYPE_REMISSION, AGPMREMISSION_PAY_INDULGENCE_QTY))
				{
				lResult = AGPMREMISSION_RESULT_FAIL_INSUFFICIENT_INDULGENCE;
				goto remit_result;
				}
			
			break;
			
		default :
			goto remit_result;
			break;
		}
	
	lResult = AGPMREMISSION_RESULT_SUCCESS;
	
  remit_result :
	if (plResult)
		*plResult = lResult;
	
	return (AGPMREMISSION_RESULT_SUCCESS == lResult);
	}


// 2007.09.28. steeple
// 아이템 타입을 추가하고, 캐쉬 인벤에서도 확인한다.
BOOL AgpmRemission::IsValidItem(AgpdCharacter *pAgpdCharacter, INT32 lItemTID, INT32 lItemType, INT32 lItemOtherType, INT32 lCount, INT32 *plCount)
{
	INT32	lTotal	= 0;

	//AgpdGridItem *pAgpdGridItem;
	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);

	lTotal += GetRemissionItemCountFromInventory(pAgpdCharacter, lItemTID, lItemType, lItemOtherType);
	lTotal += GetRemissionItemCountFromCashInventory(pAgpdCharacter, lItemTID, lItemType, lItemOtherType);

	if (plCount)
		*plCount = lTotal;
	
	return (lTotal >= lCount);
}

INT32 AgpmRemission::GetRemissionItemCountFromInventory(AgpdCharacter* pcsCharacter, INT32 lItemTID, INT32 lItemType, INT32 lItemOtherType)
{
	if(!pcsCharacter)
		return 0;

	INT32	lIndex	= 0;
	INT32	lTotal	= 0;

	AgpdGridItem *pAgpdGridItem;
	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pcsCharacter);

	for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex);
		 pAgpdGridItem;
		 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex))
	{
		// Check TID
		if (lItemTID != 0)
		{
			if (lItemTID == pAgpdGridItem->m_lItemTID)
			{
				AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
				if (!pAgpdItem || AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus)
					continue;

				if (pAgpdItem->m_pcsItemTemplate->m_bStackable)
					lTotal += pAgpdItem->m_nCount;
				else
					++lTotal;
			}
		}
		else
		{
			AgpdItem* pcsItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
			if(!pcsItem || AGPDITEM_STATUS_INVENTORY != pcsItem->m_eStatus)
				continue;

			if(pcsItem->m_pcsItemTemplate->m_nType != lItemType ||
				((AgpdItemTemplateOther*)pcsItem->m_pcsItemTemplate)->m_eOtherItemType != lItemOtherType)
				continue;

			if(pcsItem->m_pcsItemTemplate->m_bStackable)
				lTotal += pcsItem->m_nCount;
			else
				++lTotal;
		}
	}

	return lTotal;
}

// 2007.09.28. steeple
// Cash Inven 도 확인한다.
// Cash Item 인 경우에는 한장이 많은 점수를 깍을 수 있다.
INT32 AgpmRemission::GetRemissionItemCountFromCashInventory(AgpdCharacter* pcsCharacter, INT32 lItemTID, INT32 lItemType, INT32 lItemOtherType)
{
	if(!pcsCharacter)
		return 0;

	INT32	lIndex	= 0;
	INT32	lTotal	= 0;

	AgpdGridItem *pAgpdGridItem;
	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pcsCharacter);

	for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csCashInventoryGrid, &lIndex);
		 pAgpdGridItem;
		 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csCashInventoryGrid, &lIndex))
	{
		// Check TID
		if (lItemTID != 0)
		{
			if (lItemTID == pAgpdGridItem->m_lItemTID)
			{
				AgpdItem* pcsItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
				if(!pcsItem || AGPDITEM_STATUS_CASH_INVENTORY != pcsItem->m_eStatus)
					continue;

				if(pcsItem->m_pcsItemTemplate->m_bStackable)
					lTotal += (pcsItem->m_nCount * AGPMREMISSION_PAY_INDULGENCE_QTY);
				else
					lTotal += AGPMREMISSION_PAY_INDULGENCE_QTY;
			}
		}
		else
		{
			AgpdItem* pcsItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
			if(!pcsItem || AGPDITEM_STATUS_CASH_INVENTORY != pcsItem->m_eStatus)
				continue;

			if(pcsItem->m_pcsItemTemplate->m_nType != lItemType ||
				((AgpdItemTemplateOther*)pcsItem->m_pcsItemTemplate)->m_eOtherItemType != lItemOtherType)
				continue;

			if(pcsItem->m_pcsItemTemplate->m_bStackable)
				lTotal += (pcsItem->m_nCount * AGPMREMISSION_PAY_INDULGENCE_QTY);
			else
				lTotal += AGPMREMISSION_PAY_INDULGENCE_QTY;
		}
	}

	return lTotal;
}




//	Callback setting
//===================================
//
BOOL AgpmRemission::SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMREMISSION_CB_EVENT_REQUEST, pfCallback, pClass);
	}


BOOL AgpmRemission::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMREMISSION_CB_EVENT_GRANT, pfCallback, pClass);
	}


BOOL AgpmRemission::SetCallbackRemit(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMREMISSION_CB_REMIT, pfCallback, pClass);
	}




//	Make Packet
//===================================
//
PVOID AgpmRemission::MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMREMISSION_OPERATION_EVENT_REQUEST;
	
	PVOID pvPacketEvent = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketEvent)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMREMISSION_PACKET_TYPE,
										   &cOperation,
										   &lCID,
										   pvPacketEvent
										   );

	return pvPacket;
	}


PVOID AgpmRemission::MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMREMISSION_OPERATION_EVENT_GRANT;
	PVOID pvPacketEvent = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketEvent)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMREMISSION_PACKET_TYPE,
										   &cOperation,
										   &lCID,
										   pvPacketEvent
										   );

	return pvPacket;
	}




//	Event Callbacks
//======================================================
//
BOOL AgpmRemission::CBEventAction(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmRemission *pThis = (AgpmRemission *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdCharacterAction *pstAction = (AgpdCharacterAction *) pCustData;

	pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	
	if(pAgpdCharacter->m_bMove)
		pThis->m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);
	
	ApdEvent *pApdEvent = pThis->m_pApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType,
															  pstAction->m_csTargetBase.m_lID,
															  APDEVENT_FUNCTION_REMISSION);

	pThis->EnumCallback(AGPMREMISSION_CB_EVENT_REQUEST, pApdEvent, pAgpdCharacter);
	
	return TRUE;
	}


BOOL AgpmRemission::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmRemission	*pThis = (AgpmRemission *) pClass;
	ApdEvent		*pApdEvent = (ApdEvent *) pData;

	// You may attach your ...........
	pApdEvent->m_pvData = NULL;
	
	return TRUE;	
	}


BOOL AgpmRemission::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmRemission *pThis			= (AgpmRemission *) pClass;
	ApdEvent		*pApdEvent	= (ApdEvent *) pData;

	if (pApdEvent->m_pvData)
		{
		// free
		}

	return TRUE;
	}


