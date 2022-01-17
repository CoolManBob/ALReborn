#include "AgsmRelay.h"

/*
BOOL AgsmRelay::OperationCharacter(INT32 cDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelayCharacter csRelayCharacter;
	ZeroMemory(&csRelayCharacter, sizeof(csRelayCharacter));

	m_csCharPacket.GetField(FALSE, pvPacket, 0,
							&csRelayCharacter.pszAccountID,
							&csRelayCharacter.pszName,
							&csRelayCharacter.lTemplateID,
							&csRelayCharacter.pszTitle,
							&csRelayCharacter.stPosition,
							&csRelayCharacter.lCriminalStatus,
							&csRelayCharacter.lMurderPoint,
							&csRelayCharacter.lInventoryMoney,
							&csRelayCharacter.lBankMoney,
							&csRelayCharacter.ulRemainCriminalTime,
							&csRelayCharacter.ulRemainMurderTime,
							&csRelayCharacter.lHP,
							&csRelayCharacter.lMP,
							&csRelayCharacter.lSP,
							&csRelayCharacter.lExp,
							&csRelayCharacter.lLevel);

	csRelayCharacter.ulNID = ulNID;
	return EnumCallback(AGSMRELAY_OPERATION_CHARACTER, (PVOID)&csRelayCharacter, (PVOID)cDBOperation);
}
*/

BOOL AgsmRelay::OperationCharacter2(INT32 cDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelayCharacter2 csRelayCharacter;
	ZeroMemory(&csRelayCharacter, sizeof(csRelayCharacter));

	m_csCharPacket2.GetField(FALSE, pvPacket, 0,
							&csRelayCharacter.pszName,
							&csRelayCharacter.pszTitle,
							&csRelayCharacter.pszPosition,
							&csRelayCharacter.lCriminalStatus,
							&csRelayCharacter.lMurderPoint,
							&csRelayCharacter.lInventoryMoney,
							//&csRelayCharacter.lBankMoney,
							&csRelayCharacter.ulRemainCriminalTime,
							&csRelayCharacter.ulRemainMurderTime,
							&csRelayCharacter.lHP,
							&csRelayCharacter.lMP,
							&csRelayCharacter.lSP,
							&csRelayCharacter.lExp,
							&csRelayCharacter.lLevel,
							&csRelayCharacter.lSkillPoint);

	csRelayCharacter.ulNID = ulNID;
	return EnumCallback(AGSMRELAY_OPERATION2_CHARACTER, (PVOID)&csRelayCharacter, (PVOID)cDBOperation);
}

/*
BOOL AgsmRelay::CBCharacterInsert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(pData && pClass && "Character Insert를 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay*	pThis			= (AgsmRelay*)pClass;
	AgpdCharacter*	pAgpdCharacter	= (AgpdCharacter*)pData;
	
	return pThis->SendCharacterInsert(pAgpdCharacter);
}
*/

BOOL AgsmRelay::CBCharacterUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(pData && pClass && "Character Update를 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay*	pThis			= (AgsmRelay*)pClass;
	AgpdCharacter*	pAgpdCharacter	= (AgpdCharacter*)pData;
	
#ifdef	__DB3_VERSION__
	return pThis->SendCharacterUpdate2(pAgpdCharacter);
#else
	return pThis->SendCharacterUpdate(pAgpdCharacter);
#endif	//__DB3_VERSION__
}

/*
BOOL AgsmRelay::CBCharacterDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(pData && pClass && "Character Delete 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay*	pThis			= (AgsmRelay*)pClass;
	AgpdCharacter*	pAgpdCharacter	= (AgpdCharacter*)pData;
	
	return pThis->SendCharacterDelete(pAgpdCharacter);
}
*/

/*
BOOL AgsmRelay::SendResultCharacter(AgsdRelayCharacter *pAgsdRelayCharacter)
{
	return TRUE;
}

BOOL AgsmRelay::SendCharacterInsert(AgpdCharacter* pAgpdCharacter)
{
	return TRUE;
}
*/

