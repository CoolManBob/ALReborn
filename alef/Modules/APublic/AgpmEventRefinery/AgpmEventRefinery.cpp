/*===================================================

		AgpmEventRefinery.cpp

===================================================*/

#include "AgpmEventRefinery.h"

/****************************************************/
/*		The Implementation of AgpmEventRefinery		*/
/****************************************************/
//
AgpmEventRefinery::AgpmEventRefinery()
	{
	SetModuleName("AgpmEventRefinery");
	SetPacketType(AGPMEVENT_REFINERY_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT8,		1,			// Operation
							AUTYPE_PACKET,		1,			// Event Base Packet
							AUTYPE_INT32,		1,			// CID
							AUTYPE_END,			0
							);

	m_csPacketCustom.SetFlagLength(sizeof(INT8));
	m_csPacketCustom.SetFieldType(AUTYPE_INT32,		1,
								  AUTYPE_END,		0
								  );


	m_pApmEventManager = NULL;
	m_pAgpmCharacter = NULL;
	}

AgpmEventRefinery::~AgpmEventRefinery()
	{
	}


//	ApModule inherited
//=======================================
//
BOOL AgpmEventRefinery::OnAddModule()
	{
	m_pApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");

	if (!m_pApmEventManager || !m_pAgpmCharacter)
		return FALSE;

	if (!m_pAgpmCharacter->SetCallbackActionEventRefinery(CBAction, this))
		return FALSE;

	if (!m_pApmEventManager->RegisterEvent(APDEVENT_FUNCTION_REFINERY, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	return TRUE;
	}

BOOL AgpmEventRefinery::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation = -1;
	PVOID	pvPacketEventBase = NULL;
	INT32	lCID = -1;
	INT32	lExtra = 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvPacketEventBase,
						&lCID
						);
	
	if (!pvPacketEventBase)
		return FALSE;

	ApdEvent* pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEventBase);
	if(!pApdEvent)
		return FALSE;

	if (APDEVENT_FUNCTION_REFINERY != pApdEvent->m_eFunction)
		return FALSE;

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacterLock(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
	{
		pAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (cOperation)
		{
		case AGPMEVENTREFINERY_OPERATION_REQUEST:
			OnOperationRequest(pApdEvent, pAgpdCharacter);
			break;

		case AGPMEVENTREFINERY_OPERATION_GRANT:
			OnOperationGrant(pApdEvent, pAgpdCharacter);
			break;

		default :
			break;
		}

	pAgpdCharacter->m_Mutex.Release();

	return TRUE;
	}


//	Operation
//==================================================
//
BOOL AgpmEventRefinery::OnOperationRequest(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	AuPOS stTargetPos;
	memset(&stTargetPos, 0, sizeof(stTargetPos));
	
	if (m_pApmEventManager->CheckValidRange(pApdEvent, &pAgpdCharacter->m_stPos, AGPMEVENTREFINERY_MAX_USE_RANGE, &stTargetPos))
		{
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if(pAgpdCharacter->m_bMove)
			m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);

		EnumCallback(AGPMEVENTREFINERY_CB_REQUEST, pApdEvent, pAgpdCharacter);
		}
	else
		{
		// move to target position
		pAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_REFINERY;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pApdEvent->m_pcsSource->m_eType;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pApdEvent->m_pcsSource->m_lID;

		m_pAgpmCharacter->MoveCharacter(pAgpdCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
		}

	return TRUE;
	}

BOOL AgpmEventRefinery::OnOperationGrant(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	EnumCallback(AGPMEVENTREFINERY_CB_GRANT, pApdEvent, pAgpdCharacter);

	return TRUE;
	}


//	Setting callback
//===================================
//
BOOL AgpmEventRefinery::SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMEVENTREFINERY_CB_REQUEST, pfCallback, pClass);
	}

BOOL AgpmEventRefinery::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMEVENTREFINERY_CB_GRANT, pfCallback, pClass);
	}


//	Make Packet
//===================================
//
PVOID AgpmEventRefinery::MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENTREFINERY_OPERATION_REQUEST;
	
	PVOID pvPacketBase = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_REFINERY_PACKET_TYPE,
										   &cOperation,
										   pvPacketBase,
										   &lCID
										   );

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
	}

PVOID AgpmEventRefinery::MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMEVENTREFINERY_OPERATION_GRANT;
	PVOID pvPacketBase = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_REFINERY_PACKET_TYPE,
										   &cOperation,
										   pvPacketBase,
										   &lCID
										   );

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
	}


//	Callback
//====================================================
//
BOOL AgpmEventRefinery::CBAction(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventRefinery *pThis = (AgpmEventRefinery *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdCharacterAction *pstAction = (AgpdCharacterAction *) pCustData;

	if (pThis->m_pAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
		return FALSE;

	pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	
	if(pAgpdCharacter->m_bMove)
		pThis->m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);
	
	ApdEvent *pApdEvent = pThis->m_pApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType,
															  pstAction->m_csTargetBase.m_lID,
															  APDEVENT_FUNCTION_REFINERY);

	pThis->EnumCallback(AGPMEVENTREFINERY_CB_REQUEST, pApdEvent, pAgpdCharacter);
	
	return TRUE;
	}


