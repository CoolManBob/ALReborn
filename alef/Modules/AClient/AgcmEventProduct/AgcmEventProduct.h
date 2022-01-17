/*===================================================================

	AgcmEventProduct.h
	
===================================================================*/

#ifndef _AGCMEVENT_PRODUCT_H_
	#define _AGCMEVENT_PRODUCT_H_

#include "AgpmEventProduct.h"
#include "AgcmCharacter.h"
#include "AgcmEventManager.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgcmEventProductD.lib")
#else
	#pragma comment(lib, "AgcmEventProduct.lib")
#endif
#endif

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum eAGCMEVENT_PRODUCT_CB
	{
	AGCMEVENT_PRODUCT_CB_GRANT = 0,
	};


/************************************************/
/*		The Definition of AgcmEventProduct		*/
/************************************************/
//
class AgcmEventProduct : public AgcModule
	{
	private:
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmEventProduct	*m_pAgpmEventProduct;
		AgcmCharacter		*m_pAgcmCharacter;
		AgcmEventManager	*m_pAgcmEventManager;

	public:
		AgcmEventProduct();
		virtual ~AgcmEventProduct();

		// ApModule inherited
		BOOL OnAddModule();
		
		// Callback setting
		BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		// Callbacks
		static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);	
		
		//
		BOOL	SendLearnSkill(ApdEvent *pApdEvent, INT32 lSkillTID);
		BOOL	SendBuyCompose(ApdEvent *pApdEvent, INT32 lComposeID);
	};

#endif
