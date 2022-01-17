// PrelightAdjustDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "maptool.h"
#include "PrelightAdjustDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrelightAdjustDlg dialog


CPrelightAdjustDlg::CPrelightAdjustDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrelightAdjustDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrelightAdjustDlg)
	m_nGeometry		= 100;
	m_nObject		= 100;
	m_nCharacter	= 100;
	m_strGeometry	= _T("");
	m_strObject		= _T("");
	m_strCharacter	= _T("");
	//}}AFX_DATA_INIT
}


void CPrelightAdjustDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrelightAdjustDlg)
	DDX_Control(pDX, IDC_DISPLAY, m_ctlPortion);
	DDX_Scroll(pDX, IDC_GEOMETRY, m_nGeometry);
	DDX_Scroll(pDX, IDC_OBJECT, m_nObject);
	DDX_Text(pDX, IDC_EDIT_GEOMETRY, m_strGeometry);
	DDX_Text(pDX, IDC_EDIT_OBJECT, m_strObject);
	DDX_Text(pDX, IDC_EDIT_CHARACTER, m_strCharacter);
	DDX_Scroll(pDX, IDC_CHARACTER, m_nCharacter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrelightAdjustDlg, CDialog)
	//{{AFX_MSG_MAP(CPrelightAdjustDlg)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrelightAdjustDlg message handlers

BOOL CPrelightAdjustDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// 에헤헤;;;
	CRect	rect;
	GetWindowRect( rect );
	MoveWindow( m_Rect.left , m_Rect.top , rect.Width() , rect.Height() );
	
	// 스크롤 범위 정의..

	CScrollBar	* pScrollBar;
	pScrollBar = ( CScrollBar * ) GetDlgItem( IDC_CHARACTER	);
	pScrollBar->SetScrollRange( 0 , 200 );

	pScrollBar = ( CScrollBar * ) GetDlgItem( IDC_GEOMETRY	);
	pScrollBar->SetScrollRange( 0 , 200 );

	pScrollBar = ( CScrollBar * ) GetDlgItem( IDC_OBJECT	);
	pScrollBar->SetScrollRange( 0 , 200 );

	m_nGeometry		= 100;
	m_nObject		= 100;
	m_nCharacter	= 100;
	m_strGeometry	.Format( "%d%%" , m_nGeometry	- 100 );
	m_strObject		.Format( "%d%%" , m_nObject		- 100 );
	m_strCharacter	.Format( "%d%%" , m_nCharacter	- 100 );

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrelightAdjustDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CPrelightAdjustDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CPrelightAdjustDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	int	nPrevPos = pScrollBar->GetScrollPos();

	switch( nSBCode )
	{
	case SB_LINEUP       :	nPrevPos += -1	;	break;
	case SB_LINEDOWN     :	nPrevPos += 1	;	break;
	case SB_PAGEUP       :	nPrevPos += -20	;	break;
	case SB_PAGEDOWN     :	nPrevPos += 20	;	break;
	case SB_THUMBPOSITION:	nPrevPos = nPos	;	break;
	case SB_THUMBTRACK   :	nPrevPos = nPos	;	break;
	case SB_TOP          :	nPrevPos += 0	;	break;
	case SB_BOTTOM       :	nPrevPos += 200	;	break;
	default:
		return;
	}
	switch( pScrollBar->GetDlgCtrlID() )
	{
	case IDC_CHARACTER		:
		{
			UpdateData( TRUE );

			m_nCharacter= nPrevPos;

			m_strGeometry	.Format( "%d%%" , m_nGeometry	- 100 );
			m_strObject		.Format( "%d%%" , m_nObject		- 100 );
			m_strCharacter	.Format( "%d%%" , m_nCharacter	- 100 );

			UpdateData( FALSE );
		}
		return;
	case IDC_GEOMETRY	:
		{
			UpdateData( TRUE );

			m_nGeometry	= nPrevPos	;

			m_strGeometry	.Format( "%d%%" , m_nGeometry	- 100 );
			m_strObject		.Format( "%d%%" , m_nObject		- 100 );
			m_strCharacter	.Format( "%d%%" , m_nCharacter	- 100 );

			UpdateData( FALSE );
		}
		return;
	case IDC_OBJECT		:
		{
			UpdateData( TRUE );

			m_nObject	= nPrevPos	;

			m_strGeometry	.Format( "%d%%" , m_nGeometry	- 100 );
			m_strObject		.Format( "%d%%" , m_nObject		- 100 );
			m_strCharacter	.Format( "%d%%" , m_nCharacter	- 100 );

			UpdateData( FALSE );
		}
		return;

	default:
		// do nothing..
		break;
	}
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
