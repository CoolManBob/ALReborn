/*====================================================================

	AgsmRelay2Buddy.cpp

====================================================================*/


#include "AgsmRelay2.h"
#include "AgpdBuddy.h"
#include "AgsdBuddy.h"
#include "ApMemoryTracker.h"


//
//	==========		Buddy		==========
//
void AgsmRelay2::InitPacketBuddy()
	{
	m_csPacketBuddy.SetFlagLength(sizeof(INT8));
	m_csPacketBuddy.SetFieldType(AUTYPE_INT16,		1,								// eAgsmRelay2Operation
								 AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,		// character name
								 AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,		// buddy name
								 AUTYPE_UINT32,		1,								// options
								 AUTYPE_INT32,		1,								// MentorStatus
								 AUTYPE_PACKET,		1,								// select result packet
								 AUTYPE_END,		0
								 );
	}


BOOL AgsmRelay2::OnParamBuddy(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Buddy* pcsBuddy = new AgsdRelay2Buddy;

	CHAR *pszCharID	= NULL;
	CHAR *pszBuddyID	= NULL;

	m_csPacketBuddy.GetField(FALSE, pvPacket, 0,
							 &pcsBuddy->m_eOperation,
							 &pszCharID,
							 &pszBuddyID,
							 &pcsBuddy->m_dwOptions,
							 &pcsBuddy->m_nMentorStatus,
							 &pcsBuddy->pvRowsetPacket
							 );

	if (false == pcsBuddy->m_szCharID.SetText(pszCharID))
		{
		pcsBuddy->Release();
		return FALSE;
		}

	if (false == pcsBuddy->m_szBuddyID.SetText(pszBuddyID))
		{
		pcsBuddy->Release();
		return FALSE;
		}

	pcsBuddy->m_ulNID = ulNID;

	return EnumCallback(AGSMRELAY_PARAM_BUDDY, (PVOID)pcsBuddy, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBOperationResultBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pData)
		return FALSE;

	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgsdRelay2Buddy* pAgsdBuddy = static_cast<AgsdRelay2Buddy*>(pData);

	AgsdRelay2AutoPtr Auto(pAgsdBuddy);

	if (AGSMDATABASE_OPERATION_SELECT != pAgsdBuddy->m_eOperation)
		return FALSE;

	if (NULL == pAgsdBuddy->pvRowsetPacket)
		return FALSE;

	stBuddyRowset Rowset;
	if (FALSE == pThis->ParseBuddyRowsetPacket(pAgsdBuddy->pvRowsetPacket, &Rowset))
		return FALSE;

	AgpdCharacter *pcsCharacter = pThis->m_pAgpmCharacter->GetCharacterLock(pAgsdBuddy->m_szCharID);
	if (NULL == pcsCharacter)
		return FALSE;

	if (FALSE == pThis->m_pAgsmBuddy->ReceiveBuddyDataFromRelay(pcsCharacter, &Rowset))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}


BOOL AgsmRelay2::ParseBuddyRowsetPacket(PVOID pvPacket, stBuddyRowset* pstRowset)
	{
	if (!pvPacket || !pstRowset)
		return FALSE;

	CHAR*	pszQuery		= NULL;
	INT16	nQueryLength	= 0;
	PVOID	pHeaders		= NULL;
	INT16	nHeadersLength	= 0;
	INT16	nTotalBufferSize	= 0;
	PVOID	pOffsets		= NULL;
	INT16	nOffsetsLength	=0;

	m_csPacketRowset.GetField(FALSE, pvPacket, 0,
							  //&pstRowset->m_eOperation,		// op.
							  &pstRowset->m_lQueryIndex,		// query index
							  &pszQuery,						// query text
							  &nQueryLength,					// query text length
							  &pHeaders,						// headers text
							  &nHeadersLength,					//  headers text length
							  &pstRowset->m_ulRows,				// no. of rows
							  &pstRowset->m_ulCols,				// no. of cols
							  &pstRowset->m_ulRowBufferSize,	// 1 row's buffer size
							  &pstRowset->m_pBuffer,			// buffer
							  &nTotalBufferSize,				// total buffer size
							  &pOffsets,
							  &nOffsetsLength
							  );

	memset(pstRowset->m_szQuery, 0, MAX_QUERY_LENGTH);
	strncpy(pstRowset->m_szQuery, pszQuery ? pszQuery : _T(""), MAX_QUERY_LENGTH - 1);
	ASSERT(nHeadersLength < MAX_HEADER_STRING_LENGTH);
	memcpy(pstRowset->m_szHeaders, pHeaders, nHeadersLength);
	pstRowset->m_ulTotalBufferSize = nTotalBufferSize;
	pstRowset->m_lOffsets.MemSetAll();
	pstRowset->m_lOffsets.MemCopy(0, (INT32 *) pOffsets, nOffsetsLength / sizeof(INT32));

	return TRUE;
	}

