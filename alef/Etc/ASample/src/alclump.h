#include "headers.h"
#include "headers.h"

RpClump *AlClumpLoad(const RwChar *clumpPath);
void AlClumpDestroy(RpWorld *World, RpClump *Clump);
RpAtomic *AlClumpGetFirstAtomic(RpClump * clump);
RwBool AlClumpInstance(RpClump * clump);
void AlClumpGetBBox(RpClump *clump, RwBBox *bbox);
