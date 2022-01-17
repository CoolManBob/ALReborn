#include "AgsmItem.h"


//		MakePacketItemRemove
//	Functions
//		- pcsItem을 삭제한다는 패킷을 만들어 리턴한다.
//			주의) 리턴된 패킷은 사용후 꼬옥~~~ FreePacket() 해줘야 한다.
//	Arguments
//		- pcsItem : 아템 데이타 포인터
//		- pnPacketLength : 만들어진 패킷 길이
//	Return value
//		- PVOID : 만든 패킷 포인터
///////////////////////////////////////////////////////////////////////////////
PVOID AgsmItem::MakePacketItemRemove(AgpdItem *pcsItem, INT16	*pnPacketLength)
{
	if (!pcsItem)
		return NULL;

	INT8	nOperation = AGPMITEM_PACKET_OPERATION_REMOVE;

	PVOID pvPacket = m_pagpmItem->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
												  &nOperation,			//Operation
												  NULL,					//Status
												  &pcsItem->m_lID,		//ItemID
												  NULL,					//ItemTemplateID
												  &pcsItem->m_ulCID,					//ItemOwnerID
												  NULL,					//ItemCount
												  NULL,					//Field
												  NULL,					//Inventory
												  NULL,					//Bank
												  NULL,					//Equip
												  NULL, 				//Factors
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
												  NULL,					// CashInformation
												  NULL);

	return pvPacket;
}

PVOID AgsmItem::MakePacketItemUpdateStatus(AgpdItem *pcsItem, INT16 *pnPacketLength)
{
	if (!pcsItem || !pnPacketLength)
		return NULL;

	INT8	nOperation	= AGPMITEM_PACKET_OPERATION_UPDATE;
	INT8 eStatus = (INT8)pcsItem->m_eStatus;

	PVOID	pvPacket	= m_pagpmItem->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
													&nOperation,			//Operation
													&eStatus,	//Status
													&pcsItem->m_lID,		//ItemID
													NULL,					//ItemTemplateID
													&pcsItem->m_ulCID,					//ItemOwnerID
													NULL,					//ItemCount
													NULL,					//Field
													NULL,					//Inventory
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
													NULL,					// CashInformation
													NULL);

	return pvPacket;
}

PVOID AgsmItem::MakePacketItemBank(AgpdItem *pcsItem, INT16 *pnPacketLength)
{
	if (!pcsItem || !pnPacketLength)
		return NULL;

	PVOID	pvPacketBank	= m_pagpmItem->m_csPacketBank.MakePacket(FALSE, pnPacketLength, 0,
																	&pcsItem->m_anGridPos[0],
																	&pcsItem->m_anGridPos[1],
																	&pcsItem->m_anGridPos[2]);

	if (!pvPacketBank)
		return NULL;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_UPDATE;
	INT8 eStatus = (INT8)pcsItem->m_eStatus;

	PVOID	pvPacket		= m_pagpmItem->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
													&cOperation,
													&eStatus,
													&pcsItem->m_lID,
													NULL,
													&pcsItem->m_ulCID,
													&pcsItem->m_nCount,
													NULL,
													NULL,
													pvPacketBank,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,	// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,					// SkillPlus
													NULL,
													NULL,					// CashInformation
													NULL);

	m_pagpmItem->m_csPacket.FreePacket(pvPacketBank);

	return pvPacket;
}

PVOID AgsmItem::MakePacketItemQuest(AgpdItem *pcsItem, INT16 *pnPacketLength)
{
	if (!pcsItem || !pnPacketLength)
		return NULL;

	PVOID	pvPacketQuest	= m_pagpmItem->m_csPacketQuest.MakePacket(FALSE, pnPacketLength, 0,
																	&pcsItem->m_anGridPos[0],
																	&pcsItem->m_anGridPos[1],
																	&pcsItem->m_anGridPos[2]);

	if (!pvPacketQuest)
		return NULL;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_UPDATE;
	INT8 eStatus = (INT8)pcsItem->m_eStatus;

	PVOID	pvPacket		= m_pagpmItem->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
													&cOperation,
													&eStatus,
													&pcsItem->m_lID,
													NULL,
													&pcsItem->m_ulCID,
													&pcsItem->m_nCount,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													pvPacketQuest,	// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,					// SkillPlus
													NULL,
													NULL,					// CashInformation
													NULL);

	m_pagpmItem->m_csPacketQuest.FreePacket(pvPacketQuest);

	return pvPacket;
}

PVOID AgsmItem::MakePacketItemServerData(AgpdItem *pcsItem, INT16 *pnPacketLength, BOOL bLogin)
{
	if (!pcsItem || !pnPacketLength)
		return NULL;

	AgsdItem	*pcsAgsdItem = GetADItem(pcsItem);
	if (!pcsAgsdItem)
		return NULL;

	INT16	nPacketLength = 0;

	PVOID	pvPacketItem = NULL;

	if (bLogin)
	{
		pvPacketItem = m_csPacketData.MakePacket(FALSE, &nPacketLength, 0,
										&pcsItem->m_lID,
										&pcsAgsdItem->m_ullDBIID,
										&pcsAgsdItem->m_ulOwnTime,
										NULL,
										NULL,
										&pcsAgsdItem->m_ulDropTime,
										NULL,
										NULL
										);
	}
	else
	{
		PVOID	pvPacketFirstLooterBase = m_pagsmCharacter->MakePacketBase((ApBase *) &pcsAgsdItem->m_csFirstLooterBase);

		pvPacketItem = m_csPacketData.MakePacket(FALSE, &nPacketLength, 0,
										&pcsItem->m_lID,
										&pcsAgsdItem->m_ullDBIID,
										&pcsAgsdItem->m_ulOwnTime,
										&pcsAgsdItem->m_lUseItemRemain,
										&pcsAgsdItem->m_ulNextUseItemTime,
										&pcsAgsdItem->m_ulDropTime,
										pvPacketFirstLooterBase,
										&pcsAgsdItem->m_bIsNeedInsertDB
										);

		if (pvPacketFirstLooterBase)
			m_pagsmCharacter->m_csPacketBase.FreePacket(pvPacketFirstLooterBase);
	}

	if (!pvPacketItem)
		return NULL;

	UINT32	ulClockCount	= GetClockCount();

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMITEM_PACKET_TYPE,
										pvPacketItem,
										NULL,
										&ulClockCount,
										NULL,
										NULL,
										NULL
										);

	m_csPacketData.FreePacket(pvPacketItem);

	return pvPacket;
}

