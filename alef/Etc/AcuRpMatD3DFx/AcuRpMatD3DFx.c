#ifndef _CRT_SECURE_NO_DEPRECATE
	#define _CRT_SECURE_NO_DEPRECATE
#endif

//@{ Jaewon 20040813
// copied & modified.
// renderware material plugin for d3d fx file.
//@} Jaewon

/****************************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 2004 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *
 * matd3dfx.c
 *
 * Copyright (C) 2004 Criterion Technologies.
 *
 * Original author: Matt Reynolds.
 *
 * Purpose: D3D9 pipeline providing D3DX FX file support.
 *
 ****************************************************************************/
#include <d3d9.h>
#include <d3dx9.h>
#include "rwcore.h"
#include "rpworld.h"
#include "AcuRpMatD3DFx.h"
#include "effect.h"
#include "pipeline.h"
#include "rwd3d9.h"
#include "myassert.h"

//@{ Jaewon 20040922
// RpSkin definitions for 'vertexMaps' access
typedef void * SkinUnaligned;

typedef struct SkinPlatformData         SkinPlatformData;
typedef struct SkinAtomicPlatformData   SkinAtomicPlatformData;
typedef struct SkinGlobalPlatform       SkinGlobalPlatform;

void (*MatDestrucTorCallbackUV)( void* , void* , void* );

struct SkinPlatformData
{
    RwUInt32 maxNumBones;
    RwUInt32 useVertexShader;
};

typedef struct SkinSplitData SkinSplitData;
struct SkinSplitData
{
    RwUInt32            boneLimit;
    RwUInt32            numMeshes;
    RwUInt32            numRLE;
    RwUInt8             *matrixRemapIndices;
    RwUInt8             *meshRLECount;
    RwUInt8             *meshRLE;
};

typedef struct SkinBoneData SkinBoneData;
struct SkinBoneData
{
    RwUInt32            numBones;
    RwUInt32            numUsedBones;
    RwUInt8             *usedBoneList;
    RwMatrix            *invBoneToSkinMat;
};

typedef struct SkinVertexMaps SkinVertexMaps;
struct SkinVertexMaps
{
    RwUInt32            maxWeights;
    RwUInt32            *matrixIndices;
    RwMatrixWeights     *matrixWeights;
};

struct RpSkin
{
    SkinBoneData        boneData;
    SkinVertexMaps      vertexMaps;
    SkinPlatformData    platformData;
    SkinSplitData       skinSplitData;
    SkinUnaligned       *unaligned;
};
//@} Jaewon

/* define EFFECT_RELEASE_EFFECT_ON_ATOMIC_DISABLE so when disabling fx on an atomic release the effects on the
   atomic's materials. This means if the application re-enables an atomic the application must reset an effect
   to all materials */
//#define EFFECT_RELEASE_EFFECT_ON_ATOMIC_DISABLE

#define ATOMIC_GET_FX_DATA(atomic)  \
    ((AtomicExt *)(((RwUInt8 *)atomic)+(AtomicOffset)))

typedef struct AtomicExt AtomicExt;
struct AtomicExt
{
    RwBool enabled;
};

static RwChar EffectSearchPath[_MAX_PATH] = "\0";
static RwInt32 AtomicOffset = -1;

RwModuleInfo RpMaterialD3DFxModule = {0, 0};
RwInt32 RpMaterialD3DFxMaterialOffset = -1;

//@{ Jaewon 20041025
// critical section object for multi-threaded loading
//CRITICAL_SECTION _criticalSection; 
//@} Jaewon

/*
 *************************************************************************************************************
 */
