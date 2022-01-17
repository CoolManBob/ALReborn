#ifndef _CRT_SECURE_NO_DEPRECATE
	#define _CRT_SECURE_NO_DEPRECATE
#endif

#pragma warning ( disable : 4047)

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
 * effect.c
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
#include "rtfsyst.h"
#include "myassert.h"
#include "AcuRpMatD3DFx.h"
#include "rwd3d9.h"
#include "effect.h"
//@{ Jaewon 20041007
#include "effectStateManager.h"
//@} Jaewon


/* define EFFECT_USE_IMAGE_PATH_IN_FX to use paths in fx file for loading textures instead of current image
   path */
#define EFFECT_USE_IMAGE_PATH_IN_FXx
/* define EFFECT_RESTORE_IMAGE_PATH to restore the image path after loading a texture */
#define EFFECT_RESTORE_IMAGE_PATHx

#if (defined(EFFECT_RESTORE_IMAGE_PATH) && !defined(EFFECT_USE_IMAGE_PATH_IN_FX))
#undef EFFECT_RESTORE_IMAGE_PATH
#endif /* (defined(EFFECT_RESTORE_IMAGE_PATH) && !defined(EFFECT_USE_IMAGE_PATH_IN_FX)) */
/*
typedef struct rpDxEffect rpDxEffect;
struct rpDxEffect
{
    DxEffect *effect;
    rpDxEffect *next;
};

typedef void* (*EffectCallBack)(DxEffect *, void *data);

static rpDxEffect *DxEffects = NULL;
static RwFreeList *DxEffectFreelist = NULL;*/
static rwD3D9DeviceRestoreCallBack OldDeviceRestoreCallBack = NULL;
static rwD3D9DeviceReleaseCallBack OldDeviceReleaseCallBack = NULL;

//static DxEffect* ForAllEffectsInFreelist(EffectCallBack callback, void *data);
//static void EffectListRemove(DxEffect *effect);
//static void EffectListAdd(DxEffect *effect);

typedef struct rpD3dXEffect rpD3dXEffect;
struct rpD3dXEffect
{
	SharedD3dXEffectPtr *pD3dXEffect;
	//@{ Jaewon 20041022
	// one instance of wrapping DxEffects.
	DxEffect* pEffect;
	//@} Jaewon
	rpD3dXEffect *next;
};

static rpD3dXEffect *D3dXEffects = NULL;
static RwFreeList *D3dXEffectFreelist = NULL;
static void D3dXEffectListRemove(SharedD3dXEffectPtr *);
static void D3dXEffectListAdd(SharedD3dXEffectPtr *, DxEffect * instance);

//@{ Jaewon 20050722
// Memory pooling for performance
static RwFreeList *DxEffectFreeList;
static RwFreeList *HandleFloatFreeList;
static RwFreeList *HandleVectorFreeList;
static RwFreeList *HandleMatrix4x4FreeList;
static RwFreeList *HandleTextureFreeList;
//@} Jaewon

static RwReal GUIDefaultMinRange = -50.0f;
static RwReal GUIDefaultMaxRange = 50.0f;
static RwReal GUIDefaultStepSize = 0.5f;

static RwBool StreamWriteReadError = FALSE;

LPDIRECT3DDEVICE9 EffectD3DDevice = NULL;
//@{ Jaewon 20041022
// TRUE -> FALSE
RwBool EffectInitMatricesToIdentity = FALSE;
//@} Jaewon

//@{ Jaewon 20040922
RwUInt32 _nWeights = 0;
//@} Jaewon

//@{ Jaewon 20041021
/*
 *************************************************************************************************************
 */
RwBool
EffectGetFloat(DxEffect *effect, RwChar *name, RwReal *data)
{
    RwUInt32 i;
	HandleFloat *handleFloat;

    /* Assign the floating point number to the parameter in the effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(name);
	my_assert(data);

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL || data == NULL || name == NULL || name[0] == NULL )
		return FALSE;
	//@}

    TEST_HANDLE_NAME_LENGTH(name, "EffectSetFloat");

    //@{ Jaewon 20050722
	// For memory pooling
	handleFloat = effect->real;
	for (i=0; i!=effect->realCount; ++i)
	{
		if (0 == strcmp(handleFloat->name, name))
		{
			*data = handleFloat->data;
			return TRUE;
		}

		handleFloat = handleFloat->next;
	}
	//@} Jaewon

    return FALSE;
}


/*
 *************************************************************************************************************
 */
RwBool
EffectGetVector(DxEffect *effect, RwChar *name, D3DXVECTOR4 *data)
{
    RwUInt32 i;
	HandleVector *handleVector;

    /* Assign the vector to the parameter in the effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(name);
    my_assert(data);

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL || data == NULL || name == NULL || name[0] == NULL )
		return FALSE;
	//@}

    TEST_HANDLE_NAME_LENGTH(name, "EffectSetVector");

    //@{ Jaewon 20050722
	// For memory pooling
	handleVector = effect->vector;
	for (i=0; i!=effect->vectorCount; ++i)
	{
		if (0 == strcmp(handleVector->name, name))
		{
			*data = handleVector->data;
			return TRUE;
		}

		handleVector = handleVector->next;
	}
	//@} Jaewon

    return FALSE;
}

/*
 *************************************************************************************************************
 */
RwBool
EffectGetMatrix4x4(DxEffect *effect, RwChar *name, D3DXMATRIX *data)
{
    RwUInt32 i;
	HandleMatrix4x4 *handleMatrix;

    /* Assign the matrix to the parameter in the effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(name);
    my_assert(data);

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL || data == NULL || name == NULL || name[0] == NULL )
		return FALSE;
	//@}

    TEST_HANDLE_NAME_LENGTH(name, "EffectSetMatrix4x4");

    //@{ Jaewon 20050722
	// For memory pooling
	handleMatrix = effect->matrix4x4;
	for (i=0; i!=effect->matrix4x4Count; ++i)
	{
		if (0 == strcmp(handleMatrix->name, name))
		{
			*data = handleMatrix->data;
			return TRUE;
		}

		handleMatrix = handleMatrix->next;
	}
	//@} Jaewon

    return FALSE;
}

/*
 *************************************************************************************************************
 */
RwBool
EffectGetTexture(DxEffect *effect, RwChar *name, RwTexture **data)
{
    RwUInt32 i;
	HandleTexture *handleTexture;

    /* Assign RwTexture to the parameter in the effect. If the parameter already has a texture assigned this
       texture will be released with RwTextureDestroy. This call will increment the reference count of the
       texture */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(name);
    my_assert(data);

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL || data == NULL || name == NULL || name[0] == NULL )
		return FALSE;
	//@}

    TEST_HANDLE_NAME_LENGTH(name, "EffectSetTexture");

    //@{ Jaewon 20050722
	// For memory pooling
	handleTexture = effect->texture;
	for (i=0; i!=effect->textureCount; ++i)
	{
		if (0 == strcmp(handleTexture->name, name))
		{
			*data = handleTexture->data;

			return TRUE;
		}

		handleTexture = handleTexture->next;
	}
	//@} Jaewon

    return FALSE;
}
//@} Jaewon

/*
 *************************************************************************************************************
 */
RwBool
EffectSetFloat(DxEffect *effect, RwChar *name, RwReal data)
{
    RwUInt32 i;
	HandleFloat *handleFloat;

    /* Assign the floating point number to the parameter in the effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(name);

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL || name == NULL || name[0] == NULL )
		return FALSE;
	//@}

    TEST_HANDLE_NAME_LENGTH(name, "EffectSetFloat");

    //@{ Jaewon 20050722
	// For memory pooling
	handleFloat = effect->real;
	for (i=0; i!=effect->realCount; ++i)
	{
		if (0 == strcmp(handleFloat->name, name))
		{
			handleFloat->data = data;
			return TRUE;
		}

		handleFloat= handleFloat->next;
	}
	//@} Jaewon

    return FALSE;
}


/*
 *************************************************************************************************************
 */
RwBool
EffectSetVector(DxEffect *effect, RwChar *name, D3DXVECTOR4 *data)
{
    RwUInt32 i;
	HandleVector *handleVector;

    /* Assign the vector to the parameter in the effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(name);
    my_assert(data);

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL || data == NULL || name == NULL || name[0] == NULL )
		return FALSE;
	//@}

    TEST_HANDLE_NAME_LENGTH(name, "EffectSetVector");

    //@{ Jaewon 20050722
	// for memory pooling
	handleVector = effect->vector;
	for (i=0; i!=effect->vectorCount; ++i)
	{
		if (0 == strcmp(handleVector->name, name))
		{
			handleVector->data = *data;
			return TRUE;
		}

		handleVector = handleVector->next;
	}
	//@} Jaewon

    return FALSE;
}


/*
 *************************************************************************************************************
 */
RwBool
EffectSetMatrix4x4(DxEffect *effect, RwChar *name, D3DXMATRIX *data)
{
    RwUInt32 i;
	HandleMatrix4x4 *handleMatrix;

    /* Assign the matrix to the parameter in the effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(name);
    my_assert(data);

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL || data == NULL || name == NULL || name[0] == NULL )
		return FALSE;
	//@}

    TEST_HANDLE_NAME_LENGTH(name, "EffectSetMatrix4x4");

    //@{ Jaewon 20050722
	// For memory pooling
	handleMatrix = effect->matrix4x4;
	for (i=0; i!=effect->matrix4x4Count; ++i)
	{
		if (0 == strcmp(handleMatrix->name, name))
		{
			handleMatrix->data = *data;
			return TRUE;
		}

		handleMatrix = handleMatrix->next;
	}
	//@} Jaewon

    return FALSE;
}

/*
 *************************************************************************************************************
 */
RwBool
EffectSetTexture(DxEffect *effect, RwChar *name, RwTexture *data)
{
    RwUInt32 i;
	HandleTexture *handleTexture;

    /* Assign RwTexture to the parameter in the effect. If the parameter already has a texture assigned this
       texture will be released with RwTextureDestroy. This call will increment the reference count of the
       texture */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(name);

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL || name == NULL || name[0] == NULL )
		return FALSE;
	//@}

	//@{ Jaewon 20040913
    //my_assert(data);
	//@} Jaewon

    TEST_HANDLE_NAME_LENGTH(name, "EffectSetTexture");

    //@{ Jaewon 20050722
	// for memory pooling
	handleTexture = effect->texture;
	for (i=0; i!=effect->textureCount; ++i)
	{
		if (0 == strcmp(handleTexture->name, name))
		{
			if (data)
			{
				RwTextureAddRef(data);
			}

			if (handleTexture->data)
			{
				RwTextureDestroy(handleTexture->data);
				handleTexture->data = NULL;
			}

			handleTexture->data = data;

			return TRUE;
		}

		handleTexture = handleTexture->next;
	}
	//@} Jaewon

    return FALSE;
}


/*
 *************************************************************************************************************
 */
const RwChar *
EffectGetCurrentTechniqueName(DxEffect *effect)
{
    D3DXTECHNIQUE_DESC desc;
    D3DXHANDLE tech;
    HRESULT hr;

    /* Returns the name of the effect's current technique */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL )
		return NULL;
	//@}

    //@{ Jaewon 20041102
	// lighting optimization
    tech = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetCurrentTechnique(effect->pSharedD3dXEffect->d3dxEffect[0]);
    my_assert(tech);
    hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetTechniqueDesc(effect->pSharedD3dXEffect->d3dxEffect[0], tech, &desc);
	//@} Jaewon
    my_assert(SUCCEEDED(hr));
    return desc.Name;
}


/*
 *************************************************************************************************************
 */
RwUInt32
EffectGetNumberOfTechniques(DxEffect *effect)
{
    D3DXEFFECT_DESC effectDesc;
    HRESULT hr;

    /* Returns the number of techniques in the effect */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL )
		return FALSE;
	//@}

	//@{ Jaewon 20041102
	// lighting optimization
    hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetDesc(effect->pSharedD3dXEffect->d3dxEffect[0], &effectDesc);
	//@} Jaewon
    my_assert(SUCCEEDED(hr));
    return effectDesc.Techniques;
}


/*
 *************************************************************************************************************
 */
RwBool
EffectSetFirstValidTechnique(DxEffect *effect)
{
    D3DXHANDLE current;
	//, next;
    HRESULT hr;
	int i;

    /* Set the technique for the effect.  The param index should be in the range 
       [0,EffectGetNumberOfTechniques()-1] */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    //my_assert(index < EffectGetNumberOfTechniques(effect));

	//@{ 2006/11/16 burumal
	if ( RpMaterialD3DFxModule.numInstances <= 0 || effect == NULL )
		return FALSE;
	//@}

	//@{ Jaewon 20041102
	// lighting optimization
	for(i=0; i < FX_LIGHTING_VARIATION; ++i)
	{
    hr = effect->pSharedD3dXEffect->d3dxEffect[i]->lpVtbl->FindNextValidTechnique(effect->pSharedD3dXEffect->d3dxEffect[i], NULL, &current);
    my_assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr))
		return FALSE;

	if(current == NULL)
		return FALSE;

    hr = effect->pSharedD3dXEffect->d3dxEffect[i]->lpVtbl->SetTechnique(effect->pSharedD3dXEffect->d3dxEffect[i], current);
    my_assert(SUCCEEDED(hr));
	}
	//@} Jaewon

    return TRUE;
}


/*
 *************************************************************************************************************
 */
