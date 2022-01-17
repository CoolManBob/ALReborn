#ifndef DMATERIAL_H
#define DMATERIAL_H

#include "rwcore.h"
#include "rpworld.h"

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */



/* Get plugin data */
#define DMATERIALMATERIAL(material) \
	(RWPLUGINOFFSET(DMaterialMaterialData, material, DMaterialMaterialDataOffset))

#define DMATERIALMATERIALDATA(material, var) \
    (RWPLUGINOFFSET(DMaterialMaterialData, material, DMaterialMaterialDataOffset)->var)

#define DMATERIALMATERIALDATACONST(material, var) \
    (RWPLUGINOFFSETCONST(DMaterialMaterialData, material, DMaterialMaterialDataOffset)->var)

#define DMATERIALATOMIC(atomic) \
    (RWPLUGINOFFSET(DMaterialAtomicData, atomic, DMaterialAtomicDataOffset))

#define DMATERIALATOMICDATA(atomic, var) \
    (RWPLUGINOFFSET(DMaterialAtomicData, atomic, DMaterialAtomicDataOffset)->var)

#define DMATERIALATOMICDATACONST(atomic, var) \
    (RWPLUGINOFFSETCONST(DMaterialAtomicData, atomic, DMaterialAtomicDataOffset)->var)

#define DMATERIALWORLD(world) \
    (RWPLUGINOFFSET(DMaterialWorldData, world, DMaterialWorldDataOffset))

#define DMATERIALWORLDDATA(world, var) \
    (RWPLUGINOFFSET(DMaterialWorldData, world, DMaterialWorldDataOffset)->var)

#define DMATERIALWORLDDATACONST(world, var) \
    (RWPLUGINOFFSETCONST(DMaterialWorldData, world, DMaterialWorldDataOffset)->var)

#define DMATERIALWORLDSECTOR(worldSector) \
    (RWPLUGINOFFSET(DMaterialWorldSectorData, worldSector, DMaterialWorldSectorDataOffset))

#define DMATERIALWORLDSECTORDATA(worldSector, var) \
    (RWPLUGINOFFSET(DMaterialWorldSectorData, worldSector, DMaterialWorldSectorDataOffset)->var)

#define DMATERIALWORLDSECTORDATACONST(worldSector, var) \
    (RWPLUGINOFFSETCONST(DMaterialWorldSectorData, worldSector, DMaterialWorldSectorDataOffset)->var)

/* Material plugin data */
typedef struct DMaterialMaterialData DMaterialMaterialData;
struct DMaterialMaterialData
{
	RwInt32 refCount;								/* Reference counter */
	RwChar texture1Name[rwTEXTUREBASENAMELENGTH];	/* Name of the texture1 */
	RwChar texture1Mask[rwTEXTUREBASENAMELENGTH];	/* Name of the textures1 mask */
	RwChar texture2Name[rwTEXTUREBASENAMELENGTH];	/* Name of the texture2 */
	RwChar texture2Mask[rwTEXTUREBASENAMELENGTH];	/* Name of the textures2 mask */
	void *custData;									/* Custom data */
};

/* Atomic plugin data */
typedef struct DMaterialAtomicData DMaterialAtomicData;
struct DMaterialAtomicData
{
	RwBool enabled;									/* Is Dynamic material enabled? */
	RpAtomicCallBackRender OldCallBackRender;		/* Old Redner Callback */
	RwBool loaded;									/* Is Material loaded? */
};

/* World plugin data */
typedef struct DMaterialWorldData DMaterialWorldData;
struct DMaterialWorldData
{
	RwBool enabled;									/* Is Dynamic material enabled? */
	RpWorldSectorCallBackRender OldCallBackRender;	/* Old Redner Callback */
};

/* Worldsector plugin data */
typedef struct DMaterialWorldSectorData DMaterialWorldSectorData;
struct DMaterialWorldSectorData
{
	RwBool loaded;									/* Is Material loaded? */
};


/* Plugin offsets */
extern RwInt32 DMaterialGlobalDataOffset;
extern RwInt32 DMaterialMaterialDataOffset;
extern RwInt32 DMaterialAtomicDataOffset;
extern RwInt32 DMaterialWorldDataOffset;
extern RwInt32 DMaterialWorldSectorDataOffset;

#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* DMATERIAL_H */
