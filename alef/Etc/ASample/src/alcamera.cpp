#include "camera.h"
#include "alcamera.h"
#include "alworld.h"

extern AcuCamera	*g_clCamera;

/*
 *****************************************************************************
 */
RwCamera *AlCameraCreate(RpWorld *world)
{
	RwCamera *camera;

	camera = g_clCamera->CameraCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, TRUE);
	if( camera )
	{
#ifdef _USE_DPVS_
		RpDPVSWorldAddCamera(world, camera);
#else
		RpWorldAddCamera(world, camera);
#endif _USE_DPVS_

		WorldSphere.radius *= 4.0f;
		AlCameraReset(camera);

		return camera;
	}

	return NULL;
}

/*
 *****************************************************************************
 */
void AlCameraReset(RwCamera *camera)
{
	RwFrame *cameraFrame;
	RwV3d yAxis = {0.0f, 1.0f, 0.0f};
	RwV3d camPos = {0.0f, 90.0f, -200.0f};

	RwCameraSetNearClipPlane(camera, 100.0f);
	RwCameraSetFarClipPlane(camera, WorldSphere.radius/2.0f);

	RwCameraSetFogDistance(camera, WorldSphere.radius/10.0f);

	/*
	 * Make the camera's frame coincident with the base-frame...
	 */
	cameraFrame = RwCameraGetFrame(camera);
	RwFrameSetIdentity(cameraFrame);

	RwFrameTranslate(cameraFrame, &camPos, rwCOMBINEREPLACE);
	g_clCamera->CameraTilt(camera, NULL, 15);

	return;
}

/*
 *****************************************************************************
 */
RwBool alCameraSetParentFrame(RwCamera *camera, RwFrame *frame)
{
	RwFrame *cameraFrame;

	cameraFrame = RwCameraGetFrame(camera);

	if( !RwFrameAddChild(frame, cameraFrame) )
	{
		return FALSE;
	}

	return TRUE;
}

/*
 *****************************************************************************
 */
RwBool AlCameraZoom(RwBool forward)
{
	RwV3d delta = {0.0f, 0.0f, 0.0f};

	if (forward)
		delta.z = 40.0f;
	else
		delta.z = -40.0f;

	g_clCamera->CameraMove(Camera, &delta);

	return TRUE;
}

void AlCameraSetPos(RwInt32 delta)
{
	RwFrame *cameraFrame;
	RwV3d rotate = {0.0f, 1.0f, 0.0f};

	cameraFrame = RwCameraGetFrame(Camera);

	RwFrameRotate(cameraFrame, &rotate, 45.0f * delta, rwCOMBINEPOSTCONCAT);

	return;
}
