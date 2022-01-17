// CPlugin_Blocking.cpp: implementation of the CPlugin_Blocking class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "maptool.h"
#include "Plugin_Blocking.h"
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

BEGIN_MESSAGE_MAP(CPlugin_Blocking, CWnd)
	//{{AFX_MSG_MAP(CPlugin_Blocking)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPlugin_Blocking::CPlugin_Blocking()
{
	m_pVertexBuffer		= NULL	;
	m_nVertexCount		= 0		;
	m_nTriangleCount	= 0		;
	m_bLButtonDown		= FALSE	;
	
	m_nType				= GEOMETRY_BLOCKING;

	// 옵션 설정.
	m_stOption.bShowSphere	= true;

	m_strShortName = "Block";

	VERIFY( AllocVertexBuffer( DEFAULT_VERTEX_BUFFER_SIZE ) );
}

CPlugin_Blocking::~CPlugin_Blocking()
{
	FreeVertexBuffer();
}

BOOL	CPlugin_Blocking::AllocVertexBuffer	( INT32 nSize )
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

void	CPlugin_Blocking::FreeVertexBuffer	()
{
	if( m_pVertexBuffer ) delete [] m_pVertexBuffer;

	m_pVertexBuffer		= NULL;
	m_nVertexCount		= 0;
	m_nTriangleCount	= 0;
}


BOOL CPlugin_Blocking::OnSelectedPlugin	()
{
	m_bLButtonDown	= FALSE;
	return TRUE;
}

BOOL CPlugin_Blocking::OnDeSelectedPlugin	()
{
	return TRUE;
}

BOOL CPlugin_Blocking::OnWindowRender		()
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

BOOL CPlugin_Blocking::OnLButtonDownGeometry	( RwV3d * pPos )
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
				pSelectedSegment->stTileInfo.SetGeometryBlocking( bValue );
					//!pSelectedSegment->stTileInfo.GetGeometryBlocking() );
				UpdateBlockingPolygon();

				SetSaveData();
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

BOOL CPlugin_Blocking::OnLButtonUpGeometry	( RwV3d * pPos )
{
	m_bLButtonDown = FALSE;
	return TRUE;
}

BOOL CPlugin_Blocking::OnMouseMoveGeometry	( RwV3d * pPos )
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
				SetSaveData();

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

										pSegment->stTileInfo.SetGeometryBlocking( bValue );
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

