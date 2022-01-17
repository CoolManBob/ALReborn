/*===========================================================================

	AgcmUIWantedCriminal.cpp

===========================================================================*/


#include "AgcmUIWantedCriminal.h"


/****************************************************************/
/*		The Implementation of AgcmUIWantedCriminal class		*/
/****************************************************************/
//
TCHAR AgcmUIWantedCriminal::s_szMessage[AGCMUIWANTEDCRIMINAL_MESSAGE_MAX][30] = 
	{
	_T("WC_ARRANGE"),			// AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE = 0,
	_T("WC_ARRANGE2"),			// AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE2,
	_T("WC_ARRANGE3"),			// AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE3,
	_T("WC_CANCELED"),			// AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE_CANCELED,
	_T("WC_SUCCEEDED"),			// AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE_SUCCEEDED,
	_T("WC_NOT_ENOUGH_BASE"),	// AGCMUIWANTEDCRIMINAL_MESSAGE_NOT_ENOUGH_BASE_MONEY,
	_T("WC_NOT_ENOUGH_MONEY"),	// AGCMUIWANTEDCRIMINAL_MESSAGE_NOT_ENOUGH_MONEY,	
	_T("WC_ALREADY_ARRANGED"),	// AGCMUIWANTEDCRIMINAL_MESSAGE_ALREADY_ARRANGED
	_T("WC_RANK_TOOLTIP1"),		// AGCMUIWANTEDCRIMINAL_MESSAGE_RANK_TOOLTIP1
	_T("WC_RANK_TOOLTIP2"),		// AGCMUIWANTEDCRIMINAL_MESSAGE_RANK_TOOLTIP2
	_T("WC_KILL"),				// AGCMUIWANTEDCRIMINAL_MESSAGE_KILL_WC,
	_T("WC_DEAD"),				// AGCMUIWANTEDCRIMINAL_MESSAGE_DEAD_WC,
	_T("WC_UNDERBASE")			// AGCMUIWANTEDCRIMINAL_MESSAGE_UNDER_BASE_MONEY
	};


TCHAR AgcmUIWantedCriminal::s_szEvent[AGCMUIWANTEDCRIMINAL_EVENT_MAX][30] =
	{
	_T("WC_OPEN_ARRANGE1"),			//	AGCMUIWANTEDCRIMINAL_EVENT_OPEN_ARRANGE1 = 0,
	_T("WC_OPEN_ARRANGE2"),			//	AGCMUIWANTEDCRIMINAL_EVENT_OPEN_ARRANGE2,
	_T("WC_OPEN_ARRANGE3"),			//	AGCMUIWANTEDCRIMINAL_EVENT_OPEN_ARRANGE3,
	_T("WC_OPEN_BOARD"),			//	AGCMUIWANTEDCRIMINAL_EVENT_OPEN_RANKING_BOARD,
	_T("WC_CLOSE_BOARD"),			//	AGCMUIWANTEDCRIMINAL_EVENT_CLOSE_RANKING_BOARD
	_T("WC_CHAR_ONLINE"),			//	AGCMUIWANTEDCRIMINAL_EVENT_CHAR_ONLINE,
	_T("WC_CHAR_OFFLINE"),			//	AGCMUIWANTEDCRIMINAL_EVENT_CHAR_OFFLINE,
	_T("WC_PAGE_01"),				//	AGCMUIWANTEDCRIMINAL_EVENT_PAGE01_ON,
	_T("WC_PAGE_02"),				//	AGCMUIWANTEDCRIMINAL_EVENT_PAGE02_ON,
	_T("WC_PAGE_03"),				//	AGCMUIWANTEDCRIMINAL_EVENT_PAGE03_ON,
	_T("WC_PAGE_04"),				//	AGCMUIWANTEDCRIMINAL_EVENT_PAGE04_ON,
	_T("WC_PAGE_05"),				//	AGCMUIWANTEDCRIMINAL_EVENT_PAGE05_ON,
	_T("WC_PAGE_06"),				//	AGCMUIWANTEDCRIMINAL_EVENT_PAGE06_ON,
	_T("WC_PAGE_07"),				//	AGCMUIWANTEDCRIMINAL_EVENT_PAGE07_ON,
	_T("WC_PAGE_08"),				//	AGCMUIWANTEDCRIMINAL_EVENT_PAGE08_ON,
	_T("WC_PAGE_09"),				//	AGCMUIWANTEDCRIMINAL_EVENT_PAGE09_ON,
	_T("WC_PAGE_10")				//	AGCMUIWANTEDCRIMINAL_EVENT_PAGE10_ON,
	};




AgcmUIWantedCriminal::AgcmUIWantedCriminal()
	{
	SetModuleName(_T("AgcmUIWantedCriminal"));
	EnableIdle(TRUE);

	m_pstDummy = NULL;
	m_pstBoard = NULL;
	m_pstArrangeEnable = NULL;
	m_lArrageEnable = 0;

	m_llBounty = 0;

	m_WantedCriminals.MemSetAll();

	for (INT32 i = 0; i < AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE; i++)
		{
		m_WantedCriminals[i].Init();
		m_lBoard[i] = i;
		}
	
	m_lRank = -1;
	m_llMyBounty = 0;
	m_ulPreviousClock = 0;
	m_pAgpdGridItemAlarm = NULL;
	
	ResetBoard();
	}


