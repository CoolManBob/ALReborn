#include "AgpmItemConvert.h"

INT32 AgpmItemConvert::GetNumRune(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return 0;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (pcsAttachData->m_lNumConvert > AGPDITEMCONVERT_MAX_WEAPON_SOCKET ||
		pcsAttachData->m_lNumConvert < 0)
		return 0;

	INT32	lNumRune	= 0;

	for (int i = 0; i < pcsAttachData->m_lNumConvert; ++i)
	{
		if (!pcsAttachData->m_stSocketAttr[i].bIsSpiritStone)
			++lNumRune;
	}

	return lNumRune;
}


AgpdItemConvertRuneResult AgpmItemConvert::IsRuneConvertable(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate ||
		pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY &&
		pcsItem->m_eStatus != AGPDITEM_STATUS_CASH_INVENTORY)
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP)
		return AGPDITEMCONVERT_RUNE_RESULT_INVALID_ITEM;

	if (AGPMITEM_EQUIP_KIND_WEAPON == ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind)
	{
		if (AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE == ((AgpdItemTemplateEquipWeapon *) pcsItem->m_pcsItemTemplate)->m_nWeaponType)
			return AGPDITEMCONVERT_RUNE_RESULT_FAILED;
	}

	if (IsEgoItem(pcsItem))
		return AGPDITEMCONVERT_RUNE_RESULT_IS_EGO_ITEM;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (pcsAttachData->m_lNumSocket > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	if (pcsAttachData->m_lNumSocket <= pcsAttachData->m_lNumConvert)
		return AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_FULL;

	return AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;
}

AgpdItemConvertRuneResult AgpmItemConvert::IsValidRuneItem(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_USABLE ||
		((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_RUNE)
		return AGPDITEMCONVERT_RUNE_RESULT_INVALID_RUNE_ITEM;

	if (pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY &&
		pcsItem->m_eStatus != AGPDITEM_STATUS_CASH_INVENTORY)
		return AGPDITEMCONVERT_RUNE_RESULT_INVALID_RUNE_ITEM;

	return AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;
}

AgpdItemConvertRuneResult AgpmItemConvert::IsProperPart(AgpdItem *pcsItem, AgpdItem *pcsRuneItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate || !pcsRuneItem || !pcsRuneItem->m_pcsItemTemplate)
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= GetADItemTemplate((AgpdItemTemplate *) pcsRuneItem->m_pcsItemTemplate);

	switch (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind) {
	case AGPMITEM_EQUIP_KIND_WEAPON:
		{
			if (!pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_WEAPON_INDEX])
				return AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART;
		}
		break;

	case AGPMITEM_EQUIP_KIND_ARMOUR:
		{
			if (!pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_ARMOUR_INDEX] ||
				!pcsAttachTemplateData->m_bRuneConvertableEquipPart[((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nPart])
				return AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART;
		}
		break;

	case AGPMITEM_EQUIP_KIND_SHIELD:
		{
			if (!pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_SHIELD_INDEX])
				return AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART;
		}
		break;

	case AGPMITEM_EQUIP_KIND_RING:
		{
			if (!pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_RING_INDEX])
				return AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART;
		}
		break;

	case AGPMITEM_EQUIP_KIND_NECKLACE:
		{
			if (!pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_NECKLACE_INDEX])
				return AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART;
		}
		break;

	default:
		return AGPDITEMCONVERT_RUNE_RESULT_IS_IMPROPER_PART;
		break;
	}

	// 만약 개조된 아이템에 운명의 기원석(개조 기원석)을 적용하려면 실패.
	if (((AgpdItemTemplateUsableRune *) pcsRuneItem->m_pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_CONVERT)
	{
		if (GetNumConvertedSocket(pcsItem) ||
			GetNumPhysicalConvert(pcsItem) ||
			pcsItem->m_aunOptionTID[0])
			return AGPDITEMCONVERT_RUNE_RESULT_INVALID_ITEM;
	}

	return AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;
}

