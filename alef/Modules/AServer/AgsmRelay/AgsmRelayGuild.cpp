// AgsmRelayGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 01.


#include "AgsmRelay.h"


BOOL AgsmRelay::OperationGuildMaster2(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelayGuildMaster2 csGuildMaster;
	memset(&csGuildMaster, 0, sizeof(csGuildMaster));
	
	if(pvPacket)
	{
		m_csGuildMasterPacket2.GetField(FALSE, pvPacket, 0,
								&csGuildMaster.m_szGuildID,
								&csGuildMaster.m_szMasterID,
								&csGuildMaster.m_lGuildTID,
								&csGuildMaster.m_lGuildRank,
								&csGuildMaster.m_lCreationDate,
								&csGuildMaster.m_lMaxMemberCount,
								&csGuildMaster.m_lUnionID,
								&csGuildMaster.m_szPassword);
	}

	csGuildMaster.m_ulNID = ulNID;

	if(lDBOperation == AGSMRELAY_DB_OPERATION_SELECT_RESULT)
	{
		return OperationGuildMasterSelectResult2(&csGuildMaster);
	}
	else
	{
		return EnumCallback(AGSMRELAY_OPERATION2_GUILD_MASTER, &csGuildMaster, (PVOID)lDBOperation);
	}
}

BOOL AgsmRelay::OperationGuildMember2(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelayGuildMember2 csGuildMember;
	memset(&csGuildMember, 0, sizeof(csGuildMember));

	if(pvPacket)
	{
		m_csGuildMemberPacket2.GetField(FALSE, pvPacket, 0,
								&csGuildMember.m_szMemberID,
								&csGuildMember.m_szGuildID,
								&csGuildMember.m_lMemberRank,
								&csGuildMember.m_lJoinDate,
								&csGuildMember.m_lLevel,
								&csGuildMember.m_lTID);
	}

	csGuildMember.m_ulNID = ulNID;

	if(lDBOperation == AGSMRELAY_DB_OPERATION_SELECT_RESULT)
	{
		return OperationGuildMemberSelectResult2(&csGuildMember);
	}
	else
	{
		return EnumCallback(AGSMRELAY_OPERATION2_GUILD_MEMBER, &csGuildMember, (PVOID)lDBOperation);
	}
}

// Game Server 에서 받은 것임.
BOOL AgsmRelay::OperationGuildMasterSelectResult2(AgsdRelayGuildMaster2* pcsGuildMaster)
{
	if(!m_pAgsmGuild)
		return FALSE;

	if(!pcsGuildMaster)
		return FALSE;

	return m_pAgsmGuild->GuildLoadFromDB(0, pcsGuildMaster->m_szGuildID, pcsGuildMaster->m_szMasterID, pcsGuildMaster->m_lGuildTID,
																	pcsGuildMaster->m_lGuildRank, pcsGuildMaster->m_lCreationDate, pcsGuildMaster->m_lMaxMemberCount,
																	pcsGuildMaster->m_lUnionID, pcsGuildMaster->m_szPassword);
}

BOOL AgsmRelay::OperationGuildMemberSelectResult2(AgsdRelayGuildMember2* pcsGuildMember)
{
	if(!m_pAgsmGuild)
		return FALSE;

	if(!pcsGuildMember)
		return FALSE;

	return m_pAgsmGuild->MemberLoadFromDB(pcsGuildMember->m_szMemberID, pcsGuildMember->m_szGuildID, pcsGuildMember->m_lMemberRank,
																	pcsGuildMember->m_lJoinDate, pcsGuildMember->m_lLevel, pcsGuildMember->m_lTID);
}





