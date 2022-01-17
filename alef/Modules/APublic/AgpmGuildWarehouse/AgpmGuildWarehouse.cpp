/*===========================================================================

	AgpmGuildWarehouse.cpp

===========================================================================*/


#include "AgpmGuildWarehouse.h"
#include "ApMemoryTracker.h"


/************************************************************/
/*		The Implementation of AgpmGuildWarehouse class		*/
/************************************************************/
//
AgpmGuildWarehouse::AgpmGuildWarehouse()
	{
	SetModuleName(_T("AgpmGuildWarehouse"));
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPMGUILDWAREHOUSE_PACKET_TYPE);
	
	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_INT8,			1,	// operation
							AUTYPE_INT32,			1,	// cid
							AUTYPE_INT64,			1,	// money
							AUTYPE_INT32,			1,	// slot
							AUTYPE_INT32,			1,	// item id
							AUTYPE_INT32,			1,	// layer
							AUTYPE_INT32,			1,	// row
							AUTYPE_INT32,			1,	// column
							AUTYPE_INT32,			1,	// result
							AUTYPE_END,				0
							);
	}


AgpmGuildWarehouse::~AgpmGuildWarehouse()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgpmGuildWarehouse::OnAddModule()
	{
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmGuild = (AgpmGuild *) GetModule(_T("AgpmGuild"));
	m_pAgpmItem = (AgpmItem *) GetModule(_T("AgpmItem"));
	m_pAgpmGrid = (AgpmGrid *) GetModule(_T("AgpmGrid"));

	if (!m_pAgpmCharacter || !m_pAgpmGuild || !m_pAgpmItem || !m_pAgpmGrid)
		return FALSE;

	m_nIndexGuildAD = m_pAgpmGuild->AttachGuildData(this, sizeof(AgpdGuildWarehouse), ConAgpdGuildWarehouse, DesAgpdGuildWarehouse);
	if (0 > m_nIndexGuildAD)
		return FALSE;

	return TRUE;
	}


BOOL AgpmGuildWarehouse::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8	cOperation = AGPMGUILDWAREHOUSE_OPERATION_NONE;
	INT32	lCID = AP_INVALID_CID;
	INT32	lResult = AGPMGUILDWAREHOUSE_RESULT_NONE;
	INT64	llMoney = -1;
	INT32	lSlot = -1;
	INT32	lItemID = 0;
	INT32	lLayer = -1;
	INT32	lRow = -1;
	INT32	lColumn = -1;
	
	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,			// operation
						&lCID,					// cid
						&llMoney,				// money
						&lSlot,					// slot
						&lItemID,				// item id
						&lLayer,				// layer
						&lRow,					// row
						&lColumn,				// column
						&lResult				// result
						);

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacterLock(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	switch (cOperation)
		{
		case AGPMGUILDWAREHOUSE_OPERATION_BASE_INFO :
			{
			AgpdGuildWarehouse *pAgpdGuildWarehouse = GetGuildWarehouse(pAgpdCharacter);
			if (pAgpdGuildWarehouse)
				{
				AuAutoLock LockWarehouse(pAgpdGuildWarehouse->m_Mutex);
				if (LockWarehouse.Result())
					{
					if (llMoney > -1)
						pAgpdGuildWarehouse->m_llMoney = llMoney;
					if (lSlot > -1)
						pAgpdGuildWarehouse->m_lSlot = lSlot;
						
					EnumCallback(AGPMGUILDWAREHOUSE_CB_BASE_INFO, pAgpdCharacter, pAgpdGuildWarehouse);
					}
				}
			}
			break;
		case AGPMGUILDWAREHOUSE_OPERATION_OPEN :
			EnumCallback(AGPMGUILDWAREHOUSE_CB_OPEN, pAgpdCharacter, &lResult);
			break;
			
		case AGPMGUILDWAREHOUSE_OPERATION_CLOSE :
			EnumCallback(AGPMGUILDWAREHOUSE_CB_CLOSE, pAgpdCharacter, NULL);
			break;
			
		case AGPMGUILDWAREHOUSE_OPERATION_MONEY_IN :
			EnumCallback(AGPMGUILDWAREHOUSE_CB_MONEY_IN, pAgpdCharacter, &llMoney);
			break;

		case AGPMGUILDWAREHOUSE_OPERATION_MONEY_OUT :
			EnumCallback(AGPMGUILDWAREHOUSE_CB_MONEY_OUT, pAgpdCharacter, &llMoney);
			break;

		case AGPMGUILDWAREHOUSE_OPERATION_ITEM_IN :
			{
			PVOID pvBuffer[4];
			pvBuffer[0] = &lItemID;
			pvBuffer[1] = &lLayer;
			pvBuffer[2] = &lRow;
			pvBuffer[3] = &lColumn;
			EnumCallback(AGPMGUILDWAREHOUSE_CB_ITEM_IN, pAgpdCharacter, pvBuffer);
			}
			break;

		case AGPMGUILDWAREHOUSE_OPERATION_ITEM_OUT :
			{
			PVOID pvBuffer[5];
			pvBuffer[0] = &lItemID;
			pvBuffer[1] = &lLayer;
			pvBuffer[2] = &lRow;
			pvBuffer[3] = &lColumn;
			pvBuffer[4] = &lSlot;	// lStatus (inventory or sub inventory)
			EnumCallback(AGPMGUILDWAREHOUSE_CB_ITEM_OUT, pAgpdCharacter, pvBuffer);
			}
			break;
		
		case AGPMGUILDWAREHOUSE_OPERATION_EXPAND :
			EnumCallback(AGPMGUILDWAREHOUSE_CB_EXPAND, pAgpdCharacter, &lResult);
			break;

		default :
			break;
		}

	pAgpdCharacter->m_Mutex.Release();

	return TRUE;
	}