DxEffect *
RpMaterialD3DFxGetEffect(RpMaterial *material)
{
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return NULL;

    /* Returns the material's current effect. Returns NULL if the material has no effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);

    matData = MATERIAL_GET_FX_DATA(material);
    return matData->effect;
}


/*
 *************************************************************************************************************
 */
void
RpMaterialD3DFxForAllTweakableParameters(RpMaterial *material, EffectTweakableParameterCallBack callback,
    void *data)
{
    RpMaterialD3DFxExt *matData;
    DxEffect *effect;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return;

    /* Execute the callback to all parameters in the material's current effect that are tweakable by an
       editor at runtime. If any invocation of the callback function returns NULL the iteration is
       terminated */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
    my_assert(callback);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    EffectForAllTweakableParameters(effect, callback, data);
    return;
}


/*
 *************************************************************************************************************
 */
RwBool 
RpMaterialD3DFxSetFloat(RpMaterial *material, RwChar *parameterName, RwReal data)
{
    RpMaterialD3DFxExt *matData;
    DxEffect *effect;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    /* Assign the floating point number to the parameter in the material's current effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
    my_assert(parameterName);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);

	//. 2006. 2. 22. Nonstopdj
	//. DxEffect*가 NULL일 경우 crash.
	//. invisible.fx를 정상적을 읽었을 경우 DxEffect*가 NULL이면 안된다.
	//. 일단 NULL Check로 패스. by All2one.
	if(effect != NULL)
		return EffectSetFloat(effect, parameterName, data);

	return FALSE;
}


/*
 *************************************************************************************************************
 */
RwBool 
RpMaterialD3DFxSetVector(RpMaterial *material, RwChar *parameterName, D3DXVECTOR4 *data)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    /* Assign the vector to the parameter in the material's current effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
    my_assert(parameterName);
    my_assert(data);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    return EffectSetVector(effect, parameterName, data);
}


/*
 *************************************************************************************************************
 */
RwBool 
RpMaterialD3DFxSetMatrix4x4(RpMaterial *material, RwChar *parameterName, D3DXMATRIX *data)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    /* Assign the matrix to the parameter in the material's current effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
    my_assert(parameterName);
    my_assert(data);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    return EffectSetMatrix4x4(effect, parameterName, data);
}


/*
 *************************************************************************************************************
 */
RwBool 
RpMaterialD3DFxSetTexture(RpMaterial *material, RwChar *parameterName, RwTexture *data)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    /* Assign RwTexture to the parameter in the material's current effect. If the parameter already has a
       texture assigned this texture's reference count will be decremented with RwTextureDestroy */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
    my_assert(parameterName);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);

    return EffectSetTexture(effect, parameterName, data);
}

//@{ Jaewon 20041021
RwBool 
RpMaterialD3DFxGetFloat(RpMaterial *material, RwChar *parameterName, RwReal *data)
{
    RpMaterialD3DFxExt *matData;
    DxEffect *effect;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    /* Assign the floating point number to the parameter in the material's current effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
    my_assert(parameterName);
	my_assert(data);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    return EffectGetFloat(effect, parameterName, data);
}


/*
 *************************************************************************************************************
 */
RwBool 
RpMaterialD3DFxGetVector(RpMaterial *material, RwChar *parameterName, D3DXVECTOR4 *data)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    /* Assign the vector to the parameter in the material's current effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
    my_assert(parameterName);
    my_assert(data);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    return EffectGetVector(effect, parameterName, data);
}


/*
 *************************************************************************************************************
 */
RwBool 
RpMaterialD3DFxGetMatrix4x4(RpMaterial *material, RwChar *parameterName, D3DXMATRIX *data)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    /* Assign the matrix to the parameter in the material's current effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
    my_assert(parameterName);
    my_assert(data);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    return EffectGetMatrix4x4(effect, parameterName, data);
}


/*
 *************************************************************************************************************
 */
RwBool 
RpMaterialD3DFxGetTexture(RpMaterial *material, RwChar *parameterName, RwTexture **data)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    /* Assign RwTexture to the parameter in the material's current effect. If the parameter already has a
       texture assigned this texture's reference count will be decremented with RwTextureDestroy */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
    my_assert(parameterName);
	my_assert(data);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    return EffectGetTexture(effect, parameterName, data);
}
//@} Jaewon

/*
 *************************************************************************************************************
 */
const RwChar *
RpMaterialD3DFxGetCurrentTechniqueName(RpMaterial *material)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return NULL;

    /* Returns the name of the material's effect current technique */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    return EffectGetCurrentTechniqueName(effect);
}


/*
 *************************************************************************************************************
 */
const RwChar *
RpMaterialD3DFxGetFxName(RpMaterial *material)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return NULL;

    /* Returns the name of the material's current effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    return effect->pSharedD3dXEffect->name;
}


/*
 *************************************************************************************************************
 */
