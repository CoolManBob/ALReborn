// UITileListWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "UITileListWnd.h"
#include "MainWindow.h"
#include "MainFrm.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	MainWindow	g_MainWindow		;
extern CMainFrame * g_pMainFrame	;

/////////////////////////////////////////////////////////////////////////////
// CUITileListWnd

CUITileListWnd::CUITileListWnd()
{
	m_pMapWnd			= NULL;
	m_pObjectWnd		= NULL;
	m_pTabWnd			= NULL;
	m_pOthers			= NULL;

	for( int i = 0 ; i < EDITMODE_TYPECOUNT ; i ++ )
	{
		m_pMenu[ i ].pWnd = NULL;
	}

}

CUITileListWnd::~CUITileListWnd()
{
}


BEGIN_MESSAGE_MAP(CUITileListWnd, CWnd)
	//{{AFX_MSG_MAP(CUITileListWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUITileListWnd message handlers

void CUITileListWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

//	dc.SetTextColor( RGB( 255 , 255 , 255 ) );
//	dc.TextOut( 0 , 0 , "타일윈도우" );

	// 탭 출력.
	dc.SetBkMode( TRANSPARENT );

	CDC	memDC;
	memDC.CreateCompatibleDC( &dc );

	for( int i = 0 ; i < EDITMODE_TYPECOUNT ; i ++ )
	{
		if( m_pMenu[ i ].bEnable )
		{
			if( i == m_nSelectedTab )
			{
				memDC.SelectObject( m_pMenu[ i ].abitmap[ stMenuItem::MIB_SELECTED ] );
				dc.BitBlt( m_pMenu[ i ].rect.left , m_pMenu[ i ].rect.top , m_pMenu[ i ].rect.Width() , m_pMenu[ i ].rect.Height() , &memDC , 0 , 0 , SRCCOPY );
			}
			else
			{
				memDC.SelectObject( m_pMenu[ i ].abitmap[ stMenuItem::MIB_UNSELECT ] );
				dc.BitBlt( m_pMenu[ i ].rect.left , m_pMenu[ i ].rect.top , m_pMenu[ i ].rect.Width() , m_pMenu[ i ].rect.Height() , &memDC , 0 , 0 , SRCCOPY );
			}
		}
	}
	
	// 바탕 칠하기..
	CRect	rect;
	GetClientRect( rect );
	rect.top	= TAB_HEIGHT;
	dc.FillSolidRect( rect , RGB( 0 , 0 , 255 ) );

	
	// Do not call CWnd::OnPaint() for painting messages
}

void CUITileListWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	for( int i = 0 ; i < EDITMODE_TYPECOUNT ; i ++ )
	{
		if( m_pMenu[ i ].rect.PtInRect( point ) && m_pMenu[ i ].bEnable )
		{
			// 탭 선택;
			ChangeTab( i );

			break;
		}
	}
	
	
	CWnd::OnLButtonDown(nFlags, point);
}

int CUITileListWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 윈도우 생성.

	m_pMapWnd			= new CUITileList_MapTabWnd		;
	m_pTabWnd			= new CUITileList_TileTabWnd	;
	m_pObjectWnd		= new CUITileList_ObjectTabWnd	;
	m_pOthers			= new CUITileList_Others		;

	if (!m_pMapWnd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		return -1;
	}
	m_pMapWnd->ShowWindow( FALSE );

	if (!m_pTabWnd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		return -1;
	}
	m_pTabWnd->ShowWindow( FALSE );

	if (!m_pObjectWnd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		return -1;
	}
	m_pObjectWnd->ShowWindow( FALSE );
	
	if (!m_pOthers->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		return -1;
	}
	m_pOthers->ShowWindow( FALSE );
	// 탭의 내용을 설정한다.

	INT32	nOffset = 0;

	m_pMenu[ nOffset ].nIndex = EDITMODE_GEOMETRY;
	m_pMenu[ nOffset ].name	= "Geometry"								;
	m_pMenu[ nOffset ].rect	.SetRect(	0	,	0,	75 	,	TAB_HEIGHT)	;
	m_pMenu[ nOffset ].pWnd	= m_pMapWnd									;

	m_pMenu[ nOffset ].abitmap[ stMenuItem::MIB_SELECTED	].LoadBitmap( IDB_MODE_GEOMETRY_SELECTED );
	m_pMenu[ nOffset ].abitmap[ stMenuItem::MIB_UNSELECT	].LoadBitmap( IDB_MODE_GEOMETRY_UNSELECT );
	
	nOffset++;

	m_pMenu[ nOffset ].nIndex = EDITMODE_TILE;
	m_pMenu[ nOffset ].name	= "TILE"									;
	m_pMenu[ nOffset ].rect	.SetRect(	75	,	0,	150	,	TAB_HEIGHT)	;
	m_pMenu[ nOffset ].pWnd	= m_pTabWnd									;

	m_pMenu[ nOffset ].abitmap[ stMenuItem::MIB_SELECTED	].LoadBitmap( IDB_MODE_TILE_SELECTED );
	m_pMenu[ nOffset ].abitmap[ stMenuItem::MIB_UNSELECT	].LoadBitmap( IDB_MODE_TILE_UNSELECT );

	nOffset++;

	m_pMenu[ nOffset ].nIndex = EDITMODE_OBJECT;
	m_pMenu[ nOffset ].name	= "OBJECT"									;
	m_pMenu[ nOffset ].rect	.SetRect(	150	,	0,	225 ,	TAB_HEIGHT)	;
	m_pMenu[ nOffset ].pWnd	= m_pObjectWnd								;

	m_pMenu[ nOffset ].abitmap[ stMenuItem::MIB_SELECTED	].LoadBitmap( IDB_MODE_OBJECT_SELECTED );
	m_pMenu[ nOffset ].abitmap[ stMenuItem::MIB_UNSELECT	].LoadBitmap( IDB_MODE_OBJECT_UNSELECT );

	nOffset++;

	m_pMenu[ nOffset ].nIndex = EDITMODE_OTHERS;
	m_pMenu[ nOffset ].name	= "Others"								;
	m_pMenu[ nOffset ].rect	.SetRect(	225	,	0,	300 ,	TAB_HEIGHT)	;
	m_pMenu[ nOffset ].pWnd	= m_pOthers									;

	m_pMenu[ nOffset ].abitmap[ stMenuItem::MIB_SELECTED	].LoadBitmap( IDB_MODE_SUBDIV_SELECTED );
	m_pMenu[ nOffset ].abitmap[ stMenuItem::MIB_UNSELECT	].LoadBitmap( IDB_MODE_SUBDIV_UNSELECT );

	nOffset++;
	
	// 처음 텍스트 윈도우로 선택한다.
	m_nSelectedTab = EDITMODE_TILE;
	NotifyParent();

	m_pMenu[ m_nSelectedTab ].pWnd->ShowWindow( TRUE );
	return 0;
}

void CUITileListWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	for( int i = 0 ; i < EDITMODE_TYPECOUNT ; i ++ )
	{
		if( m_pMenu[ i ].pWnd )
		{
			if( m_pMenu[ i ].bEnable )
			{
				CRect	rect;
				rect.SetRect( 0 , TAB_HEIGHT , cx , cy );
				m_pMenu[ i ].pWnd->MoveWindow( rect );
			}
			else
			{
				m_pMenu[ i ].pWnd->ShowWindow( SW_HIDE );
			}
		}
	}
}

void CUITileListWnd::OnDestroy() 
{
	CWnd::OnDestroy();
	
	if( m_pMapWnd			){	m_pMapWnd			->DestroyWindow()	;delete m_pMapWnd			;	}
	if( m_pObjectWnd		){	m_pObjectWnd		->DestroyWindow()	;delete m_pObjectWnd		;	}
	if( m_pTabWnd			){	m_pTabWnd			->DestroyWindow()	;delete m_pTabWnd			;	}
	if( m_pOthers			){	m_pOthers			->DestroyWindow()	;delete m_pOthers			;	}
}

void CUITileListWnd::NotifyParent()
{
	CMainFrame * pFrame		= ( CMainFrame * ) GetParent()	;
	pFrame->m_Document.m_nSelectedMode	= m_nSelectedTab	;
}

void CUITileListWnd::ChangeTab( int tab , BOOL bReserve )
{
	if( tab == EDITMODE_TILE		&& m_nSelectedTab == tab )
	{
		// 레이어를 변경한다..
		if( bReserve )
		{
			m_pTabWnd->ChangeLayer( ( m_pTabWnd->m_nSelectedLayer - 1 + TILELAYER_COUNT ) % TILELAYER_COUNT );
		}
		else
		{
			m_pTabWnd->ChangeLayer( ( m_pTabWnd->m_nSelectedLayer + 1 ) % TILELAYER_COUNT );
		}
	}
	else
	if( tab == EDITMODE_GEOMETRY	&& m_nSelectedTab == tab )
	{
		// 레이어를 변경한다..
		if( bReserve )
		{
			m_pMapWnd->SetBrushType( ( g_pMainFrame->m_Document.m_nBrushType - 1 + BRUSHTYPE_COUNT ) % BRUSHTYPE_COUNT );
		}
		else
		{
			m_pMapWnd->SetBrushType( ( g_pMainFrame->m_Document.m_nBrushType + 1 ) % BRUSHTYPE_COUNT );
		}
	}
	else
	if( m_nSelectedTab != tab && m_pMenu[ tab ].pWnd )
	{
		// 탭 체인지 이벤트

		// m_nSelectedTab 탭 종료
		m_pMenu[ m_nSelectedTab ].pWnd->ShowWindow( FALSE );

		// i 탭 로딩.
		m_pMenu[ tab ].pWnd->ShowWindow( TRUE );

		g_MainWindow.OnEditModeChange( tab , m_nSelectedTab );

		m_nSelectedTab = tab;
		NotifyParent();
		Invalidate( FALSE );
	}
	else
	{
		// 같은거 찍어봐야 바뀌는거 없다니까.
	}
}

INT32	CUITileListWnd::GetCurrentMode	()
{
	ASSERT( m_nSelectedTab < EDITMODE_TYPECOUNT );
	return m_pMenu[ m_nSelectedTab ].nIndex;
}
