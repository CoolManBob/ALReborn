/*===========================================================================

	AgsmRemission.h
	
===========================================================================*/

#ifndef _AGSM_REMISSION_H_
	#define _AGSM_REMISSION_H_

#include "AgpmRemission.h"
#include "AgsmCharacter.h"
#include "AgsmItem.h"
#include "AgpmLog.h"


/****************************************************/
/*		The Definition of AgsmRemission class		*/
/****************************************************/
//
class AgsmRemission : public AgsModule
	{
	private :
		AgpmCharacter	*m_pAgpmCharacter;
		AgpmItem		*m_pAgpmItem;
		AgpmGrid		*m_pAgpmGrid;
		AgpmRemission	*m_pAgpmRemission;
		AgpmLog			*m_pAgpmLog;
		
		AgsmCharacter	*m_pAgsmCharacter;
		
		AgsmItem		*m_pAgsmItem;
		
	public :
		AgsmRemission();
		virtual ~AgsmRemission();
		
		//	ApModule inherited
		BOOL	OnAddModule();
		
		//	Operations
		BOOL	OnRemit(AgpdCharacter *pAgpdCharacter, INT8 cType);

		//	Packet
		BOOL	SendRemitResult(AgpdCharacter *pAgpdCharacter, INT8 cType, INT32 lResult);
		
		//	Callbacks
		static BOOL	CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBRemit(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	ETC
		BOOL	_Pay(AgpdCharacter *pAgpdCharacter, INT8 cType);
		BOOL	_RemoveSourceItem(AgpdCharacter *pAgpdCharacter, INT32 lItemTID, INT32 lCount, INT32 lItemType = 0, INT32 lItemOtherType = 0);
		BOOL	WriteRemitLog(AgpdCharacter *pAgpdCharacter, INT8 cType);
	};


#endif