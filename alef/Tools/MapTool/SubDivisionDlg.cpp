// SubDivisionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "SubDivisionDlg.h"
#include "rwcore.h"
#include "rpworld.h"
//#include "AcuRpDWSector.h"
#include "MainWindow.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubDivisionDlg dialog

extern MainWindow			g_MainWindow	;
extern CMainFrame *			g_pMainFrame	;

CSubDivisionDlg::CSubDivisionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSubDivisionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSubDivisionDlg)
	m_strDetail		= _T("")	;
	m_bTiled		= TRUE		;
	m_bAlphaTiled	= FALSE		;
	//}}AFX_DATA_INIT

	m_nDetail	= 0;
}


void CSubDivisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubDivisionDlg)
	DDX_Control(pDX, IDC_DETAILSLIDER, m_ctlSlider);
	DDX_Text(pDX, IDC_DETAIL, m_strDetail);
	DDX_Check(pDX, IDC_TILED, m_bTiled);
	DDX_Check(pDX, IDC_ALPHATILED, m_bAlphaTiled);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSubDivisionDlg, CDialog)
	//{{AFX_MSG_MAP(CSubDivisionDlg)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_DETAILSLIDER, OnReleasedcaptureDetailslider)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubDivisionDlg message handlers

BOOL CSubDivisionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctlSlider.SetRange	( 0 , SUBDIVISIONDETAIL_COUNT - 1	);
	m_ctlSlider.SetPos		( SUBDIVISIONDETAIL_x2				);
	
	int pos = m_ctlSlider.GetPos		();
	
	switch( pos )
	{
	case	SUBDIVISIONDETAIL_ONE	:	m_strDetail.Format( "통짜!"	); break;
	case	SUBDIVISIONDETAIL_x05	:	m_strDetail.Format( "1/2"	); break;
	case	SUBDIVISIONDETAIL_x1	:	m_strDetail.Format( "*1"	); break;
	case	SUBDIVISIONDETAIL_x2	:	m_strDetail.Format( "x2"	); break;
	case	SUBDIVISIONDETAIL_x4	:	m_strDetail.Format( "x4"	); break;
	case	SUBDIVISIONDETAIL_x8	:	m_strDetail.Format( "x8"	); break;
	default:
		break;
	}

	m_nDetail	= pos;
	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSubDivisionDlg::OnReleasedcaptureDetailslider(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int pos = m_ctlSlider.GetPos		();
	
	switch( pos )
	{
	case	SUBDIVISIONDETAIL_ONE	:	m_strDetail.Format( "통짜!"	); break;
	case	SUBDIVISIONDETAIL_x05	:	m_strDetail.Format( "1/2"	); break;
	case	SUBDIVISIONDETAIL_x1	:	m_strDetail.Format( "*1"	); break;
	case	SUBDIVISIONDETAIL_x2	:	m_strDetail.Format( "x2"	); break;
	case	SUBDIVISIONDETAIL_x4	:	m_strDetail.Format( "x4"	); break;
	case	SUBDIVISIONDETAIL_x8	:	m_strDetail.Format( "x8"	); break;
	default:
		break;
	}

	m_nDetail	= pos;
	UpdateData( FALSE );
	
	*pResult = 0;
}
