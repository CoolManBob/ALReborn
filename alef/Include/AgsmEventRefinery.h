/*=================================================================

	AgsmEventRefinery.h

=================================================================*/

#ifndef _AGSM_EVENT_REFINERY_H_
	#define _AGSM_EVENT_REFINERY_H_

#include "AgpmEventRefinery.h"
#include "AgsmCharacter.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgsmEventRefineryD.lib")
#else
	#pragma comment(lib, "AgsmEventRefinery.lib")
#endif
#endif

/********************************************************/
/*		The Definition of AgsmEventRefinery class		*/
/********************************************************/
//
class AgsmEventRefinery : public AgsModule
	{
	private:
		AgpmEventRefinery	*m_pAgpmEventRefinery;
		AgsmCharacter		*m_pAgsmCharacter;
		
	public:
		AgsmEventRefinery();
		virtual ~AgsmEventRefinery();

		// ... ApModule inherited		
		BOOL OnAddModule();
		
		// ... Callback
		static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	};

#endif
