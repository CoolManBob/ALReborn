#include "AgsmRelay2.h"
#include <AgspEventSystem.h>
#include <AgsmEventSystem.h>

//////////////////////////////////////////////////////////////////////////
//
BOOL AgsmRelay2::OnParamAttendance(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	PACKET_AGSMCHARACTER_RELAY_ATTENDANCE* pPacket = (PACKET_AGSMCHARACTER_RELAY_ATTENDANCE*)pvPacket;

	switch(pPacket->nOperation)
	{
		case AGSMRELAY_PARAM_ATTENDANCE_REQUEST:
			{
				PACKET_AGSMCHARACTER_RELAY_ATTENDANCE_REQUEST* pPacket2 = (PACKET_AGSMCHARACTER_RELAY_ATTENDANCE_REQUEST*)pPacket;

				AgsdRelay2Attendance* pcsAttendance = new AgsdRelay2Attendance(ulNID, pPacket->nOperation);
				strcpy(pcsAttendance->strAccountName, pPacket2->strAccountName);
				strcpy(pcsAttendance->strCharName, pPacket2->strCharName);
				strcpy(pcsAttendance->strWorldName, pPacket2->strWorldName);
				pcsAttendance->CID = pPacket2->CID;
				pcsAttendance->NID = pPacket2->NID;
				pcsAttendance->EID = pPacket2->EID;
				pcsAttendance->STEP = pPacket2->STEP;
				pcsAttendance->Type = pPacket2->Type;

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_ATTENDANCE;
				pQuery->m_pParam = pcsAttendance;
				pQuery->SetCallback(AgsmRelay2::CBOperationResultAttendance, NULL, this, pcsAttendance);

				m_pAgsmDatabasePool->Execute(pQuery);
			} break;

		case AGSMRELAY_PARAM_ATTENDANCE_RESULT:
			{
				PACKET_AGSMCHARACTER_RELAY_ATTENDANCE_RESULT* pPacket2 = (PACKET_AGSMCHARACTER_RELAY_ATTENDANCE_RESULT*)pPacket;

				if( m_pagsmEventSystem )
					m_pagsmEventSystem->ProcessAttendanceResult(pPacket2->CID, pPacket2->NID, pPacket2->EID, pPacket2->STEP, pPacket2->Type, pPacket2->nResult);
			} break;
	}

	return TRUE;
}

