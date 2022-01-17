//	AgcmUIEventItemRepair.cpp
///////////////////////////////////////////////////////

#include "AgcmUIEventItemRepair.h"
#include "AuStrTable.h"

AgcmUIEventItemRepair::AgcmUIEventItemRepair()
{
	SetModuleName("AgcmUIEventItemRepair");

	m_lEventUIOpen			= 0;
	m_lEventUIClose			= 0;
	m_lEventInventoryUIOpen = 0;
	m_lEventInventoryUIClose = 0;

	m_lMessageAlreadyMax		= 0;
	m_lMessageInsufficientMoney = 0;
	m_lMessageDontRepair		= 0;

	m_bHold = FALSE;
	m_bIsUIOpen = FALSE;

	m_RepairCost = 0;
	m_InventoryMoney = 0;

	m_pcsUDMainGrid = NULL;
	m_pcsUDInventoryMoney = NULL;
	m_pcsUDRepairCost = NULL;

	m_pSources.MemSetAll();

	ZeroMemory(&m_stOpenPos, sizeof(m_stOpenPos));
}

AgcmUIEventItemRepair::~AgcmUIEventItemRepair()
{
}

BOOL AgcmUIEventItemRepair::OnAddModule()
{
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgcmUIManager2			= (AgcmUIManager2 *)		GetModule("AgcmUIManager2");
	m_pcsAgcmCharacter			= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pcsApmEventManager		= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pcsAgpmEventItemRepair	= (AgpmEventItemRepair *)	GetModule("AgpmEventItemRepair");
	m_pcsAgcmEventItemRepair	= (AgcmEventItemRepair *)	GetModule("AgcmEventItemRepair");
	m_pcsAgpmGrid				= (AgpmGrid *)				GetModule("AgpmGrid");
	m_pcsAgpmItem				= (AgpmItem *)				GetModule("AgpmItem");
	m_pcsAgcmUIItem				= (AgcmUIItem *)			GetModule("AgcmUIItem");
	m_pcsAgcmChatting			= (AgcmChatting2 *)			GetModule("AgcmChatting2");
	m_pcsAgpmFactors			= (AgpmFactors *)			GetModule("AgpmFactors");
	m_pcsAgcmSkill				= (AgcmSkill *)				GetModule("AgcmSkill");

	if (!m_pcsAgcmUIManager2 ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgcmCharacter || 
		!m_pcsApmEventManager ||
		!m_pcsAgpmEventItemRepair ||
		!m_pcsAgcmEventItemRepair ||
		!m_pcsAgpmGrid ||
		!m_pcsAgpmItem ||
		!m_pcsAgcmChatting ||
		!m_pcsAgcmUIItem ||
		!m_pcsAgpmFactors ||
		!m_pcsAgcmSkill)
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	if (!m_pcsAgcmEventItemRepair->SetCallbackGrant(CBGrant, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackRemoveInventoryGrid(CBRemoveInventoryGrid, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateMoney(CBUpdateMoney, this))
		return FALSE;

	if (!m_pcsAgpmEventItemRepair->SetCallbackRepairAck(CBItemRepairAck, this))
		return FALSE;

	if (!m_pcsAgcmSkill->SetCallbackCastFailForRide(CBRideFailMessage, this))
		return FALSE;

	if (!AddFunctions())
		return FALSE;

	if (!AddEvents())
		return FALSE;

	if (!AddUserDatas())
		return FALSE;

	if (!AddDisplay())
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventItemRepair::OnInit()
{
	m_pcsAgpmGrid->Init(&m_RepairItemGrid, ITEM_REPAIR_GRID_LAYER, ITEM_REPAIR_GRID_ROW, ITEM_REPAIR_GRID_COLUMN);
	m_RepairItemGrid.m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;

	return TRUE;
}

BOOL AgcmUIEventItemRepair::OnDestroy()
{
	m_pcsAgpmGrid->Remove(&m_RepairItemGrid);
	return TRUE;
}

BOOL AgcmUIEventItemRepair::AddFunctions()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "ItemRepair_MoveItem", CBMoveItem, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "ItemRepair_MoveEnd", CBMoveEndItem, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "ItemRepair_UIOpen", CBItemRepairUIOpen, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "ItemRepair_RepairReq", CBItemRepairReq, 0))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventItemRepair::AddEvents()
{
	m_lEventUIOpen = m_pcsAgcmUIManager2->AddEvent("ItemRepair_UIOpen_Event");
	if (m_lEventUIOpen < 0)
		return FALSE;

	m_lEventUIClose = m_pcsAgcmUIManager2->AddEvent("ItemRepair_UIClose_Event");
	if (m_lEventUIClose < 0)
		return FALSE;

	m_lEventInventoryUIOpen = m_pcsAgcmUIManager2->AddEvent("ItemRepair_InventoryUIOpen_Event");
	if (m_lEventInventoryUIOpen < 0)
		return FALSE;

	m_lEventInventoryUIClose = m_pcsAgcmUIManager2->AddEvent("ItemRepair_InventoryUIClose_Event");
	if (m_lEventInventoryUIClose < 0)
		return FALSE;

	m_lMessageAlreadyMax = m_pcsAgcmUIManager2->AddEvent("ItemRepair_AlreadyMax_Msg");
	if (m_lMessageAlreadyMax < 0)
		return FALSE;

	m_lMessageInsufficientMoney = m_pcsAgcmUIManager2->AddEvent("ItemRepair_InsufficientMoney_Msg");
	if (m_lMessageInsufficientMoney < 0)
		return FALSE;

	m_lMessageDontRepair = m_pcsAgcmUIManager2->AddEvent("ItemRepair_DontRepair_Msg");
	if (m_lMessageDontRepair < 0)
		return FALSE;
	
	return TRUE;
}

BOOL AgcmUIEventItemRepair::AddUserDatas()
{
	m_pcsUDRepairCost = m_pcsAgcmUIManager2->AddUserData("ItemRepair_Cost_Data", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUDRepairCost)
		return FALSE;

	m_pcsUDMainGrid = m_pcsAgcmUIManager2->AddUserData("ItemRepair_MainGrid_Data", &m_RepairItemGrid, sizeof(AgpdGrid), ITEM_REPAIR_GRID_MAX, AGCDUI_USERDATA_TYPE_GRID);
	if (!m_pcsUDMainGrid)
		return FALSE;

	m_pcsUDInventoryMoney = m_pcsAgcmUIManager2->AddUserData("ItemRepair_InventoryMoney_Data", NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUDInventoryMoney)
		return FALSE;
	
	return TRUE;
}

BOOL AgcmUIEventItemRepair::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ItemRepair_InventoryMoney_Display", 0, CBDisplayInventoryMoney, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ItemRepair_Cost_Display", 0, CBDisplayRepairCost, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventItemRepair::ClearGridMainUI()
{
	if (0 != m_RepairItemGrid.m_lItemCount)
		m_pcsAgpmGrid->Reset(&m_RepairItemGrid);

	m_pSources.MemSetAll();

	return TRUE;
}

BOOL AgcmUIEventItemRepair::RefreshGrid()
{
	ASSERT(NULL != m_pcsUDMainGrid);
	if (NULL != m_pcsUDMainGrid)
	{
		m_pcsUDMainGrid->m_stUserData.m_pvData	= &m_RepairItemGrid;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUDMainGrid);
	}

	return TRUE;
}

