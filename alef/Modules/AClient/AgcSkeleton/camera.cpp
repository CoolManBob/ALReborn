#ifdef USE_MFC

/****************************************************************************
 *
 * camera.c
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */
//#include "stdafx.h"

#include "rwcore.h"

#include "camera.h"
#include "ptrdata.h"
#include "MagDebug.h"


/*
 *****************************************************************************
 */
bool
AcuCamera::CameraSize(RwCamera * pCamera, RwRect *rect, RwReal viewWindow, RwReal aspectRatio)
{
	RwRaster *pRaster = RwCameraGetRaster(pCamera);
	if (pRaster)
	{
		RwRasterDestroy(pRaster);
		pRaster = NULL;
	}
	
	VERIFY( pRaster = RwRasterCreate( rect->w , rect->h , 0 , rwRASTERTYPECAMERA ) );

	// 생성이 안됀다면.
	if( pRaster == NULL )
	{
		RwCameraSetRaster( pCamera , NULL );
		return false;
	}
	
	RwCameraSetRaster(pCamera, pRaster);
	
	pRaster = RwCameraGetZRaster(pCamera);
	if (pRaster)
	{
		RwRasterDestroy(pRaster);
		pRaster = NULL;
	}
	
	VERIFY( pRaster = RwRasterCreate( rect->w , rect->h , 0 , rwRASTERTYPEZBUFFER ) );

	// 생성이 안됀다면.
	if( pRaster == NULL )
	{
		RwCameraSetZRaster( pCamera , NULL );
		return false;
	}
	
	RwCameraSetZRaster(pCamera, pRaster);
	
	// const float viewWindowWidth = 0.7f;
	
	RwV2d vw;
	vw.x = viewWindow;
	vw.y = viewWindow * ((RwReal)rect->h/(RwReal)rect->w);
	RwCameraSetViewWindow(pCamera, &vw);
	return true;
}


/*
 *****************************************************************************
 */
void
AcuCamera::CameraDestroy(RwCamera *camera)
{
    if( camera )
    {
        RwRaster *raster;
        RwFrame *frame;

        frame = RwCameraGetFrame(camera);
        if( frame )
        {
            RwCameraSetFrame(camera, NULL);

            RwFrameDestroy(frame);
        }

        raster = RwCameraGetRaster(camera);
        if( raster )
        {
            RwRasterDestroy(raster);

            RwCameraSetRaster(camera, NULL);
        }

        raster = RwCameraGetZRaster(camera);
        if( raster )
        {
            RwRasterDestroy(raster);

            RwCameraSetZRaster(camera, NULL);
        }

        RwCameraDestroy(camera);
    }

    return;
}


/*
 *****************************************************************************
 */
RwCamera *
AcuCamera::CameraCreate(RwInt32 width, RwInt32 height, RwBool zBuffer, RwCamera* pDestCamera, RwBool bResize)
{
	RwCamera *pCamera	;
	RwRaster *pRaster	;
	RwRaster *pZBuffer	;
	
	pCamera = RwCameraCreate();

	if( pCamera )
	{
		RwCameraSetFrame( pCamera , RwFrameCreate() );
		
		// Camera Raster Creation..
		pRaster = RwRasterCreate(width, height, 0, rwRASTERTYPECAMERA );
		RwCameraSetRaster( pCamera , pRaster);
		
		if( zBuffer )
		{
			pZBuffer = RwRasterCreate( width , height , 0, rwRASTERTYPEZBUFFER );
			RwCameraSetZRaster( pCamera , pZBuffer );
		}
		
		if( RwCameraGetFrame	(pCamera) &&
			RwCameraGetRaster	(pCamera) &&
			(!zBuffer || RwCameraGetZRaster(pCamera)) )
		{
			return pCamera;
		}
		else
		{
			// There are some errors...
			TRACE( "AcuCamera::CameraCreate 카메라 초기화 실패\n" );
			return NULL;
		}
	}
	
	// Some errors ,... Clean up
	CameraDestroy(pCamera);
	
	return NULL;
}


/*
 *****************************************************************************
 */
