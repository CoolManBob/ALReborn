#include <skeleton.h>
#include "AgcmMap.h"
#include "MagDebug.h"
#include "rtimport.h"
#include "rpcollis.h"
#include "AcuRpMTexture.h"
#include "AcuObject.h"

#include "rtintsec.h"
#include "rtpick.h"
#include "DWSStream.h"
#include "MagPackManager.h"
#include "MagUnpackManager.h"

#include "RtPITexD.h"

#include "AuRwImage.h"
#include "AcuFrameMemory.h"

#include "RtRay.h"

RtWorldImport *	AgcmMap::__MakeDetailWorldImport	( ApWorldSector * pSector , int nDetail )
{

	// 섹터에 데이타 넣기.
	ASSERT( NULL != m_pcsApmMap										);
	ASSERT( NULL != pSector											);
	ASSERT( SECTOR_EMPTY <= nDetail && nDetail < SECTOR_DETAILDEPTH	);

	ApDetailSectorInfo	* pToolSectorInfo	= pSector->GetDetailSectorInfo()		;
	if( pToolSectorInfo == NULL )
	{
		TRACE( "툴용 지형 저보가 없습니다.\n" );
		return NULL;
	}

	int					nDepth			= pToolSectorInfo->m_nDepth			[ nDetail ];
	ApDetailSegment *	pDetailSegment	= pToolSectorInfo->m_pSegment		[ nDetail ];
	AcMaterialList *	pMaterialList	= GetMaterialList( pSector );

	//ASSERT( NULL != pDetailSegment			);
	if( NULL == pDetailSegment ) return NULL;

	RtWorldImport			*	pWorldImport;
	RtWorldImportTriangle	*	pTriangles	;
	RtWorldImportVertex		*	pVertices	;

	pWorldImport = RtWorldImportCreate();
	
	if( pWorldImport == NULL )
    {
        return NULL;
    }

	switch( nDetail )
	{
	case SECTOR_EMPTY		:
		{
			// 만들어져있는데 왜 --;
		}
		break;
	case SECTOR_LOWDETAIL	:
	case SECTOR_HIGHDETAIL	:
		{
			if( nDepth == 0 ) break;

			RpDWSector		* pDWSector	;

			RwInt32					matIndex	;
			RwInt32					i			;

			pDWSector	= RpDWSectorGetDetail( pSector , nDetail );
			
			RtWorldImportAddNumVertices		( pWorldImport, pDWSector->geometry->numVertices	);
			RtWorldImportAddNumTriangles	( pWorldImport, pDWSector->geometry->numTriangles	);

			pVertices	= RtWorldImportGetVertices	( pWorldImport	);
			pTriangles	= RtWorldImportGetTriangles	( pWorldImport	);

			memset( pVertices	, 0 , sizeof(	RtWorldImportVertex		) * pDWSector->geometry->numVertices	);
			memset( pTriangles	, 0 , sizeof(	RtWorldImportTriangle	) * pDWSector->geometry->numTriangles	);

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

			// 머티리얼 리스트 카피..
			AcMaterialList::MaterialData	* pMatData;
			AuNode< AcMaterialList::MaterialData > * pNode = pMaterialList[ nDetail ].list.GetHeadNode();
			while( pNode )
			{
				pMatData	= &pNode->GetData();
				matIndex	= RtWorldImportAddMaterial	( pWorldImport , pMatData->pMateterial	);

				ASSERT( matIndex == pMatData->matindex );

				pNode = pNode->GetNextNode();
			}
			
			RpTriangle	*	pCurrentTriangle;
			INT32			nVertex			;

			for( int nTriangle = 0 ; nTriangle < pDWSector->geometry->numTriangles ; ++ nTriangle )
			{
				pCurrentTriangle = ( polygons + nTriangle );
				( pTriangles + nTriangle )->matIndex = pCurrentTriangle->matIndex;

				for( i = 0 ; i < 3 ; ++ i )
				{
					( pTriangles + nTriangle )->vertIndex[ i ]	= pCurrentTriangle->vertIndex[ i ];

					nVertex = pCurrentTriangle->vertIndex[ i ];

					( pVertices + nVertex )->OC.x				= ( vertices + nVertex	)->x		;
					( pVertices + nVertex )->OC.y				= ( vertices + nVertex	)->y		;
					( pVertices + nVertex )->OC.z				= ( vertices + nVertex	)->z		;

					( pVertices + nVertex )->normal.x			= ( normals + nVertex	)->x		;
					( pVertices + nVertex )->normal.y			= ( normals + nVertex	)->y		;
					( pVertices + nVertex )->normal.z			= ( normals + nVertex	)->z		;

					( pVertices + nVertex )->texCoords[ 0 ].u	= ( texCoords[ i ] + nVertex )->u	;
					( pVertices + nVertex )->texCoords[ 0 ].v	= ( texCoords[ i ] + nVertex )->v	;
					( pVertices + nVertex )->texCoords[ 1 ].u	= ( texCoords[ i ] + nVertex )->u	;
					( pVertices + nVertex )->texCoords[ 1 ].v	= ( texCoords[ i ] + nVertex )->v	;
					( pVertices + nVertex )->texCoords[ 2 ].u	= ( texCoords[ i ] + nVertex )->u	;
					( pVertices + nVertex )->texCoords[ 2 ].v	= ( texCoords[ i ] + nVertex )->v	;
					( pVertices + nVertex )->texCoords[ 3 ].u	= ( texCoords[ i ] + nVertex )->u	;
					( pVertices + nVertex )->texCoords[ 3 ].v	= ( texCoords[ i ] + nVertex )->v	;

					( pVertices + nVertex )->preLitCol.red		= ( preLitLum + nVertex	)->red		;
					( pVertices + nVertex )->preLitCol.green	= ( preLitLum + nVertex	)->green	;
					( pVertices + nVertex )->preLitCol.blue		= ( preLitLum + nVertex	)->blue		;
					( pVertices + nVertex )->preLitCol.alpha	= ( preLitLum + nVertex	)->alpha	;

					( pVertices + nVertex )->matIndex			= pCurrentTriangle->matIndex		;
				}
			}
		}
		break;
	}

	return pWorldImport;
}

RpAtomic * AgcmMap::MakeDetailWorldAtomic	( ApWorldSector * pSector , int nDetail )
{
    RtWorldImport			*	pWorldImport	;
    RtWorldImportParameters		params			;
	RpGeometry				*	pGeometry		;
	RpAtomic				*	pAtomic			= NULL	;

    pWorldImport = __MakeDetailWorldImport( pSector , nDetail );
    if( pWorldImport == NULL )
    {
        return NULL;
    }

    //RtWorldImportParametersInitialize(&params);
    RtWorldImportParametersInit(&params);

    params.flags					= rpWORLDTEXTURED | rpWORLDNORMALS | rpWORLDPRELIT | rpWORLDTEXTURED2 | rpWORLDMODULATEMATERIALCOLOR;
    params.conditionGeometry		= FALSE	;
    params.calcNormals				= FALSE	;
	params.numTexCoordSets			= 4		; // 멀티텍스쳐 지원.
	params.maxWorldSectorPolygons	= 2		;
	params.maxOverlapPercent		= 1		;

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
}