//	Attached Data
//==================================================
//
AgpdGuildWarehouse* AgpmGuildWarehouse::GetGuildWarehouse(AgpdGuild *pAgpdGuild)
	{
	if (!pAgpdGuild)
		return NULL;

	return (AgpdGuildWarehouse *) m_pAgpmGuild->GetAttachedModuleData(m_nIndexGuildAD, pAgpdGuild);
	}


AgpdGuildWarehouse* AgpmGuildWarehouse::GetGuildWarehouse(TCHAR *pszGuild)
	{
	return GetGuildWarehouse(m_pAgpmGuild->GetGuild(pszGuild));
	}


AgpdGuildWarehouse* AgpmGuildWarehouse::GetGuildWarehouse(AgpdCharacter *pAgpdCharacter)
	{
	AgpdGuild *pAgpdGuild = m_pAgpmGuild->GetGuild(pAgpdCharacter);
	
	if (pAgpdGuild && m_pAgpmGuild->GetMember(pAgpdGuild, pAgpdCharacter->m_szID))
		return GetGuildWarehouse(pAgpdGuild);

	return NULL;
	}


BOOL AgpmGuildWarehouse::ConAgpdGuildWarehouse(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmGuildWarehouse *pThis	= (AgpmGuildWarehouse *) pClass;
	AgpdGuild *pAgpdGuild		= (AgpdGuild *) pData;

	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->GetGuildWarehouse(pAgpdGuild);

	pAgpdGuildWarehouse->m_Mutex.Init();
	pAgpdGuildWarehouse->Reset();
	pAgpdGuildWarehouse->m_pAgpdGuild = pAgpdGuild;
	// init set of members who opened warehouse
	pAgpdGuildWarehouse->m_pOpenedMember = new OpenedMember;
	// grid
	pThis->m_pAgpmGrid->Init(&pAgpdGuildWarehouse->m_WarehouseGrid, AGPDGRID_TYPE_GUILD_WAREHOUSE);
	pAgpdGuildWarehouse->m_WarehouseGrid.m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;
	
	
	return TRUE;
	}


BOOL AgpmGuildWarehouse::DesAgpdGuildWarehouse(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmGuildWarehouse *pThis	= (AgpmGuildWarehouse *) pClass;
	AgpdGuild *pAgpdGuild		= (AgpdGuild *) pData;

	AgpdGuildWarehouse *pAgpdGuildWarehouse = pThis->GetGuildWarehouse(pAgpdGuild);

	// grid
	pThis->m_pAgpmGrid->Remove(&pAgpdGuildWarehouse->m_WarehouseGrid);
	// set
	if (pAgpdGuildWarehouse->m_pOpenedMember)
		delete pAgpdGuildWarehouse->m_pOpenedMember;

	pAgpdGuildWarehouse->Reset();
	pAgpdGuildWarehouse->m_Mutex.Destroy();
	
	return TRUE;
	}




