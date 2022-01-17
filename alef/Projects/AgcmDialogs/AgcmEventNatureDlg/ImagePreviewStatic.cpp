// ImagePreviewStatic.cpp : implementation file
//

#include "stdafx.h"
//#include "maptool.h"
#include "ImagePreviewStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImagePreviewStatic

CImagePreviewStatic::CImagePreviewStatic()
{
}

CImagePreviewStatic::~CImagePreviewStatic()
{
}


BEGIN_MESSAGE_MAP(CImagePreviewStatic, CStatic)
	//{{AFX_MSG_MAP(CImagePreviewStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImagePreviewStatic message handlers

void CImagePreviewStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 255 , 255 , 255 ) );

	if( m_nIndex == -1 )
	{
		dc.DrawText( "No Image" , rect , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}
	else
	{
		m_Bmp.Draw	( dc.GetSafeHdc()							,
					( rect.Width()	- m_Bmp.GetWidth()	) / 2	,
					( rect.Height()	- m_Bmp.GetHeight()	) / 2	);
	}
	
	// Do not call CStatic::OnPaint() for painting messages
}

INT32	CImagePreviewStatic::SetIndex( INT32 nIndex , char * pFilename )
{
	m_Bmp.Release();

	m_nIndex = nIndex;

	if( m_nIndex != -1 && pFilename )
	{
		m_Bmp.Load( pFilename , NULL );
	}

	if( m_nIndex != -1 && pFilename == NULL )
	{
		char strFilename[ 1024 ];
		wsprintf( strFilename , "%s\\Map\\Temp\\SKY%05d.bmp" , m_pEventNatureDlg->m_strCurrentDirectory , m_nIndex );

		m_Bmp.Load( strFilename , NULL );
	}

	Invalidate();

	return m_nIndex;
}

void CImagePreviewStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_nIndex = -1;
	Invalidate();

	TRACE( "Ä®¶ó»èÁ¦..\n" );
	GetParent()->PostMessage( WM_IMAGEPREVIEW_NOTIFY_EVENT , m_nIndex );
	
	CStatic::OnLButtonDown(nFlags, point);
}