AgcmUIWantedCriminal::~AgcmUIWantedCriminal()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgcmUIWantedCriminal::OnAddModule()
	{
	m_pAgpmGrid = (AgpmGrid *) GetModule(_T("AgpmGrid"));
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmWantedCriminal = (AgpmWantedCriminal *) GetModule(_T("AgpmWantedCriminal"));
	
	m_pAgcmCharacter = (AgcmCharacter *) GetModule(_T("AgcmCharacter"));
	m_pAgcmUIManager2 = (AgcmUIManager2 *) GetModule(_T("AgcmUIManager2"));
	m_pAgcmUIControl = (AgcmUIControl *) GetModule(_T("AgcmUIControl"));
	m_pAgcmUIMain = (AgcmUIMain *) GetModule(_T("AgcmUIMain"));
	m_pAgcmEventManager	= (AgcmEventManager *) GetModule(_T("AgcmEventManager"));
	m_pAgcmChatting2 = (AgcmChatting2 *) GetModule(_T("AgcmChatting2"));

	if (!m_pAgpmCharacter || !m_pAgpmWantedCriminal
		|| !m_pAgcmCharacter || !m_pAgcmUIManager2 || !m_pAgcmUIControl || !m_pAgcmUIMain
		|| !m_pAgcmEventManager || !m_pAgcmChatting2
		)
		return FALSE;	

	// module
	if (FALSE == m_pAgpmWantedCriminal->SetCallbackArrange(CBResultArrange, this))
		return FALSE;
	if (FALSE == m_pAgpmWantedCriminal->SetCallbackResponseList(CBResultList, this))
		return FALSE;
	if (FALSE == m_pAgpmWantedCriminal->SetCallbackMyRank(CBResultMyRank, this))
		return FALSE;
	if (FALSE == m_pAgpmWantedCriminal->SetCallbackUpdateKiller(CBUpdateKiller, this))
		return FALSE;
	if (FALSE == m_pAgpmWantedCriminal->SetCallbackUpdateFlag(CBUpdateFlag, this))
		return FALSE;
	if (FALSE == m_pAgpmWantedCriminal->SetCallbackNotify(CBNotify, this))
		return FALSE;
	if (FALSE == m_pAgcmCharacter->SetCallbackReleaseSelfCharacter(CBReleaseCharacter, this))
		return FALSE;

	// event
	if (FALSE == m_pAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_WANTEDCRIMINAL, CBRequestEvent, this))
		return FALSE;	
	if (FALSE == m_pAgpmWantedCriminal->SetCallbackEventGrant(CBGrant, this))
		return FALSE;
	if (FALSE == m_pAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	// Main UI callback
	if (FALSE == m_pAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	// UI
	if (!AddEvent() || !AddFunction() || !AddDisplay() || !AddUserData())
		return FALSE;
	
	if (!m_pAgcmUIManager2->AddBoolean(this, _T("WC_ACTIVE_ARRANGE"), CBIsActiveArrange, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	
	if (m_pAgpmGrid)
		m_pAgpdGridItemAlarm = m_pAgpmGrid->CreateGridItem();
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::OnInit()
	{
  #ifdef _DEBUG
	AS_REGISTER_TYPE_BEGIN(AgcmUIWantedCriminal, AgcmUIWantedCriminal);
		AS_REGISTER_METHOD0(void, TestOpenBoard);
		AS_REGISTER_METHOD0(void, TestOpenArrange);
	AS_REGISTER_TYPE_END;
  #endif
  
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::OnIdle(UINT32 ulClockCount)
	{
	if (ulClockCount > m_ulPreviousClock + AGCMUIWANTEDCRIMINAL_UPDATE_RANK_INTERVAL)
		{
		m_ulPreviousClock = ulClockCount;
		AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
		if (pAgpdCharacter && pAgpdCharacter->m_bIsWantedCriminal)
			SendRequestMyRank(pAgpdCharacter);
		}
	
	return TRUE;
	}




//	OnAddModule helper
//=================================================================
//
BOOL AgcmUIWantedCriminal::AddEvent()
	{
	for (int i=0; i < AGCMUIWANTEDCRIMINAL_EVENT_MAX; ++i)
		{
		m_lEvent[i] = m_pAgcmUIManager2->AddEvent(s_szEvent[i]);
		if (m_lEvent[i] < 0)
			return FALSE;
		}

	return TRUE;	
	}


BOOL AgcmUIWantedCriminal::AddFunction()
	{
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_ARRANGE_MENU"), CBClickArrangeOnMenu, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_ARRANGE1_OK"), CBClickArrange1OK, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_ARRANGE2_OK"), CBClickArrange2OK, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_ARRANGE3_OK"), CBClickArrange3OK, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_ARRANGE_CANCEL"), CBClickArrangeCancel, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_LEFT"), CBClickBoardLeft, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_RIGHT"), CBClickBoardRight, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_PAGE01"), CBClickBoardPage01, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_PAGE02"), CBClickBoardPage02, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_PAGE03"), CBClickBoardPage03, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_PAGE04"), CBClickBoardPage04, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_PAGE05"), CBClickBoardPage05, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_PAGE06"), CBClickBoardPage06, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_PAGE07"), CBClickBoardPage07, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_PAGE08"), CBClickBoardPage08, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_PAGE09"), CBClickBoardPage09, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_PAGE10"), CBClickBoardPage10, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_REFRESH"), CBClickBoardRefresh, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("WC_BOARD_CLOSE"), CBClickBoardClose, 0))
		return FALSE;
		
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::AddDisplay()
	{
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_ARRANGE_DESC"), 0, CBDisplayArrangeDesc, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;	
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_ARRANGE_DESC2"), 0, CBDisplayArrangeDesc2, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;			
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_ARRANGE_DESC3"), 0, CBDisplayArrangeDesc3, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;					
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_ARRANGE_CHARID"), 0, CBDisplayArrangeCharID, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_ARRANGE_TGHELD"), 0, CBDisplayArrangeTotalGheld, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_ARRANGE_BOUNTY"), 0, CBDisplayArrangeBounty, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_BOARD_RANK"), 0, CBDisplayBoardRank, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_BOARD_WANTED"), 0, CBDisplayBoardWanted, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_BOARD_BOUNTY"), 0, CBDisplayBoardBounty, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_BOARD_LEVEL"), 0, CBDisplayBoardLevel, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("WC_BOARD_CLASS"), 0, CBDisplayBoardClass, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
		
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::AddUserData()
	{
	m_pstDummy = m_pAgcmUIManager2->AddUserData(_T("WC_UD_DUMMY"), NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstDummy)
		return FALSE;
	m_pstBoard = m_pAgcmUIManager2->AddUserData(_T("WC_UD_BOARD"), m_lBoard, sizeof(INT32), AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstBoard)
		return FALSE;
	m_pstArrangeEnable = m_pAgcmUIManager2->AddUserData(_T("WC_UD_ARR_ENABLE"), &m_lArrageEnable, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstArrangeEnable)
		return FALSE;

	return TRUE;
	}




