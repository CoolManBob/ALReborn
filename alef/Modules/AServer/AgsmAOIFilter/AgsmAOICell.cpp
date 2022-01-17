#include "AgsmAOICell.h"

AgsmAOICell::AgsmAOICell()
{
	Initialize();
}

AgsmAOICell::~AgsmAOICell()
{
	Destroy();
}

BOOL AgsmAOICell::Initialize(INT32 lIndexX, INT32 lIndexZ)
{
	m_pstCharacterUnit	= NULL;
	m_pstNPCUnit		= NULL;
	m_pstMonsterUnit	= NULL;
	m_pstItemUnit		= NULL;

	m_ulGroupID			= (UINT_PTR) this;

	m_lIndexX			= lIndexX;
	m_lIndexZ			= lIndexZ;

	m_lNumCharacterUnit	= 0;
	m_lNumNPCUnit		= 0;
	m_lNumMonsterUnit	= 0;
	m_lNumItemUnit		= 0;

	m_lNumPCUnit		= 0;

	m_lNumAroundActiveCell	= 0;

	return TRUE;
}

BOOL AgsmAOICell::Destroy()
{
	/*
	if (m_pstCharacterUnit)
	{
		AgsmCellUnit	*pUnit		= NULL;
		AgsmCellUnit	*pNextUnit	= NULL;

		for( pUnit = m_pstCharacterUnit ; pUnit ; pUnit = pNextUnit )
		{
			pNextUnit = pUnit->pNext;

			delete pUnit;
		}

		m_pstCharacterUnit	= NULL;
	}

	if (m_pstItemUnit)
	{
		AgsmCellUnit	*pUnit		= NULL;
		AgsmCellUnit	*pNextUnit	= NULL;

		for( pUnit = m_pstItemUnit ; pUnit ; pUnit = pNextUnit )
		{
			pNextUnit = pUnit->pNext;

			delete pUnit;
		}

		m_pstItemUnit	= NULL;
	}
	*/

	return TRUE;
}

BOOL AgsmAOICell::AddCharacter(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (pstUnit->pcsCurrentCell)
		return FALSE;

	if (!CheckGuardByte(pstUnit))
		return FALSE;

	pstUnit->pcsCurrentCell	= this;
	pstUnit->pNext			= m_pstCharacterUnit;
	pstUnit->pPrev			= NULL;

	if (m_pstCharacterUnit)
		m_pstCharacterUnit->pPrev	= pstUnit;

	m_pstCharacterUnit		= pstUnit;

	++m_lNumCharacterUnit;

	//if (pstUnit->bIsPC)
		++m_lNumPCUnit;

	return TRUE;
}

BOOL AgsmAOICell::RemoveCharacter(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (!CheckGuardByte(pstUnit))
		return FALSE;

	if (pstUnit->pNext)
		pstUnit->pNext->pPrev	= pstUnit->pPrev;

	if (pstUnit->pPrev)
		pstUnit->pPrev->pNext	= pstUnit->pNext;
	else
		m_pstCharacterUnit	= pstUnit->pNext;

	pstUnit->pcsCurrentCell	= NULL;
	pstUnit->pNext			= NULL;
	pstUnit->pPrev			= NULL;

	--m_lNumCharacterUnit;

	//if (pstUnit->bIsPC)
		--m_lNumPCUnit;

	return TRUE;
}

BOOL AgsmAOICell::AddNPC(AgsmCellUnit *pstUnit, BOOL bIsInSiegeWarCharacter)
{
	if (!pstUnit)
		return FALSE;

	if (pstUnit->pcsCurrentCell)
		return FALSE;

	if (!CheckGuardByte(pstUnit))
		return FALSE;

	pstUnit->pcsCurrentCell	= this;
	pstUnit->pNext			= m_pstNPCUnit;
	pstUnit->pPrev			= NULL;

	if (m_pstNPCUnit)
		m_pstNPCUnit->pPrev	= pstUnit;

	m_pstNPCUnit		= pstUnit;

	m_bIsInSiegeWarCharacter	= bIsInSiegeWarCharacter;

	++m_lNumNPCUnit;

	return TRUE;
}

