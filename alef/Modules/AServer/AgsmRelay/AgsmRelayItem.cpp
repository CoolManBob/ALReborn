#include "AgsmRelay.h"

/*
BOOL AgsmRelay::OperationItem(INT32 cDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelayItem csRelayItem;
	ZeroMemory(&csRelayItem, sizeof(csRelayItem));

	m_csItemPacket.GetField(FALSE, pvPacket, 0, 
							&csRelayItem.pszCharName,
							&csRelayItem.lTemplateID,
							&csRelayItem.lStackCount,
							&csRelayItem.lStatus,
							&csRelayItem.stPosition,
							&csRelayItem.lGridTab,
							&csRelayItem.lGridRow,
							&csRelayItem.lGridColumn,
							&csRelayItem.lNeedLevel,
							&csRelayItem.lDBID);

	csRelayItem.ulNID = ulNID;
	return EnumCallback(AGSMRELAY_OPERATION_ITEM, (PVOID)&csRelayItem, (PVOID)cDBOperation);
}
*/

BOOL AgsmRelay::OperationItem2(INT32 cDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelayItem2 csRelayItem;
	ZeroMemory(&csRelayItem, sizeof(csRelayItem));

	m_csItemPacket2.GetField(FALSE, pvPacket, 0, 
							&csRelayItem.pszCharName,
							&csRelayItem.lTemplateID,
							&csRelayItem.lStackCount,
							&csRelayItem.lStatus,
							&csRelayItem.pszPosition,
							&csRelayItem.lNeedLevel,
							&csRelayItem.lDurability,
							&csRelayItem.pDeleteReasonString, &csRelayItem.nDeleteReasonLength,
							&csRelayItem.lDBID);

	csRelayItem.ulNID = ulNID;
	return EnumCallback(AGSMRELAY_OPERATION2_ITEM, (PVOID)&csRelayItem, (PVOID)cDBOperation);
}

BOOL AgsmRelay::CBItemInsert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(pData && pClass && "Item Insert를 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay*	pThis		= (AgsmRelay*)pClass;
	AgpdItem*	pAgpdItem	= (AgpdItem*)pData;
	UINT64		lDBID		= *((UINT64*)pCustData);
	
#ifdef	__DB3_VERSION__
	return pThis->SendItemInsert2(pAgpdItem, lDBID);
#else
	return pThis->SendItemInsert(pAgpdItem, lDBID);
#endif	//__DB3_VERSION__
}

BOOL AgsmRelay::CBItemUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(pData && pClass && pCustData && "Item Update 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay*	pThis		= (AgsmRelay*)pClass;
	AgpdItem*	pAgpdItem	= (AgpdItem*)pData;
	UINT64		lDBID		= *((UINT64*)pCustData);

#ifdef	__DB3_VERSION__
	return pThis->SendItemUpdate2(pAgpdItem, lDBID);
#else
	return pThis->SendItemUpdate(pAgpdItem, lDBID);
#endif
}

BOOL AgsmRelay::CBItemDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(pData && pClass && pCustData && "Item Delete 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay*	pThis		= (AgsmRelay*)pClass;
	UINT64		lDBID		= *((UINT64*)pCustData);
	AgpdItem	*pcsItem	= (AgpdItem *) pData;

#ifdef	__DB3_VERSION__
	return pThis->SendItemDelete2(lDBID, pcsItem->m_szDeleteReason);
#else
	return pThis->SendItemDelete(lDBID);
#endif
}