RwUInt32
RpMaterialD3DFxGetNumberOfTechniques(RpMaterial *material)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return 0;

    /* Returns the number of techniques in the material's current effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
	my_assert(effect);
    return EffectGetNumberOfTechniques(effect);
}


/*
 *************************************************************************************************************
 */
RwBool
RpMaterialD3DFxSetFirstValidTechnique(RpMaterial *material)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    /* Set the technique for the material's current effect.  The param index should be in the range 
       [0,RpMaterialD3DFxGetNumberOfTechniques()-1] */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
    //my_assert(index < RpMaterialD3DFxGetNumberOfTechniques(material));

    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    return EffectSetFirstValidTechnique(effect);
}


/*
 *************************************************************************************************************
 */
void
RpMaterialD3DFxSetConstantUploadCallBack(RpMaterial *material, EffectConstUploadCallBack callback)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return;

    /* Set the callback that will be called for a mesh that has this material during rendering. This callback
       gives the application a hook to set shader constant variables for the material's current effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
	my_assert(material);
    
    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    effect->constUploadCallBack = callback;
    return;
}


/*
 *************************************************************************************************************
 */
EffectConstUploadCallBack
RpMaterialD3DFxGetConstantUploadCallBack(RpMaterial *material)
{
    DxEffect *effect;
    RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return NULL;

    /* Return the callback set to the material's current effect. Returns NULL if no callback is set. */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
	my_assert(material);
    
    matData = MATERIAL_GET_FX_DATA(material);
    effect = matData->effect;
    my_assert(effect);
    return effect->constUploadCallBack;
}


/*
 *************************************************************************************************************
 */
RwChar *
RpMaterialD3DFxGetSeachPath(void)
{
	if (RpMaterialD3DFxMaterialOffset < 0)
		return NULL;

    /* Returns the search path prefixed to the fx filename when loading an fx file from disk */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    return EffectSearchPath;
}

void
RpMaterialD3DFxSetSearchPath(RwChar *path)
{
	if (RpMaterialD3DFxMaterialOffset < 0)
		return;

    /* Sets the search path to prefix to the fx filename when loading an fx file from disk. The path must
       contain a trailing path separator */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(path);
	//@{ Jaewon 20041025
	//EnterCriticalSection(&_criticalSection);
    strncpy(EffectSearchPath, path, _MAX_PATH);
	//LeaveCriticalSection(&_criticalSection);
	//@} Jaewon
    return;
}


/*
 *************************************************************************************************************
 */
//@{ Jaewon 20050127
static RwBool
RpMaterialD3DFxSetEffectWithMaxWeight(RpMaterial *material, RwChar *fxName, RwUInt32 nWeights)
{
    RpMaterialD3DFxExt *matData;
	DxEffect *effect;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    /* Set the named effect to the material.  If the material already has an effect this effect will be
       released.  If fxName is NULL fx rendering is disabled for meshes who reference this material.
       Returns true if the effect was set to the material or false if there was an error */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(material);
	my_assert(fxName);

    matData = MATERIAL_GET_FX_DATA(material);
    if (matData->effect)
    {
        EffectRelease(matData->effect);
        matData->effect = NULL;
    }

    effect = EffectLoad(fxName, nWeights);
    if (NULL == effect)
    {
        return FALSE;
    }

    matData->effect = effect;
    /* I assume there is at least one technique in the effect */
    if(RpMaterialD3DFxSetFirstValidTechnique(material) == FALSE)
	{
        EffectRelease(matData->effect);
        matData->effect = NULL;
		return FALSE;
	}
	else
		return TRUE;
}
RwBool
RpMaterialD3DFxSetEffectNoWeight(RpMaterial *material, RwChar *fxName)
{
	return RpMaterialD3DFxSetEffectWithMaxWeight(material, fxName, 0);
}
RwBool
RpMaterialD3DFxSetEffectWeight1(RpMaterial *material, RwChar *fxName)
{
	return RpMaterialD3DFxSetEffectWithMaxWeight(material, fxName, 1);
}
RwBool
RpMaterialD3DFxSetEffectWeight2(RpMaterial *material, RwChar *fxName)
{
	return RpMaterialD3DFxSetEffectWithMaxWeight(material, fxName, 2);
}
RwBool
RpMaterialD3DFxSetEffectWeight3(RpMaterial *material, RwChar *fxName)
{
	return RpMaterialD3DFxSetEffectWithMaxWeight(material, fxName, 3);
}
RwBool
RpMaterialD3DFxSetEffectWeight4(RpMaterial *material, RwChar *fxName)
{
	return RpMaterialD3DFxSetEffectWithMaxWeight(material, fxName, 4);
}
//@} Jaewon

