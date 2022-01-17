#include "AgcmUIItem.h"
#include "AgcdUISystemMessage.h"
#include "AgcmUIConsole.h"

#include "AuStrTable.h"
#include "AgcmUISkillDragonScionDef.h"

AgcmUIItem::AgcmUIItem()
{
	SetModuleName("AgcmUIItem");
	EnableIdle(TRUE);

	m_pcsApmMap = NULL;
	m_pcsAgpmGrid = NULL;
	m_pcsAgpmFactors = NULL;
	m_pcsAgpmCharacter = NULL;
	m_pcsAgpmAuction = NULL;
	m_pcsAgpmAuctionCategory = NULL;
	m_pcsAgcmCharacter = NULL;
	m_pcsAgpmItem = NULL;
	m_pcsAgpmItemConvert = NULL;
	m_pcsAgcmItemConvert = NULL;
	m_pcsAgpmSkill = NULL;
	m_pcsAgpmEventSkillMaster = NULL;
	m_pcsAgcmItem = NULL;
	m_pcsAgcmAuction = NULL;
	m_pcsAgcmPrivateTrade = NULL;
	m_pcsAgpmEventNPCTrade = NULL;
	m_pcsAgcmEventNPCTrade = NULL;
	m_pcsAgcmUIControl = NULL;
	m_pcsAgcmUIManager2 = NULL;
	m_pcsAgcmUIMain = NULL;
	m_pcsAgcmUICharacter = NULL;
	m_pcsAgcmUIAuction = NULL;
	m_pcsAgcmChatting2 = NULL;
	m_pcsAgcmUISplitItem = NULL;
	m_pcsAgpmCombat = NULL;
	m_pcsAgpmEventItemRepair = NULL;
	m_pcsAgpmReturnToLogin = NULL;
	m_pcsAgpmArchlord = NULL;

	m_pcsInventory = NULL; 
	m_pcsInventoryQuest = NULL;
	m_pcsInventoryMoney = NULL;
	m_pcsEquip = NULL;
	//m_pcsBank = NULL;
	m_pcsSalesBox = NULL;
	m_pcsSalesBox2Grid = NULL;
	m_pcsSalesBox2SalesOn = NULL;
	m_pcsSalesBox2SalesComplete = NULL;
	m_pcsSalesBox2ButtonEnable = NULL;
	m_pcsNPCTrade = NULL;
	m_pcsPrivateTrade = NULL;
	m_pcsPrivateTradeClient = NULL;
	m_pcsPrivateTraderName = NULL;
	m_pcsRequestPrivateTrade = NULL;
	m_pcsPrivateTradeSetName = NULL;
	m_pcsPrivateTradeSetClientName = NULL;
	m_pcsPrivateTradeGhelld = NULL;
	m_pcsPrivateTradeClientGhelld = NULL;
	m_pcsInventoryLayerIndex = NULL;
	m_pcsSubInventoryLayerIndex = NULL;
//	m_pcsBankLayerIndex = NULL;
	m_pcsMessageData = NULL;
	m_pcsEquipSlotBody = NULL;
	m_pcsEquipSlotHead = NULL;
	m_pcsEquipSlotHands = NULL;
	m_pcsEquipSlotLegs = NULL;
	m_pcsEquipSlotFoot = NULL;
	m_pcsEquipSlotHandLeft = NULL;
	m_pcsEquipSlotHandRight = NULL;
	m_pcsEquipSlotRing1 = NULL;
	m_pcsEquipSlotRing2 = NULL;
	m_pcsEquipSlotNecklace = NULL;
	m_pcsActiveReverseOrb = NULL;

	m_lInventoryLayerIndex = 0;
	m_lSubInventoryLayerIndex = 0;
	//m_lBankLayerIndex	= 0;

	m_pcsInventoryGrid = NULL;
	m_pcsInventoryQuestGrid = NULL;
	m_pcsEquipGrid = NULL;
//	m_pcsBankGrid = NULL;
	m_pcsSalesBoxGrid = NULL;
	m_pcsSubInventoryGrid = NULL;
	ZeroMemory(m_csSalesBox2Grid,sizeof(AgpdGrid) * AGPMAUCTION_MAX_REGISTER);
	ZeroMemory(m_strSalesBox2SalesOn,sizeof(m_strSalesBox2SalesOn));
	ZeroMemory(m_strSalesBox2SalesComplete,sizeof(m_strSalesBox2SalesComplete));

	m_lSalesBox2SalesButtonEnable = 0;

	m_pcsNPCTradeGrid				=	NULL;
	m_pcsPrivateTradeGrid			=	NULL;
	m_pcsPrivateTradeClientGrid		=	NULL;

	ZeroMemory(m_csEquipSlotGrid,sizeof(AgpdGrid) * AGPMITEM_PART_NUM);
	ZeroMemory(&m_stMessageData,sizeof(AgcmUIItemMessageData));
	
	m_lPrivateTradeRequesterCID = 0;

	m_pcsPrivateTrader = NULL;
	m_pcsPTRequest = NULL;
	m_pcsPrivateTradeSetNameChar = NULL;
	m_pcsPrivateTradeSetClientNameChar = NULL;

	m_llInventoryMoney = 0;
	m_lPrivateTradeGhelld = 0;
	m_lPrivateTradeClientGhelld = 0;

	m_lInventoryItemID = 0;
	m_lNPCTradeSellItemID = 0;
	m_lNPCTradeBuyItemID = 0;

	m_nNPCTradeBuyStatus = (-1);
	m_lNPCTradeBuyLayer = (-1);
	m_lNPCTradeBuyRow = (-1);
	m_lNPCTradeBuyColumn = (-1);

	m_bUIOpenInventory = FALSE;
	m_lInventoryUIOpen = 0;
	m_lInventoryUIClose = 0;
	m_lInventoryDropMoneyUIOpen = 0;
	m_lInventoryDropMoneyUIClose = 0;

	m_lInventoryEnableDropGhelld = 0;
	m_lInventoryDisableDropGhelld = 0;

	m_lInventoryMSGFull = 0;
	m_lInventoryMSGNotEnoughSpaceToGetGhelld = 0;
	m_lInventoryMSGGetItem = 0;
	m_lInventoryMSGDropItem = 0;
	m_lInventoryMSGDropGhelld = 0;

	m_bUIOpenBank = FALSE;
	m_lBankUIOpen = 0;
	m_lBankUIClose = 0;
	
	m_bUIOpenPrivateTradeYesNo = FALSE;
	m_bUIOpenPrivateTrade = FALSE;
	m_bUIOpenPrivateTradeClient = FALSE;
	m_lRequestPrivateTradeUIOpen = 0;
	m_lRequestPrivateTradeUIClose = 0;
	m_lPrivateTradeOpenUIYesNo = 0;
	m_lPrivateTradeCloseUIYesNo = 0;
	m_lPrivateTradeOpenUI = 0;
	m_lPrivateTradeCloseUI = 0;
	m_lPrivateTradeClientLock = 0;
	m_lPrivateTradeClientUnlock = 0;
	m_lPrivateTradeActiveReadyToExchange = 0;
	m_lPrivateTradeClientReadyToExchange = 0;
	m_lPrivateTradeMSGInventoryFull = 0;
	m_lPrivateTradeMSGTradeComplete = 0;
	m_lPrivateTradeMSGTradeCancel = 0;
	m_lPrivateTradeMSGError = 0;
	
	m_bUIOpenNPCTrade = FALSE;
	m_lNPCTradeOpenUI = 0;
	m_lNPCTradeCloseUI = 0;
	m_lOpenNPCTradeLayer = 0;
	m_lNPCTradeInitLayer = 0;

	m_bUIOpenSalesBox = FALSE;
	m_lSalesBoxUIOpen = 0;
	m_lSalesBoxUIClose = 0;
	m_lSalesBoxCancelMessageBox = 0;
	
	m_bUIOpenSubInventory = FALSE;
	m_lSubInventoryUIOpen = 0;
	m_lSubInventoryUIClose = 0;

	//한 화면에 보이는 아이템의 정보(15개)만큼 메모리를 할당한다.
	m_pcsaAgpdItem = new AgpdItem[AGPMAUCTION_MAX_ROW_PER_PAGE];

	m_pcsCancelItemInfo = NULL;
	m_pcsAgpdItem4Sale = NULL;
	m_lSalesItemQuantity = 0;
	
	m_lSalesBox2Add = 0;
	m_lSalesBox2Remove = 0;
	m_lSalesBox2Update = 0;
	m_lSalesBox2ButtonOn = 0;
	m_lSalesBox2ButtonOff = 0;

	//NPC Trade Message
	m_lNPCTradeMSGInventoryFull = 0;
	m_lNPCTradeMSGConfirmBuy = 0;
	m_lNPCTradeMSGConfirmBuyStackCount = 0;
	m_lNPCTradeMSGConfirmSell = 0;
	m_lNPCTradeMSGNotEnoughGhelld = 0;
	m_lNPCTradeMSGNotEnoughSpaceToGetGhelld = 0;

	m_lNPCTradeSuccess = 0;

	// Item Convert
	m_lUIOpenAskConvert = 0;
	m_lUIOpenConvertResultSuccess = 0;
	m_lUIOpenConvertResultFail = 0;
	m_lUIOpenConvertResultDestroyAttribute = 0;
	m_lUIOpenConvertResultDestroyItem = 0;
	m_lUIOpenConvertResultDifferRank = 0;
	m_lUIOpenConvertResultDifferType = 0;
	m_lUIOpenConvertResultFull = 0;
	m_lUIOpenConvertResultNotEquipItem = 0;
	m_lUIOpenConvertResultEgoItem = 0;

	m_bIsProcessConvert = FALSE;
	m_lConvertItemID = 0;
	m_lSpiritStoneID = 0;

	m_nIndexADItem = 0;
	m_nIndexADItemTemplate = -1;

	m_lNumListNotifyDurabilityZero = 0;
	m_lNumListNotifyDurabilityUnder5Percent = 0;
	ZeroMemory(m_alListNotifyDurabilityZero,sizeof(m_alListNotifyDurabilityZero));
	ZeroMemory(m_alListNotifyDurabilityUnder5Percent,sizeof(m_alListNotifyDurabilityUnder5Percent));
	ZeroMemory(m_aulRegisterTimeNotifyDurabilityZero,sizeof(m_aulRegisterTimeNotifyDurabilityZero));
	ZeroMemory(m_aulRegisterTimeNotifyDurabilityUnder5Percent,sizeof(m_aulRegisterTimeNotifyDurabilityUnder5Percent));

	m_ulPrevProcessClock = 0;
	m_bIsActiveReverseOrb = FALSE;

	m_lEventGetItem = 0;
	m_lEventGetMoney = 0;

	m_lEventPickupItemSuccess = 0;
	m_lEventPickupItemMoneySuccess = 0;
	m_lEventPickupItemPCBangMoneySuccess = 0;
	m_lEventPickupItemFail = 0;
	m_lEventPickupItemMoneyFail = 0;

	m_lEventAutoPickupItem = 0;

	m_lEventUseSuccessReverseOrb = 0;
	m_lEventUseFailReverseOrb = 0;

	m_lEventUseAddMovementSkill = 0;
	m_lEventUseAddAtkSpeedSkill = 0;

	m_lEventExchangeBuffSkill = 0;
	m_lEventIgnoreBuffSkill = 0;

	m_lEventTradeFailForBoundItem = 0;

	ZeroMemory(&m_stNPCTradeOpenPos,sizeof(m_stNPCTradeOpenPos));
	ZeroMemory(m_csItemTooltip,sizeof(m_csItemTooltip));
	ZeroMemory(m_aszTooptipTitle,sizeof(CHAR) * AGCMUIITEM_MAX_TOOLTIP * (AGCMUIITEM_MAX_TOOLTIP_TEXT_LENGTH + 1));
	
	m_pcsAgpdSplitItem = NULL;
	m_pcsAgpdDestroyItem	= NULL;

	m_lSplitItemTargetLayer = 0;
	m_lSplitItemTargetRow = 0;
	m_lSplitItemTargetColumn = 0;
	
	m_pcsReturnScrollTemplate = NULL;
	m_pcsReturnScrollGridItem = NULL;

	m_lAddMovementPercent = 0;
	m_lAddAttackSpeedPercent = 0;
	
	m_bIsTransformed = FALSE;

	m_pcsUISubInventory = NULL;
	m_pBtnAutoPickUp = NULL;
}

AgcmUIItem::~AgcmUIItem()
{
	if( m_pcsaAgpdItem != NULL )
		delete [] m_pcsaAgpdItem;
}

