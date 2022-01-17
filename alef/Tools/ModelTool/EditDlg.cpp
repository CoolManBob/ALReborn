// EditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "EditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditDlg dialog


CEditDlg::CEditDlg(CHAR *szEdit, CHAR *szCaption, CWnd* pParent /*=NULL*/)
	: CDialog(CEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditDlg)
	m_strEdit = szEdit;
	//}}AFX_DATA_INIT

	m_pszEdit = szEdit;
	strcpy(m_szCaption, (szCaption) ? (szCaption) : (""));
}


void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditDlg)
	DDX_Text(pDX, IDC_EDIT_, m_strEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditDlg, CDialog)
	//{{AFX_MSG_MAP(CEditDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditDlg message handlers

void CEditDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	strcpy(m_pszEdit, (LPSTR)(LPCSTR)(m_strEdit));

	CDialog::OnOK();
}

BOOL CEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(strcmp(m_szCaption, ""))
	{
		SetWindowText(m_szCaption);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
