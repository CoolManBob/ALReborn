/****************************************************************************
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

/****************************************************************************
 *
 * shadow.c
 *
 * Copyright (C) 2001 Criterion Technologies.
 *
 * Original author: Matt Thorman
 * Reviewed by:
 *
 * Purpose: Example of parallel-projection shadow rendering using
 *		  3D immediate mode.
 *
 ****************************************************************************/

#include "alshadow2.h"

#ifdef SKY
extern RwBool	   skyTexCacheRasterLock(RwRaster * rpRas,
										  RwBool bLocked);
#endif

/*
 *  Number of immediate mode triangles in last shadow
 */
RwUInt32			ShadowNumTriangles = 0;

/* 
 *  3D Immediate mode buffer
 */
#define IM3DBUFFERSIZE  (500*3)
static RwIm3DVertex Im3DBuffer[IM3DBUFFERSIZE];
static RwUInt32	 Im3DBufferCnt = 0;

/*
 *  The anti-alias and blur effects are sensitive to alignment between
 *  pixels and texels. This is platform dependent.
 */
#if defined(OPENGL_DRVMODEL_H) | defined(GCN_DRVMODEL_H)
#define TEXELOFFSET 0.0f
#else
#define TEXELOFFSET 0.5f
#endif

/*
 ******************************************************************************
 */
static RwBool
Im2DRenderQuad(RwReal x1, RwReal y1, RwReal x2, RwReal y2,
			   RwReal z, RwReal recipCamZ, RwReal uvOffset)
{
	RwIm2DVertex		vx[4];

	/*
	 *  Render an opaque white 2D quad at the given coordinates and
	 *  spanning a whole texture.
	 */

	RwIm2DVertexSetScreenX(&vx[0], x1);
	RwIm2DVertexSetScreenY(&vx[0], y1);
	RwIm2DVertexSetScreenZ(&vx[0], z);
	RwIm2DVertexSetIntRGBA(&vx[0], 255, 255, 255, 255);
	RwIm2DVertexSetRecipCameraZ(&vx[0], recipCamZ);
	RwIm2DVertexSetU(&vx[0], uvOffset, recipCamZ);
	RwIm2DVertexSetV(&vx[0], uvOffset, recipCamZ);

	RwIm2DVertexSetScreenX(&vx[1], x1);
	RwIm2DVertexSetScreenY(&vx[1], y2);
	RwIm2DVertexSetScreenZ(&vx[1], z);
	RwIm2DVertexSetIntRGBA(&vx[1], 255, 255, 255, 255);
	RwIm2DVertexSetRecipCameraZ(&vx[1], recipCamZ);
	RwIm2DVertexSetU(&vx[1], uvOffset, recipCamZ);
	RwIm2DVertexSetV(&vx[1], 1.0f + uvOffset, recipCamZ);

	RwIm2DVertexSetScreenX(&vx[2], x2);
	RwIm2DVertexSetScreenY(&vx[2], y1);
	RwIm2DVertexSetScreenZ(&vx[2], z);
	RwIm2DVertexSetIntRGBA(&vx[2], 255, 255, 255, 255);
	RwIm2DVertexSetRecipCameraZ(&vx[2], recipCamZ);
	RwIm2DVertexSetU(&vx[2], 1.0f + uvOffset, recipCamZ);
	RwIm2DVertexSetV(&vx[2], uvOffset, recipCamZ);

	RwIm2DVertexSetScreenX(&vx[3], x2);
	RwIm2DVertexSetScreenY(&vx[3], y2);
	RwIm2DVertexSetScreenZ(&vx[3], z);
	RwIm2DVertexSetIntRGBA(&vx[3], 255, 255, 255, 255);
	RwIm2DVertexSetRecipCameraZ(&vx[3], recipCamZ);
	RwIm2DVertexSetU(&vx[3], 1.0f + uvOffset, recipCamZ);
	RwIm2DVertexSetV(&vx[3], 1.0f + uvOffset, recipCamZ);

	RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vx, 4);

	return TRUE;
}

