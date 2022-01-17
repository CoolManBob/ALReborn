//@{ Jaewon 20040813
// copied & modified.
// a renderware rendering pipeline which uses a d3d fx file.
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
 * pipeline.c
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
#include "rphanim.h"
#include "rpskin.h"
#include "myassert.h"
#include "AcuRpMatD3DFx.h"
#include "effect.h"
#include "rwd3d9.h"
#include "pipeline.h"
//@{ Jaewon 20040910
#include <time.h>
//@} Jaewon

#define NUMCLUSTERSOFINTEREST   (0)
#define NUMOUTPUTS              (0)

/* the total number of bones an atomic and the example vertex shaders can support. This of course can be
   changed by the shader programmer */
//@{ Jaewon 20041005
// 128 -> 256
#define MAX_NUMBER_OF_BONES         (256)
//@} Jaewon
#define MAX_BONES_CONSTANT_SPACE    (MAX_NUMBER_OF_BONES * 3)

#define NUM_DECLARATIONS (20)

extern D3DMATRIX _RwD3D9D3D9ViewTransform;
extern D3DMATRIX _RwD3D9D3D9ProjTransform;

//@{ Jaewon 20041026
// critical section object for multi-threaded loading
//extern CRITICAL_SECTION _criticalSection; 
//@} Jaewon

typedef struct myD3D9InstanceNodeData myD3D9InstanceNodeData;
struct myD3D9InstanceNodeData
{
    /* I could have created my own type of callbacks if I wanted */
    RxD3D9AllInOneInstanceCallBack instanceCallback;
    RxD3D9AllInOneReinstanceCallBack reinstanceCallback;
    RxD3D9AllInOneRenderCallBack renderCallback;
};

static RxPipeline *Pipeline = NULL;
static D3DXMATRIX D3DMatrix;
static D3DXVECTOR4 D3DVectors[MAX_BONES_CONSTANT_SPACE];

extern RwResEntry *
_rxD3D9Instance(void *object, void *owner, RwUInt8 type, RwResEntry **resEntryPointer,
    RpMeshHeader *meshHeader, RxD3D9AllInOneInstanceCallBack instanceCallback,
    RwBool allocateNative);

// for the skin split rendering
const RwUInt8 *meshBatches;
const RwUInt8 *boneBatches;

// RpSkin definitions for 'skinSplitData' access
typedef void * SkinUnaligned;

typedef struct SkinPlatformData         SkinPlatformData;
typedef struct SkinAtomicPlatformData   SkinAtomicPlatformData;
typedef struct SkinGlobalPlatform       SkinGlobalPlatform;

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

//@{ Jaewon 20040923
//@{ Jaewon 20050418
// 2 -> 8
extern RpWorldGetAtomicLocalLightsCallBack getAtomicLocalLightsCB;
//@} Jaewon
//@} Jaewon

//@{ Jaewon 20041007
extern RwRaster* _rwD3D9RWGetRasterStage(RwUInt32 stage);
extern RwBool _rwD3D9RWSetRasterStage(RwRaster *raster, RwUInt32 stage);
extern void _rwD3D9RenderStateFlushCache(void);
//@} Jaewon

/*
 *************************************************************************************************************
 */

// just return the first ambient light found.
static RpLight* getAmbientLight()
{
    RpWorld     *world;
    RwLLLink    *cur, *end;

	//@{ Jaewon 20040915
	// world can be null.
	//my_assert(RWSRCGLOBAL(curWorld));

    world = (RpWorld *)RWSRCGLOBAL(curWorld);
	if(world == NULL)
		return NULL;
	//@} Jaewon

    cur = rwLinkListGetFirstLLLink(&world->directionalLightList);
    end = rwLinkListGetTerminator(&world->directionalLightList);
    while (cur != end)
    {
        RpLight *light;

        light = rwLLLinkGetData(cur, RpLight, inWorld);

        /* NB light may actually be a dummyTie from a enclosing ForAll */
        if (light && (rwObjectTestFlags(light, rpLIGHTLIGHTATOMICS)))
        {
            if (RpLightGetType(light) == rpLIGHTAMBIENT)
            {
				return light;
            }
        }

        /* Next */
        cur = rwLLLinkGetNext(cur);
	}

	return NULL;
}

// just return the first directional light found.
static RpLight* getDirectionalLight()
{
    RpWorld     *world;
    RwLLLink    *cur, *end;

	//@{ Jaewon 20040915
	// world can be null.
	//my_assert(RWSRCGLOBAL(curWorld));

    world = (RpWorld *)RWSRCGLOBAL(curWorld);
	if(world == NULL)
		return NULL;
	//@} Jaewon

    cur = rwLinkListGetFirstLLLink(&world->directionalLightList);
    end = rwLinkListGetTerminator(&world->directionalLightList);
    while (cur != end)
    {
        RpLight *light;

        light = rwLLLinkGetData(cur, RpLight, inWorld);

        /* NB light may actually be a dummyTie from a enclosing ForAll */
        if (light && (rwObjectTestFlags(light, rpLIGHTLIGHTATOMICS)))
        {
            if (RpLightGetType(light) == rpLIGHTDIRECTIONAL)
            {
				return light;
            }
        }

        /* Next */
        cur = rwLLLinkGetNext(cur);
	}

	return NULL;
}

//@{ Jaewon 20040922
//static void getLocalLight(RpAtomic *atomic, RpLight **ppLight0, RpLight **ppLight1)
//{
//	RwLLLink    *cur, *end;
//	
//	*ppLight0 = NULL;
//	*ppLight1 = NULL;
//
//	if((RpWorld *)RWSRCGLOBAL(curWorld) == NULL)
//	{
//		return;
//	}
//
//	//@{ Jaewon 20040923
//	if(get2AtomicLocalLightsCB)
//	{
//		RpLight *pLight[2];
//		RwUInt32 nLights, i;
//		nLights = get2AtomicLocalLightsCB(atomic, &pLight[0], &pLight[1]);
//		for(i=0; i<nLights; ++i)
//		{
//            const RwMatrix  *matrixLight;
//            const RwV3d     *pos;
//            const RwSphere  *sphere;
//            RwV3d           distanceVector;
//            RwReal          distanceSquare;
//            RwReal          distanceCollision;
//
//            /* Does the light intersect the atomics bounding sphere */
//            matrixLight = RwFrameGetLTM(RpLightGetFrame(pLight[i]));
//
//            pos = &(matrixLight->pos);
//
//            sphere = RpAtomicGetWorldBoundingSphere(atomic);
//
//            RwV3dSub(&distanceVector, &(sphere->center), pos);
//
//            distanceSquare = RwV3dDotProduct(&distanceVector, &distanceVector);
//
//            distanceCollision = (sphere->radius + RpLightGetRadius(pLight[i]));
//
//            if (distanceSquare < (distanceCollision * distanceCollision))
//            {
//                switch (RpLightGetType(pLight[i]))
//                {
//                    case rpLIGHTPOINT:
//                    case rpLIGHTSPOT:
//                    case rpLIGHTSPOTSOFT:
//						if(*ppLight0 == NULL)
//							*ppLight0 = pLight[i];
//						else
//						{
//							*ppLight1 = pLight[i];
//							return;
//						}
//                        break;
//
//                    default:
//                        break;
//                }
//            }
//		}
//	}
//	//@} Jaewon
//
//	/* For all sectors that this atomic lies in, apply all lights within */
//	cur = rwLinkListGetFirstLLLink(&atomic->llWorldSectorsInAtomic);
//	end = rwLinkListGetTerminator(&atomic->llWorldSectorsInAtomic);
//	while (cur != end)
//	{
//		RpTie       *tpTie = rwLLLinkGetData(cur, RpTie, lWorldSectorInAtomic);
//		RwLLLink    *curLight, *endLight;
//
//		/* Lights in the sector */
//		curLight = rwLinkListGetFirstLLLink(&tpTie->worldSector->lightsInWorldSector);
//		endLight = rwLinkListGetTerminator(&tpTie->worldSector->lightsInWorldSector);
//
//		while (curLight != endLight)
//		{
//			RpLightTie  *lightTie;
//			RpLight     *light;
//
//			lightTie = rwLLLinkGetData(curLight, RpLightTie, lightInWorldSector);
//
//			light = lightTie->light;
//
//			/* NB lightTie may actually be a dummyTie from a enclosing ForAll */
//
//			/* Check to see if the light has already been applied and is set to
//			* light atomics
//			*/
//			if ( light != NULL &&
//				(rwObjectTestFlags(light, rpLIGHTLIGHTATOMICS)))
//			{
//				const RwMatrix  *matrixLight;
//				const RwV3d     *pos;
//				const RwSphere  *sphere;
//				RwV3d           distanceVector;
//				RwReal          distanceSquare;
//				RwReal          distanceCollision;
//
//				/* Does the light intersect the atomics bounding sphere */
//				matrixLight = RwFrameGetLTM(RpLightGetFrame(light));
//
//				pos = &(matrixLight->pos);
//
//				sphere = RpAtomicGetWorldBoundingSphere(atomic);
//
//				RwV3dSub(&distanceVector, &(sphere->center), pos);
//
//				distanceSquare = RwV3dDotProduct(&distanceVector, &distanceVector);
//
//				distanceCollision = (sphere->radius + RpLightGetRadius(light));
//
//				if (distanceSquare < (distanceCollision * distanceCollision))
//				{
//					switch (RpLightGetType(light))
//					{
//						case rpLIGHTPOINT:
//						case rpLIGHTSPOT:
//						case rpLIGHTSPOTSOFT:
//							if(*ppLight0 == NULL)
//								*ppLight0 = light;
//							else
//							{
//								*ppLight1 = light;
//								return;
//							}
//							break;
//
//						default:
//							break;
//					}
//				}
//			}
//
//			/* Next */
//			curLight = rwLLLinkGetNext(curLight);
//		}
//
//		/* Next one */
//		cur = rwLLLinkGetNext(cur);
//	}
//}
//@} Jaewon

