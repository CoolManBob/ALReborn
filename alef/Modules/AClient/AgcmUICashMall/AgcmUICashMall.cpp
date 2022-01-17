#include "AgcmUICashMall.h"
#include "AgcmLogin.h"
#include "AuStrTable.h"

AgcmUICashMall::AgcmUICashMall()
{
	SetModuleName("AgcmUICashMall");

	m_lCurrentTab	= 0;
	m_lStartPage	= 0;
	m_lCurrentPage	= 0;
	m_lMaxPage		= 0;
	m_lCash			= 0;

	m_pcsUserDataCurrentTab		= NULL;
	m_pcsUserDataCurrentPage	= NULL;
	m_pcsUserDataItemGrid		= NULL;
	m_pcsUserDataCash			= NULL;
	m_pcsUserDataMaxPage		= NULL;

	m_pcsEditCash			= NULL;
	m_pcsListControl		= NULL;

	m_bReceiveMallInfo		= FALSE;

	m_pcsBuyItem			= NULL;

	m_bIsOpenedUI			= FALSE;
}

AgcmUICashMall::~AgcmUICashMall()
{
}

BOOL AgcmUICashMall::OnAddModule()
{
	m_pcsAgpmCharacter	= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmGrid		= (AgpmGrid *)			GetModule("AgpmGrid");
	m_pcsAgpmItem		= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmCashMall	= (AgpmCashMall *)		GetModule("AgpmCashMall");
	m_pcsAgcmCharacter	= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgcmCashMall	= (AgcmCashMall *)		GetModule("AgcmCashMall");
	m_pcsAgcmUIManager2	= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");
	m_pcsAgcmUIMain		= (AgcmUIMain *)		GetModule("AgcmUIMain");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmGrid ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmCashMall ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmCashMall ||
		!m_pcsAgcmUIManager2 ||
		!m_pcsAgcmUIMain)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateCash(CBUpdateCash, this))
		return FALSE;

	if (!m_pcsAgpmCashMall->SetCallbackUpdateMallList(CBUpdateMallList, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;

	if (!m_pcsAgpmCashMall->SetCallbackResponseBuyResult(CBResponseBuyResult, this))
		return FALSE;

	if (!m_pcsAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	if (!AddUserData() ||
		!AddDisplay() ||
		!AddEvent() ||
		!AddFunction() ||
		!AddBoolean())
		return FALSE;

	return TRUE;
}

BOOL AgcmUICashMall::OnInit()
{
	for (int i = 0; i < AGCMUICASHMALL_MAX_DISPLAY_COUNT; ++i)
		m_pcsAgpmGrid->Init(&m_acsItemGrid[i], 1, 1, 1);

	return TRUE;
}

BOOL AgcmUICashMall::OnDestroy()
{
	for (int i = 0; i < AGCMUICASHMALL_MAX_DISPLAY_COUNT; ++i)
		m_pcsAgpmGrid->Remove(&m_acsItemGrid[i]);

#ifdef _AREA_KOREA_
	// 웹젠 InGameShop
	m_WebzenShop.OnWebzenShopDestroy();
#endif

	return TRUE;
}

BOOL AgcmUICashMall::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallGetEdit", CBGetEditControl, 1, "cash edit control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallGetList", CBGetListControl, 1, "list control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallCashCharge", CBCashCharge, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallCheckCash", CBCheckCash, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallBuyProductConfirm", CBBuyProductConfirm, 1, "list userdata"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallBuyProduct", CBBuyProduct, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallOpenUI", CBOpenCashMallUI, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallSelectTab", CBSelectTab, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallPageLeft", CBPageLeft, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallPageRight", CBPageRight, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CashMallGoPage", CBGoPage, 0))
		return FALSE;

	return TRUE;
}

BOOL AgcmUICashMall::AddBoolean()
{
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "CashMall_ShowTabButton", CBIsShowTabButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "CashMall_IsActivePage", CBIsActivePage, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "CashMall_IsActiveBuy", CBIsActiveBuy, AGCDUI_USERDATA_TYPE_ITEM))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "CashMall_ShowChargeButton", CBIsShowChargeButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
}

