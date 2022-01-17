#ifndef __AGCM_UI_CHARACTER_H__
#define __AGCM_UI_CHARACTER_H__

#include "ApModule.h"
#include "AgpmFactors.h"
#include "AgpmItem.h"
#include "AgpmCombat.h"
#include "AgpmProduct.h"
#include "AgcmCharacter.h"
#include "AgcmUIManager2.h"
#include "AgcmChatting2.h"
#include "AgcmSkill.h"
#include "AgcmMinimap.h"
#include "ApmObject.h"
#include "AgpmBillInfo.h"
#include "AgpmPvP.h"
#include "AgpmConfig.h"
#include "AgpmGuild.h"
#include "AgpmSiegeWar.h"
#include "AgcmBattleGround.h"

#include "AgcdUICharacter.h"

#include "AgcmUIControl.h"
#include "AgcmUIMain.h"

#include "AgpmRide.h"
#include "CUiCharacterProperty.h"

class AgpmArchlord;

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUICharacterD" )
#else
#pragma comment ( lib , "AgcmUICharacter" )
#endif
#endif

#define	AGCM_UI_CHARACTER_OPEN_CHARACTER_MENU_RANGE		400		// 5미터

#define	AGCM_UI_CHARACTER_TRANSFORM_NOTICE_INTERVAL		60000	// MSec
#define AGCM_UI_CHARACTER_CRIMINAL_NOTICE_INTERVAL		60000	// MSec

#define	UI_MESSAGE_ID_TRANSFORM_REMAIN_NOTICE		"Transform_Remain_Notice"
#define	UI_MESSAGE_ID_TRANSFORM_RESTORE_NOTICE		"Transform_Restore_Notice"

#define	UI_MESSAGE_ID_CHARACTER_ATTR				"CharacterAttr"
#define	UI_MESSAGE_ID_CHARACTER_ATTR_MAGIC			"CharacterAttrMagic"
#define	UI_MESSAGE_ID_CHARACTER_ATTR_WATER			"CharacterAttrWater"
#define	UI_MESSAGE_ID_CHARACTER_ATTR_FIRE			"CharacterAttrFire"
#define	UI_MESSAGE_ID_CHARACTER_ATTR_EARTH			"CharacterAttrEarth"
#define	UI_MESSAGE_ID_CHARACTER_ATTR_AIR			"CharacterAttrAir"
#define	UI_MESSAGE_ID_CHARACTER_ATTR_POISON			"CharacterAttrPoison"
#define	UI_MESSAGE_ID_CHARACTER_ATTR_ICE			"CharacterAttrIce"
#define	UI_MESSAGE_ID_CHARACTER_ATTR_LIGHTENING		"CharacterAttrLightening"

#define	UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE			"CharacterAttrDamage"
#define	UI_MESSAGE_ID_CHARACTER_ATTR_RESIST			"CharacterAttrResist"

#define UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE_RATE	"CharacterAttrDamageRate"
#define UI_MESSAGE_ID_CHARACTER_ATTR_HP_RATE		"CharacterAttrHPRate"

#define UI_MESSAGE_ID_SUMMONS_PROPENSITY_ATTACK		"SummonsPropensityAttack"
#define UI_MESSAGE_ID_SUMMONS_PROPENSITY_DEFENSE	"SummonsPropensityDefense"
#define UI_MESSAGE_ID_SUMMONS_PROPENSITY_SHADOW		"SummonsPropensityShadow"

#define UI_MESSAGE_ID_BECOME_MURDERER_LV1			"BecomeMurdererLv1"
#define UI_MESSAGE_ID_BECOME_MURDERER_LV2			"BecomeMurdererLv2"
#define UI_MESSAGE_ID_BECOME_MURDERER_LV3			"BecomeMurdererLv3"
#define UI_MESSAGE_ID_MURDERER_GET_POINT			"MurdererGetPoint"
#define UI_MESSAGE_ID_FREE_FIRST_ATTACKER			"FreeFirstAttacker"
#define UI_MESSAGE_ID_BECOME_FIRST_ATTACKER			"BecomeFirstaAttacker"
#define UI_MESSAGE_ID_BLOCK_BY_PENALTY_BASE			"CharacterPenalty"


