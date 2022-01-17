// AuHistoryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AuResourceKeeper.h"
#include "AuHistoryDlg.h"
#include "AuRKSingleton.h"
#include "RKInclude.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AuHistoryDlg dialog


AuHistoryDlg::AuHistoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AuHistoryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AuHistoryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AuHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AuHistoryDlg)
	DDX_Control(pDX, IDC_HISTORY, m_ctrlHistory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AuHistoryDlg, CDialog)
	//{{AFX_MSG_MAP(AuHistoryDlg)
	ON_BN_CLICKED(IDC_GET_FILE, OnGetFile)
	ON_NOTIFY(NM_CLICK, IDC_HISTORY, OnClickHistory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AuHistoryDlg message handlers

BOOL AuHistoryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_iLastClickIndex = -1;

	InitListCtrl();
	InitHistory();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void AuHistoryDlg::InitListCtrl()
{
	// List View의 Header 초기화
	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (int i = 0; i < 3; i++)
	{
		lvcolumn.iSubItem = i;
		switch(i)
		{
		case 0:
			lvcolumn.cx = 70;
			lvcolumn.pszText = "Version";
			break;

		case 1:
			lvcolumn.cx = 105;
			lvcolumn.pszText = "User";
			break;

		case 2:
			lvcolumn.cx = 140;
			lvcolumn.pszText = "Date-Time";
			break;
		}

		m_ctrlHistory.InsertColumn(i, &lvcolumn);
	}

	m_ctrlHistory.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}
void AuHistoryDlg::InitHistory()
{
	CHAR szTmp[32];
	memset(szTmp, 0, sizeof(CHAR) * 32);

	int iSize = m_csList.GetCount();
	POSITION p = m_csList.GetHeadPosition();
	for(int i = 0; i < iSize; i++)
	{
		FileInfo FileInfo = m_csList.GetNext(p);

		sprintf(szTmp, "%d", FileInfo.m_iVersion);
		AddListItem(szTmp, FileInfo.m_strOwner, FileInfo.m_strDate);

		if(i == 0)
		{
			m_strFileName = FileInfo.m_strFileName;
		}
	}
}

// 2004.11.25. steeple
void AuHistoryDlg::SetHistoryList(CList<FileInfo, FileInfo>* pList)
{
	if(!pList)
		return;

	int iSize = pList->GetCount();
	POSITION p = pList->GetHeadPosition();
	for(int i = 0; i < iSize; i++)
	{
		FileInfo FileInfo = m_csList.GetNext(p);
		m_csList.AddTail(FileInfo);
	}
}

void AuHistoryDlg::AddListItem(LPCTSTR lpVer, LPCTSTR lpUser, LPCTSTR lpDate)
{
	LV_ITEM lvItem;
	lvItem.iItem = m_ctrlHistory.GetItemCount();
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_STATEIMAGEMASK;
	lvItem.state = 0;
	lvItem.pszText = (LPSTR)(LPCTSTR)lpVer;

	m_ctrlHistory.InsertItem(&lvItem);

	m_ctrlHistory.SetItemText(lvItem.iItem, 1, lpUser);
	m_ctrlHistory.SetItemText(lvItem.iItem, 2, lpDate);
}


void AuHistoryDlg::OnGetFile() 
{
	// TODO: Add your control notification handler code here
	INT32 lRequestVersion = ::atoi(m_ctrlHistory.GetItemText(m_iLastClickIndex, 0));
	INT32 lCurrentVersion = AuRKSingleton::Instance()->GetCurrentVersion(m_strFileName);
	
	CString szRemotePath = m_strFileName;
	AuRKSingleton::Instance()->ConvertRemotePath(szRemotePath);
	CString strFileName = AuRKSingleton::Instance()->GetFileName(szRemotePath);

	CRKEntryManager cRKEntryManager;
	CString strBackupFile;
	if(lRequestVersion == lCurrentVersion)
		strBackupFile = szRemotePath;
	else
		strBackupFile = cRKEntryManager.GetBackupFilePath(szRemotePath, lRequestVersion);

	CFileDialog FileDlg(FALSE, NULL, strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	if (IDOK == FileDlg.DoModal())
	{
		strBackupFile.MakeUpper();
		::CopyFile(strBackupFile, FileDlg.GetPathName(), FALSE);
	}
}

void AuHistoryDlg::OnClickHistory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	// 유요하지 않은 아이템이 선택되었으면 return
	if (!m_ctrlHistory.GetFirstSelectedItemPosition()) return; 

	// 마지막으로 선택된 List View의 인덱스를 저장
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_iLastClickIndex = pNMListView->iItem;

	*pResult = 0;
}
