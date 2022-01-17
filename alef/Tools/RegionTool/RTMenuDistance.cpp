// RTMenuDistance.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "RegionToolDlg.h"
#include "RegionMenuDlg.h"
#include "RTMenuDistance.h"

// CRTMenuDistance

IMPLEMENT_DYNAMIC(CRTMenuDistance, CWnd)
CRTMenuDistance::CRTMenuDistance()
{
	m_fTime	= 0.0f;
	m_fDistance	= 0.0f;
}

CRTMenuDistance::~CRTMenuDistance()
{
}


BEGIN_MESSAGE_MAP(CRTMenuDistance, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

char *	CRTMenuDistance::GetMenuName()
{
	static char _strName[] = "Distance";
	return ( char * ) _strName;
}

BOOL	CRTMenuDistance::Create( CRect * pRect , CWnd * pParent )
{
	return CWnd::Create( NULL , NULL , WS_CHILD | WS_VISIBLE , *pRect , pParent , 1346 );
}

// CRTMenuDistance 메시지 처리기입니다.

BOOL CRTMenuDistance::OnLButtonDownMap	( CPoint &point , int x , int z )
{
	AuPOS			posClick		;

	posClick.x	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( x );
	posClick.z	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( z );
	posClick.y	= 0.0f;

	m_vectorPoint.push_back( posClick );

	// 거리계산..
	{
		m_fDistance = 0.0f;
		AuPOS	* pLastPoint = &m_vectorPoint[ 0 ];
		for( int i = 1 ; i < ( int ) m_vectorPoint.size() ; i++ )
		{
			m_fDistance += AUPOS_DISTANCE_XZ( (*pLastPoint) , m_vectorPoint[ i ] );
			pLastPoint = &m_vectorPoint[ i ];
		}

		// 시간 계산..
		// 초속 6000 기준.
		m_fTime = m_fDistance / ( 6000.0f / 10 ) ;
	}
	
	Invalidate( FALSE );
	m_pParent->InvalidateRegionView();

	return TRUE;
}

void CRTMenuDistance::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	dc.SetTextColor( RGB( 255 , 255 , 255 ) );

	CRect	rect;
	GetClientRect( rect );

	dc.FillSolidRect( rect , RGB( 64 , 64 , 64 ) );

	dc.SetBkMode( TRANSPARENT );
	
	CString	str;
	str.Format( "거리 %.0fm" , m_fDistance / 100.0f );
	dc.TextOut( 0 , 0 , str );

	INT32	nSecond;
	nSecond = ( ( INT32 ) m_fTime ) % 60;

	str.Format( "시간 %d분 %d초" ,( INT32 ) m_fTime / 60 , nSecond );
	dc.TextOut( 0 , 20 , str );

	str.Format( "여기 클릭하면 리셋"  );
	dc.TextOut( 0 , 60 , str );
}

void CRTMenuDistance::OnPaintOnMap		( CDC * pDC )
{
	if( m_vectorPoint.size() <= 0 ) return;

	vector< POINT >	vectorPoint;
	POINT	point;

	for( int i = 0 ; i < ( int ) m_vectorPoint.size() ; i ++ )
	{
		point.x = m_pParent->m_pMainDlg->m_ctlRegion.REGION_POSITION_TO_OFFSET( m_vectorPoint[ i ].x )
						+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX;
		point.y = m_pParent->m_pMainDlg->m_ctlRegion.REGION_POSITION_TO_OFFSET( m_vectorPoint[ i ].z )
						+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ;

		vectorPoint.push_back( point );
	}

	const	int nTextBoxWidth	= 200;
	const	int nTextBoxHeight	= 100;

	int nCount = vectorPoint.size();

	//if( vectorPoint.size() <= 3 )
	{
		CPen	pen;
		pen.CreatePen( PS_SOLID , 3 , RGB( 255 , 255 , 255 ) );

		pDC->SelectObject( pen );

		pDC->MoveTo(	vectorPoint[ 0 ].x, vectorPoint[ 0 ].y );

		CRect	rect;
		rect.left	=
			vectorPoint[ 0 ].x
			+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX	- nTextBoxWidth / 2;
		rect.top	=
			vectorPoint[ 0 ].y
			+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ	- nTextBoxHeight * 0.7f;
		rect.right	= rect.left + nTextBoxWidth;
		rect.bottom	= rect.top + nTextBoxHeight;

		pDC->SetBkMode( TRANSPARENT );
		pDC->DrawText( "시작점" ,  rect , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		for( int i = 1 ; i < ( int ) m_vectorPoint.size() ; i ++ )
		{
			pDC->LineTo(	m_pParent->m_pMainDlg->m_ctlRegion.REGION_POSITION_TO_OFFSET( m_vectorPoint[ i ].x )
							+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX,
							m_pParent->m_pMainDlg->m_ctlRegion.REGION_POSITION_TO_OFFSET( m_vectorPoint[ i ].z )
							+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ );
		}
	}
/*
	else
	{
		// 베지어 곡선으로 그리는 코드..
		// 안쓰는게 좋겠음..

		CPen	pen;
		pen.CreatePen( PS_SOLID , 3 , RGB( 255 , 255 , 255 ) );

		pDC->SelectObject( pen );

		CRect	rect;
		rect.left	=
			vectorPoint[ 0 ].x
			+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX	- nTextBoxWidth / 2;
		rect.top	=
			vectorPoint[ 0 ].y
			+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ	- nTextBoxHeight * 0.7f;
		rect.right	= rect.left + nTextBoxWidth;
		rect.bottom	= rect.top + nTextBoxHeight;

		pDC->SetBkMode( TRANSPARENT );
		pDC->DrawText( "시작점" ,  rect , DT_CENTER | DT_VCENTER | DT_SINGLELINE );

		int nRemain		= ( vectorPoint.size() - 1 ) % 3;
		int	nLineCount	= vectorPoint.size() - nRemain;
		pDC->PolyBezier( &vectorPoint[ 0 ] , nLineCount );

		if( nRemain )
		{
			pDC->MoveTo(	vectorPoint[ nLineCount - 1 ].x, vectorPoint[ nLineCount - 1 ].y );
			for( int i = nLineCount; i < ( int ) m_vectorPoint.size() ; i ++ )
			{
				pDC->LineTo(	m_pParent->m_pMainDlg->m_ctlRegion.REGION_POSITION_TO_OFFSET( m_vectorPoint[ i ].x )
								+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX,
								m_pParent->m_pMainDlg->m_ctlRegion.REGION_POSITION_TO_OFFSET( m_vectorPoint[ i ].z )
								+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ );
			}
		}
	}
*/
}

void CRTMenuDistance::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 리셋
	m_fTime = 0.0f;
	m_fDistance = 0.0f;
	m_vectorPoint.clear();
	Invalidate();
	m_pParent->InvalidateRegionView();

	CRTMenuBase::OnLButtonDown(nFlags, point);
}
