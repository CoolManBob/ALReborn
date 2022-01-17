/*=========================================================

	AgsmEventCook.h

==========================================================*/

#ifndef _AGSM_EVENT_COOK_H_
	#define _AGSM_EVENT_COOK_H_

#include "AgpmEventCook.h"
#include "AgsmCharacter.h"

#ifdef _DEBUG
	#pragma comment(lib, "AgsmEventCookD.lib")
#else
	#pragma comment(lib, "AgsmEventCook.lib")
#endif

/****************************************************/
/*		The Definition of AgsmEventCook class		*/
/****************************************************/
//
class AgsmEventCook : public AgsModule
	{
	private:
		AgpmEventCook* m_pAgpmEventCook;
		AgsmCharacter* m_pAgsmCharacter;
		
	public:
		AgsmEventCook();
		virtual ~AgsmEventCook();
		
		BOOL OnAddModule();
		
		static BOOL CBCookEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	};

#endif