//@{ Jaewon 20040908
// in case of an effect sharing, you should update shader constants at every frame.
//@{ Jaewon 20041102
// lighting optimization
static void EffectUpdateShaderConstants(DxEffect *effect, RwUInt32 nLights)
{
	unsigned int i;
	HandleFloat *handleFloat;
	HandleVector *handleVector;
	HandleMatrix4x4 *handleMatrix;
	HandleTexture *handleTexture;

	my_assert(nLights<FX_LIGHTING_VARIATION);

	//@{ Jaewon 20050722
	// For memory pooling
	// float
	handleFloat = effect->real;
	for(i=0; i<effect->realCount; ++i)
	{
		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl
			->SetFloat(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
			handleFloat->handle[nLights],
			handleFloat->data);

		handleFloat = handleFloat->next;
	}

	// vector
	handleVector = effect->vector;
	for(i=0; i<effect->vectorCount; ++i)
	{
		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl
			->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
			handleVector->handle[nLights],
			&(handleVector->data));

		handleVector = handleVector->next;
	}

	// matrix
	handleMatrix = effect->matrix4x4;
	for(i=0; i<effect->matrix4x4Count; ++i)
	{
		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl
			->SetMatrix(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
			handleMatrix->handle[nLights],
			&(handleMatrix->data));

		handleMatrix = handleMatrix->next;
	}

	// texture
	handleTexture = effect->texture;
	for(i=0; i<effect->textureCount; ++i)
	{
		if(handleTexture->data == NULL)
			continue;

		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl
			->SetTexture(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
			handleTexture->handle[nLights],
			D3DTextureFromRwTexture(handleTexture->data));

		handleTexture = handleTexture->next;
	}
	//@} Jaewon
}
//@} Jaewon
//@} Jaewon

