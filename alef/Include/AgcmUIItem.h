#ifndef __AGCM_UI_ITEM_H__
#define __AGCM_UI_ITEM_H__

#include "AgpmGrid.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmAuction.h"
#include "AgcmCharacter.h"
#include "AgpmItem.h"
#include "AgcmItem.h"
#include "AgcmAuction.h"
#include "AgcmPrivateTrade.h"
#include "AgpmEventNPCTrade.h"
#include "AgcmEventNPCTrade.h"
#include "AgcmUIManager2.h"
#include "AcUIControlHeader.h"
#include "AgcmUIMain.h"
#include "AgcmUICharacter.h"
#include "AgcmUIAuction.h"
#include "AgcmChatting2.h"
#include "AgpmSkill.h"
#include "AgpmEventSkillMaster.h"
#include "AgcmItemConvert.h"
#include "AgpmBillInfo.h"

#include "AgpmCombat.h"

#include "AgcdUIItem.h"

#include "AgcdUIItemTooltip.h"

#include "AgpmEventItemRepair.h"

#include "AgcmUISplitItem.h"
#include "AgpmReturnToLogin.h"

#include "AgpmArchlord.h"
#include "ContainerUtil.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIItemD" )
#else
#pragma comment ( lib , "AgcmUIItem" )
#endif
#endif

enum	eItemTooptipColor
{
	AGCMUIITEM_TOOLTIP_COLOR_NORMAL			,			
	AGCMUIITEM_TOOLTIP_COLOR_SPIRIT			,	
	AGCMUIITEM_TOOLTIP_COLOR_RUNE			,	
	AGCMUIITEM_TOOLTIP_COLOR_EGO			,	
	AGCMUIITEM_TOOLTIP_COLOR_EQUIP			,	
	AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY	,	
	AGCMUIITEM_TOOLTIP_COLOR_DESCRIPTION	,	
	AGCMUIITEM_TOOLTIP_COLOR_BOUND_TYPE		,	
	AGCMUIITEM_TOOLTIP_COLOR_QUEST_TYPE		,	
	AGCMUIITEM_TOOLTIP_COLOR_OPTION			,	
	AGCMUIITEM_TOOLTIP_COLOR_RARE			,	
	AGCMUIITEM_TOOLTIP_COLOR_UNIQUE			,	
	AGCMUIITEM_TOOLTIP_COLOR_SKILL_PLUS		,

	AGCMUIITEM_TOOLTIP_COLOR_COUNT			,	
};

const int	AGCMUIITEM_PROCESS_IDLE_INTERVAL		= 5000;
const int	AGCMUIITEM_NOTIFY_DURABILITY_TIME		= 60000;

const int	AGCMUIITEM_CLOSE_UI_DISTANCE			= 150;

typedef enum	_AgcmUIItem_Display_ID {
	AGCMUI_ITEM_DISPLAY_ID_INVENTORY_MONEY			= 0,
	AGCMUI_ITEM_DISPLAY_ID_PRIVATETRADERNAME,
	AGCMUI_ITEM_DISPLAY_ID_REQUEST_PRIVATETRADE,
	AGCMUI_ITEM_DISPLAY_ID_MESSAGE_PICKUP_ITEM_NAME,
	AGCMUI_ITEM_DISPLAY_ID_MESSAGE_PICKUP_BONUS_MONEY,
	AGCMUI_ITEM_DISPLAY_ID_SET_NAME,				//거래창에 내이름 세팅.
	AGCMUI_ITEM_DISPLAY_ID_SET_CLIENTNAME,			//거래창에 상대방 이름 세팅.
	AGCMUI_ITEM_DISPLAY_ID_PT_GHELLD,
	AGCMUI_ITEM_DISPLAY_ID_PT_CLIENTGHELLD,
	AGCMUI_ITEM_DISPLAY_ID_USE_ADD_MOVEMENT_SKILL,
	AGCMUI_ITEM_DISPLAY_ID_USE_ADD_ATKSPEED_SKILL,
	AGCMUI_ITEM_DISPLAY_ID_ITEM_NAME_IN_SALESBOX2,	//세일즈 박스내의 아이템이름.
	AGCMUI_ITEM_DISPLAY_ID_ITEM_PRICE_IN_SALESBOX2,	//세일즈 박스내의 가격.
	AGCMUI_ITEM_DISPLAY_ID_ITEM_TIME_IN_SALESBOX2,	//세일즈 박스내의 시간.
	AGCMUI_ITEM_DISPLAY_ID_SALES_ON_SALESBOX2,
	AGCMUI_ITEM_DISPLAY_ID_SALES_COMPLETE_SALESBOX2,
	AGCMUI_ITEM_DISPLAY_ID_SALES_OK_CANCEL,
} AgcmUIItem_Display_ID;

const INT16	AUTOPICKUPON	= 1;
const INT16 AUTOPICKUPOFF	= 0;

typedef struct _AgcmUIItemMessageData {
	INT32				lPickupItemTID;
	INT32				lPickupItemCount;
	INT32				lPickupItemID;
	INT32				lPickupItemBonusCount;
} AgcmUIItemMessageData;

//	개인거래 보조 아이템 최대 갯수
#define	UI_TRADE_SUPPORT_ITEM_MAX_COUNT					3
#define UI_TRADE_MAIN_GRID_NAME							"CONTROL_PT_Grid"
#define UI_TRADE_SUPPORT_GRID_NAME						"CONTROL_PT_Grid_Ex"


#define UI_MESSAGE_ID_ITEM_MONEY_NAME					"Money_Name"

//#define	UI_MESSAGE_ID_ITEM_PRICE						"Item_Price"
#define	UI_MESSAGE_ID_ITEM_RANK							"Item_Rank"
#define	UI_MESSAGE_ID_ITEM_EQUIP_TYPE					"Item_Equip_Type"
#define	UI_MESSAGE_ID_ITEM_EQUIP_ONE_HAND				"Item_Equip_One_Hand"
#define	UI_MESSAGE_ID_ITEM_EQUIP_TWO_HAND				"Item_Equip_Two_Hand"
//#define	UI_MESSAGE_ID_ITEM_DAMAGE						"Item_Damage"
//#define	UI_MESSAGE_ID_ITEM_ATTACKSPEED					"Item_Attack_Speed"
#define	UI_MESSAGE_ID_ITEM_ATTACKSPEED_VERYSLOW			"Item_Attack_Speed_VerySlow"
#define	UI_MESSAGE_ID_ITEM_ATTACKSPEED_SLOW				"Item_Attack_Speed_Slow"
#define	UI_MESSAGE_ID_ITEM_ATTACKSPEED_NORMAL			"Item_Attack_Speed_Normal"
#define	UI_MESSAGE_ID_ITEM_ATTACKSPEED_FAST				"Item_Attack_Speed_Fast"
#define	UI_MESSAGE_ID_ITEM_ATTACKSPEED_VERYFAST			"Item_Attack_Speed_VeryFast"
//#define	UI_MESSAGE_ID_ITEM_ATTACKRANGE					"Item_Attack_Range"
#define	UI_MESSAGE_ID_ITEM_ATTACKRANGE_SHORT			"Item_Attack_Short_Range"
#define	UI_MESSAGE_ID_ITEM_ATTACKRANGE_LONG				"Item_Attack_Long_Range"
//#define	UI_MESSAGE_ID_ITEM_DURABILITY					"Item_Durability"
//#define	UI_MESSAGE_ID_ITEM_DEFENSEPOINT					"Item_DefensePoint"
//#define	UI_MESSAGE_ID_ITEM_PHYSICALBLOCKRATE			"Item_PhysicalBlockRate"

