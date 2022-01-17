// RgbSelectStatic.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "RgbSelectStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRgbSelectStatic

CRgbSelectStatic::CRgbSelectStatic()
{
	m_nRGBX		= 0;
	m_nRGBY		= 0;
	m_nBrightY	= 2;

	m_bLButtonDown	= FALSE;
}

CRgbSelectStatic::~CRgbSelectStatic()
{
}


BEGIN_MESSAGE_MAP(CRgbSelectStatic, CStatic)
	//{{AFX_MSG_MAP(CRgbSelectStatic)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRgbSelectStatic message handlers
void	CRgbSelectStatic::SetRGB( COLORREF rgb )
{
	m_RGB = rgb;

	Invalidate( FALSE );
}

int CRgbSelectStatic::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CRgbSelectStatic::Init( COLORREF pickedcolor ) 
{
	m_PickedColor = pickedcolor;

	m_Bitmap.LoadBitmap( IDB_COLORTABLE );
	
	CDC	*pDC = GetDC();
	m_memDC.CreateCompatibleDC( pDC );
	m_memDC.SelectObject( m_Bitmap );
	ReleaseDC( pDC );

	CRect	rect;
	rect.SetRect( 0 , 0 , RGBSELECTCONTROL_WIDTH , RGBSELECTCONTROL_HEIGHT );
	ClientToScreen( rect );
	GetParent()->ScreenToClient( rect );
	MoveWindow( rect );

	Update();
}

void CRgbSelectStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect	rect;
	rect.SetRect( RGBSELECT_MARGIN , RGBSELECT_MARGIN , RGBSELECT_MARGIN + RGBSELECT_COLOR_WIDTH , RGBSELECTCONTROL_HEIGHT - RGBSELECT_MARGIN );
	if( rect.PtInRect( point ) )
	{
		// 픽셀 정보를 얻어서 저장..
		m_nRGBX	= point.x;
		m_nRGBY	= point.y;
	}

	rect.SetRect(	RGBSELECTCONTROL_WIDTH - RGBSELECT_PREVIEW - RGBSELECT_MARGIN - RGBSELECT_BRIGHT_WIDTH	, RGBSELECT_MARGIN							,
					RGBSELECTCONTROL_WIDTH - RGBSELECT_MARGIN - RGBSELECT_BRIGHT_WIDTH						, RGBSELECTCONTROL_HEIGHT - RGBSELECT_MARGIN );

	if( rect.PtInRect( point ) )
	{
		// 픽셀 정보를 얻어서 저장..
		m_nBrightY = point.y;
	}

	Update();

	m_bLButtonDown = TRUE;
	SetCapture();
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CRgbSelectStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_bLButtonDown = FALSE;
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CRgbSelectStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bLButtonDown )
	{
		CRect	rect;
		rect.SetRect( RGBSELECT_MARGIN , RGBSELECT_MARGIN , RGBSELECT_MARGIN + RGBSELECT_COLOR_WIDTH , RGBSELECTCONTROL_HEIGHT - RGBSELECT_MARGIN );
		if( rect.PtInRect( point ) )
		{
			// 픽셀 정보를 얻어서 저장..
			m_nRGBX	= point.x;
			m_nRGBY	= point.y;
			Update();
			return;
		}

		rect.SetRect(	RGBSELECTCONTROL_WIDTH - RGBSELECT_PREVIEW - RGBSELECT_MARGIN - RGBSELECT_BRIGHT_WIDTH	, RGBSELECT_MARGIN							,
						RGBSELECTCONTROL_WIDTH - RGBSELECT_MARGIN - RGBSELECT_BRIGHT_WIDTH						, RGBSELECTCONTROL_HEIGHT - RGBSELECT_MARGIN );

		if( rect.PtInRect( point ) )
		{
			// 픽셀 정보를 얻어서 저장..
			m_nBrightY = point.y;
			Update();
			return;
		}

		Update();
	}
	
	CStatic::OnMouseMove(nFlags, point);
}

void CRgbSelectStatic::OnPaint() 
{
	CPaintDC dc(this);

	// 왼쪽에 선택된 칼라가 있다..

	dc.BitBlt( 0 , 0 , RGBSELECTCONTROL_WIDTH , RGBSELECTCONTROL_HEIGHT , &m_memDC , 0 , 0 , SRCCOPY );
	
	// 선택된 칼라 출력.

	if( m_nRGBX && m_nRGBY )
	{
		dc.SetPixel( m_nRGBX - 1 , m_nRGBY - 1 , RGB( 255 , 0 , 0 ) );
		dc.SetPixel( m_nRGBX + 0 , m_nRGBY - 1 , RGB( 255 , 0 , 0 ) );
		dc.SetPixel( m_nRGBX + 1 , m_nRGBY - 1 , RGB( 255 , 0 , 0 ) );
		dc.SetPixel( m_nRGBX - 1 , m_nRGBY + 0 , RGB( 255 , 0 , 0 ) );
		dc.SetPixel( m_nRGBX + 1 , m_nRGBY + 0 , RGB( 255 , 0 , 0 ) );
		dc.SetPixel( m_nRGBX - 1 , m_nRGBY + 1 , RGB( 255 , 0 , 0 ) );
		dc.SetPixel( m_nRGBX + 0 , m_nRGBY + 1 , RGB( 255 , 0 , 0 ) );
		dc.SetPixel( m_nRGBX + 1 , m_nRGBY + 1 , RGB( 255 , 0 , 0 ) );
	}

	if( m_nBrightY )
	{
		int startx = RGBSELECTCONTROL_WIDTH - RGBSELECT_MARGIN - RGBSELECT_BRIGHT_WIDTH - RGBSELECT_PREVIEW - 1;
		dc.SetPixel( startx , m_nBrightY , RGB( 255 , 0 , 0 ) );
		dc.SetPixel( startx + RGBSELECT_BRIGHT_WIDTH , m_nBrightY , RGB( 255 , 0 , 0 ) );

		for( int i = 0 ; i < RGBSELECT_BRIGHT_WIDTH + 2 ; i ++ )
		{
			dc.SetPixel( startx + i , m_nBrightY - 1 , RGB( 255 , 0 , 0 ) );
			dc.SetPixel( startx + i , m_nBrightY + 1 , RGB( 255 , 0 , 0 ) );
		}
	}

	// Draw Preview

	CRect	rect;
	rect.SetRect(	RGBSELECTCONTROL_WIDTH - RGBSELECT_PREVIEW , 0 ,
					RGBSELECTCONTROL_WIDTH						, RGBSELECTCONTROL_HEIGHT );
	
	dc.FillSolidRect( rect , m_RGB );
		
	// Do not call CStatic::OnPaint() for painting messages
}

void CRgbSelectStatic::Update()
{
	COLORREF	rgb;
	if( m_nRGBX && m_nRGBY )
		rgb = m_memDC.GetPixel( m_nRGBX , m_nRGBY );
	else
		rgb = m_PickedColor;

	float	brightrate = ( float ) ( RGBSELECTCONTROL_HEIGHT - RGBSELECT_MARGIN - m_nBrightY ) / ( float ) ( RGBSELECTCONTROL_HEIGHT - 2 * RGBSELECT_MARGIN );

	COLORREF	mixedrgb;
	mixedrgb = RGB(
		( int ) ( (float) GetRValue( rgb ) * brightrate ) ,
		( int ) ( (float) GetGValue( rgb ) * brightrate ) ,
		( int ) ( (float) GetBValue( rgb ) * brightrate ) );

	m_RGB = mixedrgb;

	Invalidate( FALSE );
}
