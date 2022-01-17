#include "AgsmItem.h"
#include "AuTimeStamp.h"
#include "AgsmTitle.h"

//		CallbackInit
//	Functions
//		- 새 아이템이 추가되었다. 이 아이템 정보(Detail Info)를 아이템 소유주한테 보낸다.
//	Arguments
//		- pData : 추가된 아이템 데이타
//		- pClass : this module class pointer
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackInit(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	if (pItem->m_pcsCharacter)
	{
		if (pThis->m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
			return TRUE;

		if (!pItem->m_pcsCharacter->m_bIsAddMap)
			return TRUE;
	}

	AgsdItem	*pcsAgsdItem = pThis->GetADItem(pItem);
	if (!pcsAgsdItem)
		return FALSE;

	// 임시로 DBID를 IID로 넣는다.
	//pcsAgsdItem->m_ullDBIID = pItem->m_lID;

	// DB에 저장해야 하는지 함 알아본다.
	if (pItem->m_pcsCharacter &&
		pcsAgsdItem->m_bIsNeedInsertDB)
	{
		AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pItem->m_pcsCharacter);
		if (pcsAgsdCharacter && pcsAgsdCharacter->m_szAccountID && pcsAgsdCharacter->m_szAccountID[0])
		{
			AgsdServer		*pcsRelayServer	= pThis->m_pAgsmServerManager->GetRelayServer();
			if (pcsRelayServer && pcsRelayServer->m_dpnidServer != 0)
			{
				pThis->SendRelayInsert(pItem);
				pcsAgsdItem->m_bIsNeedInsertDB = FALSE;
			}
		}
	}

	pcsAgsdItem->m_lPrevSaveStackCount	= pItem->m_nCount;

	return pThis->SendNewItem(pItem);
}


BOOL AgsmItem::CallbackNewItemToClient(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	return pThis->SendNewItemToClient(pItem);
}

//		CallbackEquip
//	Functions
//		- 아이템을 착용했다. 이 아이템의 보이는 정보(view info)를 주변에 있는 캐릭터들에게 전송한다.
//			(아이템 소유주도 이 패킷을 받고 클라이언트에서 착용하게 된다. operation은 add로 보낸다.)
//	Arguments
//		- pData : 추가된 아이템 데이타
//		- pClass : this module class pointer
//	Return value
//		- BOOL : 패킷 전송 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackEquip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackEquip");

	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CallbackEquip"));

	if (!pItem->m_pcsItemTemplate)
		return FALSE;

	AgpdCharacter	*pCharacter = NULL;
	
	if (pThis->m_pagpmCharacter)
		pCharacter = pItem->m_pcsCharacter;

	if (!pCharacter)
		return FALSE;

	AgsdItemADChar	*pcsAttachedData	= pThis->GetADCharacter(pCharacter);

	INT32	nEquipPart	= (INT32) ((AgpdItemTemplateEquip *) pItem->m_pcsItemTemplate)->m_nPart;

	ASSERT(nEquipPart >= AGPMITEM_PART_BODY && nEquipPart < AGPMITEM_PART_NUM);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket	= pThis->m_pagpmItem->MakePacketItemView(pItem, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	pThis->m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pCharacter->m_lID);

	if (pCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD)
	{
		if (pThis->m_pagsmAOIFilter)
			pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pCharacter->m_stPos, PACKET_PRIORITY_4);
	}
	else
	{
		AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pCharacter);
		if (pcsAgsdCharacter->m_dpnidCharacter)
		{
			if (!pThis->SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter, PACKET_PRIORITY_4))
			{
//				TRACEFILE("AgsmItem : failed SendPacket() in CallbackEquip()");

				return FALSE;
			}
		}
	}

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	/*
	pcsAttachedData->m_pvPacketEquipItem[nEquipPart] = pThis->MakePacketItemView(pItem, &pcsAttachedData->m_lPacketEquipItemLength[nEquipPart]);

	if (!pcsAttachedData->m_pvPacketEquipItem[nEquipPart])
		return FALSE;

	if (pCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD)
	{
		if (pThis->m_pagsmAOIFilter)
			pThis->m_pagsmAOIFilter->SendPacketNear(pcsAttachedData->m_pvPacketEquipItem[nEquipPart], pcsAttachedData->m_lPacketEquipItemLength[nEquipPart], pCharacter->m_stPos);
	}
	else
	{
		AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pCharacter);
		if (pcsAgsdCharacter->m_dpnidCharacter)
		{
			if (!pThis->SendPacket(pcsAttachedData->m_pvPacketEquipItem[nEquipPart], pcsAttachedData->m_lPacketEquipItemLength[nEquipPart], pcsAgsdCharacter->m_dpnidCharacter))
			{
				WriteLog(AS_LOG_DEBUG, "AgsmItem : failed SendPacket() in CallbackEquip()");

				return FALSE;
			}
		}
	}
	*/

	/*
	// factor를 추가한다.
	if (pItem->m_csFactor.m_bPoint)
		pThis->m_pagpmFactors->CalcFactor(&pCharacter->m_csFactorPoint, &pItem->m_csFactor, TRUE, FALSE);
	else
		pThis->m_pagpmFactors->CalcFactor(&pCharacter->m_csFactorPercent, &pItem->m_csFactor, TRUE, FALSE);
	*/

	if (pItem->m_pcsCharacter)
	{
		if (pThis->m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
			return TRUE;

		if (!pItem->m_pcsCharacter->m_bIsAddMap)
			return TRUE;
	}

	// DB에 변경내용을 적용시킨다.
	if (AGSMSERVER_TYPE_GAME_SERVER == pThis->m_pAgsmServerManager->GetThisServerType() &&
		pCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD &&
		pThis->m_pagsmCharacter->GetAccountID(pCharacter))
	{
		pThis->SendRelayUpdate(pItem);
	}

	// 내구도가 0인것은 equip 상태라도 무시한다.
	INT32	lDurability		= 0;
	INT32	lTemplateMaxDurability	= 0;
	pThis->m_pagpmFactors->GetValue(&pItem->m_csFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
	pThis->m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pItem->m_pcsItemTemplate)->m_csFactor, &lTemplateMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);

	// Max 레벨 제한 체크도 들어간다. 2008.01.28. steeple
	INT32 lCharLevel	= pThis->m_pagpmCharacter->GetLevel(pCharacter);
	INT32 lLimitedLevel	= pItem->m_pcsItemTemplate->m_lLimitedLevel;
	INT32 lMinLevel		= pThis->m_pagpmFactors->GetLevel(&pItem->m_csRestrictFactor);

	BOOL bTransform = FALSE;

	if ((lTemplateMaxDurability == 0 || lDurability >= 1) && 
		pThis->m_pagpmCharacter->IsPC(pCharacter) && 
		(lLimitedLevel == 0 || lLimitedLevel >= lCharLevel) &&	// 최대 레벨 초과
		(lMinLevel <= lCharLevel) &&							// 최소 레벨 미만.
		pThis->m_pagpmItem->CheckUseItem(pCharacter, pItem))
	{
		// 옵션 스킬 데이터 계산 먼저 한다. 2006.12.26. steeple
		pThis->CalcItemOptionSkillData(pItem, TRUE);

		AgpdFactor pcsUpdateFactor;
		AgpdFactor pcsUpdateFactorPercent;

		AgpdItem* pcsItemL		= pThis->m_pagpmItem->GetEquipSlotItem(pCharacter, AGPMITEM_PART_HAND_LEFT);
		AgpdItem* pcsItemR		= pThis->m_pagpmItem->GetEquipSlotItem(pCharacter, AGPMITEM_PART_HAND_RIGHT);

		pThis->m_pagpmFactors->InitFactor(&pcsUpdateFactor);
		pThis->m_pagpmFactors->InitFactor(&pcsUpdateFactorPercent);

		pThis->m_pagpmFactors->CopyFactor(&pcsUpdateFactor, &pItem->m_csFactor, TRUE);
		pThis->m_pagpmFactors->CopyFactor(&pcsUpdateFactorPercent, &pItem->m_csFactorPercent, TRUE);

		if(pThis->m_pagpmItem->CheckUseItem(pCharacter, pcsItemL) && pThis->m_pagpmItem->CheckUseItem(pCharacter, pcsItemR))
		{
			pThis->m_pagpmItem->AdjustDragonScionWeaponFactor(pCharacter, pItem, &pcsUpdateFactor, &pcsUpdateFactorPercent, bTransform);

			if(bTransform == FALSE)
				bTransform = TRUE;
		}
		
		pThis->m_pagpmFactors->CalcFactor(&pCharacter->m_csFactorPoint, &pcsUpdateFactor, FALSE, FALSE, TRUE, FALSE);
		pThis->m_pagpmFactors->CalcFactor(&pCharacter->m_csFactorPercent, &pcsUpdateFactorPercent, FALSE, FALSE, TRUE, FALSE);

		pThis->AdjustRecalcFactor(pCharacter, pItem, TRUE);

		if (((AgpdItemTemplateEquip*)pItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_RIDE)
			pThis->EnumCallback(AGSMITEM_CB_RIDE_MOUNT, pCharacter, &pItem->m_lID);
		else if (TRUE == pCharacter->m_bRidable)
			pThis->m_pagsmCharacter->ReCalcCharacterFactors(pCharacter, TRUE);
		else
		{
			if (AGPMITEM_EQUIP_KIND_WEAPON == ((AgpdItemTemplateEquip*)pItem->m_pcsItemTemplate)->m_nKind)
				if (AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE == ((AgpdItemTemplateEquipWeapon *) pItem->m_pcsItemTemplate)->m_nWeaponType)
				{
					pThis->m_pagsmCharacter->ReCalcCharacterFactors(pCharacter, TRUE);
					return TRUE;
				}

			pThis->m_pagsmCharacter->ReCalcCharacterResultFactors(pCharacter);
		}
	}

	return TRUE;
}

