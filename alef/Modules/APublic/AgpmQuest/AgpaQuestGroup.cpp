// AgpaQuestGroup.cpp: implementation of the AgpaQuestGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "AgpaQuestGroup.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgpaQuestGroup::AgpaQuestGroup()
{

}

AgpaQuestGroup::~AgpaQuestGroup()
{

}

AgpdQuestGroup*	AgpaQuestGroup::Add(AgpdQuestGroup *pcsQuestGroup)
{
	if (AddObject( (PVOID*) &pcsQuestGroup, pcsQuestGroup->m_lID ))
		return pcsQuestGroup;

	return NULL;
}

AgpdQuestGroup*	AgpaQuestGroup::Get(INT32 lQuestGroupID)
{
	AgpdQuestGroup **pvRetVal = (AgpdQuestGroup **) GetObject( lQuestGroupID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

BOOL AgpaQuestGroup::Remove(INT32 lQuestGroupID)
{
	return RemoveObject(lQuestGroupID);
}

BOOL AgpaQuestGroup::IsExistEstimate(INT32 lQuestGroupID, INT32 lQuestTID)
{
	AgpdQuestGroup *pcsGroup = Get(lQuestGroupID);

	if (!pcsGroup) 
		return FALSE;

	list<INT32>::iterator iter;

	for (iter = pcsGroup->m_EstimateQuest.begin(); iter != pcsGroup->m_EstimateQuest.end(); ++iter)
	{
		 if (*iter == lQuestTID)
			 return TRUE;
	}

	return FALSE;
}

BOOL AgpaQuestGroup::IsExistGrant(INT32 lQuestGroupID, INT32 lQuestTID)
{
	AgpdQuestGroup *pcsGroup = Get(lQuestGroupID);

	if (!pcsGroup) 
		return FALSE;

	list<INT32>::iterator iter;

	for (iter = pcsGroup->m_GrantQuest.begin(); iter != pcsGroup->m_EstimateQuest.end(); ++iter)
	{
		 if (*iter == lQuestTID)
			 return TRUE;
	}

	return FALSE;
}