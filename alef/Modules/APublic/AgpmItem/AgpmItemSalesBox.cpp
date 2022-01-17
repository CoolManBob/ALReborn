/******************************************************************************
Module:  AgpmItemSalesBox.cpp
Notices: Copyright (c) NHN Studio 2002 Ashulam
Purpose: 
Last Update: 2003. 2. 18
******************************************************************************/

#include <stdio.h>
#include "AgpmItem.h"

BOOL AgpmItem::SetCallbackSalesBox(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_ADD_SALESBOX_GRID, pfCallback, pClass);
}

BOOL AgpmItem::AddItemToSalesBox(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	return AddItemToSalesBox(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn);
}

BOOL AgpmItem::AddItemToSalesBox(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_SALESBOX_GRID)
		return FALSE;

	AgpdItemGridResult	eResult;

	if (lLayer >= 0)
		eResult	= Insert(pcsAgpdItem, GetSalesBox(pcsCharacter), lLayer, lRow, lColumn);
	else
		eResult	= AutoInsert(pcsAgpdItem, GetSalesBox(pcsCharacter));

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_SALESBOX_GRID);

		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_SALESBOX_GRID;

		EnumCallback(ITEM_CB_ID_ADD_SALESBOX_GRID, pcsAgpdItem, NULL);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}

	return FALSE;
}

/*
//지정한 위치에 아이템을 넣어준다.
BOOL AgpmItem::AddItemToSalesBox(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol )
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::AddItemToSalesBox() Error (1) !!!\n");
		return FALSE;
	}

	return AddItemToSalesBox(pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol );
}

BOOL AgpmItem::AddItemToSalesBox(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol )
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::AddItemToSalesBox() Error (2) !!!\n");
		return FALSE;
	}

	return AddItemToGrid(&pcsAgpdItemADChar->m_csSalesBoxGrid, -1, ITEM_CB_ID_ADD_SALESBOX_GRID, AGPDITEM_STATUS_SALESBOX_GRID,
						 pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol);
}
*/

//인벤에서 아이템을 지워준다.
BOOL AgpmItem::RemoveItemFromSalesBox(INT32 lCID, AgpdItem* pcsAgpdItem)
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::RemoveItemFromSalesBox() Error (1) !!!\n");
		return FALSE;
	}

	return RemoveItemFromSalesBox(pcsAgpdCharacter, pcsAgpdItem);
}

//인벤에서 아이템을 지워준다.
BOOL AgpmItem::RemoveItemFromSalesBox(AgpdCharacter *pcsAgpdCharacter, AgpdItem* pcsAgpdItem)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::RemoveItemFromSalesBox() Error (2) !!!\n");
		return FALSE;
	}

	EnumCallback(ITEM_CB_ID_SALESBOX_REMOVE_UI_UPDATE, pcsAgpdCharacter, pcsAgpdItem);
	return RemoveItemFromGrid(&pcsAgpdItemADChar->m_csSalesBoxGrid, -1, pcsAgpdCharacter, pcsAgpdItem);
}

BOOL AgpmItem::SetSalesBoxBackOut( AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	AgpdItemADChar	*pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);

	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::RemoveItemFromSalesBox() Error (2) !!!\n");
		return FALSE;
	}

	if(!pcsAgpdItem)
	{
		OutputDebugString("AgpmItem::RemoveItemFromSalesBox() Error (3) !!!\n");
		return FALSE;
	}

	if( m_pagpmGrid->IsExistItem(&pcsAgpdItemADChar->m_csSalesBoxGrid, AGPDGRID_ITEM_TYPE_ITEM, pcsAgpdItem->m_lID ) == FALSE )
	{ 
		OutputDebugString("AgpmItem::RemoveItemFromSalesBox() Error (4) !!!\n");
		return FALSE; // SalesBox에 없는 넘이다. 상태 변경 불가능!!
	}

	//상태를 바꿔준다.
	pcsAgpdItem->m_eStatus = AGPDITEM_STATUS_SALESBOX_BACKOUT;

	return TRUE;
}