/*
 ******************************************************************************
 */
#if ( !(defined(SKY) || defined( SOFTRAS_DRVMODEL_H )) )

static void
InvertRaster(RwCamera * camera)
{
	RwIm2DVertex		vx[4];
	RwReal			  crw, crh;
	RwRaster		   *camRas;
	RwReal			  recipZ;

	/*
	 *  Invert the RGB of the frame buffer using two opaque white immediate
	 *  mode triangles covering the entire display, with a blend value of
	 *  1 - destination color.
	 *
	 *  This is applied after rendering the clump in the shadow camera so 
	 *  that we end up with a white character on a black background.
	 *
	 *  This is not used on PS2 as the shadow effect uses the alpha channel
	 *  instead. See ShadowCameraUpdate().
	 */

	camRas = RwCameraGetRaster(camera);
	crw = (RwReal) RwRasterGetWidth(camRas);
	crh = (RwReal) RwRasterGetHeight(camRas);

	recipZ = 1.0f / RwCameraGetNearClipPlane(camera);

	RwIm2DVertexSetScreenX(&vx[0], 0.0f);
	RwIm2DVertexSetScreenY(&vx[0], 0.0f);
	RwIm2DVertexSetScreenZ(&vx[0], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetRecipCameraZ(&vx[0], recipZ);
	RwIm2DVertexSetIntRGBA(&vx[0], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&vx[1], 0.0f);
	RwIm2DVertexSetScreenY(&vx[1], crh);
	RwIm2DVertexSetScreenZ(&vx[1], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetRecipCameraZ(&vx[1], recipZ);
	RwIm2DVertexSetIntRGBA(&vx[1], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&vx[2], crw);
	RwIm2DVertexSetScreenY(&vx[2], 0.0f);
	RwIm2DVertexSetScreenZ(&vx[2], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetRecipCameraZ(&vx[2], recipZ);
	RwIm2DVertexSetIntRGBA(&vx[2], 255, 255, 255, 255);

	RwIm2DVertexSetScreenX(&vx[3], crw);
	RwIm2DVertexSetScreenY(&vx[3], crh);
	RwIm2DVertexSetScreenZ(&vx[3], RwIm2DGetNearScreenZ());
	RwIm2DVertexSetRecipCameraZ(&vx[3], recipZ);
	RwIm2DVertexSetIntRGBA(&vx[3], 255, 255, 255, 255);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) FALSE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *) NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *) TRUE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *) rwBLENDZERO);
	RwRenderStateSet(rwRENDERSTATESRCBLEND,
					 (void *) rwBLENDINVDESTCOLOR);

	RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vx, 4);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) TRUE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND,
					 (void *) rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) rwBLENDSRCALPHA);
}

#endif /* ( !(defined(SKY) || defined( SOFTRAS_DRVMODEL_H )) ) */

/*
 ******************************************************************************
 */
void
ShadowCameraDestroy(RwCamera * shadowCamera)
{
	if (shadowCamera)
	{
		RwRaster		   *raster;
		RwFrame			*frame;

		frame = RwCameraGetFrame(shadowCamera);
		if (frame)
		{
			RwCameraSetFrame(shadowCamera, NULL);
			RwFrameDestroy(frame);
		}

		raster = RwCameraGetZRaster(shadowCamera);
		if (raster)
		{
			RwCameraSetZRaster(shadowCamera, NULL);
			RwRasterDestroy(raster);
		}

		raster = RwCameraGetRaster(shadowCamera);
		if (raster)
		{
			RwCameraSetRaster(shadowCamera, NULL);
			/* Destroyed externally */
		}

		RwCameraDestroy(shadowCamera);
	}

	return;
}

/*
 ******************************************************************************
 */
