// UITileList_MapTabWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "UITileList_MapTabWnd.h"
#include "MainFrm.h"
#include "UITileListWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame * g_pMainFrame	;

/////////////////////////////////////////////////////////////////////////////
// CUITileList_MapTabWnd

CUITileList_MapTabWnd::CUITileList_MapTabWnd()
{
	m_bLButtonDown	= FALSE;
}

CUITileList_MapTabWnd::~CUITileList_MapTabWnd()
{
}


BEGIN_MESSAGE_MAP(CUITileList_MapTabWnd, CWnd)
	//{{AFX_MSG_MAP(CUITileList_MapTabWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUITileList_MapTabWnd message handlers

void CUITileList_MapTabWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );

	dc.FillSolidRect( rect , UIGM_COLOR_BACKGROUND );

	// 메뉴 출력.
	for( int i = 0 ; i < BRUSHTYPE_COUNT ; i ++ )
	{
		if( i == g_pMainFrame->m_Document.m_nBrushType )
		{
			dc.SetTextColor( TAB_SELECTED_COLOR	);
			dc.FillSolidRect( m_pMenu[ i ].rect , TAB_SELECTED_BACKCOLOR );
		}
		else
		{
			dc.SetTextColor( TAB_DEFUALT_COLOR	);
			dc.FillSolidRect( m_pMenu[ i ].rect , TAB_DEFUALT_BACKCOLOR );
		}


		dc.DrawText( m_pMenu[ i ].name , m_pMenu[ i ].rect , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}

	// 원을 그린다.
	int		nRadiusOffset;
	CRect	rectCircle;
	{

		nRadiusOffset = ( int ) ( g_pMainFrame->m_Document.m_fBrushRadius * ( float ) ( rect.Width() / 2 ) / UIGM_MAX_CIRCLE_SIZE );

		rectCircle.SetRect(	rect.Width() / 2 - nRadiusOffset , UIGM_MENU_HEIGHT + rect.Width() / 2 - nRadiusOffset ,
						rect.Width() / 2 + nRadiusOffset , UIGM_MENU_HEIGHT + rect.Width() / 2 + nRadiusOffset );

		dc.Ellipse	( rectCircle );
	}

	// 텍스트 출력.
	{
		dc.SetTextColor	( UIGM_COLOR_TEXT	);
		dc.SetBkMode	( TRANSPARENT		);
		dc.TextOut		( 0 , UIGM_MENU_HEIGHT , "브러시 크기조절" );

		CString	str;
		str.Format	( "Radius = %.1f" , g_pMainFrame->m_Document.m_fBrushRadius );
		dc.TextOut	( 0 , UIGM_MENU_HEIGHT + rect.Width() - 20 , str );

		dc.DrawText	( "요따시만함" , rectCircle , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}

	// Do not call CWnd::OnPaint() for painting messages
}

void CUITileList_MapTabWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect	rect;
	GetClientRect( rect );

	if( point.y < UIGM_MENU_HEIGHT )
	{
		// 메뉴 클릭..
		for( int i = 0 ; i < BRUSHTYPE_COUNT ; i ++ )
		{
			if( m_pMenu[ i ].rect.PtInRect( point ) )
			{
				// 탭 선택;

				if( g_pMainFrame->m_Document.m_nBrushType != i )
				{
					// 탭 체인지 이벤트

					SetBrushType( i );
					Invalidate( FALSE );

					// Parent에 알려줘야함..
					g_pMainFrame->PostMessage( WM_MAPBRUSHCHANGE );
				}
				else
				{
					// 같은거 찍어봐야 바뀌는거 없다니까.
				}
				break;
			}
		}
	}
	else if( point.y >= UIGM_MENU_HEIGHT + rect.Width() )
	{
//		// rgb윈도우창..
//		CRect	rectWindow;
//		m_ctlRGBSelectStatic.GetWindowRect( rectWindow );
//
//		ScreenToClient( rectWindow );
//
//		if( rectWindow.PtInRect( point ) )
//		{
//			point.x -= rectWindow.left;
//			point.y -= rectWindow.top;
//			m_ctlRGBSelectStatic.OnLButtonDown( nFlags , point );
//		}
	}
	else
	{

		// 브러시 선택..
		float	radius		;
		float	radiuspoint	;
		CPoint	cp = point	;


//		cp.y	-= UIGM_MENU_HEIGHT	;
//		cp.x	-= rect.Width() / 2	;

		radiuspoint	=	( float ) sqrt( 
			pow( ( double ) ( rect.Width() / 2 - point.x ) , 2 ) +
			pow( ( double ) ( UIGM_MENU_HEIGHT + rect.Width() / 2 - point.y ) , 2 )
			);

		radius = UIGM_MAX_CIRCLE_SIZE * radiuspoint / ( float ) ( rect.Width() / 2 );
		if( radius > UIGM_MAX_CIRCLE_SIZE ) radius = UIGM_MAX_CIRCLE_SIZE;

		g_pMainFrame->m_Document.m_fBrushRadius = radius;
		g_pMainFrame->PostMessage( WM_MAPBRUSHCHANGE );

		Invalidate( FALSE );

		m_bLButtonDown = TRUE;
		SetCapture();
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CUITileList_MapTabWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	m_bLButtonDown = FALSE;
	ReleaseCapture();
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CUITileList_MapTabWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect	rect;
	GetClientRect( rect );

	if( m_bLButtonDown )
	{
		// 브러시 선택..
		float	radius		;
		float	radiuspoint	;
		CPoint	cp = point	;

		radiuspoint	=	( float ) sqrt( 
			pow( ( double ) ( rect.Width() / 2 - point.x ) , 2 ) +
			pow( ( double ) ( UIGM_MENU_HEIGHT + rect.Width() / 2 - point.y ) , 2 )
			);

		radius = UIGM_MAX_CIRCLE_SIZE * radiuspoint / ( float ) ( rect.Width() / 2 );
		if( radius > UIGM_MAX_CIRCLE_SIZE ) radius = UIGM_MAX_CIRCLE_SIZE;

		g_pMainFrame->m_Document.m_fBrushRadius = radius;
		g_pMainFrame->PostMessage( WM_MAPBRUSHCHANGE );

		Invalidate( FALSE );
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

int CUITileList_MapTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pMenu[ 0 ].name	= "지형"											;
	m_pMenu[ 0 ].rect	.SetRect(	0	,	0,	60 ,	UIGM_MENU_HEIGHT)	;
	m_pMenu[ 0 ].pWnd	= NULL												;
	
	m_pMenu[ 1 ].name	= "사포"											;
	m_pMenu[ 1 ].rect	.SetRect(	60	,	0,	120	,	UIGM_MENU_HEIGHT)	;
	m_pMenu[ 1 ].pWnd	= NULL												;

	m_pMenu[ 2 ].name	= "버칼"											;
	m_pMenu[ 2 ].rect	.SetRect(	120	,	0,	180	,	UIGM_MENU_HEIGHT)	;
	m_pMenu[ 2 ].pWnd	= NULL												;

	m_pMenu[ 3 ].name	= "노말"											;
	m_pMenu[ 3 ].rect	.SetRect(	180	,	0,	240	,	UIGM_MENU_HEIGHT)	;
	m_pMenu[ 3 ].pWnd	= NULL												;

	m_pMenu[ 4 ].name	= "엣지"											;
	m_pMenu[ 4 ].rect	.SetRect(	240	,	0,	300	,	UIGM_MENU_HEIGHT)	;
	m_pMenu[ 4 ].pWnd	= NULL												;

	CRect	rect;
	rect.SetRect( 0 , 350 , 170 , 350 + 89 );

	m_ctlRGBSelectStatic.Create( "RGB선택창" , WS_CHILD | WS_VISIBLE | SS_NOTIFY , rect , this	);
	m_ctlRGBSelectStatic.Init( CHSVControlStatic::CIRCLETYPE , RGB( 128 , 128 , 128 )			);

	return 0;
}

RwRGBA	CUITileList_MapTabWnd::GetVertexColor()
{
	return RGBtoRwRGBA( RGB( m_ctlRGBSelectStatic.m_nR , m_ctlRGBSelectStatic.m_nG , m_ctlRGBSelectStatic.m_nB ) );
}

void	CUITileList_MapTabWnd::SetVertexColor( int r , int g , int b )
{
	m_ctlRGBSelectStatic.SetRGB( r , g , b );
}

void	CUITileList_MapTabWnd::SetBrushType( int type )
{
	g_pMainFrame->m_Document.m_nBrushType = type;

	Invalidate( FALSE );

	// Parent에 알려줘야함..
	g_pMainFrame->PostMessage( WM_MAPBRUSHCHANGE );
}