/*==============================================================

	AgcmEventCook.cpp

==============================================================*/

#include "AgcmEventCook.h"

/********************************************************/
/*		The Implementation of AgcmEventCook class		*/
/********************************************************/
//
AgcmEventCook::AgcmEventCook()
	{
	SetModuleName("AgcmEventCook");
	}

AgcmEventCook::~AgcmEventCook()
	{
	}

BOOL AgcmEventCook::OnAddModule()
	{
	m_pcsAgpmCharacter = (AgpmCharacter*) GetModule("AgpmCharacter");
	m_pcsAgpmEventCook = (AgpmEventCook*) GetModule("AgpmEventCook");

	m_pcsAgcmCharacter = (AgcmCharacter*) GetModule("AgcmCharacter");
	m_pcsAgcmEventManager = (AgcmEventManager*) GetModule("AgcmEventManager");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventCook ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmEventManager)
		return FALSE;

	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_COOK, CBEventRequest, this))
		return FALSE;

	if (!m_pcsAgpmEventCook->SetCallbackGrant(CBEventGrant, this))
		return FALSE;

	return TRUE;
	}

BOOL AgcmEventCook::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGCMEVENTCOOK_CB_GRANT, pfCallback, pClass);
	}

BOOL AgcmEventCook::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent *) pData;
	AgcmEventCook* pThis = (AgcmEventCook *) pClass;
	ApBase* pcsGenerator = (ApBase *) pCustData;

	if (pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventCook->MakePacketEventRequest(pcsEvent, pcsGenerator->m_lID, &nPacketLength);
	
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength);
	pThis->m_pcsAgpmEventCook->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}

BOOL AgcmEventCook::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent *) pData;
	AgcmEventCook* pThis = (AgcmEventCook *) pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter *) pCustData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return FALSE;

	if (APDEVENT_FUNCTION_COOK != pcsEvent->m_eFunction)
		return FALSE;
	
	pThis->EnumCallback(AGCMEVENTCOOK_CB_GRANT, pcsCharacter, pcsEvent);
	return TRUE;
	}