RwCamera		   *
ShadowCameraCreate(RwInt32 size)
{
	RwCamera		   *shadowCamera;

	/*
	 *  Create a parallel projection camera for shadows under directional
	 *  lighting. 
	 *
	 *  The raster should be created externally using ShadowRasterCreate(). 
	 *  Lighting direction and frustum can be set with:
	 *	  ShadowCameraSetLight()
	 *	  ShadowCameraSetFrustum()
	 *	  ShadowCameraSetCenter()
	 */

	shadowCamera = RwCameraCreate();
	if (shadowCamera)
	{
		RwCameraSetFrame(shadowCamera, RwFrameCreate());

		if (RwCameraGetFrame(shadowCamera))
		{
			RwRaster		   *zRaster;

			zRaster =
				RwRasterCreate(size, size, 0, rwRASTERTYPEZBUFFER);
			if (zRaster)
			{
				RwCameraSetZRaster(shadowCamera, zRaster);
				RwCameraSetProjection(shadowCamera, rwPARALLEL);
#ifdef RWLOGO
				/*
				 *  We don't want a logo in the shadow texture
				 */
				RpLogoSetState(shadowCamera, FALSE);
#endif /* RWLOGO */

				return (shadowCamera);
			}
		}
	}

	/*
	 *  An error occurred
	 */
	ShadowCameraDestroy(shadowCamera);

	return (NULL);
}

/*
 ******************************************************************************
 */
RwCamera		   *
ShadowCameraSetFrustum(RwCamera * shadowCamera, RwReal objectRadius)
{
	RwV2d			   vw;

	/*
	 *  Set the frustum (a cube) to enclose an object of the given radius. 
	 *  The frustum may be centered on the object using 
	 *  ShadowCameraSetCenter().
	 */

	RwCameraSetFarClipPlane(shadowCamera, 2.0f * objectRadius);
	RwCameraSetNearClipPlane(shadowCamera, 0.001f * objectRadius);

	vw.x = objectRadius;
	vw.y = objectRadius;
	RwCameraSetViewWindow(shadowCamera, &vw);

	return shadowCamera;
}

/*
 ******************************************************************************
 */
RwCamera		   *
ShadowCameraSetLight(RwCamera * camera, RpLight * light)
{
	RwFrame			*camFrame = RwCameraGetFrame(camera);
	RwFrame			*lightFrame = RpLightGetFrame(light);
	RwMatrix		   *camMatrix = RwFrameGetMatrix(camFrame);
	RwMatrix		   *lightMatrix = RwFrameGetMatrix(lightFrame);

	/*
	 *  Set the shadow camera frame to look along the direction of the light.
	 */

	*RwMatrixGetRight(camMatrix) = *RwMatrixGetRight(lightMatrix);
	*RwMatrixGetUp(camMatrix) = *RwMatrixGetUp(lightMatrix);
	*RwMatrixGetAt(camMatrix) = *RwMatrixGetAt(lightMatrix);

	RwMatrixUpdate(camMatrix);
	RwFrameUpdateObjects(camFrame);

	return camera;
}

/*
 ******************************************************************************
 */
RwCamera		   *
ShadowCameraSetCenter(RwCamera * camera, RwV3d * center)
{
	RwFrame			*camFrame = RwCameraGetFrame(camera);
	RwMatrix		   *camMatrix = RwFrameGetMatrix(camFrame);
	RwV3d			  *camPos = RwMatrixGetPos(camMatrix);

	/*
	 *  Set the center of the shadow camera frustum volume.
	 */

	*camPos = *center;
	RwV3dIncrementScaled(camPos, RwMatrixGetAt(camMatrix),
						 -0.5f * RwCameraGetFarClipPlane(camera));
	RwMatrixUpdate(camMatrix);
	RwFrameUpdateObjects(camFrame);

	return camera;
}

/*
 ******************************************************************************
 */
