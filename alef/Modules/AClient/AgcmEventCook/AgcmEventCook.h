/*=========================================================

	AgcmEventCook.h

==========================================================*/

#ifndef _AGCM_EVENT_COOK_H_
	#define _AGCM_EVENT_COOK_H_

#include "AgpmEventCook.h"

#include "AgcmCharacter.h"
#include "AgcmEventManager.h"

#ifdef _DEBUG
	#pragma comment(lib, "AgcmEventCookD.lib")
#else
	#pragma comment(lib, "AgcmEventCook.lib")
#endif

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
typedef enum _eAgcmEventCookCallback
	{
	AGCMEVENTCOOK_CB_GRANT = 0,
	} eAgcmEventCookCallBack;


/****************************************************/
/*		The Definition of AgcmEventCook class		*/
/****************************************************/
//
class AgcmEventCook : public AgcModule
	{
	private:
		AgpmCharacter* m_pcsAgpmCharacter;
		AgpmEventCook* m_pcsAgpmEventCook;

		AgcmCharacter* m_pcsAgcmCharacter;
		AgcmEventManager* m_pcsAgcmEventManager;

	public:
		AgcmEventCook();
		virtual ~AgcmEventCook();

		BOOL OnAddModule();
		
		BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);	
	};

#endif
