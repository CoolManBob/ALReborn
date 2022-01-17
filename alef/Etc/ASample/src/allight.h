#include "headers.h"

#define AMBIENTINTENSITY    (0.5f)
#define MAININTENSITY      (0.8f);

#define DLIGHT_ELEVATION    30.0f
#define DLIGHT_AZIMUTH      60.0f

extern RpLight *AmbientLight;
extern RpLight *MainLight;

RpLight *AlLightCreateAmbient(RpWorld *World);
RpLight *AlLightCreateMain(RpWorld *World);

