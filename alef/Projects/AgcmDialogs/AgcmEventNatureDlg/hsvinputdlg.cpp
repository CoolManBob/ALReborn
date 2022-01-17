// HSVInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmeventnaturedlg.h"
#include "HSVInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHSVInputDlg dialog


CHSVInputDlg::CHSVInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHSVInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHSVInputDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nR = 0;
	m_nG = 0;
	m_nB = 0;
}


void CHSVInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHSVInputDlg)
	DDX_Control(pDX, IDC_HSVCONTROL, m_ctlHSVControl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHSVInputDlg, CDialog)
	//{{AFX_MSG_MAP(CHSVInputDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHSVInputDlg message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CHSVInputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctlHSVControl.Init( CHSVControlStatic::CIRCLETYPE , RGB( m_nR , m_nG , m_nB ) );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

UINT	CHSVInputDlg::DoModal( int nR , int nG , int nB )
{
	// 데이타 초기화 하고 Dialog 를 띄운다.
	m_nR	=	nR;
	m_nG	=	nG;
	m_nB	=	nB;

	return CDialog::DoModal();
}

void CHSVInputDlg::OnOK() 
{
	// 데이타를 업데이트함.

	COLORREF color = m_ctlHSVControl.GetRGB();

	m_nR	= GetRValue( color );
	m_nG	= GetGValue( color );
	m_nB	= GetBValue( color );
	
	CDialog::OnOK();
}

// CColorStatic

CColorStatic::CColorStatic()
{
}

CColorStatic::~CColorStatic()
{
}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatic message handlers

void CColorStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Dialog 띄움..
	CHSVInputDlg	dlg;
	if( IDOK == dlg.DoModal( m_nR , m_nG , m_nB ) )
	{
		m_nR	= dlg.m_nR;
		m_nG	= dlg.m_nG;
		m_nB	= dlg.m_nB;

		// 칼라 값을 얻어서 현 Dialog에 세팅함..
		Invalidate( TRUE );
		
		// Parent Notify
		GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );
	}
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CColorStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	CString	strName;

	GetWindowText( strName );

	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 255 , 255 , 255 ) );

	rect.top += 10;
	dc.FillSolidRect( rect , GetRGB() );

	dc.TextOut( 0 , 0 , strName );
	// Do not call CStatic::OnPaint() for painting messages
}
