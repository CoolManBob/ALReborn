#include "AgpmItemConvert.h"

AgpdItemConvertResult AgpmItemConvert::IsPhysicalConvertable(AgpdItem *pcsItem, BOOL bIsCheckStatus)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate ||
		(bIsCheckStatus && pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY))
		return AGPDITEMCONVERT_RESULT_FAILED;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP)
		return AGPDITEMCONVERT_RESULT_INVALID_ITEM;

	if (AGPMITEM_EQUIP_KIND_WEAPON == ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind)
	{
		if (AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE == ((AgpdItemTemplateEquipWeapon *) pcsItem->m_pcsItemTemplate)->m_nWeaponType)
			return AGPDITEMCONVERT_RESULT_INVALID_ITEM;
	}

	if (!(((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON ||
		 (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR &&
		  (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nPart == AGPMITEM_PART_BODY ||
		   ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nPart == AGPMITEM_PART_LEGS))))
		return AGPDITEMCONVERT_RESULT_INVALID_ITEM;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (pcsAttachData->m_lPhysicalConvertLevel >= GetPhysicalItemRank(pcsItem))
		return AGPDITEMCONVERT_RESULT_IS_ALREADY_FULL;

	if (IsEgoItem(pcsItem))
		return AGPDITEMCONVERT_RESULT_IS_EGO_ITEM;

	return AGPDITEMCONVERT_RESULT_SUCCESS;
}

AgpdItemConvertResult AgpmItemConvert::IsValidCatalyst(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return AGPDITEMCONVERT_RESULT_FAILED;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_USABLE ||
		((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_CONVERT_CATALYST)
		return AGPDITEMCONVERT_RESULT_INVALID_CATALYST;

	if (pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY
		&& pcsItem->m_eStatus != AGPDITEM_STATUS_SUB_INVENTORY)
		return AGPDITEMCONVERT_RESULT_INVALID_CATALYST;

	return AGPDITEMCONVERT_RESULT_SUCCESS;
}

INT32 AgpmItemConvert::GetPhysicalConvertLevel(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return (-1);

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	return pcsAttachData->m_lPhysicalConvertLevel;
}

AgpdItemConvertResult AgpmItemConvert::PhysicalConvert(AgpdItem *pcsItem, AgpdItem *pcsCatalystItem)
{
	if (!pcsItem || !pcsCatalystItem)
		return AGPDITEMCONVERT_RESULT_FAILED;

	// 개조가 가능한 넘들인지 검사한다.
	AgpdItemConvertResult	eCheckResult	= IsPhysicalConvertable(pcsItem);
	if (eCheckResult != AGPDITEMCONVERT_RESULT_SUCCESS)
		return eCheckResult;

	eCheckResult	= IsValidCatalyst(pcsCatalystItem);
	if (eCheckResult != AGPDITEMCONVERT_RESULT_SUCCESS)
		return eCheckResult;

	INT32	lItemRank	= GetPhysicalItemRank(pcsItem);

	// 현재 개조 레벨을 가져온다.
	INT32	lConvertLevel	= GetPhysicalConvertLevel(pcsItem);
	if (lConvertLevel < 0 ||
		lConvertLevel >= lItemRank)
		return AGPDITEMCONVERT_RESULT_FAILED;

	++lConvertLevel;

	INT32	lRandomNumber	= m_csRandom.randInt(99);

	FLOAT	fSuccessProb	= 1.0f;

	PVOID pvBuffer[2];
	pvBuffer[0] = (PVOID) pcsItem;
	pvBuffer[1] = (PVOID) pcsCatalystItem;

	EnumCallback(AGPDITEMCONVERT_CB_ADJUST_SUCCESS_PROB, pvBuffer, &fSuccessProb);

	if (lRandomNumber < (INT32) (m_astTablePhysical[lItemRank].lSuccessProb[lConvertLevel] * fSuccessProb))
		return AGPDITEMCONVERT_RESULT_SUCCESS;
	else if (lRandomNumber < (INT32) (m_astTablePhysical[lItemRank].lSuccessProb[lConvertLevel] * fSuccessProb) +
							 m_astTablePhysical[lItemRank].lFailProb[lConvertLevel])
		return AGPDITEMCONVERT_RESULT_FAILED;
	else if (lRandomNumber < (INT32) (m_astTablePhysical[lItemRank].lSuccessProb[lConvertLevel] * fSuccessProb) +
							 m_astTablePhysical[lItemRank].lFailProb[lConvertLevel] +
							 m_astTablePhysical[lItemRank].lInitProb[lConvertLevel])
		return AGPDITEMCONVERT_RESULT_FAILED_AND_INIT_SAME;
	else
		return AGPDITEMCONVERT_RESULT_FAILED_AND_DESTROY;

	return AGPDITEMCONVERT_RESULT_FAILED;
}

BOOL AgpmItemConvert::InitializePhysicalConvert(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	// 이전 개조에 의해 추가되었던 속성들을 모두 뺀다.

	INT32	lItemRank	= GetPhysicalItemRank(pcsItem);

	if (pcsAttachData->m_lPhysicalConvertLevel > lItemRank)
		pcsAttachData->m_lPhysicalConvertLevel	= lItemRank;
	
	INT32	lAttrValue	= 0;

	if (IsWeapon(pcsItem))
	{
		for (int i = 1; i <= pcsAttachData->m_lPhysicalConvertLevel; ++i)
		{
			lAttrValue += m_astTablePhysical[i].lWeaponAddValue;
		}
	}
	else
	{
		for (int i = 1; i <= pcsAttachData->m_lPhysicalConvertLevel; ++i)
		{
			lAttrValue += m_astTablePhysical[i].lArmourAddValue;
		}
	}

	AgpdFactor		csUpdateFactor;
	m_pcsAgpmFactors->InitFactor(&csUpdateFactor);

	if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
	{
		AgpdFactorDamage	*pcsFactorDamage	= (AgpdFactorDamage *) m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DAMAGE);
		if (!pcsFactorDamage)
		{
			m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
			return FALSE;
		}

		m_pcsAgpmFactors->SetValue(&csUpdateFactor, (-lAttrValue), AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
		m_pcsAgpmFactors->SetValue(&csUpdateFactor, (-lAttrValue), AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	}
	else
	{
		AgpdFactorDefense	*pcsFactorDefense	= (AgpdFactorDefense *) m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DEFENSE);
		if (!pcsFactorDefense)
		{
			m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
			return FALSE;
		}

		m_pcsAgpmFactors->SetValue(&csUpdateFactor, (-lAttrValue), AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	}

	PVOID	pvPacket	= m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactor, &csUpdateFactor, TRUE, TRUE, TRUE);

	m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);

	pcsAttachData->m_lPhysicalConvertLevel	= 0;

	PVOID	pvBuffer[2];
	pvBuffer[0]		= pvPacket;
	pvBuffer[1]		= NULL;

	EnumCallback(AGPDITEMCONVERT_CB_SEND_UPDATE_FACTOR, pcsItem, pvBuffer);

	m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgpmItemConvert::ApplyPhysicalConvert(AgpdItem *pcsItem, BOOL bIsUpdateFactor)
{
	if (!pcsItem)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	INT32	lItemRank	= GetPhysicalItemRank(pcsItem);

	if (pcsAttachData->m_lPhysicalConvertLevel + 1 > lItemRank ||
		pcsAttachData->m_lPhysicalConvertLevel < 0)
		return FALSE;

	if (bIsUpdateFactor)
	{
		// 개조에 따른 부가 속성들을 적당한 Factor에 적용시킨다.
		AgpdFactor		csUpdateFactor;
		m_pcsAgpmFactors->InitFactor(&csUpdateFactor);

		if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
		{
			AgpdFactorDamage	*pcsFactorDamage	= (AgpdFactorDamage *) m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DAMAGE);
			if (!pcsFactorDamage)
			{
				m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
				return FALSE;
			}

			m_pcsAgpmFactors->SetValue(&csUpdateFactor, m_astTablePhysical[pcsAttachData->m_lPhysicalConvertLevel + 1].lWeaponAddValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			m_pcsAgpmFactors->SetValue(&csUpdateFactor, m_astTablePhysical[pcsAttachData->m_lPhysicalConvertLevel + 1].lWeaponAddValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
		}
		else
		{
			AgpdFactorDefense	*pcsFactorDefense	= (AgpdFactorDefense *) m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DEFENSE);
			if (!pcsFactorDefense)
			{
				m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
				return FALSE;
			}

			m_pcsAgpmFactors->SetValue(&csUpdateFactor, m_astTablePhysical[pcsAttachData->m_lPhysicalConvertLevel + 1].lArmourAddValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
		}

		PVOID	pvPacket	= m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactor, &csUpdateFactor, TRUE, TRUE, TRUE);

		m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);

		PVOID	pvBuffer[2];
		pvBuffer[0]		= pvPacket;
		pvBuffer[1]		= NULL;

		EnumCallback(AGPDITEMCONVERT_CB_SEND_UPDATE_FACTOR, pcsItem, pvBuffer);

		m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacket);
	}

	++pcsAttachData->m_lPhysicalConvertLevel;

	return TRUE;
}

