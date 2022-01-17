#include "headers.h"

RpHAnimAnimation *AlHAnimLoad(RwChar *file);
RwFrame *AlHAnimGetChildFrameHierarchy(RwFrame *frame, void *data);
RpHAnimHierarchy *AlHAnimGetHierarchy(RpClump *clump);
RpAtomic *AlHAnimSetHierarchyForSkinAtomic(RpAtomic *atomic, void *data);
RwBool AlHAnimCreate(RpClump *clump, RpHAnimHierarchy **hierarchy, RpHAnimAnimation *anim[], RwChar *path, RwInt32 *number);
void AlHAnimDestroy();
RpAtomic *AlHAnimGetSkinHierarchy(RpAtomic *atomic, void *data);
void AlHAnimUpdateAnimation(RpHAnimHierarchy *hierarchy, RwReal deltaTime, RwBool move, RwBool additional);
void AlHAnimSetNextAnimation(void *character, RwInt32 index);