RwBool
//@{ Jaewon 20040922
// bSkin -> pSkin
RpMaterialD3DFxSetEffect(RpMaterial *material, RwChar *fxName, RpSkin *pSkin)
//@} Jaewon
{
	//@{ Jaewon 20050127
	// Use RpMaterialD3DFxSetEffectWithMaxWeight.
	// check if pSkin == null.
	return RpMaterialD3DFxSetEffectWithMaxWeight(material, fxName, pSkin?pSkin->vertexMaps.maxWeights:0);
	//@} Jaewon
}


/*
 *************************************************************************************************************
 */
//#if defined(EFFECT_RELEASE_EFFECT_ON_ATOMIC_DISABLE)

static RpMaterial *
MaterialFxRelease(RpMaterial *material, void *data __RWUNUSED__)
{
	RpMaterialD3DFxExt *matData;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return NULL;

    my_assert(material);

    my_assert(RpMaterialD3DFxModule.numInstances > 0);

    matData = MATERIAL_GET_FX_DATA(material);
    if (matData->effect)
    {
        EffectRelease(matData->effect);
        matData->effect = NULL;
    }

    return material;
}

//#endif  /* EFFECT_RELEASE_EFFECT_ON_ATOMIC_DISABLE */

void
RpAtomicFxDisable(RpAtomic *atomic)
{
    AtomicExt *atomicExt;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return;

    /* Disables matd3dfx on the atomic by setting the default pipeline to the atomic. This function will
       force a reinstance of the atomic's vertex buffers and will tell the instancing code not to add tangents
       to the vertex stream */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(atomic);

    atomicExt = ATOMIC_GET_FX_DATA(atomic);
    if (atomicExt->enabled)
    {
        //RpGeometry *geometry;
        //RpD3D9GeometryUsageFlag flags;

        /* revert to the default atomic pipeline, I do not know what was attached before */
		//@{ Jaewon 20040917
		// a litte hack to save/restore the original pipeline.
        RpAtomicSetPipeline(atomic, (TRUE==atomicExt->enabled)?NULL:(RxPipeline *)(atomicExt->enabled));  
		//@} Jaewon

		//@{ Jaewon 20040917
		// no reinstancing
		/*
        geometry = RpAtomicGetGeometry(atomic);
        my_assert(geometry);

        // changing pipeline need to force a reinstance
        if (geometry->repEntry)
        {
            RwResourcesFreeResEntry(geometry->repEntry);
            geometry->repEntry = NULL;
        }
        else if (atomic->repEntry)
        {
            RwResourcesFreeResEntry(atomic->repEntry);
            atomic->repEntry = NULL;
        }

        flags = RpD3D9GeometryGetUsageFlags(geometry);
        flags &= ~rpD3D9GEOMETRYUSAGE_CREATETANGENTS;
        RpD3D9GeometrySetUsageFlags(geometry, flags);
		*/
		//@} Jaewon

#if defined(EFFECT_RELEASE_EFFECT_ON_ATOMIC_DISABLE)
        RpGeometryForAllMaterials(geometry, MaterialFxRelease, NULL);
#endif  /* EFFECT_RELEASE_EFFECT_ON_ATOMIC_DISABLE */
        atomicExt->enabled = FALSE;
    }

    return;
}

//@{ Jaewon 20041012
RwBool
RpAtomicFxIsEnabled(RpAtomic *atomic)
{
	AtomicExt *atomicExt;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return FALSE;

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(atomic);

	atomicExt = ATOMIC_GET_FX_DATA(atomic);

	if(atomicExt->enabled)
		return TRUE;
	else
		return FALSE;
}
//@} Jaewon

/*
 *************************************************************************************************************
 */
