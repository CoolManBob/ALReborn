/*=============================================================================

	AgsmRelay2MailBox.cpp

=============================================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"


//
//	==========		MailBox		==========
//
void AgsmRelay2::InitPacketMail()
	{
	m_csPacketMail.SetFlagLength(sizeof(INT32));
	m_csPacketMail.SetFieldType(AUTYPE_INT16,			1,									// eAgsmRelay2Operation
								AUTYPE_INT32,			1,									// CID
								AUTYPE_INT32,			1,									// result
								AUTYPE_PACKET,			1,									// packet
								AUTYPE_INT32,			1,									// mail id
								AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,			// from charid
								AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,			// to charid
								AUTYPE_UINT64,			1,									// mail db id
								AUTYPE_UINT32,			1,									// flag
								AUTYPE_CHAR,			_MAX_DATETIME_LENGTH + 1,			// date written
								AUTYPE_INT32,			1,									// item id
								AUTYPE_INT32,			1,									// item tid
								AUTYPE_INT16,			1,									// item qty
								AUTYPE_UINT64,			1,									// item seq.
								AUTYPE_CHAR,			AGPMMAILBOX_MAX_SUBJECT_LENGTH +1,	// subject
								AUTYPE_MEMORY_BLOCK,	1,									// content
								AUTYPE_END,				0
								);

	m_csPacketMailItem.SetFlagLength(sizeof(INT32));
	m_csPacketMailItem.SetFieldType(AUTYPE_INT16,			1,									// eAgsmRelay2Operation
									AUTYPE_INT32,			1,									// CID
									AUTYPE_INT32,			1,									// mail id
									AUTYPE_INT32,			1,									// result
									AUTYPE_UINT64,			1,									// mail DB ID
									AUTYPE_UINT64,			1,									// item seq.
									AUTYPE_INT32,			1,									// item tid
									AUTYPE_INT32,			1,									// item qty.
									AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,			// char id
									AUTYPE_INT32,			1,									// need level
									AUTYPE_CHAR,			_MAX_CONVERT_HISTORY_LENGTH + 1,	// convert
									AUTYPE_INT32,			1,									// durability
									AUTYPE_INT32,			1,									// max durability
									AUTYPE_INT32,			1,									// flag
									AUTYPE_CHAR,			_MAX_OPTION_LENGTH + 1,				// option
									AUTYPE_CHAR,			_MAX_OPTION_LENGTH + 1,				// skill plus
									AUTYPE_INT32,			1,									// in use
									AUTYPE_INT32,			1,									// use count
									AUTYPE_INT64,			1,									// remain time
									AUTYPE_CHAR,			_MAX_DATETIME_LENGTH + 1,			// expire date
									AUTYPE_INT64,			1,									// stamina remain time
									AUTYPE_END,				0
									);
	}


BOOL AgsmRelay2::OnParamMail(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Mail *pAgsdRelay2Mail = new AgsdRelay2Mail;

	CHAR *pszFromCharID= NULL;
	CHAR *pszToCharID	= NULL;
	CHAR *pszSubject	= NULL;
	CHAR *pszDate		= NULL;
	PVOID pvContent		= NULL;
	INT16 nContentSize	= 0;

	m_csPacketMail.GetField(FALSE, pvPacket, 0,
							&pAgsdRelay2Mail->m_eOperation,
							&pAgsdRelay2Mail->m_lCID,
							&pAgsdRelay2Mail->m_lResult,
							&pAgsdRelay2Mail->m_pvPacketEmb,
							&pAgsdRelay2Mail->m_AgpdMail.m_lID,
							&pszFromCharID,
							&pszToCharID,
							&pAgsdRelay2Mail->m_AgpdMail.m_ullDBID,
							&pAgsdRelay2Mail->m_AgpdMail.m_ulFlag,
							&pszDate,
							&pAgsdRelay2Mail->m_lItemID,
							&pAgsdRelay2Mail->m_AgpdMail.m_lItemTID,
							&pAgsdRelay2Mail->m_AgpdMail.m_nItemQty,
							&pAgsdRelay2Mail->m_AgpdMail.m_ullItemSeq,
							&pszSubject,
							&pvContent,
							&nContentSize
							);

	pAgsdRelay2Mail->m_ulNID = ulNID;

	_tcscpy(pAgsdRelay2Mail->m_AgpdMail.m_szFromCharID, pszFromCharID ? pszFromCharID : _T(""));
	_tcscpy(pAgsdRelay2Mail->m_AgpdMail.m_szToCharID, pszToCharID ? pszToCharID : _T(""));
	_tcscpy(pAgsdRelay2Mail->m_AgpdMail.m_szSubject, pszSubject ? pszSubject : _T(""));
	pAgsdRelay2Mail->m_AgpdMail.SetContent(pvContent, nContentSize);
	_tcscpy(pAgsdRelay2Mail->m_szDate, pszDate ? pszDate : _T(""));

	_i64tot(pAgsdRelay2Mail->m_AgpdMail.m_ullDBID, pAgsdRelay2Mail->m_szDBID, 10);
	_i64tot(pAgsdRelay2Mail->m_AgpdMail.m_ullItemSeq, pAgsdRelay2Mail->m_szItemSeq, 10);

	return EnumCallback(AGSMRELAY_PARAM_MAIL, (PVOID)pAgsdRelay2Mail, (PVOID)nParam);
	}


BOOL AgsmRelay2::OnParamMailItem(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2MailItem *pAgsdRelay2MailItem = new AgsdRelay2MailItem;

	CHAR	*pszCharID = NULL;
	CHAR	*pszConvert = NULL;
	CHAR	*pszOption = NULL;
	CHAR	*pszSkillPlus = NULL;
	CHAR	*pszExpireDate = NULL;
	
	m_csPacketMailItem.GetField(FALSE, pvPacket, 0,
								&pAgsdRelay2MailItem->m_eOperation,
								&pAgsdRelay2MailItem->m_lCID,
								&pAgsdRelay2MailItem->m_lMailID,
								&pAgsdRelay2MailItem->m_lResult,
								&pAgsdRelay2MailItem->m_ullDBID,
								&pAgsdRelay2MailItem->m_ullItemSeq,
								&pAgsdRelay2MailItem->m_lItemTID,
								&pAgsdRelay2MailItem->m_lItemQty,
								&pszCharID,
								&pAgsdRelay2MailItem->m_lNeedLevel,
								&pszConvert,
								&pAgsdRelay2MailItem->m_lDurability,
								&pAgsdRelay2MailItem->m_lMaxDurability,
								&pAgsdRelay2MailItem->m_lFlag,
								&pszOption,
								&pszSkillPlus,
								&pAgsdRelay2MailItem->m_lInUse,
								&pAgsdRelay2MailItem->m_lUseCount,
								&pAgsdRelay2MailItem->m_lRemainTime,
								&pszExpireDate,
								&pAgsdRelay2MailItem->m_llStaminaRemainTime
								);

	_tcscpy(pAgsdRelay2MailItem->m_szCharID, pszCharID ? pszCharID : _T(""));
	_tcscpy(pAgsdRelay2MailItem->m_szConvert, pszConvert ? pszConvert : _T(""));
	_tcscpy(pAgsdRelay2MailItem->m_szOption, pszOption ? pszOption : _T(""));
	_tcscpy(pAgsdRelay2MailItem->m_szSkillPlus, pszSkillPlus ? pszSkillPlus : _T(""));
	_tcscpy(pAgsdRelay2MailItem->m_szExpireDate, pszExpireDate ? pszExpireDate : _T(""));

	pAgsdRelay2MailItem->m_ulNID = ulNID;

	_i64tot(pAgsdRelay2MailItem->m_ullDBID, pAgsdRelay2MailItem->m_szDBID, 10);
	_i64tot(pAgsdRelay2MailItem->m_ullItemSeq, pAgsdRelay2MailItem->m_szItemSeq, 10);

	return EnumCallback(AGSMRELAY_PARAM_MAIL_ITEM, (PVOID)pAgsdRelay2MailItem, (PVOID)nParam);	
	}


BOOL AgsmRelay2::CBMailInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(NULL != pData);
	ASSERT(NULL != pClass);
	ASSERT(NULL != pCustData);

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	PVOID *ppvBuffer = (PVOID *) pData;
	AgpdMail *pAgpdMail = (AgpdMail *) ppvBuffer[0];
	INT32 lItemID = ((INT32 *) ppvBuffer[1]) ? *((UINT32 *) ppvBuffer[1]) : 0;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;

	AgsdRelay2Mail csMail;

	csMail.m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	csMail.m_lCID = pAgpdCharacter->m_lID;
	csMail.m_AgpdMail = *pAgpdMail;
	csMail.m_lItemID = lItemID;
	AuTimeStamp::ConvertTimeStampToOracleTime((UINT32)csMail.m_AgpdMail.m_lDate, csMail.m_szDate, sizeof(csMail.m_szDate));
/*
	// 편지를 보내기 전에 최종으로 첨부될 아이템을 업데이트 한다.
	if (lItemID != 0)
	{
		AgpdItem* pAgpdItem = pThis->m_pAgpmItem->GetItem(lItemID);
		if(pAgpdItem != NULL)
		{
			pThis->SendItemUpdate(pAgpdItem, pAgpdMail->m_ullItemSeq);
		}
	}
*/	
	return pThis->SendMailContent(&csMail);
	}


