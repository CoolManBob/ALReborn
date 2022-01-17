/*=============================================================================

	AgsmMailBox.cpp

=============================================================================*/


#include "AgsmMailBox.h"


#define AGSMMAILBOX_LOG_FILENAME		_T("LOG\\AgsmMailBoxLog_%04d%02d%02d.log")


/****************************************************/
/*		The Implementation of AgsmMailBox class		*/
/****************************************************/
//
AgsmMailBox::AgsmMailBox()
	{
	SetModuleName(_T("AgsmMailBox"));

	m_pAgpmLog = NULL;
	}


AgsmMailBox::~AgsmMailBox()
	{
	}




//	ApModule inherited
//============================================
//
BOOL AgsmMailBox::OnAddModule()
	{
	m_pAgpmGrid	= (AgpmGrid *) GetModule(_T("AgpmGrid"));
	m_pApmMap = (ApmMap *) GetModule(_T("ApmMap"));
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmFactors = (AgpmFactors *) GetModule(_T("AgpmFactors"));
	m_pAgpmItem = (AgpmItem *) GetModule(_T("AgpmItem"));
	m_pAgpmItemConvert = (AgpmItemConvert *) GetModule(_T("AgpmItemConvert"));
	m_pAgpmMailBox = (AgpmMailBox *)GetModule(_T("AgpmMailBox"));
	m_pAgsmCharacter = (AgsmCharacter *)GetModule(_T("AgsmCharacter"));
	m_pAgsmCharManager = (AgsmCharManager *)GetModule(_T("AgsmCharManager"));
	m_pAgsmItem = (AgsmItem *) GetModule(_T("AgsmItem"));
	m_pAgsmItemManager = (AgsmItemManager *) GetModule(_T("AgsmItemManager"));
	m_pAgsmServerManager = (AgsmServerManager *) GetModule(_T("AgsmServerManager2"));
	m_pAgsmInterServerLink = (AgsmInterServerLink *) GetModule(_T("AgsmInterServerLink"));
	m_pAgsmReturnToLogin = (AgsmReturnToLogin *) GetModule(_T("AgsmReturnToLogin"));
	m_pAgpmLog = (AgpmLog *) GetModule(_T("AgpmLog"));

	if (!m_pApmMap || !m_pAgpmCharacter || !m_pAgpmFactors || !m_pAgpmItem ||
		!m_pAgpmItemConvert || !m_pAgpmMailBox || !m_pAgsmCharacter || !m_pAgsmCharManager ||
		!m_pAgsmItem ||	!m_pAgsmItemManager || !m_pAgsmServerManager || !m_pAgsmInterServerLink ||
		! m_pAgsmReturnToLogin
		)
		return FALSE;


	// login process
	if (!m_pAgsmCharManager->SetCallbackConnectedChar(CBEnterGame, this))
		return FALSE;
	if (!m_pAgsmCharacter->SetCallbackSendCharacterAllInfo(CBSendCharacterAllInfo, this))
		return FALSE;

	// logout
	if (!m_pAgpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;
	if (!m_pAgsmReturnToLogin->SetCallbackNotifySaveAll(CBReturnToLogin, this))
		return FALSE;

	// request
	if (!m_pAgpmMailBox->SetCallbackWriteMail(CBWrite, this))
		return FALSE;
	if (!m_pAgpmMailBox->SetCallbackRemoveMail(CBRemove, this))
		return FALSE;		
	if (!m_pAgpmMailBox->SetCallbackReadMail(CBRead, this))
		return FALSE;
	if (!m_pAgpmMailBox->SetCallbackItemSave(CBItemSave, this))
		return FALSE;

	return TRUE;
	}




//	Login process(received mail) related callback
//===================================================
//
BOOL AgsmMailBox::CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmMailBox		*pThis			= (AgsmMailBox *)	pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	
	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return TRUE;

	// send to relay
	return pThis->EnumCallback(AGSMMAILBOX_CB_SELECT_MAIL, NULL, pAgpdCharacter);
	}


BOOL AgsmMailBox::CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmMailBox		*pThis			= (AgsmMailBox *)	pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	// 완전히 로그인이 되지 않은 상태에서 보내는 경우와
	// 메일을 모두 DB에서 읽어오지 못한 상태에서 보내는 것을 방지 하기 위해
	// OnSelectAllMailResult와 CBSendCharacterAllInfo에서 SendAllMail을 부른다.
	// SendAllMail에서 두 조건 충족시에 보내도록 한다.

	return pThis->SendAllMail(pAgpdCharacter, ulNID);
	}




//	Logout callback
//===================================================
//
BOOL AgsmMailBox::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmMailBox		*pThis			= (AgsmMailBox *)	pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return TRUE;

	pThis->m_pAgpmMailBox->RemoveAllMail(pAgpdCharacter, &(pThis->m_GenerateID));

	return TRUE;
	}


