#include "AgpmEventItemConvert.h"

AgpmEventItemConvert::AgpmEventItemConvert()
{
	SetModuleName("AgpmEventItemConvert");

	SetPacketType(AGPMEVENT_ITEMCONVERT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,				// operation
							AUTYPE_PACKET,		1,				// event base packet
							AUTYPE_INT32,		1,				// cid
							AUTYPE_END,			0
							);
}

AgpmEventItemConvert::~AgpmEventItemConvert()
{
}

BOOL AgpmEventItemConvert::OnAddModule()
{
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmItemConvert	= (AgpmItemConvert *)	GetModule("AgpmItemConvert");

	if (!m_pcsApmEventManager ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmItemConvert)
		return FALSE;

	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_ITEMCONVERT, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionEventItemConvert(CBActionItemConvert, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventItemConvert::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation		= AGPMEVENT_ITEMCONVERT_OPERATION_NONE;
	INT32	lCID			= AP_INVALID_CID;
	PVOID	pvPacketBase	= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
							&cOperation,
							&pvPacketBase,
							&lCID);

	if (!pstCheckArg->bReceivedFromServer &&
		pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
		pstCheckArg->lSocketOwnerID != lCID)
		return FALSE;

	ApdEvent		*pcsEvent		= m_pcsApmEventManager->GetEventFromBasePacket(pvPacketBase);
	if (!pcsEvent)
		return FALSE;

	if (pcsEvent->m_eFunction != APDEVENT_FUNCTION_ITEMCONVERT)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (cOperation) {
	case AGPMEVENT_ITEMCONVERT_OPERATION_REQUEST:
		{
			OnOperationRequest(pcsEvent, pcsCharacter);
		}
		break;

	case AGPMEVENT_ITEMCONVERT_OPERATION_GRANT:
		{
			EnumCallback(AGPMEVENT_ITEMCONVERT_CB_GRANT, pcsEvent, pcsCharacter);
		}
		break;

	case AGPMEVENT_ITEMCONVERT_OPERATION_REJECT:
		{
			EnumCallback(AGPMEVENT_ITEMCONVERT_CB_REJECT, pcsEvent, pcsCharacter);
		}
		break;
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmEventItemConvert::OnOperationRequest(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter)
{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	AuPOS	stTargetPos;
	ZeroMemory(&stTargetPos, sizeof(AuPOS));

	if (m_pcsApmEventManager->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, AGPMEVENT_ITEMCONVERT_MAX_USE_RANGE, &stTargetPos))
	{
		pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

		if (pcsCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

		if (m_pcsAgpmCharacter->HasPenalty(pcsCharacter, AGPMCHAR_PENALTY_CONVERT))
			return FALSE;

		EnumCallback(AGPMEVENT_ITEMCONVERT_CB_REQUEST, pcsEvent, pcsCharacter);
	}
	else
	{
		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_ITEMCONVERT;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsEvent->m_pcsSource->m_lID;

		m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

PVOID AgpmEventItemConvert::MakePacketRequest(INT32 lCID, ApdEvent *pcsEvent, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !pcsEvent || !pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return FALSE;

	INT8	cOperation		= AGPMEVENT_ITEMCONVERT_OPERATION_REQUEST;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_ITEMCONVERT_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventItemConvert::MakePacketGrant(INT32 lCID, ApdEvent *pcsEvent, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !pcsEvent || !pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return FALSE;

	INT8	cOperation		= AGPMEVENT_ITEMCONVERT_OPERATION_GRANT;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_ITEMCONVERT_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

PVOID AgpmEventItemConvert::MakePacketReject(INT32 lCID, ApdEvent *pcsEvent, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !pcsEvent || !pnPacketLength)
		return NULL;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return FALSE;

	INT8	cOperation		= AGPMEVENT_ITEMCONVERT_OPERATION_REJECT;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_ITEMCONVERT_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return	pvPacket;
}

BOOL AgpmEventItemConvert::SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_ITEMCONVERT_CB_REQUEST, pfCallback, pClass);
}

BOOL AgpmEventItemConvert::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_ITEMCONVERT_CB_GRANT, pfCallback, pClass);
}

BOOL AgpmEventItemConvert::SetCallbackReject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_ITEMCONVERT_CB_REJECT, pfCallback, pClass);
}

BOOL AgpmEventItemConvert::CBActionItemConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventItemConvert	*pThis				= (AgpmEventItemConvert *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	AgpdCharacterAction		*pstAction			= (AgpdCharacterAction *)	pCustData;

	if (pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

	if (pcsCharacter->m_bMove)
		pThis->m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

	ApdEvent	*pcsEvent	= pThis->m_pcsApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType, pstAction->m_csTargetBase.m_lID, APDEVENT_FUNCTION_ITEMCONVERT);

	pThis->EnumCallback(AGPMEVENT_ITEMCONVERT_CB_REQUEST, pcsEvent, pcsCharacter);

	return TRUE;
}