PVOID AgsmItem::MakePacketItemADCharacter(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;
	
	AgpdItemADChar	*pcsAgpdItemADChar	= m_pagpmItem->GetADCharacter(pcsCharacter);
	if (!pcsAgpdItemADChar)
		return NULL;

	AgsdItemADChar	*pcsItemADChar = GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return NULL;

//	INT32	alUseItem[AGPMITEM_MAX_USE_ITEM];
//	ZeroMemory(alUseItem, sizeof(INT32) * AGPMITEM_MAX_USE_ITEM);

	ApSafeArray<INT32, AGPMITEM_MAX_USE_ITEM>	alUseItem;
	alUseItem.MemSetAll();

	int i = 0;
	for (i = 0; i < pcsItemADChar->m_lNumUseItem; ++i)
	{
		alUseItem[i] = pcsItemADChar->m_lUseItemID[i];
	}

	PVOID	palUseItem = NULL;

	if (i > 0)
		palUseItem = &alUseItem[0];

	UINT32	ulUseReverseOrbTime	= 0;

	if (pcsAgpdItemADChar->m_ulUseReverseOrbTime != 0)
	{
		AgpdItemTemplate	*pcsItemTemplate	= m_pagpmItem->GetItemTemplate(1054);
		if (pcsItemTemplate)
		{
			ulUseReverseOrbTime	= ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_ulUseInterval - (GetClockCount() - pcsAgpdItemADChar->m_ulUseReverseOrbTime);
		}
	}

	UINT32	ulUseTransformTime	= 0;

	if (pcsAgpdItemADChar->m_ulUseTransformTime != 0)
	{
		INT32	lUseTransformTime	= AGPMITEM_TRANSFORM_RECAST_TIME - (GetClockCount() - pcsAgpdItemADChar->m_ulUseTransformTime);

		if (lUseTransformTime > 0)
			ulUseTransformTime	= (UINT32) lUseTransformTime;
		else
			ulUseTransformTime	= 0;
	}

	PVOID pvPacketADChar = m_csPacketADChar.MakePacket(FALSE, pnPacketLength, 0,
														&pcsCharacter->m_lID,
														palUseItem,
														(pcsItemADChar->m_lNumUseItem > 0) ? &pcsItemADChar->m_lNumUseItem : NULL,
														(pcsAgpdItemADChar->m_ulUseHPPotionTime > 0) ? &pcsAgpdItemADChar->m_ulUseHPPotionTime : NULL,
														(pcsAgpdItemADChar->m_ulUseMPPotionTime > 0) ? &pcsAgpdItemADChar->m_ulUseMPPotionTime : NULL,
														(pcsAgpdItemADChar->m_ulUseSPPotionTime > 0) ? &pcsAgpdItemADChar->m_ulUseSPPotionTime : NULL,
														(ulUseReverseOrbTime > 0) ? &ulUseReverseOrbTime : NULL,
														(ulUseTransformTime > 0) ? &ulUseTransformTime : NULL
														);

	if (!pvPacketADChar)
		return NULL;

	UINT32	ulClockCount	= GetClockCount();

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMITEM_PACKET_TYPE,
											NULL,
											pvPacketADChar,
											&ulClockCount,
											NULL,
											NULL,
											NULL);

	m_csPacketADChar.FreePacket(pvPacketADChar);

	return pvPacket;
}

PVOID AgsmItem::MakePacketAgpdDBData(AgpdItem *pcsItem, INT16 *pnPacketLength)
{
	if (!pcsItem || !pnPacketLength)
		return NULL;

	PVOID	pvPacketField			= NULL;
	PVOID	pvPacketInventory		= NULL;
	PVOID	pvPacketBank			= NULL;
	PVOID	pvPacketQuest			= NULL;
	PVOID	pvPacketOption			= NULL;
	PVOID	pvPacketSkillPlus		= NULL;
	PVOID	pvPacketCashInformation	= NULL;

	switch (pcsItem->m_eStatus) {
	case AGPDITEM_STATUS_FIELD:
		{
			pvPacketField		= m_pagpmItem->m_csPacketField.MakePacket(FALSE, pnPacketLength, 0,
																		&(pcsItem->m_posItem));
		}
		break;

	case AGPDITEM_STATUS_INVENTORY:
		{
			pvPacketInventory	= m_pagpmItem->m_csPacketInventory.MakePacket(FALSE, pnPacketLength, 0,
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN]);
		}
		break;

	case AGPDITEM_STATUS_BANK:
		{
			pvPacketBank		= m_pagpmItem->m_csPacketBank.MakePacket(FALSE, pnPacketLength, 0,
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN]);
		}
		break;

	case AGPDITEM_STATUS_QUEST:
		{
			pvPacketQuest		= m_pagpmItem->m_csPacketQuest.MakePacket(FALSE, pnPacketLength, 0,
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN]);
		}
		break;

	//SalesBox Grid는 인벤과같다.
	case AGPDITEM_STATUS_SALESBOX_GRID:
		{
			pvPacketInventory	= m_pagpmItem->m_csPacketInventory.MakePacket(FALSE, pnPacketLength, 0,
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
																		&pcsItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN]);
		}
		break;
	}

	pvPacketOption	= m_pagpmItem->m_csPacketOption.MakePacket(FALSE, pnPacketLength, 0,
													(pcsItem->m_aunOptionTID[0] != 0) ? &pcsItem->m_aunOptionTID[0] : NULL,
													(pcsItem->m_aunOptionTID[1] != 0) ? &pcsItem->m_aunOptionTID[1] : NULL,
													(pcsItem->m_aunOptionTID[2] != 0) ? &pcsItem->m_aunOptionTID[2] : NULL,
													(pcsItem->m_aunOptionTID[3] != 0) ? &pcsItem->m_aunOptionTID[3] : NULL,
													(pcsItem->m_aunOptionTID[4] != 0) ? &pcsItem->m_aunOptionTID[4] : NULL);

	pvPacketSkillPlus	= m_pagpmItem->m_csPacketSkillPlus.MakePacket(FALSE, pnPacketLength, 0,
													(pcsItem->m_aunSkillPlusTID[0] != 0) ? &pcsItem->m_aunSkillPlusTID[0] : NULL,
													(pcsItem->m_aunSkillPlusTID[1] != 0) ? &pcsItem->m_aunSkillPlusTID[1] : NULL,
													(pcsItem->m_aunSkillPlusTID[2] != 0) ? &pcsItem->m_aunSkillPlusTID[2] : NULL);

	if(pcsItem->m_eStatus == AGPDITEM_STATUS_CASH_INVENTORY || pcsItem->m_lExpireTime > 0)
	{
		pvPacketCashInformation = m_pagpmItem->m_csPacketCashInformaion.MakePacket(FALSE, pnPacketLength, 0,
																&pcsItem->m_nInUseItem, &pcsItem->m_lRemainTime, 
																&pcsItem->m_lExpireTime ,&pcsItem->m_lCashItemUseCount, 
																&pcsItem->m_llStaminaRemainTime);
	}

	INT8	cOperation			= AGPMITEM_PACKET_OPERATION_ADD;
	INT8 eStatus = (INT8)pcsItem->m_eStatus;

	PVOID	pvPacketItem		= m_pagpmItem->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
																		&cOperation,
																		&eStatus,
																		&pcsItem->m_lID,
																		&pcsItem->m_lTID,
																		(pcsItem->m_ulCID != AP_INVALID_CID) ? &pcsItem->m_ulCID : NULL,
																		(pcsItem->m_nCount) ? &pcsItem->m_nCount : NULL,
																		pvPacketField,
																		pvPacketInventory,
																		pvPacketBank,
																		NULL,
																		NULL,
																		NULL,
																		NULL,
																		NULL,
																		NULL,
																		NULL,
																		pvPacketQuest,
																		(pcsItem->m_lSkillTID != AP_INVALID_SKILLID) ? &pcsItem->m_lSkillTID : NULL,
																		NULL,
																		&pcsItem->m_lStatusFlag,
																		pvPacketOption,
																		pvPacketSkillPlus,
																		NULL,
																		pvPacketCashInformation,
																		NULL);

	if (pvPacketField)
		m_pagpmItem->m_csPacket.FreePacket(pvPacketField);
	if (pvPacketInventory)
		m_pagpmItem->m_csPacket.FreePacket(pvPacketInventory);
	if (pvPacketBank)
		m_pagpmItem->m_csPacket.FreePacket(pvPacketBank);
	if (pvPacketQuest)
		m_pagpmItem->m_csPacketQuest.FreePacket(pvPacketQuest);
	if (pvPacketOption)
		m_pagpmItem->m_csPacketOption.FreePacket(pvPacketOption);
	if (pvPacketCashInformation)
		m_pagpmItem->m_csPacketOption.FreePacket(pvPacketCashInformation);
	

	return pvPacketItem;
}

