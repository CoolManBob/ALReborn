/*===============================================================

	AgsmRelay2Test.cpp

===============================================================*/

#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"

//	TEST
//=====================================================================
//
#ifdef _AGSMRELAY2_TEST

BOOL AgsmRelay2::TestSendCharacterUpdate()
	{
	// Relay Server가 연결이 안되어 있으면 종료
	if (NULL == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	// get values
	INT16	nPacketLength;
	INT32	lMurdererPoint = 0;
	INT32	lHP = 99;
	INT32	lMP = 99;
	INT32	lSP = 99;
	INT32	lEXP = 99;
	INT32	lLevel = 9;
	INT32	lSkillPoint = 9;
	UINT32	ulCriminalRemainTime = 0;
	UINT32	ulMurderRemainTime = 0;
	CHAR	szPosition[33] = "";
	CHAR	szID[_MAX_CHARNAME_LENGTH + 1] = "RTEST";
	CHAR	szSkillInit[17] = "";
	INT16	nCriminalStatus = 0;
	INT64	llMoney = 999;

	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketCharacter = m_csPacketCharacter.MakePacket(FALSE, &nPacketLength, 0,
												&nOperation,
												szID,
												szTitle,
												szPosition,
												&nCriminalStatus,
												&lMurdererPoint,
												&llMoney,
												&ulCriminalRemainTime,
												&ulMurderRemainTime,
												&lHP,
												&lMP,
												&lSP,
												&lEXP,
												&lLevel,
												&lSkillPoint
												);
	if (!pvPacketCharacter)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_PARAM_CHARACTER, 0,
									 pvPacketCharacter);
	m_csPacketCharacter.FreePacket(pvPacketCharacter);

	if (!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);
	return bResult;
	}

BOOL AgsmRelay2::TestSendItemInsert(UINT64 ullID)
	{
	INT16	nPacketLength = 0;
	INT32	lNeedLevel	= 9;
	INT32	lDurability	= 99;
	CHAR	szPosition[33] ="POSITION";
	CHAR	szID[_MAX_CHARNAME_LENGTH + 1] = "RTEST";
	INT32	lTID = 9;
	INT32	nCount = 3;
	INT16	eStatus = 2;
	UINT64	ullDBID = ullID ? ullID : 43;

	INT16 nOperation = AGSMDATABASE_OPERATION_INSERT;
	PVOID pvPacketItem = m_csPacketItem.MakePacket(FALSE, &nPacketLength, 0,
													&nOperation,	
													szID,
													&lTID,
													&nCount,
													&eStatus,
													szPosition,
													&lNeedLevel,		// JNY TODO 2004.3.3 : Need level
													&lDurability,
													NULL,
													&ullDBID
													);

	if (!pvPacketItem)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_PARAM_ITEM, 0, 
									 pvPacketItem);
	m_csPacketItem.FreePacket(pvPacketItem);

	if (!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);
	return bResult;
	}

BOOL AgsmRelay2::TestSendItemUpdate()
	{
	if (!m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength;
	INT32	lNeedLevel	= 9;
	INT32	lDurability	= 99;
	CHAR	szPosition[33] = "POSITION";
	CHAR	szID[_MAX_CHARNAME_LENGTH + 1] = "RTEST";
	INT32	lTID = 9;
	INT32	nCount = 3;
	INT16	eStatus = 2;
	UINT64	ullDBID = 43;

	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketItem = m_csPacketItem.MakePacket(FALSE, &nPacketLength, 0,
													&nOperation,
													szID,
													NULL,
													&nCount,
													&eStatus,
													szPosition,
													&lNeedLevel,		// JNY TODO 2004.3.3 : Need level
													&lDurability,
													NULL,
													&ullDBID
													);

	if (!pvPacketItem) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_PARAM_ITEM, 0, 
									 pvPacketItem);
	m_csPacketItem.FreePacket(pvPacketItem);

	if (!pvPacket) return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);
	return bResult;
	}

BOOL AgsmRelay2::TestSendItemDelete(UINT64 ullID)
	{
	UINT64	ullDBID = ullID ? ullID : 43;

	return SendItemDelete(ullDBID, "RTEST");
	}