//	Operations
//===================================================
//
BOOL AgpmGuildWarehouse::AddItem(AgpdGuildWarehouse *pAgpdGuildWarehouse, AgpdItem *pAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bCallback)
	{
	if (!pAgpdGuildWarehouse || !pAgpdItem)
		return FALSE;

	if (AGPDITEM_STATUS_GUILD_WAREHOUSE == pAgpdItem->m_eStatus)
		return FALSE;
		
	if (!m_pAgpmItem->CheckUpdateStatus(pAgpdItem, AGPDITEM_STATUS_GUILD_WAREHOUSE, lLayer, lRow, lColumn))
		return FALSE;

	if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
		return FALSE;

	if (lLayer < 0 && lRow < 0 && lColumn < 0)
		return FALSE;

	// if gheld
	if (AGPMITEM_TYPE_OTHER == pAgpdItem->m_pcsItemTemplate->m_nType
		&& AGPMITEM_OTHER_TYPE_MONEY == ((AgpdItemTemplateOther *)pAgpdItem->m_pcsItemTemplate)->m_eOtherItemType)
		return FALSE;

	// delete owner information.
	// may cause a db update operation(empty charid)
	// however it will be deleted soon, this is just insurance
	if (!m_pAgpmItem->ChangeItemOwner(pAgpdItem, NULL))
		return FALSE;

	// check grid
	if (!m_pAgpmGrid->IsEmptyGrid(&pAgpdGuildWarehouse->m_WarehouseGrid, lLayer, lRow, lColumn, 1, 1))
		return FALSE;

	if (!m_pAgpmGrid->AddItem(&pAgpdGuildWarehouse->m_WarehouseGrid, pAgpdItem->m_pcsGridItem))
		return FALSE;

	if (!m_pAgpmGrid->Add(&pAgpdGuildWarehouse->m_WarehouseGrid, lLayer, lRow, lColumn, pAgpdItem->m_pcsGridItem, 1, 1))
		return FALSE;

	m_pAgpmItem->RemoveStatus(pAgpdItem, AGPDITEM_STATUS_INVENTORY);
	pAgpdItem->m_eStatus = AGPDITEM_STATUS_GUILD_WAREHOUSE;
	pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB]		= lLayer;
	pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW]		= lRow;
	pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN]	= lColumn;

	pAgpdItem->m_pcsGuildWarehouseBase	= pAgpdGuildWarehouse;

	if (bCallback)
		EnumCallback(AGPMGUILDWAREHOUSE_CB_ADD_ITEM, pAgpdGuildWarehouse, pAgpdItem);
	
	return TRUE;
	}


BOOL AgpmGuildWarehouse::AddItem(AgpdGuild *pAgpdGuild, AgpdItem *pAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bCallback)
	{
	return AddItem(GetGuildWarehouse(pAgpdGuild), pAgpdItem, lLayer, lRow, lColumn);
	}


BOOL AgpmGuildWarehouse::AddItem(AgpdGuildWarehouse *pAgpdGuildWarehouse, AgpdItem *pAgpdItem, BOOL bCallback)
	{
	if (!pAgpdGuildWarehouse || !pAgpdItem)
		return FALSE;
	
	return AddItem(pAgpdGuildWarehouse, pAgpdItem,
				   pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
				   pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
				   pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN],
				   bCallback
				   );
	}


BOOL AgpmGuildWarehouse::AddItem(AgpdGuild *pAgpdGuild, AgpdItem *pAgpdItem, BOOL bCallback)
	{
	if (!pAgpdGuild || !pAgpdItem)
		return FALSE;
	
	return AddItem(GetGuildWarehouse(pAgpdGuild), pAgpdItem,
				   pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
				   pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
				   pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN],
				   bCallback
				   );
	}


