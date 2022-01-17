#include "AgcmUICharacter.h"
#include "AgpmArchlord.h"
#include <float.h>
#include "ApMemoryTracker.h"

#include "AuStrTable.h"
#include "AgcmTextBoardMng.h"
#include "AgpmEventSkillMaster.h"
#include "AgcmUIItem.h"

#define AGCMUICHARACTER_NUM_TARGET_BUFFED_GRID	5

const int	AGCMUICHARACTER_DEFAULT_FOLLOW_DISTANCE		= 500;

#define	AGCMUICHARACTER_CHARSTATUS_NORMAL_TEXT_COLOR	"16777215"
#define	AGCMUICHARACTER_CHARSTATUS_ADD_TEXT_COLOR		"65280"
#define	AGCMUICHARACTER_CHARSTATUS_SUB_TEXT_COLOR		"16711680"
#define AGCMUICHARACTER_CHARSTATUS_TEXT_COLOR_YELLOW	"16776960"
#define AGCMUICHARACTER_CHARSTATUS_TEXT_COLOR_VIOLET	"61695"
#define AGCMUICHARACTER_CHARSTATUS_TEXT_COLOR_RED		"16711680"

static const CHAR g_szPenalty[AGPMCHAR_PENALTY_MAX][20] = 
	{
	"_CantPT",
	"_CantNPC",
	"_CantConvert",
	"_CantAuction",
	"_CantCharCust",
	"_ExpLose",
	"_CantAttack"
	};

// -=========================================================================
// Macro

// ex) 불속성 공격: 10
#define MAKE_ATTR_ATTACK_STR(buffer, attr)												\
		if (AP_SERVICE_AREA_WESTERN == g_eServiceArea)									\
		{																				\
			sprintf(buffer,																\
			"%s %s: %d",																\
			m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_##attr),		\
			m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE),		\
			lAttrValue);																\
		}																				\
		else																			\
		{																				\
			sprintf(buffer,																\
			"%s%s %s: %d",																\
			m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_##attr),		\
			m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR),			\
			m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE),		\
			lAttrValue);																\
		}

// ex) 불속성 저항: 10
#define MAKE_ATTR_RESI_STR(buffer, attr)													\
		if (AP_SERVICE_AREA_WESTERN == g_eServiceArea)										\
		{																					\
		sprintf(szBuffer,																	\
				"%s %s: %d%%",																\
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_##attr),		\
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_RESIST),		\
				lAttrValue);																\
		}																					\
		else																				\
		{																					\
		sprintf(szBuffer,																	\
				"%s%s %s: %d%%",																\
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_##attr),		\
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR),			\
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_RESIST),		\
				lAttrValue);																\
		}

// implementation of AgcmUICharacter
AgcmUICharacter::AgcmUICharacter()
{
	SetModuleName("AgcmUICharacter");
	EnableIdle(TRUE);

	m_pcsApmMap	= NULL;
	m_pcsAgpmFactors = NULL;
	m_pcsAgpmCharacter = NULL;
	m_pcsAgpmGrid = NULL;
	m_pcsAgpmItem = NULL;
	m_pcsAgpmSkill = NULL;
	m_pcsAgpmCombat = NULL;
	m_pcsAgpmProduct = NULL;
	m_pcsAgpmSummons = NULL;
	m_pcsAgcmCharacter = NULL;
	m_pcsAgcmSkill = NULL;
	m_pcsAgcmUIManager2 = NULL;
	m_pcsAgcmChatting2 = NULL;
	m_pcsAgcmMinimap = NULL;
	m_pcsApmObject = NULL;
	m_pcsAgpmRide = NULL;
	m_pcsAgcmUIControl = NULL;
	m_pcsAgcmUIMain = NULL;
	m_pcsAgpmConfig = NULL;
	m_pcsAgpmGuild = NULL;
	m_pcsAgpmSiegeWar = NULL;
	m_pcsAgpmArchlord = NULL;
	m_pcsAgcmBattleGround	= NULL;

	m_lADIndexObject = -1;
	m_lADIndexCharacter = 0;
	m_lADIndexCharacterTemplate = 0;

	m_pcsTargetCharacter = NULL;

	for(int i = 0;i<E_AGCD_UI_CHAR_USRDAT_NUM;++i)
	{
		m_pastAgcdUIUserData[i] = NULL;
	}

	
	m_pstUserDataTargetCharacter = NULL;
	m_pstUserDataMessageData = NULL;
	m_pstUserDataTargetBuffGrid = NULL;
	m_pstUserDataSelfCharacterDead = NULL;
	m_pstUserDataSummonsPropensity = NULL;
	m_lSummonsPropensityUDIndex = 0;

	ZeroMemory(&m_stMessageData,sizeof(m_stMessageData));
	ZeroMemory(m_astCommonLevelupMessage,sizeof(m_astCommonLevelupMessage));

	ZeroMemory(m_szOtherName, sizeof(m_szOtherName));

	m_lEventLevelUp = 0;
	m_lEventBonusExp = 0;
	m_lEventBonusPCBangExp = 0;

	m_lEventAttackResultNotEnoughArrow = 0;
	m_lEventAttackResultNotEnoughBolt = 0;
	m_lEventAttackResultNotEnoughMP = 0;

	m_lEventOpenCharacterMenu = 0;
	m_lEventCloseCharacterMenu = 0;
	
	m_lEventOpenTargetInfo = 0;
	m_lEventCloseTargetInfo = 0;

	m_lEventTargetAttrNone = 0;
	m_lEventTargetAttrAir = 0;
	m_lEventTargetAttrFire = 0;
	m_lEventTargetAttrWater = 0;
	m_lEventTargetAttrMagic = 0;
	m_lEventTargetAttrEarth = 0;
	m_lEventTargetAttrPoison = 0;
	m_lEventTargetAttrIce = 0;
	m_lEventTargetAttrLightening = 0;
	m_lEventTargetPKCriminal = 0;
	m_lEventTargetPKMurdererLevel1 = 0;
	m_lEventTargetPKMurdererLevel2 = 0;
	m_lEventTargetPKMurdererLevel3 = 0;

	m_lEventTargetableOn = 0;
	m_lEventTargetableOff = 0;
	m_lEventRefreshTargetName = 0;
	m_lEventOpenTargetAttrInfo = 0;
	m_lEventCloseTargetAttrInfo = 0;

	m_lEventOpenTargetUserMenu = 0;
	m_lEventCloseTargetUserMenu = 0;

	m_lEventOpenTargetSummonsMenu = 0;
	m_lEventCloseTargetSummonsMenu = 0;
	m_lEventOpenTargetSummonsMenu2 = 0;
	m_lEventCloseTargetSummonsMenu2 = 0;
	m_lEventDisplaySummonsPropensity = 0;

	m_lEventCloseTargetWindow = 0;

	m_lEventOpenResurrectionUI = 0;
	m_lEventCloseResurrectionUI = 0;

	m_lEventOpenConfirmResurrectTownUI = 0;
	m_lEventOpenConfirmResurrectUI = 0;
	
	m_lEventMurdererPointUp = 0;

	ZeroMemory(m_alEventGetEditControl,sizeof(m_alEventGetEditControl));
	ZeroMemory(m_apcsEditControl,sizeof(m_apcsEditControl));

	m_pstUserDataOtherName = NULL;

	m_lEventResurrectionByOther = 0;

	m_bIsOpenedCharacterMenu = FALSE;
	m_bIsExpTypePercent = FALSE;

	m_ulTransformEndTimeMSec = 0;
	m_ulTransformDurationMSec = 0;
	m_ulTransformNextNoticeMSec = 0;

	m_ulCriminalNotifyClock = 0;

	m_lTargetAttributeType = 0;
	
	m_bIsOpenedTargetUserMenu = FALSE;
	m_bIsOpenedTargetAttrInfo = FALSE;
	m_bIsOpenedTargetSummonsMenu = FALSE;
	m_bSelfCharacterDead = FALSE;

	m_pcsTransformCancelGridItem	= NULL;
	m_pcsEvolutionCancelGridItem	= NULL;
}

AgcmUICharacter::~AgcmUICharacter()
{
}

