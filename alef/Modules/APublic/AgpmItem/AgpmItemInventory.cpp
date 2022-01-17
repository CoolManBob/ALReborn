/******************************************************************************
Module:  AgpmItem.cpp
Notices: Copyright (c) NHN Studio 2002 Ashulam
Purpose: 
Last Update: 2003. 1. 8
******************************************************************************/

#include <stdio.h>
#include "AgpmItem.h"

/******************************************************************************
* Purpose : Set call-back.
*
* 100102. Bob Jung
******************************************************************************/
BOOL AgpmItem::SetCallbackInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackInventoryForAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY_FOR_ADMIN, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackRemoveFromInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_REMOVE_INVENTORY, pfCallback, pClass);
}

BOOL AgpmItem::AddItemToInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsMergeStack)
{
	return AddItemToInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn, bIsMergeStack);
}

BOOL AgpmItem::AddItemToInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsMergeStack)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_INVENTORY)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_INVENTORY, lLayer, lRow, lColumn))
		return FALSE;

	if (IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
	{
		return AddItemToCashInventory(pcsCharacter, pcsAgpdItem);
	}

	// 여기에 들어와야 할 것 같다. 2007.12.12. steeple
	// 이큅에서 내리는 아이템이 오른손 Rapier 이고, 왼손에 들고 있는 아이템을 구해서 Dagger 라면 내려준다. 2007.11.02. steeple
	if(pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_EQUIP &&
		GetWeaponType(pcsAgpdItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER)
	{
		AgpdItem* pcsLeftItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
		if(pcsLeftItem && pcsLeftItem->m_pcsItemTemplate && pcsLeftItem->m_lID != pcsAgpdItem->m_lID &&
			GetWeaponType(pcsLeftItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
		{
			// 남은 칸이 두 칸 이상이어야 한다.
			if(m_pagpmGrid->GetEmpyGridCount(GetInventory(pcsCharacter)) < 2)
				return FALSE;
		}
	}

	if (lLayer < 0 && lRow < 0 && lColumn < 0)
		return AddItemToInventory(pcsCharacter, pcsAgpdItem, bIsMergeStack);

	// 인벤토리에 넣으려고 하는 아이템이 돈인지 살펴본다.
	// 돈이라면 pcsCharacter의 돈에 추가하고 pcsAgpdItem을 삭제한다. (주변에 삭제한다는 정보를 보내야 한다.)
	if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther *) pcsAgpdItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		// 돈이다.
		// character에 돈 추가시키고 삭제한다.

		FLOAT	fPCBangBonus	= 1.0f;

		//if (m_pagpmBillInfo->IsPCBang(pcsCharacter))
		//	fPCBangBonus	= 1.5f;

		INT64 llMoney = (INT64)GetItemMoney(pcsAgpdItem);
		llMoney = (INT64)((FLOAT)llMoney * fPCBangBonus);

		m_pagpmCharacter->AddMoney(pcsCharacter, (INT64)llMoney);
		m_pagpmCharacter->UpdateMoney(pcsCharacter);

		EnumCallback(ITEM_CB_ID_PICKUP_ITEM_MONEY, pcsAgpdItem, NULL);

		pcsAgpdItem->m_Mutex.SafeRelease();

		RemoveItem(pcsAgpdItem->m_lID);

		return TRUE;
	}

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if (bIsMergeStack)
		eResult	= InsertStackMerge(pcsAgpdItem, GetInventory(pcsCharacter), lLayer, lRow, lColumn);
	else
		eResult	= Insert(pcsAgpdItem, GetInventory(pcsCharacter), lLayer, lRow, lColumn);

	return AddItemToInventoryResult(eResult,
									pcsAgpdItem,
									pcsCharacter);
}

BOOL AgpmItem::AddItemToInventory(INT32 lCID, AgpdItem *pcsAgpdItem, BOOL bIsMergeStack)
{
	return AddItemToInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem);
}

