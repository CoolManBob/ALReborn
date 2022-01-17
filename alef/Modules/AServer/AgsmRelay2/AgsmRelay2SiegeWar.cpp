/*===============================================================

	AgsmRelay2SiegeWar.cpp

===============================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"

#include "AuTranslateRegionName.h"


//
//	==========		Siege War		==========
//
void AgsmRelay2::InitPacketSiegeWar()
	{
	m_csPacketCastle.SetFlagLength(sizeof(INT16));
	m_csPacketCastle.SetFieldType(AUTYPE_INT16,		1,						// eAgsmRelay2Operation
								  AUTYPE_CHAR,		_MAX_GUILDID_LENGTH,	// castle
								  AUTYPE_CHAR,		_MAX_GUILDID_LENGTH,	// owner guild
								  AUTYPE_INT32,		1,						// indicator
								  AUTYPE_PACKET,	1,						// rowset packet
								  AUTYPE_END,		0
								  );

	m_csPacketSiege.SetFlagLength(sizeof(INT16));
	m_csPacketSiege.SetFieldType(AUTYPE_INT16,		1,						// eAgsmRelay2Operation
								 AUTYPE_CHAR,		_MAX_GUILDID_LENGTH,	// castle
								 AUTYPE_UINT32,		1,						// siege id
								 AUTYPE_UINT64,		1,						// proclaim date
								 AUTYPE_UINT64,		1,						// apply begin date
								 AUTYPE_UINT32,		1,						// apply elapsed
								 AUTYPE_UINT64,		1,						// siege begind ate
								 AUTYPE_UINT32,		1,						// siege elapsed
								 AUTYPE_UINT64,		1,						// siege end date
								 AUTYPE_UINT64,		1,						// previous end date
								 AUTYPE_INT32,		1,						// status
								 AUTYPE_CHAR,		_MAX_GUILDID_LENGTH,	// last carve guild
								 AUTYPE_INT32,		1,						// indicator
								 AUTYPE_PACKET,		1,						// rowset packet
								 AUTYPE_END,		0
								 );

	m_csPacketSiegeApplication.SetFlagLength(sizeof(INT16));
	m_csPacketSiegeApplication.SetFieldType(AUTYPE_INT16,		1,						// eAgsmRelay2Operation
											AUTYPE_CHAR,		_MAX_GUILDID_LENGTH,	// castle
											AUTYPE_UINT32,		1,						// siege id
											AUTYPE_CHAR,		_MAX_GUILDID_LENGTH,	// guild id
											AUTYPE_INT32,		1,						// side
											AUTYPE_UINT64,		1,						// apply date
											AUTYPE_INT32,		1,						// adopted
											AUTYPE_INT32,		1,						// indicator
											AUTYPE_PACKET,		1,						// rowset packet
											AUTYPE_END,			0
											);

	m_csPacketSiegeObject.SetFlagLength(sizeof(INT16));
	m_csPacketSiegeObject.SetFieldType(AUTYPE_INT16,		1,						// eAgsmRelay2Operation
									   AUTYPE_CHAR,			_MAX_GUILDID_LENGTH,	// castle
									   AUTYPE_INT32,		1,						// object id
									   AUTYPE_INT32,		1,						// tid
									   AUTYPE_INT32,		1,						// hit point
									   AUTYPE_CHAR,			_MAX_POSITION_LENGTH,	// position
									   AUTYPE_INT32,		1,						// status
									   AUTYPE_UINT64,		1,						// special status
									   AUTYPE_INT32,		1,						// indicator
									   AUTYPE_PACKET,		1,						// rowset packet
									   AUTYPE_END,			0
									   );
	
	m_csPacketTax.SetFlagLength(sizeof(INT32));
	m_csPacketTax.SetFieldType(AUTYPE_INT16,		1,						// eAgsmRelay2Operation
							   AUTYPE_CHAR,			_MAX_GUILDID_LENGTH,	// castle
							   AUTYPE_INT64,		1,						// total income
							   AUTYPE_UINT32,		1,						// latest transfer date
							   AUTYPE_UINT32,		1,						// latest modify date
							   AUTYPE_CHAR,			_MAX_GUILDID_LENGTH,	// region name	1
							   AUTYPE_INT32,		1,						// region ratio
							   AUTYPE_INT64,		1,						// region income
							   AUTYPE_CHAR,			_MAX_GUILDID_LENGTH,	// region name	2
							   AUTYPE_INT32,		1,						// region ratio
							   AUTYPE_INT64,		1,						// region income							   
							   AUTYPE_CHAR,			_MAX_GUILDID_LENGTH,	// region name	3
							   AUTYPE_INT32,		1,						// region ratio
							   AUTYPE_INT64,		1,						// region income
							   AUTYPE_CHAR,			_MAX_GUILDID_LENGTH,	// region name	4
							   AUTYPE_INT32,		1,						// region ratio
							   AUTYPE_INT64,		1,						// region income
							   AUTYPE_END,			0
							   );
	}


//
//	==========		Castle		==========
//
BOOL AgsmRelay2::OnParamCastle(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Castle *pAgsdRelay2 = new AgsdRelay2Castle;
	CHAR *pszCastle = NULL;
	CHAR *pszGuild = NULL;

	m_csPacketCastle.GetField(FALSE, pvPacket, 0,
							  &pAgsdRelay2->m_eOperation,
							  &pszCastle,
							  &pszGuild,
							  &pAgsdRelay2->m_lInd,
							  &pAgsdRelay2->m_pvPacketEmb
							  );

	pAgsdRelay2->m_ulNID = ulNID;
	_tcsncpy(pAgsdRelay2->m_szCastleID, pszCastle ? pszCastle : _T(""), AGPMSIEGEWAR_MAX_CASTLE_NAME);
	_tcsncpy(pAgsdRelay2->m_szOwnerGuildID, pszGuild ? pszGuild : _T(""), _MAX_GUILDID_LENGTH);
	
	return EnumCallback(AGSMRELAY_PARAM_CASTLE, (PVOID)pAgsdRelay2, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBOperationCastle(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationCastle,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBFinishOperationCastle(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT :
			pThis->OnSelectResultCastle(pRowset, pAgsdRelay2);
			break;
		
		default :
			break;
		}

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBOperationResultCastle(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2Castle *pAgsdRelay2 = (AgsdRelay2Castle *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT:
			{
			stBuddyRowset Rowset;
			if (NULL == pAgsdRelay2->m_pvPacketEmb && AGSMRELAY2_ROWSET_IND_EOF == pAgsdRelay2->m_lInd)
				{
				if (FALSE == pThis->m_pAgsmSiegeWar->OnSelectResultCastle(NULL, TRUE))
					return FALSE;
				}
			
			if (FALSE == pThis->ParseBuddyRowsetPacket(pAgsdRelay2->m_pvPacketEmb, &Rowset)
				|| FALSE == pThis->m_pAgsmSiegeWar->OnSelectResultCastle(&Rowset)
				)
				return FALSE;

			}
			break;
		
		default:
			break;
		}
			
	return TRUE;
	}


BOOL AgsmRelay2::OnSelectResultCastle(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2Castle *pAgsdRelay2Castle = static_cast<AgsdRelay2Castle *>(pAgsdRelay2);

	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2Castle->m_ulNID);
	if (!pGameServer)
		return FALSE;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	INT32 lTotalSize = pRowset->GetRowBufferSize() * pRowset->GetRowCount();
	INT32 lTotalStep = 1 + (INT32) (lTotalSize / 20000);
	INT32 lRowsPerStep = 20000 / pRowset->GetRowBufferSize();
	for (INT32 lStep = 0; lStep < lTotalStep; lStep++)
		{
		PVOID pvPacket = NULL;
		pAgsdRelay2Castle->m_pvPacketEmb = MakeRowsetPacket2(pRowset, lStep, lRowsPerStep);
		pAgsdRelay2Castle->m_lInd = lStep + 1;

		BOOL bResult = SendCastle(pAgsdRelay2Castle, pGameServer->m_dpnidServer);

		printf("\nCastle List sended[%d/%d][%s]\n", lStep+1, lTotalStep,  bResult ? _T("TRUE") : _T("FALSE"));
		}

	// last
	pAgsdRelay2Castle->m_pvPacketEmb = NULL;
	pAgsdRelay2Castle->m_lInd = AGSMRELAY2_ROWSET_IND_EOF;

	return SendCastle(pAgsdRelay2Castle, pGameServer->m_dpnidServer);
	}


BOOL AgsmRelay2::CBCastleSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;

	AgsdRelay2Castle csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_SELECT;

	return pThis->SendCastle(&csRelay2);
	}


BOOL AgsmRelay2::CBCastleUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;
	CHAR		*pszCastle = (CHAR *) pData;
	CHAR		*pszGuild = (CHAR *) pCustData;

	AgsdRelay2Castle csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;
	_tcsncpy(csRelay2.m_szCastleID, pszCastle, AGPMSIEGEWAR_MAX_CASTLE_NAME);
	_tcsncpy(csRelay2.m_szOwnerGuildID, pszGuild, _MAX_GUILDID_LENGTH);

	return pThis->SendCastle(&csRelay2);
	}


BOOL AgsmRelay2::SendCastle(AgsdRelay2Castle *pAgsdRelay2, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketEmb= m_csPacketCastle.MakePacket(FALSE, &nPacketLength, 0,
												   &pAgsdRelay2->m_eOperation,
												   pAgsdRelay2->m_szCastleID,
												   pAgsdRelay2->m_szOwnerGuildID,
												   &pAgsdRelay2->m_lInd,
												   pAgsdRelay2->m_pvPacketEmb
												   );
	if (!pvPacketEmb)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_CASTLE);
	else
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_CASTLE, ulNID);

	m_csPacketCastle.FreePacket(pvPacketEmb);
	
	return bResult;	
	}


//
//	==========		Siege		==========
//
BOOL AgsmRelay2::OnParamSiege(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Siege *pAgsdRelay2 = new AgsdRelay2Siege;
	CHAR *pszCastle = NULL;
	CHAR *pszGuild = NULL;

	m_csPacketSiege.GetField(FALSE, pvPacket, 0,
							 &pAgsdRelay2->m_eOperation,
							 &pszCastle,						// castle
							 &pAgsdRelay2->m_ulSiegeID,
							 &pAgsdRelay2->m_ullProclaimDate,	// proclaim date
							 &pAgsdRelay2->m_ullApplyBeginDate,	// apply begin date
							 &pAgsdRelay2->m_ulApplyElapsed,	// apply elapsed
							 &pAgsdRelay2->m_ullSiegeBeginDate,	// siege begind ate
							 &pAgsdRelay2->m_ulSiegeElapsed,	// siege elapsed
							 &pAgsdRelay2->m_ullSiegeEndDate,	// siege end date
							 &pAgsdRelay2->m_ullPrevEndDate,	// previous end date
							 &pAgsdRelay2->m_lStatus,			// status
							 &pszGuild,							// last carve guild
							 &pAgsdRelay2->m_lInd,				// indicator
							 &pAgsdRelay2->m_pvPacketEmb		// rowset packet
							 );

	pAgsdRelay2->m_ulNID = ulNID;
	_tcsncpy(pAgsdRelay2->m_szCastleID, pszCastle ? pszCastle : _T(""), AGPMSIEGEWAR_MAX_CASTLE_NAME);
	_tcsncpy(pAgsdRelay2->m_szLastCarveGuildID, pszGuild ? pszGuild : _T(""), _MAX_GUILDID_LENGTH);
	AuTimeStamp2::ConvertTimeStampToOracleTime(pAgsdRelay2->m_ullProclaimDate, pAgsdRelay2->m_szProclaimDate, sizeof(pAgsdRelay2->m_szProclaimDate));
	AuTimeStamp2::ConvertTimeStampToOracleTime(pAgsdRelay2->m_ullApplyBeginDate, pAgsdRelay2->m_szApplyBeginDate, sizeof(pAgsdRelay2->m_szApplyBeginDate));
	AuTimeStamp2::ConvertTimeStampToOracleTime(pAgsdRelay2->m_ullSiegeBeginDate, pAgsdRelay2->m_szSiegeBeginDate, sizeof(pAgsdRelay2->m_szSiegeBeginDate));
	AuTimeStamp2::ConvertTimeStampToOracleTime(pAgsdRelay2->m_ullSiegeEndDate, pAgsdRelay2->m_szSiegeEndDate, sizeof(pAgsdRelay2->m_szSiegeEndDate));
	AuTimeStamp2::ConvertTimeStampToOracleTime(pAgsdRelay2->m_ullPrevEndDate, pAgsdRelay2->m_szPrevEndDate, sizeof(pAgsdRelay2->m_szPrevEndDate));
	
	return EnumCallback(AGSMRELAY_PARAM_SIEGE, (PVOID)pAgsdRelay2, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBOperationSiege(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationSiege,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBFinishOperationSiege(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT :
			pThis->OnSelectResultSiege(pRowset, pAgsdRelay2);
			break;
		
		default :
			break;
		}

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBOperationResultSiege(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2Siege *pAgsdRelay2 = (AgsdRelay2Siege *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT:
			{
			stBuddyRowset Rowset;
			if (NULL == pAgsdRelay2->m_pvPacketEmb && AGSMRELAY2_ROWSET_IND_EOF == pAgsdRelay2->m_lInd)
				{
				if (FALSE == pThis->m_pAgsmSiegeWar->OnSelectResultSiege(NULL, TRUE))
					return FALSE;
				}
			
			if (FALSE == pThis->ParseBuddyRowsetPacket(pAgsdRelay2->m_pvPacketEmb, &Rowset)
				|| FALSE == pThis->m_pAgsmSiegeWar->OnSelectResultSiege(&Rowset)
				)
				return FALSE;

			}
			break;
		
		default:
			break;
		}
			
	return TRUE;
	}


BOOL AgsmRelay2::OnSelectResultSiege(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2Siege *pAgsdRelay2Siege = static_cast<AgsdRelay2Siege *>(pAgsdRelay2);

	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2Siege->m_ulNID);
	if (!pGameServer)
		return FALSE;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	INT32 lTotalSize = pRowset->GetRowBufferSize() * pRowset->GetRowCount();
	INT32 lTotalStep = 1 + (INT32) (lTotalSize / 20000);
	INT32 lRowsPerStep = 20000 / pRowset->GetRowBufferSize();
	for (INT32 lStep = 0; lStep < lTotalStep; lStep++)
		{
		PVOID pvPacket = NULL;
		pAgsdRelay2Siege->m_pvPacketEmb = MakeRowsetPacket2(pRowset, lStep, lRowsPerStep);
		pAgsdRelay2Siege->m_lInd = lStep + 1;

		BOOL bResult = SendSiege(pAgsdRelay2Siege, pGameServer->m_dpnidServer);

		printf("\nSiege List sended[%d/%d][%s]\n", lStep+1, lTotalStep,  bResult ? _T("TRUE") : _T("FALSE"));
		}

	// last
	pAgsdRelay2Siege->m_pvPacketEmb = NULL;
	pAgsdRelay2Siege->m_lInd = AGSMRELAY2_ROWSET_IND_EOF;

	return SendSiege(pAgsdRelay2Siege, pGameServer->m_dpnidServer);
	}


BOOL AgsmRelay2::CBSiegeSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;

	AgsdRelay2Siege csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_SELECT;

	return pThis->SendSiege(&csRelay2);
	}


BOOL AgsmRelay2::CBSiegeUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;
	AgpdSiegeWar *pAgpdSiegeWar = (AgpdSiegeWar *) pData;
	PVOID		*ppvBuffer = (PVOID *) pCustData;

	if (!ppvBuffer[0] || !ppvBuffer[1] || !ppvBuffer[2])
		return FALSE;
		
	AgsdRelay2Siege csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;
	csRelay2.m_ulApplyElapsed = *((UINT32 *) ppvBuffer[0]);
	csRelay2.m_ulSiegeElapsed = *((UINT32 *) ppvBuffer[1]);
	csRelay2.m_ullSiegeEndDate = *((UINT64 *) ppvBuffer[2]);
	csRelay2.m_lStatus = pAgpdSiegeWar->m_eCurrentStatus;
	_tcsncpy(csRelay2.m_szLastCarveGuildID, pAgpdSiegeWar->m_strCarveGuildName.GetBuffer(), AGPMGUILD_MAX_GUILD_ID_LENGTH);
	_tcsncpy(csRelay2.m_szCastleID, pAgpdSiegeWar->m_strCastleName.GetBuffer(), AGPMSIEGEWAR_MAX_CASTLE_NAME);
	csRelay2.m_ulSiegeID = pAgpdSiegeWar->m_ulSiegeDBID;

	return pThis->SendSiege(&csRelay2);
	}


BOOL AgsmRelay2::CBSiegeInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;
	AgpdSiegeWar *pAgpdSiegeWar = (AgpdSiegeWar *) pData;
	PVOID		*ppvBuffer = (PVOID *) pCustData;

	if (!ppvBuffer[0] || !ppvBuffer[1] || !ppvBuffer[2])
		return FALSE;

	AgsdRelay2Siege csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	_tcsncpy(csRelay2.m_szCastleID, pAgpdSiegeWar->m_strCastleName.GetBuffer(), AGPMSIEGEWAR_MAX_CASTLE_NAME);
	csRelay2.m_ulSiegeID = pAgpdSiegeWar->m_ulSiegeDBID;
	csRelay2.m_ullApplyBeginDate = pAgpdSiegeWar->m_ullApplicationTimeDate;
	csRelay2.m_ulApplyElapsed = *((UINT32 *) ppvBuffer[0]);
	csRelay2.m_ullSiegeBeginDate = pAgpdSiegeWar->m_ullNextSiegeWarTimeDate;
	csRelay2.m_ulSiegeElapsed = *((UINT32 *) ppvBuffer[1]);
	csRelay2.m_ullSiegeEndDate = *((UINT64 *) ppvBuffer[2]);
	csRelay2.m_ullPrevEndDate = pAgpdSiegeWar->m_ullPrevSiegeWarTimeDate;
	csRelay2.m_lStatus = pAgpdSiegeWar->m_eCurrentStatus;
	_tcsncpy(csRelay2.m_szLastCarveGuildID, pAgpdSiegeWar->m_strCarveGuildName.GetBuffer(), AGPMGUILD_MAX_GUILD_ID_LENGTH);

	return pThis->SendSiege(&csRelay2);
	}


BOOL AgsmRelay2::SendSiege(AgsdRelay2Siege *pAgsdRelay2, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketEmb= m_csPacketSiege.MakePacket(FALSE, &nPacketLength, 0,
												  &pAgsdRelay2->m_eOperation,
												  pAgsdRelay2->m_szCastleID,			// castle
												  &pAgsdRelay2->m_ulSiegeID,				// siege id
												  &pAgsdRelay2->m_ullProclaimDate,		// proclaim date
												  &pAgsdRelay2->m_ullApplyBeginDate,		// apply begin date
												  &pAgsdRelay2->m_ulApplyElapsed,		// apply elapsed
												  &pAgsdRelay2->m_ullSiegeBeginDate,		// siege begind ate
												  &pAgsdRelay2->m_ulSiegeElapsed,		// siege elapsed
												  &pAgsdRelay2->m_ullSiegeEndDate,		// siege end date
												  &pAgsdRelay2->m_ullPrevEndDate,		// previous end date
												  &pAgsdRelay2->m_lStatus,				// status
												  pAgsdRelay2->m_szLastCarveGuildID,	// last carve guild
												  &pAgsdRelay2->m_lInd,					// indicator
												  pAgsdRelay2->m_pvPacketEmb			// rowset packet
												  );	
	if (!pvPacketEmb)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_SIEGE);
	else
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_SIEGE, ulNID);

	m_csPacketSiege.FreePacket(pvPacketEmb);
	
	return bResult;	
	}


//
//	==========		Siege Application		==========
//
BOOL AgsmRelay2::OnParamSiegeApplication(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2SiegeApplication *pAgsdRelay2 = new AgsdRelay2SiegeApplication;
	CHAR *pszCastle = NULL;
	CHAR *pszGuild = NULL;

	m_csPacketSiegeApplication.GetField(FALSE, pvPacket, 0,
										&pAgsdRelay2->m_eOperation,
										&pszCastle,					// castle
										&pAgsdRelay2->m_ulSiegeID,	// siege id
										&pszGuild,					// guild id
										&pAgsdRelay2->m_lSide,		// side
										&pAgsdRelay2->m_ullApplyDate,	// apply date
										&pAgsdRelay2->m_lAdopted,	// adopted
										&pAgsdRelay2->m_lInd,		// indicator
										&pAgsdRelay2->m_pvPacketEmb	// rowset packet
										);

	pAgsdRelay2->m_ulNID = ulNID;
	_tcsncpy(pAgsdRelay2->m_szCastleID, pszCastle ? pszCastle : _T(""), AGPMSIEGEWAR_MAX_CASTLE_NAME);
	_tcsncpy(pAgsdRelay2->m_szGuildID, pszGuild ? pszGuild : _T(""), _MAX_GUILDID_LENGTH);
	AuTimeStamp2::ConvertTimeStampToOracleTime(pAgsdRelay2->m_ullApplyDate, pAgsdRelay2->m_szApplyDate, sizeof(pAgsdRelay2->m_szApplyDate));
	
	return EnumCallback(AGSMRELAY_PARAM_SIEGE_APPLICATION, (PVOID)pAgsdRelay2, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBOperationSiegeApplication(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationSiegeApplication,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBFinishOperationSiegeApplication(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT :
			pThis->OnSelectResultSiegeApplication(pRowset, pAgsdRelay2);
			break;
		
		default :
			break;
		}

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBOperationResultSiegeApplication(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2SiegeApplication *pAgsdRelay2 = (AgsdRelay2SiegeApplication *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT:
			{
			stBuddyRowset Rowset;
			if (NULL == pAgsdRelay2->m_pvPacketEmb && AGSMRELAY2_ROWSET_IND_EOF == pAgsdRelay2->m_lInd)
				{
				if (FALSE == pThis->m_pAgsmSiegeWar->OnSelectResultSiegeApplication(NULL, TRUE))
					return FALSE;
				}
			
			if (FALSE == pThis->ParseBuddyRowsetPacket(pAgsdRelay2->m_pvPacketEmb, &Rowset)
				|| FALSE == pThis->m_pAgsmSiegeWar->OnSelectResultSiegeApplication(&Rowset)
				)
				return FALSE;

			}
			break;
		
		default:
			break;
		}
			
	return TRUE;
	}


BOOL AgsmRelay2::OnSelectResultSiegeApplication(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2SiegeApplication *pAgsdRelay2SiegeApplication = static_cast<AgsdRelay2SiegeApplication *>(pAgsdRelay2);

	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2SiegeApplication->m_ulNID);
	if (!pGameServer)
		return FALSE;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	INT32 lTotalSize = pRowset->GetRowBufferSize() * pRowset->GetRowCount();
	INT32 lTotalStep = 1 + (INT32) (lTotalSize / 20000);
	INT32 lRowsPerStep = 20000 / pRowset->GetRowBufferSize();
	for (INT32 lStep = 0; lStep < lTotalStep; lStep++)
		{
		PVOID pvPacket = NULL;
		pAgsdRelay2SiegeApplication->m_pvPacketEmb = MakeRowsetPacket2(pRowset, lStep, lRowsPerStep);
		pAgsdRelay2SiegeApplication->m_lInd = lStep + 1;

		BOOL bResult = SendSiegeApplication(pAgsdRelay2SiegeApplication, pGameServer->m_dpnidServer);

		printf("\nSiege Application List sended[%d/%d][%s]\n", lStep+1, lTotalStep,  bResult ? _T("TRUE") : _T("FALSE"));
		}

	// last
	pAgsdRelay2SiegeApplication->m_pvPacketEmb = NULL;
	pAgsdRelay2SiegeApplication->m_lInd = AGSMRELAY2_ROWSET_IND_EOF;

	return SendSiegeApplication(pAgsdRelay2SiegeApplication, pGameServer->m_dpnidServer);
	}


BOOL AgsmRelay2::CBSiegeApplicationSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;

	AgsdRelay2SiegeApplication csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_SELECT;

	return pThis->SendSiegeApplication(&csRelay2);
	}


BOOL AgsmRelay2::CBSiegeApplicationUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;
	CHAR *pszCastle = (CHAR *) pData;
	PVOID *ppvBuffer = (PVOID *) pCustData;

	if (!ppvBuffer[0] || !ppvBuffer[1] || !ppvBuffer[2])
		return FALSE;

	AgsdRelay2SiegeApplication csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;
	
	_tcsncpy(csRelay2.m_szCastleID, pszCastle, AGPMSIEGEWAR_MAX_CASTLE_NAME);
	csRelay2.m_ulSiegeID = *((UINT32 *) ppvBuffer[1]);
	CHAR *pszGuild = (CHAR *) ppvBuffer[0];
	_tcsncpy(csRelay2.m_szGuildID, pszGuild, _MAX_GUILDID_LENGTH);
	//csRelay2.m_lSide;
	//csRelay2.m_ulApplyDate;
	csRelay2.m_lAdopted = *((INT32 *) ppvBuffer[2]);
	
	return pThis->SendSiegeApplication(&csRelay2);
	}


BOOL AgsmRelay2::CBSiegeApplicationInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;
	CHAR *pszCastle = (CHAR *) pData;
	PVOID *ppvBuffer = (PVOID *) pCustData;

	if (!ppvBuffer[0] || !ppvBuffer[1] || !ppvBuffer[2] || !ppvBuffer[3] || !ppvBuffer[4])
		return FALSE;

	AgsdRelay2SiegeApplication csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	
	_tcsncpy(csRelay2.m_szCastleID, pszCastle, AGPMSIEGEWAR_MAX_CASTLE_NAME);
	csRelay2.m_ulSiegeID = *((UINT32 *) ppvBuffer[1]);
	CHAR *pszGuild = (CHAR *) ppvBuffer[0];
	_tcsncpy(csRelay2.m_szGuildID, pszGuild, _MAX_GUILDID_LENGTH);
	csRelay2.m_lSide = *((INT32 *) ppvBuffer[3]);
	csRelay2.m_ullApplyDate = *((UINT64 *) ppvBuffer[4]);
	csRelay2.m_lAdopted = *((INT32 *) ppvBuffer[2]);
		
	return pThis->SendSiegeApplication(&csRelay2);
	}


BOOL AgsmRelay2::SendSiegeApplication(AgsdRelay2SiegeApplication *pAgsdRelay2, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketEmb= m_csPacketSiegeApplication.MakePacket(FALSE, &nPacketLength, 0,
															 &pAgsdRelay2->m_eOperation,
															 pAgsdRelay2->m_szCastleID,
															 &pAgsdRelay2->m_ulSiegeID,	// siege id
															 pAgsdRelay2->m_szGuildID,	// guild id
															 &pAgsdRelay2->m_lSide,		// side
															 &pAgsdRelay2->m_ullApplyDate,	// apply date
															 &pAgsdRelay2->m_lAdopted,	// adopted
															 &pAgsdRelay2->m_lInd,		// indicator
															 pAgsdRelay2->m_pvPacketEmb	// rowset packet
															 );
	if (!pvPacketEmb)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_SIEGE_APPLICATION);
	else
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_SIEGE_APPLICATION, ulNID);

	m_csPacketSiegeApplication.FreePacket(pvPacketEmb);
	
	return bResult;	
	}


//
//	==========		Siege Object		==========
//
BOOL AgsmRelay2::OnParamSiegeObject(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2SiegeObject *pAgsdRelay2 = new AgsdRelay2SiegeObject;
	CHAR *pszCastle = NULL;
	CHAR *pszPosition = NULL;

	m_csPacketSiegeObject.GetField(FALSE, pvPacket, 0,
								   &pAgsdRelay2->m_eOperation,
								   &pszCastle,					// castle
								   &pAgsdRelay2->m_lObjectID,	// object id
								   &pAgsdRelay2->m_lTID,		// tid
								   &pAgsdRelay2->m_lHP,			// hit point
								   &pszPosition,
								   &pAgsdRelay2->m_lStatus, 	// status
								   &pAgsdRelay2->m_ulSpecialStatus,			// special status
								   &pAgsdRelay2->m_lInd,		// indicator
								   &pAgsdRelay2->m_pvPacketEmb	// rowset packet
								   );

	pAgsdRelay2->m_ulNID = ulNID;
	_tcsncpy(pAgsdRelay2->m_szCastleID, pszCastle ? pszCastle : _T(""), AGPMSIEGEWAR_MAX_CASTLE_NAME);
	_tcsncpy(pAgsdRelay2->m_szPosition, pszPosition ? pszPosition : _T(""), _MAX_POSITION_LENGTH);

	_i64toa(pAgsdRelay2->m_ulSpecialStatus, pAgsdRelay2->m_szSpecialStatus, 10);

	return EnumCallback(AGSMRELAY_PARAM_SIEGE_OBJECT, (PVOID)pAgsdRelay2, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBOperationSiegeObject(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;

	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationSiegeObject,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBFinishOperationSiegeObject(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT :
			pThis->OnSelectResultSiegeObject(pRowset, pAgsdRelay2);
			break;
		
		default :
			break;
		}

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBOperationResultSiegeObject(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2SiegeObject *pAgsdRelay2 = (AgsdRelay2SiegeObject *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT:
			{
			stBuddyRowset Rowset;
			if (NULL == pAgsdRelay2->m_pvPacketEmb && AGSMRELAY2_ROWSET_IND_EOF == pAgsdRelay2->m_lInd)
				{
				if (FALSE == pThis->m_pAgsmSiegeWar->OnSelectResultSiegeObject(NULL, TRUE))
					return FALSE;
				}
			
			if (FALSE == pThis->ParseBuddyRowsetPacket(pAgsdRelay2->m_pvPacketEmb, &Rowset)
				|| FALSE == pThis->m_pAgsmSiegeWar->OnSelectResultSiegeObject(&Rowset)
				)
				return FALSE;

			}
			break;
		
		default:
			break;
		}
			
	return TRUE;
	}


BOOL AgsmRelay2::OnSelectResultSiegeObject(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2SiegeObject *pAgsdRelay2SiegeObject = static_cast<AgsdRelay2SiegeObject *>(pAgsdRelay2);

	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2SiegeObject->m_ulNID);
	if (!pGameServer)
		return FALSE;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	INT32 lTotalSize = pRowset->GetRowBufferSize() * pRowset->GetRowCount();
	INT32 lTotalStep = 1 + (INT32) (lTotalSize / 20000);
	INT32 lRowsPerStep = 20000 / pRowset->GetRowBufferSize();
	for (INT32 lStep = 0; lStep < lTotalStep; lStep++)
		{
		PVOID pvPacket = NULL;
		pAgsdRelay2SiegeObject->m_pvPacketEmb = MakeRowsetPacket2(pRowset, lStep, lRowsPerStep);
		pAgsdRelay2SiegeObject->m_lInd = lStep + 1;

		BOOL bResult = SendSiegeObject(pAgsdRelay2SiegeObject, pGameServer->m_dpnidServer);

		printf("\nSiege Object List sended[%d/%d][%s]\n", lStep+1, lTotalStep,  bResult ? _T("TRUE") : _T("FALSE"));
		}

	// last
	pAgsdRelay2SiegeObject->m_pvPacketEmb = NULL;
	pAgsdRelay2SiegeObject->m_lInd = AGSMRELAY2_ROWSET_IND_EOF;

	return SendSiegeObject(pAgsdRelay2SiegeObject, pGameServer->m_dpnidServer);
	}


BOOL AgsmRelay2::CBSiegeObjectSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;

	AgsdRelay2SiegeObject csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_SELECT;

	return pThis->SendSiegeObject(&csRelay2);
	}


BOOL AgsmRelay2::CBSiegeObjectUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;
	CHAR *pszCastle = (CHAR *) pData;
	PVOID *ppvBuffer = (PVOID *) pCustData;

	if (!ppvBuffer[0] || !ppvBuffer[1])
		return FALSE;
	
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) ppvBuffer[0];
	INT32 lObjectID = *((INT32 *) ppvBuffer[1]);

	INT32	lHP	= 0;															// HP
	pThis->m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lHP, AGPD_FACTORS_TYPE_RESULT,
									AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	INT32 lActionStatus = (INT32) pAgpdCharacter->m_unActionStatus;				// STATUS
	
	CHAR szPosition[64 + 1];													// POSITION
	ZeroMemory(szPosition, sizeof(szPosition));
	pThis->m_pAgsmCharacter->EncodingPosition(&pAgpdCharacter->m_stPos, szPosition, 64);
	
	AgsdRelay2SiegeObject csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;
	_tcsncpy(csRelay2.m_szCastleID, pszCastle, AGPMSIEGEWAR_MAX_CASTLE_NAME);
	csRelay2.m_lObjectID = lObjectID;
	csRelay2.m_lTID = pAgpdCharacter->m_lTID1;
	csRelay2.m_lHP = lHP;
	csRelay2.m_lStatus = lActionStatus;;
	csRelay2.m_ulSpecialStatus = (UINT64) pAgpdCharacter->m_ulSpecialStatus;
	_tcsncpy(csRelay2.m_szPosition, szPosition, _MAX_POSITION_LENGTH);

	return pThis->SendSiegeObject(&csRelay2);
	}


BOOL AgsmRelay2::CBSiegeObjectInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;
	CHAR *pszCastle = (CHAR *) pData;
	PVOID *ppvBuffer = (PVOID *) pCustData;

	if (!ppvBuffer[0] || !ppvBuffer[1])
		return FALSE;
	
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) ppvBuffer[0];
	INT32 lObjectID = *((INT32 *) ppvBuffer[1]);

	INT32 lTID = pAgpdCharacter->m_pcsCharacterTemplate->m_lID;					// TID
	INT32 lHP	= 0;															// HP
	pThis->m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lHP, AGPD_FACTORS_TYPE_RESULT,
									AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	INT32 lActionStatus = (INT32) pAgpdCharacter->m_unActionStatus;				// STATUS
	CHAR szPosition[64 + 1];													// POSITION
	ZeroMemory(szPosition, sizeof(szPosition));
	pThis->m_pAgsmCharacter->EncodingPosition(&pAgpdCharacter->m_stPos, szPosition, 64);
	
	AgsdRelay2SiegeObject csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	_tcsncpy(csRelay2.m_szCastleID, pszCastle, AGPMSIEGEWAR_MAX_CASTLE_NAME);
	csRelay2.m_lObjectID = lObjectID;
	csRelay2.m_lTID = lTID;
	csRelay2.m_lHP = lHP;
	csRelay2.m_lStatus = lActionStatus;;
	csRelay2.m_ulSpecialStatus = (UINT64) pAgpdCharacter->m_ulSpecialStatus;
	_tcsncpy(csRelay2.m_szPosition, szPosition, _MAX_POSITION_LENGTH);

	return pThis->SendSiegeObject(&csRelay2);
	}


BOOL AgsmRelay2::CBSiegeObjectDelete(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;
	CHAR *pszCastle = (CHAR *) pData;

	AgsdRelay2SiegeObject csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_DELETE;
	_tcsncpy(csRelay2.m_szCastleID, pszCastle, AGPMSIEGEWAR_MAX_CASTLE_NAME);

	return pThis->SendSiegeObject(&csRelay2);
	}


BOOL AgsmRelay2::SendSiegeObject(AgsdRelay2SiegeObject *pAgsdRelay2, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketEmb = m_csPacketSiegeObject.MakePacket(FALSE, &nPacketLength, 0,
														 &pAgsdRelay2->m_eOperation,
														 pAgsdRelay2->m_szCastleID,		// castle
														 &pAgsdRelay2->m_lObjectID,		// object id
														 &pAgsdRelay2->m_lTID,			// tid
														 &pAgsdRelay2->m_lHP,			// hit point
														 pAgsdRelay2->m_szPosition,		// position
														 &pAgsdRelay2->m_lStatus, 		// status
														 &pAgsdRelay2->m_ulSpecialStatus, // special status
														 &pAgsdRelay2->m_lInd,			// indicator
														 pAgsdRelay2->m_pvPacketEmb		// rowset packet
														 );
	if (!pvPacketEmb)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_SIEGE_OBJECT);
	else
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_SIEGE_OBJECT, ulNID);

	m_csPacketSiegeObject.FreePacket(pvPacketEmb);
	
	return bResult;	
	}


//
//	==========		Tax		==========
//
BOOL AgsmRelay2::OnParamTax(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Tax *pAgsdRelay2 = new AgsdRelay2Tax;
	CHAR	*pszCastle = NULL;
	CHAR	*pszRegion[4] = {NULL,NULL,NULL,NULL};

	m_csPacketTax.GetField(FALSE, pvPacket, 0,
							  &pAgsdRelay2->m_eOperation,
							  &pszCastle,
							  &pAgsdRelay2->m_llTotalIncome,
							  &pAgsdRelay2->m_ulLatestTransferDate,
							  &pAgsdRelay2->m_ulLatestModifyDate,
							  &pszRegion[0],
							  &pAgsdRelay2->m_lRegionRatios[0],
							  &pAgsdRelay2->m_llRegionIncomes[0],
							  &pszRegion[1],
							  &pAgsdRelay2->m_lRegionRatios[1],
							  &pAgsdRelay2->m_llRegionIncomes[1],
							  &pszRegion[2],
							  &pAgsdRelay2->m_lRegionRatios[2],
							  &pAgsdRelay2->m_llRegionIncomes[2],
							  &pszRegion[3],
							  &pAgsdRelay2->m_lRegionRatios[3],
							  &pAgsdRelay2->m_llRegionIncomes[3]
							  );

	pAgsdRelay2->m_ulNID = ulNID;
	_tcsncpy(pAgsdRelay2->m_szCastleID, pszCastle ? pszCastle : _T(""), AGPMSIEGEWAR_MAX_CASTLE_NAME);
	_i64toa(pAgsdRelay2->m_llTotalIncome, pAgsdRelay2->m_szTotalIncome, 10);
	AuTimeStamp::ConvertTimeStampToOracleTime(pAgsdRelay2->m_ulLatestTransferDate, pAgsdRelay2->m_szLatestTransferDate, sizeof(pAgsdRelay2->m_szLatestTransferDate));
	AuTimeStamp::ConvertTimeStampToOracleTime(pAgsdRelay2->m_ulLatestModifyDate, pAgsdRelay2->m_szLatestModifyDate, sizeof(pAgsdRelay2->m_szLatestModifyDate));
	
	for (INT32 i = 0; i < AGPDTAX_REGION_MAX; i++)
		{
		_tcsncpy(pAgsdRelay2->m_szRegionNames[i], pszRegion[i] ? pszRegion[i] : _T(""), _MAX_GUILDID_LENGTH);
		_i64toa(pAgsdRelay2->m_llRegionIncomes[i], pAgsdRelay2->m_szRegionIncomes[i], 10);
		}
	
	return EnumCallback(AGSMRELAY_PARAM_TAX, (PVOID)pAgsdRelay2, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBTaxUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay2	*pThis = (AgsmRelay2 *)	pClass;
	AgpdTax		*pAgpdTax = (AgpdTax *) pData;

	AgsdRelay2Tax csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_UPDATE;
	_tcsncpy(csRelay2.m_szCastleID, pAgpdTax->m_szCastle, AGPMSIEGEWAR_MAX_CASTLE_NAME);
	csRelay2.m_llTotalIncome = pAgpdTax->m_llTotalIncome;
	csRelay2.m_ulLatestTransferDate = pAgpdTax->m_ulLatestTransferDate;
	csRelay2.m_ulLatestModifyDate = pAgpdTax->m_ulLatestModifyDate;
	
	for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
		{
		AgpdRegionTax *pAgpdRegionTax = pAgpdTax->m_RegionTaxes[i];
		if (pAgpdRegionTax)
			{
				if (g_eServiceArea == AP_SERVICE_AREA_KOREA)
					_tcsncpy(csRelay2.m_szRegionNames[i], pAgpdRegionTax->m_szRegionName, _MAX_GUILDID_LENGTH);
				else
					{
					std::string regionName = GetEngName(pAgpdRegionTax->m_szRegionName);
					if (regionName == "") 
						regionName = pAgpdRegionTax->m_szRegionName;

					_tcsncpy(csRelay2.m_szRegionNames[i], regionName.c_str(), _MAX_GUILDID_LENGTH);
					}
			csRelay2.m_lRegionRatios[i] = pAgpdRegionTax->m_lRatio;
			csRelay2.m_llRegionIncomes[i] = pAgpdRegionTax->m_llIncome;
			}
		}

	return pThis->SendTax(&csRelay2);
	}


BOOL AgsmRelay2::SendTax(AgsdRelay2Tax *pAgsdRelay2, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketEmb= m_csPacketTax.MakePacket(FALSE, &nPacketLength, 0,
												&pAgsdRelay2->m_eOperation,
												pAgsdRelay2->m_szCastleID,
												&pAgsdRelay2->m_llTotalIncome,
												&pAgsdRelay2->m_ulLatestTransferDate,
												&pAgsdRelay2->m_ulLatestModifyDate,
												pAgsdRelay2->m_szRegionNames[0],
												&pAgsdRelay2->m_lRegionRatios[0],
												&pAgsdRelay2->m_llRegionIncomes[0],
												pAgsdRelay2->m_szRegionNames[1],
												&pAgsdRelay2->m_lRegionRatios[1],
												&pAgsdRelay2->m_llRegionIncomes[1],
												pAgsdRelay2->m_szRegionNames[2],
												&pAgsdRelay2->m_lRegionRatios[2],
												&pAgsdRelay2->m_llRegionIncomes[2],
												pAgsdRelay2->m_szRegionNames[3],
												&pAgsdRelay2->m_lRegionRatios[3],
												&pAgsdRelay2->m_llRegionIncomes[3]
												);
												
	if (!pvPacketEmb)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_TAX);
	else
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_TAX, ulNID);

	m_csPacketTax.FreePacket(pvPacketEmb);
	
	return bResult;	
	}



/************************************************************/
/*		The Implementation of AgsdRelay2Castle class		*/
/************************************************************/
//
AgsdRelay2Castle::AgsdRelay2Castle()
	{
	ZeroMemory(m_szCastleID, sizeof(m_szCastleID));
	ZeroMemory(m_szOwnerGuildID, sizeof(m_szOwnerGuildID));

	m_lInd = 0;
	m_pvPacketEmb = NULL;
	}