//	Operation
//===========================================================
//
BOOL AgcmUIWantedCriminal::OpenBoard()
	{
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	OnClickPage(0);
	
	m_stOpenPos	= pAgpdCharacter->m_stPos;
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIWANTEDCRIMINAL_EVENT_OPEN_RANKING_BOARD]);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstBoard);
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::OpenArrange()
	{
	// get wanted criminal
	TCHAR *pszKiller = m_pAgpmWantedCriminal->GetFinalAttacker(m_pAgcmCharacter->GetSelfCharacter());
	if (NULL == pszKiller || 0 >= _tcslen(pszKiller))
		return FALSE;
	
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIWANTEDCRIMINAL_EVENT_OPEN_ARRANGE1]);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstDummy);
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::OnClickPage(INT32 lPage, BOOL bRefresh)
	{
	if (0 > lPage || lPage >= AGPMWANTEDCRIMINAL_MAX_PAGE)
		return FALSE;
	
	if (!bRefresh && m_lPage == lPage)	// ignore
		return FALSE;
	
	m_lPage = lPage;
	
	m_lList = 0;
	for (INT32 i=0; i < AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE; i++)
		{
		m_WantedCriminals[i].Reset();
		}
	
	return SendRequstList(m_pAgcmCharacter->GetSelfCharacter(), lPage);
	}


#ifdef _DEBUG

void AgcmUIWantedCriminal::TestOpenBoard()
	{
	OpenBoard();
	}


void AgcmUIWantedCriminal::TestOpenArrange()
	{
	AgpdCharacter *pAgpdCharacterSelf = m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacterSelf)
		return;
	
	m_pAgpmWantedCriminal->SetFinalAttacker(pAgpdCharacterSelf, _T("ABCD"));
	
	OpenArrange();
	}

#endif


//	Packet Send
//============================================
//
BOOL AgcmUIWantedCriminal::SendRequestEvent(AgpdCharacter *pAgpdCharacter, ApdEvent *pApdEvent)
	{
	if (NULL == pAgpdCharacter || NULL == pApdEvent)
		return FALSE;

	INT16 nPacketLength	= 0;
	PVOID pvPacket = m_pAgpmWantedCriminal->MakePacketEventRequest(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);

	return	bResult;
	}


BOOL AgcmUIWantedCriminal::SendArrange(AgpdCharacter *pAgpdCharacter, TCHAR *pszCharID, INT64 llMoney)
	{
	if (NULL == pAgpdCharacter || NULL == pszCharID)
		return FALSE;

	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_ARRANGE;
	
	PVOID pvPacketEmb = m_pAgpmWantedCriminal->m_csPacketWC.MakePacket(FALSE, &nPacketLength, 0,
												pszCharID,		// char id
												&llMoney,		// money
												NULL,			// level
												NULL,			// class
												NULL			// online
												);

	if (NULL == pvPacketEmb || 0 >= nPacketLength)
		return FALSE;

	PVOID pvPacket = m_pAgpmWantedCriminal->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
																&cOperation,				// operation
																&pAgpdCharacter->m_lID,		// CID
																NULL,						// page
																NULL,						// rank
																NULL,						// result
																pvPacketEmb					// embedded
																);

	m_pAgpmWantedCriminal->m_csPacketWC.FreePacket(pvPacketEmb);
	
	if (NULL == pvPacket || 0 >= nPacketLength)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);
	
	return bResult;	
	}


