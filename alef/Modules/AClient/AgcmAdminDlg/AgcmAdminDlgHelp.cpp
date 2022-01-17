// AgcmAdminDlgHelp.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 10. 19.

#include "AgcmAdminDlgInclude.h"
#include "resource.h"

AgcmAdminDlgHelp::AgcmAdminDlgHelp()
{
	m_iResourceID = IDD_HELP;
	m_pfDlgProc = HelpDlgProc;

	m_pfCBHelpComplete = NULL;
	m_pfCBCharInfo = NULL;
	m_pfCBChatting = NULL;

	m_nSelectedIndex = -1;

	m_szChatMsg = NULL;

	SetProcessHelp(NULL);
}

AgcmAdminDlgHelp::~AgcmAdminDlgHelp()
{
	ClearHelpList();
	if(!m_szChatMsg) delete [] m_szChatMsg;
}

BOOL AgcmAdminDlgHelp::SetCallbackHelpComplete(ADMIN_CB pfCallback)
{
	m_pfCBHelpComplete = pfCallback;
	return TRUE;
}

BOOL AgcmAdminDlgHelp::SetCallbackCharInfo(ADMIN_CB pfCallback)
{
	m_pfCBCharInfo = pfCallback;
	return TRUE;
}

BOOL AgcmAdminDlgHelp::SetCallbackChatting(ADMIN_CB pfCallback)
{
	m_pfCBChatting = pfCallback;
	return TRUE;
}

BOOL AgcmAdminDlgHelp::AddHelp(stAgpdAdminHelp* pstHelp)
{
	if(!pstHelp) return FALSE;

	stAgpdAdminHelp* pstNewHelp = new stAgpdAdminHelp;
	memcpy(pstNewHelp, pstHelp, sizeof(stAgpdAdminHelp));
	m_listHelp.AddTail(pstNewHelp);

	// 현재 처리할 것으로 세팅한다.
	SetProcessHelp(pstHelp);

	// 화면을 비우고
	ClearHelpListView();

	return ShowHelpList();
}

BOOL AgcmAdminDlgHelp::RemoveHelp(stAgpdAdminHelp* pstHelp)
{
	if(!pstHelp) return FALSE;

	if(m_listHelp.IsEmpty())
		return FALSE;

	AuNode<stAgpdAdminHelp*>* pcNode = m_listHelp.GetHeadNode();
	while(pcNode)
	{
		if(pcNode->GetData() && pcNode->GetData()->m_lCount == pstHelp->m_lCount)
		{
			delete pcNode->GetData();
			m_listHelp.RemoveNode(pcNode);
			break;
		}
		pcNode = pcNode->GetNextNode();
	}

	return TRUE;
}

BOOL AgcmAdminDlgHelp::SetProcessHelp(stAgpdAdminHelp* pstHelp)
{
	if(pstHelp)
		memcpy(&m_stProcessHelp, pstHelp, sizeof(m_stProcessHelp));
	else
		memset(&m_stProcessHelp, 0, sizeof(m_stProcessHelp));

	return TRUE;
}

stAgpdAdminHelp* AgcmAdminDlgHelp::GetProcessHelp()
{
	return &m_stProcessHelp;
}

BOOL AgcmAdminDlgHelp::SetCharInfo(stAgpdAdminCharData* pstCharData)
{
	if(pstCharData && CheckReceivedCharInfo(pstCharData))
		memcpy(&m_stCharData, pstCharData, sizeof(m_stCharData));
	else
		memset(&m_stCharData, 0, sizeof(m_stCharData));

	return ShowCharInfo();
}

stAgpdAdminCharData* AgcmAdminDlgHelp::GetCharInfo()
{
	return &m_stCharData;
}

BOOL AgcmAdminDlgHelp::CheckReceivedCharInfo(stAgpdAdminCharData* pstCharData)
{
	if(!pstCharData)
		return FALSE;

	HWND hHelpList = GetDlgItem(m_hDlg, IDC_LV_HELP_LIST);
	if(!hHelpList)
		return FALSE;

	// 현재 선택중인 놈의 이름과 비교한다.
	char szTmp[255];
	memset(szTmp, 0, 255);
	ListView_GetItemText(hHelpList, m_nSelectedIndex, 1, szTmp, AGPACHARACTER_MAX_ID_STRING);

	return strcmp(szTmp, pstCharData->m_stBasic.m_szCharName) == 0 ? TRUE : FALSE;
}

