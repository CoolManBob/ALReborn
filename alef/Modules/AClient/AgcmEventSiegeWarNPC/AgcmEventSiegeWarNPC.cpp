#include "AgcmEventSiegeWarNPC.h"

AgcmEventSiegeWarNPC::AgcmEventSiegeWarNPC()
{
	SetModuleName("AgcmEventSiegeWarNPC");
}

AgcmEventSiegeWarNPC::~AgcmEventSiegeWarNPC()
{
}

BOOL AgcmEventSiegeWarNPC::OnAddModule()
{
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmEventSiegeWarNPC	= (AgpmEventSiegeWarNPC *)	GetModule("AgpmEventSiegeWarNPC");
	m_pcsAgcmEventManager		= (AgcmEventManager *)		GetModule("AgcmEventManager");
	m_pcsAgcmCharacter			= (AgcmCharacter *)			GetModule("AgpmCharacter");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventSiegeWarNPC ||
		!m_pcsAgcmEventManager ||
		!m_pcsAgcmCharacter)
		return FALSE;

	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_SIEGEWAR_NPC, CBEventRequest, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmEventSiegeWarNPC::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent				*pcsEvent	= (ApdEvent *) pData;
	AgcmEventSiegeWarNPC	*pThis		= (AgcmEventSiegeWarNPC *) pClass;
	ApBase					*pGenerator	= (ApBase *) pCustData;

	if (pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventSiegeWarNPC->MakePacketEventRequest(pcsEvent, pGenerator->m_lID, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength);

	pThis->m_pcsAgpmEventSiegeWarNPC->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmEventSiegeWarNPC::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ApdEvent				*pcsEvent	= (ApdEvent *) pData;
	AgcmEventSiegeWarNPC	*pThis		= (AgcmEventSiegeWarNPC *) pClass;


	return TRUE;
}