void
RpAtomicFxEnable(RpAtomic *atomic)
{
    AtomicExt *atomicExt;
	RxD3D9ResEntryHeader    *resEntryHeader;

	if (RpMaterialD3DFxMaterialOffset < 0)
		return;

    /* Sets the pipeline to the atomic which supports FX rendering. This function will force a reinstance of
       the atomic's vertex buffers and will tell the instancing code to add tangents to the vertex stream */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(atomic);
		
	//@{ Jaewon 20040907
	// if vertex shaders are not supported, do not enable the fx pipe.
	if((((const D3DCAPS9*)RwD3D9GetCaps())->VertexShaderVersion & 0xffff) < 0x0101)
	{
        _rwDebugSendMessage(rwDEBUGMESSAGE, __FILE__, __LINE__, "RpAtomicFxEnable", 
							"No vertex shader support. cannot enable the fx pipe.");
		return;
	}
	//@} Jaewon

    atomicExt = ATOMIC_GET_FX_DATA(atomic);
    if (!atomicExt->enabled)
    {
        RpGeometry *geometry;
        RpD3D9GeometryUsageFlag flags;
        RxPipeline *pipeline;
		RpSkin *pSkin = NULL;

		//@{ Jaewon 20040917
		RxPipeline *original;
		RpAtomicGetPipeline(atomic, &original);
		//@} Jaewon

        pipeline = EffectGetPipeline();
        my_assert(pipeline);
        RpAtomicSetPipeline(atomic, pipeline);  

		//@{ Jaewon 20040917
		// no reinstancing
        geometry = RpAtomicGetGeometry(atomic);
        //my_assert(geometry);
		//@{ Jaewon 20041026
		// reinstancing can be necessary.
        // changing pipeline need to force a reinstance
		pSkin = geometry?RpSkinGeometryGetSkin(geometry):NULL;
		if(pSkin && (pSkin->platformData.useVertexShader == 0))
		// the current instancing is invalid.
		{
			if (geometry->repEntry)
			{
				//.RwResourcesFreeResEntry(geometry->repEntry);
				//.geometry->repEntry = NULL;
				resEntryHeader = (RxD3D9ResEntryHeader *)(geometry->repEntry + 1);
				
				CS_RESENTRYHEADER_LOCK(resEntryHeader);
				resEntryHeader->serialNumber = -1;
				CS_RESENTRYHEADER_UNLOCK(resEntryHeader);

			}
			else if (atomic->repEntry)
			{
				//.RwResourcesFreeResEntry(atomic->repEntry);
				//.atomic->repEntry = NULL;

				resEntryHeader = (RxD3D9ResEntryHeader *)(atomic->repEntry + 1);
				
				CS_RESENTRYHEADER_LOCK(resEntryHeader);
				resEntryHeader->serialNumber = -1;
				CS_RESENTRYHEADER_UNLOCK(resEntryHeader);
			}
		}
		//@} Jaewon
		//@{ Jaewon 20041021
		if(geometry)
		{
			flags = RpD3D9GeometryGetUsageFlags(geometry);
			RpD3D9GeometrySetUsageFlags(geometry, flags | rpD3D9GEOMETRYUSAGE_CREATETANGENTS);
		}
		//@} Jaewon
		//@} Jaewon

		//@{ Jaewon 20040917
		// a litte hack to save/restore the original pipeline.		
		
		#pragma message( "마고자: 코드 자체가 이해가 안돼서 워닝을 고치지 않음.." )
		atomicExt->enabled = original ? original : TRUE;
		//@} Jaewon
    }

    return;
}


/*
 *************************************************************************************************************
 */
static void *
MaterialOpen(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RpMaterialD3DFxExt *matData;
    my_assert(object);

    matData = MATERIAL_GET_FX_DATA(object);
    matData->effect = NULL;
    return object;
}

static void *
MaterialClose(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RpMaterialD3DFxExt *matData;

    my_assert(object);

    matData = MATERIAL_GET_FX_DATA(object);
    if (matData->effect)
    {
        EffectRelease(matData->effect);
        matData->effect = NULL;
    }

	MatDestrucTorCallbackUV( object , 0 , 0 );

    return object;
}

