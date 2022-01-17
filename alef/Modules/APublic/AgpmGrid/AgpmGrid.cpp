/******************************************************************************
Module:  AgpmGrid.cpp
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2002. 09. 4
******************************************************************************/

#include "AgpmGrid.h"

#include <stdio.h>
//#include "AgpmItem.h"
#include "AgpdItem.h"
#include "AgpdItemTemplate.h"
#include "AgpmItem.h"
#include "ApMemoryTracker.h"

AgpmGrid::AgpmGrid()
{
	SetModuleName("AgpmGrid");
	
	// setting module data
	SetModuleData( sizeof(AgpdGridItem), AGPMGRID_DATA_GRIDITEM );
	m_bUseMemoryPool = FALSE;
}

AgpmGrid::~AgpmGrid()
{
}

BOOL AgpmGrid::Init( AgpdGrid *pcsAgpdGrid, EnumGridType eGridType )
{
	ASSERT(NULL != pcsAgpdGrid);
	if( pcsAgpdGrid == NULL )
		return FALSE;

	if (pcsAgpdGrid->m_lGridCount > 0)
		Remove(pcsAgpdGrid);

	// Memory Pooling을 이용
	switch(eGridType)
	{
	case AGPDGRID_TYPE_BANK:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_BANK_MAX_LAYER, AGPMITEM_BANK_ROW, AGPMITEM_BANK_COLUMN, 
										pcsAgpdGrid, &m_BankGridItem, &m_BankLockTable);
		}
		break;

	case AGPDGRID_TYPE_INVENTORY:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_MAX_INVENTORY, AGPMITEM_INVENTORY_ROW, AGPMITEM_INVENTORY_COLUMN, 
										pcsAgpdGrid, &m_InventoryGridItem, &m_InventoryLockTable);
		}
		break;

	case AGPDGRID_TYPE_EQUIP:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_EQUIP_LAYER, AGPMITEM_EQUIP_ROW, AGPMITEM_EQUIP_COLUMN, 
										pcsAgpdGrid, &m_EquipGridItem, &m_EquipLockTable);
		}
		break;
		
	case AGPDGRID_TYPE_TRADEBOX:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_TRADEBOX_LAYER, AGPMITEM_TRADEBOX_ROW, AGPMITEM_TRADEBOX_COLUMN, 
										pcsAgpdGrid, &m_TradeGridItem, &m_TradeLockTable);
		}
		break;

	case AGPDGRID_TYPE_SALES:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_SALES_LAYER, AGPMITEM_SALES_ROW, AGPMITEM_SALES_COLUMN, 
										pcsAgpdGrid, &m_SalesGridItem, &m_SalesLockTable);
		}
		break;

	case AGPDGRID_TYPE_NPCTRADEBOX:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_NPCTRADEBOX_LAYER, AGPMITEM_NPCTRADEBOX_ROW, AGPMITEM_NPCTRADEBOX_COLUMN, 
										pcsAgpdGrid, &m_NPCTradeGridItem, &m_NPCTradeLockTable);
		}
		break;

	case AGPDGRID_TYPE_QUEST:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_QUEST_LAYER, AGPMITEM_QUEST_ROW, AGPMITEM_QUEST_COLUMN, 
										pcsAgpdGrid, &m_QuestGridItem, &m_QuestLockTable);
		}
		break;
	case AGPDGRID_TYPE_CASH_INVENTORY:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_CASH_INVENTORY_LAYER, AGPMITEM_CASH_INVENTORY_ROW, AGPMITEM_CASH_INVENTORY_COLUMN, 
										pcsAgpdGrid, &m_CashInventoryGridItem, &m_CashInventoryLockTable);
		}
		break;
	case AGPDGRID_TYPE_GUILD_WAREHOUSE:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_GUILD_WAREHOUSE_LAYER, AGPMITEM_GUILD_WAREHOUSE_ROW, AGPMITEM_GUILD_WAREHOUSE_COLUMN, 
										pcsAgpdGrid, &m_GuildWarehouseGridItem, &m_GuildWarehouseLockTable);
		}
		break;
	case AGPDGRID_TYPE_SUB_INVENTORY:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_SUB_LAYER, AGPMITEM_SUB_ROW, AGPMITEM_SUB_COLUMN, 
										pcsAgpdGrid, &m_SubInventoryGridItem, &m_SubInventoryLockTable);
		}
		break;
/*
	case AGPDGRID_TYPE_UNSEEN_INVENTORY:
		{
			return CreateByMemoryPool(eGridType, AGPMITEM_UNSEEN_LAYER, AGPMITEM_UNSEEN_ROW, AGPMITEM_UNSEEN_COLUMN,
										pcsAgpdGrid, &m_UnseenInventoryGridItem, &m_UnseenInventoryLockTable);
		}
	break;
*/
	default:
		{
			ASSERT(!__FUNCTION__);
		}
	};

	return FALSE;
}

BOOL AgpmGrid::Init( AgpdGrid *pcsAgpdGrid, INT16 nLayer, INT16 nRow, INT16 nColumn )
{
	// Memory Pool을 이용하지 않는 Grid는 여기서 모두 처리한다.
	ASSERT(0 != nLayer);
	ASSERT(0 != nRow);
	ASSERT(0 != nColumn);
	ASSERT(NULL != pcsAgpdGrid);

	INT32 lGridCount = nLayer * nRow * nColumn;
	pcsAgpdGrid->m_ppcGridData	= new AgpdGridItem *[lGridCount];
	pcsAgpdGrid->m_pbLockTable	= new BOOL [lGridCount];

	ASSERT(NULL != pcsAgpdGrid->m_ppcGridData);
	ASSERT(NULL != pcsAgpdGrid->m_pbLockTable);

	if (NULL == pcsAgpdGrid->m_ppcGridData)
		return FALSE;

	if (NULL == pcsAgpdGrid->m_pbLockTable)
		return FALSE;

	pcsAgpdGrid->m_eGridType		= AGPDGRID_TYPE_NONE;
	pcsAgpdGrid->m_lGridCount		= lGridCount;
	pcsAgpdGrid->m_nLayer			= nLayer;
	pcsAgpdGrid->m_nRow				= nRow;
	pcsAgpdGrid->m_nColumn			= nColumn;
	pcsAgpdGrid->m_nMaxRowExistItem = 0;

	memset(pcsAgpdGrid->m_ppcGridData,0,sizeof(AgpdGridItem *) * lGridCount);
	memset(pcsAgpdGrid->m_pbLockTable,0,sizeof(BOOL) * lGridCount);

	pcsAgpdGrid->m_pcsSelectedGridItem	= NULL;

	return TRUE;
}

