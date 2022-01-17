#include "alshadow.h"
#include "alshadow2.h"
#include "alcharacter.h"
#include "allight.h"

extern RpWorld	*World;

/*
 *  Shadow
 */
static RwCamera	*ShadowCamera = NULL;
static RwRaster	*ShadowCameraRaster = NULL;

static RwCamera	*ShadowIPCamera = NULL;
static RwRaster	*ShadowRasterAA = NULL;
static RwRaster	*ShadowRasterTemp = NULL;

static RwRaster	*ShadowRenderRaster = NULL;

static RwV2d		ShadowPreviewVerts[2] =
	{ {0.83f, 0.02f}, {0.98f, 0.17f} };

RwBool ShadowEnabled = FALSE;
static RwInt32	  ShadowResolutionIndex = 8;
static RwInt32	  ShadowBlur = 0;
static RwBool	   ShadowAA = FALSE;
static RwReal	   ShadowZoneRadius = 300.0f;
static RwReal	   ShadowStrength = 0.8f;
static RwBool	   ShadowFade = TRUE;

/*
 *  Misc
 */
static const RwV3d  Xaxis = { 1.0f, 0.0f, 0.0f };
static const RwV3d  Yaxis = { 0.0f, 1.0f, 0.0f };
static const RwV3d  Zaxis = { 0.0f, 0.0f, 1.0f };

/*
 ******************************************************************************
 */
RwBool SetupShadow()
{
	RwUInt32			res, res2;

	/*
	 *  Get resolution for shadow camera raster, and the other image
	 *  processing rasters. The latter are half the size if we use
	 *  an anti-alias step.
	 */
	res = 1 << ShadowResolutionIndex;

	if (ShadowAA)
	{
		res2 = res >> 1;
	}
	else
	{
		res2 = res;
	}

	/*
	 *  Create a shadow camera, and set the lighting direction, frustum
	 *  size, and frustum center.
	 */
	ShadowCamera = ShadowCameraCreate(res);
	if (!ShadowCamera)
	{
		return FALSE;
	}

	ShadowCameraSetLight(ShadowCamera, MainLight);
	ShadowCameraSetFrustum(ShadowCamera,
						   1.1f * CharacterTemplates[Characters[CurrentPlayer].nTemplate].ClumpBoundingSphere.radius);
	ShadowCameraSetCenter(ShadowCamera,
						  &Characters[CurrentPlayer].ClumpWorldBoundingSphere.center);

	RpWorldAddCamera(World, ShadowCamera);

	/*
	 *  Create a second camera for image processing.
	 */
	ShadowIPCamera = ShadowCameraCreate(res2);
	if (!ShadowIPCamera)
	{
		return FALSE;
	}

	/*
	 *  Create the main raster for rendering shadows into.
	 */
	ShadowCameraRaster = ShadowRasterCreate(res);
	if (!ShadowCameraRaster)
	{
		return FALSE;
	}

	RwCameraSetRaster(ShadowCamera, ShadowCameraRaster);

	/*
	 *  Create rasters for processed images (anti-alias and blur).
	 */
	if (ShadowAA)
	{
		ShadowRasterAA = ShadowRasterCreate(res2);
		if (!ShadowRasterAA)
		{
			return FALSE;
		}
	}

	ShadowRasterTemp = ShadowRasterCreate(res2);
	if (!ShadowRasterTemp)
	{
		return FALSE;
	}

	return TRUE;
}

/*
 ******************************************************************************
 */
void DestroyShadow()
{
	if (ShadowCamera)
	{
		RpWorldRemoveCamera(World, ShadowCamera);
		ShadowCameraDestroy(ShadowCamera);
		ShadowCamera = NULL;
	}

	if (ShadowIPCamera)
	{
		ShadowCameraDestroy(ShadowIPCamera);
		ShadowIPCamera = NULL;
	}

	if (ShadowCameraRaster)
	{
		ShadowRasterDestroy(ShadowCameraRaster);
		ShadowCameraRaster = NULL;
	}

	if (ShadowRasterAA)
	{
		ShadowRasterDestroy(ShadowRasterAA);
		ShadowRasterAA = NULL;
	}

	if (ShadowRasterTemp)
	{
		ShadowRasterDestroy(ShadowRasterTemp);
		ShadowRasterTemp = NULL;
	}

	return;
}

/*
 ******************************************************************************
 */