BOOL AgsdRelay2Castle::SetParamSelect(AuStatement *pStatement)
	{
	return TRUE;
	}


BOOL AgsdRelay2Castle::SetParamUpdate(AuStatement *pStatement)
	{
	pStatement->SetParam(0, m_szOwnerGuildID, sizeof(m_szOwnerGuildID));
	pStatement->SetParam(1, m_szCastleID, sizeof(m_szCastleID));
	return TRUE;
	}




/********************************************************/
/*		The Implementation of AgsdRelay2Siege class		*/
/********************************************************/
//
AgsdRelay2Siege::AgsdRelay2Siege()
	{
	ZeroMemory(m_szCastleID, sizeof(m_szCastleID));
	m_ulSiegeID = 0;
	m_ullProclaimDate = 0;
	m_ullApplyBeginDate = 0;
	m_ulApplyElapsed = 0;
	m_ullSiegeBeginDate = 0;
	m_ulSiegeElapsed = 0;
	m_ullSiegeEndDate = 0;
	m_ullPrevEndDate = 0;
	m_lStatus = AGPD_SIEGE_WAR_STATUS_OFF;
	ZeroMemory(m_szLastCarveGuildID, sizeof(m_szLastCarveGuildID));

	m_lInd = 0;
	m_pvPacketEmb = NULL;
	
	ZeroMemory(m_szProclaimDate, sizeof(m_szProclaimDate));
	ZeroMemory(m_szApplyBeginDate, sizeof(m_szApplyBeginDate));
	ZeroMemory(m_szSiegeBeginDate, sizeof(m_szSiegeBeginDate));
	ZeroMemory(m_szSiegeEndDate, sizeof(m_szSiegeEndDate));
	ZeroMemory(m_szPrevEndDate, sizeof(m_szPrevEndDate));
	}