BOOL AgpmGrid::CreateByMemoryPool(EnumGridType eGridType, INT32 nLayer, INT32 nRow, INT32 nColumn, AgpdGrid *pcsAgpdGrid,
								  ApMemoryPool* pPoolGridItem, ApMemoryPool* pPoolLockTable)
{
	ASSERT(0 != nLayer);
	ASSERT(0 != nRow);
	ASSERT(0 != nColumn);
	ASSERT(NULL != pcsAgpdGrid);
	ASSERT(NULL != pPoolGridItem);
	ASSERT(NULL != pPoolLockTable);

	INT32 lGridCount = nLayer * nRow * nColumn;
	if (m_bUseMemoryPool)
	{
		pcsAgpdGrid->m_ppcGridData = (AgpdGridItem**)pPoolGridItem->Alloc();
		pcsAgpdGrid->m_pbLockTable = (BOOL*)pPoolLockTable->Alloc();
	}
	else
	{
		pcsAgpdGrid->m_ppcGridData	= new AgpdGridItem *[lGridCount];
		pcsAgpdGrid->m_pbLockTable	= new BOOL [lGridCount];
	}

	ASSERT(NULL != pcsAgpdGrid->m_ppcGridData);
	ASSERT(NULL != pcsAgpdGrid->m_pbLockTable);

	if (NULL == pcsAgpdGrid->m_ppcGridData)
		return FALSE;

	if (NULL == pcsAgpdGrid->m_pbLockTable)
		return FALSE;

	pcsAgpdGrid->m_eGridType		= eGridType;
	pcsAgpdGrid->m_lGridCount		= lGridCount;
	pcsAgpdGrid->m_nLayer			= nLayer;
	pcsAgpdGrid->m_nRow				= nRow;
	pcsAgpdGrid->m_nColumn			= nColumn;
	pcsAgpdGrid->m_nMaxRowExistItem = 0;

	memset(pcsAgpdGrid->m_ppcGridData,0,sizeof(AgpdGridItem *) * lGridCount);
	memset(pcsAgpdGrid->m_pbLockTable,0,sizeof(BOOL) * lGridCount);

	pcsAgpdGrid->m_pcsSelectedGridItem	= NULL;
	return TRUE;
}

BOOL AgpmGrid::DeleteByMemoryPool(AgpdGrid *pcsAgpdGrid, ApMemoryPool* pPoolGridItem, ApMemoryPool* pPoolLockTable)
{
	ASSERT(NULL != pcsAgpdGrid);
	ASSERT(NULL != pcsAgpdGrid->m_ppcGridData);
	ASSERT(NULL != pPoolGridItem);
	ASSERT(NULL != pPoolLockTable);

	if (m_bUseMemoryPool)
	{
		if (NULL != pcsAgpdGrid->m_ppcGridData)
			pPoolGridItem->Free(pcsAgpdGrid->m_ppcGridData);

		if (NULL != pcsAgpdGrid->m_pbLockTable)
			pPoolLockTable->Free(pcsAgpdGrid->m_pbLockTable);
	}
	else
	{
		if (NULL != pcsAgpdGrid->m_ppcGridData)
			delete [] pcsAgpdGrid->m_ppcGridData;

		if (NULL != pcsAgpdGrid->m_pbLockTable)
			delete [] pcsAgpdGrid->m_pbLockTable;
	}

	pcsAgpdGrid->Clear();
	return TRUE;
}

void AgpmGrid::InitGridMemoryPool(INT32 lPoolCount)
{
	INT32 lGridCount;
	lGridCount = AGPMITEM_BANK_MAX_LAYER * AGPMITEM_BANK_ROW * AGPMITEM_BANK_COLUMN;
	m_BankGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount, "m_BankGridItem");
	m_BankLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount, "m_BankLockTable");

	lGridCount = AGPMITEM_MAX_INVENTORY * AGPMITEM_INVENTORY_ROW * AGPMITEM_INVENTORY_COLUMN;
	m_InventoryGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount, "m_InventoryGridItem");
	m_InventoryLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount, "m_InventoryLockTable");

	lGridCount = AGPMITEM_EQUIP_LAYER * AGPMITEM_EQUIP_ROW * AGPMITEM_EQUIP_COLUMN;
	m_EquipGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount, "m_EquipGridItem");
	m_EquipLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount, "m_EquipLockTable");

	lGridCount = AGPMITEM_TRADEBOX_LAYER * AGPMITEM_TRADEBOX_ROW * AGPMITEM_TRADEBOX_COLUMN;
	m_TradeGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount * 2, "m_TradeGridItem");
	m_TradeLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount * 2, "m_TradeLockTable");

	lGridCount = AGPMITEM_SALES_LAYER * AGPMITEM_SALES_ROW * AGPMITEM_SALES_COLUMN;
	m_SalesGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount, "m_SalesGridItem");
	m_SalesLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount, "m_SalesLockTable");

	lGridCount = AGPMITEM_NPCTRADEBOX_LAYER * AGPMITEM_NPCTRADEBOX_ROW * AGPMITEM_NPCTRADEBOX_COLUMN;
	m_NPCTradeGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount, "m_NPCTradeGridItem");
	m_NPCTradeLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount, "m_NPCTradeLockTable");

	lGridCount = AGPMITEM_QUEST_LAYER * AGPMITEM_QUEST_ROW * AGPMITEM_QUEST_COLUMN;
	m_QuestGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount, "m_QuestGridItem");
	m_QuestLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount, "m_QuestLockTable");

	lGridCount = AGPMITEM_CASH_INVENTORY_LAYER * AGPMITEM_CASH_INVENTORY_ROW * AGPMITEM_CASH_INVENTORY_COLUMN;
	m_CashInventoryGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount, "m_CashInventoryGridItem");
	m_CashInventoryLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount, "m_CashInventoryLockTable");

	lGridCount = AGPMITEM_GUILD_WAREHOUSE_LAYER * AGPMITEM_GUILD_WAREHOUSE_ROW * AGPMITEM_GUILD_WAREHOUSE_COLUMN;
	m_GuildWarehouseGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount, "m_GuildWarehouseGridItem");
	m_GuildWarehouseLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount, "m_GuildWarehouseLockTable");

	lGridCount = AGPMITEM_SUB_LAYER * AGPMITEM_SUB_ROW * AGPMITEM_SUB_COLUMN;
	m_SubInventoryGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount, "m_SubInventoryGridItem");
	m_SubInventoryLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount, "m_SubInventoryLockTable");

	/*
	lGridCount = AGPMITEM_UNSEEN_LAYER * AGPMITEM_UNSEEN_ROW * AGPMITEM_UNSEEN_COLUMN;
	m_UnseenInventoryGridItem.Initialize(lGridCount * sizeof(AgpdGridItem*), lPoolCount, "m_UnseenInventoryGridItem");
	m_UnseenInventoryLockTable.Initialize(lGridCount * sizeof(BOOL), lPoolCount, "m_UnseenInventoryLockTable");
	*/

	m_bUseMemoryPool = TRUE;
}

INT16 AgpmGrid::GetLayer( AgpdGrid *pcsAgpdGrid )
{
	return pcsAgpdGrid->m_nLayer;
}

INT16 AgpmGrid::GetRow( AgpdGrid *pcsAgpdGrid )
{
	return pcsAgpdGrid->m_nRow;
}

INT16 AgpmGrid::GetColumn( AgpdGrid *pcsAgpdGrid )
{
	return pcsAgpdGrid->m_nColumn;
}

INT16 AgpmGrid::GetLayerByIndex( AgpdGrid *pcsAgpdGrid, INT32 lIndex )
{
	if (!pcsAgpdGrid || pcsAgpdGrid->m_nRow * pcsAgpdGrid->m_nColumn == 0)
		return 0;

	INT16	nLayer = lIndex / ( pcsAgpdGrid->m_nRow * pcsAgpdGrid->m_nColumn );

	return nLayer;
}

INT16 AgpmGrid::GetRowByIndex( AgpdGrid *pcsAgpdGrid, INT32 lIndex )
{
	if (!pcsAgpdGrid || pcsAgpdGrid->m_nRow * pcsAgpdGrid->m_nColumn == 0 || pcsAgpdGrid->m_nColumn == 0)
		return 0;

	INT16	nRow = (lIndex % ( pcsAgpdGrid->m_nRow * pcsAgpdGrid->m_nColumn )) / pcsAgpdGrid->m_nColumn;

	return nRow;
}