RwCamera		   *
ShadowCameraUpdate(RwCamera * shadowCamera, RpClump * clump)
{
	RwRGBA			  bgColor = { 255, 255, 255, 0 };

	/*
	 *  Render the clump into the shadow camera. All lights should be
	 *  disabled prior to this so that the object is rendered black.
	 *
	 *  The details are platform dependent due to differences in the
	 *  available blending modes.
	 *
	 *  On PS2, we just work with alpha. This is initially set to zero
	 *  and will become 255 in the object shadow region once the clump
	 *  is rendered. This may be used directly as a blending factor for
	 *  the shadow and may be scaled by alpha in the shadow triangle
	 *  vertices to achieve semi-transparent shadows.
	 *
	 *  On other platforms, the RGB channels are used, which are initially
	 *  set to white. After rendering, the shadow region will be black.
	 *  This may be used directly for black shadows. However, to support
	 *  semi-transparent shadows, we need to invert the buffer to white
	 *  object on black background. This may be modulated by shadow vertex
	 *  color when blending the shadow.
	 */

	/*
	 *  Clear to white background with alpha = 0.
	 */
	RwCameraClear(shadowCamera, &bgColor,
				  rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

	/* 
	 *  Render the clump
	 */
	if (RwCameraBeginUpdate(shadowCamera))
	{
		RpClumpRender(clump);

#if ( !(defined(SKY) || defined( SOFTRAS_DRVMODEL_H )) )
		InvertRaster(shadowCamera);
#endif /* ( !(defined(SKY) || defined( SOFTRAS_DRVMODEL_H )) ) */

		RwCameraEndUpdate(shadowCamera);

#ifdef DOLPHIN
		RwGameCubeCameraTextureFlush(RwCameraGetRaster(shadowCamera));
#endif
	}

	return shadowCamera;
}

/*
 ******************************************************************************
 */
RwRaster		   *
ShadowRasterCreate(RwUInt32 size)
{
	RwRaster		   *raster;

	/*
	 *  Create a square shadow raster of the given dimension.
	 */

#ifdef SKY

	/*
	 *  On PS2, we use the alpha channel and require a full 32bit RGBA buffer
	 *  for the soft shadow effects. For a hard shadow, a 16-bit 
	 *  rwRASTERFORMAT1555 would be sufficient and would save video memory.
	 */
	raster = RwRasterCreate(size, size, 32,
							rwRASTERTYPECAMERATEXTURE |
							rwRASTERFORMAT8888);

	if (raster)
	{
		skyTexCacheRasterLock(raster, TRUE);
	}

#else

	/*
	 *  Use camera texture raster of default type.
	 */
	raster = RwRasterCreate(size, size, 0, rwRASTERTYPECAMERATEXTURE);

#endif /* SKY */

	return raster;
}

/*
 ******************************************************************************
 */
void
ShadowRasterDestroy(RwRaster * raster)
{

#ifdef SKY
	skyTexCacheRasterLock(raster, FALSE);
#endif

	RwRasterDestroy(raster);

	return;
}

/*
 ******************************************************************************
 */
RwBool
ShadowRasterResample(RwRaster * destRaster,
					 RwRaster * sourceRaster, RwCamera * camera)
{
	RwReal			  size;
	RwReal			  uvOffset;
	RwReal			  recipCamZ;

	/*
	 *  This resamples the sourceRaster into the destRaster which should
	 *  be half the size in both dimensions, using the sourceRaster as a
	 *  texture and the destRaster as a rendering target. Bilinear filtering
	 *  achieves an anti-alias effect by averaging four pixels down into one.
	 *
	 *  A 'dummy' camera is required for the rendering which should
	 *  have a z-raster of the same size as the destination raster.
	 */

	size = (RwReal) RwRasterGetWidth(destRaster);
	uvOffset = TEXELOFFSET / size;
	recipCamZ = 1.0f / RwCameraGetFarClipPlane(camera);

	RwCameraSetRaster(camera, destRaster);
	if (RwCameraBeginUpdate(camera))
	{
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) rwBLENDONE);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *) rwBLENDZERO);
		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) FALSE);
		RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,
						 (void *) rwFILTERLINEAR);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER,
						 (void *) sourceRaster);

		Im2DRenderQuad(0.0f, 0.0f, size, size,
					   RwIm2DGetFarScreenZ(), recipCamZ, uvOffset);

		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) TRUE);
		RwRenderStateSet(rwRENDERSTATESRCBLEND,
						 (void *) rwBLENDSRCALPHA);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND,
						 (void *) rwBLENDINVSRCALPHA);

		RwCameraEndUpdate(camera);

