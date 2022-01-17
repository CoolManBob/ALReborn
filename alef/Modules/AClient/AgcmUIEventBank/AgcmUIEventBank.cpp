#include "AgcmUIEventBank.h"
#include "AuStrTable.h"

AgcmUIEventBank::AgcmUIEventBank()
{
	SetModuleName("AgcmUIEventBank");

	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmItem			= NULL;

	m_pcsAgcmCharacter		= NULL;
	m_pcsAgcmItem			= NULL;
	m_pcsAgcmEventBank		= NULL;
	m_pcsAgcmUIManager2		= NULL;

	m_pcsUserDataBank		= NULL;
	m_pcsUserDataDummy		= NULL;

	m_pcsBankGrid			= NULL;

	ZeroMemory(&m_stBankOpenPos, sizeof(AuPOS));

	m_bIsInputMoneyToBank	= FALSE;

	m_bIsBankUIOpen			= FALSE;
}

AgcmUIEventBank::~AgcmUIEventBank()
{
}

BOOL AgcmUIEventBank::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgcmItem			= (AgcmItem *)			GetModule("AgcmItem");
	m_pcsAgcmEventBank		= (AgcmEventBank *)		GetModule("AgcmEventBank");
	m_pcsAgcmUIManager2		= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmItem ||
		!m_pcsAgcmEventBank ||
		!m_pcsAgcmUIManager2)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateBankMoney(CBUpdateBankMoney, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackUpdateBankSize(CBUpdateBankSize, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackBank(CBUpdateBank, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUIUpdateBank(CBUpdateBankUI, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackAddItemToBank(CBAddItemToBank, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackRemoveItemFromBank(CBRemoveItemFromBank, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	if (!m_pcsAgcmEventBank->SetCallbackGrantEventBank(CBGrantBankEvent, this))
		return FALSE;

	if (!AddEvent())
		return FALSE;
	if (!AddFunction())
		return FALSE;
	if (!AddDisplay())
		return FALSE;
	if (!AddUserData())
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventBank::AddEvent()
{
	m_lEventOpenBankUI = m_pcsAgcmUIManager2->AddEvent("Bank_OpenUI");
	if (m_lEventOpenBankUI < 0)
		return FALSE;

	m_lEventCloseBankUI = m_pcsAgcmUIManager2->AddEvent("Bank_CloseUI");
	if (m_lEventCloseBankUI < 0)
		return FALSE;

	m_lEventBankUpdate = m_pcsAgcmUIManager2->AddEvent("Bank_ItemUpdate");
	if (m_lEventBankUpdate < 0)
		return FALSE;

	m_lEventConfirmBuyBankSlot		= m_pcsAgcmUIManager2->AddEvent("Item_ConfirmBuyBankSlot");
	if (m_lEventConfirmBuyBankSlot < 0)
		return FALSE;

	m_lEventNotEnoughBuyCost	= m_pcsAgcmUIManager2->AddEvent("Bank_NotEnoughBuyCost");
	if (m_lEventNotEnoughBuyCost < 0)
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventBank::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Bank_MoveItem", CBBankMoveItem, 1))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Bank_Money_Input", CBInputMoneyToBank, 1, "money edit control"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Bank_Money_Set_Input", CBSetInputMoneyToBank, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Bank_Money_Set_Output", CBSetOutputMoneyFromBank, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Item_RequestBuyBankSlot", CBRequestBuyBankSlot, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Item_BuyBankSlot", CBBuyBankSlot, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Item_CancelBankSlot", CBCancelBankSlot, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Item_SetBankPrice", CBSetBankPrice, 1, "BankPrice Edit Control"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Item_BankUpdateLayer", CBBankUpdateLayer, 1, "Bank Grid Control"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Item_BankDragDropItem", CBBankDragDropItem, 1 ) )
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActiveBuyBankSlot", CBIsActiveBuyBankSlot, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActiveBankSlot1", CBIsActiveBankSlot1, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActiveBankSlot2", CBIsActiveBankSlot2, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActiveBankSlot3", CBIsActiveBankSlot3, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventBank::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "BankMoney", AGCMUIEVENTBANK_DISPLAY_ID_BANK_MONEY, CBDisplayBankMoney, AGCDUI_USERDATA_TYPE_UINT64))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "BankMoneyConfirm", AGCMUIEVENTBANK_DISPLAY_ID_BANK_MONEY_CONFIRM, CBDisplayConfirmMessage, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ItemBankBuySlotTitle", AGCMUIEVENTBANK_DISPLAY_ID_BUY_BANK_SLOT_TITLE, CBDisplayBuyBankSlotTitle, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventBank::AddUserData()
{
	m_pcsUserDataBank = m_pcsAgcmUIManager2->AddUserData("BankGrid", m_pcsBankGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pcsUserDataBank)
		return FALSE;

	m_pcsUserDataBankMoney = m_pcsAgcmUIManager2->AddUserData("BankMoney", &m_llBankMoney, sizeof(INT64), 1, AGCDUI_USERDATA_TYPE_UINT64);
	if (!m_pcsUserDataBank)
		return FALSE;

	m_pcsUserDataDummy = m_pcsAgcmUIManager2->AddUserData("BankDummy", &m_lDummyData, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataDummy)
		return FALSE;

	if (!(m_pcsBankLayerIndex = m_pcsAgcmUIManager2->AddUserData("BankLayerIndex", &m_lBankLayerIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventBank::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventBank		*pThis				= (AgcmUIEventBank *)	pClass;
	AgpdCharacter		*pcsSelfCharacter	= (AgpdCharacter *)		pData;

	AgpdItemADChar		*pcsItemADChar		= pThis->m_pcsAgpmItem->GetADCharacter(pcsSelfCharacter);

	pThis->m_pcsBankGrid		= &pcsItemADChar->m_csBankGrid;
	pThis->m_llBankMoney		= pcsSelfCharacter->m_llBankMoney;

	pThis->m_pcsUserDataBank->m_stUserData.m_pvData			= pThis->m_pcsBankGrid;
	pThis->m_pcsUserDataBankMoney->m_stUserData.m_pvData	= &pThis->m_llBankMoney;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataBank);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataBankMoney);

	return TRUE;
}

BOOL AgcmUIEventBank::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmUIEventBank::CBSelfUpdatePosition");

	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventBank			*pThis				= (AgcmUIEventBank *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	if (!pThis->m_bIsBankUIOpen)
		return TRUE;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pThis->m_stBankOpenPos);

	if ((INT32) fDistance < AGCMUIEVENTBANK_CLOSE_UI_DISTANCE)
		return TRUE;

	pThis->m_bIsBankUIOpen	= FALSE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseBankUI);

	return TRUE;
}

BOOL AgcmUIEventBank::CBUpdateBank(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventBank		*pThis				= (AgcmUIEventBank *)	pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)			pData;

	if (!pcsItem->m_pcsCharacter)
		return TRUE;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsItem->m_pcsCharacter)
		return TRUE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataBank);

	return TRUE;
}