BOOL AgsmItem::CallbackUnEquip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackUnEquip");

	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CallbackUnEquip"));

	if (!pItem->m_pcsItemTemplate)
		return FALSE;

	AgpdCharacter	*pCharacter = NULL;
	
	if (pThis->m_pagpmCharacter)
		pCharacter = pItem->m_pcsCharacter;

	if (!pCharacter)
		return FALSE;

	AgsdItemADChar	*pcsAttachedData	= pThis->GetADCharacter(pCharacter);

	INT32	nEquipPart	= (INT32) ((AgpdItemTemplateEquip *) pItem->m_pcsItemTemplate)->m_nPart;

	ASSERT(nEquipPart >= AGPMITEM_PART_BODY && nEquipPart < AGPMITEM_PART_NUM);

	if (pcsAttachedData->m_pvPacketEquipItem[nEquipPart])
	{
		pThis->m_csPacket.FreeStaticPacket(pcsAttachedData->m_pvPacketEquipItem[nEquipPart]);

		pcsAttachedData->m_pvPacketEquipItem[nEquipPart]		= NULL;
		pcsAttachedData->m_lPacketEquipItemLength[nEquipPart]	= 0;
	}

	//pThis->m_pagsmCharacter->ReCalcCharacterFactors(pCharacter);

	// character factor 에서 pItem 의 factor를 뺀다.

	/*
	if (pItem->m_csFactor.m_bPoint)
		pThis->m_pagpmFactors->CalcFactor(&pCharacter->m_csFactorPoint, &pItem->m_csFactor, FALSE);
	else
		pThis->m_pagpmFactors->CalcFactor(&pCharacter->m_csFactorPercent, &pItem->m_csFactor, FALSE);
	*/

	if (pItem->m_pcsCharacter)
	{
		if (pThis->m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
			return TRUE;

		if (!pItem->m_pcsCharacter->m_bIsAddMap)
			return TRUE;
	}

	// 내구도가 0인것은 equip 상태라도 무시한다.
	INT32	lDurability		= 0;
	INT32	lTemplateMaxDurability	= 0;
	pThis->m_pagpmFactors->GetValue(&pItem->m_csFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
	pThis->m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pItem->m_pcsItemTemplate)->m_csFactor, &lTemplateMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);

	// Max 레벨 제한 체크도 들어간다. 2008.01.28. steeple
	INT32 lCharLevel = pThis->m_pagpmCharacter->GetLevel(pCharacter);
	INT32 lLimitedLevel = pItem->m_pcsItemTemplate->m_lLimitedLevel;
	INT32 lMinLevel		= pThis->m_pagpmFactors->GetLevel(&pItem->m_csRestrictFactor);

	if ((lTemplateMaxDurability == 0 || lDurability >= 1) && 
		pThis->m_pagpmCharacter->IsPC(pCharacter) && 
		(lLimitedLevel == 0 || lLimitedLevel >= lCharLevel) &&	// 최대 레벨 초과
		(lMinLevel <= lCharLevel) &&							// 최소 레벨 미만.
		pThis->m_pagpmItem->CheckUseItem(pCharacter, pItem))
	{
		BOOL bCalcFactor = TRUE;

		AgpdItem* pcsItemL		= pThis->m_pagpmItem->GetEquipSlotItem(pCharacter, AGPMITEM_PART_HAND_LEFT);
		AgpdItem* pcsItemR		= pThis->m_pagpmItem->GetEquipSlotItem(pCharacter, AGPMITEM_PART_HAND_RIGHT);

		if(pThis->m_pagpmItem->GetWeaponType(pItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON)
		{
			if(pcsItemL && pThis->m_pagpmItem->CheckUseItem(pCharacter, pcsItemL) && 
				pThis->m_pagpmItem->GetWeaponType(pcsItemL->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON)
			{
				bCalcFactor = FALSE;
			}
		}

		if(pThis->m_pagpmItem->GetWeaponType(pItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON)
		{
			if(pcsItemR && pThis->m_pagpmItem->CheckUseItem(pCharacter, pcsItemR) && 
				pThis->m_pagpmItem->GetWeaponType(pcsItemR->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON)
			{
				bCalcFactor = FALSE;
			}
		}

		AgpdFactor	csItemUpdateFactorPoint;
		AgpdFactor	csItemUpdateFactorPercent;
		pThis->m_pagpmFactors->InitFactor(&csItemUpdateFactorPoint);
		pThis->m_pagpmFactors->InitFactor(&csItemUpdateFactorPercent);

		pThis->m_pagpmFactors->CopyFactor(&csItemUpdateFactorPoint, &pItem->m_csFactor, TRUE);
		pThis->m_pagpmFactors->CopyFactor(&csItemUpdateFactorPercent, &pItem->m_csFactorPercent, TRUE);

		if(!bCalcFactor)
		{
			pThis->m_pagpmFactors->SetValue(&csItemUpdateFactorPoint, 0, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
			pThis->m_pagpmFactors->SetValue(&csItemUpdateFactorPercent, 0, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
		}

		pThis->m_pagpmFactors->CalcFactor(&pCharacter->m_csFactorPoint, &csItemUpdateFactorPoint, FALSE, FALSE, FALSE, FALSE);
		pThis->m_pagpmFactors->CalcFactor(&pCharacter->m_csFactorPercent, &csItemUpdateFactorPercent, FALSE, FALSE, FALSE, FALSE);
		
		
		// 뺄 때는 먼저 값을 빼주고, 아이템 데이터를 다시 계산한다. 2006.12.26. steeple
		pThis->CalcItemOptionSkillData(pItem, FALSE);

//		for (int i = 0; i < pItem->m_stConvertHistory.lConvertLevel; ++i)
//		{
//			if (pItem->m_stConvertHistory.bUseSpiritStone[i])
//				continue;
//
//			pThis->m_pagpmFactors->CalcFactor(&pCharacter->m_csFactorPoint, &pItem->m_stConvertHistory.csFactorHistory[i], FALSE, FALSE, FALSE);
//		}

		if (((AgpdItemTemplateEquip*)pItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_RIDE)
			pThis->EnumCallback(AGSMITEM_CB_RIDE_DISMOUNT, pCharacter, &pItem->m_lID);
		else if (TRUE == pCharacter->m_bRidable)
			pThis->m_pagsmCharacter->ReCalcCharacterFactors(pCharacter, TRUE);
		else
		{
			if (AGPMITEM_EQUIP_KIND_WEAPON == ((AgpdItemTemplateEquip*)pItem->m_pcsItemTemplate)->m_nKind)
				if (AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE == ((AgpdItemTemplateEquipWeapon *) pItem->m_pcsItemTemplate)->m_nWeaponType)
				{
					pThis->m_pagsmCharacter->ReCalcCharacterFactors(pCharacter, TRUE);
					return TRUE;
				}

			pThis->AdjustRecalcFactor(pCharacter, pItem, FALSE);
			pThis->m_pagsmCharacter->ReCalcCharacterResultFactors(pCharacter);
		}
	}

	return TRUE;
}

BOOL AgsmItem::CallbackRemoveForNearCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)	pData;

	if (!pcsItem->m_pcsCharacter)
		return TRUE;

	INT16			nPacketLength	= 0;
	PVOID			pvPacket		= pThis->MakePacketItemRemove(pcsItem, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	pThis->m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItem->m_pcsCharacter->m_lID);

	BOOL			bSendResult		= pThis->m_pagsmAOIFilter->SendPacketNearExceptSelf(pvPacket, nPacketLength,
												pcsItem->m_pcsCharacter->m_stPos,
												pThis->m_pagpmCharacter->GetRealRegionIndex(pcsItem->m_pcsCharacter),
												pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter),
												PACKET_PRIORITY_3);

	pThis->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmItem::CallbackUseItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)	pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pCustData;

	AgpdCharacter	*pcsItemOwner	= pcsItem->m_pcsCharacter;

	//STOPWATCH2(pThis->GetModuleName(), _T("CallbackUseItem"));

	// 2005.09.29. steeple
	// 과연 이게 맞는 것일까.... -0-
	if(pcsItemOwner == NULL)
		return FALSE;

	// 말을 탄 상태에서는 변신물약을 사용못한다.
	if (TRUE == pcsItemOwner->m_bRidable || TRUE == pcsItemOwner->m_bIsTrasform)
	{
		if ( (AGPMITEM_TYPE_USABLE == pcsItem->m_pcsItemTemplate->m_nType) &&
			 (AGPMITEM_USABLE_TYPE_TRANSFORM == ((AgpdItemTemplateUsable *)(pcsItem->m_pcsItemTemplate))->m_nUsableItemType) )
			 return FALSE;

		if ( (AGPMITEM_TYPE_USABLE == pcsItem->m_pcsItemTemplate->m_nType) &&
			 (AGPMITEM_USABLE_TYPE_SKILL_SCROLL == ((AgpdItemTemplateUsable *)(pcsItem->m_pcsItemTemplate))->m_nUsableItemType) )
		{
			if (AGPMITEM_USABLE_SCROLL_SUBTYPE_ATTACK_SPEED == ((AgpdItemTemplateUsableSkillScroll *)(pcsItem->m_pcsItemTemplate))->m_eScrollSubType)
				return FALSE;

			if (AGPMITEM_USABLE_SCROLL_SUBTYPE_MOVE_SPEED == ((AgpdItemTemplateUsableSkillScroll *)(pcsItem->m_pcsItemTemplate))->m_eScrollSubType)
				return FALSE;
		}
	}

	AgpdItemCooldownBase stCooldownBase = pThis->m_pagpmItem->GetCooldownBase(pcsItemOwner, ((AgpdItemTemplate*)pcsItem->m_pcsItemTemplate)->m_lID);

	if (stCooldownBase.m_lTID == 0 && pcsItem->m_pcsGridItem->m_ulUseItemTime + pcsItem->m_pcsGridItem->m_ulReuseIntervalTime > pThis->GetClockCount())
	{
		if( 1054 == pcsItem->m_pcsItemTemplate->m_lID)
		{
			INT16	nPacketLength	= 0;
			PVOID	pvPacket		= pThis->MakePacketUseItemResult(pcsItem->m_pcsCharacter, AGPMITEM_USE_RESULT_FAILED_REVERSEORB, &nPacketLength);

			if( pvPacket && nPacketLength > sizeof(PACKET_HEADER) )
				pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
		}

		return FALSE;
	}
		
	// Cooldown 쪽도 체크한다. 2008.02.18. steeple
	else if(stCooldownBase.m_lTID != 0 && stCooldownBase.m_bPause == FALSE && stCooldownBase.m_ulRemainTime > 0)
	{
		if( 1054 == stCooldownBase.m_lTID )
		{
			INT16	nPacketLength	= 0;
			PVOID	pvPacket		= pThis->MakePacketUseItemResult(pcsItem->m_pcsCharacter, AGPMITEM_USE_RESULT_FAILED_REVERSEORB, &nPacketLength);

			if ( pvPacket && nPacketLength > sizeof(PACKET_HEADER) )
				pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
		}

		return FALSE;
	}

	// 이런식의 쿨다운 타임 체크는 약간의 latency 가 있다. 위의 체크만 하도록 한다.
	//// cooldown time 체크 2008.02.15. steeple
	//if(pThis->m_pagpmItem->GetRemainTimeByTID(pcsCharacter, pcsItem->m_pcsItemTemplate->m_lID) > 0)
	//	return FALSE;

	INT32			lTID			= ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lID;

	BOOL			bUseResult		= pThis->UseItem(pcsItem, TRUE, pcsCharacter);

	if(bUseResult)
		pThis->m_pagsmTitle->OnItemUse(pcsCharacter, pcsItem);

	// 2008.02.13. steeple
	// 사용이 성공했다면, UseInterval 을 설정해 둔다.
	UINT32 ulUseInterval = 0;
	if(bUseResult && pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
	{
		if(stCooldownBase.m_lTID != 0 && stCooldownBase.m_bPause)
			ulUseInterval = stCooldownBase.m_ulRemainTime;
		else
			ulUseInterval = pThis->m_pagpmItem->GetReuseInterval(pcsItem->m_pcsItemTemplate);

		if(ulUseInterval > 0)
		{
			// 아래의 UseInterval 은 템플릿 값만 가져왔다. (물약 딜레이 현상 때문에..) 2008.04.15. steeple
			if(stCooldownBase.m_lTID != 0 || ((AgpdItemTemplateUsable*)pcsItem->m_pcsItemTemplate)->m_ulUseInterval > 1000)	// 이미 있거나 1초 초과인 녀석들만 한다.
			{
				pThis->m_pagpmItem->SetCooldownByTID(pcsItem->m_pcsCharacter, lTID, ulUseInterval, 0);
				pThis->m_pagsmCharacter->SetIdleInterval(pcsItem->m_pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
			}

			if(pcsItem->m_pcsGridItem)
				pcsItem->m_pcsGridItem->SetUseItemTime(pThis->GetClockCount(), ulUseInterval);

		}
	}

	if (IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType))
	{
		BOOL bSendPacket = TRUE;
		if (pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
		{
			AgpdItemTemplateUsable	*pcsTemplateUsable = (AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate;
			if (pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL ||
				pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
				bSendPacket = FALSE;
		}

		if (bSendPacket)
		{
			if (bUseResult)
			{
				pThis->SendPacketUseItemSuccess(pcsItemOwner, pcsItem);

				// 쿨다운 세팅 된 것만 보내준다. 2008.04.15. steeple
				AgpdItemCooldownBase stCooldownBase = pThis->m_pagpmItem->GetCooldownBase(pcsItemOwner, lTID);
				if(stCooldownBase.m_lTID != 0)
					pThis->SendPacketUpdateCooldown(pcsItemOwner, stCooldownBase);	// 위의 함수에서 같이 보내고 싶었으나, 위의 함수를 따로 부르는 곳이 있어서 분리. 2008.02.18. steeple
			}
			else
				pThis->SendPacketUseItemFailByTID(pcsItemOwner, lTID, pcsCharacter);
		}
	}
	else
	{
		if (bUseResult)
			pThis->SendPacketUseItemByTID(pcsItemOwner, lTID, pcsCharacter, ulUseInterval);
		else
			pThis->SendPacketUseItemFailByTID(pcsItemOwner, lTID, pcsCharacter);
	}

	return TRUE;
}

BOOL AgsmItem::CallbackUnuseItem(PVOID pData, PVOID pClass, PVOID pCustData)	// ITEM_CB_ID_UNUSE_ITEM
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)	pData;

	AgpdCharacter	*pcsItemOwner	= pcsItem->m_pcsCharacter;

	if(pcsItemOwner == NULL) return FALSE;

	if (pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_UNUSE) return FALSE;

	// 2008.02.21. steeple
	// PC 방 전용 아이템은 PC 방이 아닌 곳에서는 정지가 되어야 하므로 서버상에서의 체크는 통과시켜준다.
	// 클라이언트에서는 버튼 비활성화 때문에, 아래 CheckEnagleStopCashItem 을 수정하기는 애매하다.
	//
	// 사용 중지 가능 여부 확인
	if (!pThis->m_pagpmItem->CheckEnableStopCashItem(pcsItem) && !(pcsItem->m_pcsItemTemplate->m_lIsUseOnlyPCBang & AGPMITEM_PCBANG_TYPE_USE_ONLY))
	{
		// 사용중지가 불가능합니다라는 메시지 전송
		pThis->m_pagsmSystemMessage->SendSystemMessage(pcsItemOwner, AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_CANNOT_UNUSE_ITEM);
		return FALSE;
	}

	// 2006.02.03. steeple
	// 현재 상태가 SkillBlock 상태라면 UnuseItem 못해야 한다.
	// 중요한 건, 클라이언트에서 요청할 때만 이렇게 처리해야 한다.
	// 서버에서는 SkillBlock 과 상관없이 중지해야할 경우가 있기 때문에.
	// 서버에서 강제로 중지시킬 경우는 아래 UnuseItem 을 직접 불러서 처리한다.
	if (pThis->m_pagpmCharacter->IsActionBlockCondition(pcsItemOwner, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL))
	{
		// 사용중지가 불가능합니다라는 메시지 전송
		pThis->m_pagsmSystemMessage->SendSystemMessage(pcsItemOwner, AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_CANNOT_UNUSE_STATUS);
		return FALSE;
	}

	BOOL  bUnuseResult = pThis->UnuseItem(pcsItem);

	// If the item has a stamina value, do time check once again.
	if(pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_STAMINA &&
		pcsItem->m_llStaminaRemainTime != 0)
	{
		pThis->m_pagsmCharacter->SetIdleInterval(pcsItemOwner, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
	}
	else if ( pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_ONE_ATTACK )
	{
		pThis->m_pagsmCharacter->SetIdleInterval(pcsItemOwner, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
	}

	return bUnuseResult;
}

//		CallbackAddInventory
//	Functions
//		- 아이템이 인벤토리에 추가되었다. 아이템 소유주는 이미 이 아이템의 세부정보를 다 알고 있다.
//		  그러므로 Update 패킷으로 아템 상태, 인벤토리내 위치만 보낸다.
//	Arguments
//		- pData : 추가된 아이템 데이타
//		- pClass : this module class pointer
//	Return value
//		- BOOL : 패킷 전송 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackAddInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	if (pItem->m_pcsCharacter)
	{
		if (pThis->m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
			return TRUE;

		if (!pItem->m_pcsCharacter->m_bIsAddMap)
			return TRUE;
	}

	AgpdCharacter	*pCharacter = NULL;
	
	if (pThis->m_pagpmCharacter)
		pCharacter = pItem->m_pcsCharacter;

	if (!pCharacter)
		return FALSE;

	// DB에 변경내용을 적용시킨다.

	/*
	if (AGSMSERVER_TYPE_GAME_SERVER == pThis->m_pAgsmServerManager->GetThisServerType() &&
		pCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD &&
		pThis->m_pagsmCharacter->GetAccountID(pCharacter))
	{
		pThis->SendRelayUpdate(pItem);
	}
	*/

	pThis->AddItemToDB(pItem);

	AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pCharacter);
	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return FALSE;

	//INT16	nPacketLength	= 0;
	//PVOID	pvPacket		= pThis->m_pagpmItem->MakePacketItem(pItem, &nPacketLength);

	if(pItem->m_lExpireTime > 0)
		pThis->m_pagsmCharacter->SetIdleInterval(pCharacter, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);

	if( pcsAgsdCharacter->GetGachaBlock() )
	{
		// do nothing..
		// 패킷 따로 보냄
		pcsAgsdCharacter->AddGachaItemUpdate( AgsdCharacter::GachaDelayItemUpdateData::NORMAL , pItem );
		return TRUE;
	}
	else
		return pThis->SendPacketItem(pItem, pcsAgsdCharacter->m_dpnidCharacter);

	/*
	INT16 nPacketLength;
	INT16 nOperation = AGPMITEM_PACKET_OPERATION_UPDATE;

	PVOID pvPacketInventory = pThis->m_pagpmItem->m_csPacketInventory.MakePacket(FALSE, &nPacketLength, AGPMITEM_PACKET_TYPE, 
													&pItem->m_anGridPos[0],
													&pItem->m_anGridPos[1],
													&pItem->m_anGridPos[2]);

	if (!pvPacketInventory)
		return FALSE;

	PVOID pvPacket = pThis->m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE, 
													&nOperation,			//Operation
													&pItem->m_eStatus,		//Status
													&pItem->m_lID,			//ItemID
													&pItem->m_lTID,			//ItemTemplateID
													&pCharacter->m_lID,		//ItemOwnerID
													&pItem->m_nCount,		//ItemCount
													NULL,					//Field
													pvPacketInventory,		//Inventory
													NULL,					//Bank
													NULL,					//Equip
													NULL,					//Factors
													NULL,
													NULL,					//TargetItemID
													NULL,
													NULL,
													NULL,
													NULL,					//Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	pThis->m_pagpmItem->m_csPacketInventory.FreePacket(pvPacketInventory);

	if (!pvPacket)
		return FALSE;

	if (!pThis->SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter))
	{
		pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

//		TRACEFILE("AgsmItem : failed SendPacket() in CallbackAddInventory()");

		return FALSE;
	}

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return TRUE;
	*/
}

//		CallbackAddCashInventory
//	Functions
//		- 아이템이 Cash 인벤토리에 추가되었다. 아이템 소유주는 이미 이 아이템의 세부정보를 다 알고 있다.
//		  그러므로 Update 패킷으로 아템 상태, 인벤토리내 위치만 보낸다.
//	Arguments
//		- pData : 추가된 아이템 데이타
//		- pClass : this module class pointer
//	Return value
//		- BOOL : 패킷 전송 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackAddCashInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	if (pItem->m_pcsCharacter)
	{
		if (pThis->m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
			return TRUE;

		if (!pItem->m_pcsCharacter->m_bIsAddMap)
			return TRUE;
	}

	AgpdCharacter	*pCharacter = NULL;

	if (pThis->m_pagpmCharacter)
		pCharacter = pItem->m_pcsCharacter;

	if (!pCharacter)
		return FALSE;

	// DB에 변경내용을 적용시킨다.
	pThis->AddItemToDB(pItem);

	AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pCharacter);
	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return FALSE;

	if( pcsAgsdCharacter->GetGachaBlock() )
	{
		// do nothing..
		// 패킷 따로 보냄
		pcsAgsdCharacter->AddGachaItemUpdate( AgsdCharacter::GachaDelayItemUpdateData::NORMAL , pItem );
		return TRUE;
	}
	else
		return pThis->SendPacketItem(pItem, pcsAgsdCharacter->m_dpnidCharacter);
}

BOOL AgsmItem::CallbackAddItemToQuest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	if (pItem->m_pcsCharacter)
	{
		if (pThis->m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
			return TRUE;

		if (!pItem->m_pcsCharacter->m_bIsAddMap)
			return TRUE;
	}

	AgpdCharacter	*pCharacter = NULL;
	
	if (pThis->m_pagpmCharacter)
		pCharacter = pItem->m_pcsCharacter;

	if (!pCharacter)
		return FALSE;

	// DB에 변경내용을 적용시킨다.

	/*
	if (AGSMSERVER_TYPE_GAME_SERVER == pThis->m_pAgsmServerManager->GetThisServerType() &&
		pCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD &&
		pThis->m_pagsmCharacter->GetAccountID(pCharacter))
	{
		pThis->SendRelayUpdate(pItem);
	}
	*/

	pThis->AddItemToDB(pItem);

	return TRUE;
}

BOOL AgsmItem::CallbackRemoveInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	/*
	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	AgpdCharacter	*pCharacter = NULL;
	
	if (pThis->m_pagpmCharacter)
		pCharacter = pItem->m_pcsCharacter;

	if (!pCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pCharacter);
	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return FALSE;

	INT16 nPacketLength;
	INT16 nOperation = AGPMITEM_PACKET_OPERATION_REMOVE;

	PVOID pvPacketInventory = pThis->m_pagpmItem->m_csPacketInventory.MakePacket(FALSE, &nPacketLength, AGPMITEM_PACKET_TYPE, 
													&pItem->m_anInventoryPos[0],
													&pItem->m_anInventoryPos[1],
													&pItem->m_anInventoryPos[2]);

	if (!pvPacketInventory)
		return FALSE;

	PVOID pvPacket = pThis->m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE, 
													&nOperation,			//Operation
													&pItem->m_eStatus,		//Status
													&pItem->m_lID,			//ItemID
													NULL,					//ItemTemplateID
													&pCharacter->m_lID,		//ItemOwnerID
													NULL,					//ItemCount
													NULL,					//Field
													pvPacketInventory,		//Inventory
													NULL,					//Bank
													NULL,					//Equip
													NULL,					//Factors
													NULL);					//TargetItemID

	pThis->m_pagpmItem->m_csPacketInventory.FreePacket(pvPacketInventory);

	if (!pvPacket)
		return FALSE;

	if (!pThis->SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter))
	{
		pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

		WriteLog(AS_LOG_DEBUG, "AgsmItem : failed SendPacket() in CallbackAddInventory()");

		return FALSE;
	}

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);
	*/

	return TRUE;
}

BOOL AgsmItem::CallbackUpdateTradeGrid(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	AgpdCharacter	*pCharacter = NULL;
	
	if (pThis->m_pagpmCharacter)
		pCharacter = pItem->m_pcsCharacter;

	if (!pCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pCharacter);
	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return FALSE;

	INT16 nPacketLength;
	INT8 nOperation = AGPMITEM_PACKET_OPERATION_UPDATE;

	PVOID pvPacketInventory = pThis->m_pagpmItem->m_csPacketInventory.MakePacket(FALSE, &nPacketLength, AGPMITEM_PACKET_TYPE, 
													&pItem->m_anGridPos[0],
													&pItem->m_anGridPos[1],
													&pItem->m_anGridPos[2]);

	if (!pvPacketInventory)
		return FALSE;

	INT8 eStatus = (INT8)pItem->m_eStatus;

	PVOID pvPacket = pThis->m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE, 
													&nOperation,			//Operation
													&eStatus,		//Status
													&pItem->m_lID,			//ItemID
													NULL,					//ItemTemplateID
													&pCharacter->m_lID,		//ItemOwnerID
													&pItem->m_nCount,		//ItemCount
													NULL,					//Field
													pvPacketInventory,		//Inventory
													NULL,					//Bank
													NULL,					//Equip
													NULL,					//Factors
													NULL,
													NULL,					//TargetItemID
													NULL,
													NULL,
													NULL,
													NULL,					// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,					// SkillPlus
													NULL,
													NULL,
													NULL);

	pThis->m_pagpmItem->m_csPacketInventory.FreePacket(pvPacketInventory);

	if (!pvPacket)
		return FALSE;

	if (!pThis->SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter))
	{
		pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

//		TRACEFILE("AgsmItem : failed SendPacket() in CallbackAddInventory()");

		return FALSE;
	}

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmItem::CallbackUpdateSalesBoxGrid(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	AgpdCharacter	*pCharacter = NULL;
	
	if (pThis->m_pagpmCharacter)
		pCharacter = pItem->m_pcsCharacter;

	if (!pCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pCharacter);
	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return FALSE;

	// DB에 변경내용을 적용시킨다.
	if (AGSMSERVER_TYPE_GAME_SERVER == pThis->m_pAgsmServerManager->GetThisServerType() &&
		pCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD &&
		pThis->m_pagsmCharacter->GetAccountID(pCharacter))
	{
		pThis->SendRelayUpdate(pItem);
	}

	INT16 nPacketLength;
	INT8 nOperation = AGPMITEM_PACKET_OPERATION_UPDATE;

	PVOID pvPacketInventory = pThis->m_pagpmItem->m_csPacketInventory.MakePacket(FALSE, &nPacketLength, AGPMITEM_PACKET_TYPE, 
													&pItem->m_anGridPos[0],
													&pItem->m_anGridPos[1],
													&pItem->m_anGridPos[2]);

	if (!pvPacketInventory)
		return FALSE;

	INT8 eStatus = (INT8)pItem->m_eStatus;

	PVOID pvPacket = pThis->m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE, 
													&nOperation,			//Operation
													&eStatus,		//Status
													&pItem->m_lID,			//ItemID
													NULL,					//ItemTemplateID
													&pCharacter->m_lID,		//ItemOwnerID
													&pItem->m_nCount,		//ItemCount
													NULL,					//Field
													pvPacketInventory,		//Inventory
													NULL,					//Bank
													NULL,					//Equip
													NULL,					//Factors
													NULL,
													NULL,					//TargetItemID
													NULL,
													NULL,
													NULL,
													NULL,					// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,					// SkillPlus
													NULL,
													NULL,
													NULL);

	pThis->m_pagpmItem->m_csPacketInventory.FreePacket(pvPacketInventory);

	if (!pvPacket)
		return FALSE;

	if (!pThis->SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter))
	{
		pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

//		TRACEFILE("AgsmItem : failed SendPacket() in CallbackAddInventory()");

		return FALSE;
	}

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

//		CallbackAddField
//	Functions
//		- 아이템이 필드에 추가되었다. 필드에 추가된 아이템은 모두 view info만 필요하기땜시 view info만 주변에 보내준다.
//	Arguments
//		- pData : 추가된 아이템 데이타
//		- pClass : this module class pointer
//	Return value
//		- BOOL : 패킷 전송 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackAddField(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackAddField");

	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis = (AgsmItem *) pClass;
	AgpdItem	*pItem = (AgpdItem *) pData;

	AgsdItem	*pcsAgsdItem	= pThis->GetADItem(pItem);

	/*
	// DB에 변경내용을 적용시킨다.
	if (pcsAgsdItem && !pcsAgsdItem->m_bIsNeedInsertDB)
	{
		pThis->SendRelayUpdate(pItem);
	}
	*/

	INT16 nPacketLength;
	INT16 nOperation = AGPMITEM_PACKET_OPERATION_ADD;
	PVOID pvPacket = pThis->m_pagpmItem->MakePacketItemView(pItem, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	if (pThis->m_pagsmAOIFilter)
		pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pItem->m_posItem, PACKET_PRIORITY_3);

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	pThis->EnumCallback(AGSMITEM_CB_SEND_ITEM_VIEW, pItem, NULL);

	pcsAgsdItem->m_ulDropTime	= pThis->GetClockCount();

	//pThis->m_csAdminFieldItem.AddObject(&pItem->m_lID, pItem->m_lID);

	return TRUE;
}

