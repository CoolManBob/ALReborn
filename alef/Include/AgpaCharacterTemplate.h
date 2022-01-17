#ifndef _AGPACHARACTERTEMPLATE_H_
#define _AGPACHARACTERTEMPLATE_H_

#include "ApBase.h"
#include "ApAdmin.h"
#include "AgpdCharacter.h"
//#include "AuIniFile.h"

class AgpaCharacterTemplate : public ApAdmin
{
public:
	AgpaCharacterTemplate();
	~AgpaCharacterTemplate();

	//Character Template Data manage function 
	BOOL			LoadTemplateFromIni();	
	
	AgpdCharacterTemplate*		AddCharacterTemplate(AgpdCharacterTemplate* pCharacterT, INT32 lTID);
	AgpdCharacterTemplate*		GetCharacterTemplate(INT32 lTID);
	AgpdCharacterTemplate*		GetCharacterTemplate(CHAR *szTName);
	BOOL						RemoveCharacterTemplate(INT32	lTID, CHAR *szTName = NULL);

	BOOL						AddTemplateStringKey(INT32 lTID, CHAR *szTName);
};
#endif
