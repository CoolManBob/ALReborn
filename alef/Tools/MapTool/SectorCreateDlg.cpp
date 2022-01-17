// SectorCreateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "SectorCreateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSectorCreateDlg dialog


CSectorCreateDlg::CSectorCreateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSectorCreateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSectorCreateDlg)
	m_strDetail			= _T("")	;
	m_nDepth			= 0			;
	m_fDefaultHeight	= 1.0f		;
	//}}AFX_DATA_INIT
}


void CSectorCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSectorCreateDlg)
	DDX_Text(pDX, IDC_CREATEDETAIL, m_strDetail);
	DDX_Text(pDX, IDC_CREATE_DEPTH, m_nDepth);
	DDV_MinMaxInt(pDX, m_nDepth, 1, 16);
	DDX_Text(pDX, IDC_CREATE_HEIGHT, m_fDefaultHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSectorCreateDlg, CDialog)
	//{{AFX_MSG_MAP(CSectorCreateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSectorCreateDlg message handlers

void CSectorCreateDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