AgpdItemConvertRuneResult AgpmItemConvert::IsProperLevel(AgpdItem *pcsItem, AgpdItem *pcsRuneItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate || !pcsItem->m_pcsCharacter ||
		!pcsRuneItem || !pcsRuneItem->m_pcsItemTemplate)
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	//INT32	lCharacterLevel	= m_pcsAgpmCharacter->GetLevel(pcsItem->m_pcsCharacter);
	INT32	lItemEquipLevel	= m_pcsAgpmFactors->GetLevel(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csRestrictFactor);

	AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= GetADItemTemplate((AgpdItemTemplate *) pcsRuneItem->m_pcsItemTemplate);

	/*
	if (pcsAttachTemplateData->m_lRuneRestrictLevel > lCharacterLevel)
		return AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_CHAR_LEVEL;
	*/

	if (pcsAttachTemplateData->m_lRuneRestrictLevel > lItemEquipLevel)
		return AGPDITEMCONVERT_RUNE_RESULT_IS_LOW_ITEM_LEVEL;

	return AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;
}

AgpdItemConvertRuneResult AgpmItemConvert::IsAntiNumber(AgpdItem *pcsItem, AgpdItem *pcsRuneItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate || !pcsRuneItem)
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= GetADItemTemplate((AgpdItemTemplate *) pcsRuneItem->m_pcsItemTemplate);

	if (pcsAttachTemplateData->m_lAntiTypeNumber > 0)
	{
		AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

		for (int i = 0; i < pcsAttachData->m_lNumConvert; ++i)
		{
			if (pcsAttachData->m_stSocketAttr[i].bIsSpiritStone)
				continue;

			AgpdItemConvertADItemTemplate	*pcsAttachConvertTemplateData	= GetADItemTemplate(pcsAttachData->m_stSocketAttr[i].pcsItemTemplate);
			if (pcsAttachConvertTemplateData->m_lAntiTypeNumber == pcsAttachTemplateData->m_lAntiTypeNumber)
				return AGPDITEMCONVERT_RUNE_RESULT_IS_ALREADY_ANTI_CONVERT;
		}
	}
	
	return AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;
}

