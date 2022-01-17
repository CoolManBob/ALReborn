// AgpaSpawnGroup.h: interface for the AgpaSpawnGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPASPAWNGROUP_H__C8E4F7A0_F71E_435A_B540_904275C8F641__INCLUDED_)
#define AFX_AGPASPAWNGROUP_H__C8E4F7A0_F71E_435A_B540_904275C8F641__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApAdmin.h"
#include "AgpdEventSpawn.h"

class AgpaSpawnGroup : public ApAdmin  
{
public:
	AgpaSpawnGroup();
	virtual ~AgpaSpawnGroup();

	AgpdSpawnGroup *	AddGroup(AgpdSpawnGroup *pstGroup);
	AgpdSpawnGroup *	GetGroup(INT32 lSGID);
	AgpdSpawnGroup *	GetGroup(CHAR *szName);
	BOOL				RemoveGroup(AgpdSpawnGroup *pstGroup);
	BOOL				RemoveGroup(INT32 lSGID);
};

#endif // !defined(AFX_AGPASPAWNGROUP_H__C8E4F7A0_F71E_435A_B540_904275C8F641__INCLUDED_)
