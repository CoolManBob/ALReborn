#include "AgcmUISkill2.h"
#include "AuStrTable.h"
#include "AgcmUISkillDragonScion.h"

extern INT32	g_alFactorTable[AGPMSKILL_CONST_PERCENT_END + 1][3];

AgcmUISkill2::AgcmUISkill2()
{
	SetModuleName("AgcmUISkill2");

	m_pcsAgpmGrid					= NULL;
	m_pcsAgpmFactors				= NULL;
	m_pcsAgpmCharacter				= NULL;
	m_pcsAgpmSkill					= NULL;
	m_pcsAgpmEventSkillMaster		= NULL;
	m_pcsAgcmCharacter				= NULL;
	m_pcsAgcmItem					= NULL;
	m_pcsAgcmSkill					= NULL;
	m_pcsAgcmEventSkillMaster		= NULL;
	m_pcsAgcmUIManager2				= NULL;
	m_pcsAgcmUICharacter			= NULL;
	m_pcsAgcmUIMain					= NULL;
	m_pcsAgcmUIItem					= NULL;
	m_pcsAgcmUIControl				= NULL;
	m_pcsAgpmBillInfo				= NULL;

	//m_pcsUserDataSkillMastery		= NULL;
	m_pcsUserDataSkillGrid			= NULL;
	m_pcsUserDataSkillBuyGrid		= NULL;
	m_pcsUserDataSkillUpgradeGrid	= NULL;

	m_pcsUserDataSkillPoint			= NULL;

	m_pcsUserDataActiveSkillTab			= NULL;
	m_pcsUserDataActiveBuySkillTab		= NULL;
	m_pcsUserDataActiveUpgradeSkillTab	= NULL;

	m_pcsUserDataMainDialogMsg		= NULL;

	m_pcsUserDataIsActiveInitialize	= NULL;

	ZeroMemory(m_pcsUserDataSkillHighLevel, sizeof(AgcdUIUserData*) * AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL);

	m_lSkillPoint					= 0;
	ZeroMemory(m_lSkillPointHighLevel, sizeof(INT32) * AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL);

	m_pcsSelfCharacter				= NULL;

	ZeroMemory(m_lSkillMasteryList, sizeof(INT32) * AGPMEVENT_SKILL_MAX_MASTERY * AGPMEVENT_SKILL_MAX_MASTERY_SKILL);

	m_eRaceType						= AURACE_TYPE_NONE;
	m_eClassType					= AUCHARCLASS_TYPE_NONE;

	m_pcsEvent						= NULL;

	//m_pcsSelectedGrid				= NULL;

	m_llInvenMoney					= 0;

	m_lSelectedItemIndex			= (-1);
	m_lSelectedBuyItemIndex			= (-1);
	m_lSelectedUpgradeItemIndex		= (-1);

	m_pcsUseSkillBook				= NULL;

	m_bIsOpenSkillEventUI			= FALSE;

	ZeroMemory(&m_stOpenSkillUIPos, sizeof(AuPOS));

	ZeroMemory(m_stSkillHighLevelGrid, sizeof(AgpdGrid) * AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL);

//	m_lMasteryTabIndex				= 0;
//	m_lMasteryBuyTabIndex			= 0;
//	m_lMasteryUpgradeTabIndex		= 0;

	m_lLastSetMasteryIndex			= 0;

	m_bIsDestroy					= FALSE;

	m_bIsActiveInitialize			= FALSE;

	m_lSelectedHighLevelSkillTID		= 0;
	m_lSelectedUpgradeHighLevelSkillTID = 0;

	m_bIsOpenSkillHighLevelUI			= FALSE;
	m_bIsOpenSkillUpgradeHighLevelUI	= FALSE;

	m_bIsOpenSkillHeroicUI				= FALSE;
	m_bIsOpenSkillUpgradeHeroicUI		= FALSE;
}

AgcmUISkill2::~AgcmUISkill2()
{
}