BOOL AgsmItem::CallbackRemoveField(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmItem	*pThis	= (AgsmItem *) pClass;
	AgpdItem	*pItem	= (AgpdItem *) pCustData; 

	//return pThis->m_csAdminFieldItem.RemoveObject(pItem->m_lID);
	return TRUE;
}

BOOL AgsmItem::CallbackChangeOwner(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackChangeOwner");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem			*pThis			= (AgsmItem *)		pClass;
	AgpdItem			*pcsItem		= (AgpdItem *)		pData;
	INT32				lPrevOwner		= *(INT32 *)		pCustData;

	if (pcsItem->m_pcsCharacter)
	{
		if (pThis->m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
			return TRUE;

		if (!pcsItem->m_pcsCharacter->m_bIsAddMap)
			return TRUE;
	}

	// 주인이 바뀌었다. 상황에 따라 DB에 반영한다.
	//////////////////////////////////////////////////////////////

	AgsdItem			*pcsAgsdItem	= (AgsdItem *)		pThis->GetADItem(pcsItem);

	if (pcsItem->m_ulCID == AP_INVALID_CID)
	{
		if (pcsAgsdItem->m_ullDBIID != 0)
		{
			// 아템의 정보를 업데이트 시킨다.
			if (!pcsAgsdItem->m_bIsNeedInsertDB)
			{
				pThis->SendRelayUpdate(pcsItem);
			}

			/*
			// 아템을 DB에서 삭제한다. (정책적인 문제인데 일단은 삭제 안해도 될듯하다)
			if (pThis->m_pagsmServerManager->GetThisServerType() == AGSMSERVER_TYPE_GAME_SERVER)
				return pThis->DeleteItemDB(pcsItem);
			*/
		}
	}
	else
	{
		/*
		AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsItem->m_pcsCharacter);
		if (!pcsAgsdCharacter)
			return FALSE;

		// DB에 저장해야 하는지 함 알아본다.
		if (pcsItem->m_pcsCharacter &&
			pcsAgsdItem->m_bIsNeedInsertDB)
		{
			if (pThis->m_pagsmCharacter->GetAccountID(pcsItem->m_pcsCharacter))
			{
				AgsdServer		*pcsRelayServer	= pThis->m_pAgsmServerManager->GetRelayServer();
				if (pcsRelayServer && pcsRelayServer->m_dpnidServer != 0)
				{
					pThis->SendRelayInsert(pcsItem);

					// 2004.07.30. steeple
					pThis->WriteOwnerChangeLog(pcsItem->m_pcsCharacter->m_lID, pcsItem, lPrevOwner);

//					for (int i = 0; i < pcsItem->m_stConvertHistory.lConvertLevel; ++i)
//					{
//						if (pcsItem->m_pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD &&
//							pThis->m_pagsmServerManager->GetThisServerType() == AGSMSERVER_TYPE_GAME_SERVER)
//						{
//							pThis->EnumCallback(AGSMITEM_CB_INSERT_CONVERT_HISTORY_TO_DB, pcsItem, NULL);
//						}
//					}

					pcsAgsdItem->m_bIsNeedInsertDB = FALSE;
				}
			}
		}
		else if (AGSMSERVER_TYPE_GAME_SERVER == pThis->m_pAgsmServerManager->GetThisServerType() &&
				pcsItem->m_pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD &&
				pThis->m_pagsmCharacter->GetAccountID(pcsItem->m_pcsCharacter))
		{
			pThis->SendRelayUpdate(pcsItem);

			// 2004.07.30. steeple
			pThis->WriteOwnerChangeLog(pcsItem->m_pcsCharacter->m_lID, pcsItem, lPrevOwner);
		}
		*/

		/*
		if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		{
			// 아템을 DB에서 삭제한다. (정책적인 문제인데 일단은 삭제 안해도 될듯하다)
		}
		else
		{
			// DB에 새로 저장한다.
			if (pThis->m_pagsmServerManager->GetThisServerType() == AGSMSERVER_TYPE_GAME_SERVER)
				return pThis->InsertItemDB(pcsItem);
		}
		*/
	}

	return TRUE;
}

BOOL AgsmItem::CallbackUpdateBank(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis		= (AgsmItem *)	pClass;
	AgpdItem	*pcsItem	= (AgpdItem *)	pData;

	if (pcsItem->m_ulCID == AP_INVALID_CID)
		return FALSE;

	if (!pcsItem->m_pcsCharacter)
	{
		pcsItem->m_pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pcsItem->m_ulCID);
		if (!pcsItem->m_pcsCharacter)
			return FALSE;
	}

	// DB에 변경내용을 적용시킨다.
	if (AGSMSERVER_TYPE_GAME_SERVER == pThis->m_pAgsmServerManager->GetThisServerType() &&
		pcsItem->m_pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD &&
		pThis->m_pagsmCharacter->GetAccountID(pcsItem->m_pcsCharacter))
	{
		pThis->SendRelayUpdate(pcsItem);
	}

	// log
	if (pCustData)		// 뱅크내 이동은 제외한다.
	{
		BOOL bLog = *(BOOL *) pCustData;
		if (bLog && pcsItem->m_pcsCharacter->m_bIsAddMap)	// 게임에 들어올때 Add하는 것은 제외.
		{
			pThis->WriteItemLog(AGPDLOGTYPE_ITEM_BANK_IN,
								pcsItem->m_pcsCharacter->m_lID,
								pcsItem,
								pcsItem->m_nCount ? pcsItem->m_nCount : 1
								);
		}
	}

	// pcsItem->m_pcsCharacter 에게 pcsItem의 상태가 Bank로 바뀌었다고 알려준다.

	if (pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter) == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->MakePacketItemBank(pcsItem, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmItem::CallbackRemoveBank(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis		= (AgsmItem *)	pClass;
	AgpdItem	*pcsItem	= (AgpdItem *)	pData;
	AgpdItemStatus eStatus	= (AgpdItemStatus)PtrToInt(pCustData);

	if (pcsItem->m_ulCID == AP_INVALID_CID)
		return FALSE;

	if (!pcsItem->m_pcsCharacter)
	{
		pcsItem->m_pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pcsItem->m_ulCID);
		if (!pcsItem->m_pcsCharacter)
			return FALSE;
	}

	// log
	if (AGPDITEM_STATUS_NONE != eStatus)	// 로그 아웃 제외.
	{
		pThis->WriteItemLog(AGPDLOGTYPE_ITEM_BANK_OUT,
							pcsItem->m_pcsCharacter->m_lID,
							pcsItem,
							pcsItem->m_nCount ? pcsItem->m_nCount : 1
							);
	}
	
	return TRUE;
}

BOOL AgsmItem::CallbackUpdateQuest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis		= (AgsmItem *)	pClass;
	AgpdItem	*pcsItem	= (AgpdItem *)	pData;

	if (pcsItem->m_ulCID == AP_INVALID_CID)
		return FALSE;

	if (!pcsItem->m_pcsCharacter)
	{
		pcsItem->m_pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pcsItem->m_ulCID);
		if (!pcsItem->m_pcsCharacter)
			return FALSE;
	}

	// DB에 변경내용을 적용시킨다.
	if (AGSMSERVER_TYPE_GAME_SERVER == pThis->m_pAgsmServerManager->GetThisServerType() &&
		pcsItem->m_pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD &&
		pThis->m_pagsmCharacter->GetAccountID(pcsItem->m_pcsCharacter))
	{
		pThis->SendRelayUpdate(pcsItem);
	}


	if (pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter) == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->MakePacketItemQuest(pcsItem, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}
//		CallbackSyncAddChar
//	Functions
//		- 맵에 추가된 캐릭터에 대한 정보를 주변에 뿌려준다. 
//			(이넘이 입고 있는 아이템 정보를 주변놈들에게 준다.)
//	Arguments
//		- pData : 아템 정보를 보내줘야할 캐릭터 데이타 포인터
//		- pClass : this module class pointer
//	Return value
//		- BOOL : 패킷 전송 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackSyncAddChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackSyncAddChar");

	if (!pData || !pClass)
		return FALSE;

	AgpdCharacter	*pcsCharacter = (AgpdCharacter *) pData;
	AgsmItem		*pThis = (AgsmItem *) pClass;

	pThis->SyncAddChar(pcsCharacter);

	return TRUE;
}

/*
//		CallbackAddSector
//	Functions
//		- 맵에 아이템이 추가되었다. (필드에 추가된거다)
//			(아이템 정보를 주변놈들에게 준다.)
//	Arguments
//		- pData : 아템이 추가된 섹터 포인터
//		- pClass : this module class pointer
//		- pCustData : 추가된 아이템 아뒤
//	Return value
//		- BOOL : 패킷 전송 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackAddSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackAddSector");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis		= (AgsmItem *) pClass;
	ApWorldSector	*pSector	= (ApWorldSector *) pData;
	//AgpdItem		*pItem		= (AgpdItem *) (*(INT32 *) pCustData);

	INT32			lIID		= *(INT32 *) pCustData;
	AgpdItem		*pItem		= pThis->m_pagpmItem->GetItem(lIID);
	if (!pItem)
		return FALSE;

	// DB에 변경내용을 적용시킨다.
	if (AGSMSERVER_TYPE_GAME_SERVER == pThis->m_pagsmServerManager->GetThisServerType())
	{
		pThis->SendRelayUpdate(pItem);
	}

	AgsdAOIFilter	*pAOIFilter	= NULL;
	
	if (pThis->m_pagsmAOIFilter)
		pAOIFilter = pThis->m_pagsmAOIFilter->GetADMap(pSector);

	if (!pAOIFilter)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = pThis->MakePacketItemView(pItem, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	if (!pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pSector))
	{
		pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	AgsdItem		*pAgsdItem	= pThis->GetADItem(pItem);

	pAgsdItem->m_ulDropTime		= pThis->GetClockCount();

	pThis->m_csAdminFieldItem.AddObject(&pItem->m_lID, pItem->m_lID);

	return TRUE;
	//return pThis->AddIdleEvent(pThis->GetClockCount() + AGSMITEM_MAX_PRESERVE_ITEM_FIELD_TIME, pItem->m_lID, pClass, ProcessIdleRemoveFieldItem, NULL);
}

//		CallbackRemoveSector
//	Functions
//		- 맵에 있던 아이템이 삭제되었다. (필드에서 누가 주웠다)
//			(아이템이 없어졌단 정보를 보낸다.)
//	Arguments
//		- pData : 아템이 삭제된 섹터 포인터
//		- pClass : this module class pointer
//		- pCustData : 삭제된 아이템 아뒤
//	Return value
//		- BOOL : 패킷 전송 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackRemoveSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackRemoveSector");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis		= (AgsmItem *) pClass;
	ApWorldSector	*pSector	= (ApWorldSector *) pData;
	//AgpdItem		*pcsItem	= (AgpdItem *) (*(INT32 *) pCustData);

	INT32			lIID		= *(INT32 *) pCustData;
	AgpdItem		*pcsItem		= pThis->m_pagpmItem->GetItem(lIID);
	if (!pcsItem)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = pThis->MakePacketItemRemove(pcsItem, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	if (pcsItem->m_pcsCharacter)
	{
		if (pThis->m_pagsmAOIFilter && !pThis->m_pagsmAOIFilter->SendPacketNearExceptSelf(pvPacket, nPacketLength, pSector, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter)))
		{
			pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

			return FALSE;
		}
	}
	else
	{
		if (pThis->m_pagsmAOIFilter && !pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pSector))
		{
			pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

			return FALSE;
		}
	}

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	pThis->m_csAdminFieldItem.RemoveObject(pcsItem->m_lID);

	//pThis->RemoveIdleEvent(pcsItem->m_lID);

	return TRUE;
}
*/

//		CallbackAddCell
//	Functions
//		- 맵에 아이템이 추가되었다. (필드에 추가된거다)
//			(아이템 정보를 주변놈들에게 준다.)
//	Arguments
//		- pData : 아템이 추가된 섹터 포인터
//		- pClass : this module class pointer
//		- pCustData : 추가된 아이템 아뒤
//	Return value
//		- BOOL : 패킷 전송 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackAddCell(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackAddCell");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis		= (AgsmItem *) pClass;
	AgsmAOICell		*pcsCell	= (AgsmAOICell *) pData;
	AgpdItem		*pItem		= (AgpdItem *) pCustData;

	// DB에 변경내용을 적용시킨다.
	if (AGSMSERVER_TYPE_GAME_SERVER == pThis->m_pAgsmServerManager->GetThisServerType())
	{
		pThis->SendRelayUpdate(pItem);
	}

	INT16	nPacketLength = 0;
	PVOID	pvPacket = pThis->m_pagpmItem->MakePacketItemView(pItem, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	if (!pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCell, -1, PACKET_PRIORITY_3))
	{
		pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	pThis->EnumCallback(AGSMITEM_CB_SEND_ITEM_VIEW, pItem, NULL);

	AgsdItem		*pAgsdItem	= pThis->GetADItem(pItem);

	pAgsdItem->m_ulDropTime		= pThis->GetClockCount();

	pThis->m_csAdminFieldItem.AddObject(&pItem->m_lID, pItem->m_lID);

	return TRUE;
	//return pThis->AddIdleEvent(pThis->GetClockCount() + AGSMITEM_MAX_PRESERVE_ITEM_FIELD_TIME, pItem->m_lID, pClass, ProcessIdleRemoveFieldItem, NULL);
}