BOOL AgcmUICharacter::OnAddModule()
{
	m_pcsApmMap					= (ApmMap *) GetModule("ApmMap");
	m_pcsAgpmFactors			= (AgpmFactors *)(GetModule("AgpmFactors"));
	m_pcsAgpmCharacter			= (AgpmCharacter *)(GetModule("AgpmCharacter"));
	m_pcsAgpmGrid				= (AgpmGrid *)(GetModule("AgpmGrid"));
	m_pcsAgpmItem				= (AgpmItem *)(GetModule("AgpmItem"));
	m_pcsAgpmSkill				= (AgpmSkill *) GetModule("AgpmSkill");
	m_pcsAgpmProduct			= (AgpmProduct *) GetModule("AgpmProduct");
	m_pcsAgpmSummons			= (AgpmSummons *) GetModule("AgpmSummons");
	m_pcsAgpmCombat				= (AgpmCombat *) GetModule("AgpmCombat");
	m_pcsAgcmCharacter			= (AgcmCharacter *)(GetModule("AgcmCharacter"));
	m_pcsAgcmSkill				= (AgcmSkill *) GetModule("AgcmSkill");
	m_pcsAgcmUIManager2			= (AgcmUIManager2 *)(GetModule("AgcmUIManager2"));
	m_pcsAgcmChatting2			= (AgcmChatting2 *) GetModule("AgcmChatting2");
	m_pcsAgcmMinimap			= (AgcmMinimap *) GetModule("AgcmMinimap");
	m_pcsApmObject				= (ApmObject *) GetModule("ApmObject");
	m_pcsAgpmBillInfo			= (AgpmBillInfo *) GetModule("AgpmBillInfo");
	m_pcsAgpmRide				= (AgpmRide *) GetModule("AgpmRide");
	m_pcsAgpmPvP				= (AgpmPvP *) GetModule("AgpmPvP");
	m_pcsAgcmUIControl			= (AgcmUIControl *) GetModule("AgcmUIControl");
	m_pcsAgcmUIMain				= (AgcmUIMain *) GetModule("AgcmUIMain");
	m_pcsAgpmConfig				= (AgpmConfig *) GetModule("AgpmConfig");
	m_pcsAgpmGuild				= (AgpmGuild *) GetModule("AgpmGuild");
	m_pcsAgpmSiegeWar			= (AgpmSiegeWar *) GetModule("AgpmSiegeWar");
	m_pcsAgpmArchlord			= (AgpmArchlord *) GetModule("AgpmArchlord");
	m_pcsAgcmGuild				= (AgcmGuild *) GetModule("AgcmGuild");
		
	if( !m_pcsApmMap ||
		(!m_pcsAgpmFactors) || (!m_pcsAgpmCharacter) || (!m_pcsAgcmUIManager2) || (!m_pcsAgcmCharacter) ||
		!m_pcsAgpmItem || !m_pcsAgcmChatting2 || !m_pcsAgpmGrid || !m_pcsAgpmSkill || !m_pcsAgcmSkill ||
		!m_pcsAgpmBillInfo || !m_pcsAgpmRide || !m_pcsAgpmPvP || !m_pcsAgcmUIControl || !m_pcsAgcmUIMain ||
		!m_pcsAgpmArchlord)
		return FALSE;

	if(!m_pcsAgpmCharacter->SetCallbackUpdateFactor(CharacterUpdateFactorCB, this))
		return FALSE;

	m_lADIndexCharacter = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgcdUICharacterADBase), CBConstBase, CBDestBase);
	if (m_lADIndexCharacter < 0)
		return FALSE;

	m_lADIndexCharacterTemplate = m_pcsAgpmCharacter->AttachCharacterTemplateData(this, sizeof(AgcdUICharacterADCharacterTemplate), NULL, CBDesCharTemplate);
	if (m_lADIndexCharacterTemplate < 0)
		return FALSE;

	if (m_pcsApmObject)
	{
		m_lADIndexObject = m_pcsApmObject->AttachObjectData(this, sizeof(AgcdUICharacterADBase), CBConstBase, CBDestBase);
		if (m_lADIndexObject < 0)
			return FALSE;
	}

	if(!m_pcsAgpmCharacter->SetCallbackUpdateLevel(CharacterLevelUp, this))
		return TRUE;

	if(!m_pcsAgpmFactors->SetCallbackUpdateFactorBonusExp(CharacterUpdateFactorBonusExpCB, this))
		return FALSE;

	if(!m_pcsAgpmFactors->SetCallbackUpdateFactorMurdererPoint(CharacterUpdateFactorMurdererPointCB, this))
		return FALSE;
	if(!m_pcsAgpmFactors->SetCallbackUpdateFactorCharismaPoint(CharacterUpdateFactorCharismaPointCB, this))
		return FALSE;
	if(!m_pcsAgpmCharacter->SetCallbackUpdateCriminalStatus(CharacterUpdateCriminalStatusCB, this))
		return FALSE;
	if(!m_pcsAgpmCharacter->SetCallbackBlockByPenalty(CharacterBlockByPenalty, this))
		return FALSE;

	if(!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(SetSelfCharacterCB, this))
		return FALSE;

	if(!m_pcsAgcmCharacter->SetCallbackReleaseSelfCharacter(ReleaseSelfCharacterCB, this))
		return FALSE;

	if(!m_pcsAgcmCharacter->SetCallbackActionResultNotEnoughArrow(CBAttackResultNotEnoughArrow, this))
		return FALSE;
	if(!m_pcsAgcmCharacter->SetCallbackActionResultNotEnoughBolt(CBAttackResultNotEnoughBolt, this))
		return FALSE;
	if(!m_pcsAgcmCharacter->SetCallbackActionResultNotEnoughMP(CBAttackResultNotEnoughMP, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackInitChar(CBInitCharacter, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackUpdatePosition(CBUpdatePosition, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateActionStatus(CBUpdateActionStatus, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackResurrectionByOther(CBResurrectionByOther, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackEquip(UpdateEquipItemCB, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUnEquip(UpdateEquipItemCB, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackLockTarget(CBLockTarget, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackUnlockTarget(CBUnlockTarget, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackSelectTarget(CBSelectTarget, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackUnSelectTarget(CBUnSelectTarget, this))
		return FALSE;

	if (!m_pcsAgpmSkill->SetCallbackAddBuffedList(CBAddBuffedSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackRemoveBuffedList(CBRemoveBuffedSkill, this))
		return FALSE;

	if (!m_pcsAgpmRide->SetCallbackRideAck(CBRideAck, this))
		return FALSE;

	if (!m_pcsAgpmRide->SetCallbackDismountAck(CBRideAck, this))
		return FALSE;

	if(m_pcsAgpmSummons)
	{
		if(!m_pcsAgpmSummons->SetCallbackSetPropensity(CBReceiveSummonsSetPropensity, this))
			return FALSE;
	}

	if(!AddEvent())
		return FALSE;

	if(!AddFunction())
		return FALSE;

	if(!AddDisplay())
		return FALSE;

	if(!AddUserData())
		return FALSE;

	if(!AddBoolean())
		return FALSE;

	return TRUE;
}

BOOL AgcmUICharacter::OnDestroy()
{
	INT32	lIndex;

	for (lIndex = 0; lIndex < AGPMCHAR_MAX_LEVEL; ++lIndex)
	{
		if (m_astCommonLevelupMessage[lIndex].m_szMessage1)
			delete [] m_astCommonLevelupMessage[lIndex].m_szMessage1;
		m_astCommonLevelupMessage[lIndex].m_szMessage1 = NULL;

		if (m_astCommonLevelupMessage[lIndex].m_szMessage2)
			delete [] m_astCommonLevelupMessage[lIndex].m_szMessage2;
		m_astCommonLevelupMessage[lIndex].m_szMessage2 = NULL;
	}

	return TRUE;
}

BOOL AgcmUICharacter::OnInit()
{
	m_pcsAgcmBattleGround		= (AgcmBattleGround *) GetModule("AgcmBattleGround");

	m_pcsAgpmGrid->Init(&m_csTargetBuffGrid, 1, 1, AGCMUICHARACTER_NUM_TARGET_BUFFED_GRID);

	if (m_pcsAgcmUIManager2)
	{
		m_csTargetAttrTooltip.m_Property.bTopmost = TRUE						;
		m_pcsAgcmUIManager2->AddWindow( (AgcWindow*)(&m_csTargetAttrTooltip ) )	;
		m_csTargetAttrTooltip.ShowWindow( FALSE )								;

		m_csTargetNameToolTip.m_Property.bTopmost = TRUE						;
		m_pcsAgcmUIManager2->AddWindow( (AgcWindow*)(&m_csTargetNameToolTip ) )	;
		m_csTargetNameToolTip.ShowWindow( FALSE )								;
	}

	return TRUE;
}

BOOL AgcmUICharacter::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmUICharacter::OnIdle");

	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (pcsSelfCharacter && ( pcsSelfCharacter->m_bIsTrasform || pcsSelfCharacter->m_bIsEvolution ) )
	{
		if (m_ulTransformEndTimeMSec == 0)
			return TRUE;

		if (ulClockCount >= m_ulTransformEndTimeMSec)		// 끝나기 5초정도 전에 알려준다. 끝난다고...
		{
			m_pcsAgcmChatting2->AddSystemMessage(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TRANSFORM_RESTORE_NOTICE));

			m_ulTransformEndTimeMSec	= 0;
			m_ulTransformDurationMSec	= 0;
			m_ulTransformNextNoticeMSec	= 0;
		}
		else if (ulClockCount >= m_ulTransformNextNoticeMSec)
		{
			m_ulTransformNextNoticeMSec	= ulClockCount + AGCM_UI_CHARACTER_TRANSFORM_NOTICE_INTERVAL;

			m_ulTransformDurationMSec	-= AGCM_UI_CHARACTER_TRANSFORM_NOTICE_INTERVAL;

			INT32	lRemainTimeMinute	= m_ulTransformDurationMSec / 60 / 1000;

			CHAR	szBuffer[64];
			ZeroMemory(szBuffer, sizeof(CHAR) * 64);

			sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TRANSFORM_REMAIN_NOTICE), lRemainTimeMinute);

			m_pcsAgcmChatting2->AddSystemMessage(szBuffer);
		}
	}

	//if (pcsSelfCharacter
	//	&& 0 < m_ulCriminalNotifyClock
	//	&& ulClockCount > m_ulCriminalNotifyClock + 500
	//	)
	//{
	//	RefreshCharacterStatus();
	//	m_ulCriminalNotifyClock = ulClockCount;
	//}

	return TRUE;
}

BOOL AgcmUICharacter::CBConstBase(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUICharacter *			pThis = (AgcmUICharacter *) pClass;
	ApBase *					pcsBase = (ApdObject *) pData;
	AgcdUICharacterADBase *		pstADBase = pThis->GetBaseData(pcsBase, NULL);

	pstADBase->m_lMinimapIndex	= -1;

	return TRUE;
}

BOOL AgcmUICharacter::CBDestBase(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUICharacter *			pThis = (AgcmUICharacter *) pClass;
	ApBase *					pcsBase = (ApdObject *) pData;
	AgcdUICharacterADBase *		pstADBase = pThis->GetBaseData(pcsBase, NULL);

	if (pstADBase->m_lMinimapIndex >= 0 && pThis->m_pcsAgcmMinimap)
	{
		pThis->m_pcsAgcmMinimap->RemovePoint(pstADBase->m_lMinimapIndex);
		pstADBase->m_lMinimapIndex	= -1;
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBDesCharTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	INT32						lIndex;
	AgcmUICharacter *			pThis = (AgcmUICharacter *) pClass;
	AgpdCharacterTemplate *		pcsTemplate = (AgpdCharacterTemplate *) pData;
	AgcdUICharacterADCharacterTemplate *	pstADCharTemplate;
	AgcdUICharacterLevelupMessage *			pstLevelupMessage;

	pstADCharTemplate = pThis->GetCharacterTemplateData(pcsTemplate);
	if (pstADCharTemplate->m_pstLevelUpMessage)
	{
		for (lIndex = 0; lIndex < AGPMCHAR_MAX_LEVEL; ++lIndex)
		{
			pstLevelupMessage = pstADCharTemplate->m_pstLevelUpMessage + lIndex;

			if (pstLevelupMessage->m_szMessage1)
			{
				delete [] pstLevelupMessage->m_szMessage1;
				pstLevelupMessage->m_szMessage1 = NULL;
			}

			if (pstLevelupMessage->m_szMessage2)
			{
				delete [] pstLevelupMessage->m_szMessage2;
				pstLevelupMessage->m_szMessage2 = NULL;
			}
		}

		delete [] pstADCharTemplate->m_pstLevelUpMessage;
		pstADCharTemplate->m_pstLevelUpMessage = NULL;
	}

	return TRUE;
}

BOOL AgcmUICharacter::AddEvent()
{
	m_lEventLevelUp = m_pcsAgcmUIManager2->AddEvent("레벨업");
	if (m_lEventLevelUp < 0)
		return FALSE;

	m_lEventBonusExp = m_pcsAgcmUIManager2->AddEvent("Exp_획득");
	if (m_lEventBonusExp < 0)
		return FALSE;

	m_lEventBonusPCBangExp = m_pcsAgcmUIManager2->AddEvent("Exp_획득_(PC방_보너스)");
	if (m_lEventBonusPCBangExp < 0)
		return FALSE;

	m_lEventAttackResultNotEnoughArrow = m_pcsAgcmUIManager2->AddEvent("CharacterAtkResultNotEnoughArrow");
	if (m_lEventAttackResultNotEnoughArrow < 0)
		return FALSE;
	m_lEventAttackResultNotEnoughBolt = m_pcsAgcmUIManager2->AddEvent("CharacterAtkResultNotEnoughBolt");
	if (m_lEventAttackResultNotEnoughBolt < 0)
		return FALSE;
	m_lEventAttackResultNotEnoughMP = m_pcsAgcmUIManager2->AddEvent("CharacterAtkResultNotEnoughMP");
	if (m_lEventAttackResultNotEnoughMP < 0)
		return FALSE;

	m_lEventOpenCharacterMenu = m_pcsAgcmUIManager2->AddEvent("CharacterMenuOpen");
	if (m_lEventOpenCharacterMenu < 0)
		return FALSE;

	m_lEventCloseCharacterMenu = m_pcsAgcmUIManager2->AddEvent("CharacterMenuClose");
	if (m_lEventCloseCharacterMenu < 0)
		return FALSE;

	m_lEventOpenTargetInfo = m_pcsAgcmUIManager2->AddEvent("TargetInfoOpen");
	if (m_lEventOpenTargetInfo < 0)
		return FALSE;
	m_lEventCloseTargetInfo = m_pcsAgcmUIManager2->AddEvent("TargetInfoClose");
	if (m_lEventCloseTargetInfo < 0)
		return FALSE;

	m_lEventTargetAttrNone = m_pcsAgcmUIManager2->AddEvent("TargetInfoAttrNone");
	if (m_lEventTargetAttrNone < 0)
		return FALSE;
	m_lEventTargetAttrAir = m_pcsAgcmUIManager2->AddEvent("TargetInfoAttrAir");
	if (m_lEventTargetAttrAir < 0)
		return FALSE;
	m_lEventTargetAttrFire = m_pcsAgcmUIManager2->AddEvent("TargetInfoAttrFire");
	if (m_lEventTargetAttrFire < 0)
		return FALSE;
	m_lEventTargetAttrWater = m_pcsAgcmUIManager2->AddEvent("TargetInfoAttrWater");
	if (m_lEventTargetAttrWater < 0)
		return FALSE;
	m_lEventTargetAttrMagic = m_pcsAgcmUIManager2->AddEvent("TargetInfoAttrMagic");
	if (m_lEventTargetAttrMagic < 0)
		return FALSE;
	m_lEventTargetAttrEarth = m_pcsAgcmUIManager2->AddEvent("TargetInfoAttrEarth");
	if (m_lEventTargetAttrEarth < 0)
		return FALSE;

	m_lEventTargetAttrPoison = m_pcsAgcmUIManager2->AddEvent("TargetInfoAttrPoison");
	if (m_lEventTargetAttrPoison < 0)
		return FALSE;
	m_lEventTargetAttrIce = m_pcsAgcmUIManager2->AddEvent("TargetInfoAttrIce");
	if (m_lEventTargetAttrIce < 0)
		return FALSE;
	m_lEventTargetAttrLightening = m_pcsAgcmUIManager2->AddEvent("TargetInfoAttrLightening");
	if (m_lEventTargetAttrLightening < 0)
		return FALSE;
	m_lEventTargetPKCriminal = m_pcsAgcmUIManager2->AddEvent("TargetPKCriminal");
	if (m_lEventTargetPKCriminal < 0)
		return FALSE;
	m_lEventTargetPKMurdererLevel1 = m_pcsAgcmUIManager2->AddEvent("TargetPKMurdererLevel1");
	if (m_lEventTargetPKMurdererLevel1 < 0)
		return FALSE;
	m_lEventTargetPKMurdererLevel2 = m_pcsAgcmUIManager2->AddEvent("TargetPKMurdererLevel2");
	if (m_lEventTargetPKMurdererLevel2 < 0)
		return FALSE;
	m_lEventTargetPKMurdererLevel3 = m_pcsAgcmUIManager2->AddEvent("TargetPKMurdererLevel3");
	if (m_lEventTargetPKMurdererLevel3 < 0)
		return FALSE;					

	m_lEventTargetableOn = m_pcsAgcmUIManager2->AddEvent("TargetInfoOn");
	if (m_lEventTargetableOn < 0)
		return FALSE;
	m_lEventTargetableOff = m_pcsAgcmUIManager2->AddEvent("TargetInfoOff");
	if (m_lEventTargetableOff < 0)
		return FALSE;

	m_lEventRefreshTargetName = m_pcsAgcmUIManager2->AddEvent("RefreshTargetName");
	if (m_lEventRefreshTargetName < 0)
		return FALSE;

	m_lEventOpenTargetAttrInfo = m_pcsAgcmUIManager2->AddEvent("TargetAttrMenuOpen");
	if (m_lEventOpenTargetAttrInfo < 0)
		return FALSE;
	m_lEventCloseTargetAttrInfo = m_pcsAgcmUIManager2->AddEvent("TargetAttrMenuClose");
	if (m_lEventCloseTargetAttrInfo < 0)
		return FALSE;

	m_lEventOpenTargetUserMenu = m_pcsAgcmUIManager2->AddEvent("TargetUserMenuOpen");
	if (m_lEventOpenTargetUserMenu < 0)
		return FALSE;
	m_lEventCloseTargetUserMenu = m_pcsAgcmUIManager2->AddEvent("TargetUserMenuClose");
	if (m_lEventCloseTargetUserMenu < 0)
		return FALSE;

	// 2005.09.25. steeple
	m_lEventOpenTargetSummonsMenu = m_pcsAgcmUIManager2->AddEvent("TargetUserSummonsMenuOpen");
	if (m_lEventOpenTargetSummonsMenu < 0)
		return FALSE;
	m_lEventCloseTargetSummonsMenu = m_pcsAgcmUIManager2->AddEvent("TargetUserSummonsMenuClose");
	if( m_lEventCloseTargetSummonsMenu < 0)
		return FALSE;
	m_lEventOpenTargetSummonsMenu2 = m_pcsAgcmUIManager2->AddEvent("TargetUserSummonsMenu2Open");
	if (m_lEventOpenTargetSummonsMenu2 < 0)
		return FALSE;
	m_lEventCloseTargetSummonsMenu2 = m_pcsAgcmUIManager2->AddEvent("TargetUserSummonsMenu2Close");
	if( m_lEventCloseTargetSummonsMenu2 < 0)
		return FALSE;
	m_lEventDisplaySummonsPropensity = m_pcsAgcmUIManager2->AddEvent("DisplaySummmonsPropensity");
	if( m_lEventDisplaySummonsPropensity < 0)
		return FALSE;

	m_lEventOpenResurrectionUI = m_pcsAgcmUIManager2->AddEvent("OpenResurrectionUI");
	if (m_lEventOpenResurrectionUI < 0)
		return FALSE;
	m_lEventCloseResurrectionUI = m_pcsAgcmUIManager2->AddEvent("CloseResurrectionUI");
	if (m_lEventCloseResurrectionUI < 0)
		return FALSE;

	m_lEventOpenConfirmResurrectTownUI			= m_pcsAgcmUIManager2->AddEvent("OpenConfirmResurrectTownUI");
	if (m_lEventOpenConfirmResurrectTownUI < 0)
		return FALSE;
	m_lEventOpenConfirmResurrectUI			= m_pcsAgcmUIManager2->AddEvent("OpenConfirmResurrectUI");
	if (m_lEventOpenConfirmResurrectUI < 0)
		return FALSE;

	m_lEventCloseTargetWindow = m_pcsAgcmUIManager2->AddEvent("CloseTargetAllWindow");
	if (m_lEventCloseTargetWindow < 0)
		return FALSE;
	
	m_lEventMurdererPointUp = m_pcsAgcmUIManager2->AddEvent("CharacterMurdererPointUp");
	if (m_lEventMurdererPointUp  < 0)
		return FALSE;

	m_nEventUpdateTargetRaceHuman = m_pcsAgcmUIManager2->AddEvent( "TargetRace_Human" );
	if( m_nEventUpdateTargetRaceHuman < 0 ) return FALSE;
	
	m_nEventUpdateTargetRaceOrc = m_pcsAgcmUIManager2->AddEvent( "TargetRace_Orc" );
	if( m_nEventUpdateTargetRaceOrc < 0 ) return FALSE;

	m_nEventUpdateTargetRaceMoonElf = m_pcsAgcmUIManager2->AddEvent( "TargetRace_MoonElf" );
	if( m_nEventUpdateTargetRaceMoonElf < 0 ) return FALSE;

	m_nEventUpdateTargetRaceDragonScion = m_pcsAgcmUIManager2->AddEvent( "TargetRace_DragonScion" );
	if( m_nEventUpdateTargetRaceDragonScion < 0 ) return FALSE;

	m_nEventUpdateTargetClassKnight = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Knight" );
	if( m_nEventUpdateTargetClassKnight < 0 ) return FALSE;

	m_nEventUpdateTargetClassArcher = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Archer" );
	if( m_nEventUpdateTargetClassArcher < 0 ) return FALSE;

	m_nEventUpdateTargetClassMage = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Mage" );
	if( m_nEventUpdateTargetClassMage < 0 ) return FALSE;

	m_nEventUpdateTargetClassBerserker = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Berserker" );
	if( m_nEventUpdateTargetClassBerserker < 0 ) return FALSE;

	m_nEventUpdateTargetClassHunter = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Hunter" );
	if( m_nEventUpdateTargetClassHunter < 0 ) return FALSE;

	m_nEventUpdateTargetClassSocerer = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Socerer" );
	if( m_nEventUpdateTargetClassSocerer < 0 ) return FALSE;

	m_nEventUpdateTargetClassSwashBuckler = m_pcsAgcmUIManager2->AddEvent( "TargetClass_SwashBuckler" );
	if( m_nEventUpdateTargetClassSwashBuckler < 0 ) return FALSE;

	m_nEventUpdateTargetClassRanger = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Ranger" );
	if( m_nEventUpdateTargetClassRanger < 0 ) return FALSE;

	m_nEventUpdateTargetClassElementalist = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Elementalist" );
	if( m_nEventUpdateTargetClassElementalist < 0 ) return FALSE;

	m_nEventUpdateTargetClassScion = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Scion" );
	if( m_nEventUpdateTargetClassScion < 0 ) return FALSE;

	m_nEventUpdateTargetClassSlayer = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Slayer" );
	if( m_nEventUpdateTargetClassSlayer < 0 ) return FALSE;

	m_nEventUpdateTargetClassObiter = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Obiter" );
	if( m_nEventUpdateTargetClassObiter < 0 ) return FALSE;

	m_nEventUpdateTargetClassSummerner = m_pcsAgcmUIManager2->AddEvent( "TargetClass_Summerner" );
	if( m_nEventUpdateTargetClassSummerner < 0 ) return FALSE;
	// get edit control
	////////////////////////////////////////////////////////////////////////

	m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_CON]			= m_pcsAgcmUIManager2->AddEvent("Char_Get_Edit_Con");
	if (m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_CON] < 0)
		return FALSE;
	m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_WIS]			= m_pcsAgcmUIManager2->AddEvent("Char_Get_Edit_Wis");
	if (m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_WIS] < 0)
		return FALSE;
	m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_DEX]			= m_pcsAgcmUIManager2->AddEvent("Char_Get_Edit_Dex");
	if (m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_DEX] < 0)
		return FALSE;
	m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_STR]			= m_pcsAgcmUIManager2->AddEvent("Char_Get_Edit_Str");
	if (m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_STR] < 0)
		return FALSE;
	m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_INT]			= m_pcsAgcmUIManager2->AddEvent("Char_Get_Edit_Int");
	if (m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_INT] < 0)
		return FALSE;
	m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_CHAR]			= m_pcsAgcmUIManager2->AddEvent("Char_Get_Edit_Char");
	if (m_alEventGetEditControl[AGCD_UI_CHAR_EDIT_CHAR] < 0)
		return FALSE;

	m_lEventOpenSelectSiegeResurrect	= m_pcsAgcmUIManager2->AddEvent("Char_OpenSelectSiegeResurrect");
	if (m_lEventOpenSelectSiegeResurrect < 0)
		return FALSE;

	m_lEventResurrectionByOther			= m_pcsAgcmUIManager2->AddEvent("Char_OpenResurrectionByOther", CBResurrectionByOtherConfirm, this);
	if (m_lEventResurrectionByOther < 0)
		return FALSE;
		
	return TRUE;
}

BOOL AgcmUICharacter::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "RefreshTargetName", CBRefreshTargetName, 1, "EditControl"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "ToggleTargetUserMenu", CBToggleTargetUserMenu, 1, "UserMenu UI"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "ToggleTargetAttrTooltip", CBToggleTargetAttrTooltip, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CloseTargetAttrTooltip", CBCloseTargetAttrTooltip, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "ToggleTargetSummonsMenu", CBToggleTargetSummonsMenu, 1, "SummonsMenu UI"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SummonsPropensityAttackClick", CBSummonsPropensityAttackClick, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SummonsPropensityDefenseClick", CBSummonsPropensityDefenseClick, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SummonsPropensityShadowClick", CBSummonsPropensityShadowClick, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharToggleExpType", CBToggleExpType, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharResurrectionToTown", CBResurrectionToTown, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharResurrectionNow", CBResurrectionNow, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharResurrectionSiege", CBResurrectionSiege, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharCheckResurrectionToTown", CBCheckResurrectionToTown, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharCheckResurrectionNow", CBCheckResurrectionNow, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharResurrectionSiegeInner", CBResurrectionSiegeInner, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharResurrectionSiegeOuter", CBResurrectionSiegeOuter, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharOpenTargetNameTooltip", CBOpenTargetNameTooltip, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharCloseTargetNameTooltip", CBCloseTargetNameTooltip, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharSetFollowTarget", CBSetFollowTarget, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharGetEditCon", GetEditCon, 1, "EditControl"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharGetEditWis", GetEditWis, 1, "EditControl"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharGetEditDex", GetEditDex, 1, "EditControl"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharGetEditStr", GetEditStr, 1, "EditControl"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharGetEditInt", GetEditInt, 1, "EditControl"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CharGetEditChar", GetEditChar, 1, "EditControl"))
		return FALSE;

	if ( !m_pcsAgcmUIManager2->AddFunction( this, "UiCharacterStatusDialog_OnSelectPropertyAttack", CallBack_OnSelectPropertyAttack, 0 ) )
		return FALSE;		

	if ( !m_pcsAgcmUIManager2->AddFunction( this, "UiCharacterStatusDialog_OnSelectPropertyTab", CallBack_OnSelectPropertyTab, 0 ) )
		return FALSE;	

	return TRUE;
}

