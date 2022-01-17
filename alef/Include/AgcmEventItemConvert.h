#ifndef	__AGCMEVENTITEMCONVERT_H__
#define	__AGCMEVENTITEMCONVERT_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventItemConvertD" )
#else
#pragma comment ( lib , "AgcmEventItemConvert" )
#endif
#endif

#include "AgpmCharacter.h"
#include "AgpmItemConvert.h"
#include "AgpmEventItemConvert.h"

#include "AgcmCharacter.h"
#include "AgcmEventManager.h"

typedef enum	_eAgcmEventItemConvertCB {
	AGCMEVENT_ITEMCONVERT_CB_RECEIVE_GRANT			= 0,
} AgcmEventItemConvertCB;

class AgcmEventItemConvert : public AgcModule {
private:
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmItemConvert			*m_pcsAgpmItemConvert;
	AgpmEventItemConvert	*m_pcsAgpmEventItemConvert;

	AgcmCharacter			*m_pcsAgcmCharacter;
	AgcmEventManager		*m_pcsAgcmEventManager;

public:
	AgcmEventItemConvert();
	virtual ~AgcmEventItemConvert();

	BOOL			OnAddModule();

	static BOOL		CBEventItemConvert(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBReceiveGrant(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBReceiveReject(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL			SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGCMEVENTITEMCONVERT_H__