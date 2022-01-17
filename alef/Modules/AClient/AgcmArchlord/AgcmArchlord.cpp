#include "AgcmArchlord.h"
#include "AgpmArchlord.h"

AgcmArchlord::AgcmArchlord()
{
	SetModuleName("AgcmArchlord");
}

AgcmArchlord::~AgcmArchlord()
{
}

BOOL AgcmArchlord::OnAddModule()
{
	m_pcsAgpmArchlord = (AgpmArchlord*)GetModule("AgpmArchlord");
	m_pcsAgpmCharacter = (AgpmCharacter *)GetModule("AgpmCharacter");
	m_pcsAgcmCharacter = (AgcmCharacter *)GetModule("AgcmCharacter");
	m_pcsAgcmEventManager = (AgcmEventManager *) GetModule("AgcmEventManager");

	if (!m_pcsAgpmArchlord || !m_pcsAgpmCharacter || !m_pcsAgcmCharacter || !m_pcsAgcmEventManager)
		return FALSE;

	if (!m_pcsAgpmArchlord->SetCallbackOperationSetArchlord(CBOperationSetArchlord, this))
		return FALSE;

	if (!m_pcsAgpmArchlord->SetCallbackOperationCancelArchlord(CBOperationCancelArchlord, this))
		return FALSE;
	
	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_ARCHLORD, CBRequestEvent, this))
		return FALSE;	

	return TRUE;
}

BOOL AgcmArchlord::CBOperationSetArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmArchlord *pThis = (AgcmArchlord*)pClass;
	char* szID = (char*)pData;

	return pThis->OperationSetArchlord(szID);
}

BOOL AgcmArchlord::CBOperationCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmArchlord::CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmArchlord	*pThis			= (AgcmArchlord *)		pClass;
	ApdEvent		*pApdEvent		= (ApdEvent *)			pData;
	ApBase			*pcsGenerator	= (ApBase *)			pCustData;

	if (pcsGenerator->m_eType == APBASE_TYPE_CHARACTER &&
		pcsGenerator->m_lID	== pThis->m_pcsAgcmCharacter->GetSelfCID())
		{
		if (pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
			return FALSE;

		return pThis->SendRequestEvent(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter, pApdEvent);
		}

	return TRUE;	
	}


BOOL AgcmArchlord::OperationSetArchlord(char* szID)
{
	m_pcsAgpmArchlord->SetArchlord(szID);
	return TRUE;
}

BOOL AgcmArchlord::SendRequestEvent(AgpdCharacter *pcsCharacter, ApdEvent *pApdEvent)
{
	if (NULL == pcsCharacter || NULL == pApdEvent)
		return FALSE;

	INT16 nPacketLength	= 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketEventRequest(&nPacketLength, pApdEvent, pcsCharacter->m_lID);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmArchlord->m_csPacket.FreePacket(pvPacket);

	return	bResult;
}