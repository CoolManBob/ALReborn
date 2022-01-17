// AgpaAI.cpp: implementation of the AgpaAI class.
//
//////////////////////////////////////////////////////////////////////

#include "AgpaAI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgpaAI::AgpaAI()
{

}

AgpaAI::~AgpaAI()
{

}

AgpdAITemplate *	AgpaAI::AddAITemplate(AgpdAITemplate *pstTemplate)
{
	if (!AddObject((PVOID *) &pstTemplate, pstTemplate->m_lID))
		return NULL;

	return pstTemplate;
}

AgpdAITemplate *	AgpaAI::GetAITemplate(INT32 lTID)
{
	AgpdAITemplate **	ppstTemplate;

	ppstTemplate = (AgpdAITemplate **) GetObject(lTID);
	if (!ppstTemplate)
		return NULL;

	return *ppstTemplate;
}

BOOL				AgpaAI::RemoveAITemplate(INT32 lTID)
{
	return RemoveObject(lTID);
}