BOOL AgpmItemConvert::CalcPhysicalConvertFactor(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (pcsAttachData->m_lPhysicalConvertLevel == 0)
		return TRUE;

	INT32	lItemRank	= GetPhysicalItemRank(pcsItem);

	if (pcsAttachData->m_lPhysicalConvertLevel > lItemRank ||
		pcsAttachData->m_lPhysicalConvertLevel < 0)
		return FALSE;

	// 개조에 따른 부가 속성들을 적당한 Factor에 적용시킨다.
	INT32	lAddValue	= 0;

	for (int i = 1; i <= pcsAttachData->m_lPhysicalConvertLevel; ++i)
	{
		if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
		{
			lAddValue	+= m_astTablePhysical[i].lWeaponAddValue;
		}
		else
		{
			lAddValue	+= m_astTablePhysical[i].lArmourAddValue;
		}
	}

	if (lAddValue == 0)
		return TRUE;

	AgpdFactor		csUpdateFactor;
	m_pcsAgpmFactors->InitFactor(&csUpdateFactor);

	if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
	{
		AgpdFactorDamage	*pcsFactorDamage	= (AgpdFactorDamage *) m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DAMAGE);
		if (!pcsFactorDamage)
			return FALSE;

		m_pcsAgpmFactors->SetValue(&csUpdateFactor, lAddValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
		m_pcsAgpmFactors->SetValue(&csUpdateFactor, lAddValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	}
	else
	{
		AgpdFactorDefense	*pcsFactorDefense	= (AgpdFactorDefense *) m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DEFENSE);
		if (!pcsFactorDefense)
			return FALSE;

		m_pcsAgpmFactors->SetValue(&csUpdateFactor, lAddValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	}

	m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactor, &csUpdateFactor, TRUE, FALSE, TRUE);

	m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);

	return TRUE;
}