BOOL	AgsmRelay2::OnParamItemGiveEvent(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	PACKET_AGSMCHARACTER_RELAY_EVENT_ITEM* pPacket = (PACKET_AGSMCHARACTER_RELAY_EVENT_ITEM*)pvPacket;

	switch(pPacket->nOperation)
	{
	case AGSMRELAY_PARAM_EVENT_USER_REQUEST:
		{
			PACKET_AGSMCHARACTER_RELAY_EVENT_USER_REQUEST *pPacket2 = (PACKET_AGSMCHARACTER_RELAY_EVENT_USER_REQUEST *)pPacket;

			AgsdRelay2EventUser *pcsEventUser = new AgsdRelay2EventUser(ulNID, pPacket->nOperation);
			strcpy(pcsEventUser->strAccountName, pPacket2->strAccountName);
			strcpy(pcsEventUser->strCharName, pPacket2->strCharName);
			strcpy(pcsEventUser->strWorldName, pPacket2->strWorldName);
			pcsEventUser->CID = pPacket2->CID;
			pcsEventUser->NID = pPacket2->NID;
			pcsEventUser->EID = pPacket2->EID;
			pcsEventUser->STEP = pPacket2->STEP;
			pcsEventUser->Type = pPacket2->Type;

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
			pQuery->m_nIndex = AGSMRELAY_QUERY_EVENTUSERCHECK;
			pQuery->m_pParam = pcsEventUser;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultEventUser, NULL, this, pcsEventUser);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGSMRELAY_PARAM_EVENT_USER_RESULT:
		{
			PACKET_AGSMCHARACTER_RELAY_EVENT_USER_RESULT* pPacket2 = (PACKET_AGSMCHARACTER_RELAY_EVENT_USER_RESULT*)pPacket;

			if( m_pagsmEventSystem )
				m_pagsmEventSystem->ProcessEventUserResult(pPacket2->CID, pPacket2->NID, pPacket2->EID, pPacket2->STEP, pPacket2->Type, pPacket2->nResult);
		}
		break;

		case AGSMRELAY_PARAM_EVENT_USER_FLAG_UPDATE:
			{
				PACKET_AGSMCHARACTER_RELAY_EVENT_USER_FLAG_UPDATE* pPacket2 = (PACKET_AGSMCHARACTER_RELAY_EVENT_USER_FLAG_UPDATE*)pPacket;

				AgsdRelay2EventUserFlagUpdate *pcsEventUserFlagUpdate = new AgsdRelay2EventUserFlagUpdate(ulNID, pPacket->nOperation);
				strcpy(pcsEventUserFlagUpdate->strAccountName, pPacket2->strAccountName);
				strcpy(pcsEventUserFlagUpdate->strCharName, pPacket2->strCharName);
				strcpy(pcsEventUserFlagUpdate->strWorldName, pPacket2->strWorldName);
				pcsEventUserFlagUpdate->CID = pPacket2->CID;
				pcsEventUserFlagUpdate->NID = pPacket2->NID;
				pcsEventUserFlagUpdate->EID = pPacket2->EID;
				pcsEventUserFlagUpdate->STEP = pPacket2->STEP;
				pcsEventUserFlagUpdate->Type = pPacket2->Type;
				pcsEventUserFlagUpdate->Flag = pPacket2->Flag;

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_EVENTUSERFLAGUPDATE;
				pQuery->m_pParam = pcsEventUserFlagUpdate;
				pQuery->SetCallback(NULL, NULL, this, pcsEventUserFlagUpdate);

				m_pAgsmDatabasePool->Execute(pQuery);
			} break;
	}

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultAttendance(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2Attendance *pAgsdRelay2 = (AgsdRelay2Attendance *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSMCHARACTER_RELAY_ATTENDANCE_RESULT pPacket;
	pPacket.CID = pAgsdRelay2->CID;
	pPacket.NID = pAgsdRelay2->NID;
	pPacket.EID = pAgsdRelay2->EID;
	pPacket.STEP = pAgsdRelay2->STEP;
	pPacket.Type = pAgsdRelay2->Type;
	pPacket.nResult = pAgsdRelay2->m_nCode;

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;

}

BOOL AgsmRelay2::CBOperationResultEventUser(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2EventUser *pAgsdRelay2 = (AgsdRelay2EventUser *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSMCHARACTER_RELAY_EVENT_USER_RESULT pPacket;
	pPacket.CID = pAgsdRelay2->CID;
	pPacket.NID = pAgsdRelay2->NID;
	pPacket.EID = pAgsdRelay2->EID;
	pPacket.STEP = pAgsdRelay2->STEP;
	pPacket.Type = pAgsdRelay2->Type;
	pPacket.nResult = pAgsdRelay2->m_nCode;

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

AgsdRelay2Attendance::AgsdRelay2Attendance(UINT32 ulNID, INT16 Operation)
	: m_nCode(-1)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strCharName, 0, sizeof(strCharName));
	memset(strAccountName, 0, sizeof(strAccountName));
	CID = 0;
	NID = 0;
	EID = 0;
	STEP = 0;
	Type = 0;
}

AgsdRelay2Attendance::~AgsdRelay2Attendance()
{

}

void AgsdRelay2Attendance::Release()
{ 
	delete this;
}

BOOL AgsdRelay2Attendance::SetParamExecute(AuStatement* pStatement)
{
	switch(nOperation)
	{
	case AGSMRELAY_PARAM_ATTENDANCE_REQUEST:
		{
			// in
			pStatement->SetParam( 0, &EID );
			pStatement->SetParam( 1, &Type );
			pStatement->SetParam( 2, strAccountName, sizeof(strAccountName) );
			pStatement->SetParam( 3, strWorldName, sizeof(strWorldName) );
			pStatement->SetParam( 4, strCharName, sizeof(strCharName) );

			// out
			pStatement->SetParam( 5, &m_nCode, TRUE);
		} break;
	}

	return TRUE;
}

AgsdRelay2EventUser::AgsdRelay2EventUser(UINT32 ulNID, INT16 Operation)
: m_nCode(-1)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strCharName, 0, sizeof(strCharName));
	memset(strAccountName, 0, sizeof(strAccountName));
	memset(strWorldName, 0, sizeof(strWorldName));
	CID = 0;
	NID = 0;
	EID = 0;
	STEP = 0;
	Type = 0;
}

AgsdRelay2EventUser::~AgsdRelay2EventUser()
{

}

void AgsdRelay2EventUser::Release()
{ 
	delete this;
}

BOOL AgsdRelay2EventUser::SetParamExecute(AuStatement* pStatement)
{
	switch(nOperation)
	{
	case AGSMRELAY_PARAM_EVENT_USER_REQUEST:
		{
			// in
			pStatement->SetParam( 0, &EID );
			pStatement->SetParam( 1, &Type );
			pStatement->SetParam( 2, strAccountName, sizeof(strAccountName) );
			pStatement->SetParam( 3, strWorldName, sizeof(strWorldName) );
			pStatement->SetParam( 4, strCharName, sizeof(strCharName) );

			// out
			pStatement->SetParam( 5, &m_nCode, TRUE);
		} break;
	}

	return TRUE;
}

AgsdRelay2EventUserFlagUpdate::AgsdRelay2EventUserFlagUpdate(UINT32 ulNID, INT16 Operation)
: m_nCode(-1)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strCharName, 0, sizeof(strCharName));
	memset(strAccountName, 0, sizeof(strAccountName));
	memset(strWorldName, 0, sizeof(strWorldName));
	CID = 0;
	NID = 0;
	EID = 0;
	STEP = 0;
	Type = 0;
	Flag = 0;
}

