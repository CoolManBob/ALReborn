/*===============================================================


	AgsmRelay2Item.cpp

===============================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"
#include "AuTimeStamp.h"

//
//	==========		Item		==========
//
void AgsmRelay2::InitPacketItem()
	{
	m_csPacketItem.SetFlagLength(sizeof(INT32));
	m_csPacketItem.SetFieldType(AUTYPE_INT16,		1,							// eAgsmRelay2Operation
								AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,	// Account ID
								AUTYPE_CHAR,		_MAX_SERVERNAME_LENGTH + 1,	// Server Name
								AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,	// Owner ID
								AUTYPE_UINT64,		1,							// DB ID
								AUTYPE_INT32,		1,							// Item template ID
								AUTYPE_INT32,		1,							// Stack count
								AUTYPE_INT16,		1,							// Status
								AUTYPE_CHAR,		_MAX_POSITION_LENGTH + 1,	// encoded position (grid or field)
								AUTYPE_INT32,		1,							// Need level
								AUTYPE_INT32,		1,							// Durability
								AUTYPE_INT32,		1,							// Max Durability
								AUTYPE_INT32,		1,							// status flag
								AUTYPE_CHAR,		_MAX_OPTION_LENGTH + 1,		// option template
								AUTYPE_CHAR,		_MAX_OPTION_LENGTH + 1,		// skill plus
								AUTYPE_INT16,		1,							// in use
								AUTYPE_INT32,		1,							// use count
								AUTYPE_INT64,		1,							// remain time
								AUTYPE_UINT32,		1,							// expire time
								AUTYPE_INT64,		1,							// stamina remaing time
								AUTYPE_MEMORY_BLOCK,		1,					// Deleted Reason
								AUTYPE_MEMORY_BLOCK,		1,					// Convert History
								AUTYPE_END,         0
								);
	}


BOOL AgsmRelay2::OnParamItem(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Item *pcsRelayItem = new AgsdRelay2Item;

	CHAR	*pszAccount = NULL;
	CHAR	*pszCharName = NULL;
	CHAR	*pszServerName = NULL;
	CHAR	*pszPosition = NULL;
	CHAR	*pszOption = NULL;
	CHAR	*pszSkillPlus = NULL;
	CHAR	*pDeleteReason = NULL;
	INT16	nDeleteReasonLength = 0;
	CHAR*	pConvertHistory = NULL;
	INT16	nConvertHistoryLength = 0;
	UINT64	ullDBID = 0;
	
	m_csPacketItem.GetField(FALSE, pvPacket, 0,
							&pcsRelayItem->m_eOperation,
							&pszAccount,
							&pszServerName,
							&pszCharName,
							&ullDBID,
							&pcsRelayItem->m_lTemplateID,
							&pcsRelayItem->m_lStackCount,
							&pcsRelayItem->m_nStatus,
							&pszPosition,
							&pcsRelayItem->m_lNeedLevel,
							&pcsRelayItem->m_lDurability,
							&pcsRelayItem->m_lMaxDurability,
							&pcsRelayItem->m_lStatusFlag,
							&pszOption,
							&pszSkillPlus,
							&pcsRelayItem->m_nInUse,
							&pcsRelayItem->m_lUseCount,
							&pcsRelayItem->m_lRemainTime,
							&pcsRelayItem->m_lExpireTime,
							&pcsRelayItem->m_llStaminaRemainTime,
							&pDeleteReason,
							&nDeleteReasonLength,
							&pConvertHistory,
							&nConvertHistoryLength
							);

	if (0 == ullDBID)
		{
		pcsRelayItem->Release();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!!Error : DBID is 0 in OnParamItem()\n");
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);
		return FALSE;
		}

	pcsRelayItem->m_ulNID = ulNID;
	_tcsncpy(pcsRelayItem->m_szCharName, pszCharName ? pszCharName : _T(""), _MAX_CHARNAME_LENGTH);
	_tcsncpy(pcsRelayItem->m_szPosition, pszPosition ? pszPosition : _T(""), _MAX_POSITION_LENGTH);
	_tcsncpy(pcsRelayItem->m_szOption, pszOption ? pszOption : _T(""), _MAX_OPTION_LENGTH);
	_tcsncpy(pcsRelayItem->m_szSkillPlus, pszSkillPlus ? pszSkillPlus : _T(""), _MAX_OPTION_LENGTH);
	_tcsncpy(pcsRelayItem->m_szAccount, pszAccount ? pszAccount: _T(""), _MAX_CHARNAME_LENGTH);
	_tcsncpy(pcsRelayItem->m_szServerName, pszServerName ? pszServerName : _T(""), _MAX_SERVERNAME_LENGTH);
	if (AGSMRELAY_PARAM_GUILD_WAREHOUSE_ITEM == nParam)
		{
		if (nDeleteReasonLength <= _MAX_CONVERT_HISTORY_LENGTH)
			{
			_tcsncpy(pcsRelayItem->m_szConvertDummy, pDeleteReason ? pDeleteReason : _T(""), nDeleteReasonLength);
			pcsRelayItem->m_szConvertDummy[nDeleteReasonLength] = _ctextend;
			}
		else
			{
			_tcsncpy(pcsRelayItem->m_szConvertDummy, pDeleteReason ? pDeleteReason : _T(""), _MAX_CONVERT_HISTORY_LENGTH);
			pcsRelayItem->m_szConvertDummy[_MAX_CONVERT_HISTORY_LENGTH] = _ctextend;
			}
		}
	else
		{
		if (nDeleteReasonLength <= _MAX_REASON_LENGTH)
			{
			_tcsncpy(pcsRelayItem->m_szDeleteReason, pDeleteReason ? pDeleteReason : _T(""), nDeleteReasonLength);
			pcsRelayItem->m_szDeleteReason[nDeleteReasonLength] = _ctextend;
			}
		else
			{
			_tcsncpy(pcsRelayItem->m_szDeleteReason, pDeleteReason ? pDeleteReason : _T(""), _MAX_REASON_LENGTH);
			pcsRelayItem->m_szDeleteReason[_MAX_REASON_LENGTH] = _ctextend;
			}
		}

	// 2008.01.08. steeple
	if (nConvertHistoryLength <= _MAX_CONVERT_HISTORY_LENGTH)
		{
		_tcsncpy(pcsRelayItem->m_szConvertHistory, pConvertHistory ? pConvertHistory : _T(""), nConvertHistoryLength);
		pcsRelayItem->m_szConvertHistory[nConvertHistoryLength] = _ctextend;
		}
	else
		{
		_tcsncpy(pcsRelayItem->m_szConvertHistory, pConvertHistory ? pConvertHistory : _T(""), _MAX_CONVERT_HISTORY_LENGTH);
		pcsRelayItem->m_szConvertHistory[_MAX_CONVERT_HISTORY_LENGTH] = _ctextend;
		}

	_i64toa(ullDBID, pcsRelayItem->m_szDBID, 10);
	
	AuTimeStamp::ConvertTimeStampToOracleTime(pcsRelayItem->m_lExpireTime, pcsRelayItem->m_szExpireTime, sizeof(pcsRelayItem->m_szExpireTime));
	
	if (m_pAgsmDatabasePool)
		pcsRelayItem->m_nForcedPoolIndex = (INT16)(ullDBID % m_pAgsmDatabasePool->m_nTotalForced) + 
										   m_pAgsmDatabasePool->m_nForcedStart;

	return EnumCallback(nParam, (PVOID)pcsRelayItem, (PVOID)nParam);
	}




//	Game server side callback
//=============================================
//
BOOL AgsmRelay2::CBItemInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(pData && pClass && "Item Insert를 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay2*	pThis	= (AgsmRelay2*)pClass;
	AgpdItem*	pAgpdItem	= (AgpdItem*)pData;
	UINT64		lDBID		= *((UINT64*)pCustData);
	
	if (pThis->m_pAgpmCharacter->IsReturnToLoginServerStatus(pAgpdItem->m_pcsCharacter))
		return TRUE;

	return pThis->SendItemInsert(pAgpdItem, lDBID);
	}


BOOL AgsmRelay2::CBItemUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(pData && pClass && pCustData && "Item Update 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay2*	pThis		= (AgsmRelay2*)pClass;
	AgpdItem*		pAgpdItem	= (AgpdItem*)pData;
	UINT64			lDBID		= *((UINT64*)pCustData);

	if (pThis->m_pAgpmCharacter->IsReturnToLoginServerStatus(pAgpdItem->m_pcsCharacter))
		return TRUE;

	return pThis->SendItemUpdate(pAgpdItem, lDBID);
	}


BOOL AgsmRelay2::CBItemDelete(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(pData && pClass && pCustData && "Item Delete 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay2*	pThis		= (AgsmRelay2*)pClass;
	UINT64			lDBID		= *((UINT64*)pCustData);
	AgpdItem		*pcsItem	= (AgpdItem *) pData;

	return pThis->SendItemDelete(lDBID, pcsItem->m_szDeleteReason, IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType));
	}




//	Send
//=============================================
//
BOOL AgsmRelay2::SendItemInsert(AgpdItem *pAgpdItem, UINT64 ullDBID)
	{
	if (NULL == pAgpdItem || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;
	
	// item packet(embedded)
	INT16 nOperation = AGSMDATABASE_OPERATION_INSERT;
	INT16	nPacketLength = 0;
	PVOID pvPacketItem = MakeItemPacket(pAgpdItem, ullDBID, nOperation, &nPacketLength);
	if (!pvPacketItem)
		return FALSE;

	BOOL bResult = FALSE;
	if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
		bResult = MakeAndSendRelayPacket(pvPacketItem, AGSMRELAY_PARAM_CASHITEM);
	else
		bResult = MakeAndSendRelayPacket(pvPacketItem, AGSMRELAY_PARAM_ITEM);
	m_csPacketItem.FreePacket(pvPacketItem);
	
	return bResult;
	}


BOOL AgsmRelay2::SendItemUpdate(AgpdItem *pAgpdItem, UINT64 ullDBID)
	{
	if (NULL == pAgpdItem || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	if (NULL == pAgpdItem->m_pcsCharacter && 0 == pAgpdItem->m_lPrevOwner)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("SendItemUpdate"));

	// item packet(embedded)
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	INT16	nPacketLength = 0;
	PVOID pvPacketItem = MakeItemPacket(pAgpdItem, ullDBID, nOperation, &nPacketLength);
	if (!pvPacketItem)
		return FALSE;

	BOOL bResult = FALSE;
	if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
		bResult = MakeAndSendRelayPacket(pvPacketItem, AGSMRELAY_PARAM_CASHITEM);
	else
		bResult = MakeAndSendRelayPacket(pvPacketItem, AGSMRELAY_PARAM_ITEM);
	m_csPacketItem.FreePacket(pvPacketItem);

	return bResult;
	}


BOOL AgsmRelay2::SendItemDelete(UINT64 ullDBID, CHAR *szDeleteReason, BOOL bCash)
	{
	if (!m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength	= 0;
	INT16 nDeleteReasonLength	= (INT16)_tcslen(szDeleteReason);
	INT16 nOperation = AGSMDATABASE_OPERATION_DELETE;

	PVOID pvPacketItem = m_csPacketItem.MakePacket(FALSE, &nPacketLength, 0,
													&nOperation,
													NULL,
													NULL,
													NULL,
													&ullDBID,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													szDeleteReason,
													&nDeleteReasonLength,
													NULL
													);
	if (!pvPacketItem) return FALSE;
	
	BOOL bResult = FALSE;
	if (bCash)
		bResult = MakeAndSendRelayPacket(pvPacketItem, AGSMRELAY_PARAM_CASHITEM);
	else
		bResult = MakeAndSendRelayPacket(pvPacketItem, AGSMRELAY_PARAM_ITEM);
	m_csPacketItem.FreePacket(pvPacketItem);

	return bResult;
	}




//	Extra
//========================================================
//
PVOID AgsmRelay2::MakeItemPacket(AgpdItem *pAgpdItem, UINT64 ullDBID, INT16 nOperation, INT16 *pnPacketLength, CHAR *pszGuildID,
									PVOID pvExtra, INT16 *pnExtraLength, stAgpdAdminItemOperation* pstItemOperation)
	{
	if (NULL == pAgpdItem || NULL == pnPacketLength)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("MakeItemPacket"));

	INT32	lNeedLevel	= 0;
	INT32	lDurability	= 0;
	INT32	lMaxDurability = 0;
	AuPOS	stPos;
	CHAR	szPosition[_MAX_POSITION_LENGTH + 1];		ZeroMemory(szPosition, sizeof(szPosition));
	CHAR	szOption[_MAX_OPTION_LENGTH + 1];			ZeroMemory(szOption, sizeof(szOption));
	CHAR	szSkillPlus[_MAX_OPTION_LENGTH + 1];		ZeroMemory(szSkillPlus, sizeof(szSkillPlus));
	CHAR	*pszCharID = NULL;
	CHAR	*pszAccountID = NULL;
	CHAR	*pszServerName = NULL;
	
	
	if (pszGuildID)
		pszCharID = pszGuildID;
	else
		pszCharID = pAgpdItem->m_pcsCharacter ? pAgpdItem->m_pcsCharacter->m_szID : NULL;

	if(pAgpdItem->m_pcsCharacter)
	{
		pszAccountID = m_pAgsmCharacter->GetAccountID(pAgpdItem->m_pcsCharacter);
		pszServerName = m_pAgsmCharacter->GetServerName(pAgpdItem->m_pcsCharacter);
	}
	else if(pstItemOperation)
	{
		// 어드민이 아이템을 만들 때 2007.02.14. steeple
		pszCharID = pstItemOperation->m_szCharName;
		pszAccountID = pstItemOperation->m_szAccName;

		// 여기서는 ServerName 못 구한다.
	}
	
	// position
	if (AGPDITEM_STATUS_FIELD == pAgpdItem->m_eStatus)
		stPos	= pAgpdItem->m_posItem;
	else
		{
		stPos.x	= (FLOAT) pAgpdItem->m_anGridPos[0];
		stPos.y	= (FLOAT) pAgpdItem->m_anGridPos[1];
		stPos.z	= (FLOAT) pAgpdItem->m_anGridPos[2];
		}
	m_pAgsmCharacter->EncodingPosition(&stPos, szPosition, _MAX_POSITION_LENGTH);
	// need level
	m_pAgpmFactors->GetValue(&pAgpdItem->m_csRestrictFactor, &lNeedLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	// durability
	m_pAgpmFactors->GetValue(&pAgpdItem->m_csFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
	// max durability
	m_pAgpmFactors->GetValue(&pAgpdItem->m_csFactor, &lMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);
	// option
	if (0 != pAgpdItem->m_aunOptionTID[0])			// at least one option
		m_pAgsmItem->EncodingOption(pAgpdItem, szOption, _MAX_OPTION_LENGTH);
	// skill plus
	if (0 != pAgpdItem->m_aunSkillPlusTID[0])			// at least one option
		m_pAgsmItem->EncodingSkillPlus(pAgpdItem, szSkillPlus, _MAX_OPTION_LENGTH);
	// skill TID
	INT32 lSkillTID	= pAgpdItem->m_lSkillTID;
	// cash item use flag. 2006.01.06. steeple	일시 정지중인 아이템은 사용중으로 바꿔주는센스
	INT16 nInUseItem = (INT16) pAgpdItem->m_nInUseItem;
	if (AGPDITEM_CASH_ITEM_PAUSE == nInUseItem)
		nInUseItem = AGPDITEM_CASH_ITEM_INUSE;

	// 2008.01.08. steeple
	CHAR	szConvertHistory[_MAX_CONVERT_HISTORY_LENGTH + 1];
	ZeroMemory(szConvertHistory, sizeof(szConvertHistory));
	m_pAgsmItemConvert->EncodeConvertHistory(pAgpdItem, szConvertHistory, _MAX_CONVERT_HISTORY_LENGTH);
	INT16	nConvertHistoryLength	= (INT16)_tcslen(szConvertHistory);

	PVOID pvPacketItem = NULL;
	
	if (pvExtra && pnExtraLength)
		{
		pvPacketItem = m_csPacketItem.MakePacket(FALSE, pnPacketLength, 0,
													&nOperation,
													pszAccountID,
													pszServerName,
													pszCharID,
													&ullDBID,
													&pAgpdItem->m_pcsItemTemplate->m_lID,
													&pAgpdItem->m_nCount,
													&pAgpdItem->m_eStatus,
													szPosition,
													&lNeedLevel,		// JNY TODO 2004.3.3 : Need level
													&lDurability,
													&lMaxDurability,
													&pAgpdItem->m_lStatusFlag,
													(0 != pAgpdItem->m_aunOptionTID[0]) ? szOption : NULL,
													szSkillPlus,
													&nInUseItem,
													&pAgpdItem->m_lCashItemUseCount,
													&pAgpdItem->m_lRemainTime,
													&pAgpdItem->m_lExpireTime,
													&pAgpdItem->m_llStaminaRemainTime,
													pvExtra,
													pnExtraLength,
													szConvertHistory,
													&nConvertHistoryLength);
		}
	else
		{
		pvPacketItem = m_csPacketItem.MakePacket(FALSE, pnPacketLength, 0,
													&nOperation,
													pszAccountID,
													pszServerName,
													pszCharID,
													&ullDBID,
													&pAgpdItem->m_pcsItemTemplate->m_lID,
													&pAgpdItem->m_nCount,
													&pAgpdItem->m_eStatus,
													szPosition,
													&lNeedLevel,		// JNY TODO 2004.3.3 : Need level
													&lDurability,
													&lMaxDurability,
													&pAgpdItem->m_lStatusFlag,
													(0 != pAgpdItem->m_aunOptionTID[0]) ? szOption : NULL,
													szSkillPlus,
													&nInUseItem,
													&pAgpdItem->m_lCashItemUseCount,
													&pAgpdItem->m_lRemainTime,
													&pAgpdItem->m_lExpireTime,
													&pAgpdItem->m_llStaminaRemainTime,
													NULL,
													szConvertHistory,
													&nConvertHistoryLength);
		}

	return pvPacketItem;
	}




/********************************************************/
/*		The Implementation of AgsdRelay2Item class		*/
/********************************************************/
//
AgsdRelay2Item::AgsdRelay2Item()
	{
	ZeroMemory(m_szDBID, sizeof(m_szDBID));
	ZeroMemory(m_szAccount, sizeof(m_szAccount));
	ZeroMemory(m_szServerName, sizeof(m_szServerName));
	ZeroMemory(m_szCharName, sizeof(m_szCharName));
	m_lTemplateID = 0;
	m_lStackCount = 0;
	m_nStatus = 0;
	ZeroMemory(m_szPosition, sizeof(m_szPosition));
	m_lNeedLevel = 0;
	m_lDurability = 0;
	m_lMaxDurability = 0;
	ZeroMemory(m_szDeleteReason, sizeof(m_szDeleteReason));
	m_lStatusFlag = 0;
	ZeroMemory(m_szOption, sizeof(m_szOption));
	ZeroMemory(m_szSkillPlus, sizeof(m_szSkillPlus));
	m_nInUse = 0;
	m_lRemainTime = 0;
	m_lExpireTime = 0;
	m_llStaminaRemainTime = 0;

	ZeroMemory(m_szConvertHistory, sizeof(m_szConvertHistory));

	// dummy
	ZeroMemory(m_szExpireTime, sizeof(m_szExpireTime));
	ZeroMemory(m_szConvertDummy, sizeof(m_szConvertDummy));
	}


