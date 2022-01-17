#include "headers.h"

extern RwCamera *Camera;

RwCamera *AlCameraCreate(RpWorld *world);
void AlCameraReset(RwCamera *camera);
RwBool alCameraSetParentFrame(RwCamera *camera, RwFrame *frame);
RwBool AlCameraZoom(RwBool forward);
void AlCameraSetPos(RwInt32 delta);

