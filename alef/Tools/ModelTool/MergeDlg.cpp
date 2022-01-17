// MergeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modeltool.h"
#include "MergeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeDlg dialog


CMergeDlg::CMergeDlg(CMergeDlgSet *pcsSet, CHAR *pszFindFilePathName1, CWnd* pParent /*=NULL*/)
	: CDialog(CMergeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMergeDlg)
	m_strMerge1 = _T("");
	m_strMerge2 = _T("");
	m_strMerge3 = _T("");
	m_strMerge4 = _T("");
	m_strMerge5 = _T("");
	m_strMerge6 = _T("");
	m_strMerge7 = _T("");
	m_strMerge8 = _T("");
	m_strMerge9 = _T("");
	//}}AFX_DATA_INIT

	m_pcsSet = pcsSet;
	strcpy(m_szFindFilePathName1, pszFindFilePathName1);
}


void CMergeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeDlg)
	DDX_Text(pDX, IDC_EDIT_MERGE_1, m_strMerge1);
	DDX_Text(pDX, IDC_EDIT_MERGE_2, m_strMerge2);
	DDX_Text(pDX, IDC_EDIT_MERGE_3, m_strMerge3);
	DDX_Text(pDX, IDC_EDIT_MERGE_4, m_strMerge4);
	DDX_Text(pDX, IDC_EDIT_MERGE_5, m_strMerge5);
	DDX_Text(pDX, IDC_EDIT_MERGE_6, m_strMerge6);
	DDX_Text(pDX, IDC_EDIT_MERGE_7, m_strMerge7);
	DDX_Text(pDX, IDC_EDIT_MERGE_8, m_strMerge8);
	DDX_Text(pDX, IDC_EDIT_MERGE_9, m_strMerge9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMergeDlg, CDialog)
	//{{AFX_MSG_MAP(CMergeDlg)
	ON_BN_CLICKED(IDC_BUTTON_SET_MERGE_1, OnButtonSetMerge1)
	ON_BN_CLICKED(IDC_BUTTON_SET_MERGE_2, OnButtonSetMerge2)
	ON_BN_CLICKED(IDC_BUTTON_SET_MERGE_3, OnButtonSetMerge3)
	ON_BN_CLICKED(IDC_BUTTON_SET_MERGE_4, OnButtonSetMerge4)
	ON_BN_CLICKED(IDC_BUTTON_SET_MERGE_5, OnButtonSetMerge5)
	ON_BN_CLICKED(IDC_BUTTON_SET_MERGE_6, OnButtonSetMerge6)
	ON_BN_CLICKED(IDC_BUTTON_SET_MERGE_7, OnButtonSetMerge7)
	ON_BN_CLICKED(IDC_BUTTON_SET_MERGE_8, OnButtonSetMerge8)
	ON_BN_CLICKED(IDC_BUTTON_SET_MERGE_9, OnButtonSetMerge9)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDlg message handlers

void CMergeDlg::OnButtonSetMerge1() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszRt;
	AgcmFileListDlg dlg;

	pszRt = dlg.OpenFileList(m_szFindFilePathName1, NULL, NULL);
	if(pszRt)
	{
		m_strMerge1 = pszRt;
		UpdateData(FALSE);
	}
}

void CMergeDlg::OnButtonSetMerge2() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszRt;
	AgcmFileListDlg dlg;

	pszRt = dlg.OpenFileList(m_szFindFilePathName1, NULL, NULL);
	if(pszRt)
	{
		m_strMerge2 = pszRt;
		UpdateData(FALSE);
	}
}

void CMergeDlg::OnButtonSetMerge3() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszRt;
	AgcmFileListDlg dlg;

	pszRt = dlg.OpenFileList(m_szFindFilePathName1, NULL, NULL);
	if(pszRt)
	{
		m_strMerge3 = pszRt;
		UpdateData(FALSE);
	}
}

void CMergeDlg::OnButtonSetMerge4() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszRt;
	AgcmFileListDlg dlg;

	pszRt = dlg.OpenFileList(m_szFindFilePathName1, NULL, NULL);
	if(pszRt)
	{
		m_strMerge4 = pszRt;
		UpdateData(FALSE);
	}
}

void CMergeDlg::OnButtonSetMerge5() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszRt;
	AgcmFileListDlg dlg;

	pszRt = dlg.OpenFileList(m_szFindFilePathName1, NULL, NULL);
	if(pszRt)
	{
		m_strMerge5 = pszRt;
		UpdateData(FALSE);
	}
}

void CMergeDlg::OnButtonSetMerge6() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszRt;
	AgcmFileListDlg dlg;

	pszRt = dlg.OpenFileList(m_szFindFilePathName1, NULL, NULL);
	if(pszRt)
	{
		m_strMerge6 = pszRt;
		UpdateData(FALSE);
	}
}

void CMergeDlg::OnButtonSetMerge7() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszRt;
	AgcmFileListDlg dlg;

	pszRt = dlg.OpenFileList(m_szFindFilePathName1, NULL, NULL);
	if(pszRt)
	{
		m_strMerge7 = pszRt;
		UpdateData(FALSE);
	}
}

void CMergeDlg::OnButtonSetMerge8() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszRt;
	AgcmFileListDlg dlg;

	pszRt = dlg.OpenFileList(m_szFindFilePathName1, NULL, NULL);
	if(pszRt)
	{
		m_strMerge8 = pszRt;
		UpdateData(FALSE);
	}
}

void CMergeDlg::OnButtonSetMerge9() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszRt;
	AgcmFileListDlg dlg;

	pszRt = dlg.OpenFileList(m_szFindFilePathName1, NULL, NULL);
	if(pszRt)
	{
		m_strMerge9 = pszRt;
		UpdateData(FALSE);
	}
}

void CMergeDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	if(m_strMerge1 != "")
	{
		strcpy(m_pcsSet->m_szData[0], (LPSTR)(LPCSTR)(m_strMerge1));
	}

	if(m_strMerge2 != "")
	{
		strcpy(m_pcsSet->m_szData[1], (LPSTR)(LPCSTR)(m_strMerge2));
	}

	if(m_strMerge3 != "")
	{
		strcpy(m_pcsSet->m_szData[2], (LPSTR)(LPCSTR)(m_strMerge3));
	}

	if(m_strMerge4 != "")
	{
		strcpy(m_pcsSet->m_szData[3], (LPSTR)(LPCSTR)(m_strMerge4));
	}

	if(m_strMerge5 != "")
	{
		strcpy(m_pcsSet->m_szData[4], (LPSTR)(LPCSTR)(m_strMerge5));
	}

	if(m_strMerge6 != "")
	{
		strcpy(m_pcsSet->m_szData[5], (LPSTR)(LPCSTR)(m_strMerge6));
	}

	if(m_strMerge7 != "")
	{
		strcpy(m_pcsSet->m_szData[6], (LPSTR)(LPCSTR)(m_strMerge7));
	}

	if(m_strMerge8 != "")
	{
		strcpy(m_pcsSet->m_szData[7], (LPSTR)(LPCSTR)(m_strMerge8));
	}

	if(m_strMerge9 != "")
	{
		strcpy(m_pcsSet->m_szData[8], (LPSTR)(LPCSTR)(m_strMerge9));
	}

	CDialog::OnOK();
}
