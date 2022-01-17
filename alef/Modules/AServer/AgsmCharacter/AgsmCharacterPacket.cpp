#include "AgsmCharacter.h"

PVOID AgsmCharacter::MakePacket(INT16* pnPacketLength, INT8* pcOperation, INT32* plCID, INT8* pcStatus, CHAR* szZoneServerAddress,
								INT32* plZoneServerID, PVOID pvServerCharData, PVOID pvBanData, INT32 *plNewCID)
{
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMCHARACTER_PACKET_TYPE, 
														pcOperation,
														plCID,
														pcStatus,
														szZoneServerAddress,
														plZoneServerID,
														pvServerCharData,
														pvBanData,
														plNewCID);
}


PVOID AgsmCharacter::MakePacketAddCharacter(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, PVOID pvPacketFactor)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	PVOID	pvPacket;
	INT16	nSize;
	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_ADD;

	INT8	cMoveFlag;
	cMoveFlag = (pcsCharacter->m_eMoveDirection != MD_NODIRECTION ? AGPMCHARACTER_MOVE_FLAG_DIRECTION : 0) |
				(pcsCharacter->m_bPathFinding ? AGPMCHARACTER_MOVE_FLAG_PATHFINDING : 0) |
				(pcsCharacter->m_bMove ? 0 : AGPMCHARACTER_MOVE_FLAG_STOP) |
				(pcsCharacter->m_bSync ? AGPMCHARACTER_MOVE_FLAG_SYNC : 0) |
				(pcsCharacter->m_bMoveFast ? AGPMCHARACTER_MOVE_FLAG_FAST : 0) |
				(pcsCharacter->m_bMoveFollow ? AGPMCHARACTER_MOVE_FLAG_FOLLOW : 0) |
				(pcsCharacter->m_bHorizontal ? AGPMCHARACTER_MOVE_FLAG_HORIZONTAL : 0);

	INT8	cMoveDirection = (INT8)pcsCharacter->m_eMoveDirection;

	PVOID pvPacketMove = m_pcsAgpmCharacter->m_csPacketMove.MakePacket(FALSE, &nSize, 0, 
														&pcsCharacter->m_stPos,
														(pcsCharacter->m_bMove) ? &pcsCharacter->m_stDestinationPos : &pcsCharacter->m_stPos,
														&pcsCharacter->m_lFollowTargetID,
														&pcsCharacter->m_lFollowDistance,
														&pcsCharacter->m_fTurnX,
														&pcsCharacter->m_fTurnY,
														&cMoveFlag,
														&cMoveDirection);
	if (!pvPacketMove)
		return NULL;
	
	UINT8	ucBindingRegionIndex	= (UINT8)	pcsCharacter->m_nLastExistBindingIndex;

	UINT16	unIDLength	= (UINT16)strlen(GetRealCharName(pcsCharacter->m_szID));
	if (unIDLength > AGPDCHARACTER_MAX_ID_LENGTH)
		unIDLength	= AGPDCHARACTER_MAX_ID_LENGTH;

	INT8	cIsTransform		= 0;
	if (pcsCharacter->m_bIsTrasform)
		cIsTransform |= AGPMCHAR_FLAG_TRANSFORM;
	if (pcsCharacter->m_bRidable)
		cIsTransform |= AGPMCHAR_FLAG_RIDABLE;
	if (pcsCharacter->m_bIsEvolution)
		cIsTransform |= AGPMCHAR_FLAG_EVOLUTION;

	INT8	cFaceIndex	= (INT8)	pcsCharacter->m_lFaceIndex;
	INT8	cHairIndex	= (INT8)	pcsCharacter->m_lHairIndex;


	INT32	lRemainedCriminalTime = m_pcsAgpmCharacter->GetRemainedCriminalTime(pcsCharacter);
	INT32	lRemainedMurdererTime = m_pcsAgpmCharacter->GetRemainedMurdererTime(pcsCharacter);
	UINT32	lLastKilledTimeInBattleSquare = m_pcsAgpmCharacter->GetLastKilledTimeInBattleSquare(pcsCharacter);
	
	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE, 
														&cOperation,												// Operation
														&pcsCharacter->m_lID,										// Character ID
														&pcsCharacter->m_lTID1,										// Character Template ID
														GetRealCharName(pcsCharacter->m_szID), &unIDLength,			// Game ID
														&pcsCharacter->m_unCurrentStatus,							// Character Status
														pvPacketMove,												// Move Packet
														NULL,														// Action Packet
														pvPacketFactor,												// Factor Packet
														&pcsCharacter->m_llMoney,									// llMoney
														&pcsCharacter->m_llBankMoney,								// bank money
														&pcsCharacter->m_llCash,									// cash
														&pcsCharacter->m_unActionStatus,							// character action status
														&pcsCharacter->m_unCriminalStatus,							// character criminal status
														NULL,														// attacker id (정당방위 설정에 필요)
														NULL,														// 새로 생성되서 맵에 들어간넘인지 여부
														&ucBindingRegionIndex,										// region index
														NULL,														// social action index
														&pcsCharacter->m_ulSpecialStatus,							// special status
														&cIsTransform,												// is transform status
														pcsCharacter->m_szSkillInit,								// skill initialization text
														&cFaceIndex,												// face index
														&cHairIndex,												// hair index
														NULL,														// Option Flag
														&pcsCharacter->m_cBankSize,									// bank size
														&pcsCharacter->m_unEventStatusFlag,							// event status flag
														&lRemainedCriminalTime,										// remained criminal status time
														&lRemainedMurdererTime,										// remained murderer point time
														pcsCharacter->m_szNickName,									// nick name
														NULL,														// gameguard
														&lLastKilledTimeInBattleSquare								// last killed time in battlesquare
														);

	m_pcsAgpmCharacter->m_csPacketMove.FreePacket(pvPacketMove);

	return pvPacket;
}

PVOID AgsmCharacter::MakePacketBase(ApBase *pcsBase)
{
	if (!pcsBase)
		return NULL;

	INT16	nPacketLength = 0;

	return m_csPacketBase.MakePacket(FALSE, &nPacketLength, 0,
									&pcsBase->m_eType,
									&pcsBase->m_lID
									);
}

PVOID AgsmCharacter::MakePacketBanData(AgpdCharacter* pcsCharacter, INT16* pnPacketLength)
{
	if(!pcsCharacter || !pnPacketLength)
		return NULL;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return NULL;

	PVOID pvBanData = m_csPacketBanData.MakePacket(FALSE, pnPacketLength, 0,
												&pcsAgsdCharacter->m_stBan.m_lChatBanStartTime,
												&pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime,
												&pcsAgsdCharacter->m_stBan.m_lCharBanStartTime,
												&pcsAgsdCharacter->m_stBan.m_lCharBanKeepTime,
												&pcsAgsdCharacter->m_stBan.m_lAccBanStartTime,
												&pcsAgsdCharacter->m_stBan.m_lAccBanKeepTime
												);
	if(!pvBanData)
	{
		OutputDebugString("AgsmCharacter::MakePacketBanData(...) pvBanData is NULL\n");
		return NULL;
	}

	INT8 cOperation = AGSMCHAR_PACKET_OPERATION_BAN_DATA;
	PVOID pvPacket = MakePacket(pnPacketLength, &cOperation, &pcsCharacter->m_lID, NULL, NULL, NULL, NULL, pvBanData);

	m_csPacketBanData.FreePacket(pvBanData);

	return pvPacket;
}

PVOID AgsmCharacter::MakePacketHistory(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter)
		return NULL;

	if (pcsAgsdCharacter->m_stHistory.m_lEntryNum < 1)
		return NULL;

	INT16	nPacketLength = 0;

//	PVOID	pvPacketHistoryEntries[AGSD_CHARACTER_HISTORY_NUMBER];
//	ZeroMemory(pvPacketHistoryEntries, sizeof(PVOID) * AGSD_CHARACTER_HISTORY_NUMBER);

	ApSafeArray<PVOID, AGSD_CHARACTER_HISTORY_NUMBER>	pvPacketHistoryEntries;
	pvPacketHistoryEntries.MemSetAll();

	for (int i = 0; i < pcsAgsdCharacter->m_stHistory.m_lEntryNum; ++i)
	{
		INT8	cFirstHit = (INT8) pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_bFirstHit;

		PVOID	pvPacketBase = MakePacketBase((ApBase *) &pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_csSource);

		if (!pvPacketBase)
			continue;

		PVOID	pvPacketFactor = m_pagpmFactors->MakePacketFactors(pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor);
		if (!pvPacketFactor)
		{
			m_csPacketBase.FreePacket(pvPacketBase);
			continue;
		}

		pvPacketHistoryEntries[i] = m_csPacketHistoryEntry.MakePacket(FALSE, &nPacketLength, 0,
														pvPacketBase,
														pvPacketFactor,
														&pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_ulFirstHitTime,
														&pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_ulLastHitTime,
														&cFirstHit,
														&pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_fAgro
														);

		m_csPacketBase.FreePacket(pvPacketBase);
		m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);
	}

	PVOID	pvPacketHistory = m_csPacketHistory.MakePacket(FALSE, &nPacketLength, 0,
														&pcsAgsdCharacter->m_stHistory.m_lEntryNum,
														pvPacketHistoryEntries
														);

	for (int i = 0; i < pcsAgsdCharacter->m_stHistory.m_lEntryNum; ++i)
	{
		if (pvPacketHistoryEntries[i])
			m_csPacketHistoryEntry.FreePacket(pvPacketHistoryEntries[i]);
	}

	return pvPacketHistory;
}

PVOID AgsmCharacter::MakePacketAgsdCharacter(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter)
		return NULL;

	PVOID	pvPacket = NULL;
	INT8	cOperation = AGSMCHAR_PACKET_OPERATION_UPDATE;

	PVOID	pvPacketHistory = MakePacketHistory(pcsCharacter);

	UINT32	ulCurrentClockCount	= GetClockCount();

	INT8	cDestroyWhenDie = (INT8) pcsAgsdCharacter->m_bDestroyWhenDie;

	AgpdTeleportAttachToChar	*pcsTeleportAttachData	= m_pcsAgpmEventTeleport->GetADCharacter(pcsCharacter);

	PVOID	pvPacketCharData = m_csPacketServerData.MakePacket(FALSE, pnPacketLength, 0,
														&pcsAgsdCharacter->m_ulServerID,
														pcsAgsdCharacter->m_szAccountID,
														&pcsAgsdCharacter->m_ulPrevSendMoveTime,
														&pcsAgsdCharacter->m_ulNextAttackTime,
														&pcsAgsdCharacter->m_ulNextSkillTime,
														&cDestroyWhenDie,
														pvPacketHistory,
														&pcsAgsdCharacter->m_ullDBID,
														pcsAgsdCharacter->m_szLastShoutWord,
														&pcsAgsdCharacter->m_ulLastUpdateMukzaPointTime,
														&pcsAgsdCharacter->m_ulRemainUpdateMukzaPointTime,
														&pcsAgsdCharacter->m_lReceivedSkillPoint,
														&pcsAgsdCharacter->m_ulLastUpdateActionStatusTime,
														&ulCurrentClockCount,
														(pcsTeleportAttachData->m_pcsLastUseTeleportPoint) ? &pcsTeleportAttachData->m_pcsLastUseTeleportPoint->m_szPointName : NULL,
														pcsAgsdCharacter->m_szServerName,
														&pcsAgsdCharacter->m_lMemberBillingNum
														);

	if (pvPacketHistory)
		m_csPacketHistory.FreePacket(pvPacketHistory);

	if (!pvPacketCharData)
		return NULL;

	pvPacket = MakePacket(pnPacketLength, &cOperation, &pcsCharacter->m_lID, NULL, NULL, NULL, pvPacketCharData, NULL);

	m_csPacketServerData.FreePacket(pvPacketCharData);

	return pvPacket;
}

