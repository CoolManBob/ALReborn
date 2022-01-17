/*===========================================================================

	AgcmUIGuildWarehouse.cpp

===========================================================================*/


#include "AgcmUIGuildWarehouse.h"
#include "AuStrTable.h"
#include "AgcmUIEventBank.h"


/****************************************************************/
/*		The Implementation of AgcmUIGuildWarehouse class		*/
/****************************************************************/
//
//	Constants
//=================================
//
CHAR AgcmUIGuildWarehouse::s_szEvent[AGCMUIGUILDWAREHOUSE_EVENT_MAX][30] = 
	{
	"GWARE_OPEN",
	"GWARE_CLOSE"
	};


CHAR AgcmUIGuildWarehouse::s_szMessage[AGCMUIGUILDWAREHOUSE_MESSAGE_MAX][30] = 
	{
	"GWARE_CAPTION",
	"GWARE_NOT_ENOUGH_MONEY",	// AGCMUIGUILDWAREHOUSE_MESSAGE_NOT_ENOUGH_MONEY
	"GWARE_EXPAND_CONFIRM"		// AGCMUIGUILDWAREHOUSE_MESSAGE_EXPAND_CONFIRM
	};




AgcmUIGuildWarehouse::AgcmUIGuildWarehouse()
	{
	SetModuleName("AgcmUIGuildWarehouse");

	m_pWarehouseGrid = NULL;
	m_lWarehouseLayerIndex = 0;
	m_llWarehouseMoney = 0;
	ZeroMemory(&m_stOpenPos, sizeof(AuPOS));
	m_bIsOpen = FALSE;
	m_bMoneyInput = TRUE;
	m_bSetGrid = FALSE;
	}


AgcmUIGuildWarehouse::~AgcmUIGuildWarehouse()
	{
	}




