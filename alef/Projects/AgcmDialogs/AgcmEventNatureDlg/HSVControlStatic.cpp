// HSVControlStatic.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "HSVControlStatic.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b);
void rgb_to_hsv(int r, int g, int b, float *h, float *s, float *v);

/////////////////////////////////////////////////////////////////////////////
// CHSVControlStatic

CHSVControlStatic::CHSVControlStatic()
{
	m_nType			= BOXTYPE	;
	m_bPreviewPress	= FALSE		;
	m_bLButtonDown	= FALSE		;
}

CHSVControlStatic::~CHSVControlStatic()
{
}


BEGIN_MESSAGE_MAP(CHSVControlStatic, CStatic)
	//{{AFX_MSG_MAP(CHSVControlStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHSVControlStatic message handlers

void CHSVControlStatic::Init( int type , COLORREF color )
{
	m_nType = type;

	// 초기화..
	m_nR	= GetRValue( color );
	m_nG	= GetGValue( color );
	m_nB	= GetBValue( color );

	rgb_to_hsv( m_nR , m_nG , m_nB , & m_fH , & m_fS , & m_fV );

	// 데이타 초기화.
	// 백버퍼 초기화.

	switch( type )
	{
	case	BOXTYPE		:
		{
			// 비트맵 초기화..
			CRect	rect;
			GetClientRect( rect );

			// HS , V 테이블 생성.
			CDC	* pDC = GetDC();
			m_Bitmap1.CreateCompatibleBitmap( pDC , m_nBox_H_Width = ( ( int ) ( ( float ) rect.Width() * 0.9f ) ), rect.Height()		);
			m_Bitmap2.CreateCompatibleBitmap( pDC , rect.Width() - m_nBox_H_Width , rect.Height()						);

			m_MemDC1.CreateCompatibleDC( pDC );
			m_MemDC2.CreateCompatibleDC( pDC );

			m_MemDC1.SelectObject( m_Bitmap1 );
			m_MemDC2.SelectObject( m_Bitmap2 );
			ReleaseDC( pDC );

			// 테이블 그리자.

			CRect	rectBox;
			rectBox = rect;

			rectBox.right	= m_nBox_H_Width				;
			m_RectHS		= rectBox						;
			DrawHSTable	( & m_MemDC1 , rectBox	)			;

			rectBox.left	= m_nBox_H_Width				;
			rectBox.right	= rect.right					;
			m_RectV			= rectBox						;

			rectBox.left	= 0								;
			rectBox.right	= m_RectV.Width()				;
			DrawVTable	( & m_MemDC2 , rectBox	)			;
		}
		break;
	case	ALPHATYPE	:
		{
			// 비트맵 초기화..
			CRect	rect;
			GetClientRect( rect );

			// HS , V 테이블 생성.
			CDC	* pDC = GetDC();
			m_Bitmap2.CreateCompatibleBitmap( pDC , rect.Width() , rect.Height() );

			m_MemDC2.CreateCompatibleDC( pDC );

			m_MemDC2.SelectObject( m_Bitmap2 );
			ReleaseDC( pDC );

			// 테이블 그리자.

			DrawVTable	( & m_MemDC2 , rect	);
		}
		break;

	case	CIRCLETYPE	:
		// 둥근형태.. 크기조절들어감.
		{
			m_RectCircle	.SetRect( 0 , 0 , HSV_CIRCLE_WIDTH , HSV_CIRCLE_HEIGHT									);
			m_RectVBox		.SetRect( HSV_CIRCLE_BOX_X1 , HSV_CIRCLE_BOX_Y1 , HSV_CIRCLE_BOX_X2 , HSV_CIRCLE_BOX_Y2	);
			m_RectPreview	.SetRect( 6 , 173 , 52 , 201															);
			m_RectRef		.SetRect( 145 , 169 , 187 , 201															);

			CRect	rect;
			GetClientRect( rect );
			ClientToScreen( rect );
			GetParent()->ScreenToClient( rect );

			rect.right	= rect.left	+ m_RectCircle.Width()	;
			rect.bottom	= rect.top	+ m_RectCircle.Height()	;
			MoveWindow( rect );

			// 비트맵 로딩 & 생성.
			CDC	* pDC = GetDC();
			m_Bitmap1.LoadBitmap				( IDB_HSV_TABLE										);
			m_Bitmap2.CreateCompatibleBitmap	( pDC , m_RectVBox.Width() , m_RectVBox.Height()	);

			m_MemDC1.CreateCompatibleDC( pDC );
			m_MemDC2.CreateCompatibleDC( pDC );

			m_MemDC1.SelectObject( m_Bitmap1 );
			m_MemDC2.SelectObject( m_Bitmap2 );

			m_bmpBackBuffer		.CreateCompatibleBitmap	( pDC , HSV_CIRCLE_WIDTH , HSV_CIRCLE_HEIGHT	);
			m_MemDCBackBuffer	.CreateCompatibleDC		( pDC											);
			m_MemDCBackBuffer	.SelectObject			( m_bmpBackBuffer								);

			ReleaseDC( pDC );

			// 이미지 생성
			DrawSVTable( &m_MemDC2 , m_RectVBox );
			
		}
		break;
	}

	m_BlackPen.CreatePen( PS_SOLID , 1 , RGB( 0		, 0		, 0		) );
	m_WhitePen.CreatePen( PS_SOLID , 1 , RGB( 255	, 255	, 255	) );

	m_Font.CreatePointFont( 80 , "굴림" );

	m_NullBrush.CreateStockObject( NULL_BRUSH );

	CWinApp *app = AfxGetApp();
	VERIFY( m_hCursor = app->LoadCursor( IDC_SPOID ) );

	m_nPosition = POS_NONE;
}

void CHSVControlStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	switch( m_nType )
	{
	case	ALPHATYPE	:	ALPHA_LButtonDown	( nFlags , point );	break;
	case	BOXTYPE		:	BOX_LButtonDown		( nFlags , point );	break;
	case	CIRCLETYPE	:	CIRCLE_LButtonDown	( nFlags , point );	break;
	default:
		break;
	}
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CHSVControlStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	switch( m_nType )
	{
	case	ALPHATYPE	:	ALPHA_LButtonUp		( nFlags , point );	break;
	case	BOXTYPE		:	BOX_LButtonUp		( nFlags , point );	break;
	case	CIRCLETYPE	:	CIRCLE_LButtonUp	( nFlags , point );	break;
	default:
		break;
	}
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CHSVControlStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	switch( m_nType )
	{
	case	ALPHATYPE	:	ALPHA_MouseMove		( nFlags , point );	break;
	case	BOXTYPE		:	BOX_MouseMove		( nFlags , point );	break;
	case	CIRCLETYPE	:	CIRCLE_MouseMove	( nFlags , point );	break;
	default:
		break;
	}
	
	CStatic::OnMouseMove(nFlags, point);
}

void CHSVControlStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect	rect;
	GetClientRect( rect );

	switch( m_nType )
	{
	case BOXTYPE:
		{
			dc.BitBlt( 0 , 0 , m_RectHS.Width() , m_RectHS.Height() , & m_MemDC1 , 0 , 0 , SRCCOPY );
			dc.BitBlt( m_RectV.left , 0 , m_RectV.Width() , m_RectV.Height() , & m_MemDC2 , 0 , 0 , SRCCOPY );

			BOX_DrawSelectBox( &dc , rect );
		}
		break;
 	case ALPHATYPE	:
		{
			if( m_bLButtonDown )
			{
				dc.BitBlt( rect.left , 0 , rect.Width() , rect.Height() , & m_MemDC2 , 0 , 0 , SRCCOPY );

				ALPHA_DrawSelectBox( &dc , rect );
			}
			else
			{
				dc.FillSolidRect( rect , GetRGB() );
				dc.SetBkMode( TRANSPARENT );

				dc.SetTextColor( RGB( 255 , 255 , 0 ) );
				dc.TextOut( 0 , 0 , "Alpha" );
			}
		}
		break;

	case CIRCLETYPE:
		{
			
			m_MemDCBackBuffer.BitBlt( 0 , 0 , rect.Width() , rect.Height() , & m_MemDC1 , 0 , 0 , SRCCOPY );
			m_MemDCBackBuffer.BitBlt( m_RectVBox.left , m_RectVBox.top , m_RectVBox.Width() , m_RectVBox.Height() , & m_MemDC2 , 0 , 0 , SRCCOPY );

			CIRCLE_DrawSelectBox( &m_MemDCBackBuffer , rect );
		
			m_MemDCBackBuffer.FillSolidRect( m_RectPreview , RGB( m_nR , m_nG , m_nB ) );

			// RGB값 출력..
			m_MemDCBackBuffer.SelectObject( m_Font );
			m_MemDCBackBuffer.SetBkMode( TRANSPARENT );
			m_MemDCBackBuffer.SetTextColor( RGB( 0 , 0 , 0 ) );

			if( m_bPreviewPress )
			{
				m_MemDCBackBuffer.FillSolidRect( m_RectRef , RGB( 255 , 0 , 0 ) );
				m_MemDCBackBuffer.DrawText( "칼라입력중!" , m_RectRef , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
			}
			else
			{
				CString	str;
				str.Format( "R:%d" , m_nR );	m_MemDCBackBuffer.TextOut( m_RectRef.left + 2 , m_RectRef.top + 2 + 0	, str	);
				str.Format( "G:%d" , m_nG );	m_MemDCBackBuffer.TextOut( m_RectRef.left + 2 , m_RectRef.top + 2 + 9	, str	);
				str.Format( "B:%d" , m_nB );	m_MemDCBackBuffer.TextOut( m_RectRef.left + 2 , m_RectRef.top + 2 + 18	, str	);
			}
			
			dc.BitBlt( 0 , 0 , rect.Width() , rect.Height() , & m_MemDCBackBuffer , 0 , 0 , SRCCOPY );
		}
		break;
	}
	
	// Do not call CStatic::OnPaint() for painting messages
}