BOOL AgpmGuildWarehouse::RemoveItem(AgpdGuildWarehouse *pAgpdGuildWarehouse, AgpdItem *pAgpdItem)
	{
	if (!pAgpdGuildWarehouse || !pAgpdItem)
		return FALSE;

	if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
		return FALSE;

	// if gheld
	if (AGPMITEM_TYPE_OTHER == pAgpdItem->m_pcsItemTemplate->m_nType
		&& AGPMITEM_OTHER_TYPE_MONEY == ((AgpdItemTemplateOther *)pAgpdItem->m_pcsItemTemplate)->m_eOtherItemType)
		return FALSE;

	// check exist
	INT32 lIndex = -1;
	if (!m_pAgpmGrid->IsExistItem(&pAgpdGuildWarehouse->m_WarehouseGrid, AGPDGRID_ITEM_TYPE_ITEM, pAgpdItem->m_lID, &lIndex))
		return FALSE;

	if (!m_pAgpmGrid->DeleteItem(&pAgpdGuildWarehouse->m_WarehouseGrid, pAgpdItem->m_pcsGridItem))
		return FALSE;
	
	if (pAgpdItem->m_pcsItemTemplate && -1 != lIndex)
		{
		INT16 nLayer = m_pAgpmGrid->GetLayerByIndex(&pAgpdGuildWarehouse->m_WarehouseGrid, lIndex);
		INT16 nRow = m_pAgpmGrid->GetRowByIndex(&pAgpdGuildWarehouse->m_WarehouseGrid, lIndex);
		INT16 nColumn = m_pAgpmGrid->GetColumnByIndex(&pAgpdGuildWarehouse->m_WarehouseGrid, lIndex);
		INT16 nWidth = pAgpdItem->m_pcsItemTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_WIDTH];
		INT16 nHeight = pAgpdItem->m_pcsItemTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_HEIGHT];

		m_pAgpmGrid->Clear(&pAgpdGuildWarehouse->m_WarehouseGrid,
						   nLayer,
						   nRow,
						   nColumn,
						   nHeight,
						   nWidth						  						   
						   );

		pAgpdItem->m_pcsGuildWarehouseBase	= NULL;
		}

	//pAgpdItem->m_eStatus = AGPDITEM_STATUS_NONE;

	EnumCallback(AGPMGUILDWAREHOUSE_CB_REMOVE_ITEM, pAgpdGuildWarehouse, pAgpdItem);
	
	return TRUE;
	}


BOOL AgpmGuildWarehouse::RemoveItem(AgpdGuild *pAgpdGuild, AgpdItem *pAgpdItem)
	{
	return RemoveItem(GetGuildWarehouse(pAgpdGuild), pAgpdItem);
	}


BOOL AgpmGuildWarehouse::AddMoney(AgpdGuildWarehouse *pAgpdGuildWarehouse, INT64 llMoney)
	{
	if (!pAgpdGuildWarehouse || 0 > llMoney)
		return FALSE;
	
	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;

	pAgpdGuildWarehouse->m_llMoney += llMoney;
	
	EnumCallback(AGPMGUILDWAREHOUSE_CB_UPDATE_MONEY, pAgpdGuildWarehouse, &llMoney);
	
	return TRUE;
	}

	
BOOL AgpmGuildWarehouse::AddMoney(AgpdGuild *pAgpdGuild, INT64 llMoney)
	{
	return AddMoney(GetGuildWarehouse(pAgpdGuild), llMoney);
	}


BOOL AgpmGuildWarehouse::AddMoney(TCHAR *pszGuild, INT64 llMoney)
	{
	return AddMoney(m_pAgpmGuild->GetGuild(pszGuild), llMoney);
	}


BOOL AgpmGuildWarehouse::SubMoney(AgpdGuildWarehouse *pAgpdGuildWarehouse, INT64 llMoney)
	{
	if (!pAgpdGuildWarehouse || 0 > llMoney)
		return FALSE;
	
	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	if (llMoney > pAgpdGuildWarehouse->m_llMoney)
		return FALSE;

	pAgpdGuildWarehouse->m_llMoney -= llMoney;
	
	INT64 llDiff = -llMoney;
	EnumCallback(AGPMGUILDWAREHOUSE_CB_UPDATE_MONEY, pAgpdGuildWarehouse, &llDiff);
	
	return TRUE;
	}


BOOL AgpmGuildWarehouse::SubMoney(AgpdGuild *pAgpdGuild, INT64 llMoney)
	{
	return SubMoney	(GetGuildWarehouse(pAgpdGuild), llMoney);
	}


BOOL AgpmGuildWarehouse::SubMoney(TCHAR *pszGuild, INT64 llMoney)
	{
	return SubMoney(m_pAgpmGuild->GetGuild(pszGuild), llMoney);
	}


