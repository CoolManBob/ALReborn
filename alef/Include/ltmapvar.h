//@{ Jaewon 20041203
// copied from rtltmap & modified.
//@} Jaewon
/*===========================================================================*
 *-                                                                         -*
 *-  Module  :   ltmapvar.h                                                 -*
 *-                                                                         -*
 *-  Purpose :   AcuRtAmbOcclMap toolkit                                    -*
 *-                                                                         -*
 *===========================================================================*/

#ifndef __LTMAPVAR_H__
#define __LTMAPVAR_H__

/*===========================================================================*
 *--- Include files ---------------------------------------------------------*
 *===========================================================================*/

/* Needed for memset */
#include "string.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rprandom.h"

#include "rpcollis.h"
#include "rtbary.h"

#include "rpdbgerr.h"

#include "rppvs.h"
#include "AcuRpMatD3DFx.h"
#include "AcuRtAmbOcclMap.h"


/*===========================================================================*
 *--- Local defines ---------------------------------------------------------*
 *===========================================================================*/

/* If this is defined, RtLtMapLightMapsCreate() will colour
 * in lightmaps to show usage and will print out statistics.
 * Yellow denotes used space, green free and red wasted. Blue
 * is uninitialised space and should not be seen. */
#define SHOWLIGHTMAPUSAGEx


/*===========================================================================*
 *--- Local Global Variables ------------------------------------------------*
 *===========================================================================*/

typedef struct LtMapCachedTriangle LtMapCachedTriangle ;

struct LtMapCachedTriangle
{
    RpCollisionTriangle triangle;
    RwUInt32            hit;
    RwBool              sky;
};

typedef struct rtLtMapGlobalVars rtLtMapGlobalVars;

struct rtLtMapGlobalVars
{
    /* Polygon packing & UV allocation  */
    RwV3d       rightVector[6];
    RwV3d       upVector[6];
    RwV3d       sortVector;
    RwSList     *ltMapSlots;

    /* Visibility determination */
    RwUInt32                        skyFlags;
    RtAmbOcclMapIlluminateVisCallBack    visCallBack;
    RwReal                          visCallBackCollisionScalar;

    /* Sampling */
    RpWorld                 *lightWorld;
    RwUInt32                lightObjectFlag;

    /* Post process */
    RtAmbOcclMapAmbOcclMapProcessCallBack ltMapProcessCallBack;

    LtMapCachedTriangle cachedTriangle;

#if (defined(SHOWLIGHTMAPUSAGE))
    RwImage *gLightMapUsageImage;
#endif /* (defined(SHOWLIGHTMAPUSAGE)) */

};

/*===========================================================================*
 *--- Global Variables ------------------------------------------------------*
 *===========================================================================*/

extern rtLtMapGlobalVars rtLtMapGlobals;


#endif /* __LTMAPVAR_H__ */