BOOL AgcmUICharacter::AddDisplay()
{
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_CHAR_NAME, E_AGCD_UI_CHAR_DISP_CHAR_NAME, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_CLASS_NAME, E_AGCD_UI_CHAR_DISP_CLASS_NAME, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_RACE_NAME, E_AGCD_UI_CHAR_DISP_RACE_NAME, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_GENDER_NAME, E_AGCD_UI_CHAR_DISP_GENDER_NAME, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_RACE_CLASS, E_AGCD_UI_CHAR_DISP_RACE_CLASS, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_CUR_LEVEL, E_AGCD_UI_CHAR_DISP_CUR_LEVEL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_CUR_EXP, E_AGCD_UI_CHAR_DISP_CUR_EXP, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_MAX_EXP, E_AGCD_UI_CHAR_DISP_MAX_EXP, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_EXP_PERCENT, E_AGCD_UI_CHAR_DISP_EXP_PERCENT, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_EXP_PERCENT2, E_AGCD_UI_CHAR_DISP_EXP_PERCENT2, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_EXP_BAR, E_AGCD_UI_CHAR_DISP_EXP_BAR, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_CON, E_AGCD_UI_CHAR_DISP_CON, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_WIS, E_AGCD_UI_CHAR_DISP_WIS, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DEX, E_AGCD_UI_CHAR_DISP_DEX, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_STR, E_AGCD_UI_CHAR_DISP_STR, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_INT, E_AGCD_UI_CHAR_DISP_INT, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_CHA, E_AGCD_UI_CHAR_DISP_CHA, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_HP, E_AGCD_UI_CHAR_DISP_HP, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_HP_MAX, E_AGCD_UI_CHAR_DISP_HP_MAX, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_HP_ALL, E_AGCD_UI_CHAR_DISP_HP_ALL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_HP_BAR, E_AGCD_UI_CHAR_DISP_HP_BAR, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_MP, E_AGCD_UI_CHAR_DISP_MP, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_MP_MAX, E_AGCD_UI_CHAR_DISP_MP_MAX, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_MP_ALL, E_AGCD_UI_CHAR_DISP_MP_ALL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_MP_PERCENT, E_AGCD_UI_CHAR_DISP_MP_PERCENT, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_MP_BAR, E_AGCD_UI_CHAR_DISP_MP_BAR, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_SP, E_AGCD_UI_CHAR_DISP_SP, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_SP_MAX, E_AGCD_UI_CHAR_DISP_SP_MAX, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_SP_ALL, E_AGCD_UI_CHAR_DISP_SP_ALL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_SP_PERCENT, E_AGCD_UI_CHAR_DISP_SP_PERCENT, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay("AR", E_AGCD_UI_CHAR_DISP_AR, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay("DR", E_AGCD_UI_CHAR_DISP_DR, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay("AttackSpeed", E_AGCD_UI_CHAR_DISP_ATTACkSPEED, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay("MoveFast", E_AGCD_UI_CHAR_DISP_MOVEFAST, CharacterStatusCB))
		return FALSE;


	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DEFENSE_PHYSICAL, E_AGCD_UI_CHAR_DISP_DEFENSE_PHYSICAL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DEFENSE_MAGIC, E_AGCD_UI_CHAR_DISP_DEFENSE_MAGIC, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DEFENSE_FIRE, E_AGCD_UI_CHAR_DISP_DEFENSE_FIRE, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DEFENSE_WATER, E_AGCD_UI_CHAR_DISP_DEFENSE_WATER, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DEFENSE_AIR, E_AGCD_UI_CHAR_DISP_DEFENSE_AIR, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DEFENSE_EARTH, E_AGCD_UI_CHAR_DISP_DEFENSE_EARTH, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DEFENSE_PARTY, E_AGCD_UI_CHAR_DISP_DEFENSE_PARTY, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_PHYSICAL_MIN, E_AGCD_UI_CHAR_DISP_DAMAGE_PHYSICAL_MIN, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_PHYSICAL_MAX, E_AGCD_UI_CHAR_DISP_DAMAGE_PHYSICAL_MAX, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_PHYSICAL_ALL, E_AGCD_UI_CHAR_DISP_DAMAGE_PHYSICAL_ALL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_MAGIC_MIN, E_AGCD_UI_CHAR_DISP_DAMAGE_MAGIC_MIN, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_MAGIC_MAX, E_AGCD_UI_CHAR_DISP_DAMAGE_MAGIC_MAX, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_MAGIC_ALL, E_AGCD_UI_CHAR_DISP_DAMAGE_MAGIC_ALL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_FIRE_MIN, E_AGCD_UI_CHAR_DISP_DAMAGE_FIRE_MIN, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_FIRE_MAX, E_AGCD_UI_CHAR_DISP_DAMAGE_FIRE_MAX, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_FIRE_ALL, E_AGCD_UI_CHAR_DISP_DAMAGE_FIRE_ALL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_AIR_MIN, E_AGCD_UI_CHAR_DISP_DAMAGE_AIR_MIN, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_AIR_MAX, E_AGCD_UI_CHAR_DISP_DAMAGE_AIR_MAX, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_AIR_ALL, E_AGCD_UI_CHAR_DISP_DAMAGE_AIR_ALL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_WATER_MIN, E_AGCD_UI_CHAR_DISP_DAMAGE_WATER_MIN, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_WATER_MAX, E_AGCD_UI_CHAR_DISP_DAMAGE_WATER_MAX, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_WATER_ALL, E_AGCD_UI_CHAR_DISP_DAMAGE_WATER_ALL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_EARTH_MIN, E_AGCD_UI_CHAR_DISP_DAMAGE_EARTH_MIN, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_EARTH_MAX, E_AGCD_UI_CHAR_DISP_DAMAGE_EARTH_MAX, CharacterStatusCB))
		return FALSE;
	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_EARTH_ALL, E_AGCD_UI_CHAR_DISP_DAMAGE_EARTH_ALL, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_PARTY, E_AGCD_UI_CHAR_DISP_DAMAGE_PARTY, CharacterStatusCB))
		return FALSE;

	if(!AddDisplay(AGCD_UI_CHAR_DISP_NAME_DAMAGE_BASIC_SKILL, E_AGCD_UI_CHAR_DISP_DAMAGE_BASIC_SKILL, CharacterStatusCB))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCD_UI_CHAR_DISP_NAME_TARGET_CHAR_NAME, E_AGCD_UI_CHAR_DISP_TARGET_CHAR_NAME, CBTargetCharName, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCD_UI_CHAR_DISP_NAME_MESSAGE_DATA_LEVEL_UP, E_AGCD_UI_CHAR_DISP_MESSAGE_DATA_LEVEL_UP, CBDisplayMessageData, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCD_UI_CHAR_DISP_NAME_MESSAGE_DATA_BONUS_EXP, E_AGCD_UI_CHAR_DISP_MESSAGE_DATA_BONUS_EXP, CBDisplayMessageData, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, AGCD_UI_CHAR_DISP_NAME_MESSAGE_DATA_BONUS_PCBANG_EXP, E_AGCD_UI_CHAR_DISP_MESSAGE_DATA_BONUS_PCBANG_EXP, CBDisplayMessageData, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, ClientStr().GetStr(STI_TARGET_ATTR), 100, CBTargetAttributeType, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

	// 2005.09.25. steeple
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SummonsPropensity", 0, CBDisplaySummonsPropensity, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SiegeResurrection", 0, CBDisplaySiegeResurrection, AGCDUI_USERDATA_TYPE_BOOL))
		return FALSE;

	// 2007.08.29. steeple
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "OtherName", 0, CBDisplayOtherName, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	return TRUE;
}

BOOL AgcmUICharacter::AddUserData()
{
	if(!AddUserData(AGCD_UI_CHAR_USRDAT_NAME_CUR_EXP, NULL, sizeof(INT32), 1, E_AGCD_UI_CHAR_USRDAT_CUR_EXP))
		return FALSE;
	
	if(!AddUserData(AGCD_UI_CHAR_USRDAT_NAME_MAX_EXP, NULL, sizeof(INT32), 1, E_AGCD_UI_CHAR_USRDAT_MAX_EXP))
			return FALSE;
	
	if(!AddUserData(AGCD_UI_CHAR_USRDAT_NAME_SELF_CHAR, NULL, sizeof(AgpdCharacter), 1, E_AGCD_UI_CHAR_USRDAT_SELF_CHAR))
		return FALSE;

	m_pstUserDataTargetCharacter = m_pcsAgcmUIManager2->AddUserData(AGCD_UI_CHAR_USRDAT_NAME_TARGET_CHAR,
										m_pcsTargetCharacter,
										sizeof(AgpdCharacter),
										1,
										AGCDUI_USERDATA_TYPE_CHARACTER);

	if (!m_pstUserDataTargetCharacter)
		return FALSE;

	m_pstUserDataMessageData = m_pcsAgcmUIManager2->AddUserData(AGCD_UI_CHAR_USRDAT_NAME_MESSAGE_DATA,
										&m_stMessageData,
										sizeof(AgcmUICharacterMessageData),
										1,
										AGCDUI_USERDATA_TYPE_STRING);

	if (!m_pstUserDataMessageData)
		return FALSE;

	m_pstUserDataTargetBuffGrid = m_pcsAgcmUIManager2->AddUserData("TargetBuffGrid",
										&m_csTargetBuffGrid,
										sizeof(AgpdGrid),
										1,
										AGCDUI_USERDATA_TYPE_GRID);

	if (!m_pstUserDataTargetBuffGrid)
		return FALSE;

	m_pstUserDataSelfCharacterDead = m_pcsAgcmUIManager2->AddUserData("SelfCharacterDead", &m_bSelfCharacterDead, sizeof(m_bSelfCharacterDead), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pstUserDataSelfCharacterDead)
		return FALSE;

	// 2005.09.25. steeple
	m_pstUserDataSummonsPropensity = m_pcsAgcmUIManager2->AddUserData("SummonsPropensityUD", &m_lSummonsPropensityUDIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUserDataSummonsPropensity)
		return FALSE;

	// Resurrection시 경험치 페널티?
	m_pstUserDataExpPenalty = m_pcsAgcmUIManager2->AddUserData("ExpPenalty", &m_bExpPenalty, sizeof(m_bExpPenalty), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pstUserDataExpPenalty)
		return FALSE;

	// 2007.08.29. steeple
	m_pstUserDataOtherName = m_pcsAgcmUIManager2->AddUserData("OtherNameUD", m_szOtherName, AGPACHARACTER_MAX_ID_STRING, 1, AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pstUserDataOtherName)
		return FALSE;

	return TRUE;
}

BOOL AgcmUICharacter::AddDisplay(CHAR *szName, INT32 lID, AgcUIDisplayOldCB fCallBack)
{
	AgcdUIDisplay	*pstDisplay;
	
	pstDisplay = m_pcsAgcmUIManager2->CreateDisplay();
	if(!pstDisplay)
		return FALSE;

	pstDisplay->m_szName		= szName;
	pstDisplay->m_lID			= lID;
	pstDisplay->m_pvClass		= this;
	pstDisplay->m_fnCallback	= NULL;
	pstDisplay->m_fnOldCallback	= fCallBack;
	pstDisplay->m_ulDataType	= AGCDUI_USERDATA_TYPE_CHARACTER;

	if(m_pcsAgcmUIManager2->AddDisplay(pstDisplay) < 0)
		return FALSE;

	return TRUE;
}

BOOL AgcmUICharacter::AddDisplay(CHAR *szName, INT32 lID, AgcUIDisplayCB fCallBack)
{
	AgcdUIDisplay	*pstDisplay;
	
	pstDisplay = m_pcsAgcmUIManager2->CreateDisplay();
	if(!pstDisplay)
		return FALSE;

	pstDisplay->m_szName		= szName;
	pstDisplay->m_lID			= lID;
	pstDisplay->m_pvClass		= this;
	pstDisplay->m_fnCallback	= fCallBack;
	pstDisplay->m_fnOldCallback	= NULL;
	pstDisplay->m_ulDataType	= AGCDUI_USERDATA_TYPE_CHARACTER;

	if(m_pcsAgcmUIManager2->AddDisplay(pstDisplay) < 0)
		return FALSE;

	return TRUE;
}


BOOL AgcmUICharacter::AddUserData(CHAR *szName, PVOID pvData, INT32 lDataSize, INT32 lDataCount, INT32 lUserDataIndex)
{
	AgcdUIUserData	*pstUserData;

	pstUserData = m_pcsAgcmUIManager2->CreateUserData();
	if(!pstUserData)
		return FALSE;

	pstUserData->m_szName					= szName;
	pstUserData->m_stUserData.m_pvData		= pvData;
	pstUserData->m_stUserData.m_lDataSize	= lDataSize;
	pstUserData->m_stUserData.m_lCount		= lDataCount;
	pstUserData->m_eType					= AGCDUI_USERDATA_TYPE_CHARACTER;

	if(m_pcsAgcmUIManager2->AddUserData(pstUserData) < 0)
		return FALSE;

	m_pastAgcdUIUserData[lUserDataIndex]	= pstUserData;

	return TRUE;
}

BOOL AgcmUICharacter::AddBoolean()
{
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActiveFollowMenu", CBIsActiveFollowMenu, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsExpPenalty", CBIsExpPenalty, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActiveSiegeResurrection", CBIsActiveSiegeResurrection, AGCDUI_USERDATA_TYPE_BOOL))
		return FALSE;

	return TRUE;
}

BOOL AgcmUICharacter::SetCharacter(AgpdCharacter *pcsAgpdCharacter)
{
	if (!pcsAgpdCharacter)
		return FALSE;

	m_ulCriminalNotifyClock = GetClockCount();

	INT32 *plTemp;

	plTemp = m_pcsAgpmFactors->GetValuePointer(&pcsAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW);
	if(plTemp)
	{
		m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_CUR_EXP]->m_stUserData.m_pvData	= (PVOID)(plTemp);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_CUR_EXP]);
	}

	plTemp = m_pcsAgpmFactors->GetValuePointer(&pcsAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW);
	if(plTemp)
	{
		m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_MAX_EXP]->m_stUserData.m_pvData	= (PVOID)(plTemp);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_MAX_EXP]);
	}

	m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_SELF_CHAR]->m_stUserData.m_pvData	= (PVOID)(pcsAgpdCharacter);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_SELF_CHAR]);

	RefreshCharacterStatus();
	return TRUE;
}

BOOL AgcmUICharacter::ReleaseCharacter()
{
	m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_SELF_CHAR]->m_stUserData.m_pvData	= NULL;
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_SELF_CHAR]);

	m_ulCriminalNotifyClock = 0;

	RefreshCharacterStatus();

	return TRUE;
}