//	ApModule inherited
//===============================================
//
BOOL AgcmUIGuildWarehouse::OnAddModule()
	{
	m_pAgpmFactors		= (AgpmFactors *) GetModule("AgpmFactors");
	m_pAgpmGrid			= (AgpmGrid *) GetModule("AgpmGrid");	
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmItem			= (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmGuild		= (AgpmGuild *) GetModule("AgpmGuild");
	m_pAgpmGuildWarehouse	= (AgpmGuildWarehouse *) GetModule("AgpmGuildWarehouse");
	m_pAgpmSiegeWar		= (AgpmSiegeWar *) GetModule("AgpmSiegeWar");
	m_pAgcmCharacter	= (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pAgcmEventGuild	= (AgcmEventGuild *) GetModule("AgcmEventGuild");
	m_pAgcmItem			= (AgcmItem *) GetModule("AgcmItem");
	m_pAgcmUIManager2	= (AgcmUIManager2 *) GetModule("AgcmUIManager2");
	m_pAgcmUIControl	= (AgcmUIControl *) GetModule("AgcmUIControl");
	m_pAgcmUIMain		= (AgcmUIMain *) GetModule("AgcmUIMain");
	m_pAgcmUIItem		= (AgcmUIItem *) GetModule("AgcmUIItem");

	if (!m_pAgpmFactors ||!m_pAgpmGrid || !m_pAgpmCharacter ||
		!m_pAgpmItem || !m_pAgpmGuild || !m_pAgpmGuildWarehouse || !m_pAgpmSiegeWar ||
		!m_pAgcmCharacter || !m_pAgcmEventGuild || !m_pAgcmItem ||
		!m_pAgcmUIManager2 || !m_pAgcmUIControl || !m_pAgcmUIMain || !m_pAgcmUIItem
		)
		return FALSE;
	
	// event grant callback
	if (!m_pAgcmEventGuild->SetCallbackGrantWarehouse(CBGrant, this))
		return FALSE;

	if (!m_pAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;

	if (!m_pAgcmCharacter->SetCallbackReleaseSelfCharacter(CBReleaseSelfCharacter, this))
		return FALSE;
	
	if (!m_pAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	if (!m_pAgpmItem->SetCallbackAddItemToGuildWarehouse(CBAddItemToGuildWarehouse, this))
		return FALSE;
	
	if (!m_pAgpmItem->SetCallbackRemoveItemFromGuildWarehouse(CBRemoveItemFromGuildWarehouse, this))
		return FALSE;
		
	if (!m_pAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	if (!m_pAgpmGuildWarehouse->SetCallbackBaseInfo(CBBaseInfo, this))		// money, slot
		return FALSE;

	if (!m_pAgpmGuildWarehouse->SetCallbackOpen(CBOpenResult, this))
		return FALSE;
	
	if (!m_pAgpmGuildWarehouse->SetCallbackExpand(CBExpandResult, this))
		return FALSE;

	if (!m_pAgcmUIItem->SetCallbackMovedFromGuildWarehouse(CBItemMovedToInventory, this))
		return FALSE;

	if (!AddEvent() || !AddFunction() || !AddDisplay() || !AddUserData() || !AddBoolean())
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::OnInit()
	{
  #ifdef _DEBUG
	AS_REGISTER_TYPE_BEGIN(AgcmUIGuildWarehouse, AgcmUIGuildWarehouse);
		AS_REGISTER_METHOD0(void, TestOpen);
	AS_REGISTER_TYPE_END;
  #endif
  
	return TRUE;
	}




//	Operations
//========================================================
//
#ifdef _DEBUG

void AgcmUIGuildWarehouse::TestOpen()
	{
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	if (pAgpdCharacter)
		{
		m_stOpenPos	= pAgpdCharacter->m_stPos;
		SendOpen(pAgpdCharacter);
		}
	}

#endif


BOOL AgcmUIGuildWarehouse::Open()
	{
	// open window
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIGUILDWAREHOUSE_EVENT_OPEN]);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstWarehouseGrid);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstWarehouseLayerIndex);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstActiveButton);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstDummy);
	m_bIsOpen = TRUE;

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::SetGrid(AgpdCharacter *pAgpdCharacter)
	{
	if (m_bSetGrid)
		return TRUE;
	
	AgpdGuildWarehouse *pAgpdGuildWarehouse = m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	m_pWarehouseGrid = &pAgpdGuildWarehouse->m_WarehouseGrid;
	m_llWarehouseMoney = pAgpdGuildWarehouse->m_llMoney;

	m_pstWarehouseGrid->m_stUserData.m_pvData = m_pWarehouseGrid;
	m_pstWarehouseMoney->m_stUserData.m_pvData = &m_llWarehouseMoney;
	
	m_bSetGrid = TRUE;
	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::OnItemMovedIn(AgpdItem *pAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
	{
	// it must be in inventory, sub inventory
	if (NULL == m_pWarehouseGrid
		|| ((AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus) && (AGPDITEM_STATUS_SUB_INVENTORY != pAgpdItem->m_eStatus)))
		return FALSE;

	if(pAgpdItem->m_lExpireTime != 0)
		return FALSE;

	// is empty grid position?
	if (!m_pAgpmGrid->IsEmptyGrid(m_pWarehouseGrid, nLayer, nRow, nColumn, 1, 1))
		{
		return FALSE;
		}
	
	// bound type check
	if (m_pAgpmItem->GetBoundType(pAgpdItem) != E_AGPMITEM_NOT_BOUND)
		return FALSE;
	
	// send request
	return SendItemIn(m_pAgcmCharacter->GetSelfCharacter(), pAgpdItem, nLayer, nRow, nColumn);
	}


BOOL AgcmUIGuildWarehouse::OnItemMovedOut(AgpdItem *pAgpdItem, INT16 nStatus, INT16 nLayer, INT16 nRow, INT16 nColumn)
	{
	// it must be in inventory
	if (AGPDITEM_STATUS_GUILD_WAREHOUSE != pAgpdItem->m_eStatus)
		return FALSE;
	
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	if (!pAgpdItemADChar)
		return FALSE;

	if(nStatus == AGPDITEM_STATUS_INVENTORY)
	{
		// inventory full check
		// is empty grid position
		if(m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid) ||
			!m_pAgpmGrid->IsEmptyGrid(&pAgpdItemADChar->m_csInventoryGrid, nLayer, nRow, nColumn, 1, 1))
			return FALSE;
	}
	else if(nStatus == AGPDITEM_STATUS_SUB_INVENTORY)
	{
		// sub inventory full check
		// is empty grid position
		if(m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csSubInventoryGrid) ||
			!m_pAgpmGrid->IsEmptyGrid(&pAgpdItemADChar->m_csSubInventoryGrid, nLayer, nRow, nColumn, 1, 1))
			return FALSE;
	}
	else
		return FALSE;
	
	// send request
	return SendItemOut(pAgpdCharacter, pAgpdItem, nStatus, nLayer, nRow, nColumn);
	}




//	Module Callbacks
//===========================================
//
BOOL AgcmUIGuildWarehouse::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *)(pClass);
	AgpdCharacter			*pAgpdCharacter	= (AgpdCharacter *)(pData);

	pThis->SetGrid(pAgpdCharacter);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWarehouseGrid);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWarehouseMoney);

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *)(pClass);
	AgpdCharacter			*pAgpdCharacter	= (AgpdCharacter *)(pData);
	
	pThis->m_pWarehouseGrid = NULL;
	pThis->m_pstWarehouseGrid->m_stUserData.m_pvData = NULL;
	pThis->m_bSetGrid = FALSE;
	
	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBBaseInfo(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuildWarehouse *pThis = (AgcmUIGuildWarehouse *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdGuildWarehouse *pAgpdGuildWarehouse = (AgpdGuildWarehouse *) pCustData;	
	
	if (pThis->m_pAgcmCharacter->GetSelfCharacter() == pAgpdCharacter)
		{
		pThis->SetGrid(pAgpdCharacter);
		pThis->m_llWarehouseMoney = pAgpdGuildWarehouse->m_llMoney;
		pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWarehouseMoney);
		}
	
	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBOpenResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	INT32 lResult = *((INT32 *) pCustData);
	
	if (AGPMGUILDWAREHOUSE_RESULT_LOAD_ENDED == lResult)
		pThis->Open();
	
	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBExpandResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	INT32 lResult = *((INT32 *) pCustData);

	AgpdCharacter *pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (pAgpdCharacter != pAgpdCharacterSelf)
		return FALSE;
	
	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;

	switch (lResult)
		{
		case AGPMGUILDWAREHOUSE_RESULT_SUCCESS :
			pAgpdGuildWarehouse->m_lSlot++;
			pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWarehouseLayerIndex);
			break;
		
		case AGPMGUILDWAREHOUSE_RESULT_NOT_ENOUGH_MONEY :
			{
			CHAR *psz = pThis->GetMessageTxt(AGCMUIGUILDWAREHOUSE_MESSAGE_NOT_ENOUGH_MONEY);
			if (psz)
				pThis->m_pAgcmUIManager2->ActionMessageOKDialog(psz);
			}
			break;	
		}

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBItemMovedToInventory(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	AgpdItem *pAgpdItem = (AgpdItem *) pData;
	PVOID *ppvBuffer = (PVOID *) pCustData;
	if (!ppvBuffer[0] || !ppvBuffer[1] || !ppvBuffer[2] || !ppvBuffer[3])
		return FALSE;
	
	INT32 lLayer = *((INT32 *) ppvBuffer[0]);
	INT32 lRow = *((INT32 *) ppvBuffer[1]);
	INT32 lColumn = *((INT32 *) ppvBuffer[2]);
	INT32 lStatus = *((INT32 *) ppvBuffer[3]);
	
	return pThis->OnItemMovedOut(pAgpdItem, lStatus, lLayer, lRow, lColumn);
	}