void CHSVControlStatic::DrawHSTable(CDC *pDC, CRect &rect)
{
	float	h,s,v;
	int		r,g,b;

	v = 1.0;

	// h 가 x 축 s 가 y 축으로..;
	COLORREF	rgb;

	for( int y = rect.top ; y < rect.bottom ; y ++ )
	{
		// s 벨루 구하기..
		s = ( float )( y - rect.top ) / ( float ) ( rect.bottom		-	rect.top		);

		for( int x = rect.left ; x < rect.right ; x ++ )
		{
			// h 벨류 구하기..
			h = ( float ) ( x - rect.left ) / ( float ) ( rect.right	-	rect.left		) * 360.0f;

			// hsv로 RGB값 구하기..

			hsv_to_rgb( h , s , v , &r , &g , &b );
			rgb	= RGB( r , g , b );

			// 픽셀 찍기.

			pDC->SetPixel( x , y , rgb );
		}
	}
}

void CHSVControlStatic::DrawVTable	( CDC * pDC , CRect &rect )
{
	float	h,s,v;
	int		r,g,b;

	h	= 0.0f;
	s	= 0.0f;

	// h 가 x 축 s 가 y 축으로..;
	COLORREF	rgb;

	for( int y = rect.top ; y < rect.bottom ; y ++ )
	{
		// s 벨루 구하기..
		v = ( float )( y - rect.top ) / ( float ) ( rect.bottom		-	rect.top		);
		hsv_to_rgb( h , s , v , &r , &g , &b );
		rgb	= RGB( r , g , b );

		// 픽셀 찍기.

		for( int x = rect.left ; x < rect.right ; x ++ )
		{
			pDC->SetPixel( x , y , rgb );
		}
	}
}

