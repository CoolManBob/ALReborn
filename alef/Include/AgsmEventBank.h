#ifndef	__AGSMEVENTBANK_H__
#define	__AGSMEVENTBANK_H__

#include "AgpmEventBank.h"

#include "AgsmCharacter.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmEventBankD" )
#else
#pragma comment ( lib , "AgsmEventBank" )
#endif
#endif


class AgsmEventBank : public AgsModule {
private:
	AgpmEventBank		*m_pcsAgpmEventBank;

	AgsmCharacter		*m_pcsAgsmCharacter;

public:
	AgsmEventBank();
	virtual ~AgsmEventBank();

	BOOL				OnAddModule();

	static BOOL			CBBankEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	
	BOOL				OpenAnywhere(AgpdCharacter *pcsCharacter);
};


#endif	//__AGSMEVENTBANK_H__