static void *
GetGUICallBack(DxEffect *effect, RwUInt32 type __RWUNUSED__, RwChar *name, void *data __RWUNUSED__,
    void *callbackData)
{
    EffectGUIParams *params;

    my_assert(effect);
    my_assert(name);
    my_assert(callbackData);

    params = (EffectGUIParams *)callbackData;

	//@{ Jaewon 20041102
	// lighting optimization
	// d3dxEffect -> d3dxEffect[0]
    if (0 == strcmp(name, params->name))
    {
        D3DXHANDLE annot;
        D3DXHANDLE handle;
        HRESULT hr;    

        handle = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetParameterByName(effect->pSharedD3dXEffect->d3dxEffect[0], NULL, name);

        annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "UIWidget");
        if (annot)
        {   
            hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetString(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &params->widget);
            my_assert(SUCCEEDED(hr));
        }
		else
        {
            strncpy(params->widget, "None", FX_HANDLE_NAME_LENGTH);
        }

        annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "UIMin");
        if (annot)
        {
            hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetFloat(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &params->minRange);
            my_assert(SUCCEEDED(hr));
        }
        else
        {
            params->minRange = GUIDefaultMinRange;
        }

        annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "UIMax");
        if (annot)
        {
            hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetFloat(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &params->maxRange);
            my_assert(SUCCEEDED(hr));
        }
        else
        {
            params->maxRange = GUIDefaultMaxRange;
        }

        annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "UIStep");
        if (annot)
        {
            hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetFloat(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &params->step);
            my_assert(SUCCEEDED(hr));
        }
        else
        {
            params->step = GUIDefaultStepSize;
        }

        annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "UIName");
        if (annot)
        {   
            hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetString(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &params->guiName);
            my_assert(SUCCEEDED(hr));
        }
        else
        {
            strncpy(params->guiName, params->name, FX_HANDLE_NAME_LENGTH);
        }

        return NULL;
    }
	//@} Jaewon

    return effect;
}


/*
 *************************************************************************************************************
 */
void
EffectParameterGetGUI(DxEffect *effect, RwChar *parameterName, EffectGUIParams *params)
{
    /* Returns the GUI data for the parameter. Not all GUI data needs to be provided by the fx file. If any
       data is missing then defaults will be used.  The range defaults can be set with
       EffectSetDefaultGUIRanges.  For widget it is "default_widget" and for guiName it is the name of the 
       parameter.

       The widget can be anything the shader author wants e.g. "color" to indicate a colour GUI widget should
       be used. This function just returns the string for the GUI annotation, rather than the plugin defining
       a set of specific widgets */

    D3DXHANDLE annot;
    D3DXHANDLE handle;
    HRESULT hr;    

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(parameterName);
    my_assert(params);

    TEST_HANDLE_NAME_LENGTH(parameterName, "EffectParameterGetGUI");

    params->name = parameterName;
//    EffectForAllTweakableParameters(effect, GetGUICallBack, (void *)params);

	//@{ Jaewon 20041102
	// lighting optimization
	// d3dxEffect -> d3dxEffect[0]
	handle = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetParameterByName(effect->pSharedD3dXEffect->d3dxEffect[0], NULL, params->name);

	if(handle == NULL) 
		return;

    annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "UIWidget");
    if (annot)
    {   
        hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetString(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &params->widget);
        my_assert(SUCCEEDED(hr));
    }
	else
    {
        strncpy(params->widget, "None", FX_HANDLE_NAME_LENGTH);
    }

    annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "UIMin");
    if (annot)
    {
        hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetFloat(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &params->minRange);
        my_assert(SUCCEEDED(hr));
    }
    else
    {
        params->minRange = GUIDefaultMinRange;
    }

    annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "UIMax");
    if (annot)
    {
        hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetFloat(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &params->maxRange);
        my_assert(SUCCEEDED(hr));
    }
    else
    {
        params->maxRange = GUIDefaultMaxRange;
    }

    annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "UIStep");
    if (annot)
    {
        hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetFloat(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &params->step);
        my_assert(SUCCEEDED(hr));
    }
    else
    {
        params->step = GUIDefaultStepSize;
    }

    annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "UIName");
    if (annot)
    {   
        hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetString(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &params->guiName);
        my_assert(SUCCEEDED(hr));
    }
    else
    {
        strncpy(params->guiName, params->name, FX_HANDLE_NAME_LENGTH);
    }
	//@} Jaewon

    return;
}


/*
 *************************************************************************************************************
 */
void 
EffectSetDefaultGUIRanges(RwReal minRange, RwReal maxRange, RwReal step)
{
    /* Sets the default ranges that should be returned from EffectParameterGetMenuRanges if the parameter does
       not have GUI range annotations */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);

    GUIDefaultMinRange = minRange;
    GUIDefaultMaxRange = maxRange;
    GUIDefaultStepSize = step;
    return;
}

void 
EffectGetDefaultGUIRanges(RwReal *minRange, RwReal *maxRange, RwReal *step)
{
    /* Gets the default ranges that are used if a parameter does not have GUI range annotations */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);

    *minRange = GUIDefaultMinRange;
    *maxRange = GUIDefaultMaxRange;
    *step = GUIDefaultStepSize;
    return;
}


/*
 *************************************************************************************************************
 */
void
EffectForAllTweakableParameters(DxEffect *effect, EffectTweakableParameterCallBack callback,
    void *callbackData)
{
    DxEffectTweakableParams params;
    RwUInt32 realIndex, vectorIndex, matrix4x4Index, textureIndex;
    RwUInt32 i;
	HandleFloat *handleFloat;
	HandleVector *handleVector;
	HandleMatrix4x4 *handleMatrix;
	HandleTexture *handleTexture;

    /* Execute the callback to all parameters in the effect that are tweakable. If any invocation of the
       callback function returns NULL the iteration is terminated */

    my_assert(effect);
    my_assert(callback);

    realIndex = 0;
    vectorIndex = 0;
    matrix4x4Index = 0;
	textureIndex = 0;
	//@{ Jaewon 20050722
	// For memory pooling
	handleFloat = effect->real;
	handleVector = effect->vector;
	handleMatrix = effect->matrix4x4;
	handleTexture = effect->texture;
	//@} Jaewon

    params = effect->tweakableParams;
    for (i=0; i<params.count; ++i)
    {
        RwChar *name;
        void *data;
        RwBool executeCallback = FALSE;

        switch (params.type[i])
        {
            case EFFECT_TWEAKABLE_PARAM_FLOAT:
            {
                //@{ Jaewon 20050726
				// 1 -> handleFloat
				while (handleFloat)
				//@} Jaewon
                {
                    //@{ Jaewon 20050722
					// effect->real[realIndex]. => handleFloat->
					if (handleFloat->tweakable)
					{
						name = handleFloat->name;
						data = (void *)&handleFloat->data;
						++realIndex;
						//@{ Jaewon 20050810
						// --; my bad...
						handleFloat = handleFloat->next;
						//@} Jaewon
						executeCallback = TRUE;
						break;
					}
					++realIndex;
					handleFloat = handleFloat->next;
					//@} Jaewon
                }
                break;
            }

            case EFFECT_TWEAKABLE_PARAM_VECTOR:
            {
                //@{ Jaewon 20050726
				// 1 -> handleVector
				while (handleVector)
				//@} Jaewon
                {
                    //@{ Jaewon 20050722
					// effect->vector[vectorIndex]. => handleVector->
					if (handleVector->tweakable)
					{
						name = handleVector->name;
						data = (void *)&handleVector->data;
						++vectorIndex;
						//@{ Jaewon 20050810
						// --; my bad...
						handleVector = handleVector->next;
						//@} Jaewon
						executeCallback = TRUE;
						break;
					}
					++vectorIndex;
					handleVector = handleVector->next;
					//@} Jaewon
                }
                break;
            }
        
            case EFFECT_TWEAKABLE_PARAM_MATRIX4X4:
            {
                //@{ Jaewon 20050726
				// 1 -> handleMatrix
				while (handleMatrix)
				//@} Jaewon
                {
                    //@{ Jaewon 20050722
					// effect->matrix4x4[matrix4x4Index]. => handleMatrix->
					if (handleMatrix->tweakable)
					{
						name = handleMatrix->name;
						data = (void *)&handleMatrix->data;
						++matrix4x4Index;
						//@{ Jaewon 20050810
						// --; my bad...
						handleMatrix = handleMatrix->next;
						//@} Jaewon
						executeCallback = TRUE;
						break;
					}
					++matrix4x4Index;
					handleMatrix = handleMatrix->next;
					//@} Jaewon
                }
                break;
            }

            case EFFECT_TWEAKABLE_PARAM_TEXTURE:
            {
                //@{ Jaewon 20050726
				// 1 -> handleTexture
				while (handleTexture)
				//@} Jaewon
                {
                    //@{ Jaewon 20050722
					// effect->texture[textureIndex]. => handleTexture->
					if (handleTexture->tweakable)
					{
						name = handleTexture->name;
						data = (void *)&handleTexture->data;
						++textureIndex;
						//@{ Jaewon 20050810
						// --; my bad...
						handleTexture = handleTexture->next;
						//@} Jaewon
						executeCallback = TRUE;
						break;
					}
					++textureIndex;
					//@{ Jaewon 20050726
					// A bug! --; 
					// handleMatrix -> handleTexture
					handleTexture = handleTexture->next;
					//@} Jaewon
					//@} Jaewon
                }
                break;
            }

            case EFFECT_TWEAKABLE_PARAM_NATYPE:
            default:
            {
                RwChar msg[] = "Unknown type. Callback will not be called for this parameter.";
                _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectForAllTweakableParameters",
                    msg);
                break;
            }
        }

        if (executeCallback)
        {
            if (NULL == callback(effect, params.type[i], name, data, callbackData))
            {
                return;
            }
        }
    }

    return;
}


/*
 *************************************************************************************************************
 */
static void
EffectInitEffect(DxEffect *effect)
{
    my_assert(effect);

    effect->pSharedD3dXEffect = NULL;
    effect->real = NULL;
    effect->realCount = 0;
    effect->vector = NULL;
    effect->vectorCount = 0;
    effect->matrix4x4 = NULL;
    effect->matrix4x4Count = 0;
    effect->texture = NULL;
    effect->textureCount = 0;
    memset(effect->rw, 0, sizeof(effect->rw));
	effect->cameraDirectionSpace = 0;
	effect->cameraPositionSpace = 0;
	effect->lightDirectionSpace = 0;
	//@{ Jaewon 20050419
	// Up to 8 lights
	memset(effect->localLightPositionSpace, 0, sizeof(effect->localLightPositionSpace));
	memset(effect->localLightDirectionSpace, 0, sizeof(effect->localLightDirectionSpace));
	//@} Jaewon
    effect->constUploadCallBack = NULL;
    effect->tweakableParams.count = 0;
	effect->selected = FALSE;

    return;
}


/*
 *************************************************************************************************************
 */
static void *
EffectCompareName(SharedD3dXEffectPtr *effect, void *data)
{
    my_assert(effect);
    my_assert(data);

    if (0 == strncmp(effect->name, (RwChar *)data, FX_FILE_NAME_LENGTH)
		//@{ Jaewon 20040922
		// bSkin -> nWeights
		&& effect->nWeights == _nWeights)
		//@} Jaewon
    {
        return NULL;
    }

    return effect;
}

//@{ Jaewon 20041022
static DxEffect * 
//@{ Jaewon 20040922
// bSkin -> nWeights
EffectFindEffect(RwChar *name, RwUInt32 nWeights)
//@} Jaewon
{
    DxEffect *effect;

	//@{ Jaewon 20041025
	//EnterCriticalSection(&_criticalSection);

    my_assert(name);
	//@{ Jaewon 20040922
	// bSkin -> nWeights
	_nWeights = nWeights;
	//@} Jaewon
    effect = ForAllD3dXEffectsInFreelist(EffectCompareName, (void *)name);
	
	//LeaveCriticalSection(&_criticalSection);
	//@} Jaewon

    return effect;
}
//@} Jaewon


/*
 *************************************************************************************************************
 */
RwUInt32
EffectGetSize(DxEffect *effect)
{
    RwUInt32 size = 0;
    RwUInt32 i;

    /* Returns the size in bytes the effect and its tweakable parameters will take on disk */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    
    /* fx name */
    size += sizeof(RwChar) * FX_FILE_NAME_LENGTH;

	// nWeights
	size += sizeof(RwUInt32);

    /* tweakable parameter count */
    size += sizeof(RwUInt32);

    /* tweakable type list. Write out the list so I know what to do with the following data */
    size += sizeof(RwUInt32) * effect->tweakableParams.count;

    /* I also write out the parameter name to safe guard against a fx parameters being changed but not the
       plugin data on disk, see EffectRead for what I do */
    size += sizeof(RwChar) * FX_HANDLE_NAME_LENGTH * effect->tweakableParams.count;

    /* tweakable parameters */
    for (i=0; i!=effect->tweakableParams.count; ++i)
    {
        switch (effect->tweakableParams.type[i])
        {
            case EFFECT_TWEAKABLE_PARAM_FLOAT:
                size += sizeof(RwReal);
                break;
            case EFFECT_TWEAKABLE_PARAM_VECTOR:
                size += sizeof(RwReal) * 4;
                break;
            case EFFECT_TWEAKABLE_PARAM_MATRIX4X4:
                size += sizeof(RwReal) * 16;
				break;
			case EFFECT_TWEAKABLE_PARAM_TEXTURE:
				size += sizeof(RwChar) * FX_FILE_NAME_LENGTH;
                break;
        }
    }

    return size;
}


/*
 *************************************************************************************************************
 */
static void *
WriteTweakableParameterNameCallBack(DxEffect *effect __RWUNUSED__, RwUInt32 type __RWUNUSED__, RwChar *name,
    void *data __RWUNUSED__, void *callbackData)
{
    RwStream *success;

    my_assert(name);
    my_assert(callbackData);

    success = RwStreamWrite((RwStream *)callbackData, name, sizeof(RwChar) * FX_HANDLE_NAME_LENGTH);
    if (NULL == success)
    {
        RwChar msg[MAX_MSG_SIZE] = "Failed to write effect tweakable parameter name:";
        strncat(msg, name, __min(strlen(name), MAX_STRING_LEN(msg)-strlen(msg)));
        _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "WriteTweakableParameterNameCallBack", msg);
        StreamWriteReadError = TRUE;
    }

    return success;
}

