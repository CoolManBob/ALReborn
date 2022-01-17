#include "AgpmItemConvert.h"

AgpmItemConvert::AgpmItemConvert()
{
	SetModuleName("AgpmItemConvert");

	m_astTablePhysical.MemSetAll();
	//m_astTablePhysicalFail.MemSetAll();
	m_astTableSocket.MemSetAll();
	m_astTableSocketFail.MemSetAll();
	m_astTableSpiritStone.MemSetAll();
	ZeroMemory(&m_stTableSameAttrBonus, sizeof(AgpdItemConvertSameAttrBonus));
	m_astTableRune.MemSetAll();
	m_astTableRuneWeaponFail.MemSetAll();

	m_lIndexADItem			= (-1);
	m_lIndexADItemTemplate	= (-1);

	SetPacketType(AGPMITEMCONVERT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,				// operation
							AUTYPE_INT32,		1,				// character id
							AUTYPE_INT32,		1,				// item id
							AUTYPE_INT32,		1,				// catalyst (rune, spiritstone) item id
							AUTYPE_INT8,		1,				// action result
							AUTYPE_INT8,		1,				// # of physical convert
							AUTYPE_INT8,		1,				// # of socket
							AUTYPE_INT8,		1,				// # of converted socket
							AUTYPE_INT32,		1,				// add convert item tid
							AUTYPE_PACKET,		1,				// converted item tid list
							AUTYPE_END,			0);

	m_csPacketTID.SetFlagLength(sizeof(INT8));
	m_csPacketTID.SetFieldType(
							AUTYPE_INT32,		1,				// converted item tid list
							AUTYPE_INT32,		1,
							AUTYPE_INT32,		1,
							AUTYPE_INT32,		1,
							AUTYPE_INT32,		1,
							AUTYPE_INT32,		1,
							AUTYPE_INT32,		1,
							AUTYPE_INT32,		1,
							AUTYPE_END,			0);

	ZeroMemory(&m_stConvertPoint, sizeof(m_stConvertPoint));
}

AgpmItemConvert::~AgpmItemConvert()
{
}

BOOL AgpmItemConvert::OnAddModule()
{
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmSkill			= (AgpmSkill *)			GetModule("AgpmSkill");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmSkill)
		return FALSE;

	m_lIndexADChar	= m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdItemConvertADChar), ConAgpdItemConvertADChar, DesAgpdItemConvertADChar);
	if (m_lIndexADChar < 0)
		return FALSE;

	m_lIndexADItem	= m_pcsAgpmItem->AttachItemData(this, sizeof(AgpdItemConvertADItem), ConAgpdItemConvertADItem, DesAgpdItemConvertADItem);
	if (m_lIndexADItem < 0)
		return FALSE;

	m_lIndexADItemTemplate	= m_pcsAgpmItem->AttachItemTemplateData(this, sizeof(AgpdItemConvertADItemTemplate), ConAgpdItemConvertADItemTemplate, DesAgpdItemConvertADItemTemplate);
	if (m_lIndexADItemTemplate < 0)
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackEquip(CBEquipItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUnEquip(CBUnEquipItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackRequestSpiritStoneConvert(CBRequestConvert, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackInit(CBInitItem, this))
		return FALSE;

	return TRUE;
}

AgpdItemConvertADChar* AgpmItemConvert::GetADChar(AgpdCharacter *pcsCharacter)
{
	return (AgpdItemConvertADChar *) m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexADChar, (PVOID) pcsCharacter);
}

AgpdItemConvertADItem* AgpmItemConvert::GetADItem(AgpdItem *pcsItem)
{
	return (AgpdItemConvertADItem *) m_pcsAgpmItem->GetAttachedModuleData(m_lIndexADItem, (PVOID) pcsItem);
}

AgpdItemConvertADItemTemplate* AgpmItemConvert::GetADItemTemplate(AgpdItemTemplate *pcsItemTemplate)
{
	return (AgpdItemConvertADItemTemplate *) m_pcsAgpmItem->GetAttachedModuleData(m_lIndexADItemTemplate, (PVOID) pcsItemTemplate);
}

