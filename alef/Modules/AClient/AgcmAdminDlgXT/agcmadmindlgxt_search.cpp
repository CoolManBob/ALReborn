// agcmadmindlgxt_search.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Search dialog


AgcmAdminDlgXT_Search::AgcmAdminDlgXT_Search(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAdminDlgXT_Search::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAdminDlgXT_Search)
	m_szSearchName = _T("");
	//}}AFX_DATA_INIT

	m_bInitialized = FALSE;

	m_pfCBSearch = NULL;

	SetCharData(NULL);
	m_szClickName = _T("");
}


void AgcmAdminDlgXT_Search::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAdminDlgXT_Search)
	DDX_Control(pDX, IDC_E_CHAR_SEARCH_DATA, m_csSearchEdit);
	DDX_Control(pDX, IDC_LV_SEARCH_RESULT, m_csResultListView);
	DDX_Control(pDX, IDC_CB_SEARCH_TYPE, m_csTypeCB);
	DDX_Control(pDX, IDC_CB_SEARCH_FIELD, m_csFieldCB);
	DDX_Control(pDX, IDC_BTN_SEARCH_SEARCH, m_csSearchBtn);
	DDX_Control(pDX, IDC_BTN_SEARCH_MODIFY, m_csModifyBtn);
	DDX_Control(pDX, IDC_BTN_SEARCH_GO, m_csGoBtn);
	DDX_Control(pDX, IDC_BTN_SEARCH_GM, m_csGMBtn);
	DDX_Control(pDX, IDC_BTN_SEARCH_ADD, m_csAddBtn);
	DDX_Text(pDX, IDC_E_CHAR_SEARCH_DATA, m_szSearchName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAdminDlgXT_Search, CDialog)
	//{{AFX_MSG_MAP(AgcmAdminDlgXT_Search)
	ON_BN_CLICKED(IDC_BTN_SEARCH_ADD, OnBtnSearchAdd)
	ON_BN_CLICKED(IDC_BTN_SEARCH_GM, OnBtnSearchGm)
	ON_BN_CLICKED(IDC_BTN_SEARCH_GO, OnBtnSearchGo)
	ON_BN_CLICKED(IDC_BTN_SEARCH_MODIFY, OnBtnSearchModify)
	ON_BN_CLICKED(IDC_BTN_SEARCH_SEARCH, OnBtnSearchSearch)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CLICK, IDC_LV_SEARCH_RESULT, OnClickLvSearchResult)
	ON_NOTIFY(NM_DBLCLK, IDC_LV_SEARCH_RESULT, OnDblclkLvSearchResult)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Search message handlers

BOOL AgcmAdminDlgXT_Search::Create()
{
	return TRUE;
}

BOOL AgcmAdminDlgXT_Search::OpenDlg(INT nShowCmd)
{
	if(::IsWindow(m_hWnd))
		SetFocus();
	else
		CDialog::Create(IDD, m_pParentWnd);

	ShowWindow(nShowCmd);

	return TRUE;
}

BOOL AgcmAdminDlgXT_Search::CloseDlg()
{
	if(::IsWindow(m_hWnd))
		ShowWindow(SW_HIDE);
	
	return TRUE;
}

BOOL AgcmAdminDlgXT_Search::IsInitialized()
{
	return m_bInitialized;
}

void AgcmAdminDlgXT_Search::Lock()
{
	m_csLock.Lock();
}

void AgcmAdminDlgXT_Search::Unlock()
{
	m_csLock.Unlock();
}

BOOL AgcmAdminDlgXT_Search::SetCBSearch(ADMIN_CB pfCBSearch)
{
	m_pfCBSearch = pfCBSearch;
	return TRUE;
}

BOOL AgcmAdminDlgXT_Search::SetCharData(stAgpdAdminCharData* pstCharData)
{
	if(pstCharData)
		memcpy(&m_stCharData, pstCharData, sizeof(m_stCharData));
	else
		memset(&m_stCharData, 0, sizeof(m_stCharData));

	return TRUE;
}

