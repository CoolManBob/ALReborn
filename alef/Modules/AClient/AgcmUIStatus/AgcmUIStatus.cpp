#include "AgcmUIStatus.h"

AgcmUIStatus::AgcmUIStatus()
{
	SetModuleName("AgcmUIStatus");

	EnableIdle(TRUE);

	m_ulUpdateAutoUseHPGageTimeMSec	= 0;
	m_ulUpdateAutoUseMPGageTimeMSec	= 0;
}

AgcmUIStatus::~AgcmUIStatus()
{
}

BOOL AgcmUIStatus::OnAddModule()
{
	m_pcsApmEventManager		= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pcsAgpmSkill				= (AgpmSkill *)				GetModule("AgpmSkill");
	m_pcsAgpmUIStatus			= (AgpmUIStatus *)			GetModule("AgpmUIStatus");
	m_pcsAgcmCharacter			= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pcsAgcmUIMain				= (AgcmUIMain *)			GetModule("AgcmUIMain");
	m_pcsAgcmItem				= (AgcmItem *)				GetModule("AgcmItem");

	if (!m_pcsApmEventManager ||
		!m_pcsAgpmSkill ||
		!m_pcsAgpmUIStatus ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmUIMain ||
		!m_pcsAgcmItem)
		return FALSE;

	if (!m_pcsAgpmUIStatus->SetCallbackAddQuickBeltInformation(CBReceiveQuickBeltInfo, this))
		return FALSE;

	if (!m_pcsAgpmUIStatus->SetCallbackUpdateViewHelmetOption(CBUpdateViewHelmetOption, this))
		return FALSE;

	if (!m_pcsAgcmUIMain->SetCallbackAddShortcut(CBAddShortcut, this))
		return FALSE;
	if (!m_pcsAgcmUIMain->SetCallbackRemoveShortcut(CBRemoveShortcut, this))
		return FALSE;

	if (!m_pcsAgcmUIMain->SetCallbackUpdateHPPotion(CBUpdateHPPotion, this))
		return FALSE;
	if (!m_pcsAgcmUIMain->SetCallbackUpdateMPPotion(CBUpdateMPPotion, this))
		return FALSE;

	if (!m_pcsAgcmUIMain->SetCallbackUpdateHPPointGage(CBUpdateAutoUseHPGage, this))
		return FALSE;
	if (!m_pcsAgcmUIMain->SetCallbackUpdateMPPointGage(CBUpdateAutoUseMPGage, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIStatus::OnInit()
{
	return TRUE;
}

BOOL AgcmUIStatus::OnDestroy()
{
	return TRUE;
}

BOOL AgcmUIStatus::OnIdle(UINT32 ulClockCount)
{
	if (m_ulUpdateAutoUseHPGageTimeMSec > 0 &&
		m_ulUpdateAutoUseHPGageTimeMSec + 20000 < ulClockCount)
	{
		SendPacketUpdateQBeltItem(m_pcsAgcmCharacter->GetSelfCharacter(),
								(-1), NULL,
								AP_INVALID_IID,
								AP_INVALID_IID,
								(INT8) m_pcsAgcmUIMain->m_lHPPointScrollBarValue,
								(-1));

		m_ulUpdateAutoUseHPGageTimeMSec	= 0;
	}

	if (m_ulUpdateAutoUseMPGageTimeMSec > 0 &&
		m_ulUpdateAutoUseMPGageTimeMSec + 20000 < ulClockCount)
	{
		SendPacketUpdateQBeltItem(m_pcsAgcmCharacter->GetSelfCharacter(),
								(-1), NULL,
								AP_INVALID_IID,
								AP_INVALID_IID,
								(-1),
								(INT8) m_pcsAgcmUIMain->m_lMPPointScrollBarValue);

		m_ulUpdateAutoUseMPGageTimeMSec	= 0;
	}

	return TRUE;
}

BOOL AgcmUIStatus::CBUpdateViewHelmetOption(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIStatus		*pThis				= (AgcmUIStatus *)		pClass;
	AgpdCharacter		*pcsAgpdCharacter	= (AgpdCharacter *)		pData;
	INT8				*pnOptionViewHelmet	= (INT8 *)				pCustData;

	AgcdCharacter		*pcsAgcdCharacter	= pThis->m_pcsAgcmCharacter->GetCharacterData( pcsAgpdCharacter );

	pThis->m_pcsAgcmItem->ViewHelmet( pcsAgcdCharacter, (*pnOptionViewHelmet) != 0 );

	return	TRUE;
}

BOOL AgcmUIStatus::CBReceiveQuickBeltInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIStatus		*pThis				= (AgcmUIStatus *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	if (pcsCharacter != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return FALSE;

	AgpdUIStatusADChar	*pcsAttachData		= pThis->m_pcsAgpmUIStatus->GetADCharacter(pcsCharacter);

	for (int nLayer	= 0; nLayer < AGPMUISTATUS_MAX_QUICKBELT_LAYER; ++nLayer)
	{
		int		nColumn		= 0;

		for (int i = nLayer * AGPMUISTATUS_MAX_QUICKBELT_COLUMN; i < nLayer * AGPMUISTATUS_MAX_QUICKBELT_COLUMN + AGPMUISTATUS_HALF_QUICKBELT_COLUMN; ++i)
		{
			BOOL	bIsAdded	= FALSE;

			if (pcsAttachData->m_acsQBeltItem[i].m_eType != APBASE_TYPE_NONE &&
				pcsAttachData->m_acsQBeltItem[i].m_lID != 0)
			{
				ApBase		*pcsBase	= pThis->m_pcsAgpmSkill->GetBase(pcsAttachData->m_acsQBeltItem[i].m_eType, pcsAttachData->m_acsQBeltItem[i].m_lID);
				if (pcsBase)
				{
					switch (pcsBase->m_eType) {
					case APBASE_TYPE_ITEM:
						{
							bIsAdded	= pThis->m_pcsAgcmUIMain->AddShortcut(&pThis->m_pcsAgcmUIMain->m_astQuickBeltGrid[0], ((AgpdItem *) pcsBase)->m_pcsGridItem, nLayer, 0, nColumn, FALSE);
						}
						break;

					case APBASE_TYPE_SKILL:
						{
							bIsAdded	= pThis->m_pcsAgcmUIMain->AddShortcut(&pThis->m_pcsAgcmUIMain->m_astQuickBeltGrid[0], ((AgpdSkill *) pcsBase)->m_pcsGridItem, nLayer, 0, nColumn, FALSE);
						}
						break;
					}
				}
			}

			if (!bIsAdded)
			{
				pThis->m_pcsAgcmUIMain->RemoveShortcut(&pThis->m_pcsAgcmUIMain->m_astQuickBeltGrid[0], nLayer, 0, nColumn);
			}

			++nColumn;
		}

		nColumn	= 0;

		for (int i = nLayer * AGPMUISTATUS_MAX_QUICKBELT_COLUMN + AGPMUISTATUS_HALF_QUICKBELT_COLUMN; i < (nLayer + 1) * AGPMUISTATUS_MAX_QUICKBELT_COLUMN; ++i)
		{
			BOOL	bIsAdded	= FALSE;

			if (pcsAttachData->m_acsQBeltItem[i].m_eType != APBASE_TYPE_NONE &&
				pcsAttachData->m_acsQBeltItem[i].m_lID != 0)
			{
				ApBase		*pcsBase	= pThis->m_pcsAgpmSkill->GetBase(pcsAttachData->m_acsQBeltItem[i].m_eType, pcsAttachData->m_acsQBeltItem[i].m_lID);
				if (pcsBase)
				{
					switch (pcsBase->m_eType) {
					case APBASE_TYPE_ITEM:
						{
							bIsAdded	= pThis->m_pcsAgcmUIMain->AddShortcut(&pThis->m_pcsAgcmUIMain->m_astQuickBeltGrid[1], ((AgpdItem *) pcsBase)->m_pcsGridItem, nLayer, 0, nColumn, FALSE);
						}
						break;

					case APBASE_TYPE_SKILL:
						{
							bIsAdded	= pThis->m_pcsAgcmUIMain->AddShortcut(&pThis->m_pcsAgcmUIMain->m_astQuickBeltGrid[1], ((AgpdSkill *) pcsBase)->m_pcsGridItem, nLayer, 0, nColumn, FALSE);
						}
						break;
					}
				}
			}

			if (!bIsAdded)
			{
				pThis->m_pcsAgcmUIMain->RemoveShortcut(&pThis->m_pcsAgcmUIMain->m_astQuickBeltGrid[1], nLayer, 0, nColumn);
			}

			++nColumn;
		}
	}

	if (pcsAttachData->m_lHPPotionTID != AP_INVALID_IID)
		pThis->m_pcsAgcmUIMain->AddHPPotion(pcsAttachData->m_lHPPotionTID);

	if (pcsAttachData->m_lMPPotionTID != AP_INVALID_IID)
		pThis->m_pcsAgcmUIMain->AddMPPotion(pcsAttachData->m_lMPPotionTID);

	if (pcsAttachData->m_cAutoUseHPGage >= 0 && pcsAttachData->m_cAutoUseHPGage <= 100)
		pThis->m_pcsAgcmUIMain->SetHPPointScrollBarValue((INT32) pcsAttachData->m_cAutoUseHPGage);
	if (pcsAttachData->m_cAutoUseMPGage >= 0 && pcsAttachData->m_cAutoUseMPGage <= 100)
		pThis->m_pcsAgcmUIMain->SetMPPointScrollBarValue((INT32) pcsAttachData->m_cAutoUseMPGage);

	return TRUE;
}

BOOL AgcmUIStatus::CBAddShortcut(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIStatus		*pThis				= (AgcmUIStatus *)		pClass;
	PVOID				*ppvBuffer			= (PVOID *)				pData;

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	AgpdGrid			*pcsGrid			= (AgpdGrid *)			ppvBuffer[0];
	AgpdGridItem		*pcsGridItem		= (AgpdGridItem *)		ppvBuffer[1];
	INT32				lLayer				= (INT32)				ppvBuffer[2];
	INT32				lRow				= (INT32)				ppvBuffer[3];
	INT32				lColumn				= (INT32)				ppvBuffer[4];

//	if (&pThis->m_pcsAgcmUIMain->m_astQuickBeltGrid[0] == pcsGrid ||
//		&pThis->m_pcsAgcmUIMain->m_astVQuickBeltGrid[0] == pcsGrid)
	if (&pThis->m_pcsAgcmUIMain->m_astQuickBeltGrid[0] == pcsGrid)
	{
		ApBase			*pcsBaseItem		= NULL;

		switch (pcsGridItem->m_eType) {
		case AGPDGRID_ITEM_TYPE_ITEM:
			pcsBaseItem	= pThis->m_pcsApmEventManager->GetBase(APBASE_TYPE_ITEM, pcsGridItem->m_lItemID);
			break;

		case AGPDGRID_ITEM_TYPE_SKILL:
			pcsBaseItem	= pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
			break;
		}

		pThis->SendPacketUpdateQBeltItem(pcsSelfCharacter, lLayer * AGPMUISTATUS_MAX_QUICKBELT_COLUMN + lColumn, pcsBaseItem);
	}
	else
	{
		ApBase			*pcsBaseItem		= NULL;

		switch (pcsGridItem->m_eType) {
		case AGPDGRID_ITEM_TYPE_ITEM:
			pcsBaseItem	= pThis->m_pcsApmEventManager->GetBase(APBASE_TYPE_ITEM, pcsGridItem->m_lItemID);
			break;

		case AGPDGRID_ITEM_TYPE_SKILL:
			pcsBaseItem	= pThis->m_pcsAgpmSkill->GetSkill(pcsGridItem->m_lItemID);
			break;
		}

		pThis->SendPacketUpdateQBeltItem(pcsSelfCharacter, lLayer * AGPMUISTATUS_MAX_QUICKBELT_COLUMN + lColumn + AGPMUISTATUS_HALF_QUICKBELT_COLUMN, pcsBaseItem);
	}

	return TRUE;
}

BOOL AgcmUIStatus::CBRemoveShortcut(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIStatus		*pThis				= (AgcmUIStatus *)		pClass;
	PVOID				*ppvBuffer			= (PVOID *)				pData;

	AgpdGrid			*pcsGrid			= (AgpdGrid *)			ppvBuffer[0];
	INT32				lLayer				= (INT32)				ppvBuffer[2];
	INT32				lRow				= (INT32)				ppvBuffer[3];
	INT32				lColumn				= (INT32)				ppvBuffer[4];
	BOOL				bIsCleared			= (BOOL)				ppvBuffer[5];

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (bIsCleared)
	{
	//	if (&pThis->m_pcsAgcmUIMain->m_astQuickBeltGrid[0] == pcsGrid ||
	//		&pThis->m_pcsAgcmUIMain->m_astVQuickBeltGrid[0] == pcsGrid)
		if (&pThis->m_pcsAgcmUIMain->m_astQuickBeltGrid[0] == pcsGrid)
		{
			pThis->SendPacketUpdateQBeltItem(pcsSelfCharacter, lLayer * AGPMUISTATUS_MAX_QUICKBELT_COLUMN + lColumn, NULL);
		}
		else
		{
			pThis->SendPacketUpdateQBeltItem(pcsSelfCharacter, lLayer * AGPMUISTATUS_MAX_QUICKBELT_COLUMN + lColumn + AGPMUISTATUS_HALF_QUICKBELT_COLUMN, NULL);
		}
	}

	return TRUE;
}

BOOL AgcmUIStatus::CBUpdateHPPotion(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIStatus	*pThis			= (AgcmUIStatus *)	pClass;
	INT32			*plHPPotionTID	= (INT32 *)			pData;

	if (*plHPPotionTID != AP_INVALID_IID)
		pThis->SendPacketUpdateQBeltItem(pThis->m_pcsAgcmCharacter->GetSelfCharacter(),
										 (-1), NULL,
										 *plHPPotionTID,
										 AP_INVALID_IID);

	return TRUE;
}

BOOL AgcmUIStatus::CBUpdateMPPotion(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIStatus	*pThis			= (AgcmUIStatus *)	pClass;
	INT32			*plMPPotionTID	= (INT32 *)			pData;

	if (*plMPPotionTID != AP_INVALID_IID)
		pThis->SendPacketUpdateQBeltItem(pThis->m_pcsAgcmCharacter->GetSelfCharacter(),
										 (-1), NULL,
										 AP_INVALID_IID,
										 *plMPPotionTID);

	return TRUE;
}

BOOL AgcmUIStatus::CBUpdateAutoUseHPGage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIStatus	*pThis				= (AgcmUIStatus *)	pClass;

	if (pThis->m_ulUpdateAutoUseHPGageTimeMSec > 0)
		return TRUE;

	pThis->m_ulUpdateAutoUseHPGageTimeMSec	= pThis->GetClockCount();

	return TRUE;
}

BOOL AgcmUIStatus::CBUpdateAutoUseMPGage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIStatus	*pThis				= (AgcmUIStatus *)	pClass;

	if (pThis->m_ulUpdateAutoUseMPGageTimeMSec > 0)
		return TRUE;

	pThis->m_ulUpdateAutoUseMPGageTimeMSec	= pThis->GetClockCount();

	return TRUE;
}

BOOL AgcmUIStatus::SendPacketUpdateQBeltItem(AgpdCharacter *pcsCharacter, INT32 lIndex, ApBase *pcsItemBase, INT32 lHPPotionTID, INT32 lMPPotionTID, INT8 cAutoUseHPGage, INT8 cAutoUseMPGage)
{
	if (!pcsCharacter || lIndex < (-1) || lIndex >= AGPMUISTATUS_MAX_QUICKBELT_LAYER * AGPMUISTATUS_MAX_QUICKBELT_COLUMN)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmUIStatus->MakePacketUpdateQuickBeltItem(pcsCharacter, lIndex, pcsItemBase, &nPacketLength, lHPPotionTID, lMPPotionTID, cAutoUseHPGage, cAutoUseMPGage);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmUIStatus->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmUIStatus::SendPacketUpdateViewHelmetOption(AgpdCharacter *pcsCharacter, INT8 cOptionViewHelmet)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmUIStatus->MakePacketUpdateViewHelmetOption(pcsCharacter->m_lID, &nPacketLength, cOptionViewHelmet);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmUIStatus->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}