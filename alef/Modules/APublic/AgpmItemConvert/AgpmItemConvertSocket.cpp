#include "AgpmItemConvert.h"

INT32 AgpmItemConvert::GetNumSocket(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return (-1);

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	return pcsAttachData->m_lNumSocket;
}

INT32 AgpmItemConvert::GetNumConvertedSocket(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return 0;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	return pcsAttachData->m_lNumConvert;
}

INT64 AgpmItemConvert::GetSocketCost(AgpdItem *pcsItem, INT32 lNumSocket)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return (-1);

	if (IsWeapon(pcsItem))
	{
		if (lNumSocket < 0 ||
			lNumSocket > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
			return (-1);

		return m_astTableSocket[lNumSocket].lWeaponCost;
	}
	else if (IsArmour(pcsItem))
	{
		if (lNumSocket < 0 ||
			lNumSocket > AGPDITEMCONVERT_MAX_ARMOUR_SOCKET)
			return (-1);

		return m_astTableSocket[lNumSocket].lArmourCost;
	}
	else if (IsEtc(pcsItem))
	{
		if (lNumSocket < 0 ||
			lNumSocket > AGPDITEMCONVERT_MAX_ETC_SOCKET)
			return (-1);

		return m_astTableSocket[lNumSocket].lEtcCost;
	}
	else
		return (-1);

	return (-1);
}

INT64 AgpmItemConvert::GetSocketCost(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return (-1);

	INT32	lNumSocket	= GetNumSocket(pcsItem);

	return GetSocketCost(pcsItem, lNumSocket);
}

INT64 AgpmItemConvert::GetNextSocketCost(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return (-1);

	INT32	lNumSocket	= GetNumSocket(pcsItem) + 1;

	return GetSocketCost(pcsItem, lNumSocket);
}

BOOL AgpmItemConvert::IsEnoughSocketCost(AgpdCharacter *pcsCharacter, INT32 lNumSocket, AgpdItem *pcsItem)
{
	if (!pcsCharacter ||
		!pcsItem)
		return FALSE;

	INT64	llMoney	= 0;
	m_pcsAgpmCharacter->GetMoney(pcsCharacter, &llMoney);

	if (IsWeapon(pcsItem))
	{
		if (lNumSocket < 1 ||
			lNumSocket > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
			return FALSE;

		if (m_astTableSocket[lNumSocket].lWeaponCost > llMoney)
			return FALSE;
		else
			return TRUE;
	}
	else if (IsArmour(pcsItem))
	{
		if (lNumSocket < 1 ||
			lNumSocket > AGPDITEMCONVERT_MAX_ARMOUR_SOCKET)
			return FALSE;

		if (m_astTableSocket[lNumSocket].lArmourCost > llMoney)
			return FALSE;
		else
			return TRUE;
	}
	else if (IsEtc(pcsItem))
	{
		if (lNumSocket < 1 ||
			lNumSocket > AGPDITEMCONVERT_MAX_ETC_SOCKET)
			return FALSE;

		if (m_astTableSocket[lNumSocket].lEtcCost > llMoney)
			return FALSE;
		else
			return TRUE;
	}
	else
		return FALSE;

	return TRUE;
}

BOOL AgpmItemConvert::IsWeapon(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP)
		return FALSE;

	if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
		return TRUE;

	return FALSE;
}

BOOL AgpmItemConvert::IsArmour(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP)
		return FALSE;

	if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR)
//	if ((((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR &&
//		((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nPart == AGPMITEM_PART_BODY) ||
//		((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD)
		return TRUE;

	return FALSE;
}