//		CallbackRemoveCell
//	Functions
//		- 맵에 있던 아이템이 삭제되었다. (필드에서 누가 주웠다)
//			(아이템이 없어졌단 정보를 보낸다.)
//	Arguments
//		- pData : 아템이 삭제된 섹터 포인터
//		- pClass : this module class pointer
//		- pCustData : 삭제된 아이템 아뒤
//	Return value
//		- BOOL : 패킷 전송 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackRemoveCell(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackRemoveCell");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis		= (AgsmItem *) pClass;
	AgsmAOICell		*pcsCell	= (AgsmAOICell *) pData;
	AgpdItem		*pcsItem	= (AgpdItem *) pCustData;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = pThis->MakePacketItemRemove(pcsItem, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	if (pcsItem->m_pcsCharacter)
	{
		if (pThis->m_pagsmAOIFilter && !pThis->m_pagsmAOIFilter->SendPacketNearExceptSelf(pvPacket, nPacketLength, pcsCell, -1, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter), PACKET_PRIORITY_3))
		{
			pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

			return FALSE;
		}
	}
	else
	{
		if (pThis->m_pagsmAOIFilter && !pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCell, -1, PACKET_PRIORITY_3))
		{
			pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

			return FALSE;
		}
	}

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	pThis->m_csAdminFieldItem.RemoveObject(pcsItem->m_lID);

	//pThis->RemoveIdleEvent(pcsItem->m_lID);

	return TRUE;
}

/*
//		CallbackConnectDB
//	Functions
//		- DB에 연결되었다.
//		- Generated Max Item DBID 를 가져와 세팅한다.
//	Arguments
//		- pData : NULL
//		- pClass : this module class pointer
//		- pCustData : NULL
//	Return value
//		- BOOL : 
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackConnectDB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmItem	*pThis	= (AgsmItem *)	pClass;

	AgsdServer	*pcsServer = pThis->m_pagsmServerManager->GetThisServer();
	if (!pcsServer)
		return FALSE;

	//stAgsmGenerateMaxIDDB	stGeneratedMaxID;

	//pThis->m_pagsmDBStream->InitGeneratedMaxID(&stGeneratedMaxID);

	//strncpy(stGeneratedMaxID.szServerName, ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szName, AGSM_MAX_SERVER_NAME);

	//return pThis->m_pagsmDBStream->GetGeneratedMaxItemIDDB(&stGeneratedMaxID, 0);
	return TRUE;
}

//		CallbackDisconnectDB
//	Functions
//		- DB와의 연결이 종료된다.
//		- Generated Max Item DBID를 저장한다.
//	Arguments
//		- pData : NULL
//		- pClass : this module class pointer
//		- pCustData : NULL
//	Return value
//		- BOOL : 
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackDisconnectDB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmItem	*pThis	= (AgsmItem *)	pClass;

	AgsdServer	*pcsServer = pThis->m_pagsmServerManager->GetThisServer();
	if (!pcsServer)
		return FALSE;

	//stAgsmGenerateMaxIDDB	stGeneratedMaxID;

	//pThis->m_pagsmDBStream->InitGeneratedMaxID(&stGeneratedMaxID);

	//strncpy(stGeneratedMaxID.szServerName, ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szName, AGSM_MAX_SERVER_NAME);
	//strncpy(stGeneratedMaxID.szGroupName, ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szGroupName, AGSM_MAX_SERVER_NAME);
	//stGeneratedMaxID.lMaxID = pThis->m_csGenerateID64.GetCurrentID();

	//return pThis->m_pagsmDBStream->UpdateGeneratedMaxItemIDDB(&stGeneratedMaxID, 0);
	return TRUE;
}
*/

/*
//		CallbackDBOperationResult
//	Functions
//		- DB query 결과를 받아서 처리한다.
//	Arguments
//		- pData : pstAgsmDBOperationResult
//		- pClass : this module pointer
//	Return value
//		- BOOL : 처리결과
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackDBOperationResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem					*pThis			= (AgsmItem *)					pClass;
	pstAgsmDBOperationResult	pstOperation	= (pstAgsmDBOperationResult)	pData;

	if (pstOperation->nDataType == AGSMDB_DATATYPE_MAXITEMID)
	{
		if (pstOperation->bSuccess)
		{
			// DB Operation이 성공했다.
			if (pstOperation->nOperation == AGSMDB_OPERATION_SELECT)
			{
				pstAgsmGenerateMaxIDDB pstGeneratedMaxID = (pstAgsmGenerateMaxIDDB) pstOperation->pvResultBuffer;
				if (pstGeneratedMaxID)
					return FALSE;

				return pThis->InitItemDBIDServer(pstGeneratedMaxID->lMaxID);
			}
		}
		else
		{
			// DB Operation이 실패했다.
			if (pstOperation->nOperation == AGSMDB_OPERATION_SELECT)
			{
				return FALSE;
			}
			else if (pstOperation->nOperation == AGSMDB_OPERATION_UPDATE)
			{
				// update 가 실패한경우 데이타가 없다고 가정하고 insert를 시도한다.
				AgsdServer	*pcsServer	= pThis->m_pagsmServerManager->GetThisServer();
				if (!pcsServer)
					return FALSE;

				stAgsmGenerateMaxIDDB	stGeneratedMaxID;

				pThis->m_pagsmDBStream->InitGeneratedMaxID(&stGeneratedMaxID);

				strncpy(stGeneratedMaxID.szServerName, ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szName, AGSM_MAX_SERVER_NAME);
				strncpy(stGeneratedMaxID.szGroupName, ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szGroupName, AGSM_MAX_SERVER_NAME);
				stGeneratedMaxID.lMaxID = pThis->m_csGenerateID64.GetCurrentID();

				if (pThis->m_pagsmDBStream->AddGeneratedMaxItemIDDB(&stGeneratedMaxID, 0))
				{
					return FALSE;
				}
			}
			else if (pstOperation->nOperation == AGSMDB_OPERATION_INSERT)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
*/

//		CallbackRemoveItemOnly
//	Functions
//		- 캐릭터는 두고 그넘이 가지고 있는 (그넘이 소유주인) 모든 아템을 삭제한다.
//	Arguments
//		- pData : 삭제한 캐릭터
//		- pClass : this module class pointer
//		- pCustData : not used
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackRemoveItemOnly(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem *pThis = (AgsmItem *) pClass;

	return pThis->m_pagpmItem->CBRemoveChar(pData, pThis->m_pagpmItem, pCustData);
}

//BOOL AgsmItem::CallbackUpdateEgoExp(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgsmItem		*pThis			= (AgsmItem *)		pClass;
//	AgpdItem		*pcsItem		= (AgpdItem *)		pData;
//	BOOL			bIsLevelUp		= *(BOOL *)			pCustData;
//
//	if (!pcsItem->m_pcsCharacter || pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter) == 0)
//		return FALSE;
//
//	// 에고 아이템 Exp가 변경되었다.
//	// 패킷을 만들어 보낸다.
//	// 레벨업이 된경우 레벨업 정보도 같이 보낸다.
//
//	PVOID	pvPacketFactor			= pThis->m_pagsmFactors->MakePacketUpdateEgoExp(&pcsItem->m_csFactor);
//
//	PVOID	pvPacketRestrictFactor	= NULL;
//	if (bIsLevelUp)
//		pvPacketRestrictFactor		= pThis->m_pagsmFactors->MakePacketUpdateLevel(&pcsItem->m_csRestrictFactor);
//
//	INT16	nPacketLength	= 0;
//	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_UPDATE;
//
//	PVOID	pvPacket = pThis->m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												NULL,
//												NULL,
//												&pcsItem->m_ulCID,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketFactor,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketRestrictFactor,
//												NULL
//												);
//
//	if (pvPacketFactor)
//		pThis->m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);
//	if (pvPacketRestrictFactor)
//		pThis->m_pagpmFactors->m_csPacket.FreePacket(pvPacketRestrictFactor);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
//
//	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}
//
//BOOL AgsmItem::CallbackUpdateEgoLevel(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass)
//		return FALSE;
//
//	AgsmItem		*pThis			= (AgsmItem *)		pClass;
//	AgpdItem		*pcsItem		= (AgpdItem *)		pData;
//
//	if (pcsItem->m_pcsCharacter)
//	{
//		pThis->m_pagsmCharacter->ReCalcCharacterFactors(pcsItem->m_pcsCharacter, TRUE);
//	}
//
//	return TRUE;
//}
//
//BOOL AgsmItem::CallbackPutSoulIntoCube(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgsmItem			*pThis			= (AgsmItem *)			pClass;
//	AgpdItem			*pcsSoulCube	= (AgpdItem *)			pData;
//	AgpmItemEgoResult	*pstEgoResult	= (AgpmItemEgoResult *)	pCustData;
//
//	if (!pcsSoulCube->m_pcsCharacter)
//		return FALSE;
//
//	PVOID	pvPacket = NULL;
//	INT16	nPacketLength = 0;
//
//	switch (*pstEgoResult) {
//	case AGPMITEM_EGO_PUT_SOUL_SUCCESS:
//		pvPacket = pThis->m_pagpmItem->MakePacketItemPutSoulResult(pcsSoulCube->m_lID, TRUE, &nPacketLength);
//		break;
//
//	case AGPMITEM_EGO_PUT_SOUL_FAIL:
//		pvPacket = pThis->m_pagpmItem->MakePacketItemPutSoulResult(pcsSoulCube->m_lID, FALSE, &nPacketLength);
//		break;
//
//	default:
//		return FALSE;
//		break;
//	}
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsSoulCube->m_pcsCharacter));
//
//	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}
//
//BOOL AgsmItem::CallbackUseSoulCube(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgsmItem			*pThis			= (AgsmItem *)			pClass;
//	AgpdItem			*pcsSoulCube	= (AgpdItem *)			pData;
//	AgpmItemEgoResult	*pstEgoResult	= (AgpmItemEgoResult *)	pCustData;
//
//	if (!pcsSoulCube->m_pcsCharacter ||
//		pThis->m_pagsmCharacter->GetCharDPNID(pcsSoulCube->m_pcsCharacter) == 0)
//		return FALSE;
//
//	PVOID	pvPacket = NULL;
//	INT16	nPacketLength = 0;
//
//	switch (*pstEgoResult) {
//	case AGPMITEM_EGO_PUT_SOUL_SUCCESS:
//		pvPacket = pThis->m_pagpmItem->MakePacketItemUseSoulCubeResult(pcsSoulCube->m_lID, TRUE, &nPacketLength);
//		break;
//
//	case AGPMITEM_EGO_PUT_SOUL_FAIL:
//		pvPacket = pThis->m_pagpmItem->MakePacketItemUseSoulCubeResult(pcsSoulCube->m_lID, FALSE, &nPacketLength);
//		break;
//
//	default:
//		return FALSE;
//		break;
//	}
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsSoulCube->m_pcsCharacter));
//
//	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}

//		CallbackSendCharEquipItem
//	Functions
//		- 캐릭터에 대한 입고있는 Equip 아이템 (즉, 보이는 정보) 를 보내준다.
//			(캐릭터 싱크 맞추는데 사용된다)
//	Arguments
//		- pData : 캐릭터 데이타
//		- pClass : this module class pointer
//		- pCustData : 데이타를 보낼 network id (DPNID)
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::CallbackSendCharEquipItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackSendCharEquipItem");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pCharacter		= (AgpdCharacter *) pData;

	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	UINT32			ulNID			= PtrToUint(ppvBuffer[0]);
	BOOL			bGroupNID		= PtrToInt(ppvBuffer[1]);
	BOOL			bIsExceptSelf	= PtrToInt(ppvBuffer[2]);

	if (ulNID == 0)
		return TRUE;

	AgsdItemADChar	*pcsAttachedData	= pThis->GetADCharacter(pCharacter);

	// 캐릭터가 입고 있는 equip item 들에 대한 view info를 보내준다.
	for (int i = 0; i < AGPMITEM_PART_NUM; ++i)
	{
		/*
		if (!pcsAttachedData->m_pvPacketEquipItem[i] ||
			pcsAttachedData->m_lPacketEquipItemLength[i] < 1)
			continue;

		if (bGroupNID)
		{
			if (bIsExceptSelf != 0)
				pThis->m_pagsmAOIFilter->SendPacketGroupExceptSelf(pcsAttachedData->m_pvPacketEquipItem[i], pcsAttachedData->m_lPacketEquipItemLength[i], ulNID, pThis->m_pagsmCharacter->GetCharDPNID(pCharacter));
			else
				pThis->m_pagsmAOIFilter->SendPacketGroup(pcsAttachedData->m_pvPacketEquipItem[i], pcsAttachedData->m_lPacketEquipItemLength[i], ulNID);
		}
		else
		{
			pThis->SendPacket(pcsAttachedData->m_pvPacketEquipItem[i], pcsAttachedData->m_lPacketEquipItemLength[i], ulNID);
		}
		*/

		AgpdItem	*pcsItem	= NULL;
		
		{
			PROFILE("AgsmItem::CallbackSendCharEquipItem - GetEquipSlotItem");

			pcsItem	= pThis->m_pagpmItem->GetEquipSlotItem(pCharacter, (AgpmItemPart) i);
		}

		if (pcsItem)
		{
			INT16	nPacketLength	= 0;
			PVOID	pvPacket		= NULL;
			
			{
				PROFILE("AgsmItem::CallbackSendCharEquipItem - MakePacketItemView");
				pvPacket	= pThis->m_pagpmItem->MakePacketItemView(pcsItem, &nPacketLength);
			}

			if (!pvPacket ||
				nPacketLength < 1)
				continue;

			{
				PROFILE("AgsmItem::CallbackSendCharEquipItem - SendPacket");
				pThis->m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pCharacter->m_lID);
				
				if (bGroupNID)
				{
					if (bIsExceptSelf != 0)
						pThis->m_pagsmAOIFilter->SendPacketGroupExceptSelf(pvPacket, nPacketLength, ulNID, pThis->m_pagsmCharacter->GetCharDPNID(pCharacter), PACKET_PRIORITY_3);
					else
						pThis->m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_3);
				}
				else
				{
					pThis->SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_3);
				}
			}

			pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

			PVOID	pvBuffer[3];
			pvBuffer[0] = UintToPtr(ulNID);
			pvBuffer[1] = IntToPtr(bGroupNID);

			if (bIsExceptSelf != 0)
				pvBuffer[2] = UintToPtr(pThis->m_pagsmCharacter->GetCharDPNID(pCharacter));
			else
				pvBuffer[2]	= NULL;

			{
				PROFILE("AgsmItem::CallbackSendCharEquipItem - Callback");

				pThis->EnumCallback(AGSMITEM_CB_SEND_ITEM_VIEW, pcsItem, pvBuffer);
			}
		}
	}

	/*
	for (int i = 0; i < AGPMITEM_PART_NUM; i++)
	{
		pcsAgpdGridItem = pThis->m_pagpmItem->GetEquipItem( pCharacter, i );

		if (pcsAgpdGridItem == NULL )
			continue;

		AgpdItem	*pItem = pThis->m_pagpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
		if (!pItem)
			continue;

		if (bIsExceptSelf)
			pThis->SendPacketItemView(pItem, ulNID, bGroupNID, pThis->m_pagsmCharacter->GetCharDPNID(pCharacter));
		else
			pThis->SendPacketItemView(pItem, ulNID, bGroupNID, 0);
	}
	*/

	return TRUE;
}

BOOL AgsmItem::CallbackZoningStart(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgsdServer		*pcsServer		= (AgsdServer *)	pCustData;

	if (pcsServer->m_dpnidServer == 0)
		return FALSE;

	return pThis->SendPacketItemAll(pcsCharacter, pcsServer->m_dpnidServer);	
}

BOOL AgsmItem::CallbackZoningPassControl(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgsdServer		*pcsServer		= (AgsdServer *)	pCustData;

	if (pcsServer->m_dpnidServer == 0)
		return FALSE;

	// pcsServer로 입고 있는 아이템을 제외한 나머지 모든 아템 정보를 보낸다.
	return pThis->SendPacketItemAll(pcsCharacter, pcsServer->m_dpnidServer);
}