static void *
WriteTweakableParameterDataCallBack(DxEffect *effect __RWUNUSED__, RwUInt32 type,
    RwChar *name, void *data, void *callbackData)
{
    RwStream *success;
    RwUInt32 numBytes = 0;

    my_assert(name);
    my_assert(callbackData);

    switch (type)
    {
        case EFFECT_TWEAKABLE_PARAM_FLOAT:
            numBytes += sizeof(RwReal);
            break;
        case EFFECT_TWEAKABLE_PARAM_VECTOR:
            numBytes += sizeof(RwReal) * 4;
            break;
        case EFFECT_TWEAKABLE_PARAM_MATRIX4X4:
            numBytes += sizeof(RwReal) * 16;
            break;
		case EFFECT_TWEAKABLE_PARAM_TEXTURE:
			numBytes += sizeof(RwChar) * FX_FILE_NAME_LENGTH;
			break;
    }

    if(type == EFFECT_TWEAKABLE_PARAM_TEXTURE)
	{
		// get the texture name and output it.
		RwChar buf[FX_FILE_NAME_LENGTH];
		memset(buf, 0, numBytes);
		//@{ Jaewon 20040913
		// texture parameter writing bug fixed.
		if(*(RwTexture**)data)
			strcpy(buf, RwTextureGetName(*(RwTexture**)data));
		else
		{
			//@{ Jaewon 20041005
			//MD_SetErrorMessage("AcuRpMatD3DFx : A %s parameter not specified!", name);
			//RwChar tmp[256];
			//sprintf(tmp, "A %s parameter not specified!", name);
			//MessageBox(NULL, tmp, "AcuRpMatD3DFx", MB_OK);
			//@} Jaewon
		}
		//@} Jaewon
		success = RwStreamWrite((RwStream *)callbackData, buf, numBytes);
	}
    else
		success = RwStreamWriteReal((RwStream *)callbackData, (RwReal *)data, numBytes);
    if (NULL == success)
    {
        RwChar msg[MAX_MSG_SIZE] = "Failed to write effect tweakable parameter data:";
        strncat(msg, name, __min(strlen(name), MAX_STRING_LEN(msg)-strlen(msg)));
        _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "WriteTweakableParameterDataCallBack", msg);

        StreamWriteReadError = TRUE;
    }

    return success;
}


/*
 *************************************************************************************************************
 */
RwBool
EffectWrite(DxEffect *effect, RwStream *stream)
{
    RwStream *success;

    /* Writes the effect to the stream.  Returns NULL if there was an error */

    my_assert(effect);
    my_assert(stream);

    /* fx name */
    success = RwStreamWrite(stream, effect->pSharedD3dXEffect->name, sizeof(RwChar) * FX_FILE_NAME_LENGTH);
    if (NULL == success)
    {
        RwChar msg[MAX_MSG_SIZE] = "Failed to write effect name to stream:";
        strncat(msg, effect->pSharedD3dXEffect->name, __min(strlen(effect->pSharedD3dXEffect->name), MAX_STRING_LEN(msg)-strlen(msg)));
        _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectWrite", msg);
        return FALSE;
    }

	//@{ Jaewon 20040922
	// bSkin -> nWeights
	// nWeights
	success = RwStreamWriteInt32(stream, &effect->pSharedD3dXEffect->nWeights, sizeof(RwInt32));
    if (NULL == success)
    {
        RwChar msg[MAX_MSG_SIZE] = "Failed to write effect nWeights";
        _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectWrite", msg);
        return FALSE;
    }
	//@} Jaewon

    /* how many parameters to write */
    success = RwStreamWriteInt32(stream, &effect->tweakableParams.count, sizeof(RwUInt32));
    if (NULL == success)
    {
        RwChar msg[] = "Failed to write effect tweakableParams.count";
        _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectWrite", msg);
        return FALSE;
    }

    if (effect->tweakableParams.count > 0)
    {
        /* the parameters type */
		//@{ Jaewon 20050722
		// ;)
		RwUInt32 type[MAX_TWEAKABLE_PARAM_COUNT];
		RwUInt32 i;
		for(i=0; i<effect->tweakableParams.count; ++i)
			type[i] = effect->tweakableParams.type[i];
		success = RwStreamWriteInt32(stream, type, sizeof(RwUInt32) *
			effect->tweakableParams.count);
		//@} Jaewon
        if (NULL == success)
        {
            RwChar msg[] = "Failed to write effect tweakableParams.type";
            _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectWrite", msg);
            return FALSE;
        }

        /* its name */
        StreamWriteReadError = FALSE;
        EffectForAllTweakableParameters(effect, WriteTweakableParameterNameCallBack, (void *)stream);
        if (StreamWriteReadError)
        {
            return FALSE;
        }

        /* finally its data */
        StreamWriteReadError = FALSE;
        EffectForAllTweakableParameters(effect, WriteTweakableParameterDataCallBack, (void *)stream);
        if (StreamWriteReadError)
        {
            return FALSE;
        }
    }

    return TRUE;
}


/*
 *************************************************************************************************************
 */
#define STREAM_READ_FREE_TEMP_ALLOCS(data_, name_, type_)   \
do{                                                         \
    if (data_) RwFree(data_);                               \
    if (name_) RwFree(name_);                               \
    if (type_) RwFree(type_);                               \
}while(0)

//@{ Jaewon 20050810
// ;)
void ValidateTweakableParameters(DxEffect *effect)
{
	DxEffectTweakableParams params;
	RwUInt32 realIndex, vectorIndex, matrix4x4Index, textureIndex;
	RwUInt32 i;
	HandleFloat *handleFloat;
	HandleVector *handleVector;
	HandleMatrix4x4 *handleMatrix;
	HandleTexture *handleTexture;

	my_assert(effect);

	realIndex = 0;
	vectorIndex = 0;
	matrix4x4Index = 0;
	textureIndex = 0;
	handleFloat = effect->real;
	handleVector = effect->vector;
	handleMatrix = effect->matrix4x4;
	handleTexture = effect->texture;

	params = effect->tweakableParams;
	effect->tweakableParams.count = 0;
	for (i=0; i<params.count; ++i)
	{
		RwBool valid = FALSE;

		switch (params.type[i])
		{
		case EFFECT_TWEAKABLE_PARAM_FLOAT:
			{
				while (handleFloat)
				{
					if (handleFloat->tweakable)
					{
						++realIndex;
						handleFloat = handleFloat->next;
						valid = TRUE;
						break;
					}
					++realIndex;
					handleFloat = handleFloat->next;
				}
				break;
			}

		case EFFECT_TWEAKABLE_PARAM_VECTOR:
			{
				while (handleVector)
				{
					if (handleVector->tweakable)
					{
						++vectorIndex;
						handleVector = handleVector->next;
						valid = TRUE;
						break;
					}
					++vectorIndex;
					handleVector = handleVector->next;
				}
				break;
			}

		case EFFECT_TWEAKABLE_PARAM_MATRIX4X4:
			{
				while (handleMatrix)
				{
					if (handleMatrix->tweakable)
					{
						++matrix4x4Index;
						handleMatrix = handleMatrix->next;
						valid = TRUE;
						break;
					}
					++matrix4x4Index;
					handleMatrix = handleMatrix->next;
				}
				break;
			}

		case EFFECT_TWEAKABLE_PARAM_TEXTURE:
			{
				while (handleTexture)
				{
					if (handleTexture->tweakable)
					{
						++textureIndex;
						handleTexture = handleTexture->next;
						valid = TRUE;
						break;
					}
					++textureIndex;
					handleTexture = handleTexture->next;
				}
				break;
			}

		case EFFECT_TWEAKABLE_PARAM_NATYPE:
		default:
			{
				RwChar msg[] = "Unknown type. Callback will not be called for this parameter.";
				_rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectForAllTweakableParameters",
					msg);
				break;
			}
		}

		if (valid)
		{
			effect->tweakableParams.type[effect->tweakableParams.count] = params.type[i];
			++(effect->tweakableParams.count);
		}
	}

	return;
}
//@} Jaewon

