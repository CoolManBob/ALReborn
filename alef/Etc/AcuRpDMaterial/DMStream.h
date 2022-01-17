#ifndef DMSTREAM_H
#define DMSTREAM_H

#include "rwcore.h"
#include "rpworld.h"

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */


/* Material stream functions */
RwStream *DMaterialMaterialDataReadStream(RwStream *stream, RwInt32 length, void *material, RwInt32 offset, RwInt32 size);
RwStream *DMaterialMaterialDataWriteStream(RwStream *stream, RwInt32 length, const void *material, RwInt32 offset, RwInt32 size);
RwInt32 DMaterialMaterialDataGetStreamSize(const void *object, RwInt32 offset, RwInt32 size);

/* Atomic stream functions */
RwStream *DMaterialAtomicDataReadStream(RwStream *stream, RwInt32 length, void *atomic, RwInt32 offset, RwInt32 size);
RwStream *DMaterialAtomicDataWriteStream(RwStream *stream, RwInt32 length, const void *atomic, RwInt32 offset, RwInt32 size);
RwInt32 DMaterialAtomicDataGetStreamSize(const void *object, RwInt32 offset, RwInt32 size);

/* World stream functions */
RwStream *DMaterialWorldDataReadStream(RwStream *stream, RwInt32 length, void *world, RwInt32 offset, RwInt32 size);
RwStream *DMaterialWorldDataWriteStream(RwStream *stream, RwInt32 length, const void *world, RwInt32 offset, RwInt32 size);
RwInt32 DMaterialWorldDataGetStreamSize(const void *object, RwInt32 offset, RwInt32 size);

/* Worldsector stream functions */
RwStream *DMaterialWorldSectorDataReadStream(RwStream *stream, RwInt32 length, void *worldSector, RwInt32 offset, RwInt32 size);
RwStream *DMaterialWorldSectorDataWriteStream(RwStream *stream, RwInt32 length, const void *worldSector, RwInt32 offset, RwInt32 size);
RwInt32 DMaterialWorldSectorDataGetStreamSize(const void *object, RwInt32 offset, RwInt32 size);


#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* DMSTREAM_H */
