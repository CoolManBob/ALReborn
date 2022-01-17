#ifndef	__AGSMEVENTCHARCUSTOMIZE_H__
#define	__AGSMEVENTCHARCUSTOMIZE_H__

#include "AgpmEventCharCustomize.h"
#include "AgsmCharacter.h"
#include "AgpmLog.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmEventCharCustomizeD" )
#else
#pragma comment ( lib , "AgsmEventCharCustomize" )
#endif
#endif

class AgsmEventCharCustomize : public AgsModule {
private:
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmEventCharCustomize	*m_pcsAgpmEventCharCustomize;
	AgsmCharacter			*m_pcsAgsmCharacter;
	
	AgpmLog					*m_pcsAgpmLog;

public:
	AgsmEventCharCustomize();
	virtual ~AgsmEventCharCustomize();

	BOOL	OnAddModule();

	static BOOL	CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAfterUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif	//__AGSMEVENTCHARCUSTOMIZE_H__