#ifdef DOLPHIN
		RwGameCubeCameraTextureFlush(RwCameraGetRaster(camera));
#endif
	}

	RwCameraSetRaster(camera, NULL);

	return TRUE;
}

/*
 ******************************************************************************
 */
RwBool
ShadowRasterBlur(RwRaster * shadowRaster,
				 RwRaster * tempRaster,
				 RwCamera * camera, RwUInt32 numPass)
{
	RwUInt32			iPass;
	RwReal			  size;
	RwReal			  uvStep;
	RwReal			  recipCamZ;

	/*
	 *  Blur the image in shadowRaster using tempRaster as a temporary
	 *  buffer (both should be the same size and created with 
	 *  ShadowRasterCreate). A 2D immediate mode rendering method is used
	 *  which requires a 'dummy' camera to work with. This should have an
	 *  appropriately sized Z-raster.
	 *
	 *  The technique used is one that will work on PS2 where methods of
	 *  blurring the alpha channel are limited, but will also work on other
	 *  platforms where color channels are used.
	 *
	 *  It works by effectively blitting one raster into another but with
	 *  half a pixel offset in U and V so that bilinear filtering causes
	 *  each pixel to be an average of four from the source. This is
	 *  repeated with the opposite UV offset to increase the blur and
	 *  leave the texture with no net displacement. The overall effect is
	 *  equivalent to applying a 3x3 convolution matrix of
	 *
	 *	  1 2 1
	 *	  2 4 2
	 *	  1 2 1
	 */

	size = (RwReal) RwRasterGetWidth(shadowRaster);
	uvStep = 1.0f / size;
	recipCamZ = 1.0f / RwCameraGetFarClipPlane(camera);

	for (iPass = 0; iPass < numPass; iPass++)
	{
		RwCameraSetRaster(camera, tempRaster);
		if (RwCameraBeginUpdate(camera))
		{
			RwRenderStateSet(rwRENDERSTATESRCBLEND,
							 (void *) rwBLENDONE);
			RwRenderStateSet(rwRENDERSTATEDESTBLEND,
							 (void *) rwBLENDZERO);
			RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) FALSE);
			RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,
							 (void *) rwFILTERLINEAR);
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER,
							 (void *) shadowRaster);

			Im2DRenderQuad(0.0f, 0.0f, size, size,
						   RwIm2DGetFarScreenZ(), recipCamZ,
						   (TEXELOFFSET + 0.5f) * uvStep);

			RwCameraEndUpdate(camera);

#ifdef DOLPHIN
			RwGameCubeCameraTextureFlush(RwCameraGetRaster(camera));
#endif
		}

		RwCameraSetRaster(camera, shadowRaster);
		if (RwCameraBeginUpdate(camera))
		{
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER,
							 (void *) tempRaster);

			Im2DRenderQuad(0.0f, 0.0f, size, size,
						   RwIm2DGetFarScreenZ(), recipCamZ,
						   (TEXELOFFSET - 0.5f) * uvStep);

			RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) TRUE);
			RwRenderStateSet(rwRENDERSTATESRCBLEND,
							 (void *) rwBLENDSRCALPHA);
			RwRenderStateSet(rwRENDERSTATEDESTBLEND,
							 (void *) rwBLENDINVSRCALPHA);

			RwCameraEndUpdate(camera);

#ifdef DOLPHIN
			RwGameCubeCameraTextureFlush(RwCameraGetRaster(camera));
#endif
		}
	}

	RwCameraSetRaster(camera, NULL);

	return TRUE;
}

/*
 ******************************************************************************
 */
