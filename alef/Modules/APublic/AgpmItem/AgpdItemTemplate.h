#if !defined(__AGPDITEMTEMPLATE_H__)
#define __AGPDITEMTEMPLATE_H__

#include "ApBase.h"
#include "AgpmGrid.h"
#include "ApdItemTemplate.h"
#include "AgpdItemOptionTemplate.h"
#include "AgpmFactors.h"

const INT32 AGPMITEM_MAX_ITEM_NAME						= 60;
const INT32 AGPMITEM_MAX_SKILL_PLUS						= 60;
const INT32 AGPMITEM_MAX_SKILL_PLUS_EFFECT				= 3;

enum AgpdItemTemplateSize
{
	AGPDITEMTEMPLATESIZE_WIDTH  = 0,
	AGPDITEMTEMPLATESIZE_HEIGHT,
	AGPDITEMTEMPLATESIZE_NUM
};

enum AgpmItemType
{
	AGPMITEM_TYPE_EQUIP	= 0, 
	AGPMITEM_TYPE_USABLE, 
	AGPMITEM_TYPE_OTHER, 
	AGPMITEM_TYPE_NUM
};

enum AgpmItemPart
{
	AGPMITEM_PART_NONE					=	1,
	AGPMITEM_PART_BODY,					//	2
	AGPMITEM_PART_HEAD,					//	3
	AGPMITEM_PART_ARMS,					//	4
	AGPMITEM_PART_HANDS,				//	5
	AGPMITEM_PART_LEGS,					//	6
	AGPMITEM_PART_FOOT,					//	7
	AGPMITEM_PART_ARMS2,				//	8
	AGPMITEM_PART_HAND_LEFT,			//	9
	AGPMITEM_PART_HAND_RIGHT,			//	10
	AGPMITEM_PART_ACCESSORY_RING1,		//	11
	AGPMITEM_PART_ACCESSORY_RING2,		//	12
	AGPMITEM_PART_ACCESSORY_NECKLACE,	//	13
	AGPMITEM_PART_RIDE,					//	14
	AGPMITEM_PART_LANCER,				//	15

	// 아래의 파트는 착용시 효과등은 동일하게 적용되며 클라이언트에서 동일파트의 뷰를 대신한다. 
	AGPMITEM_PART_V_BODY,				//	16		
	AGPMITEM_PART_V_HEAD,				//	17
	AGPMITEM_PART_V_ARMS,				//	18
	AGPMITEM_PART_V_HANDS,				//	19
	AGPMITEM_PART_V_LEGS,				//	20
	AGPMITEM_PART_V_FOOT,				//	21
	AGPMITEM_PART_V_ARMS2,				//	22
	AGPMITEM_PART_V_HAND_LEFT,			//	23
	AGPMITEM_PART_V_HAND_RIGHT,			//	24
	AGPMITEM_PART_V_ACCESSORY_RING1,	//	25
	AGPMITEM_PART_V_ACCESSORY_RING2,	//	26
	AGPMITEM_PART_V_ACCESSORY_NECKLACE,	//	27
	AGPMITEM_PART_V_RIDE,				//	28
	AGPMITEM_PART_V_LANCER,				//	29
	
	// 클라이언트에서 모든 파트의 뷰를 대신한다. 
	AGPMITEM_PART_V_ALL,				//	30
	
	AGPMITEM_PART_NUM
};

// AgpmItemEquipKind 이걸 수정하려면 꼭 아래 Define 들도 같이 손봐줘야 한다.
enum AgpmItemEquipKind
{
	AGPMITEM_EQUIP_KIND_ARMOUR		= 0x01, 
	AGPMITEM_EQUIP_KIND_WEAPON		= 0x02, 
	AGPMITEM_EQUIP_KIND_SHIELD		= 0x04, 
	AGPMITEM_EQUIP_KIND_RING		= 0x08,
	AGPMITEM_EQUIP_KIND_NECKLACE	= 0x10, 
	AGPMITEM_EQUIP_KIND_RIDE		= 0x20,
	AGPMITEM_EQUIP_KIND_NUM			= 5
};