BOOL AgcmUIEventBank::CBUpdateBankUI(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventBank		*pThis				= (AgcmUIEventBank *)	pClass;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataBank);

	return TRUE;
}

BOOL AgcmUIEventBank::CBAddItemToBank(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventBank		*pThis				= (AgcmUIEventBank *)	pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBankUpdate);

	return TRUE;
}

BOOL AgcmUIEventBank::CBRemoveItemFromBank(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventBank		*pThis				= (AgcmUIEventBank *)	pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBankUpdate);

	return TRUE;
}

BOOL AgcmUIEventBank::CBUpdateBankMoney(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventBank		*pThis				= (AgcmUIEventBank *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	
	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() == pcsCharacter)
	{
		pThis->m_llBankMoney	= pThis->m_pcsAgpmCharacter->GetBankMoney(pcsCharacter);

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataBankMoney);
	}

	return TRUE;
}

BOOL AgcmUIEventBank::CBGrantBankEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventBank		*pThis				= (AgcmUIEventBank *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	pThis->m_stBankOpenPos	= pcsCharacter->m_stPos;

	pThis->m_bIsBankUIOpen	= TRUE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsBankLayerIndex );

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenBankUI);
}

BOOL AgcmUIEventBank::CBBankMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIEventBank			*pThis				= (AgcmUIEventBank *)	pClass;
	AgcdUIControl			*pcsControl			= pcsSourceControl;

	if (!pcsControl ||
		!pcsControl->m_pcsBase ||
		pcsControl->m_lType != AcUIBase::TYPE_GRID)
		return FALSE;

	AcUIGrid				*pcsUIGrid			= (AcUIGrid *) pcsControl->m_pcsBase;
	AgpdGridSelectInfo		*pstGridSelectInfo	= pcsUIGrid->GetDragDropMessageInfo();
	if (!pstGridSelectInfo || !pstGridSelectInfo->pGridItem || pstGridSelectInfo->pGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM)
		return FALSE;

	AgpdGrid				*pcsControlGrid		= pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);
	if (!pcsControlGrid)
		return FALSE;

	AgpdItem				*pcsItem			= pThis->m_pcsAgpmItem->GetItem(pstGridSelectInfo->pGridItem->m_lItemID);
	if (!pcsItem)
		return FALSE;

	if (pcsItem->m_eStatus == AGPDITEM_STATUS_INVENTORY || pcsItem->m_eStatus == AGPDITEM_STATUS_SUB_INVENTORY)
	{
		// 인벤토리에서 뱅크로 옮겨왔다.
		pThis->m_pcsAgcmItem->SendBankInfo(pcsItem->m_lID, pThis->m_pcsAgcmCharacter->GetSelfCID(), pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn);
	}
	else if (pcsItem->m_eStatus == AGPDITEM_STATUS_BANK)
	{
		// 뱅크 내에서 위치 저조절이 있는 경우이다.
		// 먼저 같은 자리인지 본다. 같은 자리면 암것도 안한다.
		if (pcsItem->m_anGridPos[AGPDITEM_GRID_POS_TAB] != pcsUIGrid->m_lNowLayer ||
			pcsItem->m_anGridPos[AGPDITEM_GRID_POS_ROW] != pstGridSelectInfo->lGridRow ||
			pcsItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN] != pstGridSelectInfo->lGridColumn)
		{
			pThis->m_pcsAgcmItem->SendBankInfo(pcsItem->m_lID, pThis->m_pcsAgcmCharacter->GetSelfCID(), pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn);
		}
	}

	return TRUE;
}

