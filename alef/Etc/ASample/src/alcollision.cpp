#include "alcollision.h"
#include "alclump.h"

RwBool bMove = FALSE;

extern RpAtomic *DetailMapAtomic;

/*
 *****************************************************************************
 */
RpCollisionTriangle *AlCollisionWorldLineCallback(RpIntersection *intersection __RWUNUSED__,
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
RpCollisionTriangle *AlCollisionGeometryLineCallback(RpIntersection *intersection __RWUNUSED__,
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
RwBool AlCollisionLine(RwV3d *FramePos, RwV3d *FrameDelta, RpWorld *world, RwBBox *bbox)
{
	RpIntersection intersection;
	RwLine line;
	RwReal range;
	RwReal range2;

	/* 
	 * Construct a vertical line through the position of the Frameera.
	 * The line extends the full height of the world bounding-box...
	 */
	RwV3dAdd(&line.start, FramePos, FrameDelta);
	line.start.y = (bbox->sup.y);

	line.end = line.start;
	line.end.y = (bbox->inf.y);
	
	/*
	 * Create intersection object...
	 */
	intersection.type = rpINTERSECTLINE;
	intersection.t.line = line;
	
	/* 
	 * Test for collisions...
	 */
	range = FARAWAY;
	range2 = FARAWAY;

	RpCollisionWorldForAllIntersections(world, 
		&intersection, AlCollisionWorldLineCallback, &range);

	if (DetailMapAtomic)
	{
		RpAtomicForAllIntersections(DetailMapAtomic	, 
			&intersection, AlCollisionGeometryLineCallback, &range2);

		if (range2 < range)
			range = range2;
	}

	if( range < FARAWAY )
	{
		RwV3d pos;

		/* 
		 * Compute the position of the closest intersection point...
		 */
		RwV3dSub(&pos, &line.end, &line.start);
		RwV3dScale(&pos, &pos, range);
		RwV3dAdd(&pos, &pos, &line.start);
		
		/*
		 * Compute new camera delta vector...
		 */
		RwV3dSub(&pos, &pos, FramePos);
		RwV3dAdd(FrameDelta, &pos, FrameDelta);
	}
	else
	{
		/*
		 * Should never get to here...
		 */

		return FALSE;
	}

	return TRUE;
}

/*
 *****************************************************************************
 */
RwV3d FindNearestPointOnLine(RwV3d *point, RwV3d *start, RwV3d *end)
{
	RwReal mu;
	RwV3d line, candidate;

	RwV3dSub(&line, end, start);

	/*
	 * Project point onto the line. The value of mu is the 
	 * distance of the projected point from the start of the line 
	 * scaled by the length of the line...
	 */
	mu = RwV3dDotProduct(point, &line) - RwV3dDotProduct(start, &line);

	if( mu <= 0 )
	{
		/*
		 * Point lies on the line outside the start point...
		 */
		candidate = *start;
	}
	else
	{
		RwReal lineLength2;

		lineLength2 = RwV3dDotProduct(&line, &line);

		if( mu < lineLength2 )
		{
			/*
			 * Point lies on the line between start and end.
			 * Calculate point along line...
			 */
			mu /= lineLength2;

			RwV3dScale(&candidate, &line, mu);
			RwV3dAdd(&candidate, &candidate, start);
		}
		else
		{
			/*
			 * Point lies on the line outside end point...
			 */
			candidate = *end;
		}
	}

	return candidate;
}

/*
 *****************************************************************************
 */
void AlCollisionConfine(RwV3d *pos, RwV3d *delta, RpWorld *world)
{
	const RwBBox *bbox;

	/* 
	 * Does not bound y-values to world bounding box - may prevent camera with 
	 * large radius from climbing to top of highest peak in world...
	 */

	bbox = RpWorldGetBBox(world);

	if( (pos->x - delta->x) < bbox->inf.x )
	{
		delta->x = bbox->inf.x - pos->x;
	}
	else if( (pos->x + delta->x) > bbox->sup.x )
	{ 
		delta->x = bbox->sup.x - pos->x;
	}

	if( (pos->z + delta->z) < bbox->inf.z )
	{
		delta->z = bbox->inf.z - pos->z;
	}
	else if( (pos->z + delta->z) > bbox->sup.z )
	{ 
		delta->z = bbox->sup.z - pos->z;
	}
	
	return;
}
