// AgpaSpawnGroup.cpp: implementation of the AgpaSpawnGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "AgpaSpawnGroup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgpaSpawnGroup::AgpaSpawnGroup()
{

}

AgpaSpawnGroup::~AgpaSpawnGroup()
{

}

AgpdSpawnGroup *	AgpaSpawnGroup::AddGroup(AgpdSpawnGroup* pstGroup)
{
	if (AddObject( (PVOID*) &pstGroup, pstGroup->m_lID, pstGroup->m_szName))
		return pstGroup;

	return NULL;
}

AgpdSpawnGroup *	AgpaSpawnGroup::GetGroup(INT32 lSGID)
{
	AgpdSpawnGroup **pvRetVal = (AgpdSpawnGroup **) GetObject( lSGID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

AgpdSpawnGroup *	AgpaSpawnGroup::GetGroup(CHAR *szName)
{
	AgpdSpawnGroup **pvRetVal = (AgpdSpawnGroup **) GetObject( szName );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

BOOL	AgpaSpawnGroup::RemoveGroup(INT32 lSGID)
{
	BOOL				bRemove	= FALSE;
	AgpdSpawnGroup *	pstGroup = GetGroup(lSGID);

	if (!pstGroup)
		return FALSE;

	bRemove = RemoveObject(lSGID, pstGroup->m_szName);

	return bRemove;
}

BOOL	AgpaSpawnGroup::RemoveGroup(AgpdSpawnGroup* pstGroup)
{
	BOOL	bRemove = FALSE;

	bRemove = RemoveObject(pstGroup->m_lID, pstGroup->m_szName);

	return bRemove;
}