BOOL AgsdRelay2Siege::SetParamSelect(AuStatement *pStatement)
	{
	return TRUE;
	}



BOOL AgsdRelay2Siege::SetParamInsert(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, m_szCastleID, sizeof(m_szCastleID));
	pStatement->SetParam(i++, &m_ulSiegeID);
	pStatement->SetParam(i++, m_szApplyBeginDate, sizeof(m_szApplyBeginDate));
	pStatement->SetParam(i++, &m_ulApplyElapsed);
	pStatement->SetParam(i++, m_szSiegeBeginDate, sizeof(m_szSiegeBeginDate));
	pStatement->SetParam(i++, &m_ulSiegeElapsed);
	pStatement->SetParam(i++, m_szSiegeEndDate, sizeof(m_szSiegeEndDate));
	pStatement->SetParam(i++, m_szPrevEndDate, sizeof(m_szPrevEndDate));	
	pStatement->SetParam(i++, &m_lStatus);
	pStatement->SetParam(i++, m_szLastCarveGuildID, sizeof(m_szLastCarveGuildID));
	
	return TRUE;
	}


BOOL AgsdRelay2Siege::SetParamUpdate(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, &m_ulApplyElapsed);
	pStatement->SetParam(i++, &m_ulSiegeElapsed);
	pStatement->SetParam(i++, m_szSiegeEndDate, sizeof(m_szSiegeEndDate));
	pStatement->SetParam(i++, &m_lStatus);
	pStatement->SetParam(i++, m_szLastCarveGuildID, sizeof(m_szLastCarveGuildID));
	
	pStatement->SetParam(i++, m_szCastleID, sizeof(m_szCastleID));
	pStatement->SetParam(i++, &m_ulSiegeID);
	
	return TRUE;
	}




