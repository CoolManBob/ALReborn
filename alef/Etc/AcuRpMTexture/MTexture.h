#ifndef MTEXTURE_H
#define MTEXTURE_H

#include "rwcore.h"
#include "rpworld.h"

#include "AcuRpMTexture.h"

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */

#define MTEXTURE_MAX_TEXTURES	7

/* Get plugin data */
#define MTEXTUREMATERIAL(material) \
	(RWPLUGINOFFSET(MTextureMaterialData, material, MTextureMaterialDataOffset))

#define MTEXTUREMATERIALDATA(material, var) \
    (RWPLUGINOFFSET(MTextureMaterialData, material, MTextureMaterialDataOffset)->var)

#define MTEXTUREMATERIALDATACONST(material, var) \
    (RWPLUGINOFFSETCONST(MTextureMaterialData, material, MTextureMaterialDataOffset)->var)

#define MTEXTUREATOMIC(atomic) \
    (RWPLUGINOFFSET(MTextureAtomicData, atomic, MTextureAtomicDataOffset))

#define MTEXTUREATOMICDATA(atomic, var) \
    (RWPLUGINOFFSET(MTextureAtomicData, atomic, MTextureAtomicDataOffset)->var)

#define MTEXTUREATOMICDATACONST(atomic, var) \
    (RWPLUGINOFFSETCONST(MTextureAtomicData, atomic, MTextureAtomicDataOffset)->var)

#define MTEXTUREWORLDSECTOR(worldSector) \
    (RWPLUGINOFFSET(MTextureWorldSectorData, worldSector, MTextureWorldSectorDataOffset))

#define MTEXTUREWORLDSECTORDATA(worldSector, var) \
    (RWPLUGINOFFSET(MTextureWorldSectorData, worldSector, MTextureWorldSectorDataOffset)->var)

#define MTEXTUREWORLDSECTORDATACONST(worldSector, var) \
    (RWPLUGINOFFSETCONST(MTextureWorldSectorData, worldSector, MTextureWorldSectorDataOffset)->var)

/* Material plugin data */
typedef struct MTextureMaterialData MTextureMaterialData;
struct MTextureMaterialData
{
	RwUInt16		textureNumber;
	RwTexture *		texture[MTEXTURE_MAX_TEXTURES];		/* Second texture */
	RpMTextureType	textureType[MTEXTURE_MAX_TEXTURES];
};

/* Atomic plugin data */
typedef struct MTextureAtomicData MTextureAtomicData;
struct MTextureAtomicData
{
	RwBool enabled;			/* Enable flag */
};

/* Worldsector plugin data */
typedef struct MTextureWorldSectorData MTextureWorldSectorData;
struct MTextureWorldSectorData
{
	RwBool enabled;			/* Enable flag */
};

/* Plugin offsets */
extern RwInt32 MTextureGlobalDataOffset;
extern RwInt32 MTextureMaterialDataOffset;
extern RwInt32 MTextureAtomicDataOffset;
extern RwInt32 MTextureWorldSectorDataOffset;

#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* MTEXTURE_H */