BOOL AgsmMailBox::CBReturnToLogin(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmMailBox		*pThis			= (AgsmMailBox *)	pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;

	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return TRUE;

	pThis->m_pAgpmMailBox->RemoveAllMail(pAgpdCharacter, &(pThis->m_GenerateID));
	pThis->SendRemoveAllMail(pAgpdCharacter);

	return TRUE;
	}




//	Request callbacks
//===========================================
//
BOOL AgsmMailBox::CBWrite(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmMailBox		*pThis = (AgsmMailBox *) pClass;;
	AgpdMailArg		*pMailArg = (AgpdMailArg *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->OnWrite(pAgpdCharacter, pMailArg);
	}


BOOL AgsmMailBox::CBRemove(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmMailBox		*pThis = (AgsmMailBox *) pClass;;
	AgpdMailArg		*pMailArg = (AgpdMailArg *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->OnRemove(pAgpdCharacter, pMailArg);
	}


BOOL AgsmMailBox::CBRead(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmMailBox		*pThis = (AgsmMailBox *) pClass;;
	AgpdMailArg		*pMailArg = (AgpdMailArg *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->OnRead(pAgpdCharacter, pMailArg);
	}


BOOL AgsmMailBox::CBItemSave(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmMailBox		*pThis = (AgsmMailBox *) pClass;;
	AgpdMailArg		*pMailArg = (AgpdMailArg *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->OnItemSave(pAgpdCharacter, pMailArg);	
	}




//	Request processing
//=====================================
//
BOOL AgsmMailBox::OnWrite(AgpdCharacter *pAgpdCharacter, AgpdMailArg *pMailArg)
	{
	if (NULL == pAgpdCharacter || NULL == pMailArg || NULL == pMailArg->m_pAgpdMail)
		return FALSE;

	if (m_pAgpmCharacter->IsCombatMode(pAgpdCharacter) ||
		m_pAgpmCharacter->IsInJail(pAgpdCharacter)
		|| (pAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE)
		|| (pAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_ATTACK)
		|| (pAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
		|| (m_pAgpmCharacter->IsActionBlockCondition(pAgpdCharacter))
		)
	{
		return FALSE;
	}

	m_pAgpmCharacter->SetActionBlockTime(pAgpdCharacter, 10000);

	// validation
	if (0 != _tcscmp(pMailArg->m_pAgpdMail->m_szFromCharID, pAgpdCharacter->m_szID)
		|| 0 >= _tcslen(pMailArg->m_pAgpdMail->m_szToCharID)
		|| 0 >= pMailArg->m_pAgpdMail->GetContentSize()
		)
	{
		SendWriteResult(pAgpdCharacter, NULL, AGPMMAILBOX_RESULT_FAIL);

		m_pAgpmCharacter->ClearActionBlockTime(pAgpdCharacter);

		return FALSE;
	}
	
	// check available letter
	INT32 lNormal = 0;
	INT32 lPremium = 0;
	m_pAgpmMailBox->GetLetterCount(pAgpdCharacter, lNormal, lPremium);
	if (0 >= lNormal)
	{
		SendWriteResult(pAgpdCharacter, NULL, AGPMMAILBOX_RESULT_NO_LETTER);

		m_pAgpmCharacter->ClearActionBlockTime(pAgpdCharacter);

		return FALSE;
	}

	// create mail(by copy)
	AgpdMail *pAgpdMail = m_pAgpmMailBox->CreateMail();
	if (NULL == pAgpdMail)
	{
		m_pAgpmCharacter->ClearActionBlockTime(pAgpdCharacter);

		return FALSE;
	}
	
	*pAgpdMail = *(pMailArg->m_pAgpdMail);
	pAgpdMail->m_lID = m_GenerateID.GetID();
	time(&pAgpdMail->m_lDate);
	pAgpdMail->m_ulFlag = AgpdMail::AGPMMAILBOX_MAIL_FLAG_NONE;
	pAgpdMail->m_ullDBID = 0;

	// item
	AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pMailArg->m_lItemID);
	pAgpdMail->m_ullItemSeq = 0;
	INT32 lItemID = 0;
	if (pAgpdItem)
	{
		if (FALSE == m_pAgpmMailBox->IsValidAttachItem(pAgpdItem))
		{
			m_GenerateID.AddRemoveID(pAgpdMail->m_lID);
			m_pAgpmMailBox->DestroyMail(pAgpdMail);
			SendWriteResult(pAgpdCharacter, NULL, AGPMMAILBOX_RESULT_FAIL);

			m_pAgpmCharacter->ClearActionBlockTime(pAgpdCharacter);

			return FALSE;
		}
		
		AgsdItem *pAgsdItem = m_pAgsmItem->GetADItem(pAgpdItem);
		if (NULL == pAgsdItem)
		{
			m_GenerateID.AddRemoveID(pAgpdMail->m_lID);
			m_pAgpmMailBox->DestroyMail(pAgpdMail);
			
			SendWriteResult(pAgpdCharacter, NULL, AGPMMAILBOX_RESULT_FAIL);

			m_pAgpmCharacter->ClearActionBlockTime(pAgpdCharacter);

			return FALSE;
		}
		
		lItemID = pAgpdItem->m_lID;
		pAgpdMail->m_ullItemSeq = pAgsdItem->m_ullDBIID;
		pAgpdMail->m_lItemTID = pAgpdItem->m_pcsItemTemplate->m_lID;
		pAgpdMail->m_nItemQty = pAgpdItem->m_pcsItemTemplate->m_bStackable ? pAgpdItem->m_nCount : 1;
	}

	if (FALSE == m_pAgpmMailBox->AddMail(pAgpdMail))
	{
		m_GenerateID.AddRemoveID(pAgpdMail->m_lID);
		m_pAgpmMailBox->DestroyMail(pAgpdMail);
		SendWriteResult(pAgpdCharacter, NULL, AGPMMAILBOX_RESULT_FAIL);

		m_pAgpmCharacter->ClearActionBlockTime(pAgpdCharacter);

		return FALSE;
	}

	// send to relay
	PVOID pvBuffer[2];
	pvBuffer[0] = pAgpdMail;
	pvBuffer[1] = &lItemID;
	return EnumCallback(AGSMMAILBOX_CB_WRITE_MAIL, pvBuffer, pAgpdCharacter);
	}


BOOL AgsmMailBox::OnRemove(AgpdCharacter *pAgpdCharacter, AgpdMailArg *pMailArg)
	{
	if (!pAgpdCharacter || !pMailArg)
		return FALSE;
	
	// get mail
	INT32 lMailID = pMailArg->m_pAgpdMail->m_lID;
	UINT32 ulFlag = pMailArg->m_pAgpdMail->m_ulFlag;
	AgpdMail *pAgpdMail = m_pAgpmMailBox->GetMail(lMailID);
	if (NULL == pAgpdMail)
		return FALSE;
	
	if (0 != _tcscmp(pAgpdCharacter->m_szID, pAgpdMail->m_szToCharID))
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	
	m_pAgpmLog->WriteLog_ETC(AGPDLOGTYPE_ETC_MAIL_DELETE, 0, &pcsAgsdCharacter->m_strIPAddress[0], pcsAgsdCharacter->m_szAccountID,
							pcsAgsdCharacter->m_szServerName, pAgpdCharacter->m_szID, 
							pAgpdCharacter->m_lID,
							pAgpdMail->m_lItemTID,
							pAgpdMail->m_nItemQty,
							pAgpdMail->m_ullItemSeq,
							0,
							0,
							NULL,
							NULL,
							pAgpdMail->m_ulFlag,
							pAgpdMail->m_szFromCharID);

	m_pAgpmMailBox->RemoveMailFromCAD(pAgpdCharacter, pAgpdMail);

	_RemoveAndDestroyMail(pAgpdMail);
	
	return SendRemoveMail(pAgpdCharacter, lMailID, AGPMMAILBOX_RESULT_SUCCESS);
	}


BOOL AgsmMailBox::OnRead(AgpdCharacter *pAgpdCharacter, AgpdMailArg *pMailArg)
	{
	if (!pAgpdCharacter || !pMailArg || !pMailArg->m_pAgpdMail)
		return FALSE;

	// get mail
	AgpdMail *pAgpdMail = m_pAgpmMailBox->GetMail(pMailArg->m_pAgpdMail->m_lID);
	if (NULL == pAgpdMail)
		return FALSE;

	if (0 != _tcscmp(pAgpdCharacter->m_szID, pAgpdMail->m_szToCharID))
		return FALSE;	
	
	// flag의 변경만 있으니 굳이 realy로 부터 결과를 받지 않아도 되겠다.
	// flag 변경하고 DB Update하라고 포스팅만 하자.
	pAgpdMail->SetRead(TRUE);
		
	EnumCallback(AGSMMAILBOX_CB_UPDATE_MAIL, pAgpdMail, pAgpdCharacter);

	return TRUE;
	}


BOOL AgsmMailBox::OnItemSave(AgpdCharacter *pAgpdCharacter, AgpdMailArg *pMailArg)
	{
	if (!pAgpdCharacter || !pMailArg || !pMailArg->m_pAgpdMail)
		return FALSE;

	// get mail
	AgpdMail *pAgpdMail = m_pAgpmMailBox->GetMail(pMailArg->m_pAgpdMail->m_lID);
	if (NULL == pAgpdMail)
		return FALSE;

	// check mail
	if (0 != _tcscmp(pAgpdCharacter->m_szID, pAgpdMail->m_szToCharID))
		return FALSE;	
	//JK_거래중금지
	if( pAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
		return FALSE;
	
	if (pAgpdMail->GetReadAttach())
		return FALSE;//SendItemSaveResult();
	
	if (0 == pAgpdMail->m_ullDBID)
		return FALSE;//SendItemSaveResult();
	
	// check inven full
	if (_IsFullInventory(pAgpdCharacter))
		return SendItemSaveResult(pAgpdCharacter, pAgpdMail->m_lID, AGPMMAILBOX_RESULT_INVEN_FULL);
	
	EnumCallback(AGSMMAILBOX_CB_ITEM_SAVE, pAgpdMail, pAgpdCharacter);

	return TRUE;
	}




//	Result processing
//==================================================
//
//	Login process(received mail) related
//=====================================================
//
BOOL AgsmMailBox::OnSelectResult(AgpdCharacter *pAgpdCharacter, stBuddyRowset *pRowset, BOOL bEnd)
	{
	if (NULL == pRowset && FALSE == bEnd)
		return FALSE;

	AgpdMailCAD *pAgpdMailCAD = m_pAgpmMailBox->GetCAD(pAgpdCharacter);
	if (NULL == pAgpdMailCAD)
		return FALSE;
	
	if (bEnd)
		return SendAllMail(pAgpdCharacter, _GetCharacterNID(pAgpdCharacter));

	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
		{
		// create mail
		AgpdMail *pAgpdMail = m_pAgpmMailBox->CreateMail();
		if (!pAgpdMail)
			return FALSE;

		UINT32 ulCol = 0;
		CHAR *psz = NULL;

		/*
		SELECT
			MAILID, CHARID, FROMCHARID, SUBJECT,
			CONTENTS, TO_CHAR('YYYY-MM-DD HH24:MI:SS', WDATE), FLAG,
			ITEMSEQ, ITEMTID, ITEMQTY
		FROM MAILBOX WHERE CHARID = :1
		*/

		// fill value from rowset
		psz = pRowset->Get(ul, ulCol++);		// mail id
		if (NULL == psz)
			continue;
		pAgpdMail->m_ullDBID = _ttoi64(psz);
		if (0 == pAgpdMail->m_ullDBID)
			continue;
		
		psz = pRowset->Get(ul, ulCol++);		// to charid
		if (NULL == psz)
			continue;
		_tcscpy(pAgpdMail->m_szToCharID, psz);

		psz = pRowset->Get(ul, ulCol++);		// from charid
		if (NULL == psz)
			continue;
		_tcscpy(pAgpdMail->m_szFromCharID, psz);

		psz = pRowset->Get(ul, ulCol++);		// subject
		if (NULL == psz)
			continue;
		_tcscpy(pAgpdMail->m_szSubject, psz);
		
		psz = pRowset->Get(ul, ulCol++);		// content
		if (NULL == psz)
			continue;
		pAgpdMail->SetContent(psz);

		psz = pRowset->Get(ul, ulCol++);		// wdate
		if (NULL == psz)
			continue;
		pAgpdMail->m_lDate = AuTimeStamp::ConvertOracleTimeToTimeStamp(psz);

		psz = pRowset->Get(ul, ulCol++);		// flag
		if (NULL == psz)
			continue;
		pAgpdMail->m_ulFlag = _ttoi(psz);

		psz = pRowset->Get(ul, ulCol++);		// item seq
		if (NULL == psz)
			continue;
		pAgpdMail->m_ullItemSeq = _ttoi64(psz);

		psz = pRowset->Get(ul, ulCol++);		// item tid
		if (NULL == psz)
			continue;
		pAgpdMail->m_lItemTID = _ttoi(psz);
		
		psz = pRowset->Get(ul, ulCol++);		// item qty
		if (NULL == psz)
			continue;
		pAgpdMail->m_nItemQty = _ttoi(psz);		

		pAgpdMail->m_lID = m_GenerateID.GetID();	// id

		// add to admin and CAD
		if (FALSE == m_pAgpmMailBox->AddMail(pAgpdMail)
			|| FALSE == m_pAgpmMailBox->AddMailToCAD(pAgpdCharacter, pAgpdMail, FALSE)	// add to tail
			)
			{
			CHAR szFile[MAX_PATH];
			SYSTEMTIME st;
			GetLocalTime(&st);
			sprintf(szFile, AGSMMAILBOX_LOG_FILENAME, st.wYear, st.wMonth, st.wDay);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Can't add Mail[%I64d] to CAD[%s] in OnSelectResult()\n", pAgpdMail->m_ullDBID, pAgpdCharacter->m_szID);
			AuLogFile_s(szFile, strCharBuff);

			// remove
			m_pAgpmMailBox->RemoveMail(pAgpdMail);			// admin
			m_GenerateID.AddRemoveID(pAgpdMail->m_lID);		// id reuse
			m_pAgpmMailBox->DestroyMail(pAgpdMail);			// destroy module data
			
			return FALSE;
			}
		}

	return TRUE;
	}


BOOL AgsmMailBox::OnWriteResult(AgpdCharacter *pAgpdCharacter, INT32 lID, UINT64 ullDBID, INT32 lItemID, INT32 lResult)
	{
	if (NULL == pAgpdCharacter)
		return FALSE;

	AgpdMail *pAgpdMail = m_pAgpmMailBox->GetMail(lID);
	if (NULL == pAgpdMail)
		return FALSE;
	
	if (AGPMMAILBOX_RESULT_SUCCESS != lResult)
	{
		// remove
		m_pAgpmMailBox->RemoveMail(pAgpdMail);			// admin
		m_GenerateID.AddRemoveID(pAgpdMail->m_lID);		// id reuse
		m_pAgpmMailBox->DestroyMail(pAgpdMail);			// destroy module data
		
		SendWriteResult(pAgpdCharacter, NULL, lResult);

		m_pAgpmCharacter->ClearActionBlockTime(pAgpdCharacter);

		return FALSE;
	}
	
	// 편지지 하나 지운다.
	m_pAgpmMailBox->SubLetter(pAgpdCharacter);
	
	// 아이템을 지운다. DB에서는 삭제하지 않는다.
	AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(lItemID);
	if (NULL != pAgpdItem)
	{
		m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_MAIL_ATTACH, pAgpdCharacter->m_lID, pAgpdItem, pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1, 0, pAgpdMail->m_szToCharID);
		m_pAgpmItem->RemoveItem(pAgpdItem);
	}

	AgsdCharacter *pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);

	m_pAgpmLog->WriteLog_ETC(AGPDLOGTYPE_ETC_MAIL_WRITE, 0, &pcsAgsdCharacter->m_strIPAddress[0], pcsAgsdCharacter->m_szAccountID,
							pcsAgsdCharacter->m_szServerName, pAgpdCharacter->m_szID, 
							pAgpdCharacter->m_lID,
							pAgpdMail->m_lItemTID,
							pAgpdMail->m_nItemQty,
							pAgpdMail->m_ullItemSeq,
							0,
							0,
							NULL,
							NULL,
							pAgpdMail->m_ulFlag,
							pAgpdMail->m_szToCharID);
	
	// notify to receiver
	AgpdCharacter *pAgpdCharacterRecv = m_pAgpmCharacter->GetCharacter(pAgpdMail->m_szToCharID);
	if (pAgpdCharacterRecv)
		{
		pAgpdMail->m_ullDBID = ullDBID;
		INT32 lRemovedID = 0;
		if (m_pAgpmMailBox->AddMailToCAD(pAgpdCharacterRecv, pAgpdMail, TRUE, &lRemovedID))		// add to head
			{
			// send add mail
			SendAddMail(pAgpdCharacterRecv, pAgpdMail, _GetCharacterNID(pAgpdCharacterRecv));
			if (0 != lRemovedID)
				m_GenerateID.AddRemoveID(lRemovedID);
			}
		else
			{
			// 이미 DB에 들어간 상황이므로 나중에라도 확인 가능하니 서버에서만 없애자.
			// remove
			m_pAgpmMailBox->RemoveMail(pAgpdMail);			// admin
			m_GenerateID.AddRemoveID(pAgpdMail->m_lID);		// id reuse
			m_pAgpmMailBox->DestroyMail(pAgpdMail);			// destroy module data
			}
		}
	else
		{
		// remove
		m_pAgpmMailBox->RemoveMail(pAgpdMail);			// admin
		m_GenerateID.AddRemoveID(pAgpdMail->m_lID);		// id reuse
		m_pAgpmMailBox->DestroyMail(pAgpdMail);			// destroy module data		
		}
	
	// send result to sender
	SendWriteResult(pAgpdCharacter, pAgpdMail, AGPMMAILBOX_RESULT_SUCCESS);

	m_pAgpmCharacter->ClearActionBlockTime(pAgpdCharacter);

	return TRUE;
	}


BOOL AgsmMailBox::OnItemSaveResult(AgpdCharacter *pAgpdCharacter, INT32 lMailID, UINT64 ullItemSeq,
								INT32 lItemTID, INT32 lItemQty, INT16 nStatus, INT32 lNeedLevel,
								CHAR *pszConvert, INT32 lDurability, INT32 lMaxDurability, INT32 lFlag,
								CHAR *pszOption, CHAR *pszSkillPlus, INT32 lInUse, INT32 lUseCount,
								INT64 lRemainTime, CHAR *pszExpireDate, INT64 llStaminaRemainTime,
								INT32 lResult)
	{
	if (NULL == pAgpdCharacter)
		return FALSE;

	AgpdMail *pAgpdMail = m_pAgpmMailBox->GetMail(lMailID);
	if (NULL == pAgpdMail)
		return FALSE;
	
	if (AGPMMAILBOX_RESULT_SUCCESS != lResult)
		{
		return SendItemSaveResult(pAgpdCharacter, lMailID, lResult);
		}
	
	// success
	time_t time = AuTimeStamp::ConvertOracleTimeToTimeStamp(pszExpireDate);
	INT32 lExpireDate = (INT32) time;
	AgpdItem *pAgpdItem = m_pAgsmItemManager->CreateItem(lItemTID, lItemQty, NULL, nStatus, pszConvert, lDurability,
													lMaxDurability, lFlag, pszOption, pszSkillPlus,
													lInUse, lUseCount, lRemainTime, lExpireDate, llStaminaRemainTime,
													ullItemSeq, pAgpdCharacter
													);
	if (NULL == pAgpdItem)
		{
		//########################## 조때따.
		return FALSE;
		}
	
	// add item to inventory. don't merge
	if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
		m_pAgpmItem->AddItemToCashInventory(pAgpdCharacter, pAgpdItem);
	else
	{
		if(m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem, FALSE) == FALSE)
		{
			// if it enable to use a sub inventory, insert to a sub inventory.
			if(m_pAgpmItem->IsEnableSubInventory(pAgpdCharacter))
				m_pAgpmItem->AddItemToSubInventory(pAgpdCharacter, pAgpdItem, FALSE);
		}
	}
	
	m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_MAIL_READ, pAgpdCharacter->m_lID, pAgpdItem, pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1, 0, pAgpdMail->m_szFromCharID);

	m_pAgsmItem->SendPacketItem(pAgpdItem, _GetCharacterNID(pAgpdCharacter));

	// reset item
	pAgpdMail->m_ullItemSeq = 0;
	pAgpdMail->SetReadAttach(TRUE);
	
	return SendItemSaveResult(pAgpdCharacter, lMailID, lResult);
	}




//	Arrange
//========================================
//
//	Not used.
//
BOOL AgsmMailBox::ArrangeMailBox(AgpdCharacter *pAgpdCharacter)
	{
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	AgpdMailCAD *pAgpdMailCAD = m_pAgpmMailBox->GetCAD(pAgpdCharacter);
	return ArrangeMailBox(pAgpdMailCAD);
	}


BOOL AgsmMailBox::ArrangeMailBox(AgpdMailCAD *pAgpdMailCAD)
	{
	if (NULL == pAgpdMailCAD)
		return FALSE;
	
	if (AGPMMAILBOX_MAX_MAILS_IN_EFFECT > pAgpdMailCAD->GetCount())	
		return TRUE;
	
	// 최대메일수로 4개의 array와 각 array에 count를 갖는 array를 준비하고
	// 각 메일의 상태에 따라 4개의 array에 나눠 넣은 후
	INT32	alArrangeMail[4][AGPMMAILBOX_MAX_MAILS_IN_EFFECT+20];
	ZeroMemory(alArrangeMail, sizeof(alArrangeMail));
	INT32	alMailCount[4];
	ZeroMemory(alMailCount, sizeof(alMailCount));

	AgpdMail *pAgpdMail = NULL;
	for (INT32 i=0; i<pAgpdMailCAD->GetCount(); ++i)
		{
		pAgpdMail = m_pAgpmMailBox->GetMail(pAgpdMailCAD->Get(i));
		if (NULL == pAgpdMail)
			continue;
		
		if (0 == pAgpdMail->m_ullItemSeq)							// no item
			if (pAgpdMail->GetRead())									// read
				alArrangeMail[0][(alMailCount[0])++] = pAgpdMail->m_lID;
			else														// unread
				alArrangeMail[1][(alMailCount[1])++] = pAgpdMail->m_lID;
		else														// item attached
			if (pAgpdMail->GetRead())									// read
				alArrangeMail[2][(alMailCount[2])++] = pAgpdMail->m_lID;
				
			else														// unread
				alArrangeMail[3][(alMailCount[3])++] = pAgpdMail->m_lID;
		}

	// remove mail
	BOOL bEnd = FALSE;
	for (INT32 j=0; j<4; ++j)
		{
		for (INT32 k=0; k < alMailCount[j]; ++k)
			{
			if (AGPMMAILBOX_MAX_MAILS_IN_EFFECT > pAgpdMailCAD->GetCount())
				{
				bEnd = TRUE;
				break;		// break k loop
				}
			
			pAgpdMail = m_pAgpmMailBox->GetMail(alArrangeMail[j][k]);
			if (NULL != pAgpdMail)
				{
				pAgpdMail->SetSizeOver(TRUE);
				pAgpdMailCAD->Remove(pAgpdMail->m_lID);
				_RemoveAndDestroyMail(pAgpdMail);
				}
			}
		
		if (bEnd)
			break;			// break j loop
		}
	
	return bEnd;
	}




//	Packet send
//========================================
//
BOOL AgsmMailBox::SendAddMail(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail, UINT32 ulNID)
	{
	if (!pAgpdCharacter || !pAgpdMail)
		return FALSE;

	INT8 cOperation = AGPMMAILBOX_OPERATION_ADD_MAIL;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmMailBox->MakePacketMail(pAgpdMail, &nPacketLength);
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmMailBox->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMMAILBOX_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketEmb,
														NULL,
														NULL
														);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, ulNID);
	}