void	CPlugin_Blocking::UpdateBlockingPolygon( BOOL bGeometry , BOOL bObject )
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

	INT32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
	INT32	nObjectCount;
	
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

					if( pSegment )
					{
						switch( m_nType )
						{
						case GEOMETRY_BLOCKING	:
							{
								if( bGeometry && pSegment->stTileInfo.GetGeometryBlocking() ) // 여기서 블러킹 체크.
								{
									AddGeometryBlockingPolygon( pSector, x , z , fX , fZ , pSegment );
								}

								// 4방향 오브젝트 블러킹 체크.
								//AddObjectBlockingPolygon( pSector , x , z , fX , fZ , pSegment );
							}
							break;
						case OBJECT_BLOCKING	:
							{

								nObjectCount	=	pSector->GetObjectCollisionID( x , z , aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT );
								if( nObjectCount )
									AddGeometryBlockingTablePolygon( pSector, x , z , fX , fZ , pSegment );
							}
							break;
						case OBJECT_RIDABLE	:
							{
								nObjectCount	=	pSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT , x , z , aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT );
								if( nObjectCount )
									AddGeometryBlockingTablePolygon( pSector, x , z , fX , fZ , pSegment );
							}
							break;
						case SKY_BLOCKING:
							{
								if( bGeometry && pSegment->stTileInfo.GetSkyBlocking() ) // 여기서 블러킹 체크.
								{
									AddGeometryBlockingPolygon( pSector, x , z , fX , fZ , pSegment );
								}
							}
							break;
						}
					}
				}	
			}
		}
	}
}
void	CPlugin_Blocking::AddGeometryBlockingPolygon	( ApWorldSector	* pSector , int x , int z , FLOAT fX , FLOAT fZ , ApDetailSegment *	pSegment )
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
/*
void	CPlugin_Blocking::AddObjectBlockingPolygon( ApWorldSector	* pSector , int x , int z , FLOAT fXOrigin , FLOAT fZOrigin , ApDetailSegment * pSegment )
{
	if( NULL == g_MainWindow.m_pCurrentGeometry ) return;

	// 높이값 정리.
	FLOAT	pHeight[ 9 ];
	// 0 1 2
	// 3 4 5
	// 6 7 8

	pHeight[ 0 ]	=	pSector->D_GetHeight2( x		, z		);
	pHeight[ 2 ]	=	pSector->D_GetHeight2( x + 1	, z		);
	pHeight[ 6 ]	=	pSector->D_GetHeight2( x		, z + 1	);
	pHeight[ 8 ]	=	pSector->D_GetHeight2( x + 1	, z + 1	);

	// 중간 높이 계산.
	pHeight[ 1 ]	=	( pHeight[ 0 ] + pHeight[ 2 ] ) * 0.5f;
	pHeight[ 3 ]	=	( pHeight[ 0 ] + pHeight[ 6 ] ) * 0.5f;
	pHeight[ 4 ]	=	( pHeight[ 2 ] + pHeight[ 6 ] ) * 0.5f;
	pHeight[ 5 ]	=	( pHeight[ 2 ] + pHeight[ 8 ] ) * 0.5f;
	pHeight[ 7 ]	=	( pHeight[ 6 ] + pHeight[ 8 ] ) * 0.5f;

	// 옵셋 증가
	for( int i = 0 ; i < 9 ; ++ i ) pHeight[ i ] += OBJECT_BLOCKING_OFFSET;

	RwIm3DVertex *		pLineList			;
	FLOAT				fX , fZ , fStepSize	;
	fStepSize = MAP_STEPSIZE / 2.0f;

	if( pSegment->stTileInfo.GetObjectBlocking( TP_LEFTTOP ) )
	{
		pLineList	= m_pVertexBuffer + m_nTriangleCount * 3;

		fX	= fXOrigin + 0;
		fZ	= fZOrigin + 0;

		RwIm3DVertexSetPos	( &pLineList[ 0 ] , fX , pHeight[ 0 ] , fZ	);
		RwIm3DVertexSetU	( &pLineList[ 0 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 0 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 0 , 0 , 255 , 128	);

		RwIm3DVertexSetPos	( &pLineList[ 1 ] , fX + fStepSize , pHeight[ 1 ] , fZ + 0	);
		RwIm3DVertexSetU	( &pLineList[ 1 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 1 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 1 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 2 ] , fX + 0 , pHeight[ 3 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 2 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 2 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 2 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 3 ] , fX + 0 , pHeight[ 3 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 3 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 3 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 3 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 4 ] , fX +fStepSize , pHeight[ 1 ] , fZ + 0	);
		RwIm3DVertexSetU	( &pLineList[ 4 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 4 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 4 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 5 ] , fX + fStepSize , pHeight[ 4 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 5 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 5 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 5 ] , 0 , 0 , 255 , 128	);

		m_nTriangleCount += 2;
	}
	
	if( pSegment->stTileInfo.GetObjectBlocking( TP_RIGHTTOP ) )
	{
		pLineList	= m_pVertexBuffer + m_nTriangleCount * 3;

		fX	= fXOrigin + fStepSize;
		fZ	= fZOrigin + 0;

		RwIm3DVertexSetPos	( &pLineList[ 0 ] , fX , pHeight[ 1 ] , fZ	);
		RwIm3DVertexSetU	( &pLineList[ 0 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 0 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 0 , 0 , 255 , 128	);

		RwIm3DVertexSetPos	( &pLineList[ 1 ] , fX + fStepSize , pHeight[ 2 ] , fZ + 0	);
		RwIm3DVertexSetU	( &pLineList[ 1 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 1 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 1 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 2 ] , fX + 0 , pHeight[ 4 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 2 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 2 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 2 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 3 ] , fX + 0 , pHeight[ 4 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 3 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 3 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 3 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 4 ] , fX + fStepSize , pHeight[ 2 ] , fZ + 0	);
		RwIm3DVertexSetU	( &pLineList[ 4 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 4 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 4 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 5 ] , fX + fStepSize , pHeight[ 5 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 5 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 5 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 5 ] , 0 , 0 , 255 , 128	);

		m_nTriangleCount += 2;
	}	
	
	if( pSegment->stTileInfo.GetObjectBlocking( TP_LEFTBOTTOM ) )
	{
		pLineList	= m_pVertexBuffer + m_nTriangleCount * 3;

		fX	= fXOrigin + 0;
		fZ	= fZOrigin + fStepSize;

		RwIm3DVertexSetPos	( &pLineList[ 0 ] , fX , pHeight[ 3 ] , fZ	);
		RwIm3DVertexSetU	( &pLineList[ 0 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 0 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 0 , 0 , 255 , 128	);

		RwIm3DVertexSetPos	( &pLineList[ 1 ] , fX + fStepSize , pHeight[ 4 ] , fZ + 0	);
		RwIm3DVertexSetU	( &pLineList[ 1 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 1 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 1 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 2 ] , fX + 0 , pHeight[ 6 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 2 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 2 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 2 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 3 ] , fX + 0 , pHeight[ 6 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 3 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 3 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 3 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 4 ] , fX + fStepSize , pHeight[ 4 ] , fZ + 0	);
		RwIm3DVertexSetU	( &pLineList[ 4 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 4 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 4 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 5 ] , fX + fStepSize , pHeight[ 7 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 5 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 5 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 5 ] , 0 , 0 , 255 , 128	);

		m_nTriangleCount += 2;
	}	
	
	if( pSegment->stTileInfo.GetObjectBlocking( TP_RIGHTBOTTOM ) )
	{
		pLineList	= m_pVertexBuffer + m_nTriangleCount * 3;

		fX	= fXOrigin + fStepSize;
		fZ	= fZOrigin + fStepSize;

		RwIm3DVertexSetPos	( &pLineList[ 0 ] , fX , pHeight[ 4 ] , fZ	);
		RwIm3DVertexSetU	( &pLineList[ 0 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 0 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 0 , 0 , 255 , 128	);

		RwIm3DVertexSetPos	( &pLineList[ 1 ] , fX + fStepSize , pHeight[ 5 ] , fZ + 0	);
		RwIm3DVertexSetU	( &pLineList[ 1 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 1 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 1 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 2 ] , fX + 0 , pHeight[ 7 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 2 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 2 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 2 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 3 ] , fX + 0 , pHeight[ 7 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 3 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 3 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 3 ] , 0 , 0 , 255 , 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 4 ] , fX + fStepSize , pHeight[ 5 ] , fZ + 0	);
		RwIm3DVertexSetU	( &pLineList[ 4 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 4 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 4 ] , 0 , 0 , 255, 128	);
		
		RwIm3DVertexSetPos	( &pLineList[ 5 ] , fX + fStepSize , pHeight[ 8 ] , fZ + fStepSize	);
		RwIm3DVertexSetU	( &pLineList[ 5 ] , 1.0f				);    
		RwIm3DVertexSetV	( &pLineList[ 5 ] , 1.0f				);
		RwIm3DVertexSetRGBA	( &pLineList[ 5 ] , 0 , 0 , 255 , 128	);

		m_nTriangleCount += 2;
	}
}
*/

