#ifndef	__AGPAAI2_H__
#define	__AGPAAI2_H__

#include "ApAdmin.h"
#include "AgpdAI2.h"

class AgpaAI2Template : public ApAdmin  
{
public:
	AgpaAI2Template();
	virtual ~AgpaAI2Template();

	AgpdAI2Template		*AddAITemplate(AgpdAI2Template *pstTemplate);
	AgpdAI2Template		*GetAITemplate(INT32 lTID);
	BOOL				RemoveAITemplate(INT32 lTID);
	AgpdAI2Template		*AgpaAI2Template::GetTemplateSequence(INT32 *plIndex);
};

#endif
