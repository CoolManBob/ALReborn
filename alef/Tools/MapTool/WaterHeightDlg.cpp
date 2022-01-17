// WaterHeightDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "WaterHeightDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaterHeightDlg dialog


CWaterHeightDlg::CWaterHeightDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaterHeightDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaterHeightDlg)
	m_fHeight = 0.0f;
	//}}AFX_DATA_INIT
}


void CWaterHeightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaterHeightDlg)
	DDX_Text(pDX, IDC_HEIGHT, m_fHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaterHeightDlg, CDialog)
	//{{AFX_MSG_MAP(CWaterHeightDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaterHeightDlg message handlers
