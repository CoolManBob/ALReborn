#include "ApBase.h"
#include <skeleton.h>
#include "AgcmMap.h"
#include "MagDebug.h"
#include "rtimport.h"
#include "rpcollis.h"
#include "AcuRpMTexture.h"
#include "AcuObject.h"

#include "rtintsec.h"

#include <memory.h>

#include "rpcriter.h"
#include "dwsstream.h"

//#include "acurpdwsector.h"

#include <MagDebug.h>

/*
 * Initialize DWSector Object
 * dwSector : DWSector must be fetched by RpDWSectorGetDetail()
 * numVertices : Number of vertices
 * numPolygons : Number of Polygons
 * numTexCoords : Number of Texture Coordinates
 */
RpDWSector *		AgcmMap::RpDWSectorInit				( RpDWSector *dwSector, RwUInt32 numVertices, RwUInt32 numPolygons, RwUInt8 numTexCoords)
{
	// DWSector 데이타 준비함.
	ASSERT( NULL != dwSector	);
	ASSERT( numVertices		> 0	);
	ASSERT( numPolygons		> 0	);
	ASSERT( numTexCoords	> 0	);
	ASSERT( NULL == dwSector->geometry );

	// 지오메트리 생성..
	dwSector->geometry	= RpGeometryCreate	(
		numVertices ,
		numPolygons	,
		//rpGEOMETRYTRISTRIP					| 
		rpGEOMETRYTEXCOORDSETS(numTexCoords)	|
		rpGEOMETRYNORMALS						|
		rpGEOMETRYPRELIT						|	// 프리라잇 옵션 제거..
		rpGEOMETRYLIGHT							);

	ASSERT( NULL != dwSector->geometry );

	// 아토믹 생성..
	dwSector->atomic	= RpAtomicCreate	();

	ASSERT( NULL != dwSector->atomic );

	// 아토믹에 지오메트리와 프레임 연결함..
	LockFrame();
	if (!RpAtomicSetGeometry	( dwSector->atomic , dwSector->geometry , rpATOMICSAMEBOUNDINGSPHERE )	)
	{
		UnlockFrame();
		return NULL;
	}

	RpAtomicSetFrame		( dwSector->atomic , RwFrameCreate()	);
	RpGeometryDestroy(dwSector->geometry);

	UnlockFrame();

	dwSector->texdict = NULL;

	// 성공..

	// 포인터 설정은 이보다 상위에서 진행한다.
	// 데이타 연결함..
	// AcuObject	acuObject;
	//!acuObject.SetAtomicType( pDWSector->atomic , ACUOBJECT_TYPE_WORLDSECTOR , pSector->GetArrayIndexDWORD() );
		
	return dwSector;
}

/* Get detail object (0 base)*/
RpDWSector *		AgcmMap::RpDWSectorGetDetail		( ApWorldSector * pWorldSector ,  RwUInt8 numDetail )
{
	ASSERT( numDetail >= 0 && numDetail < DWSECTOR_MAX_DETAIL );
	if( numDetail < 0 || numDetail >= DWSECTOR_MAX_DETAIL )
	{
		numDetail = 2;
	}

	// 현재 디테일을 얻어냄..
	DWSectorData * pDWSectorData;

	pDWSectorData = _GetDWSectorData( pWorldSector );

	if (pDWSectorData && pDWSectorData->details)
		return ( pDWSectorData->details + numDetail);
	//else
	//	return NULL;

	return NULL;
}

