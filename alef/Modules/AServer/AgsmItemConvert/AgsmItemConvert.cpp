#include "AgsmItemConvert.h"
#include "AgpmConfig.h"

AgsmItemConvert::AgsmItemConvert()
{
	SetModuleName("AgsmItemConvert");
}

AgsmItemConvert::~AgsmItemConvert()
{
}

BOOL AgsmItemConvert::OnAddModule()
{
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmItemConvert	= (AgpmItemConvert *)	GetModule("AgpmItemConvert");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgsmAOIFilter		= (AgsmAOIFilter *)		GetModule("AgsmAOIFilter");
	m_pcsAgsmCharacter		= (AgsmCharacter *)		GetModule("AgsmCharacter");
	m_pcsAgsmItem			= (AgsmItem *)			GetModule("AgsmItem");
	m_pcsAgsmCombat			= (AgsmCombat *)		GetModule("AgsmCombat");
	m_pcsAgsmSkill			= (AgsmSkill *)			GetModule("AgsmSkill");
	m_pcsAgpmLog			= (AgpmLog *)			GetModule("AgpmLog");	
	
	if (!m_pcsAgpmFactors || !m_pcsAgsmSkill || !m_pcsAgpmItem || !m_pcsAgpmItemConvert || !m_pcsAgpmCharacter ||
		!m_pcsAgsmAOIFilter || !m_pcsAgsmCharacter || !m_pcsAgsmItem || !m_pcsAgsmCombat)
		return FALSE;

	if (!m_pcsAgpmItemConvert->SetCallbackProcessPhysicalConvert(CBProcessPhysicalConvert, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackProcessSocketConvert(CBProcessSocketConvert, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackProcessSpiritStoneConvert(CBProcessSpiritStoneConvert, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackProcessRuneConvert(CBProcessRuneConvert, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackProcessSocketInitialize(CBProcessSocketInitialize, this))
		return FALSE;

	if (!m_pcsAgpmItemConvert->SetCallbackSendUpdateFactor(CBSendUpdateFactor, this))
		return FALSE;

	if (!m_pcsAgpmItemConvert->SetCallbackRuneCheckResult(CBRuneCheckResult, this))
		return FALSE;
	if (!m_pcsAgpmItemConvert->SetCallbackSpiritStoneCheckResult(CBSpiritStoneCheckResult, this))
		return FALSE;

	if (!m_pcsAgpmItemConvert->SetCallbackAdjustSuccessProb(CBAdjustSuccessProb, this))
		return FALSE;

	if (!m_pcsAgsmItem->SetCallbackSendItem(CBSendItem, this))
		return FALSE;
	if (!m_pcsAgsmItem->SetCallbackSendItemView(CBSendItemView, this))
		return FALSE;
	if (!m_pcsAgsmItem->SetCallbackItemConvertHistoryInsert(CBItemConvertHistoryInsert, this))
		return FALSE;

	if (!m_pcsAgsmCombat->SetCallbackCheckDefense(CBCheckDefense, this))
		return FALSE;

	if (!m_pcsAgsmSkill->SetCallbackCheckSpecialStatusFactor(CBCheckSpecialStatusFactor, this))
		return FALSE;
	if (!m_pcsAgsmSkill->SetCallbackCheckPoisonStatusFactor(CBCheckPoisonStatusFactor, this))
		return FALSE;
	
	return TRUE;
}

BOOL AgsmItemConvert::OnInit()
{
	m_pcsAgpmTitle			= (AgpmTitle *)			GetModule("AgpmTitle");
	m_pcsAgpmConfig			= (AgpmConfig *)		GetModule("AgpmConfig");

	return TRUE;
}

BOOL AgsmItemConvert::CBProcessPhysicalConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmItemConvert			*pThis				= (AgsmItemConvert *)		pClass;
	AgpdItem				*pcsItem			= (AgpdItem *)				pData;
	AgpdItemConvertResult	*peResult			= (AgpdItemConvertResult *)	pCustData;

	if (!pcsItem->m_pcsCharacter)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData		= pThis->m_pcsAgpmItemConvert->GetADItem(pcsItem);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmItemConvert->MakePacketResponsePhysicalConvert(pcsItem->m_ulCID, pcsItem->m_lID, (INT8) *peResult, pcsAttachData->m_lPhysicalConvertLevel, pcsAttachData->m_lNumSocket, pcsAttachData->m_lNumConvert, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	pThis->m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	AgsdItem	*pcsAgsdItem	= pThis->m_pcsAgsmItem->GetADItem(pcsItem);
	if (pcsAgsdItem && !pcsAgsdItem->m_bIsNeedInsertDB)
		pThis->EnumCallback(AGSMITEMCONVERT_CB_UPDATE_CONVERT_HISTORY_TO_DB, pcsItem, NULL);

	// 2005.03.15. steeple
	pThis->WriteConvertLog(pcsItem->m_pcsCharacter, pcsItem, 1, (INT8)*peResult, 0);

	return	bSendResult;
}

BOOL AgsmItemConvert::CBProcessSocketConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmItemConvert				*pThis				= (AgsmItemConvert *)				pClass;
	AgpdItem					*pcsItem			= (AgpdItem *)						pData;
	AgpdItemConvertSocketResult	*peResult			= (AgpdItemConvertSocketResult *)	pCustData;

	if (!pcsItem->m_pcsCharacter)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData		= pThis->m_pcsAgpmItemConvert->GetADItem(pcsItem);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmItemConvert->MakePacketResponseSocketAdd(pcsItem->m_ulCID, pcsItem->m_lID, (INT8) *peResult, pcsAttachData->m_lPhysicalConvertLevel, pcsAttachData->m_lNumSocket, pcsAttachData->m_lNumConvert, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	pThis->m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	AgsdItem	*pcsAgsdItem	= pThis->m_pcsAgsmItem->GetADItem(pcsItem);
	if (pcsAgsdItem && !pcsAgsdItem->m_bIsNeedInsertDB)
		pThis->EnumCallback(AGSMITEMCONVERT_CB_UPDATE_CONVERT_HISTORY_TO_DB, pcsItem, NULL);

	// 2005.03.15. steeple
	pThis->WriteConvertLog(pcsItem->m_pcsCharacter, pcsItem, 4, (INT8)*peResult, 0);

	return	bSendResult;
}

BOOL AgsmItemConvert::CBProcessSpiritStoneConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData || !pData)
		return FALSE;

	AgsmItemConvert						*pThis				= (AgsmItemConvert *)					pClass;
	AgpdItem							*pcsItem			= (AgpdItem *)							pData;
	AgpdItemConvertSpiritStoneResult	*peResult			= (AgpdItemConvertSpiritStoneResult *)	pCustData;

	if (!pcsItem->m_pcsCharacter)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData		= pThis->m_pcsAgpmItemConvert->GetADItem(pcsItem);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmItemConvert->MakePacketResponseSpiritStoneConvert(
									pcsItem->m_ulCID,
									pcsItem->m_lID,
									(INT8) *peResult,
									pcsAttachData->m_lPhysicalConvertLevel,
									pcsAttachData->m_lNumSocket,
									pcsAttachData->m_lNumConvert,
									(pcsAttachData->m_lNumConvert > 0) ? pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert - 1].lTID : AP_INVALID_IID,
									&nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	pThis->m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	AgsdItem	*pcsAgsdItem	= pThis->m_pcsAgsmItem->GetADItem(pcsItem);
	if (pcsAgsdItem && !pcsAgsdItem->m_bIsNeedInsertDB)
		pThis->EnumCallback(AGSMITEMCONVERT_CB_UPDATE_CONVERT_HISTORY_TO_DB, pcsItem, NULL);

	// 2005.03.15. steeple
	pThis->WriteConvertLog(pcsItem->m_pcsCharacter, pcsItem, 2, (INT8)*peResult, 0);

	return	bSendResult;
}

BOOL AgsmItemConvert::CBProcessRuneConvert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmItemConvert				*pThis				= (AgsmItemConvert *)			pClass;
	AgpdItem					*pcsItem			= (AgpdItem *)					pData;
	AgpdItemConvertRuneResult	*peResult			= (AgpdItemConvertRuneResult *)	pCustData;

	if (!pcsItem->m_pcsCharacter)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData		= pThis->m_pcsAgpmItemConvert->GetADItem(pcsItem);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmItemConvert->MakePacketResponseRuneConvert(
									pcsItem->m_ulCID,
									pcsItem->m_lID,
									(INT8) *peResult,
									pcsAttachData->m_lPhysicalConvertLevel,
									pcsAttachData->m_lNumSocket,
									pcsAttachData->m_lNumConvert,
									(pcsAttachData->m_lNumConvert > 0) ? pcsAttachData->m_stSocketAttr[pcsAttachData->m_lNumConvert - 1].lTID : AP_INVALID_IID,
									&nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	pThis->m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	AgsdItem	*pcsAgsdItem	= pThis->m_pcsAgsmItem->GetADItem(pcsItem);
	if (pcsAgsdItem && !pcsAgsdItem->m_bIsNeedInsertDB)
		pThis->EnumCallback(AGSMITEMCONVERT_CB_UPDATE_CONVERT_HISTORY_TO_DB, pcsItem, NULL);

	// 2005.03.15. steeple
	pThis->WriteConvertLog(pcsItem->m_pcsCharacter, pcsItem, 3, (INT8)*peResult, 0);
	return	bSendResult;
}

BOOL AgsmItemConvert::CBProcessSocketInitialize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmItemConvert				*pThis				= (AgsmItemConvert *)			pClass;
	AgpdItem					*pcsItem			= (AgpdItem *)					pData;
	INT32						lCatalystIID		= *((INT32 *) pCustData);

	if (!pcsItem->m_pcsCharacter)
		return FALSE;

	AgpdItem *pAgpdItem_Catalyst = pThis->m_pcsAgpmItem->GetItem(lCatalystIID);
	if (!pAgpdItem_Catalyst)
		return FALSE;

	if (AGPMITEM_TYPE_USABLE != pAgpdItem_Catalyst->m_pcsItemTemplate->m_nType
		|| AGPMITEM_USABLE_TYPE_SOCKET_INIT != ((AgpdItemTemplateUsable *) pAgpdItem_Catalyst->m_pcsItemTemplate)->m_nUsableItemType)
		return FALSE;

	if (FALSE == pThis->m_pcsAgpmItemConvert->IsSocketInitializable(pcsItem))
		{
		return FALSE;
		}

	pThis->m_pcsAgpmItemConvert->InitializeSocket(pcsItem, TRUE);
	
	pThis->WriteConvertLog(pcsItem->m_pcsCharacter, pcsItem, 5, AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS, 0);

	pThis->m_pcsAgsmItem->WriteUseLog(pcsItem->m_pcsCharacter->m_lID, pAgpdItem_Catalyst);

	pThis->m_pcsAgpmItem->SubItemStackCount(pAgpdItem_Catalyst, 1, TRUE);

	// send packet
	AgpdItemConvertADItem	*pcsAttachData		= pThis->m_pcsAgpmItemConvert->GetADItem(pcsItem);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmItemConvert->MakePacketResponseSocketAdd(pcsItem->m_ulCID, pcsItem->m_lID, AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS, pcsAttachData->m_lPhysicalConvertLevel, pcsAttachData->m_lNumSocket, pcsAttachData->m_lNumConvert, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	pThis->m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	AgsdItem	*pcsAgsdItem	= pThis->m_pcsAgsmItem->GetADItem(pcsItem);
	if (pcsAgsdItem && !pcsAgsdItem->m_bIsNeedInsertDB)
		pThis->EnumCallback(AGSMITEMCONVERT_CB_UPDATE_CONVERT_HISTORY_TO_DB, pcsItem, NULL);

	return	bSendResult;
}

BOOL AgsmItemConvert::CBSendUpdateFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItemConvert			*pThis					= (AgsmItemConvert *)		pClass;
	AgpdItem				*pcsItem				= (AgpdItem *)				pData;
	PVOID					*ppvBuffer				= (PVOID *)					pCustData;

	PVOID					pvPacketFactorPoint		= (PVOID)					ppvBuffer[0];
	PVOID					pvPacketFactorPercent	= (PVOID)					ppvBuffer[1];

	return pThis->m_pcsAgsmItem->SendPacketItemFactor(pcsItem, pvPacketFactorPoint, pvPacketFactorPercent, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
}

BOOL AgsmItemConvert::CBRuneCheckResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItemConvert			*pThis					= (AgsmItemConvert *)		pClass;
	AgpdItem				*pcsItem				= (AgpdItem *)				pData;
	PVOID					*ppvBuffer				= (PVOID *)					pCustData;

	AgpdItem				*pcsRuneItem			= (AgpdItem *)				ppvBuffer[0];
	AgpdItemConvertRuneResult	eResult				= (AgpdItemConvertRuneResult) PtrToInt(ppvBuffer[1]);

	if (!pcsItem->m_pcsCharacter || !pcsRuneItem)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmItemConvert->MakePacketResponseRuneCheckResult(pcsItem->m_ulCID, pcsItem->m_lID, pcsRuneItem->m_lID, (INT8) eResult, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	pThis->m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmItemConvert::CBSpiritStoneCheckResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItemConvert			*pThis					= (AgsmItemConvert *)		pClass;
	AgpdItem				*pcsItem				= (AgpdItem *)				pData;
	PVOID					*ppvBuffer				= (PVOID *)					pCustData;

	AgpdItem				*pcsSpiritStone			= (AgpdItem *)				ppvBuffer[0];
	AgpdItemConvertSpiritStoneResult	eResult		= (AgpdItemConvertSpiritStoneResult) PtrToInt(ppvBuffer[1]);

	if (!pcsItem->m_pcsCharacter || !pcsSpiritStone)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmItemConvert->MakePacketResponseSpiritStoneCheckResult(pcsItem->m_ulCID, pcsItem->m_lID, pcsSpiritStone->m_lID, (INT8) eResult, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	pThis->m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmItemConvert::CBAdjustSuccessProb(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItemConvert	*pThis			= (AgsmItemConvert *)	pClass;
	PVOID			*ppvBuffer		= static_cast<PVOID*>(pData);
	FLOAT			*pfSuccessProb	= (FLOAT *)				pCustData;

	AgpdItem *pcsItem = (AgpdItem *)ppvBuffer[0];
	AgpdItem *pcsConvertItem = (AgpdItem *)ppvBuffer[1];

	BOOL			bUseScroll		= FALSE;

	if (!pcsItem->m_pcsCharacter)
		return FALSE;

	if(pThis->m_pcsAgpmTitle)
	{
		INT32 nEffectSet = 0;
		INT32 nEffectValue1 = 0;
		INT32 nEffectValue2 = 0;

		pThis->m_pcsAgpmTitle->GetEffectValue(pcsItem->m_pcsCharacter, AGPMTITLE_TITLE_EFFECT_CONVERT_BONUS_RATE, &nEffectSet, &nEffectValue1, &nEffectValue2);

		if(pThis->m_pcsAgpmTitle->CheckSameItemGroup(nEffectValue1, pcsConvertItem->m_pcsItemTemplate->m_lID))
			*pfSuccessProb += (FLOAT)((FLOAT)nEffectValue2 / 100.0f);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check TPack User (If User has TPack Benefit, Add SuccessProbablity TPack Benefit)
	INT32 ulPCRoomType = 0;

	pThis->m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pcsItem->m_pcsCharacter, &ulPCRoomType);

	if(ulPCRoomType > 0)
	{
		if(ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK)
		{
			if(pThis->m_pcsAgpmConfig)
			{
				*pfSuccessProb += static_cast<FLOAT>(pThis->m_pcsAgpmConfig->GetTPackConvertRatio() / 100.0f);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	AgpdGridItem	*pcsLuckyScrollGrid	= pThis->m_pcsAgpmItem->GetUsingCashItemUsableByType(pcsItem->m_pcsCharacter, AGPMITEM_USABLE_TYPE_LUCKY_SCROLL, AGPMITEM_USABLE_SCROLL_SUBTYPE_NONE);

	if (!pcsLuckyScrollGrid)
		return FALSE;

	AgpdItem *pcsLuckyScroll = pThis->m_pcsAgpmItem->GetItem(pcsLuckyScrollGrid);

	if (!pcsLuckyScroll)
		return FALSE;

	// 2007.01.26. steeple
	// 사용중일 때만 해야 한다.
	if (pcsLuckyScroll->m_nInUseItem != AGPDITEM_CASH_ITEM_INUSE)
		return FALSE;

	*pfSuccessProb += 0.2f;

	pThis->m_pcsAgpmItem->SubItemStackCount(pcsLuckyScroll, 1, TRUE);

	return TRUE;
}

BOOL AgsmItemConvert::CBSendItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItemConvert	*pThis				= (AgsmItemConvert *)	pClass;
	AgpdItem		*pcsItem			= (AgpdItem *)			pData;
	PVOID			*ppvBuffer			= (PVOID *)				pCustData;

	UINT32			ulNID				= PtrToUint(ppvBuffer[0]);
	BOOL			bGroupNID			= PtrToInt(ppvBuffer[1]);

	AgpdItemConvertADItem	*pcsAttachData	= pThis->m_pcsAgpmItemConvert->GetADItem(pcsItem);
	if (pcsAttachData->m_lNumSocket == 0 &&
		pcsAttachData->m_lPhysicalConvertLevel == 0)
		return TRUE;

	INT16			nPacketLength		= 0;
	PVOID			pvPacket			= pThis->m_pcsAgpmItemConvert->MakePacketAdd(pcsItem, &nPacketLength);

	if (pvPacket && nPacketLength > 0)
	{
		if (pcsItem->m_pcsCharacter)
			pThis->m_pcsAgpmItemConvert->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItem->m_pcsCharacter->m_lID);

		if (bGroupNID)
			pThis->m_pcsAgsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);
		else
			pThis->SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);
	}

	pThis->m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmItemConvert::CBSendItemView(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;
	
	PROFILE("AgsmItemConvert::CBSendItemView");

	AgsmItemConvert	*pThis				= (AgsmItemConvert *)	pClass;
	AgpdItem		*pcsItem			= (AgpdItem *)			pData;

	PVOID			*ppvBuffer			= NULL;

	UINT32			ulNID				= 0;
	BOOL			bGroupNID			= FALSE;
	UINT32			ulSelfNID			= 0;

	if (pCustData)
	{
		ppvBuffer			= (PVOID *)	pCustData;

		ulNID				= PtrToUint(ppvBuffer[0]);
		bGroupNID			= PtrToInt(ppvBuffer[1]);
		ulSelfNID			= PtrToUint(ppvBuffer[2]);
	}

	AgpdItemConvertADItem	*pcsAttachData	= pThis->m_pcsAgpmItemConvert->GetADItem(pcsItem);
	if (pcsAttachData->m_lNumSocket == 0 &&
		pcsAttachData->m_lPhysicalConvertLevel == 0)
		return TRUE;

	INT16			nPacketLength		= 0;
	PVOID			pvPacket			= NULL;
	
	{
		PROFILE("AgsmItemConvert::CBSendItemView - MakePacket");

		pvPacket	= pThis->m_pcsAgpmItemConvert->MakePacketAdd(pcsItem, &nPacketLength);
	}

	{
		PROFILE("AgsmItemConvert::CBSendItemView - SendPacket");

		if (pvPacket && nPacketLength > 0)
		{
			if (pcsItem->m_pcsCharacter)
				pThis->m_pcsAgpmItemConvert->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItem->m_pcsCharacter->m_lID);

			if (pCustData)
			{
				if (bGroupNID)
				{
					if (ulSelfNID != 0)
						pThis->m_pcsAgsmAOIFilter->SendPacketGroupExceptSelf(pvPacket, nPacketLength, ulNID, ulSelfNID, PACKET_PRIORITY_4);
					else
						pThis->m_pcsAgsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);
				}
				else
					pThis->SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);
			}
			else
			{
				pThis->m_pcsAgsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsItem->m_posItem, PACKET_PRIORITY_4);
			}
		}
	}

	pThis->m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmItemConvert::CBItemConvertHistoryInsert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmItemConvert				*pThis				= (AgsmItemConvert *)			pClass;
	AgpdItem					*pcsItem			= (AgpdItem *)					pData;

	if (pThis->m_pcsAgpmItemConvert->GetNumPhysicalConvert(pcsItem) > 0 ||
		pThis->m_pcsAgpmItemConvert->GetNumSocket(pcsItem) > 0)
	{
		pThis->EnumCallback(AGSMITEMCONVERT_CB_UPDATE_CONVERT_HISTORY_TO_DB, pcsItem, NULL);
	}

	return TRUE;
}

