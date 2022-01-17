/******************************************************************************
Module:  AgpdItem.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 15
******************************************************************************/

#if !defined(__AGPDITEM_H__)
#define __AGPDITEM_H__

#include "ApModule.h"

#include "AgpmGrid.h"

#include "ApdItem.h"
#include "ApBase.h"
#include "AgpmFactors.h"

#include "AgpmCharacter.h"
#include "AgpdCharacter.h"

#include "AgpdItemTemplate.h"

#include "AgpdItemOptionTemplate.h"

#define AGPDITEM_MAKE_ITEM_ID(lIID,lITID)	((((INT64) (lITID)) << 32) | (lIID))
#define AGPDITEM_GET_IID(llID)				((INT32) ((llID) & 0xffffffff))
#define AGPDITEM_GET_ITID(llID)				((INT32) (((llID) >> 32) & 0xffffffff))

// AGPDITEM_CASH_ITEM_XXX
#define	AGPDITEM_CASH_ITEM_UNUSE				0
#define	AGPDITEM_CASH_ITEM_INUSE				1
#define AGPDITEM_CASH_ITEM_PAUSE				2

#define AGPDITEM_PRIVATE_TRADE_OPTION_MAX		3

#define	AGPDITEM_CASH_ITEM_MAX_USABLE_COUNT		0x0FFFFFFF
// Constant Values
//const INT32 AGPMITEM_MAX_ITEM_NAME						= 40;
const INT32 AGPMITEM_MAX_ITEM_MAGIC_ATTR				= 22;
const INT32 AGPMITEM_MAX_ITEM_SPIRITED					= 10;
const INT32 AGPMITEM_MAX_ITEM_DATA_COUNT				= 1000000;
const INT32 AGPMITEM_MAX_ITEM_OWN						= 100;		// 한 캐릭터가 최대로 가질 수 있는 아템 수

const INT32 AGPMITEM_MAX_PICKUP_ITEM_DISTANCE			= 400;

//Equip
const INT32 AGPMITEM_EQUIP_LAYER						= 2;
const INT32 AGPMITEM_EQUIP_ROW							= 3;
const INT32 AGPMITEM_EQUIP_COLUMN						= 4;

//Inventory
const INT32 AGPMITEM_MAX_INVENTORY						= 4;
const INT32 AGPMITEM_INVENTORY_ROW						= 4;
const INT32 AGPMITEM_INVENTORY_COLUMN					= 4;

const INT32 AGPAITEM_MAX_ITEM_TEMPLATE					= 256;

const INT32 AGPMITEM_BTREE_ORDER						= 30;

const INT32 AGPMITEM_INVALID_IID						= 0;

const INT32 AGPMITEM_SALES_BOX_SIZE						= 10;

//Bank
const INT32	AGPMITEM_BANK_MAX_LAYER						= 4;
const INT32 AGPMITEM_BANK_ROW							= 7;
const INT32 AGPMITEM_BANK_COLUMN						= 4;

//Trade Box(1:1교환)
const INT32 AGPMITEM_TRADEBOX_LAYER						= 1; //기획의 요청에 따라 얼마든지 변경하자~ ㅎㅎ
const INT32 AGPMITEM_TRADEBOX_ROW						= 3; //기획의 요청에 따라 얼마든지 변경하자~ ㅎㅎ
const INT32 AGPMITEM_TRADEBOX_COLUMN					= 4; //기획의 요청에 따라 얼마든지 변경하자~ ㅎㅎ

//SalesBox
const INT32 AGPMITEM_SALES_LAYER						= 1; //기획의 요청에 따라 얼마든지 변경하자~ ㅎㅎ
const INT32 AGPMITEM_SALES_ROW							= 1; //기획의 요청에 따라 얼마든지 변경하자~ ㅎㅎ
const INT32 AGPMITEM_SALES_COLUMN						= 10; //기획의 요청에 따라 얼마든지 변경하자~ ㅎㅎ

//NPCTradeBox
const INT32 AGPMITEM_NPCTRADEBOX_LAYER					= 1; //기획의 요청에 따라 얼마든지 변경하자~ ㅎㅎ
const INT32 AGPMITEM_NPCTRADEBOX_ROW					= 20; //기획의 요청에 따라 얼마든지 변경하자~ ㅎㅎ
const INT32 AGPMITEM_NPCTRADEBOX_COLUMN					= 4; //기획의 요청에 따라 얼마든지 변경하자~ ㅎㅎ

//Quest Grid
const INT32 AGPMITEM_QUEST_LAYER						= 1;
const INT32 AGPMITEM_QUEST_ROW							= 4;
const INT32	AGPMITEM_QUEST_COLUMN						= 4;

