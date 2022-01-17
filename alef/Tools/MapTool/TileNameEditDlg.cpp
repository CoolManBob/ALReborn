// TileNameEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "TileNameEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTileNameEditDlg dialog


CTileNameEditDlg::CTileNameEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTileNameEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTileNameEditDlg)
	m_strName = _T("");
	m_nNumber = 0;
	//}}AFX_DATA_INIT
}


void CTileNameEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTileNameEditDlg)
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_TYPE, m_nNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTileNameEditDlg, CDialog)
	//{{AFX_MSG_MAP(CTileNameEditDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTileNameEditDlg message handlers
