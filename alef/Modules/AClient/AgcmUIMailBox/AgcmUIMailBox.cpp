/*===========================================================================

	AgcmUIMailBox.cpp

===========================================================================*/


#include "AgcmUIMailBox.h"
#include "AgcmChatting2.h"
#include "StrUtil.h"


/********************************************************/
/*		The Implementation of AgcmUIMailBox class		*/
/********************************************************/
//
TCHAR AgcmUIMailBox::s_szEvent[AGCMUIMAILBOX_EVENT_MAX][30] = 
	{
	_T("MAIL_OPEN_LIST"),		//AGCMUIMAILBOX_EVENT_OPEN_LIST,
	_T("MAIL_OPEN_WRITE"),		//AGCMUIMAILBOX_EVENT_OPEN_WRITE,
	_T("MAIL_CLOSE_WRITE"),		//AGCMUIMAILBOX_EVENT_CLOSE_WRITE
	_T("MAIL_OPEN_READ"),		//AGCMUIMAILBOX_EVENT_OPEN_READ
	_T("MAIL_CLOSE_READ"),		//AGCMUIMAILBOX_EVENT_CLOSE_READ
	_T("MAIL_ICONREAD"),		//AGCMUIMAILBOX_EVENT_ICON_READ,
	_T("MAIL_ICONUNREAD"),		//AGCMUIMAILBOX_EVENT_ICON_UNREAD,
	_T("MAIL_ICONATTACH"),		//AGCMUIMAILBOX_EVENT_ICON_ATTACH,
	_T("MAIL_ICONUNATTACH"),	//AGCMUIMAILBOX_EVENT_ICON_UNATTACH,
	_T("MAIL_NOTIFY_UNREAD")	//AGCMUIMAILBOX_EVENT_NOTIFY_UNREAD
	};


TCHAR AgcmUIMailBox::s_szMessage[AGCMUIMAILBOX_MESSAGE_MAX][30] = 
	{
	_T("MAIL_LETTER_COUNT"),		//AGCMUIMAILBOX_MESSAGE_LETTER_COUNT = 0,
	_T("MAIL_CLOSE_CONFIRM"),		//AGCMUIMAILBOX_MESSAGE_CLOSE_CONFIRM
	_T("MAIL_WRITE_SUCCESS"),		//AGCMUIMAILBOX_MESSAGE_WRITE_SUCCESS,
	_T("MAIL_WRITE_FAIL"),			//AGCMUIMAILBOX_MESSAGE_WRITE_FAIL,
	_T("MAIL_ITEM_SAVED"),			//AGCMUIMAILBOX_MESSAGE_ITEM_SAVED,
	_T("MAIL_WRITE_DESC")			//AGCMUIMAILBOX_MESSAGE_WRITE_DESC
	};




AgcmUIMailBox::AgcmUIMailBox()
	{
	SetModuleName(_T("AgcmUIMailBox"));
	EnableIdle(TRUE);

	m_pAgpmCharacter = NULL;
	m_pAgpmMailBox = NULL;
	m_pAgpmGrid = NULL;
	m_pAgpmItem = NULL;
	
	m_pAgcmCharacter = NULL;
	m_pAgcmItem = NULL;
	m_pAgcmUIManager2 = NULL;
	m_pAgcmUIControl = NULL;
	m_pAgcmUIMain = NULL;

	m_pstListDummy = NULL;
	m_pstListMail = NULL;
	for (INT32 i=0; i<AGPMMAILBOX_MAX_MAILS_IN_EFFECT; ++i)
		{
		m_lListMailIndex[i] = i;
		}
	m_pstReadItem = NULL;
	m_pstReadDummy = NULL;
	m_pstReadItemSave = NULL;
	m_pstWriteItem = NULL;
	m_pstWriteDummy = NULL;
	m_pstCommonWriteMail = NULL;

	m_pAgpdCharacterSelf = NULL;
	m_pAgpdMailCADSelf = NULL;
	m_lSelectedIndex = -1;
	
	m_bReply = FALSE;
	}


AgcmUIMailBox::~AgcmUIMailBox()
	{
	}