PVOID AgsmItem::MakePacketAgsdDBData(AgpdItem *pcsItem, INT16 *pnPacketLength)
{
	if (!pcsItem || !pnPacketLength)
		return NULL;

	AgsdItem	*pcsAgsdItem	= GetADItem(pcsItem);
	if (!pcsAgsdItem)
		return NULL;

	PVOID		pvPacketData	= m_csPacketData.MakePacket(FALSE, pnPacketLength, 0,
															&pcsItem->m_lID,
															&pcsAgsdItem->m_ullDBIID,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL,
															&pcsAgsdItem->m_bIsNeedInsertDB);

	PVOID		pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMITEM_PACKET_TYPE,
															pvPacketData,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL);

	if (pvPacketData)
		m_csPacketData.FreePacket(pvPacketData);

	return		pvPacket;
}

/*
BOOL AgsmItem::SendPacketBasicDBData(AgpdItem *pcsItem, DPNID dpnid, BOOL bGroupNID)
{
	if (!pcsItem || dpnid == 0)
		return FALSE;

	INT16	nAgpdPacketLength	= 0;
	INT16	nAgsdPacketLength	= 0;

	PVOID	pvPacketAgpdData	= MakePacketAgpdDBData(pcsItem, &nAgpdPacketLength);
	PVOID	pvPacketAgsdData	= MakePacketAgsdDBData(pcsItem, &nAgsdPacketLength);

	if (!pvPacketAgpdData || 
		!pvPacketAgsdData ||
		nAgpdPacketLength < 1 ||
		nAgsdPacketLength < 1)
	{
		if (pvPacketAgpdData)
			m_csPacket.FreePacket(pvPacketAgpdData);
		if (pvPacketAgsdData)
			m_csPacket.FreePacket(pvPacketAgsdData);

		return FALSE;
	}

	BOOL	bSendResult	= TRUE;

	if (bGroupNID)
	{
		bSendResult	&= m_pagsmAOIFilter->SendPacketGroup(pvPacketAgpdData, nAgpdPacketLength, dpnid);
		bSendResult	&= m_pagsmAOIFilter->SendPacketGroup(pvPacketAgsdData, nAgsdPacketLength, dpnid);
	}
	else
	{
		bSendResult	&= SendPacket(pvPacketAgpdData, nAgpdPacketLength, dpnid);
		bSendResult	&= SendPacket(pvPacketAgsdData, nAgsdPacketLength, dpnid);
	}

	AgsdItem	*pcsAgsdItem	= GetADItem(pcsItem);
	if (bSendResult &&
		pcsAgsdItem &&
		pcsAgsdItem->m_bIsNeedInsertDB)
		pcsAgsdItem->m_bIsNeedInsertDB	= FALSE;

	if (pvPacketAgpdData)
		m_csPacket.FreePacket(pvPacketAgpdData);
	if (pvPacketAgsdData)
		m_csPacket.FreePacket(pvPacketAgsdData);

	return bSendResult;
}
*/

//BOOL AgsmItem::SendPacketConvertHistory(AgpdItem *pcsItem, UINT32 dpnid)
//{
//	if (!pcsItem || dpnid == 0)
//		return FALSE;
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= m_pagpmItem->MakePacketItemConvertHistory(pcsItem, &nPacketLength);
//
//	if (!pvPacket || nPacketLength <= 0)
//		return FALSE;
//
//	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, dpnid);
//
//	m_pagpmItem->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}
//
//BOOL AgsmItem::SendPacketAddConvertHistory(AgpdItem *pcsItem, UINT32 dpnid)
//{
//	if (!pcsItem || dpnid == 0)
//		return FALSE;
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= m_pagpmItem->MakePacketItemAddConvertHistory(pcsItem, &nPacketLength, TRUE);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, dpnid);
//
//	m_pagpmItem->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}
//
//BOOL AgsmItem::SendPacketRemoveConvertHistory(AgpdItem *pcsItem, INT32 lIndex, UINT32 dpnid)
//{
//	if (!pcsItem || dpnid == 0)
//		return FALSE;
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= m_pagpmItem->MakePacketItemRemoveConvertHistory(pcsItem, lIndex, &nPacketLength);
//
//	if (!pvPacket || nPacketLength < 1)
//		return FALSE;
//
//	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, dpnid);
//
//	m_pagpmItem->m_csPacket.FreePacket(pvPacket);
//
//	return bSendResult;
//}

