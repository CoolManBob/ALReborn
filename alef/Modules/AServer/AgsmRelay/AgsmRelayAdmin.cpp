// AgsmRelayAdmin.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 08. 16.

// Relay 를 거쳐야 하고,
// Admin 에서 필요한 DB Operation 이 있으면 모두 이곳을 거친다.


#include "AgsmRelay.h"


BOOL AgsmRelay::OperationSearchCharacter(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelaySearchCharacter csSearchCharacter;
	memset(&csSearchCharacter, 0, sizeof(csSearchCharacter));

	if(pvPacket)
	{
		m_csSearchCharacterPacket.GetField(FALSE, pvPacket, 0,
								&csSearchCharacter.m_szAdminName,
								&csSearchCharacter.m_szCharName,
								&csSearchCharacter.m_szAccName,
								&csSearchCharacter.m_lLevel,
								&csSearchCharacter.m_lRace,
								&csSearchCharacter.m_lClass,
								&csSearchCharacter.m_lStatus,
								&csSearchCharacter.m_szCreationDate
								);
	}

	csSearchCharacter.m_ulNID = ulNID;

	if(lDBOperation == AGSMRELAY_DB_OPERATION_SELECT_RESULT)
	{
		return OperationSearchCharacterSelectResult(&csSearchCharacter);
	}
	else
	{
		return EnumCallback(AGSMRELAY_OPERATION2_SEARCH_CHARACTER, &csSearchCharacter, (PVOID)lDBOperation);
	}
}

BOOL AgsmRelay::OperationSearchCharacterSelectResult(AgsdRelaySearchCharacter* pcsSearchCharacter)
{
	if(!m_pAgsmAdmin)
		return FALSE;

	if(!pcsSearchCharacter)
		return FALSE;
	
	return m_pAgsmAdmin->SendSearchResult(pcsSearchCharacter->m_szAdminName,
											pcsSearchCharacter->m_szCharName,
											pcsSearchCharacter->m_szAccName,
											pcsSearchCharacter->m_lLevel,
											pcsSearchCharacter->m_lRace,
											pcsSearchCharacter->m_lClass,
											pcsSearchCharacter->m_lStatus,
											pcsSearchCharacter->m_szCreationDate
											);
}

BOOL AgsmRelay::SendSearchCharacterSelect(CHAR* szAdminName, CHAR* szAccName, CHAR* szCharName)
{
	if(!szAdminName)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvSearchCharacterPacket = m_csSearchCharacterPacket.MakePacket(FALSE, &nPacketLength, 0,
											szAdminName,
											szCharName ? szCharName : NULL,
											szAccName ? szAccName : NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL
											);
	if(!pvSearchCharacterPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_SEARCH_CHARACTER, 0, AGSMRELAY_DB_OPERATION_SELECT,
											NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvSearchCharacterPacket);

	m_csSearchCharacterPacket.FreePacket(pvSearchCharacterPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer);
	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmRelay::SendSearchCharacterSelectResult(UINT32 ulNID, AgsdRelaySearchCharacter* pcsSearchCharacter)
{
	if(!ulNID || !pcsSearchCharacter)
		return FALSE;

	AgsdServer2* pcsGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(ulNID);
	if(!pcsGameServer)
		return FALSE;

	ulNID = pcsGameServer->m_dpnidServer;
	
	INT16 nPacketLength = 0;
	PVOID pvSearchCharacterPacket = m_csSearchCharacterPacket.MakePacket(FALSE, &nPacketLength, 0,
											pcsSearchCharacter->m_szAdminName,
											pcsSearchCharacter->m_szCharName,
											pcsSearchCharacter->m_szAccName,
											&pcsSearchCharacter->m_lLevel,
											&pcsSearchCharacter->m_lRace,
											&pcsSearchCharacter->m_lClass,
											&pcsSearchCharacter->m_lStatus,
											pcsSearchCharacter->m_szCreationDate
											);
	if(!pvSearchCharacterPacket)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_SEARCH_CHARACTER, 0, AGSMRELAY_DB_OPERATION_SELECT_RESULT,
											NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvSearchCharacterPacket);
	
	m_csSearchCharacterPacket.FreePacket(pvSearchCharacterPacket);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmRelay::CBSearchCharacterByAccountSelect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szAdminName = (CHAR*)pData;
	AgsmRelay* pThis = (AgsmRelay*)pClass;
	CHAR* szAccName = (CHAR*)pCustData;
	
	return pThis->SendSearchCharacterSelect(szAdminName, szAccName, NULL);
}