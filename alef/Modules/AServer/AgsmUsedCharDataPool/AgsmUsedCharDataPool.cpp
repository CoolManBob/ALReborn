#include "AgsmUsedCharDataPool.h"

AgsmUsedCharDataPool::AgsmUsedCharDataPool()
{
	SetModuleName("AgsmUsedCharDataPool");

	m_ppTIDDataList			= NULL;
	m_pRemoveDataListHead	= NULL;
	m_pRemoveDataListTail	= NULL;

	m_Mutex.Init();
}

AgsmUsedCharDataPool::~AgsmUsedCharDataPool()
{
	if (m_ppTIDDataList)
	{
		delete [] m_ppTIDDataList;
		m_ppTIDDataList	= NULL;
	}

	m_Mutex.Destroy();
}

BOOL AgsmUsedCharDataPool::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)	GetModule("AgpmCharacter");
	m_pcsAgpmEventSpawn		= (AgpmEventSpawn *) GetModule("AgpmEventSpawn");
	m_pcsAgpmProduct		= (AgpmProduct *)	GetModule("AgpmProduct");
	m_pcsAgsmFactors		= (AgsmFactors *)	GetModule("AgsmFactors");
	m_pcsAgsmCharacter		= (AgsmCharacter *)	GetModule("AgsmCharacter");
	m_pcsAgsmCharManager	= (AgsmCharManager *)	GetModule("AgsmCharManager");
	m_pcsAgsmSkill			= (AgsmSkill *)		GetModule("AgsmSkill");
	m_pcsAgsmSummons		= (AgsmSummons *)	GetModule("AgsmSummons");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventSpawn ||
		!m_pcsAgpmProduct ||
		!m_pcsAgsmFactors ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmSkill ||
		!m_pcsAgsmCharManager ||
		!m_pcsAgsmSummons)
		return FALSE;

#ifdef	_USE_CHARDATAPOOL_
	m_lIndexAttachData	= m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgsdUsedPoolList), NULL, NULL);
	if (m_lIndexAttachData < 0)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackReadAllCharacterTemplate(CBReadCharacterTemplate, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackRemoveCharacterFromMap(CBRemoveCharacterFromMap, this))
		return FALSE;
#endif

	return TRUE;
}

AgsdUsedPoolList* AgsmUsedCharDataPool::GetCharacterAttachedData(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	return (AgsdUsedPoolList *) m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexAttachData, pcsCharacter);
}

BOOL AgsmUsedCharDataPool::CBReadCharacterTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmUsedCharDataPool	*pThis	= (AgsmUsedCharDataPool *)	pClass;

	if (!pThis->m_pcsAgpmCharacter)
		return FALSE;

	if (pThis->m_pcsAgpmCharacter->GetMaxTID() > 0)
	{
		if (pThis->m_ppTIDDataList)
		{
			delete [] pThis->m_ppTIDDataList;
			pThis->m_ppTIDDataList	= NULL;
		}

		pThis->m_ppTIDDataList	= new AgpdCharacter *[pThis->m_pcsAgpmCharacter->GetMaxTID() + 1];

		ZeroMemory(pThis->m_ppTIDDataList, sizeof(AgpdCharacter *) * (pThis->m_pcsAgpmCharacter->GetMaxTID() + 1));
	}

	return TRUE;
}

BOOL AgsmUsedCharDataPool::CBRemoveCharacterFromMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;

	if (!pData || !pClass)
		return FALSE;

	AgsmUsedCharDataPool	*pThis			= (AgsmUsedCharDataPool *)	pClass;
	AgpdCharacter			*pcsCharacter	= (AgpdCharacter *)			pData;

	if (pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	return pThis->PushUsedData(pcsCharacter);
}

BOOL AgsmUsedCharDataPool::PushUsedData(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	ASSERT(!pcsCharacter->m_bIsAddMap);

	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	if (!PushRemoveList(pcsCharacter))
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "PushRemoveList() is failed.\n");
		AuLogFile_s("LOG\\_SpawnError.txt", strCharBuff);

		return FALSE;
	}

	if (!PushTIDList(pcsCharacter))
	{
		DeleteFromRemoveList(pcsCharacter);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "PushTIDList() is failed.\n");
		AuLogFile_s("LOG\\_SpawnError.txt", strCharBuff);
		
		return FALSE;
	}

	return TRUE;
}

AgpdCharacter* AgsmUsedCharDataPool::PopUsedData()
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return NULL;

    AgpdCharacter	*pcsPopData	= PopRemoveList();
	if (!pcsPopData)
		return NULL;

	DeleteFromTIDList(pcsPopData);

	ASSERT(!pcsPopData->m_bIsAddMap);
	if (pcsPopData->m_bIsAddMap)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "PopUsedData() : pcsPopData->m_bIsAddMap is TRUE\n");
		AuLogFile_s("LOG\\_SpawnError.txt", strCharBuff);
	}

	return pcsPopData;
}