//		SendNewItem
//	Functions
//		- CallbackNew()에 의해서 호출되며, 아이템 소유주가 캐릭터인 경우만 그 캐릭터에게 아이템 정보를 보낸다.
//			(필드에 떨어진 아이템이거나 NPC가 착용하고 있는 아이템은 보내지 않는다. 이런 경우엔 view 데이타만 나중에 보낸다.)
//	Arguments
//		- pItem : 추가된 아이템 데이타
//	Return value
//		- BOOL : 패킷 전송 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::SendNewItem(AgpdItem *pItem)
{
	if (!pItem || !pItem->m_pcsCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pItem->m_pcsCharacter);

	return SendPacketItem(pItem, pcsAgsdCharacter->m_dpnidCharacter);
}

//자신과 거래중인 PC에게 내가 TradeGrid에 넣은 Item을 알려준다.
BOOL AgsmItem::SendNewItemToClient(AgpdItem *pItem)
{
	if (!pItem || !pItem->m_pcsCharacter)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter = NULL;
	AgsdCharacter	*pcsAgsdCharacter = NULL;
	AgpdItemADChar	*pcsAgpdItemADChar = NULL;

	INT32			lTargetID;

	//거래자의 ID를 얻어낸다.
	if (m_pagpmCharacter)
		pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pItem->m_pcsCharacter);

	lTargetID = pcsAgpdItemADChar->m_lTradeTargetID;

	//거래자의 AgsdCharacter를 얻어낸다. 
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lTargetID);

	pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pItem->m_pcsCharacter);

	//m_dpnidCharacter를 얻었으니 거래자에게 알려준다.
	return SendPacketItem(pItem, pcsAgsdCharacter->m_dpnidCharacter );
}

//		SendPacketItem
//	Functions
//		- pcsItem 정보(detail info)를 ulNID에게 보낸다.
//	Arguments
//		- pcsItem : 보낼 아템 데이타
//		- ulNID : 보낼 network id (DPNID)
//		- nFlag : 보낼때 사용할 Flag (보통의 경우 걍 이 플래그는 건들지 않는다.)
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::SendPacketItem(AgpdItem *pcsItem, UINT32 ulNID, BOOL bGroupNID, INT16 nFlag )
{
	if (!pcsItem || !ulNID)
		return FALSE;

	INT16	nPacketLength;

	PVOID pvPacket = m_pagpmItem->MakePacketItem(pcsItem, &nPacketLength );

	if (!pvPacket)
	{
		return FALSE;
	}

	BOOL	bSendResult	= FALSE;
	
	if (pcsItem->m_pcsCharacter)
		m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItem->m_pcsCharacter->m_lID);

	if (bGroupNID)
		bSendResult	= m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_3);
	else
		bSendResult = SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_3);

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	if (bSendResult)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0] = UintToPtr(ulNID);
		pvBuffer[1] = IntToPtr(bGroupNID);

		EnumCallback(AGSMITEM_CB_SEND_ITEM, pcsItem, pvBuffer);
	}

	return bSendResult;
}

BOOL AgsmItem::SendPacketNewItemID(AgpdItem *pcsItem, UINT32 ulNID)
{
	if (!pcsItem || ulNID == 0)
		return FALSE;

	INT16	nItemPacketLength	= 0;
	PVOID	pvPacketItem		= m_pagpmItem->MakePacketItem(pcsItem, &nItemPacketLength);

	INT16	nItemConvertPacketLength	= 0;
	PVOID	pvPacketItemConvert	= m_pagpmItemConvert->MakePacketAdd(pcsItem, &nItemConvertPacketLength);

	AgsdItem	*pcsAgsdItem	= GetADItem(pcsItem);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMITEM_PACKET_TYPE,
										NULL,
										NULL,
										NULL,
										pvPacketItem, &nItemPacketLength,
										&pcsAgsdItem->m_ullDBIID,
										pvPacketItemConvert, &nItemConvertPacketLength
										);

	m_csPacket.FreePacket(pvPacketItem);
	m_csPacket.FreePacket(pvPacketItemConvert);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

//		SendPacketItemView
//	Functions
//		- pcsItem 정보(view info)를 ulNID에게 보낸다.
//	Arguments
//		- pcsItem : 보낼 아템 데이타
//		- ulNID : 보낼 network id (DPNID)
//		- nFlag : 보낼때 사용할 Flag (보통의 경우 걍 이 플래그는 건들지 않는다.)
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::SendPacketItemView(AgpdItem *pcsItem, UINT32 ulNID, BOOL bGroupNID, UINT32 ulSelfNID, INT16 nFlag)
{
	if (!pcsItem || ulNID == 0)
		return FALSE;

	INT16	nPacketLength;

	PVOID pvPacket = m_pagpmItem->MakePacketItemView(pcsItem, &nPacketLength);

	if (!pvPacket)
	{
		return FALSE;
	}

	BOOL	bSendResult	= FALSE;

	if (pcsItem->m_pcsCharacter)
		m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItem->m_pcsCharacter->m_lID);

	if (bGroupNID)
	{
		if (ulSelfNID != 0)
			bSendResult	= m_pagsmAOIFilter->SendPacketGroupExceptSelf(pvPacket, nPacketLength, ulNID, ulSelfNID, PACKET_PRIORITY_3);
		else
			bSendResult	= m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_3);
	}
	else
		bSendResult = SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_3);

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	if (bSendResult)
	{
		PVOID	pvBuffer[3];
		pvBuffer[0] = UintToPtr(ulNID);
		pvBuffer[1] = IntToPtr(bGroupNID);
		pvBuffer[2] = UintToPtr(ulSelfNID);

		EnumCallback(AGSMITEM_CB_SEND_ITEM_VIEW, pcsItem, pvBuffer);
	}

	return bSendResult;
}

//		SendPacketItemAll
//	Functions
//		- pcsCharacter가 가지고 있는 모든 아템 정보(detail info)를 ulNID에게 보낸다.
//	Arguments
//		- pcsCharacter : 보낼 캐릭터
//		- ulNID : 보낼 network id (DPNID)
//		- nFlag : 보낼때 사용할 Flag (보통의 경우 걍 이 플래그는 건들지 않는다.)
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::SendPacketItemAll(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bGroupNID, INT16 nFlag)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	AgpdGridItem	*pcsGridItem;
	AgpdItemADChar	*pcsItemADChar = m_pagpmItem->GetADCharacter( pcsCharacter );

	if( pcsItemADChar == NULL )
		return FALSE;

	int i = 0;
	for (i = 0; i < AGPMITEM_PART_NUM; i++)
	{
		pcsGridItem = m_pagpmItem->GetEquipItem( pcsCharacter, i );

		if( pcsGridItem == NULL )
			continue;

		AgpdItem *pcsItem = m_pagpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		SendPacketItem(pcsItem, ulNID, bGroupNID, nFlag);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		SendPacketItem(pcsItem, ulNID, bGroupNID, nFlag);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		SendPacketItem(pcsItem, ulNID, bGroupNID, nFlag);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csQuestGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csQuestGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		SendPacketItem(pcsItem, ulNID, bGroupNID, nFlag);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csSalesBoxGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csSalesBoxGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		SendPacketItem(pcsItem, ulNID, bGroupNID, nFlag);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		SendPacketItem(pcsItem, ulNID, bGroupNID, nFlag);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csSubInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csSubInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		SendPacketItem(pcsItem, ulNID, bGroupNID, nFlag);
	}

	return TRUE;
}

