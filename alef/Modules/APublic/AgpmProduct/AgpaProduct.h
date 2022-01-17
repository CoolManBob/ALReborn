/*====================================================================

	AgpaProduct.h

====================================================================*/

#ifndef _AGPA_PRODUCT_H_
	#define _AGPA_PRODUCT_H_

#include "ApAdmin.h"
#include "AgpdProduct.h"


/************************************************************/
/*		The Definition of AgpaProductSkillFactor class		*/
/************************************************************/
//
class AgpaProductSkillFactor : public ApAdmin
	{
	public:
		AgpaProductSkillFactor();
		virtual ~AgpaProductSkillFactor();

		BOOL Add(AgpdProductSkillFactor *pAgpdProductSkillFactor);
		BOOL Remove(AgpdProductSkillFactor *pAgpdProductSkillFactor);
		AgpdProductSkillFactor* Get(CHAR *pszSkill);
	};


/********************************************************/
/*		The Definition of AgpaComposeTemplate class		*/
/********************************************************/
//
class AgpaComposeTemplate : public ApAdmin
	{
	public:
		AgpaComposeTemplate();
		virtual ~AgpaComposeTemplate();

		BOOL Add(AgpdComposeTemplate *pAgpdComposeTemplate);
		BOOL Remove(AgpdComposeTemplate *pAgpdComposeTemplate);
		AgpdComposeTemplate* Get(INT32 lComposeID);
		AgpdComposeTemplate* Get(CHAR pszCompose);
	};

#endif