BOOL AgsmRelay2::CBMailUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(NULL != pData);
	ASSERT(NULL != pClass);

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdMail *pAgpdMail = (AgpdMail *) pData;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;
		
	AgsdRelay2Mail csMail;

	csMail.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;
	csMail.m_lCID = pAgpdCharacter ? pAgpdCharacter->m_lID : AP_INVALID_CID;
	csMail.m_AgpdMail.m_ullDBID = pAgpdMail->m_ullDBID;
	csMail.m_AgpdMail.m_ulFlag = pAgpdMail->m_ulFlag;

	return pThis->SendMail(&csMail);
	}


BOOL AgsmRelay2::CBMailDelete(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(NULL != pData);
	ASSERT(NULL != pClass);

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdMail *pAgpdMail = (AgpdMail *) pData;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;
		
	AgsdRelay2Mail csMail;

	csMail.m_eOperation = AGSMDATABASE_OPERATION_DELETE;
	csMail.m_lCID = pAgpdCharacter ? pAgpdCharacter->m_lID : AP_INVALID_CID;
	csMail.m_AgpdMail.m_ullDBID = pAgpdMail->m_ullDBID;
	csMail.m_AgpdMail.m_ulFlag = pAgpdMail->m_ulFlag;

	return pThis->SendMail(&csMail);
	}


