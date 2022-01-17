#include "AgcmEventSkillMaster.h"

AgcmEventSkillMaster::AgcmEventSkillMaster()
{
	SetModuleName("AgcmEventSkillMaster");

	m_pcsAgpmCharacter			= NULL;
	m_pcsAgpmEventSkillMaster	= NULL;
	m_pcsAgcmCharacter			= NULL;
	m_pcsAgcmItem				= NULL;
	m_pcsAgcmSkill				= NULL;
	m_pcsAgcmEventManager		= NULL;
}

AgcmEventSkillMaster::~AgcmEventSkillMaster()
{
}

BOOL AgcmEventSkillMaster::OnAddModule()
{
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmEventSkillMaster	= (AgpmEventSkillMaster *)	GetModule("AgpmEventSkillMaster");
	m_pcsAgcmCharacter			= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pcsAgcmItem				= (AgcmItem *)				GetModule("AgcmItem");
	m_pcsAgcmSkill				= (AgcmSkill *)				GetModule("AgcmSkill");
	m_pcsAgcmEventManager		= (AgcmEventManager *)		GetModule("AgcmEventManager");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventSkillMaster ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmEventManager)
		return FALSE;

	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_SKILLMASTER, CBExecuteEvent, this))
		return FALSE;

	if (!m_pcsAgpmEventSkillMaster->SetCallbackResponseUpgrade(CBResponseSkillUpgrade, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackResponseEvent(CBResponseEvent, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmEventSkillMaster::SendRequestEvent(ApdEvent *pcsEvent)
{
	if (!pcsEvent)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmEventSkillMaster->MakePacketRequestEvent(pcsEvent, m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmEventSkillMaster::SendBuySkillBook(ApdEvent *pcsEvent, INT32 lSkillTID)
{
	if (!pcsEvent ||
		lSkillTID == AP_INVALID_SKILLID)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmEventSkillMaster->MakePacketBuySkill(pcsEvent, m_pcsAgcmCharacter->GetSelfCID(), lSkillTID, &nPacketLength);
	
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmEventSkillMaster::SendLearnSkill(ApdEvent *pcsEvent, INT32 lSkillTID)
{
	if (!pcsEvent ||
		lSkillTID == AP_INVALID_SKILLID)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmEventSkillMaster->MakePacketLearnSkill(pcsEvent, m_pcsAgcmCharacter->GetSelfCID(), lSkillTID, &nPacketLength);
	
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmEventSkillMaster::SendRequestSkillUpgrade(ApdEvent *pcsEvent, INT32 lSkillID, INT32 lUpgradePoint)
{
	if (!pcsEvent || lSkillID == AP_INVALID_SKILLID || lUpgradePoint < 1)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmEventSkillMaster->MakePacketRequestUpgrade(pcsEvent, m_pcsAgcmCharacter->GetSelfCID(), lSkillID, lUpgradePoint, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmEventSkillMaster::SendRequestSkillInitialize(ApdEvent *pcsEvent)
{
	if (!pcsEvent)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmEventSkillMaster->MakePacketRequestSkillInitialize(pcsEvent, m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmEventSkillMaster->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmEventSkillMaster::CBResponseSkillUpgrade(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventSkillMaster	*pThis					= (AgcmEventSkillMaster *)		pClass;
	ApdEvent				*pcsEvent				= (ApdEvent *)					pData;
	PVOID					*ppvBuffer				= (PVOID *)						pCustData;

	AgpdCharacter			*pcsCharacter			= (AgpdCharacter *)				ppvBuffer[0];

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	INT32					lSkillID				= (INT32) ppvBuffer[1];
	AgpmEventSkillUpgradeResult	eResult				= (AgpmEventSkillUpgradeResult)	(INT32) ppvBuffer[2];

	PVOID	pvBuffer[3];
	pvBuffer[0]		= (PVOID) pcsEvent;
	pvBuffer[1]		= (PVOID) lSkillID;
	pvBuffer[2]		= (PVOID) (INT32) eResult;

	return pThis->EnumCallback(AGCMEVENT_SKILL_CB_RESULT_SKILL_UPGRADE, pcsCharacter, pvBuffer);
}

BOOL AgcmEventSkillMaster::CBResponseEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventSkillMaster	*pThis					= (AgcmEventSkillMaster *)		pClass;
	AgpdCharacter			*pcsCharacter			= (AgpdCharacter *)				pData;
	PVOID					*ppvBuffer				= (PVOID *)						pCustData;

	ApdEvent				*pcsEvent				= (ApdEvent *)					ppvBuffer[0];
	AgpmEventRequestResult	eResult					= (AgpmEventRequestResult) (INT32)	ppvBuffer[1];

	if (!pcsEvent)
		return FALSE;

	// UI를 띄워준다.

	switch (eResult) {
	case AGPMEVENT_SKILL_REQUEST_RESULT_SUCCESS:
		{
			PVOID	pvBuffer[3];
			pvBuffer[0]		= (PVOID) pcsEvent;
			pvBuffer[1]		= (PVOID) (INT32) eResult;
			pvBuffer[2]		= (PVOID) ppvBuffer[2];

			pThis->EnumCallback(AGCMEVENT_SKILL_CB_RESPONSE_GRANT, pcsCharacter, pvBuffer);
		}
		break;

	case AGPMEVENT_SKILL_REQUEST_RESULT_FAIL:
		{
		}
		break;
	}

	return TRUE;
}

BOOL AgcmEventSkillMaster::CBExecuteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventSkillMaster	*pThis					= (AgcmEventSkillMaster *)		pClass;
	ApdEvent				*pcsEvent				= (ApdEvent *)					pData;
	ApBase					*pcsGenerator			= (ApBase *)					pCustData;

	// 이벤트가 발생했다...
	// 서버로 패킷 날려.. 거리나 이것 저것.. 체크한다.

	if (pcsGenerator->m_eType == APBASE_TYPE_CHARACTER &&
		pcsGenerator->m_lID	== pThis->m_pcsAgcmCharacter->GetSelfCID())
	{
		if (pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
			return FALSE;

		return pThis->SendRequestEvent(pcsEvent);
	}

	return TRUE;
}

BOOL AgcmEventSkillMaster::SetCallbackResultSkillUpgrade(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMEVENT_SKILL_CB_RESULT_SKILL_UPGRADE, pfCallback, pClass);
}

BOOL AgcmEventSkillMaster::SetCallbackResponseGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMEVENT_SKILL_CB_RESPONSE_GRANT, pfCallback, pClass);
}