#define AGPMITEM_EQUIP_KIND_ARMOUR_INDEX		0
#define AGPMITEM_EQUIP_KIND_WEAPON_INDEX		1
#define AGPMITEM_EQUIP_KIND_SHIELD_INDEX		2
#define AGPMITEM_EQUIP_KIND_RING_INDEX			3
#define AGPMITEM_EQUIP_KIND_NECKLACE_INDEX		4


enum AgpmItemEquipWeaponType
{
	AGPMITEM_EQUIP_WEAPON_TYPE_NONE = -1,			// 초기값
	AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_SWORD = 0,
	AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_AXE,
	AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_MACE,
	AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_HAMMER,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SWORD,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_AXE,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_MACE,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_HAMMER,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_POLEARM,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SCYTHE,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CLAW,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WING,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STAFF,
	AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_TROPHY,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WAND,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE,		// 마상무기
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_KATARIYA,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHAKRAM,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD,		// 마상깃발
	AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER,
	AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_RAPIERDAGGER,	// 문엘프용 이도류
	AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON,
	AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON,
	AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON,
	AGPMITEM_EQUIP_WEAPON_TYPE_NUM
};

enum AgpmItemUsableType
{
	AGPMITEM_USABLE_TYPE_POTION				= 0	,	// potion item
	AGPMITEM_USABLE_TYPE_BRANK				= 1	,	// empty Type(why?? ㅜ.ㅜ)
	AGPMITEM_USABLE_TYPE_SPIRIT_STONE		= 2	,	// spirit stone
	AGPMITEM_USABLE_TYPE_SOUL_CUBE				,	// soul cube
	AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL		,	// teleport scroll
	AGPMITEM_USABLE_TYPE_ARROW					,	// arrow(bow)
	AGPMITEM_USABLE_TYPE_BOLT					,	// bolt(cross-bow)
	AGPMITEM_USABLE_TYPE_TRANSFORM				,	// character transform item
	AGPMITEM_USABLE_TYPE_CONVERT_CATALYST		,	// 개조를 위한 촉매제로 쓰이는 아템
	AGPMITEM_USABLE_TYPE_RUNE					,	// 기원석
	AGPMITEM_USABLE_TYPE_RECEIPE				,	// 레시피
	AGPMITEM_USABLE_TYPE_SKILL_BOOK				,	// skill book
	AGPMITEM_USABLE_TYPE_SKILL_SCROLL			,	// skill scroll
	AGPMITEM_USABLE_TYPE_SKILLROLLBACK_SCROLL	,	// skill rollback scroll
	AGPMITEM_USABLE_TYPE_REVERSE_ORB			,	// reverse orb
	AGPMITEM_USABLE_TYPE_RECALL_PARTY			,	// recall party member
	AGPMITEM_USABLE_TYPE_MAP					,	// Local map item
	AGPMITEM_USABLE_TYPE_LOTTERY_BOX			,	// 보물상자
	AGPMITEM_USABLE_TYPE_LOTTERY_KEY			,	// 보물상자 키
	AGPMITEM_USABLE_TYPE_ANTI_DROP				,	// 수호부
	AGPMITEM_USABLE_TYPE_AREA_CHATTING			,	// 수꾸림~
	AGPMITEM_USABLE_TYPE_ADD_BANK_SLOT			,	// 뱅크 슬롯 추가
	AGPMITEM_USABLE_TYPE_USE_EFFECT				,	// Effect 발동
	AGPMITEM_USABLE_TYPE_CHATTING				,	// 채팅 관련 아이템 (사용하는 동안 채팅에 특수 효과)
	AGPMITEM_USABLE_TYPE_JUMP					,	// 특정대상으로 점프 2006.09.22. steeple
	AGPMITEM_USABLE_TYPE_LUCKY_SCROLL			,	// 행운 설명서 (개조시 성공률을 20% 올려준다.)
	AGPMITEM_USABLE_TYPE_NICK					,	// 별칭
	AGPMITEM_USABLE_TYPE_SKILL_INIT				,	// 스킬 초기화
	AGPMITEM_USABLE_TYPE_SOCKET_INIT			,	// 소켓 초기화
	AGPMITEM_USABLE_TYPE_ANY_SUNDRY				,	// 원거리 만물상점
	AGPMITEM_USABLE_TYPE_ANY_BANK				,	// 원거리 금고
	AGPMITEM_USABLE_TYPE_AVATAR					,	// 아바타 아이템
	AGPMITEM_USABLE_TYPE_CUSTOMIZE				,	// 성형술사 이용권
	AGPMITEM_USABLE_TYPE_QUEST					,	// 퀘스트 부여 아이템..
	AGPMITEM_USABLE_TYPE_PET_FOOD				,	// 펫 먹이
	AGPMITEM_USABLE_TYPE_PRIVATE_TRADE_OPTION	,	// 개인 거래 보조 아이템 (ex. 귀속 아이템 거래 가능)
	AGPMITEM_USABLE_TYPE_CERARIUMORB			= 36,	// 케라리움오브
	AGPMITEM_USABLE_TYPE_NUM
};

