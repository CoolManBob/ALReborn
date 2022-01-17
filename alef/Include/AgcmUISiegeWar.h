#pragma once

#ifndef _AGCMUISIEGEWAR_H_
#define _AGCMUISIEGEWAR_H_

#include "AgpmGrid.h"
#include "AgcmSiegeWar.h"
#include "AgcmUIManager2.h"
#include "AgcmEventSiegeWarNPC.h"
#include "AgcmUICooldown.h"
#include "AgcmUIItem.h"
#include "AgcmChatting2.h"
#include "AgcmUICharacter.h"

#include "AgpmAdmin.h"

class AgcmTextBoardMng;

#define	UI_MESSAGE_ID_SIEGEWAR_MAIN_TEXT					"SiegeWarMainText"
#define	UI_MESSAGE_ID_SIEGEWAR_PREFIX_NPC_NAME				"SiegeWarPrefixNPCName"
#define	UI_MESSAGE_ID_SIEGEWAR_DATE							"SiegeWarDate"
#define	UI_MESSAGE_ID_SIEGEWAR_DATE_YEAR					"SiegeWarDateYear"
#define	UI_MESSAGE_ID_SIEGEWAR_DATE_MONTH					"SiegeWarDateMonth"
#define	UI_MESSAGE_ID_SIEGEWAR_DATE_DAY						"SiegeWarDateDay"
#define	UI_MESSAGE_ID_SIEGEWAR_DATE_HOUR					"SiegeWarDateHour"
#define	UI_MESSAGE_ID_SIEGEWAR_DATE_PM						"SiegeWarDatePM"
#define	UI_MESSAGE_ID_SIEGEWAR_COMPLETE						"SiegeWarComplete"
#define	UI_MESSAGE_ID_SIEGEWAR_RESERVE						"SiegeWarReserve"
#define	UI_MESSAGE_ID_SIEGEWAR_SET_TIME_HELP				"SiegeWarSetTimeHelp"
#define	UI_MESSAGE_ID_SIEGEWAR_REMAIN_TIME					"SiegeWarRemainTime"
#define	UI_MESSAGE_ID_SIEGEWAR_DEFENSE_GUILD_HELP			"SiegeWarDefenseGuildHelp"
#define	UI_MESSAGE_ID_SIEGEWAR_NOT_SELECTED_DEFENSE_GUILD	"SiegeWarNotSelectedDefenseGuild"
#define	UI_MESSAGE_ID_SIEGEWAR_DEFENSE_GUILD				"SiegeWarDefenseGuild"
#define	UI_MESSAGE_ID_SIEGEWAR_CATAPULT_DESC				"SiegeWarCatapultDesc"
#define	UI_MESSAGE_ID_SIEGEWAR_ATK_RES_TOWER_DESC			"SiegeWarAtkResTowerDesc"

#define	UI_MESSAGE_ID_SIEGEWAR_START_CARVING				"SiegeWarStartCarving"
#define	UI_MESSAGE_ID_SIEGEWAR_CANCEL_CARVING				"SiegeWarCancelCarving"
#define	UI_MESSAGE_ID_SIEGEWAR_END_CARVING					"SiegeWarEndCarving"

