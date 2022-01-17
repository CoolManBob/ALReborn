/******************************************************************************
Module:  AgsdItem.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 07. 29
******************************************************************************/

#if !defined(__AGSDITEM_H__)
#define __AGSDITEM_H__

#include "AgpmItem.h"
#include "AgsdAccount.h"

typedef enum _eAgsdItemPauseReason
{
	AGSDITEM_PAUSE_REASON_NONE = 0,
	AGSDITEM_PAUSE_REASON_INVISIBLE,
	AGSDITEM_PAUSE_REASON_RIDE,
	AGSDITEM_PAUSE_REASON_TRANSFORM,
	AGSDITEM_PAUSE_REASON_JAIL,
	AGSDITEM_PAUSE_REASON_EVOLUTION,
} eAgsdItemPauseReason;

class AgsdItem {
public:
	UINT64		m_ullDBIID;				// DB에 들어가있는 아템 아뒤

	UINT32		m_ulOwnTime;			// 이 아템을 획득한 시간

	INT32		m_lUseItemRemain;		// 아템이 use 상태인경우, 그리고 몇번에 걸쳐 사용되는 경우 남아있는 사용횟수
	UINT32		m_ulNextUseItemTime;	// 위에서 사용되는 횟수가 남아있는경우 다음 사용시간

	// item drop & loot
	UINT32		m_ulDropTime;			// 아템을 떨군 시간
	ApBaseExLock	m_csFirstLooterBase;	// 루팅에 우선권을 갖고 있는 캐릭터

	BOOL		m_bIsNeedInsertDB;		// DB에 저장을 해야 하는 넘인지 여부
										// DB에 저장한후 이값을 FALSE로 세팅해야 한다.

	INT32		m_lPrevSaveStackCount;	// 

	eAgsdItemPauseReason	m_ePauseReason;	// 어떤 원인으로 인해서 아이템이 정지 되었는지. 주로 NONE 이다.
	UINT32		m_ulPausedClock;

	stOptionSkillData		m_stOptionSkillData;	// 2007.05.04. steeple 

	//BOOL		m_bUseOnEnterGameWorld;	// EnterGameWorld 시에 자동으로 사용되는 아이템인지 (예:이미 사용중인 캐쉬아이템) 2005.12.13. steeple
};

class AgsdItemADChar 
{
public:
	ApSafeArray<INT32, AGPMITEM_MAX_USE_ITEM>		m_lUseItemID;
	INT32		m_lNumUseItem;
	
	BOOL		m_bUseTimeLimitItem;
	UINT32		m_ulTimeLimitItemCheckClock;

	ApSafeArray<PVOID, AGPMITEM_PART_NUM>		m_pvPacketEquipItem;
	ApSafeArray<INT16, AGPMITEM_PART_NUM>		m_lPacketEquipItemLength;

	// 변신한 경우 세팅한다.
	INT32		m_lAddMaxHP;
	INT32		m_lAddMaxMP;
};

typedef enum _eAgsdItemCashItemBuyListStatus {
	AGSDITEM_CASHITEMBUYLIST_STATUS_REQUEST,
	AGSDITEM_CASHITEMBUYLIST_STATUS_COMPLETE,
	AGSDITEM_CASHITEMBUYLIST_STATUS_DRAWBACK,
	AGSDITEM_CASHITEMBUYLIST_STATUS_GMGIVE,
	//AGSDITEM_CASHITEMBUYLIST_STATUS_EXHAUST,
	} eAgsdItemCashItemBuyListStatus;

typedef struct _stCashItemBuyList {
	UINT64		m_ullBuyID;
	INT32		m_lItemTID;
	INT32		m_lItemQty;
	UINT64		m_ullItemSeq;
	INT64		m_llMoney;
	UINT64		m_ullOrderNo;
	INT8		m_cStatus;
	
	CHAR			m_szCharName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	CHAR			m_szAccName[AGSMACCOUNT_MAX_ACCOUNT_NAME + 1];
	CHAR			m_szIP[15];
	INT32			m_lCharTID;
	INT32			m_lCharLevel;
	
	CHAR			m_szOrderID[32 + 1];
} stCashItemBuyList;

struct AgsdSealData
{
	UINT64 DBID;
	AgpdSealData SealData;
};

#endif //__AGSDITEM_H__