BOOL AgsmMailBox::SendRemoveMail(AgpdCharacter *pAgpdCharacter, INT32 lID, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMMAILBOX_OPERATION_REMOVE_MAIL;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmMailBox->MakePacketMailOnlyID(lID, &nPacketLength);
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmMailBox->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMMAILBOX_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketEmb,
														NULL,
														&lResult
														);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmMailBox::SendRemoveAllMail(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	INT8 cOperation = AGPMMAILBOX_OPERATION_REMOVE_ALL_MAIL;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmMailBox->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMMAILBOX_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														NULL,
														NULL,
														NULL
														);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmMailBox::SendWriteResult(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMMAILBOX_OPERATION_WRITE_MAIL;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = NULL;
	if (pAgpdMail)
		{
		pvPacketEmb = m_pAgpmMailBox->m_csPacketMail.MakePacket(FALSE, &nPacketLength, 0,
																&pAgpdMail->m_lID,
																NULL,
																pAgpdMail->m_szToCharID,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL						// content is null
																);
		if (!pvPacketEmb || nPacketLength < 1)
			return FALSE;
		}

	PVOID pvPacket = m_pAgpmMailBox->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMMAILBOX_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketEmb,
														NULL,
														&lResult
														);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmMailBox::SendItemSaveResult(AgpdCharacter *pAgpdCharacter, INT32 lID, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMMAILBOX_OPERATION_ITEM_SAVE;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmMailBox->MakePacketMailOnlyID(lID, &nPacketLength);
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;

	PVOID pvPacket = m_pAgpmMailBox->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMMAILBOX_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketEmb,
														NULL,
														&lResult
														);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmMailBox::SendAllMail(AgpdCharacter *pAgpdCharacter, UINT32 ulNID)
	{
	AgpdMailCAD *pAgpdMailCAD = m_pAgpmMailBox->GetCAD(pAgpdCharacter);
	if (!pAgpdMailCAD)
		return FALSE;
	
	// 다 읽지 못하거나 클라이언트가 받을 준비가 안됐는데 보내는 경우를 막기위해
	// CBSendCharacterAllInfo, OnSelectMailResult 둘 중에 나중에 온경우에만 보내준다.
	if (FALSE == pAgpdMailCAD->m_bCheckResult)
		{
		pAgpdMailCAD->m_bCheckResult = TRUE;
		return FALSE;
		}
	
	SendRemoveAllMail(pAgpdCharacter);
	
	AuAutoLock Lock(pAgpdMailCAD->m_Mutex);
	if (!Lock.Result()) return FALSE;

	AgpdMail *pAgpdMail = NULL;
	BOOL bResult = TRUE;
	for (INT16 i=pAgpdMailCAD->GetCount(); i > 0; i--)		// 클라이언트 소팅 문제로 거꾸로 보낸다.
		{
		pAgpdMail = m_pAgpmMailBox->GetMail(pAgpdMailCAD->Get(i-1));
		
		// check if mine
		
		if (!SendAddMail(pAgpdCharacter, pAgpdMail, ulNID))
			bResult = FALSE;
		}
	
	return bResult;
	}




