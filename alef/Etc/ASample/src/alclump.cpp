#include "alclump.h"
#include "alcharacter.h"

/*
 *****************************************************************************
 */
RpClump *AlClumpLoad(const RwChar *clumpPath)
{
	RwStream *stream = (RwStream *)NULL;
	RpClump *clump = (RpClump *)NULL;
	RwChar *path = (RwChar *)NULL;

	path = RsPathnameCreate(clumpPath);

	/*
	 * Now load the clump...
	 */
	stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);
	if( stream )
	{
		if( RwStreamFindChunk(stream, rwID_CLUMP,
							  (RwUInt32 *)NULL, (RwUInt32 *)NULL) )
		{
			/*
			* Read the clump chunk...
			*/
			clump = RpClumpStreamRead(stream);

			RwStreamClose (stream, NULL);
		}
	}

	RsPathnameDestroy(path);


	return clump;
}

/*
 *****************************************************************************
 */
void AlClumpDestroy(RpWorld *world, RpClump *Clump)
{
	if( Clump )
	{
		if (world)
			RpWorldRemoveClump(world, Clump);

		RpClumpDestroy(Clump);
	}
	return;
}

/*
 ******************************************************************************
 */
RpAtomic *GetFirstAtomic(RpAtomic * atomic, void *data)
{
	RpAtomic		  **firstAtomic = (RpAtomic **) data;

	*firstAtomic = atomic;
	return NULL;
}

/*
 ******************************************************************************
 */
RpAtomic *AlClumpGetFirstAtomic(RpClump * clump)
{
	RpAtomic		   *atomic;

	RpClumpForAllAtomics(clump, GetFirstAtomic, (void *) &atomic);

	return atomic;
}

RpAtomic *Instancing(RpAtomic * atomic, void *data)
{
	RwBool *ret = (RwBool *) data;

	*ret = RpAtomicInstance(atomic);

	if (*ret)
		return atomic;

	return NULL;
}

RwBool AlClumpInstance(RpClump * clump)
{
	RwBool ret;

	RpClumpForAllAtomics(clump, Instancing, (void *) &ret);

	return ret;
}

RpAtomic *GetBBox(RpAtomic *atomic, void *data)
{
	RwBBox *bbox = (RwBBox *) data;
	RwSphere *sphere;

	sphere = RpAtomicGetBoundingSphere(atomic);

	if (bbox->inf.x > sphere->center.x - sphere->radius)
		bbox->inf.x = sphere->center.x - sphere->radius;

	if (bbox->inf.y > sphere->center.y - sphere->radius)
		bbox->inf.y = sphere->center.y - sphere->radius;

	if (bbox->inf.z > sphere->center.z - sphere->radius)
		bbox->inf.z = sphere->center.z - sphere->radius;

	if (bbox->sup.x < sphere->center.x + sphere->radius)
		bbox->sup.x = sphere->center.x + sphere->radius;

	if (bbox->sup.y < sphere->center.y + sphere->radius)
		bbox->sup.y = sphere->center.y + sphere->radius;

	if (bbox->sup.z < sphere->center.z + sphere->radius)
		bbox->sup.z = sphere->center.z + sphere->radius;

	return atomic;
}

void AlClumpGetBBox(RpClump *clump, RwBBox *bbox)
{
	memset(bbox, 0, sizeof(RwBBox));

	RpClumpForAllAtomics(clump, GetBBox, (void *) bbox);
}