BOOL AgcmUISkill2::OnAddModule()
{
	m_pcsAgpmGrid				= (AgpmGrid *)				GetModule("AgpmGrid");
	m_pcsAgpmFactors			= (AgpmFactors *)			GetModule("AgpmFactors");
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmItem				= (AgpmItem *)				GetModule("AgpmItem");
	m_pcsAgpmSkill				= (AgpmSkill *)				GetModule("AgpmSkill");
	m_pcsAgpmEventSkillMaster	= (AgpmEventSkillMaster *)	GetModule("AgpmEventSkillMaster");
	m_pcsAgcmCharacter			= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pcsAgcmItem				= (AgcmItem *)				GetModule("AgcmItem");
	m_pcsAgcmSkill				= (AgcmSkill *)				GetModule("AgcmSkill");
	m_pcsAgcmEventSkillMaster	= (AgcmEventSkillMaster *)	GetModule("AgcmEventSkillMaster");
	m_pcsAgcmUIManager2			= (AgcmUIManager2 *)		GetModule("AgcmUIManager2");
	m_pcsAgcmUICharacter		= (AgcmUICharacter *)		GetModule("AgcmUICharacter");
	m_pcsAgcmUIMain				= (AgcmUIMain *)			GetModule("AgcmUIMain");
	m_pcsAgcmUIItem				= (AgcmUIItem *)			GetModule("AgcmUIItem");
	m_pcsAgcmUIControl			= (AgcmUIControl *)			GetModule("AgcmUIControl");
	m_pcsAgcmEventEffect		= (AgcmEventEffect *)		GetModule("AgcmEventEffect");
	m_pcsAgpmBillInfo			= (AgpmBillInfo *)			GetModule("AgpmBillInfo");
	m_pcsAgcmUICashInven		= (AgcmUICashInven *)		GetModule("AgcmUICashInven");

	m_UIDragonScion.OnAddModule( this );

	if (!m_pcsAgpmGrid ||
		!m_pcsAgpmFactors ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmSkill ||
		!m_pcsAgpmEventSkillMaster ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmItem ||
		!m_pcsAgcmEventSkillMaster ||
		!m_pcsAgcmUIManager2 ||
		!m_pcsAgcmUICharacter ||
		!m_pcsAgcmUIMain ||
		!m_pcsAgcmUIItem ||
		!m_pcsAgcmUIControl ||
		!m_pcsAgpmBillInfo ||
		!m_pcsAgcmUICashInven)
		return FALSE;

	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorSkillPoint(CBUpdateCharFactorSkillPoint, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateActionStatus(CBUpdateActionStatus, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackUpdateSkillPoint(CBUpdateCharSkillPoint, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackUpdateMoney(CBUpdateInvenMoney, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackReleaseSelfCharacter(CBReleaseSelfCharacter, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	if (!m_pcsAgpmSkill->SetCallbackInitSkill(CBAddSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackRemoveSkill(CBRemoveSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackUpdateSkillPoint(CBUpdateSkillPoint, this))
		return FALSE;
	/*
	if (!m_pcsAgpmSkill->SetCallbackUpdateSkillExp(CBUpdateSkillExp, this))
		return FALSE;
	*/
	if (!m_pcsAgpmSkill->SetCallbackUpdateSkill(CBUpdateSkillPoint, this))
		return FALSE;

	if (!m_pcsAgpmSkill->SetCallbackReceiveAction(CBReceiveSkillAction, this))
		return FALSE;
	if (m_pcsAgcmSkill && !m_pcsAgcmSkill->SetCallbackCheckCastSkillResult(CBCheckCastSkillResult, this))
		return FALSE;

	if (!m_pcsAgpmSkill->SetCallbackAddBuffedList(CBAddBuffedList, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackRemoveBuffedList(CBRemoveBuffedList, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackInitCoolTime(CBInitCoolTime, this))
		return FALSE;

	if (!m_pcsAgcmItem->SetCallbackConfirmUseSkillBook(CBConfirmUseSkillBook, this))
		return FALSE;

	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackOpenTooltip(CBOpenQuickBeltToolTip, this))
		return FALSE;
	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackCloseTooltip(CBCloseQuickBeltToolTip, this))
		return FALSE;
	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackCloseAllUITooltip(CBCloseAllUIToolTip, this))
		return FALSE;
	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackUpdateAlarmGrid(CBUpdateAlarmGrid, this))
		return FALSE;

	if (m_pcsAgcmUIItem && !m_pcsAgcmUIItem->SetCallbackSetTooltip(CBSetItemTooltip, this))
		return FALSE;

	/*
	if (m_pcsAgcmUICharacter && !m_pcsAgcmUICharacter->SetCallbackRefreshUpdateCharStatus(CBRefreshUpdateCharStatus, this))
		return FALSE;
	*/

	if (!m_pcsAgpmEventSkillMaster->SetCallbackBuyResult(CBBuySkillResult, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackResponseUpgrade(CBUpgradeSkillResult, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackLearnResult(CBLearnSkillResult, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackSkillInitializeResult(CBSkillInitializeResult, this))
		return FALSE;

	if (!m_pcsAgcmEventSkillMaster->SetCallbackResponseGrant(CBResponseGrant, this))
		return FALSE;

	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorHp(CBUpdateCost, this))
		return FALSE;
	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorMp(CBUpdateCost, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackRemoveFromInventory(CBUpdateItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackInventory(CBUpdateItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUIUpdateInventory(CBUpdateItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackEquip(CBUpdateItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUnEquip(CBUpdateItem, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackGetModifiedSkillLevel( CBGetSkillLevel , this) )
		return FALSE;

	if (!AddFunction() ||
		!AddEvent() ||
		!AddUserData() ||
		!AddDisplay() ||
		!AddBoolean())
		return FALSE;

	return TRUE;
}

BOOL AgcmUISkill2::OnInit()
{
	INT32				i;

	for (i = 0; i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL; ++i)
	{
		m_pcsAgpmGrid->Init(&m_stSkillListGrid[i], 1, 1, 1);
		m_stSkillListGrid[i].m_lGridType	= AGPDGRID_ITEM_TYPE_SKILL;

		m_pcsAgpmGrid->Init(&m_stSkillBuyListGrid[i], 1, 1, 1);
		m_stSkillBuyListGrid[i].m_lGridType	= AGPDGRID_ITEM_TYPE_SKILL;

		m_pcsAgpmGrid->Init(&m_stSkillUpgradeListGrid[i], 1, 1, 1);
		m_stSkillUpgradeListGrid[i].m_lGridType	= AGPDGRID_ITEM_TYPE_SKILL;
	}

	for (i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
	{
		m_pcsAgpmGrid->Init(&m_stSkillHighLevelGrid[i], 1, 1, 1);
		m_stSkillHighLevelGrid[i].m_lGridType = AGPDGRID_ITEM_TYPE_SKILL;
	}


	if (m_pcsAgcmUIManager2)
	{
		m_csSkillToolTip.m_Property.bTopmost = TRUE								;
		m_pcsAgcmUIManager2->AddWindow( (AgcWindow*)(&m_csSkillToolTip ) )	;
		m_csSkillToolTip.ShowWindow( FALSE )										;

		m_csMasteryToolTip.m_Property.bTopmost = TRUE								;
		m_pcsAgcmUIManager2->AddWindow( (AgcWindow*)(&m_csMasteryToolTip ) )	;
		m_csMasteryToolTip.ShowWindow( FALSE )										;

		m_csAddSPToolTip.m_Property.bTopmost = TRUE								;
		m_pcsAgcmUIManager2->AddWindow( (AgcWindow*)(&m_csAddSPToolTip ) )	;
		m_csAddSPToolTip.ShowWindow( FALSE )				
			;
	}

	m_HeroicSkill.Initialize( this );
	_InitSkillTooltipColor();
	m_UIDragonScion.OnInit();

	// Heroic Skill Open/Close Event
	m_lEventOpenSkillHeroicUI			=	m_HeroicSkill.GetHeroicOpenEventOther();
	m_lEventCloseSkillHeroicUI			=	m_HeroicSkill.GetHeroicCloseEventOther();
	m_lEventOpenSkillHeroicUpgradeUI	=	m_HeroicSkill.GetHeroicUpOpenEventOther();
	m_lEventCloseSkillHeroicUpgradeUI	=	m_HeroicSkill.GetHeroicUpCloseEventOther();


	return TRUE;
}

BOOL AgcmUISkill2::OnDestroy()
{
	INT32				i;

	for (i = 0; i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL; ++i)
	{
		m_pcsAgpmGrid->Remove(&m_stSkillListGrid[i]);
		m_pcsAgpmGrid->Remove(&m_stSkillBuyListGrid[i]);
		m_pcsAgpmGrid->Remove(&m_stSkillUpgradeListGrid[i]);
	}

	for (i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
	{
		m_pcsAgpmGrid->Remove(&m_stSkillHighLevelGrid[i]);
	}

	m_bIsDestroy	= TRUE;

	return TRUE;
}

BOOL AgcmUISkill2::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetMastery0", CBSetMastery0, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetMastery1", CBSetMastery1, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetMastery2", CBSetMastery2, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetMastery3", CBSetMastery3, 0))
		return FALSE;

	// 2006.10.12. steeple
	// 아크로드 스킬 탭으로 변경.
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetMastery4", CBSetMastery4, 0))// ## Product
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetMasteryHighLevel", CBSetMasteryHighLevel, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetMasteryHeroic" , CBSetMasteryHeroic, 0))
		return FALSE;
	
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetBuyMastery0", CBSetBuyMastery0, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetBuyMastery1", CBSetBuyMastery1, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetBuyMastery2", CBSetBuyMastery2, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetBuyMastery3", CBSetBuyMastery3, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetUpgradeMastery0", CBSetUpgradeMastery0, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetUpgradeMastery1", CBSetUpgradeMastery1, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetUpgradeMastery2", CBSetUpgradeMastery2, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SkillSetUpgradeMastery3", CBSetUpgradeMastery3, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_UI_Open", CBOpenSkillUI, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_BuyUI_Open", CBOpenSkillBuyUI, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_UpgradeUI_Open", CBOpenSkillUpgradeUI, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_HighLevelUI_Open", CBOpenSkillHighLevelUI, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_HighLevelUpgradeUI_Open", CBOpenSkillHighLevelSkillUpgradeUI, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_HeroicUI_Open", CBOpenSkillHeroicUI, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_HeroicUpgradeUI_Open" , CBOpenSkillHeroicSkillUpgradeUI, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_HighLevelUI_Close", CBCloseSkillHighLevelUI, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_HeroicUI_Close", CBCloseSkillHeroicUI, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_SelectGrid", CBSelectSkillGrid, 1, "grid user data (parent user data)"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_SelectBuyGrid", CBSelectBuySkillGrid, 1, "grid user data (parent user data)"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_SelectUpgradeGrid", CBSelectUpgradeSkillGrid, 1, "grid user data (parent user data)"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_Buy", CBBuySkill, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_Upgrade", CBUpgradeSkill, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_Open_Tooltip", CBOpenSkillToolTip, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_Open_ButtonTooltip", CBOpenButtonSkillToolTip, 1, "SkillGridControl"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_Open_Buy_Tooltip", CBOpenBuySkillToolTip, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_Open_Buy_ButtonTooltip", CBOpenBuyButtonSkillToolTip, 1, "SkillGridControl"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_Open_Upgrade_Tooltip", CBOpenUpgradeSkillToolTip, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_Open_Upgrade_ButtonTooltip", CBOpenUpgradeButtonSkillToolTip, 1, "SkillGridControl"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_Close_Tooltip", CBCloseSkillToolTip, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_UpdateListItemStatus", CBUpdateListItemStatus, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_UpdateBuyListItemStatus", CBUpdateBuyListItemStatus, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_UpdateUpgradeListItemStatus", CBUpdateUpgradeListItemStatus, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_CloseSkillEventUI", CBCloseSkillEventUI, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_ToggleUI", CBToggleSkillUI, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_OpenDescTooltip", CBOpenSkillDescToolTip, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_CloseDescTooltip", CBCloseSkillDescToolTip, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_TreeInitialize", CBSkillInitialize, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_SetFailInitializeText", CBSetFailInitializeText, 1, "Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_SetConfirmInitializeText", CBSetConfirmInitializeText, 1, "Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_InitializeOK", CBSkillInitializeOK, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Skill_Rollback", CBSkillRollback, 0))
		return FALSE;

	m_UIDragonScion.OnAddFunction( m_pcsAgcmUIManager2 );
	return TRUE;
}

BOOL AgcmUISkill2::AddEvent()
{
	m_lEventOpenSkillNPCDialogUI = m_pcsAgcmUIManager2->AddEvent("Skill_NPCDialog_UI_Open");
	if (m_lEventOpenSkillNPCDialogUI < 0)
		return FALSE;
	m_lEventCloseSkillNPCDialogUI = m_pcsAgcmUIManager2->AddEvent("Skill_NPCDialog_UI_Close");
	if (m_lEventCloseSkillNPCDialogUI < 0)
		return FALSE;

	m_lEventOpenSkillUI = m_pcsAgcmUIManager2->AddEvent("Skill_UI_Open");
	if (m_lEventOpenSkillUI < 0)
		return FALSE;

	m_lEventCloseSkillUI = m_pcsAgcmUIManager2->AddEvent("Skill_UI_Close");
	if (m_lEventCloseSkillUI < 0)
		return FALSE;

	m_lEventOpenSkillBuyUI = m_pcsAgcmUIManager2->AddEvent("Skill_Buy_UI_Open");
	if (m_lEventOpenSkillBuyUI < 0)
		return FALSE;

	m_lEventCloseSkillBuyUI = m_pcsAgcmUIManager2->AddEvent("Skill_Buy_UI_Close");
	if (m_lEventCloseSkillBuyUI < 0)
		return FALSE;

	m_lEventOpenSkillHighLevelUI = m_pcsAgcmUIManager2->AddEvent("Skill_HighLevel_UI_Open");
	if (m_lEventOpenSkillHighLevelUI < 0)
		return FALSE;

	m_lEventCloseSkillHighLevelUI = m_pcsAgcmUIManager2->AddEvent("Skill_HighLevel_UI_Close");
	if (m_lEventCloseSkillHighLevelUI < 0)
		return FALSE;

	m_lEventOpenSkillHighLevelUpgradeUI = m_pcsAgcmUIManager2->AddEvent("Skill_HighLevel_Upgrade_UI_Open");
	if (m_lEventOpenSkillHighLevelUpgradeUI < 0)
		return FALSE;

	m_lEventCloseSkillHighLevelUpgradeUI = m_pcsAgcmUIManager2->AddEvent("Skill_HighLevel_Upgrade_UI_Close");
	if (m_lEventCloseSkillHighLevelUpgradeUI < 0)
		return FALSE;

	m_lEventCastResultFailed = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_Failed");
	if (m_lEventCastResultFailed < 0)
		return FALSE;

	m_lEventCastResultInvalidTarget = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_InvalidTarget");
	if (m_lEventCastResultInvalidTarget < 0)
		return FALSE;

	m_lEventCastResultNeedRequirement = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_NeedRequirement");
	if (m_lEventCastResultNeedRequirement < 0)
		return FALSE;

	m_lEventCastResultNotEnoughHP = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_NotEnoughHP");
	if (m_lEventCastResultNotEnoughHP < 0)
		return FALSE;

	m_lEventCastResultNotEnoughSP = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_NotEnoughSP");
	if (m_lEventCastResultNotEnoughSP < 0)
		return FALSE;

	m_lEventCastResultNotEnoughMP = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_NotEnoughMP");
	if (m_lEventCastResultNotEnoughMP < 0)
		return FALSE;

	m_lEventCastResultNotEnoughArrow = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_NotEnoughArrow");
	if (m_lEventCastResultNotEnoughArrow < 0)
		return FALSE;

	m_lEventCastResultAlreadyUse = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_AlreadyUse");
	if (m_lEventCastResultAlreadyUse < 0)
		return FALSE;

	m_lEventCastResultNotReadyCast = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_NotReadyCast");
	if (m_lEventCastResultNotReadyCast < 0)
		return FALSE;

	m_lEventCastResultMiss = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_Miss");
	if (m_lEventCastResultMiss < 0)
		return FALSE;

	m_lEventCastResultInvalidTargetToGather = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_InvalidTargetToGather");
	if (m_lEventCastResultInvalidTargetToGather < 0)
		return FALSE;

	m_lEventCastResultNotEnoughProduct = m_pcsAgcmUIManager2->AddEvent("Skill_Cast_NotEnoughProduct");
	if (m_lEventCastResultNotEnoughProduct < 0)
		return FALSE;

//	m_lEventSetMasteryTab1	= m_pcsAgcmUIManager2->AddEvent("Skill_SetMasteryTab_1");
//	if (m_lEventSetMasteryTab1 < 0)
//		return FALSE;
//	m_lEventSetMasteryTab2	= m_pcsAgcmUIManager2->AddEvent("Skill_SetMasteryTab_2");
//	if (m_lEventSetMasteryTab2 < 0)
//		return FALSE;
//	m_lEventSetMasteryTab3	= m_pcsAgcmUIManager2->AddEvent("Skill_SetMasteryTab_3");
//	if (m_lEventSetMasteryTab3 < 0)
//		return FALSE;
//	m_lEventSetMasteryTab4	= m_pcsAgcmUIManager2->AddEvent("Skill_SetMasteryTab_4");
//	if (m_lEventSetMasteryTab4 < 0)
//		return FALSE;
	m_lEventSetMasteryTab5	= m_pcsAgcmUIManager2->AddEvent("Skill_SetMasteryTab_5");	// ## Product
	if (m_lEventSetMasteryTab5 < 0)
		return FALSE;
//	m_lEventSetMasteryTab6	= m_pcsAgcmUIManager2->AddEvent("Skill_SetMasteryTab_6");
//	if (m_lEventSetMasteryTab6 < 0)
//		return FALSE;

	m_lEventSelectedItem	= m_pcsAgcmUIManager2->AddEvent("Skill_SelectedItem");
	if (m_lEventSelectedItem < 0)
		return FALSE;
	m_lEventUnSelectedItem	= m_pcsAgcmUIManager2->AddEvent("Skill_UnSelectedItem");
	if (m_lEventUnSelectedItem < 0)
		return FALSE;

	m_lEventActiveItem		= m_pcsAgcmUIManager2->AddEvent("Skill_ActiveItem");
	if (m_lEventActiveItem < 0)
		return FALSE;
	m_lEventDisableItem		= m_pcsAgcmUIManager2->AddEvent("Skill_DisableItem");
	if (m_lEventDisableItem < 0)
		return FALSE;

	m_lEventBuySelectedItem	= m_pcsAgcmUIManager2->AddEvent("Skill_BuySelectedItem");
	if (m_lEventBuySelectedItem < 0)
		return FALSE;
	m_lEventBuyUnSelectedItem	= m_pcsAgcmUIManager2->AddEvent("Skill_BuyUnSelectedItem");
	if (m_lEventBuyUnSelectedItem < 0)
		return FALSE;

	m_lEventBuyActiveItem		= m_pcsAgcmUIManager2->AddEvent("Skill_BuyActiveItem");
	if (m_lEventBuyActiveItem < 0)
		return FALSE;
	m_lEventBuyDisableItem		= m_pcsAgcmUIManager2->AddEvent("Skill_BuyDisableItem");
	if (m_lEventBuyDisableItem < 0)
		return FALSE;

	m_lEventUpgradeSelectedItem	= m_pcsAgcmUIManager2->AddEvent("Skill_UpgradeSelectedItem");
	if (m_lEventUpgradeSelectedItem < 0)
		return FALSE;
	m_lEventUpgradeUnSelectedItem	= m_pcsAgcmUIManager2->AddEvent("Skill_UpgradeUnSelectedItem");
	if (m_lEventUpgradeUnSelectedItem < 0)
		return FALSE;

	m_lEventUpgradeActiveItem		= m_pcsAgcmUIManager2->AddEvent("Skill_UpgradeActiveItem");
	if (m_lEventUpgradeActiveItem < 0)
		return FALSE;
	m_lEventUpgradeDisableItem		= m_pcsAgcmUIManager2->AddEvent("Skill_UpgradeDisableItem");
	if (m_lEventUpgradeDisableItem < 0)
		return FALSE;

	for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
	{
		CHAR szEventName[128];
		_stprintf(szEventName, "Skill_HighLevelSelectedItem_%d", i);
		m_lEventHighLevelSelectedItem[i]		= m_pcsAgcmUIManager2->AddEvent(szEventName);
		if(m_lEventHighLevelSelectedItem[i] < 0)
			return FALSE;

		_stprintf(szEventName, "Skill_HighLevelUnSelectedItem_%d", i);
		m_lEventHighLevelUnSelectedItem[i]		= m_pcsAgcmUIManager2->AddEvent(szEventName);
		if(m_lEventHighLevelUnSelectedItem[i] < 0)
			return FALSE;

		_stprintf(szEventName, "Skill_UpgradeHighLevelSelectedItem_%d", i);
		m_lEventUpgradeHighLevelSelectedItem[i]		= m_pcsAgcmUIManager2->AddEvent(szEventName);
		if(m_lEventUpgradeHighLevelSelectedItem[i] < 0)
			return FALSE;

		_stprintf(szEventName, "Skill_UpgradeHighLevelUnSelectedItem_%d", i);
		m_lEventUpgradeHighLevelUnSelectedItem[i]		= m_pcsAgcmUIManager2->AddEvent(szEventName);
		if(m_lEventUpgradeHighLevelUnSelectedItem[i] < 0)
			return FALSE;

		_stprintf(szEventName, "Skill_HighLevelActiveItem_%d", i);
		m_lEventHighLevelActiveItem[i]		= m_pcsAgcmUIManager2->AddEvent(szEventName);
		if(m_lEventHighLevelActiveItem[i] < 0)
			return FALSE;

		_stprintf(szEventName, "Skill_HighLevelDisableItem_%d", i);
		m_lEventHighLevelDisableItem[i]		= m_pcsAgcmUIManager2->AddEvent(szEventName);
		if(m_lEventHighLevelDisableItem[i] < 0)
			return FALSE;

		_stprintf(szEventName, "Skill_UpgradeHighLevelActiveItem_%d", i);
		m_lEventUpgradeHighLevelActiveItem[i]		= m_pcsAgcmUIManager2->AddEvent(szEventName);
		if(m_lEventUpgradeHighLevelActiveItem[i] < 0)
			return FALSE;

		_stprintf(szEventName, "Skill_UpgradeHighLevelDisableItem_%d", i);
		m_lEventUpgradeHighLevelDisableItem[i]		= m_pcsAgcmUIManager2->AddEvent(szEventName);
		if(m_lEventUpgradeHighLevelDisableItem[i] < 0)
			return FALSE;
	}

	m_lEventSkillBuySuccess			= m_pcsAgcmUIManager2->AddEvent("Skill_BuySuccess");
	if (m_lEventSkillBuySuccess < 0)
		return FALSE;
	m_lEventSkillLearnSuccess		= m_pcsAgcmUIManager2->AddEvent("Skill_LearnSuccess");
	if (m_lEventSkillLearnSuccess < 0)
		return FALSE;
	m_lEventSkillUpgradeSuccess		= m_pcsAgcmUIManager2->AddEvent("Skill_UpgradeSuccess");
	if (m_lEventSkillUpgradeSuccess < 0)
		return FALSE;

	m_lEventNotEnoughMoney			= m_pcsAgcmUIManager2->AddEvent("Skill_ErrorNotEnoughMoney");
	if (m_lEventNotEnoughMoney < 0)
		return FALSE;
	m_lEventInvalidClass			= m_pcsAgcmUIManager2->AddEvent("Skill_ErrorInvalidClass");
	if (m_lEventInvalidClass < 0)
		return FALSE;
	m_lEventLowLevel				= m_pcsAgcmUIManager2->AddEvent("Skill_ErrorLowLevel");
	if (m_lEventLowLevel < 0)
		return FALSE;
	m_lEventNotEnoughSkillPoint		= m_pcsAgcmUIManager2->AddEvent("Skill_ErrorNotEnoughSkillPoint");
	if (m_lEventNotEnoughSkillPoint < 0)
		return FALSE;
	m_lEventNotSelectBuySkill		= m_pcsAgcmUIManager2->AddEvent("Skill_ErrorNotSelectBuySkill");
	if (m_lEventNotSelectBuySkill < 0)
		return FALSE;
	m_lEventNotSelectUpgradeSkill	= m_pcsAgcmUIManager2->AddEvent("Skill_ErrorNotSelectUpgradeSkill");
	if (m_lEventNotSelectUpgradeSkill < 0)
		return FALSE;
	m_lEventAlreadyLearnSkill		= m_pcsAgcmUIManager2->AddEvent("Skill_ErrorAlreadyLearnSkill");
	if (m_lEventAlreadyLearnSkill < 0)
		return FALSE;

	m_lEventConfirmBuySkill			= m_pcsAgcmUIManager2->AddEvent("Skill_ConfirmBuySkill", CBReturnConfirmBuySkill, this);
	if (m_lEventConfirmBuySkill < 0)
		return FALSE;
	m_lEventConfirmLearnSkill		= m_pcsAgcmUIManager2->AddEvent("Skill_ConfirmLearnSkill", CBReturnConfirmLearnSkill, this);
	if (m_lEventConfirmLearnSkill < 0)
		return FALSE;
	m_lEventConfirmUpgradeSkill		= m_pcsAgcmUIManager2->AddEvent("Skill_ConfirmUpgradeSkill", CBReturnConfirmUpgradeSkill, this);
	if (m_lEventConfirmUpgradeSkill < 0)
		return FALSE;

	m_lEventConfirmSkillInitialize	= m_pcsAgcmUIManager2->AddEvent("Skill_ConfirmSkillInitialize");
	if (m_lEventConfirmSkillInitialize < 0)
		return FALSE;
	m_lEventFailSkillInitialize		= m_pcsAgcmUIManager2->AddEvent("Skill_FailSkillInitialize");
	if (m_lEventFailSkillInitialize < 0)
		return FALSE;

	m_lEventSkillInitializeSuccess	= m_pcsAgcmUIManager2->AddEvent("Skill_SkillInitialize_Success");
	if (m_lEventSkillInitializeSuccess < 0)
		return FALSE;
	m_lEventSkillInitializeFailed	= m_pcsAgcmUIManager2->AddEvent("Skill_SkillInitialize_Failed");
	if (m_lEventSkillInitializeFailed < 0)
		return FALSE;

	m_IEventSkillInitializeMoneyFull = m_pcsAgcmUIManager2->AddEvent("Skill_SkillInitialize_MoneyFull");
	if (m_IEventSkillInitializeMoneyFull < 0)
		return FALSE;

	m_lEventSkillRollbackConfirm	= m_pcsAgcmUIManager2->AddEvent("Skill_RollbackConfirm", CBReturnConfirmSkillRollback, this);
	if (m_lEventSkillRollbackConfirm < 0)
		return FALSE;

	m_lEventNotEnoughHeroicPoint	= m_pcsAgcmUIManager2->AddEvent("Skill_ErrorNotEnoughHeroicPoint");
	if( !m_lEventNotEnoughHeroicPoint < 0 )
		return FALSE;

	m_lEventNotEnoughCharismaPoint	= m_pcsAgcmUIManager2->AddEvent("Skill_ErrorNotEnoughCharismaPoint");
	if( m_lEventNotEnoughCharismaPoint < 0 )
		return FALSE;

	m_UIDragonScion.OnAddEvent( m_pcsAgcmUIManager2 );
	return TRUE;
}

BOOL AgcmUISkill2::AddUserData()
{
//	AgpdEventSkillMastery	*pcsMastery	= m_pcsAgpmEventSkillMaster->GetMastery();
//
//	m_pcsUserDataSkillMastery	= m_pcsAgcmUIManager2->AddUserData("SkillMastery",
//																   &pcsMastery,
//																   sizeof(CHAR),
//																   sizeof(AgpdEventSkillMastery) / sizeof(CHAR),
//																   AGCDUI_USERDATA_TYPE_STRING);
//	if (!m_pcsUserDataSkillMastery)
//		return FALSE;

	m_pcsUserDataSkillGrid		= m_pcsAgcmUIManager2->AddUserData("SkillListGrid",
																	m_stSkillListGrid,
																	sizeof(AgpdGrid),
																	0,
																	AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pcsUserDataSkillGrid)
		return FALSE;

	m_pcsUserDataSkillBuyGrid		= m_pcsAgcmUIManager2->AddUserData("SkillBuyListGrid",
																	m_stSkillBuyListGrid,
																	sizeof(AgpdGrid),
																	AGPMEVENT_SKILL_MAX_MASTERY_SKILL,
																	AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pcsUserDataSkillBuyGrid)
		return FALSE;

	m_pcsUserDataSkillUpgradeGrid		= m_pcsAgcmUIManager2->AddUserData("SkillUpgradeListGrid",
																	m_stSkillUpgradeListGrid,
																	sizeof(AgpdGrid),
																	AGPMEVENT_SKILL_MAX_MASTERY_SKILL,
																	AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pcsUserDataSkillUpgradeGrid)
		return FALSE;

	for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
	{
		CHAR szGridName[128];
		_stprintf(szGridName, "SkillHighLevelGrid%d", i);
		m_pcsUserDataSkillHighLevel[i]		= m_pcsAgcmUIManager2->AddUserData(szGridName,
																		&m_stSkillHighLevelGrid[i],
																		sizeof(AgpdGrid),
																		1,
																		AGCDUI_USERDATA_TYPE_GRID);
		if(!m_pcsUserDataSkillHighLevel[i])
			return FALSE;
	}

	m_pcsUserDataSkillPoint		= m_pcsAgcmUIManager2->AddUserData("SkillPoint",
																	&m_lSkillPoint,
																	sizeof(INT32),
																	1,
																	AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataSkillPoint)
		return FALSE;

	for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
	{
		CHAR szGridName[128];
		_stprintf(szGridName, "SkillPointHighLevel%d", i);
		m_pcsUserDataSkillHighLevelPoint[i]		= m_pcsAgcmUIManager2->AddUserData(szGridName,
																		&m_lSkillPointHighLevel[i],
																		sizeof(INT32),
																		1,
																		AGCDUI_USERDATA_TYPE_INT32);
		if (!m_pcsUserDataSkillHighLevelPoint[i])
			return FALSE;
	}

	m_pcsUserDataInvenMoney		= m_pcsAgcmUIManager2->AddUserData("Skill_InvenMoney",
																	&m_llInvenMoney,
																	sizeof(INT64),
																	1,
																	AGCDUI_USERDATA_TYPE_UINT64);
	if (!m_pcsUserDataInvenMoney)
		return FALSE;

	m_pcsUserDataActiveBuyButton		= m_pcsAgcmUIManager2->AddUserData("Skill_ActiveBuyButton",
																	&m_lDummyData,
																	sizeof(INT32),
																	1,
																	AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataActiveBuyButton)
		return FALSE;

	m_pcsUserDataActiveUpgradeButton	= m_pcsAgcmUIManager2->AddUserData("Skill_ActiveUpgradeButton",
																	&m_lDummyData,
																	sizeof(INT32),
																	1,
																	AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataActiveUpgradeButton)
		return FALSE;

	m_pcsUserDataActiveSkillTab			= m_pcsAgcmUIManager2->AddUserData("Skill_ActiveTabButton",
																	&m_lDummyData,
																	sizeof(INT32),
																	1,
																	AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataActiveSkillTab)
		return FALSE;

	m_pcsUserDataActiveBuySkillTab		= m_pcsAgcmUIManager2->AddUserData("Skill_ActiveBuyTabButton",
																	&m_lDummyData,
																	sizeof(INT32),
																	1,
																	AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataActiveBuySkillTab)
		return FALSE;

	m_pcsUserDataActiveUpgradeSkillTab	= m_pcsAgcmUIManager2->AddUserData("Skill_ActiveUpgradeTabButton",
																	&m_lDummyData,
																	sizeof(INT32),
																	1,
																	AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataActiveUpgradeSkillTab)
		return FALSE;

	m_pcsUserDataMainDialogMsg			= m_pcsAgcmUIManager2->AddUserData("Skill_MainDialogMsg",
																	&m_lDummyData,
																	sizeof(INT32),
																	1,
																	AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataMainDialogMsg)
		return FALSE;

	m_pcsUserDataIsActiveInitialize		= m_pcsAgcmUIManager2->AddUserData("Skill_IsActiveInitialize",
																	&m_bIsActiveInitialize,
																	sizeof(INT32),
																	1,
																	AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pcsUserDataIsActiveInitialize)
		return FALSE;

	m_UIDragonScion.OnAddUserData( m_pcsAgcmUIManager2 );

	return TRUE;
}

BOOL AgcmUISkill2::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPoint", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT, CBDisplaySkillPoint, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillName", AGCMUI_SKILL2_DISPLAY_ID_SKILL_NAME, CBDisplaySkillName, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillLevel", AGCMUI_SKILL2_DISPLAY_ID_SKILL_LEVEL, CBDisplaySkillLevel, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillLevel2", AGCMUI_SKILL2_DISPLAY_ID_SKILL_LEVEL_2, CBDisplaySkillLevel, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPrice", AGCMUI_SKILL2_DISPLAY_ID_SKILL_PRICE, CBDisplaySkillPrice, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillTrainerClass", AGCMUI_SKILL2_DISPLAY_ID_SKILL_TRAINER_CLASS, CBDisplaySkillTrainerClass, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillMainDialog", AGCMUI_SKILL2_DISPLAY_ID_SKILL_MAIN_DIALOG, CBDisplaySkillMainDialog, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillInvenMoney", AGCMUI_SKILL2_DISPLAY_ID_INVEN_MONEY, CBDisplayInvenMoney, AGCDUI_USERDATA_TYPE_UINT64))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillBuyButton", AGCMUI_SKILL2_DISPLAY_ID_SKILL_BUY_BUTTON, CBDisplaySkillBuyButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillSelectedName", AGCMUI_SKILL2_DISPLAY_ID_SKILL_SELECTED_NAME, CBDisplaySelectedSkillName, AGCDUI_USERDATA_TYPE_BOOL))
		return FALSE;

	// High Level Skill Point
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel0", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_0, CBDisplaySkillPointHighLevel0, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel1", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_1, CBDisplaySkillPointHighLevel1, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel2", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_2, CBDisplaySkillPointHighLevel2, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel3", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_3, CBDisplaySkillPointHighLevel3, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel4", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_4, CBDisplaySkillPointHighLevel4, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel5", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_5, CBDisplaySkillPointHighLevel5, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel6", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_6, CBDisplaySkillPointHighLevel6, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel7", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_7, CBDisplaySkillPointHighLevel7, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel8", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_8, CBDisplaySkillPointHighLevel8, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel9", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_9, CBDisplaySkillPointHighLevel9, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillPointHighLevel10", AGCMUI_SKILL2_DISPLAY_ID_SKILL_POINT_HIGH_LEVEL_10, CBDisplaySkillPointHighLevel10, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	// ## Product
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillLevelExp", AGCMUI_SKILL2_DISPLAY_ID_SKILL_LEVEL_EXP, CBDisplaySkillLevelExp, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillExpDegree", AGCMUI_SKILL2_DISPLAY_ID_SKILL_EXP_DEGREE, CBDisplaySkillExpDegree, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SkillExpDegreeMax", AGCMUI_SKILL2_DISPLAY_ID_SKILL_EXP_DEGREE_MAX, CBDisplaySkillExpDegree, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	m_UIDragonScion.OnAddDisplay( m_pcsAgcmUIManager2 );
	return TRUE;
}

BOOL AgcmUISkill2::AddBoolean()
{
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "Skill_ActiveBuyButton", CBIsActiveBuyButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "Skill_ActiveUpgradeButton", CBIsActiveUpgradeButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "Skill_ActiveRollbackButton", CBIsActiveRollbackButton, AGCDUI_USERDATA_TYPE_BOOL))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "Skill_ActiveRollbackButton2", CBIsActiveRollbackButton2, AGCDUI_USERDATA_TYPE_BOOL))
		return FALSE;

	return TRUE;
}

