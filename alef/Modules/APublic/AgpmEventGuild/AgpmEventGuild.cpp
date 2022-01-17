// AgpmEventGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 13.

#include "AgpmEventGuild.h"

AgpmEventGuild::AgpmEventGuild()
{
	SetModuleName("AgpmEventGuild");
	
	SetPacketType(AGPMEVENT_GUILD_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,			// Operation
							AUTYPE_PACKET,		1,			// Event Base Packet
							AUTYPE_INT32,		1,			// CID
							AUTYPE_END,			0);

	m_pcsAgpmCharacter = NULL;
	m_pcsApmEventManager = NULL;
}

AgpmEventGuild::~AgpmEventGuild()
{
}

BOOL AgpmEventGuild::OnAddModule()
{
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsApmEventManager = (ApmEventManager*)GetModule("ApmEventManager");
	
	if(!m_pcsAgpmCharacter || !m_pcsApmEventManager)
		return FALSE;

	if(!m_pcsAgpmCharacter->SetCallbackActionEventGuild(CBActionGuild, this))
		return FALSE;

	if(!m_pcsAgpmCharacter->SetCallbackActionEventGuildWarehouse(CBActionGuildWarehouse, this))
		return FALSE;

	if(!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_GUILD, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	if(!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_GUILD_WAREHOUSE, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	//if(!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_WORLD_CHAMPIONSHIP, NULL, NULL, NULL, NULL, NULL, this))
	//	return FALSE;

	return TRUE;
}

BOOL AgpmEventGuild::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	PVOID pvPacketEventBase = NULL;
	INT32 lCID = -1;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvPacketEventBase,
						&lCID);
	
	if(!pvPacketEventBase)
		return FALSE;

	ApdEvent* pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvPacketEventBase);
	if(!pcsEvent)
		return FALSE;

	if(pcsEvent->m_eFunction != APDEVENT_FUNCTION_GUILD
		&& pcsEvent->m_eFunction != APDEVENT_FUNCTION_GUILD_WAREHOUSE
		&& pcsEvent->m_eFunction != APDEVENT_FUNCTION_WORLD_CHAMPIONSHIP)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch(cOperation)
	{
		case AGPMEVENT_GUILD_PACKET_REQUEST:
			OnOperationRequest(pcsEvent, pcsCharacter);
			break;

		case AGPMEVENT_GUILD_PACKET_GRANT:
			OnOperationGrant(pcsEvent, pcsCharacter);
			break;

		case AGPMEVENT_GUILD_PACKET_REQUEST_WAREHOUSE:
			OnOperationRequestWarehouse(pcsEvent, pcsCharacter);
			break;

		case AGPMEVENT_GUILD_PACKET_GRANT_WAREHOUSE:
			OnOperationGrantWarehouse(pcsEvent, pcsCharacter);
			break;

		case AGPMEVENT_GUILD_PACKET_REQUEST_WORLD_CHAMPIONSHIP:
			{
				OnOperationRequestWorldChampionship(pcsEvent, pcsCharacter);
			} break;

		case AGPMEVENT_GUILD_PACKET_GRANT_WORLD_CHAMPIONSHIP:
			{
				OnOperationGrantWorldChampionship(pcsEvent, pcsCharacter);
			} break;
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmEventGuild::OnOperationRequest(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter)
{
	if(!pcsEvent || !pcsCharacter)
		return FALSE;

	AuPOS stTargetPos;
	memset(&stTargetPos, 0, sizeof(stTargetPos));
	
	if(m_pcsApmEventManager->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, AGPMEVENT_GUILD_MAX_USE_RANGE, &stTargetPos))
	{
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if(pcsCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

		EnumCallback(AGPMEVENT_GUILD_CB_REQUEST, pcsEvent, pcsCharacter);
	}
	else
	{
		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_GUILD;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsEvent->m_pcsSource->m_lID;

		m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgpmEventGuild::OnOperationGrant(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter)
{
	if(!pcsEvent || !pcsCharacter)
		return FALSE;

	EnumCallback(AGPMEVENT_GUILD_CB_GRANT, pcsEvent, pcsCharacter);

	return TRUE;
}

BOOL AgpmEventGuild::OnOperationRequestWarehouse(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter)
{
	if(!pcsEvent || !pcsCharacter)
		return FALSE;

	AuPOS stTargetPos;
	memset(&stTargetPos, 0, sizeof(stTargetPos));
	
	if(m_pcsApmEventManager->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, AGPMEVENT_GUILD_MAX_USE_RANGE, &stTargetPos))
	{
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if(pcsCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

		EnumCallback(AGPMEVENT_GUILD_CB_REQUEST_WAREHOUSE, pcsEvent, pcsCharacter);
	}
	else
	{
		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_GUILD_WAREHOUSE;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsEvent->m_pcsSource->m_lID;

		m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgpmEventGuild::OnOperationGrantWarehouse(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter)
{
	if(!pcsEvent || !pcsCharacter)
		return FALSE;

	EnumCallback(AGPMEVENT_GUILD_CB_GRANT_WAREHOUSE, pcsEvent, pcsCharacter);

	return TRUE;
}

BOOL AgpmEventGuild::OnOperationRequestWorldChampionship(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter)
{
	if(!pcsEvent || !pcsCharacter)
		return FALSE;

	AuPOS stTargetPos;
	memset(&stTargetPos, 0, sizeof(stTargetPos));

	if(m_pcsApmEventManager->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, AGPMEVENT_GUILD_MAX_USE_RANGE, &stTargetPos))
	{
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if(pcsCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

		EnumCallback(AGPMEVENT_GUILD_CB_REQUEST_WORLD_CHAMPIONSHIP, pcsEvent, pcsCharacter);
	}
	else
	{
		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_GUILD_WAREHOUSE;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsEvent->m_pcsSource->m_lID;

		m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgpmEventGuild::OnOperationGrantWorldChampionship(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter)
{
	if(!pcsEvent || !pcsCharacter)
		return FALSE;

	EnumCallback(AGPMEVENT_GUILD_CB_GRANT_WORLD_CHAMPIONSHIP, pcsEvent, pcsCharacter);

	return TRUE;
}

BOOL AgpmEventGuild::SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_GUILD_CB_REQUEST, pfCallback, pClass);
}

BOOL AgpmEventGuild::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_GUILD_CB_GRANT, pfCallback, pClass);
}

BOOL AgpmEventGuild::SetCallbackRequestWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_GUILD_CB_REQUEST_WAREHOUSE, pfCallback, pClass);
}

BOOL AgpmEventGuild::SetCallbackGrantWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_GUILD_CB_GRANT_WAREHOUSE, pfCallback, pClass);
}