INT16 AgpmGrid::GetColumnByIndex( AgpdGrid *pcsAgpdGrid, INT32 lIndex )
{
	if (!pcsAgpdGrid || pcsAgpdGrid->m_nRow * pcsAgpdGrid->m_nColumn == 0 || pcsAgpdGrid->m_nColumn == 0)
		return 0;

	INT16	nColumn =  (lIndex % ( pcsAgpdGrid->m_nRow * pcsAgpdGrid->m_nColumn )) % pcsAgpdGrid->m_nColumn;

	return nColumn;
}

INT32 AgpmGrid::GetItemCount( AgpdGrid *pcsAgpdGrid )
{
	return pcsAgpdGrid->m_lItemCount;
}

AgpdGridItem *AgpmGrid::GetItem( AgpdGrid *pcsAgpdGrid, INT16 nLayer, INT16 nRow, INT16 nColumn )
{
	AgpdGridItem	*pcsGridItem = NULL;

	if( pcsAgpdGrid != NULL )
	{
		INT32			lIndex;

		if( IsInGrid( pcsAgpdGrid, nLayer, nRow, nColumn, 1, 1 ) == TRUE )
		{
			lIndex = nLayer*pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn;
			lIndex += (nRow*pcsAgpdGrid->m_nColumn);
			lIndex += nColumn;

			if (lIndex >= pcsAgpdGrid->m_lGridCount)
				return NULL;

			pcsGridItem = pcsAgpdGrid->m_ppcGridData[lIndex];
		}
	}

	return pcsGridItem;
}

AgpdGridItem *AgpmGrid::GetItem( AgpdGrid *pcsAgpdGrid, INT32 lIndex )
{
	AgpdGridItem	*pcsGridItem;

	pcsGridItem = NULL;

	if( pcsAgpdGrid != NULL )
	{
		if( ( 0 <= lIndex ) && ( lIndex < pcsAgpdGrid->m_lGridCount ) )
		{
			pcsGridItem = pcsAgpdGrid->m_ppcGridData[lIndex];
		}
	}

	return pcsGridItem;
}

BOOL AgpmGrid::Remove( AgpdGrid *pcsAgpdGrid )
{
	if( NULL == pcsAgpdGrid )
		return FALSE;

	switch (pcsAgpdGrid->m_eGridType)
	{
	case AGPDGRID_TYPE_BANK:		return DeleteByMemoryPool(pcsAgpdGrid, &m_BankGridItem, &m_BankLockTable);
	case AGPDGRID_TYPE_INVENTORY:	return DeleteByMemoryPool(pcsAgpdGrid, &m_InventoryGridItem, &m_InventoryLockTable);
	case AGPDGRID_TYPE_EQUIP:		return DeleteByMemoryPool(pcsAgpdGrid, &m_EquipGridItem, &m_EquipLockTable);
	case AGPDGRID_TYPE_TRADEBOX:	return DeleteByMemoryPool(pcsAgpdGrid, &m_TradeGridItem, &m_TradeLockTable);
	case AGPDGRID_TYPE_SALES:		return DeleteByMemoryPool(pcsAgpdGrid, &m_SalesGridItem, &m_SalesLockTable);
	case AGPDGRID_TYPE_NPCTRADEBOX:	return DeleteByMemoryPool(pcsAgpdGrid, &m_NPCTradeGridItem, &m_NPCTradeLockTable);
	case AGPDGRID_TYPE_QUEST:		return DeleteByMemoryPool(pcsAgpdGrid, &m_QuestGridItem, &m_QuestLockTable);
	case AGPDGRID_TYPE_CASH_INVENTORY:	return DeleteByMemoryPool(pcsAgpdGrid, &m_CashInventoryGridItem, &m_CashInventoryLockTable);
	case AGPDGRID_TYPE_GUILD_WAREHOUSE:	return DeleteByMemoryPool(pcsAgpdGrid, &m_GuildWarehouseGridItem, &m_GuildWarehouseLockTable);
	case AGPDGRID_TYPE_SUB_INVENTORY:	return DeleteByMemoryPool(pcsAgpdGrid, &m_SubInventoryGridItem, &m_SubInventoryLockTable);
//	case AGPDGRID_TYPE_UNSEEN_INVENTORY:	return DeleteByMemoryPool(pcsAgpdGrid, &m_UnseenInventoryGridItem, &m_UnseenInventoryLockTable);
	case AGPDGRID_TYPE_NONE:
		{
			if (NULL != pcsAgpdGrid->m_ppcGridData)
				delete [] pcsAgpdGrid->m_ppcGridData;

			if (NULL != pcsAgpdGrid->m_pbLockTable)
				delete [] pcsAgpdGrid->m_pbLockTable;

			pcsAgpdGrid->Clear();
			return TRUE;
		}

	default:
		{
			ASSERT(!__FUNCTION__);
		}
	};

	return TRUE;
}

BOOL AgpmGrid::IsExistItem( AgpdGrid *pcsAgpdGrid, AgpdGridItemType eType, INT32 lItemID, INT32 *plIndex )
{
	BOOL			bResult;
	AgpdGridItem *	pcsGridItem;

	bResult = FALSE;

	if( pcsAgpdGrid != NULL && pcsAgpdGrid->m_ppcGridData != NULL )
	{
		for( int i=0; i<pcsAgpdGrid->m_lGridCount; ++i )
		{
			pcsGridItem = pcsAgpdGrid->m_ppcGridData[i];

			if( pcsGridItem && pcsGridItem->m_eType == eType && pcsGridItem->m_lItemID == lItemID )
			{
				bResult = TRUE;
				if (plIndex)
					*plIndex = i;
				break;
			}
		}
	}

	return bResult;
}

BOOL AgpmGrid::IsExistItemTemplate( AgpdGrid *pcsAgpdGrid, AgpdGridItemType eType, INT32 lItemTID )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdGrid != NULL )
	{
		if (GetItemByTemplate( pcsAgpdGrid, eType, lItemTID) != NULL)
			bResult = TRUE;
	}

	return bResult;
}

AgpdGridItem *AgpmGrid::GetItemByTemplate( AgpdGrid *pcsAgpdGrid, AgpdGridItemType eType, INT32 lItemTID )
{
	AgpdGridItem	*pcsAgpdGridItem;

	pcsAgpdGridItem = NULL;

	if( pcsAgpdGrid != NULL )
	{
		for( int i=0; i<pcsAgpdGrid->m_lGridCount; ++i )
		{
			pcsAgpdGridItem = pcsAgpdGrid->m_ppcGridData[i];

			if( pcsAgpdGridItem && pcsAgpdGridItem->m_eType == eType && pcsAgpdGridItem->m_lItemTID == lItemTID )
			{
				return pcsAgpdGridItem;
			}
		}
	}

	return NULL;
}

AgpdGridItem *AgpmGrid::GetQuestItemByTemplate( AgpdGrid *pcsAgpdGrid, AgpdGridItemType eType, INT32 lItemTID )
{
	AgpdGridItem	*pcsAgpdGridItem;

	pcsAgpdGridItem = NULL;

	if( pcsAgpdGrid != NULL )
	{
		for( int i=0; i<pcsAgpdGrid->m_lGridCount; ++i )
		{
			pcsAgpdGridItem = pcsAgpdGrid->m_ppcGridData[i];

			if( pcsAgpdGridItem && pcsAgpdGridItem->m_eType == eType && pcsAgpdGridItem->m_lItemTID == lItemTID )
			{
				AgpmItem* a_pcsAgpmItem = (AgpmItem*)GetModule("AgpmItem");
				AgpdItem* a_pditem = a_pcsAgpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
				if( a_pcsAgpmItem->IsQuestItem( a_pditem ) )
					return pcsAgpdGridItem;
			}
		}
	}

	return NULL;
}