BOOL AgcmAdminDlgHelp::ReceiveChatMsg(stAgpdAdminChatData* pstAdminChatData)
{
	if(!pstAdminChatData)
		return FALSE;

	// ID 가 다를 수도 있는데.. 흠...
	//if(pstAdminChatData->m_lSenderID != GetProcessHelp()->m_lCID)
	//	return FALSE;

	// 이름 비교
	if(strcmp(pstAdminChatData->m_szSenderName, GetProcessHelp()->m_szCharName) != 0)
		return FALSE;

	CHAR szMsg[AGPACHARACTER_MAX_ID_STRING + AGPMADMIN_MAX_CHAT_STRING + 2];
	memset(szMsg, 0, AGPACHARACTER_MAX_ID_STRING + AGPMADMIN_MAX_CHAT_STRING + 2);

	sprintf(szMsg, "%s : %s", pstAdminChatData->m_szSenderName, pstAdminChatData->m_szMessage);

	AddChatMsg(szMsg);

	return TRUE;
}

// lCount 인 Help 를 List 에서 찾는다.
stAgpdAdminHelp* AgcmAdminDlgHelp::GetHelpInList(INT32 lCount)
{
	if(m_listHelp.GetCount() == 0)
		return NULL;

	AuNode<stAgpdAdminHelp*>* pcNode = m_listHelp.GetHeadNode();
	while(pcNode)
	{
		if(pcNode->GetData()->m_lCount == lCount)
			return pcNode->GetData();

		pcNode = pcNode->GetNextNode();
	}

	return NULL;
}

// szText 의 버퍼가 255 이상 되어야 한다.
BOOL AgcmAdminDlgHelp::GetSubjectText(CHAR* szText)
{
	if(!szText)
		return FALSE;

	GetDlgItemText(m_hDlg, IDC_E_HELP_MSG, szText, 255);
	return TRUE;
}

// szText 의 버퍼가 255 이상 되어야 한다.
BOOL AgcmAdminDlgHelp::GetMemoText(CHAR* szText)
{
	if(!szText)
		return FALSE;

	GetDlgItemText(m_hDlg, IDC_E_HELP_MEMO, szText, 255);
	return TRUE;
}

// Status 를 보고 적절하게 처리힌다.
BOOL AgcmAdminDlgHelp::ReceiveHelpCompleteResult(stAgpdAdminHelp* pstHelp)
{
	switch(pstHelp->m_lStatus)
	{
		case AGPMADMIN_HELP_STATUS_FAILURE:
			MessageBox(m_hDlg, "진정 처리 결과가 실패하였습니다.", "에러!!!", MB_OK);
			break;

		case AGPMADMIN_HELP_STATUS_COMPLETE:
			MessageBox(m_hDlg, "진정 처리가 정상적으로 완료하였습니다.", "결과", MB_OK);
			break;

		default:
			MessageBox(m_hDlg, "쓰봉알라이제이션~", "뷁", MB_OK);
			break;
	}

	// 진행중인 진정을 없애준다.
	SetProcessHelp(NULL);

	return TRUE;
}

BOOL AgcmAdminDlgHelp::ShowHelpList(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hList = GetDlgItem(hDlg, IDC_LV_HELP_LIST);
	if(!hList)
		return FALSE;

	if(m_listHelp.IsEmpty())
		return FALSE;

	CHAR szTmp[255];
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	int iRows = 0;
	AuNode<stAgpdAdminHelp*>* pcNode = m_listHelp.GetTailNode();
	while(pcNode)
	{
		if(pcNode->GetData() == NULL)
			break;

		lvItem.iItem = iRows;

		// Count
		lvItem.iSubItem = 0;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lCount);
		lvItem.pszText = szTmp;
		ListView_InsertItem(hList, &lvItem);

		lvItem.iSubItem = 1;
		lvItem.pszText = pcNode->GetData()->m_szCharName;
		ListView_SetItem(hList, &lvItem);

		lvItem.iSubItem = 2;
		lvItem.pszText = pcNode->GetData()->m_szSubject;
		ListView_SetItem(hList, &lvItem);

		lvItem.iSubItem = 3;
		lvItem.pszText = pcNode->GetData()->m_szMemo;
		ListView_SetItem(hList, &lvItem);

		lvItem.iSubItem = 4;
		// UNIX TimeStamp 로 부터 날짜를 얻어낸다.
		AgcmAdminDlgManager::Instance()->GetDateTimeByTimeStamp(pcNode->GetData()->m_lDate, szTmp);
		lvItem.pszText = szTmp;
		ListView_SetItem(hList, &lvItem);

		iRows++;
		pcNode = pcNode->GetPrevNode();
	}

	return TRUE;
}

