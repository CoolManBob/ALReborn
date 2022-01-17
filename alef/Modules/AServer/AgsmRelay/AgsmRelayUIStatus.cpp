#include "AgsmRelay.h"

BOOL AgsmRelay::OperationUIStatus2(INT32 cDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelayUIStatus2 csRelayUIStatus;
	ZeroMemory(&csRelayUIStatus, sizeof(AgsdRelayUIStatus2));

	m_csUIStatusPacket2.GetField(FALSE, pvPacket, 0,
							&csRelayUIStatus.pszCharName,
							&csRelayUIStatus.pszQBeltString, &csRelayUIStatus.nStringLength);

	csRelayUIStatus.ulNID = ulNID;
	return EnumCallback(AGSMRELAY_OPERATION2_UI_STATUS, (PVOID)&csRelayUIStatus, (PVOID)cDBOperation);
}

BOOL AgsmRelay::CBUIStatusUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay		*pThis			= (AgsmRelay *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	return pThis->SendUIStatusUpdate2(pcsCharacter);
}

BOOL AgsmRelay::SendUIStatusUpdate2(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	// Relay Server가 연결이 안되어 있으면 종료
	if (0 == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	CHAR	szStringBuffer[AGPMUISTATUS_MAX_QUICKBELT_STRING + 1];
	ZeroMemory(szStringBuffer, sizeof(CHAR) * (AGPMUISTATUS_MAX_QUICKBELT_STRING + 1));

	if (!m_pAgsmUIStatus->EncodingQBeltString(pcsCharacter, szStringBuffer, AGPMUISTATUS_MAX_QUICKBELT_STRING))
		return FALSE;

	INT16	nStringLength	= strlen(szStringBuffer);
	if (nStringLength > AGPMUISTATUS_MAX_QUICKBELT_STRING)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvUIStatusPacket		= m_csUIStatusPacket2.MakePacket(FALSE, &nPacketLength, 0,
															&pcsCharacter->m_szID,
															szStringBuffer, &nStringLength);

	if (!pvUIStatusPacket || nPacketLength < 1)
		return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_UI_STATUS, 0, 
										AGSMRELAY_DB_OPERATION_UPDATE, NULL, NULL, NULL, NULL, pvUIStatusPacket, NULL, NULL, NULL, NULL);

	m_csUIStatusPacket2.FreePacket(pvUIStatusPacket);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}