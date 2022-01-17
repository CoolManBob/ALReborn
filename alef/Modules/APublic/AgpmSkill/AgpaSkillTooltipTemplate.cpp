#include "AgpaSkillTooltipTemplate.h"

AgpaSkillTooltipTemplate::AgpaSkillTooltipTemplate()
{
}

AgpaSkillTooltipTemplate::~AgpaSkillTooltipTemplate()
{
}

AgpdSkillTooltipTemplate* AgpaSkillTooltipTemplate::AddSkillTooltipTemplate(AgpdSkillTooltipTemplate* pcsSkillTooltipTemplate)
{
	if (AddObject( (PVOID*) &pcsSkillTooltipTemplate, pcsSkillTooltipTemplate->m_szSkillName ))
		return pcsSkillTooltipTemplate;

	return NULL;
}

AgpdSkillTooltipTemplate* AgpaSkillTooltipTemplate::GetSkillTooltipTemplate(CHAR *szSkillName)
{
	AgpdSkillTooltipTemplate **pvRetVal = (AgpdSkillTooltipTemplate **) GetObject( szSkillName );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

BOOL AgpaSkillTooltipTemplate::RemoveSkillTooltipTemplate(CHAR *szSkillName)
{
	return RemoveObject( szSkillName );
}