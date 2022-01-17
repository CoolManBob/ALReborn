/*===============================================================

	AgsmRelay2Guild.cpp

===============================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"
#include "AgppGuild.h"

//
//	==========		Guild		==========
//
void AgsmRelay2::InitPacketGuildMaster()
	{
	// 2005.04.17. steeple 수정
	m_csPacketGuildMaster.SetFlagLength(sizeof(INT32));
	m_csPacketGuildMaster.SetFieldType(AUTYPE_INT16,		1,								// eAgsmRelay2Operation
									   AUTYPE_CHAR,			_MAX_GUILDID_LENGTH + 1,		// GuildID
									   AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,		// Master ID
									   AUTYPE_INT32,		1,								// Guild TID
									   AUTYPE_INT32,		1,								// Guild Rank
									   AUTYPE_INT32,		1,								// Creation Date
									   AUTYPE_INT32,		1,								// Max Member Count
									   AUTYPE_INT32,		1,								// Union ID
									   AUTYPE_CHAR,			_MAX_GUILDPW_LENGTH + 1,		// Password
									   AUTYPE_INT8,			1,								// Status
									   AUTYPE_INT32,		1,								// Win
									   AUTYPE_INT32,		1,								// Draw
									   AUTYPE_INT32,		1,								// Lose
									   AUTYPE_CHAR,			_MAX_GUILDID_LENGTH + 1,		// Enemy Guild ID
									   AUTYPE_CHAR,			AGPMGUILD_MAX_DATE_LENGTH + 1,	// Battle StartTime
									   AUTYPE_UINT32,		1,								// Battle Duration
									   AUTYPE_INT32,		1,								// MyScore
									   AUTYPE_INT32,		1,								// Enemy Score
									   AUTYPE_INT32,		1,								// GuildMarkTID		2005.10.19. By SungHoon
									   AUTYPE_INT32,		1,								// GuildMarkColor	2005.10.19. By SungHoon
									   AUTYPE_INT64,		1,								// money
									   AUTYPE_INT32,		1,								// slot
									   AUTYPE_UINT32,		1,								// Guild Point
									   AUTYPE_END,			0
									   );
	}


void AgsmRelay2::InitPacketGuildMember()
	{
	m_csPacketGuildMember.SetFlagLength(sizeof(INT8));
	m_csPacketGuildMember.SetFieldType(AUTYPE_INT16,		1,							// eAgsmRelay2Operation
									   AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,	// CharID
									   AUTYPE_CHAR,			_MAX_GUILDID_LENGTH + 1,	// GuildID
									   AUTYPE_INT32,		1,							// Member Rank
									   AUTYPE_INT32,		1,							// Join Date
									   AUTYPE_INT32,		1,							// Level
									   AUTYPE_INT32,		1,							// TID
									   AUTYPE_END,			0
									   );
	}


void AgsmRelay2::InitPacketGuildItem()
	{
	m_csPacketGuildItem.SetFlagLength(sizeof(INT8));
	m_csPacketGuildItem.SetFieldType(AUTYPE_INT16,		1,							// eAgsmRelay2Operation
									 AUTYPE_INT32,		1,							// CID
									 AUTYPE_CHAR,		_MAX_GUILDID_LENGTH + 1,	// GuildID
									 AUTYPE_INT32,		1,							// indicator
									 AUTYPE_PACKET,		1,							// embeded packet
									 AUTYPE_END,		0
									 );
	}


BOOL AgsmRelay2::OnParamGuildMaster(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2GuildMaster *pcsRelayGuildMaster = new AgsdRelay2GuildMaster;

	CHAR*	pszGuild = NULL;
	CHAR*	pszMaster = NULL;
	CHAR*	pszPassword = NULL;
	CHAR*	pszEnemyGuildID = NULL;
	CHAR*	pszBattleStartTime = NULL;

	m_csPacketGuildMaster.GetField(FALSE, pvPacket, 0,
								   &pcsRelayGuildMaster->m_eOperation,
								   &pszGuild,
								   &pszMaster,
								   &pcsRelayGuildMaster->m_lGuildTID,
								   &pcsRelayGuildMaster->m_lGuildRank,
								   &pcsRelayGuildMaster->m_lCreationDate,
								   &pcsRelayGuildMaster->m_lMaxMemberCount,
								   &pcsRelayGuildMaster->m_lUnionID,
								   &pszPassword,
								   &pcsRelayGuildMaster->m_cStatus,
								   &pcsRelayGuildMaster->m_lWin,
								   &pcsRelayGuildMaster->m_lDraw,
								   &pcsRelayGuildMaster->m_lLose,
								   &pszEnemyGuildID,
								   &pszBattleStartTime,
								   &pcsRelayGuildMaster->m_ulBattleDuration,
								   &pcsRelayGuildMaster->m_lMyScore,
								   &pcsRelayGuildMaster->m_lEnemyScore,
								   &pcsRelayGuildMaster->m_lGuildMarkTID,		//2005.10.19. By SungHoon
								   &pcsRelayGuildMaster->m_lGuildMarkColor,		//2005.10.19. By SungHoon
								   &pcsRelayGuildMaster->m_llMoney,				// money
								   &pcsRelayGuildMaster->m_lSlot,				// slot
								   &pcsRelayGuildMaster->m_lGuildPoint
								   );

	if (AGSMDATABASE_OPERATION_SELECT != pcsRelayGuildMaster->m_eOperation && (!pszGuild || _T('\0') == *pszGuild))
		{
		pcsRelayGuildMaster->Release();
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : GuildID is NULL or Empty in OnParamGuildMaster()\n");
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);

		return FALSE;
		}

	pcsRelayGuildMaster->m_ulNID = ulNID;
	_tcsncpy(pcsRelayGuildMaster->m_szGuildID, pszGuild ? pszGuild : _T(""), _MAX_GUILDID_LENGTH);
	_tcsncpy(pcsRelayGuildMaster->m_szMasterID, pszMaster ? pszMaster : _T(""), _MAX_CHARNAME_LENGTH);
	_tcsncpy(pcsRelayGuildMaster->m_szPassword, pszPassword ? pszPassword : _T(""), _MAX_GUILDPW_LENGTH);
	_tcsncpy(pcsRelayGuildMaster->m_szEnemyGuildID, pszEnemyGuildID ? pszEnemyGuildID : _T(""), _MAX_GUILDID_LENGTH);
	_tcsncpy(pcsRelayGuildMaster->m_szBattleStartTime, pszBattleStartTime ? pszBattleStartTime : _T(""), AGPMGUILD_MAX_DATE_LENGTH);
	_i64tot(pcsRelayGuildMaster->m_llMoney, pcsRelayGuildMaster->m_szMoney, 10);

	if (AGSMRELAY_PARAM_GUILD_MASTER_CHECK == nParam)
	{
		pcsRelayGuildMaster->m_bCheckSelect = TRUE;
	}
	else if(AGSMRELAY_PARAM_GUILD_BATTLE_HISTORY == nParam)
	{
		pcsRelayGuildMaster->m_bBattleHistoryInsert = TRUE;
	}
	
	return EnumCallback(nParam, (PVOID)pcsRelayGuildMaster, (PVOID)nParam);
	}


BOOL AgsmRelay2::OnParamGuildMember(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2GuildMember *pcsRelayGuildMember = new AgsdRelay2GuildMember;

	CHAR*	pszGuild = NULL;
	CHAR*	pszMember = NULL;

	m_csPacketGuildMember.GetField(FALSE, pvPacket, 0,
								   &pcsRelayGuildMember->m_eOperation,	
								   &pszMember,
								   &pszGuild,
								   &pcsRelayGuildMember->m_lMemberRank,
								   &pcsRelayGuildMember->m_lJoinDate,
								   &pcsRelayGuildMember->m_lLevel,
								   &pcsRelayGuildMember->m_lTID
								   );

	if (AGSMDATABASE_OPERATION_SELECT != pcsRelayGuildMember->m_eOperation && (!pszMember || _T('\0') == pszMember))
		{
		pcsRelayGuildMember->Release();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!!Error : Member is NULL or Empty in OnParamGuildMember()\n");
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);
		return FALSE;
		}

	//pcsRelayGuildMember->Dump(_T("RCV"));

	pcsRelayGuildMember->m_ulNID = ulNID;
	_tcsncpy(pcsRelayGuildMember->m_szMemberID, pszMember ? pszMember : _T(""), _MAX_CHARNAME_LENGTH);
	_tcsncpy(pcsRelayGuildMember->m_szGuildID, pszGuild, _MAX_GUILDID_LENGTH);
	AuTimeStamp::ConvertTimeStampToOracleTime(pcsRelayGuildMember->m_lJoinDate, pcsRelayGuildMember->m_szJoinDate, sizeof(pcsRelayGuildMember->m_szJoinDate));		//	2005.07.28. SungHoon

	return EnumCallback(AGSMRELAY_PARAM_GUILD_MEMBER, (PVOID)pcsRelayGuildMember, (PVOID)nParam);
	}


BOOL AgsmRelay2::OnParamGuildItem(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2GuildItem *pAgsdRelay2 = new AgsdRelay2GuildItem;

	CHAR*	pszGuild = NULL;
	m_csPacketGuildItem.GetField(FALSE, pvPacket, 0,
								 &pAgsdRelay2->m_eOperation,
								 &pAgsdRelay2->m_lCID,
								 &pszGuild,
								 &pAgsdRelay2->m_lInd,
								 &pAgsdRelay2->m_pvPacketEmb
								 );

	pAgsdRelay2->m_ulNID = ulNID;
	_tcsncpy(pAgsdRelay2->m_szGuildID, pszGuild, _MAX_GUILDID_LENGTH);

	BOOL bResult = FALSE;
	if (AGSMDATABASE_OPERATION_SELECT == pAgsdRelay2->m_eOperation)
		{
		bResult = EnumCallback(AGSMRELAY_PARAM_GUILD_WAREHOUSE_ITEM, pAgsdRelay2, (PVOID)nParam);
		}
	else
		{
		if (pAgsdRelay2->m_pvPacketEmb)
			bResult = OnParamItem(AGSMRELAY_PARAM_GUILD_WAREHOUSE_ITEM, pAgsdRelay2->m_pvPacketEmb, ulNID);
		pAgsdRelay2->Release();
		}

	return bResult;
	}




//	CBOperation
//====================================================
//
BOOL AgsmRelay2::CBOperationGuildMaster(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;
	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationGuildMaster,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBOperationGuildMasterCheck(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;
	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationGuildMasterCheck,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBOperationGuildMember(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;
	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationGuildMember,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}


BOOL AgsmRelay2::CBOperationGuildItem(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pData;
	INT16 nParam = (INT16) pCustData;

	pAgsdRelay2->m_nParam = nParam;
	INT16 nIndex = pAgsdRelay2->m_eOperation + nParam;
	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = nIndex;
	pQuery->m_pParam = pAgsdRelay2;
	pQuery->SetCallback(AgsmRelay2::CBFinishOperationGuildItem,
						AgsmRelay2::CBFailOperation,
						pThis, pAgsdRelay2);

	return pThis->m_pAgsmDatabasePool->Execute(pQuery);
	}




//	CBFinishOperation
//====================================================
//
BOOL AgsmRelay2::CBFinishOperationGuildMaster(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	if (NULL != pRowset && AGSMDATABASE_OPERATION_SELECT == pAgsdRelay2->m_eOperation)
		pThis->OnSelectResultGuildMaster(pRowset, pAgsdRelay2);

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBFinishOperationGuildMasterCheck(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	if (NULL != pRowset && AGSMDATABASE_OPERATION_SELECT == pAgsdRelay2->m_eOperation)
		pThis->OnSelectResultGuildMaster(pRowset, pAgsdRelay2, TRUE);

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBFinishOperationGuildMember(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	if (NULL != pRowset && AGSMDATABASE_OPERATION_SELECT == pAgsdRelay2->m_eOperation)
	{
		pThis->OnSelectResultGuildMember(pRowset, pAgsdRelay2);

		if(pAgsdRelay2->m_nParam == AGSMRELAY_PARAM_GUILD_MEMBER)
			pThis->SendGuildMemberLoadingComplete(pAgsdRelay2);
	}

	pAgsdRelay2->Release();

	return TRUE;
	}


BOOL AgsmRelay2::CBFinishOperationGuildItem(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdDBParam *pAgsdRelay2 = (AgsdDBParam *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT :
			pThis->OnSelectResultGuildItem(pRowset, pAgsdRelay2);
			break;
		
		default :
			break;
		}

	pAgsdRelay2->Release();

	return TRUE;
	}




//	CBOperationResult
//====================================================
//
//	Game server side result operation callback
//
BOOL AgsmRelay2::CBOperationResultGuildMaster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2GuildMaster *pAgsdRelay2 = (AgsdRelay2GuildMaster *) pData;
	
	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	if (AGSMDATABASE_OPERATION_SELECT != pAgsdRelay2->m_eOperation)
		return FALSE;

	return pThis->m_pAgsmGuild->GuildLoadFromDB(pAgsdRelay2);
}


BOOL AgsmRelay2::CBOperationResultGuildMasterCheck(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2GuildMaster *pAgsdRelay2 = (AgsdRelay2GuildMaster *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	if (AGSMDATABASE_OPERATION_SELECT != pAgsdRelay2->m_eOperation)
		return FALSE;

	return pThis->m_pAgsmGuild->GuildIDCheckFromDB((BOOL)pAgsdRelay2->m_lGuildTID,	// 여기다가 Select 결과를 넣었음.
												pAgsdRelay2->m_szGuildID,
												pAgsdRelay2->m_szMasterID,
												pAgsdRelay2->m_szPassword);
	}


BOOL AgsmRelay2::CBOperationResultGuildMember(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2GuildMember *pAgsdRelay2 = (AgsdRelay2GuildMember *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	if (AGSMDATABASE_OPERATION_SELECT != pAgsdRelay2->m_eOperation)
		return FALSE;

	return pThis->m_pAgsmGuild->MemberLoadFromDB(pAgsdRelay2->m_szMemberID, pAgsdRelay2->m_szGuildID,
										  pAgsdRelay2->m_lMemberRank, pAgsdRelay2->m_lJoinDate,
										  pAgsdRelay2->m_lLevel, pAgsdRelay2->m_lTID);
	}


BOOL AgsmRelay2::CBOperationResultGuildItem(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2GuildItem *pAgsdRelay2 = (AgsdRelay2GuildItem *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	switch (pAgsdRelay2->m_eOperation)
		{
		case AGSMDATABASE_OPERATION_SELECT:
			{
			AgpdCharacter *pAgpdCharacter = pThis->m_pAgpmCharacter->GetCharacter(pAgsdRelay2->m_lCID);
			stBuddyRowset Rowset;
			if (NULL == pAgsdRelay2->m_pvPacketEmb && AGSMRELAY2_ROWSET_IND_EOF == pAgsdRelay2->m_lInd)
				{
				if (FALSE == pThis->m_pAgsmGuildWarehouse->OnLoadResult(pAgpdCharacter, NULL, TRUE))
					return FALSE;
				}
			
			if (FALSE == pThis->ParseBuddyRowsetPacket(pAgsdRelay2->m_pvPacketEmb, &Rowset)
				|| FALSE == pThis->m_pAgsmGuildWarehouse->OnLoadResult(pAgpdCharacter, &Rowset, FALSE)
				)
				return FALSE;
			}
			break;
		
		default:
			break;
		}
			
	return TRUE;
	}




//	OnSelectResult
//===================================================
//
BOOL AgsmRelay2::OnSelectResultGuildMaster(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2, BOOL bCheck)
	{
	if (AGSMRELAY_PARAM_GUILD_RELATION == pAgsdRelay2->m_nParam)
		{
		for (UINT32 i = 0; i < pRowset->GetRowCount(); i++)
			{
			CHAR *pszGuildID = (CHAR *)pRowset->Get(i, 0);
			CHAR *pszTargetGuildID = (CHAR *)pRowset->Get(i, 1);
			INT8 cRelation = pRowset->Get(i, 2) ? atoi((CHAR *)pRowset->Get(i, 2)) : 0;
			UINT32 ulDate = pRowset->Get(i, 3) ? atoi((CHAR *)pRowset->Get(i, 3)) : 0;
			
			SendGuildJointSelect(pszGuildID, pszTargetGuildID, ulDate, cRelation, pAgsdRelay2->m_ulNID);
			}


		// Relation 이 끝나면 길드 멤버 로딩한다.

			//// Naver_charmaster 조인을 먼저 해준다.
			//AgsdRelay2GuildMember* pcsGuildMemberNaver = new AgsdRelay2GuildMember;
			//pcsGuildMemberNaver->m_eOperation = AGSMDATABASE_OPERATION_SELECT;
			//pcsGuildMemberNaver->m_ulNID = pAgsdRelay2->m_ulNID;
			//INT16 nParamNaver = AGSMRELAY_PARAM_GUILD_MEMBER_NAVER;

			//EnumCallback(AGSMRELAY_PARAM_GUILD_MEMBER, (PVOID)pcsGuildMemberNaver, (PVOID)nParamNaver);

			//Sleep(500);

			AgsdRelay2GuildMember* pcsGuildMember = new AgsdRelay2GuildMember;
			pcsGuildMember->m_eOperation = AGSMDATABASE_OPERATION_SELECT;
			pcsGuildMember->m_ulNID = pAgsdRelay2->m_ulNID;
			INT16 nParam = AGSMRELAY_PARAM_GUILD_MEMBER;

			EnumCallback(AGSMRELAY_PARAM_GUILD_MEMBER, (PVOID)pcsGuildMember, (PVOID)nParam);

			return TRUE;
		}

	AgsdRelay2GuildMaster csGuildMaster;

	// 루프를 돌면서 몽땅 보낸다.
	UINT32 i = 0;
	if(!bCheck)
		{
		for (i = 0; i < pRowset->GetRowCount(); i++)
			{
			_tcsncpy(csGuildMaster.m_szGuildID, (CHAR *) pRowset->Get(i, 2) ? (CHAR *) pRowset->Get(i, 0) : "", _MAX_GUILDID_LENGTH);		// 길드 아이디
			csGuildMaster.m_lGuildTID = atoi((CHAR *) pRowset->Get(i, 1) ? (CHAR *) pRowset->Get(i, 1) : "");		// 길드 TID
			_tcsncpy(csGuildMaster.m_szMasterID, (CHAR *) pRowset->Get(i, 2) ? (CHAR *) pRowset->Get(i, 2) : "", _MAX_CHARNAME_LENGTH);	// 길드마스터 아이디
			csGuildMaster.m_lMaxMemberCount = atoi((CHAR *) pRowset->Get(i, 3) ? (CHAR *) pRowset->Get(i, 3) : "");// Max Member Count

			// 2005.04.29. steeple.
			// Create Date 를 TimeStamp 로 변환해준다.
			if(pRowset->Get(i, 4))
				csGuildMaster.m_lCreationDate = (INT32)AuTimeStamp::ConvertOracleTimeToTimeStamp((CHAR*)pRowset->Get(i, 4));
			else
				csGuildMaster.m_lCreationDate = 0;
			//csGuildMaster.m_lCreationDate = atoi((CHAR *) pRowset->Get(i, 4) ? (CHAR *) pRowset->Get(i, 4) : "");	// 생성 날짜
			csGuildMaster.m_lUnionID = atoi((CHAR *) pRowset->Get(i, 5) ? (CHAR *) pRowset->Get(i, 5) : "");		// 유니온 아이디
			csGuildMaster.m_lGuildRank = atoi((CHAR *) pRowset->Get(i, 6) ? (CHAR *) pRowset->Get(i, 6) : "");		// 길드 랭크
			_tcsncpy(csGuildMaster.m_szPassword, (CHAR *) pRowset->Get(i, 7) ? (CHAR *) pRowset->Get(i, 7) : "", _MAX_GUILDPW_LENGTH);		// Password

			csGuildMaster.m_lWin = atoi((CHAR *) pRowset->Get(i, 8) ? (CHAR *) pRowset->Get(i, 8) : "");			// 승리수
			csGuildMaster.m_lDraw = atoi((CHAR *) pRowset->Get(i, 9) ? (CHAR *) pRowset->Get(i, 9) : "");			// 무승부수
			csGuildMaster.m_lLose = atoi((CHAR *) pRowset->Get(i, 10) ? (CHAR *) pRowset->Get(i, 10) : "");			// 패배수
			_tcsncpy(csGuildMaster.m_szEnemyGuildID, (CHAR *) pRowset->Get(i, 11) ? (CHAR *) pRowset->Get(i, 11) : "", _MAX_GUILDID_LENGTH);	// 적길드 이름
			_tcsncpy(csGuildMaster.m_szBattleStartTime, (CHAR *) pRowset->Get(i, 12) ? (CHAR *) pRowset->Get(i, 12) : "", AGPMGUILD_MAX_DATE_LENGTH);	// 배틀 시작 시간
			csGuildMaster.m_ulBattleDuration = atol((CHAR *) pRowset->Get(i, 13) ? (CHAR *) pRowset->Get(i, 13) : "");	// 배틀기간. 초단위
			csGuildMaster.m_lMyScore = atoi((CHAR *) pRowset->Get(i, 14) ? (CHAR *) pRowset->Get(i, 14) : "");		// 우리편 점수
			csGuildMaster.m_lGuildMarkTID= atoi((CHAR *) pRowset->Get(i, 15) ? (CHAR *) pRowset->Get(i, 15) : "");		// 길드마크TID
			csGuildMaster.m_lGuildMarkColor = atoi((CHAR *) pRowset->Get(i, 16) ? (CHAR *) pRowset->Get(i, 16) : "");	// 길드마크Color
			csGuildMaster.m_llMoney = _ttoi64((CHAR *) pRowset->Get(i, 17) ? (CHAR *) pRowset->Get(i, 17) : "");	// money
			csGuildMaster.m_lSlot = atoi((CHAR *) pRowset->Get(i, 18) ? (CHAR *) pRowset->Get(i, 18) : "");	// slot
			csGuildMaster.m_lGuildPoint = atoi((CHAR *) pRowset->Get(i, 19) ? (CHAR *) pRowset->Get(i, 19) : "");	// slot
			

			csGuildMaster.m_lEnemyScore = 0;

			csGuildMaster.m_cStatus = _tcslen(csGuildMaster.m_szEnemyGuildID) > 0 ? AGPMGUILD_STATUS_BATTLE : AGPMGUILD_STATUS_NONE;

			SendSelectResultGuildMaster(pAgsdRelay2->m_ulNID, &csGuildMaster, bCheck);
			}

			// 2005.05.04. steeple
			// 길드가 하나두 없으면 걍 다 뜬걸로 보내준다.
			if(pRowset->GetRowCount() == 0)
			{
				SendGuildMemberLoadingComplete(pAgsdRelay2);
			}
		}
	else
		{
		_tcsncpy(csGuildMaster.m_szGuildID, ((AgsdRelay2GuildMaster*)pAgsdRelay2)->m_szGuildID, _MAX_GUILDID_LENGTH);		// 길드 아이디
		csGuildMaster.m_lGuildTID = pRowset->GetRowCount() == 0 ? 1 : 0;
		_tcsncpy(csGuildMaster.m_szMasterID, ((AgsdRelay2GuildMaster*)pAgsdRelay2)->m_szMasterID, _MAX_CHARNAME_LENGTH);	// 길드마스터 아이디
		_tcsncpy(csGuildMaster.m_szPassword, ((AgsdRelay2GuildMaster*)pAgsdRelay2)->m_szPassword, _MAX_GUILDPW_LENGTH);		// Password

		SendSelectResultGuildMaster(pAgsdRelay2->m_ulNID, &csGuildMaster, bCheck);
		}

	// 2006.08.11. steeple
	// 길드 로딩이 끝나면, 길드 연대/적대를 로딩한다.
	if (!bCheck && (i > 0))
		{
		AgsdRelay2GuildMaster* pcsGuildMaster = new AgsdRelay2GuildMaster;
		pcsGuildMaster->m_eOperation = AGSMDATABASE_OPERATION_SELECT;
		pcsGuildMaster->m_ulNID = pAgsdRelay2->m_ulNID;
		INT16 nParam = AGSMRELAY_PARAM_GUILD_RELATION;

		return EnumCallback(AGSMRELAY_PARAM_GUILD_MASTER, (PVOID)pcsGuildMaster, (PVOID)nParam);
		}
	return TRUE;
	}


BOOL AgsmRelay2::OnSelectResultGuildMember(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2GuildMember csGuildMember;
	
	// 루프를 돌면서 몽땅 보낸다.
	UINT32 i = 0;
	for (i = 0; i < pRowset->GetRowCount(); i++)
		{
		_tcsncpy(csGuildMember.m_szMemberID, (CHAR *) pRowset->Get(i, 0) ? (CHAR *) pRowset->Get(i, 0) : "", _MAX_CHARNAME_LENGTH);		// Char ID
		_tcsncpy(csGuildMember.m_szGuildID, (CHAR *) pRowset->Get(i, 1) ? (CHAR *) pRowset->Get(i, 1) : "", _MAX_GUILDID_LENGTH);			// Guild ID
		csGuildMember.m_lMemberRank = atoi((CHAR *) pRowset->Get(i, 2));		// Member Rank
//		csGuildMember.m_lJoinDate = atoi((CHAR *) pRowset->Get(i, 3));			// Join Date
		csGuildMember.m_lJoinDate = AuTimeStamp::ConvertOracleTimeToTimeStamp((CHAR *) pRowset->Get(i, 3));			// Join Date	2005.08.11. By SungHoon
		csGuildMember.m_lTID = atoi((CHAR *) pRowset->Get(i, 4));				// Char TID
		csGuildMember.m_lLevel = atoi((CHAR *) pRowset->Get(i, 5));			// Level

		SendSelectResultGuildMember(pAgsdRelay2->m_ulNID, &csGuildMember);
		}

	return TRUE;
	}


BOOL AgsmRelay2::OnSelectResultGuildItem(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2)
	{
	AgsdRelay2GuildItem *pAgsdRelay2GuildItem = static_cast<AgsdRelay2GuildItem *>(pAgsdRelay2);
	AgsdServer *pGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(pAgsdRelay2->m_ulNID);
	if (!pGameServer)
		return FALSE;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	INT32 lTotalSize = pRowset->GetRowBufferSize() * pRowset->GetRowCount();
	INT32 lTotalStep = 1 + (INT32) (lTotalSize / 20000);
	INT32 lRowsPerStep = 20000 / pRowset->GetRowBufferSize();
	for (INT32 lStep = 0; lStep < lTotalStep; lStep++)
		{
		PVOID pvPacket = NULL;
		pAgsdRelay2GuildItem->m_pvPacketEmb = MakeRowsetPacket2(pRowset, lStep, lRowsPerStep);
		pAgsdRelay2GuildItem->m_lInd = lStep + 1;

		BOOL bResult = SendGuildItem(pAgsdRelay2GuildItem, pGameServer->m_dpnidServer);

		printf("\nCastle List sended[%d/%d][%s]\n", lStep+1, lTotalStep,  bResult ? _T("TRUE") : _T("FALSE"));
		}

	// last
	pAgsdRelay2GuildItem->m_pvPacketEmb = NULL;
	pAgsdRelay2GuildItem->m_lInd = AGSMRELAY2_ROWSET_IND_EOF;

	return SendGuildItem(pAgsdRelay2GuildItem, pGameServer->m_dpnidServer);
	}

BOOL AgsmRelay2::SendGuildJointSelect(CHAR* szGuildID, CHAR* szJointGuildID, UINT32 ulDate, INT8 cRelation, UINT32 ulNID)
{
	if (0 != ulNID && (!szGuildID || !szJointGuildID))
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,		
																 szGuildID,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 &cRelation,
																 NULL,
																 NULL,
																 NULL,
																 szJointGuildID,
																 NULL,
																 &ulDate,
																 NULL,
																 NULL,
																 NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
																 NULL,
																 NULL,
																 NULL
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = FALSE;
	if (0 == ulNID)
		MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_RELATION);
	else
		MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_RELATION, ulNID);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
}

BOOL AgsmRelay2::SendGuildJointInsert(CHAR* szGuildID, CHAR* szJointGuildID, UINT32 ulDate, INT8 cRelation)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_INSERT;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,		
																 szGuildID,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 &cRelation,
																 NULL,
																 NULL,
																 NULL,
																 szJointGuildID,
																 NULL,
																 &ulDate,
																 NULL,
																 NULL,
																 NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
																 NULL,
																 NULL,
																 NULL
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_RELATION);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
}

BOOL AgsmRelay2::SendGuildJointUpdate(CHAR* szGuildID, CHAR* szJointGuildID, INT8 cRelation)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,		
																 szGuildID,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 &cRelation,
																 NULL,
																 NULL,
																 NULL,
																 szJointGuildID,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
																 NULL,
																 NULL,
																 NULL
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_RELATION);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
}

BOOL AgsmRelay2::SendGuildJointDelete(CHAR* szGuildID, CHAR* szJointGuildID)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_DELETE;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,		
																 szGuildID,
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
																 szJointGuildID,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
																 NULL,
																 NULL,
																 NULL
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_RELATION);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
}

BOOL AgsmRelay2::SendGuildHostileInsert(CHAR* szGuildID, CHAR* szHostileGuildID, UINT32 ulDate)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	INT8 cRelation = AGPMGUILD_RELATION_HOSTILE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_INSERT;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,		
																 szGuildID,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 &cRelation,
																 NULL,
																 NULL,
																 NULL,
																 szHostileGuildID,
																 NULL,
																 &ulDate,
																 NULL,
																 NULL,
																 NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
																 NULL,
																 NULL,
																 NULL
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_RELATION);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
}

BOOL AgsmRelay2::SendGuildHostileDelete(CHAR* szGuildID, CHAR* szHostileGuildID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_DELETE;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,		
																 szGuildID,
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
																 szHostileGuildID,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
																 NULL,
																 NULL,
																 NULL
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_RELATION);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
}





//	Packet send mothods
//========================================
//
BOOL AgsmRelay2::SendGuildMasterInsert(AgpdGuild* pcsGuild)
	{
	if (!pcsGuild || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_INSERT;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,		
																 pcsGuild->m_szID,
																 pcsGuild->m_szMasterID,
																 &pcsGuild->m_lTID,
																 &pcsGuild->m_lRank,
																 &pcsGuild->m_lCreationDate,
																 &pcsGuild->m_lMaxMemberCount,
																 &pcsGuild->m_lUnionID,
																 pcsGuild->m_szPassword,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 &pcsGuild->m_lGuildMarkTID,		// GuildMarkTID
																 &pcsGuild->m_lGuildMarkColor,		// GuildMarkColor
																 NULL,								// money
																 NULL,								// slot
																 NULL								// GuildPoint
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_MASTER);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
	}


BOOL AgsmRelay2::SendGuildMasterUpdate(AgpdGuild* pcsGuild)
	{
	if (!pcsGuild || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	CHAR szBattleStartTime[AGPMGUILD_MAX_DATE_LENGTH + 1];
	memset(szBattleStartTime, 0, sizeof(szBattleStartTime));
	AuTimeStamp::ConvertTimeStampToOracleTime(pcsGuild->m_csCurrentBattleInfo.m_ulStartTime, szBattleStartTime, sizeof(szBattleStartTime));

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,	
																 pcsGuild->m_szID,
																 pcsGuild->m_szMasterID,
																 &pcsGuild->m_lTID,
																 &pcsGuild->m_lRank,
																 &pcsGuild->m_lCreationDate,
																 &pcsGuild->m_lMaxMemberCount,
																 &pcsGuild->m_lUnionID,
																 NULL,
																 &pcsGuild->m_cStatus,
																 &pcsGuild->m_lWin,
																 &pcsGuild->m_lDraw,
																 &pcsGuild->m_lLose,
																 pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
																 szBattleStartTime,
																 &pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
																 &pcsGuild->m_csCurrentBattleInfo.m_lMyScore,
																 &pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore,
																 &pcsGuild->m_lGuildMarkTID,	//	2005.10.20. By SungHoon
																 &pcsGuild->m_lGuildMarkColor,	//	2005.10.20. By SungHoon
																 NULL,						// money
																 NULL,						// slot
																 &pcsGuild->m_lGuildPoint
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_MASTER);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
	}


BOOL AgsmRelay2::SendGuildMasterDelete(CHAR *pszGuildID)
	{
	if (!pszGuildID || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_DELETE;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,
																 pszGuildID,
																 NULL, NULL, NULL, NULL, NULL, NULL,
																 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
																 NULL,		// GuildMarkTID
																 NULL,		// GuildMarkColor
																 NULL,		// money
																 NULL,		// slot
																 NULL		// GuildPoint
																 );
	if(!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_MASTER);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
	}


BOOL AgsmRelay2::SendGuildMasterSelect()
	{
	if (!m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,
																 NULL,
																 NULL, NULL, NULL, NULL, NULL, NULL,
																 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
																 NULL,		// GuildMarkTID
																 NULL,		// GuildMarkColor
																 NULL,		// money
																 NULL,		// slot
																 NULL		// GuildPoint
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_MASTER);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
	}


BOOL AgsmRelay2::SendGuildMasterCheck(AgpdGuild* pcsGuild)
	{
	if (!m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,
																 pcsGuild->m_szID,
																 pcsGuild->m_szMasterID,
																 NULL, NULL, NULL, NULL, NULL, 
																 pcsGuild->m_szPassword,
																 NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
																 NULL,	// GuildMarkTID
																 NULL,	// GuildMarkColor
																 NULL,	// money
																 NULL,	// slot
																 NULL	// GuildPoint
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_MASTER_CHECK);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
	}


BOOL AgsmRelay2::SendGuildMemberInsert(CHAR* pszGuildID, AgpdGuildMember* pcsMember)
	{
	if (!pszGuildID || !pcsMember || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_INSERT;
	PVOID pvPacketGuildMember = m_csPacketGuildMember.MakePacket(FALSE, &nPacketLength, 0,
																  &nOperation,
																  pcsMember->m_szID,
																  pszGuildID,
																  &pcsMember->m_lRank,
																  &pcsMember->m_lJoinDate,
																  &pcsMember->m_lLevel,
																  &pcsMember->m_lTID
																  );
	if (!pvPacketGuildMember)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMember, AGSMRELAY_PARAM_GUILD_MEMBER);
	m_csPacketGuildMember.FreePacket(pvPacketGuildMember);

	return bResult;
	}


BOOL AgsmRelay2::SendGuildMemberUpdate(CHAR* pszGuildID, AgpdGuildMember* pcsMember)
	{
	if (!pszGuildID || !pcsMember || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketGuildMember = m_csPacketGuildMember.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,
																 pcsMember->m_szID,
																 pszGuildID,
																 &pcsMember->m_lRank,
																 &pcsMember->m_lJoinDate,
																 &pcsMember->m_lLevel,
																 &pcsMember->m_lTID
																 );
	if (!pvPacketGuildMember)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMember, AGSMRELAY_PARAM_GUILD_MEMBER);
	m_csPacketGuildMember.FreePacket(pvPacketGuildMember);

	return bResult;
	}


BOOL AgsmRelay2::SendGuildMemberDelete(CHAR *pszGuildID, CHAR *pszCharID)
	{
	if (!pszGuildID || !pszCharID || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_DELETE;
	PVOID pvPacketGuildMember = m_csPacketGuildMember.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,
																 pszCharID,
																 pszGuildID,
																 NULL, NULL, NULL, NULL);
	if (!pvPacketGuildMember)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMember, AGSMRELAY_PARAM_GUILD_MEMBER);
	m_csPacketGuildMember.FreePacket(pvPacketGuildMember);

	return bResult;
	}


BOOL AgsmRelay2::SendGuildMemberSelect()
	{
	if (!m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	PVOID pvPacketGuildMember = m_csPacketGuildMember.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,
																 NULL,
																 NULL, NULL, NULL, NULL, NULL, NULL);
	if (!pvPacketGuildMember)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMember, AGSMRELAY_PARAM_GUILD_MEMBER);
	m_csPacketGuildMember.FreePacket(pvPacketGuildMember);

	return bResult;
	}

BOOL AgsmRelay2::SendGuildMemberLoadingComplete(AgsdDBParam *pAgsdRelay2)
{
	// 다 보냈다는 거 하나 더 보낸다.
	AgsdRelay2GuildMember csGuildMember;
	_tcscpy(csGuildMember.m_szMemberID, AGSMGUILD_LOAD_COMPLETE_STRING);
	ZeroMemory(csGuildMember.m_szGuildID, sizeof(csGuildMember.m_szGuildID));
	csGuildMember.m_lMemberRank = csGuildMember.m_lJoinDate = csGuildMember.m_lTID = csGuildMember.m_lLevel = 0;

	SendSelectResultGuildMember(pAgsdRelay2->m_ulNID, &csGuildMember);
	return TRUE;
}


BOOL AgsmRelay2::SendGuildBattleHistroyInsert(AgpdGuild* pcsGuild)
	{
	if (!pcsGuild || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	CHAR szBattleStartTime[AGPMGUILD_MAX_DATE_LENGTH + 1];
	memset(szBattleStartTime, 0, sizeof(szBattleStartTime));
	AuTimeStamp::ConvertTimeStampToOracleTime(pcsGuild->m_csCurrentBattleInfo.m_ulStartTime, szBattleStartTime, sizeof(szBattleStartTime));

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_INSERT;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,		
																 pcsGuild->m_szID,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 &pcsGuild->m_csCurrentBattleInfo.m_cResult,	// 여기다가 Result 를 보낸다.
																 NULL,
																 NULL,
																 NULL,
																 pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
																 szBattleStartTime,
																 &pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
																 &pcsGuild->m_csCurrentBattleInfo.m_lMyScore,
																 &pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore,
																 NULL,	// GuildMarkTID
																 NULL,	// GuildMarkColor
																 NULL,	// money
																 NULL,	// slot
																 NULL	// GuildPoint
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_BATTLE_HISTORY);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
	}


BOOL AgsmRelay2::SendGuildRename(CHAR *pszOldGuildID, CHAR *pszNewGuildID)
	{
	if (NULL == pszOldGuildID || NULL == pszNewGuildID || NULL == m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,		
																 pszOldGuildID,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,	// 여기다가 Result 를 보낸다.
																 NULL,
																 NULL,
																 NULL,
																 pszNewGuildID,
																 NULL,
																 NULL,
																 NULL,
																 NULL,
																 NULL,	// GuildMarkTID
																 NULL,	// GuildMarkColor
																 NULL,	// money
																 NULL,	// slot
																 NULL
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, AGSMRELAY_PARAM_GUILD_RENAME);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;	
	}


BOOL AgsmRelay2::SendGuildWarehouseMoneyUpdate(AgpdGuildWarehouse *pAgpdGuildWarehouse)
	{
	if (!pAgpdGuildWarehouse || !pAgpdGuildWarehouse->m_pAgpdGuild|| !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketGuild = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,	
																 pAgpdGuildWarehouse->m_pAgpdGuild->m_szID,
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
																 NULL,
																 NULL,
																 NULL,
																 NULL,	//	2005.10.20. By SungHoon
																 NULL,	//	2005.10.20. By SungHoon
																 &pAgpdGuildWarehouse->m_llMoney,	// money
																 &pAgpdGuildWarehouse->m_lSlot,		// slot
																 NULL								// GuildPoint
																 );
	if (!pvPacketGuild)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuild, AGSMRELAY_PARAM_GUILD_WAREHOUSE_MONEY);
	m_csPacketGuildMaster.FreePacket(pvPacketGuild);

	return bResult;
	}


BOOL AgsmRelay2::SendGuildItem(AgsdRelay2GuildItem *pAgsdRelay2, UINT32 ulNID)
	{
	INT16 nPacketLength	= 0;
	PVOID pvPacketEmb= m_csPacketGuildItem.MakePacket(FALSE, &nPacketLength, 0,
													  &pAgsdRelay2->m_eOperation,
													  &pAgsdRelay2->m_lCID,
													  pAgsdRelay2->m_szGuildID,
													  &pAgsdRelay2->m_lInd,
													  pAgsdRelay2->m_pvPacketEmb
													  );
	if (!pvPacketEmb)
		return FALSE;

	BOOL bResult = FALSE;

	if (0 == ulNID)
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_GUILD_WAREHOUSE_ITEM);
	else
		bResult = MakeAndSendRelayPacket(pvPacketEmb, AGSMRELAY_PARAM_GUILD_WAREHOUSE_ITEM, ulNID);

	m_csPacketGuildItem.FreePacket(pvPacketEmb);
	
	return bResult;	
	}




//	Callbacks
//===============================================
//
BOOL AgsmRelay2::CBGuildMasterInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgpdGuild *pcsGuild = (AgpdGuild *)pData;
	AgsmRelay2 *pThis = (AgsmRelay2 *)pClass;

	if (!pcsGuild || !pThis)
		return FALSE;

	return pThis->SendGuildMasterInsert(pcsGuild);
	}


BOOL AgsmRelay2::CBGuildMasterUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgpdGuild *pcsGuild = (AgpdGuild *)pData;
	AgsmRelay2 *pThis = (AgsmRelay2 *)pClass;

	if(!pcsGuild || !pThis)
		return FALSE;

	return pThis->SendGuildMasterUpdate(pcsGuild);
	}


BOOL AgsmRelay2::CBGuildMasterDelete(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	CHAR *pszGuildID = (CHAR *) pData;
	AgsmRelay2 *pThis = (AgsmRelay2 *)pClass;

	if (!pszGuildID || !pThis)
		return FALSE;

	return pThis->SendGuildMasterDelete(pszGuildID);
	}


BOOL AgsmRelay2::CBGuildMasterSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;

	if (!pThis)
		return FALSE;

	return pThis->SendGuildMasterSelect();
	}


BOOL AgsmRelay2::CBGuildMasterCheck(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgpdGuild *pcsGuild = (AgpdGuild *)pData;
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;

	if (!pThis)
		return FALSE;
	
	return pThis->SendGuildMasterCheck(pcsGuild);
	}


BOOL AgsmRelay2::CBGuildMemberInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	CHAR *pszGuildID = (CHAR *) pData;
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdGuildMember *pcsMember = (AgpdGuildMember *) pCustData;

	if (!pszGuildID || !pThis || !pcsMember)
		return FALSE;

	return pThis->SendGuildMemberInsert(pszGuildID, pcsMember);
	}


BOOL AgsmRelay2::CBGuildMemberUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	CHAR *pszGuildID = (CHAR *) pData;
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgpdGuildMember *pcsMember = (AgpdGuildMember *) pCustData;

	if (!pszGuildID || !pThis || !pcsMember)
		return FALSE;

	return pThis->SendGuildMemberUpdate(pszGuildID, pcsMember);
	}


BOOL AgsmRelay2::CBGuildMemberDelete(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	CHAR *pszGuildID = (CHAR *) pData;
	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	CHAR *pszCharID = (CHAR *) pCustData;

	if (!pszGuildID || !pThis || !pszCharID)
		return FALSE;

	return pThis->SendGuildMemberDelete(pszGuildID, pszCharID);
	}


BOOL AgsmRelay2::CBGuildMemberSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *)pClass;

	if (!pThis)
		return FALSE;

	return pThis->SendGuildMemberSelect();
	}


BOOL AgsmRelay2::CBGuildBattleHistroyInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgpdGuild *pcsGuild = (AgpdGuild *)pData;
	AgsmRelay2 *pThis = (AgsmRelay2 *)pClass;

	if (!pcsGuild || !pThis)
		return FALSE;

	return pThis->SendGuildBattleHistroyInsert(pcsGuild);
	}


BOOL AgsmRelay2::CBGuildRename(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgsmRelay2 *pThis = (AgsmRelay2 *)pClass;
	CHAR *pszOldGuildID = (CHAR *) pData;	
	CHAR *pszNewGuildID = (CHAR *) pCustData;	

	if (NULL == pThis)
		return FALSE;

	return pThis->SendGuildRename(pszOldGuildID, pszNewGuildID);
	}


BOOL AgsmRelay2::CBGuildWarehouseMoneyUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmRelay2 *pThis = (AgsmRelay2 *)pClass;
	AgpdGuildWarehouse *pAgpdGuildWarehouse = (AgpdGuildWarehouse *)pData;

	return pThis->SendGuildWarehouseMoneyUpdate(pAgpdGuildWarehouse);
	}


BOOL AgsmRelay2::CBGuildItemSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmRelay2 *pThis = (AgsmRelay2 *)pClass;
	AgpdGuildWarehouse *pAgpdGuildWarehouse = (AgpdGuildWarehouse *)pData;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;

	AgsdRelay2GuildItem csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_SELECT;
	csRelay2.m_lCID = pAgpdCharacter->m_lID;
	ZeroMemory(csRelay2.m_szGuildID, _MAX_GUILDID_LENGTH+1);
	_tcsncpy(csRelay2.m_szGuildID, pAgpdGuildWarehouse->m_pAgpdGuild->m_szID, _MAX_GUILDID_LENGTH);

	return pThis->SendGuildItem(&csRelay2);
	}


BOOL AgsmRelay2::CBGuildItemInsert(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmRelay2			*pThis = (AgsmRelay2 *)pClass;
	AgpdGuildWarehouse	*pAgpdGuildWarehouse = (AgpdGuildWarehouse *)pData;
	AgpdItem			*pAgpdItem = (AgpdItem *) pCustData;
	AgsdItem			*pAgsdItem = pThis->m_pAgsmItem->GetADItem(pAgpdItem);
	if (!pAgsdItem)
		return FALSE;

	AgsdRelay2GuildItem csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_INSERT;
	ZeroMemory(csRelay2.m_szGuildID, _MAX_GUILDID_LENGTH+1);
	_tcsncpy(csRelay2.m_szGuildID, pAgpdGuildWarehouse->m_pAgpdGuild->m_szID, _MAX_GUILDID_LENGTH);

	// convert history
	// delete reason에 끼워 보낸다.
	CHAR	szConvertHistory[_MAX_CONVERT_HISTORY_LENGTH + 1];
	ZeroMemory(szConvertHistory, sizeof(szConvertHistory));
	pThis->m_pAgsmItemConvert->EncodeConvertHistory(pAgpdItem, szConvertHistory, _MAX_CONVERT_HISTORY_LENGTH);
	INT16	nConvertHistoryLength = (INT16)_tcslen(szConvertHistory);
	
	INT16 nOperation = AGSMDATABASE_OPERATION_INSERT;		
	INT16 nPacketLength = 0;
	PVOID pvPacketItem = pThis->MakeItemPacket(pAgpdItem, pAgsdItem->m_ullDBIID, nOperation, &nPacketLength, csRelay2.m_szGuildID, (PVOID) szConvertHistory, &nConvertHistoryLength);
	if (!pvPacketItem)
		return FALSE;
	
	csRelay2.m_pvPacketEmb = pvPacketItem;	
	BOOL bResult = pThis->SendGuildItem(&csRelay2);
	pThis->m_csPacketItem.FreePacket(pvPacketItem);
	
	return bResult;
	}


BOOL AgsmRelay2::CBGuildItemDelete(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmRelay2 *pThis = (AgsmRelay2 *)pClass;
	AgpdGuildWarehouse *pAgpdGuildWarehouse = (AgpdGuildWarehouse *)pData;
	AgpdItem *pAgpdItem = (AgpdItem *) pCustData;
	AgsdItem *pAgsdItem = pThis->m_pAgsmItem->GetADItem(pAgpdItem);
	if (!pAgsdItem)
		return FALSE;

	AgsdRelay2GuildItem csRelay2;
	csRelay2.m_eOperation = AGSMDATABASE_OPERATION_DELETE;
	ZeroMemory(csRelay2.m_szGuildID, _MAX_GUILDID_LENGTH+1);
	_tcsncpy(csRelay2.m_szGuildID, pAgpdGuildWarehouse->m_pAgpdGuild->m_szID, _MAX_GUILDID_LENGTH);

	// item packet
	INT16 nOperation = AGSMDATABASE_OPERATION_DELETE;
	INT16 nPacketLength = 0;
	PVOID pvPacketItem = pThis->m_csPacketItem.MakePacket(FALSE, &nPacketLength, 0,
													&nOperation,
													NULL,
													NULL,
													NULL,
													&pAgsdItem->m_ullDBIID,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,		// JNY TODO 2004.3.3 : Need level
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
													NULL
													);	
	csRelay2.m_pvPacketEmb = pvPacketItem;	
	BOOL bResult = pThis->SendGuildItem(&csRelay2);
	pThis->m_csPacketItem.FreePacket(pvPacketItem);

	return bResult;
	}


BOOL AgsmRelay2::CBGuildJointSelect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass)
		return FALSE;

	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);

	pThis->SendGuildJointSelect(NULL, NULL, 0, 0);
	return TRUE;
}

BOOL AgsmRelay2::CBGuildJointInsert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdGuild* pcsGuild = static_cast<AgpdGuild*>(pData);
	CHAR* szGuildID = static_cast<CHAR*>(pCustData);

	AgpdGuildRelationUnit* pUnit = pThis->m_pAgpmGuild->GetJointGuild(pcsGuild, szGuildID);
	if(!pUnit)
		return FALSE;

	//BOOL bIsLeader = pThis->m_pAgpmGuild->IsJointLeader(pcsGuild);

	pThis->SendGuildJointInsert(pcsGuild->m_szID, szGuildID, pUnit->m_ulDate, pUnit->m_cRelation);
	//pThis->SendGuildJointInsert(szGuildID, pcsGuild->m_szID, pUnit->m_ulDate,
	//								bIsLeader ? AGPMGUILD_RELATION_JOINT_LEADER : AGPMGUILD_RELATION_JOINT);
	return TRUE;
}

BOOL AgsmRelay2::CBGuildJointUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdGuild* pcsGuild = static_cast<AgpdGuild*>(pData);
	CHAR* szGuildID = static_cast<CHAR*>(pCustData);

	AgpdGuildRelationUnit* pUnit = pThis->m_pAgpmGuild->GetJointGuild(pcsGuild, szGuildID);
	if(!pUnit)
		return FALSE;

	return pThis->SendGuildJointUpdate(pcsGuild->m_szID, szGuildID, pUnit->m_cRelation);
}

BOOL AgsmRelay2::CBGuildJointDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdGuild* pcsGuild = static_cast<AgpdGuild*>(pData);
	CHAR* szGuildID = static_cast<CHAR*>(pCustData);

	pThis->SendGuildJointDelete(pcsGuild->m_szID, szGuildID);
	pThis->SendGuildJointDelete(szGuildID, pcsGuild->m_szID);
	return TRUE;
}

BOOL AgsmRelay2::CBGuildHostileInsert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdGuild* pcsGuild = static_cast<AgpdGuild*>(pData);
	CHAR* szGuildID = static_cast<CHAR*>(pCustData);

	AgpdGuildRelationUnit* pUnit = pThis->m_pAgpmGuild->GetHostileGuild(pcsGuild, szGuildID);
	if(!pUnit)
		return FALSE;

	pThis->SendGuildHostileInsert(pcsGuild->m_szID, szGuildID, pUnit->m_ulDate);
	pThis->SendGuildHostileInsert(szGuildID, pcsGuild->m_szID, pUnit->m_ulDate);
	return TRUE;
}

BOOL AgsmRelay2::CBGuildHostileDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRelay2* pThis = static_cast<AgsmRelay2*>(pClass);
	AgpdGuild* pcsGuild = static_cast<AgpdGuild*>(pData);
	CHAR* szGuildID = static_cast<CHAR*>(pCustData);

	pThis->SendGuildHostileDelete(pcsGuild->m_szID, szGuildID);
	pThis->SendGuildHostileDelete(szGuildID, pcsGuild->m_szID);
	return TRUE;
}

BOOL AgsmRelay2::SendSelectResultGuildMaster(UINT32 ulNID, AgsdRelay2GuildMaster *pcsGuildMaster, BOOL bCheck)
	{
	if (!ulNID || !pcsGuildMaster)
		return FALSE;

	AgsdServer* pcsGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(ulNID);
	if (!pcsGameServer)
		return FALSE;

	UINT32 ulNewNID = pcsGameServer->m_dpnidServer;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	PVOID pvPacketGuildMaster = m_csPacketGuildMaster.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,
																 pcsGuildMaster->m_szGuildID,
																 pcsGuildMaster->m_szMasterID,
																 &pcsGuildMaster->m_lGuildTID,
																 &pcsGuildMaster->m_lGuildRank,
																 &pcsGuildMaster->m_lCreationDate,
																 &pcsGuildMaster->m_lMaxMemberCount,
																 &pcsGuildMaster->m_lUnionID,
																 pcsGuildMaster->m_szPassword,
																 &pcsGuildMaster->m_cStatus,
																 &pcsGuildMaster->m_lWin,
																 &pcsGuildMaster->m_lDraw,
																 &pcsGuildMaster->m_lLose,
																 pcsGuildMaster->m_szEnemyGuildID,
																 pcsGuildMaster->m_szBattleStartTime,
																 &pcsGuildMaster->m_ulBattleDuration,
																 &pcsGuildMaster->m_lMyScore,
																 &pcsGuildMaster->m_lEnemyScore,
																 &pcsGuildMaster->m_lGuildMarkTID,		//	2005.10.20. By SungHoon
																 &pcsGuildMaster->m_lGuildMarkColor,	//	2005.10.20. By SungHoon
																 &pcsGuildMaster->m_llMoney,
																 &pcsGuildMaster->m_lSlot,
																 &pcsGuildMaster->m_lGuildPoint
																 );
	if (!pvPacketGuildMaster)
		return FALSE;

	INT16 nParam = bCheck ? AGSMRELAY_PARAM_GUILD_MASTER_CHECK : AGSMRELAY_PARAM_GUILD_MASTER;
	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMaster, nParam, ulNewNID);
	m_csPacketGuildMaster.FreePacket(pvPacketGuildMaster);

	return bResult;
	}


BOOL AgsmRelay2::SendSelectResultGuildMember(UINT32 ulNID, AgsdRelay2GuildMember *pcsGuildMember)
	{
	if (!ulNID || !pcsGuildMember)
		return FALSE;

	AgsdServer2* pcsGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(ulNID);
	if(!pcsGameServer)
		return FALSE;

	INT32 ulNewNID = pcsGameServer->m_dpnidServer;

	INT16 nPacketLength = 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_SELECT;
	PVOID pvPacketGuildMember = m_csPacketGuildMember.MakePacket(FALSE, &nPacketLength, 0,
																 &nOperation,
																 pcsGuildMember->m_szMemberID,
																 pcsGuildMember->m_szGuildID,
																 &pcsGuildMember->m_lMemberRank,
																 &pcsGuildMember->m_lJoinDate,
																 &pcsGuildMember->m_lLevel,
																 &pcsGuildMember->m_lTID
																 );
	if (!pvPacketGuildMember)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketGuildMember, AGSMRELAY_PARAM_GUILD_MEMBER, ulNewNID);
	m_csPacketGuildMember.FreePacket(pvPacketGuildMember);

	return bResult;
	}

BOOL AgsmRelay2::CBOperationResultGuildRelation(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2GuildMaster *pAgsdRelay2 = (AgsdRelay2GuildMaster *) pData;

	AgsdRelay2AutoPtr Auto(pAgsdRelay2);

	if (AGSMDATABASE_OPERATION_SELECT != pAgsdRelay2->m_eOperation)
		return FALSE;

	return pThis->m_pAgsmGuild->RelationLoadFromDB(pAgsdRelay2->m_szGuildID, pAgsdRelay2->m_szEnemyGuildID, pAgsdRelay2->m_cStatus, pAgsdRelay2->m_ulBattleDuration);
	}




/****************************************************************/
/*		The Implementation of AgsdRelay2GuildMaster class		*/
/****************************************************************/
//
AgsdRelay2GuildMaster::AgsdRelay2GuildMaster()
	{
	ZeroMemory(m_szGuildID, sizeof(m_szGuildID));
	ZeroMemory(m_szMasterID, sizeof(m_szMasterID));
	m_lGuildTID = 0;
	m_lGuildRank = 0;
	m_lCreationDate = 0;
	m_lMaxMemberCount = 0;
	m_lUnionID = 0;
	ZeroMemory(m_szPassword, sizeof(m_szPassword));
	m_bCheckSelect = FALSE;
	m_bBattleHistoryInsert = FALSE;
	m_llMoney = 0;
	ZeroMemory(m_szMoney, sizeof(m_szMoney));
	m_lSlot = 0;
	}


