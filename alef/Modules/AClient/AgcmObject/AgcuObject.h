#ifndef __AGCU_OBJECT_H__
#define __AGCU_OBJECT_H__

#include "ApBase.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rphanim.h"
#include "rpskin.h"
#include "rphanim.h"
#include "rpusrdat.h"
#include "rplodatm.h"

#define				AGCU_OBJECT_USRDATA_NAME_BILLBOARD			"Billboard"

RpHAnimHierarchy	*AgcuObject_GetHierarchy(RpClump *clump);
RwFrame				*AgcuObject_GetChildFrameHierarchy(RwFrame *frame, void *data);
RpAtomic			*AgcuObject_SetHierarchyForSkinAtomic(RpAtomic *atomic, void *data);
RpAtomic			*AgcuObject_CopyAtomic(RpAtomic *pstAtomic);
RpAtomic			*AgcuObject_SetAtomic(RpAtomic *atomic, void *data);
//RpGeometry			*AgcuObject_FindLODBillboard(RpGeometry *geom, void *data);
//RpAtomic			*AgcuObject_FindLODBillboard(RpAtomic *atomic, void *data);

#endif // __AGCU_OBJECT_H__

/******************************************************************************
******************************************************************************/