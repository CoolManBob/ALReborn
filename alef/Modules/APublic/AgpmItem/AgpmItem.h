#if !defined(__AGPMITEM_H__)
#define __AGPMITEM_H__

#include "ApModule.h"
#include "AgpdItem.h"
#include "AgpaItem.h"
#include "ApMutualEx.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgpmFactors.h"
#include "AuPacket.h"
#include "AgpmBillInfo.h"

#include "AgpmSystemMessage.h"

#include "ApmMap.h"

#include "AuRandomNumber.h"

#include "AgpdItemImportData.h"
#include <vector>
#include <AgppItem.h>

#include "AuXmlParser/TinyXml/tinyxml.h"

using namespace std;

enum eAGPM_DURABILITY_EQUIP_PART
{
	AGPM_DURABILITY_EQUIP_PART_HEAD = 0,
	AGPM_DURABILITY_EQUIP_PART_BREAST,
	AGPM_DURABILITY_EQUIP_PART_LEG,
	AGPM_DURABILITY_EQUIP_PART_SLEEVE,
	AGPM_DURABILITY_EQUIP_PART_FOOT,
	AGPM_DURABILITY_EQUIP_PART_SHIELD,
	AGPM_DURABILITY_EQUIP_PART_WEAPON,
	AGPM_DURABILITY_EQUIP_PART_MAX,
};

enum eAGPM_PRIVATE_TRADE_ITEM
{
	AGPM_PRIVATE_TRADE_ITEM_NORMAL = 0,
	AGPM_PRIVATE_TRADE_ITEM_BOUND_ON_OWNER,
	AGPM_PRIVATE_TRADE_ITEM_BOUND_ON_OWNER_ENABLE_TRADE,
};

/*
typedef enum AgpmItemPacketStatus
{
	AGPMITEM_PACKET_STATUS_FIELD = 0,
	AGPMITEM_PACKET_STATUS_INVENTORY,
	AGPMITEM_PACKET_STATUS_BANK,
	AGPMITEM_PACKET_STATUS_EQUIP,
	AGPMITEM_PACKET_STATUS_NOTHING,
	AGPMITEM_PACKET_STATUS_FIELD_ADD = 0,
	AGPMITEM_PACKET_STATUS_FIELD_MOVE,
	AGPMITEM_PACKET_STATUS_FIELD_REMOVE,
	AGPMITEM_PACKET_STATUS_INVENTORY_ADD,
	AGPMITEM_PACKET_STATUS_INVENTORY_MOVE,
	AGPMITEM_PACKET_STATUS_INVENTORY_REMOVE,
	AGPMITEM_PACKET_STATUS_EQUIP_ADD,
	AGPMITEM_PACKET_STATUS_EQUIP_MOVE,
	AGPMITEM_PACKET_STATUS_EQUIP_REMOVE,
	AGPMITEM_PACKET_STATUS_NUM
} AgpmItemPacketStatus;
*/

#define	AGPMITEM_PROCESS_REMOVE_INTERVAL			3000
#define	AGPMITEM_PRESERVE_ITEM_DATA					5000
#define AGPMITEM_REGISTER_FEE_RATIO					0.03

#define	AGPMITEM_INI_NAME_NAME						"Name"
#define	AGPMITEM_INI_NAME_TYPE						"Type"
#define	AGPMITEM_INI_NAME_PART						"Part"
#define	AGPMITEM_INI_NAME_KIND						"Kind"
#define	AGPMITEM_INI_NAME_WeaponType				"WeaponType"
#define AGPMITEM_INI_NAME_TWOHANDS					"TwoHands"
#define	AGPMITEM_INI_NAME_DEFAULT_ITEM				"DefaultITID"
#define	AGPMITEM_INI_NAME_DEFAULT_INVEN_INFO		"DefaultInvenInfo"
#define	AGPMITEM_INI_NAME_DEFAULT_MONEY				"DefaultMoney"
#define AGPMITEM_INI_NAME_SIZE_WIDTH				"SizeW"
#define AGPMITEM_INI_NAME_SIZE_HEIGHT				"SizeH"
#define	AGPMITEM_INI_NAME_RESTRICT_FACTOR			"RestrictFactor"
#define AGPMITEM_INI_NAME_IS_STACKABLE				"IsStackable"
#define AGPMITEM_INI_NAME_MAX_STACKABLE_COUNT		"MaxStackableCount"
#define	AGPMITEM_INI_NAME_LightInfo					"LightInfo"

// usable item
/////////////////////////////////////////////////////////////////////////
#define	AGPMITEM_INI_NAME_USABLE_TYPE				"UsableType"
#define	AGPMITEM_INI_NAME_USE_INTERVAL				"UseInterval"
// potion
#define	AGPMITEM_INI_NAME_EFFECT_FACTOR				"EffectFactor"
#define	AGPMITEM_INI_NAME_EFFECT_ACTIVITY_TIME		"EffectActivityTime"
#define	AGPMITEM_INI_NAME_EFFECT_APPLY_COUNT		"EffectApplyCount"
// spell book
#define	AGPMITEM_INI_NAME_SPELL_NAME				"SpellName"
// spirit stone
#define AGPMITEM_INI_NAME_SPIRIT_STONE_TYPE			"SPIRIT_STONE_TYPE"
// transform
#define	AGPMITEM_INI_NAME_TRANSFORM_DURATION		"TransformDuration"

// other item
////////////////////////////////////////////////////////////////////////
#define AGPMITEM_INI_NAME_OTHER_TYPE				"OtherType"

// rune template
////////////////////////////////////////////////////////////////////////
#define	AGPMITEM_INI_NAME_RUNE_NAME					"RuneName"
#define	AGPMITEM_INI_NAME_RUNE_TYPE					"RuneType"

#define	AGPMITEM_INI_NAME_RUNE_ATTR_DAMAGE			"Damage"
#define	AGPMITEM_INI_NAME_RUNE_ATTR_ATTACK_POINT	"AttackPoint"
#define	AGPMITEM_INI_NAME_RUNE_ATTR_DEFENSE_RATE	"DefenseRate"
#define	AGPMITEM_INI_NAME_RUNE_ATTR_DEFENSE_POINT	"DefensePoint"
#define	AGPMITEM_INI_NAME_RUNE_ATTR_MAX_HP			"MaxHP"
#define	AGPMITEM_INI_NAME_RUNE_ATTR_MAX_MP			"MaxMP"
#define	AGPMITEM_INI_NAME_RUNE_ATTR_MAX_SP			"MaxSP"
#define	AGPMITEM_INI_NAME_RUNE_ATTR_HP_RECOVERY		"HPRecovery"
#define	AGPMITEM_INI_NAME_RUNE_ATTR_MP_RECOVERY		"MPRecovery"
#define	AGPMITEM_INI_NAME_RUNE_ATTR_SP_RECOVERY		"SPRecovery"
#define	AGPMITEM_INI_NAME_RUNE_ATTR_ATTACK_SPEED	"AttackSpeed"


typedef enum AgpmItemPacketOperation
{
	AGPMITEM_PACKET_OPERATION_ADD						= 0,
	AGPMITEM_PACKET_OPERATION_REMOVE,
	AGPMITEM_PACKET_OPERATION_UPDATE,
	AGPMITEM_PACKET_OPERATION_USE_ITEM,
	AGPMITEM_PACKET_OPERATION_USE_ITEM_BY_TID,
	AGPMITEM_PACKET_OPERATION_USE_ITEM_FAILED_BY_TID,
	AGPMITEM_PACKET_OPERATION_USE_RETURN_SCROLL,
	AGPMITEM_PACKET_OPERATION_USE_RETURN_SCROLL_FAILED,
	AGPMITEM_PACKET_OPERATION_CANCEL_RETURN_SCROLL,
	AGPMITEM_PACKET_OPERATION_ENABLE_RETURN_SCROLL,
	AGPMITEM_PACKET_OPERATION_DISABLE_RETURN_SCROLL,
	AGPMITEM_PACKET_OPERATION_PICKUP_ITEM,
	AGPMITEM_PACKET_OPERATION_PICKUP_ITEM_RESULT,
	//AGPMITEM_PACKET_OPERATION_CONVERT_ITEM,
	AGPMITEM_PACKET_OPERATION_EGO_ITEM,
	AGPMITEM_PACKET_OPERATION_STACK_ITEM,
	AGPMITEM_PACKET_OPERATION_DROP_MONEY,
	AGPMITEM_PACKET_OPERATION_USE_ITEM_SUCCESS,
	AGPMITEM_PACKET_OPERATION_SPLIT_ITEM,
	AGPMITEM_PACKET_OPERATION_UPDATE_REUSE_TIME_FOR_REVERSE_ORB,
	AGPMITEM_PACKET_OPERATION_UPDATE_REUSE_TIME_FOR_TRANSFORM,
	AGPMITEM_PACKET_OPERATION_INIT_TIME_FOR_TRANSFORM,
	AGPMITEM_PACKET_OPERATION_REQUEST_DESTROY_ITEM,
	AGPMITEM_PACKET_OPERATION_REQUEST_BUY_BANK_SLOT,
	AGPMITEM_PACKET_OPERATION_UNUSE_ITEM,		// 캐쉬 아이템 사용중지
	AGPMITEM_PACKET_OPERATION_UNUSE_ITEM_FAILED,
	AGPMITEM_PACKET_OPERATION_PAUSE_ITEM,		// 캐쉬 아이템 일시정지. 2006.01.08. steeple
	AGPMITEM_PACKET_OPERATION_UPDATE_ITEM_USE_TIME,	// 아이템의 사용 정보(사용 여부, 남은 시간 등)
	AGPMITEM_PACKET_OPERATION_USE_ITEM_RESULT,
	AGPMITEM_PACKET_OPERATION_UPDATE_COOLDOWN,
	AGPMITEM_PACKET_OPERATION_UPDATE_STAMINA_REMAIN_TIME,		// a remaining stamina time
	AGPMITEM_PACKET_OPERATION_CHANGE_AUTOPICK_ITEM,
	AGPMITEM_PACKET_OPERATION_NUM,
} AgpmItemPacketOperation;

typedef enum _eAddItemInventoryResult
{
	AGPMITEM_AddItemInventoryResult_FALSE = 0,
	AGPMITEM_AddItemInventoryResult_TRUE,
	AGPMITEM_AddItemInventoryResult_RemoveByStack
} eAddItemInventoryResult;

typedef enum _AgpdItemGridResult
{
	AGPDITEM_INSERT_SUCCESS					= 0,
	AGPDITEM_INSERT_SUCCESS_STACKCOUNT_ZERO,
	AGPDITEM_INSERT_NOT_EMPTY,
	AGPDITEM_INSERT_IS_FULL,
	AGPDITEM_INSERT_MAX_STACKCOUNT_OVER,
	AGPDITEM_INSERT_FAIL,
	AGPDITEM_UPDATE_SUCCESS,
	AGPDITEM_UPDATE_FAIL,
	AGPDITEM_UPDATE_NOT_EMPTY,
} AgpdItemGridResult;

typedef enum _AgpmItemUseResult
{
	AGPMITEM_USE_RESULT_FAILED_BY_SIEGEWAR	= 1,
	AGPMITEM_USE_RESULT_CANNOT_TRANSFORM_CREATURE,
	AGPMITEM_USE_RESULT_FAILED_REVERSEORB,
} AgpmItemUseResult;

