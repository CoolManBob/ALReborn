// AgpdQustFlag.cpp: implementation of the AgpdQustFlag class.
//
//////////////////////////////////////////////////////////////////////

#include "AgpdQuestFlag.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgpdQuestFlag::AgpdQuestFlag()
{
	InitFlags();
}

AgpdQuestFlag::~AgpdQuestFlag()
{

}

void AgpdQuestFlag::InitFlags()
{
//	memset(m_btQuest, 0x01, sizeof(m_btQuest));
	// 남영의 단순 실수에 의해서 0x02로 해야된다.
	// 8의 배수를 빼야 됐지만 8의 배수 + 1로해서 문제가 되었다.
	memset(m_btQuestBlock1, DEFAULT_FLAG, sizeof(m_btQuestBlock1));
	memset(m_btQuestBlock2, DEFAULT_FLAG, sizeof(m_btQuestBlock2));		//AGSDQUEST_EXPAND_BLOCK
	memset(m_btQuestBlock3, DEFAULT_FLAG, sizeof(m_btQuestBlock3));		//AGSDQUEST_EXPAND_BLOCK
}

INT32 AgpdQuestFlag::GetBlockIndex(INT32 lQuestDBID)
{
	return (lQuestDBID / (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS));
}

BOOL AgpdQuestFlag::MemoryCopy(BYTE* pMemory, INT32 lBlockIndex)
{
	ASSERT(pMemory);

	switch (lBlockIndex)
	{
	case 0: memcpy(m_btQuestBlock1, pMemory, AGSDQUEST_MAX_FLAG); break;
	case 1: memcpy(m_btQuestBlock2, pMemory, AGSDQUEST_MAX_FLAG); break;
	case 2: memcpy(m_btQuestBlock3, pMemory, AGSDQUEST_MAX_FLAG); break;
	default:
		{
			ASSERT(!"index error");
			return FALSE;
		}
		break;
	};
	
	return TRUE;
}


// ID가 유효한 범위내에 있는지 확인
BOOL AgpdQuestFlag::IsValid(INT32 lQuestDBID)
{
// AGSDQUEST_EXPAND_BLOCK
	if ((lQuestDBID < (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS * AGSDQUEST_BLOCK_COUNT)) && (lQuestDBID > 0))
		return TRUE;
	else
	{
		ASSERT(!"Quest TID가 유효한 값이 아닙니다.");
		return FALSE;
	}
}

BYTE* AgpdQuestFlag::GetBlockMemory(INT32 lBlockIndex)
{
	switch (lBlockIndex)
	{
	case 0:	return m_btQuestBlock1;
	case 1: return m_btQuestBlock2;
	case 2: return m_btQuestBlock3;
	default:
		ASSERT(!"error index");
		break;
	};

	return NULL;
}

// ID에 해당하는 Bit의 값을 가져온다.
BOOL AgpdQuestFlag::GetValueByDBID(INT32 lQuestDBID)
{
	if (FALSE == IsValid(lQuestDBID))
		return FALSE;

	BYTE* pBlock = GetBlockMemory(GetBlockIndex(lQuestDBID));
	if (NULL == pBlock)
		return FALSE;

	INT32 OffsetID = lQuestDBID % (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS);

	INT32 lQuotient	= OffsetID / AGSDQUEST_BITS;
	INT32 lRemainder = OffsetID % AGSDQUEST_BITS;

	BYTE  btTargetByte = pBlock[lQuotient];
	btTargetByte >>= lRemainder;

	return (btTargetByte & 0x01);
}