//#define	UI_MESSAGE_ID_ITEM_ATTR_MAGICDEFENSE			"Item_Attr_MagicDefense"
//#define	UI_MESSAGE_ID_ITEM_ATTR_FIREDEFENSE				"Item_Attr_FireDefense"
//#define	UI_MESSAGE_ID_ITEM_ATTR_WATERDEFENSE			"Item_Attr_WaterDefense"
//#define	UI_MESSAGE_ID_ITEM_ATTR_AIRDEFENSE				"Item_Attr_AirDefense"
//#define	UI_MESSAGE_ID_ITEM_ATTR_EARTHDEFENSE			"Item_Attr_EarthDefense"
//#define	UI_MESSAGE_ID_ITEM_ATTR_STR						"Item_Attr_Str"
//#define	UI_MESSAGE_ID_ITEM_ATTR_CON						"Item_Attr_Con"
//#define	UI_MESSAGE_ID_ITEM_ATTR_INT						"Item_Attr_Int"
//#define	UI_MESSAGE_ID_ITEM_ATTR_WIS						"Item_Attr_Wis"
//#define	UI_MESSAGE_ID_ITEM_ATTR_DEX						"Item_Attr_Dex"

//#define	UI_MESSAGE_ID_ITEM_POTION_HP					"Item_Potion_HP"
//#define	UI_MESSAGE_ID_ITEM_POTION_MP					"Item_Potion_MP"
//#define	UI_MESSAGE_ID_ITEM_POTION_SP					"Item_Potion_SP"

#define	UI_MESSAGE_ID_ITEM_ARROW_BOW					"Item_Arrow_Bow"
#define	UI_MESSAGE_ID_ITEM_ARROW_CROSSBOW				"Item_Arrow_CrossBow"

//#define	UI_MESSAGE_ID_ITEM_SCROLL_GOTOWN				"Item_Scroll_GoTown"
//#define	UI_MESSAGE_ID_ITEM_SCROLL_RETURNTOWN			"Item_Scroll_ReturnTown"

#define	UI_MESSAGE_ID_ITEM_RESTRICT_RACEALL				"Item_Restrict_RaceAll"
#define	UI_MESSAGE_ID_ITEM_RESTRICT_RACEHUMAN			"Item_Restrict_RaceHuman"
#define	UI_MESSAGE_ID_ITEM_RESTRICT_RACEORC				"Item_Restrict_RaceOrc"
#define UI_MESSAGE_ID_ITEM_RESTRICT_RACEMOONELF			"Item_Restrict_RaceMoonElf"
#define UI_MESSAGE_ID_ITEM_RESTRICT_DRAGONSCION			"DragonScion"
#define	UI_MESSAGE_ID_ITEM_RESTRICT_CLASSALL			"Item_Restrict_ClassAll"
#define	UI_MESSAGE_ID_ITEM_RESTRICT_CLASSKNIGHT			"Item_Restrict_ClassKnight"
#define	UI_MESSAGE_ID_ITEM_RESTRICT_CLASSRANGER			"Item_Restrict_ClassRanger"
#define	UI_MESSAGE_ID_ITEM_RESTRICT_CLASSMAGE			"Item_Restrict_ClassMage"
//#define	UI_MESSAGE_ID_ITEM_RESTRICT_LEVEL				"Item_Restrict_Level"

#define	UI_MESSAGE_ID_ITEM_CONVERT_PHYSICAL				"Item_Convert_Physical"
#define	UI_MESSAGE_ID_ITEM_CONVERT_SPIRITSTONE			"Item_Convert_SpiritStone"
#define	UI_MESSAGE_ID_ITEM_CONVERT_RUNE					"Item_Convert_Rune"
#define	UI_MESSAGE_ID_ITEM_CONVERT_STEP					"Item_Convert_Step"

#define	UI_MESSAGE_ID_ITEM_ADDITIONALOPTION_TITLE		"Item_AdditionalOption_Title"

#define	UI_MESSAGE_ID_ITEM_CONVERT_SOCKET_STATUS		"Item_Convert_Socket_Status"
#define	UI_MESSAGE_ID_ITEM_CONVERT_SOCKET_STONEINFO1	"Item_Convert_Socket_StoneInfo1"
#define	UI_MESSAGE_ID_ITEM_CONVERT_SOCKET_STONEINFO2	"Item_Convert_Socket_StoneInfo2"
#define	UI_MESSAGE_ID_ITEM_CONVERT_SET_ATTR				"Item_Convert_Set_Attr"

#define	UI_MESSAGE_ID_ITEM_ALREADY_USE_TRANSFORM		"Item_Already_Use_Transform"
#define	UI_MESSAGE_ID_ITEM_TRYUSETELEPORTSCROLLINPVP	"Item_TryUseTelScrollInPvPZone"
#define	UI_MESSAGE_ID_ITEM_TRYUSETELEPORTSCROLLOnDead	"Item_TryUseTelScrollOnDead"
#define	UI_MESSAGE_ID_ITEM_TRYUSETELEPORTSCROLLOnTransfrom	"Item_TryUseTelScrollOnTransform"
#define	UI_MESSAGE_ID_ITEM_TRYUSETELEPORTSCROLLOnCombat	"Item_TryUseTelScrollOnCombat"
//#define	UI_MESSAGE_ID_ITEM_SPLIT_STACK_COUNT			"Item_Split_Stack_Count"
#define UI_MESSAGE_ID_ITEMTELEPORTSCROLLCONFIRM			"Item_TelScrollConfirm"

