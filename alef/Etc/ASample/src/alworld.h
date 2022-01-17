#include "headers.h"

extern RwSphere WorldSphere;

RpWorld *AlWorldCreate(RwChar *path);
void AlWorldGetBoundingSphere(RpWorld *world, RwSphere *sphere);
RpWorld *AlWorldLoad(RwChar *bspPath);
void AlWorldChangeSector();