AgpdItemConvertRuneResult AgpmItemConvert::RuneConvert(AgpdItem *pcsItem, AgpdItem *pcsRuneItem, BOOL bAdmin)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate || !pcsRuneItem)
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	AgpdItemConvertRuneResult	eCheckResult	= IsValidRuneItem(pcsRuneItem);
	if (eCheckResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return eCheckResult;

	eCheckResult	= IsRuneConvertable(pcsItem);
	if (eCheckResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return eCheckResult;

	eCheckResult	= IsProperPart(pcsItem, pcsRuneItem);
	if (eCheckResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return eCheckResult;

	eCheckResult	= IsProperLevel(pcsItem, pcsRuneItem);
	if (eCheckResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return eCheckResult;

	eCheckResult	= IsAntiNumber(pcsItem, pcsRuneItem);
	if (eCheckResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return eCheckResult;

	// 2005.05.26. steeple
	// 어드민일 경우 정령석과 마찬가지로 기본검사만 하고 확률검사는 하지 않는다.
	if(bAdmin)
		return AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;

	// Rune 이 캐쉬아이템인 경우 무조건 성공이다. 2008.07.18. steeple
	if(IS_CASH_ITEM(pcsRuneItem->m_pcsItemTemplate->m_eCashItemType))
		return AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;

	INT32	lRandomNumber	= m_csRandom.randInt(100);

	AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= GetADItemTemplate((AgpdItemTemplate *) pcsRuneItem->m_pcsItemTemplate);

	BOOL	bIsFailed	= FALSE;

	FLOAT	fRuneSuccessProbability	= 1.0f;

	if (((AgpdItemTemplateUsableRune *) pcsRuneItem->m_pcsItemTemplate)->m_eRuneAttributeType != AGPMITEM_RUNE_ATTR_CONVERT)
	{
		PVOID pvBuffer[2];
		pvBuffer[0] = (PVOID) pcsItem;
		pvBuffer[1] = (PVOID) pcsRuneItem;
		EnumCallback(AGPDITEMCONVERT_CB_ADJUST_SUCCESS_PROB, pvBuffer, &fRuneSuccessProbability);
	}

	if ((INT32) (pcsAttachTemplateData->m_lRuneSuccessProbability * fRuneSuccessProbability) < lRandomNumber)
	{
		bIsFailed	= TRUE;
	}
	else
	{
		INT32	lNumConvertedRuneAttr	= GetNumRune(pcsItem);

		if (lNumConvertedRuneAttr >= 0 &&
			lNumConvertedRuneAttr < AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		{
			lRandomNumber	= m_csRandom.randInt(100);

			if (IsWeapon(pcsItem))
			{
				if ((INT32) (m_astTableRune[lNumConvertedRuneAttr + 1].lWeaponProbability * fRuneSuccessProbability) < lRandomNumber)
					bIsFailed	= TRUE;
			}
			else
			{
				if ((INT32) (m_astTableRune[lNumConvertedRuneAttr + 1].lArmourProbability * fRuneSuccessProbability) < lRandomNumber)
					bIsFailed	= TRUE;
			}
		}
	}

	if (bIsFailed)
	{
		INT32	lNumConvertedRuneAttr	= GetNumRune(pcsItem);

		if (lNumConvertedRuneAttr >= 0 &&
			lNumConvertedRuneAttr < AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		{
			lRandomNumber	= m_csRandom.randInt(100);

			if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
			{
				if (m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lWeaponKeepCurrent >= lRandomNumber)
					return AGPDITEMCONVERT_RUNE_RESULT_FAILED;
				else if (m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lWeaponKeepCurrent +
					m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lWeaponInitializeSame >= lRandomNumber)
					return AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT_SAME;
				else if (m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lWeaponKeepCurrent +
					m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lWeaponInitializeSame +
					m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lWeaponInitialize >= lRandomNumber)
					return AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT;
				else
					return AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_DESTROY;
			}
			else
			{
				if (m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lArmourKeepCurrent >= lRandomNumber)
					return AGPDITEMCONVERT_RUNE_RESULT_FAILED;
				else if (m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lArmourKeepCurrent +
					m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lArmourInitializeSame >= lRandomNumber)
					return AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT_SAME;
				else if (m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lArmourKeepCurrent +
					m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lArmourInitializeSame +
					m_astTableRuneWeaponFail[lNumConvertedRuneAttr + 1].lArmourInitialize >= lRandomNumber)
					return AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT;
				else
					return AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_DESTROY;
			}
		}
	}

	return AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;
}

BOOL AgpmItemConvert::InitializeRuneConvert(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);
	if(!pcsAttachData)
		return FALSE;

	for (int i = 0; i < AGPDITEMCONVERT_MAX_WEAPON_SOCKET; ++i)
	{
		if (pcsAttachData->m_stSocketAttr[i].bIsSpiritStone)
			continue;

		AgpdItemTemplate* pcsTemplate = pcsAttachData->m_stSocketAttr[i].pcsItemTemplate;
		if (pcsTemplate)
		{
			if (pcsTemplate->m_nType == AGPMITEM_TYPE_USABLE &&	((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_RUNE)
			{
				for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; ++i)
				{
					RemoveItemOptionRune(pcsItem, pcsTemplate->m_alOptionTID[i]);
				}
			}
		}

		pcsAttachData->m_stSocketAttr[i].lTID				= AP_INVALID_IID;
		pcsAttachData->m_stSocketAttr[i].pcsItemTemplate	= NULL;
	}

	pcsAttachData->m_lNumConvert = 0;

	return TRUE;
}

BOOL AgpmItemConvert::AddRuneConvert(AgpdItem *pcsItem, AgpdItem *pcsRuneItem)
{
	if (!pcsItem || !pcsRuneItem)
		return FALSE;

	return AddRuneConvert(pcsItem, (AgpdItemTemplate *) pcsRuneItem->m_pcsItemTemplate);
}

BOOL AgpmItemConvert::AddRuneConvert(AgpdItem *pcsItem, AgpdItemTemplate *pcsItemTemplate, BOOL bIsUpdateFactor)
{
	if (!pcsItem || !pcsItemTemplate)
		return FALSE;

	if (((AgpdItemTemplateUsableRune *) pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_CONVERT)
	{
		EnumCallback(AGPDITEMCONVERT_CB_CONVERT_AS_DROP, pcsItem, NULL);

		return TRUE;
	}

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);
	if(!pcsAttachData)
		return FALSE;

	if (pcsAttachData->m_lNumConvert >= 0 && pcsAttachData->m_lNumConvert < AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
	{
		pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert].bIsSpiritStone	= FALSE;
		pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert].lTID			= pcsItemTemplate->m_lID;
		pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert].pcsItemTemplate	= pcsItemTemplate;

		++pcsAttachData->m_lNumConvert;

		for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; ++i)
		{
			AddItemOptionRune(pcsItem, pcsItemTemplate->m_alOptionTID[i], FALSE);
		}
	}

	return TRUE;
}

AgpdItemConvertRuneResult AgpmItemConvert::ProcessRuneConvert(AgpdItem *pcsItem, AgpdItem *pcsRuneItem)
{
	if (!pcsItem || !pcsRuneItem)
		return AGPDITEMCONVERT_RUNE_RESULT_FAILED;

	AgpdItemConvertRuneResult	eResult	= RuneConvert(pcsItem, pcsRuneItem);

	BOOL	bRemoveRuneItem	= TRUE;
	BOOL	bRemoveItem		= FALSE;

	// 만약 개조된 아이템에 운명의 기원석(개조 기원석)을 적용하려면 실패.
	if (((AgpdItemTemplateUsableRune *) pcsRuneItem->m_pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_CONVERT)
	{
		if (GetNumConvertedSocket(pcsItem) ||
			GetNumPhysicalConvert(pcsItem) ||
			pcsItem->m_aunOptionTID[0])
		{
			bRemoveRuneItem = FALSE;
			eResult = AGPDITEMCONVERT_RUNE_RESULT_FAILED;
		}
	}

	switch (eResult) {
	case AGPDITEMCONVERT_RUNE_RESULT_SUCCESS:
		{
			AddRuneConvert(pcsItem, pcsRuneItem);
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_FAILED:
		{
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT_SAME:
		{
			InitializeSocket(pcsItem, TRUE);
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT:
		{
			InitializePhysicalConvert(pcsItem);
			InitializeSocket(pcsItem, TRUE);
		}
		break;

	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_DESTROY:
		{
			bRemoveItem		= TRUE;
		}
		break;

	default :
		{
			bRemoveRuneItem = FALSE;
		}
		break;
	}

	if (bRemoveRuneItem)
	{
		m_pcsAgpmItem->SubItemStackCount(pcsRuneItem, 1);

		/*
		strncpy(pcsRuneItem->m_szDeleteReason, "기원석개조로 아이템 삭제", AGPMITEM_MAX_DELETE_REASON);
		m_pcsAgpmItem->RemoveItem(pcsRuneItem->m_lID, TRUE);
		*/
	}

	EnumCallback(AGPDITEMCONVERT_CB_PROCESS_RUNE_CONVERT, pcsItem, &eResult);

	if (bRemoveItem)
	{
		ZeroMemory(pcsItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsItem->m_szDeleteReason, "Rune convert was failed", AGPMITEM_MAX_DELETE_REASON);
		m_pcsAgpmItem->RemoveItem(pcsItem->m_lID, TRUE);
	}

	return eResult;
}

BOOL AgpmItemConvert::OnOperationRequestRuneConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lRuneIID)
{
	if (!pcsCharacter || !pcsItem || lRuneIID == AP_INVALID_IID)
		return FALSE;

	AgpdItem	*pcsRuneItem	= m_pcsAgpmItem->GetItem(lRuneIID);
	if (!pcsRuneItem)
		return FALSE;

	if (pcsItem->m_pcsCharacter != pcsCharacter ||
		pcsRuneItem->m_pcsCharacter != pcsCharacter)
		return FALSE;

	AgpdItemConvertRuneResult	eResult	= ProcessRuneConvert(pcsItem, pcsRuneItem);

	return TRUE;
}

BOOL AgpmItemConvert::OnOperationResponseRuneConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT32 lTID)
{
	if (!pcsCharacter || cActionResult == (-1))
		return FALSE;

	AgpdItemConvertRuneResult	eResult			= (AgpdItemConvertRuneResult) cActionResult;

	BOOL	bIsUpdated	= TRUE;

//	switch (eResult) {
//	case AGPDITEMCONVERT_RUNE_RESULT_SUCCESS:
//		{
//			AddRuneConvert(pcsItem, m_pcsAgpmItem->GetItemTemplate(lTID), FALSE);
//
//			bIsUpdated	= TRUE;
//		}
//		break;
//
//	case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT:
//		{
//			InitializeSocket(pcsItem);
//
//			bIsUpdated	= TRUE;
//		}
//		break;
//	}

	if (pcsItem)
	{
		if (pcsItem->m_pcsCharacter != pcsCharacter ||
			cNumSocket < 0 || cNumSocket > AGPDITEMCONVERT_MAX_WEAPON_SOCKET ||
			cNumConvertedSocket < 0 || cNumConvertedSocket > cNumSocket)
			return FALSE;

		AgpdItemConvertADItem		*pcsAttachData	= GetADItem(pcsItem);

		pcsAttachData->m_lPhysicalConvertLevel		= (INT32) cNumPhysicalConvert;
		pcsAttachData->m_lNumSocket					= (INT32) cNumSocket;
		pcsAttachData->m_lNumConvert				= (INT32) cNumConvertedSocket;

		if (eResult == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS &&
			pcsAttachData->m_lNumConvert > 0)
		{
			pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert - 1].bIsSpiritStone	= FALSE;
			pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert - 1].lTID			= lTID;
			pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert - 1].pcsItemTemplate	= m_pcsAgpmItem->GetItemTemplate(lTID);
		}

		if (bIsUpdated)
			EnumCallback(AGPDITEMCONVERT_CB_UPDATE, pcsItem, NULL);
	}

	EnumCallback(AGPDITEMCONVERT_CB_RESULT_RUNE_CONVERT, pcsItem, &eResult);

	return TRUE;
}

BOOL AgpmItemConvert::OnOperationCheckCashRuneConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lRuneIID)
{
	if (!pcsCharacter || !pcsItem || lRuneIID == AP_INVALID_IID)
		return FALSE;

	AgpdItem	*pcsRuneItem	= m_pcsAgpmItem->GetItem(lRuneIID);
	if (!pcsRuneItem)
		return FALSE;

	if (pcsItem->m_pcsCharacter != pcsCharacter ||
		pcsRuneItem->m_pcsCharacter != pcsCharacter)
		return FALSE;

	return CBRequestConvert(pcsItem, this, pcsRuneItem);
}

BOOL AgpmItemConvert::CBRequestRuneConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmItemConvert		*pThis			= (AgpmItemConvert *)	pClass;
	AgpdItem			*pcsItem		= (AgpdItem *)			pData;
	AgpdItem			*pcsRuneItem	= (AgpdItem *)			pCustData;

	if (!pcsItem->m_pcsCharacter ||
		pcsItem->m_pcsCharacter != pcsRuneItem->m_pcsCharacter)
		return FALSE;

	AgpdItemConvertRuneResult	eResult	= pThis->IsValidRuneItem(pcsRuneItem);
	if (eResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= (PVOID) pcsRuneItem;
		pvBuffer[1]	= (PVOID) eResult;

		pThis->EnumCallback(AGPDITEMCONVERT_CB_RUNE_CONVERT_CHECK_RESULT, pcsItem, pvBuffer);

		return TRUE;
	}

	eResult	= pThis->IsRuneConvertable(pcsItem);
	if (eResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= (PVOID) pcsRuneItem;
		pvBuffer[1]	= (PVOID) eResult;

		pThis->EnumCallback(AGPDITEMCONVERT_CB_RUNE_CONVERT_CHECK_RESULT, pcsItem, pvBuffer);

		return TRUE;
	}

	eResult	= pThis->IsProperPart(pcsItem, pcsRuneItem);
	if (eResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= (PVOID) pcsRuneItem;
		pvBuffer[1]	= (PVOID) eResult;

		pThis->EnumCallback(AGPDITEMCONVERT_CB_RUNE_CONVERT_CHECK_RESULT, pcsItem, pvBuffer);

		return TRUE;
	}

	eResult	= pThis->IsProperLevel(pcsItem, pcsRuneItem);
	if (eResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= (PVOID) pcsRuneItem;
		pvBuffer[1]	= (PVOID) eResult;

		pThis->EnumCallback(AGPDITEMCONVERT_CB_RUNE_CONVERT_CHECK_RESULT, pcsItem, pvBuffer);

		return TRUE;
	}

	eResult	= pThis->IsAntiNumber(pcsItem, pcsRuneItem);
	if (eResult != AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= (PVOID) pcsRuneItem;
		pvBuffer[1]	= (PVOID) eResult;

		pThis->EnumCallback(AGPDITEMCONVERT_CB_RUNE_CONVERT_CHECK_RESULT, pcsItem, pvBuffer);

		return TRUE;
	}

	eResult	= AGPDITEMCONVERT_RUNE_RESULT_SUCCESS;

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID) pcsRuneItem;
	pvBuffer[1]	= (PVOID) eResult;

	pThis->EnumCallback(AGPDITEMCONVERT_CB_RUNE_CONVERT_CHECK_RESULT, pcsItem, pvBuffer);

	return TRUE;
}

