#ifndef __AGSM_TIMER_H__
#define __AGSM_TIMER_H__

#include "AgpmTimer.h"
#include "AgpmEventNature.h"

#include "AgsmCharacter.h"
#include "AgsmCharManager.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmTimerD" )
#else
#pragma comment ( lib , "AgsmTimer" )
#endif
#endif

class AgsmTimer : public AgsModule
{
protected:
	AgpmTimer			*m_pcsAgpmTimer;
	AgpmEventNature		*m_pcsAgpmEventNature;

	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmCharManager		*m_pcsAgsmCharManager;

public:
	AgsmTimer();
	virtual ~AgsmTimer();

	BOOL				OnAddModule();

	BOOL				OnInit();
	BOOL				OnDestroy();

	BOOL				SendGameTime(UINT32 ulNID);

	static BOOL			CBConnectedClient(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif // __AGSM_TIMER_H__