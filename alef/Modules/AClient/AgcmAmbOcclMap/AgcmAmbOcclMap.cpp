// -----------------------------------------------------------------------------
//                                                   _      ____            _ __  __                                  
//     /\                            /\             | |    / __ \          | |  \/  |                                 
//    /  \    __ _  ___ _ __ ___    /  \   _ __ ___ | |__ | |  | | ___  ___| | \  / | __ _ _ __       ___ _ __  _ __  
//   / /\ \  / _` |/ __| '_ ` _ \  / /\ \ | '_ ` _ \| '_ \| |  | |/ __|/ __| | |\/| |/ _` | '_ \     / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| | | | | |/ ____ \| | | | | | |_) | |__| | (__| (__| | |  | | (_| | |_) | _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|_| |_| |_/_/    \_\_| |_| |_|_.__/ \____/ \___|\___|_|_|  |_|\__,_| .__/ (_) \___| .__/| .__/ 
//            __/ |                                                                       | |            | |   | |    
//           |___/                                                                        |_|            |_|   |_|    
//
// Archlord object extension for saving ambient occlusion map infos
//
// -----------------------------------------------------------------------------
// Originally created on 12/21/2004 by Jaewon Jung
// Author's homepage - http://3dengine.org/
//
// Copyright 2004, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "rwcore.h"
#include "AgcmAmbOcclMap.h"
#include "ApModuleStream.h"

static RwInt32 _RwD3D9RasterExtOffset=0;

// -----------------------------------------------------------------------------
AgcmAmbOcclMap::AgcmAmbOcclMap()
			: pApmObject_(NULL), pAgcmObject_(NULL), objectAttachIndex_(-1)
{
	SetModuleName("AgcmAmbOcclMap");
}

// -----------------------------------------------------------------------------
AgcmAmbOcclMap::~AgcmAmbOcclMap()
{
}

// -----------------------------------------------------------------------------
BOOL AgcmAmbOcclMap::OnAddModule()
{
	_RwD3D9RasterExtOffset = RwRasterGetPluginOffset(rwID_DEVICEMODULE);

	// get a pointer to ApmObject(grandparent module).
	pApmObject_ = (ApmObject*)GetModule("ApmObject");
	if(NULL == pApmObject_)
		return FALSE;

	// get a pointer to AgcmObject(parent module).
	pAgcmObject_ = (AgcmObject*)GetModule("AgcmObject");
	if(NULL == pAgcmObject_)
		return FALSE;

	// attach data to the ApmObject.
	objectAttachIndex_ = pApmObject_->AttachObjectData(this, sizeof(ObjectData), objectConstructor, objectDestructor);

	// register callbacks.
	if(NULL == pApmObject_->AddStreamCallback(APMOBJECT_DATA_OBJECT, objectStreamReadCB, objectStreamWriteCB, this))
		return FALSE;

	//@{ Jaewon 20050713
	// pApmObject_ -> pAgcmObject_
	// You should use 'AgcmObject' version in order to ensure that the clump loading had been completed.
	if(NULL == pAgcmObject_->SetCallbackInitObject(initObjectCB, this))
	//@} Jaewon
		return FALSE;

	return TRUE;
}

BOOL AgcmAmbOcclMap::OnInit()
{
	//@{ Jaewon 20050721
	// ;)
	objectDataMemPool_.Initialize(sizeof(AtomicData), 10000);
	//@} Jaewon

	return TRUE;
}

BOOL AgcmAmbOcclMap::OnDestroy()
{
	return TRUE;
}

// -----------------------------------------------------------------------------
AgcmAmbOcclMap::ObjectData* AgcmAmbOcclMap::getObjectData(ApdObject *pApdObject)
{
	return (ObjectData*) pApmObject_->GetAttachedModuleData(objectAttachIndex_, pApdObject);
}