/*
BOOL AgsmRelay::SendCharacterUpdate(AgpdCharacter* pAgpdCharacter)
{
	if (!pAgpdCharacter) 
		return FALSE;

	// Relay Server가 연결이 안되어 있으면 종료
	if (NULL == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	INT16 nPacketLength;

	INT32 lMurdererPoint = 0;
	INT32 lHP = 0;
	INT32 lMP = 0;
	INT32 lSP = 0;
	INT32 lEXP = 0;
	INT32 lLevel = 0;
	UINT32 ulCriminalRemainTime = 0;
	UINT32 ulMurderRemainTime = 0;

	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lMurdererPoint, 
							AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lHP, 
							AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lMP, 
							AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lSP, 
							AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lEXP, 
							AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lLevel, 
							AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	AgsdCharacter	*pAgsdCharacter	= m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);

	PVOID pvCharPacket = m_csCharPacket.MakePacket(FALSE, &nPacketLength, 0,
												NULL, //pAgsdCharacter->m_szAccountID,
												&pAgpdCharacter->m_szID,
												NULL, //pAgpdCharacter->m_pcsCharacterTemplate->m_lID,
												&pAgpdCharacter->m_szTitle,
												&pAgpdCharacter->m_stPos,
												&pAgpdCharacter->m_unCriminalStatus,
												&lMurdererPoint,
												&pAgpdCharacter->m_llMoney,
												&pAgpdCharacter->m_llBankMoney,
												&ulCriminalRemainTime,
												&ulMurderRemainTime,
												&lHP,
												&lMP,
												&lSP,
												&lEXP,
												&lLevel
												);

	if (!pvCharPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION_CHARACTER, 0, AGSMRELAY_DB_OPERATION_UPDATE,
									pvCharPacket, NULL, NULL, NULL, NULL);

	m_csCharPacket.FreePacket(pvCharPacket);

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

BOOL AgsmRelay::SendCharacterUpdate2(AgpdCharacter* pAgpdCharacter)
{
	if (!pAgpdCharacter) 
		return FALSE;

	// Relay Server가 연결이 안되어 있으면 종료
	if (NULL == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	INT16 nPacketLength;

	INT32 lMurdererPoint = 0;
	INT32 lHP = 0;
	INT32 lMP = 0;
	INT32 lSP = 0;
	INT32 lEXP = 0;
	INT32 lLevel = 0;
	INT32 lSkillPoint = 0;
	UINT32 ulCriminalRemainTime = 0;
	UINT32 ulMurderRemainTime = 0;

	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lMurdererPoint, 
							AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lHP, 
							AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lMP, 
							AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lSP, 
							AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lEXP, 
							AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lLevel, 
							AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	lSkillPoint	= m_pAgpmCharacter->GetSkillPoint(pAgpdCharacter);

	CHAR	szPosition[33];
	ZeroMemory(szPosition, sizeof(CHAR) * 33);

	m_pAgsmCharacter->EncodingPosition(&pAgpdCharacter->m_stPos, szPosition, 32);

	AgsdCharacter	*pAgsdCharacter	= m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);

	PVOID pvCharPacket = m_csCharPacket2.MakePacket(FALSE, &nPacketLength, 0,
												pAgpdCharacter->m_szID,
												pAgpdCharacter->m_szTitle,
												szPosition,
												&pAgpdCharacter->m_unCriminalStatus,
												&lMurdererPoint,
												&pAgpdCharacter->m_llMoney,
												//&pAgpdCharacter->m_llBankMoney,
												&ulCriminalRemainTime,
												&ulMurderRemainTime,
												&lHP,
												&lMP,
												&lSP,
												&lEXP,
												&lLevel,
												&lSkillPoint
												);

	if (!pvCharPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_CHARACTER, 0, AGSMRELAY_DB_OPERATION_UPDATE,
									pvCharPacket, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	m_csCharPacket2.FreePacket(pvCharPacket);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, m_pAgsmServerManager->GetRelayServer()->m_dpnidServer))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

/*
BOOL AgsmRelay::SendCharacterDelete(AgpdCharacter* pAgpdCharacter)
{
	return TRUE;
}
*/