//@{ Jaewon 20041102
// lighting optimization
static void
EffectUploadRenderWareShaderConstants(DxEffect *effect, RpAtomic *atomic, RpMaterial *material, RwUInt32 nLights, RpLight *pLight[])
{
    D3DXVECTOR4 d3dVector;
	int i;

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(atomic);

	if(effect->rw[ID_WORLD_VIEW_PROJ_MATRIX].handle[nLights])
	{
        _rwD3D9VSGetComposedTransformMatrix(&D3DMatrix);
        effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetMatrixTranspose(effect->pSharedD3dXEffect->d3dxEffect[nLights], effect->rw[ID_WORLD_VIEW_PROJ_MATRIX].handle[nLights],
            &D3DMatrix);
	}
	if(effect->rw[ID_PROJ_MATRIX].handle[nLights])
	{
        _rwD3D9VSGetProjectionTransposedMatrix(&D3DMatrix);
        effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetMatrixTranspose(effect->pSharedD3dXEffect->d3dxEffect[nLights], effect->rw[ID_PROJ_MATRIX].handle[nLights],
            &D3DMatrix);
	}
	if(effect->rw[ID_WORLD_VIEW_MATRIX].handle[nLights])
	{
        _rwD3D9VSGetWorldViewTransposedMatrix(&D3DMatrix);
        effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetMatrixTranspose(effect->pSharedD3dXEffect->d3dxEffect[nLights], effect->rw[ID_WORLD_VIEW_MATRIX].handle[nLights],
            &D3DMatrix);
	}
	if(effect->rw[ID_WORLD_MATRIX].handle[nLights])
	{
		_rwD3D9VSGetWorldNormalizedTransposeMatrix(&D3DMatrix);
        effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetMatrixTranspose(effect->pSharedD3dXEffect->d3dxEffect[nLights], effect->rw[ID_WORLD_MATRIX].handle[nLights],
            &D3DMatrix);
	}
	if(effect->rw[ID_CAMERA_POSITION].handle[nLights])
	{
        RwV3d *pos;
        RwCamera *camera;
        RwV3d out;

        camera = RwCameraGetCurrentCamera();
        my_assert(camera);
        pos = RwMatrixGetPos(RwFrameGetLTM(RwCameraGetFrame(camera)));

		if(ID_LOCAL_SPACE == effect->cameraPositionSpace)
		// local space
			_rwD3D9VSGetPointInLocalSpace(pos, &out);
		else if(ID_VIEW_SPACE == effect->cameraPositionSpace)
		// view space
		{
			D3DXVec3TransformCoord((D3DXVECTOR3 *)&out,
								(D3DXVECTOR3 *)pos,
								(const D3DMATRIX *)&_RwD3D9D3D9ViewTransform);
			my_assert(RwV3dLength(&out) < 0.001f);
		}
		else if(ID_WORLD_SPACE == effect->cameraPositionSpace)
		// world space
			RwV3dAssign(&out, pos);
		else
			my_assert(0);

        d3dVector.x = out.x;
        d3dVector.y = out.y;
        d3dVector.z = out.z;
        d3dVector.w = 1.0f;
        effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], effect->rw[ID_CAMERA_POSITION].handle[nLights], &d3dVector);
	}
	if(effect->rw[ID_CAMERA_DIRECTION].handle[nLights])
	{
        RwV3d *at;
        RwCamera *camera;
        RwV3d out;

        camera = RwCameraGetCurrentCamera();
        my_assert(camera);
        at = RwMatrixGetAt(RwFrameGetLTM(RwCameraGetFrame(camera)));

		if(ID_LOCAL_SPACE == effect->cameraDirectionSpace)
		// local space
			_rwD3D9VSGetNormalInLocalSpace(at, &out);
		else if(ID_VIEW_SPACE == effect->cameraDirectionSpace)
		// view space
		{
			D3DXVec3TransformNormal((D3DXVECTOR3 *)&out,
								(D3DXVECTOR3 *)at,
								(const D3DMATRIX *)&_RwD3D9D3D9ViewTransform);
			my_assert(fabs(out.x) < 0.001f && fabs(out.y) < 0.001f);
		}
		else if(ID_WORLD_SPACE == effect->cameraDirectionSpace)
		// world space
			RwV3dAssign(&out, at);
		else
			my_assert(0);

        d3dVector.x = out.x;
        d3dVector.y = out.y;
        d3dVector.z = out.z;
        d3dVector.w = 0.0f;
        effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], effect->rw[ID_CAMERA_DIRECTION].handle[nLights], &d3dVector);
	}
	//@{ Jaewon 20041022
	// check...check...check...
	my_assert(!(RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic)) && 
				(effect->rw[ID_BONE_MATRICES].handle[nLights] == NULL)));
	//@} Jaewon
	if(effect->rw[ID_BONE_MATRICES].handle[nLights])
	{
        RpSkin *skin;

        /* upload bone matrices. pipeline does not support skin split. we assume there is enough constants
           available for the model */
        skin = RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic));
        
        /* some of the example fx files include all the RW default constant names so I test if there is
           skin instead of an assert */
        /* my_assert(skin); */
        if (skin)
        {
            const RwMatrix *skinToBone;
            RwUInt32 j, index;
            RpHAnimHierarchy *hierarchy;

            hierarchy = RpSkinAtomicGetHAnimHierarchy(atomic);
            
			//@{ Jaewon 20041008
			// no crash, just skip...
			//my_assert(hierarchy);
			if(hierarchy)
			{
            
            skinToBone = RpSkinGetSkinToBoneMatrices(skin);
            my_assert(skinToBone);
            //my_assert(hierarchy->numNodes <= MAX_NUMBER_OF_BONES);

            if (rpHANIMHIERARCHYNOMATRICES & hierarchy->flags)
            {
                RwMatrix invLtm;

                my_assert(RpAtomicGetFrame(atomic));
                my_assert(RwFrameGetLTM(RpAtomicGetFrame(atomic)));

                RwMatrixInvert(&invLtm, RwFrameGetLTM(RpAtomicGetFrame(atomic)));

                for(j=0, index=0; j!=(RwUInt32)hierarchy->numNodes; ++j, index+=3)
                {
                    RwMatrix result;
                    RwMatrix temp;
                    RwMatrix *ltm;

                    my_assert(hierarchy->pNodeInfo[j].pFrame);
                    ltm = RwFrameGetLTM(hierarchy->pNodeInfo[j].pFrame);
                    my_assert(ltm);

                    RwMatrixMultiply(&temp, &skinToBone[j], ltm);
                    RwMatrixMultiply(&result, &temp, &invLtm);

                    D3DVectors[index].x = result.right.x;
                    D3DVectors[index].y = result.up.x;
                    D3DVectors[index].z = result.at.x;
                    D3DVectors[index].w = result.pos.x;

                    D3DVectors[index+1].x = result.right.y;
                    D3DVectors[index+1].y = result.up.y;
                    D3DVectors[index+1].z = result.at.y;
                    D3DVectors[index+1].w = result.pos.y;

                    D3DVectors[index+2].x = result.right.z;
                    D3DVectors[index+2].y = result.up.z;
                    D3DVectors[index+2].z = result.at.z;
                    D3DVectors[index+2].w = result.pos.z; 
                }
            }
            else if (rpHANIMHIERARCHYLOCALSPACEMATRICES & hierarchy->flags)
            {
                RwMatrix *matrixArray;
                matrixArray = hierarchy->pMatrixArray;
                my_assert(matrixArray);

                for(j=0, index=0; j!=(RwUInt32)hierarchy->numNodes; ++j, index+=3)
                {
                    RwMatrix result;
                
                    RwMatrixMultiply(&result, &skinToBone[j], &matrixArray[j]);

                    D3DVectors[index].x = result.right.x;
                    D3DVectors[index].y = result.up.x;
                    D3DVectors[index].z = result.at.x;
                    D3DVectors[index].w = result.pos.x;

                    D3DVectors[index+1].x = result.right.y;
                    D3DVectors[index+1].y = result.up.y;
                    D3DVectors[index+1].z = result.at.y;
                    D3DVectors[index+1].w = result.pos.y;

                    D3DVectors[index+2].x = result.right.z;
                    D3DVectors[index+2].y = result.up.z;
                    D3DVectors[index+2].z = result.at.z;
                    D3DVectors[index+2].w = result.pos.z; 
                }
            }
            else
            {
                RwMatrix invLtm;
                RwMatrix *matrixArray;

                matrixArray = hierarchy->pMatrixArray;
                my_assert(matrixArray);
                my_assert(RpAtomicGetFrame(atomic));
                my_assert(RwFrameGetLTM(RpAtomicGetFrame(atomic)));

                RwMatrixInvert(&invLtm, RwFrameGetLTM(RpAtomicGetFrame(atomic)));

                for(j=0, index=0; j!=(RwUInt32)hierarchy->numNodes; ++j, index+=3)
                {
                    RwMatrix result;
                    RwMatrix temp;

                    RwMatrixMultiply(&temp, &skinToBone[j], &matrixArray[j]);
                    RwMatrixMultiply(&result, &temp, &invLtm);

                    D3DVectors[index].x = result.right.x;
                    D3DVectors[index].y = result.up.x;
                    D3DVectors[index].z = result.at.x;
                    D3DVectors[index].w = result.pos.x;

                    D3DVectors[index+1].x = result.right.y;
                    D3DVectors[index+1].y = result.up.y;
                    D3DVectors[index+1].z = result.at.y;
                    D3DVectors[index+1].w = result.pos.y;

                    D3DVectors[index+2].x = result.right.z;
                    D3DVectors[index+2].y = result.up.z;
                    D3DVectors[index+2].z = result.at.z;
                    D3DVectors[index+2].w = result.pos.z; 
                }
            }

			/* Upload matrices */
			if (meshBatches != NULL)
			{
				const RwUInt32 firstBatch = meshBatches[0];
				const RwUInt32 numBatches = meshBatches[1];
				RwUInt32 currentBatch;

				meshBatches += 2;

				for (currentBatch = 0; currentBatch < numBatches; ++currentBatch)
				{
					RwUInt32 firstBone = boneBatches[(firstBatch + currentBatch) * 2];
					RwUInt32 numBones = boneBatches[(firstBatch + currentBatch) * 2 + 1];

					do
					{
						int h;
						for(h=0; h<3; ++h)
						{
							D3DXHANDLE hElement;
							hElement = effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl
								->GetParameterElement(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
													  effect->rw[ID_BONE_MATRICES].handle[nLights], 
													  (skin->skinSplitData.matrixRemapIndices[firstBone] * 3) + h);
							//@{ Jaewon 20041024
							my_assert(hElement);
							//@} Jaewon

							effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl
								->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
											hElement,
											D3DVectors + (firstBone * 3) + h);
						}

						++firstBone;
					}
					while(--numBones);
				}
			}
			else
				effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVectorArray(effect->pSharedD3dXEffect->d3dxEffect[nLights], effect->rw[ID_BONE_MATRICES].handle[nLights], D3DVectors,
					hierarchy->numNodes * 3);
			
			}
			//@{ Jaewon 20041022
			else
			{
				// it should not be here...
				RwBool Oops = TRUE;

				//@{ Jaewon 20050407
				// Put it to a default pose.
				for(j=0; j<(RwUInt32)MAX_NUMBER_OF_BONES; ++j)
				{
					D3DVectors[3*j].x = 1.0f;
					D3DVectors[3*j].y = 0.0f;
					D3DVectors[3*j].z = 0.0f;
					D3DVectors[3*j].w = 0.0f;

					D3DVectors[3*j+1].x = 0.0f;
					D3DVectors[3*j+1].y = 1.0f;
					D3DVectors[3*j+1].z = 0.0f;
					D3DVectors[3*j+1].w = 0.0f;

					D3DVectors[3*j+2].x = 0.0f;
					D3DVectors[3*j+2].y = 0.0f;
					D3DVectors[3*j+2].z = 1.0f;
					D3DVectors[3*j+2].w = 0.0f;
				}
				/* Upload matrices */
				if (meshBatches != NULL)
				{
					const RwUInt32 firstBatch = meshBatches[0];
					const RwUInt32 numBatches = meshBatches[1];
					RwUInt32 currentBatch;

					meshBatches += 2;

					for (currentBatch = 0; currentBatch < numBatches; ++currentBatch)
					{
						RwUInt32 firstBone = boneBatches[(firstBatch + currentBatch) * 2];
						RwUInt32 numBones = boneBatches[(firstBatch + currentBatch) * 2 + 1];

						do
						{
							int h;
							for(h=0; h<3; ++h)
							{
								D3DXHANDLE hElement;
								hElement = effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl
									->GetParameterElement(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
									effect->rw[ID_BONE_MATRICES].handle[nLights], 
									(skin->skinSplitData.matrixRemapIndices[firstBone] * 3) + h);
								my_assert(hElement);

								effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl
									->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
									hElement,
									D3DVectors + (firstBone * 3) + h);
							}

							++firstBone;
						}
						while(--numBones);
					}
				}
				else
				//@{ Jaewon 20050412
				// Count should be smaller than the actual size.
				// (A different behavior from the specified in the DXSDK document)
				{
					HRESULT hr;
					D3DXPARAMETER_DESC desc;
					effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->GetParameterDesc(
								effect->pSharedD3dXEffect->d3dxEffect[nLights],
								effect->rw[ID_BONE_MATRICES].handle[nLights], 
								&desc);
					hr = effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVectorArray(
								effect->pSharedD3dXEffect->d3dxEffect[nLights], 
								effect->rw[ID_BONE_MATRICES].handle[nLights], 
								D3DVectors, desc.Elements);
					my_assert(hr==D3D_OK);
				}
				//@} Jaewon
				//@} Jaewon
			}
			//@} Jaewon
			//@} Jaewon
        }
	}
	if(effect->rw[ID_BASE_TEXTURE].handle[nLights])
	{
		RwTexture *texture;
		texture = RpMaterialGetTexture(material);
		//my_assert(texture);

		if(texture)
		{
			effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetTexture(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
											effect->rw[ID_BASE_TEXTURE].handle[nLights], 
											D3DTextureFromRwTexture(texture));
		}
	}
	if(effect->rw[ID_LIGHT_DIRECTION].handle[nLights])
	{
        RwV3d *at;
        RpLight *light;
        RwV3d out;

        light = getDirectionalLight();
		//@{ Jaewon 20040915
		// there can be no light.
		if(light)
		{
        at = RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(light)));

		if(ID_LOCAL_SPACE == effect->lightDirectionSpace)
		// local space
			_rwD3D9VSGetNormalInLocalSpace(at, &out);
		else if(ID_VIEW_SPACE == effect->lightDirectionSpace)
		// view space
		{
			D3DXVec3TransformNormal((D3DXVECTOR3 *)&out,
								(D3DXVECTOR3 *)at,
								(const D3DMATRIX *)&_RwD3D9D3D9ViewTransform);
		}
		else if(ID_WORLD_SPACE == effect->lightDirectionSpace)
		// world space
			RwV3dAssign(&out, at);
		else
			my_assert(0);
		}
		else
		{
			out.x = out.y = 0.0f;
			out.z = 1.0f;
		}
		//@} Jaewon

        d3dVector.x = out.x;
        d3dVector.y = out.y;
        d3dVector.z = out.z;
        d3dVector.w = 0.0f;
        effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], effect->rw[ID_LIGHT_DIRECTION].handle[nLights], &d3dVector);
	}
	if(effect->rw[ID_LIGHT_COLOR].handle[nLights])
	{
		const RwRGBAReal *color;
		RpLight *light;
		//@{ Jaewon 20040915
		// no light = black color
		RwRGBAReal black = { 0.0f, 0.0f, 0.0f, 1.0f };

		light = getDirectionalLight();
        if(light)
			color = RpLightGetColor(light);
		else
			color = &black;
		//@} Jaewon

		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
								effect->rw[ID_LIGHT_COLOR].handle[nLights], 
								(const D3DXVECTOR4 *)color);
	}
	if(effect->rw[ID_AMBIENT].handle[nLights])
	{
		RwRGBAReal color;
		RwRGBAReal ambient;
		const RwSurfaceProperties *surfaceProp;

		RwRGBARealFromRwRGBA(&color, (RwRGBA *)RpMaterialGetColor(material));
		surfaceProp = RpMaterialGetSurfaceProperties(material);
		RwRGBARealScale(&ambient, &color, surfaceProp->ambient);

		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
								effect->rw[ID_AMBIENT].handle[nLights], 
								(const D3DXVECTOR4 *)&ambient);
	}
	if(effect->rw[ID_DIFFUSE].handle[nLights])
	{
		RwRGBAReal color;
		RwRGBAReal diffuse;
		const RwSurfaceProperties *surfaceProp;

		RwRGBARealFromRwRGBA(&color, (RwRGBA *)RpMaterialGetColor(material));
		surfaceProp = RpMaterialGetSurfaceProperties(material);
		RwRGBARealScale(&diffuse, &color, surfaceProp->diffuse);

		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
								effect->rw[ID_DIFFUSE].handle[nLights], 
								(const D3DXVECTOR4 *)&diffuse);
	}
	if(effect->rw[ID_SPECULAR].handle[nLights])
	{
		RwRGBAReal color;
		RwRGBAReal specular;
		const RwSurfaceProperties *surfaceProp;

		RwRGBARealFromRwRGBA(&color, (RwRGBA *)RpMaterialGetColor(material));
		surfaceProp = RpMaterialGetSurfaceProperties(material);
		RwRGBARealScale(&specular, &color, surfaceProp->specular);

		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
								effect->rw[ID_SPECULAR].handle[nLights], 
								(const D3DXVECTOR4 *)&specular);
	}
	if(effect->rw[ID_LIGHT_AMBIENT].handle[nLights])
	{
		const RwRGBAReal *color;
		RpLight *light;
		//@{ Jaewon 20040915
		// no light = black color
		RwRGBAReal black = { 0.0f, 0.0f, 0.0f, 1.0f };

		light = getAmbientLight();
        if(light)
			color = RpLightGetColor(light);
		else 
			color = &black;
		//@} Jaewon

		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
								effect->rw[ID_LIGHT_AMBIENT].handle[nLights], 
								(const D3DXVECTOR4 *)color);
	}
	if(effect->rw[ID_FOG].handle[nLights])
	{
		const RwCamera *camera = RwCameraGetCurrentCamera();
        const RwReal fogPlane  = camera->fogPlane;
        const RwReal farPlane  = camera->farPlane;
		RwV4d fogData;

        /* Fog Constant: x = -1/(fog_end-fog_start), y = fog_end/(fog_end-fog_start) */
        fogData.x = -1.f / (farPlane - fogPlane);
        fogData.y = farPlane / (farPlane - fogPlane);
        fogData.z = 0.0f;
        fogData.w = 1.0f;

		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
								effect->rw[ID_FOG].handle[nLights], 
								(const D3DXVECTOR4 *)&fogData);
	}
	//@{ Jaewon 20040910
	if(effect->rw[ID_TIME].handle[nLights])
	{
		effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetFloat(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
								effect->rw[ID_TIME].handle[nLights], 
								(float)clock()/(float)CLOCKS_PER_SEC);
	}
	//@} Jaewon
	//@{ Jaewon 20040922
	{
	// set local light parameters.
	//@{ Jaewon 20050419
	// Up to 8 lights
	for(i=0; i<8; ++i)
	{
		if(pLight[i] == NULL)
		{
			if(effect->rw[ID_LIGHT_COLOR0+i].handle[nLights])
			{
				// just set its color to 'black'.
				RwRGBAReal black = { 0.0f, 0.0f, 0.0f, 1.0f };
				effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
															effect->rw[ID_LIGHT_COLOR0+i].handle[nLights], 
															(const D3DXVECTOR4 *)&black);
			}
		}
		else
		{
			// color
			if(effect->rw[ID_LIGHT_COLOR0+i].handle[nLights])
			{
				effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
															effect->rw[ID_LIGHT_COLOR0+i].handle[nLights], 
															(const D3DXVECTOR4 *)RpLightGetColor(pLight[i]));
			}
			// parameter
			if(effect->rw[ID_LIGHT_PARAMETERS0+i].handle[nLights])
			{
				// {1/r, 1/(1-cos), -cos/(1-cos), 0}
				D3DXVECTOR4 param;
				float r, c;
				r = RpLightGetRadius(pLight[i]);
				param.x = r!=0.0f ? 1.0f/r : 0.0f;
				if(RpLightGetType(pLight[i]) == rpLIGHTPOINT)
				{
					param.y = 0.0f;
					param.z = 1.0f;
				}
				else
				{
					c = (float)RwCos(RpLightGetConeAngle(pLight[i]));
					param.y = c>0.999f ? 1/(0.001f) : 1/(1-c);
					param.z = -c * param.y;
				}
				param.w = 0.0f;
				effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], 
															effect->rw[ID_LIGHT_PARAMETERS0+i].handle[nLights], 
															&param);
			}
			// position
			if(effect->rw[ID_LIGHT_POSITION0+i].handle[nLights])
			{
				RwV3d *pos;
				RwV3d out;
				enum SpaceAnnotation space;

				pos = RwMatrixGetPos(RwFrameGetLTM(RpLightGetFrame(pLight[i])));

				//@{ Jaewon 20050419
				// Up to 8 lights
				space = effect->localLightPositionSpace[i];
				//@} Jaewon
				
				if(ID_LOCAL_SPACE == space)
				// local space
					_rwD3D9VSGetPointInLocalSpace(pos, &out);
				else if(ID_VIEW_SPACE == space)
				// view space
				{
					D3DXVec3TransformCoord((D3DXVECTOR3 *)&out,
										(D3DXVECTOR3 *)pos,
										(const D3DMATRIX *)&_RwD3D9D3D9ViewTransform);
				}
				else if(ID_WORLD_SPACE == space)
				// world space
					RwV3dAssign(&out, pos);
				else
					my_assert(0);

				d3dVector.x = out.x;
				d3dVector.y = out.y;
				d3dVector.z = out.z;
				d3dVector.w = 1.0f;
				effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], effect->rw[ID_LIGHT_POSITION0+i].handle[nLights], &d3dVector);
			}
			// direction
			if(effect->rw[ID_LIGHT_DIRECTION0+i].handle[nLights])
			{
				RwV3d *at;
				RwV3d out;
				enum SpaceAnnotation space;

				at = RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(pLight[i])));

				//@{ Jaewon 20050419
				// Up to 8 lights
				space = effect->localLightDirectionSpace[i];
				//@} Jaewon

				if(ID_LOCAL_SPACE == space)
				// local space
					_rwD3D9VSGetNormalInLocalSpace(at, &out);
				else if(ID_VIEW_SPACE == space)
				// view space
				{
					D3DXVec3TransformNormal((D3DXVECTOR3 *)&out,
										(D3DXVECTOR3 *)at,
										(const D3DMATRIX *)&_RwD3D9D3D9ViewTransform);
				}
				else if(ID_WORLD_SPACE == space)
				// world space
					RwV3dAssign(&out, at);
				else
					my_assert(0);

				d3dVector.x = out.x;
				d3dVector.y = out.y;
				d3dVector.z = out.z;
				d3dVector.w = 0.0f;
				effect->pSharedD3dXEffect->d3dxEffect[nLights]->lpVtbl->SetVector(effect->pSharedD3dXEffect->d3dxEffect[nLights], effect->rw[ID_LIGHT_DIRECTION0+i].handle[nLights], &d3dVector);
			}
		}
	}
	//@} Jaewon
	}
	//@} Jaewon

    return;
}
//@} Jaewon


