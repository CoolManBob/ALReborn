#ifndef DWSECTOR_H
#define DWSECTOR_H

#include "rwcore.h"
#include "rpworld.h"
//#include "acurpdwsector.h"

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

#define DWSECTORATOMIC(atomic) \
	(RWPLUGINOFFSET(DWSectorAtomicData, atomic, DWSectorAtomicDataOffset))

typedef struct DWSectorData DWSectorData;
struct DWSectorData
{
	RwUInt8				numDetail;				/* Number of detail */
	RwInt8				curDetail;				/* Current detail index (-1 when disabled) */
	RwInt8				orgDetail;				/* Original current detail (for stream) */
	RpDWSector			details[DWSECTOR_MAX_DETAIL];	/* Detail data */
};

/* Atomic plugin data */
typedef struct DWSectorAtomicData DWSectorAtomicData;
struct DWSectorAtomicData
{
	void	*	pWorldSector	;	// RpWorldSector 포인터를 저장해둠..
};

/* Plugin offsets */

RpDWSector *	DWSectorCopy		( RpDWSector *dstDWSector, RpDWSector *srcDWSector	);
RwBool			RpDWSectorDestroy	( RpDWSector *dwSector								);

#ifdef	__cplusplus
}
#endif  /* __cplusplus */

#endif /* DWSECTOR_H */