BOOL AgcmUIGuildWarehouse::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;

	if (!pThis->m_bIsOpen)
		return TRUE;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pAgpdCharacter->m_stPos, pThis->m_stOpenPos);
	if ((INT32) fDistance < 150)
		return TRUE;

	pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEvent[AGCMUIGUILDWAREHOUSE_EVENT_CLOSE]);
	
	// 
	pThis->SendClose(pAgpdCharacter);
	
	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBAddItemToGuildWarehouse(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIGuildWarehouse *pThis = (AgcmUIGuildWarehouse *) pClass;
	AgpdItem *pAgpdItem = (AgpdItem *) pData;
	PVOID *ppvBuffer = (PVOID *) pCustData;
	if (!ppvBuffer[0] || !ppvBuffer[1] || !ppvBuffer[2])
		return FALSE;
	
	INT16 lLayer = *((INT16 *) ppvBuffer[0]);
	INT16 lRow = *((INT16 *) ppvBuffer[1]);
	INT16 lColumn = *((INT16 *) ppvBuffer[2]);	

	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pThis->m_pAgcmCharacter->GetSelfCharacter());
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	if (!pThis->m_pAgpmGuildWarehouse->AddItem(pAgpdGuildWarehouse, pAgpdItem, lLayer, lRow, lColumn))
		return FALSE;

	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWarehouseGrid);

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBRemoveItemFromGuildWarehouse(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	AgpdItem *pAgpdItem = (AgpdItem *) pData;

	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pThis->m_pAgcmCharacter->GetSelfCharacter());
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	if (!pThis->m_pAgpmGuildWarehouse->RemoveItem(pAgpdGuildWarehouse, pAgpdItem))
		return FALSE;

	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWarehouseGrid);

	return TRUE;
	}