BOOL AgsmItem::CallbackReCalcFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackReCalcFactor");

	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdGridItem	*pcsAgpdGridItem;

	BOOL bCheckDragonScionWeapon = FALSE;
	BOOL bUseBothWeapon = FALSE;

	for (int i = 0; i < AGPMITEM_PART_NUM; i++)
	{
		if (pcsCharacter->m_bRidable)
		{
			// 말에 탄 상태일때 무시해야 되는 아이템
			if (AGPMITEM_PART_HAND_LEFT == i) continue;
			if (AGPMITEM_PART_HAND_RIGHT == i) continue;
		}
		else
		{
			// 말에 타지 않았을때 무시해야 되는 아이템
			if (AGPMITEM_PART_LANCER == i) continue;
			if (AGPMITEM_PART_RIDE == i) continue;
		}

		pcsAgpdGridItem = pThis->m_pagpmItem->GetEquipItem( pcsCharacter, i );

		if (pcsAgpdGridItem == NULL)
			continue;

		AgpdItem	*pcsItem = pThis->m_pagpmItem->GetItem(pcsAgpdGridItem);
		if (!pcsItem)
			continue;

		// 내구도가 0인것은 equip 상태라도 무시한다.
		INT32	lDurability		= 0;
		INT32	lTemplateMaxDurability	= 0;
		pThis->m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
		pThis->m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lTemplateMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);

		INT32 lCharLevel	= pThis->m_pagpmCharacter->GetLevel(pcsCharacter);
		INT32 lLimitedLevel	= pcsItem->m_pcsItemTemplate->m_lLimitedLevel;
		INT32 lMinLevel		= pThis->m_pagpmFactors->GetLevel(&pcsItem->m_csRestrictFactor);

		if ((lTemplateMaxDurability > 0 && lDurability < 1) ||
			!pThis->m_pagpmCharacter->IsPC(pcsCharacter) ||
			(lLimitedLevel != 0 && lLimitedLevel < lCharLevel) ||	// 최대 레벨 초과
			(lMinLevel > lCharLevel) ||
			!pThis->m_pagpmItem->CheckUseItem(pcsCharacter, pcsItem) )
			continue;

		AgpdItem* pcsItemL		= pThis->m_pagpmItem->GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
		AgpdItem* pcsItemR		= pThis->m_pagpmItem->GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);

		AgpdFactor UpdateFactorPoint;
		AgpdFactor UpdateFactorPercent;

		pThis->m_pagpmFactors->InitFactor(&UpdateFactorPoint);
		pThis->m_pagpmFactors->InitFactor(&UpdateFactorPercent);

		pThis->m_pagpmFactors->CopyFactor(&UpdateFactorPoint, &pcsItem->m_csFactor, TRUE);
		pThis->m_pagpmFactors->CopyFactor(&UpdateFactorPercent, &pcsItem->m_csFactorPercent, TRUE);

		if(i == AGPMITEM_PART_HAND_LEFT || i == AGPMITEM_PART_HAND_RIGHT)
		{
			if(pThis->m_pagpmItem->CheckUseItem(pcsCharacter, pcsItemL) && pThis->m_pagpmItem->CheckUseItem(pcsCharacter, pcsItemR))
			{
				pThis->m_pagpmItem->AdjustDragonScionWeaponFactor(pcsCharacter, pcsItem, &UpdateFactorPoint, &UpdateFactorPercent, bCheckDragonScionWeapon);

				if(bUseBothWeapon == FALSE)
					bUseBothWeapon = TRUE;
			}
		}

		pThis->m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, &UpdateFactorPoint, FALSE, FALSE, TRUE, FALSE);
		pThis->m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, &UpdateFactorPercent, FALSE, FALSE, TRUE, FALSE);

		pThis->AdjustRecalcFactor(pcsCharacter, pcsItem, TRUE, bCheckDragonScionWeapon);

		if(bCheckDragonScionWeapon == FALSE && bUseBothWeapon == TRUE)
			bCheckDragonScionWeapon = TRUE;
	}

	return TRUE;
}

BOOL AgsmItem::CallbackUpdateFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)	pData;

	if (!pcsItem->m_pcsCharacter)
		pcsItem->m_pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pcsItem->m_ulCID);

	if (!pcsItem->m_pcsCharacter)
		return FALSE;

	pThis->m_pagsmCharacter->ReCalcCharacterFactors(pcsItem->m_pcsCharacter);

	return TRUE;
}

BOOL AgsmItem::CallbackSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	if (ulNID == 0)
		return FALSE;

	pThis->SendPacketItemADCharacter(pcsCharacter, ulNID);

	if (pThis->m_pagpmOptimizedPacket2)
	{
		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= pThis->m_pagpmOptimizedPacket2->MakePacketCharAllItemSkillExceptBankCash(pcsCharacter, &nPacketLength);

		if (pvPacket && nPacketLength > 0)
		{
			pThis->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

			pThis->SendPacket(pvPacket, nPacketLength, ulNID);

			pThis->m_csPacket.FreePacket(pvPacket);
		}

		nPacketLength	= 0;
		pvPacket		= pThis->m_pagpmOptimizedPacket2->MakePacketCharAllBankItem(pcsCharacter, &nPacketLength);

		if (pvPacket && nPacketLength > 0)
		{
			pThis->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

			pThis->SendPacket(pvPacket, nPacketLength, ulNID);

			pThis->m_csPacket.FreePacket(pvPacket);
		}

		INT32 lItemIndex = 0;
		nPacketLength	 = 0;
		pvPacket		 = pThis->m_pagpmOptimizedPacket2->MakePacketCharAllCashItem(pcsCharacter, &nPacketLength, &lItemIndex);

		if (pvPacket && nPacketLength > 0)
		{
			pThis->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);
			pThis->SendPacket(pvPacket, nPacketLength, ulNID);
			pThis->m_csPacket.FreePacket(pvPacket);
		}

		// Item의 크기가 커짐에 따라서 Cash item 200개를 한 패킷으로 보낼수가 없다.
		// 여기서 잘린부분을 다시 한번 보내자.
		if (CashItemPacketIsOk != lItemIndex)
		{
			nPacketLength = 0;
			pvPacket	  = pThis->m_pagpmOptimizedPacket2->MakePacketCharAllCashItem(pcsCharacter, &nPacketLength, &lItemIndex);

			if (pvPacket && nPacketLength > 0)
			{
				pThis->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);
				pThis->SendPacket(pvPacket, nPacketLength, ulNID);
				pThis->m_csPacket.FreePacket(pvPacket);
			}
		}

		return TRUE;
	}
	else
		return pThis->SendPacketItemAll(pcsCharacter, ulNID);
}

BOOL AgsmItem::CallbackSendCharacterNewID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	if (ulNID == 0)
		return FALSE;

	pThis->SendPacketItemADCharacter(pcsCharacter, ulNID);

	return pThis->SendPacketItemAllNewID(pcsCharacter, ulNID);
}

BOOL AgsmItem::CallbackSendCharacterAllServerInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			*pulBuffer		= (UINT32 *)		pCustData;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (pcsAgsdCharacter->m_bIsNewCID)
		return TRUE;

	BOOL	bSendResult = pThis->SendPacketItemServerData(pcsCharacter, pulBuffer[0], (BOOL) pulBuffer[1]);

	bSendResult &= pThis->SendPacketItemADCharacter(pcsCharacter, pulBuffer[0]);

	return bSendResult;
}

BOOL AgsmItem::CallbackUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackUpdateCharacter");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis				= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)	pData;
	UINT32			ulClockCount		= PtrToUint(pCustData);
	UINT32			ulElapsedClockCount	= pThis->m_pagsmCharacter->GetElapsedTimeFromLastIdleProcess(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, ulClockCount);

	if (!pThis->m_pagsmCharacter->IsIdleProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, ulClockCount))
		return TRUE;

	pThis->m_pagsmCharacter->ResetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM);
	pThis->m_pagsmCharacter->SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, ulClockCount);

	AgsdItemADChar	*pcsAttachCharacter	= pThis->GetADCharacter(pcsCharacter);
	if (!pcsAttachCharacter)
		return FALSE;

	// 현재 사용하고 있는 아이템이 있으면 처리해준다.
	// 예) 물약을 빨고 있다. 등등....
	if (pcsAttachCharacter->m_lNumUseItem > 0)
	{
		INT32		lNumUseItem		= pcsAttachCharacter->m_lNumUseItem;

		ASSERT(lNumUseItem <= AGPMITEM_MAX_USE_ITEM);

		for (int i = 0; i < lNumUseItem; ++i)
		{
			AgpdItem	*pcsUseItem	= pThis->m_pagpmItem->GetItem(pcsAttachCharacter->m_lUseItemID[i]);
			if (!pcsUseItem)
				continue;

			AgsdItem	*pcsAgsdItem = pThis->GetADItem(pcsUseItem);
			if (!pcsAgsdItem)
				continue;

			// 아템 사용횟수가 남아있는넘인지 확인한다.
			if (pcsAgsdItem->m_lUseItemRemain < 1)
			{
				pThis->RemoveItemFromUseList(pcsUseItem);
				continue;
			}

			if (pcsAgsdItem->m_ulNextUseItemTime < ulClockCount)
			{
				// 다시 아템을 사용해야 하는 시간이다.
				pThis->UseItem(pcsUseItem, FALSE);
			}
		}
	}

	if(pThis->m_pagpmCharacter->IsPC(pcsCharacter) && pcsCharacter->m_szID[0] != '\0' )
	{
		AuAutoLock pLock(pcsCharacter->m_Mutex);
		if(pLock.Result())
		{
			BOOL bLimitTimeItemInUse = pThis->IsCharacterUsingTimeLimitItem(pcsCharacter);
			if (bLimitTimeItemInUse)
			{
				if(pcsAttachCharacter->m_ulTimeLimitItemCheckClock + AGSDCHAR_IDLE_INTERVAL_TEN_SECONDS*6  < ulClockCount)
				{
					pThis->UpdateAllInUseLimitTimeItemTime(pcsCharacter, ulElapsedClockCount, AuTimeStamp::GetCurrentTimeStamp());
					
					pcsAttachCharacter->m_ulTimeLimitItemCheckClock = ulClockCount;
				}

				pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_TEN_SECONDS);
			}
		}
	}

	// 쿨다운 타임 처리 2008.02.15. steeple
	BOOL bCooldownProcessing = pThis->m_pagpmItem->IsProgressingCooldown(pcsCharacter);
	if(bCooldownProcessing)
	{
		pThis->m_pagpmItem->ProcessAllItemCooldown(pcsCharacter, ulElapsedClockCount);
		pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
	}

	// 캐시 아이템 사용 시간 처리
	BOOL bCashItemInUse = pThis->m_pagpmItem->IsCharacterUsingCashItem(pcsCharacter);
	if (bCashItemInUse)
	{
		// update time of cash item
		pThis->UpdateAllInUseCashItemTime(pcsCharacter, ulElapsedClockCount, AuTimeStamp::GetCurrentTimeStamp());	// currenttimestamp 아직 구현 안 함!!!!! 20051201, kelovon
		pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
	}

	if (pcsAttachCharacter->m_lNumUseItem > 0)
	{
		// AGSDCHAR_IDLE_INTERVAL_100_MS -_-; 2008.04.15. steeple
		// 아래 Interval Time 1ms -> 500ms 로 바꿨다. 2008.02.15. steeple
		pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_200_MS);
	}

	return TRUE;
}

// 2006.12.26. steeple
// Level Up 하면 Equip 상태의 LinkID 달린 Item 들 다시 계산해준다. 
BOOL AgsmItem::CallbackCharLevelUp(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmItem* pThis = static_cast<AgsmItem*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	INT32* plLevelUpNum = static_cast<INT32*>(pCustData);

	INT32 lCurrLevel = pThis->m_pagpmCharacter->GetLevel(pcsCharacter);
	INT32 lPrevLevel = (plLevelUpNum) ? lCurrLevel - *plLevelUpNum : lCurrLevel - 1;

	AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// 강제로 레벨이 세팅된 경우라면 이전 레벨을 바꿔준다.
	if(pcsAgsdCharacter->m_bLevelUpForced && pcsAgsdCharacter->m_lLevelBeforeForced)
		lPrevLevel = pcsAgsdCharacter->m_lLevelBeforeForced;

	// Recalc 는 한번만 해주기 위해서 아래 작업한다.
	BOOL bNeedRecalcFactor = FALSE;
	bNeedRecalcFactor = pThis->UpdateItemOptionSkillData(pcsCharacter, FALSE, lPrevLevel);

	// Limited Max Level Item Check 한다. 2008.01.28. steeple
	bNeedRecalcFactor |= pThis->ProcessLimitedLevelItemOnLevelUp(pcsCharacter);

	if(bNeedRecalcFactor)
	{
		pThis->m_pagsmCharacter->ReCalcCharacterResultFactors(pcsCharacter, TRUE);

		// 2007.02.06. steeple
		// Modified Skill Level 을 다시 계산해준다.
		pThis->m_pagpmSkill->UpdateModifiedSkillLevel((ApBase*)pcsCharacter);
	}

	return TRUE;
}

// 2007.05.04. steeple
// 리전이 바뀔 때 체크한다. 감옥으로 들어갈 때는 Cash 아이템 정지시켜준다.
BOOL AgsmItem::CBUpdateRegionIndex(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem*			pThis					= static_cast<AgsmItem*>(pClass);
	AgpdCharacter*		pcsCharacter			= static_cast<AgpdCharacter*>(pData);
	INT16				nPrevRegionIndex		= *static_cast<INT16*>(pCustData);

	ApmMap::RegionTemplate	*pcsRegionTemplate		= pThis->m_papmMap->GetTemplate(pcsCharacter->m_nBindingRegionIndex);
	ApmMap::RegionTemplate	*pcsPrevRegionTemplate	= pThis->m_papmMap->GetTemplate(nPrevRegionIndex);

	if(pcsRegionTemplate && pcsPrevRegionTemplate)
	{
		// 새로 들어간 곳이 감옥이면 Cash 아이템 정지.
		if(pcsRegionTemplate->ti.stType.bJail && !pcsPrevRegionTemplate->ti.stType.bJail)
			pThis->UnUseAllCashItem(pcsCharacter, FALSE, AGSDITEM_PAUSE_REASON_JAIL);

		// 나올 때는 사용하게 해준다.
		else if(!pcsRegionTemplate->ti.stType.bJail && pcsPrevRegionTemplate->ti.stType.bJail)
			pThis->UsePausedCashItem(pcsCharacter, AGSDITEM_PAUSE_REASON_JAIL);
	}

	return TRUE;
}

//BOOL AgsmItem::CallbackAskReallyConvertItem(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgsmItem			*pThis				= (AgsmItem *)			pClass;
//	AgpdItem			*pcsItem			= (AgpdItem *)			pData;
//	AgpdItem			*pcsSpiritStone		= (AgpdItem *)			pCustData;
//
//	if (!pcsItem->m_pcsCharacter)
//		return FALSE;
//
//	INT16				nPacketLength		= 0;
//	PVOID				pvPacket			= pThis->m_pagpmItem->MakePacketItemConvertAskReallyConvert(pcsItem, pcsSpiritStone, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL				bSendResult			= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
//
//	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}
//
//BOOL AgsmItem::CallbackConvertItem(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgsmItem		*pThis			= (AgsmItem *)	pClass;
//	AgpdItem		*pcsItem		= (AgpdItem *)	pData;
//	INT32			*plResult		= (INT32 *) ((PVOID *)	pCustData)[0];
//	PVOID			pvUpdateFactor	= ((PVOID *)	pCustData)[1];
//	AgpdItem		*pcsSpiritStone = (AgpdItem *)((PVOID*)	pCustData)[2];
//
//	if (!plResult)
//		return FALSE;
//
//	if (!pcsItem->m_pcsCharacter)
//	{
//		pcsItem->m_pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pcsItem->m_ulCID);
//		if (!pcsItem->m_pcsCharacter)
//			return FALSE;
//	}
//
//	BOOL			bResult		= TRUE;
//
//	switch (*plResult) {
//	case AGPMITEM_CONVERT_FAIL:
//	case AGPMITEM_CONVERT_DESTROY_ITEM:
//		// 바뀐게 없다. 암것도 할게 없다.
//		bResult = FALSE;
//		break;
//
//	case AGPMITEM_CONVERT_DESTROY_ATTRIBUTE:
//	case AGPMITEM_CONVERT_SUCCESS:
//		{
//			// 입고 있는넘이 바뀐건지 아닌지 살펴본다.
//			// 입고 있는넘이 바뀐거라면 owner의 result factor를 다시 계산해야 한다.
//			if (pcsItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
//			{
//				pThis->m_pagsmCharacter->ReCalcCharacterFactors(pcsItem->m_pcsCharacter, TRUE);
//			}
//		}
//		break;
//
//		/*
//	default:
//		return FALSE;
//		break;
//		*/
//	}
//
//	INT16	nPacketLength = 0;
//	PVOID	pvPacket	= pThis->m_pagpmItem->MakePacketItemConvertResult(pcsItem, (AgpmItemConvertResult) *plResult, &nPacketLength);
//
//	if (!pvPacket || nPacketLength <= 0)
//		return FALSE;
//
//	BOOL	bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
//
//	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);
//
//	if (pvUpdateFactor)
//	{
//		pThis->m_pagsmCharacter->SendPacketFactor(pvUpdateFactor, pcsItem->m_pcsCharacter);
//	}
//
//	if (pcsSpiritStone)
//	{
//		// Log - 2004.05.04. steeple
//		pThis->WriteConvertLog(pcsItem->m_pcsCharacter->m_lID, pcsItem, pcsSpiritStone, (INT8)*plResult);
//	}
//
//	return bSendResult;
//}
//
//BOOL AgsmItem::CallbackAddConvertHistory(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass)
//		return FALSE;
//
//	AgsmItem		*pThis			= (AgsmItem *)		pClass;
//	AgpdItem		*pcsItem		= (AgpdItem *)		pData;
//
//	if (!pcsItem->m_pcsCharacter)
//		return TRUE;
//
//	if (pcsItem->m_pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD &&
//		pThis->m_pagsmServerManager->GetThisServerType() == AGSMSERVER_TYPE_GAME_SERVER)
//	{
//		pThis->EnumCallback(AGSMITEM_CB_INSERT_CONVERT_HISTORY_TO_DB, pcsItem, NULL);
//	}
//
//	return pThis->SendPacketAddConvertHistory(pcsItem, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
//}
//
//BOOL AgsmItem::CallbackRemoveConvertHistory(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgsmItem		*pThis			= (AgsmItem *)		pClass;
//	AgpdItem		*pcsItem		= (AgpdItem *)		pData;
//	INT32			lIndex			= *(INT32 *)		pCustData;
//
//	if (!pcsItem->m_pcsCharacter)
//		return TRUE;
//
//	if (pcsItem->m_pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD &&
//		pThis->m_pagsmServerManager->GetThisServerType() == AGSMSERVER_TYPE_GAME_SERVER)
//	{
//		pThis->EnumCallback(AGSMITEM_CB_REMOVE_CONVERT_HISTORY_FROM_DB, pcsItem, &lIndex);
//	}
//
//	return pThis->SendPacketRemoveConvertHistory(pcsItem, lIndex, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
//}
//
//BOOL AgsmItem::CallbackUpdateConvertHistory(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass)
//		return FALSE;
//
//	AgsmItem		*pThis		= (AgsmItem *)		pClass;
//	AgpdItem		*pcsItem	= (AgpdItem *)		pData;
//
//	// 서버쪽에선 할일이 없다.
//	//
//	//
//
//	return TRUE;
//}
//
//BOOL AgsmItem::CallbackSendConvertHistory(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass)
//		return FALSE;
//
//	AgsmItem		*pThis		= (AgsmItem *)		pClass;
//	AgpdItem		*pcsItem	= (AgpdItem *)		pData;
//
//	// 개조 히스토리를 클라이언트(pcsItem 주인)로 보내준다.
//	//
//	if (pcsItem->m_ulCID == AP_INVALID_CID)
//		return FALSE;
//
//	return pThis->SendPacketConvertHistory(pcsItem, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
//}

