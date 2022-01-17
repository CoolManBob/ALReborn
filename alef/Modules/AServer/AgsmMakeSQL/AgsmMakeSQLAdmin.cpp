// AgsmMakeSQLAdmin.cpp
// 2004.08.17. steeple
// Admin 에서 사용하는 Query 모음

#include "AgsmMakeSQL.h"


BOOL AgsmMakeSQL::SearchCharacterByAccount(AgsdRelaySearchCharacter* pcsSearchCharacter)
{
	if(!pcsSearchCharacter)
		return FALSE;

	// 유효성 검사
	if(!pcsSearchCharacter->m_szAdminName || strlen(pcsSearchCharacter->m_szAdminName) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;
	if(!pcsSearchCharacter->m_szAccName || strlen(pcsSearchCharacter->m_szAccName) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;
	
	SQLBuffer* pSQLBuffer = new SQLBuffer;

	sprintf(pSQLBuffer->Buffer, "SELECT a.charid, a.accountid, a.chartid, b.lv, a.creationdate FROM "
												"charmaster a, chardetail b "
												"WHERE a.accountid='%s' AND a.charid=b.charid",
												pcsSearchCharacter->m_szAccName);

	ASSERT(strlen(pSQLBuffer->Buffer) < sizeof(pSQLBuffer->Buffer));
	
	AgsdRelaySearchCharacter* pcsNewSearchCharacter = new AgsdRelaySearchCharacter;
	memcpy(pcsNewSearchCharacter, pcsSearchCharacter, sizeof(AgsdRelaySearchCharacter));

	// Callback Setting
	pSQLBuffer->SetCallbackParam(CBSearchCharacterSelectResult, this, pcsNewSearchCharacter);
	
	return m_pAgsmDBPool->ExecuteSQL(pSQLBuffer);
}

BOOL AgsmMakeSQL::SearchCharacterByCharName(AgsdRelaySearchCharacter* pcsSearchCharacter)
{
	if(!pcsSearchCharacter)
		return FALSE;

	return TRUE;
}

BOOL AgsmMakeSQL::CBSearchCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	INT32 lDBOperation = (INT32)pCustData;
	AgsmMakeSQL* pThis = (AgsmMakeSQL*)pClass;
	AgsdRelaySearchCharacter* pcsSearchCharacter = (AgsdRelaySearchCharacter*)pData;
	
	switch(lDBOperation)
	{
		case AGSMRELAY_DB_OPERATION_SELECT:
		{
			if(pcsSearchCharacter->m_szAccName && strlen(pcsSearchCharacter->m_szAccName) > 0)
				pThis->SearchCharacterByAccount(pcsSearchCharacter);
			else if(pcsSearchCharacter->m_szCharName && strlen(pcsSearchCharacter->m_szCharName) > 0)
				pThis->SearchCharacterByCharName(pcsSearchCharacter);

			break;
		}
	}

	return TRUE;
}

BOOL AgsmMakeSQL::CBSearchCharacterSelectResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AuRowset* pRowSet = (AuRowset*)pData;
	AgsmMakeSQL* pThis = (AgsmMakeSQL*)pClass;
	AgsdRelaySearchCharacter* pcsSearchCharacter = (AgsdRelaySearchCharacter*)pCustData;

	// 임시 변수
	AgsdRelaySearchCharacter csSearchCharacter;

	INT32 i = 0;
	for(i = 0; i < pRowSet->GetRowCount(); i++)
	{
		memset(&csSearchCharacter, 0, sizeof(csSearchCharacter));

		csSearchCharacter.m_szAdminName = pcsSearchCharacter->m_szAdminName;

		if(pRowSet->Get(i, 0))	// CharID
			csSearchCharacter.m_szCharName = (CHAR*)pRowSet->Get(i, 0);			
		if(pRowSet->Get(i, 1))	// AccountID
			csSearchCharacter.m_szAccName = (CHAR*)pRowSet->Get(i, 1);
		if(pRowSet->Get(i, 2))	// CharTID
			csSearchCharacter.m_lRace = atoi((const char*)pRowSet->Get(i, 2));
		if(pRowSet->Get(i, 3))	// Level
			csSearchCharacter.m_lLevel = atoi((const char*)pRowSet->Get(i, 3));
		if(pRowSet->Get(i, 4))	// Creation Date
			csSearchCharacter.m_szCreationDate = (CHAR*)pRowSet->Get(i, 4);

		pThis->m_pAgsmRelay->SendSearchCharacterSelectResult(pcsSearchCharacter->m_ulNID, &csSearchCharacter);
	}

	// 쿼리 할 때 만든 놈 해제
	delete pcsSearchCharacter;

	return TRUE;
}