//	Callback setting
//=========================================================
//
BOOL AgsmMailBox::SetCallbackWriteMail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMMAILBOX_CB_WRITE_MAIL, pfCallback, pClass);
	}


BOOL AgsmMailBox::SetCallbackUpdateMail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMMAILBOX_CB_UPDATE_MAIL, pfCallback, pClass);
	}


BOOL AgsmMailBox::SetCallbackDeleteMail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMMAILBOX_CB_DELETE_MAIL, pfCallback, pClass);
	}


BOOL AgsmMailBox::SetCallbackSelectMail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMMAILBOX_CB_SELECT_MAIL, pfCallback, pClass);
	}


BOOL AgsmMailBox::SetCallbackItemSave(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMMAILBOX_CB_ITEM_SAVE, pfCallback, pClass);
	}




//	Helper methods
//==========================================================
//
BOOL AgsmMailBox::_IsFullInventory(AgpdCharacter *pAgpdCharacter)
	{
	AgpdItemADChar* pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	if (!pAgpdItemADChar || m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid))
		return TRUE;
	return FALSE;
	}


UINT32 AgsmMailBox::_GetCharacterNID(INT32 lCID)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(lCID);
	}


UINT32 AgsmMailBox::_GetCharacterNID(AgpdCharacter *pAgpdCharacter)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter);
	}