//	ApModule inherited
//======================================================
//
BOOL AgcmUIMailBox::OnAddModule()
	{
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmMailBox		= (AgpmMailBox *) GetModule(_T("AgpmMailBox"));
	m_pAgpmGrid			= (AgpmGrid *) GetModule(_T("AgpmGrid"));
	m_pAgpmItem			= (AgpmItem *) GetModule(_T("AgpmItem"));
	
	m_pAgcmCharacter	= (AgcmCharacter *) GetModule(_T("AgcmCharacter"));
	m_pAgcmItem			= (AgcmItem *) GetModule(_T("AgcmItem"));
	m_pAgcmUIManager2	= (AgcmUIManager2 *) GetModule(_T("AgcmUIManager2"));
	m_pAgcmUIControl	= (AgcmUIControl *) GetModule(_T("AgcmUIControl"));
	m_pAgcmUIMain		= (AgcmUIMain *) GetModule(_T("AgcmUIMain"));

	if (!m_pAgpmCharacter || !m_pAgpmMailBox || !m_pAgpmGrid || !m_pAgpmItem
		|| !m_pAgcmCharacter || !m_pAgcmItem || !m_pAgcmUIManager2 || !m_pAgcmUIControl || !m_pAgcmUIMain
		)
		return FALSE;	

	if (!m_pAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;
	if (!m_pAgcmCharacter->SetCallbackReleaseSelfCharacter(CBReleaseSelfCharacter, this))
		return FALSE;		

	if (!m_pAgpmMailBox->SetCallbackAddMail(CBAddMail, this))
		return FALSE;
	if (!m_pAgpmMailBox->SetCallbackWriteMail(CBWriteMail, this))
		return FALSE;
	if (!m_pAgpmMailBox->SetCallbackRemoveMail(CBRemoveMail, this))
		return FALSE;
	if (!m_pAgpmMailBox->SetCallbackItemSave(CBItemSave, this))
		return FALSE;
	
	if (!m_pAgpmItem->SetCallbackRemove(CBRemoveItem, this))
		return FALSE;
	
	// UI
	if (!AddEvent() || !AddFunction() || !AddDisplay() || !AddUserData() || !AddBoolean())
		return FALSE;
	
	return TRUE;
	}


BOOL AgcmUIMailBox::OnInit()
	{
	m_pAgpmGrid->Init(&m_AgpdGridRead, 1, 1, 1);
	m_AgpdGridRead.m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;
	
	m_pAgpmGrid->Init(&m_AgpdGridWrite, 1, 1, 1);
	m_AgpdGridWrite.m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;
	
	// grid item
	m_pAgpdGridItemRead = m_pAgpmGrid->CreateGridItem();
	if (!m_pAgpdGridItemRead)
		return FALSE;
	
  #ifdef _DEBUG
	AS_REGISTER_TYPE_BEGIN(AgcmUIMailBox, AgcmUIMailBox);
		AS_REGISTER_METHOD0(void, TestOpenWriteWindow);
		AS_REGISTER_METHOD0(void, TestOpenList);
	AS_REGISTER_TYPE_END;
  #endif
	
	return TRUE;
	}


BOOL AgcmUIMailBox::OnDestroy()
	{
	// grid item
	if (m_pAgpdGridItemRead)
		{
		m_pAgpmGrid->DeleteGridItem(m_pAgpdGridItemRead);
		m_pAgpdGridItemRead = NULL;
		}
	
	// grid	
	m_pAgpmGrid->Remove(&m_AgpdGridRead);
	m_pAgpmGrid->Remove(&m_AgpdGridWrite);	
	
	return TRUE;
	}

BOOL	AgcmUIMailBox::OnIdle(UINT32 ulClockCount)
{
	//RefreshList();
	if (NULL == m_pAgpdMailCADSelf)
		return TRUE;
	
	m_pstListMail->m_stUserData.m_lCount = m_pAgpdMailCADSelf->GetCount();
	for (INT32 l = 0; l < m_pstListMail->m_stUserData.m_lCount; ++l)
		{
		AgpdMail *pAgpdMail = m_pAgpmMailBox->GetMail(m_pAgpdMailCADSelf->Get(l));
		
		if (pAgpdMail)
			{
			// is item attached?
			if (0 != pAgpdMail->m_lItemTID && 0 != pAgpdMail->m_nItemQty && FALSE == pAgpdMail->GetReadAttach())
				m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIMAILBOX_EVENT_ICON_ATTACH], l);
			else
				m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIMAILBOX_EVENT_ICON_UNATTACH], l);
			
			// is read?
			if (pAgpdMail->GetRead())
				m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIMAILBOX_EVENT_ICON_READ], l);
			else
				m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIMAILBOX_EVENT_ICON_UNREAD], l);
			
			}
		}	
	
	return TRUE;
}
		





//	OnAddModule helper
//=================================================================
//
BOOL AgcmUIMailBox::AddEvent()
	{
	for (int i=0; i < AGCMUIMAILBOX_EVENT_MAX; ++i)
		{
		m_lEvent[i] = m_pAgcmUIManager2->AddEvent(s_szEvent[i]);
		if (m_lEvent[i] < 0)
			return FALSE;
		}

	return TRUE;	
	}


BOOL AgcmUIMailBox::AddFunction()
	{
	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_LIST_OPEN"), CBOpenWindowList, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_LIST_READ"), CBClickButtonListRead, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_LIST_DELETE"), CBClickButtonListDelete, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_LIST_WRITE"), CBClickButtonListWrite, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_WRITE_SEND"), CBClickButtonWriteSend, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_WRITE_CLOSE"), CBCloseWindowWrite, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_WRITE_MOVE"), CBMoveItemWrite, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_WRITE_MVEND"), CBMoveEndItemWrite, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_WRITE_TAB"), CBInputTabWrite, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_READ_SAVE"), CBClickButtonReadItemSave, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_READ_REPLY"), CBClickButtonReadReply, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("MAIL_READ_CLOSE"), CBCloseWindowRead, 0))
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIMailBox::AddDisplay()
	{
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_LIST_SUBJECT"), 0, CBDisplayListSubject, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_LIST_DATE"), 0, CBDisplayListDate, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_LETTER_COUNT"), 0, CBDisplayAvailableLetter, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;		

	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_READ_TOCHAR"), 0, CBDisplayReadToCharID, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_READ_SUBJECT"), 0, CBDisplayReadSubject, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_READ_CONTENT"), 0, CBDisplayReadContent, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_READ_FROMCHAR"), 0, CBDisplayReadFromCharID, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_READ_DESC"), 0, CBDisplayReadDesc, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_WRITE_FROMCHAR"), 0, CBDisplayWriteFromCharID, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_WRITE_DESC"), 0, CBDisplayWriteDesc, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_WRITE_TOCHAR"), 0, CBDisplayWriteToCharID, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_WRITE_SUBJECT"), 0, CBDisplayWriteSubject, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("MAIL_WRITE_CONTENT"), 0, CBDisplayWriteContent, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	
	return TRUE;
	}


