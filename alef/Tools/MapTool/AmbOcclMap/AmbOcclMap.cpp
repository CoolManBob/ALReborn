// -----------------------------------------------------------------------------
//                     _      ____            _ __  __                                  
//     /\             | |    / __ \          | |  \/  |                                 
//    /  \   _ __ ___ | |__ | |  | | ___  ___| | \  / | __ _ _ __       ___ _ __  _ __  
//   / /\ \ | '_ ` _ \| '_ \| |  | |/ __|/ __| | |\/| |/ _` | '_ \     / __| '_ \| '_ \ 
//  / ____ \| | | | | | |_) | |__| | (__| (__| | |  | | (_| | |_) | _ | (__| |_) | |_) |
// /_/    \_\_| |_| |_|_.__/ \____/ \___|\___|_|_|  |_|\__,_| .__/ (_) \___| .__/| .__/ 
//                                                          | |            | |   | |    
//                                                          |_|            |_|   |_|    
//
// Ambient occlusion map toolkit for MapTool
//
// -----------------------------------------------------------------------------
// Originally created on 12/29/2004 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2004, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "../stdafx.h"
#include "../MyEngine.h"
#include <assert.h>
#include "AmbOcclMap.h"
#include <vector>

static 	RwInt32 _RwD3D9RasterExtOffset = 0;
// -----------------------------------------------------------------------------
AmbOcclMap::AmbOcclMap()
		: pWorld_(NULL), superSample_(2)
{
}

// -----------------------------------------------------------------------------
AmbOcclMap::~AmbOcclMap()
{
	destroy();
}

// -----------------------------------------------------------------------------
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
	// set its ambient occlusion texture from objectData->lightMap.
	ASSERT(RpAmbOcclMapAtomicGetAmbOcclMap(atomic));
	if( RpAmbOcclMapAtomicGetAmbOcclMap(atomic) )
	{
		effect->pSharedD3dXEffect->d3dxEffect[nLights]
			//@{ Jaewon 20050420
			// 1 -> effect->textureCount-1 for ambientOcclusionMVL.fx
			->SetTexture(effect->texture[effect->textureCount-1].handle[nLights],
			//@} Jaewon
				RASTEREXTFROMRASTER(RwTextureGetRaster(RpAmbOcclMapAtomicGetAmbOcclMap(atomic)))->texture);
	}
}

#define	AOMMAP_TEXTURE_FORMAT	"%04d%04d"
struct	DivisionCount
{
	UINT32	uDivision	;
	UINT32	uCount		;
	DivisionCount():uDivision(0),uCount(0){}
};