BOOL AgcmUIEventBank::CBInputMoneyToBank(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIEventBank			*pThis				= (AgcmUIEventBank *)	pClass;
	AgcdUIControl			*pcsMoneyControl	= (AgcdUIControl *)		pData1;

	if (pcsMoneyControl->m_lType != AcUIBase::TYPE_EDIT)
		return FALSE;

	INT64 llMoney = (INT64) atof(((AcUIEdit *) pcsMoneyControl->m_pcsBase)->GetText());
	if (llMoney <= 0)
		return FALSE;

	if (pThis->m_bIsInputMoneyToBank)
	{
		pThis->m_pcsAgcmCharacter->SendMoveMoneyFromInventoryToBank(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), llMoney);
	}
	else
	{
		AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if (pcsCharacter)
		{
			// 소지 금액 초과 검사
			if (pcsCharacter->m_llMoney + llMoney <= AGPDCHARACTER_MAX_INVEN_MONEY)
				pThis->m_pcsAgcmCharacter->SendMoveMoneyFromBankToInventory(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), llMoney);
			else
				pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog(ClientStr().GetStr(STI_HAVE_NOT_MORE_GELD));
		}
	}

	pThis->m_bIsInputMoneyToBank = FALSE;

	return	TRUE;;
}

BOOL AgcmUIEventBank::CBSetInputMoneyToBank(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventBank			*pThis				= (AgcmUIEventBank *)	pClass;

	pThis->m_bIsInputMoneyToBank	= TRUE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataDummy);

	return TRUE;
}