BOOL AgcmUISkill2::SetMastery(INT32 lMasteryIndex)
{
	// 윈도우 위치 조정
	_MoveSkillWindow();

	if (lMasteryIndex == AGPMEVENT_SKILL_ARCHLORD_MASTERY_INDEX)	// 아크로드 스킬
		return SetMasteryArchlord(lMasteryIndex);

	if (lMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX)
		return SetMasteryHighLevel(lMasteryIndex);

	if( lMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX )
		return SetMasteryHeroic(lMasteryIndex);

	if (lMasteryIndex < 0 || lMasteryIndex >= AGPMEVENT_SKILL_MAX_MASTERY)
		return FALSE;

	m_pcsUserDataActiveSkillTab->m_lSelectedIndex	= lMasteryIndex;
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataActiveSkillTab);

	m_lSelectedItemIndex	= (-1);
	m_lSelectedHighLevelSkillTID		= 0;
	m_lSelectedUpgradeHighLevelSkillTID	= 0;

	INT32	lAddedItem	= 0;

	int i;
	for (i = 0; i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL; ++i)
	{
		m_pcsAgpmGrid->Reset(&m_stSkillListGrid[i]);

		AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->GetSkill(m_lSkillMasteryList[lMasteryIndex][i]);
		if (pcsSkill && pcsSkill->m_pcsTemplate)
		{
			if (m_pcsAgpmGrid->Add(&m_stSkillListGrid[lAddedItem], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1))
				++lAddedItem;
		}
	}

	m_pcsUserDataSkillGrid->m_stUserData.m_lCount	= lAddedItem;

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillGrid);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillPoint);

	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUserDataSkillGrid);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataIsActiveInitialize);

	for (i = 0; i < lAddedItem; ++i)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventUnSelectedItem, i);
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventActiveItem, i);
	}

	if (lMasteryIndex == 4)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventSetMasteryTab5);

		m_bIsOpenSkillUI		= FALSE;
		m_bIsOpenSkillProductUI	= TRUE;
	}
	else
	{
		m_bIsOpenSkillUI		= TRUE;
		m_bIsOpenSkillProductUI	= FALSE;
	}

	switch (lMasteryIndex) {
	//case 0:
	//	m_pcsAgcmUIManager2->ThrowEvent(m_lEventSetMasteryTab1);
	//	break;
	//case 1:
	//	m_pcsAgcmUIManager2->ThrowEvent(m_lEventSetMasteryTab2);
	//	break;
	//case 2:
	//	m_pcsAgcmUIManager2->ThrowEvent(m_lEventSetMasteryTab3);
	//	break;
	//case 3:
	//	m_pcsAgcmUIManager2->ThrowEvent(m_lEventSetMasteryTab4);
	//	break;
	// ## Product
	case 4:
		// 원 스킬창을 닫는건 INI에 ...?
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventSetMasteryTab5);
		break;
	}

	m_lLastSetMasteryIndex	= lMasteryIndex;

	return TRUE;
}

BOOL AgcmUISkill2::SetBuyMastery(INT32 lMasteryIndex)
{
	if (lMasteryIndex < 0 || lMasteryIndex >= AGPMEVENT_SKILL_MAX_MASTERY)
		return FALSE;

	m_pcsUserDataActiveBuySkillTab->m_lSelectedIndex	= lMasteryIndex;
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataActiveBuySkillTab);

	m_lSelectedBuyItemIndex	= (-1);
	m_lSelectedUpgradeHighLevelSkillTID = 0;

	INT32	lAddedItem	= 0;
	m_lSelectedHighLevelSkillTID		= 0;
	m_lSelectedUpgradeHighLevelSkillTID	= 0;

	INT32	*pcsSkillMasteryList	= m_pcsAgpmEventSkillMaster->GetMastery(m_eRaceType, m_eClassType, lMasteryIndex);
	if (!pcsSkillMasteryList)
		return FALSE;

	INT32	i;

	for (i = 0; i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL; ++i)
	{
		m_pcsAgpmGrid->Reset(&m_stSkillBuyListGrid[i]);

		AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pcsSkillMasteryList[i]);
		if (pcsSkillTemplate && m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkillTemplate) && m_pcsAgcmSkill)
		{
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);

			if (m_pcsAgpmGrid->Add(&m_stSkillBuyListGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1))
				++lAddedItem;

			AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->GetSkillByTID(m_pcsSelfCharacter, pcsSkillTemplate->m_lID);
			if (pcsSkill)
				pcsSkillTemplate->m_pcsGridItem->m_lItemID	= pcsSkill->m_lID;
			else
				pcsSkillTemplate->m_pcsGridItem->m_lItemID	= 0;
		}
	}

	m_pcsUserDataSkillBuyGrid->m_stUserData.m_lCount	= lAddedItem;

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillBuyGrid);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillPoint);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataActiveBuyButton);

	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUserDataSkillBuyGrid); 

	for (i = 0; i < lAddedItem; ++i)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventBuyUnSelectedItem, i);

		SetStatusActiveBuyItem(i);
	}

	m_lLastSetMasteryIndex	= lMasteryIndex;

	return TRUE;
}

BOOL AgcmUISkill2::SetUpgradeMastery(INT32 lMasteryIndex)
{
	if (lMasteryIndex < 0 || lMasteryIndex >= AGPMEVENT_SKILL_MAX_MASTERY)
		return FALSE;

	m_pcsUserDataActiveUpgradeSkillTab->m_lSelectedIndex	= lMasteryIndex;
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataActiveUpgradeSkillTab);

	m_lSelectedUpgradeItemIndex	= (-1);
	m_lSelectedHighLevelSkillTID		= 0;
	m_lSelectedUpgradeHighLevelSkillTID	= 0;

	INT32	i;
	INT32	lAddedItem	= 0;

	for (i = 0; i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL; ++i)
	{
		m_pcsAgpmGrid->Reset(&m_stSkillUpgradeListGrid[i]);

		AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->GetSkill(m_lSkillMasteryList[lMasteryIndex][i]);
		if (pcsSkill && pcsSkill->m_pcsTemplate)
		{
			if (m_pcsAgpmGrid->Add(&m_stSkillUpgradeListGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1))
				++lAddedItem;
		}
	}

	m_pcsUserDataSkillUpgradeGrid->m_stUserData.m_lCount	= lAddedItem;

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillUpgradeGrid);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillPoint);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataActiveUpgradeButton);

	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUserDataSkillUpgradeGrid);

	for (i = 0; i < lAddedItem; ++i)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventUpgradeUnSelectedItem, i);

		SetStatusActiveUpgradeItem(i);
	}

	m_lLastSetMasteryIndex	= lMasteryIndex;

	return TRUE;
}

BOOL AgcmUISkill2::SetMasteryArchlord(INT32 lMasteryIndex)
{
	if(lMasteryIndex != AGPMEVENT_SKILL_ARCHLORD_MASTERY_INDEX)
		return FALSE;

	m_pcsUserDataActiveSkillTab->m_lSelectedIndex	= lMasteryIndex;
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataActiveSkillTab);

	m_lSelectedItemIndex	= (-1);
	m_lSelectedHighLevelSkillTID		= 0;
	m_lSelectedUpgradeHighLevelSkillTID	= 0;

	INT32	lAddedItem	= 0;

	ArchlordSkillList& vcArchlordSkill = m_pcsAgpmEventSkillMaster->GetArchlordSkillList();
	ArchlordSkillIter iter = vcArchlordSkill.begin();
	
	int i = 0;
	while(iter != vcArchlordSkill.end() && i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL)
	{
		m_pcsAgpmGrid->Reset(&m_stSkillListGrid[i]);

		AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(*iter);
		if(!pcsSkillTemplate)
		{
			++iter; ++i;
			continue;
		}

		// 아크로드이면 잘 나오고 아니면 그레이로 나온다.
		AgpdSkill* pcsSkill	= m_pcsAgpmSkill->GetSkill(m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
		if (pcsSkill && pcsSkill->m_pcsTemplate)
		{
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkill);

			if (m_pcsAgpmGrid->Add(&m_stSkillListGrid[lAddedItem], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1))
				++lAddedItem;
		}
		else
		{
			if(m_pcsAgpmCharacter->IsArchlord(m_pcsSelfCharacter->m_szID))
				m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);
			else
				m_pcsAgcmSkill->SetGridSkillAttachedUnableTexture(pcsSkillTemplate);

			if (m_pcsAgpmGrid->Add(&m_stSkillListGrid[lAddedItem], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1))
				++lAddedItem;
		}

		++iter;	++i;
	}

	m_pcsUserDataSkillGrid->m_stUserData.m_lCount	= lAddedItem;

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillGrid);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillPoint);

	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUserDataSkillGrid);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataIsActiveInitialize);

	for (i = 0; i < lAddedItem; ++i)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventUnSelectedItem, i);
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventActiveItem, i);
	}

	m_lLastSetMasteryIndex	= lMasteryIndex;

	return TRUE;
}

// 2007.05.20. steeple
BOOL AgcmUISkill2::SetMasteryHighLevel(INT32 lMasteryIndex)
{
	if(lMasteryIndex != AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX)
		return FALSE;

	if(!m_pcsSelfCharacter || !m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	m_pcsUserDataActiveSkillTab->m_lSelectedIndex	= lMasteryIndex;
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataActiveSkillTab);

	m_lSelectedItemIndex		= (-1);
	m_lSelectedUpgradeItemIndex = (-1);

	m_lSelectedHighLevelSkillTID		= 0;
	m_lSelectedUpgradeHighLevelSkillTID	= 0;

	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector(m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID);
	if(!pVector)
	{
		for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
			m_pcsAgpmGrid->Reset(&m_stSkillHighLevelGrid[i]);
	}
	else
	{
		int i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL)
		{
			m_pcsAgpmGrid->Reset(&m_stSkillHighLevelGrid[i]);

			AgpdEventSkillHighLevel stHighLevel = *iter;
			AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			if(!pcsSkillTemplate)
			{
				++iter; ++i;
				continue;
			}

			// 일단 그리드 텍스쳐는 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);

			// 스킬을 배웠으면 잘 나오고 아니면 그레이로 나온다.
			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkill(m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
			if(pcsSkill && pcsSkill->m_pcsTemplate)
			{
				// Active 해준다.
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventHighLevelActiveItem[i]);

				if (m_pcsAgpmGrid->Add(&m_stSkillHighLevelGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1))
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillHighLevel[i]);
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillHighLevelPoint[i]);
				}
			}
			else
			{
				// Disable 해준다.
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventHighLevelDisableItem[i]);

				if (m_pcsAgpmGrid->Add(&m_stSkillHighLevelGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1))
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillHighLevel[i]);
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillHighLevelPoint[i]);
				}
			}

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventHighLevelUnSelectedItem[i]);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillPoint);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataIsActiveInitialize);

	m_lLastSetMasteryIndex	= lMasteryIndex;

	return TRUE;
}

BOOL AgcmUISkill2::SetMasteryHeroic(INT32 lMasteryIndex)
{
	m_HeroicSkill.LoadHeroicSkill();
	m_lLastSetMasteryIndex	= lMasteryIndex;

	return TRUE;
}

BOOL AgcmUISkill2::SetUpgradeMasteryHighLevel()
{
	if(!m_pcsSelfCharacter || !m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataActiveSkillTab);

	m_lSelectedItemIndex	= (-1);
	m_lSelectedUpgradeHighLevelSkillTID = 0;

	INT32 lSelectedCharTID = m_pcsAgpmEventSkillMaster->GetCharTID(m_eRaceType, m_eClassType);
	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector(lSelectedCharTID);
	if(!pVector)
	{
		for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
			m_pcsAgpmGrid->Reset(&m_stSkillHighLevelGrid[i]);
	}
	else
	{
		int i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL)
		{
			m_pcsAgpmGrid->Reset(&m_stSkillHighLevelGrid[i]);

			AgpdEventSkillHighLevel stHighLevel = *iter;
			AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			if(!pcsSkillTemplate || !pcsSkillTemplate->m_pcsGridItem)
			{
				++iter; ++i;
				continue;
			}

			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkillByTID(m_pcsSelfCharacter, pcsSkillTemplate->m_lID);
			if(pcsSkill)
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = pcsSkill->m_lID;
			else
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = 0;

			// 일단 그리드는 제대로 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);
			if(IsActiveUpgradeHighLevelItem(pcsSkillTemplate->m_lID))
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventUpgradeHighLevelActiveItem[i]);
			else
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventUpgradeHighLevelDisableItem[i]);

			if(m_pcsAgpmGrid->Add(&m_stSkillHighLevelGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1))
			{
				m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillHighLevel[i]);
				m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillHighLevelPoint[i]);
			}

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventUpgradeHighLevelUnSelectedItem[i]);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillPoint);
	//m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataIsActiveInitialize);

	m_lLastSetMasteryIndex	= AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX;

	return TRUE;
}

