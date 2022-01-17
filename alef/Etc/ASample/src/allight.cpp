#include "allight.h"

RpLight *AmbientLight = (RpLight *)NULL;
RpLight *MainLight = (RpLight *)NULL;

/*
 *****************************************************************************
 */
RpLight *AlLightCreateAmbient(RpWorld *World)
{
	RpLight *light = (RpLight *)NULL;

	light = RpLightCreate(rpLIGHTAMBIENT);
	if( light )
	{
		RwRGBAReal color;

		color.red = color.green = color.blue = AMBIENTINTENSITY;
		color.alpha = 0;

		RpLightSetColor(light, &color);

		RpWorldAddLight(World,light);

		return light;
	}

	return (RpLight *)NULL;
}

/*
 *****************************************************************************
 */
RpLight *AlLightCreateMain(RpWorld *World)
{
	RpLight *light = (RpLight *)NULL;

	light = RpLightCreate(rpLIGHTDIRECTIONAL);
	if( light )
	{
		RwFrame *frame = (RwFrame *)NULL;
		RwRGBAReal color;

		color.red = color.green = color.blue = MAININTENSITY;
		color.alpha = 0;

		RpLightSetColor(light, &color);

		frame = RwFrameCreate();
		if( frame )
		{
			RwV3d xAxis = {1.0f, 0.0f, 0.0f};
			RwV3d yAxis = {0.0f, 1.0f, 0.0f};

			RwFrameRotate(frame, &xAxis, DLIGHT_ELEVATION, rwCOMBINEREPLACE);
			RwFrameRotate(frame, &yAxis, -DLIGHT_AZIMUTH, rwCOMBINEPOSTCONCAT);

			RpLightSetFrame(light, frame);

			RpWorldAddLight(World,light);

			return light;
		}

		RpLightDestroy(light);
	}

	return (RpLight *)NULL;
}