BOOL AgsmRelay2::CBMailSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(NULL != pCustData);
	ASSERT(NULL != pClass);

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;
		
	AgsdRelay2Mail csMail;

	csMail.m_eOperation = AGSMDATABASE_OPERATION_SELECT;
	csMail.m_lCID = pAgpdCharacter->m_lID;
	_tcscpy(csMail.m_AgpdMail.m_szToCharID, pAgpdCharacter->m_szID);

	return pThis->SendMail(&csMail);
	}


BOOL AgsmRelay2::CBMailItemSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdMail *pAgpdMail = (AgpdMail *) pData;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	AgsdRelay2MailItem csMailItem;
	
	csMailItem.m_eOperation = AGSMDATABASE_OPERATION_SELECT;
	csMailItem.m_lCID = pAgpdCharacter->m_lID;
	csMailItem.m_lMailID = pAgpdMail->m_lID;
	csMailItem.m_ullDBID = pAgpdMail->m_ullDBID;
	csMailItem.m_ullItemSeq = pAgpdMail->m_ullItemSeq;
	csMailItem.m_lItemTID = pAgpdMail->m_lItemTID;
	csMailItem.m_lItemQty = pAgpdMail->m_nItemQty;
	_tcscpy(csMailItem.m_szCharID, pAgpdCharacter->m_szID);
	
	return pThis->SendMailItem(&csMailItem);
	}