BOOL AgsdRelay2GuildMaster::SetParamSelect(AuStatement* pStatement)
	{
	if (m_bCheckSelect)
		pStatement->SetParam(0, m_szGuildID, sizeof(m_szGuildID));
	
	return TRUE;
	}


BOOL AgsdRelay2GuildMaster::SetParamUpdate(AuStatement* pStatement)
	{
	INT16 i = 0;
	
	switch (m_nParam)
		{
		case AGSMRELAY_PARAM_GUILD_MASTER:
			{
			pStatement->SetParam(i++, m_szMasterID, sizeof(m_szMasterID));
			pStatement->SetParam(i++, &m_lMaxMemberCount);
			pStatement->SetParam(i++, &m_lGuildRank);

			pStatement->SetParam(i++, &m_lWin);
			pStatement->SetParam(i++, &m_lDraw);
			pStatement->SetParam(i++, &m_lLose);
			pStatement->SetParam(i++, m_szEnemyGuildID, sizeof(m_szEnemyGuildID));
			pStatement->SetParam(i++, m_szBattleStartTime, sizeof(m_szBattleStartTime));
			pStatement->SetParam(i++, &m_ulBattleDuration);
			pStatement->SetParam(i++, &m_lMyScore);
			pStatement->SetParam(i++, &m_lGuildMarkTID);
			pStatement->SetParam(i++, &m_lGuildMarkColor);
			pStatement->SetParam(i++, &m_lGuildPoint);

			pStatement->SetParam(i++, m_szGuildID, sizeof(m_szGuildID));
			}
			break;
		case AGSMRELAY_PARAM_GUILD_RENAME:
			{
			pStatement->SetParam(i++, m_szEnemyGuildID, sizeof(m_szEnemyGuildID));
			pStatement->SetParam(i++, m_szGuildID, sizeof(m_szGuildID));
			}
			break;
		case AGSMRELAY_PARAM_GUILD_WAREHOUSE_MONEY :
			{
			pStatement->SetParam(i++, m_szMoney, sizeof(m_szMoney));
			pStatement->SetParam(i++, &m_lSlot);
			pStatement->SetParam(i++, m_szGuildID, sizeof(m_szGuildID));
			}
			break;
		case AGSMRELAY_PARAM_GUILD_RELATION:
			{
			pStatement->SetParam(i++, &m_cStatus);
			pStatement->SetParam(i++, m_szGuildID, sizeof(m_szGuildID));
			pStatement->SetParam(i++, m_szEnemyGuildID, sizeof(m_szEnemyGuildID));
			}
			break;
		}

	return TRUE;
	}