RwRaster		   *
ShadowRasterRender(RwRaster * shadowRaster, RwV2d * vx)
{
	RwCamera		   *camera = RwCameraGetCurrentCamera();
	RwRaster		   *camRas;
	RwReal			  crw, crh;
	RwReal			  recipCamZ;

	/*
	 *  Render a preview of the shadow raster using the given coordinates
	 *  for the upper-left and lower-right corners. These should be 
	 *  specified as a fraction of the display width and height.
	 */

	camRas = RwCameraGetRaster(camera);
	crw = (RwReal) RwRasterGetWidth(camRas);
	crh = (RwReal) RwRasterGetHeight(camRas);

	recipCamZ = 1.0f / RwCameraGetNearClipPlane(camera);

	/*
	 *  Set renderstate
	 */
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *) rwBLENDZERO);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,
					 (void *) rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *) shadowRaster);

	Im2DRenderQuad(vx[0].x * crw, vx[0].y * crh, vx[1].x * crw,
				   vx[1].y * crh, RwIm2DGetNearScreenZ(), recipCamZ,
				   0.0f);

	/*
	 *  Restore renderstate
	 */
	RwRenderStateSet(rwRENDERSTATEDESTBLEND,
					 (void *) rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) rwBLENDSRCALPHA);

	return shadowRaster;
}

/*
 ******************************************************************************
 */
typedef struct _ProjectionParam
{
	RwV3d			   at;	 /* Camera at vector */
	RwMatrix			invMatrix; /* Transforms to shadow camera space */
	RwUInt8			 shadowValue; /* Shadow opacity value */
	RwBool			  fade;   /* Shadow fades with distance */
	RwUInt32			numIm3DBatch; /* Number of buffer flushes */

}
ProjectionParam;