BOOL AgcmUIEventItemRepair::RefreshInventoryMoney()
{
	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUDInventoryMoney);
}

BOOL AgcmUIEventItemRepair::RefreshRepairCost()
{
	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUDRepairCost);
}

BOOL AgcmUIEventItemRepair::FindItemRepairGrid(AgpdItem *pAgpdItem)
{
	for (INT16 i = 0; i < ITEM_REPAIR_GRID_MAX; ++i)
	{
		if (m_pSources[i] && pAgpdItem == m_pSources[i])
		{
			return TRUE;
		}
	}

	return FALSE;
}

void AgcmUIEventItemRepair::ShowMessage(EnumAgpmEventItemRepairResultCode eResult)
{
	switch (eResult)
	{
	case AGPMEVENT_ITEMREPAIR_RESULTCODE_SUCCESS:			
		AddSystemMessage(ClientStr().GetStr(STI_REPAIR_COMPLETE));	break;
			
	case AGPMEVENT_ITEMREPAIR_RESULTCODE_NOT_ENOUGH_MONEY:
		m_pcsAgcmUIManager2->ThrowEvent(m_lMessageInsufficientMoney);	break;

	case AGPMEVENT_ITEMREPAIR_RESULTCODE_ALREADY_FULL:
		m_pcsAgcmUIManager2->ThrowEvent(m_lMessageAlreadyMax);	break;

	case AGPMEVENT_ITEMREPAIR_RESULTCODE_REPAIR_FAIL:
		AddSystemMessage(ClientStr().GetStr(STI_REPAIR_FAIL));	break;

	case AGPMEVENT_ITEMREPAIR_RESLUTCODE_CANNOT_REPAIR_ITEM:
		AddSystemMessage(ClientStr().GetStr(STI_REPAIR_CANT));	break;
	};
}