DxEffect *
EffectRead(RwStream *stream)
{
    DxEffect *effect;
    RwChar name[FX_FILE_NAME_LENGTH];
    RwStream *success;
    RwUInt32 successInt;
	//@{ Jaewon 20040922
	// bSkin -> nWeights
	RwUInt32 nWeights;
	//@} Jaewon

	//@{ Jaewon 20041025
	//EnterCriticalSection(&_criticalSection);
	
    /* Returns an effect from the stream. Returns NULL if there was an error */

    my_assert(stream);
    
    /* fx name */
    successInt = RwStreamRead(stream, name, sizeof(RwChar) * FX_FILE_NAME_LENGTH);
    if (successInt != sizeof(RwChar) * FX_FILE_NAME_LENGTH)
    {
		//LeaveCriticalSection(&_criticalSection);
        return NULL;
    }

	//@{ Jaewon 20040922
	// bSkin -> nWeights
	// bSkin
	success = RwStreamReadInt32(stream, &nWeights, sizeof(RwUInt32));
	//@} Jaewon
    if (NULL == success)
    {
		//LeaveCriticalSection(&_criticalSection);
        return NULL;
    }

	//@{ Jaewon 20040922
	// bSkin -> nWeights
    effect = EffectLoad(name, nWeights);
	//@} Jaewon
    if (effect)
    {
        if (0)//EffectStreamCurrentEffectIsReference)
        {
            RwUInt32 count;

            /* the effect is referenced by multiple materials so I will not read in the parameter data as
               this will overwrite the data for all materials using this effect. The parameter data will only
               be taken from the first material streamed in.  

               If effects are to be referenced then a better streaming system would be to write the effect
               parameter data to a seperate file (.rfx for example) rather than embed the data into the
               material stream as it will just be ignored. The material write and read callbacks would just
               write out the effect name and attach the effect to the material */

            /* how many bytes to skip, remember I have already read some of the data. I cannot use
               EffectGetSize because the fx file could have been updated after the data was streamed out and
               would return the size of the new effect */
            
            /* how many parameters are there */
            success = RwStreamReadInt32(stream, &count, sizeof(RwUInt32));
            if (NULL == success)
            {
				//LeaveCriticalSection(&_criticalSection);
                return NULL;
            }

            if (count > 0)
            {
                RwUInt32 offset;
                RwUInt32 *type;
                RwUInt32 i;

                type = (RwUInt32 *)RwMalloc(sizeof(RwUInt32) * count, MATERIAL_FX_PLUGIN |
                    rwMEMHINTDUR_FUNCTION);
                if (NULL == type)
                {
					//LeaveCriticalSection(&_criticalSection);
                    return NULL;
                }

                success = RwStreamReadInt32(stream, type, sizeof(RwUInt32) * count);
                if (NULL == success)
                {
                    RwFree(type);
					//LeaveCriticalSection(&_criticalSection);
                    return NULL;
                }

                for (i=0, offset = 0; i!=count; ++i)
                {
                    /* the size of the params name */
                    offset += sizeof(RwChar) * FX_HANDLE_NAME_LENGTH;

                    /* and its data */
                    switch (type[i])
                    {
                        case EFFECT_TWEAKABLE_PARAM_FLOAT:
                            offset += sizeof(RwReal);
                            break;
                        case EFFECT_TWEAKABLE_PARAM_VECTOR:
                            offset += sizeof(RwReal) * 4;
                            break;
                        case EFFECT_TWEAKABLE_PARAM_MATRIX4X4:
                            offset += sizeof(RwReal) * 16;
                            break;
						case EFFECT_TWEAKABLE_PARAM_TEXTURE:
							offset += sizeof(RwChar) * FX_FILE_NAME_LENGTH;
							break;
                    }
                }

                RwFree(type);

                success = RwStreamSkip(stream, offset);

//                EffectStreamCurrentEffectIsReference = FALSE;
                if( NULL == success)
                {
					//LeaveCriticalSection(&_criticalSection);
                    return NULL;
                }
            }

//            EffectStreamCurrentEffectIsReference = FALSE;
        }
        else
        {
            RwUInt32 count;

			//@{ Jaewon 20041008
			RwBool bJustSkip = ((((const D3DCAPS9*)RwD3D9GetCaps())->VertexShaderVersion & 0xffff) < 0x0101)
								|| (effect->pSharedD3dXEffect->refCount > 1);

			//@{ Jaewon 20050126
			// Do not set a technique during a loading...
			if(0)//!bJustSkip)			
			//@} Jaewon
			{
			RwBool bOK = EffectSetFirstValidTechnique(effect);
			//@{ Jaewon 20041005
			if(!bOK)
			//@} Jaewon
			{								
				RwChar tmp[256];
				sprintf(tmp, "No valid technique can be set!");
				my_assert(!"No valid technique can be set!");
//				MessageBox(NULL, tmp, "AcuRpMatD3DFx", MB_OK);
			}
			}

            /* how many parameters are there */
            success = RwStreamReadInt32(stream, &count, sizeof(RwUInt32));
            if (NULL == success)
            {
				//LeaveCriticalSection(&_criticalSection);
                return NULL;
            }

			//@{ Jaewon 20050722
			// ;)
			my_assert(count <= MAX_TWEAKABLE_PARAM_COUNT);
			effect->tweakableParams.count = count;
			//@} Jaewon
            if (count > 0)
            {
                /* must init these pointers to NULL when using STREAM_READ_FREE_TEMP_ALLOCS */
                RwUInt32 *type = NULL;
                RwChar *name = NULL;
                RwReal *data = NULL;
                RwUInt32 i, size;
                RwChar *paramName;
                RwReal *paramData;

                type = (RwUInt32 *)RwMalloc(sizeof(RwUInt32) * count, MATERIAL_FX_PLUGIN |
                    rwMEMHINTDUR_FUNCTION);
                if (NULL == type)
                {
					//LeaveCriticalSection(&_criticalSection);
                    return NULL;
                }

                success = RwStreamReadInt32(stream, type, sizeof(RwUInt32) * count);
                if (NULL == success)
                {
                    STREAM_READ_FREE_TEMP_ALLOCS(data, name, type);
                    //LeaveCriticalSection(&_criticalSection);
					return NULL;
                }

				//@{ Jaewon 20050722
				// ;)
				for(i=0; i<count; ++i)
					effect->tweakableParams.type[i] = (RwUInt8)type[i];
				//@} Jaewon

                name = (RwChar *)RwMalloc(sizeof(RwChar) * FX_FILE_NAME_LENGTH * count, MATERIAL_FX_PLUGIN |
                    rwMEMHINTDUR_FUNCTION);
                if (NULL == name)
                {
                    STREAM_READ_FREE_TEMP_ALLOCS(data, name, type);
                    //LeaveCriticalSection(&_criticalSection);
					return NULL;
                }

                successInt = RwStreamRead(stream, name, sizeof(RwChar) * FX_FILE_NAME_LENGTH * count);
                if (successInt != sizeof(RwChar) * FX_FILE_NAME_LENGTH * count)
                {
                    STREAM_READ_FREE_TEMP_ALLOCS(data, name, type);
                    //LeaveCriticalSection(&_criticalSection);
					return NULL;
                }

                for (i=0, size = 0; i!=count; ++i)
                {
                    switch (type[i])
                    {
                        case EFFECT_TWEAKABLE_PARAM_FLOAT:
                            size += sizeof(RwReal);
                            break;
                        case EFFECT_TWEAKABLE_PARAM_VECTOR:
                            size += sizeof(RwReal) * 4;
                            break;
                        case EFFECT_TWEAKABLE_PARAM_MATRIX4X4:
                            size += sizeof(RwReal) * 16;
                            break;
						case EFFECT_TWEAKABLE_PARAM_TEXTURE:
							size += sizeof(RwChar) * FX_FILE_NAME_LENGTH;
                    }
                }

                /* there should be a least one piece of floating point data */
                my_assert(size >= sizeof(RwReal)); 
                data = (RwReal *)RwMalloc(size, MATERIAL_FX_PLUGIN | rwMEMHINTDUR_FUNCTION);
                if (NULL == data)
                {
                    STREAM_READ_FREE_TEMP_ALLOCS(data, name, type);
                    //LeaveCriticalSection(&_criticalSection);
					return NULL;
                }

                paramName = name;
                paramData = data;
                for (i=0; i!=count; ++i)
                {
                    RwUInt32 offset;
                    D3DXMATRIX matrix4x4;
                    D3DXVECTOR4 vector;
					RwChar texName[FX_FILE_NAME_LENGTH];
                    RwReal real;
                    RwBool success;
					RwStream *streamSuccess;

                    switch (type[i])
                    {
                        case EFFECT_TWEAKABLE_PARAM_FLOAT:
							offset = 1;
							streamSuccess = RwStreamReadReal(stream, paramData, sizeof(RwReal)*offset);
							if (NULL == streamSuccess)
							{
								STREAM_READ_FREE_TEMP_ALLOCS(data, name, type);
								//LeaveCriticalSection(&_criticalSection);
								return NULL;
							}
                            real = *paramData;
                            success = EffectSetFloat(effect, paramName, real);
                            break;
                        case EFFECT_TWEAKABLE_PARAM_VECTOR:
							offset = 4;
							streamSuccess = RwStreamReadReal(stream, paramData, sizeof(RwReal)*offset);
							if (NULL == streamSuccess)
							{
								STREAM_READ_FREE_TEMP_ALLOCS(data, name, type);
								//LeaveCriticalSection(&_criticalSection);
								return NULL;
							}
                            memcpy(&vector, paramData, sizeof(RwReal) * offset);
                            success = EffectSetVector(effect, paramName, &vector);
                            break;
                        case EFFECT_TWEAKABLE_PARAM_MATRIX4X4:
							offset = 16;
							streamSuccess = RwStreamReadReal(stream, paramData, sizeof(RwReal)*offset);
							if (NULL == streamSuccess)
							{
								STREAM_READ_FREE_TEMP_ALLOCS(data, name, type);
								//LeaveCriticalSection(&_criticalSection);
								return NULL;
							}
                            memcpy(&matrix4x4, paramData, sizeof(RwReal) * offset);
                            success = EffectSetMatrix4x4(effect, paramName, &matrix4x4);
                            break;
						case EFFECT_TWEAKABLE_PARAM_TEXTURE:
							offset = sizeof(RwChar)*FX_FILE_NAME_LENGTH;
							successInt = RwStreamRead(stream, paramData, offset);
							if (offset != successInt)
							{
								STREAM_READ_FREE_TEMP_ALLOCS(data, name, type);
								//LeaveCriticalSection(&_criticalSection);
								return NULL;
							}
							memcpy(texName, paramData, offset);
							//@{ Jaewon 20041005
							{
							RwTexture *texture = RwTextureRead(texName, NULL);
							//@{ Jaewon 20041210
							// RwTextureRead(texName, NULL) -> texture
                            success = EffectSetTexture(effect, paramName, texture);
							//@} Jaewon
							}
							//@} Jaewon
							break;
                    }

                    if (!success)
                    {
                        RwChar msg[MAX_MSG_SIZE];
                        RwChar *typeStr = NULL;
                        RwChar floatStr[] = "float";
                        RwChar vectorStr[] = "vector";
                        RwChar matrix4x4Str[] = "matrix4x4";
						RwChar textureStr[] = "texture";

                        switch (type[i])
                        {
                            case EFFECT_TWEAKABLE_PARAM_FLOAT:
                                typeStr = floatStr;
                                break;
                            case EFFECT_TWEAKABLE_PARAM_VECTOR:
                                typeStr = vectorStr;
                                break;
                            case EFFECT_TWEAKABLE_PARAM_MATRIX4X4:
                                typeStr = matrix4x4Str;
                                break;
							case EFFECT_TWEAKABLE_PARAM_TEXTURE:
								typeStr = textureStr;
								break;
                        }
                        _snprintf(msg, MAX_MSG_SIZE, "Effect: %s does not have param: %s of type: %s from \
stream: 0x%p. This parameter will be ignored", effect->pSharedD3dXEffect->name, paramName, typeStr, (void *)stream);

                        _rwDebugSendMessage(rwDEBUGMESSAGE, __FILE__, __LINE__, "EffectRead", msg);
                    }

                    paramName += sizeof(RwChar) * FX_FILE_NAME_LENGTH;
                    if(type[i] == EFFECT_TWEAKABLE_PARAM_TEXTURE)
						paramData = (RwReal*)((RwUInt32)paramData + offset);
					else
						paramData += offset;
                }

                STREAM_READ_FREE_TEMP_ALLOCS(data, name, type);
            }
			//@} Jaewon
        }

		//@{ Jaewon 20050810
		// ;)
		ValidateTweakableParameters(effect);
		//@} Jaewon

        //LeaveCriticalSection(&_criticalSection);
		return effect;
    }

	//LeaveCriticalSection(&_criticalSection);
	//@} Jaewon

    return NULL;
}


/*
 *************************************************************************************************************
 */
void
EffectRelease(DxEffect *effect)
{
    /* Release the effect. If the effect's reference count equals zero the effect and ID3DXEffect will be
       destroyed */

    RwUInt32 i;
	HandleMatrix4x4 *handleMatrix;
	HandleTexture *handleTexture;
	HandleFloat *handleFloat;
	HandleVector *handleVector;

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    
   //@{ Jaewon 20050722
	// for memory pooling
	handleTexture = effect->texture;
	for (i=0; i!=effect->textureCount; ++i)
	{
		if (handleTexture->data)
		{
			RwTextureDestroy(handleTexture->data);
			handleTexture->data = NULL;
		}
		handleTexture = handleTexture->next;
	}
	//@} Jaewon

	if(effect->pSharedD3dXEffect->refCount > 0)
	{
	effect->pSharedD3dXEffect->refCount -= 1;
	if(effect->pSharedD3dXEffect->refCount == 0)
	{
		//@{ Jaewon 20041102
		// lighting optimization
		for(i=0; i<FX_LIGHTING_VARIATION; ++i)
			effect->pSharedD3dXEffect->d3dxEffect[i]->lpVtbl->Release(effect->pSharedD3dXEffect->d3dxEffect[i]);
		//@} Jaewon
		D3dXEffectListRemove(effect->pSharedD3dXEffect);
		RwFree(effect->pSharedD3dXEffect);
	}
	}
	
    //@{ Jaewon 20050722
	// For memory pooling
	handleMatrix = effect->matrix4x4;
	while(handleMatrix)
	{
		HandleMatrix4x4 *next = handleMatrix->next;
		RwFreeListFree(HandleMatrix4x4FreeList, handleMatrix);
		handleMatrix = next;
	}
	effect->matrix4x4 = NULL;

	handleTexture = effect->texture;
	while(handleTexture)
	{
		HandleTexture *next = handleTexture->next;
		RwFreeListFree(HandleTextureFreeList, handleTexture);
		handleTexture = next;
	}
	effect->texture = NULL;

	handleFloat = effect->real;
	while(handleFloat)
	{
		HandleFloat *next = handleFloat->next;
		RwFreeListFree(HandleFloatFreeList, handleFloat);
		handleFloat = next;
	}
	effect->real = NULL;

	handleVector = effect->vector;
	while(handleVector)
	{
		HandleVector *next = handleVector->next;
		RwFreeListFree(HandleVectorFreeList, handleVector);
		handleVector = next;
	}
	effect->vector = NULL;
	//@} Jaewon

	memset(effect->rw, 0, sizeof(effect->rw));

    //@{ Jaewon 20050722
	// Memory pooling for performance
	RwFreeListFree(DxEffectFreeList, effect);
	//@} Jaewon
    
    return;
}


/*
 *************************************************************************************************************
 */
static BOOL
//@{ Jaewon 20040922
// bSkin -> nWeights
//@{ Jaewon 20041110
// md5 encryption support
EffectLoadFromFile(DxEffect *effect, RwChar *fxFile, RwChar *fxName, RwUInt32 nWeights, RwChar *fxMD5)
//@} Jaewon
//@} Jaewon
{
    LPD3DXBUFFER compileErrors;
    HRESULT hr;
    DWORD flags = 0;
	int i;

	//@{ Jaewon 20040922
	// NUM_WEIGHTS macro added.
	char buf[8];
	char buf2[8];
	char buf3[8];
	//@{ Jaewon 20050309
	// 25 -> 30
	int  maxBonesConstantSpace = ((D3DCAPS9 *)RwD3D9GetCaps())->MaxVertexShaderConst - 30;
	//@} Jaewon
	D3DXMACRO macro[] = {
		{ "SKINNING", "1" },
		{ "MAX_BONES_CONSTANT_SPACE", buf },
		{ "NUM_WEIGHTS", buf2 },
		//@{ Jaewon 20041102
		// lighting optimization
		{ "NUM_LIGHTS", buf3},
		//@} Jaewon
		{ NULL, NULL }
	};
	//@{ Jaewon 20041008
	// MaxVertexShaderConst can be null.
	if(maxBonesConstantSpace <= 0)
		maxBonesConstantSpace = 1;
	//@} Jaewon
	_itoa(maxBonesConstantSpace, buf, 10);
	_itoa(nWeights, buf2, 10);
	//@} Jaewon

    my_assert(effect);
    my_assert(fxFile);  

    effect->pSharedD3dXEffect = (SharedD3dXEffectPtr *)RwMalloc(sizeof(SharedD3dXEffectPtr), MATERIAL_FX_PLUGIN | rwMEMHINTDUR_EVENT);
    my_assert(effect->pSharedD3dXEffect);
	//@{ Jaewon 20041102
	// lighting optimization
	effect->pSharedD3dXEffect->d3dxEffect[0] = NULL;
	effect->pSharedD3dXEffect->d3dxEffect[1] = NULL;
	effect->pSharedD3dXEffect->d3dxEffect[2] = NULL;
	//@} Jaewon
	effect->pSharedD3dXEffect->refCount = 0;
	strncpy(effect->pSharedD3dXEffect->name, "no_fx_set", FX_FILE_NAME_LENGTH);
	//@{ Jaewon 20040922
	// bSkin -> nWeights
	effect->pSharedD3dXEffect->nWeights = nWeights;
    
#ifdef RWDEBUG
    /*flags |= D3DXSHADER_DEBUG;*/
    /*flags |= D3DXSHADER_SKIPOPTIMIZATION;*/
#endif

	//@{ Jaewon 20041102
	// lighting optimization
	if(fxMD5)
	// encrypted file
	{
		FILE *pFile;
		long fileSize;
		char *buffer;
		size_t readBytes;
		BOOL bOK;

		// get the file to a memory.
		pFile = fopen(fxMD5, "rb");
		my_assert(pFile);

		fseek(pFile, 0, SEEK_END);	
	    fileSize = ftell(pFile);		
		fseek(pFile, 0, SEEK_SET);

		buffer = malloc(fileSize);
		memset(buffer, 0, fileSize);
		readBytes = fread(buffer, sizeof(char), fileSize, pFile);
		my_assert(readBytes == fileSize);
		fclose(pFile);

		// decrypt the file.
		bOK = DecryptMD5(buffer, fileSize);
		if(bOK == FALSE)
		{
			free(buffer);
			my_assert(0);
		}
		else
		{
			for(i=0; i<FX_LIGHTING_VARIATION; ++i)
			{
				_itoa(i, buf3, 10);
				hr = D3DXCreateEffect(EffectD3DDevice, buffer, fileSize, nWeights>0?macro:(&(macro[3])), NULL, flags, NULL, &effect->pSharedD3dXEffect->d3dxEffect[i],
					&compileErrors);
				if(FAILED(hr))
					break;
			}

			free(buffer);
		}
	}
	else
	// plain file
	{
		for(i=0; i<FX_LIGHTING_VARIATION; ++i)
		{
			_itoa(i, buf3, 10);
			hr = D3DXCreateEffectFromFile(EffectD3DDevice, fxFile, nWeights>0?macro:(&(macro[3])), NULL, flags, NULL, &effect->pSharedD3dXEffect->d3dxEffect[i],
				&compileErrors);
			if(FAILED(hr))
				break;
		}
	}
	//@} Jaewon
	//@} Jaewon
	//@} Jaewon
    if (SUCCEEDED(hr))
    {
        EffectBuildParams(effect);
        strncpy(effect->pSharedD3dXEffect->name, fxName, FX_FILE_NAME_LENGTH);
        effect->pSharedD3dXEffect->refCount = 1;
		
		//@{ Jaewon 20050126
		effect->pSharedD3dXEffect->technicSet = FALSE;
		//@} Jaewon

		//@{ Jaewon 20041022
		// the second argument added.
		D3dXEffectListAdd(effect->pSharedD3dXEffect, effect);
		//@} Jaewon

		//@{ Jaewon 20041007
		// use RenderWare state manager.
		//@{ Jaewon 20041102
		// lighting optimization
		for(i=0; i<FX_LIGHTING_VARIATION; ++i)
			EffectSetStageManager(effect->pSharedD3dXEffect->d3dxEffect[i]);
		//@} Jaewon
		//@} Jaewon
    }
    else
    {
        RwInt32 length;
        RwChar *errors;
        RwChar msg1[] = "D3DXCreateEffectFromFile failed.";
        RwChar *msg2;

        _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectLoadFromFile", msg1);

        errors = compileErrors->lpVtbl->GetBufferPointer(compileErrors);
        length = (RwInt32)strlen(errors) + 1;
        msg2 = (RwChar *)RwMalloc(length, MATERIAL_FX_PLUGIN | rwMEMHINTDUR_FUNCTION);
        if (msg2)
        {
            strncpy(msg2, errors, length);
            _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectLoadFromFile", msg2);
            RwFree(msg2);
        }

		if(effect->pSharedD3dXEffect)
		{
			RwFree(effect->pSharedD3dXEffect);
			effect->pSharedD3dXEffect = NULL;
		}

		return FALSE;
    }

    if (compileErrors)
    {
        compileErrors->lpVtbl->Release(compileErrors);
    }

	return TRUE;
}


