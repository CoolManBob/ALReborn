/*====================================================

	AgcmEventRefinery.cpp

====================================================*/

#include "AgcmEventRefinery.h"

#include "ApMemoryTracker.h"


/************************************************************/
/*		The Implementation of AgcmEventRefinery class		*/
/************************************************************/
//
AgcmEventRefinery::AgcmEventRefinery()
	{
	SetModuleName("AgcmEventRefinery");

	m_pAgpmCharacter		= NULL;
	m_pAgpmEventRefinery	= NULL;
	m_pAgcmCharacter		= NULL;
	m_pAgcmEventManager		= NULL;

	m_pLastestEvent	= NULL;
	}

AgcmEventRefinery::~AgcmEventRefinery()
	{
	}


//	ApModule inherited
//============================================
//
BOOL AgcmEventRefinery::OnAddModule()
	{
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmEventRefinery = (AgpmEventRefinery *) GetModule("AgpmEventRefinery");

	m_pAgcmCharacter = (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pAgcmEventManager = (AgcmEventManager *) GetModule("AgcmEventManager");

	if (!m_pAgpmCharacter || !m_pAgpmEventRefinery ||
		!m_pAgcmCharacter || !m_pAgcmEventManager)
		return FALSE;

	if (!m_pAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_REFINERY, CBEventRequest, this))
		return FALSE;

	if (!m_pAgpmEventRefinery->SetCallbackGrant(CBEventGrant, this))
		return FALSE;

	return TRUE;
	}


//	Callback setting
//============================================
//
BOOL AgcmEventRefinery::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGCMEVENTREFINERY_CB_GRANT, pfCallback, pClass);
	}

BOOL AgcmEventRefinery::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventRefinery *pThis = (AgcmEventRefinery *) pClass;
	ApdEvent *pApdEvent = (ApdEvent *) pData;
	ApBase *pGenerator = (ApBase *) pCustData;

	if (pThis->m_pAgpmCharacter->IsActionBlockCondition(pThis->m_pAgcmCharacter->m_pcsSelfCharacter))
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pAgpmEventRefinery->MakePacketEventRequest(pApdEvent, pGenerator->m_lID, &nPacketLength);
	
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength);
	pThis->m_pAgpmEventRefinery->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}

BOOL AgcmEventRefinery::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventRefinery *pThis = (AgcmEventRefinery *) pClass;
	ApdEvent *pApdEvent = (ApdEvent *) pData;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;

	if (pThis->m_pAgcmCharacter->GetSelfCharacter() != pAgpdCharacter)
		return FALSE;

	if(pApdEvent->m_eFunction != APDEVENT_FUNCTION_REFINERY)
		return FALSE;
	
	pThis->m_pLastestEvent = pApdEvent;

	pThis->EnumCallback(AGCMEVENTREFINERY_CB_GRANT, pAgpdCharacter, pApdEvent);

	return TRUE;
	}