#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_START						"SiegeWarStatusStart"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_REMAIN_TIME_TO_START		"SiegeWarStatusRemainTimeToStart"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_REMAIN_TIME_TO_TERMINATE	"SiegeWarStatusRemainTimeToTerminate"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_DESTROY_OBJECT			"SiegeWarStatusDestroyObject"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_DESTROY_TOWER				"SiegeWarStatusDestroyTower"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_DESTROY_REVIVAL			"SiegeWarStatusDestroyRevival"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_DESTROY_CATAPULT			"SiegeWarStatusDestroyCatapult"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_RELEASE_THRONE			"SiegeWarStatusReleaseThrone"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_ACTIVE_ARCHON_LEFT_EYE	"SiegeWarStatusActiveArchonLeftEye"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_ACTIVE_ARCHON_RIGHT_EYE	"SiegeWarStatusActiveArchonRightEye"
#define UI_MESSAGE_ID_SIEGEWAR_STATUS_OPEN_ALL_EYES				"SiegeWarStatusOpenAllEyes"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_FAILED_ACTIVE_ARCHON_EYE	"SiegeWarStatusFailedActiveArchonEye"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_START_CARVE_GUILD			"SiegeWarStatusStartCarveGuild"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_CANCEL_CARVE_GUILD		"SiegeWarStatusCancelCarveGuild"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_NEW_CARVE_GUILD			"SiegeWarStatusNewCarveGuild"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_STOP_BATTLE_UNTIL_END		"SiegeWarStatusStopBattleUntilEnd"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_TERMINATE					"SiegeWarStatusTerminate"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_ACTIVE_LEFT_ATTACK_RES_TOWER		"SiegeWarStatusActiveLeftAttackResTower"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_ACTIVE_RIGHT_ATTACK_RES_TOWER		"SiegeWarStatusActiveRightAttackResTower"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_DISABLE_LEFT_ATTACK_RES_TOWER		"SiegeWarStatusDisableLeftAttackResTower"
#define	UI_MESSAGE_ID_SIEGEWAR_STATUS_DISABLE_RIGHT_ATTACK_RES_TOWER	"SiegeWarStatusDisableRightAttackResTower"

#define	UI_MESSAGE_ID_SIEGEWAR_OBJECT_GUARD_TOWER				"SiegeWarObjectGuardTower"
#define	UI_MESSAGE_ID_SIEGEWAR_OBJECT_INNER_GATE				"SiegeWarObjectInnerGate"
#define	UI_MESSAGE_ID_SIEGEWAR_OBJECT_OUTER_GATE				"SiegeWarObjectOuterGate"
#define	UI_MESSAGE_ID_SIEGEWAR_OBJECT_LIFE_TOWER				"SiegeWarObjectLiftTower"
#define	UI_MESSAGE_ID_SIEGEWAR_OBJECT_DEFENSE_RES_TOWER			"SiegeWarObjectDefenseResTower"
#define	UI_MESSAGE_ID_SIEGEWAR_OBJECT_ATTACK_RES_TOWER			"SiegeWarObjectAttackResTower"
#define	UI_MESSAGE_ID_SIEGEWAR_OBJECT_CATAPULT					"SiegeWarObjectCatapult"

#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_START_TIME				"SiegeWarMsgStartTime"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_END_TIME					"SiegeWarMsgEndTime"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_ALREADY_USE				"SiegeWarMsgAlreadyUse"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_ALREADY_REPAIR			"SiegeWarMsgAlreadyRepair"
#define UI_MESSAGE_ID_SIEGEWAR_MESSAGE_REACHED_LIMITATION		"SiegeWarStatusReachedLimitation"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_ONLY_GUILD_MASTER		"SiegeWarMsgOnlyGuildMaster"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_START_REPAIR				"SiegeWarMsgStartRepair"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_END_REPAIR				"SiegeWarMsgEndRepair"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_LEFT						"SiegeWarMsgLeft"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_RIGHT					"SiegeWarMsgRight"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_CATAPULT					"SiegeWarMsgCatapult"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_ATKRESTOWER				"SiegeWarMsgAtkResTower"
#define	UI_MESSAGE_ID_SIEGEWAR_MESSAGE_ALREADY_ACTIVE			"SiegeWarAlreadyActive"

#define	UI_MESSAGE_ID_SIEGEWAR_NOT_STARTED						"SiegeWarNotStarted"