BOOL AgsmAOICell::RemoveNPC(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (!CheckGuardByte(pstUnit))
		return FALSE;

	if (pstUnit->pNext)
		pstUnit->pNext->pPrev	= pstUnit->pPrev;

	if (pstUnit->pPrev)
		pstUnit->pPrev->pNext	= pstUnit->pNext;
	else
		m_pstNPCUnit	= pstUnit->pNext;

	pstUnit->pcsCurrentCell	= NULL;
	pstUnit->pNext			= NULL;
	pstUnit->pPrev			= NULL;

	--m_lNumNPCUnit;

	return TRUE;
}

BOOL AgsmAOICell::AddMonster(AgsmCellUnit *pstUnit, BOOL bIsInSiegeWarCharacter)
{
	if (!pstUnit)
		return FALSE;

	if (pstUnit->pcsCurrentCell)
		return FALSE;

	if (!CheckGuardByte(pstUnit))
		return FALSE;

	pstUnit->pcsCurrentCell	= this;
	pstUnit->pNext			= m_pstMonsterUnit;
	pstUnit->pPrev			= NULL;

	if (m_pstMonsterUnit)
		m_pstMonsterUnit->pPrev	= pstUnit;

	m_pstMonsterUnit		= pstUnit;

	m_bIsInSiegeWarCharacter	= bIsInSiegeWarCharacter;

	++m_lNumMonsterUnit;

	return TRUE;
}

BOOL AgsmAOICell::RemoveMonster(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (!CheckGuardByte(pstUnit))
		return FALSE;

	if (pstUnit->pNext)
		pstUnit->pNext->pPrev	= pstUnit->pPrev;

	if (pstUnit->pPrev)
		pstUnit->pPrev->pNext	= pstUnit->pNext;
	else
		m_pstMonsterUnit	= pstUnit->pNext;

	pstUnit->pcsCurrentCell	= NULL;
	pstUnit->pNext			= NULL;
	pstUnit->pPrev			= NULL;

	--m_lNumMonsterUnit;

	return TRUE;
}

BOOL AgsmAOICell::AddItem(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (pstUnit->pcsCurrentCell)
		return FALSE;

	if (!CheckGuardByte(pstUnit))
		return FALSE;

	pstUnit->pcsCurrentCell	= this;
	pstUnit->pNext			= m_pstItemUnit;
	pstUnit->pPrev			= NULL;

	if (m_pstItemUnit)
		m_pstItemUnit->pPrev	= pstUnit;

	m_pstItemUnit			= pstUnit;

	++m_lNumItemUnit;

	return TRUE;
}

BOOL AgsmAOICell::RemoveItem(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (!CheckGuardByte(pstUnit))
		return FALSE;

	if (pstUnit->pNext)
		pstUnit->pNext->pPrev	= pstUnit->pPrev;

	if (pstUnit->pPrev)
		pstUnit->pPrev->pNext	= pstUnit->pNext;
	else
		m_pstItemUnit	= pstUnit->pNext;

	pstUnit->pcsCurrentCell	= NULL;
	pstUnit->pNext			= NULL;
	pstUnit->pPrev			= NULL;

	--m_lNumItemUnit;

	return TRUE;
}

VOID AgsmAOICell::RestoreCharacterHeadList()
{
	if (m_pstCharacterUnit && m_pstCharacterUnit->pPrev)
		m_pstCharacterUnit->pPrev	= NULL;
}

VOID AgsmAOICell::RestoreNPCHeadList()
{
	if (m_pstNPCUnit && m_pstNPCUnit->pPrev)
		m_pstNPCUnit->pPrev	= NULL;
}

VOID AgsmAOICell::RestoreMonsterHeadList()
{
	if (m_pstMonsterUnit && m_pstMonsterUnit->pPrev)
		m_pstMonsterUnit->pPrev	= NULL;
}

VOID AgsmAOICell::RestoreItemHeadList()
{
	if (m_pstItemUnit && m_pstItemUnit->pPrev)
		m_pstItemUnit->pPrev	= NULL;
}

