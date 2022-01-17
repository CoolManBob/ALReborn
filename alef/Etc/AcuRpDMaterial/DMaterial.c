#include "rwcore.h"
#include "rpworld.h"

#include "dmaterial.h"
#include "dmstream.h"
#include "dmtexture.h"

#include "acurpdmaterial.h"
#include "acurpmtexture.h"

#include <d3d8.h>

RwInt32 DMaterialGlobalDataOffset = -1;
RwInt32 DMaterialMaterialDataOffset = -1;
RwInt32 DMaterialAtomicDataOffset = -1;
RwInt32 DMaterialWorldDataOffset = -1;
RwInt32 DMaterialWorldSectorDataOffset = -1;

/*
 *****************************************************************************
 */
static void *
DMaterialOpen(void *instance,
							 RwInt32 offset __RWUNUSED__,
							 RwInt32 size __RWUNUSED__)
{
	return instance;
}


static void *
DMaterialClose(void *instance,
							RwInt32 offset __RWUNUSED__,
							RwInt32 size __RWUNUSED__)
{
	return instance;
}


/*
 *****************************************************************************
 */
static void *
DMaterialMaterialDataConstructor(void *material,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	/*
	 * Set initial value
	 */
	if (DMaterialMaterialDataOffset > 0 )
	{
		DMATERIALMATERIALDATA(material, refCount) = 0;
		DMATERIALMATERIALDATA(material, texture1Name)[0] = 0;
		DMATERIALMATERIALDATA(material, texture1Mask)[0] = 0;
		DMATERIALMATERIALDATA(material, texture2Name)[0] = 0;
		DMATERIALMATERIALDATA(material, texture2Mask)[0] = 0;
	}

	return material;
}


static void *
DMaterialMaterialDataDestructor(void *material,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	return material;
}


