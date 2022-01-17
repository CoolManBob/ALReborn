/*===============================================================

	AgsmRelay2Archlord.cpp

===============================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"


//
//	==========		Archlord		==========
//
void AgsmRelay2::InitPacketArchlord()
	{
	m_csPacketArchlord.SetFlagLength(sizeof(INT16));
	m_csPacketArchlord.SetFieldType(AUTYPE_INT16,		1,							// eAgsmRelay2Operation
									AUTYPE_INT32,		1,							// CID
									AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,	// char id
									AUTYPE_CHAR,		_MAX_GUILDID_LENGTH + 1,	// guild id
									AUTYPE_UINT32,		1,							// siege id
									AUTYPE_INT32,		1,							// indicator
									AUTYPE_PACKET,		1,							// packet
									AUTYPE_END,			0
									);
	}


BOOL AgsmRelay2::OnParamArchlord(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Archlord *pAgsdRelay2 = new AgsdRelay2Archlord;

	CHAR *pszCharID = NULL;
	CHAR *pszGuildID = NULL;

	m_csPacketArchlord.GetField(FALSE, pvPacket, 0,
								&pAgsdRelay2->m_eOperation,
								&pAgsdRelay2->m_lCID,
								&pszCharID,
								&pszGuildID,
								&pAgsdRelay2->m_ulSiegeID,
								&pAgsdRelay2->m_lInd,
								&pAgsdRelay2->m_pvPacketEmb
								);

	pAgsdRelay2->m_ulNID = ulNID;
	
	_tcsncpy(pAgsdRelay2->m_szCharID, pszCharID ? pszCharID : _T(""), _MAX_CHARNAME_LENGTH);
	_tcsncpy(pAgsdRelay2->m_szGuildID, pszGuildID ? pszGuildID : _T(""), _MAX_GUILDID_LENGTH);

	return EnumCallback(AGSMRELAY_PARAM_ARCHLORD, (PVOID)pAgsdRelay2, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBOperationArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationArchlord,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBFinishOperationArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT :
			pThis->OnSelectResultArchlord(pRowset, pAgsdRelay2);
			break;
		
		default :
			break;
		}

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBOperationResultArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2Archlord *pAgsdRelay2 = (AgsdRelay2Archlord *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);
	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgpmCharacter->GetCharacter(pAgsdRelay2->m_lCID);
	if (NULL == pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT :
			{
			stBuddyRowset Rowset;
			if (NULL == pAgsdRelay2->m_pvPacketEmb && AGSMRELAY2_ROWSET_IND_EOF == pAgsdRelay2->m_lInd)
				{
				if (FALSE == pThis->m_pAgsmArchlord->OnSelectResultArchlord(pAgpdCharacter, NULL, TRUE))
					return FALSE;
				}
			
			if (FALSE == pThis->ParseBuddyRowsetPacket(pAgsdRelay2->m_pvPacketEmb, &Rowset))
				return FALSE;

			if (FALSE == pThis->m_pAgsmArchlord->OnSelectResultArchlord(pAgpdCharacter, &Rowset))
				return FALSE;
			}
			break;
		
		default :
			break;
		}
			
	return TRUE;
	}


BOOL AgsmRelay2::OnSelectResultArchlord(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2Archlord *pAgsdRelay2Archlord = static_cast<AgsdRelay2Archlord *>(pAgsdRelay2);

	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2Archlord->m_ulNID);
	if (!pGameServer)
		return FALSE;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	INT32 lTotalSize = pRowset->GetRowBufferSize() * pRowset->GetRowCount();
	INT32 lTotalStep = 1 + (INT32) (lTotalSize / 20000);
	INT32 lRowsPerStep = 20000 / pRowset->GetRowBufferSize();
	for (INT32 lStep = 0; lStep < lTotalStep; lStep++)
		{
		PVOID pvPacket = NULL;
		pAgsdRelay2Archlord->m_pvPacketEmb = MakeRowsetPacket2(pRowset, lStep, lRowsPerStep);
		pAgsdRelay2Archlord->m_lInd = lStep + 1;

		BOOL bResult = SendArchlord(pAgsdRelay2Archlord, pGameServer->m_dpnidServer);

		printf("\nArchlord List sended[%d/%d][%s]\n", lStep+1, lTotalStep,  bResult ? _T("TRUE") : _T("FALSE"));
		}

	// last
	pAgsdRelay2Archlord->m_pvPacketEmb = NULL;
	pAgsdRelay2Archlord->m_lInd = AGSMRELAY2_ROWSET_IND_EOF;

	return SendArchlord(pAgsdRelay2Archlord, pGameServer->m_dpnidServer);
	}


BOOL AgsmRelay2::CBArchlordSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2		*pThis = (AgsmRelay2 *)	pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;

	AgsdRelay2Archlord csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_SELECT;
	csRelay2.m_lCID = pAgpdCharacter->m_lID;

	return pThis->SendArchlord(&csRelay2);
	}


BOOL AgsmRelay2::CBArchlordUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmRelay2		*pThis = (AgsmRelay2 *)	pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;
	UINT32			ulSiegeID = *((UINT32 *) pCustData);
	
	AgpdGuild *pAgpdGuild = pThis->m_pAgpmGuild->GetGuild(pAgpdCharacter);
	if (!pAgpdGuild)
		return FALSE;
	
	AgsdRelay2Archlord csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;
	csRelay2.m_lCID = pAgpdCharacter->m_lID;

	ZeroMemory(csRelay2.m_szCharID, _MAX_CHARNAME_LENGTH+1);
	ZeroMemory(csRelay2.m_szGuildID, _MAX_GUILDID_LENGTH+1);
	_tcsncpy(csRelay2.m_szCharID, pAgpdCharacter->m_szID, _MAX_CHARNAME_LENGTH);
	_tcsncpy(csRelay2.m_szGuildID, pAgpdGuild->m_szID, _MAX_GUILDID_LENGTH);
	csRelay2.m_ulSiegeID = ulSiegeID;
	
	return pThis->SendArchlord(&csRelay2);
	}


BOOL AgsmRelay2::CBArchlordInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmRelay2		*pThis = (AgsmRelay2 *)	pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;
	UINT32			ulSiegeID = *((UINT32 *) pCustData);

	AgpdGuild *pAgpdGuild = pThis->m_pAgpmGuild->GetGuild(pAgpdCharacter);
	if (!pAgpdGuild)
		return FALSE;
	
	AgsdRelay2Archlord csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	csRelay2.m_lCID = pAgpdCharacter->m_lID;

	ZeroMemory(csRelay2.m_szCharID, _MAX_CHARNAME_LENGTH+1);
	ZeroMemory(csRelay2.m_szGuildID, _MAX_GUILDID_LENGTH+1);
	_tcsncpy(csRelay2.m_szCharID, pAgpdCharacter->m_szID, _MAX_CHARNAME_LENGTH);
	_tcsncpy(csRelay2.m_szGuildID, pAgpdGuild->m_szID, _MAX_GUILDID_LENGTH);
	csRelay2.m_ulSiegeID = ulSiegeID;
	
	return pThis->SendArchlord(&csRelay2);
	}


BOOL AgsmRelay2::SendArchlord(AgsdRelay2Archlord *pAgsdRelay2, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketEmb= m_csPacketArchlord.MakePacket(FALSE, &nPacketLength, 0,
													 &pAgsdRelay2->m_eOperation,
													 &pAgsdRelay2->m_lCID,
													 pAgsdRelay2->m_szCharID,		// char id
													 pAgsdRelay2->m_szGuildID,		// guild id
													 &pAgsdRelay2->m_ulSiegeID,		// siege id
													 &pAgsdRelay2->m_lInd,
													 pAgsdRelay2->m_pvPacketEmb
													 );
	if (!pvPacketEmb)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_ARCHLORD);
	else
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_ARCHLORD, ulNID);

	m_csPacketArchlord.FreePacket(pvPacketEmb);
	
	return bResult;	
	}




//
//	==========		Lord Guard		==========
//
void AgsmRelay2::InitPacketLordGuard()
	{
	m_csPacketLordGuard.SetFlagLength(sizeof(INT16));
	m_csPacketLordGuard.SetFieldType(AUTYPE_INT16,		1,							// eAgsmRelay2Operation
									 AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,	// char id
									 AUTYPE_INT32,		1,							// indicator
									 AUTYPE_PACKET,		1,							// packet
									 AUTYPE_END,		0
									 );
	}


BOOL AgsmRelay2::OnParamLordGuard(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2LordGuard *pAgsdRelay2 = new AgsdRelay2LordGuard;

	CHAR *pszCharID = NULL;

	m_csPacketLordGuard.GetField(FALSE, pvPacket, 0,
							 	 &pAgsdRelay2->m_eOperation,
								 &pszCharID,
								 &pAgsdRelay2->m_lInd,
								 &pAgsdRelay2->m_pvPacketEmb
								 );

	pAgsdRelay2->m_ulNID = ulNID;
	_tcsncpy(pAgsdRelay2->m_szCharID, pszCharID ? pszCharID : _T(""), _MAX_CHARNAME_LENGTH);

	return EnumCallback(AGSMRELAY_PARAM_LORDGUARD, (PVOID)pAgsdRelay2, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBOperationLordGuard(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationLordGuard,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBFinishOperationLordGuard(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT :
			pThis->OnSelectResultLordGuard(pRowset, pAgsdRelay2);
			break;
		
		default :
			break;
		}

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBOperationResultLordGuard(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2Archlord *pAgsdRelay2 = (AgsdRelay2Archlord *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT :
			{
			stBuddyRowset Rowset;
			if (NULL == pAgsdRelay2->m_pvPacketEmb && AGSMRELAY2_ROWSET_IND_EOF == pAgsdRelay2->m_lInd)
				{
				if (FALSE == pThis->m_pAgsmArchlord->OnSelectResultLordGuard(NULL, TRUE))
					return FALSE;
				}
			
			if (FALSE == pThis->ParseBuddyRowsetPacket(pAgsdRelay2->m_pvPacketEmb, &Rowset))
				return FALSE;

			if (FALSE == pThis->m_pAgsmArchlord->OnSelectResultLordGuard(&Rowset))
				return FALSE;
			}
			break;
		
		default :
			break;
		}
			
	return TRUE;
	}


BOOL AgsmRelay2::OnSelectResultLordGuard(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2LordGuard *pAgsdRelay2LordGuard = static_cast<AgsdRelay2LordGuard *>(pAgsdRelay2);

	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2LordGuard->m_ulNID);
	if (!pGameServer)
		return FALSE;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	INT32 lTotalSize = pRowset->GetRowBufferSize() * pRowset->GetRowCount();
	INT32 lTotalStep = 1 + (INT32) (lTotalSize / 20000);
	INT32 lRowsPerStep = 20000 / pRowset->GetRowBufferSize();
	for (INT32 lStep = 0; lStep < lTotalStep; lStep++)
		{
		PVOID pvPacket = NULL;
		pAgsdRelay2LordGuard->m_pvPacketEmb = MakeRowsetPacket2(pRowset, lStep, lRowsPerStep);
		pAgsdRelay2LordGuard->m_lInd = lStep + 1;

		BOOL bResult = SendLordGuard(pAgsdRelay2LordGuard, pGameServer->m_dpnidServer);

		printf("\nLord Guard[%d/%d][%s]\n", lStep+1, lTotalStep,  bResult ? _T("TRUE") : _T("FALSE"));
		}

	// last
	pAgsdRelay2LordGuard->m_pvPacketEmb = NULL;
	pAgsdRelay2LordGuard->m_lInd = AGSMRELAY2_ROWSET_IND_EOF;

	return SendLordGuard(pAgsdRelay2LordGuard, pGameServer->m_dpnidServer);
	}


BOOL AgsmRelay2::CBLordGuardSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgsmRelay2			*pThis = (AgsmRelay2 *)	pClass;

	AgsdRelay2LordGuard csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_SELECT;

	return pThis->SendLordGuard(&csRelay2);
	}


BOOL AgsmRelay2::CBLordGuardInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2		*pThis = (AgsmRelay2 *)	pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;
	
	AgsdRelay2LordGuard csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	_tcsncpy(csRelay2.m_szCharID, pAgpdCharacter->m_szID, _MAX_CHARNAME_LENGTH);
	
	return pThis->SendLordGuard(&csRelay2);
	}


BOOL AgsmRelay2::CBLordGuardDelete(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2		*pThis = (AgsmRelay2 *)	pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;
	
	AgsdRelay2LordGuard csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_DELETE;
	_tcsncpy(csRelay2.m_szCharID, pAgpdCharacter->m_szID, _MAX_CHARNAME_LENGTH);
	
	return pThis->SendLordGuard(&csRelay2);
	}


BOOL AgsmRelay2::SendLordGuard(AgsdRelay2LordGuard *pAgsdRelay2, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketEmb= m_csPacketLordGuard.MakePacket(FALSE, &nPacketLength, 0,
													  &pAgsdRelay2->m_eOperation,
													  pAgsdRelay2->m_szCharID,		// char id
													  &pAgsdRelay2->m_lInd,			// indicator
													  pAgsdRelay2->m_pvPacketEmb
													  );
	if (!pvPacketEmb)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_LORDGUARD);
	else
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_LORDGUARD, ulNID);

	m_csPacketLordGuard.FreePacket(pvPacketEmb);
	
	return bResult;	
	}




/************************************************************/
/*		The Implementation of AgsdRelay2Archlord class		*/
/************************************************************/
//
AgsdRelay2Archlord::AgsdRelay2Archlord()
	{
	m_lCID = AP_INVALID_CID;
	ZeroMemory(m_szCharID, sizeof(m_szCharID));
	ZeroMemory(m_szGuildID, sizeof(m_szGuildID));
	m_ulSiegeID = 0;
	m_pvPacketEmb = NULL;
	m_lInd = 0;
	}