PVOID AgsmCharacter::MakePacketAgsdCharacterLogin(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter)
		return NULL;

	UINT32	ulClockCount	= GetClockCount();

	PVOID	pvPacket = NULL;
	INT8	cOperation = AGSMCHAR_PACKET_OPERATION_UPDATE;

	AgpdTeleportAttachToChar	*pcsTeleportAttachData	= m_pcsAgpmEventTeleport->GetADCharacter(pcsCharacter);

	PVOID	pvPacketCharData = m_csPacketServerData.MakePacket(FALSE, pnPacketLength, 0,
														&pcsAgsdCharacter->m_ulServerID,
														pcsAgsdCharacter->m_szAccountID,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														&pcsAgsdCharacter->m_ullDBID,
														NULL,
														&pcsAgsdCharacter->m_ulLastUpdateMukzaPointTime,
														&pcsAgsdCharacter->m_ulRemainUpdateMukzaPointTime,
														&pcsAgsdCharacter->m_lReceivedSkillPoint,
														NULL,
														&ulClockCount,
														(pcsTeleportAttachData->m_pcsLastUseTeleportPoint) ? pcsTeleportAttachData->m_pcsLastUseTeleportPoint->m_szPointName : NULL,
														pcsAgsdCharacter->m_szServerName,
														&pcsAgsdCharacter->m_lMemberBillingNum
														);

	if (!pvPacketCharData)
		return NULL;

	pvPacket = MakePacket(pnPacketLength, &cOperation, &pcsCharacter->m_lID, NULL, NULL, NULL, pvPacketCharData, NULL);

	m_csPacketServerData.FreePacket(pvPacketCharData);

	return pvPacket;
}

