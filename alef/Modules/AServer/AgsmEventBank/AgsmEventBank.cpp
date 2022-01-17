#include "AgsmEventBank.h"

AgsmEventBank::AgsmEventBank()
{
	SetModuleName("AgsmEventBank");

	m_pcsAgpmEventBank	= NULL;
	m_pcsAgsmCharacter	= NULL;
}

AgsmEventBank::~AgsmEventBank()
{
}

BOOL AgsmEventBank::OnAddModule()
{
	m_pcsAgpmEventBank		= (AgpmEventBank *)			GetModule("AgpmEventBank");
	m_pcsAgsmCharacter		= (AgsmCharacter *)			GetModule("AgsmCharacter");

	if (!m_pcsAgpmEventBank || !m_pcsAgsmCharacter)
		return FALSE;

	if (!m_pcsAgpmEventBank->SetCallbackRequest(CBBankEventRequest, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmEventBank::CBBankEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventBank			*pThis				= (AgsmEventBank *)			pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pData;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pCustData;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventBank->MakePacketEventGrant(pcsEvent, pcsCharacter->m_lID, &nPacketLength);

	BOOL	bSendResult		= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	pThis->m_pcsAgpmEventBank->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmEventBank::OpenAnywhere(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;
	
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmEventBank->MakePacketGrantAnywhere(pcsCharacter->m_lID, &nPacketLength);

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	m_pcsAgpmEventBank->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}