//	Event callback
//==========================================================
//
BOOL AgcmUIGuildWarehouse::CBGrant(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	AgpdCharacter			*pAgpdCharacter	= (AgpdCharacter *)	pData;
	AgpdCharacter			*pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	
	if (pAgpdCharacter != pAgpdCharacterSelf)
		return FALSE;

	// if in castle, only member of owner guild can use warehouse
	AgpdSiegeWar *pAgpdSiegeWar = pThis->m_pAgpmSiegeWar->GetSiegeWarInfo(pAgpdCharacter);
	if (pAgpdSiegeWar)
		{
		AgpdGuild *pAgpdGuild = pThis->m_pAgpmGuild->GetGuild(pAgpdCharacter);
		if (!pAgpdGuild
			|| COMPARE_EQUAL != pAgpdSiegeWar->m_strOwnerGuildName.Compare(pAgpdGuild->m_szID))
			return FALSE;
		}

	pThis->m_stOpenPos	= pAgpdCharacter->m_stPos;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "UI_GuildWarehouse" );
	if( !pcdUI ) return FALSE;

	// 이미 창이 열려 있으면 넘어간다.
	if( pcdUI->m_eStatus >= AGCDUI_STATUS_CLOSING ) return TRUE;

	// before request, remove all gabage items in warehouse
	pThis->m_pAgpmGuildWarehouse->ClearAllItems(pAgpdCharacter);
	
	pThis->SendOpen(pAgpdCharacter);

	return TRUE;
	}




//	System driven UI close callback
//==================================================
//
BOOL AgcmUIGuildWarehouse::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGuildWarehouse *pThis = (AgcmUIGuildWarehouse *) pClass;
	
	pThis->m_bIsOpen= FALSE;

	return TRUE;
	}




//	Function Callbacks
//=========================================================
//
BOOL AgcmUIGuildWarehouse::CBMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl ||
		!pcsSourceControl->m_pcsBase ||
		AcUIBase::TYPE_GRID != pcsSourceControl->m_lType
		)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis					= (AgcmUIGuildWarehouse *) pClass;
	AcUIGrid				*pAcUIGrid				= (AcUIGrid *) pcsSourceControl->m_pcsBase;
	AgpdGridSelectInfo		*pAgpdGridSelectInfo	= pAcUIGrid->GetDragDropMessageInfo();

	if (!pAgpdGridSelectInfo || !pAgpdGridSelectInfo->pGridItem ||
		 AGPDGRID_ITEM_TYPE_ITEM != pAgpdGridSelectInfo->pGridItem->m_eType)
		return FALSE;

	AgpdItem *pAgpdItem = pThis->m_pAgpmItem->GetItem(pAgpdGridSelectInfo->pGridItem->m_lItemID);
	if (!pAgpdItem)
		return FALSE;

	pThis->OnItemMovedIn(pAgpdItem, pAcUIGrid->m_lNowLayer, pAgpdGridSelectInfo->lGridRow, pAgpdGridSelectInfo->lGridColumn);

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBClickLayer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIGuildWarehouse *pThis = (AgcmUIGuildWarehouse*) pClass;
	AgcdUIControl *pGridControl = (AgcdUIControl *)	pData1;

	if (pGridControl)
		{
		AgcdUIUserData	*pstUserData = pThis->m_pAgcmUIManager2->GetControlUserData(pGridControl);
		if (pstUserData && pstUserData->m_lSelectedIndex != -1)
			((AcUIGrid *) pGridControl->m_pcsBase)->SetNowLayer(pstUserData->m_lSelectedIndex);
		}

	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWarehouseGrid);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWarehouseLayerIndex);

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBClickMoneyIn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	
	pThis->m_bMoneyInput = TRUE;
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWarehouseMoney);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstDummy);

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBClickMoneyOut(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	
	pThis->m_bMoneyInput = FALSE;
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWarehouseMoney);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstDummy);

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBClickMoneyConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	AgcdUIControl			*pControlEdit = (AgcdUIControl *) pData1;

	if (AcUIBase::TYPE_EDIT != pControlEdit->m_lType)
		return FALSE;

	INT64 llMoney = _ttoi64(((AcUIEdit *) pControlEdit->m_pcsBase)->GetText());
	if (llMoney <= 0)
		return FALSE;
	
	return pThis->SendMoneyChange(pThis->m_pAgcmCharacter->GetSelfCharacter(), llMoney, pThis->m_bMoneyInput);
	}