BOOL AgcmUIWantedCriminal::SendRequstList(AgpdCharacter *pAgpdCharacter, INT32 lPage)
	{
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_REQ_LIST;
	PVOID pvPacket = m_pAgpmWantedCriminal->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
																&cOperation,				// operation
																&pAgpdCharacter->m_lID,		// CID
																&lPage,						// page
																NULL,						// rank
																NULL,						// result
																NULL						// embedded
																);
	if (NULL == pvPacket || 0 >= nPacketLength)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
	}


BOOL AgcmUIWantedCriminal::SendRequestMyRank(AgpdCharacter *pAgpdCharacter)
	{
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_MY_RANK;
	PVOID pvPacket = m_pAgpmWantedCriminal->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
																&cOperation,				// operation
																&pAgpdCharacter->m_lID,		// CID
																NULL,						// page
																NULL,						// rank
																NULL,						// result
																NULL						// embedded
																);
	if (NULL == pvPacket || 0 >= nPacketLength)
		return FALSE;
	
	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmWantedCriminal->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
	}




//	Module Callback
//============================================
//
BOOL AgcmUIWantedCriminal::CBResultArrange(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdWantedCriminalParam *pParam = (AgpdWantedCriminalParam *) pCustData;

	AgpdCharacter *pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacterSelf)
		return FALSE;
	
	switch (pParam->m_lResult)
		{
		case AGPMWANTEDCRIMINAL_RESULT_SUCCESS :
			{
			TCHAR sz[256];
			ZeroMemory(sz, sizeof(sz));
			TCHAR *psz = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE_SUCCEEDED);
			if (psz)
			{
				if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
					_stprintf(sz, psz, pThis->m_llBounty, pThis->m_pAgpmWantedCriminal->GetFinalAttacker(pAgpdCharacterSelf));
				else
					_stprintf(sz, psz, pThis->m_pAgpmWantedCriminal->GetFinalAttacker(pAgpdCharacterSelf), pThis->m_llBounty);
			}

			pThis->m_pAgpmWantedCriminal->SetFinalAttacker(pAgpdCharacterSelf, NULL);
			pThis->m_llBounty = 0;
			SystemMessage.ProcessSystemMessage(sz);
			
			pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstArrangeEnable);
			}
			break;
		
		case AGPMWANTEDCRIMINAL_RESULT_FAIL_NOT_ENOUGH_BASE_MONEY :
			{
			TCHAR sz[256];
			TCHAR *psz = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_NOT_ENOUGH_BASE_MONEY);
			if (psz)
				{
				_stprintf(sz, psz, pThis->m_pAgpmWantedCriminal->GetBaseBounty(pAgpdCharacterSelf));
				SystemMessage.ProcessSystemMessage(sz);
				}
			}
			break;
			
		case AGPMWANTEDCRIMINAL_RESULT_FAIL_NOT_ENOUGH_MONEY :
			SystemMessage.ProcessSystemMessage(pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_NOT_ENOUGH_MONEY));
			break;
			
		case AGPMWANTEDCRIMINAL_RESULT_FAIL_ALREADY_ARRANGED :
			SystemMessage.ProcessSystemMessage(pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_ALREADY_ARRANGED));
			break;
		}
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBResultList(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdWantedCriminalParam *pParam = (AgpdWantedCriminalParam *) pCustData;
	
	if (AGPMWANTEDCRIMINAL_END_OF_LIST == pParam->m_lPage)
		{
		// regresh UI
		pThis->m_pstBoard->m_stUserData.m_lCount = pThis->m_lList;
		pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstBoard);
		pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIWANTEDCRIMINAL_EVENT_PAGE_01 + pThis->m_lPage]);
		
		for (INT32 i=0; i < pThis->m_pstBoard->m_stUserData.m_lCount; i++)
			{
			if (pThis->m_WantedCriminals[i].m_bOnline)
				pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIWANTEDCRIMINAL_EVENT_CHAR_ONLINE], i);
			else
				pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIWANTEDCRIMINAL_EVENT_CHAR_OFFLINE], i);
			}
		
		pThis->m_lList = 0;
		return TRUE;
		}
	
	if (pParam->m_lPage != pThis->m_lPage)
		return FALSE;
	
	if (NULL == pParam->m_pAgpdWantedCriminal)
		return FALSE;
	
	INT32 lIndex = pParam->m_lRank - (pThis->m_lPage * 10) - 1;
	if (0 > lIndex || lIndex >= AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE)
		return FALSE;
	
	pThis->m_lList++;
	
	// update
	pThis->m_WantedCriminals[lIndex].m_Mutex.Lock();
	pThis->m_WantedCriminals[lIndex].Reset();
	pThis->m_WantedCriminals[lIndex] = *(pParam->m_pAgpdWantedCriminal);
	pThis->m_WantedCriminals[lIndex].m_Mutex.Unlock();

	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBResultMyRank(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdWantedCriminalParam *pParam = (AgpdWantedCriminalParam *) pCustData;
	
	if (pThis->m_pAgpdGridItemAlarm && pParam->m_lRank > 0)
		{
		pThis->m_lRank = pParam->m_lRank;
		pThis->m_llMyBounty = pParam->m_pAgpdWantedCriminal->m_llMoney;

		TCHAR szTooltip[1024];
		TCHAR *pszTooltip = szTooltip;
		TCHAR *psz = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_RANK_TOOLTIP1);
		if (psz)
			pszTooltip += _stprintf(pszTooltip, psz, pThis->m_lRank);

		psz = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_RANK_TOOLTIP2);
		if (psz)
			{
			pszTooltip += _stprintf(pszTooltip, _T("  "));
			_stprintf(pszTooltip, psz, pThis->m_llMyBounty);
			}
			
		// update tooltip text
		pThis->m_pAgpdGridItemAlarm->SetTooltip(szTooltip);
		}
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBUpdateKiller(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	TCHAR *pszKiller = (TCHAR *) pCustData;

	pThis->m_pAgpmWantedCriminal->SetFinalAttacker(pAgpdCharacter, pszKiller);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstArrangeEnable);
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBUpdateFlag(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	BOOL bIsWantedCriminal = *((BOOL *) pCustData);

	if (pThis->m_pAgcmCharacter->GetSelfCharacter() == pAgpdCharacter)
		{
		pAgpdCharacter->m_bIsWantedCriminal = bIsWantedCriminal;

		if (NULL == pThis->m_pAgpdGridItemAlarm)
			return TRUE;
		
		if (bIsWantedCriminal)
			{
			TCHAR *pszTextureName = pThis->m_pAgcmUIControl->GetCustomTexture(AGCMUIWANTEDCRIMINAL_ALARM_ICON);
			RwTexture **ppTexture = pThis->m_pAgcmUIControl->GetAttachGridItemTextureData(pThis->m_pAgpdGridItemAlarm);

			if (!ppTexture || !pszTextureName)
				return FALSE;

			if (!*ppTexture)
				*ppTexture = RwTextureRead(pszTextureName, NULL);

			pThis->m_pAgpdGridItemAlarm->SetTooltip( "현상수배" );
			pThis->m_pAgcmUIMain->AddSystemMessageGridItem(pThis->m_pAgpdGridItemAlarm, NULL, pThis);
			}
		else
			{
			pThis->m_lRank = -1;
			pThis->m_llMyBounty = 0;
			pThis->m_pAgcmUIMain->RemoveSystemMessageGridItem(pThis->m_pAgpdGridItemAlarm);
			}
		}
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBNotify(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdWantedCriminalParam *pParam = (AgpdWantedCriminalParam *) pCustData;
	
	AgpdCharacter *pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacterSelf || pAgpdCharacterSelf != pAgpdCharacter)
		return FALSE;
	
	if (NULL == pParam->m_pAgpdWantedCriminal
		|| 0 >= _tcslen(pParam->m_pAgpdWantedCriminal->m_szCharID))
		return FALSE;
	
	TCHAR *psz = NULL;
	switch (pParam->m_lResult)
		{
		case AGPMWANTEDCRIMINAL_RESULT_KILL_WANTED_CRIMINAL :
			psz = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_KILL_WC);
			break;
		
		case AGPMWANTEDCRIMINAL_RESULT_DEAD_WANTED_CRIMINAL :
			psz = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_DEAD_WC);
			break;
		
		default :
			break;
		}
		
	if (psz)
		{
		TCHAR sz[1024];
		ZeroMemory(sz, sizeof(sz));
		_stprintf(sz, psz, pParam->m_pAgpdWantedCriminal->m_szCharID, pParam->m_pAgpdWantedCriminal->m_llMoney);
		SystemMessage.ProcessSystemMessage(sz);
		}
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBReleaseCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;

	// 2007.04.04. laki.
	// grid item removed when use transformation
	//pThis->m_pAgcmUIMain->RemoveSystemMessageGridItem(pThis->m_pAgpdGridItemAlarm);
	
	return TRUE;		
	}




