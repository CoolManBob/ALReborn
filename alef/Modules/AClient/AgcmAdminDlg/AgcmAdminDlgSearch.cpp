// AgcmAdminDlgSearch.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 29.

#include "AgcmAdminDlgInclude.h"
#include "resource.h"

#define MAX_COLUMN	15

AgcmAdminDlgSearch::AgcmAdminDlgSearch()
{
	m_iResourceID = IDD_SEARCH;
	m_pfDlgProc = SearchDlgProc;

	m_pfCBSearch = NULL;

	memset(m_szClickName, 0, AGPACHARACTER_MAX_ID_STRING+1);

	SetCharData(NULL);
}

AgcmAdminDlgSearch::~AgcmAdminDlgSearch()
{
	ClearResultList();
}

BOOL AgcmAdminDlgSearch::SetCBSearch(ADMIN_CB pfCBSearch)
{
	m_pfCBSearch = pfCBSearch;
	return TRUE;
}

void AgcmAdminDlgSearch::SetCharData(stAgpdAdminCharData* pstCharData)
{
	if(pstCharData)
	{
		memcpy(&m_stCharData, pstCharData, sizeof(m_stCharData));
	}
	else
	{
		ZeroMemory(&m_stCharData, sizeof(m_stCharData));
	}
}

stAgpdAdminCharData* AgcmAdminDlgSearch::GetCharData()
{
	return &m_stCharData;
}

BOOL AgcmAdminDlgSearch::SetResultList(PVOID pList)
{
	AuList<stAgpdAdminSearchResult*>* pListSearchResult = (AuList<stAgpdAdminSearchResult*>*)pList;
	if(!pListSearchResult)
		return FALSE;

	// 일단 멤버데이터를 다시한번 비우고.. 그러나 이미 비어져 있을 것이다.
	ClearResultList();
	ClearResultListView();

	// pListSearchResult 를 멤버 데이터에 세팅한다.
	AuNode<stAgpdAdminSearchResult*>* pcNode = pListSearchResult->GetHeadNode();
	while(pcNode != NULL)
	{
		stAgpdAdminSearchResult* pstSearchResult = new stAgpdAdminSearchResult;
		memcpy(pstSearchResult, pcNode->GetData(), sizeof(stAgpdAdminSearchResult));
		m_listSearchResult.AddTail(pstSearchResult);

		pcNode = pcNode->GetNextNode();
	}

	return ShowData();
}

BOOL AgcmAdminDlgSearch::SetResult(stAgpdAdminSearch* pstSearch, stAgpdAdminSearchResult* pstSearchResult)
{
	if(pstSearchResult == NULL)
		return FALSE;

	// 이미 리스트에 있으면 그냥 나간다.
	if(IsInResultList(pstSearchResult))
		return FALSE;

	stAgpdAdminSearchResult* pstSearchResult2 = new stAgpdAdminSearchResult;
	memcpy(pstSearchResult2, pstSearchResult, sizeof(stAgpdAdminSearchResult));
	m_listSearchResult.AddTail(pstSearchResult2);

	return ShowData();
}

BOOL AgcmAdminDlgSearch::IsInResultList(stAgpdAdminSearchResult* pstSearchResult)
{
	if(pstSearchResult == NULL)
		return FALSE;

	BOOL bResult = FALSE;

	AuNode<stAgpdAdminSearchResult*>* pcNode = m_listSearchResult.GetHeadNode();
	while(pcNode)
	{
		if(pstSearchResult->m_lCID == pcNode->GetData()->m_lCID ||
			strcmp(pstSearchResult->m_szCharName, pcNode->GetData()->m_szCharName) == 0)
		{
			bResult = TRUE;
			break;
		}

		pcNode = pcNode->GetNextNode();
	}

	return bResult;
}

