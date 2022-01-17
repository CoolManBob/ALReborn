#include "AgsmRelay2.h"
#include "AgsmTitle.h"

BOOL AgsmRelay2::OnParamTitle(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	if(!pvPacket)
		return FALSE;

	PACKET_AGSP_TITLE_RELAY *pPacket = (PACKET_AGSP_TITLE_RELAY *) pvPacket;

	switch(pPacket->nOperation)
	{
	case AGPMTITLE_PACKET_OPERATION_TITLE_ADD:
		{
			PACKET_AGSP_TITLE_ADD_RELAY *pPacket2 = (PACKET_AGSP_TITLE_ADD_RELAY *)pPacket;

			AgsdRelay2TitleAdd *pcsTitleAdd = new AgsdRelay2TitleAdd(ulNID, pPacket2->nParam);

			strncpy(pcsTitleAdd->strCharName, pPacket2->strCharName, AGPACHARACTER_MAX_ID_STRING);
			pcsTitleAdd->nTitleTid = pPacket2->nTitleID;

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
			pQuery->m_nIndex = AGSMRELAY_QUERY_TITLE_ADD;

			pQuery->m_pParam = pcsTitleAdd;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultAddTitleSuccess, AgsmRelay2::CBOperationResultAddTitleFail, this, pcsTitleAdd);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_USE:
		{
			PACKET_AGSP_TITLE_USE_RELAY *pPacket2 = (PACKET_AGSP_TITLE_USE_RELAY *)pPacket;

			AgsdRelay2TitleUse *pcsTitleUse = new AgsdRelay2TitleUse(ulNID, pPacket2->nParam);

			if(pPacket2->nPauseTitleTid == 0)
			{
				pcsTitleUse->nTitleTid = pPacket2->nTitleID;
				pcsTitleUse->bUse = TRUE;
			}
			else
			{
				pcsTitleUse->nTitleTid = pPacket2->nPauseTitleTid;
				pcsTitleUse->nNextTitleTid = pPacket2->nTitleID;
				pcsTitleUse->bUse = FALSE;
			}

			strncpy(pcsTitleUse->strCharName, pPacket2->strCharName, AGPACHARACTER_MAX_ID_STRING);
			strncpy(pcsTitleUse->strDate, pPacket2->m_szDate, _MAX_DATETIME_LENGTH);

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
			pQuery->m_nIndex = AGSMRELAY_QUERY_TITLE_USE;

			pQuery->m_pParam = pcsTitleUse;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultUseTitleSuccess, AgsmRelay2::CBOperationResultUseTitleFail, this, pcsTitleUse);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_LIST:
		{
			PACKET_AGSP_TITLE_LIST_RELAY *pPacket2 = (PACKET_AGSP_TITLE_LIST_RELAY *)pPacket;

			AgsdRelay2TitleList *pcsTitleList = new AgsdRelay2TitleList(ulNID, pPacket2->nParam);

			strncpy(pcsTitleList->strCharName, pPacket2->strCharName, AGPACHARACTER_MAX_ID_STRING);

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
			pQuery->m_nIndex = AGSMRELAY_QUERY_TITLE_LIST;

			pQuery->m_pParam = pcsTitleList;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultListTitleSuccess, AgsmRelay2::CBOperationResultListTitleFail, this, pcsTitleList);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_ADD_RESUILT:
		{
			PACKET_AGSP_TITLE_ADD_RESULT_RELAY *pPacket2 = (PACKET_AGSP_TITLE_ADD_RESULT_RELAY *)pPacket;

			m_pAgsmTitle->TitleAddResult(pPacket2);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_USE_RESULT:
		{
			PACKET_AGSP_TITLE_USE_RESULT_RELAY *pPacket2 = (PACKET_AGSP_TITLE_USE_RESULT_RELAY *)pPacket;

			m_pAgsmTitle->TitleUseResult(pPacket2);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_LIST_RESULT:
		{
			PACKET_AGSP_TITLE_LIST_RESULT_RELAY *pPacket2 = (PACKET_AGSP_TITLE_LIST_RESULT_RELAY *)pPacket;

			m_pAgsmTitle->TitleListResult(pPacket2);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_QUEST:
		{
			PACKET_AGSP_TITLE_QUEST *pPacket2 = (PACKET_AGSP_TITLE_QUEST *)pPacket;

			OnParamTitleQuest(pPacket2, ulNID);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_DELETE:
		{
			PACKET_AGSP_TITLE_DELETE *pPacket2 = (PACKET_AGSP_TITLE_DELETE *)pPacket;

			AgsdRelay2TitleDelete *pcsTitleDelete = new AgsdRelay2TitleDelete(ulNID, pPacket2->nParam);

			strncpy(pcsTitleDelete->strCharName, pPacket2->strCharName, AGPACHARACTER_MAX_ID_STRING);
			pcsTitleDelete->nTitleTid = pPacket2->nTitleID;

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
			pQuery->m_nIndex = AGSMRELAY_QUERY_TITLE_DELETE;

			pQuery->m_pParam = pcsTitleDelete;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultTitleDeleteSuccess, AgsmRelay2::CBOperationResultTitleDeleteFail, this, pcsTitleDelete);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGPMTITLE_PACKET_OPERATION_TITLE_DELETE_RESULT:
		{
			PACKET_AGSP_TITLE_DELETE_RESULT *pPacket2 = (PACKET_AGSP_TITLE_DELETE_RESULT *)pPacket;

			m_pAgsmTitle->TitleDeleteResult(pPacket2);
		}
		break;
	}
	return TRUE;
}

BOOL AgsmRelay2::OnParamTitleQuest(PACKET_AGSP_TITLE_QUEST* pvPacket, UINT32 ulNID)
{
	switch(pvPacket->nOperation2)
	{
	case AGPMTITLE_QUEST_REQUEST:
		{
			PACKET_AGSP_TITLE_QUEST_REQUEST *pPacket = (PACKET_AGSP_TITLE_QUEST_REQUEST *)pvPacket;

			AgsdRelay2TitleQuestRequest *pcsTitleQuestRequest = new AgsdRelay2TitleQuestRequest(ulNID, pPacket->nParam);

			strncpy(pcsTitleQuestRequest->strCharName, pPacket->strCharName, AGPACHARACTER_MAX_ID_STRING);
			pcsTitleQuestRequest->nTitleTid = pPacket->nTitleID;
			strncpy(pcsTitleQuestRequest->strDate, pPacket->m_szDate,_MAX_DATETIME_LENGTH);

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
			pQuery->m_nIndex = AGSMRELAY_QUERY_TITLE_QUEST_REQUEST;

			pQuery->m_pParam = pcsTitleQuestRequest;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultTitleQuestRequestSuccess, AgsmRelay2::CBOperationResultTitleQuestRequestFail, this, pcsTitleQuestRequest);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGPMTITLE_QUEST_REQUEST_RESULT:
		{
			PACKET_AGSP_TITLE_QUEST_REQUEST_RESULT *pPacket = (PACKET_AGSP_TITLE_QUEST_REQUEST_RESULT *)pvPacket;

			m_pAgsmTitle->TitleQuestRequestResult(pPacket);
		}
		break;
	case AGPMTITLE_QUEST_CHECK:
		{
			PACKET_AGSP_TITLE_QUEST_CHECK *pPacket = (PACKET_AGSP_TITLE_QUEST_CHECK *)pvPacket;

			AgsdRelay2TitleQuestCheck *pcsTitleQuestCheck = new AgsdRelay2TitleQuestCheck(ulNID, pPacket->nParam);

			strncpy(pcsTitleQuestCheck->strCharName, pPacket->strCharName, AGPACHARACTER_MAX_ID_STRING);
			pcsTitleQuestCheck->nTitleTid = pPacket->nTitleID;
			
			for(int i = 0; i < AGPDTITLE_MAX_TITLE_CHECK; i++)
			{
				pcsTitleQuestCheck->nTitleCheckValue[i] = pPacket->nTitleCurrentValue[i];
			}

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
			pQuery->m_nIndex = AGSMRELAY_QUERY_TITLE_QUEST_CHECK;

			pQuery->m_pParam = pcsTitleQuestCheck;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultTitleQuestCheckSuccess, AgsmRelay2::CBOperationResultTitleQuestCheckFail, this, pcsTitleQuestCheck);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGPMTITLE_QUEST_CHECK_RESULT:
		{
			PACKET_AGSP_TITLE_QUEST_CHECK_RESULT *pPacket = (PACKET_AGSP_TITLE_QUEST_CHECK_RESULT *)pvPacket;

			m_pAgsmTitle->TitleQuestCheckResult(pPacket);
		}
		break;
	case AGPMTITLE_QUEST_COMPLETE:
		{
			PACKET_AGSP_TITLE_QUEST_COMPLETE *pPacket = (PACKET_AGSP_TITLE_QUEST_COMPLETE *)pvPacket;

			AgsdRelay2TitleQuestComplete *pcsTitleQuestComplete = new AgsdRelay2TitleQuestComplete(ulNID, pPacket->nParam);

			strncpy(pcsTitleQuestComplete->strCharName, pPacket->strCharName, AGPACHARACTER_MAX_ID_STRING);
			pcsTitleQuestComplete->nTitleTid = pPacket->nTitleID;
			pcsTitleQuestComplete->bComplete = TRUE;
			strncpy(pcsTitleQuestComplete->strDate, pPacket->m_szDate, _MAX_DATETIME_LENGTH);

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
			pQuery->m_nIndex = AGSMRELAY_QUERY_TITLE_QUEST_COMPLETE;

			pQuery->m_pParam = pcsTitleQuestComplete;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultTitleQuestCompleteSuccess, AgsmRelay2::CBOperationResultTitleQuestCompleteFail, this, pcsTitleQuestComplete);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGPMTITLE_QUEST_COMPLETE_RESULT:
		{
			PACKET_AGSP_TITLE_QUEST_COMPLETE_RESULT *pPacket = (PACKET_AGSP_TITLE_QUEST_COMPLETE_RESULT *)pvPacket;

			m_pAgsmTitle->TitleQuestCompleteResult(pPacket);
		}
		break;
	case AGPMTITLE_QUEST_LIST:
		{
			PACKET_AGSP_TITLE_QUEST_LIST *pPacket = (PACKET_AGSP_TITLE_QUEST_LIST *)pvPacket;

			AgsdRelay2TitleQuestList *pcsTitleQuestList = new AgsdRelay2TitleQuestList(ulNID, pPacket->nParam);

			strncpy(pcsTitleQuestList->strCharName, pPacket->strCharName, AGPACHARACTER_MAX_ID_STRING);

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
			pQuery->m_nIndex = AGSMRELAY_QUERY_TITLE_QUEST_LIST;

			pQuery->m_pParam = pcsTitleQuestList;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultTitleQuestListSuccess, AgsmRelay2::CBOperationResultTitleQuestListFail, this, pcsTitleQuestList);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;
	case AGPMTITLE_QUEST_LIST_RESULT:
		{
			PACKET_AGSP_TITLE_QUEST_LIST_RESULT *pPacket = (PACKET_AGSP_TITLE_QUEST_LIST_RESULT *)pvPacket;

			m_pAgsmTitle->TitleQuestListResult(pPacket);
		}
		break;
	}

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultAddTitleSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleAdd *pAgsdRelay2 = (AgsdRelay2TitleAdd *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_ADD_RESULT_RELAY pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, TRUE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultAddTitleFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleAdd *pAgsdRelay2 = (AgsdRelay2TitleAdd *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_ADD_RESULT_RELAY pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, FALSE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultUseTitleSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleUse *pAgsdRelay2 = (AgsdRelay2TitleUse *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_USE_RESULT_RELAY pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, pAgsdRelay2->strDate, pAgsdRelay2->bUse, TRUE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	if(pAgsdRelay2->nNextTitleTid != 0)
	{
		AgsdRelay2TitleUse *pcsTitleUse = new AgsdRelay2TitleUse(pAgsdRelay2->m_ulNID, pAgsdRelay2->nOperation);

		pcsTitleUse->nTitleTid = pAgsdRelay2->nNextTitleTid;
		pcsTitleUse->nNextTitleTid = 0;
		pcsTitleUse->bUse = TRUE;

		strncpy(pcsTitleUse->strCharName, pAgsdRelay2->strCharName, AGPACHARACTER_MAX_ID_STRING);
		strncpy(pcsTitleUse->strDate, pAgsdRelay2->strDate, _MAX_DATETIME_LENGTH);

		AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
		pQuery->m_nIndex = AGSMRELAY_QUERY_TITLE_USE;

		pQuery->m_pParam = pcsTitleUse;
		pQuery->SetCallback(AgsmRelay2::CBOperationResultUseTitleSuccess, AgsmRelay2::CBOperationResultUseTitleFail, pThis, pcsTitleUse);

		pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultUseTitleFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleUse *pAgsdRelay2 = (AgsdRelay2TitleUse *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_USE_RESULT_RELAY pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, pAgsdRelay2->strDate, pAgsdRelay2->bUse, FALSE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultListTitleSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleList *pAgsdRelay2 = (AgsdRelay2TitleList *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	INT32 lRowCount = pRowset->GetRowCount();

	for(int i = 0; i < lRowCount; i++)
	{
		CHAR tempCharName[AGPACHARACTER_MAX_ID_STRING+1];
		strncpy(tempCharName, (CHAR*)pRowset->Get(i, 1) ,AGPACHARACTER_MAX_ID_STRING);

		if(strcmp(tempCharName, pAgsdRelay2->strCharName))
			continue;

		PACKET_AGSP_TITLE_LIST_RESULT_RELAY pPacket(pAgsdRelay2->strCharName, TRUE, FALSE);

		pPacket.nTitleID = atoi((CHAR*)pRowset->Get(i, 0));
		pPacket.bUseTitle = atoi((CHAR*)pRowset->Get(i, 3));

		strncpy(pPacket.m_szDate, (CHAR*)pRowset->Get(i, 2), _MAX_DATETIME_LENGTH);

		AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);
	}


	PACKET_AGSP_TITLE_LIST_RESULT_RELAY pPacket(pAgsdRelay2->strCharName, TRUE, TRUE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultListTitleFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleList *pAgsdRelay2 = (AgsdRelay2TitleList *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_LIST_RESULT_RELAY pPacket(pAgsdRelay2->strCharName, FALSE, TRUE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultTitleQuestRequestSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleQuestRequest *pAgsdRelay2 = (AgsdRelay2TitleQuestRequest *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_QUEST_REQUEST_RESULT pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, TRUE);

	strncpy(pPacket.m_szDate, pAgsdRelay2->strDate, _MAX_DATETIME_LENGTH);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}
BOOL AgsmRelay2::CBOperationResultTitleQuestRequestFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleQuestRequest *pAgsdRelay2 = (AgsdRelay2TitleQuestRequest *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_QUEST_REQUEST_RESULT pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, TRUE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultTitleQuestCheckSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleQuestCheck *pAgsdRelay2 = (AgsdRelay2TitleQuestCheck *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_QUEST_CHECK_RESULT pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, TRUE);

	for(int i = 0; i < AGPDTITLE_MAX_TITLE_CHECK; i++)
	{
		pPacket.nTitleCurrentValue[i] = pAgsdRelay2->nTitleCheckValue[i];
	}

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultTitleQuestCheckFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleQuestCheck *pAgsdRelay2 = (AgsdRelay2TitleQuestCheck *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_QUEST_CHECK_RESULT pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, FALSE);

	for(int i = 0; i < AGPDTITLE_MAX_TITLE_CHECK; i++)
	{
		pPacket.nTitleCurrentValue[i] = pAgsdRelay2->nTitleCheckValue[i];
	}

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultTitleQuestCompleteSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleQuestComplete *pAgsdRelay2 = (AgsdRelay2TitleQuestComplete *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_QUEST_COMPLETE_RESULT pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, TRUE);

	strncpy(pPacket.m_szDate, pAgsdRelay2->strDate, _MAX_DATETIME_LENGTH);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultTitleQuestCompleteFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleQuestComplete *pAgsdRelay2 = (AgsdRelay2TitleQuestComplete *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;
	
	PACKET_AGSP_TITLE_QUEST_COMPLETE_RESULT pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, FALSE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultTitleQuestListSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleQuestList *pAgsdRelay2 = (AgsdRelay2TitleQuestList *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	INT32 lRowCount = pRowset->GetRowCount();

	for(int i = 0; i < lRowCount; i++)
	{
		CHAR tempCharName[AGPACHARACTER_MAX_ID_STRING+1];
		strncpy(tempCharName, (CHAR*)pRowset->Get(i, 1) ,AGPACHARACTER_MAX_ID_STRING);

		if(strcmp(tempCharName, pAgsdRelay2->strCharName))
			continue;

		PACKET_AGSP_TITLE_QUEST_LIST_RESULT pPacket(pAgsdRelay2->strCharName, TRUE, FALSE);

		pPacket.nTitleID = atoi((CHAR*)pRowset->Get(i, 0));
		pPacket.bTitleQuestComplete = atoi((CHAR*)pRowset->Get(i, 2));

		for(int j = 0; j < AGPDTITLE_MAX_TITLE_CHECK; j++)
		{
			pPacket.nTitleQuestCheckValue[j] = atoi((CHAR*)pRowset->Get(i, j+3));
		}

		strncpy(pPacket.m_szDateAccept, (CHAR*)pRowset->Get(i, 8), _MAX_DATETIME_LENGTH);
		strncpy(pPacket.m_szDateComplete, (CHAR*)pRowset->Get(i, 9), _MAX_DATETIME_LENGTH);

		AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);
	}


	PACKET_AGSP_TITLE_QUEST_LIST_RESULT pPacket(pAgsdRelay2->strCharName, TRUE, TRUE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultTitleQuestListFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleQuestList *pAgsdRelay2 = (AgsdRelay2TitleQuestList *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSP_TITLE_QUEST_LIST_RESULT pPacket(pAgsdRelay2->strCharName, FALSE, TRUE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultTitleDeleteSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleDelete *pAgsdRelay2 = (AgsdRelay2TitleDelete *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;
	
	PACKET_AGSP_TITLE_DELETE_RESULT pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, TRUE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultTitleDeleteFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2TitleDelete *pAgsdRelay2 = (AgsdRelay2TitleDelete *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;
	
	PACKET_AGSP_TITLE_DELETE_RESULT pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->nTitleTid, FALSE);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Be Related To Query                                               //
////////////////////////////////////////////////////////////////////////////////////////////////////////

AgsdRelay2TitleAdd::AgsdRelay2TitleAdd(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	nTitleTid = 0;
	bUse = FALSE;
	bCompleteQuest = TRUE;
	memset(strCharName, 0, sizeof(strCharName));
	memset(strDate, 0, sizeof(strDate));
}

AgsdRelay2TitleAdd::~AgsdRelay2TitleAdd()
{
	
}

void AgsdRelay2TitleAdd::Release()
{ 
	delete this;
}

BOOL AgsdRelay2TitleAdd::SetParamExecute(AuStatement* pStatement)
{
	pStatement->SetParam( 0, &bCompleteQuest);
	pStatement->SetParam( 1, &bUse);
	pStatement->SetParam( 2, strDate, sizeof(strDate) );
	pStatement->SetParam( 3, strCharName, sizeof(strCharName) );
	pStatement->SetParam( 4, &nTitleTid);

	return TRUE;
}



AgsdRelay2TitleUse::AgsdRelay2TitleUse(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	nTitleTid = 0;
	nNextTitleTid = 0;
	bUse = FALSE;
	memset(strCharName, 0, sizeof(strCharName));
	memset(strDate, 0, sizeof(strDate));
}

AgsdRelay2TitleUse::~AgsdRelay2TitleUse()
{
	
}

void AgsdRelay2TitleUse::Release()
{ 
	delete this;
}

BOOL AgsdRelay2TitleUse::SetParamExecute(AuStatement* pStatement)
{
	pStatement->SetParam( 0, strDate, sizeof(strDate) );
	pStatement->SetParam( 1, &bUse);
	pStatement->SetParam( 2, &nTitleTid);
	pStatement->SetParam( 3, strCharName, sizeof(strCharName) );

	return TRUE;
}



AgsdRelay2TitleList::AgsdRelay2TitleList(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strCharName, 0, sizeof(strCharName));
	bIsTitle = TRUE;
}

AgsdRelay2TitleList::~AgsdRelay2TitleList()
{
	
}

void AgsdRelay2TitleList::Release()
{ 
	delete this;
}

BOOL AgsdRelay2TitleList::SetParamExecute(AuStatement* pStatement)
{
	pStatement->SetParam( 0, strCharName, sizeof(strCharName) );
	pStatement->SetParam( 1, &bIsTitle);

	return TRUE;
}


AgsdRelay2TitleQuestRequest::AgsdRelay2TitleQuestRequest(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strCharName, 0, sizeof(strCharName));
	memset(strDate, 0, sizeof(strDate));
}

AgsdRelay2TitleQuestRequest::~AgsdRelay2TitleQuestRequest()
{
	
}

void AgsdRelay2TitleQuestRequest::Release()
{ 
	delete this;
}

BOOL AgsdRelay2TitleQuestRequest::SetParamExecute(AuStatement* pStatement)
{
	pStatement->SetParam( 0, strCharName, sizeof(strCharName) );
	pStatement->SetParam( 1, &nTitleTid );
	pStatement->SetParam( 2, strDate, sizeof(strDate) );

	return TRUE;
}


AgsdRelay2TitleQuestCheck::AgsdRelay2TitleQuestCheck(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strCharName, 0, sizeof(strCharName));
}

AgsdRelay2TitleQuestCheck::~AgsdRelay2TitleQuestCheck()
{
	
}

void AgsdRelay2TitleQuestCheck::Release()
{ 
	delete this;
}

BOOL AgsdRelay2TitleQuestCheck::SetParamExecute(AuStatement* pStatement)
{
	pStatement->SetParam( 0, &nTitleCheckValue[0] );
	pStatement->SetParam( 1, &nTitleCheckValue[1] );
	pStatement->SetParam( 2, &nTitleCheckValue[2] );
	pStatement->SetParam( 3, &nTitleCheckValue[3] );
	pStatement->SetParam( 4, &nTitleCheckValue[4] );
	pStatement->SetParam( 5, &nTitleTid );
	pStatement->SetParam( 6, strCharName, sizeof(strCharName) );

	return TRUE;
}


AgsdRelay2TitleQuestComplete::AgsdRelay2TitleQuestComplete(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strCharName, 0, sizeof(strCharName));
	memset(strDate, 0, sizeof(strDate));
}

AgsdRelay2TitleQuestComplete::~AgsdRelay2TitleQuestComplete()
{
	
}

void AgsdRelay2TitleQuestComplete::Release()
{ 
	delete this;
}

BOOL AgsdRelay2TitleQuestComplete::SetParamExecute(AuStatement* pStatement)
{
	pStatement->SetParam( 0, &bComplete );
	pStatement->SetParam( 1, strDate, sizeof(strDate) );
	pStatement->SetParam( 2, &nTitleTid );
	pStatement->SetParam( 3, strCharName, sizeof(strCharName) );

	return TRUE;
}


AgsdRelay2TitleQuestList::AgsdRelay2TitleQuestList(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strCharName, 0, sizeof(strCharName));
}

AgsdRelay2TitleQuestList::~AgsdRelay2TitleQuestList()
{
	
}

void AgsdRelay2TitleQuestList::Release()
{ 
	delete this;
}

BOOL AgsdRelay2TitleQuestList::SetParamExecute(AuStatement* pStatement)
{
	pStatement->SetParam( 0, strCharName, sizeof(strCharName) );

	return TRUE;
}



AgsdRelay2TitleDelete::AgsdRelay2TitleDelete(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strCharName, 0, sizeof(strCharName));
	nTitleTid = 0;
}

AgsdRelay2TitleDelete::~AgsdRelay2TitleDelete()
{
	
}

void AgsdRelay2TitleDelete::Release()
{ 
	delete this;
}

BOOL AgsdRelay2TitleDelete::SetParamExecute(AuStatement* pStatement)
{
	pStatement->SetParam( 0, strCharName, sizeof(strCharName));
	pStatement->SetParam( 1, &nTitleTid);

	return TRUE;
}