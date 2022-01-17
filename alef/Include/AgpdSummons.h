// AgpdSummons.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2005. 07. 26.

#ifndef _AGPDSUMMONS_H_
#define _AGPDSUMMONS_H_

#include "ApBase.h"
#include "AgpdCharacter.h"

const INT32 AGPMSUMMONS_MAX_SUMMONS			= 5;		// 소환수 최대 개수
const INT32 AGPMSUMMONS_MAX_TAME			= 10;		// 테이밍 최대 개수
const INT32 AGPMSUMMONS_MAX_FIXED			= 5;		// 고정형 소환수 최대 개수
const INT32 AGPMSUMMONS_MAX_SUMMONS_START	= 1;		// 기본으로 시작하는 개수. 스킬로 조정된다.

enum EnumAgpdSummonsType
{
	AGPMSUMMONS_TYPE_NONE		= 0,
	AGPMSUMMONS_TYPE_ELEMENTAL	= (1 << 0),
	AGPMSUMMONS_TYPE_TAME		= (1 << 1),
	AGPMSUMMONS_TYPE_FIXED		= (1 << 2),
	AGPMSUMMONS_TYPE_PET		= (1 << 3),				// 2007.03.20. steeple
	AGPMSUMMONS_TYPE_PET2		= (1 << 4),				// 2008.05.08. steeple
	AGPMSUMMONS_TYPE_MONSTER	= (1 << 5),				
};

enum EnumAgpdSummonsPropensity
{
	AGPMSUMMONS_PROPENSITY_ATTACK	= 0,
	AGPMSUMMONS_PROPENSITY_DEFENSE	= (1 << 0),
	AGPMSUMMONS_PROPENSITY_SHADOW	= (1 << 1),
};

enum EnumAgpdSummonsStatus
{
	AGPMSUMMONS_STATUS_NORMAL		= 0,
	AGPMSUMMONS_STATUS_HUNGRY		= (1 << 0),
};

struct AgpdSummons
{
	INT32						m_lCID;
	INT32						m_lTID;
	EnumAgpdSummonsType			m_eType;
	EnumAgpdSummonsPropensity	m_ePropensity;
	EnumAgpdSummonsStatus		m_eStatus;
	UINT32						m_ulStartTime;
	UINT32						m_ulEndTime;

	bool operator == (INT32 lCID)
	{
		if (m_lCID == lCID)
			return true;

		return false;
	}
};

struct AgpdSummonsArray
{
	// 2006.10.26. steeple
	// 기존의 배열을 vector 로 완전 변경.
	// 많은 수의 Summons 을 소환하는 스킬이 생겨서...

	typedef vector<AgpdSummons>					SummonsVector;
	typedef vector<AgpdSummons>::iterator		iterator;

	INT8 m_cMaxNum;
	SummonsVector* m_pSummons;

	//typedef ApSafeArray<AgpdSummons, AGPMSUMMONS_MAX_SUMMONS>::iterator iterator;
	//ApVector<AgpdSummons, AGPMSUMMONS_MAX_SUMMONS> m_arrSummons;
};

struct AgpdTameArray
{
	typedef ApSafeArray<AgpdSummons, AGPMSUMMONS_MAX_TAME>::iterator iterator;

	INT8 m_cMaxNum;
	ApVector<AgpdSummons, AGPMSUMMONS_MAX_TAME> m_arrTame;
};

struct AgpdFixedArray
{
	typedef ApSafeArray<AgpdSummons, AGPMSUMMONS_MAX_FIXED>::iterator iterator;

	INT8 m_cMaxNum;
	ApVector<AgpdSummons, AGPMSUMMONS_MAX_FIXED> m_arrFixed;
};

// 메모리를 줄이기 위해서 아래와 같이 함.
struct AgpdSummonsADChar
{
	AgpdSummonsArray m_SummonsArray;			// For Owner - 소환수를 관리
	AgpdTameArray m_TameArray;					// For Owner - 테이밍 몬스터를 관리
	AgpdFixedArray m_FixedArray;				// For Owner - 고정형 소환수를 관리

	INT32 m_lOwnerCID;							// For Summons - 나를 소환한 주인의 CID
	INT32 m_lOwnerTID;							// For Summons - 나를 소환한 주인의 TID
	AgpdSummons m_stSummonsData;				// For Summons - 내가 소환되었을때 필요한 정보들(소환 시간 등)
};

#endif //_AGPDSUMMONS_H_
