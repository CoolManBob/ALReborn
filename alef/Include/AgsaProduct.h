/*====================================================================

	AgsaProduct.h

====================================================================*/

#ifndef _AGSA_PRODUCT_H_
	#define _AGSA_PRODUCT_H_

#include "ApAdmin.h"
#include "AgsdProduct.h"

/********************************************************/
/*		The Definition of AgsaProductItemSet class		*/
/********************************************************/
//
class AgsaProductItemSet : public ApAdmin
	{
	public:
		AgsaProductItemSet();
		virtual ~AgsaProductItemSet();

		BOOL Add(AgsdProductItemSet *pAgsdProductItemSet);
		BOOL Remove(AgsdProductItemSet *pAgsdProductItemSet);
		AgsdProductItemSet* Get(INT32 lID);
	};


#endif