//	Event Callback
//============================================
//
BOOL AgcmUIWantedCriminal::CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIWantedCriminal	*pThis			= (AgcmUIWantedCriminal *)		pClass;
	ApdEvent				*pApdEvent		= (ApdEvent *)					pData;
	ApBase					*pcsGenerator	= (ApBase *)					pCustData;

	if (pcsGenerator->m_eType == APBASE_TYPE_CHARACTER &&
		pcsGenerator->m_lID	== pThis->m_pAgcmCharacter->GetSelfCID())
		{
		if (pThis->m_pAgpmCharacter->IsActionBlockCondition(pThis->m_pAgcmCharacter->m_pcsSelfCharacter))
			return FALSE;

		return pThis->SendRequestEvent(pThis->m_pAgcmCharacter->m_pcsSelfCharacter, pApdEvent);
		}

	return TRUE;	
	}


BOOL AgcmUIWantedCriminal::CBGrant(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pCustData || !pClass)
		return FALSE;

	AgcmUIWantedCriminal	*pThis = (AgcmUIWantedCriminal *) pClass;
	AgpdCharacter			*pAgpdCharacter	= (AgpdCharacter *)	pData;
	
	if (pAgpdCharacter != pThis->m_pAgcmCharacter->GetSelfCharacter())
		return FALSE;
	
	// open board
	return pThis->OpenBoard();
	}