BOOL AgpmItemConvert::OnOperationResponseRuneCheckResult(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lRuneID, INT8 cActionResult)
{
	if (!pcsCharacter || !pcsItem || lRuneID == AP_INVALID_IID)
		return FALSE;

	AgpdItemConvertRuneResult	eResult	= (AgpdItemConvertRuneResult) cActionResult;

	if (eResult == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
	{
		AgpdItem	*pcsRuneItem	= m_pcsAgpmItem->GetItem(lRuneID);

		EnumCallback(AGPDITEMCONVERT_CB_ASK_REALLY_RUNE_CONVERT, pcsItem, pcsRuneItem);
	}
	else
	{
		EnumCallback(AGPDITEMCONVERT_CB_RESULT_RUNE_CONVERT, pcsItem, &eResult);
	}

	return TRUE;
}

BOOL AgpmItemConvert::IsConvertedAttribute(AgpdItem *pcsItem, AgpmItemRuneAttribute eRuneAttribute)
{
	if (!pcsItem)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);
	if (pcsAttachData->m_lNumConvert <= 0)
		return FALSE;

	for (int i = 0; i < pcsAttachData->m_lNumConvert; ++i)
	{
		if (pcsAttachData->m_stSocketAttr[i].bIsSpiritStone ||
			!pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)
			continue;

		if (((AgpdItemTemplateUsableRune *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == eRuneAttribute)
			return TRUE;
	}

	return FALSE;
}

INT32 AgpmItemConvert::GetConvertedExtraType(AgpdItem* pcsItem, AgpmItemRuneAttribute eRuneAttribute)
{
	if (!pcsItem)
		return 0;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);
	if (pcsAttachData->m_lNumConvert <= 0)
		return 0;

	for (int i = 0; i < pcsAttachData->m_lNumConvert; ++i)
	{
		if (pcsAttachData->m_stSocketAttr[i].bIsSpiritStone ||
			!pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)
			continue;

		if (((AgpdItemTemplateUsableRune *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == eRuneAttribute)
			return pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_lExtraType;
	}

	return 0;
}

BOOL AgpmItemConvert::CalcRuneConvertFactor(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);
	if (pcsAttachData->m_lNumConvert <= 0 || pcsAttachData->m_lNumConvert > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		return TRUE;

	for (int i = 0; i < pcsAttachData->m_lNumConvert; ++i)
	{
		AgpdItemTemplate	*pcsTemplate	= pcsAttachData->m_stSocketAttr[i].pcsItemTemplate;
		if (pcsTemplate)
		{
			if (pcsTemplate->m_nType == AGPMITEM_TYPE_USABLE &&	((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_RUNE)
			{
				for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; ++i)
				{
					AddItemOptionRune(pcsItem, pcsTemplate->m_alOptionTID[i], FALSE);
				}
			}
		}
	}

	return TRUE;
}

BOOL AgpmItemConvert::AddItemOptionRune(AgpdItem *pcsItem, INT32 lItemOptionTID, BOOL bIsValidCheck)
{
	if (!pcsItem )
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);
	if(!pcsAttachData)
		return FALSE;

	AgpdItemOptionTemplate *pcsItemOptionTemplate = m_pcsAgpmItem->GetItemOptionTemplate(lItemOptionTID);
	if(!pcsItemOptionTemplate)
		return FALSE;

	if (bIsValidCheck)
	{
		if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lMaxOptionNum > AGPDITEM_OPTION_MAX_NUM)
			((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lMaxOptionNum	= AGPDITEM_OPTION_MAX_NUM;

		if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lMaxOptionNum <= 0 ||
			pcsItem->m_aunOptionTIDRune[((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lMaxOptionNum - 1] != 0)
			return FALSE;

		/*if (IsAlreadySetType(pcsItem, pcsItemOptionTemplate))
			return FALSE;*/

		// 2007.01.31. steeple
		// 옵션이 날라갔다.....
		if (!m_pcsAgpmItem->IsProperPart((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate, pcsItemOptionTemplate))
			return FALSE;
	}

	int i = 0;
	for (i = 0; i < AGPDITEM_OPTION_RUNE_MAX_NUM; ++i)
	{
		if (pcsItem->m_aunOptionTIDRune[i] == 0)
			break;
	}

	if (i == AGPDITEM_OPTION_RUNE_MAX_NUM)
		return FALSE;

	pcsItem->m_aunOptionTIDRune[i]			= pcsItemOptionTemplate->m_lID;
	pcsItem->m_apcsOptionTemplateRune[i]	= pcsItemOptionTemplate;

	/*m_pagpmFactors->CalcFactor(&pcsItem->m_csFactor, &pcsItemOptionTemplate->m_csFactor, TRUE, FALSE, TRUE);
	m_pagpmFactors->CalcFactor(&pcsItem->m_csFactorPercent, &pcsItemOptionTemplate->m_csFactorPercent, TRUE, FALSE, TRUE);

	EnumCallback(ITEM_CB_ID_ADD_ITEM_OPTION, pcsItem, pcsItemOptionTemplate);*/

	// 업데이트 시키고 변경된 값을 아템 주인에게 보내준다.
	PVOID	pvPacketFactor = m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactor, &pcsItemOptionTemplate->m_csFactor, TRUE, TRUE, TRUE);
	pvPacketFactor = m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactor, &pcsItemOptionTemplate->m_csSkillFactor, TRUE, TRUE, TRUE);
	PVOID	pvPacketFactorPercent = m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactorPercent, &pcsItemOptionTemplate->m_csFactorPercent, TRUE, TRUE, TRUE);
	pvPacketFactorPercent = m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactorPercent, &pcsItemOptionTemplate->m_csSkillFactorPercent, TRUE, TRUE, TRUE);

	pcsAttachData->m_stOptionSkillData += pcsItemOptionTemplate->m_stSkillData;

	PVOID	pvBuffer[2];
	pvBuffer[0]		= pvPacketFactor;
	pvBuffer[1]		= pvPacketFactorPercent;

	EnumCallback(AGPDITEMCONVERT_CB_SEND_UPDATE_FACTOR, pcsItem, pvBuffer);

	m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacketFactor);
	m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacketFactorPercent);

	return TRUE;
}

