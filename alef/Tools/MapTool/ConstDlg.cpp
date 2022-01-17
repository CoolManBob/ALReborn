// ConstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "ConstDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConstDlg dialog



CConstDlg::CConstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConstDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConstDlg)
	m_nDepth		= 0		;
	m_nHoriCount	= 0		;
	m_fSectorWidth	= 0.0f	;
	m_nVertCount	= 0		;
	//}}AFX_DATA_INIT
}


void CConstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConstDlg)
	DDX_Text(pDX, IDC_DEPTH, m_nDepth);
	DDX_Text(pDX, IDC_HORICOUNT, m_nHoriCount);
	DDX_Text(pDX, IDC_SECTORWIDTH, m_fSectorWidth);
	DDX_Text(pDX, IDC_VERTCOUNT, m_nVertCount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConstDlg, CDialog)
	//{{AFX_MSG_MAP(CConstDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConstDlg message handlers

void CConstDlg::OnOK() 
{
	// 디테일이 짝수여야함..
	UpdateData( TRUE );
//	if( ( m_nHoriCount % 2 ) || ( m_nVertCount % 2)  )
//	{
//		MessageBox( "가로와 세로의 크기는 짝수여야 합니다.");
//		return;
//	}
	
	CDialog::OnOK();
}