void CHSVControlStatic::ALPHA_DrawSelectBox	( CDC * pDC , CRect &rect	)
{
	// 냐냥;;..

	// H S V 값을 좌표 값으로 변경함..
	CPoint	v;

	v.x		= rect.CenterPoint().x				;
	v.y		= ( int ) ( ( float ) rect.Height() * m_fV				);

	// 서클 그림..

	CRect	rectCircle;
	pDC->SelectObject	( m_NullBrush	);
	pDC->SelectObject	( m_BlackPen	);

	rectCircle.left		= v.x - 1;
	rectCircle.top		= v.y - 1;
	rectCircle.right	= v.x + 3;
	rectCircle.bottom	= v.y + 3;
	pDC->Ellipse		( rectCircle );

	pDC->SelectObject	( m_WhitePen );

	rectCircle.left		= v.x - 2;
	rectCircle.top		= v.y - 2;
	rectCircle.right	= v.x + 2;
	rectCircle.bottom	= v.y + 2;
	pDC->Ellipse		( rectCircle );
}
void CHSVControlStatic::BOX_DrawSelectBox	( CDC * pDC , CRect &rect	)
{
	// 냐냥;;..

	// H S V 값을 좌표 값으로 변경함..
	CPoint	hs;
	CPoint	v;

	hs.x	= ( int ) ( ( float ) m_RectHS.Width() * ( m_fH / 255.0f )	);
	hs.y	= ( int ) ( ( float ) m_RectHS.Height() * m_fS				);

	v.x		= m_RectV.CenterPoint().x				;
	v.y		= ( int ) ( ( float ) m_RectV.Height() * m_fV				);


	// 서클 그림..

	CRect	rectCircle;
	pDC->SelectObject	( m_NullBrush	);
	pDC->SelectObject	( m_BlackPen	);

	rectCircle.left		= hs.x - 1;
	rectCircle.top		= hs.y - 1;
	rectCircle.right	= hs.x + 3;
	rectCircle.bottom	= hs.y + 3;
	// 검은 박스.
	pDC->Ellipse		( rectCircle );

	rectCircle.left		= v.x - 1;
	rectCircle.top		= v.y - 1;
	rectCircle.right	= v.x + 3;
	rectCircle.bottom	= v.y + 3;
	pDC->Ellipse		( rectCircle );

	pDC->SelectObject	( m_WhitePen );

	rectCircle.left		= hs.x - 2;
	rectCircle.top		= hs.y - 2;
	rectCircle.right	= hs.x + 2;
	rectCircle.bottom	= hs.y + 2;
	pDC->Ellipse		( rectCircle );
	
	rectCircle.left		= v.x - 2;
	rectCircle.top		= v.y - 2;
	rectCircle.right	= v.x + 2;
	rectCircle.bottom	= v.y + 2;
	pDC->Ellipse		( rectCircle );
}

void CHSVControlStatic::BOX_LButtonDown		( UINT nFlags, CPoint point	)
{
	if( m_RectHS.PtInRect( point ) )
	{
		m_fS = ( float ) ( point.y - m_RectHS.top	) / ( float ) ( m_RectHS.bottom	-	m_RectHS.top		)			;
		m_fH = ( float ) ( point.x - m_RectHS.left	) / ( float ) ( m_RectHS.right	-	m_RectHS.left		) * 360.0f	;

		hsv_to_rgb( m_fH , m_fS , m_fV , &m_nR , &m_nG , &m_nB );
		Invalidate( FALSE );

		GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );
	}
	else
	if( m_RectV.PtInRect( point ) )
	{
		m_fV = ( float ) ( point.y - m_RectHS.top ) / ( float ) ( m_RectHS.bottom		-	m_RectHS.top		);

		hsv_to_rgb( m_fH , m_fS , m_fV , &m_nR , &m_nG , &m_nB );
		Invalidate( FALSE );
		GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );
	}
}
void CHSVControlStatic::BOX_LButtonUp		( UINT nFlags, CPoint point	)
{
}
void CHSVControlStatic::BOX_MouseMove		( UINT nFlags, CPoint point	)
{
}

void CHSVControlStatic::DrawSVTable			( CDC * pDC , CRect	&rect	)
{
	float	h,s,v;
	int		r,g,b;

	h  = m_fH;

	// h 가 x 축 s 가 y 축으로..;
	COLORREF	rgb;

	for( int y = rect.top ; y < rect.bottom ; y ++ )
	{
		// s 벨루 구하기..
		v = ( float )( y - rect.top ) / ( float ) ( rect.bottom		-	rect.top		);

		for( int x = rect.left ; x < rect.right ; x ++ )
		{
			// h 벨류 구하기..
			s = ( float ) ( x - rect.left ) / ( float ) ( rect.right	-	rect.left		);

			// hsv로 RGB값 구하기..

			hsv_to_rgb( h , s , v , &r , &g , &b );
			rgb	= RGB( r , g , b );

			// 픽셀 찍기.

			pDC->SetPixel( x - rect.left , y - rect.top , rgb );
		}
	}
}

