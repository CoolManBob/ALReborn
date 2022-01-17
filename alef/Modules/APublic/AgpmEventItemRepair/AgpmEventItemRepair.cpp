//	AgpmEventItemRepair module source file
//		- item 수리(내구도 수리)를 담당한다.
/////////////////////////////////////////////////////////////////////////

#include "AgpmEventItemRepair.h"

AgpmEventItemRepair::AgpmEventItemRepair()
{
	SetModuleName("AgpmEventItemRepair");

	SetPacketType(AGPMEVENT_ITEMREPAIR_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,						// Operation
							AUTYPE_PACKET,	1,						// Event Base Packet
							AUTYPE_INT32,	1,						// Character id
							AUTYPE_INT32,	ITEM_REPAIR_GRID_MAX,	// repair item IDs
							AUTYPE_INT64,	1,						// repair cost
							AUTYPE_INT32,	1,						// Result Code
							AUTYPE_END,		0
							);

	m_csPacketEventData.SetFlagLength(sizeof(INT8));
	m_csPacketEventData.SetFieldType(
									AUTYPE_INT32,	1,	// Quest Group ID
									AUTYPE_END,		0
									);
}

AgpmEventItemRepair::~AgpmEventItemRepair()
{
}

BOOL AgpmEventItemRepair::OnAddModule()
{
	m_pcsAgpmFactors			= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter			= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem				= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsApmEventManager		= (ApmEventManager *)	GetModule("ApmEventManager");

	if (!m_pcsAgpmCharacter || !m_pcsAgpmItem || !m_pcsApmEventManager)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionEventItemRepair(CBActionItemRepair, this))
		return FALSE;

	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_ITEM_REPAIR, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	if (!m_pcsApmEventManager->RegisterPacketFunction(CBEmbeddedMakeEventPacket, CBEmbeddedReceiveEventPacket, this, APDEVENT_FUNCTION_ITEM_REPAIR))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventItemRepair::OnInit()
{
	return TRUE;
}

BOOL AgpmEventItemRepair::OnDestroy()
{
	return TRUE;
}

PVOID AgpmEventItemRepair::MakePacketItemRepair(ApdEvent *pcsEvent, EnumAgpmEventItemRepairOperation eOperation, INT32 lCID, 
												INT32* ItemIDs, INT64 lRepairCost, EnumAgpmEventItemRepairResultCode eResultCode, 
												INT16 *pnPacketLength)
{
	if (!pcsEvent ||
		lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	PVOID	pvPacketEventBase = m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketEventBase)
		return NULL;

	INT8	cOperation	= (INT8 ) eOperation;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_ITEMREPAIR_PACKET_TYPE,
										   &cOperation,
										   pvPacketEventBase,
										   &lCID,
										   ItemIDs,
										   &lRepairCost,
										   &eResultCode);

	m_csPacket.FreePacket(pvPacketEventBase);

	return pvPacket;
}

BOOL AgpmEventItemRepair::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket ||
		nSize < 1)
		return FALSE;

	AgpdItemRepair ItemRepairInfo;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&ItemRepairInfo.cOperation,
						&ItemRepairInfo.pvPacketEventBase,
						&ItemRepairInfo.lCID,
						&ItemRepairInfo.ItemIDs[0],
						&ItemRepairInfo.llRepairCost,
						&ItemRepairInfo.eResultCode);

	// Event를 가져온다.
	ApdEvent *pcsEvent = NULL;
	if (ItemRepairInfo.pvPacketEventBase)
	{
		pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket(ItemRepairInfo.pvPacketEventBase);
		if (!pcsEvent || APDEVENT_FUNCTION_ITEM_REPAIR != pcsEvent->m_eFunction)
			return FALSE;
	}

	if (ItemRepairInfo.lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter	*pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(ItemRepairInfo.lCID);
	if (!pcsCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (ItemRepairInfo.cOperation) {
	case AGPMEVENT_ITEMREPAIR_OPERATION_REPAIR_REQ:
		{
			EnumCallback(AGPMEVENT_ITEMREPAIR_CB_REPAIR_REQ, pcsCharacter, &ItemRepairInfo);
		}
		break;

	case AGPMEVENT_ITEMREPAIR_OPERATION_REPAIR_ACK:
		{
			EnumCallback(AGPMEVENT_ITEMREPAIR_CB_REPAIR_ACK, pcsCharacter, &ItemRepairInfo);
		}
		break;

	case AGPMEVENT_ITEMREPAIR_OPERATION_REPAIR_RESULT:
		{
			EnumCallback(AGPMEVENT_ITEMREPAIR_CB_REPAIR_RESULT, pcsCharacter, &ItemRepairInfo);
		}
		break;

	case AGPMEVENT_ITEMREPAIR_OPERATION_GRANT:
		{
			if (NULL == pcsEvent)
				return FALSE;
			EnumCallback(AGPMEVENT_ITEMREPAIR_CB_GRANT, pcsEvent, pcsCharacter);
		}
		break;

	case AGPMEVENT_ITEMREPAIR_OPERATION_EVENT_REQ:
		{
			OnOperationEventReq(pcsEvent, pcsCharacter);
		}
		break;

	default:
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmEventItemRepair::SetCallbackRepairReq(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_ITEMREPAIR_CB_REPAIR_REQ, pfCallback, pClass);
}

BOOL AgpmEventItemRepair::SetCallbackRepairAck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_ITEMREPAIR_CB_REPAIR_ACK, pfCallback, pClass);
}

