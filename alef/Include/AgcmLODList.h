#ifndef __AGCM_LOD_LIST_H__
#define __AGCM_LOD_LIST_H__

#include "ApBase.h"
#include "acdefine.h"

class AgcmLODList
{
public:
	AgcmLODList();
	virtual ~AgcmLODList();

public:
	BOOL		RemoveAllLODData(AgcdLOD *pstLOD);
	AgcdLODData	*AddLODData(AgcdLOD *pstLOD, AgcdLODData *pcsData = NULL);
	BOOL		RemoveLODData(AgcdLOD *pstLOD, INT32 lIndex);
	AgcdLODData	*GetLODData(AgcdLOD *pstLOD, INT32 lIndex);
};

#endif // __AGCM_LOD_LIST_H__