#ifndef			__AGPASKILL_H__
#define			__AGPASKILL_H__

#include "ApBase.h"
#include "AgpdSkill.h"
#include "ApAdmin.h"

class AgpaSkill : public ApAdmin
{
public:
	AgpaSkill();
	~AgpaSkill();
	
	//Skill Template Manage Function
	AgpdSkill*	AddSkill(AgpdSkill* pcsSkill);
	AgpdSkill*	GetSkill(INT32 lSkillID);
	BOOL		RemoveSkill(INT32 lSkillID);
};

#endif 	//__AGPASKILL_H__