stAgpdAdminCharData* AgcmAdminDlgXT_Search::GetCharData()
{
	return &m_stCharData;
}

BOOL AgcmAdminDlgXT_Search::SetResultList(PVOID pList)
{
	list<stAgpdAdminSearchResult*>* pListSearchResult = (list<stAgpdAdminSearchResult*>*)pList;
	if(!pListSearchResult)
		return FALSE;

	// 일단 멤버데이터를 다시한번 비우고.. 그러나 이미 비어져 있을 것이다.
	ClearResultList();
	ClearResultListView();

	// pListSearchResult 를 멤버 데이터에 세팅한다.
	list<stAgpdAdminSearchResult*>::iterator iterData = pListSearchResult->begin();
	while(iterData != pListSearchResult->end())
	{
		stAgpdAdminSearchResult* pstSearchResult = new stAgpdAdminSearchResult;
		memcpy(pstSearchResult, (*iterData), sizeof(stAgpdAdminSearchResult));
		m_listSearchResult.push_back(pstSearchResult);

		iterData++;
	}

	return ShowData();
}

BOOL AgcmAdminDlgXT_Search::SetResult(stAgpdAdminSearch* pstSearch, stAgpdAdminSearchResult* pstSearchResult)
{
	if(pstSearchResult == NULL)
		return FALSE;

	// 이미 리스트에 있으면 그냥 나간다.
	if(IsInResultList(pstSearchResult))
		return FALSE;

	stAgpdAdminSearchResult* pstSearchResult2 = new stAgpdAdminSearchResult;
	memcpy(pstSearchResult2, pstSearchResult, sizeof(stAgpdAdminSearchResult));
	m_listSearchResult.push_back(pstSearchResult2);

	return ShowData();
}

BOOL AgcmAdminDlgXT_Search::IsInResultList(stAgpdAdminSearchResult* pstSearchResult)
{
	if(pstSearchResult == NULL)
		return FALSE;

	BOOL bResult = FALSE;

	list<stAgpdAdminSearchResult*>::iterator iterData = m_listSearchResult.begin();
	while(iterData != m_listSearchResult.end())
	{
		if(strcmp(pstSearchResult->m_szCharName, (*iterData)->m_szCharName) == 0)
		{
			bResult = TRUE;
			break;
		}

		iterData++;
	}

	return bResult;
}

stAgpdAdminSearchResult* AgcmAdminDlgXT_Search::GetSearchResult(CString& szName)
{
	if(szName.IsEmpty())
		return NULL;

	list<stAgpdAdminSearchResult*>::iterator iterData = m_listSearchResult.begin();
	while(iterData != m_listSearchResult.end())
	{
		if(strcmp((*iterData)->m_szCharName, (LPCTSTR)szName) == 0)
			return *iterData;

		iterData++;
	}

	return NULL;
}