enum AgpmItemUsableScrollSubType
{
	AGPMITEM_USABLE_SCROLL_SUBTYPE_NONE = 0,
	AGPMITEM_USABLE_SCROLL_SUBTYPE_MOVE_SPEED,		// 신속의 물약 계열
	AGPMITEM_USABLE_SCROLL_SUBTYPE_ATTACK_SPEED,	// 광속의 물약 계열
	AGPMITEM_USABLE_SCROLL_SUBTYPE_DEFENSE,			// 방어의 물약 계열
};

enum AgpmItemUsablePotionType
{
	AGPMITEM_USABLE_POTION_TYPE_NONE = 0,
	AGPMITEM_USABLE_POTION_TYPE_HP,
	AGPMITEM_USABLE_POTION_TYPE_MP,
	AGPMITEM_USABLE_POTION_TYPE_SP,
	AGPMITEM_USABLE_POTION_TYPE_BOTH,
};

enum AgpmItemUsablePotionType2
{
	AGPMITEM_USABLE_POTION_TYPE2_NORMAL		= 0,	// 일반 포션
	AGPMITEM_USABLE_POTION_TYPE2_HOT		= 1,	// 추운지역에서 쓰는 매운포션
	AGPMITEM_USABLE_POTION_TYPE2_GUILD		= 2,	// 길드전 전용 포션..
	AGPMITEM_USABLE_POTION_TYPE2_VITAL		= 3,	// 활력 포션
	AGPMITEM_USABLE_POTION_TYPE2_RECOVERY	= 4,	// 회복 물약
	AGPMITEM_USABLE_POTION_TYPE2_CURE		= 5,    // 치료의 물약 
};

enum AgpmItemUsableTeleportScrollType
{
	AGPMITEM_USABLE_TELEPORT_SCROLL_GO_TOWN			= 0,
	AGPMITEM_USABLE_TELEPORT_SCROLL_RETURN_TOWN,
	AGPMITEM_USABLE_TELEPORT_SCROLL_DESTINATION,
	AGPMITEM_USABLE_TELEPORT_SCROLL_GO_TOWN_NOW,
};

enum AgpmItemUsableSpiritStoneType
{
	AGPMITEM_USABLE_SS_TYPE_NONE = 0,
	AGPMITEM_USABLE_SS_TYPE_MAGIC,				// 마법의 정령석
	AGPMITEM_USABLE_SS_TYPE_WATER,				// 물의 정령석
	AGPMITEM_USABLE_SS_TYPE_FIRE,				// 불의 정령석
	AGPMITEM_USABLE_SS_TYPE_EARTH,				// 땅의 정령석
	AGPMITEM_USABLE_SS_TYPE_AIR,				// 공기의 정령석
	AGPMITEM_USABLE_SS_TYPE_ICE,				// 얼음의 정령석
	AGPMITEM_USABLE_SS_TYPE_LIGHTENING,			// 번개의 정령석
	AGPMITEM_USABLE_SS_TYPE_POISON,				// 독의 정령석
	AGPMITEM_USABLE_SS_TYPE_CON = 100,			// 힘의 정령석
	AGPMITEM_USABLE_SS_TYPE_STR,				// 체력의 정령석
	AGPMITEM_USABLE_SS_TYPE_INT,				// 지식의 정령석
	AGPMITEM_USABLE_SS_TYPE_DEX,				// 민첩의 정령석
	AGPMITEM_USABLE_SS_TYPE_WIS = 106,			// 지혜의 정령석
	AGPMITEM_USABLE_SS_TYPE_NUM
};