BOOL AgcmUICharacter::CharacterStatusCB(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (eType != AGCDUI_USERDATA_TYPE_CHARACTER)
		return FALSE;

	AgcmUICharacter		*pcsThis		= (AgcmUICharacter *)(pClass);
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)(pData);

	if (!pcsThis)
		return FALSE;

	if (!pcsCharacter)
		return pcsThis->ResetDisplayCharacterStatus(szDisplay, plValue);

	AgpmFactors			*pcsAgpmFactors	= pcsThis->GetAgpmFactorsModule();
	if (!pcsAgpmFactors)
		return FALSE;

	AgpdFactor			*pcsFactor		= (AgpdFactor *) pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);

	INT32				lTemp			= 0;
	INT32				lTemp2			= 0;

	switch(lID)
	{
	case E_AGCD_UI_CHAR_DISP_CHAR_NAME:
		{
			sprintf(szDisplay, "%s", pcsCharacter->m_szID);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_CLASS_NAME:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

			CHAR	*szClassName	= pcsAgpmFactors->GetCharacterClassName(lTemp, lTemp2); 
			if (szClassName && szClassName[0])
				sprintf(szDisplay, "%s", szClassName);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_RACE_NAME:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);

			CHAR	*szRaceName	= pcsAgpmFactors->GetCharacterRaceName(lTemp); 
			if (szRaceName && szRaceName[0])
				sprintf(szDisplay, "%s", szRaceName);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_GENDER_NAME:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_GENDER);

			CHAR	*szGenderName	= pcsAgpmFactors->GetCharacterGenderName(lTemp); 
			if (szGenderName && szGenderName[0])
				sprintf(szDisplay, "%s", szGenderName);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_RACE_CLASS:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

			CHAR	*szClassName	= pcsAgpmFactors->GetCharacterClassName(lTemp, lTemp2); 
			CHAR	*szRaceName		= pcsAgpmFactors->GetCharacterRaceName(lTemp);

			if (szRaceName && szRaceName[0])
				sprintf(szDisplay, "%s", szRaceName);

			if (szClassName && szClassName[0])
				sprintf(szDisplay + strlen(szDisplay), " %s", szClassName);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_CUR_LEVEL:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);

			ApmMap::RegionTemplate	*pcsRegionTemplate = pcsThis->m_pcsApmMap->GetTemplate( pcsCharacter->m_nBindingRegionIndex);
			if( pcsRegionTemplate && pcsRegionTemplate->nLevelLimit )
			{
				INT32	nLevelOriginal	= pcsAgpmFactors->GetLevel		( &pcsCharacter->m_csFactor );
				INT32	nLevelLimit		= pcsRegionTemplate->nLevelLimit;

				if( nLevelLimit <= nLevelOriginal )
					pcsSourceControl->m_pcsBase->SetBlink( TRUE , 0xffff0000 , 1000 );
				else
					pcsSourceControl->m_pcsBase->SetBlink( FALSE );
			}
			else
			{
				pcsSourceControl->m_pcsBase->SetBlink( FALSE );
			}
		}
		break;

	case E_AGCD_UI_CHAR_DISP_CUR_EXP:
		{
			INT64	llCurrentExp	= pcsAgpmFactors->GetExp(pcsFactor);

			INT64	llMaxExp		= pcsAgpmFactors->GetMaxExp(pcsFactor);

			if (llMaxExp < llCurrentExp)
				llCurrentExp	= llMaxExp;

			// 64비트 값을 32비트로 캐스팅해서 넘기기 때문에 102레벨 이후로 경험치 수치값이 짤림
			// 그래서 경험치 바를 업데이트하기 위한 수치값은 100 을 기준으로 한 비율값으로 변경해서 넘기도록 수정함
			if( llMaxExp > 0 )
			{
				*plValue = ( int )( ( float )( ( double )llCurrentExp / ( double )llMaxExp ) * 100.0f );
			}
			else
			{
				*plValue = 0;
			}

			sprintf(szDisplay, "%I64d / %I64d", llCurrentExp, llMaxExp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_MAX_EXP:
		{
			INT64	llMaxExp	= pcsAgpmFactors->GetMaxExp(pcsFactor);

			// 64비트 값을 32비트로 캐스팅해서 넘기기 때문에 102레벨 이후로 경험치 수치값이 짤림
			// 그래서 경험치 바를 업데이트하기 위한 수치값은 100 을 기준으로 한 비율값으로 변경해서 넘기도록 수정함
			//*plValue = (INT32) llMaxExp;
			*plValue = 100;

			sprintf(szDisplay, "%I64d", llMaxExp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_EXP_PERCENT:
		{
			INT64	llCurrentExp	= pcsAgpmFactors->GetExp(pcsFactor);
			INT64	llMaxExp		= pcsAgpmFactors->GetMaxExp(pcsFactor);

			if (llCurrentExp > llMaxExp)
				llCurrentExp	= llMaxExp;

			if (llCurrentExp > 0 && llMaxExp > 0)
				sprintf(szDisplay, "%.2f%%", (double)llCurrentExp * (double)100 / (double)(llMaxExp));
			else
				sprintf(szDisplay, "0%%");
		}
		break;

	case E_AGCD_UI_CHAR_DISP_EXP_PERCENT2:
		{
			INT64	llCurrentExp	= pcsAgpmFactors->GetExp(pcsFactor);
			INT64	llMaxExp		= pcsAgpmFactors->GetMaxExp(pcsFactor);

			if (llCurrentExp > llMaxExp)
				llCurrentExp	= llMaxExp;

			if (pcsThis->m_bIsExpTypePercent)
			{
				if (llCurrentExp > 0 && llMaxExp > 0)
					sprintf(szDisplay, "%s %.2f%%", ClientStr().GetStr(STI_EXP), (double)llCurrentExp * (double)100 / (double)(llMaxExp));
				else
					sprintf(szDisplay, " 0%%", ClientStr().GetStr(STI_EXP));
			}
			else
			{
				sprintf(szDisplay, "%s %I64d/%I64d", ClientStr().GetStr(STI_EXP), llCurrentExp, llMaxExp);
			}
		}
		break;

	case E_AGCD_UI_CHAR_DISP_EXP_BAR:
		{
			INT64	llCurrentExp	= pcsAgpmFactors->GetExp(pcsFactor);
			INT64	llMaxExp		= pcsAgpmFactors->GetMaxExp(pcsFactor);

			if (llCurrentExp > llMaxExp)
				llCurrentExp	= llMaxExp;

			sprintf(szDisplay, "EXP %I64d/%I64d", llCurrentExp, llMaxExp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_CON:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_WIS:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);
			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DEX:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_STR:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);
			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_INT:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);
			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_CHA:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_HP:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
			if (lTemp < 0)
				lTemp	= 0;

			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
			if (lTemp > lTemp2)
				lTemp	= lTemp2;

			if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
				lTemp	= 0;

			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_HP_MAX:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_HP_ALL:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

			if (lTemp < 0)
				lTemp	= 0;
			if (lTemp > lTemp2)
				lTemp	= lTemp2;

			if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
				lTemp	= 0;

			sprintf(szDisplay, "%d/%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_HP_BAR:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

			if (lTemp < 0)
				lTemp	= 0;
			if (lTemp > lTemp2)
				lTemp	= lTemp2;

			if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
				lTemp	= 0;

			sprintf(szDisplay, "HP %d/%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_MP:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
			if (lTemp < 0)
				lTemp	= 0;

			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
			if (lTemp > lTemp2)
				lTemp	= lTemp2;

			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_MP_MAX:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_MP_ALL:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

			if (lTemp < 0)
				lTemp	= 0;
			if (lTemp > lTemp2)
				lTemp	= lTemp2;

			sprintf(szDisplay, "%d/%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_MP_PERCENT:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

			if (lTemp < 0)
				lTemp	= 0;
			if (lTemp > lTemp2)
				lTemp	= lTemp2;

			if (lTemp > 0 && lTemp2 > 0)
				sprintf(szDisplay, "%d%%", (INT32) (lTemp / (lTemp2 + 0.0) * 100));
			else
				sprintf(szDisplay, "0%%");
		}
		break;

	case E_AGCD_UI_CHAR_DISP_MP_BAR:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

			if (lTemp < 0)
				lTemp	= 0;
			if (lTemp > lTemp2)
				lTemp	= lTemp2;

			sprintf(szDisplay, "MP %d/%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_SP:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
			if (lTemp < 0)
				lTemp	= 0;

			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);
			if (lTemp > lTemp2)
				lTemp	= lTemp2;

			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_SP_MAX:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);
			*plValue = lTemp;
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_SP_ALL:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

			if (lTemp < 0)
				lTemp	= 0;
			if (lTemp > lTemp2)
				lTemp	= lTemp2;

			sprintf(szDisplay, "%d/%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_SP_PERCENT:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp2, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

			if (lTemp < 0)
				lTemp	= 0;
			if (lTemp > lTemp2)
				lTemp	= lTemp2;

			if (lTemp > 0 && lTemp2 > 0)
				sprintf(szDisplay, "%d%%", (INT32) (lTemp / (lTemp2 + 0.0) * 100));
			else
				sprintf(szDisplay, "0%%");
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DEFENSE_PHYSICAL:
		{
			pcsAgpmFactors->GetValue(pcsFactor, &lTemp, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DEFENSE_MAGIC:
		{
			INT32	lDefense	= pcsThis->m_pcsAgpmCombat->CalcFirstSpiritDefense(pcsCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
			sprintf(szDisplay, "%d%%", lDefense);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DEFENSE_FIRE:
		{
			INT32	lDefense	= pcsThis->m_pcsAgpmCombat->CalcFirstSpiritDefense(pcsCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
			sprintf(szDisplay, "%d%%", lDefense);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DEFENSE_WATER:
		{
			INT32	lDefense	= pcsThis->m_pcsAgpmCombat->CalcFirstSpiritDefense(pcsCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
			sprintf(szDisplay, "%d%%", lDefense);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DEFENSE_AIR:
		{
			INT32	lDefense	= pcsThis->m_pcsAgpmCombat->CalcFirstSpiritDefense(pcsCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
			sprintf(szDisplay, "%d%%", lDefense);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DEFENSE_EARTH:
		{
			INT32	lDefense	= pcsThis->m_pcsAgpmCombat->CalcFirstSpiritDefense(pcsCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
			sprintf(szDisplay, "%d%%", lDefense);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DEFENSE_PARTY:
		{// 없다... ㅠ.ㅠ
			sprintf(szDisplay, "%d", 0);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_PHYSICAL_MIN:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcPhysicalAttack(pcsCharacter, NULL, pcsThis->m_pcsAgpmItem->IsEquipWeapon(pcsCharacter), TRUE, TRUE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_PHYSICAL_MAX:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcPhysicalAttack(pcsCharacter, NULL, pcsThis->m_pcsAgpmItem->IsEquipWeapon(pcsCharacter), TRUE, FALSE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_PHYSICAL_ALL:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcPhysicalAttack(pcsCharacter, NULL, pcsThis->m_pcsAgpmItem->IsEquipWeapon(pcsCharacter), TRUE, TRUE);
			lTemp2 = pcsThis->m_pcsAgpmCombat->CalcPhysicalAttack(pcsCharacter, NULL, pcsThis->m_pcsAgpmItem->IsEquipWeapon(pcsCharacter), TRUE, FALSE);

			sprintf(szDisplay, "%d~%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_MAGIC_MIN:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC, TRUE, TRUE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_MAGIC_MAX:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC, TRUE, FALSE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_MAGIC_ALL:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC, TRUE, TRUE);
			lTemp2 = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC, TRUE, FALSE);

			sprintf(szDisplay, "%d~%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_FIRE_MIN:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE, TRUE, TRUE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_FIRE_MAX:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE, TRUE, FALSE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_FIRE_ALL:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE, TRUE, TRUE);
			lTemp2 = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE, TRUE, FALSE);

			sprintf(szDisplay, "%d~%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_WATER_MIN:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER, TRUE, TRUE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_WATER_MAX:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER, TRUE, FALSE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_WATER_ALL:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER, TRUE, TRUE);
			lTemp2 = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER, TRUE, FALSE);

			sprintf(szDisplay, "%d~%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_AIR_MIN:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR, TRUE, TRUE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_AIR_MAX:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR, TRUE, FALSE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_AIR_ALL:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR, TRUE, TRUE);
			lTemp2 = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR, TRUE, FALSE);

			sprintf(szDisplay, "%d~%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_EARTH_MIN:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH, TRUE, TRUE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_EARTH_MAX:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH, TRUE, FALSE);

			sprintf(szDisplay, "%d", lTemp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_EARTH_ALL:
		{
			if (!pcsThis->m_pcsAgpmCombat)
				return FALSE;

			lTemp = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH, TRUE, TRUE);
			lTemp2 = pcsThis->m_pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH, TRUE, FALSE);

			sprintf(szDisplay, "%d~%d", lTemp, lTemp2);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_PARTY:
		{ // 몰라... ㅠ.ㅠ
			sprintf(szDisplay, "%d", 0);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_DAMAGE_BASIC_SKILL:
		{
			sprintf(szDisplay, "%d", pcsThis->m_pcsAgpmCombat->CalcPhysicalAttackForSkill(pcsCharacter, NULL, pcsThis->m_pcsAgpmItem->IsEquipWeapon(pcsCharacter), TRUE));
		}
		break;

	case E_AGCD_UI_CHAR_DISP_AR:
		sprintf(szDisplay, "%d", (INT32) pcsThis->m_pcsAgpmCombat->GetAR(pcsCharacter, pcsThis->m_pcsAgpmFactors->GetClass(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor)));
		break;

	case E_AGCD_UI_CHAR_DISP_DR:
		sprintf(szDisplay, "%d", (INT32) pcsThis->m_pcsAgpmCombat->GetDR(pcsCharacter, pcsThis->m_pcsAgpmFactors->GetClass(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor)));
		break;

	case E_AGCD_UI_CHAR_DISP_MOVEFAST:
		{
			INT32	lOriginalMoveFast	= 0;

			if (pcsCharacter->m_bIsTrasform)
			{
				AgpdCharacterTemplate	*pcsOriginalTemplate	= pcsThis->m_pcsAgpmCharacter->GetCharacterTemplate(pcsCharacter->m_lOriginalTID);
				if (pcsOriginalTemplate)
					pcsThis->m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csFactor, &lOriginalMoveFast, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
			}
			else
			{
				pcsThis->m_pcsAgpmFactors->GetValue(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor, &lOriginalMoveFast, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
			}

			INT32	lMoveFast			= 0;

			pcsThis->m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMoveFast, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			if (lOriginalMoveFast == 0)
				sprintf(szDisplay, "0%%");
			else
				sprintf(szDisplay, "%d%%", (INT32) ((double)lMoveFast * (double)100 / (double)lOriginalMoveFast));
		}
		break;

	case E_AGCD_UI_CHAR_DISP_ATTACkSPEED:
		{
			INT32	lAttackSpeed	= 0;

			pcsThis->m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lAttackSpeed, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);

			INT32	lOriginalAttackSpeed	= 0;

			if (pcsCharacter->m_bRidable)
			{
				pcsThis->m_pcsAgpmFactors->GetValue(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor, &lOriginalAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
			}
			else
			{
				AgcmItem* pcmItem = ( AgcmItem* )pcsThis->GetModule( "AgcmItem" );
				if( pcmItem )
				{
					AgpdItem	*pcsWeapon		= pcmItem->GetCurrentEquipWeapon(pcsCharacter);
					if (pcsWeapon)
						pcsThis->m_pcsAgpmFactors->GetValue(&pcsWeapon->m_csFactor, &lOriginalAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
					else
					{
						if (pcsCharacter->m_bIsTrasform)
						{
							AgpdCharacterTemplate	*pcsOriginalTemplate	= pcsThis->m_pcsAgpmCharacter->GetCharacterTemplate(pcsCharacter->m_lOriginalTID);
							if (pcsOriginalTemplate)
								pcsThis->m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csFactor, &lOriginalAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
						}
						else
						{
							pcsThis->m_pcsAgpmFactors->GetValue(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor, &lOriginalAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
						}
					}
				}
			}

			if (lOriginalAttackSpeed == 0)
				sprintf(szDisplay, "0%%");
			else
				sprintf(szDisplay, "%d%%", (INT32) ((double)lAttackSpeed * (double)100 / (double)lOriginalAttackSpeed));
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBTargetCharName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pData ||
		eType != AGCDUI_USERDATA_TYPE_CHARACTER ||
		!szDisplay ||
		!pcsSourceControl)
		return FALSE;

	AgcmUICharacter* pThis = (AgcmUICharacter *)	pClass;
	AgpdCharacter* pcsTargetCharacter = (AgpdCharacter *)pData;
	AgpdCharacter* pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();

	if( pThis->m_pcsAgcmBattleGround->IsOtherRace( pcsTargetCharacter ) ||
		pThis->m_pcsApmMap->CheckRegionPerculiarity( pcsSelfCharacter->m_nBindingRegionIndex , APMMAP_PECULIARITY_SHOWNAME ) == APMMAP_PECULIARITY_RETURN_DISABLE_USE )
	{
		pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor	= 0xffffff;
		for( int i=0; i<(int)strlen( pcsTargetCharacter->m_szID ); ++i )
			szDisplay[i] = '*';
		szDisplay[strlen( pcsTargetCharacter->m_szID )] = NULL;
		return TRUE;
	}

	INT32	lSelfLevel		= pThis->m_pcsAgpmCharacter->GetLevel(pcsSelfCharacter);
	INT32	lTargetLevel	= pThis->m_pcsAgpmCharacter->GetLevel(pcsTargetCharacter);

	DWORD	color	= 0;

	INT32	diff = lTargetLevel - lSelfLevel;

	if( diff >= -3 && diff <= 3)
	{
		color = 0xffffff;	//. 기본 흰색
	}
	else if( diff >= 4)
	{
		color = 0xff8000;	//. 주황
	}
	else if( diff <= -4)			
	{
		color = 0x00ff00;	//. 연두
	}
		
	pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor	= color;

	if (strlen(pcsTargetCharacter->m_szID) > 0)
		sprintf(szDisplay, "%s", pcsTargetCharacter->m_szID);
	else
	{
		if (pcsTargetCharacter->m_lFixTID != pcsTargetCharacter->m_pcsCharacterTemplate->m_lID &&
			pThis->m_pcsAgpmCharacter->IsPolyMorph(pcsTargetCharacter))
		{
			AgpdCharacterTemplate	*pcsOriginalTemplate	= pThis->m_pcsAgpmCharacter->GetCharacterTemplate(pcsTargetCharacter->m_lFixTID);
			if (pcsOriginalTemplate)
				sprintf(szDisplay, "%s", pcsOriginalTemplate->m_szTName);
			else
				sprintf(szDisplay, "%s", pcsTargetCharacter->m_pcsCharacterTemplate->m_szTName);
		}
		else
			sprintf(szDisplay, "%s", pcsTargetCharacter->m_pcsCharacterTemplate->m_szTName);
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBDisplayMessageData(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData ||
		eType != AGCDUI_USERDATA_TYPE_STRING ||
		!szDisplay)
		return FALSE;

	switch (lID) {
	case E_AGCD_UI_CHAR_DISP_MESSAGE_DATA_LEVEL_UP:
		{
			sprintf(szDisplay, "%d", ((AgcmUICharacterMessageData *) pData)->lLevelUp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_MESSAGE_DATA_BONUS_EXP:
		{
			sprintf(szDisplay, "%d", ((AgcmUICharacterMessageData *) pData)->lBonusExp);
		}
		break;

	case E_AGCD_UI_CHAR_DISP_MESSAGE_DATA_BONUS_PCBANG_EXP:
		{
			sprintf(szDisplay, "%d", ((AgcmUICharacterMessageData *) pData)->lBonusPCBangExp);
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBTargetAttributeType(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass ||
		!szDisplay)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;

	switch (pThis->m_lTargetAttributeType) {
	case AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_MAGIC));
		break;

	case AGPD_FACTORS_ATTRIBUTE_TYPE_WATER:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_WATER));
		break;

	case AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_FIRE));
		break;

	case AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_EARTH));
		break;

	case AGPD_FACTORS_ATTRIBUTE_TYPE_AIR:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_AIR));
		break;

	case AGPD_FACTORS_ATTRIBUTE_TYPE_POISON:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_POISON));
		break;

	case AGPD_FACTORS_ATTRIBUTE_TYPE_ICE:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_ICE));
		break;

	case AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_LIGHTENING));
		break;

	default:
		sprintf( szDisplay , "%s" , "" );
		break;
	}

	pThis->m_lTargetAttributeType	=	0;
	return TRUE;
}

BOOL AgcmUICharacter::CharacterUpdateFactorCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUICharacter		*pcsThis			= (AgcmUICharacter *)(pClass);
	AgpdCharacter		*pcsAgpdCharacter	= (AgpdCharacter *)(pData);

	if((!pcsThis) || (!pcsAgpdCharacter))
		return FALSE;

	INT32 *plTemp;
	
	plTemp = pcsThis->m_pcsAgpmFactors->GetValuePointer(&pcsAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW);
	if(plTemp)
	{
		pcsThis->m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_CUR_EXP]->m_stUserData.m_pvData	= (PVOID)(plTemp);
		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_CUR_EXP]);
	}

	plTemp = pcsThis->m_pcsAgpmFactors->GetValuePointer(&pcsAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW);
	if(plTemp)
	{
		pcsThis->m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_MAX_EXP]->m_stUserData.m_pvData	= (PVOID)(plTemp);
		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_MAX_EXP]);
	}

	if(pcsThis->m_pcsAgcmCharacter->m_pcsSelfCharacter &&
		pcsAgpdCharacter->m_lID == pcsThis->m_pcsAgcmCharacter->m_pcsSelfCharacter->m_lID)
	{
		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_SELF_CHAR]);

		pcsThis->RefreshCharacterStatus();
	}

	if (pcsThis->m_pcsTargetCharacter &&
		pcsThis->m_pcsTargetCharacter == pcsAgpdCharacter)
	{
		pcsThis->RefreshUserDataTargetCharacter();
	}

	return TRUE;
}

BOOL AgcmUICharacter::CharacterLevelUp(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter ||
		pcsSelfCharacter->m_lID != pcsCharacter->m_lID)
		return TRUE;

	INT32				lCurrentLevel		= pThis->m_pcsAgpmCharacter->GetLevel(pcsSelfCharacter);

	// 레벨이 lCurrentLevel로 변경되었다는걸 알려준다.
	pThis->m_stMessageData.lLevelUp	= lCurrentLevel;
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventLevelUp);

	// 레벨업에 따른 LevelUp 메세지 표시
	AgcdUICharacterADCharacterTemplate *	pstADCharTemplate;
	AgcdUICharacterLevelupMessage *			pstMessage;
	BOOL									bProcessed = FALSE;

	pstADCharTemplate = pThis->GetCharacterTemplateData(pcsSelfCharacter->m_pcsCharacterTemplate);
	if (pstADCharTemplate->m_pstLevelUpMessage && lCurrentLevel < AGPMCHAR_MAX_LEVEL)
	{
		pstMessage = pstADCharTemplate->m_pstLevelUpMessage + lCurrentLevel;
		if (pstMessage->m_szMessage1)
		{
			pThis->m_pcsAgcmUIManager2->Notice(pstMessage->m_szMessage1, pstMessage->m_dwColor1, pstMessage->m_szMessage2, pstMessage->m_dwColor2, -1, 0.4f);
			bProcessed = TRUE;
		}
	}

	if (!bProcessed)
	{
		pstMessage = pThis->m_astCommonLevelupMessage + lCurrentLevel;
		if (pstMessage->m_szMessage1)
		{
			pThis->m_pcsAgcmUIManager2->Notice(pstMessage->m_szMessage1, pstMessage->m_dwColor1, pstMessage->m_szMessage2, pstMessage->m_dwColor2, -1, 0.4f);
			bProcessed = TRUE;
		}
	}

	return TRUE;
}

BOOL AgcmUICharacter::CharacterUpdateFactorBonusExpCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	AgpdFactor			*pcsFactor			= (AgpdFactor *)		pData;

	INT32				lOwnerCID			= AP_INVALID_CID;

	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lOwnerCID, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_ID);

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	// 자기 자신일 경우만 처리한다.
	if (pcsSelfCharacter->m_lID != lOwnerCID)
		return TRUE;

	FLOAT				fBonusExp			= 0.0f;
	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &fBonusExp, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_BONUS_EXP);

	if (fBonusExp > 0.0f)
	{
		// Get Bonus Exp. Display message about get bonus exp.
		pThis->m_stMessageData.lBonusExp	= (INT32) fBonusExp;
		pThis->m_stMessageData.lBonusPCBangExp	= (INT32) (((INT32) fBonusExp) / 11.0f);

		if (pThis->m_stMessageData.lBonusPCBangExp <= 0 ||
			fBonusExp == (FLOAT) (INT32) fBonusExp ||
			!pThis->m_pcsAgpmBillInfo->IsPCBang(pcsSelfCharacter))
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBonusExp);
		else
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBonusPCBangExp);
	}
	else if (fBonusExp < 0.0f)
	{
	}

	return TRUE;
}

BOOL AgcmUICharacter::CharacterUpdateFactorMurdererPointCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICharacter	*pThis = (AgcmUICharacter *) pClass;
	AgpdFactor		*pcsFactor = (AgpdFactor *) pData;
	INT32			lOldMurdererPoint = *((INT32 *) pCustData);

	AgpdCharacter *pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	// 내꺼 아니면 배째삼.
	INT32 lOwnerID = 0;
	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lOwnerID, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_ID);
	if (lOwnerID == 0 || pcsSelfCharacter->m_lID != lOwnerID)
		return FALSE;

	// 포인트를 얻었다는 메시지를 뿌려주자.
	INT32 lNewMurdererPoint = pThis->m_pcsAgpmCharacter->GetMurdererPoint(pcsSelfCharacter);
	INT32 lDiff = lNewMurdererPoint - lOldMurdererPoint;
	if (lDiff > 0)
	{
		// for sound	
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMurdererPointUp);	
	
		CHAR szBuffer[256];
		CHAR *pszFormat = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_MURDERER_GET_POINT);
		if (pszFormat)
		{
			sprintf(szBuffer, pszFormat, lDiff);
			pThis->m_pcsAgcmChatting2->AddSystemMessage(szBuffer, 0xffcc00);
		}
	}

	INT32 lNewLevel = pThis->m_pcsAgpmCharacter->GetMurdererLevelOfPoint(lNewMurdererPoint);
	INT32 lOldLevel = pThis->m_pcsAgpmCharacter->GetMurdererLevelOfPoint(lOldMurdererPoint);
	CHAR *psz = NULL;
	// 레벨이 오르면 거시기 하자.
	if (lNewLevel > lOldLevel)
	{
		switch (lNewLevel)
		{
			case AGPMCHAR_MURDERER_LEVEL1_POINT	:
				psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_BECOME_MURDERER_LV1);
				break;
				
			case AGPMCHAR_MURDERER_LEVEL2_POINT	:
				psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_BECOME_MURDERER_LV2);
				break;
							
			case AGPMCHAR_MURDERER_LEVEL3_POINT	:
				psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_BECOME_MURDERER_LV3);
				break;			
		}
	}

	// 내릴때는 Message dialog문제로 UIRemission으로 옮김.

	if (psz)
		pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog(psz);

	pThis->RefreshCharacterStatus();

	return TRUE;
}