static void *
MaterialCopy(void *dstObject, const void *srcObject, RwInt32 offset __RWUNUSED__ , RwInt32 size __RWUNUSED__)
{
    RpMaterialD3DFxExt *matData;
    RpMaterialD3DFxExt *dstMatData;
    
	my_assert(dstObject);
    my_assert(srcObject);

	//@{ Jaewon 20041007
	// destination effect structure can be null...
    matData = MATERIAL_GET_FX_DATA(srcObject);
    dstMatData = MATERIAL_GET_FX_DATA(dstObject);
	
	if(dstMatData->effect)
	{
		EffectRelease(dstMatData->effect);
		dstMatData->effect = NULL;
	}
	
	if (matData->effect)
    {
		dstMatData->effect = EffectCopy(matData->effect);
    }
	//@} Jaewon

    return dstObject;
}


/*
 *************************************************************************************************************
 */
static RwStream *
MaterialWrite(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offset __RWUNUSED__, 
    RwInt32 size __RWUNUSED__)
{
    RpMaterialD3DFxExt *matData;
    
    my_assert(object);

    matData = MATERIAL_GET_FX_DATA(object);
    if (matData->effect)
    {
        if (!EffectWrite(matData->effect, stream))
        {
            RwChar msg[MAX_MSG_SIZE] = "Failed to write material effect data for material 0x";
            RwChar matAddr[sizeof(void *)];
            _snprintf(matAddr, sizeof(void *), "%p", object);
            strncat(msg, matAddr, __min(strlen(matAddr), MAX_STRING_LEN(msg)-strlen(msg)));
            _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "MaterialWrite", msg);
            return NULL;
        }
    }
    
    return stream;
}

static RwStream *
MaterialRead(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offset __RWUNUSED__, 
    RwInt32 size __RWUNUSED__)
{
    RpMaterialD3DFxExt *matData;
    my_assert(object);

    matData = MATERIAL_GET_FX_DATA(object);
    matData->effect = EffectRead(stream);
    if (NULL == matData->effect)
    {
        RwChar msg[MAX_MSG_SIZE] = "Failed to read material effect data for material 0x";
        RwChar matAddr[sizeof(void *)];
        _snprintf(matAddr, sizeof(void *), "%p", object);
        strncat(msg, matAddr, __min(strlen(matAddr), MAX_STRING_LEN(msg)-strlen(msg)));
        _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "MaterialRead", msg);
        return NULL;
    }
	//@{ Jaewon 20050819
	// A stopgap measure...-- not good!!!
//@{ Jaewon 20051017
// Do not apply the stopgap measure to MapTool.
#ifndef USE_MFC
	else
	{
		RwChar name[FX_FILE_NAME_LENGTH];
		strncpy(name, matData->effect->pSharedD3dXEffect->name, FX_FILE_NAME_LENGTH);
		_strlwr(name);
		if(strstr(name, "ambientocclusion"))
		{
			EffectRelease(matData->effect);
			matData->effect = NULL;
		}
	}
#endif
//@} Jaewon
	//@} Jaewon
    return stream;
}

static RwInt32
MaterialGetSize(const void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RwInt32 effectSize;
    RpMaterialD3DFxExt *matData;

    my_assert(object);

    matData = MATERIAL_GET_FX_DATA(object);
    if (matData->effect)
    {
        effectSize = EffectGetSize(matData->effect);
    }
    else
    {
        effectSize = 0;
    }

    return effectSize;
}


/*
 *************************************************************************************************************
 */
static RpMaterial *
MaterialTestDXEffect(RpMaterial *material, void *data)
{
    RpMaterialD3DFxExt *matData;
    RwUInt8 *result = (RwUInt8 *)data;

    my_assert(material);

    matData = MATERIAL_GET_FX_DATA(material);
    if (matData->effect)
    {
        /* at least one material in the geometry has an effect */
        *result |= 1;
    }
    else
    {
        /* some materials in the geometry are missing effects */
        *result |= 2;
    }

    return material;
}