BOOL AgsdRelay2Item::SetParamInsert(AuStatement* pStatement)
	{
	INT16 i = 0;
	
	// 2008.01.16. steeple
	// Cash Item 인 경우는 m_nStatus 고정
	if (AGSMRELAY_PARAM_CASHITEM == m_nParam)
		m_nStatus = AGPDITEM_STATUS_CASH_INVENTORY;
	
	if (AGSMRELAY_PARAM_ITEM == m_nParam || AGSMRELAY_PARAM_CASHITEM == m_nParam)
		{
		pStatement->SetParam(i++, m_szDBID, sizeof(m_szDBID));
		pStatement->SetParam(i++, m_szAccount, sizeof(m_szAccount));
		pStatement->SetParam(i++, m_szServerName, sizeof(m_szServerName));
		pStatement->SetParam(i++, m_szCharName, sizeof(m_szCharName));
		pStatement->SetParam(i++, &m_lTemplateID);
		pStatement->SetParam(i++, &m_lStackCount);
		pStatement->SetParam(i++, &m_nStatus);
		pStatement->SetParam(i++, m_szPosition, sizeof(m_szPosition));
		pStatement->SetParam(i++, &m_lDurability);
		pStatement->SetParam(i++, &m_lMaxDurability);
		pStatement->SetParam(i++, &m_lStatusFlag);
		pStatement->SetParam(i++, m_szOption, sizeof(m_szOption));
		pStatement->SetParam(i++, m_szSkillPlus, sizeof(m_szSkillPlus));
		pStatement->SetParam(i++, &m_nInUse);
		pStatement->SetParam(i++, &m_lUseCount);
		pStatement->SetParam(i++, (UINT32*)&m_lRemainTime);
		pStatement->SetParam(i++, m_szExpireTime, sizeof(m_szExpireTime));
		pStatement->SetParam(i++, m_szConvertHistory, sizeof(m_szConvertHistory));
		pStatement->SetParam(i++, (UINT32*)&m_llStaminaRemainTime);
		}
	else if (AGSMRELAY_PARAM_GUILD_WAREHOUSE_ITEM == m_nParam)
		{
		pStatement->SetParam(i++, m_szDBID, sizeof(m_szDBID));
		pStatement->SetParam(i++, m_szCharName, sizeof(m_szCharName));			// as guild ID
		pStatement->SetParam(i++, &m_lTemplateID);
		pStatement->SetParam(i++, &m_lStackCount);
		pStatement->SetParam(i++, &m_nStatus);
		pStatement->SetParam(i++, m_szPosition, sizeof(m_szPosition));
		pStatement->SetParam(i++, m_szConvertDummy, sizeof(m_szConvertDummy));
		pStatement->SetParam(i++, &m_lDurability);
		pStatement->SetParam(i++, &m_lMaxDurability);
		pStatement->SetParam(i++, &m_lStatusFlag);
		pStatement->SetParam(i++, m_szOption, sizeof(m_szOption));
		pStatement->SetParam(i++, m_szSkillPlus, sizeof(m_szSkillPlus));
		pStatement->SetParam(i++, &m_nInUse);
		pStatement->SetParam(i++, &m_lUseCount);
		pStatement->SetParam(i++, (UINT32*)&m_lRemainTime);
		pStatement->SetParam(i++, m_szExpireTime, sizeof(m_szExpireTime));
		pStatement->SetParam(i++, (UINT32*)&m_llStaminaRemainTime);
		}
	else
		return FALSE;

	return TRUE;
	}


