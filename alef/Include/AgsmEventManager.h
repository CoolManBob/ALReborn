#ifndef	__AGSMEVENTMANAGER_H__
#define	__AGSMEVENTMANAGER_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmEventManagerD" )
#else
#pragma comment ( lib , "AgsmEventManager" )
#endif
#endif

#include "ApmEventManager.h"

#include "AgsmCharacter.h"

class AgsmEventManager : public AgsModule {
private:
	ApmEventManager		*m_pcsApmEventManager;
	AgsmAOIFilter		*m_pcsAgsmAOIFilter;
	AgsmCharacter		*m_pcsAgsmCharacter;

public:
	AgsmEventManager();
	virtual ~AgsmEventManager();

	BOOL		OnAddModule();

	static BOOL	CBSendCharacterView(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL		SendEvent(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bIsGroupNID, BOOL bIsExceptSelf);
};

#endif	//__AGSMEVENTMANAGER_H__