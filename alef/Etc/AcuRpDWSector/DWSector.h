#ifndef DWSECTOR_H
#define DWSECTOR_H

#include "rwcore.h"
#include "rpworld.h"
#include "acurpdwsector.h"

#ifdef	__cplusplus
extern "C"
{
#endif  /* __cplusplus */


#define DWSECTOR_MAX_DETAIL		3

/* Get plugin data */
#define DWSECTORWORLD(world) \
	(RWPLUGINOFFSET(DWSectorWorldData, world, DWSectorWorldDataOffset))

#define DWSECTORWORLDDATA(world, var) \
	(RWPLUGINOFFSET(DWSectorWorldData, world, DWSectorWorldDataOffset)->var)

#define DWSECTORWORLDDATACONST(world, var) \
	(RWPLUGINOFFSETCONST(DWSectorWorldData, world, DWSectorWorldDataOffset)->var)

#define DWSECTORWORLDSECTOR(worldSector) \
	(RWPLUGINOFFSET(DWSectorWorldSectorData, worldSector, DWSectorWorldSectorDataOffset))

#define DWSECTORWORLDSECTORDATA(worldSector, var) \
	(RWPLUGINOFFSET(DWSectorWorldSectorData, worldSector, DWSectorWorldSectorDataOffset)->var)

#define DWSECTORWORLDSECTORDATACONST(worldSector, var) \
	(RWPLUGINOFFSETCONST(DWSectorWorldSectorData, worldSector, DWSectorWorldSectorDataOffset)->var)

#define DWSECTORCOLLISIONDATA(worldSector) \
	(RWPLUGINOFFSET(void *, worldSector, DWSectorCollisionDataOffset))

/* World plugin data */
typedef struct DWSectorWorldData DWSectorWorldData;
struct DWSectorWorldData
{
	RwUInt8				numDetail;				/* Number of detail */
	RwUInt8				modeWrite;				/* Stream Write Mode (RpWorldStreamWrite()) */
};

/* Worldsector plugin data */
typedef struct DWSectorWorldSectorData DWSectorWorldSectorData;
struct DWSectorWorldSectorData
{
	RwUInt8				numDetail;				/* Number of detail */
	RwInt8				curDetail;				/* Current detail index (-1 when disabled) */
	RwInt8				orgDetail;				/* Original current detail (for stream) */
	RpDWSector			details[DWSECTOR_MAX_DETAIL];	/* Detail data */
};


/* Plugin offsets */
extern RwInt32 DWSectorGlobalDataOffset;
extern RwInt32 DWSectorWorldDataOffset;
extern RwInt32 DWSectorWorldSectorDataOffset;

extern RwInt32 DWSectorCollisionDataOffset;		/* Offset of world sector collision offset */

RpDWSector *DWSectorCopy(RpDWSector *dstDWSector, RpDWSector *srcDWSector);


#ifdef	__cplusplus
}
#endif  /* __cplusplus */

#endif /* DWSECTOR_H */