void CHSVControlStatic::CIRCLE_DrawSelectBox	( CDC * pDC , CRect &rect	)
{
	// 
	// 냐냥;;..

	// H S V 값을 좌표 값으로 변경함..
	CPoint	h;
	CPoint	sv;

	double	theta;
	theta = + 2.0 * 3.1415927 / 360.0 * ( double ) m_fH - 55.0 / 360.0 * 2.0 * 3.14159262			;

	h.x		= HSV_CIRCLE_CENTER_X + ( int ) ( ( double ) BAN_JI_RUM * sin( theta )	)	;
	h.y		= HSV_CIRCLE_CENTER_Y - ( int ) ( ( double ) BAN_JI_RUM * cos( theta )	)	;

	sv.x	= m_RectVBox.left	+ ( int ) ( ( float ) m_RectVBox.Width()	* m_fS	)	;
	sv.y	= m_RectVBox.top	+ ( int ) ( ( float ) m_RectVBox.Height()	* m_fV	)	;

	// 서클 그림..

	CRect	rectCircle;
	pDC->SelectObject	( m_NullBrush	);
	pDC->SelectObject	( m_BlackPen	);

	rectCircle.left		= h.x - 4;
	rectCircle.top		= h.y - 4;
	rectCircle.right	= h.x + 6;
	rectCircle.bottom	= h.y + 6;
	// 검은 박스.
	pDC->Ellipse		( rectCircle );

	rectCircle.left		= sv.x - 4;
	rectCircle.top		= sv.y - 4;
	rectCircle.right	= sv.x + 6;
	rectCircle.bottom	= sv.y + 6;
	pDC->Ellipse		( rectCircle );

	pDC->SelectObject	( m_WhitePen );

	rectCircle.left		= h.x - 5;
	rectCircle.top		= h.y - 5;
	rectCircle.right	= h.x + 5;
	rectCircle.bottom	= h.y + 5;
	pDC->Ellipse		( rectCircle );
	
	rectCircle.left		= sv.x - 5;
	rectCircle.top		= sv.y - 5;
	rectCircle.right	= sv.x + 5;
	rectCircle.bottom	= sv.y + 5;
	pDC->Ellipse		( rectCircle );
}

void CHSVControlStatic::CIRCLE_LButtonDown		( UINT nFlags, CPoint point	)
{
	if( m_RectVBox.PtInRect( point ) )
	{
		m_fS = ( float ) ( point.x - m_RectVBox.left	) / ( float ) ( m_RectVBox.right	-	m_RectVBox.left		);
		m_fV = ( float ) ( point.y - m_RectVBox.top		) / ( float ) ( m_RectVBox.bottom	-	m_RectVBox.top		);

		hsv_to_rgb( m_fH , m_fS , m_fV , &m_nR , &m_nG , &m_nB );
		Invalidate( FALSE );

		GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );

		m_nPosition = POS_SATVAL;
		SetCapture();
	}
	else if( m_RectRef.PtInRect( point ) )
	{
		SetCapture();
		SetCursor( m_hCursor );

		m_bPreviewPress	= TRUE;

		// 스포이드 기능과 동시에 한다..
		// Left up이 이 박으 안에서 일어날경우..
		// RGB창을 띄우고 , 그렇지 않으면 스포이드 기능을 수행한다..

		m_nRBackup		= m_nR	;	// 0~255
		m_nGBackup		= m_nG	;	// 0~255
		m_nBBackup		= m_nB	;	// 0~255
	}
	else
	{
		// 반지름 계산.
		double	radius;

		radius = sqrt
		(
			double
			(
				( point.x - HSV_CIRCLE_CENTER_X ) * ( point.x - HSV_CIRCLE_CENTER_X ) +
				( point.y - HSV_CIRCLE_CENTER_Y ) * ( point.y - HSV_CIRCLE_CENTER_Y )
			)
		);

		if( ( int ) radius <= 83 && ( int ) radius >= 63 )
		{
			// 범위...

			double theta;

			double x2 , y2;
			x2 = ( double ) ( point.x - HSV_CIRCLE_CENTER_X );
			y2 = ( double ) ( point.y - HSV_CIRCLE_CENTER_Y );

			if( x2 >= 0.0 )
			{
				// 0보다 크면..
				theta = acos( -y2 / radius );
			}
			else
			{
				// 0보다 작으면..
				theta = acos( y2 / radius );
				theta += 3.1415927;
			}

			// Hue 값을 구한다.

			m_fH = ( float ) ( + 360.0 / ( 2.0 * 3.1415927 ) * ( theta + 55.0 * 2.0 * 3.1415927 / 360.0 ) );

			while( m_fH > 360.0f	) m_fH -= 360.0f;
			while( m_fH < 0.0f		) m_fH += 360.0f;

			DrawSVTable( &m_MemDC2 , m_RectVBox );
			
			hsv_to_rgb( m_fH , m_fS , m_fV , &m_nR , &m_nG , &m_nB );
			Invalidate( FALSE );

			GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );
			
			m_nPosition = POS_HUE;
			SetCapture();
		}
	}
}