#define	UI_MESSAGE_ID_ITEM_BIND_ON_ACQUIRE				"Item_Bind_On_Acquire"
#define	UI_MESSAGE_ID_ITEM_BIND_ON_EQUIP				"Item_Bind_On_Equip"
#define	UI_MESSAGE_ID_ITEM_BIND_ON_USE					"Item_Bind_On_Use"
#define	UI_MESSAGE_ID_ITEM_BOUND_ON_OWNER				"Item_Bound_On_Owner"
#define	UI_MESSAGE_ID_ITEM_QUEST_TYPE					"Item_Quest_Type"

#define UI_MESSAGE_ID_ITEM_BUYER_TYPE_GUILD_MASTER_ONLY			"Item_Buyer_Type_Guild_Master_Only"
#define UI_MESSAGE_ID_ITEM_USING_TYPE_SIEGE_WAR_ONLY			"Item_Using_Type_Siege_War_Only"
#define UI_MESSAGE_ID_ITEM_USING_TYPE_SIEGE_WAR_ATTACK_ONLY		"Item_Using_Type_Siege_War_Attack_Only"
#define UI_MESSAGE_ID_ITEM_USING_TYPE_SIEGE_WAR_DEFENSE_ONLY	"Item_Using_Type_Siege_War_Defense_Only"

#define	UI_MESSAGE_ID_ITEM_CONVERTABLE_PHYSICAL_RANK	"Item_Convertable_Physical_Rank"
#define	UI_MESSAGE_ID_ITEM_CONVERTABLE_SS_RANK			"Item_Convertable_SpiritStone_Rank"

#define	UI_MESSAGE_ID_TRADE_REQUEST_FAILED_REFUSE_USER	"Option_Trade_Off_User"
#define	UI_MESSAGE_ID_TRADE_NEGATIVE_TEXT_COLOR			"Negative_Text_Color"

#define	UI_MESSAGE_ID_ITEM_EVENT_ITEM					"Item_Event_Only"
#define	UI_MESSAGE_ID_ITEM_PCBANG_ONLY					"Item_PCBang_Only"

#define	UI_MESSAGE_ID_USE_ITEM_RESULT_FAILED_BY_SIEGEWAR	"UseItemFailedBySiegewar"
#define	UI_MESSAGE_ID_USE_ITEM_RESULT_CANNOT_TRANSFORM_CREATURE	"UseItemCannotTransformCreature"

#define	UI_MESSAGE_ID_INVALID_USE_AREA					"InvalidUseArea"

#define UI_MESSAGE_ID_ITEM_NOT_SOCKET_INITIALIZABLE		"Item_Not_Socket_Initializable"
#define UI_MESSAGE_ID_ITEM_SOCKET_INIT_CONFIRM			"Item_Socket_Init_Confirm"

#define UI_MESSAGE_ID_USE_ITEM_TRANDE_FAILED			"TradeUseItem"

#define UI_MESSAGE_ID_ITEM_AUTO_PICK_GROUND_ITEM_ON		"AutoPickGroundItemON"
#define UI_MESSAGE_ID_ITEM_AUTO_PICK_GROUND_ITEM_OFF	"AutoPickGroundItemOFF"

typedef enum	_AgcmUIItemCB {
	AGCMUIITEM_CB_SET_TOOLTIP							= 0,
	AGCMUIITEM_CB_MOVED_FROM_GUILDWAREHOUSE,
	AGCMUIITEM_CB_REMOVE_SIEGEWAR_GRID,
	AGCMUIITEM_CB_GET_ITEM_TIME_INFO,
	AGCMUIITEM_CB_GET_ITEM_STAMINA_TIME,
} AgcmUIItemCB;


struct stCharacterAutoPickUpInfo
{
	std::string												m_strCharacterName;
	BOOL													m_bAuto;

	stCharacterAutoPickUpInfo( void )
	{
		m_bAuto = TRUE;
	}
};

struct stWorldAutoPickUpInfo
{
	ContainerMap< std::string, stCharacterAutoPickUpInfo >	m_mapCharacters;
	std::string												m_strWorldName;
	std::string												m_strCurrCharacterName;

	stWorldAutoPickUpInfo( void )
	{
		m_mapCharacters.Clear();
	}

	void						AddCharacter				( char* pCharacterName, BOOL bAuto = FALSE );
	void						UseCharacter				( char* pCharacterName );
	void						DeleteCharacter				( char* pCharacterName );
	stCharacterAutoPickUpInfo*	GetCharacter				( char* pCharacterName = NULL );
};

struct stAutoPickUpSettings
{
	ContainerMap< std::string, stWorldAutoPickUpInfo >		m_mapWorlds;
	std::string												m_strCurrWorldName;

	stAutoPickUpSettings( void )
	{
		m_mapWorlds.Clear();
	}

	void						LoadFromFile				( char* pFileName );
	void						SaveToFile					( char* pFileName );

	void						AddWorld					( char* pWorldName );
	void						UseWorld					( char* pWorldName );
	void						DeleteWorld					( char* pWorldName );
	stWorldAutoPickUpInfo*		GetWorld					( char* pWorldName = NULL );

	void						AddCharacter				( char* pCharacterName, BOOL bAuto = FALSE );
	void						UseCharacter				( char* pCharacterName );
	void						DeleteCharacter				( char* pCharacterName );

	BOOL						IsAutoPickUp				( void );
	BOOL						ToggleAutoPickUp			( void );
};


class AgcmUIItem : public ApModule
{
	ApmMap				*m_pcsApmMap;
	AgpmGrid			*m_pcsAgpmGrid;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmAuction			*m_pcsAgpmAuction;
	AgpmAuctionCategory	*m_pcsAgpmAuctionCategory;
	AgcmCharacter		*m_pcsAgcmCharacter;
	AgpmItem			*m_pcsAgpmItem;
	AgpmItemConvert		*m_pcsAgpmItemConvert;
	AgcmItemConvert		*m_pcsAgcmItemConvert;
	AgpmSkill			*m_pcsAgpmSkill;
	AgpmEventSkillMaster *m_pcsAgpmEventSkillMaster;
	AgcmItem			*m_pcsAgcmItem;
	AgcmAuction			*m_pcsAgcmAuction;
	AgcmPrivateTrade	*m_pcsAgcmPrivateTrade;
	AgpmEventNPCTrade	*m_pcsAgpmEventNPCTrade;
	AgcmEventNPCTrade	*m_pcsAgcmEventNPCTrade;
	AgcmUIControl		*m_pcsAgcmUIControl;
	AgcmUIManager2		*m_pcsAgcmUIManager2;
	AgcmUIMain			*m_pcsAgcmUIMain;
	AgcmUICharacter		*m_pcsAgcmUICharacter;
	AgcmUIAuction		*m_pcsAgcmUIAuction;
	AgcmChatting2		*m_pcsAgcmChatting2;
	AgcmUISplitItem		*m_pcsAgcmUISplitItem;
	AgpmBillInfo		*m_pcsAgpmBillInfo;

