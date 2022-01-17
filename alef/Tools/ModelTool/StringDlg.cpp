#include "stdafx.h"
#include "ModelTool.h"
#include "StringDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CStringDlg::CStringDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStringDlg::IDD, pParent)
{
	m_strCur = _T("");
}


void CStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_CUR_STR, m_strCur);
}

BEGIN_MESSAGE_MAP(CStringDlg, CDialog)
	//{{AFX_MSG_MAP(CStringDlg)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStringDlg message handlers
VOID CStringDlg::SetString(CHAR *szStr)
{	
	m_strCur = szStr;
	UpdateData(FALSE);

	RedrawWindow();
}

BOOL CStringDlg::Create(CWnd* pParentWnd) 
{
	BOOL bResult = CDialog::Create(IDD, pParentWnd);
	RedrawWindow();
	
	return bResult;
}

void CStringDlg::OnCancel()
{
	CDialog::OnCancel();
}