BOOL AgsmRelay2::CBBuddyInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(NULL != pData);
	ASSERT(NULL != pClass);
	ASSERT(NULL != pCustData);

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pData;
	AgpdBuddyElement *pcsBuddy = (AgpdBuddyElement *) pCustData;
		
	AgsdRelay2Buddy csBuddy;

	csBuddy.m_szCharID.SetText(pcsCharacter->m_szID);
	csBuddy.m_szBuddyID.SetText(pcsBuddy->m_szName);
	csBuddy.m_dwOptions = pcsBuddy->GetOptions();
	csBuddy.m_nMentorStatus = pcsBuddy->GetMentorStatus();
	csBuddy.m_eOperation = AGSMDATABASE_OPERATION_INSERT;

	return pThis->SendBuddy(&csBuddy, NULL);
	}

BOOL AgsmRelay2::CBBuddyRemove(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(NULL != pData);
	ASSERT(NULL != pClass);
	ASSERT(NULL != pCustData);

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pData;
	AgpdBuddyElement *pcsBuddy = (AgpdBuddyElement *) pCustData;
		
	AgsdRelay2Buddy csBuddy;

	csBuddy.m_szCharID.SetText(pcsCharacter->m_szID);
	csBuddy.m_szBuddyID.SetText(pcsBuddy->m_szName);
	csBuddy.m_dwOptions = pcsBuddy->GetOptions();
	csBuddy.m_nMentorStatus = pcsBuddy->GetMentorStatus();
	csBuddy.m_eOperation = AGSMDATABASE_OPERATION_DELETE;

	return pThis->SendBuddy(&csBuddy, NULL);
	}

// 2007.07.25. steeple
BOOL AgsmRelay2::CBBuddyRemove2(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(NULL != pData);
	ASSERT(NULL != pClass);
	ASSERT(NULL != pCustData);

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	CHAR* pszName = (CHAR*) pData;
	AgpdBuddyElement *pcsBuddy = (AgpdBuddyElement *) pCustData;
		
	AgsdRelay2Buddy csBuddy;

	csBuddy.m_szCharID.SetText(pszName);
	csBuddy.m_szBuddyID.SetText(pcsBuddy->m_szName);
	csBuddy.m_dwOptions = pcsBuddy->GetOptions();
	csBuddy.m_nMentorStatus = pcsBuddy->GetMentorStatus();
	csBuddy.m_eOperation = AGSMDATABASE_OPERATION_DELETE;

	return pThis->SendBuddy(&csBuddy, NULL);
	}

BOOL AgsmRelay2::CBBuddyOptions(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(NULL != pData);
	ASSERT(NULL != pClass);
	ASSERT(NULL != pCustData);

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pData;
	AgpdBuddyElement *pcsBuddy = (AgpdBuddyElement *) pCustData;
		
	AgsdRelay2Buddy csBuddy;

	csBuddy.m_szCharID.SetText(pcsCharacter->m_szID);
	csBuddy.m_szBuddyID.SetText(pcsBuddy->m_szName);
	csBuddy.m_dwOptions = pcsBuddy->GetOptions();
	csBuddy.m_nMentorStatus = pcsBuddy->GetMentorStatus();
	csBuddy.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;

	return pThis->SendBuddy(&csBuddy, NULL);
	}

BOOL AgsmRelay2::CBBuddyOptions2(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(NULL != pData);
	ASSERT(NULL != pClass);
	ASSERT(NULL != pCustData);

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	CHAR* pszName = (CHAR*) pData;
	AgpdBuddyElement *pcsBuddy = (AgpdBuddyElement *) pCustData;

	AgsdRelay2Buddy csBuddy;

	csBuddy.m_szCharID.SetText(pszName);
	csBuddy.m_szBuddyID.SetText(pcsBuddy->m_szName);
	csBuddy.m_dwOptions = pcsBuddy->GetOptions();
	csBuddy.m_nMentorStatus = pcsBuddy->GetMentorStatus();
	csBuddy.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;

	return pThis->SendBuddy(&csBuddy, NULL);
	}