AgpdItemConvertResult AgpmItemConvert::ProcessPhysicalConvert(AgpdItem *pcsItem, AgpdItem *pcsCatalystItem)
{
	if (!pcsItem || !pcsCatalystItem)
		return AGPDITEMCONVERT_RESULT_FAILED;

	AgpdItemConvertResult	eConvertResult	= PhysicalConvert(pcsItem, pcsCatalystItem);

	BOOL	bDestroyCatalyst	= FALSE;
	BOOL	bDestroyItem		= FALSE;

	switch (eConvertResult) {
	case AGPDITEMCONVERT_RESULT_FAILED:
		{
			bDestroyCatalyst	= TRUE;
		}
		break;

	case AGPDITEMCONVERT_RESULT_FAILED_AND_INIT_SAME:
		{
			bDestroyCatalyst	= TRUE;

			InitializePhysicalConvert(pcsItem);
		}
		break;

		/*
	case AGPDITEMCONVERT_RESULT_FAILED_AND_INIT:
		{
			bDestroyCatalyst	= TRUE;

			InitializePhysicalConvert(pcsItem);
			InitializeSocket(pcsItem);
		}
		break;
		*/

	case AGPDITEMCONVERT_RESULT_FAILED_AND_DESTROY:
		{
			bDestroyCatalyst	= TRUE;
			bDestroyItem		= TRUE;
		}
		break;

	case AGPDITEMCONVERT_RESULT_SUCCESS:
		{
			bDestroyCatalyst	= TRUE;
			ApplyPhysicalConvert(pcsItem);
		}
		break;
	}

	EnumCallback(AGPDITEMCONVERT_CB_PROCESS_PHYSICAL_CONVERT, pcsItem, &eConvertResult);

	if (bDestroyCatalyst)
	{
		m_pcsAgpmItem->SubItemStackCount(pcsCatalystItem, 1);

//		strncpy(pcsCatalystItem->m_szDeleteReason, "물리개조로 인한 강화축성제 삭제", AGPMITEM_MAX_DELETE_REASON);
//		m_pcsAgpmItem->RemoveItem(pcsCatalystItem->m_lID, TRUE);
	}

	if (bDestroyItem)
	{
		ZeroMemory(pcsItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsItem->m_szDeleteReason, "Physical convert was failed", AGPMITEM_MAX_DELETE_REASON);
		m_pcsAgpmItem->RemoveItem(pcsItem->m_lID, TRUE);
	}

	return eConvertResult;
}

INT32 AgpmItemConvert::GetNumPhysicalConvert(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return 0;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	return pcsAttachData->m_lPhysicalConvertLevel;
}

