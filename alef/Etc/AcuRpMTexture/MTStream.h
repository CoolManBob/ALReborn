#ifndef MTSTREAM_H
#define MTSTREAM_H

#include "rwcore.h"
#include "rpworld.h"

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */


/* Material stream functions */
RwStream *MTextureMaterialDataReadStream(RwStream *stream, RwInt32 length, void *material, RwInt32 offset, RwInt32 size);
RwStream *MTextureMaterialDataWriteStream(RwStream *stream, RwInt32 length, const void *material, RwInt32 offset, RwInt32 size);
RwInt32 MTextureMaterialDataGetStreamSize(const void *object, RwInt32 offset, RwInt32 size);

/* Atomic stream functions */
RwStream *MTextureAtomicDataReadStream(RwStream *stream, RwInt32 length, void *atomic, RwInt32 offset, RwInt32 size);
RwStream *MTextureAtomicDataWriteStream(RwStream *stream, RwInt32 length, const void *atomic, RwInt32 offset, RwInt32 size);
RwInt32 MTextureAtomicDataGetStreamSize(const void *object, RwInt32 offset, RwInt32 size);

/* Worldsector stream functions */
RwStream *MTextureWorldSectorDataReadStream(RwStream *stream, RwInt32 length, void *worldSector, RwInt32 offset, RwInt32 size);
RwStream *MTextureWorldSectorDataWriteStream(RwStream *stream, RwInt32 length, const void *worldSector, RwInt32 offset, RwInt32 size);
RwInt32 MTextureWorldSectorDataGetStreamSize(const void *object, RwInt32 offset, RwInt32 size);


#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* MTSTREAM_H */
