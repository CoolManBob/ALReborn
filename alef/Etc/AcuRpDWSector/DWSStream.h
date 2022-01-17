#ifndef DWSSTREAM_H
#define DWSSTREAM_H

#include "rwcore.h"
#include "rpworld.h"

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */


/* Worldsector stream functions */
RwStream *	DWSectorWorldSectorDataReadStream		( RwStream *stream, RwInt32 length, void *worldSector, RwInt32 offset, RwInt32 size);
RwStream *	DWSectorWorldSectorDataWriteStream		( RwStream *stream, RwInt32 length, const void *worldSector, RwInt32 offset, RwInt32 size);
RwInt32		DWSectorWorldSectorDataGetStreamSize	( const void *object, RwInt32 offset, RwInt32 size);

/* World stream functions */
RwStream *	DWSectorWorldDataReadStream				( RwStream *stream, RwInt32 length, void *world, RwInt32 offset, RwInt32 size);
RwStream *	DWSectorWorldDataWriteStream			( RwStream *stream, RwInt32 length, const void *world, RwInt32 offset, RwInt32 size);
RwInt32		DWSectorWorldDataGetStreamSize			( const void *object, RwInt32 offset, RwInt32 size);


#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* DWSSTREAM_H */