	AgpmCombat			*m_pcsAgpmCombat;

	AgpmEventItemRepair	*m_pcsAgpmEventItemRepair;

	AgpmReturnToLogin	*m_pcsAgpmReturnToLogin;
	AgpmArchlord		*m_pcsAgpmArchlord;

	AgcdUIUserData		*m_pcsInventory;
	AgcdUIUserData		*m_pcsInventoryQuest;
	AgcdUIUserData		*m_pcsInventoryMoney;
	AgcdUIUserData		*m_pcsEquip;
	//AgcdUIUserData		*m_pcsBank;
	AgcdUIUserData		*m_pcsSalesBox;
	//SalesBox2 Grid UserData
	AgcdUIUserData		*m_pcsSalesBox2Grid;
	AgcdUIUserData		*m_pcsSalesBox2SalesOn;
	AgcdUIUserData		*m_pcsSalesBox2SalesComplete;
	AgcdUIUserData		*m_pcsSalesBox2ButtonEnable;

	AgcdUIUserData		*m_pcsNPCTrade;
	AgcdUIUserData		*m_pcsPrivateTrade;
	AgcdUIUserData		*m_pcsPrivateTradeEx;
	AgcdUIUserData		*m_pcsPrivateTradeClient;
	AgcdUIUserData		*m_pcsPrivateTradeClientEx;
	AgcdUIUserData		*m_pcsPrivateTraderName;
	AgcdUIUserData		*m_pcsRequestPrivateTrade;

	AgcdUIUserData		*m_pcsPrivateTradeSetName;
	AgcdUIUserData		*m_pcsPrivateTradeSetClientName;

	AgcdUIUserData		*m_pcsPrivateTradeGhelld;
	AgcdUIUserData		*m_pcsPrivateTradeClientGhelld;

	AgcdUIUserData		*m_pcsInventoryLayerIndex;
	AgcdUIUserData		*m_pcsSubInventoryLayerIndex;
	//AgcdUIUserData		*m_pcsBankLayerIndex;

	AgcdUIUserData		*m_pcsMessageData;

	AgcdUIUserData		*m_pcsEquipSlotBody;
	AgcdUIUserData		*m_pcsEquipSlotHead;
	AgcdUIUserData		*m_pcsEquipSlotHands;
	AgcdUIUserData		*m_pcsEquipSlotLegs;
	AgcdUIUserData		*m_pcsEquipSlotFoot;
	AgcdUIUserData		*m_pcsEquipSlotHandLeft;
	AgcdUIUserData		*m_pcsEquipSlotHandRight;
	AgcdUIUserData		*m_pcsEquipSlotRing1;
	AgcdUIUserData		*m_pcsEquipSlotRing2;
	AgcdUIUserData		*m_pcsEquipSlotNecklace;
	AgcdUIUserData		*m_pcsEquipSlotRide;
	AgcdUIUserData		*m_pcsEquipSlotLancer;

	AgcdUIUserData		*m_pcsSubInventory;

	AgcdUIUserData		*m_pcsActiveReverseOrb;

	INT32				m_lInventoryLayerIndex;
	INT32				m_lSubInventoryLayerIndex;
	//INT32				m_lBankLayerIndex;

	AgpdGrid			*m_pcsInventoryGrid;		//인벤 그리드
	AgpdGrid			*m_pcsInventoryQuestGrid;	//퀘스트 인벤토리 그리드
	AgpdGrid			*m_pcsEquipGrid;			//Equip 그리드
//	AgpdGrid			*m_pcsBankGrid;				//뱅크 그리드

	AgpdGrid			*m_pcsSalesBoxGrid;			//세일즈박스 그리드
	AgpdGrid			m_csSalesBox2Grid[AGPMAUCTION_MAX_REGISTER];		//세일즈박스2의 그리드수 10개.
	char				m_strSalesBox2SalesOn[80];
	char				m_strSalesBox2SalesComplete[80];
	INT32				m_lSalesBox2SalesButtonEnable;
	AgpdGrid			*m_pcsNPCTradeGrid;				//	NPC 거래 그리드
	AgpdGrid			*m_pcsPrivateTradeGrid;			//	1:1거래 그리드( 자신의창 )
	AgpdGrid			m_PrivateTradeGridEx;		//	1:1거래 그리드( 자신의 확장인벤)
	AgpdGrid			*m_pcsPrivateTradeClientGrid;	//	1:1거래 그리드( 상대방창 )
	AgpdGrid			m_PrivateTradeClientGridEx;	//	1:1거래 그리드( 상대방 확장인벤 )

	AgpdGrid			m_csEquipSlotGrid[AGPMITEM_PART_NUM];

	AgpdGrid			*m_pcsSubInventoryGrid;		// 서브 인벤 그리드

	AgcmUIItemMessageData	m_stMessageData;

	AcUIToolTip			m_csItemToolTip				;	// AgcmUIItem에 사용될 ToolTip!

	INT32				m_lPrivateTradeRequesterCID; //1:1거래를 요청한 PC의 ID

	AgpdCharacter		*m_pcsPrivateTrader;
	AgpdCharacter		*m_pcsPTRequest;
	AgpdCharacter		*m_pcsPrivateTradeSetNameChar;
	AgpdCharacter		*m_pcsPrivateTradeSetClientNameChar;

	INT64				m_llInventoryMoney;
	INT32				m_lPrivateTradeGhelld;
	INT32				m_lPrivateTradeClientGhelld;

	INT32				m_lInventoryItemID;
	INT32				m_lNPCTradeSellItemID;
	INT32				m_lNPCTradeBuyItemID;

	INT16				m_nNPCTradeBuyStatus;
	INT32				m_lNPCTradeBuyLayer;
	INT32				m_lNPCTradeBuyRow;
	INT32				m_lNPCTradeBuyColumn;

	//Event ID List
	//Inventory, Equip
	BOOL				m_bUIOpenInventory;
	INT32				m_lInventoryUIOpen;
	INT32				m_lInventoryUIClose;
	INT32				m_lInventoryDropMoneyUIOpen;
	INT32				m_lInventoryDropMoneyUIClose;

	INT32				m_lInventoryEnableDropGhelld;
	INT32				m_lInventoryDisableDropGhelld;

	//Inventory, Equip MSG
	INT32				m_lInventoryMSGFull;
	INT32				m_lInventoryMSGNotEnoughSpaceToGetGhelld;
	INT32				m_lInventoryMSGGetItem;
	INT32				m_lInventoryMSGDropItem;
	INT32				m_lInventoryMSGDropGhelld;

