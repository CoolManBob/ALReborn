#include "AgsmAOIMap.h"
#include "ApMemoryTracker.h"

AgsmAOIMap::AgsmAOIMap()
{
	ZeroMemory(m_ppCell, sizeof(AgsmAOICell *) * AGSMAOIMAP_WORLD_INDEX_WIDTH * AGSMAOIMAP_WORLD_INDEX_HEIGHT);
}

AgsmAOIMap::~AgsmAOIMap()
{
	for (int i = 0; i < AGSMAOIMAP_WORLD_INDEX_WIDTH; ++i)
	{
		for (int j = 0; j < AGSMAOIMAP_WORLD_INDEX_HEIGHT; ++j)
		{
			if (m_ppCell[i][j])
			{
				//m_ppCell[i][j]->m_Mutex.Destroy();

				delete (m_ppCell[i][j]);

				m_ppCell[i][j]	= NULL;
			}
		}
	}
}

BOOL AgsmAOIMap::AddCharacterToMap(AuPOS *pstPos, AgsmCellUnit *pstUnit)
{
	if (!pstPos || !pstUnit)
		return FALSE;

	AgsmAOICell	*pcsCell	= GetCell(pstPos);
	if (!pcsCell)
		return FALSE;

	pcsCell->m_csRWLock.LockWriter();

	BOOL	bRetval	= pcsCell->AddCharacter(pstUnit);

	BOOL	bActiveStatus	= FALSE;

	if (pstUnit->bIsPC && pcsCell->GetNumPCUnit() == 1)
		bActiveStatus	= TRUE;

	pcsCell->m_csRWLock.UnlockWriter();

	if (bActiveStatus)
		NoticeActiveStatus(pcsCell);

	return bRetval;
}

BOOL AgsmAOIMap::RemoveCharacterFromMap(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (!pstUnit->pcsCurrentCell)
		return FALSE;

	AgsmAOICell	*pcsCell	= pstUnit->pcsCurrentCell;

	pcsCell->m_csRWLock.LockWriter();

	BOOL	bRetval	= pcsCell->RemoveCharacter(pstUnit);

	BOOL	bDisableStatus	= FALSE;
	if (pstUnit->bIsPC && pcsCell->GetNumPCUnit() == 0)
		bDisableStatus = TRUE;

	pcsCell->m_csRWLock.UnlockWriter();

	if (bDisableStatus)
		NoticeDisableStatus(pcsCell);

	return bRetval;
}

AgsmAOICell* AgsmAOIMap::UpdateCharacterPosition(AuPOS *pstCurrentPos, AgsmCellUnit *pstUnit)
{
	if (!pstCurrentPos || !pstUnit)
		return NULL;

	AgsmAOICell	*pcsCurrentCell		= GetCell(pstCurrentPos);
	AgsmAOICell *pcsPrevCell		= pstUnit->pcsCurrentCell;

	if (pcsCurrentCell == pcsPrevCell)
		return NULL;

	if (pcsPrevCell)
	{
		pcsPrevCell->m_csRWLock.LockWriter();

		pcsPrevCell->RemoveCharacter(pstUnit);

		BOOL	bDisableStatus	= FALSE;

		if (pstUnit->bIsPC && pcsPrevCell->GetNumPCUnit() == 0)
			bDisableStatus	= TRUE;

		pcsPrevCell->m_csRWLock.UnlockWriter();

		if (bDisableStatus)
			NoticeDisableStatus(pcsPrevCell);
	}
//	else
//	{
//		if (pstUnit->pcsCurrentCell)
//		{
//			AgsmAOICell	*pcsCell	= pstUnit->pcsCurrentCell;
//			//pcsCell->m_Mutex.WLock();
//			EnterCriticalSection(&pcsCell->m_csCriticalSection);
//			pcsCell->RemoveCharacter(pstUnit);
//			LeaveCriticalSection(&pcsCell->m_csCriticalSection);
//			//pcsCell->m_Mutex.Release();
//		}
//	}

	pcsCurrentCell->m_csRWLock.LockWriter();

	BOOL	bRetval	= pcsCurrentCell->AddCharacter(pstUnit);

	BOOL	bActiveStatus	= FALSE;
	if (pstUnit->bIsPC && pcsCurrentCell->GetNumPCUnit() == 1)
		bActiveStatus	= TRUE;

	pcsCurrentCell->m_csRWLock.UnlockWriter();

	if (bActiveStatus)
		NoticeActiveStatus(pcsCurrentCell);

	if (!bRetval)
		return NULL;

	return pcsCurrentCell;
}