BOOL AgcmUICashMall::AddUserData()
{
	m_pcsUserDataCurrentTab = m_pcsAgcmUIManager2->AddUserData("CashMallCurrentTab", &m_lCurrentTab, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataCurrentTab)
		return FALSE;

	m_pcsUserDataCurrentPage = m_pcsAgcmUIManager2->AddUserData("CashMallCurrentPage", &m_lCurrentPage, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataCurrentPage)
		return FALSE;

	m_pcsUserDataItemGrid = m_pcsAgcmUIManager2->AddUserData("CashMallListItemGrid", m_acsItemGrid, sizeof(AgpdGrid), AGCMUICASHMALL_MAX_DISPLAY_COUNT, AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pcsUserDataItemGrid)
		return FALSE;

	m_pcsUserDataCash = m_pcsAgcmUIManager2->AddUserData("CashMallCash", &m_lCash, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataCash)
		return FALSE;

	m_pcsUserDataBuyProduct = m_pcsAgcmUIManager2->AddUserData("CashMallBuyProduct", m_pcsBuyItem, sizeof(AgpdItem *), 1, AGCDUI_USERDATA_TYPE_ITEM);
	if (!m_pcsUserDataBuyProduct)
		return FALSE;

	m_pcsUserDataMaxPage = m_pcsAgcmUIManager2->AddUserData("CashMallMaxPage", &m_lUserDataMaxPage, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataMaxPage)
		return FALSE;
	
	return TRUE;
}

BOOL AgcmUICashMall::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallProductName", 0, CBDisplayProductName, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallProductDesc", 0, CBDisplayProductDesc, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallProductPrice", 0, CBDisplayProductPrice, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallDisplayTabName", 0, CBDisplayTabName, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallDisplayPageNumber", 0, CBDisplayPageNumber, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallDisplayMaxPageNumber", 0, CBDisplayMaxPageNumber, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallDetailList", 0, CBDisplayBuyProductDetailList, AGCDUI_USERDATA_TYPE_ITEM))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallDetailValuePrice", 0, CBDisplayBuyProductDetailValuePrice, AGCDUI_USERDATA_TYPE_ITEM))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallDetailValueUseCash", 0, CBDisplayBuyProductDetailValueUseCash, AGCDUI_USERDATA_TYPE_ITEM))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallDetailValueRemainCash", 0, CBDisplayBuyProductDetailValueRemainCash, AGCDUI_USERDATA_TYPE_ITEM))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CashMallBuyMessage", 0, CBDisplayBuyMessage, AGCDUI_USERDATA_TYPE_ITEM))
		return FALSE;

	return TRUE;
}

