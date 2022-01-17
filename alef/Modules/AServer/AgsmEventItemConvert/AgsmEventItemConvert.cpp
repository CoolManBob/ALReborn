#include "AgsmEventItemConvert.h"

AgsmEventItemConvert::AgsmEventItemConvert()
{
	SetModuleName("AgsmEventItemConvert");
}

AgsmEventItemConvert::~AgsmEventItemConvert()
{
}

BOOL AgsmEventItemConvert::OnAddModule()
{
	m_pcsAgpmEventItemConvert	= (AgpmEventItemConvert *)	GetModule("AgpmEventItemConvert");
	m_pcsAgsmCharacter			= (AgsmCharacter *)			GetModule("AgsmCharacter");

	if (!m_pcsAgpmEventItemConvert ||
		!m_pcsAgsmCharacter)
		return FALSE;

	if (!m_pcsAgpmEventItemConvert->SetCallbackRequest(CBItemConvertRequest, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmEventItemConvert::CBItemConvertRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventItemConvert	*pThis				= (AgsmEventItemConvert *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pData;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pCustData;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventItemConvert->MakePacketGrant(pcsCharacter->m_lID, pcsEvent, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	pThis->m_pcsAgpmEventItemConvert->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}