BOOL AgcmAdminDlgSearch::ProcessSearch(CHAR* szName)
{
	if(!m_pfCBSearch || !m_pcsCBClass)
		return FALSE;

	if(!szName || strlen(szName) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	strcpy(stSearch.m_szSearchName, szName);

	// Set Last Search
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->SetLastSearch(&stSearch);

	// Callback Execute
	m_pfCBSearch(&stSearch, m_pcsCBClass, NULL);

	// Open Character Dlg
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->OpenDlg();

	return TRUE;
}

BOOL AgcmAdminDlgSearch::ShowData(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hResult = GetDlgItem(hDlg, IDC_LV_SEARCH_RESULT);
	if(hResult == NULL)
		return FALSE;

	// 화면만 비운다.
	ClearResultListView();

	CHAR szTmp[255];
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	int iRows = 0;
	AuNode<stAgpdAdminSearchResult*>* pcNode = m_listSearchResult.GetHeadNode();
	while(pcNode)
	{
		if(pcNode->GetData() == NULL)
			break;

		lvItem.iItem = iRows;

		// CharName
		lvItem.iSubItem = 0;
		lvItem.pszText = pcNode->GetData()->m_szCharName;
		ListView_InsertItem(hResult, &lvItem);

		// CharNo
		lvItem.iSubItem = 1;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lCID);
		lvItem.pszText = szTmp;
		ListView_SetItem(hResult, &lvItem);

		// AccName
		lvItem.iSubItem = 2;
		lvItem.pszText = pcNode->GetData()->m_szAccName;
		ListView_SetItem(hResult, &lvItem);

		// Name
		lvItem.iSubItem = 3;
		lvItem.pszText = pcNode->GetData()->m_szName;
		ListView_SetItem(hResult, &lvItem);

		// Race
		lvItem.iSubItem = 4;
		lvItem.pszText = pcNode->GetData()->m_szRace;
		ListView_SetItem(hResult, &lvItem);

		// Class
		lvItem.iSubItem = 5;
		lvItem.pszText = pcNode->GetData()->m_szClass;
		ListView_SetItem(hResult, &lvItem);

		// Level
		lvItem.iSubItem = 6;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lLevel);
		lvItem.pszText = szTmp;
		ListView_SetItem(hResult, &lvItem);
		
		// UT
		lvItem.iSubItem = 7;
		lvItem.pszText = pcNode->GetData()->m_szUT;
		ListView_SetItem(hResult, &lvItem);

		// CreDate
		lvItem.iSubItem = 8;
		lvItem.pszText = pcNode->GetData()->m_szCreDate;
		ListView_SetItem(hResult, &lvItem);

		// IP
		lvItem.iSubItem = 9;
		lvItem.pszText = pcNode->GetData()->m_szIP;
		ListView_SetItem(hResult, &lvItem);

		// Status
		lvItem.iSubItem = 10;
		wsprintf(szTmp, "%d", pcNode->GetData()->m_lStatus);
		lvItem.pszText = szTmp;
		ListView_SetItem(hResult, &lvItem);

		pcNode = pcNode->GetNextNode();
		iRows++;
	}

	return TRUE;
}

BOOL AgcmAdminDlgSearch::ClearResultList()
{
	if(!m_listSearchResult.IsEmpty())
	{
		AuNode<stAgpdAdminSearchResult*>* pcNode = NULL;
		AuNode<stAgpdAdminSearchResult*>* pcNode2 = NULL;

		pcNode = m_listSearchResult.GetHeadNode();
		while(pcNode)
		{
			if(pcNode->GetData())
			{
				delete pcNode->GetData();

				pcNode2 = pcNode->GetNextNode();
				m_listSearchResult.RemoveNode(pcNode);
				pcNode = pcNode2;
			}
			else
				break;
		}
	}

	return TRUE;
}

BOOL AgcmAdminDlgSearch::ClearResultListView(HWND hDlg)
{
	if(hDlg == NULL) hDlg = m_hDlg;

	HWND hResult = GetDlgItem(hDlg, IDC_LV_SEARCH_RESULT);
	if(hResult == NULL)
		return FALSE;

	return ListView_DeleteAllItems(hResult);
}

