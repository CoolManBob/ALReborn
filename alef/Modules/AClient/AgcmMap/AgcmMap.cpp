// AgcmMap.cpp: implementation of the AgcmMap class.
//
//////////////////////////////////////////////////////////////////////

#include "ApBase.h"
#include <skeleton.h>
#include "AgcmMap.h"
#include "MagDebug.h"
#include "rtimport.h"
#include "rpcollis.h"
#include "AcuRpMTexture.h"
#include "AcuObject.h"
#include "AgcEngine.h"
//@{ Jaewon 20050420
#include "AcuRtAmbOcclMap.h"
//@} Jaewon

#include "rtintsec.h"
#include "rtpick.h"
#include "DWSStream.h"
#include "MagPackManager.h"
//#include "MagUnpackManager.h"

#include "RtPITexD.h"

#include "AuRwImage.h"
#include "AcuFrameMemory.h"

#include "RtRay.h"
#include "AcuTexture.h"
#include "bmp.h"

#include "ApMemoryTracker.h"
#include "AgcmUIConsole.h"

#define	AGCMMAP_GEOMETRY_FILE_VERSION	0xff010101
#define	AGCMMAP_MOONEE_FILE_VERSION	0xff010102

#define _USE_MAP_EXPANSION_

//@{ Jaewon 20050420
// TODO: There are 3 functions doing the same job...
static RwInt32 _RwD3D9RasterExtOffset=0;

typedef struct _rwD3D9Palette _rwD3D9Palette;
struct _rwD3D9Palette
{
	PALETTEENTRY    entries[256];
	RwInt32     globalindex;
};

typedef LPDIRECT3DSURFACE9 LPSURFACE;
typedef LPDIRECT3DTEXTURE9 LPTEXTURE;

typedef struct _rwD3D9RasterExt _rwD3D9RasterExt;
struct _rwD3D9RasterExt
{
	LPTEXTURE               texture;
	_rwD3D9Palette          *palette;
	RwUInt8                 alpha;              /* This texture has alpha */
	RwUInt8                 cube : 4;           /* This texture is a cube texture */
	RwUInt8                 face : 4;           /* The active face of a cube texture */
	RwUInt8                 automipmapgen : 4;  /* This texture uses automipmap generation */
	RwUInt8                 compressed : 4;     /* This texture is compressed */
	RwUInt8                 lockedMipLevel;
	LPSURFACE               lockedSurface;
	D3DLOCKED_RECT          lockedRect;
	D3DFORMAT               d3dFormat;          /* D3D format */
	LPDIRECT3DSWAPCHAIN9    swapChain;
	HWND                    window;
};

#define RASTEREXTFROMRASTER(raster) \
	((_rwD3D9RasterExt *)(((RwUInt8 *)(raster)) + _RwD3D9RasterExtOffset))

static void fxConstUploadCB(RpAtomic *atomic, RpMaterial *material, DxEffect *effect, RwUInt32 pass, RwUInt32 nLights)
{
	ASSERT(RpAmbOcclMapAtomicGetAmbOcclMap(atomic));
	if( RpAmbOcclMapAtomicGetAmbOcclMap(atomic) )
	{
		effect->pSharedD3dXEffect->d3dxEffect[nLights]
			->SetTexture(effect->texture[effect->textureCount-1].handle[nLights],
				RASTEREXTFROMRASTER(RwTextureGetRaster(RpAmbOcclMapAtomicGetAmbOcclMap(atomic)))->texture);
	}
}
//@} Jaewon

__declspec( thread )	UINT32	tid = 0;
UINT32							btid = 0;

// 마고자 (2004-05-07 오전 11:07:13) : 커서클럼프 펑션에서 어느 타입을 검사할건지 추가함.
// GetCursorClump 에서 사용함..
static	INT32	g_nGetCursorClumpType	= ACUOBJECT_TYPE_OBJECT;
BOOL	g_bDisablePolygonLoading = FALSE;

#define	AGCMMAP_THIS ( &AgcmMap::GetInstance() )

struct	_FindClumpStruct
{
	float		distance;
	RpClump	*	pClump	;
};

struct	_FindHeightStruct
{
	_FindHeightStruct(): pcsAgcmMap( NULL ) , fHeight ( 0.0f ) , bExistRidableObject( FALSE ) , bCollisionAvailable( FALSE ){}
	
	BOOL		bCollisionAvailable	;

	AgcmMap *	pcsAgcmMap			;
	FLOAT		fHeight				;
	BOOL		bExistRidableObject	;
	FLOAT		fCharacterHeight	;	// 케릭터의 위치.

	RpIntersection	* pIntersection	;
};

// 마고자 (2005-03-23 오후 5:26:37) : 
// 클라이언트에 저장할 데이타..
struct	_MaptoolClientSaveData
{
	// Common
	RwInt32             numVertices		;

	// Geometry
	//RwV3d				*vertices	;
	RwV3d				*normals	;

	// Moonee
	RwInt32             numTexCoordSets	;
	RwTexCoords			*texCoords[rwMAXTEXTURECOORDS]	;
	//RwRGBA				*preLitLum	;

	//@{ Jaewon 20050420
	// For geometry effect & ambient occlusion
	RwUInt32 aoObjectFlag_;
	RwUInt32 aoMapSize_;
	RwChar aoMapName_[32];
	RwUInt32 aoNumSamples_;
	//@} Jaewon

	_MaptoolClientSaveData() :
		numVertices		( 0 ),
		numTexCoordSets	( 0 ),
		//vertices		( NULL ),
		normals			( NULL )
		//preLitLum		( NULL )
		//@{ Jaewon 20050420
		,
		aoObjectFlag_(rtAMBOCCLMAPOBJECTNAFLAG),
		aoMapSize_(0),
		aoNumSamples_(0)
		//@} Jaewon
	{
		for( int i = 0 ; i < rwMAXTEXTURECOORDS ; i ++ )
		{
			texCoords[ i ] = NULL;
		}
		//@{ Jaewon 20050420
		memset(aoMapName_, 0, sizeof(aoMapName_));
		//@} Jaewon
	}

	~_MaptoolClientSaveData()
	{
		DeleteContents();
	}

	void	DeleteContents()
	{
		numVertices		= 0;
		numTexCoordSets	= 0;

		#define __DELETE( a ) if( a ) { delete [] a; a = NULL; }
		//__DELETE( vertices );
		__DELETE( normals );
		//__DELETE( preLitLum );

		for( int i = 0 ; i < rwMAXTEXTURECOORDS ; i ++ )
		{
			__DELETE( texCoords[ i ] );
		}

		//@{ Jaewon 20050420
		aoObjectFlag_ = rtAMBOCCLMAPOBJECTNAFLAG;
		aoMapSize_ = 0;
		aoNumSamples_ = 0;
		memset(aoMapName_, 0, sizeof(aoMapName_));
		//@} Jaewon
	}

	BOOL	GeometryStreamWrite	( ApWorldSector * pSector , INT32 nDetail , FILE * pFile );
	BOOL	GeometryStreamRead	( ApWorldSector * pSector , INT32 nDetail , FILE * pFile );
	BOOL	MooneeStreamWrite	( ApWorldSector * pSector , INT32 nDetail , FILE * pFile );
	BOOL	MooneeStreamRead	( ApWorldSector * pSector , INT32 nDetail , FILE * pFile );

	BOOL	IsLoadedGeometry()
	{
		if( numVertices && 
			//vertices && 
			normals ) return TRUE;
		else return FALSE;
	}

	BOOL	IsLoadedMoonee()
	{
		if( numVertices && texCoords[ 0 ]
			//&& preLitLum
			) return TRUE;
		else return FALSE;
	}
};

BOOL	_MaptoolClientSaveData::GeometryStreamWrite	( ApWorldSector * pSector , INT32 nDetail , FILE * pFile )
{
	if( NULL == pFile ) return FALSE;
	// Vertex 갯수
	// 포지션
	// 노멀 

	UINT32	uVersion = AGCMMAP_GEOMETRY_FILE_VERSION;
	fwrite( ( void * ) &uVersion , sizeof INT32	, 1 , pFile	);

	rsDWSectorInfo dwSectorInfo;
	rsDWSectorInfo * pDWSectorInfo = AGCMMAP_THIS->GetDWSector( pSector , nDetail , &dwSectorInfo );

	fwrite( ( void * ) &pDWSectorInfo->numVertices , sizeof INT32	, 1 , pFile	);

	if( pDWSectorInfo->numVertices )
	{
		//fwrite( ( void * ) pDWSectorInfo->vertices	, sizeof RwV3d , pDWSectorInfo->numVertices , pFile	);
		fwrite( ( void * ) pDWSectorInfo->normals	, sizeof RwV3d , pDWSectorInfo->numVertices , pFile	);
	}
	else
	{
		// do nothing..
	}

	return TRUE;
}

BOOL	_MaptoolClientSaveData::GeometryStreamRead	( ApWorldSector * pSector , INT32 nDetail , FILE * pFile )
{
	if( NULL == pFile ) return FALSE;

	UINT32	uVersion;
	fread( ( void * ) &uVersion , sizeof INT32	, 1 , pFile	);
	if( AGCMMAP_GEOMETRY_FILE_VERSION != uVersion ) return FALSE;

	// Vertex 갯수
	// 포지션
	// 노멀

	//if( vertices	) delete [] vertices	;
	if( normals		) delete [] normals		;
	//vertices	= NULL;
	normals		= NULL;
	
	fread( ( void * ) &numVertices , sizeof INT32	, 1 , pFile	);

	if( numVertices )
	{
		// 마고자 (2005-03-23 오후 6:01:42) : 
		// 메모리 얼록!

		//vertices	= new RwV3d[ numVertices ];
		normals		= new RwV3d[ numVertices ];

		//fread( ( void * ) vertices	, sizeof RwV3d , numVertices , pFile	);
		fread( ( void * ) normals	, sizeof RwV3d , numVertices , pFile	);

		return TRUE;
	}
	else
	{
		// do nothing..
		return FALSE;
	}
}

BOOL	_MaptoolClientSaveData::MooneeStreamWrite	( ApWorldSector * pSector , INT32 nDetail , FILE * pFile )
{
	if( NULL == pFile ) return FALSE;

	UINT32	uVersion = AGCMMAP_MOONEE_FILE_VERSION;
	fwrite( ( void * ) &uVersion , sizeof INT32	, 1 , pFile	);

	rsDWSectorInfo dwSectorInfo;
	rsDWSectorInfo * pDWSectorInfo = AGCMMAP_THIS->GetDWSector( pSector , nDetail , &dwSectorInfo  );
	// Vertex 갯수
	// 텍스쳐 쿠디네잇 갯수
	// UV정보
	// 칼라정보 .

	fwrite( ( void * ) &pDWSectorInfo->numVertices , sizeof INT32	, 1 , pFile	);

	if( pDWSectorInfo->numVertices )
	{
		INT32	nTexCoods = 0;
		if( pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
		{
			//@{ Jaewon 20050416
			//@{ Jaewon 20051006
			// (RtAmbOcclMapAtomicGetFlags(pDWSectorInfo->pDWSector->atomic) & rtAMBOCCLMAPOBJECTINDOOR) check added.
			// If the ambient occlusion data has been destroyed, do not save the second texture coordinates.
			nTexCoods = (pDWSectorInfo->texCoords[1] && (RtAmbOcclMapAtomicGetFlags(pDWSectorInfo->pDWSector->atomic) & rtAMBOCCLMAPOBJECTINDOOR))?2:1;
			//@} Jaewon
			//@} Jaewon
		}
		else
			nTexCoods = 6;

		fwrite( ( void * ) &nTexCoods , sizeof INT32	, 1 , pFile	);

		// UV 정보..
		for( int i = 0 ; i < nTexCoods ; i ++ )
		{
			fwrite( ( void * ) pDWSectorInfo->texCoords[ i ] , sizeof RwTexCoords , pDWSectorInfo->numVertices , pFile	);
		}

		// 칼라정보..
		//fwrite( ( void * ) pDWSectorInfo->preLitLum , sizeof RwRGBA , pDWSectorInfo->numVertices , pFile	);

		//@{ Jaewon 20050420
		// Write ambient occlusion data.
		if( nDetail == SECTOR_HIGHDETAIL &&
			pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
		{
			// aoVersion
			RwChar aoVersion = 0;
			fwrite((const void*)&aoVersion, sizeof(RwChar), 1, pFile);

			//@{ Jaewon 20050713
			// Do not save an atomic of which computation has not be finished.
			// aoObjectFlag_
			aoObjectFlag_ = RtAmbOcclMapAtomicGetFlags(pDWSectorInfo->pDWSector->atomic);

			RwTexture* pTexture = RpAmbOcclMapAtomicGetAmbOcclMap(pDWSectorInfo->pDWSector->atomic);
			if((aoObjectFlag_ & rtAMBOCCLMAPOBJECTAMBOCCLMAP) && pTexture
				&& !(aoObjectFlag_ & rtAMBOCCLMAPOBJECTNEEDILLUMINATION))
			{
				aoMapSize_ = RwRasterGetWidth(RwTextureGetRaster(pTexture));
				strncpy(aoMapName_, RwTextureGetName(pTexture), 32-1);
				aoNumSamples_ = RtAmbOcclMapAtomicGetNumSamples(pDWSectorInfo->pDWSector->atomic);
			}
			else
			{
				aoObjectFlag_ = 0;
				aoMapSize_ = 0;
				aoMapName_[0] = '\0';
				aoNumSamples_ = 0;
			}
			fwrite((const void*)&aoObjectFlag_, sizeof(RwUInt32), 1, pFile);
			//@} Jaewon

			// aoMapSize_
			fwrite((const void*)&aoMapSize_, sizeof(RwUInt32), 1, pFile);
			
			// aoMapName_
			RwChar length = (RwChar)strlen(aoMapName_);
			fwrite((const void*)&length, sizeof(RwChar), 1, pFile);
			fwrite((const void*)aoMapName_, sizeof(char), length, pFile);

			// aoNumSamples_
			fwrite((const void*)&aoNumSamples_, sizeof(RwUInt32), 1, pFile);
		}
		//@} Jaewon
	}

	return TRUE;
}
BOOL	_MaptoolClientSaveData::MooneeStreamRead	( ApWorldSector * pSector , INT32 nDetail , FILE * pFile )
{
	if( NULL == pFile ) return FALSE;

	UINT32	uVersion;
	fread( ( void * ) &uVersion , sizeof INT32	, 1 , pFile	);
	if( AGCMMAP_MOONEE_FILE_VERSION != uVersion ) return FALSE;

	// Vertex 갯수
	// 텍스쳐 쿠디네잇 갯수
	// UV정보
	// 칼라정보 .

	INT32	numVert;
	fread( ( void * ) &numVert , sizeof INT32	, 1 , pFile	);

	//@{ Jaewon 20050421
	numVertices = numVert;
	//if( numVertices && numVert != numVertices ) return FALSE;
	//@} Jaewon

	if( numVertices )
	{
		fread( ( void * ) &numTexCoordSets , sizeof INT32	, 1 , pFile	);

		// UV 정보..
		for( int i = 0 ; i < numTexCoordSets ; i ++ )
		{
			texCoords[ i ]	= new RwTexCoords[ numVertices ];
			fread( ( void * ) texCoords[ i ] , sizeof RwTexCoords , numVertices , pFile	);
		}

		// 칼라정보..
		//preLitLum	= new RwRGBA[ numVertices ];
		//fread( ( void * ) preLitLum , sizeof RwRGBA , numVertices , pFile	);

		//@{ Jaewon 20050420
		// Read ambient occlusion data.
		if( nDetail == SECTOR_HIGHDETAIL &&
			pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
		{
			// aoVersion
			RwChar aoVersion;
			fread((void*)&aoVersion, sizeof(RwChar), 1, pFile);
			ASSERT(aoVersion==0);

			// aoObjectFlag_
			fread((void*)&aoObjectFlag_, sizeof(RwUInt32), 1, pFile);

			// aoMapSize_
			fread((void*)&aoMapSize_, sizeof(RwUInt32), 1, pFile);

			// aoMapName_
			RwChar length;
			fread((void*)&length, sizeof(RwChar), 1, pFile);
			fread((void*)aoMapName_, sizeof(char), length, pFile);

			// aoNumSamples_
			fread((void*)&aoNumSamples_, sizeof(RwUInt32), 1, pFile);
		}
		//@} Jaewon
	}

	return TRUE;
}

RwRGBA & _RwRGBAAssigApRGB ( RwRGBA & rwRGBA , ApRGBA & apRGBA )
{
	rwRGBA.alpha	= apRGBA.alpha	;
	rwRGBA.red		= apRGBA.red	;
	rwRGBA.green	= apRGBA.green	;
	rwRGBA.blue		= apRGBA.blue	;

	return rwRGBA;
}

inline	BOOL __CalcPolygonNormal( rsDWSectorInfo * pSectorInfo , INT32 nPolygonIndex , RwV3d * pNormal )
{
	ASSERT( NULL != pSectorInfo							);
	ASSERT( 0 <= nPolygonIndex							);
	ASSERT( NULL != pNormal								);

	ASSERT( nPolygonIndex < pSectorInfo->numPolygons	);
	if( nPolygonIndex >= pSectorInfo->numPolygons )
	{
		// 마고자 (2004-06-28 오후 1:45:46) : 발생하면 안됌..
		return FALSE;
	}
	else
	{
		RwV3d	a , b;

 		RwV3dSub( &a,
			&pSectorInfo->vertices[ pSectorInfo->polygons[ nPolygonIndex ].vertIndex[ 2 ] ],
			&pSectorInfo->vertices[ pSectorInfo->polygons[ nPolygonIndex ].vertIndex[ 0 ] ] );
		RwV3dSub( &b,
			&pSectorInfo->vertices[ pSectorInfo->polygons[ nPolygonIndex ].vertIndex[ 1 ] ],
			&pSectorInfo->vertices[ pSectorInfo->polygons[ nPolygonIndex ].vertIndex[ 0 ] ] );

		RwV3dCrossProduct	( pNormal , &a , &b	);
		RwV3dNormalize		( pNormal , pNormal	);

		RwV3dScale			( pNormal , pNormal , -1.0f	);

		return TRUE;
	}
}


FLOAT	_SplitValue( FLOAT source , FLOAT destination , int offset , int offsetmax )
{
	return source + ( destination - source ) / ( FLOAT ) offsetmax * ( FLOAT ) offset;
}

void _PreviewTextureSetup( ApWorldSector * pSector , int indexX , int indexY , int tileindex , int depth , RwTexCoords * pCoord )
{
	if( pSector == NULL || tileindex != ALEF_SECTOR_DEFAULT_TILE_INDEX ) return;

	// 섹터 UV 범위를 구함
	RwTexCoords uvStart , uvEnd;
	
	uvStart.u	= ( ( float ) SectorIndexToArrayIndexX( pSector->GetIndexX() )			/ ( float ) MAP_WORLD_INDEX_WIDTH	);
	uvStart.v	= ( ( float ) SectorIndexToArrayIndexZ( pSector->GetIndexZ() )			/ ( float ) MAP_WORLD_INDEX_HEIGHT	);
	uvEnd.u		= ( ( float ) ( SectorIndexToArrayIndexX( pSector->GetIndexX() ) + 1 )	/ ( float ) MAP_WORLD_INDEX_WIDTH	);
	uvEnd.v		= ( ( float ) ( SectorIndexToArrayIndexZ( pSector->GetIndexZ() ) + 1 )	/ ( float ) MAP_WORLD_INDEX_HEIGHT	);

	// uv 값을 적용함..
	pCoord->u = uvStart.u + ( uvEnd.u - uvStart.u ) / ( float ) depth * ( float ) indexX;
	pCoord->v = uvStart.v + ( uvEnd.v - uvStart.v ) / ( float ) depth * ( float ) indexY;
}

RpAtomic * AgcmMap::GetBBoxCallback(RpAtomic * pAtomic, void *data)
{
	RwBBox * pBBox	= ( RwBBox * ) data;

	RwFrame	* pFrame	= RpAtomicGetFrame( pAtomic );
	ASSERT( NULL != pFrame );

	AgcmObject::m_pThis->LockFrame();
	RwMatrix * pMatrix = RwFrameGetLTM( pFrame );
	AgcmObject::m_pThis->UnlockFrame();

	RpGeometry * pGeometry = RpAtomicGetGeometry( pAtomic );
	RpMorphTarget * pMorphTarget = pGeometry ? RpGeometryGetMorphTarget( pGeometry , 0 ) : NULL ;
	if( NULL == pGeometry || NULL == pMorphTarget ) return pAtomic;

	RwBBox bbox;
	RwBBoxCalculate( & bbox , pMorphTarget->verts , pGeometry->numVertices );

	RwV3d	vPos[4]	;

	FLOAT	fX1,fX2 , fZ1, fZ2;
	fX1	= bbox.inf.x < bbox.sup.x ? bbox.inf.x : bbox.sup.x;
	fX2	= bbox.inf.x < bbox.sup.x ? bbox.sup.x : bbox.inf.x;
	fZ1	= bbox.inf.z < bbox.sup.z ? bbox.inf.z : bbox.sup.z;
	fZ2	= bbox.inf.z < bbox.sup.z ? bbox.sup.z : bbox.inf.z;

	#define __SETVERTEX( v , fx , fz )	{ (v).x = (fx) ; (v).z = (fz) ; (v).y = 0.0f;}

	__SETVERTEX( vPos[ 0 ]	, fX1 , fZ1 );
	__SETVERTEX( vPos[ 1 ]	, fX2 , fZ1 );
	__SETVERTEX( vPos[ 2 ]	, fX1 , fZ2 );
	__SETVERTEX( vPos[ 3 ]	, fX2 , fZ2 );

	RwV3dTransformPoints( vPos , vPos , 4 , pMatrix );

    //*pBBox = bbox;

	pBBox->inf.x = vPos[ 0 ].x;
	pBBox->sup.x = vPos[ 0 ].x;
	pBBox->inf.z = vPos[ 0 ].z;
	pBBox->sup.z = vPos[ 0 ].z;

	for( int i = 1 ; i < 4 ; i ++ )
	{
		if( vPos[ i ].x < pBBox->inf.x ) pBBox->inf.x = vPos[ i ].x;
		if( vPos[ i ].x > pBBox->sup.x ) pBBox->sup.x = vPos[ i ].x;
		if( vPos[ i ].z < pBBox->inf.z ) pBBox->inf.z = vPos[ i ].z;
		if( vPos[ i ].z > pBBox->sup.z ) pBBox->sup.z = vPos[ i ].z;
	}

	return	pAtomic;
}

static RpAtomic *	__GetWorldSectorGeometryCallback(
							 RpIntersection	*	intersection	,
							 RpWorldSector	*	sector			,
							 RpAtomic		*	pAtomic			,
							 RwReal				distance		,
							 void			*	data			)
{
	// ASSERT( NULL != data );
	AuList< _FindWorldSectorGeometryStruct > *	pListSector		= (	AuList< _FindWorldSectorGeometryStruct > * ) data;

	// 우선 월드 아토믹인지 검사함..
	INT32		index		;
	INT32		nType		= AcuObject::GetAtomicType( pAtomic , & index );

	// 아토믹의 타입이 월드섹터 일때만 등록..

	RpClump		* pClump			= RpAtomicGetClump( pAtomic )	;
	RpAtomic	* pCollisionAtomic	= pAtomic						;

	if( pClump )
	{
		// 아토믹에 없으면..클럼프엔 있나 확인해야함..
		nType		= AcuObject::GetClumpType( pClump , & index );
	}

	switch( ACUOBJECT_TYPE_TYPEFILEDMASK & nType )
	{
	case	ACUOBJECT_TYPE_WORLDSECTOR	:
		break;
	case	ACUOBJECT_TYPE_OBJECT		:
		{
			if( AcuObject::GetProperty( nType ) & ACUOBJECT_TYPE_RIDABLE )
			{
				// 높이체크를 해준다.
				if( AGCMMAP_THIS->m_pfGetCollisionAtomicCallback )
					pCollisionAtomic = AGCMMAP_THIS->m_pfGetCollisionAtomicCallback( pClump , NULL );

				if( NULL == pCollisionAtomic ) pCollisionAtomic = pAtomic;
			}
			else
				return pAtomic;
		}
		break;
	case	ACUOBJECT_TYPE_CHARACTER	:
	case	ACUOBJECT_TYPE_ITEM			:
	case	ACUOBJECT_TYPE_NONE			:
	default:
		return pAtomic;
	}

	// 리스트에 추가함..
	_FindWorldSectorGeometryStruct	cs( distance, pAtomic );
	pListSector->AddTail( cs );

	return pAtomic;
}

struct	__IntersectionPolugonInfo
{
	BOOL	bCheckVerticalPolygon;
	INT32	nPolygonIndex	;
	RwV3d	vPos			;
	FLOAT	fDistance		;
};

static RpCollisionTriangle * __RsIntersectionCallBackGeometryTriangleAtomic
											(RpIntersection *intersection, RpCollisionTriangle *collTriangle,
											RwReal distance, void *data)
{

	__IntersectionPolugonInfo * pInfo = ( __IntersectionPolugonInfo * ) data;

	if (!pInfo->bCheckVerticalPolygon && 0.001 > MagFABS(collTriangle->normal.y) )
	{
		return collTriangle;
	}

	if( pInfo->nPolygonIndex == -1 )
	{
		pInfo->nPolygonIndex	= collTriangle->index	;
		
		pInfo->vPos.x = intersection->t.line.start.x + (intersection->t.line.end.x - intersection->t.line.start.x) * distance;
		pInfo->vPos.y = intersection->t.line.start.y + (intersection->t.line.end.y - intersection->t.line.start.y) * distance;
		pInfo->vPos.z = intersection->t.line.start.z + (intersection->t.line.end.z - intersection->t.line.start.z) * distance;

		pInfo->fDistance	= distance;

		return collTriangle;
	}
	else
	{
		if( distance < pInfo->fDistance )
		{
			pInfo->nPolygonIndex	= collTriangle->index	;
			
			pInfo->vPos.x = intersection->t.line.start.x + (intersection->t.line.end.x - intersection->t.line.start.x) * distance;
			pInfo->vPos.y = intersection->t.line.start.y + (intersection->t.line.end.y - intersection->t.line.start.y) * distance;
			pInfo->vPos.z = intersection->t.line.start.z + (intersection->t.line.end.z - intersection->t.line.start.z) * distance;

			pInfo->fDistance	= distance;
		}

		return collTriangle;
	}
}

RpCollisionTriangle * AgcmMap::_IntersectionCallBackFindDistance
											(RpIntersection *intersection, RpCollisionTriangle *collTriangle,
											RwReal distance, void *data)
{
	PROFILE("AgcmMap::_IntersectionCallBackFindDistance");
	FLOAT	* pDistance = ( FLOAT * ) data;

	if( distance < *pDistance )
		*pDistance = distance;

	return collTriangle;
}

RpAtomic *	AgcmMap::_GetClumpCallback(
							 RpIntersection *	intersection	,
							 RpWorldSector *	sector			,
							 RpAtomic *			atomic			,
							 RwReal				distance		,
							 void *				data			)
{
	AuList< _FindClumpStruct >	* plistClump = ( AuList< _FindClumpStruct > * ) data;
	AuNode< _FindClumpStruct >	* pNode			;
	_FindClumpStruct			* pclumpstruct	;
	RpClump						* pClump		;

	INT32		index		;

	INT32		nType		= AcuObject::GetAtomicType( atomic , & index );

	// 아토믹의 타입이 월드섹터 일때만 등록..

	pClump = RpAtomicGetClump( atomic );
	// 마고자 (2004-03-18 오후 2:51:50) : 아토믹 만일경우 이 함수에서 체크하지 않는다.
	if( NULL == pClump ) return atomic;

	if( pClump && ( nType & ACUOBJECT_TYPE_TYPEFILEDMASK ) == ACUOBJECT_TYPE_NONE )
	{
		// 아토믹에 없으면..클럼프엔 있나 확인해야함..

		nType		= AcuObject::GetClumpType( pClump , & index );
	}

	static BOOL _sbUseDungeonObjectFilter = TRUE;
	if( _sbUseDungeonObjectFilter )
		if( g_nGetCursorClumpType != ( ACUOBJECT_TYPE_TYPEFILEDMASK & nType ) ||
			// 마고자 (2004-11-30 오전 11:14:57) : 던젼 오브젝트이면 튕겨냄 ..
			nType & ACUOBJECT_TYPE_DUNGEON_STRUCTURE	) return atomic;

	if( NULL == pClump ) return atomic;

	// 리스트에 pClump가 있는지 찾는다.
	pNode = plistClump->GetHeadNode();

	while( pNode )
	{
		pclumpstruct	= &pNode->GetData();
		if( pclumpstruct->pClump == pClump )
			break;

		plistClump->GetNext( pNode );
	}

	// 실제 아토믹의 거리를 찾는다..
	RpIntersection	atomic_intersection	;
	atomic_intersection.type	=	rpINTERSECTLINE			;
	atomic_intersection.t.line	=	intersection->t.line	;

	FLOAT			fRealDistance	= 1000000.0f;

	AGCMMAP_THIS->LockFrame();

	// 피킹 지오메트리로 처리한다..

	if( RpAtomicGetGeometry( atomic ) &&
		RpGeometryGetTriangles(RpAtomicGetGeometry( atomic ) ) )
	{
		RpAtomicForAllIntersections (
			atomic								,
			&atomic_intersection				,
			AgcmMap::_IntersectionCallBackFindDistance	,
			( void * ) &fRealDistance			);
	}

	AGCMMAP_THIS->UnlockFrame();
	// 콜리젼 데이타 초기화
	
	RwV3d	deltaVector;
	RwV3dSub( &deltaVector , &intersection->t.line.start , &intersection->t.line.end );
	FLOAT	fDistance	= RwV3dLength( &deltaVector ) * fRealDistance ;

	if( fRealDistance == 1000000.0f )
		return atomic;

	// 있는거라면 distance업데이트
	if( pNode )
	{
		// Found!
		if( fDistance < pclumpstruct->distance )
			pclumpstruct->distance = fDistance;
	}
	else
	{
		// 없으면 리스트에 추가.
		_FindClumpStruct	cs;
		cs.distance	= fDistance	;
		cs.pClump	= pClump	;

		plistClump->AddTail( cs );
	}

	return atomic;
}


RpAtomic *	AgcmMap::_GetClumpPickingOnlyCallback(
							 RpIntersection *	intersection	,
							 RpWorldSector *	sector			,
							 RpAtomic *			atomic			,
							 RwReal				distance		,
							 void *				data			)
{
	AuList< _FindClumpStruct >	* plistClump = ( AuList< _FindClumpStruct > * ) data;
	AuNode< _FindClumpStruct >	* pNode			;
	_FindClumpStruct			* pclumpstruct	;
	RpClump						* pClump		;

	INT32		index		;

	INT32		nType		= AcuObject::GetAtomicType( atomic , & index );

	// 아토믹의 타입이 월드섹터 일때만 등록..

	pClump = RpAtomicGetClump( atomic );
	if( NULL == pClump ) return atomic;

	if( pClump && nType == ACUOBJECT_TYPE_NONE )
	{
		// 아토믹에 없으면..클럼프엔 있나 확인해야함..

		nType		= AcuObject::GetClumpType( pClump , & index );
	}

	if( g_nGetCursorClumpType != ( ACUOBJECT_TYPE_TYPEFILEDMASK & nType ) ||
		// 마고자 (2004-11-30 오전 11:14:57) : 던젼 오브젝트이면 튕겨냄 ..
		nType & ACUOBJECT_TYPE_DUNGEON_STRUCTURE	) return atomic;

	if( NULL == pClump ) return atomic;

	// 리스트에 pClump가 있는지 찾는다.
	pNode = plistClump->GetHeadNode();

	while( pNode )
	{
		pclumpstruct	= &pNode->GetData();
		if( pclumpstruct->pClump == pClump )
			break;

		plistClump->GetNext( pNode );
	}

	// 실제 아토믹의 거리를 찾는다..
	RpIntersection	atomic_intersection	;
	atomic_intersection.type	=	rpINTERSECTLINE			;
	atomic_intersection.t.line	=	intersection->t.line	;

	FLOAT			fRealDistance	= 1000000.0f;

	RpAtomic	*	pPickingGeometry	= (RpAtomic*)pClump->stType.pPickAtomic;
	if( NULL == pPickingGeometry ) return atomic;

	// 피킹 지오메트리로 처리한다..

	if( RpAtomicGetGeometry( pPickingGeometry ) &&
		RpGeometryGetTriangles(RpAtomicGetGeometry( pPickingGeometry )))
	{
		AGCMMAP_THIS->LockFrame();

		RpAtomicForAllIntersections (
			pPickingGeometry					,
			&atomic_intersection				,
			AgcmMap::_IntersectionCallBackFindDistance	,
			( void * ) &fRealDistance			);

		AGCMMAP_THIS->UnlockFrame();
	}
	// 콜리젼 데이타 초기화
	
	RwV3d	deltaVector;
	RwV3dSub( &deltaVector , &intersection->t.line.start , &intersection->t.line.end );
	FLOAT	fDistance	= RwV3dLength( &deltaVector ) * fRealDistance ;

	if( fRealDistance == 1000000.0f )
		return atomic;

	// 있는거라면 distance업데이트
	if( pNode )
	{
		// Found!
		if( fDistance < pclumpstruct->distance )
			pclumpstruct->distance = fDistance;
	}
	else
	{
		// 없으면 리스트에 추가.
		_FindClumpStruct	cs;
		cs.distance	= fDistance	;
		cs.pClump	= pClump	;

		plistClump->AddTail( cs );
	}

	return atomic;
}

static RpAtomic * _GetClumpHeightCallback(RpAtomic * pAtomic, void *data)
{
	_FindHeightStruct	*		pHeightStruct	= ( _FindHeightStruct * ) data;
	FLOAT			fRealDistance	= 1000000.0f;

	RpGeometry * pGeometry = RpAtomicGetGeometry( pAtomic );
	
	// 인스턴싱 데이타인지 체크.
	if( NULL == pGeometry							) return pAtomic;
	if( NULL == RpGeometryGetTriangles( pGeometry )	) return pAtomic;

	// 아니라면 콜리젼 때림.
	if( RpAtomicGetGeometry( pAtomic ) &&
		RpGeometryGetTriangles(RpAtomicGetGeometry( pAtomic )))
	{
		AGCMMAP_THIS->LockFrame();

		RpAtomicForAllIntersections (
			pAtomic										,
			pHeightStruct->pIntersection				,
			AgcmMap::_IntersectionCallBackFindDistance	,
			( void * ) &fRealDistance					);

		AGCMMAP_THIS->UnlockFrame();
	}
	// 콜리젼 데이타 초기화
	
	FLOAT	fDistance	= pHeightStruct->pIntersection->t.line.start.y - ( pHeightStruct->pIntersection->t.line.start.y - pHeightStruct->pIntersection->t.line.end.y ) * fRealDistance ;

	if( fRealDistance == 1000000.0f )
		return	pAtomic;

	if( pHeightStruct->fHeight	< fDistance							&&
		fDistance				< pHeightStruct->fCharacterHeight	)
	{
		pHeightStruct->fHeight				= fDistance	;
		// 이까지오면 올라탈 수 있는 오브젝트가 있음.
		pHeightStruct->bExistRidableObject	= TRUE		;
	}

	return	pAtomic;
}

#define MAP(x,y)			( VertexList	[ ( y ) * ( MAP_WIDTH + 1 ) + ( x ) ] )
#define PRELIGHT(x,y)		( pPreLightList	[ ( y ) * ( MAP_WIDTH + 1 ) + ( x ) ] )

RpCollisionTriangle *AgcmMap::CollisionAtomicSectorCallback(
							RpIntersection		*	pstIntersection	,
							RpCollisionTriangle	*	pstCollTriangle	,
							RwReal					fDistance		, 
							void				*	pvData			)
{
	FLOAT	*pfHeight = ( FLOAT * ) pvData;
	*pfHeight = pstIntersection->t.line.start.y + (pstIntersection->t.line.end.y - pstIntersection->t.line.start.y) * fDistance;
	return NULL;
}

inline	void _SetPolygonVertex( RpTriangle * pTriange , INT32 baseindex , INT32 vert1 , INT32 vert2 , INT32 vert3 , INT32 matindex )
{
	pTriange->vertIndex[0]		= baseindex + vert1		;
	pTriange->vertIndex[1]		= baseindex + vert2		;
	pTriange->vertIndex[2]		= baseindex	+ vert3		;
	pTriange->matIndex			= matindex				;
}

inline INT32	__GetRequiredVertexCount		( INT32 nType , INT32 nDepth )
{
	switch( nType )
	{
	case	R_LEFTTOP			:
	case	R_RIGHTTOP		:
	case	R_LEFTBOTTOM		:
	case	R_RIGHTBOTTOM		:
		return 4 + ( nDepth - 1 ) * 2;
	case	R_TOP				:
	case	R_LEFT			:
	case	R_RIGHT			:
	case	R_BOTTOM			:
		return 4 + ( nDepth - 1 );

	case	R_CENTER			:
		return 4;
	}
	
	return 0;
}

inline INT32	__GetRequiredPolugonCount	( INT32 nType , INT32 nDepth )
{
	switch( nType )
	{
	case	R_LEFTTOP			:
	case	R_RIGHTTOP		:
	case	R_LEFTBOTTOM		:
	case	R_RIGHTBOTTOM		:
		return 2 + ( nDepth - 1 ) * 2;
	case	R_TOP				:
	case	R_LEFT			:
	case	R_RIGHT			:
	case	R_BOTTOM			:
		return 2 + ( nDepth - 1 );

	case	R_CENTER			:
		return 2;
	}

	return 0;
}

inline INT32	__GetPolygonType				( INT32 x , INT32 y , INT32 xMax , INT32 yMax )
{
	ASSERT( x >= 0 && x <= xMax );
	ASSERT( y >= 0 && y <= yMax );
	
	if( y == 0 )
	{
		if( x == 0		)	return R_LEFTTOP		;
		else
		if( x == xMax	)	return R_RIGHTTOP		;
		else				return R_TOP			;
	}
	else if( y == yMax )
	{
		if( x == 0		)	return R_LEFTBOTTOM		;
		else
		if( x == xMax	)	return R_RIGHTBOTTOM	;
		else				return R_BOTTOM			;
	}
	else
	{
		if( x == 0		)	return R_LEFT			;
		else
		if( x == xMax	)	return R_RIGHT			;
		else				return R_CENTER			;
	}

	ASSERT( !"-_-?????????????" );
	return R_TYPETOTAL;	// ERROR
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4355)
AgcmMap::AgcmMap()
{
#pragma warning(default:4355)

	SetModuleName("AgcmMap");

	EnableIdle(TRUE);

	// 이 Module은 Datamodule이다.
	m_uType = AgcModule::DATAMODULE;

	m_bInited	= TRUE		;
	m_bInitialized = FALSE	;

	// 섹터 포인터 초기화..

	m_nGeometryLockMode		= rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKNORMALS	;
	m_nVertexColorLockMode	= rpGEOMETRYLOCKPRELIGHT							;

	m_pWorld				= NULL												;
//	m_pcsAgcmObject			= NULL												;

	m_ulPrevLoadTime = 0;

	// 반지름임..
	m_nMapDataLoadRadius	= 5	;
	m_nRoughLoadRadius		= 4	;
	m_nDetailLoadRadius		= 3	;

	m_nMapDataReleaseRadius	= ( INT32 ) ( ( FLOAT ) 5	* 1.5f );
	m_nRoughReleaseRadius	= ( INT32 ) ( ( FLOAT ) 4	* 1.5f );	// fRate 범위만큼 넓게 잡는다.
	m_nDetailReleaseRadius	= ( INT32 ) ( ( FLOAT ) 3	* 1.5f );

	m_bUseTexDict = FALSE;
	
	m_pfSectorWaterLoadCallback				= NULL;
	m_pfSectorWaterDestroyCallback			= NULL;
	m_pfGetCollisionAtomicCallback			= NULL;
	m_pfOnFirstMyCharacterPositionCallback	= NULL;

	m_pVertexList	= NULL	;
	m_pPreLightList	= NULL	;

	m_pSelfCharacterSector = NULL;

	m_nSectorAttachedDataOffset = -1;

	m_ulCurTick = 0;
	
	m_lMapLoadEndCBMode	=	AGCMMAP_MAPLOADEND_CB_MODE_OFF	;

	m_posCurrentAvatarPosition.x	=	0;
	m_posCurrentAvatarPosition.y	=	0;
	m_posCurrentAvatarPosition.z	=	0;
	m_bAvatarPositionSet			=	FALSE;
	
	m_pcsApmMap						= NULL;
	m_pcsAgcmRender					= NULL;
	m_pcsAgcmResourceLoader			= NULL;
	m_pcsAgcmOcTree					= NULL;
	m_pcsApmOcTree					= NULL;
	strcpy( m_strToolImagePath , "" );
	m_bFirstPosition				= FALSE;

	m_bRangeApplied	= FALSE;
	m_nLoadRangeX1	= 0;
	m_nLoadRangeZ1	= 0;
	m_nLoadRangeX2	= 0;
	m_nLoadRangeZ2	= 0;

	m_lSectorsToLoad = 0;

	m_szTexturePath[0] = 0;
	m_nZoneLoadDivision	= 0;

	m_bShowZoneRough	= TRUE;

	#ifdef USE_MFC
	m_uTiled	= 0;
	m_uReplace	= 0;
	#endif // USE_MFC

	m_lLoaderChangeID	= -1;
	m_lLoaderDestroyID	= -1;


	m_nLoadLockCount	=0;
}

AgcmMap::~AgcmMap()
{
	if( m_pVertexList	) delete [] m_pVertexList	;
	if( m_pPreLightList	) delete [] m_pPreLightList	;
}

BOOL	AgcmMap::OnAddModule	()
{
	//@{ Jaewon 20050420
	// TODO: Fix this...==
	_RwD3D9RasterExtOffset = RwRasterGetPluginOffset(rwID_DEVICEMODULE);
	//@} Jaewon

	// 모듈 등록될때 일어나는 이벤트.
	m_pcsApmMap		= ( ApmMap *	) GetModule( "ApmMap"		);

	ASSERT( NULL != m_pcsApmMap );

	if( !m_pcsApmMap )
		return FALSE;

	m_pcsAgcmRender	= ( AgcmRender *	) GetModule( "AgcmRender"	);

	m_pcsAgcmResourceLoader = ( AgcmResourceLoader *) GetModule( "AgcmResourceLoader" );

	m_pcsAgcmOcTree	= (AgcmOcTree	*) GetModule("AgcmOcTree");

	m_pcsApmOcTree = (ApmOcTree	*) GetModule("ApmOcTree");
	
	// CullMode set
	if ( m_pcsAgcmRender)
		m_bUseCullMode = m_pcsAgcmRender->m_bUseCullMode;

	if (m_pcsAgcmResourceLoader)
	{
		m_lLoaderChangeID = m_pcsAgcmResourceLoader->RegisterLoader(this, CBMyCharacterPositionChange, CBMyCharacterPositionChange2);
		if (m_lLoaderChangeID < 0)
			return FALSE;

		m_lLoaderDestroyID = m_pcsAgcmResourceLoader->RegisterLoader(this, CBDestroySector_Start, CBDestroySector_End);
		if (m_lLoaderDestroyID < 0)
			return FALSE;
	}

	// 필요한 어테치 데이타들은 여기에..

	// 퍼블릭 모듈에 콜백등록..
	m_pcsApmMap->SetSectorCallback( CallBackMapDataLoad , CallBackMapDataDestroy );
	
	// 섹터 모듈 데이타 등록.
	m_nSectorAttachedDataOffset = 
		m_pcsApmMap->AttachSectorData(
		this							,
		sizeof ( AgcmMapSectorData )	, 
		_AgcmMapSectorDataConstructor	,
		_AgcmMapSectorDataDestructor	);

	// 마고자 (2005-03-23 오후 3:51:05) : 콜백 지정..
	#ifdef USE_MFC
	m_pcsApmMap->SetCallbackSaveGeometry	( CBSaveGeometry	, this );
	m_pcsApmMap->SetCallbackLoadGeometry	( CBLoadGeometry	, this );
	m_pcsApmMap->SetCallbackSaveMoonee		( CBSaveMoonee		, this );
	m_pcsApmMap->SetCallbackLoadMoonee		( CBLoadMoonee		, this );
	#endif USE_MFC

	if(m_pcsAgcmRender)
		m_pcsAgcmRender->SetCallbackMTextureRender( CBMTextureRender , this );

	SetCamera(GetCamera());

	return TRUE;
}

BOOL	AgcmMap::OnInit			()
{
	// 초기화 이벤트.

	// Thread로 만들었던 거를 OnIdle로 빼자
	/*
	if (!m_csMapLoader.StartThread())
		return FALSE;
	*/

	m_pcsApmMap->LoadRegionPerculiarity( TRUE );

	
	AS_REGISTER_TYPE_BEGIN(AgcmMap, AgcmMap);
		AS_REGISTER_METHOD0(void, ShowAll);
		AS_REGISTER_METHOD0(void, ToggleZoneRough);
	AS_REGISTER_TYPE_END;

	return TRUE;
}

void	AgcmMap::ToggleZoneRough()
{
	m_bShowZoneRough = !m_bShowZoneRough;

	if( m_bShowZoneRough )
	{
		print_ui_console( _T("존로딩시 러프맵을 표시합니다") );
	}
	else
	{
		print_ui_console( _T("존로딩시 러프맵을 표시하지않습니다") );
	}
}

void	AgcmMap::ShowAll()
{
	SetCurrentDetail( SECTOR_HIGHDETAIL , TRUE );
}

BOOL	AgcmMap::OnDestroy		()
{
	// 종료 이벤트.
//	FreeUndoManager		();

	// 이제 Thread 안쓰고, OnIdle에서 Load한다.
	//m_csMapLoader.StopThread();

//	ApWorldSector * pSector;
//
//	if( IsRangeApplied() )
//	{
//		for( int	x = ( GetLoadRangeX1() > 1 ? ( GetLoadRangeX1() - 1 ) : GetLoadRangeX1()	) ;
//					x < ( GetLoadRangeX2() < MAP_WORLD_INDEX_WIDTH - 1 ? ( GetLoadRangeX2() + 1 ) : GetLoadRangeX2()	);
//					x ++																		)
//		{
//			for( int	z = ( GetLoadRangeZ1() > 1 ? ( GetLoadRangeZ1() - 1 ) : GetLoadRangeZ1()	);
//						z < ( GetLoadRangeZ2() < MAP_WORLD_INDEX_WIDTH - 1 ? ( GetLoadRangeZ2() + 1 ) : GetLoadRangeZ2()	);
//						z ++																		)
//			{
//				pSector = m_pcsApmMap->GetSector( x	, 0	, z );
//
//				if( m_ppDWSector[ x ][ z ] && NULL == pSector )
//				{
//					VERIFY( m_pcsApmMap->DeleteSector( pSector ) );
//				}
//			}
//		}	
//	}

	m_TextureList.DeleteAll();
	
	return TRUE;
}

BOOL	AgcmMap::OnIdle			( UINT32 ulClockCount	)
{
	/*
	if (ulClockCount - m_ulPrevLoadTime >= 0 && !m_csMapLoader.IsEmpty())
	{
		//m_csMapLoader.OnIdleLoad();
		m_ulPrevLoadTime = ulClockCount;
	}


	// Map Load End CB관련 
	if ( AGCMMAP_MAPLOADEND_CB_MODE_LOAD_START == m_lMapLoadEndCBMode )
	{
		if ( m_csMapLoader.IsEmpty() )
		{
			EnumCallback( AGCM_CB_ID_MAP_LOAD_END, NULL, NULL);
			m_lMapLoadEndCBMode = AGCMMAP_MAPLOADEND_CB_MODE_OFF;	
		}
	}
	*/

	m_ulCurTick = ulClockCount;

	return TRUE;
}

VOID	AgcmMap::SetTexturePath		( CHAR *szTexturePath				)
{
	m_szTexturePath[0] = 0;
	strncat(m_szTexturePath, szTexturePath, 127);
}

BOOL	AgcmMap::SetTexDict			( char *pTexDictFileName			)
{
	m_bUseTexDict = TRUE;

	strncpy(m_TextureList.m_strTextureDictionary, pTexDictFileName, 255);
	m_TextureList.m_strTextureDictionary[255] = 0;

	return TRUE;
}

BOOL	AgcmMap::SetCamera				(RwCamera * pCamera)
{
	m_pstCamera = pCamera;

	return TRUE;
}

BOOL	AgcmMap::SetDataPath			( char * pInitToolImagePath			)
{
	ASSERT( NULL != pInitToolImagePath	);
	ASSERT( FALSE == IsBadStringPtr( pInitToolImagePath , 256 )	);
	strncpy( m_strToolImagePath , pInitToolImagePath , 256 );
	return TRUE;
}

BOOL	AgcmMap::InitMapClientModule( RpWorld * pWorld  )
{
	ASSERT( NULL != pWorld		);
	ASSERT( FALSE == IsInitialized() );

	// 초기화 이벤트.

	// 월드 섹터 검사해서 섹터 포인터 정리함..
	//RpWorldForAllWorldSectors( pWorld , FindSectorCallback , ( void * ) this );

	// 최초의 포지션 초기화..
	m_bFirstPosition	= TRUE;

	SetInitialize();

	// 월드 섹터가 제대로 들어가있는지 확인..

	// 월드 포인터 저장함..
	m_pWorld = pWorld;

	return TRUE;
}

BOOL	AgcmMap::InitMapToolData		( char *pTextureScriptFileName , char *pObjectScripFileName )
{
	ASSERT( NULL != pTextureScriptFileName	);
	ASSERT( IsInitialized()					);

	// 텍스쳐리스트 , Mat List 초기화...
	m_TextureList.m_strTextureDirectory	= m_strToolImagePath;
	m_TextureList.LoadScript( pTextureScriptFileName );
	
	//RpDWSector * pDWSector;

	// Mat List 에 텍스쳐 리스트 포인터 설정..

	AcMaterialList::m_pTextureList	= &m_TextureList;

	m_pVertexList	= new RwV3d		[ ( ( MAP_DEFAULT_DEPTH + 1 ) * ( MAP_DEFAULT_DEPTH + 1	) ) ];
	m_pPreLightList	= new RwRGBA	[ ( ( MAP_DEFAULT_DEPTH + 1 ) * ( MAP_DEFAULT_DEPTH + 1	) ) ];

	return TRUE;
}

void	AgcmMap::InitMapToolLoadRange( int x1 , int z1 , int x2 , int z2 )
{
	// ASSERT( m_bRangeApplied == FALSE );
	m_bRangeApplied = TRUE;

	m_nLoadRangeX1	= x1;
	m_nLoadRangeZ1	= z1;
	m_nLoadRangeX2	= x2;
	m_nLoadRangeZ2	= z2;

	// add by gemani(04.0303)
	m_pcsAgcmRender->m_iLoadRangeX1 = x1;
	m_pcsAgcmRender->m_iLoadRangeX2 = x2;
	m_pcsAgcmRender->m_iLoadRangeZ1 = z1;
	m_pcsAgcmRender->m_iLoadRangeZ2 = z2;
}


void	AgcmMap::MyCharacterPositionChange( AuPOS prevpos , AuPOS pos , BOOL bForceReload )
{
	PROFILE("AgcmMap::MyCharacterPositionChange()");

	 // 좌표가 이동했을경우만
	if( prevpos == pos && !m_bFirstPosition )
	{
		m_posCurrentAvatarPosition = prevpos;
		return;
	}

	m_bAvatarPositionSet					= TRUE	;

	// Map Load End Callback Mode Setting
	if ( AGCMMAP_MAPLOADEND_CB_MODE_ON == m_lMapLoadEndCBMode )
		m_lMapLoadEndCBMode = AGCMMAP_MAPLOADEND_CB_MODE_LOAD_START;

	static	BOOL	bRealFirst				= TRUE	;
	BOOL			bCallFirstLoadCallback	= FALSE	;
	BOOL			bLoadMainThread			= FALSE	;

	// 잠시 prevpos 를 사용하지 않음..
	prevpos = m_posCurrentAvatarPosition;

	// 위치 얻어냄..
	ApWorldSector * pPrevSector = m_pcsApmMap->GetSector( prevpos	);
	ApWorldSector * pNextSector = m_pcsApmMap->GetSector( pos		);

	// 두 섹터가 로딩 돼어있지 않으면 로딩 시킨다.
	if( NULL == pPrevSector )
	{
		PROFILE("AgcmMap::MyCharacterPositionChange() - LoadSector");

		pPrevSector = LoadSector( 
					PosToSectorIndexX( prevpos.x )			,
					0										,
					PosToSectorIndexX( prevpos.z )			);
		// Callback으로 OnLoadSector 호출됨..
	}	

	if( NULL == pNextSector )
	{
		TRACE("Load Next Sector %d,%d !!!!!!!!!1\n", PosToSectorIndexX( pos.x ), PosToSectorIndexZ( pos.z ));

		PROFILE("AgcmMap::MyCharacterPositionChange() - LoadSector");

		pNextSector = LoadSector( 
					PosToSectorIndexX( pos.x )				,
					0										,
					PosToSectorIndexZ( pos.z )				);
		// Callback으로 OnLoadSector 호출됨..
	}	

	m_posCurrentAvatarPosition = pos;

//	ASSERT( pPrevSector || pNextSector );
	if( !pPrevSector && !pNextSector )
	{
		// 말이돼!?
		return;
	}

	// 강제 로딩이면 , 체크하지 않음..
	if(	!bForceReload											&&
		pPrevSector && pNextSector								&&
		pPrevSector->GetIndexX()	== pNextSector->GetIndexX()	&&
		pPrevSector->GetIndexZ()	== pNextSector->GetIndexZ()	&&
		FALSE						== m_bFirstPosition			)
	{
		// 아무것도 로딩하지 않음..
		return;
	}
	else if
	(
		pPrevSector && pNextSector								&&
		m_bFirstPosition										&&
		pPrevSector->GetIndexX()	== pNextSector->GetIndexX()	&&
		pPrevSector->GetIndexZ()	== pNextSector->GetIndexZ()
	)
	{
		// 처음..
		m_bFirstPosition	= FALSE	;
//		if( bRealFirst )
		{
			pPrevSector		= NULL	;
			bRealFirst		= FALSE	;
			bCallFirstLoadCallback	= TRUE;
		}
		// 이것만 해주면 돼겠지..
	}

	m_pSelfCharacterSector = pNextSector;

	EnumCallback( AGCM_CB_ID_MAP_SECTOR_CHANGE_PRE, pNextSector, pPrevSector );

	// 존로딩 처리..
	if( CheckSelfCharacterRegionChange( &prevpos , &m_posCurrentAvatarPosition ) )
	{
		// 존로딩 상태에서만 이쪽을 타게 됨..

		// 로딩을 하지 않는다.

		if( m_pSelfCharacterSector == pNextSector )
		{
			EnumCallback( AGCM_CB_ID_MAP_LOAD_END, pNextSector, NULL);
		}

		// 로딩 기다리기
		while( m_csMapLoader.GetWaitCount()  )
		{
			m_csMapLoader.OnIdleLoad();
		}

		// First Load Callback.
		if( bCallFirstLoadCallback && m_pfOnFirstMyCharacterPositionCallback )
			m_pfOnFirstMyCharacterPositionCallback();

		// 마고자 : 04/03/23 요 위에 사이에 섹터 데이타가 사라지는 경우가 있는거 같다.
		// 그래서 한번더 얻는 작업을 해줌.
		// 이쪽에서 죽는 경우가 있어서 슬쩍 체크해둠.. <네통아저씨 요청>
	}
	else
	{
		// 존로딩이 아닐경우..
		if (m_pcsAgcmResourceLoader)
		{
			m_pcsAgcmResourceLoader->RemoveAllEntry( m_lLoaderChangeID );
			m_pcsAgcmResourceLoader->AddLoadEntry( m_lLoaderChangeID, pPrevSector, pNextSector);
		}
		else
		{
			CBMyCharacterPositionChange		( pPrevSector , this , pNextSector );
			CBMyCharacterPositionChange2	( pPrevSector , this , pNextSector );
		}

		// First Load Callback.
		if( bCallFirstLoadCallback && m_pfOnFirstMyCharacterPositionCallback )
			m_pfOnFirstMyCharacterPositionCallback();		 
	}

	pPrevSector = m_pcsApmMap->GetSector( prevpos	);
	pNextSector = m_pcsApmMap->GetSector( pos		);

	if(pNextSector != NULL)
	{
		m_pcsApmOcTree->m_pCenterSector = pNextSector;
		
		EnumCallback( AGCM_CB_ID_MAP_SECTOR_CHANGE, pNextSector, pPrevSector );		
	}

	TRACE( "현재 로드되어있는 섹터수 %d\n" , m_pcsApmMap->GetCurrentLoadedSectorCount() );
}

BOOL	AgcmMap::OnLoadSector		( ApWorldSector * pSector )
{
	ASSERT( NULL != pSector		);
	//ASSERTONCE( pSector->IsLoadedCompactData() || pSector->IsLoadedDetailData() );

	return TRUE;
}

BOOL	AgcmMap::OnLoadRough	( ApWorldSector * pSector )
{
	ASSERT( NULL != pSector		);
	ASSERT( NULL != m_pcsApmMap	);
	// 지형 오토로딩..
	if( m_pcsApmMap->IsAutoLoadData() )
	{
		// 지형 데이타 로딩..
		if( LoadDWSector( pSector , SECTOR_LOWDETAIL ) )
		{
			if (!m_pcsAgcmResourceLoader)
				SetCurrentDetail( pSector , SECTOR_LOWDETAIL , TRUE );

			/*
			//오브젝트 로딩함..
			if( m_pcsApmObject )
				m_pcsApmObject->StreamRead( pSector );
			*/

			if( m_pfSectorWaterLoadCallback )
				m_pfSectorWaterLoadCallback( pSector , SECTOR_LOWDETAIL );

			EnumCallback(AGCM_CB_ID_ONLOAD_MAP, pSector, (PVOID) SECTOR_LOWDETAIL);
		}
		else
		{
			TRACE( "AgcmMap::OnLoadRough 이미 로딩돼어있심!\n" );
		}
	}
	else
	{
		if (!m_pcsAgcmResourceLoader)
			SetCurrentDetail( pSector , SECTOR_LOWDETAIL , TRUE );
	}

	return TRUE;
}

BOOL	AgcmMap::OnLoadDetail	( ApWorldSector * pSector )
{
	ASSERT( NULL != pSector		);
	ASSERT( NULL != m_pcsApmMap	);
//	ASSERT( NULL != m_pcsApmObject	);
	// 지형 오토로딩..
	if( m_pcsApmMap->IsAutoLoadData() )
	{
		// 지형 데이타 로딩..
		if( LoadDWSector( pSector , SECTOR_HIGHDETAIL ) )
		{
			if (!m_pcsAgcmResourceLoader)
				SetCurrentDetail( pSector , SECTOR_HIGHDETAIL , TRUE );

			if( m_pfSectorWaterLoadCallback )
				m_pfSectorWaterLoadCallback( pSector , SECTOR_HIGHDETAIL );

			EnumCallback(AGCM_CB_ID_ONLOAD_MAP, pSector, (PVOID) SECTOR_HIGHDETAIL);
		}
		else
		{
			TRACE( "AgcmMap::OnLoadDetail 이미 로딩돼어있심!\n" );
		}

	}
	else
	{
		if (!m_pcsAgcmResourceLoader)
			SetCurrentDetail( pSector , SECTOR_HIGHDETAIL , TRUE );
	}

	return TRUE;
}


BOOL	AgcmMap::OnDestorySector	( ApWorldSector * pSector )
{
	ASSERT( NULL != m_pcsApmMap		);
	ASSERT( NULL != pSector			);

	// 섹터가 없어지기 전!

	
	if( m_pcsApmMap->IsAutoLoadData() )
	{
		// 로딩 큐에서 섹터 내용 제거함..
		m_csMapLoader.RemoveQueue( pSector );

		// 이것은 확인사살 이라고함.
		// DWSector 처리..
		// 폴리건 정보 제거..
		VERIFY( ReleaseDWSector	( pSector ) );
	}
	else
	{


	}
	
	return TRUE;
}

BOOL	AgcmMap::OnDestroyRough		( ApWorldSector * pSector )
{
	ASSERT( NULL != m_pcsApmMap		);
	ASSERT( NULL != pSector			);

	PROFILE("AgcmMap::OnDestroyRough");

	RpDWSectorSetCurrentDetail( pSector , SECTOR_EMPTY );

	if( m_pcsApmMap->IsAutoLoadData() )
	{
		PROFILE("AgcmMap::OnDestroyRough - RpDWSectorDestroyDetail");
		// 폴리건 정보 제거..
		if( RpDWSectorDestroyDetail	( pSector , SECTOR_LOWDETAIL ) )
		{
			/*
			// 오브젝트 제거함..
			if( m_pcsApmObject )
				m_pcsApmObject->DeleteObject( pSector );
			*/
			// 워러!.
			{
			PROFILE("AgcmMap::OnDestroyRough - Water");
			if( m_pfSectorWaterDestroyCallback ) m_pfSectorWaterDestroyCallback( pSector , SECTOR_LOWDETAIL );
			}

			{
			PROFILE("AgcmMap::OnDestroyRough - AGCM_CB_ID_ONUNLOAD_MAP");
			m_pcsAgcmResourceLoader->AddLoadEntry( m_lLoaderDestroyID, pSector, NULL);
			}
		}
	}
	else
	{

	}

	return TRUE;
}

BOOL	AgcmMap::OnDestroyDetail	( ApWorldSector * pSector )
{
	ASSERT( NULL != m_pcsApmMap		);
	ASSERT( NULL != pSector			);
//	ASSERT( NULL != m_pcsApmObject	);

	PROFILE("AgcmMap::OnDestroyDetail");

	RpDWSectorSetCurrentDetail	( pSector , SECTOR_LOWDETAIL	);

	if( m_pcsApmMap->IsAutoLoadData() )
	{
		PROFILE("AgcmMap::OnDestroyDetail - RpDWSectorDestroyDetail");
		// 폴리건 정보 제거..
		if( RpDWSectorDestroyDetail		( pSector , SECTOR_HIGHDETAIL	) )
		{
			{
			PROFILE("AgcmMap::OnDestroyDetail - Water");
			if( m_pfSectorWaterDestroyCallback ) m_pfSectorWaterDestroyCallback( pSector , SECTOR_HIGHDETAIL );
			}

			{
			PROFILE("AgcmMap::OnDestroyDetail - AGCM_CB_ID_ONUNLOAD_MAP");
			EnumCallback(AGCM_CB_ID_ONUNLOAD_MAP, pSector, (PVOID) SECTOR_HIGHDETAIL);
			}
		}
	}
	else
	{
		
	}
	return TRUE;
}


RpWorld	*	AgcmMap::GetWorld			( void )
{
	RpWorld *	pWorld	;
	RwBBox		bb		;

	bb.inf.x = bb.inf.y = bb.inf.z = -1000000.0f	;
	bb.sup.x = bb.sup.y = bb.sup.z = 1000000.0f	;

	pWorld = RpWorldCreate(&bb);

	return pWorld;
//	ASSERT( NULL != m_pcsApmMap );
//
//	// 더미월드를 생성.
//    RtWorldImport *			worldImport	;
//    RtWorldImportParameters	params		;
//    RpWorld *				world		;
//
//	TRACE( "WorldImport 데이타를 작성합니다.\n" );
//    worldImport = CreateWorldImport();
//    if( worldImport == NULL )
//    {
//        return NULL;
//    }
//
//    RtWorldImportParametersInitialize(&params);
//
//    params.flags = rpWORLDTEXTURED | rpWORLDNORMALS | rpWORLDLIGHT | rpWORLDTEXTURED2 | rpWORLDPRELIT;
//    params.conditionGeometry		= FALSE	;
//    params.calcNormals				= FALSE	;
//	params.numTexCoordSets			= 4		; // 멀티텍스쳐 지원.
//	params.maxWorldSectorPolygons	= 2		;
//	params.maxOverlapPercent		= 1		;
//
//	/*
//	params.userSpecifiedBBox		= 1		;
//	params.userpBBox->inf.x			= -100000.0;
//	params.userpBBox->inf.y			= -1000.0;
//	params.userpBBox->inf.z			= -100000.0;
//	params.userpBBox->sup.x			= 100000.0;
//	params.userpBBox->sup.y			= 1000.0;
//	params.userpBBox->sup.z			= 100000.0;
//	params.weldPolygons			= FALSE	;
//	params.weldThreshold		= 1	;
//	params.retainCreases 	 	= TRUE	;
//	params.fixTJunctions	 	= FALSE	;
//	*/
//
//	/*
//	RwInt32 numOfCandidates = 10;
//	RwReal sizeOfSector = 2000;
//	RtWorldImportSetBuildCallBacks( MySelector, MyTerminator );
//	RtWorldImportSetBuildCallBacksUserData((void *) &numOfCandidates, (void *) &sizeOfSector);
//	*/
//
//	TRACE( "WorldImport 를 통해 월드를 생성합니다.\n" );
//    world = RtWorldImportCreateWorld(worldImport, &params);
//    if( world )
//    {
//		RpCollisionWorldBuildData( world , NULL );
//    }
//
//	ASSERT( NULL != world );
//	if( NULL == world ) return world;
//
//    RtWorldImportDestroy(worldImport);
//
//	TRACE( "나뉘어진 섹터를 관리할 구조체를 형성합니다.\n" );
//	// DWSector 설정.
//	//RpDWSectorInitWorld				( world , SECTOR_DETAILDEPTH , 0	);
//	//RpDWSectorBuildWorldCollision	( world								);
//
//	// 각 월드 세부 지형 로드함.
//	// 데이타를 이용해서 지형을 추가한다.
//	// 마고자 (2002-07-12 오후 2:20:20) : 
//
//	return world;
}

int			AgcmMap::GetCurrentDetail	( ApWorldSector * pSector	)
{
	return RpDWSectorGetCurrentDetail( pSector );
}

void		AgcmMap::MakeDetailWorld	( ApWorldSector * pSector , int nDetail		)	// 해당섹터에 디테일 맵 정보를 생성한다.
{
	#ifdef USE_MFC

	// 섹터에 데이타 넣기.
	ASSERT( NULL != m_pcsApmMap										);
	ASSERT( NULL != pSector											);
	ASSERT( SECTOR_EMPTY <= nDetail && nDetail < SECTOR_DETAILDEPTH	);

	ApDetailSectorInfo	* pToolSectorInfo	= pSector->GetDetailSectorInfo()		;
	if( pToolSectorInfo == NULL )
	{
		TRACE( "툴용 지형 저보가 없습니다.\n" );
		return;
	}

	int					nDepth			= pToolSectorInfo->m_nDepth			[ nDetail ];
	ApDetailSegment *	pDetailSegment	= pToolSectorInfo->m_pSegment		[ nDetail ];
	AcMaterialList *	pMaterialList	= GetMaterialList( pSector );

	//ASSERT( NULL != pDetailSegment			);
	//if( NULL == pDetailSegment ) return;

	FLOAT	xStart	= pSector->GetXStart();
	FLOAT	xEnd	= pSector->GetXEnd	();
	FLOAT	zStart	= pSector->GetZStart();
	FLOAT	zEnd	= pSector->GetZEnd	();

	/*
	float	pSecStepSizeX = pSector->GetStepSizeX( nDetail );
	float	pSecStepSizeZ = pSector->GetStepSizeZ( nDetail );
	*/
	_MaptoolClientSaveData * pSaveData;

	switch( nDetail )
	{
	case SECTOR_EMPTY		:
		{
			// 만들어져있는데 왜 --;
			if( nDepth == 0 || pDetailSegment == NULL ) break;
		}
		break;
	case SECTOR_LOWDETAIL	:
		// Rough nDetail 생성.
		// HighDetail의 1/4 로 생성한다.
		{
			{
				// 마고자 (2005-03-24 오전 10:57:17) : 
				// 세이브 데이타 얻어냄..
				AgcmMapSectorData	* pClientSectorData		= GetAgcmAttachedData( pSector );
				pSaveData = ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData1;
			}

			nDepth = pSector->D_GetDepth( SECTOR_HIGHDETAIL );
			FLOAT	fStepSize = pSector->GetStepSizeX( SECTOR_HIGHDETAIL );

			float	pSecStepSizeX = fStepSize;
			float	pSecStepSizeZ = fStepSize;

			if( nDepth == 0 || NULL == pToolSectorInfo->m_pSegment[ SECTOR_HIGHDETAIL ] ) break;

			RpDWSector * pDWSector;
			//const	int	cTargetDetail = 4;
			ApDetailSegment * pSegment;

//			RwInt32	matIndex	;
			RwInt32 i, j;//, k		;

			// 데이타 준비.
			int	MAP_WIDTH	= nDepth;
			int	MAP_HEIGHT	= nDepth;

			int	MAP_SIZE	= ( ( MAP_WIDTH + 1	) * ( MAP_HEIGHT + 1	)		)	;
			int	NOT			= ( ( MAP_WIDTH		) * ( MAP_HEIGHT		) * 2	)	;	// Number of Triangles
			int	NOV			= MAP_SIZE												;

			// 폴리건 양 계산 끝.

			DestroyDWSector( pSector , nDetail );

			// 텍스쳐 로딩..
			char			filename	[ 256 ];
			
			wsprintf( filename , SECTOR_ROUGH_TEXTURE , pSector->GetArrayIndexX() , pSector->GetArrayIndexZ() );

			//RwTexture	* pTexture = RwD3D9DDSTextureRead( filename , NULL );
			this->m_pcsAgcmResourceLoader->EnableEncryption( TRUE );
			RwTexture	* pTexture = this->m_pcsAgcmResourceLoader->LoadTexture( filename , NULL , NULL , NULL , -1 , "Map\\Temp\\Rough\\" );
			
			if( NULL == pTexture )
			{
				#ifdef _DEBUG
				char	strDir[ 1024 ];
				GetCurrentDirectory( 1024 , strDir );
				TRACE( "'%s'-'%s' 텍스쳐가 존재하지 않습니다.\n" , strDir , filename );
				// 없이 그냥 진행함. 나중에 제너레이트 하겠지.
				#endif

				// 생성안함..
				break;
			}
			else
			{	
				//RwTextureSetFilterMode( pTexture, rwFILTERLINEARMIPLINEAR );
				RwTextureSetFilterMode( pTexture , rwFILTERLINEAR			);
				RwTextureSetAddressing( pTexture , rwTEXTUREADDRESSCLAMP	);
			}

			// 폴리건 메모리 할당.
			pDWSector = CreateDWSector( pSector , nDetail , NOV , NOT , 1 );

			// 별 필요 없을거 같지만 일단 Enable
			RpMTextureAtomicEnableEffect( pDWSector->atomic );

			VERIFY( RpGeometryLock( pDWSector->geometry , rpGEOMETRYLOCKALL ) );

			RpMaterial	* pMaterial = RpMaterialCreate();
			ASSERT( NULL != pMaterial );

			RwSurfaceProperties		surfProp	;
			surfProp.ambient	= ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_AMBIENT	;
			surfProp.diffuse	= ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_DIFFUSE	;
			surfProp.specular	= ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_SPECULAR;

			RpMaterialSetSurfaceProperties( pMaterial, &surfProp);

			RwRGBA	rgb;
			rgb.alpha	= 255;
			rgb.blue	= 255;
			rgb.green	= 255;
			rgb.red		= 255;

			RpMaterialSetColor( pMaterial , &rgb );

			RpMaterialSetTexture( pMaterial , pTexture );

			INT32	nMaterialIndex = _rpMaterialListAppendMaterial( & pDWSector->geometry->matList , pMaterial );

			// RepCount 확인 요!// 마고자 (2004-02-24 오전 11:33:16) : 
			if( pMaterial	)
			{
				RpMaterialDestroy	( pMaterial	);
				pMaterial = NULL;
			}

			if( pTexture	)
			{
				RwTextureDestroy	( pTexture	);
				pTexture = NULL;
			}
		
			//memset( stDataSet.pVertices	, 0 , sizeof(	RwV3d		) * NOV	);
			//memset( stDataSet.pPolygons	, 0 , sizeof(	RpTriangle	) * NOT	);

			_MakeDetailDataSet	stDataSet;

			stDataSet.nWidth			= MAP_WIDTH			;
			stDataSet.nHeight			= MAP_HEIGHT		;
			stDataSet.nVertexIndex		= 0					;
			stDataSet.pSector			= pSector			;
			stDataSet.pVertexList		= m_pVertexList		;
			stDataSet.pPreLightList		= m_pPreLightList	;

			stDataSet.pMorphTarget	= RpGeometryGetMorphTarget		( pDWSector->geometry , 0	);

			ASSERT( NULL != stDataSet.pMorphTarget );

			stDataSet.pPolygons		= RpGeometryGetTriangles		( pDWSector->geometry		);			//	Polygons themselves 
			stDataSet.pVertices		= RpMorphTargetGetVertices		( stDataSet.pMorphTarget	);			//	Vertex positions 
			stDataSet.pNormals		= RpMorphTargetGetVertexNormals	( stDataSet.pMorphTarget	);			//	Vertex normals 
			stDataSet.pPreLitLum	= RpGeometryGetPreLightColors	( pDWSector->geometry		);
			
			for( i = 0 ; i < rwMAXTEXTURECOORDS ; ++i )
				stDataSet.pTexCoords[ i ] = RpGeometryGetVertexTexCoords( pDWSector->geometry , 
								( RwTextureCoordinateIndex ) ( rwTEXTURECOORDINATEINDEX0 + i ) );//	Texture coordinates 

			if( pSaveData->IsLoadedGeometry() )
			{
				//memcpy( ( void * ) stDataSet.pVertices , ( void * ) pSaveData->vertices , ( sizeof RwV3d ) * NOV );
				memcpy( ( void * ) stDataSet.pNormals , ( void * ) pSaveData->normals , ( sizeof RwV3d ) * NOV );
				stDataSet.bValidNormal = TRUE;
			}

			if( pSaveData->IsLoadedMoonee() )
			{
				// 로드가 돼어 있으면 버택스랑 프라라잇 정보를 덮어써버린다.
				//memcpy( ( void * ) stDataSet.pPreLitLum , ( void * ) pSaveData->preLitLum , ( sizeof RwRGBA ) * NOV );
				for( int i = 0 ; i < pSaveData->numTexCoordSets ; i ++ )
				{
					memcpy( ( void * ) stDataSet.pTexCoords[ i ] , ( void * ) pSaveData->texCoords[ i ] , ( sizeof RwTexCoords ) * NOV );
				}
				stDataSet.bValidTexCoords = TRUE;
			}

			RwV3d normal = {0.0f, 1.0f, 0.0f};

			for ( i = 0 ; i < MAP_HEIGHT + 1 ; ++i )
			{
				for (j = 0 ; j < MAP_WIDTH + 1 ; ++j )
				{
					pSegment		= pSector->D_GetSegment(  SECTOR_HIGHDETAIL , j , i )			;

					if( pSegment )
					{
						stDataSet.pVertices->x = xStart + ( j ) * pSecStepSizeX	;
						stDataSet.pVertices->z = zStart + ( i ) * pSecStepSizeZ	;
						stDataSet.pVertices->y = pSegment->height;
						++stDataSet.pVertices	;

						_RwRGBAAssigApRGB( *stDataSet.pPreLitLum , pSegment->vertexcolor );
						++stDataSet.pPreLitLum;

						stDataSet.pTexCoords[0]->u	= ( FLOAT ) j / ( FLOAT ) MAP_WIDTH		;
						stDataSet.pTexCoords[0]->v	= ( FLOAT ) i / ( FLOAT ) MAP_HEIGHT	;
						++stDataSet.pTexCoords[0]	;

						*stDataSet.pNormals	= normal;
						++stDataSet.pNormals	;
					}
					else
					{
						ApRGBA rgba;
						rgba.alpha = 255 ; rgba.red = 255 ; rgba.green = 255 ; rgba.blue = 255 ;

						stDataSet.pVertices->x = xStart + ( j ) * pSecStepSizeX	;
						stDataSet.pVertices->z = zStart + ( i ) * pSecStepSizeZ	;
						stDataSet.pVertices->y = ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
						++stDataSet.pVertices	;

						_RwRGBAAssigApRGB( *stDataSet.pPreLitLum , rgba );
						++stDataSet.pPreLitLum;

						stDataSet.pTexCoords[0]->u	= ( FLOAT ) j / ( FLOAT ) MAP_WIDTH		;
						stDataSet.pTexCoords[0]->v	= ( FLOAT ) i / ( FLOAT ) MAP_HEIGHT	;
						++stDataSet.pTexCoords[0]	;

						*stDataSet.pNormals	= normal;
						++stDataSet.pNormals	;
					}
				}
			}			///////////////

			int	polygoncount = 0;
			// k는 vertex index

			ApDetailSegment	* pWorkingSegment;

			#define	INDEX( x , z ) ( ( i + (x) )  + ( j + (z) ) * ( MAP_WIDTH + 1 ) )

			for (i = 0; i < MAP_HEIGHT ; ++i)
			{
				for (j = 0; j < MAP_WIDTH ; ++j ) // 폴리건 두개씩 처리함.
				{
					pWorkingSegment = pSector->D_GetSegment(  SECTOR_HIGHDETAIL , j , i );
					
					// 삼각형 설정.
					if( pWorkingSegment->stTileInfo.GetEdgeTurn() )
					{
						stDataSet.pPolygons->vertIndex[0]		= INDEX( 0 , 0 );
						stDataSet.pPolygons->vertIndex[1]		= INDEX( 0 , 1 );
						stDataSet.pPolygons->vertIndex[2]		= INDEX( 1 , 1 );
						stDataSet.pPolygons->matIndex			= nMaterialIndex	;
						++stDataSet.pPolygons;
						
						stDataSet.pPolygons->vertIndex[0]		= INDEX( 0 , 0 );
						stDataSet.pPolygons->vertIndex[1]		= INDEX( 1 , 1 );
						stDataSet.pPolygons->vertIndex[2]		= INDEX( 1 , 0 );
						stDataSet.pPolygons->matIndex			= nMaterialIndex	;
						++stDataSet.pPolygons;
					}
					else
					{
						stDataSet.pPolygons->vertIndex[0]		= INDEX( 0 , 0 );
						stDataSet.pPolygons->vertIndex[1]		= INDEX( 0 , 1 );
						stDataSet.pPolygons->vertIndex[2]		= INDEX( 1 , 0 );
						stDataSet.pPolygons->matIndex			= nMaterialIndex	;
						++stDataSet.pPolygons;
						
						stDataSet.pPolygons->vertIndex[0]		= INDEX( 0 , 1 );
						stDataSet.pPolygons->vertIndex[1]		= INDEX( 1 , 1 );
						stDataSet.pPolygons->vertIndex[2]		= INDEX( 1 , 0 );
						stDataSet.pPolygons->matIndex			= nMaterialIndex	;
						++stDataSet.pPolygons;
					}
				}
			}

			RpGeometryUnlock( pDWSector->geometry );
			
			RwSphere	sphere;
			RpMorphTargetCalcBoundingSphere	( stDataSet.pMorphTarget , &sphere );
			RpMorphTargetSetBoundingSphere	( stDataSet.pMorphTarget , &sphere );

		}
		break;
	case SECTOR_HIGHDETAIL	:
		{
			if( nDepth == 0 ) break;
			if( NULL == pDetailSegment ) break;

			float	pSecStepSizeX = pSector->GetStepSizeX( nDetail );
			float	pSecStepSizeZ = pSector->GetStepSizeZ( nDetail );

			{
				// 마고자 (2005-03-24 오전 10:57:17) : 
				// 세이브 데이타 얻어냄..
				AgcmMapSectorData	* pClientSectorData		= GetAgcmAttachedData( pSector );
				pSaveData = ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData2;
			}

			RpDWSector		* pDWSector	;
			ApDetailSegment	* pSegment	;

			RwInt32					matIndex	;
			RwInt32					i, j, k		;

			// 데이타 준비.
			int	MAP_WIDTH	= nDepth;
			int	MAP_HEIGHT	= nDepth;

			int	MAP_SIZE	= ( ( MAP_WIDTH + 1	) * ( MAP_HEIGHT + 1	)		)	;
			int	NOT			= ( ( MAP_WIDTH		) * ( MAP_HEIGHT		) * 2	)	;	// Number of Triangles
			int	NOV			= NOT * 3												;

			RwV3d * VertexList;
			VertexList		= m_pVertexList		;
			RwRGBA	* pPreLightList;
			pPreLightList	= m_pPreLightList	;

			DestroyDWSector( pSector , nDetail );
			
			// 마고자 (2004-04-16 오후 6:27:45) : 지금은 6개만 쓰므로 6으로 고정 
			if( pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
			{
				// 마고자 (2005-03-15 오후 5:38:22) : 
				// 지형 효과 플래그.. 텍스쳐 좌표는 2개로 고정.
				//@{ Jaewon 20050416
				// 2 -> 1
				//@{ Jaewon 20050420
				// 1 -> pSaveData->numTexCoordSets==2?2:1
				ASSERT(pSaveData->numTexCoordSets==0 || pSaveData->numTexCoordSets==1 || pSaveData->numTexCoordSets==2);
				pDWSector = CreateDWSector( pSector , nDetail , NOV , NOT , pSaveData->numTexCoordSets==2?2:1 ); 
				//@} Jaewon
				//@} Jaewon
			}
			else
			{
				pDWSector = CreateDWSector( pSector , nDetail , NOV , NOT , 6 ); // TD_DEPTH
				RpMTextureAtomicEnableEffect( pDWSector->atomic );
			}

			VERIFY( RpGeometryLock( pDWSector->geometry , rpGEOMETRYLOCKALL ) );

			RpMorphTarget			*pMorphTarget = RpGeometryGetMorphTarget		( pDWSector->geometry , 0	);

			ASSERT( NULL != pMorphTarget );

			RpTriangle				*polygons	= RpGeometryGetTriangles			( pDWSector->geometry		);			//	Polygons themselves 
			RwV3d					*vertices	= RpMorphTargetGetVertices			( pMorphTarget				);			//	Vertex positions 
			RwV3d					*normals	= RpMorphTargetGetVertexNormals		( pMorphTarget				);			//	Vertex normals 
			RwTexCoords				*texCoords	[rwMAXTEXTURECOORDS]	;
			RwRGBA					*preLitLum	= RpGeometryGetPreLightColors		( pDWSector->geometry		);
			
			for( i = 0 ; i < rwMAXTEXTURECOORDS ; ++i )
				texCoords[ i ] = RpGeometryGetVertexTexCoords( pDWSector->geometry , 
								( RwTextureCoordinateIndex ) ( rwTEXTURECOORDINATEINDEX0 + i ) );//	Texture coordinates 

			//폴리건 정보 초기화..
			// memset( polygons	, 0 , sizeof(	RpTriangle	) * NOT	);

			if( pSaveData->IsLoadedGeometry() )
			{
				// memcpy( ( void * ) vertices , ( void * ) pSaveData->vertices , ( sizeof RwV3d ) * NOV );
				memcpy( ( void * ) normals , ( void * ) pSaveData->normals , ( sizeof RwV3d ) * NOV );
			}

			if( pSaveData->IsLoadedMoonee() )
			{
				// 로드가 돼어 있으면 버택스랑 프라라잇 정보를 덮어써버린다.
				//memcpy( ( void * ) preLitLum , ( void * ) pSaveData->preLitLum , ( sizeof RwRGBA ) * NOV );
				for( int i = 0 ; i < pSaveData->numTexCoordSets ; i ++ )
				{
					memcpy( ( void * ) texCoords[ i ] , ( void * ) pSaveData->texCoords[ i ] , ( sizeof RwTexCoords ) * NOV );
				}
			}

			/* 
			* Define vertices and triangles for pentagons... 
			*/
			for ( i = 0 ; i < MAP_HEIGHT + 1 ; ++i )
			{
				for (j = 0 ; j < MAP_WIDTH + 1 ; ++j )
				{
					pSegment			= pSector->D_GetSegment(  nDetail , j , i )			;

					if( pSegment )
					{
						MAP( j , i ).x		= xStart + ( j ) * pSecStepSizeX	;
						MAP( j , i ).z		= zStart + ( i ) * pSecStepSizeZ	;
						MAP( j , i ).y		= pSegment->height;

						_RwRGBAAssigApRGB( PRELIGHT( j , i ) , pSegment->vertexcolor );
					}
					else
					{
						MAP( j , i ).x		= xStart + ( j ) * pSecStepSizeX	;
						MAP( j , i ).z		= zStart + ( i ) * pSecStepSizeZ	;
						MAP( j , i ).y		= ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
						ApRGBA rgba;
						rgba.alpha = 255 ; rgba.red = 255 ; rgba.green = 255 ; rgba.blue = 255 ;
						_RwRGBAAssigApRGB( PRELIGHT( j , i ) , rgba );
					}
				}
			}///////////////////////


			int	polygoncount = 0;
			// k는 vertex index

			ApDetailSegment	* pWorkingSegment;

			
			if( pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
			{
				// 마고자 (2005-03-15 오후 5:40:31) : 
				// 지형효과 적용된 곳에서는...
				// 지형을 따로 생성.

				for (i = 0, k = 0; i < MAP_HEIGHT ; ++i)
				{
					for (j = 0; j < MAP_WIDTH ; ++j ) // 폴리건 두개씩 처리함.
					{
						RwV3d normal = {0.0f, 1.0f, 0.0f};
						
						// 왼쪽 위의것.

						pWorkingSegment = pSector->D_GetSegment(  nDetail , j , i );
						matIndex = pMaterialList[ nDetail ].GetMatIndex	( pWorkingSegment->pIndex[ TD_FIRST	] );

						*vertices				= MAP		( j	, i	);
						++vertices	;
						*vertices				= MAP(j + 1, i);
						++vertices	;
						*vertices				= MAP(j, i + 1);
						++vertices	;
						*vertices				= MAP(j + 1, i + 1);
						++vertices	;
						if( pWorkingSegment->stTileInfo.GetEdgeTurn() )
						{
							*vertices				= MAP		( j	, i	);
							++vertices	;
							*vertices				= MAP(j + 1, i + 1);
							++vertices	;
						}
						else
						{
							*vertices				= MAP(j, i + 1);
							++vertices	;
							*vertices				= MAP(j + 1, i);
							++vertices	;
						}

						if( !pSaveData->IsLoadedGeometry() )
						{
							// 로딩 안돼있으면..
							*normals				= normal;
							++normals	;
							*normals				= normal;
							++normals	;
							*normals				= normal;
							++normals	;
							*normals				= normal;
							++normals	;
							*normals				= normal;
							++normals	;
							*normals				= normal;
							++normals	;
						}
						if( preLitLum )
						{
							*preLitLum = PRELIGHT	( j , i );
							++preLitLum;
							*preLitLum				= PRELIGHT(j + 1, i);
							++preLitLum;
							*preLitLum				= PRELIGHT(j, i + 1);
							++preLitLum;
							*preLitLum				= PRELIGHT(j + 1, i + 1);
							++preLitLum;
							if( pWorkingSegment->stTileInfo.GetEdgeTurn() )
							{
								*preLitLum = PRELIGHT	( j , i );
								++preLitLum;
								*preLitLum				= PRELIGHT(j + 1, i + 1);
								++preLitLum;
							}
							else
							{
								*preLitLum				= PRELIGHT(j, i + 1);
								++preLitLum;
								*preLitLum				= PRELIGHT(j + 1, i);
								++preLitLum;
							}
						}
	
						if( !pSaveData->IsLoadedMoonee() )
						{
							texCoords[0]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
							texCoords[0]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
							_PreviewTextureSetup( pSector  , j , i , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

							// 마고자 (2005-03-15 오후 5:41:52) : 예비
							//texCoords[1]->u	= 0.0f;
							//texCoords[1]->v	= 0.0f;
							++texCoords[0]	;		++texCoords[1]	;
							
							texCoords[0]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
							texCoords[0]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
							_PreviewTextureSetup( pSector  , j + 1, i , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

							// 마고자 (2005-03-15 오후 5:41:52) : 예비
							//texCoords[1]->u	= 0.0f;
							//texCoords[1]->v	= 0.0f;

							++texCoords[0]	;		++texCoords[1]	;

							texCoords[0]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
							texCoords[0]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
							_PreviewTextureSetup( pSector  , j , i + 1 , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

							// 마고자 (2005-03-15 오후 5:41:52) : 예비
							//texCoords[1]->u	= 0.0f;
							//texCoords[1]->v	= 0.0f;

							++texCoords[0]	;		++texCoords[1]	;
							
							texCoords[0]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
							texCoords[0]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
							_PreviewTextureSetup( pSector  , j + 1, i + 1 , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

							// 마고자 (2005-03-15 오후 5:41:52) : 예비
							//texCoords[1]->u	= 0.0f;
							//texCoords[1]->v	= 0.0f;
							
							++texCoords[0]	;		++texCoords[1]	;
							
							// 삼각형 설정.
							if( pWorkingSegment->stTileInfo.GetEdgeTurn() )
							{
								// 엣지턴 관련 버택스 2개 처리.
								texCoords[0]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
								texCoords[0]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
								_PreviewTextureSetup( pSector  , j , i , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

								// 마고자 (2005-03-15 오후 5:41:52) : 예비
								//texCoords[1]->u	= 0.0f;
								//texCoords[1]->v	= 0.0f;

								++texCoords[0]	;		++texCoords[1]	;

								texCoords[0]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
								texCoords[0]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
								_PreviewTextureSetup( pSector  , j + 1, i + 1 , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

								// 마고자 (2005-03-15 오후 5:41:52) : 예비
								//texCoords[1]->u	= 0.0f;
								//texCoords[1]->v	= 0.0f;

								++texCoords[0]	;		++texCoords[1]	;
							}
							else
							{
								texCoords[0]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
								texCoords[0]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
								_PreviewTextureSetup( pSector  , j , i + 1 , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

								// 마고자 (2005-03-15 오후 5:41:52) : 예비
								//texCoords[1]->u	= 0.0f;
								//texCoords[1]->v	= 0.0f;

								++texCoords[0]	;		++texCoords[1]	;

								texCoords[0]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
								texCoords[0]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
								_PreviewTextureSetup( pSector  , j + 1, i , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

								// 마고자 (2005-03-15 오후 5:41:52) : 예비
								//texCoords[1]->u	= 0.0f;
								//texCoords[1]->v	= 0.0f;

								++texCoords[0]	;		++texCoords[1]	;								
							}
						}
						// 폴리건 설정
						if( pWorkingSegment->stTileInfo.GetEdgeTurn() )
						{
							polygons->vertIndex[0]		= k + 0		;
							polygons->vertIndex[1]		= k + 2		;
							polygons->vertIndex[2]		= k	+ 3		;
							polygons->matIndex			= matIndex	;
							++polygons;
							
							polygons->vertIndex[0]		= k + 4		;
							polygons->vertIndex[1]		= k + 5		;
							polygons->vertIndex[2]		= k + 1		;
							polygons->matIndex			= matIndex	;
							++polygons;
						}
						else
						{
							polygons->vertIndex[0]		= k + 0		;
							polygons->vertIndex[1]		= k + 2		;
							polygons->vertIndex[2]		= k	+ 1		;
							polygons->matIndex			= matIndex	;
							++polygons;
							
							polygons->vertIndex[0]		= k + 4		;
							polygons->vertIndex[1]		= k + 3		;
							polygons->vertIndex[2]		= k + 5		;
							polygons->matIndex			= matIndex	;
							++polygons;
						}

						polygoncount += 2;
						k += 6;	
					}
				}
			}
			else
			{
				for (i = 0, k = 0; i < MAP_HEIGHT ; ++i)
				{
					for (j = 0; j < MAP_WIDTH ; ++j ) // 폴리건 두개씩 처리함.
					{
						RwV3d normal = {0.0f, 1.0f, 0.0f};
						
						// 왼쪽 위의것.

						pWorkingSegment = pSector->D_GetSegment(  nDetail , j , i );

						// 마고자 (2005-08-29 오후 2:58:20) : 
						// 타일 바꿔치기 기능
						if( m_uTiled )
						{
							for( int nTile = 0 ; nTile < TD_DEPTH ; nTile ++ )
							{
								if( IsSameTexture( pWorkingSegment->pIndex[ nTile ] , m_uTiled ) )
								{
									pWorkingSegment->pIndex[ nTile ] &= 0x000000ff;
									pWorkingSegment->pIndex[ nTile ] |= ( m_uReplace & 0x0fffff00 );
								}
							}
						}

						matIndex = pMaterialList[ nDetail ].GetMatIndex	(
																			pWorkingSegment->pIndex[ TD_FIRST	] ,
																			pWorkingSegment->pIndex[ TD_SECOND	] ,
																			pWorkingSegment->pIndex[ TD_THIRD	] ,
																			pWorkingSegment->pIndex[ TD_FOURTH	] ,
																			pWorkingSegment->pIndex[ TD_FIFTH	] ,
																			pWorkingSegment->pIndex[ TD_SIXTH	] 
																			);

						*vertices				= MAP		( j	, i	);
						++vertices	;
						*vertices				= MAP(j + 1, i);
						++vertices	;
						*vertices				= MAP(j, i + 1);
						++vertices	;
						*vertices				= MAP(j + 1, i + 1);
						++vertices	;
						if( pWorkingSegment->stTileInfo.GetEdgeTurn() )
						{
							*vertices				= MAP		( j	, i	);
							++vertices	;
							*vertices				= MAP(j + 1, i + 1);
							++vertices	;
						}
						else
						{
							*vertices				= MAP(j, i + 1);
							++vertices	;
							*vertices				= MAP(j + 1, i);
							++vertices	;
						}

						if( !pSaveData->IsLoadedGeometry() )
						{
							// 로딩 안돼있으면..
							*normals				= normal;
							++normals	;
							*normals				= normal;
							++normals	;
							*normals				= normal;
							++normals	;
							*normals				= normal;
							++normals	;
							*normals				= normal;
							++normals	;
							*normals				= normal;
							++normals	;
						}

						if( preLitLum )
						{
							*preLitLum = PRELIGHT	( j , i );
							++preLitLum;

							*preLitLum				= PRELIGHT(j + 1, i);
							++preLitLum;
							
							*preLitLum				= PRELIGHT(j, i + 1);
							++preLitLum;

							*preLitLum				= PRELIGHT(j + 1, i + 1);
							++preLitLum;
							
							// 삼각형 설정.
							if( pWorkingSegment->stTileInfo.GetEdgeTurn() )
							{
								*preLitLum = PRELIGHT	( j , i );
								++preLitLum;

								*preLitLum				= PRELIGHT(j + 1, i + 1);
								++preLitLum;
							}
							else
							{
								*preLitLum				= PRELIGHT(j, i + 1);
								++preLitLum;

								*preLitLum				= PRELIGHT(j + 1, i);
								++preLitLum;
							}
						}

						if( !pSaveData->IsLoadedMoonee() )
						{
							texCoords[0]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
							texCoords[0]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
							_PreviewTextureSetup( pSector  , j , i , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

							texCoords[1]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_SECOND] );
							texCoords[1]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_SECOND] );

							texCoords[2]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_THIRD	] );
							texCoords[2]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_THIRD	] );

							texCoords[3]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FOURTH] );
							texCoords[3]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FOURTH] );

							texCoords[4]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIFTH] );
							texCoords[4]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIFTH] );

							texCoords[5]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_SIXTH] );
							texCoords[5]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_SIXTH] );

							++texCoords[0]	;		++texCoords[1]	;		++texCoords[2]	;		++texCoords[3];++texCoords[4];++texCoords[5];

							texCoords[0]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
							texCoords[0]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
							_PreviewTextureSetup( pSector  , j + 1, i , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

							texCoords[1]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_SECOND] );
							texCoords[1]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_SECOND] );

							texCoords[2]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_THIRD	] );
							texCoords[2]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_THIRD	] );

							texCoords[3]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FOURTH] );
							texCoords[3]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FOURTH] );

							texCoords[4]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIFTH] );
							texCoords[4]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIFTH] );

							texCoords[5]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_SIXTH] );
							texCoords[5]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_SIXTH] );

							++texCoords[0]	;		++texCoords[1]	;		++texCoords[2]	;		++texCoords[3];++texCoords[4];++texCoords[5];
							texCoords[0]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
							texCoords[0]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
							_PreviewTextureSetup( pSector  , j , i + 1 , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

							texCoords[1]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_SECOND] );
							texCoords[1]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_SECOND] );

							texCoords[2]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_THIRD	] );
							texCoords[2]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_THIRD	] );

							texCoords[3]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FOURTH] );
							texCoords[3]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FOURTH] );

							texCoords[4]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIFTH] );
							texCoords[4]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIFTH] );

							texCoords[5]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_SIXTH] );
							texCoords[5]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_SIXTH] );
							
							++texCoords[0]	;		++texCoords[1]	;		++texCoords[2]	;		++texCoords[3];++texCoords[4];++texCoords[5];
							texCoords[0]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
							texCoords[0]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
							_PreviewTextureSetup( pSector  , j + 1, i + 1 , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

							texCoords[1]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_SECOND] );
							texCoords[1]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_SECOND] );

							texCoords[2]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_THIRD	] );
							texCoords[2]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_THIRD	] );

							texCoords[3]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FOURTH] );
							texCoords[3]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FOURTH] );

							texCoords[4]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIFTH] );
							texCoords[4]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIFTH] );

							texCoords[5]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_SIXTH] );
							texCoords[5]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_SIXTH] );
							
							++texCoords[0]	;		++texCoords[1]	;		++texCoords[2]	;		++texCoords[3];++texCoords[4];++texCoords[5];

							// 삼각형 설정.
							if( pWorkingSegment->stTileInfo.GetEdgeTurn() )
							{
								texCoords[0]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
								texCoords[0]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
								_PreviewTextureSetup( pSector  , j , i , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

								texCoords[1]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_SECOND] );
								texCoords[1]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_SECOND] );

								texCoords[2]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_THIRD	] );
								texCoords[2]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_THIRD	] );

								texCoords[3]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FOURTH] );
								texCoords[3]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FOURTH] );

								texCoords[4]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIFTH] );
								texCoords[4]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIFTH] );

								texCoords[5]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_SIXTH] );
								texCoords[5]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_SIXTH] );

								++texCoords[0]	;		++texCoords[1]	;		++texCoords[2]	;		++texCoords[3];++texCoords[4];++texCoords[5];
								texCoords[0]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
								texCoords[0]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
								_PreviewTextureSetup( pSector  , j + 1, i + 1 , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

								texCoords[1]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_SECOND] );
								texCoords[1]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_SECOND] );

								texCoords[2]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_THIRD	] );
								texCoords[2]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_THIRD	] );

								texCoords[3]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FOURTH] );
								texCoords[3]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FOURTH] );

								texCoords[4]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIFTH] );
								texCoords[4]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIFTH] );

								texCoords[5]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_SIXTH] );
								texCoords[5]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_SIXTH] );
								
								++texCoords[0]	;		++texCoords[1]	;		++texCoords[2]	;		++texCoords[3];++texCoords[4];++texCoords[5];
							}
							else
							{
								texCoords[0]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
								texCoords[0]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
								_PreviewTextureSetup( pSector  , j , i + 1 , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

								texCoords[1]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_SECOND] );
								texCoords[1]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_SECOND] );

								texCoords[2]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_THIRD	] );
								texCoords[2]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_THIRD	] );

								texCoords[3]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FOURTH] );
								texCoords[3]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FOURTH] );

								texCoords[4]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_FIFTH] );
								texCoords[4]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_FIFTH] );

								texCoords[5]->u	= GET_TEXTURE_U_START	( pWorkingSegment->pIndex[ TD_SIXTH] );
								texCoords[5]->v	= GET_TEXTURE_V_END		( pWorkingSegment->pIndex[ TD_SIXTH] );
								
								++texCoords[0]	;		++texCoords[1]	;		++texCoords[2]	;		++texCoords[3];++texCoords[4];++texCoords[5];
								texCoords[0]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIRST	] );
								texCoords[0]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIRST	] );
								_PreviewTextureSetup( pSector  , j + 1, i , pWorkingSegment->pIndex[ TD_FIRST ] , nDepth , texCoords[0] );

								texCoords[1]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_SECOND] );
								texCoords[1]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_SECOND] );

								texCoords[2]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_THIRD	] );
								texCoords[2]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_THIRD	] );

								texCoords[3]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FOURTH] );
								texCoords[3]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FOURTH] );

								texCoords[4]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_FIFTH] );
								texCoords[4]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_FIFTH] );

								texCoords[5]->u	= GET_TEXTURE_U_END		( pWorkingSegment->pIndex[ TD_SIXTH] );
								texCoords[5]->v	= GET_TEXTURE_V_START	( pWorkingSegment->pIndex[ TD_SIXTH] );

								++texCoords[0]	;		++texCoords[1]	;		++texCoords[2]	;		++texCoords[3];++texCoords[4];++texCoords[5];
							}
						}


						// 폴리건 설정
						if( pWorkingSegment->stTileInfo.GetEdgeTurn() )
						{
							polygons->vertIndex[0]		= k + 0		;
							polygons->vertIndex[1]		= k + 2		;
							polygons->vertIndex[2]		= k	+ 3		;
							polygons->matIndex			= matIndex	;
							++polygons;
							
							polygons->vertIndex[0]		= k + 4		;
							polygons->vertIndex[1]		= k + 5		;
							polygons->vertIndex[2]		= k + 1		;
							polygons->matIndex			= matIndex	;
							++polygons;
						}
						else
						{
							polygons->vertIndex[0]		= k + 0		;
							polygons->vertIndex[1]		= k + 2		;
							polygons->vertIndex[2]		= k	+ 1		;
							polygons->matIndex			= matIndex	;
							++polygons;
							
							polygons->vertIndex[0]		= k + 4		;
							polygons->vertIndex[1]		= k + 3		;
							polygons->vertIndex[2]		= k + 5		;
							polygons->matIndex			= matIndex	;
							++polygons;
						}

						polygoncount += 2;
						k += 6;
					}
				}
			}

			RpGeometryUnlock( pDWSector->geometry );

			// BBox 지정.

			RwSphere sphere;
			RpMorphTargetCalcBoundingSphere	( pMorphTarget , & sphere );
			RpMorphTargetSetBoundingSphere	( pMorphTarget , & sphere );

			//@{ Jaewon 20050420
			// Initialize the atomic's ambient occlusion data, if necessary.
			if(pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE)
			{
				if(pSaveData->aoObjectFlag_ & rtAMBOCCLMAPOBJECTAMBOCCLMAP)
				{
					AmbOcclMapObjectData *objectData;
					objectData = RPAMBOCCLMAPATOMICGETDATA(pDWSector->atomic);
					objectData->numSamples = pSaveData->aoNumSamples_;

					RwTexture *pTexture = NULL;
					ASSERT(strcmp(pSaveData->aoMapName_, ""));
					if(strcmp(pSaveData->aoMapName_, ""))
					// Load the ambient occlusion map.
					{
						pTexture = RwTextureRead(pSaveData->aoMapName_, NULL);
					}

					if(NULL == pTexture)
					// Create a dummy checkerboard texture for debug purposes.
					{
						ASSERT(pSaveData->aoMapSize_ > 0);
						pTexture = RwTextureCreate(RwRasterCreate(pSaveData->aoMapSize_, 
							pSaveData->aoMapSize_, 0, 
							RpAmbOcclMapGetRasterFormat()));
						ASSERT(pTexture);
						RwTextureSetAddressing(pTexture, rwTEXTUREADDRESSWRAP);
						RwTextureSetFilterMode(pTexture, rwFILTERLINEARMIPLINEAR);
						RwTextureSetName(pTexture, pSaveData->aoMapName_);
					}

					ASSERT(pSaveData->aoMapSize_ == RwRasterGetWidth(RwTextureGetRaster(pTexture)));

					// Register it to the AmbOcclMap plugin.
					pDWSector->atomic = RpAmbOcclMapAtomicSetAmbOcclMap(pDWSector->atomic, pTexture);
					ASSERT(pDWSector->atomic);

					//@{ Jaewon 20050720
					// rtAMBOCCLMAPOBJECTAMBOCCLMAP -> pSaveData->aoObjectFlag_
					RtAmbOcclMapAtomicSetFlags(pDWSector->atomic, pSaveData->aoObjectFlag_);
					//@} Jaewon

					// Set the pipe & register the shader constant upload callback for the ambient occlusion map.
					RpGeometry *pGeometry = RpAtomicGetGeometry(pDWSector->atomic);
					if(pGeometry)
					{
						ASSERT(NULL == RpSkinGeometryGetSkin(pGeometry));
						RpAtomicFxEnable(pDWSector->atomic);
						for(int j=0; j<RpGeometryGetNumMaterials(pGeometry); ++j)
						{	
							RtAmbOcclMapMaterialSetFlags(RpGeometryGetMaterial(pGeometry, j),
								rtAMBOCCLMAPMATERIALAMBOCCLMAP);
							//@{ Jaewon 20050818
							// Ok, it's my bad... If RpMaterialD3DFxSetEffect() returns FALSE, this should not be called.
							if(RpMaterialD3DFxSetEffect(RpGeometryGetMaterial(pGeometry, j), "ambientOcclusionMVL.fx", NULL))
								RpMaterialD3DFxSetConstantUploadCallBack(RpGeometryGetMaterial(pGeometry, j), fxConstUploadCB);
							//@} Jaewon
						}
					}

					RwTextureDestroy(pTexture);
					pTexture = NULL;
				}
			}
			//@} Jaewon
		}
		break;
	}

	#endif // USE_MFC
}

// 지형 데이타 수정..
void		AgcmMap::RecalcNormal		( ApWorldSector * pSector , INT32 nDetail , BOOL bReculsive	)
{
	if( !LockSector( pSector , GetGeometryLockMode() , nDetail ) ) return;

	switch( nDetail )
	{
	case SECTOR_LOWDETAIL:
		{
			if( pSector->IsLoadedDetailData() )
			{
				rsDWSectorInfo	dwSectorInfoRough , dwSectorDetail;
				GetDWSector( pSector , & dwSectorInfoRough );
				GetDWSector( pSector , SECTOR_HIGHDETAIL , & dwSectorDetail );		

				int j ;

				if( dwSectorDetail.vertices && dwSectorDetail.normals )
				{
					for( int i = 0 ; i < dwSectorInfoRough.numVertices ; i ++ )
					{
						for( j = 0 ; j < dwSectorDetail.numVertices ; ++ j )
						{
							if( dwSectorDetail.vertices[ j ].x	==	dwSectorInfoRough.vertices[ i ].x && 
								dwSectorDetail.vertices[ j ].z	==	dwSectorInfoRough.vertices[ i ].z )
							{
								break;
							}
						}

						dwSectorInfoRough.normals[ i ].x = dwSectorDetail.normals[ j ].x;
						dwSectorInfoRough.normals[ i ].y = dwSectorDetail.normals[ j ].y;
						dwSectorInfoRough.normals[ i ].z = dwSectorDetail.normals[ j ].z;
					}
				}
			}

		}
		break;
	case SECTOR_HIGHDETAIL:
		if( pSector->IsLoadedDetailData() )
		{
			int x	, z		;
			//int dx	, dz	;

			ApDetailSegment	*	pSegment	;
			float				fx , fz		;
			//ApSubDivisionSegmentInfo *pDetailSegmentInfo;

			UINT	uCheckTime = GetTickCount();

			for( z = 0 ; z < pSector->D_GetDepth( nDetail ) ; ++z  )
			{
				for( x = 0 ; x < pSector->D_GetDepth( nDetail ) ; ++x  )
				{
					pSegment = pSector->D_GetSegment( nDetail , x , z , &fx , &fz );
					if( pSegment )
					{
						// UpdateNormal2( pSector , fx , fz );
						D_SetHeight( pSector , nDetail , x , z , pSegment->height , bReculsive );
					}

				}
			}
		}
		break;
	}
}

void		AgcmMap::RecalcBoundingBox	( ApWorldSector * pSector 	)
{
	rsDWSectorInfo	dwSectorInfo;
	GetDWSector( pSector , & dwSectorInfo );

	ASSERT( NULL != dwSectorInfo.pDWSector	);
	ASSERT( NULL != dwSectorInfo.vertices	);
	
	if(	//NULL == pWorldSector			||
		NULL == dwSectorInfo.pDWSector	||
		NULL == dwSectorInfo.vertices	) return;

	//바운딩 박스 다시 계산.
	float	min_height , max_height;
	for( int i = 0 ; i < dwSectorInfo.numVertices ; ++i  )
	{
		if( i == 0 )
		{
			min_height = dwSectorInfo.vertices[ i ].y;
			max_height = dwSectorInfo.vertices[ i ].y;
		}
		else
		{
			if( max_height < dwSectorInfo.vertices[ i ].y )
			{
				max_height = dwSectorInfo.vertices[ i ].y;
			}
			else if ( min_height > dwSectorInfo.vertices[ i ].y )
			{
				min_height = dwSectorInfo.vertices[ i ].y;
			}
		}
	}

	//pWorldSector->boundingBox.inf.y	= min_height;
	//pWorldSector->boundingBox.sup.y	= max_height;

	//pWorldSector->tightBoundingBox = pWorldSector->boundingBox;

}

void		AgcmMap::Update				( ApWorldSector * pSector , int detail	)
{
	// 예외 처리..
	if( pSector->IsLoadedDetailData() == FALSE	) return;

	int	nPrevDetail = pSector->GetCurrentDetail();

	if( detail == -1 )
	{
		// use default
	}
	else
	{
		// use specified
		ASSERT( 0 <= detail && SECTOR_DETAILDEPTH > detail	);
		nPrevDetail = detail;
	}
	
	// 섹터 리메이크
	MakeDetailWorld			( pSector , nPrevDetail			);
	SetCurrentDetail		( pSector , nPrevDetail , TRUE	);
	
	// Lock Unlock 해줌..
	rsDWSectorInfo	dwSectorInfo , * pResult;
	pResult = GetDWSector( pSector , & dwSectorInfo );

	/*
	if( pResult && dwSectorInfo.pDWSector && dwSectorInfo.pDWSector->geometry )
	{
		RpCollisionGeometryDestroyData	( dwSectorInfo.pDWSector->geometry			);
		RpCollisionGeometryBuildData	( dwSectorInfo.pDWSector->geometry , NULL	);

		RecalcBoundingBox				( pSector									);
	}
	*/
}

void		AgcmMap::RecalcBoundingBox	(						)
{
	for( int x = GetLoadRangeX1() ; x < GetLoadRangeX2() ; ++x  )
	{
		for( int z = GetLoadRangeZ1() ; z < GetLoadRangeZ2() ; ++z  )
		{
			RecalcBoundingBox( m_pcsApmMap->GetSectorByArray( x , 0 , z ) );
		}
	}
}

void		AgcmMap::RecalcNormals		( BOOL bDetail , ProgressCallback pfCallback , void * pData )
{
	ASSERT( IsRangeApplied() );

	static	char	strMessage[] = "노멀값 갱신중..";
	int		nTarget		=	( GetLoadRangeX2() - GetLoadRangeX1() + 1 )		*
							( GetLoadRangeZ2() - GetLoadRangeZ1() + 1 )		* 2;
	int		nCurrent	= 1													;
	int		x;
	int		z;
	int count = 0 ;

	rsDWSectorInfo		dwSectorInfo;
	ApWorldSector	*	pSector		;

	for( x = GetLoadRangeX1() ; x < GetLoadRangeX2() ; ++x  )
	{
		for( z = GetLoadRangeZ1() ; z < GetLoadRangeZ2() ; ++z  )
		{
			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );

			pSector = m_pcsApmMap->GetSectorByArray( x , 0 , z );
			GetDWSector( pSector , & dwSectorInfo );

			if( dwSectorInfo.pDWSector && dwSectorInfo.pDWSector->geometry )
			{
				SetCurrentDetail( pSector , SECTOR_LOWDETAIL	);
				RecalcNormal( pSector , SECTOR_LOWDETAIL );
			}
		}
	}	
	UnlockSectors();

	for( x = GetLoadRangeX1() ; x < GetLoadRangeX2() ; ++x  )
	{
		for( z = GetLoadRangeZ1() ; z < GetLoadRangeZ2() ; ++z  )
		{
			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );

			pSector = m_pcsApmMap->GetSectorByArray( x , 0 , z );
			GetDWSector( pSector , & dwSectorInfo );

			if( dwSectorInfo.pDWSector && dwSectorInfo.pDWSector->geometry )
			{
				SetCurrentDetail( pSector , SECTOR_HIGHDETAIL	);
				RecalcNormal( pSector , SECTOR_HIGHDETAIL );
			}
		}
	}	
	UnlockSectors();
}

void		AgcmMap::UpdateAll			(						)
{
	ASSERT( IsRangeApplied() );

	int	x, z;
	ApWorldSector * pSector;
	for( x = GetLoadRangeX1() ; x < GetLoadRangeX2() ; ++x  )
	{
		for( z = GetLoadRangeZ1() ; z < GetLoadRangeZ2() ; ++z  )
		{
			pSector = m_pcsApmMap->GetSectorByArray( x , 0 , z );
			Update( pSector , pSector->GetCurrentDetail() );
		}
	}	
}

BOOL		AgcmMap::GenerateRoughMap	( INT32 nTargetDetail , INT32 nSourceDetail , int alphatexture ,
										FLOAT offset , BOOL bHeight , BOOL bTile , BOOL bVertexColor , BOOL bApplyAlpha )
{
	ASSERT( IsRangeApplied() );
	ASSERT( NULL != m_pcsApmMap );
	ASSERT( 0 <= nSourceDetail && SECTOR_DETAILDEPTH > nSourceDetail	);
	ASSERT( 0 <= nTargetDetail && SECTOR_DETAILDEPTH > nTargetDetail	);
	
	ApWorldSector * pSector;

	int x , z;
	
	for( x = GetLoadRangeX1() ; x <= GetLoadRangeX2() ; ++x )
	{
		for( z = GetLoadRangeZ1() ; z <= GetLoadRangeZ2() ; ++z )
		{
			// 아싸
			pSector = m_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( pSector )
			{
				SetCurrentDetail( pSector , nTargetDetail );

				pSector->GenerateRoughMap( nTargetDetail , nSourceDetail , offset ,
										bHeight , bTile , bVertexColor );

				//if( bApplyAlpha )
				//	ApplyAlphaToSector( pSector , alphatexture , nTargetDetail );
			}
		}
	}

	for( x = GetLoadRangeX1() ; x <= GetLoadRangeX2() ; ++x )
	{
		for( z = GetLoadRangeZ1() ; z <= GetLoadRangeZ2() ; ++z )
		{
			// 아싸
			pSector = m_pcsApmMap->GetSectorByArray( x , 0 , z );
			if( pSector )
			{
				Update( pSector , nTargetDetail );
			}
		}
	}


	for( x = GetLoadRangeX1() ; x <= GetLoadRangeX2() ; ++x )
	{
		for( z = GetLoadRangeZ1() ; z <= GetLoadRangeZ2() ; ++z )
		{
			pSector			= m_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( pSector )
			{
				if( pSector->IsLoadedDetailData() )
				{
					// 러프 디테일 둘다 생성..
					if( LockSector					( pSector , GetGeometryLockMode() , SECTOR_LOWDETAIL	) )
					{
						RecalcNormal					( pSector , SECTOR_LOWDETAIL							);
						UnlockSectors					(														);
					}
				}
			}
		}
	}	


	return TRUE;
}


void		AgcmMap::SetCurrentDetail	( int detail , BOOL bForce )
{
//	ASSERT( IsRangeApplied() );
//	ASSERT( NULL != m_pcsApmMap );
//	ASSERT( 0 <= detail && SECTOR_DETAILDEPTH > detail	);
//
//	ApWorldSector * pSector;
//
//	for( int	x = ( GetLoadRangeX1() > 1 ? ( GetLoadRangeX1() - 1 ) : GetLoadRangeX1()	) ;
//				x < ( GetLoadRangeX2() < MAP_WORLD_INDEX_WIDTH - 1 ? ( GetLoadRangeX2() + 1 ) : GetLoadRangeX2()	);
//				++x 																		)
//	{
//		for( int	z = ( GetLoadRangeZ1() > 1 ? ( GetLoadRangeZ1() - 1 ) : GetLoadRangeZ1()	);
//					z < ( GetLoadRangeZ2() < MAP_WORLD_INDEX_WIDTH - 1 ? ( GetLoadRangeZ2() + 1 ) : GetLoadRangeZ2()	);
//					++z 																		)
//		{
//			// 아싸
//			pSector = m_pcsApmMap->GetSectorByArray( x , 0 , z );
//			if( pSector )
//			{
//				SetCurrentDetail( pSector , detail );
//			}	
//					
//		}
//	}	

	for( int i = 0 ; i < ( int ) m_pcsApmMap->GetCurrentLoadedSectorCount() ; ++ i )
	{
		if( m_pcsApmMap->GetCurrentLoadedSectors()[ i ] )
		{
			SetCurrentDetail( m_pcsApmMap->GetCurrentLoadedSectors()[ i ] , detail , bForce );
		}	
	}

}

void		AgcmMap::SetCurrentDetail	( ApWorldSector * pSector , int detail	, BOOL bForce 	)
{
	// comment by gemani 2004.11.21 (필요없는 코드 같아 주석 처리)
	//tid = GetCurrentThreadId();
	//if (tid == btid)
		//TRACE("");

	ASSERT( NULL != pSector );
	ASSERT( 0 <= detail && SECTOR_DETAILDEPTH > detail	);
	if( NULL == pSector || detail < 0 || detail >= SECTOR_DETAILDEPTH ) return;


	#ifndef USE_MFC
	if( ApWorldSector::OP_DONOTLOADSECTOR & pSector->GetFlag() )
	{
		return;
	}
	#endif
	
	/*
	if( pSector->GetDetailSectorInfo() )
	{
		// 디테일 데이타가 로딩 돼어 있으면...

		if( detail > 0 && pSector->D_GetDepth( detail ) == 0 )
		{
			// 데이타가 없으므로 더미섹터로 변경..
			pSector->SetCurrentDetail( SECTOR_EMPTY );
			
			RpDWSectorSetCurrentDetail	( pSector , SECTOR_EMPTY	);

			return;
		}

		if( RpDWSectorSetCurrentDetail	( pSector , detail	, bForce	) )
		{
			// 성공 할 경우에만 플래그 설정..

			int	prevDetail = pSector->GetCurrentDetail();
			pSector->SetCurrentDetail( detail );
		}
		
		return;
	}
	else// if( pSector->GetCompactSectorInfo() )

	*/
	{
		if( RpDWSectorSetCurrentDetail	( pSector , detail	, bForce	) )
		{
			// 성공 할 경우에만 플래그 설정..

			int	prevDetail = pSector->GetCurrentDetail();
			pSector->SetCurrentDetail( detail );
		}
		return;
	}
//	else
//	{
//		//ASSERT( !"데이타가 로딩돼어 있지 않습니다." );
//		TRACE( "AgcmMap::SetCurrentDetail 데이타가 로딩돼어 있지 않습니다.\n" );
//	}
}


RtWorldImport *	AgcmMap::CreateWorldImport()
{
	ASSERT( NULL != m_pcsApmMap );
/*
* Creates an world import and allocates and initializes the world's 
* vertices and triangles...
	*/
	RtWorldImport *			worldImport	;
	RtWorldImportTriangle *	triangles	;
	RtWorldImportVertex *	vertices	;
	RwTexture *				texture		;
	RpMaterial *			material	;
	RwInt32					matIndex	;

	// 데이타 준비.
	int	MAP_WIDTH	= GetLoadRangeX2() - GetLoadRangeX1();
	int	MAP_HEIGHT	= GetLoadRangeZ2() - GetLoadRangeZ1();

	int	MAP_SIZE	= ( ( MAP_WIDTH + 1	) * ( MAP_HEIGHT + 1	)		);
	int	NOT			= ( ( MAP_WIDTH		) * ( MAP_HEIGHT		) * 2	);	// Number of Triangles
	int	NOV			= NOT * 2;

	RwV3d * VertexList;
	VertexList		= new RwV3d[ MAP_SIZE ];

	ASSERT( VertexList != NULL );

	#define MAPIMPORT(x,y)			( VertexList[ ( y ) * ( MAP_WIDTH + 1 ) + ( x ) ] )
	//#define GETSECTOR( x , y )	aSector[ x + MAP_WORLD_INDEX_WIDTH * y ];


    RwInt32 i, j, k;
	
	worldImport = RtWorldImportCreate();
	if( worldImport == NULL )
    {
        return NULL;
    }
	
	RtWorldImportAddNumVertices		(worldImport, NOV);
    RtWorldImportAddNumTriangles	(worldImport, NOT);

	// Load Texture : 더미 섹터 확인용 텍스쳐.

	char	fname[ 256 ] , fname2[ 256 ];
	wsprintf( fname , "%s\\MAP\\TILE\\full_map.png" , m_strToolImagePath );
	texture = RwTextureRead	( RWSTRING ( fname ), NULL	);

	ASSERTE( texture != NULL && "full_map.png 파일을 찾을 수 없습니다!" );
	
	_RemoveFullPath	( fname		, fname2 );
	RwTextureSetName( texture	, fname2 );

	// 기본 테스쳐가 존재하지 않음..
	RwTextureSetFilterMode	( texture, rwFILTERLINEAR				);

    material	= RpMaterialCreate();

    RpMaterialSetTexture( material , texture );

    matIndex	= RtWorldImportAddMaterial	( worldImport , material	);

	vertices	= RtWorldImportGetVertices	( worldImport				);
    triangles	= RtWorldImportGetTriangles	( worldImport				);
	
	memset( vertices	, 0 , sizeof(	RtWorldImportVertex		) * NOV	);
	memset( triangles	, 0 , sizeof(	RtWorldImportTriangle	) * NOT	);
	
    /* 
	* Define vertices and triangles for pentagons... 
	*/

	ApWorldSector * pSector = NULL;
    for ( i = 0 ; i < MAP_HEIGHT ; ++i )
    {
		for (j = 0 ; j < MAP_WIDTH ; ++j )
		{
			pSector = m_pcsApmMap->GetSectorByArray( j + GetLoadRangeX1() , 0 , i + GetLoadRangeZ1() );
			
			ASSERT( NULL != pSector );

			MAPIMPORT( j , i ).x = pSector->GetXStart()	;
			MAPIMPORT( j , i ).z = pSector->GetZStart()	;
			MAPIMPORT( j , i ).y = 0.0f; // 더미는 높이가 일정함.
		}

		pSector = m_pcsApmMap->GetSectorByArray( j + GetLoadRangeX1() - 1 , 0 , i + GetLoadRangeZ1() );

		ASSERT( NULL != pSector );

		MAPIMPORT( j , i ).x	= pSector->GetXStart();
		MAPIMPORT( j , i ).z	= pSector->GetZStart();
		MAPIMPORT( j , i ).y	= 0.0f;
	}

	// 마지막라인..
	for ( j = 0 ; j < MAP_WIDTH ; ++j )
	{
		pSector = m_pcsApmMap->GetSectorByArray( ( j + GetLoadRangeX1() ) , 0 , ( i + GetLoadRangeZ1() - 1 ) );

		ASSERT( NULL != pSector );

		MAPIMPORT( j , i ).x = pSector->GetXStart()	;
		MAPIMPORT( j , i ).z = pSector->GetZEnd()	;
		MAPIMPORT( j , i ).y = 0.0f;
	}

	pSector = m_pcsApmMap->GetSectorByArray( ( j + GetLoadRangeX1() - 1 ) , 0 , ( i + GetLoadRangeZ1() - 1 ) );

	ASSERT( NULL != pSector );

	MAPIMPORT( j , i ).x	= pSector->GetXEnd()	;
	MAPIMPORT( j , i ).z	= pSector->GetZEnd()	;
	MAPIMPORT( j , i ).y	= 0.0f;
	
	#define CALC_U( a )	( ( float ) (a + GetLoadRangeX1() ) / ( float ) MAP_WORLD_INDEX_WIDTH );
	#define CALC_V( a )	( ( float ) (a + GetLoadRangeZ1() ) / ( float ) MAP_WORLD_INDEX_HEIGHT );

	// k는 vertex index
    for (i = 0, k = 0; i < MAP_HEIGHT ; ++i)
    {
		for (j = 0; j < MAP_WIDTH ; ++j, k += 4 ) // 폴리건 두개씩 처리함.
		{
			RwV3d normal = {0, 1, 0};
			
			vertices->OC				= MAPIMPORT	(j, i)		;
			vertices->normal			= normal			;
			vertices->texCoords[0].u	= CALC_U( j )		;
			vertices->texCoords[0].v	= CALC_V( i )		;
			vertices->preLitCol.alpha	= 255				;
			vertices->preLitCol.red		= 255				;
			vertices->preLitCol.green	= 255				;
			vertices->preLitCol.blue	= 255				;
			vertices->matIndex			= matIndex			;
			++vertices										;
			
			vertices->OC				= MAPIMPORT	(j + 1, i)	;
			vertices->normal			= normal			;
			vertices->texCoords[0].u	= CALC_U( j + 1 )	;
			vertices->texCoords[0].v	= CALC_V( i )		;
			vertices->preLitCol.alpha	= 255				;
			vertices->preLitCol.red		= 255				;
			vertices->preLitCol.green	= 255				;
			vertices->preLitCol.blue	= 255				;
			vertices->matIndex			= matIndex			;
			++vertices										;
			
			vertices->OC				= MAPIMPORT(j, i + 1)		;
			vertices->normal			= normal			;
			vertices->texCoords[0].u	= CALC_U( j )		;
			vertices->texCoords[0].v	= CALC_V( i + 1 )	;
			vertices->preLitCol.alpha	= 255				;
			vertices->preLitCol.red		= 255				;
			vertices->preLitCol.green	= 255				;
			vertices->preLitCol.blue	= 255				;
			vertices->matIndex			= matIndex			;
			++vertices										;
			
			vertices->OC				= MAPIMPORT(j + 1, i + 1)	;
			vertices->normal			= normal			;
			vertices->texCoords[0].u	= CALC_U( j + 1 )	;
			vertices->texCoords[0].v	= CALC_V( i + 1 )	;
			vertices->preLitCol.alpha	= 255				;
			vertices->preLitCol.red		= 255				;
			vertices->preLitCol.green	= 255				;
			vertices->preLitCol.blue	= 255				;
			vertices->matIndex			= matIndex			;
			++vertices										;

			// 삼각형 설정.
			triangles->vertIndex[0] = k + 2					;
			triangles->vertIndex[1] = k + 1					;
			triangles->vertIndex[2] = k						;
			triangles->matIndex		= matIndex				;
			++triangles										;
			
			triangles->vertIndex[0] = k + 2					;
			triangles->vertIndex[1] = k + 3					;
			triangles->vertIndex[2] = k + 1					;
			triangles->matIndex		= matIndex				;
			++triangles										;
		}
	}

	if( VertexList )
	{
		delete [] VertexList;
		VertexList = NULL;
	}

    RwTextureDestroy	( texture	);
	texture = NULL;

    RpMaterialDestroy	( material	);
	material = NULL;
	
	return worldImport;
}

RpDWSector *		AgcmMap::CreateDWSector		( ApWorldSector * pSector ,	int detail , int nov , int not , int tex )
{
	// 별로특별한것은 없고.. DWSector 포인터를 얻어내는 용도..
	ASSERT( NULL != pSector								);
	ASSERT( 0 <= detail && SECTOR_DETAILDEPTH > detail	);

	AcMaterialList *	pMaterialList	= GetMaterialList	( pSector );

	ASSERT( NULL != pMaterialList );

	// 섹터 정보 클리어..
	DestroyDWSector( pSector , detail );

	// 해딩 디테일 ㅍ인터 얻어서..
	RpDWSector * pDWSector = RpDWSectorGetDetail( pSector , detail );
	ASSERT( NULL != pDWSector );

	// 데이타 설정..
	pDWSector = RpDWSectorInit( pDWSector , nov , not , tex );
	ASSERT( NULL != pDWSector );

	// 아토믹에 멀티 텍스쳐 설정..
	ASSERT( NULL != pDWSector->atomic );

	// Material 설정..
	ASSERT( NULL != pDWSector->geometry );
	pMaterialList[ detail ].m_pGeometry = pDWSector->geometry;

	// 머티리얼 추가함..
    RpMaterial *			pMaterial	;

    pMaterial	= RpMaterialCreate();
    RpMaterialSetTexture	( pMaterial , NULL );

	ASSERT( NULL != pMaterial );

	RwTexture * pTexture = NULL;

	// 월드 맵 로딩~
	/*
	char	fname[ 256 ] , fname2[ 256 ];
	wsprintf( fname , "%s\\Map\\Tile\\full_map.png" , m_strToolImagePath );
	_RemoveFullPath	( fname		, fname2 );
	pTexture = RwTextureRead	( RWSTRING ( fname2 ), NULL	);
	if( NULL == pTexture )
	{
		pTexture = RwTextureRead	( RWSTRING ( fname ), NULL	);

		if( NULL != pTexture )
		{
			RwTextureSetName		( pTexture	, fname2			);
			RwTextureSetFilterMode	( pTexture	, rwFILTERLINEAR	);
		}

		RwError error;
		RwErrorGet( &error );
	}
	*/

	RpMaterialSetTexture( pMaterial , pTexture );

	int	matIndex = _rpMaterialListAppendMaterial( & pDWSector->geometry->matList , pMaterial );

	// Ref Count Down
	if( pTexture	)
	{
		RwTextureDestroy	( pTexture	);
		pTexture = NULL;
	}

	if( pMaterial	)
	{
		RpMaterialDestroy	( pMaterial	);
		pMaterial = NULL;
	}

	// Object Index 설정..
	if( detail == SECTOR_HIGHDETAIL )
	{
		if( tex >= 6 )
		{
			AcuObject::SetAtomicType(
				pDWSector->atomic				,
				ACUOBJECT_TYPE_WORLDSECTOR		,
				( INT32 ) pSector				); // 해당 섹터의 포인터를 삽입해둔다.
		}
		else
		{
			//@{ Jaewon 20050416
			// 2 -> 1
			//@{ Jaewon 20050421
			// 1 -> 1 or 2
			ASSERT( tex == 1 || tex == 2 );
			//@} Jaewon
			//@} Jaewon
			AcuObject::SetAtomicType(
				pDWSector->atomic				,
				ACUOBJECT_TYPE_WORLDSECTOR | ACUOBJECT_TYPE_NO_MTEXTURE	,
				( INT32 ) pSector				); // 해당 섹터의 포인터를 삽입해둔다.
		}
	}
	else if( detail == SECTOR_LOWDETAIL )
	{
		AcuObject::SetAtomicType(
			pDWSector->atomic			,
			ACUOBJECT_TYPE_WORLDSECTOR	| ACUOBJECT_TYPE_SECTOR_ROUGHMAP ,
			( INT32 ) pSector			); // 해당 섹터의 포인터를 삽입해둔다.
	}

	// 로딩할때도 같은 방법을 적용한다.

	return pDWSector;

}

void				AgcmMap::DestroyDWSector	( ApWorldSector * pSector ,	int detail )
{
	ASSERT( NULL != pSector								);
	ASSERT( 0 <= detail && SECTOR_DETAILDEPTH > detail	);

	AcMaterialList *	pMaterialList	= GetMaterialList	( pSector );

	RpDWSector * pDWSector = RpDWSectorGetDetail( pSector , detail );

	//ASSERT( NULL != pDWSector );
	if( pDWSector )
		RpDWSectorDestroy( pDWSector );

	// Material 정리..

	pMaterialList[ detail ].RemoveAll();
}

rsDWSectorInfo *	AgcmMap::GetDWSector		( ApWorldSector * pSector ,	int detail ,  rsDWSectorInfo * pDWSectorInfo )
{
	// 별로특별한것은 없고.. DWSector 포인터를 얻어내는 용도..
	ASSERT( NULL != pSector			);
	ASSERT( NULL != pDWSectorInfo	);

	ASSERT( 0 <= detail && SECTOR_DETAILDEPTH > detail	);
	if( detail == -1 ) return NULL;

	AcMaterialList *	pMaterialList	= GetMaterialList	( pSector );

	if( NULL == pSector )
	{
		// do no op...
		pDWSectorInfo->pDWSector	= NULL	;
		pDWSectorInfo->nDetail		= 0		;
		
		pDWSectorInfo->pMorphTarget	= NULL	;
		pDWSectorInfo->polygons		= NULL	;
		pDWSectorInfo->vertices		= NULL	;
		pDWSectorInfo->normals		= NULL	;
		pDWSectorInfo->preLitLum	= NULL	;

		pDWSectorInfo->numPolygons	= 0		;
		pDWSectorInfo->numVertices	= 0		;
		
		for( int i = 0 ; i < rwMAXTEXTURECOORDS ; ++i  )
			pDWSectorInfo->texCoords[ i ] = NULL;
	}
	else
	{
		pDWSectorInfo->pDWSector	= RpDWSectorGetDetail				( pSector , detail					);
		pDWSectorInfo->nDetail		= detail																 ;

		if( pDWSectorInfo->pDWSector && pDWSectorInfo->pDWSector->geometry )
		{
			pDWSectorInfo->pMorphTarget	= RpGeometryGetMorphTarget		( pDWSectorInfo->pDWSector->geometry , 0	);
			pDWSectorInfo->polygons		= RpGeometryGetTriangles		( pDWSectorInfo->pDWSector->geometry		);
			pDWSectorInfo->vertices		= RpMorphTargetGetVertices		( pDWSectorInfo->pMorphTarget				);
			pDWSectorInfo->normals		= RpMorphTargetGetVertexNormals	( pDWSectorInfo->pMorphTarget				);
			pDWSectorInfo->preLitLum	= RpGeometryGetPreLightColors	( pDWSectorInfo->pDWSector->geometry		);

			pDWSectorInfo->numPolygons	= pDWSectorInfo->pDWSector->geometry->numTriangles							 ;
			pDWSectorInfo->numVertices	= pDWSectorInfo->pDWSector->geometry->numVertices							 ;
			
			for( int i = 0 ; i < rwMAXTEXTURECOORDS ; ++i  )
				pDWSectorInfo->texCoords[ i ] = RpGeometryGetVertexTexCoords( pDWSectorInfo->pDWSector->geometry , 
												( RwTextureCoordinateIndex ) ( rwTEXTURECOORDINATEINDEX0 + i ) );//	Texture coordinates 
		}
		else
		{
			pDWSectorInfo->pMorphTarget	= NULL	;
			pDWSectorInfo->polygons		= NULL	;
			pDWSectorInfo->vertices		= NULL	;
			pDWSectorInfo->normals		= NULL	;
			pDWSectorInfo->preLitLum	= NULL	;

			pDWSectorInfo->numPolygons	= 0		;
			pDWSectorInfo->numVertices	= 0		;
			
			for( int i = 0 ; i < rwMAXTEXTURECOORDS ; ++i  )
				pDWSectorInfo->texCoords[ i ] = NULL;
		}
	}

	return pDWSectorInfo;
}

void	AgcmMap::UpdateNormal2( ApWorldSector * pSector , float fx , float fz )
{
	ASSERT( NULL != pSector );

	// 주변섹터도 모두 적용함.
#define	UPDATE_ARRAY_MAX_COUNT 16
	ApWorldSector	* pAlefSector				= NULL	;
	ApWorldSector	* pAlefSectorUpdateArray[ UPDATE_ARRAY_MAX_COUNT ]		;

	rsDWSectorInfo	dwSectorInfo					;

	int		count		= 0	;
	RwV3d	normal			;
	RwV3d	polygonnormal	;
	RwV3d	a				;
	RwV3d	b				;

	// 노멀 정보 바꾸기.
	normal.x = 0.0f;
	normal.y = 0.0f;
	normal.z = 0.0f;

	// 왼쪽위 섹터..
	if(	fx == pSector->GetXStart() && fz == pSector->GetZStart() )
	{	
		// 섹터 검색
		if( pSector->GetNearSector( TD_WEST ) && pSector->GetNearSector( TD_WEST )->GetNearSector( TD_NORTH ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_WEST )->GetNearSector( TD_NORTH );
		}
		else if( pSector->GetNearSector( TD_NORTH ) && pSector->GetNearSector( TD_NORTH )->GetNearSector( TD_WEST ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_NORTH )->GetNearSector( TD_WEST );
		}
		else
		{
			// do nothing;
		}

		if( pAlefSector && ( pAlefSector->IsLoadedCompactData() || pAlefSector->IsLoadedDetailData() ) && IsPolygonDataLoaded( pAlefSector ) )
		{
			ASSERT( count < UPDATE_ARRAY_MAX_COUNT );
			pAlefSectorUpdateArray[ count ++ ] = pAlefSector;

			GetDWSector( pAlefSector , &dwSectorInfo );

//			ASSERT( dwSectorInfo.numVertices >= 0	);
//			ASSERT( NULL != dwSectorInfo.vertices	);
//			ASSERT( NULL != dwSectorInfo.normals	);
			for( int polygonindex = 0 ; polygonindex < dwSectorInfo.numPolygons ; polygonindex ++)
			{
				// 해당 버텍스가 포함된 폴리건 찾아냄.
				if(
					//sector->polygons[ i ].matIndex
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].z	== fz
					)
				{
					// 찾았심.
					// 해당면의 노멀을 계산.
					RwV3dSub( &a,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );
					RwV3dSub( &b,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );

					RwV3dCrossProduct	( &polygonnormal , &a , &b					);
					RwV3dNormalize		( &polygonnormal , &polygonnormal			);

					RwV3dScale			( &polygonnormal , &polygonnormal , -1.0f	);

					// normal에 더함.
					RwV3dAdd			( &normal , &normal , &polygonnormal		);
				}
			}
		}
	}
	// 위쪽..
	if( fz == pSector->GetZStart() )
	{
		if( pSector->GetNearSector( TD_NORTH ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_NORTH );
		}
		else
		{
			// do nothing;
		}

		if( pAlefSector && ( pAlefSector->IsLoadedCompactData() || pAlefSector->IsLoadedDetailData() ) && IsPolygonDataLoaded( pAlefSector ) )
		{
			GetDWSector( pAlefSector , &dwSectorInfo );
			ASSERT( count < UPDATE_ARRAY_MAX_COUNT );
			pAlefSectorUpdateArray[ count ++ ] = pAlefSector;
//			ASSERT( dwSectorInfo.numVertices >= 0	);
//			ASSERT( NULL != dwSectorInfo.vertices	);
//			ASSERT( NULL != dwSectorInfo.normals	);
			for( int polygonindex = 0 ; polygonindex < dwSectorInfo.numPolygons ; polygonindex ++)
			{
				// 해당 버텍스가 포함된 폴리건 찾아냄.
				if(
					//sector->polygons[ i ].matIndex
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].z	== fz
					)
				{
					// 찾았심.
					// 해당면의 노멀을 계산.
					RwV3dSub( &a,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );
					RwV3dSub( &b,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );

					RwV3dCrossProduct( &polygonnormal , &a , &b );
					RwV3dNormalize(&polygonnormal, &polygonnormal);

					RwV3dScale	( &polygonnormal, &polygonnormal, -1.0f);

					// normal에 더함.
					RwV3dAdd( &normal , &normal , &polygonnormal );
				}
			}
		}
	}
	// 오른쪽 위..
	if( fx == pSector->GetXEnd() && fz == pSector->GetYStart() )
	{
		// 섹터 검색
		if( pSector->GetNearSector( TD_NORTH ) && pSector->GetNearSector( TD_NORTH )->GetNearSector( TD_EAST  ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_NORTH )->GetNearSector( TD_EAST );
		}
		else if( pSector->GetNearSector( TD_EAST ) && pSector->GetNearSector( TD_EAST )->GetNearSector( TD_NORTH ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_EAST )->GetNearSector( TD_NORTH );
		}
		else
		{
			// do nothing;
		}
		if( pAlefSector && ( pAlefSector->IsLoadedCompactData() || pAlefSector->IsLoadedDetailData() ) && IsPolygonDataLoaded( pAlefSector ) )
		{
			GetDWSector( pAlefSector , &dwSectorInfo );
			ASSERT( count < UPDATE_ARRAY_MAX_COUNT );
			pAlefSectorUpdateArray[ count ++ ] = pAlefSector;
//			ASSERT( dwSectorInfo.numVertices >= 0	);
//			ASSERT( NULL != dwSectorInfo.vertices	);
//			ASSERT( NULL != dwSectorInfo.normals	);
			for( int polygonindex = 0 ; polygonindex < dwSectorInfo.numPolygons ; ++polygonindex)
			{
				// 해당 버텍스가 포함된 폴리건 찾아냄.
				if(
					//sector->polygons[ i ].matIndex
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].z	== fz
					)
				{
					// 찾았심.
					// 해당면의 노멀을 계산.
					RwV3dSub( &a,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );
					RwV3dSub( &b,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );

					RwV3dCrossProduct( &polygonnormal , &a , &b );
					RwV3dNormalize(&polygonnormal, &polygonnormal);

					RwV3dScale	( &polygonnormal, &polygonnormal, -1.0f);

					// normal에 더함.
					RwV3dAdd( &normal , &normal , &polygonnormal );
				}
			}
		}
	}
	// 왼쪽
	if( fx == pSector->GetXStart() )
	{
		if( pSector->GetNearSector( TD_WEST ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_WEST );
		}
		else
		{
			// do nothing;
		}
		if( pAlefSector && ( pAlefSector->IsLoadedCompactData() || pAlefSector->IsLoadedDetailData() ) && IsPolygonDataLoaded( pAlefSector ) )
		{
			GetDWSector( pAlefSector , &dwSectorInfo );
			ASSERT( count < UPDATE_ARRAY_MAX_COUNT );
			pAlefSectorUpdateArray[ count ++ ] = pAlefSector;
//			ASSERT( dwSectorInfo.numVertices >= 0	);
//			ASSERT( NULL != dwSectorInfo.vertices	);
//			ASSERT( NULL != dwSectorInfo.normals	);
			for( int polygonindex = 0 ; polygonindex < dwSectorInfo.numPolygons ; ++polygonindex)
			{
				// 해당 버텍스가 포함된 폴리건 찾아냄.
				if(
					//sector->polygons[ i ].matIndex
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].z	== fz
					)
				{
					// 찾았심.
					// 해당면의 노멀을 계산.
					RwV3dSub( &a,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );
					RwV3dSub( &b,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );

					RwV3dCrossProduct( &polygonnormal , &a , &b );
					RwV3dNormalize(&polygonnormal, &polygonnormal);

					RwV3dScale	( &polygonnormal, &polygonnormal, -1.0f);

					// normal에 더함.
					RwV3dAdd( &normal , &normal , &polygonnormal );
				}
			}
		}
	}
	// 오른쪽
	if( fx == pSector->GetXEnd() )
	{
		if( pSector->GetNearSector( TD_EAST ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_EAST );
		}
		else
		{
			// do nothing;
		}
		if( pAlefSector && ( pAlefSector->IsLoadedCompactData() || pAlefSector->IsLoadedDetailData() ) && IsPolygonDataLoaded( pAlefSector ) )
		{
			GetDWSector( pAlefSector , &dwSectorInfo );
			ASSERT( count < UPDATE_ARRAY_MAX_COUNT );
			pAlefSectorUpdateArray[ count ++ ] = pAlefSector;
//			ASSERT( dwSectorInfo.numVertices >= 0	);
//			ASSERT( NULL != dwSectorInfo.vertices	);
//			ASSERT( NULL != dwSectorInfo.normals	);
			for( int polygonindex = 0 ; polygonindex < dwSectorInfo.numPolygons ; ++polygonindex)
			{
				// 해당 버텍스가 포함된 폴리건 찾아냄.
				if(
					//sector->polygons[ i ].matIndex
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].z	== fz
					)
				{
					// 찾았심.
					// 해당면의 노멀을 계산.
					RwV3dSub( &a,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );
					RwV3dSub( &b,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );

					RwV3dCrossProduct( &polygonnormal , &a , &b );
					RwV3dNormalize(&polygonnormal, &polygonnormal);

					RwV3dScale	( &polygonnormal, &polygonnormal, -1.0f);

					// normal에 더함.
					RwV3dAdd( &normal , &normal , &polygonnormal );
				}
			}
		}
	}
	// 왼쪽 아래
	if(	fx == pSector->GetXStart() && fz == pSector->GetYEnd() )
	{
		// 섹터 검색
		if( pSector->GetNearSector( TD_WEST ) && pSector->GetNearSector( TD_WEST )->GetNearSector( TD_SOUTH ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_WEST )->GetNearSector( TD_SOUTH );
		}
		else if( pSector->GetNearSector( TD_SOUTH ) && pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_WEST ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_WEST );
		}
		else
		{
			// do nothing;
		}
		if( pAlefSector && ( pAlefSector->IsLoadedCompactData() || pAlefSector->IsLoadedDetailData() ) && IsPolygonDataLoaded( pAlefSector ) )
		{
			GetDWSector( pAlefSector , &dwSectorInfo );
			ASSERT( count < UPDATE_ARRAY_MAX_COUNT );
			pAlefSectorUpdateArray[ count ++ ] = pAlefSector;
//			ASSERT( dwSectorInfo.numVertices >= 0	);
//			ASSERT( NULL != dwSectorInfo.vertices	);
//			ASSERT( NULL != dwSectorInfo.normals	);
			for( int polygonindex = 0 ; polygonindex < dwSectorInfo.numPolygons ; ++polygonindex)
			{
				// 해당 버텍스가 포함된 폴리건 찾아냄.
				if(
					//sector->polygons[ i ].matIndex
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].z	== fz
					)
				{
					// 찾았심.
					// 해당면의 노멀을 계산.
					RwV3dSub( &a,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );
					RwV3dSub( &b,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );

					RwV3dCrossProduct( &polygonnormal , &a , &b );
					RwV3dNormalize(&polygonnormal, &polygonnormal);

					RwV3dScale	( &polygonnormal, &polygonnormal, -1.0f);

					// normal에 더함.
					RwV3dAdd( &normal , &normal , &polygonnormal );
				}
			}
		}
	}
	// 아래
	if(	fz == pSector->GetYEnd() )
	{
		if( pSector->GetNearSector( TD_SOUTH ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_SOUTH );
		}
		else
		{
			// do nothing;
		}
		if( pAlefSector && ( pAlefSector->IsLoadedCompactData() || pAlefSector->IsLoadedDetailData() ) && IsPolygonDataLoaded( pAlefSector ) )
		{
			GetDWSector( pAlefSector , &dwSectorInfo );
			ASSERT( count < UPDATE_ARRAY_MAX_COUNT );
			pAlefSectorUpdateArray[ count ++ ] = pAlefSector;
//			ASSERT( dwSectorInfo.numVertices >= 0	);
//			ASSERT( NULL != dwSectorInfo.vertices	);
//			ASSERT( NULL != dwSectorInfo.normals	);
			for( int polygonindex = 0 ; polygonindex < dwSectorInfo.numPolygons ; ++polygonindex)
			{
				// 해당 버텍스가 포함된 폴리건 찾아냄.
				if(
					//sector->polygons[ i ].matIndex
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].z	== fz
					)
				{
					// 찾았심.
					// 해당면의 노멀을 계산.
					RwV3dSub( &a,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );
					RwV3dSub( &b,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );

					RwV3dCrossProduct( &polygonnormal , &a , &b );
					RwV3dNormalize(&polygonnormal, &polygonnormal);

					RwV3dScale	( &polygonnormal, &polygonnormal, -1.0f);

					// normal에 더함.
					RwV3dAdd( &normal , &normal , &polygonnormal );
				}
			}
		}
	}
	// 오른쪽 아래.
	if(	fx == pSector->GetXEnd()  && fz == pSector->GetZEnd() )
	{
		// 섹터 검색
		if( pSector->GetNearSector( TD_WEST ) && pSector->GetNearSector( TD_WEST )->GetNearSector( TD_SOUTH ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_WEST )->GetNearSector( TD_SOUTH );
		}
		else if( pSector->GetNearSector( TD_SOUTH ) && pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_WEST ) )
		{
			pAlefSector	= pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_WEST );
		}
		else
		{
			// do nothing;
		}
		if( pAlefSector && ( pAlefSector->IsLoadedCompactData() || pAlefSector->IsLoadedDetailData() ) && IsPolygonDataLoaded( pAlefSector ) )
		{
			GetDWSector( pAlefSector , &dwSectorInfo );
			ASSERT( count < UPDATE_ARRAY_MAX_COUNT );
			pAlefSectorUpdateArray[ count ++ ] = pAlefSector;
//			ASSERT( dwSectorInfo.numVertices >= 0	);
//			ASSERT( NULL != dwSectorInfo.vertices	);
//			ASSERT( NULL != dwSectorInfo.normals	);
			for( int polygonindex = 0 ; polygonindex < dwSectorInfo.numPolygons ; ++polygonindex)
			{
				// 해당 버텍스가 포함된 폴리건 찾아냄.
				if(
					//sector->polygons[ i ].matIndex
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].z	== fz ||
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].x	== fx &&
					dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].z	== fz
					)
				{
					// 찾았심.
					// 해당면의 노멀을 계산.
					RwV3dSub( &a,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );
					RwV3dSub( &b,
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ],
						&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );

					RwV3dCrossProduct( &polygonnormal , &a , &b );
					RwV3dNormalize(&polygonnormal, &polygonnormal);

					RwV3dScale	( &polygonnormal, &polygonnormal, -1.0f);

					// normal에 더함.
					RwV3dAdd( &normal , &normal , &polygonnormal );
				}
			}
		}
	}

	pAlefSector = pSector;

	if( pAlefSector && ( pAlefSector->IsLoadedCompactData() || pAlefSector->IsLoadedDetailData() ) && IsPolygonDataLoaded( pAlefSector ) )
	{
		GetDWSector( pAlefSector , &dwSectorInfo );
		ASSERT( count < UPDATE_ARRAY_MAX_COUNT );
		pAlefSectorUpdateArray[ count ++ ] = pAlefSector;
//		ASSERT( dwSectorInfo.numVertices >= 0	);
//		ASSERT( NULL != dwSectorInfo.vertices	);
//		ASSERT( NULL != dwSectorInfo.normals	);
		for( int polygonindex = 0 ; polygonindex < dwSectorInfo.numPolygons ; ++polygonindex)
		{
			// 해당 버텍스가 포함된 폴리건 찾아냄.
			if(
				//sector->polygons[ i ].matIndex
				dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].x	== fx &&
				dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ].z	== fz ||
				dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].x	== fx &&
				dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ].z	== fz ||
				dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].x	== fx &&
				dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ].z	== fz
				)
			{
				// 찾았심.
				// 해당면의 노멀을 계산.
				RwV3dSub( &a,
					&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 2 ] ],
					&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );
				RwV3dSub( &b,
					&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 1 ] ],
					&dwSectorInfo.vertices[ dwSectorInfo.polygons[ polygonindex ].vertIndex[ 0 ] ] );

				RwV3dCrossProduct	( &polygonnormal , &a , &b				);
				RwV3dNormalize		( &polygonnormal, &polygonnormal		);

				RwV3dScale			( &polygonnormal, &polygonnormal, -1.0f	);

				// normal에 더함.
				RwV3dAdd			( &normal , &normal , &polygonnormal	);
			}
		}
	}

	// normal을 normalize시킴..
	RwV3dNormalize(&normal, &normal);
	// 버텍스 업데이트.

	ASSERT( count <= UPDATE_ARRAY_MAX_COUNT && count >= 0 );
	for( int i = 0 ; i < count ; ++i  )
	{
		GetDWSector( pAlefSectorUpdateArray[ i ] , &dwSectorInfo );

//		ASSERT( dwSectorInfo.numVertices >= 0	);
//		ASSERT( NULL != dwSectorInfo.vertices	);
//		ASSERT( NULL != dwSectorInfo.normals	);
	
		for( int vertexindex = 0 ; vertexindex < dwSectorInfo.numVertices ; ++vertexindex )
		{
			if( dwSectorInfo.vertices[ vertexindex ].x == fx &&
				dwSectorInfo.vertices[ vertexindex ].z == fz )
			{
				dwSectorInfo.normals[ vertexindex ].x = ( normal.x );
				dwSectorInfo.normals[ vertexindex ].y = ( normal.y );
				dwSectorInfo.normals[ vertexindex ].z = ( normal.z );
			}
		}
	}
}

void		AgcmMap::MakeDetailWorld		( ProgressCallback pfCallback , void * pData )
{
	#ifdef USE_MFC

	#ifdef _DEBUG
	UINT32	uStartTime = GetTickCount();
	char str[ 256 ];
	#endif

	ASSERT( IsRangeApplied() );
	static	char	strMessage[] = "폴리건 데이타 생성";
	INT32 x1,x2,z1,z2;
	x1 = GetLoadRangeX1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeX1() ) % MAP_DEFAULT_DEPTH;
	z1 = GetLoadRangeZ1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeZ1() ) % MAP_DEFAULT_DEPTH;
	x2 = GetLoadRangeX2() - ( GetLoadRangeX2() % MAP_DEFAULT_DEPTH );
	z2 = GetLoadRangeZ2() - ( GetLoadRangeZ2() % MAP_DEFAULT_DEPTH );

	// 마고자 (2005-09-09 오후 3:20:05) : 
	// 잠시 블럭

	#ifdef _USE_MAP_EXPANSION_
	// 위아래좌우로 한디비젼씩 추가로함.
	x1 -= AgcmMap::EnumDivisionExpandValue;
	z1 -= AgcmMap::EnumDivisionExpandValue;
	x2 += AgcmMap::EnumDivisionExpandValue;
	z2 += AgcmMap::EnumDivisionExpandValue;
	#endif //_USE_MAP_EXPANSION_

	int		nTarget		=	( x2 - x1 + 1 )		*
							( z2 - z1 + 1 )		;
	int		nCurrent	= 1	;

	ApWorldSector * pSector;
	// 데이타는 한 섹터 넓게 잡아서 로딩한다.
	for( int x = x1 ; x < x2 ; x++ )
	{
		if( pfCallback ) pfCallback( strMessage , nCurrent += ( x2 - x1 ) , nTarget , pData );

		for( int z = z1 ; z < z2 ; z++ )
		{
			pSector			= m_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( pSector )
			{
				if( pSector->IsLoadedDetailData() )
				{
					// 러프 디테일 둘다 생성..
					Update				( pSector , SECTOR_LOWDETAIL	);
					Update				( pSector , SECTOR_HIGHDETAIL	);
					
					if( m_pfSectorWaterLoadCallback ) m_pfSectorWaterLoadCallback( pSector , SECTOR_HIGHDETAIL	);
				}
			}
			else
			{
//				TRACE( "섹터 포인터가 널입니다! (%d,%d) \n" , 
//					aSector[ x + ALEF_MAX_HORI_SECTOR * y ].nIndexSectorX ,
//					aSector[ x + ALEF_MAX_HORI_SECTOR * y ].nIndexSectorY );
			}
//			dlg.SetProgress( ++ count );

			//TRACE( "MakeDetail , %d/%d개 진행중\n" , ( x - GetLoadRangeX1() ) * ( GetLoadRangeZ2() - GetLoadRangeZ1() ) + z - GetLoadRangeZ1() , ( GetLoadRangeX2() - GetLoadRangeX1() ) * ( GetLoadRangeZ2() - GetLoadRangeZ1() ) );
		}
	}	

	nCurrent = 1;

	#ifdef _DEBUG
	sprintf( str , "맵로딩 경과시간 %.1f - Update완료\n" , ( FLOAT ) ( GetTickCount() - uStartTime ) / 1000.0f );
	TRACE( str );
	#endif

	SetCurrentDetail( SECTOR_HIGHDETAIL );

	_MaptoolClientSaveData * pSaveData;

	for( int x = x1 ; x < x2 ; x++ )
	{
		for( int z = z1 ; z < z2 ; z++ )
		{
			pSector			= m_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( pSector )
			{
				if( pSector->IsLoadedDetailData() )
				{
					{
						// 마고자 (2005-03-24 오전 10:57:17) : 
						// 세이브 데이타 얻어냄..
						AgcmMapSectorData	* pClientSectorData		= GetAgcmAttachedData( pSector );
						pSaveData = ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData2;
					}

					if( pSaveData && pSaveData->IsLoadedGeometry() )
					{
						// do nothing..
					}
					else
					{
						// 러프 디테일 둘다 생성..
						if( LockSector						( pSector , GetGeometryLockMode() , SECTOR_HIGHDETAIL	) )
						{
							RecalcNormal					( pSector , SECTOR_HIGHDETAIL							);
							UnlockSectors					(														);
						}
					}
				}
			}

			//TRACE( "MakeDetail , %d/%d개 진행중\n" , ( x - GetLoadRangeX1() ) * ( GetLoadRangeZ2() - GetLoadRangeZ1() ) + z - GetLoadRangeZ1() , ( GetLoadRangeX2() - GetLoadRangeX1() ) * ( GetLoadRangeZ2() - GetLoadRangeZ1() ) );
		}
	}	

	nCurrent = 1;

	#ifdef _DEBUG
	sprintf( str , "맵로딩 경과시간 %.1f - 디테일 노멀 업데이트\n" , ( FLOAT ) ( GetTickCount() - uStartTime ) / 1000.0f );
	TRACE( str );
	#endif

	SetCurrentDetail( SECTOR_LOWDETAIL );

	for( int x = x1 ; x < x2 ; x++ )
	{
		for( int z = z1 ; z < z2 ; z++ )
		{
			pSector			= m_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( pSector )
			{
				if( pSector->IsLoadedDetailData() )
				{
					{
						// 마고자 (2005-03-24 오전 10:57:17) : 
						// 세이브 데이타 얻어냄..
						AgcmMapSectorData	* pClientSectorData		= GetAgcmAttachedData( pSector );
						pSaveData = ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData1;
					}

					if( pSaveData && pSaveData->IsLoadedGeometry() )
					{
						// do nothing..
					}
					else
					{
						if( LockSector						( pSector , GetGeometryLockMode() , SECTOR_LOWDETAIL	) )
						{
							RecalcNormal					( pSector , SECTOR_LOWDETAIL							);
							UnlockSectors					(														);
						}
					}
				}
			}

			//TRACE( "MakeDetail , %d/%d개 진행중\n" , ( x - GetLoadRangeX1() ) * ( GetLoadRangeZ2() - GetLoadRangeZ1() ) + z - GetLoadRangeZ1() , ( GetLoadRangeX2() - GetLoadRangeX1() ) * ( GetLoadRangeZ2() - GetLoadRangeZ1() ) );
		}
	}	

	SetCurrentDetail( SECTOR_HIGHDETAIL );

	#ifdef _DEBUG
	sprintf( str , "맵로딩 경과시간 %.1f - 러프 노멀 업데이트\n" , ( FLOAT ) ( GetTickCount() - uStartTime ) / 1000.0f );
	TRACE( str );
	#endif

	for( int x = x1 ; x < x2 ; x++ )
	{
		for( int z = z1 ; z < z2 ; z++ )
		{
			pSector			= m_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( pSector )
			{
				// 마고자 (2005-03-24 오전 10:57:17) : 
				// 세이브 데이타 얻어냄..
				AgcmMapSectorData	* pClientSectorData		= GetAgcmAttachedData( pSector );

				pSaveData = ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData1;
				if( pSaveData ) pSaveData->DeleteContents();
				pSaveData = ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData2;
				if( pSaveData ) pSaveData->DeleteContents();
			}
		}
	}

	#ifdef _DEBUG
	sprintf( str , "맵로딩 경과시간 %.1f\n" , ( FLOAT ) ( GetTickCount() - uStartTime ) / 1000.0f );
	TRACE( str );
	#endif

	#endif // USE_MFC
}

BOOL AgcmMap::LockSector				( ApWorldSector * pSector	,  RwInt32 nLockMode , INT32 nDetail )
{
	// 섹터를 락하고 리스트로 남김..

	ASSERT( NULL != pSector );
	if( NULL == pSector )
		return FALSE;

	rsDWSectorInfo	dwSectorInfo	;

	if( nDetail == -1 )
	{
		nDetail = GetCurrentDetail( pSector );
	}

	// 마고자 (2004-06-21 오후 3:55:27) : 범위 체크..
	if( nDetail <= 0 )
		return FALSE;
	ASSERT( nDetail >= 0 && nDetail < DWSECTOR_MAX_DETAIL );	
	if( nDetail >= DWSECTOR_MAX_DETAIL )
		return FALSE;

	GetDWSector( pSector , nDetail , & dwSectorInfo );

	// 섹터 정보가 있나?..
	ASSERT( NULL != dwSectorInfo.pDWSector );
	if( dwSectorInfo.nDetail != nDetail				)
		return FALSE;
	if( NULL == dwSectorInfo.pDWSector				)
		return FALSE;
	if( NULL == dwSectorInfo.pDWSector->geometry	)
		return FALSE;

	// 락 리스트에 먼저 있는지 검사함..

	// ApplyAlpha 함수에서 Lock했던 모든 섹터를 언락 한다.

	AuNode< AgcmMap::stLockSectorInfo > *	pNode			= m_listLockedSectorList.GetHeadNode()	;
	AgcmMap::stLockSectorInfo			*	pSectorInList	;
	while( pNode )
	{
		pSectorInList = &pNode->GetData();

		if( pSectorInList->pSector == pSector	&&
			pSectorInList->nDetail == nDetail	)
		{
			// 이미 존재함..
			if( ( pSectorInList->nLockMode & nLockMode ) == nLockMode ) return TRUE;

			// 락모드가 틀리다!
			TRACE( "Lock 모드가 달라요!n" );
			// 락을 다시함..

			RpGeometryUnlock( dwSectorInfo.pDWSector->geometry );
			RpGeometryLock( dwSectorInfo.pDWSector->geometry , nLockMode | pSectorInList->nLockMode );

			pSectorInList->nLockMode	= nLockMode | pSectorInList->nLockMode;
			return TRUE;
		}
	
		m_listLockedSectorList.GetNext( pNode );
	}

	// 이전에 락하지 않았으므로 락에 들어감..

	if( RpGeometryLock( dwSectorInfo.pDWSector->geometry , nLockMode ) )
	{
		// 성공.
		// 리스트에 추가함..

		AgcmMap::stLockSectorInfo	stLock;
		stLock.pSector		= pSector	;
		stLock.nDetail		= nDetail	;
		stLock.nLockMode	= nLockMode	;

		m_listLockedSectorList.AddTail( stLock );
		return TRUE;
	}
	else
	{
		// 실패..
		ASSERT( !"MainWindow::LockSector 락 실패!" );
		return FALSE;
	}
}

BOOL AgcmMap::IsLockedSector			( ApWorldSector * pSector	, RwInt32 nLockMode	, INT32 nDetail )
{
	AuNode< AgcmMap::stLockSectorInfo > *	pNode		;
	AgcmMap::stLockSectorInfo			*	pLockInfo	;
	//rsDWSectorInfo						dwSectorInfo	;

	pNode = m_listLockedSectorList.GetHeadNode();

	while( pNode )
	{
		pLockInfo = &pNode->GetData();

		ASSERT( NULL != pLockInfo );

		if( pLockInfo->pSector		== pSector									&&
			( pLockInfo->nLockMode & nLockMode )	== 	nLockMode				&&
			pLockInfo->nDetail		== nDetail									)
		{
			return TRUE;
		}
	
		pNode = pNode->GetNextNode();
	}
	
	return FALSE;
}

BOOL AgcmMap::UnlockSectors			(  BOOL bUpdateCollision , BOOL bUpdateRough )	// 락한 모든 섹터를 언락함.
{
	AuNode< AgcmMap::stLockSectorInfo > *	pNode		;
	AgcmMap::stLockSectorInfo			*	pLockInfo	;
	rsDWSectorInfo						dwSectorInfo	;

	while( pNode = m_listLockedSectorList.GetHeadNode() )
	{
		pLockInfo = &pNode->GetData();

		ASSERT( NULL != pLockInfo );

		if( pLockInfo->pSector )
		{
			GetDWSector			( pLockInfo->pSector ,  pLockInfo->nDetail , & dwSectorInfo	);

			if( dwSectorInfo.nDetail == pLockInfo->nDetail	&&
				NULL != dwSectorInfo.pDWSector				&&	
				NULL != dwSectorInfo.pDWSector->geometry	)
			{
				// 마고자 (2004-06-21 오후 3:59:39) : 인자체크..

				RpGeometryUnlock	( dwSectorInfo.pDWSector->geometry	);

				if( bUpdateCollision )
				{
//					if( GetAsyncKeyState( VK_CONTROL ) < 0 )
//					{
//						// 컨트롤키 누르고 있으면 업데이트 안함..
//					}
//					else
//					{
//					}
					RpCollisionGeometryBuildData( dwSectorInfo.pDWSector->geometry , NULL );
					
					RpMorphTarget * pDWMorph = RpGeometryGetMorphTarget( dwSectorInfo.pDWSector->geometry , 0 );
					RwSphere	sphere;
					RpMorphTargetCalcBoundingSphere	( pDWMorph , &sphere );
					RpMorphTargetSetBoundingSphere	( pDWMorph , &sphere );

					dwSectorInfo.pDWSector->atomic->boundingSphere		= sphere;
					dwSectorInfo.pDWSector->atomic->worldBoundingSphere	= sphere;
				}

				if( bUpdateRough && pLockInfo->nDetail == SECTOR_LOWDETAIL )
				{
					Update( pLockInfo->pSector , SECTOR_LOWDETAIL );
				}
			}
		}
	
		m_listLockedSectorList.RemoveHead();
	}

	return TRUE;
}

BOOL	AgcmMap::LoadTargetPosition	( FLOAT fX , FLOAT fZ , ProgressCallback pfCallback , void * pData  )
{
	AuPOS	next;
	next.x	= fX;
	next.y	= 0.0f;
	next.z	= fZ;

	// 마고자 (2005-03-11 오후 6:46:05) : 
	// 읽기전에 플러시..
	// 문제가 있을지도 모름 -_-;;;
	ClearAllSectors();

	MyCharacterPositionChange( m_posCurrentAvatarPosition , next );

	while( m_csMapLoader.GetWaitCount()  )
	{
		m_csMapLoader.OnIdleLoad();
	}

	return TRUE;
}

BOOL	AgcmMap::LoadAll ( FLOAT	fX , FLOAT fZ , INT32 nRange , ProgressCallback pfCallback , void * pData )
{
	ASSERT( IsRangeApplied() );
	static	char	strMessage[] = "loading map data";
	int		nSectorX = PosToSectorIndexX( fX );
	int		nSectorZ = PosToSectorIndexZ( fZ );

	int		x1 , x2 , z1 , z2;

	// 범위계산..
	x1	= nSectorX	- nRange;
	x2	= nSectorX	+ nRange;
	z1	= nSectorZ	- nRange;
	z2	= nSectorZ	+ nRange;

	int		nTarget		=	( x2 - x1 + 1 )		*
							( z2 - z1 + 1 )		;
	int		nCurrent	= 1	;

	ApWorldSector * pSector;
	// 데이타는 한 섹터 넓게 잡아서 로딩한다.
	for( int x = x1 ; x <= x2; 	++x )
	{
		for( int z = z1 ; z < z2 ; ++z )
		{
			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );
			pSector = LoadSector( ArrayIndexToSectorIndexX( x ) , 0 , ArrayIndexToSectorIndexZ( z ) );
			// 푼 파일 제거.. 물론 압축 파일을 찾았을때만..
			if( pSector )
			{
				// 콜벡 강제호출해줌.
				OnLoadRough	( pSector );
				OnLoadDetail( pSector );
			}
			else
			{
				TRACE( "AgcmMap::LoadAll can't find sector\n" );
			}
		}
	}
				
	return TRUE;
}

// 전체 로딩..
BOOL AgcmMap::LoadAll( BOOL bCreate , ProgressCallback pfCallback , void * pData )
{
	ASSERT( IsRangeApplied() );
	static char strMessage[] = "loading map data";

	INT32 x1,x2,z1,z2;
	x1 = GetLoadRangeX1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeX1() ) % MAP_DEFAULT_DEPTH;
	z1 = GetLoadRangeZ1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeZ1() ) % MAP_DEFAULT_DEPTH;
	x2 = GetLoadRangeX2() - ( GetLoadRangeX2() % MAP_DEFAULT_DEPTH );
	z2 = GetLoadRangeZ2() - ( GetLoadRangeZ2() % MAP_DEFAULT_DEPTH );

	// 마고자 (2005-09-09 오후 3:20:05) : 
	// 잠시 블럭

	#ifdef _USE_MAP_EXPANSION_
	// 위아래좌우로 한디비젼씩 추가로함.
	x1 -= AgcmMap::EnumDivisionExpandValue;
	z1 -= AgcmMap::EnumDivisionExpandValue;
	x2 += AgcmMap::EnumDivisionExpandValue;
	z2 += AgcmMap::EnumDivisionExpandValue;
	#endif //_USE_MAP_EXPANSION_

	int		nTarget		=	( x2 - x1 + 1 )		*
							( z2 - z1 + 1 )		;
	int		nCurrent	= 1	;

	ApWorldSector * pSector;
	// 데이타는 한 섹터 넓게 잡아서 로딩한다.
	for( int x = x1 ; x < x2 ; x++ )
	{
		for( int z = z1 ; z < z2 ; z++ )
		{
			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );
			pSector = LoadSector( ArrayIndexToSectorIndexX( x ) , 0 , ArrayIndexToSectorIndexZ( z ) );

			if( !bCreate )
			{
				// 직접 읽어들인다.
				LoadDWSector( pSector );
			}
			// 푼 파일 제거.. 물론 압축 파일을 찾았을때만..

			// 콜벡 강제호출해줌.
			if( pSector )
			{
				// 콜벡 강제호출해줌.
				OnLoadRough	( pSector );
				OnLoadDetail( pSector );
			}
			else
			{
				TRACE( "AgcmMap::LoadAll can't find sector\n" );
			}
		}
	}	

	return TRUE;

}

BOOL	AgcmMap::SaveAll			( BOOL bGeometry , BOOL bTile , BOOL bCompact , ProgressCallback pfCallback , void * pData , char * pBackupDirectory )
{
	ASSERT( IsRangeApplied() );

	ApWorldSector * pSector			;
	//RpWorldSector * pWorldSector	;

	// 포함돼는 섹터의 모든 압축 파일을 품..

	int x , z;
	char				strFileName	[ 256	];
	char				strFullPath	[ 1024	];
	char				strFullTmp	[ 1024	];
	
	int	x1 , x2 , z1 , z2;	// 계산된범위..

	// MAP_DEFAULT_DEPTH 단위로 잘라냄...
	x1 = GetLoadRangeX1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeX1() ) % MAP_DEFAULT_DEPTH;
	z1 = GetLoadRangeZ1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeZ1() ) % MAP_DEFAULT_DEPTH;
	x2 = GetLoadRangeX2() - ( GetLoadRangeX2() % MAP_DEFAULT_DEPTH );
	z2 = GetLoadRangeZ2() - ( GetLoadRangeZ2() % MAP_DEFAULT_DEPTH );

	///////////////////////////////////////////////////////////////////////
	char	strMessage[256] = "Save All 작업중..";
	int		nTarget		=	( x2 - x1 )		*
							( z2 - z1 )		;
	int		nCurrent	= 1													;
	//////////////////////////////////////////////////////////////////////

	char				strDirCompact	[ 1024 ];
	char				strDirGeometry	[ 1024 ];
	char				strDirMoonee	[ 1024 ];

	int					nDivisionCount	= 0;

	wsprintf( strDirCompact	, "%s"	, this->m_pcsApmMap->GetMapCompactDirectory	() );
	wsprintf( strDirGeometry, "%s"	, this->m_pcsApmMap->GetMapGeometryDirectory() );
	wsprintf( strDirMoonee	, "%s"	, this->m_pcsApmMap->GetMapMooneeDirectory	() );

	// 디비젼 갯수 구함..
	for( x = x1 ; x < x2 ; x += MAP_DEFAULT_DEPTH )
		for( z = z1 ; z < z2 ; z += MAP_DEFAULT_DEPTH )
			nDivisionCount ++;

	// 저장하기..
	for( x = x1 ; x < x2 ; ++x  )
	{
		for( z = z1 ; z < z2 ; ++z  )
		{
			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );

			pSector			= m_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( pSector )
			{
				if( bCompact	)	pSector->SaveCompactData		();
				if( bGeometry	)	pSector->SaveDetailDataGeometry	();
				if( bTile		)	pSector->SaveDetailDataTile		();
			}
		}
	}	

	AuList< CMagPackHeaderInfo * >	listCompact		;
	AuList< CMagPackHeaderInfo * >	listGeometry	;
	AuList< CMagPackHeaderInfo * >	listMoonee		;
	CMagPackHeaderInfo *			pInfo			;

	int	localx , localz;
	ApWorldSector * pWorldSector;

	strcpy( strMessage , "디비젼 별로 붂는 중.." );
	nTarget		=	nDivisionCount * 5	;
	nCurrent	=	1					;

	// 섹터별로 압축..
	for( x = x1 ; x < x2 ; x += MAP_DEFAULT_DEPTH )
	{
		for( z = z1 ; z < z2 ; z += MAP_DEFAULT_DEPTH )
		{
			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );

			/////////////////////////////////////
			for( localz = z ; localz < z + MAP_DEFAULT_DEPTH ; ++localz  )
			{
				for( localx = x ; localx < x + MAP_DEFAULT_DEPTH ; ++localx )
				{
					pWorldSector	= m_pcsApmMap->GetSectorByArray( localx , 0 , localz );
					if( NULL == pWorldSector ) continue;

					if( bCompact	)
					{
						pInfo = new CMagPackHeaderInfo;
						sprintf( pInfo->filename , "%s\\C%d,%d.amf",
							m_pcsApmMap->GetMapCompactDirectory()	,
							localx									,
							localz									);
						listCompact.AddTail( pInfo );
					}

					if( bGeometry	)
					{
						pInfo = new CMagPackHeaderInfo;
						sprintf( pInfo->filename , "%s\\G%d,%d.amf",
							m_pcsApmMap->GetMapGeometryDirectory()	,
							localx									,
							localz									);
						listGeometry.AddTail( pInfo );					
					}

					if( bTile		)
					{
						pInfo = new CMagPackHeaderInfo;
						sprintf( pInfo->filename , "%s\\T%d,%d.amf",
							m_pcsApmMap->GetMapMooneeDirectory()	,
							localx									,
							localz									);
						listMoonee.AddTail( pInfo );					
					}

					// 루프..
				}
			}
			/////////////////////////////////
			TRACE( "스트림 클로즈.\n" );

			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );

			if( bCompact )
			{
				wsprintf( strFileName , SECTOR_COMPACT_BLOCK_FILE	, 
					GetDivisionIndex( x , z ) );
				wsprintf( strFullTmp , "%s\\%s" , strDirCompact	, "test.tmp" );
				wsprintf( strFullPath , "%s\\%s" , strDirCompact	, strFileName );
				TRACE( "Compact 압축중..\n" );
				if( PackFiles( strFullTmp	, &listCompact	) )
				{
					// 성공
					// 원본 파일 지우고..
					// 파일 이름을 바꿈..
					DeleteFile	( strFullPath );
					MoveFile	( strFullTmp , strFullPath ) ;

					if( pBackupDirectory )
					{
						wsprintf( strFullTmp , "%s\\Map\\Data\\Compact\\%s" , pBackupDirectory , strFileName );
						if( FALSE == CopyFile( strFullPath , strFullTmp , FALSE ) )
						{
							TRACE( "SaveAll , 파일카피에러 (%d)\n" , GetLastError() );
						}
					}
				}
				else
				{
					// 실패..
					return FALSE;
				}

			}

			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );

			if( bGeometry )
			{
				wsprintf( strFileName , SECTOR_GEOMETRY_BLOCK_FILE	, 
					GetDivisionIndex( x , z ) );
				wsprintf( strFullTmp , "%s\\%s" , strDirGeometry	, "test.tmp" );
				wsprintf( strFullPath , "%s\\%s" , strDirGeometry	, strFileName );

				TRACE( "Geometry 압축중..\n" );

				if( PackFiles( strFullTmp	, &listGeometry	) )
				{
					// 성공
					// 원본 파일 지우고..
					// 파일 이름을 바꿈..
					DeleteFile	( strFullPath );
					MoveFile	( strFullTmp , strFullPath ) ;

					if( pBackupDirectory )
					{
						wsprintf( strFullTmp , "%s\\Map\\Data\\Geometry\\%s" , pBackupDirectory , strFileName );
						if( FALSE == CopyFile( strFullPath , strFullTmp , FALSE ) )
						{
							TRACE( "SaveAll , 파일카피에러 (%d)\n" , GetLastError() );
						}
					}
				}
				else
				{
					// 실패..
					return FALSE;
				}

			}

			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );
			
			if( bTile )
			{
				wsprintf( strFileName , SECTOR_MOONIE_BLOCK_FILE	, 
					GetDivisionIndex( x , z ) );
				wsprintf( strFullTmp , "%s\\%s" , strDirMoonee	, "test.tmp" );
				wsprintf( strFullPath , "%s\\%s" , strDirMoonee		, strFileName );

				TRACE( "Moonee 압축중..\n" );

				if( PackFiles( strFullTmp	, &listMoonee	) )
				{
					// 성공
					// 원본 파일 지우고..
					// 파일 이름을 바꿈..
					DeleteFile	( strFullPath );
					MoveFile	( strFullTmp , strFullPath ) ;

					if( pBackupDirectory )
					{
						wsprintf( strFullTmp , "%s\\Map\\Data\\Moonee\\%s" , pBackupDirectory , strFileName );
						if( FALSE == CopyFile( strFullPath , strFullTmp , FALSE ) )
						{
							TRACE( "SaveAll , 파일카피에러 (%d)\n" , GetLastError() );
						}
					}
				}
				else
				{
					// 실패..
					return FALSE;
				}
			}

			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );
			
			TRACE( "데이타 정리중....\n" );

			// 리스트 제거..
			while( listCompact.GetHeadNode() )
			{
				pInfo = listCompact.GetHead();
				wsprintf( strFullPath , "%s\\%s" , strDirCompact	, pInfo->filename );
				DeleteFile( strFullPath );
				delete pInfo;
				listCompact.RemoveHead();
			}
			// 리스트 제거..
			while( listGeometry.GetHeadNode() )
			{
				pInfo = listGeometry.GetHead();
				wsprintf( strFullPath , "%s\\%s" , strDirGeometry	, pInfo->filename );
				DeleteFile( strFullPath );
				delete pInfo;
				listGeometry.RemoveHead();
			}
			// 리스트 제거..
			while( listMoonee.GetHeadNode() )
			{
				pInfo =  listMoonee.GetHead();
				wsprintf( strFullPath , "%s\\%s" , strDirMoonee		, pInfo->filename );
				DeleteFile( strFullPath );
				delete pInfo;
				listMoonee.RemoveHead();
			}

			TRACE( "한개완료\n" );
		}
	}

	// 기존 파일 제거..

	return TRUE;
}

BOOL	AgcmMap::SaveAll		( char * pTarget , BOOL bGeometry , BOOL bTile , BOOL bCompact , ProgressCallback pfCallback , void * pData )
{
	ASSERT( IsRangeApplied() );
	ASSERT( NULL != pTarget );

	ApWorldSector * pSector			;
	//RpWorldSector * pWorldSector	;

	// 포함돼는 섹터의 모든 압축 파일을 품..

	int x , z;
//	CMagUnpackManager	*pUnpack;
	char				strFileName	[ 256	];
	char				strFullPath	[ 1024	];
	char				strFullTmp	[ 1024	];
	
	int	x1 , x2 , z1 , z2;	// 계산된범위..

	// 10 단위로 잘라냄...
	x1 = GetLoadRangeX1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeX1() ) % MAP_DEFAULT_DEPTH;
	z1 = GetLoadRangeZ1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeZ1() ) % MAP_DEFAULT_DEPTH;
	x2 = GetLoadRangeX2() - ( GetLoadRangeX2() % MAP_DEFAULT_DEPTH );
	z2 = GetLoadRangeZ2() - ( GetLoadRangeZ2() % MAP_DEFAULT_DEPTH );

	char				strDirCompact	[ 1024 ];
	char				strDirGeometry	[ 1024 ];
	char				strDirMoonee	[ 1024 ];

	wsprintf( strDirCompact	, "%s\\Compact"		, pTarget );
	wsprintf( strDirGeometry, "%s\\Geometry"	, pTarget );
	wsprintf( strDirMoonee	, "%s\\Moonee"		, pTarget );

	CreateDirectory( pTarget		, NULL );
	CreateDirectory( strDirCompact	, NULL );
	CreateDirectory( strDirGeometry	, NULL );
	CreateDirectory( strDirMoonee	, NULL );

//	for( x = x1 ; x < x2 ; x += MAP_DEFAULT_DEPTH )
//	{
//		for( z = z1 ; z < z2 ; z += MAP_DEFAULT_DEPTH )
//		{
//			if( bCompact	)
//			{
//				// 파일 이름 생성
//				//wsprintf( strFileName , SECTOR_COMPACT_BLOCK_FILE , 
//				//	GetDivisionIndex( x , z ) );
//				pUnpack = m_pcsApmMap->GetUnpackManagerDivisionCompact( GetDivisionIndex( x , z ) );
//				wsprintf( strFullPath , "%s\\%s" , strDirCompact , strFileName );
//				// 압축 풀기..
//
//				if( pUnpack )
//				{
//					pUnpack->GetAllFiles( strDirCompact );
//				}
//			}
//
//			if( bGeometry	)
//			{
//				// 파일 이름 생성
//				//wsprintf( strFileName , SECTOR_GEOMETRY_BLOCK_FILE , 
//				//	GetDivisionIndex( x , z ) );
//				pUnpack = m_pcsApmMap->GetUnpackManagerDivisionGeometry( GetDivisionIndex( x , z ) );
//				wsprintf( strFullPath , "%s\\%s" , strDirGeometry , strFileName );
//				// 압축 풀기..
//
//				if( pUnpack )
//				{
//					pUnpack->GetAllFiles( strDirGeometry );
//				}
//			}
//
//			if( bTile		)
//			{
//				// 파일 이름 생성
//				//wsprintf( strFileName , SECTOR_MOONIE_BLOCK_FILE , 
//				//	GetDivisionIndex( x , z ) );
//				pUnpack = m_pcsApmMap->GetUnpackManagerDivisionMoonee( GetDivisionIndex( x , z ) );
//				wsprintf( strFullPath , "%s\\%s" , strDirMoonee , strFileName );
//				// 압축 풀기..
//
//				if( pUnpack )
//				{
//					pUnpack->GetAllFiles( strDirMoonee );
//				}
//			}
//
//			// 루프..
//		}
//	}

	// 저장하기..
	for( x = x1 ; x < x2 ; ++x  )
	{
		for( z = z1 ; z < z2 ; ++z  )
		{
			pSector			= m_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( pSector )
			{
				if( bCompact	)	pSector->SaveCompactData		();
				if( bGeometry	)	pSector->SaveDetailDataGeometry	();
				if( bTile		)	pSector->SaveDetailDataTile		();
			}
		}
	}
	AuList< CMagPackHeaderInfo * >	listCompact		;
	AuList< CMagPackHeaderInfo * >	listGeometry	;
	AuList< CMagPackHeaderInfo * >	listMoonee		;
	CMagPackHeaderInfo *			pInfo			;

	int	localx , localz;
	ApWorldSector * pWorldSector;

	// 섹터별로 압축..
	for( x = x1 ; x < x2 ; x += MAP_DEFAULT_DEPTH )
	{
		for( z = z1 ; z < z2 ; z += MAP_DEFAULT_DEPTH )
		{
			/////////////////////////////////////
			for( localz = z ; localz < z + MAP_DEFAULT_DEPTH ; ++localz  )
			{
				for( localx = x ; localx < x + MAP_DEFAULT_DEPTH ; ++localx )
				{
					pWorldSector	= m_pcsApmMap->GetSectorByArray( localx , 0 , localz );
					if( NULL == pWorldSector ) continue;

					if( bCompact	)
					{
						pInfo = new CMagPackHeaderInfo;
						sprintf( pInfo->filename , "%s\\C%d,%d.amf",
							m_pcsApmMap->GetMapCompactDirectory()	,
							localx									,
							localz									);
						listCompact.AddTail( pInfo );
					}

					if( bGeometry	)
					{
						pInfo = new CMagPackHeaderInfo;
						sprintf( pInfo->filename , "%s\\G%d,%d.amf",
							m_pcsApmMap->GetMapGeometryDirectory()	,
							localx									,
							localz									);
						listGeometry.AddTail( pInfo );					
					}

					if( bTile		)
					{
						pInfo = new CMagPackHeaderInfo;
						sprintf( pInfo->filename , "%s\\T%d,%d.amf",
							m_pcsApmMap->GetMapMooneeDirectory()	,
							localx									,
							localz									);
						listMoonee.AddTail( pInfo );					
					}

					// 루프..
				}
			}
			/////////////////////////////////
			TRACE( "스트림 클로즈.\n" );

			if( bCompact )
			{
				wsprintf( strFileName , SECTOR_COMPACT_BLOCK_FILE	, 
					GetDivisionIndex( x , z ) );
				wsprintf( strFullTmp , "%s\\%s" , strDirCompact	, "test.tmp" );
				wsprintf( strFullPath , "%s\\%s" , strDirCompact	, strFileName );
				TRACE( "Compact 압축중..\n" );
				if( PackFiles( strFullTmp	, &listCompact	) )
				{
					// 성공
					// 원본 파일 지우고..
					// 파일 이름을 바꿈..
					DeleteFile	( strFullPath );
					MoveFile	( strFullTmp , strFullPath ) ;
				}
				else
				{
					// 실패..
					return FALSE;
				}

			}

			if( bGeometry )
			{
				wsprintf( strFileName , SECTOR_GEOMETRY_BLOCK_FILE	, 
					GetDivisionIndex( x , z ) );
				wsprintf( strFullTmp , "%s\\%s" , strDirGeometry	, "test.tmp" );
				wsprintf( strFullPath , "%s\\%s" , strDirGeometry	, strFileName );

				TRACE( "Geometry 압축중..\n" );

				if( PackFiles( strFullTmp	, &listGeometry	) )
				{
					// 성공
					// 원본 파일 지우고..
					// 파일 이름을 바꿈..
					DeleteFile	( strFullPath );
					MoveFile	( strFullTmp , strFullPath ) ;
				}
				else
				{
					// 실패..
					return FALSE;
				}

			}

			if( bTile )
			{
				wsprintf( strFileName , SECTOR_MOONIE_BLOCK_FILE	, 
					GetDivisionIndex( x , z ) );
				wsprintf( strFullTmp , "%s\\%s" , strDirMoonee	, "test.tmp" );
				wsprintf( strFullPath , "%s\\%s" , strDirMoonee		, strFileName );

				TRACE( "Moonee 압축중..\n" );

				if( PackFiles( strFullTmp	, &listMoonee	) )
				{
					// 성공
					// 원본 파일 지우고..
					// 파일 이름을 바꿈..
					DeleteFile	( strFullPath );
					MoveFile	( strFullTmp , strFullPath ) ;
				}
				else
				{
					// 실패..
					return FALSE;
				}
			}

			TRACE( "데이타 정리중....\n" );
			// 리스트 제거..
			while( listCompact.GetHeadNode() )
			{
				pInfo = listCompact.GetHead();
				wsprintf( strFullPath , "%s\\%s" , strDirCompact	, pInfo->filename );
				DeleteFile( strFullPath );
				delete pInfo;
				listCompact.RemoveHead();
			}
			// 리스트 제거..
			while( listGeometry.GetHeadNode() )
			{
				pInfo = listGeometry.GetHead();
				wsprintf( strFullPath , "%s\\%s" , strDirGeometry	, pInfo->filename );
				DeleteFile( strFullPath );
				delete pInfo;
				listGeometry.RemoveHead();
			}
			// 리스트 제거..
			while( listMoonee.GetHeadNode() )
			{
				pInfo =  listMoonee.GetHead();
				wsprintf( strFullPath , "%s\\%s" , strDirMoonee		, pInfo->filename );
				DeleteFile( strFullPath );
				delete pInfo;
				listMoonee.RemoveHead();
			}

			TRACE( "한개완료\n" );
		}
	}

	// 기존 파일 제거..

	return TRUE;
}


BOOL	AgcmMap::D_SetHeight				( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z , FLOAT height , BOOL bReculsive , BOOL bUpdateNormal )
{
	ASSERT( NULL != pSector														);
	ASSERT( NULL != m_pcsApmMap													);
	ASSERT( SECTOR_EMPTY <= nTargetDetail && nTargetDetail < SECTOR_DETAILDEPTH	);

	// 그냥 변경을 하기도 하고..
	// 경계면의 경우는.. 해당 부분에 대한 처리를 해준다.
	// R_LEFT , R_TOP 은.. 섹터 경계에 부분을 기준으로 직선을 이루도록한다.
	// R_RIGHT R_BOTTOM 은 아래쪽, 오른쪽 섹터의 퍼스트 라인을 기준으로 한다.

	float	fx , fz;
	rsDWSectorInfo	dwSectorInfo;
	rsDWSectorInfo	dwSectorInfoAnother;
	rsDWSectorInfo	*pDwSector	;
	stFindMapInfo	stFindInfo	;
	ApDetailSegment * pSegment = pSector->D_GetSegment( nTargetDetail , x , z , &fx , &fz );

//	ASSERT( NULL != pSegment );
	
	if( pSegment )
	{
		RwV3d	normal			;
		RwV3d	normalSmooth	;

		// Smooth Normal 을 위해서..
		const	INT32	NORMAL_MAX	= 32;
		RwV3d *	pNormalList	[ NORMAL_MAX ];
		INT32	nIndex		= 0;

		normalSmooth.x	= 0.0f;
		normalSmooth.y	= 0.0f;
		normalSmooth.z	= 0.0f;

		// 높이 변화..
		pSector->D_SetHeight( nTargetDetail , x , z , height );

		// 섹터와 관계있따.. 이건 커런트에서만 사용함.
		if( nTargetDetail != pSector->GetCurrentDetail() ) return FALSE;
		
		// 폴리건 정보까지 변경함..
		GetDWSector	( pSector , nTargetDetail , & dwSectorInfo			);

		ASSERT( NULL != dwSectorInfo.pDWSector->geometry );

		#define	UpdateNomalMacro( polygonIndex , offset )	{																	\
			pDwSector->vertices[ pDwSector->polygons[ ( polygonIndex ) ].vertIndex[ offset ] ].y = height;						\
			if( bUpdateNormal )																									\
			{																													\
				__CalcPolygonNormal( pDwSector , ( polygonIndex ) , &normal );													\
				if( pSegment->GetFlag( ApDetailSegment::USEFLATSHADING ) )														\
				{																												\
					pDwSector->normals[ pDwSector->polygons[ ( polygonIndex ) ].vertIndex[ offset ] ]	= normal;				\
				}																												\
				else																											\
				{																												\
					ASSERT( nIndex < NORMAL_MAX );																				\
					if( nIndex < NORMAL_MAX )																					\
						pNormalList[ nIndex++ ] = &pDwSector->normals[ pDwSector->polygons[ ( polygonIndex ) ].vertIndex[ offset ] ];\
				}																												\
				RwV3dAdd			( &normalSmooth , &normalSmooth , &normal	);												\
			}																													\
		}

		if( LockSector	( pSector , GetGeometryLockMode() , nTargetDetail ) )
		{
			if( FALSE == bReculsive )
			{
				BOOL	bLeft = TRUE , bTop = TRUE , bLeftTop = TRUE;

				if( x == 0 && GetWorldSectorInfo( &stFindInfo , pSector , x - 1 , z ) )
				{
					if( IsLockedSector( stFindInfo.pSector , GetGeometryLockMode() , nTargetDetail ) )
						bLeft = TRUE;
					else
						bLeft = FALSE;
				}

				if( z == 0 && GetWorldSectorInfo( &stFindInfo , pSector , x , z - 1 ) )
				{
					if( IsLockedSector( stFindInfo.pSector , GetGeometryLockMode() , nTargetDetail ) )
						bTop = TRUE;
					else
						bTop = FALSE;
				}

				if( x == 0 && z == 0 && GetWorldSectorInfo( &stFindInfo , pSector , x - 1 , z - 1 ) )
				{
					if( IsLockedSector( stFindInfo.pSector , GetGeometryLockMode() , nTargetDetail ) )
						bLeftTop = TRUE;
					else
						bLeftTop = FALSE;
				}

				// 마고자 (2004-06-28 오후 4:42:59) : 리컬시브에서는 처리하지 않음..
				if( !bLeft || !bTop || !bLeftTop )
					return FALSE;
			}

			if( GetWorldSectorInfo( &stFindInfo , pSector , x , z ) )
			{
				// 폴리건 정보 얻어냄..
				if( pSector != stFindInfo.pSector )
				{
					GetDWSector( stFindInfo.pSector , &dwSectorInfoAnother );
					pDwSector = & dwSectorInfoAnother	;
					VERIFY( LockSector	( stFindInfo.pSector , GetGeometryLockMode() , nTargetDetail ) );
				}
				else
				{
					pDwSector = & dwSectorInfo			;
				}

				if( stFindInfo.pSegment->stTileInfo.GetEdgeTurn() )
				{
					// 양쪽다 계산..
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst		, 0 );
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst + 1	, 0 );
				}
				else
				{
					// 왼쪽 위에것만 계산..
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst		, 0 );
				}
			}

			if( GetWorldSectorInfo( &stFindInfo , pSector , x - 1 , z ) )
			{
				// 폴리건 정보 얻어냄..
				if( pSector != stFindInfo.pSector )
				{
					GetDWSector( stFindInfo.pSector , &dwSectorInfoAnother );
					pDwSector = & dwSectorInfoAnother	;
					VERIFY( LockSector	( stFindInfo.pSector , GetGeometryLockMode() , nTargetDetail ) );
				}
				else
				{
					pDwSector = & dwSectorInfo			;
				}

				if( stFindInfo.pSegment->stTileInfo.GetEdgeTurn() )
				{
					// 오른쪽 위에것..
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst + 1	, 2 );
				}
				else
				{
					// 양쪽다 계산..
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst		, 2 );
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst + 1	, 2 );
				}
			}
			if( GetWorldSectorInfo( &stFindInfo , pSector , x , z - 1 ) )
			{
				// 폴리건 정보 얻어냄..
				if( pSector != stFindInfo.pSector )
				{
					GetDWSector( stFindInfo.pSector , &dwSectorInfoAnother );
					pDwSector = & dwSectorInfoAnother	;
					VERIFY( LockSector	( stFindInfo.pSector , GetGeometryLockMode() , nTargetDetail ) );
				}
				else
				{
					pDwSector = & dwSectorInfo			;
				}


				if( stFindInfo.pSegment->stTileInfo.GetEdgeTurn() )
				{
					// 왼쪽 아래것..
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst		, 1 );
				}
				else
				{
					// 양쪽다 계산..
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst		, 1 );
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst + 1	, 0 );
				}
			}
			if( GetWorldSectorInfo( &stFindInfo , pSector , x - 1 , z - 1 ) )
			{
				// 폴리건 정보 얻어냄..
				if( pSector != stFindInfo.pSector )
				{
					GetDWSector( stFindInfo.pSector , &dwSectorInfoAnother );
					pDwSector = & dwSectorInfoAnother	;
					VERIFY( LockSector	( stFindInfo.pSector , GetGeometryLockMode() , nTargetDetail ) );
				}
				else
				{
					pDwSector = & dwSectorInfo			;
				}


				if( stFindInfo.pSegment->stTileInfo.GetEdgeTurn() )
				{
					// 양쪽다 계산..
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst		, 2 );
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst + 1	, 1 );
				}
				else
				{
					// 오른쪽 아래것 계산..
					UpdateNomalMacro( stFindInfo.nPolygonIndexFirst + 1	, 1 );
				}
			}

		}

		if( bUpdateNormal )
		{
			RwV3dNormalize( &normalSmooth, &normalSmooth );

			while( nIndex -- )
			{
				pNormalList[ nIndex ]->x	= normalSmooth.x;
				pNormalList[ nIndex ]->y	= normalSmooth.y;
				pNormalList[ nIndex ]->z	= normalSmooth.z;

			}
		}

		#ifdef USE_MFC
		GetAgcmAttachedData( pSector )->m_bValidGrid = FALSE;
		#endif

		return TRUE;
	}
	else
		return FALSE;
}

BOOL	AgcmMap::D_SetValue				( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z , ApRGBA value	)
{
	ASSERT( NULL != pSector														);
	ASSERT( NULL != m_pcsApmMap													);
	ASSERT( SECTOR_EMPTY <= nTargetDetail && nTargetDetail < SECTOR_DETAILDEPTH	);

	float	fx , fz;
	rsDWSectorInfo	dwSectorInfo;
	ApDetailSegment * pSegment = pSector->D_GetSegment( nTargetDetail , x , z , &fx , &fz );
	
	if( pSegment )
	{
		//pSegment->vertexcolor	= value;
		pSector->D_SetValue( nTargetDetail , x , z , value );


		// 섹터와 관계있따.. 이건 커런트에서만 사용함.
		if( nTargetDetail != pSector->GetCurrentDetail() ) return FALSE;
		
		// 폴리건 정보까지 변경함..
		GetDWSector	( pSector ,  nTargetDetail , & dwSectorInfo				);

		if( dwSectorInfo.preLitLum )
		{
			int vertexindex;
			// 프리라잇 값이 있을때만
			if( LockSector	( pSector , GetVertexColorLockMode() , nTargetDetail ) )
			{
				for( vertexindex = 0 ; vertexindex < dwSectorInfo.numVertices ; ++vertexindex  )
				{
					if( dwSectorInfo.vertices[ vertexindex ].x == fx &&
						dwSectorInfo.vertices[ vertexindex ].z == fz )
					{
						_RwRGBAAssigApRGB( dwSectorInfo.preLitLum[ vertexindex ] , value );
					}
				}
			}

			// 주변 폴리건 정보를 변경함.

			if( x == 0 && pSector->GetNearSector( TD_WEST ) )
			{
				GetDWSector	( pSector->GetNearSector( TD_WEST ) ,  nTargetDetail , & dwSectorInfo			);
				if( LockSector	( pSector->GetNearSector( TD_WEST ) , GetVertexColorLockMode() , nTargetDetail	) )
				{
					for( vertexindex = 0 ; vertexindex < dwSectorInfo.numVertices ; ++vertexindex  )
					{
						if( dwSectorInfo.vertices[ vertexindex ].x == fx &&
							dwSectorInfo.vertices[ vertexindex ].z == fz )
						{
							_RwRGBAAssigApRGB( dwSectorInfo.preLitLum[ vertexindex ] , value );
						}
					}
				}
			}

			if( z == 0 && pSector->GetNearSector( TD_NORTH ) )
			{
				GetDWSector	( pSector->GetNearSector( TD_NORTH ) ,  nTargetDetail , & dwSectorInfo			);
				if( LockSector	( pSector->GetNearSector( TD_NORTH ) , GetVertexColorLockMode()	, nTargetDetail	) )
				{
					for( vertexindex = 0 ; vertexindex < dwSectorInfo.numVertices ; ++vertexindex  )
					{
						if( dwSectorInfo.vertices[ vertexindex ].x == fx &&
							dwSectorInfo.vertices[ vertexindex ].z == fz )
						{
							_RwRGBAAssigApRGB( dwSectorInfo.preLitLum[ vertexindex ] , value );
						}
					}
				}
			}

			if( x == 0 && z == 0 )
			{
				ApWorldSector * pNearSector = NULL;
				if( pSector->GetNearSector( TD_WEST		) && pSector->GetNearSector( TD_WEST	)->GetNearSector( TD_NORTH	)  )
					pNearSector = pSector->GetNearSector( TD_WEST	)->GetNearSector( TD_NORTH	);
				if( pSector->GetNearSector( TD_NORTH	) && pSector->GetNearSector( TD_NORTH	)->GetNearSector( TD_WEST	)  )
					pNearSector = pSector->GetNearSector( TD_NORTH	)->GetNearSector( TD_WEST	);

				if( pNearSector )
				{
					GetDWSector	( pNearSector , nTargetDetail , & dwSectorInfo				);
					if( LockSector	( pNearSector , GetVertexColorLockMode() , nTargetDetail	) )
					{
						for( vertexindex = 0 ; vertexindex < dwSectorInfo.numVertices ; ++vertexindex  )
						{
							if( dwSectorInfo.vertices[ vertexindex ].x == fx &&
								dwSectorInfo.vertices[ vertexindex ].z == fz )
							{
								_RwRGBAAssigApRGB( dwSectorInfo.preLitLum[ vertexindex ] , value );
							}
						}
					}
				}
			}


		}
		return TRUE;
	}
	else
		return FALSE;
}

BOOL	AgcmMap::D_GetEdgeTurn	( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z )
{
	// 마고자 (2004-06-15 오전 11:22:23) : 엣지턴 처리..
	ASSERT( NULL != pSector														);
	ASSERT( NULL != m_pcsApmMap													);
	ASSERT( SECTOR_EMPTY <= nTargetDetail && nTargetDetail < SECTOR_DETAILDEPTH	);

	ApDetailSegment *	pSegment = pSector->D_GetSegment( nTargetDetail , x , z );

	if( pSegment )
	{
		return pSegment->stTileInfo.GetEdgeTurn();
	}
	else return FALSE;
}

BOOL	AgcmMap::D_GetEdgeTurn	( FLOAT fX , FLOAT fZ )
{
	ASSERT( NULL != m_pcsApmMap													);

	ApWorldSector * pSector = m_pcsApmMap->GetSector( fX , fZ );
	ASSERT( NULL != pSector														);
	if( NULL == pSector ) return FALSE;

	ApDetailSegment *	pSegment = pSector->D_GetSegment( SECTOR_HIGHDETAIL , fX , fZ );

	if( pSegment )
	{
		return pSegment->stTileInfo.GetEdgeTurn();
	}
	else return FALSE;
}

BOOL	AgcmMap::D_SetEdgeTurn	( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z , BOOL bEdge	)
{
	// 마고자 (2004-06-15 오전 11:22:29) : 엣지턴 처리..
	ASSERT( NULL != pSector														);
	ASSERT( NULL != m_pcsApmMap													);
	ASSERT( SECTOR_EMPTY <= nTargetDetail && nTargetDetail < SECTOR_DETAILDEPTH	);

	ApDetailSegment *	pSegment = pSector->D_GetSegment( nTargetDetail , x , z );

	if( pSegment )
	{
		if( pSegment->stTileInfo.GetEdgeTurn() == bEdge )
		{
			return TRUE;
		}
		else
		{
			pSegment->stTileInfo.SetEdgeTurn( bEdge );

			// 폴리건 변경..
			rsDWSectorInfo	dwSectorInfo	;
			GetDWSector( pSector , &dwSectorInfo );

			if( LockSector	( pSector , rpGEOMETRYLOCKALL , nTargetDetail ) )
			{
				stFindMapInfo	FindMapInfo;
				if( GetWorldSectorInfo( &FindMapInfo , pSector , x , z ) )
				{
					int k = FindMapInfo.nFirstVertexIndex;

					if( pSegment->stTileInfo.GetEdgeTurn() )
					{
						dwSectorInfo.vertices	[ FindMapInfo.nFirstVertexIndex			+ 4 ] = dwSectorInfo.vertices	[ FindMapInfo.nFirstVertexIndex			+ 0 ];	
						dwSectorInfo.texCoords	[ 0 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 0 ][ FindMapInfo.nFirstVertexIndex	+ 0 ];	
						dwSectorInfo.texCoords	[ 1 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 1 ][ FindMapInfo.nFirstVertexIndex	+ 0 ];	
						dwSectorInfo.texCoords	[ 2 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 2 ][ FindMapInfo.nFirstVertexIndex	+ 0 ];	
						dwSectorInfo.texCoords	[ 3 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 3 ][ FindMapInfo.nFirstVertexIndex	+ 0 ];	
						dwSectorInfo.texCoords	[ 4 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 4 ][ FindMapInfo.nFirstVertexIndex	+ 0 ];	
						dwSectorInfo.texCoords	[ 5 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 5 ][ FindMapInfo.nFirstVertexIndex	+ 0 ];	
						dwSectorInfo.vertices	[ FindMapInfo.nFirstVertexIndex			+ 5 ] = dwSectorInfo.vertices	[ FindMapInfo.nFirstVertexIndex			+ 3 ];	
						dwSectorInfo.texCoords	[ 0 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 0 ][ FindMapInfo.nFirstVertexIndex	+ 3 ];	
						dwSectorInfo.texCoords	[ 1 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 1 ][ FindMapInfo.nFirstVertexIndex	+ 3 ];	
						dwSectorInfo.texCoords	[ 2 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 2 ][ FindMapInfo.nFirstVertexIndex	+ 3 ];	
						dwSectorInfo.texCoords	[ 3 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 3 ][ FindMapInfo.nFirstVertexIndex	+ 3 ];	
						dwSectorInfo.texCoords	[ 4 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 4 ][ FindMapInfo.nFirstVertexIndex	+ 3 ];	
						dwSectorInfo.texCoords	[ 5 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 5 ][ FindMapInfo.nFirstVertexIndex	+ 3 ];	
						
						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst		].vertIndex[ 0 ]	= k + 0		;
						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst		].vertIndex[ 1 ]	= k + 2		;
						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst		].vertIndex[ 2 ]	= k	+ 3		;

						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst + 1	].vertIndex[ 0 ]	= k + 4		;
						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst + 1	].vertIndex[ 1 ]	= k + 5		;
						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst + 1	].vertIndex[ 2 ]	= k + 1		;
					}
					else
					{
						dwSectorInfo.vertices	[ FindMapInfo.nFirstVertexIndex			+ 4 ] = dwSectorInfo.vertices	[ FindMapInfo.nFirstVertexIndex			+ 2 ];	
						dwSectorInfo.texCoords	[ 0 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 0 ][ FindMapInfo.nFirstVertexIndex	+ 2 ];	
						dwSectorInfo.texCoords	[ 1 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 1 ][ FindMapInfo.nFirstVertexIndex	+ 2 ];	
						dwSectorInfo.texCoords	[ 2 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 2 ][ FindMapInfo.nFirstVertexIndex	+ 2 ];	
						dwSectorInfo.texCoords	[ 3 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 3 ][ FindMapInfo.nFirstVertexIndex	+ 2 ];	
						dwSectorInfo.texCoords	[ 4 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 4 ][ FindMapInfo.nFirstVertexIndex	+ 2 ];	
						dwSectorInfo.texCoords	[ 5 ][ FindMapInfo.nFirstVertexIndex	+ 4 ] = dwSectorInfo.texCoords	[ 5 ][ FindMapInfo.nFirstVertexIndex	+ 2 ];	
						dwSectorInfo.vertices	[ FindMapInfo.nFirstVertexIndex			+ 5 ] = dwSectorInfo.vertices	[ FindMapInfo.nFirstVertexIndex			+ 1 ];	
						dwSectorInfo.texCoords	[ 0 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 0 ][ FindMapInfo.nFirstVertexIndex	+ 1 ];	
						dwSectorInfo.texCoords	[ 1 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 1 ][ FindMapInfo.nFirstVertexIndex	+ 1 ];	
						dwSectorInfo.texCoords	[ 2 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 2 ][ FindMapInfo.nFirstVertexIndex	+ 1 ];	
						dwSectorInfo.texCoords	[ 3 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 3 ][ FindMapInfo.nFirstVertexIndex	+ 1 ];	
						dwSectorInfo.texCoords	[ 4 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 4 ][ FindMapInfo.nFirstVertexIndex	+ 1 ];	
						dwSectorInfo.texCoords	[ 5 ][ FindMapInfo.nFirstVertexIndex	+ 5 ] = dwSectorInfo.texCoords	[ 5 ][ FindMapInfo.nFirstVertexIndex	+ 1 ];	
						
						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst		].vertIndex[ 0 ]	= k + 0		;
						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst		].vertIndex[ 1 ]	= k + 2		;
						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst		].vertIndex[ 2 ]	= k	+ 1		;

						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst + 1	].vertIndex[ 0 ]	= k + 4		;
						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst + 1	].vertIndex[ 1 ]	= k + 3		;
						dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst + 1	].vertIndex[ 2 ]	= k + 5		;
					}
				}
			}
		}////////////

		return TRUE;
	}
	else return FALSE;
}

BOOL	AgcmMap::D_SetTile				( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z ,
															UINT32 firsttexture		,
															UINT32 secondtexture	,
															UINT32 thirdtexture		,
															UINT32 fourthtexture	,
															UINT32 fifthtexture		,
															UINT32 sixthtexture		)
{
	ASSERT( NULL != pSector														);
	ASSERT( NULL != m_pcsApmMap													);
	ASSERT( SECTOR_EMPTY <= nTargetDetail && nTargetDetail < SECTOR_DETAILDEPTH	);

	if( firsttexture == 0x01010180 ) firsttexture = ALEF_SECTOR_DEFAULT_TILE_INDEX;

	float fx , fz;

	float fZoomRate = 1.0f;
	// 러프맵 텍스쳐 처리..
	if( nTargetDetail == SECTOR_LOWDETAIL )	fZoomRate = 2.5f;

	ApDetailSegment *	pSegment = pSector->D_GetSegment( nTargetDetail , x , z , &fx , &fz );

	if( pSegment )
	{
		if(	pSegment->pIndex[ TD_FIRST	] == firsttexture	&& 
			pSegment->pIndex[ TD_SECOND	] == secondtexture	&&
			pSegment->pIndex[ TD_THIRD	] == thirdtexture	&&
			pSegment->pIndex[ TD_FOURTH	] == fourthtexture	&&
			pSegment->pIndex[ TD_FIFTH	] == fifthtexture	&&
			pSegment->pIndex[ TD_SIXTH	] == sixthtexture	)
		{
			// 같으므로.. 아무일 안함..
			return FALSE;
		}

		pSector->D_SetTile( nTargetDetail , x , z , firsttexture , secondtexture , thirdtexture	, fourthtexture	, fifthtexture , sixthtexture );

		if( nTargetDetail != pSector->GetCurrentDetail() ) return FALSE;

		// 폴리건 정보 변경함..

		rsDWSectorInfo	dwSectorInfo	;
		GetDWSector( pSector , &dwSectorInfo );

		if( LockSector	( pSector , rpGEOMETRYLOCKPOLYGONS | rpGEOMETRYLOCKTEXCOORDSALL	, nTargetDetail ) )
		{
			int materialindex;

			// 마고자 (2005-03-15 오후 5:48:02) : 지형 효과 처리..
			if( pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
			{
				materialindex = GetMaterialList( pSector ) [ pSector->GetCurrentDetail() ].GetMatIndex( firsttexture ) ;
			}
			else
			{
				materialindex = GetMaterialList( pSector ) [ pSector->GetCurrentDetail() ].GetMatIndex(
					firsttexture ,	secondtexture ,	thirdtexture	, fourthtexture	, fifthtexture , sixthtexture ) ;
			}

			int				nVertexFirst	;
			stFindMapInfo	FindMapInfo		;
			if( GetWorldSectorInfo( &FindMapInfo , pSector , x , z ) )
			{
				dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst		].matIndex	= materialindex;
				dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst + 1	].matIndex	= materialindex;

				// 마고자 (2005-03-15 오후 5:48:02) : 지형 효과 처리..
				if( pSector->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE )
				{
					nVertexFirst = dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst		].vertIndex[ 0 ];

					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 0 ].u = GET_TEXTURE_U_START	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 0 ].v = GET_TEXTURE_V_START	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 1 ].u = GET_TEXTURE_U_END	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 1 ].v = GET_TEXTURE_V_START	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 2 ].u = GET_TEXTURE_U_START	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 2 ].v = GET_TEXTURE_V_END	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 3 ].u = GET_TEXTURE_U_END	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 3 ].v = GET_TEXTURE_V_END	( firsttexture ) * fZoomRate;
					
					SectorPreviwTextureSetup( &FindMapInfo , firsttexture );

					if( pSegment->stTileInfo.GetEdgeTurn() )
					{
						dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 0 ];
						dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 3 ];
					}
					else
					{
						dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 2 ];
						dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 1 ];
					}

				}
				else
				{
					nVertexFirst = dwSectorInfo.polygons[ FindMapInfo.nPolygonIndexFirst		].vertIndex[ 0 ];

					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 0 ].u = GET_TEXTURE_U_START	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 0 ].v = GET_TEXTURE_V_START	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 1 ].u = GET_TEXTURE_U_END	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 1 ].v = GET_TEXTURE_V_START	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 2 ].u = GET_TEXTURE_U_START	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 2 ].v = GET_TEXTURE_V_END	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 3 ].u = GET_TEXTURE_U_END	( firsttexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 3 ].v = GET_TEXTURE_V_END	( firsttexture ) * fZoomRate;
					
					SectorPreviwTextureSetup( &FindMapInfo , firsttexture );

					dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 0 ].u = GET_TEXTURE_U_START	( secondtexture );
					dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 0 ].v = GET_TEXTURE_V_START	( secondtexture );
					dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 1 ].u = GET_TEXTURE_U_END	( secondtexture );
					dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 1 ].v = GET_TEXTURE_V_START	( secondtexture );
					dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 2 ].u = GET_TEXTURE_U_START	( secondtexture );
					dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 2 ].v = GET_TEXTURE_V_END	( secondtexture );
					dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 3 ].u = GET_TEXTURE_U_END	( secondtexture );
					dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 3 ].v = GET_TEXTURE_V_END	( secondtexture );

					dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 0 ].u = GET_TEXTURE_U_START	( thirdtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 0 ].v = GET_TEXTURE_V_START	( thirdtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 1 ].u = GET_TEXTURE_U_END	( thirdtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 1 ].v = GET_TEXTURE_V_START	( thirdtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 2 ].u = GET_TEXTURE_U_START	( thirdtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 2 ].v = GET_TEXTURE_V_END	( thirdtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 3 ].u = GET_TEXTURE_U_END	( thirdtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 3 ].v = GET_TEXTURE_V_END	( thirdtexture ) * fZoomRate;

					dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 0 ].u = GET_TEXTURE_U_START	( fourthtexture );
					dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 0 ].v = GET_TEXTURE_V_START	( fourthtexture );
					dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 1 ].u = GET_TEXTURE_U_END	( fourthtexture );
					dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 1 ].v = GET_TEXTURE_V_START	( fourthtexture );
					dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 2 ].u = GET_TEXTURE_U_START	( fourthtexture );
					dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 2 ].v = GET_TEXTURE_V_END	( fourthtexture );
					dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 3 ].u = GET_TEXTURE_U_END	( fourthtexture );
					dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 3 ].v = GET_TEXTURE_V_END	( fourthtexture );

					dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 0 ].u = GET_TEXTURE_U_START	( fifthtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 0 ].v = GET_TEXTURE_V_START	( fifthtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 1 ].u = GET_TEXTURE_U_END	( fifthtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 1 ].v = GET_TEXTURE_V_START	( fifthtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 2 ].u = GET_TEXTURE_U_START	( fifthtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 2 ].v = GET_TEXTURE_V_END	( fifthtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 3 ].u = GET_TEXTURE_U_END	( fifthtexture ) * fZoomRate;
					dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 3 ].v = GET_TEXTURE_V_END	( fifthtexture ) * fZoomRate;

					dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 0 ].u = GET_TEXTURE_U_START	( sixthtexture );
					dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 0 ].v = GET_TEXTURE_V_START	( sixthtexture );
					dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 1 ].u = GET_TEXTURE_U_END	( sixthtexture );
					dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 1 ].v = GET_TEXTURE_V_START	( sixthtexture );
					dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 2 ].u = GET_TEXTURE_U_START	( sixthtexture );
					dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 2 ].v = GET_TEXTURE_V_END	( sixthtexture );
					dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 3 ].u = GET_TEXTURE_U_END	( sixthtexture );
					dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 3 ].v = GET_TEXTURE_V_END	( sixthtexture );

					if( pSegment->stTileInfo.GetEdgeTurn() )
					{
						dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 0 ];
						dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 0 ];
						dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 0 ];
						dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 0 ];
						dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 0 ];
						dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 0 ];

						dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 3 ];
						dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 3 ];
						dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 3 ];
						dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 3 ];
						dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 3 ];
						dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 3 ];
					}
					else
					{
						dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 2 ];
						dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 2 ];
						dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 2 ];
						dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 2 ];
						dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 2 ];
						dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 4 ] = dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 2 ];

						dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 0 ][ nVertexFirst + 1 ];
						dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 1 ][ nVertexFirst + 1 ];
						dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 2 ][ nVertexFirst + 1 ];
						dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 3 ][ nVertexFirst + 1 ];
						dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 4 ][ nVertexFirst + 1 ];
						dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 5 ] = dwSectorInfo.texCoords[ 5 ][ nVertexFirst + 1 ];
					}

				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL	AgcmMap::GetWorldSectorInfo( stFindMapInfo * pMapInfo , ApWorldSector * pSector , int x , int z , int detailedX , int detailedZ )
{
	ASSERT( NULL != pMapInfo	);
	ASSERT( NULL != pSector		);
// 해당섹터 해당 와 좌표를 입력하고 , 실제 폴리건 정보를 얻는다.
	if( x >= pSector->D_GetDepth() && z >= pSector->D_GetDepth() )
	{
		if( pSector->GetNearSector( TD_SOUTH ) && pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) &&
			pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST )->IsLoadedDetailData() )
		{
			x		= x - pSector->D_GetDepth();
			z		= z - pSector->D_GetDepth();

			pSector = pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST );

			return GetWorldSectorInfo( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		if( pSector->GetNearSector( TD_EAST ) && pSector->GetNearSector( TD_WEST )->GetNearSector( TD_SOUTH ) &&
			pSector->GetNearSector( TD_EAST )->GetNearSector( TD_SOUTH )->IsLoadedDetailData() )
		{
			x		= x - pSector->D_GetDepth();
			z		= z - pSector->D_GetDepth();
			pSector	= pSector->GetNearSector( TD_EAST )->GetNearSector( TD_SOUTH );

			return GetWorldSectorInfo( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}
	else if( x >= pSector->D_GetDepth() )
	{
		if( pSector->GetNearSector( TD_EAST ) && pSector->GetNearSector( TD_EAST )->IsLoadedDetailData() )
		{
			x = x - pSector->D_GetDepth();
			pSector = pSector->GetNearSector( TD_EAST );
			return GetWorldSectorInfo( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}
	else if( z >= pSector->D_GetDepth() )
	{
		if( pSector->GetNearSector( TD_SOUTH ) && pSector->GetNearSector( TD_SOUTH )->IsLoadedDetailData() )
		{
			z = z - pSector->D_GetDepth();
			pSector = pSector->GetNearSector( TD_SOUTH );
			return GetWorldSectorInfo( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}

	if( x < 0 && z < 0 )
	{
		if( pSector->GetNearSector( TD_NORTH ) && pSector->GetNearSector( TD_NORTH )->GetNearSector( TD_WEST ) &&
			pSector->GetNearSector( TD_NORTH )->GetNearSector( TD_WEST )->IsLoadedDetailData() )
		{
			pSector	= pSector->GetNearSector( TD_NORTH )->GetNearSector( TD_WEST );
			x		= pSector->D_GetDepth() + x;
			z		= pSector->D_GetDepth() + z;

			return GetWorldSectorInfo( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		if( pSector->GetNearSector( TD_WEST ) && pSector->GetNearSector( TD_WEST )->GetNearSector( TD_NORTH ) &&
			pSector->GetNearSector( TD_WEST )->GetNearSector( TD_NORTH )->IsLoadedDetailData() )
		{
			pSector	= pSector->GetNearSector( TD_WEST )->GetNearSector( TD_NORTH );
			x		= pSector->D_GetDepth() + x;
			z		= pSector->D_GetDepth() + z;

			return GetWorldSectorInfo( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}
	else if( x < 0 )
	{
		if( pSector->GetNearSector( TD_WEST ) && pSector->GetNearSector( TD_WEST )->IsLoadedDetailData() )
		{
			pSector	= pSector->GetNearSector( TD_WEST );
			x		= pSector->D_GetDepth() + x;
			return GetWorldSectorInfo( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}
	else if( z < 0 )
	{
		if( pSector->GetNearSector( TD_NORTH ) && pSector->GetNearSector( TD_NORTH )->IsLoadedDetailData() )
		{
			pSector	= pSector->GetNearSector( TD_NORTH );
			z		= pSector->D_GetDepth() + z;
			return GetWorldSectorInfo( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}
	
	// 해당 폴리건 번호를 찾아낸다.
	// 좌표를 구해냄
	RwV3d	R_LEFTTOP , R_RIGHTBOTTOM;
	ApDetailSegment	* pSegment;

	// 세그먼트 정보 얻어냄..
	pSegment		= pSector->D_GetSegment( x , z , &R_LEFTTOP.x , &R_LEFTTOP.z )	;
	if( NULL == pSegment )
	{
		pMapInfo->Empty();
		return FALSE;
	}
	R_RIGHTBOTTOM.x	= R_LEFTTOP.x + pSector->GetStepSizeX()						;
	R_RIGHTBOTTOM.z	= R_LEFTTOP.z + pSector->GetStepSizeZ()						;

	//float stepsize = ( ( pSector->GetXEnd() - pSector->GetXStart() ) / ( ( float ) pSector->m_nDepth[ pSector->GetCurrentDetail() ] ) );
	
	// R_LEFTTOP , R_RIGHTBOTTOM 에는 y값은 의미가 없고 , 각각 좌표를 가진다.


	// 해당 폴리건을 찾아냄.
	pMapInfo->nCurrentDetail		= RpDWSectorGetCurrentDetail( pSector );

	switch( pMapInfo->nCurrentDetail )
	{
	case SECTOR_LOWDETAIL:
	case SECTOR_HIGHDETAIL:
		pMapInfo->bSuccess					= TRUE				;
		pMapInfo->nPolygonIndexFirst		= ( pSector->D_GetDepth() * z + x ) * 2	;
		pMapInfo->pSector					= pSector			;
		pMapInfo->pSegment					= pSegment			;
		pMapInfo->nSegmentX					= x					;
		pMapInfo->nSegmentZ					= z					;

		pMapInfo->bEdgeTurn					= pSegment->stTileInfo.GetEdgeTurn();
		pMapInfo->nFirstVertexIndex			= ( pSector->D_GetDepth() * z + x ) * 6	;
		return TRUE;

	default:
		pMapInfo->Empty();
		return FALSE;	
	}
}

BOOL	AgcmMap::GetWorldSectorInfoOnly( stFindMapInfo * pMapInfo , ApWorldSector * pSector , int x , int z , int detailedX , int detailedZ )
{
	ASSERT( NULL != pMapInfo	);
	ASSERT( NULL != pSector		);
// 해당섹터 해당 와 좌표를 입력하고 , 실제 폴리건 정보를 얻는다.
	if( x >= pSector->D_GetDepth() && z >= pSector->D_GetDepth() )
	{
		if( pSector->GetNearSector( TD_SOUTH ) && pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) &&
			pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST )->IsLoadedDetailData() )
		{
			x		= x - pSector->D_GetDepth();
			z		= z - pSector->D_GetDepth();

			pSector = pSector->GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST );

			return GetWorldSectorInfoOnly( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		if( pSector->GetNearSector( TD_EAST ) && pSector->GetNearSector( TD_WEST )->GetNearSector( TD_SOUTH ) &&
			pSector->GetNearSector( TD_EAST )->GetNearSector( TD_SOUTH )->IsLoadedDetailData() )
		{
			x		= x - pSector->D_GetDepth();
			z		= z - pSector->D_GetDepth();
			pSector	= pSector->GetNearSector( TD_EAST )->GetNearSector( TD_SOUTH );

			return GetWorldSectorInfoOnly( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}
	else if( x >= pSector->D_GetDepth() )
	{
		if( pSector->GetNearSector( TD_EAST ) && pSector->GetNearSector( TD_EAST )->IsLoadedDetailData() )
		{
			x = x - pSector->D_GetDepth();
			pSector = pSector->GetNearSector( TD_EAST );
			return GetWorldSectorInfoOnly( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}
	else if( z >= pSector->D_GetDepth() )
	{
		if( pSector->GetNearSector( TD_SOUTH ) && pSector->GetNearSector( TD_SOUTH )->IsLoadedDetailData() )
		{
			z = z - pSector->D_GetDepth();
			pSector = pSector->GetNearSector( TD_SOUTH );
			return GetWorldSectorInfoOnly( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}

	if( x < 0 && z < 0 )
	{
		if( pSector->GetNearSector( TD_NORTH ) && pSector->GetNearSector( TD_NORTH )->GetNearSector( TD_WEST ) &&
			pSector->GetNearSector( TD_NORTH )->GetNearSector( TD_WEST )->IsLoadedDetailData() )
		{
			pSector	= pSector->GetNearSector( TD_NORTH )->GetNearSector( TD_WEST );
			x		= pSector->D_GetDepth() + x;
			z		= pSector->D_GetDepth() + z;

			return GetWorldSectorInfoOnly( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		if( pSector->GetNearSector( TD_WEST ) && pSector->GetNearSector( TD_WEST )->GetNearSector( TD_NORTH ) &&
			pSector->GetNearSector( TD_WEST )->GetNearSector( TD_NORTH )->IsLoadedDetailData() )
		{
			pSector	= pSector->GetNearSector( TD_WEST )->GetNearSector( TD_NORTH );
			x		= pSector->D_GetDepth() + x;
			z		= pSector->D_GetDepth() + z;

			return GetWorldSectorInfoOnly( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}
	else if( x < 0 )
	{
		if( pSector->GetNearSector( TD_WEST ) && pSector->GetNearSector( TD_WEST )->IsLoadedDetailData() )
		{
			pSector	= pSector->GetNearSector( TD_WEST );
			x		= pSector->D_GetDepth() + x;
			return GetWorldSectorInfoOnly( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}
	else if( z < 0 )
	{
		if( pSector->GetNearSector( TD_NORTH ) && pSector->GetNearSector( TD_NORTH )->IsLoadedDetailData() )
		{
			pSector	= pSector->GetNearSector( TD_NORTH );
			z		= pSector->D_GetDepth() + z;
			return GetWorldSectorInfoOnly( pMapInfo , pSector , x , z , detailedX , detailedZ );
		}
		else
		{
			pMapInfo->Empty();
			return FALSE;
		}
	}
	
	// 해당 폴리건 번호를 찾아낸다.
	// 좌표를 구해냄
	RwV3d	R_LEFTTOP , R_RIGHTBOTTOM;
	//int		i;
	ApDetailSegment	* pSegment;

	// 세그먼트 정보 얻어냄..
	pSegment		= pSector->D_GetSegment( x , z , &R_LEFTTOP.x , &R_LEFTTOP.z )	;
	R_RIGHTBOTTOM.x	= R_LEFTTOP.x + pSector->GetStepSizeX()						;
	R_RIGHTBOTTOM.z	= R_LEFTTOP.z + pSector->GetStepSizeZ()						;

	pMapInfo->Empty();
	pMapInfo->nCurrentDetail		= RpDWSectorGetCurrentDetail( pSector );

	pMapInfo->bSuccess					= TRUE				;
	pMapInfo->pSector					= pSector			;
	pMapInfo->pSegment					= pSegment			;
	pMapInfo->nSegmentX					= x					;
	pMapInfo->nSegmentZ					= z					;
	
	return TRUE;

}


void AgcmMap::SectorPreviwTextureSetup( stFindMapInfo * pMapInfo , int tileindex  )
{
	if( tileindex != ALEF_SECTOR_DEFAULT_TILE_INDEX ) return;

	rsDWSectorInfo	dwSectorInfo;
	GetDWSector( pMapInfo->pSector ,  & dwSectorInfo );
	
	int nVertexOffset = dwSectorInfo.polygons[ pMapInfo->nPolygonIndexFirst	].vertIndex[ 0 ];
	_PreviewTextureSetup( pMapInfo->pSector , pMapInfo->nSegmentX + 0 , pMapInfo->nSegmentZ + 0 , tileindex , &dwSectorInfo.texCoords[ 0 ][ nVertexOffset + 0 ] );
	_PreviewTextureSetup( pMapInfo->pSector , pMapInfo->nSegmentX + 1 , pMapInfo->nSegmentZ + 0 , tileindex , &dwSectorInfo.texCoords[ 0 ][ nVertexOffset + 1 ] );
	_PreviewTextureSetup( pMapInfo->pSector , pMapInfo->nSegmentX + 0 , pMapInfo->nSegmentZ + 1 , tileindex , &dwSectorInfo.texCoords[ 0 ][ nVertexOffset + 2 ] );
	_PreviewTextureSetup( pMapInfo->pSector , pMapInfo->nSegmentX + 1 , pMapInfo->nSegmentZ + 1 , tileindex , &dwSectorInfo.texCoords[ 0 ][ nVertexOffset + 3 ] );
}

BOOL	AgcmMap::IsInSectorRadius		( ApWorldSector * pSector , float x , float z , float radius )
{
	if( radius == 0.0f )
	{
		if( x >= pSector->GetXStart() && x < pSector->GetXEnd() && z >= pSector->GetZStart() && z < pSector->GetZEnd() )
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		RwSphere	sphere;

		// 스피어 설정.
		sphere.center.x	= x		;
		sphere.center.y	= z		;
		sphere.center.z	= 0.0f	;

		sphere.radius	= radius;

		// 트라이엥글 설정
		// 0 1
		// 2 3
		RwV3d	vertex[ 4 ];
		vertex[ 0 ].x	=	pSector->GetXStart	()	;
		vertex[ 0 ].y	=	pSector->GetZStart	()	;
		vertex[ 0 ].z	=	0.0f					;

		vertex[ 1 ].x	=	pSector->GetXEnd	()	;
		vertex[ 1 ].y	=	pSector->GetZStart	()	;
		vertex[ 1 ].z	=	0.0f					;

		vertex[ 2 ].x	=	pSector->GetXStart	()	;
		vertex[ 2 ].y	=	pSector->GetZEnd	()	;
		vertex[ 2 ].z	=	0.0f					;

		vertex[ 3 ].x	=	pSector->GetXEnd	()	;
		vertex[ 3 ].y	=	pSector->GetZEnd	()	;
		vertex[ 3 ].z	=	0.0f					;

		RwV3d	normal;
		normal.x	= 0.0f;
		normal.y	= 0.0f;
		normal.z	= 1.0f;

		RwReal	distance = 0.0f;

		if( RtIntersectionSphereTriangle( &sphere , vertex + 0 , vertex + 1 , vertex + 2 , &normal , &distance ) )
		{
			// 아자!..
			return TRUE;
		}
		else
		if( RtIntersectionSphereTriangle( &sphere , vertex + 1 , vertex + 3 , vertex + 2 , &normal , &distance ) )
		{
			// 아자!..
			return TRUE;
		}
		else return FALSE;
		return FALSE;
	}
	
}

RpAtomic *	AgcmMap::_GetHeightCallback(
							 RpIntersection *	intersection	,
							 RpWorldSector *	sector			,
							 RpAtomic *			atomic			,
							 RwReal				distance		,
							 void *				data			)
{
	_FindHeightStruct	* pHeightStruct	= ( _FindHeightStruct * ) data;
	RpClump				* pClump		;

	INT32		index		;

	INT32		nType		= AcuObject::GetAtomicType( atomic , & index );

	pClump = RpAtomicGetClump( atomic );

	// 클럼프가 있으면 클럼프의 타입을 사용한다.
	if( pClump )
	{
		nType		= AcuObject::GetClumpType( pClump , & index );
	}
	else
		return atomic;

	switch( ACUOBJECT_TYPE_TYPEFILEDMASK & nType )
	{
	case ACUOBJECT_TYPE_NONE			:
		// Do no op
		// 맵툴에서 로딩한건 이거 세팅 안돼어 있나봄.
		return atomic;

	case ACUOBJECT_TYPE_OBJECT			:
		{
			// Ridable 만 체크함..
			INT32	nProperty = AcuObject::GetProperty( nType );
			if( nProperty & ACUOBJECT_TYPE_RIDABLE )
				break;
			else
				return atomic;
		}
		break;

	case ACUOBJECT_TYPE_CHARACTER		:
		// Do no op
 		return atomic;
	case ACUOBJECT_TYPE_ITEM			:
		// Do no op
		return atomic;
	case ACUOBJECT_TYPE_WORLDSECTOR		:
		// Do no op
		return atomic;
	default:
		return atomic;
	}

	// 실제 아토믹의 거리를 찾는다..
	RpIntersection	atomic_intersection	;
	atomic_intersection.type	=	rpINTERSECTLINE			;
	atomic_intersection.t.line	=	intersection->t.line	;

	FLOAT			fRealDistance	= 1000000.0f;

	RpAtomic	* pCollisionAtomic	= NULL		;

	if( pClump )	pCollisionAtomic = (RpAtomic*)pClump->stType.pCollisionAtomic;
	else			pCollisionAtomic = AcuObject::GetAtomicGetCollisionAtomic	( atomic );

	if( pCollisionAtomic )
	{
		if( RpAtomicGetGeometry( pCollisionAtomic ) &&
			RpGeometryGetTriangles(RpAtomicGetGeometry( pCollisionAtomic )))
		{
			AGCMMAP_THIS->LockFrame();

			RpAtomicForAllIntersections (
				pCollisionAtomic							,
				&atomic_intersection						,
				AgcmMap::_IntersectionCallBackFindDistance	,
				( void * ) &fRealDistance					);

			AGCMMAP_THIS->UnlockFrame();
		}
	}
	else
	{
		if( RpAtomicGetGeometry( atomic ) &&
			RpGeometryGetTriangles(RpAtomicGetGeometry( atomic )))
		{
			AGCMMAP_THIS->LockFrame();

			RpAtomicForAllIntersections (
				atomic										,
				&atomic_intersection						,
				AgcmMap::_IntersectionCallBackFindDistance	,
				( void * ) &fRealDistance					);

			AGCMMAP_THIS->UnlockFrame();
		}
	}

	// 콜리젼 데이타 초기화
	
	FLOAT	fDistance	= intersection->t.line.start.y - 
		( intersection->t.line.start.y - intersection->t.line.end.y ) * fRealDistance ;

	// 마고자 (2005-05-11 오후 12:57:00) : 
	// '보스방 계단' 이란 오브젝트에서 이상하게 이값이 떨어지는 경우가 발생한다...
	// 뭐가 문제지 -_-?...
	if( fRealDistance == 0.0f )
		return atomic;

	if( fRealDistance == 1000000.0f )
		return atomic;

	if( pHeightStruct->fHeight < fDistance && fDistance < pHeightStruct->fCharacterHeight )
	{
		// 케릭터 높이보단 낮아야 한다..
		pHeightStruct->fHeight = fDistance;
	}

	// 일단 케싱을 하지 않도록 변경한다..
	// 이까지오면 올라탈 수 있는 오브젝트가 존재한다.
	pHeightStruct->bExistRidableObject	= TRUE;

	return atomic;	
}

RpAtomic *	AgcmMap::_GetHeightClientCallback(
							 RpIntersection *	intersection	,
							 RpWorldSector *	sector			,
							 RpAtomic *			atomic			,
							 RwReal				distance		,
							 void *				data			)
{
	_FindHeightStruct	*		pHeightStruct	= ( _FindHeightStruct * ) data;
	//FLOAT						* pHeight		 = ( FLOAT * ) data;
	RpClump						* pClump		;
	RpAtomic					* pAtomicGet	= atomic;

	INT32		index		;

	INT32		nType		;

	pClump = RpAtomicGetClump( atomic );

	if( pClump )
	{
		// 아토믹에 없으면..클럼프엔 있나 확인해야함..
		nType		= AcuObject::GetClumpType( pClump , & index , NULL , NULL , NULL , (PVOID *) &pAtomicGet );
	}
	else
	{
		nType		= AcuObject::GetAtomicType( atomic , & index , NULL , NULL , NULL , (PVOID *) &pAtomicGet );
	}


	if( ( ( ( INT32 ) ACUOBJECT_TYPE_TYPEFILEDMASK ) & nType ) != ( ( INT32 ) ACUOBJECT_TYPE_WORLDSECTOR )  )
		return atomic;

	// 실제 아토믹의 거리를 찾는다..
	RpIntersection	atomic_intersection	;
	atomic_intersection.type	=	rpINTERSECTLINE			;
	atomic_intersection.t.line	=	intersection->t.line	;

	FLOAT			fRealDistance		= 1000000.0f;

	RpAtomic	* pCollisionAtomic = NULL;

	if( pClump )	pCollisionAtomic = (RpAtomic*)pClump->stType.pCollisionAtomic;
	else			pCollisionAtomic = (RpAtomic*)atomic->stType->pCollisionAtomic;

	if( pCollisionAtomic )
	{
		pAtomicGet = pCollisionAtomic;
	}

	if( NULL == pAtomicGet )
	{
		pAtomicGet = atomic;
	}

	if( RpAtomicGetGeometry( pAtomicGet ) && pAtomicGet->geometry->triangles )
	{
		AGCMMAP_THIS->LockFrame();

		RpAtomicForAllIntersections (
			pAtomicGet									,
			&atomic_intersection						,
			AgcmMap::_IntersectionCallBackFindDistance	,
			( void * ) &fRealDistance					);

		AGCMMAP_THIS->UnlockFrame();
	}
	// 콜리젼 데이타 초기화
	
	FLOAT	fDistance	= intersection->t.line.start.y - ( intersection->t.line.start.y - intersection->t.line.end.y ) * fRealDistance ;

	if( fRealDistance == 1000000.0f )
		return atomic;

	if( pHeightStruct->fHeight	< fDistance				)
	{
		pHeightStruct->fHeight				= fDistance	;
		// 이까지오면 올라탈 수 있는 오브젝트가 있음.
		pHeightStruct->bCollisionAvailable	= TRUE		;
	}

	return atomic;	
}


FLOAT	AgcmMap::HP_GetHeight( ApWorldSector * pSector , int x , int z , FLOAT fY )
{
	PROFILE("AgcmMap::HP_GetHeight");

	ASSERT( x >= 0 && x < MAP_DEFAULT_DEPTH );
	ASSERT( z >= 0 && z < MAP_DEFAULT_DEPTH );
	
	FLOAT	fX , fZ;
	
	FLOAT	* pHeight;
	// fX , fZ의 높이를 계산한다..

	pSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z , &fX , &fZ );

	pHeight = pSector->HP_GetHeight( x , z );

	if( pHeight )
	{
		return *pHeight;
	}
	else
	{
		BOOL	bCash = FALSE;
		FLOAT	fHeight = GetHeight_Lowlevel( fX , fZ , fY , & bCash );

		if( pSector->IsLoadedCompactData() && fHeight == INVALID_HEIGHT )
		{
			fHeight = pSector->C_GetHeight( x , z );
		}

		if( bCash )
		{
			// 무조건 캐싱함..
			pSector->HP_SetHeight( x , z , fHeight );
		}
		
		return	fHeight;
	}
}

FLOAT	AgcmMap::HP_GetHeight	( FLOAT fX , FLOAT fZ , FLOAT fY )
{
	int x , z;
	FLOAT	fSegmentX , fSegmentZ;
	// fX , fZ의 높이를 계산한다..

	ApWorldSector * pSector;
	pSector = m_pcsApmMap->GetSector( fX , fZ );

	if( pSector )
	{
		pSector->D_GetSegment( SECTOR_HIGHDETAIL , fX , fZ , &x , &z );
		// 그리드 관련 정보를 얻음..
		pSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z , &fSegmentX , &fSegmentZ );

		if( fX == fSegmentX && fZ == fSegmentZ )
		{
			// 격자면 격자용 펑션으로 날림..
			return HP_GetHeight( pSector , x , z , fY );
		}

		// 격자가 아닌경우는..
		// 그냥 콜리젼을 때림..

		return GetHeight_Lowlevel( fX , fZ , fY );
	}
	else
	{
		return 1.0f;
	}
}

FLOAT	AgcmMap::HP_GetHeightGeometryOnly	( ApWorldSector * pSector , int x , int z , FLOAT fY )
{
	PROFILE("AgcmMap::HP_GetHeightGeometryOnly");

	ASSERT( x >= 0 && x < MAP_DEFAULT_DEPTH );
	ASSERT( z >= 0 && z < MAP_DEFAULT_DEPTH );
	
	FLOAT	fX , fZ;
	
	FLOAT	* pHeight;
	// fX , fZ의 높이를 계산한다..

	pSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z , &fX , &fZ );

	pHeight = pSector->HP_GetHeightGeometryOnly( x , z );

	if( pHeight )
	{
		return *pHeight;
	}
	else
	{
		BOOL	bCash = FALSE;
		FLOAT	fHeight = GetHeight_Lowlevel_HeightOnly( fX , fZ , & bCash );

		if( pSector->IsLoadedCompactData() && fHeight == INVALID_HEIGHT )
		{
			fHeight = pSector->C_GetHeight( x , z );
		}

		if( bCash )
		{
			// 무조건 캐싱함..
			pSector->HP_SetHeightGeometryOnly( x , z , fHeight );
		}
		
		return	fHeight;
	}
}

FLOAT	AgcmMap::HP_GetHeightGeometryOnly	( FLOAT fX , FLOAT fZ , FLOAT fY )
{
	int x , z;
	FLOAT	fSegmentX , fSegmentZ;
	// fX , fZ의 높이를 계산한다..

	ApWorldSector * pSector;
	pSector = m_pcsApmMap->GetSector( fX , fZ );

	if( pSector )
	{
		pSector->D_GetSegment( SECTOR_HIGHDETAIL , fX , fZ , &x , &z );
		// 그리드 관련 정보를 얻음..
		pSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z , &fSegmentX , &fSegmentZ );

		if( fX == fSegmentX && fZ == fSegmentZ )
		{
			// 격자면 격자용 펑션으로 날림..
			return HP_GetHeightGeometryOnly( pSector , x , z , fY );
		}

		// 격자가 아닌경우는..
		// 그냥 콜리젼을 때림..

		return GetHeight_Lowlevel_HeightOnly( fX , fZ );
	}
	else
	{
		return 1.0f;
	}
}


FLOAT	AgcmMap::GetHeight		( FLOAT x , FLOAT z , FLOAT y )
{
	ASSERT( NULL != m_pcsApmMap );
	//@{ 2006/12/08 burumal
	if ( NULL == m_pcsApmMap )
		return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
	//@}

	ApWorldSector * pWorldSector;
	pWorldSector = m_pcsApmMap->GetSector( x , z );
	if( pWorldSector )
	{
		#ifndef USE_MFC
		if( !IsLoadedDetail( pWorldSector , SECTOR_LOWDETAIL ) )
		{
			// 맵데이타가 아직 로드 돼어있지 않은상태라면..
			// 맵 로딩이 끝난후 높이 업데이트를 다시해준다.
			// 플래그만 설정해놓고
			// AgcmObject 에서 맵로딩 끝난시점에서
			// 다시 한번 get height를 해준다.
			pWorldSector->SetFlag( pWorldSector->GetFlag() | ApWorldSector::OP_HEIGHTUPDATEAFTERLOAD );
			return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
		}
		#endif

		int segx, segz;
		BOOL bEdgeTurn	= FALSE;
		BOOL bNoLayer	= FALSE;
		ApDetailSegment		* pDetailSegment	;
		ApCompactSegment	* pCompactSegment	;

		if( NULL == ( pDetailSegment = pWorldSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z , & segx , &segz ) ) )
		{
			if( pCompactSegment = pWorldSector->C_GetSegment( segx , segz ) )
			{
				bEdgeTurn	= pCompactSegment->stTileInfo.GetEdgeTurn	()	;
				bNoLayer	= pCompactSegment->stTileInfo.IsNoLayer		()	;
			}
			else
			{
				bEdgeTurn	= TRUE	;
				bNoLayer	= FALSE	;
			}
		}
		else
		{
			bEdgeTurn	= pDetailSegment->stTileInfo.GetEdgeTurn	()	;
			bNoLayer	= pDetailSegment->stTileInfo.IsNoLayer		()	;
		}

		if( bNoLayer )
		{
			// 레이어 없는 옵션이 있는경우 무조건 위로 튀겨 버린다.
			y = SECTOR_MAX_HEIGHT;
		}

		float	fsegstartx , fsegstartz;
		float	fstepsize;
		float	height;

		pWorldSector->D_GetSegment( SECTOR_HIGHDETAIL , segx , segz , & fsegstartx , &fsegstartz );
		fstepsize = pWorldSector->D_GetStepSize( SECTOR_HIGHDETAIL );
		
		// 마고자 (2004-01-05 오전 10:35:40) : 라이더블 있는지 확인을 먼저함.
		INT32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
		INT32	nObjectCount = pWorldSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT , segx , segz , aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT );

		if( nObjectCount ) return HP_GetHeight( x , z , y );

		if( x == fsegstartx && z == fsegstartz )
		{
			return HP_GetHeight( pWorldSector , segx , segz , y );
		}
		else if( x == fsegstartx )
		{
			// y 축만고려
			float startheight	= HP_GetHeight( pWorldSector , segx , segz , y );
			float endheight		= GetHeight( fsegstartx + fstepsize * ( 0 )	, fsegstartz + fstepsize * ( 1 ) , y );

			height = startheight + ( z - fsegstartz ) / fstepsize * ( endheight - startheight );
			return height;
		}
		else if ( z == fsegstartz )
		{
			// x 축만 고려
			float startheight	= HP_GetHeight( pWorldSector , segx , segz , y );
			float endheight		= GetHeight( fsegstartx + fstepsize * ( 1 )	, fsegstartz + fstepsize * ( 0 ) , y );

			height = startheight + ( x - fsegstartx ) / fstepsize * ( endheight - startheight );
			return height;
		}
		else
		{
			// -_-;
			// return 	GetHeight_Lowlevel( x , z , y );


			float h11 , h21 , h12 , h22;
			h11 = HP_GetHeight( pWorldSector , segx , segz , y );
			h21 = GetHeight( fsegstartx + fstepsize * ( 1 )	, fsegstartz + fstepsize * ( 0 ) , y );
			h12 = GetHeight( fsegstartx + fstepsize * ( 0 )	, fsegstartz + fstepsize * ( 1 ) , y );
			h22 = GetHeight( fsegstartx + fstepsize * ( 1 )	, fsegstartz + fstepsize * ( 1 ) , y );

			// 기울기..
			x -= fsegstartx;
			z -= fsegstartz;
			
			if( bEdgeTurn )
			{
				float kiulki = z / x;

				if( kiulki < 1.0f )
				{
					// 오른쪽 위것.

					float daechoongy = kiulki * fstepsize;
					float daechoongheight;

					daechoongheight = h21 + ( daechoongy ) * ( h22 - h21 ) / fstepsize;

					height = h11 + ( x ) * ( daechoongheight - h11 ) / fstepsize;

					return height;
				}
				else
				{
					// 왼쪽 아래 삼각형.
					float daechoongx = fstepsize / kiulki;
					float daechoongheight;

					daechoongheight = h12 + ( daechoongx ) * ( h22 - h12 ) / fstepsize;

					height = h11 + ( z ) * ( daechoongheight - h11 ) / fstepsize;

					return height;
				}
			}
			else
			{
				float kiulki = - z / ( fstepsize - x );

				if( kiulki > -1.0f )
				{
					// 왼쪽 아래것..

					float fZp = - kiulki * fstepsize;
					float fHy ;

					fHy = h11 + ( fZp ) * ( h12 - h11 ) / fstepsize;

					height = fHy + x * ( h21 - fHy  ) / fstepsize;

					return height;
				}
				else
				{
					float fXp = ( kiulki + 1.0f ) / kiulki * fstepsize;
					float fHx;

					fHx = h12 + ( fXp ) * ( h22 - h12 ) / fstepsize;

					height = h21 + z * ( fHx - h21 ) / fstepsize;

					return height;
				}

			}
			////
		}
	}
	else
	{
		return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
	}
}
FLOAT		AgcmMap::GetHeight_Lowlevel_HeightOnly( FLOAT x , FLOAT z , BOOL * pValidCash )
{
	ASSERT( NULL != m_pcsApmMap );
	//@{ 2006/12/08 burumal
	if ( NULL == m_pcsApmMap )
		return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
	//@}

	ApWorldSector * pWorldSector;
	pWorldSector = m_pcsApmMap->GetSector( x , z );
	if( pWorldSector )
	{
		ASSERT( m_pcsApmMap->IsLoadingDetailData() || m_pcsApmMap->IsLoadingCompactData() );
		if( m_pcsApmMap->IsLoadingDetailData() )
		{
			FLOAT			fHeight		= pWorldSector->D_GetHeight( x , z );
			return fHeight;

		}
		else if( m_pcsApmMap->IsLoadingCompactData() )
		{
			// 아토믹을 상대로 콜리전 하여 데이타를 얻는다..

			RpDWSector *	pDWSector	;
			FLOAT			fHeight		= INVALID_HEIGHT;
			pDWSector	= RpDWSectorGetDetail( pWorldSector , SECTOR_HIGHDETAIL  );

			// ASSERT( NULL != pDWSector );
			if( NULL == pDWSector || NULL == pDWSector->atomic )
			{
				if( pValidCash ) * pValidCash = FALSE;
				// 디테일이 없으면 러프라도..
				pDWSector	= RpDWSectorGetDetail( pWorldSector , SECTOR_LOWDETAIL  );
				if( NULL == pDWSector || NULL == pDWSector->atomic )
				{
					return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
				}

				// 러프라도 있으면 그 데이타를 쓴다.
			}
			else
			{
				// 디테일 있으면 캐시해도 된다.
				if( pValidCash ) * pValidCash = TRUE;
			}
			

			RpIntersection	stIntersection		;

			stIntersection.type = rpINTERSECTLINE;

			stIntersection.t.line.start.x	= x			;
			stIntersection.t.line.start.z	= z			;
			stIntersection.t.line.start.y	= SECTOR_MAX_HEIGHT	;

			stIntersection.t.line.end.x		= x			;
			stIntersection.t.line.end.z		= z			;
			stIntersection.t.line.end.y		= SECTOR_MIN_HEIGHT	;

			if( RpAtomicGetGeometry( pDWSector->atomic ) &&
				RpGeometryGetTriangles(RpAtomicGetGeometry( pDWSector->atomic )))
			{
				LockFrame();

				RpAtomicForAllIntersections( pDWSector->atomic , &stIntersection, CollisionAtomicSectorCallback , ( void * ) & fHeight );

				UnlockFrame();
			}

			return fHeight;

		}
		else
			return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
	}
	else
	{
		return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
	}
}

// 마고자 (2005-08-31 오후 2:28:36) : 
// 라이더블 디버그용 글로벌 변수.
INT32	g_nRidableCount		= 0		;
BOOL	g_bRidableChecked	= FALSE	;

FLOAT	AgcmMap::GetHeight_Lowlevel		( FLOAT x , FLOAT z , FLOAT y , BOOL * pValidCash )
{
	ASSERT( NULL != m_pcsApmMap );
	//@{ 2006/12/08 burumal
	if ( NULL == m_pcsApmMap )
		return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
	//@}

	ApWorldSector * pWorldSector;
	pWorldSector = m_pcsApmMap->GetSector( x , z );
	if( pWorldSector )
	{
		ASSERT( m_pcsApmMap->IsLoadingDetailData() || m_pcsApmMap->IsLoadingCompactData() );
		if( m_pcsApmMap->IsLoadingDetailData() )
		{
			FLOAT			fHeight		= pWorldSector->D_GetHeight( x , z );

			_FindHeightStruct	stHeight;
			stHeight.fHeight			= fHeight	;
			stHeight.pcsAgcmMap			= this		;
			stHeight.fCharacterHeight	= y;	// 케릭터 위치 삽입..

			{
				AuList< _FindClumpStruct >	listClumpSorted;

				// 현재 커서 위치에 있는 클럼프를 구해낸다.
				BOOL			bMultiClump		= FALSE;

				AuList< _FindClumpStruct >	listClump;

				RpIntersection	stIntersection		;

				stIntersection.type = rpINTERSECTLINE;

				stIntersection.t.line.start.x	= x			;
				stIntersection.t.line.start.z	= z			;
				stIntersection.t.line.start.y	= SECTOR_MAX_HEIGHT	;

				stIntersection.t.line.end.x		= x			;
				stIntersection.t.line.end.z		= z			;
				stIntersection.t.line.end.y		= SECTOR_MIN_HEIGHT	;

				if(!m_pcsApmOcTree || !m_pcsApmOcTree->m_bOcTreeEnable)
				{
					if(m_bUseCullMode)
					{
						SectorForAllAtomicsIntersection(
						pWorldSector,
						&stIntersection,
						AgcmMap::_GetHeightCallback	,
						( void * ) &stHeight			);
					}
					else
					{
						m_pcsAgcmRender->RenderWorldForAllIntersections( 
						&stIntersection				,
						AgcmMap::_GetHeightCallback	,
						( void * ) &stHeight			);
					}
				}
				else
				{
					m_pcsAgcmOcTree->OneOcTreeForAllAtomicsIntersection(
						pWorldSector->GetArrayIndexX(),
						pWorldSector->GetArrayIndexZ(),
						&stIntersection,
						AgcmMap::_GetHeightCallback ,
						( void * ) &stHeight );
				}
			}
			
			// 마고자 (2004-10-16 오후 2:13:26) : 
			// 디테일 섹터가 로딩되어있으면,
			// 여긴 맵툴이므로 언제나 Valid.

			if( pValidCash ) *pValidCash = TRUE;

			INT32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
			INT32	nSegmentX , nSegmentZ;
			pWorldSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z , & nSegmentX , & nSegmentZ );

			INT32	nObjectCount = pWorldSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT , nSegmentX , nSegmentZ , aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT);

			if( pValidCash && nObjectCount )
			{
				stCollisionAtomicInfo	stCollInfo;
				stCollInfo.fX				= x					;
				stCollInfo.fZ				= z					;
				stCollInfo.fHeight			= stHeight.fHeight	;
				stCollInfo.fCharacterHeight	= y					;
				stCollInfo.nObject			= nObjectCount		;
				stCollInfo.aObject			= aObjectList		;

				if( EnumCallback( AGCM_CB_ID_ONGETHEIGHT , ( void * ) & stCollInfo , NULL ) )
				{
					stHeight.fHeight	= stCollInfo.fHeight;
					* pValidCash		= FALSE				;	// 캐싱을하지않음.
				}
			}

			return stHeight.fHeight;
		}
		else if( m_pcsApmMap->IsLoadingCompactData() )
		{
			// 아토믹을 상대로 콜리전 하여 데이타를 얻는다..
			_FindHeightStruct	stHeight;
			stHeight.fHeight			= INVALID_HEIGHT;
			stHeight.pcsAgcmMap			= this;
			stHeight.fCharacterHeight	= y;

			RpIntersection	stIntersection		;

			stIntersection.type = rpINTERSECTLINE;

			stIntersection.t.line.start.x	= x			;
			stIntersection.t.line.start.z	= z			;
			stIntersection.t.line.start.y	= SECTOR_MAX_HEIGHT	;

			stIntersection.t.line.end.x		= x			;
			stIntersection.t.line.end.z		= z			;
			stIntersection.t.line.end.y		= SECTOR_MIN_HEIGHT	;

			if(!m_pcsApmOcTree || !m_pcsApmOcTree->m_bOcTreeEnable)
			{
				if(m_bUseCullMode)
				{
					SectorForAllAtomicsIntersection(
					pWorldSector,
					&stIntersection,
					AgcmMap::_GetHeightClientCallback	,
					( void * ) &stHeight			);

					// 마고자 (2003-12-03 오후 2:08:44) : 락추가.
				}
				else
				{
					m_pcsAgcmRender->RenderWorldForAllIntersections( 
					&stIntersection				,
					AgcmMap::_GetHeightClientCallback	,
					( void * ) &stHeight			);
				}
			}
			else
			{
				m_pcsAgcmOcTree->OneOcTreeForAllAtomicsIntersection(
				pWorldSector->GetArrayIndexX(),
				pWorldSector->GetArrayIndexZ(),
				&stIntersection,
				AgcmMap::_GetHeightClientCallback	,
				( void * ) &stHeight			);
			}

			if( pValidCash )
			{
				// 마고자 (2004-10-16 오후 2:17:37) : 
				// 해당 섹터의 현재 디테일이 High Detail 일 경우만
				// 캐시가 벨리드이다.

				ApWorldSector * pSector = m_pcsApmMap->GetSector( x , z );

				INT32	nCurrentDetail	= RpDWSectorGetCurrentDetail( pSector );

				if( nCurrentDetail == SECTOR_HIGHDETAIL )
				{
					*pValidCash = TRUE;
				}
				else
				{
					*pValidCash = FALSE;
				}
			}

			// 인덱스 큐를 이용함..
			INT32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
			INT32	nSegmentX , nSegmentZ;
			pWorldSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z , & nSegmentX , & nSegmentZ );

			INT32	nObjectCount = pWorldSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT , nSegmentX , nSegmentZ , aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT );

			g_nRidableCount = nObjectCount;
			g_bRidableChecked = FALSE;

			if( nObjectCount )
			{
				stCollisionAtomicInfo	stCollInfo;
				stCollInfo.fX				= x						;
				stCollInfo.fZ				= z						;
				stCollInfo.fHeight			= stHeight.fHeight		;
				stCollInfo.fCharacterHeight	= y						;
				stCollInfo.nObject			= nObjectCount			;
				stCollInfo.aObject			= aObjectList			;

				// 마고자 (2004-02-18 오전 5:25:50) : 이부분 왜 제대로 안돼는지 모르겠음..
				// 일단 무조건 케싱 하지 않게 바꿈,.

				if( pValidCash ) * pValidCash		= FALSE	;	// 캐싱을하지않음.

				if( EnumCallback( AGCM_CB_ID_ONGETHEIGHT , ( void * ) & stCollInfo , NULL ) )
				{
					stHeight.fHeight	= stCollInfo.fHeight;
					g_bRidableChecked	= TRUE;
				}
			}

			if( !stHeight.bCollisionAvailable && nObjectCount && pValidCash )
			{
				// 콜리젼 데이타가 없고 , 콜리젼 오브젝트가 있는 경우에는.
				// 캐싱하지 않음..
				
				if( pValidCash ) * pValidCash		= FALSE	;	// 캐싱을하지않음.
			}

			return stHeight.fHeight;

		}
		else
			return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
	}
	else
	{
		return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
	}
}

FLOAT	AgcmMap::GetHeight_Lowlevel_Collision	( FLOAT x , FLOAT z , FLOAT y , BOOL * pbRiddable )
{
	ASSERT( NULL != m_pcsApmMap );
	//@{ 2006/12/08 burumal
	if ( NULL == m_pcsApmMap )
		return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
	//@}

	ApWorldSector * pWorldSector;
	pWorldSector = m_pcsApmMap->GetSector( x , z );
	if( pWorldSector )
	{
		// 아토믹을 상대로 콜리전 하여 데이타를 얻는다..
		_FindHeightStruct	stHeight;
		stHeight.fHeight			= INVALID_HEIGHT;
		stHeight.pcsAgcmMap			= this;
		stHeight.fCharacterHeight	= y;

		RpIntersection	stIntersection		;

		stIntersection.type = rpINTERSECTLINE;

		stIntersection.t.line.start.x	= x			;
		stIntersection.t.line.start.z	= z			;
		stIntersection.t.line.start.y	= y			;	//10000.0f	;

		stIntersection.t.line.end.x		= x			;
		stIntersection.t.line.end.z		= z			;
		stIntersection.t.line.end.y		= INVALID_HEIGHT	;

		if(!m_pcsApmOcTree || !m_pcsApmOcTree->m_bOcTreeEnable)
		{
			if(m_bUseCullMode)
			{
				SectorForAllAtomicsIntersection(
				pWorldSector,
				&stIntersection,
				AgcmMap::_GetHeightClientCallback	,
				( void * ) &stHeight			);
			}
			else
			{
				m_pcsAgcmRender->RenderWorldForAllIntersections( 
				&stIntersection				,
				AgcmMap::_GetHeightClientCallback	,
				( void * ) &stHeight			);
			}
		}
		else
		{
			m_pcsAgcmOcTree->OneOcTreeForAllAtomicsIntersection(
			pWorldSector->GetArrayIndexX(),
			pWorldSector->GetArrayIndexZ(),
			&stIntersection,
			AgcmMap::_GetHeightClientCallback	,
			( void * ) &stHeight			);
		}

		if( pbRiddable )
		{
			if( stHeight.fHeight == INVALID_HEIGHT )
				* pbRiddable = TRUE;	// 캐싱을하지않음.
			else
				* pbRiddable = stHeight.bExistRidableObject;
		}

		return stHeight.fHeight;
	}
	else
	{
		return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
	}
}

ApWorldSector *		AgcmMap::GetSector		( RpAtomic		* pAtomic		)	// 아토믹으로 부터.. 섹터 정보 얻어내기.
{
	ASSERT( IsInitialized()		);
	ASSERT( NULL != m_pcsApmMap	);
	//@{ 2006/12/08 burumal
	if ( NULL == m_pcsApmMap )
		return NULL;
	//@}

	INT32	nIndex = 0;
	if( IsGeometry( pAtomic , & nIndex ) )
	{
		// 섹터 차았다..
		INT32 x , z ;

		x = GetArrayIndexXDWORD( nIndex );
		z = GetArrayIndexZDWORD( nIndex );

		return m_pcsApmMap->GetSector( x , z );
	}
	else return NULL;
}

BOOL				AgcmMap::IsGeometry		( RpAtomic		* pAtomic	, INT32 * pIndex )
{
	ASSERT( NULL != pAtomic );
	if( NULL == pAtomic ) return FALSE;

	INT32		index		;

	if( NULL == pIndex ) pIndex = &index;

	switch( AcuObject::GetAtomicType( pAtomic , pIndex ) )
	{
	case ACUOBJECT_TYPE_WORLDSECTOR		:
		return TRUE		;
	default:
		return FALSE	;
	}
}

BOOL			AgcmMap::LoadDWSector		( ApWorldSector * pSector )
{
	if( pSector->GetFlag() & ApWorldSector::OP_DONOTLOADSECTOR )
	{
		// 마고자 (2005-07-19 오후 5:15:36) : 
		// 읽지 않음 설정한것 튕겨냄.
		return FALSE;
	}
	else
	{
		// 섹터 디테일 로디잉..
		if( !LoadDWSector( pSector , SECTOR_LOWDETAIL	) )
			return FALSE;
		if( !LoadDWSector( pSector , SECTOR_HIGHDETAIL	) )
			return FALSE;
		
		return TRUE;
	}
}

RwTexture *		CBMoveTexture(RwTexture *pstTexture, PVOID pvData)
{
	RwTexture **	ppstTexture = (RwTexture **) pvData;

	*ppstTexture = pstTexture;

	return NULL;
}

BOOL			AgcmMap::LoadDWSector		( ApWorldSector * pSector , int nDetail )
{
	ASSERT( NULL != pSector											);
	ASSERT( SECTOR_EMPTY <= nDetail && nDetail < SECTOR_DETAILDEPTH	);

	RpDWSector	*pDWSector	;

	// 만약에 이미 Load 되어있으면, 그냥 return
	pDWSector	=	RpDWSectorGetDetail	( pSector	 , nDetail	);
	if( pDWSector && pDWSector->atomic )
	{
		return TRUE;
	}

	if( ApWorldSector::OP_DONOTLOADSECTOR & pSector->GetFlag() )
	{
		// 로딩에서 제외함.
		return FALSE;
	}

	char	str			[ 256	];
	char	fullpath	[ 1024	];
	char	strDir		[ 1024	];

	char	strPackedFilename[ 256 ];

	// DWSector 처리..

	INT32	nDivisionIndex = GetDivisionIndex( SectorIndexToArrayIndexX( pSector->GetIndexX() ) , SectorIndexToArrayIndexZ( pSector->GetIndexZ() ) );

	switch( nDetail )
	{
	case SECTOR_LOWDETAIL	:
		wsprintf( str , ALEF_WORLD_ROUGH_SECTOR_FILE_NAME_FORMAT , 	nDivisionIndex );
		wsprintf( strPackedFilename , SECTOR_DFF_FILE_FORMAT_ROUGH , 
			pSector->GetArrayIndexX() , pSector->GetArrayIndexZ() );
		break;
	case SECTOR_HIGHDETAIL	:
		wsprintf( str , ALEF_WORLD_DETAIL_SECTOR_FILE_NAME_FORMAT , nDivisionIndex );
		wsprintf( strPackedFilename , SECTOR_DFF_FILE_FORMAT_DETAIL , 
			pSector->GetArrayIndexX() , pSector->GetArrayIndexZ() );
		break;
	default:
		MD_SetErrorMessage( "AgcmMap::LoadDWSector 생기면 안돼는 오륭-_-;" );
		ASSERT( !"이러면 안돼요." );
		return FALSE;
	}

	wsprintf( strDir , "%s\\World" , m_strToolImagePath );
	// 디렉토리 변경..
	//SetCurrentDirectory( strDir );
	
	wsprintf( fullpath , "World\\%s" , str );

	// DWSector 저장됀 파일 읽어 들인후에..
	// 언하는 섹터 데이타만 뽑아서 로딩하는 루틴..
	// 전에 이야기했던 파인드 청크 연타하는 방식..

	RwStream	*stream		;
	CMagUnpackManager *	pUnpack = NULL;

	pUnpack = m_csMapLoader.GetUnpackManager( nDetail , nDivisionIndex , fullpath );

	if( NULL == pUnpack )
	{
		//@{ kday 20050705
		//잠시주석
		// TODO : AgcmCharacter::LoadTemplateClump() 잡고 주석 풀것
		//MD_SetErrorMessage( "AgcmMap::LoadDWSector 지형 파일을 찾을수 없어요" );
		//@} kday
		return FALSE;
	}

	BYTE	* pPtr = NULL;
	UINT	nSize;

	if( ( nSize = pUnpack->GetFileBinary( strPackedFilename , pPtr ) ) == 0  )
	{
		MD_SetErrorMessage( "AgcmMap::LoadDWSector 압축된 파일을 찾을 수 없습니다" );
		return FALSE;
	}

	RwTexture *	pTexture = NULL;
	RwMemory	memory;
	memory.start	= pPtr	;
	memory.length	= nSize	;

	stream	= RwStreamOpen		( rwSTREAMMEMORY , rwSTREAMREAD , &memory );

	// 잘못됐으면 FALSE return한다.
	if( !stream )
	{
		MD_SetErrorMessage( "AgcmMap::LoadDWSector 스트림 오픈실패" );
		return FALSE;
	}

	// 크리에이트 해줘야하나?...

	// Texture Dictionary Setting해준다.
	// 텍스쳐 딕셔너리 정보 저장.
	if( SECTOR_LOWDETAIL == nDetail )
	{
		// 텍스쳐 리딩.
		pTexture = LoadRoughTexture( stream );
	}
	else
	{
		if (m_pcsAgcmResourceLoader)
		{
			m_pcsAgcmResourceLoader->SetDefaultTexDict();
			m_pcsAgcmResourceLoader->SetTexturePath(m_szTexturePath);
		}
	}

	if( RpDWSectorStreamRead( pDWSector , stream ) )
	{
		if( pTexture )
		{
			// 마고자 2004/04/07
			// 러프맵의 경우 , 텍스쳐 딕셔너리에서 먼저 텍스쳐를 로딩해
			// 두기때문에 딕셔너리 올라갈때 레퍼런스 카운트는 1이돼고
			// 이 섹터 로딩할때 2로 증가하기때문에 , 자동 디스트로이되게
			// 하기 위해서 강제로 한번 디스트로이를 때린다.
			RwTextureDestroy( pTexture );
			pTexture = NULL;
		}

		// 마고자 (2003-12-05 오후 4:13:03) : 콜리전 아토믹 읽어들인다.
		RpAtomic * pCollisionAtomic = NULL;
		// 마고자 (2005-03-05 ) : 콜리전 아토믹을 사용하지 않으므로 리드를 시도하지도 않게함.
		/*
		if (RwStreamFindChunk(stream, rwID_ATOMIC, NULL, NULL))
		{
			pCollisionAtomic = RpAtomicStreamRead( stream );

			if( !pCollisionAtomic )
			{
				#ifdef _DEBUG
				RwError error;
				RwErrorGet( & error );
				#endif // _DEBUG

				TRACE( "이상한 에러 발생!" );
			}
			else
			{
				// 성공적으로 아토믹읽음..
				// 아토믹 리드 상태에서는 프레임이 없기때문에
				// 하나 넣어준다.
				LockFrame();

				RpAtomicSetFrame( pCollisionAtomic , RwFrameCreate() );

				UnlockFrame();
			}
		}
		*/

		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->ResetDefaultTexDict();
		// success , do no op

		// Atomic Type 설정한다.
		if( pDWSector->atomic )
		{
			//@{ Jaewon 20050615
			// Fix bugs related to ambient occlusion in dungeon.
			INT32	nType;

			if( SECTOR_LOWDETAIL == nDetail )
			{
				nType = ACUOBJECT_TYPE_WORLDSECTOR | ACUOBJECT_TYPE_SECTOR_ROUGHMAP;
			}
			else
			switch( pDWSector->atomic->geometry->numTexCoordSets )
			{
			default:
			case 6:
				nType = ACUOBJECT_TYPE_WORLDSECTOR;
				break;
			case 2:
				{
					nType = ACUOBJECT_TYPE_WORLDSECTOR | ACUOBJECT_TYPE_NO_MTEXTURE;
					// Register the shader constant upload callback for the ambient occlusion map.
					RpGeometry *pGeometry = RpAtomicGetGeometry(pDWSector->atomic);
					if(pGeometry)
					{
						ASSERT(NULL == RpSkinGeometryGetSkin(pGeometry));
						ASSERT(RpAtomicFxIsEnabled(pDWSector->atomic));
						for(int j=0; j<RpGeometryGetNumMaterials(pGeometry); ++j)
						{	
							ASSERT(RtAmbOcclMapMaterialGetFlags(RpGeometryGetMaterial(pGeometry, j)) & rtAMBOCCLMAPMATERIALAMBOCCLMAP);
							//@{ Jaewon 20050818
							// Ok, it's my bad... If RpMaterialD3DFxGetEffect() returns FALSE, this should not be called.
							if(RpMaterialD3DFxGetEffect(RpGeometryGetMaterial(pGeometry, j)))
								RpMaterialD3DFxSetConstantUploadCallBack(RpGeometryGetMaterial(pGeometry, j), fxConstUploadCB);
							//@} Jaewon
						}
					}
				}
				break;
			}

			AcuObject::SetAtomicType(
				pDWSector->atomic				,
				nType							,
				( INT32 ) pSector				,
				NULL							,
				NULL							,
				NULL							,
				( void * ) pCollisionAtomic		); // 해당 섹터의 포인터를 삽입해둔다
			//@} Jaewon
		}
	}
	else
	{
		// 에러 리턴..
		MD_SetErrorMessage( "AgcmMap::LoadDWSector 섹터 DW 데이타 로드할때 에러 발생 ( %d,%d 섹터 )\n" , pSector->GetIndexX() , pSector->GetIndexZ() );

		return FALSE;
	}

	VERIFY( RwStreamClose( stream , NULL	) );

	return TRUE;
}

BOOL			AgcmMap::ReleaseDWSector	( ApWorldSector * pSector )
{
	// 우어..
	if( RpDWSectorDestroyDetail( pSector ) )
	{
		AcMaterialList *	pMaterialList	= GetMaterialList( pSector );

		for( int i = 0 ; i < SECTOR_DETAILDEPTH ; ++i  )
		{
			pMaterialList[ i ].RemoveAll();
		}
		return TRUE	;
	}
	else
		return FALSE;
}

//ApWorldSector* AgcmMap::LoadSector( ApWorldSector * pSector )
//{
//	ASSERT( !"이거 쓰면 안돼용" );
//	return pSector;
//}

ApWorldSector *	AgcmMap::LoadSector		( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z )
{
	ASSERT( NULL != m_pcsApmMap );
	if ( !m_pcsApmMap ) return NULL;

	// 섹터가 존재하는지 확인
	ApWorldSector * pSector = m_pcsApmMap->GetSector( sectorindex_x , sectorindex_y , sectorindex_z );

	// 섹터가 없으면 섹터 셋업함..
	if( NULL == pSector )
	{
		pSector = m_pcsApmMap->SetupSector( sectorindex_x , sectorindex_y , sectorindex_z );
		
		// ASSERT( NULL != pSector );
		if( !pSector ) return NULL;

		ApAutoWriterLock	csLock( pSector->m_RWLock );

		// 지형데이타 로딩함..
		/*BOOL bRet = */m_pcsApmMap->LoadSector( pSector );
	}

	return pSector;
}

BOOL	AgcmMap::ReleaseSector	( ApWorldSector * pSector											)
{
	ASSERT( NULL != pSector );
	if( NULL == pSector ) return FALSE;

	INT32 arrayx = pSector->GetArrayIndexX();
	INT32 arrayz = pSector->GetArrayIndexZ();

	if( m_pcsApmMap->IsAutoLoadData() )
	{
		// 섹터 데이타 제거..
		// 메모리에서 제거함..
		m_csMapLoader.RemoveQueue( pSector );
		m_pcsApmMap->DeleteSector( pSector );
	}

	return TRUE;
}

BOOL	AgcmMap::ReleaseSector	( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	)
{
	ASSERT( NULL != m_pcsApmMap );

	ApWorldSector * pWorldSector;

	pWorldSector = m_pcsApmMap->GetSector( sectorindex_x , sectorindex_y , sectorindex_z );

	ReleaseSector( pWorldSector );
	
	return TRUE;
}

void		AgcmMap::CallBackMapDataLoad		( ApWorldSector * pSector )
{
	ASSERT( NULL != AGCMMAP_THIS );

	// 섹터로드 콜백을 호출함..
	AGCMMAP_THIS->OnLoadSector		( pSector );
}

void		AgcmMap::CallBackMapDataDestroy		( ApWorldSector * pSector )
{
	ASSERT( NULL != AGCMMAP_THIS );
	
	// 섹터디스트로이 콜백을 호출함..

	if( AGCMMAP_THIS->IsLoadedDetail( pSector , SECTOR_HIGHDETAIL ) )
	{
		AGCMMAP_THIS->OnDestroyDetail	( pSector );
	}
	if( AGCMMAP_THIS->IsLoadedDetail( pSector , SECTOR_LOWDETAIL ) )
	{
		AGCMMAP_THIS->OnDestroyRough	( pSector );	
	}

	AGCMMAP_THIS->OnDestorySector	( pSector );	

	if( AGCMMAP_THIS->m_pcsApmMap->IsAutoLoadData() )
	{
		
	}
	else
	{
		// 폴리건 정보 제거..
		VERIFY( AGCMMAP_THIS->ReleaseDWSector	( pSector ) );
	}
}


BOOL		AgcmMap::BspExport		( BOOL bDetail , BOOL bRough , BOOL bCompact , BOOL bServer ,
									char * pDestinationDirectory ,
									ProgressCallback pfCallback , void * pData )
{
	// BSP Export!!!!!!!!!!!!!!
	ASSERT( NULL != m_pcsApmMap );
	ASSERT( IsRangeApplied() );
	if( FALSE == IsRangeApplied() )
	{
		return FALSE;
	}

	if( !bDetail && !bRough && !bCompact && !bServer) return TRUE; // do nothing

	UINT		uStartTime = GetTickCount();

	if( GetLoadRangeX1() % MAP_DEFAULT_DEPTH != 0 ||
		GetLoadRangeZ1() % MAP_DEFAULT_DEPTH != 0 )
	{
		//DisplayMessage( AEM_ERROR , "디테일 데이타를 익스포트할땐 지형을 로딩할때 10 섹터 기준으로 끊어주어야합니다." );
		return FALSE;
	}

	TRACE( "디테일 섹터를 익스포트 합니다! -_-+\n" );


	// 디테일 섹터 익스포트 루틴..
	if( bDetail || bRough )
		RecalcBoundingBox();

//	CProgressDlg	progressdlg;
//	progressdlg.StartProgress( "디테일 BSP 데이타 익스포트" , ( ALEF_LOAD_RANGE_X2 - ALEF_LOAD_RANGE_X1 ) * ( ALEF_LOAD_RANGE_Y2 - ALEF_LOAD_RANGE_Y1 ) , this );
//	progressdlg.SetProgress( ( ALEF_LOAD_RANGE_X2 - ALEF_LOAD_RANGE_X1 ) * ( ALEF_LOAD_RANGE_Y2 - ALEF_LOAD_RANGE_Y1 ) );
// Detail Sector File

	char	strDetailSector	[ 256 ]	;	ZeroMemory( strDetailSector , 256 );
	char	strRoughSector	[ 256 ]	;	ZeroMemory( strRoughSector , 256 );
	char	strCompactSector[ 256 ]	;	ZeroMemory( strCompactSector , 256 );
	char	strServerData	[ 256 ] ;	ZeroMemory( strServerData , 256 );

	char	strDetailSectorUnit	[ 256 ]	;	ZeroMemory( strDetailSectorUnit , 256 );
	char	strRoughSectorUnit	[ 256 ]	;	ZeroMemory( strRoughSectorUnit , 256 );

	char	filename	[ 256 ];			ZeroMemory( filename , 256 );

	int		x2	=	0 , z2	=	0;

	RwStream	*pDetailStream		=	NULL;
	RwStream	*pRoughStream		=	NULL;

	RwChar		*pathDetail			=	NULL;
	RwChar		*pathRough			=	NULL;

	RpDWSector	*pDWSector			=	NULL;

	ApWorldSector	*	pWorldSector	=	NULL;

	// 디렉토리 변경..
	char	strDir[ 1024 ];
	ZeroMemory( strDir , 1024 );

	if( pDestinationDirectory )
	{
		// 지정된 디렉토리에..
		// pDestinationDirectory 는 "abc" 까지만.. 끝에 \\ 안붙음.
		wsprintf( strDir , "%s\\World" , pDestinationDirectory );
		CreateDirectory( pDestinationDirectory	, NULL );
		wsprintf( strDir , "%s\\World\\server" , pDestinationDirectory );
		CreateDirectory( strDir					, NULL );
	}

	wsprintf( strDir , "%s\\World\\server" , m_strToolImagePath );
	CreateDirectory( strDir					, NULL );
	wsprintf( strDir , "%s\\World" , m_strToolImagePath );
	SetCurrentDirectory( strDir );

//	INT32			userData	;
//	AcuObjectType	atomicType	;
	AuList< CMagPackHeaderInfo * >	listCompact		;
	AuList< CMagPackHeaderInfo * >	listDetail		;
	AuList< CMagPackHeaderInfo * >	listRough		;
	AuList< CMagPackHeaderInfo * >	listServer		;
	CMagPackHeaderInfo	*			pCompactInfo		=	NULL;
	RpAtomic			*			pCollisionAtomic	=	NULL;
	
	// 템포러리 디렉토리 생성..
	// 
	CreateDirectory( SECTOR_TEMP_DIRECTORY , NULL );

	///////////////////////////////////////////////////////////////////////
	static	char	strMessage[] = "Bsp Export 하고있심";
	int		nTarget		=	( GetLoadRangeX2() - GetLoadRangeX1() + 1 )		*
							( GetLoadRangeZ2() - GetLoadRangeZ1() + 1 )		;
	int		nCurrent	= 1													;
	//////////////////////////////////////////////////////////////////////

	int lxend = GetLoadRangeX2();
	int lzend = GetLoadRangeZ2();
	for( int x = GetLoadRangeX1() ; x < lxend ; x += MAP_DEFAULT_DEPTH )
	{
		for( int z = GetLoadRangeZ1() ; z < lzend ; z += MAP_DEFAULT_DEPTH )
		{
			RpGeometry * pSortedGeometry	;
			RpGeometry * pOriginalGeometry	;

			for( z2 = z ; z2 < z + MAP_DEFAULT_DEPTH ; ++z2  )
			{
				for( x2 = x ; x2 < x + MAP_DEFAULT_DEPTH ; ++x2 )
				{
					if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );

					// 아싸..
					// 월드섹터에서 디테일 섹터 데이타를 뽑아냄.
					pWorldSector	= m_pcsApmMap->GetSectorByArray( x2 , 0 , z2 );
					// 하이디테일..
					if( bDetail )
					{
						TRACE( "Detail 스트림 스타트.\n" );
						wsprintf( strDetailSectorUnit , SECTOR_TEMP_DIRECTORY "\\" SECTOR_DFF_FILE_FORMAT_DETAIL , 
							pWorldSector->GetArrayIndexX() , pWorldSector->GetArrayIndexZ() );

						// 스트림 오픈..
						pathDetail	= RsPathnameCreate	(	RWSTRING( strDetailSectorUnit	)	);
						ASSERT( NULL != pathDetail	);
						pDetailStream	= RwStreamOpen	(	rwSTREAMFILENAME , rwSTREAMWRITE , pathDetail	);
						ASSERT( pDetailStream	!= NULL );
						if( pDetailStream == NULL  )
						{
							TRACE( "파일 오픈 실패!\n" );
							continue;
						}

						RsPathnameDestroy( pathDetail	);

						pDWSector		= RpDWSectorGetDetail( pWorldSector , SECTOR_HIGHDETAIL );
						// 뽑아낸거 저장함..

						ASSERT( NULL != pDWSector			);
						ASSERT( NULL != pDWSector->atomic	);
						ASSERT( NULL != pDWSector->geometry	);

						// 마고자 (2003-12-05 오후 3:14:06) : 콜리전 아토믹이 있을경우 여기서 추가함..
						if( m_pfGetCollisionAtomicCallbackFromAtomic )
						{
							pCollisionAtomic = m_pfGetCollisionAtomicCallbackFromAtomic( pDWSector->atomic , NULL );
						}
						else
						{
							pCollisionAtomic = NULL;
						}

						if( pDWSector->geometry )
							RpCollisionGeometryDestroyData( pDWSector->geometry );

						// 머티리얼 소팅
						pSortedGeometry = RpGeometrySortByMaterial( pDWSector->geometry , NULL );

						pOriginalGeometry = pDWSector->geometry;

						// 살짝 바꿔치기.
						rpGeometryAddRef( pOriginalGeometry );
						LockFrame();
						RpAtomicSetGeometry( pDWSector->atomic , pSortedGeometry , 0 );
						UnlockFrame();
						pDWSector->geometry	= pSortedGeometry;

						{
							// 인스턴싱하지 않음..
							TRACE( "(%d) 섹터 인덱스 ( %d , %d ) , 싸이즈 %d\n" , ( z2 - z ) * 10 + ( x2 - x ) + 1 , x2 , z2 , RpDWSectorStreamGetSize( pDWSector ) );
							VERIFY( RpDWSectorStreamWrite( pDWSector , pDetailStream ) );

							TRACE( "이지형은 콜리젼 아토믹이 없습니다.\n" );
						}

						// 살짝 돌려놓기.
						LockFrame();
						RpAtomicSetGeometry( pDWSector->atomic , pOriginalGeometry , 0 );
						UnlockFrame();
						pDWSector->geometry	= pOriginalGeometry;

						RpGeometryDestroy( pSortedGeometry		);
						RpGeometryDestroy( pOriginalGeometry	); // 레퍼런스 카우트 낮추기.

						// 스트림 클로즈..
						RwStreamClose	( pDetailStream	, NULL	);
					
						// 압축 리스트 작성..
						pCompactInfo = new CMagPackHeaderInfo;
						sprintf( pCompactInfo->filename , strDetailSectorUnit	);
						listDetail.AddTail( pCompactInfo );

						// 마고자 (2004-12-02 오후 5:50:06) : 
						// 콜리젼 데이타 정리..
						RpCollisionBuildParam		param			;
						RpCollisionGeometryBuildData( pOriginalGeometry , &param );
						
					}

					// 로우 테일..
					if( bRough )
					{
						TRACE( "Rough 스트림 스타트.\n" );
						wsprintf( strRoughSectorUnit , SECTOR_TEMP_DIRECTORY "\\" SECTOR_DFF_FILE_FORMAT_ROUGH , 
							pWorldSector->GetArrayIndexX() , pWorldSector->GetArrayIndexZ() );

						pathRough	= RsPathnameCreate	(	RWSTRING( strRoughSectorUnit	)	);
						ASSERT( NULL != pathRough	);
						pRoughStream	= RwStreamOpen	(	rwSTREAMFILENAME , rwSTREAMWRITE , pathRough	);
						ASSERT( pRoughStream	!= NULL );			
						if( pRoughStream == NULL  )
						{
							TRACE( "파일 오픈 실패!\n" );
							continue;
						}
						RsPathnameDestroy( pathRough	);

						pDWSector		= RpDWSectorGetDetail( pWorldSector , SECTOR_LOWDETAIL );
						// 뽑아낸거 저장함..

						ASSERT( NULL != pDWSector			);
						ASSERT( NULL != pDWSector->atomic	);
						ASSERT( NULL != pDWSector->geometry	);

						// 마고자 (2003-12-05 오후 3:14:06) : 콜리전 아토믹이 있을경우 여기서 추가함..
						if( m_pfGetCollisionAtomicCallbackFromAtomic )
						{
							pCollisionAtomic = m_pfGetCollisionAtomicCallbackFromAtomic( pDWSector->atomic , NULL );
						}
						else
						{
							pCollisionAtomic = NULL;
						}
						
						RpCollisionGeometryDestroyData( pDWSector->geometry );
						// 머티리얼 소팅
						VERIFY( pSortedGeometry = RpGeometrySortByMaterial( pDWSector->geometry , NULL ) );

						pOriginalGeometry = pDWSector->geometry;

						// 살짝 바꿔치기.
						rpGeometryAddRef( pOriginalGeometry );
						LockFrame();
						RpAtomicSetGeometry( pDWSector->atomic , pSortedGeometry , 0 );
						UnlockFrame();
						pDWSector->geometry	= pSortedGeometry;

							// 텍스쳐 익스포트..
						{
							wsprintf( filename , SECTOR_ROUGH_TEXTURE , x2 , z2 );

							SetCurrentDirectory	( m_strToolImagePath );
							this->m_pcsAgcmResourceLoader->EnableEncryption( TRUE );
							RwTexture	* pTexture = this->m_pcsAgcmResourceLoader->LoadTexture( filename , NULL , NULL , NULL , -1 , "world\\rough\\" );
							SetCurrentDirectory( strDir );
							RwTextureAddRef( pTexture );

							ASSERT( NULL != pTexture );

							// 텍스쳐 딕셔너리 생성.
							RwTexDictionary *	pTextureDictionary = RwTexDictionaryCreate();
							RwTexDictionaryAddTexture( pTextureDictionary , pTexture );
							RwTexDictionaryStreamWrite( pTextureDictionary , pRoughStream );

							// 텍스쳐 폭!
							RwTextureDestroy( pTexture );
							pTexture = NULL;

							// 텍스쳐딕셔너리 폭!
							RwTexDictionaryDestroy( pTextureDictionary );
							pTextureDictionary = NULL;
						}


						if( pCollisionAtomic )
						{
							// 마고자 (2003-12-08 오전 11:31:28) : 인스턴싱 적용..
							if( RwCameraBeginUpdate( m_pstCamera ) )
							{
								RpAtomicInstance(pDWSector->atomic);
								RwCameraEndUpdate( m_pstCamera );
							}

							TRACE( "(%d) 섹터 인덱스 ( %d , %d ) , 싸이즈 %d\n" , ( z2 - z ) * 10 + ( x2 - x ) + 1 , x2 , z2 , RpDWSectorStreamGetSize( pDWSector ) );
			
							VERIFY( RpDWSectorStreamWrite( pDWSector , pRoughStream ) );

							// 콜리젼 아토믹 익스포트..
							VERIFY( RpAtomicStreamWrite( pCollisionAtomic , pRoughStream ) );
						}
						else
						{
							// 인스턴싱하지 않음..
							TRACE( "(%d) 섹터 인덱스 ( %d , %d ) , 싸이즈 %d\n" , ( z2 - z ) * 10 + ( x2 - x ) + 1 , x2 , z2 , RpDWSectorStreamGetSize( pDWSector ) );
							VERIFY( RpDWSectorStreamWrite( pDWSector , pRoughStream ) );

							TRACE( "이지형은 콜리젼 아토믹이 없습니다.\n" );
						}

						// 살짝 돌려놓기.
						LockFrame();
						RpAtomicSetGeometry( pDWSector->atomic , pOriginalGeometry , 0 );
						UnlockFrame();
						pDWSector->geometry	= pOriginalGeometry;

						RpGeometryDestroy( pSortedGeometry		);
						RpGeometryDestroy( pOriginalGeometry	); // 레퍼런스 카우트 낮추기.

						// 스트림 Close..
						RwStreamClose	( pRoughStream	, NULL	);

						// 압축 리스트 작성..
						pCompactInfo = new CMagPackHeaderInfo;
						sprintf( pCompactInfo->filename , strRoughSectorUnit	);
						listRough.AddTail( pCompactInfo );

						// 마고자 (2004-12-02 오후 5:50:06) : 
						// 콜리젼 데이타 정리..
						RpCollisionBuildParam		param			;
						RpCollisionGeometryBuildData( pOriginalGeometry , &param );
					}

					// 클라이언트 데이타 생성..
					if( bServer || bCompact )
					{
						VERIFY( CreateCompactDataFromDetailInfo( pWorldSector )	);

						if( bCompact || bServer )
						{
							// 압축 리스트 작성..
							pCompactInfo = new CMagPackHeaderInfo;
							sprintf( pCompactInfo->filename , "%s\\C%d,%d.amf"		,
								pWorldSector->m_pModuleMap->GetMapCompactDirectory(),
								pWorldSector->GetArrayIndexX()						,
								pWorldSector->GetArrayIndexZ()						);
							listCompact.AddTail( pCompactInfo );

							VERIFY( pWorldSector->SaveCompactData()					);
						}
						if( bServer )
						{
							pCompactInfo = new CMagPackHeaderInfo;
							sprintf( pCompactInfo->filename , "%s\\S%d,%d.amf"		,
								pWorldSector->m_pModuleMap->GetMapCompactDirectory(),
								pWorldSector->GetArrayIndexX()						,
								pWorldSector->GetArrayIndexZ()						);
							listServer.AddTail( pCompactInfo );

							VERIFY( pWorldSector->SaveCompactData( TRUE )			);
						}
					}
				}
			}

			TRACE( "스트림 클로즈.\n" );

			// 파일이름 생성..
			wsprintf( strDetailSector , ALEF_WORLD_DETAIL_SECTOR_FILE_NAME_FORMAT , 
				GetDivisionIndex( x , z ) );
			wsprintf( strRoughSector , ALEF_WORLD_ROUGH_SECTOR_FILE_NAME_FORMAT , 
				GetDivisionIndex( x , z ) );

			// 클라이언트 데이타파일 생성..		
			wsprintf( strCompactSector , ALEF_WORLD_COMPACT_SECTOR_FILE_NAME_FORMAT , 
				GetDivisionIndex( x , z ) );
			wsprintf( strServerData , COMPACT_SERVER_FORMAT , 
				GetDivisionIndex( x , z ) );

			if( bDetail )
			{
				TRACE( "Detail 압축중..\n" );
				VERIFY( PackFiles( strDetailSector	, &listDetail	) );

				if( pDestinationDirectory )
				{
					char	strDestination[ 1024 ];
					// 지정된 디렉토리에..
					// pDestinationDirectory 는 "abc" 까지만.. 끝에 \\ 안붙음.
					wsprintf( strDestination , "%s\\%s\\World\\%s" , m_strToolImagePath , pDestinationDirectory , strDetailSector );
					CopyFile( strDetailSector , strDestination , FALSE );
				}
			}

			if( bRough )
			{
				TRACE( "Rough 압축중..\n" );
				VERIFY( PackFiles( strRoughSector	, &listRough	) );

				if( pDestinationDirectory )
				{
					char	strDestination[ 1024 ];
					// 지정된 디렉토리에..
					// pDestinationDirectory 는 "abc" 까지만.. 끝에 \\ 안붙음.
					wsprintf( strDestination , "%s\\%s\\World\\%s" , m_strToolImagePath , pDestinationDirectory , strRoughSector );
					CopyFile( strRoughSector , strDestination , FALSE );
				}
			}

			if( bCompact )
			{
				TRACE( "Compact 압축중..\n" );
				VERIFY( PackFiles( strCompactSector	, &listCompact	) );

				if( pDestinationDirectory )
				{
					char	strDestination[ 1024 ];
					// 지정된 디렉토리에..
					// pDestinationDirectory 는 "abc" 까지만.. 끝에 \\ 안붙음.
					wsprintf( strDestination , "%s\\%s\\World\\%s" , m_strToolImagePath , pDestinationDirectory , strCompactSector );
					CopyFile( strCompactSector , strDestination , FALSE );
				}
			}

			static BOOL bUseNewVersionServerdata = TRUE;
			if( bServer )
			{
				if( bUseNewVersionServerdata )
				{
					// 마고자 (2006-03-07 오전 11:00:54) : 
					// 서버데이타 저장.. 
					// 압축하지 않는 포멧
					char	strDestination[ 1024 ];
					// 지정된 디렉토리에..
					// pDestinationDirectory 는 "abc" 까지만.. 끝에 \\ 안붙음.
					wsprintf( strDestination , "%s\\World\\server\\%s" , m_strToolImagePath , strServerData );

					FILE	* pFile = fopen( strDestination , "wb" );
					if( NULL == pFile )
					{
						// 에러!
					}
					else
					{
						// SERVER_DATA_VERSION 
						// 버젼 정보 기록..
						// 각 섹터 데이타의 싸이즈를 기록함.

						UINT32	uVersion = SERVER_DATA_VERSION;
						fwrite( ( void * ) &uVersion , sizeof( uVersion ) , 1 , pFile );

						char	strFilePath[ 1024 ];
						AuNode< CMagPackHeaderInfo * >	* pNode	= listCompact.GetHeadNode();
						CMagPackHeaderInfo * pInfo;

						UINT32	uOffset = 4; // 버젼 기록한후니까..

						while( pNode )
						{
							pInfo = pNode->GetData();

							FILE	*pSource;

							UINT32 uSize = 0;

							char strFname[ 256 ] , strExt[ 256 ];
							_splitpath( pInfo->filename , NULL , NULL , strFname , strExt );

							sprintf( strFilePath , "%s\\Map\\Data\\Compact\\%s%s", m_strToolImagePath , strFname , strExt );
							pSource = fopen( strFilePath , "rb" );
							if( pSource )
							{
								//char pBuffer[ 65535 ];

								fseek( pSource , 0 , SEEK_END );
								uSize = ftell( pSource );
								fclose( pSource );
							}

							// 옵셋을 기록..

							UINT32	uDataOffset = uOffset + 256 * sizeof( UINT32 );
							fwrite( ( void * ) &uDataOffset , sizeof( uVersion ) , 1 , pFile );
							uOffset += uSize;

							pNode = pNode->GetNextNode();
						}

						pNode	= listCompact.GetHeadNode();
						while( pNode )
						{
							pInfo = pNode->GetData();

							char strFname[ 256 ] , strExt[ 256 ];
							_splitpath( pInfo->filename , NULL , NULL , strFname , strExt );

							sprintf( strFilePath , "%s\\Map\\Data\\Compact\\%s%s", m_strToolImagePath , strFname , strExt );

							FILE	*pSource;
							pSource = fopen( strFilePath , "rb" );
							if( pSource )
							{
								char pBuffer[ 65535 ];

								int nSize;
								fseek( pSource , 0 , SEEK_END );
								nSize = ftell( pSource );
								fseek( pSource , 0 , SEEK_SET );
								fread( ( void * ) pBuffer , nSize , sizeof( char ), pSource );
								fclose( pSource );

								// 라이팅..
								fwrite( ( void * ) pBuffer , nSize , sizeof( char ), pFile );
							}

							pNode = pNode->GetNextNode();
						}

						fclose( pFile );

						if( pDestinationDirectory )
						{
							char	strDestination2[ 1024 ];
							wsprintf( strDestination2 , "%s\\%s\\World\\server\\%s" , m_strToolImagePath , pDestinationDirectory , strServerData );
							CopyFile( strDestination , strDestination2 , FALSE );
						}
					}
				}
				else
				{
					// 마고자 (2006-03-07 오전 11:00:54) : 
					// 서버데이타 저장.. 
					// 압축하지 않는 포멧
					char	strDestination[ 1024 ];
					// 지정된 디렉토리에..
					// pDestinationDirectory 는 "abc" 까지만.. 끝에 \\ 안붙음.
					wsprintf( strDestination , "%s\\World\\server\\%s" , m_strToolImagePath , strServerData );

					FILE	* pFile = fopen( strDestination , "wb" );
					if( NULL == pFile )
					{
						// 에러!
					}
					else
					{
						//char	strFilePath[ 1024 ];
						AuNode< CMagPackHeaderInfo * >	* pNode	= listServer.GetHeadNode();
						CMagPackHeaderInfo * pInfo;
						while( pNode )
						{
							pInfo = pNode->GetData();

							FILE	*pSource;

							pSource = fopen( pInfo->filename , "rb" );
							if( pSource )
							{
								char pBuffer[ 65535 ];

								int nSize;
								fseek( pSource , 0 , SEEK_END );
								nSize = ftell( pSource );
								fseek( pSource , 0 , SEEK_SET );
								fread( ( void * ) pBuffer , nSize , sizeof( char ), pSource );
								fclose( pSource );

								// 라이팅..
								fwrite( ( void * ) pBuffer , nSize , sizeof( char ), pFile );
							}

							pNode = pNode->GetNextNode();
						}

						fclose( pFile );

						if( pDestinationDirectory )
						{
							char	strDestination2[ 1024 ];
							wsprintf( strDestination2 , "%s\\%s\\World\\server\\%s" , m_strToolImagePath , pDestinationDirectory , strServerData );
							CopyFile( strDestination , strDestination2 , FALSE );
						}
					}
				}
			}

			TRACE( "데이타 정리중....\n" );
			// 리스트 제거..
			while( listDetail.GetHeadNode() )
			{
				pCompactInfo = listDetail.GetHead();
				wsprintf( strDetailSectorUnit , "%s\\%s" , SECTOR_TEMP_DIRECTORY , pCompactInfo->filename );
				DeleteFile( strDetailSectorUnit );
				delete pCompactInfo;
				listDetail.RemoveHead();
			}
			// 리스트 제거..
			while( listRough.GetHeadNode() )
			{
				pCompactInfo = listRough.GetHead();
				wsprintf( strRoughSectorUnit , "%s\\%s" , SECTOR_TEMP_DIRECTORY , pCompactInfo->filename );
				DeleteFile( strRoughSectorUnit );
				delete pCompactInfo;
				listRough.RemoveHead();
			}
			// 리스트 제거..
			while( listCompact.GetHeadNode() )
			{
				pCompactInfo =  listCompact.GetHead();

				char strFname[ 256 ] , strExt[ 256 ];
				_splitpath( pCompactInfo->filename , NULL , NULL , strFname , strExt );

				char strFilename[ 1024 ];
				sprintf( strFilename , "%s\\%s%s"		,
							pWorldSector->m_pModuleMap->GetMapCompactDirectory() , 
							strFname , strExt);

				DeleteFile( strFilename );
				delete pCompactInfo;
				listCompact.RemoveHead();
			}
			while( listServer.GetHeadNode() )
			{
				pCompactInfo =  listServer.GetHead();

				DeleteFile( pCompactInfo->filename );
				delete pCompactInfo;
				listServer.RemoveHead();
			}

			TRACE( "한개완료\n" );
		}//for( int z = GetLoadRangeZ1() ; z < GetLoadRangeZ2() ; z += 10 )
	}//for( int x = GetLoadRangeX1() ; x < GetLoadRangeX2() ; x += 10 )

	// 템포러리 디렉토리 제거..
	// 

	TRACE( "익스포트 완료\n" );
	// 디렉토리 변경..
	SetCurrentDirectory	( m_strToolImagePath );

	return TRUE;
}
ApWorldSector *	AgcmMap::GetMapPositionFromMousePosition_tool( INT32 x , INT32 y , FLOAT *px , FLOAT *py , FLOAT *pz , INT32 *pnPolygonIndex , FLOAT * pFoundDistance )
{
	// 리턴이 -1일 경우 에러처리.
	// 그외의 경우는 참임.
	ASSERT( NULL != m_pstCamera );
	if( NULL == m_pstCamera ) return NULL;

	// 아싸..;

	RpIntersection	intersection	;
	RwLine			pixelRay		;
	RwV2d			pos				;

	pos.x			= ( FLOAT ) x	;
	pos.y			= ( FLOAT ) y	;

	RwCameraCalcPixelRay( m_pstCamera , &pixelRay , &pos );

	intersection.type	=	rpINTERSECTLINE	;
	intersection.t.line	=	pixelRay		;

	RpAtomic		* pAtomic		;
	ApWorldSector	* pSector		= NULL;

	__IntersectionPolugonInfo	Info;
	Info.nPolygonIndex	= -1;
	Info.bCheckVerticalPolygon = TRUE;

	// 콜리젼 데이타 초기화
	
	// 콜리전 테스팅..
	AuList< _FindWorldSectorGeometryStruct >	listWorldSector;

	if(!m_pcsApmOcTree || !m_pcsApmOcTree->m_bOcTreeEnable)
	{
		if(m_bUseCullMode)
		{
			LoadingSectorsForAllAtomicsIntersection5( &intersection,
			__GetWorldSectorGeometryCallback	,
			( void * ) &listWorldSector	, FALSE		);
		}
		else
		{
			m_pcsAgcmRender->RenderWorldForAllIntersections( 
			&intersection					,
			__GetWorldSectorGeometryCallback	,
			( void * ) &listWorldSector		);
		}
	}
	else
	{
		m_pcsAgcmOcTree->OcTreesForAllAtomicsIntersection(3,&intersection,
		__GetWorldSectorGeometryCallback	,
		( void * ) &listWorldSector	, FALSE		);
	}

	FLOAT			fFoundDistance		;
	RpAtomic	*	pCollisionAtomic	= NULL;

	switch( listWorldSector.GetCount() )
	{
	case 0:
		// 콜리젼 된것 없음.

		return NULL;

	case 1:
		pAtomic	= listWorldSector.GetHeadNode()->GetData().pAtomic;

		pSector = AcuGetWorldSectorFromAtomicPointer( pAtomic );

		if( pSector == NULL ) break;

		if( pAtomic )
		{
			INT32		nType		;
			INT32		nIndex		;
			nType				= AcuObject::GetAtomicType				( pAtomic , & nIndex	);
			pCollisionAtomic	= AcuObject::GetAtomicGetPickingAtomic	( pAtomic				);

			if( NULL == pCollisionAtomic )
			{
				// 없는건 어쩔수없고.
				pCollisionAtomic = pAtomic;
			}
			else
			{
				// 우헤헤 --;
				// 성공이닷 - -++
			}

			if( AcuObject::GetType( nType ) == ACUOBJECT_TYPE_WORLDSECTOR && pCollisionAtomic && pCollisionAtomic->geometry->triangles )
			{
				Info.bCheckVerticalPolygon = TRUE;

				RpCollisionGeometryForAllIntersections (
					RpAtomicGetGeometry( pCollisionAtomic )	,
					&intersection					,
					__RsIntersectionCallBackGeometryTriangleAtomic ,
					( void * ) &Info );
			}
		}
		else
		{
			return NULL;
		}

		fFoundDistance	= Info.fDistance;

		break;

	default:
		// 라인을 검사함..
		{
			AuNode< _FindWorldSectorGeometryStruct >	* pNode = listWorldSector.GetHeadNode()	;
			_FindWorldSectorGeometryStruct				* pGeometryStruct						;

			bool										bFirst = true							;

			__IntersectionPolugonInfo					InfoCurrent;

			while( pNode )
			{
				pGeometryStruct	= & pNode->GetData();

				InfoCurrent.nPolygonIndex	= -1;
				InfoCurrent.bCheckVerticalPolygon = TRUE;

				intersection.type	=	rpINTERSECTLINE	;
				intersection.t.line	=	pixelRay		;

				INT32		nType		;
				INT32		nIndex		;

				if (RpAtomicGetClump(pGeometryStruct->pAtomic))
					nType	= AcuObject::GetClumpType( RpAtomicGetClump(pGeometryStruct->pAtomic) , & nIndex	);
				else
					nType	= AcuObject::GetAtomicType( pGeometryStruct->pAtomic , & nIndex	);

				if (nType & (ACUOBJECT_TYPE_OBJECT | ACUOBJECT_TYPE_BLOCKING))
					InfoCurrent.bCheckVerticalPolygon = FALSE;

				pCollisionAtomic	= AcuObject::GetAtomicGetPickingAtomic	( pGeometryStruct->pAtomic				);

				if( NULL == pCollisionAtomic )
				{
					// 없는건 어쩔수없고.
					pCollisionAtomic = pGeometryStruct->pAtomic;
				}
				else
				{
					// 우헤헤 --;
					// 성공이닷 - -++
				}

				if( AcuObject::GetType( nType ) == ACUOBJECT_TYPE_WORLDSECTOR )
				{
					LockFrame();

					if( RpAtomicGetGeometry( pCollisionAtomic ) && 
						RpGeometryGetTriangles( RpAtomicGetGeometry( pCollisionAtomic ) ) &&
						RpAtomicForAllIntersections (
							pCollisionAtomic								,
							&intersection									,
							__RsIntersectionCallBackGeometryTriangleAtomic	,
							( void * ) &InfoCurrent )
						)
					{
						// 뭔가 발견 돼었다.
						if( InfoCurrent.nPolygonIndex != -1 )
						{
							if( bFirst )
							{
								bFirst			= false											;
								fFoundDistance	= InfoCurrent.fDistance							;
								pAtomic			= pGeometryStruct->pAtomic						;

								// 마고자 (2004-01-07 오후 4:05:53) : 유져데이타에서 얻어내는게 좋긴하나..
								// 콜리젼 아토믹을 한칸 크게 했기때문에 , 옆에 섹터가 불릴 가능성이 크다.
								// 그래서 그냥 맵에서 섹터를 얻어 오게 하였다.

								// pSector			= AcuGetWorldSectorFromAtomicPointer( pAtomic )	;
								// if( NULL == pSector )
									pSector = m_pcsApmMap->GetSector( InfoCurrent.vPos.x , InfoCurrent.vPos.z );

								Info = InfoCurrent;
							}
							else if( InfoCurrent.fDistance < fFoundDistance )
							{
								fFoundDistance	= InfoCurrent.fDistance							;
								pAtomic			= pGeometryStruct->pAtomic						;
								
								// 마고자 (2004-01-07 오후 4:05:53) : 유져데이타에서 얻어내는게 좋긴하나..
								// 콜리젼 아토믹을 한칸 크게 했기때문에 , 옆에 섹터가 불릴 가능성이 크다.
								// 그래서 그냥 맵에서 섹터를 얻어 오게 하였다.

								//pSector			= AcuGetWorldSectorFromAtomicPointer( pAtomic )	;
								//if( NULL == pSector )
									pSector = m_pcsApmMap->GetSector( InfoCurrent.vPos.x , InfoCurrent.vPos.z );

								Info = InfoCurrent;
							}
						}
					}

					UnlockFrame();
				}

				listWorldSector.GetNext( pNode );
			}
			
		}
		break;
	}

	if( -1 != Info.nPolygonIndex )
	{
		if( px ) *px = Info.vPos.x;
		if( py ) *py = Info.vPos.y;
		if( pz ) *pz = Info.vPos.z;
		if( pnPolygonIndex ) *pnPolygonIndex = Info.nPolygonIndex;
		if( pFoundDistance ) *pFoundDistance = ( float ) ( fFoundDistance * AUPOS_DISTANCE(intersection.t.line.start, intersection.t.line.end) );

		return pSector;
	}
	else
	{
		return NULL;
	}
}

BOOL	AgcmMap::GetMapPositionFromMousePosition( INT32 x , INT32 y , FLOAT *px , FLOAT *py , FLOAT *pz , INT32 *pnPolygonIndex , FLOAT * pFoundDistanceRatio , FLOAT * pFoundDistance )
{
	// 리턴이 -1일 경우 에러처리.
	// 그외의 경우는 참임.
	ASSERT( NULL != m_pstCamera );
	if( NULL == m_pstCamera ) return FALSE;

	// 아싸..;

	RpIntersection	intersection	;
	RwLine			pixelRay		;
	RwV2d			pos				;

	pos.x			= ( FLOAT ) x	;
	pos.y			= ( FLOAT ) y	;

	RwCameraCalcPixelRay( m_pstCamera , &pixelRay , &pos );

	intersection.type	=	rpINTERSECTLINE	;
	intersection.t.line	=	pixelRay		;

	RpAtomic		* pAtomic		;
	// ApWorldSector	* pSector		= NULL;

	__IntersectionPolugonInfo	Info;
	Info.nPolygonIndex	= -1;
	Info.bCheckVerticalPolygon = TRUE;

	// 콜리젼 데이타 초기화
	
	// 콜리전 테스팅..
	AuList< _FindWorldSectorGeometryStruct >	listWorldSector;

	if(!m_pcsApmOcTree || !m_pcsApmOcTree->m_bOcTreeEnable)
	{
		if(m_bUseCullMode)
		{
			LoadingSectorsForAllAtomicsIntersection(3,&intersection,
			__GetWorldSectorGeometryCallback	,
			( void * ) &listWorldSector	, FALSE		);
		}
		else
		{
			m_pcsAgcmRender->RenderWorldForAllIntersections( 
			&intersection					,
			__GetWorldSectorGeometryCallback	,
			( void * ) &listWorldSector		);
		}
	}
	else
	{
		m_pcsAgcmOcTree->OcTreesForAllAtomicsIntersection(3,&intersection,
		__GetWorldSectorGeometryCallback	,
		( void * ) &listWorldSector	, FALSE		);
	}

	FLOAT			fFoundDistance		;
	RpAtomic	*	pCollisionAtomic	= NULL;

	switch( listWorldSector.GetCount() )
	{
	case 0:
		// 콜리젼 된것 없음.

		break;

	case 1:
		pAtomic	= listWorldSector.GetHeadNode()->GetData().pAtomic;

		//pSector = AcuGetWorldSectorFromAtomicPointer( pAtomic );
		//if( pSector == NULL ) break;

		if( pAtomic )
		{
			INT32		nType		;
			INT32		nIndex		;
			nType				= AcuObject::GetAtomicType				( pAtomic , & nIndex	);
			pCollisionAtomic	= AcuObject::GetAtomicGetPickingAtomic	( pAtomic				);

			if( NULL == pCollisionAtomic )
			{
				// 없는건 어쩔수없고.
				pCollisionAtomic = pAtomic;
			}
			else
			{
				// 우헤헤 --;
				// 성공이닷 - -++
			}


			if( pCollisionAtomic && pCollisionAtomic->geometry->triangles )
			{
				Info.bCheckVerticalPolygon = TRUE;

				RpCollisionGeometryForAllIntersections (
					RpAtomicGetGeometry( pCollisionAtomic )	,
					&intersection					,
					__RsIntersectionCallBackGeometryTriangleAtomic ,
					( void * ) &Info );
			}
		}
		else
		{
			break;
		}

		fFoundDistance	= Info.fDistance;

		break;

	default:
		// 라인을 검사함..
		{
			AuNode< _FindWorldSectorGeometryStruct >	* pNode = listWorldSector.GetHeadNode()	;
			_FindWorldSectorGeometryStruct				* pGeometryStruct						;

			bool										bFirst = true							;

			__IntersectionPolugonInfo					InfoCurrent;

			while( pNode )
			{
				pGeometryStruct	= & pNode->GetData();

				InfoCurrent.nPolygonIndex	= -1;
				InfoCurrent.bCheckVerticalPolygon = TRUE;

				intersection.type	=	rpINTERSECTLINE	;
				intersection.t.line	=	pixelRay		;

				if( pGeometryStruct->pAtomic )
				{
					INT32		nType		;
					INT32		nIndex		;

					if (RpAtomicGetClump(pGeometryStruct->pAtomic))
						nType	= AcuObject::GetClumpType( RpAtomicGetClump(pGeometryStruct->pAtomic) , & nIndex	);
					else
						nType	= AcuObject::GetAtomicType( pGeometryStruct->pAtomic , & nIndex	);

					if (nType & (ACUOBJECT_TYPE_OBJECT | ACUOBJECT_TYPE_BLOCKING))
						InfoCurrent.bCheckVerticalPolygon = FALSE;

					pCollisionAtomic	= AcuObject::GetAtomicGetPickingAtomic	( pGeometryStruct->pAtomic				);

					if( NULL == pCollisionAtomic )
					{
						// 없는건 어쩔수없고.
						pCollisionAtomic = pGeometryStruct->pAtomic;
					}
					else
					{
						// 우헤헤 --;
						// 성공이닷 - -++
					}
				}

				LockFrame();

				if( RpAtomicGetGeometry( pCollisionAtomic ) && 
					RpGeometryGetTriangles( RpAtomicGetGeometry( pCollisionAtomic ) ) &&
					RpAtomicForAllIntersections (
						pCollisionAtomic								,
						&intersection									,
						__RsIntersectionCallBackGeometryTriangleAtomic	,
						( void * ) &InfoCurrent )
					)
				{
					// 뭔가 발견 돼었다.
					if( InfoCurrent.nPolygonIndex != -1 )
					{
						if( bFirst )
						{
							bFirst			= false											;
							fFoundDistance	= InfoCurrent.fDistance							;
							pAtomic			= pGeometryStruct->pAtomic						;

							// 마고자 (2004-01-07 오후 4:05:53) : 유져데이타에서 얻어내는게 좋긴하나..
							// 콜리젼 아토믹을 한칸 크게 했기때문에 , 옆에 섹터가 불릴 가능성이 크다.
							// 그래서 그냥 맵에서 섹터를 얻어 오게 하였다.

							// if( NULL == pSector )
							// pSector = m_pcsApmMap->GetSector( InfoCurrent.vPos.x , InfoCurrent.vPos.z );

							Info = InfoCurrent;
						}
						else if( InfoCurrent.fDistance < fFoundDistance )
						{
							fFoundDistance	= InfoCurrent.fDistance							;
							pAtomic			= pGeometryStruct->pAtomic						;
							
							// 마고자 (2004-01-07 오후 4:05:53) : 유져데이타에서 얻어내는게 좋긴하나..
							// 콜리젼 아토믹을 한칸 크게 했기때문에 , 옆에 섹터가 불릴 가능성이 크다.
							// 그래서 그냥 맵에서 섹터를 얻어 오게 하였다.

							//if( NULL == pSector )
							// pSector = m_pcsApmMap->GetSector( InfoCurrent.vPos.x , InfoCurrent.vPos.z );

							Info = InfoCurrent;
						}
					}
				}

				UnlockFrame();

				listWorldSector.GetNext( pNode );
			}
			
		}
		break;
	}

	if( -1 != Info.nPolygonIndex )
	{
		if( px ) *px = Info.vPos.x;
		if( py ) *py = Info.vPos.y;
		if( pz ) *pz = Info.vPos.z;
		if( pnPolygonIndex ) *pnPolygonIndex = Info.nPolygonIndex;

		//@{ kday 20050616
		// ;)
		if( pFoundDistanceRatio ) *pFoundDistanceRatio = fFoundDistance;
		if( pFoundDistance ) *pFoundDistance = ( float ) ( fFoundDistance * AUPOS_DISTANCE(intersection.t.line.start, intersection.t.line.end) );
		//@} kday

		return TRUE;
	}
	else
	{
		if( px ) *px =  pixelRay.end.x;
		if( py ) *py =  pixelRay.end.y;
		if( pz ) *pz =  pixelRay.end.z;

		if( pFoundDistanceRatio ) *pFoundDistanceRatio = 1.0f;
		if( pFoundDistance ) *pFoundDistance = AgcmEventNature::SKY_RADIUS;
		return TRUE;
	}
}

BOOL	AgcmMap::SetAutoLoadRange		( INT32 nMapdata , INT32 nRough , INT32 nDetail)
{
	const FLOAT fRate = 1.0f;

	// 마고자 (2004-02-19 오전 10:36:03) : 어설트를 띄우지 않고 , 알아서 바꾸게 했음.
	// ASSERT( nMapdata	>= nRough	);
	// ASSERT( nRough		>= nDetail	);

	if( nMapdata	< nRough	) nMapdata	= nRough	;
	if( nRough		< nDetail	) nRough	= nDetail	;

	m_nMapDataLoadRadius	= nMapdata	;
	m_nRoughLoadRadius		= nRough	;
	m_nDetailLoadRadius		= nDetail	;

	m_nMapDataReleaseRadius	= ( INT32 ) ( ( FLOAT ) nMapdata* fRate );
	m_nRoughReleaseRadius	= ( INT32 ) ( ( FLOAT ) nRough	* fRate );	// fRate 범위만큼 넓게 잡는다.
	m_nDetailReleaseRadius	= ( INT32 ) ( ( FLOAT ) nDetail	* fRate );

	if(m_pcsApmOcTree)
	{
		m_pcsApmOcTree->m_iLoadRange = m_nMapDataReleaseRadius;
	}

	if(m_pcsAgcmRender)
	{
		m_pcsAgcmRender->m_iDrawRange = nRough;
	}

	return TRUE;
}
BOOL	AgcmMap::__SetAutoLoadRange		( INT32 nMapdata , INT32 nRough , INT32 nDetail , INT32 nViewOption )
{
	BOOL	bPrevMode = m_pcsAgcmResourceLoader->m_bForceImmediate;

	SetAutoLoadRange( nMapdata, nRough, nDetail );

	if (m_bAvatarPositionSet)
	{
		m_pcsAgcmResourceLoader->m_bForceImmediate = TRUE;
		MyCharacterPositionChange(m_posCurrentAvatarPosition, m_posCurrentAvatarPosition, TRUE);
		m_pcsAgcmResourceLoader->m_bForceImmediate = bPrevMode;
	}

	// 마고자 (2005-03-31 오후 5:43:45) : 콜백호출 
	EnumCallback( AGCM_CB_ID_SETAUTOLOADCHANGE , ( void * ) nViewOption , ( void * ) nDetail);

	return TRUE;

	const FLOAT fRate = 1.0f;

	ASSERT( nMapdata	>= nRough	);
	ASSERT( nRough		>= nDetail	);

	m_nMapDataLoadRadius	= nMapdata	;
	m_nRoughLoadRadius		= nRough	;
	m_nDetailLoadRadius		= nDetail	;

	m_nMapDataReleaseRadius	= ( INT32 ) ( ( FLOAT ) nMapdata* fRate );
	m_nRoughReleaseRadius	= ( INT32 ) ( ( FLOAT ) nRough	* fRate );	// fRate 범위만큼 넓게 잡는다.
	m_nDetailReleaseRadius	= ( INT32 ) ( ( FLOAT ) nDetail	* fRate );

	// 아바타의 위치를 구함..
	ApWorldSector * pCenterSector = m_pcsApmMap->GetSector( m_posCurrentAvatarPosition );

	// 로드 되어있지 않은 섹터다.
	if( NULL == pCenterSector ) return TRUE;
	
	// 이미 올라간 지형의 디테일을 수정한다.
	ApWorldSector	*	pSector			;
	int					nCurrentDetail	;
	int					nTargetDetail	;
	int					nDistance		;


	for( int i = 0 ; i < ( int ) m_pcsApmMap->GetCurrentLoadedSectorCount() ; ++ i )
	{
		pSector = m_pcsApmMap->GetCurrentLoadedSectors()[ i ];

		// 거리와 디테일을 구함..
		nCurrentDetail	= GetCurrentDetail	( pSector					);
		nDistance		= GetSectorDistance	( pSector , pCenterSector	);

		// 타겟 디테일 결정..
		if		( nDistance <= GetDetailLoadRadius	() ) nTargetDetail = SECTOR_HIGHDETAIL	;
		else if	( nDistance <= GetRoughLoadRadius	() ) nTargetDetail = SECTOR_LOWDETAIL	;
		else	nTargetDetail = SECTOR_EMPTY;

		if( nCurrentDetail != nTargetDetail )
		{
			// 못 읽어들인것 다시 읽어서 디테일을 수정한다..
			switch( nTargetDetail )
			{
			case SECTOR_HIGHDETAIL	:
				{
					m_csMapLoader.LoadDWSector( pSector , SECTOR_LOWDETAIL	);
					m_csMapLoader.LoadDWSector( pSector , SECTOR_HIGHDETAIL	);
				}
				break;
			case SECTOR_LOWDETAIL	:
				{
					m_csMapLoader.LoadDWSector( pSector , SECTOR_LOWDETAIL	);
				}
				break;
			case SECTOR_EMPTY		:
				{
				}
				break;
			}

			SetCurrentDetail( pSector , nTargetDetail );
		}

		if( nDistance > GetDetailReleaseRadius	() )	m_csMapLoader.RemoveQueue( pSector , SECTOR_HIGHDETAIL );
		if( nDistance > GetRoughReleaseRadius	() )	m_csMapLoader.RemoveQueue( pSector , SECTOR_LOWDETAIL );
		if( nDistance > GetMapDataReleaseRadius	() )
		{
			if( m_pcsApmMap->IsAutoLoadData() )
			{
				m_csMapLoader.RemoveQueue( pSector );
				VERIFY( m_pcsApmMap->DeleteSector( pSector ) );
			}
		}
	}

	return TRUE;
}

BOOL	AgcmMap::IsLoadedDetail				( ApWorldSector * pWorldSector , INT32 nDetail )
{
	ASSERT( NULL != pWorldSector );
	if( NULL == pWorldSector ) return FALSE;
	RpDWSector * pDWSector = RpDWSectorGetDetail( pWorldSector , nDetail );

	if( pDWSector )
	{
		if( pDWSector->atomic )
		{
			return TRUE;
		}
		else
		{
			// 로딩큐에 들어있는지 점검..
			return FALSE;
		}
	}
	else
	{
		//ASSERT( !"섹터가 로딩돼면 무조건 DWSector는 잡혀 있어야 하는데 ,. 이런경우는 나오면 안됀다." );
		return FALSE;
	}
}

BOOL	AgcmMap::MakeRoughPolygon		( INT32 nDetail , INT32 nType , INT32 x , INT32 z , FLOAT fStepSize ,  _MakeDetailDataSet * pDataSet , ApDetailSegment * pSegment , INT32 matIndex )
{
	RwV3d				normal			= {0.0f , 1.0f , 0.0f};
	INT32				nDepth			= 4;
	INT32				nTargetDetail	= 4;
	_MakeDetailDataSet	stDataSetCopy	= * pDataSet;

	*pDataSet->pVertices		= *pDataSet->Map( x , z );
	++pDataSet->pVertices	;
	*pDataSet->pVertices		= *pDataSet->Map(x + 1, z);
	++pDataSet->pVertices	;
	*pDataSet->pVertices		= *pDataSet->Map(x, z + 1);
	++pDataSet->pVertices	;
	*pDataSet->pVertices		= *pDataSet->Map(x + 1, z + 1);
	++pDataSet->pVertices	;

	if( !pDataSet->bValidNormal )
	{
		/*
		*pDataSet->pNormals		= normal;
		++pDataSet->pNormals		;
		*pDataSet->pNormals		= normal;
		++pDataSet->pNormals		;
		*pDataSet->pNormals		= normal;
		++pDataSet->pNormals		;
		*pDataSet->pNormals		= normal;
		++pDataSet->pNormals		;
		*/
	}

	if( pDataSet->pPreLitLum )
	{
		*pDataSet->pPreLitLum	= *pDataSet->Prelight( x , z );
		++pDataSet->pPreLitLum;
		*pDataSet->pPreLitLum	= *pDataSet->Prelight( x + 1 , z );
		++pDataSet->pPreLitLum;
		*pDataSet->pPreLitLum	= *pDataSet->Prelight( x , z + 1 );
		++pDataSet->pPreLitLum;
		*pDataSet->pPreLitLum	= *pDataSet->Prelight( x + 1 , z + 1 );
		++pDataSet->pPreLitLum;
	}

	if( !pDataSet->bValidTexCoords )
	{
		pDataSet->pTexCoords[0]->u	= ( FLOAT ) x / ( FLOAT ) nDepth ;
		pDataSet->pTexCoords[0]->v	= ( FLOAT ) z / ( FLOAT ) nDepth ;

		++pDataSet->pTexCoords[0]	;
		
		pDataSet->pTexCoords[0]->u	= ( FLOAT ) ( x + 1 ) / ( FLOAT ) nDepth ;
		pDataSet->pTexCoords[0]->v	= ( FLOAT ) ( z + 0 ) / ( FLOAT ) nDepth ;

		++pDataSet->pTexCoords[0]	;

		pDataSet->pTexCoords[0]->u	= ( FLOAT ) ( x + 0 ) / ( FLOAT ) nDepth ;
		pDataSet->pTexCoords[0]->v	= ( FLOAT ) ( z + 1 ) / ( FLOAT ) nDepth ;

		++pDataSet->pTexCoords[0]	;

		pDataSet->pTexCoords[0]->u	= ( FLOAT ) ( x + 1 ) / ( FLOAT ) nDepth ;
		pDataSet->pTexCoords[0]->v	= ( FLOAT ) ( z + 1 ) / ( FLOAT ) nDepth ;

		++pDataSet->pTexCoords[0]	;
	}

	switch( nType )
	{
	case	R_LEFTTOP			:
		{
			int	i;

			// 왼쪽 위의것.
			// 왼쪽 4개 먼저..
			for( i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 0 ].x , stDataSetCopy.pVertices[ 2 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 0 ].z , stDataSetCopy.pVertices[ 2 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;
				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum ) 
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}	

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].u , stDataSetCopy.pTexCoords[ 0 ][ 2 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].v , stDataSetCopy.pTexCoords[ 0 ][ 2 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}
			
			// 위에거..
			for( i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 0 ].x , stDataSetCopy.pVertices[ 1 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 0 ].z , stDataSetCopy.pVertices[ 1 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;

				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum )
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].u , stDataSetCopy.pTexCoords[ 0 ][ 1 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].v , stDataSetCopy.pTexCoords[ 0 ][ 1 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}

			// 삼격형 설정..
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 2	, 3 ,	6	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 6	, 3 ,	5	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 5	, 3 ,	4	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 4	, 3 ,	0	, matIndex );

			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0	, 3 ,	7	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 7	, 3 ,	8	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 8	, 3 ,	9	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 9	, 3 ,	1	, matIndex );

			pDataSet->nVertexIndex += 4 + ( nDepth - 1 ) * 2;
		}
		return TRUE;
	case	R_RIGHTTOP		:
		{
			int	i;

			// 왼쪽 위의것.
			// 왼쪽 4개 먼저..
			for( i = 1 ; i < nTargetDetail ; ++i  )
			{

				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 0 ].x , stDataSetCopy.pVertices[ 1 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 0 ].z , stDataSetCopy.pVertices[ 1 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;
				
				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum ) 
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].u , stDataSetCopy.pTexCoords[ 0 ][ 1 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].v , stDataSetCopy.pTexCoords[ 0 ][ 1 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}
			
			// 위에거..
			for( i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 1 ].x , stDataSetCopy.pVertices[ 3 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 1 ].z , stDataSetCopy.pVertices[ 3 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;
	
				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum ) 
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 1 ].u , stDataSetCopy.pTexCoords[ 0 ][ 3 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 1 ].v , stDataSetCopy.pTexCoords[ 0 ][ 3 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}

			// 삼격형 설정..
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0	, 2 ,	4	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 4	, 2 ,	5	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 5	, 2 ,	6	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 6	, 2 ,	1	, matIndex );

			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 1	, 2 ,	7	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 7	, 2 ,	8	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 8	, 2 ,	9	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 9	, 2 ,	3	, matIndex );

			pDataSet->nVertexIndex += 4 + ( nDepth - 1 ) * 2;
		}
		return TRUE;
	case	R_LEFTBOTTOM		:
		{
			// 왼쪽 위의것.
			// 왼쪽 4개 먼저..
			int i;
			for(i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 0 ].x , stDataSetCopy.pVertices[ 2 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 0 ].z , stDataSetCopy.pVertices[ 2 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;

				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum )
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].u , stDataSetCopy.pTexCoords[ 0 ][ 2 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].v , stDataSetCopy.pTexCoords[ 0 ][ 2 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}
			
			// 위에거..
			for( i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 2 ].x , stDataSetCopy.pVertices[ 3 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 2 ].z , stDataSetCopy.pVertices[ 3 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;
				
				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum ) 
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 2 ].u , stDataSetCopy.pTexCoords[ 0 ][ 3 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 2 ].v , stDataSetCopy.pTexCoords[ 0 ][ 3 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}

			// 삼격형 설정..
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0	,4	 ,	1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 4	,5	 ,	1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 5	,6	 ,	1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 6	,2	 ,	1 , matIndex );

			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 2	,7 ,	1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 7	,8	 ,	1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 8	,9	 ,	1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 9	,3	 ,	1 , matIndex );

			pDataSet->nVertexIndex += 4 + ( nDepth - 1 ) * 2;
		}
		return TRUE;

	case	R_RIGHTBOTTOM		:
		{
			// 왼쪽 위의것.
			// 왼쪽 4개 먼저..
			for( int i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 2 ].x , stDataSetCopy.pVertices[ 3 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 2 ].z , stDataSetCopy.pVertices[ 3 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;
				
				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum ) 
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 2 ].u , stDataSetCopy.pTexCoords[ 0 ][ 3 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 2 ].v , stDataSetCopy.pTexCoords[ 0 ][ 3 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}
			
			// 위에거..
			for( int i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 1 ].x , stDataSetCopy.pVertices[ 3 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 1 ].z , stDataSetCopy.pVertices[ 3 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;
			
				++pDataSet->pVertices	;

				if( pDataSet->pPreLitLum )
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 1 ].u , stDataSetCopy.pTexCoords[ 0 ][ 3 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 1 ].v , stDataSetCopy.pTexCoords[ 0 ][ 3 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}

			// 삼격형 설정..
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 2	,	4	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 4	,	5	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 5	,	6	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 6	,	3	, matIndex );

			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 3	,	9	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 9	,	8	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 8	,	7	, matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 7	,	1	, matIndex );

			pDataSet->nVertexIndex += 4 + ( nDepth - 1 ) * 2;
		}
		return TRUE;
	case	R_TOP				:
		{
			// 왼쪽 위의것.

			// 추가로 4개...
			// 디테일이.. 5면.. 4개 더 만들어야 한다.
			for( int i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 0 ].x , stDataSetCopy.pVertices[ 1 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 0 ].z , stDataSetCopy.pVertices[ 1 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;
			
				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum ) 
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].u , stDataSetCopy.pTexCoords[ 0 ][ 1 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].v , stDataSetCopy.pTexCoords[ 0 ][ 1 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}
			
			// 삼격형 설정..
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 2 , 4 , 0 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 2 , 5 , 4 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 2 , 3 , 5 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 3 , 6 , 5 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 3 , 1 , 6 , matIndex );

			pDataSet->nVertexIndex += 4 + ( nDepth - 1 );
		}
		return TRUE;
	case	R_LEFT			:
		{
			// 추가로 4개...
			// 디테일이.. 5면.. 4개 더 만들어야 한다.
			for( int i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 0 ].x , stDataSetCopy.pVertices[ 2 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 0 ].z , stDataSetCopy.pVertices[ 2 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;
				
				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum ) 
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 0 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].u , stDataSetCopy.pTexCoords[ 0 ][ 2 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 0 ].v , stDataSetCopy.pTexCoords[ 0 ][ 2 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}
			
			
			// 삼각형 설정.
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 4 , 1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 4 , 5 , 1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 5 , 3 , 1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 5 , 6 , 3 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 6 , 2 , 3 , matIndex );

			pDataSet->nVertexIndex += 4 + ( nDepth - 1 );
		}
		return TRUE;
	case	R_RIGHT			:
		{
			// 추가로 4개...
			// 디테일이.. 5면.. 4개 더 만들어야 한다.
			for( int i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 1 ].x , stDataSetCopy.pVertices[ 3 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 1 ].z , stDataSetCopy.pVertices[ 3 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;
				
				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum ) 
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 1 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 1 ].u , stDataSetCopy.pTexCoords[ 0 ][ 3 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 1 ].v , stDataSetCopy.pTexCoords[ 0 ][ 3 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}
			
			
			// 삼각형 설정.
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 4 , 1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 5 , 4 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 2 , 5 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 2 , 6 , 5 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 2 , 3 , 6 , matIndex );

			pDataSet->nVertexIndex += 4 + ( nDepth - 1 );
		}
		return TRUE;
	case	R_BOTTOM			:
		{
			// 추가로 4개...
			// 디테일이.. 5면.. 4개 더 만들어야 한다.
			for( int i = 1 ; i < nTargetDetail ; ++i  )
			{
				pDataSet->pVertices->x		= _SplitValue( stDataSetCopy.pVertices[ 2 ].x , stDataSetCopy.pVertices[ 3 ].x , i , nTargetDetail ) ;
				pDataSet->pVertices->z		= _SplitValue( stDataSetCopy.pVertices[ 2 ].z , stDataSetCopy.pVertices[ 3 ].z , i , nTargetDetail ) ;
				pDataSet->pVertices->y		= pDataSet->pSector->D_GetHeight	( SECTOR_HIGHDETAIL ,	pDataSet->pVertices->x , pDataSet->pVertices->z )	;
				
				++pDataSet->pVertices	;

				/*
				// 어차피 뒤에 다시 계산하니까..
				if( !pDataSet->bValidNormal )
				{
					*pDataSet->pNormals		= normal;
					++pDataSet->pNormals		;
				}
				*/

				if( pDataSet->pPreLitLum )
				{
					pDataSet->pPreLitLum->red  	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].red		,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].red		, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->green	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].green	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].green	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->blue 	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].blue	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].blue	, i , nTargetDetail ) ;
					pDataSet->pPreLitLum->alpha	= ( RwUInt8 ) _SplitValue(	( FLOAT ) stDataSetCopy.pPreLitLum[ 2 ].alpha	,
																			( FLOAT ) stDataSetCopy.pPreLitLum[ 3 ].alpha	, i , nTargetDetail ) ;
					++pDataSet->pPreLitLum;
				}

				if( !pDataSet->bValidTexCoords )
				{
					pDataSet->pTexCoords[0]->u	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 2 ].u , stDataSetCopy.pTexCoords[ 0 ][ 3 ].u , i , nTargetDetail ) ;
					pDataSet->pTexCoords[0]->v	= _SplitValue( stDataSetCopy.pTexCoords[ 0 ][ 2 ].v , stDataSetCopy.pTexCoords[ 0 ][ 3 ].v , i , nTargetDetail ) ;

					++pDataSet->pTexCoords[0]	;
				}
			}
			
			
			// 삼각형 설정.
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 2 , 4 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 4 , 5 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 0 , 5 , 1 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 1 , 5 , 6 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 1 , 6 , 3 , matIndex );

			pDataSet->nVertexIndex += 4 + ( nDepth - 1 );
		}
		return TRUE;

	case	R_CENTER			:
		{	
			// 왼쪽 위의것.
			
			// 삼각형 설정.
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 2 , 1 , 0 , matIndex );
			_SetPolygonVertex( pDataSet->pPolygons++	, pDataSet->nVertexIndex , 2 , 3 , 1 , matIndex );
			
			pDataSet->nVertexIndex += 4;
		}
		return TRUE;

	default:
		return FALSE;
	}
}


void		AgcmMap::CopyDetailNormalToRoughNormal( ApWorldSector * pSector )
{
	rsDWSectorInfo	stDWSectorInfoDetail	;
	rsDWSectorInfo	stDWSectorInfoRough		;

	GetDWSector( pSector , SECTOR_HIGHDETAIL	, &stDWSectorInfoDetail );
	GetDWSector( pSector , SECTOR_LOWDETAIL		, &stDWSectorInfoRough	);

	ASSERTONCE( pSector->IsLoadedDetailData( SECTOR_LOWDETAIL ) && pSector->IsLoadedDetailData( SECTOR_HIGHDETAIL ) );
	if( !pSector->IsLoadedDetailData( SECTOR_LOWDETAIL ) || !pSector->IsLoadedDetailData( SECTOR_HIGHDETAIL ) )
	{
		TRACE( "CopyDetailNormalToRoughNormal 데이타 부족\n" );
		return;
	}

	if( LockSector( pSector , GetGeometryLockMode() , SECTOR_LOWDETAIL) )
	{
		// 냐냐냐;;;
		// 노멀값은 카피한다!!!!!!!!!!!!

		int nDetail ;
		for( int nRough = 0 ; nRough < stDWSectorInfoRough.numVertices ; ++nRough  )
		{
			for( nDetail = 0 ; nDetail < stDWSectorInfoDetail.numVertices ; ++nDetail  )
			{
				if( stDWSectorInfoRough.vertices[ nRough ].x	== stDWSectorInfoDetail.vertices[ nDetail ].x	&&
					stDWSectorInfoRough.vertices[ nRough ].z	== stDWSectorInfoDetail.vertices[ nDetail ].z	)
				{
					// 같은 녀석이면...
					stDWSectorInfoRough.normals[ nRough ].x	= stDWSectorInfoDetail.normals[ nDetail ].x;
					stDWSectorInfoRough.normals[ nRough ].y	= stDWSectorInfoDetail.normals[ nDetail ].y;
					stDWSectorInfoRough.normals[ nRough ].z	= stDWSectorInfoDetail.normals[ nDetail ].z;

					break;
				}
			}

			if( nDetail == stDWSectorInfoDetail.numVertices )
			{
				TRACE( "인생이 이러면 안돼는데....\n" );
			}
		}
	}
}

BOOL		AgcmMap::ApplyAlpha	(stFindMapInfo *pMapInfo, int basetile , int maskedindex , int alphaindex , BOOL bReculsive , BOOL bFirst )
{
	static UINT8 amask[256] = {
		0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e ,
		0x0e , 0x03 , 0x0e , 0x0e , 0x0e , 0x03 , 0x0e , 0x0e , 0x0e , 0x0e , 
		0x0e , 0x0e , 0x02 , 0x02 , 0x0e , 0x0e , 0x0e , 0x03 , 0x0e , 0x0e , 
		0x02 , 0x07 , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 
		0x0e , 0x0e , 0x0e , 0x03 , 0x0e , 0x0e , 0x0e , 0x03 , 0x0e , 0x0e , 
		// 50
		0x0e , 0x0e , 0x0e , 0x0e , 0x02 , 0x0e , 0x0e , 0x0e , 0x0e , 0x03 , 
		0x0e , 0x0e , 0x0e , 0x07 , 0x02 , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 
		0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x03 , 0x0e , 0x0e , 0x0e , 0x03 , 
		0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x02 , 0x0e , 0x0e , 0x0e , 
		0x0e , 0x03 , 0x0e , 0x0e , 0x02 , 0x07 , 0x0e , 0x0e , 0x0e , 0x0e , 
		// 100
		0x0e , 0x0e , 0x0e , 0x0e , 0x01 , 0x01 , 0x0e , 0x05 , 0x0e , 0x0e , 
		0x0e , 0x05 , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x02 , 0x0e , 
		0x01 , 0x01 , 0x01 , 0x05 , 0x01 , 0x01 , 0x0d , 0x0b , 0x0e , 0x0e , 
		0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x03 , 
		0x0e , 0x0e , 0x0e , 0x03 , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 
		// 150
		0x02 , 0x02 , 0x0e , 0x0e , 0x0e , 0x03 , 0x0e , 0x0e , 0x02 , 0x07 , 
		0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 
		0x0e , 0x03 , 0x0e , 0x0e , 0x0e , 0x03 , 0x0e , 0x0e , 0x0e , 0x0e , 
		0x0e , 0x0e , 0x02 , 0x0e , 0x0e , 0x0e , 0x0e , 0x03 , 0x0e , 0x0e , 
		0x02 , 0x07 , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 
		// 200
		0x0e , 0x0e , 0x0e , 0x03 , 0x0e , 0x0e , 0x0e , 0x03 , 0x00 , 0x00 , 
		0x00 , 0x00 , 0x00 , 0x00 , 0x06 , 0x06 , 0x00 , 0x00 , 0x00 , 0x0c , 
		0x00 , 0x00 , 0x06 , 0x0a , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 0x0e , 
		0x0e , 0x0e , 0x01 , 0x01 , 0x01 , 0x05 , 0x01 , 0x01 , 0x01 , 0x05 , 
		0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x06 , 0x06 , 0x04 , 0x04 , 
		//250
		0x04 , 0x09 , 0x04 , 0x04 , 0x08 , 0x0f
	};

	static int	offsetX[9]		= {+1, 0, -1, +1, 0, -1, +1, 0, -1};
	static int	offsetY[9]		= {+1, +1, +1, 0, 0, 0, -1, -1, -1};
	static int	m_nCheckTime	;

	int	posx2 , posy2;

	// 하하하! -_-+

	// 검사 한곳인지 체크.

	if( bFirst )
	{
		// 시작 시간 저장..
		m_nCheckTime		= GetTickCount();
	}

	if( pMapInfo->pSegment->uCheckTime == m_nCheckTime )
		return TRUE;

	pMapInfo->pSegment->uCheckTime = m_nCheckTime;

	// 검사한 플래그를 남김.
	// 주위 9개의 정보를 얻어냄.
	// 자신의 텍스쳐를 결정함.
	// 9개에 각각 리컬시브 콜을 넘김.
	// 

	stFindMapInfo pFindMapSelf[ 9 ];
	int i;
	for (i = 0; i < 9; ++i )
	{
		if( i == 4 ) continue;

		posx2 = pMapInfo->nSegmentX + offsetX[i];
		posy2 = pMapInfo->nSegmentZ + offsetY[i];

		GetWorldSectorInfoOnly( &pFindMapSelf[ i ] , pMapInfo->pSector , posx2 , posy2 );
	}

	if( bFirst )
	{
		// masktile을 알아낸다.

		for (i = 0; i < 9; ++i )
		{
			if( i == 4 ) continue;

			if( pFindMapSelf[ i ].pSector )
			{
				if( IsUsedThisTile( pFindMapSelf[ i ].pSegment , basetile ) )
				{
					// ...
				}
				else
				{
					// 이거다!..
					maskedindex	= pFindMapSelf[ i ].pSegment->pIndex[ TD_FIRST ];
					break;
				}
			}
		}

		if( i == 9 )
		{
			// 모두 같은것이다.
			return FALSE;
		}
	}
	
	// Base Tile , MaskTile Update;
	basetile	= _GetRandTileOffset( basetile		);
	maskedindex	= _GetRandTileOffset( maskedindex	);

	bool check[ 9 ];
	for ( i = 0; i < 9; ++i )
	{
		if( i == 4 ) continue;

		if (	pFindMapSelf[ i ].pSector 	)
		{
			if( IsUsedThisTile( pFindMapSelf[ i ].pSegment , basetile ) )
			{
				check[ i ] = true;
			}
			else
				check[ i ] = false;

			// 리컬시브..
		}
		else
		{
			check[ i ] = true;
		}
	}

	int index;
	index =	( check[8] << 7 ) +
			( check[7] << 6 ) +
			( check[6] << 5 ) +
			( check[5] << 4 ) +
			( check[3] << 3 ) +
			( check[2] << 2 ) +
			( check[1] << 1 ) +
			( check[0] << 0 ) ;

	if( amask[ index ] == 0x0f )
	{
		// 바꾸자!
		// 전부다 차있는거니까. 

		pMapInfo->pSector->D_SetTile(
			pMapInfo->pSector->GetCurrentDetail() ,
			pMapInfo->nSegmentX , pMapInfo->nSegmentZ ,
			basetile
			, 
			ALEF_TEXTURE_NO_TEXTURE		
			,
			ALEF_TEXTURE_NO_TEXTURE
			,
			pMapInfo->pSegment->pIndex[ TD_FOURTH ] );
	}
	else
	{
		int secondtexture = UPDATE_TEXTURE_OFFSET( alphaindex , amask[ index ] );
		// 변경..
		pMapInfo->pSector->D_SetTile( pMapInfo->pSector->GetCurrentDetail() ,
			pMapInfo->nSegmentX , pMapInfo->nSegmentZ ,
			maskedindex
			, 
			secondtexture		
			,
			basetile
			,
			pMapInfo->pSegment->pIndex[ TD_FOURTH ] );

	}

	// 주위 정보 변경..
	for ( i = 0; i < 9; ++i )
	{
		if( i == 4 ) continue;

		if ( pFindMapSelf[ i ].pSector	)
		{
			if( IsUsedThisTile( pFindMapSelf[ i ].pSegment , basetile ) )
			{
				if( bReculsive ) 
					ApplyAlpha( & pFindMapSelf[ i ] , basetile , maskedindex , alphaindex , bReculsive , FALSE );
				else if( bFirst )
					ApplyAlpha( & pFindMapSelf[ i ] , basetile , maskedindex , alphaindex , bReculsive , FALSE );

			}
		}
	}

	return TRUE;
}

BOOL		AgcmMap::ApplyAlphaToSector	( ApWorldSector * pSector , int alphatexture , int nDetail  )
{
	if( NULL == pSector ) return FALSE;

	if( nDetail == -1 )
	{
		nDetail = pSector->GetCurrentDetail();
	}

	int					x , z		;
	stFindMapInfo		FindMapInfo	;

	INT32				lDepth = pSector->D_GetDepth( nDetail );

	for( z = 1 ; z < lDepth ; z += 2 )
	{
		for( x = 1 ; x < lDepth ; x += 2 )
		{
			if( GetWorldSectorInfoOnly( &FindMapInfo , pSector , x , z ) )
			{
				ApplyAlpha(
					&FindMapInfo									,
					FindMapInfo.pSegment->pIndex[ TD_FIRST ]	,
					0												,
					alphatexture									,
					FALSE											);
			}

		}
	}

	return TRUE;
}

void			AgcmMap::DeleteMyCharacterCheck()
{
	m_bFirstPosition = TRUE;

	m_csMapLoader.EmptyQueue();
}

BOOL AgcmMap::DumpTexDict()
{
	if (!m_TextureList.m_pTexDict)
		return FALSE;

	// Texture Dictionary가 있으면, StreamWrite한다. Platform Independent Data로
	if (m_TextureList.m_pTexDict && m_TextureList.m_strTextureDictionary[0])
	{
		RwStream *		stream;

		stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMWRITE, m_TextureList.m_strTextureDictionary );
		if( stream )
		{
			RtPITexDictionaryStreamWrite( m_TextureList.m_pTexDict , stream );
   
			RwStreamClose(stream, NULL);
		}
		else
			return FALSE;
	}

	return TRUE;
}

RpClump *	AgcmMap::GetCursorClump	( RwCamera * pCamera , RwV2d * pScreenPos , INT32 nOption )
{
	RpIntersectionCallBackAtomic	pCallback;

	// 마고자 (2004-03-18 오후 2:54:48) : 피킹 온리일 경우,, 피킹 콜백 사용함..
	switch( nOption )
	{
	case	PICKINGOBJECTCLUMP				:	
		g_nGetCursorClumpType	= ACUOBJECT_TYPE_OBJECT	;
		pCallback				= _GetClumpCallback		;
		break;
	case	PICKINGOBJECTCOLLISIONATOMIC	:	
		g_nGetCursorClumpType	= ACUOBJECT_TYPE_OBJECT	;
		pCallback	= _GetClumpPickingOnlyCallback	;
		break;
	case	PICKINGCHARACTER				:
		// 케릭터 검사.
		g_nGetCursorClumpType	= ACUOBJECT_TYPE_CHARACTER	;
		pCallback	= _GetClumpPickingOnlyCallback	;
		break;
	default:
		return NULL;
	}

	AuList< _FindClumpStruct >	listClumpSorted;

	RpClump		*	pClump			;

	// 현재 커서 위치에 있는 클럼프를 구해낸다.
	RpIntersection	intersection	;
	RwLine			pixelRay		;
	BOOL			bMultiClump		= FALSE;

	ASSERT( NULL != m_pWorld	);
	ASSERT( NULL != pCamera		);
	ASSERT( NULL != pScreenPos	);

	if( NULL == m_pWorld || NULL == pCamera || NULL == pScreenPos )
		return NULL;

	RwCameraCalcPixelRay( pCamera , &pixelRay , pScreenPos );

	intersection.type	=	rpINTERSECTLINE	;
	intersection.t.line	=	pixelRay		;

	AuList< _FindClumpStruct >	listClump;

	if(!m_pcsApmOcTree || !m_pcsApmOcTree->m_bOcTreeEnable)
	{
		if(m_bUseCullMode)
		{
			LoadingSectorsForAllAtomicsIntersection(4,&intersection,
			pCallback					,
			( void * ) &listClump		);
		}
		else
		{
			m_pcsAgcmRender->RenderWorldForAllIntersections( 
			&intersection				,
			pCallback					,
			( void * ) &listClump		);
		}
	}
	else
	{
		m_pcsAgcmOcTree->OcTreesForAllAtomicsIntersection(4,&intersection,
		pCallback					,
		( void * ) &listClump		);
	}
	
	switch( listClump.GetCount() )
	{
	case 0:
		pClump = NULL;
		break;
	case 1:
		pClump = listClump.GetHeadNode()->GetData().pClump;
		break;
	default:
		// 메뉴띄움..
		pClump		= NULL;
		bMultiClump	= TRUE;
		break;
	}

	if( bMultiClump )
	{
		FLOAT	fNearClipPlane = RwCameraGetNearClipPlane( pCamera );
		FLOAT	fDistance		;
		// 디스턴스별 소팅
		AuNode< _FindClumpStruct >	* pNode , * pNodeClose;

		//fNearClipPlane	= 0.0f;

		listClumpSorted.RemoveAll();

		while( listClump.GetCount() )
		{
			pNode		= listClump.GetHeadNode()	;
			pNodeClose	= pNode						;

			while( pNode )
			{
				if( pNode->GetData().distance < pNodeClose->GetData().distance )
					pNodeClose = pNode;

				listClump.GetNext( pNode );
			}

			fDistance	= pNodeClose->GetData().distance;
			if( fDistance >=  fNearClipPlane )
				listClumpSorted.	AddTail		( pNodeClose->GetData()	);
			listClump.			RemoveNode	( pNodeClose			);
		}

		if( listClumpSorted.GetCount() > 0 )
		{
			pClump = listClumpSorted.GetHead().pClump;
		}
	}

	return pClump;
}

void	AgcmMap::RemoveHPInfo	()
{
	ApWorldSector * pSector;
	INT32	x2 = GetLoadRangeX2() < MAP_WORLD_INDEX_WIDTH - 1 ? ( GetLoadRangeX2() + 1 ) : GetLoadRangeX2();
	INT32	z2 = GetLoadRangeZ2() < MAP_WORLD_INDEX_WIDTH - 1 ? ( GetLoadRangeZ2() + 1 ) : GetLoadRangeZ2();

	for( int	x = ( GetLoadRangeX1() > 1 ? ( GetLoadRangeX1() - 1 ) : GetLoadRangeX1()	) ;
				x < x2;
				++x 																		)
	{
		for( int	z = ( GetLoadRangeZ1() > 1 ? ( GetLoadRangeZ1() - 1 ) : GetLoadRangeZ1()	);
					z < z2;
					++z 																		)
		{
			// 아싸
			pSector = m_pcsApmMap->GetSectorByArray( x , 0 , z );
			if( pSector )
			{
				pSector->FreeHeightPool();
			}
		}
	}	
	
}

BOOL	AgcmMap::SetSectorIndexInsert( int indexType , ApWorldSector * pSector , INT32 nObjectIndex , RwSphere * pSphere )
{
	ASSERT( NULL != pSphere			);

	if( NULL == pSector )
	{
		// 
		pSector = this->m_pcsApmMap->GetSector( pSphere->center.x , pSphere->center.z );

		if( NULL == pSector )
		{
			// -_-;;
			return FALSE;
		}
	}

	if( pSector->IsInSectorRadius( pSphere->center.x , pSphere->center.z , pSphere->radius ) )
	{
		int		segmentx , segmentz	;
		FLOAT	fSegPosX , fSegPosZ	;
		//FLOAT	fDistance			;
		
		FLOAT	aBoxObject	[ 4 ];	// 0 X1 , 1 X2 , 2 Z1 , 3 Z2;
		FLOAT	aBoxTile	[ 4 ];	// 0 X1 , 1 X2 , 2 Z1 , 3 Z2;

		// 박스..
		aBoxObject[ 0 ]	= pSphere->center.x - pSphere->radius;//AX1
		aBoxObject[ 1 ]	= pSphere->center.x + pSphere->radius;//AX2
		aBoxObject[ 2 ]	= pSphere->center.z - pSphere->radius;//AZ1
		aBoxObject[ 3 ]	= pSphere->center.z + pSphere->radius;//AZ2

		for( segmentz = 0 ; segmentz < MAP_DEFAULT_DEPTH ; ++segmentz  )
		{
			for( segmentx = 0 ; segmentx < MAP_DEFAULT_DEPTH ; ++segmentx  )
			{
				pSector->D_GetSegment( SECTOR_HIGHDETAIL , segmentx , segmentz , &fSegPosX , &fSegPosZ );

				aBoxTile[ 0 ]	= fSegPosX					; // BX1
				aBoxTile[ 1 ]	= fSegPosX + MAP_STEPSIZE	; // BX2
				aBoxTile[ 2 ]	= fSegPosZ					; // BZ1
				aBoxTile[ 3 ]	= fSegPosZ + MAP_STEPSIZE	; // BZ2

				// 마고자 (2004-04-09 오후 2:46:30) : 
				//충돌 사각형 검출
				//사각형 두개가 있을 때, 
				//
				// 1번째 사각형: AX1, AX2, AY1, AY2
				// 2번째 사각형: BX1, BX2, BY1, BY2
				//
				// 이런 경우에 서로 겹치는지 체크한다 
				//
				// if (AX2 > BX1 && BX2 > AX1)
				//  if (AY2 > BY1 && BY2 > AY1)
				//   return true;
				// else return false;

				if (aBoxObject[ 1 ] > aBoxTile[ 0 ] && aBoxTile[ 1 ] > aBoxObject[ 0 ])
				{
					if (aBoxObject[ 3 ] > aBoxTile[ 2 ] && aBoxTile[ 3 ] > aBoxObject[ 2 ])
					{
						// 충돌..
						// 인덱스 추가.
						pSector->AddIndex( indexType , segmentx , segmentz , nObjectIndex , TRUE );
					}
				}
			}
		}
	}

	return FALSE;
}

BOOL	AgcmMap::SetSectorIndexInsertBBox( int indexType , ApWorldSector * pSector , INT32 nObjectIndex , RwBBox * pBBox )
{
	if( NULL == pSector )
	{
		return FALSE;
	}

	BOOL	bUpdate = FALSE;
	if( pSector->IsInSectorRadiusBBox( pBBox->inf.x , pBBox->inf.z , pBBox->sup.x , pBBox->sup.z ) )
	{
		int		segmentx , segmentz	;
		FLOAT	fSegPosX , fSegPosZ	;
		//FLOAT	fDistance			;
		
		FLOAT	aBoxObject	[ 4 ];	// 0 X1 , 1 X2 , 2 Z1 , 3 Z2;
		FLOAT	aBoxTile	[ 4 ];	// 0 X1 , 1 X2 , 2 Z1 , 3 Z2;

		// 박스..
		aBoxObject[ 0 ]	= pBBox->inf.x < pBBox->sup.x ? pBBox->inf.x : pBBox->sup.x;//AX1
		aBoxObject[ 1 ]	= pBBox->inf.x < pBBox->sup.x ? pBBox->sup.x : pBBox->inf.x;//AX2
		aBoxObject[ 2 ]	= pBBox->inf.z < pBBox->sup.z ? pBBox->inf.z : pBBox->sup.z;//AZ1
		aBoxObject[ 3 ]	= pBBox->inf.z < pBBox->sup.z ? pBBox->sup.z : pBBox->inf.z;//AZ2

		for( segmentz = 0 ; segmentz < MAP_DEFAULT_DEPTH ; ++segmentz  )
		{
			for( segmentx = 0 ; segmentx < MAP_DEFAULT_DEPTH ; ++segmentx  )
			{
				pSector->D_GetSegment( SECTOR_HIGHDETAIL , segmentx , segmentz , &fSegPosX , &fSegPosZ );

				aBoxTile[ 0 ]	= fSegPosX					; // BX1
				aBoxTile[ 1 ]	= fSegPosX + MAP_STEPSIZE	; // BX2
				aBoxTile[ 2 ]	= fSegPosZ					; // BZ1
				aBoxTile[ 3 ]	= fSegPosZ + MAP_STEPSIZE	; // BZ2

				// 마고자 (2004-04-09 오후 2:46:30) : 
				//충돌 사각형 검출
				//사각형 두개가 있을 때, 
				//
				// 1번째 사각형: AX1, AX2, AY1, AY2
				// 2번째 사각형: BX1, BX2, BY1, BY2
				//
				// 이런 경우에 서로 겹치는지 체크한다 
				//
				// if (AX2 > BX1 && BX2 > AX1)
				//  if (AY2 > BY1 && BY2 > AY1)
				//   return true;
				// else return false;

				if (aBoxObject[ 1 ] > aBoxTile[ 0 ] && aBoxTile[ 1 ] > aBoxObject[ 0 ])
				{
					if (aBoxObject[ 3 ] > aBoxTile[ 2 ] && aBoxTile[ 3 ] > aBoxObject[ 2 ])
					{
						// 충돌..
						// 인덱스 추가.
						pSector->AddIndex( indexType , segmentx , segmentz , nObjectIndex , TRUE );
						bUpdate = TRUE;
					}
				}
			}
		}
	}

	return bUpdate;
}

struct	IndexPattern
{
	INT32	nIndex[ TD_DEPTH ];
};

BOOL	AgcmMap::CreateImAtomic				( MagImAtomic * pAtomic , ApWorldSector * pSector )
{
	// 아토믹 생성...
	// 우선 읽혀있는 지형에 따라서 
	// 체크한다..

	// 우선 텍스쳐부터 생성..
	
	enum
	{
		TEXTURE_NONE	=	0,
		TEXTURE_64		=	1,
		TEXTURE_128		=	2,
		TEXTURE_256		=	4,
		TEXTURE_512		=	8,
		TEXTURE_1024	=	16
	};

	// StartTime Log..
	UINT	uTime	= GetTickCount();

	if( NULL == pSector ) return TRUE;

	TRACE( "CGeometryPatternTexture %d,%d 섹터 처리중\n" , pSector->GetIndexX() , pSector->GetIndexZ() );
	
	AuList< IndexPattern >	list;
	AuNode< IndexPattern >	* pNode;
	IndexPattern			pattern;
	IndexPattern			* pPattern;

	int x , z;
	int	i;

	ApDetailSegment	*pSegment;
	for( z = 0 ; z < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; z ++ )
	{
		for( x = 0 ; x < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; x ++ )
		{
			pSegment	= pSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z );
			if( NULL != pSegment )
			{
				// 냐하하;;;

				// 페턴검사..
				

				// 이미 있는 페턴인지 검사..
				pNode = list.GetHeadNode();
				while( pNode )
				{
					pPattern	= &pNode->GetData();

					for( i = 0 ; i < TD_DEPTH ; i ++ )
					{
						if( pPattern->nIndex[ i ] != (INT32) pSegment->pIndex[ i ] )
						{
							break;
						}
					}

					if( i == TD_DEPTH )
					{
						// 같다!..
						break;
					}

					list.GetNext( pNode );
				}

				// 같은것이 있다면?...
				if( pNode )
				{
					// do no op..
				}
				else
				{
					// 없으면 추가함..
					
					for( i = 0 ; i < TD_DEPTH ; i ++ )
					{
						pattern.nIndex[ i ] = pSegment->pIndex[ i ];
					}

					list.AddTail( pattern );
				}
				
			}
		}
	}

	// 어레이 생성..

	IndexPattern	* pArrayPattern = NULL;
	VERIFY( pArrayPattern	= new IndexPattern[ list.GetCount() ] );

	int	count = 0;
	pNode = list.GetHeadNode();
	while( pNode )
	{
		pPattern	= &pNode->GetData();
		pArrayPattern[ count ++ ]	= * pPattern;
		list.GetNext( pNode );

		TRACE("CGeometryPatternTexture:: %d,%d,%d,%d\n"			, 
			pPattern->nIndex[ 0 ] , pPattern->nIndex[ 1 ]	,
			pPattern->nIndex[ 2 ] , pPattern->nIndex[ 3 ]	);
	}
	
	int	nType;
	// 크기 결정..
	if( count <= 256	)	nType	=	TEXTURE_1024	;
	if( count <= 64		)	nType	=	TEXTURE_512		;
	if( count <= 16		)	nType	=	TEXTURE_256		;
	if( count <= 4		)	nType	=	TEXTURE_128		;
	if( count <= 1		)	nType	=	TEXTURE_64		;
	if( count <= 0		)	nType	=	TEXTURE_NONE	;
	
	ASSERT( count <= 256 );

	// 이미지 생성...
	RwRaster * pRaster_Image	= RwRasterCreate( 64 * nType , 64 * nType , 32 , rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888 );
	RwRaster * pRasterTile		= RwRasterCreate( 64 , 64 , 32 , rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888 );

	if( NULL == pRaster_Image || NULL == pRasterTile )
	{
		TRACE( "%d 타입!\n" , nType );
		return FALSE;
	}

	AuRwImage	Layer1	;
	AuRwImage	Layer2	;
	AuRwImage	Layer3	;
	AuRwImage	Alpha	;
	RwRect		rect	;

	RwRasterPushContext( pRaster_Image );

	RwRaster	* pSubRaster;

	#define GetLeft( index )	( ( ( index ) % ( nType ) ) * 64 )
	#define GetTop( index )		( ( ( index ) / ( nType ) ) * 64 )
	#define GetRight( index )	( ( ( index ) % ( nType ) ) * 64 + 64 )
	#define GetBottom( index )	( ( ( index ) / ( nType ) ) * 64 + 64 )

	#define GetUVLeft( index )		( FLOAT )( ( ( index ) % ( nType ) ) / ( FLOAT ) nType  )
	#define GetUVTop( index )		( FLOAT )( ( ( index ) / ( nType ) ) / ( FLOAT ) nType  )
	#define GetUVRight( index )		( FLOAT )( ( ( index ) % ( nType ) + 1 ) / ( FLOAT ) nType  )
	#define GetUVBottom( index )	( FLOAT )( ( ( index ) / ( nType ) + 1 ) / ( FLOAT ) nType  )

	for( i = 0 ; i < count ; ++ i )
	{
		//RwRasterSubRaster( pSubRaster_RenderTarget , pRaster_Image , &rect );
		TRACE("CGeometryPatternTexture:: 루프(%d/%d)!\n" , i , count );
	
		// 각 페턴별로..
		Layer1.Set( m_TextureList.LoadImage( pArrayPattern[ i ].nIndex[ TD_FIRST ] ) );

		rect.x	= (INT32) ( ( FLOAT ) Layer1.GetWidth()		* GET_TEXTURE_U_START	( pArrayPattern[ i ].nIndex[ TD_FIRST ] ) );
		rect.y	= (INT32) ( ( FLOAT ) Layer1.GetHeight()	* GET_TEXTURE_V_START	( pArrayPattern[ i ].nIndex[ TD_FIRST ] ) );
		rect.w	= (INT32) ( ( FLOAT ) Layer1.GetWidth()		* GET_TEXTURE_U_END		( pArrayPattern[ i ].nIndex[ TD_FIRST ] ) ) - rect.x	;
		rect.h	= (INT32) ( ( FLOAT ) Layer1.GetHeight()	* GET_TEXTURE_V_END		( pArrayPattern[ i ].nIndex[ TD_FIRST ] ) ) - rect.y	;

		Layer1.CheckOutRaster();

		RwRasterPushContext( pRasterTile );

		pSubRaster	= Layer1.GetSubRaster( rect.x , rect.y , rect.w , rect.h );

		VERIFY( RwRasterRender( pSubRaster , 0 , 0 ) );

		// 알파 이미지 출력

		if( pArrayPattern[ i ].nIndex[ TD_SECOND	] != ALEF_TEXTURE_NO_TEXTURE	&&
			pArrayPattern[ i ].nIndex[ TD_THIRD	] != ALEF_TEXTURE_NO_TEXTURE	)
		{
			AuRwImage	subImageAlpha , subImageLayer2;

			// 알파이미지 생성..
			BOOL	bRet ;
			bRet = Alpha.Set( m_TextureList.LoadImage( pArrayPattern[ i ].nIndex[ TD_SECOND ] ) );

			rect.x	= (INT32) ( ( FLOAT ) Alpha.GetWidth()	* GET_TEXTURE_U_START	( pArrayPattern[ i ].nIndex[ TD_SECOND ] ) );
			rect.y	= (INT32) ( ( FLOAT ) Alpha.GetHeight()	* GET_TEXTURE_V_START	( pArrayPattern[ i ].nIndex[ TD_SECOND ] ) );
			rect.w	= (INT32) ( ( FLOAT ) Alpha.GetWidth()	* GET_TEXTURE_U_END		( pArrayPattern[ i ].nIndex[ TD_SECOND ] ) ) - rect.x	;
			rect.h	= (INT32) ( ( FLOAT ) Alpha.GetHeight()	* GET_TEXTURE_V_END		( pArrayPattern[ i ].nIndex[ TD_SECOND ] ) ) - rect.y	;

			Alpha.CreateSubImage( & subImageAlpha , rect.x , rect.y , rect.w , rect.h );

			bRet = Layer2.Set( m_TextureList.LoadImage( pArrayPattern[ i ].nIndex[ TD_THIRD ] ) );

			rect.x	= (INT32) ( ( FLOAT ) Layer2.GetWidth()		* GET_TEXTURE_U_START	( pArrayPattern[ i ].nIndex[ TD_THIRD ] ) );
			rect.y	= (INT32) ( ( FLOAT ) Layer2.GetHeight()	* GET_TEXTURE_V_START	( pArrayPattern[ i ].nIndex[ TD_THIRD ] ) );
			rect.w	= (INT32) ( ( FLOAT ) Layer2.GetWidth()		* GET_TEXTURE_U_END		( pArrayPattern[ i ].nIndex[ TD_THIRD ] ) ) - rect.x	;
			rect.h	= (INT32) ( ( FLOAT ) Layer2.GetHeight()	* GET_TEXTURE_V_END		( pArrayPattern[ i ].nIndex[ TD_THIRD ] ) ) - rect.y	;

			Layer2.CreateSubImage( &subImageLayer2 , rect.x , rect.y , rect.w , rect.h );
			
			subImageAlpha.MakeMask();
			subImageLayer2.ApplyMask( subImageAlpha.GetImagePointer() );

			subImageLayer2.CheckOutRaster();
			RwRasterRender( subImageLayer2.GetRaster() , 0 , 0 );
			subImageLayer2.CheckInRaster();

		}

		if( pArrayPattern[ i ].nIndex[ TD_FOURTH	] != ALEF_TEXTURE_NO_TEXTURE	)
		{
			Layer3.Set( m_TextureList.LoadImage( pArrayPattern[ i ].nIndex[ TD_FOURTH ] ) );

			rect.x	= (INT32) ( ( FLOAT ) Layer3.GetWidth()		* GET_TEXTURE_U_START	( pArrayPattern[ i ].nIndex[ TD_FOURTH ] ) );
			rect.y	= (INT32) ( ( FLOAT ) Layer3.GetHeight()	* GET_TEXTURE_V_START	( pArrayPattern[ i ].nIndex[ TD_FOURTH ] ) );
			rect.w	= (INT32) ( ( FLOAT ) Layer3.GetWidth()		* GET_TEXTURE_U_END		( pArrayPattern[ i ].nIndex[ TD_FOURTH ] ) ) - rect.x	;
			rect.h	= (INT32) ( ( FLOAT ) Layer3.GetHeight()	* GET_TEXTURE_V_END		( pArrayPattern[ i ].nIndex[ TD_FOURTH ] ) ) - rect.y	;

			Layer3.CheckOutRaster();
			RwRasterRender( Layer3.GetRaster() , 0 , 0 );
			Layer3.CheckInRaster();

		}

		RwRasterPopContext();
		RwRasterRender( pRasterTile , GetLeft( i ) , GetTop( i ) );

		RwRasterDestroy( pSubRaster );
		Layer1.CheckInRaster();
	}

	RwRasterPopContext();

	// 이미지에 카피..
	RwTexture * pAssembledTexture	= RwTextureCreate( pRaster_Image );
	ASSERT( NULL != pAssembledTexture );

	// 마고자 (2003-08-20 오후 3:13:50) : Texture가 Destroy 됄때 같이 없어지므로.
	//										따로 처리하지 않아도 됀다.

	//if( pRaster_Image	) RwRasterDestroy( pRaster_Image	);
	if( pRasterTile		) RwRasterDestroy( pRasterTile		);

	// 텍스쳐 만들기 끝..
	TRACE( "텍스쳐 만들때 걸린 시간 %d ms\n" , GetTickCount() - uTime );
	uTime = GetTickCount();

	// 폴리건 만들기..
	{
		// 섹터에 데이타 넣기.
		ASSERT( NULL != m_pcsApmMap										);
		ASSERT( NULL != pSector											);

		int nDetail = SECTOR_HIGHDETAIL;
		ASSERT( SECTOR_EMPTY <= nDetail && nDetail < SECTOR_DETAILDEPTH	);

		ApDetailSectorInfo	* pToolSectorInfo	= pSector->GetDetailSectorInfo()		;
		if( pToolSectorInfo == NULL )
		{
			ASSERT( !"툴용 지형 저보가 없습니다.\n" );
			return NULL;
		}

		int					nDepth			= pToolSectorInfo->m_nDepth			[ nDetail ];
		ApDetailSegment *	pDetailSegment	= pToolSectorInfo->m_pSegment		[ nDetail ];
		AcMaterialList *	pMaterialList	= GetMaterialList( pSector );

		if( NULL == pDetailSegment ) return FALSE;

		FLOAT	xStart	= pSector->GetXStart();
		FLOAT	xEnd	= pSector->GetXEnd	();
		FLOAT	zStart	= pSector->GetZStart();
		FLOAT	zEnd	= pSector->GetZEnd	();

		float	pSecStepSizeX = pSector->GetStepSizeX( nDetail );
		float	pSecStepSizeZ = pSector->GetStepSizeZ( nDetail );

		ASSERT( nDepth != 0 );
		{
			RwInt32					i, j, k		;

			// 데이타 준비.
			int	MAP_WIDTH	= nDepth;
			int	MAP_HEIGHT	= nDepth;

			int	MAP_SIZE	= ( ( MAP_WIDTH + 1	) * ( MAP_HEIGHT + 1	)		)	;
			int	NOT			= ( ( MAP_WIDTH		) * ( MAP_HEIGHT		) * 2	)	;	// Number of Triangles
			int	NOV			= NOT * 3												;


			// 잠시.. 기본지형에 대한 정보 얻어냄.
			NOT = 0;
			NOV = 0;

			ApDetailSegment * pSegment;

			for( j = 0 ; j < MAP_HEIGHT ; ++j )
			{
				for( int i = 0 ; i < MAP_WIDTH ; ++i )
				{
					// 각섹터 검사..
					pSegment = pSector->D_GetSegment(  nDetail , i , j );
					if( pSegment )
					{
						// 섹터가 존재하고 디테일 섹터가 아니면!.
						NOT += 2;
						NOV += 6;
					}
				}
			}

			RwV3d * VertexList;
			VertexList		= new RwV3d		[ MAP_SIZE ];
			RwRGBA	* pPreLightList;
			pPreLightList	= new RwRGBA	[ MAP_SIZE ];

			int	total_nov = NOV;
			int	total_not = NOT;

			VERIFY( pAtomic->CreateAtomic( total_nov , total_not * 3 ) );

			RwIm3DVertex			*pVertex	= pAtomic->GetVertexBuffer	();
			RwImVertexIndex			*pIndex		= pAtomic->GetIndexBuffer	();
			int	nCount = 0;
			
			memset( pVertex	, 0 , sizeof(	RwIm3DVertex	) * total_nov		);
			memset( pIndex	, 0 , sizeof(	RwImVertexIndex	) * total_not * 3	);
			
			/* 
			* Define vertices and triangles for pentagons... 
			*/

			for ( i = 0 ; i < MAP_HEIGHT + 1 ; ++i )
			{
				for (j = 0 ; j < MAP_WIDTH + 1 ; ++j )
				{
					pSegment			= pSector->D_GetSegment(  nDetail , j , i )			;

					MAP( j , i ).x		= xStart + ( j ) * pSecStepSizeX	;
					MAP( j , i ).z		= zStart + ( i ) * pSecStepSizeZ	;
					if( pSegment )
					{
						MAP( j , i ).y		= pSegment->height;
						_RwRGBAAssigApRGB( PRELIGHT( j , i ) , pSegment->vertexcolor );
					}
					else
					{
						//ASSERT( !"MakeDetail , 이러면 안돼는데" );
						ApRGBA rgba;
						rgba.alpha = 255 ; rgba.red = 255 ; rgba.green = 255 ; rgba.blue = 255 ;
						MAP( j , i ).y		= ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
						_RwRGBAAssigApRGB( PRELIGHT( j , i ) , rgba );
					}
				}
			}

			int	polygoncount = 0;
			// k는 vertex index

			ApDetailSegment	* pWorkingSegment;
			int nPattenIndex;
			for (i = 0, k = 0; i < MAP_HEIGHT ; ++i)
			{
				for (j = 0; j < MAP_WIDTH ; ++j ) // 폴리건 두개씩 처리함.
				{
					RwV3d normal = {0.0f, 1.0f, 0.0f};
					
					// 왼쪽 위의것.

					pWorkingSegment = pSector->D_GetSegment(  nDetail , j , i );

					{
						// nPattenIndex 를 얻어내는 부분.
						pNode = list.GetHeadNode();
						nPattenIndex	= 0;
						while( pNode )
						{
							pPattern	= &pNode->GetData();
							int			l;

							for( l = 0 ; l < TD_DEPTH ; l ++ )
							{
								if( pPattern->nIndex[ l ] != ( INT32 ) pWorkingSegment->pIndex[ l ] )
								{
									break;
								}
							}

							if( l == TD_DEPTH )
							{
								// 같다!..
								break;
							}

							++ nPattenIndex;
							list.GetNext( pNode );
						}

						ASSERT( NULL != pNode );					
					}

					RwV3d	vPos	;
					RwRGBA	vRgb	;
					
					vPos	= MAP		( j	, i	);
					vRgb	= PRELIGHT	( j , i );
					
					RwIm3DVertexSetPos	( pVertex ,	vPos.x , vPos.y , vPos.z );
					RwIm3DVertexSetRGBA	( pVertex , vRgb.red , vRgb.green , vRgb.blue , vRgb.alpha );
					RwIm3DVertexSetNormal( pVertex , normal.x , normal.y , normal.z );

					RwIm3DVertexSetU	( pVertex ,	GetUVLeft	( nPattenIndex	)	);    
					RwIm3DVertexSetV	( pVertex ,	GetUVTop	( nPattenIndex	)	);

					++pVertex	;
					++nCount	;
					
					vPos	= MAP		( j	+ 1 , i	);
					vRgb	= PRELIGHT	( j + 1 , i );
					
					RwIm3DVertexSetPos	( pVertex ,	vPos.x , vPos.y , vPos.z );
					RwIm3DVertexSetRGBA	( pVertex , vRgb.red , vRgb.green , vRgb.blue , vRgb.alpha );
					RwIm3DVertexSetNormal( pVertex , normal.x , normal.y , normal.z );

					RwIm3DVertexSetU	( pVertex ,	GetUVRight( nPattenIndex	)	);    
					RwIm3DVertexSetV	( pVertex ,	GetUVTop	( nPattenIndex	)	);

					++pVertex	;
					++nCount	;

					vPos	= MAP		( j	, i	+ 1 );
					vRgb	= PRELIGHT	( j , i + 1 );
					
					RwIm3DVertexSetPos	( pVertex ,	vPos.x , vPos.y , vPos.z );
					RwIm3DVertexSetRGBA	( pVertex , vRgb.red , vRgb.green , vRgb.blue , vRgb.alpha );
					RwIm3DVertexSetNormal( pVertex , normal.x , normal.y , normal.z );

					RwIm3DVertexSetU	( pVertex ,	GetUVLeft	( nPattenIndex	)	);    
					RwIm3DVertexSetV	( pVertex ,	GetUVBottom	( nPattenIndex	)	);

					++pVertex	;
					++nCount	;

					// 오른쪽 아래것.

					vPos	= MAP		( j	+ 1 , i	);
					vRgb	= PRELIGHT	( j + 1 , i );

					RwIm3DVertexSetPos	( pVertex ,	vPos.x , vPos.y , vPos.z );
					RwIm3DVertexSetRGBA	( pVertex , vRgb.red , vRgb.green , vRgb.blue , vRgb.alpha );
					RwIm3DVertexSetNormal( pVertex , normal.x , normal.y , normal.z );

					RwIm3DVertexSetU	( pVertex ,	GetUVRight( nPattenIndex	)	);    
					RwIm3DVertexSetV	( pVertex ,	GetUVTop	( nPattenIndex	)	);

					++pVertex	;
					++nCount	;
					
					vPos	= MAP		( j	+ 1 , i	+ 1 );
					vRgb	= PRELIGHT	( j + 1 , i + 1 );
					
					RwIm3DVertexSetPos	( pVertex ,	vPos.x , vPos.y , vPos.z );
					RwIm3DVertexSetRGBA	( pVertex , vRgb.red , vRgb.green , vRgb.blue , vRgb.alpha );
					RwIm3DVertexSetNormal( pVertex , normal.x , normal.y , normal.z );

					RwIm3DVertexSetU	( pVertex ,	GetUVRight( nPattenIndex	)	);    
					RwIm3DVertexSetV	( pVertex ,	GetUVBottom	( nPattenIndex	)	);

					++pVertex	;
					++nCount	;

					vPos	= MAP		( j	, i	+ 1 );
					vRgb	= PRELIGHT	( j , i + 1 );
					
					RwIm3DVertexSetPos	( pVertex ,	vPos.x , vPos.y , vPos.z );
					RwIm3DVertexSetRGBA	( pVertex , vRgb.red , vRgb.green , vRgb.blue , vRgb.alpha );
					RwIm3DVertexSetNormal( pVertex , normal.x , normal.y , normal.z );

					RwIm3DVertexSetU	( pVertex ,	GetUVLeft	( nPattenIndex	)	);    
					RwIm3DVertexSetV	( pVertex ,	GetUVBottom	( nPattenIndex	)	);

					++pVertex	;
					++nCount	;

					// 삼각형 설정.
					*pIndex++	= k + 2		;
					*pIndex++	= k + 1		;
					*pIndex++	= k			;
					
					*pIndex++	= k + 5		;
					*pIndex++	= k + 4		;
					*pIndex++	= k + 3		;

					polygoncount += 2;
					k += 6;

				}
			}

			if( VertexList			) delete [] VertexList			;
			if( pPreLightList		) delete [] pPreLightList		;
		}

		pAtomic->SetTexture( pAssembledTexture );
	}
	
	// 다 끝난다음에 레퍼런스 카운트 다운시키고 , 안쓰는것은 날려버림둥.
	if( pArrayPattern		)
	{
		delete []  pArrayPattern;
		pArrayPattern = NULL;
	}

	if( pAssembledTexture	)
	{
		RwTextureDestroy( pAssembledTexture);
		pAssembledTexture = NULL;
	}

	// 폴리건 만들기 끝..
	TRACE( "폴리건 만들때 걸린 시간 %d ms\n" , GetTickCount() - uTime );
	uTime = GetTickCount();

	return TRUE;	
}

//◎◎◎◎◎◎◎◎◎◎◎◎◎ Intersection 함수 ◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎ add by gemani
void	AgcmMap::SectorForAllAtomicsIntersection( ApWorldSector* pWorldSector, RpIntersection* intersection, RpIntersectionCallBackAtomic callBack, void* data )
{
	if( intersection->type != rpINTERSECTLINE )		return;

	RwLine		line = intersection->t.line;
	RwReal		dist;

	SectorRenderList* pList = GetSectorData( pWorldSector );
	if( pList->terrain )
		if( RtLineSphereIntersectionTest( &line, RpAtomicGetBoundingSphere(pList->terrain), &dist) )
			callBack( intersection, NULL, pList->terrain, dist, data );

	for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
	{
		WorldSphere& cWorldSphere = (*Itr);
		if( RtLineSphereIntersectionTest( &line, &(cWorldSphere.cSphere), &dist ) )
			callBack( intersection, NULL, cWorldSphere.atomic, dist , data );
	}

	for( RpAtomicListItr Itr = pList->listInMove->begin(); Itr != pList->listInMove->end(); ++Itr )
	{
		RwSphere	bs2;
		m_pcsAgcmRender->CalcWorldBoundingSphere( &bs2, (*Itr) );
		if( RtLineSphereIntersectionTest( &line, &bs2, &dist ) )
			callBack( intersection, NULL, (*Itr), dist , data );
	}

	// World도 check하자 (character같은 경우는 world에 add)
	m_pcsAgcmRender->RenderWorldForAllIntersections( intersection, callBack, data );
}

void	AgcmMap::LoadingSectorsForAllAtomicsIntersection( INT32 ID, RpIntersection*	intersection, RpIntersectionCallBackAtomic callBack, void* data, BOOL bLock )
{
	if(intersection->type != rpINTERSECTLINE)		return;

	++m_ulCurTick;

	RwLine		line = intersection->t.line;
	RwReal		dist;

	IntersectionSectors* list = LineGetIntersectionSectors( &line );
	while(list)
	{
		SectorRenderList* pList = GetSectorData(list->pSector);
		
		if( pList->terrain )
		{
			ASSERT(pList->terrain->object.object.type == 1);

			if( RtLineSphereIntersectionTest( &line, RpAtomicGetBoundingSphere( pList->terrain ), &dist ) )
				callBack( intersection, NULL, pList->terrain, dist, data );
		}

		for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
			if( (*Itr).atomic->stRenderInfo.intersectTick != (INT32)m_ulCurTick)// || ID == 4)		// 한번만 체크(GetCursorClump제외)
			{
				(*Itr).atomic->stRenderInfo.intersectTick = m_ulCurTick;
				if( RtLineSphereIntersectionTest( &line, &((*Itr).cSphere), &dist ) )
					callBack( intersection, NULL, (*Itr).atomic, dist , data );
			}

		for( RpAtomicListItr Itr = pList->listInMove->begin(); Itr != pList->listInMove->end(); ++Itr )
		{
			if( (*Itr)->stRenderInfo.intersectTick != (INT32)m_ulCurTick)//ID == 4)		// 한번만 체크(GetCursorClump제외)
			{
				(*Itr)->stRenderInfo.intersectTick = m_ulCurTick;

				RwSphere	bs2;
				m_pcsAgcmRender->CalcWorldBoundingSphere( &bs2, (*Itr) );
				if( RtLineSphereIntersectionTest( &line, &bs2, &dist ) )
					callBack( intersection, NULL, (*Itr), dist , data );
			}
		}

		list = list->next;
	}
	
	// World도 check하자 (character같은 경우는 world에 add)
	m_pcsAgcmRender->RenderWorldForAllIntersections( intersection, callBack, data );
}

void	AgcmMap::LoadingSectorsForAllAtomicsIntersection2(INT32 ID, RpIntersection*	intersection,
											 	RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL	bLock )
{
	if( intersection->type != rpINTERSECTLINE )		return;
		

	++m_ulCurTick;

	RwLine		line = intersection->t.line;
	RwReal		dist;

	IntersectionSectors* list = LineGetIntersectionSectors(&line);
	while(list)
	{
		SectorRenderList* pList = GetSectorData(list->pSector);
		
		for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
			if( (*Itr).atomic->stRenderInfo.intersectTick != (INT32)m_ulCurTick)// || ID == 4)		// 한번만 체크(GetCursorClump제외)
			{
				(*Itr).atomic->stRenderInfo.intersectTick = m_ulCurTick;
				if( RtLineSphereIntersectionTest( &line, &(*Itr).cSphere , &dist ) )
					callBack( intersection, NULL, (*Itr).atomic, dist , data );
			}

		for( RpAtomicListItr Itr = pList->listInMove->begin(); Itr != pList->listInMove->end(); ++Itr )
		{
			if( (*Itr)->stRenderInfo.intersectTick != (INT32)m_ulCurTick)//ID == 4)		// 한번만 체크(GetCursorClump제외)
			{
				(*Itr)->stRenderInfo.intersectTick = m_ulCurTick;

				RwSphere	bs2;
				m_pcsAgcmRender->CalcWorldBoundingSphere( &bs2, (*Itr) );
				if( RtLineSphereIntersectionTest( &line, &bs2, &dist ) )
					callBack( intersection, NULL, (*Itr), dist , data );
			}
		}

		list = list->next;
	}
	
	// World도 check하자 (character같은 경우는 world에 add)
	m_pcsAgcmRender->RenderWorldForAllIntersections( intersection, callBack, data );
}

void	AgcmMap::LoadingSectorsForAllAtomicsIntersection3( INT32 ID, RpIntersection* intersection, RpIntersectionCallBackAtomic callBack, void* data, BOOL bLock )
{
	if( intersection->type != rpINTERSECTLINE )		return;

	++m_ulCurTick;

	RwLine		line = intersection->t.line;
	RwReal		dist;

	IntersectionSectors* list = LineGetIntersectionSectors(&line);
	while(list)
	{
		SectorRenderList* pList = GetSectorData(list->pSector);
		
		for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
		if( (*Itr).atomic->stRenderInfo.intersectTick != (INT32)m_ulCurTick)// || ID == 4)		// 한번만 체크(GetCursorClump제외)
		{
			(*Itr).atomic->stRenderInfo.intersectTick = m_ulCurTick;
			if( RtLineSphereIntersectionTest( &line, &(*Itr).cSphere , &dist ) )
				callBack( intersection, NULL, (*Itr).atomic, dist , data );
		}

		for( RpAtomicListItr Itr = pList->listInMove->begin(); Itr != pList->listInMove->end(); ++Itr )
		{
			if( (*Itr)->stRenderInfo.intersectTick != (INT32)m_ulCurTick)//ID == 4)		// 한번만 체크(GetCursorClump제외)
			{
				(*Itr)->stRenderInfo.intersectTick = m_ulCurTick;

				RwSphere	bs2;
				m_pcsAgcmRender->CalcWorldBoundingSphere( &bs2, (*Itr) );
				if( RtLineSphereIntersectionTest( &line, &bs2, &dist ) )
					callBack( intersection, NULL, (*Itr), dist , data );
			}
		}


		list = list->next;
	}
}

void	AgcmMap::LoadingSectorsForAllAtomicsIntersection4(INT32 ID, RpIntersection*	intersection,
											 	RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL	bLock )
{
	// World도 check하자 (character같은 경우는 world에 add)
	m_pcsAgcmRender->RenderWorldForAllIntersections( intersection, callBack, data );
}

BOOL	AgcmMap::LoadingSectorsForAllAtomicsIntersection5(RpIntersection*	intersection,	RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL	bLock )
{
	if(intersection->type != rpINTERSECTLINE)		// 현재는 Line만 체크
		return FALSE;

	RwLine		line = intersection->t.line;
	RwReal		dist;

	SectorRenderList*		pList		;
	RwSphere			*	pSphere		;

	IntersectionSectors*	list = LineGetIntersectionSectors(&line);

	while(list)
	{
		pList = GetSectorData(list->pSector);
		
		// 지형 체크
		if(pList->terrain)
		{
			ASSERT(pList->terrain->object.object.type == 1);

			pSphere = RpAtomicGetBoundingSphere( pList->terrain );

			if( RtLineSphereIntersectionTest ( &line , pSphere , &dist ) )// Intersection!!
			{
				callBack (intersection, NULL , pList->terrain , dist , data);
			}
		}

		list = list->next;
	}
	
	return TRUE;
}

IntersectionSectors*	AgcmMap::LineGetIntersectionSectors(RwLine*	line)
{
	int		xdiff,zdiff;
	int		xunit,zunit;

	int		x1 = SectorIndexToArrayIndexX(PosToSectorIndexX(line->start.x));
	int		x2 = SectorIndexToArrayIndexX(PosToSectorIndexX(line->end.x));

	int		z1 = SectorIndexToArrayIndexZ(PosToSectorIndexZ(line->start.z));
	int		z2 = SectorIndexToArrayIndexZ(PosToSectorIndexZ(line->end.z));

	int		x = x1,z = z1;

	if(x1 > x2)
	{
		xdiff = x1 - x2;
		xunit = -1;
	}
	else 
	{
		xdiff = x2 - x1;
		xunit = 1;
	}

	if(z1 > z2)
	{
		zdiff = z1 - z2;
		zunit = -1;
	}
	else 
	{
		zdiff = z2 - z1;
		zunit = 1;
	}

	int		error_term = 0;
	
	ApWorldSector*	pSector;
	IntersectionSectors*	result = NULL;
	IntersectionSectors*	new_node;
	
	if(xdiff>zdiff)
	{
		for(int i=0; i<= xdiff; ++i)
		{
			pSector = m_pcsApmMap->GetSectorByArray(x,0,z);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				// AllocFrameMemory 에러처리 추가.
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}

			pSector = m_pcsApmMap->GetSectorByArray(x,0,z-1);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				// AllocFrameMemory 에러처리 추가.
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}

			pSector = m_pcsApmMap->GetSectorByArray(x,0,z+1);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				// AllocFrameMemory 에러처리 추가.
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}
			
			x += xunit;
			error_term += zdiff;
			if(error_term > xdiff)
			{
				error_term -= xdiff;
				z += zunit;
			}
		}
	}
	else 
	{
		for(int i=0; i<= zdiff; ++i)
		{
			pSector = m_pcsApmMap->GetSectorByArray(x,0,z);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				// AllocFrameMemory 에러처리 추가.
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}

			pSector = m_pcsApmMap->GetSectorByArray(x-1,0,z);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				// AllocFrameMemory 에러처리 추가.
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}

			pSector = m_pcsApmMap->GetSectorByArray(x+1,0,z);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				// AllocFrameMemory 에러처리 추가.
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}
			
			z += zunit;
			error_term += xdiff;
			if(error_term > zdiff)
			{
				error_term -= zdiff;
				x += xunit;
			}
		}
	}

	return result;
}

void	AgcmMap::LoadingSectorsForAllClumps(	INT32				ID			, 
											 	RpClumpCallBack		pCallBack	,
												void *				data		,
												BOOL				bLock		)
{
	// 맵툴에서만 쓰는 듯 하다.
	// 시간 체크 기능을 제외 시킨다.
	//++m_ulCurTick;
	RpClump*	_CorruptCheckArray[ 65536 ];
	INT32		_nCorruptMax = 65536;
	INT32		_nCurrentClumpCount = 0;
	BOOL		bCorrupt;

	for( int nSector = 0; nSector < (int)m_pcsApmMap->GetCurrentLoadedSectorCount(); ++nSector )
	{
		SectorRenderList* pList = GetSectorData( m_pcsApmMap->GetCurrentLoadedSectors()[ nSector ] );

		for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
		{
			RpClump* pClump = RpAtomicGetClump( (*Itr).atomic );
			if( !pClump )	continue;

			bCorrupt	= FALSE;

			for( int i = 0 ; i < _nCurrentClumpCount ; ++i  )
			{
				if( _CorruptCheckArray[ i ] == pClump )
				{
					bCorrupt = TRUE;
					break;
				}
			}

			if( !bCorrupt && _nCurrentClumpCount < 65536 )
			{
				_CorruptCheckArray[ _nCurrentClumpCount++ ] = pClump;
				pCallBack( pClump , data );
			}
		}
	}
	
	RpWorldForAllClumps( m_pWorld, pCallBack, data );		// World도 check하자 (character같은 경우는 world에 add)
}

void	AgcmMap::LoadingSectorsForAllClumps_OCTREE(	INT32				ID			, 
											 	RpClumpCallBack		pCallBack	,
												void *				data		,
												BOOL				bLock		)
{
	//@{ 2006/11/22 burumal
	static INT32 nCorruptCheckArray[65536];
	//@}

	++m_ulCurTick;

	for( int nSector = 0 ; nSector < ( int ) m_pcsApmMap->GetCurrentLoadedSectorCount() ; ++ nSector )
	{
		m_pcsAgcmOcTree->OcTreeForAllClumps(
			m_pcsApmMap->GetCurrentLoadedSectors()[ nSector ]->GetArrayIndexX(),
			m_pcsApmMap->GetCurrentLoadedSectors()[ nSector ]->GetArrayIndexZ(),
			pCallBack,data, nCorruptCheckArray);
	}
	
	// World도 check하자 (character같은 경우는 world에 add)
	RpWorldForAllClumps( m_pWorld , pCallBack , data );
}

/*
void	AgcmMap::LoadingSectorsForAllAtomicsIntersection(INT32 ID, RpIntersection*	intersection,
											 	RpIntersectionCallBackAtomic    callBack,    void *    data )
{

	if(intersection->type != rpINTERSECTLINE)		// 현재는 Line만 체크
		return;

	if(!m_pSelfCharacterSector) return;

	RwLine		line = intersection->t.line;
	const RwSphere*	bsphere;
	RwReal		dist;

	int sx = m_pSelfCharacterSector->GetArrayIndexX() - 10;//m_nMapDataLoadRadius;
	int	sz = m_pSelfCharacterSector->GetArrayIndexZ() - 10;//m_nMapDataLoadRadius;
	int ex = m_pSelfCharacterSector->GetArrayIndexX() + 10;//m_nMapDataLoadRadius;
	int	ez = m_pSelfCharacterSector->GetArrayIndexZ() + 10;//m_nMapDataLoadRadius;

	ApWorldSector*	pSector;
	SectorRenderList*	pList;
	StaticAtomicInList*		cur_static;
	MovableAtomicInList*	cur_movable;

	for(int i=sx;i<ex;++i)
	{
		for(int j=sz;j<ez;++j)
		{
			pSector = m_pcsApmMap->GetSectorByArrayIndex(i,j);
			if(!pSector) continue;

			pList = GetSectorData(pSector);

			// 지형 체크
			if(pList->terrain)
			{
				RtLineSphereIntersectionTest ( &line, RpAtomicGetBoundingSphere(pList->terrain) , &dist);
				if(dist >= 0.0f && dist <= 1.0f)		// Intersection!!
				{
					callBack (intersection, NULL , pList->terrain , dist , data);
				}
			}

			cur_static = pList->object;
			while(cur_static)
			{
			//	if(cur_data->atomic->object.object.type != 1)
			//		{
			//			FILE* fp = fopen("tetest.txt","a+");
			//			fprintf(fp,"%d\n",cur_data->atomic->object.object.type);
			//			fclose(fp);
			//		}

				RtLineSphereIntersectionTest ( &line, &cur_static->WorldSphere , &dist);

				if(dist >= 0.0f && dist <= 1.0f)		// Intersection!!
				{
					callBack (intersection, NULL , cur_static->atomic , dist , data);
				}
	
				cur_static = cur_static->next;
			}

			cur_movable = pList->movable;
			while(cur_movable)
			{
				bsphere = RpAtomicGetWorldBoundingSphere(cur_movable->atomic);
				RtLineSphereIntersectionTest ( &line,(RwSphere*)bsphere, &dist);

				if(dist >= 0.0f && dist <= 1.0f)		// Intersection!!
				{
					callBack (intersection, NULL , cur_movable->atomic , dist , data);
				}
			
				cur_movable = cur_movable->next;
			}
		}
	}
}*/

BOOL	AgcmMap::_AgcmMapSectorDataConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmMap				* pCmMap				= ( AgcmMap *			) pClass;
	ApWorldSector		* pSector				= ( ApWorldSector *		) pData	;
	AgcmMapSectorData	* pClientSectorData		= pCmMap->GetAgcmAttachedData( pSector );

	// DWSector 초기화..
	memset( pClientSectorData , 0, sizeof(AgcmMapSectorData) )		;
	pClientSectorData->m_DWSector.curDetail = SECTOR_EMPTY			;
	pClientSectorData->m_DWSector.numDetail = DWSECTOR_MAX_DETAIL	;

	#ifdef USE_MFC
	pClientSectorData->m_bValidGrid		= FALSE;
	pClientSectorData->m_pLoadedData1	= ( void * ) new _MaptoolClientSaveData;	// Rough
	pClientSectorData->m_pLoadedData2	= ( void * ) new _MaptoolClientSaveData;	// Detail
	#endif
	
	return TRUE;
}

BOOL	AgcmMap::_AgcmMapSectorDataDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmMap				* pCmMap				= ( AgcmMap *			) pClass;
	ApWorldSector		* pSector				= ( ApWorldSector *		) pData	;
	AgcmMapSectorData	* pClientSectorData		= pCmMap->GetAgcmAttachedData( pSector );

	// 확인사살..;;
	for( int i = 0 ; i < pClientSectorData->m_DWSector.numDetail ; i ++ )
	{
		pCmMap->RpDWSectorDestroy( pClientSectorData->m_DWSector.details + i );
	}

	for( int i = 0 ; i < SECTOR_DETAILDEPTH ; ++ i )
	{
		pClientSectorData->m_MaterialList[ i ].RemoveAll();
	}

	#ifdef USE_MFC
	if( pClientSectorData->m_pLoadedData1 ) delete pClientSectorData->m_pLoadedData1;
	if( pClientSectorData->m_pLoadedData2 ) delete pClientSectorData->m_pLoadedData2;
	#endif

	return TRUE;	
}

BOOL	AgcmMap::CBMyCharacterPositionChange(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmMap::CBMyCharacterPositionChange");

	btid = GetCurrentThreadId();

	AgcmMap	*		pThis		= ( AgcmMap			* ) pClass		;
	ApWorldSector *	pPrevSector = ( ApWorldSector	* ) pData		;
	ApWorldSector *	pNextSector = ( ApWorldSector	* ) pCustData	;
	ApWorldSector *	pSector		;
	INT32			i , j		;
	INT32			nDistance	;

	const int DepthLimit = 10;
	
	// Loading Loop
	if( pNextSector )
	{
		// Load해야할 Sector의 개수를 Reset한다.
		pThis->m_lSectorsToLoad = 0;

		INT32	Next_Sector_IndexX = pNextSector->GetIndexX();
		INT32	Next_Sector_IndexZ = pNextSector->GetIndexZ();
		// 다음 지형이 있을경우 , 다음 지형기준으로 , 데이타가 준비돼어있는지 체크 하여
		// 없는 데이타를 로딩한다.

		// 맵데이타 체크..
		for( i = -pThis->GetMapDataLoadRadius() ; i <= pThis->GetMapDataLoadRadius() ; ++i )
		{
			for( j = -pThis->GetMapDataLoadRadius() ; j <= pThis->GetMapDataLoadRadius() ; ++j )
			{
				pSector = pThis->m_pcsApmMap->GetSector(
								Next_Sector_IndexX + i	,
								0						,
								Next_Sector_IndexZ + j	);

				if( NULL == pSector )
				{
					PROFILE("AgcmMap::CBMyCharacterPositionChange() - LoadSector");

					pSector = pThis->LoadSector( 
								Next_Sector_IndexX + i	,
								0						,
								Next_Sector_IndexZ + j	);

					TRACE("AgcmMap::CBMyCharacterPositionChange() Load Sector %x\n", pSector);
					// Callback으로 OnLoadSector 호출됨..
				}

				if( pSector )
				{
					ApAutoReaderLock	csLock( pSector->m_RWLock );

					nDistance	= GetSectorDistance( pSector , pNextSector );

					if( nDistance <= pThis->GetRoughLoadRadius() && !pThis->IsLoadedDetail( pSector , SECTOR_LOWDETAIL ) )
						++pThis->m_lSectorsToLoad;

					if( nDistance <= pThis->GetDetailLoadRadius() && !pThis->IsLoadedDetail( pSector , SECTOR_HIGHDETAIL ) )
						++pThis->m_lSectorsToLoad;
				}
			}
		}

		pThis->FlushLoadSector( pNextSector );
	}

	return TRUE;
}

void	AgcmMap::FlushLoadSector	( ApWorldSector * pNextSector )
{
	// Map Data는 있고 DWSector Load해야할 것들 Load
	PROFILE("AgcmMap::FlushLoadSector() - DWSector");

	ApWorldSector * pSector		;
	INT32			nDistance	;

	for( int i = 0 ; i < ( INT32 ) m_pcsApmMap->GetCurrentLoadedSectorCount() ; ++ i )
	{
		pSector		= m_pcsApmMap->GetCurrentLoadedSectors()[ i ];

		if( pNextSector )
			nDistance	= GetSectorDistance( pSector , pNextSector );
		else
			nDistance	= 0; // 무조건 로딩.

		// 디테일 변경체크..
		{
			PROFILE("AgcmMap::CBMyCharacterPositionChange() - Load DWSector");
			if( nDistance <= GetRoughLoadRadius()  )
			{
				if( !IsLoadedDetail( pSector , SECTOR_LOWDETAIL ) )
				{
					OnLoadRough( pSector );

					--this->m_lSectorsToLoad;
				}
			}

			if( nDistance <= GetDetailLoadRadius() )
			{
				if( !IsLoadedDetail( pSector , SECTOR_HIGHDETAIL ) )
				{
					OnLoadDetail( pSector );

					--this->m_lSectorsToLoad;
				}
			}
		}
	}
}

BOOL	AgcmMap::CBMyCharacterPositionChange2(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmMap::CBMyCharacterPositionChange2");

	AgcmMap *		pThis		= ( AgcmMap			* ) pClass		;
	ApWorldSector *	pPrevSector	= ( ApWorldSector	* ) pData		;
	ApWorldSector *	pNextSector	= ( ApWorldSector	* ) pCustData	;
	
	// 맵에서 빠지는 부분체크..
	if( pThis->m_pSelfCharacterSector == pNextSector )
	{
		PROFILE("AgcmMap::CBMyCharacterPositionChange2() - DWSector");
		ApWorldSector *	pSector		;
		INT32			i			;
		INT32			nDistance	;

		INT32				iSectorLoadedCount = pThis->m_pcsApmMap->GetCurrentLoadedSectorCount();
		ApWorldSector**		pSectorLoadedArray = pThis->m_pcsApmMap->GetCurrentLoadedSectors();

		INT32				iRoughLoadRadius = pThis->GetRoughLoadRadius();
		INT32				iDetailLoadRadius = pThis->GetDetailLoadRadius();

		INT32				iRoughReleaseRadius = pThis->GetRoughReleaseRadius();
		INT32				iDetailReleaseRadius = pThis->GetDetailReleaseRadius();

		INT32				iMapDataReleaseRadius = pThis->GetMapDataReleaseRadius();
		BOOL				bAutoLoadData = pThis->m_pcsApmMap->IsAutoLoadData();

		for( i = 0 ; i < iSectorLoadedCount ; ++ i )
		{
			pSector	= pSectorLoadedArray[ i ];

			// 마고자 (2004-03-29 오후 6:29:57) : 딜리트의 경우체크하지 않음..
			if( pSector->GetStatus() & ApWorldSector::STATUS_DELETED ) continue;

			nDistance	= GetSectorDistance( pSector , pThis->m_pSelfCharacterSector );

			// 디테일 변경체크..
			{
				PROFILE("AgcmMap::CBMyCharacterPositionChange2() - Load DWSector");
				if( nDistance <= iRoughLoadRadius  )
				{
					pThis->SetCurrentDetail( pSector , SECTOR_LOWDETAIL );
				}

				if( nDistance <= iDetailLoadRadius )
				{
					pThis->SetCurrentDetail( pSector , SECTOR_HIGHDETAIL );
				}
			}

			// 릴리즈 되는거 체크..
			{
				PROFILE("AgcmMap::CBMyCharacterPositionChange2() - Release DWSector");

				if( nDistance > iDetailReleaseRadius )
				{
					if( !(pSector->GetStatus() & ApWorldSector::STATUS_DELETED) )
						pThis->m_csMapLoader.RemoveQueue( pSector , SECTOR_HIGHDETAIL );
				}

				if( nDistance > iRoughReleaseRadius )
				{
					if( !(pSector->GetStatus() & ApWorldSector::STATUS_DELETED) )
						pThis->m_csMapLoader.RemoveQueue( pSector , SECTOR_LOWDETAIL );
				}

				/*
				if( nDistance >= pThis->GetRoughLoadRadius		() )
				{
					pThis->SetCurrentDetail( pSector , SECTOR_EMPTY );
				}
				*/
			}

			{
				PROFILE("AgcmMap::CBMyCharacterPositionChange2() - Delete Sector");

				if( nDistance > iMapDataReleaseRadius )
				{
					// pmMap에서 데이타 제거..

					if( bAutoLoadData )
					{
						pThis->m_csMapLoader.RemoveQueue( pSector );
						VERIFY( pThis->m_pcsApmMap->DeleteSector( pSector ) );
					}
					else
					{
						pThis->SetCurrentDetail( pSector , SECTOR_EMPTY );
					}
				}
			}
		}

		pThis->EnumCallback( AGCM_CB_ID_MAP_LOAD_END, pNextSector, NULL);
	}

	return TRUE;// pThis->m_csMapLoader.OnIdleLoad();
}

BOOL	AgcmMap::SetPreLightForAllGeometry	( FLOAT fValue , ProgressCallback pfCallback , void * pData)
{
	int x , z;
	int	x1 , x2 , z1 , z2;	// 계산된범위..

	// MAP_DEFAULT_DEPTH 단위로 잘라냄...
	x1 = GetLoadRangeX1() - GetLoadRangeX1() % MAP_DEFAULT_DEPTH				;
	z1 = GetLoadRangeZ1() - GetLoadRangeZ1() % MAP_DEFAULT_DEPTH				;
	x2 = GetLoadRangeX2() + ( MAP_DEFAULT_DEPTH - GetLoadRangeX2() % MAP_DEFAULT_DEPTH ) % MAP_DEFAULT_DEPTH	;
	z2 = GetLoadRangeZ2() + ( MAP_DEFAULT_DEPTH - GetLoadRangeZ2() % MAP_DEFAULT_DEPTH ) % MAP_DEFAULT_DEPTH	;

	ApWorldSector * pSector					;
	int				nSegmentZ , nSegmentX	;
	ApDetailSegment	* pSegment				;
	RwRGBA			rwRGBA					;
	UINT32			r,g,b					;

	static	char	strMessage[] = "SetPreLightForAllGeometry";
	int		nTarget		=	( x2 - x1 + 1 )		*
							( z2 - z1 + 1 )		;
	int		nCurrent	= 1						;

	for( x = x1 ; x < x2 ; x ++ )
	{
		for( z = z1 ; z < z2 ; z ++ )
		{
			pSector			= m_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );
			
			if( pSector )
			{
			
				for( nSegmentZ = 0 ; nSegmentZ < pSector->D_GetDepth() ; nSegmentZ ++ )
				{
					for( nSegmentX = 0 ; nSegmentX < pSector->D_GetDepth() ; nSegmentX ++ )
					{
						pSegment = pSector->D_GetSegment( nSegmentX , nSegmentZ );

						if( pSegment )
						{
							// Segment 칼라계산..
							r				= ( UINT32 ) ( ( ( FLOAT ) pSegment->vertexcolor.red	) * fValue );
							g				= ( UINT32 ) ( ( ( FLOAT ) pSegment->vertexcolor.green	) * fValue );
							b				= ( UINT32 ) ( ( ( FLOAT ) pSegment->vertexcolor.blue	) * fValue );

							if( r > 255 ) r = 255;
							if( g > 255 ) g = 255;
							if( b > 255 ) b = 255;

							rwRGBA.red		= ( UINT8 ) r;
							rwRGBA.green	= ( UINT8 ) g;
							rwRGBA.blue		= ( UINT8 ) b;
							rwRGBA.alpha	= pSegment->vertexcolor.alpha;

							D_SetValue(
								pSector						,
								pSector->GetCurrentDetail()	,
								nSegmentX					,
								nSegmentZ					,
								RwRGBAToApRGBA( rwRGBA )	);

						}
						/////////////////////////////////////
					}
				}// 세그먼트 루프


			}


		}
	}

	UnlockSectors();

	return TRUE;
}

RpAtomic *	AgcmMap::CreateCollisionAtomic		( ApWorldSector * pWorldSector , INT32 nDetail )
{
	ASSERT( NULL != pWorldSector );

	// 더미월드를 생성.
    RtWorldImport *				pWorldImport;
    RtWorldImportParameters		params		;
	RpGeometry				*	pGeometry	;
	RpAtomic				*	pAtomic		;

	TRACE( "WorldImport 데이타를 작성합니다.\n" );
    pWorldImport = __CreateWorldCollisionImport( pWorldSector , nDetail );
    if( pWorldImport == NULL )
    {
        return NULL;
    }

    RtWorldImportParametersInit(&params);

	params.flags					= rpWORLDTEXTURED;
	params.conditionGeometry		= FALSE	;
	params.calcNormals				= FALSE	;
	params.numTexCoordSets			= 1		; // 멀티텍스쳐 지원.
	params.maxWorldSectorPolygons	= 2		;
	params.maxOverlapPercent		= 1		;

//	params.flags					= 0		;	// 암것도 안씀..
//	params.numTexCoordSets			= 1		; // 멀티텍스쳐 지원.
//	params.maxWorldSectorPolygons	= 2		;
//	params.maxOverlapPercent		= 1		;

    VERIFY( pGeometry = RtWorldImportCreateGeometry(pWorldImport, &params) );
	if( pGeometry )
	{
		pAtomic	= RpAtomicCreate();
		LockFrame();

		RpAtomicSetGeometry	( pAtomic , pGeometry , 0	);
		RpAtomicSetFrame	( pAtomic , RwFrameCreate()	);

		UnlockFrame();

		RpGeometryDestroy	( pGeometry					);
	}

    RtWorldImportDestroy(pWorldImport);
	return pAtomic;
/*
	// 지오메트리 생성..
	RpAtomic * pAtomic ;
	RpGeometry * pGeometry;
	// 값 넣기.
	ASSERT( NULL != pWorldSector							);
	ASSERT( 0 <= nDetail && nDetail < SECTOR_DETAILDEPTH	);

	WORD i, j;

	INT32	nDepth		= pWorldSector->D_GetDepth		( nDetail )	;
	FLOAT	fStepSize	= pWorldSector->D_GetStepSize	( nDetail )	;
	FLOAT	xStart		= pWorldSector->GetXStart		()			;
	FLOAT	zStart		= pWorldSector->GetZStart		()			;

	if( nDepth <= 0 )
	{
		// 로딩이 아직 안된녀석이다.
		return NULL;
	}

	// 마고자 (2003-12-05 오후 2:45:29) : 콜리젼 문제를 해겨하기위해
	// 한줄씩 더 붙여보기로 함.
	nDepth += 2;
	xStart -= fStepSize;
	zStart -= fStepSize;

	INT32	nVertex		= ( nDepth + 1 ) * ( nDepth + 1 )	;
	INT32	nTrinagles	= nDepth * nDepth * 2				;

	// 지형 만듬
	{
		pGeometry	= RpGeometryCreate	(
			nVertex , nTrinagles ,
			rpGEOMETRYPOSITIONS			);

		ASSERT( NULL != pGeometry );

		// 아토믹 생성..
		pAtomic = RpAtomicCreate	();

		ASSERT( NULL != pAtomic );

		// 아토믹에 지오메트리와 프레임 연결함..
		LockFrame();

		if (!RpAtomicSetGeometry	( pAtomic , pGeometry , rpATOMICSAMEBOUNDINGSPHERE )	)
		{
			UnlockFrame();
			return NULL;
		}

		if (!RpAtomicSetFrame		( pAtomic , RwFrameCreate()	)								)
		{
			UnlockFrame();
			return NULL;
		}

		UnlockFrame();

		RpGeometryDestroy( pGeometry );
	}

	VERIFY( RpGeometryLock( pGeometry , rpGEOMETRYLOCKALL ) );

	RpMaterial	* pMaterial = RpMaterialCreate();
	ASSERT( NULL != pMaterial );
	RpMaterialSetTexture( pMaterial , NULL );
	INT32	nMaterialIndex = _rpMaterialListAppendMaterial( & pGeometry->matList , pMaterial );
	// RepCount 확인 요!// 마고자 (2004-02-24 오전 11:33:16) : 
	RpMaterialDestroy( pMaterial );

	_MakeDetailDataSet	stDataSet;

	stDataSet.pMorphTarget	= RpGeometryGetMorphTarget		( pGeometry , 0	);

	ASSERT( NULL != stDataSet.pMorphTarget );

	stDataSet.pPolygons		= RpGeometryGetTriangles		( pGeometry		);			//	Polygons themselves 
	stDataSet.pVertices		= RpMorphTargetGetVertices		( stDataSet.pMorphTarget	);			//	Vertex positions 
	stDataSet.pNormals		= RpMorphTargetGetVertexNormals	( stDataSet.pMorphTarget	);			//	Vertex normals 
	//stDataSet.pPreLitLum	= RpGeometryGetPreLightColors	( pGeometry		);
	
	for( i = 0 ; i < rwMAXTEXTURECOORDS ; ++i )
		stDataSet.pTexCoords[ i ] = RpGeometryGetVertexTexCoords( pGeometry , 
						( RwTextureCoordinateIndex ) ( rwTEXTURECOORDINATEINDEX0 + i ) );//	Texture coordinates 

	memset( stDataSet.pVertices	, 0 , sizeof(	RwV3d		) * nVertex		);
	memset( stDataSet.pPolygons	, 0 , sizeof(	RpTriangle	) * nTrinagles	);
	
	// Vertex Setting..

	FLOAT	xPos , yPos , zPos;
	ApWorldSector	* pSector;
	
	for ( i = 0 ; i < nDepth + 1 ; ++i )
	{
		for (j = 0 ; j < nDepth + 1 ; ++j )
		{
			xPos	= xStart + ( j ) * fStepSize			;
			zPos	= zStart + ( i ) * fStepSize			;
			
			pSector = m_pcsApmMap->GetSector( xPos , zPos )	;
			if( pSector )
			{
				yPos	= pSector->D_GetHeight( nDetail , xPos , zPos )	;
			}
			else
			{
				yPos	= INVALID_HEIGHT								; 
			}
			
			stDataSet.pVertices[ j + i * ( nDepth + 1 )	].x	= xPos;
			stDataSet.pVertices[ j + i * ( nDepth + 1 )	].y	= yPos;
			stDataSet.pVertices[ j + i * ( nDepth + 1 )	].z	= zPos;
		}
	}

	// Triangle Setting.
	INT32	nTriangleOffset	= 0;
	for ( i = 0 ; i < nDepth ; ++i )
	{
		for (j = 0 ; j < nDepth ; ++j )
		{
			stDataSet.pPolygons[ nTriangleOffset ].matIndex			= nMaterialIndex;
			stDataSet.pPolygons[ nTriangleOffset ].vertIndex[ 0 ]	= ( j + 0 ) + ( i + 1 ) * ( nDepth + 1 );
			stDataSet.pPolygons[ nTriangleOffset ].vertIndex[ 1 ]	= ( j + 1 ) + ( i + 0 ) * ( nDepth + 1 );
			stDataSet.pPolygons[ nTriangleOffset ].vertIndex[ 2 ]	= ( j + 0 ) + ( i + 0 ) * ( nDepth + 1 );

			nTriangleOffset++;

			stDataSet.pPolygons[ nTriangleOffset ].matIndex			= nMaterialIndex;
			stDataSet.pPolygons[ nTriangleOffset ].vertIndex[ 0 ]	= ( j + 0 ) + ( i + 1 ) * ( nDepth + 1 );
			stDataSet.pPolygons[ nTriangleOffset ].vertIndex[ 1 ]	= ( j + 1 ) + ( i + 1 ) * ( nDepth + 1 );
			stDataSet.pPolygons[ nTriangleOffset ].vertIndex[ 2 ]	= ( j + 1 ) + ( i + 0 ) * ( nDepth + 1 );

			nTriangleOffset++;
		}
	}

	RpGeometryUnlock( pGeometry );
	
	RwSphere	sphere;
	RpMorphTargetCalcBoundingSphere	( stDataSet.pMorphTarget , &sphere );
	RpMorphTargetSetBoundingSphere	( stDataSet.pMorphTarget , &sphere );

	return pAtomic;
*/
}

RtWorldImport *	AgcmMap::__CreateWorldCollisionImport		( ApWorldSector * pWorldSector , INT32 nDetail )
{
	ASSERT( NULL != pWorldSector							);
	ASSERT( 0 <= nDetail && nDetail < SECTOR_DETAILDEPTH	);

	RtWorldImport			*	pWorldImport;
	RtWorldImportTriangle	*	pTriangles	;
	RtWorldImportVertex		*	pVertices	;
	RwTexture				*	pTexture	=	NULL;
	RpMaterial				*	pMaterial	;
	RwInt32						matIndex	;

    WORD i, j;

	pWorldImport = RtWorldImportCreate();
	if( pWorldImport == NULL )
    {
        return NULL;
    }

	INT32	nDepth		= pWorldSector->D_GetDepth		( nDetail )	;
	FLOAT	fStepSize	= pWorldSector->D_GetStepSize	( nDetail )	;
	FLOAT	xStart		= pWorldSector->GetXStart		()			;
	FLOAT	zStart		= pWorldSector->GetZStart		()			;

	if( nDepth <= 0 )
	{
		// 로딩이 아직 안된녀석이다.
		return NULL;
	}

	// 마고자 (2003-12-05 오후 2:45:29) : 콜리젼 문제를 해겨하기위해
	// 한줄씩 더 붙여보기로 함.
	nDepth += 2;
	xStart -= fStepSize;
	zStart -= fStepSize;

	INT32	nVertex		= ( nDepth + 1 ) * ( nDepth + 1 )	;
	INT32	nTrinagles	= nDepth * nDepth * 2				;

	RtWorldImportAddNumVertices		( pWorldImport, nVertex		);
    RtWorldImportAddNumTriangles	( pWorldImport, nTrinagles	);

	// Create Emptry Material
    pMaterial	= RpMaterialCreate();
    matIndex	= RtWorldImportAddMaterial	( pWorldImport , pMaterial	);
    RpMaterialDestroy	( pMaterial	);

	pVertices	= RtWorldImportGetVertices	( pWorldImport				);
    pTriangles	= RtWorldImportGetTriangles	( pWorldImport				);
	
	memset( pVertices	, 0 , sizeof(	RtWorldImportVertex		) * nVertex		);
	memset( pTriangles	, 0 , sizeof(	RtWorldImportTriangle	) * nTrinagles	);

	
	// Vertex Setting..

	FLOAT	xPos , yPos , zPos;
	ApWorldSector	* pSector;
	
	for ( i = 0 ; i < nDepth + 1 ; ++i )
	{
		for (j = 0 ; j < nDepth + 1 ; ++j )
		{
			xPos	= xStart + ( j ) * fStepSize			;
			zPos	= zStart + ( i ) * fStepSize			;
			
			pSector = m_pcsApmMap->GetSector( xPos , zPos )	;
			if( pSector )
			{
				yPos	= pSector->D_GetHeight( nDetail , xPos , zPos )	;
			}
			else
			{
				yPos	= INVALID_HEIGHT								; 
			}
			
			pVertices[ j + i * ( nDepth + 1 )	].OC.x	= xPos	;
			pVertices[ j + i * ( nDepth + 1 )	].OC.z	= zPos	;
			pVertices[ j + i * ( nDepth + 1 )	].OC.y	= yPos	;
		}
	}

	// Triangle Setting.
	INT32	nTriangleOffset	= 0;
	for ( i = 0 ; i < nDepth ; ++i )
	{
		for (j = 0 ; j < nDepth ; ++j )
		{
			pTriangles[ nTriangleOffset ].matIndex			= 0;
			pTriangles[ nTriangleOffset ].vertIndex[ 0 ]	= ( j + 0 ) + ( i + 1 ) * ( nDepth + 1 );
			pTriangles[ nTriangleOffset ].vertIndex[ 1 ]	= ( j + 1 ) + ( i + 0 ) * ( nDepth + 1 );
			pTriangles[ nTriangleOffset ].vertIndex[ 2 ]	= ( j + 0 ) + ( i + 0 ) * ( nDepth + 1 );

			nTriangleOffset++;

			pTriangles[ nTriangleOffset ].matIndex			= 0;
			pTriangles[ nTriangleOffset ].vertIndex[ 0 ]	= ( j + 0 ) + ( i + 1 ) * ( nDepth + 1 );
			pTriangles[ nTriangleOffset ].vertIndex[ 1 ]	= ( j + 1 ) + ( i + 1 ) * ( nDepth + 1 );
			pTriangles[ nTriangleOffset ].vertIndex[ 2 ]	= ( j + 1 ) + ( i + 0 ) * ( nDepth + 1 );

			nTriangleOffset++;
		}
	}

	return pWorldImport;
}

BOOL	AgcmMap::CreateAllCollisionAtomic	( ProgressCallback pfCallback , void * pData )
{
	// 마고자 (2003-12-04 오후 5:51:40) : 맵툴용 펑션.
	ApWorldSector	* pSector	;
	RpAtomic		* pAtomic	;
	RpAtomic		* pAtomicGet;
	RpDWSector		* pDWSector	;

	for( int i = 0 ; i < ( int ) m_pcsApmMap->GetCurrentLoadedSectorCount() ; ++ i )
	{
		pSector		= m_pcsApmMap->GetCurrentLoadedSectors()[ i ]				;


		// 러프맵 콜리젼 아토믹 생성.
		// 마고자 (2004-01-09 오후 3:11:59) : 알수없는 크래시로 막아둠.
//		pAtomic		= CreateCollisionAtomic	( pSector , SECTOR_LOWDETAIL	);
//		pDWSector	= RpDWSectorGetDetail	( pSector , SECTOR_LOWDETAIL	);
//
//		if( pDWSector->atomic )
//		{
//			AcuObject::GetAtomicType( pDWSector->atomic , & index , NULL , NULL , NULL , (PVOID *) &pAtomicGet );
//			if( pAtomicGet )
//			{
//				RpAtomicDestroy( pAtomicGet );
//			}
//
//			if( pAtomic )
//			{
//				AcuObject::SetAtomicType(
//					pDWSector->atomic				,
//					ACUOBJECT_TYPE_WORLDSECTOR		,
//					( INT32 ) pSector				,
//					NULL							,
//					NULL							,
//					NULL							,
//					pAtomic							); // 해당 섹터의 포인터를 삽입해둔다.		
//			}
//		}

		// 디테일맵 콜리젼 아토믹 생성.
		pAtomic		= CreateCollisionAtomic	( pSector , SECTOR_HIGHDETAIL );
		pDWSector	= RpDWSectorGetDetail	( pSector , SECTOR_HIGHDETAIL );

		if( pDWSector && pDWSector->atomic )
		{
			pAtomicGet	= AcuObject::GetAtomicGetPickingAtomic	( pDWSector->atomic );

			if( pAtomicGet )
			{
				RpAtomicDestroy( pAtomicGet );
			}

			if( pAtomic )
			{
				AcuObject::SetAtomicType(
					pDWSector->atomic				,
					ACUOBJECT_TYPE_WORLDSECTOR		,
					( INT32 ) pSector				,
					NULL							,
					NULL							,
					NULL							,
					pAtomic							); // 해당 섹터의 포인터를 삽입해둔다.		
			}
		}
	}
	
	return TRUE;
}

BOOL	AgcmMap::DestroyAllCollisionAtomic	( ProgressCallback pfCallback , void * pData )
{
	ApWorldSector	* pSector	;
	RpAtomic		* pAtomic	;
	RpAtomic		* pAtomicGet;
	RpDWSector		* pDWSector	;
	INT32			index		;

	for( int i = 0 ; i < ( int ) m_pcsApmMap->GetCurrentLoadedSectorCount() ; ++ i )
	{
		pSector		= m_pcsApmMap->GetCurrentLoadedSectors()[ i ]				;
		pAtomic		= CreateCollisionAtomic	( pSector )			;
		pDWSector	= RpDWSectorGetDetail	( pSector , SECTOR_HIGHDETAIL );

		AcuObject::GetAtomicType( pDWSector->atomic , & index , NULL , NULL , NULL , (PVOID *) &pAtomicGet );
		if( pAtomicGet )
		{
			RpAtomicDestroy( pAtomicGet );
		}

		AcuObject::SetAtomicType(
			pDWSector->atomic				,
			ACUOBJECT_TYPE_WORLDSECTOR		,
			( INT32 ) pSector				,
			NULL							,
			NULL							,
			NULL							,
			NULL							); // 해당 섹터의 포인터를 삽입해둔다.		
	}
	return TRUE;
}

BOOL	AgcmMap::EnumLoadedDivision		( DivisionCallback pCallback , PVOID pData )
{
	INT32 x1,x2,z1,z2;
	x1 = GetLoadRangeX1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeX1() ) % MAP_DEFAULT_DEPTH;
	z1 = GetLoadRangeZ1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeZ1() ) % MAP_DEFAULT_DEPTH;
	x2 = GetLoadRangeX2() - ( GetLoadRangeX2() % MAP_DEFAULT_DEPTH );
	z2 = GetLoadRangeZ2() - ( GetLoadRangeZ2() % MAP_DEFAULT_DEPTH );

	int				nDivisionCount	= 0;
	DivisionInfo	stInfo;
	//ApWorldSector	* pSector;

	INT32	x,z;
	BOOL	bRet = TRUE;

	// 디비젼 갯수 구함..
	for( x = x1 ; x < x2 ; x += MAP_DEFAULT_DEPTH )
		for( z = z1 ; z < z2 ; z += MAP_DEFAULT_DEPTH )
			nDivisionCount ++;

	for( x = x1 ; x < x2 ; x += MAP_DEFAULT_DEPTH )
	{
		for( z = z1 ; z < z2 ; z += MAP_DEFAULT_DEPTH )
		{
			stInfo.nX		= x		;
			stInfo.nZ		= z		;
			stInfo.nDepth	= 16	;

			stInfo.fStartX	= GetSectorStartX( ArrayIndexToSectorIndexX( x ) );
			stInfo.fStartZ	= GetSectorStartZ( ArrayIndexToSectorIndexZ( z ) );
			stInfo.fWidth	= MAP_SECTOR_WIDTH * stInfo.nDepth;

			stInfo.nIndex	= GetDivisionIndex( x , z );

			if( pCallback ) bRet = pCallback( & stInfo , pData );
			if( bRet == FALSE ) return 0;
		}
	}

	return nDivisionCount;
}

BOOL	AgcmMap::CBLoadedDivisionCheck( DivisionInfo * pDivisionInfo , PVOID pData )
{
	stLoadedDivisionCheck	*pstDiv =  ( stLoadedDivisionCheck * ) pData;

	ASSERT( pstDiv );

	if( pDivisionInfo->nIndex == pstDiv->nDivision )
	{
		pstDiv->bInLoadedRange = TRUE;
	}

	return TRUE;
}

BOOL	AgcmMap::IsLoadedDivision( AuPOS * pPos )
{
	return IsLoadedDivision( GetDivisionIndexF( pPos->x , pPos->z ) );
}

BOOL	AgcmMap::IsLoadedDivision( INT32	nDivision )
{
	stLoadedDivisionCheck	stDiv;

	stDiv.nDivision = nDivision;

	EnumLoadedDivision( CBLoadedDivisionCheck , ( void * ) &stDiv );

	return stDiv.bInLoadedRange;
}

INT32	AgcmMap::EnumLoadedDivisionObject( DivisionCallback pCallback , PVOID pData )
{
	INT32 x1,x2,z1,z2;
	x1 = GetLoadRangeX1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeX1() ) % MAP_DEFAULT_DEPTH;
	z1 = GetLoadRangeZ1() + ( MAP_DEFAULT_DEPTH - GetLoadRangeZ1() ) % MAP_DEFAULT_DEPTH;
	x2 = GetLoadRangeX2() - ( GetLoadRangeX2() % MAP_DEFAULT_DEPTH );
	z2 = GetLoadRangeZ2() - ( GetLoadRangeZ2() % MAP_DEFAULT_DEPTH );

	int				nDivisionCount	= 0;
	DivisionInfo	stInfo;
	//ApWorldSector	* pSector;

	// 마고자 (2005-09-09 오후 3:20:05) : 
	// 잠시 블럭

	#ifdef _USE_MAP_EXPANSION_
	// 위아래좌우로 한디비젼씩 추가로함.
	x1 -= AgcmMap::EnumDivisionExpandValue;
	z1 -= AgcmMap::EnumDivisionExpandValue;
	x2 += AgcmMap::EnumDivisionExpandValue;
	z2 += AgcmMap::EnumDivisionExpandValue;
	#endif //_USE_MAP_EXPANSION_

	INT32	nDivisionX1 , nDivisionZ1 , nDivisionX2 , nDivisionZ2;

	nDivisionX1	= GetDivisionXIndex( GetDivisionIndex( x1 , z1 ) );
	nDivisionZ1	= GetDivisionZIndex( GetDivisionIndex( x1 , z1 ) );
	nDivisionX2	= GetDivisionXIndex( GetDivisionIndex( x2 , z2 ) );
	nDivisionZ2	= GetDivisionZIndex( GetDivisionIndex( x2 , z2 ) );

	AgcmObject * pcsAgcmObject = ( AgcmObject * ) GetModule( "AgcmObject" );

	if( pcsAgcmObject )
	{
		pcsAgcmObject->SetRange(
			GetSectorStartX	( ArrayIndexToSectorIndexX( x1 ) ),
			GetSectorStartZ	( ArrayIndexToSectorIndexZ( z1 ) ),
			GetSectorEndX	( ArrayIndexToSectorIndexX( x2 ) ),
			GetSectorEndZ	( ArrayIndexToSectorIndexZ( z2 ) )
			);
		pcsAgcmObject->UseRange( TRUE );	
	}

	INT32	x,z;
	BOOL	bRet = TRUE;

	/*
	// 디비젼 갯수 구함..
	for( x = x1 ; x < x2 ; x += MAP_DEFAULT_DEPTH )
		for( z = z1 ; z < z2 ; z += MAP_DEFAULT_DEPTH )
			nDivisionCount ++;

	for( x = x1 ; x < x2 ; x += MAP_DEFAULT_DEPTH )
	{
		for( z = z1 ; z < z2 ; z += MAP_DEFAULT_DEPTH )
		{
			stInfo.nX		= x		;
			stInfo.nZ		= z		;
			stInfo.nDepth	= 16	;

			stInfo.fStartX	= GetSectorStartX( ArrayIndexToSectorIndexX( x ) );
			stInfo.fStartZ	= GetSectorStartZ( ArrayIndexToSectorIndexZ( z ) );
			stInfo.fWidth	= MAP_SECTOR_WIDTH * stInfo.nDepth;

			stInfo.nIndex	= GetDivisionIndex( x , z );

			if( pCallback ) bRet = pCallback( & stInfo , pData );
			if( bRet == FALSE ) return 0;
		}
	}
	*/
	// 디비젼 갯수 구함..
	for( x = nDivisionX1 ; x <= nDivisionX2 ; x ++ )
		for( z = nDivisionZ1 ; z <= nDivisionZ2 ; z ++ )
			nDivisionCount ++;

	INT32 nDivisionIndex;

	for( x = nDivisionX1 ; x <= nDivisionX2 ; x ++ )
	{
		for( z = nDivisionZ1 ; z <= nDivisionZ2 ; z ++ )
		{
			nDivisionIndex	= MakeDivisionIndex( x , z );

			stInfo.nX		= GetFirstSectorXInDivision( nDivisionIndex );
			stInfo.nZ		= GetFirstSectorZInDivision( nDivisionIndex );
			stInfo.nDepth	= 16	;

			stInfo.fStartX	= GetSectorStartX( ArrayIndexToSectorIndexX( stInfo.nX ) );
			stInfo.fStartZ	= GetSectorStartZ( ArrayIndexToSectorIndexZ( stInfo.nZ ) );
			stInfo.fWidth	= MAP_SECTOR_WIDTH * stInfo.nDepth;

			stInfo.nIndex	= nDivisionIndex;

			if( pCallback ) bRet = pCallback( & stInfo , pData );
			if( bRet == FALSE ) return 0;
		}
	}

	return nDivisionCount;
}

BOOL	AgcmMap::CreateCompactDataFromDetailInfo	( ApWorldSector * pSector )	//	디테일 데이타에서 컴펙트 데이타를 생성함.
{
	ASSERT( NULL != pSector );
	if( NULL == pSector ) return FALSE;

	// 메모리 준비..
	VERIFY( pSector->FreeCompactSectorInfo	() );
	VERIFY( pSector->AllocCompactSectorInfo	() );
	ASSERT( NULL != pSector->m_pCompactSectorInfo	);
	ASSERT( NULL == pSector->m_pCompactSectorInfo->m_pCompactSegmentArray	);

	// 메모리 할당..

	pSector->m_pCompactSectorInfo->Alloc( MAP_DEFAULT_DEPTH );
	ASSERT( NULL != pSector->m_pCompactSectorInfo->m_pCompactSegmentArray	);
	ASSERT( MAP_DEFAULT_DEPTH == pSector->m_pCompactSectorInfo->m_nCompactSegmentDepth 	);

	// 데이타 모으기
	int		x , z	;

	ApDetailSegment	*	pSegment		;
	ApCompactSegment *	pCompactSegment	;

	AcTextureList::TextureInfo		*	pTextureInfo	;
	
	INT32	nOffsetX , nOffsetZ;
	AuNode< ApmMap::RegionElement > * pNode;

	for( z = 0 ; z < MAP_DEFAULT_DEPTH ; ++z )
	{
		for( x = 0 ; x < MAP_DEFAULT_DEPTH ; ++x )
		{
			// 세그먼트 정보 얻음..
			pSegment		= pSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z );
			pCompactSegment	= pSector->m_pCompactSectorInfo->GetSegment( x , z );

			ASSERT( NULL != pSegment			);
			ASSERT( NULL != pCompactSegment		);

			// 마고자 (2005-12-21 오후 4:23:16) : 
			// 높이 지정은 이제 안함..
			// pCompactSegment->SetHeight( pSegment->height );

			// 타일정보 복사
			pCompactSegment->stTileInfo	= pSegment->stTileInfo;

			// 마고자 (2004-08-24 오후 4:32:16) : 타일 속성을 지정함..
			pTextureInfo = m_TextureList.GetTextureInfo( pSegment->pIndex[ TD_FIRST ] );
			if( NULL == pTextureInfo )
			{
				// 타일 정보가 없는경우 솔리드로..
				pCompactSegment->stTileInfo.tiletype	= APMMAP_MATERIAL_SOIL;
			}
			else
			{
				pCompactSegment->stTileInfo.tiletype	= pTextureInfo->nProperty;
			}

			// 마고자 (2004-09-17 오후 5:07:06) : 리젼 정보..
			nOffsetX = ( pSector->GetArrayIndexX() * MAP_DEFAULT_DEPTH + x ) * 2;
			nOffsetZ = ( pSector->GetArrayIndexZ() * MAP_DEFAULT_DEPTH + z ) * 2;

			if( pNode = m_pcsApmMap->GetRegion( nOffsetX , nOffsetZ ) )
			{
				pCompactSegment->SetRegion( pNode->GetData().nIndex );
			}
		}
	}

	return TRUE;
}

BOOL	AgcmMap::SaveGeometryBlocking( INT32 nDivisionIndex , char * pFileName )
{
	CBmp	bmp;
	if( !bmp.Create( 256 , 256 ) )
	{
		TRACE( "비트맵 생성 실패..\n" );
		return FALSE;
	}

	// Clear..
	HDC hDC = bmp.GetDC();
	{
		::SetBkColor(hDC, RGB( 0 , 0 , 0 ) );

		RECT	rect;
		rect.left	= 0;
		rect.top	= 0;
		rect.right	= 256;
		rect.bottom	= 256;

		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	}

	INT32	nSectorIndexX , nSectorIndexZ;
	nSectorIndexX = ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivisionIndex ) );
	nSectorIndexZ = ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivisionIndex ) );

	ApWorldSector * pSector ;
	
	INT32	nSectorX , nSectorZ;
	INT32	nX , nZ;

	ApDetailSegment * pSegment;
	BOOL	bBlock;

	COLORREF	rgbColor;

	for( nSectorZ = nSectorIndexZ ; nSectorZ < nSectorIndexZ + MAP_DEFAULT_DEPTH ; nSectorZ ++ )
	{
		for( nSectorX = nSectorIndexX ; nSectorX < nSectorIndexX + MAP_DEFAULT_DEPTH ; nSectorX ++ )
		{
			pSector	= this->m_pcsApmMap->GetSector( nSectorX , nSectorZ );

			if( pSector )
			{
				for( nZ = 0 ; nZ < MAP_DEFAULT_DEPTH ; nZ ++ )
				{
					for( nX = 0 ; nX < MAP_DEFAULT_DEPTH ; nX ++ )
					{
						pSegment = pSector->D_GetSegment( SECTOR_HIGHDETAIL , nX , nZ );
						if( pSegment )
						{
							bBlock = pSegment->stTileInfo.GetGeometryBlocking();

							if( bBlock )
							{
								rgbColor = RGB( 255 , 0 , 0 );
								::SetPixel( hDC , 
									( nSectorX - nSectorIndexX ) * MAP_DEFAULT_DEPTH + nX ,
									( nSectorZ - nSectorIndexZ ) * MAP_DEFAULT_DEPTH + nZ , rgbColor );
							}
						}
					}
				}
			}
			////////////
		}
	}

	// 비트맵 완성..

	// 저장..
	if( bmp.Save( pFileName ) )	return TRUE		;
	else						return FALSE	;
}

BOOL	AgcmMap::LoadGeometryBlocking( INT32 nDivisionIndex , char * pFileName )
{
	CBmp	bmp;
	if( !bmp.Load( pFileName , NULL ) )
	{
		TRACE( "파일 로드 실패 (%s)\n" , pFileName );
		return FALSE;
	}

	HDC hDC = bmp.GetDC();

	INT32	nSectorIndexX , nSectorIndexZ;
	nSectorIndexX = ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivisionIndex ) );
	nSectorIndexZ = ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivisionIndex ) );

	ApWorldSector * pSector ;
	
	INT32	nSectorX , nSectorZ;
	INT32	nX , nZ;

	ApDetailSegment * pSegment;
	BOOL	bBlock;

	COLORREF	rgbColor;

	for( nSectorZ = nSectorIndexZ ; nSectorZ < nSectorIndexZ + MAP_DEFAULT_DEPTH ; nSectorZ ++ )
	{
		for( nSectorX = nSectorIndexX ; nSectorX < nSectorIndexX + MAP_DEFAULT_DEPTH ; nSectorX ++ )
		{
			pSector	= this->m_pcsApmMap->GetSector( nSectorX , nSectorZ );

			if( pSector )
			{
				for( nZ = 0 ; nZ < MAP_DEFAULT_DEPTH ; nZ ++ )
				{
					for( nX = 0 ; nX < MAP_DEFAULT_DEPTH ; nX ++ )
					{
						pSegment = pSector->D_GetSegment( nX , nZ );
						if( pSegment )
						{
							rgbColor = ::GetPixel( hDC , ( nSectorX - nSectorIndexX ) * MAP_DEFAULT_DEPTH + nX , ( nSectorZ - nSectorIndexZ ) * MAP_DEFAULT_DEPTH + nZ );

							if( rgbColor != RGB( 0 , 0 , 0 ) )	bBlock = TRUE	;
							else								bBlock = FALSE	;

							pSegment->stTileInfo.SetGeometryBlocking( bBlock );
						}
					}
				}
			}
			////////////
		}
	}

	return TRUE;
}

BOOL	AgcmMap::LoadSkyBlocking		( INT32 nDivisionIndex , char * pFileName )
{
	CBmp	bmp;
	if( !bmp.Load( pFileName , NULL ) )
	{
		TRACE( "파일 로드 실패 (%s)\n" , pFileName );
		return FALSE;
	}

	HDC hDC = bmp.GetDC();

	INT32	nSectorIndexX , nSectorIndexZ;
	nSectorIndexX = ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivisionIndex ) );
	nSectorIndexZ = ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivisionIndex ) );

	ApWorldSector * pSector ;
	
	INT32	nSectorX , nSectorZ;
	INT32	nX , nZ;

	ApDetailSegment * pSegment;
	BOOL	bBlock;

	COLORREF	rgbColor;

	for( nSectorZ = nSectorIndexZ ; nSectorZ < nSectorIndexZ + MAP_DEFAULT_DEPTH ; nSectorZ ++ )
	{
		for( nSectorX = nSectorIndexX ; nSectorX < nSectorIndexX + MAP_DEFAULT_DEPTH ; nSectorX ++ )
		{
			pSector	= this->m_pcsApmMap->GetSector( nSectorX , nSectorZ );

			if( pSector )
			{
				for( nZ = 0 ; nZ < MAP_DEFAULT_DEPTH ; nZ ++ )
				{
					for( nX = 0 ; nX < MAP_DEFAULT_DEPTH ; nX ++ )
					{
						pSegment = pSector->D_GetSegment( nX , nZ );
						if( pSegment )
						{
							rgbColor = ::GetPixel( hDC , ( nSectorX - nSectorIndexX ) * MAP_DEFAULT_DEPTH + nX , ( nSectorZ - nSectorIndexZ ) * MAP_DEFAULT_DEPTH + nZ );

							if( rgbColor != RGB( 0 , 0 , 0 ) )	bBlock = TRUE	;
							else								bBlock = FALSE	;

							pSegment->stTileInfo.SetSkyBlocking( bBlock );
						}
					}
				}
			}
			////////////
		}
	}

	return TRUE;
}

BOOL	AgcmMap::CBLoadGeometry	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	FILE			* pFile		= ( FILE * ) pData;
	ApWorldSector	* pSector	= ( ApWorldSector * ) pCustData;
	AgcmMap			* pThis		= ( AgcmMap * ) pClass;

	AgcmMapSectorData	* pClientSectorData		= pThis->GetAgcmAttachedData( pSector );

	#ifdef USE_MFC
	_MaptoolClientSaveData	*pSaveData1			= ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData1;
	_MaptoolClientSaveData	*pSaveData2			= ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData2;

	ASSERT( pSaveData1 );
	ASSERT( pSaveData2 );

	if( g_bDisablePolygonLoading )
	{
		// do nothing.
	}
	else
	{
		// 마고자 (2005-03-24 오후 5:26:45) : 별도움안됌...
		/*
		pSaveData2->GeometryStreamRead( pSector , SECTOR_HIGHDETAIL	, pFile );
		pSaveData1->GeometryStreamRead( pSector , SECTOR_LOWDETAIL	, pFile );
		*/
	}

	#endif // USE_MFC

	return TRUE;
}

BOOL	AgcmMap::CBSaveGeometry	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	FILE			* pFile		= ( FILE * ) pData;
	ApWorldSector	* pSector	= ( ApWorldSector * ) pCustData;
	AgcmMap			* pThis		= ( AgcmMap * ) pClass;

	AgcmMapSectorData	* pClientSectorData		= pThis->GetAgcmAttachedData( pSector );

	#ifdef USE_MFC
	_MaptoolClientSaveData	*pSaveData1			= ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData1;
	_MaptoolClientSaveData	*pSaveData2			= ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData2;

	ASSERT( pSaveData1 );
	ASSERT( pSaveData2 );

	// 마고자 (2005-03-24 오후 5:26:45) : 별도움안됌...
	/*
	pSaveData2->GeometryStreamWrite( pSector , SECTOR_HIGHDETAIL	, pFile );
	pSaveData1->GeometryStreamWrite( pSector , SECTOR_LOWDETAIL		, pFile );
	*/

	#endif USE_MFC

	return TRUE;
}

BOOL	AgcmMap::CBLoadMoonee	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	FILE			* pFile		= ( FILE * ) pData;
	ApWorldSector	* pSector	= ( ApWorldSector * ) pCustData;
	AgcmMap			* pThis		= ( AgcmMap * ) pClass;

	AgcmMapSectorData	* pClientSectorData		= pThis->GetAgcmAttachedData( pSector );

	#ifdef USE_MFC
	_MaptoolClientSaveData	*pSaveData1			= ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData1;
	_MaptoolClientSaveData	*pSaveData2			= ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData2;

	ASSERT( pSaveData1 );
	ASSERT( pSaveData2 );

	if( g_bDisablePolygonLoading )
	{
		// do nothing.
	}
	else
	{
		pSaveData2->MooneeStreamRead( pSector , SECTOR_HIGHDETAIL	, pFile );
		// pSaveData1->MooneeStreamRead( pSector , SECTOR_LOWDETAIL	, pFile );
	}
	#endif USE_MFC

	return TRUE;
}

BOOL	AgcmMap::CBSaveMoonee	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	FILE			* pFile		= ( FILE * ) pData;
	ApWorldSector	* pSector	= ( ApWorldSector * ) pCustData;
	AgcmMap			* pThis		= ( AgcmMap * ) pClass;

	AgcmMapSectorData	* pClientSectorData		= pThis->GetAgcmAttachedData( pSector );

	#ifdef USE_MFC
	_MaptoolClientSaveData	*pSaveData1			= ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData1;
	_MaptoolClientSaveData	*pSaveData2			= ( _MaptoolClientSaveData * ) pClientSectorData->m_pLoadedData2;

	ASSERT( pSaveData1 );
	ASSERT( pSaveData2 );

	pSaveData2->MooneeStreamWrite( pSector , SECTOR_HIGHDETAIL	, pFile );
	//pSaveData1->MooneeStreamWrite( pSector , SECTOR_LOWDETAIL	, pFile );

	{
		// 

	}


	#endif USE_MFC

	return TRUE;
}

BOOL	AgcmMap::GetOnObject		( FLOAT fX , FLOAT fY , FLOAT fZ )
{
	// fX , fZ가 어느섹터 어느 타일인지 알아냄..
	ApWorldSector * pSector = m_pcsApmMap->GetSector( fX , fZ );

	if( pSector )
	{
		int segx, segz;
		pSector->D_GetSegment( SECTOR_HIGHDETAIL , fX , fZ , & segx , &segz );

		// 라이더블 오브젝트가 있는지 점검..
		INT32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
		INT32	nObjectCount = pSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT , segx , segz , aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT );

		if( nObjectCount )
		{
			stCollisionAtomicInfo	stCollInfo;
			stCollInfo.fX				= fX					;
			stCollInfo.fZ				= fZ					;
			stCollInfo.fHeight			= HP_GetHeightGeometryOnly( fX , fZ );
			stCollInfo.fCharacterHeight	= fY					;
			stCollInfo.nObject			= nObjectCount			;
			stCollInfo.aObject			= aObjectList			;

			// 라이더블 리스트 루프
			if( EnumCallback( AGCM_CB_ID_ONGETHEIGHT , ( void * ) & stCollInfo , NULL ) )
			{
				return TRUE;
			}
			else
			{
				// 걸리는게 없으면 
				// 그냥 ApmMap->GetType 을 호출하여 리턴함.
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}	
	else
	{
		return FALSE;
	}
}

UINT8	AgcmMap::GetType	( FLOAT fX , FLOAT fY , FLOAT fZ )
{
	// fX , fZ가 어느섹터 어느 타일인지 알아냄..
	ApWorldSector * pSector = m_pcsApmMap->GetSector( fX , fZ );

	if( pSector )
	{
		int segx, segz;
		pSector->D_GetSegment( SECTOR_HIGHDETAIL , fX , fZ , & segx , &segz );

		// 라이더블 오브젝트가 있는지 점검..
		INT32	aObjectList[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
		INT32	nObjectCount = pSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT , segx , segz , aObjectList , SECTOR_MAX_COLLISION_OBJECT_COUNT);

		if( nObjectCount )
		{
			stCollisionAtomicInfo	stCollInfo;
			stCollInfo.fX				= fX					;
			stCollInfo.fZ				= fZ					;
			stCollInfo.fHeight			= HP_GetHeightGeometryOnly( fX , fZ );
			stCollInfo.fCharacterHeight	= fY					;
			stCollInfo.nObject			= nObjectCount			;
			stCollInfo.aObject			= aObjectList			;

			// 라이더블 리스트 루프
			if( EnumCallback( AGCM_CB_ID_ONGETHEIGHT , ( void * ) & stCollInfo , NULL ) )
			{
				return stCollInfo.uRidableType;
			}
			else
			{
				// 걸리는게 없으면 
				// 그냥 ApmMap->GetType 을 호출하여 리턴함.
				return pSector->GetType( fX , 0.0f , fZ );
			}
		}
		else
		{
			// 걸리는게 없으면 
			// 그냥 ApmMap->GetType 을 호출하여 리턴함.
			return pSector->GetType( fX , 0.0f , fZ );
		}
	}	
	else
	{
		return APMMAP_MATERIAL_SOIL;
	}
}

void	AgcmMap::ClearAllSectors()
{
	if( IsLocked() ) return;

	// 로딩됀 모든 섹터 정보를 날린다.

	ApWorldSector * pSector;
	BOOL			bPrevMode	= m_pcsAgcmResourceLoader->m_bForceImmediate;

	m_pcsAgcmResourceLoader->m_bForceImmediate = TRUE;

	for( int i = 0 ; i < ( int ) m_pcsApmMap->GetCurrentLoadedSectorCount() ; ++ i )
	{
		pSector		= m_pcsApmMap->GetCurrentLoadedSectors()[ i ];

		if( this->m_pSelfCharacterSector != pSector )
		{
			VERIFY( m_pcsApmMap->DeleteSector( pSector ) );
		}
	}

	m_pcsApmMap->FlushDeleteSectors();

	m_nZoneLoadDivision = 0;

	m_pcsAgcmResourceLoader->m_bForceImmediate = bPrevMode;

	m_posCurrentAvatarPosition.x	= -9999999999.0f;
	m_posCurrentAvatarPosition.z	= -9999999999.0f;
	m_posCurrentAvatarPosition.y	= -9999999999.0f;
}

BOOL	AgcmMap::ZoneLoadingDivision( INT32 nDivision , INT16 nRegion )
{
	if( m_nZoneLoadDivision == nDivision ) return FALSE;

	BOOL	bPrevMode = m_pcsAgcmResourceLoader->m_bForceImmediate;
	m_pcsAgcmResourceLoader->m_bForceImmediate = TRUE;

	INT32	lFirstSectorIndexX	= ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivision ) );
	INT32	lFirstSectorIndexZ	= ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivision ) );

	int nSectorX , nSectorZ;
	ApWorldSector * pSector;

	this->m_lSectorsToLoad = 0;

	for( nSectorZ = lFirstSectorIndexZ ; nSectorZ < lFirstSectorIndexZ + MAP_DEFAULT_DEPTH  ; nSectorZ++ )
	{
		for( nSectorX = lFirstSectorIndexX ; nSectorX < lFirstSectorIndexX + MAP_DEFAULT_DEPTH  ; nSectorX++ )
		{
			pSector = m_pcsApmMap->GetSector(
							nSectorX	,
							0			,
							nSectorZ	);

			if( NULL == pSector )
			{
				PROFILE("AgcmMap::CheckSelfCharacterRegionChange() - LoadSector");

				pSector = LoadSector( 
							nSectorX	,
							0			,
							nSectorZ	);

				TRACE("AgcmMap::CheckSelfCharacterRegionChange() Load Sector %x\n", pSector);
				// Callback으로 OnLoadSector 호출됨..
			}

			if( pSector )
			{
				//pSector->m_Mutex.Lock();

				// 러프 & 디테일.
				++this->m_lSectorsToLoad;
				++this->m_lSectorsToLoad;

				//pSector->m_Mutex.Unlock();
			}
		}
	}

	// 존로딩 러프 섹터 정보 로딩.
	{
		/*
		const	INT32	nLoadRadius = 6;

		FLOAT	fHeightOffset	= -20000.0f	;
		FLOAT	fSourceX		= -44800.0f	;
		FLOAT	fSourceZ		= 17200.0f	;
		UINT32	nDivision		= 3232		;
		*/

		ApmMap::RegionTemplate*	pTemplate = m_pcsApmMap->GetTemplate( nRegion );

		if( pTemplate->nParentIndex != -1 )
		{
			pTemplate	=	m_pcsApmMap->GetTemplate( pTemplate->nParentIndex );
		}

		if( pTemplate && pTemplate->ti.stType.bZoneLoadArea )
		{
			LoadZoneRoughBuffer(	pTemplate->zi.fSrcX				,
									pTemplate->zi.fSrcZ				,
									pTemplate->zi.fHeightOffset		,
									pTemplate->zi.fDstX				,
									pTemplate->zi.fDstZ				,
									pTemplate->zi.nRadius			);
		}

	}

	FlushLoadSector( NULL );

	m_nZoneLoadDivision = nDivision;
	m_pcsAgcmResourceLoader->m_bForceImmediate = bPrevMode;

	return TRUE;
}

BOOL	AgcmMap::LoadZoneRoughBuffer( FLOAT fSrcX , FLOAT fSrcZ , FLOAT fHeight , FLOAT fDstX , FLOAT fDstZ , INT32 nRadius )
{
	FlushZoneRoughBuffer();

	if( 0 == nRadius )
	{
		// 존로딩 하지 않음.
		return TRUE;
	}
	
	INT32	nCenterX = PosToSectorIndexX( fSrcX );
	INT32	nCenterZ = PosToSectorIndexZ( fSrcZ );

	INT32	nSectorX , nSectorZ;
	RpAtomic	* pAtomic;

	for( nSectorZ = nCenterZ - nRadius ; nSectorZ < nCenterZ + nRadius ; nSectorZ ++ )
	{
		for( nSectorX = nCenterX - nRadius ; nSectorX < nCenterX + nRadius ; nSectorX ++ )
		{
			pAtomic = LoadRoughAtomic( nSectorX , nSectorZ , fSrcX , fSrcZ , fHeight , fDstX , fDstZ );
			if( pAtomic )
			{
				m_vectorZoneRough.push_back( pAtomic );

				m_pcsAgcmRender->InitAtomicSetRenderCallBack( pAtomic, AgcmRender::RenderCallbackForNonAlphaAtomic ,R_NONALPHA,R_BLENDSRCAINVA_ADD,FALSE);
			}
		}
	}

	return TRUE;
}

void	AgcmMap::FlushZoneRoughBuffer()
{
	vector<RpAtomic *>::iterator Iter;

	for ( Iter = m_vectorZoneRough.begin( ) ; Iter != m_vectorZoneRough.end( ) ; Iter++ )
	{
		RpAtomicDestroy( *Iter );
	}

	m_vectorZoneRough.clear();
}

BOOL	AgcmMap::CheckSelfCharacterRegionChange( AuPOS *pPrev , AuPOS *pNext )
//void	SelfCharacterRegionChange(  )
{
	INT16	nRegionPrev = this->m_pcsApmMap->GetRegion( pPrev->x , pPrev->z );
	INT16	nRegionNext = this->m_pcsApmMap->GetRegion( pNext->x , pNext->z );

	//ApmMap::RegionTemplate*	pTemplatePrev = this->m_pcsApmMap->GetTemplate( nRegionPrev );
	ApmMap::RegionTemplate*	pTemplateNext = this->m_pcsApmMap->GetTemplate( nRegionNext );

	INT32 nDivision = GetDivisionIndexF( pNext->x , pNext->z );

	ASSERT( NULL != pTemplateNext );
	if( NULL == pTemplateNext ) return FALSE;

	if( nRegionPrev == nRegionNext )
	{
		// 리젼이 변하지 않았으므로..
		m_nZoneLoadDivision = 0;
		return pTemplateNext->ti.stType.bZoneLoadArea;
	}

	// 현재 존 로딩 중인지 점검
	// 로딩 중이면 
	if( IsZoneLoading() )
	{
		// 현재 존로딩 지역

		if( pTemplateNext->ti.stType.bZoneLoadArea )
		{
			if( m_nZoneLoadDivision != nDivision )
			{
				// 디비젼이 다를 경우만 새로 로딩함.
				ClearAllSectors();

				ZoneLoadingDivision( nDivision , nRegionNext );
				m_posCurrentAvatarPosition.x = pNext->x;
				m_posCurrentAvatarPosition.y = pNext->y;
				m_posCurrentAvatarPosition.z = pNext->z;

				ShowAll();
			}
			else
			{
				// do nothing..
			}

			return TRUE;
		}
		else
		{
			TRACE( "AgcmMap::SelfCharacterRegionChange 일반 로딩 지역으로 이동...\n" );
			// 일반 로딩 지역으로 이동..

			// 존 로딩한 데이타를 날리고..
			// MyCharacterPosition을 호출한다.

			// 로딩한 데이타를 날리고..
			// 현재 디비젼의 데이타를 로딩한다.

			m_nZoneLoadDivision = 0;

			return FALSE;
		}
	}
	else
	{
		// 현재 일반 로딩 지역
		if( pTemplateNext->ti.stType.bZoneLoadArea )
		{
			TRACE( "AgcmMap::SelfCharacterRegionChange 존 로딩 지역으로 이동...\n" );
			// 존 로딩 지역으로 이동..

			// 존 로딩 지역으로 이동..

			ClearAllSectors();

			ZoneLoadingDivision( nDivision , nRegionNext );
			m_posCurrentAvatarPosition.x = pNext->x;
			m_posCurrentAvatarPosition.y = pNext->y;
			m_posCurrentAvatarPosition.z = pNext->z;

			ShowAll();

			return TRUE;
		}
		else
		{
			// 일반 로딩 지역으로 이동..

			// do no thing..

			return FALSE;
		}
	}
}

INT32	AgcmMap::GetSectorList		( RwBBox *pbbox , vector< ApWorldSector * > * pVector )
{
	// Set Blocking...

	// 범위를 조사함..
	INT32	uSectorX1 , uSectorX2 , uSectorZ1 , uSectorZ2;
	AuPOS	pos;
	pos.y	= 0.0f;
	pos.x	= ( pbbox->inf.x < pbbox->sup.x ? pbbox->inf.x : pbbox->sup.x ) ;
	pos.z	= ( pbbox->inf.z < pbbox->sup.z ? pbbox->inf.z : pbbox->sup.z ) ;
	m_pcsApmMap->GetSector( pos , &uSectorX1 , NULL , &uSectorZ1 );
	pos.x	= ( pbbox->inf.x < pbbox->sup.x ? pbbox->sup.x : pbbox->inf.x ) ;
	pos.z	= ( pbbox->inf.z < pbbox->sup.z ? pbbox->sup.z : pbbox->inf.z ) ;
	m_pcsApmMap->GetSector( pos , &uSectorX2 , NULL , &uSectorZ2 );

	INT32 sx , sz;
	ApWorldSector * pSectorTmp;

	for( sz = uSectorZ1 ; sz <= uSectorZ2 ; sz ++ )
	{
		for( sx = uSectorX1 ; sx <= uSectorX2 ; sx ++ )
		{
			pSectorTmp = m_pcsApmMap->GetSector( sx , sz );

			if( pSectorTmp )
			{
				pVector->push_back( pSectorTmp );
			}
		}
	}

	return pVector->size();
}

RwTexture *	AgcmMap::LoadRoughTexture( RwStream *stream )
{
	ASSERT( NULL != stream );

	RwTexDictionary	*	pTextureDictionary	= NULL;
	RwTexture		*	pTexture			= NULL;

	if ( RwStreamFindChunk( stream , rwID_TEXDICTIONARY, NULL, NULL) )
	{
		pTextureDictionary = RwTexDictionaryStreamRead( stream );

		if (m_pcsAgcmResourceLoader && pTextureDictionary)
		{
			// 딕셔너리에 텍스쳐가 하나 존재한다.. 그텍스쳐를 뽑아냄..
			RwTexDictionaryForAllTextures( pTextureDictionary, CBMoveTexture, &pTexture );

			// 텍스쳐를 메인 딕셔너리로 옮긴다.
			RwTexDictionaryRemoveTexture( pTexture );
			RwTexDictionaryAddTexture( m_pcsAgcmResourceLoader->GetDefaultTexDict(), pTexture);
			RwTexDictionaryDestroy( pTextureDictionary );

			m_pcsAgcmResourceLoader->SetDefaultTexDict();
		}
	}
	else
	{
		ASSERT( !"이거 러프맵이 옛날데이타!" );
	}

	return pTexture;
}

RpAtomic *		AgcmMap::LoadRoughAtomic		( INT32 wx	, INT32 wz , FLOAT fSrcX , FLOAT fSrcZ , FLOAT fHeightOffset , FLOAT fDstX , FLOAT fDstZ )
{
	char	str			[ 256	];
	char	fullpath	[ 1024	];
	char	strDir		[ 1024	];

	char	strPackedFilename[ 256 ];

	// DWSector 처리..

	INT32	nDivisionIndex = GetDivisionIndex( SectorIndexToArrayIndexX( wx ) , SectorIndexToArrayIndexZ( wz ) );

	wsprintf( str , ALEF_WORLD_ROUGH_SECTOR_FILE_NAME_FORMAT , 	nDivisionIndex );
	wsprintf( strPackedFilename , SECTOR_DFF_FILE_FORMAT_ROUGH , 
		SectorIndexToArrayIndexX( wx ) , SectorIndexToArrayIndexZ( wz ) );

	wsprintf( strDir , "%s\\World" , m_strToolImagePath );
	// 디렉토리 변경..
	//SetCurrentDirectory( strDir );
	
	wsprintf( fullpath , "World\\%s" , str );

	// DWSector 저장됀 파일 읽어 들인후에..
	// 언하는 섹터 데이타만 뽑아서 로딩하는 루틴..
	// 전에 이야기했던 파인드 청크 연타하는 방식..

	RwStream	*stream		;
	CMagUnpackManager *	pUnpack = NULL;

	pUnpack = m_csMapLoader.GetUnpackManager( SECTOR_LOWDETAIL , nDivisionIndex , fullpath );

	if( NULL == pUnpack )
	{
		return NULL;
	}

	BYTE	* pPtr = NULL;
	UINT	nSize;

	if( ( nSize = pUnpack->GetFileBinary( strPackedFilename , pPtr ) ) == 0  )
	{
		MD_SetErrorMessage( "AgcmMap::LoadDWSector 압축된 파일을 찾을 수 없습니다" );
		return FALSE;
	}

	RwTexture *	pTexture = NULL;
	RwMemory	memory;
	memory.start	= pPtr	;
	memory.length	= nSize	;

	stream	= RwStreamOpen		( rwSTREAMMEMORY , rwSTREAMREAD , &memory );

	// 잘못됐으면 FALSE return한다.
	if( !stream )
	{
		MD_SetErrorMessage( "AgcmMap::LoadDWSector 스트림 오픈실패" );
		return FALSE;
	}

	pTexture = LoadRoughTexture( stream );

	RpAtomic * pAtomic = NULL;
	if( pAtomic = RpDWSectorStreamRead( NULL , stream ) )
	{
		if( pTexture )
		{
			// 마고자 2004/04/07
			// 러프맵의 경우 , 텍스쳐 딕셔너리에서 먼저 텍스쳐를 로딩해
			// 두기때문에 딕셔너리 올라갈때 레퍼런스 카운트는 1이돼고
			// 이 섹터 로딩할때 2로 증가하기때문에 , 자동 디스트로이되게
			// 하기 위해서 강제로 한번 디스트로이를 때린다.
			RwTextureDestroy( pTexture );
			pTexture = NULL;
		}

		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->ResetDefaultTexDict();
		// success , do no op

		// Atomic Type 설정한다.
		if( pAtomic )
		{
			//@{ Jaewon 20050615
			// Fix bugs related to ambient occlusion in dungeon.
			INT32	nType = ACUOBJECT_TYPE_WORLDSECTOR;

			AcuObject::SetAtomicType(
				pAtomic							,
				nType							,
				NULL							, // ( INT32 ) pSector				, 문제가 돼려나?..
				NULL							,
				NULL							,
				NULL							,
				NULL							); // 해당 섹터의 포인터를 삽입해둔다
			//@} Jaewon


			// 위치조절..
			{
				FLOAT	fTargetX		= fDstX ;//GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDiv ) ) ) + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;
				FLOAT	fTargetZ		= fDstZ ;//GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDiv ) ) ) + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;

				FLOAT	fOffsetX		= fTargetX - fSrcX;
				FLOAT	fOffsetZ		= fTargetZ - fSrcZ;

				//RwFrame * pFrame = RpAtomicGetFrame( pAtomic );

				RwV3d	vTranslate;
				vTranslate.x	= fOffsetX		;
				vTranslate.y	= fHeightOffset	;
				vTranslate.z	= fOffsetZ		;

				RwMatrix matTranslate;

				RwMatrixTranslate( &matTranslate , &vTranslate , rwCOMBINEREPLACE );
				//RwFrameTransform( pFrame , &matTranslate , rwCOMBINEREPLACE );

				RpMorphTarget	*pMorphTarget;
				pMorphTarget = RpGeometryGetMorphTarget( pAtomic->geometry , 0 );
				RwV3d      * pVertice	= RpMorphTargetGetVertices		( pMorphTarget	);			//	Vertex positions 

				RwV3dTransformPoints( pVertice , pVertice , pAtomic->geometry->numVertices , &matTranslate );
			}
		}
	}
	else
	{
		// 에러 리턴..
		//MD_SetErrorMessage( "AgcmMap::LoadDWSector 섹터 DW 데이타 로드할때 에러 발생 ( %d,%d 섹터 )\n" , pSector->GetIndexX() , pSector->GetIndexZ() );

		return NULL;
	}

	VERIFY( RwStreamClose( stream , NULL	) );

	return pAtomic;
}

BOOL	AgcmMap::CBMTextureRender(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmMap	*		pThis		= ( AgcmMap			* ) pClass		;

	if( pThis && pThis->m_bShowZoneRough )
	{
		vector<RpAtomic *>::iterator Iter;
		RpAtomicCallBackRender	pFunc;
		RpAtomic	* pAtomic;

		for ( Iter = pThis->m_vectorZoneRough.begin( ) ; Iter != pThis->m_vectorZoneRough.end( ) ; Iter++ )
		{
			pAtomic = *Iter;

			pFunc = (RpAtomicCallBackRender)pAtomic->stRenderInfo.backupCB;

			pAtomic->stRenderInfo.shaderUseType		= 0;
			pAtomic->stRenderInfo.beforeLODLevel	= 0;

			if( pFunc )
			{
				pFunc(pAtomic);
			}
		}
	}

	return TRUE;
}

BOOL	AgcmMap::AllocSectorGridInfo( ApWorldSector * pSector )
{
	#ifdef USE_MFC
	RwIm3DVertex	* pImVertex = GetAgcmAttachedData( pSector )->m_pImVertex;

	if( !GetAgcmAttachedData( pSector )->m_bValidGrid )
	{
		int i ;

		FLOAT	fStartX		= pSector->GetXStart();
		FLOAT	fStartZ		= pSector->GetZStart();
		FLOAT	fEndX		= pSector->GetXEnd();
		FLOAT	fEndZ		= pSector->GetZEnd();
		FLOAT	fStepSize	= pSector->GetStepSizeX();

		#define	COLOR_SETTING	RwIm3DVertexSetRGBA	( &pImVertex[ nCount ] , 241 , 251 , 159 , 255  )

		INT32	nCount = 0;
		for( i = 0 ; i <= 16 ; i ++ )
		{
			RwIm3DVertexSetPos	( &pImVertex[ nCount ] , fStartX , pSector->D_GetHeight2( 0 , i ) + 10.0f , fStartZ + fStepSize * ( FLOAT ) i );
			RwIm3DVertexSetU	( &pImVertex[ nCount ] , 1.0f				);    
			RwIm3DVertexSetV	( &pImVertex[ nCount ] , 1.0f				);
			COLOR_SETTING;

			nCount++;
		}

		for( i = 1 ; i <= 16 ; i ++ )
		{
			RwIm3DVertexSetPos	( &pImVertex[ nCount ] , fStartX + fStepSize * ( FLOAT ) i , pSector->D_GetHeight2( i , 16 ) + 10.0f , fEndZ  );
			RwIm3DVertexSetU	( &pImVertex[ nCount ] , 1.0f				);    
			RwIm3DVertexSetV	( &pImVertex[ nCount ] , 1.0f				);
			COLOR_SETTING;

			nCount++;
		}
	
		for( i = 15 ; i >= 0 ; i -- )
		{
			RwIm3DVertexSetPos	( &pImVertex[ nCount ] , fEndX , pSector->D_GetHeight2( 16 , i ) + 10.0f , fStartZ + fStepSize * ( FLOAT ) i );
			RwIm3DVertexSetU	( &pImVertex[ nCount ] , 1.0f				);    
			RwIm3DVertexSetV	( &pImVertex[ nCount ] , 1.0f				);
			COLOR_SETTING;

			nCount++;
		}

		for( i = 15 ; i >= 0 ; i -- )
		{
			RwIm3DVertexSetPos	( &pImVertex[ nCount ] , fStartX + fStepSize * ( FLOAT ) i , pSector->D_GetHeight2( i , 0 ) + 10.0f , fStartZ );
			RwIm3DVertexSetU	( &pImVertex[ nCount ] , 1.0f				);    
			RwIm3DVertexSetV	( &pImVertex[ nCount ] , 1.0f				);
			COLOR_SETTING;

			nCount++;
		}

		GetAgcmAttachedData( pSector )->m_bValidGrid = TRUE;
	}
	#endif // USE_MFC

	return TRUE;
}

void	AgcmMap::RenderSectorGrid( ApWorldSector * pSector )
{
	#ifdef USE_MFC
	// 섹터 주위 테두리.

	AllocSectorGridInfo( pSector );

	// 데이타 없으면 생성
	RwIm3DVertex	* pImVertex = GetAgcmAttachedData( pSector )->m_pImVertex;
	// 선택적으로 덮어 씌우기..

	if( RwIm3DTransform( pImVertex , 65 , NULL, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXRGBA ) )
	{                         
		RwIm3DRenderPrimitive( rwPRIMTYPELINELIST );
		RwIm3DEnd();
	}
	#endif // USE_MFC
}

BOOL	AgcmMap::CBDestroySector_Start	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmMap	*		pThis		= ( AgcmMap			* ) pClass		;
	ApWorldSector *	pSector		= ( ApWorldSector	* ) pData		;

	pThis->EnumCallback(AGCM_CB_ID_ONUNLOAD_MAP, pSector, (PVOID) SECTOR_LOWDETAIL);

	return TRUE;
}
BOOL	AgcmMap::CBDestroySector_End	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmMap	*		pThis		= ( AgcmMap			* ) pClass		;
	ApWorldSector *	pSector		= ( ApWorldSector	* ) pData		;

	// do nothing
	return TRUE;
}