BOOL AgsmCharacter::SendCellToChar(AgsmAOICell *pcsCell, AgpdCharacter *pCharacter)
{
	PROFILE("AgsmCharacter::SendCellToChar");

	if (!pcsCell || !pCharacter)
		return FALSE;

//	INT32		lCharacterBuffer[512];
//	ZeroMemory(lCharacterBuffer, sizeof(INT32) * 512);

	AgsdCharacter	*		pcsAgsdCharacter = GetADCharacter(pCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("SendCellToChar"));

	ApSafeArray<INT_PTR, 512>		lCharacterBuffer;
	lCharacterBuffer.MemSetAll();

	AgsmCellUnit	*pcsCellUnit	= NULL;

	int	nIndex	= 0;

	pcsCell->m_csRWLock.LockReader();

	// 섹터에서 캐릭터 데이타를 가져온다.

	if (pcsCell->GetCharacterUnit())
	{
		for (pcsCellUnit = pcsCell->GetCharacterUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT_PTR) pCharacter)
				continue;

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}

	/*
	if (pcsCell->GetNPCUnit())
	{
		for (pcsCellUnit = pcsCell->GetNPCUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT32) pCharacter)
				continue;

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}
	*/

	if (pcsCell->GetMonsterUnit())
	{
		for (pcsCellUnit = pcsCell->GetMonsterUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT_PTR) pCharacter)
				continue;

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}

	pcsCell->m_csRWLock.UnlockReader();

	for (int i = 0; i < nIndex; ++i)
	{
		AgpdCharacter	*pcsCellChar = (AgpdCharacter *) lCharacterBuffer[i];
		if (!pcsCellChar)
			continue;

		if (pcsCellChar->m_bIsReadyRemove)
			continue;

		if (!pcsCellChar->m_Mutex.WLock())
			continue;

		SendPacketCharView(pcsCellChar, pcsAgsdCharacter->m_dpnidCharacter, FALSE);

		pcsCellChar->m_Mutex.Release();
	}

	EnumCallback(AGSMCHARACTER_CB_SEND_CELL_INFO, pcsCell, &pcsAgsdCharacter->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmCharacter::SendCellNPCToChar(AgsmAOICell *pcsCell, AgpdCharacter *pCharacter)
{
	PROFILE("AgsmCharacter::SendCellNPCToChar");

	if (!pcsCell || !pCharacter)
		return FALSE;

//	INT32		lCharacterBuffer[512];
//	ZeroMemory(lCharacterBuffer, sizeof(INT32) * 512);

	AgsdCharacter	*		pcsAgsdCharacter = GetADCharacter(pCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("SendCellNPCToChar"));

	ApSafeArray<INT_PTR, 512>		lCharacterBuffer;
	lCharacterBuffer.MemSetAll();

	AgsmCellUnit	*pcsCellUnit	= NULL;

	int	nIndex	= 0;

	pcsCell->m_csRWLock.LockReader();

	// 섹터에서 캐릭터 데이타를 가져온다.

	/*
	if (pcsCell->GetCharacterUnit())
	{
		for (pcsCellUnit = pcsCell->GetCharacterUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT32) pCharacter)
				continue;

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}
	*/

	// 마을 NPC 전송은 region 변경 때만 하도록 수정 2005.06.24 by kelovon
	ApmMap::RegionTemplate *pstCurrentRegionTemplate = m_papmMap->GetTemplate(pCharacter->m_nBindingRegionIndex);
	BOOL bIsTown = (pstCurrentRegionTemplate != NULL
					&& pstCurrentRegionTemplate->ti.stType.uFieldType == ApmMap::FT_TOWN);

	if (pcsCell->GetNPCUnit())
	{
		for (pcsCellUnit = pcsCell->GetNPCUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT_PTR) pCharacter)
				continue;

			// 마을에 있으면서 같은 마을에 있는 NPC이면 전송 안 함
			if (bIsTown
				&& ((AgpdCharacter*)pcsCellUnit->lID)->m_pcsCharacterTemplate->m_ulCharType == AGPMCHAR_TYPE_NPC
				&& pCharacter->m_nBindingRegionIndex ==  ((AgpdCharacter*)pcsCellUnit->lID)->m_nBindingRegionIndex)
			{
				continue;
			}

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}

	/*
	if (pcsCell->GetMonsterUnit())
	{
		for (pcsCellUnit = pcsCell->GetMonsterUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT32) pCharacter)
				continue;

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}
	*/

	pcsCell->m_csRWLock.UnlockReader();

	for (int i = 0; i < nIndex; ++i)
	{
		AgpdCharacter	*pcsCellChar = (AgpdCharacter *) lCharacterBuffer[i];
		if (!pcsCellChar)
			continue;

		//if (!pcsCellChar->m_Mutex.WLock())
		//	continue;

		SendPacketCharView(pcsCellChar, pcsAgsdCharacter->m_dpnidCharacter, FALSE);

		//pcsCellChar->m_Mutex.Release();
	}

	//EnumCallback(AGSMCHARACTER_CB_SEND_CELL_INFO, pcsCell, &pcsAgsdCharacter->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmCharacter::SendRegionNPCToChar(INT16 nRegionIndex, AgpdCharacter *pCharacter)
{
	PROFILE("AgsmCharacter::SendRegionNPCToChar");

	if (!pCharacter)
		return FALSE;

	AgsdCharacter	*		pcsAgsdCharacter = GetADCharacter(pCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("SendRegionNPCToChar"));

	map<INT16, vector<AgpdCharacter*> >::iterator itr
		= m_pcsAgpmCharacter->m_mapTownToNPCList.find(pCharacter->m_nBindingRegionIndex);

	if (m_pcsAgpmCharacter->m_mapTownToNPCList.end() == itr)
	{
		return FALSE;
	}

	vector<AgpdCharacter*> &tmpNPCVector = itr->second;

	for (int i = 0; i < tmpNPCVector.size(); i++)
	{
		SendPacketCharView(tmpNPCVector[i], pcsAgsdCharacter->m_dpnidCharacter, FALSE);
	}

	return TRUE;
}

BOOL AgsmCharacter::SendRegionNPCRemoveToChar(INT16 nRegionIndex, AgpdCharacter *pCharacter)
{
	PROFILE("AgsmCharacter::SendRegionNPCRemoveToChar");

	if (!pCharacter)
		return FALSE;

	AgsdCharacter	*		pcsAgsdCharacter = GetADCharacter(pCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("SendRegionNPCRemoveToChar"));

	map<INT16, vector<AgpdCharacter*> >::iterator itr
		= m_pcsAgpmCharacter->m_mapTownToNPCList.find(nRegionIndex);

	if (m_pcsAgpmCharacter->m_mapTownToNPCList.end() == itr)
	{
		return FALSE;
	}

	vector<AgpdCharacter*> &tmpNPCVector = itr->second;

	for (int i = 0; i < tmpNPCVector.size(); i++)
	{
		SendPacketCharRemoveForView(tmpNPCVector[i], pcsAgsdCharacter->m_dpnidCharacter, FALSE);
	}

	return TRUE;
}

BOOL AgsmCharacter::SendCharToCell(AgpdCharacter *pCharacter, AgsmAOICell *pcsCell, BOOL bIsNewChar)
{
	PROFILE("AgsmCharacter::SendCharToCell");

	if (!pCharacter || !pcsCell)
		return FALSE;

	if (pcsCell->GetGroupID() == 0)
		return TRUE;

	return SendPacketCharView(pCharacter, pcsCell->GetGroupID(), TRUE, bIsNewChar, TRUE);
}

BOOL AgsmCharacter::SendCellRemoveToChar(AgsmAOICell *pcsCell, AgpdCharacter *pCharacter)
{
	PROFILE("AgsmCharacter::SendCellRemoveToChar");

	if (!pcsCell || !pCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = GetADCharacter(pCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("SendCellRemoveToChar"));

	AgsmCellUnit	*pcsCellUnit	= NULL;

	int	nIndex	= 0;

//	INT32		lCharacterBuffer[512];
//	ZeroMemory(lCharacterBuffer, sizeof(INT32) * 512);

	ApSafeArray<INT_PTR, 512>		lCharacterBuffer;
	lCharacterBuffer.MemSetAll();

	pcsCell->m_csRWLock.LockReader();

	if (pcsCell->GetCharacterUnit())
	{
		for (pcsCellUnit = pcsCell->GetCharacterUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT_PTR) pCharacter)
				continue;

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}

	/*
	if (pcsCell->GetNPCUnit())
	{
		for (pcsCellUnit = pcsCell->GetNPCUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT32) pCharacter)
				continue;

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}
	*/

	if (pcsCell->GetMonsterUnit())
	{
		for (pcsCellUnit = pcsCell->GetMonsterUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT_PTR) pCharacter)
				continue;

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}

	pcsCell->m_csRWLock.UnlockReader();

	// 캐릭터를 삭제하라는 패킷을 보낸다.
	for (int i = 0; i < nIndex; ++i)
	{
		SendPacketCharRemoveForView((AgpdCharacter *) lCharacterBuffer[i], pcsAgsdCharacter->m_dpnidCharacter, FALSE);
	}

	EnumCallback(AGSMCHARACTER_CB_SEND_CELL_REMOVE_INFO, pcsCell, &pcsAgsdCharacter->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmCharacter::SendCellNPCRemoveToChar(AgsmAOICell *pcsCell, AgpdCharacter *pCharacter)
{
	PROFILE("AgsmCharacter::SendCellNPCRemoveToChar");

	if (!pcsCell || !pCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = GetADCharacter(pCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("SendCellNPCRremoveToChar"));

	AgsmCellUnit	*pcsCellUnit	= NULL;

	int	nIndex	= 0;

//	INT32		lCharacterBuffer[512];
//	ZeroMemory(lCharacterBuffer, sizeof(INT32) * 512);

	ApSafeArray<INT_PTR, 512>		lCharacterBuffer;
	lCharacterBuffer.MemSetAll();

	pcsCell->m_csRWLock.LockReader();

	/*
	if (pcsCell->GetCharacterUnit())
	{
		for (pcsCellUnit = pcsCell->GetCharacterUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT32) pCharacter)
				continue;

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}
	*/

	// 마을 NPC 전송은 region 변경 때만 하도록 수정 2005.06.24 by kelovon
	ApmMap::RegionTemplate *pstCurrentRegionTemplate = m_papmMap->GetTemplate(pCharacter->m_nBindingRegionIndex);
	BOOL bIsTown = (pstCurrentRegionTemplate != NULL
					&& pstCurrentRegionTemplate->ti.stType.uFieldType == ApmMap::FT_TOWN);

	if (pcsCell->GetNPCUnit())
	{
		for (pcsCellUnit = pcsCell->GetNPCUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT_PTR) pCharacter)
				continue;

			// 마을에 있으면서 같은 마을에 있는 NPC이면 전송 안 함
			if (bIsTown
				&& ((AgpdCharacter*)pcsCellUnit->lID)->m_pcsCharacterTemplate->m_ulCharType == AGPMCHAR_TYPE_NPC
				&& pCharacter->m_nBindingRegionIndex ==  ((AgpdCharacter*)pcsCellUnit->lID)->m_nBindingRegionIndex)
			{
				continue;
			}

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}

	/*
	if (pcsCell->GetMonsterUnit())
	{
		for (pcsCellUnit = pcsCell->GetMonsterUnit(); pcsCellUnit; pcsCellUnit = pcsCell->GetNext(pcsCellUnit))
		{
			if (pcsCellUnit->lID == 0 ||
				pcsCellUnit->lID == (INT32) pCharacter)
				continue;

			lCharacterBuffer[nIndex++] = pcsCellUnit->lID;

			if (nIndex >= 512)
				break;
		}
	}
	*/

	pcsCell->m_csRWLock.UnlockReader();

	// 캐릭터를 삭제하라는 패킷을 보낸다.
	for (int i = 0; i < nIndex; ++i)
	{
		SendPacketCharRemoveForView((AgpdCharacter *) lCharacterBuffer[i], pcsAgsdCharacter->m_dpnidCharacter, FALSE);
	}

	//EnumCallback(AGSMCHARACTER_CB_SEND_CELL_REMOVE_INFO, pcsCell, &pcsAgsdCharacter->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmCharacter::SendCharRemoveToCell(AgpdCharacter *pCharacter, AgsmAOICell *pcsCell, BOOL bIsForView)
{
	PROFILE("AgsmCharacter::SendCharRemoveToCell");

	if (!pCharacter || !pcsCell)
		return FALSE;

	if (pcsCell->GetGroupID() == 0)
		return TRUE;

	if (bIsForView)
		return SendPacketCharRemove(pCharacter, pcsCell->GetGroupID(), TRUE);
	else
		return SendPacketCharRemoveForView(pCharacter, pcsCell->GetGroupID(), TRUE);

	return TRUE;
}

BOOL AgsmCharacter::SendPacketCharBasicData(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bGroupNID)
{
	if (!pcsCharacter)
		return FALSE;

	if (ulNID == 0)
		return TRUE;

	INT16	nPacketLength	= 0;

	PVOID	pvPacketBasic	= MakePacketAgpdDBData(pcsCharacter, &nPacketLength);

	if (!pvPacketBasic || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= FALSE;
	
	if (bGroupNID)
	{
		bSendResult	= m_pagsmAOIFilter->SendPacketGroup(pvPacketBasic, nPacketLength, ulNID, PACKET_PRIORITY_4);
	}
	else
	{
		bSendResult	= SendPacket(pvPacketBasic, nPacketLength, ulNID);
	}

	m_csPacket.FreePacket(pvPacketBasic);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketCharView(AgpdCharacter *pCharacter, UINT_PTR ulNID, BOOL bGroupNID, BOOL bIsNewChar, BOOL bIsExceptSelf)
{
	PROFILE("AgsmCharacter::SendPacketCharView");
	
	if (!pCharacter)
		return FALSE;

	if (ulNID == 0)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("SendPacketCharView"));

	BOOL	bSendResult	= TRUE;

	if (m_pagpmOptimizedPacket2)
	{
		INT16	nPacketLength	= 0;
		PVOID	pvPacketView	= NULL;
		
		{
			PROFILE("MakePacketCharView");
			pvPacketView	= m_pagpmOptimizedPacket2->MakePacketCharView(pCharacter, &nPacketLength, bIsNewChar);
		}

		if (!pvPacketView || nPacketLength < 1)
			return FALSE;

		bSendResult	= FALSE;

		{
			PROFILE("SendPacket");

			if (bGroupNID)
			{
				if (bIsExceptSelf)
				{
					bSendResult = m_pagsmAOIFilter->SendPacketGroupExceptSelf(pvPacketView, nPacketLength, ulNID, GetCharDPNID(pCharacter), PACKET_PRIORITY_1);
				}
				else
				{
					bSendResult = m_pagsmAOIFilter->SendPacketGroup(pvPacketView, nPacketLength, ulNID, PACKET_PRIORITY_1);
				}
			}
			else
				bSendResult	= SendPacket(pvPacketView, nPacketLength, (UINT32)ulNID, PACKET_PRIORITY_1);
		}

		m_csPacket.FreePacket(pvPacketView);

		// 2005.01.18. steeple
		PVOID	pvBuffer[3];
		pvBuffer[0]	= (PVOID) ulNID;
		pvBuffer[1] = IntToPtr(bGroupNID);
		pvBuffer[2] = IntToPtr(bIsExceptSelf);

		EnumCallback(AGSMCHARACTER_CB_SEND_CHAR_VIEW, pCharacter, pvBuffer);
	}
//	else if (m_pagpmOptimizedPacket)
//	{
//		INT16	nPacketLength	= 0;
//		PVOID	pvPacketView	= m_pagpmOptimizedPacket->MakePacketView(pCharacter, bIsNewChar, &nPacketLength);
//
//		if (!pvPacketView)
//			return FALSE;
//
//		bSendResult	= FALSE;
//
//		if (bGroupNID)
//		{
//			if (bIsExceptSelf)
//			{
//				bSendResult = m_pagsmAOIFilter->SendPacketGroupExceptSelf(pvPacketView, nPacketLength, ulNID, GetCharDPNID(pCharacter));
//			}
//			else
//			{
//				bSendResult = m_pagsmAOIFilter->SendPacketGroup(pvPacketView, nPacketLength, ulNID);
//			}
//		}
//		else
//			bSendResult	= SendPacket(pvPacketView, nPacketLength, ulNID);
//
//		m_pagpmOptimizedPacket->m_csPacketView.FreePacket(pvPacketView);
//	}
	else
	{
		PVOID	pvPacket		= NULL;
		INT16	nPacketLength	= 0;

		{
			PROFILE("AgsmCharacter::SendPacketCharView - MakePacket");

			INT8 cOperation = AGPMCHAR_PACKET_OPERATION_ADD;
			
			INT8 cMoveFlag = (pCharacter->m_eMoveDirection != MD_NODIRECTION ? AGPMCHARACTER_MOVE_FLAG_DIRECTION : 0) |
							 (pCharacter->m_bPathFinding ? AGPMCHARACTER_MOVE_FLAG_PATHFINDING : 0) |
			 				 (pCharacter->m_bMove ? 0 : AGPMCHARACTER_MOVE_FLAG_STOP) |
							 (pCharacter->m_bSync ? AGPMCHARACTER_MOVE_FLAG_SYNC : 0) |
							 (pCharacter->m_bMoveFast ? AGPMCHARACTER_MOVE_FLAG_FAST : 0) |
							 (pCharacter->m_bMoveFollow ? AGPMCHARACTER_MOVE_FLAG_FOLLOW : 0) |
							 (pCharacter->m_bHorizontal ? AGPMCHARACTER_MOVE_FLAG_HORIZONTAL : 0);

			INT8 cMoveDirection = (INT8)pCharacter->m_eMoveDirection;

			PVOID pvPacketMove = m_pcsAgpmCharacter->m_csPacketMove.MakePacket(FALSE, &nPacketLength, 0, 
																	&pCharacter->m_stPos,
																	(pCharacter->m_bMove) ? &pCharacter->m_stDestinationPos : &pCharacter->m_stPos,
																	&pCharacter->m_lFollowTargetID,
																	&pCharacter->m_lFollowDistance,
																	&pCharacter->m_fTurnX,
																	&pCharacter->m_fTurnY,
																	&cMoveFlag,
																	&cMoveDirection);
			if (!pvPacketMove)
				return FALSE;

			PVOID pvPacketFactor = m_pagpmFactors->MakePacketFactorsCharView(&pCharacter->m_csFactor);
			if (!pvPacketFactor)
			{
				m_pcsAgpmCharacter->m_csPacketMove.FreePacket(pvPacketMove);
				return FALSE;
			}

			INT8	cIsNewChar	= (INT8) bIsNewChar;

			UINT16	unIDLength	= (UINT16)strlen(pCharacter->m_szID);
			if (unIDLength > AGPDCHARACTER_MAX_ID_LENGTH)
				unIDLength	= AGPDCHARACTER_MAX_ID_LENGTH;

			INT8	cIsTransform		= 0;
			if (pCharacter->m_bIsTrasform)
				cIsTransform |= AGPMCHAR_FLAG_TRANSFORM;
			if (pCharacter->m_bRidable)
				cIsTransform |= AGPMCHAR_FLAG_RIDABLE;
			if (pCharacter->m_bIsEvolution)
				cIsTransform |= AGPMCHAR_FLAG_EVOLUTION;

			INT8	cFaceIndex	= (INT8)	pCharacter->m_lFaceIndex;
			INT8	cHairIndex	= (INT8)	pCharacter->m_lHairIndex;

			INT32 lRemainedCriminalTime = m_pcsAgpmCharacter->GetRemainedCriminalTime(pCharacter);
			INT32 lRemainedMurdererTime = m_pcsAgpmCharacter->GetRemainedMurdererTime(pCharacter);

			pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE, 
																	&cOperation,										// Operation
																	&pCharacter->m_lID,									// Character ID
																	&pCharacter->m_lTID1,								// Character Template ID
																	pCharacter->m_szID, &unIDLength,					// Game ID
																	&pCharacter->m_unCurrentStatus,						// Character Status
																	pvPacketMove,										// Move Packet
																	NULL,												// Action Packet
																	pvPacketFactor,										// Factor Packet
																	NULL,												// llMoney
																	NULL,												// bank money
																	NULL,												// cash
																	&pCharacter->m_unActionStatus,						// character action status
																	&pCharacter->m_unCriminalStatus,					// character criminal status
																	NULL,												// attacker id (정당방위 설정에 필요)
																	(bIsNewChar) ? &cIsNewChar : NULL,					// 새로 생성되서 맵에 들어간넘인지 여부
																	NULL,												// region index
																	NULL,												// social action index
																	&pCharacter->m_ulSpecialStatus,						// special status
																	&cIsTransform,										// is transform status
																	NULL,												// skill initialization text
																	&cFaceIndex,										// face index
																	&cHairIndex,										// hair index
																	NULL,												// Option Flag
																	NULL,												// bank size
																	&pCharacter->m_unEventStatusFlag,					// event status flag
																	&lRemainedCriminalTime,								// remained criminal status time
																	&lRemainedMurdererTime,								// remained murderer point time
																	pCharacter->m_szNickName,							// nick name
																	NULL,												// gameguard
																	NULL												// last killed time in battlesquare
																	);

			m_pcsAgpmCharacter->m_csPacketMove.FreePacket(pvPacketMove);
			m_pcsAgpmCharacter->m_csPacketMove.FreePacket(pvPacketFactor);

			if (!pvPacket)
				return FALSE;
		}

		bSendResult	= FALSE;

		{
			PROFILE("AgsmCharacter::SendPacketCharView - SendPacket");

			if (bGroupNID)
			{
				if (bIsExceptSelf)
				{
					bSendResult = m_pagsmAOIFilter->SendPacketGroupExceptSelf(pvPacket, nPacketLength, ulNID, GetCharDPNID(pCharacter), PACKET_PRIORITY_1);
				}
				else
				{
					bSendResult = m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_1);
				}
			}
			else
				bSendResult	= SendPacket(pvPacket, nPacketLength, (UINT32)ulNID, PACKET_PRIORITY_1);
		}

		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

		PVOID	pvBuffer[3];
		pvBuffer[0]	= (PVOID) ulNID;
		pvBuffer[1] = IntToPtr(bGroupNID);
		pvBuffer[2] = IntToPtr(bIsExceptSelf);

		{
			PROFILE("AgsmCharacter::SendPacketCharView - Callback");

			// 여기서 아템 정보를 보내라고 callback을 호출한다.
			EnumCallback(AGSMCHARACTER_CB_SENDEQUIPITEM, pCharacter, pvBuffer);
		}
	}

	return bSendResult;
}

PVOID AgsmCharacter::MakePacketCharRemove(INT32 lCID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !pnPacketLength)
		return NULL;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_REMOVE;
	
	return m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,							// Operation
															&lCID,									// Character ID
															NULL,									// Character Template ID
															NULL,									// Game ID
															NULL,									// Character Status
															NULL,									// Move Packet
															NULL,									// Action Packet
															NULL,									// Factor Packet
															NULL,									// llMoney
															NULL,									// bank money
															NULL,									// cash
															NULL,									// character action status
															NULL,									// character criminal status
															NULL,									// attacker id (정당방위 설정에 필요)
															NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
															NULL,									// region index
															NULL,									// social action index
															NULL,									// special status
															NULL,									// is transform status
															NULL,									// skill initialization text
															NULL,									// face index
															NULL,									// hair index
															NULL,									// Option Flag
															NULL,									// bank size
															NULL,									// event status flag
															NULL,									// remained criminal status time
															NULL,									// remained murderer point time
															NULL,									// nick name
															NULL,									// gameguard
															NULL									// last killed time in battlesquare
															);
}