void CPlugin_Blocking::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );

	//dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );
	dc.FillRect( rect , &m_Brush );

	dc.SetBkColor( RGB( 255 , 172 , 146 ) );

	switch( m_nType )
	{
	case GEOMETRY_BLOCKING	:	dc.TextOut( 0 , 0 , "지형블러킹"		); break;
	case OBJECT_BLOCKING	:	dc.TextOut( 0 , 0 , "블러킹 테이블"		); break;
	case OBJECT_RIDABLE		:	dc.TextOut( 0 , 0 , "라이더블 테이블"	); break;
	case SKY_BLOCKING		:	dc.TextOut( 0 , 0 , "스카이블러킹"		); break;
	}
	
	// Do not call CWnd::OnPaint() for painting messages
}

int CPlugin_Blocking::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CBitmap	bitmap;
	bitmap	.LoadBitmap			( IDB_BLOCKING_PLUGIN	);
	m_Brush	.CreatePatternBrush	( & bitmap				);

	return 0;
}

void CPlugin_Blocking::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CPlugin_Blocking::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// 브러킹 표시 모드 변경.
	m_nType = ( m_nType + 1 ) % MAX_BLOCKING_COUNT;
	Invalidate( FALSE );
	UpdateBlockingPolygon();
	
	CWnd::OnRButtonDown(nFlags, point);
}