BOOL AgsmItem::SendPacketItemAllNewID(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	AgpdGridItem	*pcsGridItem;
	AgpdItemADChar	*pcsItemADChar = m_pagpmItem->GetADCharacter( pcsCharacter );

	if( pcsItemADChar == NULL )
		return FALSE;

	INT32	lOriginalCID	= AP_INVALID_CID;

	int i = 0;
	for (i = 0; i < AGPMITEM_PART_NUM; i++)
	{
		pcsGridItem = m_pagpmItem->GetEquipItem( pcsCharacter, i );

		if( pcsGridItem == NULL )
			continue;

		AgpdItem *pcsItem = m_pagpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		pcsItem->m_pcsCharacter	= pcsCharacter;
		lOriginalCID			= pcsItem->m_ulCID;
		pcsItem->m_ulCID		= pcsCharacter->m_lID;

		SendPacketNewItemID(pcsItem, ulNID);

		pcsItem->m_ulCID	= lOriginalCID;
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		pcsItem->m_pcsCharacter	= pcsCharacter;
		lOriginalCID			= pcsItem->m_ulCID;
		pcsItem->m_ulCID		= pcsCharacter->m_lID;

		SendPacketNewItemID(pcsItem, ulNID);

		pcsItem->m_ulCID	= lOriginalCID;
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		pcsItem->m_pcsCharacter	= pcsCharacter;
		lOriginalCID			= pcsItem->m_ulCID;
		pcsItem->m_ulCID		= pcsCharacter->m_lID;

		SendPacketNewItemID(pcsItem, ulNID);

		pcsItem->m_ulCID	= lOriginalCID;
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csQuestGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csQuestGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		pcsItem->m_pcsCharacter	= pcsCharacter;
		lOriginalCID			= pcsItem->m_ulCID;
		pcsItem->m_ulCID		= pcsCharacter->m_lID;

		SendPacketNewItemID(pcsItem, ulNID);

		pcsItem->m_ulCID	= lOriginalCID;
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csSalesBoxGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csSalesBoxGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		pcsItem->m_pcsCharacter	= pcsCharacter;
		lOriginalCID			= pcsItem->m_ulCID;
		pcsItem->m_ulCID		= pcsCharacter->m_lID;

		SendPacketNewItemID(pcsItem, ulNID);

		pcsItem->m_ulCID	= lOriginalCID;
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		pcsItem->m_pcsCharacter	= pcsCharacter;
		lOriginalCID			= pcsItem->m_ulCID;
		pcsItem->m_ulCID		= pcsCharacter->m_lID;

		SendPacketNewItemID(pcsItem, ulNID);

		pcsItem->m_ulCID	= lOriginalCID;
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csSubInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csSubInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		pcsItem->m_pcsCharacter	= pcsCharacter;
		lOriginalCID			= pcsItem->m_ulCID;
		pcsItem->m_ulCID		= pcsCharacter->m_lID;

		SendPacketNewItemID(pcsItem, ulNID);

		pcsItem->m_ulCID	= lOriginalCID;
	}

	return TRUE;
}

//		SendPacketItemRemove
//	Functions
//		- lIID가 삭제되야 한다는 정보를 보낸다.
//	Arguments
//		- lIID : 삭제될 아템 정보
//		- ulNID : 보낼 network id (DPNID)
//		- nFlag : 보낼때 사용할 Flag (보통의 경우 걍 이 플래그는 건들지 않는다.)
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmItem::SendPacketItemRemove(INT32 lIID, UINT32 ulNID, BOOL bGroupNID, INT16 nFlag)
{
	if (lIID == AP_INVALID_IID || ulNID == 0)
		return FALSE;

	AgpdItem	*pcsItem = m_pagpmItem->GetItem(lIID);
	if (!pcsItem)
		return FALSE;

	INT16	nPacketLength;
	PVOID	pvPacket = MakePacketItemRemove(pcsItem, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	BOOL	bSendResult	= FALSE;

	if (pcsItem->m_pcsCharacter)
		m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItem->m_pcsCharacter->m_lID);

	if (bGroupNID)
		bSendResult	= m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_3);
	else
		bSendResult = SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_3);

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

// 2008.03.21. steeple
BOOL AgsmItem::SendPacketItemRemoveNear(INT32 lIID, AuPOS stPos)
{
	if (lIID == AP_INVALID_IID)
		return FALSE;

	AgpdItem	*pcsItem = m_pagpmItem->GetItem(lIID);
	if (!pcsItem)
		return FALSE;

	INT16	nPacketLength;
	PVOID	pvPacket = MakePacketItemRemove(pcsItem, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	BOOL bSendResult	= m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, stPos, PACKET_PRIORITY_3);
	m_pagpmItem->m_csPacket.FreePacket(pvPacket);
	return bSendResult;
}

BOOL AgsmItem::SendPacketItemServerData(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bLogin)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	AgpdItemADChar	*pcsItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return FALSE;

	INT16			nPacketLength;
	AgpdGridItem *	pcsGridItem;

	int i = 0;
	for (i = 0; i < AGPMITEM_PART_NUM; i++)
	{
		pcsGridItem = m_pagpmItem->GetEquipItem( pcsCharacter, i );

		if (pcsGridItem == NULL)
			continue;

		AgpdItem *pcsItem = m_pagpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		PVOID	pvPacket = MakePacketItemServerData(pcsItem, &nPacketLength, bLogin);
		if (!pvPacket || nPacketLength <= 0)
			continue;

		SendPacket(pvPacket, nPacketLength, ulNID);

		m_csPacket.FreePacket(pvPacket);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		PVOID	pvPacket = MakePacketItemServerData(pcsItem, &nPacketLength, bLogin);
		if (!pvPacket || nPacketLength <= 0)
			continue;

		SendPacket(pvPacket, nPacketLength, ulNID);

		m_csPacket.FreePacket(pvPacket);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csSubInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csSubInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		PVOID	pvPacket = MakePacketItemServerData(pcsItem, &nPacketLength, bLogin);
		if (!pvPacket || nPacketLength <= 0)
			continue;

		SendPacket(pvPacket, nPacketLength, ulNID);

		m_csPacket.FreePacket(pvPacket);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		PVOID	pvPacket = MakePacketItemServerData(pcsItem, &nPacketLength, bLogin);
		if (!pvPacket || nPacketLength <= 0)
			continue;

		SendPacket(pvPacket, nPacketLength, ulNID);

		m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}