BOOL AgcmUIEventBank::CBSetOutputMoneyFromBank(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventBank			*pThis				= (AgcmUIEventBank *)	pClass;

	pThis->m_bIsInputMoneyToBank	= FALSE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataDummy);

	return TRUE;
}

BOOL AgcmUIEventBank::CBDisplayBankMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData ||
		eType != AGCDUI_USERDATA_TYPE_UINT64 ||
		lID != AGCMUIEVENTBANK_DISPLAY_ID_BANK_MONEY ||
		!szDisplay)
		return FALSE;

	sprintf(szDisplay, "%I64d", *(INT64 *) pData);

	return TRUE;
}

BOOL AgcmUIEventBank::CBDisplayConfirmMessage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass ||
		lID != AGCMUIEVENTBANK_DISPLAY_ID_BANK_MONEY_CONFIRM ||
		!szDisplay)
		return FALSE;

	AgcmUIEventBank		*pThis		= (AgcmUIEventBank *)	pClass;
	CHAR				*szMessage	= NULL;

	szDisplay[0] = 0;

	if (pThis->m_bIsInputMoneyToBank)
		szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_BANK_INPUT_MONEY_CONFIRM);
	else
		szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_BANK_OUTPUT_MONEY_CONFIRM);

	if (!szMessage)
		return FALSE;

	sprintf(szDisplay, szMessage);

	return TRUE;
}

BOOL AgcmUIEventBank::CBUpdateBankSize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventBank		*pThis				= (AgcmUIEventBank *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)			pData;

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();

	if (!pcsSelfCharacter || pcsSelfCharacter != pcsCharacter)
		return TRUE;

	return pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsBankLayerIndex);
}

BOOL AgcmUIEventBank::CBBuyBankSlot(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventBank	*pThis	= (AgcmUIEventBank *)	pClass;

	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	INT64	llBuyBankSlotCost	= pThis->m_pcsAgpmItem->GetBuyBankSlotCost(pcsSelfCharacter);
	if (pcsSelfCharacter->m_llMoney < llBuyBankSlotCost)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotEnoughBuyCost);
		return TRUE;
	}

	return pThis->m_pcsAgcmItem->SendBuyBankSlot(pcsSelfCharacter->m_lID);
}

BOOL AgcmUIEventBank::CBCancelBankSlot(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

    AgcmUIEventBank	*pThis	= (AgcmUIEventBank *)	pClass;

	return TRUE;
}

BOOL AgcmUIEventBank::CBRequestBuyBankSlot(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventBank	*pThis	= (AgcmUIEventBank *)	pClass;

	// 구입 가능한지 검사한다.

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsBankLayerIndex);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventConfirmBuyBankSlot);

	return TRUE;
}

BOOL AgcmUIEventBank::CBSetBankPrice(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIEventBank		*pThis			= (AgcmUIEventBank *)	pClass;
	AgcdUIControl	*pcsEditControl	= (AgcdUIControl *)	pData1;

	if (pcsEditControl->m_lType != AcUIBase::TYPE_EDIT ||
		!pcsEditControl->m_pcsBase)
		return FALSE;

	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	// 창고 구입 가격을 세팅한다.

	CHAR	szBuffer[512];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	INT64	llBuyBankSlotCost	= pThis->m_pcsAgpmItem->GetBuyBankSlotCost(pcsSelfCharacter);
	if (pcsSelfCharacter->m_llMoney < llBuyBankSlotCost)
	{
		sprintf(szBuffer, "%s : <C16711680>%I64d<C16773320> %s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_BUY_BANK_SLOT_PRICE),
										pThis->m_pcsAgpmItem->GetBuyBankSlotCost(pThis->m_pcsAgcmCharacter->GetSelfCharacter()),
										pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME));
	}
	else
	{
		sprintf(szBuffer, "%s : <C10092288>%I64d<C16773320> %s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_BUY_BANK_SLOT_PRICE),
										pThis->m_pcsAgpmItem->GetBuyBankSlotCost(pThis->m_pcsAgcmCharacter->GetSelfCharacter()),
										pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME));
	}

	return ((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
}

BOOL AgcmUIEventBank::CBBankUpdateLayer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIEventBank		*pThis			= (AgcmUIEventBank *)	pClass;
	AgcdUIControl	*pcsGridControl	= (AgcdUIControl *)	pData1;

	if (pcsGridControl)
	{
		AgcdUIUserData	*pstUserData = pThis->m_pcsAgcmUIManager2->GetControlUserData(pcsSourceControl);
		if (pstUserData && pstUserData->m_lSelectedIndex != -1)
			((AcUIGrid *) pcsGridControl->m_pcsBase)->SetNowLayer(pstUserData->m_lSelectedIndex);
	}

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsUserDataBank );
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsBankLayerIndex );

	return TRUE;
}