BOOL AgsmMailBox::_RemoveAndDestroyMail(AgpdMail *pAgpdMail, BOOL bDB)
	{
	// 삭제는 relay로부터 결과없이 그냥 포스팅한다.
	if (bDB)
		EnumCallback(AGSMMAILBOX_CB_DELETE_MAIL, pAgpdMail, NULL);

	m_pAgpmMailBox->RemoveMail(pAgpdMail);
	m_GenerateID.AddRemoveID(pAgpdMail->m_lID);
	m_pAgpmMailBox->DestroyMail(pAgpdMail);

	return TRUE;
	}



/*
AgpdAuctionSales* AgsmMailBox::AddSales(UINT64 ullDocID, UINT64 ullItemSeq, INT32 lItemID, INT32 lPrice,
										INT16 nQuantity, INT16 nStatus, CHAR *pszDate, INT32 lItemTID,
										AgpdCharacter *pAgpdCharacter)
	{
	AgpdAuctionSales *pAgpdMail = m_pAgpmAuction->CreateSales();
	if (pAgpdMail)
		{
		//pAgpdMail->m_lID = lItemID;
		pAgpdMail->m_lID = m_GenerateID.GetID();
		
		pAgpdMail->m_ullDocID = ullDocID;		
		pAgpdMail->m_ullItemSeq = ullItemSeq;
		pAgpdMail->m_lItemID = lItemID;
		pAgpdMail->m_lPrice = lPrice;
		pAgpdMail->m_nQuantity = nQuantity;
		pAgpdMail->m_nStatus = nStatus;
		pAgpdMail->m_lItemTID = lItemTID;
	
		if (pszDate)
			{
			strncpy(pAgpdMail->m_szDate, pszDate, 32);
			pAgpdMail->m_szDate[32] = _T('\0');
			}
			
		if (!m_pAgpmAuction->AddSalesToCAD(pAgpdCharacter, pAgpdMail))
			{
			m_GenerateID.AddRemoveID(pAgpdMail->m_lID);
			m_pAgpmAuction->DestroySales(pAgpdMail);
			return NULL;
			}
		}

	return pAgpdMail;
	}
*/



