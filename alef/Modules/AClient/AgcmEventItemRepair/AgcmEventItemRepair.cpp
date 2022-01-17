//	Item Repair Event Module (client-side)
//		- 아이템 수리에 관한 처리(클라이언트쪽)를 담당하는 모듈이다.
/////////////////////////////////////////////////////////////

#include "AgcmEventItemRepair.h"

AgcmEventItemRepair::AgcmEventItemRepair()
{
	SetModuleName("AgcmEventItemRepair");

	m_pcsLastGrantEvent = NULL;
}

AgcmEventItemRepair::~AgcmEventItemRepair()
{
}

BOOL AgcmEventItemRepair::OnAddModule()
{
	m_pcsAgpmEventItemRepair	= (AgpmEventItemRepair *)	GetModule("AgpmEventItemRepair");
	m_pcsAgcmEventManager		= (AgcmEventManager *)		GetModule("AgcmEventManager");
	m_pcsAgcmCharacter			= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");

	if (!m_pcsAgpmEventItemRepair || !m_pcsAgcmEventManager || !m_pcsAgcmCharacter || !m_pcsAgpmCharacter)
		return FALSE;

	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_ITEM_REPAIR, CBEventRequest, this))
		return FALSE;

	if (!m_pcsAgpmEventItemRepair->SetCallbackGrant(CBEventGrant, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmEventItemRepair::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMEVENT_ITEMREPAIR_CB_GRANT, pfCallback, pClass);
}

BOOL AgcmEventItemRepair::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pApdEvent = (ApdEvent *) pData;
	AgcmEventItemRepair	*pThis = (AgcmEventItemRepair *) pClass;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pAgpdCharacter)
		return FALSE;

	if (APDEVENT_FUNCTION_ITEM_REPAIR != pApdEvent->m_eFunction)
		return FALSE;

	pThis->m_pcsLastGrantEvent = pApdEvent;
	pThis->EnumCallback(AGCMEVENT_ITEMREPAIR_CB_GRANT, pAgpdCharacter, pApdEvent);

	return TRUE;
}

BOOL AgcmEventItemRepair::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pApdEvent = (ApdEvent *) pData;
	AgcmEventItemRepair	*pThis = (AgcmEventItemRepair *) pClass;
	ApBase				*pGenerator = (ApBase *) pCustData;

	if (pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventItemRepair->MakePacketEventRequest(pApdEvent, pGenerator->m_lID, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength);
	pThis->m_pcsAgpmEventItemRepair->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}