void
AcuCamera::CameraMove(RwCamera *camera, RwV3d *delta)
{
    RwV3d offset;
    RwFrame *cameraFrame;
    RwMatrix *cameraMatrix;
    RwV3d *at, *up, *right;

    cameraFrame = RwCameraGetFrame(camera);
    cameraMatrix = RwFrameGetMatrix(cameraFrame);
    
    at = RwMatrixGetAt(cameraMatrix);
    up = RwMatrixGetUp(cameraMatrix);
    right = RwMatrixGetRight(cameraMatrix);

    offset.x = delta->x * right->x + delta->y * up->x + delta->z * at->x;
    offset.y = delta->x * right->y + delta->y * up->y + delta->z * at->y;
    offset.z = delta->x * right->z + delta->y * up->z + delta->z * at->z;

    RwFrameTranslate(cameraFrame, &offset, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 *****************************************************************************
 */
void
AcuCamera::CameraPan(RwCamera *camera, const RwV3d *pos, RwReal angle)
{
    RwV3d invCamPos;
    RwFrame *cameraFrame;
    RwMatrix *cameraMatrix;
    RwV3d camPos;

    cameraFrame = RwCameraGetFrame(camera);
    cameraMatrix = RwFrameGetMatrix(cameraFrame);

    camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);

    RwV3dScale(&invCamPos, &camPos, -1.0f);

    /* 
     * Translate the camera back to the rotation origin...
     */
    RwFrameTranslate(cameraFrame, &invCamPos, rwCOMBINEPOSTCONCAT);

    /* 
     * Get the camera's UP vector and use this as the axis of rotation...
     */
    RwMatrixRotate(cameraMatrix, 
        RwMatrixGetUp(cameraMatrix), angle, rwCOMBINEPOSTCONCAT);

    /* 
     * Translate the camera back to its original position...
     */
    RwFrameTranslate(cameraFrame, &camPos, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 *****************************************************************************
 */
void
AcuCamera::CameraTilt(RwCamera *camera, const RwV3d *pos, RwReal angle)
{
    RwV3d invCamPos;
    RwFrame *cameraFrame;
    RwMatrix *cameraMatrix;
    RwV3d camPos;

    cameraFrame = RwCameraGetFrame(camera);
    cameraMatrix = RwFrameGetMatrix(cameraFrame);

    camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);

    RwV3dScale(&invCamPos, &camPos, -1.0f);

    /* 
     * Translate the camera back to the rotation origin...
     */
    RwFrameTranslate(cameraFrame, &invCamPos, rwCOMBINEPOSTCONCAT);

    /* 
     * Get the camera's RIGHT vector and use this as the axis of rotation...
     */
    RwMatrixRotate(cameraMatrix, 
        RwMatrixGetRight(cameraMatrix), angle, rwCOMBINEPOSTCONCAT);

    /* 
     * Translate the camera back to its original position...
     */
    RwFrameTranslate(cameraFrame, &camPos, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 *****************************************************************************
 */
void
AcuCamera::CameraRotate(RwCamera *camera, const RwV3d *pos, RwReal angle)
{
    RwV3d invCamPos;
    RwFrame *cameraFrame;
    RwMatrix *cameraMatrix;
    RwV3d camPos;

    cameraFrame = RwCameraGetFrame(camera);
    cameraMatrix = RwFrameGetMatrix(cameraFrame);

    camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);

    RwV3dScale(&invCamPos, &camPos, -1.0f);

    /* 
     * Translate the camera back to the rotation origin...
     */
    RwFrameTranslate(cameraFrame, &invCamPos, rwCOMBINEPOSTCONCAT);

    /* 
     * Get the camera's AT vector and use this as the axis of rotation...
     */
    RwMatrixRotate(cameraMatrix, RwMatrixGetAt(cameraMatrix),
                   angle, rwCOMBINEPOSTCONCAT);

    /* 
     * Translate the camera back to its original position...
     */
    RwFrameTranslate(cameraFrame, &camPos, rwCOMBINEPOSTCONCAT);

    return;
}


/*
 *****************************************************************************
 */
RwRaster *
AcuCamera::CameraCreateCrossHair(void)
{
    RwImage *image;

    image = RwImageCreate(CROSSHAIRWIDTH, CROSSHAIRWIDTH, 32);
    if( image )
    {
        RwRaster *raster;

        RwImageSetStride(image, CROSSHAIRWIDTH * 4);
        RwImageSetPixels(image, CrossHairData);

        /*
         * Create the pointer raster...
         */
        raster = RwRasterCreate(
            RwImageGetWidth(image), RwImageGetHeight(image), 0, rwRASTERTYPENORMAL);

        if( raster )
        {
            /*
             * ...and initialize its pixels from those in the image...
             */
            RwRasterSetFromImage(raster, image);

            RwImageDestroy(image);

            return raster;
        }
    }

    return NULL;
}

/*
 *****************************************************************************
 */

#endif // USE_MFC