BOOL AgsdRelay2Item::SetParamUpdate(AuStatement* pStatement)
	{
	// 2008.01.16. steeple
	// Cash Item 인 경우는 m_nStatus 고정
	if (AGSMRELAY_PARAM_CASHITEM == m_nParam)
		m_nStatus = AGPDITEM_STATUS_CASH_INVENTORY;

	INT16 i = 0;
	pStatement->SetParam(i++, m_szAccount, sizeof(m_szAccount));
	pStatement->SetParam(i++, m_szServerName, sizeof(m_szServerName));
	pStatement->SetParam(i++, m_szCharName, sizeof(m_szCharName));
	pStatement->SetParam(i++, &m_lStackCount);
	pStatement->SetParam(i++, &m_nStatus);
	pStatement->SetParam(i++, m_szPosition, sizeof(m_szPosition));
	pStatement->SetParam(i++, &m_lDurability);
	pStatement->SetParam(i++, &m_lMaxDurability);
	pStatement->SetParam(i++, &m_lStatusFlag);
	pStatement->SetParam(i++, m_szOption, sizeof(m_szOption));
	pStatement->SetParam(i++, m_szSkillPlus, sizeof(m_szSkillPlus));
	pStatement->SetParam(i++, &m_nInUse);
	pStatement->SetParam(i++, &m_lUseCount);
	pStatement->SetParam(i++, (UINT32*)&m_lRemainTime);
	pStatement->SetParam(i++, m_szExpireTime, sizeof(m_szExpireTime));
	pStatement->SetParam(i++, (UINT32*)&m_llStaminaRemainTime);
	pStatement->SetParam(i++, m_szDBID, sizeof(m_szDBID));

	return TRUE;
	}


