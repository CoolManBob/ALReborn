#include "AgcmEventCharCustomize.h"

AgcmEventCharCustomize::AgcmEventCharCustomize()
{
	SetModuleName("AgcmEventCharCustomize");
}

AgcmEventCharCustomize::~AgcmEventCharCustomize()
{
}

BOOL AgcmEventCharCustomize::OnAddModule()
{
	m_pcsAgpmCharacter			= (AgpmCharacter *)				GetModule("AgpmCharacter");
	m_pcsAgpmEventCharCustomize	= (AgpmEventCharCustomize *)	GetModule("AgpmEventCharCustomize");
	m_pcsAgcmCharacter			= (AgcmCharacter *)				GetModule("AgcmCharacter");
	m_pcsAgcmEventManager		= (AgcmEventManager *)			GetModule("AgcmEventManager");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventCharCustomize ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmEventManager)
		return FALSE;

	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_CHAR_CUSTOMIZE, CBEventCharCustomize, this))
		return FALSE;

	if (!m_pcsAgpmEventCharCustomize->SetCallbackGrant(CBEventGrant, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmEventCharCustomize::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventCharCustomize	*pThis	= (AgcmEventCharCustomize *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	ApdEvent	*pcsEvent	= (ApdEvent *)	pCustData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	return pThis->EnumCallback(AGCMEVENTCHARCUSTOMIZE_CB_GRANT_EVENT, pcsCharacter, pcsEvent);
}

BOOL AgcmEventCharCustomize::CBEventCharCustomize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventCharCustomize		*pThis			= (AgcmEventCharCustomize *)		pClass;
	ApdEvent					*pcsEvent		= (ApdEvent *)						pData;
	ApBase						*pcsGenerator	= (ApBase *)						pCustData;

	if (pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
		return FALSE;

	//if (pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter->m_bIsTrasform)
	//	return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventCharCustomize->MakePacketRequestEvent(pcsGenerator->m_lID, pcsEvent, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength);

	pThis->m_pcsAgpmEventCharCustomize->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmEventCharCustomize::SetCallbackGrantEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMEVENTCHARCUSTOMIZE_CB_GRANT_EVENT, pfCallback, pClass);
}