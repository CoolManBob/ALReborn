// ClipPlaneSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "ClipPlaneSetDlg.h"
#include "MyEngine.h"
#include "MainWindow.h"
extern	MainWindow	g_MainWindow		;

#include <AgcEngine.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClipPlaneSetDlg dialog


CClipPlaneSetDlg::CClipPlaneSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClipPlaneSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClipPlaneSetDlg)
	m_bUseFog			= FALSE	;
	m_bUseSky			= FALSE	;
	m_nLoadRange_Data	= 0		;
	m_nLoadRange_Detail	= 0		;
	m_nLoadRange_Rough	= 0		;
	m_fFogDistance = 0.0f;
	m_fFogFarClip = 0.0f;
	//}}AFX_DATA_INIT
}


void CClipPlaneSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClipPlaneSetDlg)
	DDX_Control(pDX, IDC_FOG_DISTANCE_SLIDER, m_ctlFogDistance);
	DDX_Control(pDX, IDC_FOG_FARCLIP_SLIDER, m_ctlFogFarClip);
	DDX_Check(pDX, IDC_USEFOG, m_bUseFog);
	DDX_Check(pDX, IDC_USESKY, m_bUseSky);
	DDX_Text(pDX, IDC_LOADRANGE_DATA, m_nLoadRange_Data);
	DDX_Text(pDX, IDC_LOADRANGE_DETAIL, m_nLoadRange_Detail);
	DDX_Text(pDX, IDC_LOADRANGE_ROUGH, m_nLoadRange_Rough);
	DDX_Text(pDX, IDC_FOG_DISTANCE, m_fFogDistance);
	DDX_Text(pDX, IDC_FOG_FARCLIP, m_fFogFarClip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClipPlaneSetDlg, CDialog)
	//{{AFX_MSG_MAP(CClipPlaneSetDlg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_USEFOG, OnUsefog)
	ON_BN_CLICKED(IDC_USESKY, OnUsesky)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClipPlaneSetDlg message handlers

BOOL CClipPlaneSetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctlFogFarClip		.SetRange	( 0	, ( INT32 ) ( 2.0f * MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 100.0f )	);
	m_ctlFogDistance	.SetRange	( 0	, ( INT32 ) ( 2.0f * MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 100.0f )	);

	m_ctlFogFarClip		.SetPos		( ( int ) ( m_fFogFarClip	/ 100.0f )	);
	m_ctlFogDistance	.SetPos		( ( int ) ( m_fFogDistance	/ 100.0f )	);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClipPlaneSetDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	UpdateData( TRUE );

	if( nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION )
	{
		if( pScrollBar->GetSafeHwnd() == m_ctlFogDistance.GetSafeHwnd() )
		{
			// Distance
			nPos = m_ctlFogDistance.GetPos();
			m_fFogDistance = ( FLOAT ) ( nPos ) * 100.0f;
			if( m_fFogDistance > m_fFogFarClip ) m_fFogFarClip = m_fFogDistance;

			UpdateData( FALSE );

			m_ctlFogDistance.SetPos( ( INT32 ) ( m_fFogDistance	/ 100.0f ) );
			m_ctlFogFarClip	.SetPos( ( INT32 ) ( m_fFogFarClip	/ 100.0f ) );

			g_pcsAgcmEventNature->SetFog(
				g_pcsAgcmEventNature->IsFogOn		()	,
				m_fFogDistance									,
				m_fFogFarClip									,
				g_pcsAgcmEventNature->GetFogColor	()	);		
		}
		else if( pScrollBar->GetSafeHwnd() == m_ctlFogFarClip.GetSafeHwnd() )
		{
			// Far Clip
			nPos = m_ctlFogFarClip.GetPos();
			m_fFogFarClip = ( FLOAT ) ( nPos ) * 100.0f;
			if( m_fFogDistance > m_fFogFarClip ) m_fFogDistance = m_fFogFarClip;

			UpdateData( FALSE );

			m_ctlFogDistance.SetPos( ( INT32 ) ( m_fFogDistance	/ 100.0f ) );
			m_ctlFogFarClip	.SetPos( ( INT32 ) ( m_fFogFarClip	/ 100.0f ) );

			g_pcsAgcmEventNature->SetFog(
				g_pcsAgcmEventNature->IsFogOn		()	,
				m_fFogDistance									,
				m_fFogFarClip									,
				g_pcsAgcmEventNature->GetFogColor	()	);		
		}
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CClipPlaneSetDlg::OnUsefog() 
{
	UpdateData( TRUE );

	g_pcsAgcmEventNature->SetFog(
		m_bUseFog											,
		g_pcsAgcmEventNature->GetFogDistance	()	,
		g_pcsAgcmEventNature->GetFogFarClip	()	,
		g_pcsAgcmEventNature->GetFogColor		()	);
}

void CClipPlaneSetDlg::OnUsesky() 
{
	UpdateData( TRUE );

	g_pcsAgcmEventNature->ShowSky( m_bUseSky );
}

void CClipPlaneSetDlg::OnOK() 
{
	UpdateData( TRUE );

	// Load Range 체크..

	if( m_nLoadRange_Rough >= m_nLoadRange_Detail && m_nLoadRange_Detail > 0 )
	{
		CDialog::OnOK();
	}
	else
	{
		MessageBox( "로드 레인지는 , 러프가 디테일보다 항상 커야합니다." );
	}
}
