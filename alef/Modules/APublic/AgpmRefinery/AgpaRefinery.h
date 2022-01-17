/*====================================================================

	AgpaRefienry.h

====================================================================*/

#ifndef _AGPA_REFINERY_H_
	#define _AGPA_REFINERY_H_

#include "ApAdmin.h"
#include "AgpdRefinery.h"


/********************************************************/
/*		The Definition of AgpaRefineTemplate class		*/
/********************************************************/
//
class AgpaRefineTemplate : public ApAdmin
	{
	public:
		AgpaRefineTemplate();
		virtual ~AgpaRefineTemplate();

		BOOL Add(AgpdRefineTemplate *pAgpdRefineTemplate);
		BOOL Remove(AgpdRefineTemplate *pAgpdRefineTemplate);
		AgpdRefineTemplate* Get(INT32 lID);
	};

/****************************************************/
/*		The Definition of AgpaRefineItem class		*/
/****************************************************/
//
class AgpaRefineItem : public ApAdmin
{
public:
	AgpaRefineItem();
	virtual ~AgpaRefineItem();

    BOOL Add(AgpdRefineItem2 *pAgpdRefineItem);
    BOOL Remove(AgpdRefineItem2 *pAgpdRefineItem);
    AgpdRefineItem2* Get(const char* key);
};
#endif