BOOL AgpmItem::AddItemToInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bIsMergeStack)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_INVENTORY, -1, -1, -1))
		return FALSE;

	if (IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
	{
		return AddItemToCashInventory(pcsCharacter, pcsAgpdItem);
	}

	// 인벤토리에 넣으려고 하는 아이템이 돈인지 살펴본다.
	// 돈이라면 pcsCharacter의 돈에 추가하고 pcsAgpdItem을 삭제한다. (주변에 삭제한다는 정보를 보내야 한다.)
	if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther *) pcsAgpdItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		// 돈이다.
		// character에 돈 추가시키고 삭제한다.

		FLOAT	fPCBangBonus	= 1.0f;

		//if (m_pagpmBillInfo->IsPCBang(pcsCharacter))
		//	fPCBangBonus	= 1.5f;

		INT64 llMoney = (INT64)GetItemMoney(pcsAgpdItem);
		llMoney = (INT64)((FLOAT)llMoney * fPCBangBonus);

		m_pagpmCharacter->AddMoney(pcsCharacter, (INT64)llMoney);
		m_pagpmCharacter->UpdateMoney(pcsCharacter);

		EnumCallback(ITEM_CB_ID_PICKUP_ITEM_MONEY, pcsAgpdItem, NULL);

		pcsAgpdItem->m_Mutex.SafeRelease();

		RemoveItem(pcsAgpdItem->m_lID);

		return TRUE;
	}

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if (bIsMergeStack)
		eResult	= AutoInsertStackMerge(pcsAgpdItem, GetInventory(pcsCharacter));
	else
		eResult	= AutoInsert(pcsAgpdItem, GetInventory(pcsCharacter));

	return AddItemToInventoryResult(eResult,
									pcsAgpdItem,
									pcsCharacter);
}

BOOL AgpmItem::AddItemToInventoryResult(AgpdItemGridResult eResult, AgpdItem *pcsAgpdItem, AgpdCharacter *pcsCharacter)
{
	if (!pcsAgpdItem || !pcsCharacter)
		return FALSE;

	AgpdItemADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);

	INT32	lStackCount	= pcsAgpdItem->m_nCount;

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		// 이전 상태를 Release 시키고 현재 상태를 Inventory로 바꾼다.
		//RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_INVENTORY); 
		//JK_아이템 탈착시 1단계 실패하면 인벤토리에도 넣지 말아라..
		if(!RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_INVENTORY))
			return FALSE;

		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_INVENTORY;

		ChangeItemOwner(pcsAgpdItem, pcsCharacter);

		// 만약 화살류의 아템이라면 카운트를 계산한다.
		if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
		{
			pcsAttachData->m_lNumArrowCount += lStackCount;
		}
		else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
		{
			pcsAttachData->m_lNumBoltCount += lStackCount;
		}

		EnumCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY, pcsAgpdItem, NULL);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_SUCCESS_STACKCOUNT_ZERO)
	{
		// 만약 화살류의 아템이라면 카운트를 계산한다.
		if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
		{
			pcsAttachData->m_lNumArrowCount += lStackCount;
		}
		else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
		{
			pcsAttachData->m_lNumBoltCount += lStackCount;
		}

		ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsAgpdItem->m_szDeleteReason, "Inventory:stackcount == 0", AGPMITEM_MAX_DELETE_REASON);
		//만들어진 아이템을 없앤다.
		RemoveItem( pcsAgpdItem, TRUE );

		return TRUE;
	}

	return FALSE;
}

BOOL AgpmItem::UpdateItemInInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	return UpdateItemInInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn);
}

BOOL AgpmItem::UpdateItemInInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_INVENTORY, lLayer, lRow, lColumn))
		return FALSE;

	if (pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_INVENTORY)
		return AddItemToInventory(pcsCharacter, pcsAgpdItem, lLayer, lRow, lColumn);

	INT32	lPrevLayer	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB];
	INT32	lPrevRow	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW];
	INT32	lPrevColumn	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN];

	AgpdItemADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);

	AgpdItemGridResult	eResult	= InsertStackMerge(pcsAgpdItem, &pcsAttachData->m_csInventoryGrid, lLayer, lRow, lColumn);

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		m_pagpmGrid->Clear(&pcsAttachData->m_csInventoryGrid, lPrevLayer, lPrevRow, lPrevColumn, 1, 1);
		m_pagpmGrid->DeleteItem(&pcsAttachData->m_csInventoryGrid, pcsAgpdItem->m_pcsGridItem);

		// Admin Client 를 위해서 Update 되었을 때도 불러준다.
		EnumCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY_FOR_ADMIN, pcsAgpdItem, NULL);

		BOOL	bIsUpdate	= TRUE;

		EnumCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY, pcsAgpdItem, &bIsUpdate);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_SUCCESS_STACKCOUNT_ZERO)
	{
		ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsAgpdItem->m_szDeleteReason, "Inventory:stackcount == 0", AGPMITEM_MAX_DELETE_REASON);
		//만들어진 아이템을 없앤다.
		RemoveItem( pcsAgpdItem, TRUE );

		return  TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_NOT_EMPTY)
	{
		AgpdItem	*pcsAgpdExistItem = GetItem(m_pagpmGrid->GetItem(&pcsAttachData->m_csInventoryGrid, lLayer, lRow, lColumn));
		if (!pcsAgpdExistItem)
			return FALSE;

		// 둘중 한넘이 정령석인경우 개조 처리를 한다.
		//		pcsAgpdExistItem 위에 pcsAgpdItem을 얻어놓은 경우이다.
		//		이때 pcsAgpdItem 이 정령석인경우 개조가 가능한지 여부를 판단해 그 뒤 과정을 처리한다.
		//////////////////////////////////////////////////////////////////////////
		EnumCallback(ITEM_CB_ID_REQUEST_SPIRITSTONE_CONVERT, pcsAgpdExistItem, pcsAgpdItem);

		return FALSE;
	}
	else
		return FALSE;

	return TRUE;
}