static RwBool
AtomicAlways(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RpAtomic *atomic;
    RpGeometry *geometry;
    RwUInt8 result;

    /* Caution, if the atomic has a rights callback e.g. RpSkin, then that pipeline will be set instead of
       this one. This can be avoided by having a clump always callback which calls this function or calling
       RpAtomicFxEnable after RpClumpStreamRead/RpAtomicStreamRead */
    my_assert(object);
    atomic = (RpAtomic *)object;

    result = 0;
    geometry = RpAtomicGetGeometry(atomic);
    my_assert(geometry);
	//@{ Jaewon 20050127
	// Don't crash...
	if(NULL == geometry)
		return TRUE;
	//@} Jaewon
    RpGeometryForAllMaterials(geometry, MaterialTestDXEffect, (void *)&result);
    if (result & 1)
    {
        /* at least one of the atomic's materials has an fx applied so enable the atomic pipeline */
        RpAtomicFxEnable(atomic);

        if (result & 2)
        {
            RwChar msg[] = "Some materials do not have an effect.Meshes that reference these materials will \
be rendered using the current render state of D3D, when using using the matd3dfx pipeline";
            _rwDebugSendMessage(rwDEBUGMESSAGE, __FILE__, __LINE__, "AtomicAlways", msg);
        }
    }

    return TRUE;
}

//@{ Jaewon 20040917
// a geometry which has the fx pipeline should have tangents.
static RwBool
AtomicRights( void *object,
			  RwInt32 offset __RWUNUSED__,
              RwInt32 size   __RWUNUSED__,
              RwUInt32 extraData )
{
	RpAtomic *atomic;

	my_assert(object);

	atomic = (RpAtomic *)object;

	if ((((const D3DCAPS9*)RwD3D9GetCaps())->VertexShaderVersion & 0xffff) >= 0x0101)
	{
		RpGeometry *geometry;
		RpD3D9GeometryUsageFlag flags;

		geometry = RpAtomicGetGeometry(atomic);

		flags = RpD3D9GeometryGetUsageFlags(geometry);

		flags |= rpD3D9GEOMETRYUSAGE_CREATETANGENTS;

		RpD3D9GeometrySetUsageFlags(geometry, flags);
	}

	return TRUE;
}
//@} Jaewon

/*
 *************************************************************************************************************
 */
static void *
AtomicOpen(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    AtomicExt *data;
    my_assert(object);

    data = ATOMIC_GET_FX_DATA(object);
    data->enabled = 0;
    return object;
}

static void *
AtomicClose(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    AtomicExt *data;
    my_assert(object);

    data = ATOMIC_GET_FX_DATA(object);
    data->enabled = 0;
    return object;
}

static void *
AtomicCopy(void *dstObject, const void *srcObject, RwInt32 offset __RWUNUSED__ , RwInt32 size __RWUNUSED__)
{
    AtomicExt *srcData, *dstData;

    my_assert(dstObject);
    my_assert(srcObject);

    srcData = ATOMIC_GET_FX_DATA(srcObject);
	//@{ Jaewon 20041022
	// initialize the flag.
	dstData = ATOMIC_GET_FX_DATA(dstObject);
	dstData->enabled = 0;
	//@} Jaewon
    if (srcData->enabled)
    {
        RpAtomicFxEnable((RpAtomic *)dstObject);
    }
    else
    {
        RpAtomicFxDisable((RpAtomic *)dstObject);
    }

    return dstObject;
}


/*
 *************************************************************************************************************
 */
static RpAtomic *
ClumpAtomicAlwaysCB(RpAtomic *atomic, void *data __RWUNUSED__)
{
    my_assert(atomic);
    AtomicAlways(atomic, 0, 0);
    return atomic;
}

static RwBool
ClumpAlways(void *object, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    my_assert(object);
    RpClumpForAllAtomics((RpClump *)object, ClumpAtomicAlwaysCB, NULL);
    return TRUE;
}

/*
 *************************************************************************************************************
 */
static void *
EngineOpen(void *instance, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    if (0 == RpMaterialD3DFxModule.numInstances)
    {
		//@{ Jaewon 20041025
		//InitializeCriticalSection(&_criticalSection);
		//@} Jaewon
        EffectPipelineOpen();
        EffectSetD3DDevice();
        EffectSetDeviceLostRestoreCallBacks();
        EffectCreateEffectFreelist();
        EffectSearchPath[0] = '\0';
    }

    RpMaterialD3DFxModule.numInstances++;
    return instance;
}