BOOL AgsdRelay2GuildMaster::SetParamInsert(AuStatement* pStatement)
	{
	INT16 i = 0;

	if (AGSMRELAY_PARAM_GUILD_RELATION == m_nParam)
		{
		pStatement->SetParam(i++, m_szGuildID, sizeof(m_szGuildID));
		pStatement->SetParam(i++, m_szEnemyGuildID, sizeof(m_szEnemyGuildID));
		pStatement->SetParam(i++, &m_cStatus);
		pStatement->SetParam(i++, &m_ulBattleDuration);
		return TRUE;
		}
	
	if(!m_bBattleHistoryInsert)
		{
		pStatement->SetParam(i++, m_szGuildID, sizeof(m_szGuildID));
		pStatement->SetParam(i++, &m_lGuildTID);
		pStatement->SetParam(i++, m_szMasterID, sizeof(m_szMasterID));
		pStatement->SetParam(i++, &m_lMaxMemberCount);
		//pStatement->SetParam(i++, &m_lCreationDate);
		pStatement->SetParam(i++, &m_lUnionID);
		pStatement->SetParam(i++, &m_lGuildRank);
		pStatement->SetParam(i++, m_szPassword, sizeof(m_szPassword));

		pStatement->SetParam(i++, &m_lGuildMarkTID);		//	2005.10.20. By SungHoon
		pStatement->SetParam(i++, &m_lGuildMarkColor);		//	2005.10.20. By SungHoon
		}
	else
		{
		pStatement->SetParam(i++, m_szGuildID, sizeof(m_szGuildID));
		pStatement->SetParam(i++, m_szEnemyGuildID, sizeof(m_szEnemyGuildID));
		pStatement->SetParam(i++, &m_cStatus);
		pStatement->SetParam(i++, m_szBattleStartTime, sizeof(m_szBattleStartTime));
		pStatement->SetParam(i++, &m_ulBattleDuration);
		pStatement->SetParam(i++, &m_lMyScore);
		pStatement->SetParam(i++, &m_lEnemyScore);
		}

	return TRUE;
	}