BOOL AgsdRelay2Archlord::SetParamSelect(AuStatement* pStatement)
	{
	return TRUE;
	}


BOOL AgsdRelay2Archlord::SetParamUpdate(AuStatement* pStatement)
	{
	INT16 i=0;
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	pStatement->SetParam(i++, &m_ulSiegeID);
	return TRUE;
	}


BOOL AgsdRelay2Archlord::SetParamInsert(AuStatement *pStatement)
	{
	INT16 i=0;
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	pStatement->SetParam(i++, m_szGuildID, sizeof(m_szGuildID));
	pStatement->SetParam(i++, &m_ulSiegeID);
	return TRUE;	
	}




/************************************************************/
/*		The Implementation of AgsdRelay2LordGuard class		*/
/************************************************************/
//
AgsdRelay2LordGuard::AgsdRelay2LordGuard()
	{
	ZeroMemory(m_szCharID, sizeof(m_szCharID));
	m_pvPacketEmb = NULL;
	m_lInd = 0;
	}


BOOL AgsdRelay2LordGuard::SetParamSelect(AuStatement* pStatement)
	{
	return TRUE;
	}


BOOL AgsdRelay2LordGuard::SetParamInsert(AuStatement* pStatement)
	{
	INT16 i=0;
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	return TRUE;
	}


BOOL AgsdRelay2LordGuard::SetParamDelete(AuStatement *pStatement)
	{
	INT16 i=0;
	pStatement->SetParam(i++, m_szCharID, sizeof(m_szCharID));
	return TRUE;
	}

