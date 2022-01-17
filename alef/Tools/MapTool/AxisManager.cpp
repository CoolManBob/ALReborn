#include "stdafx.h"
#include "MapTool.h"
#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rpcollis.h"
#include "rtpick.h"
#include "rtcharse.h"
#include "rpmatfx.h"
#include "rtintsec.h"

#include ".\axismanager.h"

static INT32 g_cAxisDetail	= 20;
static INT32 g_cAxisPolygon	= 18;
static INT32 g_cAxisVertex	= 54;
static INT32 g_cAxisVertexTotal	= 162;

inline void SetXColor( RwIm3DVertex * pVertex )
{
	RwIm3DVertexSetRGBA	( pVertex , 255 , 0 , 0 , 255	);
}
inline void SetYColor( RwIm3DVertex * pVertex )
{
	RwIm3DVertexSetRGBA	( pVertex , 0 , 255 , 0 , 255	);
}
inline void SetZColor( RwIm3DVertex * pVertex )
{
	RwIm3DVertexSetRGBA	( pVertex , 0 , 0 , 255 , 255	);
}
inline void SetSelectedColor( RwIm3DVertex * pVertex )
{
	RwIm3DVertexSetRGBA	( pVertex , 255 , 255 , 255 , 255	);
}

CAxisManager::CAxisManager(void)
{
	m_pImVertex		= NULL	;
	m_uVertexCount	= 0		;
}

CAxisManager::~CAxisManager(void)
{
	if( m_pImVertex )
	{
		delete [] m_pImVertex;
		m_pImVertex = NULL;
	}
}

INT32			CAxisManager::SetSelectedAxis( INT32 nAxis )
{
	// 이전 인덱스 의 칼라 변경..
	// 새인덱스 칼라변경...
	// 인덱스 초기화..
	// 이전 인덱스 리턴..
	return -1;
}

INT32			CAxisManager::GetCollisionAxis( RwCamera * pCamera , RsMouseStatus *ms )
{
	// 폴리건 단위 콜리젼 실행..
	// 콜리젼 폴리건 & 거리 저장.
	// 가장 짧은 녀석의 축을 알아냄..
	// 축 리턴..
	// 현재 커서 위치에 있는 클럼프를 구해낸다.
	RwLine			pixelRay		;
	RwCameraCalcPixelRay( pCamera , &pixelRay , &ms->pos );

	INT32	nFound		= -1	;
	FLOAT	fFoundDistance	= 0.0f	;
	FLOAT	fDistance;

	RwV3d	v3dTransformed[ 3 ];

	RwV3d	v3dLineDelta;
	RwV3dSub( &v3dLineDelta , &pixelRay.end, &pixelRay.start );

	for( int i = 0 ; i < g_cAxisVertex ; i ++ )
	{
		RwV3dTransformPoint( v3dTransformed		, &m_pImVertex[ i * 3 + 0 ].objVertex ,  &m_matTransform );
		RwV3dTransformPoint( v3dTransformed + 1	, &m_pImVertex[ i * 3 + 1 ].objVertex ,  &m_matTransform );
		RwV3dTransformPoint( v3dTransformed + 2	, &m_pImVertex[ i * 3 + 2 ].objVertex ,  &m_matTransform );

		if( RtIntersectionLineTriangle( &pixelRay.start , &v3dLineDelta , 
			&v3dTransformed[ 0 ],
			&v3dTransformed[ 1 ],
			&v3dTransformed[ 2 ],
			&fDistance
			)
			||
			RtIntersectionLineTriangle( &pixelRay.start , &v3dLineDelta , 
			&v3dTransformed[ 0 ],
			&v3dTransformed[ 2 ],
			&v3dTransformed[ 1 ],
			&fDistance
			)
		)
		{
			if( nFound == -1 ||
				fDistance < fFoundDistance )
			{
				nFound = i;
				fFoundDistance = fDistance;
			}
		}
	}

	if( nFound != -1 )
	{
		return nFound / g_cAxisPolygon + 1;
	}
	else
	{
		return AXIS_NOSELECTED;
	}
}

BOOL			CAxisManager::SetScale	( FLOAT fScale )
{
	m_fScale = fScale;
	// 스케일 변환시켜서 매트릭스 새로만듬.
	RwV3d	vScale;
	vScale.x = m_fScale;
	vScale.y = m_fScale;
	vScale.z = m_fScale;
	RwMatrixScale		( &m_matTransform , &vScale , rwCOMBINEREPLACE );
	RwMatrixTranslate	( &m_matTransform , &m_vPos	, rwCOMBINEPOSTCONCAT  );
	return TRUE;
}