BOOL AgcmUICashMall::AddEvent()
{
	m_lEventGetEditControl	= m_pcsAgcmUIManager2->AddEvent("CashMallGetCashEditControl");
	if (m_lEventGetEditControl < 0)
		return FALSE;
	m_lEventGetListControl	= m_pcsAgcmUIManager2->AddEvent("CashMallGetListControl");
	if (m_lEventGetListControl < 0)
		return FALSE;

	m_lEventOpenCashMallUI	= m_pcsAgcmUIManager2->AddEvent("CashMallOpenCashMallUI");
	if (m_lEventOpenCashMallUI < 0)
		return FALSE;
	m_lEventCloseCashMallUI	= m_pcsAgcmUIManager2->AddEvent("CashMallCloseCashMallUI");
	if (m_lEventCloseCashMallUI < 0)
		return FALSE;

	m_lEventNormalProduct	= m_pcsAgcmUIManager2->AddEvent("CashMallNormalProduct");
	if (m_lEventNormalProduct < 0)
		return FALSE;
	m_lEventNewProduct		= m_pcsAgcmUIManager2->AddEvent("CashMallNewProduct");
	if (m_lEventNewProduct < 0)
		return FALSE;
	m_lEventHotProduct		= m_pcsAgcmUIManager2->AddEvent("CashMallHotProduct");
	if (m_lEventHotProduct < 0)
		return FALSE;
	m_lEventEventProduct		= m_pcsAgcmUIManager2->AddEvent("CashMallEventProduct");
	if (m_lEventEventProduct < 0)
		return FALSE;
	m_lEventSaleProduct			= m_pcsAgcmUIManager2->AddEvent("CashMallSaleProduct");
	if( m_lEventSaleProduct < 0 )
		return FALSE;

	m_lEventSPCProduct			= m_pcsAgcmUIManager2->AddEvent( "CashMallSPCProduct" );
	if( m_lEventSPCProduct < 0 )
		return FALSE;
	m_lEventTPackProduct		= m_pcsAgcmUIManager2->AddEvent( "CashMallTPackProduct" );
	if( m_lEventTPackProduct < 0 )
		return FALSE;

	m_lEventSelectPageSlot[0]	= m_pcsAgcmUIManager2->AddEvent("CashMallSelectPage1");
	if (m_lEventSelectPageSlot[0] < 0)
		return FALSE;
	m_lEventSelectPageSlot[1]	= m_pcsAgcmUIManager2->AddEvent("CashMallSelectPage2");
	if (m_lEventSelectPageSlot[1] < 0)
		return FALSE;
	m_lEventSelectPageSlot[2]	= m_pcsAgcmUIManager2->AddEvent("CashMallSelectPage3");
	if (m_lEventSelectPageSlot[2] < 0)
		return FALSE;
	m_lEventSelectPageSlot[3]	= m_pcsAgcmUIManager2->AddEvent("CashMallSelectPage4");
	if (m_lEventSelectPageSlot[3] < 0)
		return FALSE;
	m_lEventSelectPageSlot[4]	= m_pcsAgcmUIManager2->AddEvent("CashMallSelectPage5");
	if (m_lEventSelectPageSlot[4] < 0)
		return FALSE;
	m_lEventSelectPageSlot[5]	= m_pcsAgcmUIManager2->AddEvent("CashMallSelectPage6");
	if (m_lEventSelectPageSlot[5] < 0)
		return FALSE;
	m_lEventSelectPageSlot[6]	= m_pcsAgcmUIManager2->AddEvent("CashMallSelectPage7");
	if (m_lEventSelectPageSlot[6] < 0)
		return FALSE;
	m_lEventSelectPageSlot[7]	= m_pcsAgcmUIManager2->AddEvent("CashMallSelectPage8");
	if (m_lEventSelectPageSlot[7] < 0)
		return FALSE;
	m_lEventSelectPageSlot[8]	= m_pcsAgcmUIManager2->AddEvent("CashMallSelectPage9");
	if (m_lEventSelectPageSlot[8] < 0)
		return FALSE;

	m_lEventBuyProductConfirm	= m_pcsAgcmUIManager2->AddEvent("CashMallBuyProductConfirm");
	if (m_lEventBuyProductConfirm < 0)
		return FALSE;

	m_lEventNotEnoughCash	= m_pcsAgcmUIManager2->AddEvent("CashMallNotEnoughMall");
	if (m_lEventNotEnoughCash < 0)
		return FALSE;

	m_lEventBuyResultSuccess	= m_pcsAgcmUIManager2->AddEvent("CashMallBuyResultSuccess");
	if (m_lEventBuyResultSuccess < 0)
		return FALSE;
	m_lEventBuyResultNotEnoughCash	= m_pcsAgcmUIManager2->AddEvent("CashMallBuyResultNotEnoughCash");
	if (m_lEventBuyResultNotEnoughCash < 0)
		return FALSE;
	m_lEventBuyResultInventoryFull	= m_pcsAgcmUIManager2->AddEvent("CashMallBuyResultInventoryFull");
	if (m_lEventBuyResultInventoryFull < 0)
		return FALSE;
	m_lEventBuyResultNeedNewItemList	= m_pcsAgcmUIManager2->AddEvent("CashMallBuyResultNeedNewItemList");
	if (m_lEventBuyResultNeedNewItemList < 0)
		return FALSE;
	m_lEventBuyResultPCBangOnly	= m_pcsAgcmUIManager2->AddEvent("CashMallBuyResultPCBangOnly");
	if (m_lEventBuyResultPCBangOnly < 0)
		return FALSE;
	m_lEventBuyResultLimitedCount = m_pcsAgcmUIManager2->AddEvent("CashMallBuyResultLimitedCount");
	if (m_lEventBuyResultLimitedCount < 0)
		return FALSE;

	m_lEventBuyResultNotTPCBang = m_pcsAgcmUIManager2->AddEvent("CashMallBuyResultNotTPCBang");
	if (m_lEventBuyResultNotTPCBang < 0)
		return FALSE;

	m_lEventBuyResultNotSPCBang = m_pcsAgcmUIManager2->AddEvent("CashMallBuyResultNotSPCBang");
	if (m_lEventBuyResultNotSPCBang < 0)
		return FALSE;

	m_lEventOpenRefreshCashUI	= m_pcsAgcmUIManager2->AddEvent("CashMallOpenRefreshCashUI", CBReturnRefresh, this);
	if (m_lEventOpenRefreshCashUI < 0)
		return FALSE;

#ifdef _AREA_KOREA_
	//if( !m_WebzenShop.OnWebzenShopInitialize() ) return FALSE;
	m_WebzenShop.OnWebzenShopInitialize();
#endif

	return TRUE;
}

