#ifndef ACURPMATFXEX_H
#define ACURPMATFXEX_H

#include "RwCore.h"
#include "RpWorld.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuRpMatFxExD" )
#else
#pragma comment ( lib , "AcuRpMatFxEx" )
#endif
#endif

#define rwVENDORID_NHN		(0xfffff0L)
#define rwID_MATFXEX_NHN	(0x07)

#define rwID_MATFXEX		MAKECHUNKID(rwVENDORID_NHN, rwID_MATFXEX_NHN)

typedef struct
{
	RwTexture*		pTex;
}MatFxExData;

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */

/* Attach Plugin */
extern RwInt32 
RpMatFxExPluginAttach(void);

/* Set Uv Anim Texture */
extern RpAtomic *
RpMatFxExAtomicSetTexture(RpAtomic *atomic, RwTexture *pTex);

/* Get Uv Anim Texture */
extern RwTexture *
RpMatFxAtomicGetTexture(RpAtomic *atomic);

#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* ACURPUVANIMDATA_H */
