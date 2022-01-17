// AgcmEventQuest.cpp: implementation of the AgcmEventQuest class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmEventQuest.h"

#include "ApMemoryTracker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmEventQuest::AgcmEventQuest()
{
	SetModuleName("AgcmEventQuest");

	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmEventQuest		= NULL;
	m_pcsAgcmCharacter		= NULL;
	m_pcsAgcmEventManager	= NULL;	
	m_pcsAgcmObject			= NULL;

	m_pcsLastGrantEvent		= NULL;
}

AgcmEventQuest::~AgcmEventQuest()
{

}

BOOL AgcmEventQuest::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgpmEventQuest		= (AgpmEventQuest*)GetModule("AgpmEventQuest");
	m_pcsAgcmCharacter		= (AgcmCharacter*)GetModule("AgcmCharacter");
	m_pcsAgcmEventManager	= (AgcmEventManager*)GetModule("AgcmEventManager");
	m_pcsAgcmObject			= (AgcmObject*)GetModule("AgcmObject");

	if(!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventQuest ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmEventManager ||
		!m_pcsAgcmObject)
		return FALSE;

	if(!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_QUEST, CBEventRequest, this))
		return FALSE;

	if(!m_pcsAgpmEventQuest->SetCallbackGrant(CBEventGrant, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmEventQuest::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMEVENTQUEST_CB_GRANT, pfCallback, pClass);
}

BOOL AgcmEventQuest::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgcmEventQuest* pThis = (AgcmEventQuest*)pClass;
	ApBase* pcsGenerator = (ApBase*)pCustData;

	if(pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
		return FALSE;
	
	INT16 nPacketLength = 0;
	INT32 lRange = 0;
	if (APBASE_TYPE_OBJECT == pcsEvent->m_pcsSource->m_eType)
	{
		lRange = ( INT32 ) ( pThis->m_pcsAgcmObject->GetBoundingSphereRange((ApdObject*)pcsEvent->m_pcsSource) + 50.0f );
	}

	PVOID pvPacket = pThis->m_pcsAgpmEventQuest->MakePacketEventRequest(pcsEvent, pcsGenerator->m_lID, &nPacketLength, lRange);
	
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength);
	pThis->m_pcsAgpmEventQuest->m_csPacket.FreePacket(pvPacket);

	return bResult;

}

BOOL AgcmEventQuest::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgcmEventQuest* pThis = (AgcmEventQuest*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	if(pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return FALSE;

	if(pcsEvent->m_eFunction != APDEVENT_FUNCTION_QUEST)
		return FALSE;

	pThis->m_pcsLastGrantEvent = pcsEvent;
	
	pThis->EnumCallback(AGCMEVENTQUEST_CB_GRANT, pcsCharacter, pcsEvent);

	return TRUE;
}
