// TestEnvDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Casper.h"
#include "TestEnvDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestEnvDialog dialog


CTestEnvDialog::CTestEnvDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTestEnvDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestEnvDialog)
	m_strHigherID = _T("");
	m_lID_Begin = 0;
	m_lID_End = 0;
	m_strServerIP = _T("121.189.45.199");
	m_ulServerPort = 11008;
	m_lClientNumber = 50;
	//}}AFX_DATA_INIT
}


void CTestEnvDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestEnvDialog)
	DDX_Text(pDX, IDC_HIGHER_ID, m_strHigherID);
	DDX_Text(pDX, IDC_LOWER_BEGIN, m_lID_Begin);
	DDX_Text(pDX, IDC_LOWER_END, m_lID_End);
	DDX_Text(pDX, IDC_CASPER_SERVER_IP, m_strServerIP);
	DDX_Text(pDX, IDC_CASPER_SERVER_PORT, m_ulServerPort);
	DDX_Text(pDX, IDC_CASPER_CLIENT_NUMBER, m_lClientNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestEnvDialog, CDialog)
	//{{AFX_MSG_MAP(CTestEnvDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestEnvDialog message handlers

BOOL CTestEnvDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTestEnvDialog::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	CDialog::OnOK();
}