void CHSVControlStatic::CIRCLE_LButtonUp		( UINT nFlags, CPoint point	)
{
	ReleaseCapture();

	if( m_bPreviewPress )
	{
		m_bPreviewPress	= FALSE;

		if( m_RectRef.PtInRect( point ) )
		{
			// 원래값 복구하고..
			m_nR	= m_nRBackup;
			m_nG	= m_nGBackup;
			m_nB	= m_nBBackup;
			rgb_to_hsv	( m_nR , m_nG , m_nB , & m_fH , & m_fS , & m_fV	);
			DrawSVTable	( &m_MemDC2 , m_RectVBox						);
			Invalidate	( FALSE											);

			// RGB를 입력받음..

			CHSVControlRGBInputDlg dlg;
			dlg.m_nR	= m_nR;
			dlg.m_nG	= m_nG;
			dlg.m_nB	= m_nB;

			if( IDOK == dlg.DoModal() )
			{
				m_nR	= dlg.m_nR;
				m_nG	= dlg.m_nG;
				m_nB	= dlg.m_nB;

				rgb_to_hsv	( m_nR , m_nG , m_nB , & m_fH , & m_fS , & m_fV	);
				DrawSVTable	( &m_MemDC2 , m_RectVBox						);
				Invalidate	( FALSE											);
			}
		}
		else
		{
			// Do nothing...
			Invalidate	( FALSE											);
		}
	}
	else
	{
		m_nPosition = POS_NONE;
	}
}

void CHSVControlStatic::CIRCLE_MouseMove		( UINT nFlags, CPoint point	)
{
	if( m_bPreviewPress )
	{
		SetCursor( m_hCursor );

		if( m_RectRef.PtInRect( point ) )
		{
			// 원래값 복구하고..
			m_nR	= m_nRBackup;
			m_nG	= m_nGBackup;
			m_nB	= m_nBBackup;
			rgb_to_hsv	( m_nR , m_nG , m_nB , & m_fH , & m_fS , & m_fV	);
			DrawSVTable	( &m_MemDC2 , m_RectVBox						);
			Invalidate	( FALSE											);
		}
		else
		{
			// 칼라값을 읽어와서 설정..
			// 마우스가 움직이는 동안 칼라키를 표시해주는 작업.
			ClientToScreen( &point );
			HDC			hdc = ::GetDC	( NULL						);
			COLORREF	ref = ::GetPixel( hdc , point.x , point.y	);

			::ReleaseDC( NULL , hdc );

			m_nR	= GetRValue( ref );
			m_nG	= GetGValue( ref );
			m_nB	= GetBValue( ref );

			rgb_to_hsv	( m_nR , m_nG , m_nB , & m_fH , & m_fS , & m_fV	);
			DrawSVTable	( &m_MemDC2 , m_RectVBox						);
			Invalidate	( FALSE											);
		}
	}
	else
	switch( m_nPosition )
	{
	case POS_HUE	:
		{
			// 반지름 계산.
			double	radius;

			radius = sqrt
			(
				double
				(
					( point.x - HSV_CIRCLE_CENTER_X ) * ( point.x - HSV_CIRCLE_CENTER_X ) +
					( point.y - HSV_CIRCLE_CENTER_Y ) * ( point.y - HSV_CIRCLE_CENTER_Y )
				)
			);


			double theta;

			double x2 , y2;
			x2 = ( double ) ( point.x - HSV_CIRCLE_CENTER_X );
			y2 = ( double ) ( point.y - HSV_CIRCLE_CENTER_Y );

			if( x2 >= 0.0 )
			{
				// 0보다 크면..
				theta = acos( -y2 / radius );
			}
			else
			{
				// 0보다 작으면..
				theta = acos( y2 / radius );
				theta += 3.1415927;
			}
			// Hue 값을 구한다.

			m_fH = ( float ) ( + 360.0 / ( 2.0 * 3.1415927 ) * ( theta + 55.0 * 2.0 * 3.1415927 / 360.0 ) );
			while( m_fH > 360.0f	) m_fH -= 360.0f;
			while( m_fH < 0.0f		) m_fH += 360.0f;

			DrawSVTable( &m_MemDC2 , m_RectVBox );
			
			hsv_to_rgb( m_fH , m_fS , m_fV , &m_nR , &m_nG , &m_nB );
			Invalidate( FALSE );
			GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );
		}
		break;
	case POS_SATVAL	:
		{
			m_fS = ( float ) ( point.x - m_RectVBox.left	) / ( float ) ( m_RectVBox.right	-	m_RectVBox.left		);
			m_fV = ( float ) ( point.y - m_RectVBox.top		) / ( float ) ( m_RectVBox.bottom	-	m_RectVBox.top		);

			if	( m_fS > 1.0f	) m_fS = 1.0f;
			if	( m_fS < 0.0f	) m_fS = 0.0f;
			if	( m_fV > 1.0f	) m_fV = 1.0f;
			if	( m_fV < 0.0f	) m_fV = 0.0f;

			hsv_to_rgb( m_fH , m_fS , m_fV , &m_nR , &m_nG , &m_nB );
			Invalidate( FALSE );

			GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );
		}
		break;
	}
	
}