BOOL AgsmCharacter::SendPacketCharRemove(AgpdCharacter *pCharacter, UINT_PTR ulNID, BOOL bGroupNID)
{
	if (!pCharacter || !ulNID)
		return FALSE;

	return SendPacketCharRemove(pCharacter->m_lID, ulNID, bGroupNID);
}

BOOL AgsmCharacter::SendPacketCharRemove(INT32 lCID, UINT_PTR ulNID, BOOL bGroupNID)
{
	if (lCID == AP_INVALID_CID || !ulNID)
		return FALSE;

	INT16 nPacketLength;
	
	PVOID pvPacket = MakePacketCharRemove(lCID, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	BOOL	bSendResult	= FALSE;
	
	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, lCID);

	if (bGroupNID)
		bSendResult	= m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_1);
	else
		bSendResult	= SendPacket(pvPacket, nPacketLength, (UINT32)ulNID, PACKET_PRIORITY_1);
	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketCharRemoveForView(AgpdCharacter *pCharacter, UINT_PTR ulNID, BOOL bGroupNID)
{
	if (!pCharacter || !ulNID)
		return FALSE;

	return SendPacketCharRemoveForView(pCharacter->m_lID, ulNID, bGroupNID);
}

BOOL AgsmCharacter::SendPacketCharRemoveForView(INT32 lCID, UINT_PTR ulNID, BOOL bGroupNID)
{
	if (lCID == AP_INVALID_CID || !ulNID)
		return FALSE;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_REMOVE_FOR_VIEW;
	INT16 nPacketLength;
	
	PVOID pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,						// Operation
															&lCID,								// Character ID
															NULL,								// Character Template ID
															NULL,								// Game ID
															NULL,								// Character Status
															NULL,								// Move Packet
															NULL,								// Action Packet
															NULL,								// Factor Packet
															NULL,								// llMoney
															NULL,								// bank money
															NULL,								// cash
															NULL,								// character action status
															NULL,								// character criminal status
															NULL,								// attacker id (정당방위 설정에 필요)
															NULL,								// 새로 생성되서 맵에 들어간넘인지 여부
															NULL,								// region index
															NULL,								// social action index
															NULL,								// special status
															NULL,								// is transform status
															NULL,								// skill initialization text
															NULL,								// face index
															NULL,								// hair index
															NULL,								// Option Flag
															NULL,								// bank size
															NULL,								// event status flag
															NULL,								// remained criminal status time
															NULL,								// remained murderer point time
															NULL,								// nick name
															NULL,								// gameguard
															NULL								// last killed time in battlesquare
															);

	if (!pvPacket)
		return FALSE;

	BOOL	bSendResult	= FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, lCID);

	if (bGroupNID)
		bSendResult	= m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_1);
	else
		bSendResult	= SendPacket(pvPacket, nPacketLength, (UINT32)ulNID, PACKET_PRIORITY_1);
	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketMurdererPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter || pcsAgsdCharacter->m_dpnidCharacter == 0)	return FALSE;

	INT32 lRemainedMurdererTime = m_pcsAgpmCharacter->GetRemainedMurdererTime(pcsCharacter);

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_UPDATE;

	PVOID	pvPacketFactor	= m_pagpmFactors->MakePacketFactors(&pcsCharacter->m_csFactor);
	if (!pvPacketFactor)
		return FALSE;

	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
													&cOperation,								// Operation
													&pcsCharacter->m_lID,						// Character ID
													NULL,										// Character Template ID
													NULL,										// Game ID
													NULL,										// Character Status
													NULL,										// Move Packet
													NULL,										// Action Packet
													pvPacketFactor,								// Factor Packet
													NULL,										// llMoney
													NULL,										// bank money
													NULL,										// cash
													NULL,										// character action status
													NULL,										// character criminal status
													NULL,										// attacker id (정당방위 설정에 필요)
													NULL,										// 새로 생성되서 맵에 들어간넘인지 여부
													NULL,										// region index
													NULL,										// social action index
													NULL,										// special status
													NULL,										// is transform status
													NULL,										// skill initialization text
													NULL,										// face index
													NULL,										// hair index
													NULL,										// Option Flag
													NULL,										// bank size
													NULL,										// event status flag
													NULL,										// remained criminal status time
													&lRemainedMurdererTime,						// remained murderer point time
													NULL,										// nick name
													NULL,										// gameguard
													NULL										// last killed time in battlesquare
													);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	BOOL bSendResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketNickName(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter || pcsAgsdCharacter->m_dpnidCharacter == 0)	return FALSE;

	INT32 lRemainedMurdererTime = m_pcsAgpmCharacter->GetRemainedMurdererTime(pcsCharacter);

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_UPDATE;

	PVOID	pvPacketFactor	= m_pagpmFactors->MakePacketFactors(&pcsCharacter->m_csFactor);
	if (!pvPacketFactor)
		return FALSE;

	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
													&cOperation,								// Operation
													&pcsCharacter->m_lID,						// Character ID
													NULL,										// Character Template ID
													NULL,										// Game ID
													NULL,										// Character Status
													NULL,										// Move Packet
													NULL,										// Action Packet
													pvPacketFactor,								// Factor Packet
													NULL,										// llMoney
													NULL,										// bank money
													NULL,										// cash
													NULL,										// character action status
													NULL,										// character criminal status
													NULL,										// attacker id (정당방위 설정에 필요)
													NULL,										// 새로 생성되서 맵에 들어간넘인지 여부
													NULL,										// region index
													NULL,										// social action index
													NULL,										// special status
													NULL,										// is transform status
													NULL,										// skill initialization text
													NULL,										// face index
													NULL,										// hair index
													NULL,										// Option Flag
													NULL,										// bank size
													NULL,										// event status flag
													NULL,										// remained criminal status time
													NULL,										// remained murderer point time
													pcsCharacter->m_szNickName,					// nick name
													NULL,										// gameguard
													NULL										// last killed time in battlesquare
													);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	BOOL bSendResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketMukzaPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter || pcsAgsdCharacter->m_dpnidCharacter == 0)	return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_UPDATE;

	PVOID	pvPacketFactor	= m_pagpmFactors->MakePacketFactors(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA);
	if (!pvPacketFactor)
		return FALSE;

	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
													&cOperation,								// Operation
													&pcsCharacter->m_lID,						// Character ID
													NULL,										// Character Template ID
													NULL,										// Game ID
													NULL,										// Character Status
													NULL,										// Move Packet
													NULL,										// Action Packet
													pvPacketFactor,								// Factor Packet
													NULL,										// llMoney
													NULL,										// bank money
													NULL,										// cash
													NULL,										// character action status
													NULL,										// character criminal status
													NULL,										// attacker id (정당방위 설정에 필요)
													NULL,										// 새로 생성되서 맵에 들어간넘인지 여부
													NULL,										// region index
													NULL,										// social action index
													NULL,										// special status
													NULL,										// is transform status
													NULL,										// skill initialization text
													NULL,										// face index
													NULL,										// hair index
													NULL,										// Option Flag
													NULL,										// bank size
													NULL,										// event status flag
													NULL,										// remained criminal status time
													NULL,										// remained murderer point time
													NULL,										// nick name
													NULL,										// gameguard
													NULL										// last killed time in battlesquare
													);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	BOOL bSendResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketCharismaPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter || pcsAgsdCharacter->m_dpnidCharacter == 0)	return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_UPDATE;

	PVOID	pvPacketFactor	= m_pagpmFactors->MakePacketFactors(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
	if (!pvPacketFactor)
		return FALSE;

	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
													&cOperation,										// Operation
													&pcsCharacter->m_lID,								// Character ID
													NULL,												// Character Template ID
													NULL,												// Game ID
													NULL,												// Character Status
													NULL,												// Move Packet
													NULL,												// Action Packet
													pvPacketFactor,										// Factor Packet
													NULL,												// llMoney
													NULL,												// bank money
													NULL,												// cash
													NULL,												// character action status
													NULL,												// character criminal status
													NULL,												// attacker id (정당방위 설정에 필요)
													NULL,												// 새로 생성되서 맵에 들어간넘인지 여부
													NULL,												// region index
													NULL,												// social action index
													NULL,												// special status
													NULL,												// is transform status
													NULL,												// skill initialization text
													NULL,												// face index
													NULL,												// hair index
													NULL,												// Option Flag
													NULL,												// bank size
													NULL,												// event status flag
													NULL,												// remained criminal status time
													NULL,												// remained murderer point time
													NULL,												// nick name
													NULL,												// gameguard
													NULL												// last killed time in battlesquare
													);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	BOOL bSendResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendAddAttackerID(AgpdCharacter *pcsCharacter, INT32 lAttackerID, UINT32 ulNID)
{
	if (!pcsCharacter || lAttackerID == AP_INVALID_CID || ulNID == 0)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_ADD_ATTACKER;
	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
													&cOperation,									// Operation
													&pcsCharacter->m_lID,							// Character ID
													NULL,											// Character Template ID
													NULL,											// Game ID
													NULL,											// Character Status
													NULL,											// Move Packet
													NULL,											// Action Packet
													NULL,											// Factor Packet
													NULL,											// llMoney
													NULL,											// bank money
													NULL,											// cash
													NULL,											// character action status
													NULL,											// character criminal status
													&lAttackerID,									// attacker id (정당방위 설정에 필요)
													NULL,											// 새로 생성되서 맵에 들어간넘인지 여부
													NULL,											// region index
													NULL,											// social action index
													NULL,											// special status
													NULL,											// is transform status
													NULL,											// skill initialization text
													NULL,											// face index
													NULL,											// hair index
													NULL,											// Option Flag
													NULL,											// bank size
													NULL,											// event status flag
													NULL,											// remained criminal status time
													NULL,											// remained murderer point time
													NULL,											// nick name
													NULL,											// gameguard
													NULL											// last killed time in battlesquare
													);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketTransformAppear(AgpdCharacter *pcsCharacter, INT32 lTID)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_TRANSFORM;
	PVOID	pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,							// Operation
																&pcsCharacter->m_lID,					// Character ID
																&lTID,									// Character Template ID
																NULL,									// Game ID
																NULL,									// Character Status
																NULL,									// Move Packet
																NULL,									// Action Packet
																NULL,									// Factor Packet
																NULL,									// llMoney
																NULL,									// bank money
																NULL,									// cash
																NULL,									// character action status
																NULL,									// character criminal status
																NULL,									// attacker id (정당방위 설정에 필요)
																NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,									// region index
																NULL,									// social action index
																NULL,									// special status
																NULL,									// is transform status
																NULL,									// skill initialization text
																NULL,									// face index
																NULL,									// hair index
																NULL,									// Option Flag
																NULL,									// bank size
																NULL,									// event status flag
																NULL,									// remained criminal status time
																NULL,									// remained murderer point time
																NULL,									// nick name
																NULL,									// gameguard
																NULL									// last killed time in battlesquare
																);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bResult = TRUE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	bResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_6);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmCharacter::SendPacketEvolutionAppear(AgpdCharacter *pcsCharacter, INT32 lTID)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_EVOLUTION;
	PVOID	pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,							// Operation
																&pcsCharacter->m_lID,					// Character ID
																&lTID,									// Character Template ID
																NULL,									// Game ID
																NULL,									// Character Status
																NULL,									// Move Packet
																NULL,									// Action Packet
																NULL,									// Factor Packet
																NULL,									// llMoney
																NULL,									// bank money
																NULL,									// cash
																NULL,									// character action status
																NULL,									// character criminal status
																NULL,									// attacker id (정당방위 설정에 필요)
																NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,									// region index
																NULL,									// social action index
																NULL,									// special status
																NULL,									// is transform status
																NULL,									// skill initialization text
																NULL,									// face index
																NULL,									// hair index
																NULL,									// Option Flag
																NULL,									// bank size
																NULL,									// event status flag
																NULL,									// remained criminal status time
																NULL,									// remained murderer point time
																NULL,									// nick name
																NULL,									// gameguard
																NULL									// last killed time in battlesquare
																);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bResult = TRUE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	bResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_6);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmCharacter::SendPacketRestoreTransformAppear(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_RESTORE_TRANSFORM;
	PVOID	pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,							// Operation
																&pcsCharacter->m_lID,					// Character ID
																NULL,									// Character Template ID
																NULL,									// Game ID
																NULL,									// Character Status
																NULL,									// Move Packet
																NULL,									// Action Packet
																NULL,									// Factor Packet
																NULL,									// llMoney
																NULL,									// bank money
																NULL,									// cash
																NULL,									// character action status
																NULL,									// character criminal status
																NULL,									// attacker id (정당방위 설정에 필요)
																NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,									// region index
																NULL,									// social action index
																NULL,									// special status
																NULL,									// is transform status
																NULL,									// skill initialization text
																NULL,									// face index
																NULL,									// hair index
																NULL,									// Option Flag
																NULL,									// bank size
																NULL,									// event status flag
																NULL,									// remained criminal status time
																NULL,									// remained murderer point time
																NULL,									// nick name
																NULL,									// gameguard
																NULL									// last killed time in battlesquare
																);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bResult = TRUE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	bResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_6);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmCharacter::SendPacketRestoreEvolutionAppear(AgpdCharacter *pcsCharacter, INT32 lTID)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_RESTORE_EVOLUTION;
	PVOID	pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,							// Operation
																&pcsCharacter->m_lID,					// Character ID
																&lTID,									// Character Template ID
																NULL,									// Game ID
																NULL,									// Character Status
																NULL,									// Move Packet
																NULL,									// Action Packet
																NULL,									// Factor Packet
																NULL,									// llMoney
																NULL,									// bank money
																NULL,									// cash
																NULL,									// character action status
																NULL,									// character criminal status
																NULL,									// attacker id (정당방위 설정에 필요)
																NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,									// region index
																NULL,									// social action index
																NULL,									// special status
																NULL,									// is transform status
																NULL,									// skill initialization text
																NULL,									// face index
																NULL,									// hair index
																NULL,									// Option Flag
																NULL,									// bank size
																NULL,									// event status flag
																NULL,									// remained criminal status time
																NULL,									// remained murderer point time
																NULL,									// nick name
																NULL,									// gameguard
																NULL									// last killed time in battlesquare
																);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bResult = TRUE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	bResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_6);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmCharacter::SendPacketRidableAppear(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_RIDABLE;
	PVOID	pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,									// Operation
																&pcsCharacter->m_lID,							// Character ID
																NULL,											// Character Template ID
																NULL,											// Game ID
																NULL,											// Character Status
																NULL,											// Move Packet
																NULL,											// Action Packet
																NULL,											// Factor Packet
																NULL,											// llMoney
																NULL,											// bank money
																NULL,											// cash
																NULL,											// character action status
																NULL,											// character criminal status
																NULL,											// attacker id (정당방위 설정에 필요)
																NULL,											// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,											// region index
																NULL,											// social action index
																NULL,											// special status
																NULL,											// is transform status
																NULL,											// skill initialization text
																NULL,											// face index
																NULL,											// hair index
																NULL,											// Option Flag
																NULL,											// bank size
																NULL,											// event status flag
																NULL,											// remained criminal status time
																NULL,											// remained murderer point time
																NULL,											// nick name
																NULL,											// gameguard
																NULL											// last killed time in battlesquare
																);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bResult = TRUE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	bResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_6);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmCharacter::SendPacketRestoreRidable(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_RESTORE_RIDABLE;
	PVOID	pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,							// Operation
																&pcsCharacter->m_lID,					// Character ID
																NULL,									// Character Template ID
																NULL,									// Game ID
																NULL,									// Character Status
																NULL,									// Move Packet
																NULL,									// Action Packet
																NULL,									// Factor Packet
																NULL,									// llMoney
																NULL,									// bank money
																NULL,									// cash
																NULL,									// character action status
																NULL,									// character criminal status
																NULL,									// attacker id (정당방위 설정에 필요)
																NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,									// region index
																NULL,									// social action index
																NULL,									// special status
																NULL,									// is transform status
																NULL,									// skill initialization text
																NULL,									// face index
																NULL,									// hair index
																NULL,									// Option Flag
																NULL,									// bank size
																NULL,									// event status flag
																NULL,									// remained criminal status time
																NULL,									// remained murderer point time
																NULL,									// nick name
																NULL,									// gameguard
																NULL									// last killed time in battlesquare
																);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bResult = TRUE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	bResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_6);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmCharacter::SendPacketFactor(PVOID pvPacketFactor, AgpdCharacter *pcsCharacter, PACKET_PRIORITY ePriority)
{
	if (!pvPacketFactor || !pcsCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = GetADCharacter(pcsCharacter);

	return SendPacketFactor(pvPacketFactor, pcsCharacter, pcsAgsdCharacter->m_dpnidCharacter, FALSE, ePriority);
}

BOOL AgsmCharacter::SendPacketFactor(PVOID pvPacketFactor, AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID, PACKET_PRIORITY ePriority)
{
	PROFILE("AgsmCharacter::SendPacketFactor");

	if (!pvPacketFactor || !pcsCharacter)
		return FALSE;

	if (dpnid == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID pvPacket = MakePacketFactor(pcsCharacter, pvPacketFactor, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	BOOL	bSendResult	= FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	if (bGroupNID)
		bSendResult	= m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, dpnid, ePriority);
	else
		bSendResult	= SendPacket(pvPacket, nPacketLength, dpnid, ePriority);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

PVOID AgsmCharacter::MakePacketFactor(AgpdCharacter *pcsCharacter, PVOID pvPacketFactor, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pvPacketFactor || !pnPacketLength)
		return NULL;

	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,							// Operation
																&pcsCharacter->m_lID,					// Character ID
																NULL,									// Character Template ID
																NULL,									// Game ID
																NULL,									// Character Status
																NULL,									// Move Packet
																NULL,									// Action Packet
																pvPacketFactor,							// Factor Packet
																NULL,									// llMoney
																NULL,									// bank money
																NULL,									// cash
																NULL,									// character action status
																NULL,									// character criminal status
																NULL,									// attacker id (정당방위 설정에 필요)
																NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,									// region index
																NULL,									// social action index
																NULL,									// special status
																NULL,									// is transform status
																NULL,									// skill initialization text
																NULL,									// face index
																NULL,									// hair index
																NULL,									// Option Flag
																NULL,									// bank size
																NULL,									// event status flag
																NULL,									// remained criminal status time
																NULL,									// remained murderer point time
																NULL,									// nick name
																NULL,									// gameguard
																NULL									// last killed time in battlesquare
																);

	return pvPacket;
}

BOOL AgsmCharacter::SendPacketCharStatus(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = GetADCharacter(pcsCharacter);

	return SendPacketCharStatus(pcsCharacter, pcsAgsdCharacter->m_dpnidCharacter);
}

BOOL AgsmCharacter::SendPacketCharStatus(AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID)
{
	if (!pcsCharacter)
		return FALSE;

	if (dpnid == 0)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
	PVOID pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,								// Operation
																&pcsCharacter->m_lID,						// Character ID
																NULL,										// Character Template ID
																NULL,										// Game ID
																&pcsCharacter->m_unCurrentStatus,			// Character Status
																NULL,										// Move Packet
																NULL,										// Action Packet
																NULL,										// Factor Packet
																NULL,										// llMoney
																NULL,										// bank money
																NULL,										// cash
																NULL,										// character action status
																NULL,										// character criminal status
																NULL,										// attacker id (정당방위 설정에 필요)
																NULL,										// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,										// region index
																NULL,										// social action index
																NULL,										// special status
																NULL,										// is transform status
																NULL,										// skill initialization text
																NULL,										// face index
																NULL,										// hair index
																NULL,										// Option Flag
																NULL,										// bank size
																NULL,										// event status flag
																NULL,										// remained criminal status time
																NULL,										// remained murderer point time
																NULL,										// nick name
																NULL,										// gameguard
																NULL										// last killed time in battlesquare
																);

	BOOL	bSendResult	= FALSE;

	if (pvPacket)
	{
		m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		if (bGroupNID)
			bSendResult	= m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, dpnid, PACKET_PRIORITY_3);
		else
			bSendResult	= SendPacket(pvPacket, nPacketLength, dpnid, PACKET_PRIORITY_3);

		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
	}

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketCharActionStatus(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = GetADCharacter(pcsCharacter);

	return SendPacketCharActionStatus(pcsCharacter, pcsAgsdCharacter->m_dpnidCharacter, FALSE);
}

