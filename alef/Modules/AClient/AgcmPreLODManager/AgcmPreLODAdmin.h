#ifndef __AGCMPRELODADMIN_H__
#define __AGCMPRELODADMIN_H__

#include "AgcdPreLOD.h"

class AgcmPreLODAdmin
{
public:
	AgcmPreLODAdmin();
	virtual ~AgcmPreLODAdmin();

public:
	BOOL			RemoveAllPreLODData(AgcdPreLOD *pstLOD);
	AgcdPreLODData	*AddPreLODData(AgcdPreLOD *pstLOD, AgcdPreLODData *pcsData = NULL);
	BOOL			RemovePreLODData(AgcdPreLOD *pstLOD, INT32 lIndex);
	AgcdPreLODData	*GetPreLODData(AgcdPreLOD *pstLOD, INT32 lIndex);

	BOOL			CopyPreLOD(AgcdPreLOD *pstDest, AgcdPreLOD *pstSrc);
};

#endif // __AGCMPRELODADMIN_H__