void	CHSVControlStatic::SetRGB( int r , int g , int b		)
{
	// 범위체크는 따로 하지 않음.
	m_nR = r ; m_nG = g ; m_nB = b;
	rgb_to_hsv( m_nR , m_nG , m_nB , &m_fH , &m_fS , &m_fV );

	switch( m_nType )
	{
	case BOXTYPE	:
	case ALPHATYPE	:
		{
			Invalidate	( FALSE						);
		}
		break;
	case CIRCLETYPE	:
		{
			DrawSVTable	( &m_MemDC2 , m_RectVBox	);
			Invalidate	( FALSE						);
		}
		break;
	}	

	GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );
}

void	CHSVControlStatic::SetHSV( float h , float s , float v	)
{
	// 범위체크는 따로 하지 않음.
	m_fH = h ; m_fS = s ; m_fV = v;
	hsv_to_rgb( m_fH , m_fS , m_fV , &m_nR , &m_nG , &m_nB );

	switch( m_nType )
	{
	case BOXTYPE	:
	case ALPHATYPE	:
		{
			Invalidate	( FALSE						);
		}
		break;
	case CIRCLETYPE	:
		{
			DrawSVTable	( &m_MemDC2 , m_RectVBox	);
			Invalidate	( FALSE						);
		}
		break;
	}
	
	GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );
}

void	CHSVControlStatic::ALPHA_LButtonDown	( UINT nFlags, CPoint point	)
{
	CRect	rect;
	GetClientRect( rect );
	if( rect.PtInRect( point ) )
	{
		m_fV = ( float ) ( point.y - rect.top ) / ( float ) ( rect.bottom	-	rect.top		);

		hsv_to_rgb( m_fH , m_fS , m_fV , &m_nR , &m_nG , &m_nB );
		Invalidate( FALSE );
		GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );

		m_bLButtonDown = TRUE;
		SetCapture();
	}
}
void	CHSVControlStatic::ALPHA_LButtonUp		( UINT nFlags, CPoint point	)
{
	if( m_bLButtonDown )
	{
		m_bLButtonDown = FALSE;
		ReleaseCapture();
		Invalidate( FALSE );
	}
}

void	CHSVControlStatic::ALPHA_MouseMove		( UINT nFlags, CPoint point	)
{
	if( m_bLButtonDown )
	{
		CRect	rect;
		GetClientRect( rect );

		if( point.y < rect.top		) point.y = rect.top	;
		if( point.y > rect.bottom	) point.y = rect.bottom	;

		m_fV = ( float ) ( point.y - rect.top ) / ( float ) ( rect.bottom	-	rect.top		);

		hsv_to_rgb( m_fH , m_fS , m_fV , &m_nR , &m_nG , &m_nB );
		Invalidate( FALSE );
		GetParent()->PostMessage( WM_HSVCONTROL_NOTIFY , GetDlgCtrlID() , RGB( m_nR , m_nG , m_nB ) );

		SetCapture();
	}
}

void CHSVControlStatic::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CHSVControlRGBInputDlg dlg;
	dlg.m_nR	= m_nR;
	dlg.m_nG	= m_nG;
	dlg.m_nB	= m_nB;

	dlg.m_nType	= m_nType;

	if( IDOK == dlg.DoModal() )
	{
		m_nR	= dlg.m_nR;
		m_nG	= dlg.m_nG;
		m_nB	= dlg.m_nB;

		rgb_to_hsv	( m_nR , m_nG , m_nB , & m_fH , & m_fS , & m_fV	);
		DrawSVTable	( &m_MemDC2 , m_RectVBox						);
		Invalidate	( FALSE											);
	}
	
	CStatic::OnLButtonDblClk(nFlags, point);
}