BOOL AgcmUIWantedCriminal::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIWantedCriminal	*pThis = (AgcmUIWantedCriminal *) pClass;
	AgpdCharacter			*pAgpdCharacter = (AgpdCharacter *) pData;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pAgpdCharacter->m_stPos, pThis->m_stOpenPos);
	if ((INT32) fDistance < 150)
		return TRUE;

	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIWANTEDCRIMINAL_EVENT_CLOSE_RANKING_BOARD]);
	pThis->ResetBoard();

	return TRUE;
	}




//	Main UI Callback
//================================================
//
BOOL AgcmUIWantedCriminal::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	
	// reset
	pThis->ResetBoard();
	
	return TRUE;
	}




//	UI Function Callback
//================================================
//
//	==========	Arrange	==========
//
BOOL AgcmUIWantedCriminal::CBClickArrangeOnMenu(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal	*pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OpenArrange();
	}

	
BOOL AgcmUIWantedCriminal::CBClickArrange1OK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal	*pThis = (AgcmUIWantedCriminal *) pClass;

	AgpdCharacter *pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacterSelf)
		return FALSE;
	
	// validate
	if (!pThis->m_pAgpmWantedCriminal->IsEnoughBaseBounty(pAgpdCharacterSelf))
		{
		TCHAR sz[256];
		TCHAR *psz = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_NOT_ENOUGH_BASE_MONEY);
		if (psz)
			{
			_stprintf(sz, psz, pThis->m_pAgpmWantedCriminal->GetBaseBounty(pAgpdCharacterSelf));
			SystemMessage.ProcessSystemMessage(sz);
			}
		return FALSE;
		}
	
	pThis->m_llBounty = pThis->m_pAgpmWantedCriminal->GetBaseBounty(pAgpdCharacterSelf);
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIWANTEDCRIMINAL_EVENT_OPEN_ARRANGE2]);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstDummy);
		
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBClickArrange2OK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIWantedCriminal	*pThis = (AgcmUIWantedCriminal *) pClass;
	AgcdUIControl			*pControlMoney = (AgcdUIControl *) pData1;

	if (AcUIBase::TYPE_EDIT != pControlMoney->m_lType || NULL == pControlMoney->m_pcsBase)
		return FALSE;

	TCHAR *pszMoney = (TCHAR *) ((AcUIEdit *)(pControlMoney->m_pcsBase))->GetText();
	INT64 llMoney = _ttoi64(pszMoney);
	
	AgpdCharacter *pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacterSelf)
		return FALSE;
	
	// validate
	if (!pThis->m_pAgpmWantedCriminal->IsEnoughBaseBounty(pAgpdCharacterSelf))
		{
		TCHAR sz[256];
		TCHAR *psz = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_NOT_ENOUGH_BASE_MONEY);
		if (psz)
			{
			_stprintf(sz, psz, pThis->m_pAgpmWantedCriminal->GetBaseBounty(pAgpdCharacterSelf));
			SystemMessage.ProcessSystemMessage(sz);
			}
		return FALSE;
		}

	if (pThis->m_pAgpmWantedCriminal->GetBaseBounty(pAgpdCharacterSelf) > llMoney)
		{
		TCHAR sz[256];
		TCHAR *psz = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_UNDER_BASE_MONEY);
		if (psz)
			{
			_stprintf(sz, psz, pThis->m_pAgpmWantedCriminal->GetBaseBounty(pAgpdCharacterSelf));
			SystemMessage.ProcessSystemMessage(sz);
			}
		return FALSE;
		}
	
	if (!pThis->m_pAgpmWantedCriminal->IsEnoughBounty(pAgpdCharacterSelf, llMoney))
		{
		SystemMessage.ProcessSystemMessage(pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_NOT_ENOUGH_MONEY));
		return FALSE;
		}
	
	pThis->m_llBounty = llMoney;
	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIWANTEDCRIMINAL_EVENT_OPEN_ARRANGE3]);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstDummy);
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBClickArrange3OK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;

	AgpdCharacter *pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacterSelf)
		return FALSE;

	// final check
	TCHAR *pszKiller = pThis->m_pAgpmWantedCriminal->GetFinalAttacker(pAgpdCharacterSelf);
	if (NULL == pszKiller || 0 >= _tcslen(pszKiller))
		return FALSE;

	// validate
	if (!pThis->m_pAgpmWantedCriminal->IsEnoughBaseBounty(pAgpdCharacterSelf))
		return FALSE;

	if (pThis->m_pAgpmWantedCriminal->GetBaseBounty(pAgpdCharacterSelf) > pThis->m_llBounty)
		return FALSE;
	
	if (!pThis->m_pAgpmWantedCriminal->IsEnoughBounty(pAgpdCharacterSelf, pThis->m_llBounty))	
		return FALSE;
	
	// send packet
	return pThis->SendArrange(pAgpdCharacterSelf, pszKiller, pThis->m_llBounty);
	}


