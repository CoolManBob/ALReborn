// AuUsersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AuResourceKeeper.h"
#include "AuUsersDlg.h"
#include "AuRKSingleton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AuUsersDlg dialog


AuUsersDlg::AuUsersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AuUsersDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AuUsersDlg)
	m_strUserID = _T("");
	m_strPassword = _T("");
	//}}AFX_DATA_INIT
}


void AuUsersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AuUsersDlg)
	DDX_Control(pDX, IDC_USERS_LIST, m_ctrlUsers);
	DDX_Text(pDX, IDC_USER_ID, m_strUserID);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AuUsersDlg, CDialog)
	//{{AFX_MSG_MAP(AuUsersDlg)
	ON_BN_CLICKED(IDC_USER_DELETE, OnUserDelete)
	ON_BN_CLICKED(IDC_ADD_USER, OnAddUser)
	ON_NOTIFY(NM_CLICK, IDC_USERS_LIST, OnClickUsersList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AuUsersDlg message handlers

BOOL AuUsersDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_iRClickIndex = -1;
	
	// TODO: Add extra initialization here
	InitListCtrl();
	InitUserList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AuUsersDlg::InitListCtrl()
{
	// List View의 Header 초기화

	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (int i = 0; i < 1; i++)
	{
		lvcolumn.iSubItem = i;
		switch(i)
		{
		case 0:
			lvcolumn.cx = 120;
			lvcolumn.pszText = "User ID";
			break;

//		case 1:
//			lvcolumn.cx = 100;
//			lvcolumn.pszText = "Login";
//			break;
		}

		m_ctrlUsers.InsertColumn(i, &lvcolumn);
	}	

	m_ctrlUsers.SetExtendedStyle(LVS_EX_FULLROWSELECT);

}
void AuUsersDlg::InitUserList()
{
	m_UserManager.SetRemotePath(AuRKSingleton::Instance()->m_strRepoRootPath);

	CList<stUserInfo, stUserInfo> cList;
	m_UserManager.GetUserList(cList, AuRKSingleton::Instance()->m_strRepoRootPath);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();
	
	for(INT32 i = 0; i < iSize; i++)
	{
		stUserInfo UserInfo = cList.GetNext(p);
		AddListItem(UserInfo.szUserName);
	}
}

void AuUsersDlg::AddListItem(LPCTSTR lpUserID)
{
	LV_ITEM lvItem;
	lvItem.iItem = m_ctrlUsers.GetItemCount();
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_STATEIMAGEMASK;
	lvItem.state = 0;
	lvItem.pszText = (LPSTR)(LPCTSTR)lpUserID;

	m_ctrlUsers.InsertItem(&lvItem);
}

void AuUsersDlg::OnUserDelete() 
{
	// TODO: Add your control notification handler code here
	if (-1 == m_iRClickIndex) return;

	m_UserManager.RemoveUser(m_ctrlUsers.GetItemText(m_iRClickIndex, 0), AuRKSingleton::Instance()->m_strRepoRootPath);
	m_ctrlUsers.DeleteItem(m_iRClickIndex);
}

void AuUsersDlg::OnAddUser() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_UserManager.AddUser(m_strUserID, m_strPassword, AuRKSingleton::Instance()->m_strRepoRootPath))
	{
		AddListItem(m_strUserID);

		m_strUserID.Empty();
		m_strPassword.Empty();
		UpdateData(FALSE);
	}
	

}

void AuUsersDlg::OnClickUsersList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if (!m_ctrlUsers.GetFirstSelectedItemPosition()) return; 

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_iRClickIndex = pNMListView->iItem;

	*pResult = 0;
}