BOOL AgcmUICharacter::CharacterUpdateFactorCharismaPointCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICharacter	*pThis = (AgcmUICharacter *) pClass;
	AgpdFactor		*pcsFactor = (AgpdFactor *) pData;
	INT32			lOldPoint = *((INT32 *) pCustData);

	AgpdCharacter *pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	// 내꺼 아니면 배째삼.
	INT32 lOwnerID = 0;
	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lOwnerID, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_ID);
	if (lOwnerID == 0 || pcsSelfCharacter->m_lID != lOwnerID)
		return FALSE;
	
	pThis->RefreshCharacterStatus();

	return TRUE;
}

BOOL AgcmUICharacter::CharacterUpdateCriminalStatusCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICharacter	*pThis = (AgcmUICharacter *) pClass;
	AgpdCharacter	*pcsCharacter = (AgpdCharacter *) pData;
	AgpdCharacterCriminalStatus	eOldStatus = *((AgpdCharacterCriminalStatus *) pCustData);
	
	AgpdCharacter *pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter || pcsSelfCharacter != pcsCharacter)
		return FALSE;
	
	INT8 cOldStatus = (INT8) eOldStatus;

	if (cOldStatus != pcsSelfCharacter->m_unCriminalStatus)
	{
		if (AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED == pcsSelfCharacter->m_unCriminalStatus)
		{
			CHAR *psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_BECOME_FIRST_ATTACKER);
			if (psz)
				pThis->m_pcsAgcmChatting2->AddSystemMessage(psz, 0xFF0000);				
		}
		else
		{
			CHAR *psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_FREE_FIRST_ATTACKER);
			if (psz)
				pThis->m_pcsAgcmChatting2->AddSystemMessage(psz, 0x00FF00);
		}
	}
		
	pThis->RefreshCharacterStatus();

	return TRUE;
}

BOOL AgcmUICharacter::CharacterBlockByPenalty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUICharacter			*pThis = (AgcmUICharacter *) pClass;
	AgpdCharacter			*pcsCharacter = (AgpdCharacter *) pData;
	eAgpmCharacterPenalty	ePenalty = *((eAgpmCharacterPenalty *) pCustData);

	AgpdCharacter *pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter || pcsSelfCharacter != pcsCharacter)
		return FALSE;

	if (AGPMCHAR_PENALTY_NONE >= ePenalty || ePenalty >= AGPMCHAR_PENALTY_MAX)
		return FALSE;	
	
	
	CHAR szKey[128];
	ZeroMemory(szKey, sizeof(szKey));
	
	sprintf(szKey, "%s%s", UI_MESSAGE_ID_BLOCK_BY_PENALTY_BASE, g_szPenalty[ePenalty]);
	
	CHAR *psz = pThis->m_pcsAgcmUIManager2->GetUIMessage(szKey);
	if (psz)
		pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog(psz);
	
	return TRUE;
}

BOOL AgcmUICharacter::SetSelfCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUICharacter		*pcsThis			= (AgcmUICharacter *)(pClass);
	AgpdCharacter		*pcsAgpdCharacter	= (AgpdCharacter *)(pData);

	if((!pcsThis) || (!pcsAgpdCharacter))
		return FALSE;

	pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pstUserDataExpPenalty);

	if (pcsAgpdCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_lEventCloseResurrectionUI);
	else
	{
		pcsThis->m_bSelfCharacterDead	= TRUE;
		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_lEventOpenResurrectionUI);

		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pstUserDataSelfCharacterDead);
	}

	for (int i = AGCD_UI_CHAR_EDIT_CON; i < AGCD_UI_CHAR_EDIT_MAX ; ++i)
	{
		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_alEventGetEditControl[i], 0, TRUE);
	}

	pcsThis->SetCharacter(pcsAgpdCharacter);
	return TRUE;
}

BOOL AgcmUICharacter::ReleaseSelfCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter	*pcsThis		= (AgcmUICharacter *)	pClass;

	return pcsThis->ReleaseCharacter();
}

BOOL AgcmUICharacter::UpdateEquipItemCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter	*pThis			= (AgcmUICharacter *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)			pData;

	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return TRUE;

	if (!pcsItem->m_pcsCharacter || pcsItem->m_pcsCharacter->m_lID != pcsSelfCharacter->m_lID)
		return TRUE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_SELF_CHAR]);

	pThis->RefreshCharacterStatus();
	return TRUE;
}

BOOL AgcmUICharacter::CBLockTarget(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	AgcdCharacter			*pcsAgcdCharacter	= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

	AgpdCharacter			*pcsTargetCharacter	= pThis->m_pcsAgpmCharacter->GetCharacter(pcsAgcdCharacter->m_lLockTargetID);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventTargetableOn);
	pThis->SetTargetCharacter(pcsTargetCharacter);
	return TRUE;
}

BOOL AgcmUICharacter::CBUnlockTarget(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventTargetableOff);

	return TRUE;
}

BOOL AgcmUICharacter::CBSelectTarget(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	AgcdCharacter			*pcsAgcdCharacter	= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

	AgpdCharacter			*pcsTargetCharacter	= pThis->m_pcsAgpmCharacter->GetCharacter(pcsAgcdCharacter->m_lSelectTargetID);

	pThis->SetTargetCharacter(pcsTargetCharacter);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventTargetableOff);

	return TRUE;
}

BOOL AgcmUICharacter::CBUnSelectTarget(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	pThis->ResetTargetCharacter();

	return TRUE;
}

BOOL AgcmUICharacter::CBCancelTransform(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)		pClass;

	AgpdCharacter* ppdMyCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	INT32 nMyCID = pThis->m_pcsAgcmCharacter->GetSelfCID();
	if( !ppdMyCharacter ) return FALSE;

	if( ppdMyCharacter->m_bIsTrasform )
	{
		// 변신 취소한다고 서버로 보낸다.
		return pThis->m_pcsAgcmCharacter->SendCancelTransform(nMyCID);
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBCancelEvolution(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)		pClass;

	AgpdCharacter* ppdMyCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	INT32 nMyCID = pThis->m_pcsAgcmCharacter->GetSelfCID();
	if( !ppdMyCharacter ) return FALSE;

	if( ppdMyCharacter->m_bIsEvolution )
	{
		return pThis->m_pcsAgcmCharacter->SendCancelEvolution(nMyCID);
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBAttackResultNotEnoughArrow(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter || pcsSelfCharacter->m_lID != pcsCharacter->m_lID)
		return TRUE;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventAttackResultNotEnoughArrow);
}

BOOL AgcmUICharacter::CBAttackResultNotEnoughBolt(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter || pcsSelfCharacter->m_lID != pcsCharacter->m_lID)
		return TRUE;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventAttackResultNotEnoughBolt);
}

BOOL AgcmUICharacter::CBAttackResultNotEnoughMP(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter || pcsSelfCharacter->m_lID != pcsCharacter->m_lID)
		return TRUE;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventAttackResultNotEnoughMP);
}

BOOL AgcmUICharacter::RefreshUserDataTargetCharacter()
{
	if (!m_pcsTargetCharacter)
		return FALSE;

	m_pstUserDataTargetCharacter->m_stUserData.m_pvData = m_pcsTargetCharacter;

	if (!m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUserDataTargetCharacter))
		return FALSE;

	return TRUE;
}

BOOL AgcmUICharacter::SetTargetCharacter(AgpdCharacter *pcsTargetCharacter)
{
	if (!pcsTargetCharacter)
		return FALSE;

	if (m_pcsAgcmCharacter->GetSelfCharacter() == pcsTargetCharacter)
		return TRUE;

	if( !pcsTargetCharacter->m_bNPCDisplayForNameBoard )
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseTargetInfo);
		return TRUE;
	}

	m_pcsTargetCharacter	= pcsTargetCharacter;

	RefreshUserDataTargetCharacter();

	m_pcsAgcmUIManager2->ThrowEvent(m_lEventRefreshTargetName);
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseTargetAttrInfo);
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseTargetUserMenu);
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseTargetSummonsMenu);
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseTargetSummonsMenu2);

	AgpdCharacter *pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	// 2005.09.25. steeple
	BOOL bMySummons = m_pcsAgpmSummons->IsMySummons(pcsSelfCharacter, pcsTargetCharacter->m_lID);
	if (bMySummons)
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenTargetSummonsMenu);

	ResetTargetBuffedSkill();

	// if dead, set applicable product skill icon
	if (AGPDCHAR_STATUS_DEAD == pcsTargetCharacter->m_unActionStatus)
	{
		if (m_pcsAgpmProduct)
		{
			INT32 lClass = (INT32) AUCHARCLASS_TYPE_NONE;
			m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);
			
			if (lClass > AUCHARCLASS_TYPE_NONE && lClass < AUCHARCLASS_TYPE_MAX)
			{
				AgpdGatherCharacterTAD *pAgpdGatherCharacterTAD = 
					m_pcsAgpmProduct->GetGatherCharacterTAD(pcsTargetCharacter->m_pcsCharacterTemplate);
				if (pAgpdGatherCharacterTAD)
				{
					AgpdSkillTemplate *pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pAgpdGatherCharacterTAD->m_ClassElement[lClass].m_lSkillTID);
					if (pcsSkillTemplate)
					{
						// Self 에게만 Effect 가 보여야 하는 스킬인데, 타겟이왔다. 2005.12.20. steeple
						if (!m_pcsAgpmSkill->IsVisibleEffectTypeSelfOnly(pcsSkillTemplate) || pcsSelfCharacter->m_lID == pcsTargetCharacter->m_lID)
						{
							m_pcsAgcmSkill->SetGridSkillAttachedSmallTexture(pcsSkillTemplate);
						
							AddTargetBuffedSkill(pcsSkillTemplate->m_pcsGridItemAlarm);					
						}
					}
				}
			}
		}
	}
	else
	{
		AgpdSkillAttachData	*pcsSkillAttachData	= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsTargetCharacter);
		if (pcsSkillAttachData)
		{
			for (int i = 0; i < AGPMSKILL_MAX_SKILL_BUFF; ++i)
			{
				AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(pcsSkillAttachData->m_astBuffSkillList[i].lSkillTID);
				if( !pcsSkillTemplate )		break;

				// Self 에게만 Effect 가 보여야 하는 스킬인데, 타겟이왔다. 2005.12.20. steeple
				if(m_pcsAgpmSkill->IsVisibleEffectTypeSelfOnly(pcsSkillTemplate) &&
					pcsSelfCharacter->m_lID != pcsTargetCharacter->m_lID)
					continue;

				m_pcsAgcmSkill->SetGridSkillAttachedSmallTexture(pcsSkillTemplate);
				
				AddTargetBuffedSkill(pcsSkillTemplate->m_pcsGridItemAlarm);
			}
		}
	}

	AgpdCharacterTemplate	*pcsTemplate	= pcsTargetCharacter->m_pcsCharacterTemplate;

	m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenTargetInfo);
	EnumCallback(AGCMUICHAR_CB_SET_TARGET_CHARACTER, m_pcsTargetCharacter, NULL);

	// 플레이어 캐릭터의 경우 종족 아이콘과 클래스 아이콘을 넣어준다. 2008. 09. 01. UI 리뉴얼
	if( m_pcsAgpmCharacter->IsPC( pcsTargetCharacter ) )
	{
		_UpdateTargetPlayerRaceAndClass( pcsTargetCharacter );

		 //캐릭터고 선공, 악당이면 속성무시하고 뿌려준다.	배틀그라운드가 아닐때만
		if( !m_pcsAgcmBattleGround->IsOtherRace( pcsTargetCharacter ) )
		{
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenTargetUserMenu);
			
			INT32 lMurdererLevel = m_pcsAgpmCharacter->GetMurdererLevel(pcsTargetCharacter);
			if (AGPMCHAR_MURDERER_LEVEL3_POINT == lMurdererLevel)
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetPKMurdererLevel3);
			else if (AGPMCHAR_MURDERER_LEVEL2_POINT == lMurdererLevel)
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetPKMurdererLevel2);
			else if (AGPMCHAR_MURDERER_LEVEL1_POINT == lMurdererLevel)
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetPKMurdererLevel1);
			else if (AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED == pcsTargetCharacter->m_unCriminalStatus )
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetPKCriminal);
		}
	}
	else
	{
		if (bMySummons == FALSE )
		{
			// 속성을 알아온다.
			INT32	lAttrValue	= 0;
			int i;
			for (i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_AIR; ++i)
			{
				lAttrValue	= 0;
				m_pcsAgpmFactors->GetValue(&pcsTemplate->m_csFactor, &lAttrValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, i);
				if (lAttrValue > 0)
					break;
			}

			if (lAttrValue > 0)
			{
				m_lTargetAttributeType	= i;

				switch (i) {
				case AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrMagic);
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenTargetAttrInfo);
					break;

				case AGPD_FACTORS_ATTRIBUTE_TYPE_WATER:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrWater);
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenTargetAttrInfo);
					break;

				case AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrFire);
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenTargetAttrInfo);
					break;

				case AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrEarth);
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenTargetAttrInfo);
					break;

				case AGPD_FACTORS_ATTRIBUTE_TYPE_AIR:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrAir);
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenTargetAttrInfo);
					break;

				case AGPD_FACTORS_ATTRIBUTE_TYPE_POISON:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrPoison);
					break;

				case AGPD_FACTORS_ATTRIBUTE_TYPE_ICE:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrIce);
					break;

				case AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrLightening);
					break;

				default:
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrNone);
					break;
				}
			}
			else
			{
				switch (m_pcsTargetCharacter->m_pcsCharacterTemplate->m_eAttributeType) {
				case AGPD_FACTORS_ATTRIBUTE_TYPE_POISON:
					m_lTargetAttributeType	= AGPD_FACTORS_ATTRIBUTE_TYPE_POISON;	 
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrPoison);
					break;

				case AGPD_FACTORS_ATTRIBUTE_TYPE_ICE:
					m_lTargetAttributeType	= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE;
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrIce);
					break;

				case AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING:
					m_lTargetAttributeType	= AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING;
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrLightening);
					break;

				default:
					m_lTargetAttributeType	= AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL;
					m_pcsAgcmUIManager2->ThrowEvent(m_lEventTargetAttrNone);
					break;
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmUICharacter::ResetTargetCharacter()
{
	ResetTargetBuffedSkill();

	m_pcsTargetCharacter	= NULL;

	m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseTargetInfo);
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseTargetAttrInfo);
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseTargetUserMenu);
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseTargetSummonsMenu);
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseTargetSummonsMenu2);

	EnumCallback(AGCMUICHAR_CB_RESET_TARGET_CHARACTER, m_pcsTargetCharacter, NULL);

	return TRUE;
}

BOOL AgcmUICharacter::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)			pData;

	if (pThis->m_bIsOpenedCharacterMenu)
		pThis->CloseCharacterMenu();

	return TRUE;
}

BOOL AgcmUICharacter::CBUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)			pData;
	AgcdUICharacterADBase	*pstAttachedData = pThis->GetBaseData(pcsCharacter);

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (pcsSelfCharacter && pcsSelfCharacter->m_lID == pcsCharacter->m_lID)
	{
		if (pThis->m_bIsOpenedTargetAttrInfo ||
			pThis->m_bIsOpenedTargetUserMenu ||
			pThis->m_bIsOpenedTargetSummonsMenu)
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseTargetWindow);
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseTargetSummonsMenu);
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseTargetSummonsMenu2);

			pThis->m_csTargetAttrTooltip.ShowWindow(FALSE);
			pThis->m_csTargetAttrTooltip.DeleteAllStringInfo();

			pThis->m_csTargetNameToolTip.ShowWindow(FALSE);
			pThis->m_csTargetNameToolTip.DeleteAllStatusInfo();

			pThis->m_bIsOpenedTargetAttrInfo	= FALSE;
			pThis->m_bIsOpenedTargetUserMenu	= FALSE;
			pThis->m_bIsOpenedTargetSummonsMenu	= FALSE;
		}

		if (pThis->m_pcsTargetCharacter && pThis->m_pcsTargetCharacter->m_lID == pcsCharacter->m_lID)
		{
			if (pThis->m_bIsOpenedCharacterMenu)
				pThis->CloseCharacterMenu();
		}
	}

	if (pstAttachedData->m_lMinimapIndex >= 0 && pThis->m_pcsAgcmMinimap)
	{
		pThis->m_pcsAgcmMinimap->UpdatePointPosition(pstAttachedData->m_lMinimapIndex, &pcsCharacter->m_stPos);
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)			pData;
	AgcdUICharacterADBase	*pstAttachedData = pThis->GetBaseData(pcsCharacter);

	if (pThis->m_pcsAgpmCharacter->IsNPC(pcsCharacter))
	{
		pThis->AddBaseMinimapType(pcsCharacter, AgcmMinimap::MPInfo::MPI_NPC);
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)			pData;
	AgcdUICharacterADBase	*pstAttachedData = pThis->GetBaseData(pcsCharacter);

	if (pThis->m_pcsTargetCharacter &&
		pThis->m_pcsTargetCharacter->m_lID == pcsCharacter->m_lID)
	{
		if (pThis->m_bIsOpenedCharacterMenu)
			pThis->CloseCharacterMenu();

		pThis->ResetTargetCharacter();
	}

	pThis->RemoveBaseMinimapType(pcsCharacter);

	return TRUE;
}

BOOL AgcmUICharacter::CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)			pData;

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return TRUE;

	if (pcsSelfCharacter == pcsCharacter)
	{
		if (pcsSelfCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		{
			pThis->EnumCallback(AGCMUICHAR_CB_UPDATE_RESURRECT_STATUS, pcsSelfCharacter, NULL);

			pThis->m_bSelfCharacterDead	= TRUE;
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenResurrectionUI);

			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUserDataSelfCharacterDead);
		}
		else if (pThis->m_bSelfCharacterDead)
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseResurrectionUI);

			pThis->m_bSelfCharacterDead	= FALSE;

			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUserDataSelfCharacterDead);
		}

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_SELF_CHAR]);
	}
	else if (AP_INVALID_CID != pThis->m_pcsAgcmCharacter->GetSelectTarget(pcsSelfCharacter) &&
			 pcsCharacter->m_lID == pThis->m_pcsAgcmCharacter->GetSelectTarget(pcsSelfCharacter))
	{
		if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		{
			pThis->SetTargetCharacter(pcsCharacter);
		}
	}

	return TRUE;
}

