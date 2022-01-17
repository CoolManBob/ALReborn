#include "AgpaSkillSpecializeTemplate.h"

AgpaSkillSpecializeTemplate::AgpaSkillSpecializeTemplate()
{
}

AgpaSkillSpecializeTemplate::~AgpaSkillSpecializeTemplate()
{
}

AgpdSkillSpecializeTemplate* AgpaSkillSpecializeTemplate::AddSkillSpecializeTemplate(AgpdSkillSpecializeTemplate* pcsSpecializeTemplate)
{
	if (AddObject( (PVOID*) &pcsSpecializeTemplate, pcsSpecializeTemplate->m_lID ))
		return pcsSpecializeTemplate;

	return NULL;
}

AgpdSkillSpecializeTemplate* AgpaSkillSpecializeTemplate::GetSkillSpecializeTemplate(INT32 lTID)
{
	AgpdSkillSpecializeTemplate **pvRetVal = (AgpdSkillSpecializeTemplate **) GetObject( lTID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

BOOL AgpaSkillSpecializeTemplate::RemoveSkillSpecializeTemplate(INT32 lTID)
{
	return RemoveObject( lTID );
}