BOOL AgcmUIGuildWarehouse::CBClickExpansion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;

	INT64 llCost = 999999999999;
	CHAR *pszFormat = pThis->GetMessageTxt(AGCMUIGUILDWAREHOUSE_MESSAGE_EXPAND_CONFIRM);
	if (!pszFormat)
		return FALSE;
	CHAR szConfirm[200];
	ZeroMemory(szConfirm, sizeof(szConfirm));
	_stprintf(szConfirm, pszFormat, llCost);
	
	if (IDOK != pThis->m_pAgcmUIManager2->ActionMessageOKCancelDialog(szConfirm))
		return TRUE;

	if (pAgpdCharacter->m_llMoney < llCost)
		{
		CHAR *psz = pThis->GetMessageTxt(AGCMUIGUILDWAREHOUSE_MESSAGE_NOT_ENOUGH_MONEY);
		if (psz)
			pThis->m_pAgcmUIManager2->ActionMessageOKDialog(psz);

		return TRUE;
		}

	return pThis->SendExpandSlot(pAgpdCharacter);
	}




//	Display callbacks
//====================================================
//
BOOL AgcmUIGuildWarehouse::CBDisplayCaption(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	
	CHAR *pszFormat = pThis->GetMessageTxt(AGCMUIGUILDWAREHOUSE_MESSAGE_CAPTION);
	if (NULL == pszFormat || 0 >= _tcslen(pszFormat))
		return FALSE;
	
	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pThis->m_pAgcmCharacter->GetSelfCharacter());
	if (!pAgpdGuildWarehouse || !pAgpdGuildWarehouse->m_pAgpdGuild)
		return FALSE;

	if ( AP_SERVICE_AREA_WESTERN != g_eServiceArea )
		_stprintf(szDisplay, pszFormat,	pAgpdGuildWarehouse->m_pAgpdGuild->m_szID);
	else
		strncpy_s( szDisplay, 64, pszFormat, _TRUNCATE );

	return TRUE;
	}	


BOOL AgcmUIGuildWarehouse::CBDisplayMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pThis->m_pAgcmCharacter->GetSelfCharacter());
	if (!pAgpdGuildWarehouse || !pAgpdGuildWarehouse->m_pAgpdGuild)
		return FALSE;

	_stprintf(szDisplay, _T("%I64d"), pAgpdGuildWarehouse->m_llMoney);

	return TRUE;
	}	


BOOL AgcmUIGuildWarehouse::CBDisplayMoneyConfirm(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;

	CHAR *psz = NULL;
	if (pThis->m_bMoneyInput)
		psz = pThis->m_pAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_BANK_INPUT_MONEY_CONFIRM);
	else
		psz = pThis->m_pAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_BANK_OUTPUT_MONEY_CONFIRM);

	if (psz)
		sprintf(szDisplay, psz);

	return TRUE;
	}	




//	Boolean callback
//====================================================
//
BOOL AgcmUIGuildWarehouse::CBIsActiveMoneyInButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;
	
	return pThis->m_pAgpmGuildWarehouse->CheckPreviledge(pAgpdCharacter, NULL, AGPDGUILDWAREHOUSE_PREV_MONEY_IN);
	}


BOOL AgcmUIGuildWarehouse::CBIsActiveMoneyOutButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;
	
	return pThis->m_pAgpmGuildWarehouse->CheckPreviledge(pAgpdCharacter, NULL, AGPDGUILDWAREHOUSE_PREV_MONEY_OUT);
	}