/*
 *************************************************************************************************************
 */
static void
AtomicFxRenderCallBack(RwResEntry *repEntry, void *object, RwUInt8 type, RwUInt32 flags)
{
    RxD3D9ResEntryHeader *resEntryHeader;
    RxD3D9InstanceData *instancedData;
    RpAtomic *atomic;
    RwInt32 numMeshes;
    RwMatrix *ltm;
    RwBool noTextures;
    RpGeometry *geometry;
	RpSkin *skin;
	//@{ Jaewon 20041119
	// .net found a hidden bug...
    RwUInt32 numPasses;
    HRESULT hr;
	//@} Jaewon
	//@{ Jaewon 20041006
	LPD3DXEFFECT d3dxEffectLast = NULL;
	//@} Jaewon
	//@{ Jaewon 20050203
	D3DCOLOR fogColor;
	//@} Jaewon

    my_assert(RpMaterialD3DFxModule.numInstances > 0);

    _rwD3D9EnableClippingIfNeeded(object, type);

    resEntryHeader = (RxD3D9ResEntryHeader *)(repEntry + 1);
    if (resEntryHeader->indexBuffer != NULL)
    {
        RwD3D9SetIndices(resEntryHeader->indexBuffer);
    }

    _rwD3D9SetStreams(resEntryHeader->vertexStream, resEntryHeader->useOffsets);
    RwD3D9SetVertexDeclaration(resEntryHeader->vertexDeclaration);

    atomic = (RpAtomic *)object;
    ltm = RwFrameGetLTM(RpAtomicGetFrame(atomic));
    _rwD3D9VSSetActiveWorldMatrix(ltm);
 
    geometry = RpAtomicGetGeometry(atomic);
    my_assert(geometry);
    noTextures = RpGeometryGetNumTexCoordSets(geometry) == 0;

	/* Get skin split data */
    skin = RpSkinGeometryGetSkin(geometry);
	meshBatches = skin?skin->skinSplitData.meshRLECount:NULL;
	boneBatches = skin?skin->skinSplitData.meshRLE:NULL;

    instancedData = (RxD3D9InstanceData *)(resEntryHeader + 1);
    numMeshes = resEntryHeader->numMeshes;
    while (numMeshes--)
    {
        RpMaterial *material;
        RpMaterialD3DFxExt *matData;
        DxEffect *effect;
        LPD3DXEFFECT d3dxEffect;
        RwUInt32 i;
		//@{ Jaewon 20050418
		// 2 -> 8
		RpLight *pLight[8];
		//@} Jaewon
		RwUInt32 nLights = 0;

        material = instancedData->material;
        my_assert(material);
        matData = MATERIAL_GET_FX_DATA(material);
        effect = matData->effect;

        /* the example viewer allows materials in the atomic not to have an effect applied, so I need to have
           this test rather than the assert */
        //@{ Jaewon 20041011
		//my_assert(effect);
        if(NULL == effect)
        {
			//@{ Jaewon 20041019
			// just skip.
			/*
            RwD3D9SetVertexShader(NULL);
            RwD3D9SetPixelShader(NULL);
            if (resEntryHeader->indexBuffer != NULL)
            {
                RwD3D9DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
                    instancedData->baseIndex,0, instancedData->numVertices,
                    instancedData->startIndex, instancedData->numPrimitives);
            }
            else
            {
                RwD3D9DrawPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
                    instancedData->baseIndex, instancedData->numPrimitives);
            }*/
			//@} Jaewon
            ++instancedData;
            continue;
        }
		//@} Jaewon

		//@{ Jaewon 20041102
		// lighting optimization
		//@{ Jaewon 20050418
		// 2 -> 8
		memset(pLight, 0, 8*sizeof(RpLight*));
		//@{ Jaewon 20050421
		// getAtomicLocalLightsCB can be null! --;
		if(getAtomicLocalLightsCB)
			nLights = getAtomicLocalLightsCB(atomic, pLight);
		else
			nLights = 0;
		//@} Jaewon
		if(nLights >= 2)
			nLights = 2;
		//@} Jaewon
        d3dxEffect = effect->pSharedD3dXEffect->d3dxEffect[nLights];
        my_assert(d3dxEffect);
		//@} Jaewon

		//@{ Jaewon 20041025
		my_assert(!skin || (skin->vertexMaps.maxWeights == effect->pSharedD3dXEffect->nWeights));
		//@} Jaewon

        /* How you could improve performance:
           i.   if all materials in an atomic share the same effect then you can move Begin/End calls outside
                of this per mesh loop
           ii.  create an interface to ID3DXEffectStateManager and use ID3DXEffect::SetStateManager. In the
                interface callbacks make use of RW's state cache with the RwD3D9 API. You would then pass the
                appropriate flags into ID3DXEffect::Begin. Note RwD3D9SetTexture takes a RwTexture and not a
                LPDIRECT3DBASETEXTURE9 so for ID3DXEffectStateManager::SetTexture IDirect3DDevice8_SetTexture
                should be called directly.  To make sure RW's cache does not get out of sync before
                ID3DXEffect::Pass cache the current rasters for all stages with _rwD3D9RWGetRasterStage. After
                the call to RwD3D9Draw*Primitive restore RW's state by calling,
                    // this call is require to make sure the second calls IDirect3DDevice8_SetTexture
                    _rwD3D9RWSetRasterStage(i, NULL);
                    _rwD3D9RWSetRasterStage(i, YourCacheRasterForStage_i);

                Before the call to RwD3D9Draw*Primitive you must call _rwD3D9RenderStateFlushCache();

                You will need to extern the following function prototypes,
                    RwRaster* _rwD3D9RWGetRasterStage(RwUInt32 stage);
                    RwBool _rwD3D9RWSetRasterStage(RwRaster *raster, RwUInt32 stage);
                    void _rwD3D9RenderStateFlushCache(void);
           iii. Do not do what the code predicatd on USE_RWSKEL_MENU does
           */

		//@{ Jaewon 20050203
		// Back up fog color - quick, but ugly hack--;.
		RwD3D9GetRenderState(D3DRS_FOGCOLOR, &fogColor);
		//@} Jaewon

		//@{ Jaewon 20041006
		// d3d effect caching for fast rendering
		if(d3dxEffect != d3dxEffectLast)
		{
			if(d3dxEffectLast)
				d3dxEffectLast->lpVtbl->End(d3dxEffectLast);

			//@{ Jaewon 20050126
			if(effect->pSharedD3dXEffect->technicSet == FALSE)
			{
				for(i=0; i < FX_LIGHTING_VARIATION; ++i)
				{
					D3DXHANDLE handle;
					effect->pSharedD3dXEffect->d3dxEffect[i]->lpVtbl->FindNextValidTechnique(effect->pSharedD3dXEffect->d3dxEffect[i], NULL, &handle);
					effect->pSharedD3dXEffect->d3dxEffect[i]->lpVtbl->SetTechnique(effect->pSharedD3dXEffect->d3dxEffect[i], handle);
				}

				effect->pSharedD3dXEffect->technicSet = TRUE;
			}
			//@} Jaewon

			// D3DXFX_DONOTSAVESTATE
			//@{ 2005308
			// D3DXFX_DONOTSAVESTATE -> 0, Strange Nvidia bug! --+
	        hr = d3dxEffect->lpVtbl->Begin(d3dxEffect, &numPasses, 0);//D3DXFX_DONOTSAVESTATE);
			//@} Jaewon
		}
		//@} Jaewon
        if (SUCCEEDED(hr))
        {
			RwTexture *pTexture;
			RwRaster *backup[8];

			//@{ Jaewon 20040908
			// in case of an effect sharing, you should update shader constants at every frame.
			//@{ Jaewon 20041021
			//if(effect->pSharedD3dXEffect->refCount > 1)
			//@} Jaewon
			//@{ Jaewon 20041102
			// lighting optimization
				EffectUpdateShaderConstants(effect, nLights);
			//@} Jaewon
			//@} Jaewon

			//@{ Jaewon 20041102
			// lighting optimization
            EffectUploadRenderWareShaderConstants(effect, atomic, material, nLights, pLight);
			//@} Jaewon

			//@{ Jaewon 20041019
			// use RW versions instead for caching validity.
			if(effect->selected)
				RwD3D9SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			else
				RwD3D9SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

			//@{ Jaewon 20040907
			// if the base texture has alpha channel, enable alpha blending.
			pTexture = RpMaterialGetTexture(material);
			//@{ Jaewon 20041021
			// diffuse alpha condition added.
			if((pTexture && RwTextureHasAlpha(pTexture)) ||
			   (RpMaterialGetColor(material)->alpha != 0xff))
			//@} Jaewon
			{
				RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				RwD3D9SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				RwD3D9SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}
			//@} Jaewon
			//@} Jaewon

			//d3dxEffect->lpVtbl->CommitChanges(d3dxEffect);

			//@{ Jaewon 20041007
			// keep sync with RW's texture caches.
			// back up rasters.
			for(i=0; i<8; ++i)
				backup[i] = _rwD3D9RWGetRasterStage(i);
			//@} Jaewon

            for (i=0; i<numPasses; ++i)
            {
                if (effect->constUploadCallBack)
                {
					//@{ Jaewon 20041223
					// nLights parameter added.
                    effect->constUploadCallBack(atomic, material, effect, i, nLights);
					//@} Jaewon
                }

				_rwD3D9RenderStateFlushCache();

                hr = d3dxEffect->lpVtbl->BeginPass(d3dxEffect, i);

                if (resEntryHeader->indexBuffer != NULL)
                {
                    RwD3D9DrawIndexedPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
                        instancedData->baseIndex,0, instancedData->numVertices,
                        instancedData->startIndex, instancedData->numPrimitives);
                }
                else
                {
                    RwD3D9DrawPrimitive((D3DPRIMITIVETYPE)resEntryHeader->primType,
                        instancedData->baseIndex, instancedData->numPrimitives);
                }

				d3dxEffect->lpVtbl->EndPass(d3dxEffect);
            }

			//@{ Jaewon 20041007
			// restore rasters.
			for(i=0; i<8; ++i)
			{
				_rwD3D9RWSetRasterStage(NULL, i);
				_rwD3D9RWSetRasterStage(backup[i], i);
			}
			//@} Jaewon

			//@{ Jaewon 20041006
			d3dxEffectLast = d3dxEffect;
			//@} Jaewon
        }

		//@{ Jaewon 20050203
		// Restore fog color - quick, but ugly hack--;.
		RwD3D9SetRenderState(D3DRS_FOGCOLOR, fogColor);
		//@{ Jaewon 20050204
		// Restore blend factors.
		RwD3D9SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		RwD3D9SetRenderState(D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA);
		//@} Jaewon
		//@} Jaewon

        ++instancedData;
    }

	//@{ Jaewon 20041006
	// d3d effect caching for fast rendering
	if(d3dxEffectLast)
		d3dxEffectLast->lpVtbl->End(d3dxEffectLast);
	//@} Jaewon

    return;
}