BOOL AgpmItemConvert::ConAgpdItemConvertADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmItemConvert		*pThis			= (AgpmItemConvert *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;

	AgpdItemConvertADChar	*pcsAttachData	= pThis->GetADChar(pcsCharacter);
	if(!pcsAttachData)
		return FALSE;

	pThis->m_pcsAgpmFactors->InitFactor(&pcsAttachData->m_csRuneAttrFactor);
	/*pcsAttachData->m_stOptionSkillData.init();*/

	return TRUE;
}

BOOL AgpmItemConvert::DesAgpdItemConvertADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgpmItemConvert::ConAgpdItemConvertADItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmItemConvert		*pThis			= (AgpmItemConvert *)	pClass;
	AgpdItem			*pcsItem		= (AgpdItem *)			pData;

	AgpdItemConvertADItem	*pcsAttachData	= pThis->GetADItem(pcsItem);

	pcsAttachData->m_lPhysicalConvertLevel		= 0;

	pcsAttachData->m_lNumSocket					= 0;
	pcsAttachData->m_lNumConvert				= 0;

	pcsAttachData->m_stSocketAttr.MemSetAll();

	pThis->m_pcsAgpmFactors->InitFactor(&pcsAttachData->m_csRuneAttrFactor);
	pcsAttachData->m_stOptionSkillData.init();

	return TRUE;
}

BOOL AgpmItemConvert::DesAgpdItemConvertADItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	/*
	if (!pData || !pClass)
		return FALSE;

	AgpmItemConvert		*pThis			= (AgpmItemConvert *)	pClass;
	AgpdItem			*pcsItem		= (AgpdItem *)			pData;

	AgpdItemConvertADItem	*pcsAttachData	= pThis->GetADItem(pcsItem);
	*/

	return TRUE;
}

BOOL AgpmItemConvert::ConAgpdItemConvertADItemTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmItemConvert		*pThis			= (AgpmItemConvert *)	pClass;
	AgpdItemTemplate	*pcsTemplate	= (AgpdItemTemplate *)	pData;

	AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= pThis->GetADItemTemplate(pcsTemplate);

	pcsAttachTemplateData->m_bRuneConvertableEquipKind.MemSetAll();
	pcsAttachTemplateData->m_bRuneConvertableEquipPart.MemSetAll();

	pcsAttachTemplateData->m_lRuneSuccessProbability	= 0;
	pcsAttachTemplateData->m_lRuneRestrictLevel			= 0;

	ZeroMemory(pcsAttachTemplateData->m_szDescription, sizeof(CHAR) * (AGPDITEMCONVERT_MAX_DESCRIPTION + 1));

	pcsAttachTemplateData->m_lAntiTypeNumber			= 0;

	return TRUE;
}

BOOL AgpmItemConvert::DesAgpdItemConvertADItemTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgpmItemConvert::DispatchAddPacket(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, PVOID pvPacket, INT16 nSize)
{
	if (!pcsCharacter || !pcsItem || !pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation				= (-1);
	INT32	lCID					= AP_INVALID_CID;
	INT32	lIID					= AP_INVALID_IID;
	INT32	lCatalystIID			= AP_INVALID_IID;
	INT8	cActionResult			= (-1);
	INT8	cNumPhysicalConvert		= (-1);
	INT8	cNumSocket				= (-1);
	INT8	cNumConvertedSocket		= (-1);
	INT32	lTID					= AP_INVALID_IID;
	PVOID	pvPacketTIDList			= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
							&cOperation,
							&lCID,
							&lIID,
							&lCatalystIID,
							&cActionResult,
							&cNumPhysicalConvert,
							&cNumSocket,
							&cNumConvertedSocket,
							&lTID,
							&pvPacketTIDList);

	return OnOperationAdd(pcsCharacter, pcsItem, cNumPhysicalConvert, cNumSocket, cNumConvertedSocket, pvPacketTIDList);
}

