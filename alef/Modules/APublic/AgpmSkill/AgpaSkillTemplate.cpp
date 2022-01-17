#include "AgpaSkillTemplate.h"

/*****************************************************************
*   Function : AgpaSkillTemplate()
*   Comment  : 생성자 
*   Date&Time : 2002-04-17, 오후 12:02
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpaSkillTemplate::AgpaSkillTemplate()
{
}

/*****************************************************************
*   Function : ~AgpaSkillTemplate()
*   Comment  : 소멸자 
*   Date&Time : 2002-04-17, 오후 12:02
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpaSkillTemplate::~AgpaSkillTemplate()
{
}

/*****************************************************************
*   Function : AddSkillTemplate
*   Comment  : Add Skill Template
*   Date&Time : 2002-04-18, 오후 1:25
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdSkillTemplate*   AgpaSkillTemplate::AddSkillTemplate(AgpdSkillTemplate* pcsSkillTemplate)
{
	if (AddObject( (PVOID*) &pcsSkillTemplate, pcsSkillTemplate->m_lID ))
		return pcsSkillTemplate;

	return NULL;
}

/*****************************************************************
*   Function : GetSkillTemplate
*   Comment  : Get Skill Template
*   Date&Time : 2002-04-18, 오후 1:25
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdSkillTemplate*   AgpaSkillTemplate::GetSkillTemplate( INT32	lTID )
{
	AgpdSkillTemplate **pvRetVal = (AgpdSkillTemplate **) GetObject( lTID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

/*****************************************************************
*   Function : GetSkillTemplate
*   Comment  : Get Skill Template
*   Date&Time : 2002-04-18, 오후 1:25
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdSkillTemplate*   AgpaSkillTemplate::GetSkillTemplate( CHAR *szName )
{
	AgpdSkillTemplate **pvRetVal = (AgpdSkillTemplate **) GetObject( szName );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

/*****************************************************************
*   Function : RemoveSkillTemplate
*   Comment  : Remove Skill Template
*   Date&Time : 2002-04-18, 오후 1:25
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgpaSkillTemplate::RemoveSkillTemplate(INT32	lTID)
{
	return RemoveObject( lTID );
}

BOOL	AgpaSkillTemplate::AddTemplateStringKey(INT32 lTID, CHAR *szTName)
{
	return AddStringKey(lTID, szTName);
}