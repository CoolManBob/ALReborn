// PortionStatic.cpp : implementation file
//

#include "stdafx.h"
//#include "hsvcontroltest.h"
#include "PortionStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define	LEFT_OFFSET		20
#define	ELEMENT_MARGIN	40
#define	ELEMENT_WIDTH	5
#define	BOTTOM_OFFSET	20

/////////////////////////////////////////////////////////////////////////////
// CPortionStatic

COLORREF	g_ColorTable[ PORTIONSTATIC_MAX_ELEMENT ] =
{
	RGB( 255 , 255 , 255 )	,
	RGB( 219 , 184 , 101 )	,
	RGB( 229 , 121 , 113 )	,
	RGB( 229 , 113 , 211 )	,
	RGB( 132 , 113 , 229 )	,

	RGB( 113 , 192 , 229 )	,
	RGB( 113 , 229 , 165 )	,
	RGB( 157 , 229 , 113 )	,
	RGB( 225 , 229 , 113 )	,
	RGB( 255 , 255 , 0	)	
};

CPortionStatic::CPortionStatic() : m_nElement( 0 )
{
}

CPortionStatic::~CPortionStatic()
{
}


BEGIN_MESSAGE_MAP(CPortionStatic, CStatic)
	//{{AFX_MSG_MAP(CPortionStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPortionStatic message handlers

void CPortionStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect , rectBar;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );

	INT32	nHeight	;
	CPortionStatic::Element	* pElement;

	for( int i = 0 ; i < GetCount() ; ++ i )
	{
		pElement	= GetElement( i );

		if( pElement->nRate == -1 )
			// ÆÛ¸Ó³ÍÆ® ¿¤¸®¸ÕÆ®..
			nHeight = rect.Height() - BOTTOM_OFFSET;
		else
			nHeight		= ( INT32 ) ( ( FLOAT ) ( rect.Height() - BOTTOM_OFFSET ) *
						( ( FLOAT ) pElement->nRate / 100.0f ) );
		rectBar.SetRect( LEFT_OFFSET + ELEMENT_MARGIN * i , rect.Height() - BOTTOM_OFFSET - nHeight , 
			LEFT_OFFSET + ELEMENT_MARGIN * i + ELEMENT_WIDTH , rect.Height() - BOTTOM_OFFSET );

		if( pElement->nRate == -1 )
			dc.FillSolidRect( rectBar , RGB( 92 , 92 , 92 ) );
		else
			dc.FillSolidRect( rectBar , g_ColorTable[ i ] );

		dc.TextOut( ELEMENT_MARGIN * i , rect.Height() - BOTTOM_OFFSET , pElement->strName );
	}
	
	// Do not call CStatic::OnPaint() for painting messages
}

BOOL		CPortionStatic::AddElement( CString strName , INT32 nRate )
{
	ASSERT( m_nElement < PORTIONSTATIC_MAX_ELEMENT );
	if( m_nElement >= PORTIONSTATIC_MAX_ELEMENT )
	{
		return FALSE;
	}

	m_aElement[ m_nElement ].strName	= strName	;
	m_aElement[ m_nElement ].nRate		= nRate		;
	m_nElement++;

	return TRUE;
}

CPortionStatic::Element *	CPortionStatic::GetElement( INT32 nIndex )
{
	if( nIndex < 0 || nIndex >= m_nElement ) return NULL;
	else return m_aElement + nIndex ;
}

void CPortionStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	INT32	nIndex = point.x / ELEMENT_MARGIN;
	if( nIndex >= 0 && nIndex < GetCount() )
	{
		m_nSelectedElement	= nIndex;
		CRect	rect;

		GetClientRect( rect );

		if( point.y < 0								) point.y = 0;
		if( point.y > rect.Height() - BOTTOM_OFFSET	) point.y = rect.Height() - BOTTOM_OFFSET;

		INT32	nRate = ( INT32 ) ( 100.0f * ( FLOAT ) ( rect.Height() - BOTTOM_OFFSET - point.y ) / 
		 ( FLOAT ) ( rect.Height() - BOTTOM_OFFSET ) );


		CPortionStatic::Element	* pElement = GetElement( nIndex );

		if( pElement->nRate >= 0 )
		{
			pElement->nRate = nRate;

			BOOL	bExit = FALSE;
			while( 100 < GetPortionTotal() )
			{
				for ( int i = 0 ; i < GetCount() ; i ++ )
				{
					pElement = GetElement( i );

					if( pElement->nRate >= 1 && i != nIndex)
					{
						pElement->nRate -= 1;
						if( 100 >= GetPortionTotal() )
						{
							bExit = TRUE;
							break;
						}
					}
				}

				if( bExit ) break;
			}	

			Invalidate( FALSE );
		}

		GetParent()->PostMessage( WM_PORTIONL_NOTIFY );
		SetCapture();
	}
	else
		m_nSelectedElement = -1;
	
	CStatic::OnLButtonDown(nFlags, point);
}

INT32	CPortionStatic::GetPortionTotal()
{
	INT32	nRateTotal = 0;
	CPortionStatic::Element	* pElement;

	for ( int i = 0 ; i < GetCount() ; i ++ )
	{
		pElement = GetElement( i );

		if( pElement->nRate >= 0 )
			nRateTotal += pElement->nRate;
	}
	
	return nRateTotal;
}


void CPortionStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_nSelectedElement = -1;
	Invalidate( FALSE );
	ReleaseCapture();
	
	GetParent()->PostMessage( WM_PORTIONL_NOTIFY );
	CStatic::OnLButtonUp(nFlags, point);
}

void CPortionStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_nSelectedElement >= 0 && m_nSelectedElement < GetCount() )
	{
		CRect	rect;

		GetClientRect( rect );

		if( point.y < 0								) point.y = 0;
		if( point.y > rect.Height() - BOTTOM_OFFSET	) point.y = rect.Height() - BOTTOM_OFFSET;

		INT32	nRate = ( INT32 ) ( 100.0f * ( FLOAT ) ( rect.Height() - BOTTOM_OFFSET - point.y ) / 
		 ( FLOAT ) ( rect.Height() - BOTTOM_OFFSET ) );


		CPortionStatic::Element	* pElement = GetElement( m_nSelectedElement );

		if( pElement->nRate >= 0 )
		{
			pElement->nRate = nRate;

			BOOL	bExit = FALSE;
			while( 100 < GetPortionTotal() )
			{
				for ( int i = 0 ; i < GetCount() ; i ++ )
				{
					pElement = GetElement( i );

					if( pElement->nRate >= 1 && i != m_nSelectedElement)
					{
						pElement->nRate -= 1;
						if( 100 >= GetPortionTotal() )
						{
							bExit = TRUE;
							break;
						}
					}
				}

				if( bExit ) break;
			}	
		}

		Invalidate( FALSE );
		GetParent()->PostMessage( WM_PORTIONL_NOTIFY );
	}
	
	CStatic::OnMouseMove(nFlags, point);
}

INT32	CPortionStatic::GetTemporaryTotal()
{
	INT32	nCount = 0;
	CPortionStatic::Element	* pElement;

	for ( int i = 0 ; i < GetCount() ; i ++ )
	{
		pElement = GetElement( i );

		if( pElement->nRate >= 0 )
			nCount ++;
	}
	
	return nCount;
}

void CPortionStatic::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	INT32	nIndex = point.x / ELEMENT_MARGIN;
	if( nIndex >= 0 && nIndex < GetCount() )
	{
		m_nSelectedElement	= nIndex;
		CRect	rect;

		GetClientRect( rect );

		CPortionStatic::Element	* pElement = GetElement( nIndex );

		if( pElement->nRate	>= 0 )	pElement->nRate	= -1;
		else						pElement->nRate = 0	;

		Invalidate( FALSE );
		SetCapture();
		GetParent()->PostMessage( WM_PORTIONL_NOTIFY );
	}
	else
		m_nSelectedElement = -1;
	
	
	CStatic::OnLButtonDblClk(nFlags, point);
}