// ID에 해당하는 Bit의 값을 설정
BOOL AgpdQuestFlag::SetValueByDBID(INT32 lQuestDBID, BOOL bMaster)
{
	if (!IsValid(lQuestDBID)) 
		return FALSE;

	BYTE  btTargetByte;
	BYTE *pBlock = GetBlockMemory(GetBlockIndex(lQuestDBID));
	if (NULL == pBlock)
		return FALSE;

	INT32 OffsetID = lQuestDBID % (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS);

	INT32 lQuotient	= OffsetID / AGSDQUEST_BITS;
	INT32 lRemainder = OffsetID % AGSDQUEST_BITS;

	if (bMaster)
	{
		// 특정 비트를 1로 설정할때
		btTargetByte = bMaster;
		btTargetByte <<= lRemainder;
		pBlock[lQuotient] |= btTargetByte;
	}
	else
	{
		// 특정 비트를 0으로 설정할때(Toggle이 아니면 다음과 같이 해야된다.)
		btTargetByte = TRUE;
		btTargetByte <<= lRemainder;
		btTargetByte ^= 0xFF;
		pBlock[lQuotient] &= btTargetByte;
	}

	return TRUE;
}


/*
BOOL AgpdQuestFlag::SetValueByDBID(INT32 lQuestDBID, BOOL bMaster)
{
	if (!IsValid(lQuestDBID)) return FALSE;

	BYTE  btTargetByte;
	INT32 lQuotient		= lQuestDBID / AGSDQUEST_BITS;
	INT32 lRemainder	= lQuestDBID % AGSDQUEST_BITS;
	if (bMaster)
	{
		// 특정 비트를 1로 설정할때
		btTargetByte = bMaster;
		btTargetByte <<= lRemainder;
		m_btQuest[lQuotient] |= btTargetByte;
	}
	else
	{
		// 특정 비트를 0으로 설정할때(Toggle이 아니면 다음과 같이 해야된다.)
		btTargetByte = TRUE;
		btTargetByte <<= lRemainder;
		btTargetByte ^= 0xFF;
		m_btQuest[lQuotient] &= btTargetByte;
	}

	return TRUE;
}

BOOL AgpdQuestFlag::MemoryCopy(BYTE* pMemory, BYTE* pMemorySE)
{
	memcpy(m_btQuest, pMemory, AGSDQUEST_MAX_FLAG);
#ifdef AGSDQUEST_ADD_BLOCK
	memcpy(m_btQuestSE, pMemorySE, AGSDQUEST_MAX_FLAG);
#endif
	return TRUE;
}

// ID가 유효한 범위내에 있는지 확인
BOOL AgpdQuestFlag::IsValid(INT32 lQuestDBID)
{
#ifdef AGSDQUEST_ADD_BLOCK
	if ((lQuestDBID < (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS * AGSDQUEST_BLOCK_COUNT)) && (lQuestDBID > 0))
		return TRUE;
	else
	{
		ASSERT(!"Quest TID가 유효한 값이 아닙니다.");
		return FALSE;
	}
#else
	if ((lQuestDBID < (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS)) && (lQuestDBID > 0))
		return TRUE;
	else
	{
		ASSERT(!"Quest TID가 유효한 값이 아닙니다.");
		return FALSE;
	}
#endif
}

BOOL AgpdQuestFlag::IsValid(INT32 lQuestDBID)
{
	if ((lQuestDBID < (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS)) && (lQuestDBID > 0))
		return TRUE;
	else
	{
		ASSERT(!"Quest TID가 유효한 값이 아닙니다.");
		return FALSE;
	}
}

BOOL AgpdQuestFlag::GetValueByDBID(INT32 lQuestDBID)
{
	if (IsValid(lQuestDBID))
	{
		INT32 lQuotient		= lQuestDBID / AGSDQUEST_BITS;
		INT32 lRemainder	= lQuestDBID % AGSDQUEST_BITS;
		BYTE  btTargetByte	= m_btQuest[lQuotient];

		btTargetByte >>= lRemainder;

		return (btTargetByte & 0x01);
	} 
	else {return FALSE;}
}

void AgpdQuestFlag::InitFlags()
{
//	memset(m_btQuest, 0x01, sizeof(m_btQuest));
	// 남영의 단순 실수에 의해서 0x02로 해야된다.
	// 8의 배수를 빼야 됐지만 8의 배수 + 1로해서 문제가 되었다.
	memset(m_btQuest, DEFAULT_FLAG, sizeof(m_btQuest));

}
*/
