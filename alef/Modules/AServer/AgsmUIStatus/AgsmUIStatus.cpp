#include "AgsmUIStatus.h"

AgsmUIStatus::AgsmUIStatus()
{
	SetModuleName("AgsmUIStatus");
}

AgsmUIStatus::~AgsmUIStatus()
{
}

BOOL AgsmUIStatus::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)				GetModule("AgpmItem");
	m_pcsAgpmSkill			= (AgpmSkill *)				GetModule("AgpmSkill");
	m_pcsAgpmUIStatus		= (AgpmUIStatus *)			GetModule("AgpmUIStatus");
	m_pcsAgpmEventSkillMaster	= (AgpmEventSkillMaster *)	GetModule("AgpmEventSkillMaster");

	m_pcsAgsmAOIFilter		= (AgsmAOIFilter *)			GetModule("AgsmAOIFilter");
	m_pcsAgsmCharacter		= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pcsAgsmCharManager	= (AgsmCharManager *)		GetModule("AgsmCharManager");
	m_pcsAgsmItem			= (AgsmItem *)				GetModule("AgsmItem");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmSkill ||
		!m_pcsAgpmEventSkillMaster ||
		!m_pcsAgpmUIStatus ||

		!m_pcsAgsmAOIFilter ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmCharManager ||
		!m_pcsAgsmItem)
		return FALSE;

	if (!m_pcsAgpmUIStatus->SetCallbackReceivedEncodedQBeltString(CBReceiveEncodingString, this))
		return FALSE;
	if (!m_pcsAgpmUIStatus->SetCallbackUpdateViewHelmetOption(CBUpdateViewHelmetOption, this))
		return FALSE;

	if (!m_pcsAgsmCharacter->SetCallbackEnterGameworld(CBEnterGameWorld, this))
		return FALSE;
	if (!m_pcsAgsmCharacter->SetCallbackAllUpdate(CBUpdateAllToDB, this))
		return FALSE;
	if (!m_pcsAgsmCharacter->SetCallbackSendCharacterAllServerInfo(CBSendCharacterAllServerInfo, this))
		return FALSE;

	if (!m_pcsAgsmCharManager->SetCallbackSetCharacterGameData(CBSetCharacterGameData, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmUIStatus::OnInit()
{
	return TRUE;
}

BOOL AgsmUIStatus::OnDestroy()
{
	return TRUE;
}

BOOL AgsmUIStatus::CBReceiveEncodingString(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmUIStatus		*pThis				= (AgsmUIStatus *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	PVOID				*ppvBuffer			= (PVOID *)				pCustData;

	CHAR				*szEncodedString	= (CHAR *)				ppvBuffer[0];
	INT32				lStringLength		= PtrToInt(ppvBuffer[1]);
	CHAR				*szCooldown			= (CHAR *)				ppvBuffer[2];
	INT32				lCooldownLength		= PtrToInt(ppvBuffer[3]);

	//if (!szEncodedString || lStringLength < 1)
	//	return FALSE;

	pThis->m_pcsAgpmUIStatus->SetQBeltEncodedString(pcsCharacter, szEncodedString, lStringLength);
	pThis->m_pcsAgpmUIStatus->SetCooldownEncodedString(pcsCharacter, szCooldown, lCooldownLength);

	return TRUE;
}

BOOL AgsmUIStatus::CBUpdateViewHelmetOption(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmUIStatus		*pThis				= (AgsmUIStatus *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	INT8				*pnOptionViewHelmet	= (INT8 *)				pCustData;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmUIStatus->MakePacketUpdateViewHelmetOption(pcsCharacter->m_lID, &nPacketLength, *pnOptionViewHelmet);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->m_pcsAgsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_4);

	pThis->m_pcsAgpmUIStatus->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmUIStatus::DecodingQBeltString(AgpdCharacter *pcsCharacter, CHAR *szString, INT32 lStringLength)
{
	if (!pcsCharacter || !szString || !szString[0] || lStringLength < 1)
		return FALSE;

	AgpdUIStatusADChar	*pcsAttachData		= m_pcsAgpmUIStatus->GetADCharacter(pcsCharacter);

	INT32	lDecodedStringIndex				= 0;

	INT32	lType							= 0;
	UINT64	ullID							= 0;

	INT32	lIndex							= 0;

	while (lIndex <= lStringLength)
	{
		if (szString[lIndex++] == ';')
			break;
	}

	sscanf(szString, "%d,%d;", &pcsAttachData->m_lHPPotionTID, &pcsAttachData->m_lMPPotionTID);

	lDecodedStringIndex	= lIndex;

	for (int i = 0; i < AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN; ++i)
	{
		lType	= 0;
		ullID	= 0;

		while (lIndex <= lStringLength)
		{
			if (szString[lIndex++] == ':')
				break;
		}

		sscanf(szString + lDecodedStringIndex, "%d,%I64d:", &lType, &ullID);

		lDecodedStringIndex = lIndex;

		pcsAttachData->m_acsQBeltItem[i].m_eType	= APBASE_TYPE_NONE;
		pcsAttachData->m_acsQBeltItem[i].m_lID		= 0;

		switch (lType) {
		case 1:
			{
				AgpdItem	*pcsItem	= m_pcsAgsmItem->GetItemByDBID(pcsCharacter, ullID);
				if (pcsItem &&
					(pcsItem->m_eStatus == AGPDITEM_STATUS_INVENTORY ||
					 pcsItem->m_eStatus == AGPDITEM_STATUS_CASH_INVENTORY))
				{
					pcsAttachData->m_acsQBeltItem[i].m_eType	= APBASE_TYPE_ITEM;
					pcsAttachData->m_acsQBeltItem[i].m_lID		= pcsItem->m_lID;
				}
				else
				{
					//AuLogFile("QBErr.txt", "DecodingQBeltString::GetItemByDBID() return NULL. TID(%I64d)\n", ullID);
				}
			}
			break;

		case 2:
			{
				AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->GetSkillByTID(pcsCharacter, (INT32) ullID);
				if (pcsSkill)
				{
					pcsAttachData->m_acsQBeltItem[i].m_eType	= APBASE_TYPE_SKILL;
					pcsAttachData->m_acsQBeltItem[i].m_lID		= pcsSkill->m_lID;
				}
				else
				{
					//AuLogFile("QBErr.txt", "DecodingQBeltString::GetSkillByTID() return NULL. TID(%d)\n", ullID);
				}
			}
			break;
		}
	}

	INT32	lAutoUseHPGage	= 0;
	INT32	lAutoUseMPGage	= 0;

	sscanf(szString + lDecodedStringIndex, "%d,%d;", &lAutoUseHPGage, &lAutoUseMPGage);

	pcsAttachData->m_cAutoUseHPGage	= (INT8) lAutoUseHPGage;
	pcsAttachData->m_cAutoUseMPGage	= (INT8) lAutoUseMPGage;

	return TRUE;
}

BOOL AgsmUIStatus::EncodingQBeltString(AgpdCharacter *pcsCharacter, CHAR *szStringBuffer, INT32 lStringBufferLength)
{
	if (!pcsCharacter || !szStringBuffer || lStringBufferLength < 1)
		return FALSE;

	ZeroMemory(szStringBuffer, sizeof(CHAR) * lStringBufferLength);

	AgpdUIStatusADChar	*pcsAttachData		= m_pcsAgpmUIStatus->GetADCharacter(pcsCharacter);

	CHAR	szBuffer[64];
	INT32	lEncodedStringLength	= 0;
	
	ZeroMemory(szBuffer, sizeof(CHAR) * 64);

	sprintf(szBuffer, "%d,%d;", pcsAttachData->m_lHPPotionTID, pcsAttachData->m_lMPPotionTID);
	ASSERT(strlen(szBuffer) < 64);

	CopyMemory(szStringBuffer, szBuffer, sizeof(CHAR) * strlen(szBuffer));
	lEncodedStringLength = (INT32)strlen(szBuffer);

	for (int i = 0; i < AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN; ++i)
	{
		ZeroMemory(szBuffer, sizeof(CHAR) * 64);

		BOOL	bIsEncodingItem	= FALSE;

		switch (pcsAttachData->m_acsQBeltItem[i].m_eType) {
		case APBASE_TYPE_ITEM:
			{
				AgpdItem	*pcsItem	= m_pcsAgpmItem->GetItem(pcsAttachData->m_acsQBeltItem[i].m_lID);
				if (pcsItem)
				{
					AgsdItem	*pcsAgsdItem	= m_pcsAgsmItem->GetADItem(pcsItem);

					sprintf(szBuffer, "1,%I64d:", pcsAgsdItem->m_ullDBIID);
					ASSERT(strlen(szBuffer) < 64);

					bIsEncodingItem	= TRUE;
				}
				else
				{
					//AuLogFile("QBErr.txt", "EncodingQBeltString::GetItem() return NULL. ID(%d)\n", pcsAttachData->m_acsQBeltItem[i].m_lID);
				}
			}
			break;

		case APBASE_TYPE_SKILL:
			{
				AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->GetSkill(pcsAttachData->m_acsQBeltItem[i].m_lID);
				if (pcsSkill && pcsSkill->m_pcsTemplate)
				{
					sprintf(szBuffer, "2,%d:", pcsSkill->m_pcsTemplate->m_lID);
					ASSERT(strlen(szBuffer) < 64);

					bIsEncodingItem	= TRUE;
				}
				else
				{
					//AuLogFile("QBErr.txt", "EncodingQBeltString::GetSkill() return NULL. ID(%d)\n", pcsAttachData->m_acsQBeltItem[i].m_lID);
				}
			}
			break;

		default:
			break;
		}

		if (!bIsEncodingItem)
		{
			sprintf(szBuffer, "0,0:");
			ASSERT(strlen(szBuffer) < 64);
		}

		if (lEncodedStringLength + (INT32)strlen(szBuffer) > lStringBufferLength)
		{
			return FALSE;
		}
		else
		{
			CopyMemory(szStringBuffer + lEncodedStringLength, szBuffer, sizeof(CHAR) * strlen(szBuffer));
			lEncodedStringLength += (INT32)strlen(szBuffer);
		}
	}

	ZeroMemory(szBuffer, sizeof(CHAR) * 64);

	sprintf(szBuffer, "%d,%d;", pcsAttachData->m_cAutoUseHPGage, pcsAttachData->m_cAutoUseMPGage);
	ASSERT(strlen(szBuffer) < 64);

	CopyMemory(szStringBuffer + lEncodedStringLength, szBuffer, sizeof(CHAR) * strlen(szBuffer));
	lEncodedStringLength += (INT32)strlen(szBuffer);

	ASSERT((INT32)strlen(szStringBuffer) < lStringBufferLength);

	return TRUE;
}

BOOL AgsmUIStatus::EncodingDefaultQBeltString(AgpdCharacter *pcsCharacter, CHAR *szStringBuffer, INT32 lStringBufferLength)
{
	if (!pcsCharacter || !pcsCharacter->m_pcsCharacterTemplate || !szStringBuffer || lStringBufferLength < 1)
		return FALSE;

	ZeroMemory(szStringBuffer, sizeof(CHAR) * lStringBufferLength);

	AgpdUIStatusADChar	*pcsAttachData		= m_pcsAgpmUIStatus->GetADCharacter(pcsCharacter);

	CHAR	szBuffer[64];
	INT32	lEncodedStringLength	= 0;
	
	ZeroMemory(szBuffer, sizeof(CHAR) * 64);

	sprintf(szBuffer, "%d,%d;", 190, 193);
	ASSERT(strlen(szBuffer) < 64);

	CopyMemory(szStringBuffer, szBuffer, sizeof(CHAR) * strlen(szBuffer));
	lEncodedStringLength = (INT32)strlen(szBuffer);

	AgpdEventSkillAttachCharTemplateData	*pcsAttachCharTemplateData	= m_pcsAgpmEventSkillMaster->GetSkillAttachCharTemplateData(pcsCharacter->m_pcsCharacterTemplate);

	for (int i = 0; i < AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN; ++i)
	{
		ZeroMemory(szBuffer, sizeof(CHAR) * 64);

		BOOL	bIsEncodingItem	= FALSE;

		if (i < pcsAttachCharTemplateData->m_lNumDefaultSkill && pcsAttachCharTemplateData->m_apcsDefaultSkillTemplate[i])
		{
			sprintf(szBuffer, "2,%d:", pcsAttachCharTemplateData->m_apcsDefaultSkillTemplate[i]->m_lID);
			ASSERT(strlen(szBuffer) < 64);

			bIsEncodingItem	= TRUE;
		}

		if (!bIsEncodingItem)
		{
			sprintf(szBuffer, "0,0:");
			ASSERT(strlen(szBuffer) < 64);
		}

		if (lEncodedStringLength + (INT32)strlen(szBuffer) > lStringBufferLength)
		{
			return FALSE;
		}
		else
		{
			CopyMemory(szStringBuffer + lEncodedStringLength, szBuffer, sizeof(CHAR) * strlen(szBuffer));
			lEncodedStringLength += (INT32)strlen(szBuffer);
		}
	}

	ZeroMemory(szBuffer, sizeof(CHAR) * 64);

	sprintf(szBuffer, "0,0;");
	ASSERT(strlen(szBuffer) < 64);

	CopyMemory(szStringBuffer + lEncodedStringLength, szBuffer, sizeof(CHAR) * strlen(szBuffer));
	lEncodedStringLength += (INT32)strlen(szBuffer);

	ASSERT((INT32)strlen(szStringBuffer) < lStringBufferLength);

	return TRUE;
}

BOOL AgsmUIStatus::SendQBeltString(AgpdCharacter *pcsCharacter, CHAR *szString, INT32 lStringLength, CHAR* szCooldown, INT32 lCooldownLength, UINT32 ulNID)
{
	if (!pcsCharacter ||
		ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmUIStatus->MakePacketQBeltString(pcsCharacter, szString, lStringLength, szCooldown, lCooldownLength, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmUIStatus->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmUIStatus::SendQBeltString(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	AgpdUIStatusADChar	*pcsAttachData		= m_pcsAgpmUIStatus->GetADCharacter(pcsCharacter);

	if (!pcsAttachData->pszQBeltEncodedString && !pcsAttachData->pszCooldown)
		return FALSE;

	return SendQBeltString(pcsCharacter,
							pcsAttachData->pszQBeltEncodedString, pcsAttachData->pszQBeltEncodedString ? (INT32)strlen(pcsAttachData->pszQBeltEncodedString) : 0,
							pcsAttachData->pszCooldown, pcsAttachData->pszCooldown ? (INT32)strlen(pcsAttachData->pszCooldown) : 0,
							ulNID);
}

BOOL AgsmUIStatus::SendWholeQBeltInfo(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmUIStatus->MakePacketWholeQuickBeltInfo(pcsCharacter, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmUIStatus->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmUIStatus::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmUIStatus		*pThis			 = (AgsmUIStatus *)		pClass;
	AgpdCharacter		*pcsCharacter	 = (AgpdCharacter *)	pData;

	UINT32				ulDPNID				= pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter);

	if (pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter) && ulDPNID != 0)
	{
		pThis->SendWholeQBeltInfo(pcsCharacter, ulDPNID);
	}

	return TRUE;
}

BOOL AgsmUIStatus::CBUpdateAllToDB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmUIStatus		*pThis			= (AgsmUIStatus *)		pClass;

	return pThis->EnumCallback(AGSMUISTATUS_CB_UPDATE_DATA_TO_DB, pData, NULL);
}

BOOL AgsmUIStatus::CBSendCharacterAllServerInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmUIStatus		*pThis			= (AgsmUIStatus *)		pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;
	UINT32				*aulBuffer		= (UINT32 *)			pCustData;

	pThis->SendQBeltString(pcsCharacter, aulBuffer[0]);

	return TRUE;
}

BOOL AgsmUIStatus::CBSetCharacterGameData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmUIStatus	*pThis			= (AgsmUIStatus *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdUIStatusADChar	*pcsAttachData		= pThis->m_pcsAgpmUIStatus->GetADCharacter(pcsCharacter);

	if(pcsAttachData->pszQBeltEncodedString)
		pThis->DecodingQBeltString(pcsCharacter, pcsAttachData->pszQBeltEncodedString, (INT32) strlen(pcsAttachData->pszQBeltEncodedString));

	if(pcsAttachData->pszCooldown)
		pThis->m_pcsAgsmItem->DecodingCooldown(pcsCharacter, pcsAttachData->pszCooldown, (INT32) strlen(pcsAttachData->pszCooldown));

	return TRUE;
}

BOOL AgsmUIStatus::SetCallbackUpdateDataToDB(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMUISTATUS_CB_UPDATE_DATA_TO_DB, pfCallback, pClass);
}