/*
typedef enum _eAgpmItemPacketConvertOperation
{
	AGPMITEM_PACKET_CONVERT_OPERATION_REQUEST			= 0,
	AGPMITEM_PACKET_CONVERT_OPERATION_ASK_REALLY_CONVERT,
	AGPMITEM_PACKET_CONVERT_OPERATION_SUCCESS,
	AGPMITEM_PACKET_CONVERT_OPERATION_FAIL,
	AGPMITEM_PACKET_CONVERT_OPERATION_DESTROY_ATTRIBUTE,
	AGPMITEM_PACKET_CONVERT_OPERATION_DESTROY_ITEM,
	AGPMITEM_PACKET_CONVERT_OPERATION_DIFFER_RANK,
	AGPMITEM_PACKET_CONVERT_OPERATION_DIFFER_TYPE,
	AGPMITEM_PACKET_CONVERT_OPERATION_FULL,
	AGPMITEM_PACKET_CONVERT_OPERATION_NOT_EQUIP_ITEM,
	AGPMITEM_PACKET_CONVERT_OPERATION_EGO_ITEM,
	AGPMITEM_PACKET_CONVERT_OPERATION_REQUEST_HISTORY,
	AGPMITEM_PACKET_CONVERT_OPERATION_ADD_HISTORY,
	AGPMITEM_PACKET_CONVERT_OPERATION_REMOVE_HISTORY,
	AGPMITEM_PACKET_CONVERT_OPERATION_HISTORY,
	AGPMITEM_PACKET_CONVERT_OPERATION_NUM
} eAgpmItemPacketConvertOperation;

typedef enum _eAgpmItemPacketEgoOperation
{
	AGPMITEM_PACKET_EGO_OPERATION_PUT_SOUL_INTO_CUBE	= 0,
	AGPMITEM_PACKET_EGO_OPERATION_PUT_SOUL_SUCCESS,
	AGPMITEM_PACKET_EGO_OPERATION_PUT_SOUL_FAIL,
	AGPMITEM_PACKET_EGO_OPERATION_USE_SOULCUBE,
	AGPMITEM_PACKET_EGO_OPERATION_USE_SOULCUBE_SUCCESS,
	AGPMITEM_PACKET_EGO_OPERATION_USE_SOULCUBE_FAIL
} eAgpmItemPacketEgoOperation;
*/

typedef enum _AgpmItemPacketPickupItemResult
{
	AGPMITEM_PACKET_PICKUP_ITEM_RESULT_SUCCESS			= 0,
	AGPMITEM_PACKET_PICKUP_ITEM_RESULT_FAIL
} AgpmItemPacketPickupItemResult;

typedef enum _eAGPMITEM_DATA_TYPE {
	AGPMITEM_DATA_TYPE_ITEM								= 0,
	AGPMITEM_DATA_TYPE_TEMPLATE,
	AGPMITEM_DATA_TYPE_RUNE_TEMPLATE,
	AGPMITEM_DATA_TYPE_OPTION_TEMPLATE,
	AGPMITEM_DATA_TYPE_NUM
} eAGPMITEM_DATA_TYPE;

typedef enum _eAGPMITEM_MAGIC_ATTR {
	AGPMITEM_MAGIC_ATTR_AC								= 0,
	AGPMITEM_MAGIC_ATTR_MIN_DMG,
	AGPMITEM_MAGIC_ATTR_MAX_DMG,
	AGPMITEM_MAGIC_ATTR_DEF_RATE,
	AGPMITEM_MAGIC_ATTR_FIRE,
	AGPMITEM_MAGIC_ATTR_WATER,
	AGPMITEM_MAGIC_ATTR_AIR,
	AGPMITEM_MAGIC_ATTR_EARTH,
	AGPMITEM_MAGIC_ATTR_MAGIC,
	AGPMITEM_MAGIC_ATTR_ICE,
	AGPMITEM_MAGIC_ATTR_POISON,
	AGPMITEM_MAGIC_ATTR_LIGHT,
	AGPMITEM_MAGIC_ATTR_STR,
	AGPMITEM_MAGIC_ATTR_DEX,
	AGPMITEM_MAGIC_ATTR_INT,
	AGPMITEM_MAGIC_ATTR_CON,
	AGPMITEM_MAGIC_ATTR_CHA,
	AGPMITEM_MAGIC_ATTR_HP,
	AGPMITEM_MAGIC_ATTR_MP,
	AGPMITEM_MAGIC_ATTR_SP,
	AGPMITEM_MAGIC_ATTR_HP_RECOVERY,
	AGPMITEM_MAGIC_ATTR_MP_RECOVERY,
	AGPMITEM_MAGIC_ATTR_SP_RECOVERY,
	AGPMITEM_MAGIC_ATTR_NUM
} eAGPMITEM_MAGIC_ATTR;

typedef enum _eAGPMITEM_PROCESS_TYPE {
	AGPMITEM_PROCESS_ADD_ITEM,
	AGPMITEM_PROCESS_REMOVE_ITEM,
	AGPMITEM_PROCESS_UPDATE_ITEM,
	AGPMITEM_PROCESS_NUM
} eAGPMITEM_PROCESS_TYPE;

enum eAgpmItemCB_ID				// callback id in item module
{
	ITEM_CB_ID_INIT										= 0,
	ITEM_CB_ID_ADD,
	ITEM_CB_ID_REMOVE,
	ITEM_CB_ID_REMOVE_ID,
	ITEM_CB_ID_DELETE,
	ITEM_CB_ID_CHAR_EQUIP,
	ITEM_CB_ID_CHAR_UNEQUIP,
	ITEM_CB_ID_REMOVE_FOR_NEAR_CHARACTER,
	ITEM_CB_ID_FIELD,
	ITEM_CB_ID_CHAR_ADD_INVENTORY,
	ITEM_CB_ID_CHAR_REMOVE_INVENTORY,
	ITEM_CB_ID_CHAR_BANK,
	ITEM_CB_ID_CHAR_REMOVE_BANK,
	ITEM_CB_ID_CHAR_CHECK_BANK,
	ITEM_CB_ID_QUEST,
	ITEM_CB_ID_ADD_QUEST,
	ITEM_CB_ID_UPDATE_QUEST,
	ITEM_CB_ID_REMOVE_QUEST,
	ITEM_CB_ID_CHECK_QUEST,
	ITEM_CB_ID_ADD_TRADE_GRID,				//TradeGrid Add연산후 부르는 CallBack
	ITEM_CB_ID_REMOVE_TRADE_GRID,			//TradeGrid Remove연산후 부르는 CallBack
	ITEM_CB_ID_ADD_CLIENT_TRADE_GRID,		//ClientTradeGrid Add연산후 부르는 CallBack
	ITEM_CB_ID_REMOVE_CLIENT_TRADE_GRID,	//ClientTradeGrid Remove연산후 부르는 CallBack
	ITEM_CB_ID_UPDATE_FACTOR,
	ITEM_CB_ID_UPDATE_TRADEGRID,
	ITEM_CB_ID_UPDATE_TARGET_TRADEGRID,
	ITEM_CB_ID_CHANGE_ITEM_OWNER,
//	ITEM_CB_ID_SET_POSITION,
//	ITEM_CB_ID_CHAR_THROWITEM,
//	ITEM_CB_ID_CHAR_GETITEM,
	ITEM_CB_ID_USE_ITEM,
	ITEM_CB_ID_USE_ITEM_BY_TID,
	ITEM_CB_ID_USE_ITEM_FAILED_BY_TID,
	ITEM_CB_ID_USE_ITEM_SUCCESS,
	ITEM_CB_ID_UPDATE_RETURN_TELEPORT_STATUS,
	ITEM_CB_ID_USE_RETURN_SCROLL,
	ITEM_CB_ID_USE_RETURN_SCROLL_RESULT_FAILED,
	/*
	ITEM_CB_ID_ASK_REALLY_CONVERT_ITEM,
	ITEM_CB_ID_CONVERT_ITEM,
	ITEM_CB_ID_ADD_CONVERT_HISTORY,
	ITEM_CB_ID_REMOVE_CONVERT_HISTORY,
	ITEM_CB_ID_UPDATE_CONVERT_HISTORY,
	ITEM_CB_ID_SEND_CONVERT_HISTORY,
	*/
	ITEM_CB_ID_CHECK_CAN_PICKUP_ITEM,
	ITEM_CB_ID_PICKUP_ITEM_MONEY,

	ITEM_CB_ID_PICKUP_ITEM_RESULT,

	ITEM_CB_ID_UPDATE_STACK_COUNT,
	
	ITEM_CB_ID_UPDATE_REQUIRE_LEVEL,

	ITEM_CB_ID_PUT_SOUL_INTO_CUBE,
	ITEM_CB_ID_USE_SOULCUBE,
	ITEM_CB_ID_UPDATE_EGO_EXP,
	ITEM_CB_ID_UPDATE_EGO_LEVEL,

	ITEM_CB_ID_ITEM_SLOT_SOUND,				// Item 을 Slot에 넣을때 Sound Play 해주기 위한 콜백
	ITEM_CB_ID_UI_UPDATE_INVENTORY,			// Client Inventory를 갱신
	ITEM_CB_ID_UI_UPDATE_EQUIP,				// Client Inventory를 갱신
	ITEM_CB_ID_UI_UPDATE_TRADE,				// Client 거래창을 갱신
	ITEM_CB_ID_UI_UPDATE_TRADE_CLIENT,		// Client 상대방 거래창을 갱신
	ITEM_CB_ID_UI_UPDATE_TRADE_OPTION,		// Trade Option 아이템 목록 갱신
	ITEM_CB_ID_UI_UPDATE_BANK,				// Bank갱신
	ITEM_CB_ID_UI_UPDATE_SALESBOX,			// SalesBox갱신
	ITEM_CB_ID_UI_UPDATE_SUB_INVENTORY,		// SubInventroy 갱신

	ITEM_CB_ID_UI_UPDATE_TRADE_EX,			// 확장인벤 갱신
	ITEM_CB_ID_UI_UPDATE_TRADE_CLIENT_EX,	// 상대방 확징인벤 갱신

	ITEM_CB_ID_READ_TEMPLATE,				// Template Ini 를 읽고 부른다.

	ITEM_CB_ID_RECEIVE_ITEM_DATA,
	//ITEM_CB_ID_RECEIVE_CONVERT_DATA,

	ITEM_CB_ID_STREAM_READ_IMPORT_DATA_ERROR_REPORT,
	ITEM_CB_ID_DROP_MONEY_TO_FIELD,

	ITEM_CB_ID_CHAR_EQUIP_FOR_ADMIN,
	ITEM_CB_ID_CHAR_ADD_INVENTORY_FOR_ADMIN,

	ITEM_CB_ID_ADD_ITEM_TO_MAP,
	ITEM_CB_ID_REMOVE_ITEM_FROM_MAP,
	ITEM_CB_ID_REQUEST_SPIRITSTONE_CONVERT,
	ITEM_CB_ID_ADD_ITEM_TO_BANK,
	ITEM_CB_ID_REMOVE_ITEM_FROM_BANK,
	ITEM_CB_ID_SPLIT_ITEM,
	ITEM_CB_ID_CHECK_JOIN_ITEM,
	ITEM_CB_ID_ADD_SALESBOX_GRID,				//SalesBox Grid
	ITEM_CB_ID_REMOVE_SALESBOX_GRID,				//SalesBox Grid
	ITEM_CB_ID_UPDATE_REUSETIME_FOR_REVERSE_ORB,
	ITEM_CB_ID_UPDATE_REUSETIME_FOR_TRANSFORM,
	ITEM_CB_ID_INIT_REUSETIME_FOR_TRANSFORM,
	ITEM_CB_ID_UPDATE_ITEM_STATUS_FLAG,

	ITEM_CB_ID_STREAM_READ_IMPORT_DATA,
	ITEM_CB_ID_STREAM_READ_OPTION_TABLE,