BOOL AgsmRelay2::CBOperationMail(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationMail,
						AgsmRelay2::CBFailOperation,
						pThis,
						pAgsdRelay2
						);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBOperationMailItem(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationMailItem,
						AgsmRelay2::CBFailOperation,
						pThis,
						pAgsdRelay2
						);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBFinishOperationMail(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT:
			pThis->OnSelectResultMail(pRowset, pAgsdRelay2);
			break;
		
		case AGSMDATABASE_OPERATION_INSERT:
			pThis->OnInsertResultMail(pAgsdRelay2);
			break;
		
		case AGSMDATABASE_OPERATION_UPDATE:
		case AGSMDATABASE_OPERATION_DELETE:
			break;
		}

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBFinishOperationMailItem(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT:
			pThis->OnSelectResultMailItem(pAgsdRelay2);
			break;
		
		case AGSMDATABASE_OPERATION_INSERT:
		case AGSMDATABASE_OPERATION_UPDATE:
		case AGSMDATABASE_OPERATION_DELETE:
			break;
		}

	pAgsdRelay2->Release();

	return TRUE;	
	}


BOOL AgsmRelay2::OnSelectResultMail(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2Mail *pAgsdRelay2Mail = static_cast<AgsdRelay2Mail *>(pAgsdRelay2);

	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2Mail->m_ulNID);
	if (!pGameServer)
		return FALSE;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	INT32 lTotalSize = pRowset->GetRowBufferSize() * pRowset->GetRowCount();
	INT32 lTotalStep = 1 + (INT32) (lTotalSize / 20000);
	pAgsdRelay2Mail->m_nStatus = (INT16) lTotalStep;
	INT32 lRowsPerStep = 20000 / pRowset->GetRowBufferSize();
	for (INT32 lStep = 0; lStep < lTotalStep; lStep++)
		{
		PVOID pvPacket = NULL;
		pAgsdRelay2Mail->m_pvPacketEmb = MakeRowsetPacket2(pRowset, lStep, lRowsPerStep);

		BOOL bResult = SendMail(pAgsdRelay2Mail, pGameServer->m_dpnidServer);

		printf("\nMail of CID[%d] sended[%d/%d][%s]\n", pAgsdRelay2Mail->m_lCID,
										lStep + 1, lTotalStep,  bResult ? _T("TRUE") : _T("FALSE"));
		}

	// last
	pAgsdRelay2Mail->m_pvPacketEmb = NULL;
	pAgsdRelay2Mail->m_lResult = AGPMMAILBOX_RESULT_SUCCESS;

	return SendMail(pAgsdRelay2Mail, pGameServer->m_dpnidServer);
	}


BOOL AgsmRelay2::OnInsertResultMail(AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2Mail *pAgsdRelay2Mail = static_cast<AgsdRelay2Mail *>(pAgsdRelay2);
	
	pAgsdRelay2Mail->m_AgpdMail.m_ullDBID = _ttoi64(pAgsdRelay2Mail->m_szDBID);
	
	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2Mail->m_ulNID);
	if (!pGameServer)
		return FALSE;

	return SendMail(pAgsdRelay2Mail, pGameServer->m_dpnidServer);
	}


BOOL AgsmRelay2::OnSelectResultMailItem(AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2MailItem *pAgsdRelay2MailItem = static_cast<AgsdRelay2MailItem *>(pAgsdRelay2);
	
	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2MailItem->m_ulNID);
	if (!pGameServer)
		return FALSE;

	return SendMailItem(pAgsdRelay2MailItem, pGameServer->m_dpnidServer);
	}




