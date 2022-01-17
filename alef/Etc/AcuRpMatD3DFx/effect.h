//@{ Jaewon 20040813
// copied & modified.
// do all dirty jobs here for the AcuRpMatD3DFx material plugin.
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
 * effect.h
 *
 * Copyright (C) 2004 Criterion Technologies.
 *
 * Original author: Matt Reynolds.
 *
 ****************************************************************************/

#ifndef __EFFECT_H__
#define __EFFECT_H__

#include <d3dx9.h>
/* Name of semantics in your shaders that should be used for common data */
/* if you edit these defines make sure to update EffectUploadRenderWareShaderConstants and EffectBuildParams*/
enum RenderWareSemantics
{
	ID_WORLD_VIEW_PROJ_MATRIX = 0,
	ID_PROJ_MATRIX,
	ID_WORLD_VIEW_MATRIX,
	ID_WORLD_MATRIX,
	ID_CAMERA_POSITION,
	ID_CAMERA_DIRECTION,
	ID_BONE_MATRICES,

	ID_BASE_TEXTURE,
	ID_LIGHT_DIRECTION,
	ID_LIGHT_COLOR,
	ID_AMBIENT,
	ID_DIFFUSE,
	ID_SPECULAR,
	ID_LIGHT_AMBIENT,
	ID_FOG,
	//@{ Jaewon 20050419
	// Up to 8 lights
	ID_LIGHT_POSITION0,
	ID_LIGHT_POSITION1,
	ID_LIGHT_POSITION2,
	ID_LIGHT_POSITION3,
	ID_LIGHT_POSITION4,
	ID_LIGHT_POSITION5,
	ID_LIGHT_POSITION6,
	ID_LIGHT_POSITION7,
	ID_LIGHT_PARAMETERS0,
	ID_LIGHT_PARAMETERS1,
	ID_LIGHT_PARAMETERS2,
	ID_LIGHT_PARAMETERS3,
	ID_LIGHT_PARAMETERS4,
	ID_LIGHT_PARAMETERS5,
	ID_LIGHT_PARAMETERS6,
	ID_LIGHT_PARAMETERS7,
	ID_LIGHT_COLOR0,
	ID_LIGHT_COLOR1,
	ID_LIGHT_COLOR2,
	ID_LIGHT_COLOR3,
	ID_LIGHT_COLOR4,
	ID_LIGHT_COLOR5,
	ID_LIGHT_COLOR6,
	ID_LIGHT_COLOR7,
	ID_LIGHT_DIRECTION0,
	ID_LIGHT_DIRECTION1,
	ID_LIGHT_DIRECTION2,
	ID_LIGHT_DIRECTION3,
	ID_LIGHT_DIRECTION4,
	ID_LIGHT_DIRECTION5,
	ID_LIGHT_DIRECTION6,
	ID_LIGHT_DIRECTION7,
	//@} Jaewon
	//@{ Jaewon 20040910
	ID_TIME,
	//@} Jaewon

	ID_TOTAL
};

enum SpaceAnnotation
{
	ID_LOCAL_SPACE = 0,
	ID_WORLD_SPACE,
	ID_VIEW_SPACE,

	ID_SPACE_TOTAL
};

#define RW_WORLD_VIEW_PROJ_MATRIX   "WorldViewProjection"	// world * view * projection matrix
#define RW_PROJ_MATRIX              "Projection"			// projection matrix
#define RW_WORLD_VIEW_MATRIX        "WorldView"				// world * view matrix
#define RW_WORLD_MATRIX             "World"					// atomic world matrix (LTM)
#define RW_CAMERA_POSITION          "CameraPosition"		// camera position in world space(replaced by position with space=view annotation)
#define RW_CAMERA_DIRECTION         "CameraDirection"       // camera direction in world space(replaced by position with space=view annotation)
#define RW_BONE_MATRICES            "BoneMatrices"			// skinning bone matrix array

#define RW_BASE_TEXTURE				"BaseTexture"			// material texture
#define RW_LIGHT_DIRECTION			"LightDirection"		// direction of the main light in world space(replaced by direction with space=view annotation)
#define RW_LIGHT_COLOR				"LightColor"			// color of the main light
#define RW_AMBIENT					"Ambient"				// material ambient
#define RW_DIFFUSE					"Diffuse"				// material diffuse
#define RW_SPECULAR					"Specular"				// material specular
#define RW_LIGHT_AMBIENT			"LightAmbient"			// light ambient
#define RW_FOG						"Fog"					// fog
#define RW_LIGHT_POSITION0			"LightPosition0"		// position of spot light #0
#define RW_LIGHT_PARAMETERS0		"LightParameters0"		// parameters of spot light #0
#define RW_LIGHT_POSITION1			"LightPosition1"		// position of spot light #0
#define RW_LIGHT_PARAMETERS1		"LightParameters1"		// parameters of spot light #0
#define RW_LIGHT_COLOR0				"LightColor0"			// color of spot light #0
#define RW_LIGHT_COLOR1				"LightColor1"			// color of spot light #1
#define RW_LIGHT_DIRECTION0			"LightDirection0"		// direction of spot light #0
#define RW_LIGHT_DIRECTION1			"LightDirection1"		// direction of spot light #1
//@{ Jaewon 20040910
#define RW_TIME						"Time"					// time in seconds
//@} Jaewon
//@{ Jaewon 20050419
// Up to 8 lights
#define RW_LIGHT_POSITIONX			"LightPosition"
#define RW_LIGHT_PARAMETERSX		"LightParameters"
#define RW_LIGHT_COLORX				"LightColor"
#define RW_LIGHT_DIRECTIONX			"LightDirection"
//@} Jaewon