BOOL AgsmRelay2::CBBuddySelect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(NULL != pData);
	ASSERT(NULL != pClass);

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pData;
		
	AgsdRelay2Buddy csBuddy;

	csBuddy.m_szCharID.SetText(pcsCharacter->m_szID);
	csBuddy.m_eOperation = AGSMDATABASE_OPERATION_SELECT;

	return pThis->SendBuddy(&csBuddy, NULL);
}


BOOL AgsmRelay2::SendBuddy(AgsdRelay2Buddy *pBuddy, PVOID pvRowsetPacket, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketBuddy = m_csPacketBuddy.MakePacket(FALSE, &nPacketLength, 0,
													 &pBuddy->m_eOperation,
													 pBuddy->m_szCharID.GetBuffer(),
													 pBuddy->m_szBuddyID.GetBuffer(),
													 &pBuddy->m_dwOptions,
													 &pBuddy->m_nMentorStatus,
													 pvRowsetPacket
													 );
	if (!pvPacketBuddy)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketBuddy, AGSMRELAY_PARAM_BUDDY);
	else
		bResult = MakeAndSendRelayPacket(pvPacketBuddy, AGSMRELAY_PARAM_BUDDY, ulNID);

	m_csPacketBuddy.FreePacket(pvPacketBuddy);
	
	return bResult;
	}


BOOL AgsmRelay2::CBOperationBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	//###############
	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationBuddy,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBFinishOperationBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	if (NULL != pRowset && AGSMDATABASE_OPERATION_SELECT == pAgsdRelay2->m_eOperation)
		pThis->OnSelectResultBuddy(pRowset, pAgsdRelay2);

	pAgsdRelay2->Release();

	return TRUE;
	}



BOOL AgsmRelay2::OnSelectResultBuddy(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	PVOID pvPacket = MakeRowsetPacket(pRowset);
	
	AgsdRelay2Buddy* pcsBuddy = static_cast<AgsdRelay2Buddy*>(pAgsdRelay2);

	AgsdServer* pcsGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pcsBuddy->m_ulNID);
	if (!pcsGameServer)
		return FALSE;

	return SendBuddy(pcsBuddy, pvPacket, pcsGameServer->m_dpnidServer);
	}


/********************************************************/
/*		The Implementation of AgsdRelay2Buddy class		*/
/********************************************************/
//
AgsdRelay2Buddy::AgsdRelay2Buddy()
	{
		m_dwOptions = 0;
		m_nMentorStatus = AGSMBUDDY_MENTOR_NONE;
		pvRowsetPacket = NULL;
	}


BOOL AgsdRelay2Buddy::SetParamSelect(AuStatement* pStatement)
	{
	pStatement->SetParam(0, m_szCharID, m_szCharID.GetByteLength());

	return TRUE;
	}


BOOL AgsdRelay2Buddy::SetParamUpdate(AuStatement* pStatement)
	{
	pStatement->SetParam(0, (INT32*)&m_dwOptions);
	pStatement->SetParam(1, (INT32*)&m_nMentorStatus);
	pStatement->SetParam(2, m_szCharID, m_szCharID.GetByteLength());
	pStatement->SetParam(3, m_szBuddyID, m_szBuddyID.GetByteLength());
	
	return TRUE;
	}


BOOL AgsdRelay2Buddy::SetParamInsert(AuStatement* pStatement)
	{
	pStatement->SetParam(0, m_szCharID, m_szCharID.GetByteLength());
	pStatement->SetParam(1, m_szBuddyID, m_szBuddyID.GetByteLength());
	pStatement->SetParam(2, (INT32*)&m_dwOptions);
	pStatement->SetParam(3, (INT32*)&m_nMentorStatus);

	return TRUE;
	}


BOOL AgsdRelay2Buddy::SetParamDelete(AuStatement* pStatement)
	{
	pStatement->SetParam(0, m_szCharID, m_szCharID.GetByteLength());
	pStatement->SetParam(1, m_szBuddyID, m_szBuddyID.GetByteLength());

	return TRUE;
	}


void AgsdRelay2Buddy::Dump(CHAR *pszOp)
	{
	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "LOG\\%04d%02d%02d-%02d-RELAY_DUMP_BUDDY.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d:%02d:%02d][%s]"
						"ID[%s] BUDDYID[%s] Options[%d]\n",
				st.wHour, st.wMinute, st.wSecond,
				pszOp, m_szCharID.GetBuffer(), m_szBuddyID.GetBuffer(), m_dwOptions);
	AuLogFile_s(szFile, strCharBuff);
	}