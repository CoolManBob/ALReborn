// InputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "InputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputDlg dialog


CInputDlg::CInputDlg(INT32 *plDestValue, CWnd* pParent /*=NULL*/)
	: CDialog(CInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputDlg)
	m_nInput = (plDestValue) ? (*plDestValue) : (0);
	//}}AFX_DATA_INIT

	m_plDestValue = plDestValue;
}


void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputDlg)
	DDX_Text(pDX, IDC_EDIT_INPUT, m_nInput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
	//{{AFX_MSG_MAP(CInputDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputDlg message handlers

void CInputDlg::OnOK() 
{
	// TODO: Add extra validation here
	if(m_plDestValue)
	{
		UpdateData(TRUE);

		*(m_plDestValue) = m_nInput;
	}

	CDialog::OnOK();
}