enum AgpmItemUsableLotteryType			// ItemDataTable 의 SubType 에 세팅된다.
{
	AGPMITEM_USABLE_LOTTERY_TYPE_NONE = 0,
	AGPMITEM_USABLE_LOTTERY_TYPE_GOLD,
	AGPMITEM_USABLE_LOTTERY_TYPE_SILVER,
	AGPMITEM_USABLE_LOTTERY_TYPE_BRONZE,
	AGPMITEM_USABLE_LOTTERY_TYPE_PLATINUM,
	AGPMITEM_USABLE_LOTTERY_TYPE_MAX,
};

enum AgpmItemUsableEffectType
{
	AGPMITEM_USABLE_EFFECT_TYPE_FIREWORK = 0,
	AGPMITEM_USABLE_EFFECT_TYPE_MAX
};

enum AgpmItemUsableChatting
{
	AGPMITEM_USABLE_CHATTING_TYPE_EMPHASIS = 0,
	AGPMITEM_USABLE_CHATTING_TYPE_MAX
};

enum AgpmItemOtherType
{
	AGPMITEM_OTHER_TYPE_SKULL		= 0,	// skull
	AGPMITEM_OTHER_TYPE_MONEY,				// money
	AGPMITEM_OTHER_TYPE_ETC_ITEM,			// 잡템
	AGPMITEM_OTHER_TYPE_FIRST_LOOTER_ONLY,	// 잡은놈만 주울 수 있다. 쭈욱
	AGPMITEM_OTHER_TYPE_REMISSION,			// 사면장
	AGPMITEM_OTHER_TYPE_ARCHON_SCROLL,		// 아콘스크롤
	AGPMITEM_OTHER_TYPE_NUM
};

enum AgpmItemOtherSkullType
{
	AGPMITEM_OTHER_SKULL_TYPE_HUMAN		= 0,
	AGPMITEM_OTHER_SKULL_TYPE_ORC,
	AGPMITEM_OTHER_SKULL_TYPE_1,
	AGPMITEM_OTHER_SKULL_TYPE_2,
	AGPMITEM_OTHER_SKULL_TYPE_3,
	AGPMITEM_OTHER_SKULL_TYPE_4,
	AGPMITEM_OTHER_SKULL_TYPE_5,
	AGPMITEM_OTHER_SKULL_TYPE_NUM
};

enum AgpmItemUsableAreaChattingType// ItemDataTable 의 SubType 에 설정된다.
{
	AGPMITEM_USABLE_AREA_CHATTING_TYPE_NONE = 0,
	AGPMITEM_USABLE_AREA_CHATTING_TYPE_RACE,
	AGPMITEM_USABLE_AREA_CHATTING_TYPE_ALL,
	AGPMITEM_USABLE_AREA_CHATTING_TYPE_GLOBAL,
	AGPMITEM_USABLE_AREA_CHATTING_TYPE_MAX,
};