BOOL AgpmItemConvert::OnOperationRequestPhysicalConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lCatalystIID)
{
	if (!pcsCharacter || !pcsItem || lCatalystIID == AP_INVALID_IID)
		return FALSE;

	AgpdItem	*pcsCatalystItem	= m_pcsAgpmItem->GetItem(lCatalystIID);
	if (!pcsCatalystItem)
		return FALSE;

	if (pcsItem->m_pcsCharacter != pcsCharacter ||
		pcsCatalystItem->m_pcsCharacter != pcsCharacter)
		return FALSE;

	AgpdItemConvertResult	eResult	= ProcessPhysicalConvert(pcsItem, pcsCatalystItem);

	return TRUE;
}

BOOL AgpmItemConvert::OnOperationResponsePhysicalConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket)
{
	if (!pcsCharacter || cActionResult == (-1))
		return FALSE;

	AgpdItemConvertResult	eResult	= (AgpdItemConvertResult) cActionResult;

	BOOL	bIsUpdated	= TRUE;

//	switch (eResult) {
//	case AGPDITEMCONVERT_RESULT_SUCCESS:
//		{
////			ApplyPhysicalConvert(pcsItem, FALSE);
//
//			bIsUpdated	= TRUE;
//		}
//		break;
//
//	case AGPDITEMCONVERT_RESULT_FAILED_AND_INIT_SAME:
//	case AGPDITEMCONVERT_RESULT_FAILED_AND_INIT:
//		{
////			InitializePhysicalConvert(pcsItem);
//	
//			bIsUpdated	= TRUE;
//		}
//		break;
//	}

	if (pcsItem)
	{
		if (pcsItem->m_pcsCharacter != pcsCharacter ||
			cNumPhysicalConvert == (-1))
			return FALSE;

		AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

		if (pcsAttachData->m_lPhysicalConvertLevel != (INT32) cNumPhysicalConvert)
			pcsAttachData->m_lPhysicalConvertLevel = (INT32) cNumPhysicalConvert;

		if (pcsAttachData->m_lNumSocket != (INT32) cNumSocket)
			pcsAttachData->m_lNumSocket	= (INT32) cNumSocket;

		if (pcsAttachData->m_lNumConvert != (INT32) cNumConvertedSocket)
			pcsAttachData->m_lNumConvert = (INT32) cNumConvertedSocket;

		if (bIsUpdated)
			EnumCallback(AGPDITEMCONVERT_CB_UPDATE, pcsItem, NULL);
	}

	EnumCallback(AGPDITEMCONVERT_CB_RESULT_PHYSICAL_CONVERT, pcsItem, &eResult);

	return TRUE;
}

BOOL AgpmItemConvert::SetPhysicalConvert(AgpdItem *pcsItem, INT32 lConvertLevel, BOOL bIsCheckStatus)
{
	if (!pcsItem)
		return FALSE;

	INT32	lItemRank	= GetPhysicalItemRank(pcsItem);

	if (lConvertLevel < 1 ||
		lConvertLevel > lItemRank)
		return FALSE;

	// 개조가 가능한 넘들인지 검사한다.
	AgpdItemConvertResult	eCheckResult	= IsPhysicalConvertable(pcsItem, bIsCheckStatus);
	if (eCheckResult != AGPDITEMCONVERT_RESULT_SUCCESS)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	pcsAttachData->m_lPhysicalConvertLevel	= lConvertLevel;

	if (CalcPhysicalConvertFactor(pcsItem))
	{
		AgpdItemConvertResult	eResult	= AGPDITEMCONVERT_RESULT_SUCCESS;
		EnumCallback(AGPDITEMCONVERT_CB_PROCESS_PHYSICAL_CONVERT, pcsItem, &eResult);
	}

	return TRUE;
}

INT32 AgpmItemConvert::GetPhysicalItemRank(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return 0;

	INT32	lItemRank	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_PHYSICAL_RANK);

	if (lItemRank < 1 || lItemRank > AGPDITEMCONVERT_MAX_ITEM_RANK)
		return 0;

	return lItemRank;
}

INT32 AgpmItemConvert::GetPhysicalItemRank(AgpdItemTemplate *pcsItemTemplate)
{
	if (!pcsItemTemplate)
		return 0;

	INT32	lItemRank	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItemTemplate->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_PHYSICAL_RANK);

	if (lItemRank < 1 || lItemRank > AGPDITEMCONVERT_MAX_ITEM_RANK)
		return 0;

	return lItemRank;
}

CHAR* AgpmItemConvert::GetPhysicalItemRankName(AgpdItem *pcsItem)
{
	INT32	lItemRank	= GetPhysicalItemRank(pcsItem);

	return m_astTablePhysical[lItemRank].szRank;
}