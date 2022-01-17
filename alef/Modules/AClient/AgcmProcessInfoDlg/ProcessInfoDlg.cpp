// ProcessInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AgcmProcessInfoDlg.h"
#include "ProcessInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcessInfoDlg dialog


CProcessInfoDlg::CProcessInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcessInfoDlg)
	m_strInfo = _T("");
	//}}AFX_DATA_INIT
}


void CProcessInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcessInfoDlg)
	DDX_Text(pDX, IDC_EDIT_PROCESS_INFO, m_strInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcessInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CProcessInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessInfoDlg message handlers

BOOL CProcessInfoDlg::Create(CHAR *szInitInfo, CWnd* pParentWnd/*= NULL */) 
{
	// TODO: Add your specialized code here and/or call the base class	
	if(!CDialog::Create(IDD, pParentWnd))
		return FALSE;

	m_strInfo = szInitInfo;
	UpdateData(FALSE);
	UpdateWindow();

	return TRUE;
}

VOID CProcessInfoDlg::SetProcessInfo(CHAR *szInfo)
{
	CHAR szTemp[256];
	sprintf(szTemp, "\r\n%s", szInfo);

	m_strInfo += szTemp;
	UpdateData(FALSE);

	CEdit *pcsEdit = (CEdit *)(GetDlgItem(IDC_EDIT_PROCESS_INFO));
	if(pcsEdit)
	{
		INT32 lMin, lMax;
		pcsEdit->GetScrollRange(SB_VERT, &lMin, &lMax);

		pcsEdit->LineScroll(lMax);
	}
}

void CProcessInfoDlg::OnOK() 
{
	// TODO: Add extra validation here
	if(AgcmProcessInfoDlg::GetInstance())
	{
		AgcmProcessInfoDlg::GetInstance()->CloseProcessInfoDlg();
	}

//	CDialog::OnOK();
}