//2005.11.16. By SungHoon for cash inventory
const INT32 AGPMITEM_CASH_INVENTORY_LAYER				= 1;
const INT32 AGPMITEM_CASH_INVENTORY_ROW					= 200;
const INT32	AGPMITEM_CASH_INVENTORY_COLUMN				= 1;

const INT32 AGPMITEM_GUILD_WAREHOUSE_LAYER				= 2;
const INT32 AGPMITEM_GUILD_WAREHOUSE_ROW				= 7;
const INT32	AGPMITEM_GUILD_WAREHOUSE_COLUMN				= 4;

// 2008.05.08. steeple
const INT32 AGPMITEM_SUB_LAYER							= 4;
const INT32 AGPMITEM_SUB_ROW							= 2;
const INT32 AGPMITEM_SUB_COLUMN							= 4;

/*
// 2008.11.07. iluvs for Unseen Inventory
const INT32 AGPMITEM_UNSEEN_LAYER						= 1;
const INT32 AGPMITEM_UNSEEN_ROW							= 2;
const INT32 AGPMITEM_UNSEEN_COLUMN						= 4;
*/

const INT32	AGPMITEM_MAX_USE_ITEM						= 20;	// 캐릭터가 동시에 사용할 수 있는 최대 아템수

const INT32	AGPMITEM_MAX_CONVERT						= 10;
const INT32	AGPMITEM_MAX_CONVERT_WEAPON					= 10;	// 공격 무기인경우 정령석으로 개조할 수 있는 최대횟수
const INT32	AGPMITEM_MAX_CONVERT_ARMOUR					= 5;	// 방어구인경우 정령석으로 개조할 수 있는 최대횟수

const INT32 AGPMITEM_MAX_DURABILITY						= 100;	// 최대 내구도

const INT32 AGPMITEM_DEFAULT_INVEN_NUM					= 5;	// 캐릭터가 기본으로 가지고 있는 인벤속 아이템 배열 갯수

const INT32	AGPMITEM_REQUIRE_LEVEL_UP_STEP				= 2;

const INT32 AGPMITEM_MAX_DELETE_REASON					= 32;

const INT32 AGPMITEM_TRANSFORM_RECAST_TIME				= 600000;

typedef enum _eAgpdItemDefaultInvenIndex
{
	AGPDITEM_DEFAULT_INVEN_INDEX_TID = 0,
	AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT,
	AGPDITEM_DEFAULT_INVEN_INDEX_NUM
} eAgpdItemDefaultInvenIndex;

typedef enum _eAgpdItemMaterial
{
	AGPDITEM_MATERIAL_NUM
} eAgpdItemMaterial;

typedef enum AgpdItemGridPos
{
	AGPDITEM_GRID_POS_TAB = 0,
	AGPDITEM_GRID_POS_ROW,
	AGPDITEM_GRID_POS_COLUMN,
	AGPDITEM_GRID_POS_NUM
} AgpdItemGridPos;

typedef enum AgpdItemTradeStatus
{
	AGPDITEM_TRADE_STATUS_NONE = 0,            //거래상태가 아님.
	AGPDITEM_TRADE_STATUS_WAIT_CONFIRM,        //거래하자고 요청을하고 응답을 기다리거나 답해야하는 상황
	AGPDITEM_TRADE_STATUS_TRADING,             //거래중(아이템을 올리고 내리고....), Unlock상태
	AGPDITEM_TRADE_STATUS_LOCK,				   //Lock된경우.
	AGPDITEM_TRADE_STATUS_READY_TO_EXCHANGE    //거래준비 완료( 아이템을 다 올리고 ok버튼을 누른 상태)
} AgpdItemTradeStatus;

//typedef enum _AgpmItemConvertResult
//{
//	AGPMITEM_CONVERT_FAIL					= 0,
//	AGPMITEM_CONVERT_DIFFER_RANK,
//	AGPMITEM_CONVERT_DIFFER_TYPE,
//	AGPMITEM_CONVERT_FULL,
//	AGPMITEM_CONVERT_NOT_EQUIP_ITEM,
//	AGPMITEM_CONVERT_EGO_ITEM,
//	AGPMITEM_CONVERT_DESTROY_ATTRIBUTE,
//	AGPMITEM_CONVERT_DESTROY_ITEM,
//	AGPMITEM_CONVERT_SUCCESS,
//} AgpmItemConvertResult;

