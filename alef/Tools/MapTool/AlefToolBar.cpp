// AlefToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MapTool.h"
#include "AlefToolBar.h"
#include "ApmMap.h"
#include "mainfrm.h"
#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *		g_pMainFrame;

/////////////////////////////////////////////////////////////////////////////
// CAlefToolBar

CAlefToolBar::CAlefToolBar()
{
	m_bLButtonDown		= FALSE			;
	m_nCurrentDetail	= SECTOR_EMPTY	;

	m_nHour				= 14			;
	m_nMinute			= 0				;
	m_bStopTimer		= true			;
	m_bSpeedTimer		= false			;
	m_nHourCount		= 0				;
}

CAlefToolBar::~CAlefToolBar()
{
}


BEGIN_MESSAGE_MAP(CAlefToolBar, CWnd)
	//{{AFX_MSG_MAP(CAlefToolBar)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAlefToolBar message handlers

void CAlefToolBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );

	dc.FillSolidRect( rect , RGB( 51 , 102 , 153 ) );
	
//	dc.SetTextColor( RGB( 0 , 0 , 0 ) );
//	dc.TextOut( 0 , 0 , "여긴 툴윈도우" );

	if( NULL == g_pcsAgpmTimer			|| 
		NULL == g_pcsAgpmEventNature	) return;
	
	m_nHour		= g_pcsAgpmTimer->GetHour		( g_pcsAgpmTimer->GetGameTime() );
	m_nMinute	= g_pcsAgpmTimer->GetMinute		( g_pcsAgpmTimer->GetGameTime() );

	POINT	pt;
	pt.y	= 0;
	for( int i = 0 ; i < m_imageList_Toolbar_up.GetImageCount() ; i ++ )
	{
		pt.x	= i * TOOLBAR_ICON_WIDTH;
		if( m_bLButtonDown && i == m_nSelectedIndex )
		{
			m_imageList_Toolbar_down.	Draw( &dc , i , pt , ILD_NORMAL );
		}
		else
			m_imageList_Toolbar_up.		Draw( &dc , i , pt , ILD_NORMAL );
	}

	// 시계 그리기...

	CRect	rectClock;
	rectClock.SetRect( rect.Width() - CLOCK_WIDTH , 0 , rect.Width() , rect.Height() );

	CString	str;
	
	static bool	bToggle = true;

	if( bToggle && ! g_pcsAgpmEventNature->GetTimer() )
	{
		str.Format( "%02d:%02d x%d" , m_nHour , m_nMinute , ( g_pcsAgpmEventNature->GetSpeedRate() + 1 ) / 64 );
	}
	else
	{
		str.Format( "%02d %02d x%d" , m_nHour , m_nMinute , ( g_pcsAgpmEventNature->GetSpeedRate() + 1 ) / 64 );
	}

	if( !g_pcsAgpmEventNature->GetTimer() )
		dc.FillSolidRect( rectClock , RGB( 64 , 64 , 64 ) );
	else
		dc.FillSolidRect( rectClock , RGB( 64 , 0 , 0 ) );
	if( m_bSpeedTimer )
		dc.FillSolidRect( rectClock , RGB( 0 , 0 , 64 ) );

	//dc.FillSolidRect( rectClock , RGB( 64 , 64 , 64 ) );
	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB( 255 , 255 , 255 ) );

	dc.DrawText( str , rectClock , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	
	bToggle = ! bToggle;


	// 에디트 모드 출력..
	if( GetFPSEditMode() )
	{
		CRect	rectEditMode;
		rectEditMode.SetRect(	rect.Width() - CLOCK_WIDTH - FPS_EDITMODE_WIDTH	, 0				, 
								rect.Width() - CLOCK_WIDTH						, rect.Height()	);

		str = "FPS Edit Mode!!";

		dc.FillSolidRect( rectEditMode , RGB( 210 , 56 , 74 ) );

		dc.SetBkMode( TRANSPARENT );
		dc.SetTextColor( RGB( 223 , 218 , 128 ) );

		dc.DrawText( str , rectEditMode , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}
	

	// 콤보 그리기.
//	CRect	rectCombo;
//	rectCombo.SetRect( rect.Width() - DETAIL_COMBOBOX_WIDTH , 0 , rect.Width() , rect.Height() );
//	dc.FillSolidRect( rectCombo , SECTORDETAIL_BACK_COLOR );
//	dc.SetBkMode( TRANSPARENT );
//
//	switch( m_nCurrentDetail )
//	{
//	case	SECTOR_EMPTY		:
//		{
//			dc.SetTextColor( SECTORDETAIL_TEXT_COLOR_EMPTY );
//			dc.DrawText( SECTORDETAIL_STRING_EMPTY , rectCombo , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
//		}
//		break;
//	case	SECTOR_LOWDETAIL	:
//		{
//			dc.SetTextColor( SECTORDETAIL_TEXT_COLOR_LOW );
//			dc.DrawText( SECTORDETAIL_STRING_LOWDETAIL , rectCombo , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
//		}
//		break;
//	case	SECTOR_HIGHDETAIL	:
//		{
//			dc.SetTextColor( SECTORDETAIL_TEXT_COLOR_HIGH );
//			dc.DrawText( SECTORDETAIL_STRING_HIGHDETAIL , rectCombo , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
//		}
//		break;
//		
//	}

	
}

int CAlefToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_imageList_Toolbar_up		.Create( IDB_TOOLBAR_UP		, TOOLBAR_ICON_WIDTH , 0 , RGB( 255 , 0 , 0 ) );
	m_imageList_Toolbar_down	.Create( IDB_TOOLBAR_DOWN	, TOOLBAR_ICON_WIDTH , 0 , RGB( 255 , 0 , 0 ) );
	
	ToggleTimer();
	
	return 0;
}

void CAlefToolBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_nSelectedIndex = point.x / TOOLBAR_ICON_WIDTH;

	if( m_nSelectedIndex < TOOLBAR_INDEX_MAX )
	{
//		GetParent()->PostMessage( WM_TOOLBAR_SELECT , 0 , m_nSelectedIndex );

		CRect	rect;
		rect.left	= TOOLBAR_ICON_WIDTH * m_nSelectedIndex			;
		rect.top	= 0												;
		rect.right	= TOOLBAR_ICON_WIDTH * ( m_nSelectedIndex + 1 )	;
		rect.bottom	= 20											;
		InvalidateRect( rect , FALSE );

		m_bLButtonDown = TRUE;
		SetCapture();
	}

	CRect	rect;
	GetClientRect( rect );
	CRect	rectClock;
	rectClock.SetRect( rect.Width() - CLOCK_WIDTH , 0 , rect.Width() , rect.Height() );
	InvalidateRect( rectClock , FALSE );

	if( rectClock.PtInRect( point ) )
	{
		// 까꿍..
		UINT8	rate = g_pcsAgpmEventNature->GetSpeedRate();
		switch( rate )
		{
		case 0		:	rate	=	64	;	break;
		case 64		:	rate	=	128	;	break;
		case 128	:	rate	=	255	;	break;
		case 255	:	rate	=	0	;	break;
		default:
			rate = 64;
			break;
		}

		g_pcsAgpmEventNature->SetSpeedRate( rate );
	}


	CWnd::OnLButtonDown(nFlags, point);
}

void CAlefToolBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int current = point.x / TOOLBAR_ICON_WIDTH;

	if( current == m_nSelectedIndex && m_bLButtonDown && m_nSelectedIndex < TOOLBAR_INDEX_MAX )
	{
		GetParent()->PostMessage( WM_TOOLBAR_SELECT , 0 , m_nSelectedIndex );
		ReleaseCapture();
	}	

	m_bLButtonDown = FALSE;
	CRect	rect;
	rect.left	= TOOLBAR_ICON_WIDTH * m_nSelectedIndex			;
	rect.top	= 0												;
	rect.right	= TOOLBAR_ICON_WIDTH * ( m_nSelectedIndex + 1 )	;
	rect.bottom	= 20											;
	InvalidateRect( rect , FALSE );
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CAlefToolBar::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
}

void CAlefToolBar::OnChangeDetailCombo()
{
	// 페어런트 노티파이..
	GetParent()->PostMessage( WM_TOOLBAR_DETAILCHANGE , DETAIL_NOTIFY , m_nCurrentDetail );
}

void CAlefToolBar::SetDetail(int detail)
{
	m_nCurrentDetail = detail;
	Invalidate();
}

void CAlefToolBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnLButtonDown( nFlags , point );

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CAlefToolBar::ToggleTimer		()
{
	static	int interval = 500;
	//static	int interval = 2000;

	if( m_bStopTimer )
	{
		SetTimer( 1 , interval , NULL );
	}
	else
	{
		KillTimer( 1 );
	}

	m_bStopTimer = !m_bStopTimer;

	CRect	rect;
	GetClientRect( rect );
	CRect	rectClock;
	rectClock.SetRect( rect.Width() - CLOCK_WIDTH , 0 , rect.Width() , rect.Height() );
	InvalidateRect( rectClock , FALSE );
}

void CAlefToolBar::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	switch( nIDEvent )
	{
	case 1:	// 노멀 타이머.
		{
			CRect	rect;
			GetClientRect( rect );
			CRect	rectClock;
			rectClock.SetRect( rect.Width() - CLOCK_WIDTH , 0 , rect.Width() , rect.Height() );
			InvalidateRect( rectClock , FALSE );
		}
		break;
	}

	// Notify Parent
	// GetParent()->PostMessage( WM_TOOLBAR_TIME_UPDATE , m_nHour , m_nMinute );
	
	CWnd::OnTimer(nIDEvent);
}

void CAlefToolBar::OnDestroy() 
{
	if( !m_bStopTimer )
	{
		KillTimer( 1 );
	}

	CWnd::OnDestroy();
}

void CAlefToolBar::AllRound()
{
	return;
}

void CAlefToolBar::ToggleFPSEditMode	()
{
	SetFPSEditMode( !GetFPSEditMode() );
}

void CAlefToolBar::SetFPSEditMode		( BOOL bEdit )
{
	g_pMainFrame->m_Document.m_bFPSEditMode = bEdit;
	Invalidate( FALSE );
}

BOOL CAlefToolBar::GetFPSEditMode		()
{
	return g_pMainFrame->m_Document.m_bFPSEditMode;
}