BOOL AgcmUIEventBank::CBBankDragDropItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass ||
		!pcsSourceControl ||
		!pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIEventBank			*pThis				= (AgcmUIEventBank *)			pClass;
	AcUIButton			*pcsUIButton		= (AcUIButton *)			pcsSourceControl->m_pcsBase;

	AcUIGridItem		*pcsDragDropItem	= pcsUIButton->GetDragDropGridItem();
	if (!pcsDragDropItem || !pcsDragDropItem->m_ppdGridItem)
		return FALSE;

	AgpdGridItem		*pcsGridItem		= pcsDragDropItem->m_ppdGridItem;
	if (pcsGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM ||
		pcsGridItem->m_lItemID == AP_INVALID_IID)
		return FALSE;

	INT32				lLayerIndex			= pcsSourceControl->m_lUserDataIndex;

	AgpdItem			*pcsAgpdItem		= pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (pcsAgpdItem &&
		pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_BANK)
	{
		return FALSE;
	}

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() &&
		pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_cBankSize < lLayerIndex)
		return FALSE;

	return pThis->m_pcsAgcmItem->SendBankInfo(pcsGridItem->m_lItemID, pThis->m_pcsAgcmCharacter->GetSelfCID(), lLayerIndex);
}

BOOL AgcmUIEventBank::CBIsActiveBuyBankSlot(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventBank	*pThis	= (AgcmUIEventBank *)	pClass;

	AgpdCharacter		*pcsSelfCharacter	= (AgpdCharacter *)		pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (!pThis->m_pcsAgpmItem->IsBuyBankSlot(pcsSelfCharacter))
		return FALSE;

	if (pThis->m_pcsAgpmItem->m_llBankSlotPrice[pcsSelfCharacter->m_cBankSize + 1] < 0)
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventBank::CBIsActiveBankSlot1(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventBank	*pThis	= (AgcmUIEventBank *)	pClass;

	AgpdCharacter		*pcsSelfCharacter	= (AgpdCharacter *)		pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter->m_cBankSize >= 1)
		return TRUE;

	return FALSE;
}

BOOL AgcmUIEventBank::CBIsActiveBankSlot2(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventBank	*pThis	= (AgcmUIEventBank *)	pClass;

	AgpdCharacter		*pcsSelfCharacter	= (AgpdCharacter *)		pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter->m_cBankSize >= 2)
		return TRUE;

	return FALSE;
}

BOOL AgcmUIEventBank::CBIsActiveBankSlot3(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventBank	*pThis	= (AgcmUIEventBank *)	pClass;

	AgpdCharacter		*pcsSelfCharacter	= (AgpdCharacter *)		pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter->m_cBankSize >= 3)
		return TRUE;

	return FALSE;
}

BOOL AgcmUIEventBank::CBDisplayBuyBankSlotTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventBank	*pThis		= (AgcmUIEventBank *)	pClass;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		sprintf(szDisplay, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_BUY_BANK_SLOT_TITLE), pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_cBankSize + 1);

	return TRUE;
}