BOOL AgcmAdminDlgHelp::ShowCharInfo(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hCharInfo = GetDlgItem(hDlg, IDC_LV_HELP_BASIC);
	if(!hCharInfo)
		return FALSE;

	// 없다면 깨끗하게 비우자. 0 으로 하지말고.
	if(m_stCharData.m_stBasic.m_lCID == 0)
	{
		ListView_DeleteAllItems(hCharInfo);
		return TRUE;
	}

	CHAR szTmp[255];
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	lvItem.iItem = 0;

	// CharName
	lvItem.iSubItem = 0;
	lvItem.pszText = m_stCharData.m_stBasic.m_szCharName;
	ListView_InsertItem(hCharInfo, &lvItem);

	// CharID
	lvItem.iSubItem = 1;
	wsprintf(szTmp, "%d", m_stCharData.m_stBasic.m_lCID);
	lvItem.pszText = szTmp;
	ListView_SetItem(hCharInfo, &lvItem);

	// ID
	lvItem.iSubItem = 2;
	lvItem.pszText = m_stCharData.m_stSub.m_szAccName;
	ListView_SetItem(hCharInfo, &lvItem);

	// Name
	lvItem.iSubItem = 3;
	lvItem.pszText = m_stCharData.m_stSub.m_szName;
	ListView_SetItem(hCharInfo, &lvItem);

	// Race
	lvItem.iSubItem = 4;
	wsprintf(szTmp, "%d", m_stCharData.m_stStatus.m_lRace);
	lvItem.pszText = szTmp;
	ListView_SetItem(hCharInfo, &lvItem);

	// Class
	lvItem.iSubItem = 5;
	wsprintf(szTmp, "%d", m_stCharData.m_stStatus.m_lClass);
	lvItem.pszText = szTmp;
	ListView_SetItem(hCharInfo, &lvItem);

	// Lev
	lvItem.iSubItem = 6;
	wsprintf(szTmp, "%d", m_stCharData.m_stStatus.m_lLevel);
	lvItem.pszText = szTmp;
	ListView_SetItem(hCharInfo, &lvItem);

	// UT
	lvItem.iSubItem = 7;
	lvItem.pszText = "UT";
	ListView_SetItem(hCharInfo, &lvItem);

	// CreDate
	lvItem.iSubItem = 8;
	lvItem.pszText = "";
	ListView_SetItem(hCharInfo, &lvItem);

	// IP
	lvItem.iSubItem = 9;
	lvItem.pszText = m_stCharData.m_stSub.m_szIP;
	ListView_SetItem(hCharInfo, &lvItem);

	return TRUE;
}

BOOL AgcmAdminDlgHelp::ShowSubject(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	return TRUE;
}

BOOL AgcmAdminDlgHelp::ShowMemo(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	return TRUE;
}

BOOL AgcmAdminDlgHelp::ClearHelpList()
{
	if(m_listHelp.IsEmpty())
		return FALSE;

	AuNode<stAgpdAdminHelp*>* pcNode = m_listHelp.GetHeadNode();
	AuNode<stAgpdAdminHelp*>* pcNode2 = NULL;
	while(pcNode)
	{
		if(pcNode->GetData())
		{
			pcNode2 = pcNode->GetNextNode();
			delete pcNode->GetData();
			m_listHelp.RemoveNode(pcNode);
			pcNode = pcNode2;

			continue;
		}
		else
		{
			pcNode = pcNode->GetNextNode();
		}
	}
	
	return TRUE;
}

BOOL AgcmAdminDlgHelp::ClearHelpListView(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hList = GetDlgItem(hDlg, IDC_LV_HELP_LIST);
	if(!hList)
		return FALSE;

	return ListView_DeleteAllItems(hList);
}