// -----------------------------------------------------------------------------
BOOL AgcmAmbOcclMap::objectStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgcmAmbOcclMap *pThis = (AgcmAmbOcclMap*) pClass;
	ApdObject *pApdObject = (ApdObject*) pData;
	ObjectData *pAmbOcclMapData = pThis->getObjectData(pApdObject);

	if(NULL == pStream->WriteValue(AGCMAMBOCCLMAP_INI_NAME_ATOMIC_COUNT, (int) pAmbOcclMapData->atomicCount_))
		return FALSE;

	CHAR buf[256];
	//@{ Jaewon 20050721
	// Introduce 'AtomicData'.
	AtomicData *atomicData = pAmbOcclMapData->atomicData_;
	for(int i=0; i<(int) pAmbOcclMapData->atomicCount_; ++i)
	{
		sprintf(buf, "%s%d", AGCMAMBOCCLMAP_INI_NAME_ATOMIC_ID, i);
		if(NULL == pStream->WriteValue(buf, (int) atomicData->atomicId_))
			return FALSE;

		sprintf(buf, "%s%d", AGCMAMBOCCLMAP_INI_NAME_OBJECT_FLAGS, i);
		if(NULL == pStream->WriteValue(buf, (int) atomicData->flags_))
			return FALSE;

		if(atomicData->flags_ & rtAMBOCCLMAPOBJECTAMBOCCLMAP)
		{
			sprintf(buf, "%s%d", AGCMAMBOCCLMAP_INI_NAME_MAP_SIZE, i);
			if(NULL == pStream->WriteValue(buf, (int) atomicData->mapSize_))
				return FALSE;

			sprintf(buf, "%s%d", AGCMAMBOCCLMAP_INI_NAME_MAP_NAME, i);
			if(NULL == pStream->WriteValue(buf, atomicData->mapName_))
				return FALSE;
		}

		atomicData = atomicData->next_;
	}
	//@} Jaewon

	return TRUE;
}

BOOL AgcmAmbOcclMap::objectStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgcmAmbOcclMap *pThis = (AgcmAmbOcclMap*) pClass;
	ApdObject *pApdObject = (ApdObject*) pData;
	ObjectData *pAmbOcclMapData = pThis->getObjectData(pApdObject);

	//@{ Jaewon 20050721
	// Introduce 'AtomicData' & the memory pool.
	AtomicData *atomicData;
	pStream->ReadNextValue();
	const CHAR *name = pStream->GetValueName();
	ASSERT(!strcmp(name, AGCMAMBOCCLMAP_INI_NAME_ATOMIC_COUNT));
	pStream->GetValue((int*)& pAmbOcclMapData->atomicCount_);

	pAmbOcclMapData->atomicData_ = pThis->allocAtomicData(pAmbOcclMapData->atomicCount_);

	CHAR buf[256] = "";
	atomicData = pAmbOcclMapData->atomicData_;
	for(RwUInt32 i=0; i<pAmbOcclMapData->atomicCount_; ++i)
	{
		pStream->ReadNextValue();
		name = pStream->GetValueName();
		ASSERT(sprintf(buf, "%s%d", AGCMAMBOCCLMAP_INI_NAME_ATOMIC_ID, i)
			&& !strcmp(name, buf));
		pStream->GetValue((int*)& atomicData->atomicId_);

		pStream->ReadNextValue();
		name = pStream->GetValueName();
		ASSERT(sprintf(buf, "%s%d", AGCMAMBOCCLMAP_INI_NAME_OBJECT_FLAGS, i)
			&& !strcmp(name, buf));
		pStream->GetValue((int*)& atomicData->flags_);
		
		if(atomicData->flags_ & rtAMBOCCLMAPOBJECTAMBOCCLMAP)
		{
			pStream->ReadNextValue();
			name = pStream->GetValueName();
			ASSERT(sprintf(buf, "%s%d", AGCMAMBOCCLMAP_INI_NAME_MAP_SIZE, i)
				&& !strcmp(name, buf));
			pStream->GetValue((int*)& atomicData->mapSize_);

			pStream->ReadNextValue();
			name = pStream->GetValueName();
			ASSERT(sprintf(buf, "%s%d", AGCMAMBOCCLMAP_INI_NAME_MAP_NAME, i)
				&& !strcmp(name, buf));
			pStream->GetValue(atomicData->mapName_, 32);
		}

		atomicData = atomicData->next_;
	}
	//@} Jaewon

	return TRUE;
}

// -----------------------------------------------------------------------------
BOOL AgcmAmbOcclMap::objectDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmAmbOcclMap *pThis = (AgcmAmbOcclMap*) pClass;
	ApdObject *pApdObject = (ApdObject*) pData;
	ObjectData *pAmbOcclMapData = pThis->getObjectData(pApdObject);

	//@{ Jaewon 20050721
	// Introduce 'AtomicData' & the memory pool.
	pThis->freeAtomicData(pAmbOcclMapData->atomicData_);
	pAmbOcclMapData->atomicCount_ = 0;
	pAmbOcclMapData->atomicData_ = NULL;
	//@} Jaewon

	return TRUE;
}