BOOL AgsmItemConvert::CBCheckDefense(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItemConvert				*pThis				= (AgsmItemConvert *)			pClass;
	stAgsmCombatAttackResult	*pstAttackResult	= (stAgsmCombatAttackResult *)	pData;
	INT32						*plReflectDamage	= (INT32 *)						pCustData;

	return TRUE;
}


BOOL AgsmItemConvert::CBCheckSpecialStatusFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItemConvert		*pThis				= (AgsmItemConvert *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	PVOID				*ppvBuffer			= (PVOID *)				pCustData;

	UINT64				*pulSpecialStatus	= (UINT64 *)			ppvBuffer[0];
	UINT32				*pulDuration		= (UINT32 *)			ppvBuffer[2];
	INT32				*plProbability		= (INT32 *)				ppvBuffer[3];

	if (!pulSpecialStatus || !pulDuration || !plProbability)
		return FALSE;

	switch (*pulSpecialStatus) {
	case AGPDCHAR_SPECIAL_STATUS_STUN:
		{
			//*plProbability	= pThis->m_pcsAgpmItemConvert->GetReduceStunProbability(pcsCharacter);
		}
		break;

	case AGPDCHAR_SPECIAL_STATUS_SLOW:
		{
			//*pulDuration	= pThis->m_pcsAgpmItemConvert->GetReduceSlowDuration(pcsCharacter);
		}
		break;
	}

	return TRUE;
}