//typedef enum _AgpmItemEgoResult
//{
//	AGPMITEM_EGO_PUT_SOUL_SUCCESS	= 0,
//	AGPMITEM_EGO_PUT_SOUL_FAIL,
//	AGPMITEM_EGO_USE_SOULCUBE_SUCCESS,
//	AGPMITEM_EGO_USE_SOULCUBE_FAIL,
//} AgpmItemEgoResult;


typedef enum	_AgpmItemBoundTypes {
	E_AGPMITEM_NOT_BOUND				= 0,
	E_AGPMITEM_BIND_ON_ACQUIRE,
	E_AGPMITEM_BIND_ON_EQUIP,
	E_AGPMITEM_BIND_ON_USE,
	E_AGPMITEM_BIND_ON_GUILDMASTER		// 길드 마스터만 획득 가능
} AgpmItemBoundTypes;

//	Define item status flag
//////////////////////////////////////////////////////////////////

//	BoundTypes
//////////////////////////////////////////////////////////////////
#define	AGPMITEM_BIND_ON_ACQUIRE						0x00000001
#define	AGPMITEM_BIND_ON_EQUIP							0x00000002
#define	AGPMITEM_BIND_ON_USE							0x00000004
#define	AGPMITEM_BOUND_ON_OWNER							0x00000008

//	QuestType
//////////////////////////////////////////////////////////////////
#define	AGPMITEM_STATUS_QUEST							0x00000010

//	Define item status flag
//////////////////////////////////////////////////////////////////



//typedef	struct _stAgpmItemConvertHistory
//{
//	INT32					lConvertLevel;
//
//	FLOAT					fConvertConstant;
//	
//	BOOL					bUseSpiritStone[AGPMITEM_MAX_CONVERT];
//	
//	INT32					lTID[AGPMITEM_MAX_CONVERT];
//	ApBase					*pcsTemplate[AGPMITEM_MAX_CONVERT];
//
//	AgpmItemRuneAttribute	eRuneAttribute[AGPMITEM_MAX_CONVERT];
//	INT32					lRuneAttributeValue[AGPMITEM_MAX_CONVERT];
//
//	AgpdFactor				csFactorHistory[AGPMITEM_MAX_CONVERT];
//	AgpdFactor				csFactorPercentHistory[AGPMITEM_MAX_CONVERT];
//} stAgpmItemConvertHistory;

class AgpdItem;

typedef struct _stAgpmItemCheckPickupItem
{
	AgpdItem	*pcsItem;
	AgpdCharacter	*pcsCharacter;
	INT32		lStackCount;
} stAgpmItemCheckPickupItem, *pstAgpmItemCheckPickupItem;

typedef struct _stAgpmItemSkullInfo
{
	CHAR		szSkullOwner[AGPACHARACTER_MAX_ID_STRING + 1];
	CHAR		szKiller[AGPACHARACTER_MAX_ID_STRING + 1];
	//INT32		lPartyMemberID[AGPMPARTY_MAX_PARTY_MEMBER];
	ApSafeArray<INT32, AGPMPARTY_MAX_PARTY_MEMBER>	lPartyMemberID;

	INT32		lSkullUnionRank;
	INT32		lKillerUnionRank;
} AgpmItemSkullInfo;


//아이템을 그리드에 넣기전에 원래 있던 그리드의 위치 정보
//Balanced BST가 완성되면 나중에 교체하자~ 우선은 배열로..... ㅠ.ㅠ
class AgpmGridItemOriginInfo
{
public:
	INT64			m_llItemID;
	INT16			m_ePrevStatus;// 이 아템의 상태... (착용, 인벤토리, 땅바닦 등등.)
	//INT16			m_anGridPos[AGPDITEM_GRID_POS_NUM];
	ApSafeArray<INT16, AGPDITEM_GRID_POS_NUM>		m_anGridPos;

	void Set( INT64 llItemID, AgpdItemStatus eStatus, INT16 nLayer, INT16 nRow, INT16 nColumn );
	void Reset();
};

class AgpdItemADCharTemplate : public ApBase {
public:
	//INT32			m_lDefaultEquipITID[AGPMITEM_PART_NUM];
	ApSafeArray<INT32, AGPMITEM_PART_NUM>			m_lDefaultEquipITID;
	INT32			m_aalDefaultInvenInfo[AGPMITEM_DEFAULT_INVEN_NUM][AGPDITEM_DEFAULT_INVEN_INDEX_NUM];

	INT32			m_lDefaultMoney;
};

// 2008.02.14. steeple
struct AgpdItemCooldownBase
{
	INT32			m_lTID;
	UINT32			m_ulRemainTime;
	BOOL			m_bPause;
	BOOL			m_bPrevPause;

	AgpdItemCooldownBase() :
		m_lTID(0), m_ulRemainTime(0), m_bPause(0), m_bPrevPause(0)
	{;}

