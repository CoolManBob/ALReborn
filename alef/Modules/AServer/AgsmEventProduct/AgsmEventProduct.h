/*==============================================================

	AgsmEventProduct.h
	
==============================================================*/

#ifndef	_AGSMEVENT_PRODUCT_H_
	#define _AGSMEVENT_PRODUCT_H_

#include "AgpmSkill.h"
#include "AgpmProduct.h"
#include "AgpmEventProduct.h"
#include "AgsmCharacter.h"
#include "AgsmProduct.h"
#include "AgpmLog.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgsmEventProductD.lib")
#else
	#pragma comment(lib, "AgsmEventProduct.lib")
#endif
#endif


/********************************************************/
/*		The Definition of AgsmEventProduct class		*/
/********************************************************/
//
class AgsmEventProduct : public AgsModule
	{
	private:
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmSkill			*m_pAgpmSkill;
		AgpmProduct			*m_pAgpmProduct;
		AgpmEventProduct	*m_pAgpmEventProduct;
		AgsmCharacter		*m_pAgsmCharacter;
		AgsmProduct			*m_pAgsmProduct;
		AgpmLog				*m_pAgpmLog;
		
		
	public:
		AgsmEventProduct();
		virtual ~AgsmEventProduct();
		
		// ApModule inherited
		BOOL OnAddModule();

		// Callback		
		static BOOL CBProductEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBLearnSkill(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBBuyCompose(PVOID pData, PVOID pClass, PVOID pCustData);

		// Packet
		BOOL	SendPacketLearnSkillResult(AgpdCharacter *pAgpdCharacter, INT32 lSkillTID, INT32 lResult);
		BOOL	SendPacketBuyComposeResult(AgpdCharacter *pAgpdCharacter, INT32 lComposeID, INT32 lResult);
	};

#endif