/*
 *************************************************************************************************************
 */
static RwBool
AtomicFxInstanceCallBack(void *object, RxD3D9ResEntryHeader *resEntryHeader, RwBool reinstance)
{
    D3DVERTEXELEMENT9 declaration[NUM_DECLARATIONS];
    RwUInt32 declarationIndex, offset;
    RwUInt8 *lockedVertexBuffer;
    RxD3D9VertexStream *vertexStream;
    RxD3D9InstanceData *instancedData;
    RwUInt32 vbSize;
    RwUInt32 numMeshes;
    const RwV3d *pos;
    const RwV3d *norm;
    RpGeometry *geometry;
    RpAtomic *atomic;
    RpMorphTarget *mtarget;
    RwBool success;
    RwInt32 geomFlags;
    RwMatrixWeights *weights;
    RpSkin *skin;
    RpHAnimHierarchy *hierarchy;
    RwUInt32 i;
    RpD3D9GeometryUsageFlag usageFlags;
    RwInt32 numTextureCoords;
    D3DCAPS9 *deviceCaps;

    my_assert(object);
    my_assert(resEntryHeader);
    
    atomic = (RpAtomic *)object;
    geometry = RpAtomicGetGeometry(atomic);
    my_assert(geometry);
    geomFlags = RpGeometryGetFlags(geometry);

    if (reinstance)
    {
		/*
		 * 2006. 7. 19. nonstopdj.
		 * debug message block
		 *
        RwChar msg[] = "Reinstance support not implemented. After edits call \
RwResourcesFreeResEntry(geometry->repEntry);geometry->repEntry = NULL; If morph animation use atomic instead \
of geometry\n";
        _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "AtomicFxInstanceCallBack", msg);
		*/
        return FALSE;      
    }

	//@{ Jaewon 20041026
	//EnterCriticalSection(&_criticalSection);
	//@} Jaewon

    deviceCaps = (D3DCAPS9 *)RwD3D9GetCaps();

    resEntryHeader->totalNumVertex = geometry->numVertices;
    vertexStream = &(resEntryHeader->vertexStream[0]);

    declarationIndex = 0;
    offset = 0;

    /* I assume there is always positions */
    declaration[declarationIndex].Stream = 0;
    declaration[declarationIndex].Offset = offset;
    declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
    declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
    declaration[declarationIndex].Usage = D3DDECLUSAGE_POSITION;
    declaration[declarationIndex].UsageIndex = 0;
    ++declarationIndex;
    offset += sizeof(RwV3d);
    vertexStream->stride = sizeof(RwV3d);
    vertexStream->geometryFlags = rpGEOMETRYLOCKVERTICES;

    if (geomFlags & rpGEOMETRYNORMALS)
    {
	    declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_NORMAL;
        declaration[declarationIndex].UsageIndex = 0;

        if (deviceCaps->DeclTypes & D3DDTCAPS_DEC3N)
        {
            declaration[declarationIndex].Type = D3DDECLTYPE_DEC3N;
            vertexStream->stride += sizeof(RwUInt32);
            offset += sizeof(RwUInt32);
        }
        else
        {
            declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
            vertexStream->stride += sizeof(RwV3d);
            offset += sizeof(RwV3d);
        }

        vertexStream->geometryFlags |= rpGEOMETRYLOCKNORMALS;
        ++declarationIndex;
    }

    if (geomFlags & rpGEOMETRYPRELIT)
    {
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_D3DCOLOR;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_COLOR;
        declaration[declarationIndex].UsageIndex = 0;
        ++declarationIndex;
        offset += sizeof(RwRGBA);
        vertexStream->stride += sizeof(RwRGBA);
        vertexStream->geometryFlags |= rpGEOMETRYLOCKPRELIGHT;
    }

    numTextureCoords = RpGeometryGetNumTexCoordSets(geometry);
    if (numTextureCoords)
    {
        RwInt32 n;
        for (n=0; n<numTextureCoords; ++n)
        {
            declaration[declarationIndex].Stream = 0;
            declaration[declarationIndex].Offset = offset;
            declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT2;
            declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
            declaration[declarationIndex].Usage = D3DDECLUSAGE_TEXCOORD;
            declaration[declarationIndex].UsageIndex = n;
            ++declarationIndex;
            offset += sizeof(RwTexCoords);
            vertexStream->stride += sizeof(RwTexCoords);
            vertexStream->geometryFlags |= (rpGEOMETRYLOCKTEXCOORDS1 << n);
        }
    }

    usageFlags = RpD3D9GeometryGetUsageFlags(geometry);
    if ((usageFlags & rpD3D9GEOMETRYUSAGE_CREATETANGENTS) != 0 && numTextureCoords > 0)
    {
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_TANGENT;
        declaration[declarationIndex].UsageIndex = 0;

        if (deviceCaps->DeclTypes & D3DDTCAPS_DEC3N)
        {
            declaration[declarationIndex].Type = D3DDECLTYPE_DEC3N;
            vertexStream->stride += sizeof(RwUInt32);
            offset += sizeof(RwUInt32);
        }
        else
        {
            declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
            vertexStream->stride += sizeof(RwV3d);
            offset += sizeof(RwV3d);
        }

        ++declarationIndex;
    }

    skin = RpSkinGeometryGetSkin(geometry);
    if (skin)
    {
        /* Weights */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT4;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_BLENDWEIGHT;
        declaration[declarationIndex].UsageIndex = 0;
        ++declarationIndex;
        offset += sizeof(RwV4d);
        vertexStream->stride += sizeof(RwV4d);

        /* Indices */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
		//@{ Jaewon 20040916
		// D3DCOLOR -> UBYTE4 or SHORT4
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_BLENDINDICES;
        declaration[declarationIndex].UsageIndex = 0;
		if (deviceCaps->DeclTypes & D3DDTCAPS_UBYTE4)
		{
			declaration[declarationIndex].Type = D3DDECLTYPE_UBYTE4;
			offset += sizeof(RwRGBA);
			vertexStream->stride += sizeof(RwRGBA);
		}
		else
		{
			declaration[declarationIndex].Type = D3DDECLTYPE_SHORT4;
			offset += 2*sizeof(RwRGBA);
			vertexStream->stride += 2*sizeof(RwRGBA);
		}
		//@} Jaewon
		++declarationIndex;

		//@{ Jaewon 20040916
		// to be compatible to the shadow map.
		if(skin->skinSplitData.meshRLECount != NULL)
		{
			skin->platformData.maxNumBones = skin->skinSplitData.boneLimit;
		}
		else
		{
			skin->platformData.maxNumBones = skin->boneData.numUsedBones;
		}

		skin->platformData.useVertexShader = TRUE;
		//@} Jaewon
    }

    /* no more vertex data */
	declaration[declarationIndex].Stream = 0xFF;
    declaration[declarationIndex].Offset = 0;
    declaration[declarationIndex].Type = D3DDECLTYPE_UNUSED;
    declaration[declarationIndex].Method = 0;
    declaration[declarationIndex].Usage = 0;
    declaration[declarationIndex].UsageIndex = 0;

    RwD3D9CreateVertexDeclaration(declaration, &(resEntryHeader->vertexDeclaration));

    vbSize = (vertexStream->stride) * (resEntryHeader->totalNumVertex);
    vertexStream->managed = TRUE;

    success = RwD3D9CreateVertexBuffer(vertexStream->stride, vbSize, &vertexStream->vertexBuffer,
        &vertexStream->offset);
    my_assert(success);

    /* Fix base index */
    resEntryHeader->useOffsets = FALSE;

    numMeshes = resEntryHeader->numMeshes;
    instancedData = (RxD3D9InstanceData *)(resEntryHeader + 1);

    do
    {
        instancedData->baseIndex = instancedData->minVert + (vertexStream->offset / vertexStream->stride);
        ++instancedData;
    }
    while( --numMeshes );

    IDirect3DVertexBuffer9_Lock((LPDIRECT3DVERTEXBUFFER9)vertexStream->vertexBuffer,
        vertexStream->offset, vbSize, &lockedVertexBuffer, 0);

    declarationIndex = 0;
    offset = 0;

    mtarget = RpGeometryGetMorphTarget(geometry, 0);
    my_assert(mtarget);

    geomFlags = RpGeometryGetFlags(geometry);

    pos = (const RwV3d *)RpMorphTargetGetVertices(mtarget);
    my_assert(pos);

    offset += _rpD3D9VertexDeclarationInstV3d(declaration[declarationIndex].Type,
        lockedVertexBuffer + offset, pos, resEntryHeader->totalNumVertex, vertexStream->stride);

    if (geomFlags & rpGEOMETRYNORMALS)
    {
        norm = (const RwV3d *)RpMorphTargetGetVertexNormals(mtarget);
        my_assert(norm);

        declarationIndex = 0;
        while (declaration[declarationIndex].Usage != D3DDECLUSAGE_NORMAL ||
            declaration[declarationIndex].UsageIndex != 0)
        {
            ++declarationIndex;
        }
        my_assert(declarationIndex < NUM_DECLARATIONS);

        offset += _rpD3D9VertexDeclarationInstV3d(declaration[declarationIndex].Type,
            lockedVertexBuffer + declaration[declarationIndex].Offset, norm, resEntryHeader->totalNumVertex,
            vertexStream->stride);
    }

    if (geomFlags & rpGEOMETRYPRELIT)
    {
        const RwRGBA *colour = (const RwRGBA *)RpGeometryGetPreLightColors(geometry);
        my_assert(colour);

        declarationIndex = 0;
        while (declaration[declarationIndex].Usage != D3DDECLUSAGE_COLOR ||
            declaration[declarationIndex].UsageIndex != 0)
        {
            ++declarationIndex;
        }
        my_assert(declarationIndex < NUM_DECLARATIONS);

        numMeshes = resEntryHeader->numMeshes;
        instancedData = (RxD3D9InstanceData *)(resEntryHeader + 1);

        do
        {
            instancedData->vertexAlpha = _rpD3D9VertexDeclarationInstColor(lockedVertexBuffer + offset +
                ((instancedData->minVert) * vertexStream->stride), colour + instancedData->minVert,
                instancedData->numVertices, vertexStream->stride);

            ++instancedData;
        }
        while (--numMeshes);

        declarationIndex++;
        offset += sizeof(RwRGBA);
    }

    if (numTextureCoords)
    {
        RwInt32 n;

        for (n=0; n<numTextureCoords; ++n)
        {
            const RwTexCoords *texCoord = (const RwTexCoords *)(geometry->texCoords[n]);
            my_assert(texCoord);

            declarationIndex = 0;
            while (declaration[declarationIndex].Usage != D3DDECLUSAGE_TEXCOORD ||
                    declaration[declarationIndex].UsageIndex != n)
            {
                ++declarationIndex;
            }
            my_assert(declarationIndex < NUM_DECLARATIONS);

            offset += _rpD3D9VertexDeclarationInstV2d(declaration[declarationIndex].Type,
                lockedVertexBuffer + declaration[declarationIndex].Offset, (const RwV2d *)texCoord,
                resEntryHeader->totalNumVertex, vertexStream->stride);
        }
    }

    if (usageFlags & rpD3D9GEOMETRYUSAGE_CREATETANGENTS && numTextureCoords > 0)
    {
        const RwV3d *pos;
        const RwTexCoords *texCoord;

        declarationIndex = 0;
        while (declaration[declarationIndex].Usage != D3DDECLUSAGE_TANGENT || 
            declaration[declarationIndex].UsageIndex != 0)
        {
            ++declarationIndex;
        }
        my_assert(declarationIndex < NUM_DECLARATIONS);

        pos = (const RwV3d *)RpMorphTargetGetVertices(mtarget);
        my_assert(pos);
        texCoord = (const RwTexCoords *)(geometry->texCoords[numTextureCoords - 1]);
        my_assert(texCoord);

        norm = (const RwV3d *)RpMorphTargetGetVertexNormals(mtarget);
        my_assert(norm);

        _rpD3D9VertexDeclarationInstTangent(declaration[declarationIndex].Type,
            lockedVertexBuffer + declaration[declarationIndex].Offset, pos, norm, NULL, texCoord, resEntryHeader,
            vertexStream->stride
			//@{ Jaewon 20050330
			,
			geometry->mesh
			//@} Jaewon
			);
    }

    if (skin)
    {
        hierarchy = RpSkinAtomicGetHAnimHierarchy(atomic);
		//@{ Jaewon 20041008
		// no crash, just guess...
        //my_assert(hierarchy);
		//@{ Jaewon 20041022
		if(!hierarchy)
		{
			// it should not be here...
			RwBool Oops = TRUE;
		}
		//@} Jaewon
		//@} Jaewon

        /* Matrix weights */
        weights = (RwMatrixWeights *)RpSkinGetVertexBoneWeights(skin);
        my_assert(weights);

        for(i=0; i<resEntryHeader->totalNumVertex; ++i)
        {
            if( weights[i].w0 > 1.0f )
            {
                weights[i].w0 = 1.0f;
                weights[i].w1 = 0.0f;
                weights[i].w2 = 0.0f;
                weights[i].w3 = 0.0f;
            }
        }

        declarationIndex = 0;
        while (declaration[declarationIndex].Usage != D3DDECLUSAGE_BLENDWEIGHT)
        {
            ++declarationIndex;
        }
        my_assert(declarationIndex < NUM_DECLARATIONS);

        offset += _rpD3D9VertexDeclarationInstWeights(declaration[declarationIndex].Type,
            lockedVertexBuffer + declaration[declarationIndex].Offset, (const RwV4d *)weights,
            resEntryHeader->totalNumVertex, vertexStream->stride);

        /* Indices */
        my_assert(RpSkinGetVertexBoneIndices(skin));
     
        declarationIndex = 0;
        while (declaration[declarationIndex].Usage != D3DDECLUSAGE_BLENDINDICES)
        {
            ++declarationIndex;
        }
        my_assert(declarationIndex < NUM_DECLARATIONS);

        /* I do not use _rpD3D9VertexDeclarationInstIndicesRemap as this swizzles the indices for D3DCOLOR, 
           which will break D3DCOLORtoUBYTE4 used in the vertex shader */
        //@{ Jaewon 20040916
		//my_assert(D3DDECLTYPE_D3DCOLOR == declaration[declarationIndex].Type);
		//@} Jaewon
        {
            const RwUInt8 *src = (const RwUInt8 *)RpSkinGetVertexBoneIndices(skin);
            RwUInt8 *dstPosition;
			const RwUInt8  *usedBones;

            dstPosition = (RwUInt8 *)lockedVertexBuffer + declaration[declarationIndex].Offset;

			if (skin->skinSplitData.meshRLECount != NULL)
			{
				//@{ Jaewon 20041008
				// no crash, just guess...
				if (hierarchy && 
					skin->skinSplitData.matrixRemapIndices[0] >= hierarchy->numNodes)
				//@} Jaewon
				{
					skin->skinSplitData.matrixRemapIndices[0] = 0;
				}

				usedBones = skin->skinSplitData.matrixRemapIndices;

				//@{ Jaewon 20040916
				if(declaration[declarationIndex].Type == D3DDECLTYPE_SHORT4)
				// RwUInt16
				{
				for(i=0; i!=resEntryHeader->totalNumVertex; ++i)
				{
					/* bone matrices is an array of float4 vectors rather than float3x4 matrices in the vertex
					   shader constant store, so adjust each index */
					*(((RwUInt16*)dstPosition) + 0) = usedBones[src[0]] * 3;
					*(((RwUInt16*)dstPosition) + 1) = usedBones[src[1]] * 3;
					*(((RwUInt16*)dstPosition) + 2) = usedBones[src[2]] * 3;
					*(((RwUInt16*)dstPosition) + 3) = usedBones[src[3]] * 3;

					src += 4;
					dstPosition += vertexStream->stride;
				}
				}
				else
				// UBYTE4
				{
				for(i=0; i!=resEntryHeader->totalNumVertex; ++i)
				{
					dstPosition[0] = usedBones[src[0]] * 3;
					dstPosition[1] = usedBones[src[1]] * 3;
					dstPosition[2] = usedBones[src[2]] * 3;
					dstPosition[3] = usedBones[src[3]] * 3;

					src += 4;
					dstPosition += vertexStream->stride;
				}
				}
			}
			else
			{
				if(declaration[declarationIndex].Type == D3DDECLTYPE_SHORT4)
				// RwUInt16
				{
				for(i=0; i!=resEntryHeader->totalNumVertex; ++i)
				{
					/* bone matrices is an array of float4 vectors rather than float3x4 matrices in the vertex
					   shader constant store, so adjust each index */
					*(((RwUInt16*)dstPosition) + 0) = src[0] * 3;
					*(((RwUInt16*)dstPosition) + 1) = src[1] * 3;
					*(((RwUInt16*)dstPosition) + 2) = src[2] * 3;
					*(((RwUInt16*)dstPosition) + 3) = src[3] * 3;
					
					src += 4;
					dstPosition += vertexStream->stride;
				}
				}
				else
				// UBYTE4
				{
				for(i=0; i!=resEntryHeader->totalNumVertex; ++i)
				{
					dstPosition[0] = src[0] * 3;
					dstPosition[1] = src[1] * 3;
					dstPosition[2] = src[2] * 3;
					dstPosition[3] = src[3] * 3;
					
					src += 4;
					dstPosition += vertexStream->stride;
				}
				}
			}
        }
        offset += sizeof(RwRGBA);
        ++declarationIndex;
    }

    IDirect3DVertexBuffer9_Unlock((LPDIRECT3DVERTEXBUFFER9)vertexStream->vertexBuffer);

	//@{ Jaewon 20041026
	//LeaveCriticalSection(&_criticalSection);
	//@} Jaewon
    return TRUE;
}


