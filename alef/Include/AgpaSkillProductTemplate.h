#ifndef			_AGPASKILLPRODUCTTEMPLATE_H_
#define			_AGPASKILLPRODUCTTEMPLATE_H_

#include "ApBase.h"
#include "AgpdSkill.h"
#include "ApAdmin.h"
#include "AgpdSkillProduct.h"

class AgpaSkillProductTemplate : public ApAdmin
{
public:
	AgpaSkillProductTemplate();
	~AgpaSkillProductTemplate();
	
	//Skill Template Manage Function
	AgpdSkillProductTemplate*	AddSkillProductTemplate(AgpdSkillProductTemplate* pcsSkillTemplate);
	//AgpdSkillProductTemplate*	GetSkillProductTemplate(INT32 lTID);
	AgpdSkillProductTemplate*	GetSkillProductTemplate(CHAR *pszName);
	//BOOL						RemoveSkillProductTemplate(INT32 lTID);
	BOOL						RemoveSkillProductTemplate(CHAR *pszName);
};

#endif 	