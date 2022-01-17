// CPlugin_NoLayer.cpp: implementation of the CPlugin_NoLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "maptool.h"
#include "Plugin_NoLayer.h"
#include "MainWindow.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GEOMETRY_BLOCKING_OFFSET	( 50.0f )
#define OBJECT_BLOCKING_OFFSET		( 100.0f )

extern MainWindow			g_MainWindow	;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPlugin_NoLayer, CWnd)
	//{{AFX_MSG_MAP(CPlugin_NoLayer)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPlugin_NoLayer::CPlugin_NoLayer()
{
	m_pVertexBuffer		= NULL	;
	m_nVertexCount		= 0		;
	m_nTriangleCount	= 0		;
	
	// 옵션 설정.
	m_stOption.bShowSphere	= true;

	m_strShortName = "NoLayer";
}

CPlugin_NoLayer::~CPlugin_NoLayer()
{
	FreeVertexBuffer();
}

BOOL	CPlugin_NoLayer::AllocVertexBuffer	( INT32 nSize )
{
	ASSERT( nSize > 0 );

	// 혹시모르니 한방..
	FreeVertexBuffer();

	VERIFY( m_pVertexBuffer = new RwIm3DVertex[ nSize ] );

	if( m_pVertexBuffer )
	{
		m_nVertexCount	= nSize;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void	CPlugin_NoLayer::FreeVertexBuffer	()
{
	if( m_pVertexBuffer ) delete [] m_pVertexBuffer;

	m_pVertexBuffer		= NULL;
	m_nVertexCount		= 0;
	m_nTriangleCount	= 0;
}


BOOL CPlugin_NoLayer::OnSelectedPlugin	()
{
	m_bLButtonDown	= FALSE;
	VERIFY( AllocVertexBuffer( DEFAULT_VERTEX_BUFFER_SIZE ) );
	return TRUE;
}

BOOL CPlugin_NoLayer::OnDeSelectedPlugin	()
{
	FreeVertexBuffer();
	return TRUE;
}

BOOL CPlugin_NoLayer::OnWindowRender		()
{
	// 블러킹 정보 그리기..
	if( m_nTriangleCount )
	{
		// m_
		if( RwIm3DTransform( m_pVertexBuffer , m_nTriangleCount * 3 , NULL, rwIM3D_VERTEXRGBA ) )
		{                         
			RwIm3DRenderPrimitive( rwPRIMTYPETRILIST );
			RwIm3DEnd();
		}
	}

	return TRUE;
}

BOOL CPlugin_NoLayer::OnLButtonDownGeometry	( RwV3d * pPos )
{
	if( ISBUTTONDOWN( VK_MENU ) )
	{
		// 한개씩 찍기 모드
		if( g_MainWindow.m_pCurrentGeometry )
		{
			ApDetailSegment	*	pSelectedSegment	;
			ApWorldSector	*	pSector				;

			pSector = g_pcsApmMap->GetSector( pPos->x , pPos->z );

			if( NULL == pSector ) return TRUE;

			pSelectedSegment	= pSector->D_GetSegment( pPos->x , pPos->z );

			if( pSelectedSegment )
			{
				BOOL	bValue;

				if( ISBUTTONDOWN( VK_CONTROL ) )
					bValue = FALSE;
				else
					bValue = TRUE;
					// 정보수정..
				pSelectedSegment->stTileInfo.SetNoLayer( bValue );
				UpdateBlockingPolygon();

				SaveSetChangeMoonee();
				//SetSaveData();
			}
		}
	}
	else
	{
		// 브러시 모드
		m_bLButtonDown	= TRUE;
		return OnMouseMoveGeometry( pPos );
	}
	
	return TRUE;
}

BOOL CPlugin_NoLayer::OnLButtonUpGeometry	( RwV3d * pPos )
{
	m_bLButtonDown = FALSE;
	return TRUE;
}

BOOL CPlugin_NoLayer::OnMouseMoveGeometry	( RwV3d * pPos )
{
	BOOL	bValue;

	if( ISBUTTONDOWN( VK_CONTROL ) )
		bValue = FALSE;
	else
		bValue = TRUE;

					// 정보수정..
	if( m_bLButtonDown )
	{
		if( g_MainWindow.m_pCurrentGeometry )
		{
			if( g_MainWindow.m_pCurrentGeometry->GetCurrentDetail() == SECTOR_HIGHDETAIL )
			{
				SaveSetChangeMoonee();
				//SetSaveData();

				// 좌표에서 r 범위 안에 존재하는 섹터를 찾아낸다..
				// 브러시 적용..

				int				i , j				;
				float			fSegPosX, fSegPosY	;
				float			distance			;
				ApWorldSector *	pWorkingSector		;
				ApDetailSegment	* pSegment	;

				// 머티리얼 준비..

				int segmentx , segmentz;

				int x1 = ALEF_LOAD_RANGE_X1 , x2 = ALEF_LOAD_RANGE_X2 , z1 = ALEF_LOAD_RANGE_Y1 , z2 = ALEF_LOAD_RANGE_Y2;
				int	xc , zc;

				xc = g_MainWindow.m_pCurrentGeometry->GetArrayIndexX();
				zc = g_MainWindow.m_pCurrentGeometry->GetArrayIndexZ();

				x1 = xc - 1;
				z1 = zc - 1;
				x2 = xc + 1;
				z2 = zc + 1;
		
				// 그냥 지형 올리기..
				for( j = z1 ; j <= z2 ; j ++ )
				{
					for( i = x1 ; i <= x2 ; i ++ )
					{
						// 아자 찾아보자.
						pWorkingSector = g_pcsApmMap->GetSectorByArray(  i , 0 , j );
						if( NULL == pWorkingSector ) continue;

						if( NULL == pWorkingSector->GetNearSector( TD_NORTH	) ) continue;
						if( NULL == pWorkingSector->GetNearSector( TD_EAST	) ) continue;
						if( NULL == pWorkingSector->GetNearSector( TD_SOUTH	) ) continue;
						if( NULL == pWorkingSector->GetNearSector( TD_WEST	) ) continue;

						if( pWorkingSector->IsLoadedDetailData() &&
							AGCMMAP_THIS->IsInSectorRadius( g_pcsApmMap->GetSectorByArray( i , 0 , j ) ,
								pPos->x , pPos->z , g_pMainFrame->m_Document.m_fBrushRadius )
							)
						{
							// 아자.;; 
							// 이제 각각 세그먼트를 둘러서. 원 범위 내에 들어있는 세그먼트라면 Height를 적용한다.
							for( segmentz = 0 ; segmentz < pWorkingSector->D_GetDepth() ; segmentz ++ )
							{
								for( segmentx = 0 ; segmentx < pWorkingSector->D_GetDepth() ; segmentx ++ )
								{
									pSegment = pWorkingSector->D_GetSegment( segmentx , segmentz , &fSegPosX , &fSegPosY );
									// 에헤헤..
									if( pSegment )
									{
										if( ( distance = sqrt( ( fSegPosX - pPos->x ) * ( fSegPosX - pPos->x ) + ( fSegPosY - pPos->z ) * ( fSegPosY - pPos->z ) ) )
											> g_pMainFrame->m_Document.m_fBrushRadius ) continue;

										pSegment->stTileInfo.SetNoLayer( bValue );
									}

								}
							}

						}
					}
				}// 섹터 포 루프..
			}
		}//if( g_MainWindow.m_pCurrentGeometry )

		UpdateBlockingPolygon();
	}
	else
	{
		static ApWorldSector * s_pCurrentSector = NULL;
		if( s_pCurrentSector != g_MainWindow.m_pCurrentGeometry )
		{
			// 업데이트..
			s_pCurrentSector = g_MainWindow.m_pCurrentGeometry;
			UpdateBlockingPolygon();
		}
	}

	return TRUE;
}

void	CPlugin_NoLayer::UpdateBlockingPolygon()
{
	// 으헤헤;;
	m_nTriangleCount = 0;

	if( NULL == g_MainWindow.m_pCurrentGeometry ) return;

	int x1 = ALEF_LOAD_RANGE_X1 , x2 = ALEF_LOAD_RANGE_X2 , z1 = ALEF_LOAD_RANGE_Y1 , z2 = ALEF_LOAD_RANGE_Y2;
	int	xc , zc;

	xc = g_MainWindow.m_pCurrentGeometry->GetArrayIndexX();
	zc = g_MainWindow.m_pCurrentGeometry->GetArrayIndexZ();

	x1 = xc - 1 ;
	z1 = zc - 1 ;
	x2 = xc + 1 ;
	z2 = zc + 1 ;

	int					x , z;
	ApDetailSegment *	pSegment;
	FLOAT				fX , fZ;
	ApWorldSector	*	pSector;
	int					i , j ;

	for( j = z1 ; j <= z2 ; j ++ )
	{
		for( i = x1 ; i <= x2 ; i ++ )
		{
			pSector = g_pcsApmMap->GetSectorByArray(  i , 0 , j );

			if( NULL == pSector ) continue;

			if( NULL == pSector->GetNearSector( TD_NORTH	) ) continue;
			if( NULL == pSector->GetNearSector( TD_EAST		) ) continue;
			if( NULL == pSector->GetNearSector( TD_SOUTH	) ) continue;
			if( NULL == pSector->GetNearSector( TD_WEST		) ) continue;

			for( z = 0 ; z < MAP_DEFAULT_DEPTH ; z ++ )
			{
				for( x = 0 ; x < MAP_DEFAULT_DEPTH ; x ++ )
				{
					pSegment = pSector->D_GetSegment( x , z , &fX , &fZ );

					if( pSegment && pSegment->stTileInfo.IsNoLayer() ) // 여기서 블러킹 체크.
					{
						AddGeometryBlockingPolygon( pSector, x , z , fX , fZ , pSegment );
					}
				}	
			}
		}
	}
}

void	CPlugin_NoLayer::AddGeometryBlockingPolygon	( ApWorldSector	* pSector , int x , int z , FLOAT fX , FLOAT fZ , ApDetailSegment *	pSegment )
{
	if( NULL == g_MainWindow.m_pCurrentGeometry ) return;

	RwIm3DVertex *		pLineList;
	FLOAT				fHeight;

	pLineList	= m_pVertexBuffer + m_nTriangleCount * 3;

	RwIm3DVertexSetPos	( &pLineList[ 0 ] , fX , pSegment->height + GEOMETRY_BLOCKING_OFFSET , fZ	);
	RwIm3DVertexSetU	( &pLineList[ 0 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 0 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 255 , 0 , 0 , 128	);

	fHeight = pSector->D_GetHeight2( x + 1 , z ) + GEOMETRY_BLOCKING_OFFSET;
	RwIm3DVertexSetPos	( &pLineList[ 1 ] , fX + MAP_STEPSIZE , fHeight , fZ + 0	);
	RwIm3DVertexSetU	( &pLineList[ 1 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 1 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 1 ] , 255 , 0 , 0 , 128	);
	
	fHeight = pSector->D_GetHeight2( x , z + 1 ) + GEOMETRY_BLOCKING_OFFSET;
	RwIm3DVertexSetPos	( &pLineList[ 2 ] , fX + 0 , fHeight , fZ + MAP_STEPSIZE	);
	RwIm3DVertexSetU	( &pLineList[ 2 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 2 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 2 ] , 255 , 0 , 0 , 128	);
	
	fHeight = pSector->D_GetHeight2( x , z + 1 ) + GEOMETRY_BLOCKING_OFFSET;
	RwIm3DVertexSetPos	( &pLineList[ 3 ] , fX + 0 , fHeight , fZ + MAP_STEPSIZE	);
	RwIm3DVertexSetU	( &pLineList[ 3 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 3 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 3 ] , 255 , 0 , 0 , 128	);
	
	fHeight = pSector->D_GetHeight2( x + 1 , z ) + GEOMETRY_BLOCKING_OFFSET;
	RwIm3DVertexSetPos	( &pLineList[ 4 ] , fX + MAP_STEPSIZE , fHeight , fZ + 0	);
	RwIm3DVertexSetU	( &pLineList[ 4 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 4 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 4 ] , 255 , 0 , 0 , 128	);
	
	fHeight = pSector->D_GetHeight2( x + 1 , z + 1 ) + GEOMETRY_BLOCKING_OFFSET;
	RwIm3DVertexSetPos	( &pLineList[ 5 ] , fX + MAP_STEPSIZE , fHeight , fZ + MAP_STEPSIZE	);
	RwIm3DVertexSetU	( &pLineList[ 5 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 5 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 5 ] , 255 , 0 , 0 , 128	);

	m_nTriangleCount += 2;
}


void CPlugin_NoLayer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );

	dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );
	//dc.FillRect( rect , &m_Brush );

	dc.SetBkColor( RGB( 255 , 172 , 146 ) );

	// Do not call CWnd::OnPaint() for painting messages
}

int CPlugin_NoLayer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CPlugin_NoLayer::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CPlugin_NoLayer::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// 브러킹 표시 모드 변경.
	//m_nType = ( m_nType + 1 ) % MAX_BLOCKING_COUNT;
	Invalidate( FALSE );
	UpdateBlockingPolygon();
	
	CWnd::OnRButtonDown(nFlags, point);
}