BOOL AgcmAdminDlgHelp::ClearCharInfoView(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	return TRUE;
}

BOOL AgcmAdminDlgHelp::ClearSubject(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	return TRUE;
}

BOOL AgcmAdminDlgHelp::ClearMemo(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	return TRUE;
}

BOOL AgcmAdminDlgHelp::AddChatMsg(CHAR* szMsg)
{
	if(!szMsg)
		return FALSE;

	// 처음 쓰는 거라면
	if(m_szChatMsg == NULL)
	{
		m_szChatMsg = new char[strlen(szMsg)+1];
		strcpy(m_szChatMsg, szMsg);
	}
	else
	{
		int iLength1 = strlen(m_szChatMsg);
		int iLength2 = strlen(szMsg);

		char* szNewChatMsg = new char[iLength1 + iLength2 + 4];
		sprintf(szNewChatMsg, "%s\r\n%s", m_szChatMsg, szMsg);

		char* szTmp = m_szChatMsg;
		m_szChatMsg = szNewChatMsg;
		delete [] szTmp;
	}

	SetDlgItemText(m_hDlg, IDC_E_HELP_CHAT, m_szChatMsg);

	return TRUE;
}

BOOL AgcmAdminDlgHelp::ClearChatMsg(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	SetDlgItemText(hDlg, IDC_E_HELP_CHAT, "");
	if(m_szChatMsg)
	{
		delete [] m_szChatMsg;
		m_szChatMsg = NULL;
	}

	return TRUE;
}



LRESULT CALLBACK AgcmAdminDlgHelp::HelpDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_INITDIALOG:
			return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnInitDialog(hDlg, wParam, lParam);

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_B_HELP_REFRESH:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnRefreshBtn(hDlg);

				case IDC_B_HELP_GO:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnGoBtn(hDlg);
				case IDC_B_HELP_MODIFY:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnModifyBtn(hDlg);
				case IDC_B_HELP_GM:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnGMBtn(hDlg);
				case IDC_B_HELP_XYZ:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnXYZBtn(hDlg);

				case IDC_B_HELP_COMPLETE:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnCompleteBtn(hDlg);
				case IDC_B_HELP_DEFER:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnDeferBtn(hDlg);
				case IDC_B_HELP_DELIVER:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnDeliverBtn(hDlg);
				case IDC_B_HELP_REJECT:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnRejectBtn(hDlg);
				case IDC_B_HELP_DELETE:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnDeleteBtn(hDlg);

				case IDC_B_HELP_SAVE:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnSaveBtn(hDlg);
				case IDC_B_HELP_CANCEL:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnCancelBtn(hDlg);

				case IDC_B_HELP_CHAT_SEND:
					return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnChatting(hDlg);
			}
			return FALSE;

		case WM_NOTIFY:
		{
			LPNMHDR hdr;
			LPNMLISTVIEW nlv;
			hdr = (LPNMHDR)lParam;
			nlv = (LPNMLISTVIEW)lParam;
			HWND hHelpList = GetDlgItem(hDlg, IDC_LV_HELP_LIST);

			if(hdr != NULL && hdr->hwndFrom == hHelpList)
			{
				switch(hdr->code)
				{
					case NM_CLICK:
						return AgcmAdminDlgManager::Instance()->GetHelpDlg()->OnHelpListViewClk(hHelpList, hdr, nlv);
				}
			}

			return TRUE;
		}


		case WM_CLOSE:
			return AgcmAdminDlgManager::Instance()->GetHelpDlg()->CloseDlg(hDlg);
	}
	return FALSE;
}

