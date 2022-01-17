#include "AgpmUIStatus.h"
#include "ApMemoryTracker.h"

AgpmUIStatus::AgpmUIStatus()
{
	SetModuleName("AgpmUIStatus");
	
	SetPacketType(AGPMUISTATUS_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,			// operation
							AUTYPE_INT32,			1,			// character id

							// whole QBelt Item Information
							AUTYPE_PACKET,			AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN,	// Quick Belt Grid Base

							// update QBelt Item
							AUTYPE_INT8,			1,			// QBelt Index
							AUTYPE_PACKET,			1,			// Updated Item Packet
							AUTYPE_MEMORY_BLOCK,	1,			// Encoded QBelt Information (Type String)
							AUTYPE_INT32,			1,			// HP Potion TID
							AUTYPE_INT32,			1,			// MP Potion TID

							AUTYPE_INT8,			1,			// view helmet option value

							AUTYPE_INT8,			1,			// auto use hp gage
							AUTYPE_INT8,			1,			// auto use mp gage

							AUTYPE_MEMORY_BLOCK,	1,			// Encoded Cooldown (Type String)
							AUTYPE_END,				0
							);
}

AgpmUIStatus::~AgpmUIStatus()
{
}

BOOL AgpmUIStatus::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)				GetModule("AgpmItem");
	m_pcsAgpmSkill			= (AgpmSkill *)				GetModule("AgpmSkill");
	m_pcsApmEventManager	= (ApmEventManager *)		GetModule("ApmEventManager");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmSkill ||
		!m_pcsApmEventManager)
		return FALSE;

	m_nIndexADCharacter		= m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdUIStatusADChar), ConAgpdUIStatusADChar, DesAgpdUIStatusADChar);
	if (m_nIndexADCharacter < 0)
		return FALSE;

	return TRUE;
}

BOOL AgpmUIStatus::OnInit()
{
	return TRUE;
}

BOOL AgpmUIStatus::OnDestroy()
{
	return TRUE;
}

BOOL AgpmUIStatus::ConAgpdUIStatusADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmUIStatus		*pThis				= (AgpmUIStatus *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	AgpdUIStatusADChar	*pcsAttachData		= pThis->GetADCharacter(pcsCharacter);

	//ZeroMemory(pcsAttachData->m_acsQBeltItem, sizeof(ApBaseExLock) * AGPMUISTATUS_MAX_QUICKBELT_COLUMN * AGPMUISTATUS_MAX_QUICKBELT_LAYER);
	pcsAttachData->m_acsQBeltItem.MemSetAll();

	pcsAttachData->pszQBeltEncodedString	= NULL;

	pcsAttachData->m_lHPPotionTID			= AP_INVALID_IID;
	pcsAttachData->m_lMPPotionTID			= AP_INVALID_IID;

	pcsAttachData->m_cOptionViewHelmet		= TRUE;

	pcsAttachData->m_cAutoUseHPGage			= 0;
	pcsAttachData->m_cAutoUseMPGage			= 0;

	pcsAttachData->pszCooldown				= NULL;

	return TRUE;
}

BOOL AgpmUIStatus::DesAgpdUIStatusADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmUIStatus		*pThis				= (AgpmUIStatus *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	AgpdUIStatusADChar	*pcsAttachData		= pThis->GetADCharacter(pcsCharacter);

	if (pcsAttachData->pszQBeltEncodedString)
	{
		delete [] pcsAttachData->pszQBeltEncodedString;
		pcsAttachData->pszQBeltEncodedString	= NULL;
	}

	if (pcsAttachData->pszCooldown)
	{
		delete [] pcsAttachData->pszCooldown;
		pcsAttachData->pszCooldown = NULL;
	}

	return TRUE;
}

AgpdUIStatusADChar* AgpmUIStatus::GetADCharacter(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	return (AgpdUIStatusADChar *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, (PVOID) pcsCharacter);
}

PVOID AgpmUIStatus::MakePacketWholeQuickBeltInfo(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	ApSafeArray<PVOID, AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN>	pvPacketQBeltItem;
	pvPacketQBeltItem.MemSetAll();

	AgpdUIStatusADChar		*pcsAttachData	= GetADCharacter(pcsCharacter);

	for (int i = 0; i < AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN; ++i)
	{
		pvPacketQBeltItem[i]	= m_pcsAgpmSkill->MakeBasePacket((ApBase *) &pcsAttachData->m_acsQBeltItem[i]);
	}

	INT8	cOperation	= AGPMUISTATUS_PACKET_OPERATION_ADD;

	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMUISTATUS_PACKET_TYPE,
												&cOperation,
												&pcsCharacter->m_lID,
												&pvPacketQBeltItem[0],
												NULL,
												NULL,
												NULL,
												(pcsAttachData->m_lHPPotionTID != AP_INVALID_IID) ? &pcsAttachData->m_lHPPotionTID : NULL,
												(pcsAttachData->m_lMPPotionTID != AP_INVALID_IID) ? &pcsAttachData->m_lMPPotionTID : NULL,
												NULL,
												&pcsAttachData->m_cAutoUseHPGage,
												&pcsAttachData->m_cAutoUseMPGage,
												NULL);

	for (int i = 0; i < AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN; ++i)
	{
		if (pvPacketQBeltItem[i])
			m_csPacket.FreePacket(pvPacketQBeltItem[i]);
	}

	return pvPacket;
}

PVOID AgpmUIStatus::MakePacketUpdateQuickBeltItem(AgpdCharacter *pcsCharacter, INT32 lIndex, ApBase *pcsBaseItem, INT16 *pnPacketLength, INT32 lHPPotionTID, INT32 lMPPotionTID, INT8 cAutoUseHPGage, INT8 cAutoUseMPGage)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	if (!pcsBaseItem && lIndex < 0 &&
		lHPPotionTID == AP_INVALID_IID &&
		lMPPotionTID == AP_INVALID_IID &&
		cAutoUseHPGage == (-1) &&
		cAutoUseMPGage == (-1))
		return NULL;

	PVOID	pvPacketItem	= NULL;

	if (pcsBaseItem)
	{
		if (lIndex < 0 || lIndex >= AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN)
			return NULL;

		if (pcsBaseItem->m_eType != APBASE_TYPE_NONE &&
			pcsBaseItem->m_lID != 0)
			pvPacketItem	= m_pcsAgpmSkill->MakeBasePacket(pcsBaseItem);
	}

	INT8	cOperation		= AGPMUISTATUS_PACKET_OPERATION_UPDATE_ITEM;
	INT8	cIndex			= (INT8) lIndex;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMUISTATUS_PACKET_TYPE,
													&cOperation,
													&pcsCharacter->m_lID,
													NULL,
													(cIndex >= 0) ? &cIndex : NULL,
													pvPacketItem,
													NULL,
													(lHPPotionTID != AP_INVALID_IID) ? &lHPPotionTID : NULL,
													(lMPPotionTID != AP_INVALID_IID) ? &lMPPotionTID : NULL,
													NULL,
													(cAutoUseHPGage > (-1)) ? &cAutoUseHPGage : NULL,
													(cAutoUseMPGage > (-1)) ? &cAutoUseMPGage : NULL,
													NULL);

	if (pvPacketItem)
		m_csPacket.FreePacket(pvPacketItem);

	return pvPacket;
}

PVOID AgpmUIStatus::MakePacketQBeltString(AgpdCharacter *pcsCharacter, CHAR *szString, INT16 nStringLength, CHAR* szCooldown, INT16 nCooldownLength, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMUISTATUS_PACKET_OPERATION_ENCODED_STRING;
	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMUISTATUS_PACKET_TYPE,
													&cOperation,
													&pcsCharacter->m_lID,
													NULL,
													NULL,
													NULL,
													szString ? szString : _T(""), &nStringLength,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													szCooldown ? szCooldown : _T(""), &nCooldownLength);

	return pvPacket;
}

