// MapSelectStatic.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "apmmap.h"
#include "MapSelectStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapSelectStatic

void	__AlphaUpdate( CDC * pDC , CRect * pRect , COLORREF color )
{
	/*
	COLORREF	screen;
	color = ( color & ( 0xfefefefe ) >> 1 );
	for( int y = pRect->top ; y < pRect->bottom ; y ++ )
	{
		for( int x = pRect->left ; x < pRect->right ; x ++ )
		{
			screen = pDC->GetPixel( x , y );
			pDC->SetPixel( x , y , ( screen & ( 0xfefefefe ) >> 1 ) + color );
		}
	}
	*/

	for( int y = pRect->top ; y <= pRect->bottom ; y += 3 )
	{
		for( int x = pRect->left ; x <= pRect->right ; x += 3 )
		{
			pDC->MoveTo( pRect->left	, y );
			pDC->LineTo( pRect->right	, y );

			pDC->MoveTo( x , pRect->top		);
			pDC->LineTo( x , pRect->bottom	);
		}
	}
}

CMapSelectStatic::CMapSelectStatic()
{
//	preview = NULL;
	m_nMode	= NORMAL_LOADING_SELECT;

	m_bLButtonDown = FALSE;
	m_nCurrentMap	= WORLD_MAP;

	char	filename[ 1024 ];
	// 맵설정..
	m_MapList[ WORLD_MAP ].nStartOffsetX	= MAP_HARDCODED_OFFSET_X;
	m_MapList[ WORLD_MAP ].nStartOffsetZ	= MAP_HARDCODED_OFFSET_Z;
	m_MapList[ WORLD_MAP ].strName			= "월드맵";
	if( strlen( ALEF_CURRENT_DIRECTORY ) > 0 )
		wsprintf( filename , "%s\\Map\\%s" , ALEF_CURRENT_DIRECTORY , "full_map.bmp" );
	else
		wsprintf( filename , "Map\\%s" , "full_map.bmp" );

	if( m_MapList[ WORLD_MAP ].bmpMap.Load( filename , NULL ) )
	{

	}
	else
	{
		// 실패..
		m_MapList[ WORLD_MAP ].bmpMap.Create( 768 , 768 );
	
		::SetBkColor(m_MapList[ WORLD_MAP ].bmpMap.GetDC(), RGB( 255 , 113, 42 ) );
		CRect rect(0, 0, 768, 768);
		::ExtTextOut(m_MapList[ WORLD_MAP ].bmpMap.GetDC() , 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	}
/*
	// 맵설정..
	m_MapList[ DUNGEON_MAP ].nStartOffsetX	= 0;
	m_MapList[ DUNGEON_MAP ].nStartOffsetZ	= 0;
	m_MapList[ DUNGEON_MAP ].strName			= "던젼맵";
	wsprintf( filename , "%s\\Map\\%s" , ALEF_CURRENT_DIRECTORY , "dungeon.bmp" );
	if( m_MapList[ DUNGEON_MAP ].bmpMap.Load( filename ) )
	{

	}
	else
	{
		// 실패..
		m_MapList[ DUNGEON_MAP ].bmpMap.Create( 768 , 768 );
	
		::SetBkColor(m_MapList[ DUNGEON_MAP ].bmpMap.GetDC(), RGB( 255 , 113, 42 ) );
		CRect rect(0, 0, 768, 768);
		::ExtTextOut(m_MapList[ DUNGEON_MAP ].bmpMap.GetDC() , 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	}
*/
}

CMapSelectStatic::~CMapSelectStatic()
{
//	delete [] preview;
}

BEGIN_MESSAGE_MAP(CMapSelectStatic, CStatic)
	//{{AFX_MSG_MAP(CMapSelectStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMapSelectStatic::SetMode( int nMode )
{
	m_nMode = nMode;
}


/////////////////////////////////////////////////////////////////////////////
// CMapSelectStatic message handlers

void CMapSelectStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	MapInfo	* pMapInfo = GetMapInfo();
	ASSERT( NULL != pMapInfo );
	if( NULL == pMapInfo ) return;

	// TODO: Add your message handler code here and/or call default
	int x , y;
	if( GetIndex( point.x , point.y , x , y ) ) 
	{
		// 선택 되었다.

		if( m_nMode == NORMAL_LOADING_SELECT )
		{
			m_bSelected = TRUE;

			m_nSelectedIndexX = x;
			m_nSelectedIndexY = y;

			Invalidate( FALSE );

			ReportParent();
		}
		else if( m_nMode == EXPORT_RANGE_SELECT )
		{
			AddSelection( MakeDivisionIndex( x , y ) );
			Invalidate( FALSE );
		}
	}
	else
	{
		m_bSelected = FALSE;
		Invalidate( FALSE );
	}

	m_bLButtonDown = TRUE;
	SetCapture();
	
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CMapSelectStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	MapInfo	* pMapInfo = GetMapInfo();
	ASSERT( NULL != pMapInfo );
	if( NULL == pMapInfo ) return;
	
	// TODO: Add your message handler code here and/or call default
	if( m_bLButtonDown )
	{
		int x , y;
		if( GetIndex( point.x , point.y , x , y ) )
		{
			// 선택 되었다.
			m_bSelected = TRUE;

			m_nSelectedIndexX = x;
			m_nSelectedIndexY = y;

			Invalidate( FALSE );

			ReportParent();
		}
		else
		{
			m_bSelected = FALSE;
			Invalidate( FALSE );
		}
	}
	
	CStatic::OnMouseMove(nFlags, point);
}

void CMapSelectStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bLButtonDown = FALSE;
	ReleaseCapture();
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CMapSelectStatic::OnPaint() 
{
	MapInfo	* pMapInfo = GetMapInfo();
	ASSERT( NULL != pMapInfo );
	if( NULL == pMapInfo ) return;
	
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );
	pMapInfo->bmpMap.Draw( dc.GetSafeHdc() , 0 , 0 , rect.Width() , rect.Height() , 0 , 0 );

	// Selection이 있을경우 반전표시..

	switch( m_nMode )
	{
	case NORMAL_LOADING_SELECT:
		{
			if( m_bSelected )
			{
				// 셀렉션있는 부분을 강조해서 표시해줌.

				CPen	*pPrevPen;
				CPen	whitepen;
				whitepen.CreatePen( PS_SOLID , 1 , RGB( 255 , 255 , 255 ) );

				int	left	= ( m_nSelectedIndexX - pMapInfo->nStartOffsetX ) * ( ALEF_PREVIEW_DIVISION_WIDTH	);
				int top		= ( m_nSelectedIndexY - pMapInfo->nStartOffsetZ ) * ( ALEF_PREVIEW_DIVISION_WIDTH	);

				dc.MoveTo( left + 1 , top + 1 );
				dc.LineTo( left + 1 + ALEF_PREVIEW_DIVISION_WIDTH * ALEF_PREVIEW_MAP_SELECT_SIZE , top + 1 );
				dc.LineTo( left + 1 + ALEF_PREVIEW_DIVISION_WIDTH * ALEF_PREVIEW_MAP_SELECT_SIZE , top + 1 + ( ALEF_PREVIEW_DIVISION_WIDTH * ALEF_PREVIEW_MAP_SELECT_SIZE	) );
				dc.LineTo( left + 1 , top + 1 + ( ALEF_PREVIEW_DIVISION_WIDTH * ALEF_PREVIEW_MAP_SELECT_SIZE	) );
				dc.LineTo( left + 1 , top + 1 );

				pPrevPen = ( CPen * ) dc.SelectObject( & whitepen );
				dc.MoveTo( left , top );
				dc.LineTo( left + ALEF_PREVIEW_DIVISION_WIDTH * ALEF_PREVIEW_MAP_SELECT_SIZE , top );
				dc.LineTo( left + ALEF_PREVIEW_DIVISION_WIDTH * ALEF_PREVIEW_MAP_SELECT_SIZE , top + ( ALEF_PREVIEW_DIVISION_WIDTH * ALEF_PREVIEW_MAP_SELECT_SIZE	) );
				dc.LineTo( left , top + ( ALEF_PREVIEW_DIVISION_WIDTH * ALEF_PREVIEW_MAP_SELECT_SIZE	) );
				dc.LineTo( left , top );

				dc.SelectObject( pPrevPen );
			}
		}
		break;
	case EXPORT_RANGE_SELECT:
		{
			int					nValue;

			CRect				rect;

			CPen	redpen;
			redpen.CreatePen( PS_SOLID , 1 , RGB( 255 , 0 , 0 ) );
			dc.SelectObject( redpen );

			vector <int>::iterator Iter;
			for ( Iter = m_vectorDivision.begin( ) ; Iter != m_vectorDivision.end( ) ; Iter++ )
			{
				nValue = *Iter;

				// 마고자 (2004-12-07 오후 5:25:46) : 
				// 범위체크..
				if( pMapInfo->nStartOffsetX <= ( INT32 ) GetDivisionXIndex( nValue )						&&
					( INT32 ) GetDivisionXIndex( nValue ) < pMapInfo->nStartOffsetX + MAP_HARDCODED_RANGE_X	&&
					
					pMapInfo->nStartOffsetZ <= ( INT32 ) GetDivisionZIndex( nValue )						&&
					( INT32 ) GetDivisionZIndex( nValue ) < pMapInfo->nStartOffsetZ + MAP_HARDCODED_RANGE_Z	)
				{
					rect.left	= ( GetDivisionXIndex( nValue ) - pMapInfo->nStartOffsetX ) * ( ALEF_PREVIEW_DIVISION_WIDTH	);
					rect.top	= ( GetDivisionZIndex( nValue ) - pMapInfo->nStartOffsetZ ) * ( ALEF_PREVIEW_DIVISION_WIDTH	);

					rect.right	= rect.left	+ ALEF_PREVIEW_DIVISION_WIDTH;
					rect.bottom	= rect.top	+ ALEF_PREVIEW_DIVISION_WIDTH;

					__AlphaUpdate( &dc , &rect , RGB( 255 , 0 , 0 ) );
				}
			}
		}
		break;
	}
	
	// Do not call CStatic::OnPaint() for painting messages
}