/*
 *****************************************************************************
 */
static RwBool
AtomicFxReinstanceCallBack(void *object, RwResEntry *resEntry,
    RxD3D9AllInOneInstanceCallBack instanceCallback)
{
    const RpAtomic *atomic;
    RpGeometry *geometry;

    assert(object);
    assert(resEntry);

    atomic = (const RpAtomic *)object;

    geometry = RpAtomicGetGeometry(atomic);
    assert(geometry);
    assert(1 == RpGeometryGetNumMorphTargets(geometry));

    if (instanceCallback)
    {
        RxD3D9ResEntryHeader *resEntryHeader;

        resEntryHeader = (RxD3D9ResEntryHeader *)(resEntry + 1);
        if (!instanceCallback(object, resEntryHeader, TRUE))
        {
            return FALSE;
        }
    }

    return TRUE;
}

static RwBool
myD3D9AtomicAllInOnePipelineInit(RxPipelineNode *node)
{
    myD3D9InstanceNodeData *instanceData;

    assert(node);
    instanceData = (myD3D9InstanceNodeData *)node->privateData;
    assert(instanceData);

    /* Default callbacks */
    instanceData->instanceCallback = AtomicFxInstanceCallBack;
    instanceData->reinstanceCallback = AtomicFxReinstanceCallBack;
    instanceData->renderCallback = AtomicFxRenderCallBack;

    return TRUE;
}