// space annotations
#define RW_SPACE					"Space"
#define RW_LOCAL_SPACE				"Object"
#define RW_WORLD_SPACE				"World"
#define RW_VIEW_SPACE				"View"

#define FX_HANDLE_NAME_LENGTH   (32)
#define FX_FILE_NAME_LENGTH     (32)

/* These are the effect parameters the current RW implementation allows to be edited at runtime. If you add
   more parameters increment the type value do not change the value for existing types. That would break
   materials already streamed to disk */
#define EFFECT_TWEAKABLE_PARAM_NATYPE       (0)
#define EFFECT_TWEAKABLE_PARAM_FLOAT        (1)
#define EFFECT_TWEAKABLE_PARAM_VECTOR       (2)
#define EFFECT_TWEAKABLE_PARAM_MATRIX4X4    (3)
#define EFFECT_TWEAKABLE_PARAM_TEXTURE		(4)

//@{ Jaewon 20041102
// lighting optimization
#define FX_LIGHTING_VARIATION 3
//@} Jaewon

typedef struct HandleFloat HandleFloat;
struct HandleFloat
{
    RwReal data;
	//@{ Jaewon 20041102
	// lighting optimization
    D3DXHANDLE handle[FX_LIGHTING_VARIATION];
	//@} Jaewon
    RwChar name[FX_HANDLE_NAME_LENGTH];
    RwBool tweakable;

	//@{ Jaewon 20050722
	// For memory pooling
	HandleFloat *next;
	//@} Jaewon
};

typedef struct HandleVector HandleVector;
struct HandleVector
{
    D3DXVECTOR4 data;
    //@{ Jaewon 20041102
	// lighting optimization
    D3DXHANDLE handle[FX_LIGHTING_VARIATION];
	//@} Jaewon
    RwChar name[FX_HANDLE_NAME_LENGTH];
    RwBool tweakable;

	//@{ Jaewon 20050722
	// For memory pooling
	HandleVector *next;
	//@} Jaewon
};

typedef struct HandleMatrix4x4 HandleMatrix4x4;
struct HandleMatrix4x4
{
    D3DXMATRIX data;
    //@{ Jaewon 20041102
	// lighting optimization
    D3DXHANDLE handle[FX_LIGHTING_VARIATION];
	//@} Jaewon
    RwChar name[FX_HANDLE_NAME_LENGTH];
    RwBool tweakable;

	//@{ Jaewon 20050722
	// For memory pooling
	HandleMatrix4x4 *next;
	//@} Jaewon
};

typedef struct HandleTexture HandleTexture;
struct HandleTexture
{
    RwTexture *data;
    //@{ Jaewon 20041102
	// lighting optimization
    D3DXHANDLE handle[FX_LIGHTING_VARIATION];
	//@} Jaewon
    RwChar name[FX_HANDLE_NAME_LENGTH];
	RwBool tweakable;

	//@{ Jaewon 20050722
	// For memory pooling
	HandleTexture *next;
	//@} Jaewon
};

typedef struct HandleRw HandleRw;
struct HandleRw
{
    //@{ Jaewon 20041102
	// lighting optimization
    D3DXHANDLE handle[FX_LIGHTING_VARIATION];
	//@} Jaewon
    RwChar name[FX_HANDLE_NAME_LENGTH];
};

//@{ Jaewon 20050722
// ;)
#define MAX_TWEAKABLE_PARAM_COUNT 32
//@} Jaewon
typedef struct DxEffectTweakableParams DxEffectTweakableParams;
struct DxEffectTweakableParams
{
    //@{ Jaewon 20050722
	// No dyanmic allocation
	RwUInt8 type[MAX_TWEAKABLE_PARAM_COUNT];  /* one of the EFFECT_TWEAKABLE_PARAM defines */
	//@} Jaewon
    RwUInt32 count;
};

typedef struct SharedD3dXEffectPtr SharedD3dXEffectPtr;

struct SharedD3dXEffectPtr
{
	//@{ Jaewon 20041102
	// lighting optimization
	LPD3DXEFFECT d3dxEffect[FX_LIGHTING_VARIATION];
	//@} Jaewon
	RwUInt32 refCount;
	RwChar name[FX_FILE_NAME_LENGTH];
	//@{ Jaewon 20040922
	// bSkin -> nWeights
	RwUInt32 nWeights;
	//@} Jaewon
	//@{ Jaewon 20050126
	BOOL technicSet;
	//@} Jaewon
};

typedef struct DxEffect DxEffect;

