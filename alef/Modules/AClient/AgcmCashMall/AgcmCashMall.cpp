#include "AgcmCashMall.h"

AgcmCashMall::AgcmCashMall()
{
	SetModuleName("AgcmCashMall");
}

BOOL AgcmCashMall::OnAddModule()
{
	m_pcsAgpmFactors	= (AgpmFactors *)	GetModule("AgpmFactors");
	m_pcsAgpmItem		= (AgpmItem *)		GetModule("AgpmItem");
	m_pcsAgpmCashMall	= (AgpmCashMall *)	GetModule("AgpmCashMall");
	m_pcsAgcmCharacter	= (AgcmCharacter *)	GetModule("AgcmCharacter");
	m_pcsAgcmItem		= (AgcmItem *)		GetModule("AgcmItem");

	if (!m_pcsAgpmFactors ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmCashMall ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmItem)
		return FALSE;

	if (!m_pcsAgpmCashMall->SetCallbackUpdateMallList(CBUpdateMallList, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmCashMall::OnDestroy()
{
	DestroyAllDummyItem();

	return TRUE;
}

BOOL AgcmCashMall::SendPacketRequestMallList(INT32 lTab)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmCashMall->MakePacketRequestMallInfo(m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength, lTab);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacket(pvPacket, nPacketLength);
}

BOOL AgcmCashMall::SendPacketBuyProduct(INT32 lProductID)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmCashMall->MakePacketBuyProduct(lProductID, m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacket(pvPacket, nPacketLength);
}

BOOL AgcmCashMall::SendPacketRefreshCash()
{
	AgpdCashMall	*pcsAttachData	= m_pcsAgpmCashMall->GetADCharacter(m_pcsAgcmCharacter->GetSelfCharacter());
	if (!pcsAttachData)
		return FALSE;

	if (pcsAttachData->m_ulLastRefreshCashTimeMsec + AGPMCASHMALL_MIN_INTERVAL_REFRESH_CASH > GetClockCount())
		return TRUE;

	pcsAttachData->m_ulLastRefreshCashTimeMsec	= GetClockCount() + AGPMCASHMALL_MIN_INTERVAL_REFRESH_CASH;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmCashMall->MakePacketRefreshCash(m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacket(pvPacket, nPacketLength);
}

BOOL AgcmCashMall::SendPacketCheckListVersion()
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmCashMall->MakePacketCheckListVersion(m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacket(pvPacket, nPacketLength);
}

AgpdItem* AgcmCashMall::CreateDummyItem(INT32 lTID, INT32 lCount)
{
	AgpdItemTemplate *pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(lTID);
	if (!pcsItemTemplate)
		return NULL;

	//스태커블 아이템인데 count가 0보다 작거나 최대 숫자보다 크다?
	if (pcsItemTemplate->m_bStackable)
	{
		if ((lCount <= 0) && (pcsItemTemplate->m_lMaxStackableCount < lCount))
			return NULL;
	}

	AgpdItem *pcsItem = m_pcsAgpmItem->CreateItem();
	if (!pcsItem)
		return NULL;

	pcsItem->m_lID = AP_INVALID_IID;
	pcsItem->m_lTID = lTID;

	if (pcsItemTemplate->m_bStackable)
	{	
		if (pcsItemTemplate->m_lMaxStackableCount < lCount)
			pcsItem->m_nCount	= pcsItemTemplate->m_lMaxStackableCount;
		else
			pcsItem->m_nCount	= lCount;
	}
	else
		pcsItem->m_nCount	= 0;

	pcsItem->m_pcsItemTemplate = pcsItemTemplate;

	m_pcsAgpmFactors->CopyFactor(&pcsItem->m_csFactor, &pcsItemTemplate->m_csFactor, FALSE);

	m_pcsAgpmFactors->CopyFactor(&pcsItem->m_csRestrictFactor, &pcsItemTemplate->m_csRestrictFactor, FALSE);

	pcsItem->m_lStatusFlag	= pcsItemTemplate->m_lStatusFlag;

	m_pcsAgcmItem->SetGridItemAttachedTexture(pcsItemTemplate);
	m_pcsAgcmItem->SetGridItemAttachedTexture(pcsItem);

	pcsItem->m_pcsGridItem->m_eType	= AGPDGRID_ITEM_TYPE_ITEM;

	return pcsItem;
}

BOOL AgcmCashMall::DestroyDummyItem(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	m_pcsAgcmItem->ReleaseGridItemAttachedTexture(pcsItem->m_pcsItemTemplate);

	return m_pcsAgpmItem->DestroyItem(pcsItem);
}

BOOL AgcmCashMall::DestroyAllDummyItem()
{
	for (int i = 0; i < m_pcsAgpmCashMall->GetCashMallInfo()->m_lNumMallItemInfo; ++i)
	{
		DestroyDummyItem(m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallItemInfo[i].m_pcsItem);
	}

	return TRUE;
}

BOOL AgcmCashMall::CBUpdateMallList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmCashMall	*pThis	= (AgcmCashMall *)	pClass;

	pThis->DestroyAllDummyItem();

	for (int i = 0; i < pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_lNumMallItemInfo; ++i)
	{
		pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallItemInfo[i].m_pcsItem
			= pThis->CreateDummyItem(pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallItemInfo[i].m_alItemTID[0],
									 pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallItemInfo[i].m_alItemQty[0]);

		if (pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallItemInfo[i].m_pcsItem)
		{
			pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallItemInfo[i].m_pcsItem->m_ulCID	= 
				pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallItemInfo[i].m_lProductID;
	
			pThis->m_pcsAgpmItem->SetItemPrice(pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallItemInfo[i].m_pcsItem,
											   pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallItemInfo[i].m_llPrice);
		}
	}

	return TRUE;
}