const int	AGCMUISIEGEWAR_MAX_SELECT_DATE			= 2;
const int	AGCMUISIEGEWAR_MAX_SELECT_HOUR			= 3;
const int	AGCMUISIEGEWAR_MAX_COMBO_STRING			= 64;
const int	AGCMUISIEGEWAR_MAX_ATTACK_GUILD_LIST	= 15;
const int	AGCMUISIEGEWAR_MAX_DEFENSE_GUILD_LIST	= 15;

typedef enum _eAgcmUISiegeWarCB
{
	AGCMUISIEGEWAR_CB_RECEIVE_ATTACK_GUILD_LIST = 0,
	AGCMUISIEGEWAR_CB_RECEIVE_ENEMY_GUILD_LIST,
	AGCMUISIEGEWAR_CB_MAX,
} eAgcmUISiegeWarCB;

typedef struct AtttackGuildList {
	CHAR	szGuildName[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
	CHAR	szGuildMasterName[AGPACHARACTER_MAX_ID_STRING + 1];
	UINT32	ulGuildPoint;
} AttackGuildList;

//class AgcmChatting2;

class AgcmUISiegeWar : public AgcModule {
public:
	AgcmUISiegeWar();
	virtual ~AgcmUISiegeWar();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);

private:
	ApmMap					*m_pcsApmMap;
	ApmEventManager			*m_pcsApmEventManager;
	AgpmItem				*m_pcsAgpmItem;
	AgpmSiegeWar			*m_pcsAgpmSiegeWar;
	AgpmEventSiegeWarNPC	*m_pcsAgpmEventSiegeWarNPC;
	AgpmGuild				*m_pcsAgpmGuild;
	AgpmAdmin				*m_pcsAgpmAdmin;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgcmCharacter			*m_pcsAgcmCharacter;
	AgcmSiegeWar			*m_pcsAgcmSiegeWar;
	AgcmEventSiegeWarNPC	*m_pcsAgcmEventSiegeWarNPC;
	AgcmUIManager2			*m_pcsAgcmUIManager2;
	AgpmGrid				*m_pcsAgpmGrid;
	AgcmUICharacter			*m_pcsAgcmUICharacter;
	AgcmUICooldown			*m_pcsAgcmUICooldown;
	AgcmChatting2			*m_pcsAgcmChatting2;
	AgcmUIItem				*m_pcsAgcmUIItem;
	AgcmTextBoardMng		*m_pcsAgcmTextBoard;
	AgcmUIControl			*m_pcsAgcmUIControl;

	AgcdUIUserData	*m_pcsUserDataSiegeWar;
	AgcdUIUserData	*m_pcsUserDataDateCombo;
	AgcdUIUserData	*m_pcsUserDataHourCombo;

	AgcdUIUserData	*m_pcsUserDataAttackList;
	AgcdUIUserData	*m_pcsUserDataAttackCurrentPage;

	AgcdUIUserData	*m_pcsUserDataDefenseList;
	AgcdUIUserData	*m_pcsUserDataDefenseCurrentPage;

	AgcdUIUserData	*m_pcsUserDataCatapultGrid;
	AgcdUIUserData	*m_pcsUserDataAtkResTowerGrid;

	AgcdUIUserData	*m_pcsUserDataActiveCatapultRepairButton;
	AgcdUIUserData	*m_pcsUserDataActiveAtkResTowerRepairButton;

	AgcdUIUserData	*m_pcsUserDataActiveCatapultActiveButton;
	AgcdUIUserData	*m_pcsUserDataActiveAtkResTowerActiveButton;

	AgcdUIUserData	*m_pcsUserDataActiveAttackApplButton;

	AgcdUIUserData	*m_pcsUserDataUseCharName;
	AgcdUIUserData	*m_pcsUserDataUseRepairBar;

	INT32			m_lEventOpenSiegeWarMainUI;
	INT32			m_lEventOpenSetTimeUI;
	INT32			m_lEventOpenApplicationDefenseUI;
	INT32			m_lEventOpenSelectDefenseGuildUI;
	INT32			m_lEventOpenApplicationAttackUI;
	INT32			m_lEventCloseSiegeWarUI;

	INT32			m_lEventOpenCatapultUI;
	INT32			m_lEventOpenAtkResTowerUI;
	INT32			m_lEventCloseCatapultUI;
	INT32			m_lEventCloseAtkResTowerUI;

	INT32			m_lEventGetMainUIDescEdit;
	INT32			m_lEventGetMainUISiegeWarTimeEdit;

	INT32			m_lEventGetAttackHelpEdit;
	INT32			m_lEventGetAttackSiegeWarTimeEdit;

	INT32			m_lEventGetDefenseHelpEdit;
	INT32			m_lEventGetDefenseGuidInfoEdit;

	INT32			m_lEventGetDateCombo;
	INT32			m_lEventGetHourCombo;

	INT32			m_lEventRequireCastleMasterMessage;
	INT32			m_lEventRequireGuildMasterMessage;
	INT32			m_lEventRequireGuildConditionMessage;
	INT32			m_lEventAlreadyApplGuildMessage;
	INT32			m_lEventCannotSetSiegeWarTimeMessage;
	INT32			m_lEventCannotApplSiegeWarMessage;
	INT32			m_lEventDefenseApplSuccessMessage;
	INT32			m_lEventAttackApplSuccessMessage;
	INT32			m_lEventSetDefenseGuildSuccessMessage;
	INT32			m_lEventSetNextSiegeWarTimeSuccessMessage;
	INT32			m_lEventNotEnoughMoneyMessage;
	INT32			m_lEventNoMoreRepairMessage;
	INT32			m_lEventConfirmApplGuildMessage;

	INT32			m_lEventSelectAttackPageSlot[9];
	INT32			m_lEventSelectDefensePageSlot[9];

	INT32			m_lEventGetCatapultEdit;
	INT32			m_lEventGetAtkResTowerEdit;

	INT32			m_lEventSelectedDefenseGuild;
	INT32			m_lEventDisableDefenseGuild;
	INT32			m_lEventNormalDefenseGuild;

	INT32			m_lEventOpenCarveMessageUI;
	INT32			m_lEventCloseCarveMessageUI;

	INT32			m_lEventOpenSiegeWarInfoUI;
	INT32			m_lEventCloseSiegeWarInfoUI;

	INT32			m_lEventGetSiegeWarInfoEditControl;
	INT32			m_lEventGetRemainTimeEditControl;

	INT32			m_lEventUseCharNameMessage;
	INT32			m_lEventRepairCharNameMessage;

	INT32			m_lEventOpenRepairBarUI;
	INT32			m_lEventCloseRepairBarUI;

	AcUIEdit		*m_pcsMainUIDescEdit;
	AcUIEdit		*m_pcsMainUISiegeWarTimeEdit;

	AcUIEdit		*m_pcsAttackHelpEdit;
	AcUIEdit		*m_pcsAttackSiegeWarTimeEdit;

	AcUIEdit		*m_pcsDefenseHelpEdit;
	AcUIEdit		*m_pcsDefenseGuildInfoEdit;

	AcUIEdit		*m_pcsCatapultDescEdit;
	AcUIEdit		*m_pcsAtkResTowerDescEdit;

	AcUIEdit		*m_pcsSiegeWarInfoEdit;
	AcUIEdit		*m_pcsRemainTimeEdit;

	AcUICombo		*m_pcsDateCombo;
	AcUICombo		*m_pcsHourCombo;

	ApdEvent		*m_pcsEvent;
	AgpdSiegeWar	*m_pcsSiegeWar;

	CHAR			m_szDateCombo[AGCMUISIEGEWAR_MAX_SELECT_DATE][AGCMUISIEGEWAR_MAX_COMBO_STRING];
	CHAR			m_szHourCombo[AGCMUISIEGEWAR_MAX_SELECT_HOUR][AGCMUISIEGEWAR_MAX_COMBO_STRING];

	CHAR			*m_aszDateComboPointer[AGCMUISIEGEWAR_MAX_SELECT_DATE];
	CHAR			*m_aszHourComboPointer[AGCMUISIEGEWAR_MAX_SELECT_HOUR];

	AtttackGuildList	m_astAttackApplGuildList[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];
	AtttackGuildList	m_astDefenseApplGuildList[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];

	AtttackGuildList	*m_apstAttackApplGuildListPointer[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];
	AtttackGuildList	*m_apstDefenseApplGuildListPointer[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];

	AgpdGrid		*m_pcsCatapultGrid;
	AgpdGrid		*m_pcsAtkResTowerGrid;

	BOOL			m_bIsActiveCatapultRepairButton;
	BOOL			m_bIsActiveAtkResTowerRepairButton;
	BOOL			m_bIsActiveCatapultActiveButton;
	BOOL			m_bIsActiveAtkResTowerActiveButton;

	BOOL			m_bIsActiveAttackApplButton;

	AgpdCharacter	*m_pcsTarget;

	INT32			m_lCooldownIndex;

	INT32			m_lSiegeWarInfoScroll;
	INT32			m_lUseCharName;
	INT32			m_lRepairBar;

	CHAR			m_szUseCharName[AGPDCHARACTER_MAX_ID_LENGTH + 1];

	BOOL			m_bOpenUI;
	AuPOS			m_stOpenUIPos;

	BOOL			m_bInfoOpenUI;

public:		 // 2006.06.20. steeple
	AtttackGuildList	m_astAttackGuildList[AGPMSIEGEWAR_TOTAL_ATTACK_GUILD];
	AtttackGuildList	m_astDefenseGuildList[AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD];

private:

	INT32			m_lAttackStartPage;
	INT32			m_lAttackCurrentPage;
	INT32			m_lAttackMaxPage;

	INT32			m_lDefenseStartPage;
	INT32			m_lDefenseCurrentPage;
	INT32			m_lDefenseMaxPage;

	INT32			m_lSelectedDefenseGuildList;

	INT32			m_lRepairStartTimeMSec;
	INT32			m_lRepairDurationMSec;

	BOOL			m_bIsOpenedRepairBarUI;

private:
	BOOL			AddEvent();
	BOOL			AddFunction();
	BOOL			AddUserData();
	BOOL			AddDisplay();
	BOOL			AddBoolean();

	BOOL			SetSiegeWarTimeEdit(AcUIEdit *pcsTimeEdit);

	BOOL			SetMainUIEdit();
	BOOL			SetSiegeWarTimeUIEdit();

	BOOL			SetComboBoxData();
	BOOL			GetComboTime(UINT64 *pullTime);

	BOOL			SetSiegeWarDefenseHelpEdit();
	BOOL			SetSiegeWarDefenseGuildInfoEdit();

	BOOL			SetSiegeWarInfoEdit();
	BOOL			SetRemainTimeEdit(UINT32 ulClockCount);

	BOOL			SetAttackGuildPage(INT32 lStartPage, INT32 lSelectPage);
	BOOL			SetAttackSelectPageSlot(INT32 lSelectPage);

	BOOL			SetDefenseGuildPage(INT32 lStartPage, INT32 lSelectPage);
	BOOL			SetDefenseSelectPageSlot(INT32 lSelectPage);

	BOOL			AddChatMessage(CHAR *pszMessage);

	BOOL			SetUseCharName(CHAR *pszCharName, BOOL bUse);

public:
	inline BOOL		IsInAttackGuild(const CHAR* szGuildID);
	inline BOOL		IsInDefenseGuild(const CHAR* szGuildID);

	BOOL			SetCallbackReceiveAttackGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackReceiveEnemyGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//SiegeWarInfo창을 이용할 함수
	BOOL			IsOpenSiegeWarInfo()		{	return m_bInfoOpenUI;	}
	BOOL			OpenSiegeWarInfo();
	void			CloseSiegeWarInfo();

	void			SetRemainTime( UINT32 ulStart, UINT32 ulClockCount );
	void			SetInfoEdit( char* szText, UINT32 ulColor );

public:
	static BOOL	CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUpdateCastleInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBReceiveResult(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBReceiveAttackApplGuildList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBReceiveDefenseApplGuildList(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBReceiveAttackGuildList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBReceiveDefenseGuildList(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBIsFriendGuild(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBIsEnemyGuild(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBResponseUseAttackObject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAddItemResult(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBStatusInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSyncMessage(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBRemoveSiegeWarGrid(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBOpenCheckResult(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBInputSiegeWarMessage(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBChangeCharacterBindingRegion(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBEndCooldown(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBSetTargetCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBResetTargetCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	// display
public:
	static BOOL	CBDisplayNPCName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL	CBDisplayAttackGuildName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayAttackGuildMasterName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayAttackGuildPoint(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayAttackPageNumber(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static BOOL	CBDisplayDefenseGuildName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayDefenseGuildMasterName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayDefenseGuildPoint(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayDefensePageNumber(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static BOOL	CBDisplayComboDate(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayComboHour(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static BOOL	CBDisplayUseCharacterName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static BOOL	CBDisplayRepairBarMax(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayRepairBarCurrent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	// function
public:
	static BOOL	CBGetMainUIDescEdit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBGetMainUISiegeWarTimeEdit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBGetAttackHelpEdit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBGetAttackSiegeWarTimeEdit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBGetDefenseHelpEdit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBGetDefenseGuildInfoEdit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBGetDateCombo(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBGetHourCombo(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBOpenSiegeWarTimeSetUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBOpenAttackApplicationUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBOpenDefenseApplicationUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBConfirmSiegeWarTime(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBSelectDefenseGuild(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBApplicationDefense(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBCancelDefense(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBApplicationAttack(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBAttackPageLeft(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBAttackPageRight(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBAttackGoPage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBRegisterDefenseGuild(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBDefensePageLeft(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDefensePageRight(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDefenseGoPage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBDefenseGuildListSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBGetCatapultDescEdit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBGetAtkResTowerDescEdit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBRepairCatapult(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBRepairAtkResTower(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBActiveCatapult(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBActiveAtkResTower(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBCatapultDragDrop(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBAtkResTowerDragDrop(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBGetSiegeWarInfoEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGetRemainTimeEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBSiegeWarInfoScrollUp(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSiegeWarInfoScrollDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// boolean callback functions
public:
	static BOOL CBIsActiveCatapultRepairButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsActiveCatapultActiveButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsActiveAtkResTowerRepairButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsActiveAtkResTowerActiveButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	static BOOL CBIsActiveAttackApplButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

private:
	BOOL	OpenCatapultUI(AgpdCharacter *pcsCharacter);
	BOOL	OpenAtkResTowerUI(AgpdCharacter *pcsCharacter);

	BOOL	SetTextEffect(CHAR *pszString);

	BOOL	SetSelectedDefenseGuild();

	BOOL	IsUsableItemForCatapult(AgpdItem *pcsItem);
	BOOL	IsUsableItemForAtkResTower(AgpdItem *pcsItem);

	BOOL	UpdateSiegeWarInfoUI( AgpdSiegeWar* pSiegeWarInfo = NULL, BOOL bBattleInfo = FALSE );

private :
	// 화면 좌상단에 남은시간 등을 표시하는 메세지창을 위한 것들..
	CHAR			m_strMessage[ 1024 ];		// 메세지문자열	
	INT32			m_nTextColor;				// 문자열 색상

	BOOL	_SetMessageText( CHAR* pText, UINT32 nColor );
	BOOL	_SetMsgTextToEditBox( INT32 nScrollValue );

};

#endif//_AGCMUISIEGEWAR_H_
