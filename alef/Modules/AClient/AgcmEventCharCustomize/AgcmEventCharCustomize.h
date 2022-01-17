#if !defined(__AGCMEVENTCHARCUSTOMIZE_H__)
#define	__AGCMEVENTCHARCUSTOMIZE_H__

#include "AgpmEventCharCustomize.h"

#include "AgcmCharacter.h"
#include "AgcmEventManager.h"

typedef enum	_AgcmEventCharCustomizeCBID {
	AGCMEVENTCHARCUSTOMIZE_CB_GRANT_EVENT		= 0,
} AgcmEventCharCustomizeCBID;

class AgcmEventCharCustomize : public AgcModule {
private:
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmEventCharCustomize	*m_pcsAgpmEventCharCustomize;
	AgcmCharacter			*m_pcsAgcmCharacter;
	AgcmEventManager		*m_pcsAgcmEventManager;

public:
	AgcmEventCharCustomize();
	virtual ~AgcmEventCharCustomize();

	BOOL	OnAddModule();

	static BOOL	CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBEventCharCustomize(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	SetCallbackGrantEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGCMEVENTCHARCUSTOMIZE_H__