// AgsmEventQuest.cpp: implementation of the AgsmEventQuest class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmEventQuest.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmEventQuest::AgsmEventQuest()
{
	SetModuleName("AgsmEventQuest");

	m_pcsAgpmEventQuest = NULL;
	m_pcsAgsmCharacter = NULL;
}

AgsmEventQuest::~AgsmEventQuest()
{

}

BOOL AgsmEventQuest::OnAddModule()
{
	m_pcsAgpmEventQuest = (AgpmEventQuest*)GetModule("AgpmEventQuest");
	m_pcsAgsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");

	if(!m_pcsAgpmEventQuest || !m_pcsAgsmCharacter)
		return FALSE;

	if(!m_pcsAgpmEventQuest->SetCallbackRequest(CBProductEventRequest, this))
		return FALSE;
	
	return TRUE;
}

BOOL AgsmEventQuest::CBProductEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgsmEventQuest* pThis = (AgsmEventQuest*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventQuest->MakePacketEventGrant(pcsEvent, pcsCharacter->m_lID, &nPacketLength);

	BOOL bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
	pThis->m_pcsAgpmEventQuest->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}