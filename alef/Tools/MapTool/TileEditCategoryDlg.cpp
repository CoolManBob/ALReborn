// TileEditCategoryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "TileEditCategoryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTileEditCategoryDlg dialog


CTileEditCategoryDlg::CTileEditCategoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTileEditCategoryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTileEditCategoryDlg)
	m_strName = _T("");
	m_nType = 0;
	//}}AFX_DATA_INIT
}


void CTileEditCategoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTileEditCategoryDlg)
	DDX_Text(pDX, IDC_CATEGORYNAME, m_strName);
	DDX_Text(pDX, IDC_TYPE, m_nType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTileEditCategoryDlg, CDialog)
	//{{AFX_MSG_MAP(CTileEditCategoryDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTileEditCategoryDlg message handlers