BOOL AgsmItemConvert::CBCheckPoisonStatusFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmItemConvert		*pThis				= (AgsmItemConvert *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	PVOID				*ppvBuffer			= (PVOID *)				pCustData;

	UINT32				*pulDuration		= (UINT32 *)			ppvBuffer[1];

	if (!pulDuration)
		return FALSE;

	//*pulDuration	= pThis->m_pcsAgpmItemConvert->GetReducePoisonDuration(pcsCharacter);

	return TRUE;
}

BOOL AgsmItemConvert::EncodeConvertHistory(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferSize)
{
	if (!pcsItem || !szBuffer || lBufferSize < 1)
		return FALSE;

	ZeroMemory(szBuffer, sizeof(CHAR) * lBufferSize);

	AgpdItemConvertADItem	*pcsAttachData	= m_pcsAgpmItemConvert->GetADItem(pcsItem);

	return m_pcsAgpmItemConvert->EncodeConvertHistory(pcsAttachData, szBuffer, lBufferSize);
}

BOOL AgsmItemConvert::DecodeConvertHistory(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferSize)
{
	if (!pcsItem || !szBuffer || lBufferSize < 1)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= m_pcsAgpmItemConvert->GetADItem(pcsItem);

	return m_pcsAgpmItemConvert->DecodeConvertHistory(pcsAttachData, szBuffer, lBufferSize, FALSE, pcsItem);
}