//////////////////////////////////////////////////////////////////////////
// Send Packet
BOOL AgsmRelay::SendGuildMasterInsert2(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildMasterPacket = m_csGuildMasterPacket2.MakePacket(FALSE, &nPacketLength, 0,
																pcsGuild->m_szID,
																pcsGuild->m_szMasterID,
																&pcsGuild->m_lTID,
																&pcsGuild->m_lRank,
																&pcsGuild->m_lCreationDate,
																&pcsGuild->m_lMaxMemberCount,
																&pcsGuild->m_lUnionID,
																&pcsGuild->m_szPassword
																);
	if(!pvGuildMasterPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_GUILD_MASTER, 0, AGSMRELAY_DB_OPERATION_INSERT,
																NULL, NULL, NULL, NULL, NULL, pvGuildMasterPacket, NULL, NULL, NULL);
	
	m_csGuildMasterPacket2.FreePacket(pvGuildMasterPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmRelay::SendGuildMasterUpdate2(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildMasterPacket = m_csGuildMasterPacket2.MakePacket(FALSE, &nPacketLength, 0,
																pcsGuild->m_szID,
																pcsGuild->m_szMasterID,
																&pcsGuild->m_lTID,
																&pcsGuild->m_lRank,
																&pcsGuild->m_lCreationDate,
																&pcsGuild->m_lMaxMemberCount,
																&pcsGuild->m_lUnionID,
																NULL
																);
	if(!pvGuildMasterPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_GUILD_MASTER, 0, AGSMRELAY_DB_OPERATION_UPDATE,
																NULL, NULL, NULL, NULL, NULL, pvGuildMasterPacket, NULL, NULL, NULL);
	
	m_csGuildMasterPacket2.FreePacket(pvGuildMasterPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmRelay::SendGuildMasterDelete2(CHAR* szGuildID)
{
	if(!szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildMasterPacket = m_csGuildMasterPacket2.MakePacket(FALSE, &nPacketLength, 0,
																szGuildID,
																NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if(!pvGuildMasterPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_GUILD_MASTER, 0, AGSMRELAY_DB_OPERATION_DELETE,
																NULL, NULL, NULL, NULL, NULL, pvGuildMasterPacket, NULL, NULL, NULL);
	
	m_csGuildMasterPacket2.FreePacket(pvGuildMasterPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Game Server 에서 사용한다.
BOOL AgsmRelay::SendGuildMasterSelect2()
{
	INT16 nPacketLength = 0;
	PVOID pvGuildMasterPacket = m_csGuildMasterPacket2.MakePacket(FALSE, &nPacketLength, 0,
																NULL,
																NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if(!pvGuildMasterPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_GUILD_MASTER, 0, AGSMRELAY_DB_OPERATION_SELECT,
																NULL, NULL, NULL, NULL, NULL, pvGuildMasterPacket, NULL, NULL, NULL);
	
	m_csGuildMasterPacket2.FreePacket(pvGuildMasterPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Relay Server 에서 사용한다.
BOOL AgsmRelay::SendGuildMasterSelectResult2(UINT32 lNID, CHAR* szGuildID, INT32 lGuildTID, CHAR* szMasterID, INT32 lMaxMemberCount,
											 INT32 lCreationDate, INT32 lUnionID, INT32 lGuildRank, CHAR* szPassword)
{
	if(!lNID || !szGuildID || !szMasterID)
		return FALSE;

	AgsdServer2* pcsGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(lNID);
	if(!pcsGameServer)
		return FALSE;

	lNID = pcsGameServer->m_dpnidServer;

	INT16 nPacketLength = 0;
	PVOID pvGuildMasterPacket = m_csGuildMasterPacket2.MakePacket(FALSE, &nPacketLength, 0,
																szGuildID,
																szMasterID,
																&lGuildTID,
																&lGuildRank,
																&lCreationDate,
																&lMaxMemberCount,
																&lUnionID,
																szPassword
																);
	if(!pvGuildMasterPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_GUILD_MASTER, 0, AGSMRELAY_DB_OPERATION_SELECT_RESULT,
																NULL, NULL, NULL, NULL, NULL, pvGuildMasterPacket, NULL, NULL, NULL);
	
	m_csGuildMasterPacket2.FreePacket(pvGuildMasterPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, lNID);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Relay Server 에서 사용한다.
BOOL AgsmRelay::SendGuildMasterSelectResult2(UINT32 lNID, AgsdRelayGuildMaster2* pcsGuildMaster)
{
	if(!lNID || !pcsGuildMaster)
		return FALSE;

	return SendGuildMasterSelectResult2(lNID, pcsGuildMaster->m_szGuildID, pcsGuildMaster->m_lGuildTID, pcsGuildMaster->m_szMasterID, 
															pcsGuildMaster->m_lMaxMemberCount, pcsGuildMaster->m_lCreationDate, pcsGuildMaster->m_lUnionID,
															pcsGuildMaster->m_lGuildRank, pcsGuildMaster->m_szPassword);
}

BOOL AgsmRelay::SendGuildMemberInsert2(CHAR* szGuildID, AgpdGuildMember* pcsMember)
{
	if(!szGuildID || !pcsMember)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildMemberPacket = m_csGuildMemberPacket2.MakePacket(FALSE, &nPacketLength, 0,
																pcsMember->m_szID,
																szGuildID,
																&pcsMember->m_lRank,
																&pcsMember->m_lJoinDate,
																&pcsMember->m_lLevel,
																&pcsMember->m_lTID
																);
	if(!pvGuildMemberPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_GUILD_MEMBER, 0, AGSMRELAY_DB_OPERATION_INSERT,
																NULL, NULL, NULL, NULL, NULL, NULL, pvGuildMemberPacket, NULL, NULL);

	m_csGuildMemberPacket2.FreePacket(pvGuildMemberPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmRelay::SendGuildMemberUpdate2(CHAR* szGuildID, AgpdGuildMember* pcsMember)
{
	if(!szGuildID || !pcsMember)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildMemberPacket = m_csGuildMemberPacket2.MakePacket(FALSE, &nPacketLength, 0,
																pcsMember->m_szID,
																szGuildID,
																&pcsMember->m_lRank,
																&pcsMember->m_lJoinDate,
																&pcsMember->m_lLevel,
																&pcsMember->m_lTID
																);
	if(!pvGuildMemberPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_GUILD_MEMBER, 0, AGSMRELAY_DB_OPERATION_UPDATE,
																NULL, NULL, NULL, NULL, NULL, NULL, pvGuildMemberPacket, NULL, NULL);

	m_csGuildMemberPacket2.FreePacket(pvGuildMemberPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmRelay::SendGuildMemberDelete2(CHAR* szGuildID, CHAR* szCharID)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildMemberPacket = m_csGuildMemberPacket2.MakePacket(FALSE, &nPacketLength, 0,
																szCharID,
																szGuildID,
																NULL, NULL, NULL, NULL);
	if(!pvGuildMemberPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_GUILD_MEMBER, 0, AGSMRELAY_DB_OPERATION_DELETE,
																NULL, NULL, NULL, NULL, NULL, NULL, pvGuildMemberPacket, NULL, NULL);

	m_csGuildMemberPacket2.FreePacket(pvGuildMemberPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Game Server 에서 사용한다.
BOOL AgsmRelay::SendGuildMemberSelect2()
{
	INT16 nPacketLength = 0;
	PVOID pvGuildMasterPacket = m_csGuildMasterPacket2.MakePacket(FALSE, &nPacketLength, 0,
																NULL,
																NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if(!pvGuildMasterPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_GUILD_MEMBER, 0, AGSMRELAY_DB_OPERATION_SELECT,
																NULL, NULL, NULL, NULL, NULL, pvGuildMasterPacket, NULL, NULL, NULL);
	
	m_csGuildMasterPacket2.FreePacket(pvGuildMasterPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Relay Server 에서 사용한다.
BOOL AgsmRelay::SendGuildMemberSelectResult2(UINT32 lNID, CHAR* szMemberID, CHAR* szGuildID, INT32 lRank, INT32 lJoinDate, INT32 lTID, INT32 lLevel)
{
	if(!lNID || !szMemberID || !szGuildID)
		return FALSE;

	AgsdServer2* pcsGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(lNID);
	if(!pcsGameServer)
		return FALSE;

	lNID = pcsGameServer->m_dpnidServer;

	INT16 nPacketLength = 0;
	PVOID pvGuildMemberPacket = m_csGuildMemberPacket2.MakePacket(FALSE, &nPacketLength, 0,
																szMemberID,
																szGuildID,
																&lRank,
																&lJoinDate,
																&lLevel,
																&lTID
																);
	if(!pvGuildMemberPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_GUILD_MEMBER, 0, AGSMRELAY_DB_OPERATION_SELECT_RESULT,
																NULL, NULL, NULL, NULL, NULL, NULL, pvGuildMemberPacket, NULL, NULL);

	m_csGuildMemberPacket2.FreePacket(pvGuildMemberPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, lNID);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Relay Server 에서 사용한다.
BOOL AgsmRelay::SendGuildMemberSelectResult2(UINT32 lNID, AgsdRelayGuildMember2* pcsGuildMember)
{
	if(!lNID || !pcsGuildMember)
		return FALSE;

	return SendGuildMemberSelectResult2(lNID, pcsGuildMember->m_szMemberID, pcsGuildMember->m_szGuildID, pcsGuildMember->m_lMemberRank,
																pcsGuildMember->m_lJoinDate, pcsGuildMember->m_lTID, pcsGuildMember->m_lLevel);
}




//////////////////////////////////////////////////////////////////////////
// Callback
BOOL AgsmRelay::CBGuildMasterInsert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgsmRelay* pThis = (AgsmRelay*)pClass;

	if(!pcsGuild || !pThis)
		return FALSE;

	return pThis->SendGuildMasterInsert2(pcsGuild);
}

BOOL AgsmRelay::CBGuildMasterUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgsmRelay* pThis = (AgsmRelay*)pClass;

	if(!pcsGuild || !pThis)
		return FALSE;

	return pThis->SendGuildMasterUpdate2(pcsGuild);
}

BOOL AgsmRelay::CBGuildMasterDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmRelay* pThis = (AgsmRelay*)pClass;

	if(!szGuildID || !pThis)
		return FALSE;

	return pThis->SendGuildMasterDelete2(szGuildID);
}

BOOL AgsmRelay::CBGuildMasterSelect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmRelay* pThis = (AgsmRelay*)pClass;

	if(!pThis)
		return FALSE;

	return pThis->SendGuildMasterSelect2();
}

BOOL AgsmRelay::CBGuildMemberInsert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmRelay* pThis = (AgsmRelay*)pClass;
	AgpdGuildMember* pcsMember = (AgpdGuildMember*)pCustData;

	if(!szGuildID || !pThis || !pcsMember)
		return FALSE;

	return pThis->SendGuildMemberInsert2(szGuildID, pcsMember);
}

BOOL AgsmRelay::CBGuildMemberUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmRelay* pThis = (AgsmRelay*)pClass;
	AgpdGuildMember* pcsMember = (AgpdGuildMember*)pCustData;

	if(!szGuildID || !pThis || !pcsMember)
		return FALSE;

	return pThis->SendGuildMemberUpdate2(szGuildID, pcsMember);
}

BOOL AgsmRelay::CBGuildMemberDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmRelay* pThis = (AgsmRelay*)pClass;
	CHAR* szCharID = (CHAR*)pCustData;

	if(!szGuildID || !pThis || !szCharID)
		return FALSE;

	return pThis->SendGuildMemberDelete2(szGuildID, szCharID);
}

BOOL AgsmRelay::CBGuildMemberSelect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmRelay* pThis = (AgsmRelay*)pClass;

	if(!pThis)
		return FALSE;

	return pThis->SendGuildMemberSelect2();
}