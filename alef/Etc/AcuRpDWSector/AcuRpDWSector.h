#ifndef ACURPDWSECTOR_H
#define ACURPDWSECTOR_H

#include "rwcore.h"
#include "rpworld.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuRpDWSectorD" )
#else
#pragma comment ( lib , "AcuRpDWSector" )
#endif
#endif


#define rwVENDORID_NHN			(0xfffff0L)
#define rwID_DWSECTOR_NHN		(0x03)
#define rwID_DWSECTOR_DATA_NHN	(0x04)

#define rwID_DWSECTOR			MAKECHUNKID(rwVENDORID_NHN, rwID_DWSECTOR_NHN)
#define rwID_DWSECTOR_DATA		MAKECHUNKID(rwVENDORID_NHN, rwID_DWSECTOR_DATA_NHN)

typedef enum RpDWSectorFields
{
	rpDWSECTOR_FIELD_NONE			= 0x0000,
	rpDWSECTOR_FIELD_VERTICES		= 0x0100,
	rpDWSECTOR_FIELD_POLYGONS		= 0x0200,
	rpDWSECTOR_FIELD_NORMALS		= 0x0400,
	rpDWSECTOR_FIELD_PRELITLUM		= 0x0800,
	rpDWSECTOR_FIELD_FLAG_TEXCOORDS	= 0x00ff
} RpDWSectorFields;

typedef enum RpDWSectorWriteMode
{
	rpDWSECTOR_WRITEMODE_ALL = 0, 
	rpDWSECTOR_WRITEMODE_CURRENT
} RpDWSectorWriteMode;

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */


/* Worldsector detail data */
typedef struct RpDWSector RpDWSector;
struct RpDWSector
{
	RpGeometry			*	geometry	;			// terrain geometry
	RpAtomic			*	atomic		;			// terrain atomic
};

/* Attach Plugin */
extern RwBool
RpDWSectorPluginAttach		( void );

/*
 * Initialize world
 * numDetail : Number of details
 * curDetail : Current detail number (currently world data)
 */
extern RpWorld *
RpDWSectorInitWorld			( RpWorld *world, RwUInt8 numDetail, RwInt8 curDetail);

/*
 * Initialize DWSector Object
 * dwSector : DWSector must be fetched by RpDWSectorGetDetail()
 * numVertices : Number of vertices
 * numPolygons : Number of Polygons
 * numTexCoords : Number of Texture Coordinates
 */
extern RpDWSector *
RpDWSectorInit				( RpDWSector *dwSector, RwUInt32 numVertices, RwUInt32 numPolygons, RwUInt8 numTexCoords);

/* Destroy world sector */
extern RpWorldSector *
RpDWSectorDestroyWorldSector( RpWorldSector *worldSector);

/* Must be called before RpWorldDestroy() */
extern RpWorld *
RpDWSectorDestroyWorld		( RpWorld *world);

/* Update current world sector data to proper detail. Must be called after polygon or vertice data is modified */
extern RpWorldSector *
RpDWSectorUpdateCurrent		( RpWorldSector *worldSector);

/* Get detail object (0 base)*/
extern RpDWSector *
RpDWSectorGetDetail			( RpWorldSector *worldSector, RwUInt8 numDetail);

/* Get current detail index (0 base) */
extern RwInt8 
RpDWSectorGetCurrentDetail	( RpWorldSector *worldSector);

/* Set current detail index (0 base, one sector) */
extern RpWorldSector *
RpDWSectorSetCurrentDetail	( RpWorldSector *worldSector, RwUInt8 numDetail);

/* Set current detail index (0 base, all the sectors) */
extern RpWorld *
RpDWSectorSetCurrentWorldDetail		( RpWorld *world, RwUInt8 numDetail);

/* Build world sector collision data */
extern RpWorldSector *
RpDWSectorBuildWorldSectorCollision	( RpWorldSector *worldSector);

/* Build world collision data */
extern RpWorld *
RpDWSectorBuildWorldCollision		( RpWorld *world);

/* Destroy DWSector (internally use) */
extern RwBool 
RpDWSectorDestroy			( RpDWSector *dwSector);

/* Destroy WorldSector detail (internally use) */
extern RpWorldSector *
RpDWSectorDestroyDetail		( RpWorldSector *worldSector, RwUInt8 numDetail);

extern RwStream *
RpDWSectorStreamWrite		( RpDWSector *dwSector, RwStream *stream, RpWorldSector *worldSector);

extern RwStream *
RpDWSectorStreamRead		( RpDWSector *dwSector, RwStream *stream, RpWorldSector *worldSector);

extern RwInt32
RpDWSectorStreamGetSize		( RpDWSector *dwSector, RpWorldSector *worldSector);

extern RwBool
RpDWSectorSetWriteMode		( RpWorld *world, RwUInt8 mode);

extern RpDWSectorFields
RpDWSectorGetFields			( RpDWSector *dwSector);

#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* ACURPDWSECTOR_H */