BOOL AgcmUISkill2::SetUpgradeMasteryHeroic()
{
	m_HeroicSkill.LoadHeroicSkillBuy();

	m_lLastSetMasteryIndex	=	AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX;
	return TRUE;
}

BOOL AgcmUISkill2::CBSetMastery0(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);

	if(pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX ||
		pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX)
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillUI);

	return pThis->SetMastery(0);
}

BOOL AgcmUISkill2::CBSetMastery1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);

	if(pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX ||
		pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX )
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillUI);

	return pThis->SetMastery(1);
}

BOOL AgcmUISkill2::CBSetMastery2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 고렙 스킬창 닫아준다. 2007.05.20. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

	// 고렙 업그레이드 스킬창 닫아준다. 2007.06.04. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);

	// 기존의 창이 고렙 스킬창이라면 새롭게 창을 열어줘야 한다.
	if(pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX ||
		pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX )
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillUI);

	return pThis->SetMastery(2);
}

BOOL AgcmUISkill2::CBSetMastery3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 고렙 스킬창 닫아준다. 2007.05.20. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);

	// 고렙 업그레이드 스킬창 닫아준다. 2007.06.04. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	// 기존의 창이 고렙 스킬창이라면 새롭게 창을 열어줘야 한다.
	if( pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX ||
		pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX )
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillUI);

	return pThis->SetMastery(3);
}

// 2006.10.12. steeple
// 아크로드 스킬 탭으로 변경.
BOOL AgcmUISkill2::CBSetMastery4(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 고렙 스킬창 닫아준다. 2007.05.20. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

	// 고렙 업그레이드 스킬창 닫아준다. 2007.06.04. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


	// 기존의 창이 고렙 스킬창이라면 새롭게 창을 열어줘야 한다.
	if(pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX ||
		pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX )
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillUI);

	return pThis->SetMastery(AGPMEVENT_SKILL_ARCHLORD_MASTERY_INDEX);
}

// 2007.05.20. steeple
BOOL AgcmUISkill2::CBSetMasteryHighLevel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 업그레이드 창 닫아준다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	// 일반 스킬창 닫아준다. 2007.05.20. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);

	// 고렙 스킬창 연다. 2007.05.20. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillHighLevelUI);

	return pThis->SetMastery(AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX);
}

BOOL AgcmUISkill2::CBSetMasteryHeroic(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

	// 일반 스킬창 닫아준다. 2007.05.20. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);

	// Heroic UI Open
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillHeroicUI);

	return pThis->SetMastery(AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX);
}

BOOL AgcmUISkill2::CBSetBuyMastery0(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 고렙 업그레이드 스킬창 닫아준다. 2007.06.04. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	// 고렙 스킬창 닫아준다. 2007.05.20. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


	// 기존의 창이 고렙 업그레이드 스킬창이라면 새롭게 창을 열어줘야 한다.
	if(pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX ||
		pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX )
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillBuyUI);

	pThis->_MoveSkillWindow( TRUE );

	return pThis->SetBuyMastery(0);
}

BOOL AgcmUISkill2::CBSetBuyMastery1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 고렙 업그레이드 스킬창 닫아준다. 2007.06.04. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	// 고렙 스킬창 닫아준다. 2007.05.20. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


	// 기존의 창이 고렙 업그레이드 스킬창이라면 새롭게 창을 열어줘야 한다.
	if(pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX ||
		pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX )
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillBuyUI);

	pThis->_MoveSkillWindow( TRUE );

	return pThis->SetBuyMastery(1);
}

BOOL AgcmUISkill2::CBSetBuyMastery2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 고렙 업그레이드 스킬창 닫아준다. 2007.06.04. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	// 고렙 스킬창 닫아준다. 2007.05.20. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


	// 기존의 창이 고렙 업그레이드 스킬창이라면 새롭게 창을 열어줘야 한다.
	if(pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX ||
		pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX )
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillBuyUI);

	pThis->_MoveSkillWindow( TRUE );

	return pThis->SetBuyMastery(2);
}

BOOL AgcmUISkill2::CBSetBuyMastery3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 고렙 업그레이드 스킬창 닫아준다. 2007.06.04. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	// 고렙 스킬창 닫아준다. 2007.05.20. steeple
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


	// 기존의 창이 고렙 업그레이드 스킬창이라면 새롭게 창을 열어줘야 한다.
	if(pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX ||
		pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX )
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillBuyUI);

	pThis->_MoveSkillWindow( TRUE );

	return pThis->SetBuyMastery(3);
}

BOOL AgcmUISkill2::CBSetUpgradeMastery0(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	return pThis->SetUpgradeMastery(0);
}

BOOL AgcmUISkill2::CBSetUpgradeMastery1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;
	if( pThis->m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) != AURACE_TYPE_DRAGONSCION ) return TRUE;

	return pThis->SetUpgradeMastery(1);
}

BOOL AgcmUISkill2::CBSetUpgradeMastery2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	return pThis->SetUpgradeMastery(2);
}

BOOL AgcmUISkill2::CBSetUpgradeMastery3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	return pThis->SetUpgradeMastery(3);
}

BOOL AgcmUISkill2::CBOpenSkillUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	pThis->_MoveSkillWindow( TRUE );

	if( pThis->m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) != AURACE_TYPE_DRAGONSCION )
	{
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillUI);
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);

			return TRUE;
		}

		else if(pThis->m_bIsOpenSkillHeroicUI)
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillUI );
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillHighLevelUI );
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillHeroicUI );

			return TRUE;
		}

		if(pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX)
		{
			// 스킬창 닫아준다.
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillUI);
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);

			// 고렙 스킬창 연다.
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillHighLevelUI);

			return TRUE;
		}
		else if(pThis->m_lLastSetMasteryIndex == AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillUI);
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillHeroicUI);
		}

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataIsActiveInitialize);

		AgcdUI*	pUI = (AgcdUI*)pData1;
		if( pUI )
		{
			if( pUI->m_eStatus == AGCDUI_STATUS_CLOSING )
				pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_pcsAgcmUIManager2->m_nEventToggleUIClose );
			else
				pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_pcsAgcmUIManager2->m_nEventToggleUIOpen );
		}

		return pThis->SetMastery(pThis->m_lLastSetMasteryIndex);
	}
	else
	{
		// 단축키 이벤트에 기존 스킬창을 열도록 되어있다.. 닫아주자..
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillUI);

		// 열려있는 Dragon Scion 관련 창이 있다면 닫는다
		if( pThis->m_UIDragonScion.IsCurScionWindowOpen() )
		{
			pThis->m_UIDragonScion.CloseSkillUI();
		}
		else
		{
			// 드래곤시온용 스킬창을 연다.
			return pThis->m_UIDragonScion.OpenSkillUI( SkillUI_Scion );
		}
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBOpenSkillBuyUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();

	pThis->_MoveSkillWindow( TRUE );

	if (pcsSelfCharacter)
	{
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataIsActiveInitialize);
	}

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


	if( pThis->m_pcsAgpmFactors->GetRace( &pcsSelfCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
	{
		if( pThis->m_eRaceType	==	AURACE_TYPE_DRAGONSCION  )
		{
			return pThis->m_UIDragonScion.OpenSkillUpUI( SkillUI_Scion );	
		}
	}

	return pThis->SetBuyMastery(0);
}

BOOL AgcmUISkill2::CBOpenSkillUpgradeUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	return pThis->SetUpgradeMastery(0);
}

BOOL AgcmUISkill2::CBOpenSkillHighLevelUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	if( pThis->m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
	{
		// 기존에 사용하는 창 닫아주고..
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillBuyUI );
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillHighLevelUI );
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillHighLevelUpgradeUI );

		// Heroic close all related windows
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


		// 드래곤시온용 창 열어준다.
		pThis->m_UIDragonScion.OpenSkillUpUI( SkillUI_Scion );
		return TRUE;
	}

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);

	pThis->m_bIsOpenSkillHighLevelUI = TRUE;
	return pThis->SetMastery(AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX);
}

BOOL AgcmUISkill2::CBOpenSkillHighLevelSkillUpgradeUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 드래곤시온의 경우 기존 UI 창을 닫고 드래곤시온용 UI 창을 띄워준다.
	if( pThis->m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION &&
		pThis->m_eRaceType	==	AURACE_TYPE_DRAGONSCION )
	{
		// 기존에 사용하는 창 닫아주고..
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillBuyUI );
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillHighLevelUI );
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillHighLevelUpgradeUI );

		// Heroic close all related windows
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


		// 드래곤시온용 창 열어준다.
		pThis->m_UIDragonScion.OpenSkillUpUI( SkillUI_Scion );
		return TRUE;
	}

	// 고렙 일반 스킬창 닫아준다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


	pThis->_MoveSkillWindow( TRUE );
	pThis->m_bIsOpenSkillUpgradeHighLevelUI = TRUE;
	return pThis->SetUpgradeMasteryHighLevel();
}

BOOL AgcmUISkill2::CBOpenSkillHeroicUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	if( pThis->m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
	{
		// 기존에 사용하는 창 닫아주고..
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillBuyUI );
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillHighLevelUI );
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillHighLevelUpgradeUI );

		// Heroic close all related windows
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


		// 드래곤시온용 창 열어준다.
		pThis->m_UIDragonScion.OpenSkillUpUI( SkillUI_Scion );
		return TRUE;
	}

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);

	pThis->m_bIsOpenSkillHeroicUI = TRUE;
	return pThis->SetMastery(AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX);
}

BOOL AgcmUISkill2::CBOpenSkillHeroicSkillUpgradeUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 드래곤시온의 경우 기존 UI 창을 닫고 드래곤시온용 UI 창을 띄워준다.
	if( pThis->m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION &&
		pThis->m_eRaceType	==	AURACE_TYPE_DRAGONSCION )
	{
		// 기존에 사용하는 창 닫아주고..
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillBuyUI );
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillHighLevelUI );
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseSkillHighLevelUpgradeUI );

		// Heroic close all related windows
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUpgradeUI);


		// 드래곤시온용 창 열어준다.
		pThis->m_UIDragonScion.OpenSkillUpUI( SkillUI_Scion );
		return TRUE;
	}

	// 고렙 일반 스킬창 닫아준다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHighLevelUpgradeUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillBuyUI);

	// Heroic close all related windows
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillHeroicUI);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillHeroicUpgradeUI);

	pThis->_MoveSkillWindow( TRUE );
	pThis->m_bIsOpenSkillUpgradeHeroicUI = TRUE;
	return pThis->SetUpgradeMasteryHeroic();
}

BOOL AgcmUISkill2::CBCloseSkillHighLevelUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	pThis->m_bIsOpenSkillHighLevelUI = FALSE;
	return TRUE;
}

BOOL AgcmUISkill2::CBCloseSkillHeroicUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if( !pClass )
		return FALSE;

	AgcmUISkill2*	pThis			=	static_cast< AgcmUISkill2* >(pClass);
	AgpdCharacter*	pSelfCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pThis || !pSelfCharacter )
		return FALSE;

	pThis->m_bIsOpenSkillHeroicUI	=	FALSE;

	return TRUE;
}

BOOL AgcmUISkill2::CBSelectSkillGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgcdUIControl	*pcsUIControl	= (AgcdUIControl *)	pData1;

//	if (!pcsUIControl->m_pstUserData ||
//		!pcsUIControl->m_pstUserData->m_stUserData.m_pvData)
//		return FALSE;

	// 고렙 스킬인지 확인해야 한다. 2007.05.28. steeple
	AcUIGrid* pGrid = static_cast<AcUIGrid*>(pcsUIControl->m_pcsBase);
	if(pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData)
	{
		INT32 lSkillTID = pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;
		INT32 lCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
		INT32 lSkillIndex = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillIndex(lSkillTID, lCharTID);

		if(lSkillIndex >= 0 && lSkillIndex < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL)
		{
			// 모두 UnSelect 해주고
			for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventHighLevelUnSelectedItem[i]);

			pThis->m_lSelectedHighLevelSkillTID = lSkillTID;
			pThis->m_lSelectedItemIndex = (-1);

			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventHighLevelSelectedItem[lSkillIndex]);
			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataIsActiveInitialize);		// 개별초기화 벌튼 활성-비활성화

			return TRUE;
		}
	}

	if (pThis->m_lSelectedItemIndex >= 0)
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUnSelectedItem, pThis->m_lSelectedItemIndex);

	pThis->m_lSelectedItemIndex		= pcsSourceControl->m_lUserDataIndex;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelectedItem, pThis->m_lSelectedItemIndex);

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataIsActiveInitialize);

	return TRUE;
}

BOOL AgcmUISkill2::CBSelectBuySkillGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgcdUIControl	*pcsUIControl	= (AgcdUIControl *)	pData1;

//	if (!pcsUIControl->m_pstUserData ||
//		!pcsUIControl->m_pstUserData->m_stUserData.m_pvData)
//		return FALSE;

	// 고렙 스킬인지 확인해야 한다. 2007.05.28. steeple
	AcUIGrid* pGrid = static_cast<AcUIGrid*>(pcsUIControl->m_pcsBase);
	if(pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData)
	{
		INT32 lSkillTID = pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;
		INT32 lCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);
		INT32 lSkillIndex = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillIndex(lSkillTID, lCharTID);

		if(lSkillIndex >= 0 && lSkillIndex < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL)
		{
			pThis->m_lSelectedUpgradeHighLevelSkillTID = lSkillTID;

			// 모두 UnSelect 해주고
			for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpgradeHighLevelUnSelectedItem[i]);

			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpgradeHighLevelSelectedItem[lSkillIndex]);
			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataActiveUpgradeButton);		// 업그레이드 버튼 활성화-비활성화.

			pThis->m_lSelectedBuyItemIndex = (-1);
			pThis->m_lSelectedUpgradeItemIndex = (-1);

			return TRUE;
		}
	}

	if (pThis->m_lSelectedBuyItemIndex >= 0)
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyUnSelectedItem, pThis->m_lSelectedBuyItemIndex);

	pThis->m_lSelectedBuyItemIndex		= pcsSourceControl->m_lUserDataIndex;
	
	pThis->m_lSelectedUpgradeHighLevelSkillTID = 0;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuySelectedItem, pThis->m_lSelectedBuyItemIndex);

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataActiveBuyButton);

	return TRUE;
}

BOOL AgcmUISkill2::CBSelectUpgradeSkillGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgcdUIControl	*pcsUIControl	= (AgcdUIControl *)	pData1;

//	if (!pcsUIControl->m_pstUserData ||
//		!pcsUIControl->m_pstUserData->m_stUserData.m_pvData)
//		return FALSE;

	if (pThis->m_lSelectedUpgradeItemIndex >= 0)
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpgradeUnSelectedItem, pThis->m_lSelectedUpgradeItemIndex);

	pThis->m_lSelectedUpgradeItemIndex		= pcsSourceControl->m_lUserDataIndex;

	pThis->m_lSelectedUpgradeHighLevelSkillTID = 0;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpgradeSelectedItem, pThis->m_lSelectedUpgradeItemIndex);

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataActiveUpgradeButton);

	return TRUE;
}

BOOL AgcmUISkill2::CBBuySkill(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	if (!pThis->m_pcsEvent)
		return FALSE;

	if (pThis->m_lSelectedBuyItemIndex < 0)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotSelectBuySkill);
		return TRUE;
	}

	if (pThis->m_lSelectedBuyItemIndex < 0 || !pThis->m_pcsEvent)
		return FALSE;

	AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pThis->m_stSkillBuyListGrid[pThis->m_lSelectedBuyItemIndex], 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	AgpdSkill		*pcsSkill		= pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	if (pcsSkill)
	{
		return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventConfirmUpgradeSkill);
	}
	else
	{
		return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventConfirmBuySkill);
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBUpgradeSkill(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	if (!pThis->m_pcsEvent)
		return FALSE;

	if (pThis->m_lSelectedUpgradeItemIndex < 0 && pThis->m_lSelectedUpgradeHighLevelSkillTID == 0)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotSelectUpgradeSkill);
		return TRUE;
	}

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventConfirmUpgradeSkill);
}