BOOL AgpmEventGuild::SetCallbackRequestWorldChampionship(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_GUILD_CB_REQUEST_WORLD_CHAMPIONSHIP, pfCallback, pClass);
}

BOOL AgpmEventGuild::SetCallbackGrantWorldChampionship(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_GUILD_CB_GRANT_WORLD_CHAMPIONSHIP, pfCallback, pClass);
}

PVOID AgpmEventGuild::MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if(!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_GUILD_PACKET_REQUEST;
	
	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if(!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_GUILD_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmEventGuild::MakePacketEventGrant(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if(!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_GUILD_PACKET_GRANT;
	
	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if(!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_GUILD_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmEventGuild::MakePacketEventRequestWarehouse(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if(!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_GUILD_PACKET_REQUEST_WAREHOUSE;
	
	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if(!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_GUILD_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmEventGuild::MakePacketEventGrantWarehouse(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if(!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_GUILD_PACKET_GRANT_WAREHOUSE;
	
	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if(!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_GUILD_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmEventGuild::MakePacketEventRequestWorldChampionship(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if(!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_GUILD_PACKET_REQUEST_WORLD_CHAMPIONSHIP;

	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if(!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_GUILD_PACKET_TYPE,
		&cOperation,
		pvPacketBase,
		&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmEventGuild::MakePacketEventGrantWorldChampionship(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if(!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENT_GUILD_PACKET_GRANT_WORLD_CHAMPIONSHIP;

	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if(!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_GUILD_PACKET_TYPE,
		&cOperation,
		pvPacketBase,
		&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

BOOL AgpmEventGuild::CBActionGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmEventGuild* pThis = (AgpmEventGuild*)pClass;
	AgpdCharacterAction* pstAction = (AgpdCharacterAction*)pCustData;

	if (pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	
	if(pcsCharacter->m_bMove)
		pThis->m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
	
	ApdEvent* pcsEvent = pThis->m_pcsApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType, pstAction->m_csTargetBase.m_lID, APDEVENT_FUNCTION_GUILD);

	pThis->EnumCallback(AGPMEVENT_GUILD_CB_REQUEST, pcsEvent, pcsCharacter);
	
	return TRUE;
}

BOOL AgpmEventGuild::CBActionGuildWarehouse(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmEventGuild* pThis = (AgpmEventGuild*)pClass;
	AgpdCharacterAction* pstAction = (AgpdCharacterAction*)pCustData;

	if (pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	
	if(pcsCharacter->m_bMove)
		pThis->m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
	
	ApdEvent* pcsEvent = pThis->m_pcsApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType, pstAction->m_csTargetBase.m_lID, APDEVENT_FUNCTION_GUILD_WAREHOUSE);

	pThis->EnumCallback(AGPMEVENT_GUILD_CB_REQUEST_WAREHOUSE, pcsEvent, pcsCharacter);
	
	return TRUE;
}