BOOL AgcmUIGuildWarehouse::CBIsActiveExpandSlot(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;
	
	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	if (pAgpdGuildWarehouse->m_lSlot > 3)
		return FALSE;
	
	INT64 llCost = 999999999999;
	
	if (pAgpdCharacter->m_llMoney < llCost)
		return FALSE;
	
	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBIsActiveSlot1(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;
	
	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	if (1 > pAgpdGuildWarehouse->m_lSlot)
		return FALSE;
	
	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBIsActiveSlot2(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;
	
	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	if (2 > pAgpdGuildWarehouse->m_lSlot)
		return FALSE;
	
	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::CBIsActiveSlot3(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIGuildWarehouse	*pThis = (AgcmUIGuildWarehouse *) pClass;
	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;
	
	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->m_pAgpmGuildWarehouse->GetGuildWarehouse(pAgpdCharacter);
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	if (3 > pAgpdGuildWarehouse->m_lSlot)
		return FALSE;
	
	return TRUE;
	}




//	Packet
//=======================================================
//
BOOL AgcmUIGuildWarehouse::SendOpen(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	INT8 cOperation = AGPMGUILDWAREHOUSE_OPERATION_OPEN;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmGuildWarehouse->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGUILDWAREHOUSE_PACKET_TYPE,
																  &cOperation,					// operation
																  &pAgpdCharacter->m_lID,		// cid
																  NULL,							// money
																  NULL,							// slot
																  NULL,							// item id
																  NULL,							// layer
																  NULL,							// row
																  NULL,							// column
																  NULL							// result
																  );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmGuildWarehouse->m_csPacket.FreePacket(pvPacket);	
	
	return bResult;
	}


BOOL AgcmUIGuildWarehouse::SendClose(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	INT8 cOperation = AGPMGUILDWAREHOUSE_OPERATION_CLOSE;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmGuildWarehouse->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGUILDWAREHOUSE_PACKET_TYPE,
																  &cOperation,				// operation
																  &pAgpdCharacter->m_lID,	// cid
																  NULL,						// money
																  NULL,						// slot
																  NULL,						// item id
																  NULL,						// layer
																  NULL,						// row
																  NULL,						// column
																  NULL						// result
																  );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmGuildWarehouse->m_csPacket.FreePacket(pvPacket);	
	
	return bResult;
	}


BOOL AgcmUIGuildWarehouse::SendItemIn(AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
	{
	if (!pAgpdCharacter || !pAgpdItem)
		return FALSE;
	
	INT8 cOperation = AGPMGUILDWAREHOUSE_OPERATION_ITEM_IN;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmGuildWarehouse->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGUILDWAREHOUSE_PACKET_TYPE,
																  &cOperation,				// operation
																  &pAgpdCharacter->m_lID,	// cid
																  NULL,						// money
																  NULL,						// slot
																  &pAgpdItem->m_lID,		// item id
																  &lLayer,					// layer
																  &lRow,					// row
																  &lColumn,					// column
																  NULL						// result
																  );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmGuildWarehouse->m_csPacket.FreePacket(pvPacket);	
	
	return bResult;
	}


BOOL AgcmUIGuildWarehouse::SendItemOut(AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem, INT32 lStatus, INT32 lLayer, INT32 lRow, INT32 lColumn)
	{
	if (!pAgpdCharacter || !pAgpdItem)
		return FALSE;
	
	INT8 cOperation = AGPMGUILDWAREHOUSE_OPERATION_ITEM_OUT;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmGuildWarehouse->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGUILDWAREHOUSE_PACKET_TYPE,
																  &cOperation,				// operation
																  &pAgpdCharacter->m_lID,	// cid
																  NULL,						// money
																  &lStatus,						// slot
																  &pAgpdItem->m_lID,		// item id
																  &lLayer,					// layer
																  &lRow,					// row
																  &lColumn,					// column
																  NULL						// result
																  );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmGuildWarehouse->m_csPacket.FreePacket(pvPacket);	
	
	return bResult;
	}


BOOL AgcmUIGuildWarehouse::SendMoneyChange(AgpdCharacter *pAgpdCharacter, INT64 llMoney, BOOL bIn)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	INT8 cOperation = bIn ? AGPMGUILDWAREHOUSE_OPERATION_MONEY_IN : AGPMGUILDWAREHOUSE_OPERATION_MONEY_OUT;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmGuildWarehouse->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGUILDWAREHOUSE_PACKET_TYPE,
																  &cOperation,				// operation
																  &pAgpdCharacter->m_lID,	// cid
																  &llMoney,					// money
																  NULL,						// slot
																  NULL,						// item id
																  NULL,						// layer
																  NULL,						// row
																  NULL,						// column
																  NULL						// result
																  );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmGuildWarehouse->m_csPacket.FreePacket(pvPacket);	
	
	return bResult;
	}