/*
 *************************************************************************************************************
 */
//@{ Jaewon 20041022
static void EffectCopyParams(DxEffect *dstEffect, DxEffect *srcEffect)
{
	unsigned int i, j;
	HandleFloat *handleFloat;
	HandleVector *handleVector;
	HandleMatrix4x4 *handleMatrix;
	HandleTexture *handleTexture;
	HandleFloat *srcHandleFloat;
	HandleVector *srcHandleVector;
	HandleMatrix4x4 *srcHandleMatrix;
	HandleTexture *srcHandleTexture;
	
    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(dstEffect);
	my_assert(srcEffect);

    my_assert(0 == dstEffect->realCount);
    my_assert(0 == dstEffect->vectorCount);
    my_assert(0 == dstEffect->matrix4x4Count);
    my_assert(0 == dstEffect->textureCount);
    my_assert(0 == dstEffect->tweakableParams.count);

	// copy counts.
    dstEffect->realCount = srcEffect->realCount;
	dstEffect->vectorCount = srcEffect->vectorCount;
	dstEffect->matrix4x4Count = srcEffect->matrix4x4Count;
	dstEffect->textureCount = srcEffect->textureCount;
	dstEffect->tweakableParams.count = srcEffect->tweakableParams.count;

	// memory allocations
    //@{ Jaewon 20050722
	// Memory pooling for performance
	for(i=0; i<dstEffect->realCount; ++i)
	{
		handleFloat = (HandleFloat *)RwFreeListAlloc(HandleFloatFreeList, MATERIAL_FX_PLUGIN | 
			rwMEMHINTDUR_EVENT);
		my_assert(handleFloat);
		handleFloat->next = dstEffect->real;
		dstEffect->real = handleFloat;
	}

	for(i=0; i<dstEffect->vectorCount; ++i)
	{
		handleVector = (HandleVector *)RwFreeListAlloc(HandleVectorFreeList, MATERIAL_FX_PLUGIN | 
			rwMEMHINTDUR_EVENT);
		my_assert(handleVector);
		handleVector->next = dstEffect->vector;
		dstEffect->vector = handleVector;
	}

	for(i=0; i<dstEffect->matrix4x4Count; ++i)
	{
		handleMatrix = (HandleMatrix4x4 *)RwFreeListAlloc(HandleMatrix4x4FreeList, MATERIAL_FX_PLUGIN | 
			rwMEMHINTDUR_EVENT);
		my_assert(handleMatrix);
		handleMatrix->next = dstEffect->matrix4x4;
		dstEffect->matrix4x4 = handleMatrix;
	}
    
	for(i=0; i<dstEffect->textureCount; ++i)
	{
		handleTexture = (HandleTexture *)RwFreeListAlloc(HandleTextureFreeList, MATERIAL_FX_PLUGIN | 
			rwMEMHINTDUR_EVENT);
		my_assert(handleTexture);
		handleTexture->next = dstEffect->texture;
		dstEffect->texture = handleTexture;
	}
	//@} Jaewon

    if (dstEffect->tweakableParams.count > 0)
    {
		//@{ Jaewon 20050722
		// No dynamic allocation
		my_assert(dstEffect->tweakableParams.count <= MAX_TWEAKABLE_PARAM_COUNT);
		memset(dstEffect->tweakableParams.type, 0, sizeof(RwUInt8) * dstEffect->tweakableParams.count);
		//@} Jaewon
    }

	// copy data.
	//@{ Jaewon 20050722
	// For memory pooling
	srcHandleFloat = srcEffect->real;
	for(i=0; i<dstEffect->realCount; ++i)
	{
		handleFloat->data = srcHandleFloat->data;
		//@{ Jaewon 20041102
		for(j=0; j<FX_LIGHTING_VARIATION; ++j)
			handleFloat->handle[j] = srcHandleFloat->handle[j];
		//@} Jaewon
		handleFloat->tweakable = srcHandleFloat->tweakable;
		strncpy(handleFloat->name, srcHandleFloat->name, FX_HANDLE_NAME_LENGTH);

		handleFloat = handleFloat->next;
		srcHandleFloat = srcHandleFloat->next;
	}

	srcHandleVector = srcEffect->vector;
	for(i=0; i<dstEffect->vectorCount; ++i)
	{
		handleVector->data = srcHandleVector->data;
		//@{ Jaewon 20041102
		for(j=0; j<FX_LIGHTING_VARIATION; ++j)
			handleVector->handle[j] = srcHandleVector->handle[j];
		//@} Jaewon
		handleVector->tweakable = srcHandleVector->tweakable;
		strncpy(handleVector->name, srcHandleVector->name, FX_HANDLE_NAME_LENGTH);

		handleVector = handleVector->next;
		srcHandleVector = srcHandleVector->next;
	}

	srcHandleMatrix = srcEffect->matrix4x4;
	for(i=0; i<dstEffect->matrix4x4Count; ++i)
	{
		handleMatrix->data = srcHandleMatrix->data;
		//@{ Jaewon 20041102
		for(j=0; j<FX_LIGHTING_VARIATION; ++j)
			handleMatrix->handle[j] = srcHandleMatrix->handle[j];
		//@} Jaewon
		handleMatrix->tweakable = srcHandleMatrix->tweakable;
		strncpy(handleMatrix->name, srcHandleMatrix->name, FX_HANDLE_NAME_LENGTH);

		handleMatrix = handleMatrix->next;
		srcHandleMatrix = srcHandleMatrix->next;
	}

	srcHandleTexture = srcEffect->texture;
	for(i=0; i<dstEffect->textureCount; ++i)
	{
		//@{ Jaewon 20041210
		handleTexture->data = srcHandleTexture->data;//==NULL?NULL:
		//	  RwTextureRead(RwTextureGetName(srcEffect->texture[i].data), NULL);
		if(handleTexture->data)
			RwTextureAddRef(handleTexture->data);
		//@} Jaewon

		//@{ Jaewon 20041102
		for(j=0; j<FX_LIGHTING_VARIATION; ++j)
			handleTexture->handle[j] = srcHandleTexture->handle[j];
		//@} Jaewon
		handleTexture->tweakable = srcHandleTexture->tweakable;
		strncpy(handleTexture->name, srcHandleTexture->name, FX_HANDLE_NAME_LENGTH);

		handleTexture = handleTexture->next;
		srcHandleTexture = srcHandleTexture->next;
	}
	//@} Jaewon
	
	for(i=0; i<ID_TOTAL; ++i)
	{
		//@{ Jaewon 20041102
		for(j=0; j<FX_LIGHTING_VARIATION; ++j)
			dstEffect->rw[i].handle[j] = srcEffect->rw[i].handle[j];
		//@} Jaewon
		strncpy(dstEffect->rw[i].name, srcEffect->rw[i].name, FX_HANDLE_NAME_LENGTH);
	}
	
	for(i=0; i<dstEffect->tweakableParams.count; ++i)
	{
		dstEffect->tweakableParams.type[i] = srcEffect->tweakableParams.type[i];
	}

	// etc.
	dstEffect->cameraPositionSpace = srcEffect->cameraPositionSpace;
	dstEffect->cameraDirectionSpace = srcEffect->cameraDirectionSpace;
	dstEffect->lightDirectionSpace = srcEffect->lightDirectionSpace;
	//@{ Jaewon 20050419
	// Up to 8 lights
	memcpy(dstEffect->localLightPositionSpace, srcEffect->localLightPositionSpace, sizeof(srcEffect->localLightPositionSpace));
	memcpy(dstEffect->localLightDirectionSpace, srcEffect->localLightDirectionSpace, sizeof(srcEffect->localLightDirectionSpace));
	//@} Jaewon

	dstEffect->constUploadCallBack = srcEffect->constUploadCallBack;
}
//@} Jaewon

DxEffect *
//@{ Jaewon 20040922
// bSkin -> nWeights
EffectLoad(RwChar *fxName, RwUInt32 nWeights)
//@} Jaewon
{
    RwChar fxPath[_MAX_PATH];
    DxEffect *effect;
	//@{ Jaewon 20041022
	DxEffect *pD3dXEffect;
	//@} Jaewon

	//@{ Jaewon 20041025
	//EnterCriticalSection(&_criticalSection);

    my_assert(fxName);
    my_assert(EffectD3DDevice);

    /* Returns the effect who is named fxName. Depending upon the compile options of this file will either
       return a reference to an existing effect or load a new instance of the effect from disk. Returns NULL
       if there was an error */

    TEST_EFFECT_NAME_LENGTH(fxName, "EffectLoad");

    //@{ Jaewon 20050722
	// Memory pooling for performance
	effect = (DxEffect *)RwFreeListAlloc(DxEffectFreeList, MATERIAL_FX_PLUGIN | rwMEMHINTDUR_EVENT);
	//@} Jaewon
    my_assert(effect);
    EffectInitEffect(effect);

	//@{ Jaewon 20040922
	// bSkin -> nWeights
    pD3dXEffect = EffectFindEffect(fxName, nWeights);
	//@} Jaewon
    if (pD3dXEffect)
    {
		//@{ Jaewon 20041022
		effect->pSharedD3dXEffect = pD3dXEffect->pSharedD3dXEffect;
		my_assert(effect->pSharedD3dXEffect);
		effect->pSharedD3dXEffect->refCount += 1;	
		//@{ Jaewon 20041102
		// lighting optimization
		my_assert(effect->pSharedD3dXEffect->d3dxEffect[0]);
		my_assert(effect->pSharedD3dXEffect->d3dxEffect[1]);
		my_assert(effect->pSharedD3dXEffect->d3dxEffect[2]);
		//@} Jaewon

		EffectCopyParams(effect, pD3dXEffect);
        //EffectBuildParams(effect);
		//@} Jaewon
		
		//LeaveCriticalSection(&_criticalSection);
        return effect;
    }
    else
    {
		//@{ Jaewon 20041110
		RwChar fxMD5[MAX_PATH];
		RwInt32 len;
		BOOL bEncryption = FALSE;
		//@} Jaewon

        strncpy(fxPath, RpMaterialD3DFxGetSeachPath(), MAX_PATH);
        /* see MSDN example code for strncat to avoid buffer overrun */
        strncat(fxPath, fxName, __min(strlen(fxName), MAX_STRING_LEN(fxPath)-strlen(fxPath)));
        
		//@{ Jaewon 20041110
		// support md5 encryption of the fx file.
		fxMD5[MAX_PATH-1] = '\0';
		strncpy(fxMD5, fxPath, MAX_PATH-1);
		len = (RwInt32)strlen(fxMD5);
		while (len >= 0)
		{
			if (fxMD5[len] == '.')
			{
				fxMD5[len] = '\0';
				//@{ Jaewon 20050307
				// case-insensitive
				my_assert((fxMD5[len+1]=='f' || fxMD5[len+1]=='F') && (fxMD5[len+2]=='x' || fxMD5[len+2]=='X'));
				//@} Jaewon
				break;
			}
			len--;
		}
		strcat(fxMD5, ".txt");
		if(RwFexist(fxMD5))
			bEncryption = TRUE;
        if (bEncryption || RwFexist(fxPath))
		//@} Jaewon
        {
			//@{ Jaewon 20040922
			// bSkin -> nWeights
			//@{ Jaewon 20041110
			// bEncryption
            BOOL bOK = EffectLoadFromFile(effect, fxPath, fxName, nWeights, bEncryption?fxMD5:NULL);
			//@} Jaewon
			//@} Jaewon
            if (bOK)
            {
				//LeaveCriticalSection(&_criticalSection);
                return effect;
            }
            else
            {
                RwChar msg[MAX_MSG_SIZE] = "Cannot load or compile fx file:";

                strncat(msg, fxPath, __min(strlen(fxPath), MAX_STRING_LEN(msg)-strlen(msg)));
                _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectLoad", msg);
            }
        }
        else
        {
            RwChar msg[MAX_MSG_SIZE] = "Cannot find fx file:";

            strncat(msg, fxPath, __min(strlen(fxPath), MAX_STRING_LEN(msg)-strlen(msg)));
            _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectLoad", msg);
        }
    }

    if(effect)
    {   
        //@{ Jaewon 20050722
		// Memory pooling for performance
		RwFreeListFree(DxEffectFreeList, effect);
		//@} Jaewon
        effect = NULL;
    }

	//LeaveCriticalSection(&_criticalSection);
	//@} Jaewon
    return NULL;
}