	ITEM_CB_ID_SALESBOX_REMOVE_UI_UPDATE,
	ITEM_CB_ID_REMOVE_INVENTORY_GRID,
	ITEM_CB_ID_CHAR_ADD_ITEM_CASH_INVENTORY,
	ITEM_CB_ID_CHAR_REMOVE_ITEM_CASH_INVENTORY,
	ITEM_CB_ID_UI_UPDATE_CASH_INVENTORY,
	ITEM_CB_ID_CHAR_USE_CASH_ITEM,		// 20051201, kelovon
	ITEM_CB_ID_CHAR_UNUSE_CASH_ITEM,	// 20051201, kelovon
	ITEM_CB_ID_CHAR_PAUSE_CASH_ITEM,	// 2006.01.08. steeple
	ITEM_CB_ID_UNUSE_ITEM,
	ITEM_CB_ID_ADD_ITEM_TO_GUILD_WAREHOUSE,
	ITEM_CB_ID_REMOVE_ITEM_FROM_GUILD_WAREHOUSE,
	ITEM_CB_ID_USE_ITEM_RESULT,
	ITEM_CB_ID_REMOVE_GUILD_WAREHOUSE_GRID,
	ITEM_CB_ID_ADD_ITEM_OPTION,
	ITEM_CB_ID_UPDATE_COOLDOWN,

	ITEM_CB_ID_SUB_INVENTORY_CHECK_ADD,
	ITEM_CB_ID_SUB_INVENTORY_ADD,
	ITEM_CB_ID_SUB_INVENTORY_UPDATE,
	ITEM_CB_ID_SUB_INVENTORY_REMOVE,

	ITEM_CB_ID_UNSEEN_INVENTORY_ADD,

	ITEM_CB_ID_GET_PET_TID_BY_ITEM_FROM_SKILL,

	ITEM_CB_ID_CHANGE_AUTOPICK_ITEM,
	ITEM_CB_ID_ADD_ITEM_EXTRADATA,
	ITEM_CB_ID_GET_EXTRADATA,

	ITEM_CB_ID_NUM,
};

#define AGPMITEM_MAX_SKULL_LEVEL		10		// 2005.01.14. steeple.

// Lottery Box 2005.9.2 kelovon
struct AgpdLotteryItemInfo
{
	INT32		nPotItemTID;
	INT32		nMinStackCount;
	INT32		nMaxStackCount;
	INT32		nPercent;
};

// 아바타 세트 아이템 저장용..
struct AgpdAvatarSetItem
{
	INT32	nTID;
	INT32	lBaseTID;
	char	strName[ AGPMITEM_MAX_ITEM_NAME + 1 ];
	vector< INT32 >	vecItemTIDs;

	AgpdAvatarSetItem():nTID ( -1 ), lBaseTID(0)
	{
		strncpy( strName , "" , AGPMITEM_MAX_ITEM_NAME );
	}
};

typedef vector<AgpdAvatarSetItem>					VectorAvatarSetItem;
typedef vector<AgpdAvatarSetItem>::iterator			IterAvatarSetItem;

typedef map<INT32, vector<AgpdLotteryItemInfo> >	MapLotteryBoxToItems;

#define LOTTERY_BOX_POT_ITEM_NOTHING_ITEM_TID		-1
#define LOTTERY_BOX_POT_ITEM_NOTHING_ITEM_STRING	"nothing"

class AgpmItem : public ApModule {
public:
	static const INT32	s_lBeDurabilityEquipItem[AGPM_DURABILITY_EQUIP_PART_MAX];

	ApmMap*				m_papmMap;
	AgpmCharacter*		m_pagpmCharacter;
	AgpmFactors*		m_pagpmFactors;
	AgpmBillInfo*		m_pagpmBillInfo;

	AgpmSystemMessage*	m_pagpmSystemMessage;

	AgpaItem			csItemAdmin;
	AgpaItemTemplate	csTemplateAdmin;
	AgpaItemOptionTemplate	csOptionTemplateAdmin;

	ApAdmin				m_csAdminItemRemove;
	UINT32				m_ulPrevRemoveClockCount;

	INT16				m_nIndexADCharacter;
	// Parn님 작업 CharacterTemplate에 붙인 Data Index
	INT16				m_nIndexADCharacterTemplate;
	//INT32				m_lEquipItemTable[AGPMITEM_PART_NUM];
	ApSafeArray<INT32, AGPMITEM_PART_NUM>	m_lEquipItemTable;

	MTRand				m_csRandom;

	// Lottery Box 2005.9.2 kelovon
	MapLotteryBoxToItems	m_mapLotteryBoxToItems;

	// Magoja , 아바타 세트아이템..
	VectorAvatarSetItem		m_vecAvatarSetItem;

public:

	BOOL				OnOperationAdd(BOOL bReceivedFromServer, INT8 cStatus, INT32 lIID, INT32 lTID, AgpdCharacter *pcsCharacter, INT32 lItemCount, PVOID pField, PVOID pInventory, PVOID pBank, PVOID pEquip, PVOID pFactor, PVOID pFactorPercent, PVOID pRestrictFactor, PVOID pConvert, PVOID pQuest, INT32 lSkillTID, INT32 lStatusFlag, PVOID pOption, PVOID pSkillPlus, PVOID pvCashInformation, PVOID pvExtraInformation);
	BOOL				OnOperationRemove(INT8 cStatus, INT32 lIID, INT32 lTID, AgpdCharacter *pcsCharacter, PVOID pField, PVOID pInventory, PVOID pBank, PVOID pEquip, PVOID pFactor, PVOID pFactorPercent, PVOID pQuest);
	BOOL				OnOperationUpdate(BOOL bReceivedFromServer, INT8 cStatus, INT32 lIID, INT32 lTID, AgpdCharacter *pcsCharacter, INT32 lItemCount, PVOID pField, PVOID pInventory, PVOID pBank, PVOID pEquip, PVOID pFactor, PVOID pFactorPercent, PVOID pRestrictFactor, PVOID pConvert, PVOID pQuest, INT32 lSkillTID, INT32 lStatusFlag, PVOID pOption, PVOID pSkillPlus, PVOID pCashInformtaion, PVOID pvExtraInformation);
	BOOL				OnOperationUpdate(BOOL bReceivedFromServer, INT8 cStatus, AgpdItem *pcsItem, INT32 lTID, AgpdCharacter *pcsCharacter, INT32 lItemCount, PVOID pField, PVOID pInventory, PVOID pBank, PVOID pEquip, PVOID pFactor, PVOID pFactorPercent, PVOID pRestrictFactor, PVOID pConvert, PVOID pQuest, INT32 lSkillTID, INT32 lStatusFlag, PVOID pOption, PVOID pSkillPlus, PVOID pCashInformtaion, PVOID pvExtraInformation);
	BOOL				OnOperationUseItem(INT8 cStatus, INT32 lIID, INT32 lTID, AgpdCharacter *pcsCharacter, PVOID pField, PVOID pInventory, PVOID pBank, PVOID pEquip, PVOID pFactor, PVOID pFactorPercent, INT32 lTargetID);
	BOOL				OnOperationUseItemByTID(INT32 lTID, AgpdCharacter *pcsCharacter, INT32 lTargetID, UINT32 ulUseInterval);
	BOOL				OnOperationUseItemFailedByTID(INT32 lTID, AgpdCharacter *pcsCharacter, INT32 lTargetID);
	BOOL				OnOperationPickupItem(INT8 cStatus, INT32 lIID, AgpdCharacter *pcsCharacter);
	BOOL				OnOperationPickupItemResult(INT8 cStatus, AgpdCharacter *pcsCharacter, INT32 lIID, INT32 lTID, INT32 lItemCount);
	//BOOL				OnOperationConvertItem(BOOL bReceivedFromServer, INT8 cStatus, INT32 lIID, PVOID pConvert, PVOID pRestrictFactor);
	//BOOL				OnOperationSoulCube(PVOID pEgo);
	BOOL				OnOperationStackItem( INT8 cStatus, INT32 lIID, AgpdCharacter *pcsCharacter, void *pInventory, void *pBank, void *pQuest );
	BOOL				OnOperationDropMoney( AgpdCharacter *pcsCharacter, INT32 lItemCount );
	BOOL				OnOperationUseItemSuccess(AgpdCharacter *pcsCharacter, INT32 lIID);
	BOOL				OnOperationSplitItem(AgpdCharacter *pcsCharacter, INT32 lIID, INT32 lSplitStackCount, INT8 cStatus, PVOID pField, PVOID pInventory);

	BOOL				OnOperationPutSoulIntoCube(INT32 lIID, INT32 lTargetID);
	BOOL				OnOperationUseSoulCube(INT32 lIID, INT32 lTargetID);

	BOOL				OnOperationUpdateReturnScrollStatus(AgpdCharacter *pcsCharacter, BOOL bIsEnable);
	BOOL				OnOperationUseReturnScroll(AgpdCharacter *pcsCharacter);
	BOOL				OnOperationUseReturnScrollResultFailed(AgpdCharacter *pcsCharacter);
	BOOL				OnOperationCancelReturnScroll(AgpdCharacter *pcsCharacter);

	BOOL				OnOperationRequestDestroyItem(AgpdCharacter *pcsCharacter, INT32 lIID);
	BOOL				OnOperationRequestBuyBankSlot(AgpdCharacter *pcsCharacter);

	BOOL				OnOperationUnuseItem(INT8 cStatus, INT32 lIID, INT32 lTID, AgpdCharacter *pcsCharacter);	//	2005.10.30. By SungHoon
	BOOL				OnOperationPauseItem(INT8 cStatus, INT32 lIID, INT32 lTID, AgpdCharacter* pcsCharacter);	//	2006.01.08. steeple

	BOOL				OnOperationUpdateItemUseTime(AgpdCharacter *pcsCharacter, INT32 lIID, PVOID pCashInformation);	// 20051202, kelovon
	BOOL				OnOperationUseItemResult(AgpdCharacter *pcsCharacter, AgpmItemUseResult eResult);

	BOOL				OnOperationUpdateCooldown(AgpdCharacter* pcsCharacter, INT32 lTID, UINT32 ulRemainTime, BOOL bPause);	// 2008.02.14. steeple
	BOOL				OnOperationUpdateStaminaRemainTime(AgpdCharacter* pcsCharacter, INT32 llID, PVOID pCashInformation);	// 2008.06.13. steeple

	BOOL				OnOperationChangeAutoPickUpItem(AgpdCharacter* pcsCharacter, INT32 llID, PVOID pAutoPickUpItem);		// 2008.09.08. iluvs
private:

	INT32				m_lItemMoneyTID;			// money template id를 저장해 놓는다.
	INT32				m_lItemArrowTID;
	INT32				m_lItemBoltTID;

	INT32				m_lItemHumanSkullTID;
	INT32				m_lItemOrcSkullTID;
	INT32				m_alSkullTID[AGPMITEM_MAX_SKULL_LEVEL];

	INT32				m_lItemCatalystTID;
	INT32				m_lItemLuckyScrollTID;

	INT32				m_lItemReverseOrbTID;

	INT32				m_lItemChattingEmphasisTID;

	INT32				m_lItemSkillBookTID[AURACE_TYPE_MAX][AUCHARCLASS_TYPE_MAX];

	BOOL				m_bIsRemovePolearm;

	BOOL				m_bIsCompareTemplate;

public:
	INT64				m_llBankSlotPrice[AGPMITEM_BANK_MAX_LAYER + 1];

public:
	BOOL				ProcessRefreshUI( INT32 lCID, INT16 cOldStatus, INT16 cCurrentStatus );
	BOOL				ProcessRefreshCount( INT32 lCID, AgpdItem *pcsAgpdItem );
	BOOL				ProcessRefreshCount( AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem );

	AgpmGrid*			m_pagpmGrid;

	PACKET_ITEM         m_csPacket;
	AuPacket			m_csPacketEquip;
	AuPacket			m_csPacketInventory;
	AuPacket			m_csPacketBank;
	AuPacket			m_csPacketQuest;
	AuPacket			m_csPacketField;
	AuPacket			m_csPacketConvert;
	AuPacket			m_csPacketEgo;
	AuPacket			m_csPacketOption;
	AuPacket			m_csPacketSkillPlus;
	AuPacket			m_csPacketCashInformaion;
	AuPacket			m_csPacketAutoPickItem;
	AuPacket			m_csPacketExtra;
	
	AgpmItem();
	~AgpmItem();

	INT32					GetEquipIndexFromTable( INT32 lPart );

	BOOL					ParseADCharTemplate(AgpdItemADCharTemplate *pcsItem, CHAR *szValue);

	// update
//	BOOL                    PreUpdateItem(AgpdItem *pItem, AgpdItemStatus eCurrentStatus);

	BOOL					CheckUseItem(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem);
	BOOL					CheckUseItem(AgpdCharacter *pcsCharacter, AgpdItemTemplate *pcsItemTemplate);

	BOOL					SaveItemStatus(AgpdItem *pcsItem, AgpdItem *pcsBuffer);
	BOOL					RestoreItemStatus(AgpdItem *pcsItem, AgpdItem *pcsBuffer);

	INT32					GetWeaponType(AgpdItemTemplate* pcsItemTemplate);
	BOOL					IsReverseOrbTypeItem(AgpdItem *pcsItem);

	// equip 관련 (Equip, UnEquip)
	AgpdGridItem			*GetEquipItem( INT32 lCID, INT32 lPart );
	AgpdGridItem			*GetEquipItem( AgpdCharacter *pcsAgpdCharacter, INT32 lPart );

	BOOL					EquipItem(INT32 lCID, INT32 lIID, BOOL bCheckUseItem = TRUE, BOOL bLogin = FALSE);
	BOOL					EquipItem(INT32 lCID, AgpdItem *pcsAgpdItem, BOOL bCheckUseItem = TRUE, BOOL bLogin = FALSE);
	BOOL					EquipItem(AgpdCharacter *pcsCharacter, INT32 lIID, BOOL bCheckUseItem = TRUE, BOOL bLogin = FALSE);
	BOOL					EquipItem(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bCheckUseItem = TRUE, BOOL bLogin = FALSE);
	BOOL					EquipItem(AgpdCharacter *pcsCharacter, AgpdItemADChar *pcsItemADChar, AgpdItem *pcsItem, BOOL bCheckUseItem = TRUE, BOOL bLogin = FALSE);
	BOOL					UnEquipItem(INT32 lCID, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem = TRUE);
	BOOL					UnEquipItem(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem = TRUE);
//	AgpdItem				*FindCharacterDefaultItem(INT32 lCID, INT32 lTID);

	AgpdAvatarSetItem		*GetAvatarSetTemplate( INT32 nTID );
	AgpdAvatarSetItem*		GetAvatarSetTemplateByBase(INT32 lBaseTID);

	BOOL					EquipAvatarItem(INT32 lCID, INT32 lID, BOOL bCheckUseItem = TRUE);
	BOOL					EquipAvatarItem(AgpdCharacter* pcsCharacter, AgpdItem* pcsAgpdItem, BOOL bCheckUseItem = TRUE);
	BOOL					UnEquipAvatarItem(INT32 lCID, INT32 lID, BOOL bEquipDefaultItem = TRUE);
	BOOL					UnEquipAvatarItem(AgpdCharacter* pcsCharacter, AgpdItem* pcsAgpdItem, BOOL bEquipDefaultItem = TRUE);

	BOOL					Debug_EquipAvatarSet( INT32 lCID, INT32 nTID , BOOL bCheckUseItem = TRUE);
	BOOL					Debug_EquipAvatarSet(AgpdCharacter *pcsCharacter, INT32 nTID, BOOL bCheckUseItem = TRUE);
	BOOL					Debug_UnEquipAvatarSet(INT32 lCID, INT32 nTID , BOOL bEquipDefaultItem = TRUE);
	BOOL					Debug_UnEquipAvatarSet(AgpdCharacter *pcsCharacter, INT32 nTID , BOOL bEquipDefaultItem = TRUE);
	INT32					GetEquipAvatarSet( INT32 lCID );
	INT32					GetEquipAvatarSet( AgpdCharacter *pcsAgpdCharacter );

	BOOL					IsAvatarItem(AgpdItemTemplate* pcsItemTemplate);
	BOOL					IsBaseAvatarItem(AgpdItemTemplate* pcsItemTemplate);
	BOOL					IsEnableEquipAvatarItem(AgpdItem* pcsItem);
	BOOL					IsEnableEquipAvatarItem(AgpdCharacter* pcsCharacter, AgpdItemTemplate* pcsItemTemplate);
	BOOL					IsEnableEquipItemInMyRegion(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem);
	eAgpmItemSectionType	GetItemSectionNum(AgpdItem* pcsItem);

	AgpdItem				*GetEquipWeapon(AgpdCharacter *pcsCharacter);
	BOOL					IsEquipWeapon(AgpdCharacter *pcsCharacter);
	BOOL					IsEquipOneHandSword(AgpdCharacter* pcsCharacter);	// 2004.12.14. steeple
	BOOL					IsEquipOneHandAxe(AgpdCharacter* pcsCharacter);		// 2004.12.14. steeple
	BOOL					IsEquipBlunt(AgpdCharacter* pcsCharacter);			// 2004.12.14. steeple
	BOOL					IsEquipTwoHandSlash(AgpdCharacter* pcsCharacter);	// 2004.12.14. steeple
	BOOL					IsEquipStaff(AgpdCharacter* pcsCharacter);			// 2004.12.14. steeple
	BOOL					IsEquipWand(AgpdCharacter* pcsCharacter);			// 2004.12.14. steeple
	BOOL					IsEquipBow(AgpdCharacter *pcsCharacter);			// 2004.12.14. steeple 변경
	BOOL					IsEquipCrossBow(AgpdCharacter *pcsCharacter);		// 2004.12.14. steeple 변경
	BOOL					IsEquipShield(AgpdCharacter* pcsCharacter);			// 2005.01.16. steeple 추가
	BOOL					IsEquipKatariya(AgpdCharacter* pcsCharacter);		// 2005.09.06. steeple 추가
	BOOL					IsEquipChakram(AgpdCharacter* pcsCharacter);		// 2005.09.06. steeple 추가
	BOOL					IsEquipStandard(AgpdCharacter* pcsCharacter);		// 2005.11.02. steeple 추가
	BOOL					IsEquipTwoHandBlunt(AgpdCharacter* pcsCharacter);	// 2007.10.18. magoja 추가
	BOOL					IsEquipRightHandOnly(AgpdCharacter* pcsCharacter);	// 2007.10.29. steeple 추가
	BOOL					IsEquipCharon(AgpdCharacter* pcsCharacter);
	BOOL					IsEquipZenon(AgpdCharacter* pcsCharacter);
	BOOL					IsEquipUnableItemInEpicZone(AgpdCharacter* pcsCharacter);

	AgpdGrid				*GetInventory( INT32 lCID );
	AgpdGrid				*GetInventory( AgpdCharacter *pcsAgpdCharacter );
	AgpdGrid				*GetEquipGrid( INT32 lCID );
	AgpdGrid				*GetEquipGrid( AgpdCharacter *pcsAgpdCharacter );
	AgpdGrid				*GetBank( INT32 lCID );
	AgpdGrid				*GetBank( AgpdCharacter	*pcsAgpdCharacter );
	AgpdGrid				*GetTradeGrid( INT32 lCID );
	AgpdGrid				*GetTradeGrid( AgpdCharacter *pcsAgpdCharacter );
	AgpdGrid				*GetClientTradeGrid( INT32 lCID );
	AgpdGrid				*GetClientTradeGrid( AgpdCharacter *pcsAgpdCharacter );
	AgpdGrid				*GetSalesBox( INT32 lCID );
	AgpdGrid				*GetSalesBox( AgpdCharacter	*pcsAgpdCharacter );
	AgpdGrid				*GetQuestGrid( INT32 lCID );
	AgpdGrid				*GetQuestGrid( AgpdCharacter *pcsAgpdCharacter );

//	2005.11.16. By SungHoon
	AgpdGrid				*GetCashInventoryGrid( INT32 lCID );
	AgpdGrid				*GetCashInventoryGrid( AgpdCharacter *pcsAgpdCharacter );
	AgpdGrid				*GetSubInventory( INT32 lCID );
	AgpdGrid				*GetSubInventory( AgpdCharacter *pcsAgpdCharacter );
//	AgpdGrid				*GetUnseenInventory( INT32 lCID );
//	AgpdGrid				*GetUnseenInventory( AgpdCharacter *pcsAgpdCharacter );

	// Grid 관련 (Add, Remove)
	//BOOL					AddItemToGrid(AgpdGrid *pcsGrid, INT32 lCheckAddCB, INT32 lUpdateCB, INT32 lItemStatus, AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnGridIdx = NULL, INT16 *pnGridRow = NULL, INT16 *pnGridCol = NULL );
	BOOL					RemoveItemFromGrid(AgpdGrid *pcsGrid, INT32 lCheckRemoveCB, AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem);
	//BOOL					UpdateGrid(AgpdGrid *pcsGrid, INT32 lCheckAddCB, INT32 lUpdateCB, INT32 lItemStatus, AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);
	//BOOL					MoveGridLayer(AgpdGrid *pcsGrid, INT32 lCheckAddCB, INT32 lUpdateCB, INT32 lItemStatus, AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer);

	// inventory 관련 (Add, Remove)
	/*
	INT32                   AddItemToInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx = NULL, INT16 *pnInvRow = NULL, INT16 *pnInvCol = NULL, BOOL bRemoveItem = TRUE, BOOL bIsStackMerge = TRUE);
	INT32                   AddItemToInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx = NULL, INT16 *pnInvRow = NULL, INT16 *pnInvCol = NULL, BOOL bRemoveItem = TRUE, BOOL bIsStackMerge = TRUE);
	*/