BOOL AgcmAmbOcclMap::objectConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmAmbOcclMap *pThis = (AgcmAmbOcclMap*) pClass;
	ApdObject *pApdObject = (ApdObject*) pData;
	ObjectData *pAmbOcclMapData = pThis->getObjectData(pApdObject);

	//@{ Jaewon 20050721
	// Introduce 'AtomicData'.
	pAmbOcclMapData->atomicCount_ = 0;
	pAmbOcclMapData->atomicData_ = NULL;
	//@} Jaewon

	return TRUE;
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
	else
	{
		// do nothing.
		// AOMMap 텍스쳐가 설정은 돼어있는데 실제로 업는경우 발생.
	}
}
BOOL AgcmAmbOcclMap::initObjectCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmAmbOcclMap *pThis = (AgcmAmbOcclMap*) pClass;
	ApdObject *pApdObject = (ApdObject*) pData;
	ObjectData *pAmbOcclMapData = pThis->getObjectData(pApdObject);

	//@{ Jaewon 20050721
	// Introduce 'AtomicData'.
	AtomicData *atomicData = pAmbOcclMapData->atomicData_;
	for(int i=0; i< (int) pAmbOcclMapData->atomicCount_; ++i)
	{
		//@{ Jaewon 20050819
		// A stopgap measure...-- not good!!!
//@{ Jaewon 20051017
// Do not apply the stopgap measure to MapTool.
#ifndef USE_MFC
		if(0)//atomicData->flags_ & rtAMBOCCLMAPOBJECTAMBOCCLMAP)
#else
		if(atomicData->flags_ & rtAMBOCCLMAPOBJECTAMBOCCLMAP)
#endif
//@} Jaewon
		//@} Jaewon
		{
			// search clumps for an atomic of the specified id.
			RpAtomic *pAtomic = pThis->searchAtomicById(pApdObject, atomicData->atomicId_);
			ASSERT(pAtomic);

			RwTexture *texture = NULL;
			ASSERT(strcmp(atomicData->mapName_, ""));
			if(strcmp(atomicData->mapName_, ""))
				// load a ambient occlusion map.
			{
				texture = RwTextureRead(atomicData->mapName_, NULL);
				ASSERT( texture && atomicData->mapSize_ == RwRasterGetWidth(RwTextureGetRaster(texture)));
			}

			if(NULL == texture)
				// create a dummy checkerboard texture for debug purposes.
			{
				ASSERT(atomicData->mapSize_ > 0);
				texture = RwTextureCreate(RwRasterCreate(atomicData->mapSize_, 
					atomicData->mapSize_, 0, 
					RpAmbOcclMapGetRasterFormat()));
				ASSERT(texture);
				RwTextureSetAddressing(texture, rwTEXTUREADDRESSWRAP);
				RwTextureSetFilterMode(texture, rwFILTERLINEARMIPLINEAR);
				RwTextureSetName(texture, atomicData->mapName_);

				// AOMMap 텍스쳐가 설정은 돼어있는데 실제로 업는경우 발생.
				MD_SetErrorMessage( "'%s' 오브젝트 %d번 읽는도중 AOMMap 텍스쳐를 발견하지 못했습니다.\n" ,
					pApdObject->m_pcsTemplate ? pApdObject->m_pcsTemplate->m_szName	: "Unknown"	,
					pApdObject->m_lID															);
			}

			// register it to the AmbOcclMap plugin.
			pAtomic = RpAmbOcclMapAtomicSetAmbOcclMap(pAtomic, texture);
			ASSERT(pAtomic);

			//@{ Jaewon 20041230
			//@{ Jaewon 20050421
			// rtAMBOCCLMAPOBJECTAMBOCCLMAP -> pAmbOcclMapData->flags_[i]
			RtAmbOcclMapAtomicSetFlags(pAtomic, atomicData->flags_);
			RwChar *fxName;
			if(atomicData->flags_ & rtAMBOCCLMAPOBJECTINDOOR)
				fxName = "ambientOcclusionMVL.fx";
			else
				fxName = "ambientOcclusion.fx";
			//@} Jaewon
			//@} Jaewon

			// set the pipe & register the shader constant upload callback for the ambient occlusion map.
			RpGeometry *pGeometry = RpAtomicGetGeometry(pAtomic);
			if(pGeometry)
			{
				ASSERT(NULL == RpSkinGeometryGetSkin(pGeometry));
				RpAtomicFxEnable(pAtomic);
				for(int j=0; j<RpGeometryGetNumMaterials(pGeometry); ++j)
				{	
					//@{ Jaewon 20041230
					RtAmbOcclMapMaterialSetFlags(RpGeometryGetMaterial(pGeometry, j),
						rtAMBOCCLMAPMATERIALAMBOCCLMAP);
					//@} Jaewon
					//@{ Jaewon 20050421
					// "ambientOcclusion.fx" -> fxName
					if(RpMaterialD3DFxSetEffect(RpGeometryGetMaterial(pGeometry, j), fxName, NULL))
					//@} Jaewon
						//@{ Jaewon 20050818
						// Ok, it's my bad... If RpMaterialD3DFxSetEffect() returns FALSE, this should not be called.
						RpMaterialD3DFxSetConstantUploadCallBack(RpGeometryGetMaterial(pGeometry, j), fxConstUploadCB);
						//@} Jaewon
				}
			}

			RwTextureDestroy(texture);
			texture = NULL;
		}

		atomicData = atomicData->next_;
	}
	//@} Jaewon

	return TRUE;
}

