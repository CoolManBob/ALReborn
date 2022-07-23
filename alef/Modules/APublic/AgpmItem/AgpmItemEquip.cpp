/******************************************************************************
Module:  AgpmItem.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 16
******************************************************************************/

#include <stdio.h>
#include "AgpmItem.h"

BOOL AgpmItem::SetCallbackEquip(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_EQUIP, pfCallback, pClass);
}

// 2004.04.01. steeple
BOOL AgpmItem::SetCallbackEquipForAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_EQUIP_FOR_ADMIN, pfCallback, pClass);
}

/******************************************************************************
* Purpose : Set call-back.
*
* 091202. Bob Jung
******************************************************************************/
BOOL AgpmItem::SetCallbackUnEquip(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_UNEQUIP, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackRemoveForNearCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_REMOVE_FOR_NEAR_CHARACTER, pfCallback, pClass);
}

BOOL AgpmItem::CBGetItemLancer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmItem *pThis = (AgpmItem *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;

	AgpdGridItem *pcsAgpdGridItem = pThis->GetEquipItem(pcsCharacter, AGPMITEM_PART_LANCER);
	if (!pcsAgpdGridItem)
		return FALSE;

	AgpdItem *pcsItem = pThis->GetItem(pcsAgpdGridItem);
	if (NULL == pcsItem)
		return FALSE;

	return TRUE;
}

AgpdGridItem *AgpmItem::GetEquipItem( INT32 lCID, INT32 lPart )
{
	if (lPart <= AGPMITEM_PART_NONE || lPart >= AGPMITEM_PART_NUM)
		return NULL;

	AgpdGridItem		*pcsAgpdGridItem;
	AgpdCharacter		*pcsAgpdCharacter;

	pcsAgpdGridItem = NULL;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if( pcsAgpdCharacter )
		pcsAgpdGridItem = GetEquipItem( pcsAgpdCharacter, lPart );

	return pcsAgpdGridItem;
}

AgpdGridItem *AgpmItem::GetEquipItem( AgpdCharacter *pcsAgpdCharacter, INT32 lPart )
{
	if (!pcsAgpdCharacter)
		return NULL;

	if (lPart <= AGPMITEM_PART_NONE || lPart >= AGPMITEM_PART_NUM)
		return NULL;

	AgpdItemADChar		*pcsAgpdItemADChar;
	AgpdGridItem		*pcsAgpdGridItem;

	pcsAgpdGridItem = NULL;
	
	pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);

	if(pcsAgpdItemADChar)
	{
		INT32	lEquipIndex	= GetEquipIndexFromTable(lPart);
		if (lEquipIndex < 0)
			return NULL;

		pcsAgpdGridItem = m_pagpmGrid->GetItem( &pcsAgpdItemADChar->m_csEquipGrid, lEquipIndex );
	}

	return pcsAgpdGridItem;
}

BOOL AgpmItem::EquipItem(INT32 lCID, INT32 lIID, BOOL bCheckUseItem, BOOL bLogin)
{
	if (m_pagpmCharacter)
		return EquipItem(m_pagpmCharacter->GetCharacter(lCID), GetItem(lIID), bCheckUseItem, bLogin);

	return FALSE;
}

BOOL AgpmItem::EquipItem(INT32 lCID, AgpdItem *pcsAgpdItem, BOOL bCheckUseItem, BOOL bLogin)
{
	if (m_pagpmCharacter)
		return EquipItem(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, bCheckUseItem, bLogin);

	return FALSE;
}

BOOL AgpmItem::EquipItem(AgpdCharacter *pcsAgpdCharacter, INT32 lIID, BOOL bCheckUseItem, BOOL bLogin)
{
	return EquipItem(pcsAgpdCharacter, GetItem(lIID), bCheckUseItem, bLogin);
}

BOOL AgpmItem::EquipItem(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bCheckUseItem, BOOL bLogin)
{
	if (!pcsAgpdItem || !pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::EquipItem() Error (1) !!!\n");
		return FALSE;
	}

	AgpdItemADChar			*pcsAgpdItemADChar;

	pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);

	if (!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::EquipItem() Error (2) !!!\n");
		return FALSE;
	}

	return EquipItem(pcsAgpdCharacter, pcsAgpdItemADChar, pcsAgpdItem, bCheckUseItem, bLogin);
}

