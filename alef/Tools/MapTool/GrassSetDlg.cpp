// GrassSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "GrassSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrassSetDlg dialog


CGrassSetDlg::CGrassSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGrassSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGrassSetDlg)
	m_bGrassDraw = TRUE;
	m_iDensity = 0;
	m_fScaleMin = 1.0f;
	m_fScaleMax = 1.0f;
	m_iRotMin = 0;
	m_iRotMax = 0;
	m_iRotYMin = 0;
	m_iRotYMax = 0;
	//}}AFX_DATA_INIT
}


void CGrassSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrassSetDlg)
	DDX_Check(pDX, IDC_CHECK_GRASSONOFF, m_bGrassDraw);
	DDX_Text(pDX, IDC_GRASS_EDIT1, m_iDensity);
	DDX_Text(pDX, IDC_GRASS_EDIT4, m_fScaleMin);
	DDX_Text(pDX, IDC_GRASS_EDIT5, m_fScaleMax);
	DDX_Text(pDX, IDC_GRASS_EDIT2, m_iRotMin);
	DDX_Text(pDX, IDC_GRASS_EDIT3, m_iRotMax);
	DDX_Text(pDX, IDC_GRASS_EDIT6, m_iRotYMin);
	DDX_Text(pDX, IDC_GRASS_EDIT7, m_iRotYMax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrassSetDlg, CDialog)
	//{{AFX_MSG_MAP(CGrassSetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrassSetDlg message handlers

