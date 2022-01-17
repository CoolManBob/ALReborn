#include "AgcmEventItemConvert.h"

#include "ApMemoryTracker.h"

AgcmEventItemConvert::AgcmEventItemConvert()
{
	SetModuleName("AgcmEventItemConvert");
}

AgcmEventItemConvert::~AgcmEventItemConvert()
{
}

BOOL AgcmEventItemConvert::OnAddModule()
{
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmItemConvert		= (AgpmItemConvert *)		GetModule("AgpmItemConvert");
	m_pcsAgpmEventItemConvert	= (AgpmEventItemConvert *)	GetModule("AgpmEventItemConvert");

	m_pcsAgcmCharacter			= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pcsAgcmEventManager		= (AgcmEventManager *)		GetModule("AgcmEventManager");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmItemConvert ||
		!m_pcsAgpmEventItemConvert ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmEventManager)
		return FALSE;

	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_ITEMCONVERT, CBEventItemConvert, this))
		return FALSE;

	if (!m_pcsAgpmEventItemConvert->SetCallbackGrant(CBReceiveGrant, this))
		return FALSE;
	if (!m_pcsAgpmEventItemConvert->SetCallbackReject(CBReceiveReject, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmEventItemConvert::CBEventItemConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventItemConvert		*pThis			= (AgcmEventItemConvert *)			pClass;
	ApdEvent					*pcsEvent		= (ApdEvent *)						pData;
	ApBase						*pcsGenerator	= (ApBase *)						pCustData;

	if (pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventItemConvert->MakePacketRequest(pcsGenerator->m_lID, pcsEvent, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength);

	pThis->m_pcsAgpmEventItemConvert->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmEventItemConvert::CBReceiveGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventItemConvert	*pThis				= (AgcmEventItemConvert *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pData;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pCustData;

	return pThis->EnumCallback(AGCMEVENT_ITEMCONVERT_CB_RECEIVE_GRANT, pcsEvent, pcsCharacter);
}

BOOL AgcmEventItemConvert::CBReceiveReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventItemConvert	*pThis				= (AgcmEventItemConvert *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pData;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pCustData;

	return TRUE;
}

BOOL AgcmEventItemConvert::SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMEVENT_ITEMCONVERT_CB_RECEIVE_GRANT, pfCallback, pClass);
}