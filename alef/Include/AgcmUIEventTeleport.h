//	AgcmUIEventTeleport.h
/////////////////////////////////////////////////////////////

#ifndef	__AGCMUIEVENTTELEPORT_H__
#define	__AGCMUIEVENTTELEPORT_H__

#include "ApmEventManager.h"

#include "AgcmEventTeleport.h"
#include "AgcmMap.h"

#include "AgcmUIManager2.h"
#include "AgcmUIMain.h"

#include "AgcmChatting2.h"
#include "AgpmSiegeWar.h"

#define AGCMUI_TELEPORT_MAX_TARGET_POINT	20


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIEventTeleportD" )
#else
#pragma comment ( lib , "AgcmUIEventTeleport" )
#endif
#endif


#define AGCMUITELEPORT_EVENT_LOADING_WINDOW_OPEN			"TELEPORT_LOADING_WINDOW_OPEN"
#define AGCMUITELEPORT_EVENT_LOADING_WINDOW_CLOSE			"TELEPORT_LOADING_WINDOW_CLOSE"
#define AGCMUITELEPORT_EVENT_SELECT_WINDOW_CLOSE			"TELEPORT_SELECT_WINDOW_CLOSE"

#define	UI_MESSAGE_ID_TELEPORT_TRANSFORM_CHAR				"Teleport_TransformChar"
#define	UI_MESSAGE_ID_TELEPORT_NAME_RETURN_ONLY				"Teleport_ReturnOnly"
#define	UI_MESSAGE_ID_TELEPORT_COMBAT_MODE					"Teleport_CombatMode"

const int	AGCMUITELEPORT_CLOSE_UI_DISTANCE			= 150;

class AgcmUIEventTeleport : public AgcModule {
private:
	ApmEventManager*		m_pcsApmEventManager;

	AgcmUIManager2*			m_pcsAgcmUIManager2;
	AgcmEventTeleport*		m_pcsAgcmEventTeleport;
	AgpmEventTeleport*		m_pcsAgpmEventTeleport;

	AgpmCharacter*			m_pcsAgpmCharacter;
	AgcmCharacter*			m_pcsAgcmCharacter;

	AgcmChatting2*			m_pcsAgcmChatting2;

	AgcmMap*				m_pcsAgcmMap;

	AgpmSiegeWar*			m_pcsAgpmSiegeWar;

	INT32					m_lTargetPoint;
	AgpdTeleportPoint *		m_apcsTargetPoint[AGCMUI_TELEPORT_MAX_TARGET_POINT];
	INT32					m_lDisableTargetPoint[AGCMUI_TELEPORT_MAX_TARGET_POINT];

	AgcdUIUserData *		m_pstTargetUD;

	// event id
	///////////////////////////////////////////////////////////////
	INT32					m_lEventSelectTeleportWindowClose;

	INT32					m_lEventLoadingWindowOpen;
	INT32					m_lEventLoadingWindowClose;

	INT32					m_lEventIDSelectTeleportPoint;

	INT32					m_lEventConfirmPvPZone;
	INT32					m_lEventNotEnoughMoney;

	INT32					m_lEventDisplayNormal;
	INT32					m_lEventDisplayNoMoney;
	INT32					m_lEventDisplayDisable;

	BOOL					m_bIsStartedMapLoading;

	BOOL					m_bIsOpenedTeleportListWindow;

	AuPOS					m_stTeleportOpenPos;

	CHAR					*m_szTeleportSelfName;

	AgpdTeleportSpecialType	m_ePointSpecialType;

	ApdEvent				*m_pcsEvent;
	AgpdTeleportPoint		*m_pcsPvPZonePoint;

	BOOL					m_bIsDisablePoint;

public:
	AgcmUIEventTeleport();
	virtual ~AgcmUIEventTeleport();
		
	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();
	BOOL					OnIdle(UINT32 ulClockCount);

	BOOL					AddFunctions();
	BOOL					AddEvents();
	BOOL					AddUserData();
	BOOL					AddDisplay();

	BOOL					RefreshTeleportList();

	static BOOL				CBRequestTeleport(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL				CBRefreshList(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL				CBSelectTeleportPoint(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBReceivedTeleportStart(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBReceivedTeleportCanceled(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBMapLoadEnd(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBCannotUseForTransforming(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBCannotUseForCombatMode(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBDisplayTeleport(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL				CBDisplaySelfTeleport(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL				CBDisplayTeleportFee(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL				CBConfirmPvPZone(PVOID pClass, INT32 lTrueCancel, CHAR *szInputMessage);

	BOOL					IsDisablePoint(AgpdTeleportPoint *pcsTeleportPoint);

	BOOL					IsStartedMapLoading() {return m_bIsStartedMapLoading; }
	void					CloseEventLoadingWindow() { m_pcsAgcmUIManager2->ThrowEvent(m_lEventLoadingWindowClose); m_bIsStartedMapLoading = FALSE; }
};

#endif	//__AGCMUIEVENTTELEPORT_H__