// -----------------------------------------------------------------------------
static RpAtomic *searchAtomicCB(RpAtomic *pAtomic, void *pData)
{
	struct Param
	{
		int id_;
		RpAtomic *result_;
	};

	Param *pParam = (Param*)pData;

	if(RpAtomicGetId(pAtomic) == pParam->id_)
	{
		pParam->result_ = pAtomic;
		return NULL;
	}
	else
		return pAtomic;
}
RpAtomic *AgcmAmbOcclMap::searchAtomicById(ApdObject *pApdObject, int id)
{
	//@{ Jaewon 20050110
	// temporarily granted...
	//if(id < 0)
	//	return NULL;
	//@} Jaewon

	//@{ Jaewon 20050302
	// Most significant 8 bits are used for a clump id.
	int clumpId = (id & 0xff000000) >> 24;
	id &= 0x00ffffff;
	//@} Jaewon

	struct Param
	{
		int id_;
		RpAtomic *result_;
	};

	AgcdObject *pAgcdObject = pAgcmObject_->GetObjectData(pApdObject);
	if(NULL == pAgcdObject)
		return NULL;

	AgcdObjectGroupData *pAgcdOGD;

	for(int i=0; ; ++i)
	{
		pAgcdOGD = pAgcmObject_->GetObjectList()->GetObjectGroup(& pAgcdObject->m_stGroup, i);

		if(NULL == pAgcdOGD)
			return NULL;

		if(pAgcdOGD->m_pstClump)
		{
			//@{ Jaewon 20050302
			// Check the clump id.
			if(i == clumpId)
			//@} Jaewon
			{
				Param param;
				param.id_ = id;
				param.result_ = NULL;
				RpClumpForAllAtomics(pAgcdOGD->m_pstClump, searchAtomicCB, (void*)&param);

				if(param.result_)
					return param.result_;
			}
		}
	}

	ASSERT(NULL);
	return NULL;
}

//@{ Jaewon 20050721
// Introduce 'AtomicData' & the memory pool.
AgcmAmbOcclMap::AtomicData *AgcmAmbOcclMap::allocAtomicData(RwUInt32 count)
{
	AtomicData *result = NULL, *atomicData;
	for(RwUInt32 i=0; i<count; ++i)
	{
		atomicData = (AtomicData*)objectDataMemPool_.Alloc();
		atomicData->next_ = result;
		result = atomicData;
	}

	return result;
}
void AgcmAmbOcclMap::freeAtomicData(AtomicData *atomicData)
{
	while(atomicData)
	{
		AtomicData *next = atomicData->next_;
		objectDataMemPool_.Free(atomicData);
		atomicData = next;
	}
}
//@} Jaewon
// -----------------------------------------------------------------------------
// AgcmAmbOcclMap.cpp - End of file
// -----------------------------------------------------------------------------
