#ifndef	_AGPASKILLSPECIALIZETEMPLATE_H_
#define	_AGPASKILLSPECIALIZETEMPLATE_H_

#include "ApAdmin.h"
#include "AgpdSkill.h"

class AgpaSkillSpecializeTemplate : public ApAdmin
{
public:
	AgpaSkillSpecializeTemplate();
	~AgpaSkillSpecializeTemplate();
	
	//Skill Template Manage Function
	AgpdSkillSpecializeTemplate*	AddSkillSpecializeTemplate(AgpdSkillSpecializeTemplate* pcsSpecializeTemplate);
	AgpdSkillSpecializeTemplate*	GetSkillSpecializeTemplate(INT32 lTID);
	BOOL							RemoveSkillSpecializeTemplate(INT32 lTID);
};

#endif	//_AGPASKILLSPECIALIZETEMPLATE_H_