BOOL AgcmUIItem::OnAddModule()
{
	m_pcsApmMap					= (ApmMap *)(GetModule("ApmMap"));
	m_pcsAgpmGrid				= (AgpmGrid *)(GetModule("AgpmGrid"));
	m_pcsAgpmFactors			= (AgpmFactors *)(GetModule("AgpmFactors"));
	m_pcsAgpmCharacter			= (AgpmCharacter *)(GetModule("AgpmCharacter"));
	m_pcsAgpmAuction			= (AgpmAuction *)(GetModule("AgpmAuction"));
	m_pcsAgcmCharacter			= (AgcmCharacter *)(GetModule("AgcmCharacter"));
	m_pcsAgpmAuctionCategory	= (AgpmAuctionCategory *)(GetModule("AgpmAuctionCategory"));
	m_pcsAgpmItem				= (AgpmItem *)(GetModule("AgpmItem"));
	m_pcsAgpmItemConvert		= (AgpmItemConvert *)(GetModule("AgpmItemConvert"));
	m_pcsAgpmSkill				= (AgpmSkill *)(GetModule("AgpmSkill"));
	m_pcsAgpmEventSkillMaster	= (AgpmEventSkillMaster *)GetModule("AgpmEventSkillMaster");
	m_pcsAgcmItem				= (AgcmItem *)(GetModule("AgcmItem"));
	m_pcsAgcmAuction			= (AgcmAuction *)(GetModule("AgcmAuction"));
	m_pcsAgcmPrivateTrade		= (AgcmPrivateTrade *)(GetModule("AgcmPrivateTrade"));
	m_pcsAgpmEventNPCTrade		= (AgpmEventNPCTrade *)(GetModule("AgpmEventNPCTrade"));
	m_pcsAgcmEventNPCTrade		= (AgcmEventNPCTrade *)(GetModule("AgcmEventNPCTrade"));
	m_pcsAgcmUIControl			= (AgcmUIControl *)(GetModule("AgcmUIControl"));
	m_pcsAgcmUIManager2			= (AgcmUIManager2 *)(GetModule("AgcmUIManager2"));
	m_pcsAgcmUIMain				= (AgcmUIMain *) GetModule("AgcmUIMain");
	m_pcsAgcmEventNPCTrade		= (AgcmEventNPCTrade *) GetModule("AgcmEventNPCTrade");
	m_pcsAgpmCombat				= (AgpmCombat *) GetModule("AgpmCombat");
	m_pcsAgcmUICharacter		= (AgcmUICharacter *) GetModule("AgcmUICharacter");
	m_pcsAgpmEventItemRepair	= (AgpmEventItemRepair *) GetModule("AgpmEventItemRepair");
	m_pcsAgcmUIAuction			= (AgcmUIAuction *) GetModule( "AgcmUIAuction" );
	m_pcsAgcmChatting2			= (AgcmChatting2 *)	GetModule("AgcmChatting2");
	m_pcsAgcmUISplitItem		= (AgcmUISplitItem *) GetModule("AgcmUISplitItem");
	m_pcsAgcmItemConvert		= (AgcmItemConvert *) GetModule("AgcmItemConvert");
	m_pcsAgpmBillInfo			= (AgpmBillInfo *) GetModule("AgpmBillInfo");
	m_pcsAgpmReturnToLogin		= (AgpmReturnToLogin *) GetModule("AgpmReturnToLogin");
	m_pcsAgpmArchlord			= (AgpmArchlord *) GetModule("AgpmArchlord");
	
	m_pcsInventoryGrid = NULL;
	//m_pcsBankGrid = NULL;

	m_pcsSalesBoxGrid				=	NULL;
	m_pcsNPCTradeGrid				=	NULL;
	m_pcsPrivateTradeGrid			=	NULL;
	m_pcsPrivateTradeClientGrid		=	NULL;
	m_pcsSubInventoryGrid			=	NULL;

	m_bUIOpenInventory = FALSE;
	m_bUIOpenBank = FALSE;
	m_bUIOpenPrivateTradeYesNo = FALSE;
	m_bUIOpenPrivateTrade = FALSE;
	m_bUIOpenPrivateTradeClient = FALSE;
	m_bUIOpenNPCTrade = FALSE;
	m_bUIOpenSalesBox = FALSE;

	if( (!m_pcsAgpmGrid) || (!m_pcsAgpmFactors) || (!m_pcsAgpmCharacter) || (!m_pcsAgpmCharacter) || (!m_pcsAgpmItem) || 
		(!m_pcsAgcmItem) || (!m_pcsAgcmPrivateTrade) || (!m_pcsAgcmEventNPCTrade) || (!m_pcsAgcmUIManager2) ||(!m_pcsAgcmEventNPCTrade) ||
		!m_pcsAgcmUICharacter ||
		!m_pcsAgpmItemConvert ||
		!m_pcsAgcmUIControl ||
		!m_pcsAgpmEventItemRepair ||
		!m_pcsAgcmUISplitItem ||
		!m_pcsAgpmSkill || !m_pcsAgpmEventSkillMaster ||
		!m_pcsApmMap ||
		!m_pcsAgcmItemConvert ||
		!m_pcsAgpmReturnToLogin ||
		!m_pcsAgpmArchlord)
		return FALSE;

	if (m_pcsAgpmItem)
	{
		m_nIndexADItem = m_pcsAgpmItem->AttachItemData(this, sizeof(AgcdUIItem), NULL, NULL);
		if (m_nIndexADItem < 0)
			return FALSE;

		m_nIndexADItemTemplate	= m_pcsAgpmItem->AttachItemTemplateData(this, sizeof(AgcdUIItemTooltipDesc), NULL, NULL);
		if (m_nIndexADItemTemplate < 0)
			return FALSE;
	}

	if( !m_pcsAgpmEventNPCTrade->SetCallbackDeleteGrid(CBNPCTradeDeleteGrid, this ) )
		return FALSE;

	if( !m_pcsAgpmItem->SetCallbackRemoveFromInventory(CBRemoveItemFromInventory, this ) )
		return FALSE;

	if( !m_pcsAgpmItem->SetCallbackRemove(CBRemoveItem, this ) )
		return FALSE;

	if( !m_pcsAgcmItem->SetCallbackSetupItem(CBSetupItem, this) )
		return FALSE;

	if( !m_pcsAgcmItem->SetCallbackCheckUseItem(CBCheckUseItem, this) )
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateActionStatus(CBUpdateActionStatus, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateLevel(CBCharacterLevelUp, this))
		return FALSE;

	//Inventory
	if( !m_pcsAgpmItem->SetCallbackUIUpdateInventory( CBUIUpdateInventory, this ) )
		return FALSE;
	if( !m_pcsAgpmCharacter->SetCallbackUISetMoney( CBUpdateInventoryMoney, this ) )
		return FALSE;

	//Equip
	if( !m_pcsAgpmItem->SetCallbackUIUpdateEquip( CBUIUpdateEquip, this ) )
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackEquip(CBEquipItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUnEquip(CBUnEquipItem, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackUpdateReturnTeleportStatus(CBUpdateReturnScrollStatus, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUseReturnScrollResultFailed(CBUseReturnScrollResultFailed, this))
		return FALSE;

	// SubInventory
	if( !m_pcsAgpmItem->SetCallbackUIUpdateSubInventory( CBUIUpdateSubInventory, this ) )
		return FALSE;

	// Quest
	if (!m_pcsAgpmItem->SetCallbackInventoryQuestAdd(CBInventoryQuestAdd, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackInventoryQuestRemove(CBInventoryQuestRemove, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackInventoryQuestUpdate(CBInventoryQuestUpdate, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackUseItemResult(CBUseItemResult, this))
		return FALSE;

	//Private Trade
	if( !m_pcsAgpmItem->SetCallbackUIUpdateTrade( CBUIUpdateTrade, this ) )
		return FALSE;
	if( !m_pcsAgpmItem->SetCallbackUIUpdateTradeClient( CBUIUpdateTradeClient, this ) )
		return FALSE;

	if(!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(SetSelfCharacterCB, this))
		return FALSE;
	if(!m_pcsAgcmCharacter->SetCallbackReleaseSelfCharacter(ReleaseSelfCharacterCB, this))
		return FALSE;
	if(!m_pcsAgcmPrivateTrade->SetCallbackTradeOpenUIYesNo( CBUIOpenPrivateTradeYesNo, this ) )
		return FALSE;
	if(!m_pcsAgcmPrivateTrade->SetCallbackTradeOpenUI( CBUIOpenPrivateTrade, this ) )
		return FALSE;
	if(!m_pcsAgcmPrivateTrade->SetCallbackTradeCloseUI( CBUIClosePrivateTrade, this ) )
		return FALSE;
	if(!m_pcsAgcmPrivateTrade->SetCallbackTradeUpdateTargetLockButton( CBPrivateTradeUpdateClientLock, this ) ) 
		return FALSE;
	if(!m_pcsAgcmPrivateTrade->SetCallbackTradeActiveReadyToExchange( CBPrivateTradeActiveReadyToExchange, this ) ) 
		return FALSE;
	if(!m_pcsAgcmPrivateTrade->SetCallbackTradeClientReadyToExchange( CBPrivateTradeClientReadyToExchange, this ) ) 
		return FALSE;
	if(!m_pcsAgcmPrivateTrade->SetCallbackUpdateTradeMoney( CBPrivateTradeUpdateGhelld, this ) )
		return FALSE;
	if(!m_pcsAgcmPrivateTrade->SetCallbackUpdateTargetTradeMoney( CBPrivateTradeUpdateClientGhelld, this ) )
		return FALSE;
	if(!m_pcsAgcmPrivateTrade->SetCallbackTradeRequestRefuseUser( CBPrivateTradeRequestRefuseUser, this ) )		//	2005.06.02. By SungHoon
		return FALSE;

	// Private Trade Option
	if( !m_pcsAgpmItem->SetCallbackUIUpdateTradeOption( CBUpdateTradeOptionGrid , this ) )
		return FALSE;

	//Private Trade MSG
	if( !m_pcsAgcmPrivateTrade->SetCallbackTradeMSGComplete( CBPrivateTradeMSGComplete, this ) )
		return FALSE;
	if( !m_pcsAgcmPrivateTrade->SetCallbackTradeMSGCancel( CBPrivateTradeMSGCancel, this ) )
		return FALSE;

	//Sales Box.
	if( !m_pcsAgpmItem->SetCallbackUIUpdateSalesBox( CBUpdateSalesBoxGrid, this) )
		return FALSE;

	if( m_pcsAgcmUIAuction && !m_pcsAgcmUIAuction->SetCallbackSetFocus( CBSetFocusOnBoard, this ) )
		return FALSE;

	if( m_pcsAgcmUIAuction && !m_pcsAgcmUIAuction->SetCallbackKillFocus( CBKillFocusOnBoard, this ) )
		return FALSE;


	//Sales Box2
	if( m_pcsAgcmAuction && !m_pcsAgcmAuction->SetCallbackSalesBox2Add( CBSalesBox2Add, this ) )
		return FALSE;

	if( m_pcsAgcmAuction && !m_pcsAgcmAuction->SetCallbackSalesBox2Remove( CBSalesBox2Remove, this ) )
		return FALSE;

	if( m_pcsAgcmAuction && !m_pcsAgcmAuction->SetCallbackSalesBox2Update( CBSalesBox2Update, this ) )
		return FALSE;

	//Duplicate Inven

	//NPC Trade
	if(!m_pcsAgcmEventNPCTrade->SetCallbackUpdateNPCTradeGrid( CBUIUpdateNPCTradeGrid, this ) ) 
		return FALSE;
	if(!m_pcsAgcmEventNPCTrade->SetCallbackUpdateInventoryGrid( CBUIUpdateInventory, this ) ) 
		return FALSE;
	if(!m_pcsAgcmEventNPCTrade->SetCallbackOpenUINPCTrade( CBNPCTradeUIOpen, this ) ) 
		return FALSE;
	if(!m_pcsAgcmEventNPCTrade->SetCallbackInvenFull( CBUINPCTradeMSGFullInven, this ) )
		return FALSE;
	/*
	if(!m_pcsAgcmEventNPCTrade->SetCallbackBuyItem( CBUINPCTradeMSGConfirmBuy, this ) )
		return FALSE;
	if(!m_pcsAgcmEventNPCTrade->SetCallbackSellItem( CBUINPCTradeMSGConfirmSell, this ) )
		return FALSE;
	*/
	if(!m_pcsAgcmEventNPCTrade->SetCallbackBuyItem( CBNPCTradeSuccess, this ) )
		return FALSE;
	if(!m_pcsAgcmEventNPCTrade->SetCallbackSellItem( CBNPCTradeSuccess, this ) )
		return FALSE;
	if(!m_pcsAgcmEventNPCTrade->SetCallbackNotEnoughGhelld( CBUINPCTradeMSGNotEnoughGhelld, this ) )
		return FALSE;
	if(!m_pcsAgcmEventNPCTrade->SetCallbackNotEnoughSpaceToReturnGhelld( CBUINPCTradeMSGNotEnoughSpaceToGetGhelld, this ) )
		return FALSE;

	if(!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(SelfUpdatePositionCB, this))
		return FALSE;

	// pickup item (money)
	if (!m_pcsAgpmItem->SetCallbackPickupItemResult(CBPickupItemResult, this))
		return FALSE;
	// item convert
	if(!m_pcsAgpmItemConvert->SetCallbackAskReallySpiritStoneConvert( CBAskReallyConvert, this ) )
		return FALSE;
	if(!m_pcsAgpmItemConvert->SetCallbackAskReallyRuneConvert( CBAskReallyConvert, this ) )
		return FALSE;
	if(!m_pcsAgpmItemConvert->SetCallbackResultSpiritStoneConvert( CBConvertResult, this ) )
		return FALSE;
	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackOpenTooltip(CBOpenQuickBeltToolTip, this))
		return FALSE;
	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackCloseTooltip(CBCloseQuickBeltToolTip, this))
		return FALSE;
	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackCloseAllUITooltip(CBCloseAllUIToolTip, this))
		return FALSE;
	if (m_pcsAgpmItem && !m_pcsAgpmItem->SetCallbackUpdateFactor(CBUpdateItemFactor, this))
		return FALSE;
//	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackUpdateAlarmGrid(CBUpdateAlarmGrid, this))
//		return FALSE;
	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackKeydownESC(CBClosePrivateTradeUI,this))
		return FALSE;
	if (m_pcsAgcmUIMain && !m_pcsAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	if (m_pcsAgpmItem && !m_pcsAgpmItem->SetCallbackUseItemSuccess(CBUseItemSuccess, this))
		return FALSE;
	if (m_pcsAgpmItem && !m_pcsAgpmItem->SetCallbackUseItemByTID(CBUseItemByTID, this))
		return FALSE;
	if (m_pcsAgpmItem && !m_pcsAgpmItem->SetCallbackUseItemFailedByTID(CBUseItemFailedByTID, this))
		return FALSE;

	if (m_pcsAgpmItem && !m_pcsAgpmItem->SetCallbackUpdateReuseTimeForReverseOrb(CBUpdateReuseTimeForReverseOrb, this))
		return FALSE;
	if (m_pcsAgpmItem && !m_pcsAgpmItem->SetCallbackUpdateReuseTimeForTransform(CBUpdateReuseTimeForTransform, this))
		return FALSE;
	if (m_pcsAgpmItem && !m_pcsAgpmItem->SetCallbackInitReuseTimeForTransform(CBInitReuseTimeForTransform, this))
		return FALSE;

	/*
	if (m_pcsAgcmUICharacter && !m_pcsAgcmUICharacter->SetCallbackRefreshUpdateCharStatus(CBRefreshUpdateCharStatus, this))
		return FALSE;
	*/

	if (m_pcsAgpmFactors && !m_pcsAgpmFactors->SetCallbackUpdateFactorDurability(CBUpdateFactorDurability, this))
		return FALSE;

	if (!m_pcsAgpmReturnToLogin->SetCallbackEndProcess(CBReturnToLoginEnd, this))
		return FALSE;

	if (m_pcsAgpmItem && !m_pcsAgpmItem->SetCallbackUpdateCooldown(CBUpdateCooldown, this))
		return FALSE;

	if(!AddEvent())
		return FALSE;

	if(!AddFunction())
		return FALSE;

	if(!AddDisplay())
		return FALSE;

	if(!AddUserData())
		return FALSE;

	if (!AddBoolean())
		return FALSE;
	
	return TRUE;
}

BOOL AgcmUIItem::OnDestroy()
{
	if (m_pcsReturnScrollGridItem)
	{
		m_pcsAgpmGrid->DeleteGridItem(m_pcsReturnScrollGridItem);
		m_pcsReturnScrollGridItem	= NULL;
	}

	for (int i = 0; i < AGPMITEM_PART_NUM; ++i)
		m_pcsAgpmGrid->Remove(&m_csEquipSlotGrid[i]);

	return TRUE;
}

BOOL AgcmUIItem::OnInit()
{
	if ( m_pcsAgcmUIManager2 != NULL )
	{
		m_csItemToolTip.m_Property.bTopmost = TRUE								;
		m_pcsAgcmUIManager2->AddWindow( (AgcWindow*)(&m_csItemToolTip ) )	;
		m_csItemToolTip.ShowWindow( FALSE )										;
	}

	for (int i = 0; i < AGPMITEM_PART_NUM; ++i)
	{
		m_csEquipSlotGrid[i].m_lGridType	= AGPDGRID_ITEM_TYPE_ITEM;
		m_pcsAgpmGrid->Init(&m_csEquipSlotGrid[i], 1, 1, 1);
	}

	for (int i=0; i<AGPMAUCTION_MAX_REGISTER; i++ )
	{
		m_csSalesBox2Grid[i].m_lGridType	= AGPDGRID_ITEM_TYPE_ITEM;
		m_pcsAgpmGrid->Init(&m_csSalesBox2Grid[i], 1, 1, 1);
	}
	
	// Support 아이템 그리드 초기화
	m_pcsAgpmGrid->Init( &m_PrivateTradeGridEx , 1 , 3 , 1 );
	m_PrivateTradeGridEx.m_lGridType		=	AGPDGRID_ITEM_TYPE_ITEM;

	m_pcsAgpmGrid->Init( &m_PrivateTradeClientGridEx , 1 , 3 , 1 );
	m_PrivateTradeClientGridEx.m_lGridType	=	AGPDGRID_ITEM_TYPE_ITEM;

	AgcmUIConsole* pcsAgcmUIConsole = (AgcmUIConsole*)GetModule("AgcmUIConsole");
	if(pcsAgcmUIConsole)
	{
		AS_REGISTER_TYPE_BEGIN(AgcmUIItem, AgcmUIItem);
		AS_REGISTER_METHOD0(void, EnableSubInventory);
		AS_REGISTER_METHOD0(void, DisableSubInventory);

		AS_REGISTER_TYPE_END;
	}

	m_AutoPickUp.LoadFromFile( "ini\\AutoPickUp.xml" );
	_InitTooltipColor();

	return TRUE;
}

BOOL AgcmUIItem::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmUIItem::OnIdle");

	if (m_ulPrevProcessClock + AGCMUIITEM_PROCESS_IDLE_INTERVAL > ulClockCount)
		return TRUE;

	// 내구력 감소에 의한 Alarm Grid에 추가된 넘들의 시간을 계산해
	// 정해진 시간이 지난 넘들을 Alarm Grid에서 삭제해준다.

	BOOL	bUpdateAlarmGrid	= FALSE;
	for (int i = 0; i < m_lNumListNotifyDurabilityZero; ++i)
	{
		if (m_aulRegisterTimeNotifyDurabilityZero[i] + AGCMUIITEM_NOTIFY_DURABILITY_TIME < ulClockCount)
		{
			// 정해진 시간이 지났으니 Alarm Grid에서 삭제한다.
			RemoveItemDurabilityZeroList(m_pcsAgpmItem->GetItem(m_alListNotifyDurabilityZero[i]));

			bUpdateAlarmGrid	= TRUE;
		}
	}

	for (int i = 0; i < m_lNumListNotifyDurabilityUnder5Percent; ++i)
	{
		if (m_aulRegisterTimeNotifyDurabilityUnder5Percent[i] + AGCMUIITEM_NOTIFY_DURABILITY_TIME < ulClockCount)
		{
			// 정해진 시간이 지났으니 Alarm Grid에서 삭제한다.
			RemoveItemDurabilityUnder5PercentList(m_pcsAgpmItem->GetItem(m_alListNotifyDurabilityUnder5Percent[i]));

			bUpdateAlarmGrid	= TRUE;
		}
	}

	//if (bUpdateAlarmGrid)
	//	m_pcsAgcmUIMain->UpdateAlarmGrid();

	m_ulPrevProcessClock = ulClockCount;

	return TRUE;
}

BOOL AgcmUIItem::AddEvent()
{
	//Inventory , Equip
	m_lInventoryUIOpen = m_pcsAgcmUIManager2->AddEvent("INV_OpenEvent");
	if( m_lInventoryUIOpen < 0 )
		return FALSE;
	m_lInventoryUIClose = m_pcsAgcmUIManager2->AddEvent("INV_CloseEvent");
	if( m_lInventoryUIClose < 0 )
		return FALSE;
	m_lInventoryDropMoneyUIOpen = m_pcsAgcmUIManager2->AddEvent("INV_OpenDropMoney");
	if( m_lInventoryDropMoneyUIOpen < 0 )
		return FALSE;
	m_lInventoryDropMoneyUIClose = m_pcsAgcmUIManager2->AddEvent("INV_CloseDropMoney");
	if( m_lInventoryDropMoneyUIClose < 0 )
		return FALSE;
	m_lInventoryEnableDropGhelld = m_pcsAgcmUIManager2->AddEvent("INV_EnableDropGhelld" );
	if( m_lInventoryEnableDropGhelld < 0 )
		return FALSE;
	m_lInventoryDisableDropGhelld = m_pcsAgcmUIManager2->AddEvent("INV_DisableDropGhelld" );
	if( m_lInventoryDisableDropGhelld < 0 )
		return FALSE;

	m_lSalesBoxCancelMessageBox = m_pcsAgcmUIManager2->AddEvent( "AC_CancelMessageBox", CBReturnConfirmCancelItemFromAuction, this );
	if( m_lSalesBoxCancelMessageBox < 0 )
		return FALSE;

	//Inventory , Equip MSG
	m_lInventoryMSGFull = m_pcsAgcmUIManager2->AddEvent("INV_MSG_Full");
	if( m_lInventoryUIClose < 0 )
		return FALSE;
	m_lInventoryMSGNotEnoughSpaceToGetGhelld = m_pcsAgcmUIManager2->AddEvent("INV_MSG_NOT_ENOUGH_SPACE_TO_GET_GHELLD");
	if( m_lInventoryMSGNotEnoughSpaceToGetGhelld < 0 )
		return FALSE;
	m_lInventoryMSGGetItem = m_pcsAgcmUIManager2->AddEvent( "INV_MSG_GET_ITEM" );
	if( m_lInventoryMSGGetItem < 0 )
		return FALSE;
	m_lInventoryMSGDropItem = m_pcsAgcmUIManager2->AddEvent( "INV_MSG_Drop_ITEM", CBUIInventoryDropItem, this );
	if( m_lInventoryMSGDropItem < 0 )
		return FALSE;
	m_lInventoryMSGDropGhelld = m_pcsAgcmUIManager2->AddEvent( "INV_MSG_Drop_Ghelld", CBUIInventoryDropGhelld, this );
	if( m_lInventoryMSGDropGhelld < 0 )
		return FALSE;

	//Bank
	m_lBankUIOpen = m_pcsAgcmUIManager2->AddEvent("Bank_OpenEvent");
	if( m_lBankUIOpen < 0 )
		return FALSE;
	m_lBankUIClose = m_pcsAgcmUIManager2->AddEvent("Bank_CloseEvent");
	if( m_lBankUIClose < 0 )
		return FALSE;

	//Private Trade
	m_lRequestPrivateTradeUIOpen = m_pcsAgcmUIManager2->AddEvent("PT_OpenRequest");
	if (m_lRequestPrivateTradeUIOpen < 0) 
		return FALSE;
	m_lRequestPrivateTradeUIClose = m_pcsAgcmUIManager2->AddEvent("PT_CloseRequest");
	if (m_lRequestPrivateTradeUIClose < 0) 
		return FALSE;
	m_lPrivateTradeOpenUIYesNo = m_pcsAgcmUIManager2->AddEvent("PT_OpenConfirmEvent");
	if (m_lPrivateTradeOpenUIYesNo < 0)
		return FALSE;
	m_lPrivateTradeCloseUIYesNo = m_pcsAgcmUIManager2->AddEvent("PT_CloseConfirmEvent");
	if (m_lPrivateTradeOpenUIYesNo < 0)
		return FALSE;
	m_lPrivateTradeOpenUI = m_pcsAgcmUIManager2->AddEvent("PT_OpenUIEvent");
	if( m_lPrivateTradeOpenUI < 0 )
		return FALSE;
	m_lPrivateTradeCloseUI = m_pcsAgcmUIManager2->AddEvent("PT_CloseUIEvent");
	if( m_lPrivateTradeOpenUI < 0 )
		return FALSE;
	m_lPrivateTradeClientLock = m_pcsAgcmUIManager2->AddEvent("PT_ClientLock");
	if( m_lPrivateTradeClientLock < 0 )
		return FALSE;
	m_lPrivateTradeClientUnlock = m_pcsAgcmUIManager2->AddEvent("PT_ClientUnlock");
	if( m_lPrivateTradeClientUnlock < 0 )
		return FALSE;
	m_lPrivateTradeActiveReadyToExchange = m_pcsAgcmUIManager2->AddEvent("PT_ActiveReadyToExchange");
	if( m_lPrivateTradeActiveReadyToExchange < 0 )
		return FALSE;
	m_lPrivateTradeClientReadyToExchange = m_pcsAgcmUIManager2->AddEvent("PT_ClientReadyToExchange");
	if( m_lPrivateTradeClientReadyToExchange < 0 )
		return FALSE;
	//Private Trade MSG Event
	m_lPrivateTradeMSGInventoryFull = m_pcsAgcmUIManager2->AddEvent( "PTMSG_InvenFull" );
	if( m_lPrivateTradeMSGInventoryFull < 0 )
		return FALSE;
	m_lPrivateTradeMSGTradeComplete = m_pcsAgcmUIManager2->AddEvent( "PTMSG_TradeComplete" );
	if( m_lPrivateTradeMSGTradeComplete < 0 )
		return FALSE;
	m_lPrivateTradeMSGTradeCancel = m_pcsAgcmUIManager2->AddEvent( "PTMSG_TradeCancel" );
	if( m_lPrivateTradeMSGTradeCancel < 0 )
		return FALSE;
	m_lPrivateTradeMSGError = m_pcsAgcmUIManager2->AddEvent( "PTMSG_Error" );
	if( m_lPrivateTradeMSGError < 0 )
		return FALSE;

	//NPCTrade
	m_lNPCTradeOpenUI = m_pcsAgcmUIManager2->AddEvent("NPCT_OpenUI");;
	if( m_lNPCTradeOpenUI < 0 )
		return FALSE;
	m_lNPCTradeCloseUI = m_pcsAgcmUIManager2->AddEvent("NPCT_CloseUI");;
	if( m_lNPCTradeCloseUI < 0 )
		return FALSE;
	m_lNPCTradeInitLayer = m_pcsAgcmUIManager2->AddEvent("NPCT_InitLayer");;
	if( m_lNPCTradeInitLayer < 0 )
		return FALSE;

	//Sales Box
	m_lSalesBoxUIOpen = m_pcsAgcmUIManager2->AddEvent( "SalesBox_Open" );
	if( m_lSalesBoxUIOpen < 0 )
		return FALSE;

	m_lSalesBoxUIClose = m_pcsAgcmUIManager2->AddEvent( "SalesBox_Close" );
	if( m_lSalesBoxUIClose < 0 )
		return FALSE;

	//SalesBox2
	m_lSalesBox2Add = m_pcsAgcmUIManager2->AddEvent( "SalesBox2_Add" );
	if( m_lSalesBox2Add < 0 )
		return FALSE;

	m_lSalesBox2Remove = m_pcsAgcmUIManager2->AddEvent( "SalesBox2_Remove" );
	if( m_lSalesBox2Remove < 0 )
		return FALSE;

	m_lSalesBox2Update = m_pcsAgcmUIManager2->AddEvent( "SalesBox2_Update" );
	if( m_lSalesBox2Update < 0 )
		return FALSE;

	m_lSalesBox2ButtonOn = m_pcsAgcmUIManager2->AddEvent( "SalesBox2_ButtonOn" );
	if( m_lSalesBox2ButtonOn < 0 )
		return FALSE;

	m_lSalesBox2ButtonOff = m_pcsAgcmUIManager2->AddEvent( "SalesBox2_ButtonOff" );
	if( m_lSalesBox2ButtonOff < 0 )
		return FALSE;

	m_lSubInventoryUIOpen = m_pcsAgcmUIManager2->AddEvent("SubInv_OpenEvent");
	if( m_lSubInventoryUIOpen < 0 )
		return FALSE;
	m_lSubInventoryUIClose = m_pcsAgcmUIManager2->AddEvent("SubInv_CloseEvent");
	if( m_lSubInventoryUIClose < 0 )
		return FALSE;

	//NPCTrade MSG Event
	m_lNPCTradeMSGInventoryFull = m_pcsAgcmUIManager2->AddEvent( "NPCTMSG_InvenFull" );
	if( m_lNPCTradeMSGInventoryFull < 0 )
		return FALSE;
	m_lNPCTradeMSGConfirmBuy = m_pcsAgcmUIManager2->AddEvent( "NPCTMSG_ConfirmBuy", CBUINPCTradeMSGConfirmBuy, this );
	if( m_lNPCTradeMSGConfirmBuy < 0 )
		return FALSE;
	m_lNPCTradeMSGConfirmBuyStackCount = m_pcsAgcmUIManager2->AddEvent( "NPCTMSG_ConfirmBuyStackCount" );
	if ( m_lNPCTradeMSGConfirmBuyStackCount < 0 )
		return FALSE;
	m_lNPCTradeMSGConfirmSell = m_pcsAgcmUIManager2->AddEvent( "NPCTMSG_ConfirmSell", CBUINPCTradeMSGConfirmSell, this );
	if( m_lNPCTradeMSGConfirmSell < 0 )
		return FALSE;
	m_lNPCTradeMSGNotEnoughGhelld = m_pcsAgcmUIManager2->AddEvent( "NPCTMSG_NotEnoughGhelld" );
	if( m_lNPCTradeMSGNotEnoughGhelld < 0 )
		return FALSE;
	m_lNPCTradeMSGNotEnoughSpaceToGetGhelld = m_pcsAgcmUIManager2->AddEvent( "NPCTMSG_NotEnoughSpaceToGetGhelld" );
	if( m_lNPCTradeMSGNotEnoughSpaceToGetGhelld < 0 )
		return FALSE;
	m_lNPCTradeSuccess = m_pcsAgcmUIManager2->AddEvent("NPCTMSG_TradeSuccess");
	if (m_lNPCTradeSuccess < 0)
		return FALSE;

	// Item Convert
	m_lUIOpenAskConvert = m_pcsAgcmUIManager2->AddEvent("IC_ASK_CONVERT");
	if( m_lUIOpenAskConvert < 0 )
		return FALSE;

	m_lUIOpenConvertResultSuccess = m_pcsAgcmUIManager2->AddEvent("IC_RESULT_SUCCESS");
	if( m_lUIOpenConvertResultSuccess < 0 )
		return FALSE;

	m_lUIOpenConvertResultFail = m_pcsAgcmUIManager2->AddEvent("IC_RESULT_FAIL");
	if( m_lUIOpenConvertResultFail < 0 )
		return FALSE;

	m_lUIOpenConvertResultDestroyAttribute = m_pcsAgcmUIManager2->AddEvent("IC_RESULT_RUIN");
	if( m_lUIOpenConvertResultDestroyAttribute < 0 )
		return FALSE;

	m_lUIOpenConvertResultDestroyItem = m_pcsAgcmUIManager2->AddEvent("IC_RESULT_DESTROY_ITEM");
	if( m_lUIOpenConvertResultDestroyItem < 0 )
		return FALSE;

	m_lUIOpenConvertResultDifferRank = m_pcsAgcmUIManager2->AddEvent("IC_RESULT_DIFFER_RANK");
	if( m_lUIOpenConvertResultDifferRank < 0 )
		return FALSE;

	m_lUIOpenConvertResultDifferType = m_pcsAgcmUIManager2->AddEvent("IC_RESULT_DIFFER_TYPE");
	if( m_lUIOpenConvertResultDifferType < 0 )
		return FALSE;

	m_lUIOpenConvertResultFull = m_pcsAgcmUIManager2->AddEvent("IC_RESULT_ALREADY_FULL");
	if( m_lUIOpenConvertResultFull < 0 )
		return FALSE;

	m_lUIOpenConvertResultNotEquipItem = m_pcsAgcmUIManager2->AddEvent("IC_RESULT_NOT_EQUIP_ITEM");
	if( m_lUIOpenConvertResultNotEquipItem < 0 )
		return FALSE;

	m_lUIOpenConvertResultEgoItem = m_pcsAgcmUIManager2->AddEvent("IC_RESULT_EGO_ITEM");
	if( m_lUIOpenConvertResultEgoItem < 0 )
		return FALSE;

	m_lEventGetItem = m_pcsAgcmUIManager2->AddEvent("Item_GetItem");
	if (m_lEventGetItem < 0)
		return FALSE;

	m_lEventGetMoney = m_pcsAgcmUIManager2->AddEvent("Item_GetMoney");
	if (m_lEventGetMoney < 0)
		return FALSE;

	m_lEventPickupItemSuccess = m_pcsAgcmUIManager2->AddEvent("Item_SuccessPickupItem");
	if (m_lEventPickupItemSuccess < 0)
		return FALSE;

	m_lEventPickupItemMoneySuccess = m_pcsAgcmUIManager2->AddEvent("Item_SuccessPickupMoney");
	if (m_lEventPickupItemMoneySuccess < 0)
		return FALSE;

	m_lEventPickupItemPCBangMoneySuccess = m_pcsAgcmUIManager2->AddEvent("Item_SuccessPickupPCBangBonusMoney");
	if (m_lEventPickupItemPCBangMoneySuccess < 0)
		return FALSE;
	
	m_lEventPickupItemFail = m_pcsAgcmUIManager2->AddEvent("Item_FailPickupItem");
	if (m_lEventPickupItemFail < 0)
		return FALSE;

	m_lEventPickupItemMoneyFail = m_pcsAgcmUIManager2->AddEvent("Item_FailPickupMoney");
	if (m_lEventPickupItemMoneyFail < 0)
		return FALSE;

	m_lEventAutoPickupItem = m_pcsAgcmUIManager2->AddEvent("Item_AutoPickupItem");
	if (m_lEventAutoPickupItem < 0)
		return FALSE;

//	m_lEventEnableReturnScroll = m_pcsAgcmUIManager2->AddEvent("Item_EnableReturnScroll");
//	if (m_lEventEnableReturnScroll < 0)
//		return FALSE;
//	m_lEventDisableReturnScroll = m_pcsAgcmUIManager2->AddEvent("Item_DisableReturnScroll");
//	if (m_lEventDisableReturnScroll < 0)
//		return FALSE;

	m_lEventUseSuccessReverseOrb	= m_pcsAgcmUIManager2->AddEvent("Item_UseSuccessReverseOrb");
	if (m_lEventUseSuccessReverseOrb < 0)
		return FALSE;

	m_lEventUseFailReverseOrb		= m_pcsAgcmUIManager2->AddEvent("Item_UseFailReverseOrb");
	if (m_lEventUseFailReverseOrb < 0)
		return FALSE;

	m_lEventUseAddMovementSkill		= m_pcsAgcmUIManager2->AddEvent("Item_UseAddMovementSkill");
	if (m_lEventUseAddMovementSkill < 0)
		return FALSE;

	m_lEventUseAddAtkSpeedSkill		= m_pcsAgcmUIManager2->AddEvent("Item_UseAddAtkSpeedSkill");
	if (m_lEventUseAddAtkSpeedSkill < 0)
		return FALSE;

	m_lEventExchangeBuffSkill		= m_pcsAgcmUIManager2->AddEvent("Item_ExchangeBuffSkill");
	if (m_lEventExchangeBuffSkill < 0)
		return FALSE;

	m_lEventIgnoreBuffSkill			= m_pcsAgcmUIManager2->AddEvent("Item_IgnoreBuffSkill");
	if (m_lEventIgnoreBuffSkill < 0)
		return FALSE;

	m_lEventTradeFailForBoundItem	= m_pcsAgcmUIManager2->AddEvent("Item_TradeFailForBoundItem");
	if (m_lEventTradeFailForBoundItem < 0)
		return FALSE;

	m_lEventConfirmDestroyItem		= m_pcsAgcmUIManager2->AddEvent("Item_ConfirmDestroyItem", CBUIInventoryDestroyItem, this);
	if (m_lEventConfirmDestroyItem < 0)
		return FALSE;

	m_lEventConfirmDestroyCashItem	= m_pcsAgcmUIManager2->AddEvent("Item_ConfirmDestroyCashItem", CBUIInventoryDestroyItem, this);
	if (m_lEventConfirmDestroyCashItem < 0)
		return FALSE;

	m_lEventTradeFailForUsingCashItem = m_pcsAgcmUIManager2->AddEvent("Item_TradeFailForUsingCashItem");
	if (m_lEventTradeFailForUsingCashItem < 0 )
		return FALSE;

	m_lEventEquipStatusHeadOn		= m_pcsAgcmUIManager2->AddEvent("EquipStatusHeadOn");
	if (m_lEventEquipStatusHeadOn < 0)	return FALSE;
	m_lEventEquipStatusHandOn		= m_pcsAgcmUIManager2->AddEvent("EquipStatusHandOn");
	if (m_lEventEquipStatusHandOn < 0)	return FALSE;
	m_lEventEquipStatusBodyOn		= m_pcsAgcmUIManager2->AddEvent("EquipStatusBodyOn");
	if (m_lEventEquipStatusBodyOn < 0)	return FALSE;
	m_lEventEquipStatusLegsOn		= m_pcsAgcmUIManager2->AddEvent("EquipStatusLegsOn");
	if (m_lEventEquipStatusLegsOn < 0)	return FALSE;
	m_lEventEquipStatusFootOn		= m_pcsAgcmUIManager2->AddEvent("EquipStatusFootOn");
	if (m_lEventEquipStatusFootOn < 0)	return FALSE;
	m_lEventEquipStatusShieldOn		= m_pcsAgcmUIManager2->AddEvent("EquipStatusShieldOn");
	if (m_lEventEquipStatusShieldOn < 0)	return FALSE;
	m_lEventEquipStatusWeaponOn		= m_pcsAgcmUIManager2->AddEvent("EquipStatusWeaponOn");
	if (m_lEventEquipStatusWeaponOn < 0)	return FALSE;

	m_lEventEquipStatusHeadOff		= m_pcsAgcmUIManager2->AddEvent("EquipStatusHeadOff");
	if (m_lEventEquipStatusHeadOff < 0)	return FALSE;
	m_lEventEquipStatusHandOff		= m_pcsAgcmUIManager2->AddEvent("EquipStatusHandOff");
	if (m_lEventEquipStatusHandOff < 0)	return FALSE;
	m_lEventEquipStatusBodyOff		= m_pcsAgcmUIManager2->AddEvent("EquipStatusBodyOff");
	if (m_lEventEquipStatusBodyOff < 0)	return FALSE;
	m_lEventEquipStatusLegsOff		= m_pcsAgcmUIManager2->AddEvent("EquipStatusLegsOff");
	if (m_lEventEquipStatusLegsOff < 0)	return FALSE;
	m_lEventEquipStatusFootOff		= m_pcsAgcmUIManager2->AddEvent("EquipStatusFootOff");
	if (m_lEventEquipStatusFootOff < 0)	return FALSE;
	m_lEventEquipStatusShieldOff	= m_pcsAgcmUIManager2->AddEvent("EquipStatusShieldOff");
	if (m_lEventEquipStatusShieldOff < 0)	return FALSE;
	m_lEventEquipStatusWeaponOff	= m_pcsAgcmUIManager2->AddEvent("EquipStatusWeaponOff");
	if (m_lEventEquipStatusWeaponOff < 0)	return FALSE;

	m_lEventEquipStatusOpen			= m_pcsAgcmUIManager2->AddEvent("EquipStatusOpen");
	if (m_lEventEquipStatusOpen < 0)	return FALSE;
	m_lEventEquipStatusClose		= m_pcsAgcmUIManager2->AddEvent("EquipStatusClose");
	if (m_lEventEquipStatusClose < 0)	return FALSE;

	m_lEventFullBankSlot			= m_pcsAgcmUIManager2->AddEvent("BankSlotFull");
	if (m_lEventFullBankSlot < 0)		return FALSE;

	m_lEventReverseOrbError			= m_pcsAgcmUIManager2->AddEvent("ReverseOrbError");
	if (m_lEventReverseOrbError < 0)	return FALSE;

	m_nEventGetBtnAutoPickUp		= m_pcsAgcmUIManager2->AddEvent("SubInven_GetBtnAutoPickUp");
	if (m_nEventGetBtnAutoPickUp < 0 )	return FALSE;

	return TRUE;
}

BOOL AgcmUIItem::AddFunction()
{
	//Inventory
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Inventory_MoveItem", CBInventoryMoveItem, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Inventory_MoveEndItem", CBInventoryMoveEndItem, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Inventory_UIUpdate", CBInventoryUIUpdate, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Inventory_UIUpdateLayer", CBInventoryOpenInvenLayer, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Inventory_UseItem", CBUseItem, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Inventory_DragDropItem", CBDragDropItem, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Inventory_OpenDropMoneyUI", CBDropMoneyToField, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Inventory_DropMoneyYes", CBDropMoneyToFieldYes, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Inventory_DropMoneyCancel", CBDropMoneyToFieldCancel, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "InventorySmall_Layer0", CBInventorySmallLayer0, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "InventorySmall_Layer1", CBInventorySmallLayer1, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "InventorySmall_Layer2", CBInventorySmallLayer2, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "InventorySmall_Layer3", CBInventorySmallLayer3, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "InventoryDoubleClick", CBInventoryDoubleClick, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "InventoryWaste_DragDropItem", CBWasteDragDropItem, 1 ) )
		return FALSE;


	//Equip
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Equip_MoveItem", CBEquipMoveItem, 1 ))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Equip_DragDropBase", CBEquipDragDropBase, 1 ))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Equip_DoubleClick", CBEquipDoubleClick, 1 ) )
		return FALSE;

	// Sub Inventory
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SubInventory_MoveItem", CBSubInventoryMoveItem, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "SubInventory_MoveEndItem", CBSubInventoryMoveEndItem, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "SubInventory_UIUpdate", CBSubInventoryUIUpdate, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "SubInventory_UIUpdateLayer", CBSubInventoryOpenInvenLayer, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "SubInventory_DragDropItem", CBDragDropItemToSubInventory, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "SubInventory_ToggleAutoGetDropItem", CBToggleAutoGetDropItem, 1 ) )
		return FALSE;


	//NPCTrade
	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCT_MoveItem", CBNPCTradeMoveItem, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCT_UIOpen", CBNPCTradeUIOpen, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCT_UIClose", CBNPCTradeUIClose, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCT_MoveNextLayer", CBNPCTradeOpenNextLayer, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCT_MovePrevLayer", CBNPCTradeOpenPrevLayer, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCT_InitLayer", CBNPCTradeInitLayer, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCT_BuyStackCountOK", CBUINPCTradeMSGConfirmBuyStackCountOK, 1, "Stack Count Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCT_BuyStackCountCancel", CBUINPCTradeMSGConfirmBuyStackCountCancel, 1, "Stack Count Edit Control"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCT_BuyItemDirect", CBBuyItemDirect, 1 ))
		return FALSE;

	//Private Trade
	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_RequestCancel", CBRequestCancel, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_Accept", CBPrivateTradeAccept, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_Reject", CBPrivateTradeReject, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_MoveItem", CBPrivateTradeMoveItem, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_UpdateMoney", CBPrivateTradeUpdateMoney, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_Lock", CBPrivateTradeLock, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_Unlock", CBPrivateTradeUnlock, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_ReadyToExchange", CBPrivateTradeReadyToExchange, 1 ))
		return FALSE;