BOOL AgcmUISkill2::CBUpdateListItemStatus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	INT32				i;
	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)	pClass;

	for (i = 0; i < pThis->m_pcsUserDataSkillGrid->m_stUserData.m_lCount; ++i)
	{
		if (pThis->m_lSelectedItemIndex == i)
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelectedItem, i);
		else
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUnSelectedItem, i);

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventActiveItem, i);
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBUpdateBuyListItemStatus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)	pClass;
	INT32				i;

	for (i = 0; i < pThis->m_pcsUserDataSkillBuyGrid->m_stUserData.m_lCount; ++i)
	{
		if (pThis->m_lSelectedBuyItemIndex == i)
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuySelectedItem, i);
		else
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyUnSelectedItem, i);

		pThis->SetStatusActiveBuyItem(i);
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBUpdateUpgradeListItemStatus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)	pClass;
	INT32				i;

	for (i = 0; i < pThis->m_pcsUserDataSkillUpgradeGrid->m_stUserData.m_lCount; ++i)
	{
		if (pThis->m_lSelectedUpgradeItemIndex == i)
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpgradeSelectedItem, i);
		else
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpgradeUnSelectedItem, i);

		pThis->SetStatusActiveUpgradeItem(i);
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBCloseSkillEventUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;
	
	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)	pClass;

	pThis->m_bIsOpenSkillEventUI	= FALSE;

	return TRUE;
}

BOOL AgcmUISkill2::CBToggleSkillUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)	pClass;
	AgpdCharacter* ppdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	if (pThis->m_bIsOpenSkillUI ||
		pThis->m_bIsOpenSkillProductUI)
	{
		pThis->m_bIsOpenSkillUI			= FALSE;
		pThis->m_bIsOpenSkillProductUI	= FALSE;

		if( pThis->m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) != AURACE_TYPE_DRAGONSCION )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillNPCDialogUI);
		}
		else
		{
			pThis->m_UIDragonScion.CloseSkillUI();
		}
	}
	else
	{
		pThis->m_bIsOpenSkillUI			= TRUE;
		pThis->m_bIsOpenSkillProductUI	= FALSE;

		if( pThis->m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) != AURACE_TYPE_DRAGONSCION )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillUI);
		}
		else
		{
			pThis->m_UIDragonScion.OpenSkillUI( SkillUI_Scion );
		}
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBSkillInitialize(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)	pClass;
	AgpdCharacter		*pcsCharacter		=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();

	// Dragon Scion 예외 처리
	if( pThis->m_pcsAgpmFactors->GetRace( &pcsCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
	{
		// 진화상태라면
		if( pThis->m_pcsAgpmFactors->GetClass( &pcsCharacter->m_csFactor ) != AUCHARCLASS_TYPE_SCION )
		{			
			pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( pThis->m_pcsAgcmUIManager2->GetUIMessage( "IsEvolutionNotUseInitSkillItem" ) );
			return FALSE;
		}
	}

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventConfirmSkillInitialize);

	return TRUE;
}

BOOL AgcmUISkill2::CBSetFailInitializeText(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pData1 || !pClass)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)	pClass;
	AgcdUIControl		*pcsEditControl		= (AgcdUIControl *)	pData1;

	CHAR	szBuffer[1024];
	ZeroMemory(szBuffer, sizeof(CHAR) * 1024);

	sprintf(szBuffer,
			pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SKILL_FAIL_INITIALIZE));

	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);

	return TRUE;
}

BOOL AgcmUISkill2::CBSetConfirmInitializeText(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pData1 || !pClass)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)	pClass;
	AgcdUIControl		*pcsEditControl		= (AgcdUIControl *)	pData1;

	CHAR	szBuffer[1024];
	ZeroMemory(szBuffer, sizeof(CHAR) * 1024);

	sprintf(szBuffer,
			pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SKILL_CONFIRM_INITIALIZE));

	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);

	return TRUE;
}

BOOL AgcmUISkill2::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)	pData;

	pThis->m_pcsSelfCharacter	= pcsCharacter;

	INT32	alIndex[AGPMEVENT_SKILL_MAX_MASTERY];

	ZeroMemory(alIndex, sizeof(INT32) * AGPMEVENT_SKILL_MAX_MASTERY);

	AgpdSkillAttachData	*pcsSkillAttachData	= pThis->m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);
	INT32				i;

	for (i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		AgpdSkill	*pcsSkill	= pThis->m_pcsAgpmSkill->GetSkill(pcsSkillAttachData->m_alSkillID[i]);
		if (!pcsSkill)
			break;

		// 아크로드 스킬은 따로 관리한다.
		if(pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			continue;

		// 고레벨 스킬도 따로 관리한다.
		if(pThis->m_pcsAgpmEventSkillMaster->IsHighLevelSkill(pcsSkill->m_pcsTemplate->m_lID, pcsCharacter->m_pcsCharacterTemplate->m_lID))
			continue;

		// 히로익 스킬도 따로 관리한다.
		if(pThis->m_pcsAgpmEventSkillMaster->IsHeroicSkill(pcsSkill->m_pcsTemplate->m_lID , pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID))
			continue;

		INT32	lMasteryIndex	= pThis->m_pcsAgpmEventSkillMaster->GetMasteryIndex((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);		// 일단 없으니 0으로 걍 세팅해버린다.

		pThis->m_lSkillMasteryList[lMasteryIndex][alIndex[lMasteryIndex]++]	= pcsSkill->m_lID;

		if (alIndex[lMasteryIndex] >= AGPMEVENT_SKILL_MAX_MASTERY_SKILL)
			break;
	}

	pThis->m_lSkillPoint		= pThis->m_pcsAgpmCharacter->GetSkillPoint(pcsCharacter);

	// 2006.03.14. steeple
	// 다시 세팅해준다.
	pThis->SetMastery(pThis->m_pcsUserDataActiveSkillTab->m_lSelectedIndex);
	
	// 2006.11.08. steeple
	// 이거 불러주는 것이 좋다.
	pThis->RefreshUsableOwnSkill(pThis->m_pcsSelfCharacter);

	return TRUE;
}

BOOL AgcmUISkill2::CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	INT32				i;

	pThis->m_pcsSelfCharacter		= NULL;

	for (i = 0; i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL; ++i)
	{
		pThis->m_pcsAgpmGrid->Reset(&pThis->m_stSkillListGrid[i]);
		pThis->m_pcsAgpmGrid->Reset(&pThis->m_stSkillBuyListGrid[i]);
		pThis->m_pcsAgpmGrid->Reset(&pThis->m_stSkillUpgradeListGrid[i]);
	}

	for (i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_SKILL; ++i)
	{
		pThis->m_pcsAgpmGrid->Reset(&pThis->m_stSkillHighLevelGrid[i]);
	}

	ZeroMemory(pThis->m_lSkillMasteryList, sizeof(INT32) * AGPMEVENT_SKILL_MAX_MASTERY * AGPMEVENT_SKILL_MAX_MASTERY_SKILL);

	return TRUE;
}

BOOL AgcmUISkill2::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2			*pThis				= (AgcmUISkill2 *)			pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	if (!pThis->m_bIsOpenSkillEventUI)
		return TRUE;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pThis->m_stOpenSkillUIPos);

	if ((INT32) fDistance < AGCMUISKILL2_CLOSE_UI_DISTANCE)
		return TRUE;

	pThis->m_bIsOpenSkillEventUI	= FALSE;

	if( pThis->m_pcsAgpmFactors->GetRace( &pcsCharacter->m_csFactor ) != AURACE_TYPE_DRAGONSCION )
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseSkillNPCDialogUI);
	else
		pThis->m_UIDragonScion.CloseSkillUpUI();

	return TRUE;
}

BOOL AgcmUISkill2::CBAddSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdSkill		*pcsSkill		= (AgpdSkill *)		pData;
	INT32			i;

	if (!pThis->m_pcsSelfCharacter)
		return TRUE;

	// 생산 스킬이면 넘어가자. 다른데 들어간다.
	if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
		return TRUE;

	// 아크로드 스킬은 따로 관리한다.
	if (pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
		return pThis->AddArchlordSkill(pcsSkill);

	// 고레벨 스킬도 따로 관리한다.
	if(pThis->m_pcsAgpmEventSkillMaster->IsHighLevelSkill(pcsSkill->m_pcsTemplate->m_lID, pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID))
		return pThis->AddHighLevelSkill(pcsSkill);

	// 히로익 스킬도 따로 관리한다.
	if(pThis->m_pcsAgpmEventSkillMaster->IsHeroicSkill(pcsSkill->m_pcsTemplate->m_lID , pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID))
		return pThis->AddHeroicSkill(pcsSkill);

	INT32	lMasteryIndex	= pThis->m_pcsAgpmEventSkillMaster->GetMasteryIndex((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);		// 일단 없으니 0으로 걍 세팅해버린다.

	for (i = 0; i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL; ++i)
	{
		if (pThis->m_lSkillMasteryList[lMasteryIndex][i] == AP_INVALID_SKILLID)
		{
			pThis->m_lSkillMasteryList[lMasteryIndex][i]	= pcsSkill->m_lID;
			break;
		}
	}

	if (i == AGPMEVENT_SKILL_MAX_MASTERY_SKILL)
		return FALSE;

	pThis->SetMastery(lMasteryIndex);
	//pThis->SetUpgradeMastery(lMasteryIndex);
	pThis->SetBuyMastery(lMasteryIndex);

	pThis->RefreshUsableOwnSkill(pThis->m_pcsSelfCharacter);

	/*
	if (pThis->m_pcsAgpmSkill->IsPassiveSkill(pcsSkill))
	{
		AgcmUIUpdateCharStatus	stUpdateCharStatus;
		ZeroMemory(&stUpdateCharStatus, sizeof(AgcmUIUpdateCharStatus));

		pThis->SetUpdateCharStatus(pcsSkill->m_pcsTemplate->m_lID, &stUpdateCharStatus);

		pThis->m_pcsAgcmUICharacter->SetUpdateCharStatus(stUpdateCharStatus);
	}
	*/

	return TRUE;
}

BOOL AgcmUISkill2::CBRemoveSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdSkill		*pcsSkill		= (AgpdSkill *)		pData;
	INT32			i;

	if (!pThis->m_pcsSelfCharacter)
		return TRUE;

	// 아크로드 스킬은 따로 관리한다.
	if (pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
		return pThis->RemoveArchlordSkill(pcsSkill);

	// 고레벨 스킬도 따로 관리한다.
	if(pThis->m_pcsAgpmEventSkillMaster->IsHighLevelSkill(pcsSkill->m_pcsTemplate->m_lID, pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID))
		return pThis->RemoveHighLevelSkill(pcsSkill);

	// 히로익 스킬도 따로 관리한다.
	if(pThis->m_pcsAgpmEventSkillMaster->IsHeroicSkill(pcsSkill->m_pcsTemplate->m_lID , pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID))
		return pThis->RemoveHeroicSkill(pcsSkill);

	INT32	lMasteryIndex	= pThis->m_pcsAgpmEventSkillMaster->GetMasteryIndex((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);		// 일단 없으니 0으로 걍 세팅해버린다.

	for (i = 0; i < AGPMEVENT_SKILL_MAX_MASTERY_SKILL; ++i)
	{
		if (pThis->m_lSkillMasteryList[lMasteryIndex][i] == pcsSkill->m_lID)
		{
			pThis->m_lSkillMasteryList[lMasteryIndex][i]	= AP_INVALID_SKILLID;
			break;
		}
	}

	if (i == AGPMEVENT_SKILL_MAX_MASTERY_SKILL)
		return FALSE;

	pThis->SetMastery(lMasteryIndex);
	pThis->SetUpgradeMastery(lMasteryIndex);

	if (pThis->m_pcsAgcmUIMain)
		pThis->m_pcsAgcmUIMain->RemoveGridItemFromQuickBelt(pcsSkill->m_pcsGridItem);

	/*
	if (pThis->m_pcsAgpmSkill->IsPassiveSkill(pcsSkill))
	{
		AgcmUIUpdateCharStatus	stUpdateCharStatus;
		ZeroMemory(&stUpdateCharStatus, sizeof(AgcmUIUpdateCharStatus));

		pThis->SetUpdateCharStatus(pcsSkill->m_pcsTemplate->m_lID, &stUpdateCharStatus);

		pThis->m_pcsAgcmUICharacter->ResetUpdateCharStatus(stUpdateCharStatus);
	}
	*/

	return TRUE;
}

BOOL AgcmUISkill2::AddHighLevelSkill(AgpdSkill* pcsSkill)
{
	if(!pcsSkill)
		return FALSE;

	SetMasteryHighLevel(AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX);
	SetUpgradeMasteryHighLevel();

	return TRUE;
}

BOOL AgcmUISkill2::RemoveHighLevelSkill(AgpdSkill* pcsSkill)
{
	if(!pcsSkill)
		return FALSE;

	if(m_bIsOpenSkillHighLevelUI)
		SetMasteryHighLevel(AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX);
	else if(m_bIsOpenSkillUpgradeHighLevelUI)
		SetUpgradeMasteryHighLevel();

	if(m_pcsAgcmUIMain)
		m_pcsAgcmUIMain->RemoveGridItemFromQuickBelt(pcsSkill->m_pcsGridItem);
	RefreshUsableOwnSkill(m_pcsSelfCharacter);

	return TRUE;
}

BOOL AgcmUISkill2::AddArchlordSkill(AgpdSkill* pcsSkill)
{
	if(!pcsSkill)
		return FALSE;

	SetMastery(AGPMEVENT_SKILL_ARCHLORD_MASTERY_INDEX);
	RefreshUsableOwnSkill(m_pcsSelfCharacter);

	return TRUE;
}

BOOL AgcmUISkill2::RemoveArchlordSkill(AgpdSkill* pcsSkill)
{
	if(!pcsSkill)
		return FALSE;

	SetMastery(AGPMEVENT_SKILL_ARCHLORD_MASTERY_INDEX);
	if(m_pcsAgcmUIMain)
		m_pcsAgcmUIMain->RemoveGridItemFromQuickBelt(pcsSkill->m_pcsGridItem);
	RefreshUsableOwnSkill(m_pcsSelfCharacter);

	return TRUE;
}

BOOL AgcmUISkill2::AddHeroicSkill(AgpdSkill* pcsSkill)
{
	if( !pcsSkill )
		return FALSE;

	SetMastery( AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX );
	SetUpgradeMasteryHeroic();
	return TRUE;
}

BOOL AgcmUISkill2::RemoveHeroicSkill(AgpdSkill* pcsSkill)
{
	if( !pcsSkill )
		return FALSE;

	if(m_bIsOpenSkillHeroicUI)
		SetMasteryHighLevel(AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX);
	else if(m_bIsOpenSkillUpgradeHeroicUI)
		SetUpgradeMasteryHeroic();

	if(m_pcsAgcmUIMain)
		m_pcsAgcmUIMain->RemoveGridItemFromQuickBelt(pcsSkill->m_pcsGridItem);

	RefreshUsableOwnSkill(m_pcsSelfCharacter);

	return TRUE;
}

BOOL AgcmUISkill2::CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	INT32				i;
	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)		pClass;
	AgpdSkill			*pcsSkill			= (AgpdSkill *)			pData;

	if (!pThis->m_pcsSelfCharacter)
		return FALSE;

	// pcsSkill의 Skill Point가 변경되었다.
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataSkillGrid);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataSkillUpgradeGrid);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataSkillPoint);

	pThis->m_pcsAgcmUIManager2->RefreshUserData(pThis->m_pcsUserDataSkillGrid);
	pThis->m_pcsAgcmUIManager2->RefreshUserData(pThis->m_pcsUserDataSkillUpgradeGrid);
	pThis->m_pcsAgcmUIManager2->RefreshUserData(pThis->m_pcsUserDataSkillPoint);

	for (i = 0; i < pThis->m_pcsUserDataSkillGrid->m_stUserData.m_lCount; ++i)
	{
		if (pThis->m_lSelectedItemIndex == i)
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelectedItem);
		else
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUnSelectedItem, i);

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventActiveItem, i);
	}

	for (i = 0; i < pThis->m_pcsUserDataSkillUpgradeGrid->m_stUserData.m_lCount; ++i)
	{
		if (pThis->m_lSelectedUpgradeItemIndex == i)
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpgradeSelectedItem);
		else
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUpgradeUnSelectedItem, i);

		pThis->SetStatusActiveUpgradeItem(i);
	}

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataActiveBuyButton);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataActiveUpgradeButton);

	return TRUE;
}

