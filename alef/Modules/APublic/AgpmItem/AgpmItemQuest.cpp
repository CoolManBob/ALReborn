#include <stdio.h>
#include "AgpmItem.h"

BOOL AgpmItem::SetCallbackQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_QUEST, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackRemoveQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_REMOVE_QUEST, pfCallback, pClass);
}

BOOL AgpmItem::AddItemToQuest(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol )
{
	/*
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::AddItemToQuest() Error (1) !!!\n");
		return FALSE;
	}

	if (!AddItemToQuest(pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol ))
		return FALSE;

	EnumCallback(ITEM_CB_ID_ADD_QUEST, NULL, NULL);
	*/

	return TRUE;
}

BOOL AgpmItem::AddItemToQuest(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol )
{
	return TRUE;

	/*
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::AddItemToQuest() Error (2) !!!\n");
		return FALSE;
	}

	INT32	lStackCount	= pcsAgpdItem->m_nCount;

	{
		if (!(pnInvIdx && pnInvRow && pnInvCol))
		{
			BOOL				bStackStatus;

			bStackStatus = TRUE;

//			if( (pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_NPC_TRADE) || ( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_TRADE_GRID ) || ( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_CLIENT_TRADE_GRID ) )
			if( ( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_TRADE_GRID ) || ( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_CLIENT_TRADE_GRID ) )
			{
				bStackStatus = FALSE;
			}

			//스태커블 아이템인 경우.
			if( ((AgpdItemTemplate *)pcsAgpdItem->m_pcsItemTemplate)->m_bStackable && bStackStatus )
			{
				AgpdItem			*pcsAgpdStackItem;
				AgpdGrid			*pcsGrid;
				AgpdItemTemplate	*pTemplate;

				INT32			lIndex;
				INT32			lTempStackCount;

				lIndex = 0;
				pcsGrid = &pcsAgpdItemADChar->m_csQuestGrid;
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
					strncpy(pcsAgpdItem->m_szDeleteReason, "Quest:스택카운트 0", AGPMITEM_MAX_DELETE_REASON);
					//만들어진 아이템을 없앤다.
					RemoveItem( pcsAgpdItem, TRUE );

					return TRUE;
				}
			}
		}
	}

	BOOL	bAddResult	= AddItemToGrid(&pcsAgpdItemADChar->m_csQuestGrid, ITEM_CB_ID_CHECK_QUEST, ITEM_CB_ID_QUEST, AGPDITEM_STATUS_QUEST,
										pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol);

	if (bAddResult)
		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_QUEST;

	EnumCallback(ITEM_CB_ID_ADD_QUEST, NULL, NULL);

	return bAddResult;
	*/
}

BOOL AgpmItem::RemoveItemFromQuest(INT32 lCID, AgpdItem *pcsAgpdItem)
{
	return TRUE;

	/*
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::RemoveItemFromQuest() Error (1) !!!\n");
		return FALSE;
	}

	if (!RemoveItemFromQuest(pcsAgpdCharacter, pcsAgpdItem))
		return FALSE;

	EnumCallback(ITEM_CB_ID_REMOVE_QUEST, NULL, NULL);

	return TRUE;
	*/
}

BOOL AgpmItem::RemoveItemFromQuest(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem)
{
	return TRUE;

	/*
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::RemoveItemFromQuest() Error (2) !!!\n");
		return FALSE;
	}

	pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_NONE;

	if (!RemoveItemFromGrid(&pcsAgpdItemADChar->m_csQuestGrid, ITEM_CB_ID_CHECK_QUEST, pcsAgpdCharacter, pcsAgpdItem))
		return FALSE;

	EnumCallback(ITEM_CB_ID_REMOVE_QUEST, NULL, NULL);

	return TRUE;
	*/
}

//뱅크 패킷 파싱.
BOOL AgpmItem::ParseQuestPacket(PVOID pQuest, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol)
{
	if( !pQuest )
		return FALSE;

	if( !pnInvIdx || !pnInvRow || !pnInvCol )
		return FALSE;

	m_csPacketQuest.GetField(FALSE, pQuest, *((UINT16 *)(pQuest)),
								pnInvIdx,
								pnInvRow,
								pnInvCol);

	return TRUE;
}

BOOL AgpmItem::UpdateQuest(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
{
	return TRUE;

	/*
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::UpdateQuest() Error (1) !!!\n");
		return FALSE;
	}

	if (!UpdateQuest(pcsAgpdCharacter, pcsAgpdItem, nLayer, nRow, nColumn ))
		return FALSE;

	return EnumCallback(ITEM_CB_ID_UPDATE_QUEST, NULL, NULL);
	*/
}

BOOL AgpmItem::UpdateQuest(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn)
{
	return TRUE;

	/*
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::UpdateQuest() Error (2) !!!\n");
		return FALSE;
	}

	if (!UpdateGrid(&pcsAgpdItemADChar->m_csQuestGrid, ITEM_CB_ID_CHECK_QUEST, ITEM_CB_ID_QUEST, AGPDITEM_STATUS_QUEST,
					  pcsAgpdCharacter, pcsAgpdItem, nLayer, nRow, nColumn))
		return FALSE;

	return EnumCallback(ITEM_CB_ID_UPDATE_QUEST, NULL, NULL);
	*/
}

