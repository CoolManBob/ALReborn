#include "AgpmEventBank.h"

AgpmEventBank::AgpmEventBank()
{
	SetModuleName("AgpmEventBank");

	SetPacketType(AGPMEVENT_BANK_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,				// operation
							AUTYPE_PACKET,		1,				// event base packet
							AUTYPE_INT32,		1,				// CID
							AUTYPE_END,			0);

	m_pcsApmEventManager	= NULL;
	m_pcsAgpmCharacter		= NULL;
}

AgpmEventBank::~AgpmEventBank()
{
}

BOOL AgpmEventBank::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");

	if (!m_pcsAgpmCharacter ||
		!m_pcsApmEventManager)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionEventBank(CBActionBank, this))
		return FALSE;

	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_BANK, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventBank::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation			= (-1);
	PVOID	pvPacketEventBase	= NULL;
	INT32	lCID				= AP_INVALID_CID;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvPacketEventBase,
						&lCID);

	if (!pstCheckArg->bReceivedFromServer &&
		pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
		pstCheckArg->lSocketOwnerID != lCID)
		return FALSE;

	ApdEvent		*pcsEvent	= NULL;
	if (pvPacketEventBase)
		pcsEvent		= m_pcsApmEventManager->GetEventFromBasePacket(pvPacketEventBase);

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (cOperation) {
	case AGPMEVENT_BANK_PACKET_OPERATION_REQUEST:
		{
			if (!pcsEvent || pcsEvent->m_eFunction != APDEVENT_FUNCTION_BANK)
				return FALSE;		
		
			OnOperationRequest(pcsEvent, pcsCharacter);
		}
		break;

	case AGPMEVENT_BANK_PACKET_OPERATION_GRANT:
		{
			if (!pcsEvent || pcsEvent->m_eFunction != APDEVENT_FUNCTION_BANK)
				return FALSE;		

			OnOperationGrant(pcsEvent, pcsCharacter);
		}
		break;
		
	case AGPMEVENT_BANK_PACKET_OPERATION_GRANT_ANY:
		{
			EnumCallback(AGPMEVENT_BANK_CB_GRANT_ANY, NULL, pcsCharacter);
		}
		break;
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmEventBank::OnOperationRequest(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter)
{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	AuPOS	stTargetPos;
	ZeroMemory(&stTargetPos, sizeof(AuPOS));

	if (m_pcsApmEventManager->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, AGPMEVENT_BANK_MAX_USE_RANGE, &stTargetPos))
	{
		pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

		if (pcsCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

		EnumCallback(AGPMEVENT_BANK_CB_REQUEST, pcsEvent, pcsCharacter);
	}
	else
	{
		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_BANK;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsEvent->m_pcsSource->m_lID;

		m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgpmEventBank::OnOperationGrant(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter)
{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	EnumCallback(AGPMEVENT_BANK_CB_GRANT, pcsEvent, pcsCharacter);

	return TRUE;
}

BOOL AgpmEventBank::SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_BANK_CB_REQUEST, pfCallback, pClass);
}

BOOL AgpmEventBank::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_BANK_CB_GRANT, pfCallback, pClass);
}

BOOL AgpmEventBank::SetCallbackGrantAnywhere(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_BANK_CB_GRANT_ANY, pfCallback, pClass);
}

PVOID AgpmEventBank::MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation		= AGPMEVENT_BANK_PACKET_OPERATION_REQUEST;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_BANK_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmEventBank::MakePacketEventGrant(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation		= AGPMEVENT_BANK_PACKET_OPERATION_GRANT;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_BANK_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmEventBank::MakePacketGrantAnywhere(INT32 lCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation		= AGPMEVENT_BANK_PACKET_OPERATION_GRANT_ANY;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_BANK_PACKET_TYPE,
													&cOperation,
													NULL,
													&lCID);

	return pvPacket;
}

BOOL AgpmEventBank::CBActionBank(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventBank			*pThis				= (AgpmEventBank *)			pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	AgpdCharacterAction		*pstAction			= (AgpdCharacterAction *)	pCustData;

	pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

	if (pcsCharacter->m_bMove)
		pThis->m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

	ApdEvent	*pcsEvent	= pThis->m_pcsApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType, pstAction->m_csTargetBase.m_lID, APDEVENT_FUNCTION_BANK);

	pThis->EnumCallback(AGPMEVENT_BANK_CB_REQUEST, pcsEvent, pcsCharacter);

	return TRUE;
}