typedef enum AgcmUICharacterCB {
	AGCMUICHAR_CB_UPDATE_RESURRECT_STATUS	= 0,
	AGCMUICHAR_CB_SET_TARGET_CHARACTER,
	AGCMUICHAR_CB_RESET_TARGET_CHARACTER,
} AgcmUICharacterCB;

typedef struct _AgcmUICharacterMessageData {
	INT32				lLevelUp;
	INT32				lBonusExp;
	INT32				lBonusPCBangExp;
} AgcmUICharacterMessageData;

class AgcmUICharacter : public ApModule, public CUiCharacterProperty
{
protected:
	ApmMap				*m_pcsApmMap;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmGrid			*m_pcsAgpmGrid;
	AgpmItem			*m_pcsAgpmItem;
	AgpmSkill			*m_pcsAgpmSkill;
	AgpmCombat			*m_pcsAgpmCombat;
	AgpmProduct			*m_pcsAgpmProduct;
	AgpmSummons			*m_pcsAgpmSummons;
	AgcmCharacter		*m_pcsAgcmCharacter;
	AgcmSkill			*m_pcsAgcmSkill;
	AgcmUIManager2		*m_pcsAgcmUIManager2;
	AgcmChatting2		*m_pcsAgcmChatting2;
	AgcmMinimap			*m_pcsAgcmMinimap;
	ApmObject			*m_pcsApmObject;
	AgpmBillInfo		*m_pcsAgpmBillInfo;
	AgpmRide			*m_pcsAgpmRide;
	AgpmPvP				*m_pcsAgpmPvP;
	AgcmUIControl		*m_pcsAgcmUIControl;
	AgcmUIMain			*m_pcsAgcmUIMain;
	AgpmConfig			*m_pcsAgpmConfig;
	AgpmGuild			*m_pcsAgpmGuild;
	AgpmSiegeWar		*m_pcsAgpmSiegeWar;
	AgpmArchlord		*m_pcsAgpmArchlord;
	AgcmGuild			*m_pcsAgcmGuild;
	AgcmBattleGround	*m_pcsAgcmBattleGround;

	INT32				m_lADIndexObject;
	INT32				m_lADIndexCharacter;
	INT32				m_lADIndexCharacterTemplate;

	AgpdCharacter		*m_pcsTargetCharacter;