BOOL AgsmCharacter::SendPacketCharActionStatus(AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID)
{
	if (!pcsCharacter)
		return FALSE;

	if (dpnid == 0)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
	PVOID pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,								// Operation
																&pcsCharacter->m_lID,						// Character ID
																NULL,										// Character Template ID
																NULL,										// Game ID
																NULL,										// Character Status
																NULL,										// Move Packet
																NULL,										// Action Packet
																NULL,										// Factor Packet
																NULL,										// llMoney
																NULL,										// bank money
																NULL,										// cash
																&pcsCharacter->m_unActionStatus,			// character action status
																NULL,										// character criminal status
																NULL,										// attacker id (정당방위 설정에 필요)
																NULL,										// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,										// region index
																NULL,										// social action index
																NULL,										// special status
																NULL,										// is transform status
																NULL,										// skill initialization text
																NULL,										// face index
																NULL,										// hair index
																NULL,										// Option Flag
																NULL,										// bank size
																NULL,										// event status flag
																NULL,										// remained criminal status time
																NULL,										// remained murderer point time
																NULL,										// nick name
																NULL,										// gameguard
																NULL										// last killed time in battlesquare
																);

	BOOL	bSendResult	= FALSE;

	if (pvPacket)
	{
		m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		if (bGroupNID)
			bSendResult	= m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, dpnid, PACKET_PRIORITY_3);
		else
			bSendResult	= SendPacket(pvPacket, nPacketLength, dpnid, PACKET_PRIORITY_3);

		PVOID	pvBuffer[2];
		pvBuffer[0]		= (PVOID) pvPacket;
		pvBuffer[1]		= (PVOID) nPacketLength;

		EnumCallback(AGSMCHARACTER_CB_SYNC_CHARACTER_ACTIONS, pcsCharacter, pvBuffer);

		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
	}

	return bSendResult;
}

