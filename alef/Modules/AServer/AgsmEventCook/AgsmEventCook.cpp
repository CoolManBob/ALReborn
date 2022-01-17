/*=========================================================

	AgsmEventCook.cpp

==========================================================*/

#include "AgsmEventCook.h"

/********************************************************/
/*		The Implementation of AgsmEventCook class		*/
/********************************************************/
//
AgsmEventCook::AgsmEventCook()
	{
	SetModuleName("AgsmEventCook");

	m_pAgpmEventCook = NULL;
	m_pAgsmCharacter = NULL;
	}

AgsmEventCook::~AgsmEventCook()
	{
	}

BOOL AgsmEventCook::OnAddModule()
	{
	m_pAgpmEventCook = (AgpmEventCook *) GetModule("AgpmEventCook");
	m_pAgsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");

	if (!m_pAgpmEventCook || !m_pAgsmCharacter)
		return FALSE;

	if (!m_pAgpmEventCook->SetCallbackRequest(CBCookEventRequest, this))
		return FALSE;
	
	return TRUE;
	}

BOOL AgsmEventCook::CBCookEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent *) pData;
	AgsmEventCook* pThis = (AgsmEventCook *) pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter *) pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pAgpmEventCook->MakePacketEventGrant(pcsEvent, pcsCharacter->m_lID, &nPacketLength);

	BOOL bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pAgsmCharacter->GetCharDPNID(pcsCharacter));
	pThis->m_pAgpmEventCook->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
	}