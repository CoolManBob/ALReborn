#ifndef _SKINNULL_H
#define _SKINNULL_H

/*===========================================================================*
 *--- Include files ---------------------------------------------------------*
 *===========================================================================*/
#include <rwcore.h>
#include <rpworld.h>

#include "rpskin.h"

/*===========================================================================*
 *--- Global Types ----------------------------------------------------------*
 *===========================================================================*/
struct SkinPlatformData
{
    RwV3d *vertices;
    RwV3d *normals;
};

struct SkinGlobalPlatform {
    RxPipeline *pipelines[rpSKINNULLPIPELINEMAX - 1];
};

/*===========================================================================*
 *--- Private Global Variables ----------------------------------------------*
 *===========================================================================*/

/*===========================================================================*
 *--- Private Defines -------------------------------------------------------*
 *===========================================================================*/
#define rpSKINMAXNUMBEROFMATRICES 256

#define _rpSkinPipeline(pipeline)                                       \
    (_rpSkinGlobals.platform.pipelines[pipeline - 1])

/*===========================================================================*
 *--- Private Functions -----------------------------------------------------*
 *===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SKINNULL_H */