BOOL AgcmUICharacter::OpenCharacterMenu(ApBase *pcsBase)
{
	if( !pcsBase || pcsBase->m_eType != APBASE_TYPE_CHARACTER )		return FALSE;
	if( !m_pcsAgpmCharacter->IsPC((AgpdCharacter *) pcsBase) )		return FALSE;
		
	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsSelfCharacter )			return FALSE;

	// 정해진 거리 이내에 들어왔는지 검사한다.
	if( !m_pcsAgpmFactors->IsInRange( &pcsSelfCharacter->m_stPos, &((AgpdCharacter *) pcsBase)->m_stPos, AGCM_UI_CHARACTER_OPEN_CHARACTER_MENU_RANGE, 0, NULL ) )
		return FALSE;

	if( m_pcsAgcmBattleGround->IsBattleGournd( pcsSelfCharacter ) )	return FALSE;

	if( m_pcsApmMap->CheckRegionPerculiarity( pcsSelfCharacter->m_nBindingRegionIndex , APMMAP_PECULIARITY_SHOWNAME ) == APMMAP_PECULIARITY_RETURN_DISABLE_USE )
		return FALSE;

	SetTargetCharacter((AgpdCharacter *) pcsBase);

	RefreshUserDataTargetCharacter();

	m_bIsOpenedCharacterMenu	= TRUE;

	return m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenCharacterMenu);
}

BOOL AgcmUICharacter::CloseCharacterMenu()
{
	if (!m_bIsOpenedCharacterMenu)
		return TRUE;

	m_bIsOpenedCharacterMenu	= FALSE;

	return m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseCharacterMenu);
}

BOOL AgcmUICharacter::ResetDisplayCharacterStatus(CHAR *szDisplay, INT32 *plValue)
{
	if (!szDisplay)
		return FALSE;

	sprintf(szDisplay, "");

	return TRUE;
}

BOOL AgcmUICharacter::SetCallbackUpdateResurrectStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUICHAR_CB_UPDATE_RESURRECT_STATUS, pfCallback, pClass);
}

BOOL AgcmUICharacter::SetCallbackSetTargetCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUICHAR_CB_SET_TARGET_CHARACTER, pfCallback, pClass);
}

BOOL AgcmUICharacter::SetCallbackResetTargetCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUICHAR_CB_RESET_TARGET_CHARACTER, pfCallback, pClass);
}

BOOL AgcmUICharacter::SetTransformDurationMSec(UINT32 ulTransformDurationMSec)
{
	m_ulTransformEndTimeMSec	= ulTransformDurationMSec + GetClockCount();

	INT32	lRemainTimeMinute	= ulTransformDurationMSec / 60 / 1000;

	CHAR	szBuffer[64];
	ZeroMemory(szBuffer, sizeof(CHAR) * 64);

	sprintf(szBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TRANSFORM_REMAIN_NOTICE), lRemainTimeMinute);

	m_pcsAgcmChatting2->AddSystemMessage(szBuffer);

	m_ulTransformDurationMSec	= ulTransformDurationMSec;
	m_ulTransformNextNoticeMSec	= GetClockCount() + AGCM_UI_CHARACTER_TRANSFORM_NOTICE_INTERVAL;

	return TRUE;
}

BOOL AgcmUICharacter::CBRefreshTargetName(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	AgcdUIControl		*pcsControl			= (AgcdUIControl *)		pData1;

	if (!pThis->m_pcsTargetCharacter)
		return FALSE;

	AcUIEdit			*pcsEditControl		= (AcUIEdit *)			pcsControl->m_pcsBase;

	CHAR	szBuffer[128];
	ZeroMemory(szBuffer, sizeof(CHAR) * 128);

	DWORD	dwLevelColor = 0xffffff;

	//. 2005. 10. 19. Nonstopdj
	//. Monster와 PC의 Target ID, Level color조정
	//. self character의 레벨을 기준으로 target 창에 표시되는 색상 변경
	INT32	lSelfLevel		= pThis->m_pcsAgpmCharacter->GetLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter());
	INT32	lTargetLevel	= pThis->m_pcsAgpmCharacter->GetLevel(pThis->m_pcsTargetCharacter);

	INT32	diff = lTargetLevel - lSelfLevel;
	BOOL	bDisplayLevel = FALSE;

	CHAR	szCharacterName[128];

	if( pThis->m_pcsAgcmBattleGround->IsOtherRace( pThis->m_pcsTargetCharacter ) || 
		pThis->m_pcsApmMap->CheckRegionPerculiarity(  pThis->m_pcsTargetCharacter->m_nBindingRegionIndex , APMMAP_PECULIARITY_SHOWNAME ) == APMMAP_PECULIARITY_RETURN_DISABLE_USE )
	{
		memset( szCharacterName, 0, 128 );
		for( int i=0; i<strlen(pThis->m_pcsTargetCharacter->m_szID); ++i )
			szCharacterName[i] = '*';
		szCharacterName[strlen(pThis->m_pcsTargetCharacter->m_szID)] = NULL;
	}
	else
	{
		if(pThis->m_pcsAgpmCharacter->IsMonster(pThis->m_pcsTargetCharacter) || pThis->m_pcsAgpmCharacter->IsPC(pThis->m_pcsTargetCharacter) )
		{
			if(pThis->m_pcsAgpmCharacter->IsMonster(pThis->m_pcsTargetCharacter) || g_eServiceArea == AP_SERVICE_AREA_WESTERN)
				bDisplayLevel = TRUE;
		}

		if (pThis->m_pcsTargetCharacter->m_szID && pThis->m_pcsTargetCharacter->m_szID[0])
			strcpy(szCharacterName, pThis->m_pcsTargetCharacter->m_szID);
		else
		{
			if (pThis->m_pcsTargetCharacter->m_lFixTID != pThis->m_pcsTargetCharacter->m_pcsCharacterTemplate->m_lID &&
				pThis->m_pcsAgpmCharacter->IsPolyMorph(pThis->m_pcsTargetCharacter))
			{
				AgpdCharacterTemplate	*pcsOriginalTemplate	= pThis->m_pcsAgpmCharacter->GetCharacterTemplate(pThis->m_pcsTargetCharacter->m_lFixTID);
				if (pcsOriginalTemplate)
					strcpy(szCharacterName, pcsOriginalTemplate->m_szTName);
				else
					strcpy(szCharacterName, pThis->m_pcsTargetCharacter->m_pcsCharacterTemplate->m_szTName);
			}
			else
				strcpy(szCharacterName, pThis->m_pcsTargetCharacter->m_pcsCharacterTemplate->m_szTName);
		}

		if (pcsEditControl)
		{
			CHAR	szTemp[128];
			INT32	lLevelWidth = 0;

			if (bDisplayLevel)
			{
				sprintf(szTemp, "  Lv%d ", lTargetLevel);
				lLevelWidth = ( INT32 ) ( pcsEditControl->m_pAgcmFont->GetTextExtent(pcsEditControl->m_lFontType, szTemp, strlen(szTemp) ) * pcsEditControl->m_fScale );
			}

			pcsEditControl->MakeStringFit(szCharacterName, pcsEditControl->w - lLevelWidth);
		}

		if (pThis->m_pcsAgpmCharacter->IsMonster(pThis->m_pcsTargetCharacter) || pThis->m_pcsAgpmCharacter->IsPC(pThis->m_pcsTargetCharacter) )
		{
			if( diff >= -3 && diff <= 3)
			{
				dwLevelColor = 0xffffff;	//. 기본 흰색
			}
			else if( diff >= 4)
			{
				dwLevelColor = 0xff8000;	//. 주황
			}
			else if( diff <= -4)			
			{
				dwLevelColor = 0x00ff00;	//. 연두
			}
		}
	}

	//. Level color와 ID color를 동일하게 사용
	sprintf(szBuffer, "<C%d>%s", dwLevelColor, szCharacterName);

	if(bDisplayLevel)
		//. '[Lv ??]'에서 '[', ']'를 제외시킴 
		sprintf(szBuffer + strlen(szBuffer), " <C16777215> <C%d>Lv%d<C16777215> ", dwLevelColor, lTargetLevel);

	pcsEditControl->SetText(szBuffer);

	return TRUE;
}

BOOL AgcmUICharacter::CBToggleTargetUserMenu(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1 || !pcsSourceControl)
		return FALSE;

	if (((ApBase *) pData1)->m_eType != APBASE_TYPE_UI)
		return FALSE;

	AgcmUICharacter		*pThis			= (AgcmUICharacter *)	pClass;

	AgcdUI				*pcsTargetUI	= (AgcdUI *)		pData1;

	pThis->m_bIsOpenedTargetUserMenu	= pcsTargetUI->m_pcsUIWindow->m_bOpened;

	return TRUE;
}

BOOL AgcmUICharacter::CBToggleTargetAttrTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICharacter		*pThis			= (AgcmUICharacter *)	pClass;

	if (pThis->m_bIsOpenedTargetAttrInfo)
	{
		pThis->m_csTargetAttrTooltip.ShowWindow(FALSE);
		pThis->m_csTargetAttrTooltip.DeleteAllStringInfo();

		pThis->m_bIsOpenedTargetAttrInfo	= FALSE;
	}
	else
	{
		if (!pcsSourceControl->m_pcsParentUI || !pcsSourceControl->m_pcsParentUI->m_pcsUIWindow)
			return FALSE;
		
		pThis->m_csTargetAttrTooltip.MoveWindow(pcsSourceControl->m_pcsParentUI->m_pcsUIWindow->x,
												pcsSourceControl->m_pcsParentUI->m_pcsUIWindow->y + 31,
												pThis->m_csTargetAttrTooltip.w,
												pThis->m_csTargetAttrTooltip.h);

		pThis->SetTargetAttrToolTipInfo();

		pThis->m_csTargetAttrTooltip.ShowWindow( TRUE );

		pThis->m_bIsOpenedTargetAttrInfo	= TRUE;
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBCloseTargetAttrTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter		*pThis			= (AgcmUICharacter *)	pClass;

	pThis->m_csTargetAttrTooltip.ShowWindow(FALSE);
	pThis->m_csTargetAttrTooltip.DeleteAllStringInfo();

	pThis->m_bIsOpenedTargetAttrInfo		= FALSE;

	return TRUE;
}

// 2005.09.25. steeple
BOOL AgcmUICharacter::CBToggleTargetSummonsMenu(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUICharacter* pThis = static_cast<AgcmUICharacter*>(pClass);

	if(!pThis->m_pcsAgpmSummons)
		return TRUE;

	if(!pThis->m_pcsTargetCharacter)
		return FALSE;

	if(pThis->m_pcsAgpmCharacter->IsStatusSummoner(pThis->m_pcsTargetCharacter) == FALSE &&
		pThis->m_pcsAgpmCharacter->IsStatusTame(pThis->m_pcsTargetCharacter) == FALSE)
		return TRUE;

	if(pThis->m_pcsAgpmSummons->IsMySummons(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsTargetCharacter->m_lID) == FALSE)
		return TRUE;

    pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUserDataSummonsPropensity);
	return TRUE;
}

// 2005.09.25. steeple
BOOL AgcmUICharacter::CBDisplaySummonsPropensity(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUICharacter* pThis = static_cast<AgcmUICharacter*>(pClass);

	if(!pThis->m_pcsAgpmSummons)
		return TRUE;

	if(!pThis->m_pcsTargetCharacter)
		return FALSE;

	// 2006.11.16. steeple
	// 클라이언트에서는 Teleport 등의 이동 후에는 Target 의 정보가 정확하지가 않다.
	// 그래서 항상 주인의 것에서 검색해서 받아온다.
	AgpdSummonsADChar* pcsSummonsADChar = pThis->m_pcsAgpmSummons->GetADCharacter(pThis->m_pcsAgcmCharacter->GetSelfCharacter());
	if(!pcsSummonsADChar)
		return FALSE;

	AgpdSummonsArray::iterator iter = std::find(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
												pcsSummonsADChar->m_SummonsArray.m_pSummons->end(),
												pThis->m_pcsTargetCharacter->m_lID);
	if(iter == pcsSummonsADChar->m_SummonsArray.m_pSummons->end())
		return FALSE;

	CHAR* szPropensity = NULL;

	switch(iter->m_ePropensity)
	{
		case AGPMSUMMONS_PROPENSITY_ATTACK:
			szPropensity = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SUMMONS_PROPENSITY_ATTACK);
			break;

		case AGPMSUMMONS_PROPENSITY_DEFENSE:
			szPropensity = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SUMMONS_PROPENSITY_DEFENSE);
			break;

		case AGPMSUMMONS_PROPENSITY_SHADOW:
			szPropensity = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SUMMONS_PROPENSITY_SHADOW);
			break;
	}

	if(szPropensity)
		strcpy(szDisplay, szPropensity);
	else
		szDisplay[0] = '\0';

	return TRUE;
}

BOOL AgcmUICharacter::CBDisplaySiegeResurrection(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pClass)
		return FALSE;
	
	AgcmUICharacter* pThis = static_cast<AgcmUICharacter*>(pClass);
	
	AgpdCharacter *pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (pThis->m_pcsAgpmSiegeWar && pcsSelfCharacter)
	{
		AgpdSiegeWar *pcsSiegeWar = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsSelfCharacter);
		if (pcsSiegeWar)
			sprintf(szDisplay, "%s", pcsSiegeWar->m_strCastleName.GetBuffer());
	}

	return TRUE;
}

// 2005.09.25. steeple
BOOL AgcmUICharacter::CBSummonsPropensityAttackClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUICharacter* pThis = static_cast<AgcmUICharacter*>(pClass);

	if(!pThis->m_pcsAgpmSummons)
		return FALSE;

	if(!pThis->m_pcsTargetCharacter || pThis->m_pcsAgpmCharacter->IsStatusSummoner(pThis->m_pcsTargetCharacter) == FALSE)
		return FALSE;

	return pThis->SendSummonsSetPropenstiy(pThis->m_pcsTargetCharacter->m_lID, AGPMSUMMONS_PROPENSITY_ATTACK);
}

// 2005.09.26. steeple
BOOL AgcmUICharacter::CBSummonsPropensityDefenseClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUICharacter* pThis = static_cast<AgcmUICharacter*>(pClass);

	if(!pThis->m_pcsAgpmSummons)
		return FALSE;

	if(!pThis->m_pcsTargetCharacter || pThis->m_pcsAgpmCharacter->IsStatusSummoner(pThis->m_pcsTargetCharacter) == FALSE)
		return FALSE;

	return pThis->SendSummonsSetPropenstiy(pThis->m_pcsTargetCharacter->m_lID, AGPMSUMMONS_PROPENSITY_DEFENSE);
}

// 2005.09.26. steeple
BOOL AgcmUICharacter::CBSummonsPropensityShadowClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUICharacter* pThis = static_cast<AgcmUICharacter*>(pClass);

	if(!pThis->m_pcsAgpmSummons)
		return FALSE;

	if(!pThis->m_pcsTargetCharacter || pThis->m_pcsAgpmCharacter->IsStatusSummoner(pThis->m_pcsTargetCharacter) == FALSE)
		return FALSE;

	return pThis->SendSummonsSetPropenstiy(pThis->m_pcsTargetCharacter->m_lID, AGPMSUMMONS_PROPENSITY_SHADOW);
}

// 2005.09.26. steeple
BOOL AgcmUICharacter::SendSummonsSetPropenstiy(INT32 lSummonsCID, EnumAgpdSummonsPropensity eSummonsPropensity)
{
	if(lSummonsCID < 1 || eSummonsPropensity < 0)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	// 서버로 패킷을 보낸다.
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmSummons->MakeSetPropensityPacket(&nPacketLength,
																pcsSelfCharacter->m_lID, 
																lSummonsCID,
																(INT8)eSummonsPropensity);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	return bResult;
}

// 2005.09.26. steeple
BOOL AgcmUICharacter::CBReceiveSummonsSetPropensity(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsOwner = static_cast<AgpdCharacter*>(pData);
	AgcmUICharacter* pThis = static_cast<AgcmUICharacter*>(pClass);
	AgpdCharacter* pcsSummons = static_cast<AgpdCharacter*>(pCustData);

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return TRUE;

	if(pcsOwner->m_lID == pcsSelfCharacter->m_lID)
	{
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUserDataSummonsPropensity);
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBToggleExpType(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter		*pThis			= (AgcmUICharacter *)	pClass;

	if (pThis->m_bIsExpTypePercent)
		pThis->m_bIsExpTypePercent	= FALSE;
	else
		pThis->m_bIsExpTypePercent	= TRUE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pastAgcdUIUserData[E_AGCD_UI_CHAR_USRDAT_SELF_CHAR]);

	pThis->RefreshCharacterStatus();

	return TRUE;
}

BOOL AgcmUICharacter::SetTargetAttrToolTipInfo()
{
	if (!m_pcsTargetCharacter)
		return FALSE;

	AgpdCharacterTemplate	*pcsTemplate	= m_pcsTargetCharacter->m_pcsCharacterTemplate;

	// 속성을 알아온다.

	INT32	lToolTipStartLine	= m_csTargetAttrTooltip.GetLinePos();

	INT32	lAttrValue	= 0;

	int i;
	for (i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_AIR; ++i)
	{
		lAttrValue	= 0;

		m_pcsAgpmFactors->GetValue(&pcsTemplate->m_csFactor, &lAttrValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, i);

		if (lAttrValue > 0)
		{
			CHAR	szBuffer[128];
			ZeroMemory(szBuffer, sizeof(CHAR) * 128);

			switch (i) {
			case AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC:
				MAKE_ATTR_ATTACK_STR(szBuffer, MAGIC);
				m_csTargetAttrTooltip.AddString(szBuffer);
				m_csTargetAttrTooltip.AddNewLine(14);
				break;

			case AGPD_FACTORS_ATTRIBUTE_TYPE_WATER:
				MAKE_ATTR_ATTACK_STR(szBuffer, WATER);
				m_csTargetAttrTooltip.AddString(szBuffer);
				m_csTargetAttrTooltip.AddNewLine(14);
				break;

			case AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE:
				MAKE_ATTR_ATTACK_STR(szBuffer, FIRE);
				m_csTargetAttrTooltip.AddString(szBuffer);
				m_csTargetAttrTooltip.AddNewLine(14);
				break;

			case AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH:
				MAKE_ATTR_ATTACK_STR(szBuffer, EARTH);
				m_csTargetAttrTooltip.AddString(szBuffer);
				m_csTargetAttrTooltip.AddNewLine(14);
				break;

			case AGPD_FACTORS_ATTRIBUTE_TYPE_AIR:
				MAKE_ATTR_ATTACK_STR(szBuffer, AIR);
				m_csTargetAttrTooltip.AddString(szBuffer);
				m_csTargetAttrTooltip.AddNewLine(14);
				break;
			}
		}
	}

	m_csTargetAttrTooltip.SetLinePos(lToolTipStartLine);

	for (i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_AIR; ++i)
	{
		lAttrValue	= 0;

		m_pcsAgpmFactors->GetValue(&pcsTemplate->m_csFactor, &lAttrValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, i);

		if (lAttrValue > 0)
		{
			CHAR	szBuffer[128];
			ZeroMemory(szBuffer, sizeof(CHAR) * 128);

			switch (i) {
			case AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC:
				MAKE_ATTR_RESI_STR(szBuffer, MAGIC);
				m_csTargetAttrTooltip.AddString(szBuffer, 120);
				m_csTargetAttrTooltip.AddNewLine(14);
				break;

			case AGPD_FACTORS_ATTRIBUTE_TYPE_WATER:
				MAKE_ATTR_RESI_STR(szBuffer, WATER);
				m_csTargetAttrTooltip.AddString(szBuffer, 120);
				m_csTargetAttrTooltip.AddNewLine(14);
				break;

			case AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE:
				MAKE_ATTR_RESI_STR(szBuffer, FIRE);
				m_csTargetAttrTooltip.AddString(szBuffer, 120);
				m_csTargetAttrTooltip.AddNewLine(14);
				break;

			case AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH:
				MAKE_ATTR_RESI_STR(szBuffer, EARTH);
				m_csTargetAttrTooltip.AddString(szBuffer, 120);
				m_csTargetAttrTooltip.AddNewLine(14);
				break;

			case AGPD_FACTORS_ATTRIBUTE_TYPE_AIR:
				MAKE_ATTR_RESI_STR(szBuffer, AIR);
				m_csTargetAttrTooltip.AddString(szBuffer, 120);
				m_csTargetAttrTooltip.AddNewLine(14);
				break;
			}
		}
	}

	return TRUE;
}

