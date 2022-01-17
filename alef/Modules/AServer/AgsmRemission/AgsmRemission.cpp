/*===========================================================================

		AgsmRemission.cpp

===========================================================================*/


#include "AgsmRemission.h"


/********************************************************/
/*		The Implementation of AgsmRemission class		*/
/********************************************************/
//
AgsmRemission::AgsmRemission()
	{
	SetModuleName("AgsmRemission");
	SetModuleType(APMODULE_TYPE_SERVER);

	m_pAgpmCharacter = NULL;
	m_pAgpmItem = NULL;
	m_pAgpmRemission = NULL;
	m_pAgpmGrid = NULL;
	m_pAgpmLog = NULL;
	
	m_pAgsmCharacter = NULL;
	
	m_pAgsmItem = NULL;
	}


AgsmRemission::~AgsmRemission()
	{
	}




//	ApModule inherited
//=======================================
//
BOOL AgsmRemission::OnAddModule()
	{
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmItem = (AgpmItem *) GetModule("AgpmItem");	
	m_pAgpmRemission = (AgpmRemission *) GetModule("AgpmRemission");
	m_pAgpmGrid = (AgpmGrid *) GetModule("AgpmGrid");
	m_pAgpmLog = (AgpmLog *) GetModule("AgpmLog");
	
	m_pAgsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");

	m_pAgsmItem = (AgsmItem *) GetModule("AgsmItem");

	if (!m_pAgpmCharacter || !m_pAgpmItem || !m_pAgpmRemission || !m_pAgsmCharacter || !m_pAgpmGrid)
		return FALSE;

	if (!m_pAgpmRemission->SetCallbackRequest(CBEventRequest, this))
		return FALSE;

	if (!m_pAgpmRemission->SetCallbackRemit(CBRemit, this))
		return FALSE;

	return TRUE;
	}




//	Operation
//==================================================
//
BOOL AgsmRemission::OnRemit(AgpdCharacter *pAgpdCharacter, INT8 cType)
	{
	INT32 lResult = AGPMREMISSION_RESULT_UNKNOWN;

	// remitable time check	
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;
	
	if (m_pAgpmCharacter->GetRemainedMurdererTime(pAgpdCharacter) > 0)
		{
		return SendRemitResult(pAgpdCharacter, cType, AGPMREMISSION_RESULT_FAIL_NOT_YET_TIME);
		}

	// cause remitable time reside on server, we must test it and send result code
	if (AGPMREMISSION_TYPE_TEST == cType)
		return SendRemitResult(pAgpdCharacter, cType, AGPMREMISSION_RESULT_REMITABLE);
	
	// validate
	if (m_pAgpmRemission->IsValidStatus(pAgpdCharacter, cType, &lResult))
		{
		if (_Pay(pAgpdCharacter, cType))
			{
			m_pAgpmCharacter->SubMurdererPoint(pAgpdCharacter, AGPMREMISSION_REMIT_POINT_PER_TIME);
			WriteRemitLog(pAgpdCharacter, cType);
			}
		else
			{
			lResult = AGPMREMISSION_RESULT_UNKNOWN;
			}
		}

	return SendRemitResult(pAgpdCharacter, cType, lResult);
	}




//	Packet
//===================================
//
BOOL AgsmRemission::SendRemitResult(AgpdCharacter *pAgpdCharacter, INT8 cType, INT32 lResult)
	{
	BOOL bResult = FALSE;
	INT8 cOperation = AGPMREMISSION_OPERATION_REMIT;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmRemission->m_csPacketRemit.MakePacket(FALSE, &nPacketLength, 0,
															&cType,
															&lResult
															);


	if (!pvPacketEmb || nPacketLength < 1)
		return bResult;
	
	PVOID pvPacket = m_pAgpmRemission->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMREMISSION_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketEmb
														);
	m_pAgpmRemission->m_csPacketRemit.FreePacket(pvPacketEmb);
	
	if (!pvPacket || nPacketLength < 1)
		return bResult;	

	bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter));
	
	m_pAgpmRemission->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
	}




//	Callback
//====================================================
//
BOOL AgsmRemission::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRemission *pThis = (AgsmRemission *)pClass;
	ApdEvent *pApdEvent = (ApdEvent *) pData;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pAgpmRemission->MakePacketEventGrant(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter));
	pThis->m_pAgpmRemission->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgsmRemission::CBRemit(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRemission *pThis = (AgsmRemission *) pClass;
	AgpdRemitArg *pAgpdRemitArg = (AgpdRemitArg *) pData;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->OnRemit(pAgpdCharacter, pAgpdRemitArg->m_cType);
	}




//	ETC
//======================================================
//
BOOL AgsmRemission::_Pay(AgpdCharacter *pAgpdCharacter, INT8 cType)
	{
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	switch (cType)
		{
		case AGPMREMISSION_TYPE_GHELD :
			{
			INT64 llMoney = 0;
			m_pAgpmCharacter->GetMoney(pAgpdCharacter, &llMoney);
			
			if (AGPMREMISSION_PAY_GHELD <= llMoney
				&& m_pAgpmCharacter->SubMoney(pAgpdCharacter, AGPMREMISSION_PAY_GHELD))
				return TRUE;
			}
			break;
			
		case AGPMREMISSION_TYPE_SKELETON :
			if (_RemoveSourceItem(pAgpdCharacter, AGPMREMISSION_PAY_SKELETON_TID, AGPMREMISSION_PAY_SKELETON_QTY))
				return TRUE;
			
			break;
			
		case AGPMREMISSION_TYPE_INDULGENCE :
			if (_RemoveSourceItem(pAgpdCharacter, 0, AGPMREMISSION_PAY_INDULGENCE_QTY, AGPMITEM_TYPE_OTHER, AGPMITEM_OTHER_TYPE_REMISSION))
				return TRUE;
			
			break;
			
		default :
			break;
		}

	return FALSE;	
	}

