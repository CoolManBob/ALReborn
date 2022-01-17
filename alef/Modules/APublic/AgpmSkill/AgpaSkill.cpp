#include "AgpaSkill.h"

/*****************************************************************
*   Function : AgpaSkillTemplate()
*   Comment  : 생성자 
*   Date&Time : 2002-04-17, 오후 12:02
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpaSkill::AgpaSkill()
{
}

/*****************************************************************
*   Function : ~AgpaSkillTemplate()
*   Comment  : 소멸자 
*   Date&Time : 2002-04-17, 오후 12:02
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpaSkill::~AgpaSkill()
{
}

/*****************************************************************
*   Function : AddSkillTemplate
*   Comment  : Add Skill Template
*   Date&Time : 2002-04-18, 오후 1:25
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdSkill*   AgpaSkill::AddSkill(AgpdSkill* pcsSkill)
{
	if (AddObject( (PVOID*) &pcsSkill, pcsSkill->m_lID ))
		return pcsSkill;

	return NULL;
}

/*****************************************************************
*   Function : GetSkillTemplate
*   Comment  : Get Skill Template
*   Date&Time : 2002-04-18, 오후 1:25
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpdSkill*   AgpaSkill::GetSkill( INT32	lSkillID )
{
	AgpdSkill **pvRetVal = (AgpdSkill **) GetObject( lSkillID );

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
BOOL	AgpaSkill::RemoveSkill ( INT32 lSkillID )
{
	return RemoveObject( lSkillID );
}
