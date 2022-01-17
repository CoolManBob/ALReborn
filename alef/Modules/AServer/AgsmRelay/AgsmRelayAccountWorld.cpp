#include "AgsmRelay.h"

BOOL AgsmRelay::OperationAccountWorld(INT32 cDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelayAccountWorld csRelayAccountWorld;
	ZeroMemory(&csRelayAccountWorld, sizeof(csRelayAccountWorld));

	m_csAccountWorldPacket.GetField(FALSE, pvPacket, 0,
							&csRelayAccountWorld.m_szAccountID,
							&csRelayAccountWorld.m_llBankMoney);

	csRelayAccountWorld.m_ulNID = ulNID;
	return EnumCallback(AGSMRELAY_OPERATION2_ACCOUNT_WORLD, (PVOID)&csRelayAccountWorld, (PVOID)cDBOperation);
}

BOOL AgsmRelay::CBAccountWorldUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay*	pThis			= (AgsmRelay*)pClass;
	AgpdCharacter*	pAgpdCharacter	= (AgpdCharacter*)pData;
	
	return pThis->SendAccountWorldUpdate(pThis->m_pAgsmCharacter->GetAccountID(pAgpdCharacter), pAgpdCharacter->m_llBankMoney);
}

BOOL AgsmRelay::SendAccountWorldUpdate(CHAR *szAccountID, INT64 llBankMoney)
{
	if (!szAccountID)
		return FALSE;

	// Relay Server가 연결이 안되어 있으면 종료
	if (NULL == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	INT16 nPacketLength	= 0;

	PVOID pvAccountWorldPacket = m_csAccountWorldPacket.MakePacket(FALSE, &nPacketLength, 0,
																	szAccountID,
																	&llBankMoney);

	if (!pvAccountWorldPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_ACCOUNT_WORLD, 0, AGSMRELAY_DB_OPERATION_UPDATE,
									NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvAccountWorldPacket, NULL);

	m_csAccountWorldPacket.FreePacket(pvAccountWorldPacket);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}