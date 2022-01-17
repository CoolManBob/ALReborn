// RegionViewStatic.cpp : implementation file
//

#include "stdafx.h"
#include "RegionTool.h"
#include "RegionViewStatic.h"
#include "RegionToolDlg.h"

#include "TeleportDlg.h"
#include "RTMenuRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_COLOR_SAMPLE 25
COLORREF	g_ColorSample[ MAX_COLOR_SAMPLE ] =
{
	RGB( 255 , 0 , 0 ),
	RGB( 0 , 255 , 0 ),
	RGB( 0 , 0 , 255 ),
	RGB( 64 , 128 , 0 ),
	RGB( 0 , 192 , 64 ),

	RGB( 64 , 128 , 64 ),
	RGB( 255 , 64 , 0 ),
	RGB( 128 , 64 , 64 ),
	RGB( 192 , 64 , 128 ),
	RGB( 255 , 64 , 256 ),

	RGB( 255 , 128 , 0 ),
	RGB( 128 , 255 , 0 ),
	RGB( 192 , 128 , 64 ),
	RGB( 128 , 128 , 255 ),
	RGB( 128 , 0 , 128 ),

    RGB( 240 , 204 , 105 ),
	RGB( 250 , 204 , 206 ),
	RGB( 192 , 250 , 37 ),
	RGB( 212 , 212 , 245 ),
	RGB( 4 , 253 , 191 ),

	RGB( 255 , 169 , 3 ),
	RGB( 252 , 110 , 130 ),
	RGB( 179 , 139 , 202 ),
	RGB( 1 , 254 , 233 ),
	RGB( 225 , 228 , 222 )
};

COLORREF __GetColor( int nColor ) { return g_ColorSample[ nColor % MAX_COLOR_SAMPLE ]; }


/////////////////////////////////////////////////////////////////////////////
// CRegionViewStatic

CRegionViewStatic::CRegionViewStatic() : m_eDisplayMode( DEFAULT )
{
	m_nOffsetX	= 0;
	m_nOffsetZ	= 0;
	m_bRbuttonDown		= FALSE;
	m_fScale	= 1.0f;

	for( int i = 0 ; i < MAX_REGION_BITMAP_CASH ; ++ i )
	{
		m_pBitmapCash[ i ] = NULL;
	}

	m_bDivisionGrid		= TRUE	;
	m_nDivisionInRender	= 0		;
}

CRegionViewStatic::~CRegionViewStatic()
{
}


BEGIN_MESSAGE_MAP(CRegionViewStatic, CStatic)
	//{{AFX_MSG_MAP(CRegionViewStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionViewStatic message handlers

void CRegionViewStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPoint pointOriginal = point;
	point.x -= m_nOffsetX;
	point.y -= m_nOffsetZ;

	if( (	FALSE == g_bTeleportOnlyMode			&&
			GetAsyncKeyState( VK_CONTROL	) < 0	&&
			GetAsyncKeyState( VK_MENU		) < 0	&&
			GetAsyncKeyState( VK_SHIFT		) < 0
		)											||
			g_bTeleportOnlyMode						)
	{
		#define WM_ALEFPOSITIONWARP	(WM_USER + 17 )

		// 이동 모드..
		AuPOS			posClick		;

		posClick.x	= REGION_OFFSET_TO_POSITION( point.x );
		posClick.z	= REGION_OFFSET_TO_POSITION( point.y );
		posClick.y	= 0.0f;

		m_posLastClick = posClick;

		Invalidate( FALSE );

		char	str[ 1024 ];
		sprintf( str , "%f , %f 다용 \n" , posClick.x , posClick.z );
		TRACE( str );

		HWND	hWnd;
		hWnd = ::FindWindow( NULL , "AlefClient" );

		if( hWnd )
		{
			WPARAM	wParam = *( ( INT32 * ) &posClick.x );
			LPARAM	lParam = *( ( INT32 * ) &posClick.z );

			::PostMessage( hWnd , WM_ALEFPOSITIONWARP , wParam , lParam );

			static BOOL _sbShowMessageBox = TRUE;

			if( _sbShowMessageBox )
			{
				CTeleportDlg dlg;
				sprintf( str , "(%.0f , %.0f) (Div%04d) 위치로 송신 완료\n" , posClick.x , posClick.z , GetDivisionIndexF( posClick.x , posClick.z ) );
				dlg.m_strMessage = str;
				dlg.DoModal();

				_sbShowMessageBox = dlg.m_bShow;
			}
		}
		else
		{
			sprintf( str , "클라이언트가 안떠있어요~(%f , %f)\n" , posClick.x , posClick.z );
			MessageBox( str );
		}
	}
	else
	{
		CRegionToolDlg * pParent = ( CRegionToolDlg * ) GetParent();
		if( pParent->m_pRegionMenu->m_vectorMenuWnd[ pParent->m_pRegionMenu->m_nCurrentMenu ].pMenu )
		{
			pParent->m_pRegionMenu->m_vectorMenuWnd[ pParent->m_pRegionMenu->m_nCurrentMenu ].pMenu->OnLButtonDownMap( pointOriginal , point.x , point.y );
		}
	}


	CStatic::OnLButtonDown(nFlags, point);
}

void CRegionViewStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CPoint pointOriginal = point;
	point.x -= m_nOffsetX;
	point.y -= m_nOffsetZ;

	CRegionToolDlg * pParent = ( CRegionToolDlg * ) GetParent();
	if( pParent->m_pRegionMenu->m_vectorMenuWnd[ pParent->m_pRegionMenu->m_nCurrentMenu ].pMenu )
	{
		pParent->m_pRegionMenu->m_vectorMenuWnd[ pParent->m_pRegionMenu->m_nCurrentMenu ].pMenu->OnLButtonUpMap( pointOriginal , point.x , point.y );
	}
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CRegionViewStatic::OnRButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	m_bRbuttonDown		= TRUE	;
	m_pointLastPress	= point	;
	//m_pSelectedElement	= NULL	;
	
	CStatic::OnRButtonDown(nFlags, point);
}

void CRegionViewStatic::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_bRbuttonDown = FALSE;
	
	CStatic::OnRButtonUp(nFlags, point);
}

void CRegionViewStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bRbuttonDown )
	{
		// 위치이동..
		int	dx , dz;

		dx	= m_pointLastPress.x - point.x ;
		dz	= m_pointLastPress.y - point.y ;

		// 이동 처리..

		m_nOffsetX	-= dx;
		m_nOffsetZ	-= dz;
		
		m_pointLastPress = point;

		Invalidate( FALSE );
	}

	CPoint pointOriginal = point;
	point.x -= m_nOffsetX;
	point.y -= m_nOffsetZ;

	CRegionToolDlg * pParent = ( CRegionToolDlg * ) GetParent();
	if( pParent->m_pRegionMenu->m_vectorMenuWnd[ pParent->m_pRegionMenu->m_nCurrentMenu ].pMenu )
	{
		pParent->m_pRegionMenu->m_vectorMenuWnd[ pParent->m_pRegionMenu->m_nCurrentMenu ].pMenu->OnMouseMoveMap( pointOriginal , point .x , point.y );
	}
	
	CStatic::OnMouseMove(nFlags, point);
}

void CRegionViewStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	UpdateMemDC();

	CRect	rect;
	GetClientRect( rect );
	m_memDC.FillSolidRect( rect , RGB( 0 , 64 , 0 ) );
	m_memDC.SetStretchBltMode( HALFTONE );

	DivisionImageData *	pDivisionStruct = NULL;
	CRect	rectMap;

	CPen	pen;
	pen.CreateStockObject( WHITE_PEN );
	m_memDC.SelectObject( pen );

	int	sx , sy , cx , cy , imx , imy ;

	m_nDivisionInRender = 0;

	BOOL bControl	= GetAsyncKeyState( VK_CONTROL	) < 0;
	BOOL bShift		= GetAsyncKeyState( VK_SHIFT	) < 0;

	m_vecDisplayedDivision.clear();

	for( int i = 17 ; i <= 32 ; ++i )
	{
		for( int j = 17 ; j <= 32 ; ++j )
		{
			rectMap.SetRect(
				( INT32 ) ( i * MAP_IMAGE_WIDTH * m_fScale ) + m_nOffsetX,
				( INT32 ) ( j * MAP_IMAGE_WIDTH * m_fScale ) + m_nOffsetZ,
				( INT32 ) ( i * MAP_IMAGE_WIDTH * m_fScale + MAP_IMAGE_WIDTH * m_fScale ) + m_nOffsetX ,
				( INT32 ) ( j * MAP_IMAGE_WIDTH * m_fScale + MAP_IMAGE_WIDTH * m_fScale ) + m_nOffsetZ);

			if ( rect.right > rectMap.left && rectMap.right > rect.left)
			{
				if (rect.bottom > rectMap.top && rectMap.bottom > rect.top )
				{
					// 화면에 포함된다..
					pDivisionStruct = GetMap( i , j );

					if( pDivisionStruct )
					{
						// 렌더 범위 넣기..
						if( rect.left > rectMap.left && rectMap.right > rect.right &&
							rect.top > rectMap.top && rectMap.bottom > rect.bottom )
						{
							pDivisionStruct->eViewState	= DI_INSIDE;
						}
						else
						{
							pDivisionStruct->eViewState	= DI_OVERLAP;
						}

						m_vecDisplayedDivision.push_back( pDivisionStruct );

						// 출력돼는 위치 저장.
						pDivisionStruct->nStartX	= rectMap.left;
						pDivisionStruct->nStartY	= rectMap.top;

						// 마고자 (2005-06-07 오후 6:00:59) : 
						// 인덱스 저장해둠..
						m_aDivisionInRender[ m_nDivisionInRender ].x	= i;
						m_aDivisionInRender[ m_nDivisionInRender ].y	= j;
						m_nDivisionInRender++;

						if( m_fScale == 1.0f )
						{
							cx = 512;
							cy = 512;

							if( rectMap.left < 0 )
							{
								sx = 0				;
								cx += rectMap.left	;
								imx = -rectMap.left	;
							}
							else
							{
								sx = rectMap.left	;
								imx = 0;
							}

							if( rectMap.top < 0 )
							{
								sy = 0				;
								cy += rectMap.top	;
								imy = -rectMap.top	;
							}
							else
							{
								sy = rectMap.top	;
								imy = 0				;
							}

							if( rect.right < rectMap.right )
							{
								cx -= rectMap.right - rect.right;
							}

							if( rect.bottom < rectMap.bottom )
							{
								cy -= rectMap.bottom - rect.bottom;
							}

//							if( pDivisionStruct->uLock )
//							{
//								m_memDC.FillSolidRect( sx , sy , cx , cy , RGB( 0 , 0 , 0 ) );
//							}
//							else
							{
								if( g_bTeleportOnlyMode )
								{
									LoadCompactData( i , j );
									if( bControl )
									{
										// 리젼 정보
										//pDivisionStruct->bmpRegion.Draw( m_memDC.GetSafeHdc() , sx , sy , cx , cy , imx , imy );
									}
									else if( bShift )
									{
										// 블러킹 정보
										pDivisionStruct->bmpBlocking.Draw( m_memDC.GetSafeHdc() , sx , sy , cx , cy , imx , imy );
									}
									else
									{
										// 그냥 지형 출력
										pDivisionStruct->bmp.Draw( m_memDC.GetSafeHdc() , sx , sy , cx , cy , imx , imy );
									}
								}
								else
								{
									switch( GetDisplayMode() )
									{
									default:
									case DEFAULT:
										if( bShift )
										{
											// 블러킹 정보
											pDivisionStruct->bmpBlocking.Draw( m_memDC.GetSafeHdc() , sx , sy , cx , cy , imx , imy );
										}
										else
										{
											pDivisionStruct->bmp.Draw( m_memDC.GetSafeHdc() , sx , sy , cx , cy , imx , imy );										
										}
										break;
									case SKYBLOCK:
										if( bShift )
										{
											// 블러킹 정보
											pDivisionStruct->bmpBlocking.Draw( m_memDC.GetSafeHdc() , sx , sy , cx , cy , imx , imy );
											pDivisionStruct->bmpSkyBlocking.Draw( m_memDC.GetSafeHdc() , sx , sy , cx , cy , imx , imy , SRCPAINT);
										}
										else
										{
											pDivisionStruct->bmp.Draw( m_memDC.GetSafeHdc() , sx , sy , cx , cy , imx , imy );										
											pDivisionStruct->bmpSkyBlocking.Draw( m_memDC.GetSafeHdc() , sx , sy , cx , cy , imx , imy , SRCPAINT);
										}
										break;
									}
									// 그냥 지형 출력
								}

								if( m_bDivisionGrid )
								{
									m_memDC.MoveTo( rectMap.right , rectMap.top		);
									m_memDC.LineTo( rectMap.left , rectMap.top		);
									m_memDC.LineTo( rectMap.left , rectMap.bottom	);
								}
							}
						}
						else
						{
							HDC	hDC = pDivisionStruct->bmp.GetDC();
							::StretchBlt( m_memDC.GetSafeHdc() ,
								rectMap.left , rectMap.top , 
								( INT32 ) ( pDivisionStruct->bmp.GetWidth() * m_fScale ) ,
								( INT32 ) ( pDivisionStruct->bmp.GetHeight() * m_fScale ) ,
								hDC , 
								0 , 0 , 
								pDivisionStruct->bmp.GetWidth() , pDivisionStruct->bmp.GetHeight() , 
								SRCCOPY );

							if( bControl && g_bTeleportOnlyMode )
							{
								LoadCompactData( i , j );
								/*
								hDC = pDivisionStruct->bmpRegion.GetDC();
								::StretchBlt( m_memDC.GetSafeHdc() ,
									rectMap.left , rectMap.top , 
									( INT32 ) ( pDivisionStruct->bmp.GetWidth() * m_fScale ) ,
									( INT32 ) ( pDivisionStruct->bmp.GetHeight() * m_fScale ) ,
									hDC , 
									0 , 0 , 
									pDivisionStruct->bmp.GetWidth() , pDivisionStruct->bmp.GetHeight() , 
									SRCCOPY );
								*/
							}

							if( m_bDivisionGrid )
							{
								m_memDC.MoveTo( rectMap.right , rectMap.top		);
								m_memDC.LineTo( rectMap.left , rectMap.top		);
								m_memDC.LineTo( rectMap.left , rectMap.bottom	);
							}
						}
					}
				}
			}
			////////////////////////////////////
		}
	}
	
	CRegionToolDlg * pParent = ( CRegionToolDlg * ) GetParent();
	if( pParent->m_pRegionMenu->m_vectorMenuWnd[ pParent->m_pRegionMenu->m_nCurrentMenu ].pMenu )
	{
		pParent->m_pRegionMenu->m_vectorMenuWnd[ pParent->m_pRegionMenu->m_nCurrentMenu ].pMenu->OnPaintOnMap( & m_memDC );
	}

	dc.BitBlt( 0 , 0 , rect.Width() , rect.Height() , &m_memDC , 0 , 0 , SRCCOPY );

	// 텔레포트할때 전에 텔레포트한 위치 표사ㅣ.
	{
		CBrush	brush;
		brush.CreateSolidBrush( RGB( 255 , 60 , 60 ) );
		dc.SelectObject( brush );

		const	int	cWidth = 10;
		CRect	rectPos;
		rectPos.left	= REGION_POSITION_TO_OFFSET( m_posLastClick.x ) - cWidth + m_nOffsetX;
		rectPos.top		= REGION_POSITION_TO_OFFSET( m_posLastClick.z ) - cWidth + m_nOffsetZ;
		rectPos.right	= rectPos.left	+ cWidth * 2;
		rectPos.bottom	= rectPos.top	+ cWidth * 2;

		dc.Ellipse( rectPos );
	}

	// Do not call CStatic::OnPaint() for painting messages
}

