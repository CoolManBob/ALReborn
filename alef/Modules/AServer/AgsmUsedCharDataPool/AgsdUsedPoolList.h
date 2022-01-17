#pragma once

#include "AgpdCharacter.h"

typedef struct _AgsdUsedPoolList
{
	AgpdCharacter	*pNextTIDData;
	AgpdCharacter	*pPrevTIDData;

	AgpdCharacter	*pPrevRemoveData;
	AgpdCharacter	*pNextRemoveData;

	BOOL			m_bIsAddedUsedPool;
} AgsdUsedPoolList;