//@{ Jaewon 20050203
// Dungeons should have a different fx applied 
// and be computed by using a ray of finite length.
//@{ Jaewon 2005020
// rayCount parameter added.
//@{ Jaewon 20050712
// recreateAll parameter added.
bool AmbOcclMap::create(RwUInt32 rayCount, float rayLength, bool recreateAll)
//@} Jaewon
//@} Jaewon
//@} Jaewon
{
	vector< DivisionCount >	listDivision;

	if(pWorld_)
		return false;

	_RwD3D9RasterExtOffset = RwRasterGetPluginOffset(rwID_DEVICEMODULE);

	//@{ Jaewon 20050203
	// rayLength
	pWorld_ = gatherAndSetupAtomics(rayLength);
	//@} Jaewon

	RtAmbOcclMapLightingSessionInitialize(&lightingSession_, pWorld_);

	RtAmbOcclMapAmbOcclMapSetDefaultSize(128);

	//@{ Jaewon 20050207
	// 64 -> rayCount
	RtAmbOcclMapSetRayCount(rayCount);
	//@} Jaewon

	//@{ Jaewon 20050408
	// If rayLength is not zero, it should do the special averaging for the indoors.
	if(rayLength)
		_rpAmbOcclMapGlobals.renderStyle |= rpAMBOCCLMAPSTYLEINDOOR;
	else
		_rpAmbOcclMapGlobals.renderStyle &= ~rpAMBOCCLMAPSTYLEINDOOR;
	//@} Jaewon

	AmbOcclMapWorldData  *worldData;
	worldData = RPAMBOCCLMAPWORLDGETDATA(lightingSession_.world);
	worldData->lightMapDensity = 0.02f;
	worldData->flag |= rpAMBOCCLMAPWORLDFLAGAMBOCCLMAP;

	// create maps of the given sizes.
	LtMapSessionInfo sessionInfo;
	_rtLtMapLightingSessionInfoCreate(&sessionInfo, &lightingSession_, TRUE);

	//@{ Jaewon 20050712
	if(!recreateAll)
	// Get base indices for map naming.
	{
		for(int i=0; i<(int)sessionInfo.numAtomics; ++i)
		{
			RpAtomic *atomic = *(RpAtomic **)rwSListGetEntry(sessionInfo.localAtomics, i);
			AmbOcclMapObjectData *objectData = RPAMBOCCLMAPATOMICGETDATA(atomic);

			if((RpAtomicGetFlags(atomic) & rpATOMICRENDER) &&
				(objectData->flags & rtAMBOCCLMAPOBJECTAMBOCCLMAP) &&
				objectData->lightMap)
			{
				RwUInt32 division, count;
				sscanf(RwTextureGetName(objectData->lightMap), AOMMAP_TEXTURE_FORMAT, &division, &count);

				RwUInt32 j;
				for( j=0; j<listDivision.size(); ++j)
				{
					if(listDivision[j].uDivision == division)
						break;
				}

				if(j == listDivision.size())
				{
					DivisionCount	dCount;
					dCount.uDivision = division;
					dCount.uCount = count;
					listDivision.push_back(dCount);
				}
				else
					listDivision[j].uCount = max(listDivision[j].uCount, count);
			}
		}
	}
	//@} Jaewon

	for(int i = 0; i < (int)sessionInfo.numAtomics; ++i)
	{
		RpAtomic *atomic = *(RpAtomic **)rwSListGetEntry(sessionInfo.localAtomics, i);
		RpGeometry *geom = RpAtomicGetGeometry(atomic);
		AmbOcclMapObjectData *objectData = RPAMBOCCLMAPATOMICGETDATA(atomic);

		UINT32	uDivision;
		{
			// 마고자 (2005-06-30 오전 10:44:16) : 
			// make the AOMmap texture file's name with the sector's division index that contain it
			ApWorldSector *pSector = AcuGetWorldSectorFromAtomicPointer( atomic );
			if( pSector )
			{
				uDivision = GetDivisionIndex( pSector->GetArrayIndexX() , pSector->GetArrayIndexZ() );
			}
			else
			{
				ASSERT( NULL != atomic );
				RwFrame		* pFrame	= RpAtomicGetFrame	( atomic );
				ASSERT( NULL != pFrame );
				RwMatrix	* pMatrix	= RwFrameGetLTM		( pFrame );
				ASSERT( NULL != pMatrix );

				uDivision = GetDivisionIndexF( pMatrix->pos.x , pMatrix->pos.z );
			}			
		}

		/* Only lightmap atomics flagged with rpATOMICRENDER and
		* rtAMBOCCLMAPOBJECTAMBOCCLMAP (also, paranoidly check for empty atomics). */
		if((RpAtomicGetFlags(atomic) & rpATOMICRENDER) &&
		  (objectData->flags & rtAMBOCCLMAPOBJECTAMBOCCLMAP) &&
		  (RpGeometryGetNumTriangles(geom) > 0) )
		{
			//@{ Jaewon 20050712
			// For the incremental ambient occlusion computation
			if(objectData->lightMap == NULL || recreateAll)
			//@} Jaewon
			{
				assert(RtAmbOcclMapAtomicGetAmbOcclMapSize(atomic) > 0);
				//@{ Jaewon 20050421
				if(rayLength>0)
					objectData->flags |= rtAMBOCCLMAPOBJECTINDOOR;
				//@} Jaewon
				RpAtomicFxEnable(atomic);
				RpGeometry *pGeometry = RpAtomicGetGeometry(atomic);
				assert(RpGeometryGetVertexTexCoords(pGeometry, (RwTextureCoordinateIndex)2));
				for(int j=0; j<RpGeometryGetNumMaterials(pGeometry); ++j)
				{
					assert(RtAmbOcclMapMaterialGetFlags(RpGeometryGetMaterial(pGeometry, j)) & rtAMBOCCLMAPMATERIALAMBOCCLMAP);

					//@{ Jaewon 20050203
					// ambientOcclusionPPL.fx
					//@{ Jaewon 20050420
					// ambientOcclusionPPL.fx -> ambientOcclusionMVL.fx
					if(RpMaterialD3DFxSetEffect(RpGeometryGetMaterial(pGeometry, j), rayLength>0?"ambientOcclusionMVL.fx":"ambientOcclusion.fx", NULL))
					//@} Jaewon
					//@} Jaewon
					// register fx constant upload callback for the correct rendering 
					// in case of multiple object instances.
						//@{ Jaewon 20050818
						// Ok, it's my bad... If RpMaterialD3DFxSetEffect() returns FALSE, this should not be called.
						RpMaterialD3DFxSetConstantUploadCallBack(RpGeometryGetMaterial(pGeometry, j), fxConstUploadCB);
						//@} Jaewon
				}

				//@{ Jaewon 20050712
				// If there is already one, delete it first.
				if(objectData->lightMap)
				{
					RwTextureDestroy(objectData->lightMap);
					objectData->lightMap = NULL;
				}
				//@} Jaewon

				assert(objectData->sampleMap==NULL);
				RwUInt32 size = RtAmbOcclMapAtomicGetAmbOcclMapSize(atomic);
				objectData->lightMap = RwTextureCreate(RwRasterCreate(size, size, 0, RpAmbOcclMapGetRasterFormat()));

				// 마고자 (2005-06-30 오후 5:15:13) : 
				// 텍스쳐 네이밍 설정..
				{
					UINT32 uOffset;
					for( uOffset = 0 ; uOffset < listDivision.size() ; uOffset++ )
					{
						if( listDivision[ uOffset ].uDivision == uDivision ) break;
					}

					if( uOffset == listDivision.size() )
					{
						// 없다.
						DivisionCount	dCount;
						dCount.uDivision = uDivision;
						listDivision.push_back( dCount );
					}
					else
					{
						// 있다.
						listDivision[ uOffset ].uCount ++;
					}

					wsprintf(
						RwTextureGetName(objectData->lightMap) ,
						AOMMAP_TEXTURE_FORMAT					,
						listDivision[ uOffset ].uDivision		,
						listDivision[ uOffset ].uCount			);
					// _rtLtMapNameGen(RwTextureGetName(pObjectData->lightMap), FALSE);
				}

				//@{ Jaewon 20050110
				objectData->flags |= rtAMBOCCLMAPOBJECTNEEDILLUMINATION;
				//@} Jaewon
				objectData->sampleMap = _rpLtMapSampleMapCreate(size, size);
			}
			//@{ Jaewon 20050712
			// ;)
			else
			{
				assert(RpAtomicFxIsEnabled(atomic));
				RpGeometry *pGeometry = RpAtomicGetGeometry(atomic);
				assert(RpGeometryGetVertexTexCoords(pGeometry, (RwTextureCoordinateIndex)2));
				for(int j=0; j<RpGeometryGetNumMaterials(pGeometry); ++j)
				{
					assert(RtAmbOcclMapMaterialGetFlags(RpGeometryGetMaterial(pGeometry, j)) & rtAMBOCCLMAPMATERIALAMBOCCLMAP);
					assert(RpMaterialD3DFxGetConstantUploadCallBack(RpGeometryGetMaterial(pGeometry, j)));
				}
			}
			//@} Jaewon
		}
	}
	_rtLtMapLightingSessionInfoDestroy(&sessionInfo);

	return true;
}
bool AmbOcclMap::clear()
{
	if(NULL == pWorld_)
		return false;

	//@{ Jaewon 20050110
	// to enable recomputing after clearing
	lightingSession_.startObj = 0;
	//@} Jaewon

	RtAmbOcclMapAmbOcclMapsClear(&lightingSession_, NULL);

	return true;
}
//@{ Jaewon 20050712
// destroyAll parameter added.
bool AmbOcclMap::destroy(bool destroyAll)
//@} Jaewon
{
	if(NULL == pWorld_)
		return false;

	// destroy maps.
	LtMapSessionInfo sessionInfo;
	_rtLtMapLightingSessionInfoCreate(&sessionInfo, &lightingSession_, TRUE);
	for(int i = 0; i < (int)sessionInfo.numAtomics; ++i)
	{
		RpAtomic *atomic = *(RpAtomic **)rwSListGetEntry(sessionInfo.localAtomics, i);
		RpGeometry *geom = RpAtomicGetGeometry(atomic);
		AmbOcclMapObjectData *objectData = RPAMBOCCLMAPATOMICGETDATA(atomic);

		/* Only lightmap atomics flagged with rpATOMICRENDER and
		* rtAMBOCCLMAPOBJECTAMBOCCLMAP (also, paranoidly check for empty atomics). */
		//@{ Jaewon 20050712
		// (objectData->sampleMap || destroyAll) condition added.
		if((RpAtomicGetFlags(atomic) & rpATOMICRENDER) &&
			(objectData->flags & rtAMBOCCLMAPOBJECTAMBOCCLMAP) &&
			(RpGeometryGetNumTriangles(geom) > 0) &&
			(objectData->sampleMap || destroyAll))
		//@} Jaewon
		{
			// turn off the matd3dfx.
			RpAtomicFxDisable(atomic);
			// release matd3dfx resources.
			RpAtomicFxRelease(atomic);
			RpAtomicSetPipeline(atomic, (RxPipeline *)NULL);

			assert(objectData->lightMap && (objectData->sampleMap || destroyAll));
			RwTextureDestroy(objectData->lightMap);
			objectData->lightMap = NULL;
			//@{ Jaewon 20050712
			// In case of (destroyAll=true), 'objectData->sampleMap' can be null.
			if(objectData->sampleMap)
			//@} Jaewon
			{
				_rpLtMapSampleMapDestroy(objectData->sampleMap);
				objectData->sampleMap = NULL;
			}

			//@{ Jaewon 20051006
			RtAmbOcclMapAtomicSetFlags(atomic, RtAmbOcclMapAtomicGetFlags(atomic) 
							& ~rtAMBOCCLMAPOBJECTINDOOR);
			//@} Jaewon
		}

		//@{ Jaewon 20050203
		// Remove the atomic from the ambient occlusion world.
		RpWorldRemoveAtomic(pWorld_, atomic);
		//@} Jaewon
	}
	_rtLtMapLightingSessionInfoDestroy(&sessionInfo);

	//@{ Jaewon 20050630
	// Once it was initialized, it should be deinitialized.
	RtAmbOcclMapLightingSessionDeInitialize(&lightingSession_);
	//@} Jaewon

	if(pWorld_)
	{
		RpWorldDestroy(pWorld_);
		pWorld_ = NULL;
	}

	//@{ Jaewon 20051006
	// Update object data.
	{
		INT32 i = 0;
		ApdObject *pApdObject;
		for(pApdObject = g_pcsApmObject->GetObjectSequence(&i);
			pApdObject;
			pApdObject = g_pcsApmObject->GetObjectSequence(&i))
		{
			AgcdObject *pAgcdObject = g_pcsAgcmObject->GetObjectData(pApdObject);
			if(NULL == pAgcdObject)
				continue;

			fillObjectData(pApdObject);
		}
	}
	//@} Jaewon

	return true;
}
int AmbOcclMap::compute(RtAmbOcclMapIlluminateProgressCallBack progressCB)
{
	if(lightingSession_.totalObj > 0 && lightingSession_.startObj >= lightingSession_.totalObj)
	// illumination complete
		return true;

	if(NULL == pWorld_)
		return false;
	lightingSession_.progressCallBack = progressCB;

	if(lightingSession_.totalObj == 0)
		lightingSession_.startObj = 0;

	lightingSession_.numObj = 1;

	int numObjs = RtAmbOcclMapIlluminate(&lightingSession_, superSample_);

	if(numObjs == -1)
		return -1;
	else
	{
		lightingSession_.startObj += numObjs;
		return numObjs;
	}
}
static RwTexture *saveAmbOcclImageCB(RwTexture *pTexture, void *pData)
{
	char fullpath[MAX_PATH];
	const char *directory = (const char*)pData;

	// increment a refcount for the texture dictionary.
	RwTextureAddRef(pTexture);

	strncpy(fullpath, directory, MAX_PATH-1);
	strncat(fullpath, pTexture->name, MAX_PATH-1);
	strncat(fullpath, ".png", MAX_PATH-1);

	RwRaster *pRaster = RwTextureGetRaster(pTexture);
	RwImage *pAmbOcclMap = RwImageCreate(RwRasterGetWidth(pRaster), RwRasterGetHeight(pRaster), 32);
	assert(pAmbOcclMap);
	pAmbOcclMap = RwImageAllocatePixels(pAmbOcclMap);
	assert(pAmbOcclMap);
	pAmbOcclMap = RwImageSetFromRaster(pAmbOcclMap, pRaster);
	assert(pAmbOcclMap);

	RwImageWrite(pAmbOcclMap, fullpath);

	RwImageDestroy(pAmbOcclMap);

	return pTexture;
}
bool AmbOcclMap::save(const char *directory)
{
	if(NULL == directory)
		return false;

	if(NULL == pWorld_)
		return false;

	// save objects.
	//@{ Jaewon 20051006
	RwInt32 i = 0;
	ApdObject *pApdObject;
	for(pApdObject = g_pcsApmObject->GetObjectSequence(&i);
		pApdObject;
		pApdObject = g_pcsApmObject->GetObjectSequence(&i))
	{
		AgcdObject *pAgcdObject = g_pcsAgcmObject->GetObjectData(pApdObject);
		if(NULL == pAgcdObject)
			continue;

		fillObjectData(pApdObject);
	}
	//@} Jaewon

	// save images.
	RwTexDictionary *pTexDic = RtAmbOcclMapTexDictionaryCreate(&lightingSession_);
	if(NULL == pTexDic)
		return false;

	RwTexDictionaryForAllTextures(pTexDic, saveAmbOcclImageCB, (void*)directory);
	
	RwTexDictionaryDestroy(pTexDic);

	return true;
}

