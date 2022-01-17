/******************************************************************************
Module:  AgpmItem.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 16
******************************************************************************/

#include <stdio.h>
#include "AgpmItem.h"

/******************************************************************************
* Purpose : Set call-back.
*
* 100102. Bob Jung
******************************************************************************/
BOOL AgpmItem::SetCallbackField(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_FIELD, pfCallback, pClass);
}

/******************************************************************************
* Purpose : Add item
*
* 100102. Bob Jung
******************************************************************************/
BOOL AgpmItem::AddItemToField(AgpdItem *pcsAgpdItem)
{
	if (!pcsAgpdItem)
		return FALSE;

	if (!RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_FIELD))
		return FALSE;

	pcsAgpdItem->m_eStatus = AGPDITEM_STATUS_FIELD;

	if (m_papmMap)
		m_papmMap->AddItem( pcsAgpdItem->m_nDimension , pcsAgpdItem->m_posItem, (INT32) pcsAgpdItem->m_lID);

	if( pcsAgpdItem->m_pcsCharacter != NULL )
		EnumCallback( ITEM_CB_ID_UI_UPDATE_INVENTORY, &pcsAgpdItem->m_pcsCharacter->m_lID, NULL );

	ChangeItemOwner(pcsAgpdItem, NULL);

	EnumCallback(ITEM_CB_ID_FIELD, pcsAgpdItem, NULL);

	EnumCallback(ITEM_CB_ID_ADD_ITEM_TO_MAP, pcsAgpdItem, NULL);

	return TRUE;
}

/******************************************************************************
* Purpose : Remove item
*
* 100102. Bob Jung
******************************************************************************/
BOOL AgpmItem::RemoveItemFromField(AgpdItem *pcsAgpdItem, BOOL bIsDeleteFromMap, BOOL bIsInitStatus)
{
	if (!pcsAgpdItem)
		return FALSE;

	if (bIsInitStatus)
		pcsAgpdItem->m_eStatus = AGPDITEM_STATUS_NONE;

	if (bIsDeleteFromMap && m_papmMap)
		m_papmMap->DeleteItem( pcsAgpdItem->m_nDimension , pcsAgpdItem->m_posItem, (INT32) pcsAgpdItem->m_lID);

	EnumCallback(ITEM_CB_ID_REMOVE_ITEM_FROM_MAP, pcsAgpdItem, NULL);

	return TRUE;
}

/******************************************************************************
* Purpose : 필드 패킷 파싱.
*
* 091602. Bob Jung
******************************************************************************/
BOOL AgpmItem::ParseFieldPacket(PVOID pField, AuPOS *pPos)
{
	AuPOS stTempPos;

	if (!pField || !pPos)
		return FALSE;

	m_csPacketField.GetField(FALSE, pField, *((UINT16 *)(pField)),
		                    &stTempPos);

	if(pPos)
	{
		*pPos = stTempPos;
	}

	return TRUE;
}

/******************************************************************************
******************************************************************************/