BOOL AgcmUISkill2::CBCheckCastSkillResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2			*pThis				= (AgcmUISkill2 *)					pClass;
	eAgpmSkillActionType	eActionType			= (eAgpmSkillActionType) (INT32)	pData;

	switch (eActionType) {
	case AGPMSKILL_ACTION_MISS_CAST_SKILL:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultMiss);
		break;

	case AGPMSKILL_ACTION_FAILED_CAST:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultFailed);
		break;

	case AGPMSKILL_ACTION_INVALID_TARGET:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultInvalidTarget);
		break;

	case AGPMSKILL_ACTION_NEED_REQUIREMENT:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultNeedRequirement);
		break;

	case AGPMSKILL_ACTION_NOT_ENOUGH_HP:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultNotEnoughHP);
		break;

	case AGPMSKILL_ACTION_NOT_ENOUGH_MP:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultNotEnoughMP);
		break;

	case AGPMSKILL_ACTION_NOT_ENOUGH_SP:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultNotEnoughSP);
		break;

	case AGPMSKILL_ACTION_NOT_ENOUGH_ARROW:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultNotEnoughArrow);
		break;

	case AGPMSKILL_ACTION_ALREADY_USE_SKILL:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultAlreadyUse);
		break;

	case AGPMSKILL_ACTION_NOT_READY_CAST:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultNotReadyCast);
		break;

	case AGPMSKILL_ACTION_CAST_SKILL:

		break;

	case AGPMSKILL_INVALID_TARGET_TO_GATHER:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultInvalidTargetToGather);
		break;

	case AGPMSKILL_NOT_ENOUGH_PRODUCT:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCastResultNotEnoughProduct);
		break;
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBReceiveSkillAction(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2				*pThis					= (AgcmUISkill2 *)			pClass;
	pstAgpmSkillActionData		pstActionData			= (pstAgpmSkillActionData)	pData;

	AgpdCharacter				*pcsSelfCharacter		= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter ||
		pcsSelfCharacter->m_eType != pstActionData->pcsOwnerBase->m_eType ||
		pcsSelfCharacter->m_lID != pstActionData->pcsOwnerBase->m_lID)
		return TRUE;

	switch (pstActionData->nActionType) {
	case AGPMSKILL_ACTION_CAST_SKILL:
	case AGPMSKILL_ACTION_MISS_CAST_SKILL:
		pThis->SetRecastSkillTimeToGridItem(pstActionData->lSkillID, pstActionData->ulDuration, pstActionData->ulRecastDelay);
		break;
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBAddBuffedList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkill2		*pThis			= (AgcmUISkill2 *)			pClass;
	ApBase				*pcsBase		= (ApBase *)				pData;
	INT32				lTID			= (INT32)					pCustData;

	AgpdCharacter				*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return TRUE;

	if (pcsBase->m_eType	!= pcsSelfCharacter->m_eType ||
		pcsBase->m_lID		!= pcsSelfCharacter->m_lID)
		return TRUE;

	/*
	// 어떤 스탯들이 변화되었는지 캐릭터 스탯창에 변화를 준다.
	AgcmUIUpdateCharStatus	stUpdateCharStatus;
	ZeroMemory(&stUpdateCharStatus, sizeof(AgcmUIUpdateCharStatus));

	pThis->SetUpdateCharStatus(lTID, &stUpdateCharStatus);

	pThis->m_pcsAgcmUICharacter->SetUpdateCharStatus(stUpdateCharStatus);
	*/

	return pThis->m_pcsAgcmUIMain->UpdateAlarmGrid();
}

BOOL AgcmUISkill2::CBRemoveBuffedList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkill2		*pThis			= (AgcmUISkill2 *)			pClass;
	INT32				lTID			= (INT32)					pCustData;

	/*
	// 캐릭터 스탯창 초기화 해준다.
	AgcmUIUpdateCharStatus	stUpdateCharStatus;
	ZeroMemory(&stUpdateCharStatus, sizeof(AgcmUIUpdateCharStatus));

	pThis->SetUpdateCharStatus(lTID, &stUpdateCharStatus);

	pThis->m_pcsAgcmUICharacter->ResetUpdateCharStatus(stUpdateCharStatus);
	*/

	return pThis->m_pcsAgcmUIMain->UpdateAlarmGrid();
}

BOOL AgcmUISkill2::CBConfirmUseSkillBook(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)		pClass;
	AgpdItem			*pcsSkillBook		= (AgpdItem *)			pData;

	pThis->m_pcsUseSkillBook	= pcsSkillBook;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventConfirmLearnSkill);
}

BOOL AgcmUISkill2::CBSetItemTooltip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)		pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)			pData;
	AcUIToolTip			*pcsToolTip			= (AcUIToolTip *)		pCustData;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE)
	{
		if (((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_BOOK)
		{
			pThis->SetSkillBookTooltip(pcsItem, pcsToolTip);
		}
		else if (((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL)
		{
			pThis->SetSkillScrollTooltip(pcsItem, pcsToolTip);
		}
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBUpdateAlarmGrid(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)	pClass;
	INT32				i;

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData		= pThis->m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsSelfCharacter);
	if (!pcsAttachData)
		return FALSE;

	for (i = 0; i < AGPMSKILL_MAX_SKILL_BUFF; i++)
	{
		AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(pcsAttachData->m_astBuffSkillList[i].lSkillTID);
		if (pcsSkillTemplate)
		{
			AgcdSkillTemplate	*pcsAgcdSkillTemplate	= pThis->m_pcsAgcmSkill->GetADSkillTemplate(pcsSkillTemplate);
			
			//if (!pcsAgcdSkillTemplate->m_pSmallSkillTexture)
				pThis->m_pcsAgcmSkill->SetGridSkillAttachedSmallTexture(pcsSkillTemplate);

			pThis->m_pcsAgcmUIMain->AddItemToAlarmGrid(pcsSkillTemplate->m_pcsGridItemAlarm);

			if (pcsAttachData->m_astBuffSkillList[i].ulEndTimeMSec > 0 &&
				pcsAttachData->m_astBuffSkillList[i].ulEndTimeMSec - AGCMUISKILL2_MAX_BLINK_TIME <= pThis->GetPrevClockCount() &&
				pThis->m_pcsAgpmSkill->IsDurationByDistanceSkill(pcsSkillTemplate) == FALSE &&
				pThis->m_pcsAgpmSkill->IsDurationUnlimited(pcsSkillTemplate) == FALSE)
			{
				// 아직 깜박이고 있지 않은 넘이면 깜박이게 하라고 세팅한다.
				//  번쩍! 번쩍! 번쩍!
				//
				//
				//
				//
				//
				//
				//

				pcsSkillTemplate->m_pcsGridItemAlarm->m_bTwinkleMode	= TRUE;
			}
			else
			{
				// 깜박이고 있는 상태라면 안하라고 세팅한다.
				//  안번쩍! 안번쩍! 안번쩍!
				//
				//
				//
				//
				//
				//
				//

				pcsSkillTemplate->m_pcsGridItemAlarm->m_bTwinkleMode	= FALSE;
			}
		}
		else
			break;
	}

	return TRUE;
}

/*
BOOL AgcmUISkill2::CBRefreshUpdateCharStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2				*pThis					= (AgcmUISkill2 *)			pClass;
	AgpdCharacter				*pcsCharacter			= (AgpdCharacter *)			pData;

	// 어떤 스탯들이 변화되었는지 캐릭터 스탯창에 변화를 준다.
	AgcmUIUpdateCharStatus	stUpdateCharStatus;

	AgpdSkillAttachData			*pcsAttachData			= pThis->m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);
	INT32				i;

	for (i = 0; i < AGPMSKILL_MAX_SKILL_BUFF; ++i)
	{
		ZeroMemory(&stUpdateCharStatus, sizeof(AgcmUIUpdateCharStatus));

		pThis->SetUpdateCharStatus(pcsAttachData->m_astBuffSkillList[i].lSkillTID, &stUpdateCharStatus);

		pThis->m_pcsAgcmUICharacter->SetUpdateCharStatus(stUpdateCharStatus);
	}

	for (i = 0; i < AGPMSKILL_MAX_SKILL_USE; ++i)
	{
		AgpdSkill				*pcsSkill				= pThis->m_pcsAgpmSkill->GetSkill(pcsAttachData->m_alUsePassiveSkillID[i]);
		if (pcsSkill)
		{
			ZeroMemory(&stUpdateCharStatus, sizeof(AgcmUIUpdateCharStatus));

			pThis->SetUpdateCharStatus(pcsSkill->m_pcsTemplate->m_lID, &stUpdateCharStatus);

			pThis->m_pcsAgcmUICharacter->SetUpdateCharStatus(stUpdateCharStatus);
		}
	}

	return TRUE;
}
*/

BOOL AgcmUISkill2::CBUpdateCharFactorSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2			*pThis			= (AgcmUISkill2 *)		pClass;
	AgpdFactor			*pcsFactor			= (AgpdFactor *)		pData;

	if (!pThis->m_pcsSelfCharacter)
		return FALSE;

	INT32				lOwnerID			= 0;

	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lOwnerID, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_ID);

	if (lOwnerID == 0)
		return FALSE;

	if (pThis->m_pcsSelfCharacter->m_lID == lOwnerID)
	{
		pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &pThis->m_lSkillPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_POINT);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataSkillPoint);
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkill2		*pThis				= (AgcmUISkill2 *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	INT16				nOldStatus			= *(INT16 *)			pCustData;
	INT32				i;

	if (nOldStatus == AGPDCHAR_STATUS_DEAD &&
		pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
	{
		AgpdSkillAttachData	*pcsAttachData		= pThis->m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);
		if (!pcsAttachData)
			return FALSE;

		for (i = 0; i < AGPMSKILL_MAX_SKILL_OWN; i++)
		{
			AgpdSkill	*pcsSkill	= pThis->m_pcsAgpmSkill->GetSkill(pcsAttachData->m_alSkillID[i]);
			if (pcsSkill)
			{
				pcsSkill->m_pcsGridItem->SetUseItemTime(0, 0);			
			}
		}
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBUpdateCharSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	pThis->m_lSkillPoint	= pThis->m_pcsAgpmCharacter->GetSkillPoint(pcsCharacter);

	return pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataSkillPoint);
}

BOOL AgcmUISkill2::CBUpdateInvenMoney(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	pThis->m_pcsAgpmCharacter->GetMoney(pcsCharacter, &pThis->m_llInvenMoney);

	return pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataInvenMoney);
}

BOOL AgcmUISkill2::CBResponseGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	ApdEvent		*pcsEvent		= (ApdEvent *)		ppvBuffer[0];
	if (!pcsEvent || !pcsEvent->m_pvData)
		return FALSE;

	AgpdSkillEventAttachData	*pcsAttachData	= (AgpdSkillEventAttachData *) pcsEvent->m_pvData;

	pThis->m_eRaceType	= pcsAttachData->eRaceType;
	pThis->m_eClassType	= pcsAttachData->eClassType;

	pThis->m_pcsEvent	= pcsEvent;

	pThis->m_bIsOpenSkillEventUI		= TRUE;
	pThis->m_stOpenSkillUIPos	= pcsCharacter->m_stPos;

	pThis->m_bIsActiveInitialize	= !((BOOL) ppvBuffer[2]);

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenSkillNPCDialogUI);
}

BOOL AgcmUISkill2::CBUpdateCost(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdFactor		*pcsFactor		= (AgpdFactor *)	pData;

	INT32			lOwnerCID		= AP_INVALID_CID;

	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lOwnerCID, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_ID);

	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	// 자기 자신일 경우만 처리한다.
	if (pcsSelfCharacter->m_lID != lOwnerCID)
		return TRUE;

	return pThis->RefreshUsableOwnSkill(pcsSelfCharacter);
}

BOOL AgcmUISkill2::CBUpdateItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsItem->m_pcsCharacter)
		return TRUE;

	return pThis->RefreshUsableOwnSkill(pcsItem->m_pcsCharacter);
}

