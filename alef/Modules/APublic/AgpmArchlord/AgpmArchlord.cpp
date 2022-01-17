#include "AgpmArchlord.h"
#include "ApDefine.h"
#include "AgpmCharacter.h"

AgpmArchlord::AgpmArchlord()
{
	SetModuleName("AgpmArchlord");
	SetPacketType(AGPMARCHLORD_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));

	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1, // Operation
							AUTYPE_CHAR,			AGPACHARACTER_MAX_CHARACTER_TITLE,	// archlord name
							AUTYPE_CHAR,			AGPACHARACTER_MAX_CHARACTER_TITLE,	// guard name
							AUTYPE_INT8,			1, // set guard result
							AUTYPE_INT32,			1, // current step
							AUTYPE_INT32,			1, // count of current guard
							AUTYPE_INT32,			1, // count of max guard
							AUTYPE_INT32,			1, // message id
							AUTYPE_END
							);

	m_pcsAgpmCharacter = NULL;
	m_eCurrentStep = AGPMARCHLORD_STEP_NONE;
}

AgpmArchlord::~AgpmArchlord()
{
}

BOOL AgpmArchlord::OnAddModule()
{
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsApmEventManager = (ApmEventManager *) GetModule(_T("ApmEventManager"));

	if (!m_pcsAgpmCharacter || !m_pcsApmEventManager)
		return FALSE;

	if(!m_pcsAgpmCharacter->SetCallbackIsArchlord(CBIsArchlord, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionEventArchlord(CBEventAction, this))
		return FALSE;

	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_ARCHLORD, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmArchlord::OnInit()
{
	return TRUE;
}

BOOL AgpmArchlord::OnIdle()
{
	return TRUE;
}

BOOL AgpmArchlord::OnDestroy()
{
	return TRUE;
}

// packet operations
BOOL AgpmArchlord::SetCallbackOperationSetArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_OPERATION_SET_ARCHLORD, pfCallback, pClass);
}

BOOL AgpmArchlord::SetCallbackOperationCancelArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_OPERATION_CANCEL_ARCHLORD, pfCallback, pClass);
}

BOOL AgpmArchlord::SetCallbackOperationSetGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_OPERATION_SET_GUARD, pfCallback, pClass);
}

BOOL AgpmArchlord::SetCallbackOperationCancelGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_OPERATION_CANCEL_GUARD, pfCallback, pClass);
}

BOOL AgpmArchlord::SetCallbackOperationGuardInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_OPERATION_GUARD_INFO, pfCallback, pClass);
}

BOOL AgpmArchlord::SetCallbackOperationCurrentStep(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_OPERATION_CURRENT_STEP, pfCallback, pClass);
}

BOOL AgpmArchlord::SetCallbackOperationMessageId(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_OPERATION_MESSAGE_ID, pfCallback, pClass);
}

// callback operations
BOOL AgpmArchlord::SetCallbackSetArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_SET_ARCHLORD, pfCallback, pClass);
}

BOOL AgpmArchlord::SetCallbackCancelArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_CANCEL_ARCHLORD, pfCallback, pClass);
}

BOOL AgpmArchlord::SetCallbackEventRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_EVENT_REQUEST, pfCallback, pClass);
}

BOOL AgpmArchlord::SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMARCHLORD_CB_EVENT_GRANT, pfCallback, pClass);
}

AgpmArchlordStep AgpmArchlord::GetCurrentStep()
{
	return m_eCurrentStep;
}

void AgpmArchlord::SetCurrentStep(AgpmArchlordStep eStep)
{
	m_eCurrentStep = eStep;
}

