#include "AgsmEventManager.h"

AgsmEventManager::AgsmEventManager()
{
	SetModuleName("AgsmEventManager");

	m_pcsApmEventManager	= NULL;
	m_pcsAgsmAOIFilter		= NULL;
	m_pcsAgsmCharacter		= NULL;
}

AgsmEventManager::~AgsmEventManager()
{
}

BOOL AgsmEventManager::OnAddModule()
{
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pcsAgsmAOIFilter		= (AgsmAOIFilter *)		GetModule("AgsmAOIFilter");
	m_pcsAgsmCharacter		= (AgsmCharacter *)		GetModule("AgsmCharacter");

	if (!m_pcsApmEventManager ||
		!m_pcsAgsmAOIFilter ||
		!m_pcsAgsmCharacter)
		return FALSE;

	if (!m_pcsAgsmCharacter->SetCallbackSendEquipItem(CBSendCharacterView, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmEventManager::CBSendCharacterView(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;
	
	PROFILE("AgsmEventManager::CBSendCharacterView");

	AgsmEventManager	*pThis			= (AgsmEventManager *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;
	PVOID				*ppvBuffer		= (PVOID *)				pCustData;

	UINT32				ulNID			= PtrToUint(ppvBuffer[0]);
	BOOL				bGroupNID		= PtrToInt(ppvBuffer[1]);
	BOOL				bIsExceptSelf	= PtrToInt(ppvBuffer[2]);

	return pThis->SendEvent(pcsCharacter, ulNID, bGroupNID, bIsExceptSelf);
}

BOOL AgsmEventManager::SendEvent(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bIsGroupNID, BOOL bIsExceptSelf)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	ApdEventAttachData	*pcsEventAttachData	= m_pcsApmEventManager->GetEventData((ApBase *) pcsCharacter);
	if (!pcsEventAttachData)
		return FALSE;

	for (int i = 0; i < pcsEventAttachData->m_unFunction; ++i)
	{
		INT32	lEventIndex	= (INT32) pcsEventAttachData->m_astEvent[i].m_eFunction;

		if (lEventIndex <= APDEVENT_FUNCTION_NONE ||
			lEventIndex >= APDEVENT_MAX_FUNCTION)
			continue;

		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= m_pcsApmEventManager->MakePacketEventData(&pcsEventAttachData->m_astEvent[i], (ApdEventFunction) lEventIndex, &nPacketLength);

		if (!pvPacket || nPacketLength < 1)
			continue;

		m_pcsApmEventManager->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		if (bIsGroupNID)
		{
			if (bIsExceptSelf)
				m_pcsAgsmAOIFilter->SendPacketGroupExceptSelf(pvPacket, nPacketLength, ulNID, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_3);
			else
				m_pcsAgsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_3);
		}
		else
		{
			SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_3);
		}

		m_pcsApmEventManager->m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}