BOOL AgcmUIWantedCriminal::CBClickArrangeCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;

	SystemMessage.ProcessSystemMessage(pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE_CANCELED));
	
	pThis->m_llBounty = 0;
	
	return TRUE;
	}




//
//	==========	Board	==========
//
BOOL AgcmUIWantedCriminal::CBClickBoardLeft(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	
	INT32 lPage = pThis->m_lPage;
	lPage--;
	if (0 > lPage)
		lPage = 0;
	
	return pThis->OnClickPage(lPage);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardRight(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;

	INT32 lPage = pThis->m_lPage;
	lPage++;
	
	if (lPage >= AGPMWANTEDCRIMINAL_MAX_PAGE)
		lPage = AGPMWANTEDCRIMINAL_MAX_PAGE - 1;

	return pThis->OnClickPage(lPage);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardPage01(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OnClickPage(0);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardPage02(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OnClickPage(1);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardPage03(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OnClickPage(2);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardPage04(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OnClickPage(3);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardPage05(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OnClickPage(4);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardPage06(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OnClickPage(5);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardPage07(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OnClickPage(6);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardPage08(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OnClickPage(7);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardPage09(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OnClickPage(8);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardPage10(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	return pThis->OnClickPage(9);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardRefresh(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
		
	return pThis->OnClickPage(pThis->m_lPage, TRUE);
	}


BOOL AgcmUIWantedCriminal::CBClickBoardClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	
	pThis->ResetBoard();
	
	return TRUE;
	}




//	Display Callback
//================================================
//
//	==========	Arrange	==========
//
BOOL AgcmUIWantedCriminal::CBDisplayArrangeDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || NULL == pcsSourceControl->m_pcsBase)
		return FALSE;
	
	TCHAR *pszFormat = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE);
	TCHAR *pszKiller = pThis->m_pAgpmWantedCriminal->GetFinalAttacker(pThis->m_pAgcmCharacter->GetSelfCharacter());
	if (pszFormat && pszKiller)
		{
		TCHAR sz[200];
		ZeroMemory(sz, sizeof(sz));
		_stprintf(sz, pszFormat, pszKiller);
		((AcUIEdit *)(pcsSourceControl->m_pcsBase))->SetText(sz);
		}
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBDisplayArrangeDesc2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || NULL == pcsSourceControl->m_pcsBase)
		return FALSE;
	
	TCHAR *pszFormat = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE2);
	TCHAR *pszKiller = pThis->m_pAgpmWantedCriminal->GetFinalAttacker(pThis->m_pAgcmCharacter->GetSelfCharacter());
	if (pszFormat && pszKiller)
		{
		TCHAR sz[200];
		ZeroMemory(sz, sizeof(sz));
		_stprintf(sz, pszFormat, pszKiller);
		((AcUIEdit *)(pcsSourceControl->m_pcsBase))->SetText(sz);
		}
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBDisplayArrangeDesc3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || NULL == pcsSourceControl->m_pcsBase)
		return FALSE;
	
	TCHAR *pszFormat = pThis->GetMessageTxt(AGCMUIWANTEDCRIMINAL_MESSAGE_ARRANGE3);
	TCHAR *pszKiller = pThis->m_pAgpmWantedCriminal->GetFinalAttacker(pThis->m_pAgcmCharacter->GetSelfCharacter());
	if (pszFormat && pszKiller)
		{
		TCHAR sz[200];
		ZeroMemory(sz, sizeof(sz));
		_stprintf(sz, pszFormat, pszKiller);
		((AcUIEdit *)(pcsSourceControl->m_pcsBase))->SetText(sz);
		}
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBDisplayArrangeCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	
	TCHAR *pszKiller = pThis->m_pAgpmWantedCriminal->GetFinalAttacker(pThis->m_pAgcmCharacter->GetSelfCharacter());
	if (pszKiller)
		_tcscpy(szDisplay, pszKiller);
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBDisplayArrangeTotalGheld(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	
	AgpdCharacter *pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (pAgpdCharacterSelf)
		{
		INT64 llTotal = pAgpdCharacterSelf->m_llMoney + pAgpdCharacterSelf->m_llBankMoney;
		_stprintf(szDisplay, _T("%I64d"), llTotal);
		}
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBDisplayArrangeBounty(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || NULL == pcsSourceControl->m_pcsBase)
		return FALSE;
	
	if (0 >= pThis->m_llBounty)
		return FALSE;

	TCHAR sz[100];
	_stprintf(sz, _T("%I64d"), pThis->m_llBounty);

	((AcUIEdit *)(pcsSourceControl->m_pcsBase))->SetText(sz);
		
	return TRUE;
	}


//
//	==========	Board	==========
//
BOOL AgcmUIWantedCriminal::CBDisplayBoardRank(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	INT32 lIndex = *((INT32 *)pData);
	
	if (0 > lIndex || lIndex >= AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE)
		return FALSE;
	
	if (0 > pThis->m_lPage || pThis->m_lPage >= AGPMWANTEDCRIMINAL_MAX_PAGE)
		return FALSE;
	
	INT32 lRank = ((pThis->m_lPage) * 10) + lIndex + 1;
	
	_stprintf(szDisplay, _T("%d"), lRank);
			
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBDisplayBoardWanted(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	INT32 lIndex = *((INT32 *)pData);
	
	if (0 > lIndex || lIndex >= AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE)
		return FALSE;
	
	if (0 > pThis->m_lPage || pThis->m_lPage >= AGPMWANTEDCRIMINAL_MAX_PAGE)
		return FALSE;
	
	_tcscpy(szDisplay, pThis->m_WantedCriminals[lIndex].m_szCharID);
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBDisplayBoardBounty(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	INT32 lIndex = *((INT32 *)pData);
	
	if (0 > lIndex || lIndex >= AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE)
		return FALSE;
	
	if (0 > pThis->m_lPage || pThis->m_lPage >= AGPMWANTEDCRIMINAL_MAX_PAGE)
		return FALSE;
	
	_stprintf(szDisplay, _T("%I64d"), pThis->m_WantedCriminals[lIndex].m_llMoney);
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBDisplayBoardLevel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	INT32 lIndex = *((INT32 *)pData);
	
	if (0 > lIndex || lIndex >= AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE)
		return FALSE;
	
	if (0 > pThis->m_lPage || pThis->m_lPage >= AGPMWANTEDCRIMINAL_MAX_PAGE)
		return FALSE;
	
	if (0 != pThis->m_WantedCriminals[lIndex].m_lLevel)
		_stprintf(szDisplay, _T("%d"), pThis->m_WantedCriminals[lIndex].m_lLevel);
	
	return TRUE;
	}


BOOL AgcmUIWantedCriminal::CBDisplayBoardClass(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIWantedCriminal *pThis = (AgcmUIWantedCriminal *) pClass;
	INT32 lIndex = *((INT32 *)pData);
	
	if (0 > lIndex || lIndex >= AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE)
		return FALSE;
	
	if (0 > pThis->m_lPage || pThis->m_lPage >= AGPMWANTEDCRIMINAL_MAX_PAGE)
		return FALSE;

	TCHAR *psz = NULL;
	switch (pThis->m_WantedCriminals[lIndex].m_lClass)
		{
		case 96 :
			psz = pThis->m_pAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_KNIGHT);
			break;
		
		case 1 :	
			psz = pThis->m_pAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_ARCHER);
			break;
		
		case 6 :	
			psz = pThis->m_pAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_MAGE);
			break;
		
		case 4 :	
			psz = pThis->m_pAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_BERSERKER);
			break;
		
		case 8 :	
			psz = pThis->m_pAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_HUNTER);
			break;
		
		case 3 :	
			psz = pThis->m_pAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SORCERER);
			break;
		
		case 460 :	
			psz = pThis->m_pAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_RANGER);
			break;
		
		case 9 :	
			psz = pThis->m_pAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_ELEMENTALIST);
			break;
		
		default :
			break;
		}

	if (psz)
		_tcscpy(szDisplay, psz);

	return TRUE;
	}