// 그리드에서 인덱스와 타입과 TID에 매칭되는 GridItem을 찾는다. supertj@20100407
AgpdGridItem* AgpmGrid::GetItemByIndexAndTID( INT32 &Index, AgpdGrid *pcsAgpdGrid, AgpdGridItemType eType, INT32 lItemTID )
{
	AgpdGridItem* pcsAgpdGridItem = NULL;

	if(pcsAgpdGrid)
	{
		if(NULL == pcsAgpdGrid->m_ppcGridData)
			return NULL;
		
		if( Index > pcsAgpdGrid->m_lGridCount ) return NULL;

		pcsAgpdGridItem = pcsAgpdGrid->m_ppcGridData[Index];

		if( pcsAgpdGridItem )
			if( pcsAgpdGridItem->m_eType == eType )
				if( pcsAgpdGridItem->m_lItemTID == lItemTID )
					return pcsAgpdGridItem;
	}

	return NULL;
}

AgpdGridItem *AgpmGrid::GetItemByTemplate( INT32 &lIndex, AgpdGrid *pcsAgpdGrid, AgpdGridItemType eType, INT32 lItemTID )
{
	AgpdGridItem* pcsAgpdGridItem = NULL;

	if( pcsAgpdGrid != NULL )
	{
		for( ; lIndex<pcsAgpdGrid->m_lGridCount; lIndex++ )
		{
			// NULL Check 넣음. 2004.03.18. steeple
			if(pcsAgpdGrid->m_ppcGridData == NULL)
				break;
			
			pcsAgpdGridItem = pcsAgpdGrid->m_ppcGridData[lIndex];

			if( pcsAgpdGridItem && pcsAgpdGridItem->m_eType == eType 
				&& pcsAgpdGridItem->m_lItemTID == lItemTID )
			{
				//다음번을 위하여 인덱스를 하나 증가 시킨다.
				lIndex++;

				return pcsAgpdGridItem;

				break;
			}
		}
	}

	return NULL;
}

BOOL AgpmGrid::AddItem( AgpdGrid *pcsGrid, AgpdGridItem *pcsGridItem )
{
	pcsGrid->m_lItemCount++;

	return TRUE;
}

BOOL AgpmGrid::DeleteItem( AgpdGrid *pcsGrid, AgpdGridItem *pcsGridItem )
{
	pcsGrid->m_lItemCount--;

	return TRUE;
}

BOOL AgpmGrid::IsInGrid( AgpdGrid *pcsAgpdGrid, INT16 nLayer, INT16 nRow, INT16 nColumn, INT16 nHeight, INT16 nWidth )
{
	BOOL			bResult = FALSE;

	if( pcsAgpdGrid != NULL )
	{
		//위치 체크 Part 1
		if( (0 <= nLayer) && (nLayer < pcsAgpdGrid->m_nLayer) && (0 <= nRow) && (nRow < pcsAgpdGrid->m_nRow) && (0 <= nColumn) && (nColumn < pcsAgpdGrid->m_nColumn) )
		{
			//위치체크 Part 2
			if( (0 <= nWidth) && ( (nColumn+nWidth) <= pcsAgpdGrid->m_nColumn ) && ( 0 <= nHeight ) && ( (nRow+nHeight) <= pcsAgpdGrid->m_nRow) )
			{
				//여기까지 왔다면 그리드 내부가 맞다.
				bResult = TRUE;
			}
		}
	}

	return bResult;
}

//지정한 공간을 인벤 전체에서 찾아본다.
BOOL AgpmGrid::GetEmptyGrid( AgpdGrid *pcsAgpdGrid, INT16 *pnLayer, INT16 *pnRow, INT16 *pnColumn, INT16 nHeight, INT16 nWidth )
{
	return GetEmptyGridWithStartPos(pcsAgpdGrid, 0, 0, 0, pnLayer, pnRow, pnColumn, nHeight, nWidth);

	/*
	BOOL			bResult = FALSE;

	if( pcsAgpdGrid != NULL )
	{
		//넣고자 하는 블럭이 인벤에 들어갈수 있는 사이즈라면....
		if( IsInGrid( pcsAgpdGrid, 0, 0, 0, nHeight, nWidth ) == TRUE )
		{
			int				iStartOfLayer;
			int				iStartOfRow;

			int		limit1 = pcsAgpdGrid->m_nColumn-nWidth; //y
			int		limit2 = pcsAgpdGrid->m_nRow-nHeight;	//x

			for( int iLayer=0; iLayer<pcsAgpdGrid->m_nLayer; ++iLayer )
			{
				iStartOfLayer = iLayer*(pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn);

				for( int iRow=0; iRow<limit2; ++iRow )
				{
					iStartOfRow = iStartOfLayer + (iRow*pcsAgpdGrid->m_nColumn);

					for( int iColumn=0; iColumn<limit1; ++iColumn )
					{
						//빈칸을 찾았소이다!!
						if( !pcsAgpdGrid->m_pbLockTable[iStartOfRow + iColumn] && pcsAgpdGrid->m_ppcGridData[iStartOfRow + iColumn] == NULL )
						{
							BOOL			bFound;
							int				iStartOfWidth;

							bFound = TRUE;

							for( int iHeight=0; iHeight<nHeight; ++iHeight )
							{
								iStartOfWidth = iStartOfRow + iHeight*pcsAgpdGrid->m_nColumn + iColumn;

								for( int iWidth=0; iWidth<nWidth; ++iWidth )
								{
									if( !pcsAgpdGrid->m_pbLockTable[iStartOfWidth + iWidth] && pcsAgpdGrid->m_ppcGridData[iStartOfWidth+iWidth] != NULL )
									{
										bFound = FALSE;
										break;
									}
								}

								if( bFound == FALSE )
									break;
							}

							if( bFound == TRUE )
							{
								*pnLayer = iLayer;
								*pnRow = iRow;
								*pnColumn = iColumn;

								return bFound;
							}
						}
					}
				}
			}
		}
	}

	return bResult;
	*/
}

