#include "AgcuObject.h"

RwFrame *AgcuObject_GetChildFrameHierarchy(RwFrame *frame, void *data)
{    
    RpHAnimHierarchy *hierarchy = *(RpHAnimHierarchy **)data;

    /*
     * Return the first hierarchy found that is attached to one of the atomic
     * frames...
     */

    hierarchy = RpHAnimFrameGetHierarchy(frame);
    if( hierarchy == NULL )
    {
        RwFrameForAllChildren(frame, AgcuObject_GetChildFrameHierarchy, data);

        return frame;
    }

    *(void **)data = (void *)hierarchy;

    return NULL;
}

RpHAnimHierarchy *AgcuObject_GetHierarchy(RpClump *clump)
{
    RpHAnimHierarchy *hierarchy = NULL;
    
    /*
     * Return the hierarchy for this model...
     */

    RwFrameForAllChildren(RpClumpGetFrame(clump), AgcuObject_GetChildFrameHierarchy,
        (void *)&hierarchy);

    return hierarchy;
}

RpAtomic *AgcuObject_SetHierarchyForSkinAtomic(RpAtomic *atomic, void *data)
{
#if (defined(RPSKIN310_H))
    RpSkinSetHAnimHierarchy(RpSkinAtomicGetSkin(atomic),
                            (RpHAnimHierarchy *)data);
#else /* (defined(RPSKIN310_H)) */
    RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy *)data);
#endif /* (defined(RPSKIN310_H)) */
    
    return atomic;
}

RpAtomic *AgcuObject_CopyAtomic(RpAtomic *pstAtomic)
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

RpAtomic *AgcuObject_SetAtomic(RpAtomic *atomic, void *data)
{
	RpAtomic **pstDestAtomic = (RpAtomic **)(data);
	*(pstDestAtomic) = atomic;

	return atomic;
}

/*
RpGeometry	*AgcuObject_FindLODBillboard(RpGeometry *geom, void *data)
{
	BOOL			*pbBillboard	= (BOOL *)(data);

	RwChar			*pszName;
	RpUserDataArray	*pstUDA;
	INT32			nMax			= RpGeometryGetUserDataArrayCount(geom);

	for(INT32 nIndex = 0; nIndex < nMax; ++nIndex)
	{
		pstUDA = RpGeometryGetUserDataArray(geom, nIndex);
		if(pstUDA)
		{
			pszName = RpUserDataArrayGetName(pstUDA);
			if(!strcmp(pszName, AGCU_OBJECT_USRDATA_NAME_BILLBOARD))
			{
				*(pbBillboard) = TRUE;
			}
		}
	}

	return geom;
}

RpAtomic *AgcuObject_FindLODBillboard(RpAtomic *atomic, void *data)
{
	RpLODAtomicForAllLODGeometries(atomic, AgcuObject_FindLODBillboard, data);

	return atomic;
}
*/
/******************************************************************************
******************************************************************************/