// -----------------------------------------------------------------------------
//@{ Jaewon 20050414
// Process its uv sets properly for the ambient occlusion fx and turn its flag on.
#include "polypack.h"
void AmbOcclMap::prepareForAmbOcclMap(RpAtomic *pAtomic, AmbOcclMapSizeScale scale) const
{
	RtAmbOcclMapLightingSession lightingSession;
	RpWorld *pWorld;

	// Create a dummy world.
	RwBBox bbox;
	bbox.inf.x = bbox.sup.x = 0;
	bbox.inf.y = bbox.sup.y = 0;
	bbox.inf.z = bbox.sup.z = 0;
	pWorld = RpWorldCreate(&bbox);
	RpWorldAddAtomic(pWorld, pAtomic);

	// Initialize a dummy session.
	RtAmbOcclMapLightingSessionInitialize(&lightingSession, pWorld);

	lightingSession.camera = NULL;

	RtAmbOcclMapAmbOcclMapSetDefaultSize(128);

	RtAmbOcclMapSetRayCount(64);

	// Set up the atomic for the ambient occlusion fx.
	RpGeometry *pGeometry = RpAtomicGetGeometry(pAtomic);

	for(RwUInt32 i=0; i< ( RwUInt32 ) RpGeometryGetNumMaterials(pGeometry); ++i)
	{
		RpMaterial *pMaterial = RpGeometryGetMaterial(pGeometry, i);
		RtAmbOcclMapMaterialSetFlags(pMaterial, RtAmbOcclMapMaterialGetFlags(pMaterial)
			| rtAMBOCCLMAPMATERIALAMBOCCLMAP);
	}

	RtAmbOcclMapAtomicSetFlags(pAtomic, (RtAmbOcclMapAtomicGetFlags(pAtomic) 
		| rtAMBOCCLMAPOBJECTAMBOCCLMAP) & ~rtAMBOCCLMAPOBJECTVERTEXLIGHT);

	// Adjust its ambient occlusion map size according to the size of the atomic.
	const RwSphere *pBoundingSphere = RpMorphTargetGetBoundingSphere(RpGeometryGetMorphTarget(pGeometry,0));
	RwUInt32 size = 128;
	if(2 * pBoundingSphere->radius * 0.02f > 192)
		size *= 2;
	else if(2 * pBoundingSphere->radius * 0.02f < 64)
		size /= 2;

	// Adjust the size by a given scale.
	switch(scale)
	{
	case AOMSS_QUATER:
		size /= 4;
		break;
	case AOMSS_HALF:
		size /= 2;
		break;
	case AOMSS_ONE:
		break;
	case AOMSS_DOUBLE:
		size *= 2;
		break;
	case AOMSS_QUADRUPLE:
		size *= 4;
		break;
	default:
		assert(0);
		break;
	}

	RtAmbOcclMapAtomicSetAmbOcclMapSize(pAtomic, size);

	RtAmbOcclMapAmbOcclMapsCreateUVsOnly(&lightingSession, 0.02f);

	// check, check, check!
	assert(RpGeometryGetVertexTexCoords(RpAtomicGetGeometry(pAtomic), (RwTextureCoordinateIndex)2));

	// Clean up a temporary data.
	/* Destroy the PolySet array associated with the geometry */
	AmbOcclMapGeometryData *geometryData = RPAMBOCCLMAPGEOMETRYGETDATA(RpAtomicGetGeometry(pAtomic));
	if(geometryData->PolySetArray != NULL)
	{
		RwInt32 j;

		/* Release the polysets */
		for(j = 0; j < geometryData->numSets; ++j)
		{
			rwSListDestroy(geometryData->PolySetArray[j].members);
		}

		RwFree(geometryData->PolySetArray);
		geometryData->PolySetArray = NULL;
	}


	// Destroy the dummy world.
	RpWorldRemoveAtomic(pWorld, pAtomic);
	RpWorldDestroy(pWorld);
}
//@} Jaewon
//@{ Jaewon 20050203
// rayLength parameter added for dungeons.
RpWorld * AmbOcclMap::gatherAndSetupAtomics(float rayLength)
//@} Jaewon
{
	RwBBox bbox;
	std::vector<RpAtomic*> sectors;
	std::vector<RpAtomic*> objects;

	bool boxUninitialized = true;

	// get terrain atomics.
	ApWorldSector **pSector = g_pcsApmMap->GetCurrentLoadedSectors();

	int i;
	for( i=0; i<( int ) g_pcsApmMap->GetCurrentLoadedSectorCount(); ++i)
	{
		RpDWSector *pDWSector = AGCMMAP_THIS->RpDWSectorGetDetail(pSector[i], SECTOR_HIGHDETAIL);
		//@{ Jaewon 20050926
		// pDWSector->atomic check added.
		if(pDWSector && pDWSector->atomic)
		//@} Jaewon
		{
			sectors.push_back(pDWSector->atomic);
			// update the world bounding box.
			const RwSphere *pSphere = RpAtomicGetBoundingSphere(pDWSector->atomic);
			RwV3d inf, sup;
			RwV3d diag = { 1.0f, 1.0f, 1.0f };
			RwV3dTransformPoints(&inf, & pSphere->center, 1, RwFrameGetLTM(RpAtomicGetFrame(pDWSector->atomic)));
			sup = inf;
			RwV3dIncrementScaled(&inf, &diag, -pSphere->radius);
			RwV3dIncrementScaled(&sup, &diag, pSphere->radius);
			if(boxUninitialized)
			{
				RwBBoxInitialize(&bbox, &inf);
				boxUninitialized = false;
			}
			else
				RwBBoxAddPoint(&bbox, &inf);
			RwBBoxAddPoint(&bbox, &sup);

			//@{ Jaewon 20050414
			// If this is a sector which has been geometry-effect-enabled,
			// it needs special treatments.
			//@{ Jaewon 20050421
			// If it already has got ambient occlusions, skip it.
			//@{ Jaewon 20051006
			// 'rayLength>0' check added.
			// rtAMBOCCLMAPOBJECTAMBOCCLMAP -> rtAMBOCCLMAPOBJECTINDOOR
			if(rayLength>0 && pSector[i]->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE
				&& !(RtAmbOcclMapAtomicGetFlags(pDWSector->atomic) & rtAMBOCCLMAPOBJECTINDOOR))
			//@} Jaewon
			//@} Jaewon
			{
				RwUInt32 numVertices = RpGeometryGetNumVertices(RpAtomicGetGeometry(pDWSector->atomic));
				// An geometry-effect-enabled sector atomic should be always ambient-occlusion-illuminated.
				// So process its uv sets properly for the ambient occlusion fx and turn its flag on.
				prepareForAmbOcclMap(pDWSector->atomic, AOMSS_DOUBLE);
				//@{ Jaewon 20050418
				// The geometry has been recreated, so inform it to the RpDwSector.
				pDWSector->geometry = RpAtomicGetGeometry(pDWSector->atomic);
				assert(numVertices==RpGeometryGetNumVertices(pDWSector->geometry));
				//@} Jaewon
			}
			//@] Jaewon
		}
	}

	// get object atomics.
	i = 0;
	ApdObject *pApdObject;
	for(pApdObject = g_pcsApmObject->GetObjectSequence(&i);
		pApdObject;
		pApdObject = g_pcsApmObject->GetObjectSequence(&i))
	// for all loaded objects,
	{
		AgcdObject *pAgcdObject = g_pcsAgcmObject->GetObjectData(pApdObject);
		if(NULL == pAgcdObject)
			continue;

		AgcmAmbOcclMap::ObjectData *pAmbOcclMapData = g_pcsAgcmAmbOcclMap->getObjectData(pApdObject);

		AgcdObjectGroupData *pAgcdOGD;

		for(int j=0; ; ++j)
		// for each clump,
		{
			pAgcdOGD = g_pcsAgcmObject->GetObjectList()->GetObjectGroup(& pAgcdObject->m_stGroup, j);

			if(NULL == pAgcdOGD)
				break;

			if(pAgcdOGD->m_pstClump)
			{
				//@{ Jaewon 20050302
				RpAtomic *cur_atomic, *end_atomic, *next_atomic;

				if(pAgcdOGD->m_pstClump->atomicList)
				{
					cur_atomic = pAgcdOGD->m_pstClump->atomicList;
					end_atomic = cur_atomic;
					do
					// for each atomic of the clump,
					{
						RpAtomic *pAtomic = cur_atomic;

						next_atomic = cur_atomic->next;

						objects.push_back(pAtomic);
						// update the world bounding box.
						const RwSphere *pSphere = RpAtomicGetBoundingSphere(pAtomic);
						RwV3d inf, sup;
						RwV3d diag = { 1.0f, 1.0f, 1.0f };
						RwV3dTransformPoints(&inf, & pSphere->center, 1, RwFrameGetLTM(RpAtomicGetFrame(pAtomic)));
						sup = inf;
						RwV3dIncrementScaled(&inf, &diag, -pSphere->radius);
						RwV3dIncrementScaled(&sup, &diag, pSphere->radius);
						if(boxUninitialized)
						{
							RwBBoxInitialize(&bbox, &inf);
							boxUninitialized = false;
						}
						else
							RwBBoxAddPoint(&bbox, &inf);
						RwBBoxAddPoint(&bbox, &sup);

						/* Onto the next atomic */
						cur_atomic = next_atomic;
					}
					while(cur_atomic != end_atomic);
				}

				//RwLLLink *cur, *end, *next;

				///* Enumerate all of the atomics in this clump */
				//cur = rwLinkListGetFirstLLLink(& pAgcdOGD->m_pstClump->atomicList);
				//end = rwLinkListGetTerminator(& pAgcdOGD->m_pstClump->atomicList);

				//while(cur != end)
				//// for each atomic of the clump,
				//{
				//	RpAtomic *pAtomic = rwLLLinkGetData(cur, RpAtomic, inClumpLink);

				//	/* Find next now, just in case we destroy the link */
				//	next = rwLLLinkGetNext(cur);

				//	objects.push_back(pAtomic);
				//	// update the world bounding box.
				//	const RwSphere *pSphere = RpAtomicGetBoundingSphere(pAtomic);
				//	RwV3d inf, sup;
				//	RwV3d diag = { 1.0f, 1.0f, 1.0f };
				//	RwV3dTransformPoints(&inf, & pSphere->center, 1, RwFrameGetLTM(RpAtomicGetFrame(pAtomic)));
				//	sup = inf;
				//	RwV3dIncrementScaled(&inf, &diag, -pSphere->radius);
				//	RwV3dIncrementScaled(&sup, &diag, pSphere->radius);
				//	if(boxUninitialized)
				//	{
				//		RwBBoxInitialize(&bbox, &inf);
				//		boxUninitialized = false;
				//	}
				//	else
				//		RwBBoxAddPoint(&bbox, &inf);
				//	RwBBoxAddPoint(&bbox, &sup);

				//	/* Onto the next atomic */
				//	cur = next;
				//}
				//@} Jaewon
			}
		}
	}

	// create a world.
	//@{ Jaewon 20050203
	// If rayLength parameter has been given, use it to a world bounding box.
	RpWorld *pWorld;
	if(0 == rayLength)
		pWorld = RpWorldCreate(&bbox);
	else
	{
		RwBBox rayBox;
		rayBox.inf.x = rayBox.inf.y = rayBox.inf.z = 0;
		rayBox.sup.x = rayBox.sup.y = rayBox.sup.z = rayLength/3.0f;
		pWorld = RpWorldCreate(&rayBox);
	}
	//@} Jaewon

	// set up atomics and add them to the world.
	for(i=0; i< (int) sectors.size(); ++i)
	{
		RpGeometry *pGeometry = RpAtomicGetGeometry(sectors[i]); 
		// build collision data.
		if(pGeometry && !RpCollisionGeometryQueryData(pGeometry))
		{
			RpCollisionGeometryBuildData(pGeometry, NULL);
		}
		// terrain atomics only function as occluders(they have no ambient occlusion map).
		RpWorldAddAtomic(pWorld, sectors[i]);
	}
			
	for(i=0; i< (int) objects.size(); ++i)
	{
		RpGeometry *pGeometry = RpAtomicGetGeometry(objects[i]);
		// build collision data.
		if(pGeometry && !RpCollisionGeometryQueryData(pGeometry))
		{
			RpCollisionGeometryBuildData(pGeometry, NULL);
		}

		RpWorldAddAtomic(pWorld, objects[i]);
	}

	return pWorld;
}