enum AgpdItemStatus
{
	AGPDITEM_STATUS_NONE  = -1,
	AGPDITEM_STATUS_NOTSETTING,
	AGPDITEM_STATUS_FIELD,
	AGPDITEM_STATUS_INVENTORY,
	AGPDITEM_STATUS_EQUIP,
	AGPDITEM_STATUS_TEMP,
	AGPDITEM_STATUS_NUM,
	AGPDITEM_STATUS_TRADE_GRID,				//개인창에 올라간아이템.
	AGPDITEM_STATUS_CLIENT_TRADE_GRID,		//상대방이 올린 아이템을 관리하는 Grid
	AGPDITEM_STATUS_SALESBOX_GRID,			//SaleBox에 올라가 있는 경우.
	AGPDITEM_STATUS_SALESBOX_BACKOUT,		//유찰되어 SaleBox에 있는 경우.
	AGPDITEM_STATUS_NPC_TRADE,				//NPCTrade EventData로 존재하는 경우.
	AGPDITEM_STATUS_RECEIPE,				//아이템 조합에서 Receipe가 사용되는 경우
	AGPDITEM_STATUS_QUEST,					//퀘스트에 사용중인 아이템
	AGPDITEM_STATUS_SUB_INVENTORY,			// Pet Inventory
	AGPDITEM_STATUS_TRADE_OPTION_GRID,		// PrivateTradeOptionGrid
//	AGPDITEM_STATUS_UNSEEN_INVENTORY,		// Unseen Inventory (무장해제)
	AGPDITEM_STATUS_CASH_MALL			= 50,	// login시 item select에서 제외하기 위함. by laki
	AGPDITEM_STATUS_CASH_INVENTORY,
	AGPDITEM_STATUS_MAILBOX,
	AGPDITEM_STATUS_GUILD_WAREHOUSE,
	AGPDITEM_STATUS_SIEGEWAR_OBJECT		= 90,
	AGPDITEM_STATUS_BANK				= 100,
};

enum AgpmItemRuneAttribute
{
	AGPMITEM_RUNE_ATTR_NONE				= (-1),
	//AGPMITEM_RUNE_ATTR_ADD_DAMAGE						= 0,
	//AGPMITEM_RUNE_ATTR_ADD_ICE_DAMAGE					=1,
	//AGPMITEM_RUNE_ATTR_ADD_LIGHTENING_DAMAGE			=2,
	//AGPMITEM_RUNE_ATTR_ADD_POISON_DAMAGE				=3,
	//AGPMITEM_RUNE_ATTR_ADD_SPIKE_DAMAGE					=4,
	//AGPMITEM_RUNE_ATTR_ADD_AR							=5,
	//AGPMITEM_RUNE_ATTR_ADD_BLOCK_RATE					=6,
	//AGPMITEM_RUNE_ATTR_ADD_AC							=7,
	//AGPMITEM_RUNE_ATTR_ADD_MAX_HP_POINT					=8,
	//AGPMITEM_RUNE_ATTR_ADD_MAX_MP_POINT					=9,
	//AGPMITEM_RUNE_ATTR_ADD_MAX_HP_PERCENT				=10,
	//AGPMITEM_RUNE_ATTR_ADD_MAX_MP_PERCENT				=11,
	//AGPMITEM_RUNE_ATTR_ADD_ATTACK_SPEED					=12,
	//AGPMITEM_RUNE_ATTR_REDUCE_POISON					=13,
	//AGPMITEM_RUNE_ATTR_REDUCE_SLOW						=14,
	//AGPMITEM_RUNE_ATTR_REDUCE_STUN						=15,
	//AGPMITEM_RUNE_ATTR_INDISTRUCTABLE					=16,
	//AGPMITEM_RUNE_ATTR_ADD_HP_REGEN						=17,
	//AGPMITEM_RUNE_ATTR_ADD_MP_REGEN						=18,
	//AGPMITEM_RUNE_ATTR_GET_MORE_GHELLD					=19,
	//AGPMITEM_RUNE_ATTR_ADD_PHYSICAL_ATTACK_RESIST		=20,
	//AGPMITEM_RUNE_ATTR_ADD_SKILL_ATTACK_BLOCK			=21,
	AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING			=22,
	AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING2		=23,
	AGPMITEM_RUNE_ATTR_CONVERT							=24,
	//AGPMITEM_RUNE_ATTR_MOVEMENT_SPEED					=25,
	//AGPMITEM_RUNE_ATTR_CRITICAL							=26,
	//AGPMITEM_RUNE_ATTR_STR								=27,
	//AGPMITEM_RUNE_ATTR_DEX								=28,
	//AGPMITEM_RUNE_ATTR_INT								=29,
	//AGPMITEM_RUNE_ATTR_CON								=30,
	//AGPMITEM_RUNE_ATTR_WIS								=31,
	//AGPMITEM_RUNE_ATTR_BONUS_DROP						=32,
	AGPMITEM_RUNE_ATTR_ADD_ALL_REGISTANCE_WITH_CLOAK	=33,
	//AGPMITEM_RUNE_ATTR_MAX								=34,
};