static void *
DMaterialMaterialDataCopier(void *dstmaterial, const void *srcmaterial,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{
	/*
	 * Copy only texture names
	 */
	if (DMaterialMaterialDataOffset > 0 )
	{
		rwstrncpy(DMATERIALMATERIALDATA(dstmaterial, texture1Name), DMATERIALMATERIALDATACONST(srcmaterial, texture1Name), rwTEXTUREBASENAMELENGTH);
		rwstrncpy(DMATERIALMATERIALDATA(dstmaterial, texture1Mask), DMATERIALMATERIALDATACONST(srcmaterial, texture1Mask), rwTEXTUREBASENAMELENGTH);
		rwstrncpy(DMATERIALMATERIALDATA(dstmaterial, texture2Name), DMATERIALMATERIALDATACONST(srcmaterial, texture2Name), rwTEXTUREBASENAMELENGTH);
		rwstrncpy(DMATERIALMATERIALDATA(dstmaterial, texture2Mask), DMATERIALMATERIALDATACONST(srcmaterial, texture2Mask), rwTEXTUREBASENAMELENGTH);
	}

	return dstmaterial;
}


/*
 *****************************************************************************
 */
static void *
DMaterialAtomicDataConstructor(void *atomic,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	if (DMaterialAtomicDataOffset > 0 )
	{
		DMATERIALATOMICDATA(atomic, enabled) = FALSE;
		DMATERIALATOMICDATA(atomic, OldCallBackRender) = NULL;
		DMATERIALATOMICDATA(atomic, loaded) = FALSE;
	}

	return atomic;
}


static void *
DMaterialAtomicDataDestructor(void *atomic,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	return atomic;
}


static void *
DMaterialAtomicDataCopier(void *dstatomic, const void *srcatomic,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{
	/*
	 * If source is enabled, enable destination
	 */
	if (DMaterialAtomicDataOffset > 0)
	{
		if (DMATERIALATOMICDATA(srcatomic, enabled))
		{
			RpDMaterialAtomicEnableDynamic(dstatomic);
		}
	}

	return dstatomic;
}


/*
 *****************************************************************************
 */
static void *
DMaterialWorldDataConstructor(void *world,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	if (DMaterialWorldDataOffset > 0 )
	{
		DMATERIALWORLDDATA(world, enabled) = FALSE;
		DMATERIALWORLDDATA(world, OldCallBackRender) = NULL;
	}

	return world;
}


static void *
DMaterialWorldDataDestructor(void *world,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	return world;
}


static void *
DMaterialWorldDataCopier(void *dstworld, const void *srcworld,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{
	/*
	 * If source is enabled, enable destination
	 */
	if (DMaterialWorldDataOffset > 0)
	{
		if (DMATERIALWORLDDATA(srcworld, enabled))
		{
			RpDMaterialWorldEnableDynamic(dstworld);
		}
	}

	return dstworld;
}


/*
 *****************************************************************************
 */
static void *
DMaterialWorldSectorDataConstructor(void *worldSector,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	if (DMaterialWorldSectorDataOffset > 0 )
	{
		DMATERIALWORLDSECTORDATA(worldSector, loaded) = FALSE;
	}

	return worldSector;
}


static void *
DMaterialWorldSectorDataDestructor(void *worldSector,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	return worldSector;
}


static void *
DMaterialWorldSectorDataCopier(void *dstworldSector, const void *srcworldSector,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{

	return dstworldSector;
}



/*
 *****************************************************************************
 */
RwBool
RpDMaterialPluginAttach(void)
{
	RwInt32 offset;

	/*
	 * Register global space...
	 */
	DMaterialGlobalDataOffset = RwEngineRegisterPlugin(0,
										  rwID_DMATERIAL,
										  DMaterialOpen,
										  DMaterialClose);

	if (DMaterialGlobalDataOffset < 0 )
	{
		return FALSE;
	}

	/*
	 * Register material extension space...
	 */
	DMaterialMaterialDataOffset = RpMaterialRegisterPlugin(sizeof(DMaterialMaterialData),
										rwID_DMATERIAL,
										DMaterialMaterialDataConstructor,
										DMaterialMaterialDataDestructor,
										DMaterialMaterialDataCopier);

	if (DMaterialMaterialDataOffset < 0 )
	{
		return FALSE;
	}

	/*
	 * Register binary stream functionality for extension data...
	 */
	offset = -1;
	offset = RpMaterialRegisterPluginStream(
										rwID_DMATERIAL,
										 DMaterialMaterialDataReadStream,
										 DMaterialMaterialDataWriteStream,
										 DMaterialMaterialDataGetStreamSize);

	if (offset != DMaterialMaterialDataOffset )
	{
		return FALSE;
	}

	/*
	 * Register atomic extension space...
	 */
	DMaterialAtomicDataOffset = RpAtomicRegisterPlugin(sizeof(DMaterialAtomicData),
										rwID_DMATERIAL,
										DMaterialAtomicDataConstructor,
										DMaterialAtomicDataDestructor,
										DMaterialAtomicDataCopier);

	if (DMaterialAtomicDataOffset < 0 )
	{
		return FALSE;
	}

	/*
	 * Register binary stream functionality for extension data...
	 */
	offset = -1;
	offset = RpAtomicRegisterPluginStream(
										rwID_DMATERIAL,
										 DMaterialAtomicDataReadStream,
										 DMaterialAtomicDataWriteStream,
										 DMaterialAtomicDataGetStreamSize);

	if (offset != DMaterialAtomicDataOffset )
	{
		return FALSE;
	}

	/*
	 * Register world extension space...
	 */
	DMaterialWorldDataOffset = RpWorldRegisterPlugin(sizeof(DMaterialWorldData),
										rwID_DMATERIAL,
										DMaterialWorldDataConstructor,
										DMaterialWorldDataDestructor,
										DMaterialWorldDataCopier);

	if (DMaterialWorldDataOffset < 0 )
	{
		return FALSE;
	}

	/*
	 * Register binary stream functionality for extension data...
	 */
	offset = -1;
	offset = RpWorldRegisterPluginStream(
										rwID_DMATERIAL,
										 DMaterialWorldDataReadStream,
										 DMaterialWorldDataWriteStream,
										 DMaterialWorldDataGetStreamSize);

	if (offset != DMaterialWorldDataOffset )
	{
		return FALSE;
	}

	/*
	 * Register worldsector extension space...
	 */
	DMaterialWorldSectorDataOffset = RpWorldSectorRegisterPlugin(sizeof(DMaterialWorldSectorData),
										rwID_DMATERIAL,
										DMaterialWorldSectorDataConstructor,
										DMaterialWorldSectorDataDestructor,
										DMaterialWorldSectorDataCopier);

	if (DMaterialWorldSectorDataOffset < 0 )
	{
		return FALSE;
	}

	/*
	 * Register binary stream functionality for extension data...
	 */
	offset = -1;
	offset = RpWorldSectorRegisterPluginStream(
										 rwID_DMATERIAL,
										 DMaterialWorldSectorDataReadStream,
										 DMaterialWorldSectorDataWriteStream,
										 DMaterialWorldSectorDataGetStreamSize);

	if (offset != DMaterialWorldSectorDataOffset )
	{
		return FALSE;
	}

	return TRUE;
}


/*
 *****************************************************************************
 */
RpAtomic *
RpDMaterialAtomicEnableDynamic(RpAtomic *atomic)
{
	/* Lets enable the effects. */
	if (DMATERIALATOMICDATA(atomic, enabled) == FALSE)
	{
		DMATERIALATOMICDATA(atomic, OldCallBackRender) = RpAtomicGetRenderCallBack(atomic);
		DMATERIALATOMICDATA(atomic, enabled) = TRUE;
		RpAtomicSetRenderCallBack(atomic, DMaterialAtomicCallbackRender);
	}

	return atomic;
}


/*
 *****************************************************************************
 */
RpWorld *
RpDMaterialWorldEnableDynamic(RpWorld *world)
{
	/* Lets enable the effects. */
	if (DMATERIALWORLDDATA(world, enabled) == FALSE)
	{
		DMATERIALWORLDDATA(world, OldCallBackRender) = RpWorldGetSectorRenderCallBack(world);
		DMATERIALWORLDDATA(world, enabled) = TRUE;
 		RpWorldSetSectorRenderCallBack(world, DMaterialWorldSectorCallbackRender);
	}

	return world;
}


/*
 *****************************************************************************
 */
RpAtomic *
RpDMaterialAtomicUpdateMaterial(RpAtomic *atomic)
{
	RpMaterial *material;
	DMaterialMaterialData *data;
	RwTexture *texture;
	RwInt32 i;
	RpGeometry *geometry = RpAtomicGetGeometry(atomic);
	RwInt32 numMaterials = RpGeometryGetNumMaterials(geometry);

	for (i = 0; i < numMaterials; i++)
	{
		material = RpGeometryGetMaterial(geometry, i);

		data = DMATERIALMATERIAL(material);

		texture = RpMaterialGetTexture(material);
		if ( texture )
		{
			rwstrncpy(data->texture1Name, texture->name, rwTEXTUREBASENAMELENGTH);
			rwstrncpy(data->texture1Mask, texture->mask, rwTEXTUREBASENAMELENGTH);
		}

		texture = RpMTextureMaterialGetTexture(material, 0, NULL);
		if ( texture )
		{
			rwstrncpy(data->texture2Name, texture->name, rwTEXTUREBASENAMELENGTH);
			rwstrncpy(data->texture2Mask, texture->mask, rwTEXTUREBASENAMELENGTH);
		}
	}

	return atomic;
}


/*
 *****************************************************************************
 */
RpWorld *
RpDMaterialWorldUpdateMaterial(RpWorld *world)
{
	RpMaterial *material;
	DMaterialMaterialData *data;
	RwTexture *texture;
	RwInt32 i;
	RwInt32 numMaterials = RpWorldGetNumMaterials(world);

	for (i = 0; i < numMaterials; i++)
	{
		material = RpWorldGetMaterial(world, i);

		data = DMATERIALMATERIAL(material);

		texture = RpMaterialGetTexture(material);
		if ( texture )
		{
			rwstrncpy(data->texture1Name, texture->name, rwTEXTUREBASENAMELENGTH);
			rwstrncpy(data->texture1Mask, texture->mask, rwTEXTUREBASENAMELENGTH);
		}

		texture = RpMTextureMaterialGetTexture(material, 0, NULL);
		if ( texture )
		{
			rwstrncpy(data->texture2Name, texture->name, rwTEXTUREBASENAMELENGTH);
			rwstrncpy(data->texture2Mask, texture->mask, rwTEXTUREBASENAMELENGTH);
		}
	}

	return world;
}


/*
 *****************************************************************************
 */
void *
RpDMaterialGetCustomData(RpMaterial *material)
{
   if (DMaterialMaterialDataOffset > 0)
   {
	   return DMATERIALMATERIALDATA(material, custData);
   }

   return NULL;
}


/*
 *****************************************************************************
 */
RpMaterial *
RpDMaterialSetCustomData(RpMaterial *material, void *data)
{
   if (DMaterialMaterialDataOffset > 0)
   {
	   DMATERIALMATERIALDATA(material, custData) = data;

	   return material;
   }

   return NULL;
}


/*
 *****************************************************************************
 */
RwInt32
RpDMaterialGetMaterialRefCount(RpMaterial *material)
{
   if (DMaterialMaterialDataOffset > 0)
   {
	   return DMATERIALMATERIALDATA(material, refCount);
   }

   return -1;
}