	bool operator == (INT32 lTID)
	{
		if(m_lTID == lTID)
			return true;

		return false;
	}
};

struct AgpdItemCooldownArray
{
	typedef vector<AgpdItemCooldownBase>			CooldownBaseVector;
	typedef vector<AgpdItemCooldownBase>::iterator	CooldownBaseIter;

	CooldownBaseVector* m_pBases;

	AgpdItemCooldownArray()
	{
		m_pBases = NULL;
	}
};

struct AgpdPrivateOptionItem
{
	INT32		m_lItemTID;
	INT32		m_lItemIID;
	BOOL		m_bUsed;
	BOOL		m_bOnGrid;
	
	AgpdPrivateOptionItem()
	{
		m_lItemTID = 0;
		m_lItemIID = 0;
		m_bUsed = FALSE;
		m_bOnGrid = FALSE;
	}
};

struct AgpdSealData
{
	CHAR strCharName[AGPDCHARACTER_NAME_LENGTH+1];
	INT32 TID;
	INT32 Level;
	INT64 Exp;
}; 

typedef struct
{
//public:
	INT16		m_nNumItem;							// 갖고 있는 아이템 총 갯수
//	INT16		m_nNewPart;							// 새로 바뀐 Item Part (Equip Callback에 사용) Remove 081002 Bob Jung.-필요없음.
//	PVOID		m_pvItemData[AGPMITEM_MAX_ITEM_OWN];	// 아이템 데이타 포인터

	AgpdGrid		m_csInventoryGrid;			// 인벤토리
	AgpdGrid		m_csEquipGrid;				// Equip
	AgpdGrid		m_csSalesBoxGrid;			// SalesBox
	AgpdGrid		m_csBankGrid;				// Bank
	AgpdGrid		m_csTradeGrid;				// 자신의 Trade Grid.
	AgpdGrid		m_csClientTradeGrid;		// 거래자의 Trade Grid.
	AgpdGrid		m_csQuestGrid;				// Quest Item 보관용 Grid
	AgpdGrid		m_csCashInventoryGrid;		// CashItem 인벤토리
	AgpdGrid		m_csSubInventoryGrid;		// PetItem 인벤토리
//	AgpdGrid		m_csUnseenInventoryGrid;	// Unseen 인벤토리

	INT32			m_lTradeTargetID;			//누구와 거래중인가?
	INT32			m_lTradeStatus;				//거래상태.
	INT32			m_lMoneyCount;				//내가 상대방에게 줄 금액
	INT32			m_lClientMoneyCount;		//상대방이 내게 줄 금액.
	bool			m_bConfirmButtonDown;		//내 컴펌버튼이 눌렸는지를 알아내는 멤버
	bool			m_bTargetConfirmButtonDown; //상대의 컴펌버튼이 눌렸는지를 알아내는 멤버

	INT32			m_lArrowIDInInventory;		// 인벤토리안에 있는 화살 아템 아뒤 (화살을 찾기위해 매번 인벤토리를 검색할 수는 없다.)
	INT32			m_lBoltIDInInventory;		// 인벤토리안에 있는 화살 아템 아뒤 (화살을 찾기위해 매번 인벤토리를 검색할 수는 없다.)

	INT32			m_lNumArrowCount;			// 인벤토리안에 있는 총 화살 카운트
	INT32			m_lNumBoltCount;			// 인벤토리안에 있는 총 볼트 카운트

	INT32			m_lHPPotionIDInInventory;	// 인벤토리안에 있는 HP 물약 아뒤
	INT32			m_lMPPotionIDInInventory;	// 인벤토리안에 있는 MP 물약 아뒤
	INT32			m_lSPPotionIDInInventory;	// 인벤토리안에 있는 SP 물약 아뒤

	BOOL			m_bUseReturnTeleportScroll;	// 왕복 문서를 사용했는지 여부
	AuPOS			m_stReturnPosition;			// 왕복 문서를 사용한 지점. 되돌아올때 이자리로 돌아온다.

	UINT32			m_ulUseHPPotionTime;		// positon을 마지막으로 사용한 시간
	UINT32			m_ulUseMPPotionTime;		// positon을 마지막으로 사용한 시간
	UINT32			m_ulUseSPPotionTime;		// positon을 마지막으로 사용한 시간

	UINT32			m_ulUseReverseOrbTime;		// reverse orb 를 마지막으로 사용한 시간
	UINT32			m_ulUseTransformTime;		// transform item 을 마지막으로 사용한 시간

	UINT32			m_ulUseJumpTime;			// 마지막으로 jump 아이템 사용한 시간

	UINT32			m_ulUseChattingEmphasis;	// 마지막으로 성언술 아이템 사용한 시간

	AgpdItemCooldownArray	m_CooldownInfo;		// ItemTID 별 cooldown 정보.

	// 개인거래시 옵션아이템 
	AgpdPrivateOptionItem	m_stTradeOptionItem[AGPDITEM_PRIVATE_TRADE_OPTION_MAX];
	AgpdPrivateOptionItem	m_stClientTradeOptionItem[AGPDITEM_PRIVATE_TRADE_OPTION_MAX];

} AgpdItemADChar;