enum AgpmItemOtherEtcType
{
	AGPMITEM_OTHER_ETC_TYPE_NONE		= (-1),
};

enum AgpmItemBuyerType		// 해당 아이템을 구입할 수 있는 상대를 정해준다. 2006.06.21. steeple
{
	AGPMITEM_BUYER_TYPE_ALL					= 0,
	AGPMITEM_BUYER_TYPE_GUILD_MASTER_ONLY	= 1,
};

enum AgpmItemUsingType		// 해당 아이템을 사용할 수 있는 때를 정해준다. 2006.06.21. steeple
{
	AGPMITEM_USING_TYPE_ALL						= 0,
	AGPMITEM_USING_TYPE_SIEGE_WAR_ONLY			= 1,	// 공성전 때만 사용 가능한 아이템
	AGPMITEM_USING_TYPE_SIEGE_WAR_ATTACK_ONLY	= 2,	// 공성 길드만 가능
	AGPMITEM_USING_TYPE_SIEGE_WAR_DEFENSE_ONLY	= 4,	// 수성 길드만 가능
};

enum AgpmItemCashItemType
{
	AGPMITEM_CASH_ITEM_TYPE_NONE		= 0,	// NON-cash item
	AGPMITEM_CASH_ITEM_TYPE_ONE_USE		= 1,	// 일반 캐쉬 아이템, 소모성, 한 번 사용에 하나 소진
	AGPMITEM_CASH_ITEM_TYPE_PLAY_TIME	= 2,	// 플레이 타임 계산, 정량제
	AGPMITEM_CASH_ITEM_TYPE_REAL_TIME	= 3,	// 리얼 타임 계산, 정액제
	AGPMITEM_CASH_ITEM_TYPE_ONE_ATTACK	= 4,	// 소모성, 한 번 공격에 하나 소진
	AGPMITEM_CASH_ITEM_TYPE_EQUIP		= 5,	// Equip Item
	AGPMITEM_CASH_ITEM_TYPE_STAMINA		= 6,	// Stamina
};

#define IS_CASH_ITEM(_TYPE)		(_TYPE != AGPMITEM_CASH_ITEM_TYPE_NONE)

enum AgpmItemCashItemUseType
{
	AGPMITEM_CASH_ITEM_USE_TYPE_UNUSABLE	= 0,	// 사용 불가, 사용 버튼 비활성
	AGPMITEM_CASH_ITEM_USE_TYPE_ONCE		= 1,	// 사용 시 한 번 효과 발생, 사용 버튼 활성, 사용중 상태 없음
	AGPMITEM_CASH_ITEM_USE_TYPE_CONTINUOUS	= 2,	// 사용 시 일정 기간동안 효과 발생, 사용 버튼 활성, 사용중 상태 있음
};

enum AgpmItemPCBangType
{
	AGPMITEM_PCBANG_TYPE_NONE			= 0x00,
	AGPMITEM_PCBANG_TYPE_BUY_ONLY		= 0x01,
	AGPMITEM_PCBANG_TYPE_USE_ONLY		= 0x02,
	AGPMITEM_PCBANG_TYPE_DOUBLE_EFFECT	= 0x04,
	AGPMITEM_PCBANG_TYPE_LIMITED_COUNT	= 0x08,
	AGPMITEM_PCBANG_TYPE_USE_ONLY_TPACK = 0x10,
};

enum eAgpmItemSectionType
{
	AGPMITEM_SECTION_TYPE_NONE = 0x00,
	AGPMITEM_SECTION_TYPE_UNABLE_USE_EPIC_ZONE = 0x01,
	AGPMITEM_SECTION_TYPE_MAX = 0x02,
};