void	CRegionViewStatic::LoadCompactData( int x , int z )
{
	ASSERT( x < MAX_DIVISION );
	ASSERT( z < MAX_DIVISION );

	if( x >= MAX_DIVISION ) return;
	if( z >= MAX_DIVISION ) return;

	DivisionImageData	* pDivisionStruct = &m_bmpMap[ x ][ z ];

	if( pDivisionStruct->bRegionDataLoaded ) return;
	pDivisionStruct->bRegionDataLoaded = TRUE;

	// 추가 초기화 작업..
	
	// 마고자 (2005-06-07 오후 6:07:00) : 
	// 텔레포트 모드 일경우 컴팩트데이타를 이용하여
	// 리젼 정보를 비트맵으로 생성한다.

	//CBmp	*pBmpRegion	= &pDivisionStruct->bmpRegion	;
	CBmp	*pBmpBlock	= &pDivisionStruct->bmpBlocking	;

	//pBmpRegion->Create( 512 , 512 );
	pBmpBlock->Create( 512 , 512 );

	int	nSectorStartX , nSectorStartZ;

	nSectorStartX = GetFirstSectorXInDivision( MakeDivisionIndex( x , z ) );
	nSectorStartZ = GetFirstSectorZInDivision( MakeDivisionIndex( x , z ) );

	int	nSegmentX , nSegmentZ;

	CDC	*pDcBmp;// = CDC::FromHandle( pBmpRegion->GetDC() );

	int	nPixX , nPixZ;

	int	nSectorWidthPixel = 2 * MAP_DEFAULT_DEPTH;

	ApCompactSegment	*pSegment;

	int nSectorX , nSectorZ;

	// Region
	for( nSectorZ = nSectorStartZ ; nSectorZ < nSectorStartZ + MAP_DEFAULT_DEPTH ; nSectorZ ++ )
	{
		for( nSectorX = nSectorStartX ; nSectorX < nSectorStartX + MAP_DEFAULT_DEPTH ; nSectorX ++ )
		{
			// 시작 픽셀 좌표..
			nPixX = ( nSectorX - nSectorStartX ) * nSectorWidthPixel;
			nPixZ = ( nSectorZ - nSectorStartZ ) * nSectorWidthPixel;

			ApWorldSector * pSector;

			pSector = g_pcsApmMap->GetSectorByArray( nSectorX , 0 , nSectorZ );
			if( pSector )
			{
				INT32	nRegion;
				for( nSegmentZ = 0 ; nSegmentZ < MAP_DEFAULT_DEPTH ; nSegmentZ++ )
				{
					for( nSegmentX = 0 ; nSegmentX < MAP_DEFAULT_DEPTH ; nSegmentX++ )
					{
						pSegment = pSector->C_GetSegment( nSegmentX , nSegmentZ );
						if( pSegment )
						{
							nRegion = pSegment->GetRegion();
						}
						else
						{
							nRegion = 0;
						}

						// 픽셀정보 채움..
						COLORREF color = __GetColor( nRegion );
						//pDcBmp->FillSolidRect( nPixX + nSegmentX * 2 , nPixZ + nSegmentZ * 2 , 2 , 2 , color );
					}
				}
			}
			else
			{
				// 정보가 없으면 그부분을 검은색으로 채움..

				//pDcBmp->FillSolidRect( nPixX , nPixZ , nSectorWidthPixel , nSectorWidthPixel , RGB( 0 , 0 , 0 ) );
			}

		}
	}

	pDcBmp = CDC::FromHandle( pBmpBlock->GetDC() );

	// Blocking 표시..
	for( nSectorZ = nSectorStartZ ; nSectorZ < nSectorStartZ + MAP_DEFAULT_DEPTH ; nSectorZ ++ )
	{
		for( nSectorX = nSectorStartX ; nSectorX < nSectorStartX + MAP_DEFAULT_DEPTH ; nSectorX ++ )
		{
			// 시작 픽셀 좌표..
			nPixX = ( nSectorX - nSectorStartX ) * nSectorWidthPixel;
			nPixZ = ( nSectorZ - nSectorStartZ ) * nSectorWidthPixel;

			ApWorldSector * pSector;

			pSector = g_pcsApmMap->GetSectorByArray( nSectorX , 0 , nSectorZ );
			if( pSector )
			{
				INT32	nRegion;
				for( nSegmentZ = 0 ; nSegmentZ < MAP_DEFAULT_DEPTH ; nSegmentZ++ )
				{
					for( nSegmentX = 0 ; nSegmentX < MAP_DEFAULT_DEPTH ; nSegmentX++ )
					{
						pSegment = pSector->C_GetSegment( nSegmentX , nSegmentZ );
						if( pSegment )
						{
							nRegion = pSegment->stTileInfo.geometryblock;
						}
						else
						{
							nRegion = 0;
						}

						// 픽셀정보 채움..
						//COLORREF color = RGB( 63 * nRegion , 0 , 0 );
						COLORREF color = RGB( ( nRegion ) ? 255 : 0  , 0 , 0 );
						
						pDcBmp->FillSolidRect( nPixX + nSegmentX * 2 , nPixZ + nSegmentZ * 2 , 2 , 2 , color );
					}
				}
			}
			else
			{
				// 정보가 없으면 그부분을 검은색으로 채움..

				pDcBmp->FillSolidRect( nPixX , nPixZ , nSectorWidthPixel , nSectorWidthPixel , RGB( 0 , 0 , 0 ) );
			}

		}
	}
}


