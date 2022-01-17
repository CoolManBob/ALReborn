// TimeSelectStatic.cpp : implementation file
//

#include "stdafx.h"
#include "TimeSelectStatic.h"
#include "ApBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeSelectStatic

CTimeSelectStatic::CTimeSelectStatic()
{
	for( int i = 0 ; i < ASkySetting::TS_MAX ; ++ i )
		m_abTime[ i ] = FALSE;

	m_abTime[ 0 ] = TRUE;

	m_bLButtonDown	= FALSE	;
	m_nLButtonTime	= 0		;
}

CTimeSelectStatic::~CTimeSelectStatic()
{
}


BEGIN_MESSAGE_MAP(CTimeSelectStatic, CStatic)
	//{{AFX_MSG_MAP(CTimeSelectStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeSelectStatic message handlers

void CTimeSelectStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect	rect	;
	CRect	rect2	;
	CString	str		;

	GetClientRect( rect );

	rect.SetRect( 0 , 0 , rect.Width() , TSS_TIME_HEIGHT * ASkySetting::TS_MAX );

	dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );

	dc.SetTextColor( RGB( 255 , 255 , 255 ) );
	dc.SetBkMode( TRANSPARENT );

	for( int i = 0 ; i < ASkySetting::TS_MAX ; i ++ )
	{
		rect2.SetRect( 0 , TSS_TIME_HEIGHT * i , rect.Width() , TSS_TIME_HEIGHT * ( i + 1 ) );

		switch( i )
		{
		case ASkySetting::TS_NIGHT2		: str = "00~04"	; break;
		case ASkySetting::TS_DAWN		: str = "05~06"	; break;
		case ASkySetting::TS_DAYTIME1	: str = "07~12"	; break;
		case ASkySetting::TS_DAYTIME2	: str = "13~18"	; break;
		case ASkySetting::TS_EVENING	: str = "19~20"	; break;
		case ASkySetting::TS_NIGHT1		: str = "21~24"	; break;
		}

		if( m_abTime[ i ] )
		{
			dc.FillSolidRect( rect2 , RGB( 64 , 84 , 187 ) );
		}

		dc.DrawText( str , rect2 , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}
	
	
	// Do not call CStatic::OnPaint() for painting messages
}

void CTimeSelectStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int index = point.y / TSS_TIME_HEIGHT;

	if( index < ASkySetting::TS_MAX && index >= 0)
	{
		short nShiftState = GetAsyncKeyState( VK_SHIFT );
		if( nShiftState < 0 )
		{
			if( m_nLButtonTime == index ) SetTime( m_nLButtonTime );
			else
			if( m_nLButtonTime < index )
			{
				SetTime( m_nLButtonTime );
				for( int i = m_nLButtonTime ; i <= index ; ++i )
				{
					AdditionalSetTime( i , TRUE );
				}
			}
			else
			if( m_nLButtonTime > index )
			{
				SetTime( index );
				for( int i = index ; i <= m_nLButtonTime ; ++i )
				{
					AdditionalSetTime( i , TRUE );
				}
			}
		}
		else
		{
			SetTime( index );

			SetCapture();
			m_bLButtonDown	= TRUE	;
			m_nLButtonTime	= index	;
		}
	}
	else
	{
		TRACE( "CTimeSelectStatic::OnLButtonDown 범위에서 벗어남\n" );
	}	
	
	CStatic::OnLButtonDown(nFlags, point);
}

INT32	CTimeSelectStatic::SetTime( INT32	time	)	// 시간을 지정함..
{
	ASSERT( 0 <= time && time < ASkySetting::TS_MAX );

	INT32	prevtime = GetTime();

	// 먼저 메시지를 보낸다.
	GetParent()->SendMessage( WM_TIMESELECT_EVENT , prevtime , time );

	for( int i = 0 ; i < ASkySetting::TS_MAX ; ++ i ) m_abTime[ i ] = FALSE;

	m_abTime[ time ] = TRUE;

	Invalidate();
	

	return time;
}

void CTimeSelectStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_bLButtonDown = FALSE;
	ReleaseCapture();
	CStatic::OnLButtonUp(nFlags, point);
}

void CTimeSelectStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bLButtonDown )
	{
		int index = point.y / TSS_TIME_HEIGHT;

		if( index < ASkySetting::TS_MAX && index >= 0)
		{

			if( m_nLButtonTime == index ) SetTime( m_nLButtonTime );
			else
			if( m_nLButtonTime < index )
			{
				SetTime( m_nLButtonTime );
				for( int i = m_nLButtonTime ; i <= index ; ++i )
				{
					AdditionalSetTime( i , TRUE );
				}
			}
			else
			if( m_nLButtonTime > index )
			{
				SetTime( index );
				for( int i = index ; i <= m_nLButtonTime ; ++i )
				{
					AdditionalSetTime( i , TRUE );
				}
			}
		}
	}
	CStatic::OnMouseMove(nFlags, point);
}

INT32	CTimeSelectStatic::AdditionalSetTime( INT32	time , BOOL bSelect	)	// 셀렉션 추가..
{
	ASSERT( 0 <= time );
	ASSERT( time < ASkySetting::TS_MAX );

	m_abTime[ time ] = bSelect;

	Invalidate( FALSE );

	return m_abTime[ time ];

}

INT32	CTimeSelectStatic::GetSelectCount()
{
	INT32	nCount = 0;
	for( int i = 0 ; i < ASkySetting::TS_MAX ; ++ i )
	{
		if( m_abTime[ i ] )
			nCount++;
	}

	return nCount;
}

INT32	CTimeSelectStatic::GetTime( INT32 nOffset )
{
	INT32	nCount = 0;
	for( int i = 0 ; i < ASkySetting::TS_MAX ; ++ i )
	{
		if( m_abTime[ i ] )
		{
			if( nCount == nOffset )
				return i;
			nCount++;
		}		
	}

	// 요까지 오면 안됌

	ASSERT( !"요까지 오면 이상한것임둥!\n" );

	return 0;
}