/* Set current detail index (0 base, one sector) */
RpAtomic *			AgcmMap::RpDWSectorSetCurrentDetail	( ApWorldSector * pWorldSector ,  RwUInt8 numDetail , BOOL bForce )
{
	PROFILE("AgcmMap::RpDWSectorSetCurrentDetail");

	ASSERT( numDetail >= 0 && numDetail < DWSECTOR_MAX_DETAIL );
	ASSERT( NULL != pWorldSector	);
	ASSERT( NULL != m_pWorld		);

	//@{ 2006/07/28 burumal
	if ( NULL == m_pWorld )
		return NULL;
	//@}

	DWSectorData	*	pDWSectorData	;
	RpDWSector		*	pPrevDetail		;
	RpDWSector		*	pCurrentdetail	;
	
	if( !IsPolygonDataLoaded( pWorldSector ) ) return NULL;

	pDWSectorData = _GetDWSectorData( pWorldSector );

	ASSERT( NULL != pDWSectorData								);
	ASSERT( pDWSectorData->curDetail < pDWSectorData->numDetail	);

	if ( numDetail < pDWSectorData->numDetail )
	{
		pPrevDetail		= pDWSectorData->details + pDWSectorData->curDetail	;
		pCurrentdetail	= pDWSectorData->details + numDetail				;

		if ( bForce || pDWSectorData->curDetail != numDetail	)
		{
			// 아토믹 교환 작업..
			if ( m_pWorld && pPrevDetail->atomic && pDWSectorData->curDetail != -1 )
			{
				if (m_pcsAgcmRender)
				{
					VERIFY( m_pcsAgcmRender->RemoveAtomicFromWorld	( pPrevDetail->atomic				)	);
				}
				else
					RpWorldRemoveAtomic						( m_pWorld, pPrevDetail->atomic		);

			}

			if ( m_pWorld && pCurrentdetail->atomic )
			{
				if (m_pcsAgcmRender)
				{
					VERIFY( m_pcsAgcmRender->AddAtomicToWorld		(pCurrentdetail->atomic, ONLY_NONALPHA	) );
				}
				else
					RpWorldAddAtomic						( m_pWorld, pCurrentdetail->atomic	);

				pDWSectorData->curDetail = numDetail;
			}

			pDWSectorData->curDetail = numDetail;
		}
		
		return pCurrentdetail->atomic;

	}
	else return NULL;
}

/* Build world sector collision data */
ApWorldSector *			AgcmMap::RpDWSectorBuildAtomicCollision( ApWorldSector * pWorldSector )
{
	DWSectorData			*	pDWSectorData	;
	RpCollisionBuildParam		param			;
	RwUInt8						curDetail		;
	RwInt8						i				;

	ASSERT( NULL != pWorldSector );
	if( NULL == pWorldSector ) return NULL;

	pDWSectorData	= _GetDWSectorData( pWorldSector )	;

	ASSERT( NULL != pDWSectorData );

	curDetail		= pDWSectorData->curDetail	;

//	param.dummy		= 0							;

	ASSERT( pDWSectorData->numDetail > 0 );

	for ( i = 0; i < pDWSectorData->numDetail ; ++i )
	{
		if( pDWSectorData->details[ i ].geometry )
			RpCollisionGeometryBuildData( pDWSectorData->details[ i ].geometry , &param );
	}

	return pWorldSector;
}

ApWorldSector *		AgcmMap::RpDWSectorDestroyDetail	( ApWorldSector * pWorldSector , RwUInt8 numDetail	)
{
	PROFILE("AgcmMap::RpDWSectorDestroyDetail");

	ASSERT( NULL != pWorldSector );

	if( NULL == pWorldSector ) return NULL;
	ASSERT( numDetail >= 0 && numDetail < DWSECTOR_MAX_DETAIL );

	DWSectorData			*	pDWSectorData	;
	RpDWSector				*	pDetail			;
	RwInt8						curDetail		;

	pDWSectorData	= _GetDWSectorData( pWorldSector )	;

	//ASSERT( NULL != pDWSectorData );
	if( NULL == pDWSectorData ) return NULL;

	//curDetail		= pDWSectorData->curDetail			;
	curDetail		= SECTOR_EMPTY		;

	pDetail = pDWSectorData->details + numDetail;

	if ( !RpDWSectorDestroy( pDetail  ) )
		return NULL;

	return pWorldSector;
}

ApWorldSector *		AgcmMap::RpDWSectorDestroyDetail		( ApWorldSector * pWorldSector)
{
	ASSERT( NULL != pWorldSector );
	DWSectorData			*	pDWSectorData	;
	pDWSectorData	= _GetDWSectorData( pWorldSector )	;
	if( NULL == pDWSectorData ) return pWorldSector;
	for( int i = 0 ; i < pDWSectorData->numDetail ; ++i )
		RpDWSectorDestroyDetail( pWorldSector , i );

	return pWorldSector;
}