//지정한 위치에서 지정한 공간을 인벤 전체에서 찾아본다.
BOOL AgpmGrid::GetEmptyGridWithStartPos( AgpdGrid *pcsAgpdGrid, INT16 nStartLayer, INT16 nStartRow, INT16 nStartColumn, INT16 *pnLayer, INT16 *pnRow, INT16 *pnColumn, INT16 nHeight, INT16 nWidth )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdGrid != NULL )
	{
		//넣고자 하는 블럭이 인벤에 들어갈수 있는 사이즈라면....
		if( IsInGrid( pcsAgpdGrid, nStartLayer, nStartRow, nStartColumn, nHeight, nWidth ) == TRUE )
		{
			int				iStartOfLayer;
			int				iStartOfRow;

			for( int iLayer=nStartLayer; iLayer<pcsAgpdGrid->m_nLayer; ++iLayer )
			{
				iStartOfLayer = iLayer*(pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn);

				for( int iRow=nStartRow; iRow<=pcsAgpdGrid->m_nRow-nHeight; ++iRow )
				{
					iStartOfRow = iStartOfLayer + (iRow*pcsAgpdGrid->m_nColumn);

					for( int iColumn=nStartColumn; iColumn<=pcsAgpdGrid->m_nColumn-nWidth; ++iColumn )
					{
						//빈칸을 찾았소이다!!
						if( !pcsAgpdGrid->m_pbLockTable[iStartOfRow+iColumn] && pcsAgpdGrid->m_ppcGridData[iStartOfRow + iColumn] == NULL )
						{
							BOOL			bFound;
							int				iStartOfWidth;

							bFound = TRUE;

							for( int iHeight=0; iHeight<nHeight; ++iHeight )
							{
								iStartOfWidth = iStartOfRow + iHeight*pcsAgpdGrid->m_nColumn + iColumn;

								for( int iWidth=0; iWidth<nWidth; ++iWidth )
								{
									if( !pcsAgpdGrid->m_pbLockTable[iStartOfWidth+iWidth] && pcsAgpdGrid->m_ppcGridData[iStartOfWidth+iWidth] != NULL )
									{
										bFound = FALSE;
										break;
									}
								}

								if( bFound == FALSE )
									break;
							}

							if( bFound == TRUE )
							{
								*pnLayer = iLayer;
								*pnColumn = iColumn;
								*pnRow = iRow;

								return bFound;
							}
						}
					}
				}
			}
		}
	}

	return bResult;
}


//지정한 위치에 넣을수 있는지 확인한다.
BOOL AgpmGrid::IsEmptyGrid( AgpdGrid *pcsAgpdGrid, INT16 nLayer, INT16 nRow, INT16 nColumn, INT16 nHeight, INT16 nWidth )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdGrid != NULL )
	{
		bResult = IsInGrid( pcsAgpdGrid, nLayer, nRow, nColumn, nHeight, nWidth );

		if( bResult == FALSE )
		{
			return bResult;
		}

		int			iStartLayer;
		int			iStartRow;

		iStartLayer = nLayer*pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn;

		for( int iHeight=0; iHeight<nHeight; ++iHeight )
		{
			iStartRow = iStartLayer + (nRow+iHeight)*pcsAgpdGrid->m_nColumn;

			for( int iWidth=0; iWidth<nWidth; ++iWidth )
			{
				if (pcsAgpdGrid->m_pbLockTable[iStartRow + nColumn + iWidth])
					return FALSE;

				if( pcsAgpdGrid->m_ppcGridData[iStartRow + nColumn + iWidth] != NULL )
				{
					return FALSE;
				}
			}
		}

		//여기까지 왔다면 넣을수 있다는 의미이다.
		bResult = TRUE;
	}

	return bResult;
}

// 2007.12.12. steeple
// 해당 그리드에 빈칸 개수를 구한다.
INT32 AgpmGrid::GetEmpyGridCount(AgpdGrid* pcsAgpdGrid)
{
	return (pcsAgpdGrid->m_lGridCount - pcsAgpdGrid->m_lItemCount);
}

//지정한 위치에 넣을수 있는지 확인한다.
BOOL AgpmGrid::IsMovalbeGrid( AgpdGrid *pcsAgpdGrid, INT16 nLayer, INT16 nRow, INT16 nColumn, AgpdGridItem *pcsGridItem, INT16 nHeight, INT16 nWidth )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdGrid != NULL )
	{
		bResult = IsInGrid( pcsAgpdGrid, nLayer, nRow, nColumn, nHeight, nWidth );

		if( bResult == FALSE )
		{
			return bResult;
		}

		int			iStartLayer;
		int			iStartRow;

		iStartLayer = nLayer*pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn;

		for( int iHeight=0; iHeight<nHeight; ++iHeight )
		{
			iStartRow = iStartLayer + (nRow+iHeight)*pcsAgpdGrid->m_nColumn;

			for( int iWidth=0; iWidth<nWidth; ++iWidth )
			{
				//0빈 슬롯이 아니고....
				if( pcsAgpdGrid->m_ppcGridData[iStartRow + nColumn + iWidth] != NULL )
				{
					//옮기려는 ID와 다르기까지 하다면? 옮길수 없다.
					if( pcsAgpdGrid->m_ppcGridData[iStartRow + nColumn + iWidth] != pcsGridItem )
					{
						return FALSE;
					}
				}
			}
		}

		//여기까지 왔다면 넣을수 있다는 의미이다.
		bResult = TRUE;
	}

	return bResult;
}

//그리드에 추가한다.
BOOL AgpmGrid::Add( AgpdGrid *pcsAgpdGrid, AgpdGridItem *pcsGridItem, INT16 nHeight, INT16 nWidth )
{
	BOOL			bResult;
	INT16			nLayer, nRow, nColumn;

	bResult = FALSE;

	if( pcsAgpdGrid != NULL )
	{
		bResult = GetEmptyGridWithStartPos( pcsAgpdGrid, 0, 0, 0, &nLayer, &nRow, &nColumn, nHeight, nWidth );

		if( bResult == TRUE )
		{
			int				iStartOfLayer;
			int				iStartOfRow;

			iStartOfLayer = nLayer * pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn;

			int				limit1 = nColumn+nWidth;
			int				limit2 = nRow+nHeight;

			for( int iHeight = nRow; iHeight < limit2; ++iHeight )
			{
				iStartOfRow = iStartOfLayer + (nRow) * pcsAgpdGrid->m_nColumn;

				for( int iWidth=nColumn; iWidth<limit1; ++iWidth )
				{
					//ItemID를 Grid에 복사한다.
					pcsAgpdGrid->m_ppcGridData[iStartOfRow+iWidth] = pcsGridItem;
				}
			}

			if (pcsAgpdGrid->m_nMaxRowExistItem < limit2 - 1)
				pcsAgpdGrid->m_nMaxRowExistItem = limit2 - 1;
		}
	}

	return bResult;
}

//그리드에 추가한다.
BOOL AgpmGrid::Add( AgpdGrid *pcsAgpdGrid, INT16 StartLayer, INT16 StartRow, INT16 StartColumn, INT16 *nLayer, INT16 *nRow, INT16 *nColumn, AgpdGridItem *pcsGridItem, INT16 nHeight, INT16 nWidth )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdGrid != NULL )
	{
		bResult = GetEmptyGridWithStartPos( pcsAgpdGrid, StartLayer, StartRow, StartColumn, nLayer, nRow, nColumn, nHeight, nWidth );

		if( bResult == TRUE )
		{
			int				iStartOfLayer;
			int				iStartOfRow;

			iStartOfLayer = (*nLayer)*(pcsAgpdGrid->m_nRow)*(pcsAgpdGrid->m_nColumn);

			int				limit1 = *nColumn+nWidth;
			int				limit2 = *nRow+nHeight;

//			for( int iHeight=*nRow; iHeight<limit2; ++iHeight )
//			{
//				iStartOfRow = iStartOfLayer + (iHeight)*pcsAgpdGrid->m_nColumn;
//
//				for( int iWidth=*nColumn; iWidth<limit1; ++iWidth )
//				{
//					if (pcsAgpdGrid->m_pbLockTable[iStartOfRow+iWidth])
//						return FALSE;
//				}
//			}

			for( int iHeight=*nRow; iHeight<limit2; ++iHeight )
			{
				iStartOfRow = iStartOfLayer + (iHeight)*pcsAgpdGrid->m_nColumn;

				for( int iWidth=*nColumn; iWidth<limit1; ++iWidth )
				{
					//ItemID를 Grid에 복사한다.
					pcsAgpdGrid->m_ppcGridData[iStartOfRow+iWidth] = pcsGridItem;
				}
			}

			if (pcsAgpdGrid->m_nMaxRowExistItem < limit2 - 1)
				pcsAgpdGrid->m_nMaxRowExistItem = limit2 - 1;
		}
	}

	return bResult;
}