/*
 *************************************************************************************************************
 */
#if defined(EFFECT_USE_IMAGE_PATH_IN_FX)
static RwChar *
Win32PathnameCreate(const RwChar *srcBuffer)
{
    RwChar *dstBuffer;
    RwChar *charToConvert;

    /* First duplicate the string */
    dstBuffer = (RwChar *)RwMalloc(sizeof(RwChar) * (strlen(srcBuffer) + 1), MATERIAL_FX_PLUGIN | 
        rwMEMHINTDUR_FUNCTION);
    if (dstBuffer)
    {
        strncpy(dstBuffer, srcBuffer, strlen(srcBuffer)+1);

        /* Convert a path for use on Windows. Convert all /s into \s */
        while ((charToConvert = strchr(dstBuffer, '/')))
        {
            *charToConvert = '\\';
        }
    }

    return dstBuffer;
}

static void
Win32PathnameDestroy(RwChar *buffer)
{
    if (buffer)
    {
        RwFree(buffer);
    }
    return;
}
#endif /* EFFECT_USE_IMAGE_PATH_IN_FX */


/*
 *************************************************************************************************************
 */
static RwTexture *
EffectRwTextureReadFromFx(LPD3DXEFFECT d3dxEffect, D3DXHANDLE handle)
{
    RwChar texName[rwTEXTUREBASENAMELENGTH];
    RwChar imagePath[_MAX_PATH];
    RwChar *path;
    D3DXHANDLE annot;
    RwInt32 len;
    HRESULT hr;
    RwTexture *texture;

#if defined(EFFECT_RESTORE_IMAGE_PATH)
    RwChar *oldPath;
#endif /* EFFECT_RESTORE_IMAGE_PATH */

    my_assert(d3dxEffect);
    my_assert(handle);

    /* get the filename for the texture */
    annot = d3dxEffect->lpVtbl->GetAnnotationByName(d3dxEffect, handle, "ResourceName");
    if (NULL == annot)
    {
/*      RwChar msg[] = "Texture parameter does not have string \"filename\" annotation. Texture will not be \
loaded.";
        _rwDebugSendMessage(rwDEBUGERROR, __FILE__, __LINE__, "EffectRwTextureReadFromFx", msg);
*/      return NULL;
    }

    hr = d3dxEffect->lpVtbl->GetString(d3dxEffect, annot, &path);
    my_assert(SUCCEEDED(hr));

    /* build an image path and texture name suitable for loading by RW */
    strncpy(imagePath, path, _MAX_PATH);
    len = (RwInt32)strlen(imagePath);
    while (len >= 0)
    {
        if (imagePath[len] == '/')
        {
            imagePath[len+1] = '\0';
            break;
        }
        len--;
    }

    strncpy(texName, path+len+1, rwTEXTUREBASENAMELENGTH);
    len = (RwInt32)strlen(texName);
    while (len >= 0)
    {
        if (texName[len] == '.')
        {
            texName[len] = '\0';
            break;
        }
        len--;
    }

#if defined(EFFECT_RESTORE_IMAGE_PATH)
    len = (RwInt32)strlen(RwImageGetPath()) + 1;
    oldPath = (RwChar *)RwMalloc(len * sizeof(RwChar), MATERIAL_FX_PLUGIN | rwMEMHINTDUR_FUNCTION);
    if( oldPath )
    {
        strncpy(oldPath, RwImageGetPath(), len);
    }
#endif /* EFFECT_RESTORE_IMAGE_PATH */

#if defined(EFFECT_USE_IMAGE_PATH_IN_FX)
    path = Win32PathnameCreate(imagePath);
    my_assert(path);
    RwImageSetPath(path);
    Win32PathnameDestroy(path);
#endif /* EFFECT_USE_IMAGE_PATH_IN_FX */

    /* if the application wants dds texture support then it should overload the texture read callback. See the
       example viewer for what to do */
    texture = RwTextureRead(texName, NULL);
	//BOB(100105)
	if (NULL != texture)
	{
		//@{ Jaewon 20041210
		RwTextureAddRef(texture);
		//@} Jaewon
	}

#if defined(EFFECT_RESTORE_IMAGE_PATH)
    if (oldPath)
    {
        RwImageSetPath(oldPath);
        RwFree(oldPath);
    }
#endif /* EFFECT_RESTORE_IMAGE_PATH */

	//@{ Jaewon 20041005
	if(texture == NULL)
	{
		//BOB(100105)
		//@{ Jaewon 20050519
#ifdef DEBUG
		D3DXPARAMETER_DESC desc;
		d3dxEffect->lpVtbl->GetParameterDesc(d3dxEffect, handle, &desc);
		AulogFile("ERROR_EXPORT_RESOURCE.TXT", "%에서 %s가 없습니다.", desc.Name, texName);
#endif
		//@} Jaewon
		//RwChar tmp[256];
		//sprintf(tmp, "A texture \"%s\" not found!", texName);
		//my_assert(!"AcuRpMatD3DFx::Texture Not Found");
//		MessageBox(NULL, tmp, "AcuRpMatD3DFx", MB_OK);
	}
	//@} Jaewon

    return texture;
}


static int getRwSemanticID(const char *semantic)
{
	if(semantic == NULL)
		return -1;

	if(0 == strcmp(semantic, RW_WORLD_VIEW_PROJ_MATRIX))
		return ID_WORLD_VIEW_PROJ_MATRIX;
	else if(0 == strcmp(semantic, RW_PROJ_MATRIX))
		return ID_PROJ_MATRIX;
	else if(0 == strcmp(semantic, RW_WORLD_VIEW_MATRIX))
		return ID_WORLD_VIEW_MATRIX;
	else if(0 == strcmp(semantic, RW_CAMERA_POSITION))
		return ID_CAMERA_POSITION;
	else if(0 == strcmp(semantic, RW_CAMERA_DIRECTION))
		return ID_CAMERA_DIRECTION;
	else if(0 == strcmp(semantic, RW_BONE_MATRICES))
		return ID_BONE_MATRICES;
	else if(0 == strcmp(semantic, RW_WORLD_MATRIX))
		return ID_WORLD_MATRIX;
	else if(0 == strcmp(semantic, RW_BASE_TEXTURE))
		return ID_BASE_TEXTURE;
	else if(0 == strcmp(semantic, RW_LIGHT_DIRECTION0))
		return ID_LIGHT_DIRECTION0;
	else if(0 == strcmp(semantic, RW_LIGHT_DIRECTION1))
		return ID_LIGHT_DIRECTION1;
	//@{ Jaewon 20041129
	// added "Direction" semantic for fx composer.
	else if(0 == strcmp(semantic, RW_LIGHT_DIRECTION) || 0 == strcmp(semantic, "Direction"))
	//@} Jaewon
		return ID_LIGHT_DIRECTION;
	else if(0 == strcmp(semantic, RW_LIGHT_COLOR0))
		return ID_LIGHT_COLOR0;
	else if(0 == strcmp(semantic, RW_LIGHT_COLOR1))
		return ID_LIGHT_COLOR1;
	else if(0 == strcmp(semantic, RW_LIGHT_COLOR))
		return ID_LIGHT_COLOR;
	else if(0 == strcmp(semantic, RW_AMBIENT))
		return ID_AMBIENT;
	else if(0 == strcmp(semantic, RW_DIFFUSE))
		return ID_DIFFUSE;
	else if(0 == strcmp(semantic, RW_SPECULAR))
		return ID_SPECULAR;
	else if(0 == strcmp(semantic, RW_LIGHT_AMBIENT))
		return ID_LIGHT_AMBIENT;
	else if(0 == strcmp(semantic, RW_FOG))
		return ID_FOG;
	else if(0 == strcmp(semantic, RW_LIGHT_POSITION0))
		return ID_LIGHT_POSITION0;
	else if(0 == strcmp(semantic, RW_LIGHT_PARAMETERS0))
		return ID_LIGHT_PARAMETERS0;
	else if(0 == strcmp(semantic, RW_LIGHT_POSITION1))
		return ID_LIGHT_POSITION1;
	else if(0 == strcmp(semantic, RW_LIGHT_PARAMETERS1))
		return ID_LIGHT_PARAMETERS1;
	//@{ Jaewon 20040910
	else if(0 == strcmp(semantic, RW_TIME))
		return ID_TIME;	
	//@} Jaewon
	//@{ Jaewon 20050419
	// Up to 8 lights
	else if(semantic == strstr(semantic, RW_LIGHT_COLORX))
	{
		RwInt32 i = atoi(semantic+strlen(RW_LIGHT_COLORX));
		if(i>0 && i<8)
			return ID_LIGHT_COLOR0 + i;
	}
	else if(semantic == strstr(semantic, RW_LIGHT_DIRECTIONX))
	{
		RwInt32 i = atoi(semantic+strlen(RW_LIGHT_DIRECTIONX));
		if(i>0 && i<8)
			return ID_LIGHT_DIRECTION0 + i;
	}
	else if(semantic == strstr(semantic, RW_LIGHT_PARAMETERSX))
	{
		RwInt32 i = atoi(semantic+strlen(RW_LIGHT_PARAMETERSX));
		if(i>0 && i<8)
			return ID_LIGHT_PARAMETERS0 + i;
	}
	else if(semantic == strstr(semantic, RW_LIGHT_POSITIONX))
	{
		RwInt32 i = atoi(semantic+strlen(RW_LIGHT_POSITIONX));
		if(i>0 && i<8)
			return ID_LIGHT_POSITION0 + i;
	}
	//@} Jaewon
	
	return -1;
}

/*
 *************************************************************************************************************
 */
static enum SpaceAnnotation getSpaceAnnotation(LPD3DXEFFECT d3dxEffect, D3DXHANDLE handle)
{
	D3DXHANDLE hAnno;
	LPCSTR szSpace;
	hAnno
		= d3dxEffect->lpVtbl->GetAnnotationByName(d3dxEffect, handle, RW_SPACE);
	if(hAnno 
		&& SUCCEEDED(d3dxEffect->lpVtbl->GetString(d3dxEffect, hAnno, &szSpace)))
	{
		if(0 == _stricmp(szSpace, RW_LOCAL_SPACE))
			return ID_LOCAL_SPACE;
		else if(0 == _stricmp(szSpace, RW_VIEW_SPACE))
			return ID_VIEW_SPACE;
		else
			return ID_WORLD_SPACE;
	}
	else
		return ID_WORLD_SPACE;
}