BOOL AgcmUIEventItemRepair::CBGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventItemRepair	*pThis = (AgcmUIEventItemRepair*)pClass;
	AgpdCharacter			*pAgpdCharacter = (AgpdCharacter*)pData;
	ApdEvent				*pcsEvent = (ApdEvent*)pCustData;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUIOpen);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventInventoryUIOpen);
	
	pThis->m_stOpenPos = pAgpdCharacter->m_stPos;
	pThis->m_bIsUIOpen	= TRUE;

	return TRUE;
}

BOOL AgcmUIEventItemRepair::CBItemRepairAck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter			*pcsCharacter = (AgpdCharacter *) pData;
	AgcmUIEventItemRepair	*pThis = (AgcmUIEventItemRepair *) pClass;
	AgpdItemRepair			*pcsRepairInfo = (AgpdItemRepair *) pCustData;

	pThis->m_bHold = FALSE;
	pThis->ShowMessage(pcsRepairInfo->eResultCode);

	if (AGPMEVENT_ITEMREPAIR_RESULTCODE_SUCCESS == pcsRepairInfo->eResultCode)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUIClose);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventInventoryUIClose);
	}

	return TRUE;
}

BOOL AgcmUIEventItemRepair::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventItemRepair	*pThis			= (AgcmUIEventItemRepair *)	pClass;
	AgpdCharacter			*pcsCharacter	= (AgpdCharacter *)	pData;

	if (!pThis->m_bIsUIOpen)
		return TRUE;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pThis->m_stOpenPos);

	if ((INT32) fDistance < AGCMUIEVENT_ITEMREPAIR_CLOSE_UI_DISTANCE)
		return TRUE;

	// Accept 윈도우를 닫는다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUIClose);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventInventoryUIClose);

	pThis->m_bIsUIOpen = FALSE;

	return TRUE;
}
BOOL AgcmUIEventItemRepair::CBRideFailMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUIEventItemRepair *pThis = (AgcmUIEventItemRepair*)pClass;
	pThis->AddSystemMessage(ClientStr().GetStr(STI_REPAIR_WITH_MOUNT));

	return TRUE;
}

BOOL AgcmUIEventItemRepair::CBMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl ||
		!pcsSourceControl->m_pcsBase ||
		AcUIBase::TYPE_GRID != pcsSourceControl->m_lType
		)
		return FALSE;

	AgcmUIEventItemRepair	*pThis					= (AgcmUIEventItemRepair *) pClass;
	AcUIGrid				*pAcUIGrid				= (AcUIGrid *) pcsSourceControl->m_pcsBase;
	AgpdGridSelectInfo		*pAgpdGridSelectInfo	= pAcUIGrid->GetDragDropMessageInfo();

	if (!pAgpdGridSelectInfo || !pAgpdGridSelectInfo->pGridItem ||
		 AGPDGRID_ITEM_TYPE_ITEM != pAgpdGridSelectInfo->pGridItem->m_eType)
		return FALSE;

	if (pThis->m_bHold)
		return FALSE;

	AgpdItem *pAgpdItem = pThis->m_pcsAgpmItem->GetItem(pAgpdGridSelectInfo->pGridItem->m_lItemID);
	if (!pAgpdItem)
		return FALSE;

	pThis->OnMoveItem(pAgpdItem, pAcUIGrid->m_lNowLayer, pAgpdGridSelectInfo->lGridRow, pAgpdGridSelectInfo->lGridColumn);

	return TRUE;
}