static RwBool
myD3D9AtomicAllInOneNode(RxPipelineNodeInstance *self, const RxPipelineNodeParam *params)
{
    RpAtomic *atomic;
    RpGeometry *geometry;
    RwResEntry *resEntry;
    RpMeshHeader *meshHeader;
    RwUInt32 geomFlags;
    myD3D9InstanceNodeData *privateData;

    assert(self);
    assert(params);

    atomic = (RpAtomic *)RxPipelineNodeParamGetData(params);
    assert(atomic);

    geometry = RpAtomicGetGeometry(atomic);
    assert(geometry);

    privateData = (myD3D9InstanceNodeData *)self->privateData;

    /* the default pipelines would simple return true here, but I consider
       it an application error to try to render an empty atomic... */
    assert(geometry->numVertices > 0);

    /* ...the same goes for the number of meshes */
    meshHeader = geometry->mesh;
    assert(meshHeader->numMeshes > 0);

    geomFlags = RpGeometryGetFlags(geometry);

    /* if we have native data, don't even look at instancing */
    if (!(rpGEOMETRYNATIVE & geomFlags))
    {    
        /* I am not supporting morph animation */
        assert(1 == RpGeometryGetNumMorphTargets(geometry));
        resEntry = geometry->repEntry;

        /* If the meshes have changed we should re-instance */
        if (resEntry)
        {
            RxD3D9ResEntryHeader *resEntryHeader;
			RpSkin *pSkin = NULL;

            resEntryHeader = (RxD3D9ResEntryHeader *)(resEntry + 1);
			//@{ Jaewon 20041029
			// the current instancing can be invalid.
			pSkin = geometry?RpSkinGeometryGetSkin(geometry):NULL;
            if ((resEntryHeader->serialNumber != meshHeader->serialNum) ||
				(pSkin && (pSkin->platformData.useVertexShader == 0)))
			//@} Jaewon
            {
                /* Destroy resources to force reinstance */
                RwResourcesFreeResEntry(resEntry);
                resEntry = NULL;
            }
        }

        /* Check to see if a resource entry already exists */
        if (resEntry)
        {
            /*if (geometry->lockedSinceLastInst ||
                (RpGeometryGetNumMorphTargets(geometry) != 1))*/
            if (geometry->lockedSinceLastInst)
            {
                if (privateData->reinstanceCallback)
                {
                    if (!privateData->reinstanceCallback((void *)atomic,resEntry,
                        privateData->instanceCallback))
                    {
                        RwResourcesFreeResEntry(resEntry);
                        return FALSE;
                    }
                }

                /* Just make sure the interpolator is no longer dirty */
                atomic->interpolator.flags &= ~rpINTERPOLATORDIRTYINSTANCE;

                /* The geometry is up to date */
                geometry->lockedSinceLastInst = 0;
            }

            RwResourcesUseResEntry(resEntry);
        }
        else
        {
            RwResEntry **resEntryPointer;
            void *owner;

            meshHeader = geometry->mesh;

            assert(1 == RpGeometryGetNumMorphTargets(geometry));
            owner = (void *)geometry;
            resEntryPointer = &geometry->repEntry;

            /* Create vertex buffers and instance */
            resEntry = _rxD3D9Instance((void *)atomic, owner, rpATOMIC,
                resEntryPointer, meshHeader, privateData->instanceCallback,
                (rpGEOMETRYNATIVEINSTANCE & geomFlags) != 0);

            if (!resEntry)
            {
                return FALSE;
            }

            /* The geometry is up to date */
            geometry->lockedSinceLastInst = 0;
        }
    }
    else
    {
        resEntry = geometry->repEntry;
    }

    /* Early out of rendering if we're really preinstancing */
    if (rpGEOMETRYNATIVEINSTANCE & geomFlags)
    {
        return TRUE;
    }

    if (privateData->renderCallback)
    {
        privateData->renderCallback(resEntry, (void *)atomic, rpATOMIC, geomFlags);
    }

#ifdef RWMETRICS
    RWSRCGLOBAL(metrics)->numVertices  += RpGeometryGetNumVertices(geometry);
    RWSRCGLOBAL(metrics)->numTriangles += RpGeometryGetNumTriangles(geometry);
#endif

    return TRUE;
}