INT32			AgcmMap::RpDWSectorGetCurrentDetail	( ApWorldSector * pSector )
{
	DWSectorData			*	pDWSectorData	;
	pDWSectorData	= _GetDWSectorData( pSector )	;

	ASSERT( NULL != pDWSectorData );

	if( pDWSectorData )
	{
		return pDWSectorData->curDetail;
	}
	else
		return SECTOR_ERROR;
}


BOOL				AgcmMap::IsPolygonDataLoaded	( ApWorldSector * pSector )
{
	DWSectorData			*	pDWSectorData	;
	pDWSectorData	= _GetDWSectorData( pSector )	;

	//ASSERT( NULL != pDWSectorData );

	if( pDWSectorData					&&
		pDWSectorData->numDetail >= 0	&&
		pDWSectorData->numDetail <= 3	)
	{
		return TRUE;
	}
	else
		return FALSE;

}

ApWorldSector * AcuGetWorldSectorFromAtomicPointer( RpAtomic * pAtomic )
{
	ASSERT( NULL != pAtomic );
	// 아토믹 유저데이타에서 월드의 정보를 얻어낸다.

	INT32				nIndex		;
	INT32				nType		= AcuObject::GetAtomicType( pAtomic , &nIndex );
	ApWorldSector	*	pWorldSector;

	switch( AcuObject::GetType( nType ) )
	{
	case ACUOBJECT_TYPE_WORLDSECTOR:
		pWorldSector = ( ApWorldSector * ) nIndex;
		return pWorldSector;
	default:
		return NULL;
	}

}

RwTexture *aaa(RwTexture *texture, void *data)
{
	ASSERT(texture->refCount == 1 && RwTextureGetName(texture));

	return texture;
}

BOOL AgcmMap::RpDWSectorDestroy(RpDWSector *dwSector)
{
	PROFILE("AgcmMap::RpDWSectorDestroy");

	ASSERT( NULL != dwSector );

	if( dwSector->atomic )
	{
		RpAtomic *		pstAtomic	= dwSector->atomic;
		RpGeometry *	pstGeometry	= dwSector->geometry;

		dwSector->atomic	= NULL;
		dwSector->geometry	= NULL;

		if( pstGeometry )
			RpCollisionGeometryDestroyData( pstGeometry );

		RpWorld * pWorld;

		pWorld = RpAtomicGetWorld( pstAtomic );

		if (m_pcsAgcmRender)
		{
			// 마고자 (2004-12-02 오후 5:12:10) : 
			// 이거 누가 주석처리해놨찌? -_-
			m_pcsAgcmRender->RemoveAtomicFromWorld	( pstAtomic			);
		}
		else if (pWorld)
			RpWorldRemoveAtomic						( pWorld , pstAtomic	);

		{
			// 마고자 (2003-12-05 오후 6:19:51) : 
			// 콜리전 아토믹 제거.
			RpAtomic		* pAtomicGet;
			INT32			index		;
			AcuObject::GetAtomicType( pstAtomic , & index , NULL , NULL , NULL , (PVOID *) &pAtomicGet );
			
			if( pAtomicGet )
			{
				if (m_pcsAgcmResourceLoader)
					m_pcsAgcmResourceLoader->AddDestroyAtomic( pAtomicGet );
				else
					RpAtomicDestroy( pAtomicGet );
			}
		}

		RwFrame * pFrame = RpAtomicGetFrame( pstAtomic );

		ASSERT( NULL != pFrame );

		LockFrame();

		RpAtomicSetFrame	( pstAtomic , NULL	);
		RwFrameDestroy		( pFrame					);

		UnlockFrame();

		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->AddDestroyAtomic( pstAtomic );
		else
			RpAtomicDestroy		( pstAtomic			);

		if( dwSector->texdict )
		{
			RwTexDictionaryForAllTextures( dwSector->texdict, aaa, NULL );
			RwTexDictionaryDestroy( dwSector->texdict );
		}

		dwSector->atomic	= NULL;
		dwSector->geometry	= NULL;
		dwSector->texdict	= NULL;
	}
	else
	{
		// 빈녀석이다..
		return FALSE;
	}


	return TRUE;
}