BOOL AgcmUICashMall::SetItemGrid(INT32 lTab, INT32 lStartPage, INT32 lSelectPage)
{
	if (lTab < 0 || lTab >= m_pcsAgpmCashMall->GetCashMallInfo()->m_lNumTab ||
		lStartPage < 0 || lSelectPage < 0)
		return FALSE;

	if (m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo == NULL)
		return TRUE;

	if (m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[lTab].m_lNumItem <= (lSelectPage - 1) * AGCMUICASHMALL_MAX_DISPLAY_COUNT)
		return FALSE;

	int i;
	for (i = 0; i < AGCMUICASHMALL_MAX_DISPLAY_COUNT; ++i)
	{
		m_pcsAgpmGrid->Reset(&m_acsItemGrid[i]);
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventNormalProduct, i, TRUE);
	}

	m_lCurrentPageItemCount	= 0;

	for (i = 0; i < AGCMUICASHMALL_MAX_DISPLAY_COUNT; ++i)
	{
		if ((lSelectPage - 1) * AGCMUICASHMALL_MAX_DISPLAY_COUNT + i >= m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[lTab].m_lNumItem)
			break;

		pAgpmCashItemInfo	pCashItemInfo	= m_pcsAgpmCashMall->GetCashItem(m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[lTab].m_plTabProductList[(lSelectPage - 1) * AGCMUICASHMALL_MAX_DISPLAY_COUNT + i]);

		if (!pCashItemInfo || !pCashItemInfo->m_pcsItem)
			break;

		m_pcsAgpmGrid->Reset(&m_acsItemGrid[i]);
		m_pcsAgpmGrid->Add(&m_acsItemGrid[i], 0, 0, 0, pCashItemInfo->m_pcsItem->m_pcsGridItem, 1, 1);

		++m_lCurrentPageItemCount;
	}

	m_lCurrentTab	= lTab;
	m_lStartPage	= lStartPage;
	m_lCurrentPage	= lSelectPage;

	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUserDataCurrentPage, TRUE);

	INT32	lBaseSlotPos	= AGCMUICASHMALL_MAX_DISPLAY_PAGE_SLOT / 2 + 1;
    INT32	lMaxPage		= (m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[m_lCurrentTab].m_lNumItem - 1) / AGCMUICASHMALL_MAX_DISPLAY_COUNT + 1;
	m_lMaxPage = lMaxPage;

	lMaxPage = min( lMaxPage, 5 );
	INT32	lStartSlotIndex	= lBaseSlotPos - (lMaxPage - 1);

	SetSelectPageSlot(lStartSlotIndex + (m_lCurrentPage - m_lStartPage) * 2);

//	if (m_pcsListControl)
//		m_pcsListControl->m_lVisibleListItemRow	= m_lCurrentPageItemCount;

	m_pcsUserDataItemGrid->m_stUserData.m_lCount	= m_lCurrentPageItemCount;
	m_pcsUserDataItemGrid->m_bUpdateList		= TRUE;

	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUserDataMaxPage, TRUE);
	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUserDataItemGrid, TRUE);

	for (i = 0; i < AGCMUICASHMALL_MAX_DISPLAY_COUNT; ++i)
	{
		if ((lSelectPage - 1) * AGCMUICASHMALL_MAX_DISPLAY_COUNT + i >= m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[lTab].m_lNumItem)
			break;

		pAgpmCashItemInfo	pCashItemInfo	= m_pcsAgpmCashMall->GetCashItem(m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[lTab].m_plTabProductList[(lSelectPage - 1) * AGCMUICASHMALL_MAX_DISPLAY_COUNT + i]);

		if (!pCashItemInfo || !pCashItemInfo->m_pcsItem)
			break;

		if (pCashItemInfo->m_lSpecialFlag == AGPMCASH_SPECIALFLAG_NEW) 
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventNewProduct, i, TRUE);
		else if (pCashItemInfo->m_lSpecialFlag == AGPMCASH_SPECIALFLAG_HOT)
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventHotProduct, i, TRUE);
		else if (pCashItemInfo->m_lSpecialFlag == AGPMCASH_SPECIALFLAG_EVENT)
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventEventProduct, i, TRUE);
		else if (pCashItemInfo->m_lSpecialFlag == AGPMCASH_SPECIALFLAG_SALE)
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSaleProduct, i , TRUE );
		else if (pCashItemInfo->m_lSpecialFlag == AGPMCASH_SPECIALFLAG_SPC)
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSPCProduct , i , TRUE );
		else if (pCashItemInfo->m_lSpecialFlag == AGPMCASH_SPECIALFLAG_TPACK)
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventTPackProduct , i , TRUE );

	}

	return TRUE;
}

