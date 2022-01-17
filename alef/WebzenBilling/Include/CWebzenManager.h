#ifndef __CLASS_WEBZEN_MANAGER_H__
#define __CLASS_WEBZEN_MANAGER_H__






#include "CWebzenBilling.h"
#include "CWebzenShop.h"




class CWebzenManager : public CWebzenBilling, public CWebzenShop
{
public :
	CWebzenManager( void );
	virtual ~CWebzenManager( void );
};






#endif