// RTMenuDistance.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "RegionToolDlg.h"
#include "RegionMenuDlg.h"
#include "RTMenuDistance.h"
#include "RTMenuBlock.h"

// CRTMenuBlock

IMPLEMENT_DYNAMIC(CRTMenuBlock, CWnd)
CRTMenuBlock::CRTMenuBlock():
	m_bLButtonDownMap( FALSE ),
	m_nBrushSize( 20 ),
	m_bUnsavedData( FALSE )
{
}

CRTMenuBlock::~CRTMenuBlock()
{
}


BEGIN_MESSAGE_MAP(CRTMenuBlock, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

char *	CRTMenuBlock::GetMenuName()
{
	static char _strName[] = "Blocking";
	return ( char * ) _strName;
}

BOOL	CRTMenuBlock::Create( CRect * pRect , CWnd * pParent )
{
	return CWnd::Create( NULL , NULL , WS_CHILD | WS_VISIBLE , *pRect , pParent , 1346 );
}

// CRTMenuBlock 메시지 처리기입니다.
/*
BOOL CRTMenuBlock::OnLButtonDownMap	( CPoint &point , int x , int z )
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
*/

BOOL CRTMenuBlock::OnLButtonDownMap	( CPoint &point , int x , int z )
{
	m_bLButtonDownMap = TRUE;

	AuPOS			posClick		;

	posClick.x	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( x );
	posClick.z	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( z );
	posClick.y	= 0.0f;

	BOOL	bControl = GetAsyncKeyState( VK_CONTROL ) < 0;

	// 클릭한 범위를 체크해서
	// 적용 디비전을 골라내고
	// 원형으로 이미지 적용함.

	CRegionViewStatic::DivisionImageData *	pDivision;
	vector <CRegionViewStatic::DivisionImageData *>::iterator IterDivision;

	CRect	rectBrush;
	rectBrush.SetRect(	point.x - m_nBrushSize , point.y - m_nBrushSize ,
						point.x + m_nBrushSize , point.y + m_nBrushSize );

	CDC	memDC;
	CBitmap	memBitmap;

	CDC * pDC = GetDC();
	memDC.CreateCompatibleDC( pDC );
	memBitmap.CreateCompatibleBitmap( pDC , rectBrush.Width() , rectBrush.Height() );
	ReleaseDC( pDC );
	memDC.SelectObject( memBitmap );

	CBrush	brush;
	CPen	pen;
	brush.CreateSolidBrush( RGB( 0 , 0 , 255 ) );
	pen.CreatePen( PS_SOLID , 0 , RGB(0 , 0 , 255 ) );

	memDC.SelectObject( brush );
	memDC.SelectObject( pen );

	CBrush	brushBlack;
	CPen	penBlack;
	brushBlack.CreateSolidBrush( RGB( 0 , 0 , 0 ) );
	penBlack.CreatePen( PS_SOLID , 0 , RGB(0 , 0 , 0 ) );


	memDC.FillSolidRect( 0 , 0 , rectBrush.Width() , rectBrush.Height() , RGB( 0 , 0 , 0 ) );
	memDC.Ellipse( 0 , 0 , rectBrush.Width() , rectBrush.Height() );

	for (
		IterDivision = this->m_pParent->m_pMainDlg->m_ctlRegion.m_vecDisplayedDivision.begin( ) ;
		IterDivision != this->m_pParent->m_pMainDlg->m_ctlRegion.m_vecDisplayedDivision.end( ) ;
		IterDivision++ )
	{
		pDivision = (*IterDivision);
		switch( pDivision->eViewState )
		{
		case DI_OUTSIDE:
			break;
		case DI_OVERLAP:
		case DI_INSIDE:
			{
				// 화면에 표시돼는 디비젼.

				CRect	rectMap;
				rectMap.left	= pDivision->nStartX;
				rectMap.top		= pDivision->nStartY;
				rectMap.right	= pDivision->nStartX + 512;
				rectMap.bottom	= pDivision->nStartY + 512;

				if( rectBrush.right > rectMap.left && rectMap.right > rectBrush.left &&
					rectBrush.bottom > rectMap.top && rectMap.bottom > rectBrush.top &&
					!pDivision->bmpSkyBlocking.IsEmpty() )
				{
					// 범위 포함..
					// 
					CDC * pDC = CDC::FromHandle( pDivision->bmpSkyBlocking.GetDC() );

					int	nLocalX,nLocalY;
					nLocalX = point.x - pDivision->nStartX;
					nLocalY = point.y - pDivision->nStartY;

					CRect	rectLocal = rectBrush;
					rectLocal.left		-= pDivision->nStartX;
					rectLocal.top		-= pDivision->nStartY;
					rectLocal.right		-= pDivision->nStartX;
					rectLocal.bottom	-= pDivision->nStartY;

					//pDC->Ellipse( rectLocal );//, RGB( 0 , 0 , 255 ) );

					if( bControl )
					{
						pDC->SelectObject( brushBlack );
						pDC->SelectObject( penBlack );

						pDC->Ellipse( rectLocal );
					}
					else
					{
						pDC->BitBlt( rectLocal.left , rectLocal.top , rectLocal.Width() , rectLocal.Height() ,
							& memDC , 0 , 0 , SRCPAINT );
					}

					// 플래그 업데이트.
					// 하드코딩 -.-;;
					pDivision->uFlag	|= DUF_BLOCK;
					m_bUnsavedData		= TRUE;
				}
			}
			break;
		}
	}

	m_pParent->InvalidateRegionView();
	m_pParent->SetCaptureRegionView();

	return TRUE;
}
BOOL CRTMenuBlock::OnLButtonUpMap	( CPoint &point , int x , int z )
{
	m_pParent->ReleaseCaptureRegionView();
	m_bLButtonDownMap = FALSE;
	return TRUE;
} 
BOOL CRTMenuBlock::OnMouseMoveMap	( CPoint &point , int x , int z )
{
	if( m_bLButtonDownMap )
	{
		// LButton Dragging Event
		BOOL	bControl = GetAsyncKeyState( VK_CONTROL ) < 0;

		// 클릭한 범위를 체크해서
		// 적용 디비전을 골라내고
		// 원형으로 이미지 적용함.

		CRegionViewStatic::DivisionImageData *	pDivision;
		vector <CRegionViewStatic::DivisionImageData *>::iterator IterDivision;

		CRect	rectBrush;
		rectBrush.SetRect(	point.x - m_nBrushSize , point.y - m_nBrushSize ,
							point.x + m_nBrushSize , point.y + m_nBrushSize );

		CDC	memDC;
		CBitmap	memBitmap;

		CDC * pDC = GetDC();
		memDC.CreateCompatibleDC( pDC );
		memBitmap.CreateCompatibleBitmap( pDC , rectBrush.Width() , rectBrush.Height() );
		ReleaseDC( pDC );
		memDC.SelectObject( memBitmap );

		CBrush	brush;
		CPen	pen;
		brush.CreateSolidBrush( RGB( 0 , 0 , 255 ) );
		pen.CreatePen( PS_SOLID , 0 , RGB(0 , 0 , 255 ) );

		memDC.SelectObject( brush );
		memDC.SelectObject( pen );

		CBrush	brushBlack;
		CPen	penBlack;
		brushBlack.CreateSolidBrush( RGB( 0 , 0 , 0 ) );
		penBlack.CreatePen( PS_SOLID , 0 , RGB(0 , 0 , 0 ) );


		memDC.FillSolidRect( 0 , 0 , rectBrush.Width() , rectBrush.Height() , RGB( 0 , 0 , 0 ) );
		memDC.Ellipse( 0 , 0 , rectBrush.Width() , rectBrush.Height() );

		for (
			IterDivision = this->m_pParent->m_pMainDlg->m_ctlRegion.m_vecDisplayedDivision.begin( ) ;
			IterDivision != this->m_pParent->m_pMainDlg->m_ctlRegion.m_vecDisplayedDivision.end( ) ;
			IterDivision++ )
		{
			pDivision = (*IterDivision);
			switch( pDivision->eViewState )
			{
			case DI_OUTSIDE:
				break;
			case DI_OVERLAP:
			case DI_INSIDE:
				{
					// 화면에 표시돼는 디비젼.

					CRect	rectMap;
					rectMap.left	= pDivision->nStartX;
					rectMap.top		= pDivision->nStartY;
					rectMap.right	= pDivision->nStartX + 512;
					rectMap.bottom	= pDivision->nStartY + 512;

					if( rectBrush.right > rectMap.left && rectMap.right > rectBrush.left &&
						rectBrush.bottom > rectMap.top && rectMap.bottom > rectBrush.top &&
						!pDivision->bmpSkyBlocking.IsEmpty() )
					{
						// 범위 포함..
						// 
						CDC * pDC = CDC::FromHandle( pDivision->bmpSkyBlocking.GetDC() );

						int	nLocalX,nLocalY;
						nLocalX = point.x - pDivision->nStartX;
						nLocalY = point.y - pDivision->nStartY;

						CRect	rectLocal = rectBrush;
						rectLocal.left		-= pDivision->nStartX;
						rectLocal.top		-= pDivision->nStartY;
						rectLocal.right		-= pDivision->nStartX;
						rectLocal.bottom	-= pDivision->nStartY;

						//pDC->Ellipse( rectLocal );//, RGB( 0 , 0 , 255 ) );

						if( bControl )
						{
							pDC->SelectObject( brushBlack );
							pDC->SelectObject( penBlack );

							pDC->Ellipse( rectLocal );
						}
						else
						{
							pDC->BitBlt( rectLocal.left , rectLocal.top , rectLocal.Width() , rectLocal.Height() ,
								& memDC , 0 , 0 , SRCPAINT );
						}

						// 플래그 업데이트.
						// 하드코딩 -.-;;
						pDivision->uFlag	|= DUF_BLOCK;
						m_bUnsavedData		= TRUE;
					}
				}
				break;
			}
		}
		m_pParent->InvalidateRegionView();
	}
	return TRUE;
}

void CRTMenuBlock::OnPaint()
{
	CPaintDC dc(this); // device context for painting


	CRect	rect;
	GetClientRect( rect );

	dc.FillSolidRect( rect , RGB( 0 , 0 , 64 ) );

	// Brush 표시
	CRect	rectBrush;

	rectBrush.left		= rect.CenterPoint().x - m_nBrushSize;
	rectBrush.right		= rect.CenterPoint().x + m_nBrushSize;
	rectBrush.top		= rect.CenterPoint().y - m_nBrushSize;
	rectBrush.bottom	= rect.CenterPoint().y + m_nBrushSize;

	CBrush	brushPen;
	brushPen.CreateSolidBrush( RGB( 255 , 255 , 255 ) );
	dc.SelectObject( brushPen );
	dc.Ellipse( rectBrush );

	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB( 255 , 0 , 0 ) );
	
	CRect	rectText;
	rectText.left	= 0;
	rectText.right	= rect.right;
	rectText.top	= rect.CenterPoint().y - 20;
	rectText.bottom	= rect.CenterPoint().y + 20;

	dc.DrawText( "요따시만함" , rectText , DT_CENTER | DT_VCENTER | DT_SINGLELINE );

	dc.TextOut( 0 , 0 , "Shift 로 블러킹표시" );
	dc.TextOut( 0 , 20 , "Control로 지우개" );	
}

