// AuSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AuResourceKeeper.h"
#include "AuSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AuSelectDlg dialog


AuSelectDlg::AuSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AuSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AuSelectDlg)
	//}}AFX_DATA_INIT
}


void AuSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AuSelectDlg)
	DDX_Control(pDX, IDC_SELECT, m_ctrlSelect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AuSelectDlg, CDialog)
	//{{AFX_MSG_MAP(AuSelectDlg)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_CLEAN, OnClean)
	ON_BN_CLICKED(IDC_GET_FILES, OnGetFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AuSelectDlg message handlers

BOOL AuSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitListCtrl();
	InitListItem();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void AuSelectDlg::InitListItem()
{
	POSITION p = m_pUpdatedFileList->GetHeadPosition();
	while (p)
	{
		FileInfo stFileInfo = m_pUpdatedFileList->GetNext(p);
		AddListItem(stFileInfo.m_strFileName, stFileInfo.m_strDate);
	}
}

void AuSelectDlg::SetFileList(CList<FileInfo, FileInfo> *pUpdatedFileList, CList<FileInfo, FileInfo> *pSelectedFileList)
{
	m_pUpdatedFileList = pUpdatedFileList;
	m_pSelectedFileList = pSelectedFileList;
}

void AuSelectDlg::AddListItem(LPCTSTR lpText, LPCTSTR lpDate)
{
	LV_ITEM lvItem;
	lvItem.iItem = m_ctrlSelect.GetItemCount();
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_STATEIMAGEMASK;
	lvItem.state = 0;
	lvItem.pszText = (LPSTR)(LPCTSTR)"";

	m_ctrlSelect.InsertItem(&lvItem);
	m_ctrlSelect.SetItemText(lvItem.iItem, 1, lpText);
	m_ctrlSelect.SetItemText(lvItem.iItem, 2, lpDate);
}

void AuSelectDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}

void AuSelectDlg::SetAllItemCheck(BOOL bCheck)
{
	INT32 lCount = m_ctrlSelect.GetItemCount();

	for (INT32 i = 0; i < lCount; i++)
	{
		m_ctrlSelect.SetCheck(i, bCheck);
	}
}

void AuSelectDlg::OnSelectAll() 
{
	// TODO: Add your control notification handler code here
	SetAllItemCheck(TRUE);
}

void AuSelectDlg::OnClean() 
{
	// TODO: Add your control notification handler code here
	SetAllItemCheck(FALSE);
}

void AuSelectDlg::OnGetFiles() 
{
	// TODO: Add your control notification handler code here
	INT32 lCount = m_ctrlSelect.GetItemCount();
	for (INT32 i = 0; i < lCount; i++)
	{
		if (m_ctrlSelect.GetCheck(i))
		{
			FileInfo stFileInfo;
			stFileInfo.m_strFileName = m_ctrlSelect.GetItemText(i, 1);
			stFileInfo.m_strDate = m_ctrlSelect.GetItemText(i, 2);
			m_pSelectedFileList->AddTail(stFileInfo);
		}
	}

	CDialog::OnOK();
}

void AuSelectDlg::InitListCtrl()
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
			lvcolumn.cx = 22;
			lvcolumn.pszText = "";
			break;

		case 1:
			lvcolumn.cx = 310;
			lvcolumn.pszText = "Filename";
			break;

		case 2:
			lvcolumn.cx = 125;
			lvcolumn.pszText = "Date-Time";
			break;
		}

		m_ctrlSelect.InsertColumn(i, &lvcolumn);
	}	

	m_ctrlSelect.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
}