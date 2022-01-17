#include "rwcore.h"
#include "rpworld.h"

#include "acurpdmaterial.h"
#include "dmaterial.h"
#include "dmtexture.h"
#include "acurpmtexture.h"


/*
 *****************************************************************************
 */
RpMaterial *DMaterialLoadMaterial(RpMaterial *material)
{
	RwTexture *texture;
	RwChar *name1;
	RwChar *name2;
	RwChar *mask1;
	RwChar *mask2;

	name1 = DMATERIALMATERIALDATA(material, texture1Name);
	mask1 = DMATERIALMATERIALDATA(material, texture1Mask);
	name2 = DMATERIALMATERIALDATA(material, texture2Name);
	mask2 = DMATERIALMATERIALDATA(material, texture2Mask);

	if (!RpMaterialGetTexture(material) && name1[0])
	{
		texture = RwTextureRead(name1, mask1[0] ? mask1:NULL);
		RpMaterialSetTexture(material, texture);
		RwTextureDestroy(texture);
	}

	if (!RpMTextureMaterialGetTexture(material, 0, NULL) && name2[0])
	{
		texture = RwTextureRead(name2, mask2[0] ? mask2:NULL);
		RpMTextureMaterialSetTexture(material, 0, texture, rpMTEXTURE_TYPE_ALPHA);
		RwTextureDestroy(texture);
	}

	DMATERIALMATERIALDATA(material, refCount) += 1;

	return material;
}


RpMaterial *DMaterialUnloadMaterial(RpMaterial *material)
{
	if (DMATERIALMATERIALDATA(material, refCount) > 0)
		DMATERIALMATERIALDATA(material, refCount) -= 1;

	if (DMATERIALMATERIALDATA(material, refCount) == 0)
	{
		RpMaterialSetTexture(material, NULL);
		RpMTextureMaterialSetTexture(material, 0, NULL, rpMTEXTURE_TYPE_NONE);
	}

	return material;
}

/*
 *****************************************************************************
 */
RpAtomic *DMaterialAtomicLoadMaterial(RpAtomic *atomic)
{
	RpMaterial *material;
	RwInt32 i;
	RpGeometry *geometry = RpAtomicGetGeometry(atomic);
	RpTriangle *triangles = RpGeometryGetTriangles(geometry);
	RwInt32 numTriangles = RpGeometryGetNumTriangles(geometry);

	if (!DMATERIALATOMICDATA(atomic, loaded))
	{
		for (i = 0; i < numTriangles; i++)
		{
			material = RpGeometryGetMaterial(geometry, triangles[i].matIndex);
			if (material)
			{
				DMaterialLoadMaterial(material);
			}
		}

		DMATERIALATOMICDATA(atomic, loaded) = TRUE;
	}

	return atomic;
}


RpWorldSector *DMaterialWorldSectorLoadMaterial(RpWorldSector *worldSector)
{
	RpMaterial *material;
	RwInt32 i;
	RpWorld *world = RpWorldSectorGetWorld(worldSector);

	if (!DMATERIALWORLDSECTORDATA(worldSector, loaded))
	{
		for (i = 0; i < worldSector->numPolygons; i++)
		{
			material = RpWorldGetMaterial(world, worldSector->polygons[i].matIndex);
			if (material)
			{
				DMaterialLoadMaterial(material);
			}
		}

		DMATERIALWORLDSECTORDATA(worldSector, loaded) = TRUE;
	}

	return worldSector;
}


/*
 *****************************************************************************
 */
RpAtomic *DMaterialAtomicCallbackRender(RpAtomic *atomic)
{
	RpAtomicCallBackRender OldCallBackRender;

	OldCallBackRender = DMATERIALATOMICDATA(atomic, OldCallBackRender);

	if (!DMATERIALATOMICDATA(atomic, loaded))
		atomic = DMaterialAtomicLoadMaterial(atomic);

	if (atomic && OldCallBackRender)
	{
		return OldCallBackRender(atomic);
	}

	return atomic;
}

RpWorldSector *DMaterialWorldSectorCallbackRender(RpWorldSector *worldSector)
{
	RpWorldSectorCallBackRender OldCallBackRender;

	OldCallBackRender = DMATERIALWORLDDATA(RpWorldSectorGetWorld(worldSector), OldCallBackRender);

	if (!DMATERIALWORLDSECTORDATA(worldSector, loaded))
		worldSector = DMaterialWorldSectorLoadMaterial(worldSector);

	if (worldSector && OldCallBackRender)
	{

		return OldCallBackRender(worldSector);
	}

	return NULL;
}


/*
 *****************************************************************************
 */
RpAtomic *
RpDMaterialAtomicUnload(RpAtomic *atomic)
{
	RwInt32 i;
	RpMaterial *material;
	RpGeometry *geometry = RpAtomicGetGeometry(atomic);
	RwInt32 numTriangles = RpGeometryGetNumTriangles(geometry);
	RpTriangle *triangles = RpGeometryGetTriangles(geometry);

	if (DMATERIALATOMICDATA(atomic, loaded))
	{
		for (i = 0; i < numTriangles; i++)
		{
			material = RpGeometryGetMaterial(geometry, triangles[i].matIndex);
			if (material)
			{
				DMaterialUnloadMaterial(material);
			}
		}

		DMATERIALATOMICDATA(atomic, loaded) = FALSE;
	}

	return atomic;
}

RpWorldSector *DMaterialUnloadWorldSector(RpWorldSector *worldSector, void *data)
{
	return RpDMaterialWorldSectorUnload(worldSector);
}

RpWorld *
RpDMaterialWorldUnload(RpWorld *world)
{
	RpWorldForAllWorldSectors(world, DMaterialUnloadWorldSector, NULL);

	return world;
}

RpWorldSector *
RpDMaterialWorldSectorUnload(RpWorldSector *worldSector)
{
	RwInt32 i;
	RpMaterial *material;
	RwInt32 numPolygons = RpWorldSectorGetNumPolygons(worldSector);
	RpWorld *world = RpWorldSectorGetWorld((const RpWorldSector *) worldSector);

	if (DMATERIALWORLDSECTORDATA(worldSector, loaded))
	{
		for (i = 0; i < numPolygons; i++)
		{
			material = RpWorldGetMaterial(world, worldSector->polygons[i].matIndex);
			if (material)
			{
				DMaterialUnloadMaterial(material);
			}
		}

		DMATERIALWORLDSECTORDATA(worldSector, loaded) = FALSE;
	}

	return worldSector;
}


/*
 *****************************************************************************
 */
RpAtomic *
RpDMaterialAtomicChangeMaterial(RpAtomic *atomic, RpTriangle *triangle, RwUInt16 matIndex)
{
	return atomic;
}


RpWorldSector *
RpDMaterialWorldChangeMaterial(RpWorldSector *worldSector, RpPolygon *polygon, RwUInt16 matIndex)
{
	RpMaterial *material;
	RpWorld *world = RpWorldSectorGetWorld((const RpWorldSector *) worldSector);

	material = RpWorldGetMaterial(world, polygon->matIndex);
	if (material)
	{
		DMaterialUnloadMaterial(material);
	}

	material = RpWorldGetMaterial(world, matIndex);
	if (material)
	{
		DMaterialLoadMaterial(material);
	}

	return worldSector;
}
