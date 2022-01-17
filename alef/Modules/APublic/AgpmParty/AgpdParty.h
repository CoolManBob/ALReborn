/******************************************************************************
Module:  AgpdParty.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 01. 27
******************************************************************************/

#if !defined(__AGPDPARTY_H__)
#define __AGPDPARTY_H__

#include "ApBase.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"

typedef enum _eAGPMPARTY_DATA_TYPE {
	AGPMPARTY_DATA_TYPE_PARTY			= 0,
} eAGPMPARTY_DATA_TYPE;

typedef enum	_AgpmPartyCalcExpType {
//	AGPMPARTY_EXP_TYPE_BY_NONE				= 0,
	AGPMPARTY_EXP_TYPE_BY_DAMAGE			=	0,
	AGPMPARTY_EXP_TYPE_BY_LEVEL,
//	AGPMPARTY_EXP_TYPE_BY_COMPLEX,
} AgpmPartyCalcExpType;

//	2005.04.15	Start By SungHoon
//	파티옵션중 아이템획득에 관한 Option Enum
typedef enum	_AgpmPartyOptionDivisionItem {
//	AGPMPARTY_DIVISION_ITEM_NONE			= 0,
	AGPMPARTY_DIVISION_ITEM_SEQUENCE		=	0,				// 순차적 획득
	AGPMPARTY_DIVISION_ITEM_FREE,								// 자유 획득
	AGPMPARTY_DIVISION_ITEM_DAMAGE								// 자유 획득
} AgpmPartyOptionDivisionItem;
//	2005.04.15	Finish By SungHoon

//	AgpdParty class (data only class)
///////////////////////////////////////////////////////////////////////////////
class AgpdParty : public ApBase {
public:
	INT16					m_nMaxMember;			// 최대 파티 참여 가능 인원
	INT16					m_nCurrentMember;		// 현재 파티 참여 인원

	// 제일 앞에 있는 넘이 리더가 된당.
//	INT32					m_lMemberListID[AGPMPARTY_MAX_PARTY_MEMBER];
//	AgpdCharacter*			m_pcsMemberList[AGPMPARTY_MAX_PARTY_MEMBER];

	ApSafeArray<INT32, AGPMPARTY_MAX_PARTY_MEMBER>				m_lMemberListID;
	//ApSafeArray<AgpdCharacter*, AGPMPARTY_MAX_PARTY_MEMBER>		m_pcsMemberList;

	INT32					m_lHighestMemberLevel;	// 이 파티 멤버중 레벨이 젤 높은넘의 레벨
	INT32					m_lLowerMemberLevel;	// 이 파티 멤버중 레벨이 젤 낮은넘의 레벨
	INT32					m_lTotalMemberLevel;	// 이 파티 멤버들의 레벨 총합

	INT32					m_lHighestMemberUnionRank;	// 이 파티 멤버중 유니온랭크가 젤 높은넘의 랭크

	UINT32					m_ulRemoveTimeMSec;

	//BOOL					m_bIsInEffectArea[AGPMPARTY_MAX_PARTY_MEMBER];
	ApSafeArray<BOOL, AGPMPARTY_MAX_PARTY_MEMBER>				m_bIsInEffectArea;

	AgpmPartyCalcExpType	m_eCalcExpType;

//	2005.04.15	Start By SungHoon
//	아이템 순차적 획득에 필요한 변수
	INT32					m_lCurrentGetItemCID;
	INT32					m_lFinishGetItemCID;

	AgpmPartyOptionDivisionItem	m_eDivisionItem;
//	2005.04.15	Finish
//
};

//	AgpdPartyADChar class (data class)
//		- character 모듈에 붙여 관리할 데이타
///////////////////////////////////////////////////////////////////////////////
class AgpdPartyADChar {
public:
	INT32		lPID;								// party id
	AgpdParty	*pcsParty;							// party data pointer

	AgpdFactor	m_csFactorPoint;
};

#endif //__AGPDPARTY_H__
