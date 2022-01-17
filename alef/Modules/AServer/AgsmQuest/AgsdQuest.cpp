#include "AgsdQuest.h"

AgsdQuest::AgsdQuest()
{
	::ZeroMemory(&m_btQuest, sizeof(m_btQuest));
// AGSDQUEST_EXPAND_BLOCK
	::ZeroMemory(&m_btQuestSE, sizeof(m_btQuest));
}

// ID가 유효한 범위내에 있는지 확인
BOOL AgsdQuest::IsValid(INT32 lQuestDBID)
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

// ID에 해당하는 Bit의 값을 가져온다.
BOOL AgsdQuest::GetValueByDBID(INT32 lQuestDBID)
{
	BYTE  btTargetByte = 0x00;
	INT32 lRemainder = 0x00;
	INT32 lQuotient = 0x00;
// AGSDQUEST_EXPAND_BLOCK
	UINT8 bBlockIndex = 1; 
	if (IsValid(lQuestDBID))
	{
		if( lQuestDBID >= (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS))	
		{
			lQuestDBID -= (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS);
			bBlockIndex = 2;
		}
		lQuotient		= lQuestDBID / AGSDQUEST_BITS;
		lRemainder	= lQuestDBID % AGSDQUEST_BITS;
		
		if( 1 == bBlockIndex)
			btTargetByte	= m_btQuest[lQuotient];
		else
			btTargetByte	= m_btQuestSE[lQuotient];

		btTargetByte >>= lRemainder;

		return (btTargetByte & 0x01);
	} 
	else {return FALSE;}
}

// ID에 해당하는 Bit의 값을 설정
BOOL AgsdQuest::SetValueByDBID(INT32 lQuestDBID, BOOL bMaster)
{
	if (!IsValid(lQuestDBID)) return FALSE;

	BYTE  btTargetByte;
// AGSDQUEST_EXPAND_BLOCK
	UINT8 bBlockIndex = 1;
	if( lQuestDBID >= (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS))	// DBID 가 2048 이상이면	
	{
		lQuestDBID -= (AGSDQUEST_MAX_FLAG * AGSDQUEST_BITS);	// DBID 가 2048 이상이면 - 2048
		bBlockIndex = 2;										// 2번째 블럭에 셋팅
	}
	INT32 lQuotient		= lQuestDBID / AGSDQUEST_BITS;
	INT32 lRemainder	= lQuestDBID % AGSDQUEST_BITS;

	if (bMaster)
	{
		// 특정 비트를 1로 설정할때
		btTargetByte = bMaster;
		btTargetByte <<= lRemainder;
		if( 1 == bBlockIndex)
			m_btQuest[lQuotient] |= btTargetByte;
		else
			m_btQuestSE[lQuotient] |= btTargetByte;
	}
	else
	{
		// 특정 비트를 0으로 설정할때(Toggle이 아니면 다음과 같이 해야된다.)
		btTargetByte = TRUE;
		btTargetByte <<= lRemainder;
		btTargetByte ^= 0xFF;
		if( 1 == bBlockIndex)
			m_btQuest[lQuotient] &= btTargetByte;
		else
			m_btQuestSE[lQuotient] &= btTargetByte;
	}
	return TRUE;
}
/*
// ID에 해당하는 Bit의 값을 설정
BOOL AgsdQuest::SetValueByDBID(INT32 lQuestDBID, BOOL bMaster)
{
	if (IsValid(lQuestDBID))
	{
		INT32 lQuotient		= lQuestDBID / AGSDQUEST_BITS;
		INT32 lRemainder	= lQuestDBID % AGSDQUEST_BITS;

		BYTE btTargetByte = bMaster;
		btTargetByte <<= lRemainder;

		m_btQuest[lQuotient] |= btTargetByte;

		return TRUE;
	}
	else {return FALSE;}
}
*/

/*
AgsdQuest::AgsdQuestProxy::AgsdQuestProxy(BYTE* pByte, INT32 lQuestID, BOOL bValid) 
	: m_pByte(pByte), m_lQuestID(lQuestID), m_bValid(bValid) 
{
}

AgsdQuest::AgsdQuestProxy AgsdQuest::operator[](INT32 lQuestID)
{
	return AgsdQuestProxy(&m_btQuest[lQuestID/AgsdQuest_BITS], lQuestID, IsValid(lQuestID));
}

AgsdQuest::AgsdQuestProxy& AgsdQuest::AgsdQuestProxy::operator=(const AgsdQuest::AgsdQuestProxy& rProxy)
{
	*this = (BOOL)rProxy;
	return *this;
}

AgsdQuest::AgsdQuestProxy& AgsdQuest::AgsdQuestProxy::operator=(BOOL bMaster)
{
	BYTE  btTargetByte;
	if (bMaster)
	{
		// 특정 비트를 1로 설정할때
		btTargetByte = bMaster;
		btTargetByte <<= m_lQuestID % AgsdQuest_BITS;
		*m_pByte |= btTargetByte;
	}
	else
	{
		// 특정 비트를 0으로 설정할때
		btTargetByte = TRUE;
		btTargetByte <<= m_lQuestID % AgsdQuest_BITS;
		btTargetByte ^= 0xFF;
		*m_pByte &= btTargetByte;
	}

	return *this;
}

AgsdQuest::AgsdQuestProxy::operator BOOL() const
{
	BYTE  btTargetByte	= *m_pByte;
	btTargetByte >>= m_lQuestID % AgsdQuest_BITS;
	return (btTargetByte & 0x01);
}
*/