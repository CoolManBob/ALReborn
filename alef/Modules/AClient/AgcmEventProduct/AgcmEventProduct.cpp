/*===================================================================

	AgcmEventProduct.cpp
	
===================================================================*/

#include "AgcmEventProduct.h"

#include "ApMemoryTracker.h"

/************************************************************/
/*		the Implementation of AgcmEventProduct class		*/
/************************************************************/
//
AgcmEventProduct::AgcmEventProduct()
	{
	SetModuleName("AgcmEventProduct");
	}

AgcmEventProduct::~AgcmEventProduct()
	{
	}


//	ApModule inherited
//=========================================
//
BOOL AgcmEventProduct::OnAddModule()
	{
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmEventProduct = (AgpmEventProduct *) GetModule("AgpmEventProduct");
	m_pAgcmCharacter = (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pAgcmEventManager = (AgcmEventManager *) GetModule("AgcmEventManager");

	if (!m_pAgpmCharacter || !m_pAgpmEventProduct ||
		!m_pAgcmCharacter || !m_pAgcmEventManager)
		return FALSE;

	if (!m_pAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_PRODUCT, CBEventRequest, this))
		return FALSE;

	if (!m_pAgpmEventProduct->SetCallbackGrant(CBEventGrant, this))
		return FALSE;

	return TRUE;
	}


//	Callback setting
//==========================================
//
BOOL AgcmEventProduct::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGCMEVENT_PRODUCT_CB_GRANT, pfCallback, pClass);
	}


//	Callbacks
//==========================================
//
BOOL AgcmEventProduct::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pApdEvent = (ApdEvent *) pData;
	AgcmEventProduct	*pThis = (AgcmEventProduct *) pClass;
	ApBase				*pGenerator = (ApBase *) pCustData;

	if (pThis->m_pAgpmCharacter->IsActionBlockCondition(pThis->m_pAgcmCharacter->GetSelfCharacter()))
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pAgpmEventProduct->MakePacketEventRequest(pApdEvent, pGenerator->m_lID, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength);
	pThis->m_pAgpmEventProduct->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}

BOOL AgcmEventProduct::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pApdEvent = (ApdEvent *) pData;
	AgcmEventProduct	*pThis = (AgcmEventProduct *) pClass;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;

	if (pThis->m_pAgcmCharacter->GetSelfCharacter() != pAgpdCharacter)
		return FALSE;

	if (APDEVENT_FUNCTION_PRODUCT != pApdEvent->m_eFunction)
		return FALSE;
	
	pThis->EnumCallback(AGCMEVENT_PRODUCT_CB_GRANT, pAgpdCharacter, pApdEvent);

	return TRUE;
	}


//	Packet
//===============================================
//
BOOL  AgcmEventProduct::SendLearnSkill(ApdEvent *pApdEvent, INT32 lSkillTID)
	{
	if (!pApdEvent)
		return FALSE;
		
	INT16 nPacketLength	= 0;
	PVOID pvPacket = m_pAgpmEventProduct->MakePacketLearnSkill(pApdEvent, m_pAgcmCharacter->GetSelfCID(), lSkillTID, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmEventProduct->m_csPacket.FreePacket(pvPacket);

	return	bResult;	
	}

BOOL AgcmEventProduct::SendBuyCompose(ApdEvent *pApdEvent, INT32 lComposeID)
	{
	if (!pApdEvent)
		return FALSE;
		
	INT16 nPacketLength	= 0;
	PVOID pvPacket = m_pAgpmEventProduct->MakePacketBuyCompose(pApdEvent, m_pAgcmCharacter->GetSelfCID(), lComposeID, &nPacketLength);	
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmEventProduct->m_csPacket.FreePacket(pvPacket);

	return	bResult;
	}