BOOL AgsdRelay2GuildMaster::SetParamDelete(AuStatement* pStatement)
	{
	switch (m_nParam)
		{
		case AGSMRELAY_PARAM_GUILD_MASTER:
			{
			pStatement->SetParam(0, m_szGuildID, sizeof(m_szGuildID));
			}
			break;
		case AGSMRELAY_PARAM_GUILD_RELATION:
			{
			pStatement->SetParam(0, m_szGuildID, sizeof(m_szGuildID));
			pStatement->SetParam(1, m_szEnemyGuildID, sizeof(m_szEnemyGuildID));
			}
			break;
		}
	
	return TRUE;
	}


void AgsdRelay2GuildMaster::Dump(CHAR *pszOp)
	{
	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "LOG\\%04d%02d%02d-%02d-RELAY_DUMP_GUILD.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d:%02d:%02d][%s] "
						"P[MASTER] ID[%s] TID[%d] Master[%s] Max[%d] Union[%d] Rank[%d] Pwd[%s]\n",
						st.wHour, st.wMinute, st.wSecond, pszOp,
						m_szGuildID, m_lGuildTID, m_szMasterID, m_lMaxMemberCount,
						m_lUnionID, m_lGuildRank, m_szPassword);
	AuLogFile_s(szFile, strCharBuff);
	}