BOOL AgpmArchlord::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || !pstCheckArg || nSize < sizeof(PACKET_HEADER))
		return FALSE;

	INT8 cOperation = -1;
	CHAR* szArchlordID = NULL;
	CHAR* szGuardID = NULL;
	INT8 cResult = -1;
	INT32 lStep = -1;
	INT32 lCurrentGuard = -1;
	INT32 lMaxGuard = -1;
	INT32 lMessageID = -1;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&szArchlordID,
						&szGuardID,
						&cResult,
						&lStep,
						&lCurrentGuard,
						&lMaxGuard,
						&lMessageID);

	if (!pstCheckArg->bReceivedFromServer &&
		pstCheckArg->lSocketOwnerID != AP_INVALID_CID)
		return FALSE;

	switch (cOperation)
	{
	case AGPMARCHLORD_OPERATION_SET_ARCHLORD:
		{
			if (strlen(szArchlordID) > AGPDCHARACTER_MAX_ID_LENGTH)
				return FALSE;

			if (SetArchlord(szArchlordID))
				EnumCallback(AGPMARCHLORD_CB_OPERATION_SET_ARCHLORD, szArchlordID, NULL);
		}
		break;

	case AGPMARCHLORD_OPERATION_CANCEL_ARCHLORD:
		{
			if (strlen(szArchlordID) > AGPDCHARACTER_MAX_ID_LENGTH)
				return FALSE;

			CancelArchlord(szArchlordID);

			EnumCallback(AGPMARCHLORD_CB_OPERATION_CANCEL_ARCHLORD, szArchlordID, NULL);
		}
		break;

	case AGPMARCHLORD_OPERATION_SET_GUARD:
		{
			EnumCallback(AGPMARCHLORD_CB_OPERATION_SET_GUARD, szGuardID, &cResult);
		}
		break;

	case AGPMARCHLORD_OPERATION_CANCEL_GUARD:
		{
			EnumCallback(AGPMARCHLORD_CB_OPERATION_CANCEL_GUARD, szGuardID, &cResult);
		}
		break;

	case AGPMARCHLORD_OPERATION_GUARD_INFO:
		{
			EnumCallback(AGPMARCHLORD_CB_OPERATION_GUARD_INFO, &lCurrentGuard, &lMaxGuard);
		}
		break;

	case AGPMARCHLORD_OPERATION_STEP:
		{
			SetCurrentStep((AgpmArchlordStep)lStep);
			EnumCallback(AGPMARCHLORD_CB_OPERATION_CURRENT_STEP, &lStep, NULL);
		}
		break;

	case AGPMARCHLORD_OPERATION_MESSAGE_ID:
		{
			EnumCallback(AGPMARCHLORD_CB_OPERATION_MESSAGE_ID, &lMessageID, NULL);
		}
		break;
	
	case AGPMARCHLORD_OPERATION_EVENT_REQUEST :
		{
		//ApdEvent* pApdEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvPacketEmb);
		//if(!pApdEvent)
		//	return FALSE;

		//if (APDEVENT_FUNCTION_ARCHLORD != pApdEvent->m_eFunction)
		//	return FALSE;
		//		
		//OnOperationEventRequest(pApdEvent, pcsCharacter);
		}
		break;

	case AGPMARCHLORD_OPERATION_EVENT_GRANT :
		{
		//ApdEvent* pApdEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvPacketEmb);
		//if(!pApdEvent)
		//	return FALSE;

		//if (APDEVENT_FUNCTION_ARCHLORD != pApdEvent->m_eFunction)
		//	return FALSE;
		//		
		//OnOperationEventGrant(pApdEvent, pcsCharacter);
		}
		break;
	};

	return TRUE;
}

BOOL AgpmArchlord::OnOperationEventRequest(ApdEvent *pApdEvent, AgpdCharacter *pcsCharacter)
	{
	if (!pApdEvent || !pcsCharacter)
		return FALSE;

	AuPOS stTargetPos;
	memset(&stTargetPos, 0, sizeof(stTargetPos));
	
	if (m_pcsApmEventManager->CheckValidRange(pApdEvent, &pcsCharacter->m_stPos, AGPMARCHLORD_MAX_USE_RANGE, &stTargetPos))
		{
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if(pcsCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

		EnumCallback(AGPMARCHLORD_CB_EVENT_REQUEST, pcsCharacter, pApdEvent);
		}
	else
		{
		// move to target position
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_ARCHLORD;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pApdEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pApdEvent->m_pcsSource->m_lID;

		m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION , FALSE, TRUE, FALSE, TRUE, FALSE);
		}

	return TRUE;
	}

BOOL AgpmArchlord::OnOperationEventGrant(ApdEvent *pApdEvent, AgpdCharacter *pcsCharacter)
{
	if (!pApdEvent || !pcsCharacter)
		return FALSE;

	EnumCallback(AGPMARCHLORD_CB_EVENT_GRANT, pcsCharacter, pApdEvent);

	return TRUE;
}

BOOL AgpmArchlord::SetArchlord(CHAR *szID)
{
	ASSERT(NULL != szID);

	// 새로운 아크로드를 설정한다.
	if (false == m_szArchlord.SetText(szID))
		return FALSE;

	EnumCallback(AGPMARCHLORD_CB_SET_ARCHLORD, szID, NULL);
		
	return TRUE;
}

BOOL AgpmArchlord::IsArchlord(CHAR *szID)
{
	ASSERT(NULL != szID);	

	if (m_szArchlord.IsEmpty())
		return FALSE;

	if (COMPARE_EQUAL != m_szArchlord.Compare(szID))
		return FALSE;

	return TRUE;
}

BOOL AgpmArchlord::CancelArchlord(CHAR *szID)
{
	ASSERT(NULL != szID);

	m_szArchlord.Clear();

	EnumCallback(AGPMARCHLORD_CB_CANCEL_ARCHLORD, szID, NULL);

	return TRUE;
}

PVOID AgpmArchlord::MakePacketSetArchlord(INT16 *pnPacketLength, char *szArchlordID)
{
	INT8	cOperation	= AGPMARCHLORD_OPERATION_SET_ARCHLORD;

	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMARCHLORD_PACKET_TYPE,
												&cOperation,
												szArchlordID,
												NULL,
												0,
												0,
												0,
												0,
												0);
	return	pvPacket;	
}