BOOL AgpmGuildWarehouse::ClearAllItems(AgpdGuildWarehouse *pAgpdGuildWarehouse)
	{
	// this call has no effect to Database, just remove items in memory and reset grid
	
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	AuAutoLock Lock(pAgpdGuildWarehouse->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	for (int i = 0; i < pAgpdGuildWarehouse->m_WarehouseGrid.m_nLayer; ++i)
		{
		for (int j = 0; j < pAgpdGuildWarehouse->m_WarehouseGrid.m_nColumn; ++j)
			{
			for (int k = 0; k < pAgpdGuildWarehouse->m_WarehouseGrid.m_nRow; ++k)
				{
				AgpdGridItem *pAgpdGridItem = m_pAgpmGrid->GetItem(&pAgpdGuildWarehouse->m_WarehouseGrid, i, k, j);
				if (pAgpdGridItem)
					{
					AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem);
					m_pAgpmItem->RemoveItem(pAgpdItem);
					}
				}
			}
		}
	
	m_pAgpmGrid->Reset(&pAgpdGuildWarehouse->m_WarehouseGrid);
	
	return TRUE;
	}


BOOL AgpmGuildWarehouse::ClearAllItems(AgpdCharacter *pAgpdCharacter)
	{
	return ClearAllItems(GetGuildWarehouse(pAgpdCharacter));
	}




//	Validation
//===================================================
//
BOOL AgpmGuildWarehouse::CheckPreviledge(AgpdCharacter *pAgpdCharacter, AgpdGuild *pAgpdGuild, eAGPDGUILDWAREHOUSE_PREV ePrev)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	if (!pAgpdGuild)
		pAgpdGuild = m_pAgpmGuild->GetGuild(pAgpdCharacter);

	AgpdGuildWarehouse *pAgpdGuildWarehouse = GetGuildWarehouse(pAgpdGuild);
	if (!pAgpdGuildWarehouse)
		return FALSE;
	
	BOOL bMaster = m_pAgpmGuild->IsMaster(pAgpdGuild, pAgpdCharacter->m_szID);
	
	BOOL bResult = FALSE;
	
	if (bMaster)
		bResult = pAgpdGuildWarehouse->GetMasterPreviledge(ePrev);
	else
		bResult = pAgpdGuildWarehouse->GetMemberPreviledge(ePrev);

	return bResult;
	}


BOOL AgpmGuildWarehouse::CheckPreviledge(AgpdCharacter *pAgpdCharacter, eAGPDGUILDWAREHOUSE_PREV ePrev)
	{
	return CheckPreviledge(pAgpdCharacter, m_pAgpmGuild->GetGuild(pAgpdCharacter), ePrev);
	}



//	Module Callbacks
//===================================================
//



//	Callback setting
//===================================================
//
BOOL AgpmGuildWarehouse::SetCallbackBaseInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_BASE_INFO, pfCallback, pClass);
	}


BOOL AgpmGuildWarehouse::SetCallbackOpen(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_OPEN, pfCallback, pClass);
	}


BOOL AgpmGuildWarehouse::SetCallbackClose(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_CLOSE, pfCallback, pClass);
	}


BOOL AgpmGuildWarehouse::SetCallbackMoneyIn(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_MONEY_IN, pfCallback, pClass);
	}


BOOL AgpmGuildWarehouse::SetCallbackMoneyOut(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_MONEY_OUT, pfCallback, pClass);
	}


BOOL AgpmGuildWarehouse::SetCallbackItemIn(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_ITEM_IN, pfCallback, pClass);
	}


BOOL AgpmGuildWarehouse::SetCallbackItemOut(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_ITEM_OUT, pfCallback, pClass);
	}


BOOL AgpmGuildWarehouse::SetCallbackExpand(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_EXPAND, pfCallback, pClass);
	}


BOOL AgpmGuildWarehouse::SetCallbackUpdateMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_UPDATE_MONEY, pfCallback, pClass);
	}


BOOL AgpmGuildWarehouse::SetCallbackAddItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)	
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_ADD_ITEM, pfCallback, pClass);
	}


BOOL AgpmGuildWarehouse::SetCallbackRemoveItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMGUILDWAREHOUSE_CB_REMOVE_ITEM, pfCallback, pClass);
	}