/*
//지정한 위치에 아이템을 넣어준다.
INT32 AgpmItem::AddItemToInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol, BOOL bRemoveItem, BOOL bIsStackMerge)
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::AddItemToInventory() Error (1) !!!\n");
		return FALSE;
	}

	return AddItemToInventory(pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol, bRemoveItem, bIsStackMerge);
}

BOOL AgpmItem::AddItemToInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol, BOOL bRemoveItem, BOOL bIsStackMerge)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return AGPMITEM_AddItemInventoryResult_FALSE;

	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::AddItemToInventory() Error (2) !!!\n");
		return AGPMITEM_AddItemInventoryResult_FALSE;
	}

	INT32	lStackCount	= pcsAgpdItem->m_nCount;

	// 인벤토리에 넣으려고 하는 아이템이 돈인지 살펴본다.
	// 돈이라면 pcsAgpdCharacter의 돈에 추가하고 pcsAgpdItem을 삭제한다. (주변에 삭제한다는 정보를 보내야 한다.)
	if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther *) pcsAgpdItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		// 돈이다.
		// character에 돈 추가시키고 삭제한다.
		m_pagpmCharacter->AddMoney(pcsAgpdCharacter, (INT64) GetItemMoney(pcsAgpdItem));
		m_pagpmCharacter->UpdateMoney(pcsAgpdCharacter);

		EnumCallback(ITEM_CB_ID_PICKUP_ITEM_MONEY, pcsAgpdItem, NULL);

		// 삭제한다.
		if (bRemoveItem)
		{
			pcsAgpdItem->m_Mutex.SafeRelease();

			RemoveItem(pcsAgpdItem->m_lID);
		}

		return AGPMITEM_AddItemInventoryResult_TRUE;
	}
	//돈은 아니고~
	else
	{
		if (!(pnInvIdx && pnInvRow && pnInvCol))
		{
			BOOL				bStackStatus;

			bStackStatus = TRUE;

//			if( (pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_NPC_TRADE) || ( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_TRADE_GRID ) || ( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_CLIENT_TRADE_GRID ) )
			if(		( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_TRADE_GRID )
				||	( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_CLIENT_TRADE_GRID )
				||	( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_NONE )
				)
			{
				bStackStatus = FALSE;
			}

			//스태커블 아이템인 경우.
			if( ((AgpdItemTemplate *)pcsAgpdItem->m_pcsItemTemplate)->m_bStackable && bStackStatus && bIsStackMerge)
			{
				AgpdItem			*pcsAgpdStackItem;
				AgpdGrid			*pcsGrid;
				AgpdItemTemplate	*pTemplate;

				INT32			lIndex;
				INT32			lTempStackCount;

				lIndex = 0;
				pcsGrid = &pcsAgpdItemADChar->m_csInventoryGrid;
				pTemplate = (AgpdItemTemplate *)pcsAgpdItem->m_pcsItemTemplate;

				while( 1 )
				{
					AgpdGridItem		*pcsGridItem;

					pcsGridItem = m_pagpmGrid->GetItemByTemplate( lIndex, pcsGrid, AGPDGRID_ITEM_TYPE_ITEM, pTemplate->m_lID );

					if( pcsGridItem == NULL )
					{
						break;
					}
					else
					{
						pcsAgpdStackItem = GetItem( pcsGridItem );

						if( pcsAgpdStackItem )
						{
							if (CheckJoinItem(pcsAgpdStackItem, pcsAgpdItem))
							{
								//스택이 가득차지 않았다면?
								if( pcsAgpdStackItem->m_nCount < pTemplate->m_lMaxStackableCount )
								{
									lTempStackCount = pTemplate->m_lMaxStackableCount - pcsAgpdStackItem->m_nCount;

									if( lTempStackCount < pcsAgpdItem->m_nCount )
									{
										AddItemStackCount(pcsAgpdStackItem, lTempStackCount);
										SubItemStackCount(pcsAgpdItem, lTempStackCount);
									}
									else
									{
										AddItemStackCount(pcsAgpdStackItem, pcsAgpdItem->m_nCount);
										pcsAgpdItem->m_nCount = 0;
									}

									//변경된 아이템 즉, pcsAgpdStackItem를 갱신하라고 클라이언트로 패킷을 날린다.
									//EnumCallback(ITEM_CB_ID_UPDATE_STACK_COUNT, pcsAgpdStackItem, NULL);

									//남아있는 스택카운트가 없다면 브레이크!
									if( pcsAgpdItem->m_nCount <= 0 )
									{
										break;
									}
								}
							}
						}
					}
				}

				if( pcsAgpdItem->m_nCount <= 0 )
				{
					// 만약 화살류의 아템이라면 카운트를 계산한다.
					if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
					{
						pcsAgpdItemADChar->m_lNumArrowCount += lStackCount;
					}
					else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
					{
						pcsAgpdItemADChar->m_lNumBoltCount += lStackCount;
					}

					ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
					strncpy(pcsAgpdItem->m_szDeleteReason, "Inventory:스택카운트 0", AGPMITEM_MAX_DELETE_REASON);
					//만들어진 아이템을 없앤다.
					RemoveItem( pcsAgpdItem, TRUE );

					return AGPMITEM_AddItemInventoryResult_RemoveByStack;
				}
			}
		}
	}

	BOOL	bAddResult	= AddItemToGrid(&pcsAgpdItemADChar->m_csInventoryGrid, -1, ITEM_CB_ID_CHAR_ADD_INVENTORY, AGPDITEM_STATUS_INVENTORY,
										pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol);

	if (bAddResult)
	{
		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_INVENTORY;

		// 만약 화살류의 아템이라면 카운트를 계산한다.
		if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
		{
			pcsAgpdItemADChar->m_lNumArrowCount += lStackCount;
		}
		else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
		{
			pcsAgpdItemADChar->m_lNumBoltCount += lStackCount;
		}
	}

	return bAddResult;
}
*/