//	Event Callbacks
//======================================================
//
BOOL AgpmEventRefinery::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventRefinery	*pThis = (AgpmEventRefinery *) pClass;
	ApdEvent		*pApdEvent = (ApdEvent *) pData;

	// You may attach your ...........
	pApdEvent->m_pvData = NULL;
	
	return TRUE;	
	}

BOOL AgpmEventRefinery::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventRefinery *pThis			= (AgpmEventRefinery *) pClass;
	ApdEvent		*pApdEvent	= (ApdEvent *) pData;

	if (pApdEvent->m_pvData)
		{
		// free
		}

	return TRUE;
	}

BOOL AgpmEventRefinery::CBEventStreamWrite(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventRefinery *pThis = (AgpmEventRefinery *) pClass;
	ApdEvent *pApdEvent = (ApdEvent *) pData;
	ApModuleStream *pStream = (ApModuleStream *) pCustData;

	/*
	AgpdQuestEventAttachData	*pcsAttachData		= (AgpdQuestEventAttachData *)	pcsEvent->m_pvData;
	if (!pcsAttachData)
		return FALSE;

	if (!pStream->WriteValue(AGPMEVENT_QUEST_STREAM_NAME_GROUP_ID, (INT32) pcsAttachData->lQuestGroupID))
	{
		OutputDebugString("AgpmEventSkillMaster::CBStreamWriteEvent() Error (1) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMEVENT_QUEST_STREAM_NAME_EVENT_END, 0))
		return FALSE;
	*/
	return TRUE;
	}

BOOL AgpmEventRefinery::CBEventStreamRead(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventRefinery *pThis = (AgpmEventRefinery *) pClass;
	ApdEvent *pApdEvent = (ApdEvent *) pData;
	ApModuleStream *pStream = (ApModuleStream *) pCustData;

	/*
	AgpdQuestEventAttachData	*pcsAttachData		= (AgpdQuestEventAttachData *)	pcsEvent->m_pvData;
	if (!pcsAttachData)
		return FALSE;

	const CHAR					*szValueName		= NULL;

	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPMEVENT_QUEST_STREAM_NAME_GROUP_ID))
		{
			INT32	lQuestGroupID	= 0;

			if (!pStream->GetValue(&lQuestGroupID))
			{
				TRACE("AgpmEventQuest::CBStreamReadEvent() GetRaceType Failed\n"); 
				ASSERT(!"AgpmEventQuest::CBStreamReadEvent() GetRaceType Failed");
			}

			pcsAttachData->lQuestGroupID = lQuestGroupID;
		}
		else if (!strcmp(szValueName, AGPMEVENT_QUEST_STREAM_NAME_EVENT_END))
			break;
	}
	*/	
	return TRUE;
	}

BOOL AgpmEventRefinery::CBEventPacketMake(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventRefinery *pThis = (AgpmEventRefinery *) pClass;
	ApdEvent *pApdEvent = (ApdEvent *) pData;
	PVOID *ppvPacketCustom = (PVOID *) pCustData;

	*ppvPacketCustom = pThis->MakePacketCustomEvent(pApdEvent);
	return TRUE;
	}

BOOL AgpmEventRefinery::CBEventPacketReceive(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventRefinery *pThis = (AgpmEventRefinery *) pClass;
	ApdEvent *pApdEvent = (ApdEvent *) pData;
	PVOID pvPacketCustom = (PVOID) pCustData;

	return pThis->ParsePacketCustomEvent(pApdEvent, pvPacketCustom);
	}


//	Custom Event Packet(Embedded)
//=======================================================
//
PVOID AgpmEventRefinery::MakePacketCustomEvent(ApdEvent *pApdEvent)
	{
	if (!pApdEvent || !pApdEvent->m_pvData)
		return NULL;

	/*
	AgpdQuestEventAttachData	*pcsAttachData	= (AgpdQuestEventAttachData *)	pcsEvent->m_pvData;
	INT32	lQuestGroupID	= (INT32) pcsAttachData->lQuestGroupID;
	return m_csPacketCustom.MakePacket(FALSE, NULL, 0, &lQuestGroupID);	
	*/
	return NULL;
	}

BOOL AgpmEventRefinery::ParsePacketCustomEvent(ApdEvent *pApdEvent, PVOID pvPacketCustom)
	{
	if (!pApdEvent || !pApdEvent->m_pvData || !pvPacketCustom)
		return FALSE;

	/*
	INT32 lQuestGroupID = 0;
	m_csPacketCustom.GetField(FALSE, pvPacketCustomData, 0,	&lQuestGroupID);
	AgpdQuestEventAttachData	*pcsAttachData	= (AgpdQuestEventAttachData *)	pcsEvent->m_pvData;
	pcsAttachData->lQuestGroupID = lQuestGroupID;
	*/

	return TRUE;
	}