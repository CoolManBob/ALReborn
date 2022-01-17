#ifndef ALSHADOW_H
#define ALSHADOW_H

#include "headers.h"

RwBool SetupShadow();
void DestroyShadow();
void UpdateShadow(RwReal deltaTime __RWUNUSED__, RpClump *clump);
RwBool RenderShadow();
RwBool ResetShadow(RwBool justCheck);
RwBool SceneMenuShadowBlurCallback( RwBool testEnable );

#endif /* ALSHADOW_H */
