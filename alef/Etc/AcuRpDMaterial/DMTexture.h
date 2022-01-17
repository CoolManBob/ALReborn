#ifndef DMTEXTURE_H
#define DMTEXTURE_H

#include "rwcore.h"
#include "rpworld.h"

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */


/* Render callbacks */
RpAtomic *DMaterialAtomicCallbackRender(RpAtomic *atomic);
RpWorldSector *DMaterialWorldSectorCallbackRender(RpWorldSector *worldSector);


#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* DMTEXTURE_H */