void	CPlugin_Blocking::AddGeometryBlockingTablePolygon	( ApWorldSector	* pSector , int x , int z , FLOAT fX , FLOAT fZ , ApDetailSegment *	pSegment )
{
	if( NULL == g_MainWindow.m_pCurrentGeometry ) return;

	RwRGBA	rgb;
	rgb.alpha	= 0;
	rgb.red		= 0;
	rgb.green	= 0;
	rgb.blue	= 0;

	switch( m_nType )
	{
	case GEOMETRY_BLOCKING	:	rgb.red		= 255;	break;
	case OBJECT_BLOCKING	:	rgb.blue	= 255;	break;
	case OBJECT_RIDABLE		:	rgb.green	= 255;	break;
	}

	RwIm3DVertex *		pLineList;
	FLOAT				fHeight;

	pLineList	= m_pVertexBuffer + m_nTriangleCount * 3;

	RwIm3DVertexSetPos	( &pLineList[ 0 ] , fX , pSegment->height + GEOMETRY_BLOCKING_OFFSET , fZ	);
	RwIm3DVertexSetU	( &pLineList[ 0 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 0 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , rgb.red , rgb.green , rgb.blue , 128	);

	fHeight = pSector->D_GetHeight2( x + 1 , z ) + GEOMETRY_BLOCKING_OFFSET;
	RwIm3DVertexSetPos	( &pLineList[ 1 ] , fX + MAP_STEPSIZE , fHeight , fZ + 0	);
	RwIm3DVertexSetU	( &pLineList[ 1 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 1 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 1 ] , rgb.red , rgb.green , rgb.blue , 128	);
	
	fHeight = pSector->D_GetHeight2( x , z + 1 ) + GEOMETRY_BLOCKING_OFFSET;
	RwIm3DVertexSetPos	( &pLineList[ 2 ] , fX + 0 , fHeight , fZ + MAP_STEPSIZE	);
	RwIm3DVertexSetU	( &pLineList[ 2 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 2 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 2 ] , rgb.red , rgb.green , rgb.blue , 128	);
	
	fHeight = pSector->D_GetHeight2( x , z + 1 ) + GEOMETRY_BLOCKING_OFFSET;
	RwIm3DVertexSetPos	( &pLineList[ 3 ] , fX + 0 , fHeight , fZ + MAP_STEPSIZE	);
	RwIm3DVertexSetU	( &pLineList[ 3 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 3 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 3 ] , rgb.red , rgb.green , rgb.blue , 128	);
	
	fHeight = pSector->D_GetHeight2( x + 1 , z ) + GEOMETRY_BLOCKING_OFFSET;
	RwIm3DVertexSetPos	( &pLineList[ 4 ] , fX + MAP_STEPSIZE , fHeight , fZ + 0	);
	RwIm3DVertexSetU	( &pLineList[ 4 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 4 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 4 ] , rgb.red , rgb.green , rgb.blue , 128	);
	
	fHeight = pSector->D_GetHeight2( x + 1 , z + 1 ) + GEOMETRY_BLOCKING_OFFSET;
	RwIm3DVertexSetPos	( &pLineList[ 5 ] , fX + MAP_STEPSIZE , fHeight , fZ + MAP_STEPSIZE	);
	RwIm3DVertexSetU	( &pLineList[ 5 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 5 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 5 ] , rgb.red , rgb.green , rgb.blue , 128	);

	m_nTriangleCount += 2;
}

BOOL CPlugin_Blocking::OnQuerySaveData	( char * pStr )
{
	strcpy( pStr , "지형 블러킹 정보 저장" );

	return TRUE;
}

#define ALEF_GEOMETRY_BLOCKING_SAVE_FILE_PATH		"map\\data\\blocking"
#define ALEF_GEOMETRY_BLOCKING_SAVE_FILE_NAME		"gb%04d.bmp"
#define ALEF_SKY_BLOCKING_SAVE_FILE_NAME			"sb%04d.bmp"

BOOL	__DivisionSaveBlockingInfoCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	char * pSubData = ( char * ) pData;

	char	strFilename[ MAX_PATH ];
	wsprintf( strFilename , "%s\\" ALEF_GEOMETRY_BLOCKING_SAVE_FILE_PATH "\\" ALEF_GEOMETRY_BLOCKING_SAVE_FILE_NAME , 
		ALEF_CURRENT_DIRECTORY , pDivisionInfo->nIndex );

	if( !AGCMMAP_THIS->SaveGeometryBlocking( pDivisionInfo->nIndex , strFilename ) )
	{
		TRACE( "블러킹 정보 저장실패!" );
	}

	wsprintf( strFilename , "%s\\" ALEF_GEOMETRY_BLOCKING_SAVE_FILE_PATH "\\" ALEF_GEOMETRY_BLOCKING_SAVE_FILE_NAME , 
		pSubData , pDivisionInfo->nIndex );

	if( !AGCMMAP_THIS->SaveGeometryBlocking( pDivisionInfo->nIndex , strFilename ) )
	{
		TRACE( "블러킹 정보 저장실패!" );
	}
	return TRUE;
}

BOOL	__DivisionLoadBlockingInfoCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	char	strFilename[ MAX_PATH ];
	wsprintf( strFilename , "%s\\" ALEF_GEOMETRY_BLOCKING_SAVE_FILE_PATH "\\" ALEF_GEOMETRY_BLOCKING_SAVE_FILE_NAME , 
		ALEF_CURRENT_DIRECTORY , pDivisionInfo->nIndex );

	AGCMMAP_THIS->LoadGeometryBlocking( pDivisionInfo->nIndex , strFilename );

	wsprintf( strFilename , "%s\\" ALEF_GEOMETRY_BLOCKING_SAVE_FILE_PATH "\\" ALEF_SKY_BLOCKING_SAVE_FILE_NAME , 
		ALEF_CURRENT_DIRECTORY , pDivisionInfo->nIndex );

	AGCMMAP_THIS->LoadSkyBlocking( pDivisionInfo->nIndex , strFilename );

	return TRUE;
}

BOOL CPlugin_Blocking::OnSaveData		()
{
	char	strSubDataPath[ MAX_PATH ];
	GetSubDataDirectory( strSubDataPath );

	//char	strBlocking		[ MAX_PATH ];
	//wsprintf( strBlocking		, "%s\\" ALEF_GEOMETRY_BLOCKING_SAVE_FILE_PATH , strSubDataPath );

	_CreateDirectory( strSubDataPath );
	_CreateDirectory( "%s\\map" , strSubDataPath );
	_CreateDirectory( "%s\\map\\data" , strSubDataPath );
	_CreateDirectory( "%s\\map\\data\\blocking" , strSubDataPath );

	return 	AGCMMAP_THIS->EnumLoadedDivision( __DivisionSaveBlockingInfoCallback , ( void * ) strSubDataPath );
}

BOOL CPlugin_Blocking::OnLoadData		()
{
	return 	AGCMMAP_THIS->EnumLoadedDivision( __DivisionLoadBlockingInfoCallback , NULL );
}
