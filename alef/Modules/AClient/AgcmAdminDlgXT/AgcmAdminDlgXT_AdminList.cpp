// AgcmAdminDlgXT_AdminList.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_AdminList dialog


AgcmAdminDlgXT_AdminList::AgcmAdminDlgXT_AdminList(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAdminDlgXT_AdminList::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAdminDlgXT_AdminList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bInitialized = FALSE;
}


void AgcmAdminDlgXT_AdminList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAdminDlgXT_AdminList)
	DDX_Control(pDX, IDC_B_ADMIN_SELECT, m_csSelectBtn);
	DDX_Control(pDX, IDC_LV_ADMIN, m_csAdminLV);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAdminDlgXT_AdminList, CDialog)
	//{{AFX_MSG_MAP(AgcmAdminDlgXT_AdminList)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_AdminList message handlers

BOOL AgcmAdminDlgXT_AdminList::Create()
{
	return TRUE;
}

BOOL AgcmAdminDlgXT_AdminList::OpenDlg(INT nShowCmd)
{
	if(::IsWindow(m_hWnd))
		SetFocus();
	else
		CDialog::Create(IDD, m_pParentWnd);

	ShowWindow(nShowCmd);

	return TRUE;
}

BOOL AgcmAdminDlgXT_AdminList::CloseDlg()
{
	if(::IsWindow(m_hWnd))
		ShowWindow(SW_HIDE);

	return TRUE;
}

BOOL AgcmAdminDlgXT_AdminList::IsInitialized()
{
	return m_bInitialized;
}

void AgcmAdminDlgXT_AdminList::Lock()
{
	m_csLock.Lock();
}

void AgcmAdminDlgXT_AdminList::Unlock()
{
	m_csLock.Unlock();
}

BOOL AgcmAdminDlgXT_AdminList::AddAdmin(stAgpdAdminInfo* pstAdminInfo)
{
	if(!pstAdminInfo)
		return FALSE;

	if(!pstAdminInfo->m_szAdminName && pstAdminInfo->m_lCID == 0)
		return FALSE;

	// 있는 지 확인 하면서 있으면 업데이트 한다.
	stAgpdAdminInfo* pstAdminInfoInList = GetAdmin(pstAdminInfo->m_szAdminName);
	if(pstAdminInfoInList)
	{
		memcpy(pstAdminInfoInList, pstAdminInfo, sizeof(stAgpdAdminInfo));
	}
	else
	{
		pstAdminInfoInList = new stAgpdAdminInfo;
		memcpy(pstAdminInfoInList, pstAdminInfo, sizeof(stAgpdAdminInfo));

		m_listAdmin.push_back(pstAdminInfoInList);
	}

	// Main Window 의 Admin Count 를 업데이트 한다.
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->UpdateAdminCount();

	ClearAdminList();
	return ShowAdminList();
}

stAgpdAdminInfo* AgcmAdminDlgXT_AdminList::GetAdmin(CHAR* szName)
{
	if(!szName)
		return NULL;

	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if(strcmp((*iterData)->m_szAdminName, szName) == 0)
			return *iterData;
		
		iterData++;
	}

	return NULL;
}

stAgpdAdminInfo* AgcmAdminDlgXT_AdminList::GetAdmin(INT32 lCID)
{
	if(lCID == 0)
		return NULL;

	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if((*iterData)->m_lCID == lCID)
			return *iterData;
		
		iterData++;
	}

	return NULL;
}

INT32 AgcmAdminDlgXT_AdminList::GetAdminCount()
{
	return m_listAdmin.size();
}

BOOL AgcmAdminDlgXT_AdminList::RemoveAdmin(stAgpdAdminInfo* pstAdminInfo)
{
	if(!pstAdminInfo)
		return FALSE;

	if(!pstAdminInfo->m_szAdminName && pstAdminInfo->m_lCID == 0)
		return FALSE;

	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if(strcmp((*iterData)->m_szAdminName, pstAdminInfo->m_szAdminName) == 0 ||
			(*iterData)->m_lCID == pstAdminInfo->m_lCID)
		{
			delete *iterData;
			m_listAdmin.erase(iterData);
			break;
		}
		
		iterData++;
	}

	// Main Window 의 Admin Count 를 업데이트 한다.
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->UpdateAdminCount();

	ClearAdminList();
	return ShowAdminList();
}