//	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_ClientLock", CBPrivateTradeClientLock, 1 ))
//		return FALSE;

//	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_ClientUnlock", CBPrivateTradeClientUnlock, 1 ))
//		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_Cancel", CBPrivateTradeCancel, 1 ))
		return FALSE;

//	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_SetClientReadyToExchange", CBPrivateTradeSetClientReadyToExchange, 1 ))
//		return FALSE;

//	if (!m_pcsAgcmUIManager2->AddFunction(this, "PT_ResetClientReadyToExchange", CBPrivateTradeResetClientReadyToExchange, 1 ))
//		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "ItemAutoPickup", CBAutoPickupItem, 0 ))
		return FALSE;

	//Sales Box
	if (!m_pcsAgcmUIManager2->AddFunction(this, "AC_SalesBox2Open", CBSalesBox2Open, 1))
		return FALSE;

	//Sales Box
	if (!m_pcsAgcmUIManager2->AddFunction(this, "AC_MoveToSalesBox", CBSalesBoxMoveItem, 1))
		return FALSE;

	//Sales Box2
	if (!m_pcsAgcmUIManager2->AddFunction(this, "AC_SalesBox2CancelComplete", CBSalesBox2ClickCancelCompleteButton, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "AC_SalesBox2Add", CBSalesBox2Add, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "AC_SalesBox2Remove", CBSalesBox2Remove, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "AC_SalesBox2Update", CBSalesBox2Update, 1 ) )
		return FALSE;

	//Duplicate Inven
	if (!m_pcsAgcmUIManager2->AddFunction(this, "AC_FunctionUIOpen", CBOpenDuplicateInven, 0 ))
		return FALSE;

	// Item Convert
	if (!m_pcsAgcmUIManager2->AddFunction(this, "IC_ConvertItem", CBSendConvertItem, 0 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "IC_CancelConvertItem", CBCancelConvertItem, 0 ))
		return FALSE;

	// 98pastel - Tool Tip 관련 
	if (!m_pcsAgcmUIManager2->AddFunction( this, "Grid_OpenItemToolTip", CBOpenGridItemToolTip, 0 ))
		return FALSE;
	
	if (!m_pcsAgcmUIManager2->AddFunction( this, "Grid_CloseItemToolTip", CBCloseGridItemToolTip, 0 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction( this, "PT_RequestTrade", CBRequestPrivateTrade, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction( this, "Item_OpenReturnScrollUI", CBOpenReturnScroll, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction( this, "Item_UseReturnScroll", CBUseReturnScroll, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction( this, "Item_CancelReturnScroll", CBCancelReturnScroll, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction( this, "Item_UseReverseOrb", CBUseReverseOrb, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction( this, "Item_GetBtnAutoPickUp", CBGetBtnAutoPickUp, 0 ) )
		return FALSE;

	return TRUE;
}

BOOL AgcmUIItem::AddDisplay()
{
	// add inventory money
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "InventoryMoney", AGCMUI_ITEM_DISPLAY_ID_INVENTORY_MONEY, CBDisplayInventoryMoney, AGCDUI_USERDATA_TYPE_UINT64))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PrivateTraderName", AGCMUI_ITEM_DISPLAY_ID_PRIVATETRADERNAME, CBDisplayPrivateTraderName, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Diaplay_PT_Request", AGCMUI_ITEM_DISPLAY_ID_REQUEST_PRIVATETRADE, CBDisplayRequestPrivateTrade, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ItemPickupName", AGCMUI_ITEM_DISPLAY_ID_MESSAGE_PICKUP_ITEM_NAME, CBDisplayMessageData, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ItemPickupBonusMoney", AGCMUI_ITEM_DISPLAY_ID_MESSAGE_PICKUP_BONUS_MONEY, CBDisplayMessageData, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PrivateTradeSetName", AGCMUI_ITEM_DISPLAY_ID_SET_NAME, CBDisplayPrivateTradeSetName, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PrivateTradeSetClientName", AGCMUI_ITEM_DISPLAY_ID_SET_CLIENTNAME, CBDisplayPrivateTradeSetClientName, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PrivateTradeGhelld", AGCMUI_ITEM_DISPLAY_ID_PT_GHELLD, CBDisplayPrivateTradeGhelld, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "PrivateTradeClientGhelld", AGCMUI_ITEM_DISPLAY_ID_PT_CLIENTGHELLD, CBDisplayPrivateTradeClientGhelld, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ItemUseAddMovementSkill", AGCMUI_ITEM_DISPLAY_ID_USE_ADD_MOVEMENT_SKILL, CBDisplayUseAddMovementSkill, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ItemUseAddAtkSpeedSkill", AGCMUI_ITEM_DISPLAY_ID_USE_ADD_ATKSPEED_SKILL, CBDisplayUseAddAtkSpeedSkill, AGCDUI_USERDATA_TYPE_CHARACTER))
		return FALSE;

	//AGCDUI_USERDATA_TYPE_GRID인 이유는 User Data가 Grid라 디스플레이 호출을 위해 넣었다.
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SalesBox2ItemName", AGCMUI_ITEM_DISPLAY_ID_ITEM_NAME_IN_SALESBOX2, CBDisplaySalesBox2ItemName, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	//AGCDUI_USERDATA_TYPE_GRID인 이유는 User Data가 Grid라 디스플레이 호출을 위해 넣었다.
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SalesBox2ItemPrice", AGCMUI_ITEM_DISPLAY_ID_ITEM_PRICE_IN_SALESBOX2, CBDisplaySalesBox2ItemPrice, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	//AGCDUI_USERDATA_TYPE_GRID인 이유는 User Data가 Grid라 디스플레이 호출을 위해 넣었다.
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SalesBox2ItemTime", AGCMUI_ITEM_DISPLAY_ID_ITEM_TIME_IN_SALESBOX2, CBDisplaySalesBox2ItemTime, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SalesBox2SalesOn", AGCMUI_ITEM_DISPLAY_ID_SALES_ON_SALESBOX2, CBDisplaySalesBox2SalesOn, AGCDUI_USERDATA_TYPE_STRING ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SalesBox2SalesComplete", AGCMUI_ITEM_DISPLAY_ID_SALES_COMPLETE_SALESBOX2, CBDisplaySalesBox2SalesComplete, AGCDUI_USERDATA_TYPE_STRING ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "SalesBox2SalesOkCancel", AGCMUI_ITEM_DISPLAY_ID_SALES_OK_CANCEL, CBDisplaySalesBox2OkCancel, AGCDUI_USERDATA_TYPE_STRING ) )
		return FALSE;

	return TRUE;
}

BOOL AgcmUIItem::AddUserData()
{
	//Grid추가
	if (!(m_pcsInventory = m_pcsAgcmUIManager2->AddUserData("InventoryGrid", m_pcsInventoryGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquip = m_pcsAgcmUIManager2->AddUserData("EquipGrid", m_pcsEquipGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsInventoryQuest = m_pcsAgcmUIManager2->AddUserData("InventoryQuestGrid", m_pcsInventoryQuestGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	/*
	if (!(m_pcsBank = m_pcsAgcmUIManager2->AddUserData("BankGrid", m_pcsBankGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	*/
	if (!(m_pcsSalesBox = m_pcsAgcmUIManager2->AddUserData("SalesBoxGrid", m_pcsSalesBoxGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

	//SalesBox2
	if (!(m_pcsSalesBox2Grid = m_pcsAgcmUIManager2->AddUserData("SalesBox2Grid", m_csSalesBox2Grid, sizeof(AgpdGrid), AGPMAUCTION_MAX_REGISTER, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsSalesBox2SalesOn = m_pcsAgcmUIManager2->AddUserData("SalesBox2SalesOn", m_strSalesBox2SalesOn, sizeof(m_strSalesBox2SalesOn), 1, AGCDUI_USERDATA_TYPE_STRING)))
		return FALSE;
	if (!(m_pcsSalesBox2SalesComplete = m_pcsAgcmUIManager2->AddUserData("SalesBox2SalesComplete", m_strSalesBox2SalesComplete, sizeof(m_strSalesBox2SalesComplete), 1, AGCDUI_USERDATA_TYPE_STRING)))
		return FALSE;
	if (!(m_pcsSalesBox2ButtonEnable = m_pcsAgcmUIManager2->AddUserData("SalesBox2SalesButtonEnable", &m_lSalesBox2SalesButtonEnable, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;

	if (!(m_pcsNPCTrade = m_pcsAgcmUIManager2->AddUserData("NPCTradeGrid", m_pcsNPCTradeGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsPrivateTrade = m_pcsAgcmUIManager2->AddUserData("PrivateTradeGrid", m_pcsPrivateTradeGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

	if (!(m_pcsPrivateTradeEx = m_pcsAgcmUIManager2->AddUserData("PrivateTradeGridEx" , &m_PrivateTradeGridEx , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsPrivateTradeClientEx = m_pcsAgcmUIManager2->AddUserData("PrivateTradeClientGridEx" , &m_PrivateTradeClientGridEx ,  sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

	if (!(m_pcsPrivateTradeClient= m_pcsAgcmUIManager2->AddUserData("PrivateTradeClientGrid", m_pcsPrivateTradeClientGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsInventoryMoney = m_pcsAgcmUIManager2->AddUserData("InventoryMoney", &m_llInventoryMoney, sizeof(INT64), 1, AGCDUI_USERDATA_TYPE_UINT64)))
		return FALSE;
	if (!(m_pcsPrivateTraderName = m_pcsAgcmUIManager2->AddUserData("PrivateTraderName", m_pcsPrivateTrader, sizeof(AgpdCharacter *), 1, AGCDUI_USERDATA_TYPE_CHARACTER)))
		return FALSE;
	if (!(m_pcsRequestPrivateTrade = m_pcsAgcmUIManager2->AddUserData("PT_Request", m_pcsPTRequest, sizeof(AgpdCharacter *), 1, AGCDUI_USERDATA_TYPE_CHARACTER)))
		return FALSE;

	if (!(m_pcsPrivateTradeSetName = m_pcsAgcmUIManager2->AddUserData( "PrivateTradeSetName", m_pcsPrivateTradeSetNameChar, sizeof(AgpdCharacter *), 1, AGCDUI_USERDATA_TYPE_CHARACTER)))
		return FALSE;
	if (!(m_pcsPrivateTradeSetClientName = m_pcsAgcmUIManager2->AddUserData( "PrivateTradeSetClientName", m_pcsPrivateTradeSetClientNameChar, sizeof(AgpdCharacter *), 1, AGCDUI_USERDATA_TYPE_CHARACTER)))
		return FALSE;

	if ( !(m_pcsPrivateTradeGhelld = m_pcsAgcmUIManager2->AddUserData( "PrivateTradeGhelld", &m_lPrivateTradeGhelld, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;
	if ( !(m_pcsPrivateTradeClientGhelld = m_pcsAgcmUIManager2->AddUserData( "PrivateTradeClientGhelld", &m_lPrivateTradeClientGhelld, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;

	if (!(m_pcsInventoryLayerIndex = m_pcsAgcmUIManager2->AddUserData("InventoryLayerIndex", &m_lInventoryLayerIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;
	if (!(m_pcsSubInventoryLayerIndex = m_pcsAgcmUIManager2->AddUserData("SubInventoryLayerIndex", &m_lSubInventoryLayerIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;
	/*
	if (!(m_pcsBankLayerIndex = m_pcsAgcmUIManager2->AddUserData("BankLayerIndex", &m_lBankLayerIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;
	*/

	if (!(m_pcsMessageData = m_pcsAgcmUIManager2->AddUserData("아이템 메시지 데이타들", &m_stMessageData, sizeof(AgcmUIItemMessageData), 1, AGCDUI_USERDATA_TYPE_STRING)))
		return FALSE;

	if (!(m_pcsEquipSlotBody = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridBody", &m_csEquipSlotGrid[AGPMITEM_PART_BODY], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotHead = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridHead", &m_csEquipSlotGrid[AGPMITEM_PART_HEAD], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotHands = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridHands", &m_csEquipSlotGrid[AGPMITEM_PART_HANDS], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotLegs = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridLegs", &m_csEquipSlotGrid[AGPMITEM_PART_LEGS], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotFoot = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridFoot", &m_csEquipSlotGrid[AGPMITEM_PART_FOOT], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotHandLeft = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridHandLeft", &m_csEquipSlotGrid[AGPMITEM_PART_HAND_LEFT], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotHandRight = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridHandRight", &m_csEquipSlotGrid[AGPMITEM_PART_HAND_RIGHT], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotRing1 = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridRing1", &m_csEquipSlotGrid[AGPMITEM_PART_ACCESSORY_RING1], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotRing2 = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridRing2", &m_csEquipSlotGrid[AGPMITEM_PART_ACCESSORY_RING2], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotNecklace = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridNecklace", &m_csEquipSlotGrid[AGPMITEM_PART_ACCESSORY_NECKLACE], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotRide = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridRide", &m_csEquipSlotGrid[AGPMITEM_PART_RIDE], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;
	if (!(m_pcsEquipSlotLancer = m_pcsAgcmUIManager2->AddUserData("EquipSlotGridLancer", &m_csEquipSlotGrid[AGPMITEM_PART_LANCER], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

	if (!(m_pcsActiveReverseOrb = m_pcsAgcmUIManager2->AddUserData("IsActiveReverseOrb", &m_bIsActiveReverseOrb, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_BOOL)))
		return FALSE;

	if (!(m_pcsSubInventory = m_pcsAgcmUIManager2->AddUserData("SubInventoryGrid", m_pcsSubInventoryGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID)))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIItem::AddBoolean()
{
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActivePrivateTradeMemu", CBIsActivePrivateTradeMemu, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsEnableSalesBox2Button", CBIsEnableSalesBox2Button, AGCDUI_USERDATA_TYPE_INT32 ))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIItem::SetSelfCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem			*pcsThis			= (AgcmUIItem *)(pClass);
	AgpdCharacter		*pcsAgpdCharacter	= (AgpdCharacter *)(pData);
	AgpdItemADChar		*pcsAgpdItemADChar;

	if((!pcsThis) || (!pcsAgpdCharacter))
		return FALSE;

	pcsAgpdItemADChar = pcsThis->m_pcsAgpmItem->GetADCharacter( pcsAgpdCharacter );

	if( pcsAgpdItemADChar != NULL )
	{
		pcsThis->m_pcsInventoryGrid = &pcsAgpdItemADChar->m_csInventoryGrid;
		pcsThis->m_pcsInventoryQuestGrid = &pcsAgpdItemADChar->m_csQuestGrid;
		pcsThis->m_pcsAgpmCharacter->GetMoney(pcsAgpdCharacter, &pcsThis->m_llInventoryMoney);
		pcsThis->m_pcsEquipGrid = &pcsAgpdItemADChar->m_csEquipGrid;
		//pcsThis->m_pcsBankGrid = &pcsAgpdItemADChar->m_csBankGrid;

		pcsThis->m_pcsSalesBoxGrid = &pcsAgpdItemADChar->m_csSalesBoxGrid;
		//pcsThis->m_pcsNPCTradeGrid = ?? <-요놈은 어떻게 방법하지? 우울타~
		pcsThis->m_pcsPrivateTradeGrid = &pcsAgpdItemADChar->m_csTradeGrid;
		pcsThis->m_pcsPrivateTradeClientGrid = &pcsAgpdItemADChar->m_csClientTradeGrid;

		pcsThis->m_pcsSubInventoryGrid = &pcsAgpdItemADChar->m_csSubInventoryGrid;

		pcsThis->m_pcsInventory->m_stUserData.m_pvData				= pcsThis->m_pcsInventoryGrid;
		pcsThis->m_pcsInventoryQuest->m_stUserData.m_pvData			= pcsThis->m_pcsInventoryQuestGrid;
		pcsThis->m_pcsEquip->m_stUserData.m_pvData					= pcsThis->m_pcsEquipGrid;
		//pcsThis->m_pcsBank->m_stUserData.m_pvData					= pcsThis->m_pcsBankGrid;
		pcsThis->m_pcsSalesBox->m_stUserData.m_pvData				= pcsThis->m_pcsSalesBoxGrid;
		pcsThis->m_pcsPrivateTrade->m_stUserData.m_pvData			= pcsThis->m_pcsPrivateTradeGrid;
		pcsThis->m_pcsPrivateTradeEx->m_stUserData.m_pvData			= &pcsThis->m_PrivateTradeGridEx;
		pcsThis->m_pcsPrivateTradeClientEx->m_stUserData.m_pvData	= &pcsThis->m_PrivateTradeClientGridEx;
		pcsThis->m_pcsPrivateTradeClient->m_stUserData.m_pvData		= pcsThis->m_pcsPrivateTradeClientGrid;
		pcsThis->m_pcsSubInventory->m_stUserData.m_pvData			= pcsThis->m_pcsSubInventoryGrid;

		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventory);
		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventoryQuest);
		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventoryMoney);
		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsEquip);
		//pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsBank);
		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsSalesBox);
		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsPrivateTrade);
		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsPrivateTradeClient);
		pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsSubInventory);

		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_lEventEquipStatusBodyOff);
		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_lEventEquipStatusHeadOff);
		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_lEventEquipStatusHandOff);
		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_lEventEquipStatusLegsOff);
		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_lEventEquipStatusFootOff);
		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_lEventEquipStatusShieldOff);
		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_lEventEquipStatusWeaponOff);

		pcsThis->m_pcsAgcmUIManager2->ThrowEvent(pcsThis->m_lEventEquipStatusClose);

		for (int i = AGPMITEM_PART_BODY; i < AGPMITEM_PART_NUM; ++i)
		{
			AgpdItem	*pcsItem	= pcsThis->m_pcsAgpmItem->GetEquipSlotItem(pcsAgpdCharacter, (AgpmItemPart) i);
			if (pcsItem)
				pcsThis->CBEquipItem(pcsItem, pcsThis, NULL);
		}

		pcsThis->UpdateEquipSlotGrid();

		// 리버스, 레저력선 오브 확인한다. 2006.08.21. steeple
		pcsThis->CBUpdateActionStatus(pcsAgpdCharacter, pcsThis, NULL);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmUIItem::CBReturnToLoginEnd(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem			*pcsThis			= (AgcmUIItem *)(pClass);
	int					i;

	if(!pcsThis)
		return FALSE;

	for (i = 0; i < AGPMAUCTION_MAX_REGISTER; ++i)
		pcsThis->m_pcsAgpmGrid->Reset(&pcsThis->m_csSalesBox2Grid[i]);
	for (i = 0; i < AGPMITEM_PART_NUM; ++i)
		pcsThis->m_pcsAgpmGrid->Reset(&pcsThis->m_csEquipSlotGrid[i]);

	return TRUE;
}

BOOL AgcmUIItem::ReleaseSelfCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem			*pcsThis			= (AgcmUIItem *)(pClass);

	if(!pcsThis)
		return FALSE;

	/*
	for (int i = 0; i < AGPMAUCTION_MAX_REGISTER; ++i)
		pcsThis->m_pcsAgpmGrid->Reset(&pcsThis->m_csSalesBox2Grid[i]);
	for (i = 0; i < AGPMITEM_PART_NUM; ++i)
		pcsThis->m_pcsAgpmGrid->Reset(&pcsThis->m_csEquipSlotGrid[i]);
	*/

	pcsThis->m_pcsInventoryGrid				= NULL;
	pcsThis->m_pcsInventoryQuestGrid		= NULL;
	pcsThis->m_llInventoryMoney				= 0;
	pcsThis->m_pcsEquipGrid					= NULL;
	//pcsThis->m_pcsBankGrid					= NULL;

	pcsThis->m_pcsSalesBoxGrid				= NULL;
	pcsThis->m_pcsPrivateTradeGrid			= NULL;
	pcsThis->m_pcsPrivateTradeClientGrid	= NULL;
	pcsThis->m_pcsSubInventoryGrid			= NULL;

	pcsThis->m_pcsInventory->m_stUserData.m_pvData			= NULL;
	pcsThis->m_pcsInventoryQuest->m_stUserData.m_pvData		= NULL;
	pcsThis->m_pcsEquip->m_stUserData.m_pvData				= NULL;
	//pcsThis->m_pcsBank->m_stUserData.m_pvData				= NULL;
	pcsThis->m_pcsSalesBox->m_stUserData.m_pvData			= NULL;
	pcsThis->m_pcsPrivateTrade->m_stUserData.m_pvData		= NULL;
	pcsThis->m_pcsPrivateTradeClient->m_stUserData.m_pvData	= NULL;
	pcsThis->m_pcsSubInventory->m_stUserData.m_pvData		= NULL;

	pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventory);
	pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventoryQuest);
	pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventoryMoney);
	pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsEquip);
	//pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsBank);
	pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsSalesBox);
	pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsPrivateTrade);
	pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsPrivateTradeClient);
	pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsSubInventory);

	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;

	// 드래곤 시온은 들어와도 무시한다
	if( pcsThis->m_pcsAgpmFactors->GetRace( &pAgpdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
		return TRUE;

	if (pAgpdCharacter->m_bIsTrasform)
	{
		pcsThis->m_bIsTransformed = TRUE;
	}

	else
	{
		if (!pcsThis->m_bIsTransformed)
			pcsThis->m_pcsAgpmAuction->RemoveAllSales(pAgpdCharacter);

		pcsThis->m_bIsTransformed = FALSE;
	}

	return TRUE;
}

/*
BOOL AgcmUIItem::CBRefreshUpdateCharStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem			*pThis				= (AgcmUIItem *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	for (int i = AGPMITEM_PART_BODY; i < AGPMITEM_PART_NUM; ++i)
	{
		AgpdGridItem	*pcsGridItem		= pThis->m_pcsAgpmItem->GetEquipItem(pcsCharacter, i);
		if (!pcsGridItem)
			continue;

		AgpdItem		*pcsItem			= pThis->m_pcsAgpmItem->GetItem(pcsGridItem->m_lItemID);
		if (pcsItem)
		{
			// 마법 속성이 있는 아이템이라면 캐릭터 스탯창에 변화를 준다.
			if (pThis->m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem) > 0)
			{
				AgcmUIUpdateCharStatus	stUpdateCharStatus;
				ZeroMemory(&stUpdateCharStatus, sizeof(AgcmUIUpdateCharStatus));

				pThis->SetUpdateCharStatus(pcsItem, &stUpdateCharStatus);

				pThis->m_pcsAgcmUICharacter->SetUpdateCharStatus(stUpdateCharStatus);
			}
		}
	}

	return TRUE;
}
*/

BOOL AgcmUIItem::CBInventoryQuestAdd(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pcsThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pcsThis != NULL )
	{
		if( pcsThis->m_pcsInventoryQuest != NULL )
		{
			pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventoryQuest);
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBInventoryQuestUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pcsThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pcsThis != NULL )
	{
		if( pcsThis->m_pcsInventoryQuest != NULL )
		{
			pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventoryQuest);
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUseItemResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpmItemUseResult	eResult	= (AgpmItemUseResult) *(INT32 *) pCustData;

	switch (eResult)
	{
		case AGPMITEM_USE_RESULT_FAILED_BY_SIEGEWAR:
			{
				pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_USE_ITEM_RESULT_FAILED_BY_SIEGEWAR));
			}
			break;
			
		case AGPMITEM_USE_RESULT_CANNOT_TRANSFORM_CREATURE:
			{
				pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_USE_ITEM_RESULT_CANNOT_TRANSFORM_CREATURE));
			}
			break;

		case AGPMITEM_USE_RESULT_FAILED_REVERSEORB:
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventReverseOrbError );

				AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )pThis->GetModule( "AgcmUICharacter" );
				if( pcmUICharacter )
				{
					pcmUICharacter->OnShowUIResurrection();
				}
			}
			break;
	}

	return TRUE;
}

BOOL AgcmUIItem::CBInventoryQuestRemove(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pcsThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pcsThis != NULL )
	{
		if( pcsThis->m_pcsInventoryQuest != NULL )
		{
			pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventoryQuest);
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIUpdateInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pcsThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pcsThis != NULL )
	{
		if( pcsThis->m_pcsInventory != NULL )
		{
			pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventory);

			if (pcsThis->m_pcsAgcmUIMain)
			{
				pcsThis->m_pcsAgcmUIMain->RefreshQuickBeltGrid();
			}

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIUpdateSubInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pcsThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pcsThis != NULL )
	{
		if( pcsThis->m_pcsSubInventory != NULL )
		{
			pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsSubInventory);

			if (pcsThis->m_pcsAgcmUIMain)
			{
				pcsThis->m_pcsAgcmUIMain->RefreshQuickBeltGrid();
			}

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIUpdateInventoryQuest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pcsThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pcsThis != NULL )
	{
		if( pcsThis->m_pcsInventoryQuest != NULL )
		{
			pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsInventoryQuest);

			if (pcsThis->m_pcsAgcmUIMain)
			{
				pcsThis->m_pcsAgcmUIMain->RefreshQuickBeltGrid();
			}

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIUpdateEquip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pcsThis = (AgcmUIItem *) pClass;

	BOOL			bResult	= FALSE;

	if( pcsThis != NULL )
	{
		if( pcsThis->m_pcsEquip != NULL )
		{
			pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsEquip);

			pcsThis->UpdateEquipSlotGrid();

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIUpdateTrade(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pcsThis = (AgcmUIItem *) pClass;

	BOOL			bResult;
	INT32			lSelfCID;
	INT32			lCID;

	bResult = FALSE;

	if( pcsThis != NULL )
	{
		if( (pcsThis->m_pcsPrivateTrade != NULL) && (pcsThis->m_pcsAgcmCharacter != NULL) && (pData != NULL) )
		{
			lCID = *((INT32 *)pData);
			lSelfCID = pcsThis->m_pcsAgcmCharacter->m_lSelfCID;

			if( (lSelfCID == lCID) && (lSelfCID !=0) )
			{
				bResult = pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsPrivateTrade);
			}
			else
			{
				bResult = pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsPrivateTradeClient);
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIUpdateTradeClient(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pcsThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pcsThis != NULL )
	{
		if( pcsThis->m_pcsPrivateTradeClient != NULL )
		{
			pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsPrivateTradeClient);
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIUpdateNPCTradeGrid( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem	*pcsThis = (AgcmUIItem *) pClass;
	ApdEvent	*pcsNPCTradeEvent = (ApdEvent *)pData;

	BOOL			bResult;

	bResult = FALSE;

	if( !pcsNPCTradeEvent || !pcsNPCTradeEvent->m_pvData)
	{
		pcsThis->m_pcsNPCTradeGrid	= NULL;
		pcsThis->m_pcsNPCTrade->m_stUserData.m_pvData	= NULL;
		pcsThis->m_lOpenNPCTradeLayer	= 0;
	}
	else
	{
		if( pcsThis->m_pcsAgcmEventNPCTrade != NULL )
		{
			pcsThis->m_pcsNPCTradeGrid = ((AgpdEventNPCTradeData *)pcsNPCTradeEvent->m_pvData)->m_pcsGrid;

			((AgpdEventNPCTradeData *)pcsNPCTradeEvent->m_pvData)->m_pcsGrid->m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;

			pcsThis->m_pcsNPCTrade->m_stUserData.m_pvData	= pcsThis->m_pcsNPCTradeGrid;
			pcsThis->m_lOpenNPCTradeLayer	= 0;

			bResult = TRUE;
		}
	}

	pcsThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pcsThis->m_pcsNPCTrade);

	return bResult;
}

BOOL AgcmUIItem::CBRemoveItemFromInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;
	AgpdItemStatus	eNewStatus		= (AgpdItemStatus)	(INT32) pCustData;

	if (pThis->m_pcsAgcmUIMain &&
		eNewStatus != AGPDITEM_STATUS_INVENTORY)
	{
		pThis->m_pcsAgcmUIMain->RemoveGridItemFromQuickBelt(pcsItem->m_pcsGridItem);
	}

	return TRUE;
}

BOOL AgcmUIItem::CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;

	if (pThis->m_pcsAgcmUIMain)
	{
		pThis->m_pcsAgcmUIMain->RemoveGridItemFromQuickBelt(pcsItem->m_pcsGridItem);
	}

	return TRUE;
}

BOOL AgcmUIItem::CBOpenQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIItem			*pThis			= (AgcmUIItem *)	pClass;

	return pThis->CBOpenGridItemToolTip(pClass, NULL, NULL, NULL, NULL, NULL, NULL, (AgcdUIControl *) pData);
}

BOOL AgcmUIItem::CBCloseQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem			*pThis			= (AgcmUIItem *)	pClass;

	return pThis->CBCloseGridItemToolTip(pClass, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

BOOL AgcmUIItem::CBCloseAllUIToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem			*pThis			= (AgcmUIItem *)	pClass;

	pThis->m_csItemToolTip.ShowWindow( FALSE )		;
	pThis->m_csItemToolTip.DeleteAllStringInfo()	;	

	return TRUE;
}

BOOL AgcmUIItem::CBUpdateItemFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem			*pThis			= (AgcmUIItem *)	pClass;
	AgpdItem			*pcsItem		= (AgpdItem *)		pData;

	return pThis->CheckItemDurability(pcsItem);
}

BOOL	AgcmUIItem::CheckItemLevel( AgpdCharacter * pcsCharacter , AgpdItem *pcsItem )
{
	INT32 lCharLevel	= m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	INT32 lLimitedLevel	= pcsItem->m_pcsItemTemplate->m_lLimitedLevel;
	INT32 lMinLevel		= m_pcsAgpmFactors->GetLevel(&pcsItem->m_csRestrictFactor);

	if( lCharLevel < lMinLevel ) return FALSE;
	else if( lLimitedLevel && lLimitedLevel < lCharLevel ) return FALSE;
	else return TRUE;	// 정상..
}

BOOL	AgcmUIItem::IsItemDurable( AgpdCharacter * pcsCharacter , UINT32 ulPercent, INT32 lOnEvent, INT32 lOffEvent )
{
	if( !pcsCharacter )		return FALSE;

	AgpdItem* pcsEquipItem = m_pcsAgcmItem->GetCurrentEquipWeapon( pcsCharacter );
	if( !pcsEquipItem )		return FALSE;

	INT32 nDurablePercent = m_pcsAgpmItem->GetItemDurabilityPercent( pcsEquipItem );
	//ASSERT( nDurablePercent >= 0 );
	pcsEquipItem->m_pcsGridItem->SetDurabilityZero( nDurablePercent == 0 ? TRUE : FALSE );

	if( pcsEquipItem->m_eStatus == AGPDITEM_STATUS_EQUIP )
		pcsEquipItem->m_pcsGridItem->SetLevelLimited( !CheckItemLevel( pcsCharacter , pcsEquipItem ) );
	else
		pcsEquipItem->m_pcsGridItem->SetLevelLimited( FALSE );

	if( nDurablePercent <= 20 )
	{
		m_pcsAgcmUIManager2->ThrowEvent( lOnEvent );
		return TRUE;
	}
	else
	{
		if( pcsEquipItem->m_pcsGridItem->IsLevelLimited() )
		{
			m_pcsAgcmUIManager2->ThrowEvent( lOnEvent );
			return TRUE;
		}
		else
		{
			m_pcsAgcmUIManager2->ThrowEvent( lOffEvent );
			return FALSE;
		}
	}
}

BOOL	AgcmUIItem::IsItemPartDurable( AgpdCharacter * pcsCharacter , UINT32 ulPercent, INT32 lOnEvent, INT32 lOffEvent, AgpmItemPart ePart )
{
	if( !pcsCharacter )			return FALSE;

	AgpdItem* pcsEquipItem = m_pcsAgpmItem->GetEquipSlotItem( pcsCharacter, ePart );
	if( !pcsEquipItem )		return FALSE;

	INT32 nDurablePercent = m_pcsAgpmItem->GetItemDurabilityPercent( pcsEquipItem );
	//ASSERT( nDurablePercent >= 0 );
	pcsEquipItem->m_pcsGridItem->SetDurabilityZero( nDurablePercent == 0 ? TRUE : FALSE );

	if( pcsEquipItem->m_eStatus == AGPDITEM_STATUS_EQUIP )
		pcsEquipItem->m_pcsGridItem->SetLevelLimited( !CheckItemLevel( pcsCharacter , pcsEquipItem ) );
	else
		pcsEquipItem->m_pcsGridItem->SetLevelLimited( FALSE );

	if( nDurablePercent <= 20 )
	{
		m_pcsAgcmUIManager2->ThrowEvent( lOnEvent );
		return TRUE;
	}
	else
	{
		if( pcsEquipItem->m_pcsGridItem->IsLevelLimited() )
		{
			m_pcsAgcmUIManager2->ThrowEvent( lOnEvent );
			return TRUE;
		}
		else
		{
			m_pcsAgcmUIManager2->ThrowEvent( lOffEvent );
			return FALSE;
		}
	}
}

BOOL AgcmUIItem::CheckItemDurability(AgpdItem *pcsItem)
{
	if (!pcsItem)		return FALSE;

	if (!pcsItem->m_pcsCharacter)
		pcsItem->m_pcsCharacter = m_pcsAgpmCharacter->GetCharacter(pcsItem->m_ulCID);

	if (!pcsItem->m_pcsCharacter)
		return FALSE;

	if (!(pcsItem->m_pcsItemTemplate &&
		  ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP &&
		  (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR ||
		   ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON ||
		   ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD)))
		return TRUE;

	if( pcsItem->m_eStatus != AGPDITEM_STATUS_EQUIP && pcsItem->m_pcsGridItem )
		pcsItem->m_pcsGridItem->SetLevelLimited( FALSE );

	return CheckItemDurability( pcsItem->m_pcsCharacter );
}

BOOL AgcmUIItem::CheckItemDurability( AgpdCharacter * pcsCharacter)
{
	if (pcsCharacter != m_pcsAgcmCharacter->GetSelfCharacter())
		return TRUE;

	// Equip Item (Weapon & Armour & Shield) 인 경우 Durability가 20% 이하로 떨어졌는지 검사한다.
	////////////////////////////////////////////////////////////////////////////////////////////

	//상태를 일단 clear 해준다
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventEquipStatusWeaponOff );
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventEquipStatusBodyOff );
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventEquipStatusHeadOff );
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventEquipStatusHandOff );
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventEquipStatusLegsOff );
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventEquipStatusFootOff );
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventEquipStatusShieldOff );

	INT32 nOpen = 0;
	nOpen += IsItemDurable( pcsCharacter, 20, m_lEventEquipStatusWeaponOn, m_lEventEquipStatusWeaponOff );
	nOpen += IsItemPartDurable( pcsCharacter, 20, m_lEventEquipStatusBodyOn, m_lEventEquipStatusBodyOff, AGPMITEM_PART_BODY );
	nOpen += IsItemPartDurable( pcsCharacter, 20, m_lEventEquipStatusHeadOn, m_lEventEquipStatusHeadOff, AGPMITEM_PART_HEAD );
	nOpen += IsItemPartDurable( pcsCharacter, 20, m_lEventEquipStatusHandOn, m_lEventEquipStatusHandOff, AGPMITEM_PART_HANDS );
	nOpen += IsItemPartDurable( pcsCharacter, 20, m_lEventEquipStatusLegsOn, m_lEventEquipStatusLegsOff, AGPMITEM_PART_LEGS );
	nOpen += IsItemPartDurable( pcsCharacter, 20, m_lEventEquipStatusFootOn, m_lEventEquipStatusFootOff, AGPMITEM_PART_FOOT );
	nOpen += IsItemPartDurable( pcsCharacter, 20, m_lEventEquipStatusShieldOn, m_lEventEquipStatusShieldOff, AGPMITEM_PART_HAND_LEFT );	//방패나 왼손무기(양손잡이)

	m_pcsAgcmUIManager2->ThrowEvent( nOpen > 0 ? m_lEventEquipStatusOpen : m_lEventEquipStatusClose );

	return TRUE;
}

AgcdUIItem* AgcmUIItem::GetADItem(AgpdItem *pcsItem)
{
	if (m_pcsAgpmItem)
		return (AgcdUIItem *) m_pcsAgpmItem->GetAttachedModuleData(m_nIndexADItem, (PVOID) pcsItem);

	return NULL;
}

AgcdUIItemTooltipDesc* AgcmUIItem::GetADItemTemplate(AgpdItemTemplate *pcsItemTemplate)
{
	if (m_pcsAgpmItem)
		return (AgcdUIItemTooltipDesc *) m_pcsAgpmItem->GetAttachedModuleData(m_nIndexADItemTemplate, (PVOID) pcsItemTemplate);

	return NULL;
}

BOOL AgcmUIItem::AddItemDurabilityZeroList(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	AgcdUIItem		*pcsAgcdUIItem	= GetADItem(pcsItem);
	if (!pcsAgcdUIItem)
		return FALSE;

	if (pcsAgcdUIItem->m_bNotifyDurabilityZero)
		return TRUE;

	// 먼저 다른 리스트에 있는지 본다. 있음 지운다.
	RemoveItemDurabilityUnder5PercentList(pcsItem);

	// 이미 들어있는 넘인지 함 검사해본다.
	for (int i = 0; i < m_lNumListNotifyDurabilityZero; ++i)
	{
		if (m_alListNotifyDurabilityZero[i] == pcsItem->m_lID)
		{
			// 시간을 업데이트하고 걍 리턴한다.
			m_aulRegisterTimeNotifyDurabilityZero[i] = GetPrevClockCount();

			return TRUE;
		}
	}

	if (m_lNumListNotifyDurabilityZero == AGPMITEM_PART_NUM)
		return FALSE;

	m_aulRegisterTimeNotifyDurabilityZero[m_lNumListNotifyDurabilityZero] = GetPrevClockCount();
	m_alListNotifyDurabilityZero[m_lNumListNotifyDurabilityZero++] = pcsItem->m_lID;

	pcsAgcdUIItem->m_bNotifyDurabilityZero	= TRUE;

	m_pcsAgcmUIMain->UpdateAlarmGrid();

	return TRUE;
}

BOOL AgcmUIItem::RemoveItemDurabilityZeroList(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	INT32			i;
	AgcdUIItem		*pcsAgcdUIItem	= GetADItem(pcsItem);
	if (!pcsAgcdUIItem)
		return FALSE;

	for (i = 0; i < m_lNumListNotifyDurabilityZero; ++i)
	{
		if (m_alListNotifyDurabilityZero[i] == pcsItem->m_lID)
			break;
	}

	if (i == m_lNumListNotifyDurabilityZero)
		return FALSE;

	CopyMemory(m_alListNotifyDurabilityZero + i,
			   m_alListNotifyDurabilityZero + i + 1,
			   sizeof(INT32) * (m_lNumListNotifyDurabilityZero - i - 1));

	CopyMemory(m_aulRegisterTimeNotifyDurabilityZero + i,
			   m_aulRegisterTimeNotifyDurabilityZero + i + 1,
			   sizeof(UINT32) * (m_lNumListNotifyDurabilityZero - i - 1));

	--m_lNumListNotifyDurabilityZero;

	pcsAgcdUIItem->m_bNotifyDurabilityZero	= FALSE;

	m_pcsAgcmUIMain->UpdateAlarmGrid();

	return TRUE;
}

BOOL AgcmUIItem::AddItemDurabilityUnder5PercentList(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	AgcdUIItem		*pcsAgcdUIItem	= GetADItem(pcsItem);
	if (!pcsAgcdUIItem)
		return FALSE;

	if (pcsAgcdUIItem->m_bNotifyDurabilityUnder5Percent)
		return TRUE;

	RemoveItemDurabilityZeroList(pcsItem);

	// 이미 들어있는 넘인지 함 검사해본다.
	for (int i = 0; i < m_lNumListNotifyDurabilityUnder5Percent; ++i)
	{
		if (m_alListNotifyDurabilityUnder5Percent[i] == pcsItem->m_lID)
		{
			// 시간을 업데이트하고 리턴한다.
			m_aulRegisterTimeNotifyDurabilityUnder5Percent[i] = GetPrevClockCount();

			return TRUE;
		}
	}

	if (m_lNumListNotifyDurabilityUnder5Percent == AGPMITEM_PART_NUM)
		return FALSE;

	m_aulRegisterTimeNotifyDurabilityUnder5Percent[m_lNumListNotifyDurabilityUnder5Percent]	= GetPrevClockCount();
	m_alListNotifyDurabilityUnder5Percent[m_lNumListNotifyDurabilityUnder5Percent++] = pcsItem->m_lID;

	pcsAgcdUIItem->m_bNotifyDurabilityUnder5Percent	= TRUE;

	m_pcsAgcmUIMain->UpdateAlarmGrid();

	return TRUE;
}

BOOL AgcmUIItem::RemoveItemDurabilityUnder5PercentList(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	INT32			i;
	AgcdUIItem		*pcsAgcdUIItem	= GetADItem(pcsItem);
	if (!pcsAgcdUIItem)
		return FALSE;

	for (i = 0; i < m_lNumListNotifyDurabilityUnder5Percent; ++i)
	{
		if (m_alListNotifyDurabilityUnder5Percent[i] == pcsItem->m_lID)
			break;
	}

	if (i == m_lNumListNotifyDurabilityUnder5Percent)
		return FALSE;

	CopyMemory(m_alListNotifyDurabilityUnder5Percent + i,
			   m_alListNotifyDurabilityUnder5Percent + i + 1,
			   sizeof(INT32) * (m_lNumListNotifyDurabilityUnder5Percent - i - 1));

	CopyMemory(m_aulRegisterTimeNotifyDurabilityUnder5Percent + i,
			   m_aulRegisterTimeNotifyDurabilityUnder5Percent + i + 1,
			   sizeof(UINT32) * (m_lNumListNotifyDurabilityUnder5Percent - i - 1));

	--m_lNumListNotifyDurabilityUnder5Percent;

	pcsAgcdUIItem->m_bNotifyDurabilityUnder5Percent	= FALSE;

	m_pcsAgcmUIMain->UpdateAlarmGrid();

	return TRUE;
}

BOOL AgcmUIItem::CBUpdateAlarmGrid(PVOID pData, PVOID pClass, PVOID pCustData)
{
//	if (!pClass)
//		return FALSE;
//
//	AgcmUIItem			*pThis				= (AgcmUIItem *)		pClass;
//
//	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
//	if (!pcsSelfCharacter)
//		return FALSE;
//
//	for (int i = 0; i < pThis->m_lNumListNotifyDurabilityZero; ++i)
//	{
//		AgpdItemTemplate	*pcsItemTemplate	= pThis->m_pcsAgpmItem->GetItemTemplate(pThis->m_alListNotifyDurabilityZero[i]);
//		if (pcsItemTemplate)
//		{
//			pThis->m_pcsAgcmUIMain->AddItemToAlarmGrid(pcsItemTemplate->m_pcsGridItemDurabilityZero);
//		}
//	}
//
//	for (i = 0; i < pThis->m_lNumListNotifyDurabilityUnder5Percent; ++i)
//	{
//		AgpdItemTemplate	*pcsItemTemplate	= pThis->m_pcsAgpmItem->GetItemTemplate(pThis->m_alListNotifyDurabilityUnder5Percent[i]);
//		if (pcsItemTemplate)
//		{
//			pThis->m_pcsAgcmUIMain->AddItemToAlarmGrid(pcsItemTemplate->m_pcsGridItemDurabilityUnder5);
//		}
//	}

	return TRUE;
}

BOOL AgcmUIItem::CBChangeOwner(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *	pThis = (AgcmUIItem *) pClass;
	AgpdItem *		pcsItem = (AgpdItem *) pData;

	if (pcsItem->m_ulCID == pThis->m_pcsAgcmCharacter->m_lSelfCID)
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGetItem);

	return TRUE;
}

BOOL AgcmUIItem::CBPickupItemResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIItem			*pThis			= (AgcmUIItem *)	pClass;
	INT8				cResult			= *(INT8 *)			pData;
	PVOID				*ppvBuffer		= (PVOID *)			pCustData;

	AgpdItemTemplate	*pcsItemTemplate	= (AgpdItemTemplate *)	ppvBuffer[0];
	INT32				lItemCount			= (INT32)				ppvBuffer[1];
	INT32				lItemID				= (INT32)				ppvBuffer[2];

	if (!pcsItemTemplate)
		return FALSE;

	pThis->m_stMessageData.lPickupItemTID	= pcsItemTemplate->m_lID;
	pThis->m_stMessageData.lPickupItemCount	= lItemCount;
	pThis->m_stMessageData.lPickupItemID	= lItemID;
	pThis->m_stMessageData.lPickupItemBonusCount	= 0;

	switch (cResult) {
	case AGPMITEM_PACKET_PICKUP_ITEM_RESULT_SUCCESS:
		{
			if (pcsItemTemplate->m_lID == pThis->m_pcsAgpmItem->GetMoneyTID())
			{
				//if (pThis->m_pcsAgpmBillInfo->IsPCBang(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
				//{
				//	pThis->m_stMessageData.lPickupItemCount	= (INT32) (lItemCount * 1.5f);
				//	pThis->m_stMessageData.lPickupItemBonusCount	= pThis->m_stMessageData.lPickupItemCount - lItemCount;
				//}

				//if (pThis->m_stMessageData.lPickupItemBonusCount > 0)
				//	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPickupItemPCBangMoneySuccess);
				//else
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPickupItemMoneySuccess);
			}
			else
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPickupItemSuccess);
			}
		}
		break;

	case AGPMITEM_PACKET_PICKUP_ITEM_RESULT_FAIL:
		{
			if (pcsItemTemplate->m_lID == pThis->m_pcsAgpmItem->GetMoneyTID())
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPickupItemMoneyFail);
			}
			else
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPickupItemFail);
			}
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL AgcmUIItem::CBAutoPickupItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem			*pThis			= (AgcmUIItem *)	pClass;

	return pThis->m_pcsAgcmItem->AutoPickupItem();
}