	AgcdUIUserData		*m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_NUM];
	
	AgcdUIUserData		*m_pstUserDataTargetCharacter;

	AgcdUIUserData		*m_pstUserDataMessageData;

	AgcdUIUserData		*m_pstUserDataTargetBuffGrid;

	AgcdUIUserData		*m_pstUserDataSelfCharacterDead;

	AgcdUIUserData		*m_pstUserDataSummonsPropensity;		// 2005.09.25. steeple
	INT32				m_lSummonsPropensityUDIndex;			// 2005.09.25. steeple 그냥 임시 변수

	AgcdUIUserData		*m_pstUserDataExpPenalty;

	AgcdUIUserData		*m_pstUserDataOtherName;				// 2007.08.29.steeple

	AgcmUICharacterMessageData		m_stMessageData;

	AgcdUICharacterLevelupMessage	m_astCommonLevelupMessage[AGPMCHAR_MAX_LEVEL];

	CHAR				m_szOtherName[AGPDCHARACTER_MAX_ID_LENGTH + 1];

	// Event
	INT32				m_lEventLevelUp;
	INT32				m_lEventBonusExp;
	INT32				m_lEventBonusPCBangExp;

	INT32				m_lEventAttackResultNotEnoughArrow;
	INT32				m_lEventAttackResultNotEnoughBolt;
	INT32				m_lEventAttackResultNotEnoughMP;

	INT32				m_lEventOpenCharacterMenu;
	INT32				m_lEventCloseCharacterMenu;

	// target information ui event
	INT32				m_lEventOpenTargetInfo;
	INT32				m_lEventCloseTargetInfo;

	INT32				m_lEventTargetAttrNone;
	INT32				m_lEventTargetAttrAir;
	INT32				m_lEventTargetAttrFire;
	INT32				m_lEventTargetAttrWater;
	INT32				m_lEventTargetAttrMagic;
	INT32				m_lEventTargetAttrEarth;
	INT32				m_lEventTargetAttrPoison;
	INT32				m_lEventTargetAttrIce;
	INT32				m_lEventTargetAttrLightening;
	INT32				m_lEventTargetPKCriminal;
	INT32				m_lEventTargetPKMurdererLevel1;
	INT32				m_lEventTargetPKMurdererLevel2;
	INT32				m_lEventTargetPKMurdererLevel3;

	INT32				m_lEventTargetableOn;
	INT32				m_lEventTargetableOff;

	INT32				m_lEventRefreshTargetName;
	
	INT32				m_lEventOpenTargetAttrInfo;
	INT32				m_lEventCloseTargetAttrInfo;

	INT32				m_lEventOpenTargetUserMenu;
	INT32				m_lEventCloseTargetUserMenu;

	INT32				m_lEventOpenTargetSummonsMenu;			// 2005.09.25. steeple
	INT32				m_lEventCloseTargetSummonsMenu;			// 2005.09.25. steeple
	INT32				m_lEventOpenTargetSummonsMenu2;			// 2005.09.25. steeple
	INT32				m_lEventCloseTargetSummonsMenu2;		// 2005.09.25. steeple
	INT32				m_lEventDisplaySummonsPropensity;		// 2005.09.25. steeple

	INT32				m_lEventCloseTargetWindow;

	INT32				m_lEventOpenResurrectionUI;
	INT32				m_lEventCloseResurrectionUI;

	INT32				m_lEventOpenConfirmResurrectTownUI;
	INT32				m_lEventOpenConfirmResurrectUI;

	INT32				m_lEventMurdererPointUp;

	INT32				m_lEventOpenSelectSiegeResurrect;

	INT32				m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_MAX];
	AgcdUIControl*		m_apcsEditControl[AGCD_UI_CHAR_EDIT_MAX];

	BOOL				m_bIsOpenedCharacterMenu;
	BOOL				m_bIsExpTypePercent;

	INT32				m_lEventResurrectionByOther;

	UINT32				m_ulTransformEndTimeMSec;
	UINT32				m_ulTransformDurationMSec;
	UINT32				m_ulTransformNextNoticeMSec;

	UINT32				m_ulCriminalNotifyClock;

	INT32				m_lTargetAttributeType;
	
	AgpdGrid			m_csTargetBuffGrid;

	BOOL				m_bIsOpenedTargetUserMenu;
	BOOL				m_bIsOpenedTargetAttrInfo;
	BOOL				m_bIsOpenedTargetSummonsMenu;
	
	AcUIToolTip			m_csTargetAttrTooltip;
	AcUIToolTip			m_csTargetNameToolTip;

	BOOL				m_bSelfCharacterDead;
	BOOL				m_bExpPenalty;

	BOOL				SendRequestResurrectionToTown();
	BOOL				SendRequestResurrectionNow();
	BOOL				SendRequestResurrectionSiege(BOOL bIsInner);
	BOOL				SendRequestResurrectionByOther(CHAR* szName);

	BOOL				SetEditTextType1(AcUIEdit *pcsUIEdit, INT32 lOriginalValue, INT32 lCurrentValue);
	BOOL				SetEditTextType4(AcUIEdit *pcsUIEdit, INT32 lCurrentValue, CHAR *pszColor = NULL);

	AgpdGridItem		*m_pcsTransformCancelGridItem;
	AgpdGridItem		*m_pcsEvolutionCancelGridItem;