	//Bank
	BOOL				m_bUIOpenBank;
	INT32				m_lBankUIOpen;
	INT32				m_lBankUIClose;

	//Private Trade
	BOOL				m_bUIOpenPrivateTradeYesNo;
	BOOL				m_bUIOpenPrivateTrade;
	BOOL				m_bUIOpenPrivateTradeClient;
	INT32				m_lRequestPrivateTradeUIOpen;
	INT32				m_lRequestPrivateTradeUIClose;
	INT32				m_lPrivateTradeOpenUIYesNo;
	INT32				m_lPrivateTradeCloseUIYesNo;
	INT32				m_lPrivateTradeOpenUI;
	INT32				m_lPrivateTradeCloseUI;
	INT32				m_lPrivateTradeClientLock;
	INT32				m_lPrivateTradeClientUnlock;
	INT32				m_lPrivateTradeActiveReadyToExchange;
	INT32				m_lPrivateTradeClientReadyToExchange;
	//Private Trade Message
	INT32				m_lPrivateTradeMSGInventoryFull;
	INT32				m_lPrivateTradeMSGTradeComplete;
	INT32				m_lPrivateTradeMSGTradeCancel;
	INT32				m_lPrivateTradeMSGError;

	//NPCTrade
	BOOL				m_bUIOpenNPCTrade;
	INT32				m_lNPCTradeOpenUI;
	INT32				m_lNPCTradeCloseUI;
	INT32				m_lOpenNPCTradeLayer;
	INT32				m_lNPCTradeInitLayer;

	//Sales Box
	BOOL				m_bUIOpenSalesBox;
	INT32				m_lSalesBoxUIOpen;
	INT32				m_lSalesBoxUIClose;
	INT32				m_lSalesBoxCancelMessageBox;
	AgpdItem			*m_pcsaAgpdItem;
	AgpdItem			*m_pcsCancelItemInfo;
	AgpdItem			*m_pcsAgpdItem4Sale;
	INT32				m_lSalesItemQuantity;

	// Sub Inventory
	BOOL				m_bUIOpenSubInventory;
	INT32				m_lSubInventoryUIOpen;
	INT32				m_lSubInventoryUIClose;
	
	//SalesBox2
	INT32				m_lSalesBox2Add;
	INT32				m_lSalesBox2Remove;
	INT32				m_lSalesBox2Update;
	INT32				m_lSalesBox2ButtonOn;
	INT32				m_lSalesBox2ButtonOff;

	//NPC Trade Message
	INT32				m_lNPCTradeMSGInventoryFull;
	INT32				m_lNPCTradeMSGConfirmBuy;
	INT32				m_lNPCTradeMSGConfirmBuyStackCount;
	INT32				m_lNPCTradeMSGConfirmSell;
	INT32				m_lNPCTradeMSGNotEnoughGhelld;
	INT32				m_lNPCTradeMSGNotEnoughSpaceToGetGhelld;

	INT32				m_lNPCTradeSuccess;

	// Item Convert
	INT32				m_lUIOpenAskConvert;
	INT32				m_lUIOpenConvertResultSuccess;
	INT32				m_lUIOpenConvertResultFail;
	INT32				m_lUIOpenConvertResultDestroyAttribute;
	INT32				m_lUIOpenConvertResultDestroyItem;
	INT32				m_lUIOpenConvertResultDifferRank;
	INT32				m_lUIOpenConvertResultDifferType;
	INT32				m_lUIOpenConvertResultFull;
	INT32				m_lUIOpenConvertResultNotEquipItem;
	INT32				m_lUIOpenConvertResultEgoItem;

	BOOL				m_bIsProcessConvert;
	INT32				m_lConvertItemID;
	INT32				m_lSpiritStoneID;

	INT32				m_nIndexADItem;
	INT16				m_nIndexADItemTemplate;

	INT32				m_lNumListNotifyDurabilityZero;
	INT32				m_lNumListNotifyDurabilityUnder5Percent;
	INT32				m_alListNotifyDurabilityZero[AGPMITEM_PART_NUM];
	INT32				m_alListNotifyDurabilityUnder5Percent[AGPMITEM_PART_NUM];
	UINT32				m_aulRegisterTimeNotifyDurabilityZero[AGPMITEM_PART_NUM];
	UINT32				m_aulRegisterTimeNotifyDurabilityUnder5Percent[AGPMITEM_PART_NUM];

	UINT32				m_ulPrevProcessClock;

	BOOL				m_bIsActiveReverseOrb;

	// Event List
	INT32				m_lEventGetItem;
	INT32				m_lEventGetMoney;

	INT32				m_lEventPickupItemSuccess;
	INT32				m_lEventPickupItemMoneySuccess;
	INT32				m_lEventPickupItemPCBangMoneySuccess;
	INT32				m_lEventPickupItemFail;
	INT32				m_lEventPickupItemMoneyFail;

	INT32				m_lEventAutoPickupItem;

	INT32				m_lEventUseSuccessReverseOrb;
	INT32				m_lEventUseFailReverseOrb;

	INT32				m_lEventUseAddMovementSkill;
	INT32				m_lEventUseAddAtkSpeedSkill;

	INT32				m_lEventExchangeBuffSkill;
	INT32				m_lEventIgnoreBuffSkill;

	INT32				m_lEventTradeFailForBoundItem;

	INT32				m_lEventConfirmDestroyItem;
	INT32				m_lEventConfirmDestroyCashItem;

	INT32				m_lEventEquipStatusHeadOn;
	INT32				m_lEventEquipStatusHandOn;
	INT32				m_lEventEquipStatusBodyOn;
	INT32				m_lEventEquipStatusLegsOn;
	INT32				m_lEventEquipStatusFootOn;
	INT32				m_lEventEquipStatusShieldOn;
	INT32				m_lEventEquipStatusWeaponOn;

	INT32				m_lEventEquipStatusHeadOff;
	INT32				m_lEventEquipStatusHandOff;
	INT32				m_lEventEquipStatusBodyOff;
	INT32				m_lEventEquipStatusLegsOff;
	INT32				m_lEventEquipStatusFootOff;
	INT32				m_lEventEquipStatusShieldOff;
	INT32				m_lEventEquipStatusWeaponOff;

	INT32				m_lEventEquipStatusOpen;
	INT32				m_lEventEquipStatusClose;

	INT32				m_lEventFullBankSlot;

	INT32				m_lEventReverseOrbError;

//	INT32				m_lEventEnableReturnScroll;
//	INT32				m_lEventDisableReturnScroll;

	//BOOL				SetUpdateCharStatus(AgpdItem *pcsItem, AgcmUIUpdateCharStatus *pstUpdateCharStatus);

