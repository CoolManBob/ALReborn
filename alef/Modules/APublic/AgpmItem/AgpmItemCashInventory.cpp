#include "AgpmItem.h"

BOOL AgpmItem::SetCallbackAddItemCashInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_ADD_ITEM_CASH_INVENTORY, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackRemoveItemCashInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_REMOVE_ITEM_CASH_INVENTORY, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackUpdateItemCashInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_UI_UPDATE_CASH_INVENTORY, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackUseCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_USE_CASH_ITEM, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackUnUseCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_UNUSE_CASH_ITEM, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackPauseCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_PAUSE_CASH_ITEM, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackGetPetTIDByItemFromSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_GET_PET_TID_BY_ITEM_FROM_SKILL, pfCallback, pClass);
}

BOOL AgpmItem::IsAnyEmptyCashInventory(AgpdCharacter *pcsCharacter)
{
	// 비어있는 그리드가 있는지 본다.
	INT16	nLayer	= 0;
	INT16	nRow	= 0;
	INT16	nColumn	= 0;

	if (!m_pagpmGrid->GetEmptyGrid(GetCashInventoryGrid(pcsCharacter), &nLayer, &nRow, &nColumn, 1, 1))
		return FALSE;

	return TRUE;
}

/*
	2005.11.16. By SungHoon
	캐쉬 인벤토리에 아이템을 추가한다.
*/
BOOL AgpmItem::AddItemToCashInventory(INT32 lCID, AgpdItem *pcsItem )
{
	if (!lCID || !pcsItem) return FALSE;

	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsCharacter)
	{
		OutputDebugString("AgpmItem::AddItemToCashInventory() Error (1) !!!\n");
		return FALSE;
	}

	return AddItemToCashInventory(pcsCharacter, pcsItem );
}

/*
	2005.11.16. By SungHoon
	캐쉬 인벤토리에 아이템을 추가한다.
*/
BOOL AgpmItem::AddItemToCashInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem )
{
	if (!pcsCharacter || !pcsItem)
		return FALSE;

	AgpdItemADChar *pcsItemADChar = GetADCharacter(pcsCharacter);
	if(!pcsItemADChar)
	{
		OutputDebugString("AgpmItem::AddItemToCashInventory() Error (2) !!!\n");
		return FALSE;
	}
	AgpdItemGridResult eResult = AutoInsert(pcsItem, GetCashInventoryGrid(pcsCharacter));

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		RemoveStatus(pcsItem, AGPDITEM_STATUS_CASH_INVENTORY);
		pcsItem->m_eStatus = AGPDITEM_STATUS_CASH_INVENTORY;

		ChangeItemOwner(pcsItem, pcsCharacter);
		EnumCallback(ITEM_CB_ID_CHAR_ADD_ITEM_CASH_INVENTORY, pcsItem, pcsCharacter);

		return TRUE;	
	}

	return FALSE;
}


/*
	2005.11.16. By SungHoon
	캐쉬 아이템을 아이템 TID로 업어온다.
*/
AgpdGridItem *AgpmItem::GetCashItemByTID( INT32 lCID, INT32 lTID )
{
	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if( !pcsCharacter ) return NULL;

	return GetCashItemByTID( pcsCharacter, lTID );
}

/*
	2005.11.16. By SungHoon
	캐쉬 아이템을 아이템 TID로 업어온다.
*/
AgpdGridItem *AgpmItem::GetCashItemByTID( AgpdCharacter *pcsCharacter, INT32 lTID )
{
	if (!pcsCharacter || !lTID) return NULL;

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid) 
		return NULL;

	for (int i = 0; i < pcsCashInventoryGrid->m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(pcsCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsItem = GetItem(pcsGridItem);
			if (pcsItem && pcsItem->m_pcsItemTemplate)
			{
				if (lTID == ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lID)
					return pcsGridItem;
			}
		}
	}

	return NULL;
}

/*
	2005.12.02. By kevelon
	캐쉬 아이템중 eUsableType, lSubType인 아이템중 사용중인 제일 처음 아이템을 리턴한다.
*/
AgpdGridItem *AgpmItem::GetCashItemUsableByType(AgpdCharacter *pcsCharacter, AgpmItemUsableType eUsableType, INT32 lSubType)
{
	if (!pcsCharacter) return NULL;

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid) return NULL;

	for (int i = 0; i < pcsCashInventoryGrid->m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(pcsCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsItem = GetItem(pcsGridItem);
			if (pcsItem && pcsItem->m_pcsItemTemplate && pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
			{
				AgpdItemTemplateUsable* pcsAgpdItemTemplateUsable = (AgpdItemTemplateUsable*)pcsItem->m_pcsItemTemplate;
				if (eUsableType == pcsAgpdItemTemplateUsable->m_nUsableItemType
					&& lSubType == pcsAgpdItemTemplateUsable->m_nSubType)
					return pcsGridItem;
			}
		}
	}

	return NULL;
}