//	Log
//====================================
//
/*
void AgsmMailBox::WriteLog(eAGPDLOGTYPE_ITEM eType, AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem, INT32 lMoney, CHAR *pszCharID2, UINT64 ulDocID)
	{
	if (m_pAgpmLog)
		{
		AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
		AgsdItem *pAgsdItem = m_pAgsmItem->GetADItem(pAgpdItem);
		if (pAgsdCharacter && pAgsdItem)
			{
			AgpdItemConvertADItem *pAgpdItemConvertADItem = m_pAgpmItemConvert->GetADItem(pAgpdItem);

			CHAR szConvert[AGPDLOG_MAX_ITEM_CONVERT_STRING+1];
			ZeroMemory(szConvert, sizeof(szConvert));
			m_pAgpmItemConvert->EncodeConvertHistory(pAgpdItemConvertADItem, szConvert, AGPDLOG_MAX_ITEM_CONVERT_STRING);

			CHAR szOption[AGPDLOG_MAX_ITEM_OPTION+1];
			ZeroMemory(szOption, sizeof(szOption));
			m_pAgsmItem->EncodingOption(pAgpdItem, szOption, AGPDLOG_MAX_ITEM_OPTION);

			
			CHAR szDoc[AGPACHARACTER_MAX_ID_STRING+1];
			_i64toa(ulDocID, szDoc, 10);
			
			m_pAgpmLog->WriteLog_Item(eType,
									0,
									&pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pAgpdCharacter->m_szID,
									((AgpdCharacterTemplate*)pAgpdCharacter->m_pcsCharacterTemplate)->m_lID,
									m_pAgpmCharacter->GetLevel(pAgpdCharacter),
									m_pAgpmCharacter->GetExp(pAgpdCharacter),
									pAgpdCharacter->m_llMoney,
									pAgpdCharacter->m_llBankMoney,
									pAgsdItem->m_ullDBIID,
									((AgpdItemTemplate*)pAgpdItem->m_pcsItemTemplate)->m_lID,
									pAgpdItem->m_nCount,
									szConvert,
									szOption,
									lMoney,
									pszCharID2 ? pszCharID2 : szDoc
									);
			}
		}
	}
*/