//인벤에서 아이템을 지워준다.
BOOL AgpmItem::RemoveItemFromInventory(INT32 lCID, AgpdItem* pcsAgpdItem)
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::RemoveItemFromInventory() Error (1) !!!\n");
		return FALSE;
	}

	return RemoveItemFromInventory(pcsAgpdCharacter, pcsAgpdItem);
}

//인벤에서 아이템을 지워준다.
BOOL AgpmItem::RemoveItemFromInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem* pcsAgpdItem)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::RemoveItemFromInventory() Error (2) !!!\n");
		return FALSE;
	}

	pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_NONE;

	// 만약 화살류의 아템이라면 카운트를 계산한다.
	if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
	{
		pcsAgpdItemADChar->m_lNumArrowCount -= pcsAgpdItem->m_nCount;
	}
	else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
	{
		pcsAgpdItemADChar->m_lNumBoltCount -= pcsAgpdItem->m_nCount;
	}

	EnumCallback(ITEM_CB_ID_REMOVE_INVENTORY_GRID, (PVOID)pcsAgpdItem, NULL);

	return RemoveItemFromGrid(&pcsAgpdItemADChar->m_csInventoryGrid, -1, pcsAgpdCharacter, pcsAgpdItem);
}

/*
BOOL AgpmItem::UpdateInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn )
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::UpdateInventory() Error (1) !!!\n");
		return FALSE;
	}

	return UpdateInventory(pcsAgpdCharacter, pcsAgpdItem, nLayer, nRow, nColumn );
}

BOOL AgpmItem::UpdateInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn )
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::UpdateInventory() Error (2) !!!\n");
		return FALSE;
	}

	// 현재 돈의 OtherType 이 세팅되지 않았다. 고로.. 위 코드로 실행해야 되는디 일단 아래껄로 임시 처리한다.
	if (GetMoneyTID() == ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID)
	{
		m_pagpmCharacter->AddMoney(pcsAgpdCharacter, (INT64) GetItemMoney(pcsAgpdItem));

		pcsAgpdItem->m_Mutex.Release();

		return RemoveItem(pcsAgpdItem->m_lID);
	}

	// Admin Client 를 위해서 Update 되었을 때도 불러준다.
	EnumCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY_FOR_ADMIN, pcsAgpdItem, NULL);

	return UpdateGrid(&pcsAgpdItemADChar->m_csInventoryGrid, -1, ITEM_CB_ID_CHAR_ADD_INVENTORY, AGPDITEM_STATUS_INVENTORY,
					  pcsAgpdCharacter, pcsAgpdItem, nLayer, nRow, nColumn);
}
*/