/********************************************************************/
/*		The Implementation of AgsdRelay2SiegeAplilication class		*/
/********************************************************************/
//
AgsdRelay2SiegeApplication::AgsdRelay2SiegeApplication()
	{
	ZeroMemory(m_szCastleID, sizeof(m_szCastleID));
	m_ulSiegeID = 0;
	ZeroMemory(m_szGuildID, sizeof(m_szGuildID));
	m_ullApplyDate = 0;
	m_lSide = 0;
	m_lAdopted = 0;

	m_lInd = 0;
	m_pvPacketEmb = NULL;

	ZeroMemory(m_szApplyDate, sizeof(m_szApplyDate));
	}


BOOL AgsdRelay2SiegeApplication::SetParamInsert(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, m_szCastleID, sizeof(m_szCastleID));
	pStatement->SetParam(i++, &m_ulSiegeID);
	pStatement->SetParam(i++, m_szGuildID, sizeof(m_szGuildID));
	pStatement->SetParam(i++, &m_lSide);
	pStatement->SetParam(i++, m_szApplyDate, sizeof(m_szApplyDate));
	pStatement->SetParam(i++, &m_lAdopted);
	
	return TRUE;
	}


BOOL AgsdRelay2SiegeApplication::SetParamUpdate(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, &m_lAdopted);
	pStatement->SetParam(i++, m_szCastleID, sizeof(m_szCastleID));
	pStatement->SetParam(i++, &m_ulSiegeID);
	pStatement->SetParam(i++, m_szGuildID, sizeof(m_szGuildID));

	return TRUE;
	}