AgpdCharacter* AgsmUsedCharDataPool::PopUsedTemplateData(INT32 lTID)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return NULL;

	AgpdCharacter	*pcsPopData	= PopTIDList(lTID);
	if (!pcsPopData)
		return NULL;

	DeleteFromRemoveList(pcsPopData);

	ASSERT(!pcsPopData->m_bIsAddMap);
	if (pcsPopData->m_bIsAddMap)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "PopUsedTemplateData(%d) : pcsPopData->m_bIsAddMap is TRUE\n", lTID);
		AuLogFile_s("LOG\\_SpawnError.txt", strCharBuff);
	}

	return pcsPopData;
}

BOOL AgsmUsedCharDataPool::RemoveWorld(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdUsedPoolList	*pcsUsedPoolList	= GetCharacterAttachedData(pcsCharacter);
	ASSERT(!pcsUsedPoolList->m_bIsAddedUsedPool);
	if (pcsUsedPoolList->m_bIsAddedUsedPool)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "RemoveWorld() is failed. m_bIsAddedUsedPool is already set TRUE\n");
		AuLogFile_s("LOG\\_SpawnError.txt", strCharBuff);

		return FALSE;
	}

	m_pcsAgsmSkill->EndAllBuffedSkill(pcsCharacter);

	//m_pcsAgpmEventSpawn->CBRemoveCharacter(pcsCharacter, m_pcsAgpmEventSpawn, NULL);
	m_pcsAgsmSummons->CBRemoveCharacter(pcsCharacter, m_pcsAgsmSummons, NULL);

	if (!m_pcsAgpmCharacter->RemoveCharacterFromMap(pcsCharacter))
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "RemoveWorld() is failed. RemoveCharacterFromMap() return FALSE\n");
		AuLogFile_s("LOG\\_SpawnError.txt", strCharBuff);
		
		return FALSE;
	}

	pcsCharacter->m_unCurrentStatus = AGPDCHAR_STATUS_LOGOUT;

	if (!m_pcsAgpmCharacter->m_csACharacter.RemoveCharacter(pcsCharacter->m_lID, pcsCharacter->m_szID[0] ? pcsCharacter->m_szID:NULL))
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "RemoveWorld() is failed. RemoveCharacter() return FALSE\n");
		AuLogFile_s("LOG\\_SpawnError.txt", strCharBuff);

		return FALSE;
	}

	pcsUsedPoolList->m_bIsAddedUsedPool	= TRUE;

	return TRUE;
}

BOOL AgsmUsedCharDataPool::AddWorld(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdUsedPoolList	*pcsUsedPoolList	= GetCharacterAttachedData(pcsCharacter);
	ASSERT(pcsUsedPoolList->m_bIsAddedUsedPool);
	if (!pcsUsedPoolList->m_bIsAddedUsedPool)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AddWorld() is failed. m_bIsAddedUsedPool is already set FALSE\n");
		AuLogFile_s("LOG\\_SpawnError.txt", strCharBuff);
		
		return FALSE;
	}

	if (!m_pcsAgpmCharacter->AddCharacter(pcsCharacter))
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AddWorld() is failed. AddCharacter() return FALSE\n");
		AuLogFile_s("LOG\\_SpawnError.txt", strCharBuff);
		
		return FALSE;
	}

	m_pcsAgsmCharacter->InitHistory(pcsCharacter);
	m_pcsAgsmCharacter->InitPartyHistory(pcsCharacter);
	m_pcsAgsmCharacter->InitTargetInfoArray(pcsCharacter);

	m_pcsAgpmProduct->InitGatherData(pcsCharacter);
	m_pcsAgpmProduct->InitComposeData(pcsCharacter);

	//m_pcsAgsmCharManager->EnumCallback(AGSMCHARMM_CB_CREATECHAR, pcsCharacter, NULL);

	pcsCharacter->m_unActionStatus	= AGPDCHAR_STATUS_DEAD;

	m_pcsAgsmFactors->SetCharPoint(&pcsCharacter->m_csFactor, TRUE, FALSE);

	m_pcsAgpmCharacter->UpdateStatus(pcsCharacter, AGPDCHAR_STATUS_IN_GAME_WORLD);

	m_pcsAgpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_NORMAL);

	pcsUsedPoolList->m_bIsAddedUsedPool	= FALSE;

	return TRUE;
}