LRESULT AgcmAdminDlgHelp::OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if(GetInitialized() == FALSE)
	{
		OnInitHelpLV(hDlg);
		OnInitCharInfoLV(hDlg);
	}

	SetDlgItemText(hDlg, IDC_E_HELP_MSG, "");
	SetDlgItemText(hDlg, IDC_E_HELP_MEMO, "");
	ClearChatMsg(hDlg);
	SetDlgItemText(hDlg, IDC_E_HELP_CHAT_SEND, "");

	// Data 출력
	ShowHelpList(hDlg);

	::SetForegroundWindow(hDlg);

	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnInitHelpLV(HWND hDlg)
{
	HWND hHelpList = GetDlgItem(hDlg, IDC_LV_HELP_LIST);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 50;
	lvCol.pszText = "No";
	lvCol.iSubItem = 0;
	ListView_InsertColumn(hHelpList, 0, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 1;
	ListView_InsertColumn(hHelpList, 1, &lvCol);

	lvCol.cx = 180;
	lvCol.pszText = "Subject";
	lvCol.iSubItem = 2;
	ListView_InsertColumn(hHelpList, 2, &lvCol);

	lvCol.cx = 130;
	lvCol.pszText = "Memo";
	lvCol.iSubItem = 3;
	ListView_InsertColumn(hHelpList, 3, &lvCol);

	lvCol.cx = 90;
	lvCol.pszText = "Date/Time";
	lvCol.iSubItem = 4;
	ListView_InsertColumn(hHelpList, 4, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Status";
	lvCol.iSubItem = 5;
	ListView_InsertColumn(hHelpList, 5, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	ListView_SetExtendedListViewStyle(hHelpList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnInitCharInfoLV(HWND hDlg)
{
	HWND hBasicInfo = GetDlgItem(hDlg, IDC_LV_HELP_BASIC);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 80;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 0;
	ListView_InsertColumn(hBasicInfo, 0, &lvCol);

	lvCol.cx = 70;
	lvCol.pszText = "CharNo";
	lvCol.iSubItem = 1;
	ListView_InsertColumn(hBasicInfo, 1, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "AccName";
	lvCol.iSubItem = 2;
	ListView_InsertColumn(hBasicInfo, 2, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "Name";
	lvCol.iSubItem = 3;
	ListView_InsertColumn(hBasicInfo, 3, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Race";
	lvCol.iSubItem = 4;
	ListView_InsertColumn(hBasicInfo, 4, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Class";
	lvCol.iSubItem = 5;
	ListView_InsertColumn(hBasicInfo, 5, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Lev";
	lvCol.iSubItem = 6;
	ListView_InsertColumn(hBasicInfo, 6, &lvCol);

	lvCol.cx = 100;
	lvCol.pszText = "UT";
	lvCol.iSubItem = 7;
	ListView_InsertColumn(hBasicInfo, 7, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "CreDate";
	lvCol.iSubItem = 8;
	ListView_InsertColumn(hBasicInfo, 8, &lvCol);

	lvCol.cx = 120;
	lvCol.pszText = "IP";
	lvCol.iSubItem = 9;
	ListView_InsertColumn(hBasicInfo, 9, &lvCol);

	lvCol.cx = 60;
	lvCol.pszText = "Status";
	lvCol.iSubItem = 10;
	ListView_InsertColumn(hBasicInfo, 10, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	ListView_SetExtendedListViewStyle(hBasicInfo, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnHelpListViewClk(HWND hHelpList, LPNMHDR hdr, LPNMLISTVIEW nlv)
{
	if(nlv->iItem < 0)
	{
		// 뿌려져 있는 데이터를 지운다.
		ClearCharInfoView();
		ClearSubject();
		ClearMemo();
		ClearChatMsg();

		return TRUE;
	}
	
	// 마지막 Selecte Index 를 세팅한다.
	m_nSelectedIndex = nlv->iItem;

	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	// CharName 으로 다시 검색 패킷을 날린다.
	ListView_GetItemText(hHelpList, nlv->iItem, 1, stSearch.m_szSearchName, AGPACHARACTER_MAX_ID_STRING);

	// Callback Execute
	m_pfCBCharInfo(&stSearch, m_pcsCBClass, NULL);

	// 진정 내용을 뿌린다.
	char szTmp[255];
	ListView_GetItemText(hHelpList, nlv->iItem, 2, szTmp, 254);
	SetDlgItemText(m_hDlg, IDC_E_HELP_MSG, szTmp);

	// 메모를 뿌린다.
	ListView_GetItemText(hHelpList, nlv->iItem, 3, szTmp, 254);
	SetDlgItemText(m_hDlg, IDC_E_HELP_MEMO, szTmp);

	// 채팅 내용을 뿌린다.
	ClearChatMsg(m_hDlg);

	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnRefreshBtn(HWND hDlg)
{
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnGoBtn(HWND hDlg)
{
	// 현재 도움 처리 중인 캐릭터에게 이동한다.
	if(strlen(m_stProcessHelp.m_szCharName) == 0)
		return FALSE;

	AgcmAdminDlgManager::Instance()->GetMoveDlg()->ProcessSelfMove(m_stProcessHelp.m_szCharName);
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnModifyBtn(HWND hDlg)
{
	// 현재 도움 처리 중인 캐릭터를 다시 한번 Search 날린다.
	// 이때는 Last Search 가 해당 캐릭터로 바뀐다.
	if(strlen(m_stProcessHelp.m_szCharName) == 0)
		return FALSE;

	AgcmAdminDlgManager::Instance()->Instance()->GetSearchDlg()->ProcessSearch(m_stProcessHelp.m_szCharName);
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnGMBtn(HWND hDlg)
{
	OnGoBtn(hDlg);
	OnModifyBtn(hDlg);
	
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnXYZBtn(HWND hDlg)
{
	if(strlen(m_stProcessHelp.m_szCharName) == 0)
		return FALSE;

	if(hDlg == NULL) hDlg = m_hDlg;

	// Move Dialog 를 연다.
	AgcmAdminDlgManager::Instance()->GetMoveDlg()->SetName(m_stProcessHelp.m_szCharName);
	AgcmAdminDlgManager::Instance()->GetMoveDlg()->SetParentHwnd(GetDlgItem(hDlg, IDC_B_HELP_XYZ));
	AgcmAdminDlgManager::Instance()->GetMoveDlg()->OpenDlg();
	
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnCompleteBtn(HWND hDlg, INT8 nStatus)
{
	// 현재 처리중인 Help 가 없으면 나간다.
	if(m_stProcessHelp.m_lCID == 0 || m_stProcessHelp.m_lAdminCID == 0)
		return FALSE;

	stAgpdAdminHelp stHelp;
	memcpy(&stHelp, &m_stProcessHelp, sizeof(stHelp));

	// 상태를 새로 세팅해서 보낸다. - 서버에서 잘 처리하면 리턴해준다.
	stHelp.m_lStatus = nStatus;

	// Memo 를 넣어서 보낸다.
	GetDlgItemText(hDlg, IDC_E_HELP_MEMO, stHelp.m_szMemo, AGPMADMIN_MAX_HELP_MEMO_STRING);

	m_pfCBHelpComplete(&stHelp, m_pcsCBClass, NULL);

	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnDeferBtn(HWND hDlg)
{
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnDeliverBtn(HWND hDlg)
{
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnRejectBtn(HWND hDlg)
{
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnDeleteBtn(HWND hDlg)
{
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnSaveBtn(HWND hDlg)
{
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnCancelBtn(HWND hDlg)
{
	return TRUE;
}

LRESULT AgcmAdminDlgHelp::OnChatting(HWND hDlg)
{
	if(!m_pfCBChatting || !m_pcsCBClass)
		return FALSE;

	stAgpdAdminChatData stAdminChatData;
	memset(&stAdminChatData, 0, sizeof(stAdminChatData));

	stAdminChatData.m_lTargetID = GetProcessHelp()->m_lCID;
	strcpy(stAdminChatData.m_szTargetName, GetProcessHelp()->m_szCharName);

	GetDlgItemText(hDlg, IDC_E_HELP_CHAT_SEND, stAdminChatData.m_szMessage, AGPMADMIN_MAX_CHAT_STRING);
	stAdminChatData.m_lMessageLength = strlen(stAdminChatData.m_szMessage);

	if(stAdminChatData.m_lMessageLength == 0)
		return FALSE;

	m_pfCBChatting(&stAdminChatData, m_pcsCBClass, NULL);

	CHAR szMsg[AGPACHARACTER_MAX_ID_STRING + AGPMADMIN_MAX_CHAT_STRING + 2];
	sprintf(szMsg, "%s : %s", AgcmAdminDlgManager::Instance()->GetSelfAdminInfo()->m_szAdminName, stAdminChatData.m_szMessage);
	AddChatMsg(szMsg);
	SetDlgItemText(hDlg, IDC_E_HELP_CHAT_SEND, "");
	
	return TRUE;
}