BOOL AgcmUIItem::CBSetupItem(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *		pThis			= (AgcmUIItem *)	pClass;
	AgpdItem *			pcsItem			= (AgpdItem *)		pData;

	if (pcsItem->m_pcsItemTemplate)
	{
		if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
			((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL &&
			((AgpdItemTemplateUsableTeleportScroll *) pcsItem->m_pcsItemTemplate)->m_eTeleportScrollType == AGPMITEM_USABLE_TELEPORT_SCROLL_RETURN_TOWN)
			pThis->m_pcsReturnScrollTemplate	= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;
	}

	switch (pcsItem->m_eStatus)
	{
	case AGPDITEM_STATUS_TRADE_GRID:
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTrade);
		break;

	case AGPDITEM_STATUS_CLIENT_TRADE_GRID:
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeClient);
		break;

	case AGPDITEM_STATUS_SALESBOX_GRID:
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsSalesBox);
		break;

		/*
	case AGPDITEM_STATUS_BANK:
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsBank);
		break;
		*/

	case AGPDITEM_STATUS_INVENTORY:
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsInventory);
		break;

	case AGPDITEM_STATUS_QUEST:
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsInventoryQuest);
		break;

	case AGPDITEM_STATUS_EQUIP:
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsEquip);
		break;

	case AGPDITEM_STATUS_NPC_TRADE:
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsNPCTrade);
		break;

	default:
		return TRUE;
		break;
	}

	return TRUE;
}