//@{ Jaewon 20041223
// nLights parameter added.
typedef void (*EffectConstUploadCallBack)(RpAtomic *atomic, RpMaterial *material, DxEffect *effect,
    RwUInt32 pass, RwUInt32 nLights);
//@} Jaewon

struct DxEffect
{   
    SharedD3dXEffectPtr *pSharedD3dXEffect;
    HandleFloat *real;
    RwUInt32 realCount;
    HandleVector *vector;
    RwUInt32 vectorCount;
    HandleMatrix4x4 *matrix4x4;
    RwUInt32 matrix4x4Count;
    HandleTexture *texture;
    RwUInt32 textureCount;
    HandleRw rw[ID_TOTAL];
	enum SpaceAnnotation cameraPositionSpace;
	enum SpaceAnnotation cameraDirectionSpace;
	enum SpaceAnnotation lightDirectionSpace;
	//@{ Jaewon 20050419
	// Up to 8 lights
	enum SpaceAnnotation localLightPositionSpace[8];
	enum SpaceAnnotation localLightDirectionSpace[8];
	//@} Jaewon
    EffectConstUploadCallBack constUploadCallBack;
    DxEffectTweakableParams tweakableParams;
	BOOL selected;
};

typedef struct EffectGUIParams EffectGUIParams;
struct EffectGUIParams
{
    RwChar *name;
    RwChar *widget;
    RwChar *guiName;
    RwReal minRange;
    RwReal maxRange;
    RwReal step;
};

/* param effect - pointer to the effect
   param type   - data type of the effect parameter
   param name   - pointer to the name of the effect parameter
   param data   - pointer to the data for the effect parameter. the callback is expected to cast to the
                  correct type
   param callbackData - pointer to user-supplied data to pass to the callback function
   */
typedef void* (*EffectTweakableParameterCallBack)(DxEffect *effect, RwUInt32 type, RwChar *name, void *data,
    void *callbackData);

//@{ Jaewon 20040917
// moved here from effect.cpp.
typedef void* (*D3dXEffectCallBack)(SharedD3dXEffectPtr *, void *data);
//@} Jaewon

extern LPDIRECT3DDEVICE9 EffectD3DDevice;
extern RwBool EffectInitMatricesToIdentity;

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern void
EffectNullD3DDevice(void);

extern void
EffectSetD3DDevice(void);

extern void
EffectDestroyEffectFreelist(void);

extern void
EffectCreateEffectFreelist(void);

extern void
EffectResetDeviceLostRestoreCallBacks(void);

extern void
EffectSetDeviceLostRestoreCallBacks(void);

extern void
EffectBuildParams(DxEffect *effect);

extern DxEffect *
//@{ Jaewon 20040922
// bSkin -> nWeights
EffectLoad(RwChar *fxName, RwUInt32 nWeights);
//@} Jaewon

extern void
EffectRelease(DxEffect *effect);

extern RwUInt32
EffectGetSize(DxEffect *effect);

extern DxEffect *
EffectRead(RwStream *stream);

extern RwBool
EffectWrite(DxEffect *effect, RwStream *stream);

extern void
EffectForAllTweakableParameters(DxEffect *effect, EffectTweakableParameterCallBack callback, void *data);

extern void 
EffectSetDefaultGUIRanges(RwReal minRange, RwReal maxRange, RwReal step);

extern void 
EffectGetDefaultGUIRanges(RwReal *minRange, RwReal *maxRange, RwReal *step);

extern RwBool
EffectSetFirstValidTechnique(DxEffect *effect);

extern RwUInt32
EffectGetNumberOfTechniques(DxEffect *effect);

extern const RwChar *
EffectGetCurrentTechniqueName(DxEffect *effect);

extern RwBool
EffectSetTexture(DxEffect *effect, RwChar *name, RwTexture *data);

extern RwBool
EffectSetMatrix4x4(DxEffect *effect, RwChar *name, D3DXMATRIX *data);

extern RwBool
EffectSetVector(DxEffect *effect, RwChar *name, D3DXVECTOR4 *data);

extern RwBool
EffectSetFloat(DxEffect *effect, RwChar *name, RwReal data);

//@{ Jaewon 20041021
extern RwBool
EffectGetTexture(DxEffect *effect, RwChar *name, RwTexture **data);

extern RwBool
EffectGetMatrix4x4(DxEffect *effect, RwChar *name, D3DXMATRIX *data);

extern RwBool
EffectGetVector(DxEffect *effect, RwChar *name, D3DXVECTOR4 *data);

extern RwBool
EffectGetFloat(DxEffect *effect, RwChar *name, RwReal *data);
//@} Jaewon

extern void
EffectParameterGetGUI(DxEffect *effect, RwChar *parameterName, EffectGUIParams *params);

//@{ Jaewon 20040917
// no static
//@{ Jaewon 20041022
// SharedD3dXEffectPtr -> DxEffect
extern DxEffect *
ForAllD3dXEffectsInFreelist(D3dXEffectCallBack callback, void *data);
//@} Jaewon
//@} Jaewon

//@{ Jaewon 20041007
// for material copying
extern DxEffect *
EffectCopy(DxEffect *effect);
//@} Jaewon

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* __EFFECT_H__ */

