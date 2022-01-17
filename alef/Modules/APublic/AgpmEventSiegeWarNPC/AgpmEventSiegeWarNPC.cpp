#include "AgpmEventSiegeWarNPC.h"

AgpmEventSiegeWarNPC::AgpmEventSiegeWarNPC()
{
	SetModuleName("AgpmEventSiegeWarNPC");
	SetPacketType(AGPMEVENT_SIEGEWAR_NPC_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,			// Operation
							AUTYPE_PACKET,		1,			// event base packet
							AUTYPE_INT32,		1,			// Character ID
							AUTYPE_END,			0
							);
}

AgpmEventSiegeWarNPC::~AgpmEventSiegeWarNPC()
{
}

BOOL AgpmEventSiegeWarNPC::OnAddModule()
{
	m_pcsApmEventManager	= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pcsAgpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmSiegeWar		= (AgpmSiegeWar *)			GetModule("AgpmSiegeWar");

	if (!m_pcsApmEventManager ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmSiegeWar)
		return FALSE;

	// Event Manager에 Event를 등록한다.
	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_SIEGEWAR_NPC, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionEventSiegeWar(CBActionSiegeWar, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventSiegeWarNPC::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < sizeof(PACKET_HEADER) || !pstCheckArg)
		return FALSE;

	INT8	cOperation		= (-1);
	PVOID	pvPacketBase	= NULL;
	INT32	lCID			= AP_INVALID_CID;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvPacketBase,
						&lCID);

	ApdEvent		*pcsEvent		= NULL;
	
	if (pvPacketBase)
		pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvPacketBase);

	if (!pstCheckArg->bReceivedFromServer &&
		pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
		pstCheckArg->lSocketOwnerID != lCID)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	//if (!pcsCharacter)
	//	return FALSE;

	if (pcsCharacter && !pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	BOOL	bResult	= TRUE;

	switch (cOperation) {
		case AGPMEVENTSIEGEWAR_EVENT_REQUEST:
			bResult	= OnOperationEventRequest(pcsEvent, pcsCharacter);
			break;

		case AGPMEVENTSIEGEWAR_EVENT_GRANT:
			bResult	= OnOperationEventGrant(pcsEvent);
			break;

		default:
			bResult	= FALSE;
			break;
	}

	if (pcsCharacter)
		pcsCharacter->m_Mutex.Release();

	return bResult;
}

BOOL AgpmEventSiegeWarNPC::OnOperationEventRequest(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter)
{
	if (!pcsEvent || !pcsCharacter)
		return TRUE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	if (m_pcsAgpmCharacter->IsActionBlockCondition(pcsCharacter))
		return FALSE;

	AuPOS	stTargetPos	= {0,0,0};

	if (m_pcsApmEventManager->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, APMEVENT_MAX_RANGE_TO_ARISE_EVENT, &stTargetPos))
	{
		return EnumCallback(AGPMEVENTSIEGEWAR_CB_EVENT_GRANT, pcsCharacter, pcsEvent);
	}
	else
	{
		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_SIEGE_WAR;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsEvent->m_pcsSource->m_lID;

		return m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgpmEventSiegeWarNPC::OnOperationEventGrant(ApdEvent *pcsEvent)
{
	if (!pcsEvent)
		return TRUE;

	return EnumCallback(AGPMEVENTSIEGEWAR_CB_EVENT_GRANT, pcsEvent, NULL);
}

BOOL AgpmEventSiegeWarNPC::CBActionSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventSiegeWarNPC	*pThis				= (AgpmEventSiegeWarNPC *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	AgpdCharacterAction		*pstNextAction		= (AgpdCharacterAction *)	pCustData;

	if (pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	ApdEvent				*pcsEvent			= pThis->m_pcsApmEventManager->GetEvent(pstNextAction->m_csTargetBase.m_eType, pstNextAction->m_csTargetBase.m_lID, APDEVENT_FUNCTION_SIEGEWAR_NPC);
	if (!pcsEvent)
		return FALSE;

	return pThis->EnumCallback(AGPMEVENTSIEGEWAR_CB_EVENT_GRANT, pcsCharacter, pcsEvent);
}

BOOL AgpmEventSiegeWarNPC::SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENTSIEGEWAR_CB_EVENT_GRANT, pfCallback, pClass);
}

PVOID AgpmEventSiegeWarNPC::MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsEvent ||
		lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	INT8	cOperation		= AGPMEVENTSIEGEWAR_EVENT_REQUEST;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SIEGEWAR_NPC_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventSiegeWarNPC::MakePacketEventGrant(ApdEvent *pcsEvent, INT16 *pnPacketLength)
{
	if (!pcsEvent ||
		!pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	INT8	cOperation		= AGPMEVENTSIEGEWAR_EVENT_GRANT;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_SIEGEWAR_NPC_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													NULL);

	m_pcsApmEventManager->m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

AgpdSiegeWar* AgpmEventSiegeWarNPC::GetSiegeWarInfo(ApdEvent *pcsEvent)
{
	if (!pcsEvent)
		return FALSE;

	AuPOS	stBasePos;
	ZeroMemory(&stBasePos, sizeof(stBasePos));

	if (!m_pcsApmEventManager->GetBasePos(pcsEvent, &stBasePos))
		return NULL;

	return m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(stBasePos));
}