public:
			AgcmUICharacter();
	virtual	~AgcmUICharacter();

	BOOL	OnAddModule();
	BOOL	OnDestroy();
	BOOL	OnInit();
	BOOL	OnIdle(UINT32 ulClockCount);

	AgpmFactors			*GetAgpmFactorsModule()		{return m_pcsAgpmFactors;}
	AgcmCharacter		*GetAgcmCharacterModule()	{return m_pcsAgcmCharacter;}

	// refresh user data functions
	///////////////////////////////////////////////////////////////////////
	BOOL				RefreshUserDataTargetCharacter();
	
	BOOL				SetTargetCharacter(AgpdCharacter *pcsTargetCharacter);
	BOOL				ResetTargetCharacter();

	BOOL				OpenCharacterMenu(ApBase *pcsBase);
	BOOL				CloseCharacterMenu();

	BOOL				ResetDisplayCharacterStatus(CHAR *szDisplay, INT32 *plValue);

	BOOL				SetCharacter(AgpdCharacter *pcsCharacter);
	BOOL				ReleaseCharacter();

	BOOL				SetCallbackUpdateResurrectStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackSetTargetCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackResetTargetCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetTransformDurationMSec(UINT32 ulTransformDurationMSec);

	BOOL				SetTargetAttrToolTipInfo();
	BOOL				SetTargetSummonsToolTipInfo();

	BOOL				StreamReadLevelupMessages(CHAR *szFile, BOOL bEncryption);

	AgcdUICharacterADBase *	GetBaseData(ApBase *pcsBase, AuPOS *pstPos = NULL)
	{
		switch (pcsBase->m_eType)
		{
		case APBASE_TYPE_CHARACTER:
			{
				if (pstPos)
					*pstPos = ((AgpdCharacter *) pcsBase)->m_stPos;

				return (AgcdUICharacterADBase *) m_pcsAgpmCharacter->GetAttachedModuleData(m_lADIndexCharacter, pcsBase);
			}

		case APBASE_TYPE_OBJECT:
			{
				if (pstPos)
					*pstPos = ((ApdObject *) pcsBase)->m_stPosition;

				return (AgcdUICharacterADBase *) m_pcsApmObject->GetAttachedModuleData(m_lADIndexObject, pcsBase);
			}
		}

		return NULL;
	}

	AgcdUICharacterADCharacterTemplate *	GetCharacterTemplateData(AgpdCharacterTemplate *pcsCharacterTemplate)
	{
		return (AgcdUICharacterADCharacterTemplate *) m_pcsAgpmCharacter->GetAttachedModuleData(m_lADIndexCharacterTemplate, pcsCharacterTemplate);
	}

	BOOL				AddBaseMinimapType(ApBase *pcsBase, AgcmMinimap::MPInfo::TYPE eType, CHAR *szDisplay = NULL, INT32 lQuestTID = -1);
	BOOL				RemoveBaseMinimapType(ApBase *pcsBase, INT32 lQuestTID = -1);
	BOOL				ChangeBaseMinimapType(ApBase *pcsBase, AgcmMinimap::MPInfo::TYPE eType, CHAR *szDisplay = NULL);
	AgcmMinimap::MPInfo *	GetBaseMinimapInfo(ApBase *pcsBase);

