/*====================================================================

	AgpaProduct.cpp

====================================================================*/

#include "AgpaProduct.h"


/****************************************************************/
/*		The Implementation of AgpaProductSkillFactor class		*/
/****************************************************************/
//
AgpaProductSkillFactor::AgpaProductSkillFactor()
	{
	}

AgpaProductSkillFactor::~AgpaProductSkillFactor()
	{
	}

BOOL AgpaProductSkillFactor::Add(AgpdProductSkillFactor *pAgpdProductSkillFactor)
	{
	if (!AddObject((PVOID) &pAgpdProductSkillFactor, pAgpdProductSkillFactor->m_szName))
		return FALSE;

	return TRUE;	
	}

BOOL AgpaProductSkillFactor::Remove(AgpdProductSkillFactor *pAgpdProductSkillFactor)
	{
	return RemoveObject(pAgpdProductSkillFactor->m_szName);
	}

AgpdProductSkillFactor* AgpaProductSkillFactor::Get(CHAR *pszSkill)
	{
	AgpdProductSkillFactor **ppAgpdProductSkillFactor = (AgpdProductSkillFactor **) GetObject(pszSkill);
	if (!ppAgpdProductSkillFactor)
		return NULL;	

	return *ppAgpdProductSkillFactor;
	}

/************************************************************/
/*		The Implementation of AgpaComposeTemplate class		*/
/************************************************************/
//
AgpaComposeTemplate::AgpaComposeTemplate()
	{
	}

AgpaComposeTemplate::~AgpaComposeTemplate()
	{
	}

BOOL AgpaComposeTemplate::Add(AgpdComposeTemplate *pAgpdComposeTemplate)
	{
	if (!AddObject((PVOID) &pAgpdComposeTemplate, pAgpdComposeTemplate->m_lComposeID, pAgpdComposeTemplate->m_szCompose))
		return FALSE;

	return TRUE;	
	}

BOOL AgpaComposeTemplate::Remove(AgpdComposeTemplate *pAgpdComposeTemplate)
	{
	return RemoveObject(pAgpdComposeTemplate->m_lComposeID);
	}

AgpdComposeTemplate* AgpaComposeTemplate::Get(INT32 lComposeID)
	{
	AgpdComposeTemplate **ppAgpdComposeTemplate = (AgpdComposeTemplate **) GetObject(lComposeID);
	if (!ppAgpdComposeTemplate)
		return NULL;	

	return *ppAgpdComposeTemplate;
	}

AgpdComposeTemplate* AgpaComposeTemplate::Get(CHAR pszCompose)
	{
	AgpdComposeTemplate **ppAgpdComposeTemplate = (AgpdComposeTemplate **) GetObject(pszCompose);
	if (!ppAgpdComposeTemplate)
		return NULL;	

	return *ppAgpdComposeTemplate;
	}