BOOL AgcmUICashMall::CBUpdateMallList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;
	INT32			lTab	= pCustData ? *static_cast<INT32*>(pCustData) : 0;

	pThis->m_bReceiveMallInfo	= TRUE;

	pThis->SetItemGrid(lTab, 1, 1);

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenCashMallUI);
}

BOOL AgcmUICashMall::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGetEditControl);
}

BOOL AgcmUICashMall::CBResponseBuyResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUICashMall	*pThis		= (AgcmUICashMall *)	pClass;
	UINT8			*pcResult	= (UINT8 *)				pData;

	switch (*pcResult) {
		case AGPMCASH_BUY_RESULT_SUCCESS:
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyResultSuccess);
			}
			break;

		case AGPMCASH_BUY_RESULT_NOT_ENOUGH_CASH:
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyResultNotEnoughCash);
			}
			break;

		case AGPMCASH_BUY_RESULT_INVENTORY_FULL:
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyResultInventoryFull);
			}
			break;

		case AGPMCASH_BUY_RESULT_NEED_NEW_ITEM_LIST:
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyResultNeedNewItemList);
				pThis->m_pcsAgcmCashMall->SendPacketRequestMallList(pThis->m_lCurrentTab);
			}
			break;

		case AGPMCASH_BUY_RESULT_PCBANG_ONLY:
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyResultPCBangOnly);
			}
			break;

		case AGPMCASH_BUY_RESULT_LIMITED_COUNT:
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyResultLimitedCount);
			}
			break;

		case AGPMCASH_BUY_RESULT_NOT_T_PCBANG :
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyResultNotTPCBang);
			}
			break;

		case AGPMCASH_BUY_RESULT_NOT_S_PCBANG :
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyResultNotSPCBang);
			}
			break;
	}

	return TRUE;
}

BOOL AgcmUICashMall::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	pThis->m_bIsOpenedUI	= FALSE;

	return TRUE;
}

BOOL AgcmUICashMall::CBUpdateCash(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (!pThis->m_pcsAgcmCharacter->GetSelfCharacter() ||
		pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	return pThis->RefreshCashEdit();
}

BOOL AgcmUICashMall::RefreshCashEdit()
{
	if (!m_pcsEditCash)
		return FALSE;

	ApString<128>	szBuffer;
	
	szBuffer.Format("%s : %d", m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CASHMALL_KEEP_CASH), m_pcsAgpmCharacter->GetCash(m_pcsAgcmCharacter->GetSelfCharacter()));

	return m_pcsEditCash->SetText(szBuffer.GetBuffer());
}

BOOL AgcmUICashMall::CBGetEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	pThis->m_pcsEditCash			= (AcUIEdit *) ((AgcdUIControl *)	pData1)->m_pcsBase;

	return pThis->RefreshCashEdit();
}

BOOL AgcmUICashMall::CBGetListControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	pThis->m_pcsListControl			= (AcUIList *) ((AgcdUIControl *)	pData1)->m_pcsBase;

	return TRUE;
}

BOOL AgcmUICashMall::CBCashCharge(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis		= (AgcmUICashMall *)	pClass;

	::ShowWindow( g_pEngine->GethWnd() , SW_MINIMIZE );

	// 여기서 충전하는 웹 페이지를 띄워준다.
	::ShellExecute( NULL, "open", pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CASHMALL_CHARGE_URL), NULL, NULL, SW_SHOW );

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenRefreshCashUI);
}

BOOL AgcmUICashMall::CBCheckCash(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis		= (AgcmUICashMall *)	pClass;

	return pThis->m_pcsAgcmCashMall->SendPacketRefreshCash();
}

BOOL AgcmUICashMall::CBBuyProductConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICashMall	*pThis		= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return FALSE;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog(pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CASHMALL_NOT_COMMIT_ON_DEAD));
		return FALSE;
	}

	// 캐쉬 아이템 구입 개수 제한.
	if (AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter())
	{
		if (AgpdGrid* pCashInven = pThis->m_pcsAgpmItem->GetCashInventoryGrid( pcsCharacter ))
		{
			if ( pCashInven->m_lItemCount >= pCashInven->m_lGridCount )
			{
				pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog(ClientStr().GetStr(STI_CASHITEM_MAX_COUNT));
				return FALSE;
			}
		}
	}

	pThis->m_pcsBuyItem	= pThis->m_pcsAgpmItem->GetItem(pThis->m_pcsAgpmGrid->GetItem(&pThis->m_acsItemGrid[pcsSourceControl->m_lUserDataIndex], 0, 0, 0));

	pThis->m_pcsAgcmUIManager2->RefreshUserData(pThis->m_pcsUserDataBuyProduct, TRUE);

	if (pThis->m_pcsBuyItem)
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyProductConfirm);

	return TRUE;
}