PVOID AgsmCharacter::MakePacketUpdateCriminalStatus(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT32 lRemainedCriminalTime = m_pcsAgpmCharacter->GetRemainedCriminalTime(pcsCharacter);
	
	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
	return m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,									// Operation
																&pcsCharacter->m_lID,							// Character ID
																NULL,											// Character Template ID
																NULL,											// Game ID
																NULL,											// Character Status
																NULL,											// Move Packet
																NULL,											// Action Packet
																NULL,											// Factor Packet
																NULL,											// llMoney
																NULL,											// bank money
																NULL,											// cash
																NULL,											// character action status
																&pcsCharacter->m_unCriminalStatus,				// character criminal status
																NULL,											// attacker id (정당방위 설정에 필요)
																NULL,											// 새로 생성되서 맵에 들어간넘인지 여부
																NULL,											// region index
																NULL,											// social action index
																NULL,											// special status
																NULL,											// is transform status
																NULL,											// skill initialization text
																NULL,											// face index
																NULL,											// hair index
																NULL,											// Option Flag
																NULL,											// bank size
																NULL,											// event status flag
																&lRemainedCriminalTime,							// remained criminal status time
																NULL,											// remained murderer point time
																NULL,											// nick name
																NULL,											// gameguard
																NULL											// last killed time in battlesquare
																);
}

PVOID AgsmCharacter::MakePacketAgpdDBData(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pcsCharacter->m_pcsCharacterTemplate || !pnPacketLength)
		return NULL;

	PVOID	pvPacketFactor	= m_pagsmFactors->MakePacketDBData(&pcsCharacter->m_csFactor);

	INT8	cMoveFlag = AGPMCHARACTER_MOVE_FLAG_SYNC;
	INT8	cMoveDirection = (INT8)pcsCharacter->m_eMoveDirection;

	PVOID	pvPacketMove = m_pcsAgpmCharacter->m_csPacketMove.MakePacket(FALSE, pnPacketLength, 0,
																&pcsCharacter->m_stPos,
																&pcsCharacter->m_stPos,
																NULL,
																NULL,
																NULL,
																NULL,
																&cMoveFlag,
																&cMoveDirection);

	UINT8	ucBindingRegionIndex	= (UINT8)	pcsCharacter->m_nLastExistBindingIndex;

	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_UPDATE;

	INT8	cFaceIndex	= (INT8)	pcsCharacter->m_lFaceIndex;
	INT8	cHairIndex	= (INT8)	pcsCharacter->m_lHairIndex;

	UINT16	unIDLength	= (UINT16)strlen(pcsCharacter->m_szID);
	if (unIDLength > AGPDCHARACTER_MAX_ID_LENGTH)
		unIDLength	= AGPDCHARACTER_MAX_ID_LENGTH;

	PVOID	pvPacket		= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,												// Operation
																&pcsCharacter->m_lID,										// Character ID
																&pcsCharacter->m_pcsCharacterTemplate->m_lID,				// Character Template ID
																pcsCharacter->m_szID, &unIDLength,							// Game ID
																NULL,														// Character Status
																pvPacketMove,												// Move Packet
																NULL,														// Action Packet
																pvPacketFactor,												// Factor Packet
																&pcsCharacter->m_llMoney,									// llMoney
																&pcsCharacter->m_llBankMoney,								// bank money
																&pcsCharacter->m_llCash,									// cash
																NULL,														// character action status
																&((INT8) pcsCharacter->m_unCriminalStatus),					// character criminal status
																NULL,														// attacker id (정당방위 설정에 필요)
																NULL,														// 새로 생성되서 맵에 들어간넘인지 여부
																&ucBindingRegionIndex,										// region index
																NULL,														// social action index
																&pcsCharacter->m_ulSpecialStatus,							// special status
																NULL,														// is transform status
																pcsCharacter->m_szSkillInit,								// skill initialization text
																&cFaceIndex,												// face index
																&cHairIndex,												// hair index
																NULL,														// Option Flag
																&pcsCharacter->m_cBankSize,									// bank size
																&pcsCharacter->m_unEventStatusFlag,							// event status flag
																NULL,														// remained criminal status time
																NULL,														// remained murderer point time
																pcsCharacter->m_szNickName,									// nick name
																NULL,														// gameguard
																NULL														// last killed time in battlesquare
																);

	if (pvPacketFactor)
		m_csPacket.FreePacket(pvPacketFactor);
	if (pvPacketMove)
		m_csPacket.FreePacket(pvPacketMove);

	return pvPacket;
}

BOOL AgsmCharacter::SendPacketCharCriminalStatus(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = GetADCharacter(pcsCharacter);

	return SendPacketCharCriminalStatus(pcsCharacter, pcsAgsdCharacter->m_dpnidCharacter, FALSE);
}

BOOL AgsmCharacter::SendPacketCharCriminalStatus(AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID)
{
	if (!pcsCharacter)
		return FALSE;

	if (dpnid == 0)
		return FALSE;

	INT16	nPacketLength = 0;

	PVOID	pvPacket	= MakePacketUpdateCriminalStatus(pcsCharacter, &nPacketLength);

	BOOL	bSendResult	= FALSE;

	if (pvPacket && nPacketLength > 0)
	{
		m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		if (bGroupNID)
			bSendResult	= m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, dpnid, PACKET_PRIORITY_5);
		else
			bSendResult	= SendPacket(pvPacket, nPacketLength, dpnid, PACKET_PRIORITY_5);

		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
	}

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketNearCharCriminalStatus(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength = 0;

	PVOID	pvPacket	= MakePacketUpdateCriminalStatus(pcsCharacter, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	bSendResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketLevelUp(AgpdCharacter *pcsCharacter, PVOID pvPacketFactor)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_LEVEL_UP;

	PVOID	pvPacket	= m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																	&cOperation,							// Operation
																	&pcsCharacter->m_lID,					// Character ID
																	NULL,									// Character Template ID
																	NULL,									// Game ID
																	NULL,									// Character Status
																	NULL,									// Move Packet
																	NULL,									// Action Packet
																	pvPacketFactor,							// Factor Packet
																	NULL,									// llMoney
																	NULL,									// bank money
																	NULL,									// cash
																	NULL,									// character action status
																	NULL,									// character criminal status
																	NULL,									// attacker id (정당방위 설정에 필요)
																	NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
																	NULL,									// region index
																	NULL,									// social action index
																	NULL,									// special status
																	NULL,									// is transform status
																	NULL,									// skill initialization text
																	NULL,									// face index
																	NULL,									// hair index
																	NULL,									// Option Flag
																	NULL,									// bank size
																	NULL,									// event status flag
																	NULL,									// remained criminal status time
																	NULL,									// remained murderer point time
																	NULL,									// nick name
																	NULL,									// gameguard
																	NULL									// last killed time in battlesquare
																	);

	//m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	BOOL	bSendResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_4);

	PVOID	pvBuffer[2];
	pvBuffer[0]		= (PVOID) pvPacket;
	pvBuffer[1]		= (PVOID) nPacketLength;

	EnumCallback(AGSMCHARACTER_CB_SYNC_CHARACTER_ACTIONS, pcsCharacter, pvBuffer);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketCharPosition(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = GetADCharacter(pcsCharacter);

	return SendPacketCharPosition(pcsCharacter, pcsAgsdCharacter->m_dpnidCharacter);
}

BOOL AgsmCharacter::SendPacketCharPosition(AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID)
{
	if (!pcsCharacter)
		return FALSE;

	PVOID	pvPacket		= NULL;
	INT16	nPacketLength	= 0;

	if (m_pagpmOptimizedPacket2)
	{
		INT16	nPacketSelfLength = 0;

		PVOID	pvPacketSelf	= m_pagpmOptimizedPacket2->MakePacketCharMoveContainAction(pcsCharacter, &nPacketSelfLength);
		pvPacket		= m_pagpmOptimizedPacket2->MakePacketCharMove(pcsCharacter, &nPacketLength);

		m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);
		m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacketSelf, nPacketSelfLength, pcsCharacter->m_lID);

		SendPacket(pvPacketSelf, nPacketSelfLength, GetCharDPNID(pcsCharacter), PACKET_PRIORITY_5);

		if (!m_pagsmAOIFilter->SendPacketNearExceptSelf(pvPacket, nPacketLength, pcsCharacter->m_stPos, m_pcsAgpmCharacter->GetRealRegionIndex(pcsCharacter), GetCharDPNID(pcsCharacter), PACKET_PRIORITY_5))
		{
			OutputDebugString("AgsmCharacter::SendPacketCharPosition() Error (2) !!!\n");
			m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
			m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacketSelf);

			return FALSE;
		}

		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacketSelf);
	}
	else
	{
		INT8 cMoveFlag = (pcsCharacter->m_eMoveDirection != MD_NODIRECTION ? AGPMCHARACTER_MOVE_FLAG_DIRECTION : 0) |
						 (pcsCharacter->m_bPathFinding ? AGPMCHARACTER_MOVE_FLAG_PATHFINDING : 0) |
			 			 (pcsCharacter->m_bMove ? 0 : AGPMCHARACTER_MOVE_FLAG_STOP) |
						 (pcsCharacter->m_bSync ? AGPMCHARACTER_MOVE_FLAG_SYNC : 0) |
						 (pcsCharacter->m_bMoveFast ? AGPMCHARACTER_MOVE_FLAG_FAST : 0) |
						 (pcsCharacter->m_bMoveFollow ? AGPMCHARACTER_MOVE_FLAG_FOLLOW : 0) |
			 			 (pcsCharacter->m_bHorizontal ? AGPMCHARACTER_MOVE_FLAG_HORIZONTAL : 0);

		INT8 cMoveDirection = (INT8)pcsCharacter->m_eMoveDirection;

		PVOID pvPacketMove = m_pcsAgpmCharacter->m_csPacketMove.MakePacket(FALSE, &nPacketLength, 0,
																	&pcsCharacter->m_stPos,
																	pcsCharacter->m_bMove ? (pcsCharacter->m_eMoveDirection != MD_NODIRECTION ? &pcsCharacter->m_stDirection : &pcsCharacter->m_stDestinationPos) : &pcsCharacter->m_stPos,
																	(pcsCharacter->m_lFollowTargetID != AP_INVALID_CID) ? &pcsCharacter->m_lFollowTargetID : NULL,
																	(pcsCharacter->m_lFollowTargetID != AP_INVALID_CID) ? &pcsCharacter->m_lFollowDistance : NULL,
																	NULL,
																	NULL,
																	&cMoveFlag,
																	&cMoveDirection);

		if (!pvPacketMove)
			return FALSE;

		INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
		pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																	&cOperation,						// Operation
																	&pcsCharacter->m_lID,				// Character ID
																	NULL,								// Character Template ID
																	NULL,								// Game ID
																	NULL,								// Character Status
																	pvPacketMove,						// Move Packet
																	NULL,								// Action Packet
																	NULL,								// Factor Packet
																	NULL,								// llMoney
																	NULL,								// bank money
																	NULL,								// cash
																	NULL,								// character action status
																	NULL,								// character criminal status
																	NULL,								// attacker id (정당방위 설정에 필요)
																	NULL,								// 새로 생성되서 맵에 들어간넘인지 여부
																	NULL,								// region index
																	NULL,								// social action index
																	NULL,								// special status
																	NULL,								// is transform status
																	NULL,								// skill initialization text
																	NULL,								// face index
																	NULL,								// hair index
																	NULL,								// Option Flag
																	NULL,								// bank size
																	NULL,								// event status flag
																	NULL,								// remained criminal status time
																	NULL,								// remained murderer point time
																	NULL,								// nick name
																	NULL,								// gameguard
																	NULL								// last killed time in battlesquare
																	);

		m_pcsAgpmCharacter->m_csPacketMove.FreePacket(pvPacketMove);

		m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		if (!m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5))
		{
			OutputDebugString("AgsmCharacter::SendPacketCharPosition() Error (2) !!!\n");
			m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

			return FALSE;
		}

	}

	if (pvPacket)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]		= (PVOID) pvPacket;
		pvBuffer[1]		= (PVOID) nPacketLength;

		EnumCallback(AGSMCHARACTER_CB_SYNC_CHARACTER_ACTIONS, pcsCharacter, pvBuffer);

		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

		return TRUE;
	}

	return FALSE;
}