class AgpdItemTemplate : public ApdItemTemplate {
public:
	//CHAR					m_szName[AGPMITEM_MAX_ITEM_NAME + 1];			// item name
	string					m_szName;
	AgpmItemType			m_nType;										// item type (equip, usable, other)
	INT32					m_nSubType;										// item subtype	(Ride 전용)
	INT32					m_lExtraType;									// item extra type 2007.09.14. steeple
	INT16					m_nPrice;										// item price
	ApSafeArray<INT16, AGPDITEMTEMPLATESIZE_NUM>	m_nSizeInInventory;
	AgpdFactor				m_csFactor;
	AgpdFactor				m_csRestrictFactor;								// 착용 제한에 관한 factor
	INT32					m_lLimitedLevel;								// Max 착용 제한 Level. 위의 RestrictFactor 에 넣으려다가 빼놓았다. 2008.01.25. steeple

	AgpdGridItem*			m_pcsGridItem;
	AgpdGridItem*			m_pcsGridItemSmall;

	INT32					m_lFirstCategory;
	INT32					m_lSecondCategory;

	UINT32					m_ulTitleFontColor;

	INT32					m_lStatusFlag;
	BOOL					m_bFreeDuration;

	INT32					m_lMinSocketNum;
	INT32					m_lMaxSocketNum;

	INT32					m_lMinOptionNum;
	INT32					m_lMaxOptionNum;

	BOOL					m_bIsEventItem;
	INT32					m_lIsUseOnlyPCBang;
	BOOL					m_bIsVillainOnly;

	INT32					m_lHpBuff;
	INT32					m_lMpBuff;
	INT32					m_lAttackBuff;
	INT32					m_lDefenseBuff;
	INT32					m_lRunBuff;

	AgpmItemBuyerType		m_eBuyerType;				// 2006.06.21. steeple
	INT32					m_lUsingType;				// 2006.08.28. steeple. Bit Masking 되게 끔 변경.

	AgpmItemCashItemType	m_eCashItemType;			// 2005.11.21. steeple
	BOOL					m_bContinuousOff;			// 2008.03.18. steeple. On/Off 가능한 Expired Time 을 가진 아이템을 Off 시켜도 시간은 흘러간다.

	INT64					m_lRemainTime;				// AGPMITEM_CASH_ITEM_TYPE_PLAY_TIME인 경우 사용, milli-seconds
	UINT32					m_lExpireTime;				// AGPMITEM_CASH_ITEM_TYPE_REAL_TIME인 경우 사용, seconds, time_t

	INT32					m_lClassifyID;				// 아이템의 계열, 계열이 같으면 중복 사용할 수 없다. 20051208, kelovon

	BOOL					m_bCanStopUsingItem;		// 아이템 사용 중지 가능 여부, 20051213, kelovon

	AgpmItemCashItemUseType	m_eCashItemUseType;			// 아이템 사용 방식

	BOOL					m_bEnableOnRide;			// 탈 것을 탔을 때 유지가되는 지 여부. 2005.12.24. steeple

	ApSafeArray<INT32, AGPDITEM_OPTION_MAX_NUM>		m_alOptionTID;		// 2006.08.28. steeple
	ApSafeArray<INT32, AGPDITEM_LINK_MAX_NUM>		m_alLinkID;			// 2006.12.26. steeple

	ApSafeArray<UINT16, AGPMITEM_MAX_SKILL_PLUS>	m_aunSkillPlusTID;	// 2007.02.05. steeple
	
	BOOL					m_bEnableGamble	;			// 갬블 가능한 아이템인지. 2007.03.03. laki
	INT32					m_nQuestGroup	;			// 퀘스트 아이
	INT32					m_nGachaType	;			// 가챠 타입..

	INT64					m_llStaminaCure;				// An amount of filling staminas		2008.06.09. steeple
	INT64					m_llStaminaRemainTime;		// A remaining time of staminas			2008.06.09. steeple
	
	eAgpmItemSectionType	m_eItemSectionType;

	CHAR * GetName()		{ return (CHAR*)m_szName.c_str(); }
};

class AgpdItemTemplateEquip : public AgpdItemTemplate
{
public:
	AgpmItemPart			m_nPart;							// 착용 부위
	AgpmItemEquipKind		m_nKind;							// armour, shield, weapon, ... 등등
	//BOOL					m_bAvatarEquip;						// EquipItem 중에 아바타용으로 사용돼는 것만..