//	After OnParamMail callback(from relay to game)
//=========================================================
//
BOOL AgsmRelay2::CBOperationResultMail(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = static_cast<AgsmRelay2*> (pClass);
	AgsdRelay2Mail *pAgsdRelay2Mail = static_cast<AgsdRelay2Mail *> (pData);
	
	AgsdRelay2AutoPtr Auto(pAgsdRelay2Mail);

	AgpdCharacter *pAgpdCharacter = pThis->m_pAgpmCharacter->GetCharacter(pAgsdRelay2Mail->m_lCID);
	if (NULL == pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;

	switch (pAgsdRelay2Mail->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT:
			{
			stBuddyRowset Rowset;
			if (NULL == pAgsdRelay2Mail->m_pvPacketEmb && AGPMMAILBOX_RESULT_SUCCESS == pAgsdRelay2Mail->m_lResult)
				{
				if (FALSE == pThis->m_pAgsmMailBox->OnSelectResult(pAgpdCharacter, NULL, TRUE))
					return FALSE;
				}
			
			if (FALSE == pThis->ParseBuddyRowsetPacket(pAgsdRelay2Mail->m_pvPacketEmb, &Rowset))
				return FALSE;

			if (FALSE == pThis->m_pAgsmMailBox->OnSelectResult(pAgpdCharacter, &Rowset))
				return FALSE;
			}
			break;
			
		case AGSMDATABASE_OPERATION_INSERT:
			{
			if (FALSE == pThis->m_pAgsmMailBox->OnWriteResult(pAgpdCharacter, pAgsdRelay2Mail->m_AgpdMail.m_lID, pAgsdRelay2Mail->m_AgpdMail.m_ullDBID, pAgsdRelay2Mail->m_lItemID, pAgsdRelay2Mail->m_lResult))
				return FALSE;
			}
			break;

		case AGSMDATABASE_OPERATION_UPDATE:
		case AGSMDATABASE_OPERATION_DELETE:
			break;
		}
	
	return TRUE;
	}


BOOL AgsmRelay2::CBOperationResultMailItem(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = static_cast<AgsmRelay2*> (pClass);
	AgsdRelay2MailItem *pAgsdRelay2MailItem = static_cast<AgsdRelay2MailItem *> (pData);
	
	AgsdRelay2AutoPtr Auto(pAgsdRelay2MailItem);

	AgpdCharacter *pAgpdCharacter = pThis->m_pAgpmCharacter->GetCharacter(pAgsdRelay2MailItem->m_lCID);
	if (NULL == pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;

	switch (pAgsdRelay2MailItem->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT:
			{
			if (FALSE == pThis->m_pAgsmMailBox->OnItemSaveResult(pAgpdCharacter,
																pAgsdRelay2MailItem->m_lMailID,
																pAgsdRelay2MailItem->m_ullItemSeq,
																pAgsdRelay2MailItem->m_lItemTID,
																pAgsdRelay2MailItem->m_lItemQty,
																pAgsdRelay2MailItem->m_nStatus,
																pAgsdRelay2MailItem->m_lNeedLevel,
																pAgsdRelay2MailItem->m_szConvert,
																pAgsdRelay2MailItem->m_lDurability,
																pAgsdRelay2MailItem->m_lMaxDurability,
																pAgsdRelay2MailItem->m_lFlag,
																pAgsdRelay2MailItem->m_szOption,
																pAgsdRelay2MailItem->m_szSkillPlus,
																pAgsdRelay2MailItem->m_lInUse,
																pAgsdRelay2MailItem->m_lUseCount,
																pAgsdRelay2MailItem->m_lRemainTime,
																pAgsdRelay2MailItem->m_szExpireDate,
																pAgsdRelay2MailItem->m_llStaminaRemainTime,
																pAgsdRelay2MailItem->m_lResult
																)
				)
				return FALSE;
			}
			break;
			
		case AGSMDATABASE_OPERATION_INSERT:
		case AGSMDATABASE_OPERATION_UPDATE:
		case AGSMDATABASE_OPERATION_DELETE:
			break;
		}

	return TRUE;
	}


BOOL AgsmRelay2::SendMail(AgsdRelay2Mail *pAgsdRelay2Mail, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketMail = m_csPacketMail.MakePacket(FALSE, &nPacketLength, 0,
												&pAgsdRelay2Mail->m_eOperation,
												&pAgsdRelay2Mail->m_lCID,
												&pAgsdRelay2Mail->m_lResult,
												pAgsdRelay2Mail->m_pvPacketEmb,
												&pAgsdRelay2Mail->m_AgpdMail.m_lID,
												pAgsdRelay2Mail->m_AgpdMail.m_szFromCharID,
												pAgsdRelay2Mail->m_AgpdMail.m_szToCharID,
												&pAgsdRelay2Mail->m_AgpdMail.m_ullDBID,
												&pAgsdRelay2Mail->m_AgpdMail.m_ulFlag,
												pAgsdRelay2Mail->m_szDate,
												&pAgsdRelay2Mail->m_lItemID,
												NULL,				// item tid
												NULL,				// item qty
												NULL,				// item seq.
												NULL,				// subject
												NULL				// content
												);
	if (!pvPacketMail)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketMail, AGSMRELAY_PARAM_MAIL);
	else
		bResult = MakeAndSendRelayPacket(pvPacketMail, AGSMRELAY_PARAM_MAIL, ulNID);

	m_csPacketMail.FreePacket(pvPacketMail);
	
	return bResult;
	}


BOOL AgsmRelay2::SendMailContent(AgsdRelay2Mail *pAgsdRelay2Mail, UINT32 ulNID)
	{
	PVOID pvContent = pAgsdRelay2Mail->m_AgpdMail.GetContent();
	INT16 nContentSize = pAgsdRelay2Mail->m_AgpdMail.GetContentSize();
	if (NULL == pvContent || 0 > nContentSize)
		return FALSE;

	INT16 nPacketLength	= 0;
	PVOID pvPacketMail = m_csPacketMail.MakePacket(FALSE, &nPacketLength, 0,
												&pAgsdRelay2Mail->m_eOperation,
												&pAgsdRelay2Mail->m_lCID,
												&pAgsdRelay2Mail->m_lResult,
												pAgsdRelay2Mail->m_pvPacketEmb,
												&pAgsdRelay2Mail->m_AgpdMail.m_lID,
												pAgsdRelay2Mail->m_AgpdMail.m_szFromCharID,
												pAgsdRelay2Mail->m_AgpdMail.m_szToCharID,
												&pAgsdRelay2Mail->m_AgpdMail.m_ullDBID,
												&pAgsdRelay2Mail->m_AgpdMail.m_ulFlag,
												pAgsdRelay2Mail->m_szDate,
												&pAgsdRelay2Mail->m_lItemID,
												&pAgsdRelay2Mail->m_AgpdMail.m_lItemTID,
												&pAgsdRelay2Mail->m_AgpdMail.m_nItemQty,
												&pAgsdRelay2Mail->m_AgpdMail.m_ullItemSeq,
												pAgsdRelay2Mail->m_AgpdMail.m_szSubject,
												pvContent,		// content
												&nContentSize	// content size
												);
	if (!pvPacketMail)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketMail, AGSMRELAY_PARAM_MAIL);
	else
		bResult = MakeAndSendRelayPacket(pvPacketMail, AGSMRELAY_PARAM_MAIL, ulNID);

	m_csPacketMail.FreePacket(pvPacketMail);
	
	return bResult;
	}


