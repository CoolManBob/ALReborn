#include "stdafx.h"
#include "MapTool.h"
#include "Mainfrm.h"
#include "Plugin_LoadBlock.h"

BEGIN_MESSAGE_MAP(CPlugin_LoadBlock, CWnd)
	//{{AFX_MSG_MAP(CPlugin_LoadBlock)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
END_MESSAGE_MAP()

CPlugin_LoadBlock::CPlugin_LoadBlock(void)
{
	m_strShortName = "Load";
}

CPlugin_LoadBlock::~CPlugin_LoadBlock(void)
{
}

BOOL	CPlugin_LoadBlock::CBRenderSector ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CPlugin_LoadBlock * pPlugin = ( CPlugin_LoadBlock * ) pData;

	ApWorldSector * pSector;
	int x , z;
	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			if( pSector->GetFlag() & ApWorldSector::OP_DONOTLOADSECTOR )
			{
				pPlugin->RenderSectorSelection( pSector );
			}
		}
	}

	return TRUE;
}

BOOL CPlugin_LoadBlock::OnWindowRender()
{
	AGCMMAP_THIS->EnumLoadedDivision( CPlugin_LoadBlock::CBRenderSector , ( PVOID ) this );
	return TRUE;
}

void	CPlugin_LoadBlock::RenderSectorGrid( ApWorldSector * pSector )
{
	// 섹터 주위 테두리.
	RwIm3DVertex	* pImVertex = AGCMMAP_THIS->GetAgcmAttachedData( pSector )->m_pImVertex;
	if( !AGCMMAP_THIS->GetAgcmAttachedData( pSector )->m_bValidGrid ) return;

	// 선택적으로 덮어 씌우기..

	if( RwIm3DTransform( pImVertex , 65 , NULL, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXRGBA ) )
	{                         
		RwIm3DRenderPrimitive( rwPRIMTYPELINELIST );
		RwIm3DEnd();
	}
}

void	CPlugin_LoadBlock::RenderSectorSelection	( ApWorldSector * pSector )
{
	AGCMMAP_THIS->AllocSectorGridInfo( pSector );
	// 그리드 칼라 바꿔서 한번더 찍는다.

	// 데이타 없으면 생성
	RwIm3DVertex	* pImVertex = AGCMMAP_THIS->GetAgcmAttachedData( pSector )->m_pImVertex;

	for( int i = 0 ; i < 70 ; i ++ )
		RwIm3DVertexSetRGBA	( &pImVertex[ i ] , 255 , 0 , 0 , 128  );

	// 선택적으로 덮어 씌우기..

	if( RwIm3DTransform( pImVertex , 65 , NULL, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXRGBA ) )
	{                         
		RwIm3DRenderPrimitive( rwPRIMTYPELINELIST );
		RwIm3DEnd();
	}

	RwImVertexIndex pIndex[] = { 0 , 1 , 2 , 0 , 3 , 1 };

	if( RwIm3DTransform( pImVertex + 65 , 4 , NULL, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXRGBA ) )
	{            
		RwIm3DRenderIndexedPrimitive( rwPRIMTYPETRILIST , pIndex , 6 );
		RwIm3DEnd();
	}

	for( int i = 0 ; i < 70 ; i ++ )
		RwIm3DVertexSetRGBA	( &pImVertex[ i ] , 255 , 255 , 255 , 255  );
}

BOOL CPlugin_LoadBlock::OnLButtonDownGeometry	( RwV3d * pPos )
{
	ApWorldSector * pSector = g_pcsApmMap->GetSector( pPos->x , pPos->z );
	if( NULL == pSector ) return TRUE;

	// 섹터 선택

	// 데이타 세팅 여부 확인
	if( pSector->GetFlag() & ApWorldSector::OP_DONOTLOADSECTOR )
	{
		// 변환 여부 확인
		//if( IDYES == MessageBox( "지형이 읽혀지도록 하겠습니까?" , "로딩 블럭 플러그인" , MB_YESNOCANCEL ) )
		//{
			// 변환 작업..
			pSector->SetFlag( pSector->GetFlag() & ~ApWorldSector::OP_DONOTLOADSECTOR );
			SaveSetChangeMoonee();
		//}
	}
	else
	{
		// 변환 여부 확인
		//if( IDYES == MessageBox( "지형이 안읽히게 할래요?" , "로딩 블럭 플러그인" , MB_YESNOCANCEL ) )
		//{
			// 변환 작업..
			pSector->SetFlag( pSector->GetFlag() | ApWorldSector::OP_DONOTLOADSECTOR );
			SaveSetChangeMoonee();
		//}
	}
	return TRUE;
}

void CPlugin_LoadBlock::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int	nLine = 0;

	dc.SetTextColor( RGB( 255 , 255 , 255 ) );
	dc.SetBkMode( TRANSPARENT );

	dc.TextOut( 5 , nLine , "클라이언트에서 읽지 않을 지형을 설정함." );
	nLine += 20;
	dc.TextOut( 5 , nLine , "저장은 무늬파일에 같이 저장돼고" );
	nLine += 20;
	dc.TextOut( 5 , nLine , "익스포트시 Compact 데이타에 들어가용" );
	nLine += 20;

}