BOOL AgcmUIItem::CBCheckUseItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;

	AgpdItemTemplate	*pcsTemplate	= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;

	if (pcsTemplate->m_nType == AGPMITEM_TYPE_USABLE)
	{
		if (!pThis->m_pcsAgpmItem->CheckUseValidArea(pcsItem))
		{
			pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_INVALID_USE_AREA));
			return FALSE;
		}

		AgpdCharacter *pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if (NULL != pcsCharacter)
		{
			if ( pcsCharacter->m_bRidable || pcsCharacter->m_bIsTrasform)
			{
				// 탈것을 착용한 상태에서 변신 물약은 사용할 수 없다.
				if ( (AGPMITEM_TYPE_USABLE == pcsItem->m_pcsItemTemplate->m_nType) &&
					(AGPMITEM_USABLE_TYPE_TRANSFORM == ((AgpdItemTemplateUsable *)(pcsItem->m_pcsItemTemplate))->m_nUsableItemType) )
				{
					pThis->m_pcsAgcmChatting2->AddSystemMessage(ClientStr().GetStr(STI_CANT_USE_MORPH));
					return FALSE;
				}

				if ( (AGPMITEM_TYPE_USABLE == pcsItem->m_pcsItemTemplate->m_nType) &&
					(AGPMITEM_USABLE_TYPE_SKILL_SCROLL == ((AgpdItemTemplateUsable *)(pcsItem->m_pcsItemTemplate))->m_nUsableItemType) &&
					!pThis->m_pcsAgpmArchlord->IsArchlord(pcsCharacter->m_szID))
				{
					if (AGPMITEM_USABLE_SCROLL_SUBTYPE_ATTACK_SPEED == ((AgpdItemTemplateUsableSkillScroll *)(pcsItem->m_pcsItemTemplate))->m_eScrollSubType)
					{
						pThis->m_pcsAgcmChatting2->AddSystemMessage(ClientStr().GetStr(STI_CANT_USE_ATTACKP));
						return FALSE;
					}

					if (AGPMITEM_USABLE_SCROLL_SUBTYPE_MOVE_SPEED == ((AgpdItemTemplateUsableSkillScroll *)(pcsItem->m_pcsItemTemplate))->m_eScrollSubType)
					{
						pThis->m_pcsAgcmChatting2->AddSystemMessage(ClientStr().GetStr(STI_CANT_USE_MOVEP));
						return FALSE;
					}
				}
			}
		}

		if (((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
		{
			// 변신중일때는 변신물약을 사용할 수 없다.
			if (pcsItem->m_pcsCharacter && pcsItem->m_pcsCharacter->m_bIsTrasform && pThis->m_pcsAgcmChatting2)
			{
				// UI 상에 뿌려준다.
				pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_ALREADY_USE_TRANSFORM));

				return FALSE;
			}
		}

		// battle square 에서는 마을귀환문서, 왕복문서를 사용 못한다.
		else if (((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL)
		{
			// 캐릭터가 죽은경운 사용할 수 없다.
			if (pcsItem->m_pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			{
				pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_TRYUSETELEPORTSCROLLOnDead));
				return FALSE;
			}

			// 현재 변신중인지 본다.
			if (pcsItem->m_pcsCharacter->m_bIsTrasform && !pThis->m_pcsAgpmArchlord->IsArchlord(pcsCharacter->m_szID))
			{
				pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_TRYUSETELEPORTSCROLLOnTransfrom));
				return FALSE;
			}

			if (((AgpdItemTemplateUsableTeleportScroll *) pcsItem->m_pcsItemTemplate)->m_eTeleportScrollType != AGPMITEM_USABLE_TELEPORT_SCROLL_GO_TOWN_NOW &&
				pThis->m_pcsAgpmCharacter->IsCombatMode(pcsItem->m_pcsCharacter))
			{
				pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_TRYUSETELEPORTSCROLLOnCombat));
				return FALSE;
			}

			// 베틀 스퀘어 안에 있는지 체크해본다. 안에 있으면 사용 못한다.
			if (pThis->m_pcsAgpmCharacter->IsInPvPDanger(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
			{
				// UI 상에 뿌려준다.
				pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_TRYUSETELEPORTSCROLLINPVP));

				return FALSE;
			}

			// 이동문서가 마을내에서도 가능해져서 확인받는다.
			//#########
			//if (((AgpdItemTemplateUsableTeleportScroll *) pcsItem->m_pcsItemTemplate)->m_eTeleportScrollType == AGPMITEM_USABLE_TELEPORT_SCROLL_GO_TOWN &&
			//	IDOK != pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEMTELEPORTSCROLLCONFIRM))
			//	)
			//{
			//	return FALSE;
			//}
		}

		else if (((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_REVERSE_ORB)
		{
			// check owner(character) action status
			if (pcsItem->m_pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
				return FALSE;

			// 베틀 스퀘어 안에 있는지 체크해본다. 안에 있으면 사용 못한다.
			// 위험전투지역이라도 캐쉬템은 사용가능 - arycoat 2009.6.11
			if (!IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType) &&
				pThis->m_pcsAgpmCharacter->IsInPvPDanger(pcsItem->m_pcsCharacter))
				return FALSE;
		}

		// 상자 열쇠 확인
		else if (pcsItem->m_pcsItemTemplate->m_nSubType == AGPMITEM_USABLE_LOTTERY_TYPE_GOLD
			|| pcsItem->m_pcsItemTemplate->m_nSubType == AGPMITEM_USABLE_LOTTERY_TYPE_SILVER
			|| pcsItem->m_pcsItemTemplate->m_nSubType == AGPMITEM_USABLE_LOTTERY_TYPE_BRONZE
			|| pcsItem->m_pcsItemTemplate->m_nSubType == AGPMITEM_USABLE_LOTTERY_TYPE_PLATINUM)
		{
			AgpdGridItem *pcsGridItemLotteryKey
				= pThis->m_pcsAgpmItem->GetCashItemUsableByType(pcsItem->m_pcsCharacter, AGPMITEM_USABLE_TYPE_LOTTERY_KEY, pcsItem->m_pcsItemTemplate->m_nSubType);
			if (pcsGridItemLotteryKey == NULL)
			{
				// UI 상에 뿌려준다.
				if (pcsItem->m_pcsItemTemplate->m_nSubType == AGPMITEM_USABLE_LOTTERY_TYPE_GOLD)
					pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LOTTERYBOX_NEEDS_GOLD_KEY));
				else if (pcsItem->m_pcsItemTemplate->m_nSubType == AGPMITEM_USABLE_LOTTERY_TYPE_SILVER)
					pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LOTTERYBOX_NEEDS_SILVER_KEY));
				else if (pcsItem->m_pcsItemTemplate->m_nSubType == AGPMITEM_USABLE_LOTTERY_TYPE_BRONZE)
					pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LOTTERYBOX_NEEDS_BRONZE_KEY));
				else if (pcsItem->m_pcsItemTemplate->m_nSubType == AGPMITEM_USABLE_LOTTERY_TYPE_PLATINUM)
					pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LOTTERYBOX_NEEDS_PLATINUM_KEY));

				return FALSE;
			}
		}

		else if (((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_ADD_BANK_SLOT)
		{
			// 추가할 뱅크 슬롯이 있는지 본다.
			if (!pThis->m_pcsAgpmItem->IsBuyBankSlot(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
			{
				// UI에 뿌려준다.
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventFullBankSlot);
				return FALSE;
			}
		}
		
		else if (((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_INIT)
		{
			// check owner(character) action status
			if (pcsItem->m_pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
				return FALSE;

			// Dragon Scion이 변신중이라면 실패
			if( pThis->m_pcsAgpmFactors->GetRace( &pcsItem->m_pcsCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
			{
				if( pThis->m_pcsAgpmFactors->GetClass( &pcsItem->m_pcsCharacter->m_csFactor ) != AUCHARCLASS_TYPE_SCION	)
				{
					CHAR*		szUIMessage		=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "IsEvolutionNotUseInitSkillItem" );
					pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( szUIMessage );
					return FALSE;
				}
			}

			// at least 1 or more skill
			if (0 >= pThis->m_pcsAgpmEventSkillMaster->GetOwnSkillCount(pcsCharacter))
			{
				pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_SKILL_INITIALIZE_NO_OWN_SKILL));
				return FALSE;
			}

			// user confirm
			if (IDOK != pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_SKILL_INITIALIZE_CONFIRM)))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmUIItem::CBIsActivePrivateTradeMemu(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIItem			*pThis				= (AgcmUIItem *)		pClass;
	AgpdCharacter		*pcsTargetCharacter	= (AgpdCharacter *)		pData;

	AgpdCharacter		*pcsSelfCharacter	= (AgpdCharacter *)		pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter == pcsTargetCharacter)
		return FALSE;

	// Murderer나 뭐 기타 등등 체크해야 할것이 있으면 여기서 체크한다.
	if (pThis->m_pcsAgpmCharacter->GetMurdererLevel(pcsSelfCharacter) >= AGPMCHAR_MURDERER_LEVEL2_POINT ||
		pThis->m_pcsAgpmCharacter->GetMurdererLevel(pcsTargetCharacter) >= AGPMCHAR_MURDERER_LEVEL2_POINT)
		return FALSE;

	// 변신 중인지 본다.
	if (pcsSelfCharacter->m_bIsTrasform && !pThis->m_pcsAgpmArchlord->IsArchlord(pcsSelfCharacter->m_szID) ||
		pcsTargetCharacter->m_bIsTrasform && !pThis->m_pcsAgpmArchlord->IsArchlord(pcsTargetCharacter->m_szID))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIItem::CBRequestPrivateTrade( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	BOOL			bResult;

	bResult = FALSE;

	if (!pClass || !pcsSourceControl)
		return bResult;

	AgcmUIItem			*pThis				= (AgcmUIItem *)		pClass;

	if (!pcsSourceControl->m_pstUserData ||
		!pcsSourceControl->m_pstUserData->m_stUserData.m_pvData)
		return bResult;

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return bResult;

	AgpdCharacter		*pcsTargetCharacter	= (AgpdCharacter *)		pcsSourceControl->m_pstUserData->m_stUserData.m_pvData;

	if (pThis->m_pcsAgpmCharacter->HasPenalty(pcsSelfCharacter, AGPMCHAR_PENALTY_PRVTRADE))
		return FALSE;

	if( pThis->m_pcsAgpmCharacter->HasPenalty(pcsSelfCharacter, AGPMCHAR_PENALTY_AUCTION))
		return FALSE;

	AgcdUI*		pUI		=	pThis->m_pcsAgcmUIManager2->GetUI( "UI_Auction" );

	if( pUI->m_pcsUIWindow->m_bOpened )
	{
		char* pMsg = pThis->m_pcsAgcmUIManager2->GetUIMessage( "CannotAuctionInPrivateTrade" );
		if( pMsg && strlen( pMsg ) > 0 )
		{
			pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( pMsg );
		}

		return FALSE;
	}

	if( pThis->m_pcsAgcmPrivateTrade->SendTradeRequest(pcsSelfCharacter->m_lID, pcsTargetCharacter->m_lID) )
	{
		//Event를 발생시키고 UI를 연다.
		if( pcsTargetCharacter != NULL )
		{
			pThis->m_pcsRequestPrivateTrade->m_stUserData.m_pvData = pcsTargetCharacter;

			bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lRequestPrivateTradeUIOpen);

			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsRequestPrivateTrade);
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBOpenReturnScroll(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;

	pcsSourceControl->m_pcsParentUI->m_bMainUI	= TRUE;

	return TRUE;
}

BOOL AgcmUIItem::CBUseReturnScroll(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;

	return pThis->m_pcsAgcmItem->SendUseReturnScroll();
	//return pThis->m_pcsAgcmItem->OpenProgressBarWaitingReturn( 5000 );
}

BOOL AgcmUIItem::CBCancelReturnScroll(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
//	if (!pClass)
//		return FALSE;
//
//	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;
//
//	pThis->m_pcsAgcmItem->SendCancelReturnScroll();
//	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventDisableReturnScroll);
//
//	pcsSourceControl->m_pcsParentUI->m_bMainUI	= FALSE;

	return TRUE;
}

BOOL AgcmUIItem::CBUpdateReturnScrollStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
//	if (!pData || !pClass)
//		return FALSE;
//
//	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;
//	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)		pData;
//
//	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
//		return TRUE;
//
//	AgpdItemADChar	*pcsItemADChar	= pThis->m_pcsAgpmItem->GetADCharacter(pcsCharacter);
//
//	if (pcsItemADChar->m_bUseReturnTeleportScroll)
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventEnableReturnScroll);
//	else
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventDisableReturnScroll);
//
//	return TRUE;

	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)		pData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	if (!pThis->m_pcsReturnScrollTemplate)
		return FALSE;

	AgpdItemADChar	*pcsItemADChar	= pThis->m_pcsAgpmItem->GetADCharacter(pcsCharacter);

	if (pcsItemADChar->m_bUseReturnTeleportScroll)
	{
		if (!pThis->m_pcsReturnScrollGridItem)
			pThis->m_pcsReturnScrollGridItem	= pThis->m_pcsAgpmGrid->CreateGridItem();

		if (!pThis->m_pcsReturnScrollGridItem)
			return FALSE;

		pThis->m_pcsReturnScrollGridItem->m_bMoveable			= FALSE;
		pThis->m_pcsReturnScrollGridItem->SetTooltip( "Return Scroll" );

	//	pThis->m_pcsReturnScrollGridItem->m_pcsTemplateGrid	= pcsItemTemplate->m_pcsGridItemSmall;
		pThis->m_pcsReturnScrollGridItem->m_pcsTemplateGrid	= pThis->m_pcsReturnScrollTemplate->m_pcsGridItem;

		pThis->m_pcsAgcmItem->SetGridItemAttachedSmallTexture(pThis->m_pcsReturnScrollTemplate, pThis->m_pcsReturnScrollGridItem);

		pThis->m_pcsAgcmUIMain->AddSystemMessageGridItem(pThis->m_pcsReturnScrollGridItem, CBUseReturnScroll, pThis);
	}
	else
	{
		pThis->m_pcsAgcmUIMain->RemoveSystemMessageGridItem(pThis->m_pcsReturnScrollGridItem);
	}

	return TRUE;
}

BOOL AgcmUIItem::CBUseReturnScrollResultFailed(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmUIItem::CBUseReturnScroll(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;



	return pThis->m_pcsAgcmItem->SendUseReturnScroll();
	//return pThis->m_pcsAgcmItem->OpenProgressBarWaitingReturn( 5000 );
}

BOOL AgcmUIItem::CBDisplayMessageData(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData ||
		eType != AGCDUI_USERDATA_TYPE_STRING ||
		!szDisplay)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;

	AgpdItem	*pcsItem	= pThis->m_pcsAgpmItem->GetItem(((AgcmUIItemMessageData *) pData)->lPickupItemID);

	switch (lID) {
	case AGCMUI_ITEM_DISPLAY_ID_MESSAGE_PICKUP_ITEM_NAME:
		{
			AgpdItemTemplate		*pcsItemTemplate		= pThis->m_pcsAgpmItem->GetItemTemplate(((AgcmUIItemMessageData *) pData)->lPickupItemTID);
			if (pcsItemTemplate)
			{
				if (pcsItemTemplate->m_lID == pThis->m_pcsAgpmItem->GetMoneyTID())
				{
					sprintf(szDisplay, "%d %s", ((AgcmUIItemMessageData *) pData)->lPickupItemCount, ClientStr().GetStr(STI_GELD_POST));
				}
				else if (pcsItemTemplate->m_bStackable)
				{
					sprintf(szDisplay, "%s %d %s", pcsItemTemplate->m_szName, ((AgcmUIItemMessageData *) pData)->lPickupItemCount, ClientStr().GetStr(STI_UNIT_POST));
				}
				else
				{
					if (pcsItem)
					{
						DWORD	dwColor	= pThis->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL);

						INT32	lItemRank	= 0;
						pThis->m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

						if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_ulTitleFontColor > 0)
							dwColor	= ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_ulTitleFontColor;
						else if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP && lItemRank == 2)
							dwColor	= pThis->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_RARE);
						else if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP && lItemRank == 3)
							dwColor = pThis->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_UNIQUE);
						else if (pcsItem->m_apcsOptionTemplate[0])
							//dwColor = AGCMUIITEM_TOOLTIP_COLOR_OPTION;	// 옵션이 있는 노멀 아이템을 하얀색으로 바꿨다
							dwColor = pThis->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL);
						else if (pThis->m_pcsAgpmItemConvert->IsEgoItem(pcsItem))
							dwColor	= pThis->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_EGO);
						else if (pThis->m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem) > 0)
							dwColor	= pThis->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_SPIRIT);

						// Item Name
						sprintf(szDisplay, "<C%d>%s", dwColor, ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_szName);

						// Physical Convert
						if ( pThis->m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsItem) > 0 )
							sprintf( szDisplay + strlen(szDisplay), " + %d", pThis->m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsItem) );

						// Socket Convert
						if ( pThis->m_pcsAgpmItemConvert->GetNumSocket(pcsItem) > 0 )
							sprintf( szDisplay + strlen(szDisplay), " [%d/%d]", pThis->m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem), pThis->m_pcsAgpmItemConvert->GetNumSocket(pcsItem) );

						sprintf(szDisplay + strlen(szDisplay), " <HFFFFFF>%s", ClientStr().GetStr(STI_ITEM_POST));

					}
					else
					{
						sprintf(szDisplay, "%s %s", pcsItemTemplate->m_szName, ClientStr().GetStr(STI_ITEM_POST));
					}
				}
			}
		}
		break;

	case AGCMUI_ITEM_DISPLAY_ID_MESSAGE_PICKUP_BONUS_MONEY:
		{
			sprintf(szDisplay, "%d", ((AgcmUIItemMessageData *) pData)->lPickupItemBonusCount);
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUIItem::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;

	pThis->m_bUIOpenBank			= FALSE;
	pThis->m_bUIOpenInventory		= FALSE;
	pThis->m_bUIOpenNPCTrade		= FALSE;

	return TRUE;
}