BOOL AgsmCharacter::SendPacketChar(AgpdCharacter *pCharacter, UINT_PTR ulNID, BOOL bGroupNID)
{
	if (!pCharacter || ulNID == 0)
		return FALSE;

	PVOID	pvPacketFactor = m_pagpmFactors->MakePacketFactors(&pCharacter->m_csFactor);

	INT16	nPacketLength = 0;
	PVOID	pvPacket = MakePacketAddCharacter(pCharacter, &nPacketLength, pvPacketFactor);

	if (pvPacketFactor)
		m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	if (!pvPacket)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pCharacter->m_lID);

	BOOL	bSendResult	= FALSE;
	if (bGroupNID)
		bSendResult = m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_1);
	else
		bSendResult = SendPacket(pvPacket, nPacketLength, (UINT32)ulNID, PACKET_PRIORITY_1);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendCharacterAllInfo(AgpdCharacter *pCharacter, UINT32 ulNID, BOOL bSyncSend, BOOL bSendServerInfo, BOOL bLogin, INT32 lOldCID)
{
	if (!pCharacter || ulNID == 0)
		return FALSE;

	if (!SendPacketChar(pCharacter, ulNID, FALSE))
		return FALSE;

	// Admin Info 를 보낸다. 보낸 결과에는 신경쓰지 않는다. - 2004.03.05. steeple
	SendPacketCharAdminInfo(pCharacter, ulNID);

	if (m_pAgsmServerManager2->GetThisServerType() == AGSMSERVER_TYPE_LOGIN_SERVER)
	{
		EnumCallback(AGSMCHARACTER_CB_SEND_AUTH_KEY, pCharacter, &ulNID);
	}

	if (bSyncSend)
	{
		return SendCheckRecvChar(pCharacter->m_lID, ulNID, lOldCID);
	}
	else
	{
		AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pCharacter);

		if (!pcsAgsdCharacter->m_bIsNewCID)
			EnumCallback(AGSMCHARACTER_CB_SEND_CHARACTER_ALL_INFO, pCharacter, &ulNID);
		else
			EnumCallback(AGSMCHARACTER_CB_SEND_CHARACTER_NEW_ID, pCharacter, &ulNID);

		if (bSendServerInfo)
			SendCharacterAllServerInfo(pCharacter, ulNID, bLogin);
	}

	return TRUE;
}

BOOL AgsmCharacter::SendCharacterAllServerInfo(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bLogin)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	PVOID	pvPacket		= NULL;
	INT16	nPacketLength	= 0;

	if (bLogin)
		pvPacket = MakePacketAgsdCharacterLogin(pcsCharacter, &nPacketLength);
	else
		pvPacket = MakePacketAgsdCharacter(pcsCharacter, &nPacketLength);

	if (!pvPacket || nPacketLength <= 0)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	// Ban Data 를 보낸다. - 2004.03.05. steeple
	SendPacketBanData(pcsCharacter, ulNID);

	UINT32	aulBuffer[2];
	aulBuffer[0] = ulNID;
	aulBuffer[1] = (UINT32) bLogin;

	bSendResult &= EnumCallback(AGSMCHARACTER_CB_SEND_CHARACTER_ALL_SERVER_INFO, pcsCharacter, aulBuffer);

	return bSendResult;
}

BOOL AgsmCharacter::SendCheckRecvChar(INT32 lCID, UINT32 ulDPNID, INT32 lOldCID)
{
	if (lCID == AP_INVALID_CID || ulDPNID == 0)
		return FALSE;

	AgsdServer	*pcsThisServer	= m_pAgsmServerManager2->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGSMCHAR_PACKET_OPERATION_CHECK_RECV_CHAR;

	PVOID pvPacket = MakePacket(&nPacketLength, &cOperation, &lOldCID, NULL, NULL, &pcsThisServer->m_lServerID, NULL, NULL, &lCID);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bRetval = SendPacket(pvPacket, nPacketLength, ulDPNID);

	m_csPacket.FreePacket(pvPacket);

	return bRetval;
}

BOOL AgsmCharacter::SendCheckRecvCharResult(INT32 lCID, UINT32 ulDPNID, BOOL bResult)
{
	if (lCID == AP_INVALID_CID || ulDPNID == 0)
		return FALSE;

	AgsdServer	*pcsThisServer	= m_pAgsmServerManager2->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT8	cOperation;

	if (bResult)
		cOperation	= AGSMCHAR_PACKET_OPERATION_CHECK_RECV_CHAR_SUCCESS;
	else
		cOperation	= AGSMCHAR_PACKET_OPERATION_CHECK_RECV_CHAR_FAIL;

	INT16	nPacketLength	= 0;
	PVOID pvPacket = MakePacket(&nPacketLength, &cOperation, &lCID, NULL, NULL, &pcsThisServer->m_lServerID, NULL, NULL);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulDPNID);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendCompleteSendCharInfo(INT32 lCID, UINT32 ulDPNID)
{
	if (lCID == AP_INVALID_CID || ulDPNID == 0)
		return FALSE;

	AgsdServer	*pcsThisServer	= m_pAgsmServerManager2->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT8	cOperation		= AGSMCHAR_PACKET_OPERATION_COMPLETE_SEND_CHAR_INFO;
	INT16	nPacketLength	= 0;
	PVOID pvPacket = MakePacket(&nPacketLength, &cOperation, &lCID, NULL, NULL, &pcsThisServer->m_lServerID, NULL, NULL);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulDPNID);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketUpdateFactorView(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return FALSE;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
	INT16 nPacketLength;
	
	PVOID pvPacketFactor = m_pagpmFactors->MakePacketFactorsCharView(&pCharacter->m_csFactor);
	if (!pvPacketFactor)
		return FALSE;

	PVOID pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,							// Operation
															&pCharacter->m_lID,						// Character ID
															NULL,									// Character Template ID
															NULL,									// Game ID
															NULL,									// Character Status
															NULL,									// Move Packet
															NULL,									// Action Packet
															pvPacketFactor,							// Factor Packet
															NULL,									// llMoney
															NULL,									// bank money
															NULL,									// cash
															NULL,									// character action status
															NULL,									// character criminal status
															NULL,									// attacker id (정당방위 설정에 필요)
															NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
															NULL,									// region index
															NULL,									// social action index
															NULL,									// special status
															NULL,									// is transform status
															NULL,									// skill initialization text
															NULL,									// face index
															NULL,									// hair index
															NULL,									// Option Flag
															NULL,									// bank size
															NULL,									// event status flag
															NULL,									// remained criminal status time
															NULL,									// remained murderer point time
															NULL,									// nick name
															NULL,									// gameguard
															NULL									// last killed time in battlesquare
															);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	if (!pvPacket)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pCharacter->m_lID);

	BOOL	bSendResult	= m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pCharacter->m_stPos, PACKET_PRIORITY_4);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmCharacter::SendPacketUpdateRegionIndex(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return FALSE;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
	INT16 nPacketLength;

	UINT8	ucBindingRegionIndex	= (UINT8)	pCharacter->m_nLastExistBindingIndex;
	
	PVOID pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,							// Operation
															&pCharacter->m_lID,						// Character ID
															NULL,									// Character Template ID
															NULL,									// Game ID
															NULL,									// Character Status
															NULL,									// Move Packet
															NULL,									// Action Packet
															NULL,									// Factor Packet
															NULL,									// llMoney
															NULL,									// bank money
															NULL,									// cash
															NULL,									// character action status
															NULL,									// character criminal status
															NULL,									// attacker id (정당방위 설정에 필요)
															NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
															&ucBindingRegionIndex,					// region index
															NULL,									// social action index
															NULL,									// special status
															NULL,									// is transform status
															NULL,									// skill initialization text
															NULL,									// face index
															NULL,									// hair index
															NULL,									// Option Flag
															NULL,									// bank size
															NULL,									// event status flag
															NULL,									// remained criminal status time
															NULL,									// remained murderer point time
															NULL,									// nick name
															NULL,									// gameguard
															NULL									// last killed time in battlesquare
															);

	if (!pvPacket)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, GetCharDPNID(pCharacter));

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