//그리드에 추가한다.
BOOL AgpmGrid::Add( AgpdGrid *pcsAgpdGrid, INT16 nLayer, INT16 nRow, INT16 nColumn, AgpdGridItem *pcsGridItem, INT16 nHeight, INT16 nWidth )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdGrid != NULL )
	{
		bResult = IsEmptyGrid( pcsAgpdGrid, nLayer, nRow, nColumn, nHeight, nWidth );

		if( bResult == TRUE )
		{
			int				iStartOfLayer;
			int				iStartOfRow;

			iStartOfLayer = nLayer*pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn;

			int				limit1 = nColumn+nWidth;
			int				limit2 = nRow+nHeight;

			for( int iHeight=nRow; iHeight<limit2; ++iHeight )
			{
				iStartOfRow = iStartOfLayer + (iHeight)*pcsAgpdGrid->m_nColumn;

				for( int iWidth=nColumn; iWidth<limit1; ++iWidth )
				{
					if (pcsAgpdGrid->m_pbLockTable[iStartOfRow+iWidth])
						return FALSE;
				}
			}

			for( int iHeight=nRow; iHeight<limit2; ++iHeight )
			{
				iStartOfRow = iStartOfLayer + (iHeight)*pcsAgpdGrid->m_nColumn;

				for( int iWidth=nColumn; iWidth<limit1; ++iWidth )
				{
					//ItemID를 Grid에 복사한다.
					pcsAgpdGrid->m_ppcGridData[iStartOfRow+iWidth] = pcsGridItem;
				}
			}

			if (pcsAgpdGrid->m_nMaxRowExistItem < limit2 - 1)
				pcsAgpdGrid->m_nMaxRowExistItem = limit2 - 1;
		}
	}

	return bResult;
}

