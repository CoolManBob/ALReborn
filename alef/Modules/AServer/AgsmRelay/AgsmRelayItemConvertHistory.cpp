#include "AgsmRelay.h"

/*
BOOL AgsmRelay::OperationItemConvertHistory(INT32 cDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelayItemConvertHistory csRelayItemConvertHistory;
	ZeroMemory(&csRelayItemConvertHistory, sizeof(csRelayItemConvertHistory));

	m_csItemConvertHistoryPacket.GetField(FALSE, pvPacket, 0, 
							&csRelayItemConvertHistory.ullDBID,
							&csRelayItemConvertHistory.cType,
							&csRelayItemConvertHistory.lAttribute,
							&csRelayItemConvertHistory.lValue);

	csRelayItemConvertHistory.ulNID = ulNID;
	return EnumCallback(AGSMRELAY_OPERATION_ITEM_CONVERT_HISTORY, (PVOID)&csRelayItemConvertHistory, (PVOID)cDBOperation);
}
*/

BOOL AgsmRelay::OperationItemConvertHistory2(INT32 cDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelayItemConvertHistory2 csRelayItemConvertHistory;
	ZeroMemory(&csRelayItemConvertHistory, sizeof(csRelayItemConvertHistory));

	m_csItemConvertHistoryPacket2.GetField(FALSE, pvPacket, 0, 
							&csRelayItemConvertHistory.ullDBID,
							&csRelayItemConvertHistory.pConvertHistoryString, &csRelayItemConvertHistory.nStringLength);

	csRelayItemConvertHistory.ulNID = ulNID;
	return EnumCallback(AGSMRELAY_OPERATION2_ITEM_CONVERT_HISTORY, (PVOID)&csRelayItemConvertHistory, (PVOID)cDBOperation);
}

/*
BOOL AgsmRelay::SendItemConvertHistoryInsert(AgpdItem *pAgpdItem, INT32 lIndex)
{
	// Relay Server가 연결이 안되어 있으면 종료
	if (0 == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	if (!pAgpdItem || lIndex < 0 || lIndex >= pAgpdItem->m_stConvertHistory.lConvertLevel)
		return FALSE;

	AgsdItem	*pcsAgsdItem	= m_pAgsmItem->GetADItem(pAgpdItem);

	INT16	nPacketLength	= 0;

	INT8	cType			= (INT8) pAgpdItem->m_stConvertHistory.bUseSpiritStone[lIndex];
	INT32	lAttribute		= 0;
	INT32	lValue			= 0;

	if (pAgpdItem->m_stConvertHistory.bUseSpiritStone[lIndex])
		lAttribute	= pAgpdItem->m_stConvertHistory.lTID[lIndex];
	else
	{
		lAttribute	= (INT32) pAgpdItem->m_stConvertHistory.eRuneAttribute[lIndex];
		lValue		= pAgpdItem->m_stConvertHistory.lRuneAttributeValue[lIndex];
	}

	PVOID	pvPacketItemConvertHistoryInsert	= m_csItemConvertHistoryPacket.MakePacket(FALSE, &nPacketLength, 0,
																	&pcsAgsdItem->m_ullDBIID,
																	&cType,
																	&lAttribute,
																	&lValue);

	if (!pvPacketItemConvertHistoryInsert)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION_ITEM_CONVERT_HISTORY, 0, 
										AGSMRELAY_DB_OPERATION_INSERT, NULL, NULL, pvPacketItemConvertHistoryInsert, NULL, NULL);

	m_csItemPacket.FreePacket(pvPacketItemConvertHistoryInsert);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);
	
	return TRUE;
}

BOOL AgsmRelay::SendItemConvertHistoryDelete(AgpdItem *pAgpdItem, INT32 lIndex)
{
	// Relay Server가 연결이 안되어 있으면 종료
	if (0 == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	if (!pAgpdItem || lIndex < 0 || lIndex >= pAgpdItem->m_stConvertHistory.lConvertLevel)
		return FALSE;

	AgsdItem	*pcsAgsdItem	= m_pAgsmItem->GetADItem(pAgpdItem);

	INT16	nPacketLength	= 0;

	INT8	cType			= (INT8) pAgpdItem->m_stConvertHistory.bUseSpiritStone[lIndex];
	INT32	lAttribute		= 0;

	if (pAgpdItem->m_stConvertHistory.bUseSpiritStone[lIndex])
		lAttribute	= pAgpdItem->m_stConvertHistory.lTID[lIndex];
	else
		lAttribute	= (INT32) pAgpdItem->m_stConvertHistory.eRuneAttribute[lIndex];

	PVOID	pvPacketItemConvertHistoryInsert	= m_csItemConvertHistoryPacket.MakePacket(FALSE, &nPacketLength, 0,
																	&pcsAgsdItem->m_ullDBIID,
																	&cType,
																	&lAttribute,
																	NULL);

	if (!pvPacketItemConvertHistoryInsert)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION_ITEM_CONVERT_HISTORY, 0, 
										AGSMRELAY_DB_OPERATION_DELETE, NULL, NULL, pvPacketItemConvertHistoryInsert, NULL, NULL);

	m_csItemPacket.FreePacket(pvPacketItemConvertHistoryInsert);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}
*/

BOOL AgsmRelay::SendItemConvertHistoryUpdate2(AgpdItem *pAgpdItem)
{
	// Relay Server가 연결이 안되어 있으면 종료
	if (0 == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	if (!pAgpdItem)
		return FALSE;

	CHAR	szConvertHistory[257];
	ZeroMemory(szConvertHistory, sizeof(CHAR) * 257);

	m_pAgsmItemConvert->EncodeConvertHistory(pAgpdItem, szConvertHistory, 256);

	INT16	nConvertHistoryLength	= strlen(szConvertHistory);

	INT16	nPacketLength	= 0;

	AgsdItem	*pcsAgsdItem	= m_pAgsmItem->GetADItem(pAgpdItem);

	PVOID	pvPacketItemConvertHistoryUpdate	= m_csItemConvertHistoryPacket2.MakePacket(FALSE, &nPacketLength, 0,
																	&pcsAgsdItem->m_ullDBIID,
																	szConvertHistory, &nConvertHistoryLength);

	if (!pvPacketItemConvertHistoryUpdate)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_ITEM_CONVERT_HISTORY, 0, 
										AGSMRELAY_DB_OPERATION_UPDATE, NULL, NULL, pvPacketItemConvertHistoryUpdate, NULL, NULL, NULL, NULL, NULL, NULL);

	m_csItemConvertHistoryPacket2.FreePacket(pvPacketItemConvertHistoryUpdate);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmRelay::CBItemConvertHistoryInsert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay			*pThis				= (AgsmRelay *)				pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)				pData;

#ifdef	__DB3_VERSION__
	return pThis->SendItemConvertHistoryUpdate2(pcsItem);
#else
	return pThis->SendItemConvertHistoryInsert(pcsItem, pcsItem->m_stConvertHistory.lConvertLevel - 1);
#endif	//__DB3_VERSION__
}

BOOL AgsmRelay::CBItemConvertHistoryDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay			*pThis				= (AgsmRelay *)				pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)				pData;
	INT32				lIndex				= *(INT32 *)				pCustData;

#ifdef	__DB3_VERSION__
	return pThis->SendItemConvertHistoryUpdate2(pcsItem);
#else
	return pThis->SendItemConvertHistoryDelete(pcsItem, lIndex);
#endif	//__DB3_VERSION__
}