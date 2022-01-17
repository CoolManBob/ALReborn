/*===================================================================

	AgcmProduct.h

===================================================================*/

#ifndef _AGCM_PRODUCT_H_
	#define _AGCM_PRODUCT_H_

#include "AgcModule.h"
#include "AgpmProduct.h"
#include "AgpmCharacter.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgcmProductD.lib")
#else
	#pragma comment(lib, "AgcmProduct.lib")
#endif
#endif


/************************************************/
/*		The Definition of AgcmProduct class		*/
/************************************************/
//
class AgcmProduct : public AgcModule
	{
	private:
		// ... Related modules
		AgpmProduct*	m_pAgpmProduct;
		AgpmCharacter*	m_pAgpmCharacter;

	public:
		AgcmProduct();
		virtual ~AgcmProduct();

		// ... ApModule inherited
		BOOL OnAddModule();
		BOOL OnInit();
		BOOL OnDestroy();
		
		// ... Send packet
		BOOL SendPacketCompose(INT32 lCID, INT32 lSkillID, INT32 lComposeID, INT32 lReceipeID);
	};

#endif
