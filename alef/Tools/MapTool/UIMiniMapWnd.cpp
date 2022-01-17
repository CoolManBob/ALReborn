// UIMiniMapWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "UIMiniMapWnd.h"
#include "MainWindow.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern MainWindow			g_MainWindow	;
extern CMainFrame *			g_pMainFrame	;

/////////////////////////////////////////////////////////////////////////////
// CUIMiniMapWnd

CUIMiniMapWnd::CUIMiniMapWnd()
{
}

CUIMiniMapWnd::~CUIMiniMapWnd()
{
}


BEGIN_MESSAGE_MAP(CUIMiniMapWnd, CWnd)
	//{{AFX_MSG_MAP(CUIMiniMapWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUIMiniMapWnd message handlers

void CUIMiniMapWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// 블러킹 표시함..
	CRect	rect;
	GetClientRect( rect );

	dc.FillSolidRect( rect , RGB( 0 , 0 , 0) );
	
	CFont	font;
	font.CreatePointFont( 170 , "굴림" );
	dc.SelectObject( font );
	
	dc.SetTextColor( RGB( 255 , 255 , 255 ) );
	dc.SetBkMode( TRANSPARENT );
	dc.TextOut( 0 , 0 , "현재섹터" );
	
	CString	str, str2;
	if( g_MainWindow.m_pCurrentGeometry )
	{
		str.Format( "(%d,%d)" , g_MainWindow.m_pCurrentGeometry->GetArrayIndexX() , g_MainWindow.m_pCurrentGeometry->GetArrayIndexZ() );
		str2.Format( "Div%04d" , GetDivisionIndex( g_MainWindow.m_pCurrentGeometry->GetArrayIndexX() , g_MainWindow.m_pCurrentGeometry->GetArrayIndexZ() ) );
	}
	else
	{
		str.Format( "Unknown" );
		str2.Empty();
	}
	
	dc.TextOut( 0 , 20  , str	);
	dc.TextOut( 0 , 40 , str2	);

	if( g_MainWindow.m_nCurrentTileIndexX != -1 &&
		g_MainWindow.m_nCurrentTileIndexZ != -1 )
	{
		dc.SetTextColor( RGB( 200 , 162 , 235 ) );
		str.Format( "Local(%d,%d)" , g_MainWindow.m_nCurrentTileIndexX , g_MainWindow.m_nCurrentTileIndexZ );
		dc.DrawText( str , rect , DT_CENTER | DT_BOTTOM | DT_SINGLELINE );
	}
}
