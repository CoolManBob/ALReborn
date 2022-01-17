#include "AgsmCharacter.h"

BOOL AgsmCharacter::SetMaxCheckRecvChar(UINT32 lMaxCheckRecvChar)
{
	return m_csAdminCheckRecvChar.SetCount(lMaxCheckRecvChar);
}

BOOL AgsmCharacter::SetMaxWaitForRemoveChar(UINT32 lCount)
{
	return m_csAdminWaitForRemoveChar.SetCount(lCount);
}

BOOL AgsmCharacter::IsRecvChar(INT32 lCID)
{
	if (lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);

	if (!pcsCharacter)
		return FALSE;

	return TRUE;
}

BOOL AgsmCharacter::CheckRecvChar(INT32 lCID, INT32 lServerID, INT32 lOldCID)
{
	if (lCID == AP_INVALID_CID || lServerID == AP_INVALID_SERVERID)
		return FALSE;

	AgsdServer	*pcsServer = m_pAgsmServerManager2->GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	if (!IsRecvChar(lCID))
	{
		INT32	lBuffer[3] = { lCID, lOldCID, lServerID };

		if (!m_csAdminCheckRecvChar.AddObject(lBuffer, lCID))
			return FALSE;

		return TRUE;
	}

	return SendCheckRecvCharResult(lOldCID, pcsServer->m_dpnidServer, TRUE);
}

BOOL AgsmCharacter::CheckRecvCharList(INT32 lCID)
{
	if (lCID == AP_INVALID_CID)
		return FALSE;

	INT32	*pBuffer = (INT32 *) m_csAdminCheckRecvChar.GetObject(lCID);
	if (!pBuffer)
		return FALSE;

	m_csAdminCheckRecvChar.RemoveObject(lCID);

	AgsdServer	*pcsServer	= m_pAgsmServerManager2->GetServer(pBuffer[2]);
	if (!pcsServer)
		return FALSE;

	return SendCheckRecvCharResult(pBuffer[1], pcsServer->m_dpnidServer, TRUE);
}

AgsdServer* AgsmCharacter::GetRecvCharServer(AgpdCharacter* pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgsdCharacter* pagsdCharacter = GetADCharacter(pcsCharacter);
	if(!pagsdCharacter)
		return NULL;

	return (m_pAgsmServerManager2->GetServer(pagsdCharacter->m_nRecvLoginServer));
}