BOOL AgsmItem::CallbackCheckPickupItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackCheckPickupItem");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem					*pThis				= (AgsmItem *)					pClass;
	pstAgpmItemCheckPickupItem	pstCheckPickupItem	= (pstAgpmItemCheckPickupItem)	pData;
	BOOL						*pbCanPickup		= (BOOL *)						pCustData;

	pThis->EnumCallback(AGSMITEM_CB_PICKUP_CHECK_ITEM, pstCheckPickupItem, pbCanPickup);

	if (*pbCanPickup == FALSE)
		return FALSE;

	pstCheckPickupItem->lStackCount	= pThis->m_pagpmItem->GetItemStackCount(pstCheckPickupItem->pcsItem);

	if (!pstCheckPickupItem->pcsItem ||
		pstCheckPickupItem->pcsItem->m_ulCID != AP_INVALID_CID ||
		!pstCheckPickupItem->pcsCharacter)
	{
		*pbCanPickup = FALSE;
		return FALSE;
	}

	AgsdItem	*pcsAgsdItem	= pThis->GetADItem(pstCheckPickupItem->pcsItem);

	//// pc방 전용 아이템인 경우 pc방 유저들만 집어들 수 있다.
	//if (pstCheckPickupItem->pcsItem->m_pcsItemTemplate->m_bIsUseOnlyPCBang &&
	//	!pThis->m_pagpmBillInfo->IsPCBang(pstCheckPickupItem->pcsCharacter))
	//{
	//	*pbCanPickup = FALSE;
	//	return FALSE;
	//}

	// 2007.08.29. steeple
	// 투명인 놈들은 줍지 못한다.
	if (pThis->m_pagpmCharacter->IsStatusTransparent(pstCheckPickupItem->pcsCharacter) ||
		pThis->m_pagpmCharacter->IsStatusHalfTransparent(pstCheckPickupItem->pcsCharacter) ||
		pThis->m_pagpmCharacter->IsStatusFullTransparent(pstCheckPickupItem->pcsCharacter))
	{
		*pbCanPickup = FALSE;
		return FALSE;
	}

	// 루팅 권한이 유지되는 시간
	int ExTime = g_eServiceArea != AP_SERVICE_AREA_WESTERN ? 10000 : 20000;

	// 선 루팅권이 있는 넘인지 살펴본다.
	// 선루팅권을 갖고 있는넘이 캐릭인경우는 걍 아뒤만 비교하고
	// 파티인 경우는 lCID의 파티를 가져와서 비교한다.
	if (pcsAgsdItem->m_csFirstLooterBase.m_lID != AP_INVALID_CID &&
		pcsAgsdItem->m_csFirstLooterBase.m_eType != APBASE_TYPE_NONE && 
		!pThis->m_pagpmItem->IsQuestItem(pstCheckPickupItem->pcsItem) &&		// 퀘스트용 아이템은 루팅 권한이 바뀌지 않는다.
		!pThis->m_pagpmItem->IsFirstLooterOnly(pstCheckPickupItem->pcsItem->m_pcsItemTemplate) &&	// 선루팅권 변경 불가 아이템
		pcsAgsdItem->m_ulDropTime + ExTime <= pThis->GetClockCount())
	{
		pcsAgsdItem->m_csFirstLooterBase.m_lID		= AP_INVALID_CID;
		pcsAgsdItem->m_csFirstLooterBase.m_eType	= APBASE_TYPE_NONE;
	}

	// 2004.02.25. steeple
	// 돈일때는 AddItemToInventory 에서 ApgdItem 을 지워버린다. 그러므로 돈인지 아닌지 체크해 놓아야 한다.
	BOOL bIsMoney = FALSE;
	if(((AgpdItemTemplate*)pstCheckPickupItem->pcsItem->m_pcsItemTemplate)->m_lID == pThis->m_pagpmItem->GetMoneyTID())
		bIsMoney = TRUE;

	switch (pcsAgsdItem->m_csFirstLooterBase.m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			if (pcsAgsdItem->m_csFirstLooterBase.m_lID != pstCheckPickupItem->pcsCharacter->m_lID)
				*pbCanPickup = FALSE;
			else
			{
				// 퀘스트 아이템인데 귀속 아이템에 설정이 되어 있지 않으면 나의 귀속 아이템으로 설정한다.
				if (pThis->m_pagpmItem->IsQuestItem(pstCheckPickupItem->pcsItem))
				{
					if (E_AGPMITEM_BIND_ON_ACQUIRE == pThis->m_pagpmItem->GetBoundType(pstCheckPickupItem->pcsItem))
					{
						pThis->m_pagpmItem->SetBoundOnOwner(pstCheckPickupItem->pcsItem, pstCheckPickupItem->pcsCharacter);
					}
				}

				if(bIsMoney)
				{
					// 2005.12.21. steeple
					// 돈이면!!!! 캐쉬 아이템 보너스를 체크해서 전체 스택을 재 조정한다.
					INT32 lMoney = pstCheckPickupItem->lStackCount;

					INT32 lBonusMoneyByCash = pThis->m_pagpmCharacter->GetGameBonusMoney(pstCheckPickupItem->pcsCharacter);
					lMoney += (INT32)((double)lMoney * (double)(lBonusMoneyByCash) / (double)100.0);

					pstCheckPickupItem->lStackCount = lMoney;
					pstCheckPickupItem->pcsItem->m_nCount = lMoney;
				}

				*pbCanPickup = pThis->m_pagpmItem->AddItemToInventory(pstCheckPickupItem->pcsCharacter, pstCheckPickupItem->pcsItem);
			}

			if( (*pbCanPickup == AGPMITEM_AddItemInventoryResult_TRUE) && !bIsMoney )
			{
				//pThis->SendPacketItem(pstCheckPickupItem->pcsItem, pThis->m_pagsmCharacter->GetCharDPNID(pThis->m_pagpmCharacter->GetCharacter(pstCheckPickupItem->lCID)));
				pcsAgsdItem->m_csFirstLooterBase.m_eType	= APBASE_TYPE_NONE;
				pcsAgsdItem->m_csFirstLooterBase.m_lID		= AP_INVALID_CID;
			}
		}
		break;

	case APBASE_TYPE_PARTY:
		{
			AgpdCharacter	*pcsPickupChar	= pstCheckPickupItem->pcsCharacter;
			if (!pcsPickupChar)
			{
				*pbCanPickup = FALSE;
				return TRUE;
			}

			AgpdParty	*pcsParty = pThis->m_pagpmParty->GetParty(pcsPickupChar);
			if (!pcsParty)
			{
				*pbCanPickup = FALSE;
				return TRUE;
			}

			if (pcsParty->m_lID != pcsAgsdItem->m_csFirstLooterBase.m_lID)
				*pbCanPickup = FALSE;
			else
			{
				// 2005.03.31. steeple
				if(bIsMoney)
				{
////					AgpdCharacter	*pcsNearMember[AGPMPARTY_MAX_PARTY_MEMBER];
////					ZeroMemory(pcsNearMember, sizeof(AgpdCharacter *) * AGPMPARTY_MAX_PARTY_MEMBER);
//
//					ApSafeArray<AgpdCharacter *, AGPMPARTY_MAX_PARTY_MEMBER>	pcsNearMember;
//					pcsNearMember.MemSetAll();
//
//					INT32	lMemberTotalLevel	= 0;
//
////					INT32	lNumNearMember = pThis->m_pagsmParty->GetNearMember(pcsParty, pcsNearMember, &lMemberTotalLevel);
//					INT32	lNumNearMember = pThis->m_pagsmParty->GetNearMember(pcsParty, pcsPickupChar, &pcsNearMember[0], &lMemberTotalLevel);
//					if (lNumNearMember < 1)
//					{
//						*pbCanPickup = FALSE;
//						return FALSE;
//					}
//
//					INT32	lItemStackCount1	= pstCheckPickupItem->lStackCount / lNumNearMember;
//					INT32	lItemStackCount2	= pstCheckPickupItem->lStackCount % lNumNearMember;
//
//					for (int i = 0; i < lNumNearMember; ++i)
//					{
//						INT32	lPickupStackCount	= lItemStackCount1;
//						if (lItemStackCount2 > 0)
//						{
//							++lPickupStackCount;
//							--lItemStackCount2;
//						}
//
//						/*
//						if (pstCheckPickupItem->lCID == pcsParty->m_lMemberListID[i])
//						{
//							pstCheckPickupItem->lStackCount	= lPickupStackCount;
//						}
//						*/
//
//						AgpdCharacter	*pcsMember	= pThis->m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
//						if (pcsMember)
//						{
//							/*
//							pstCheckPickupItem->pcsItem->m_nCount	= lPickupStackCount;
//
//							pThis->m_pagpmItem->AddItemToInventory(pcsMember, pstCheckPickupItem->pcsItem, NULL, NULL, NULL, FALSE);
//							*/
//
//							pThis->m_pagpmCharacter->AddMoney(pcsMember, lPickupStackCount);
//						}
//					}
//
//					*pbCanPickup	= TRUE;
//
//					pThis->m_pagpmItem->RemoveItem(pstCheckPickupItem->pcsItem);
				}
				else
				{
					if (pThis->m_pagpmItem->IsQuestItem(pstCheckPickupItem->pcsItem))
					{
						if (E_AGPMITEM_BIND_ON_ACQUIRE == pThis->m_pagpmItem->GetBoundType(pstCheckPickupItem->pcsItem))
						{
							*pbCanPickup = pThis->EnumCallback(AGSMITEM_CB_USE_ITEM_PICKUP_CHECK_QUEST_VALID, pcsPickupChar, pstCheckPickupItem->pcsItem);
							if (*pbCanPickup)
							{
								pThis->m_pagpmItem->SetBoundOnOwner(pstCheckPickupItem->pcsItem, pcsPickupChar);
								*pbCanPickup = pThis->m_pagpmItem->AddItemToInventory(pcsPickupChar, pstCheckPickupItem->pcsItem);
							}
						}
					}
					else
					{
						*pbCanPickup = pThis->m_pagpmItem->AddItemToInventory(pcsPickupChar, pstCheckPickupItem->pcsItem);
					}

					if( (*pbCanPickup == AGPMITEM_AddItemInventoryResult_TRUE) && !bIsMoney )
					{
						//pThis->SendPacketItem(pstCheckPickupItem->pcsItem, pThis->m_pagsmCharacter->GetCharDPNID(pThis->m_pagpmCharacter->GetCharacter(pstCheckPickupItem->lCID)));

						pcsAgsdItem->m_csFirstLooterBase.m_eType	= APBASE_TYPE_NONE;
						pcsAgsdItem->m_csFirstLooterBase.m_lID		= AP_INVALID_CID;
					}
				}
			}
		}
		break;

	case APBASE_TYPE_NONE:
		{
			*pbCanPickup = pThis->m_pagpmItem->AddItemToInventory(pstCheckPickupItem->pcsCharacter, pstCheckPickupItem->pcsItem);

			/*
			if( (*pbCanPickup == AGPMITEM_AddItemInventoryResult_TRUE) && !bIsMoney )
				pThis->SendPacketItem(pstCheckPickupItem->pcsItem, pThis->m_pagsmCharacter->GetCharDPNID(pThis->m_pagpmCharacter->GetCharacter(pstCheckPickupItem->lCID)));
			*/
		}
		break;

	default:
		*pbCanPickup = FALSE;
		break;
	}

	if(*pbCanPickup && FALSE == bIsMoney)
	{
		// 2004.05.04. steeple
		pThis->WritePickupLog(pstCheckPickupItem->pcsCharacter->m_lID, pstCheckPickupItem->pcsItem, pstCheckPickupItem->lStackCount);
	}


	return TRUE;
}

BOOL AgsmItem::CallbackPickupItemMoney(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackPickupItemMoney");

	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;

	// 아템이 삭제될꺼다. 삭제된다고 주변에 알려준다.
	INT16	nPacketLength = 0;
	PVOID	pvPacket = pThis->MakePacketItemRemove(pcsItem, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsItem->m_posItem, PACKET_PRIORITY_3);

	pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmItem::CallbackPickupItemResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	INT8			cResult			= *(INT8 *)			pData;
	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	AgpdItemTemplate	*pcsItemTemplate	= (AgpdItemTemplate *)	ppvBuffer[0];
	INT32				lItemCount			= PtrToInt(ppvBuffer[1]);
	INT32				lCID				= PtrToInt(ppvBuffer[2]);
	AgpdItem			*pcsItem			= (AgpdItem *)			ppvBuffer[3];

	if (!pcsItemTemplate)
		return FALSE;

	if (cResult == AGPMITEM_PACKET_PICKUP_ITEM_RESULT_SUCCESS && pcsItem && pcsItem->m_pcsCharacter)
	{
		PVOID	pvPacketFactor = pThis->m_pagpmFactors->MakePacketFactorDiffer(&pcsItem->m_csFactor, &((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor);
		PVOID	pvPacketRestrictFactor = pThis->m_pagpmFactors->MakePacketFactorDiffer(&pcsItem->m_csRestrictFactor, &((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csRestrictFactor);

		INT8	nOperation = AGPMITEM_PACKET_OPERATION_UPDATE;

		INT16	nPacketLength	= 0;
		PVOID	pvPacket = pThis->m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
													&nOperation,				//Operation
													NULL,		//Status
													&pcsItem->m_lID,			//ItemID
													NULL,			//ItemTemplateID
													&pcsItem->m_ulCID,		//ItemOwnerID
													NULL,		//ItemCount
													NULL,			//Field
													NULL,		//Inventory
													NULL,				//Bank
													NULL,			//Equip
													pvPacketFactor,			//Factors
													NULL,
													NULL,						//TargetItemID
													NULL,
													pvPacketRestrictFactor,
													NULL,
													NULL,			// Quest
													NULL,
													NULL,						// reuse time for reverse orb
													NULL,
													NULL,
													NULL,			// SkillPlus
													NULL,
													NULL,
													NULL
													);

		pThis->m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItem->m_ulCID);

		pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter), PACKET_PRIORITY_4);

		pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacketFactor);
		pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacketRestrictFactor);
		pThis->m_pagpmItem->m_csPacket.FreePacket(pvPacket);
	}

	/*
	// 돈이 아닌 다른 아이템을 줍는데 성공했다면 주인빼고 나머지 주변 넘들한테 이넘이 없어졌다고 알려준다.
	if (cResult && pcsItem)
	{
		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= pThis->MakePacketItemRemove(pcsItem, &nPacketLength);

		if (pvPacket && nPacketLength > 0)
		{
			pThis->m_pagsmAOIFilter->SendPacketNearExceptSelf(pvPacket, nPacketLength, pcsItem->m_posItem, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

			pThis->m_csPacket.FreePacket(pvPacket);
		}
	}
	*/

	return pThis->SendPacketPickupItemResult(cResult, (pcsItem) ? pcsItem->m_lID : AP_INVALID_IID, pcsItemTemplate->m_lID, lItemCount, pThis->m_pagsmCharacter->GetCharDPNID(pThis->m_pagpmCharacter->GetCharacter(lCID)));
}

BOOL AgsmItem::CallbackRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;

	AgsdItem		*pcsAgsdItem	= pThis->GetADItem(pcsItem);

	//STOPWATCH2(pThis->GetModuleName(), _T("CallbackRemoveItem"));

	//pcsItem->m_ulRemoveTimeMSec		= pThis->GetClockCount();

	// 2005.05.10. steeple
	if(pcsItem && pcsAgsdItem)
	{
		pThis->RemoveDBIDAdmin(pcsAgsdItem->m_ullDBIID);
	}

	// 2007.12.05. steeple
	// 이곳에서 계속 죽어서 그냥 무조건 구하는 걸로 변경.
	AgpdCharacter* pcsOwner = pThis->m_pagpmCharacter->GetCharacter(pcsItem->m_ulCID);
	if(!pcsOwner || pcsOwner->m_bIsReadyRemove)
	{
		//AuLogFile("ItemError.log", "CallbackRemoveItem(...), pcsOwner is null, ItemTID : %d\n", pcsItem->m_lTID);
		return TRUE;
	}

	if (pcsItem->m_pcsCharacter)
	{
		if (pcsAgsdItem->m_lUseItemRemain > 0)
			pThis->RemoveItemFromUseList(pcsItem, TRUE);

		if (pThis->m_pagsmCharacter->GetCharDPNID(pcsOwner))
		{
			// 아바타일 경우 주위에 보낸다. 2008.03.21. steeple
			if (pThis->m_pagpmItem->IsAvatarItem(pcsItem->m_pcsItemTemplate))
				return pThis->SendPacketItemRemoveNear(pcsItem->m_lID, pcsOwner->m_stPos);
			else
				return pThis->SendPacketItemRemove(pcsItem->m_lID, pThis->m_pagsmCharacter->GetCharDPNID(pcsOwner));
		}
	}
	else
	{
		// 패킷만 전송해준다.
		if (pThis->m_pagsmCharacter->GetCharDPNID(pcsOwner))
		{
			// 아바타일 경우 주위에 보낸다. 2008.03.21. steeple
			if (pThis->m_pagpmItem->IsAvatarItem(pcsItem->m_pcsItemTemplate))
				return pThis->SendPacketItemRemoveNear(pcsItem->m_lID, pcsOwner->m_stPos);
			else
				return pThis->SendPacketItemRemove(pcsItem->m_lID, pThis->m_pagsmCharacter->GetCharDPNID(pcsOwner));
		}
	}

	return TRUE;
}

