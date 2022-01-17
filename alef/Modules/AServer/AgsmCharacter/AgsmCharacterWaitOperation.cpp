#include "AgsmCharacter.h"

BOOL AgsmCharacter::SetWaitOperation(AgpdCharacter *pcsCharacter, AgsmCharacterWaitOperation eWaitOperation)
{
	if (!pcsCharacter || eWaitOperation < 0 || eWaitOperation >= AGSMCHARACTER_WAIT_OPERATION_MAX)
		return FALSE;

	if (!AddListRemoveChar(pcsCharacter))
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_bWaitOperation[eWaitOperation]	= TRUE;

	//printf("\n SetWaitOperation() Character Name : %s", pcsCharacter->m_szID);

	return TRUE;
}

BOOL AgsmCharacter::ResetWaitOperation(AgpdCharacter *pcsCharacter, AgsmCharacterWaitOperation eWaitOperation)
{
	if (!pcsCharacter || eWaitOperation < 0 || eWaitOperation >= AGSMCHARACTER_WAIT_OPERATION_MAX)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_bWaitOperation[eWaitOperation]	= FALSE;

	//printf("\n ResetWaitOperation() Character Name : %s", pcsCharacter->m_szID);

	if (!IsWaitOperation(pcsCharacter))
		RemoveListRemoveChar(pcsCharacter);

	return TRUE;
}

BOOL AgsmCharacter::IsWaitOperation(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	BOOL	bWait	= FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	for (int i = 0; i < AGSMCHARACTER_WAIT_OPERATION_MAX; ++i)
	{
		if (pcsAgsdCharacter->m_bWaitOperation[i])
		{
			bWait = TRUE;
			break;
		}
	}

	return bWait;
}

BOOL AgsmCharacter::IsWaitTimeout(AgpdCharacter *pcsCharacter, UINT32 ulClockCount)
{
	if (!pcsCharacter)
		return FALSE;

	if (IsWaitOperation(pcsCharacter))
	{
		AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

		return (pcsAgsdCharacter->m_ulMaxWaitTime <= ulClockCount);
	}
	else
		RemoveListRemoveChar(pcsCharacter);

	return FALSE;
}

BOOL AgsmCharacter::AddListRemoveChar(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_bWaitOperationBeforeRemove)
	{
		pcsAgsdCharacter->m_ulMaxWaitTime				= AGSMCHARACTER_MAX_WAIT_OPERATION_TIME + GetClockCount();

		return TRUE;
	}

	if (pcsAgsdCharacter->m_lOldCID == AP_INVALID_CID)
	{
		if (!m_csAdminWaitForRemoveChar.AddObject(&pcsCharacter->m_lID, pcsCharacter->m_lID))
			return FALSE;
	}
	else
	{
		if (!m_csAdminWaitForRemoveChar.AddObject(&pcsAgsdCharacter->m_lOldCID, pcsAgsdCharacter->m_lOldCID))
			return FALSE;
	}

	pcsAgsdCharacter->m_bWaitOperationBeforeRemove	= TRUE;

	//ZeroMemory(pcsAgsdCharacter->m_bWaitOperation, sizeof(BOOL) * AGSMCHARACTER_WAIT_OPERATION_MAX);
	pcsAgsdCharacter->m_bWaitOperation.MemSetAll();

	pcsAgsdCharacter->m_ulMaxWaitTime				= AGSMCHARACTER_MAX_WAIT_OPERATION_TIME + GetClockCount();

	return TRUE;
}

BOOL AgsmCharacter::RemoveListRemoveChar(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_bWaitOperationBeforeRemove	= FALSE;

	return m_csAdminWaitForRemoveChar.RemoveObject(pcsCharacter->m_lID);
}