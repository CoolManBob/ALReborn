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
BOOL AgpmItem::SetCallbackBank(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_BANK, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackRemoveBank(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_REMOVE_BANK, pfCallback, pClass);
}

BOOL AgpmItem::AddItemToBank(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge)
{
	return AddItemToBank(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn, bStackMerge);
}

BOOL AgpmItem::AddItemToBank(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_BANK ||
		lLayer > pcsCharacter->m_cBankSize)
		return FALSE;

	if (GetBoundType(pcsAgpdItem) != E_AGPMITEM_NOT_BOUND)
		return FALSE;

	AgpdItemGridResult	eResult;

	if (bStackMerge)
		eResult	= InsertStackMerge(pcsAgpdItem, GetBank(pcsCharacter), lLayer, lRow, lColumn);
	else
		eResult	= Insert(pcsAgpdItem, GetBank(pcsCharacter), lLayer, lRow, lColumn);

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		// 이전 상태를 Release 시키고 현재 상태를 Bank로 바꾼다.
		RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_BANK);

		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_BANK;

		BOOL bLog = TRUE;
		EnumCallback(ITEM_CB_ID_CHAR_BANK, pcsAgpdItem, &bLog);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_SUCCESS_STACKCOUNT_ZERO)
	{
		ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsAgpdItem->m_szDeleteReason, "Bank:stackcount == 0", AGPMITEM_MAX_DELETE_REASON);
		//만들어진 아이템을 없앤다.
		RemoveItem( pcsAgpdItem, TRUE );

		return TRUE;
	}

	return FALSE;
}

BOOL AgpmItem::UpdateItemInBank(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	return UpdateItemInBank(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn);
}

BOOL AgpmItem::UpdateItemInBank(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	if (pcsAgpdCharacter->m_cBankSize < lLayer)
		return FALSE;

	if (pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_BANK)
		return AddItemToBank(pcsAgpdCharacter, pcsAgpdItem, lLayer, lRow, lColumn);

	INT32	lPrevLayer	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB];
	INT32	lPrevRow	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW];
	INT32	lPrevColumn	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN];

	AgpdItemADChar	*pcsAttachData	= GetADCharacter(pcsAgpdCharacter);

	AgpdItemGridResult	eResult	= InsertStackMerge(pcsAgpdItem, &pcsAttachData->m_csBankGrid, lLayer, lRow, lColumn);

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		m_pagpmGrid->Clear(&pcsAttachData->m_csBankGrid, lPrevLayer, lPrevRow, lPrevColumn, 1, 1);
		m_pagpmGrid->DeleteItem(&pcsAttachData->m_csBankGrid, pcsAgpdItem->m_pcsGridItem);

		EnumCallback(ITEM_CB_ID_CHAR_BANK, pcsAgpdItem, NULL);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_SUCCESS_STACKCOUNT_ZERO)
	{
		ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsAgpdItem->m_szDeleteReason, "Bank:stackcount == 0", AGPMITEM_MAX_DELETE_REASON);
		//만들어진 아이템을 없앤다.
		RemoveItem( pcsAgpdItem, TRUE );

		return  TRUE;
	}

	return FALSE;
}

/*
BOOL AgpmItem::AddItemToBank(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnBankIdx, INT16 *pnBankRow, INT16 *pnBankCol )
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::AddItemToBank() Error (1) !!!\n");
		return FALSE;
	}

	return AddItemToBank(pcsAgpdCharacter, pcsAgpdItem, pnBankIdx, pnBankRow, pnBankCol );
}

BOOL AgpmItem::AddItemToBank(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnBankIdx, INT16 *pnBankRow, INT16 *pnBankCol )
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::AddItemToBank() Error (2) !!!\n");
		return FALSE;
	}

	BOOL	bAddResult	= AddItemToGrid(&pcsAgpdItemADChar->m_csBankGrid, ITEM_CB_ID_CHAR_CHECK_BANK, ITEM_CB_ID_CHAR_BANK, AGPDITEM_STATUS_BANK,
										pcsAgpdCharacter, pcsAgpdItem, pnBankIdx, pnBankRow, pnBankCol);

	if (bAddResult)
		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_BANK;

	return bAddResult;
}
*/