BOOL AgcmUIMailBox::AddUserData()
	{
	m_pstListDummy = m_pAgcmUIManager2->AddUserData(_T("MAIL_UD_LIST_DUMMY"), NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstListDummy)
		return FALSE;
	m_pstListMail = m_pAgcmUIManager2->AddUserData(_T("MAIL_UD_MAIL"), m_lListMailIndex, sizeof(INT32),
												AGPMMAILBOX_MAX_MAILS_IN_EFFECT, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstListMail)
		return FALSE;
	m_pstReadItem = m_pAgcmUIManager2->AddUserData(_T("MAIL_UD_READ_ITEM"), &m_AgpdGridRead, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
	if (NULL == m_pstReadItem)
		return FALSE;
	m_pstReadDummy = m_pAgcmUIManager2->AddUserData(_T("MAIL_UD_READ_DUMMY"), NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstReadDummy)
		return FALSE;
	m_pstReadItemSave = m_pAgcmUIManager2->AddUserData(_T("MAIL_UD_SAVE_BTN"), NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstReadItemSave)
		return FALSE;
	m_pstWriteItem = m_pAgcmUIManager2->AddUserData(_T("MAIL_UD_WRITE_ITEM"), &m_AgpdGridWrite, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
	if (NULL == m_pstWriteItem)
		return FALSE;
	m_pstWriteDummy = m_pAgcmUIManager2->AddUserData(_T("MAIL_UD_WRITE_DUMMY"), NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstWriteDummy)
		return FALSE;
	m_pstCommonWriteMail = m_pAgcmUIManager2->AddUserData(_T("MAIL_UD_COM_WRITE"), NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstCommonWriteMail)
		return FALSE;
	
	return TRUE;
	}


BOOL AgcmUIMailBox::AddBoolean()
	{
	if (!m_pAgcmUIManager2->AddBoolean(this, _T("MAIL_ACTIVE_DEL_BTN"), CBIsActiveDeleteButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddBoolean(this, _T("MAIL_ACTIVE_SAV_BTN"), CBIsActiveItemSaveButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddBoolean(this, _T("MAIL_ACTIVE_WRT_BTN"), CBIsActiveWriteButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
			
	return TRUE;
	}




//
//=======================================================
//
BOOL AgcmUIMailBox::OpenWriteWindow()
	{
	// 편지지 있냐?
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	INT32 lNormal = 0;
	INT32 lPremium = 0;
	m_pAgpmMailBox->GetLetterCount(pAgpdCharacter, lNormal, lPremium);
	
	if (0 >= lNormal)
		return FALSE;
	
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIMAILBOX_EVENT_OPEN_WRITE]);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstWriteDummy);
	
	return TRUE;		
	}




//	Module callback
//=======================================================
//
BOOL AgcmUIMailBox::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgcmUIMailBox *pThis = (AgcmUIMailBox *) pClass;
	
	pThis->m_pAgpdCharacterSelf = (AgpdCharacter *) pData;
	pThis->m_pAgpdMailCADSelf = pThis->m_pAgpmMailBox->GetCAD(pThis->m_pAgpdCharacterSelf);
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUIMailBox *pThis = (AgcmUIMailBox *) pClass;	
	
	pThis->m_pAgpdCharacterSelf = NULL;
	pThis->m_pAgpdMailCADSelf = NULL;
	pThis->m_lSelectedIndex = -1;
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBAddMail(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	AgpdMailArg		*pArg = (AgpdMailArg *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;

	// SetSelfCharacter되기 이전에 불릴 수 있으므로 pAgpdCharacter로 CAD를 가져온다.	
	AgpdMail *pAgpdMail = pArg->m_pAgpdMail;
	AgpdMailCAD *pAgpdMailCAD = pThis->m_pAgpmMailBox->GetCAD(pAgpdCharacter);
	if (NULL == pAgpdMailCAD)
		{
		pThis->m_pAgpmMailBox->DestroyMail(pAgpdMail);
		return FALSE;
		}
	
	if (FALSE == pThis->m_pAgpmMailBox->AddMail(pArg->m_pAgpdMail)
		|| FALSE == pThis->m_pAgpmMailBox->AddMailToCAD(pAgpdCharacter, pArg->m_pAgpdMail, TRUE)
		)
		{
		pThis->m_pAgpmMailBox->RemoveMail(pAgpdMail);
		pThis->m_pAgpmMailBox->DestroyMail(pAgpdMail);
		return FALSE;
		}

	AgcmChatting2* pcmChatting = ( AgcmChatting2* )g_pEngine->GetModule( "AgcmChatting2" );
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( pcmChatting && pcmUIManager )
	{
		char* pString = pcmUIManager->GetUIMessage( "YouHaveMail" );
		if( pString && strlen( pString ) > 0 )
		{
			pcmChatting->AddSystemMessage( pString );
		}
	}

	// refresh UI
	pThis->RefreshList();
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBWriteMail(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	AgpdMailArg		*pArg = (AgpdMailArg *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	if (pAgpdCharacter != pThis->m_pAgpdCharacterSelf)
		return FALSE;
	
	if (AGPMMAILBOX_RESULT_SUCCESS == pArg->m_lResult)
		{
		TCHAR *psz = pThis->GetMessageTxt(AGCMUIMAILBOX_MESSAGE_WRITE_SUCCESS);
		pThis->m_pAgcmUIManager2->ActionMessageOKDialog(psz);

		// close write window
		pThis->RemoveItemFromWriteGrid(NULL);
		pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIMAILBOX_EVENT_CLOSE_WRITE]);
	
		// refresh write button
		pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstCommonWriteMail);
		
		// refresh letter count
		pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstListDummy);
		}
	else
		{
		TCHAR *psz = pThis->GetMessageTxt(AGCMUIMAILBOX_MESSAGE_WRITE_FAIL);
		pThis->m_pAgcmUIManager2->ActionMessageOKDialog(psz);
		}
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBRemoveMail(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	AgpdMailArg		*pArg = (AgpdMailArg *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	if (pAgpdCharacter != pThis->m_pAgpdCharacterSelf
		|| NULL == pThis->m_pAgpdMailCADSelf)
		return FALSE;

	if (AGPMMAILBOX_RESULT_SUCCESS != pArg->m_lResult)
		return FALSE;

	AgpdMail *pAgpdMail = pThis->m_pAgpmMailBox->GetMail(pArg->m_pAgpdMail->m_lID);
	if (NULL == pAgpdMail)
		return FALSE;

	// if read window of this mail opened, close it first
	if (-1 != pThis->m_lSelectedIndex
		&& pThis->m_lSelectedIndex == pThis->m_pAgpdMailCADSelf->GetIndex(pAgpdMail->m_lID)
		)
		{
		pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIMAILBOX_EVENT_CLOSE_READ]);
		pThis->m_lSelectedIndex = -1;
		}
		
	// remove mail
	pThis->m_pAgpdMailCADSelf->m_Mutex.WLock();	
	pThis->m_pAgpdMailCADSelf->Remove(pAgpdMail->m_lID);
	pThis->m_pAgpmMailBox->RemoveMail(pAgpdMail);
	pThis->m_pAgpmMailBox->DestroyMail(pAgpdMail);
	pThis->m_pAgpdMailCADSelf->m_Mutex.Release();	
	
	// refresh
	pThis->RefreshList();
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBItemSave(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	AgpdMailArg		*pArg = (AgpdMailArg *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	if (pAgpdCharacter != pThis->m_pAgpdCharacterSelf)
		return FALSE;
		
	if (AGPMMAILBOX_RESULT_SUCCESS != pArg->m_lResult)
		return FALSE;
	
	AgpdMail *pAgpdMail = pThis->m_pAgpmMailBox->GetMail(pArg->m_pAgpdMail->m_lID);
	if (NULL == pAgpdMail)
		return FALSE;
	
	// update
	pAgpdMail->SetReadAttach(TRUE);

	TCHAR *psz = pThis->GetMessageTxt(AGCMUIMAILBOX_MESSAGE_ITEM_SAVED);
	pThis->m_pAgcmUIManager2->ActionMessageOKDialog(psz);

	// clean up
	pAgpdMail->m_lItemTID = 0;
	pAgpdMail->m_nItemQty = 0;

	// refresh
	pThis->RefreshList();
	pThis->RemoveItemFromReadGrid();
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstReadDummy);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstReadItemSave);

	return TRUE;
	}


BOOL AgcmUIMailBox::CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	AgpdItem		*pAgpdItem = (AgpdItem *) pData;


	if (NULL == pThis->m_pAgpdCharacterSelf
		|| pAgpdItem->m_pcsCharacter != pThis->m_pAgpdCharacterSelf
		)
		return FALSE;

	AgpdGridItem *pAgpdGridItem = pThis->m_pAgpmGrid->GetItem(&(pThis->m_AgpdGridWrite), 0);
	if (NULL != pAgpdGridItem
		&& pAgpdGridItem->m_lItemID == pAgpdItem->m_lID
		)
		{
		pThis->RemoveItemFromWriteGrid(NULL);
		}
	
	return TRUE;
	}




//	UI Function callback
//=======================================================
//
BOOL AgcmUIMailBox::CBOpenWindowList(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	
	if (NULL == pThis->m_pAgpdMailCADSelf)
		return FALSE;

	// 뭐 걍 유저데이타나 리프레쉬 해주는거지
	pThis->RefreshList();
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBClickButtonListRead(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	
	if (NULL == pThis->m_pAgpdCharacterSelf)
		return FALSE;
	
	pThis->m_lSelectedIndex = pcsSourceControl->m_lUserDataIndex;
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pThis->m_lSelectedIndex);
	if (NULL == pAgpdMail)
		return FALSE;

	// 이미 어떤 메일을 읽고 있는 상태라면 아무작업도 하지 않고 넘긴다.
	if( pThis->IsOpenMailRead() ) return TRUE;

	// open read window event
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIMAILBOX_EVENT_OPEN_READ]);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstReadDummy);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstReadItemSave);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstReadItem);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstCommonWriteMail);
	pThis->AddItemToReadGrid(pAgpdMail->m_lItemTID, pAgpdMail->m_nItemQty);
	
	// Send to server
	pAgpdMail->SetRead(TRUE);
	pThis->SendReadMail(pThis->m_pAgpdCharacterSelf, pAgpdMail);
	pThis->RefreshList();
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBClickButtonListDelete(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	
	if (NULL == pThis->m_pAgpdCharacterSelf)
		return FALSE;
	
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pcsSourceControl->m_lUserDataIndex);
	if (NULL == pAgpdMail)
		return FALSE;

	return pThis->SendRemoveMail(pThis->m_pAgpdCharacterSelf, pAgpdMail);
	}


