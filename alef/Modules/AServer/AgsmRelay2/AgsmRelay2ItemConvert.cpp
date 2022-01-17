/*===============================================================

	AgsmRelay2ItemConvert.cpp

===============================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"


//
//	==========		Item Convert		==========
//
void AgsmRelay2::InitPacketItemConvert()
	{
	m_csPacketItemConvert.SetFlagLength(sizeof(INT8));
	m_csPacketItemConvert.SetFieldType(AUTYPE_INT16,		1,		// eAgsmRelay2Operation
									   AUTYPE_UINT64,		1,	// DBID
									   AUTYPE_MEMORY_BLOCK,	1,	// encoded all history list
									   AUTYPE_END,			0
									   );
	}


BOOL AgsmRelay2::OnParamItemConvert(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2ItemConvert *pcsRelayItemConvert = new AgsdRelay2ItemConvert;
	
	UINT64		ullDBID = 0;
	CHAR*		pConvertHistory = NULL;
	INT16		nConvertHistoryLength = 0;

	m_csPacketItemConvert.GetField(FALSE, pvPacket, 0, 
								   &pcsRelayItemConvert->m_eOperation,
								   &ullDBID,
								   &pConvertHistory,
								   &nConvertHistoryLength
								   );

	if (0 == ullDBID)
		{
		pcsRelayItemConvert->Release();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!!Error : DBID is 0 in OnParamItem()\n");
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);

		return FALSE;
		}

	pcsRelayItemConvert->m_ulNID = ulNID;
	_i64toa(ullDBID, pcsRelayItemConvert->m_szDBID, 10);

	if (nConvertHistoryLength <= _MAX_CONVERT_HISTORY_LENGTH)
		{
		_tcsncpy(pcsRelayItemConvert->m_szConvertHistory, pConvertHistory ? pConvertHistory : _T(""), nConvertHistoryLength);
		pcsRelayItemConvert->m_szConvertHistory[nConvertHistoryLength] = _ctextend;
		}
	else
		{
		_tcsncpy(pcsRelayItemConvert->m_szConvertHistory, pConvertHistory ? pConvertHistory : _T(""), _MAX_CONVERT_HISTORY_LENGTH);
		pcsRelayItemConvert->m_szConvertHistory[_MAX_CONVERT_HISTORY_LENGTH] = _ctextend;
		}

	if (m_pAgsmDatabasePool)
		pcsRelayItemConvert->m_nForcedPoolIndex = (INT16) (ullDBID % m_pAgsmDatabasePool->m_nTotalForced) + 
															m_pAgsmDatabasePool->m_nForcedStart;

	return EnumCallback(AGSMRELAY_PARAM_ITEM_CONVERT_HISTORY, (PVOID)pcsRelayItemConvert, (PVOID)nParam);
	}


BOOL AgsmRelay2::SendItemConvertUpdate(AgpdItem *pAgpdItem)
	{
	if (!pAgpdItem || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	CHAR	szConvertHistory[_MAX_CONVERT_HISTORY_LENGTH + 1];
	ZeroMemory(szConvertHistory, sizeof(szConvertHistory));
	m_pAgsmItemConvert->EncodeConvertHistory(pAgpdItem, szConvertHistory, _MAX_CONVERT_HISTORY_LENGTH);
	INT16	nConvertHistoryLength	= (INT16)_tcslen(szConvertHistory);
	INT16	nPacketLength	= 0;
	AgsdItem	*pcsAgsdItem	= m_pAgsmItem->GetADItem(pAgpdItem);

	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;

	PVOID	pvPacketItemConvertUpdate	= m_csPacketItemConvert.MakePacket(FALSE, &nPacketLength, 0,
																	&nOperation,
																	&pcsAgsdItem->m_ullDBIID,
																	szConvertHistory,
																	&nConvertHistoryLength);

	if (!pvPacketItemConvertUpdate)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketItemConvertUpdate, AGSMRELAY_PARAM_ITEM_CONVERT_HISTORY);
	m_csPacketItemConvert.FreePacket(pvPacketItemConvertUpdate);

	return bResult;
	}


BOOL AgsmRelay2::CBItemConvertUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay2	*pThis		= (AgsmRelay2 *)	pClass;
	AgpdItem	*pcsItem	= (AgpdItem *)	pData;

	if (pThis->m_pAgpmCharacter->IsReturnToLoginServerStatus(pcsItem->m_pcsCharacter))
		return TRUE;

	return pThis->SendItemConvertUpdate(pcsItem);
	}




/****************************************************************/
/*		The Implementation of AgsdRelay2ItemConvert class		*/
/****************************************************************/
//
AgsdRelay2ItemConvert::AgsdRelay2ItemConvert()
	{
	ZeroMemory(m_szDBID, sizeof(m_szDBID));
	ZeroMemory(m_szConvertHistory, sizeof(m_szConvertHistory));
	}


BOOL AgsdRelay2ItemConvert::SetParamUpdate(AuStatement* pStatement)
	{
	pStatement->SetParam(0, m_szConvertHistory, sizeof(m_szConvertHistory));
	pStatement->SetParam(1, m_szDBID, sizeof(m_szDBID));

	return TRUE;
	}


void AgsdRelay2ItemConvert::Dump(CHAR *pszOp)
	{
	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "LOG\\%04d%02d%02d-%02d-RELAY_DUMP_ITCV.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d:%02d:%02d][%s]"
						"SEQ[%s] Hist[%s]\n",
				st.wHour, st.wMinute, st.wSecond,
				pszOp, m_szDBID, m_szConvertHistory);
	AuLogFile_s(szFile, strCharBuff);
	}