void CRTMenuBlock::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 중점에서의 거리구함.
	CRect	rect;
	GetClientRect( rect );

	// 중점과의 거리
	float fLength = ( float )	( ( rect.CenterPoint().x - point.x ) * ( rect.CenterPoint().x - point.x ) +
								( rect.CenterPoint().y - point.y ) * ( rect.CenterPoint().y - point.y ) );
	fLength = ( float ) sqrt( fLength );
	m_nBrushSize = ( int ) fLength;

	Invalidate();
	// m_pParent->InvalidateRegionView();

	CRTMenuBase::OnLButtonDown(nFlags, point);
}

BOOL CRTMenuBlock::OnSaveData()
{
	INT32	nCount = 0;
	{
		int nDivisionX , nDivisionZ;
		int nDivisionIndex;
		for( nDivisionZ = 0 ; nDivisionZ < MAP_WORLD_INDEX_HEIGHT / MAP_DEFAULT_DEPTH ; nDivisionZ ++ )
		{
			for( nDivisionX = 0 ; nDivisionX < MAP_WORLD_INDEX_WIDTH / MAP_DEFAULT_DEPTH ; nDivisionX ++ )
			{
				CRegionViewStatic::DivisionImageData *	pDivision = &this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ];

				if( pDivision->uFlag & DUF_BLOCK )
				{
					TRACE( "CRTMenuSky::OnSaveData Division %04d 녀석이 변경됐음\n", MakeDivisionIndex( nDivisionX , nDivisionZ ) );
				}
				else
				{
					continue;
				}

				nDivisionIndex = MakeDivisionIndex( nDivisionX , nDivisionZ );

				// 저장.
				CString strSkyBlock;
				strSkyBlock.Format( "map\\data\\blocking\\sb%02d%02d.bmp" , nDivisionX , nDivisionZ );
				CBmp	bmp;
				bmp.Create( 256 , 256 );

				::StretchBlt( bmp.GetDC() ,
					0 , 0 , 
					256 ,
					256 ,
					pDivision->bmpSkyBlocking.GetDC() , 
					0 , 0 , 
					512 , 512 , 
					SRCCOPY );

				bmp.Save( (LPSTR) (LPCTSTR) strSkyBlock );

				// 언세이브 데이타 플래그 제거..
				pDivision->uFlag &= ~DUF_BLOCK;

				nCount++;
			}
		}
		
		m_bUnsavedData = FALSE;
		// 확인사살..
		this->m_pParent->m_pMainDlg->m_ctlRegion.ClearUnsavedDataCheck( DUF_BLOCK );
	}

	if( nCount )
	{
		CString	str;
		str.Format( "%d 개의 스카이 블러킹 정보를 저장하였습니다." , nCount );
		MessageBox( str );
	}
	return TRUE;
}

BOOL CRTMenuBlock::OnQuerySaveData()
{
	return m_bUnsavedData;
}

void CRTMenuBlock::OnActiveRegionMenu		()
{
	this->m_pParent->m_pMainDlg->m_ctlRegion.SetDisplayMode( CRegionViewStatic::SKYBLOCK );
}

void CRTMenuBlock::OnDeActiveRegionMenu		()
{
	this->m_pParent->m_pMainDlg->m_ctlRegion.SetDisplayMode( CRegionViewStatic::DEFAULT );
}
