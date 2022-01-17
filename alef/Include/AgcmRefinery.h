/*===================================================================

	AgcmRefinery.h

===================================================================*/

#ifndef _AGCM_REFINERY_H_
	#define _AGCM_REFINERY_H_

#include "AgcModule.h"
#include "AgpmRefinery.h"


/****************************************************/
/*		The Definition of AgcmRefinery class		*/
/****************************************************/
//
class AgcmRefinery : public AgcModule
	{
	private:
		// ... Related modules
		AgpmRefinery	*m_pAgpmRefinery;

	public:
		AgcmRefinery();
		virtual ~AgcmRefinery();

		//	ApModule inherited
		BOOL OnAddModule();
		
		//	Send packet
		//BOOL SendPacketRefine(INT32 lCID, INT32 lItemTID, INT32 *plSourceItems);
		BOOL SendPacketRefineItem(INT32 lCID, INT32 lID, INT32 *plSourceItems);
	};

#endif