BOOL AgpmItem::RemoveItemFromBank(INT32 lCID, AgpdItem *pcsAgpdItem)
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::RemoveItemFromBank() Error (1) !!!\n");
		return FALSE;
	}

	return RemoveItemFromBank(pcsAgpdCharacter, pcsAgpdItem);
}

BOOL AgpmItem::RemoveItemFromBank(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::RemoveItemFromBank() Error (2) !!!\n");
		return FALSE;
	}

	pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_NONE;

	return RemoveItemFromGrid(&pcsAgpdItemADChar->m_csBankGrid, ITEM_CB_ID_CHAR_CHECK_BANK, pcsAgpdCharacter, pcsAgpdItem);
}

//뱅크 패킷 파싱.
BOOL AgpmItem::ParseBankPacket(PVOID pBank, INT16 *pnBankIdx, INT16 *pnBankRow, INT16 *pnBankCol)
{
	if( !pBank )
		return FALSE;

	if( !pnBankIdx || !pnBankRow || !pnBankCol )
		return FALSE;

	m_csPacketBank.GetField(FALSE, pBank, *((UINT16 *)(pBank)),
								pnBankIdx,
								pnBankRow,
								pnBankCol);

	return TRUE;
}

/*
BOOL AgpmItem::UpdateBank(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
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

	return UpdateBank(pcsAgpdCharacter, pcsAgpdItem, nLayer, nRow, nColumn );
}

BOOL AgpmItem::UpdateBank(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::UpdateBank() Error (2) !!!\n");
		return FALSE;
	}

	return UpdateGrid(&pcsAgpdItemADChar->m_csBankGrid, ITEM_CB_ID_CHAR_CHECK_BANK, ITEM_CB_ID_CHAR_BANK, AGPDITEM_STATUS_BANK,
					  pcsAgpdCharacter, pcsAgpdItem, nLayer, nRow, nColumn);
}
*/

BOOL AgpmItem::IsBuyBankSlot(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_cBankSize + 1 >= AGPMITEM_BANK_MAX_LAYER)
		return FALSE;

	return TRUE;
}

BOOL AgpmItem::CheckBuyBankSlotCost(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (!IsBuyBankSlot(pcsCharacter))
		return FALSE;

	INT64	llBuyBankSlotCost	= GetBuyBankSlotCost(pcsCharacter);
	if (llBuyBankSlotCost < 0)
		return FALSE;

	if (pcsCharacter->m_llMoney < llBuyBankSlotCost)
		return FALSE;

	return TRUE;
}

INT64 AgpmItem::GetBuyBankSlotCost(AgpdCharacter *pcsCharacter, INT32 *plTax)
{
	if (!pcsCharacter ||
		pcsCharacter->m_cBankSize < 0 ||
		pcsCharacter->m_cBankSize + 1 >= AGPMITEM_BANK_MAX_LAYER)
		return (-1);

	INT64 llCost = m_llBankSlotPrice[pcsCharacter->m_cBankSize + 1];

	if (llCost < 0)
		return (-1);
	
	INT32 lTaxRatio = m_pagpmCharacter->GetTaxRatio(pcsCharacter);
	INT32 lTax = 0;
	if (lTaxRatio > 0)
	{
		lTax = ( INT32 ) ( (llCost * lTaxRatio) / 100 );
	}
	llCost = llCost + lTax;	
	if (plTax)
		*plTax = lTax;
	
	return llCost;
}

BOOL AgpmItem::OnOperationRequestBuyBankSlot(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (!IsBuyBankSlot(pcsCharacter))
		return FALSE;

	if (!CheckBuyBankSlotCost(pcsCharacter))
		return FALSE;

	INT32	lTax = 0;
	INT64	llBuyBankSlotCost	= GetBuyBankSlotCost(pcsCharacter, &lTax);

	if (!m_pagpmCharacter->SubMoney(pcsCharacter, llBuyBankSlotCost))
		return FALSE;

	m_pagpmCharacter->PayTax(pcsCharacter, lTax);

	if (!m_pagpmCharacter->UpdateBankSize(pcsCharacter, pcsCharacter->m_cBankSize + 1))
		return FALSE;

	return TRUE;
}