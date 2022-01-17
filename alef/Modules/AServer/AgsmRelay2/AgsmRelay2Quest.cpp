/*====================================================================

	AgsmRelay2Quest.cpp

====================================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"


//
//	==========		Quest		==========
//
void AgsmRelay2::InitPacketQuest()
	{
	m_csPacketQuest.SetFlagLength(sizeof(INT8));
	m_csPacketQuest.SetFieldType(AUTYPE_INT16,		1,								// eAgsmRelay2Operation
								 AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,		// Character
								 AUTYPE_INT32,		1,								// Quest TID
								 AUTYPE_INT32,		1,								// argument1
								 AUTYPE_INT32,		1,								// argument2
								 AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,			// NPC
								 AUTYPE_END,		0
								 );
	
	m_csPacketQuestFlag.SetFlagLength(sizeof(INT8));
	m_csPacketQuestFlag.SetFieldType(AUTYPE_INT16,		1,							// eAgsmRelay2Operation
								 AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,		// Character
								 AUTYPE_CHAR,		AGSDQUEST_MAX_FLAG + 1,			// Quest Block 1
								 AUTYPE_CHAR,		AGSDQUEST_MAX_FLAG + 1,			// Quest Block 2
								 AUTYPE_CHAR,		AGSDQUEST_MAX_FLAG + 1,			// Quest Block 3
								 AUTYPE_END,		0
								 );
	}


BOOL AgsmRelay2::OnParamQuest(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Quest* pcsQuest = new AgsdRelay2Quest;

	CHAR *pszCharID = NULL;
	CHAR *pszNPC	= NULL;

	m_csPacketQuest.GetField(FALSE, pvPacket, 0,
							 &pcsQuest->m_eOperation,
							 &pszCharID,
							 &pcsQuest->m_lQuestTID,
							 &pcsQuest->m_lArg1,
							 &pcsQuest->m_lArg2,
							 &pszNPC
							 );

	if (!pszCharID || _T('\0') == *pszCharID)
		{
		pcsQuest->Release();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!!Error : CharID is NULL in OnParamQuest()\n");
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);

		return FALSE;
		}

	_tcsncpy(pcsQuest->m_szCharID, pszCharID, _MAX_CHARNAME_LENGTH);
	_tcsncpy(pcsQuest->m_szNPC, pszNPC ? pszNPC : _T(""), _MAX_CHARNAME_LENGTH);

	return EnumCallback(AGSMRELAY_PARAM_QUEST, (PVOID)pcsQuest, (PVOID)nParam);
	}


BOOL AgsmRelay2::OnParamQuestFlag(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2QuestFlag* pcsQuestFlag = new AgsdRelay2QuestFlag;

	CHAR *pszCharID = NULL;
	CHAR *pszQuestBlock1 = NULL;
	CHAR *pszQuestBlock2 = NULL;
	CHAR *pszQuestBlock3 = NULL;

	m_csPacketQuestFlag.GetField(FALSE, pvPacket, 0,
							 &pcsQuestFlag->m_eOperation,
							 &pszCharID,
							 &pszQuestBlock1,
							 &pszQuestBlock2,
							 &pszQuestBlock3
							 );
	_tcsncpy(pcsQuestFlag->m_szCharID, pszCharID ? pszCharID : _T(""), _MAX_CHARNAME_LENGTH);
	_tcsncpy(pcsQuestFlag->m_szQuestBlock1, pszQuestBlock1 ? pszQuestBlock1 : _T(""), AGSDQUEST_MAX_FLAG);
	_tcsncpy(pcsQuestFlag->m_szQuestBlock2, pszQuestBlock2 ? pszQuestBlock2 : _T(""), AGSDQUEST_MAX_FLAG);
	_tcsncpy(pcsQuestFlag->m_szQuestBlock3, pszQuestBlock3 ? pszQuestBlock3 : _T(""), AGSDQUEST_MAX_FLAG);
	
  /*#ifdef _DEBUG
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "RECV [%s][%s][%s][%s]\n", pcsQuestFlag->m_szCharID, pcsQuestFlag->m_szQuestBlock1, pcsQuestFlag->m_szQuestBlock2, pcsQuestFlag->m_szQuestBlock3);
	AuLogFile_s("LOG\\QUEST_FLAG.log", strCharBuff);
  #endif*/

	return EnumCallback(AGSMRELAY_PARAM_QUEST_FLAG, (PVOID)pcsQuestFlag, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBCurrentQuest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(pData && pClass && "Invalid Parameter for Update Questin AgsmRelay2::CBQuestUpdate()");

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdCurrentQuest *pQuest = (AgpdCurrentQuest *)pData;
	INT16 nOperation = (INT16) ((PVOID *)pCustData)[0];
	CHAR *pszCharID	= (CHAR *)((PVOID *)pCustData)[1];

	if (NULL == pszCharID)
		return FALSE;
		
	AgsdRelay2Quest csQuest;

	_tcsncpy(csQuest.m_szCharID, pszCharID, _MAX_CHARNAME_LENGTH);
	csQuest.m_lQuestTID = pQuest->lQuestID;
	csQuest.m_lArg1 = pQuest->lParam1;
	csQuest.m_lArg2 = pQuest->lParam2;
//	strcpy(csQuest.m_szNPC, pQuest->szNPCName);
	strcpy(csQuest.m_szNPC, "");

	switch (nOperation)
		{
		case AGSMQUEST_DB_CURRENTQUEST_UPDATE :
			csQuest.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;
			break;

		case AGSMQUEST_DB_CURRENTQUEST_INSERT :
			csQuest.m_eOperation = AGSMDATABASE_OPERATION_INSERT;
			break;

		case AGSMQUEST_DB_CURRENTQUEST_DELETE :
			csQuest.m_eOperation = AGSMDATABASE_OPERATION_DELETE;
			break;

		default :
			return FALSE;
			break;
		}

	return pThis->SendQuest(&csQuest);
	}