LRESULT CALLBACK AgcmAdminDlgSearch::SearchDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_INITDIALOG:
			return AgcmAdminDlgManager::Instance()->GetSearchDlg()->OnInitDialog(hDlg, wParam, lParam);

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_BTN_SEARCH_SEARCH:
					return AgcmAdminDlgManager::Instance()->GetSearchDlg()->OnSearchBtn(hDlg);
				case IDC_BTN_SEARCH_GO:
					return AgcmAdminDlgManager::Instance()->GetSearchDlg()->OnGoBtn(hDlg);
				case IDC_BTN_SEARCH_MODIFY:
					return AgcmAdminDlgManager::Instance()->GetSearchDlg()->OnModifyBtn(hDlg);
				case IDC_BTN_SEARCH_GM:
					return AgcmAdminDlgManager::Instance()->GetSearchDlg()->OnGMBtn(hDlg);
			}
			return FALSE;

		case WM_NOTIFY:
		{
			LPNMHDR hdr;
			LPNMLISTVIEW nlv;
			hdr = (LPNMHDR)lParam;
			nlv = (LPNMLISTVIEW)lParam;
			HWND hResultList = GetDlgItem(hDlg, IDC_LV_SEARCH_RESULT);

			if(hdr != NULL && hdr->hwndFrom == hResultList)
			{
				switch(hdr->code)
				{
					case NM_CLICK:
						return AgcmAdminDlgManager::Instance()->GetSearchDlg()->OnResultListClk(hResultList, hdr, nlv);
					case NM_DBLCLK:
						return AgcmAdminDlgManager::Instance()->GetSearchDlg()->OnResultListDblClk(hResultList, hdr, nlv);
				}
			}

			return TRUE;
		}

		case WM_CLOSE:
			return AgcmAdminDlgManager::Instance()->GetSearchDlg()->CloseDlg(hDlg);
	}
	return FALSE;
}

LRESULT AgcmAdminDlgSearch::OnInitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	// Search Type 데이터 세팅
	HWND hSearchType = GetDlgItem(hDlg, IDC_CB_SEARCH_TYPE);
	SendMessage(hSearchType, CB_ADDSTRING, 0, (LPARAM)"PC");
	SendMessage(hSearchType, CB_ADDSTRING, 0, (LPARAM)"Item");
	SendMessage(hSearchType, CB_ADDSTRING, 0, (LPARAM)"NPC");
	SendMessage(hSearchType, CB_ADDSTRING, 0, (LPARAM)"Monster");
	
	SendMessage(hSearchType, CB_SETCURSEL, 0, 0);

	// Search Field 데이터 세팅
	HWND hSearchField = GetDlgItem(hDlg, IDC_CB_SEARCH_FIELD);
	SendMessage(hSearchField, CB_ADDSTRING, 0, (LPARAM)"CharName");
	SendMessage(hSearchField, CB_ADDSTRING, 0, (LPARAM)"CharID");
	SendMessage(hSearchField, CB_ADDSTRING, 0, (LPARAM)"AccName");
	SendMessage(hSearchField, CB_ADDSTRING, 0, (LPARAM)"Name");

	SendMessage(hSearchField, CB_SETCURSEL, 0, 0);

	// Result List-View 헤더 세팅
	ResetResultListView(hDlg);
	InitResultListView2Character(hDlg);	// 일단 캐릭터로 세팅

	::SetForegroundWindow(hDlg);

	return TRUE;
}

LRESULT AgcmAdminDlgSearch::OnSearchBtn(HWND hDlg)
{
	if(m_pfCBSearch == NULL)
		return FALSE;

	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	// Type 을 얻는다.
	stSearch.m_iType = (INT8)SendDlgItemMessage(hDlg, IDC_CB_SEARCH_TYPE, CB_GETCURSEL, 0, 0);

	// Field 를 얻는다.
	stSearch.m_iField = (INT8)SendDlgItemMessage(hDlg, IDC_CB_SEARCH_FIELD, CB_GETCURSEL, 0, 0);

	// Search Name 을 얻는다.
	GetDlgItemText(hDlg, IDC_E_CHAR_SEARCH_DATA, stSearch.m_szSearchName, AGPACHARACTER_MAX_ID_STRING);

	// Callback Execute
	m_pfCBSearch(&stSearch, m_pcsCBClass, NULL);

	// Clear Result List
	ClearResultList();
	ClearResultListView(hDlg);

	return TRUE;
}

LRESULT AgcmAdminDlgSearch::OnGoBtn(HWND hDlg)
{
	if(strlen(m_szClickName) <= 0)
		return FALSE;

	// 바로 날라간다.
	AgcmAdminDlgManager::Instance()->GetMoveDlg()->ProcessSelfMove(m_szClickName);

	return TRUE;
}

LRESULT AgcmAdminDlgSearch::OnModifyBtn(HWND hDlg)
{
	if(strlen(m_szClickName) <= 0)
		return FALSE;

	// 이미 저장해 놓은 이름으로 검색을 날린다.
	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	strcpy(stSearch.m_szSearchName, m_szClickName);

	// Set Last Search
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->SetLastSearch(&stSearch);

	m_pfCBSearch(&stSearch, m_pcsCBClass, NULL);

	// Open Character Dlg
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->OpenDlg();

	return TRUE;
}