BOOL AgsdRelay2SiegeApplication::SetParamSelect(AuStatement *pStatement)
	{
	return TRUE;
	}




/****************************************************************/
/*		The Implementation of AgsdRelay2SiegeObject class		*/
/****************************************************************/
//
AgsdRelay2SiegeObject::AgsdRelay2SiegeObject()
	{
	ZeroMemory(m_szCastleID, sizeof(m_szCastleID));
	m_lObjectID = -1;
	m_lTID = 0;
	m_lHP = 0;
	ZeroMemory(m_szPosition, sizeof(m_szPosition));
	m_lStatus = 0;
	m_ulSpecialStatus = 0;

	m_lInd = 0;
	m_pvPacketEmb = NULL;

	ZeroMemory(m_szSpecialStatus, sizeof(m_szSpecialStatus));
	}


BOOL AgsdRelay2SiegeObject::SetParamInsert(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, m_szCastleID, sizeof(m_szCastleID));
	pStatement->SetParam(i++, &m_lObjectID);
	pStatement->SetParam(i++, &m_lTID);
	pStatement->SetParam(i++, &m_lHP);
	pStatement->SetParam(i++, m_szPosition, sizeof(m_szPosition));
	pStatement->SetParam(i++, &m_lStatus);
	pStatement->SetParam(i++, m_szSpecialStatus, sizeof(m_szSpecialStatus));
	
	return TRUE;
	}