BOOL AgsmItem::SendPacketItemADCharacter(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	AgsdItemADChar	*pcsItemADChar = GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return FALSE;

	INT16	nPacketLength = 0;

	PVOID	pvPacket = MakePacketItemADCharacter(pcsCharacter, &nPacketLength);
	if (!pvPacket || nPacketLength <= 0)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmItem::SendPacketItemStackCount(AgpdItem *pcsItem, UINT32 ulNID)
{
	if (!pcsItem || ulNID == 0)
		return FALSE;

	return SendPacketItemStackCount(pcsItem, pcsItem->m_nCount, ulNID);
}

BOOL AgsmItem::SendPacketItemStackCount(AgpdItem *pcsItem, INT32 lStackCount, UINT32 ulNID)
{
	if (!pcsItem || ulNID == 0 || lStackCount < 0)
		return FALSE;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_UPDATE;
	INT16	nPacketLength	= 0;
	INT8 eStatus = (INT8)pcsItem->m_eStatus;

	PVOID	pvPacket		= m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
																&cOperation,
																&eStatus,
																&pcsItem->m_lID,
																NULL,
																(pcsItem->m_ulCID != AP_INVALID_CID) ? &pcsItem->m_ulCID : NULL,
																&lStackCount,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,		// Quest
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,		// SkillPlus
																NULL,
																NULL,		// CashInformation
																NULL);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmItem::SendPacketItemFactor(AgpdItem *pcsItem, PVOID pvPacketFactor, PVOID pvPacketFactorPercent, UINT32 ulNID)
{
	if (!pcsItem || ulNID == 0 || (!pvPacketFactor && !pvPacketFactorPercent))
		return FALSE;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_UPDATE;
	INT16	nPacketLength	= 0;

	PVOID	pvPacket		= m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
																&cOperation,
																NULL,
																&pcsItem->m_lID,
																NULL,
																&pcsItem->m_ulCID,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																pvPacketFactor,
																pvPacketFactorPercent,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,		// Quest
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,		// SkillPlus
																NULL,
																NULL,		// CashInformation
																NULL);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmItem::SendPacketItemFactor(AgpdItem *pcsItem, UINT32 ulNID)
{
	if (!pcsItem || !pcsItem->m_pcsCharacter || ulNID == 0)
		return FALSE;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_UPDATE;
	INT16	nPacketLength	= 0;

	PVOID	pvPacketFactor	= m_pagpmFactors->MakePacketFactorDiffer(&pcsItem->m_csFactor, &((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor);

	PVOID	pvPacketRestrictFactor = m_pagpmFactors->MakePacketFactorDiffer(&pcsItem->m_csRestrictFactor, &((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csRestrictFactor);

	PVOID	 pvPacket = m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
												  &cOperation,				//Operation
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
												  NULL,			// reuse time for reverse orb
												  NULL,
												  NULL,
												  NULL,			// SkillPlus
												  NULL,
												  NULL,			// CashInformation
												  NULL);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);
	m_pagpmFactors->m_csPacket.FreePacket(pvPacketRestrictFactor);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmItem::SendPacketPickupItemResult(INT8 cResult, INT32 lIID, INT32 lTID, INT32 lItemCount, UINT32 ulNID)
{
	if (ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmItem->MakePacketItemPickupResult(cResult, lIID, lTID, lItemCount, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmItem::SendRelayUpdate(AgpdItem* pItem)
{
	//STOPWATCH2(GetModuleName(), _T("SendRelayUpdate"));
	if(NULL == pItem || NULL == pItem->m_pcsItemTemplate)
		return FALSE;

	if (((AgpdItemTemplate *) pItem->m_pcsItemTemplate)->m_lID == m_pagpmItem->GetMoneyTID())
		return FALSE;

	AgsdItem	*pcsAgsdItem	= GetADItem(pItem);
	if (!pcsAgsdItem)
		return FALSE;

	if (pcsAgsdItem->m_bIsNeedInsertDB)
	{
		if (pItem->m_pcsCharacter)
		{
			AgsdServer		*pcsRelayServer	= m_pAgsmServerManager->GetRelayServer();
			if (pcsRelayServer && pcsRelayServer->m_dpnidServer != 0)
			{
				SendRelayInsert(pItem);

//				for (int i = 0; i < pItem->m_stConvertHistory.lConvertLevel; ++i)
//				{
//					EnumCallback(AGSMITEM_CB_INSERT_CONVERT_HISTORY_TO_DB, pItem, NULL);
//				}

				pcsAgsdItem->m_bIsNeedInsertDB = FALSE;
			}
		}

		return TRUE;
	}

	pcsAgsdItem->m_lPrevSaveStackCount	= pItem->m_nCount;

	UINT64 lDBID = GetDBIID(pItem);

	if (0 == lDBID) return FALSE;
	return EnumCallback(AGSMITEM_CB_UPDATE_ITEM_TO_DB, pItem, &lDBID);		
}

BOOL AgsmItem::SendRelayInsert(AgpdItem* pItem)
{
	AgsdItem	*pcsAgsdItem	= GetADItem(pItem);
	if (!pcsAgsdItem)
		return FALSE;

	if (((AgpdItemTemplate *) pItem->m_pcsItemTemplate)->m_lID == m_pagpmItem->GetMoneyTID())
		return FALSE;

	pcsAgsdItem->m_lPrevSaveStackCount	= pItem->m_nCount;

	UINT64 lDBID = GetDBIID(pItem, TRUE);
	if (0 == lDBID) return FALSE;
	
	EnumCallback(AGSMITEM_CB_INSERT_ITEM_TO_DB, pItem, &lDBID);

	pcsAgsdItem->m_bIsNeedInsertDB = FALSE;

	// 2007.01.08. steeple
	// 위에서 Insert 할 때 한방에 해결하도록 바꿨다.
	//EnumCallback(AGSMITEM_CB_INSERT_CONVERT_HISTORY_TO_DB, pItem, NULL);

	return TRUE;
}

BOOL AgsmItem::SendRelayDelete(AgpdItem* pItem)
{
	if (((AgpdItemTemplate *) pItem->m_pcsItemTemplate)->m_lID == m_pagpmItem->GetMoneyTID())
		return FALSE;

	UINT64 lDBID = GetDBIID(pItem);
	if (0 == lDBID) return FALSE;
	return EnumCallback(AGSMITEM_CB_DELETE_ITEM_TO_DB, pItem, &lDBID);
}

PVOID AgsmItem::MakePacketUpdateReturnScrollStatus(AgpdCharacter *pcsCharacter, BOOL bIsEnable, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_ENABLE_RETURN_SCROLL;

	if (!bIsEnable)
		cOperation	= AGPMITEM_PACKET_OPERATION_DISABLE_RETURN_SCROLL;

	return	m_pagpmItem->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
												&cOperation,
												NULL,
												NULL,
												NULL,
												&pcsCharacter->m_lID,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL, 
												NULL,		// Quest
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,		// SkillPlus
												NULL,
												NULL,		// CashInformation
												NULL);
}

PVOID AgsmItem::MakePacketUseReturnScrollResultFailed(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_USE_RETURN_SCROLL_FAILED;

	return	m_pagpmItem->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
												&cOperation,
												NULL,
												NULL,
												NULL,
												&pcsCharacter->m_lID,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,		// Quest
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,		// SkillPlus
												NULL,
												NULL,		// CashInformation
												NULL);
}

PVOID AgsmItem::MakePacketUseItemResult(AgpdCharacter *pcsCharacter, AgpmItemUseResult eResult, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_USE_ITEM_RESULT;
	INT8	cResult			= (INT8)	eResult;

	return	m_pagpmItem->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
												&cOperation,
												&cResult,
												NULL,
												NULL,
												&pcsCharacter->m_lID,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,		// Quest
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,		// SkillPlus
												NULL,
												NULL,		// CashInformation
												NULL);
}

BOOL AgsmItem::SendPacketUpdateReturnScrollStatus(AgpdCharacter *pcsCharacter, BOOL bIsEnable)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketUpdateReturnScrollStatus(pcsCharacter, bIsEnable, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmItem::SendPacketUseReturnScrollResult(AgpdCharacter *pcsCharacter, BOOL bIsSuccess)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= NULL;

	if (bIsSuccess)
	{
		pvPacket		= NULL;
	}
	else
	{
		pvPacket		= MakePacketUseReturnScrollResultFailed(pcsCharacter, &nPacketLength);
	}

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmItem::SendPacketUseItemByTID(AgpdCharacter *pcsItemOwner, INT32 lTID, AgpdCharacter *pcsTargetCharacter, UINT32 ulUseInterval)
{
	return SendPacketUseItemByTID(pcsItemOwner, lTID, (pcsTargetCharacter) ? pcsTargetCharacter->m_lID : AP_INVALID_CID, ulUseInterval);
}

BOOL AgsmItem::SendPacketUseItemByTID(AgpdCharacter *pcsItemOwner, INT32 lTID, INT32 lCID, UINT32 ulUseInterval)
{
	if (!pcsItemOwner || lTID == AP_INVALID_IID)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_USE_ITEM_BY_TID;

	PVOID	pvPacket		= m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
																&cOperation,
																NULL,
																NULL,
																&lTID,
																&pcsItemOwner->m_lID,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																(lCID != AP_INVALID_CID) ? &lCID : NULL,
																NULL,
																NULL,
																NULL, 
																NULL,		// Quest
																NULL,
																&ulUseInterval,
																NULL,
																NULL,
																NULL,		// SkillPlus
																NULL,
																NULL,		// CashInformation
																NULL);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItemOwner->m_lID);

	//BOOL	bSendResult	= m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsItemOwner->m_stPos, PACKET_PRIORITY_6);
	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsItemOwner));

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmItem::SendPacketUseItemFailByTID(AgpdCharacter *pcsItemOwner, INT32 lTID, AgpdCharacter *pcsTargetCharacter)
{
	if (!pcsItemOwner || lTID == AP_INVALID_IID)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_USE_ITEM_FAILED_BY_TID;

	PVOID	pvPacket		= m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
																&cOperation,
																NULL,
																NULL,
																&lTID,
																&pcsItemOwner->m_lID,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																(pcsTargetCharacter) ? &pcsTargetCharacter->m_lID : NULL,
																NULL,
																NULL,
																NULL, 
																NULL,		// Quest
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,		// SkillPlus
																NULL,
																NULL,		// CashInformation
																NULL);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItemOwner->m_lID);

	//BOOL	bSendResult	= m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsItemOwner->m_stPos, PACKET_PRIORITY_6);
	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsItemOwner));

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmItem::SendPacketUseItemSuccess(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem)
{
	if (!pcsCharacter || !pcsItem)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_USE_ITEM_SUCCESS;

	PVOID	pvPacket		= m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
																&cOperation,
																NULL,
																&pcsItem->m_lID,
																NULL,
																&pcsCharacter->m_lID,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL, 
																NULL,		// Quest
																NULL,
																NULL,		// UseInterval
																NULL,
																NULL,
																NULL,		// SkillPlus
																NULL,
																NULL,		// CashInformation
																NULL);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmItem::SendPacketUpdateReverseOrbReuseTime(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	AgpdItemADChar	*pcsAttachData	= m_pagpmItem->GetADCharacter(pcsCharacter);
	if (pcsAttachData->m_ulUseReverseOrbTime == 0)
		return TRUE;

	AgpdItemTemplate	*pcsReverseOrbTemplate	= m_pagpmItem->GetItemTemplate(1054);
	if (!pcsReverseOrbTemplate)
		return FALSE;

	UINT32	lUseReverseOrbTime	= ((AgpdItemTemplateUsable *) pcsReverseOrbTemplate)->m_ulUseInterval - (GetClockCount() - pcsAttachData->m_ulUseReverseOrbTime);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmItem->MakePacketUpdateReuseTimeForReverseOrb(pcsCharacter->m_lID, (UINT32)lUseReverseOrbTime, &nPacketLength);

	if (pvPacket && nPacketLength > 0)
	{
		m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);

		m_pagpmItem->m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}

