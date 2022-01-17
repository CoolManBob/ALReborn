#include "AgcmItemConvert.h"

AgcmItemConvert::AgcmItemConvert()
{
	SetModuleName("AgcmItemConvert");
}

AgcmItemConvert::~AgcmItemConvert()
{
}

BOOL AgcmItemConvert::OnAddModule()
{
	m_pcsAgpmItemConvert	= (AgpmItemConvert *)	GetModule("AgpmItemConvert");

	if (!m_pcsAgpmItemConvert)
		return FALSE;

	if (!m_pcsAgpmItemConvert->SetCallbackResultPhysicalConvert(CBResultPhysicalConvert, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackResultSocketConvert(CBResultSocketConvert, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackResultSocketConvert(CBResultSpiritStoneConvert, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackResultRuneConvert(CBResultRuneConvert, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmItemConvert::SendRequestPhysicalConvert(INT32 lCID, INT32 lIID, INT32 lCatalystIID)
{
	INT16	nPacketLength	= 0;

	PVOID	pvPacket		= m_pcsAgpmItemConvert->MakePacketRequestPhysicalConvert(lCID, lIID, lCatalystIID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmItemConvert::SendRequestSocketAdd(INT32 lCID, INT32 lIID)
{
	INT16	nPacketLength	= 0;

	PVOID	pvPacket		= m_pcsAgpmItemConvert->MakePacketRequestSocketAdd(lCID, lIID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmItemConvert::SendRequestRuneConvert(INT32 lCID, INT32 lIID, INT32 lRuneID)
{
	INT16	nPacketLength	= 0;

	PVOID	pvPacket		= m_pcsAgpmItemConvert->MakePacketRequestRuneConvert(lCID, lIID, lRuneID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmItemConvert::SendCheckCashRuneConvert(INT32 lCID, INT32 lIID, INT32 lRuneID)
{
	INT16	nPacketLength	= 0;

	PVOID	pvPacket		= m_pcsAgpmItemConvert->MakePacketCheckCashRuneConvert(lCID, lIID, lRuneID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmItemConvert::SendSocketInitialize(INT32 lCID, INT32 lIID, INT32 lCatalystIID)
{
	INT16	nPacketLength	= 0;

	PVOID	pvPacket		= m_pcsAgpmItemConvert->MakePacketSocketInitialize(lCID, lIID, lCatalystIID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

AgpdItemConvertResult AgcmItemConvert::IsPhysicalConvertable(AgpdItem *pcsItem, AgpdItem *pcsCatalystItem)
{
	if (!pcsItem || !pcsCatalystItem)
		return AGPDITEMCONVERT_RESULT_FAILED;

	// 개조가 가능한 넘들인지 검사한다.
	AgpdItemConvertResult	eCheckResult	= m_pcsAgpmItemConvert->IsPhysicalConvertable(pcsItem);
	if (eCheckResult != AGPDITEMCONVERT_RESULT_SUCCESS)
		return eCheckResult;

	eCheckResult	= m_pcsAgpmItemConvert->IsValidCatalyst(pcsCatalystItem);
	if (eCheckResult != AGPDITEMCONVERT_RESULT_SUCCESS)
		return eCheckResult;

	return AGPDITEMCONVERT_RESULT_SUCCESS;
}

AgpdItemConvertSocketResult AgcmItemConvert::IsSocketConvertable(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

	AgpdItemConvertSocketResult	eCheckResult	= m_pcsAgpmItemConvert->IsSocketConvertable(pcsItem);
	if (eCheckResult != AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS)
		return eCheckResult;

	return AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS;
}

AgpdItemConvertRuneResult AgcmItemConvert::IsRuneConvertable(AgpdItem *pcsItem, AgpdItem *pcsRuneItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate || !pcsRuneItem)
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	AgpdItemConvertRuneResult	eCheckResult	= m_pcsAgpmItemConvert->IsValidRuneItem(pcsRuneItem);
	if (eCheckResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return eCheckResult;

	eCheckResult	= m_pcsAgpmItemConvert->IsRuneConvertable(pcsItem);
	if (eCheckResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return eCheckResult;

	eCheckResult	= m_pcsAgpmItemConvert->IsProperPart(pcsItem, pcsRuneItem);
	if (eCheckResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return eCheckResult;

	eCheckResult	= m_pcsAgpmItemConvert->IsProperLevel(pcsItem, pcsRuneItem);
	if (eCheckResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return eCheckResult;

	return AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;
}

AgpdItemConvertResult AgcmItemConvert::PhysicalConvert(AgpdItem *pcsItem, AgpdItem *pcsCatalystItem)
{
	if (!pcsItem || !pcsCatalystItem)
		return AGPDITEMCONVERT_RESULT_FAILED;

	// 개조가 가능한 넘들인지 검사한다.
	AgpdItemConvertResult	eCheckResult	= IsPhysicalConvertable(pcsItem, pcsCatalystItem);
	if (eCheckResult != AGPDITEMCONVERT_RESULT_SUCCESS)
		return eCheckResult;

	if (!SendRequestPhysicalConvert(pcsItem->m_ulCID, pcsItem->m_lID, pcsCatalystItem->m_lID))
		return AGPDITEMCONVERT_RESULT_FAILED;

	return AGPDITEMCONVERT_RESULT_SUCCESS;
}

AgpdItemConvertSocketResult AgcmItemConvert::SocketConvert(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

	AgpdItemConvertSocketResult	eCheckResult	= IsSocketConvertable(pcsItem);
	if (eCheckResult != AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS)
		return eCheckResult;

	if (!SendRequestSocketAdd(pcsItem->m_ulCID, pcsItem->m_lID))
		return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

	return AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS;
}

AgpdItemConvertRuneResult AgcmItemConvert::RuneConvert(AgpdItem *pcsItem, AgpdItem *pcsRuneItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate || !pcsRuneItem)
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	AgpdItemConvertRuneResult	eCheckResult	= IsRuneConvertable(pcsItem, pcsRuneItem);
	if (eCheckResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return eCheckResult;

	if (!SendRequestRuneConvert(pcsItem->m_ulCID, pcsItem->m_lID, pcsRuneItem->m_lID))
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	return AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;
}

BOOL AgcmItemConvert::CBResultPhysicalConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmItemConvert			*pThis				= (AgcmItemConvert *)		pClass;
	AgpdItem				*pcsItem			= (AgpdItem *)				pData;
	AgpdItemConvertResult	*peResult			= (AgpdItemConvertResult *)	pCustData;

	return TRUE;
}

BOOL AgcmItemConvert::CBResultSocketConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmItemConvert				*pThis				= (AgcmItemConvert *)				pClass;
	AgpdItem					*pcsItem			= (AgpdItem *)						pData;
	AgpdItemConvertSocketResult	*peResult			= (AgpdItemConvertSocketResult *)	pCustData;

	return TRUE;
}

BOOL AgcmItemConvert::CBResultSpiritStoneConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData || !pData)
		return FALSE;

	AgcmItemConvert						*pThis				= (AgcmItemConvert *)					pClass;
	AgpdItem							*pcsItem			= (AgpdItem *)							pData;
	AgpdItemConvertSpiritStoneResult	*peResult			= (AgpdItemConvertSpiritStoneResult *)	pCustData;

	return TRUE;
}

BOOL AgcmItemConvert::CBResultRuneConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmItemConvert				*pThis				= (AgcmItemConvert *)			pClass;
	AgpdItem					*pcsItem			= (AgpdItem *)					pData;
	AgpdItemConvertRuneResult	*peResult			= (AgpdItemConvertRuneResult *)	pCustData;

	return TRUE;
}