CRegionViewStatic::DivisionImageData *	CRegionViewStatic::GetMap( int x , int z , BOOL bImageLoad )
{
	ASSERT( x < MAX_DIVISION );
	ASSERT( z < MAX_DIVISION );

	if( x >= MAX_DIVISION ) return NULL;
	if( z >= MAX_DIVISION ) return NULL;

	DivisionImageData	* pDivisionStruct = &m_bmpMap[ x ][ z ];

	if( !bImageLoad )  return pDivisionStruct;

	if( pDivisionStruct->bmp.IsEmpty() )
	{
		// 없으면 로드함..
		CString	strFilename;
		strFilename.Format( "RegionTool\\MP%02d%02d.bmp" , x , z );

		if( pDivisionStruct->bmp.Load( ( LPSTR ) ( LPCTSTR ) strFilename , NULL) )
		{
			// 성공
			
			// 블러킹 이미지 읽음..
			CString	strFilenameBlocking;
			strFilenameBlocking.Format( "map\\data\\blocking\\gb%02d%02d.bmp" , x , z );

			CBmp	bmp;
			std::string	strError;
			if( bmp.Load( ( LPSTR ) ( LPCTSTR ) strFilenameBlocking , & strError ) )
			{
				pDivisionStruct->bmpBlocking.Create( MAP_IMAGE_WIDTH , MAP_IMAGE_WIDTH );

				//CDC * pDC = CDC::FromHandle( pDivisionStruct->bmp.GetDC() );

				HDC	hDC = bmp.GetDC();
				::StretchBlt( pDivisionStruct->bmpBlocking.GetDC() ,
					0 , 0 , 
					512 ,
					512 ,
					hDC , 
					0 , 0 , 
					bmp.GetWidth() , bmp.GetHeight() , 
					SRCCOPY );

				if( pDivisionStruct->uFlag & DUF_BLOCK &&
					!pDivisionStruct->bmpSkyBlocking.IsEmpty() )
				{
					// 플래그가 있으면 로딩 따로 하지 않음.
				}
				else
				{
					CString strSkyBlock;
					strSkyBlock.Format( "map\\data\\blocking\\sb%02d%02d.bmp" , x , z );

					CBmp	bmpSky;
					if( bmpSky.Load( ( LPSTR ) ( LPCTSTR ) strSkyBlock , &strError) )
					{
						pDivisionStruct->bmpSkyBlocking.Create( MAP_IMAGE_WIDTH , MAP_IMAGE_WIDTH );
						HDC	hDC = bmpSky.GetDC();
						::StretchBlt( pDivisionStruct->bmpSkyBlocking.GetDC() ,
							0 , 0 , 
							512 ,
							512 ,
							hDC , 
							0 , 0 , 
							bmpSky.GetWidth() , bmpSky.GetHeight() , 
							SRCCOPY );
					}
					else
					{
						pDivisionStruct->bmpSkyBlocking.Create( MAP_IMAGE_WIDTH , MAP_IMAGE_WIDTH );
						CDC * pDC = CDC::FromHandle( pDivisionStruct->bmpSkyBlocking.GetDC() );
						pDC->FillSolidRect( 0 , 0 , 512 , 512 , RGB( 0 , 0 , 0 ) );
					}
				}
			}
			else
			{
				// 없는경우는 더미 이미지 생성..
				pDivisionStruct->bmpBlocking.Create( MAP_IMAGE_WIDTH , MAP_IMAGE_WIDTH );

				CDC * pDC = CDC::FromHandle( pDivisionStruct->bmpBlocking.GetDC() );

				ASSERT( NULL != pDC );

				CRect	rect;
				rect.SetRect( 0 , 0 , 512 , 512 );

				pDC->FillSolidRect( 0 , 0 , 512 , 512 , RGB( 255 , 0 , 0 ) );
				pDC->FillSolidRect( 10 , 10 , 492 , 492 , RGB( 128 , 128 , 128 ) );

				pDC->DrawText( strFilenameBlocking , rect , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
				rect.top += 20;
				rect.bottom += 20;
				pDC->DrawText( strError.c_str() , rect , DT_CENTER | DT_VCENTER | DT_SINGLELINE );

				// 스카이 블로킹 비트맵
				pDivisionStruct->bmpSkyBlocking.Create( MAP_IMAGE_WIDTH , MAP_IMAGE_WIDTH );
				pDC = CDC::FromHandle( pDivisionStruct->bmpSkyBlocking.GetDC() );
				pDC->FillSolidRect( 0 , 0 , 512 , 512 , RGB( 0 , 0 , 0 ) );
			}
		}
		else
		{
			// 없는경우는 더미 이미지 생성..
			pDivisionStruct->bmp.Create( MAP_IMAGE_WIDTH , MAP_IMAGE_WIDTH );

			CDC * pDC = CDC::FromHandle( pDivisionStruct->bmp.GetDC() );

			ASSERT( NULL != pDC );

			CRect	rect;
			rect.SetRect( 0 , 0 , 512 , 512 );

			pDC->FillSolidRect( 0 , 0 , 512 , 512 , RGB( 0 , 0 , 0 ) );
			pDC->FillSolidRect( 10 , 10 , 492 , 492 , RGB( 128 , 128 , 128 ) );

			pDC->DrawText( strFilename , rect , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		}

		// 캐싱..
		int i;
		for( i = 0 ; i < MAX_REGION_BITMAP_CASH ; ++ i )
		{
			if( m_pBitmapCash[ i ] == pDivisionStruct ) break;
		}

		if( i >= MAX_REGION_BITMAP_CASH )
		{
			// 맨마지막꺼 디스트로이..
			if( m_pBitmapCash[ MAX_REGION_BITMAP_CASH - 1 ] )
			{
				m_pBitmapCash[ MAX_REGION_BITMAP_CASH - 1 ]->Release();
				m_pBitmapCash[ MAX_REGION_BITMAP_CASH - 1 ] = NULL;
			}

			i = MAX_REGION_BITMAP_CASH - 1;
		}

		for( int j = i ; j > 0 ; --j )
		{
			m_pBitmapCash[ j ] = m_pBitmapCash[ j - 1 ];
		}

		m_pBitmapCash[ 0 ] = pDivisionStruct;

		return pDivisionStruct;
	}
	else
	{
		int i;
		// 캐싱..
		for( i = 0 ; i < MAX_REGION_BITMAP_CASH ; ++ i )
		{
			if( m_pBitmapCash[ i ] == pDivisionStruct ) break;
		}

		if( i == MAX_REGION_BITMAP_CASH )
		{
			// 맨마지막꺼 디스트로이..
			if( m_pBitmapCash[ MAX_REGION_BITMAP_CASH - 1 ] )
			{
				m_pBitmapCash[ MAX_REGION_BITMAP_CASH - 1 ]->Release();
				m_pBitmapCash[ MAX_REGION_BITMAP_CASH - 1 ] = NULL;
			}
		}

		for( int j = i ; j > 0 ; --j )
		{
			m_pBitmapCash[ j ] = m_pBitmapCash[ j - 1 ];
		}

		m_pBitmapCash[ 0 ] = pDivisionStruct;

		return pDivisionStruct;
	}
}

void	CRegionViewStatic::MoveTo( int nDivision )
{
	int	xIndex , zIndex;
	xIndex = nDivision / 100;
	zIndex = nDivision % 100;

	m_nOffsetX = ( INT32 ) ( -xIndex * MAP_IMAGE_WIDTH * m_fScale );
	m_nOffsetZ = ( INT32 ) ( -zIndex * MAP_IMAGE_WIDTH * m_fScale );

	CRect	rect;
	GetClientRect( rect );
	m_nOffsetX = m_nOffsetX + ( rect.Width	() / 2 - ( INT32 ) ( ( FLOAT ) ( MAP_IMAGE_WIDTH / 2 ) * m_fScale ) );
	m_nOffsetZ = m_nOffsetZ + ( rect.Height	() / 2 - ( INT32 ) ( ( FLOAT ) ( MAP_IMAGE_WIDTH / 2 ) * m_fScale ) );

	Invalidate( FALSE );
}

BOOL	CRegionViewStatic::IsUpdateMemDC()	// 출력 싸이즈가 변경되었는지 확인한다.
{
	CRect	rect;
	GetClientRect( rect );

	if( rect.Width()	== m_memRect.Width()	&&
		rect.Height()	== m_memRect.Height()	)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void	CRegionViewStatic::UpdateMemDC()		// 메모리 DC 재생성.
{
	if( IsUpdateMemDC() )
	{
		m_memBitmap.DeleteObject();
		m_memDC.DeleteDC();

		CDC * pDC = GetDC();

		GetClientRect( m_memRect );

		m_memDC.CreateCompatibleDC( pDC );
		m_memBitmap.CreateCompatibleBitmap( pDC , m_memRect.Width() , m_memRect.Height() );
		m_memDC.SelectObject( m_memBitmap );
	}
}

void CRegionViewStatic::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CPoint pointOriginal = point;
	point.x -= m_nOffsetX;
	point.y -= m_nOffsetZ;

	CRegionToolDlg * pParent = ( CRegionToolDlg * ) GetParent();
	if( pParent->m_pRegionMenu->m_vectorMenuWnd[ pParent->m_pRegionMenu->m_nCurrentMenu ].pMenu )
	{
		pParent->m_pRegionMenu->m_vectorMenuWnd[ pParent->m_pRegionMenu->m_nCurrentMenu ].pMenu->OnLButtonDblDownMap( pointOriginal , point.x , point.y );
	}
	
	CStatic::OnLButtonDblClk(nFlags, point);
}

void	CRegionViewStatic::UnsavedDataCheck( UINT32 uDivisionIndex , UINT32 uFlag )
{
	UINT32	uDivisionX , uDivisionZ;
	uDivisionX = GetDivisionXIndex( uDivisionIndex );
	uDivisionZ = GetDivisionZIndex( uDivisionIndex );

	ASSERT( uDivisionX < MAX_DIVISION );
	ASSERT( uDivisionZ < MAX_DIVISION );

	DivisionImageData * pDivisionStruct = &m_bmpMap[ uDivisionX ][ uDivisionZ ];

	// 플래그 업데이트.
	pDivisionStruct->uFlag	|= uFlag;
}

void	CRegionViewStatic::ClearUnsavedDataCheck( UINT32 uFlag )
{
	for( int i = 0 ; i < MAX_DIVISION ; ++i )
	{
		for( int j = 0 ; j < MAX_DIVISION ; ++j )
		{
			m_bmpMap[ i ][ j ].uFlag &= ~uFlag;
		}
	}
}

void	CRegionViewStatic::SetAllUnsavedDataCheck( UINT32 uFlag )
{
	for( int i = 0 ; i < MAX_DIVISION ; ++i )
	{
		for( int j = 0 ; j < MAX_DIVISION ; ++j )
		{
			m_bmpMap[ i ][ j ].uFlag |= uFlag;
		}
	}
}

void	CRegionViewStatic::SetZoom( FLOAT fZoom )
{
	if( fZoom >= 0.5f )
	{
		CRect	rect;
		GetClientRect( rect );

		m_nOffsetX -= rect.Width	() / 2;
		m_nOffsetZ -= rect.Height	() / 2;

		m_nOffsetX = ( INT32 ) ( ( FLOAT ) m_nOffsetX / m_fScale * fZoom);
		m_nOffsetZ = ( INT32 ) ( ( FLOAT ) m_nOffsetZ / m_fScale * fZoom);

		m_nOffsetX += rect.Width	() / 2;
		m_nOffsetZ += rect.Height	() / 2;

		m_fScale = fZoom;
		Invalidate( FALSE );
	}
}

void	CRegionViewStatic::ClearAllIndex()
{
	DivisionImageData *	pDivisionStruct = NULL;
	for( int i = 17 ; i <= 32 ; ++i )
	{
		for( int j = 17 ; j <= 32 ; ++j )
		{
			pDivisionStruct = GetMap( i , j , FALSE );

			if( pDivisionStruct )
			{
				pDivisionStruct->m_vecIndex.clear();
			}
		}
	}
}