// Plugin_GeometryEffect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "maptool.h"
#include "Mainfrm.h"
#include "MyEngine.h"
#include "Plugin_GeometryEffect.h"


// CPlugin_GeometryEffect

IMPLEMENT_DYNAMIC(CPlugin_GeometryEffect, CWnd)
CPlugin_GeometryEffect::CPlugin_GeometryEffect()
{
	m_strShortName = "GEffect";
}

CPlugin_GeometryEffect::~CPlugin_GeometryEffect()
{
}


BEGIN_MESSAGE_MAP(CPlugin_GeometryEffect, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CPlugin_GeometryEffect 메시지 처리기입니다.


void CPlugin_GeometryEffect::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 0 , 64 ) );
}

BOOL	__DivisionRenderSectorSelectionGridCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CPlugin_GeometryEffect * pPlugin = ( CPlugin_GeometryEffect * ) pData;

	ApWorldSector * pSector;
	int x , z;
	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			if( pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
			{
				pPlugin->RenderSectorSelection( pSector );
			}
		}
	}

	return TRUE;
}

BOOL CPlugin_GeometryEffect::OnWindowRender()
{
	AGCMMAP_THIS->EnumLoadedDivision( __DivisionRenderSectorSelectionGridCallback , ( PVOID ) this );
	return TRUE;
}

void	CPlugin_GeometryEffect::RenderSectorSelection	( ApWorldSector * pSector )
{
	AGCMMAP_THIS->AllocSectorGridInfo( pSector );
	// 그리드 칼라 바꿔서 한번더 찍는다.

	// 데이타 없으면 생성
	RwIm3DVertex	* pImVertex = AGCMMAP_THIS->GetAgcmAttachedData( pSector )->m_pImVertex;
	for( int i = 0 ; i < 65 ; i ++ )
		RwIm3DVertexSetRGBA	( &pImVertex[ i ] , 255 , 0 , 0 , 255  );

	// 선택적으로 덮어 씌우기..

	if( RwIm3DTransform( pImVertex , 65 , NULL, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXRGBA ) )
	{                         
		RwIm3DRenderPrimitive( rwPRIMTYPELINELIST );
		RwIm3DEnd();
	}

	for( int i = 0 ; i < 65 ; i ++ )
		RwIm3DVertexSetRGBA	( &pImVertex[ i ] , 255 , 255 , 255 , 255  );
}

BOOL CPlugin_GeometryEffect::OnLButtonDownGeometry	( RwV3d * pPos )
{
	ApWorldSector * pSector = g_pcsApmMap->GetSector( pPos->x , pPos->z );
	if( NULL == pSector ) return TRUE;

	// 섹터 선택

	// 데이타 세팅 여부 확인
	if( pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
	{
		// 변환 여부 확인
		if( IDYES == MessageBox( "지형 효과를 제거하겠습니까?" , "Geometry Effect Plugin" , MB_YESNOCANCEL ) )
		{
			// 변환 작업..
			pSector->SetFlag( pSector->GetFlag() & ~ApWorldSector::OP_GEOMETRYEFFECTENABLE );
			AGCMMAP_THIS->Update( pSector );
			if( AGCMMAP_THIS->LockSector		( pSector , AGCMMAP_THIS->GetGeometryLockMode() , SECTOR_HIGHDETAIL	) )
			{
				AGCMMAP_THIS->RecalcNormal		( pSector , SECTOR_HIGHDETAIL							);
				if( pSector->GetNearSector( TD_EAST ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_EAST ) , SECTOR_HIGHDETAIL	);
				}
				if( pSector->GetNearSector( TD_SOUTH ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_SOUTH ) , SECTOR_HIGHDETAIL	);
				}
				if( pSector->GetNearSector( TD_SOUTH ) && pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) , SECTOR_HIGHDETAIL	);
				}

				AGCMMAP_THIS->UnlockSectors		( TRUE													);
			}
			SaveSetChangeMoonee();
		}
	}
	else
	{
		// 변환 여부 확인
		if( IDYES == MessageBox( "지형 효과를 설치하겠습니까? 타일 알파정보는 모두 사라지게 됩니다. 언두불가!" , "Geometry Effect Plugin" , MB_YESNOCANCEL ) )
		{
			// 변환 작업..
			pSector->SetFlag( pSector->GetFlag() | ApWorldSector::OP_GEOMETRYEFFECTENABLE );
			AGCMMAP_THIS->Update( pSector );
			if( AGCMMAP_THIS->LockSector		( pSector , AGCMMAP_THIS->GetGeometryLockMode() , SECTOR_HIGHDETAIL	) )
			{
				AGCMMAP_THIS->RecalcNormal		( pSector , SECTOR_HIGHDETAIL							);
				if( pSector->GetNearSector( TD_EAST ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_EAST ) , SECTOR_HIGHDETAIL	);
				}
				if( pSector->GetNearSector( TD_SOUTH ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_SOUTH ) , SECTOR_HIGHDETAIL	);
				}
				if( pSector->GetNearSector( TD_SOUTH ) && pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) )
				{
					AGCMMAP_THIS->RecalcNormal		( pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) , SECTOR_HIGHDETAIL	);
				}

				AGCMMAP_THIS->UnlockSectors		( TRUE													);
			}
			SaveSetChangeMoonee();
		}
	}

	return TRUE;
}