/****************************************************************/
/*		The Implementation of AgsdRelay2GuildMember class		*/
/****************************************************************/
//
AgsdRelay2GuildMember::AgsdRelay2GuildMember()
	{
	ZeroMemory(m_szMemberID, sizeof(m_szMemberID));
	ZeroMemory(m_szGuildID, sizeof(m_szGuildID));
	ZeroMemory(m_szJoinDate, sizeof(m_szJoinDate));		//	2005.07.28. SungHoon
	m_lMemberRank = 0;
	m_lJoinDate = 0;
	m_lLevel = 0;
	m_lTID = 0;
	}


BOOL AgsdRelay2GuildMember::SetParamUpdate(AuStatement* pStatement)
	{
	pStatement->SetParam(0, (INT16 *) &m_lMemberRank);
	pStatement->SetParam(1, m_szJoinDate, sizeof(m_szJoinDate));		//	2005.07.28. SungHoon
	pStatement->SetParam(2, m_szMemberID, sizeof(m_szMemberID));		//	2005.07.28. SungHoon
	return TRUE;
	}


BOOL AgsdRelay2GuildMember::SetParamInsert(AuStatement* pStatement)
	{
	pStatement->SetParam(0, m_szMemberID, sizeof(m_szMemberID));
	pStatement->SetParam(1, m_szGuildID, sizeof(m_szGuildID));
	pStatement->SetParam(2, (INT16 *) &m_lMemberRank);
	//pStatement->SetParam(3, &m_lJoinDate);
	return TRUE;
	}