BOOL AgpmItemConvert::IsEtc(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP)
		return FALSE;

	if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_RING ||
		((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_NECKLACE ||
		((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD)
		return TRUE;

	return FALSE;
}

AgpdItemConvertSocketResult AgpmItemConvert::IsSocketConvertable(AgpdItem *pcsItem, BOOL IsCheckCost)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

	if (IsEgoItem(pcsItem))
		return AGPDITEMCONVERT_SOCKET_RESULT_IS_EGO_ITEM;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP)
		return AGPDITEMCONVERT_SOCKET_RESULT_INVALID_ITEM;

	if (AGPMITEM_EQUIP_KIND_WEAPON == ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind)
	{
		if (AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE == ((AgpdItemTemplateEquipWeapon *) pcsItem->m_pcsItemTemplate)->m_nWeaponType)
			return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;
	}

	if (!IsWeapon(pcsItem) && !IsArmour(pcsItem) && !IsEtc(pcsItem))
		return AGPDITEMCONVERT_SOCKET_RESULT_INVALID_ITEM;

	INT32	lCurrentSocket	= GetNumSocket(pcsItem);

	if (IsWeapon(pcsItem))
	{
		if (lCurrentSocket < 0 || lCurrentSocket > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
			return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

		if (lCurrentSocket == AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
			return AGPDITEMCONVERT_SOCKET_RESULT_IS_ALREADY_FULL;
	}
	else if (IsArmour(pcsItem))
	{
		if (lCurrentSocket < 0 || lCurrentSocket > AGPDITEMCONVERT_MAX_ARMOUR_SOCKET)
			return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

		if (lCurrentSocket == AGPDITEMCONVERT_MAX_ARMOUR_SOCKET)
			return AGPDITEMCONVERT_SOCKET_RESULT_IS_ALREADY_FULL;
	}
	else if (IsEtc(pcsItem))
	{
		if (lCurrentSocket < 0 || lCurrentSocket > AGPDITEMCONVERT_MAX_ETC_SOCKET)
			return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

		if (lCurrentSocket == AGPDITEMCONVERT_MAX_ETC_SOCKET)
			return AGPDITEMCONVERT_SOCKET_RESULT_IS_ALREADY_FULL;
	}
	else
	{
		return AGPDITEMCONVERT_SOCKET_RESULT_INVALID_ITEM;
	}

	if (IsCheckCost)
	{
		if (pcsItem->m_pcsCharacter)
		{
			if (!IsEnoughSocketCost(pcsItem->m_pcsCharacter, lCurrentSocket + 1, pcsItem))
				return AGPDITEMCONVERT_SOCKET_RESULT_NOT_ENOUGH_MONEY;
		}
	}

	return AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS;
}

BOOL AgpmItemConvert::IsSocketInitializable(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	if (0 >= GetNumConvertedSocket(pcsItem))
		return FALSE;
	
	if (IsEgoItem(pcsItem))
		return FALSE;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP)
		return FALSE;

	if (AGPMITEM_EQUIP_KIND_WEAPON == ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind)
	{
		if (AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE == ((AgpdItemTemplateEquipWeapon *) pcsItem->m_pcsItemTemplate)->m_nWeaponType)
			return FALSE;
	}

	if (!IsWeapon(pcsItem) && !IsArmour(pcsItem) && !IsEtc(pcsItem))
		return FALSE;
		
	return TRUE;
}

AgpdItemConvertSocketResult AgpmItemConvert::SocketConvert(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

	AgpdItemConvertSocketResult	eCheckResult	= IsSocketConvertable(pcsItem);
	if (eCheckResult != AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS)
		return eCheckResult;

	INT32	lNumSocket		= GetNumSocket(pcsItem) + 1;
	if (lNumSocket < 0 || lNumSocket > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

	INT32	lSuccessProb	= 0;

	if (IsWeapon(pcsItem))
		lSuccessProb	= m_astTableSocket[lNumSocket].lWeaponProb;
	else if (IsArmour(pcsItem))
		lSuccessProb	= m_astTableSocket[lNumSocket].lArmourProb;
	else if (IsEtc(pcsItem))
		lSuccessProb	= m_astTableSocket[lNumSocket].lEtcProb;
	else
		return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

	INT32	lRandomNumber	= m_csRandom.randInt(100);

	if (lSuccessProb < lRandomNumber)
	{
		// 실패했다.
		lRandomNumber	= m_csRandom.randInt(100);

		if (IsWeapon(pcsItem))
		{
			if (m_astTableSocketFail[lNumSocket].lWeaponKeepCurrent >= lRandomNumber)
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;
			else if (m_astTableSocketFail[lNumSocket].lWeaponKeepCurrent
				+ m_astTableSocketFail[lNumSocket].lWeaponInitializeSame >= lRandomNumber)
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT_SAME;
			else if (m_astTableSocketFail[lNumSocket].lWeaponKeepCurrent
				+ m_astTableSocketFail[lNumSocket].lWeaponInitializeSame
				+ m_astTableSocketFail[lNumSocket].lWeaponInitialize >= lRandomNumber)
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT;
			else
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_DESTROY;
		}
		else if (IsArmour(pcsItem))
		{
			if (m_astTableSocketFail[lNumSocket].lArmourKeepCurrent >= lRandomNumber)
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;
			else if (m_astTableSocketFail[lNumSocket].lArmourKeepCurrent
				+ m_astTableSocketFail[lNumSocket].lArmourInitializeSame >= lRandomNumber)
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT_SAME;
			else if (m_astTableSocketFail[lNumSocket].lArmourKeepCurrent
				+ m_astTableSocketFail[lNumSocket].lArmourInitializeSame
				+ m_astTableSocketFail[lNumSocket].lArmourInitialize >= lRandomNumber)
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT;
			else
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_DESTROY;
		}
		else if (IsEtc(pcsItem))
		{
			if (m_astTableSocketFail[lNumSocket].lEtcKeepCurrent >= lRandomNumber)
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;
			else if (m_astTableSocketFail[lNumSocket].lEtcKeepCurrent
				+ m_astTableSocketFail[lNumSocket].lEtcInitializeSame >= lRandomNumber)
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT_SAME;
			else if (m_astTableSocketFail[lNumSocket].lEtcKeepCurrent
				+ m_astTableSocketFail[lNumSocket].lEtcInitializeSame
				+ m_astTableSocketFail[lNumSocket].lEtcInitialize >= lRandomNumber)
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT;
			else
				return AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_DESTROY;
		}
		else
			return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

	}

	return AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS;
}

BOOL AgpmItemConvert::InitializeSocket(AgpdItem *pcsItem, BOOL bIsKeepSocket)
{
	if (!pcsItem)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	INT32	lCurrentSocket	= GetNumSocket(pcsItem);

	// 이전 개조에 의해 추가되었던 속성들을 모두 뺀다.
	InitializeSpiritStoneConvert(pcsItem);
	InitializeRuneConvert(pcsItem);

	if (bIsKeepSocket)
		pcsAttachData->m_lNumSocket		= lCurrentSocket;
	else
		pcsAttachData->m_lNumSocket		= (pcsItem->m_pcsItemTemplate->m_lMinSocketNum > 1) ? pcsItem->m_pcsItemTemplate->m_lMinSocketNum : 1;

	return TRUE;
}

BOOL AgpmItemConvert::AddSocketConvert(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (IsWeapon(pcsItem))
	{
		if (pcsAttachData->m_lNumSocket + 1 > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
			return FALSE;
	}
	else if (IsArmour(pcsItem))
	{
		if (pcsAttachData->m_lNumSocket + 1 > AGPDITEMCONVERT_MAX_ARMOUR_SOCKET)
			return FALSE;
	}
	else if (IsEtc(pcsItem))
	{
		if (pcsAttachData->m_lNumSocket + 1 > AGPDITEMCONVERT_MAX_ETC_SOCKET)
			return FALSE;
	}
	else
		return FALSE;

	++pcsAttachData->m_lNumSocket;

	return TRUE;
}

AgpdItemConvertSocketResult AgpmItemConvert::ProcessSocketConvert(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return AGPDITEMCONVERT_SOCKET_RESULT_FAILED;

	AgpdItemConvertSocketResult	eCheckResult	= SocketConvert(pcsItem);

	BOOL	bIsPayCost		= FALSE;
	BOOL	bIsDestroyItem	= FALSE;

	switch (eCheckResult) {
	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED:
		{
			bIsPayCost	= TRUE;
		}
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT_SAME:
		{
			bIsPayCost	= TRUE;

			InitializeSocket(pcsItem);
		}
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT:
		{
			bIsPayCost	= TRUE;

			InitializePhysicalConvert(pcsItem);
			InitializeSocket(pcsItem);
		}
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_DESTROY:
		{
			bIsPayCost	= TRUE;
			bIsDestroyItem	= TRUE;
		}
		break;

	case AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS:
		{
			bIsPayCost	= TRUE;

			AddSocketConvert(pcsItem);
		}
		break;
	}

	if (bIsPayCost)
	{
		INT64	llCost	= GetSocketCost(pcsItem);

		m_pcsAgpmCharacter->SubMoney(pcsItem->m_pcsCharacter, (INT64) llCost);
	}

	EnumCallback(AGPDITEMCONVERT_CB_PROCESS_SOCKET_CONVERT, pcsItem, &eCheckResult);

	if (bIsDestroyItem)
	{
		ZeroMemory(pcsItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsItem->m_szDeleteReason, "Socket convert was failed", AGPMITEM_MAX_DELETE_REASON);
		m_pcsAgpmItem->RemoveItem(pcsItem->m_lID, TRUE);
	}

	return eCheckResult;
}

BOOL AgpmItemConvert::OnOperationRequestAddSocket(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem)
{
	if (!pcsCharacter || !pcsItem)
		return FALSE;

	if (pcsItem->m_pcsCharacter != pcsCharacter)
		return FALSE;

	AgpdItemConvertSocketResult	eResult	= ProcessSocketConvert(pcsItem);

	return TRUE;
}

BOOL AgpmItemConvert::OnOperationResponseAddSocket(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket)
{
	if (!pcsCharacter || cActionResult == (-1))
		return FALSE;

	AgpdItemConvertSocketResult	eResult	= (AgpdItemConvertSocketResult) cActionResult;

	BOOL	bIsUpdated	= TRUE;

//	switch (eResult) {
//	case AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT:
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
			cNumSocket == (-1))
			return FALSE;

		AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

		pcsAttachData->m_lPhysicalConvertLevel	= (INT32) cNumPhysicalConvert;
		pcsAttachData->m_lNumSocket				= (INT32) cNumSocket;
		pcsAttachData->m_lNumConvert			= (INT32) cNumConvertedSocket;

		if (eResult	== AGPDITEMCONVERT_SOCKET_RESULT_FAILED_AND_INIT)
			pcsAttachData->m_lNumConvert	= 0;

		if (bIsUpdated)
			EnumCallback(AGPDITEMCONVERT_CB_UPDATE, pcsItem, NULL);
	}

	EnumCallback(AGPDITEMCONVERT_CB_RESULT_SOCKET_CONVERT, pcsItem, &eResult);

	return TRUE;
}

BOOL AgpmItemConvert::SetSocketConvert(AgpdItem *pcsItem, INT32 lSocketLevel)
{
	if (!pcsItem || lSocketLevel < 1)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (IsWeapon(pcsItem))
	{
		if (lSocketLevel > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
			return FALSE;
	}
	else if (IsArmour(pcsItem))
	{
		if (lSocketLevel > AGPDITEMCONVERT_MAX_ARMOUR_SOCKET)
			return FALSE;
	}
	else if (IsEtc(pcsItem))
	{
		if (lSocketLevel > AGPDITEMCONVERT_MAX_ETC_SOCKET)
			return FALSE;
	}
	else
		return FALSE;

	pcsAttachData->m_lNumSocket	= lSocketLevel;

	AgpdItemConvertSocketResult	eResult	= AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS;

	EnumCallback(AGPDITEMCONVERT_CB_PROCESS_SOCKET_CONVERT, pcsItem, &eResult);

	return TRUE;
}