VOID AgsmAOICell::RestoreList(AgsmCellUnit *pstUnit)
{
	if (pstUnit &&
		pstUnit->pNext)
	{
		if (pstUnit->pNext->pPrev != pstUnit ||
			pstUnit->pNext->pcsCurrentCell != pstUnit->pcsCurrentCell)
			pstUnit->pNext	= NULL;
	}
}

AgsmCellUnit* AgsmAOICell::GetNext(AgsmCellUnit *pstUnit)
{
	if (pstUnit)
	{
		if (!CheckGuardByte(pstUnit))
			return NULL;

		RestoreList(pstUnit);
		return pstUnit->pNext;
	}

	return NULL;
}

UINT_PTR AgsmAOICell::GetGroupID()
{
	return m_ulGroupID;
}

VOID AgsmAOICell::SetGroupID(UINT_PTR ulGroupID)
{
	m_ulGroupID = ulGroupID;
}

INT32 AgsmAOICell::GetIndexX()
{
	return m_lIndexX;
}

INT32 AgsmAOICell::GetIndexZ()
{
	return m_lIndexZ;
}

AgsmCellUnit* AgsmAOICell::GetCharacterUnit()
{
	RestoreCharacterHeadList();

	if (!CheckGuardByte(m_pstCharacterUnit))
		return NULL;

	return m_pstCharacterUnit;
}

AgsmCellUnit* AgsmAOICell::GetNPCUnit()
{
	RestoreNPCHeadList();

	if (!CheckGuardByte(m_pstNPCUnit))
		return NULL;

	return m_pstNPCUnit;
}

AgsmCellUnit* AgsmAOICell::GetMonsterUnit()
{
	RestoreMonsterHeadList();

	if (!CheckGuardByte(m_pstMonsterUnit))
		return NULL;

	return m_pstMonsterUnit;
}

AgsmCellUnit* AgsmAOICell::GetItemUnit()
{
	RestoreItemHeadList();

	if (!CheckGuardByte(m_pstItemUnit))
		return NULL;

	return m_pstItemUnit;
}

INT32 AgsmAOICell::GetNumCharacterUnit()
{
	return m_lNumCharacterUnit;
}

INT32 AgsmAOICell::GetNumNPCUnit()
{
	return m_lNumNPCUnit;
}

INT32 AgsmAOICell::GetNumMonsterUnit()
{
	return m_lNumMonsterUnit;
}

INT32 AgsmAOICell::GetNumItemUnit()
{
	return m_lNumItemUnit;
}

INT32 AgsmAOICell::GetNumPCUnit()
{
	return m_lNumPCUnit;
}

VOID AgsmAOICell::AddActiveCellCount()
{
	++m_lNumAroundActiveCell;

	ASSERT(m_lNumAroundActiveCell <= 49);
}

VOID AgsmAOICell::SubActiveCellCount()
{
	--m_lNumAroundActiveCell;

	ASSERT(m_lNumAroundActiveCell >= 0);
}

/*
BOOL AgsmAOICell::IsAnyActiveCell()
{
	if (m_lNumAroundActiveCell > 0)
		return TRUE;

	return FALSE;
}
*/

BOOL AgsmAOICell::CheckGuardByte(AgsmCellUnit *pstUnit)
{
	if (!pstUnit)
		return FALSE;

	if (pstUnit->GuardByte1 != 0 ||
		pstUnit->GuardByte2 != 0 ||
		pstUnit->GuardByte3 != 0)
	{
		if (!pstUnit->bWriteLog)
		{
			if (g_eServiceArea == AP_SERVICE_AREA_KOREA)
			{
				CHAR	*pszCrash	= NULL;
				*pszCrash	= 0;
			}

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "GuardByte Error\n   GuardByte1 : %I64d\n   GuardByte2 : %I64d\n   GuardByte3 : %I64d\n",
					  pstUnit->GuardByte1,
					  pstUnit->GuardByte2,
					  pstUnit->GuardByte3);
			AuLogFile_s("LOG\\CellUnitErr.log", strCharBuff);

			pstUnit->bWriteLog	= TRUE;
		}

		return FALSE;
	}

	return TRUE;
}