BOOL AgcmUIItem::CBUseItemSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgpdItem		*pcsItem		= (AgpdItem *)		pCustData;

	// 변신물약인 경우 변신 시간을 세팅한다.

	/*
	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
		((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
	{
		pThis->m_pcsAgcmUICharacter->SetTransformDurationMSec(((AgpdItemTemplateUsableTransform *) pcsItem->m_pcsItemTemplate)->m_ulDuration);
	}
	*/

	// GridItem에 cool time 을 표시해준다.
	pThis->SetUseItemTime(pcsItem);

	return TRUE;
}

BOOL AgcmUIItem::CBUseItemByTID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	INT32			lItemTID		= (INT32)			ppvBuffer[0];

	if (pcsCharacter != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return TRUE;

	AgpdItemTemplate	*pcsItemTemplate	= pThis->m_pcsAgpmItem->GetItemTemplate(lItemTID);
	if (!pcsItemTemplate)
		return FALSE;

	if (pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
	{
		if (((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_REVERSE_ORB)
		{
			return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUseSuccessReverseOrb);
		}
		else if (((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL)
		{
			AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(((AgpdItemTemplateUsableSkillScroll *) pcsItemTemplate)->m_lSkillTID);
			if (pcsSkillTemplate)
			{
				if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_MOVEMENT_FAST][((AgpdItemTemplateUsableSkillScroll *) pcsItemTemplate)->m_lSkillLevel] > 0)
				{
					pThis->m_lAddMovementPercent		= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_MOVEMENT_FAST][((AgpdItemTemplateUsableSkillScroll *) pcsItemTemplate)->m_lSkillLevel];

					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUseAddMovementSkill);
				}
				else if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_ATKSPEED][((AgpdItemTemplateUsableSkillScroll *) pcsItemTemplate)->m_lSkillLevel] > 0)
				{
					pThis->m_lAddAttackSpeedPercent	= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_ATKSPEED][((AgpdItemTemplateUsableSkillScroll *) pcsItemTemplate)->m_lSkillLevel];

					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUseAddAtkSpeedSkill);
				}
			}
		}
	}

	// 변신물약인 경우 변신 시간을 세팅한다.
	if (((AgpdItemTemplate *) pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
		((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
	{
		
		pThis->m_pcsAgcmUICharacter->SetTransformDurationMSec(((AgpdItemTemplateUsableTransform *) pcsItemTemplate)->m_ulDuration);
	}

	// GridItem에 cool time 을 표시해준다.
	if (((AgpdItemTemplate *) pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
		(((AgpdItemTemplateUsable *) pcsItemTemplate)->m_ulUseInterval > 0 || ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL))
	{
		AgpdItemADChar	*pcsItemADChar	= pThis->m_pcsAgpmItem->GetADCharacter(pcsCharacter);
		if (pcsItemADChar)
		{
			for (int i = 0; i < pcsItemADChar->m_csInventoryGrid.m_nLayer; ++i)
			{
				for (int j = 0; j < pcsItemADChar->m_csInventoryGrid.m_nColumn; ++j)
				{
					for (int k = 0; k < pcsItemADChar->m_csInventoryGrid.m_nRow; ++k)
					{
						AgpdItem	*pcsItem	= pThis->m_pcsAgpmItem->GetItem(pThis->m_pcsAgpmGrid->GetItem(&pcsItemADChar->m_csInventoryGrid, i, k, j));
						if (pcsItem && pcsItem->m_pcsItemTemplate && pcsItemTemplate == pcsItem->m_pcsItemTemplate)
						{
							pThis->SetUseItemTime(pcsItem);
						}
					}
				}
			}
		}

		if (pThis->m_pcsAgcmUIMain->m_pcsHPPotionItem &&
			pThis->m_pcsAgcmUIMain->m_pcsHPPotionItem->m_lItemTID == pcsItemTemplate->m_lID)
		{
			pThis->m_pcsAgcmUIMain->m_pcsHPPotionItem->SetUseItemTime(pThis->GetClockCount(), ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_ulUseInterval + ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_lEffectActivityTimeMsec * ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_lEffectApplyCount);
		}
		else if (pThis->m_pcsAgcmUIMain->m_pcsMPPotionItem &&
			pThis->m_pcsAgcmUIMain->m_pcsMPPotionItem->m_lItemTID == pcsItemTemplate->m_lID)
		{
			pThis->m_pcsAgcmUIMain->m_pcsMPPotionItem->SetUseItemTime(pThis->GetClockCount(), ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_ulUseInterval + ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_lEffectActivityTimeMsec * ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_lEffectApplyCount);
		}
	}

	return TRUE;
}

BOOL AgcmUIItem::CBUseItemFailedByTID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	INT32			lItemTID		= (INT32)			ppvBuffer[0];

	AgpdItemTemplate	*pcsItemTemplate	= pThis->m_pcsAgpmItem->GetItemTemplate(lItemTID);
	if (!pcsItemTemplate)
		return FALSE;

	if (pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
		((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_REVERSE_ORB)
	{
		return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUseFailReverseOrb);
	}

	return TRUE;
}

BOOL AgcmUIItem::CBUpdateReuseTimeForReverseOrb(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	// 캐쉬아이템은 돌지 않고, 일반 리버스 오브만 돈다. 2006.08.17. steeple
	AgpdItem		*pcsReverseOrb	= pThis->m_pcsAgpmItem->GetExistReverseOrb(pcsCharacter, FALSE);
	if (pcsReverseOrb && pcsReverseOrb->m_pcsGridItem)
	{
		AgpdItemTemplate	*pcsItemTemplate	= pcsReverseOrb->m_pcsItemTemplate;
		if (pcsItemTemplate && 
			((AgpdItemTemplateUsable *) pcsItemTemplate)->m_ulUseInterval > 0)
		{
			AgpdItemADChar	*pcsAttachItemData		= pThis->m_pcsAgpmItem->GetADCharacter(pcsCharacter);

			pcsReverseOrb->m_pcsGridItem->SetUseItemTime(pcsAttachItemData->m_ulUseReverseOrbTime, ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_ulUseInterval);
		}
	}

	if( pcsCharacter == pThis->m_pcsAgcmCharacter->GetSelfCharacter() )
		pThis->CBUpdateActionStatus( pcsCharacter, pThis, NULL );

	return TRUE;
}

BOOL AgcmUIItem::CBUpdateReuseTimeForTransform(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdItemADChar	*pcsAttachItemData	= pThis->m_pcsAgpmItem->GetADCharacter(pcsCharacter);

	if (AGPMITEM_TRANSFORM_RECAST_TIME <= 0)
		return TRUE;

	// 인벤토리에 있는 모든 아이템을 가져온다. 가져와서 변신 아이템인지 비교하고, 변신아이템이라면 모두 시간 세팅한다.
	for (int i = 0; i < pcsAttachItemData->m_csInventoryGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pcsAttachItemData->m_csInventoryGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pcsAttachItemData->m_csInventoryGrid.m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pcsAttachItemData->m_csInventoryGrid, i, k, j);
				if (!pcsGridItem)
					continue;

				AgpdItem	*pcsTransformItem	= pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
				if (!pcsTransformItem || !pcsTransformItem->m_pcsItemTemplate || !pcsTransformItem->m_pcsGridItem)
					continue;

				if (((AgpdItemTemplate *) pcsTransformItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
					((AgpdItemTemplateUsable *) pcsTransformItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
					pcsTransformItem->m_pcsGridItem->SetUseItemTime(pcsAttachItemData->m_ulUseTransformTime, AGPMITEM_TRANSFORM_RECAST_TIME);
			}
		}
	}

	return TRUE;
}

// 2006.12.14. steeple
BOOL AgcmUIItem::CBInitReuseTimeForTransform(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdItemADChar	*pcsAttachItemData	= pThis->m_pcsAgpmItem->GetADCharacter(pcsCharacter);

	// 인벤토리에 있는 모든 아이템을 가져온다. 가져와서 변신 아이템인지 비교하고, 변신아이템이라면 모두 시간 세팅한다.
	for (int i = 0; i < pcsAttachItemData->m_csInventoryGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pcsAttachItemData->m_csInventoryGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pcsAttachItemData->m_csInventoryGrid.m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pcsAttachItemData->m_csInventoryGrid, i, k, j);
				if (!pcsGridItem)
					continue;

				AgpdItem	*pcsTransformItem	= pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
				if (!pcsTransformItem || !pcsTransformItem->m_pcsItemTemplate || !pcsTransformItem->m_pcsGridItem)
					continue;

				if (((AgpdItemTemplate *) pcsTransformItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
					((AgpdItemTemplateUsable *) pcsTransformItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
					pcsTransformItem->m_pcsGridItem->SetUseItemTime(pcsAttachItemData->m_ulUseTransformTime, 0);
			}
		}
	}

	return TRUE;
}

BOOL AgcmUIItem::CBUpdateFactorDurability(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem	*pThis		= (AgcmUIItem *)	pClass;
	AgpdFactor	*pcsFactor	= (AgpdFactor *)	pData;

	INT32	lValue	= 0;
	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lValue, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_OWNER);

	AgpdItem	*pcsItem	= (AgpdItem *)	lValue;

	return pThis->CheckItemDurability(pcsItem);
}

BOOL AgcmUIItem::SetCallbackSetTooltip(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUIITEM_CB_SET_TOOLTIP, pfCallback, pClass);
}

BOOL AgcmUIItem::SetCallbackMovedFromGuildWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUIITEM_CB_MOVED_FROM_GUILDWAREHOUSE, pfCallback, pClass);
}