PVOID AgpmUIStatus::MakePacketUpdateViewHelmetOption(INT32 lCID, INT16 *pnPacketLength, INT8 cOptionViewHelmet)
{
	if (lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMUISTATUS_PACKET_OPERATION_UPDATE_VIEW_HELMET_OPTION;
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMUISTATUS_PACKET_TYPE,
													&cOperation,
													&lCID,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													&cOptionViewHelmet,
													NULL,
													NULL,
													NULL);
}

BOOL AgpmUIStatus::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket ||
		nSize < 1)
		return FALSE;

	INT8	cOperation			= (-1);
	INT32	lCharacterID		= AP_INVALID_CID;

//	PVOID	pvPacketWholeItem[AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN];
	INT8	cIndex				= (-1);
	PVOID	pvPacketUpdateItem	= NULL;

	CHAR*	szEncodedString		= NULL;
	UINT16	unStringLength		= 0;

	CHAR*	szCooldownString	= NULL;
	UINT16	unCooldownLength	= 0;

//	ZeroMemory(pvPacketWholeItem, sizeof(PVOID) * AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN);

	ApSafeArray<PVOID, AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN>	pvPacketWholeItem;
	pvPacketWholeItem.MemSetAll();

	INT32	lHPPotionTID		= AP_INVALID_IID;
	INT32	lMPPotionTID		= AP_INVALID_IID;

	INT8	cOptionViewHelmet	= 0;

	INT8	cAutoUseHPGage		= (-1);
	INT8	cAutoUseMPGage		= (-1);

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCharacterID,
						&pvPacketWholeItem[0],
						&cIndex,
						&pvPacketUpdateItem,
						&szEncodedString, &unStringLength,
						&lHPPotionTID,
						&lMPPotionTID,
						&cOptionViewHelmet,
						&cAutoUseHPGage,
						&cAutoUseMPGage,
						&szCooldownString, &unCooldownLength);

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
	if (!pcsCharacter)
		return FALSE;

	switch (cOperation) {
	case AGPMUISTATUS_PACKET_OPERATION_ADD:
		{
			OnOperationAdd(pcsCharacter, &pvPacketWholeItem[0], lHPPotionTID, lMPPotionTID, cAutoUseHPGage, cAutoUseMPGage);
		}
		break;

	case AGPMUISTATUS_PACKET_OPERATION_UPDATE_ITEM:
		{
			OnOperationUpdate(pcsCharacter, cIndex, pvPacketUpdateItem, lHPPotionTID, lMPPotionTID, cAutoUseHPGage, cAutoUseMPGage);
		}
		break;

	case AGPMUISTATUS_PACKET_OPERATION_ENCODED_STRING:
		{
			OnOperationEncodedString(pcsCharacter, szEncodedString, unStringLength, szCooldownString, unCooldownLength);
		}
		break;

	case AGPMUISTATUS_PACKET_OPERATION_UPDATE_VIEW_HELMET_OPTION:
		{
			OnOperationUpdateViewHelmetOption(pcsCharacter, cOptionViewHelmet);
		}
		break;
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmUIStatus::OnOperationAdd(AgpdCharacter *pcsCharacter, PVOID *pvPacketWholeItem, INT32 lHPPotionTID, INT32 lMPPotionTID, INT8 cAutoUseHPGage, INT8 cAutoUseMPGage)
{
	if (!pcsCharacter || !pvPacketWholeItem)
		return FALSE;

	AgpdUIStatusADChar	*pcsAttachData		= GetADCharacter(pcsCharacter);

	for (int i = 0; i < AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN; ++i)
	{
		ApBase	*pcsBase	= NULL;

		if (pvPacketWholeItem[i])
			pcsBase	= m_pcsAgpmSkill->GetBaseFromPacket(pvPacketWholeItem[i]);

		if (pcsBase)
		{
			pcsAttachData->m_acsQBeltItem[i].m_eType	= pcsBase->m_eType;
			pcsAttachData->m_acsQBeltItem[i].m_lID		= pcsBase->m_lID;
		}
		else
		{
			pcsAttachData->m_acsQBeltItem[i].m_eType	= APBASE_TYPE_NONE;
			pcsAttachData->m_acsQBeltItem[i].m_lID		= 0;
		}
	}

	pcsAttachData->m_lHPPotionTID	= lHPPotionTID;
	pcsAttachData->m_lMPPotionTID	= lMPPotionTID;

	if (cAutoUseHPGage >= 0)
		pcsAttachData->m_cAutoUseHPGage	= cAutoUseHPGage;
	if (cAutoUseMPGage >= 0)
		pcsAttachData->m_cAutoUseMPGage	= cAutoUseMPGage;

	EnumCallback(AGPMUISTATUS_CB_ADD_QUICKBELT_INFORMATION, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgpmUIStatus::OnOperationUpdate(AgpdCharacter *pcsCharacter, INT32 lIndex, PVOID pvPacketItem, INT32 lHPPotionTID, INT32 lMPPotionTID, INT8 cAutoUseHPGage, INT8 cAutoUseMPGage)
{
	if (!pcsCharacter ||
		lIndex < (-1) ||
		lIndex >= AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN)
		return FALSE;

	AgpdUIStatusADChar	*pcsAttachData		= GetADCharacter(pcsCharacter);

	ApBase	*pcsBase	= m_pcsAgpmSkill->GetBaseFromPacket(pvPacketItem);

	if (pcsBase)
	{
		if (lIndex < 0)
			return FALSE;

		for (int i = (lIndex / AGPMUISTATUS_MAX_QUICKBELT_COLUMN) * AGPMUISTATUS_MAX_QUICKBELT_COLUMN ; i < (lIndex / AGPMUISTATUS_MAX_QUICKBELT_COLUMN + 1) * AGPMUISTATUS_MAX_QUICKBELT_COLUMN; ++i)
		{
			if (pcsAttachData->m_acsQBeltItem[i].m_eType == pcsBase->m_eType &&
				pcsAttachData->m_acsQBeltItem[i].m_lID == pcsBase->m_lID)
			{
				pcsAttachData->m_acsQBeltItem[i].m_eType		= APBASE_TYPE_NONE;
				pcsAttachData->m_acsQBeltItem[i].m_lID			= 0;
			}
		}

		pcsAttachData->m_acsQBeltItem[lIndex].m_eType		= pcsBase->m_eType;
		pcsAttachData->m_acsQBeltItem[lIndex].m_lID			= pcsBase->m_lID;
	}
	else if (lIndex >= 0)
	{
		pcsAttachData->m_acsQBeltItem[lIndex].m_eType		= APBASE_TYPE_NONE;
		pcsAttachData->m_acsQBeltItem[lIndex].m_lID			= 0;
	}

	if (lHPPotionTID != AP_INVALID_IID)
		pcsAttachData->m_lHPPotionTID	= lHPPotionTID;
	if (lMPPotionTID != AP_INVALID_IID)
		pcsAttachData->m_lMPPotionTID	= lMPPotionTID;

	if (cAutoUseHPGage >= 0)
		pcsAttachData->m_cAutoUseHPGage	= cAutoUseHPGage;
	if (cAutoUseMPGage >= 0)
		pcsAttachData->m_cAutoUseMPGage	= cAutoUseMPGage;

	EnumCallback(AGPMUISTATUS_CB_UPDATE_QUICKBELT_ITEM, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgpmUIStatus::OnOperationEncodedString(AgpdCharacter *pcsCharacter, CHAR *szEncodedString, INT32 lStringLength, CHAR* szCooldownString, INT32 lCooldownLength)
{
	if (!pcsCharacter || !szEncodedString || !szEncodedString[0] || lStringLength < 1)
		return FALSE;

	PVOID	pvBuffer[4];
	pvBuffer[0]	= szEncodedString;
	pvBuffer[1]	= IntToPtr(lStringLength);
	pvBuffer[2] = szCooldownString;
	pvBuffer[3] = IntToPtr(lCooldownLength);

	return EnumCallback(AGPMUISTATUS_CB_RECEIVED_ENCODED_QUICKBELT_STRING, pcsCharacter, pvBuffer);
}

BOOL AgpmUIStatus::OnOperationUpdateViewHelmetOption(AgpdCharacter *pcsCharacter, INT8 cOptionViewHelmet)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdUIStatusADChar	*pcsAttachData		= GetADCharacter(pcsCharacter);

	pcsAttachData->m_cOptionViewHelmet		= cOptionViewHelmet;

	return EnumCallback(AGPMUISTATUS_CB_UPDATE_VIEW_HELMET_OPTION, pcsCharacter, &cOptionViewHelmet);
}

BOOL AgpmUIStatus::SetCallbackAddQuickBeltInformation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMUISTATUS_CB_ADD_QUICKBELT_INFORMATION, pfCallback, pClass);
}

BOOL AgpmUIStatus::SetCallbackUpdateQuickBeltItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMUISTATUS_CB_UPDATE_QUICKBELT_ITEM, pfCallback, pClass);
}