BOOL AgsmAOIMap::AddNPCToMap(AuPOS *pstPos, AgsmCellUnit *pstUnit, BOOL bIsSiegeWarCharacter)
{
	if (!pstPos || !pstUnit)
		return FALSE;

	AgsmAOICell	*pcsCell	= GetCell(pstPos);
	if (!pcsCell)
		return FALSE;

	pcsCell->m_csRWLock.LockWriter();

	BOOL	bRetval	= pcsCell->AddNPC(pstUnit, bIsSiegeWarCharacter);

	pcsCell->m_csRWLock.UnlockWriter();

	return bRetval;
}

BOOL AgsmAOIMap::RemoveNPCFromMap(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (!pstUnit->pcsCurrentCell)
		return FALSE;

	AgsmAOICell	*pcsCell	= pstUnit->pcsCurrentCell;

	pcsCell->m_csRWLock.LockWriter();

	BOOL	bRetval	= pcsCell->RemoveNPC(pstUnit);

	pcsCell->m_csRWLock.UnlockWriter();

	return bRetval;
}

AgsmAOICell* AgsmAOIMap::UpdateNPCPosition(AuPOS *pstCurrentPos, AgsmCellUnit *pstUnit)
{
	if (!pstCurrentPos || !pstUnit)
		return NULL;

	AgsmAOICell	*pcsCurrentCell		= GetCell(pstCurrentPos);
	AgsmAOICell *pcsPrevCell		= pstUnit->pcsCurrentCell;

	if (pcsCurrentCell == pcsPrevCell)
		return NULL;

	if (pcsPrevCell)
	{
		pcsPrevCell->m_csRWLock.LockWriter();

		pcsPrevCell->RemoveNPC(pstUnit);

		pcsPrevCell->m_csRWLock.UnlockWriter();
	}
//	else
//	{
//		if (pstUnit->pcsCurrentCell)
//		{
//			AgsmAOICell	*pcsCell	= pstUnit->pcsCurrentCell;
//			//pcsCell->m_Mutex.WLock();
//			EnterCriticalSection(&pcsCell->m_csCriticalSection);
//			pcsCell->RemoveCharacter(pstUnit);
//			LeaveCriticalSection(&pcsCell->m_csCriticalSection);
//			//pcsCell->m_Mutex.Release();
//		}
//	}

	pcsCurrentCell->m_csRWLock.LockWriter();

	BOOL	bRetval	= pcsCurrentCell->AddNPC(pstUnit);

	pcsCurrentCell->m_csRWLock.UnlockWriter();

	if (!bRetval)
		return NULL;

	return pcsCurrentCell;
}

BOOL AgsmAOIMap::AddMonsterToMap(AuPOS *pstPos, AgsmCellUnit *pstUnit, BOOL bIsSiegeWarCharacter)
{
	if (!pstPos || !pstUnit)
		return FALSE;

	AgsmAOICell	*pcsCell	= GetCell(pstPos);
	if (!pcsCell)
		return FALSE;

	pcsCell->m_csRWLock.LockWriter();

	BOOL	bRetval	= pcsCell->AddMonster(pstUnit, bIsSiegeWarCharacter);

	pcsCell->m_csRWLock.UnlockWriter();

	return bRetval;
}

BOOL AgsmAOIMap::RemoveMonsterFromMap(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (!pstUnit->pcsCurrentCell)
		return FALSE;

	AgsmAOICell	*pcsCell	= pstUnit->pcsCurrentCell;

	pcsCell->m_csRWLock.LockWriter();

	BOOL	bRetval	= pcsCell->RemoveMonster(pstUnit);

	pcsCell->m_csRWLock.UnlockWriter();

	return bRetval;
}

AgsmAOICell* AgsmAOIMap::UpdateMonsterPosition(AuPOS *pstCurrentPos, AgsmCellUnit *pstUnit)
{
	if (!pstCurrentPos || !pstUnit)
		return NULL;

	AgsmAOICell	*pcsCurrentCell		= GetCell(pstCurrentPos);
	AgsmAOICell *pcsPrevCell		= pstUnit->pcsCurrentCell;

	if (pcsCurrentCell == pcsPrevCell)
		return NULL;

	if (pcsPrevCell)
	{
		pcsPrevCell->m_csRWLock.LockWriter();

		pcsPrevCell->RemoveMonster(pstUnit);

		pcsPrevCell->m_csRWLock.UnlockWriter();
	}
//	else
//	{
//		if (pstUnit->pcsCurrentCell)
//		{
//			AgsmAOICell	*pcsCell	= pstUnit->pcsCurrentCell;
//			//pcsCell->m_Mutex.WLock();
//			EnterCriticalSection(&pcsCell->m_csCriticalSection);
//			pcsCell->RemoveCharacter(pstUnit);
//			LeaveCriticalSection(&pcsCell->m_csCriticalSection);
//			//pcsCell->m_Mutex.Release();
//		}
//	}

	pcsCurrentCell->m_csRWLock.LockWriter();

	BOOL	bRetval	= pcsCurrentCell->AddMonster(pstUnit);

	pcsCurrentCell->m_csRWLock.UnlockWriter();

	if (!bRetval)
		return NULL;

	return pcsCurrentCell;
}