BOOL AgcmUIMailBox::CBClickButtonListWrite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	
	pThis->m_bReply = FALSE;
	return pThis->OpenWriteWindow();
	}


BOOL AgcmUIMailBox::CBClickButtonWriteSend(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData1 || !pData2 || !pData3)
		return FALSE;

	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	AgcdUIControl	*pControlToCharID = (AgcdUIControl *) pData1;
	AgcdUIControl	*pControlSubject = (AgcdUIControl *) pData2;
	AgcdUIControl	*pControlContent = (AgcdUIControl *) pData3;
	
	if (!pControlToCharID->m_pcsBase || !pControlSubject->m_pcsBase || !pControlContent->m_pcsBase)
		return FALSE;
	
	TCHAR* pszToCharID = (TCHAR *) ((AcUIEdit *)pControlToCharID->m_pcsBase)->GetText();
	TCHAR* pszSubject = (TCHAR *) ((AcUIEdit *)pControlSubject->m_pcsBase)->GetText();
	TCHAR* pszContent = (TCHAR *) ((AcUIEdit *)pControlContent->m_pcsBase)->GetText();

	StrUtil::RTrim( pszToCharID );

	if (NULL == pThis->m_pAgpdCharacterSelf || NULL == pszToCharID || NULL == pszSubject || NULL == pszContent)
		return FALSE;
	
	if (0 == strcmp(pThis->m_pAgpdCharacterSelf->m_szID, pszToCharID))
		return FALSE;
	
	INT32 lLenToCharID = _tcslen(pszToCharID);
	INT32 lLenSubject = _tcslen(pszSubject);
	if (0 >= lLenToCharID || lLenToCharID > AGPDCHARACTER_NAME_LENGTH
		|| 0 >= lLenSubject || lLenSubject > AGPDCHARACTER_NAME_LENGTH
		)
		return FALSE;
	
	// make mail
	AgpdMail csMail;
	_tcscpy(csMail.m_szFromCharID, pThis->m_pAgpdCharacterSelf->m_szID);
	_tcscpy(csMail.m_szToCharID, pszToCharID);
	_tcscpy(csMail.m_szSubject, pszSubject);
	if (!csMail.SetContent(pszContent))
		return FALSE;
	
	// check item attach
	AgpdItem *pAgpdItem = NULL;
	AgpdGridItem *pAgpdGridItem = pThis->m_pAgpmGrid->GetItem(&pThis->m_AgpdGridWrite, 0, 0, 0);
	if (pAgpdGridItem)
		pAgpdItem = pThis->m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
	
	// check letter
	INT32 lNormal = 0;
	INT32 lPremium = 0;
	pThis->m_pAgpmMailBox->GetLetterCount(pThis->m_pAgpdCharacterSelf, lNormal, lPremium);
	if (0 >= lNormal)
		return FALSE;

	// make packet and send
	pThis->SendWriteMail(pThis->m_pAgpdCharacterSelf, &csMail, pAgpdItem ? pAgpdItem->m_lID : AGPMITEM_INVALID_IID);
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBCloseWindowWrite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData1 || !pData2 || !pData3)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;

	AgcdUIControl	*pControlToCharID = (AgcdUIControl *) pData1;
	AgcdUIControl	*pControlSubject = (AgcdUIControl *) pData2;
	AgcdUIControl	*pControlContent = (AgcdUIControl *) pData3;
	
	if (!pControlToCharID->m_pcsBase || !pControlSubject->m_pcsBase || !pControlContent->m_pcsBase)
		return FALSE;
	
	TCHAR *psz = pThis->GetMessageTxt(AGCMUIMAILBOX_MESSAGE_CLOSE_CONFIRM);
	if (IDOK == pThis->m_pAgcmUIManager2->ActionMessageOKCancelDialog(psz))
		{
		// fire close event
		pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIMAILBOX_EVENT_CLOSE_WRITE]);
		pThis->RemoveItemFromWriteGrid(NULL);
		}
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBMoveItemWrite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass
		|| !pcsSourceControl
		|| !pcsSourceControl->m_pcsBase
		|| AcUIBase::TYPE_GRID != pcsSourceControl->m_lType
		)
		return FALSE;
	
	AgcmUIMailBox		*pThis = (AgcmUIMailBox *) pClass;
	AcUIGrid			*pAcUIGrid = (AcUIGrid *) pcsSourceControl->m_pcsBase;
	AgpdGridSelectInfo	*pAgpdGridSelectInfo = pAcUIGrid->GetDragDropMessageInfo();
	
	if (!pAgpdGridSelectInfo || !pAgpdGridSelectInfo->pGridItem ||
		 AGPDGRID_ITEM_TYPE_ITEM != pAgpdGridSelectInfo->pGridItem->m_eType)
		return FALSE;

	AgpdItem *pAgpdItem = pThis->m_pAgpmItem->GetItem(pAgpdGridSelectInfo->pGridItem->m_lItemID);
	if (!pAgpdItem)
		return FALSE;

	if (FALSE == pThis->m_pAgpmMailBox->IsValidAttachItem(pAgpdItem))
		return FALSE;
	/*	
	INT16 nLayer = pAcUIGrid->m_lNowLayer;
	INT16 nRow = pAgpdGridSelectInfo->lGridRow;
	INT16 nColumn = pAgpdGridSelectInfo->lGridColumn;
	*/
	
	pThis->AddItemToWriteGrid(pAgpdItem->m_pcsGridItem);
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBMoveEndItemWrite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl ||
		!pcsSourceControl->m_pcsBase ||
		AcUIBase::TYPE_GRID != pcsSourceControl->m_lType
		)
		return FALSE;

	AgcmUIMailBox		*pThis					= (AgcmUIMailBox *) pClass;
	AcUIGrid			*pAcUIGrid				= (AcUIGrid *) pcsSourceControl->m_pcsBase;
	AgpdGridSelectInfo	*pAgpdGridSelectInfo	= pAcUIGrid->GetGridItemClickInfo();

	AgpdGrid *pAgpdGrid = pThis->m_pAgcmUIManager2->GetControlGrid(pcsSourceControl);
	if (!pAgpdGridSelectInfo || !pAgpdGridSelectInfo->pGridItem || !pAgpdGrid)
		return FALSE;

	// remove from grid	
	pThis->RemoveItemFromWriteGrid(pAgpdGridSelectInfo->pGridItem);
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBInputTabWrite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData1 || !pData2)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;

	AgcdUIControl	*pControl1 = (AgcdUIControl *) pData1;
	AgcdUIControl	*pControl2 = (AgcdUIControl *) pData2;
	
	if (!pControl1->m_pcsBase || !pControl2->m_pcsBase)
		return FALSE;

	((AcUIEdit *)pControl1->m_pcsBase)->ReleaseMeActiveEdit();
	((AcUIEdit *)pControl2->m_pcsBase)->SetMeActiveEdit();
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBCloseWindowRead(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	
	pThis->m_lSelectedIndex = -1;
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIMAILBOX_EVENT_CLOSE_READ]);
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBClickButtonReadItemSave(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;

	if (NULL == pThis->m_pAgpdCharacterSelf)
		return FALSE;
	
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pThis->m_lSelectedIndex);
	if (NULL == pAgpdMail)
		return FALSE;
	
	if (0 == pAgpdMail->m_lItemTID || 0 == pAgpdMail->m_nItemQty || pAgpdMail->GetReadAttach())
		return FALSE;

	// Open MessageBox to Confirm Item Save
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	char* pMessage = pcmUIManager->GetUIMessage( "MailBoxItem_SaveConfirm" );
	if( !pMessage || strlen( pMessage ) <= 0 ) return FALSE;

	int nResult = pcmUIManager->ActionMessageOKCancelDialog( pMessage );
	if( nResult == 0 ) return TRUE;

	return pThis->SendItemSave(pThis->m_pAgpdCharacterSelf, pAgpdMail);
	}


