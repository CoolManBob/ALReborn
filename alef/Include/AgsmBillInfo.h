#ifndef	__AGSMBILLINFO_H__
#define	__AGSMBILLINFO_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmBillInfoD" )
#else
#pragma comment ( lib , "AgsmBillInfo" )
#endif
#endif

#include "AgpmBillInfo.h"

#include "AgsmCharacter.h"

class AgsmBillInfo : public AgsModule {
private:
	AgpmBillInfo		*m_pcsAgpmBillInfo;
	AgsmCharacter		*m_pcsAgsmCharacter;

public:
	AgsmBillInfo();
	virtual ~AgsmBillInfo();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();

	static BOOL	CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	BOOL SendCharacterBillingInfo(AgpdCharacter* pcsCharacter);
};

#endif	//__AGSMBILLINFO_H__