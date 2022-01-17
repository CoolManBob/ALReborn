// AgpaQuest.cpp: implementation of the AgpaQuest class.
//
//////////////////////////////////////////////////////////////////////

#include "AgpaQuestTemplate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgpaQuestTemplate::AgpaQuestTemplate()
{

}

AgpaQuestTemplate::~AgpaQuestTemplate()
{

}

AgpdQuestTemplate*	AgpaQuestTemplate::Add(AgpdQuestTemplate *pcsTemplate)
{
	if (AddObject( (PVOID*) &pcsTemplate, pcsTemplate->m_lID ))
		return pcsTemplate;

	return NULL;
}

AgpdQuestTemplate*	AgpaQuestTemplate::Get(INT32 lQuestID)
{
	AgpdQuestTemplate **pvRetVal = (AgpdQuestTemplate **) GetObject( lQuestID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

BOOL AgpaQuestTemplate::Remove(INT32 lQuestID)
{
	return RemoveObject(lQuestID);
}
