/*===============================================================

	AgsmRelay2WantedCriminal.cpp

===============================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"


//
//	==========		Wanted Criminal		==========
//
void AgsmRelay2::InitPacketWantedCriminal()
	{
	m_csPacketWantedCriminal.SetFlagLength(sizeof(INT16));
	m_csPacketWantedCriminal.SetFieldType(AUTYPE_INT16,			1,		// eAgsmRelay2Operation
										  AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,
										  AUTYPE_INT32,			1,		// char tid
										  AUTYPE_INT32,			1,		// char level
										  AUTYPE_INT64,			1,		// money
										  AUTYPE_PACKET,		1,		// packet
										  AUTYPE_END,			0
										  );

	}


BOOL AgsmRelay2::OnParamWantedCriminal(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2WantedCriminal *pAgsdRelay2 = new AgsdRelay2WantedCriminal;

	CHAR *pszCharID = NULL;

	m_csPacketWantedCriminal.GetField(FALSE, pvPacket, 0,
								&pAgsdRelay2->m_eOperation,
								&pszCharID,
								&pAgsdRelay2->m_lCharTID,
								&pAgsdRelay2->m_lCharLevel,
								&pAgsdRelay2->m_llMoney,
								&pAgsdRelay2->m_pvPacketEmb
								);

	pAgsdRelay2->m_ulNID = ulNID;
	
	_tcsncpy(pAgsdRelay2->m_szCharID, pszCharID ? pszCharID : _T(""), _MAX_CHARNAME_LENGTH);

	// dummy conversion
	_i64tot(pAgsdRelay2->m_llMoney, pAgsdRelay2->m_szMoney, 10);

	return EnumCallback(AGSMRELAY_PARAM_WANTEDCRIMINAL, (PVOID)pAgsdRelay2, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBOperationWantedCriminal(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationWantedCriminal,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBFinishOperationWantedCriminal(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_EXECUTE:
			pThis->OnSelectResultOffCharInfo(pRowset, pAgsdRelay2);
			break;
		case AGSMDATABASE_OPERATION_SELECT:
			pThis->OnSelectResultWantedCriminal(pRowset, pAgsdRelay2);
			break;
		
		case AGSMDATABASE_OPERATION_INSERT:
		case AGSMDATABASE_OPERATION_UPDATE:
		case AGSMDATABASE_OPERATION_DELETE:
			break;
		}

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBOperationResultWantedCriminal(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2WantedCriminal *pAgsdRelay2 = (AgsdRelay2WantedCriminal *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT:
			{
			stBuddyRowset Rowset;
			if (NULL == pAgsdRelay2->m_pvPacketEmb && AGSMRELAY2_ROWSET_IND_EOF == pAgsdRelay2->m_lCharTID)
				{
				if (FALSE == pThis->m_pAgsmWantedCriminal->OnSelectResultWantedCriminalList(NULL, TRUE))
					return FALSE;
				}
			
			if (FALSE == pThis->ParseBuddyRowsetPacket(pAgsdRelay2->m_pvPacketEmb, &Rowset))
				return FALSE;

			if (FALSE == pThis->m_pAgsmWantedCriminal->OnSelectResultWantedCriminalList(&Rowset))
				return FALSE;
			}
			break;
		
		case AGSMDATABASE_OPERATION_EXECUTE:
			{
			pThis->m_pAgsmWantedCriminal->OnSelectOfflineCharacter(pAgsdRelay2->m_szCharID,
																   pAgsdRelay2->m_lCharTID,
																   pAgsdRelay2->m_lCharLevel
																   );
			break;
			}
		
		default:
			break;
		}
			
	return TRUE;
	}


BOOL AgsmRelay2::OnSelectResultWantedCriminal(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2WantedCriminal *pAgsdRelay2WantedCriminal = static_cast<AgsdRelay2WantedCriminal *>(pAgsdRelay2);

	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2WantedCriminal->m_ulNID);
	if (!pGameServer)
		return FALSE;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	INT32 lTotalSize = pRowset->GetRowBufferSize() * pRowset->GetRowCount();
	INT32 lTotalStep = 1 + (INT32) (lTotalSize / 20000);
	INT32 lRowsPerStep = 20000 / pRowset->GetRowBufferSize();
	for (INT32 lStep = 0; lStep < lTotalStep; lStep++)
		{
		PVOID pvPacket = NULL;
		pAgsdRelay2WantedCriminal->m_pvPacketEmb = MakeRowsetPacket2(pRowset, lStep, lRowsPerStep);
		pAgsdRelay2WantedCriminal->m_lCharTID = 0;

		BOOL bResult = SendWantedCriminal(pAgsdRelay2WantedCriminal, pGameServer->m_dpnidServer);

		printf("\nWanted Criminal List sended[%d/%d][%s]\n", lStep+1, lTotalStep,  bResult ? _T("TRUE") : _T("FALSE"));
		}

	// last
	pAgsdRelay2WantedCriminal->m_pvPacketEmb = NULL;
	pAgsdRelay2WantedCriminal->m_lCharTID = AGSMRELAY2_ROWSET_IND_EOF;

	return SendWantedCriminal(pAgsdRelay2WantedCriminal, pGameServer->m_dpnidServer);
	}


BOOL AgsmRelay2::OnSelectResultOffCharInfo(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2WantedCriminal *pAgsdRelay2WantedCriminal = static_cast<AgsdRelay2WantedCriminal *>(pAgsdRelay2);
	if (0 >= pRowset->GetRowCount())
		return FALSE;

	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2WantedCriminal->m_ulNID);
	if (!pGameServer)
		return FALSE;
	
	CHAR *psz = NULL;
	psz = (CHAR *) pRowset->Get(0, 0);
	if (psz)
		pAgsdRelay2WantedCriminal->m_lCharTID = _ttoi(psz);
	psz = (CHAR *) pRowset->Get(0, 1);
	if (psz)
		pAgsdRelay2WantedCriminal->m_lCharLevel = _ttoi(psz);

	return SendWantedCriminal(pAgsdRelay2WantedCriminal, pGameServer->m_dpnidServer);
	}


BOOL AgsmRelay2::CBWantedCriminalSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgsmRelay2			*pThis = (AgsmRelay2 *)	pClass;

	AgsdRelay2WantedCriminal csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_SELECT;

	return pThis->SendWantedCriminal(&csRelay2);
	}


BOOL AgsmRelay2::CBWantedCriminalUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay2			*pThis = (AgsmRelay2 *)	pClass;
	AgpdWantedCriminal	*pAgpdWantedCriminal = (AgpdWantedCriminal *) pData;

	AgsdRelay2WantedCriminal csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;
	_tcsncpy(csRelay2.m_szCharID, pAgpdWantedCriminal->m_szCharID, _MAX_CHARNAME_LENGTH);
	csRelay2.m_llMoney = pAgpdWantedCriminal->m_llMoney;

	return pThis->SendWantedCriminal(&csRelay2);
	}


BOOL AgsmRelay2::CBWantedCriminalInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay2			*pThis = (AgsmRelay2 *)	pClass;
	AgpdWantedCriminal	*pAgpdWantedCriminal = (AgpdWantedCriminal *) pData;
	
	AgsdRelay2WantedCriminal csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_INSERT;	
	_tcsncpy(csRelay2.m_szCharID, pAgpdWantedCriminal->m_szCharID, _MAX_CHARNAME_LENGTH);
	csRelay2.m_llMoney = pAgpdWantedCriminal->m_llMoney;
		
	return pThis->SendWantedCriminal(&csRelay2);
	}


BOOL AgsmRelay2::CBWantedCriminalDelete(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay2			*pThis = (AgsmRelay2 *)	pClass;
	AgpdWantedCriminal	*pAgpdWantedCriminal = (AgpdWantedCriminal *) pData;

	AgsdRelay2WantedCriminal csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_DELETE;	
	_tcsncpy(csRelay2.m_szCharID, pAgpdWantedCriminal->m_szCharID, _MAX_CHARNAME_LENGTH);
	csRelay2.m_llMoney = pAgpdWantedCriminal->m_llMoney;

	return pThis->SendWantedCriminal(&csRelay2);
	}


BOOL AgsmRelay2::CBWantedCriminalOffCharInfo(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay2			*pThis = (AgsmRelay2 *)	pClass;
	AgpdWantedCriminal	*pAgpdWantedCriminal = (AgpdWantedCriminal *) pData;

	AgsdRelay2WantedCriminal csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;	
	_tcsncpy(csRelay2.m_szCharID, pAgpdWantedCriminal->m_szCharID, _MAX_CHARNAME_LENGTH);

	return pThis->SendWantedCriminal(&csRelay2);
	}


BOOL AgsmRelay2::SendWantedCriminal(AgsdRelay2WantedCriminal *pAgsdRelay2, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketEmb= m_csPacketWantedCriminal.MakePacket(FALSE, &nPacketLength, 0,
										&pAgsdRelay2->m_eOperation,
										pAgsdRelay2->m_szCharID,		// char id
										&pAgsdRelay2->m_lCharTID,		// char tid
										&pAgsdRelay2->m_lCharLevel,		// char level
										&pAgsdRelay2->m_llMoney,		// money
										pAgsdRelay2->m_pvPacketEmb
										);
	if (!pvPacketEmb)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_WANTEDCRIMINAL);
	else
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_WANTEDCRIMINAL, ulNID);

	m_csPacketWantedCriminal.FreePacket(pvPacketEmb);
	
	return bResult;	
	}




/********************************************************************/
/*		The Implementation of AgsdRelay2WantedCriminal class		*/
/********************************************************************/
//
AgsdRelay2WantedCriminal::AgsdRelay2WantedCriminal()
	{
	ZeroMemory(m_szCharID, sizeof(m_szCharID));
	m_lCharTID = 0;
	m_lCharLevel = 0;
	m_llMoney = 0;
	m_pvPacketEmb = NULL;
	
	// dummy
	ZeroMemory(m_szMoney, sizeof(m_szMoney));
	}


BOOL AgsdRelay2WantedCriminal::SetParamSelect(AuStatement* pStatement)
	{
	INT16 i=0;

	//pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	
	return TRUE;
	}


BOOL AgsdRelay2WantedCriminal::SetParamUpdate(AuStatement* pStatement)
	{
	INT16 i=0;

	pStatement->SetParam(i++, m_szMoney, sizeof(m_szMoney));
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	
	return TRUE;
	}


BOOL AgsdRelay2WantedCriminal::SetParamInsert(AuStatement *pStatement)
	{
	INT16 i=0;
	
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	pStatement->SetParam(i++, m_szMoney, sizeof(m_szMoney));
	
	return TRUE;	
	}


BOOL AgsdRelay2WantedCriminal::SetParamDelete(AuStatement *pStatement)
	{
	INT16 i=0;
	
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	
	return TRUE;	
	}

BOOL AgsdRelay2WantedCriminal::SetParamExecute(AuStatement *pStatement)
	{
	INT16 i=0;
	
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	
	return TRUE;
	}


void AgsdRelay2WantedCriminal::Dump(CHAR *pszOp)
	{
	}