static void *
EngineClose(void *instance, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    --RpMaterialD3DFxModule.numInstances;

    if (0 == RpMaterialD3DFxModule.numInstances)
    {
        EffectPipelineClose();

        /* to avoid the numInstances assert in ForAllEffectsInFreelist */
        RpMaterialD3DFxModule.numInstances = 1;
        EffectDestroyEffectFreelist();
        RpMaterialD3DFxModule.numInstances = 0;

        EffectResetDeviceLostRestoreCallBacks();
        EffectNullD3DDevice();
		//@{ Jaewon 20041025
		//DeleteCriticalSection(&_criticalSection);
		//@} Jaewon
    }
    return instance;
}


/*
 *************************************************************************************************************
 */
RwBool
RpMaterialD3DFxPluginAttach(void)
{
    RwInt32 offset;

    /* RpMaterialD3DFxPluginAttach is used to attach the material D3D fx plugin to the RenderWare system to
       enable D3D9 effect rendering and management.  The plugin must be attached between initialising the
       system with RwEngineInit and opening it with RwEngineOpen. Returns TRUE if successful or FALSE if there
       is an error */

    offset = RwEngineRegisterPlugin(0, MATERIAL_FX_PLUGIN, EngineOpen, EngineClose);
    if (offset < 0)
    {
        return FALSE;
    }

    RpMaterialD3DFxMaterialOffset = RpMaterialRegisterPlugin(sizeof(RpMaterialD3DFxExt), MATERIAL_FX_PLUGIN,
        MaterialOpen, MaterialClose, MaterialCopy);
    if (RpMaterialD3DFxMaterialOffset < 0)
    {
        return FALSE;
    }

    offset = RpMaterialRegisterPluginStream(MATERIAL_FX_PLUGIN, MaterialRead, MaterialWrite, MaterialGetSize);
    if (offset != RpMaterialD3DFxMaterialOffset)
    {
        return FALSE;
    }

    AtomicOffset = RpAtomicRegisterPlugin(sizeof(AtomicExt), MATERIAL_FX_PLUGIN, AtomicOpen, AtomicClose,
        AtomicCopy);
    if (AtomicOffset < 0)
    {
        return FALSE;
    }

	//@{ Jaewon 20040917
	// register a rights callback so that it generates tangent data.
    /*offset = RpAtomicSetStreamRightsCallBack(MATERIAL_FX_PLUGIN, AtomicRights);
    if(0 >= offset)
	{
		return FALSE;
	}*/
	//@} Jaewon

    offset = RpAtomicSetStreamAlwaysCallBack(MATERIAL_FX_PLUGIN, AtomicAlways);
    if (offset != AtomicOffset)
    {
        return FALSE;
    }

    /* avoid the app having to call RpAtomicSetPipeline after loading an atomic whose materials have a
       fx already applied to them. The clump always callback is called after the atomics are read in
       overriding any pipeline set with atomic rights. This does assume atomics are read in with
       RpClumpStreamRead and not RpAtomicStreamRead, which is the case for the example viewer */
    offset = RpClumpRegisterPlugin(0, MATERIAL_FX_PLUGIN, NULL, NULL, NULL);
    if (offset < 0)
    {
        return FALSE;
    }

    offset = RpClumpSetStreamAlwaysCallBack(MATERIAL_FX_PLUGIN, ClumpAlways);
    if (offset < 0)
    {
        return FALSE;
    }

    return TRUE;
}

/*
 *************************************************************************************************************
 */
//@{ Jaewon 20040917
// iterator for all registered effects
// If any invocation of the callback function returns NULL, 
// the iteration is terminated.
void
RpMaterialD3DFxForAllEffects(D3dXEffectCallBack callback, void *data)
{
	if (RpMaterialD3DFxMaterialOffset < 0)
		return;

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(callback);

    ForAllD3dXEffectsInFreelist(callback, data);
}
//@} Jaewon

/*
 *************************************************************************************************************
 */
//@{ Jaewon 20041215
void
RpAtomicFxRelease(RpAtomic *atomic)
{
	RpGeometry *geometry;
	
	if (RpMaterialD3DFxMaterialOffset < 0)
		return;

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(atomic);

	geometry = RpAtomicGetGeometry(atomic);
	my_assert(geometry);

	RpGeometryForAllMaterials(geometry, MaterialFxRelease, NULL);
}
//@} Jaewon