BOOL AgpmItem::EquipItem(AgpdCharacter *pcsCharacter, AgpdItemADChar *pcsItemADChar, AgpdItem *pcsItem, BOOL bCheckUseItem, BOOL bLogin)
{
	if( !pcsCharacter || !pcsItemADChar || !pcsItem || !m_pagpmCharacter )	return FALSE;
	if( bCheckUseItem && !CheckUseItem( pcsCharacter, pcsItem ) )			return FALSE;

	// 현재 변신중인 경우라면 착용 할 수 없다.
	if ( (pcsCharacter->m_bIsTrasform || pcsCharacter->m_bIsEvolution) && 
		((AgpdItemTemplateEquip*)pcsItem->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_RIDE && 	// 변신중에도 탈것은 탈 수 있어야 한다
		!CheckUseItem(pcsCharacter, pcsItem))	// 변신중에도 변경가능한 무기는 변경할 수 있어야 한다
		return FALSE;		

	AgpdItemTemplateEquip* pcsAgpdItemTemplate = (AgpdItemTemplateEquip*)GetItemTemplate(pcsItem->m_lTID);
	if( !pcsAgpdItemTemplate || pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP )	return FALSE;

	if ( bLogin == FALSE ) // 최초 접속시를 제외하고 체크
	{
		// 아이템의 내구도가 0이하면 착용불가능
		INT32 lCurrentDurability = 0;
		m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lCurrentDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
		if ( lCurrentDurability <= 0 )
		{
			INT32 lEquipPart	= pcsAgpdItemTemplate->m_nPart;
			if ( AGPMITEM_PART_BODY <= lEquipPart  && lEquipPart <= AGPMITEM_PART_HAND_RIGHT ) 
			{
				return FALSE;
			}
		}
	}

	// 해당 Region에서 착용할수 없는 아이템이면 착용 불가능하다.
	if(m_pagpmCharacter->IsPC(pcsCharacter) == TRUE)
	{
		if(!bLogin && IsEnableEquipItemInMyRegion(pcsCharacter, pcsItem) == FALSE)			
		{
			if(m_pagpmSystemMessage)
				m_pagpmSystemMessage->ProcessSystemMessage(0, AGPMSYSTEMMESSAGE_CODE_DISABLE_EQUIP_ITEM_THIS_REGION, -1, -1, (CHAR*)pcsItem->m_pcsItemTemplate->m_szName.c_str(), NULL, pcsCharacter);
			return FALSE;
		}
	}

	// 전투중에는 탈것 탈 수 없다. 2007.01.23. steeple
	if(((AgpdItemTemplateEquip*)pcsAgpdItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_RIDE && m_pagpmCharacter->IsCombatMode(pcsCharacter))
	{
		if( m_pagpmSystemMessage )
			m_pagpmSystemMessage->ProcessSystemMessage( 0 ,AGPMSYSTEMMESSAGE_CODE_CANNOT_RIDE_WHILE_COMBAT, -1, -1, NULL, NULL, pcsCharacter);
		return FALSE;
	}

	if (AGPDCHAR_SPECIAL_STATUS_DISARMAMENT & pcsCharacter->m_ulSpecialStatus)
	{
		if( m_pagpmSystemMessage )
			m_pagpmSystemMessage->ProcessSystemMessage( 0 ,AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS, -1, -1, NULL, NULL, pcsCharacter);
		return FALSE;
	}

	AgpdGridItem* pcsAgpdGridItem = NULL;
	BOOL bIsAvatarItem = pcsAgpdItemTemplate->IsAvatarEquip();

	INT32 lEquipPart	= pcsAgpdItemTemplate->m_nPart;
	switch( lEquipPart )
	{
	case AGPMITEM_PART_ACCESSORY_RING1:
	case AGPMITEM_PART_ACCESSORY_RING2:
		{
			lEquipPart = AGPMITEM_PART_ACCESSORY_RING1;
			pcsAgpdGridItem = GetEquipItem( pcsCharacter, AGPMITEM_PART_ACCESSORY_RING1 );
			if ( pcsAgpdGridItem )
			{
				lEquipPart = AGPMITEM_PART_ACCESSORY_RING2;
				pcsAgpdGridItem = GetEquipItem( pcsCharacter, AGPMITEM_PART_ACCESSORY_RING2 );
			}
		}
		break;
	case AGPMITEM_PART_HAND_LEFT:
		{
			if( !bLogin && GetWeaponType(pcsItem->m_pcsItemTemplate) == (INT32)AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
			{
				AgpdItem* pcsRightItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
				if( !pcsRightItem || !pcsRightItem->m_pcsItemTemplate ||
					GetWeaponType(pcsRightItem->m_pcsItemTemplate) != (INT32)AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER )
				{
					// 이러면 착용 실패
					return FALSE;
				}
			}

			pcsAgpdGridItem = GetEquipItem( pcsCharacter, lEquipPart );
			if( !pcsAgpdGridItem )
			{
				AgpdItem	*pcsEquipItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
				if (pcsEquipItem && pcsEquipItem->m_pcsItemTemplate)
				{
					INT32	lRightHand	= 0;
					m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsEquipItem->m_pcsItemTemplate)->m_csFactor, &lRightHand, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);

					INT32	lLeftHand	= 0;
					m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lLeftHand, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);

					if( lLeftHand == 2 || lRightHand == 2 )
						pcsAgpdGridItem = pcsEquipItem->m_pcsGridItem;
				}
			}
			else
			{
				// 양손무기인지 본다.
				INT32	lHand	= 0;
				m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lHand, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);

				if (lHand == 2)
				{
					AgpdItem	*pcsEquipItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
					if (pcsEquipItem)
					{
						if( !bIsAvatarItem && !AddItemToInventory( pcsCharacter, pcsEquipItem ) )
						{
							OutputDebugString("AgpmItem::EquipItem() Error (88) !!!\n");
							return FALSE;
						}
					}
				}
			}
		}
		break;
	case AGPMITEM_PART_HAND_RIGHT:
		{
			// 2007.11.02. steeple
			// 왼손에 Dagger 를 들고 있다면
			AgpdItem* pcsLeftItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
			if(pcsLeftItem && GetWeaponType(pcsLeftItem->m_pcsItemTemplate) == (INT32)AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
			{
				// 새로 드는 무기가 Rapier 가 아니라면 내려줘야 한다.
				if(GetWeaponType(pcsItem->m_pcsItemTemplate) != (INT32)AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER)
				{
					if(!AddItemToInventory(pcsCharacter, pcsLeftItem))
						return FALSE;
				}
			}

			pcsAgpdGridItem = GetEquipItem(pcsCharacter, lEquipPart);

			// 양손무기인지 본다.
			INT32	lHand	= 0;
			m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lHand, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);

			// 양손이라면 무조건 왼손 착용 아템을 인벤에 넣는다.
			if (lHand == 2)
			{
				AgpdItem	*pcsEquipItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
				if (pcsEquipItem)
				{
					if(!bIsAvatarItem && !AddItemToInventory(pcsCharacter, pcsEquipItem))
					{
						OutputDebugString("AgpmItem::EquipItem() Error (88) !!!\n");
						return FALSE;
					}
				}
			}
			else
			{
				AgpdItem	*pcsEquipItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
				if (pcsEquipItem)
				{
					INT32	lLeftHand	= 0;
					m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsEquipItem->m_pcsItemTemplate)->m_csFactor, &lLeftHand, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);

					if (lLeftHand == 2)
					{
						if(!bIsAvatarItem && !AddItemToInventory(pcsCharacter, pcsEquipItem))
						{
							OutputDebugString("AgpmItem::EquipItem() Error (88) !!!\n");
							return FALSE;
						}
					}
				}
			}
		}
		break;
	default:
		pcsAgpdGridItem = GetEquipItem( pcsCharacter, lEquipPart );
		break;
	}
	
	if( pcsAgpdGridItem )
	{
		if( bIsAvatarItem )		return FALSE;
			
		AgpdItem* pcsAgpdItemOld = GetItem( pcsAgpdGridItem );
		if( !pcsAgpdItemOld )
		{
			OutputDebugString("AgpmItem::EquipItem() Error (6) !!!\n");
			return FALSE;
		}

		// 있는 넘이랑 집어 넣으려는 놈이랑 같은 놈인지 검사한다.
		// 같은 놈이라면 굳이 빼지 않는다.
		if (pcsAgpdItemOld->m_lID != pcsItem->m_lID)
		{
			if(!AddItemToInventory(pcsCharacter, pcsAgpdItemOld))
			{
				OutputDebugString("AgpmItem::EquipItem() Error (8) !!!\n");
				return FALSE;
			}
		}
	}

	INT32	lEquipIndex	= GetEquipIndexFromTable(lEquipPart);
	if( lEquipIndex < 0 )		return FALSE;

	INT16 nLayer = m_pagpmGrid->GetLayerByIndex( &pcsItemADChar->m_csEquipGrid, lEquipIndex );
	INT16 nRow = m_pagpmGrid->GetRowByIndex( &pcsItemADChar->m_csEquipGrid, lEquipIndex );
	INT16 nColumn = m_pagpmGrid->GetColumnByIndex( &pcsItemADChar->m_csEquipGrid, lEquipIndex );

	// 아이템 상태를 저장한다.
	//ChangeItemOwner(pcsItem, pcsCharacter);

