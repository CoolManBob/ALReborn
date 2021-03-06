#ifndef PATCHSTREAM_H
#define PATCHSTREAM_H

/*===========================================================================*
 *--- Include files ---------------------------------------------------------*
 *===========================================================================*/
#include <rwcore.h>
#include <rpworld.h>

#include "rppatch.h"

#include "patchmesh.h"

/*===========================================================================*
 *--- Private Defines -------------------------------------------------------*
 *===========================================================================*/

/*===========================================================================*
 *--- Private Types ---------------------------------------------------------*
 *===========================================================================*/

/*===========================================================================*
 *--- Private Global Variables ----------------------------------------------*
 *===========================================================================*/

/*===========================================================================*
 *--- Private functions -----------------------------------------------------*
 *===========================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern RwUInt32
_rpPatchMeshStreamGetSize( const PatchMesh *mesh );

extern PatchMesh *
_rpPatchMeshStreamRead( RwStream *stream );

extern RwStream *
_rpPatchMeshStreamWrite( const PatchMesh *mesh,
                         RwStream *stream );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PATCHSTREAM_H */
