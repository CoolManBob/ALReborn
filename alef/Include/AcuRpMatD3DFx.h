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
 * matd3dfx.h
 *
 * Copyright (C) 2004 Criterion Technologies.
 *
 * Original author: Matt Reynolds.
 *
 ****************************************************************************/

#ifndef __MATD3DFX_H__
#define __MATD3DFX_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment (lib , "AcuRpMatD3DFxD")
#else
#pragma comment (lib , "AcuRpMatD3DFx")
#endif
#endif

#include "effect.h"
//@{ Jaewon 20040922
#include <rpskin.h>
//@} Jaewon

#if !defined(_MAX_PATH)
#define _MAX_PATH   (256)
#endif /* _MAX_PATH */

#if _MSC_VER < 1300
#ifdef _DEBUG
#pragma comment ( lib , "AcuRpMatD3DFxD" )
#else
#pragma comment ( lib , "AcuRpMatD3DFx" )
#endif
#endif


#define MAX_STRING_LEN(s)   (sizeof(s)/sizeof(s[0]) - 1)
#define MAX_MSG_SIZE (256)

#define VENDOR_ID (0xfffff0L)
#define PLUGIN_ID (0x08)
#define MATERIAL_FX_PLUGIN (MAKECHUNKID(VENDOR_ID,PLUGIN_ID))

#define MATERIAL_GET_FX_DATA(material)  \
    ((RpMaterialD3DFxExt *)(((RwUInt8 *)material)+(RpMaterialD3DFxMaterialOffset)))

typedef struct RpMaterialD3DFxExt RpMaterialD3DFxExt;
struct RpMaterialD3DFxExt
{
    DxEffect *effect;
};

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern RwInt32 RpMaterialD3DFxMaterialOffset;
extern RwModuleInfo RpMaterialD3DFxModule;

extern	void (*MatDestrucTorCallbackUV)( void* , void* , void* );

extern RwBool 
RpMaterialD3DFxSetFloat(RpMaterial *material, RwChar *parameterName, RwReal data);

extern RwBool 
RpMaterialD3DFxSetVector(RpMaterial *material, RwChar *parameterName, D3DXVECTOR4 *data);

extern RwBool 
RpMaterialD3DFxSetMatrix4x4(RpMaterial *material, RwChar *parameterName, D3DXMATRIX *data);

extern RwBool 
RpMaterialD3DFxSetTexture(RpMaterial *material, RwChar *parameterName, RwTexture *data);

//@{ Jaewon 20041021
extern RwBool 
RpMaterialD3DFxGetFloat(RpMaterial *material, RwChar *parameterName, RwReal *data);

extern RwBool 
RpMaterialD3DFxGetVector(RpMaterial *material, RwChar *parameterName, D3DXVECTOR4 *data);

extern RwBool 
RpMaterialD3DFxGetMatrix4x4(RpMaterial *material, RwChar *parameterName, D3DXMATRIX *data);

extern RwBool 
RpMaterialD3DFxGetTexture(RpMaterial *material, RwChar *parameterName, RwTexture **data);
//@} Jaewon

extern RwUInt32
RpMaterialD3DFxGetNumberOfTechniques(RpMaterial *material);

extern RwBool
RpMaterialD3DFxSetFirstValidTechnique(RpMaterial *material);

extern RwChar *
RpMaterialD3DFxGetSeachPath(void);

extern void
RpMaterialD3DFxSetSearchPath(RwChar *path);

extern RwBool
//@{ Jaewon 20040922
// bSkin -> pSkin
RpMaterialD3DFxSetEffect(RpMaterial *material, RwChar *fxName, RpSkin *pSkin);
//@} Jaewon

//@{ Jaewon 20050127
// Explicit max weight versions
extern RwBool RpMaterialD3DFxSetEffectNoWeight(RpMaterial *material, RwChar *fxName);
extern RwBool RpMaterialD3DFxSetEffectWeight1(RpMaterial *material, RwChar *fxName);
extern RwBool RpMaterialD3DFxSetEffectWeight2(RpMaterial *material, RwChar *fxName);
extern RwBool RpMaterialD3DFxSetEffectWeight3(RpMaterial *material, RwChar *fxName);
extern RwBool RpMaterialD3DFxSetEffectWeight4(RpMaterial *material, RwChar *fxName);
//@} Jaewon

extern void
RpAtomicFxEnable(RpAtomic *atomic);

extern void
RpAtomicFxDisable(RpAtomic *atomic);

extern RwBool
RpMaterialD3DFxPluginAttach(void);

extern const RwChar *
RpMaterialD3DFxGetCurrentTechniqueName(RpMaterial *material);

extern void
RpMaterialD3DFxForAllTweakableParameters(RpMaterial *material, EffectTweakableParameterCallBack callback,
    void *data);

extern DxEffect *
RpMaterialD3DFxGetEffect(RpMaterial *material);

extern const RwChar *
RpMaterialD3DFxGetFxName(RpMaterial *material);


//@{ Jaewon 20040917
// iterator for all registered effects
extern void
RpMaterialD3DFxForAllEffects(D3dXEffectCallBack callback, void *data);
//@} Jaewon

//@{ Jaewon 20041012
extern RwBool
RpAtomicFxIsEnabled(RpAtomic *atomic);
//@} Jaewon

//@{ Jaewon 20041215
extern void
RpAtomicFxRelease(RpAtomic *atomic);
//@} Jaewon

//@{ Jaewon 20041224
// make public.
extern void
RpMaterialD3DFxSetConstantUploadCallBack(RpMaterial *material, EffectConstUploadCallBack callback);
extern EffectConstUploadCallBack
RpMaterialD3DFxGetConstantUploadCallBack(RpMaterial *material);
//@} Jaewon
#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* __MATD3DFX_H__ */

