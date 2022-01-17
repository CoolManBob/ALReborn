// NPCNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "NPCNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNPCNameDlg dialog


CNPCNameDlg::CNPCNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNPCNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNPCNameDlg)
	m_strName = _T("");
	m_strNameOrigin = _T("");
	//}}AFX_DATA_INIT
}

void CNPCNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNPCNameDlg)
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, AGPACHARACTER_MAX_ID_STRING);
	DDX_Text(pDX, IDC_NAMEORIGIN, m_strNameOrigin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNPCNameDlg, CDialog)
	//{{AFX_MSG_MAP(CNPCNameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNPCNameDlg message handlers

void CNPCNameDlg::OnOK() 
{
	UpdateData( TRUE );

	if( m_strName.GetLength() == 0 )
	{
		MessageBox( "이름좀 넣고 ok 누루세요!" );
		return;
	}
	
	CDialog::OnOK();
}
