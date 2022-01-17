
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

#include "camera.h"
#include "ptrdata.h"


/*
 *****************************************************************************
 */
void
CameraSize(RwCamera *camera, RwRect *rect, RwReal viewWindow, RwReal aspectRatio)
{
    if( camera )
    {
        RwVideoMode videoMode;
        static RwRect origSize = {0, 0, 0, 0};

        if( origSize.w == 0 || origSize.h == 0 )
        {
            origSize.x = origSize.y = 0;
            origSize.w = RwRasterGetWidth(RwCameraGetRaster(camera));
            origSize.h = RwRasterGetHeight(RwCameraGetRaster(camera));
        }

        RwEngineGetVideoModeInfo(&videoMode, RwEngineGetCurrentVideoMode());

        if( !rect )
        {
            static RwRect r;

            rect = &r;

            /* 
             * Rect not specified - reuse current values...
             */
            rect->w = RwRasterGetWidth(RwCameraGetRaster(camera));
            rect->h = RwRasterGetHeight(RwCameraGetRaster(camera));
            rect->x = rect->y = 0;
        }

        /* 
         * For full screen applications, resizing the camera just doesn't
         * make sense, use the video mode size...
         */
        if( videoMode.flags & rwVIDEOMODEEXCLUSIVE )
        {
            rect->x = rect->y = 0;
            rect->w = videoMode.width;
            rect->h = videoMode.height;
        }

        if( (rect->w > 0) && (rect->h > 0) )
        {
            RwV2d vw;
            RwRaster *raster = NULL;
            RwRaster *zRaster = NULL;

            /*
             * In OpenGl & D3D8 you don't need to care about the rasters
			 * of the cameras
             */
#if (defined(D3D8_DRVMODEL_H)) || (defined(D3D9_DRVMODEL_H)) || (defined(OPENGL_DRVMODEL_H))
            raster = RwCameraGetRaster(camera);
			zRaster = RwCameraGetZRaster(camera);

			raster->width = rect->w;
			raster->height= rect->h;

			zRaster->width = rect->w;
			zRaster->height= rect->h;

#else
            /*
             * Destroy rasters...
             */
            raster = RwCameraGetRaster(camera);
            if( raster )
            {
                RwRasterDestroy(raster);
                
                raster = NULL;
            }

            zRaster = RwCameraGetZRaster(camera);
            if( zRaster )
            {
                RwRasterDestroy(zRaster);
                
                zRaster = NULL;
            }

            /*
             * Create new rasters... 
             */
            raster = RwRasterCreate(rect->w, rect->h, 0, rwRASTERTYPECAMERA);
            zRaster = RwRasterCreate(rect->w, rect->h, 0, rwRASTERTYPEZBUFFER);

            if( raster && zRaster )
            {
                RwCameraSetRaster(camera, raster);
                RwCameraSetZRaster(camera, zRaster);
            }
            else
            {
                if( raster )
                {
                    RwRasterDestroy(raster);
                    
                    raster = NULL;
                }

                if( zRaster )
                {
                    RwRasterDestroy(zRaster);
                    
                    zRaster = NULL;
                }

                rect->w = origSize.w;
                rect->h = origSize.h;

                /* 
                 * Use default values... 
                 */
                raster =
                    RwRasterCreate(rect->w, rect->h, 0, rwRASTERTYPECAMERA);

                zRaster =
                    RwRasterCreate(rect->w, rect->h, 0, rwRASTERTYPEZBUFFER);

                RwCameraSetRaster(camera, raster);
                RwCameraSetZRaster(camera, zRaster);

                return;
            }
#endif

            /*
             * Figure out the view window... 
             */
            if( videoMode.flags & rwVIDEOMODEEXCLUSIVE )
            {
                /*
                 * Derive ratio from aspect ratio...
                 */
                vw.x = viewWindow;
                vw.y = viewWindow / aspectRatio;
            }
            else
            {

            /*
             * In OpenGl & D3D8 you don't need to care about the rasters
			 * of the cameras, just change the aspect ratio
             */
#if (defined(D3D8_DRVMODEL_H)) || (defined(D3D9_DRVMODEL_H)) || (defined(OPENGL_DRVMODEL_H))
#else
                rect->w = RwRasterGetWidth(RwCameraGetRaster(camera));
                rect->h = RwRasterGetHeight(RwCameraGetRaster(camera));
#endif
                /*
                 * Derive from pixel ratios... 
                 */
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

            RwCameraSetViewWindow(camera, &vw);
        }
    }

    return;
}


/*
 *****************************************************************************
 */
void
CameraDestroy(RwCamera *camera)
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
CameraCreate(RwInt32 width, RwInt32 height, RwBool zBuffer)
{
    RwCamera *camera;

    camera = RwCameraCreate();
    if( camera )
    {
        RwCameraSetFrame(camera, RwFrameCreate());

        RwCameraSetRaster(camera, 
            RwRasterCreate(width, height, 0, rwRASTERTYPECAMERA));

        if( zBuffer )
        {
            RwCameraSetZRaster(camera,
                RwRasterCreate(width, height, 0, rwRASTERTYPEZBUFFER));
        }

        /*
         * Check everything is valid...
         */
        if( RwCameraGetFrame(camera) && RwCameraGetRaster(camera) &&
            (!zBuffer || RwCameraGetZRaster(camera)) )
        {
            /*
             * Everything OK...
             */
            return camera;
        }
    }

    /*
     * Error - so clean up...
     */
    CameraDestroy(camera);

    return NULL;
}


/*
 *****************************************************************************
 */
void
CameraMove(RwCamera *camera, RwV3d *delta)
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
CameraPan(RwCamera *camera, const RwV3d *pos, RwReal angle)
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
CameraTilt(RwCamera *camera, const RwV3d *pos, RwReal angle)
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
CameraRotate(RwCamera *camera, const RwV3d *pos, RwReal angle)
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
CameraCreateCrossHair(void)
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