BOOL AgsmRelay2::CBQuestFlag(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(pData && pClass && "Invalid Parameter for Update Questin AgsmRelay2::CBQuestUpdate()");

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdQuest *pQuest = (AgpdQuest *)pData;
	INT16 nOperation = (INT16) ((PVOID *)pCustData)[0];
	CHAR *pszCharID	= (CHAR *)((PVOID *)pCustData)[1];
	
	if (NULL == pszCharID)
		return FALSE;

	AgsdRelay2QuestFlag csQuestFlag;

	csQuestFlag.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;
	_tcsncpy(csQuestFlag.m_szCharID, pszCharID, _MAX_CHARNAME_LENGTH);
	
	CHAR szQuestBlock1[AGSDQUEST_MAX_FLAG + 1];
	memcpy(szQuestBlock1, pQuest->m_csFlag.m_btQuestBlock1, AGSDQUEST_MAX_FLAG);
	_tcsncpy(csQuestFlag.m_szQuestBlock1, szQuestBlock1, AGSDQUEST_MAX_FLAG);
	csQuestFlag.m_szQuestBlock1[AGSDQUEST_MAX_FLAG] = '\0';

	CHAR szQuestBlock2[AGSDQUEST_MAX_FLAG + 1];
	memcpy(szQuestBlock2, pQuest->m_csFlag.m_btQuestBlock2, AGSDQUEST_MAX_FLAG);
	_tcsncpy(csQuestFlag.m_szQuestBlock2, szQuestBlock2, AGSDQUEST_MAX_FLAG);
	csQuestFlag.m_szQuestBlock2[AGSDQUEST_MAX_FLAG] = '\0';

	CHAR szQuestBlock3[AGSDQUEST_MAX_FLAG + 1];
	memcpy(szQuestBlock3, pQuest->m_csFlag.m_btQuestBlock3, AGSDQUEST_MAX_FLAG);
	_tcsncpy(csQuestFlag.m_szQuestBlock3, szQuestBlock3, AGSDQUEST_MAX_FLAG);
	csQuestFlag.m_szQuestBlock3[AGSDQUEST_MAX_FLAG] = '\0';

	return pThis->SendQuestFlag(&csQuestFlag);
	}