BOOL AgcmUIItem::SetCallbackRemoveSiegeWarGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUIITEM_CB_REMOVE_SIEGEWAR_GRID, pfCallback, pClass);
}

BOOL AgcmUIItem::SetCallbackGetItemTimeInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUIITEM_CB_GET_ITEM_TIME_INFO, pfCallback, pClass);
}

BOOL AgcmUIItem::SetCallbackGetItemStaminaTime(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUIITEM_CB_GET_ITEM_STAMINA_TIME, pfCallback, pClass);
}

BOOL AgcmUIItem::CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem				*pThis				= (AgcmUIItem *)			pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	ApmMap::RegionTemplate	*pcsRegionTemplate	= NULL;

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();

	if ( pcsSelfCharacter && pcsSelfCharacter == pcsCharacter )
	{
		if ( pcsSelfCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
		{
			AgpdItem	*pcsReverseOrb	= pThis->m_pcsAgpmItem->GetExistReverseOrb(pcsSelfCharacter);
			if( pcsReverseOrb )
			{
				pcsRegionTemplate	= pThis->m_pcsApmMap->GetTemplate(pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_nBindingRegionIndex);
				if ( pcsRegionTemplate && pcsRegionTemplate->ti.stType.uSafetyType == ApmMap::ST_DANGER )
				{

					// 위험전투지역이라도 캐쉬템은 사용가능 - arycoat 2009.6.11
					if(IS_CASH_ITEM(pcsReverseOrb->m_pcsItemTemplate->m_eCashItemType))
						pThis->m_bIsActiveReverseOrb	= TRUE;
					else
						pThis->m_bIsActiveReverseOrb	= FALSE;
				}
				else
					pThis->m_bIsActiveReverseOrb	= TRUE;
			}
			else
				pThis->m_bIsActiveReverseOrb	= FALSE;
		}
		else
			pThis->m_bIsActiveReverseOrb	= FALSE;

		// 어찌되었든간 아크로드가 죽으면 부활아이템 못 쓴다. 2007.05.23. steeple
		if(pThis->m_pcsAgpmArchlord->IsArchlord(pcsSelfCharacter->m_szID))
			pThis->m_bIsActiveReverseOrb = FALSE;

		// 부활 아이템이 사용 못하게 지정됐으면 못 쓰게 한다 !
		if( pcsRegionTemplate && pcsRegionTemplate->ti.stType.bItemResurrect == FALSE )
			pThis->m_bIsActiveReverseOrb = FALSE;

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsActiveReverseOrb);
	}

	return TRUE;
}