// -----------------------------------------------------------------------------
void AmbOcclMap::fillObjectData(ApdObject* pApdObject)
{
	AgcdObject *pAgcdObject = g_pcsAgcmObject->GetObjectData(pApdObject);
	if(NULL == pAgcdObject)
		return;

	AgcmAmbOcclMap::ObjectData *pAmbOcclMapData = g_pcsAgcmAmbOcclMap->getObjectData(pApdObject);

	//@{ Jaewon 20050712
	// ;)
	bool needUpdate = false;
	//@} Jaewon
	
	//@{ Jaewon 20050211
	// Get the number of atomics.
	//@{ Jaewon 20050713
	// ;)
	RwUInt32 atomicCount = 0;
	//@} Jaewon

	AgcdObjectGroupData *pAgcdOGD;

	for(int j=0; ; ++j)
	// for each clump,
	{
		pAgcdOGD = g_pcsAgcmObject->GetObjectList()->GetObjectGroup(& pAgcdObject->m_stGroup, j);

		if(NULL == pAgcdOGD)
			break;

		if(pAgcdOGD->m_pstClump)
		{
			//@{ Jaewon 20050302
			RpAtomic *cur_atomic, *end_atomic, *next_atomic;

			if(pAgcdOGD->m_pstClump->atomicList)
			{
				cur_atomic = pAgcdOGD->m_pstClump->atomicList;
				end_atomic = cur_atomic;
				do
				// for each atomic of the clump,
				{
					RpAtomic *pAtomic = cur_atomic;

					next_atomic = cur_atomic->next;

					//@{ Jaewon 20050713
					// ;)
					atomicCount += 1;
					//@} Jaewon

					//@{ Jaewon 20050712
					// If there is any atomic which has been (re)calculated,
					// this object data should be updated.
					if(RPAMBOCCLMAPATOMICGETDATA(cur_atomic)->sampleMap)
						needUpdate = true;
					//@} Jaewon

					/* Onto the next atomic */
					cur_atomic = next_atomic;
				}
				while(cur_atomic != end_atomic);
			}

			//RwLLLink *cur, *end, *next;

			///* Enumerate all of the atomics in this clump */
			//cur = rwLinkListGetFirstLLLink(& pAgcdOGD->m_pstClump->atomicList);
			//end = rwLinkListGetTerminator(& pAgcdOGD->m_pstClump->atomicList);

			//while(cur != end)
			//// for each atomic of the clump,
			//{
			//	RpAtomic *pAtomic = rwLLLinkGetData(cur, RpAtomic, inClumpLink);

			//	/* Find next now, just in case we destroy the link */
			//	next = rwLLLinkGetNext(cur);

			//	pAmbOcclMapData->atomicCount_ += 1;

			//	/* Onto the next atomic */
			//	cur = next;
			//}
			//@} Jaewon
		}
	}
	//@} Jaewon

	//@{ Jaewon 20050712
	// 'needUpdate' condition added.
	//@{ Jaewon 20050713
	// ;)
	if(atomicCount == 0
	//@} Jaewon
	|| !needUpdate)
	//@} Jaewon
		return;

	//@{ Jaewon 20050713
	// ;)
	pAmbOcclMapData->atomicCount_ = atomicCount;
	//@} Jaewon

	//@{ Jaewon 20050712
	// Stop leaks!
	//@{ Jaewon 20050721
	// Introduce 'AtomicData' & the memory pool.
	g_pcsAgcmAmbOcclMap->freeAtomicData(pAmbOcclMapData->atomicData_);
	//@} Jaewon
	//@} Jaewon

	// malloc arrays of the ObjectData.
	//@{ Jaewon 20050721
	// Introduce 'AtomicData' & the memory pool.
	pAmbOcclMapData->atomicData_ = g_pcsAgcmAmbOcclMap->allocAtomicData(atomicCount);
	//@} Jaewon

	//@{ Jaewon 20050721
	// Introduce 'AtomicData'.
	AgcmAmbOcclMap::AtomicData *atomicData = pAmbOcclMapData->atomicData_;
	//@} Jaewon
	for(int j=0; ; ++j)
	// for each clump,
	{
		pAgcdOGD = g_pcsAgcmObject->GetObjectList()->GetObjectGroup(& pAgcdObject->m_stGroup, j);

		if(NULL == pAgcdOGD)
			break;

		if(pAgcdOGD->m_pstClump)
		{
			//@{ Jaewon 20050302
			RpAtomic *cur_atomic, *end_atomic, *next_atomic;

			if(pAgcdOGD->m_pstClump->atomicList)
			{
				cur_atomic = pAgcdOGD->m_pstClump->atomicList;
				end_atomic = cur_atomic;
				do
				// for each atomic of the clump,
				{
					RpAtomic *pAtomic = cur_atomic;

					next_atomic = cur_atomic->next;

					//@{ Jaewon 20050721
					// Introduce 'AtomicData'.
					atomicData->atomicId_ = RpAtomicGetId(pAtomic) | (j<<24);
					//@{ Jaewon 20050712
					// Do not save an atomic of which computation has not be finished.
					if((RtAmbOcclMapAtomicGetFlags(pAtomic) & rtAMBOCCLMAPOBJECTAMBOCCLMAP)
						&& !(RtAmbOcclMapAtomicGetFlags(pAtomic) & rtAMBOCCLMAPOBJECTNEEDILLUMINATION))
					{
						atomicData->flags_ = RtAmbOcclMapAtomicGetFlags(pAtomic);
						RwTexture* pTexture = RpAmbOcclMapAtomicGetAmbOcclMap(pAtomic);
						assert(pTexture);
						atomicData->mapSize_ = RwRasterGetWidth(RwTextureGetRaster(pTexture));
						strncpy(atomicData->mapName_, RwTextureGetName(pTexture), 32-1);
					}
					else
					{
						atomicData->flags_ = 0;
						atomicData->mapSize_ = 0;
						atomicData->mapName_[0] = '\0';
					}
					//@} Jaewon
					//@} Jaewon

					//@{ Jaewon 20050721
					// Introduce 'AtomicData'.
					atomicData = atomicData->next_;
					//@} Jaewon

					/* Onto the next atomic */
					cur_atomic = next_atomic;
				}
				while(cur_atomic != end_atomic);
			}

			//RwLLLink *cur, *end, *next;

			///* Enumerate all of the atomics in this clump */
			//cur = rwLinkListGetFirstLLLink(& pAgcdOGD->m_pstClump->atomicList);
			//end = rwLinkListGetTerminator(& pAgcdOGD->m_pstClump->atomicList);

			//while(cur != end)
			//// for each atomic of the clump,
			//{
			//	RpAtomic *pAtomic = rwLLLinkGetData(cur, RpAtomic, inClumpLink);

			//	/* Find next now, just in case we destroy the link */
			//	next = rwLLLinkGetNext(cur);

			//	pAmbOcclMapData->atomicId_[i] = RpAtomicGetId(pAtomic) | (j<<24);
			//	pAmbOcclMapData->flags_[i] = RtAmbOcclMapAtomicGetFlags(pAtomic);
			//	if(pAmbOcclMapData->flags_[i] & rtAMBOCCLMAPOBJECTAMBOCCLMAP)
			//	{
			//		RwTexture* pTexture = RpAmbOcclMapAtomicGetAmbOcclMap(pAtomic);
			//		assert(pTexture);
			//		pAmbOcclMapData->mapSize_[i] = RwRasterGetWidth(RwTextureGetRaster(pTexture));
			//		strncpy(pAmbOcclMapData->mapName_[i], RwTextureGetName(pTexture), 32-1);
			//	}
			//	else
			//	{
			//		pAmbOcclMapData->mapSize_[i] = 0;
			//		pAmbOcclMapData->mapName_[i][0] = '\0';
			//	}

			//	++i;

			//	/* Onto the next atomic */
			//	cur = next;
			//}
			//@} Jaewon
		}
	}
}

// -----------------------------------------------------------------------------
// AmbOcclMap.cpp - End of file
// -----------------------------------------------------------------------------