BOOL AgcmUIMailBox::CBClickButtonReadReply(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	
	pThis->m_bReply = TRUE;
	pThis->OpenWriteWindow();
	
	return TRUE;
	}




//	Display callback
//=========================================================
//
BOOL AgcmUIMailBox::CBDisplayListSubject(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pData || !pClass || AGCDUI_USERDATA_TYPE_INT32 != eType)
		return FALSE;

	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	INT32			lIndex = *((INT32 *)pData);

	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(lIndex);
	if (NULL != pAgpdMail)
		{
		_stprintf(szDisplay, _T("%s"), pAgpdMail->m_szSubject);
		}

	return TRUE;	
	}


BOOL AgcmUIMailBox::CBDisplayListDate(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pData || !pClass || AGCDUI_USERDATA_TYPE_INT32 != eType)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	INT32			lIndex				= *((INT32 *)pData);
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(lIndex);
	if (NULL != pAgpdMail)
		{
		TCHAR szDate[64];
		AuTimeStamp::ConvertTimeStampToOracleTime( (UINT32)pAgpdMail->m_lDate, szDate, sizeof(szDate));
		_stprintf(szDisplay, _T("%s"), szDate);
		}

	return TRUE;
	}


BOOL AgcmUIMailBox::CBDisplayAvailableLetter(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;

	if (NULL == pThis->m_pAgpdCharacterSelf)
		return FALSE;
	
	INT32 lNormal = 0;
	INT32 lPremium = 0;
	pThis->m_pAgpmMailBox->GetLetterCount(pThis->m_pAgpdCharacterSelf, lNormal, lPremium);
	
	TCHAR *psz = pThis->GetMessageTxt(AGCMUIMAILBOX_MESSAGE_LETTER_COUNT);
	if (NULL != psz)
		{
		TCHAR sz[1024];
		ZeroMemory(sz, sizeof(sz));
		_stprintf(sz, psz, lNormal);
		((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(sz);
		}

	return TRUE;		
	}


BOOL AgcmUIMailBox::CBDisplayReadToCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pThis->m_lSelectedIndex);
	if (NULL != pAgpdMail)
		{
		_stprintf(szDisplay, _T("%s"), pAgpdMail->m_szToCharID);
		}
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBDisplayReadSubject(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pThis->m_lSelectedIndex);
	if (NULL != pAgpdMail)
		{
		_stprintf(szDisplay, _T("%s"), pAgpdMail->m_szSubject);
		}
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBDisplayReadContent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pThis->m_lSelectedIndex);
	if (NULL != pAgpdMail && NULL != pAgpdMail->GetContent())
		{
		((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(pAgpdMail->GetContent());
		}
	
	return TRUE;	
	}


BOOL AgcmUIMailBox::CBDisplayReadFromCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pThis->m_lSelectedIndex);
	if (NULL != pAgpdMail)
		{
		_stprintf(szDisplay, _T("%s"), pAgpdMail->m_szFromCharID);
		}
	
	return TRUE;	
	}