BOOL AgpmGrid::AddToLayer( AgpdGrid *pcsAgpdGrid, INT16 nLayer, AgpdGridItem *pcsGridItem, INT16 nHeight, INT16 nWidth, INT16 *nRow, INT16 *nColumn )
{
	if (!pcsAgpdGrid || !pcsGridItem)
		return FALSE;

	int	iStartOfLayer = nLayer * pcsAgpdGrid->m_nRow * pcsAgpdGrid->m_nColumn;

	for (int i = 0; i < pcsAgpdGrid->m_nRow; ++i)
	{
		for (int j = 0; j < pcsAgpdGrid->m_nColumn; ++j)
		{
			if (!pcsAgpdGrid->m_ppcGridData[iStartOfLayer + i * pcsAgpdGrid->m_nColumn + j])
			{
				if (Add(pcsAgpdGrid, nLayer, i, j, pcsGridItem, nHeight, nWidth))
				{
					if (nRow)
						*nRow		= i;
					if (nColumn)
						*nColumn	= j;

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL AgpmGrid::RemoveItem(AgpdGrid *pcsAgpdGrid, AgpdGridItem *pcsGridItem)
{
	if (!pcsAgpdGrid || !pcsAgpdGrid->m_ppcGridData || !pcsGridItem)
		return FALSE;

	for (int i = 0; i < pcsAgpdGrid->m_lGridCount; ++i)
	{
		if (pcsAgpdGrid->m_ppcGridData[i] && pcsAgpdGrid->m_ppcGridData[i] == pcsGridItem)
		{
			pcsAgpdGrid->m_ppcGridData[i] = NULL;
			pcsAgpdGrid->m_pbLockTable[i] = FALSE;

			if ( pcsAgpdGrid->m_lItemCount != 0 )	// 0일때 --하는 경우가 있어서 추가, skcho
				--pcsAgpdGrid->m_lItemCount;

			UpdateMaxRowExistItem(pcsAgpdGrid);

			return TRUE;
		}
	}

	return TRUE;
}

// 지우고... 그 뒤에 놈들을 앞으로 땡긴다. (앞에서 부터 채우는 Grid(주로 클라이언트)같은 특수한 경우에만 사용한다.)
BOOL AgpmGrid::RemoveItemAndFillFirst(AgpdGrid *pcsAgpdGrid, AgpdGridItem *pcsGridItem)
{
	if (!pcsAgpdGrid || !pcsGridItem)
		return FALSE;

	for (int i = 0; i < pcsAgpdGrid->m_lGridCount; ++i)
	{
		if (pcsAgpdGrid->m_ppcGridData[i] && pcsAgpdGrid->m_ppcGridData[i] == pcsGridItem)
		{
			CopyMemory(pcsAgpdGrid->m_ppcGridData + i,
						pcsAgpdGrid->m_ppcGridData + i + 1,
						sizeof(AgpdGridItem *) * (pcsAgpdGrid->m_lGridCount - i - 1));
			CopyMemory(pcsAgpdGrid->m_pbLockTable + i,
						pcsAgpdGrid->m_pbLockTable + i + 1,
						sizeof(BOOL) * (pcsAgpdGrid->m_lGridCount - i - 1));

			pcsAgpdGrid->m_ppcGridData[pcsAgpdGrid->m_lGridCount - 1] = NULL;
			pcsAgpdGrid->m_pbLockTable[pcsAgpdGrid->m_lGridCount - 1] = FALSE;

			--pcsAgpdGrid->m_lItemCount;

			UpdateMaxRowExistItem(pcsAgpdGrid);

			return TRUE;
		}
	}

	return TRUE;
}

//그리드의 영역을 지운다.
BOOL AgpmGrid::Clear( AgpdGrid *pcsAgpdGrid, INT16 nLayer, INT16 nRow, INT16 nColumn, INT16 nHeight, INT16 nWidth )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdGrid != NULL )
	{
		if( IsInGrid( pcsAgpdGrid, nLayer, nRow, nColumn, nHeight, nWidth ) )
		{
			int				iStartOfLayer;
			int				iStartOfRow;

			iStartOfLayer = nLayer*pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn;

			int				limit1 = nColumn+nHeight;
			int				limit2 = nRow+nWidth;

			for( int iHeight=nRow; iHeight<limit2; ++iHeight )
			{
				iStartOfRow = iStartOfLayer + (iHeight)*pcsAgpdGrid->m_nColumn;

				for( int iWidth=nColumn; iWidth<limit1; ++iWidth )
				{
					if (pcsAgpdGrid->m_ppcGridData[iStartOfRow+iWidth])
						bResult = TRUE;

					//ItemID를 Grid에 복사한다.
					pcsAgpdGrid->m_ppcGridData[iStartOfRow+iWidth] = NULL;	// 왜 0일까?(Parn)
					pcsAgpdGrid->m_pbLockTable[iStartOfRow+iWidth] = FALSE;
				}
			}
		}
	}

	return bResult;
}

AgpdGridItem *	AgpmGrid::GetItemSequence(AgpdGrid *pcsAgpdGrid, INT32 *plIndex)
{
	for (; *plIndex < pcsAgpdGrid->m_lGridCount; ++(*plIndex))
	{
		if (pcsAgpdGrid->m_ppcGridData[*plIndex])
		{
			++(*plIndex);

			return pcsAgpdGrid->m_ppcGridData[*plIndex - 1];
		}
	}

	return NULL;
}

VOID			AgpmGrid::Reset(AgpdGrid *pcsAgpdGrid)
{
	if (!pcsAgpdGrid)
		return;

	pcsAgpdGrid->m_nMaxRowExistItem = 0;
	pcsAgpdGrid->m_lItemCount		= 0;

	if (pcsAgpdGrid->m_ppcGridData)
		ZeroMemory( pcsAgpdGrid->m_ppcGridData, sizeof( AgpdGridItem * ) * pcsAgpdGrid->m_lGridCount );

	if (pcsAgpdGrid->m_pbLockTable)
		ZeroMemory( pcsAgpdGrid->m_pbLockTable, sizeof( BOOL ) * pcsAgpdGrid->m_lGridCount );
}

INT16 AgpmGrid::AttachGridItemTextureData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, AGPMGRID_DATA_GRIDITEM, nDataSize, pfConstructor, pfDestructor);
}

AgpdGridItem*	AgpmGrid::CreateGridItem()
{
	AgpdGridItem* pcsItemGrid = (AgpdGridItem*) CreateModuleData( AGPMGRID_DATA_GRIDITEM );

	if ( pcsItemGrid )
	{
		pcsItemGrid->m_eType		=		AGPDGRID_ITEM_TYPE_NONE		;
		pcsItemGrid->m_lItemID		=		AP_INVALID_IID				;
		pcsItemGrid->m_lItemTID		=		AP_INVALID_IID				;	

		pcsItemGrid->m_pcsTemplateGrid	= NULL;

		return pcsItemGrid;
	}

	return NULL;
}

BOOL AgpmGrid::DeleteGridItem(AgpdGridItem *pcsGridItem)
{
	if (!pcsGridItem)
		return FALSE;

	return DestroyModuleData(pcsGridItem, AGPMGRID_DATA_GRIDITEM);
}

BOOL AgpmGrid::IsLocked(AgpdGrid *pcsAgpdGrid, INT16 nLayer, INT16 nRow, INT16 nColumn)
{
	if (!pcsAgpdGrid)
		return FALSE;

	if (!IsInGrid(pcsAgpdGrid, nLayer, nRow, nColumn, 1, 1))
		return FALSE;

	INT32	lIndex = nLayer*pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn;
	lIndex += (nRow*pcsAgpdGrid->m_nColumn);
	lIndex += nColumn;

	return pcsAgpdGrid->m_pbLockTable[lIndex];
}

BOOL AgpmGrid::LockItem(AgpdGrid *pcsAgpdGrid, INT16 nLayer, INT16 nRow, INT16 nColumn, INT16 nHeight, INT16 nWidth)
{
	if (!pcsAgpdGrid)
		return FALSE;

	if (!IsInGrid(pcsAgpdGrid, nLayer, nRow, nColumn, nHeight, nWidth))
		return FALSE;

	int				iStartOfLayer;
	int				iStartOfRow;

	iStartOfLayer = nLayer * pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn;

	int				limit1 = nColumn+nWidth;
	int				limit2 = nRow+nHeight;

	for( int iHeight = nRow; iHeight < limit2; ++iHeight )
	{
		iStartOfRow = iStartOfLayer + (nRow) * pcsAgpdGrid->m_nColumn;

		for( int iWidth=nColumn; iWidth<limit1; ++iWidth )
		{
			// 이미 다른놈이 잠그고 있는지 본다.
			if (pcsAgpdGrid->m_pbLockTable[iStartOfRow+iWidth])
				return FALSE;
		}
	}

	for( int iHeight = nRow; iHeight < limit2; ++iHeight )
	{
		iStartOfRow = iStartOfLayer + (nRow) * pcsAgpdGrid->m_nColumn;

		for( int iWidth=nColumn; iWidth<limit1; ++iWidth )
		{
			pcsAgpdGrid->m_pbLockTable[iStartOfRow+iWidth]	= TRUE;
		}
	}

	return TRUE;
}

BOOL AgpmGrid::ReleaseItem(AgpdGrid *pcsAgpdGrid, INT16 nLayer, INT16 nRow, INT16 nColumn, INT16 nHeight, INT16 nWidth)
{
	if (!pcsAgpdGrid)
		return FALSE;

	if (!IsInGrid(pcsAgpdGrid, nLayer, nRow, nColumn, nHeight, nWidth))
		return FALSE;

	int				iStartOfLayer;
	int				iStartOfRow;

	iStartOfLayer = nLayer * pcsAgpdGrid->m_nRow*pcsAgpdGrid->m_nColumn;

	int				limit1 = nColumn+nWidth;
	int				limit2 = nRow+nHeight;

	for( int iHeight = nRow; iHeight < limit2; ++iHeight )
	{
		iStartOfRow = iStartOfLayer + (nRow) * pcsAgpdGrid->m_nColumn;

		for( int iWidth=nColumn; iWidth<limit1; ++iWidth )
		{
			pcsAgpdGrid->m_pbLockTable[iStartOfRow+iWidth]	= FALSE;
		}
	}

	return TRUE;
}

VOID	AgpmGrid::UpdateMaxRowExistItem(AgpdGrid *pcsAgpdGrid)
{
	if (!pcsAgpdGrid)
		return;

	INT16	nRow;
	INT16	nLayer;

	pcsAgpdGrid->m_nMaxRowExistItem = 0;

	for (nLayer = 0; nLayer < pcsAgpdGrid->m_nLayer; ++nLayer)
	{
		for (nRow = 0; nRow < pcsAgpdGrid->m_nRow; ++nRow)
		{
			if (pcsAgpdGrid->m_nMaxRowExistItem < nRow && !IsEmptyGrid(pcsAgpdGrid, nLayer, nRow, 0, 1, pcsAgpdGrid->m_nColumn))
				pcsAgpdGrid->m_nMaxRowExistItem = nRow;
		}
	}
}

AgpdGridItem::AgpdGridItem():
	m_bIsLevelLimited( FALSE )
{
	m_eType = AGPDGRID_ITEM_TYPE_NONE	;	
	m_lItemID = 0						;
	m_lItemTID = 0						;

	m_bMoveable	=	TRUE				;

	memset(	m_strLeftBottom, 0, sizeof( m_strLeftBottom ) )		;
	memset( m_strRightBottom, 0, sizeof( m_strRightBottom ) )	;

	m_bWriteLeftBottom		=	FALSE							;
	m_bWriteRightBottom		=	FALSE							;
	m_bTwinkleMode			=	FALSE							;

	m_ulUseItemTime			= 0;
	m_ulReuseIntervalTime	= 0;
	m_ulPauseClockCount		= 0;
	m_ulRemainTime			= 0;

	m_pcsTemplateGrid		= NULL;

	m_bForceWriteBottom		=	FALSE							;
	
	m_pcsParentBase			= NULL								;

	m_bIsDurabilityZero		= FALSE;

	m_szTooltip				= NULL;
}

AgpdGridItem::~AgpdGridItem()
{
	if (m_szTooltip)
		delete [] m_szTooltip;
	m_szTooltip				= NULL;
}

void	AgpdGridItem::SetLeftBottomString( CHAR* pString )
{
	m_bWriteLeftBottom	= TRUE;

	ZeroMemory(m_strLeftBottom, sizeof(CHAR) * (AGPDGRIDITEM_BOTTOM_STRING_LENGTH + 1));
	strncpy( m_strLeftBottom, pString, AGPDGRIDITEM_BOTTOM_STRING_LENGTH );
}

void	AgpdGridItem::SetRightBottomString( CHAR* pString )
{
	m_bWriteRightBottom	= TRUE;

	ZeroMemory(m_strRightBottom, sizeof(CHAR) * (AGPDGRIDITEM_BOTTOM_STRING_LENGTH + 1));
	strncpy( m_strRightBottom, pString, AGPDGRIDITEM_BOTTOM_STRING_LENGTH );
}

void	AgpdGridItem::SetGridDisplayData( char *pstrLeftString, char *pstrRightString )
{
	if( pstrLeftString )
	{
		m_bWriteLeftBottom = true;
		SetLeftBottomString( pstrLeftString );
	}
	else
	{
		m_bWriteLeftBottom = false;
	}

	if( pstrRightString )
	{
		m_bWriteRightBottom = true;
		SetRightBottomString( pstrRightString );
	}
	else
	{
		m_bWriteRightBottom = false;
	}
}

void	AgpdGridItem::SetUseItemTime(UINT32 ulUseTime, UINT32 ulReuseIntervalTime, UINT32 ulPauseClockCount)
{
	m_ulUseItemTime			= ulUseTime;
	m_ulReuseIntervalTime	= ulReuseIntervalTime;
	m_ulPauseClockCount		= ulPauseClockCount;

	return;
}

/*//정확히는 Move인데 Update라고 이미 쓰고있으니 나도.......
BOOL AgpmGrid::Update( AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn, INT64 llItemID, INT16 nHeight, INT16 nWidth )
{
	BOOL			bResult;

	bResult = IsMovalbeGrid( nLayer, nRow, nColumn, llItemID, nHeight, nWidth );

	if( bResult == TRUE )
	{
		//원래 위치를 얻어낸다.
		INT16			eStatus;
		INT16			nOriginLayer;
		INT16			nOriginRow;
		INT16			nOriginColumn;

		//원래 위치를 얻어낸다.
		bResult = GetOriginInfo( pcsAgpdItem->m_lID, &eStatus, &nOriginLayer, &nOriginRow, &nOriginColumn );

		if( bResult == TRUE )
		{
			//지워주고....
			bResult = Clear( nOriginLayer, nOriginRow, nOriginColumn, llItemID, nHeight, nWidth );

			if( bResult == TRUE )
			{
				bResult = Add( nLayer, nRow, nColumn, llItemID, nHeight, nWidth );
				
				if( bResult == FALSE )
				{
					//Clear시킨 아이템을 복구해준다. 이거 치명적인 에러인데?? 발생하면 안되는 상홤임.
					//어쨌든 가능성은 있으니 체크는하자~
					if( Add( nOriginLayer, nOriginRow, nOriginColumn, llItemID, nHeight, nWidth ) == FALSE )
					{
						//복구가 안된 경우임. 로그로 남겨야한다. <- 그런데..... 이런 경우가 있을까?
					}
				}
			}
		}
	}

	return bResult;
}

//정확히는 Move인데 Update라고 이미 쓰고있으니 나도.......
BOOL AgpmGrid::Update( INT16 nOriginLayer, INT16 nOriginRow, INT16 nOriginColumn, INT16 nLayer, INT16 nRow, INT16 nColumn, INT64 llItemID, INT16 nHeight, INT16 nWidth )
{
	BOOL			bResult;

	bResult = IsMovalbeGrid( nLayer, nRow, nColumn, llItemID, nHeight, nWidth );

	if( bResult == TRUE )
	{
		//원래 위치를 얻어낸다.
//		INT16			 eTempStatus;
//		INT16			nTempOriginLayer;
//		INT16			nTempOriginRow;
//		INT16			nTempOriginColumn;

		//원래 위치를 얻어낸다.
//		bResult = GetOriginInfo( lItemID, &eTempStatus, &nTempOriginLayer, &nTempOriginRow, &nTempOriginColumn );

//		if( bResult == TRUE )
		{
			//지워주고....
			bResult = Clear( nOriginLayer, nOriginRow, nOriginColumn, llItemID, nHeight, nWidth );

			if( bResult == TRUE )
			{
				bResult = Add( nLayer, nRow, nColumn, llItemID, nHeight, nWidth );
				
				if( bResult == FALSE )
				{
					//Clear시킨 아이템을 복구해준다. 이거 치명적인 에러인데?? 발생하면 안되는 상홤임.
					//어쨌든 가능성은 있으니 체크는하자~
					if( Add( nOriginLayer, nOriginRow, nOriginColumn, llItemID, nHeight, nWidth ) == FALSE )
					{
						//복구가 안된 경우임. 로그로 남겨야한다. <- 그런데..... 이런 경우가 있을까?
					}
				}
			}
		}
	}

	return bResult;
}*/

BOOL AgpmGrid::IsFullGrid( AgpdGrid *pcsAgpdGrid )
{
	return (pcsAgpdGrid->m_lGridCount == pcsAgpdGrid->m_lItemCount);
}

BOOL AgpmGrid::CheckEnoughItem(AgpdGrid *pcsAgpdGrid, INT32 lItemTID, INT32 lCount)
{
	INT32 lAddedCount = 0;
	AgpdGridItem *pcsAgpdGridItem = NULL;

	if( pcsAgpdGrid != NULL )
	{
		for( int i = 0; i < pcsAgpdGrid->m_lGridCount; ++i )
		{
			pcsAgpdGridItem = pcsAgpdGrid->m_ppcGridData[i];

			if (pcsAgpdGridItem && pcsAgpdGridItem->m_lItemTID == lItemTID)
			{
				AgpdItem* pcsAgpdItem = (AgpdItem*)pcsAgpdGridItem->GetParentBase();
				AgpdItemTemplate* pcsItemTemplate = (AgpdItemTemplate*)pcsAgpdItem->m_pcsItemTemplate;

				if (pcsItemTemplate->m_bStackable)
					lAddedCount += pcsAgpdItem->m_nCount;
				else
					++lAddedCount;

				if (lCount <= lAddedCount)
					return TRUE;
			}
		}
	}

	return FALSE;
}

INT32 AgpmGrid::GetCountByTemplate(AgpdGrid *pcsAgpdGrid, AgpdGridItemType eType, INT32 lItemTID)
{
	INT32 lAddedCount = 0;
	AgpdGridItem *pcsAgpdGridItem = NULL;

	if( pcsAgpdGrid != NULL )
	{
		for( int i = 0; i < pcsAgpdGrid->m_lGridCount; ++i )
		{
			pcsAgpdGridItem = pcsAgpdGrid->m_ppcGridData[i];

			if (pcsAgpdGridItem && pcsAgpdGridItem->m_lItemTID == lItemTID)
			{
				AgpdItem* pcsAgpdItem = (AgpdItem*)pcsAgpdGridItem->GetParentBase();
				AgpdItemTemplate* pcsItemTemplate = (AgpdItemTemplate*)pcsAgpdItem->m_pcsItemTemplate;

				if (pcsItemTemplate->m_bStackable)
					lAddedCount += pcsAgpdItem->m_nCount;
				else
					++lAddedCount;
			}
		}
	}

	return lAddedCount;
}

VOID AgpdGridItem::SetTooltip(CHAR *szTooltip)
{
	if (szTooltip && m_szTooltip && !strcmp(szTooltip, m_szTooltip))
		return;

	if (m_szTooltip)
		delete [] m_szTooltip;
	m_szTooltip = NULL;

	if (szTooltip)
	{
		m_szTooltip = new CHAR [strlen(szTooltip) + 1];
		strcpy(m_szTooltip, szTooltip);
	}
}
