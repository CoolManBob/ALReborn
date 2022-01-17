// EventNatureChangeFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "EventNatureChangeFilterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventNatureChangeFilterDlg dialog


CEventNatureChangeFilterDlg::CEventNatureChangeFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventNatureChangeFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventNatureChangeFilterDlg)
	m_bAmbience		= FALSE;
	m_bByTime		= FALSE;
	m_bCircumstance	= FALSE;
	m_bCloud		= FALSE;
	m_bBgm			= FALSE;
	m_bFog			= FALSE;
	m_bLightColor	= FALSE;
	m_bLightPosition= FALSE;
	m_bSkyColor		= FALSE;
	//}}AFX_DATA_INIT
}


void CEventNatureChangeFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventNatureChangeFilterDlg)
	DDX_Check(pDX, IDC_AMBIENCE, m_bAmbience);
	DDX_Check(pDX, IDC_BYTIME, m_bByTime);
	DDX_Check(pDX, IDC_CIRCUMSTANCE, m_bCircumstance);
	DDX_Check(pDX, IDC_CLOUD, m_bCloud);
	DDX_Check(pDX, IDC_BGM, m_bBgm);
	DDX_Check(pDX, IDC_FOG, m_bFog);
	DDX_Check(pDX, IDC_LIGHTCOLOR, m_bLightColor);
	DDX_Check(pDX, IDC_LIGHTPOSITION, m_bLightPosition);
	DDX_Check(pDX, IDC_SKYCOLOR, m_bSkyColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventNatureChangeFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CEventNatureChangeFilterDlg)
	ON_BN_CLICKED(IDC_DONOTCHANGEALL, OnDonotchangeall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventNatureChangeFilterDlg message handlers

void CEventNatureChangeFilterDlg::OnDonotchangeall() 
{
	// TODO: Add your control notification handler code here

	UpdateData( TRUE );

	BOOL	bSet = !m_bLightColor;

	m_bLightColor	= bSet;
	m_bAmbience		= bSet;
	m_bCircumstance	= bSet;
	m_bCloud		= bSet;
	m_bBgm			= bSet;
	m_bFog			= bSet;
	m_bLightPosition= bSet;
	m_bSkyColor		= bSet;

	UpdateData( FALSE );
}
