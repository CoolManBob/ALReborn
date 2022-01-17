#include "rwcore.h"
#include "rpworld.h"

#include "mtexture.h"
#include "mtpipe.h"
#include "mtstream.h"

#include "acurpmtexture.h"

#include <d3d9.h>

RwInt32 MTextureGlobalDataOffset = -1;
RwInt32 MTextureMaterialDataOffset = -1;
RwInt32 MTextureAtomicDataOffset = -1;
RwInt32 MTextureWorldSectorDataOffset = -1;

RwCamera*	RPMTEX_WorldCamera		= NULL;
RwMatrix*	RPMTEX_invShadowMatrix	= NULL;

RwTexture*	RPMTEX_ShadowTexture	= NULL;

RwUInt32	RPMTEX_FadeDist			= 15;				// 현재 DISABLE

//RwMatrix*	RPMTEX_LightCameraViewMatrix = NULL;
//RwMatrix*	RPMTEX_LightCameraProjMatrix = NULL;

/*
 *****************************************************************************
 */
static void *
MTextureOpen(void *instance,
							 RwInt32 offset __RWUNUSED__,
							 RwInt32 size __RWUNUSED__)
{
	/*
	 * Initialize MTexture Pipeline
	 */
	if (MTexturePipeOpen() == FALSE)
	{
		return NULL;
	}

	return instance;
}


static void *
MTextureClose(void *instance,
							RwInt32 offset __RWUNUSED__,
							RwInt32 size __RWUNUSED__)
{
	/*
	 * Destroy MTexture Pipeline
	 */
	MTexturePipeClose();

	return instance;
}


/*
 *****************************************************************************
 */
static void *
MTextureMaterialDataConstructor(void *material,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	/* Set texture as NULL */
	if (MTextureMaterialDataOffset > 0)
	{
		MTextureMaterialData *	pData = MTEXTUREMATERIAL(material);

		memset(pData, 0, sizeof(MTextureMaterialData));
	}

	return material;
}


static void *
MTextureMaterialDataDestructor(void *material,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	/* Destroy texture */
	if (MTextureMaterialDataOffset > 0)
	{
		RwInt16 index;
		MTextureMaterialData *	pData = MTEXTUREMATERIAL(material);

		for (index = 0; index < pData->textureNumber; index++)
		{
			if (pData->texture[index])
			{
				RwTextureDestroy(pData->texture[index]);
				pData->texture[index] = NULL;
			}
		}

		pData->textureNumber = 0;
	}

	return material;
}