//	pcsItem->m_eStatus											= AGPDITEM_STATUS_EQUIP;
//	pcsItemADChar->m_lEquipSlot[pcsAgpdItemTemplate->m_nPart]	= AGPDITEM_MAKE_ITEM_ID(pcsItem->m_lID, pcsItem->m_lTID);

//	m_pagpmGrid->Add( &pcsItemADChar->m_csEquipGrid, nLayer, nRow, nColumn, pcsAgpdGridItem, 1, 1 );
//	 Callback을 불러준다.
//	EnumCallback(ITEM_CB_ID_CHAR_EQUIP, pcsItem, &bEmptyEquipSlot);

	BOOL	bResult	= FALSE;
	AgpdItemGridResult	eResult	= Insert(pcsItem, &pcsItemADChar->m_csEquipGrid, nLayer, nRow, nColumn);
	if( eResult == AGPDITEM_INSERT_SUCCESS )
	{
		RemoveStatus(pcsItem, AGPDITEM_STATUS_EQUIP);

		pcsItem->m_eStatus	= AGPDITEM_STATUS_EQUIP;

		EnumCallback(ITEM_CB_ID_CHAR_EQUIP, pcsItem, pcsCharacter);

		bResult	= TRUE;

		pcsItem->m_anPrevGridPos	= pcsItem->m_anGridPos;
	}

	// 성공여부에 관계없이 무조건 부른다. For Admin Client - 2004.04.01. steeple
	EnumCallback(ITEM_CB_ID_CHAR_EQUIP_FOR_ADMIN, pcsItem, NULL);

	return bResult;
}