// 외부에서 사용하고 싶을 때 이 함수를 쓴다.
BOOL AgcmAdminDlgXT_Search::ProcessSearch(CHAR* szName)
{
	if(!m_pfCBSearch || !AgcmAdminDlgXT_Manager::Instance()->GetCBClass())
		return FALSE;

	if(!szName || strlen(szName) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	strcpy(stSearch.m_szSearchName, szName);

	// Set Last Search
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetLastSearch(&stSearch);

	// Callback Execute
	m_pfCBSearch(&stSearch, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);

	// Open Character Dlg
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->OpenDlg();

	return TRUE;
}

BOOL AgcmAdminDlgXT_Search::ShowData()
{
	if(!m_bInitialized)
		return FALSE;

	UpdateData();

	// 화면만 비운다.
	ClearResultListView();

	CHAR szTmp[255];
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	int iRows = 0;
	list<stAgpdAdminSearchResult*>::iterator iterData = m_listSearchResult.begin();
	while(iterData != m_listSearchResult.end())
	{
		if((*iterData) == NULL)
			break;

		lvItem.iItem = iRows;

		// CharName
		lvItem.iSubItem = 0;
		lvItem.pszText = (*iterData)->m_szCharName;
		m_csResultListView.InsertItem(&lvItem);

		// AccName
		lvItem.iSubItem = 1;
		lvItem.pszText = (*iterData)->m_szAccName;
		m_csResultListView.SetItem(&lvItem);

		// Level
		lvItem.iSubItem = 2;
		sprintf(szTmp, "%d", (*iterData)->m_lLevel);
		lvItem.pszText = szTmp;
		m_csResultListView.SetItem(&lvItem);
		
		// Race
		lvItem.iSubItem = 3;
		lvItem.pszText = (*iterData)->m_szRace;
		m_csResultListView.SetItem(&lvItem);

		// Class
		lvItem.iSubItem = 4;
		lvItem.pszText = (*iterData)->m_szClass;
		m_csResultListView.SetItem(&lvItem);

		// Status
		lvItem.iSubItem = 5;
		if((*iterData)->m_lStatus == 1)
			lvItem.pszText = "Online";
		else
			lvItem.pszText = "Offline";
		m_csResultListView.SetItem(&lvItem);

		// CreDate
		lvItem.iSubItem = 6;
		lvItem.pszText = (*iterData)->m_szCreDate;
		m_csResultListView.SetItem(&lvItem);

		iterData++; iRows++;
	}

	return TRUE;
}

BOOL AgcmAdminDlgXT_Search::ClearResultList()
{
	if(m_listSearchResult.size() == 0)
		return FALSE;

	list<stAgpdAdminSearchResult*>::iterator iterData = m_listSearchResult.begin();
	while(iterData != m_listSearchResult.end())
	{
		if(*iterData)
			delete *iterData;

		iterData++;
	}

	m_listSearchResult.clear();

	return TRUE;
}

BOOL AgcmAdminDlgXT_Search::ClearResultListView()
{
	if(!m_bInitialized)
		return FALSE;

	UpdateData();
	m_csResultListView.DeleteAllItems();
	return TRUE;
}











///////////////////////////////////////////////////////////////////////////
// Dialog Handler

BOOL AgcmAdminDlgXT_Search::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bInitialized = TRUE;

	m_csTypeCB.AddString("PC");
	//m_csTypeCB.AddString("NPC");
	m_csTypeCB.SetCurSel(0);

	m_csFieldCB.AddString("CharName");
	m_csFieldCB.AddString("AccName");
	m_csFieldCB.SetCurSel(0);
	
	OnInitResultListView();

	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;

	m_csSearchBtn.SetXButtonStyle(dwStyle);
	m_csModifyBtn.SetXButtonStyle(dwStyle);
	m_csGoBtn.SetXButtonStyle(dwStyle);
	m_csGMBtn.SetXButtonStyle(dwStyle);
	m_csGMBtn.SetXButtonStyle(dwStyle);
	m_csAddBtn.SetXButtonStyle(dwStyle);
	
	m_csSearchEdit.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL AgcmAdminDlgXT_Search::OnInitResultListView()
{
	if(!m_bInitialized)
		return FALSE;

	UpdateData();

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 80;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 0;
	m_csResultListView.InsertColumn(0, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = "AccName";
	lvCol.iSubItem = 1;
	m_csResultListView.InsertColumn(1, &lvCol);

	lvCol.cx = 40;
	lvCol.pszText = "Lev";
	lvCol.iSubItem = 2;
	m_csResultListView.InsertColumn(2, &lvCol);

	lvCol.cx = 45;
	lvCol.pszText = "Race";
	lvCol.iSubItem = 3;
	m_csResultListView.InsertColumn(3, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Class";
	lvCol.iSubItem = 4;
	m_csResultListView.InsertColumn(4, &lvCol);

	lvCol.cx = 55;
	lvCol.pszText = "Status";
	lvCol.iSubItem = 5;
	m_csResultListView.InsertColumn(5, &lvCol);
	
	lvCol.cx = 90;
	lvCol.pszText = "CreDate";
	lvCol.iSubItem = 6;
	m_csResultListView.InsertColumn(6, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	m_csResultListView.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

void AgcmAdminDlgXT_Search::OnBtnSearchAdd() 
{
	// TODO: Add your control notification handler code here
	if(m_szClickName.IsEmpty() == FALSE)
	{
		stAgpdAdminSearchResult* pstSearchResult = GetSearchResult(m_szClickName);
		if(!pstSearchResult)
			return;

		stAgpdAdminPickingData stPickingData;
		memset(&stPickingData, 0, sizeof(stAgpdAdminPickingData));

		//stPickingData.m_eType = pstSearchResult->m_;
		stPickingData.m_lCID = pstSearchResult->m_lCID;
		strcpy(stPickingData.m_szName, pstSearchResult->m_szCharName);

		AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->AddObject(&stPickingData);
	}
}

void AgcmAdminDlgXT_Search::OnBtnSearchGm() 
{
	// TODO: Add your control notification handler code here
	
}

void AgcmAdminDlgXT_Search::OnBtnSearchGo() 
{
	// TODO: Add your control notification handler code here
	
}

void AgcmAdminDlgXT_Search::OnBtnSearchModify() 
{
	// TODO: Add your control notification handler code here
	
}

void AgcmAdminDlgXT_Search::OnBtnSearchSearch() 
{
	// TODO: Add your control notification handler code here
	if(m_pfCBSearch == NULL || AgcmAdminDlgXT_Manager::Instance()->GetCBClass() == NULL)
		return;

	UpdateData();

	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	// Type 을 얻는다.
	stSearch.m_iType = (INT8)m_csTypeCB.GetCurSel();

	// Field 를 얻는다.
	stSearch.m_iField = (INT8)m_csFieldCB.GetCurSel();

	strcpy(stSearch.m_szSearchName, (LPCTSTR)m_szSearchName);

	// Callback Execute
	m_pfCBSearch(&stSearch, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);

	// Clear Result List
	ClearResultList();
	ClearResultListView();
}

void AgcmAdminDlgXT_Search::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CloseDlg();
}

void AgcmAdminDlgXT_Search::OnClickLvSearchResult(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(!m_bInitialized)
		return;

	UpdateData();
	
	LPNMLISTVIEW nlv = (LPNMLISTVIEW)pNMHDR;
	if(nlv->iItem >= 0)
		m_szClickName = m_csResultListView.GetItemText(nlv->iItem, 0);
	else
		m_szClickName.Empty();
	
	*pResult = 0;
}

void AgcmAdminDlgXT_Search::OnDblclkLvSearchResult(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(!m_bInitialized)
		return;

	UpdateData();
	
	LPNMLISTVIEW nlv = (LPNMLISTVIEW)pNMHDR;

	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));

	// CharName 으로 다시 검색 패킷을 날린다.
	CString szTmp;
	szTmp = m_csResultListView.GetItemText(nlv->iItem, 0);
	strcpy(stSearch.m_szSearchName, (LPCTSTR)szTmp);

	szTmp = m_csResultListView.GetItemText(nlv->iItem, 1);
	stSearch.m_lObjectCID = atoi((LPCTSTR)szTmp);

	// Set Last Search
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetLastSearch(&stSearch);

	// 2004.08.24.
	AgcmAdminDlgXT_Manager::Instance()->GetCharItemDlg()->SetCharName(stSearch.m_szSearchName);

	// Callback Execute
	m_pfCBSearch(&stSearch, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);

	// Open Character Dlg
	AgcmAdminDlgXT_Manager::Instance()->OpenCharacterDlg();
	
	*pResult = 0;
}


void AgcmAdminDlgXT_Search::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class

	ClearResultList();
	
	CDialog::PostNcDestroy();
}
