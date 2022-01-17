#ifndef ACURPDMATERIAL_H
#define ACURPDMATERIAL_H

#include "rwcore.h"
#include "rpworld.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuRpDMaterialD" )
#else
#pragma comment ( lib , "AcuRpDMaterial" )
#endif
#endif


#define rwVENDORID_NHN		(0xfffff0L)
#define rwID_DMATERIAL_NHN	(0x02)

#define rwID_DMATERIAL		MAKECHUNKID(rwVENDORID_NHN,rwID_DMATERIAL_NHN)

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */


/* Attach Plugin */
extern RwInt32 
RpDMaterialPluginAttach(void);

/* Enable Dynamic Material Functionality */
extern RpAtomic *
RpDMaterialAtomicEnableDynamic(RpAtomic *atomic);

extern RpWorld *
RpDMaterialWorldEnableDynamic(RpWorld *world);

/* Update Material Information (Must do this before save) */
extern RpAtomic *
RpDMaterialAtomicUpdateMaterial(RpAtomic *atomic);

extern RpWorld *
RpDMaterialWorldUpdateMaterial(RpWorld *world);

/* Unload Textures in Material (do this before save for unload textures) */
extern RpAtomic *
RpDMaterialAtomicUnload(RpAtomic *atomic);

extern RpWorld *
RpDMaterialWorldUnload(RpWorld *world);

extern RpWorldSector *
RpDMaterialWorldSectorUnload(RpWorldSector *worldSector);

/* Change Material Index of Triangle in Atomic or Polygon in World Sector */
extern RpAtomic *
RpDMaterialAtomicChangeMaterial(RpAtomic *atomic, RpTriangle *triangle, RwUInt16 matIndex);

extern RpWorldSector *
RpDMaterialWorldChangeMaterial(RpWorldSector *worldSector, RpPolygon *polygon, RwUInt16 matIndex);

/* Set/Get User Custom Data */
extern void *
RpDMaterialGetCustomData(RpMaterial *material);

extern RpMaterial *
RpDMaterialSetCustomData(RpMaterial *material, void *data);

/* Get Reference Count of Material */
extern RwInt32
RpDMaterialGetMaterialRefCount(RpMaterial *material);

#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* ACURPDMATERIAL_H */