BOOL AgsmRelay2::SendMailItem(AgsdRelay2MailItem *pAgsdRelay2MailItem, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketMailItem = m_csPacketMailItem.MakePacket(FALSE, &nPacketLength, 0,
													&pAgsdRelay2MailItem->m_eOperation,
													&pAgsdRelay2MailItem->m_lCID,
													&pAgsdRelay2MailItem->m_lMailID,
													&pAgsdRelay2MailItem->m_lResult,
													&pAgsdRelay2MailItem->m_ullDBID,
													&pAgsdRelay2MailItem->m_ullItemSeq,
													&pAgsdRelay2MailItem->m_lItemTID,
													&pAgsdRelay2MailItem->m_lItemQty,
													pAgsdRelay2MailItem->m_szCharID,
													&pAgsdRelay2MailItem->m_lNeedLevel,
													pAgsdRelay2MailItem->m_szConvert,
													&pAgsdRelay2MailItem->m_lDurability,
													&pAgsdRelay2MailItem->m_lMaxDurability,
													&pAgsdRelay2MailItem->m_lFlag,
													pAgsdRelay2MailItem->m_szOption,
													pAgsdRelay2MailItem->m_szSkillPlus,
													&pAgsdRelay2MailItem->m_lInUse,
													&pAgsdRelay2MailItem->m_lUseCount,
													&pAgsdRelay2MailItem->m_lRemainTime,
													pAgsdRelay2MailItem->m_szExpireDate,
													&pAgsdRelay2MailItem->m_llStaminaRemainTime
													);
	if (!pvPacketMailItem)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketMailItem, AGSMRELAY_PARAM_MAIL_ITEM);
	else
		bResult = MakeAndSendRelayPacket(pvPacketMailItem, AGSMRELAY_PARAM_MAIL_ITEM, ulNID);

	m_csPacketMailItem.FreePacket(pvPacketMailItem);
	
	return bResult;
	}