AgsdRelay2EventUserFlagUpdate::~AgsdRelay2EventUserFlagUpdate()
{

}

void AgsdRelay2EventUserFlagUpdate::Release()
{ 
	delete this;
}

BOOL AgsdRelay2EventUserFlagUpdate::SetParamExecute(AuStatement* pStatement)
{
	switch(nOperation)
	{
	case AGSMRELAY_PARAM_EVENT_USER_FLAG_UPDATE:
		{
			// in
			pStatement->SetParam( 0, &EID );
			pStatement->SetParam( 1, &Type );
			pStatement->SetParam( 2, strAccountName, sizeof(strAccountName) );
			pStatement->SetParam( 3, strWorldName, sizeof(strWorldName) );
			pStatement->SetParam( 4, strCharName, sizeof(strCharName) );
			pStatement->SetParam( 5, &Flag );
		} break;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 이름 변경 시스템 - arycoat 2009.4.
BOOL AgsmRelay2::OnParamChangeName(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	PACKET_AGSMCHARACTER_RELAY_CHANGENAME* pPacket = (PACKET_AGSMCHARACTER_RELAY_CHANGENAME*)pvPacket;

	switch(pPacket->nOperation)
	{
		case AGSMRELAY_PARAM_CHANGENAME_REQUEST:
			{
				PACKET_AGSMCHARACTER_RELAY_CHANGENAME_REQUEST *pPacket2 = (PACKET_AGSMCHARACTER_RELAY_CHANGENAME_REQUEST *)pPacket;

				AgsdRelay2ChangeName *pcsChangeName = new AgsdRelay2ChangeName(ulNID, pPacket->nOperation);
				strcpy(pcsChangeName->strNewName, pPacket2->strNewName);
				strcpy(pcsChangeName->strOldName, pPacket2->strOldName);
				pcsChangeName->CID = pPacket2->CID;
				pcsChangeName->NID = pPacket2->NID;
				pcsChangeName->EID = pPacket2->EID;
				pcsChangeName->STEP = pPacket2->STEP;

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_CHANGENAME;
				pQuery->m_pParam = pcsChangeName;
				pQuery->SetCallback(AgsmRelay2::CBOperationResultChangeName, NULL, this, pcsChangeName);

				m_pAgsmDatabasePool->Execute(pQuery);
			}
			break;
		case AGSMRELAY_PARAM_CHANGENAME_RESULT:
			{
				PACKET_AGSMCHARACTER_RELAY_CHANGENAME_RESULT* pPacket2 = (PACKET_AGSMCHARACTER_RELAY_CHANGENAME_RESULT*)pPacket;

				if( m_pagsmEventSystem )
					m_pagsmEventSystem->ProcessChangeNameResult(pPacket2->CID, pPacket2->NID, pPacket2->EID, pPacket2->STEP, pPacket2->nResult);
			}
			break;
	}
	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultChangeName(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2ChangeName *pAgsdRelay2 = (AgsdRelay2ChangeName *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSMCHARACTER_RELAY_CHANGENAME_RESULT pPacket;
	pPacket.CID = pAgsdRelay2->CID;
	pPacket.NID = pAgsdRelay2->NID;
	pPacket.EID = pAgsdRelay2->EID;
	pPacket.STEP = pAgsdRelay2->STEP;
	pPacket.nResult = pAgsdRelay2->m_nCode;

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

AgsdRelay2ChangeName::AgsdRelay2ChangeName(UINT32 ulNID, INT16 Operation)
	: m_nCode(-1)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strOldName, 0, sizeof(strOldName));
	memset(strNewName, 0, sizeof(strNewName));
	CID = 0;
	NID = 0;
	EID = 0;
	STEP = 0;

}

AgsdRelay2ChangeName::~AgsdRelay2ChangeName()
{

}

void AgsdRelay2ChangeName::Release()
{ 
	delete this;
}

BOOL AgsdRelay2ChangeName::SetParamExecute(AuStatement* pStatement)
{
	switch(nOperation)
	{
		case AGSMRELAY_PARAM_CHANGENAME_REQUEST:
			{
				// in
				pStatement->SetParam( 0, strOldName, sizeof(strOldName) );
				pStatement->SetParam( 1, strNewName, sizeof(strNewName) );
				
				// out
				pStatement->SetParam( 2, &m_nCode, TRUE);
			} break;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 캐릭터 봉인 시스템 - arycoat 2009.4.
BOOL AgsmRelay2::OnParamCerariumOrb(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	PACKET_RELAY_CERARIUMORB* pPacket = (PACKET_RELAY_CERARIUMORB*)pvPacket;

	switch(pPacket->nOperation)
	{
		case AGSMRELAY_PARAM_CERARIUMORB_LIST_REQUEST:
			{
				PACKET_RELAY_CERARIUMORB_LIST_REQUEST *pPacket2 = (PACKET_RELAY_CERARIUMORB_LIST_REQUEST *)pPacket;

				AgsdRelay2CerariumOrb *pcsCerariumOrb = new AgsdRelay2CerariumOrb(ulNID, pPacket->nOperation);
				strcpy(pcsCerariumOrb->strAccountName, pPacket2->strAccountName);
				strcpy(pcsCerariumOrb->strWorldName, pPacket2->strWorldName);
				pcsCerariumOrb->CID = pPacket2->CID;
				pcsCerariumOrb->NID = pPacket2->NID;
				pcsCerariumOrb->EID = pPacket2->EID;
				pcsCerariumOrb->STEP = pPacket2->STEP;

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_CERARIUMORB_REQUEST_LIST;
				pQuery->m_pParam = pcsCerariumOrb;
				pQuery->SetCallback(AgsmRelay2::CBOperationResultCerariumOrb, NULL, this, pcsCerariumOrb);

				m_pAgsmDatabasePool->Execute(pQuery);
			}
			break;
		case AGSMRELAY_PARAM_CERARIUMORB_LIST_RESULT:
			{
				PACKET_RELAY_CERARIUMORB_LIST_RESULT* pPacket2 = (PACKET_RELAY_CERARIUMORB_LIST_RESULT*)pPacket;

				if( m_pagsmEventSystem )
					m_pagsmEventSystem->CharacterSealingResultList(pPacket2->CID, pPacket2->NID, pPacket2->EID, pPacket2->STEP, (CHAR**)pPacket2->strCharName, (INT32*)pPacket2->lSlot);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_CHARACTERNAME_REQUEST:
			{
				PACKET_RELAY_CERARIUMORB_CHARACTERNAME_REQUEST* pPacket2 = (PACKET_RELAY_CERARIUMORB_CHARACTERNAME_REQUEST*)pPacket;

				AgsdRelay2CerariumOrb *pcsCerariumOrb = new AgsdRelay2CerariumOrb(ulNID, pPacket->nOperation);
				strcpy(pcsCerariumOrb->strAccountName, pPacket2->strAccountName);
				strcpy(pcsCerariumOrb->strWorldName, pPacket2->strWorldName);
				pcsCerariumOrb->CID = pPacket2->CID;
				pcsCerariumOrb->NID = pPacket2->NID;
				pcsCerariumOrb->EID = pPacket2->EID;
				pcsCerariumOrb->STEP = pPacket2->STEP;
				pcsCerariumOrb->Slot = pPacket2->Slot;

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_CERARIUMORB_REQUEST_CHARACTERNAME;
				pQuery->m_pParam = pcsCerariumOrb;
				pQuery->SetCallback(AgsmRelay2::CBOperationResultCerariumOrb, NULL, this, pcsCerariumOrb);

				m_pAgsmDatabasePool->Execute(pQuery);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_CHARACTERNAME_RESULT:
			{
				PACKET_RELAY_CERARIUMORB_LIST_RESULT* pPacket2 = (PACKET_RELAY_CERARIUMORB_LIST_RESULT*)pPacket;

				if( m_pagsmEventSystem )
					m_pagsmEventSystem->CharacterSealingResultCharacterName(pPacket2->CID, pPacket2->NID, pPacket2->EID, pPacket2->STEP, (CHAR*)pPacket2->strCharName);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING:
			{
				PACKET_RELAY_CERARIUMORB_SEALING* pPacket2 = (PACKET_RELAY_CERARIUMORB_SEALING*)pPacket;

				AgsdRelay2CerariumOrb *pcsCerariumOrb = new AgsdRelay2CerariumOrb(ulNID, pPacket->nOperation);
				strcpy(pcsCerariumOrb->strCharName, pPacket2->strCharName);
				pcsCerariumOrb->CID = pPacket2->CID;
				pcsCerariumOrb->NID = pPacket2->NID;
				pcsCerariumOrb->EID = pPacket2->EID;
				pcsCerariumOrb->STEP = pPacket2->STEP;
				pcsCerariumOrb->ItemID = pPacket2->ItemID;
				pcsCerariumOrb->ullDBIID = pPacket2->ullDBIID;
				_i64toa(pcsCerariumOrb->ullDBIID, pcsCerariumOrb->szDBID, 10);

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_CERARIUMORB_SEALING;
				pQuery->m_pParam = pcsCerariumOrb;
				pQuery->SetCallback(AgsmRelay2::CBOperationResultCerariumOrb, NULL, this, pcsCerariumOrb);

				m_pAgsmDatabasePool->Execute(pQuery);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING_RESULT:
			{
				PACKET_RELAY_CERARIUMORB_SEALING_RESULT* pPacket2 = (PACKET_RELAY_CERARIUMORB_SEALING_RESULT*)pPacket;

				if( m_pagsmEventSystem )
					m_pagsmEventSystem->CharacterSealingResult(pPacket2->CID, pPacket2->NID, pPacket2->EID, pPacket2->STEP, pPacket2->ItemID, pPacket2->ullDBIID, pPacket2->nResult);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING_DATA_REQUEST:
			{
				PACKET_RELAY_CERARIUMORB_SEALING_DATA_REQUEST* pPacket2 = (PACKET_RELAY_CERARIUMORB_SEALING_DATA_REQUEST*)pPacket;

				AgsdRelay2CerariumOrb *pcsCerariumOrb = new AgsdRelay2CerariumOrb(ulNID, pPacket->nOperation);
				pcsCerariumOrb->ItemID = pPacket2->ItemID;
				pcsCerariumOrb->ullDBIID = pPacket2->ullDBIID;
				_i64toa(pcsCerariumOrb->ullDBIID, pcsCerariumOrb->szDBID, 10);

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_CERARIUMORB_SEALING_DATA;
				pQuery->m_pParam = pcsCerariumOrb;
				pQuery->SetCallback(AgsmRelay2::CBOperationResultCerariumOrb, NULL, this, pcsCerariumOrb);

				m_pAgsmDatabasePool->Execute(pQuery);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING_DATA_RESULT:
			{
				PACKET_RELAY_CERARIUMORB_SEALING_DATA_RESULT* pPacket2 = (PACKET_RELAY_CERARIUMORB_SEALING_DATA_RESULT*)pPacket;

				if( m_pagsmEventSystem )
					m_pagsmEventSystem->ReceiveSealingData(pPacket2->ItemID, &pPacket2->SealData);

			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING_ALL_DATA_REQUEST:
			{
				PACKET_RELAY_CERARIUMORB_SEALING_ALL_DATA_REQUEST* pPacket2 = (PACKET_RELAY_CERARIUMORB_SEALING_ALL_DATA_REQUEST*)pPacket;

				AgsdRelay2CerariumOrb *pcsCerariumOrb = new AgsdRelay2CerariumOrb(ulNID, pPacket->nOperation);

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_CERARIUMORB_SEALING_DATA_ALL;
				pQuery->m_pParam = pcsCerariumOrb;
				pQuery->SetCallback(AgsmRelay2::CBOperationResultCerariumOrb, NULL, this, pcsCerariumOrb);

				m_pAgsmDatabasePool->Execute(pQuery);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_RELEASE_SEAL_REQUEST:
			{
				PACKET_RELAY_CERARIUMORB_RELEASE_SEAL_REQUEST* pPacket2 = (PACKET_RELAY_CERARIUMORB_RELEASE_SEAL_REQUEST*)pPacket;

				AgsdRelay2CerariumOrb *pcsCerariumOrb = new AgsdRelay2CerariumOrb(ulNID, pPacket->nOperation);
				strcpy(pcsCerariumOrb->strCharName, pPacket2->strCharName);
				pcsCerariumOrb->CID = pPacket2->CID;
				pcsCerariumOrb->NID = pPacket2->NID;
				pcsCerariumOrb->EID = pPacket2->EID;
				pcsCerariumOrb->STEP = pPacket2->STEP;
				pcsCerariumOrb->ItemID = pPacket2->ItemID;
				pcsCerariumOrb->ullDBIID = pPacket2->ullDBIID;
				_i64toa(pcsCerariumOrb->ullDBIID, pcsCerariumOrb->szDBID, 10);

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_CERARIUMORB_RELEASE_SEAL_REQUEST;
				pQuery->m_pParam = pcsCerariumOrb;
				pQuery->SetCallback(AgsmRelay2::CBOperationResultCerariumOrb, NULL, this, pcsCerariumOrb);

				m_pAgsmDatabasePool->Execute(pQuery);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_RELEASE_SEAL_RESULT:
			{
				PACKET_RELAY_CERARIUMORB_RELEASE_SEALING_RESULT* pPacket2 = (PACKET_RELAY_CERARIUMORB_RELEASE_SEALING_RESULT*)pPacket;
				
				if( m_pagsmEventSystem )
					m_pagsmEventSystem->CharacterReleaseSealingResult(pPacket2->CID, pPacket2->NID, pPacket2->EID, pPacket2->STEP, pPacket2->ItemID, pPacket2->ullDBIID, pPacket2->nResult);
			} break;
	}

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultCerariumOrb(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2CerariumOrb *pAgsdRelay2 = (AgsdRelay2CerariumOrb *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch(pAgsdRelay2->nOperation)
	{
		case AGSMRELAY_PARAM_CERARIUMORB_LIST_REQUEST:
			{
				INT32 lCount = pRowset->GetRowCount();

				PACKET_RELAY_CERARIUMORB_LIST_RESULT pPacket;
				pPacket.CID = pAgsdRelay2->CID;
				pPacket.NID = pAgsdRelay2->NID;
				pPacket.EID = pAgsdRelay2->EID;
				pPacket.STEP = pAgsdRelay2->STEP;

				for(INT32 i = 0; i < lCount; i++)
				{
					CHAR* strCharID = (CHAR*)pRowset->Get(i, 0);
					if(strCharID)
						strncpy(pPacket.strCharName[i], strCharID, AGPDCHARACTER_MAX_ID_LENGTH);

					CHAR* lSlot = (CHAR*)pRowset->Get(i, 1);
					if(lSlot)
						pPacket.lSlot[i] = (INT32)atoi(lSlot);
				}

				AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_CHARACTERNAME_REQUEST:
			{
				INT32 lCount = pRowset->GetRowCount();

				PACKET_RELAY_CERARIUMORB_CHARACTERNAME_RESULT pPacket;
				pPacket.CID = pAgsdRelay2->CID;
				pPacket.NID = pAgsdRelay2->NID;
				pPacket.EID = pAgsdRelay2->EID;
				pPacket.STEP = pAgsdRelay2->STEP;

				if(lCount == 1)
				{
					CHAR* strCharID = (CHAR*)pRowset->Get(0, 0);
					if(strCharID)
						strncpy(pPacket.strCharName, strCharID, AGPDCHARACTER_MAX_ID_LENGTH);
				}

				AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING:
			{
				PACKET_RELAY_CERARIUMORB_SEALING_RESULT pPacket;
				pPacket.CID = pAgsdRelay2->CID;
				pPacket.NID = pAgsdRelay2->NID;
				pPacket.EID = pAgsdRelay2->EID;
				pPacket.STEP = pAgsdRelay2->STEP;
				pPacket.ItemID = pAgsdRelay2->ItemID;
				pPacket.ullDBIID = pAgsdRelay2->ullDBIID;
				pPacket.nResult = pAgsdRelay2->m_nCode;

				AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING_DATA_REQUEST:
			{
				INT32 lCount = pRowset->GetRowCount();

				for(INT32 i = 0; i < lCount; i++)
				{
					AgsdSealData pSealData;

					CHAR* strDBID = (CHAR*)pRowset->Get(i, 0);
					if(strDBID)
						pSealData.DBID = (UINT64)_atoi64(strDBID);

					CHAR* strCharName = (CHAR*)pRowset->Get(i, 1);
					if(strCharName)
						strncpy(pSealData.SealData.strCharName, strCharName, AGPDCHARACTER_MAX_ID_LENGTH);

					CHAR* strTID = (CHAR*)pRowset->Get(i, 2);
					if(strTID)
						pSealData.SealData.TID = (INT32)atoi(strTID);

					CHAR* strLevel = (CHAR*)pRowset->Get(i, 3);
					if(strLevel)
						pSealData.SealData.Level = (INT32)atoi(strLevel);
					
					CHAR* strExp = (CHAR*)pRowset->Get(i, 4);
					if(strExp)
						pSealData.SealData.Exp = (INT32)atoi(strExp);

					PACKET_RELAY_CERARIUMORB_SEALING_DATA_RESULT pPacket;
					pPacket.ItemID = pAgsdRelay2->ItemID;
					memcpy(&pPacket.SealData, &pSealData, sizeof(AgsdSealData));

					AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);
				}
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING_ALL_DATA_REQUEST:
			{
				INT32 lCount = pRowset->GetRowCount();

				for(INT32 i = 0; i < lCount; i++)
				{
					AgsdSealData pSealData;

					CHAR* strDBID = (CHAR*)pRowset->Get(i, 0);
					if(strDBID)
						pSealData.DBID = (UINT64)_atoi64(strDBID);

					CHAR* strCharName = (CHAR*)pRowset->Get(i, 1);
					if(strCharName)
						strncpy(pSealData.SealData.strCharName, strCharName, AGPDCHARACTER_MAX_ID_LENGTH);

					CHAR* strTID = (CHAR*)pRowset->Get(i, 2);
					if(strTID)
						pSealData.SealData.TID = (INT32)atoi(strTID);

					CHAR* strLevel = (CHAR*)pRowset->Get(i, 3);
					if(strLevel)
						pSealData.SealData.Level = (INT32)atoi(strLevel);
					
					CHAR* strExp = (CHAR*)pRowset->Get(i, 4);
					if(strExp)
						pSealData.SealData.Exp = (INT32)atoi(strExp);

					PACKET_RELAY_CERARIUMORB_SEALING_DATA_RESULT pPacket;
					memcpy(&pPacket.SealData, &pSealData, sizeof(AgsdSealData));

					AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);
				}
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_RELEASE_SEAL_REQUEST:
			{
				PACKET_RELAY_CERARIUMORB_RELEASE_SEALING_RESULT pPacket;
				pPacket.CID = pAgsdRelay2->CID;
				pPacket.NID = pAgsdRelay2->NID;
				pPacket.EID = pAgsdRelay2->EID;
				pPacket.STEP = pAgsdRelay2->STEP;
				pPacket.ItemID = pAgsdRelay2->ItemID;
				pPacket.ullDBIID = pAgsdRelay2->ullDBIID;
				pPacket.nResult = pAgsdRelay2->m_nCode;

				AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);
			} break;
	}

	return TRUE;
}

AgsdRelay2CerariumOrb::AgsdRelay2CerariumOrb(UINT32 ulNID, INT16 Operation)
	: m_nCode(-1)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strAccountName, 0, sizeof(strAccountName));
	memset(strWorldName, 0, sizeof(strWorldName));
	memset(strCharName, 0, sizeof(strCharName));
	memset(szDBID, 0, sizeof(szDBID));
	CID = 0;
	NID = 0;
	EID = 0;
	STEP = 0;
	Slot = -1;
}

AgsdRelay2CerariumOrb::~AgsdRelay2CerariumOrb()
{

}

void AgsdRelay2CerariumOrb::Release()
{ 
	delete this;
}

BOOL AgsdRelay2CerariumOrb::SetParamExecute(AuStatement* pStatement)
{
	switch(nOperation)
	{
		case AGSMRELAY_PARAM_CERARIUMORB_LIST_REQUEST:
			{
				// in
				pStatement->SetParam( 0, strAccountName, sizeof(strAccountName) );
				pStatement->SetParam( 1, strWorldName, sizeof(strWorldName) );
				
				// out
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_CHARACTERNAME_REQUEST:
			{
				// in
				pStatement->SetParam( 0, strAccountName, sizeof(strAccountName) );
				pStatement->SetParam( 1, strWorldName, sizeof(strWorldName) );
				pStatement->SetParam( 2, &Slot );
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING:
			{
				// in
				pStatement->SetParam( 0, szDBID, sizeof(szDBID));
				pStatement->SetParam( 1, strCharName, sizeof(strCharName) );
				
				// out
				pStatement->SetParam( 2, &m_nCode, TRUE);
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING_DATA_REQUEST:
			{
				// in
				pStatement->SetParam( 0, szDBID, sizeof(szDBID));

				// out
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_SEALING_ALL_DATA_REQUEST:
			{
				// in

				// out
			} break;
		case AGSMRELAY_PARAM_CERARIUMORB_RELEASE_SEAL_REQUEST:
			{
				// in
				pStatement->SetParam( 0, szDBID, sizeof(szDBID));
				pStatement->SetParam( 1, strCharName, sizeof(strCharName) );
				
				// out
				pStatement->SetParam( 2, &m_nCode, TRUE);
			} break;
	}

	return TRUE;
}




BOOL AgsmRelay2::OnParamCharacterCreationDate(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	PACKET_RELAY_CHARACTER_CREATION_DATE* pPacket = (PACKET_RELAY_CHARACTER_CREATION_DATE*)pvPacket;

	switch(pPacket->nOperation)
	{
	case AGSMRELAY_PARAM_CHARACTER_CREATION_DATE_CHECK:
		{
			PACKET_RELAY_CHARACTER_CREATION_DATE_CHECK* pPacket2 = (PACKET_RELAY_CHARACTER_CREATION_DATE_CHECK*)pPacket;

			AgsdRelay2CharacterCreationDate* pcsCharacterCreationDate = new AgsdRelay2CharacterCreationDate(ulNID, pPacket->nOperation);
			pcsCharacterCreationDate->CID = pPacket2->CID;
			pcsCharacterCreationDate->NID = pPacket2->NID;
			pcsCharacterCreationDate->EID = pPacket2->EID;
			pcsCharacterCreationDate->STEP = pPacket2->STEP;
			strcpy(pcsCharacterCreationDate->strCharName, pPacket2->strCharName);
			strcpy(pcsCharacterCreationDate->strStartDate, pPacket2->strStartDate);
			strcpy(pcsCharacterCreationDate->strEndDate, pPacket2->strEndDate);

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
			pQuery->m_nIndex = AGSMRELAY_QUERY_EVENT_CREATIONDATE_CHECK;
			pQuery->m_pParam = pcsCharacterCreationDate;
			pQuery->SetCallback(AgsmRelay2::CBOperationCharacterCreationDateCheckSuccess, NULL, this, pcsCharacterCreationDate);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGSMRELAY_PARAM_CHARACTER_CREATION_DATE_CHECK_RESULT:
		{
			PACKET_RELAY_CHARACTER_CREATION_DATE_CHECK_RESULT* pPacket2 = (PACKET_RELAY_CHARACTER_CREATION_DATE_CHECK_RESULT*)pPacket;

			if( m_pagsmEventSystem )
				m_pagsmEventSystem->ProcessCharacterCreationDateCheckResult(pPacket2->CID, pPacket2->NID, pPacket2->EID, pPacket2->STEP, pPacket2->nResult);
		}
		break;
	}
	return TRUE;
}

BOOL AgsmRelay2::CBOperationCharacterCreationDateCheckSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2CharacterCreationDate *pAgsdRelay2 = (AgsdRelay2CharacterCreationDate *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_RELAY_CHARACTER_CREATION_DATE_CHECK_RESULT pPacket;
	pPacket.CID = pAgsdRelay2->CID;
	pPacket.NID = pAgsdRelay2->NID;
	pPacket.EID = pAgsdRelay2->EID;
	pPacket.STEP = pAgsdRelay2->STEP;
	pPacket.nResult = pRowset->GetRowCount();

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;

}

AgsdRelay2CharacterCreationDate::AgsdRelay2CharacterCreationDate(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	CID = 0;
	NID = 0;
	EID = 0;
	STEP = 0;

	memset(strCharName, 0, sizeof(strCharName));
	memset(strStartDate, 0, sizeof(strStartDate));
	memset(strEndDate, 0, sizeof(strEndDate));
}
AgsdRelay2CharacterCreationDate::~AgsdRelay2CharacterCreationDate()
{

}

void AgsdRelay2CharacterCreationDate::Release()
{
	delete this;
}

BOOL AgsdRelay2CharacterCreationDate::SetParamExecute(AuStatement* pStatement)
{
	switch(nOperation)
	{
	case AGSMRELAY_PARAM_CHARACTER_CREATION_DATE_CHECK:
		{
			// in
			pStatement->SetParam( 0, strCharName, sizeof(strCharName) );
			pStatement->SetParam( 1, strStartDate, sizeof(strStartDate) );
			pStatement->SetParam( 2, strEndDate, sizeof(strEndDate) );

			// out
		} 
		break;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 특성화서버 이전  시스템 - JK_특성화서버
BOOL AgsmRelay2::OnParamServerMove(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	PACKET_AGSMCHARACTER_RELAY_SERVERMOVE* pPacket = (PACKET_AGSMCHARACTER_RELAY_SERVERMOVE*)pvPacket;

	switch(pPacket->nOperation)
	{
	case AGSMRELAY_PARAM_SERVERMOVE_REQUEST:
		{
			PACKET_AGSMCHARACTER_RELAY_SERVERMOVE_REQUEST *pPacket2 = (PACKET_AGSMCHARACTER_RELAY_SERVERMOVE_REQUEST *)pPacket;

			AgsdRelay2ServerMove *pcsServerMove = new AgsdRelay2ServerMove(ulNID, pPacket->nOperation);
			pcsServerMove->CID = pPacket2->CID;
			pcsServerMove->NID = pPacket2->NID;
			pcsServerMove->EID = pPacket2->EID;
			pcsServerMove->STEP = pPacket2->STEP;
			strcpy(pcsServerMove->strCharName, pPacket2->strCharName);
			strcpy(pcsServerMove->strServerName, pPacket2->strServerGroupName);


			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
			pQuery->m_nIndex = AGSMRELAY_QUERY_SERVERMOVE;
			pQuery->m_pParam = pcsServerMove;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultServerMove, NULL, this, pcsServerMove);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGSMRELAY_PARAM_SERVERMOVE_RESULT:
		{
			PACKET_AGSMCHARACTER_RELAY_SERVERMOVE_RESULT* pPacket2 = (PACKET_AGSMCHARACTER_RELAY_SERVERMOVE_RESULT*)pPacket;

			if( m_pagsmEventSystem )
				m_pagsmEventSystem->ProcessServerMoveResult(pPacket2->CID, pPacket2->NID, pPacket2->EID, pPacket2->STEP, pPacket2->nResult);
		}
		break;
	}
	return TRUE;
}


BOOL AgsmRelay2::CBOperationResultServerMove(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2ServerMove *pAgsdRelay2 = (AgsdRelay2ServerMove *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSMCHARACTER_RELAY_SERVERMOVE_RESULT pPacket;
	pPacket.CID = pAgsdRelay2->CID;
	pPacket.NID = pAgsdRelay2->NID;
	pPacket.EID = pAgsdRelay2->EID;
	pPacket.STEP = pAgsdRelay2->STEP;
	pPacket.nResult = pAgsdRelay2->m_nCode;

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

AgsdRelay2ServerMove::AgsdRelay2ServerMove(UINT32 ulNID, INT16 Operation)
: m_nCode(-1)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	CID = 0;
	NID = 0;
	EID = 0;
	STEP = 0;

}

AgsdRelay2ServerMove::~AgsdRelay2ServerMove()
{

}

void AgsdRelay2ServerMove::Release()
{ 
	delete this;
}

BOOL AgsdRelay2ServerMove::SetParamExecute(AuStatement* pStatement)
{
	switch(nOperation)
	{
	case AGSMRELAY_PARAM_SERVERMOVE_REQUEST:
		{
			// in
			pStatement->SetParam( 0, strCharName, sizeof(strCharName) );
			pStatement->SetParam( 1, strServerName, sizeof(strServerName) );

			// out
			pStatement->SetParam( 2, &m_nCode, TRUE);
		} break;
	}

	return TRUE;
}