PVOID AgpmArchlord::MakePacketCancelArchlord(INT16 *pnPacketLength, char *szArchlordID)
{
	INT8	cOperation	= AGPMARCHLORD_OPERATION_CANCEL_ARCHLORD;

	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMARCHLORD_PACKET_TYPE,
												&cOperation,
												szArchlordID, 
												NULL,
												0,
												0,
												0,
												0,
												0);
	return	pvPacket;	
}

PVOID AgpmArchlord::MakePacketGuardInfo(INT16 *pnPacketLength, INT32 lCurrentGuard, INT32 lMaxGuard)
{
	INT8 cOperation = AGPMARCHLORD_OPERATION_GUARD_INFO;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMARCHLORD_PACKET_TYPE,
												&cOperation,
												0,
												0,
												0,
												0,
												&lCurrentGuard,
												&lMaxGuard,
												0);

	return pvPacket;
}

PVOID AgpmArchlord::MakePacketSetGuard(INT16 *pnPacketLength, char* szGuardID, CHAR cResult)
{
	INT8 cOperation = AGPMARCHLORD_OPERATION_SET_GUARD;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMARCHLORD_PACKET_TYPE,
												&cOperation,
												NULL,
												szGuardID,
												&cResult,
												0,
												0,
												0,
												0);

	return pvPacket;	
}

PVOID AgpmArchlord::MakePacketCancelGuard(INT16 *pnPacketLength, char* szGuardID, CHAR cResult)
{
	INT8 cOperation = AGPMARCHLORD_CB_OPERATION_CANCEL_GUARD;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMARCHLORD_PACKET_TYPE,
												&cOperation,
												NULL,
												szGuardID,
												&cResult,
												0,
												0,
												0,
												0);

	return pvPacket;	
}

PVOID AgpmArchlord::MakePacketCurrentStep(INT16 *pnPacketLength, INT32 lStep)
{
	INT8 cOperation = AGPMARCHLORD_CB_OPERATION_CURRENT_STEP;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMARCHLORD_PACKET_TYPE,
												&cOperation,
												NULL,
												NULL,
												0,
												&lStep,
												0,
												0,
												0);

	return pvPacket;
}

PVOID AgpmArchlord::MakepacketMessageID(INT16 *pnPacketLength, AgpmArchlordMessageId eMessageID)
{
	INT8 cOperation = AGPMARCHLORD_CB_OPERATION_MESSAGE_ID;
	INT32 lMessageID = (INT32)eMessageID;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMARCHLORD_PACKET_TYPE,
												&cOperation,
												NULL,
												NULL,
												0,
												0,
												0,
												0,
												&lMessageID);

	return pvPacket;
}

PVOID AgpmArchlord::MakePacketEventRequest(INT16 *pnPacketLength, ApdEvent *pApdEvent, INT32 lCID)
{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMARCHLORD_OPERATION_EVENT_REQUEST;
	
	PVOID pvPacketEvent = m_pcsApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketEvent)
		return NULL;

	//###################
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMARCHLORD_PACKET_TYPE,
										   &cOperation,
										   NULL,
										   NULL,
										   0,
										   0,
										   0,
										   0,
										   0,
										   lCID,
										   pvPacketEvent
										   );

	return pvPacket;
}


PVOID AgpmArchlord::MakePacketEventGrant(INT16 *pnPacketLength, ApdEvent *pApdEvent, INT32 lCID)
{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMARCHLORD_OPERATION_EVENT_GRANT;
	PVOID pvPacketEvent = m_pcsApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketEvent)
		return NULL;

	//################################
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMARCHLORD_PACKET_TYPE,
										   &cOperation,
										   NULL,
										   NULL,
										   0,
										   0,
										   0,
										   0,
										   0,
										   lCID,
										   pvPacketEvent
										   );
	return pvPacket;
}

BOOL AgpmArchlord::CBIsArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpmArchlord* pThis = static_cast<AgpmArchlord*>(pClass);

	return pThis->IsArchlord(static_cast<CHAR*>(pData));
}

BOOL AgpmArchlord::CBEventAction(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmArchlord *pThis = (AgpmArchlord *) pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pData;
	AgpdCharacterAction *pstAction = (AgpdCharacterAction *) pCustData;

	pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	
	if(pcsCharacter->m_bMove)
		pThis->m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
	
	ApdEvent *pApdEvent = pThis->m_pcsApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType,
															  pstAction->m_csTargetBase.m_lID,
															  APDEVENT_FUNCTION_ARCHLORD);

	pThis->EnumCallback(AGPMARCHLORD_CB_EVENT_REQUEST, pApdEvent, pcsCharacter);
	
	return TRUE;
}
