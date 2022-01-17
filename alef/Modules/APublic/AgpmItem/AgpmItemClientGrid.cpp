/******************************************************************************
Module:  AgpmItem.cpp
Notices: Copyright (c) NHN Studio 2002 Ashulam
Purpose: 
Last Update: 2003. 2. 3
******************************************************************************/

#include <stdio.h>
#include "AgpmItem.h"

/*BOOL AgpmItem::SetCallbackClientTradeGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_INVENTORY, pfCallback, pClass);
}*/

//내가 TradeGrid에 넣은 아이템을 상대에게 알린다.
/*BOOL AgpmItem::NewItemToClient(AgpdItem *pcsItem)
{
	// 이 아템 추가와 관련한 콜백 함수들을 호출해준다.
	EnumCallback(ITEM_CB_ID_NEW, pcsItem, pcsItem->m_pcsItemTemplate);

	return TRUE;
}*/

BOOL AgpmItem::AddItemToClientTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	return AddItemToClientTradeGrid(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn);
}

BOOL AgpmItem::AddItemToClientTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if (lLayer >= 0)
		eResult	= Insert(pcsAgpdItem, GetClientTradeGrid(pcsAgpdCharacter), lLayer, lRow, lColumn);
	else
		eResult = AutoInsert(pcsAgpdItem, GetClientTradeGrid(pcsAgpdCharacter));

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		// 이전 상태를 Release 시키고 현재 상태를 Inventory로 바꾼다.
		RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_CLIENT_TRADE_GRID);

		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_CLIENT_TRADE_GRID;

		EnumCallback(ITEM_CB_ID_ADD_CLIENT_TRADE_GRID, pcsAgpdItem, NULL);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}

	return FALSE;
}

/*
//지정한 위치에 아이템을 넣어준다.
BOOL AgpmItem::AddItemToClientTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol )
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

	return AddItemToClientTradeGrid(pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol );
}

BOOL AgpmItem::AddItemToClientTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol )
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::AddItemToInventory() Error (2) !!!\n");
		return FALSE;
	}

	return AddItemToGrid(&pcsAgpdItemADChar->m_csClientTradeGrid, -1, ITEM_CB_ID_ADD_CLIENT_TRADE_GRID, AGPDITEM_STATUS_CLIENT_TRADE_GRID,
						 pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol);
}
*/

//인벤에서 아이템을 지워준다.
BOOL AgpmItem::RemoveItemFromClientTradeGrid(INT32 lCID, AgpdItem* pcsAgpdItem)
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

	return RemoveItemFromClientTradeGrid(pcsAgpdCharacter, pcsAgpdItem);
}

//인벤에서 아이템을 지워준다.
BOOL AgpmItem::RemoveItemFromClientTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem* pcsAgpdItem)
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::RemoveItemFromClientTradeGrid() Error (2) !!!\n");
		return FALSE;
	}

	return RemoveItemFromGrid(&pcsAgpdItemADChar->m_csClientTradeGrid, -1, pcsAgpdCharacter, pcsAgpdItem);
}
