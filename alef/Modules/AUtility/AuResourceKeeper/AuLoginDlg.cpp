// AuLoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AuResourceKeeper.h"
#include "AuLoginDlg.h"
#include "AuRKSingleton.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AuLoginDlg dialog


AuLoginDlg::AuLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AuLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AuLoginDlg)
	m_strPassword = _T("");
	m_strUserID = _T("");
	//}}AFX_DATA_INIT
}


void AuLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AuLoginDlg)
	DDX_Control(pDX, IDC_REPO_LIST, m_ctrlList);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_USERID, m_strUserID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AuLoginDlg, CDialog)
	//{{AFX_MSG_MAP(AuLoginDlg)
	ON_BN_CLICKED(IDOK, OnLogin)
	ON_NOTIFY(NM_CLICK, IDC_REPO_LIST, OnClickRepoList)
	ON_BN_CLICKED(IDC_ADD_REPOSITORY, OnAddRepository)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AuLoginDlg message handlers

void AuLoginDlg::OnLogin() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	stRepositoryData* pRepoData = m_Repository.GetRepositoryData(m_iLastClickIndex);

	// Admin으로의 로그인확인
	if ((m_strUserID == ADMIN_ID) && (m_strPassword == ADMIN_PASSWORD))
	{
		m_bLoginSuccess = TRUE;
		
		AuRKSingleton::Instance()->m_bLogin = m_bLoginSuccess;
		AuRKSingleton::Instance()->m_bAdmin = TRUE;

		if (pRepoData)
		{
			AuRKSingleton::Instance()->m_strRepoRootPath = pRepoData->szPath;
			AuRKSingleton::Instance()->m_strRepoName = pRepoData->szName;
			AuRKSingleton::Instance()->m_strWorkingFolder = pRepoData->szWorkingFolder;
			
			AuRKSingleton::Instance()->m_strRepoRootPath.MakeUpper();
			AuRKSingleton::Instance()->m_strRepoName.MakeUpper();
			AuRKSingleton::Instance()->m_strWorkingFolder.MakeUpper();
		}

		CDialog::OnOK();
		return;
	}

	if (pRepoData)
	{
		// 일반 사용자 로그인 확인
		m_UserManager.SetRemotePath(pRepoData->szPath);
		m_bLoginSuccess = m_UserManager.Login(m_strUserID, m_strPassword);

		// 로그인 정보 보관
		AuRKSingleton::Instance()->m_bLogin = m_bLoginSuccess;
		AuRKSingleton::Instance()->m_bAdmin = m_UserManager.IsAdmin();
		
		// 로그인 되었을때만 설정
		if (m_bLoginSuccess)
		{
			AuRKSingleton::Instance()->m_strRepoRootPath = pRepoData->szPath;
			AuRKSingleton::Instance()->m_strUserID = m_strUserID;
			AuRKSingleton::Instance()->m_strWorkingFolder = pRepoData->szWorkingFolder;
			AuRKSingleton::Instance()->m_strRepoName = pRepoData->szName;

			AuRKSingleton::Instance()->m_strRepoRootPath.MakeUpper();
			AuRKSingleton::Instance()->m_strWorkingFolder.MakeUpper();
			AuRKSingleton::Instance()->m_strRepoName.MakeUpper();

			CDialog::OnOK();
		}
		else
		{
			MessageBox("ID 혹은 Password가 틀렸습니다.");
		}
	}
	else
	{
		MessageBox("유효한 Repository가 아닙니다.");
	}
}


BOOL AuLoginDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bLoginSuccess = FALSE;
	m_iLastClickIndex = -1;
	// TODO: Add extra initialization here
	InitListCtrl();
	InitConfig();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void AuLoginDlg::InitListCtrl()
{
	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (int i = 0; i < 2; i++)
	{
		lvcolumn.iSubItem = i;
		switch(i)
		{
		case 0:
			lvcolumn.cx = 100;
			lvcolumn.pszText = "Name";
			break;

		case 1:
			lvcolumn.cx = 175;
			lvcolumn.pszText = "Path";
			break;
		}

		m_ctrlList.InsertColumn(i, &lvcolumn);
	}	

	m_ctrlList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}

BOOL AuLoginDlg::InitConfig()
{
	// INFO_FILENAME 파일 설정
	m_Repository.SetInfoFileName(AuRKSingleton::Instance()->m_strCurrentPath + PATH(INFO_FILENAME));

	// INFO_FILENAME 파일 열기
	if (!m_Repository.LoadInfoFile()) 
		return FALSE;

	// INFO_FILENAME에서 Repository List 얻어오기
	CList<stRepositoryData, stRepositoryData> cList;
	INT16 nCount = m_Repository.GetRepositoryList(cList);
	POSITION Pos = cList.GetHeadPosition();

	for (INT16 i = 0; i < nCount; i++)
	{
		stRepositoryData RepoData = cList.GetNext(Pos);
		AddListItem(RepoData.szName, RepoData.szPath);
	}

	// Default Repository가 설정
	INT16 nDefaultIndex = m_Repository.GetDefaultIndex();

	if (nDefaultIndex != -1)
	{
		if (nCount >= nDefaultIndex)
		{
			m_iLastClickIndex = nDefaultIndex;

			// JNY TODO : Default Index를 선택되게 하는 기능 추후구현
		}
	}

	return TRUE;
}

void AuLoginDlg::OnClickRepoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// 유요하지 않은 아이템이 선택되었으면 return
	if (!m_ctrlList.GetFirstSelectedItemPosition())
	{
		m_iLastClickIndex = -1;
		return; 
	}

	// 마지막으로 선택된 List View의 인덱스를 저장
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_iLastClickIndex = pNMListView->iItem;

	*pResult = 0;
}

void AuLoginDlg::AddListItem(LPCTSTR lpName, LPCTSTR lpPath)
{
	CString strName = lpName;
	strName.TrimLeft();
	strName.TrimRight();
//	if (strName.IsEmpty()) return;

	LV_ITEM lvItem;
	lvItem.iItem = m_ctrlList.GetItemCount();
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_STATEIMAGEMASK;
	lvItem.state = 0;
	lvItem.pszText = (LPSTR)(LPCTSTR)lpName;

	m_ctrlList.InsertItem(&lvItem);

	m_ctrlList.SetItemText(lvItem.iItem, 1, lpPath);
}

void AuLoginDlg::OnAddRepository() 
{
	// TODO: Add your control notification handler code here
	CString strRepoName, strPathName;
	AuRKSingleton::Instance()->GetRepoPath(strRepoName, strPathName);

	if(!AuRKSingleton::Instance()->IsFileExist(strPathName + PATH(RKADMIN)))  return;
//	if(strPathName.Find(RKADMIN) == -1 ) return;

	if (m_Repository.AddRepository(strRepoName, strPathName, CString("")))
	{
		m_ctrlList.DeleteAllItems();
		InitConfig();
	}

}

void AuLoginDlg::OnRemove() 
{
	// Repository List 삭제
	if (m_Repository.RemoveRepository(m_ctrlList.GetItemText(m_iLastClickIndex, 0)))
	{
		// List에서도 삭제
		m_ctrlList.DeleteAllItems();
		InitConfig();
	}
}