BOOL AgpmEventItemRepair::SetCallbackRepairResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_ITEMREPAIR_CB_REPAIR_RESULT, pfCallback, pClass);
}

BOOL AgpmEventItemRepair::SetCallbackEventReq(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_ITEMREPAIR_CB_EVENT_REQ, pfCallback, pClass);
}

BOOL AgpmEventItemRepair::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_ITEMREPAIR_CB_GRANT, pfCallback, pClass);
}

INT64 AgpmEventItemRepair::GetItemRepairPrice(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter, INT64 *plTax)
{
	if (!pcsItem)
		return (-1);

	// 수리비용 산출에 필요한 데이타들을 얻어온다.
	INT32	lItemDurability	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lItemDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
	
	INT32	lMaxItemDurability	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lMaxItemDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);

	// 수리할 필요가 있는지 검사한다.
	if (lItemDurability == lMaxItemDurability)
	{
		return 0;		// 내구도가 최대치다. 수리할 필요 없다.
	}

	FLOAT	fItemPrice		= m_pcsAgpmItem->GetNPCPrice(pcsItem);

	if (fItemPrice < 0)
	{
		return (-1);
	}

	m_pcsAgpmCharacter->GetTaxRatio(pcsCharacter);

	// 수리비용을 산출한다.
	INT64 llItemPrice =  (INT64) ((fItemPrice / (FLOAT) lMaxItemDurability) * (lMaxItemDurability - lItemDurability) * 0.5);
	
	// apply tax
	INT32 lTaxRatio = m_pcsAgpmCharacter->GetTaxRatio(pcsCharacter);
	INT64 llTax = 0;
	if (lTaxRatio > 0)
	{
		llTax = (llItemPrice * lTaxRatio) / 100;
	}
	llItemPrice = llItemPrice + llTax;	
	if (plTax)
		*plTax = llTax;

	return llItemPrice;	
}

BOOL AgpmEventItemRepair::CBEmbeddedMakeEventPacket(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventItemRepair	*pThis					= (AgpmEventItemRepair *)pClass;
	ApdEvent			*pcsEvent				= (ApdEvent *)pData;
	PVOID				*ppvPacketCustomData	= (PVOID *)pCustData;

	*ppvPacketCustomData	= pThis->MakePacketEventData(pcsEvent);	
	return TRUE;
}

BOOL AgpmEventItemRepair::CBEmbeddedReceiveEventPacket(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventItemRepair	*pThis					= (AgpmEventItemRepair *)pClass;
	ApdEvent			*pcsEvent				= (ApdEvent *)pData;
	PVOID				pvPacketCustomData		= (PVOID)pCustData;

	return pThis->ParseEventDataPacket(pcsEvent, pvPacketCustomData);
}

PVOID AgpmEventItemRepair::MakePacketEventData(ApdEvent *pcsEvent)
{
	if (!pcsEvent)
		return NULL;

	return m_csPacketEventData.MakePacket(FALSE, NULL, 0, 0);	
}

BOOL AgpmEventItemRepair::ParseEventDataPacket(ApdEvent *pcsEvent, PVOID pvPacketCustomData)
{
	if (!pcsEvent || !pvPacketCustomData)
		return FALSE;

	// 현재는 부가정보가 필요없다.
//	m_csPacketEventData.GetField(FALSE, pvPacketCustomData, 0, 0);

	return TRUE;	
}

BOOL AgpmEventItemRepair::OnOperationEventReq(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
{
	if(!pApdEvent || !pAgpdCharacter)
		return FALSE;

	AuPOS stTargetPos;
	memset(&stTargetPos, 0, sizeof(stTargetPos));
	
	if (m_pcsApmEventManager->CheckValidRange(pApdEvent, &pAgpdCharacter->m_stPos, AGPMEVENT_ITEMREPAIR_MAX_USE_RANGE, &stTargetPos))
	{
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
		if (pAgpdCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);

		EnumCallback(AGPMEVENT_ITEMREPAIR_CB_EVENT_REQ, pApdEvent, pAgpdCharacter);
	}
	else
	{
		// move to <stTargetPos>
		pAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_ITEMREPAIR;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pApdEvent->m_pcsSource->m_eType;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pApdEvent->m_pcsSource->m_lID;

		m_pcsAgpmCharacter->MoveCharacter(pAgpdCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgpmEventItemRepair::CBActionItemRepair(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpmEventItemRepair *pThis = (AgpmEventItemRepair *) pClass;
	AgpdCharacterAction *pstAction = (AgpdCharacterAction *) pCustData;

	if (pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
		return FALSE;

	pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	if(pAgpdCharacter->m_bMove)
		pThis->m_pcsAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);
	
	ApdEvent *pApdEvent = pThis->m_pcsApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType, pstAction->m_csTargetBase.m_lID, APDEVENT_FUNCTION_PRODUCT);

	pThis->EnumCallback(AGPMEVENT_ITEMREPAIR_CB_EVENT_REQ, pApdEvent, pAgpdCharacter);
	
	return TRUE;
}

PVOID AgpmEventItemRepair::MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_ITEMREPAIR_OPERATION_EVENT_REQ;
	
	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_ITEMREPAIR_PACKET_TYPE,
										   &cOperation,
										   pvPacketBase,
										   &lCID,
										   NULL,
										   0,
										   0
										   );
	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmEventItemRepair::MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_ITEMREPAIR_OPERATION_GRANT;
	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_ITEMREPAIR_PACKET_TYPE,
										   &cOperation,
										   pvPacketBase,
										   &lCID,
										   NULL,
										   0,
										   0
										   );
	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}