BOOL AgcmAdminDlgXT_AdminList::RemoveAdmin(CHAR* szName)
{
	if(!szName)
		return FALSE;

	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if(strcmp((*iterData)->m_szAdminName, szName) == 0)
		{
			delete *iterData;
			m_listAdmin.erase(iterData);
			break;
		}
		
		iterData++;
	}

	// Main Window 의 Admin Count 를 업데이트 한다.
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->UpdateAdminCount();

	ClearAdminList();
	return ShowAdminList();
}

BOOL AgcmAdminDlgXT_AdminList::RemoveAdmin(INT32 lCID)
{
	if(lCID == 0)
		return FALSE;

	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if((*iterData)->m_lCID == lCID)
		{
			delete *iterData;
			m_listAdmin.erase(iterData);
			break;
		}
		
		iterData++;
	}

	// Main Window 의 Admin Count 를 업데이트 한다.
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->UpdateAdminCount();

	ClearAdminList();
	return ShowAdminList();
}

BOOL AgcmAdminDlgXT_AdminList::ClearAdminListData()
{
	if(m_listAdmin.size() == 0)
		return FALSE;

	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if(*iterData)
			delete *iterData;
		
		iterData++;
	}

	m_listAdmin.clear();

	return TRUE;
}

BOOL AgcmAdminDlgXT_AdminList::ShowAdminList()
{
	if(!m_bInitialized)
		return FALSE;

	if(m_listAdmin.size() == 0)
		return FALSE;

	UpdateData();

	CHAR szTmp[255];
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;

	int iRows = 0;
	list<stAgpdAdminInfo*>::reverse_iterator iterData = m_listAdmin.rbegin();
	while(iterData != m_listAdmin.rend())
	{
		if(!*iterData)
			break;
		
		lvItem.iItem = iRows;

		// CharName
		lvItem.iSubItem = 0;
		lvItem.pszText = (*iterData)->m_szAdminName;
		m_csAdminLV.InsertItem(&lvItem);

		// AccName
		lvItem.iSubItem = 1;
		lvItem.pszText = (*iterData)->m_szAccName;
		m_csAdminLV.SetItem(&lvItem);

		// Level
		lvItem.iSubItem = 2;
		sprintf(szTmp, "%d", (*iterData)->m_lAdminLevel);
		lvItem.pszText = szTmp;
		m_csAdminLV.SetItem(&lvItem);

		iterData++; iRows++;
	}

	UpdateData(FALSE);

	// Main Window 의 Admin Count 를 업데이트 한다.
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->UpdateAdminCount();

	return TRUE;
}

BOOL AgcmAdminDlgXT_AdminList::ClearAdminList()
{
	if(!m_bInitialized)
		return FALSE;

	UpdateData();
	m_csAdminLV.DeleteAllItems();
	UpdateData(FALSE);

	return TRUE;
}







//////////////////////////////////////////////////////////////////////////
// Dialog Message
BOOL AgcmAdminDlgXT_AdminList::InitAdminListView()
{
	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;
	
	lvCol.cx = 90;
	lvCol.pszText = "CharName";
	lvCol.iSubItem = 0;
	m_csAdminLV.InsertColumn(0, &lvCol);

	lvCol.cx = 90;
	lvCol.pszText = "AccName";
	lvCol.iSubItem = 1;
	m_csAdminLV.InsertColumn(1, &lvCol);

	lvCol.cx = 50;
	lvCol.pszText = "Level";
	lvCol.iSubItem = 2;
	m_csAdminLV.InsertColumn(2, &lvCol);

	// 한 행만 누르면 한 줄이 선택되게 한다.
	m_csAdminLV.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

void AgcmAdminDlgXT_AdminList::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
}

BOOL AgcmAdminDlgXT_AdminList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bInitialized = TRUE;

	InitAdminListView();

	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;
	m_csSelectBtn.SetXButtonStyle(dwStyle);

	ShowAdminList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmAdminDlgXT_AdminList::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CloseDlg();
}
