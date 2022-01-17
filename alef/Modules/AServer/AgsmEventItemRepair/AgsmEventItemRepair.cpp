//	AgsmEventItemRepair	module source file
//		- item repair event module (server side)
//			(클라이언트의 수리 요청을 실제 처리한다.)
///////////////////////////////////////////////////////////

#include "AgsmEventItemRepair.h"

AgsmEventItemRepair::AgsmEventItemRepair()
{
	SetModuleName("AgsmEventItemRepair");

	m_pcsAgpmLog = NULL;
}

AgsmEventItemRepair::~AgsmEventItemRepair()
{
}

BOOL AgsmEventItemRepair::OnAddModule()
{
	m_pcsAgpmFactors			= (AgpmFactors *)			GetModule("AgpmFactors");
	m_pcsAgpmGrid				= (AgpmGrid *)				GetModule("AgpmGrid");
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmItem				= (AgpmItem *)				GetModule("AgpmItem");
	m_pcsAgpmLog				= (AgpmLog *)				GetModule("AgpmLog");

	m_pcsAgpmEventItemRepair	= (AgpmEventItemRepair *)	GetModule("AgpmEventItemRepair");

	m_pcsAgsmCharacter			= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pcsAgsmItem				= (AgsmItem *)				GetModule("AgsmItem");

	if (!m_pcsAgpmFactors ||
		!m_pcsAgpmGrid ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmEventItemRepair ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmItem)
		return FALSE;
    
	if (!m_pcsAgpmEventItemRepair->SetCallbackEventReq(CBItemRepairEventReq, this))
		return FALSE;

	if (!m_pcsAgpmEventItemRepair->SetCallbackRepairReq(CBRepairItemReq, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmEventItemRepair::OnInit()
{
	return TRUE;
}

BOOL AgsmEventItemRepair::OnDestroy()
{
	return TRUE;
}

BOOL AgsmEventItemRepair::CBItemRepairEventReq(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pApdEvent = (ApdEvent *) pData;
	AgsmEventItemRepair	*pThis = (AgsmEventItemRepair *) pClass;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventItemRepair->MakePacketEventGrant(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	BOOL bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pAgpdCharacter));
	pThis->m_pcsAgpmEventItemRepair->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmEventItemRepair::CBRepairItemReq(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter		*pcsCharacter = (AgpdCharacter *) pData;
	AgsmEventItemRepair	*pThis = (AgsmEventItemRepair *) pClass;
	AgpdItemRepair		*pcsRepairInfo = (AgpdItemRepair *) pCustData;

	return pThis->ItemRepairItemReq(pcsCharacter, pcsRepairInfo);
}

BOOL AgsmEventItemRepair::ItemRepairItemReq(AgpdCharacter *pcsCharacter, AgpdItemRepair *pcsRepairInfo)
{
	INT64 lRepairCost = 0;
	INT64 llTax = 0, llTotalTax = 0;
	AgpdItem *pcsItem = NULL;

	// 돈이 충분한지 확인
	INT32 lCount = 0;
	for (int i = 0; i < ITEM_REPAIR_GRID_MAX; ++i)
	{
		if (0 == pcsRepairInfo->ItemIDs[i])
			continue;

		pcsItem = m_pcsAgpmItem->GetItem(pcsRepairInfo->ItemIDs[i]);
		if (NULL == pcsItem)
			continue;

		++lCount;
		lRepairCost += m_pcsAgpmEventItemRepair->GetItemRepairPrice(pcsItem, pcsCharacter, &llTax);
		llTotalTax += llTax;
	}

	if (0 == lCount)
		return FALSE;

	if (pcsCharacter->m_llMoney < lRepairCost)
		return SendPacketItemRepair(AGPMEVENT_ITEMREPAIR_OPERATION_REPAIR_ACK, pcsCharacter->m_lID, AGPMEVENT_ITEMREPAIR_RESULTCODE_NOT_ENOUGH_MONEY);

	// 아이템 수리
	INT32 lMaxDurability, lCurrentDurability;
	for (int i = 0; i < ITEM_REPAIR_GRID_MAX; ++i)
	{
		if (0 == pcsRepairInfo->ItemIDs[i])
			continue;

		pcsItem = m_pcsAgpmItem->GetItem(pcsRepairInfo->ItemIDs[i]);
		if (NULL == pcsItem)
			continue;

		m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);
		m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lCurrentDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);

		m_pcsAgsmItem->UpdateItemDurability(pcsItem, lMaxDurability - lCurrentDurability);
		
		CHAR szTotalCost[AGPDLOG_MAX_STR_ID+1];
		ZeroMemory(szTotalCost, sizeof(szTotalCost));
		_stprintf(szTotalCost, "%d", (INT32) lRepairCost);
		m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_REPAIR, pcsCharacter->m_lID, pcsItem, pcsItem->m_nCount ? 
					pcsItem->m_nCount : 1, (INT32)m_pcsAgpmEventItemRepair->GetItemRepairPrice(pcsItem, pcsCharacter), szTotalCost);
	}

	m_pcsAgpmCharacter->SubMoney(pcsCharacter, (INT32)lRepairCost);
	m_pcsAgpmCharacter->PayTax(pcsCharacter, llTotalTax);
	m_pcsAgpmCharacter->UpdateMoney(pcsCharacter);

	return SendPacketItemRepair(AGPMEVENT_ITEMREPAIR_OPERATION_REPAIR_ACK, pcsCharacter->m_lID, AGPMEVENT_ITEMREPAIR_RESULTCODE_SUCCESS);
}

BOOL AgsmEventItemRepair::SendPacketItemRepair(EnumAgpmEventItemRepairOperation eOperation, INT32 lCID, EnumAgpmEventItemRepairResultCode eResult)
{
	INT16 nPacketLength = 0;

	PVOID pvPacket = m_pcsAgpmEventItemRepair->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMEVENT_ITEMREPAIR_PACKET_TYPE,
																		&eOperation,
																		NULL,
																		&lCID,
																		NULL,
																		0,
																		&eResult
																		);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(lCID));
	m_pcsAgpmEventItemRepair->m_csPacket.FreePacket(pvPacket);

	return bResult;
}	