void
EffectBuildParams(DxEffect *effect)
{
    RwUInt32 i;
    D3DXEFFECT_DESC effectDesc;
    RwUInt32 realIndex, vectorIndex, matrix4x4Index, textureIndex;
    RwUInt32 tweakableStart;
	int semanticID;
	HandleFloat *handleFloat;
	HandleVector *handleVector;
	HandleMatrix4x4 *handleMatrix;
	HandleTexture *handleTexture;

    /* Allocates and builds arrays of the parameters from the effect that can be tweaked at runtime by an
       editor */

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);

    my_assert(0 == effect->realCount);
    my_assert(0 == effect->vectorCount);
    my_assert(0 == effect->matrix4x4Count);
    my_assert(0 == effect->textureCount);
    my_assert(0 == effect->tweakableParams.count);

	//@{ Jaewon 20041102
	// lighting optimization
    effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetDesc(effect->pSharedD3dXEffect->d3dxEffect[0], &effectDesc);

    my_assert(effectDesc.Techniques > 0);

    /* first count each type of parameter... */
    for (i=0; i!=effectDesc.Parameters; ++i)
    {
        D3DXPARAMETER_DESC desc;
        D3DXHANDLE handle;
        D3DXHANDLE annot;
		RwBool isTweakable;

        handle = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetParameter(effect->pSharedD3dXEffect->d3dxEffect[0], NULL, i);
        effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetParameterDesc(effect->pSharedD3dXEffect->d3dxEffect[0], handle, &desc);

        annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle, "tweakable");
        if (annot)
        {
            HRESULT hr;

            hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetBool(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &isTweakable);
            my_assert(SUCCEEDED(hr));
        }
		else
			isTweakable = FALSE;

		if(-1 == getRwSemanticID(desc.Semantic))
        {
            switch (desc.Class)
            {
                case D3DXPC_SCALAR:
                    my_assert(D3DXPT_FLOAT == desc.Type);
                    effect->realCount++;
                    /*if (D3DXPT_FLOAT == desc.Type)
                    {
                        ++effect->realCount;
                    }
                    else if (D3DXPT_INT == desc.Type)
                    {
                        ++effect->intCount;
                    }
                    else if (D3DXPT_BOOL == desc.Type)
                    {
                        ++effect->boolCount;
                    }*.
                    /* this will mean even params that are not tweakable could be managed by this plugin.
                       Though the examples viewers will do not anything with this data. D3D will just take the
                       default data from the fx file. If this behaviour is not required then memory can be
                       saved. The only changes should be to effect.c/h by removing the tweakable member from
                       structs Handle* and conditional statements use of the parameter annotation "tweakable" 
                       being equal to true, which is in this fucntion and EffectForAllTweakableParameters */

					if (isTweakable)
					{
						effect->tweakableParams.count++; 
					}
                    break;

                case D3DXPC_VECTOR:
                    my_assert(D3DXPT_FLOAT == desc.Type);
                    ++effect->vectorCount;

					if (isTweakable)
					{
						++effect->tweakableParams.count; 
					}
                    break;

                case D3DXPC_MATRIX_ROWS:
                case D3DXPC_MATRIX_COLUMNS:
                    my_assert(D3DXPT_FLOAT == desc.Type);
                    effect->matrix4x4Count++;

					if (isTweakable)
					{
						++effect->tweakableParams.count; 
					}
                    break;

                case D3DXPC_OBJECT:
                    if (D3DXPT_TEXTURE == desc.Type)
                    {
                        ++effect->textureCount;
						if (isTweakable)
						{
							++effect->tweakableParams.count; 
						}
                    }
                    else
                    {
                        RwChar msg[MAX_MSG_SIZE];

                        _snprintf(msg, MAX_MSG_SIZE, "Parameter: %s ignored by current RW pipeline \
implementation, though not by D3D", desc.Name);
                        _rwDebugSendMessage(rwDEBUGMESSAGE, __FILE__, __LINE__, "EffectBuildParams", msg);
                    }
                    break;

                default:
                    {
                        RwChar msg[MAX_MSG_SIZE];

                        _snprintf(msg, MAX_MSG_SIZE, "Parameter: %s ignored by current RW pipeline \
implementation, though not by D3D", desc.Name);
                        _rwDebugSendMessage(rwDEBUGMESSAGE, __FILE__, __LINE__, "EffectBuildParams", msg);
                    }
                    break;
            }
        }
    }

    /* ...so we can allocate the correct amount...*/
    //@{ Jaewon 20050722
	// Memory pooling for performance
	for(i=0; i<effect->realCount; ++i)
	{
		handleFloat = (HandleFloat *)RwFreeListAlloc(HandleFloatFreeList, MATERIAL_FX_PLUGIN | 
			rwMEMHINTDUR_EVENT);
		my_assert(handleFloat);
		handleFloat->next = effect->real;
		effect->real = handleFloat;
	}

	for(i=0; i<effect->vectorCount; ++i)
	{
		handleVector = (HandleVector *)RwFreeListAlloc(HandleVectorFreeList, MATERIAL_FX_PLUGIN | 
			rwMEMHINTDUR_EVENT);
		my_assert(handleVector);
		handleVector->next = effect->vector;
		effect->vector = handleVector;
	}

	for(i=0; i<effect->matrix4x4Count; ++i)
	{
		handleMatrix = (HandleMatrix4x4 *)RwFreeListAlloc(HandleMatrix4x4FreeList, MATERIAL_FX_PLUGIN | 
			rwMEMHINTDUR_EVENT);
		my_assert(handleMatrix);
		handleMatrix->next = effect->matrix4x4;
		effect->matrix4x4 = handleMatrix;
	}

	for(i=0; i<effect->textureCount; ++i)
	{
		handleTexture = (HandleTexture *)RwFreeListAlloc(HandleTextureFreeList, MATERIAL_FX_PLUGIN | 
			rwMEMHINTDUR_EVENT);
		my_assert(handleTexture);
		handleTexture->next = effect->texture;
		effect->texture = handleTexture;
	}
	//@} Jaewon

    /*effect->tweakableParams.count = effect->realCount + effect->vectorCount + effect->matrix4x4Count;*/
    if (effect->tweakableParams.count > 0)
    {
		//@{ Jaewon 20050722
		// No dynamic allocation
		my_assert(effect->tweakableParams.count <= MAX_TWEAKABLE_PARAM_COUNT);
        memset(effect->tweakableParams.type, 0, sizeof(RwUInt8) * effect->tweakableParams.count);
		//@} Jaewon
    }

    /* ...then fill in each param */
    realIndex = 0;
    vectorIndex = 0;
    matrix4x4Index = 0;
    textureIndex = 0;
    tweakableStart = 0;

    for (i=0; i!=effectDesc.Parameters; ++i)
    {
        D3DXPARAMETER_DESC desc;
        D3DXHANDLE handle[FX_LIGHTING_VARIATION];
        D3DXHANDLE annot;
        RwBool isTweakable = FALSE;
		int j;

		for(j=0; j<FX_LIGHTING_VARIATION; ++j)
	        handle[j] = effect->pSharedD3dXEffect->d3dxEffect[j]->lpVtbl->GetParameter(effect->pSharedD3dXEffect->d3dxEffect[j], NULL, i);
        effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetParameterDesc(effect->pSharedD3dXEffect->d3dxEffect[0], handle[0], &desc);

        annot = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetAnnotationByName(effect->pSharedD3dXEffect->d3dxEffect[0], handle[0], "tweakable");
        if (annot)
        {        
            HRESULT hr;
            hr = effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetBool(effect->pSharedD3dXEffect->d3dxEffect[0], annot, &isTweakable);
            my_assert(SUCCEEDED(hr));
        }

		semanticID = getRwSemanticID(desc.Semantic);
		if(semanticID != -1)
        {
            my_assert(semanticID < ID_TOTAL);
            strncpy(effect->rw[semanticID].name, desc.Name, FX_HANDLE_NAME_LENGTH);
			for(j=0; j<FX_LIGHTING_VARIATION; ++j)
	            effect->rw[semanticID].handle[j] = handle[j];
			
			// get the 'space' annotation.
			if(semanticID == ID_CAMERA_POSITION)
			{
				effect->cameraPositionSpace = 
					getSpaceAnnotation(effect->pSharedD3dXEffect->d3dxEffect[0], effect->rw[semanticID].handle[0]);
			}
			else if(semanticID == ID_CAMERA_DIRECTION)
			{
				effect->cameraDirectionSpace = 
					getSpaceAnnotation(effect->pSharedD3dXEffect->d3dxEffect[0], effect->rw[semanticID].handle[0]);
			}
			else if(semanticID == ID_LIGHT_DIRECTION)
			{
				effect->lightDirectionSpace = 
					getSpaceAnnotation(effect->pSharedD3dXEffect->d3dxEffect[0], effect->rw[semanticID].handle[0]);
			}
			//@{ Jaewon 20050419
			// Up to 8 lights
			else if(ID_LIGHT_POSITION0 <= semanticID && semanticID <= ID_LIGHT_POSITION7)
			{
				effect->localLightPositionSpace[semanticID-ID_LIGHT_POSITION0] = 
					getSpaceAnnotation(effect->pSharedD3dXEffect->d3dxEffect[0], effect->rw[semanticID].handle[0]);
			}
			else if(ID_LIGHT_DIRECTION0 <= semanticID && semanticID <= ID_LIGHT_DIRECTION7)
			{
				effect->localLightDirectionSpace[semanticID-ID_LIGHT_DIRECTION0] = 
					getSpaceAnnotation(effect->pSharedD3dXEffect->d3dxEffect[0], effect->rw[semanticID].handle[0]);
			}
			//@} Jaewon
        }
        else
        {
            switch (desc.Class)
            {
                case D3DXPC_SCALAR:
                    //@{ Jaewon 20050722
					// effect->real[realIndex]. => handleFloat->
					my_assert(realIndex < effect->realCount);
					strncpy(handleFloat->name, desc.Name, FX_HANDLE_NAME_LENGTH);
					for(j=0; j<FX_LIGHTING_VARIATION; ++j)
						handleFloat->handle[j] = handle[j];
					handleFloat->tweakable = isTweakable;
					effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetFloat(effect->pSharedD3dXEffect->d3dxEffect[0], handle[0],
						&handleFloat->data);

					if (handleFloat->tweakable)
					{
						effect->tweakableParams.type[tweakableStart++] = EFFECT_TWEAKABLE_PARAM_FLOAT;
					}

					realIndex++;
					handleFloat = handleFloat->next;
					//@} Jaewon
                    break;

                case D3DXPC_VECTOR:
                    //@{ Jaewon 20050722
					// effect->vector[vectorIndex]. => handleVector->
					my_assert(vectorIndex < effect->vectorCount);
					strncpy(handleVector->name, desc.Name, FX_HANDLE_NAME_LENGTH);
					for(j=0; j<FX_LIGHTING_VARIATION; ++j)
						handleVector->handle[j] = handle[j];
					handleVector->tweakable = isTweakable;
					effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetVector(effect->pSharedD3dXEffect->d3dxEffect[0], handle[0],
						&handleVector->data);

					if (handleVector->tweakable)
					{
						effect->tweakableParams.type[tweakableStart++] = EFFECT_TWEAKABLE_PARAM_VECTOR;
					}

					vectorIndex++;
					handleVector = handleVector->next;
					//@} Jaewon
                    break;

                case D3DXPC_MATRIX_ROWS:
                case D3DXPC_MATRIX_COLUMNS:
                    //@{ Jaewon 20050722
					// effect->matrix4x4[matrix4x4Index]. => handleMatrix->
					my_assert(matrix4x4Index < effect->matrix4x4Count);
					strncpy(handleMatrix->name, desc.Name, FX_HANDLE_NAME_LENGTH);
					for(j=0; j<FX_LIGHTING_VARIATION; ++j)
						handleMatrix->handle[j] = handle[j];
					handleMatrix->tweakable = isTweakable;

					if (EffectInitMatricesToIdentity)
					{
						D3DXMATRIX matrix;

						D3DXMatrixIdentity(&matrix);

						for(j=0; j<FX_LIGHTING_VARIATION; ++j)
							effect->pSharedD3dXEffect->d3dxEffect[j]->lpVtbl->SetMatrix(effect->pSharedD3dXEffect->d3dxEffect[j], handle[j], &matrix);
					}

					effect->pSharedD3dXEffect->d3dxEffect[0]->lpVtbl->GetMatrix(effect->pSharedD3dXEffect->d3dxEffect[0], handle[0],
						&handleMatrix->data);

					if (handleMatrix->tweakable)
					{
						effect->tweakableParams.type[tweakableStart++] = EFFECT_TWEAKABLE_PARAM_MATRIX4X4;
					}

					++matrix4x4Index;
					handleMatrix = handleMatrix->next;
					//@} Jaewon
                    break;

                case D3DXPC_OBJECT:
                    if (D3DXPT_TEXTURE == desc.Type)
                    {
                       //@{ Jaewon 20050722
						// effect->texture[textureIndex]. => handleTexture->
						RwTexture *texture;

						my_assert(textureIndex < effect->textureCount);

						texture = EffectRwTextureReadFromFx(effect->pSharedD3dXEffect->d3dxEffect[0], handle[0]);
						//@{ Jaewon 20041022
						// this setting can be invalid in a multithreaded situation.
						/*if (texture)
						{
						effect->pSharedD3dXEffect->d3dxEffect->lpVtbl->SetTexture(effect->pSharedD3dXEffect->d3dxEffect, handle,
						D3DTextureFromRwTexture(texture));
						}*/
						//@} Jaewon

						strncpy(handleTexture->name, desc.Name, FX_HANDLE_NAME_LENGTH);
						for(j=0; j<FX_LIGHTING_VARIATION; ++j)
							handleTexture->handle[j] = handle[j];
						handleTexture->tweakable = isTweakable;
						handleTexture->data = texture;

						if (handleTexture->tweakable)
						{
							effect->tweakableParams.type[tweakableStart++] = EFFECT_TWEAKABLE_PARAM_TEXTURE;
						}
						++textureIndex;
						handleTexture = handleTexture->next;
						//@} Jaewon
                    }
                    break;

                default:
                    break;
            }
        }
    }
	//@} Jaewon

    /* check we have written all that we should have */
    my_assert(realIndex == effect->realCount);
    my_assert(vectorIndex == effect->vectorCount);
    my_assert(matrix4x4Index == effect->matrix4x4Count);
    my_assert(textureIndex == effect->textureCount);
    my_assert(tweakableStart == effect->tweakableParams.count);

    return;
}


/*
 *************************************************************************************************************
 */
/*static void
EffectListRemove(DxEffect *effect)
{
    rpDxEffect *current;
    rpDxEffect *previous;

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);

    current = DxEffects;
    previous = NULL;

    while (current)
    {
        if (effect == current->effect)
        {
            break;
        }

        previous = current;
        current = current->next;
    }

    if (current)
    {
        if (previous)
        {
            previous->next = current->next;
        }
        else
        {
            my_assert(DxEffects == current);
            DxEffects = current->next;
        }

        RwFreeListFree(DxEffectFreelist, current);
    }

    return;
}

static void
EffectListAdd(DxEffect *effect)
{
    rpDxEffect *entry;

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(DxEffectFreelist);

    entry = (rpDxEffect *)RwFreeListAlloc(DxEffectFreelist, MATERIAL_FX_PLUGIN | rwMEMHINTDUR_EVENT);
    my_assert(entry);
    entry->effect = effect;
    entry->next = DxEffects;
    DxEffects = entry;

    return;
}

static DxEffect *
ForAllEffectsInFreelist(EffectCallBack callback, void *data)
{
    rpDxEffect *current;

    my_assert(RpMaterialD3DFxModule.numInstances > 0);

    my_assert(callback);

    current = DxEffects;
    while (current)
    {
        rpDxEffect *next;
        next = current->next;

        if (NULL == callback(current->effect, data))
        {
            return current->effect;
        }
        current = next;
    }

    return NULL;
}
*/

/*
 *************************************************************************************************************
 */
