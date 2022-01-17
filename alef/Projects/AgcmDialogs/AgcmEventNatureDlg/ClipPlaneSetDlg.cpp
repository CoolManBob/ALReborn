// ClipPlaneSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "ClipPlaneSetDlg.h"
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
	m_fFogDistance	= 0.0f;
	m_fFogFarClip	= 0.0f;
	//}}AFX_DATA_INIT
}


void CClipPlaneSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClipPlaneSetDlg)
	DDX_Control(pDX, IDC_FOG_FARCLIP_SLIDER, m_ctlFogFarClip);
	DDX_Control(pDX, IDC_FOG_DISTANCE_SLIDER, m_ctlFogDistance);
	DDX_Text(pDX, IDC_FOG_DISTANCE, m_fFogDistance);
	DDX_Text(pDX, IDC_FOG_FARCLIP, m_fFogFarClip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClipPlaneSetDlg, CDialog)
	//{{AFX_MSG_MAP(CClipPlaneSetDlg)
	ON_WM_HSCROLL()
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

			UpdateData( FALSE );

			if( m_fFogDistance > m_fFogFarClip ) m_fFogFarClip = m_fFogDistance;

			m_pEventNatureDlg->p__SetFogFunction(
				m_pEventNatureDlg->m_pcsAgcmEventNature						,
				m_pEventNatureDlg->m_pcsAgcmEventNature->IsFogOn		()	,
				m_fFogDistance												,
				m_fFogFarClip												,
				m_pEventNatureDlg->m_pcsAgcmEventNature->GetFogColor	()	);		
		}
		else if( pScrollBar->GetSafeHwnd() == m_ctlFogFarClip.GetSafeHwnd() )
		{
			// Far Clip
			nPos = m_ctlFogFarClip.GetPos();
			m_fFogFarClip = ( FLOAT ) ( nPos ) * 100.0f;

			if( m_fFogDistance > m_fFogFarClip ) m_fFogDistance = m_fFogFarClip;

			UpdateData( FALSE );
			m_pEventNatureDlg->p__SetFogFunction(
				m_pEventNatureDlg->m_pcsAgcmEventNature						,
				m_pEventNatureDlg->m_pcsAgcmEventNature->IsFogOn		()	,
				m_fFogDistance												,
				m_fFogFarClip												,
				m_pEventNatureDlg->m_pcsAgcmEventNature->GetFogColor	()	);		
		}
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CClipPlaneSetDlg::OnOK() 
{
	UpdateData( TRUE );

	CDialog::OnOK();
}

UINT	CClipPlaneSetDlg::DoModal( AgcmEventNatureDlg *pEventNatureDlg )
{
	ASSERT( NULL != pEventNatureDlg );
	m_pEventNatureDlg	= pEventNatureDlg;

	if( NULL == pEventNatureDlg )
		return IDCANCEL;
	else
		return CDialog::DoModal();
}