BOOL			CAxisManager::SetPosition	( RwV3d * pPos )
{
	// 축 옮겨서 매트릭스 새로만듬..
	m_vPos = *pPos;
	// 스케일 변환시켜서 매트릭스 새로만듬.
	RwV3d	vScale;
	vScale.x = m_fScale;
	vScale.y = m_fScale;
	vScale.z = m_fScale;
	RwMatrixScale		( &m_matTransform , &vScale , rwCOMBINEREPLACE );
	RwMatrixTranslate	( &m_matTransform , &m_vPos	, rwCOMBINEPOSTCONCAT  );
	return TRUE;
}

BOOL	CAxisManager::Init()
{
	if( m_pImVertex )
	{
		delete [] m_pImVertex;
		m_pImVertex = NULL;
	}

	g_cAxisPolygon	= g_cAxisDetail		* 3;
	g_cAxisVertex	= g_cAxisPolygon	* 3;
	g_cAxisVertexTotal = g_cAxisVertex * 3;

	VERIFY( m_pImVertex = new RwIm3DVertex[ g_cAxisVertexTotal ] );

	// Arrow 버택스 구성.
	// 인덱스 초기화.

	FLOAT	fAxisHeadHeightLimit = 0.7f;
	FLOAT	fScaleHead	= 0.2f;
	FLOAT	fScaleBody	= 0.1f;

	RwV2d	* aCircle = new RwV2d[ g_cAxisDetail ];;
	for( int i = 0 ; i < g_cAxisDetail ; i ++ )
	{
		aCircle[ i ].x	= sin( 3.1415927f * 2.0f * ( FLOAT ) i / ( FLOAT ) ( g_cAxisDetail ) );
		aCircle[ i ].y	= cos( 3.1415927f * 2.0f * ( FLOAT ) i / ( FLOAT ) ( g_cAxisDetail ) );
	}
	
	// X 축 
	{
		for( int nAxis = 0 ; nAxis < 3 ; nAxis ++ )
		{
			// 머리..
			int i;
			for( i = 0 ; i < g_cAxisDetail ; i ++ )
			{
				///////////////////////////////////
				RwIm3DVertexSetPos	( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 0 ] , 1.0f , 0.0f , 0.0f	);
				RwIm3DVertexSetU	( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 0 ] , 1.0f				);    
				RwIm3DVertexSetV	( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 0 ] , 1.0f				);
				SetXColor			( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 0 ]						);

				RwIm3DVertexSetPos	( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 1 ] , fAxisHeadHeightLimit , aCircle[ i ].x * fScaleHead , aCircle[ i ].y	* fScaleHead);
				RwIm3DVertexSetU	( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 1 ] , 1.0f				);    
				RwIm3DVertexSetV	( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 1 ] , 1.0f				);
				SetXColor			( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 1 ]						);

				RwIm3DVertexSetPos	( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 2 ] , fAxisHeadHeightLimit , aCircle[ ( i + 1 ) % g_cAxisDetail ].x * fScaleHead , aCircle[ ( i + 1 ) % g_cAxisDetail ].y	* fScaleHead);
				RwIm3DVertexSetU	( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 2 ] , 1.0f				);    
				RwIm3DVertexSetV	( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 2 ] , 1.0f				);
				SetXColor			( &m_pImVertex[ g_cAxisVertex * nAxis + i * 3 + 2 ]						);
			}

			for( i = 0 ; i < g_cAxisDetail ; i ++ )
			{
				RwIm3DVertexSetPos	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 0 ] , fAxisHeadHeightLimit , aCircle[ i ].x * fScaleBody , aCircle[ i ].y	* fScaleBody	);
				RwIm3DVertexSetU	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 0 ] , 1.0f				);    
				RwIm3DVertexSetV	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 0 ] , 1.0f				);
				SetXColor			( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 0 ]						);

				RwIm3DVertexSetPos	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 1 ] , fAxisHeadHeightLimit , aCircle[ ( i + 1 ) % g_cAxisDetail ].x * fScaleBody , aCircle[ ( i + 1 ) % g_cAxisDetail ].y	* fScaleBody	);
				RwIm3DVertexSetU	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 1 ] , 1.0f				);    
				RwIm3DVertexSetV	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 1 ] , 1.0f				);
				SetXColor			( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 1 ]						);

				RwIm3DVertexSetPos	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 2 ] , 0.0f , aCircle[ i ].x * fScaleBody , aCircle[ i ].y	* fScaleBody	);
				RwIm3DVertexSetU	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 2 ] , 1.0f				);    
				RwIm3DVertexSetV	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 2 ] , 1.0f				);
				SetXColor			( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 2 ]						);

				RwIm3DVertexSetPos	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 3 ] , fAxisHeadHeightLimit , aCircle[ ( i + 1 ) % g_cAxisDetail ].x * fScaleBody , aCircle[ ( i + 1 ) % g_cAxisDetail ].y	* fScaleBody		);
				RwIm3DVertexSetU	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 3 ] , 1.0f				);    
				RwIm3DVertexSetV	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 3 ] , 1.0f				);
				SetXColor			( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 3 ]						);

				RwIm3DVertexSetPos	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 4 ] , 0.0f , aCircle[ i ].x * fScaleBody , aCircle[ i ].y	* fScaleBody	);
				RwIm3DVertexSetU	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 4 ] , 1.0f				);    
				RwIm3DVertexSetV	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 4 ] , 1.0f				);
				SetXColor			( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 4 ]						);

				RwIm3DVertexSetPos	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 5 ] , 0.0f , aCircle[ ( i + 1 ) % g_cAxisDetail ].x * fScaleBody , aCircle[ ( i + 1 ) % g_cAxisDetail ].y	* fScaleBody	);
				RwIm3DVertexSetU	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 5 ] , 1.0f				);    
				RwIm3DVertexSetV	( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 5 ] , 1.0f				);
				SetXColor			( &m_pImVertex[ g_cAxisVertex * nAxis + g_cAxisDetail * 3 + i * 6 + 5 ]						);
			}

			switch( nAxis )
			{
			case 0: // X
				{
					// do nothing
				}
				break;
			case 1: // Y
				{
					FLOAT	fTemp;
					for( i = 0 ; i < g_cAxisVertex ; i ++ )
					{
						fTemp = m_pImVertex[ g_cAxisVertex * nAxis + i ].objVertex.x;
						m_pImVertex[ g_cAxisVertex * nAxis + i ].objVertex.x = m_pImVertex[ g_cAxisVertex * nAxis + i ].objVertex.y;
						m_pImVertex[ g_cAxisVertex * nAxis + i ].objVertex.y = fTemp;
						SetYColor( &m_pImVertex[ g_cAxisVertex * nAxis + i ]	);
					}
				}
				break;
			case 2: // Z
				{
					FLOAT	fTemp;
					for( i = 0 ; i < g_cAxisVertex ; i ++ )
					{
						fTemp = m_pImVertex[ g_cAxisVertex * nAxis + i ].objVertex.x;
						m_pImVertex[ g_cAxisVertex * nAxis + i ].objVertex.x = m_pImVertex[ g_cAxisVertex * nAxis + i ].objVertex.z;
						m_pImVertex[ g_cAxisVertex * nAxis + i ].objVertex.z = fTemp;
						SetZColor( &m_pImVertex[ g_cAxisVertex * nAxis + i ]	);
					}
				}
				break;
			}
		}
	}

	// Y축
	// Z축

	delete [] aCircle;

	return 0;
}

void	CAxisManager::Render()
{
	// 에로후 화면에 그려 넣음..
	/*
	if( RwIm3DTransform( m_pImVertex , g_cAxisVertex , &m_matTransform, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXRGBA ) )
	{                         
		RwIm3DRenderPrimitive( rwPRIMTYPETRILIST );
		RwIm3DEnd();
	}
	if( RwIm3DTransform( m_pImVertex + g_cAxisVertex  , g_cAxisVertex , &m_matTransform, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXRGBA ) )
	{                         
		RwIm3DRenderPrimitive( rwPRIMTYPETRILIST );
		RwIm3DEnd();
	}
	if( RwIm3DTransform( m_pImVertex + g_cAxisVertex * 2 , g_cAxisVertex , &m_matTransform, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXRGBA ) )
	{                         
		RwIm3DRenderPrimitive( rwPRIMTYPETRILIST );
		RwIm3DEnd();
	}
	*/
	if( RwIm3DTransform( m_pImVertex , g_cAxisVertexTotal , &m_matTransform, rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ ) )
	{                         
		RwIm3DRenderPrimitive( rwPRIMTYPETRILIST );
		RwIm3DEnd();
	}
}