LRESULT AgcmAdminDlgSearch::OnGMBtn(HWND hDlg)
{
	if(strlen(m_szClickName) <= 0)
		return FALSE;

	OnGoBtn(hDlg);
	OnModifyBtn(hDlg);

	return TRUE;
}

LRESULT AgcmAdminDlgSearch::OnResultListClk(HWND hResultList, LPNMHDR hdr, LPNMLISTVIEW nlv)
{
	// 클릭한 Item 의 이름을 저장한다.
	if(nlv->iItem >= 0)
	{
		ListView_GetItemText(hResultList, nlv->iItem, 0, m_szClickName, AGPACHARACTER_MAX_ID_STRING);
	}
	else
		memset(m_szClickName, 0, AGPACHARACTER_MAX_ID_STRING+1);

	return TRUE;
}

LRESULT AgcmAdminDlgSearch::OnResultListDblClk(HWND hResultList, LPNMHDR hdr, LPNMLISTVIEW nlv)
{
	char szBuf[64];
	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	// CharName 으로 다시 검색 패킷을 날린다.
	stSearch.m_iType = 0;
	stSearch.m_iField = 0;
	ListView_GetItemText(hResultList, nlv->iItem, 0, stSearch.m_szSearchName, AGPACHARACTER_MAX_ID_STRING);
	ListView_GetItemText(hResultList, nlv->iItem, 1, szBuf, 63);
	stSearch.m_lObjectCID = atoi(szBuf);

	// Set Last Search
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->SetLastSearch(&stSearch);

	// Callback Execute
	m_pfCBSearch(&stSearch, m_pcsCBClass, NULL);

	// Open Character Dlg
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->OpenDlg();

	return TRUE;
}

void AgcmAdminDlgSearch::ResetResultListView(HWND hDlg)
{
	HWND hSearchResult = GetDlgItem(hDlg, IDC_LV_SEARCH_RESULT);

	// 데이터를 비우고
	ListView_DeleteAllItems(hSearchResult);

	// 헤더도 비운다.
	for(int i = 0; i < MAX_COLUMN; i++)
	{
		ListView_DeleteColumn(hSearchResult, i);
		// 아아.. 왜 한방에 안 지워지는 거야...ㅜㅜ
		Sleep(1);
	}

	// 한 행만 누르면 한 줄이 선택되게 한다.
	ListView_SetExtendedListViewStyle(hSearchResult, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void AgcmAdminDlgSearch::InitResultListView2Character(HWND hDlg)
{
	HWND hSearchResult = GetDlgItem(hDlg, IDC_LV_SEARCH_RESULT);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 80;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 0;
	ListView_InsertColumn(hSearchResult, 0, &lvCol);

	lvCol.cx = 70;
	lvCol.pszText = "CharNo";
	lvCol.iSubItem = 1;
	ListView_InsertColumn(hSearchResult, 1, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "AccName";
	lvCol.iSubItem = 2;
	ListView_InsertColumn(hSearchResult, 2, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "Name";
	lvCol.iSubItem = 3;
	ListView_InsertColumn(hSearchResult, 3, &lvCol);

	lvCol.cx = 45;
	lvCol.pszText = "Race";
	lvCol.iSubItem = 4;
	ListView_InsertColumn(hSearchResult, 4, &lvCol);

	lvCol.cx = 40;
	lvCol.pszText = "Class";
	lvCol.iSubItem = 5;
	ListView_InsertColumn(hSearchResult, 5, &lvCol);

	lvCol.cx = 40;
	lvCol.pszText = "Lev";
	lvCol.iSubItem = 6;
	ListView_InsertColumn(hSearchResult, 6, &lvCol);

	lvCol.cx = 100;
	lvCol.pszText = "UT";
	lvCol.iSubItem = 7;
	ListView_InsertColumn(hSearchResult, 7, &lvCol);

	lvCol.cx = 90;
	lvCol.pszText = "CreDate";
	lvCol.iSubItem = 8;
	ListView_InsertColumn(hSearchResult, 8, &lvCol);

	lvCol.cx = 100;
	lvCol.pszText = "IP";
	lvCol.iSubItem = 9;
	ListView_InsertColumn(hSearchResult, 9, &lvCol);

	lvCol.cx = 45;
	lvCol.pszText = "Status";
	lvCol.iSubItem = 10;
	ListView_InsertColumn(hSearchResult, 10, &lvCol);
}