static void
D3dXEffectListRemove(SharedD3dXEffectPtr *effect)
{
    rpD3dXEffect *current;
    rpD3dXEffect *previous;

	//@{ Jaewon 20041025
	//EnterCriticalSection(&_criticalSection);

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);

    current = D3dXEffects;
    previous = NULL;

    while (current)
    {
        if (effect == current->pD3dXEffect)
        {
            break;
        }

        previous = current;
        current = current->next;
    }

    if (current)
    {
        if (previous)
        {
            previous->next = current->next;
        }
        else
        {
            my_assert(D3dXEffects == current);
            D3dXEffects = current->next;
        }

		//@{ Jaewon 20041022
		//@{ Jaewon 20041024
		// its reference count should be zero, so no real d3d release occurs.
		current->pEffect->pSharedD3dXEffect->refCount = 0;
		//@} Jaewon
		EffectRelease(current->pEffect);
		//@} Jaewon
        RwFreeListFree(D3dXEffectFreelist, current);
    }

	//LeaveCriticalSection(&_criticalSection);
	//@} Jaewon

    return;
}

//@{ Jaewon 20041022
// DxEffect argument added
static void
D3dXEffectListAdd(SharedD3dXEffectPtr * effect, DxEffect * instance)
{
    rpD3dXEffect *entry;

	//@{ Jaewon 20041025
	//EnterCriticalSection(&_criticalSection);

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);
    my_assert(D3dXEffectFreelist);
	my_assert(instance);

    entry = (rpD3dXEffect *)RwFreeListAlloc(D3dXEffectFreelist, MATERIAL_FX_PLUGIN | rwMEMHINTDUR_EVENT);
    my_assert(entry);
    entry->pD3dXEffect = effect;

	//@{ Jaewon 20050722
	// Memory pooling for performance
	entry->pEffect = (DxEffect *)RwFreeListAlloc(DxEffectFreeList, MATERIAL_FX_PLUGIN | rwMEMHINTDUR_EVENT);
	//@} Jaewon
    my_assert(entry->pEffect);
    EffectInitEffect(entry->pEffect);
	EffectCopyParams(entry->pEffect, instance);
	entry->pEffect->pSharedD3dXEffect = effect;

    entry->next = D3dXEffects;
    D3dXEffects = entry;

	//LeaveCriticalSection(&_criticalSection);
	//@} Jaewon

    return;
}
//@} Jaewon

//@{ Jaewon 20040917
// no static
//@{ Jaewon 20041022
// SharedD3dXEffectPtr -> DxEffect
DxEffect *
//@} Jaewon
ForAllD3dXEffectsInFreelist(D3dXEffectCallBack callback, void *data)
//@} Jaewon
{
    rpD3dXEffect *current;

	//@{ Jaewon 20041025
	//EnterCriticalSection(&_criticalSection);

    my_assert(RpMaterialD3DFxModule.numInstances > 0);

    my_assert(callback);

    current = D3dXEffects;
    while (current)
    {
        rpD3dXEffect *next;
        next = current->next;

        if (NULL == callback(current->pD3dXEffect, data))
        {
			//LeaveCriticalSection(&_criticalSection);
			
			//@{ Jaewon 20041022
			// current->pD3dXEffect -> current->pEffect
            return current->pEffect;
			//@} Jaewon
        }
        current = next;
    }

	//LeaveCriticalSection(&_criticalSection);
	//@} Jaewon

    return NULL;
}


/*
 *************************************************************************************************************
 */
static SharedD3dXEffectPtr *
EffectOnLostDevice(SharedD3dXEffectPtr *effect, void *data __RWUNUSED__)
{
	int i;

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);

	//@{ Jaewon 20041102
	// lighting optimization
	for(i=0; i<FX_LIGHTING_VARIATION; ++i)
	{
		if (effect->d3dxEffect[i])
		{
			effect->d3dxEffect[i]->lpVtbl->OnLostDevice(effect->d3dxEffect[i]);
		}
	}
	//@} Jaewon

    return effect;
}

static SharedD3dXEffectPtr *
EffectOnResetDevice(SharedD3dXEffectPtr *effect, void *data __RWUNUSED__)
{
	int i;

    my_assert(RpMaterialD3DFxModule.numInstances > 0);
    my_assert(effect);

	//@{ Jaewon 20041102
	// lighting optimization
	for(i=0; i<FX_LIGHTING_VARIATION; ++i)
	{
		if (effect->d3dxEffect[i])
		{
			effect->d3dxEffect[i]->lpVtbl->OnResetDevice(effect->d3dxEffect[i]);
		}
	}
	//@} Jaewon
    
	return effect;
}


/*
 *************************************************************************************************************
 */
static void
EffectDeviceRestoreCallBack(void)
{
    my_assert(RpMaterialD3DFxModule.numInstances > 0);

    if (OldDeviceRestoreCallBack)
    {
        OldDeviceRestoreCallBack();
    }

    ForAllD3dXEffectsInFreelist(EffectOnResetDevice, NULL);
    return;
}

static void
EffectDeviceReleaseCallBack(void)
{
    my_assert(RpMaterialD3DFxModule.numInstances > 0);

    if (OldDeviceReleaseCallBack)
    {
        OldDeviceReleaseCallBack();
    }

    ForAllD3dXEffectsInFreelist(EffectOnLostDevice, NULL);
    return;
}


/*
 *************************************************************************************************************
 */
void
EffectSetDeviceLostRestoreCallBacks(void)
{
    /* Enable the callbacks to handle releasing and restoring all ID3DXEffect when the current D3D device is
       lost */
    OldDeviceRestoreCallBack = _rwD3D9DeviceGetRestoreCallback();
    _rwD3D9DeviceSetRestoreCallback(EffectDeviceRestoreCallBack);

    OldDeviceReleaseCallBack = _rwD3D9DeviceGetReleaseCallback();
    _rwD3D9DeviceSetReleaseCallback(EffectDeviceReleaseCallBack);

    return;
}

void
EffectResetDeviceLostRestoreCallBacks(void)
{
    /* Restore the old callbacks for handling releasing and restore surfaces when the current D3D device is
       lost. Should only be called when shutting down the matd3dfx plugin */

    _rwD3D9DeviceSetRestoreCallback(OldDeviceRestoreCallBack);
    _rwD3D9DeviceSetReleaseCallback(OldDeviceReleaseCallBack);
    return;
}


/*
 *************************************************************************************************************
 */
void
EffectCreateEffectFreelist(void)
{
    /* Allocates the freelist which stores references to all created DxEffects */

/*    DxEffectFreelist = RwFreeListCreateAndPreallocateSpace(sizeof(rpDxEffect), 128, 16, 1, NULL,
        MATERIAL_FX_PLUGIN | rwMEMHINTDUR_GLOBAL);
    my_assert(DxEffectFreelist);
    DxEffects = NULL;
*/
	D3dXEffectFreelist = RwFreeListCreateAndPreallocateSpace(sizeof(rpD3dXEffect), 128, 16, 1, NULL,
        MATERIAL_FX_PLUGIN | rwMEMHINTDUR_GLOBAL);
	my_assert(D3dXEffectFreelist);
	D3dXEffects = NULL;

	//@{ Jaewon 20050722
	// Memory pooling for performance
	DxEffectFreeList = RwFreeListCreateAndPreallocateSpace(sizeof(DxEffect), 256, 16, 4, NULL, 
		MATERIAL_FX_PLUGIN | rwMEMHINTDUR_GLOBAL);
	my_assert(DxEffectFreeList);
	HandleFloatFreeList = RwFreeListCreateAndPreallocateSpace(sizeof(HandleFloat), 512, 16, 10, NULL, 
		MATERIAL_FX_PLUGIN | rwMEMHINTDUR_GLOBAL);
	my_assert(HandleFloatFreeList);
	HandleVectorFreeList = RwFreeListCreateAndPreallocateSpace(sizeof(HandleVector), 512, 16, 10, NULL, 
		MATERIAL_FX_PLUGIN | rwMEMHINTDUR_GLOBAL);
	my_assert(HandleVectorFreeList);
	HandleMatrix4x4FreeList = RwFreeListCreateAndPreallocateSpace(sizeof(HandleMatrix4x4), 512, 16, 5, NULL, 
		MATERIAL_FX_PLUGIN | rwMEMHINTDUR_GLOBAL);
	my_assert(HandleMatrix4x4FreeList);
	HandleTextureFreeList = RwFreeListCreateAndPreallocateSpace(sizeof(HandleTexture), 512, 16, 10, NULL, 
		MATERIAL_FX_PLUGIN | rwMEMHINTDUR_GLOBAL);
	my_assert(HandleTextureFreeList);
	//@} Jaewon
    
	return;
}


/*
 *************************************************************************************************************
 */
static SharedD3dXEffectPtr *
EffectReleaseRemainingEffects(SharedD3dXEffectPtr *effect, void *data __RWUNUSED__)
{
    RwChar msg[MAX_MSG_SIZE];
	int i;

    my_assert(effect);
    
    _snprintf(msg, MAX_MSG_SIZE, "Shutting down RWG but the effect 0x%p still has %d references to it. \
Trying to release effect anyway.", (void *)effect, effect->refCount);

    _rwDebugSendMessage(rwDEBUGMESSAGE, __FILE__, __LINE__, "EffectReleaseRemainingEffects", msg);

	//@{ Jaewon 20041102
	// lighting optimization
	for(i=0; i<FX_LIGHTING_VARIATION; ++i)
	{
		my_assert(effect->d3dxEffect[i]);
		effect->d3dxEffect[i]->lpVtbl->Release(effect->d3dxEffect[i]);
	}
	//@} Jaewon

    D3dXEffectListRemove(effect);
    RwFree(effect);

    return effect;  /* have to return a non-null value */
}

void
EffectDestroyEffectFreelist(void)
{
    /* Frees the freelist which stores references to all created DxEffects. Should only be called when
       shutting down the matd3dfx plugin */

    ForAllD3dXEffectsInFreelist(EffectReleaseRemainingEffects, NULL);

/*    if (DxEffectFreelist)
    {
        RwFreeListDestroy(DxEffectFreelist);
        DxEffectFreelist = NULL;
    }

    DxEffects = NULL;
*/
	if(D3dXEffectFreelist)
	{
		RwFreeListDestroy(D3dXEffectFreelist);
		D3dXEffectFreelist = NULL;
	}

	D3dXEffects = NULL;
	
	//@{ Jaewon 20050722
	// Memory pooling for performance
	if(DxEffectFreeList)
	{
		RwFreeListDestroy(DxEffectFreeList);
		DxEffectFreeList = NULL;
	}
	if(HandleFloatFreeList)
	{
		RwFreeListDestroy(HandleFloatFreeList);
		HandleFloatFreeList = NULL;
	}
	if(HandleVectorFreeList)
	{
		RwFreeListDestroy(HandleVectorFreeList);
		HandleVectorFreeList = NULL;
	}
	if(HandleMatrix4x4FreeList)
	{
		RwFreeListDestroy(HandleMatrix4x4FreeList);
		HandleMatrix4x4FreeList = NULL;
	}
	if(HandleTextureFreeList)
	{
		RwFreeListDestroy(HandleTextureFreeList);
		HandleTextureFreeList = NULL;
	}
	//@} Jaewon

    return;
}


/*
 *************************************************************************************************************
 */
void
EffectSetD3DDevice(void)
{
    /* Set the D3D device that should be used for all effects */
    EffectD3DDevice = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();
    my_assert(EffectD3DDevice);
    return;
}

void
EffectNullD3DDevice(void)
{
    /* Release the D3D device that should be used for all effects. Should only be called when shutting down
       the matd3dfx plugin */
    EffectD3DDevice = NULL;
    return;
}

/*
 *************************************************************************************************************
 */

//@{ Jaewon 20041007
// for material copying
DxEffect *
EffectCopy(DxEffect *effect)
{
	DxEffect *result = NULL;
	unsigned int i;
	RwBool success;
	HandleFloat *handleFloat;
	HandleVector *handleVector;
	HandleMatrix4x4 *handleMatrix;
	HandleTexture *handleTexture;

	//@{ Jaewon 20041025
	//EnterCriticalSection(&_criticalSection);

	my_assert(effect);
	my_assert(effect->pSharedD3dXEffect);

	//@{ 2006/11/16 burumal
	if ( effect == NULL || effect->pSharedD3dXEffect == NULL )
		return NULL;
	//@}

	result = EffectLoad(effect->pSharedD3dXEffect->name,
						effect->pSharedD3dXEffect->nWeights);
	my_assert(result);

	// copy parameters.
	//@{ Jaewon 20050722
	// For memory pooling
	// float
	my_assert(effect->realCount == result->realCount);
	handleFloat = effect->real;
	for(i=0; i<effect->realCount; ++i)
	{
		success = EffectSetFloat(result, handleFloat->name, handleFloat->data);
		my_assert(success);
		handleFloat = handleFloat->next;
	}

	// vector
	my_assert(effect->vectorCount == result->vectorCount);
	handleVector = effect->vector;
	for(i=0; i<effect->vectorCount; ++i)
	{
		success = EffectSetVector(result, handleVector->name, &(handleVector->data));
		my_assert(success);
		handleVector = handleVector->next;
	}

	// matrix
	my_assert(effect->matrix4x4Count == result->matrix4x4Count);
	handleMatrix = effect->matrix4x4;
	for(i=0; i<effect->matrix4x4Count; ++i)
	{
		success = EffectSetMatrix4x4(result, handleMatrix->name, &(handleMatrix->data));
		my_assert(success);
		handleMatrix = handleMatrix->next;
	}

	// texture
	my_assert(effect->textureCount == result->textureCount);
	handleTexture = effect->texture;
	for(i=0; i<effect->textureCount; ++i)
	{
		success = EffectSetTexture(result, handleTexture->name, handleTexture->data);
		my_assert(success);
		handleTexture = handleTexture->next;
	}
	//@} Jaewon

	//LeaveCriticalSection(&_criticalSection);
	//@} Jaewon

	return result;
}
//@} Jaewon