BOOL AgcmUICharacter::SetTargetSummonsToolTipInfo()
{
	if (!m_pcsTargetCharacter)
		return FALSE;

	// 2005.09.25. steeple
	if(m_pcsAgpmCharacter->IsStatusSummoner(m_pcsTargetCharacter) == FALSE)
		return FALSE;



	return TRUE;
}

BOOL AgcmUICharacter::AddTargetBuffedSkill(AgpdGridItem *pcsGridItem)
{
	if (!pcsGridItem)
		return FALSE;

	if (!m_pcsTargetCharacter)
		return FALSE;

	if (!m_pcsAgpmGrid->AddItem(&m_csTargetBuffGrid, pcsGridItem))
		return FALSE;

	if (!m_pcsAgpmGrid->Add(&m_csTargetBuffGrid, pcsGridItem, 1, 1))
		return FALSE;

	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUserDataTargetBuffGrid);
}

BOOL AgcmUICharacter::RemoveTargetBuffedSkill(AgpdGridItem *pcsGridItem)
{
	if (!pcsGridItem)
		return FALSE;

	m_pcsAgpmGrid->RemoveItemAndFillFirst(&m_csTargetBuffGrid, pcsGridItem);

	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUserDataTargetBuffGrid);
}

BOOL AgcmUICharacter::ResetTargetBuffedSkill()
{
	m_pcsAgpmGrid->Reset(&m_csTargetBuffGrid);

	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUserDataTargetBuffGrid);
}

BOOL AgcmUICharacter::CBAddBuffedSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	ApBase				*pcsBase			= (ApBase *)			pData;
	PVOID				*ppvBuffer			= (PVOID *)				pCustData;

	INT32				lBuffedTID			= (INT32)				ppvBuffer[0];
	INT32				lCasterTID			= (INT32)				ppvBuffer[1];
	BOOL				bIsOnlyUpdate		= (BOOL)				ppvBuffer[2];

	if (!pThis->m_pcsTargetCharacter ||
		pThis->m_pcsTargetCharacter != pcsBase)
		return TRUE;

	if (bIsOnlyUpdate)
		return TRUE;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return TRUE;

	AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(lBuffedTID);
	if (pcsSkillTemplate)
	{
		// Self 에게만 Effect 가 보여야 하는 스킬인데, 타겟이왔다. 2005.12.20. steeple
		if(pThis->m_pcsAgpmSkill->IsVisibleEffectTypeSelfOnly(pcsSkillTemplate) &&
			pcsSelfCharacter->m_lID != pcsBase->m_lID)
			return TRUE;

		pThis->m_pcsAgcmSkill->SetGridSkillAttachedSmallTexture(pcsSkillTemplate);

		pThis->AddTargetBuffedSkill(pcsSkillTemplate->m_pcsGridItemAlarm);
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBRemoveBuffedSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	ApBase				*pcsBase			= (ApBase *)			pData;
	INT32				lBuffedTID			= (INT32)				pCustData;

	if (!pThis->m_pcsTargetCharacter ||
		pThis->m_pcsTargetCharacter != pcsBase)
		return TRUE;

	AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(lBuffedTID);
	if (pcsSkillTemplate)
	{
		pThis->RemoveTargetBuffedSkill(pcsSkillTemplate->m_pcsGridItemAlarm);
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBRideAck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	return ((AgcmUICharacter *) pClass)->RefreshCharacterStatus();
}

BOOL AgcmUICharacter::SendRequestResurrectionToTown()
{
	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_TOWN;
	INT16	nPacketLength	= 0;

	PVOID	pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,								// Operation
																&pcsSelfCharacter->m_lID,					// Character ID
																NULL,										// Character Template ID
																NULL,										// Game ID
																NULL,										// Character Status
																NULL,										// Move Packet
																NULL,										// Action Packet
																NULL,										// Factor Packet
																NULL,										// llMoney
																NULL,										// bank money
																NULL,										// cash
																NULL,										// character action status
																NULL,										// character criminal status
																NULL,										// attacker id (정당방위 설정에 필요)
																NULL,										// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,										// region index
																NULL,										// social action index
																NULL,										// special status
																NULL,										// is transform status
																NULL,										// skill initialization text
																NULL,										// face index
																NULL,										// hair index
																NULL,										// Option Flag
																NULL,										// bank size
																NULL,										// event status flag
																NULL,										// remained criminal status time
																NULL,										// remained murderer point time
																NULL,										// nick name
																NULL,										// gameguard
																NULL										// last killed time in battlesquare
																);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmUICharacter::SendRequestResurrectionNow()
{
	return TRUE;

	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_NOW;
	INT16	nPacketLength	= 0;

	PVOID	pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,						// Operation
																&pcsSelfCharacter->m_lID,			// Character ID
																NULL,								// Character Template ID
																NULL,								// Game ID
																NULL,								// Character Status
																NULL,								// Move Packet
																NULL,								// Action Packet
																NULL,								// Factor Packet
																NULL,								// llMoney
																NULL,								// bank money
																NULL,								// cash
																NULL,								// character action status
																NULL,								// character criminal status
																NULL,								// attacker id (정당방위 설정에 필요)
																NULL,								// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,								// region index
																NULL,								// social action index
																NULL,								// special status
																NULL,								// is transform status
																NULL,								// skill initialization text
																NULL,								// face index
																NULL,								// hair index
																NULL,								// Option Flag
																NULL,								// bank size
																NULL,								// event status flag
																NULL,								// remained criminal status time
																NULL,								// remained murderer point time
																NULL,								// nick name
																NULL,								// gameguard
																NULL								// last killed time in battlesquare
																);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmUICharacter::SendRequestResurrectionSiege(BOOL bIsInner)
{
	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	INT8	cOperation;
	
	if (bIsInner)
		cOperation	= AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_SIEGE_INNER;
	else
		cOperation	= AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_SIEGE_OUTER;

	INT16	nPacketLength	= 0;

	PVOID	pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,						// Operation
																&pcsSelfCharacter->m_lID,			// Character ID
																NULL,								// Character Template ID
																NULL,								// Game ID
																NULL,								// Character Status
																NULL,								// Move Packet
																NULL,								// Action Packet
																NULL,								// Factor Packet
																NULL,								// llMoney
																NULL,								// bank money
																NULL,								// cash
																NULL,								// character action status
																NULL,								// character criminal status
																NULL,								// attacker id (정당방위 설정에 필요)
																NULL,								// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,								// region index
																NULL,								// social action index
																NULL,								// special status
																NULL,								// is transform status
																NULL,								// skill initialization text
																NULL,								// face index
																NULL,								// hair index
																NULL,								// Option Flag
																NULL,								// bank size
																NULL,								// event status flag
																NULL,								// remained criminal status time
																NULL,								// remained murderer point time
																NULL,								// nick name
																NULL,								// gameguard
																NULL								// last killed time in battlesquare
																);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

// 2007.08.06. steeple
BOOL AgcmUICharacter::SendRequestResurrectionByOther(CHAR* szName)
{
	if(!szName || _tcslen(szName) == 0)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	INT32 lFlag = 1;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_RESURRECTION_BY_OTHER;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,						// Operation
																&pcsSelfCharacter->m_lID,			// Character ID
																NULL,								// Character Template ID
																NULL,								// Game ID
																NULL,								// Character Status
																NULL,								// Move Packet
																NULL,								// Action Packet
																NULL,								// Factor Packet
																NULL,								// llMoney
																NULL,								// bank money
																NULL,								// cash
																NULL,								// character action status
																NULL,								// character criminal status
																NULL,								// attacker id (정당방위 설정에 필요)
																NULL,								// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,								// region index
																NULL,								// social action index
																NULL,								// special status
																NULL,								// is transform status
																NULL,								// skill initialization text
																NULL,								// face index
																NULL,								// hair index
																&lFlag,								// Option Flag
																NULL,								// bank size
																NULL,								// event status flag
																NULL,								// remained criminal status time
																NULL,								// remained murderer point time
																szName,								// nick name
																NULL,								// gameguard
																NULL								// last killed time in battlesquare
																);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bSendResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
	return	bSendResult;
}

BOOL AgcmUICharacter::CBResurrectionToTown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter	*pThis	= (AgcmUICharacter *)	pClass;

	return pThis->SendRequestResurrectionToTown();
}

BOOL AgcmUICharacter::CBResurrectionNow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter	*pThis	= (AgcmUICharacter *)	pClass;

	return pThis->SendRequestResurrectionNow();
}

BOOL AgcmUICharacter::CBResurrectionSiege(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter	*pThis	= (AgcmUICharacter *)	pClass;

	if (pThis->m_pcsAgpmSiegeWar->IsAttackGuild(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pThis->m_pcsAgcmCharacter->GetSelfCharacter())))
		return pThis->SendRequestResurrectionSiege(TRUE);
	else
	{
		if (pThis->m_pcsAgpmSiegeWar->IsArchlordCastle(
						pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(
									pThis->m_pcsAgcmCharacter->GetSelfCharacter())))
			return pThis->SendRequestResurrectionSiege(TRUE);
		else
			return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSelectSiegeResurrect);
	}

	return TRUE;
}

BOOL AgcmUICharacter::CBResurrectionSiegeInner(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter	*pThis	= (AgcmUICharacter *)	pClass;

	return pThis->SendRequestResurrectionSiege(TRUE);
}

BOOL AgcmUICharacter::CBResurrectionSiegeOuter(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter	*pThis	= (AgcmUICharacter *)	pClass;

	return pThis->SendRequestResurrectionSiege(FALSE);
}

// 2007.08.27. steeple
// 다른 사람이 나를 부활시켜주려고 한다. 그에 대한 패킷을 받았다.
BOOL AgcmUICharacter::CBResurrectionByOther(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICharacter* pThis = static_cast<AgcmUICharacter*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	PVOID* ppvBuffer = static_cast<PVOID*>(pCustData);

	CHAR* szName = static_cast<CHAR*>(ppvBuffer[0]);
	INT32 lFlag = *static_cast<INT32*>(ppvBuffer[1]);

	// 자신에 대한 오퍼레이션인지 확인한다.
	if(pcsCharacter != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return FALSE;

	// Display Setting
	ZeroMemory(pThis->m_szOtherName, sizeof(pThis->m_szOtherName));
	strncpy_s(pThis->m_szOtherName, sizeof(pThis->m_szOtherName), szName, _tcslen(szName));

	// Other Name Refresh
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUserDataOtherName);

	// 부활 UI 띄워준다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventResurrectionByOther);

	return TRUE;
}

// 2007.08.29. steeple
// 다른 사람이 나를 부활시키려고 했을 때, 승낙 여부.
BOOL AgcmUICharacter::CBResurrectionByOtherConfirm(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUICharacter* pThis = static_cast<AgcmUICharacter*>(pClass);

	if(lTrueCancel == (INT32)TRUE)
	{
		// 패킷을 보낸다.
		pThis->SendRequestResurrectionByOther(pThis->m_szOtherName);
	}

	return TRUE;
}

// 2007.08.29. steeple
// 누가 나를 부활시키려는 지 Display
BOOL AgcmUICharacter::CBDisplayOtherName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pClass || !szDisplay)
		return FALSE;

	AgcmUICharacter* pThis = static_cast<AgcmUICharacter*>(pClass);

	strncpy_s(szDisplay, sizeof(pThis->m_szOtherName), pThis->m_szOtherName, _tcslen(pThis->m_szOtherName));
	return TRUE;
}

BOOL AgcmUICharacter::CBOpenTargetNameTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	return TRUE;
}

BOOL AgcmUICharacter::CBCloseTargetNameTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter	*pThis	= (AgcmUICharacter *)	pClass;

	pThis->m_pcsAgcmUIManager2->CloseTooltip();
	return TRUE;
}

BOOL AgcmUICharacter::CBCheckResurrectionToTown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter	*pThis	= (AgcmUICharacter *)	pClass;

	INT32	lLevel	= pThis->m_pcsAgpmCharacter->GetLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter());

	AgpdPvPADChar* pcsAttachedPvP = pThis->m_pcsAgpmPvP->GetADCharacter(pThis->m_pcsAgcmCharacter->GetSelfCharacter());

	BOOL	bSiegeWar	= FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pThis->m_pcsAgcmCharacter->GetSelfCharacter());

	if (pcsSiegeWar &&
		pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar) &&
		(pThis->m_pcsAgpmSiegeWar->IsAttackGuild(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSiegeWar) || pThis->m_pcsAgpmSiegeWar->IsDefenseGuild(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSiegeWar)))
		bSiegeWar	= TRUE;

	if (lLevel < 6 ||
		pcsAttachedPvP->m_cDeadType == AGPMPVP_TARGET_TYPE_ENEMY_GUILD ||
		bSiegeWar
		//|| pcsAttachedPvP->m_cDeadType == AGPMPVP_TARGET_TYPE_NORMAL_PC		// PK는 창 띄우자
		)
		pThis->SendRequestResurrectionToTown();
	else
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenConfirmResurrectTownUI);

	return TRUE;
}

BOOL AgcmUICharacter::CBCheckResurrectionNow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	return TRUE;

	if (!pClass)
		return FALSE;

	AgcmUICharacter	*pThis	= (AgcmUICharacter *)	pClass;

	INT32	lLevel	= pThis->m_pcsAgpmCharacter->GetLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter());

	AgpdPvPADChar* pcsAttachedPvP = pThis->m_pcsAgpmPvP->GetADCharacter(pThis->m_pcsAgcmCharacter->GetSelfCharacter());

	if (lLevel < 6 ||
		pcsAttachedPvP->m_cDeadType == AGPMPVP_TARGET_TYPE_ENEMY_GUILD
		//|| pcsAttachedPvP->m_cDeadType == AGPMPVP_TARGET_TYPE_NORMAL_PC		// pk 는 창 뛰우자.
		)
		pThis->SendRequestResurrectionNow();
	else
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenConfirmResurrectUI);

	return TRUE;
}

BOOL AgcmUICharacter::CBIsExpPenalty(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)	pClass;

	if (pThis->m_pcsAgpmConfig && !pThis->m_pcsAgpmConfig->DoesExpPenaltyOnDeath())
		return FALSE;

	return TRUE;
}

BOOL AgcmUICharacter::CBIsActiveFollowMenu(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)	pClass;
	AgpdCharacter			*pcsTargetCharacter	= (AgpdCharacter *)		pData;

	// pcsTargetCharacter가 이미 파티 멤버인지 검사한다.
	// 파티 멤버라면 FALSE를 리턴한다.

	// pcsTargetCharacter가 PC인지 살펴본다.
	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsTargetCharacter))
		return FALSE;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter->m_lID == pcsTargetCharacter->m_lID)
		return FALSE;

	return TRUE;
}

BOOL AgcmUICharacter::CBIsActiveSiegeResurrection(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUICharacter			*pThis				= (AgcmUICharacter *)	pClass;
	AgpdCharacter			*pcsTargetCharacter	= (AgpdCharacter *)		pData;

	// check character
	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter->m_lID == pcsTargetCharacter->m_lID)
		return FALSE;

	// is player character?
	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsSelfCharacter))
		return FALSE;
	
	if (!pThis->m_pcsAgpmGuild || !pThis->m_pcsAgpmSiegeWar)
		return FALSE;
	
	// is in siege event? is participant of siege war? is able to resurrect in castle(depend on war status)
	if (!pThis->m_pcsAgpmSiegeWar->IsResurrectableInCastle(pcsSelfCharacter))
		return FALSE;

	return TRUE;
}

BOOL AgcmUICharacter::CBSetFollowTarget(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;

	AgpdCharacter		*pcsTargetCharacter	= (AgpdCharacter *)		pcsSourceControl->m_pstUserData->m_stUserData.m_pvData;
	if (pcsTargetCharacter)
		return pThis->m_pcsAgcmCharacter->SetFollowTarget(pcsTargetCharacter->m_lID, AGCMUICHARACTER_DEFAULT_FOLLOW_DISTANCE);

	return TRUE;
}


BOOL AgcmUICharacter::GetEditCon(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	AgcdUIControl		*pcsEditControl		= (AgcdUIControl *)		pData1;

	pThis->m_apcsEditControl[AGCD_UI_CHAR_EDIT_CON]	= pcsEditControl;

	return TRUE;
}

BOOL AgcmUICharacter::GetEditWis(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	AgcdUIControl		*pcsEditControl		= (AgcdUIControl *)		pData1;

	pThis->m_apcsEditControl[AGCD_UI_CHAR_EDIT_WIS]	= pcsEditControl;

	return TRUE;
}

BOOL AgcmUICharacter::GetEditDex(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	AgcdUIControl		*pcsEditControl		= (AgcdUIControl *)		pData1;

	pThis->m_apcsEditControl[AGCD_UI_CHAR_EDIT_DEX]	= pcsEditControl;

	return TRUE;
}

BOOL AgcmUICharacter::GetEditStr(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	AgcdUIControl		*pcsEditControl		= (AgcdUIControl *)		pData1;

	pThis->m_apcsEditControl[AGCD_UI_CHAR_EDIT_STR]	= pcsEditControl;

	return TRUE;
}

BOOL AgcmUICharacter::GetEditInt(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	AgcdUIControl		*pcsEditControl		= (AgcdUIControl *)		pData1;

	pThis->m_apcsEditControl[AGCD_UI_CHAR_EDIT_INT]	= pcsEditControl;

	return TRUE;
}

BOOL AgcmUICharacter::GetEditChar(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUICharacter		*pThis				= (AgcmUICharacter *)	pClass;
	AgcdUIControl		*pcsEditControl		= (AgcdUIControl *)		pData1;

	pThis->m_apcsEditControl[AGCD_UI_CHAR_EDIT_CHAR]	= pcsEditControl;

	return TRUE;
}

BOOL AgcmUICharacter::SetEditTextType1(AcUIEdit *pcsUIEdit, INT32 lOriginalValue, INT32 lCurrentValue)
{
	if (!pcsUIEdit)
		return FALSE;

	CHAR	szBuffer[128];
	ZeroMemory(szBuffer, sizeof(CHAR) * 128);

	if (lOriginalValue == lCurrentValue)
	{
		sprintf(szBuffer, "%d", lOriginalValue);
	}
	else if (lOriginalValue < lCurrentValue)
	{
		sprintf(szBuffer, "<C%s>%d<C%s>(+%d)", AGCMUICHARACTER_CHARSTATUS_ADD_TEXT_COLOR,
											   lCurrentValue,
											   AGCMUICHARACTER_CHARSTATUS_NORMAL_TEXT_COLOR,
											   lCurrentValue - lOriginalValue);
	}
	else
	{
		sprintf(szBuffer, "<C%s>%d<C%s>(-%d)", AGCMUICHARACTER_CHARSTATUS_SUB_TEXT_COLOR,
											   lCurrentValue,
											   AGCMUICHARACTER_CHARSTATUS_NORMAL_TEXT_COLOR,
											   lOriginalValue - lCurrentValue);
	}

	return pcsUIEdit->SetText(szBuffer);
}