static RpCollisionTriangle *
ShadowRenderTriangleCB(RpIntersection * intersection __RWUNUSED__,
					   RpWorldSector * worldSector __RWUNUSED__,
					   RpCollisionTriangle * collTriangle,
					   RwReal distance __RWUNUSED__, void *data)
{
	ProjectionParam	*param = (ProjectionParam *) data;
	RwV3d			   vIn[3];
	RwV3d			   vShad[3];
	RwIm3DVertex	   *imv;

	/*
	 *  Reject backfacing triangles
	 */
	if (RwV3dDotProduct(&collTriangle->normal, &param->at) > 0.0f)
	{
		return collTriangle;
	}

	/* 
	 *  Map the vertices to shadow U and V, and normalized z depth.
	 */
	vIn[0] = *(collTriangle->vertices[0]);
	vIn[1] = *(collTriangle->vertices[1]);
	vIn[2] = *(collTriangle->vertices[2]);

	RwV3dTransformPoints(&vShad[0], &vIn[0], 3, &param->invMatrix);

	/*
	 *  Reject triangles behind the camera (z test). Note that any world 
	 *  triangles lying in front of the camera but before the object may 
	 *  have a shadow applied. To minimise such artefacts, this test could 
	 *  be modified to use a specific value rather than 0.0f, perhaps
	 *  to reject triangles behind the center plane of the object.
	 *
	 *  Reject triangles that lie entirely outside the shadow texture range
	 *  (x,y test).
	 */
	if (((vShad[0].z < 0.0f) && (vShad[1].z < 0.0f)
		 && (vShad[2].z < 0.0f)) || ((vShad[0].x < 0.0f)
									 && (vShad[1].x < 0.0f)
									 && (vShad[2].x < 0.0f))
		|| ((vShad[0].x > 1.0f) && (vShad[1].x > 1.0f)
			&& (vShad[2].x > 1.0f)) || ((vShad[0].y < 0.0f)
										&& (vShad[1].y < 0.0f)
										&& (vShad[2].y < 0.0f))
		|| ((vShad[0].y > 1.0f) && (vShad[1].y > 1.0f)
			&& (vShad[2].y > 1.0f)))
	{
		return collTriangle;
	}

	/*
	 *  If the buffer is full, we need to render the buffered triangles 
	 *  before we add this one.
	 */
	if (Im3DBufferCnt > (IM3DBUFFERSIZE - 3))
	{
		if (RwIm3DTransform(Im3DBuffer, Im3DBufferCnt, NULL,
							rwIM3D_VERTEXUV | rwIM3D_VERTEXXYZ |
							rwIM3D_VERTEXRGBA))
		{
			RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
			RwIm3DEnd();
		}

		param->numIm3DBatch++;
		Im3DBufferCnt = 0;
	}

	/*
	 *  Set the immediate mode vertices for this triangle
	 */
	imv = Im3DBuffer + Im3DBufferCnt;

	RwIm3DVertexSetPos(imv, vIn[0].x, vIn[0].y, vIn[0].z);
	RwIm3DVertexSetPos(imv + 1, vIn[1].x, vIn[1].y, vIn[1].z);
	RwIm3DVertexSetPos(imv + 2, vIn[2].x, vIn[2].y, vIn[2].z);

	RwIm3DVertexSetU(imv, vShad[0].x);
	RwIm3DVertexSetU(imv + 1, vShad[1].x);
	RwIm3DVertexSetU(imv + 2, vShad[2].x);

	RwIm3DVertexSetV(imv, vShad[0].y);
	RwIm3DVertexSetV(imv + 1, vShad[1].y);
	RwIm3DVertexSetV(imv + 2, vShad[2].y);

	/*
	 *  Do we fade out the shadow with distance?
	 */
	if (param->fade)
	{
		RwReal			  fadeVal;
		RwUInt8			 val;

		fadeVal = 1.0f - vShad[0].z * vShad[0].z;
		val =
			(fadeVal <
			 0.0f) ? 0 : (RwUInt8) (fadeVal * param->shadowValue);
		RwIm3DVertexSetRGBA(imv, val, val, val, val);

		fadeVal = 1.0f - vShad[1].z * vShad[1].z;
		val =
			(fadeVal <
			 0.0f) ? 0 : (RwUInt8) (fadeVal * param->shadowValue);
		RwIm3DVertexSetRGBA(imv + 1, val, val, val, val);

		fadeVal = 1.0f - vShad[2].z * vShad[2].z;
		val =
			(fadeVal <
			 0.0f) ? 0 : (RwUInt8) (fadeVal * param->shadowValue);
		RwIm3DVertexSetRGBA(imv + 2, val, val, val, val);
	}
	else
	{
		RwUInt8			 val = param->shadowValue;

		RwIm3DVertexSetRGBA(imv, val, val, val, val);
		RwIm3DVertexSetRGBA(imv + 1, val, val, val, val);
		RwIm3DVertexSetRGBA(imv + 2, val, val, val, val);
	}

	/*
	 *  Update buffer position 
	 */
	Im3DBufferCnt += 3;

	return collTriangle;
}

/*
 ******************************************************************************
 */
