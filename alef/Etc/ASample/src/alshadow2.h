/****************************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *
 * shadow.h
 *
 * Copyright (C) 2001 Criterion Technologies.
 *
 * Original author: Matt Thorman
 * Reviewed by:
 *
 * Purpose: Example of parallel-projection shadow rendering using
 *          3D immediate mode.
 *
 ****************************************************************************/

#ifndef SHADOW_H
#define SHADOW_H

#include "headers.h"

extern RwUInt32     ShadowNumTriangles;

/*
 *  Shadow camera
 */
extern RwCamera    *ShadowCameraCreate(RwInt32 size);

extern void         ShadowCameraDestroy(RwCamera *shadowCamera);

extern RwCamera    *ShadowCameraSetFrustum(RwCamera *shadowCamera, 
                                           RwReal objectRadius);

extern RwCamera    *ShadowCameraSetLight(RwCamera *shadowCamera, 
                                         RpLight *light);

extern RwCamera    *ShadowCameraSetCenter(RwCamera *shadowCamera, 
                                          RwV3d *center);

extern RwCamera    *ShadowCameraUpdate(RwCamera *shadowCamera,
                                       RpClump *clump);

/*
 *  Shadow raster
 */
extern RwRaster    *ShadowRasterCreate(RwUInt32 size);

extern void         ShadowRasterDestroy(RwRaster *shadowRaster);

extern RwBool       ShadowRasterResample(RwRaster *destRaster,
                                         RwRaster *sourceRaster,
                                         RwCamera *camera);

extern RwBool       ShadowRasterBlur(RwRaster *shadowRaster,
                                     RwRaster *tempRaster,
                                     RwCamera *camera,
                                     RwUInt32 numPass);

extern RwRaster    *ShadowRasterRender(RwRaster *shadowRaster,
                                       RwV2d *verts);

/*
 *  Shadow rendering
 */
extern RwBool       ShadowRender(RwCamera *shadowCamera, 
                                 RwRaster *shadowRaster,
                                 RpWorld *world, 
                                 RpIntersection *shadowZone,
                                 RwReal shadowStrength, 
                                 RwReal fadeDist);

#endif /* SHADOW_H */