/* hsv_to_rgb:
*  Converts from HSV colorspace to RGB values.
*/
void hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b)
{
	float f, x, y, z;
	int i;
	
	v *= 255.0f;
	
	if (s == 0.0f)
	{
		*r = *g = *b = (int)v;
	}
	else
	{
		while (h < 0)
			h += 360.0f;
		h = ( float )( fmod(h, 360.0f) / 60.0f );
		i = (int)h;
		f = h - i;
		x = v * (1.0f - s);
		y = v * (1.0f - (s * f));
		z = v * (1.0f - (s * (1.0f - f)));
		
		switch (i) {
		case 0: *r = ( int ) v; *g = ( int ) z; *b = ( int ) x; break;
		case 1: *r = ( int ) y; *g = ( int ) v; *b = ( int ) x; break;
		case 2: *r = ( int ) x; *g = ( int ) v; *b = ( int ) z; break;
		case 3: *r = ( int ) x; *g = ( int ) y; *b = ( int ) v; break;
		case 4: *r = ( int ) z; *g = ( int ) x; *b = ( int ) v; break;
		case 5: *r = ( int ) v; *g = ( int ) x; *b = ( int ) y; break;
		}
	}
}



//rgb_to_hsv:
/// Converts an RGB value into the HSV colorspace.

void rgb_to_hsv(int r, int g, int b, float *h, float *s, float *v)
{
	float min, max, delta, rc, gc, bc;
	
	rc = (float)r / 255.0f;
	gc = (float)g / 255.0f;
	bc = (float)b / 255.0f;

	if( gc < bc )	max = bc;
	else			max = gc;
	if( max < rc )	max = rc;

	if( gc < bc )	min = gc;
	else			min = bc;
	if( rc < min )	min = rc;

	delta = max - min;
	*v = max;
	
	if (max != 0.0)
		*s = delta / max;
	else
		*s = 0.0;
	
	if (*s == 0.0) {
		*h = 0.0; 
	}
	else {
		if (rc == max)
			*h = (gc - bc) / delta;
		else if (gc == max)
			*h = 2 + (bc - rc) / delta;
		else if (bc == max)
			*h = 4 + (rc - gc) / delta;
		
		*h *= 60.0;
		if (*h < 0)
			*h += 360.0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHSVControlRGBInputDlg dialog


CHSVControlRGBInputDlg::CHSVControlRGBInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHSVControlRGBInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHSVControlRGBInputDlg)
	m_nR = 0;
	m_nG = 0;
	m_nB = 0;
	//}}AFX_DATA_INIT

	m_nType	= CHSVControlStatic::BOXTYPE;
}


void CHSVControlRGBInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHSVControlRGBInputDlg)
	DDX_Text(pDX, IDC_R, m_nR);
	DDV_MinMaxInt(pDX, m_nR, 0, 255);
	DDX_Text(pDX, IDC_G, m_nG);
	DDV_MinMaxInt(pDX, m_nG, 0, 255);
	DDX_Text(pDX, IDC_B, m_nB);
	DDV_MinMaxInt(pDX, m_nB, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHSVControlRGBInputDlg, CDialog)
	//{{AFX_MSG_MAP(CHSVControlRGBInputDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHSVControlRGBInputDlg message handlers

BOOL CHSVControlRGBInputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	switch( m_nType	)
	{
	case CHSVControlStatic::ALPHATYPE:
		{
			GetDlgItem( IDC_R_TEXT )->SetWindowText( "A" );
			GetDlgItem( IDC_G_TEXT )->ShowWindow( SW_HIDE );
			GetDlgItem( IDC_B_TEXT )->ShowWindow( SW_HIDE );

			GetDlgItem( IDC_G )->MoveWindow( 0 , 0 , 0 , 0 );
			GetDlgItem( IDC_B )->MoveWindow( 0 , 0 , 0 , 0 );
		}
		break;
	default:
		break;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHSVControlRGBInputDlg::OnOK() 
{
	UpdateData( TRUE );

	switch( m_nType	)
	{
	case CHSVControlStatic::ALPHATYPE:
		{
			m_nG = m_nB = m_nR;
			UpdateData( FALSE );
		}
		break;
	default:
		break;
	}
	
	CDialog::OnOK();
}