BOOL AgpmItemConvert::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation				= (-1);
	INT32	lCID					= AP_INVALID_CID;
	INT32	lIID					= AP_INVALID_IID;
	INT32	lCatalystIID			= AP_INVALID_IID;
	INT8	cActionResult			= (-1);
	INT8	cNumPhysicalConvert		= (-1);
	INT8	cNumSocket				= (-1);
	INT8	cNumConvertedSocket		= (-1);
	INT32	lTID					= AP_INVALID_IID;
	PVOID	pvPacketTIDList			= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
							&cOperation,
							&lCID,
							&lIID,
							&lCatalystIID,
							&cActionResult,
							&cNumPhysicalConvert,
							&cNumSocket,
							&cNumConvertedSocket,
							&lTID,
							&pvPacketTIDList);

	if (!pstCheckArg->bReceivedFromServer &&
		pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
		pstCheckArg->lSocketOwnerID != lCID)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);

	if (!pstCheckArg->bReceivedFromServer && pcsCharacter && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	/*
	if (!pcsCharacter)
		return FALSE;
	*/

	AgpdItem		*pcsItem		= NULL;
	if (lIID != AP_INVALID_IID)
		pcsItem	= m_pcsAgpmItem->GetItem(lIID);

	/*
	if (!pcsItem)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}
	*/

	switch (cOperation) {
	case AGPDITEMCONVERT_OPERATION_ADD:
		{
			if (pstCheckArg->bReceivedFromServer)
				OnOperationAdd(pcsCharacter, pcsItem, cNumPhysicalConvert, cNumSocket, cNumConvertedSocket, pvPacketTIDList);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_REQUEST_PHYSICAL_CONVERT:
		{
			OnOperationRequestPhysicalConvert(pcsCharacter, pcsItem, lCatalystIID);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_REQUEST_ADD_SOCKET:
		{
			return FALSE;

			OnOperationRequestAddSocket(pcsCharacter, pcsItem);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_REQUEST_RUNE_CONVERT:
		{
			OnOperationRequestRuneConvert(pcsCharacter, pcsItem, lCatalystIID);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_REQUEST_SPIRITSTONE_CONVERT:
		{
			OnOperationRequestSpiritStoneConvert(pcsCharacter, pcsItem, lCatalystIID);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_RESPONSE_PHYSICAL_CONVERT:
		{
			if (pstCheckArg->bReceivedFromServer)
				OnOperationResponsePhysicalConvert(pcsCharacter, pcsItem, cActionResult, cNumPhysicalConvert, cNumSocket, cNumConvertedSocket);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_RESPONSE_ADD_SOCKET:
		{
			if (pstCheckArg->bReceivedFromServer)
				OnOperationResponseAddSocket(pcsCharacter, pcsItem, cActionResult, cNumPhysicalConvert, cNumSocket, cNumConvertedSocket);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_RESPONSE_SPIRITSTONE_CONVERT:
		{
			if (pstCheckArg->bReceivedFromServer)
				OnOperationResponseSpiritStoneConvert(pcsCharacter, pcsItem, cActionResult, cNumPhysicalConvert, cNumSocket, cNumConvertedSocket, lTID);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_RESPONSE_RUNE_CONVERT:
		{
			if (pstCheckArg->bReceivedFromServer)
				OnOperationResponseRuneConvert(pcsCharacter, pcsItem, cActionResult, cNumPhysicalConvert, cNumSocket, cNumConvertedSocket, lTID);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_RESPONSE_RUNE_CHECK_RESULT:
		{
			if (pstCheckArg->bReceivedFromServer)
				OnOperationResponseRuneCheckResult(pcsCharacter, pcsItem, lCatalystIID, cActionResult);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_RESPONSE_SPIRITSTONE_CHECK_RESULT:
		{
			if (pstCheckArg->bReceivedFromServer)
				OnOperationResponseSpiritStoneCheckResult(pcsCharacter, pcsItem, lCatalystIID, cActionResult);
		}
		break;

	case AGPDITEMCONVERT_OPERATION_CHECK_CASH_RUNE_CONVERT:
		{
			OnOperationCheckCashRuneConvert(pcsCharacter, pcsItem, lCatalystIID);
		}
		break;
		
	case AGPDITEMCONVERT_OPERATION_SOCKET_INITIALIZE:
		{
			if (pcsCharacter == pcsItem->m_pcsCharacter)
				EnumCallback(AGPDITEMCONVERT_CB_PROCESS_SOCKET_INITIALIZE, pcsItem, &lCatalystIID);
		}
	}

	if (pcsCharacter)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmItemConvert::OnOperationAdd(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, PVOID pvPacketTIDList)
{
	if (!pcsItem)
		return FALSE;

	// 개조가 가능한 놈인지 확인한다.
	// 현재는 무기와 방어구 상의 갑옷만 된다.

	/*
	if (!(((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON ||
		 (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR &&
		  ((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nPart == AGPMITEM_PART_BODY)))
		  return FALSE;
	*/

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	INT32	lItemRank		= GetPhysicalItemRank(pcsItem);

	if (cNumPhysicalConvert >= 0 &&
		cNumPhysicalConvert <= lItemRank)
		pcsAttachData->m_lPhysicalConvertLevel	= (INT32) cNumPhysicalConvert;

	if (cNumSocket >= 0 &&
		cNumSocket <= AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
	{
		pcsAttachData->m_lNumSocket	= (INT32) cNumSocket;

		if (IsSocketConvertable(pcsItem) == AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS &&
			pcsAttachData->m_lNumSocket	== 0)
			pcsAttachData->m_lNumSocket = (pcsItem->m_pcsItemTemplate->m_lMinSocketNum > 1) ? pcsItem->m_pcsItemTemplate->m_lMinSocketNum : 1;

		if (cNumConvertedSocket > 0 &&
			cNumConvertedSocket <= cNumSocket &&
			pvPacketTIDList)
		{
			m_csPacketTID.GetField(FALSE, pvPacketTIDList, 0,
									&pcsAttachData->m_stSocketAttr[0].lTID,
									&pcsAttachData->m_stSocketAttr[1].lTID,
									&pcsAttachData->m_stSocketAttr[2].lTID,
									&pcsAttachData->m_stSocketAttr[3].lTID,
									&pcsAttachData->m_stSocketAttr[4].lTID,
									&pcsAttachData->m_stSocketAttr[5].lTID,
									&pcsAttachData->m_stSocketAttr[6].lTID,
									&pcsAttachData->m_stSocketAttr[7].lTID);

			// spirit stone 인지 rune 인지 보고 세팅한다.
			// 여기서 cNumConvertedSocket 의 유효성 검사도 해야 한다.

			INT32	lConvertedSocket	= 0;

			for (int i = 0; i < (int) cNumConvertedSocket; ++i)
			{
				AgpdItemTemplate	*pcsTemplate	= m_pcsAgpmItem->GetItemTemplate(pcsAttachData->m_stSocketAttr[i].lTID);
				if (!pcsTemplate ||
					pcsTemplate->m_nType != AGPMITEM_TYPE_USABLE ||
					(((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_SPIRIT_STONE &&
					 ((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_RUNE))
					continue;

				pcsAttachData->m_stSocketAttr[lConvertedSocket].pcsItemTemplate	= pcsTemplate;

				if (((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
					pcsAttachData->m_stSocketAttr[lConvertedSocket].bIsSpiritStone	= TRUE;
				else
					pcsAttachData->m_stSocketAttr[lConvertedSocket].bIsSpiritStone	= FALSE;

				lConvertedSocket++;
			}

			pcsAttachData->m_lNumConvert = lConvertedSocket;
		}
	}

	EnumCallback(AGPDITEMCONVERT_CB_ADD, pcsItem, NULL);

	return TRUE;
}

BOOL AgpmItemConvert::CBEquipItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmItemConvert		*pThis			= (AgpmItemConvert *)	pClass;
	AgpdItem			*pcsItem		= (AgpdItem *)			pData;

	if (!pcsItem->m_pcsCharacter)
		return FALSE;

	AgpdItemConvertADChar	*pcsAttachCharData	= pThis->GetADChar(pcsItem->m_pcsCharacter);
	AgpdItemConvertADItem	*pcsAttachItemData	= pThis->GetADItem(pcsItem);

	if(!pcsAttachCharData || !pcsAttachItemData)
		return FALSE;

	pThis->m_pcsAgpmFactors->CalcFactor(&pcsAttachCharData->m_csRuneAttrFactor, &pcsAttachItemData->m_csRuneAttrFactor, TRUE, FALSE, TRUE, FALSE);

	/*pcsAttachCharData->m_stOptionSkillData += pcsAttachItemData->m_stOptionSkillData;*/

	return TRUE;
}

BOOL AgpmItemConvert::CBUnEquipItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmItemConvert		*pThis			= (AgpmItemConvert *)	pClass;
	AgpdItem			*pcsItem		= (AgpdItem *)			pData;

	if (!pcsItem->m_pcsCharacter)
		return FALSE;

	AgpdItemConvertADChar	*pcsAttachCharData	= pThis->GetADChar(pcsItem->m_pcsCharacter);
	AgpdItemConvertADItem	*pcsAttachItemData	= pThis->GetADItem(pcsItem);

	if(!pcsAttachCharData || !pcsAttachItemData)
		return FALSE;

	pThis->m_pcsAgpmFactors->CalcFactor(&pcsAttachCharData->m_csRuneAttrFactor, &pcsAttachItemData->m_csRuneAttrFactor, TRUE, FALSE, FALSE, FALSE);

	/*pcsAttachCharData->m_stOptionSkillData -= pcsAttachItemData->m_stOptionSkillData;*/

	return TRUE;
}

BOOL AgpmItemConvert::SetCallbackProcessPhysicalConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_PROCESS_PHYSICAL_CONVERT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackProcessSocketConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_PROCESS_SOCKET_CONVERT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackProcessSpiritStoneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_PROCESS_SPIRITSTONE_CONVERT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackProcessRuneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_PROCESS_RUNE_CONVERT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackProcessSocketInitialize(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_PROCESS_SOCKET_INITIALIZE, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackResultPhysicalConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_RESULT_PHYSICAL_CONVERT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackResultSocketConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_RESULT_SOCKET_CONVERT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackResultSpiritStoneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_RESULT_SPIRITSTONE_CONVERT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackResultRuneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_RESULT_RUNE_CONVERT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackSendUpdateFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_SEND_UPDATE_FACTOR, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackAskReallySpiritStoneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_ASK_REALLY_SPIRITSTONE_CONVERT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackSpiritStoneCheckResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_SPIRITSTONE_CONVERT_CHECK_RESULT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackAskReallyRuneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_ASK_REALLY_RUNE_CONVERT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackRuneCheckResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_RUNE_CONVERT_CHECK_RESULT, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_ADD, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_UPDATE, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackAdjustSuccessProb(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_ADJUST_SUCCESS_PROB, pfCallback, pClass);
}

BOOL AgpmItemConvert::SetCallbackConvertAsDrop(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPDITEMCONVERT_CB_CONVERT_AS_DROP, pfCallback, pClass);
}

BOOL AgpmItemConvert::CBInitItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmItemConvert		*pThis				= (AgpmItemConvert *)	pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)			pData;

	if (pThis->IsSocketConvertable(pcsItem, FALSE) == AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS)
	{
		if (pThis->GetNumSocket(pcsItem) == 0)
		{
			AgpdItemConvertADItem	*pcsAttachData	= pThis->GetADItem(pcsItem);

			pcsAttachData->m_lNumSocket	= (pcsItem->m_pcsItemTemplate->m_lMinSocketNum > 1) ? pcsItem->m_pcsItemTemplate->m_lMinSocketNum : 1;
		}
	}

	return TRUE;
}

BOOL AgpmItemConvert::CalcConvertFactor(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	CalcPhysicalConvertFactor(pcsItem);

	CalcSpiritStoneConvertFactor(pcsItem);
	
	CalcRuneConvertFactor(pcsItem);

	return TRUE;
}

BOOL AgpmItemConvert::EncodeConvertHistory(AgpdItemConvertADItem *pcsAttachData, CHAR *szBuffer, INT32 lBufferSize)
{
	if (!pcsAttachData || !szBuffer || lBufferSize < 1)
		return FALSE;

	INT32	lStrlen	= sprintf(szBuffer, "%d:%d:", pcsAttachData->m_lPhysicalConvertLevel, pcsAttachData->m_lNumSocket);
	ASSERT(lStrlen < lBufferSize);

	for (int i = 0; i < pcsAttachData->m_lNumConvert; ++i)
	{
		lStrlen	= sprintf(szBuffer + strlen(szBuffer), "%d,", pcsAttachData->m_stSocketAttr[i].lTID);
		ASSERT(lStrlen < lBufferSize);
	}

	lStrlen	= sprintf(szBuffer + strlen(szBuffer) - 1, ":");
	ASSERT(lStrlen < lBufferSize);

	return TRUE;
}

BOOL AgpmItemConvert::DecodeConvertHistory(AgpdItemConvertADItem *pcsAttachData, CHAR *szBuffer, INT32 lBufferSize, BOOL bIsAddAttribute, AgpdItem *pcsItem)
{
	if (!pcsAttachData || !szBuffer || lBufferSize < 1)
		return FALSE;

	sscanf(szBuffer, "%d:%d:", &pcsAttachData->m_lPhysicalConvertLevel, &pcsAttachData->m_lNumSocket);

	INT32	lNumSeparator	= 0;
	INT32	i = 0;
	for (i = 0; i < lBufferSize; ++i)
	{
		if (szBuffer[i] == ':')
			++lNumSeparator;

		if (lNumSeparator == 2)
			break;
	}

	if (i == lBufferSize)
		return FALSE;
	else if (i == lBufferSize - 1)
		pcsAttachData->m_lNumConvert	= 0;
	else
	{
		++i;

		INT32	lTID[AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1];
		INT32	lNumConvert	= 0;

		ZeroMemory(lTID, sizeof(INT32) * (AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1));

		while (i < lBufferSize)
		{
			ASSERT(lNumConvert <= AGPDITEMCONVERT_MAX_WEAPON_SOCKET);
			if (lNumConvert > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
				break;

			sscanf(szBuffer + i, "%d", &lTID[lNumConvert++]);

			for ( ; i < lBufferSize; ++i)
			{
				if (szBuffer[i] == ',' ||
					szBuffer[i] == ':')
					break;
			}

			if (i == lBufferSize)
				break;
			else
				++i;
		}

		if (bIsAddAttribute)
		{
			if (pcsItem)
			{
				for (i = 0; i < lNumConvert; ++i)
				{
					AgpdItemTemplate	*pcsTemplate	= m_pcsAgpmItem->GetItemTemplate(lTID[i]);
					if (pcsTemplate)
					{
						if (pcsTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
							((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
							AddSpiritStoneConvert(pcsItem, pcsTemplate);
						else
							AddRuneConvert(pcsItem, pcsTemplate);
					}
				}
			}
		}
		else
		{
			pcsAttachData->m_lNumConvert	= lNumConvert;

			for (i = 0; i < lNumConvert; ++i)
			{
				pcsAttachData->m_stSocketAttr[i].lTID	= lTID[i];

				pcsAttachData->m_stSocketAttr[i].pcsItemTemplate	= m_pcsAgpmItem->GetItemTemplate(lTID[i]);

				if (pcsAttachData->m_stSocketAttr[i].pcsItemTemplate &&
					pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
					((AgpdItemTemplateUsable *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
					pcsAttachData->m_stSocketAttr[i].bIsSpiritStone	= TRUE;
				else
					pcsAttachData->m_stSocketAttr[i].bIsSpiritStone	= FALSE;
			}
		}
	}

	return TRUE;
}

INT32 AgpmItemConvert::GetTotalConvertPoint(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return 0;

	INT32	lConvertPoint	= 0;

	lConvertPoint	+= GetTotalPhysicalConvertPoint(pcsItem);
	lConvertPoint	+= GetTotalSocketConvertPoint(pcsItem);
	lConvertPoint	+= GetTotalSpiritStoneConvertPoint(pcsItem);
	lConvertPoint	+= GetTotalRuneConvertPoint(pcsItem);

	return lConvertPoint;
}

INT32 AgpmItemConvert::GetTotalPhysicalConvertPoint(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return 0;

	INT32	lNumPhysicalConvert	= GetNumPhysicalConvert(pcsItem);

	INT32	lItemRank			= GetPhysicalItemRank(pcsItem);

	if (lNumPhysicalConvert < 0 ||
		lNumPhysicalConvert > lItemRank)
		return 0;

	return m_stConvertPoint.lPhysicalPoint[lNumPhysicalConvert];
}

INT32 AgpmItemConvert::GetTotalSocketConvertPoint(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return 0;

	INT32	lNumSocketConvert	= GetNumSocket(pcsItem);

	if (lNumSocketConvert < 0 ||
		lNumSocketConvert > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
		return 0;

	return m_stConvertPoint.lSocketPoint[lNumSocketConvert];
}

INT32 AgpmItemConvert::GetTotalSpiritStoneConvertPoint(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return 0;

	INT32	lNumConvertedSocket	= GetNumConvertedSocket(pcsItem);
	if (lNumConvertedSocket < 1)
		return 0;

	INT32	lConvertPoint	= 0;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	for (int i = 0; i < lNumConvertedSocket; ++i)
	{
		if (pcsAttachData->m_stSocketAttr[i].bIsSpiritStone &&
			pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)
		{
			INT32	lItemRank	= 0;
			m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

			if (lItemRank > 0 && lItemRank <= AGPDITEMCONVERT_MAX_SPIRIT_STONE)
				lConvertPoint	+= m_stConvertPoint.lSpiritStonePoint[lItemRank];
		}
	}

	return lConvertPoint;
}

INT32 AgpmItemConvert::GetTotalRuneConvertPoint(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return 0;

	INT32	lNumConvertedSocket	= GetNumConvertedSocket(pcsItem);
	if (lNumConvertedSocket < 1)
		return 0;

	INT32	lConvertPoint	= 0;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	for (int i = 0; i < lNumConvertedSocket; ++i)
	{
		if (!pcsAttachData->m_stSocketAttr[i].bIsSpiritStone &&
			pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)
		{
			INT32	lItemRank	= 0;
			m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

			if (lItemRank > 0 && lItemRank <= AGPDITEMCONVERT_MAX_SPIRIT_STONE)
				lConvertPoint	+= m_stConvertPoint.lRunePoint[lItemRank];
		}
	}

	return lConvertPoint;
}

INT32 AgpmItemConvert::GetSpiritStoneAttrConvertPoint(AgpdItem *pcsItem, INT32 lAttributeType)
{
	if (!pcsItem ||
		lAttributeType < AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC ||
		lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_ICE)
		return 0;

	INT32	lNumConvertedSocket	= GetNumConvertedSocket(pcsItem);
	if (lNumConvertedSocket < 1)
		return 0;

	INT32	lConvertPoint	= 0;

	AgpdItemConvertADItem	*pcsAttachData	= GetADItem(pcsItem);

	for (int i = 0; i < lNumConvertedSocket; ++i)
	{
		if (pcsAttachData->m_stSocketAttr[i].bIsSpiritStone &&
			pcsAttachData->m_stSocketAttr[i].pcsItemTemplate &&
			pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
			((AgpdItemTemplateUsable *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
		{
			if (((AgpdItemTemplateUsableSpiritStone *) pcsAttachData->m_stSocketAttr[i].pcsItemTemplate)->m_eSpiritStoneType == lAttributeType)
			{
				INT32	lItemRank	= 0;
				m_pcsAgpmFactors->GetValue(&pcsAttachData->m_stSocketAttr[i].pcsItemTemplate->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

				if (lItemRank > 0 && lItemRank <= AGPDITEMCONVERT_MAX_SPIRIT_STONE)
					lConvertPoint	+= m_stConvertPoint.lSpiritStonePoint[lItemRank];
			}
		}
	}

	return lConvertPoint;
}

BOOL AgpmItemConvert::CBRequestConvert(PVOID pData, PVOID pClass, PVOID pCustData)
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
	if (eResult == AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
		return pThis->CBRequestSpiritStoneConvert(pData, pClass, pCustData);

	AgpdItemConvertRuneResult	eRuneResult	= pThis->IsValidRuneItem(pcsSpiritStone);
	if (eRuneResult == AGPDITEMCONVERT_RUNE_RESULT_SUCCESS)
		return pThis->CBRequestRuneConvert(pData, pClass, pCustData);

	return TRUE;
}