//	Boolean Callback
//================================================
//
BOOL AgcmUIWantedCriminal::CBIsActiveArrange(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUIWantedCriminal *pThis	= (AgcmUIWantedCriminal *)	pClass;	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;

	TCHAR *psz = pThis->m_pAgpmWantedCriminal->GetFinalAttacker(pAgpdCharacter);
	
	if (NULL == psz || 0 >= _tcslen(psz))
		return FALSE;
	
	return TRUE;
	}




//	Helper
//================================================
//
TCHAR* AgcmUIWantedCriminal::GetMessageTxt(eAGCMUIWANTEDCRIMINAL_MESSAGE eMessage)
	{
	if (0 > eMessage || eMessage > AGCMUIWANTEDCRIMINAL_MESSAGE_MAX)
		return NULL;

	return m_pAgcmUIManager2->GetUIMessage(s_szMessage[eMessage]);
	}


void AgcmUIWantedCriminal::AddSystemMessage(TCHAR *pszMessage)
	{
	if (!pszMessage)
		return;

	AgpdChatData	stChatData;
	ZeroMemory(&stChatData, sizeof(stChatData));

	stChatData.eChatType = AGPDCHATTING_TYPE_SYSTEM_LEVEL1;
	stChatData.szMessage = pszMessage;

	stChatData.lMessageLength = _tcslen(stChatData.szMessage);
		
	m_pAgcmChatting2->AddChatMessage(AGCMCHATTING_TYPE_SYSTEM, &stChatData);
	m_pAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
	}


void AgcmUIWantedCriminal::ResetBoard()
	{
	ZeroMemory(&m_stOpenPos, sizeof(m_stOpenPos));
	m_lPage = -1;
	m_lList = 0;
	
	for (INT32 i = 0; i < AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE; i++)
		{
		m_WantedCriminals[i].Reset();
		}
	}