BOOL AgpmItemConvert::RemoveItemOptionRune(AgpdItem *pcsItem, INT32 lItemOptionTID)
{
	if (!pcsItem)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);
	if(!pcsAttachData)
		return FALSE;

	AgpdItemOptionTemplate *pcsItemOptionTemplate = m_pcsAgpmItem->GetItemOptionTemplate(lItemOptionTID);
	if(!pcsItemOptionTemplate)
		return FALSE;

	for(int i = 0;i < AGPDITEM_OPTION_RUNE_MAX_NUM; i++)
	{
		if (pcsItem->m_aunOptionTIDRune[i] != lItemOptionTID)
			continue;

		pcsItem->m_aunOptionTIDRune[i]			= 0;
		pcsItem->m_apcsOptionTemplateRune[i]	= NULL;

		// 업데이트 시키고 변경된 값을 아템 주인에게 보내준다.
		PVOID	pvPacketFactor = m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactor, &pcsItemOptionTemplate->m_csFactor, TRUE, TRUE, FALSE, FALSE);
		PVOID	pvPacketFactorPercent = m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactorPercent, &pcsItemOptionTemplate->m_csFactorPercent, TRUE, TRUE, FALSE, FALSE);

		pvPacketFactor = m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactor, &pcsItemOptionTemplate->m_csSkillFactor, TRUE, TRUE, FALSE, FALSE);
		pvPacketFactorPercent = m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactorPercent, &pcsItemOptionTemplate->m_csSkillFactorPercent, TRUE, TRUE, FALSE, FALSE);

		pcsAttachData->m_stOptionSkillData -= pcsItemOptionTemplate->m_stSkillData;

		PVOID	pvBuffer[2];
		pvBuffer[0]		= pvPacketFactor;
		pvBuffer[1]		= pvPacketFactorPercent;

		EnumCallback(AGPDITEMCONVERT_CB_SEND_UPDATE_FACTOR, pcsItem, pvBuffer);

		m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacketFactor);
		m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacketFactorPercent);

		return TRUE; // 한개만 없애야 하므로
	}

	return FALSE;
}