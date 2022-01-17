#ifndef _ACU_RECLUMP_H_
#define _ACU_RECLUMP_H_

#include "rwcore.h"
#include "rpworld.h"
#include "rpmatfx.h"
#include "rplodatm.h"

#include "skeleton.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuReClumpD" )
#else
#pragma comment ( lib , "AcuReClump" )
#endif
#endif
/*
typedef struct tag_atomicdata
{
	RwV3d				**vertlist;		
	RwV3d				**normallist;
	RwTexCoords			**texclist;
	RpTriangle			*trilist;
	RwRGBA				*prelitlist;

	RwInt32				morphtarget_num;
	RwInt32				texc_num;
	RwInt32				tri_num;
	RwInt32				vert_num;

	RwUInt32			geom_flag;
	RwUInt32			atomic_flag;

	RpMaterial*			pmaterial;

	tag_atomicdata*		next;
}AtomicData;
*/

#define MAX_ATOMIC			48
#define MAX_MORPH_TARGET	2
#define MAX_TEXCOORD_SET	2
#define	MAX_TRI_NUM			15000
#define MAX_VERT_NUM		MAX_TRI_NUM * 3


typedef struct tag_atomicdata
{
	RwV3d				vertlist[MAX_MORPH_TARGET][MAX_VERT_NUM];		
	RwV3d				normallist[MAX_MORPH_TARGET][MAX_VERT_NUM];
	RwTexCoords			texclist[MAX_TEXCOORD_SET][MAX_VERT_NUM];
	RpTriangle			trilist[MAX_TRI_NUM];
	RwRGBA				prelitlist[MAX_VERT_NUM];

	RwInt32				morphtarget_num;
	RwInt32				texc_num;
	RwInt32				tri_num;
	RwInt32				vert_num;

	RwUInt32			geom_flag;
	RwUInt32			atomic_flag;

	RpMaterial*			pmaterial;

	tag_atomicdata*		next;
}AtomicData;

RpAtomic*		RearrangeAtomicCB(RpAtomic *atomic, void *data);
RpAtomic*		RearrangeLODAtomicCB(RpAtomic *atomic, void *data);
RpClump*		RearrangeClump(RpClump* src,int lodLevel);
AtomicData*		GetAData(int index);
void			ClearAData(AtomicData* pData);
void			ReleaseADataAll();

#endif