BOOL AgsmItem::SendPacketUpdateTransformReuseTime(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	AgpdItemADChar	*pcsAttachData	= m_pagpmItem->GetADCharacter(pcsCharacter);
	if (pcsAttachData->m_ulUseTransformTime == 0)
		return TRUE;

	UINT32	ulReuseTime	= AGPMITEM_TRANSFORM_RECAST_TIME - (GetClockCount() - pcsAttachData->m_ulUseTransformTime);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmItem->MakePacketUpdateReuseTimeForTransform(pcsCharacter->m_lID, ulReuseTime, &nPacketLength);

	if (pvPacket && nPacketLength > 0)
	{
		m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);

		m_pagpmItem->m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}

BOOL AgsmItem::SendPacketInitTransformReuseTime(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmItem->MakePacketInitReuseTimeForTransform(pcsCharacter->m_lID, &nPacketLength);

	if (pvPacket && nPacketLength > 0)
	{
		m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);

		m_pagpmItem->m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}

BOOL AgsmItem::SendPacketUpdateItemStatusFlag(AgpdItem *pcsItem, UINT32 ulNID)
{
	if (!pcsItem || ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_UPDATE;

	PVOID	pvPacket		= m_pagpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
																&cOperation,
																NULL,
																&pcsItem->m_lID,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL, 
																NULL,		// Quest
																NULL,
																NULL,
																&pcsItem->m_lStatusFlag,
																NULL,
																NULL,		// SkillPlus
																NULL,
																NULL,		// CashInformation
																NULL);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	if (pcsItem->m_pcsCharacter)
		m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItem->m_pcsCharacter->m_lID);

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