void UpdateShadow(RwReal deltaTime __RWUNUSED__, RpClump *clump)
{
	RwUInt32			dFlags, aFlags;
	RwInt32 index;

	for( index = 0; index < CharacterNumber; index++ )
	{
		/*
		 *  Track shadow camera along with the clump
		 */
		ShadowCameraSetCenter(ShadowCamera,
							  &Characters[index].ClumpWorldBoundingSphere.center);

		/* 
		 *  Disable lights 
		 */
		dFlags = RpLightGetFlags(MainLight);
		RpLightSetFlags(MainLight, 0);

		aFlags = RpLightGetFlags(AmbientLight);
		RpLightSetFlags(AmbientLight, 0);

		/* 
		 *  Render the clump into the shadow camera. This is a generic method
		 *  that will work for most things. Ideally, for best performance,
		 *  a special pipeline would be used to just render black polygons.
		 */
		ShadowCameraUpdate(ShadowCamera, clump);

		/*
		 *  Enable lights again
		 */
		RpLightSetFlags(MainLight, dFlags);
		RpLightSetFlags(AmbientLight, aFlags);

		/*
		 *  Anti-alias and set the raster to be used for shadow rendering. 
		 */
		if (ShadowAA)
		{
			ShadowRasterResample(ShadowRasterAA, ShadowCameraRaster,
								 ShadowIPCamera);

			ShadowRenderRaster = ShadowRasterAA;
		}
		else
		{
			ShadowRenderRaster = ShadowCameraRaster;
		}

		/*
		 *  Blur the shadow. Both anti-alias and blur may be omitted to
		 *  minimize the impact on performance, and the texture cache usage on PS2.
		 */
		if (ShadowBlur)
		{
			ShadowRasterBlur(ShadowRenderRaster, ShadowRasterTemp,
							 ShadowIPCamera, ShadowBlur);
		}
	}

	return;
}

/*
 ******************************************************************************
 */
RwBool RenderShadow()
{
	RpIntersection	  shadowZone;
	RwV3d			   boxDiag;
	RwV3d			   zoneVector;
	RwReal			  fadeDistance;
	RwInt32 index;

	for( index = 0; index < CharacterNumber; index++ )
	{
		/*
		 *  Render shadow raster preview.
		 */
		ShadowRasterRender(ShadowRenderRaster, ShadowPreviewVerts);

		/*
		 *  The ShadowRender() function requires an intersection
		 *  primitive for use in a collision test on the world to find shadow
		 *  triangles.
		 *
		 *  We start with a bounding box around the clump...
		 */
		shadowZone.type = rpINTERSECTBOX;

		boxDiag.x = boxDiag.y = boxDiag.z = CharacterTemplates[Characters[index].nTemplate].ClumpBoundingSphere.radius;

		RwV3dAdd(&shadowZone.t.box.sup,
				 &Characters[index].ClumpWorldBoundingSphere.center, &boxDiag);
		RwV3dSub(&shadowZone.t.box.inf,
				 &Characters[index].ClumpWorldBoundingSphere.center, &boxDiag);

		/*
		 *  ... and now extend it to cover the space out to the 
		 *  ShadowZoneRadius along the camera/light direction.
		 */
		RwV3dScale(&zoneVector,
				   RwMatrixGetAt(RwFrameGetMatrix
								 (RwCameraGetFrame(ShadowCamera))),
				   ShadowZoneRadius);

		if (zoneVector.x > 0.0f)
		{
			shadowZone.t.box.sup.x += zoneVector.x;
		}
		else
		{
			shadowZone.t.box.inf.x += zoneVector.x;
		}

		if (zoneVector.y > 0.0f)
		{
			shadowZone.t.box.sup.y += zoneVector.y;
		}
		else
		{
			shadowZone.t.box.inf.y += zoneVector.y;
		}

		if (zoneVector.z > 0.0f)
		{
			shadowZone.t.box.sup.z += zoneVector.z;
		}
		else
		{
			shadowZone.t.box.inf.z += zoneVector.z;
		}

		/*
		 *  Set the distance at which the shadow fades to nothing if 
		 *  fade-mode is selected (zero otherwise).
		 */
		fadeDistance = ShadowFade ? ShadowZoneRadius : 0.0f;

		/*
		 *  Render the shadow.
		 */
		ShadowRender(ShadowCamera, ShadowRenderRaster,
					 World, &shadowZone, ShadowStrength, fadeDistance);
	}

	return TRUE;
}

/*
 ******************************************************************************
 */
RwBool ResetShadow(RwBool justCheck)
{
	if (!justCheck)
	{
		/*
		 *  Destroy and recreate everything for new texture resolution
		 */
		DestroyShadow();
		SetupShadow();
	}

	return TRUE;
}

/*
 ******************************************************************************
 */
RwBool SceneMenuShadowBlurCallback( RwBool testEnable )
{
	if ( testEnable )
	{
#if defined( SOFTRAS_DRVMODEL_H )
		ShadowBlur = 0;
		return FALSE;
#endif /* defined( SOFTRAS_DRVMODEL_H ) */
	}

	return TRUE;
}
