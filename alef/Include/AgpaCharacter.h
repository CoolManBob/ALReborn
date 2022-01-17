#ifndef _AGPACHARACTER_H_
#define _AGPACHARACTER_H_

#include "ApBase.h"
#include "ApAdmin.h"
#include "AgpdCharacter.h"


class AgpaCharacter : public ApAdmin
{
public:
	AgpaCharacter();
	~AgpaCharacter();

	//Character data manage function
	AgpdCharacter*  AddCharacter(AgpdCharacter* pCharacter, INT32 lCID, CHAR* szGameID);
	AgpdCharacter*	GetCharacter(INT32 lCID);
	AgpdCharacter*	GetCharacter(CHAR*	szGameID);
	BOOL			RemoveCharacter(INT32 lCID, CHAR* szGameID);

	BOOL			IsCharacter(INT32 ulCID);
	BOOL			IsCharacter(CHAR* szGameID);
};

#endif