protected:
	BOOL	AddEvent();
	BOOL	AddFunction();
	BOOL	AddDisplay();
	BOOL	AddUserData();
	BOOL	AddBoolean();

	BOOL	AddEventStatus();

	BOOL	AddDisplay(CHAR *szName, INT32 lID, AgcUIDisplayOldCB fCallBack);
	BOOL	AddDisplay(CHAR *szName, INT32 lID, AgcUIDisplayCB fCallBack);

	BOOL	AddUserData(CHAR *szName, PVOID pvData, INT32 lDataSize, INT32 lDataCount, INT32 lUserDataIndex);

	// Callback
	static BOOL CharacterStatusCB(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue , AgcdUIControl *pcsSourceControl);

	static BOOL CBTargetCharName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static BOOL CBDisplayMessageData(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL CBTargetAttributeType(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL CBRefreshTargetName(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBToggleTargetUserMenu(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBToggleTargetAttrTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBCloseTargetAttrTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	static BOOL CBDisplaySiegeResurrection(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);

	// 2005.09.26. steeple. 소환수 성향 관련
	static BOOL CBToggleTargetSummonsMenu(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplaySummonsPropensity(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);
	static BOOL CBSummonsPropensityAttackClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSummonsPropensityDefenseClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSummonsPropensityShadowClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	BOOL SendSummonsSetPropenstiy(INT32 lSummonsCID, EnumAgpdSummonsPropensity eSummonsPropensity);
    static BOOL CBReceiveSummonsSetPropensity(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBToggleExpType(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBResurrectionToTown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBResurrectionNow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBResurrectionSiege(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBCheckResurrectionToTown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBCheckResurrectionNow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBResurrectionSiegeInner(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBResurrectionSiegeOuter(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// 2007.08.27. steeple
	static BOOL CBResurrectionByOther(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBResurrectionByOtherConfirm(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL CBDisplayOtherName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl);

	static BOOL CBOpenTargetNameTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBCloseTargetNameTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	static BOOL	CharacterUpdateFactorCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CharacterLevelUp(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CharacterUpdateFactorBonusExpCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CharacterUpdateFactorMurdererPointCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CharacterUpdateFactorCharismaPointCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CharacterUpdateCriminalStatusCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL SetSelfCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL ReleaseSelfCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	UpdateEquipItemCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CharacterBlockByPenalty(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAttackResultNotEnoughArrow(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAttackResultNotEnoughBolt(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAttackResultNotEnoughMP(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBLockTarget(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUnlockTarget(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSelectTarget(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUnSelectTarget(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBCancelTransform(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCancelEvolution(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAddBuffedSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveBuffedSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBRideAck(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL GetEditCon(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL GetEditWis(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL GetEditDex(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL GetEditStr(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL GetEditInt(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL GetEditChar(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBDesCharTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBConstBase(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBDestBase(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	RefreshCharacterStatus();

	BOOL	AddTargetBuffedSkill(AgpdGridItem *pcsGridItem);
	BOOL	RemoveTargetBuffedSkill(AgpdGridItem *pcsGridItem);
	BOOL	ResetTargetBuffedSkill();

	// boolean callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL CBIsActiveFollowMenu(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsExpPenalty(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsActiveSiegeResurrection(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	static BOOL CBSetFollowTarget(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

public:
	AgpdCharacter					*GetTargetCharacter() { return m_pcsTargetCharacter; };

	BOOL							OnToggleBtnRestoreTransform( BOOL bIsShow );
	BOOL							OnShowBtnRestoreTransform( void );
	BOOL							OnHideBtnRestoreTransform( void );

	BOOL							OnToggleBtnRestoreEvolution( BOOL bIsShow );
	BOOL							OnShowBtnRestoreEvolution( void );
	BOOL							OnHideBtnRestoreEvolution( void );

	BOOL							OnShowBlindEquipSlot( void* pCharacter, INT nPartID );
	BOOL							OnHideBlindEquipSlot( void* pCharacter, INT nPartID );

	BOOL							OnShowUIResurrection( void );

private :
	INT32							m_nEventUpdateTargetRaceHuman;
	INT32							m_nEventUpdateTargetRaceOrc;
	INT32							m_nEventUpdateTargetRaceMoonElf;
	INT32							m_nEventUpdateTargetRaceDragonScion;

	INT32							m_nEventUpdateTargetClassKnight;
	INT32							m_nEventUpdateTargetClassArcher;
	INT32							m_nEventUpdateTargetClassMage;
	INT32							m_nEventUpdateTargetClassBerserker;
	INT32							m_nEventUpdateTargetClassHunter;
	INT32							m_nEventUpdateTargetClassSocerer;
	INT32							m_nEventUpdateTargetClassSwashBuckler;
	INT32							m_nEventUpdateTargetClassRanger;
	INT32							m_nEventUpdateTargetClassElementalist;
	INT32							m_nEventUpdateTargetClassScion;
	INT32							m_nEventUpdateTargetClassSlayer;
	INT32							m_nEventUpdateTargetClassObiter;
	INT32							m_nEventUpdateTargetClassSummerner;

private :
	BOOL							_UpdateTargetPlayerRaceAndClass( AgpdCharacter* ppdCharacter );

public :
	static BOOL						CallBack_OnSelectPropertyAttack		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pBase, AgcdUIControl* pControl );
	static BOOL						CallBack_OnSelectPropertyTab		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pBase, AgcdUIControl* pControl );
};

#endif // __AGCM_UI_CHARACTER_H__