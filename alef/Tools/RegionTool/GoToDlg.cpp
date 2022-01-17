// GoToDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RegionTool.h"
#include "GoToDlg.h"
#include ".\gotodlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGoToDlg dialog


CGoToDlg::CGoToDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGoToDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGoToDlg)
	m_uDivision = 0;
	//}}AFX_DATA_INIT
}


void CGoToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGoToDlg)
	DDX_Text(pDX, IDC_DIVISION, m_uDivision);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGoToDlg, CDialog)
	//{{AFX_MSG_MAP(CGoToDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_GOTO_BATTLEFIELD, OnBnClickedGotoBattlefield)
	ON_BN_CLICKED(IDC_GOTO_DELPARAS, OnBnClickedGotoDelparas)
	ON_BN_CLICKED(IDC_GOTO_TURLAN, OnBnClickedGotoTurlan)
	ON_BN_CLICKED(IDC_GOTO_TRILGARD, OnBnClickedGotoTrilgard)
	ON_BN_CLICKED(IDC_GOTO_KUN, OnBnClickedGotoKun)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoToDlg message handlers

void CGoToDlg::OnBnClickedGotoBattlefield()
{
	m_uDivision = 1719;
	UpdateData( FALSE );
	OnOK();
}

void CGoToDlg::OnBnClickedGotoDelparas()
{
	m_uDivision = 1925;
	UpdateData( FALSE );
	OnOK();
}

void CGoToDlg::OnBnClickedGotoTurlan()
{
	m_uDivision = 2125;
	UpdateData( FALSE );
	OnOK();
}

void CGoToDlg::OnBnClickedGotoTrilgard()
{
	m_uDivision = 2727;
	UpdateData( FALSE );
	OnOK();
}

void CGoToDlg::OnBnClickedGotoKun()
{
	m_uDivision = 2728;
	UpdateData( FALSE );
	OnOK();
}