/********************************************************/
/*		The Implementation of AgsdRelay2Mail class		*/
/********************************************************/
//
AgsdRelay2Mail::AgsdRelay2Mail()
	{
	m_lCID = 0;	
	m_pvPacketEmb = NULL;
	m_lItemID = 0;

	
	// dummy
	ZeroMemory(m_szDate, sizeof(m_szDate));
	ZeroMemory(m_szDBID, sizeof(m_szDBID));
	ZeroMemory(m_szItemSeq, sizeof(m_szItemSeq));
	m_nStatus = AGPDITEM_STATUS_MAILBOX;
	
	// result
	m_lResult = AGPMMAILBOX_RESULT_NONE;
	}


BOOL AgsdRelay2Mail::SetParamSelect(AuStatement *pStatement)
	{
	pStatement->SetParam(0, m_AgpdMail.m_szToCharID, sizeof(m_AgpdMail.m_szToCharID));

	return TRUE;
	}


BOOL AgsdRelay2Mail::SetParamUpdate(AuStatement* pStatement)
	{
	pStatement->SetParam(0, &m_AgpdMail.m_ulFlag);
	pStatement->SetParam(1, m_szDBID, sizeof(m_szDBID));
	
	return TRUE;
	}


BOOL AgsdRelay2Mail::SetParamInsert(AuStatement* pStatement)
	{
	INT16 i = 0;

	// in	
	pStatement->SetParam(i++, m_AgpdMail.m_szToCharID, sizeof(m_AgpdMail.m_szToCharID));
	pStatement->SetParam(i++, m_AgpdMail.m_szFromCharID, sizeof(m_AgpdMail.m_szFromCharID));
	pStatement->SetParam(i++, m_AgpdMail.m_szSubject, sizeof(m_AgpdMail.m_szSubject));
	pStatement->SetParam(i++, m_szDate, sizeof(m_szDate));
	pStatement->SetParam(i++, &m_AgpdMail.m_ulFlag);
	pStatement->SetParam(i++, &m_AgpdMail.m_lItemTID);
	pStatement->SetParam(i++, &m_AgpdMail.m_nItemQty);
	pStatement->SetParam(i++, m_szItemSeq, sizeof(m_szItemSeq));
	pStatement->SetParam(i++, &m_nStatus);
	pStatement->SetParam(i++, m_AgpdMail.GetContent(), m_AgpdMail.GetContentSize());

	// out
	pStatement->SetParam(i++, m_szDBID, sizeof(m_szDBID), TRUE);
	pStatement->SetParam(i++, &m_lResult, TRUE);

	return TRUE;
	}