BOOL AgsmUsedCharDataPool::PushRemoveList(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdUsedPoolList	*pcsUsedPoolList	= GetCharacterAttachedData(pcsCharacter);

	if (m_pRemoveDataListTail)
	{
		AgsdUsedPoolList	*pcsUsedTailList	= GetCharacterAttachedData(m_pRemoveDataListTail);
		pcsUsedTailList->pNextRemoveData	= pcsCharacter;
	}
	else
	{
		m_pRemoveDataListHead	= pcsCharacter;
	}

	pcsUsedPoolList->pNextRemoveData	= NULL;
	pcsUsedPoolList->pPrevRemoveData	= m_pRemoveDataListTail;

	m_pRemoveDataListTail	= pcsCharacter;

	return TRUE;
}

AgpdCharacter* AgsmUsedCharDataPool::PopRemoveList()
{
	if (m_pRemoveDataListHead == m_pRemoveDataListTail)
	{
		m_pRemoveDataListHead	= NULL;
		m_pRemoveDataListTail	= NULL;

		return m_pRemoveDataListHead;
	}
	else
	{
		AgpdCharacter	*pcsPopData	= m_pRemoveDataListHead;

		DeleteFromRemoveList(pcsPopData);

		return pcsPopData;
	}

	return NULL;
}

BOOL AgsmUsedCharDataPool::DeleteFromRemoveList(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdUsedPoolList	*pcsUsedPoolList	= GetCharacterAttachedData(pcsCharacter);

	if (pcsUsedPoolList->pNextRemoveData)
	{
		AgsdUsedPoolList	*pcsNextUsedPoolList	= GetCharacterAttachedData(pcsUsedPoolList->pNextRemoveData);
		pcsNextUsedPoolList->pPrevRemoveData	= pcsUsedPoolList->pPrevRemoveData;
	}

	if (pcsUsedPoolList->pPrevRemoveData)
	{
		AgsdUsedPoolList	*pcsPrevUsedPoolList	= GetCharacterAttachedData(pcsUsedPoolList->pPrevRemoveData);
		pcsPrevUsedPoolList->pNextRemoveData	= pcsUsedPoolList->pNextRemoveData;
	}

	if (pcsCharacter == m_pRemoveDataListHead)
		m_pRemoveDataListHead	= pcsUsedPoolList->pNextRemoveData;

	if (pcsCharacter == m_pRemoveDataListTail)
		m_pRemoveDataListTail	= pcsUsedPoolList->pPrevRemoveData;

	return TRUE;
}

BOOL AgsmUsedCharDataPool::PushTIDList(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdUsedPoolList	*pcsUsedPoolList	= GetCharacterAttachedData(pcsCharacter);

	if (m_ppTIDDataList)
	{
		pcsUsedPoolList->pNextTIDData	= m_ppTIDDataList[pcsCharacter->m_pcsCharacterTemplate->m_lID];
		pcsUsedPoolList->pPrevTIDData	= NULL;

		AgsdUsedPoolList	*pcsNextTIDList	= GetCharacterAttachedData(m_ppTIDDataList[pcsCharacter->m_pcsCharacterTemplate->m_lID]);
		if (pcsNextTIDList)
			pcsNextTIDList->pPrevTIDData	= pcsCharacter;

		m_ppTIDDataList[pcsCharacter->m_pcsCharacterTemplate->m_lID]	= pcsCharacter;
	}
	else
		return FALSE;

	return TRUE;
}

AgpdCharacter* AgsmUsedCharDataPool::PopTIDList(INT32 lTID)
{
	if (m_pcsAgpmCharacter->GetMaxTID() < lTID)
		return NULL;

	if (!m_ppTIDDataList || !m_ppTIDDataList[lTID])
		return NULL;

	AgpdCharacter	*pcsPopData	= m_ppTIDDataList[lTID];

	DeleteFromTIDList(pcsPopData);

	return pcsPopData;
}

BOOL AgsmUsedCharDataPool::DeleteFromTIDList(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdUsedPoolList	*pcsUsedList	= GetCharacterAttachedData(pcsCharacter);

	if (pcsUsedList->pNextTIDData)
	{
		AgsdUsedPoolList	*pcsNextTIDPoolList	= GetCharacterAttachedData(pcsUsedList->pNextTIDData);
		pcsNextTIDPoolList->pPrevTIDData	= pcsUsedList->pPrevTIDData;
	}

	if (pcsUsedList->pPrevTIDData)
	{
		AgsdUsedPoolList	*pcsNextTIDPoolList	= GetCharacterAttachedData(pcsUsedList->pPrevTIDData);
		pcsNextTIDPoolList->pNextTIDData	= pcsUsedList->pNextTIDData;
	}

	if (pcsCharacter == m_ppTIDDataList[pcsCharacter->m_pcsCharacterTemplate->m_lID])
		m_ppTIDDataList[pcsCharacter->m_pcsCharacterTemplate->m_lID]	= pcsUsedList->pNextTIDData;

	return TRUE;
}