// 2007.09.28. steeple
// 캐쉬 아이템 에서도 체크해야 한다.
BOOL AgsmRemission::_RemoveSourceItem(AgpdCharacter *pAgpdCharacter, INT32 lItemTID, INT32 lCount, INT32 lItemType, INT32 lItemOtherType)
{
	INT32	lIndex	= 0;
	INT32	lRemained = lCount;
	INT32	lRemove = 0;

	AgpdGridItem *pAgpdGridItem;
	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);

	// 2007.10.09. steeple
	// 아래 지우는 알고리즘이 좀 이상하다.
	// 만약에 lCount 가 값이 바뀐다면 알고리즘을 다시 살펴봐야 할 듯 하다.
	//
	// 인벤토리 개수를 먼저 새야 한다. 전체 합이 lCount 보다 작으면 하질 않는다.
	INT32	lCountFromInventory = m_pAgpmRemission->GetRemissionItemCountFromInventory(pAgpdCharacter, lItemTID, lItemType, lItemOtherType);

	for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex);
		 pAgpdGridItem;
		 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex))
	{
		if(lCountFromInventory < lRemained)
			break;

		AgpdItem* pAgpdItem = NULL;
		if(lItemTID != 0)
		{
			if (lItemTID == pAgpdGridItem->m_lItemTID)
			{
				pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
				if (!pAgpdItem || AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus)
					continue;
			}
		}
		else if(lItemType != 0 && lItemOtherType != 0)
		{
			pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
			if (!pAgpdItem || AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus)
				continue;

			if(pAgpdItem->m_pcsItemTemplate->m_nType != lItemType ||
				((AgpdItemTemplateOther*)pAgpdItem->m_pcsItemTemplate)->m_eOtherItemType != lItemOtherType)
				continue;
		}

		if(!pAgpdItem)
			continue;

		if (pAgpdItem->m_pcsItemTemplate->m_bStackable)
			lRemove = min(pAgpdItem->m_nCount, lRemained);
		else
			lRemove = 1;
		
		m_pAgpmItem->SubItemStackCount(pAgpdItem, lRemove);
		lRemained -= lRemove;
		lCountFromInventory -= lRemove;
		
		if (0 >= lRemained)
			return TRUE;
	}

	// 2007.09.28. steeple
	// Cash Inven 도 확인한다.
	// Cash Item 인 경우에는 한장이 많은 점수를 깍을 수 있다.
	lIndex = lRemove = 0;
	if(lRemained % AGPMREMISSION_PAY_INDULGENCE_QTY == 0)
		lRemained = (INT32)((double)lRemained / (double)AGPMREMISSION_PAY_INDULGENCE_QTY);
	else
		lRemained = (INT32)((double)lRemained / (double)AGPMREMISSION_PAY_INDULGENCE_QTY) + 1;

	for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csCashInventoryGrid, &lIndex);
		 pAgpdGridItem;
		 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csCashInventoryGrid, &lIndex))
	{
		AgpdItem* pAgpdItem = NULL;
		if (lItemTID != 0)
		{
			if (lItemTID == pAgpdGridItem->m_lItemTID)
			{
				pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
				if (!pAgpdItem || AGPDITEM_STATUS_CASH_INVENTORY != pAgpdItem->m_eStatus)
					continue;
			}
		}
		else if(lItemType != 0 && lItemOtherType != 0)
		{
			pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
			if (!pAgpdItem || AGPDITEM_STATUS_CASH_INVENTORY != pAgpdItem->m_eStatus)
				continue;

			if(pAgpdItem->m_pcsItemTemplate->m_nType != lItemType ||
				((AgpdItemTemplateOther*)pAgpdItem->m_pcsItemTemplate)->m_eOtherItemType != lItemOtherType)
				continue;
		}
		
		if(!pAgpdItem)
			continue;

		if (pAgpdItem->m_pcsItemTemplate->m_bStackable)
			lRemove = min(pAgpdItem->m_nCount, lRemained);
		else
			lRemove = 1;
		
		m_pAgpmItem->SubItemStackCount(pAgpdItem, lRemove);
		lRemained = lRemained - lRemove;
		
		if (0 >= lRemained)
			return TRUE;
	}

	return FALSE;
}


BOOL AgsmRemission::WriteRemitLog(AgpdCharacter *pAgpdCharacter, INT8 cType)
	{
	if (!m_pAgpmLog || !pAgpdCharacter)
		return FALSE;
		
	AgsdCharacter* pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if(!pAgsdCharacter)
		return FALSE;

	INT32 lLevel = m_pAgpmCharacter->GetLevel(pAgpdCharacter);
	
	return m_pAgpmLog->WriteLog_Remission(0, &pAgsdCharacter->m_strIPAddress[0], pAgsdCharacter->m_szAccountID,
						pAgsdCharacter->m_szServerName, pAgpdCharacter->m_szID,
						((AgpdCharacterTemplate*)pAgpdCharacter->m_pcsCharacterTemplate)->m_lID,
						lLevel, m_pAgpmCharacter->GetExp(pAgpdCharacter), pAgpdCharacter->m_llMoney, pAgpdCharacter->m_llBankMoney,
						AGPMREMISSION_REMIT_POINT_PER_TIME, NULL
						);	
	}