	BOOL					AddItemToInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsMergeStack = TRUE);
	BOOL					AddItemToInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsMergeStack = TRUE);
	BOOL					AddItemToInventory(INT32 lCID, AgpdItem *pcsAgpdItem, BOOL bIsMergeStack = TRUE);
	BOOL					AddItemToInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bIsMergeStack = TRUE);

	BOOL					AddItemToInventoryResult(AgpdItemGridResult eResult, AgpdItem *pcsAgpdItem, AgpdCharacter *pcsCharacter);

	BOOL                    RemoveItemFromInventory(INT32 lCID, AgpdItem *pcsAgpdItem);
	BOOL                    RemoveItemFromInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem);

	BOOL					UpdateItemInInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);
	BOOL					UpdateItemInInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);

	//BOOL                    UpdateInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn );
	//BOOL                    UpdateInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn );

	AgpdItem				*GetInventoryItemByTID(AgpdCharacter *pcsCharacter, INT32 lTID);
	AgpdItem				*GetCashInventoryItemByTID(AgpdCharacter *pcsCharacter, INT32 lTID);

	AgpdItem				*GetInventoryPotionItem(AgpdCharacter *pcsCharacter, AgpmItemUsablePotionType ePotionType);

	AgpdItem				*GetEquipSlotItem(AgpdCharacter *pstAgpdCharacter, AgpmItemPart ePart);

	INT32					 GetEquipItems(AgpdCharacter *pcsCharacter, INT32 *pArrEquipItem, INT32 lMax);

	// Sales Box 관련 (Add, Remove)
	/*
	BOOL                    AddItemToSalesBox(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx = NULL, INT16 *pnInvRow = NULL, INT16 *pnInvCol = NULL );
	BOOL                    AddItemToSalesBox(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx = NULL, INT16 *pnInvRow = NULL, INT16 *pnInvCol = NULL );
	*/

	BOOL					AddItemToSalesBox(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);
	BOOL					AddItemToSalesBox(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);

	BOOL                    RemoveItemFromSalesBox(INT32 lCID, AgpdItem *pcsAgpdItem);
	BOOL                    RemoveItemFromSalesBox(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem);
	BOOL					SetSalesBoxBackOut( AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem);

	// field 관련 (Add, Remove)
	BOOL                    AddItemToField(AgpdItem* pcsAgpdItem);
	BOOL                    RemoveItemFromField(AgpdItem* pcsAgpdItem, BOOL bIsDeleteFromMap = FALSE, BOOL bIsInitStatus = TRUE);

	// Bank 관련 (Add, Remove)
	/*
	BOOL					AddItemToBank(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnBankIdx = NULL, INT16 *pnBankRow = NULL, INT16 *pnBankCol = NULL );
	BOOL					AddItemToBank(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnBankIdx = NULL, INT16 *pnBankRow = NULL, INT16 *pnBankCol = NULL );
	*/
	BOOL					RemoveItemFromBank(INT32 lCID, AgpdItem *pcsAgpdItem);
	BOOL					RemoveItemFromBank(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem);
	/*
	BOOL					UpdateBank(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);
	BOOL					UpdateBank(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);
	*/

	BOOL					AddItemToBank(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge = TRUE);
	BOOL					AddItemToBank(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge = TRUE);
	BOOL					UpdateItemInBank(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);
	BOOL					UpdateItemInBank(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);
	
	// Quest 관련 (Add, Remove)
	BOOL					AddItemToQuest(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx = NULL, INT16 *pnInvRow = NULL, INT16 *pnInvCol = NULL );
	BOOL					AddItemToQuest(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx = NULL, INT16 *pnInvRow = NULL, INT16 *pnInvCol = NULL );
	BOOL					RemoveItemFromQuest(INT32 lCID, AgpdItem *pcsAgpdItem);
	BOOL					RemoveItemFromQuest(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem);
	BOOL					UpdateQuest(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);
	BOOL					UpdateQuest(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);

	//TradeGrid관련.
	VOID					ResetTradeStats(AgpdItemADChar *pstItemADChar);
	/*
	BOOL					AddItemToTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx=NULL, INT16 *pnInvRow=NULL, INT16 *pnInvCol=NULL  );
	BOOL					AddItemToTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx=NULL, INT16 *pnInvRow=NULL, INT16 *pnInvCol=NULL );
	*/

	BOOL					AddItemToTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);
	BOOL					AddItemToTradeGrid(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);

	BOOL					RemoveItemFromTradeGrid(INT32 lCID, AgpdItem* pcsAgpdItem);
	BOOL					RemoveItemFromTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem* pcsAgpdItem);
	/*
	BOOL                    UpdateTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn );
	BOOL                    UpdateTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn );
	*/

	//ClientTradeGrid관련.
	/*
	BOOL					AddItemToClientTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx=NULL, INT16 *pnInvRow=NULL, INT16 *pnInvCol=NULL );
	BOOL					AddItemToClientTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx=NULL, INT16 *pnInvRow=NULL, INT16 *pnInvCol=NULL);
	*/
	BOOL					RemoveItemFromClientTradeGrid(INT32 lCID, AgpdItem* pcsAgpdItem);
	BOOL					RemoveItemFromClientTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem* pcsAgpdItem);

	BOOL					AddItemToClientTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);
	BOOL					AddItemToClientTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);

	BOOL					AddItemCheckToSubInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge = TRUE);
	BOOL					AddItemCheckToSubInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge = TRUE);
	BOOL					AddItemToSubInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge = TRUE);
	BOOL					AddItemToSubInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge = TRUE);
	BOOL					AddItemToSubInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bIsMergeStack = TRUE);
	BOOL					AddItemToSubInventoryResult(AgpdItemGridResult eResult, AgpdItem *pcsAgpdItem, AgpdCharacter *pcsCharacter);
	BOOL					UpdateItemInSubInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);
	BOOL					UpdateItemInSubInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);
	BOOL					RemoveItemFromSubInventory(INT32 lCID, AgpdItem *pcsAgpdItem);
	BOOL					RemoveItemFromSubInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem);

	BOOL					AddItemToUnseenInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge = TRUE);
	BOOL					AddItemToUnseenInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge = TRUE);
	BOOL					AddItemAutoToUnSeenInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bIsMergeStack);
	BOOL					AddItemToUnseenInventoryResult(AgpdItemGridResult eResult, AgpdItem *pcsAgpdItem, AgpdCharacter *pcsCharacter);
	BOOL					RemoveItemFromUnSeenInventory(INT32 lCID, AgpdItem *pcsAgpdItem);
	BOOL					RemoveItemFromUnSeenInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem);
	BOOL					UpdateItemInUnseenInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);
	BOOL					UpdateItemInUnseenInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);

	BOOL					CheckMovableItemBetweenGrid(AgpdItem *pcsItem, AgpdItemStatus eNewStatus);

	// Money Drop 관련
	INT32					GetMoneyTID();
	INT32					GetItemMoney(AgpdItem *pcsItem);
	float					GetNPCPrice(AgpdItem *pcsItem);
	float					GetPCPrice(AgpdItem *pcsItem);
	BOOL					SetItemMoney(AgpdItem *pcsItem, INT32 lMoney);
	AgpdItem*				AddItemMoney(INT32 lIID, INT32 lMoney, AuPOS pos);

	BOOL					SetItemPrice(AgpdItem *pcsItem, INT64 llPrice);
	INT64					GetItemPrice(AgpdItem *pcsItem);

	INT32					GetArrowTID();
	INT32					GetTotalArrowCountInInventory(AgpdCharacter *pcsCharacter);
	AgpdItem*				GetArrowItemFromInventory(AgpdCharacter *pcsCharacter);

	INT32					GetBoltTID();
	INT32					GetTotalBoltCountInInventory(AgpdCharacter *pcsCharacter);
	AgpdItem*				GetBoltItemFromInventory(AgpdCharacter *pcsCharacter);

	INT32					GetHumanSkullTID();
	BOOL					SetHumanSkullTID(INT32 lSkullTID);

	INT32					GetOrcSkullTID();
	BOOL					SetOrcSkullTID(INT32 lSkullTID);

	INT32					GetSkullTID(INT32 lSkullLevel);							// 2005.01.13. steeple
	BOOL					SetSkullTID(INT32 lSkullTID, INT32 lSkullLevel);		// 2005.01.13. steeple

	INT32					GetCatalystTID();
	BOOL					SetCatalystTID(INT32 lCatalystTID);
	AgpdItem*				GetExistCatalyst(AgpdCharacter* pcsCharacter);

	INT32					GetLuckyScrollTID();
	BOOL					SetLuckyScrollTID(INT32 lLuckyScrollTID);

	INT32					GetChattingEmphasisTID();
	BOOL					SetChattingEmphasisTID(INT32 lEmphasisTID);

	//INT32					GetReverseOrbTID() { return m_lItemReverseOrbTID; };
	//INT32					GetReverseOrbTID() { return 1054; };
	//BOOL					SetReverseOrbTID(INT32 lReverseOrbTID) { m_lItemReverseOrbTID = lReverseOrbTID; return TRUE; };

	AgpdItem*				GetExistReverseOrb(AgpdCharacter *pcsCharacter, BOOL bIncludeCash = TRUE);

	AgpdItem*				GetExistPrivateTradeOptionItem(AgpdCharacter *pcsCharacter, INT32 lIID);

	INT32					GetSkillBookTID(AuRaceType eRaceType, AuCharClassType eClassType);
	BOOL					SetSkillBookTID(AuRaceType eRaceType, AuCharClassType eClassType, INT32 lSkillBookTID);

	AgpdItem*				GetHPPotionInInventory(AgpdCharacter *pcsCharacter);
	AgpdItem*				GetMPPotionInInventory(AgpdCharacter *pcsCharacter);
	AgpdItem*				GetSPPotionInInventory(AgpdCharacter *pcsCharacter);

	// item stack count 관련
	BOOL					SetItemStackCount(AgpdItem *pcsItem, INT32 lCount, BOOL bIsSaveToDB = TRUE);
	INT32					GetItemStackCount(AgpdItem *pcsItem);
	INT32					AddItemStackCount(AgpdItem *pcsItem, INT32 lAddCount, BOOL bIsSaveToDB = TRUE);
	INT32					SubItemStackCount(AgpdItem *pcsItem, INT32 lSubCount, BOOL bIsSaveToDB = TRUE);

	// TID 관련 모든 처리. 2008.02.01. steeple
	INT32					GetItemTotalCountFromGrid(AgpdCharacter* pcsCharacter, INT32 lItemTID, AgpdItemStatus eStatus);
	BOOL					RemoveItemCompleteFromGrid(AgpdCharacter* pcsCharacter, INT32 lItemTID, INT32 lCount, AgpdItemStatus eStatus);

	// item restrict factor 관련
	BOOL					RequireLevelUp(AgpdItem *pcsItem, INT32 lLevel);
	BOOL					RequireLevelDown(AgpdItem *pcsItem, INT32 lLevel);

	// Stream 관련 함수들
	BOOL					StreamWriteTemplate(CHAR *szFile, BOOL bEncryption);
	BOOL					StreamWriteTemplates( char* pPathName, char* pEntryFileName, BOOL bEncryption );
	BOOL					StreamWriteOneTemplate( AgpdItemTemplate* ppdItemTemplate, char* pFileName, BOOL bEncryption );

	BOOL					StreamReadTemplate(CHAR *szFile, CHAR *pszErrorMessage = NULL, BOOL bDecryption = FALSE);
	BOOL					StreamReadTemplates( char* pPathName, char* pEntryFileName, char* pErrorString = NULL, BOOL bDecryption = FALSE );
	BOOL					StreamReadOneTemplate( char* pFileName, BOOL bDecryption = FALSE );

	BOOL					StreamWrite(CHAR *szFile);
	BOOL					StreamRead(CHAR *szFile);

	static BOOL				CharTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL				CharTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL				TemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL				TemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL				ItemWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL				ItemReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	// import excel file functions
	////////////////////////////////////////////////////////////////////////////////////////////////
	static BOOL				CharacterImportDataReadCB(PVOID pData, PVOID pClass, PVOID pCustData);
	BOOL					StreamReadImportData(CHAR *szFile, CHAR *szDebugString, BOOL bDecryption);
	BOOL					StreamReadTransformData(CHAR *szFile, BOOL bDecryption);
	BOOL					StreamReadOptionData(CHAR *szFile, BOOL bDecryption);
	BOOL					StreamReadBankSlotPrice(CHAR *szFile, BOOL bDecryption);
	// Lottery Box 2005.9.2 kelovon
	BOOL					StreamReadLotteryBox(CHAR *szFile, BOOL bDecryption);

	// 마고자
	BOOL					StreamReadAvatarSet(CHAR *szFile, BOOL bDecryption);

	BOOL					StreamReadOptionTID(AgpdItemTemplate* pcsItemTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow);
	BOOL					StreamReadLinkID(AgpdItemTemplate* pcsItemTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow);
	BOOL					StreamReadSkillPlusTID(AgpdItemTemplate* pcsItemTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow);

	BOOL					SetDefaultEquipments(AgpdCharacterTemplate *pstAgpdCharacterTemplate, INT32 lITID, INT32 lPart = 0);
	BOOL					SetDefaultInventoryItems(AgpdCharacterTemplate *pstAgpdCharacterTemplate, INT32 lITID, INT32 lNum, INT32 lIndex);

	PVOID					MakePacketItem(AgpdItem *pcsItem, INT16 *pnPacketLength);
	PVOID					MakePacketItemView(AgpdItem *pcsItem, INT16 *pnPacketLength);
	PVOID					MakePacketItemUse(AgpdItem *pcsItem, INT16 *pnPacketLength, INT32 lTargetID = 0);
	PVOID					MakePacketItemPickup(AgpdItem *pcsItem, INT32 lOwnerID, INT16 *pnPacketLength);
	PVOID					MakePacketItemPickupResult(INT8 cResult, INT32 lIID, INT32 lTID, INT32 lItemCount, INT16 *pnPacketLength);

	PVOID					MakePacketItemUnuse(AgpdItem *pcsItem, INT16 *pnPacketLength);		//	2005.11.30. By SungHoon
	PVOID					MakePacketItemUnuseFailed(AgpdItem *pcsItem, INT16 *pnPacketLength);		//	2005.11.30. By SungHoon

	PVOID					MakePacketItemPause(AgpdItem* pcsItem, INT16* pnPacketLength);		//	2006.01.08. steeple

	PVOID					MakePacketItemUpdateItemUseTime(AgpdItem *pcsItem, INT16 *pnPacketLength);	// 20051202, kelovon
	PVOID					MakePacketItemUpdateItemStaminaRemainTime(AgpdItem *pcsItem, INT16 *pnPacketLength);	// 20051202, kelovon

	PVOID					MakePacketItemUpdateCooldown(INT32 lOwnerCID, AgpdItemCooldownBase stCooldownBase, INT16* pnPacketLength);
	PVOID					MakePacketExtra(AgpdItem* pcsItem, INT16 *pnPacketLength);

	/*
	// item convert 함수들
	PVOID					MakePacketItemConvertAskReallyConvert(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, INT16 *pnPacketLength);
	PVOID					MakePacketItemConvertRequest(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, INT16 *pnPacketLength);
	PVOID					MakePacketItemConvertResult(AgpdItem *pcsItem, AgpmItemConvertResult eResult, INT16 *pnPacketLength);
	PVOID					MakePacketItemConvertRequestHistory(AgpdItem *pcsItem, INT16 *pnPacketLength);
	PVOID					MakePacketItemConvertHistory(AgpdItem *pcsItem, INT16 *pnPacketLength);
	PVOID					MakePacketItemAddConvertHistory(AgpdItem *pcsItem, INT16 *pnPacketLength, BOOL bIsMakeRestrictPacket = FALSE);
	PVOID					MakePacketItemRemoveConvertHistory(AgpdItem *pcsItem, INT32 lIndex, INT16 *pnPacketLength);

	PVOID					MakePacketItemConvertOnly(AgpdItem *pcsItem, INT16 *pnPacketLength);

	PVOID					MakePacketItemPutSoulIntoCube(INT32 lSoulCubeID, INT32 lSoulCID, INT16 *pnPacketLength);
	PVOID					MakePacketItemPutSoulResult(INT32 lSoulCubeID, INT16 nResult, INT16 *pnPacketLength);
	PVOID					MakePacketItemUseSoulCube(INT32 lSoulCubeID, INT32 lTargetIID, INT16 *pnPacketLength);
	PVOID					MakePacketItemUseSoulCubeResult(INT32 lSoulCubeID, INT16 nResult, INT16 *pnPacketLength);
	*/

	PVOID					MakePacketDropMoneyToField( INT32 lCID, INT32 lMoneyCount, INT16 *pnPacketLength );

	PVOID					MakePacketSplitItem(AgpdItem *pcsItem, INT32 lSplitStackCount, AgpdItemStatus eTargetStatus, INT16 nLayer, INT16 nRow, INT16 nColumn, INT16 *pnPacketLength);
	PVOID					MakePacketSplitItem(AgpdItem *pcsItem, INT32 lSplitStackCount, AuPOS *pstDestPos, INT16 *pnPacketLength);

	PVOID					MakePacketUpdateReuseTimeForReverseOrb(INT32 lCID, UINT32 ulReuseTIme, INT16 *pnPacketLength);
	PVOID					MakePacketUpdateReuseTimeForTransform(INT32 lCID, UINT32 ulReuseTIme, INT16 *pnPacketLength);
	PVOID					MakePacketInitReuseTimeForTransform(INT32 lCID, INT16 *pnPacketLength);

	PVOID					MakePacketRequestDestroyItem(INT32 lCID, INT32 lIID, INT16 *pnPacketLength);

	PVOID					MakePacketRequestBuyBankSlot(INT32 lCID, INT16 *pnPacketLength);

	PVOID					MakePacketChangeAutoPickItem(INT32 lCID, INT32 llID, INT16 *lSwitch, INT16 *pnPacketLength);
	BOOL					MakeAndSendPacketEquipInfo(INT32 lIID, INT32 lCID);

	// Dragon Scion관련 Item값 보정 함수
	BOOL					AdjustDragonScionWeaponFactor(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, AgpdFactor *pcsFactorPoint, AgpdFactor *pcsFactorPercent, BOOL bTransform = FALSE);


	// Item Sequencing 함수들
	AgpdItem*				GetItemSequence(INT32 *plIndex);

	// Item Data 생성/파과하기
	AgpdItem*				CreateItem();
	BOOL					DestroyItem(AgpdItem *pcsItem);
	//Client에 단지 Display하기위해 만들어내는 Copy Item.
	BOOL					CreateCopyItem( AgpdItem *pcAgpdItem, INT16 eStatus, INT32 lIID, INT32 lTID, INT32 lCID, INT16 nLayer, INT16 nRow, INT16 nColumn );

	AgpdItemTemplate		*CreateTemplate();
	BOOL					DestroyTemplate(AgpdItemTemplate *pcsTemplate);
	BOOL					DestroyAllTemplate();

	AgpdItemOptionTemplate*	CreateOptionTemplate();
	BOOL					DestroyOptionTemplate(AgpdItemOptionTemplate *pcsOptionTemplate);

	// Item을 List에 넣는다.
	AgpdItem*				AddItem(INT32 lIID, INT32 lTID, INT32 lCount, BOOL bAddtoAdmin = TRUE );
	AgpdItem*				AddItem(AgpdItem *pcsItem);

	AgpdItemTemplate*		AddItemTemplate(INT32 lTID);
	AgpdItemOptionTemplate*	AddItemOptionTemplate(INT32 lTID);

	BOOL					LockAdminItem();
	BOOL					ReleaseAdminItem();

	// Item Initialization (각종 Callback을 여기서 Call한다.)
	BOOL					InitItem(AgpdItem *pcsItem);

	// Item을 List에서 뺀다.
	BOOL					RemoveItem(INT32 lIID, BOOL bDelete = FALSE, BOOL bRemoveLock = FALSE, BOOL bDestroyModuleData = FALSE);
	BOOL					RemoveItem(AgpdItem *pcsAgpdItem, BOOL bDelete = FALSE, BOOL bDestroyModuleData = FALSE);

	BOOL					RemoveAllItems();

	//BOOL					DeleteItem(INT32 lIID);
	//BOOL					DeleteItem(AgpdItem *pcsAgpdItem);

	// Item을 중립상태로 만든다. (Inventory나 Equip Slot, Field 등에서 Remove한다.)
	//BOOL                    ReleaseItem(INT32 lIID);
	//BOOL                    ReleaseItem(AgpdItem *pcsAgpdItem, AgpdItemStatus eNewStatus = AGPDITEM_STATUS_NONE);

	BOOL					RemoveStatus(INT32 lIID, AgpdItemStatus eNewStatus);
	BOOL					RemoveStatus(AgpdItem *pcsAgpdItem, AgpdItemStatus eNewStatus);

	// item remove pool 관련 함수들
	BOOL					ProcessRemove(UINT32 ulClockCount);

	BOOL					AddRemoveItem(AgpdItem *pcsItem);

	// Item 가져오기
	AgpdItem*				GetItem(INT32 lIID);
	AgpdItem*				GetItemLock(INT32 lIID);
	AgpdItem*				GetItem(AgpdGridItem *pcsGridItem);
	AgpdItem*				GetItemLock(AgpdGridItem *pcsGridItem);

	AgpdItemTemplate*		GetItemTemplate(INT32 lTID);	// TID 를 이용해 직접 구함
	AgpdItemTemplate*		GetItemTemplate(CHAR *szTName);	// szTName
	AgpdItemTemplate*		GetItemTemplateUseIID(INT32 lIID);

	AgpdItemOptionTemplate*	GetItemOptionTemplate(INT32 lTID);

	// Item 주인 바꾼다.
	BOOL					ChangeItemOwner(INT32 lIID, INT32 ulNewOwnerID);
	BOOL					ChangeItemOwner(AgpdItem *pcsAgpdItem, AgpdCharacter *pcsAgpdCharacter);

	INT32					GetItemDurabilityPercent(AgpdItem *pcsAgpdItem);
	INT32					GetItemDurabilityCurrent(AgpdItem *pcsAgpdItem);			// 2006.02.08. steeple
	INT32					GetItemDurabilityMax(AgpdItem* pcsItem);					// 2006.03.25. steeple
	INT32					GetItemDurabilityMax(AgpdItemTemplate* pcsItemTemplate);	// 2006.02.08. steeple

	BOOL					PickupItem(AgpdItem *pcsItem, INT32 lCID, BOOL *pbIsRemoveItem);

	// Attached Data Constructor/Destructor
	static BOOL				ConAgpdItemADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				DesAgpdItemADChar(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				ConAgpdItemADCharTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				DesAgpdItemADCharTemplate(PVOID pData, PVOID pClass, PVOID pCustData);

	// Callback 등록
	BOOL					SetCallbackInit(ApModuleDefaultCallBack pfCallback, PVOID pClass);				// 새로운 Item이 생긴 후 UpdateInit()를 하면 Call
	BOOL					SetCallbackNewItemToClient(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// 거래할때 상대에게 아이템 정보를 준다.
	BOOL					SetCallbackAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackRemove(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackRemoveID(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackEquip(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				    SetCallbackUnEquip(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackRemoveForNearCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL                    SetCallbackField(ApModuleDefaultCallBack pfCallback, PVOID pClass);				// Field에 추가된 후 Call
	BOOL                    SetCallbackInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// Inventory에 들어간 후 Call
	BOOL					SetCallbackRemoveFromInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// Inventory에서 지워진뒤의 Call
	BOOL					SetCallbackTradeGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// TradeGrid에 들어간 후 Call
	BOOL					SetCallbackRemoveTradeGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL                    SetCallbackBank(ApModuleDefaultCallBack pfCallback, PVOID pClass);				// Bank에 들어간 후 Call
	BOOL                    SetCallbackRemoveBank(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL                    SetCallbackCheckBank(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// Bank에 들어가기 전 Call (Validation Check용 Callback)
	BOOL					SetCallbackQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackRemoveQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackCheckQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL                    SetCallbackUpdateFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUpdateTradeGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//Trade Grid에 뭔가 들어오거나 나갈때~
	BOOL					SetCallbackUpdateTargetTradeGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackInventoryQuestAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackInventoryQuestUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackInventoryQuestRemove(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackSalesBox(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackSalesBoxRemoveUIUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackSubInventoryCheckAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackSubInventoryAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackSubInventoryUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackSubInventoryRemove(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUnseenInventoryAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackChangeItemOwner(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUseItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUseItemByTID(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUseItemFailedByTID(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUseItemSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUpdateReturnTeleportStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUseReturnScroll(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUseReturnScrollResultFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackCheckPickupItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackPickupItemMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackPickupItemResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackIconSlotInput(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Icon을 Slot에 넣을때 Sound를 Play하기 위함

	// UI Update용 함수
	BOOL					SetCallbackUIUpdateInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUIUpdateEquip(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUIUpdateTrade(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUIUpdateTradeClient(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUIUpdateBank(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUIUpdateSalesBox(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUIUpdateSubInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	// 확장 인벤
	BOOL					SetCallbackUIUpdateTradeOption(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackUpdateStackCount(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUpdateRequireLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackReceiveItemData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackReadTemplate(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// Template INI 읽고 부르는 용.

	BOOL					SetCallbackStreamReadImportDataErrorReport(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	BOOL					SetCallbackDropMoneyToField(ApModuleDefaultCallBack pfCallback, PVOID pClass);	//돈을 땅에 버릴때

	BOOL					SetCallbackAddItemToMap(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackRemoveItemFromMap(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL                    SetCallbackRequestSpiritStoneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL                    SetCallbackAddItemToBank(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL                    SetCallbackRemoveItemFromBank(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL                    SetCallbackAddItemToGuildWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL                    SetCallbackRemoveItemFromGuildWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL                    SetCallbackSplitItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL                    SetCallbackCheckJoinItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL                    SetCallbackUpdateReuseTimeForReverseOrb(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL                    SetCallbackUpdateReuseTimeForTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL                    SetCallbackInitReuseTimeForTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL                    SetCallbackUpdateItemStatusFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL                    SetCallbackStreamReadImportData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL                    SetCallbackStreamReadOptionTable(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackRemoveInventoryGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackUnuseItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//	2005.11.30. By SungHoon
	BOOL					SetCallbackUseItemResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackRemoveGuildWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackAddItemOption(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackUpdateCooldown(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackChangeAutoPickUp(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackAddItemExtraData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	// Attached Data 가져오기 (GetAttachedModuleData() Wrap Functions)
	AgpdItemADChar*			GetADCharacter(AgpdCharacter *pData);
	AgpdItemADCharTemplate*	GetADCharacterTemplate(AgpdCharacterTemplate *pData);

	INT16					AttachItemData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
	INT16					AttachItemTemplateData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	// Max Item (Template) 개수 정하기
	BOOL					SetMaxItem(INT32 nCount);
	BOOL					SetMaxItemRemove(INT32 nCount);

	// 통신 관련 함수들
	BOOL				    OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL					ParseFieldPacket(PVOID pField, AuPOS *pPos);
	BOOL					ParseInventoryPacket(PVOID pInventory, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol);
	BOOL					ParseBankPacket(PVOID pBank, INT16 *pnBankIdx, INT16 *pnBankRow, INT16 *pnBankCol);
	BOOL					ParseQuestPacket(PVOID pQuest, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol);
	BOOL					ParseEquipPacket(PVOID pEquip, INT32 *plCID);
	BOOL					ParsePersonalTrade( PVOID pPersonalTrade, INT16 *pnTradeIdx, INT16 *pnTradeRow, INT16 *pnTradeCol);

	BOOL					HasItemTemplate(AgpdCharacter *pcsCharacter, INT32 lITID);
	BOOL					CharacterForAllItems(AgpdCharacter *pcsCharacter, ApModuleDefaultCallBack pfnCallback, PVOID pClass);

	static BOOL				CBRemoveChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBPickupItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBCheckActionRequirement(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBPayActionCost(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBGetItemLancer(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBInitTemplateDefaultValue(PVOID pData, PVOID pClass, PVOID pCustData);

	//static BOOL				CBReCalcFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL					UseItem(INT32 lIID, AgpdCharacter *pcsTargetChar = NULL);
	BOOL					UseItem(AgpdItem *pcsItem, AgpdCharacter *pcsTargetChar = NULL);

	BOOL					CheckUseValidArea(AgpdItem *pcsItem);
	BOOL					CheckAreaUsePotion(AgpdItem *pcsItem, AgpmItemUsablePotionType2 ePotionType2);
	
	BOOL					UnuseItem(AgpdCharacter *pcsCharacter, INT32 lIID);				//	2005.11.30. By SungHoon
	BOOL					UnuseItem(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem);		//	2005.11.30. By SungHoon

	INT32					GetTemplateDataSize();
	AgpaItemTemplate		*GetTemplateAdmin() {return &csTemplateAdmin;}

	AgpdItem*				SplitItem(AgpdItem *pcsItem, INT32 lSplitStackCount, AgpdItemStatus eTargetStatus, INT32 lLayer = -1, INT32 lRow = -1, INT32 lColumn = -1);
	AgpdItem*				SplitItem(AgpdItem *pcsItem, INT32 lSplitStackCount, AuPOS *pstDestPos);

	BOOL					CheckJoinItem(AgpdItem *pcsItem1, AgpdItem *pcsItem2);

	BOOL					LockGrid(AgpdCharacter *pcsCharacter, AgpdItemStatus eLockGrid, INT16 *pnLayer = NULL, INT16 *pnRow = NULL, INT16 *pnColumn = NULL);
	BOOL					ReleaseGrid(AgpdCharacter *pcsCharacter, AgpdItemStatus eLockGrid, INT16 nLayer, INT16 nRow, INT16 nColumn);

	BOOL					UpdateReverseOrbReuseTime(AgpdCharacter *pcsCharacter, UINT32 ulReuseTime);
	BOOL					UpdateTransformReuseTime(AgpdCharacter *pcsCharacter, UINT32 ulReuseTime);
	BOOL					InitTransformReuseTime(AgpdCharacter *pcsCharacter);

	// Cooldown 관련. 2008.02.12. steeple
	BOOL					IsProgressingCooldown(AgpdCharacter* pcsCharacter);
	INT32					ProcessAllItemCooldown(AgpdCharacter* pcsCharacter, UINT32 ulElapsedClockCount);
	BOOL					SetCooldownByTID(AgpdCharacter* pcsCharacter, INT32 lTID, UINT32 ulRemainTime, BOOL bPause);
	BOOL					SetCooldownPause(AgpdCharacter* pcsCharacter, INT32 lTID, BOOL bPause);
	UINT32					GetRemainTimeByTID(AgpdCharacter* pcsCharacter, INT32 lTID);
	BOOL					GetCooldownPause(AgpdCharacter* pcsCharacter, INT32 lTID);
	AgpdItemCooldownBase	GetCooldownBase(AgpdCharacter* pcsCharacter, INT32 lTID);
	BOOL					CallCooldownCallback(AgpdCharacter* pcsCharacter, AgpdItemCooldownBase stCooldownBase);
	UINT32					GetReuseInterval(AgpdItemTemplate* pcsItemTemplate);
	BOOL					CheckReuseIntervalWithCooldown(AgpdItem* pcsItem, UINT32 ulRemainTime, UINT32 ulElapsedClockCount = 0);


private:
	// Bank 관련 내부 함수들
	BOOL				IsItemInBank(AgpdItemADChar *pcsItemADChar, INT32 lIID);
	BOOL				SearchEmptySlotInBank(AgpdItemADChar *pcsItemADChar, AgpdItem *pcsAgpdItem, INT16 *pnBankIdx, INT16 *pnRow, INT16 *pnColumn);
	BOOL				ClearSlotInBank(AgpdItemADChar *pcsItemADChar, INT32 lIID);
	BOOL				SetSlotInBank(AgpdItemADChar *pcsItemADChar, AgpdItem *pcsAgpdItem, INT16 nBankIdx, INT16 nRow, INT16 nColumn);
	BOOL				IsEmptySlotInBank(AgpdItemADChar *pcsItemADChar, AgpdItem *pcsAgpdItem, INT16 nBankIdx, INT16 nRow, INT16 nColumn);

	VOID				ItemTimeUpdate( AgpdItem*	pAgpdItem	,	PVOID pCashInformation );

public:
	// module register
	BOOL                OnAddModule();
	BOOL                OnInit();
	BOOL                OnDestroy();
	BOOL                OnIdle2(UINT32 ulClockCount);

	BOOL                OnValid(CHAR* szData, INT16 nSize);

public:
	/*
	// item convert functions
	BOOL				CheckConvertRequest(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone);

	AgpmItemConvertResult	IsConvertable(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone);
	BOOL				ConvertItem(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, BOOL bIsAlwaysSuccess = FALSE);

	INT32				GetNumConvertAttr(AgpdItem *pcsItem, INT32	lAttributeType);

	BOOL				AddConvertHistory(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, BOOL bUseSpiritStone = TRUE);
	BOOL				AddConvertHistory(AgpdItem *pcsItem, AgpdFactor *pcsPointFactor, AgpdFactor *pcsPercentFactor, BOOL bUseSpiritStone = TRUE, INT32 lTID = 0);
	BOOL				AddConvertHistory(AgpdItem *pcsItem, AgpmItemRuneAttribute eRuneAttribute, INT32 lAttributeValue);
	BOOL				AddConvertHistoryFactor(AgpdItem *pcsItem, AgpdFactor *pcsFactor, INT32 lIndex);
	BOOL				RemoveConvertHistory(AgpdItem *pcsItem, INT32 lIndex);
	BOOL				InitConvertHistory(AgpdItem *pcsItem);

	INT32				CalcConvertResult(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, BOOL bIsAlwaysSuccess = FALSE);

	BOOL				IsConvertHistoryFull(AgpdItem *pcsItem);
	BOOL				IsConvertHistoryEmpty(AgpdItem *pcsItem);

	BOOL				CheckConvertAttr(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone);

	BOOL				UpdateConvertHistory(AgpdItem *pcsItem, INT8 cConvertLevel, FLOAT fConvertConstant, INT8 *pcUseSpiritStone, INT32 *plTID, INT8 *peRuneAttribute, INT8 *plAttributeValue, PVOID pvPacketFactorPoint[], PVOID pvPacketFactorPercent[]);
	BOOL				AddConvertHistoryPacket(AgpdItem *pcsItem, FLOAT fConvertConstant, PVOID pvPacketAddFactorPoint, PVOID pvPacketAddFactorPercent, BOOL bSpiritStone, INT32 lTID);

	INT32				GetTotalAttribute(AgpdItem *pcsItem, INT32 lAttributeType);
	*/

	// rune item
	/*
	BOOL				CheckAddRuneAttribute(AgpdItem *pcsItem, INT32 lRuneTID, INT32 lRuneAttrLevel);
	BOOL				AddRuneAttribute(AgpdItem *pcsItem, INT32 lRuneTID, INT32 lRuneAttrLevel);
	*/

	/*
	BOOL				CheckAddRuneAttribute(AgpdItem *pcsItem, AgpmItemRuneAttribute eRuneAttribute);
	BOOL				AddRuneAttribute(AgpdItem *pcsItem, AgpmItemRuneAttribute eRuneAttribute, INT32 lAttributeValue);

	BOOL				SetRuneAttributeValue(AgpdFactor *pcsFactor, AgpmItemRuneAttribute eRuneAttribute, FLOAT fAttributeValue);

	INT32				GetNumRuneConvert(AgpdItem *pcsItem);
	INT32				GetNumSpiritStoneConvert(AgpdItem *pcsItem);

	// ego item
	BOOL				IsNormalItem(AgpdItem *pcsItem);
	BOOL				IsEgoItem(AgpdItem *pcsItem);

	BOOL				PutSoulIntoCube(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter);
	BOOL				UseSoulCube(AgpdItem *pcsSoulCube, AgpdItem *pcsTargetItem);

	INT32				CalcEgoExp(AgpdCharacter *pcsCharacter, INT32 lTotalExp);
	BOOL				ProcessEgoLevelUp(AgpdItem *pcsItem);
	*/

	BOOL				SetRemovePolearm();

public:	// For Admin Client. 2004.04.01. steeple
	BOOL				SetCallbackEquipForAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackInventoryForAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetBoundType(AgpdItemTemplate *pcsItemTemplate, AgpmItemBoundTypes eBoundType);
	AgpmItemBoundTypes	GetBoundType(AgpdItemTemplate *pcsItemTemplate);

	BOOL				SetBoundType(AgpdItem *pcsItem, AgpmItemBoundTypes eBoundType);
	AgpmItemBoundTypes	GetBoundType(AgpdItem *pcsItem);

	BOOL				IsNotBound(AgpdItem *pcsItem);
	BOOL				CheckBoundType(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter);

	BOOL				IsBoundOnOwner(AgpdItem *pcsItem);
	BOOL				SetBoundOnOwner(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter);

	BOOL				IsEventItem( AgpdItem* pcsItem );

	BOOL				SetQuestType(AgpdItem *pcsItem);
	BOOL				ResetQuestType(AgpdItem *pcsItem);
	BOOL				IsQuestItem(AgpdItem *pcsItem);

	BOOL				UpdateItemStatusFlag(AgpdItem *pcsItem);
	void				GetNearPosition(AuPOS *pcsSrcPos, AuPOS *pcsDestPos);

	BOOL				IsProperPart(AgpdItemTemplate *pcsItemTemplate, AgpdItemOptionTemplate *pcsItemOptionTemplate);
	BOOL				IsAlreadySetType(AgpdItem *pcsItem, AgpdItemOptionTemplate *pcsItemOptionTemplate);

	BOOL				AddItemOption(AgpdItem *pcsItem, INT32 lItemOptionTID, BOOL bIsValidCheck = TRUE);
	BOOL				AddItemOption(AgpdItem *pcsItem, AgpdItemOptionTemplate *pcsItemOptionTemplate, BOOL bIsValidCheck = TRUE);
    BOOL                AddRefineItemOption(AgpdItem *pcsItem, AgpdItemOptionTemplate *pcsItemOptionTemplate);

	BOOL				CalcItemOptionFactor(AgpdItem *pcsItem);

	BOOL				AddItemSkillPlus(AgpdItem* pcsItem, INT32 lSkillPlusTID);
	INT32				GefEffectedSkillPlusLevel(AgpdCharacter* pcsCharacter, INT32 lSkillTID);

// GridItem 처리 함수들
	AgpdItemGridResult	Insert(AgpdItem *pcsItem, AgpdGrid *pcsGrid, INT32 lLayer, INT32 lRow, INT32 lColumn);
	AgpdItemGridResult	InsertStackMerge(AgpdItem *pcsItem, AgpdGrid *pcsGrid, INT32 lLayer, INT32 lRow, INT32 lColumn);
	AgpdItemGridResult	AutoInsert(AgpdItem *pcsItem, AgpdGrid *pcsGrid);
	AgpdItemGridResult	AutoInsertStackMerge(AgpdItem *pcsItem, AgpdGrid *pcsGrid);

	BOOL				CheckUpdateStatus(AgpdItem *pcsItem, AgpdItemStatus eStatus, INT32 lLayer, INT32 lRow, INT32 lColumn);
	
	/*
	AgpdItemGridResult	Update(AgpdItem *pcsItem, AgpdGrid *pcsGrid, INT32 lLayer, INT32 lRow, INT32 lColumn);
	AgpdItemGridResult	UpdateStackMerge(AgpdItem *pcsItem, AgpdGrid *pcsGrid, INT32 lLayer, INT32 lRow, INT32 lColumn);
	*/

	/*
	BOOL				CheckAddItem(AgpdItem *pcsItem, AgpdItemStatus eTargetStatus, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsStackMerge);

	BOOL				CheckAddItemToInventory(AgpdItem *pcsItem, AgpdItemStatus eTargetStatus, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsStackMerge);
	BOOL				CheckAddItemToEquip(AgpdItem *pcsItem, AgpdItemStatus eTargetStatus, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsStackMerge);
	BOOL				CheckAddItemToTrade(AgpdItem *pcsItem, AgpdItemStatus eTargetStatus, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsStackMerge);
	BOOL				CheckAddItemToClientTrade(AgpdItem *pcsItem, AgpdItemStatus eTargetStatus, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsStackMerge);
	BOOL				CheckAddItemToSalesBox(AgpdItem *pcsItem, AgpdItemStatus eTargetStatus, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsStackMerge);
	BOOL				CheckAddItemToSalesBoxBackout(AgpdItem *pcsItem, AgpdItemStatus eTargetStatus, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsStackMerge);
	BOOL				CheckAddItemToBank(AgpdItem *pcsItem, AgpdItemStatus eTargetStatus, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsStackMerge);
	BOOL				CheckAddItemToQuest(AgpdItem *pcsItem, AgpdItemStatus eTargetStatus, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsStackMerge);
	*/

	BOOL				SetCompareTemplate();

	BOOL				IsBuyBankSlot(AgpdCharacter *pcsCharacter);
	BOOL				CheckBuyBankSlotCost(AgpdCharacter *pcsCharacter);
	INT64				GetBuyBankSlotCost(AgpdCharacter *pcsCharacter, INT32 *plTax = NULL);

	AgpdItem*			GetSkillRollbackScroll(AgpdCharacter *pcsCharacter);
	
	BOOL				IsFirstLooterOnly(AgpdItemTemplate *pcsTemplate);

//	For AgpmItemCashInventory
public:
	BOOL				SetCallbackAddItemCashInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackRemoveItemCashInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackUpdateItemCashInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackUseCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// ITEM_CB_ID_CHAR_USE_CASH_ITEM,
	BOOL				SetCallbackUnUseCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// ITEM_CB_ID_CHAR_UNUSE_CASH_ITEM,
	BOOL				SetCallbackPauseCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// ITEM_CB_ID_CHAR_PAUSE_CASH_ITEM
	BOOL				SetCallbackGetPetTIDByItemFromSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

public:
	BOOL				AddItemToCashInventory(INT32 lCID, AgpdItem *pcsItem );
	BOOL				AddItemToCashInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem );
	AgpdGridItem*		GetCashItemByTID( INT32 lCID, INT32 lTID );
	AgpdGridItem*		GetCashItemByTID( AgpdCharacter *pcsCharacter, INT32 lTID );
	AgpdGridItem*		GetCashItemUsableByType(AgpdCharacter *pcsCharacter, AgpmItemUsableType eUsableType, INT32 lSubType);
	AgpdGridItem*		GetUsingCashItemUsableByType(AgpdCharacter *pcsCharacter, AgpmItemUsableType eUsableType, INT32 lSubType);

	AgpmItemUsableType	GetUsableType( AgpdItem*		pcsItem			);
	BOOL				IsMarvelScroll(	AgpdItem*		pcsItem			);

	AgpdGridItem*		GetUsingCashItemBySkillTID(AgpdCharacter *pcsCharacter, INT32 lSkillTID );
	AgpdGridItem*		GetUsingCashPetItemInvolveSubInventory(AgpdCharacter* pcsCharacter);
	BOOL				IsEnableSubInventory(AgpdCharacter* pcsCharacter) { return GetUsingCashPetItemInvolveSubInventory(pcsCharacter) != NULL ? TRUE : FALSE; }

	BOOL				RemoveItemFromCashInventory(INT32 lCID, INT32 lTID);
	BOOL				RemoveItemFromCashInventory(AgpdCharacter *pcsCharacter, INT32 lTID);
	BOOL				RemoveItemFromCashInventory(INT32 lCID, AgpdItem* pcsItem);
	BOOL				RemoveItemFromCashInventory(AgpdCharacter *pcsCharacter, AgpdItem* pcsItem);
	BOOL				ArrangeCashItemGridPos(AgpdCharacter* pcsCharacter);
	BOOL				IsAnyEmptyCashInventory(AgpdCharacter *pcsCharacter);
	BOOL				IsCharacterUsingCashItem(AgpdCharacter *pcsCharacter);			// 20051201, kelovon
	BOOL				UseAllAleadyInUseCashItem(AgpdCharacter *pcsCharacter);			// 20051208, kelovon
	INT32				SubCashItemStackCountOnAttack(AgpdCharacter* pcsCharacter);		// 2005.12.12. steeple
	BOOL				IsCharacterUsingCashItemOfSameClassifyIDorTID(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem);	// 20051212, kelovon

	BOOL				IsUsingItem(AgpdItem* pcsItem);

	BOOL				StartCashItemTimer(AgpdItem *pcsItem);		//	2005.11.30. By SungHoon
	BOOL				StopCashItemTimer(AgpdItem *pcsItem);		//	2005.11.30. By SungHoon
	BOOL				PauseCashItemTimer(AgpdItem* pcsItem);		//	2006.01.08. steeple

	BOOL				SetInitialStamina(AgpdItem* pcsItem);		//	2008.06.24. steeple
	BOOL				IsUsingStamina(AgpdItemTemplate* pcsItemTemplate);	//	2008.06.25. steeple

	BOOL				CheckEnableStopCashItem(AgpdItem *pcsItem);		//	2005.12.12. By SungHoon
	AgpdItem*			GetCashSkillRollbackScroll(AgpdCharacter *pcsCharacter);

	AgpdItemOptionTemplate*	GetFitLinkTemplate(INT32 lLinkID, INT32 lLevel);
	
	BOOL				IsContinuousOffItem(AgpdItemTemplate* pcsItemTemplate);


	void				Report(FILE *fp);

private:
	BOOL				_AddPrivateTradeItem( DWORD dwIID , AgpdCharacter*	pcsCharacter );


private :
	struct stItemTemplateEntry
	{
		int				m_nTID;
		char			m_strTemplateName[ AGPMITEM_MAX_ITEM_NAME + 1 ];
		char			m_strTemplateFileName[ 256 ];

		stItemTemplateEntry( void )
		{
			m_nTID = 0;
			memset( m_strTemplateName, 0, sizeof( char ) * ( AGPMITEM_MAX_ITEM_NAME + 1 ) );
			memset( m_strTemplateFileName, 0, sizeof( char ) * 256 );
		}
	};

	std::map< int, stItemTemplateEntry >		m_mapItemTemplates;

	TiXmlDocument								m_DocItemTemplateEntry;

public :
	void				ClearItemTemplateEntry( void );
	BOOL				AddItemTemplateEntry( int nTID, char* pName, char* pFileName );
	char*				GetItemTemplateName( int nTID );

	BOOL				LoadItemTemplateEntryINI( char* pFileName, BOOL bEncryption );
	BOOL				SaveItemTemplateEntryINI( char* pFileName, BOOL bEncryption );

	BOOL				LoadItemTemplateEntryXML( char* pFileName, BOOL bEncryption );
	BOOL				SaveItemTemplateEntryXML( char* pFileName, BOOL bEncryption );

private :
	BOOL				_LoadXmlFile( TiXmlDocument* pDoc, char* pFileName, BOOL bIsEncrypt );
	BOOL				_SaveXmlFile( TiXmlDocument* pDoc, char* pFileName, BOOL bIsEncrypt );
};

#endif // __AGPMITEM_H__
