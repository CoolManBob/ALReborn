#ifndef MTPIPE_H
#define MTPIPE_H

#include "rwcore.h"
#include "rpworld.h"

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */


/* Create and destroy pipelines */
RwBool MTexturePipeOpen();
void MTexturePipeClose();

/* Set pipeline at object */
RpAtomic *MTextureAtomicSetPipeline(RpAtomic *atomic);
RpWorldSector *MTextureWorldSectorSetPipeline(RpWorldSector *worldSector);
void	D3D9DefaultRenderBlack(RxD3D9ResEntryHeader *resEntryHeader,RwBool hasTextureCoordinates);


#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* MTPIPE_H */