	AgpmItemPart			GetPartIndex		() { return ( AgpmItemPart )( m_nPart >= AGPMITEM_PART_V_BODY ? m_nPart - AGPMITEM_PART_RIDE : m_nPart ); }
	AgpmItemPart			GetAvatarPartIndex	() { return ( AgpmItemPart )( m_nPart >= AGPMITEM_PART_V_BODY ? m_nPart : m_nPart + AGPMITEM_PART_RIDE ); }
	BOOL					IsAvatarEquip		() { return m_nPart >= AGPMITEM_PART_V_BODY ? TRUE : FALSE; }

	AuCharacterLightInfo	m_lightInfo;
};

class AgpdItemTemplateEquipWeapon : public AgpdItemTemplateEquip {
public:
	AgpmItemEquipWeaponType	m_nWeaponType;
};

class AgpdItemTemplateUsable : public AgpdItemTemplate {
public:
	INT16					m_nPart;							// 착용 부위
	INT16					m_nUsableItemType;					// usable item type
	INT16					m_nUsableItemPart;					// 어떤 곳에 사용할 수 있는지...

	UINT32					m_ulUseInterval;					// 사용 주기

	// potion에서 사용
	AgpdFactor				m_csEffectFactor;					// 이 아템을 사용했을때의 효과 (예: HP상승, MP상승 등등...)
	INT32					m_lEffectApplyCount;				// effect factor를 적용하는 횟수
	INT32					m_lEffectActivityTimeMsec;			// 위 효과가 모두 발동하는데 걸리는 시간 (milisecond)
};

class AgpdItemTemplateUsablePotion : public AgpdItemTemplateUsable {
public:
	AgpmItemUsablePotionType	m_ePotionType	;
	AgpmItemUsablePotionType2	m_ePotionType2	;

	BOOL						m_bIsPercentPotion;
};

class AgpdItemTemplateUsableSpiritStone : public AgpdItemTemplateUsable {
public:
	AgpmItemUsableSpiritStoneType m_eSpiritStoneType;
};

class AgpdItemTemplateUsableTeleportScroll : public AgpdItemTemplateUsable {
public:
	AgpmItemUsableTeleportScrollType	m_eTeleportScrollType;

	AuPOS					m_stDestination;		// 2005.11.21. steeple
};

class AgpdItemTemplateUsableTransform : public AgpdItemTemplateUsable {
public:
	UINT32					m_ulDuration;
	INT32					m_lTargetTID;

	INT32					m_lAddMaxHP;
	INT32					m_lAddMaxMP;
};

class AgpdItemTemplateUsableRune : public AgpdItemTemplateUsable 
{
public:
	AgpmItemRuneAttribute	m_eRuneAttributeType;
};

class AgpdItemTemplateUsableSkillBook : public AgpdItemTemplateUsable {
public:
	CHAR					m_szSkillName[AGPMSKILL_MAX_SKILL_NAME];
};

class AgpdItemTemplateUsableSkillScroll : public AgpdItemTemplateUsable {
public:
	INT32					m_lSkillTID;
	INT32					m_lSkillLevel;
	AgpmItemUsableScrollSubType		m_eScrollSubType;
};

class AgpdItemTemplateUsableSkillRollbackScroll : public AgpdItemTemplateUsable {
public:
	INT32					m_lRollbackType;
};

class AgpdItemTemplateUsableAreaChatting : public AgpdItemTemplateUsable {
public:
	AgpmItemUsableAreaChattingType	m_eAreaChattingType;
};

class AgpdItemTemplateOther : public AgpdItemTemplate {
public:
	AgpmItemOtherType		m_eOtherItemType;					// other item type
};

class AgpdItemTemplateOtherEtc : public AgpdItemTemplateOther {
public:
	AgpmItemOtherEtcType	m_eOtherEtcType;
};

class AgpdItemTemplateOtherSkull : public AgpdItemTemplateOther {
public:
	AgpmItemOtherSkullType	m_eOtherSkullType;
};

#endif //__AGPDITEMTEMPLATE_H__