class AgpdItem : public ApdItem {
public:
	AgpdItemTemplate*			m_pcsItemTemplate;

	INT32						m_nDimension	;		// 계 인덱스.

	INT32						m_ulCID;
	AgpdCharacter *				m_pcsCharacter;
	INT32						m_lPrevOwner;
	//INT8						m_nColor[2];
	ApSafeArray<INT8, 2>		m_nColor;
	//BOOL						m_bStackable;
	INT32						m_nCount;
	//INT16						m_nKeeping;
	AuPOS						m_posItem;
	//FLOAT						m_fMagicAttr[AGPMITEM_MAX_ITEM_MAGIC_ATTR];
	//INT16						m_nSpirited[AGPMITEM_MAX_ITEM_SPIRITED];
	//INT16						m_nEgoExp;
	//INT16						m_nEgoLevel;
	//INT16						m_nDurability;

	AgpdFactor					m_csFactor;
	AgpdFactor					m_csFactorPercent;
	AgpdFactor					m_csRestrictFactor;

	INT16						m_eNewStatus;						// 이 아템이 가질 상태 (init 상태에서) (2006/2/23)... (착용, 인벤토리, 땅바닦 등등.)
	INT16						m_eStatus;							// 이 아템의 상태... (착용, 인벤토리, 땅바닦 등등.)

	INT32						m_lStatusFlag;

	AgpdCharacter				*m_pcsBoundOwner;

	ApBase						*m_pcsGuildWarehouseBase;

	//INT16						m_anGridPos[AGPDITEM_GRID_POS_NUM];
	ApSafeArray<INT16, AGPDITEM_GRID_POS_NUM>	m_anGridPos;
	ApSafeArray<INT16, AGPDITEM_GRID_POS_NUM>	m_anPrevGridPos;

//	stAgpmItemConvertHistory	m_stConvertHistory;

	// ego item
	CHAR						m_szSoulMasterID[AGPACHARACTER_MAX_ID_STRING + 1];		// 아템 주인 아뒤

	AgpmItemSkullInfo*			m_pstSkullInfo;						// 이 아템이 유골인 경우만 메모리 할당해서 사용한다.

	AgpdGridItem*				m_pcsGridItem;

	UINT32						m_ulRemoveTimeMSec;		// 이 아템이 삭제된 클럭카운트

	//INT32						m_lDeleteReason;		// 이 아템을 DB에서 삭제하는 경우 삭제하라고 한곳에서 세팅한 이유
	CHAR						m_szDeleteReason[AGPMITEM_MAX_DELETE_REASON + 1];

	INT32						m_lSkillTID;			// skill book(skill scroll) 인 경우 이 아템을 사용해서 배울 수 있는 스킬 TID 세팅
	INT32						m_lSkillLevel;			// skill scroll 인 경우 이 스킬 레벨

	ApSafeArray<UINT16, AGPDITEM_OPTION_MAX_NUM>		m_aunOptionTID;
	ApSafeArray<UINT16, AGPDITEM_OPTION_RUNE_MAX_NUM>	m_aunOptionTIDRune;
	ApSafeArray<AgpdItemOptionTemplate *, AGPDITEM_OPTION_MAX_NUM>		m_apcsOptionTemplate;
	ApSafeArray<AgpdItemOptionTemplate *, AGPDITEM_OPTION_RUNE_MAX_NUM>	m_apcsOptionTemplateRune;

	ApSafeArray<UINT16, AGPMITEM_MAX_SKILL_PLUS_EFFECT>	m_aunSkillPlusTID;		// 2007.02.05. steeple

//	2005.11.30. By SungHoon	For CashItem
	INT8						m_nInUseItem;			//	사용중인지
	INT64						m_lRemainTime;			//	남은 사용기간, milli-seconds
	UINT32						m_lExpireTime;			//	완료시간, seconds, time_t

	INT32						m_lCashItemUseCount;	//	사용횟수

	INT64						m_llStaminaRemainTime;	// A remaining time of staminas		2008.06.09. steeple
};


#endif //__AGPDITEM_H__