// 2004.03.05. steeple
// AgpdAdmin 데이터를 Character 정보를 보낼때 같이 보내기 위해서 만듬.
BOOL AgsmCharacter::SendPacketCharAdminInfo(AgpdCharacter* pCharacter, UINT32 ulNID)
{
	if(!pCharacter || !ulNID)
		return FALSE;

	AgpdAdmin* pcsAttachedAdmin = m_pagpmAdmin->GetADCharacter(pCharacter);
	if(!pcsAttachedAdmin)
		return FALSE;

	INT8 nLoginFlag = 1;
	INT16 nPacketLength = 0;
	PVOID pvLoginPacket = m_pagpmAdmin->MakeLoginPacket(&nPacketLength, 
															&nLoginFlag,
															&pCharacter->m_lID,
															pCharacter->m_szID,
															&pcsAttachedAdmin->m_lAdminLevel,
															NULL,	// Server 정보는 현재 보내지 않는다.
															NULL);
	if(!pvLoginPacket)
		return FALSE;

	PVOID pvPacket = m_pagpmAdmin->MakeLoginPacket(&nPacketLength, &pCharacter->m_lID, pvLoginPacket);
	m_pagpmAdmin->m_csLoginPacket.FreePacket(pvLoginPacket);

	if(!pvPacket)
		return FALSE;

	BOOL bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

// 2004.03.30. steeple
// Character Ban 정보를 보낸다.
BOOL AgsmCharacter::SendPacketBanData(AgpdCharacter* pCharacter, UINT32 ulNID)
{
	if(!pCharacter || !ulNID)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakePacketBanData(pCharacter, &nPacketLength);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendDisconnectByAnotherUser( UINT32 ulNID )
{
	if( !ulNID )
		return FALSE;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_DISCONNECT_BY_ANOTHER_USER;
	INT16 nPacketLength;
	
	PVOID pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,						// Operation
															NULL,								// Character ID
															NULL,								// Character Template ID
															NULL,								// Game ID
															NULL,								// Character Status
															NULL,								// Move Packet
															NULL,								// Action Packet
															NULL,								// Factor Packet
															NULL,								// llMoney
															NULL,								// bank money
															NULL,								// cash
															NULL,								// character action status
															NULL,								// character criminal status
															NULL,								// attacker id (정당방위 설정에 필요)
															NULL,								// 새로 생성되서 맵에 들어간넘인지 여부
															NULL,								// region index
															NULL,								// social action index
															NULL,								// special status
															NULL,								// is transform status
															NULL,								// skill initialization text
															NULL,								// face index
															NULL,								// hair index
															NULL,								// Option Flag
															NULL,								// bank size
															NULL,								// event status flag
															NULL,								// remained criminal status time
															NULL,								// remained murderer point time
															NULL,								// nick name
															NULL,								// gameguard
															NULL								// last killed time in battlesquare
															);

	if (!pvPacket)
		return FALSE;

	BOOL	bSendResult	= FALSE;

	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketSocialAnimation(AgpdCharacter *pcsCharacter, UINT8 ucSocialAnimation)
{
	if (!pcsCharacter)
		return FALSE;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_SOCIAL_ANIMATION;
	INT16 nPacketLength;
	
	PVOID pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,						// Operation
															&pcsCharacter->m_lID,				// Character ID
															NULL,								// Character Template ID
															NULL,								// Game ID
															NULL,								// Character Status
															NULL,								// Move Packet
															NULL,								// Action Packet
															NULL,								// Factor Packet
															NULL,								// llMoney
															NULL,								// bank money
															NULL,								// cash
															NULL,								// character action status
															NULL,								// character criminal status
															NULL,								// attacker id (정당방위 설정에 필요)
															NULL,								// 새로 생성되서 맵에 들어간넘인지 여부
															NULL,								// region index
															&ucSocialAnimation,					// social action index
															NULL,								// special status
															NULL,								// is transform status
															NULL,								// skill initialization text
															NULL,								// face index
															NULL,								// hair index
															NULL,								// Option Flag
															NULL,								// bank size
															NULL,								// event status flag
															NULL,								// remained criminal status time
															NULL,								// remained murderer point time
															NULL,								// nick name
															NULL,								// gameguard
															NULL								// last killed time in battlesquare
															);

	if (!pvPacket)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	BOOL	bSendResult	= m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_6);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketRequestNewCID(INT32 lOldCID, UINT32 ulNID)
{
	if (lOldCID == AP_INVALID_CID ||
		ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketRequestNewCID(lOldCID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

PVOID AgsmCharacter::MakePacketRequestNewCID(INT32 lOldCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation		= AGSMCHAR_PACKET_OPERATION_REQUEST_NEW_CID;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMCHARACTER_PACKET_TYPE,
													&cOperation,
													&lOldCID,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);
}

PVOID AgsmCharacter::MakePacketResponseNewCID(INT32 lOldCID, INT32 lNewCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cOperation		= AGSMCHAR_PACKET_OPERATION_RESPONSE_NEW_CID;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMCHARACTER_PACKET_TYPE,
													&cOperation,
													&lOldCID,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													&lNewCID);
}

PVOID AgsmCharacter::MakePacketUpdateCustomize(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMCHAR_PACKET_OPERATION_UPDATE;

	INT8	cFaceIndex	= (INT8)	pcsCharacter->m_lFaceIndex;
	INT8	cHairIndex	= (INT8)	pcsCharacter->m_lHairIndex;
	
	return m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,							// Operation
															&pcsCharacter->m_lID,					// Character ID
															NULL,									// Character Template ID
															NULL,									// Game ID
															NULL,									// Character Status
															NULL,									// Move Packet
															NULL,									// Action Packet
															NULL,									// Factor Packet
															NULL,									// llMoney
															NULL,									// bank money
															NULL,									// cash
															NULL,									// character action status
															NULL,									// character criminal status
															NULL,									// attacker id (정당방위 설정에 필요)
															NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
															NULL,									// region index
															NULL,									// social action index
															NULL,									// special status
															NULL,									// is transform status
															NULL,									// skill initialization text
															&cFaceIndex,							// face index
															&cHairIndex,							// hair index
															NULL,									// Option Flag
															NULL,									// bank size
															NULL,									// event status flag
															NULL,									// remained criminal status time
															NULL,									// remained murderer point time
															NULL,									// nick name
															NULL,									// gameguard
															NULL									// last killed time in battlesquare
															);
}

PVOID AgsmCharacter::MakePacketUpdateBankSize(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPMCHAR_PACKET_OPERATION_UPDATE;

	return m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,								// Operation
															&pcsCharacter->m_lID,						// Character ID
															NULL,										// Character Template ID
															NULL,										// Game ID
															NULL,										// Character Status
															NULL,										// Move Packet
															NULL,										// Action Packet
															NULL,										// Factor Packet
															NULL,										// llMoney
															NULL,										// bank money
															NULL,										// cash
															NULL,										// character action status
															NULL,										// character criminal status
															NULL,										// attacker id (정당방위 설정에 필요)
															NULL,										// 새로 생성되서 맵에 들어간넘인지 여부
															NULL,										// region index
															NULL,										// social action index
															NULL,										// special status
															NULL,										// is transform status
															NULL,										// skill initialization text
															NULL,										// face index
															NULL,										// hair index
															NULL,										// Option Flag
															&pcsCharacter->m_cBankSize,					// bank size
															NULL,										// event status flag
															NULL,										// remained criminal status time
															NULL,										// remained murderer point time
															NULL,										// nick name
															NULL,										// gameguard
															NULL										// last killed time in battlesquare
															);
}

BOOL AgsmCharacter::SendPacketCharUseEffect(AgpdCharacter *pcsCharacter, AgpdCharacterAdditionalEffect eEffect, INT32 lExtraType)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation	= AGPMCHAR_PACKET_OPERATION_UPDATE;
	PVOID	pvPacketAction;
	PVOID	pvPacket;

	// SkillTID 에 ExtraType 세팅했다. 2007.09.14. steeple
	pvPacketAction = m_pcsAgpmCharacter->MakePacketCharAction(&nPacketLength, AGPDCHAR_ACTION_TYPE_USE_EFFECT, -1, lExtraType, AGPDCHAR_ACTION_RESULT_TYPE_NONE, NULL, NULL, -1, FALSE, eEffect, -1);
	if (!pvPacketAction)
		return FALSE;

	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,									// Operation
															&pcsCharacter->m_lID,							// Character ID
															NULL,											// Character Template ID
															NULL,											// Game ID
															NULL,											// Character Status
															NULL,											// Move Packet
															pvPacketAction,									// Action Packet
															NULL,											// Factor Packet
															NULL,											// llMoney
															NULL,											// bank money
															NULL,											// cash
															NULL,											// character action status
															NULL,											// character criminal status
															NULL,											// attacker id (정당방위 설정에 필요)
															NULL,											// 새로 생성되서 맵에 들어간넘인지 여부
															NULL,											// region index
															NULL,											// social action index
															NULL,											// special status
															NULL,											// is transform status
															NULL,											// skill initialization text
															NULL,											// face index
															NULL,											// hair index
															&lExtraType,									// Option Flag
															&pcsCharacter->m_cBankSize,						// bank size
															NULL,											// event status flag
															NULL,											// remained criminal status time
															NULL,											// remained murderer point time
															NULL,											// nick name
															NULL,											// gameguard
															NULL											// last killed time in battlesquare
															);

	m_pcsAgpmCharacter->m_csPacketAction.FreePacket(pvPacketAction);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	BOOL bSendResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketSkillInitString(AgpdCharacter* pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation	= AGPMCHAR_PACKET_OPERATION_UPDATE_SKILLINIT_STRING;
	PVOID	pvPacket;

	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE, 
															&cOperation,									// Operation
															&pcsCharacter->m_lID,							// Character ID
															NULL,											// Character Template ID
															NULL,											// Game ID
															NULL,											// Character Status
															NULL,											// Move Packet
															NULL,											// Action Packet
															NULL,											// Factor Packet
															NULL,											// llMoney
															NULL,											// bank money
															NULL,											// cash
															NULL,											// character action status
															NULL,											// character criminal status
															NULL,											// attacker id (정당방위 설정에 필요)
															NULL,											// 새로 생성되서 맵에 들어간넘인지 여부
															NULL,											// region index
															NULL,											// social action index
															NULL,											// special status
															NULL,											// is transform status
															pcsCharacter->m_szSkillInit,											// skill initialization text
															NULL,											// face index
															NULL,											// hair index
															NULL,											// Option Flag
															NULL,											// bank size
															NULL,											// event status flag
															NULL,											// remained criminal status time
															NULL,											// remained murderer point time
															NULL,											// nick name
															NULL,											// gameguard
															NULL											// last killed time in battlesquare
															);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);
	BOOL bSendResult = SendPacket(pvPacket, nPacketLength, GetCharDPNID(pcsCharacter));
	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketResurrectionByOther(AgpdCharacter* pcsCharacter, CHAR* szName, INT32 lFlag)
{
	if(!pcsCharacter || !szName)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmCharacter->MakePacketResurrectionByOther(&nPacketLength, pcsCharacter->m_lID, szName, lFlag);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);
	BOOL bSendResult = SendPacket(pvPacket, nPacketLength, GetCharDPNID(pcsCharacter));
	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketEventEffectID(INT32 lCID, INT32 lEventID, UINT32 ulNID)
{
	if(!lEventID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmCharacter->MakePacketEventEffectID(&nPacketLength, lCID, lEventID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pcsAgpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, lCID);
	BOOL bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCharacter::SendPacketUpdateBankSize(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketUpdateBankSize(pcsCharacter, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmCharacter::SendPakcetBlockByPenalty(AgpdCharacter *pcsCharacter, INT32 lPenalty, UINT32 ulNID)
{
	if (!pcsCharacter)
		return FALSE;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_BLOCK_BY_PENALTY;
	INT16 nPacketLength = 0;
	PVOID	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE, 
											&cOperation,							// Operation
											&pcsCharacter->m_lID,					// Character ID
											NULL,									// Character Template ID
											NULL,									// Game ID
											NULL,									// Character Status
											NULL,									// Move Packet
											NULL,									// Action Packet
											NULL,									// Factor Packet
											NULL,									// llMoney
											NULL,									// bank money
											NULL,									// cash
											NULL,									// character action status
											NULL,									// character criminal status
											NULL,									// attacker id (정당방위 설정에 필요)
											NULL,									// 새로 생성되서 맵에 들어간넘인지 여부
											NULL,									// region index
											NULL,									// social action index
											NULL,									// special status
											NULL,									// is transform status
											NULL,									// skill initialization text
											NULL,									// face index
											NULL,									// hair index
											&lPenalty,								// Option Flag
											NULL,									// bank size
											NULL,									// event status flag
											NULL,									// remained criminal status time
											NULL,									// remained murderer point time
											NULL,									// nick name
											NULL,									// gameguard
											NULL									// last killed time in battlesquare
											);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
	
	return bSendResult;
}
