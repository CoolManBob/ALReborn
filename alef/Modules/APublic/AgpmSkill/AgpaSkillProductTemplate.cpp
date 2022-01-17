#include "AgpaSkillProductTemplate.h"

/*****************************************************************
*   Function : AgpaSkillProductTemplate()
*   Comment  : 생성자 
*   Date&Time : 2002-04-17, 오후 12:02
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpaSkillProductTemplate::AgpaSkillProductTemplate()
{
}

/*****************************************************************
*   Function : ~AgpaSkillProductTemplate()
*   Comment  : 소멸자 
*   Date&Time : 2002-04-17, 오후 12:02
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpaSkillProductTemplate::~AgpaSkillProductTemplate()
{
}

/*****************************************************************
*   Function : AddSkillProductTemplate
*   Comment  : Add Skill Template
*   Date&Time : 2002-04-18, 오후 1:25
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdSkillProductTemplate*   AgpaSkillProductTemplate::AddSkillProductTemplate(AgpdSkillProductTemplate* pcsSkillProductTemplate)
{
	//if (AddObject( (PVOID*) &pcsSkillProductTemplate, pcsSkillProductTemplate->m_lID, pcsSkillProductTemplate->m_szName ))
	if (AddObject( (PVOID*) &pcsSkillProductTemplate, pcsSkillProductTemplate->m_szName ))
		return pcsSkillProductTemplate;

	return NULL;
}

/*****************************************************************
*   Function : GetSkillProductTemplate
*   Comment  : Get Skill Template
*   Date&Time : 2002-04-18, 오후 1:25
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
/*
AgpdSkillProductTemplate*   AgpaSkillProductTemplate::GetSkillProductTemplate( INT32	lTID )
{
	AgpdSkillProductTemplate **pvRetVal = (AgpdSkillProductTemplate **) GetObject( lTID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}
*/
/*****************************************************************
*   Function : GetSkillProductTemplate
*   Comment  : Get Skill Template
*   Date&Time : 2002-04-18, 오후 1:25
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdSkillProductTemplate*   AgpaSkillProductTemplate::GetSkillProductTemplate( CHAR *pszName )
{
	AgpdSkillProductTemplate **pvRetVal = (AgpdSkillProductTemplate **) GetObject( pszName );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

/*****************************************************************
*   Function : RemoveSkillProductTemplate
*   Comment  : Remove Skill Template
*   Date&Time : 2002-04-18, 오후 1:25
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
/*
BOOL	AgpaSkillProductTemplate::RemoveSkillProductTemplate(INT32	lTID)
{
	return RemoveObject( lTID );
}
*/
BOOL	AgpaSkillProductTemplate::RemoveSkillProductTemplate(CHAR *pszName)
{
	return RemoveObject( pszName );
}