BOOL AgsmItem::AdjustRecalcFactor(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, BOOL bIsAdd, BOOL bCheckDragonScionWeapon, BOOL bForced)
{
	PROFILE("AgsmItem::AdjustRecalcFactor");

	if (!pcsCharacter || !pcsItem)
		return FALSE;

	if (pcsItem->m_pcsItemTemplate && ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
	{
		//AgpdFactorDamage	*pcsItemFactorDamage	= (AgpdFactorDamage *) m_pagpmFactors->GetFactor(&pcsItem->m_csFactor, AGPD_FACTORS_TYPE_DAMAGE);
		AgpdFactorAttack	*pcsItemFactorAttack	= (AgpdFactorAttack *) m_pagpmFactors->GetFactor(&pcsItem->m_csFactor, AGPD_FACTORS_TYPE_ATTACK);

		if (/*!pcsItemFactorDamage && */!pcsItemFactorAttack)
			return TRUE;

		//AgpdFactorDamage	*pcsCharFactorDamage	= (AgpdFactorDamage *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_DAMAGE);
		AgpdFactorAttack	*pcsCharFactorAttack	= (AgpdFactorAttack *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_ATTACK);

		AgpdFactor	csUpdateFactor;
		m_pagpmFactors->InitFactor(&csUpdateFactor);

		/*
		if (pcsItemFactorDamage && pcsCharFactorDamage)
		{
			AgpdFactorDamage	*pcsDamage	= (AgpdFactorDamage *) m_pagpmFactors->SetFactor(&csUpdateFactor, NULL, AGPD_FACTORS_TYPE_DAMAGE);
			if (pcsDamage)
			{
				for (int i = 0; i < AGPD_FACTORS_DAMAGE_MAX_TYPE; ++i)
				{
					for (int j = 0; j < AGPD_FACTORS_ATTRIBUTE_MAX_TYPE; ++j)
					{
						if (pcsItemFactorDamage->csValue[i].lValue[j] > 0)
						{
							if (bIsAdd)
								pcsDamage->csValue[i].lValue[j]	= (-pcsCharFactorDamage->csValue[i].lValue[j]);
							else
								pcsDamage->csValue[i].lValue[j] = pcsCharFactorDamage->csValue[i].lValue[j];
						}
					}
				}
			}
		}
		*/

		if (pcsItemFactorAttack && pcsCharFactorAttack)
		{
			AgpdFactorAttack	*pcsAttack	= (AgpdFactorAttack *) m_pagpmFactors->SetFactor(&csUpdateFactor, NULL, AGPD_FACTORS_TYPE_ATTACK);
			if (pcsAttack)
			{
				INT32	lItemAttackRange		= 0;
				INT32	lItemAttackSpeed		= 0;
				
				INT32	lCharAttackRange		= 0;
				INT32	lCharAttackSpeed		= 0;

				m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lItemAttackRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
				m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lItemAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);

				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCharAttackRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCharAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
				
				if (lItemAttackRange > 0)
				{
					if (bIsAdd)
						m_pagpmFactors->SetValue(&csUpdateFactor, (-lCharAttackRange), AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
					else
						m_pagpmFactors->SetValue(&csUpdateFactor, ( lCharAttackRange), AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
				}

				if (lItemAttackSpeed > 0)
				{
					BOOL bCalcDragonScionFactor = FALSE;

					AgpdItem* pcsItemL		= m_pagpmItem->GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
					AgpdItem* pcsItemR		= m_pagpmItem->GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);

					if(!bCheckDragonScionWeapon)
					{
						if(m_pagpmItem->GetWeaponType(pcsItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON)
						{
							if(pcsItemL && ((m_pagpmItem->CheckUseItem(pcsCharacter, pcsItemL) || bForced == TRUE) && 
								m_pagpmItem->GetWeaponType(pcsItemL->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON))
							{
								bCalcDragonScionFactor = TRUE;
							}
						}
					}

					if(!bCheckDragonScionWeapon)
					{
						if(m_pagpmItem->GetWeaponType(pcsItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON)
						{
							if(pcsItemR && ((m_pagpmItem->CheckUseItem(pcsCharacter, pcsItemR) || bForced == TRUE)) && 
								m_pagpmItem->GetWeaponType(pcsItemR->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON)
							{
								bCalcDragonScionFactor = TRUE;
							}
						}
					}
										
					if(!bCalcDragonScionFactor)
					{
						if (bIsAdd)
						{
							if(m_pagpmItem->GetWeaponType(pcsItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
								m_pagpmFactors->SetValue(&csUpdateFactor, ( lItemAttackSpeed), AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
							else
								m_pagpmFactors->SetValue(&csUpdateFactor, (-lCharAttackSpeed), AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
						}
						else
						{
							if(m_pagpmItem->GetWeaponType(pcsItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
								m_pagpmFactors->SetValue(&csUpdateFactor, (-lItemAttackSpeed), AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
							else
								m_pagpmFactors->SetValue(&csUpdateFactor, ( lCharAttackSpeed), AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
						}
					}
					else
					{
						if ( m_pagpmFactors->GetRace( &pcsItem->m_pcsCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
						{
							if (m_pagpmItem->GetWeaponType(pcsItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON)
							{
								INT32 lLeftDurability = -1;
								m_pagpmFactors->GetValue(&pcsItemL->m_csFactor, &lLeftDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
								if (0 == lLeftDurability)
								{
									if (bIsAdd)
										m_pagpmFactors->SetValue(&csUpdateFactor, lItemAttackSpeed-lCharAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
									else
										m_pagpmFactors->SetValue(&csUpdateFactor, lCharAttackSpeed-lItemAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);

								}
							}
							else if (m_pagpmItem->GetWeaponType(pcsItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON)
							{
								INT32 lRightDurability = -1;
								m_pagpmFactors->GetValue(&pcsItemR->m_csFactor, &lRightDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
								if (0 == lRightDurability)
								{
									if (bIsAdd)
										m_pagpmFactors->SetValue(&csUpdateFactor, lItemAttackSpeed-lCharAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
									else
										m_pagpmFactors->SetValue(&csUpdateFactor, lCharAttackSpeed-lItemAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
								}
							}
						}
					}
				}
			}
		}

		m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, &csUpdateFactor, FALSE, FALSE, TRUE, FALSE);

		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
	}

	return TRUE;
}

/*
BOOL AgsmItem::CallbackSendSectorInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackSendSectorInfo");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem			*pThis				= (AgsmItem *)			pClass;
	ApWorldSector		*pcsSector			= (ApWorldSector *)		pData;
	UINT32				dpnid				= *(UINT32 *)			pCustData;

	if (dpnid == 0)
		return FALSE;

	ApWorldSector::IdPos *	pItem;

	INT32				lIndex				= 0;

	INT32				lItemBuffer[512];
	ZeroMemory(lItemBuffer, sizeof(INT32) * 512);

	pcsSector->m_Mutex.Lock();

	for (pItem = pcsSector->m_pItems; pItem; pItem = pItem->pNext)
	{
		lItemBuffer[lIndex++]	= pItem->id;

		if (lIndex >= 512)
			break;
	}

	pcsSector->m_Mutex.Unlock();

	AgpdItem	*pcsItem	= NULL;

	for (int i = 0; i < lIndex; ++i)
	{
		AgpdItem *pcsItem	= pThis->m_pagpmItem->GetItem(lItemBuffer[i]);
		if (!pcsItem)
			continue;

		pThis->SendPacketItemView(pcsItem, dpnid);
	}

	return TRUE;
}

BOOL AgsmItem::CallbackSendSectorRemoveInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackSendSectorRemoveInfo");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem			*pThis				= (AgsmItem *)			pClass;
	ApWorldSector		*pcsSector			= (ApWorldSector *)		pData;
	UINT32				dpnid				= *(UINT32 *)			pCustData;

	if (dpnid == 0)
		return FALSE;

	ApWorldSector::IdPos *	pItem;

	INT32				lIndex				= 0;

	INT32				lItemBuffer[512];
	ZeroMemory(lItemBuffer, sizeof(INT32) * 512);

	pcsSector->m_Mutex.Lock();

	for (pItem = pcsSector->m_pItems; pItem; pItem = pItem->pNext)
	{
		lItemBuffer[lIndex++]	= pItem->id;

		if (lIndex >= 512)
			break;
	}

	pcsSector->m_Mutex.Unlock();

	AgpdItem	*pcsItem	= NULL;

	for (int i = 0; i < lIndex; ++i)
	{
		AgpdItem *pcsItem	= pThis->m_pagpmItem->GetItem(lItemBuffer[i]);
		if (!pcsItem)
			continue;

		pThis->SendPacketItemRemove(pcsItem->m_lID, dpnid);
	}

	return TRUE;
}
*/

BOOL AgsmItem::CallbackSendCellInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackSendCellInfo");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem			*pThis				= (AgsmItem *)			pClass;
	AgsmAOICell			*pcsCell			= (AgsmAOICell *)		pData;
	UINT32				dpnid				= *(UINT32 *)			pCustData;

	if (dpnid == 0)
		return FALSE;

	AgsmCellUnit		*pcsCellUnit		= NULL;

	INT32				lIndex				= 0;

//	INT32				lItemBuffer[512];
//	ZeroMemory(lItemBuffer, sizeof(INT32) * 512);

	ApSafeArray<INT_PTR, 512>		lItemBuffer;
	lItemBuffer.MemSetAll();

	pcsCell->m_csRWLock.LockReader();

	for (pcsCellUnit = pcsCell->GetItemUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
	{
		lItemBuffer[lIndex++]	= pcsCellUnit->lID;

		if (lIndex >= 512)
			break;
	}

	pcsCell->m_csRWLock.UnlockReader();

	AgpdItem	*pcsItem	= NULL;

	for (int i = 0; i < lIndex; ++i)
	{
		pcsItem	= (AgpdItem *) lItemBuffer[i];
		if (!pcsItem)
			continue;

		pThis->SendPacketItemView(pcsItem, dpnid);
	}

	return TRUE;
}

BOOL AgsmItem::CallbackSendCellRemoveInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackSendCellRemoveInfo");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem			*pThis				= (AgsmItem *)			pClass;
	AgsmAOICell			*pcsCell			= (AgsmAOICell *)		pData;
	UINT32				dpnid				= *(UINT32 *)			pCustData;

	if (dpnid == 0)
		return FALSE;

	AgsmCellUnit		*pcsCellUnit		= NULL;

	INT32				lIndex				= 0;

//	INT32				lItemBuffer[512];
//	ZeroMemory(lItemBuffer, sizeof(INT32) * 512);

	ApSafeArray<INT_PTR, 512>		lItemBuffer;
	lItemBuffer.MemSetAll();

	pcsCell->m_csRWLock.LockReader();

	for (pcsCellUnit = pcsCell->GetItemUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
	{
		lItemBuffer[lIndex++]	= pcsCellUnit->lID;

		if (lIndex >= 512)
			break;
	}

	pcsCell->m_csRWLock.UnlockReader();

	AgpdItem	*pcsItem	= NULL;

	for (int i = 0; i < lIndex; ++i)
	{
		pcsItem		= (AgpdItem *) lItemBuffer[i];
		if (!pcsItem)
			continue;

		pThis->SendPacketItemRemove(pcsItem->m_lID, dpnid);
	}

	return TRUE;
}

BOOL AgsmItem::CallbackUpdateStackCount(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem			*pThis				= (AgsmItem *)			pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)			pData;
	PVOID				*ppvBuffer			= (PVOID *)				pCustData;

	AgsdItem			*pcsAgsdItem		= pThis->GetADItem(pcsItem);

	INT32	lPrevCount	= *(INT32 *) ppvBuffer[0];
	BOOL	bIsSaveToDB	= TRUE;

	if (ppvBuffer[1])
		bIsSaveToDB	= PtrToInt(ppvBuffer[1]);

	if (bIsSaveToDB)
	{
		// 아이템 타입에 따라 다른 방식으로 저장한다.
		if (pThis->GetDBIID(pcsItem) != 0)
		{
			// Event Item 인 경우
			if(pThis->m_pagpmDropItem2->IsEventItem(pcsItem->m_pcsItemTemplate->m_lID))
			{
				pThis->SendRelayUpdate(pcsItem);
			}
			// 물약인 경우
			else if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
				((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_POTION)
			{
				if (abs(pcsItem->m_nCount - pcsAgsdItem->m_lPrevSaveStackCount) > 8)
					pThis->SendRelayUpdate(pcsItem);
			}
			// 화살인 경우
			else if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
				(((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_ARROW ||
				((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_BOLT) )
			{
				if (abs(pcsItem->m_nCount - pcsAgsdItem->m_lPrevSaveStackCount) > 40)
					pThis->SendRelayUpdate(pcsItem);
			}
			// 렌즈 스톤인 경우 40개에 한번씩 DB에 반영한다.
			else if (pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
				pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_ONE_ATTACK &&
				pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE)
			{
				if (pcsItem->m_nCount <= 0 && pcsItem->m_pcsCharacter)
				{
					// AgpmItem::SubCashItemStackCountOnAttack 에 있다가 일루 옮겨옴. 2006.02.08. steeple
					// AgpmItem::UnuseItem 은 ActionBlockCondition 을 체크한다. 그런데 여기선 체크해선 안됨.
					// 그래서 AgsmItem::UnuseItem 을 불러야 한다.
					pThis->UnuseItem(pcsItem);
					pThis->WriteItemLog(AGPDLOGTYPE_ITEM_USE, pcsItem->m_pcsCharacter->m_lID, pcsItem, pcsItem->m_nCount);
				}
				else if (pcsItem->m_nCount > 0)
				{
					if (abs(pcsItem->m_nCount - pcsAgsdItem->m_lPrevSaveStackCount) > 40)
					{
						pThis->SendRelayUpdate(pcsItem);

						if (pcsItem->m_pcsCharacter)
							pThis->WriteItemLog(AGPDLOGTYPE_ITEM_USE, pcsItem->m_pcsCharacter->m_lID, pcsItem, pcsItem->m_nCount);
					}
				}
			}
			// 그 외 아템들
			else
			{
				pThis->SendRelayUpdate(pcsItem);
			}
		}
	}

	if( pcsItem->m_pcsCharacter )
	{
		AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsItem->m_pcsCharacter);
		if( pcsAgsdCharacter && pcsAgsdCharacter->GetGachaBlock() )
		{
			// do nothing..
			// 패킷 따로 보냄
			pcsAgsdCharacter->AddGachaItemUpdate( AgsdCharacter::GachaDelayItemUpdateData::STACKABLE , pcsItem );
			return TRUE;
		}
		else
			return pThis->SendPacketItemStackCount(pcsItem, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
	}
	else
		return TRUE;
}

/*
BOOL AgsmItem::CallbackSendAllDBData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem			*pThis				= (AgsmItem *)			pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	DPNID				dpnid				= *((DPNID *)			pCustData);

	if (dpnid == 0)
		return FALSE;

	AgpdItemADChar		*pcsItemADChar		= pThis->m_pagpmItem->GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return FALSE;

	if (pcsItemADChar->m_csInventoryGrid.m_ppcGridData)
	{
		for (int i = 0; i < pcsItemADChar->m_csInventoryGrid.m_nLayer * pcsItemADChar->m_csInventoryGrid.m_nRow * pcsItemADChar->m_csInventoryGrid.m_nColumn; ++i)
		{
			if (pcsItemADChar->m_csInventoryGrid.m_ppcGridData[i])
			{
				AgpdItem	*pcsItem			= pThis->m_pagpmItem->GetItem(pcsItemADChar->m_csInventoryGrid.m_ppcGridData[i]->m_lItemID);
				if (pcsItem)
				{
					pThis->SendPacketBasicDBData(pcsItem, dpnid, FALSE);
					pThis->SendPacketConvertHistory(pcsItem, dpnid);
				}
			}
		}
	}

	if (pcsItemADChar->m_csEquipGrid.m_ppcGridData)
	{
		for (int i = 0; i < pcsItemADChar->m_csEquipGrid.m_nLayer * pcsItemADChar->m_csEquipGrid.m_nRow * pcsItemADChar->m_csEquipGrid.m_nColumn; ++i)
		{
			if (pcsItemADChar->m_csEquipGrid.m_ppcGridData[i])
			{
				AgpdItem	*pcsItem			= pThis->m_pagpmItem->GetItem(pcsItemADChar->m_csEquipGrid.m_ppcGridData[i]->m_lItemID);
				if (pcsItem)
				{
					pThis->SendPacketBasicDBData(pcsItem, dpnid, FALSE);
					pThis->SendPacketConvertHistory(pcsItem, dpnid);
				}
			}
		}
	}

	if (pcsItemADChar->m_csBankGrid.m_ppcGridData)
	{
		for (int i = 0; i < pcsItemADChar->m_csBankGrid.m_nLayer * pcsItemADChar->m_csBankGrid.m_nRow * pcsItemADChar->m_csBankGrid.m_nColumn; ++i)
		{
			if (pcsItemADChar->m_csBankGrid.m_ppcGridData[i])
			{
				AgpdItem	*pcsItem			= pThis->m_pagpmItem->GetItem(pcsItemADChar->m_csBankGrid.m_ppcGridData[i]->m_lItemID);
				if (pcsItem)
				{
					pThis->SendPacketBasicDBData(pcsItem, dpnid, FALSE);
					pThis->SendPacketConvertHistory(pcsItem, dpnid);
				}
			}
		}
	}

	return TRUE;
}
*/

/*
BOOL AgsmItem::CBBackupItemBasicData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;

	return pThis->BackupBasicItemData(pcsItem);
}

BOOL AgsmItem::CBBackupItemAddConvertHistoryData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;

	AgsdServer		*pcsRelayServer	= pThis->m_pagsmServerManager->GetRelayServer();
	if (!pcsRelayServer ||
		pcsRelayServer->m_dpnidServer == 0)
		return FALSE;

	return pThis->SendPacketAddConvertHistory(pcsItem, pcsRelayServer->m_dpnidServer);
}

BOOL AgsmItem::CBBackupItemRemoveConvertHistoryData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;
	INT32			lIndex			= *((INT32 *)		pCustData);

	AgsdServer		*pcsRelayServer	= pThis->m_pagsmServerManager->GetRelayServer();
	if (!pcsRelayServer || pcsRelayServer->m_dpnidServer == 0)
		return FALSE;

	return pThis->SendPacketRemoveConvertHistory(pcsItem, lIndex, pcsRelayServer->m_dpnidServer);
}
*/

BOOL AgsmItem::CallbackUpdateAllToDB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmItem::CallbackUpdateAllToDB");

	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CallbackUpdateAllToDB"));

	AgpdItemADChar	*pcsItemADChar	= pThis->m_pagpmItem->GetADCharacter(pcsCharacter);

	// 현재 이 넘이 소유하고 있는 모든 아템을 DB에 업데이트 시킨다.
	
	// 인벤토리에 있는 모든 아템..
	if (pcsItemADChar->m_csInventoryGrid.m_ppcGridData)
	{
		for (int i = 0; i < pcsItemADChar->m_csInventoryGrid.m_nLayer * pcsItemADChar->m_csInventoryGrid.m_nRow * pcsItemADChar->m_csInventoryGrid.m_nColumn; ++i)
		{
			if (pcsItemADChar->m_csInventoryGrid.m_ppcGridData[i])
			{
				AgpdItem	*pcsItem			= pThis->m_pagpmItem->GetItem(pcsItemADChar->m_csInventoryGrid.m_ppcGridData[i]);
				if (pcsItem)
				{
					pThis->SendRelayUpdate(pcsItem);
				}
			}
		}
	}
	// 펫인벤토리에 있는 모든 아이템 m_csSubInventoryGrid
	if (pcsItemADChar->m_csSubInventoryGrid.m_ppcGridData)
	{
		for (int i = 0; i < pcsItemADChar->m_csSubInventoryGrid.m_nLayer * pcsItemADChar->m_csSubInventoryGrid.m_nRow * pcsItemADChar->m_csSubInventoryGrid.m_nColumn; ++i)
		{
			if (pcsItemADChar->m_csSubInventoryGrid.m_ppcGridData[i])
			{
				AgpdItem	*pcsItem			= pThis->m_pagpmItem->GetItem(pcsItemADChar->m_csSubInventoryGrid.m_ppcGridData[i]);
				if (pcsItem)
				{
					pThis->SendRelayUpdate(pcsItem);
				}
			}
		}
	}

	// 캐시 인벤에 있는 모든 아이템
	if (pcsItemADChar->m_csCashInventoryGrid.m_ppcGridData)
	{
		for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nLayer * pcsItemADChar->m_csCashInventoryGrid.m_nRow * pcsItemADChar->m_csCashInventoryGrid.m_nColumn; ++i)
		{
			if (pcsItemADChar->m_csCashInventoryGrid.m_ppcGridData[i])
			{
				AgpdItem	*pcsItem			= pThis->m_pagpmItem->GetItem(pcsItemADChar->m_csCashInventoryGrid.m_ppcGridData[i]);
				if (pcsItem)
				{
					pThis->SendRelayUpdate(pcsItem);
				}
			}
		}
	}

	// 장착슬롯에 있는 모든 아템..
	if (pcsItemADChar->m_csEquipGrid.m_ppcGridData)
	{
		for (int i = 0; i < pcsItemADChar->m_csEquipGrid.m_nLayer * pcsItemADChar->m_csEquipGrid.m_nRow * pcsItemADChar->m_csEquipGrid.m_nColumn; ++i)
		{
			if (pcsItemADChar->m_csEquipGrid.m_ppcGridData[i])
			{
				AgpdItem	*pcsItem			= pThis->m_pagpmItem->GetItem(pcsItemADChar->m_csEquipGrid.m_ppcGridData[i]);
				if (pcsItem)
				{
					pThis->SendRelayUpdate(pcsItem);
				}
			}
		}
	}

	// 뱅크에 있는 모든 아템..
	if (pcsItemADChar->m_csBankGrid.m_ppcGridData)
	{
		for (int i = 0; i < pcsItemADChar->m_csBankGrid.m_nLayer * pcsItemADChar->m_csBankGrid.m_nRow * pcsItemADChar->m_csBankGrid.m_nColumn; ++i)
		{
			if (pcsItemADChar->m_csBankGrid.m_ppcGridData[i])
			{
				AgpdItem	*pcsItem			= pThis->m_pagpmItem->GetItem(pcsItemADChar->m_csBankGrid.m_ppcGridData[i]);
				if (pcsItem)
				{
					pThis->SendRelayUpdate(pcsItem);
				}
			}
		}
	}

	return TRUE;
}

BOOL AgsmItem::CallbackUseReturnScroll(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	if (pThis->m_pagpmCharacter->IsAllBlockStatus(pcsCharacter))
		return TRUE;

	AgpdItemADChar	*pcsItemADChar	= pThis->m_pagpmItem->GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return FALSE;

	AgsdCharacter *pagsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (!pagsdCharacter)
		return FALSE;

	if (!pcsItemADChar->m_bUseReturnTeleportScroll)
	{
		pThis->SendPacketUseReturnScrollResult(pcsCharacter, FALSE);
		pThis->SendPacketUpdateReturnScrollStatus(pcsCharacter, FALSE);

		return TRUE;
	}

	// 전투모드인지 등등을 검사한다.
	// 감옥안인지 체크 추가. 2007.05.10. steeple
	if (pThis->m_pagpmCharacter->IsCombatMode(pcsCharacter) ||
		pThis->m_pagpmCharacter->IsInJail(pcsCharacter)
		|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE)
		|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_ATTACK)
		|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
		|| (pThis->m_pagpmCharacter->IsActionBlockCondition(pcsCharacter))
		)
	{
		pThis->m_pagsmSystemMessage->SendSystemMessage( pcsCharacter , AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS );
		pThis->SendPacketUseReturnScrollResult(pcsCharacter, FALSE);
		return TRUE;
	}

	if( pThis->m_papmMap->GetTargetPositionLevelLimit( pcsItemADChar->m_stReturnPosition ) 
		|| pThis->m_papmMap->GetTargetPositionLevelLimit( pcsCharacter->m_stPos ) )
	{
		pThis->m_pagsmSystemMessage->SendSystemMessage( pcsCharacter , AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS );
		pThis->SendPacketUseReturnScrollResult(pcsCharacter, FALSE);
		return TRUE;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	pThis->m_pagpmCharacter->SetActionBlockTime(pcsCharacter, 3000);
	pagsdCharacter->m_bIsTeleportBlock = TRUE;

	// pcsItemADChar->m_stReturnPosition로 텔레포트 시킨다.
	pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

	if (pcsCharacter->m_bMove)
		pThis->m_pagpmCharacter->StopCharacter(pcsCharacter, NULL);

	pThis->EnumCallback(AGSMITEM_CB_CHECK_RETURN_POSITION, pcsCharacter, &pcsItemADChar->m_stReturnPosition);

	//if (!pThis->m_pagpmCharacter->UpdatePosition(pcsCharacter, &pcsItemADChar->m_stReturnPosition, FALSE, TRUE))
	if (!pThis->EnumCallback(AGSMITEM_CB_USE_TELEPORT_SCROLL, pcsCharacter, &pcsItemADChar->m_stReturnPosition))
	{
		pThis->SendPacketUseReturnScrollResult(pcsCharacter, FALSE);
		pThis->SendPacketUpdateReturnScrollStatus(pcsCharacter, FALSE);

		return TRUE;
	}

	pThis->SendPacketUseReturnScrollResult(pcsCharacter, TRUE);
	pThis->SendPacketUpdateReturnScrollStatus(pcsCharacter, FALSE);

	pcsItemADChar->m_bUseReturnTeleportScroll	= FALSE;
	
	pagsdCharacter->m_bIsTeleportBlock = FALSE;

	return TRUE;
}

BOOL AgsmItem::CallbackRestoreTransform(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgsdItemADChar	*pcsAttachData	= pThis->GetADCharacter(pcsCharacter);

	pcsAttachData->m_lAddMaxHP		= 0;
	pcsAttachData->m_lAddMaxMP		= 0;

	// 2006.12.14. steeple
	// 캔슬하면, 재사용 시간도 초기화 해준다.
	pThis->m_pagpmItem->InitTransformReuseTime(pcsCharacter);

	return TRUE;
}

BOOL AgsmItem::CallbackAdjustTransformFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem		*pThis				= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)	pData;
	AgpdFactor		*pcsUpdateFactor	= (AgpdFactor *)	pCustData;

	AgsdItemADChar	*pcsAttachData		= pThis->GetADCharacter(pcsCharacter);

	if (pcsAttachData->m_lAddMaxHP != 0)
	{
		INT32	lCurrentMaxHP	= 0;

		pThis->m_pagpmFactors->GetValue(pcsUpdateFactor, &lCurrentMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

		pThis->m_pagpmFactors->SetValue(pcsUpdateFactor, lCurrentMaxHP + pcsAttachData->m_lAddMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	}

	if (pcsAttachData->m_lAddMaxMP != 0)
	{
		INT32	lCurrentMaxMP	= 0;

		pThis->m_pagpmFactors->GetValue(pcsUpdateFactor, &lCurrentMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

		pThis->m_pagpmFactors->SetValue(pcsUpdateFactor, lCurrentMaxMP + pcsAttachData->m_lAddMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	}

	return TRUE;
}

//BOOL AgsmItem::CallbackApplyBonusFactor(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass || !pCustData)
//		return FALSE;
//
//	AgsmItem		*pThis				= (AgsmItem *)		pClass;
//	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)	pData;
//	AgpdFactor		*pcsUpdateFactor	= (AgpdFactor *)	pCustData;
//
//	if (pThis->m_pagpmItemConvert->GetBonusMaxHP(pcsCharacter) > 0)
//	{
//		INT32	lCurrentMaxHP	= 0;
//
//		pThis->m_pagpmFactors->GetValue(pcsUpdateFactor, &lCurrentMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
//
//		pThis->m_pagpmFactors->SetValue(pcsUpdateFactor, lCurrentMaxHP + pThis->m_pagpmItemConvert->GetBonusMaxHP(pcsCharacter), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
//	}
//
//	if (pThis->m_pagpmItemConvert->GetBonusMaxMP(pcsCharacter) > 0)
//	{
//		INT32	lCurrentMaxMP	= 0;
//
//		pThis->m_pagpmFactors->GetValue(pcsUpdateFactor, &lCurrentMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
//
//		pThis->m_pagpmFactors->SetValue(pcsUpdateFactor, (INT32) (lCurrentMaxMP * (1 + pThis->m_pagpmItemConvert->GetBonusMaxMP(pcsCharacter) / 100.0)), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
//	}
//
//	return TRUE;
//}

BOOL AgsmItem::CallbackUpdateReverseOrbReuseTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	return pThis->SendPacketUpdateReverseOrbReuseTime(pcsCharacter, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
}

BOOL AgsmItem::CallbackUpdateTransformReuseTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	return pThis->SendPacketUpdateTransformReuseTime(pcsCharacter, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
}

BOOL AgsmItem::CallbackInitTransformReuseTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem		*pThis			= (AgsmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	return pThis->SendPacketInitTransformReuseTime(pcsCharacter, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
}

BOOL AgsmItem::CallbackUpdateItemStatusFlag(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis		= (AgsmItem *)	pClass;
	AgpdItem	*pcsItem	= (AgpdItem *)	pData;

	// send packet to item owner
	return pThis->SendPacketUpdateItemStatusFlag(pcsItem, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
}

BOOL AgsmItem::CBConvertAsDrop(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis		= (AgsmItem *)	pClass;
	AgpdItem	*pcsItem	= (AgpdItem *)	pData;

	// 2006.12.28. steeple
	pThis->SendRelayUpdate(pcsItem);
	return pThis->SendPacketItem(pcsItem, pThis->m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
}

// 2008.02.15. steeple
BOOL AgsmItem::CBUpdateCooldown(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem* pThis = static_cast<AgsmItem*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AgpdItemCooldownBase* pstCooldownBase = static_cast<AgpdItemCooldownBase*>(pCustData);

	return pThis->SendPacketUpdateCooldown(pcsCharacter, *pstCooldownBase);
}

BOOL AgsmItem::CBUseCashItem(PVOID pData, PVOID pClass, PVOID pCustData)	// ITEM_CB_ID_CHAR_USE_CASH_ITEM
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis		= (AgsmItem*)pClass;
	AgpdItem	*pcsItem	= (AgpdItem*)pData;

	if (!pcsItem)
	{
		return FALSE;
	}

	BOOL bRet = pThis->m_pagpmItem->IsCharacterUsingCashItem(pcsItem->m_pcsCharacter);
	if (bRet)
	{
		pThis->m_pagsmCharacter->SetProcessTime(pcsItem->m_pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, pThis->GetClockCount());
		pThis->m_pagsmCharacter->SetIdleInterval(pcsItem->m_pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
	}

	if (pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_REAL_TIME ||
		pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_STAMINA)
	{
		// 2008.03.18. steeple
		// 드디어 타입을 추가하였다.
		//
		// 2007.10.31. steeple
		// 사용하지 않는 데이터에서 ExpireTime 이 -1 이 되는 경우가 생겼다.
		// 아마 Avatar 에서 그런 것 같고, != 0 을 > 0 으로 바꿔서 이미 설정되어 있는 경우는 갱신 하지 않도록 했다.
		//
		// 2007.08.31. steeple
		// 무슨 문제인지 모르겠지만, ExpireTime 이 0 이 아닌 경우가 생기면서 새로 시간이 할당안되는 일이 발생했다.
		// 아바타 아이템 일 때만 체크하도록 변경한다.
		// 아바타 아이템일 때 m_lExpireTime 가 0 이 아니면 아래 값 변경하지 않는다. 2007.08.10. steeple
		//
		// m_lExpireTime 가 0 일 때만 설정해준다. 2007.08.10. steeple
		//if(pcsItem->m_lExpireTime == 0)
		//if((pThis->m_pagpmItem->IsAvatarItem(pcsItem->m_pcsItemTemplate) && pcsItem->m_lExpireTime > 0) == FALSE)
		if((pcsItem->m_lExpireTime > 0) == FALSE)
		{
			UINT32 lCurrentTimeStamp = AuTimeStamp::GetCurrentTimeStamp();
			UINT32 lExpireTimeStamp = AuTimeStamp::AddTime(lCurrentTimeStamp, 0, 0, 0, pcsItem->m_pcsItemTemplate->m_lExpireTime);
			pcsItem->m_lExpireTime = lExpireTimeStamp;
		}

		// sendpacket
		pThis->SendPacketUpdateItemUseTime(pcsItem);

		// update db
		pThis->SendRelayUpdate(pcsItem);
	}

	return TRUE;
}

BOOL AgsmItem::CBUnUseCashItem(PVOID pData, PVOID pClass, PVOID pCustData)	// ITEM_CB_ID_CHAR_UNUSE_CASH_ITEM
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItem	*pThis		= (AgsmItem*)pClass;
	AgpdItem	*pcsItem	= (AgpdItem*)pData;

	BOOL bRet = pThis->m_pagpmItem->IsCharacterUsingCashItem(pcsItem->m_pcsCharacter);
	if (!bRet)
	{
		pThis->m_pagsmCharacter->ResetIdleInterval(pcsItem->m_pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM);
	}

	// update db
	pThis->SendRelayUpdate(pcsItem);

	return TRUE;
}

BOOL AgsmItem::CBAddItemOption(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItem* pThis = static_cast<AgsmItem*>(pClass);
	AgpdItem* pcsItem = static_cast<AgpdItem*>(pData);
	AgpdItemOptionTemplate* pcsItemOptionTemplate = static_cast<AgpdItemOptionTemplate*>(pCustData);

	AgsdItem* pcsAgsdItem = pThis->GetADItem(pcsItem);
	if(!pcsAgsdItem)
		return FALSE;

	pcsAgsdItem->m_stOptionSkillData += pcsItemOptionTemplate->m_stSkillData;

	pThis->m_pagpmFactors->CalcFactor(&pcsItem->m_csFactor, &pcsItemOptionTemplate->m_csSkillFactor, FALSE, FALSE, TRUE);
	pThis->m_pagpmFactors->CalcFactor(&pcsItem->m_csFactorPercent, &pcsItemOptionTemplate->m_csSkillFactorPercent, FALSE, FALSE, TRUE);

	return TRUE;
}