BOOL AgcmUICashMall::CBBuyProduct(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICashMall	*pThis		= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsBuyItem)
		return FALSE;

	// 죽은 상태에서는 아이템을 구입할 수 없다.
	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog(pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CASHMALL_NOT_COMMIT_ON_DEAD));
		return FALSE;
	}

	// 캐쉬 아이템 구입 개수 제한.
	if (AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter())
	{
		if (AgpdGrid* pCashInven = pThis->m_pcsAgpmItem->GetCashInventoryGrid( pcsCharacter ))
		{
			if ( pCashInven->m_lItemCount >= pCashInven->m_lGridCount )
			{
				pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog(ClientStr().GetStr(STI_CASHITEM_MAX_COUNT));
				return FALSE;
			}
		}
	}

	return pThis->m_pcsAgcmCashMall->SendPacketBuyProduct(pThis->m_pcsBuyItem->m_ulCID);
}

BOOL AgcmUICashMall::CBOpenCashMallUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis		= (AgcmUICashMall *)	pClass;

#ifdef _AREA_KOREA_
	return pThis->m_WebzenShop.OnWebzenShopShow();
#endif

	if (pThis->m_bIsOpenedUI)
	{
		pThis->m_bIsOpenedUI	= FALSE;

		return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseCashMallUI);
	}
	else
	{
		pThis->m_bIsOpenedUI	= TRUE;

		// 탭에 대한 내용을 매번 요청한다.
		pThis->m_pcsAgcmCashMall->SendPacketRequestMallList(pThis->m_lCurrentTab);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGetListControl);
		return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenCashMallUI);
	}

	return TRUE;
}

BOOL AgcmUICashMall::CBSelectTab(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	//return pThis->SetItemGrid(pcsSourceControl->m_lUserDataIndex, 1, 1);

	// 2007.08.16. steeple
	// 패킷 요청을 한다.
	return pThis->m_pcsAgcmCashMall->SendPacketRequestMallList(pcsSourceControl->m_lUserDataIndex);
}

BOOL AgcmUICashMall::CBPageLeft(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	return pThis->SetItemGrid(pThis->m_lCurrentTab,
							  (pThis->m_lStartPage == 1 || pThis->m_lCurrentPage - 1 >= pThis->m_lStartPage) ? pThis->m_lStartPage : pThis->m_lStartPage - 1,
							  (pThis->m_lCurrentPage == 1) ? pThis->m_lCurrentPage : pThis->m_lCurrentPage - 1);
}

BOOL AgcmUICashMall::CBPageRight(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo == NULL)
		return TRUE;

	if (pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[pThis->m_lCurrentTab].m_lNumItem <= 0)
		return FALSE;

	INT32	lMaxPage	= (pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[pThis->m_lCurrentTab].m_lNumItem - 1) / AGCMUICASHMALL_MAX_DISPLAY_COUNT + 1;

	return pThis->SetItemGrid(pThis->m_lCurrentTab,
							  (pThis->m_lCurrentPage == lMaxPage || pThis->m_lCurrentPage <= pThis->m_lStartPage + 3) ? pThis->m_lStartPage : pThis->m_lStartPage + 1,
							  (pThis->m_lCurrentPage == lMaxPage) ? pThis->m_lCurrentPage : pThis->m_lCurrentPage + 1);
}

BOOL AgcmUICashMall::CBGoPage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pcsSourceControl->m_pcsBase->m_szStaticString ||
		_tcslen(pcsSourceControl->m_pcsBase->m_szStaticString) == 0)
		return TRUE;

	return pThis->SetItemGrid(pThis->m_lCurrentTab,
							  pThis->m_lStartPage,
							  _ttoi(pcsSourceControl->m_pcsBase->m_szStaticString));
}

BOOL AgcmUICashMall::CBIsShowTabButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsAgpmCashMall->GetCashMallInfo())
		return FALSE;

	if (pcsSourceControl->m_lUserDataIndex < 0 ||
		pcsSourceControl->m_lUserDataIndex >= pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_lNumTab)
		return FALSE;

	return TRUE;
}