BOOL AgsdRelay2SiegeObject::SetParamUpdate(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, &m_lTID);
	pStatement->SetParam(i++, &m_lHP);
	pStatement->SetParam(i++, m_szPosition, sizeof(m_szPosition));
	pStatement->SetParam(i++, &m_lStatus);
	pStatement->SetParam(i++, m_szSpecialStatus, sizeof(m_szSpecialStatus));

	pStatement->SetParam(i++, m_szCastleID, sizeof(m_szCastleID));
	pStatement->SetParam(i++, &m_lObjectID);
	
	return TRUE;
	}


BOOL AgsdRelay2SiegeObject::SetParamDelete(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, m_szCastleID, sizeof(m_szCastleID));
	
	return TRUE;
	}


BOOL AgsdRelay2SiegeObject::SetParamSelect(AuStatement *pStatement)
	{
	return TRUE;
	}




/********************************************************/
/*		The Implementation of AgsdRelay2Tax class		*/
/********************************************************/
//
AgsdRelay2Tax::AgsdRelay2Tax()
	{
	ZeroMemory(m_szCastleID, sizeof(m_szCastleID));
	m_llTotalIncome = 0;
	ZeroMemory(m_szTotalIncome, sizeof(m_szTotalIncome));
	m_ulLatestTransferDate = 0;
	m_ulLatestModifyDate = 0;
	ZeroMemory(m_szRegionNames, sizeof(m_szRegionNames));
	ZeroMemory(&m_lRegionRatios, sizeof(m_lRegionRatios));
	ZeroMemory(m_szRegionIncomes, sizeof(m_szRegionIncomes));
	ZeroMemory(&m_llRegionIncomes, sizeof(m_llRegionIncomes));

	// dummy
	ZeroMemory(m_szLatestTransferDate, sizeof(m_szLatestTransferDate));
	ZeroMemory(m_szLatestModifyDate, sizeof(m_szLatestModifyDate));
	}


