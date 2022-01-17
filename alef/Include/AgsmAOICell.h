#ifndef	__AGSMAOICELL_H__
#define	__AGSMAOICELL_H__

#include "ApmMap.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"

class AgsmAOICell;

typedef struct AgsmCellUnit {
	INT_PTR			GuardByte1;
	INT_PTR			lID;
	INT_PTR			GuardByte2;
	AgsmAOICell		*pcsCurrentCell;
	INT_PTR			GuardByte3;

	AgsmCellUnit	*pNext;
	AgsmCellUnit	*pPrev;
	BOOL			bIsPC;
	UINT32			ulNID;

	BOOL			bWriteLog;
} AgsmCellUnit;

class AgsmAOICell {
private:
	AgsmCellUnit	*m_pstCharacterUnit;
	AgsmCellUnit	*m_pstNPCUnit;
	AgsmCellUnit	*m_pstMonsterUnit;
	AgsmCellUnit	*m_pstItemUnit;

	UINT_PTR		m_ulGroupID;

	INT32			m_lIndexX;
	INT32			m_lIndexZ;

	INT32			m_lNumCharacterUnit;
	INT32			m_lNumNPCUnit;
	INT32			m_lNumMonsterUnit;
	INT32			m_lNumItemUnit;

	INT32			m_lNumPCUnit;

	INT32			m_lNumAroundActiveCell;
	BOOL			m_bIsInSiegeWarCharacter;

public:
	//ApCriticalSection	m_csCellLock;
	ApRWLock		m_csRWLock;

public:
	AgsmAOICell();
	~AgsmAOICell();

	BOOL			Initialize(INT32 lIndexX = 0, INT32 lIndexZ = 0);
	BOOL			Destroy();

	BOOL			AddCharacter(AgsmCellUnit *pstUnit);
	BOOL			RemoveCharacter(AgsmCellUnit *pstUnit);

	BOOL			AddNPC(AgsmCellUnit *pstUnit, BOOL bIsInSiegeWarCharacter = FALSE);
	BOOL			RemoveNPC(AgsmCellUnit *pstUnit);

	BOOL			AddMonster(AgsmCellUnit *pstUnit, BOOL bIsInSiegeWarCharacter = FALSE);
	BOOL			RemoveMonster(AgsmCellUnit *pstUnit);

	BOOL			AddItem(AgsmCellUnit *pstUnit);
	BOOL			RemoveItem(AgsmCellUnit *pstUnit);

	VOID			RestoreCharacterHeadList();
	VOID			RestoreNPCHeadList();
	VOID			RestoreMonsterHeadList();
	VOID			RestoreItemHeadList();

	VOID			RestoreList(AgsmCellUnit *pstUnit);

	AgsmCellUnit	*GetNext(AgsmCellUnit *pstUnit);

	UINT_PTR		GetGroupID();
	VOID			SetGroupID(UINT_PTR ulGroupID);

	INT32			GetIndexX();
	INT32			GetIndexZ();

	AgsmCellUnit*	GetCharacterUnit();
	AgsmCellUnit*	GetNPCUnit();
	AgsmCellUnit*	GetMonsterUnit();
	AgsmCellUnit*	GetItemUnit();

	INT32			GetNumCharacterUnit();
	INT32			GetNumNPCUnit();
	INT32			GetNumMonsterUnit();
	INT32			GetNumItemUnit();
	INT32			GetNumPCUnit();

	VOID			AddActiveCellCount();
	VOID			SubActiveCellCount();

	inline BOOL			IsAnyActiveCell()
	{
		if (m_lNumAroundActiveCell > 0 || m_bIsInSiegeWarCharacter)
			return TRUE;

		return FALSE;
	}

	BOOL			CheckGuardByte(AgsmCellUnit *pstUnit);
};


#endif	//__AGSMAOICELL_H__