BOOL AgsmRelay2::SendQuest(AgsdRelay2Quest *pQuest)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketQuest = m_csPacketQuest.MakePacket(FALSE, &nPacketLength, 0,
													 &pQuest->m_eOperation,
													 pQuest->m_szCharID,
													 &pQuest->m_lQuestTID,
													 &pQuest->m_lArg1,
													 &pQuest->m_lArg2,
													 pQuest->m_szNPC
													 );
	if (!pvPacketQuest)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketQuest, AGSMRELAY_PARAM_QUEST);
	m_csPacketQuest.FreePacket(pvPacketQuest);
	
	return bResult;
	}


BOOL AgsmRelay2::SendQuestFlag(AgsdRelay2QuestFlag *pQuestFlag)
	{
	INT16 nPacketLength	= 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
// AGSDQUEST_EXPAND_BLOCK
	PVOID pvPacketQuestFlag = m_csPacketQuestFlag.MakePacket(FALSE, &nPacketLength, 0,
															 &pQuestFlag->m_eOperation,
															 pQuestFlag->m_szCharID,
															 pQuestFlag->m_szQuestBlock1,
															 pQuestFlag->m_szQuestBlock2,
															 pQuestFlag->m_szQuestBlock3
															 );
	if (!pvPacketQuestFlag)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketQuestFlag, AGSMRELAY_PARAM_QUEST_FLAG);
	m_csPacketQuest.FreePacket(pvPacketQuestFlag);
	
	return bResult;
	}




/********************************************************/
/*		The Implementation of AgsdRelay2Quest class		*/
/********************************************************/
//
AgsdRelay2Quest::AgsdRelay2Quest()
	{
	ZeroMemory(m_szCharID, sizeof(m_szCharID));
	m_lQuestTID = 0;
	m_lArg1 = 0;
	m_lArg2 = 0;
	ZeroMemory(m_szNPC, sizeof(m_szNPC));
	}


BOOL AgsdRelay2Quest::SetParamUpdate(AuStatement* pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, &m_lArg1);
	pStatement->SetParam(i++, &m_lArg2);
	pStatement->SetParam(i++, m_szNPC, sizeof(m_szNPC));
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	pStatement->SetParam(i++, &m_lQuestTID);

	return TRUE;
	}


BOOL AgsdRelay2Quest::SetParamInsert(AuStatement* pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	pStatement->SetParam(i++, &m_lQuestTID);
	pStatement->SetParam(i++, &m_lArg1);
	pStatement->SetParam(i++, &m_lArg2);
	pStatement->SetParam(i++, m_szNPC, sizeof(m_szNPC));

	return TRUE;
	}


BOOL AgsdRelay2Quest::SetParamDelete(AuStatement* pStatement)
	{
	pStatement->SetParam(0, m_szCharID, sizeof(m_szCharID));
	pStatement->SetParam(1, &m_lQuestTID);

	return TRUE;
	}


void AgsdRelay2Quest::Dump(CHAR *pszOp)
	{
	/*CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "LOG\\%04d%02d%02d-%02d-RELAY_DUMP_QUEST.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d:%02d:%02d][%s]"
						"ID[%s] QTID[%d] Arg1[%d] Arg2[%d]\n",
				st.wHour, st.wMinute, st.wSecond,
				pszOp, m_szCharID, m_lQuestTID, m_lArg1, m_lArg2);
	AuLogFile_s(szFile, strCharBuff);*/
	}


AgsdRelay2QuestFlag::AgsdRelay2QuestFlag()
	{
	ZeroMemory(m_szCharID, sizeof(m_szCharID));
	ZeroMemory(m_szQuestBlock1, sizeof(m_szQuestBlock1));
	ZeroMemory(m_szQuestBlock2, sizeof(m_szQuestBlock2));
	ZeroMemory(m_szQuestBlock3, sizeof(m_szQuestBlock3));
	}


BOOL AgsdRelay2QuestFlag::SetParamUpdate(AuStatement* pStatement)
	{
	pStatement->SetParam(0, m_szQuestBlock1, sizeof(m_szQuestBlock1));
	pStatement->SetParam(1, m_szQuestBlock2, sizeof(m_szQuestBlock2));
	pStatement->SetParam(2, m_szQuestBlock3, sizeof(m_szQuestBlock3));
	pStatement->SetParam(3, m_szCharID, sizeof(m_szCharID));
	return TRUE;
	}