AgpdItem* AgpmItem::GetInventoryItemByTID(AgpdCharacter *pcsCharacter, INT32 lTID)
{
	if (!pcsCharacter || lTID == AP_INVALID_IID)
		return NULL;

	AgpdItemADChar	*pcsItemADChar	= GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return NULL;

	for (int i = 0; i < pcsItemADChar->m_csInventoryGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pcsItemADChar->m_csInventoryGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pcsItemADChar->m_csInventoryGrid.m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= m_pagpmGrid->GetItem(&pcsItemADChar->m_csInventoryGrid, i, k, j);
				if (pcsGridItem)
				{
					AgpdItem	*pcsItem	= GetItem(pcsGridItem);
					if (pcsItem && pcsItem->m_pcsItemTemplate)
					{
						if (lTID == ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lID)
							return pcsItem;
					}
				}
			}
		}
	}

	return NULL;
}

AgpdItem* AgpmItem::GetInventoryPotionItem(AgpdCharacter *pcsCharacter, AgpmItemUsablePotionType ePotionType)
{
	if (!pcsCharacter)
		return NULL;

	AgpdItemADChar	*pcsItemADChar	= GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return NULL;

	AgpdItemTemplate	*pcsItemTemplate	= NULL;
	AgpdItem			*pcsItem			= NULL;

	for (int i = 0; i < pcsItemADChar->m_csInventoryGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pcsItemADChar->m_csInventoryGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pcsItemADChar->m_csInventoryGrid.m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= m_pagpmGrid->GetItem(&pcsItemADChar->m_csInventoryGrid, i, k, j);
				if (pcsGridItem)
				{
					pcsItem		= GetItem(pcsGridItem);
					if (pcsItem && pcsItem->m_pcsItemTemplate)
					{
						AgpdItemTemplate *pcsItemTemplate	= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;
						if (pcsItemTemplate &&
							pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
							((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_POTION &&
							((AgpdItemTemplateUsablePotion *) pcsItemTemplate)->m_ePotionType == ePotionType)
						{
							return pcsItem;
						}
					}
				}
			}
		}
	}

	return NULL;
}

//인벤위치를 파싱한다.
BOOL AgpmItem::ParseInventoryPacket(PVOID pInventory, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol)
{
	if( !pInventory )
		return FALSE;

	if( !pnInvIdx || !pnInvRow || !pnInvCol )
		return FALSE;

	m_csPacketInventory.GetField(FALSE, pInventory, *((UINT16 *)(pInventory)),
								pnInvIdx,
								pnInvRow,
								pnInvCol);

	return TRUE;
}

AgpdItem* AgpmItem::GetSkillRollbackScroll(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgpdItemADChar	*pcsAttachData	= (AgpdItemADChar *)	GetADCharacter(pcsCharacter);

	for (int i = 0; i < pcsAttachData->m_csInventoryGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pcsAttachData->m_csInventoryGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pcsAttachData->m_csInventoryGrid.m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= m_pagpmGrid->GetItem(&pcsAttachData->m_csInventoryGrid, i, k, j);
				if (pcsGridItem)
				{
					AgpdItem	*pcsItem	= GetItem(pcsGridItem);
					if (pcsItem && pcsItem->m_pcsItemTemplate)
					{
						if (pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
							((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILLROLLBACK_SCROLL)
							return pcsItem;
					}
				}
			}
		}
	}

	return NULL;
}