BOOL AgsdRelay2Mail::SetParamDelete(AuStatement* pStatement)
	{
	pStatement->SetParam(0, m_szDBID, sizeof(m_szDBID));
	pStatement->SetParam(1, &m_AgpdMail.m_ulFlag);

	return TRUE;
	}


void AgsdRelay2Mail::Dump(CHAR *pszOp)
	{
	/*
	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "%04d%02d%02d-%02d-RELAY_DUMP_MAIL.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	AuLogFile(szFile, "[%02d:%02d:%02d][%s]"
						"ID[%s]\n",
				st.wHour, st.wMinute, st.wSecond,
				pszOp, "m_szCharID"
				);
	*/
	}




/************************************************************/
/*		The Implementation of AgsdRelay2MailItem class		*/
/************************************************************/
//
AgsdRelay2MailItem::AgsdRelay2MailItem()
	{
	m_lCID = 0;
	m_lMailID = 0;

	// in
	m_ullDBID = 0;
	m_ullItemSeq = 0;
	m_lItemTID = 0;
	m_lItemQty = 0;
	ZeroMemory(m_szCharID, sizeof(m_szCharID));
	m_nStatus= AGPDITEM_STATUS_MAILBOX;
		
	// out
	m_lNeedLevel = 0;
	ZeroMemory(m_szConvert, sizeof(m_szConvert));
	m_lDurability = 0;
	m_lMaxDurability = 0;
	m_lFlag = 0;
	ZeroMemory(m_szOption, sizeof(m_szOption));
	ZeroMemory(m_szSkillPlus, sizeof(m_szSkillPlus));
	m_lInUse = 0;
	m_lUseCount = 0;
	m_lRemainTime = 0;
	ZeroMemory(m_szExpireDate, sizeof(m_szExpireDate));
	m_llStaminaRemainTime = 0;
		
	// dummy
	ZeroMemory(m_szDBID, sizeof(m_szDBID));
	ZeroMemory(m_szItemSeq, sizeof(m_szItemSeq));

	// result
	m_lResult = AGPMMAILBOX_RESULT_NONE;
	}


BOOL AgsdRelay2MailItem::SetParamSelect(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, m_szDBID, sizeof(m_szDBID));
	pStatement->SetParam(i++, m_szItemSeq, sizeof(m_szItemSeq));
	pStatement->SetParam(i++, &m_lItemTID);
	pStatement->SetParam(i++, &m_lItemQty);
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	pStatement->SetParam(i++, &m_nStatus);

	// out
	pStatement->SetParam(i++, &m_lNeedLevel, TRUE);
	pStatement->SetParam(i++, m_szConvert, sizeof(m_szConvert), TRUE);
	pStatement->SetParam(i++, &m_lDurability, TRUE);
	pStatement->SetParam(i++, &m_lMaxDurability, TRUE);
	pStatement->SetParam(i++, &m_lFlag, TRUE);
	pStatement->SetParam(i++, m_szOption, sizeof(m_szOption), TRUE);
	pStatement->SetParam(i++, m_szSkillPlus, sizeof(m_szSkillPlus), TRUE);
	pStatement->SetParam(i++, &m_lInUse, TRUE);
	pStatement->SetParam(i++, &m_lUseCount, TRUE);
	pStatement->SetParam(i++, (UINT32*)&m_lRemainTime, TRUE);
	pStatement->SetParam(i++, m_szExpireDate, sizeof(m_szExpireDate), TRUE);
	pStatement->SetParam(i++, (UINT32*)&m_llStaminaRemainTime, TRUE);

	pStatement->SetParam(i++, &m_lResult, TRUE);

	return TRUE;
	}


void AgsdRelay2MailItem::Dump(CHAR *pszOp)
	{
	/*
	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "%04d%02d%02d-%02d-RELAY_DUMP_MAILITEM.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	AuLogFile(szFile, "[%02d:%02d:%02d][%s]"
						"ID[%s]\n",
				st.wHour, st.wMinute, st.wSecond,
				pszOp, "m_szCharID"
				);
	*/
	}