BOOL AgcmUISkill2::RefreshUsableOwnSkill(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (m_bIsDestroy)
		return TRUE;

	INT32				i;
	AgpdSkillAttachData	*pcsAttachData		= m_pcsAgpmSkill->GetAttachSkillData((ApBase *) pcsCharacter);
	if (!pcsAttachData)
		return FALSE;

	for (i = 0; i < AGPMSKILL_MAX_SKILL_OWN; i++)
	{
		AgpdSkill	*pcsSkill	= pcsAttachData->m_apcsSkill[i];
		if (pcsSkill)
		{
			if( IsUsableSkill( pcsSkill ) && IsEnableSkill( pcsSkill ) )
			{
				m_pcsAgcmSkill->SetGridSkillAttachedTexture( pcsSkill );
			}
			else
			{
				m_pcsAgcmSkill->SetGridSkillAttachedUnableTexture( pcsSkill );
			}
		}
	}

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataSkillGrid);
	m_pcsAgcmUIMain->RefreshQuickBeltGrid();

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPoint(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;

	sprintf(szDisplay, "%d", pThis->m_lSkillPoint);

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;
	AgpdGrid		*pcsGrid	= (AgpdGrid *)		pData;

	if (pcsGrid->m_lGridType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	AgpdSkill		*pcsSkill	= pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	if (pcsSkill)
	{
		sprintf(szDisplay, "%s", ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_szName);
	}
	else
	{
		AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(pcsGridItem->m_lItemTID);
		if (pcsSkillTemplate)
		{
			sprintf(szDisplay, "%s", pcsSkillTemplate->m_szName);
		}
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillLevel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdGrid		*pcsGrid		= (AgpdGrid *)		pData;

	if (pcsGrid->m_lGridType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	AgpdSkill		*pcsSkill		= pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	if (pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;
		if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
			!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
			!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
		{
			lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
			if(pcsSkill->m_pcsTemplate)
				lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);

			lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
		}

		if (lID == AGCMUI_SKILL2_DISPLAY_ID_SKILL_LEVEL)
		{
			// Self Skill Window
			//

			if(lModifiedSkillLevel == 0)
				sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill));
			else
				sprintf(szDisplay, "%d (+%d)", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel, lModifiedSkillLevel);
		}
		else if (lID == AGCMUI_SKILL2_DISPLAY_ID_SKILL_LEVEL_2)
		{
			// NPC Skill Window
			//

			//if(lModifiedSkillLevel == 0)
				sprintf(szDisplay, "%s %d", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SKILL_LEVEL), pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill));
			//else
			//	sprintf(szDisplay, "%s %d (+%d)", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SKILL_LEVEL),
			//										pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel,
			//										lModifiedSkillLevel);
		}
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdGrid		*pcsGrid		= (AgpdGrid *)		pData;

	if (pcsGrid->m_lGridType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	INT32	lCost	= 0;

	if (pcsGridItem->m_lItemID != AP_INVALID_SKILLID &&
		pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID))
	{
		lCost	= pThis->m_pcsAgpmEventSkillMaster->GetUpgradeCost(pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID), pThis->m_pcsAgcmCharacter->GetSelfCharacter());
	}
	else
	{
		lCost	= pThis->m_pcsAgpmEventSkillMaster->GetBuyCost(pcsGridItem->m_lItemTID, pThis->m_pcsAgcmCharacter->GetSelfCharacter());
	}

	if (lCost < 0)
		lCost	= 0;

	if (AP_SERVICE_AREA_WESTERN == g_eServiceArea)
		sprintf(szDisplay, "%dG", lCost, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME));
	else
		sprintf(szDisplay, "%d%s", lCost, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME));

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillTrainerClass(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	sprintf(szDisplay, "%s %s", pThis->m_pcsAgpmFactors->GetCharacterRaceName(pThis->m_eRaceType),
								pThis->m_pcsAgpmFactors->GetCharacterClassName(pThis->m_eRaceType, pThis->m_eClassType));

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillMainDialog(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !szDisplay || !pcsSourceControl)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	if (pcsSourceControl->m_lType == AcUIBase::TYPE_EDIT)
		((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SKILL_MAIN_DIALOG_MESSAGE));

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplayInvenMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	sprintf(szDisplay, "%I64d", pThis->m_llInvenMoney);

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillBuyButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SKILL_BUY));

	if (pThis->m_lSelectedBuyItemIndex < 0 && pThis->m_lSelectedUpgradeHighLevelSkillTID == 0)
		return TRUE;

	AgpdSkill* pcsSkill = NULL;

	if(pThis->m_lSelectedUpgradeHighLevelSkillTID == 0)
	{
		AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pThis->m_stSkillBuyListGrid[pThis->m_lSelectedBuyItemIndex], 0, 0, 0);
		if (!pcsGridItem)
			return TRUE;

		pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	}
	else
	{
		// 고렙스킬
		pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_lSelectedUpgradeHighLevelSkillTID);
	}

	if(!pcsSkill)
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SKILL_BUY));
	else
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SKILL_UPGRADE));

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySelectedSkillName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	if (pThis->m_lSelectedItemIndex < 0 && pThis->m_lSelectedHighLevelSkillTID == 0)
		return FALSE;

	AgpdSkill* pcsSkill = NULL;

	if(pThis->m_lSelectedHighLevelSkillTID == 0)
	{
		AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pThis->m_stSkillListGrid[pThis->m_lSelectedItemIndex], 0, 0, 0);
		if (!pcsGridItem)
			return FALSE;

		pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	}
	else
	{
		// 고렙스킬
		pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_lSelectedHighLevelSkillTID);
	}

	if(!pcsSkill)
		return FALSE;

	sprintf(szDisplay, "%s", ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_szName);

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel0(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(0, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(1, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(2, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(3, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(4, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(5, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);
			
				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel6(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(6, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel7(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(7, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel8(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(8, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel9(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(9, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

BOOL AgcmUISkill2::CBDisplaySkillPointHighLevel10(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);

	if(!pThis->m_pcsSelfCharacter || !pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate)
		return FALSE;

	INT32 lSelectedCharTID = 0;
	if(pThis->m_bIsOpenSkillHighLevelUI)
		lSelectedCharTID = pThis->m_pcsSelfCharacter->m_pcsCharacterTemplate->m_lID;
	else
		lSelectedCharTID = pThis->m_pcsAgpmEventSkillMaster->GetCharTID(pThis->m_eRaceType, pThis->m_eClassType);

	AgpdEventSkillHighLevel stHighLevelSkill = pThis->m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfoByIndex(10, lSelectedCharTID);

	AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(pcsSkill)
	{
		INT32 lModifiedSkillLevel = 0;

		// 일반 스킬윈도에서만 ModifiedSkillLevel 더해준다.. 2007.07.09. steeple
		if(pThis->m_bIsOpenSkillHighLevelUI)
		{
			if(pThis->m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) &&
				!pThis->m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				lModifiedSkillLevel = pThis->m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pThis->m_pcsAgcmCharacter->GetSelfCharacter());
				if(pcsSkill->m_pcsTemplate)
					lModifiedSkillLevel += pThis->m_pcsAgpmItem->GefEffectedSkillPlusLevel(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkill->m_pcsTemplate->m_lID);
			
				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		sprintf(szDisplay, "%d", pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill) + lModifiedSkillLevel);
	}
	else
		_tcscpy(szDisplay, "0");

	return TRUE;
}

// ## Product
BOOL AgcmUISkill2::CBDisplaySkillLevelExp(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdGrid		*pcsGrid		= (AgpdGrid *)		pData;

	if (pcsGrid->m_lGridType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	AgpdSkill		*pcsSkill		= pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	if (pcsSkill)
	{
		INT32 lSkillLevel = 0;//pThis->m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
		FLOAT fExpDegree = 0;//pThis->m_pcsAgpmSkill->GetProductExpDegree(pcsSkill);
		sprintf(szDisplay, "%s: %3d  %s: %4.1f%",
						ClientStr().GetStr(STI_LEVEL),
						lSkillLevel,
						ClientStr().GetStr(STI_MASTERY),
						fExpDegree);
	}

	return TRUE;
}


BOOL AgcmUISkill2::CBDisplaySkillExpDegree(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !pData || !szDisplay || !plValue)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;
	AgpdGrid		*pcsGrid		= (AgpdGrid *)		pData;
	INT32			lTemp1 = 0;
	INT32			lTemp2 = 0;

	if (pcsGrid->m_lGridType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	AgpdSkill		*pcsSkill		= pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	if (pcsSkill)
	{
		switch (lID)
		{	
			case AGCMUI_SKILL2_DISPLAY_ID_SKILL_EXP_DEGREE:
				lTemp1 = 0;//pThis->m_pcsAgpmSkill->GetCurExpOfLevel(pcsSkill);
				if (0 > lTemp1)
					lTemp1 = 0;

				lTemp2 = 0;//pThis->m_pcsAgpmSkill->GetMaxExpOfLevel(pcsSkill);
				if (lTemp1 > lTemp2)
					lTemp1 = lTemp2;

				*plValue = lTemp1;
				sprintf(szDisplay, "%d", *plValue);
				break;
			case AGCMUI_SKILL2_DISPLAY_ID_SKILL_EXP_DEGREE_MAX:
				lTemp1 = 0;//pThis->m_pcsAgpmSkill->GetMaxExpOfLevel(pcsSkill);
				*plValue = lTemp1;
				sprintf(szDisplay, "%d", *plValue);
				break;
			default:
				break;
		}
	}
	return TRUE;
}

BOOL AgcmUISkill2::CBIsActiveBuyButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	if (pThis->m_lSelectedBuyItemIndex < 0)
		return TRUE;

	if (!pThis->IsActiveBuyItem(pThis->m_lSelectedBuyItemIndex))
		return FALSE;

	return TRUE;
}

BOOL AgcmUISkill2::CBIsActiveUpgradeButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	if (pThis->m_lSelectedUpgradeItemIndex < 0 && pThis->m_lSelectedUpgradeHighLevelSkillTID == 0)
		return TRUE;

	if (pThis->m_lSelectedUpgradeHighLevelSkillTID > 0)
	{
		if(pThis->IsActiveUpgradeHighLevelItem(pThis->m_lSelectedUpgradeHighLevelSkillTID))
			return TRUE;
		else
			return FALSE;
	}

	if (!pThis->IsActiveUpgradeItem(pThis->m_lSelectedUpgradeItemIndex))
		return FALSE;

	return TRUE;
}

BOOL AgcmUISkill2::CBIsActiveRollbackButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	if (pThis->m_lSelectedItemIndex < 0 && pThis->m_lSelectedHighLevelSkillTID == 0)
		return FALSE;

	AgpdSkill* pcsSkill = NULL;

	if(pThis->m_lSelectedHighLevelSkillTID == 0)
	{
		AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pThis->m_stSkillListGrid[pThis->m_lSelectedItemIndex], 0, 0, 0);
		if (!pcsGridItem)
			return FALSE;

		pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	}
	else
	{
		// 고렙스킬
		pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_lSelectedHighLevelSkillTID);
	}

	if(!pcsSkill)
		return FALSE;

	if (!pThis->m_pcsAgpmEventSkillMaster->CheckTargetSkillForRollback(pcsSkill))
		return FALSE;

	if (pThis->m_pcsAgpmItem->GetSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()) ||
		pThis->m_pcsAgpmItem->GetCashSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
		return TRUE;

	return FALSE;
}

BOOL AgcmUISkill2::CBIsActiveRollbackButton2(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
#ifdef _AREA_JAPAN_
#else
	if(pcsSelfCharacter && _tcslen(pcsSelfCharacter->m_szSkillInit) == 0)
		return TRUE;
#endif

	return FALSE;
}

BOOL AgcmUISkill2::GetUpdateFactor(INT32 lTID, AgpdFactor *pcsFactorPoint, AgpdFactor *pcsFactorPercent)
{
	if (!pcsFactorPoint || !pcsFactorPercent)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(lTID);
	if (pcsSkillTemplate)
	{
		INT32		lSkillLevel	= 1;

		int i;
		for (i = AGPMSKILL_CONST_POINT_START + 1; i < AGPMSKILL_CONST_POINT_END; i++)
		{
			// '0' 인지 아닌지를 검사해서 '0'이 아니면 target의 factor point에 더한다.
			if (pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0)
			{
				AgpdFactorCharStatus	*pcsFactorTemp  = (AgpdFactorCharStatus *) m_pcsAgpmFactors->SetFactor(pcsFactorPoint, NULL, g_alFactorTable[i][0]);
				if (!pcsFactorTemp)
					continue;

				if (g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
				{
					AgpdFactorDamage	*pcsFactorDamageTemp = (AgpdFactorDamage *) pcsFactorTemp;

					pcsFactorDamageTemp->csValue[g_alFactorTable[i][1]].lValue[g_alFactorTable[i][2]]
						+= (INT32) pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				}
				else
				{
					pcsFactorTemp->lValue[g_alFactorTable[i][1]]
						+= (INT32) pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				}
			}
		}

		for (i = AGPMSKILL_CONST_PERCENT_START + 1; i < AGPMSKILL_CONST_PERCENT_END; i++)
		{
			// '0' 인지 아닌지를 검사해서 '0'이 아니면 target의 factor percent에 더한다.
			if (pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0)
			{
				AgpdFactorCharStatus	*pcsFactorTemp  = (AgpdFactorCharStatus *) m_pcsAgpmFactors->SetFactor(pcsFactorPercent, NULL, g_alFactorTable[i][0]);
				if (!pcsFactorTemp)
					continue;

				if (g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
				{
					AgpdFactorDamage	*pcsFactorDamageTemp = (AgpdFactorDamage *) pcsFactorTemp;

					pcsFactorDamageTemp->csValue[g_alFactorTable[i][1]].lValue[g_alFactorTable[i][2]]
						+= (INT32) pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				}
				else
				{
					pcsFactorTemp->lValue[g_alFactorTable[i][1]]
						+= (INT32) pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmUISkill2::SetRecastSkillTimeToGridItem(INT32 lSkillID, UINT32 ulDuration, UINT32 ulRecastDelay)
{
	AgpdSkill	*pcsSkill		= m_pcsAgpmSkill->GetSkill(lSkillID);
	if (!pcsSkill || !pcsSkill->m_pcsGridItem)
		return FALSE;

	pcsSkill->m_pcsGridItem->SetUseItemTime(GetPrevClockCount(), /*ulDuration + */ulRecastDelay);

	return TRUE;
}

BOOL AgcmUISkill2::CBReturnConfirmBuySkill(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass)
		return FALSE;

	if (lTrueCancel == (INT32) TRUE)
	{
		AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

		if (pThis->m_lSelectedBuyItemIndex < 0 || !pThis->m_pcsEvent)
			return FALSE;

		AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pThis->m_stSkillBuyListGrid[pThis->m_lSelectedBuyItemIndex], 0, 0, 0);
		if (!pcsGridItem)
			return FALSE;

//		return pThis->m_pcsAgcmEventSkillMaster->SendBuySkillBook(pThis->m_pcsEvent, pcsGridItem->m_lItemTID);
		return pThis->m_pcsAgcmEventSkillMaster->SendLearnSkill(pThis->m_pcsEvent, pcsGridItem->m_lItemTID);
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBReturnConfirmLearnSkill(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass)
		return FALSE;

	if (lTrueCancel == (INT32) TRUE)
	{
		AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

		if (pThis->m_pcsUseSkillBook)
		{
			// 배울 수 있는 넘인지 체크해본다.
			AgpmEventSkillLearnResult	eCheckResult	=
				pThis->m_pcsAgpmEventSkillMaster->CheckLearnSkill(pThis->m_pcsAgcmCharacter->GetSelfCharacter(),
																  pThis->m_pcsAgpmSkill->GetSkillTemplate(pThis->m_pcsUseSkillBook->m_lSkillTID));

			switch (eCheckResult) {
			case AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS:
				// 아템 사용한다는 패킷을 보낸다.
				pThis->m_pcsAgcmItem->UseItemSkillBook(pThis->m_pcsUseSkillBook, FALSE);
				break;

			case AGPMEVENT_SKILL_LEARN_RESULT_FAIL:
				break;

			case AGPMEVENT_SKILL_LEARN_RESULT_ALREADY_LEARN:
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventAlreadyLearnSkill);
				break;

			case AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_SKILLPOINT:
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotEnoughSkillPoint);
				break;

			case AGPMEVENT_SKILL_LEARN_RESULT_LOW_LEVEL:
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventLowLevel);
				break;

			case AGPMEVENT_SKILL_LEARN_RESULT_NOT_LEARNABLE_CLASS:
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventInvalidClass);
				break;
			}

			pThis->m_pcsUseSkillBook	= NULL;
		}
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBBuySkillResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)		pData;
	AgpmEventSkillBuyResult	eResult	= (AgpmEventSkillBuyResult)	(INT32) pCustData;

	switch (eResult) {
	case AGPMEVENT_SKILL_BUY_RESULT_SUCCESS:
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSkillBuySuccess);
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBUpgradeSkillResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)		pClass;
	PVOID			*ppvBuffer		= (PVOID *)				pCustData;

	INT32			lSkillID		= (INT32) ppvBuffer[1];
	AgpmEventSkillUpgradeResult	eResult	= (AgpmEventSkillUpgradeResult) (INT32) ppvBuffer[2];

	switch (eResult) {
	case AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS:
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSkillUpgradeSuccess);

			AgpdSkill	*pcsSkill	= pThis->m_pcsAgpmSkill->GetSkill(lSkillID);
			if (pcsSkill)
			{
				if(pThis->m_pcsAgpmSkill->IsHeroicSkill(static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate)) )
					pThis->SetUpgradeMasteryHeroic();

				else
				{
					pThis->SetUpgradeMasteryHighLevel();
					pThis->SetBuyMastery(pThis->m_pcsAgpmEventSkillMaster->GetMasteryIndex((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate));
				}
			}
		}
		break;

	case AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_MONEY:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotEnoughMoney);
		break;

	case AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_SKILLPOINT:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotEnoughSkillPoint);
		break;

	case AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_HEROICPOINT:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotEnoughHeroicPoint);
		break;

	case AGPMEVENT_SKILL_UPGRADE_RESULT_NOT_ENOUGH_CHARISMAPOINT:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotEnoughCharismaPoint);
		break;
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBLearnSkillResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkill2				*pThis			= (AgcmUISkill2 *)		pClass;
	AgpdCharacter				*pcsCharacter	= (AgpdCharacter *)		pData;
	AgpmEventSkillLearnResult	eResult			= (AgpmEventSkillLearnResult)	*static_cast< INT8* >(pCustData);

	switch (eResult) {
	case AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSkillLearnSuccess);
		if(pThis->m_pcsAgcmEventEffect)
			pThis->m_pcsAgcmEventEffect->SetCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_LEARN_SKILL);
		break;

	case AGPMEVENT_SKILL_LEARN_RESULT_FAIL:
		break;

	case AGPMEVENT_SKILL_LEARN_RESULT_ALREADY_LEARN:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventAlreadyLearnSkill);
		break;

	case AGPMEVENT_SKILL_LEARN_RESULT_NOT_ENOUGH_SKILLPOINT:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotEnoughSkillPoint);
		break;

	case AGPMEVENT_SKILL_LEARN_RESULT_LOW_LEVEL:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventLowLevel);
		break;

	case AGPMEVENT_SKILL_LEARN_RESULT_NOT_LEARNABLE_CLASS:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventInvalidClass);
		break;
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBSkillInitializeResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)		pData;
	INT8			cResult			= *(INT8 *)				pCustData;

	if (cResult)
	{
		switch( cResult )
		{
		case AGPMEVENT_SKILL_INITIALIZE_RESULT_FAIL :
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSkillInitializeFailed);
			}
			break;

		case AGPMEVENT_SKILL_INITIALIZE_RESULT_FAIL_MONEY_OVER :
			{
				CHAR* pMsgString = ClientStr().GetStr( STL_SKILL_INITIALIZE_FAIL_MONEY_FULL );
				pThis->OnShowMsgBox( pMsgString );
			}
			break;
		}		
	}
	else
	{
		pThis->SetBuyMastery(pThis->m_pcsUserDataActiveBuySkillTab->m_lSelectedIndex);

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSkillInitializeSuccess);

		pThis->m_bIsActiveInitialize	= FALSE;

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataIsActiveInitialize);
	}

	return TRUE;
}

// 2007.07.23. steeple
// 스킬 쿨타임을 초기화 한다.
BOOL AgcmUISkill2::CBInitCoolTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUISkill2* pThis = static_cast<AgcmUISkill2*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	INT32 lSkillTID = *static_cast<INT32*>(pCustData);

	AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pcsCharacter, lSkillTID);
	if(!pcsSkill || !pcsSkill->m_pcsGridItem)
		return TRUE;

	pcsSkill->m_pcsGridItem->SetUseItemTime(0, 0);

	return TRUE;
}

BOOL AgcmUISkill2::CBGetSkillLevel( PVOID pData , PVOID pClass , PVOID pCustData )
{
	if( !pData || !pClass || !pCustData )			return FALSE;

	AgcmUISkill2*	pThis		=	static_cast< AgcmUISkill2*	>(pClass);
	AgpdSkill*		pdSkill		=	static_cast< AgpdSkill*		>(pData);
	INT32*			pSkillLevel	=	static_cast< INT32*			>(pCustData);
	
	pThis->m_pcsAgpmFactors->GetValue(&pdSkill->m_csFactor, pSkillLevel, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_LEVEL);
	
	return TRUE;
}

BOOL AgcmUISkill2::CBReturnConfirmUpgradeSkill(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass)
		return FALSE;

	if (lTrueCancel == (INT32) TRUE)
	{
		AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

//		if (pThis->m_lSelectedUpgradeItemIndex < 0 || !pThis->m_pcsEvent)
//			return FALSE;
//
//		AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pThis->m_stSkillUpgradeListGrid[pThis->m_lSelectedUpgradeItemIndex], 0, 0, 0);
//		if (!pcsGridItem)
//			return FALSE;

		// 고렙 스킬 먼저 처리
		if (pThis->m_lSelectedUpgradeHighLevelSkillTID != 0 && pThis->m_pcsEvent)
		{
			AgpdSkillTemplate* pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate(pThis->m_lSelectedUpgradeHighLevelSkillTID);
			if(pcsSkillTemplate)
			{
				AgpdSkill* pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pThis->m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
				if(pcsSkill)
				{
					// 이미 배웠다면 업그레이드
					pThis->m_pcsAgcmEventSkillMaster->SendRequestSkillUpgrade(pThis->m_pcsEvent, pcsSkill->m_lID, 1);
				}
				else
				{
					// 처음 배우는 거
					pThis->m_pcsAgcmEventSkillMaster->SendLearnSkill(pThis->m_pcsEvent, pcsSkillTemplate->m_lID);
				}

				return TRUE;
			}
		}

		if (pThis->m_lSelectedBuyItemIndex < 0 || !pThis->m_pcsEvent)
			return FALSE;

		AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pThis->m_stSkillBuyListGrid[pThis->m_lSelectedBuyItemIndex], 0, 0, 0);
		if (!pcsGridItem)
			return FALSE;

		return pThis->m_pcsAgcmEventSkillMaster->SendRequestSkillUpgrade(pThis->m_pcsEvent, pcsGridItem->m_lItemID, 1);
	}

	return TRUE;
}

BOOL AgcmUISkill2::CBSkillInitializeOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	switch( g_eServiceArea )
	{
		case AP_SERVICE_AREA_KOREA :
		case AP_SERVICE_AREA_CHINA :
			{
				if( _tcslen( pcsSelfCharacter->m_szSkillInit ) == 0 )
				{
					return pThis->m_pcsAgcmEventSkillMaster->SendRequestSkillInitialize( pThis->m_pcsEvent );
				}
			}
			break;
	}

	return FALSE;
}

BOOL AgcmUISkill2::CBSkillRollback(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	// 2007.07.08. steeple
	// 고렙 스킬 롤백 작업
	//

	if (pThis->m_lSelectedItemIndex < 0 && pThis->m_lSelectedHighLevelSkillTID == 0)
		return FALSE;

	AgpdSkill* pcsSkill = NULL;

	if(pThis->m_lSelectedHighLevelSkillTID == 0)
	{
		AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pThis->m_stSkillListGrid[pThis->m_lSelectedItemIndex], 0, 0, 0);
		if (!pcsGridItem)
			return FALSE;

		pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	}
	else
	{
		// 고렙스킬
		pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_lSelectedHighLevelSkillTID);
	}

	if(!pcsSkill)
		return FALSE;

	if (!pThis->m_pcsAgpmEventSkillMaster->CheckTargetSkillForRollback(pcsSkill))
		return FALSE;

	if (!pThis->m_pcsAgpmItem->GetSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()) &&
		!pThis->m_pcsAgpmItem->GetCashSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
		return FALSE;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSkillRollbackConfirm);
}