/*
BOOL AgsmRelay::SendResultItem(AgsdRelayItem *pAgsdRelayItem)
{
	return TRUE;
}

BOOL AgsmRelay::SendItemInsert(AgpdItem* pAgpdItem, UINT64 lDBID)
{
	if (!pAgpdItem || !lDBID)
		return FALSE;

	INT16 nPacketLength = 0;

	INT32	lNeedLevel	= 0;
	m_pAgpmFactors->GetValue(&pAgpdItem->m_csRestrictFactor, &lNeedLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	PVOID pvItemPacket = m_csItemPacket.MakePacket(FALSE, &nPacketLength, 0,
													&pAgpdItem->m_pcsCharacter->m_szID,
													&((AgpdItemTemplate*)pAgpdItem->m_pcsItemTemplate)->m_lID,
													&pAgpdItem->m_nCount,
													&pAgpdItem->m_eStatus,
													&pAgpdItem->m_posItem,
													&pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
													&pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
													&pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN],
													&lNeedLevel,		// JNY TODO 2004.3.3 : Need level
													&lDBID
													);
	
	if (!pvItemPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION_ITEM, 0, 
										AGSMRELAY_DB_OPERATION_INSERT, NULL, pvItemPacket, NULL, NULL, NULL);

	m_csItemPacket.FreePacket(pvItemPacket);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmRelay::SendItemUpdate(AgpdItem* pAgpdItem, UINT64 lDBID)
{
	if (!pAgpdItem || !pAgpdItem->m_pcsCharacter || !pAgpdItem->m_pcsItemTemplate)
	{
		//ASSERT(!"이런 상황이면 안되는데.... ");
		return FALSE;
	}

	// Relay Server가 연결이 안되어 있으면 종료
	if (0 == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	INT16 nPacketLength;

	INT32	lNeedLevel	= 0;
	m_pAgpmFactors->GetValue(&pAgpdItem->m_csRestrictFactor, &lNeedLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	PVOID pvItemPacket = m_csItemPacket.MakePacket(FALSE, &nPacketLength, 0,
													&pAgpdItem->m_pcsCharacter->m_szID,
													&((AgpdItemTemplate*)pAgpdItem->m_pcsItemTemplate)->m_lID,
													&pAgpdItem->m_nCount,
													&pAgpdItem->m_eStatus,
													&pAgpdItem->m_posItem,
													&pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
													&pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
													&pAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN],
													&lNeedLevel,		// JNY TODO 2004.3.3 : Need level
													&lDBID
													);

	if (!pvItemPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION_ITEM, 0, 
										AGSMRELAY_DB_OPERATION_UPDATE, NULL, pvItemPacket, NULL, NULL, NULL);

	m_csItemPacket.FreePacket(pvItemPacket);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmRelay::SendItemDelete(UINT64 lDBID)
{
	// Relay Server가 연결이 안되어 있으면 종료
	if (0 == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	INT16 nPacketLength	= 0;

	PVOID pvItemPacket = m_csItemPacket.MakePacket(FALSE, &nPacketLength, 0,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,		// JNY TODO 2004.3.3 : Need level
													&lDBID
													);

	if (!pvItemPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION_ITEM, 0, 
										AGSMRELAY_DB_OPERATION_DELETE, NULL, pvItemPacket, NULL, NULL, NULL);

	m_csItemPacket.FreePacket(pvItemPacket);

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

BOOL AgsmRelay::SendItemInsert2(AgpdItem* pAgpdItem, UINT64 lDBID)
{
	if (!pAgpdItem || !lDBID)
		return FALSE;

	INT16 nPacketLength = 0;

	INT32	lNeedLevel	= 0;
	m_pAgpmFactors->GetValue(&pAgpdItem->m_csRestrictFactor, &lNeedLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	INT32	lDurability	= 0;
	m_pAgpmFactors->GetValue(&pAgpdItem->m_csFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);

	AuPOS	stPos;

	if (pAgpdItem->m_eStatus == AGPDITEM_STATUS_FIELD)
		stPos	= pAgpdItem->m_posItem;
	else
	{
		stPos.x	= (FLOAT) pAgpdItem->m_anGridPos[0];
		stPos.y	= (FLOAT) pAgpdItem->m_anGridPos[1];
		stPos.z	= (FLOAT) pAgpdItem->m_anGridPos[2];
	}

	CHAR	szPosition[33];
	ZeroMemory(szPosition, sizeof(CHAR) * 33);

	m_pAgsmCharacter->EncodingPosition(&stPos, szPosition, 32);

	PVOID pvItemPacket = m_csItemPacket2.MakePacket(FALSE, &nPacketLength, 0,
													pAgpdItem->m_pcsCharacter->m_szID,
													&((AgpdItemTemplate*)pAgpdItem->m_pcsItemTemplate)->m_lID,
													&pAgpdItem->m_nCount,
													&pAgpdItem->m_eStatus,
													szPosition,
													&lNeedLevel,		// JNY TODO 2004.3.3 : Need level
													&lDurability,
													NULL,
													&lDBID
													);

	if (!pvItemPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_ITEM, 0, 
										AGSMRELAY_DB_OPERATION_INSERT, NULL, pvItemPacket, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	m_csItemPacket2.FreePacket(pvItemPacket);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmRelay::SendItemUpdate2(AgpdItem* pAgpdItem, UINT64 lDBID)
{
	if (!pAgpdItem || !pAgpdItem->m_pcsCharacter || !pAgpdItem->m_pcsItemTemplate)
	{
		//ASSERT(!"이런 상황이면 안되는데.... ");
		return FALSE;
	}

	// Relay Server가 연결이 안되어 있으면 종료
	if (0 == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	INT16 nPacketLength;

	INT32	lNeedLevel	= 0;
	m_pAgpmFactors->GetValue(&pAgpdItem->m_csRestrictFactor, &lNeedLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	INT32	lDurability	= 0;
	m_pAgpmFactors->GetValue(&pAgpdItem->m_csFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);

	AuPOS	stPos;

	if (pAgpdItem->m_eStatus == AGPDITEM_STATUS_FIELD)
		stPos	= pAgpdItem->m_posItem;
	else
	{
		stPos.x	= (FLOAT) pAgpdItem->m_anGridPos[0];
		stPos.y	= (FLOAT) pAgpdItem->m_anGridPos[1];
		stPos.z	= (FLOAT) pAgpdItem->m_anGridPos[2];
	}

	CHAR	szPosition[33];
	ZeroMemory(szPosition, sizeof(CHAR) * 33);

	m_pAgsmCharacter->EncodingPosition(&stPos, szPosition, 32);

	PVOID pvItemPacket = m_csItemPacket2.MakePacket(FALSE, &nPacketLength, 0,
													(pAgpdItem->m_pcsCharacter) ? pAgpdItem->m_pcsCharacter->m_szID : NULL,
													NULL,
													&pAgpdItem->m_nCount,
													&pAgpdItem->m_eStatus,
													szPosition,
													&lNeedLevel,		// JNY TODO 2004.3.3 : Need level
													&lDurability,
													NULL,
													&lDBID
													);

	if (!pvItemPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_ITEM, 0, 
										AGSMRELAY_DB_OPERATION_UPDATE, NULL, pvItemPacket, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	m_csItemPacket2.FreePacket(pvItemPacket);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmRelay::SendItemDelete2(UINT64 lDBID, CHAR *szDeleteReason)
{
	// Relay Server가 연결이 안되어 있으면 종료
	if (0 == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	INT16 nPacketLength	= 0;
	INT16 nDeleteReasonLength	= strlen(szDeleteReason);

	PVOID pvItemPacket = m_csItemPacket2.MakePacket(FALSE, &nPacketLength, 0,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													szDeleteReason, &nDeleteReasonLength,
													&lDBID
													);

	if (!pvItemPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_ITEM, 0, 
										AGSMRELAY_DB_OPERATION_DELETE, NULL, pvItemPacket, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	m_csItemPacket2.FreePacket(pvItemPacket);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}