BOOL AgsdRelay2GuildMember::SetParamDelete(AuStatement* pStatement)
	{
	pStatement->SetParam(0, m_szMemberID, sizeof(m_szMemberID));
	return TRUE;
	}


void AgsdRelay2GuildMember::Dump(CHAR *pszOp)
	{
	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "LOG\\%04d%02d%02d-%02-RELAY_DUMP_GUILD.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d:%02d:%02d][%s] "
						"P[MEMBER] ID[%s] Guild[%s] Rank[%d]\n",
						st.wHour, st.wMinute, st.wSecond, pszOp,
						m_szMemberID, m_szGuildID, m_lMemberRank);
	AuLogFile_s(szFile, strCharBuff);
	}




/************************************************************/
/*		The Implementation of AgsdRelay2GuildItem class		*/
/************************************************************/
//
AgsdRelay2GuildItem::AgsdRelay2GuildItem()
	{
	ZeroMemory(m_szGuildID, sizeof(m_szGuildID));
	m_lCID = AP_INVALID_CID;
	m_pvPacketEmb = NULL;
	m_lInd = 0;
	}


BOOL AgsdRelay2GuildItem::SetParamSelect(AuStatement *pStatement)
	{
	pStatement->SetParam(0, m_szGuildID, sizeof(m_szGuildID));
	return TRUE;
	}

