/*=========================================================

	AgpmEventCook.h

==========================================================*/

#include "AgpmEventCook.h"

/********************************************************/
/*		The Implementation of AgpmEventCook class		*/
/********************************************************/
//
AgpmEventCook::AgpmEventCook()
	{
	SetModuleName("AgpmEventCook");

	SetPacketType(AGPMEVENT_COOK_PACKET_TYPE);
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT8,		1,			// Operation
							AUTYPE_PACKET,		1,			// Event Base Packet
							AUTYPE_INT32,		1,			// CID
							AUTYPE_END,			0);

	m_pcsAgpmCharacter = NULL;
	m_pcsApmEventManager = NULL;
	}

AgpmEventCook::~AgpmEventCook()
	{
	}

BOOL AgpmEventCook::OnAddModule()
	{
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	
	if (!m_pcsAgpmCharacter || !m_pcsApmEventManager)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionEventCook(CBActionCook, this))
		return FALSE;

	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_COOK, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	return TRUE;
	}

BOOL AgpmEventCook::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
	{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	PVOID pvPacketEventBase = NULL;
	INT32 lCID = -1;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvPacketEventBase,
						&lCID);
	
	if (!pvPacketEventBase)
		return FALSE;

	ApdEvent* pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvPacketEventBase);
	if (!pcsEvent)
		return FALSE;

	if (APDEVENT_FUNCTION_COOK != pcsEvent->m_eFunction)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	switch(cOperation)
		{
		case AGPMEVENT_COOK_PACKET_REQUEST:
			OnOperationRequest(pcsEvent, pcsCharacter);
			break;

		case AGPMEVENT_COOK_PACKET_GRANT:
			OnOperationGrant(pcsEvent, pcsCharacter);
			break;
		}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
	}

BOOL AgpmEventCook::OnOperationRequest(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter)
	{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	AuPOS stTargetPos;
	memset(&stTargetPos, 0, sizeof(stTargetPos));
	
	if(m_pcsApmEventManager->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, AGPMEVENT_COOK_MAX_USE_RANGE, &stTargetPos))
		{
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if(pcsCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

		EnumCallback(AGPMEVENT_COOK_CB_REQUEST, pcsEvent, pcsCharacter);
		}
	else
		{
		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_COOK;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsEvent->m_pcsSource->m_lID;

		m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE);
		}

	return TRUE;
	}

BOOL AgpmEventCook::OnOperationGrant(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter)
	{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	EnumCallback(AGPMEVENT_COOK_CB_GRANT, pcsEvent, pcsCharacter);

	return TRUE;
	}

BOOL AgpmEventCook::SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMEVENT_COOK_CB_REQUEST, pfCallback, pClass);
	}

BOOL AgpmEventCook::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMEVENT_COOK_CB_GRANT, pfCallback, pClass);
	}

PVOID AgpmEventCook::MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_COOK_PACKET_REQUEST;
	
	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_COOK_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);
	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
	}

PVOID AgpmEventCook::MakePacketEventGrant(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_COOK_PACKET_GRANT;
	
	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_COOK_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
	}

BOOL AgpmEventCook::CBActionCook(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter *) pData;
	AgpmEventCook* pThis = (AgpmEventCook *) pClass;
	AgpdCharacterAction* pstAction = (AgpdCharacterAction *) pCustData;

	pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	
	if (pcsCharacter->m_bMove)
		pThis->m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
	
	ApdEvent* pcsEvent = pThis->m_pcsApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType, pstAction->m_csTargetBase.m_lID, APDEVENT_FUNCTION_COOK);

	pThis->EnumCallback(AGPMEVENT_COOK_CB_REQUEST, pcsEvent, pcsCharacter);
	
	return TRUE;
	}
