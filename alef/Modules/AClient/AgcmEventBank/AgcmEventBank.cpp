#include "AgcmEventBank.h"

AgcmEventBank::AgcmEventBank()
{
	SetModuleName("AgcmEventBank");

	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmEventBank		= NULL;
	m_pcsAgcmEventManager	= NULL;
}

AgcmEventBank::~AgcmEventBank()
{
}

BOOL AgcmEventBank::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmEventBank		= (AgpmEventBank *)		GetModule("AgpmEventBank");

	m_pcsAgcmEventManager	= (AgcmEventManager *)	GetModule("AgcmEventManager");
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventBank ||
		!m_pcsAgcmEventManager ||
		!m_pcsAgcmCharacter)
		return FALSE;

	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_BANK, CBEventBank, this))
		return FALSE;

	if (!m_pcsAgpmEventBank->SetCallbackGrant(CBEventGrant, this))
		return FALSE;

	if (!m_pcsAgpmEventBank->SetCallbackGrantAnywhere(CBEventGrantAnywhere, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmEventBank::CBEventBank(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventBank				*pThis			= (AgcmEventBank *)					pClass;
	ApdEvent					*pcsEvent		= (ApdEvent *)						pData;
	ApBase						*pcsGenerator	= (ApBase *)						pCustData;

	if (pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventBank->MakePacketEventRequest(pcsEvent, pcsGenerator->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength);

	pThis->m_pcsAgpmEventBank->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmEventBank::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventBank			*pThis					= (AgcmEventBank *)			pClass;
	ApdEvent				*pcsEvent				= (ApdEvent *)				pData;
	AgpdCharacter			*pcsCharacter			= (AgpdCharacter *)			pCustData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	if (pcsEvent->m_eFunction != APDEVENT_FUNCTION_BANK)
		return FALSE;

	pThis->EnumCallback(AGCMEVENTBANK_CB_GRANT_EVENT_BANK, pcsCharacter, pcsEvent);

	return TRUE;
}

BOOL AgcmEventBank::CBEventGrantAnywhere(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmEventBank			*pThis					= (AgcmEventBank *)			pClass;
	AgpdCharacter			*pcsCharacter			= (AgpdCharacter *)			pCustData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	pThis->EnumCallback(AGCMEVENTBANK_CB_GRANT_EVENT_BANK, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgcmEventBank::SetCallbackGrantEventBank(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMEVENTBANK_CB_GRANT_EVENT_BANK, pfCallback, pClass);
}