//////////////////////////////////////////////////////////////////////////
// World Championship - arycoat 2008.8
//
BOOL AgsmRelay2::OnParamWorldChampionship(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP* pPacket = (PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP*)pvPacket;

	switch(pPacket->nOperation)
	{
		case AGSMRELAY_PARAM_WORLD_CHAMPIONSHIP_REQUEST: // Game -> Relay
			{
				PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_REQUEST* pPacket2 = (PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_REQUEST*)pPacket;
				
				AgsdRelay2WorldChampionship* pcsRelayWorldChampionship = new AgsdRelay2WorldChampionship(ulNID, pPacket->nOperation);
				strcpy(pcsRelayWorldChampionship->strGuildName, pPacket2->strGuildName);
				strcpy(pcsRelayWorldChampionship->strCharName, pPacket2->strCharName);

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_WORLD_CHAMPIONSHIP_REQUEST;
				pQuery->m_pParam = pcsRelayWorldChampionship;
				pQuery->SetCallback(AgsmRelay2::CBOperationResultWorldChampionshipResult, AgsmRelay2::CBOperationResultWorldChampionshipResult, this, pcsRelayWorldChampionship);

				m_pAgsmDatabasePool->Execute(pQuery);
			} break;
		
		case AGSMRELAY_PARAM_WORLD_CHAMPIONSHIP_ENTER: // Game -> Relay
			{
				PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_ENTER* pPacket2 = (PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_ENTER*)pPacket;

				AgsdRelay2WorldChampionship* pcsRelayWorldChampionship = new AgsdRelay2WorldChampionship(ulNID, pPacket->nOperation);
				strcpy(pcsRelayWorldChampionship->strCharName, pPacket2->strCharName);
				strcpy(pcsRelayWorldChampionship->m_szWorld, pPacket2->m_szWorld);
				pcsRelayWorldChampionship->m_lServerID = pPacket2->m_lServerID;

				AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
				pQuery->m_nIndex = AGSMRELAY_QUERY_WORLD_CHAMPIONSHIP_ENTER;
				pQuery->m_pParam = pcsRelayWorldChampionship;
				pQuery->SetCallback(AgsmRelay2::CBOperationResultWorldChampionshipResult, AgsmRelay2::CBOperationResultWorldChampionshipResult, this, pcsRelayWorldChampionship);

				m_pAgsmDatabasePool->Execute(pQuery);
			} break;
		
		case AGSMRELAY_PARAM_WORLD_CHAMPIONSHIP_REQUEST_RESULT: // Relay -> Game 
			{
				PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_RESULT* pPacket2 = (PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_RESULT*)pPacket;
				
				AgpdCharacter* pcsCharacter = m_pAgpmCharacter->GetCharacter(pPacket2->strCharName);
				if(!pcsCharacter)
					return FALSE;

				AgsdCharacter* pagsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);
				if(!pagsdCharacter)
					return FALSE;

				AuXmlElement* pElem = NULL;

				PACKET_GUILD_WORLDCHAMPIONSHIP_RESULT pPacket3;

				switch(pPacket2->m_nCode)
				{
					case 0:
						{
							pElem = m_pAgsmGuild->GetNoticeNodeWorldChampionship()->FirstChildElement("Notice_01");
						} break;
					case 19:
						{
							pElem = m_pAgsmGuild->GetNoticeNodeWorldChampionship()->FirstChildElement("Notice_04");
						} break;
					case 13:
						{
							pElem = m_pAgsmGuild->GetNoticeNodeWorldChampionship()->FirstChildElement("Notice_05");
						} break;
					case 3: // 길드참가 중복신청
						{
							pElem = m_pAgsmGuild->GetNoticeNodeWorldChampionship()->FirstChildElement("Notice_06");
						} break;
					default:
						{
							pElem = m_pAgsmGuild->GetNoticeNodeWorldChampionship()->FirstChildElement("Notice_02");
						} break;
				}

				if(pElem)
					strcpy(pPacket3.strMessage, pElem->GetText());

				AgsEngine::GetInstance()->SendPacket(pPacket3, pagsdCharacter->m_dpnidCharacter);

			} break;
		
		default:
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultWorldChampionshipResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2WorldChampionship *pAgsdRelay2 = (AgsdRelay2WorldChampionship *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_RESULT pPacket(pAgsdRelay2->strCharName, pAgsdRelay2->m_nCode);

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}