BOOL AgcmUICashMall::CBIsShowChargeButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	return TRUE;
}

BOOL AgcmUICashMall::CBIsActivePage(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo)
		return TRUE;

	INT32	lMaxPage	= (pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[pThis->m_lCurrentTab].m_lNumItem - 1) / AGCMUICASHMALL_MAX_DISPLAY_COUNT + 1;

	if (pThis->m_lStartPage + pcsSourceControl->m_lUserDataIndex > lMaxPage)
		return FALSE;

	return TRUE;
}

BOOL AgcmUICashMall::CBIsActiveBuy(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsBuyItem)
		return FALSE;

	INT64	llCash			= pThis->m_pcsAgpmCharacter->GetCash(pThis->m_pcsAgcmCharacter->GetSelfCharacter());
	INT64	llPrice			= pThis->m_pcsAgpmItem->GetItemPrice(pThis->m_pcsBuyItem);

	if (llPrice > llCash)
		return FALSE;

	return TRUE;
}

BOOL AgcmUICashMall::CBDisplayProductName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData || !pcsSourceControl)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;
	AgpdGrid		*pcsGrid	= (AgpdGrid *)	pData;

	((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(_T(""));

	AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	AgpdItem	*pcsItem	= pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	TCHAR	szBuffer[128];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	if (pcsItem->m_nCount > 0)
		_stprintf(szBuffer, "%s (%d%s)", pcsItem->m_pcsItemTemplate->m_szName,
												  pcsItem->m_nCount,
												  pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CASHMALL_UNIT));
	else
		_stprintf(szBuffer, "%s", pcsItem->m_pcsItemTemplate->m_szName);

	((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(szBuffer);

	/*
	if (pcsItem->m_nCount > 0)
		_stprintf(szDisplay, "%s (%d%s)", pcsItem->m_pcsItemTemplate->m_szName,
												  pcsItem->m_nCount,
												  pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CASHMALL_UNIT));
	else
		_stprintf(szDisplay, "%s", pcsItem->m_pcsItemTemplate->m_szName);
	*/

	((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetTextStartLine( 0 );

	return TRUE;
}

BOOL AgcmUICashMall::CBDisplayProductDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData || !pcsSourceControl)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;
	AgpdGrid		*pcsGrid	= (AgpdGrid *)	pData;

	((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(_T(""));

	AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	AgpdItem	*pcsItem	= pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem)
		return FALSE;

	pAgpmCashItemInfo	pCashItemInfo	= pThis->m_pcsAgpmCashMall->GetCashItem(pcsItem->m_ulCID);
	if (!pCashItemInfo)
		return FALSE;

	((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(pCashItemInfo->m_szDescription.GetBuffer());
	((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetTextStartLine( 0 );

	//_stprintf(szDisplay, "%s", pCashItemInfo->m_szDescription.GetBuffer());

	return TRUE;
}

BOOL AgcmUICashMall::CBDisplayProductPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData || !pcsSourceControl)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;
	AgpdGrid		*pcsGrid	= (AgpdGrid *)	pData;

	((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(_T(""));

	AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem)
		return FALSE;

	AgpdItem	*pcsItem	= pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem)
		return FALSE;

	pAgpmCashItemInfo	pCashItemInfo	= pThis->m_pcsAgpmCashMall->GetCashItem(pcsItem->m_ulCID);
	if (!pCashItemInfo)
		return FALSE;

	/*
	TCHAR	szBuffer[32];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	_stprintf(szBuffer, "%d", pCashItemInfo->llPrice);

	((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(szBuffer);
	*/

	_stprintf(szDisplay, "%d", pCashItemInfo->m_llPrice);

	return TRUE;
}

BOOL AgcmUICashMall::CBDisplayTabName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (pcsSourceControl->m_lUserDataIndex < 0 ||
		pcsSourceControl->m_lUserDataIndex >= pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_lNumTab)
		return TRUE;

	if (pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo == NULL)
		return TRUE;

	_stprintf(szDisplay, "%s", pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[pcsSourceControl->m_lUserDataIndex].m_szTabName.GetBuffer());

	return TRUE;
}

BOOL AgcmUICashMall::CBDisplayPageNumber(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo)
		return TRUE;

	if (pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[pThis->m_lCurrentTab].m_lNumItem <= 0)
		return FALSE;

	INT32	lBaseSlotPos	= AGCMUICASHMALL_MAX_DISPLAY_PAGE_SLOT / 2 + 1;

    INT32	lMaxPage		= (pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[pThis->m_lCurrentTab].m_lNumItem - 1) / AGCMUICASHMALL_MAX_DISPLAY_COUNT + 1;

	if (lMaxPage > 5)
		lMaxPage	= 5;

	INT32	lStartSlotIndex	= lBaseSlotPos - (lMaxPage - 1);

	if (pcsSourceControl->m_lUserDataIndex < lStartSlotIndex ||
		pcsSourceControl->m_lUserDataIndex > lStartSlotIndex + (lMaxPage - 1) * 2)
		return TRUE;

	if ((pcsSourceControl->m_lUserDataIndex - lStartSlotIndex) % 2)
		return TRUE;

	_stprintf(szDisplay, "%d", pThis->m_lStartPage + (pcsSourceControl->m_lUserDataIndex - lStartSlotIndex) / 2);

	return TRUE;
}

BOOL AgcmUICashMall::CBDisplayMaxPageNumber(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo)
		return TRUE;

	if (pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_pstMallTabInfo[pThis->m_lCurrentTab].m_lNumItem <= 0)
		return FALSE;

	_stprintf(szDisplay, "%d / %d", pThis->m_lCurrentPage, pThis->m_lMaxPage );

	return TRUE;
}

BOOL AgcmUICashMall::CBDisplayBuyProductDetailList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsBuyItem)
		return FALSE;

	((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetLineDelimiter(_T("^"));

	ApString<1024>	szBufferString;

	szBufferString.AppendFormat("%s", pThis->m_pcsBuyItem->m_pcsItemTemplate->m_szName);

	return ((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(szBufferString.GetBuffer());
}

BOOL AgcmUICashMall::CBDisplayBuyProductDetailValuePrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsBuyItem)
		return FALSE;

	_stprintf(szDisplay, "%I64d", pThis->m_pcsAgpmItem->GetItemPrice(pThis->m_pcsBuyItem));

	return TRUE;
}

BOOL AgcmUICashMall::CBDisplayBuyProductDetailValueUseCash(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsBuyItem)
		return FALSE;

	INT64	llCash			= pThis->m_pcsAgpmCharacter->GetCash(pThis->m_pcsAgcmCharacter->GetSelfCharacter());
	INT64	llPrice			= pThis->m_pcsAgpmItem->GetItemPrice(pThis->m_pcsBuyItem);

	//INT64	llUseCash		= llPrice;
	INT64	llUseCash		= (llPrice > llCash) ? llCash : llPrice;

	_stprintf(szDisplay, "%I64d", llUseCash);

	return TRUE;
}

BOOL AgcmUICashMall::CBDisplayBuyProductDetailValueRemainCash(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsBuyItem)
		return FALSE;

	INT64	llCash			= pThis->m_pcsAgpmCharacter->GetCash(pThis->m_pcsAgcmCharacter->GetSelfCharacter());
	INT64	llPrice			= pThis->m_pcsAgpmItem->GetItemPrice(pThis->m_pcsBuyItem);

	//INT64	llUseCash		= llPrice;
	INT64	llUseCash		= (llPrice > llCash) ? llCash : llPrice;
	INT64	llRemainCash	= llCash - llUseCash;

	_stprintf(szDisplay, "%I64d", llRemainCash);

	if (llPrice > llCash)
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotEnoughCash);

	return TRUE;
}

BOOL AgcmUICashMall::CBDisplayBuyMessage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	if (!pThis->m_pcsBuyItem)
		return FALSE;

	pAgpmCashItemInfo	pCashItemInfo	= pThis->m_pcsAgpmCashMall->GetCashItem(pThis->m_pcsBuyItem->m_ulCID);
	if (!pCashItemInfo)
		return FALSE;

	_stprintf(szDisplay, "%s", pCashItemInfo->m_szBuyMessage.GetBuffer());

	return TRUE;
}

BOOL AgcmUICashMall::SetSelectPageSlot(INT32 lSelectPage)
{
	for (int i = 1; i <= AGCMUICASHMALL_MAX_DISPLAY_PAGE_SLOT; ++i)
	{
		if (i == lSelectPage)
		{
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventSelectPageSlot[i - 1], TRUE);
			break;
		}
	}

	return TRUE;
}

BOOL AgcmUICashMall::CBReturnRefresh(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass)
		return FALSE;

	AgcmUICashMall	*pThis	= (AgcmUICashMall *)	pClass;

	return pThis->m_pcsAgcmCashMall->SendPacketRefreshCash();
}