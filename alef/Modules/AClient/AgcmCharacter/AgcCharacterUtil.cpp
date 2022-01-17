#include "AgcCharacterUtil.h"


RpAtomic *UtilSetHierarchyForSkinAtomic(RpAtomic *atomic, void *data)
{
#if (defined(RPSKIN310_H))
    RpSkinSetHAnimHierarchy(RpSkinAtomicGetSkin(atomic),
                            (RpHAnimHierarchy *)data);
#else /* (defined(RPSKIN310_H)) */
    RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy *)data);
#endif /* (defined(RPSKIN310_H)) */
    
    return atomic;
}

RwFrame *UtilGetChildFrameHierarchy(RwFrame *frame, void *data)
{    
    RpHAnimHierarchy *hierarchy = *(RpHAnimHierarchy **)data;

    /*
     * Return the first hierarchy found that is attached to one of the atomic
     * frames...
     */

    hierarchy = RpHAnimFrameGetHierarchy(frame);
    if( hierarchy == NULL )
    {
        RwFrameForAllChildren(frame, UtilGetChildFrameHierarchy, data);

        return frame;
    }

    *(void **)data = (void *)hierarchy;

    return NULL;
}


RpHAnimHierarchy *UtilGetHierarchy(RpClump *clump)
{
    RpHAnimHierarchy *hierarchy = NULL;
    
    /*
     * Return the hierarchy for this model...
     */

    RwFrameForAllChildren(RpClumpGetFrame(clump), UtilGetChildFrameHierarchy,
        (void *)&hierarchy);

    return hierarchy;
}

RwChar	*UtilPathnameCreate( const RwChar *srcBuffer )
{
	RwChar *dstBuffer;
    RwChar *charToConvert;

    /* 
     * First duplicate the string 
     */
    dstBuffer = (RwChar *)malloc(sizeof(RwChar) * (strlen(srcBuffer) + 1));

    if( dstBuffer )
    {
        strcpy(dstBuffer, srcBuffer);

        /* 
         * Convert a path for use on Windows. 
         * Convert all /s and :s into \s 
         */
        while( (charToConvert = strchr(dstBuffer, '/')) )
        {
            *charToConvert = '\\';
        }
#if 0
        while( (charToConvert = strchr(dstBuffer, ':')) )
        {
            *charToConvert = '\\';
        }
#endif
    }

    return dstBuffer;
}

RpHAnimAnimation *UtilLoadAnimationFile(RwChar *file)
{
    RpHAnimAnimation *anim = NULL;
    RwChar *path = NULL;

    path = UtilPathnameCreate(file);
    anim = RpHAnimAnimationRead(path);
    //RsPathnameDestroy(path);
	free(path);

    return anim;
}

RpAtomic *UtilGetBBox(RpAtomic *atomic, void *data)
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

void UtilClumpGetBBox(RpClump *clump, RwBBox *bbox)
{
	memset(bbox, 0, sizeof(RwBBox));

	RpClumpForAllAtomics(clump, UtilGetBBox, (void *) bbox);
}



RpClump *UtilClumpLoad(const RwChar *clumpPath)
{
	RwStream *stream = (RwStream *)NULL;
	RpClump *clump = (RpClump *)NULL;
	RwChar *path = (RwChar *)NULL;

	path = UtilPathnameCreate(clumpPath);
	//path = "models\\orc_male.DFF";
	
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

//	RsPathnameDestroy(path);
//	RwFree(path);
	free( path );

	return clump;
}

RpAtomic *UtilCopyAtomic(RpAtomic *pstAtomic)
{
	RwMemory	stMemory;
	RwStream	*pstReadStream;
	RwStream	*pstWriteStream;
	RpAtomic	*pstNewAtomic;

	stMemory.length = RpAtomicStreamGetSize(pstAtomic);

	pstWriteStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMWRITE, &stMemory);
	if(!pstWriteStream)
	{
		return NULL;
	}

	if(!RpAtomicStreamWrite(pstAtomic, pstWriteStream))
	{
		return NULL;
	}

	RwStreamClose(pstWriteStream, &stMemory);

	pstReadStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &stMemory);
	if(!pstReadStream)
	{
		RwFree(stMemory.start);
		return NULL;
	}

	if(!RwStreamFindChunk(pstReadStream, rwID_ATOMIC, NULL, NULL))
	{
		RwFree(stMemory.start);
		return NULL;
	}

	pstNewAtomic = RpAtomicStreamRead(pstReadStream);

	RwStreamClose(pstReadStream, &stMemory);

	RwFree(stMemory.start);

	return pstNewAtomic;
}

RpAtomic *UtilSetAtomicCB(RpAtomic *atomic, void *data)
{
	RpAtomic **pstDestAtomic = (RpAtomic **)(data);
	*(pstDestAtomic) = atomic;

	return atomic;
}

RpAtomic *UtilSetUDAInt32CB(RpAtomic *atomic, void *data)
{
	UtilSetUDAInt32Params	*params								= (UtilSetUDAInt32Params *)(data);
	RpGeometry				*geom								= RpAtomicGetGeometry(atomic);
	
	if (geom)
	{		
		RwInt32				lUDANum								= RpGeometryGetUserDataArrayCount(geom);
		RpUserDataArray		*pstUserDataArray;
		RwChar				*pszUserDataArrayName;

		for (RwInt32 lCount = 0; lCount < lUDANum; ++lCount)
		{
			pstUserDataArray									= RpGeometryGetUserDataArray(geom, lCount);
			if (pstUserDataArray)
			{
				pszUserDataArrayName							= RpUserDataArrayGetName(pstUserDataArray);
				if (!strcmp(pszUserDataArrayName, params->m_szUDAName))
				{
					params->m_alUDAInt32[params->m_lCBCount]	= RpUserDataArrayGetInt(pstUserDataArray, 0);
					//@{ 2006/11/14 burumal
					params->m_nValidCount++;
					//@}
					break;
				}
			}
		}
	}

	++params->m_lCBCount;

	return atomic;
}