// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "regiontool.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

extern BOOL	g_bTeleportOnlyMode;

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginDlg)
	m_strID = _T("");
	m_strPass = _T("");
	//}}AFX_DATA_INIT
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Text(pDX, IDC_ID, m_strID);
	DDX_Text(pDX, IDC_PASS, m_strPass);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers

void CLoginDlg::OnOK() 
{
	// Login Sequence
	UpdateData( TRUE );

	/*
	if( Login(	( LPCTSTR ) m_strID				,
				( LPCTSTR ) m_strPass			,
				( LPCTSTR ) m_strWorkingFolder	,
				( LPCTSTR ) m_strRepository		) )
	{
		g_bTeleportOnlyMode = FALSE;
		CDialog::OnOK();
	}
	else
	{
		MessageBox( "로그인 실패 했어요" , "리젼툴" , MB_ICONERROR | MB_OK );
	}
	*/
}

void CLoginDlg::OnBnClickedButton1()
{
	g_bTeleportOnlyMode = TRUE;
	CDialog::OnOK();
}