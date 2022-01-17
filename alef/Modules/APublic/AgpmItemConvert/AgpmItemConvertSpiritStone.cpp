#include "AgpmItemConvert.h"

INT32 AgpmItemConvert::GetNumSpiritStone(AgpdItem *pcsItem, BOOL bGrouping)
{
	if (!pcsItem)
		return 0;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	/* 잘못 고쳤다... -_-; Parn
	//@{ kday 20050403	
	if (pcsAttachData ||
		pcsAttachData->m_lNumConvert < 0 ||
		pcsAttachData->m_lNumConvert > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		return 0;
	//@} kday
	*/
	if (pcsAttachData->m_lNumConvert < 0 ||
		pcsAttachData->m_lNumConvert > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		return 0;

	INT32	lNumSpiritStone	= 0;
	BOOL	abCalcSpiritStone[AGPMITEM_USABLE_SS_TYPE_NUM];
	AgpmItemUsableSpiritStoneType	eStoneType;

	memset(abCalcSpiritStone, 0, sizeof(BOOL) * AGPMITEM_USABLE_SS_TYPE_NUM);

	for (int i = 0; i < pcsAttachData->m_lNumConvert; ++i)
	{
		if (pcsAttachData->m_stSocketAttr[i].bIsSpiritStone)
		{
			if (!bGrouping)
				++lNumSpiritStone;
			else
			{
				eStoneType = ((AgpdItemTemplateUsableSpiritStone *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eSpiritStoneType;
				ASSERT(abCalcSpiritStone[eStoneType] > AGPMITEM_USABLE_SS_TYPE_NONE && abCalcSpiritStone[eStoneType] < AGPMITEM_USABLE_SS_TYPE_NUM);
				if (!abCalcSpiritStone[eStoneType])
				{
					++lNumSpiritStone;
					abCalcSpiritStone[eStoneType] = TRUE;
				}
			}
		}
	}

	return lNumSpiritStone;
}

INT32 AgpmItemConvert::GetNumConvertAttr(AgpdItem *pcsItem, INT32 lAttributeType)
{
	if (!pcsItem ||
		lAttributeType < AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC ||
		lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_ICE)
		return 0;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (pcsAttachData->m_lNumConvert < 0 ||
		pcsAttachData->m_lNumConvert > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		return 0;

	INT32	lNumAttribute	= 0;

	for (int i = 0; i < pcsAttachData->m_lNumConvert; ++i)
	{
		if (!pcsAttachData->m_stSocketAttr[i].bIsSpiritStone ||
			!pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)
			continue;

		if (((AgpdItemTemplateUsableSpiritStone *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eSpiritStoneType == lAttributeType)
			++lNumAttribute;
	}

	return lNumAttribute;
}

AgpdItemConvertSpiritStoneResult AgpmItemConvert::IsSpiritStoneConvertable(AgpdItem *pcsItem)
{
	if (!pcsItem ||
		pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_FAILED;

	if (IsEgoItem(pcsItem))
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_IS_EGO_ITEM;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP)
	{
		if (AGPMITEM_EQUIP_KIND_WEAPON == ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind)
		{
			if (AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE == ((AgpdItemTemplateEquipWeapon *) pcsItem->m_pcsItemTemplate)->m_nWeaponType)
				return AGPDITEMCONVERT_SPIRITSTONE_RESULT_FAILED;
		}
	}

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (pcsAttachData->m_lNumSocket < 1)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_IMPROPER_ITEM;
	
	if (pcsAttachData->m_lNumSocket <= pcsAttachData->m_lNumConvert)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_IS_ALREADY_FULL;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP ||
		(((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_ARMOUR &&
		 ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_WEAPON))
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_IMPROPER_ITEM;

	return AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS;
}

AgpdItemConvertSpiritStoneResult AgpmItemConvert::IsValidSpiritStone(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALIE_SPIRITSTONE;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_USABLE ||
		((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALIE_SPIRITSTONE;

	if (((AgpdItemTemplateUsableSpiritStone *) pcsItem->m_pcsItemTemplate)->m_eSpiritStoneType <= AGPMITEM_USABLE_SS_TYPE_NONE ||
		((AgpdItemTemplateUsableSpiritStone *) pcsItem->m_pcsItemTemplate)->m_eSpiritStoneType > AGPMITEM_USABLE_SS_TYPE_AIR)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALIE_SPIRITSTONE;

	if (pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALIE_SPIRITSTONE;

	return AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS;
}

AgpdItemConvertSpiritStoneResult AgpmItemConvert::SpiritStoneConvert(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, BOOL bAdmin)
{
	if (!pcsItem || !pcsSpiritStone)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_FAILED;

	AgpdItemConvertSpiritStoneResult	eCheckResult	= IsValidSpiritStone(pcsSpiritStone);
	if (eCheckResult != AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
		return eCheckResult;

	eCheckResult	= IsSpiritStoneConvertable(pcsItem);
	if (eCheckResult != AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
		return eCheckResult;

	INT32	lSpiritStoneRank	= 0;
	m_pcsAgpmFactors->GetValue(&pcsSpiritStone->m_csFactor, &lSpiritStoneRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

	if (lSpiritStoneRank < 1 || lSpiritStoneRank > 5)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALIE_SPIRITSTONE;

	INT32	lItemPhysicalRank	= GetPhysicalItemRank(pcsItem);

	if (!m_astTablePhysical[lItemPhysicalRank].bIsConvertableSpirit[lSpiritStoneRank])
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALIE_SPIRITSTONE;

	// 2005.05.26. steeple
	// Admin 일 때는 확률검사는 하지 않는다.
	if(bAdmin)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS;

	INT32	lRandomNumber	= m_csRandom.randInt(100);

	FLOAT	fSuccessProb	= 1.0f;

	PVOID pvBuffer[2];
	pvBuffer[0] = (PVOID) pcsItem;
	pvBuffer[1] = (PVOID) pcsSpiritStone;

	EnumCallback(AGPDITEMCONVERT_CB_ADJUST_SUCCESS_PROB, pvBuffer, &fSuccessProb);

	if (IsWeapon(pcsItem))
	{
		if ((INT32) (m_astTableSpiritStone[lSpiritStoneRank].lWeaponProbability * fSuccessProb) < lRandomNumber)
			return AGPDITEMCONVERT_SPIRITSTONE_RESULT_FAILED;
	}
	else
	{
		if ((INT32) (m_astTableSpiritStone[lSpiritStoneRank].lArmourProbability * fSuccessProb) < lRandomNumber)
			return AGPDITEMCONVERT_SPIRITSTONE_RESULT_FAILED;
	}

	return AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS;
}

BOOL AgpmItemConvert::InitializeSpiritStoneConvert(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

//	INT32	lAddAttrValue[AGPD_FACTORS_ATTRIBUTE_MAX_TYPE];
//
//	ZeroMemory(lAddAttrValue, sizeof(INT32) * AGPD_FACTORS_ATTRIBUTE_MAX_TYPE);

	ApSafeArray<INT32, AGPD_FACTORS_ATTRIBUTE_MAX_TYPE>	lAddAttrValue;
	lAddAttrValue.MemSetAll();

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (pcsAttachData->m_lNumConvert < 0 ||
		pcsAttachData->m_lNumConvert > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		return FALSE;

	int i = 0;
	for (i = 0; i < pcsAttachData->m_lNumConvert; ++i)
	{
		if (!pcsAttachData->m_stSocketAttr[i].bIsSpiritStone ||
			!pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)
			continue;

		INT32	lItemRank	= 0;
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

		if (lItemRank < 1 ||
			lItemRank > 5)
			continue;

		INT32	lAttributeType	= ((AgpdItemTemplateUsableSpiritStone *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eSpiritStoneType;

		if (lAttributeType < AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC ||
			lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_AIR)
			continue;

		if (IsWeapon(pcsItem))
			lAddAttrValue[lAttributeType] += m_astTableSpiritStone[lItemRank].lWeaponAddValue;
		else
			lAddAttrValue[lAttributeType] += m_astTableSpiritStone[lItemRank].lArmourAddValue;
	}

//	INT32	lNumAttribute[AGPD_FACTORS_ATTRIBUTE_MAX_TYPE];
//	ZeroMemory(lNumAttribute, sizeof(INT32) * AGPD_FACTORS_ATTRIBUTE_MAX_TYPE);

	ApSafeArray<INT32, AGPD_FACTORS_ATTRIBUTE_MAX_TYPE>	lNumAttribute;
	lNumAttribute.MemSetAll();

	for (i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_AIR; ++i)
	{
		lNumAttribute[i]	= GetNumConvertAttr(pcsItem, i);

		if (lNumAttribute[i] > 1)
		{
			if (IsWeapon(pcsItem))
				lAddAttrValue[i] += (lNumAttribute[i] - 1) * m_stTableSameAttrBonus.lWeaponBonus;
			else
				lAddAttrValue[i] += (lNumAttribute[i] - 1) * m_stTableSameAttrBonus.lArmourBonus;
		}
	}

	AgpdFactor	csUpdateFactor;
	m_pcsAgpmFactors->InitFactor(&csUpdateFactor);

	if (IsWeapon(pcsItem))
	{
		if (!m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DAMAGE))
		{
			m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
			return FALSE;
		}

		for (i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_AIR; ++i)
		{
			m_pcsAgpmFactors->SetValue(&csUpdateFactor, -lAddAttrValue[i], AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, i);
			m_pcsAgpmFactors->SetValue(&csUpdateFactor, -lAddAttrValue[i], AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, i);
		}
	}
	else
	{
		if (!m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DEFENSE))
		{
			m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
			return FALSE;
		}

		for (i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_AIR; ++i)
		{
			m_pcsAgpmFactors->SetValue(&csUpdateFactor, -lAddAttrValue[i], AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, i);
		}
	}

	PVOID	pvPacket	= m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactor, &csUpdateFactor, TRUE, TRUE, TRUE);

	m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);

	PVOID	pvBuffer[2];
	pvBuffer[0]		= pvPacket;
	pvBuffer[1]		= NULL;

	EnumCallback(AGPDITEMCONVERT_CB_SEND_UPDATE_FACTOR, pcsItem, pvBuffer);

	m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacket);

	for (i = 0; i < pcsAttachData->m_lNumConvert; ++i)
	{
		if (!pcsAttachData->m_stSocketAttr[i].bIsSpiritStone ||
			!pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)
			continue;

		pcsAttachData->m_stSocketAttr[i].lTID				= AP_INVALID_IID;
		pcsAttachData->m_stSocketAttr[i].pcsItemTemplate	= NULL;
	}

	return TRUE;
}

BOOL AgpmItemConvert::AddSpiritStoneConvert(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate || !pcsSpiritStone || !pcsSpiritStone->m_pcsItemTemplate)
		return FALSE;

	return AddSpiritStoneConvert(pcsItem, (AgpdItemTemplate *) pcsSpiritStone->m_pcsItemTemplate);
}

BOOL AgpmItemConvert::AddSpiritStoneConvert(AgpdItem *pcsItem, AgpdItemTemplate *pcsItemTemplate)
{
	INT32	lNumAttribute		= GetNumConvertAttr(pcsItem, ((AgpdItemTemplateUsableSpiritStone *) pcsItemTemplate)->m_eSpiritStoneType);

	INT32	lSpiritStoneRank	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItemTemplate->m_csFactor, &lSpiritStoneRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

	if (lSpiritStoneRank < 1 ||
		lSpiritStoneRank > 5)
		return FALSE;

	INT32	lAddValue			= 0;
	if (IsWeapon(pcsItem))
	{
		lAddValue			= m_astTableSpiritStone[lSpiritStoneRank].lWeaponAddValue;

		if (lNumAttribute >= 1)
			lAddValue += m_stTableSameAttrBonus.lWeaponBonus;
	}
	else
	{
		lAddValue			= m_astTableSpiritStone[lSpiritStoneRank].lArmourAddValue;

		if (lNumAttribute >= 1)
			lAddValue += m_stTableSameAttrBonus.lArmourBonus;
	}

	AgpdFactor	csUpdateFactor;
	m_pcsAgpmFactors->InitFactor(&csUpdateFactor);

	if (IsWeapon(pcsItem))
	{
		if (!m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DAMAGE))
		{
			m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
			return FALSE;
		}

		m_pcsAgpmFactors->SetValue(&csUpdateFactor, lAddValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, ((AgpdItemTemplateUsableSpiritStone *) pcsItemTemplate)->m_eSpiritStoneType);
		m_pcsAgpmFactors->SetValue(&csUpdateFactor, lAddValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, ((AgpdItemTemplateUsableSpiritStone *) pcsItemTemplate)->m_eSpiritStoneType);
	}
	else
	{
		if (!m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DEFENSE))
		{
			m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
			return FALSE;
		}

		m_pcsAgpmFactors->SetValue(&csUpdateFactor, lAddValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, ((AgpdItemTemplateUsableSpiritStone *) pcsItemTemplate)->m_eSpiritStoneType);
	}

	PVOID	pvPacket	= m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactor, &csUpdateFactor, TRUE, TRUE, TRUE);

	m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (pcsAttachData->m_lNumConvert >= 0 &&
		pcsAttachData->m_lNumConvert <= AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
	{
		pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert].bIsSpiritStone	= TRUE;
		pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert].lTID			= pcsItemTemplate->m_lID;
		pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert].pcsItemTemplate	= pcsItemTemplate;

		++pcsAttachData->m_lNumConvert;
	}

	PVOID	pvBuffer[2];
	pvBuffer[0]		= pvPacket;
	pvBuffer[1]		= NULL;

	EnumCallback(AGPDITEMCONVERT_CB_SEND_UPDATE_FACTOR, pcsItem, pvBuffer);

	m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgpmItemConvert::CalcSpiritStoneConvertFactor(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	if (pcsAttachData->m_lNumConvert <= 0 ||
		pcsAttachData->m_lNumConvert > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		return TRUE;

//	INT32	lAddValue[AGPD_FACTORS_ATTRIBUTE_MAX_TYPE];
//	INT32	lNumAttribute[AGPD_FACTORS_ATTRIBUTE_MAX_TYPE];
//
//	ZeroMemory(lAddValue, sizeof(INT32) * AGPD_FACTORS_ATTRIBUTE_MAX_TYPE);
//	ZeroMemory(lNumAttribute, sizeof(INT32) * AGPD_FACTORS_ATTRIBUTE_MAX_TYPE);

	ApSafeArray<INT32, AGPD_FACTORS_ATTRIBUTE_MAX_TYPE>	lAddValue;
	ApSafeArray<INT32, AGPD_FACTORS_ATTRIBUTE_MAX_TYPE>	lNumAttribute;

	lAddValue.MemSetAll();
	lNumAttribute.MemSetAll();

	BOOL	bIsWeapon	= IsWeapon(pcsItem);

	INT32	lTotalAddValue	= 0;
	INT32	i = 0;

	for (i = 0; i < pcsAttachData->m_lNumConvert; ++i)
	{
		if (!pcsAttachData->m_stSocketAttr[i].bIsSpiritStone ||
			!pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)
			continue;

		if (((AgpdItemTemplateUsableSpiritStone *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eSpiritStoneType < AGPMITEM_USABLE_SS_TYPE_MAGIC ||
			((AgpdItemTemplateUsableSpiritStone *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eSpiritStoneType > AGPMITEM_USABLE_SS_TYPE_AIR)
			continue;

		++lNumAttribute[((AgpdItemTemplateUsableSpiritStone *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eSpiritStoneType];

		INT32	lSpiritStoneRank	= 0;
		m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lSpiritStoneRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

		if (bIsWeapon)
		{
			lAddValue[((AgpdItemTemplateUsableSpiritStone *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eSpiritStoneType]			+= m_astTableSpiritStone[lSpiritStoneRank].lWeaponAddValue;
			lTotalAddValue += m_astTableSpiritStone[lSpiritStoneRank].lWeaponAddValue;
		}
		else
		{
			lAddValue[((AgpdItemTemplateUsableSpiritStone *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eSpiritStoneType]			+= m_astTableSpiritStone[lSpiritStoneRank].lArmourAddValue;
			lTotalAddValue += m_astTableSpiritStone[lSpiritStoneRank].lArmourAddValue;
		}
	}

	if (lTotalAddValue == 0)
		return TRUE;

	for (i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
	{
		if (lNumAttribute[i] > 1)
		{
			if (bIsWeapon)
				lAddValue[i] += m_stTableSameAttrBonus.lWeaponBonus * (lNumAttribute[i] - 1);
			else
				lAddValue[i] += m_stTableSameAttrBonus.lArmourBonus * (lNumAttribute[i] - 1);
		}
	}

	AgpdFactor	csUpdateFactor;
	m_pcsAgpmFactors->InitFactor(&csUpdateFactor);

	if (IsWeapon(pcsItem))
	{
		if (!m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DAMAGE))
		{
			m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
			return FALSE;
		}

		for (i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
		{
			if (lAddValue[i] > 0)
			{
				m_pcsAgpmFactors->SetValue(&csUpdateFactor, lAddValue[i], AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, i);
				m_pcsAgpmFactors->SetValue(&csUpdateFactor, lAddValue[i], AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, i);
			}
		}
	}
	else
	{
		if (!m_pcsAgpmFactors->InitCalcFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_DEFENSE))
		{
			m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);
			return FALSE;
		}

		for (i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
		{
			if (lAddValue[i] > 0)
			{
				m_pcsAgpmFactors->SetValue(&csUpdateFactor, lAddValue[i], AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, i);
			}
		}
	}

	m_pcsAgpmFactors->CalcFactor(&pcsItem->m_csFactor, &csUpdateFactor, TRUE, FALSE, TRUE);

	m_pcsAgpmFactors->DestroyFactor(&csUpdateFactor);

	return TRUE;
}

AgpdItemConvertSpiritStoneResult AgpmItemConvert::ProcessSpiritStoneConvert(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone)
{
	if (!pcsItem || !pcsSpiritStone)
		return AGPDITEMCONVERT_SPIRITSTONE_RESULT_FAILED;

	AgpdItemConvertSpiritStoneResult	eResult	= SpiritStoneConvert(pcsItem, pcsSpiritStone);

	switch (eResult) {
	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS:
		{
			AddSpiritStoneConvert(pcsItem, pcsSpiritStone);
		}
		break;
	}

	EnumCallback(AGPDITEMCONVERT_CB_PROCESS_SPIRITSTONE_CONVERT, pcsItem, &eResult);

	if (eResult != AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALIE_SPIRITSTONE)
	{
		/*
		ZeroMemory(pcsSpiritStone->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsSpiritStone->m_szDeleteReason, "정령석개조로인한 아이템 삭제", AGPMITEM_MAX_DELETE_REASON);
		m_pcsAgpmItem->RemoveItem(pcsSpiritStone->m_lID, TRUE);
		*/
		m_pcsAgpmItem->SubItemStackCount(pcsSpiritStone, 1);
	}

	return eResult;
}

BOOL AgpmItemConvert::OnOperationRequestSpiritStoneConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lSpiritStoneID)
{
	if (!pcsCharacter || !pcsItem || lSpiritStoneID == AP_INVALID_IID)
		return FALSE;

	if (pcsCharacter != pcsItem->m_pcsCharacter)
		return FALSE;

	ProcessSpiritStoneConvert(pcsItem, m_pcsAgpmItem->GetItem(lSpiritStoneID));

	return TRUE;
}

BOOL AgpmItemConvert::OnOperationResponseSpiritStoneConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT32 lTID)
{
	if (!pcsCharacter || !pcsItem || cActionResult == (-1) || cNumConvertedSocket == (-1))
		return FALSE;

	if (pcsCharacter != pcsItem->m_pcsCharacter)
		return FALSE;

	AgpdItemConvertSpiritStoneResult	eResult	= (AgpdItemConvertSpiritStoneResult) cActionResult;

	BOOL	bIsUpdated	= FALSE;

	switch (eResult) {
	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS:
		{
//			AddSpiritStoneConvert(pcsItem, m_pcsAgpmItem->GetItemTemplate(lTID));

			bIsUpdated	= TRUE;
		}
		break;
	}

	if (eResult == AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
	{
		AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

		pcsAttachData->m_lPhysicalConvertLevel	= (INT32) cNumPhysicalConvert;
		pcsAttachData->m_lNumSocket				= (INT32) cNumSocket;
		pcsAttachData->m_lNumConvert			= (INT32) cNumConvertedSocket;

		if (pcsAttachData->m_lNumConvert > 0 && pcsAttachData->m_lNumConvert <= AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		{
			pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert - 1].bIsSpiritStone	= TRUE;
			pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert - 1].lTID			= lTID;
			pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert - 1].pcsItemTemplate	= m_pcsAgpmItem->GetItemTemplate(lTID);
		}
	}

	if (bIsUpdated)
		EnumCallback(AGPDITEMCONVERT_CB_UPDATE, pcsItem, NULL);

	EnumCallback(AGPDITEMCONVERT_CB_RESULT_SPIRITSTONE_CONVERT, pcsItem, &eResult);

	return TRUE;
}

BOOL AgpmItemConvert::OnOperationResponseSpiritStoneCheckResult(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lSpiritStoneID, INT8 cActionResult)
{
	if (!pcsCharacter || !pcsItem || lSpiritStoneID == AP_INVALID_IID)
		return FALSE;

	AgpdItemConvertSpiritStoneResult	eResult	= (AgpdItemConvertSpiritStoneResult) cActionResult;

	if (eResult == AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
	{
		AgpdItem	*pcsSpiritStone	= m_pcsAgpmItem->GetItem(lSpiritStoneID);

		EnumCallback(AGPDITEMCONVERT_CB_ASK_REALLY_SPIRITSTONE_CONVERT, pcsItem, pcsSpiritStone);
	}
	else
	{
		EnumCallback(AGPDITEMCONVERT_CB_RESULT_SPIRITSTONE_CONVERT, pcsItem, &eResult);
	}

	return TRUE;
}

BOOL AgpmItemConvert::CBRequestSpiritStoneConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmItemConvert		*pThis			= (AgpmItemConvert *)	pClass;
	AgpdItem			*pcsItem		= (AgpdItem *)			pData;
	AgpdItem			*pcsSpiritStone	= (AgpdItem *)			pCustData;

	if (!pcsItem->m_pcsCharacter ||
		pcsItem->m_pcsCharacter != pcsSpiritStone->m_pcsCharacter)
		return FALSE;

	AgpdItemConvertSpiritStoneResult	eResult	= pThis->IsValidSpiritStone(pcsSpiritStone);
	if (eResult != AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= (PVOID) pcsSpiritStone;
		pvBuffer[1]	= (PVOID) eResult;

		pThis->EnumCallback(AGPDITEMCONVERT_CB_SPIRITSTONE_CONVERT_CHECK_RESULT, pcsItem, pvBuffer);

		return TRUE;
	}

	eResult	= pThis->IsSpiritStoneConvertable(pcsItem);
	if (eResult != AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= (PVOID) pcsSpiritStone;
		pvBuffer[1]	= (PVOID) eResult;

		pThis->EnumCallback(AGPDITEMCONVERT_CB_SPIRITSTONE_CONVERT_CHECK_RESULT, pcsItem, pvBuffer);

		return TRUE;
	}

	INT32	lSpiritStoneRank	= 0;
	pThis->m_pcsAgpmFactors->GetValue(&pcsSpiritStone->m_csFactor, &lSpiritStoneRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

	INT32	lItemPhysicalRank	= pThis->GetPhysicalItemRank(pcsItem);

	if (lSpiritStoneRank < 1 || lSpiritStoneRank > 5 ||
		!pThis->m_astTablePhysical[lItemPhysicalRank].bIsConvertableSpirit[lSpiritStoneRank])
	{
		eResult	= AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALID_RANK;

		PVOID	pvBuffer[2];
		pvBuffer[0]	= (PVOID) pcsSpiritStone;
		pvBuffer[1]	= (PVOID) eResult;

		pThis->EnumCallback(AGPDITEMCONVERT_CB_SPIRITSTONE_CONVERT_CHECK_RESULT, pcsItem, pvBuffer);

		return TRUE;
	}

	eResult	= AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS;

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID) pcsSpiritStone;
	pvBuffer[1]	= (PVOID) eResult;

	pThis->EnumCallback(AGPDITEMCONVERT_CB_SPIRITSTONE_CONVERT_CHECK_RESULT, pcsItem, pvBuffer);

	return TRUE;
}