//BOOL AgsmItemConvert::EncodeConvertHistory(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferSize)
//{
//	if (!pcsItem || !szBuffer || lBufferSize < 1)
//		return FALSE;
//
//	ZeroMemory(szBuffer, sizeof(CHAR) * lBufferSize);
//
//	AgpdItemConvertADItem	*pcsAttachData	= m_pcsAgpmItemConvert->GetADItem(pcsItem);
//
//	sprintf(szBuffer, "%d:%d:", pcsAttachData->m_lPhysicalConvertLevel, pcsAttachData->m_lNumSocket);
//
//	for (int i = 0; i < pcsAttachData->m_lNumConvert; ++i)
//		sprintf(szBuffer + strlen(szBuffer), "%d,", pcsAttachData->m_stSocketAttr[i].lTID);
//
//	sprintf(szBuffer + strlen(szBuffer) - 1, ":");
//
//	return TRUE;
//}
//
//BOOL AgsmItemConvert::DecodeConvertHistory(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferSize)
//{
//	if (!pcsItem || !szBuffer || lBufferSize < 1)
//		return FALSE;
//
//	AgpdItemConvertADItem	*pcsAttachData	= m_pcsAgpmItemConvert->GetADItem(pcsItem);
//
//	sscanf(szBuffer, "%d:%d:", &pcsAttachData->m_lPhysicalConvertLevel, &pcsAttachData->m_lNumSocket);
//
//	INT32	lNumSeparator	= 0;
//	for (int i = 0; i < lBufferSize; ++i)
//	{
//		if (szBuffer[i] == ':')
//			++lNumSeparator;
//
//		if (lNumSeparator == 2)
//			break;
//	}
//
//	if (i == lBufferSize)
//		return FALSE;
//	else if (i == lBufferSize - 1)
//		pcsAttachData->m_lNumConvert	= 0;
//	else
//	{
//		++i;
//
//		INT32	lTID[AGPDITEMCONVERT_MAX_WEAPON_SOCKET];
//		INT32	lNumConvert	= 0;
//
//		ZeroMemory(lTID, sizeof(INT32) * AGPDITEMCONVERT_MAX_WEAPON_SOCKET);
//
//		while (i < lBufferSize)
//		{
//			sscanf(szBuffer + i, "%d", &lTID[lNumConvert++]);
//
//			for ( ; i < lBufferSize; ++i)
//			{
//				if (szBuffer[i] == ',' ||
//					szBuffer[i] == ':')
//					break;
//			}
//
//			if (i == lBufferSize)
//				break;
//			else
//				++i;
//		}
//
//		for (i = 0; i < lNumConvert; ++i)
//		{
//			AgpdItemTemplate	*pcsTemplate	= m_pcsAgpmItem->GetItemTemplate(lTID[i]);
//			if (pcsTemplate)
//			{
//				if (pcsTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
//					((AgpdItemTemplateUsable *) pcsTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
//					m_pcsAgpmItemConvert->AddSpiritStoneConvert(pcsItem, pcsTemplate);
//				else
//					m_pcsAgpmItemConvert->AddRuneConvert(pcsItem, pcsTemplate);
//			}
//		}
//	}
//
//	return TRUE;
//}

