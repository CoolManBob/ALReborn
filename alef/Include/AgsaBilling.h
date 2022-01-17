/*============================================================

	AgsaBilling.h
	
============================================================*/

#ifndef _AGSA_BILLING_H_
	#define _AGSA_BILLING_H_

#include "ApBase.h"
#include "ApAdmin.h"
#include "ApModule.h"
#include "AgsdBilling.h"


/************************************************/
/*		The Definition of AgsaBilling class		*/
/************************************************/
//
class AgsaBilling : public ApAdmin
	{
	public:
		AgsaBilling();
		~AgsaBilling();
		
		BOOL			Add(AgsdBilling *pAgsdBilling);
		BOOL			Add(AgsdBilling *pAgsdBilling, CHAR* AccountID);//JK_ºô¸µ
		BOOL			Remove(AgsdBilling *pAgsdBilling);
		BOOL			Remove(INT32 lID);
		BOOL			Remove(CHAR* AccountID);//JK_ºô¸µ
		AgsdBilling*	Get(INT32 lID);
		AgsdBilling*	Get(CHAR* szAccountID);//JK_ºô¸µ
	};


#endif