BOOL AgcmUISkill2::CBReturnConfirmSkillRollback(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	AgcmUISkill2	*pThis			= (AgcmUISkill2 *)	pClass;

	if (lTrueCancel)
	{
		// 2007.07.08. steeple
		// 고렙 스킬 롤백 작업
		//

		if (pThis->m_lSelectedItemIndex < 0 && pThis->m_lSelectedHighLevelSkillTID == 0)
			return FALSE;

		AgpdSkill* pcsSkill = NULL;

		if(pThis->m_lSelectedHighLevelSkillTID == 0)
		{
			AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pThis->m_stSkillListGrid[pThis->m_lSelectedItemIndex], 0, 0, 0);
			if (!pcsGridItem)
				return FALSE;

			pcsSkill = pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
		}
		else
		{
			// 고렙스킬
			pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_lSelectedHighLevelSkillTID);
		}

		if(!pcsSkill)
			return FALSE;

		if (!pThis->m_pcsAgpmEventSkillMaster->CheckTargetSkillForRollback(pcsSkill))
			return FALSE;

		if (!pThis->m_pcsAgpmItem->GetSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()) &&
			!pThis->m_pcsAgpmItem->GetCashSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
			return FALSE;

		// send request
		///////////////////////////////////////////
		return pThis->m_pcsAgcmSkill->SendRequestRollback(pcsSkill->m_lID);
	}

	return TRUE;
}

BOOL AgcmUISkill2::IsActiveBuyItem(INT32 lIndex)
{
	if (lIndex < 0 || m_pcsUserDataSkillBuyGrid->m_stUserData.m_lCount <= lIndex)
		return FALSE;

	AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItem(&m_stSkillBuyListGrid[lIndex], 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pcsGridItem->m_lItemTID);
	if (!pcsSkillTemplate)
		return FALSE;

	if (!m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkillTemplate))
		return FALSE;

	AgpdSkill		*pcsSkill		= m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	if (!pcsSkill)
	{
		INT32	lBuyCost	= m_pcsAgpmEventSkillMaster->GetBuyCost(pcsSkillTemplate, m_pcsSelfCharacter);
		if (lBuyCost < 0)
			return FALSE;

		INT64	llMoney		= 0;
		m_pcsAgpmCharacter->GetMoney(m_pcsSelfCharacter, &llMoney);

		if (llMoney < (INT64) lBuyCost)
			return FALSE;

		AgpmEventSkillLearnResult	eCheckResult	= m_pcsAgpmEventSkillMaster->CheckLearnSkill(m_pcsSelfCharacter, pcsSkillTemplate);
		if (eCheckResult != AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS)
			return FALSE;
	}
	else
	{
		INT32	lUpgradeCost	= m_pcsAgpmEventSkillMaster->GetUpgradeCost(pcsSkill, m_pcsSelfCharacter);
		if (lUpgradeCost < 0)
			return FALSE;

		INT64	llMoney		= 0;
		m_pcsAgpmCharacter->GetMoney(m_pcsSelfCharacter, &llMoney);

		if (llMoney < (INT64) lUpgradeCost)
			return FALSE;

		INT32	lUpgradeCostSP	= m_pcsAgpmEventSkillMaster->GetUpgradeCostSP(pcsSkill);
		if (lUpgradeCostSP < 0)
			return FALSE;

		INT32	lCharacterSP	= m_pcsAgpmCharacter->GetSkillPoint(m_pcsSelfCharacter);

		if (lCharacterSP < lUpgradeCostSP)
			return FALSE;

		INT32	lSkillLevel	= m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
		if (lSkillLevel < 1)
			return FALSE;

		INT32	lCharLevel	= m_pcsAgpmCharacter->GetLevel(m_pcsSelfCharacter);

		if (lCharLevel < ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel + 1])
			return FALSE;
	}

	return TRUE;
}

BOOL AgcmUISkill2::IsActiveUpgradeItem(INT32 lIndex)
{
	if (lIndex < 0 || m_pcsUserDataSkillUpgradeGrid->m_stUserData.m_lCount <= lIndex)
		return FALSE;

	AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItem(&m_stSkillUpgradeListGrid[lIndex], 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	AgpdSkill		*pcsSkill		= m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
	if (!pcsSkill)
		return FALSE;
	
	if (!m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkill))
		return FALSE;

	INT32	lUpgradeCost	= m_pcsAgpmEventSkillMaster->GetUpgradeCost(pcsSkill, m_pcsSelfCharacter);
	if (lUpgradeCost < 0)
		return FALSE;

	INT64	llMoney		= 0;
	m_pcsAgpmCharacter->GetMoney(m_pcsSelfCharacter, &llMoney);

	if (llMoney < (INT64) lUpgradeCost)
		return FALSE;

	INT32	lUpgradeCostSP	= m_pcsAgpmEventSkillMaster->GetUpgradeCostSP(pcsSkill);
	if (lUpgradeCostSP < 0)
		return FALSE;

	INT32	lCharacterSP	= m_pcsAgpmCharacter->GetSkillPoint(m_pcsSelfCharacter);

	if (lCharacterSP < lUpgradeCostSP)
		return FALSE;

	INT32	lSkillLevel	= m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if (lSkillLevel < 1)
		return FALSE;

	INT32	lCharLevel	= m_pcsAgpmCharacter->GetLevel(m_pcsSelfCharacter);

	if (lCharLevel < ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel + 1])
		return FALSE;
	
	return TRUE;
}

BOOL AgcmUISkill2::IsActiveUpgradeHighLevelItem(INT32 lSkillTID)
{
	if(lSkillTID <= 0)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(lSkillTID);
	if(!pcsSkillTemplate)
		return FALSE;

	if(!m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkillTemplate))
		return FALSE;

	// 스킬이 있는 지 확인한다.
	AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkill(m_pcsSelfCharacter, pcsSkillTemplate->m_szName);
	if(!pcsSkill)
	{
		INT32 lBuyCost = m_pcsAgpmEventSkillMaster->GetBuyCost(pcsSkillTemplate, m_pcsSelfCharacter);
		if(lBuyCost < 0)
			return FALSE;

		INT64 llMoney = 0;
		m_pcsAgpmCharacter->GetMoney(m_pcsSelfCharacter, &llMoney);

		if(llMoney < (INT64)lBuyCost)
			return FALSE;

		AgpmEventSkillLearnResult eCheckResult = m_pcsAgpmEventSkillMaster->CheckLearnSkill(m_pcsSelfCharacter, pcsSkillTemplate);
		if (eCheckResult != AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS)
			return FALSE;
	}
	else
	{
		INT32 lUpgradeCost = m_pcsAgpmEventSkillMaster->GetUpgradeCost(pcsSkill, m_pcsSelfCharacter);
		if(lUpgradeCost < 0)
			return FALSE;

		INT64 llMoney = 0;
		m_pcsAgpmCharacter->GetMoney(m_pcsSelfCharacter, &llMoney);

		if(llMoney < (INT64)lUpgradeCost)
			return FALSE;

		INT32 lUpgradeCostSP = m_pcsAgpmEventSkillMaster->GetUpgradeCostSP(pcsSkill);
		if(lUpgradeCostSP < 0)
			return FALSE;

		INT32 lCharacterSP = m_pcsAgpmCharacter->GetSkillPoint(m_pcsSelfCharacter);

		if(lCharacterSP < lUpgradeCostSP)
			return FALSE;

		INT32 lSkillLevel = m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
		if(lSkillLevel < 1)
			return FALSE;

		INT32 lCharLevel = m_pcsAgpmCharacter->GetLevel(m_pcsSelfCharacter);

		if(lCharLevel < ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel + 1])
			return FALSE;
	}

	return TRUE;
}

BOOL AgcmUISkill2::SetStatusActiveBuyItem(INT32 lIndex)
{
	if (lIndex < 0 || m_pcsUserDataSkillBuyGrid->m_stUserData.m_lCount <= lIndex)
		return FALSE;

	if (IsActiveBuyItem(lIndex))
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventBuyActiveItem, lIndex);
	else
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventBuyDisableItem, lIndex);

	return TRUE;
}

BOOL AgcmUISkill2::SetStatusActiveUpgradeItem(INT32 lIndex)
{
	if (lIndex < 0 || m_pcsUserDataSkillUpgradeGrid->m_stUserData.m_lCount <= lIndex)
		return FALSE;

	if (IsActiveUpgradeItem(lIndex))
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventUpgradeActiveItem, lIndex);
	else
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventUpgradeDisableItem, lIndex);

	return TRUE;
}

BOOL AgcmUISkill2::OpenSkillUI(INT32 lMasteryIndex)
{
	AgpdCharacter* ppdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	if( m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) != AURACE_TYPE_DRAGONSCION )
	{
		if(lMasteryIndex != AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX)
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenSkillUI);
		else
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenSkillHighLevelUI);

		SetMastery(lMasteryIndex);
	}

	return TRUE;
}

BOOL AgcmUISkill2::OnShowMsgBox( CHAR* pMsgString )
{
	if( !pMsgString || strlen( pMsgString ) == 0 ) return FALSE;
	if( !m_pcsAgcmUIManager2 ) return FALSE;
	m_pcsAgcmUIManager2->ActionMessageOKDialog( pMsgString );
	return TRUE;
}

// 이 함수는 해당 스킬을 사용하기 위한 전제조건들을 만족하는지를 검사한다.
BOOL AgcmUISkill2::IsUsableSkill( void* pSkill )
{	
	AgpdSkill* ppdSkill = ( AgpdSkill* )pSkill;
	if( !ppdSkill ) return FALSE;

	AgpdSkillTemplate* ppdSkillTemplate = ( AgpdSkillTemplate* )ppdSkill->m_pcsTemplate;
	if( !ppdSkillTemplate ) return FALSE;

	AgpdCharacter* ppdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 내 스킬이 아니면 사용불가
	if( !m_pcsAgpmSkill->IsOwnSkill( ppdCharacter, ( AgpdSkillTemplate* )ppdSkill->m_pcsTemplate ) ) return FALSE;

	// 이 스킬이 공격, 버프, 디버프, 진화, 소환, 패시브스킬인 경우에는 종족 및 클래스를 체크한다.
	BOOL bIsAttackSkill = m_pcsAgpmSkill->IsAttackSkill( ppdSkillTemplate );
	BOOL bIsSelfBuffSkill = m_pcsAgpmSkill->IsSelfBuffSkill( ppdSkillTemplate );
	BOOL bIsDeBuffSkill = m_pcsAgpmSkill->IsDebuffSkill( ppdSkillTemplate );
	BOOL bIsEvolutionSkill = m_pcsAgpmSkill->IsEvolutionSkill( ppdSkillTemplate );
	BOOL bIsSummonSkill = m_pcsAgpmSkill->IsSummonsSkill( ppdSkillTemplate );
	BOOL bIsPassiveSkill = m_pcsAgpmSkill->IsPassiveSkill( ppdSkillTemplate );
	BOOL bIsETCSkill = m_pcsAgpmSkill->IsETCSkill( ppdSkillTemplate );

	if( bIsAttackSkill || bIsSelfBuffSkill || bIsDeBuffSkill || bIsEvolutionSkill || bIsSummonSkill || bIsPassiveSkill || bIsETCSkill )
	{
		if( !_IsUsableSkillForMyRaceAndClass( ppdCharacter, ppdSkillTemplate ) ) return FALSE;
	}

	// 요구사항에 맞지 않으면 사용불가
	if( !m_pcsAgpmSkill->IsSatisfyRequirement( ppdSkill ) ) return FALSE;

	// 그 외에는 사용가능하다.
	return TRUE;
}

// 이 함수는 현재 내 캐릭터가 이 스킬을 사용가능한지를 검사한다.
BOOL AgcmUISkill2::IsEnableSkill( void* pSkill )
{
	AgpdSkill* ppdSkill = ( AgpdSkill* )pSkill;
	if( !ppdSkill ) return FALSE;

	AgpdSkillTemplate* ppdSkillTemplate = ( AgpdSkillTemplate* )ppdSkill->m_pcsTemplate;
	if( !ppdSkillTemplate ) return FALSE;

	AgpdCharacter* ppdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// HP, MP, SP, Arrow, Bolt 갯수 조사
	eAgpmSkillActionType eNotEnoughRequirement = AGPMSKILL_ACTION_NEED_REQUIREMENT;
	if( !m_pcsAgpmSkill->IsSufficientCost( ppdSkill, &eNotEnoughRequirement ) )
	{
		//// 뭔가 조건이 모자란다면 뭐가 모자란지 시스템 메세지 등으로 알려준다.
		//CBCheckCastSkillResult( &eNotEnoughRequirement, this, NULL );
		return FALSE;
	}

	// 모든 조건을 만족하면 사용가능
	return TRUE;
}

BOOL AgcmUISkill2::_IsUsableSkillForMyRaceAndClass( void* pCharacter, void* pSkillTemplate )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return FALSE;

	AgpdSkillTemplate* ppdSkillTemplate = ( AgpdSkillTemplate* )pSkillTemplate;
	if( !ppdSkillTemplate ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = pcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );

	INT32 nConstIndex = -1;
	switch( eRaceType )
	{
	case AURACE_TYPE_HUMAN :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_KNIGHT;			break;
			case AUCHARCLASS_TYPE_RANGER :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_ARCHER;			break;
			case AUCHARCLASS_TYPE_MAGE :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_WIZARD;			break;
			}
		}
		break;

	case AURACE_TYPE_ORC :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_BERSERKER;		break;
			case AUCHARCLASS_TYPE_RANGER :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_HUNTER;			break;
			case AUCHARCLASS_TYPE_MAGE :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_SORCERER;		break;
			}
		}
		break;

	case AURACE_TYPE_MOONELF :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_SWASHBUCKLER;	break;
			case AUCHARCLASS_TYPE_RANGER :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_RANGER;			break;
			case AUCHARCLASS_TYPE_MAGE :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_ELEMENTALER;	break;
			}
		}
		break;

	case AURACE_TYPE_DRAGONSCION :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_SLAYER;			break;
			case AUCHARCLASS_TYPE_RANGER :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_OBITER;			break;
			case AUCHARCLASS_TYPE_MAGE :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_SUMMONER;		break;
			case AUCHARCLASS_TYPE_SCION :	nConstIndex = AGPMSKILL_CONST_REQUIRE_CLASS_SCION;			break;
			}
		}
		break;
	}

	if( nConstIndex < 0 ) return FALSE;
	return ppdSkillTemplate->m_fUsedConstFactor[ nConstIndex ][ 1 ] ? TRUE : FALSE;
}

VOID	AgcmUISkill2::RefreshInitButton( VOID )
{
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pcsUserDataIsActiveInitialize );
}

BOOL	AgcmUISkill2::_MoveSkillWindow( BOOL bUpgradeSkill /* = FALSE */ )
{
	AgcdUI*		pSkillWindow		=	NULL;
	AgcdUI*		pHighLevelWindow	=	NULL;
	AgcdUI*		pHeroicWindow		=	NULL;
	AgcdUI*		pSrcWindow			=	NULL;

	
	if( bUpgradeSkill )
	{
		pSkillWindow		=	m_pcsAgcmUIManager2->GetUI( "UI_SkillBuy" );
		pHighLevelWindow	=	m_pcsAgcmUIManager2->GetUI( "UI_SkillHighLevelUp" );
		pHeroicWindow		=	m_pcsAgcmUIManager2->GetUI( "UI_HeroicSkill_Upgrade" );
	}
	else
	{
		pSkillWindow		=	m_pcsAgcmUIManager2->GetUI( "UI_Skill" );
		pHighLevelWindow	=	m_pcsAgcmUIManager2->GetUI( "UI_SkillHighLevel" );
		pHeroicWindow		=	m_pcsAgcmUIManager2->GetUI( "UI_HeroicSkill" );
	}

	switch(	m_lLastSetMasteryIndex )
	{

	case AGPMEVENT_SKILL_HEROIC_MASTERY_INDEX:
		pSrcWindow	=	pHeroicWindow;
		break;

	case AGPMEVENT_SKILL_HIGHLEVEL_MASTERY_INDEX:
		pSrcWindow	=	pHighLevelWindow;
		break;

	default:
		pSrcWindow	=	pSkillWindow;
		break;

	}

	pSkillWindow->m_pcsUIWindow->x		=	pSrcWindow->m_pcsUIWindow->x;
	pSkillWindow->m_pcsUIWindow->y		=	pSrcWindow->m_pcsUIWindow->y;

	pHighLevelWindow->m_pcsUIWindow->x	=	pSrcWindow->m_pcsUIWindow->x;
	pHighLevelWindow->m_pcsUIWindow->y	=	pSrcWindow->m_pcsUIWindow->y;

	pHeroicWindow->m_pcsUIWindow->x		=	pSrcWindow->m_pcsUIWindow->x;
	pHeroicWindow->m_pcsUIWindow->y		=	pSrcWindow->m_pcsUIWindow->y;
	
	return TRUE;
}