void CMapSelectStatic::Init()
{
	m_bSelected			= TRUE	;
	m_nSelectedIndexX	= 0		;
	m_nSelectedIndexY	= 0		;

	if( ALEF_PREVIEW_MAP_SELECT_SIZE > 3 )
	{
		ALEF_PREVIEW_MAP_SELECT_SIZE /= MAP_DEFAULT_DEPTH;
		if( ALEF_PREVIEW_MAP_SELECT_SIZE > 3 )
		{
			ALEF_PREVIEW_MAP_SELECT_SIZE = 2;
		}
	}

}

int CMapSelectStatic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

void CMapSelectStatic::LoadPreviewData()
{
	// do nothing
}

BOOL CMapSelectStatic::GetIndex(int mousex, int mousey , int & indexX , int & indexY )
{
	int x = mousex / ( ALEF_PREVIEW_DIVISION_WIDTH	); 
	int y = mousey / ( ALEF_PREVIEW_DIVISION_WIDTH	);

	if( x < 0 || y < 0 || x >= MAP_HARDCODED_RANGE_X || y >= MAP_HARDCODED_RANGE_Z ) // 50 은 Division Max
	{
		return FALSE;
	}
	else
	{
		MapInfo*	pMapInfo = GetMapInfo();

		indexX = pMapInfo->nStartOffsetX + x;
		indexY = pMapInfo->nStartOffsetZ + y;
		
		return TRUE;
	}
}


void	CMapSelectStatic::ReportParent()
{
	//WM_MAPFOCUSCHANGED
	//WM_MAPSELECTED
	GetParent()->PostMessage( WM_MAPFOCUSCHANGED , m_nSelectedIndexX , m_nSelectedIndexY );
}


void	CMapSelectStatic::ChangeSelectSize( int zDelta )
{
	if( zDelta < 0 )
	{
		// 범위 증가..

		ALEF_PREVIEW_MAP_SELECT_SIZE += 1;
		if( ALEF_PREVIEW_MAP_SELECT_SIZE > 3 ) 
			ALEF_PREVIEW_MAP_SELECT_SIZE = 3;

		if( m_nSelectedIndexX + ALEF_PREVIEW_MAP_SELECT_SIZE > MAP_DIVISION_MAX )
			m_nSelectedIndexX = MAP_DIVISION_MAX - 1;
		if( m_nSelectedIndexY + ALEF_PREVIEW_MAP_SELECT_SIZE > MAP_DIVISION_MAX )
			m_nSelectedIndexY = MAP_DIVISION_MAX - 1;
		Invalidate( FALSE );
	}
	else
	{
		// 범위 줄임.
		ALEF_PREVIEW_MAP_SELECT_SIZE -= 1;
		if( ALEF_PREVIEW_MAP_SELECT_SIZE < 1 ) 
			ALEF_PREVIEW_MAP_SELECT_SIZE = 1;
		Invalidate( FALSE );
	}

	ReportParent();
}

int		CMapSelectStatic::GetMapFromPos	( int x , int z )
{
	int i;
	MapInfo	* pMapInfo	;
	for( i = 0 ; i < MAP_MAX ; ++i )
	{
		pMapInfo = &m_MapList[ i ];

		if( pMapInfo->nStartOffsetX	<= x && x < pMapInfo->nStartOffsetX + MAP_HARDCODED_RANGE_X	&&
			pMapInfo->nStartOffsetZ	<= z && z < pMapInfo->nStartOffsetZ + MAP_HARDCODED_RANGE_Z	)
		{
			return i;
		}
	}

	// 없는경우..

	return WORLD_MAP;
}

void CMapSelectStatic::AddSelection( int nDivision )
{
	// 선택을 저장한다.

	vector <int>::iterator Iter;
	int nValue;

	for ( Iter = m_vectorDivision.begin( ) ; Iter != m_vectorDivision.end( ) ; Iter++ )
	{
		nValue = *Iter;

		if( nValue == nDivision )
		{
			//m_listDivision.RemoveNode( pNode );

			m_vectorDivision.erase( Iter );

			return;
		}
	}

	m_vectorDivision.push_back( nDivision );
}