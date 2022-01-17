#include "AcuRpUVAnimData.h"

#include <d3d9.h>

#define UVANIMDATAATOMIC(atomic) \
    (RWPLUGINOFFSET(UVAnimDataAtomicData, atomic, UVAnimDataAtomicDataOffset))

#define UVANIMDATAATOMICDATA(atomic, var) \
    (RWPLUGINOFFSET(UVAnimDataAtomicData, atomic, UVAnimDataAtomicDataOffset)->var)

#define UVANIMDATAATOMICDATACONST(atomic, var) \
    (RWPLUGINOFFSETCONST(UVAnimDataAtomicData, atomic, UVAnimDataAtomicDataOffset)->var)

RwInt32 UVAnimDataGlobalDataOffset = -1;
RwInt32 UVAnimDataAtomicDataOffset = -1;

/*
 *****************************************************************************
 */
static void *
UVAnimDataOpen(void *instance,
							 RwInt32 offset __RWUNUSED__,
							 RwInt32 size __RWUNUSED__)
{
	/*
	 * Initialize UVAnimData Pipeline
	 */

	return instance;
}


static void *
UVAnimDataClose(void *instance,
							RwInt32 offset __RWUNUSED__,
							RwInt32 size __RWUNUSED__)
{
	/*
	 * Destroy UVAnimData Pipeline
	 */

	return instance;
}

/*
 *****************************************************************************
 */
static void *
UVAnimDataAtomicDataConstructor(void *atomic,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	if (UVAnimDataAtomicDataOffset > 0)
	{
		UVANIMDATAATOMICDATA(atomic, dict) = NULL;
	}

	return atomic;
}


static void *
UVAnimDataAtomicDataDestructor(void *atomic,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	return atomic;
}


static void *
UVAnimDataAtomicDataCopier(void *dstatomic, const void *srcatomic,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{
	/*
	 * If source atomic is enabled, enable effect of destination atomic
	 */
	if (UVAnimDataAtomicDataOffset > 0)
	{
		UVANIMDATAATOMICDATA(dstatomic, dict) = UVANIMDATAATOMICDATA(srcatomic, dict);
	}

	return dstatomic;
}

/*
 *****************************************************************************
 */
RwBool
RpUVAnimDataPluginAttach(void)
{
	/*
	 * Register global space...
	 */
	//UVAnimDataGlobalDataOffset = RwEngineRegisterPlugin(0,
	//									  rwID_UVANIMDATA,
	//									  UVAnimDataOpen,
	//									  UVAnimDataClose);

	//if (UVAnimDataGlobalDataOffset < 0)
	//{
		//return FALSE;
	//}

	/*
	 * Register atomic extension space...
	 */
	UVAnimDataAtomicDataOffset = RpAtomicRegisterPlugin(sizeof(UVAnimDataAtomicData),
										rwID_UVANIMDATA,
										UVAnimDataAtomicDataConstructor,
										UVAnimDataAtomicDataDestructor,
										UVAnimDataAtomicDataCopier);

	if (UVAnimDataAtomicDataOffset < 0)
	{
		return FALSE;
	}

	return TRUE;
}

/*
 *****************************************************************************
 */
RpAtomic *
RpUVAnimDataAtomicSetDict(RpAtomic *atomic, RtDict *dict)
{
	if (UVAnimDataAtomicDataOffset > 0)
	{
		UVANIMDATAATOMICDATA(atomic, dict) = dict;

		return atomic;
	}

	return NULL;
}

RpClump *
RpUVAnimDataClumpSetDict(RpClump *clump, RtDict *dict)
{
	if (UVAnimDataAtomicDataOffset > 0)
	{
		RpClumpForAllAtomics(clump, RpUVAnimDataAtomicSetDict, dict);

		return clump;
	}

	return NULL;
}

RtDict *
RpUVAnimDataAtomicGetDict(RpAtomic *atomic)
{
	if (UVAnimDataAtomicDataOffset > 0)
	{
		return UVANIMDATAATOMICDATA(atomic, dict);
	}

	return NULL;
}

static RpAtomic *	CBGetDict(RpAtomic *atomic, PVOID pvData)
{
	RtDict ** ppstDict = (RtDict **) pvData;

	*ppstDict = RpUVAnimDataAtomicGetDict(atomic);

	return NULL;
}

RtDict *
RpUVAnimDataClumpGetDict(RpClump *clump)
{
	RtDict *	pstDict = NULL;

	RpClumpForAllAtomics(clump, CBGetDict, &pstDict);

	return pstDict;
}