BOOL AgcmUIGuildWarehouse::SendExpandSlot(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	INT8 cOperation = AGPMGUILDWAREHOUSE_OPERATION_EXPAND;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmGuildWarehouse->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMGUILDWAREHOUSE_PACKET_TYPE,
																  &cOperation,				// operation
																  &pAgpdCharacter->m_lID,	// cid
																  NULL,						// money
																  NULL,						// slot
																  NULL,						// item id
																  NULL,						// layer
																  NULL,						// row
																  NULL,						// column
																  NULL						// result
																  );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmGuildWarehouse->m_csPacket.FreePacket(pvPacket);	
	
	return bResult;
	}




//	General helper
//=======================================================
//
CHAR* AgcmUIGuildWarehouse::GetMessageTxt(eAGCMUIGUILDWAREHOUSE_MESSAGE eMessage)
	{
	if (0 > eMessage || eMessage > AGCMUIGUILDWAREHOUSE_MESSAGE_MAX)
		return NULL;

	return m_pAgcmUIManager2->GetUIMessage(s_szMessage[eMessage]);
	}




//	OnAddModule helper
//================================================
//
BOOL AgcmUIGuildWarehouse::AddEvent()
	{
	for (int i=0; i < AGCMUIGUILDWAREHOUSE_EVENT_MAX; ++i)
		{
		m_lEvent[i] = m_pAgcmUIManager2->AddEvent(s_szEvent[i]);
		if (m_lEvent[i] < 0)
			return FALSE;
		}

	return TRUE;	
	}


BOOL AgcmUIGuildWarehouse::AddFunction()
	{
	if (!m_pAgcmUIManager2->AddFunction(this, "GWARE_MOVEITEM", CBMoveItem, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "GWARE_ClickLayer", CBClickLayer, 1, "GWARE Grid Control"))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "GWARE_MONEY_IN", CBClickMoneyIn, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "GWARE_MONEY_OUT", CBClickMoneyOut, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "GWARE_MONEY_CONF", CBClickMoneyConfirm, 1, "GWARE Money Edit"))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, "GWARE_EXPAND", CBClickExpansion, 0))
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::AddDisplay()
	{
	if (!m_pAgcmUIManager2->AddDisplay(this, "GWARE_CAPTION", 0, CBDisplayCaption, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "GWARE_MONEY", 0, CBDisplayMoney, AGCDUI_USERDATA_TYPE_UINT64))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, "GWARE_CONFIRM", 0, CBDisplayMoneyConfirm, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::AddUserData()
	{
	// grid
	m_pstWarehouseGrid = m_pAgcmUIManager2->AddUserData("GWARE_Grid", m_pWarehouseGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pstWarehouseGrid)
		return FALSE;
	
	// grid layer index
	m_pstWarehouseLayerIndex = m_pAgcmUIManager2->AddUserData("GWARE_LayerIndex", &m_lWarehouseLayerIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstWarehouseLayerIndex)
		return FALSE;

	// money
	m_pstWarehouseMoney = m_pAgcmUIManager2->AddUserData("GWARE_Money", &m_llWarehouseMoney, sizeof(INT64), 1, AGCDUI_USERDATA_TYPE_UINT64);
	if (!m_pstWarehouseMoney )
		return FALSE;
		
	// active button
	m_pstActiveButton = m_pAgcmUIManager2->AddUserData("GWARE_ACTIVE_BTN", &m_lActiveButton, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstActiveButton)
		return FALSE;
	// dummy
	m_pstDummy = m_pAgcmUIManager2->AddUserData("GWARE_DUMMY", &m_lDummy, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstDummy)
		return FALSE;
			
	return TRUE;
	}


BOOL AgcmUIGuildWarehouse::AddBoolean()
	{
	if (!m_pAgcmUIManager2->AddBoolean(this, "GWARE_ACT_MONEYIN", CBIsActiveMoneyInButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddBoolean(this, "GWARE_ACT_MONEYOUT", CBIsActiveMoneyOutButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddBoolean(this, "GWARE_ACT_EXPAND", CBIsActiveExpandSlot, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddBoolean(this, "GWARE_ACT_SLOT1", CBIsActiveSlot1, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddBoolean(this, "GWARE_ACT_SLOT2", CBIsActiveSlot2, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddBoolean(this, "GWARE_ACT_SLOT3", CBIsActiveSlot3, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
	}