BOOL AgsdRelay2Item::SetParamDelete(AuStatement* pStatement)
	{
	INT16 i = 0;
	
	if (AGSMRELAY_PARAM_ITEM == m_nParam || AGSMRELAY_PARAM_CASHITEM == m_nParam)
		{
		pStatement->SetParam(0, m_szDBID, sizeof(m_szDBID));
		pStatement->SetParam(1, m_szDeleteReason, sizeof(m_szDeleteReason));
		}
	else if (AGSMRELAY_PARAM_GUILD_WAREHOUSE_ITEM == m_nParam)
		{
		pStatement->SetParam(0, m_szDBID, sizeof(m_szDBID));
		}
	else
		return FALSE;
		
	return TRUE;
	}


void AgsdRelay2Item::Dump(CHAR *pszOp)
	{
	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);

	sprintf(szFile, "LOG\\%04d%02d%02d-%02d-RELAY_DUMP_ITEM.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d:%02d:%02d][%s] "
						"ACC[%s] CHAR[%s] SEQ[%s] TID[%d] Stack[%d] Status[%d] Pos[%s] DR[%s] Skill[%d]\n",
						st.wHour, st.wMinute, st.wSecond, pszOp,
						m_szAccount, m_szCharName, m_szDBID, m_lTemplateID, m_lStackCount,
						m_nStatus, m_szPosition, m_szDeleteReason, m_lStatusFlag);
	AuLogFile_s(szFile, strCharBuff);
	}