/******************************************************************************
* Purpose : Unequip item
*
* 091202. Bob Jung
******************************************************************************/
BOOL AgpmItem::UnEquipItem(INT32 lCID, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem)
{
	if (!pcsAgpdItem)
		return FALSE;

	AgpdCharacter			*pcsAgpdCharacter = NULL;

	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::UnEquipItem() Error (1) !!!\n");
		return FALSE;
	}

	return UnEquipItem(pcsAgpdCharacter, pcsAgpdItem, bEquipDefaultItem);
}

/*AgpdItem *AgpmItem::FindCharacterDefaultItem(INT32 lCID, INT32 lTID)
{
	INT32 lIndex = 0;
	for(AgpdItem *pcsAgpdItem = GetItemSequence(&lIndex); pcsAgpdItem; pcsAgpdItem = GetItemSequence(&lIndex))
	{
		if((pcsAgpdItem->m_ulCID == lCID) && (pcsAgpdItem->m_lTID == lTID))
			return pcsAgpdItem;
	}

	return NULL;
}*/

BOOL AgpmItem::UnEquipItem(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem)
{
	if(!pcsCharacter || !pcsAgpdItem)
	{
		OutputDebugString("AgpmItem::UnEquipItem() Error (1) !!!\n");
		return FALSE;
	}

	// 현재 변신중인 경우라면 입고, 벗고를 할 수 없다.
	if ( (pcsCharacter->m_bIsTrasform || pcsCharacter->m_bIsEvolution) && 
		((AgpdItemTemplateEquip*)pcsAgpdItem->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_RIDE &&	// 변신중에도 탈것은 타거나 내릴수 있어야 한다.
		!CheckUseItem(pcsCharacter, pcsAgpdItem))	// 변신중에도 변경가능한 무기는 변경할 수 있어야 한다
		return FALSE;

	AgpdItemTemplateEquip		*pcsItemTemplateEquip	= (AgpdItemTemplateEquip *)(GetItemTemplate(pcsAgpdItem->m_lTID));
	AgpdItemADChar				*pcsADChar				= GetADCharacter(pcsCharacter);

	INT16				nLayer, nRow, nColumn;

	if((!pcsItemTemplateEquip) || (!pcsADChar) || (pcsItemTemplateEquip->m_nType != AGPMITEM_TYPE_EQUIP))
	{
		OutputDebugString("AgpmItem::UnEquipItem() Error (3) !!!\n");
		return FALSE;
	}

	//JK_독핸드 사용시 아이템 탈착으로 스텟 증가 시키는 버그수정
	if (AGPDCHAR_SPECIAL_STATUS_DISARMAMENT & pcsCharacter->m_ulSpecialStatus)
	{
		if( m_pagpmSystemMessage )
			m_pagpmSystemMessage->ProcessSystemMessage( 0 ,AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS, -1, -1, NULL, NULL, pcsCharacter);
		return FALSE;
	}

	// 내리는 아이템이 오른손 Rapier 이고, 왼손에 들고 있는 아이템을 구해서 Dagger 라면 내려준다. 2007.11.02. steeple
	if(GetWeaponType(pcsAgpdItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER)
	{
		AgpdItem* pcsLeftItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
		if(pcsLeftItem && pcsLeftItem->m_pcsItemTemplate && pcsLeftItem->m_lID != pcsAgpdItem->m_lID &&
			GetWeaponType(pcsLeftItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
		{
			// 레이피어는 이미 벗어서 인벤토리에 넣었으므로 단검을 벗어 넣을 1칸이 남아있는지 검사한다.
			if(m_pagpmGrid->GetEmpyGridCount(GetInventory(pcsCharacter)) < 1)
				return FALSE;

			if(!AddItemToInventory(pcsCharacter, pcsLeftItem))
					return FALSE;
		}
	}

	nLayer = m_pagpmGrid->GetLayerByIndex( &pcsADChar->m_csEquipGrid, GetEquipIndexFromTable(pcsItemTemplateEquip->m_nPart) );
	nRow = m_pagpmGrid->GetRowByIndex( &pcsADChar->m_csEquipGrid, GetEquipIndexFromTable(pcsItemTemplateEquip->m_nPart) );
	nColumn = m_pagpmGrid->GetColumnByIndex( &pcsADChar->m_csEquipGrid, GetEquipIndexFromTable(pcsItemTemplateEquip->m_nPart) );

	if (!RemoveItemFromGrid(&pcsADChar->m_csEquipGrid, -1, pcsCharacter, pcsAgpdItem))
		return FALSE;

/*	m_pagpmGrid->Clear( &pcsADChar->m_csEquipGrid, nLayer, nRow, nColumn, 1, 1 );
	pcsAgpdItem->m_eStatus									= AGPDITEM_STATUS_NONE;
	*/

	return EnumCallback(ITEM_CB_ID_CHAR_UNEQUIP, pcsAgpdItem, &bEquipDefaultItem);
}

AgpdItem* AgpmItem::GetEquipWeapon(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdItem *pcsWeaponItem = NULL;

	if (pcsCharacter->m_bRidable)
	{
		pcsWeaponItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_LANCER);
		if (pcsWeaponItem)
			return pcsWeaponItem;
		else
			return NULL;
	}

	pcsWeaponItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if (pcsWeaponItem)
		return pcsWeaponItem;

	pcsWeaponItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
	if (pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if (((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW ||
			((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW
			|| ((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON
			|| ((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON)
			return pcsWeaponItem;
	}

	return NULL;
}

BOOL AgpmItem::IsEquipWeapon(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdGridItem		*pcsWeaponGridItem	= GetEquipItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if (pcsWeaponGridItem)
		return TRUE;

	AgpdItem			*pcsWeaponItem		= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
	if (pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if (((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW ||
			((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW
			|| ((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON
			|| ((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON)
			return TRUE;
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipOneHandSword(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(!pcsWeaponItem)
		return FALSE;

	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_SWORD)
			return TRUE;
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipOneHandAxe(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(!pcsWeaponItem)
		return FALSE;

	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_AXE)
			return TRUE;
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipBlunt(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// 한손인지 양손인지 모르니 둘다 검사해야 한다.

	// 먼저 오른손 부터 검사
	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			(((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_MACE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_HAMMER ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_MACE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_HAMMER)
			)
			return TRUE;
	}

	// 왼손 검사
	pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			(((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_MACE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_HAMMER ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_MACE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_HAMMER)
			)
			return TRUE;
	}

	return FALSE;
}

BOOL	AgpmItem::IsEquipTwoHandBlunt(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// 양손다 동시에 검사.
	// 문엘프용 이도류를 체크함.

	AgpdItem* pcsWeaponItemRight = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	AgpdItem* pcsWeaponItemLeft = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);

	if(	pcsWeaponItemRight	&& pcsWeaponItemRight->m_pcsItemTemplate	&&
		pcsWeaponItemLeft	&& pcsWeaponItemLeft->m_pcsItemTemplate		)
	{
		AgpmItemEquipKind		eKindRight	= ((AgpdItemTemplateEquip*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nKind;
		AgpmItemEquipWeaponType eTypeRight	= ((AgpdItemTemplateEquipWeapon*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nWeaponType;
		AgpmItemEquipKind		eKindLeft	= ((AgpdItemTemplateEquip*)pcsWeaponItemLeft->m_pcsItemTemplate)->m_nKind;
		AgpmItemEquipWeaponType eTypeLeft	= ((AgpdItemTemplateEquipWeapon*)pcsWeaponItemLeft->m_pcsItemTemplate)->m_nWeaponType;

		if( eKindLeft	== AGPMITEM_EQUIP_KIND_WEAPON					&&
			eKindRight	== AGPMITEM_EQUIP_KIND_WEAPON					&&
			eTypeRight	== AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER	&&
			eTypeLeft	== AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER	)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipTwoHandSlash(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			(((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SWORD ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_AXE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_POLEARM ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SCYTHE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CLAW ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WING)
			)
			return TRUE;
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipStaff(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STAFF)
			return TRUE;
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipWand(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WAND)
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmItem::IsEquipBow(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdItem		*pcsWeapon	= GetEquipWeapon(pcsCharacter);
	if (pcsWeapon &&
		pcsWeapon->m_pcsItemTemplate &&
		((AgpdItemTemplateEquipWeapon *) pcsWeapon->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW)
		return TRUE;

	return FALSE;
}

BOOL AgpmItem::IsEquipCrossBow(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdItem		*pcsWeapon	= GetEquipWeapon(pcsCharacter);
	if (pcsWeapon &&
		pcsWeapon->m_pcsItemTemplate &&
		((AgpdItemTemplateEquipWeapon *) pcsWeapon->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW)
		return TRUE;

	return FALSE;
}

BOOL AgpmItem::IsEquipShield(AgpdCharacter* pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	// 왼손 검사
	AgpdItem* pcsShield = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
	if(pcsShield && pcsShield->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsShield->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD)
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmItem::IsEquipKatariya(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_KATARIYA)
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmItem::IsEquipChakram(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHAKRAM)
			return TRUE;
	}

	return FALSE;
}

// 2005.11.02. steeple.
// 원정대 깃발 들고 있는 지 쳌흐.
BOOL AgpmItem::IsEquipStandard(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_LANCER);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD)
			return TRUE;
	}

	return FALSE;
}

// 2007.10.29. steeple
// 오른손 무기만 들고 있는지 체크
BOOL AgpmItem::IsEquipRightHandOnly(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// 오른손에 무기가 있고 왼손은 비어있어야 한다.
	AgpdItem* pcsWeaponItemRight = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	AgpdItem* pcsWeaponItemLeft = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);

	if(pcsWeaponItemRight && pcsWeaponItemRight->m_pcsItemTemplate && !pcsWeaponItemLeft)
	{
		AgpmItemEquipKind		eKindRight	= ((AgpdItemTemplateEquip*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nKind;
		AgpmItemEquipWeaponType eTypeRight	= ((AgpdItemTemplateEquipWeapon*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nWeaponType;
		if(	eKindRight	== AGPMITEM_EQUIP_KIND_WEAPON)//					&&
			//eTypeRight	== AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgpmItem::IsEquipCharon(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItemLeft = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);

	if(pcsWeaponItemLeft && pcsWeaponItemLeft->m_pcsItemTemplate)
	{
		AgpmItemEquipKind		eKindLeft	= ((AgpdItemTemplateEquip*)pcsWeaponItemLeft->m_pcsItemTemplate)->m_nKind;
		AgpmItemEquipWeaponType eTypeLeft	= ((AgpdItemTemplateEquipWeapon*)pcsWeaponItemLeft->m_pcsItemTemplate)->m_nWeaponType;
		if(	eKindLeft	== AGPMITEM_EQUIP_KIND_WEAPON && eTypeLeft	== AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgpmItem::IsEquipZenon(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItemRight = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);

	if(pcsWeaponItemRight && pcsWeaponItemRight->m_pcsItemTemplate)
	{
		AgpmItemEquipKind		eKindRight	= ((AgpdItemTemplateEquip*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nKind;
		AgpmItemEquipWeaponType eTypeRight	= ((AgpdItemTemplateEquipWeapon*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nWeaponType;
		if(	eKindRight	== AGPMITEM_EQUIP_KIND_WEAPON && eTypeRight == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/******************************************************************************
* Purpose : Equip 패킷 파싱.
*
* 091602. Bob Jung
******************************************************************************/
BOOL AgpmItem::ParseEquipPacket(PVOID pEquip, INT32 *plCID)
{
	if (!pEquip || !plCID)
		return FALSE;

	m_csPacketEquip.GetField(FALSE, pEquip, *((UINT16 *)(pEquip)),
		                     plCID);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 060803. BOB
******************************************************************************/
AgpdItem *AgpmItem::GetEquipSlotItem(AgpdCharacter *pstAgpdCharacter, AgpmItemPart ePart)
{
	if (!pstAgpdCharacter)
		return NULL;

	AgpdGridItem	*pcsAgpdGridItem = GetEquipItem( pstAgpdCharacter, ePart );

	if (pcsAgpdGridItem)
	{
		if (pcsAgpdGridItem->GetParentBase())
			return (AgpdItem *) pcsAgpdGridItem->GetParentBase();
		else
			return GetItem( pcsAgpdGridItem->m_lItemID );
	}

	return NULL;
}

/******************************************************************************
* 장착되어 있는 아이템중 Durability에 관계된 장비를 가져오는 함수
* Out	 :   INT32 (지금 착용하고 있는 아이템의 갯수)
			 pArrEquipItem (AgpdItem* 들의 배열)
* In	 :   pcsCharacter (해당하는 캐릭터정보)
			 lMax (배열의 최대값)
******************************************************************************/

INT32 AgpmItem::GetEquipItems(AgpdCharacter *pcsCharacter, INT32* pArrEquipItem, INT32 lMax)
{
	if(!pcsCharacter || !pArrEquipItem)
		return NULL;

	INT32	  nEquipCount = 0;

	for(INT32 lEquipPart=AGPM_DURABILITY_EQUIP_PART_HEAD; lEquipPart < lMax; ++lEquipPart)
	{
		AgpdItem *EquipItem = GetEquipSlotItem(pcsCharacter, (AgpmItemPart)s_lBeDurabilityEquipItem[lEquipPart]);
		if(EquipItem)
		{
			pArrEquipItem[nEquipCount] = lEquipPart;
			nEquipCount++;
		}
	}

	return nEquipCount;
}

/******************************************************************************
******************************************************************************/

// 2007.08.07. steeple
BOOL AgpmItem::EquipAvatarItem(INT32 lCID, INT32 lID, BOOL bCheckUseItem)
{
	if(!lCID || !lID)
		return FALSE;

	return EquipAvatarItem(m_pagpmCharacter->GetCharacter(lCID), GetItem(lID), bCheckUseItem);
}

// 2007.08.07. steeple
BOOL AgpmItem::EquipAvatarItem(AgpdCharacter* pcsCharacter, AgpdItem* pcsAgpdItem, BOOL bCheckUseItem)
{
	if(!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	return TRUE;
}

// 2007.08.07. steeple
BOOL AgpmItem::UnEquipAvatarItem(INT32 lCID, INT32 lID, BOOL bEquipDefaultItem)
{
	if(!lCID || !lID)
		return FALSE;

	return UnEquipAvatarItem(m_pagpmCharacter->GetCharacter(lCID), GetItem(lID), bEquipDefaultItem);
}

// 2007.08.07. steeple
BOOL AgpmItem::UnEquipAvatarItem(AgpdCharacter* pcsCharacter, AgpdItem* pcsAgpdItem, BOOL bEquipDefaultItem)
{
	if(!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	return TRUE;
}
