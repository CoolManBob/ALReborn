/*====================================================================

	AgsmGamble.h
	
====================================================================*/


#ifndef _AGSM_GAMBLE_H_
	#define _AGSM_GAMBLE_H_


#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmGamble.h"
#include "AgpmDropItem2.h"
#include "AgsmCharacter.h"
#include "AgsmItemManager.h"
#include "AgpmLog.h"
#include "AuRandomNumber.h"


/************************************************/
/*		The Definition of AgsmGamble class		*/
/************************************************/
//
class AgsmGamble : public AgsModule
	{
	private:
		//	Related modules
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmItem			*m_pAgpmItem;
		AgpmGamble			*m_pAgpmGamble;
		AgpmDropItem2		*m_pAgpmDropItem2;
		
		AgsmCharacter		*m_pAgsmCharacter;
		AgsmItemManager		*m_pAgsmItemManager;

		AgpmLog				*m_pAgpmLog;
		
		MTRand				m_Random;
		
	public:
		AgsmGamble();
		virtual ~AgsmGamble();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();

		//	Operations
		BOOL	OnGamble(AgpdCharacter *pAgpdCharacter, INT32 lGambleTID);
		
		//	Callbacks
		static BOOL	CBGamble(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Packet Send
		BOOL	SendResultGamble(AgpdCharacter *pAgpdCharacter, INT32 lItemTID, INT32 lResult);
		
	protected:
		//	Helper
		UINT32	_GetCharacterNID(INT32 lCID);
		UINT32	_GetCharacterNID(AgpdCharacter *pAgpdCharacter);
		
		//	Log
		BOOL	WriteGambleLog(AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem, INT32 lCharge);
	};


#endif