/*
	2005.12.02. By SungHoon
	캐쉬 아이템중 eUsableType, lSubType인 아이템중 사용중인 제일 처음 아이템을 리턴한다.
*/
AgpdGridItem *AgpmItem::GetUsingCashItemUsableByType(AgpdCharacter *pcsCharacter, AgpmItemUsableType eUsableType, INT32 lSubType)
{
	if (!pcsCharacter ) return NULL;

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid) 
		return NULL;

	for (int i = 0; i < pcsCashInventoryGrid->m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(pcsCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsItem = GetItem(pcsGridItem);
			if (pcsItem &&
				(pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE) &&	//	사용중
				pcsItem->m_pcsItemTemplate && pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
			{
				AgpdItemTemplateUsable* pcsAgpdItemTemplateUsable = (AgpdItemTemplateUsable*)pcsItem->m_pcsItemTemplate;
				if (eUsableType == pcsAgpdItemTemplateUsable->m_nUsableItemType
					&& lSubType == pcsAgpdItemTemplateUsable->m_nSubType)
					return pcsGridItem;
			}
		}
	}

	return NULL;
}

// 해당 아이템의 Usable Type 을 얻어온다
AgpmItemUsableType	AgpmItem::GetUsableType( AgpdItem* pcsItem )
{
	AgpdItemTemplateUsable*		pcsAgpdItemTemplateUsable	=	static_cast< AgpdItemTemplateUsable* >(pcsItem->m_pcsItemTemplate);
	
	return static_cast< AgpmItemUsableType >(pcsAgpdItemTemplateUsable->m_nUsableItemType);
}

BOOL	AgpmItem::IsMarvelScroll( AgpdItem* pcsItem )
{
	return ( GetUsableType( pcsItem )	==	AGPMITEM_USABLE_TYPE_PRIVATE_TRADE_OPTION );
}

/*
	2005.11.16. By SungHoon
	캐쉬 아이템을 Grid에서 삭제한다.
*/
BOOL AgpmItem::RemoveItemFromCashInventory(INT32 lCID, AgpdItem* pcsItem)
{
	if (!lCID || !pcsItem) return FALSE;

	AgpdCharacter  *pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsCharacter)
	{
		OutputDebugString("AgpmItem::RemoveItemFromCashInventory() Error (1) !!!\n");
		return FALSE;
	}

	return RemoveItemFromCashInventory(pcsCharacter, pcsItem);
}

/*
	2005.11.16. By SungHoon
	캐쉬 아이템을 Grid에서 삭제한다.
*/
BOOL AgpmItem::RemoveItemFromCashInventory(AgpdCharacter *pcsCharacter, AgpdItem* pcsItem)
{
	if (!pcsCharacter || !pcsItem)
		return FALSE;

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid)
	{
		OutputDebugString("AgpmItem::RemoveItemFromCashInventory() Error (2) !!!\n");
		return FALSE;
	}
	pcsItem->m_eStatus	= AGPDITEM_STATUS_NONE;
	EnumCallback(ITEM_CB_ID_CHAR_REMOVE_ITEM_CASH_INVENTORY, pcsItem, pcsCharacter);

	if( m_pagpmGrid->IsExistItem(pcsCashInventoryGrid, AGPDGRID_ITEM_TYPE_ITEM, pcsItem->m_lID ) == FALSE )
	{ 
		OutputDebugString("AgpmItem::RemoveItemFromGrid() Error (4) !!!\n");
		return FALSE; 
	}

	m_pagpmGrid->RemoveItemAndFillFirst(pcsCashInventoryGrid, pcsItem->m_pcsGridItem );

//	m_anGridPos를 강제로 sync 맞춰줌. 좋은 방법은 아니라고 생각되는뎅 ㅡㅡ		by SungHoon
	ArrangeCashItemGridPos(pcsCharacter);

	return TRUE;
}

// 2006.01.04. steeple
// 캐쉬 아이템의 m_anGridPos 를 재정렬 해준다. 성훈형이 하신 거 copy & paste
BOOL AgpmItem::ArrangeCashItemGridPos(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid) 
		return FALSE;

	for(int i = 0; i < pcsCashInventoryGrid->m_nRow; i++)
	{
		AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItem(pcsCashInventoryGrid, 0, i, 0);
		if(pcsGridItem)
		{
			AgpdItem* pcsItem = GetItem(pcsGridItem);
			if(pcsItem && pcsItem->m_pcsItemTemplate)
				pcsItem->m_anGridPos[AGPDITEM_GRID_POS_ROW] = i;
		}
	}

	return TRUE;
}