RwBool
ShadowRender(RwCamera * shadowCamera,
			 RwRaster * shadowRaster,
			 RpWorld * world,
			 RpIntersection * shadowZone,
			 RwReal shadowStrength, RwReal fadeDist)
{
	ProjectionParam	 param;
	RwMatrix		   *shadowMatrix;
	RwReal			  radius;
	RwV3d			   scl, tr;

	/*
	 *  Render a shadow in the given world, parallel projected from the
	 *  given shadow camera and raster.
	 *
	 *  Only the triangles in the given shadowZone intersection region are
	 *  considered. A world collision test is performed using this intersection
	 *  primitive, and any intersected triangles which lie within the shadow
	 *  texture region are use to construct an array of 3D immediate mode
	 *  triangles for drawing the shadow.
	 *
	 *  The shadowStrength sets the opacity of the shadow and should be
	 *  in the range -1 to 1. Negative values produce an inverted shadow
	 *  which clearly shows the full set of immediate mode triangles being
	 *  rendered.
	 *
	 *  If the fadeDist is greater than zero, then the shadow will be
	 *  faded out to zero at this distance from the center of the shadow 
	 *  camera. This can provide both a natural looking effect, and also hide
	 *  the extent of the shadow zone, if the shadow falls outside of it.
	 *
	 *  This function should be called within a begin/end camera update.
	 */

	/* 
	 *  Set renderstate. Use clamping for the texture address mode so that
	 *  we don't get a repeating shadow.
	 */
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *) shadowRaster);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,
					 (void *) rwTEXTUREADDRESSCLAMP);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *) TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,
					 (void *) rwFILTERLINEAR);

	/*
	 *  Set blending. As explained in ShadowCameraUpdate(), we use alpha
	 *  on PS2 and RGB on other platforms to scale the current values
	 *  in the frame buffer, thus achieving a shadowing effect.
	 */
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) rwBLENDZERO);

	if (shadowStrength < 0.0f)
	{
		/*
		 *  Do an inverse shadow
		 */
		shadowStrength = -shadowStrength;
#ifdef SKY
		RwRenderStateSet(rwRENDERSTATEDESTBLEND,
						 (void *) rwBLENDSRCALPHA);
#else /* SKY */
		RwRenderStateSet(rwRENDERSTATEDESTBLEND,
						 (void *) rwBLENDSRCCOLOR);
#endif /* SKY */
	}
	else
	{
#ifdef SKY
		RwRenderStateSet(rwRENDERSTATEDESTBLEND,
						 (void *) rwBLENDINVSRCALPHA);
#else /* SKY */
		RwRenderStateSet(rwRENDERSTATEDESTBLEND,
						 (void *) rwBLENDINVSRCCOLOR);
#endif /* SKY */
	}

	/*
	 *  We now construct a matrix to map world space vertices
	 *  directly to shadow texture u and v. The z coordinate after
	 *  tranformation gives the distance from the shadow camera plane
	 *  normalized to the fade-out distance.
	 */
	shadowMatrix = RwFrameGetMatrix(RwCameraGetFrame(shadowCamera));
	param.at = *RwMatrixGetAt(shadowMatrix);
	radius = RwCameraGetViewWindow(shadowCamera)->x;

	RwMatrixInvert(&param.invMatrix, shadowMatrix);

	scl.x = scl.y = -0.5f / radius;
	scl.z = 1.0f / (fadeDist + radius);
	RwMatrixScale(&param.invMatrix, &scl, rwCOMBINEPOSTCONCAT);

	tr.x = tr.y = 0.5f;
	tr.z = 0.0f;
	RwMatrixTranslate(&param.invMatrix, &tr, rwCOMBINEPOSTCONCAT);

	/*
	 *  Set up remaining parameters for the callback then use an
	 *  intersection test to find all triangles in the zone.
	 */
	param.fade = (fadeDist > 0.0f) ? TRUE : FALSE;
	param.shadowValue = (RwUInt8) (shadowStrength * 255);
	param.numIm3DBatch = 0;

	Im3DBufferCnt = 0;

	RpCollisionWorldForAllIntersections(world, shadowZone,
										ShadowRenderTriangleCB,
										(void *) &param);

	ShadowNumTriangles =
		(IM3DBUFFERSIZE * param.numIm3DBatch + Im3DBufferCnt) / 3;

	/*
	 *  Render any triangles in the buffer
	 */
	if (Im3DBufferCnt > 0)
	{
		if (RwIm3DTransform(Im3DBuffer, Im3DBufferCnt, NULL,
							rwIM3D_VERTEXUV | rwIM3D_VERTEXXYZ |
							rwIM3D_VERTEXRGBA))
		{
			RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
			RwIm3DEnd();
		}

		Im3DBufferCnt = 0;
	}

	/*
	 *  Reset the render state
	 */
	RwRenderStateSet(rwRENDERSTATEDESTBLEND,
					 (void *) rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) rwBLENDSRCALPHA);

	return TRUE;
}
