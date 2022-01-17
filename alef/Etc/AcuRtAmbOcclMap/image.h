//@{ Jaewon 20041207
// copied from rtltmap & modified.
//@} Jaewon
/*===========================================================================*
 *-                                                                         -*
 *-  Module  :   image.c                                                    -*
 *-                                                                         -*
 *-  Purpose :   RtAmbOcclMap toolkit                                       -*
 *-                                                                         -*
 *===========================================================================*/

#ifndef __IMAGE_H__
#define __IMAGE_H__

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
 *--- Local Global Variables ------------------------------------------------*
 *===========================================================================*/


/*===========================================================================*
 *--- Local defines ---------------------------------------------------------*
 *===========================================================================*/


/*===========================================================================*
 *--- Toolkit SPI Functions -------------------------------------------------*
 *===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern RwBool
_rtLtMapLightMapImageSetFromTexture(RwTexture *lightMap,
                                 RwImage *image, rpLtMapSampleMap *sampleMap);

extern RwBool
_rtLtMapTextureSetFromLightMapImage(RwTexture *lightMap,
                                 RwImage *image, rpLtMapSampleMap *sampleMap);

extern RwTexture *
_rtLtMapDilate(RwTexture *lightMap, rpLtMapSampleMap *sampleMap);

extern RwBool
_rtLtMapLightMapClear(RwTexture *lightMap,
                     RwRGBA *colour, rpLtMapSampleMap *sampleMap);

#if (defined(SHOWLIGHTMAPUSAGE))

extern void
ImageRectClear(RwImage *image, RwRect *rect, RwRGBA colour);

extern void
RasterRectClear(RwRaster *raster, RwRect *rect, RwRGBA colour);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __IMAGE_H__ */

