#ifndef ACURPUVANIMDATA_H
#define ACURPUVANIMDATA_H

#include "RwCore.h"
#include "RpWorld.h"
#include "RtDict.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuRpUVAnimDataD" )
#else
#pragma comment ( lib , "AcuRpUVAnimData" )
#endif
#endif

// 마고자 (2004-07-06 오후 3:56:21) : 필요한 라이브러리 추가.
#pragma comment ( lib , "RtDict" )
#pragma comment ( lib , "RpUVAnim" )

#define rwVENDORID_NHN		(0xfffff0L)
#define rwID_UVANIMDATA_NHN	(0x06)

#define rwID_UVANIMDATA		MAKECHUNKID(rwVENDORID_NHN, rwID_UVANIMDATA_NHN)

typedef struct
{
	RtDict *	dict;
} UVAnimDataAtomicData;

#ifdef    __cplusplus
extern "C"
{
#endif  /* __cplusplus */

/* Attach Plugin */
extern RwInt32 
RpUVAnimDataPluginAttach(void);

/* Set Dict */
extern RpAtomic *
RpUVAnimDataAtomicSetDict(RpAtomic *atomic, RtDict *dict);

extern RpClump *
RpUVAnimDataClumpSetDict(RpClump *clump, RtDict *dict);

/* Get Dict */
extern RtDict *
RpUVAnimDataAtomicGetDict(RpAtomic *atomic);

extern RtDict *
RpUVAnimDataClumpGetDict(RpClump *clump);


#ifdef    __cplusplus
}
#endif  /* __cplusplus */

#endif /* ACURPUVANIMDATA_H */