	AuPOS				m_stNPCTradeOpenPos;

	AgcdUIItemTooltip	m_csItemTooltip[AGCDUIITEM_TOOLTIP_INDEX_MAX];

	CHAR				m_aszTooptipTitle[AGCMUIITEM_MAX_TOOLTIP][AGCMUIITEM_MAX_TOOLTIP_TEXT_LENGTH + 1];

	AgpdItem			*m_pcsAgpdSplitItem;

	INT32				m_lSplitItemTargetLayer;
	INT32				m_lSplitItemTargetRow;
	INT32				m_lSplitItemTargetColumn;
	
	AgpdItemTemplate	*m_pcsReturnScrollTemplate;
	AgpdGridItem		*m_pcsReturnScrollGridItem;

	INT32				m_lAddMovementPercent;
	INT32				m_lAddAttackSpeedPercent;

	AgpdItem			*m_pcsAgpdDestroyItem;

	//4 SalesBox
	AgcdUIItemSalesBox2Data	m_csSalesBox2Data[AGPMAUCTION_MAX_REGISTER];
	
	BOOL				m_bIsTransformed;

	// Sub Inventory UI
	AgcdUI*				m_pcsUISubInventory;

public:
	stAutoPickUpSettings	m_AutoPickUp;
	AcUIButton*				m_pBtnAutoPickUp;
	INT32					m_nEventGetBtnAutoPickUp;
	
public:
	AgcmUIItem();
	~AgcmUIItem();

	BOOL	OnAddModule()	;
	BOOL	OnDestroy()		;
	BOOL	OnInit()		;
	BOOL	OnIdle(UINT32 ulClockCount)		;

	BOOL	AddEvent();
	BOOL	AddFunction();
	BOOL	AddDisplay();
	BOOL	AddUserData();
	BOOL	AddGrid();
	BOOL	AddBoolean();

	AgcdUIItem*				GetADItem(AgpdItem *pcsItem);
	AgcdUIItemTooltipDesc*	GetADItemTemplate(AgpdItemTemplate *pcsItemTemplate);

	BOOL					IsPetItem( AgpdItem* ppdItem );
	BOOL					InitializeAutoPickup( void );
	BOOL					UpdateEquipSlotGrid();
	BOOL					UpdateEquipSlot( void* pCharacter, INT32 nPartID, void* pSlotUserData );
	BOOL					UpdateEquipSlotEnable( void* pCharacter, void* pGridItem, AuCharClassType eClassType = AUCHARCLASS_TYPE_NONE );
	BOOL					UpdateEquipSlotEnable( void* pCharacter, INT32 nPartID, AuCharClassType eClassType = AUCHARCLASS_TYPE_NONE );
	BOOL					IsUsableItem( void* pCharacter, void* pItem, AuCharClassType eClassType = AUCHARCLASS_TYPE_NONE );
	BOOL					IsAlertItemDurationUnder20( void* pItem );
	INT32					GetEquipSlotMaskEvent( INT32 nPartID, BOOL bIsOnEvent = TRUE );

