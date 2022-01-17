#ifndef	__AGCMUISTATUS_H__
#define	__AGCMUISTATUS_H__

#include "ApmEventManager.h"
#include "AgpmUIStatus.h"

#include "AgcmCharacter.h"
#include "AgcmUIMain.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIStatusD" )
#else
#pragma comment ( lib , "AgcmUIStatus" )
#endif
#endif

class AgcmUIStatus : public AgcModule {
private:
	ApmEventManager			*m_pcsApmEventManager;
	AgpmSkill				*m_pcsAgpmSkill;
	AgpmUIStatus			*m_pcsAgpmUIStatus;

	AgcmCharacter			*m_pcsAgcmCharacter;
	AgcmUIMain				*m_pcsAgcmUIMain;
	AgcmItem				*m_pcsAgcmItem;

	UINT32					m_ulUpdateAutoUseHPGageTimeMSec;
	UINT32					m_ulUpdateAutoUseMPGageTimeMSec;

public:
	AgcmUIStatus();
	~AgcmUIStatus();

	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();
	BOOL					OnIdle(UINT32 ulClockCount);

	static BOOL				CBUpdateViewHelmetOption(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBReceiveQuickBeltInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBAddShortcut(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBRemoveShortcut(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBUpdateHPPotion(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBUpdateMPPotion(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBUpdateAutoUseHPGage(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBUpdateAutoUseMPGage(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL					SendPacketUpdateQBeltItem(AgpdCharacter *pcsCharacter, INT32 lIndex, ApBase *pcsItemBase, INT32 lHPPotionTID = AP_INVALID_IID, INT32 lMPPotionTID = AP_INVALID_IID, INT8 cAutoUseHPGage = (-1), INT8 cAutoUseMPGage = (-1));
	BOOL					SendPacketUpdateViewHelmetOption(AgpdCharacter *pcsCharacter, INT8 cOptionViewHelmet);
};

#endif	//__AGCMUISTATUS_H__