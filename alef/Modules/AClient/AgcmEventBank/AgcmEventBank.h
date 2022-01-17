#ifndef	__AGCMEVENTBANK_H__
#define	__AGCMEVENTBANK_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventBankD" )
#else
#pragma comment ( lib , "AgcmEventBank" )
#endif
#endif

#include "AgpmEventBank.h"

#include "AgcmCharacter.h"
#include "AgcmEventManager.h"

typedef enum AgcmEventBankCBID {
	AGCMEVENTBANK_CB_GRANT_EVENT_BANK			= 0,
} AgcmEventBankCBID;

class AgcmEventBank : public AgcModule {
private:
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmEventBank		*m_pcsAgpmEventBank;

	AgcmCharacter		*m_pcsAgcmCharacter;
	AgcmEventManager	*m_pcsAgcmEventManager;

public:
	AgcmEventBank();
	virtual ~AgcmEventBank();

	BOOL				OnAddModule();

	BOOL				SetCallbackGrantEventBank(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL			CBEventBank(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEventGrantAnywhere(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif	//__AGCMEVENTBANK_H__