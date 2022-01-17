
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
 * Copyright (c) 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *																		 
 * pick.c
 *
 * Copyright (C) 2000 Criterion Technologies.
 *
 * Original author: Matt Reynolds.
 * Reviewed by: John Irwin.
 *																		 
 * Purpose: RW3 atomic picking example, showing how atomics can be picked 
 *		  either by their bounding spheres or triangles.
 *					   
 ****************************************************************************/

#include "headers.h"
#include "rtpick.h"

#include "alpick.h"

extern RwCamera *Camera;
extern RpWorld *World;

typedef struct
{
		RpIntersection intersection;
		RpAtomic *pickedAtomic;
		RwReal minDistance;
		RwV3d position;
}
AtomicIntersectParams;

/*
 * Number of points used in drawing a circle... 
 */
#define NUMPOINTS (100)


/*
 *****************************************************************************
 */
static RpCollisionTriangle * 
TriangleIntersectionCallback(RpIntersection *intersection __RWUNUSED__,
							 RpCollisionTriangle *triangle, 
							 RwReal distance,
							 void *data)
{
	RwReal *minDistance = (RwReal *)data;

	/*
	 * The intersection distance is the distance to the point of 
	 * intersection in the collision triangle from the start of the line,
	 * normalized to the length of the line. Compare this with the current
	 * minimum intersection distance...
	 */
	if( distance < *minDistance )
	{
		*minDistance = distance;
	}

	return triangle;
}


static RpAtomic * 
AtomicIntersectLine1(RpIntersection *intersection __RWUNUSED__,
					 RpWorldSector *sector __RWUNUSED__,
					 RpAtomic *atomic, 
					 RwReal distance __RWUNUSED__,
					 void *data)
{
	/*
	 * This callback is executed for each atomic bounding-sphere intersected. 
	 * For atomics that span more than one world sector, we get a single 
	 * callback for the world sector that is nearest to the start of the
	 * intersecting line.
	 * 
	 * Given that we have a bounding-sphere intersection, test the line 
	 * for intersections with the atomic's triangles...
	 */ 
	AtomicIntersectParams *intersectParams;
	RwReal oldDistance;

	intersectParams = (AtomicIntersectParams *)data;

	oldDistance = intersectParams->minDistance;

	RpAtomicForAllIntersections(atomic, &intersectParams->intersection, 
								TriangleIntersectionCallback, &intersectParams->minDistance);
	
	if( intersectParams->minDistance < oldDistance )
	{
		intersectParams->pickedAtomic = atomic;
	}

	return atomic;
}


/*
 *****************************************************************************
 */
static RpAtomic * 
AtomicIntersectLine2(RpIntersection *intersection __RWUNUSED__,
					 RpWorldSector *sector __RWUNUSED__,
					 RpAtomic *atomic, 
					 RwReal distance,
					 void *data)
{
	/*
	 * This callback is executed for each atomic bounding-sphere intersected. 
	 * For atomics that span more than one world sector, we get a single 
	 * callback for the world sector that is nearest to the start of the
	 * intersecting line...
	 */ 
	AtomicIntersectParams *intersectParams;

	intersectParams = (AtomicIntersectParams *)data;

	/*
	 * The intersection distance is the distance of the atomic’s 
	 * bounding-sphere center from start of line, projected onto the line, 
	 * normalized to length of line. Compare this with the current
	 * minimum intersection distance...
	 */
	if( distance < intersectParams->minDistance )
	{
		/*
		 * This atomic is currently the nearest, so remember it...
		 */
		intersectParams->pickedAtomic = atomic;

		/*
		 * Update the nearest intersection distance...
		 */
		intersectParams->minDistance = distance;
	}

	return atomic;
}

/*
 *****************************************************************************
 */
RpAtomic *
AlPickNearestAtomic(RwV2d *pixel)
{
	AtomicIntersectParams intersectParams;
	RwLine pixelRay;

	/*
	 * An atomic is picked based on the intersection with a line that passes
	 * through the camera's view-frustum, defined by the camera's center of
	 * projection and the position of the pixel on the view-plane. The line is 
	 * delimited by the near clip-plane (its start point) and the far 
	 * clip-plane (its end position)...
	 */
	RwCameraCalcPixelRay(Camera, &pixelRay, pixel);

	intersectParams.intersection.t.line = pixelRay;
	intersectParams.intersection.type = rpINTERSECTLINE;
	intersectParams.pickedAtomic = NULL;
	intersectParams.minDistance = RwRealMAXVAL;

	RpWorldForAllAtomicIntersections(World, &intersectParams.intersection, 
									 AtomicIntersectLine2, &intersectParams);

	return intersectParams.pickedAtomic;

}


/*
 *****************************************************************************
 */
RpCollisionTriangle *PickLineCallback(RpIntersection *intersection __RWUNUSED__,
							RpWorldSector *sector __RWUNUSED__, 
							RpCollisionTriangle *collTriangle,
							RwReal distance, 
							void *data)
{
	RwReal *closest = (RwReal *)data;

	/* 
	 * Keep track of closest point...
	 */
	if( distance < *closest )
	{
		*closest = distance;
	}

	return collTriangle;
}


/*
 *****************************************************************************
 */
RwV3d 
AlPickNearestWorld(RwV2d *pixel)
{
	RwReal distance = RwRealMAXVAL;
	RpIntersection intersection;
	RwLine pixelRay;
	RwV3d position;

	/*
	 * An atomic is picked based on the intersection with a line that passes
	 * through the camera's view-frustum, defined by the camera's center of
	 * projection and the position of the pixel on the view-plane. The line is 
	 * delimited by the near clip-plane (its start point) and the far 
	 * clip-plane (its end position)...
	 */
	RwCameraCalcPixelRay(Camera, &pixelRay, pixel);

	intersection.t.line = pixelRay;
	intersection.type = rpINTERSECTLINE;

	RpCollisionWorldForAllIntersections(World, &intersection, 
									 PickLineCallback, &distance);

	RwV3dSub(&position, &pixelRay.end, &pixelRay.start);
	RwV3dScale(&position, &position, distance);
	RwV3dAdd(&position, &position, &pixelRay.start);

	return position;
}

/*
*****************************************************************************
*/