BOOL AgsmItemConvert::SendPacketAdd(AgpdItem *pcsItem, UINT32 ulNID, BOOL bGroupNID)
{
	if (!pcsItem || ulNID == 0)
		return FALSE;

	AgpdItemConvertADItem	*pcsAttachData	= m_pcsAgpmItemConvert->GetADItem(pcsItem);
	if (pcsAttachData->m_lPhysicalConvertLevel == 0 &&
		pcsAttachData->m_lNumSocket == 0)
		return TRUE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmItemConvert->MakePacketAdd(pcsItem, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= FALSE;

	if (pcsItem->m_pcsCharacter)
		m_pcsAgpmItemConvert->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItem->m_pcsCharacter->m_lID);

	if (bGroupNID)
	{
		bSendResult	= m_pcsAgsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);
	}
	else
	{
		bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);
	}

	m_pcsAgpmItemConvert->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmItemConvert::SetCallbackUpdateConvertHistoryToDB(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMITEMCONVERT_CB_UPDATE_CONVERT_HISTORY_TO_DB, pfCallback, pClass);
}

// 2005.03.15. steeple
BOOL AgsmItemConvert::WriteConvertLog(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem, INT8 cFlag, INT8 cResult, INT32 lCost)
{
	if(!m_pcsAgpmLog)
		return TRUE;

	if(!pcsCharacter || !pcsItem)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	AgsdItem* pcsAgsdItem = m_pcsAgsmItem->GetADItem(pcsItem);
	if(!pcsAgsdCharacter || !pcsAgsdItem)
		return FALSE;

	AgpdItemConvertADItem* pcsItemConvertADItem = m_pcsAgpmItemConvert->GetADItem(pcsItem);
	if(!pcsItemConvertADItem)
		return FALSE;

	CHAR szConvert[AGPDLOG_MAX_ITEM_CONVERT_STRING+1];
	ZeroMemory(szConvert, sizeof(szConvert));
	m_pcsAgpmItemConvert->EncodeConvertHistory(pcsItemConvertADItem, szConvert, AGPDLOG_MAX_ITEM_CONVERT_STRING);

	CHAR szOption[AGPDLOG_MAX_ITEM_FULL_OPTION + 1];
	ZeroMemory(szOption, sizeof(szOption));

	CHAR szNormalOption[AGPDLOG_MAX_ITEM_OPTION+1];
	ZeroMemory(szNormalOption, sizeof(szNormalOption));
	m_pcsAgsmItem->EncodingOption(pcsItem, szNormalOption, AGPDLOG_MAX_ITEM_OPTION);

	//##########################
	CHAR szSkillPlus[AGPDLOG_MAX_ITEM_OPTION+1];
	ZeroMemory(szSkillPlus, sizeof(szSkillPlus));
	m_pcsAgsmItem->EncodingSkillPlus(pcsItem, szSkillPlus, AGPDLOG_MAX_ITEM_OPTION);

	sprintf(szOption, "%s::%s", szNormalOption, szSkillPlus);

	CHAR szPosition[33];
	ZeroMemory(szPosition, sizeof(szPosition));
	m_pcsAgsmCharacter->EncodingPosition(&pcsCharacter->m_stPos, szPosition, 32);

	eAGPDLOGTYPE_ITEM eType = AGPDLOGTYPE_ITEM_SM1_SC;

	switch(cFlag)
	{
		case 1:		// Physical Convert
			switch((AgpdItemConvertResult)cResult)
			{
				case AGPDITEMCONVERT_RESULT_SUCCESS:
					eType = AGPDLOGTYPE_ITEM_SM1_SC;
					break;
				case AGPDITEMCONVERT_RESULT_FAILED:
					eType = AGPDLOGTYPE_ITEM_SM1_NC;
					break;
				case AGPDITEMCONVERT_RESULT_FAILED_AND_DESTROY:
					eType = AGPDLOGTYPE_ITEM_SM1_DT;
					break;				
				case AGPDITEMCONVERT_RESULT_FAILED_AND_INIT_SAME:
					eType = AGPDLOGTYPE_ITEM_SM1_IN;
					break;
				default:
					return FALSE;
			}
		break;

		case 2:		// SpiritStone
			switch((AgpdItemConvertSpiritStoneResult)cResult)
			{
				case AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS:
					eType = AGPDLOGTYPE_ITEM_SM2_SC;
					break;
				case AGPDITEMCONVERT_SPIRITSTONE_RESULT_FAILED:
					eType = AGPDLOGTYPE_ITEM_SM2_FL;
					break;
				default:
					return FALSE;
			}
		break;

		case 3:		// Rune
			switch((AgpdItemConvertRuneResult)cResult)
			{
				case AGPDITEMCONVERT_RUNE_RESULT_SUCCESS:
					eType = AGPDLOGTYPE_ITEM_SM3_SC;
					break;
				case AGPDITEMCONVERT_RUNE_RESULT_FAILED:
					eType = AGPDLOGTYPE_ITEM_SM3_NC;
					break;
				case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT_SAME:
					eType = AGPDLOGTYPE_ITEM_SM3_IN;
					break;
				case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_INIT:
					eType = AGPDLOGTYPE_ITEM_SM3_II;
					break;
				case AGPDITEMCONVERT_RUNE_RESULT_FAILED_AND_DESTROY:
					eType = AGPDLOGTYPE_ITEM_SM3_DT;
					break;							

				default:
					return FALSE;
			}
		break;
		/*
		case 4:		// Socket
			switch((AgpdItemConvertSocketResult)cResult)
			{
				case AGPDITEMCONVERT_SOCKET_RESULT_SUCCESS:
				default:
					break;
			}
		*/

		case 5:
			eType = AGPDLOGTYPE_ITEM_SOCKET_INIT;
			break;

		default:
			return FALSE;
	}


	return m_pcsAgpmLog->WriteLog_Item(FALSE,
									eType,
									0,
									&pcsAgsdCharacter->m_strIPAddress[0],
									pcsAgsdCharacter->m_szAccountID,
									pcsAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID,
									m_pcsAgpmCharacter->GetLevel(pcsCharacter),
									m_pcsAgpmCharacter->GetExp(pcsCharacter),
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									pcsAgsdItem->m_ullDBIID,
									pcsItem->m_pcsItemTemplate->m_lID,
									1,
									szConvert,
									szOption,
									lCost,
									NULL,
									pcsItem->m_nInUseItem,
									pcsItem->m_lCashItemUseCount,
									pcsItem->m_lRemainTime,
									pcsItem->m_lExpireTime,
									pcsItem->m_llStaminaRemainTime,
									m_pcsAgpmItem->GetItemDurabilityCurrent(pcsItem),
									m_pcsAgpmItem->GetItemDurabilityMax(pcsItem),
									szPosition
									);
}