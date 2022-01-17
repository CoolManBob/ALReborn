#include "headers.h"

#define FARAWAY (RwRealMAXVAL)

extern RwBool bMove;

RpCollisionTriangle *AlCollisionLineCallback(RpIntersection *intersection __RWUNUSED__,
                            RpWorldSector *sector __RWUNUSED__, 
                            RpCollisionTriangle *collTriangle,
                            RwReal distance, 
                            void *data);
RwBool AlCollisionLine(RwV3d *FramePos, RwV3d *FrameDelta, RpWorld *world, RwBBox *bbox);
RwBool PointWithinTriangle(RwV3d *pt, RwV3d *tri[3], RwV3d *normal);
RwV3d FindNearestPointOnLine(RwV3d *point, RwV3d *start, RwV3d *end);
RpCollisionTriangle *AlCollisionSphereCallback(RpIntersection *intersection,
                              RpWorldSector *sector  __RWUNUSED__, 
                              RpCollisionTriangle *collTriangle,
                              RwReal distance __RWUNUSED__, 
                              void *data);
void AlCollisionSphere(RwV3d *FramePos, RwV3d *FrameDelta, RpWorld *world);
void AlCollisionConfine(RwV3d *pos, RwV3d *delta, RpWorld *world);
