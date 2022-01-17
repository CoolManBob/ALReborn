#ifndef DWSNATIVE_H
#define DWSNATIVE_H

#include "rwcore.h"
#include "rpworld.h"

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */

RwStream *		DWSectorNativeWrite	( RwStream *stream, const RpWorldSector *sector, RwResEntry *repEntry);
RwResEntry *	DWSectorNativeRead	( RwStream *stream, RpWorldSector *sector);
RwInt32			DWSectorNativeSize	( const RpWorldSector *sector, RwResEntry *repEntry);

#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* DWSNATIVE_H */
