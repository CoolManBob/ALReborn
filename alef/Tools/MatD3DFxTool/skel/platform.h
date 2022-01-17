
/****************************************************************************
 *
 * platform.h
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

#ifndef PLATFORM_H
#define PLATFORM_H

#include "rwcore.h"
//#include "vecfont.h"
#include "skeleton.h"

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern void psWindowSetText(const RwChar *text);
extern void psErrorMessage(const RwChar *text);
extern void psWarningMessage(const RwChar *text);
extern void psDebugMessageHandler(RwDebugType type,
                                  const RwChar *str);

extern RwUInt32 psTimer(void);

extern RwImage* psGrabScreen(RwCamera *camera);


extern RwChar *psPathnameCreate(const RwChar *srcBuffer);
extern void psPathnameDestroy(RwChar *buffer);
extern RwChar psPathGetSeparator(void);

extern RwBool psInitialize(void);
extern void   psTerminate(void);
extern RwBool psAlwaysOnTop( RwBool AlwaysOnTop );
extern void psCameraShowRaster(RwCamera *camera);
extern void psMouseSetVisibility(RwBool visible);
extern void psMouseSetPos(RwV2d *pos);

extern RwBool psSelectDevice(RwBool useDefault);

/* install the platform specific file system */
extern RwBool psInstallFileSystem(void);

/* Render platform specific metrics */
extern void psMetricsRender(struct RsVectorFont *vecFont,
                            RwV2d *pos,
                            RwMetrics *metrics);

/* Handle native texture support */
extern RwBool psNativeTextureSupport(void);

/*  A microsecond timer */
extern RwUInt64 psMicroTimer(void);

#if (defined(__R5900__))
extern void skyOverideIOPPath(char *newPath);
#endif /* (defined(__R5900__)) */

#ifdef RWSPLASH
extern RwBool   psDisplaySplashScreen(RwBool state);
#endif

extern RsEventStatus PsEventHandler(RsEvent event, void *param);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* PLATFORM_H */
