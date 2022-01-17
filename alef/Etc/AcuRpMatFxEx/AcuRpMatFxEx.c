#include "AcuRpMatFxEx.h"

#include <d3d9.h>

#define MATFXEX_ATOMIC(atomic) \
    (RWPLUGINOFFSET(MatFxExData, atomic, MatFxExAtomicDataOffset))

#define MATFXEX_ATOMICDATA(atomic, var) \
    (RWPLUGINOFFSET(MatFxExData, atomic, MatFxExAtomicDataOffset)->var)

RwInt32 MatFxExAtomicDataOffset = -1;


/*
 *****************************************************************************
 */
static void *
MatFxExAtomicDataConstructor(void *atomic,
					   RwInt32 offset __RWUNUSED__,
					   RwInt32 size __RWUNUSED__)
{
	if (MatFxExAtomicDataOffset > 0)
	{
		MATFXEX_ATOMICDATA(atomic, pTex) = NULL;
	}

	return atomic;
}


static void *
MatFxExAtomicDataDestructor(void *atomic,
					  RwInt32 offset __RWUNUSED__,
					  RwInt32 size __RWUNUSED__)
{
	return atomic;
}


static void *
MatFxExAtomicDataCopier(void *dstatomic, const void *srcatomic,
				  RwInt32 offset __RWUNUSED__,
				  RwInt32 size __RWUNUSED__)
{
	/*
	 * If source atomic is enabled, enable effect of destination atomic
	 */
	if (MatFxExAtomicDataOffset > 0)
	{
		MATFXEX_ATOMICDATA(dstatomic, pTex) = MATFXEX_ATOMICDATA(srcatomic, pTex);
	}

	return dstatomic;
}

/*
 *****************************************************************************
 */
RwBool
RpMatFxExPluginAttach(void)
{
	/*
	 * Register atomic extension space...
	 */
	MatFxExAtomicDataOffset = RpAtomicRegisterPlugin(sizeof(MatFxExData),
										rwID_MATFXEX,
										MatFxExAtomicDataConstructor,
										MatFxExAtomicDataDestructor,
										MatFxExAtomicDataCopier);

	if (MatFxExAtomicDataOffset < 0)
	{
		return FALSE;
	}

	return TRUE;
}

/*
 *****************************************************************************
 */
RpAtomic *
RpMatFxExAtomicSetTexture(RpAtomic *atomic, RwTexture *texture)
{
	if (MatFxExAtomicDataOffset > 0)
	{
		MATFXEX_ATOMICDATA(atomic, pTex) = texture;

		return atomic;
	}

	return NULL;
}

RwTexture *
RpMatFxAtomicGetTexture(RpAtomic *atomic)
{
	if (MatFxExAtomicDataOffset > 0)
	{
		return MATFXEX_ATOMICDATA(atomic, pTex);
	}

	return NULL;
}