BOOL AgsdRelay2Tax::SetParamUpdate(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, m_szTotalIncome, sizeof(m_szTotalIncome));
	pStatement->SetParam(i++, m_szLatestTransferDate, sizeof(m_szLatestTransferDate));
	pStatement->SetParam(i++, m_szLatestModifyDate, sizeof(m_szLatestModifyDate));
	pStatement->SetParam(i++, m_szRegionNames[0], sizeof(m_szRegionNames[0]));
	pStatement->SetParam(i++, &m_lRegionRatios[0]);
	pStatement->SetParam(i++, m_szRegionIncomes[0], sizeof(m_szRegionIncomes[0]));
	
	pStatement->SetParam(i++, m_szRegionNames[1], sizeof(m_szRegionNames[1]));
	pStatement->SetParam(i++, &m_lRegionRatios[1]);
	pStatement->SetParam(i++, m_szRegionIncomes[1], sizeof(m_szRegionIncomes[1]));
	
	pStatement->SetParam(i++, m_szRegionNames[2], sizeof(m_szRegionNames[2]));
	pStatement->SetParam(i++, &m_lRegionRatios[2]);
	pStatement->SetParam(i++, m_szRegionIncomes[2], sizeof(m_szRegionIncomes[2]));
	
	pStatement->SetParam(i++, m_szRegionNames[3], sizeof(m_szRegionNames[3]));
	pStatement->SetParam(i++, &m_lRegionRatios[3]);
	pStatement->SetParam(i++, m_szRegionIncomes[3], sizeof(m_szRegionIncomes[3]));
	
	pStatement->SetParam(i++, m_szCastleID, sizeof(m_szCastleID));
	
	return TRUE;
	}

