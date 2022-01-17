#include "AgpaAI2.h"

AgpaAI2Template::AgpaAI2Template()
{
}

AgpaAI2Template::~AgpaAI2Template()
{
}

AgpdAI2Template	*AgpaAI2Template::AddAITemplate(AgpdAI2Template *pstTemplate)
{

	if (!AddObject((PVOID *) &pstTemplate, pstTemplate->m_lTID))
		return NULL;

	return pstTemplate;
}

AgpdAI2Template	*AgpaAI2Template::GetAITemplate(INT32 lTID)
{
	AgpdAI2Template **	ppstTemplate;

	ppstTemplate = (AgpdAI2Template **) GetObject(lTID);
	if (!ppstTemplate)
		return NULL;

	return *ppstTemplate;
}

BOOL AgpaAI2Template::RemoveAITemplate(INT32 lTID)
{
	return RemoveObject(lTID);
}

AgpdAI2Template *AgpaAI2Template::GetTemplateSequence(INT32 *plIndex)
{
	AgpdAI2Template **ppcsTemplate = (AgpdAI2Template **)GetObjectSequence(plIndex);

	if (!ppcsTemplate)
		return NULL;

	return  *ppcsTemplate;
}