BOOL AgcmUIMailBox::CBDisplayReadDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	//((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(_T(""));
	
	return TRUE;	
	}


BOOL AgcmUIMailBox::CBDisplayWriteFromCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	if (pThis->m_pAgpdCharacterSelf)
		{
		_stprintf(szDisplay, _T("%s"), pThis->m_pAgpdCharacterSelf->m_szID);
		}
	
	return TRUE;	
	}


BOOL AgcmUIMailBox::CBDisplayWriteDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;

	if (NULL == pThis->m_pAgpdCharacterSelf)
		return FALSE;
	
	INT32 lNormal = 0;
	INT32 lPremium = 0;
	pThis->m_pAgpmMailBox->GetLetterCount(pThis->m_pAgpdCharacterSelf, lNormal, lPremium);	

	TCHAR *psz = pThis->GetMessageTxt(AGCMUIMAILBOX_MESSAGE_WRITE_DESC);
	TCHAR sz[256];
	ZeroMemory(sz, sizeof(sz));
	_stprintf(sz, psz, lNormal);

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pAgcmUIManager2->GetLineDelimiter());
	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(sz);
	
	return TRUE;	
	}


BOOL AgcmUIMailBox::CBDisplayWriteToCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	if (pThis->m_bReply)
		{
		AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pThis->m_lSelectedIndex);
		if (NULL != pAgpdMail)
			{
			((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(pAgpdMail->m_szFromCharID);
			}
		}
	else
		{
		((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(_T(""));
		}
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBDisplayWriteSubject(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	if (pThis->m_bReply)
		{
		AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pThis->m_lSelectedIndex);
		if (NULL != pAgpdMail)
			{
			TCHAR sz[AGPMMAILBOX_MAX_SUBJECT_LENGTH + 1];
			ZeroMemory(sz, sizeof(sz));
			_tcscpy(sz, _T("[RE]"));
			_tcsncat(sz, pAgpdMail->m_szSubject, AGPMMAILBOX_MAX_SUBJECT_LENGTH - 4);
			((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(sz);
			}
		}
	else
		{
		((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(_T(""));
		}
	
	return TRUE;
	}


BOOL AgcmUIMailBox::CBDisplayWriteContent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pThis->m_lSelectedIndex);

	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(_T(""));
	
	return TRUE;	
	}




//	Boolean callback
//===========================================================
//
BOOL AgcmUIMailBox::CBIsActiveDeleteButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;
	
	AgcmUIMailBox	*pThis = (AgcmUIMailBox *) pClass;
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pcsSourceControl->m_lUserDataIndex);
	if (NULL != pAgpdMail)
		{
		if (pAgpdMail->GetRead())		// deletable if already read
			return TRUE;
		}
	
	return FALSE;
	}


BOOL AgcmUIMailBox::CBIsActiveItemSaveButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	AgpdMail *pAgpdMail = pThis->GetMailOfIndex(pThis->m_lSelectedIndex);
	if (NULL != pAgpdMail)
		{
		AgpdItemTemplate *pAgpdItemTemplate = pThis->m_pAgpmItem->GetItemTemplate(pAgpdMail->m_lItemTID);
		if (NULL != pAgpdItemTemplate && pAgpdMail->m_nItemQty > 0)
			return TRUE;
		}
	
	// 여기에 오면 첨부된 아이템이 없는 경우이니 해당 그리드를 초기화해준다.
	pThis->RemoveItemFromReadGrid();
	return FALSE;		
	}


BOOL AgcmUIMailBox::CBIsActiveWriteButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIMailBox	*pThis				= (AgcmUIMailBox *)	pClass;
	
	if (NULL == pThis->m_pAgpdCharacterSelf)
		return FALSE;
	
	INT32 lNormal = 0;
	INT32 lPremium = 0;
	pThis->m_pAgpmMailBox->GetLetterCount(pThis->m_pAgpdCharacterSelf, lNormal, lPremium);
	
	if (0 < lNormal)
		return TRUE;
	
	return FALSE;
	}