static RxNodeDefinition *
myNodeDefinitionGetD3D9AtomicAllInOne(void)
{

    /* Create my one node definition, this saves the node body perform FFP
       setup, which will be done when using RxNodeDefinitionGetD3D9AtomicAllInOne */

    static RwChar atomicInstance[] = "myNodeD3D9AtomicAllInOne";

    static RxNodeDefinition myNodeD3D9AtomicAllInOne = { /* */
        atomicInstance,                        /* Name */
        {                                           /* Nodemethods */
            myD3D9AtomicAllInOneNode,              /* +-- nodebody */
            NULL,                                   /* +-- nodeinit */
            NULL,                                   /* +-- nodeterm */
            myD3D9AtomicAllInOnePipelineInit,         /* +-- pipelinenodeinit */
            NULL,                                   /* +-- pipelineNodeTerm */
            NULL,                                   /* +-- pipelineNodeConfig */
            NULL,                                   /* +-- configMsgHandler */
        },
        {                                           /* Io */
            NUMCLUSTERSOFINTEREST,                  /* +-- NumClustersOfInterest */
            NULL,                                   /* +-- ClustersOfInterest */
            NULL,                                   /* +-- InputRequirements */
            NUMOUTPUTS,                             /* +-- NumOutputs */
            NULL                                    /* +-- Outputs */
        },
        (RwUInt32)sizeof(myD3D9InstanceNodeData),  /* PipelineNodePrivateDataSize */
        (RxNodeDefEditable)FALSE,                   /* editable */
        0                                           /* inPipes */
    };

    return &myNodeD3D9AtomicAllInOne;
}


/*
 *************************************************************************************************************
 */
RwBool
EffectPipelineOpen(void)
{
    RxPipeline *pipe = NULL;
	RxPipelineNode *allinone = NULL;
	RxLockedPipe *lpipe;
	RxNodeDefinition *nodedef = NULL;
    void *success;

    /* Create the atomic pipeline that should be used for all atomics whose materials have effects applied */

    my_assert(0 == RpMaterialD3DFxModule.numInstances);

	pipe = RxPipelineCreate();
    my_assert(pipe);

	//@{ Jaewon 20040917
	// for rights callback
    pipe->pluginId = MATERIAL_FX_PLUGIN;
	//@} Jaewon

    lpipe = RxPipelineLock(pipe);
    my_assert(lpipe);

    nodedef = myNodeDefinitionGetD3D9AtomicAllInOne();
    my_assert(nodedef);
    success = RxLockedPipeAddFragment(lpipe, NULL, nodedef, NULL);
    my_assert(success);
    success = RxLockedPipeUnlock(lpipe);
    my_assert(success);
	allinone = RxPipelineFindNodeByName(pipe, nodedef->name, NULL, 0);
    my_assert(allinone);
    
    /* pipeline callbacks are set in myD3D9AtomicAllInOnePipelineInit */

    Pipeline = pipe;
	return TRUE;
}

void
EffectPipelineClose(void)
{
    /* Destroy the atomic pipeline used for effect rendering. Should only be called when shutting down the
       matd3dfx plugin*/

    my_assert(0 == RpMaterialD3DFxModule.numInstances);

    if (Pipeline)
    {
        RxPipelineDestroy(Pipeline);
        Pipeline = NULL;
    }
    return;
}


/*
 *************************************************************************************************************
 */
RxPipeline *
EffectGetPipeline(void)
{
    /* Returns the atomic pipeline that should be used for all atomics whose materials have effects applied */
    return Pipeline;
}

/*
 *************************************************************************************************************
 */