BOOL AgpmUIStatus::SetCallbackReceivedEncodedQBeltString(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMUISTATUS_CB_RECEIVED_ENCODED_QUICKBELT_STRING, pfCallback, pClass);
}

BOOL AgpmUIStatus::SetCallbackUpdateViewHelmetOption(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMUISTATUS_CB_UPDATE_VIEW_HELMET_OPTION, pfCallback, pClass);
}

BOOL AgpmUIStatus::SetQBeltEncodedString(AgpdCharacter *pcsCharacter, CHAR *szString, INT32 lStringLength)
{
	if (!pcsCharacter ||
		!szString ||
		!szString[0] ||
		lStringLength < 1)
		return FALSE;

	ASSERT(lStringLength < AGPMUISTATUS_MAX_QUICKBELT_STRING);

	if (lStringLength >= AGPMUISTATUS_MAX_QUICKBELT_STRING)
		return FALSE;

	AgpdUIStatusADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);

	if (!pcsAttachData->pszQBeltEncodedString)
		pcsAttachData->pszQBeltEncodedString	= (CHAR *) new CHAR[AGPMUISTATUS_MAX_QUICKBELT_STRING + 1];

	if (!pcsAttachData->pszQBeltEncodedString)
		return FALSE;

	ZeroMemory(pcsAttachData->pszQBeltEncodedString, sizeof(CHAR) * (AGPMUISTATUS_MAX_QUICKBELT_STRING + 1));

	CopyMemory(pcsAttachData->pszQBeltEncodedString, szString, sizeof(CHAR) * lStringLength);

	return TRUE;
}

BOOL AgpmUIStatus::SetCooldownEncodedString(AgpdCharacter *pcsCharacter, CHAR *szString, INT32 lStringLength)
{
	if(!pcsCharacter || !szString || lStringLength < 1)
		return FALSE;

	if(lStringLength >= AGPMUISTATUS_MAX_COOLDOWN_STRING)
		return FALSE;

	AgpdUIStatusADChar* pcsAttachData = GetADCharacter(pcsCharacter);

	if(!pcsAttachData->pszCooldown)
		pcsAttachData->pszCooldown = new CHAR[AGPMUISTATUS_MAX_COOLDOWN_STRING + 1];
	
	if(!pcsAttachData->pszCooldown)
		return FALSE;

	ZeroMemory(pcsAttachData->pszCooldown, sizeof(CHAR) * (AGPMUISTATUS_MAX_COOLDOWN_STRING + 1));
	CopyMemory(pcsAttachData->pszCooldown, szString, sizeof(CHAR) * lStringLength);

	return TRUE;
}