//	Packet
//============================================================
//
BOOL AgcmUIMailBox::SendWriteMail(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail, INT32 lItemID)
	{
	INT16 nPacketLength = 0;
	PVOID pvPacketMail = m_pAgpmMailBox->MakePacketMail(pAgpdMail, &nPacketLength);
	if (NULL == pvPacketMail || 0 >= nPacketLength)
		return FALSE;
	
	INT8 cOperation = AGPMMAILBOX_OPERATION_WRITE_MAIL;
	PVOID pvPacket = m_pAgpmMailBox->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMMAILBOX_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketMail,
														&lItemID,
														NULL
														);
	m_pAgpmMailBox->m_csPacketMail.FreePacket(pvPacketMail);
	if (NULL == pvPacket || 0 >= nPacketLength)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmMailBox->m_csPacket.FreePacket(pvPacket);
	
	if( m_pAgcmUIManager2 )
	{
		m_pAgcmUIManager2->ThrowEvent( m_lEvent[ AGCMUIMAILBOX_EVENT_CLOSE_WRITE ] );
	}
	return bResult;
	}


BOOL AgcmUIMailBox::SendReadMail(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail)
	{
	INT16 nPacketLength = 0;
	PVOID pvPacketMail = m_pAgpmMailBox->MakePacketMailOnlyID(pAgpdMail->m_lID, &nPacketLength);
	if (NULL == pvPacketMail || 0 >= nPacketLength)
		return FALSE;
	
	INT8 cOperation = AGPMMAILBOX_OPERATION_READ_MAIL;
	PVOID pvPacket = m_pAgpmMailBox->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMMAILBOX_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketMail,
														NULL,
														NULL
														);
	m_pAgpmMailBox->m_csPacketMail.FreePacket(pvPacketMail);
	if (NULL == pvPacket || 0 >= nPacketLength)
		return FALSE;		
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmMailBox->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
	}


BOOL AgcmUIMailBox::SendRemoveMail(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail)
	{
	INT16 nPacketLength = 0;
	PVOID pvPacketMail = m_pAgpmMailBox->MakePacketMailOnlyID(pAgpdMail->m_lID, &nPacketLength);
	if (NULL == pvPacketMail || 0 >= nPacketLength)
		return FALSE;
	
	INT8 cOperation = AGPMMAILBOX_OPERATION_REMOVE_MAIL;
	PVOID pvPacket = m_pAgpmMailBox->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMMAILBOX_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketMail,
														NULL,
														NULL
														);
	m_pAgpmMailBox->m_csPacketMail.FreePacket(pvPacketMail);
	if (NULL == pvPacket || 0 >= nPacketLength)
		return FALSE;		
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmMailBox->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
	}


BOOL AgcmUIMailBox::SendItemSave(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail)
	{
		// 메일보내기 전에 읽던 메일창을 닫는다.
		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
		if( !pcmUIManager ) return FALSE;

		if( !pcmUIManager->ThrowEvent( m_lEvent[ AGCMUIMAILBOX_EVENT_CLOSE_READ ] ) ) return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacketMail = m_pAgpmMailBox->MakePacketMailOnlyID(pAgpdMail->m_lID, &nPacketLength);
	if (NULL == pvPacketMail || 0 >= nPacketLength)
		return FALSE;
	
	INT8 cOperation = AGPMMAILBOX_OPERATION_ITEM_SAVE;
	PVOID pvPacket = m_pAgpmMailBox->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMMAILBOX_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketMail,
														NULL,
														NULL
														);
	m_pAgpmMailBox->m_csPacketMail.FreePacket(pvPacketMail);
	if (NULL == pvPacket || 0 >= nPacketLength)
		return FALSE;		
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmMailBox->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
	}




//	Test
//=========================================================
//
#ifdef _DEBUG

void AgcmUIMailBox::TestOpenWriteWindow()
	{
	OpenWriteWindow();
	}

void AgcmUIMailBox::TestOpenList()
	{
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIMAILBOX_EVENT_OPEN_LIST]);
	}

#endif




//	Helper
//=========================================================
//
CHAR* AgcmUIMailBox::GetMessageTxt(eAGCMUIMAILBOX_MESSAGE eMessage)
	{
	if (0 > eMessage || eMessage > AGCMUIMAILBOX_MESSAGE_MAX)
		return NULL;

	return m_pAgcmUIManager2->GetUIMessage(s_szMessage[eMessage]);
	}


AgpdMail* AgcmUIMailBox::GetMailOfIndex(INT32 lIndex)
	{
	AgpdMail *pAgpdMail = NULL;
	if (-1 != lIndex && NULL != m_pAgpdMailCADSelf)
		pAgpdMail = m_pAgpmMailBox->GetMail(m_pAgpdMailCADSelf->Get(lIndex));
	
	return pAgpdMail;
	}


void AgcmUIMailBox::RefreshList()
	{
	if (NULL == m_pAgpdMailCADSelf)
		return;
	
	AuAutoLock LockCAD(m_pAgpdMailCADSelf->m_Mutex);
	if (!LockCAD.Result()) return;
	
	m_pstListMail->m_stUserData.m_lCount = m_pAgpdMailCADSelf->GetCount();
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstListMail);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstListDummy);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstCommonWriteMail);
	
	for (INT32 l = 0; l < m_pstListMail->m_stUserData.m_lCount; ++l)
		{
		AgpdMail *pAgpdMail = m_pAgpmMailBox->GetMail(m_pAgpdMailCADSelf->Get(l));
		
		if (pAgpdMail)
			{
			// is item attached?
			if (0 != pAgpdMail->m_lItemTID && 0 != pAgpdMail->m_nItemQty && FALSE == pAgpdMail->GetReadAttach())
				m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIMAILBOX_EVENT_ICON_ATTACH], l);
			else
				m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIMAILBOX_EVENT_ICON_UNATTACH], l);
			
			// is read?
			if (pAgpdMail->GetRead())
				m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIMAILBOX_EVENT_ICON_READ], l);
			else
				m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIMAILBOX_EVENT_ICON_UNREAD], l);
			
			}
		}
	}


