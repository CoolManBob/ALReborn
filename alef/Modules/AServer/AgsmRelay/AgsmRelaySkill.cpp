#include "AgsmRelay.h"

/*
BOOL AgsmRelay::OperationSkill(INT32 cDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelaySkill csRelaySkill;
	ZeroMemory(&csRelaySkill, sizeof(csRelaySkill));

	m_csSkillPacket.GetField(FALSE, pvPacket, 0,
							&csRelaySkill.pszCharName,
							&csRelaySkill.lMasteryIndex,
							&csRelaySkill.lTotalSkillPoint,
							&csRelaySkill.pszTreeNode,
							&csRelaySkill.lInstSpec,
							&csRelaySkill.ulRemainInstSepc,
							&csRelaySkill.lConstSpec);

	csRelaySkill.ulNID = ulNID;
	return EnumCallback(AGSMRELAY_OPERATION_SKILL, (PVOID)&csRelaySkill, (PVOID)cDBOperation);
}
*/

BOOL AgsmRelay::OperationSkill2(INT32 cDBOperation, PVOID pvPacket, UINT32 ulNID)
{
	AgsdRelaySkill2 csRelaySkill;
	ZeroMemory(&csRelaySkill, sizeof(csRelaySkill));

	m_csSkillPacket2.GetField(FALSE, pvPacket, 0,
							&csRelaySkill.pszCharName,
//							&csRelaySkill.lMasteryIndex,
//							&csRelaySkill.lTotalSkillPoint,
							&csRelaySkill.pszTreeNode,
							&csRelaySkill.pszSpecialize);

	csRelaySkill.ulNID = ulNID;
	return EnumCallback(AGSMRELAY_OPERATION2_SKILL, (PVOID)&csRelaySkill, (PVOID)cDBOperation);
}

/*
BOOL AgsmRelay::CBSkillInsert(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(pData && pClass && "Skill Insert 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay*	pThis			= (AgsmRelay*)pClass;
	ApBase*		pcsBase			= (ApBase*)pData;
	INT32		lMasteryIndex	= (INT32) pCustData;

	if (pcsBase->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	return pThis->SendSkillInsert(pcsBase, lMasteryIndex);
}
*/

BOOL AgsmRelay::CBSkillUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(pData && pClass && "Skill Update 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay*	pThis			= (AgsmRelay*)pClass;
	ApBase*		pcsBase			= (ApBase*)pData;
//	INT32		lMasteryIndex	= (INT32) pCustData;

	if (pcsBase->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

//#ifdef	__DB3_VERSION__
//	return pThis->SendSkillUpdate2(pcsBase, lMasteryIndex);
//#else
//	return pThis->SendSkillUpdate(pcsBase, lMasteryIndex);
//#endif	//__DB3_VERSION__

	return pThis->SendSkillUpdate2(pcsBase);
}

/*
BOOL AgsmRelay::CBSkillDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(pData && pClass && "Skill delete 하는데 필요한 파라메터가 잘못 넘어옴");

	AgsmRelay*	pThis			= (AgsmRelay*)pClass;
	ApBase*		pcsBase			= (ApBase*)pData;
	INT32		lMasteryIndex	= (INT32) pCustData;

	if (pcsBase->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	return pThis->SendSkillDelete(pcsBase, lMasteryIndex);
}

BOOL AgsmRelay::SendResultSkill(AgsdRelaySkill *pAgsdRelaySkill)
{
	return TRUE;
}

BOOL AgsmRelay::SendSkillInsert(ApBase *pcsBase, INT32 lMasteryIndex)
{
	return TRUE;
}

BOOL AgsmRelay::SendSkillUpdate(ApBase *pcsBase, INT32 lMasteryIndex)
{
	if (!pcsBase || pcsBase->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *) pcsBase;

	// Relay Server가 연결이 안되어 있으면 종료
	if (0 == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	AgpdSkillAttachData		*pcsAttachData	= m_pAgpmSkill->GetAttachSkillData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	INT16	nPacketLength	= 0;

	CHAR	szTreeBuffer[129];
	ZeroMemory(szTreeBuffer, sizeof(CHAR) * 129);

	m_pAgsmSkill->MakeStringMasteryActiveNodeSequence(pcsBase, lMasteryIndex, szTreeBuffer, 128);

	PVOID	pvSkillPacket	= m_csSkillPacket.MakePacket(FALSE, &nPacketLength, 0,
														&pcsCharacter->m_szID,
														&lMasteryIndex,
														&pcsAttachData->m_csMastery[lMasteryIndex].lTotalSP,
														&szTreeBuffer,
														NULL,
														NULL,
														NULL);

	if (!pvSkillPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION_SKILL, 0, 
										AGSMRELAY_DB_OPERATION_UPDATE, NULL, NULL, NULL, pvSkillPacket, NULL);

	m_csSkillPacket.FreePacket(pvSkillPacket);

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

BOOL AgsmRelay::SendSkillUpdate2(ApBase *pcsBase)
{
	if (!pcsBase || pcsBase->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *) pcsBase;

	// Relay Server가 연결이 안되어 있으면 종료
	if (0 == m_pAgsmServerManager->GetRelayServer()->m_dpnidServer)
		return FALSE;

	AgpdSkillAttachData		*pcsAttachData	= m_pAgpmSkill->GetAttachSkillData(pcsBase);
	if (!pcsAttachData)
		return FALSE;

	INT16	nPacketLength	= 0;

	CHAR	szTreeBuffer[129];
	CHAR	szSpecializeBuffer[33];

	ZeroMemory(szTreeBuffer, sizeof(CHAR) * 129);
	ZeroMemory(szSpecializeBuffer, sizeof(CHAR) * 33);

	m_pAgsmEventSkillMaster->EncodingSkillList((AgpdCharacter *) pcsBase, szTreeBuffer, 128);

//	m_pAgsmSkill->MakeStringMasteryActiveNodeSequence(pcsBase, lMasteryIndex, szTreeBuffer, 128);
//	m_pAgsmSkill->EncodingMasterySpecialize(pcsBase, lMasteryIndex, szSpecializeBuffer, 32);

//	INT32	lTotalSP	= 0;

//#ifdef	__NEW_MASTERY__
//	INT32	lTotalSP	= (INT32) pcsAttachData->m_csMastery[lMasteryIndex].m_ucTotalInputSP;
//#else
//	INT32	lTotalSP	= pcsAttachData->m_csMastery[lMasteryIndex].lTotalSP;
//#endif	//__NEW_MASTERY__

	PVOID	pvSkillPacket	= m_csSkillPacket2.MakePacket(FALSE, &nPacketLength, 0,
														pcsCharacter->m_szID,
//														&lMasteryIndex,
//														&lTotalSP,
														szTreeBuffer,
														szSpecializeBuffer);

	if (!pvSkillPacket) return FALSE;

	PVOID pvPacket = MakeRelayPacket(TRUE, &nPacketLength, AGSMRELAY_OPERATION2_SKILL, 0, 
										AGSMRELAY_DB_OPERATION_UPDATE, NULL, NULL, NULL, pvSkillPacket, NULL, NULL, NULL, NULL, NULL);

	m_csSkillPacket2.FreePacket(pvSkillPacket);

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
BOOL AgsmRelay::SendSkillDelete(ApBase *pcsBase, INT32 lMasteryIndex)
{
	return TRUE;
}
*/