BOOL AgcmUIEventItemRepair::OnMoveItem(AgpdItem *pAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
{
	// it must be in inventory
	ASSERT(AGPDITEM_STATUS_INVENTORY == pAgpdItem->m_eStatus);
	if (AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus)
		return FALSE;

	// grid full	
	if (m_pcsAgpmGrid->IsFullGrid(&m_RepairItemGrid))
	{
		AddSystemMessage(ClientStr().GetStr(STI_REPAIR_LIST_FULL));
		return FALSE;
	}

	// is empty grid?
	if (!m_pcsAgpmGrid->IsEmptyGrid(&m_RepairItemGrid, nLayer, nRow, nColumn, 1, 1))
		return FALSE;
	
	// Equip Item 만 수리한다.
	if (((AgpdItemTemplate *) pAgpdItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lMessageDontRepair);
		return TRUE;
	}

	// Item Durability validate
	INT32	lMaxItemDurability	= 0;
	m_pcsAgpmFactors->GetValue(&pAgpdItem->m_csFactor, &lMaxItemDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);
	if (lMaxItemDurability < 1)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lMessageDontRepair);
		return TRUE;
	}

	INT32	lItemDurability		= 0;
	m_pcsAgpmFactors->GetValue(&pAgpdItem->m_csFactor, &lItemDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);

	if (lItemDurability == lMaxItemDurability)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lMessageAlreadyMax);
		return TRUE;
	}

	// check if this dropped before
	BOOL bFind = FindItemRepairGrid(pAgpdItem); 
	if (bFind)
	{
		// 만약 같은 아이템이 존재하는데 또 Drop된 아이템이라면 위치 변경이 목적
		RemoveItemRepairItemGrid(pAgpdItem);
	}

	// add to grid
	AddItemRepairItemGrid(pAgpdItem, nLayer, nRow, nColumn);

	if (!bFind)
		m_RepairCost += m_pcsAgpmEventItemRepair->GetItemRepairPrice(pAgpdItem, m_pcsAgcmCharacter->GetSelfCharacter());

	RefreshGrid();
	RefreshRepairCost();
			
	return TRUE;	
}

BOOL AgcmUIEventItemRepair::CBMoveEndItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl ||
		!pcsSourceControl->m_pcsBase ||
		AcUIBase::TYPE_GRID != pcsSourceControl->m_lType
		)
		return FALSE;

	AgcmUIEventItemRepair	*pThis					= (AgcmUIEventItemRepair *) pClass;
	AcUIGrid				*pAcUIGrid				= (AcUIGrid *) pcsSourceControl->m_pcsBase;
	AgpdGridSelectInfo		*pAgpdGridSelectInfo	= pAcUIGrid->GetGridItemClickInfo();

	AgpdGrid *pAgpdGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsSourceControl);
	if (!pAgpdGridSelectInfo || !pAgpdGridSelectInfo->pGridItem || !pAgpdGrid)
		return FALSE;

	AgpdItem *pAgpdItem = pThis->m_pcsAgpmItem->GetItem(pAgpdGridSelectInfo->pGridItem->m_lItemID);
	if (!pAgpdItem)
		return FALSE;

	if (pThis->m_bHold)
		return FALSE;
	
	// remove from grid	
	pThis->m_pcsAgpmGrid->RemoveItem(pAgpdGrid, pAgpdGridSelectInfo->pGridItem);
	pThis->m_pcsAgpmGrid->Clear(pAgpdGrid, pAcUIGrid->m_lNowLayer, pAgpdGridSelectInfo->lGridRow, pAgpdGridSelectInfo->lGridColumn, 1, 1);
	pThis->m_RepairCost -= pThis->m_pcsAgpmEventItemRepair->GetItemRepairPrice(pAgpdItem, pThis->m_pcsAgcmCharacter->GetSelfCharacter());

	pThis->RefreshGrid();	
	pThis->RefreshRepairCost();

	// remove from list
	pThis->m_pSources[pAgpdGridSelectInfo->lGridRow * ITEM_REPAIR_GRID_COLUMN + pAgpdGridSelectInfo->lGridColumn] = NULL;

	return TRUE;
}

BOOL AgcmUIEventItemRepair::AddItemRepairItemGrid(AgpdItem *pAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
{
	// save dropped item
	m_pSources[(nRow * ITEM_REPAIR_GRID_COLUMN) + nColumn] = pAgpdItem;

	m_pcsAgpmGrid->AddItem(&m_RepairItemGrid, pAgpdItem->m_pcsGridItem);
	return m_pcsAgpmGrid->Add(&m_RepairItemGrid, nLayer, nRow, nColumn, pAgpdItem->m_pcsGridItem, 1, 1);
}

BOOL AgcmUIEventItemRepair::RemoveItemRepairItemGrid(AgpdItem *pAgpdItem)
{
	for (INT16 i = 0; i < ITEM_REPAIR_GRID_MAX; ++i)
	{
		if (m_pSources[i] && pAgpdItem == m_pSources[i])
		{
			m_pSources[i] = NULL;
		}
	}

	return m_pcsAgpmGrid->RemoveItem(&m_RepairItemGrid, pAgpdItem->m_pcsGridItem);
}

BOOL AgcmUIEventItemRepair::CBRemoveInventoryGrid(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIEventItemRepair *pThis = (AgcmUIEventItemRepair*)pClass;
	AgpdItem *pcsAgpdItem = (AgpdItem*)pData;

	pThis->RemoveItemRepairItemGrid(pcsAgpdItem);
	return pThis->RefreshGrid();
}

BOOL AgcmUIEventItemRepair::CBUpdateMoney(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIEventItemRepair *pThis = (AgcmUIEventItemRepair*)pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter*)pData;

	// 가끔씩 크리쳐가 pcsAgpdCharacter로 들어와서 자신의 캐릭터인지 확인한다.
	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsAgpdCharacter)
		return FALSE;

	pThis->m_InventoryMoney = pcsAgpdCharacter->m_llMoney;
	return pThis->RefreshInventoryMoney();
}

