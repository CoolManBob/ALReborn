#include "alworld.h"

RwSphere WorldSphere;

extern RpWorld *World;
RpWorld *World2 = NULL;
RpWorldSector *WorldSector = NULL;
RpWorldSector *WorldSector2 = NULL;

RwInt32 Index1;
RwInt32 Index2;

RpWorldSector *GetFirstSector(RpWorldSector *worldSector, void *data)
{
	if (Index2 == Index1)
	{
		WorldSector = worldSector;
		return NULL;
	}

	Index2++;

	return worldSector;
}

RpWorldSector *SectorCB(RpWorldSector *worldSector, void *data)
{
	WorldSector2 = worldSector;
	RwInt32 i;

	if (!World2)
		World2 = AlWorldLoad(RWSTRING("models/ground2.bsp"));

	Index2 = 0;
	RpWorldForAllWorldSectors(World2, GetFirstSector, NULL);

	if (!WorldSector)
		return NULL;

    worldSector->type = WorldSector->type;
    worldSector->polygons = WorldSector->polygons;
    worldSector->vertices = WorldSector->vertices;
    worldSector->normals = WorldSector->normals;
	for (i = 0; i < rwMAXTEXTURECOORDS; i++)
	{
		worldSector->texCoords[i] = WorldSector->texCoords[i];
	}
    worldSector->preLitLum = WorldSector->preLitLum;
    worldSector->numVertices = WorldSector->numVertices;
    worldSector->numPolygons = WorldSector->numPolygons;
    worldSector->pad = WorldSector->pad;
    worldSector->mesh = NULL;
/*
    worldSector->boundingBox = WorldSector->boundingBox;
    worldSector->tightBoundingBox = WorldSector->tightBoundingBox;
    worldSector->mesh = WorldSector->mesh;
    worldSector->matListWindowBase = WorldSector->matListWindowBase;
    worldSector->colSectorRoot = WorldSector->colSectorRoot;
    worldSector->pipeline = WorldSector->pipeline;
    worldSector->repEntry = WorldSector->repEntry;
*/

	Index1++;

	return NULL;
}

void AlWorldChangeSector()
{
	Index1 = 0;
	World2 = NULL;
	RpWorldLock(World);

	RpWorldForAllWorldSectors(World, SectorCB, NULL);

	RpWorldUnlock(World);
}

/*
 *****************************************************************************
 */
RpWorld *AlWorldCreate(RwChar *path)
{
	RpWorld *world;
	RwInt32 index;

	world = AlWorldLoad(RWSTRING(path));


	for (index = 0; index < RpWorldGetNumMaterials(world); index++)
	{
		RwTextureSetFilterMode(RpMaterialGetTexture(RpWorldGetMaterial(world, index)), rwFILTERNEAREST);
	}

	return world;
}

/*
 *****************************************************************************
 */
void AlWorldGetBoundingSphere(RpWorld *world, RwSphere *sphere)
{
	const RwBBox *bbox = (const RwBBox *)NULL;
	RwV3d temp;

	bbox = RpWorldGetBBox(world);

	RwV3dAdd(&temp, &bbox->sup, &bbox->inf);
	RwV3dScale(&sphere->center, &temp, 0.5f);

	RwV3dSub(&temp, &bbox->sup, &bbox->inf);

	sphere->radius = RwV3dLength(&temp) * 0.5f;

	return;
}

/*
 *****************************************************************************
 */
RpWorld *AlWorldLoad(RwChar *bspPath)
{
	RwStream *stream = (RwStream *)NULL;
	RpWorld *world = (RpWorld *)NULL;
	RwChar *path = (RwChar *)NULL;

	path = RsPathnameCreate(bspPath);

	RsSetModelTexturePath(path);

	/*
	 * Now load the BSP...
	 */
	stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);
	if( stream )
	{
		if( RwStreamFindChunk(stream, rwID_WORLD,
							  (RwUInt32 *)NULL, (RwUInt32 *)NULL) )
		{
			world = RpWorldStreamRead(stream);

			RwStreamClose (stream, NULL);
		}
	}

	RsPathnameDestroy(path);

	if( world )
	{
		AlWorldGetBoundingSphere(world, &WorldSphere);
	}

	return world;
}