	//CallBacks
	static BOOL SetSelfCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL ReleaseSelfCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL SelfUpdatePositionCB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBRemoveItemFromInventory(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateItemFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	//static BOOL	CBRefreshUpdateCharStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBNPCTradeSuccess(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBCheckUseItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUseItemSuccess(PVOID pData, PVOID pClass, PVOID pCustData);

	//Bank
	static BOOL CBInventoryQuestAdd(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBInventoryQuestUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBInventoryQuestRemove(PVOID pData, PVOID pClass, PVOID pCustData);

	//Inventory
	//인벤토리로 아이템이 이동되었다.
	static BOOL	CBUIUpdateInventory(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUIUpdateInventoryQuest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBInventoryMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBInventoryUIUpdate(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBInventoryOpenInvenLayer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBInventoryMoveEndItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayInventoryMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL	CBUpdateInventoryMoney(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUseItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDragDropItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDropMoneyToField(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDropMoneyToFieldYes(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDropMoneyToFieldCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBReturnConfirmCancelItemFromAuction(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL	CBInventoryDoubleClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBWasteDragDropItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// Sub Inventory
	static BOOL	CBUIUpdateSubInventory(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSubInventoryMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBSubInventoryMoveEndItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBSubInventoryUIUpdate(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBSubInventoryOpenInvenLayer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//static BOOL	CBUseItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDragDropItemToSubInventory(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBToggleAutoGetDropItem( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL CBGetBtnAutoPickUp( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );


	//Inventory MSG
	static BOOL CBUIInventoryFull( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBUIInventoryNotEnoughSpaceToGetGhelld( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBUIInventoryDropItem( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );
	static BOOL CBUIInventoryDropGhelld( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBUIInventoryDropGhelld( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );

	static BOOL CBUIInventoryDestroyItem( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );

	//Inventory Small
	static BOOL	CBInventorySmallLayer0(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBInventorySmallLayer1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBInventorySmallLayer2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBInventorySmallLayer3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	//Item의 Owner가 바뀌었다.
	static BOOL CBChangeOwner(PVOID pData, PVOID pClass, PVOID pCustData);

	//Equip
	static BOOL	CBUIUpdateEquip(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBEquipMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBEquipDragDropBase(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBEquipDoubleClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBEquipItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUnEquipItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUpdateFactorDurability(PVOID pData, PVOID pClass, PVOID pCustData);

	//NPCTrade
	//NPC TradeGird로 아이템이 이동되었다.
	static BOOL CBUIUpdateNPCTradeGrid( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBNPCTradeMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//NPC Trade UI를 연다.
	static BOOL CBNPCTradeUIOpen(PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBNPCTradeUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBNPCTradeUIClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//NPC Trade Layer를 이동한다.
	static BOOL	CBNPCTradeOpenNextLayer( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL	CBNPCTradeOpenPrevLayer( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL	CBNPCTradeInitLayer( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//오른쪽 마우스 버튼 클릭으로 바로 아템을 산다.
	static BOOL	CBBuyItemDirect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//NPCTrade MSG
	static BOOL CBUINPCTradeMSGFullInven( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBUINPCTradeMSGConfirmBuy( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBUINPCTradeMSGConfirmBuy( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );
	static BOOL CBUINPCTradeMSGConfirmBuyStackCountOK( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL CBUINPCTradeMSGConfirmBuyStackCountCancel( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL CBUINPCTradeMSGConfirmSell( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBUINPCTradeMSGConfirmSell( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );
	static BOOL CBUINPCTradeMSGNotEnoughGhelld( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBUINPCTradeMSGNotEnoughSpaceToGetGhelld( PVOID pData, PVOID pClass, PVOID pCustData );

	//PrivateTrade
	static BOOL CBClosePrivateTradeUI(PVOID pData, PVOID pClass, PVOID pCustData);
	//거래창(내거래창)을 갱신한다.
	static BOOL	CBUIUpdateTrade(PVOID pData, PVOID pClass, PVOID pCustData);
	//상대방 거래창을 갱신한다.
	static BOOL	CBUIUpdateTradeClient(PVOID pData, PVOID pClass, PVOID pCustData);
	//
	static BOOL CBDisplayPrivateTraderName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	//
	//거래여부를 묻는 창을 연다.
	static BOOL CBUIOpenPrivateTradeYesNo( PVOID pData, PVOID pClass, PVOID pCustData );
	//거래여부를 묻는 창을 닫는다.
	static BOOL CBUIClosePrivateTradeYesNo( PVOID pData, PVOID pClass, PVOID pCustData );
	//거래 UI창을 연다.
	static BOOL CBUIOpenPrivateTrade( PVOID pData, PVOID pClass, PVOID pCustData );
	//거래 UI창을 닫는다.
	static BOOL CBUIClosePrivateTrade( PVOID pData, PVOID pClass, PVOID pCustData );
	//거래를 요청한 사람이 상대방이 요청하기전에 취소해버리는경우.
	static BOOL CBRequestCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//거래를 시작한다.
	static BOOL	CBPrivateTradeAccept(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//거래를 거부한다.
	static BOOL	CBPrivateTradeReject(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	//거래창으로 아이템이 이동되었다.( 수정 2009. 3. 3. 확장인벤에 올라가는 아이템까지 검사하도록 변경 )
	static BOOL	CBPrivateTradeMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//거래창으로 돈을 올렸다.
	static BOOL CBPrivateTradeUpdateMoney( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//거래를 하겠다~ 라는 신호를 보낸다.
	static BOOL	CBPrivateTradeLock(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//거래를 하겠다~ 라는 신호를 취소한다.
	static BOOL	CBPrivateTradeUnlock(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//거래를 취소한다.
	static BOOL	CBPrivateTradeCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//
	static BOOL	CBPrivateTradeUpdateClientLock( PVOID pData, PVOID pClass, PVOID pCustData );
	//
	static BOOL	CBPrivateTradeActiveReadyToExchange( PVOID pData, PVOID pClass, PVOID pCustData );
	//
	static BOOL	CBPrivateTradeReadyToExchange(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	//
	static BOOL	CBPrivateTradeClientReadyToExchange( PVOID pData, PVOID pClass, PVOID pCustData );
	//
	static BOOL CBPrivateTradeUpdateGhelld(PVOID pData, PVOID pClass, PVOID pCustData );
	//
	static BOOL CBPrivateTradeUpdateClientGhelld(PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL CBPrivateTradeRequestRefuseUser(PVOID pData, PVOID pClass, PVOID pCustData );	//	2005.06.02. By SungHoon

	static BOOL	CBUseItemResult(PVOID pData, PVOID pClass, PVOID pCustData);

	// 확장된 이벤 관련 Callback ( 2009. 3. 2. )

	// 자신의 확장 인벤 업데이트
	static BOOL	CBUpdateTradeOptionGrid( PVOID pData , PVOID pClass , PVOID pCustData );


	//---------------------------------------------------------------------------
	
	//PrivateTradeMSG-------------------------------------
	static BOOL CBPrivateTradeMSGComplete(PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBPrivateTradeMSGCancel(PVOID pData, PVOID pClass, PVOID pCustData );

	//SalesBox
	static BOOL CBSalesBox2Open(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSalesBoxMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBUpdateSalesBoxGrid(PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBSetFocusOnBoard(PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBKillFocusOnBoard(PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBSetQuantity4SalesBox(PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBSetPrice4SalesBox(PVOID pData, PVOID pClass, PVOID pCustData );

	//SalesBox2
	static BOOL CBDisplaySalesBox2ItemName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	static BOOL CBDisplaySalesBox2ItemPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	static BOOL CBDisplaySalesBox2ItemTime(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	static BOOL CBDisplaySalesBox2SalesOn(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	static BOOL CBDisplaySalesBox2SalesComplete(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	static BOOL CBDisplaySalesBox2OkCancel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	static BOOL CBSalesBox2ClickCancelCompleteButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSalesBox2Add(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSalesBox2Add(PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBSalesBox2Remove(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSalesBox2Remove(PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBSalesBox2Update(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSalesBox2Update(PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBIsEnableSalesBox2Button(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	AgpdAuctionSales* GetSalesOfIndex(INT32 lIndex);

	//Duplicate Inven
	static BOOL	CBOpenDuplicateInven(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// pickup item (money)
	static BOOL CBPickupItemResult(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBAutoPickupItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// 왕복문서 관련 함수들..
	//////////////////////////////////////////////////////////////
	static BOOL	CBOpenReturnScroll(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBUseReturnScroll(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBCancelReturnScroll(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBUpdateReturnScrollStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUseReturnScrollResultFailed(PVOID pData, PVOID pClass, PVOID pCustData);

	// item convert
	//////////////////////////////////////////////////////////////
	// normal callback functions
	static BOOL CBAskReallyConvert( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBConvertResult( PVOID pData, PVOID pClass, PVOID pCustData );

	// ui callback functions
	static BOOL	CBSendConvertItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBCancelConvertItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// 2003_11_10 98pastel
	static BOOL CBOpenGridItemToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBCloseGridItemToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBUseReverseOrb(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBCloseAllUIToolTip(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	OpenToolTip(INT32 lIID, INT32 lX, INT32 lY, BOOL bShowPrice, BOOL bIsNPCTradeGrid);
	BOOL	OpenToolTip(AgpdItem *pcsItem, INT32 lX, INT32 lY, BOOL bShowPrice, BOOL bIsNPCTradeGrid);
	BOOL	CloseToolTip();

	void	SetItemToolTipInfo(AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid )	;		// lIID에 해당하는 Item의 Info를 ToolTip에 Setting한다 

	BOOL	SetItemToolTipName(AgpdItem *pcsItem, BOOL bIsNPCTradeGrid);
	BOOL	SetItemToolTipPrice(AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid);
	BOOL	SetItemToolTipConvertAttrInfo(AgpdItem *pcsItem);

	BOOL	SetItemToolTipDetail(AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid, AgcdUIItemTooltip *pcsItemTooltip);

	BOOL	SetItemConvertIcon(AgpdItem *pcsItem, INT32 lMaxSocket);

	BOOL	SetItemToolTipSpiritStoneInfo(AgpdItem *pcsItem);
	BOOL	SetItemToolTipRingInfo(AgpdItem *pcsItem, AgcdUIItemTooltip *pcsItemTooltip);
	BOOL	SetItemToolTipAmuletInfo(AgpdItem *pcsItem, AgcdUIItemTooltip *pcsItemTooltip);

	BOOL	SetItemToolTipTimeInfo(AgpdItem* pcsItem, AgcdUIItemTooltip* pcsItemTooltip);

	static BOOL	CBOpenQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBCloseQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	IsItemDurable( AgpdCharacter * pcsCharacter , UINT32 ulPercent, INT32 lOnEvent, INT32 lOffEvent );
	BOOL	IsItemPartDurable( AgpdCharacter * pcsCharacter , UINT32 ulPercent, INT32 lOnEvent, INT32 lOffEvent, AgpmItemPart ePart );
	BOOL	CheckItemDurability(AgpdItem *pcsItem);
	BOOL	CheckItemDurability( AgpdCharacter * pcsCharacter);
	BOOL	CheckItemLevel( AgpdCharacter * pcsCharacter , AgpdItem *pcsItem );

	BOOL	AddItemDurabilityZeroList(AgpdItem *pcsItem);
	BOOL	RemoveItemDurabilityZeroList(AgpdItem *pcsItem);

	BOOL	AddItemDurabilityUnder5PercentList(AgpdItem *pcsItem);
	BOOL	RemoveItemDurabilityUnder5PercentList(AgpdItem *pcsItem);

	static BOOL	CBUpdateAlarmGrid(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBSetupItem(PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL CBDisplayMessageData(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	// boolean callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL	CBIsActivePrivateTradeMemu(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL	CBRequestPrivateTrade( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL CBDisplayRequestPrivateTrade(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL CBDisplayPrivateTradeSetName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayPrivateTradeSetClientName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayPrivateTradeGhelld(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayPrivateTradeClientGhelld(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	BOOL	StreamReadTooltipData(CHAR *szFile, BOOL bDecryption);
	BOOL	StreamReadTooltipDescData(CHAR *szFile, BOOL bDecryption);

	BOOL	SetTooltipList(INT32 lRow, INT32 lTotalColumn, AuExcelLib *pcsExcelTxtLib, AgcdUIItemTooltip *pcsItemTooltip);

	static BOOL CBSplitItemInventory(PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL CBSplitItemNPCTrade(PVOID pData, PVOID pClass, PVOID pCustData );

	BOOL	SetCallbackSetTooltip(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackMovedFromGuildWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveSiegeWarGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackGetItemTimeInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackGetItemStaminaTime(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL	CBUseReturnScroll(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBUseItemByTID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUseItemFailedByTID(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBUpdateReuseTimeForReverseOrb(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUpdateReuseTimeForTransform(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBInitReuseTimeForTransform(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBDisplayUseAddMovementSkill(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayUseAddAtkSpeedSkill(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL	CBNPCTradeDeleteGrid(PVOID pData, PVOID pClass, PVOID pCustData);

	INT32	m_lEventTradeFailForUsingCashItem;		//	2005.12.13. By SungHoon

	static BOOL	CBReturnToLoginEnd(PVOID pData, PVOID pClass, PVOID pCustData);

	VOID SetUseItemTime(AgpdItem *pcsItem);

	BOOL	OpenInventory();
	BOOL	OpenSubInventory();
	BOOL	CloseSubInventory();

	static BOOL CBUpdateCooldown(PVOID pData, PVOID pClass, PVOID pCustData);
	inline BOOL UpdateCooldown(AgpdItem* pcsItem, AgpdItemCooldownBase& stCooldownBase);

	static BOOL CBCharacterLevelUp(PVOID pData, PVOID pClass, PVOID pCustData);

	// For Console
	void EnableSubInventory();
	void DisableSubInventory();

	DWORD		GetTooltipColor( eItemTooptipColor eType )	{	return m_dwTooltipColor[ eType];	}
	DWORD		GetToolTipColor( AgpdItem* ppdItem, eAgpdFactorsType eType, int nType2 = -1, int nType3 = -1 );

	BOOL		IsEnableUnEquipItem( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter = NULL );

private:
	void		_SetItemToolTipEquip( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	void		_SetItemToolTipUsable( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	void		_SetItemToolTipWeapon( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );

	BOOL		_SetItemToolTipDetailName( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	BOOL		_SetItemToolTipDetailPrice( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	BOOL		_SetItemToolTipDetailType( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	BOOL		_SetItemToolTipDetailRank( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	BOOL		_SetItemToolTipDetailRequireLevel( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	BOOL		_SetItemToolTipDetailRequireRace( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	BOOL		_SetItemToolTipDetailRequireClass( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	BOOL		_SetItemToolTipDetailEnableEquipRace( INT32 nEquipRequireRace );
	BOOL		_SetItemToolTipDetailEnableEquipClass( INT32 nEquipRequireClass );
	BOOL		_SetItemToolTipDetailWeaponDamage( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	BOOL		_SetItemToolTipDetailWeaponSpeed( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	BOOL		_SetItemToolTipDetailDurability( AgpdItem *pcsItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );

	BOOL		_SetItemToolTipDetailHeroicDamage( char* pName, AgpdItem* ppdItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );
	BOOL		_SetItemToolTipDetailHeroicDefense( char* pName, AgpdItem* ppdItem, BOOL bShowPrice, BOOL bIsNPCTradeGrid );

	CHAR*		_GetItemSecondCategoryName( AgpdItemTemplate *pcsItemTemplate );
	BOOL		_CollectClassNames( CHAR* pBuffer, INT32 nBufferSize, AuCharClassType eClasType, INT32 nEnableRaces );
	BOOL		_IsEnableRace( INT32 nFactorCode, AuRaceType eRaceType );
	BOOL		_IsEnableClass( INT32 nFactorCode, AuCharClassType eClassType );

	// 2009. 3. 3. 귀속 거래가 가능한 상태인지 확인( 마블 Scroll이 올라왔는지 확인 )
	BOOL		_CheckBoundItemReleaseTradeCheck	( AgpdItem*	pItem );
	BOOL		_CheckSupportItemDuplicate			( AgpdItem*	pItem );
	BOOL		_AddSupportItem						( AgpdItem*	pItem ,	BOOL bSelfCharacter );
	INT32		_SupportItemCount					( BOOL bSelfCharacter );
	VOID		_InitPrivateOptionItem				( VOID );
	VOID		_InitTooltipColor					( VOID );

	DWORD		m_dwTooltipColor[AGCMUIITEM_TOOLTIP_COLOR_COUNT];

public :
	BOOL		OnSendPrivateTradeCancel			( void );
};

#endif