static void *
MTextureMaterialDataCopier(void *dstmaterial, const void *srcmaterial,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{
	/*
	 *Copy texture and add reference counter
	 */
	if (MTextureMaterialDataOffset > 0)
	{
		RwInt16 index;
		MTextureMaterialData *	pSrcData = MTEXTUREMATERIAL(srcmaterial);
		MTextureMaterialData *	pDstData = MTEXTUREMATERIAL(dstmaterial);

		for (index = 0; index < pSrcData->textureNumber; index++)
		{
			pDstData->texture[index] = pSrcData->texture[index];

			if (pSrcData->texture[index])
			{
				RwTextureAddRef(pSrcData->texture[index]);
			}
		}

		pDstData->textureNumber = pSrcData->textureNumber;
	}

	return dstmaterial;
}


/*
 *****************************************************************************
 */
static void *
MTextureAtomicDataConstructor(void *atomic,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	if (MTextureAtomicDataOffset > 0)
	{
		MTEXTUREATOMICDATA(atomic, enabled) = FALSE;
	}

	return atomic;
}


static void *
MTextureAtomicDataDestructor(void *atomic,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	return atomic;
}


static void *
MTextureAtomicDataCopier(void *dstatomic, const void *srcatomic,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{
	/*
	 * If source atomic is enabled, enable effect of destination atomic
	 */
	if (MTextureAtomicDataOffset > 0)
	{
		if (MTEXTUREATOMICDATA(srcatomic, enabled))
		{
			RpMTextureAtomicEnableEffect(dstatomic);
		}
	}

	return dstatomic;
}


/*
 *****************************************************************************
 */
static void *
MTextureWorldSectorDataConstructor(void *worldSector,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	if (MTextureWorldSectorDataOffset > 0)
	{
		MTEXTUREWORLDSECTORDATA(worldSector, enabled) = FALSE;
	}

	return worldSector;
}


static void *
MTextureWorldSectorDataDestructor(void *worldSector,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	return worldSector;
}


static void *
MTextureWorldSectorDataCopier(void *dstworldSector, const void *srcworldSector,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{
	/*
	 * If source worldsector is enabled, enable effect of destination worldsector
	 */
	if (MTextureWorldSectorDataOffset > 0)
	{
		if (MTEXTUREWORLDSECTORDATA(srcworldSector, enabled))
		{
			RpMTextureWorldSectorEnableEffect(dstworldSector);
		}
	}

	return dstworldSector;
}



/*
 *****************************************************************************
 */
RwBool
RpMTexturePluginAttach(void)
{
	RwInt32 offset;

	/*
	 * Register global space...
	 */
	MTextureGlobalDataOffset = RwEngineRegisterPlugin(0,
										  rwID_MTEXTURE,
										  MTextureOpen,
										  MTextureClose);

	if (MTextureGlobalDataOffset < 0)
	{
		return FALSE;
	}

	/*
	 * Register material extension space...
	 */
	MTextureMaterialDataOffset = RpMaterialRegisterPlugin(sizeof(MTextureMaterialData),
										rwID_MTEXTURE,
										MTextureMaterialDataConstructor,
										MTextureMaterialDataDestructor,
										MTextureMaterialDataCopier);

	if (MTextureMaterialDataOffset < 0)
	{
		return FALSE;
	}

	/*
	 * Register binary stream functionality for extension data...
	 */
	offset = -1;
	offset = RpMaterialRegisterPluginStream(
										 rwID_MTEXTURE,
										 MTextureMaterialDataReadStream,
										 MTextureMaterialDataWriteStream,
										 MTextureMaterialDataGetStreamSize);

	if (offset != MTextureMaterialDataOffset)
	{
		return FALSE;
	}

	/*
	 * Register atomic extension space...
	 */
	MTextureAtomicDataOffset = RpAtomicRegisterPlugin(sizeof(MTextureAtomicData),
										rwID_MTEXTURE,
										MTextureAtomicDataConstructor,
										MTextureAtomicDataDestructor,
										MTextureAtomicDataCopier);

	if (MTextureAtomicDataOffset < 0)
	{
		return FALSE;
	}

	/*
	 * Register binary stream functionality for extension data...
	 */
	offset = -1;
	offset = RpAtomicRegisterPluginStream(
										 rwID_MTEXTURE,
										 MTextureAtomicDataReadStream,
										 MTextureAtomicDataWriteStream,
										 MTextureAtomicDataGetStreamSize);

	if (offset != MTextureAtomicDataOffset)
	{
		return FALSE;
	}

	/*
	 * Register worldsector extension space...
	 */
	MTextureWorldSectorDataOffset = RpWorldSectorRegisterPlugin(sizeof(MTextureWorldSectorData),
										rwID_MTEXTURE,
										MTextureWorldSectorDataConstructor,
										MTextureWorldSectorDataDestructor,
										MTextureWorldSectorDataCopier);

	if (MTextureWorldSectorDataOffset < 0)
	{
		return FALSE;
	}

	/*
	 * Register binary stream functionality for extension data...
	 */
	offset = -1;
	offset = RpWorldSectorRegisterPluginStream(
										 rwID_MTEXTURE,
										 MTextureWorldSectorDataReadStream,
										 MTextureWorldSectorDataWriteStream,
										 MTextureWorldSectorDataGetStreamSize);

	if (offset != MTextureWorldSectorDataOffset)
	{
		return FALSE;
	}

	return TRUE;
}


/*
 *****************************************************************************
 */
RwTexture *
RpMTextureMaterialGetTexture(RpMaterial *material, RwInt16 index, RpMTextureType *type)
{
	if (MTextureMaterialDataOffset > 0 && index >= 0 && index < MTEXTURE_MAX_TEXTURES)
	{
		if (type)
			*type = MTEXTUREMATERIALDATA(material, textureType[index]);

		return MTEXTUREMATERIALDATA(material, texture[index]);
	}

	return NULL;
}


/*
 *****************************************************************************
 */
RpMaterial *
RpMTextureMaterialSetTexture(RpMaterial *material, RwInt16 index, RwTexture *texture, RpMTextureType type)
{
	if (MTextureMaterialDataOffset > 0 && index >= 0 && index < MTEXTURE_MAX_TEXTURES)
	{
		RwTexture **oldTexture;

		/*
		 * Destroy Old Texture
		 */
		oldTexture = &MTEXTUREMATERIALDATA(material, texture[index]);
		if (*oldTexture)
		{
			RwTextureDestroy(*oldTexture);
			*oldTexture = NULL;
		}

		/*
		 * Set New Texture
		 */
		*oldTexture = texture;
		if (texture)
		{
			if (MTEXTUREMATERIALDATA(material, textureNumber) <= index)
				MTEXTUREMATERIALDATA(material, textureNumber) = index + 1;

			MTEXTUREMATERIALDATA(material, textureType[index]) = type;

			RwTextureAddRef(texture);
		}
		else
			MTEXTUREMATERIALDATA(material, textureType[index]) = rpMTEXTURE_TYPE_NONE;

		return material;
	}

	return NULL;
}


/*
 *****************************************************************************
 */
RpAtomic *
RpMTextureAtomicEnableEffect(RpAtomic *atomic)
{
	if (MTextureAtomicDataOffset > 0)
	{
		/*
		 * Set Pipeline && Enable Flag
		 */
		if (MTEXTUREATOMICDATA(atomic, enabled) == FALSE)
		{
			if (!MTextureAtomicSetPipeline(atomic))
			{
				return NULL;
			}

			MTEXTUREATOMICDATA(atomic, enabled) = TRUE;
		}

		return atomic;
	}

	return NULL;
}


static RpAtomic *__EnableEffectCallback(RpAtomic *pstAtomic, PVOID pvData)
{
	RpMTextureAtomicEnableEffect( pstAtomic );

	return pstAtomic;
}

// 마고자 (2004-04-20 오후 6:43:48) : 클럼프에 적용함..
RpClump *
RpMTextureClumpEnableEffect(RpClump *pClump)
{
	if( pClump )
	{
		RpClumpForAllAtomics( pClump ,  __EnableEffectCallback , NULL );	
	}
	return pClump;
}


/*
 *****************************************************************************
 */
RpWorldSector *
RpMTextureWorldSectorEnableEffect(RpWorldSector *worldSector)
{
	if (MTextureWorldSectorDataOffset > 0)
	{
		/*
		 * Set Pipeline && Enable Flag
		 */
		if (MTEXTUREWORLDSECTORDATA(worldSector, enabled) == FALSE)
		{
			if (!MTextureWorldSectorSetPipeline(worldSector))
			{
				return NULL;
			}

			MTEXTUREWORLDSECTORDATA(worldSector, enabled) = TRUE;
		}

		return worldSector;
	}

	return NULL;
}

void RpMTextureSetCamera(RwCamera*	camera)
{
	RPMTEX_WorldCamera		= camera;
}

void RpMTextureSetInvSMatrix(RwMatrix*	matrix)
{
	RPMTEX_invShadowMatrix	= matrix;
}

void RpMTextureSetShadowTexture(RwTexture*	tex)
{
	RPMTEX_ShadowTexture = tex;
}

void RpMTextureSetFadeSectorDist( RwInt32 dist )
{
	RPMTEX_FadeDist = dist;
}

//void RpMTextureSetLightViewMatrix(RwMatrix*	matrix)
//{
//	RPMTEX_LightCameraViewMatrix = matrix;
//}

//void RpMTextureSetLightProjMatrix(RwMatrix*	matrix)
//{
//	RPMTEX_LightCameraProjMatrix = matrix;
//}
