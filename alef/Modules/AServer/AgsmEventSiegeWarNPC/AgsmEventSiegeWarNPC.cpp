#include "AgsmEventSiegeWarNPC.h"

AgsmEventSiegeWarNPC::AgsmEventSiegeWarNPC()
{
	SetModuleName("AgsmEventSiegeWarNPC");
}

AgsmEventSiegeWarNPC::~AgsmEventSiegeWarNPC()
{
}

BOOL AgsmEventSiegeWarNPC::OnAddModule()
{
	m_pcsApmEventManager		= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pcsApmMap					= (ApmMap *)				GetModule("ApmMap");
	m_pcsAgpmSiegeWar			= (AgpmSiegeWar *)			GetModule("AgpmSiegeWar");
	m_pcsAgpmEventSiegeWarNPC	= (AgpmEventSiegeWarNPC *)	GetModule("AgpmEventSiegeWarNPC");
	m_pcsAgsmCharacter			= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pcsAgsmSiegeWar			= (AgsmSiegeWar *)			GetModule("AgsmSiegeWar");

	if (!m_pcsApmEventManager ||
		!m_pcsApmMap ||
		!m_pcsAgpmSiegeWar ||
		!m_pcsAgpmEventSiegeWarNPC ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmSiegeWar)
		return FALSE;

	if (!m_pcsAgpmEventSiegeWarNPC->SetCallbackEventGrant(CBEventGrant, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmEventSiegeWarNPC::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventSiegeWarNPC	*pThis	= (AgsmEventSiegeWarNPC *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	ApdEvent	*pcsEvent	= (ApdEvent *)	pCustData;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmEventSiegeWarNPC->GetSiegeWarInfo(pcsEvent);
	if (!pcsSiegeWar)
		return FALSE;

	if (!pThis->m_pcsAgsmSiegeWar->SendPacketCastleInfo(pcsSiegeWar, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter)))
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventSiegeWarNPC->MakePacketEventGrant(pcsEvent, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	pThis->m_pcsAgpmEventSiegeWarNPC->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}