BOOL AgsmAOIMap::AddItemToMap(AuPOS *pstPos, AgsmCellUnit *pstUnit)
{
	if (!pstPos || !pstUnit)
		return FALSE;

	AgsmAOICell	*pcsCell	= GetCell(pstPos);
	if (!pcsCell)
		return FALSE;

	pcsCell->m_csRWLock.LockWriter();
	BOOL	bRetval	= pcsCell->AddItem(pstUnit);
	pcsCell->m_csRWLock.UnlockWriter();

	return bRetval;
}

BOOL AgsmAOIMap::RemoveItemFromMap(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (!pstUnit->pcsCurrentCell)
		return FALSE;

	AgsmAOICell	*pcsCell	= pstUnit->pcsCurrentCell;

	pcsCell->m_csRWLock.LockWriter();

	BOOL	bRetval	= pcsCell->RemoveItem(pstUnit);

	pcsCell->m_csRWLock.UnlockWriter();

	return bRetval;
}

/*
AgsmAOICell* AgsmAOIMap::GetCell(AuPOS *pstPos)
{
	if (!pstPos)
		return NULL;

	return GetCell(CellIndexToArrayIndexX(PosToCellIndexX(pstPos->x)), CellIndexToArrayIndexZ(PosToCellIndexZ(pstPos->z)));
}

AgsmAOICell* AgsmAOIMap::GetCell(INT32 lIndexX, INT32 lIndexZ)
{
	if (lIndexX < 0 || lIndexZ < 0 ||
		lIndexX >= AGSMAOIMAP_WORLD_INDEX_WIDTH ||
		lIndexZ >= AGSMAOIMAP_WORLD_INDEX_HEIGHT)
		return NULL;

	if (!m_ppCell[lIndexX][lIndexZ])
	{
		m_ppCell[lIndexX][lIndexZ]	= new (AgsmAOICell);
		if (!m_ppCell[lIndexX][lIndexZ])
			return NULL;

		m_ppCell[lIndexX][lIndexZ]->Initialize(lIndexX, lIndexZ);
	}

	return m_ppCell[lIndexX][lIndexZ];
}
*/

BOOL AgsmAOIMap::NoticeActiveStatus(AgsmAOICell *pcsCell)
{
	AgsmAOICell		*pcsAroundCell	= NULL;
	
	INT32	lIndexX	= pcsCell->GetIndexX();
	INT32	lIndexZ	= pcsCell->GetIndexZ();

	for (int i = lIndexX - 3; i <= lIndexX + 3; ++i)
	{
		for (int j = lIndexZ - 3; j <= lIndexZ + 3; ++j)
		{
			// 셀을 가져와 패킷을 보낸다.
			pcsAroundCell	= GetCell(i, j);
			if (!pcsAroundCell)
				continue;

			pcsAroundCell->m_csRWLock.LockWriter();
			pcsAroundCell->AddActiveCellCount();
			pcsAroundCell->m_csRWLock.UnlockWriter();
		}
	}

	return TRUE;
}

BOOL AgsmAOIMap::NoticeDisableStatus(AgsmAOICell *pcsCell)
{
	AgsmAOICell		*pcsAroundCell	= NULL;
	
	INT32	lIndexX	= pcsCell->GetIndexX();
	INT32	lIndexZ	= pcsCell->GetIndexZ();

	for (int i = lIndexX - 3; i <= lIndexX + 3; ++i)
	{
		for (int j = lIndexZ - 3; j <= lIndexZ + 3; ++j)
		{
			// 셀을 가져와 패킷을 보낸다.
			pcsAroundCell	= GetCell(i, j);
			if (!pcsAroundCell)
				continue;

			pcsAroundCell->m_csRWLock.LockWriter();
			pcsAroundCell->SubActiveCellCount();
			pcsAroundCell->m_csRWLock.UnlockWriter();
		}
	}

	return TRUE;
}