BOOL AgsmRelay2::TestSendItemConvertUpdate()
	{
	if (!m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	CHAR	szConvertHistory[129] = "HISTORY";
	INT16	nConvertHistoryLength	= strlen(szConvertHistory);
	INT16	nPacketLength	= 0;
	UINT64	ullDBID = 43;

	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID	pvPacketItemConvertUpdate	= m_csPacketItemConvert.MakePacket(FALSE, &nPacketLength, 0,
																	&nOperation,
																	&ullDBID,
																	szConvertHistory,
																	&nConvertHistoryLength
																	);

	if (!pvPacketItemConvertUpdate)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_PARAM_ITEM_CONVERT_HISTORY, 0, 
									 pvPacketItemConvertUpdate
									 );

	m_csPacketItemConvert.FreePacket(pvPacketItemConvertUpdate);

	if (!pvPacket) return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);
	return bResult;
	}

BOOL AgsmRelay2::TestSendSkillUpdate()
	{
	if (!m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16	nPacketLength	= 0;
	CHAR	szTreeBuffer[129] = "TREENODE";
	CHAR	szSpecializeBuffer[33] = "SPECIAL";
	CHAR	szID[_MAX_CHARNAME_LENGTH + 1] = "RTEST";
	
	INT32	lMasteryIndex = 2;
	INT32	lTotalSP	= 9;

	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;

	PVOID	pvPacketSkill	= m_csPacketSkill.MakePacket(FALSE, &nPacketLength, 0,
														&nOperation,
														szID,
														&lMasteryIndex,
														&lTotalSP,
														szTreeBuffer,
														szSpecializeBuffer
														);

	if (!pvPacketSkill)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_PARAM_SKILL, 0, 
									 pvPacketSkill
									 );
	m_csPacketSkill.FreePacket(pvPacketSkill);

	if (!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);
	return bResult; 
	}

BOOL AgsmRelay2::TestSendUIStatusUpdate()
	{
	if (!m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	CHAR	szID[_MAX_CHARNAME_LENGTH + 1] = "RTEST";
	CHAR	szStringBuffer[_MAX_QUICKBELT_LENGTH + 1] = "QUICKBELT";
	INT16	nStringLength	= strlen(szStringBuffer);
	if (nStringLength > _MAX_QUICKBELT_LENGTH)
		return FALSE;

	INT16	nOperation = AGSMDATABASE_OPERATION_UPDATE;
	
	INT16	nPacketLength = 0;
	PVOID	pvPacketUIStatus = m_csPacketUIStatus.MakePacket(FALSE, &nPacketLength, 0,
															 &nOperation,
															 szID,
															 szStringBuffer,
															 &nStringLength
															 );

	if (!pvPacketUIStatus || nPacketLength < 1)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_PARAM_UI_STATUS, 0, 
									 pvPacketUIStatus
									 );
	m_csPacketUIStatus.FreePacket(pvPacketUIStatus);

	if (!pvPacket) return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);
	return bResult;
	}

BOOL AgsmRelay2::TestSendGuildMasterInsert()
	{
	AgpdGuild csGuild;
	strcpy(csGuild.m_szID,"롹매니아");
	strcpy(csGuild.m_szMasterID, "RTEST");
	csGuild.m_lTID = 9;
	csGuild.m_lRank = 0;
	csGuild.m_lCreationDate = 1;
	csGuild.m_lMaxMemberCount = 1;
	csGuild.m_lUnionID = 0;
	strcpy(csGuild.m_szPassword , "abcd");

	return SendGuildMasterInsert(&csGuild);
	}

BOOL AgsmRelay2::TestSendGuildMasterUpdate()
	{
	AgpdGuild csGuild;
	strcpy(csGuild.m_szID , "롹매니아");
	strcpy(csGuild.m_szMasterID , "RTEST");
	csGuild.m_lTID = 9;
	csGuild.m_lRank = 0;
	csGuild.m_lCreationDate = 2;
	csGuild.m_lMaxMemberCount = 2;
	csGuild.m_lUnionID = 0;
	strcpy(csGuild.m_szPassword , "abcd");
	return SendGuildMasterUpdate(&csGuild);
	}

BOOL AgsmRelay2::TestSendGuildMasterDelete()
	{
	return SendGuildMasterDelete("롹매니아");
	}
	
BOOL AgsmRelay2::TestSendGuildMasterSelect()
	{
	return SendGuildMasterSelect();
	}

BOOL AgsmRelay2::TestSendGuildMemberInsert()
	{
	AgpdGuildMember csMember;
	strcpy(csMember.m_szID , "RTEST2");
	csMember.m_lRank = 0;
	csMember.m_lJoinDate =2;
	csMember.m_lLevel = 3;
	csMember.m_lTID = 96;
	return SendGuildMemberInsert("롹매니아", &csMember);
	}