BOOL AgcmUIItem::CBUseReverseOrb(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem	*pThis	= (AgcmUIItem *)	pClass;

	AgpdItem	*pcsReverseOrb	= pThis->m_pcsAgpmItem->GetExistReverseOrb(pThis->m_pcsAgcmCharacter->GetSelfCharacter());
	if (!pcsReverseOrb)
		return FALSE;

	return pThis->m_pcsAgcmItem->UseItem(pcsReverseOrb);
}

BOOL AgcmUIItem::CBInventoryDoubleClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIItem	*pThis	= (AgcmUIItem *)	pClass;

	if (!pcsSourceControl || (pcsSourceControl->m_lType != AcUIBase::TYPE_GRID) || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AcUIGrid	*pcsUIGrid = (AcUIGrid *) pcsSourceControl->m_pcsBase;

	AgpdGridSelectInfo	*pcsGridSelectInfo	= pcsUIGrid->GetGridItemClickInfo();
	if (!pcsGridSelectInfo || !pcsGridSelectInfo->pGridItem)
		return FALSE;

	AgpdItem	*pcsAgpdItem	= pThis->m_pcsAgpmItem->GetItem(pcsGridSelectInfo->pGridItem);
	if (pcsAgpdItem &&
		(pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_INVENTORY ||
		 pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_SUB_INVENTORY)
		)
	{
		if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP)
			return pThis->m_pcsAgcmItem->SendEquipInfo(pcsAgpdItem->m_lID, pThis->m_pcsAgcmCharacter->m_lSelfCID);
	}

	return TRUE;
}

BOOL AgcmUIItem::CBEquipDoubleClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIItem	*pThis	= (AgcmUIItem *)	pClass;

	if (!pcsSourceControl || (pcsSourceControl->m_lType != AcUIBase::TYPE_GRID) || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AcUIGrid	*pcsUIGrid = (AcUIGrid *) pcsSourceControl->m_pcsBase;

	AgpdGridSelectInfo	*pcsGridSelectInfo	= pcsUIGrid->GetGridItemClickInfo();
	if (!pcsGridSelectInfo || !pcsGridSelectInfo->pGridItem)
		return FALSE;

	AgpdItem	*pcsAgpdItem	= pThis->m_pcsAgpmItem->GetItem(pcsGridSelectInfo->pGridItem);
	if (pcsAgpdItem && pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
	{
		// 벗는 행동이므로 벗을 수 있는지 한번 확인을 거친다.
		if( !pThis->IsEnableUnEquipItem( pcsAgpdItem ) ) return FALSE;

		return pThis->m_pcsAgcmItem->SendInventoryInfo(pcsAgpdItem->m_lID, pThis->m_pcsAgcmCharacter->m_lSelfCID);
	}

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayUseAddMovementSkill(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIItem	*pThis		= (AgcmUIItem *)	pClass;

	sprintf(szDisplay, "%d", pThis->m_lAddMovementPercent);

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayUseAddAtkSpeedSkill(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIItem	*pThis		= (AgcmUIItem *)	pClass;

	sprintf(szDisplay, "%d", pThis->m_lAddAttackSpeedPercent);

	return TRUE;
}

VOID AgcmUIItem::SetUseItemTime(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return;

	if (pcsItem->m_pcsGridItem &&
		((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE)
	{
		UINT32	ulInterval = ((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_ulUseInterval;
		if (!ulInterval && ((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL)
		{
			AgpdItemTemplateUsableSkillScroll *	pcsScroll = (AgpdItemTemplateUsableSkillScroll *) pcsItem->m_pcsItemTemplate;
			AgpdSkillTemplate *	pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(pcsScroll->m_lSkillTID);
			if (pcsSkillTemplate)
				ulInterval = m_pcsAgpmSkill->GetRecastDelay(pcsSkillTemplate, pcsScroll->m_lSkillLevel);
		}

		pcsItem->m_pcsGridItem->m_ulRemainTime = ((AgpdItemTemplateUsable*)pcsItem->m_pcsItemTemplate)->m_ulUseInterval;
		pcsItem->m_pcsGridItem->SetUseItemTime(GetClockCount(), ulInterval + ((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_lEffectActivityTimeMsec * ((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_lEffectApplyCount);
	}
}

BOOL AgcmUIItem::CBUpdateCooldown(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIItem* pThis = static_cast<AgcmUIItem*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AgpdItemCooldownBase* pstCooldownBase = static_cast<AgpdItemCooldownBase*>(pCustData);

	// 자기 캐릭이 아니라면 아래 작업들은 안해도 된다.
	if(pcsCharacter != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return FALSE;

	// 가지고 있는 모든 아이템을 돌면서 TID 해당하는 아이템들의 Grid 시간을 설정해준다.
	AgpdItemTemplate* pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pstCooldownBase->m_lTID);
	if(!pcsItemTemplate)
		return FALSE;

	AgpdItemADChar* pcsItemADChar = pThis->m_pcsAgpmItem->GetADCharacter(pcsCharacter);
	AgpdGridItem* pcsGridItem = NULL;
	INT32 lIndex = 0;

	if(IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
	{
		// 캐쉬 인벤만 돌면 된다.
		for(pcsGridItem = pThis->m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &lIndex);
			pcsGridItem;
			pcsGridItem = pThis->m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &lIndex))
		{
			AgpdItem* pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
			if(!pcsItem || !pcsItem->m_pcsItemTemplate || pcsItem->m_pcsItemTemplate->m_lID != pstCooldownBase->m_lTID)
				continue;

			pThis->UpdateCooldown(pcsItem, *pstCooldownBase);
		}
	}
	else
	{
		// 전체 다 돈다.
		for(pcsGridItem = pThis->m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &lIndex);
			pcsGridItem;
			pcsGridItem = pThis->m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &lIndex))
		{
			AgpdItem* pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
			if(!pcsItem || !pcsItem->m_pcsItemTemplate || pcsItem->m_pcsItemTemplate->m_lID != pstCooldownBase->m_lTID)
				continue;

			pThis->UpdateCooldown(pcsItem, *pstCooldownBase);
		}

		for(pcsGridItem = pThis->m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csEquipGrid, &lIndex);
			pcsGridItem;
			pcsGridItem = pThis->m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csEquipGrid, &lIndex))
		{
			AgpdItem* pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
			if(!pcsItem || !pcsItem->m_pcsItemTemplate || pcsItem->m_pcsItemTemplate->m_lID != pstCooldownBase->m_lTID)
				continue;

			pThis->UpdateCooldown(pcsItem, *pstCooldownBase);
		}

		// Bank 는 하지 말자.
	}

	return TRUE;
}

inline BOOL AgcmUIItem::UpdateCooldown(AgpdItem* pcsItem, AgpdItemCooldownBase& stCooldownBase)
{
	if(!pcsItem || !pcsItem->m_pcsGridItem)
		return FALSE;

	if(pcsItem->m_pcsItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
		return TRUE;

	pcsItem->m_pcsGridItem->m_ulRemainTime = stCooldownBase.m_ulRemainTime;

	UINT32 ulClockCount = GetClockCount();

	// Pause 상태가 변경된 거라면 아래처럼 해 주어야 한다.
	// 현재 돌아가고 있고, 0 이 아니라면 세팅안해도 된다.
	if( stCooldownBase.m_bPause == stCooldownBase.m_bPrevPause &&
		stCooldownBase.m_ulRemainTime && 
		pcsItem->m_pcsGridItem->m_ulUseItemTime && 
		pcsItem->m_pcsGridItem->m_ulReuseIntervalTime &&
		pcsItem->m_pcsGridItem->m_ulUseItemTime + pcsItem->m_pcsGridItem->m_ulReuseIntervalTime > ulClockCount )
		return TRUE;

	UINT32 ulUseInterval = m_pcsAgpmItem->GetReuseInterval( pcsItem->m_pcsItemTemplate );
	if( ulUseInterval )
	{
		UINT32 ulDiff = ulUseInterval - stCooldownBase.m_ulRemainTime;
		if( ulDiff < ulClockCount )		ulClockCount -= ulDiff;
		else							ulClockCount = 1;
	}

	pcsItem->m_pcsGridItem->SetUseItemTime( ulClockCount, ulUseInterval, stCooldownBase.m_bPause ? GetClockCount() : 0 );

	return TRUE;
}

BOOL AgcmUIItem::CBCharacterLevelUp(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem			*pThis				= (AgcmUIItem *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter ||
		pcsSelfCharacter->m_lID != pcsCharacter->m_lID)
		return TRUE;

	pThis->CheckItemDurability( pcsSelfCharacter );

	return TRUE;
}

BOOL AgcmUIItem::CBUpdateTradeOptionGrid( PVOID pData , PVOID pClass , PVOID pCustData )
{
	AgcmUIItem*		pThis			=	static_cast		< AgcmUIItem*		>(pClass);
	DWORD			dwIID			=	reinterpret_cast< DWORD				>(pData);
	AgpdCharacter*	pcsCharacter	=	static_cast		< AgpdCharacter*	>(pCustData);

	if( !pThis )
		return FALSE;

	AgpdItem*	pItem	=	pThis->m_pcsAgpmItem->GetItem( dwIID );
	BOOL		bSelf	=	FALSE;
	DWORD		dwCount	=	0;

	ASSERT( pItem );

	// 본인
	if( pThis->m_pcsAgcmCharacter->GetSelfCharacter() == pcsCharacter )
	{
		dwCount		=	pThis->m_pcsAgpmGrid->GetItemCount( &pThis->m_PrivateTradeGridEx );
		pThis->m_pcsAgpmGrid->Add( &pThis->m_PrivateTradeGridEx , 0, dwCount, 0, pItem->m_pcsGridItem, 1, 1 );
		pThis->m_pcsAgpmGrid->AddItem( &pThis->m_PrivateTradeGridEx , NULL );
		bSelf		=	TRUE;
	}

	// 상대방
	else
	{

		dwCount		=	pThis->m_pcsAgpmGrid->GetItemCount( &pThis->m_PrivateTradeClientGridEx );
		pThis->m_pcsAgpmGrid->Add( &pThis->m_PrivateTradeClientGridEx , 0 , dwCount , 0 , pItem->m_pcsGridItem , 1 , 1 );
		pThis->m_pcsAgpmGrid->AddItem( &pThis->m_PrivateTradeClientGridEx , NULL );
		bSelf		=	FALSE;

	}

	// 중복 체크를 하고 AgpdItemADCharacter에 정보를 넣는다
	if( pThis->_CheckSupportItemDuplicate( pItem ) )
	{
		pThis->_AddSupportItem( pItem , bSelf );
	}

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsPrivateTradeEx );
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsPrivateTradeClientEx );

	return TRUE;
}