BOOL AgcmUIMailBox::AddItemToWriteGrid(AgpdGridItem *pAgpdGridItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
	{
	if (!m_pAgpmGrid->IsEmptyGrid(&m_AgpdGridWrite, nLayer, nRow, nColumn, 1, 1))
		return FALSE;
	
	if (!m_pAgpmGrid->AddItem(&m_AgpdGridWrite, pAgpdGridItem)
		|| !m_pAgpmGrid->Add(&m_AgpdGridWrite, nLayer, nRow, nColumn, pAgpdGridItem, 1, 1)
		)
		return FALSE;

	if (m_pstWriteItem)
		{
		m_pstWriteItem->m_stUserData.m_pvData = &m_AgpdGridWrite;
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstWriteItem);
		}
	
	return TRUE;
	}


BOOL AgcmUIMailBox::AddItemToReadGrid(INT32 lItemTID, INT32 lCount)
	{
	if (0 == lItemTID || 0 == lCount)
		return FALSE;
	
	AgpdItemTemplate *pAgpdItemTemplate = m_pAgpmItem->GetItemTemplate(lItemTID);
	if (NULL == pAgpdItemTemplate)
		return FALSE;
	
	SetGridItemAttachedTexture(m_pAgpdGridItemRead, pAgpdItemTemplate);
	
	if (pAgpdItemTemplate->m_bStackable)
		{
		TCHAR sz[AGPDGRIDITEM_BOTTOM_STRING_LENGTH + 1] = {0,};
		_stprintf(sz, _T("%d"), lCount);
		m_pAgpdGridItemRead->SetRightBottomString(sz);
		}
	else
		{
		m_pAgpdGridItemRead->SetRightBottomString(_T(""));
		}

	m_pAgpmGrid->Reset(&m_AgpdGridRead);
	if (!m_pAgpmGrid->AddItem(&m_AgpdGridRead, m_pAgpdGridItemRead)
		|| !m_pAgpmGrid->Add(&m_AgpdGridRead, m_pAgpdGridItemRead, 1, 1)
		)
		return FALSE;

	if (m_pstReadItem)
		{
		m_pstReadItem->m_stUserData.m_pvData = &m_AgpdGridRead;
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstReadItem);
		}

	return TRUE;
	}


void AgcmUIMailBox::RemoveItemFromWriteGrid(AgpdGridItem *pAgpdGridItem)
	{
	if (NULL == pAgpdGridItem)
		m_pAgpmGrid->Reset(&m_AgpdGridWrite);
	else
		m_pAgpmGrid->RemoveItem(&m_AgpdGridWrite, pAgpdGridItem);
	
	if (m_pstWriteItem)
		{
		m_pstWriteItem->m_stUserData.m_pvData	= &m_AgpdGridWrite;
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstWriteItem);
		}	
	}


void AgcmUIMailBox::RemoveItemFromReadGrid()
	{
	m_pAgpmGrid->Reset(&m_AgpdGridRead);
	if (m_pstReadItem)
		{
		m_pstReadItem->m_stUserData.m_pvData = &m_AgpdGridRead;
		m_pAgcmUIManager2->SetUserDataRefresh(m_pstReadItem);
		}
	}


BOOL AgcmUIMailBox::SetGridItemAttachedTexture(AgpdGridItem *pAgpdGridItem, AgpdItemTemplate *pAgpdItemTempalte)
	{
	if (!pAgpdGridItem)
		return FALSE;

	m_pAgcmItem->SetGridItemAttachedTexture(pAgpdItemTempalte);

	RwTexture **ppRwTexture = (RwTexture **)(m_pAgpmGrid->GetAttachedModuleData( 
			m_pAgcmUIControl->m_lItemGridTextureADDataIndex, pAgpdGridItem ));

	AgcdItemTemplate *pAgcdItemTemplate = m_pAgcmItem->GetTemplateData(pAgpdItemTempalte);
		
	(RwTexture *) *ppRwTexture = pAgcdItemTemplate->m_pTexture;

	return TRUE;		
	}

BOOL AgcmUIMailBox::IsOpenMailRead( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "MailRead" );
	if( !pcdUI ) return FALSE;

	if( pcdUI->m_eStatus == AGCDUI_STATUS_OPENING || pcdUI->m_eStatus == AGCDUI_STATUS_OPENED ) return TRUE;
	return FALSE;
}

BOOL AgcmUIMailBox::IsHaveNotReadMail( void )
{
	AgpmMailBox* ppmMailBox = ( AgpmMailBox* )g_pEngine->GetModule( "AgpmMailBox" );
	if( !ppmMailBox ) return FALSE;
	if( !m_pAgpdMailCADSelf ) return FALSE;

	AuAutoLock LockCAD( m_pAgpdMailCADSelf->m_Mutex );
	if( !LockCAD.Result() ) return FALSE;
	
	int nMailCount = m_pAgpdMailCADSelf->GetCount();
	for( int nCount = 0 ; nCount < nMailCount ; nCount++ )
	{
		int nMailID = m_pAgpdMailCADSelf->Get( nCount );
		AgpdMail* ppdMail = ppmMailBox->GetMail( nMailID );
	
		if( ppdMail && !ppdMail->GetRead() )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void AgcmUIMailBox::CheckHaveNotReadMail( void )
{
	if( IsHaveNotReadMail() )
	{
		AgcmChatting2* pcmChatting = ( AgcmChatting2* )g_pEngine->GetModule( "AgcmChatting2" );
		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
		if( pcmChatting && pcmUIManager )
		{
			char* pString = pcmUIManager->GetUIMessage( "YouHaveMail" );
			if( pString && strlen( pString ) > 0 )
			{
				pcmChatting->AddSystemMessage( pString );
			}
		}
	}
}