BOOL AgsmRelay2::TestSendGuildMemberUpdate()
	{
	AgpdGuildMember csMember;
	strcpy(csMember.m_szID , "RTEST");
	csMember.m_lRank = 0;
	csMember.m_lJoinDate =2;
	csMember.m_lLevel = 9;
	csMember.m_lTID = 96;

	return SendGuildMemberUpdate("롹매니아", &csMember);
	}

BOOL AgsmRelay2::TestSendGuildMemberDelete()
	{
	CHAR *pszChar = "RTEST2";
	return SendGuildMemberDelete("롹매니아", pszChar);
	}

BOOL AgsmRelay2::TestSendGuildMemberSelect()
	{
	return SendGuildMemberSelect();
	}

BOOL AgsmRelay2::TestSendAccountWorldUpdate()
	{
	CHAR *pszAccount = "raska";
	INT64 llBankMoney = 1024;

	return SendAccountWorldUpdate(pszAccount, llBankMoney);
	}


BOOL AgsmRelay2::TestLog(INT16 n)
	{
	AgpdLog *pAgpdLog = NULL;
	INT16 nParam = AGSMRELAY_PARAM_LOG_LOGINOUT;
	AgsdDBParam *pAgsdRelay2 = NULL;
	
	if (0 == n)
		{
		pAgpdLog = new AgpdLog_Loginout;
		pAgsdRelay2 = new AgsdRelay2Log_Loginout(pAgpdLog);
		nParam = AGSMRELAY_PARAM_LOG_LOGINOUT;
		}
	else if (1 == n)
		{
		pAgpdLog = new AgpdLog_LevelUp;
		pAgsdRelay2 = new AgsdRelay2Log_LevelUp(pAgpdLog);
		nParam = AGSMRELAY_PARAM_LOG_LEVELUP;
		}
	else if (2 == n)	
		{
		pAgpdLog = new AgpdLog_ItemNPC;
		pAgsdRelay2 = new AgsdRelay2Log_ItemNPC(pAgpdLog);
		nParam = AGSMRELAY_PARAM_LOG_ITEMNPC;
		}
	else if (3 == n)
		{
		pAgpdLog = new AgpdLog_ItemTrade;
		pAgsdRelay2 = new AgsdRelay2Log_ItemTrade(pAgpdLog);
		nParam = AGSMRELAY_PARAM_LOG_ITEMTRADE;
		}
	else if (4 == n)		
		{
		pAgpdLog = new AgpdLog_ItemSmelt;
		pAgsdRelay2 = new AgsdRelay2Log_ItemSmelt(pAgpdLog);
		nParam = AGSMRELAY_PARAM_LOG_ITEMSMELT;
		}
	else if (5 == n)
		{
		pAgpdLog = new AgpdLog_ItemRepair;
		pAgsdRelay2 = new AgsdRelay2Log_ItemRepair(pAgpdLog);
		nParam = AGSMRELAY_PARAM_LOG_ITEMREPAIR;
		}
	else if (6 == n)
		{
		pAgpdLog = new AgpdLog_ItemETC;
		pAgsdRelay2 = new AgsdRelay2Log_ItemETC(pAgpdLog);
		nParam = AGSMRELAY_PARAM_LOG_ITEMETC;
		}
	else if (7 == n)
		{					
		pAgpdLog = new AgpdLog_SkillBuy;
		pAgsdRelay2 = new AgsdRelay2Log_SkillBuy(pAgpdLog);
		nParam = AGSMRELAY_PARAM_LOG_SKILLBUY;
		}
	else if (8 == n)
		{
		pAgpdLog = new AgpdLog_SkillUp;
		pAgsdRelay2 = new AgsdRelay2Log_SkillUp(pAgpdLog);
		nParam = AGSMRELAY_PARAM_LOG_SKILLUP;
		}
	else if (9 == n)
		{
		pAgpdLog = new AgpdLog_Quest;
		pAgsdRelay2 = new AgsdRelay2Log_Quest(pAgpdLog);
		nParam = AGSMRELAY_PARAM_LOG_QUEST;
		}
	else
		{
		return TRUE;
		}
		
	OperationLogStatic(pAgsdRelay2, nParam);

	if (pAgpdLog)
		{
		delete pAgpdLog;
		pAgpdLog = NULL;
		}

	return TRUE;	
	}

#endif