/*
	2005.11.16. By SungHoon
	lTID 캐쉬 아이템을 Grid에서 삭제한다.
*/
BOOL AgpmItem::RemoveItemFromCashInventory(INT32 lCID, INT32 lTID)
{
	if (!lCID || !lTID) return FALSE;

	AgpdCharacter  *pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsCharacter)
	{
		OutputDebugString("AgpmItem::RemoveItemFromCashInventory() Error (1) !!!\n");
		return FALSE;
	}

	return RemoveItemFromCashInventory(pcsCharacter, lTID);
}

/*
	2005.11.16. By SungHoon
	lTID 캐쉬 아이템을 Grid에서 삭제한다.
*/
BOOL AgpmItem::RemoveItemFromCashInventory(AgpdCharacter *pcsCharacter, INT32 lTID)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdGridItem *pGridItem = GetCashItemByTID(pcsCharacter, lTID);
	if (!pGridItem) return FALSE;

	AgpdItem *pcsItem = GetItem(pGridItem);

	return RemoveItemFromCashInventory(pcsCharacter, pcsItem);
}

BOOL AgpmItem::IsCharacterUsingCashItem(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
	{
		return FALSE;
	}

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid) 
	{
		return NULL;
	}

	INT32 lIndex = 0;
	for(AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItemSequence(pcsCashInventoryGrid, &lIndex);
		pcsGridItem;
		pcsGridItem = m_pagpmGrid->GetItemSequence(pcsCashInventoryGrid, &lIndex))
	{
		AgpdItem *pcsItem = GetItem(pcsGridItem);
		if(!pcsItem)
			continue;

		if(pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE ||
			pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE)	// 포즈도 같이 리턴.
		{
			return TRUE;
		}

		// If stamina value started returns true.
		if(pcsItem->m_llStaminaRemainTime != 0 && pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime != 0 &&
			pcsItem->m_llStaminaRemainTime < pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgpmItem::IsCharacterUsingCashItemOfSameClassifyIDorTID(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem)
{
	if (!pcsCharacter || !pcsItem)
	{
		return FALSE;
	}

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid) 
	{
		return NULL;
	}

	for (int i = 0; i < pcsCashInventoryGrid->m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(pcsCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsTmpItem = GetItem(pcsGridItem);
			if (!pcsTmpItem)
			{
				continue;
			}

			// 같은 아이템은 Pass 2005.12.20. steeple
			if(pcsTmpItem->m_lID == pcsItem->m_lID)
				continue;

			if ((pcsTmpItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsTmpItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE)
				&&
				( (pcsTmpItem->m_pcsItemTemplate->m_lClassifyID == pcsItem->m_pcsItemTemplate->m_lClassifyID && pcsItem->m_pcsItemTemplate->m_lClassifyID != 0)
				||
				(pcsItem->m_pcsItemTemplate->m_lID == pcsTmpItem->m_pcsItemTemplate->m_lID) )
				)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL AgpmItem::UseAllAleadyInUseCashItem(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
	{
		return FALSE;
	}

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid) 
	{
		return NULL;
	}

	BOOL bRet = FALSE;

	for (int i = 0; i < pcsCashInventoryGrid->m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(pcsCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsItem = GetItem(pcsGridItem);
			if (pcsItem
				&& (pcsItem->m_nInUseItem || pcsItem->m_lExpireTime || pcsItem->m_lRemainTime < pcsItem->m_pcsItemTemplate->m_lRemainTime))
			{
				EnumCallback(ITEM_CB_ID_USE_ITEM, pcsItem, pcsCharacter);
				bRet = TRUE;
			}
		}
	}

	return bRet;
}


/*
	2005.12.09. By SungHoon
	캐쉬 아이템이고 사용중이고 Usable, SkillScroll, lSkillTID 인 아이템
*/
AgpdGridItem* AgpmItem::GetUsingCashItemBySkillTID(AgpdCharacter *pcsCharacter, INT32 lSkillTID )
{
	if (!pcsCharacter || !lSkillTID) return NULL;

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid)  return NULL;

	for (int i = 0; i < pcsCashInventoryGrid->m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(pcsCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsItem = GetItem(pcsGridItem);
			if (pcsItem &&
				(pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE) &&	//	사용중
					pcsItem->m_pcsItemTemplate && pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
			{
				AgpdItemTemplate* pcsAgpdItemTemplateUsable = (AgpdItemTemplate*)pcsItem->m_pcsItemTemplate;
				if (((AgpdItemTemplateUsable *)pcsAgpdItemTemplateUsable)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL
					&& (((AgpdItemTemplateUsableSkillScroll *) pcsAgpdItemTemplateUsable)->m_lSkillTID == lSkillTID))
					return pcsGridItem;
			}
		}
	}

	return NULL;
}

// 2008.06.23. steeple
// Return a pet item which has a sub inventory in using state.
// If you can't find out, returns NULL.
AgpdGridItem* AgpmItem::GetUsingCashPetItemInvolveSubInventory(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return NULL;

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid) 
		return NULL;

	INT32 lIndex = 0;
	for(AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItemSequence(pcsCashInventoryGrid, &lIndex);
		pcsGridItem;
		pcsGridItem = m_pagpmGrid->GetItemSequence(pcsCashInventoryGrid, &lIndex))
	{
		AgpdItem* pcsItem = GetItem(pcsGridItem);
		if(!pcsItem || !pcsItem->m_pcsItemTemplate || pcsItem->m_pcsItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
			continue;

		if(pcsItem->m_nInUseItem != AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime == 0)
			continue;

		INT32 lPetTID = 0;
		EnumCallback(ITEM_CB_ID_GET_PET_TID_BY_ITEM_FROM_SKILL, pcsItem->m_pcsItemTemplate, &lPetTID);
		if(lPetTID == 0)
			continue;

		AgpdCharacterTemplate* pcsCharacterTemplate = m_pagpmCharacter->GetCharacterTemplate(lPetTID);
		if(!pcsCharacterTemplate || pcsCharacterTemplate->m_lPetType != 1)
			continue;

		return pcsGridItem;
	}

	return NULL;
}

// 2005.12.12. steeple
// 캐쉬 아이템 중에서 사용중이고, Usable 인 놈들중에서 OnAttack 시에 스택 개수를 하나 줄여야 하는 놈을 줄인다.
// 리턴값은 줄어든 갯수
INT32 AgpmItem::SubCashItemStackCountOnAttack(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	// PC 가 아니라면 나간다.
	if(m_pagpmCharacter->IsPC(pcsCharacter) == FALSE)
		return 0;

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid) 
		return 0;

	INT32 lCount = 0;
	for(int i = 0; i < pcsCashInventoryGrid->m_nRow; i++)
	{
		AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItem(pcsCashInventoryGrid, 0, i, 0 );
		if(!pcsGridItem)
			continue;

		AgpdItem* pcsItem = GetItem(pcsGridItem);
		if(!pcsItem || !pcsItem->m_pcsItemTemplate)
			continue;

		if((pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE) &&	//	사용중
			pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
			pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_ONE_ATTACK)
		{
			// AgsmItem::CallbackUpdateStackCount 으로 옮김 2006.02.08. steeple
			//if(GetItemStackCount(pcsItem) == 1)
			//	UnuseItem(pcsCharacter, pcsItem);

			SubItemStackCount(pcsItem, 1);
			lCount++;
		}
	}

	return lCount;
}


/*
	2005.12.12. By SungHoon
	pcsItem 의 캐쉬아이템이 사용중지가 가능한지 검사한다.
*/
BOOL AgpmItem::CheckEnableStopCashItem(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	return pcsItem->m_pcsItemTemplate->m_bCanStopUsingItem;

	// m_bCanStopUsingItem 변수를 추가했3... 20051213, kelovon
////	if (!pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_UNUSE) return FALSE;		//	현재 사용중이 아님 아이템은 중지 불가.
//	// 정액제 아이템은 한 번 사용하면 취소할 수 없다.
//	if ((pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_REAL_TIME
//		|| pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_PLAY_TIME))
//	{
//		return FALSE;
//	}
//
//	return FALSE;		//	전체가 사용불가
//	//	가능한 아이템은 TRUE를 리턴하면 된다.
}

AgpdItem* AgpmItem::GetCashSkillRollbackScroll(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid)
		return NULL;

	for(int i = 0; i < pcsCashInventoryGrid->m_nRow; i++)
	{
		AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItem(pcsCashInventoryGrid, 0, i, 0 );
		if(!pcsGridItem)
			continue;

		AgpdItem	*pcsItem	= GetItem(pcsGridItem);
		if (pcsItem && pcsItem->m_pcsItemTemplate)
		{
			if (pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
				((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILLROLLBACK_SCROLL)
				return pcsItem;
		}
	}

	return NULL;
}

AgpdItem* AgpmItem::GetCashInventoryItemByTID(AgpdCharacter *pcsCharacter, INT32 lTID)
{
	if (!pcsCharacter || lTID == AP_INVALID_IID)
		return NULL;

	AgpdGrid *pcsCashInventoryGrid = GetCashInventoryGrid( pcsCharacter );
	if (!pcsCashInventoryGrid) 
		return NULL;

	for (int i = 0; i < pcsCashInventoryGrid->m_nLayer; ++i)
	{
		for (int j = 0; j < pcsCashInventoryGrid->m_nColumn; ++j)
		{
			for (int k = 0; k < pcsCashInventoryGrid->m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= m_pagpmGrid->GetItem(pcsCashInventoryGrid, i, k, j);
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
