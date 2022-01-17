#ifdef USE_API
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

#include "rwcore.h"
#include "ApBase.h"

#include "camera.h"
#include "ptrdata.h"
#include "magdebug.h"

#include "skeleton.h"

/*
 *****************************************************************************
 */
bool
AcuCamera::CameraSize(RwCamera *camera, RwRect *rect, RwReal viewWindow, RwReal aspectRatio)
{
    if( camera )
    {
        RwVideoMode videoMode;
        static RwRect origSize = {0, 0, 0, 0};

        if( origSize.w == 0 || origSize.h == 0 )
        {
            origSize.x = origSize.y = 0;

			RwRaster * pRaster = RwCameraGetRaster( camera );

			if( pRaster )
			{
				origSize.w = RwRasterGetWidth( pRaster );
				origSize.h = RwRasterGetHeight( pRaster );

				// origSize 값이 맛탱이 가서 아래의 작업도중 문제가 발생하는 경우가 있다.. 이 경우엔 이미 카메라가 맛탱이가 간 것이겠지만..
				// 대강 2048 * 1536 을 최대로 잡아두자.. 그 이상이 되거나 음수값이라면 래스터가 맛탱이 갔거나 카메라가 맛탱이 간거다.
				if( origSize.w > 2048 || origSize.h < 0 || origSize.h > 1536 || origSize.h < 0 )
				{
					return false;
				}
			}
			else
			{
				origSize.w = 0;
				origSize.h = 0;
			}
        }

		RwInt32	nCurrentVideoMode = RwEngineGetCurrentVideoMode();
		if( nCurrentVideoMode < 0 ) return false;

        RwEngineGetVideoModeInfo( &videoMode, nCurrentVideoMode );

        if( !rect )
        {
            static RwRect r;

            rect = &r;

			RwRaster * pRaster = RwCameraGetRaster(camera);

			if( pRaster )
			{
				rect->w = RwRasterGetWidth(RwCameraGetRaster(camera));
				rect->h = RwRasterGetHeight(RwCameraGetRaster(camera));
				rect->x = rect->y = 0;
			}
			else
			{
				rect->w = 0;
				rect->h = 0;
				rect->x = rect->y = 0;
			}
        }

        if( videoMode.flags & rwVIDEOMODEEXCLUSIVE )
        {
            rect->x = rect->y = 0;
            rect->w = videoMode.width;
            rect->h = videoMode.height;
        }

        if( (rect->w > 0) && (rect->h > 0) )
        {
            RwV2d vw;

			if( !AcuCamera::CameraCreate( rect->w, rect->h, TRUE, camera, TRUE ) )
			{
                rect->w = origSize.w;
                rect->h = origSize.h;

				if( !AcuCamera::CameraCreate( rect->w, rect->h, TRUE, camera, TRUE ) )
				{
					return false;
				}
			}

			if( videoMode.flags & rwVIDEOMODEEXCLUSIVE )
            {
                vw.x = viewWindow;
                vw.y = viewWindow / aspectRatio;
            }
            else
            {

#if (defined(D3D8_DRVMODEL_H)) || (defined(OPENGL_DRVMODEL_H))
#else
                rect->w = RwRasterGetWidth(RwCameraGetRaster(camera));
                rect->h = RwRasterGetHeight(RwCameraGetRaster(camera));
#endif

				if( rect->w > rect->h )
                {
                    vw.x = viewWindow;
                    vw.y = (rect->h * viewWindow) / rect->w;
                }
                else
                {
                    vw.x = (rect->w * viewWindow) / rect->h;
                    vw.y = viewWindow;
                }
            }
        }
    }

    return true;
}


/*
 *****************************************************************************
 */
void
AcuCamera::CameraDestroy( RwCamera* pCamera )
{
	if( !pCamera ) return;

	RwFrame* pFrame = RwCameraGetFrame( pCamera );
	if( !pFrame )
	{
        RwCameraSetFrame( pCamera, NULL );
        RwFrameDestroy( pFrame );
	}

	RwRaster* pRaster = RwCameraGetRaster( pCamera );
    if( pRaster )
    {
        RwCameraSetRaster( pCamera, NULL );
        RwRasterDestroy( pRaster );
    }

    pRaster = RwCameraGetZRaster( pCamera );
    if( pRaster )
    {
        RwCameraSetZRaster( pCamera, NULL );
        RwRasterDestroy( pRaster );
    }

    RwCameraDestroy( pCamera );
	return;
}


/*
 *****************************************************************************
 */
RwCamera *
AcuCamera::CameraCreate( RwInt32 width, RwInt32 height, RwBool zBuffer, RwCamera* pDestCamera, RwBool bResize )
{
	RwCamera* pCamera = NULL;
	if( !bResize )
	{
		pCamera = RwCameraCreate();
		if( !pCamera ) return NULL;

		RwFrame* pCameraFrame = RwFrameCreate();
		if( !pCameraFrame )
		{
			CameraDestroy( pCamera );
			return NULL;
		}

		RwCameraSetFrame( pCamera, pCameraFrame );
	}
	else
	{
		if( !pDestCamera )
		{
			return AcuCamera::CameraCreate( width, height, zBuffer );
		}
		else
		{
			pCamera = pDestCamera;
		}

		RwRaster* pRaster = RwCameraGetRaster( pCamera );
		if( pRaster )
		{
			RwCameraSetRaster( pCamera, NULL );
			RwRasterDestroy( pRaster );
		}

		pRaster = RwCameraGetZRaster( pCamera );
		if( pRaster )
		{
			RwCameraSetZRaster( pCamera, NULL );
			RwRasterDestroy( pRaster );
		}
	}

	RwRaster* pCameraRaster = RwRasterCreate( width, height, 0, rwRASTERTYPECAMERA );
	if( !pCameraRaster )
	{
		pCameraRaster = RwRasterCreate( width, height, 0, rwRASTERTYPECAMERATEXTURE );
		if( !pCameraRaster )
		{
			CameraDestroy( pCamera );
			return NULL;
		}
	}

	RwCameraSetRaster( pCamera, pCameraRaster );
	if( zBuffer )
	{
		RwRaster* pCameraZRaster = RwRasterCreate( width, height, 0, rwRASTERTYPEZBUFFER );
		if( !pCameraZRaster )
		{
			CameraDestroy( pCamera );
			return NULL;
		}

		RwCameraSetZRaster( pCamera, pCameraZRaster );
	}

	return pCamera;
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

#endif // USE_API