//	2005.04.21	Start By SungHoon
//	파티원들이 파티멤버가 아이템을 획득했다는 정보를 알기위한 패킷
PVOID AgsmItem::MakePacketItemForPartyMember(AgpdItem *pcsItem, INT32 lCID, INT16 *pnPacketLength)
{
	AgpdItemConvertADItem	*pcsAttachData	= m_pagpmItemConvert->GetADItem(pcsItem);

	INT8	cNumPhysicalConvert	= (INT8) pcsAttachData->m_lPhysicalConvertLevel;
	INT8	cNumSocket			= (INT8) pcsAttachData->m_lNumSocket;
	INT8	cNumConvertedSocket	= (INT8) pcsAttachData->m_lNumConvert;
	INT8	cOptionCount		= 0;
	
	for	(INT8 i = 0 ; i < AGPDITEM_OPTION_MAX_NUM; i++)
	{
		if (pcsItem->m_aunOptionTID[0] != 0) cOptionCount++;
	}

	PVOID pvPacket = m_csPacketPartyItem.MakePacket(TRUE, pnPacketLength, AGPMPARTYITEM_PACKET_TYPE,
							&pcsItem->m_lTID,				// Item Template ID
							&lCID,				// Item Owner CID
							&pcsItem->m_nCount,				// Item Count
							&cNumPhysicalConvert,			// # of physical convert
							&cNumSocket,						// # of socket
							&cNumConvertedSocket,			// # of converted socket
							&cOptionCount					// option Count
							);

	return pvPacket;

}

BOOL AgsmItem::SendItemToPartyMember(AgpdCharacter *pAgpdCharacter, AgpdItem *pcsItem)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketItemForPartyMember(pcsItem, pAgpdCharacter->m_lID, &nPacketLength);
	if (pvPacket)
	{
		m_pagsmParty->SendPacketToParty(pAgpdCharacter, pvPacket, nPacketLength, PACKET_PRIORITY_4);
		m_csPacketPartyItem.FreePacket(pvPacket);
	}
	return TRUE;

}
//	2005.04.21	Finish By SungHoon

/*
	2005.11.30. By SungHoon
	아이템 사용을 중지한다는 패킷을 보낸다.
*/
BOOL AgsmItem::SendPacketUnuseItem(AgpdItem *pcsItem)
{
	if (!pcsItem) return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmItem->MakePacketItemUnuse(pcsItem, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	AgpdCharacter	*pcsItemOwner	= pcsItem->m_pcsCharacter;
	m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItemOwner->m_lID);

	//BOOL	bSendResult	= m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsItemOwner->m_stPos, PACKET_PRIORITY_6);
	BOOL bSendResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

/*
	2005.11.30. By SungHoon
	아이템 사용을 중지한다는 패킷을 보낸다.
*/
BOOL AgsmItem::SendPacketUnuseItemFailed(AgpdItem *pcsItem)
{
	if (!pcsItem) return FALSE;
	AgpdCharacter	*pcsItemOwner	= pcsItem->m_pcsCharacter;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmItem->MakePacketItemUnuseFailed(pcsItem, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItemOwner->m_lID);

	//BOOL	bSendResult	= m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsItemOwner->m_stPos, PACKET_PRIORITY_6);
	BOOL bSendResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

// 2006.01.08. steeple
// 아이템을 일시정지 한다는 패킷을 보낸다.
BOOL AgsmItem::SendPacketPauseItem(AgpdItem* pcsItem)
{
	if(!pcsItem)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmItem->MakePacketItemPause(pcsItem, &nPacketLength);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItem->m_pcsCharacter->m_lID);

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsItem->m_pcsCharacter));
	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmItem::SendPacketUpdateItemUseTime(AgpdItem *pcsItem)
{
	if (!pcsItem) return FALSE;
	AgpdCharacter	*pcsItemOwner	= pcsItem->m_pcsCharacter;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmItem->MakePacketItemUpdateItemUseTime(pcsItem, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItemOwner->m_lID);

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsItemOwner));

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmItem::SendPacketUpdateItemStaminaRemainTime(AgpdItem* pcsItem)
{
	if (!pcsItem) return FALSE;
	AgpdCharacter	*pcsItemOwner	= pcsItem->m_pcsCharacter;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmItem->MakePacketItemUpdateItemStaminaRemainTime(pcsItem, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsItemOwner->m_lID);

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsItemOwner));

	m_pagpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmItem::SendPacketUpdateCooldown(AgpdCharacter* pcsCharacter, AgpdItemCooldownBase stCooldownBase)
{
	if(!pcsCharacter || stCooldownBase.m_lTID < 1)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmItem->MakePacketItemUpdateCooldown(pcsCharacter->m_lID, stCooldownBase, &nPacketLength);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmItem->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);
	BOOL bSendResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	m_pagpmItem->m_csPacket.FreePacket(pvPacket);
	return	bSendResult;
}

BOOL AgsmItem::SendPacketAllCooldown(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItemADChar* pcsItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
	if(!pcsItemADChar || pcsItemADChar->m_CooldownInfo.m_pBases->empty())
		return FALSE;

	AgpdItemCooldownArray::CooldownBaseIter iter = pcsItemADChar->m_CooldownInfo.m_pBases->begin();
	while(iter != pcsItemADChar->m_CooldownInfo.m_pBases->end())
	{
		SendPacketUpdateCooldown(pcsCharacter, *iter);
		++iter;
	}

	return TRUE;
}