BOOL AgcmUICharacter::SetEditTextType4(AcUIEdit *pcsUIEdit, INT32 lCurrentValue, CHAR *pszColor)
{
	if (!pcsUIEdit)
		return FALSE;

	CHAR	szBuffer[128];
	ZeroMemory(szBuffer, sizeof(CHAR) * 128);

	if (NULL == pszColor || 0 >= strlen(pszColor))
	{
		sprintf(szBuffer, "%d", lCurrentValue);
	}
	else
	{
		sprintf(szBuffer, "<C%s>%d<C%s>", pszColor, lCurrentValue, pszColor);
	}

	return pcsUIEdit->SetText(szBuffer);
}

BOOL AgcmUICharacter::RefreshCharacterStatus()
{
	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	INT32	lLevel	= m_pcsAgpmCharacter->GetLevel(pcsSelfCharacter);
	if (lLevel < 1 || lLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;

	AgpdCharacterTemplate	*pcsOriginalTemplate	= NULL;
	if (pcsSelfCharacter->m_bIsTrasform)
		pcsOriginalTemplate	= m_pcsAgpmCharacter->GetCharacterTemplate(pcsSelfCharacter->m_lOriginalTID);
	else
		pcsOriginalTemplate	= pcsSelfCharacter->m_pcsCharacterTemplate;

	if (m_apcsEditControl[AGCD_UI_CHAR_EDIT_CON] && m_apcsEditControl[AGCD_UI_CHAR_EDIT_CON]->m_pcsBase)
	{
		INT32	lOriginalValue	= 0;
		INT32	lCurrentValue	= 0;

		m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lCurrentValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
		m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);

		SetEditTextType1((AcUIEdit *) m_apcsEditControl[AGCD_UI_CHAR_EDIT_CON]->m_pcsBase, lOriginalValue, lCurrentValue);
	}

	if (m_apcsEditControl[AGCD_UI_CHAR_EDIT_WIS] && m_apcsEditControl[AGCD_UI_CHAR_EDIT_WIS]->m_pcsBase)
	{
		INT32	lOriginalValue	= 0;
		INT32	lCurrentValue	= 0;

		m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lCurrentValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);
		m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);

		SetEditTextType1((AcUIEdit *) m_apcsEditControl[AGCD_UI_CHAR_EDIT_WIS]->m_pcsBase, lOriginalValue, lCurrentValue);
	}

	if (m_apcsEditControl[AGCD_UI_CHAR_EDIT_DEX] && m_apcsEditControl[AGCD_UI_CHAR_EDIT_DEX]->m_pcsBase)
	{
		INT32	lOriginalValue	= 0;
		INT32	lCurrentValue	= 0;

		m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lCurrentValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
		m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

		SetEditTextType1((AcUIEdit *) m_apcsEditControl[AGCD_UI_CHAR_EDIT_DEX]->m_pcsBase, lOriginalValue, lCurrentValue);
	}

	if (m_apcsEditControl[AGCD_UI_CHAR_EDIT_STR] && m_apcsEditControl[AGCD_UI_CHAR_EDIT_STR]->m_pcsBase)
	{
		INT32	lOriginalValue	= 0;
		INT32	lCurrentValue	= 0;

		m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lCurrentValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);
		m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);

		SetEditTextType1((AcUIEdit *) m_apcsEditControl[AGCD_UI_CHAR_EDIT_STR]->m_pcsBase, lOriginalValue, lCurrentValue);
	}

	if (m_apcsEditControl[AGCD_UI_CHAR_EDIT_INT] && m_apcsEditControl[AGCD_UI_CHAR_EDIT_INT]->m_pcsBase)
	{
		INT32	lOriginalValue	= 0;
		INT32	lCurrentValue	= 0;

		m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lCurrentValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);
		m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

		SetEditTextType1((AcUIEdit *) m_apcsEditControl[AGCD_UI_CHAR_EDIT_INT]->m_pcsBase, lOriginalValue, lCurrentValue);
	}

	if (m_apcsEditControl[AGCD_UI_CHAR_EDIT_CHAR] && m_apcsEditControl[AGCD_UI_CHAR_EDIT_CHAR]->m_pcsBase)
	{
		INT32	lCurrentValue	= m_pcsAgpmCharacter->GetCharismaPoint(pcsSelfCharacter);

		SetEditTextType4((AcUIEdit *) m_apcsEditControl[AGCD_UI_CHAR_EDIT_CHAR]->m_pcsBase, lCurrentValue);
	}

	OnUpdateProperty();
	return TRUE;
}

/******************************************************************************
******************************************************************************/

BOOL AgcmUICharacter::StreamReadLevelupMessages(CHAR *szFile, BOOL bEncryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bEncryption))
	{
		OutputDebugString("AgcmUICharacter::StreamReadLevelupMessages() Error (1) !!!\n");
		return FALSE;
	}

	INT32							lRow			= csExcelTxtLib.GetRow();
	INT32							lCurRow;
	INT32							lLevel;
	INT32							lCTID;
	CHAR *							szValue;
	AgcdUICharacterLevelupMessage *	pstMessage;
	AgpdCharacterTemplate *			pcsTemplate;
	AgcdUICharacterADCharacterTemplate *	pstADTemplate;

	for ( lCurRow = 1; lCurRow < lRow; ++lCurRow)
	{
		lLevel					= csExcelTxtLib.GetDataToInt(0, lCurRow);
		lCTID					= csExcelTxtLib.GetDataToInt(1, lCurRow);

		ASSERT(lLevel < AGPMCHAR_MAX_LEVEL);

		if (!lCTID)
		{
			pstMessage = m_astCommonLevelupMessage + lLevel;
		}
		else
		{
			VERIFY(pcsTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(lCTID));
			if (!pcsTemplate)
				continue;

			pstADTemplate = GetCharacterTemplateData(pcsTemplate);
			if (!pstADTemplate->m_pstLevelUpMessage)
				VERIFY(pstADTemplate->m_pstLevelUpMessage = new AgcdUICharacterLevelupMessage [AGPMCHAR_MAX_LEVEL]);

			pstMessage = pstADTemplate->m_pstLevelUpMessage + lLevel;
		}

		if (pstMessage->m_szMessage1)
		{
			delete [] pstMessage->m_szMessage1;
			pstMessage->m_szMessage1 = NULL;
		}

		if (pstMessage->m_szMessage2)
		{
			delete [] pstMessage->m_szMessage2;
			pstMessage->m_szMessage2 = NULL;
		}

		pstMessage->m_dwColor1	= csExcelTxtLib.GetDataToInt(2, lCurRow);
		szValue					= csExcelTxtLib.GetData(3, lCurRow);
		if (szValue)
		{
			pstMessage->m_szMessage1	= new CHAR [strlen(szValue) + 1];
			if (pstMessage->m_szMessage1)
				strcpy(pstMessage->m_szMessage1, szValue);
		}

		pstMessage->m_dwColor2	= csExcelTxtLib.GetDataToInt(4, lCurRow);
		szValue					= csExcelTxtLib.GetData(5, lCurRow);
		if (szValue)
		{
			pstMessage->m_szMessage2	= new CHAR [strlen(szValue) + 1];
			if (pstMessage->m_szMessage2)
				strcpy(pstMessage->m_szMessage2, szValue);
		}
	}

	return TRUE;
}

BOOL AgcmUICharacter::AddBaseMinimapType(ApBase *pcsBase, AgcmMinimap::MPInfo::TYPE eType, CHAR *szDisplay, INT32 lQuestTID)
{
	if (!pcsBase || !m_pcsAgcmMinimap)
		return FALSE;

	AgcdUICharacterADBase *	pstADBase;
	AuPOS					stPos;

	if( pcsBase->m_eType == APBASE_TYPE_CHARACTER )
	{
		// 미니맵에 표시하지 않는다
		if( !((AgpdCharacter *) pcsBase)->m_bNPCDisplayForMap )
			return TRUE;
	}

	pstADBase = GetBaseData(pcsBase, &stPos);
	if (pstADBase->m_lMinimapIndex >= 0)
		m_pcsAgcmMinimap->RemovePoint(pstADBase->m_lMinimapIndex);

	if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
		pstADBase->m_lMinimapIndex = m_pcsAgcmMinimap->AddPoint(eType, szDisplay ? szDisplay : ((AgpdCharacter *) pcsBase)->m_szID, &stPos);
	else if (pcsBase->m_eType == APBASE_TYPE_OBJECT)
		pstADBase->m_lMinimapIndex = m_pcsAgcmMinimap->AddPoint(eType, szDisplay ? szDisplay : ((ApdObject *) pcsBase)->m_pcsTemplate->m_szName, &stPos, lQuestTID);

	return TRUE;
}

BOOL AgcmUICharacter::RemoveBaseMinimapType(ApBase *pcsBase, INT32 lQuestTID)
{
	if (!pcsBase || !m_pcsAgcmMinimap)
		return FALSE;

	AgcdUICharacterADBase *	pstADBase;
	//AuPOS					stPos;

	pstADBase = GetBaseData(pcsBase);
	if (pstADBase->m_lMinimapIndex >= 0)
	{
		//퀘스트 완료 표시 버그 수정 정영훈 2005.10.5
		// lQuestTID는 항상 -1 이 아니므로 RemovePoint는 수행되지 않는다.
		// 성능상 차이는 약간 있겠지만 구분할수 있는 방법이 없고, 서버는 죽지 않을것으로 보이므로 둘다 호출하는 방법으로 변경
		//if (-1 == lQuestTID)
			m_pcsAgcmMinimap->RemovePoint(pstADBase->m_lMinimapIndex);
		//else
			m_pcsAgcmMinimap->RemovePointGroup(lQuestTID);
		
		pstADBase->m_lMinimapIndex = -1;
	}

	return TRUE;
}

BOOL AgcmUICharacter::ChangeBaseMinimapType(ApBase *pcsBase, AgcmMinimap::MPInfo::TYPE eType, CHAR *szDisplay)
{
	return AddBaseMinimapType(pcsBase, eType, szDisplay);
}

AgcmMinimap::MPInfo *	AgcmUICharacter::GetBaseMinimapInfo(ApBase *pcsBase)
{
	if (!pcsBase || !m_pcsAgcmMinimap)
		return NULL;

	return m_pcsAgcmMinimap->GetPointInfo(GetBaseData(pcsBase)->m_lMinimapIndex);
}

BOOL AgcmUICharacter::_UpdateTargetPlayerRaceAndClass( AgpdCharacter* ppdCharacter )
{
	if( !ppdCharacter ) return FALSE;
	if( !m_pcsAgcmUIManager2 ) return FALSE;
	if( !m_pcsAgcmCharacter || !m_pcsAgpmFactors ) return AURACE_TYPE_NONE;

	AgcdCharacter* pcdCharacter = m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );
	if( !pcdCharacter ) return AURACE_TYPE_NONE;

	AgpdFactor* ppdFactor = &ppdCharacter->m_pcsCharacterTemplate->m_csFactor;
	if( !ppdFactor ) return AURACE_TYPE_NONE;

	AuRaceType eRace = ( AuRaceType )m_pcsAgpmFactors->GetRace( ppdFactor );
	AuCharClassType eClass = ( AuCharClassType )m_pcsAgpmFactors->GetClass( ppdFactor );

	switch( eRace )
	{
	case AURACE_TYPE_HUMAN :
		{
			m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetRaceHuman );

			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassKnight );			break;
			case AUCHARCLASS_TYPE_RANGER :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassArcher );			break;
			case AUCHARCLASS_TYPE_MAGE :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassMage );			break;
			}
		}
		break;

	case AURACE_TYPE_ORC :
		{
			m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetRaceOrc );

			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassBerserker );		break;
			case AUCHARCLASS_TYPE_RANGER :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassHunter );			break;
			case AUCHARCLASS_TYPE_MAGE :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassSocerer );		break;
			}
		}
		break;

	case AURACE_TYPE_MOONELF :
		{
			m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetRaceMoonElf );

			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassSwashBuckler );	break;
			case AUCHARCLASS_TYPE_RANGER :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassRanger );			break;
			case AUCHARCLASS_TYPE_MAGE :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassElementalist );	break;
			}
		}
		break;

	case AURACE_TYPE_DRAGONSCION :
		{
			m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetRaceDragonScion );

			switch( eClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassSlayer );			break;
			case AUCHARCLASS_TYPE_RANGER :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassObiter );			break;
			case AUCHARCLASS_TYPE_MAGE :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassSummerner );		break;
			case AUCHARCLASS_TYPE_SCION :	m_pcsAgcmUIManager2->ThrowEvent( m_nEventUpdateTargetClassScion );			break;
			}

		}
		break;
	}

	return TRUE;
}

BOOL AgcmUICharacter::OnToggleBtnRestoreTransform( BOOL bIsShow )
{
	return bIsShow ? OnShowBtnRestoreTransform() : OnHideBtnRestoreTransform();
}

BOOL AgcmUICharacter::OnShowBtnRestoreTransform( void )
{
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter	= pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 아크로드 변신이면 취소버튼을 만들지 않는다.
	if( m_pcsAgpmArchlord->IsArchlord( ppdCharacter->m_szID ) ) return TRUE;

	// 변신했다. 취소 버튼을 알려준다.
	if( !m_pcsTransformCancelGridItem )
	{
		m_pcsTransformCancelGridItem = m_pcsAgpmGrid->CreateGridItem();
	}

	if( !m_pcsTransformCancelGridItem ) return FALSE;

	m_pcsTransformCancelGridItem->m_bMoveable = FALSE;
	m_pcsTransformCancelGridItem->m_eType = AGPDGRID_ITEM_TYPE_ITEM;
	m_pcsTransformCancelGridItem->SetTooltip( "Transform");

	// GridItem 에 텍스쳐를 세팅한다.
	RwTexture** ppGridTexture = m_pcsAgcmUIControl->GetAttachGridItemTextureData( m_pcsTransformCancelGridItem );
	if( ppGridTexture )
	{
		*ppGridTexture = m_pcsAgcmUIControl->m_pCancelTransform;
	}

	m_pcsAgcmUIMain->AddSystemMessageGridItem( m_pcsTransformCancelGridItem, CBCancelTransform, this );
	return TRUE;
}

BOOL AgcmUICharacter::OnHideBtnRestoreTransform( void )
{
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter	= pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 변신이 풀렸다고 알려준다.
	m_ulTransformEndTimeMSec	= 0;
	m_ulTransformDurationMSec	= 0;
	m_ulTransformNextNoticeMSec	= 0;

	// 변신 취소 버튼을 없앤다.
	m_pcsAgcmUIMain->RemoveSystemMessageGridItem( m_pcsTransformCancelGridItem );
	return TRUE;
}

BOOL AgcmUICharacter::OnToggleBtnRestoreEvolution( BOOL bIsShow )
{
	return bIsShow ? OnShowBtnRestoreEvolution() : OnHideBtnRestoreEvolution();
}

BOOL AgcmUICharacter::OnShowBtnRestoreEvolution( void )
{
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter	= pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 아크로드 변신이면 취소버튼을 만들지 않는다.
	if( m_pcsAgpmArchlord->IsArchlord( ppdCharacter->m_szID ) ) return TRUE;

	// 변신했다. 취소 버튼을 알려준다.
	if( !m_pcsEvolutionCancelGridItem )
	{
		m_pcsEvolutionCancelGridItem = m_pcsAgpmGrid->CreateGridItem();
	}

	if( !m_pcsEvolutionCancelGridItem ) return FALSE;

	m_pcsEvolutionCancelGridItem->m_bMoveable = FALSE;
	m_pcsEvolutionCancelGridItem->m_eType = AGPDGRID_ITEM_TYPE_ITEM;
	m_pcsEvolutionCancelGridItem->SetTooltip( "Evolution" );

	// GridItem 에 텍스쳐를 세팅한다.
	RwTexture** ppGridTexture = m_pcsAgcmUIControl->GetAttachGridItemTextureData( m_pcsEvolutionCancelGridItem );
	if( ppGridTexture )
	{
		*ppGridTexture = m_pcsAgcmUIControl->m_pCancelEvolution;
	}

	m_pcsAgcmUIMain->AddSystemMessageGridItem( m_pcsEvolutionCancelGridItem, CBCancelEvolution, this );
	return TRUE;
}

BOOL AgcmUICharacter::OnHideBtnRestoreEvolution( void )
{
	if( !m_pcsAgcmUIMain || !m_pcsEvolutionCancelGridItem ) return FALSE;

	// 변신 취소 버튼을 없앤다.
	m_pcsAgcmUIMain->RemoveSystemMessageGridItem( m_pcsEvolutionCancelGridItem );
	return TRUE;
}

BOOL AgcmUICharacter::OnShowBlindEquipSlot( void* pCharacter, INT nPartID )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return FALSE;

	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	if( !ppmItem ) return FALSE;

	AgpdGridItem* ppdGridItem = ppmItem->GetEquipItem( ppdCharacter, ( AgpmItemPart )nPartID );
	if( ppdGridItem )
	{
		ppdGridItem->SetDurabilityZero( TRUE );
	}

	return TRUE;
}

BOOL AgcmUICharacter::OnHideBlindEquipSlot( void* pCharacter, INT nPartID )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return FALSE;

	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	AgpmFactors* ppmFactor = ( AgpmFactors* )GetModule( "AgpmFactors" );
	if( !ppmItem || !pcmCharacter || !ppmFactor ) return FALSE;

	AuRaceType eRace = ( AuRaceType )ppmFactor->GetRace( &ppdCharacter->m_csFactor );
	if( eRace == AURACE_TYPE_DRAGONSCION )
	{
		// 드래곤시온인 경우 캐릭터 타입에 따라 블라인드가 해제되면 안되는 부위가 있다.
		AuCharClassType eClass = pcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
		switch( eClass )
		{
		case AUCHARCLASS_TYPE_KNIGHT :	if( nPartID == AGPMITEM_PART_HAND_RIGHT ) return FALSE;		break;
		case AUCHARCLASS_TYPE_RANGER :	if( nPartID == AGPMITEM_PART_HAND_LEFT ) return FALSE;		break;
		}
	}

	AgpdGridItem* ppdGridItem = ppmItem->GetEquipItem( ppdCharacter, ( AgpmItemPart )nPartID );
	if( ppdGridItem )
	{
		ppdGridItem->SetDurabilityZero( FALSE );
	}

	return TRUE;
}

BOOL AgcmUICharacter::OnShowUIResurrection( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	return pcmUIManager->ThrowEvent( m_lEventOpenResurrectionUI );
}

BOOL AgcmUICharacter::CallBack_OnSelectPropertyAttack( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pBase, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )pClass;
	pcmUICharacter->OnSelectProperty( ePropertyType_Attack );
	pcmUICharacter->RefreshCharacterStatus();

	return TRUE;
}

BOOL AgcmUICharacter::CallBack_OnSelectPropertyTab( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pBase, AgcdUIControl* pControl )
{
	if( !pClass || !pControl ) return FALSE;

	int nNameLength = ( int )strlen( pControl->m_szName );
	char* pNumber = pControl->m_szName + ( nNameLength - 1 );
	if( !pNumber || strlen( pNumber ) <= 0 ) return FALSE;

	eCharacterPropertyType eType = ( eCharacterPropertyType )atoi( pNumber );

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )pClass;
	pcmUICharacter->OnSelectProperty( eType );
	pcmUICharacter->RefreshCharacterStatus();

	return TRUE;
}