BOOL AgcmUIEventItemRepair::AddSystemMessage(CHAR* pszMsg)
{
	if(!pszMsg)
		return FALSE;

	AgpdChatData	stChatData;
	memset(&stChatData, 0, sizeof(stChatData));

	stChatData.eChatType = AGPDCHATTING_TYPE_SYSTEM_LEVEL1;
	stChatData.szMessage = pszMsg;

	stChatData.lMessageLength = strlen(stChatData.szMessage);
		
	m_pcsAgcmChatting->AddChatMessage(AGCMCHATTING_TYPE_SYSTEM, &stChatData);
	m_pcsAgcmChatting->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);

	return TRUE;
}

BOOL AgcmUIEventItemRepair::CBDisplayInventoryMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIEventItemRepair* pThis = (AgcmUIEventItemRepair *)pClass;

	sprintf(szDisplay, "%I64d", pThis->m_InventoryMoney);

	return TRUE;
}

BOOL AgcmUIEventItemRepair::CBDisplayRepairCost(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIEventItemRepair* pThis = (AgcmUIEventItemRepair *)pClass;

	sprintf(szDisplay, "%I64d", pThis->m_RepairCost);

	return TRUE;
}

BOOL AgcmUIEventItemRepair::CBItemRepairUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventItemRepair* pThis = (AgcmUIEventItemRepair *)pClass;

	AgpdCharacter* pcsAgpdCahracter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	ASSERT(NULL != pcsAgpdCahracter);
	if (NULL == pcsAgpdCahracter)
		return FALSE;

	pThis->m_InventoryMoney = pcsAgpdCahracter->m_llMoney;
	pThis->m_RepairCost = 0;
	pThis->ClearGridMainUI();
	pThis->RefreshInventoryMoney();
	pThis->RefreshRepairCost();
	pThis->RefreshGrid();

	return TRUE;
}

BOOL AgcmUIEventItemRepair::CBItemRepairReq(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventItemRepair* pThis = (AgcmUIEventItemRepair *)pClass;

	pThis->m_bHold = TRUE;

	AgpdCharacter* pcsAgpdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	ASSERT(NULL != pcsAgpdCharacter);
	if (NULL == pcsAgpdCharacter)
		return FALSE;

	ApSafeArray<INT32, ITEM_REPAIR_GRID_MAX> RepaireList;
	RepaireList.MemSetAll();

	BOOL lCount = 0;
	for (int i = 0; i < ITEM_REPAIR_GRID_MAX; ++i)
	{
		if (NULL == pThis->m_pSources[i])
			RepaireList[i] = 0;
		else
		{
			RepaireList[i] = pThis->m_pSources[i]->m_lID;
			++lCount;
		}
	}

	if (0 == lCount)
	{
		pThis->AddSystemMessage(ClientStr().GetStr(STI_REPAIR_NO_ITEM));
		pThis->m_bHold = FALSE;
		return TRUE;
	}

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventItemRepair->MakePacketItemRepair(pThis->m_pcsAgcmEventItemRepair->m_pcsLastGrantEvent, 
																			AGPMEVENT_ITEMREPAIR_OPERATION_REPAIR_REQ,
																			pcsAgpdCharacter->m_lID,
																			&RepaireList[0],
																			pThis->m_RepairCost,
																			AGPMEVENT_ITEMREPAIR_RESULTCODE_NONE,
																			&nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult =  pThis->SendPacket(pvPacket, nPacketLength);
	pThis->m_pcsAgpmEventItemRepair->m_csPacket.FreePacket(pvPacket);

	return bResult;	
}