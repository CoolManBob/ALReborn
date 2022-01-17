// AgpdAdmin.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 10. 07.

#ifndef _AGPDADMIN_H_
#define _AGPDADMIN_H_

#include "ApBase.h"
#include "AgpdItemConvert.h"
#include "AgpdItem.h"
#include "AgpdSkill.h"
#include "AgpdGuild.h"
#include "AgpdTitleQuest.h"
#include <list>

#define AGPMADMIN_MAX_NUM_SEARCH_RESULT			5
#define AGPMADMIN_MAX_UT_STRING					15
#define AGPMADMIN_MAX_CREDATE_STRING			15
#define AGPMADMIN_MAX_IP_STRING					15
#define AGPMADMIN_MAX_LOGIN_STRING				15
#define AGPMADMIN_MAX_UPDATE_STRING				15
#define AGPMADMIN_MAX_HELP_MSG_STRING			255
#define AGPMADMIN_MAX_HELP_MEMO_STRING			255
#define AGPMADMIN_MAX_CHAT_STRING				255
#define AGPMADMIN_MAX_PATH						48
#define AGPMADMIN_MAX_CREATE_ITEM_NUM			10
#define AGPMADMIN_MAX_SERVERNAME_LENGTH			64
#define AGPMADMIN_MAX_SKILL_STRING_LENGTH		512
#define AGPMADMIN_MAX_USER_POSITION_COUNT		100
#define AGPMADMIN_MAX_REGION_INDEX				256

// Data Type
typedef enum _eAgpmAdminDataType
{
	AGPMADMIN_DATA_TYPE = 0,
} eAgpmAdminDataType;

// Packet Operation
typedef enum _eAgpmAdminPacketType
{
	AGPMADMIN_PACKET_LOGIN = 0,
	AGPMADMIN_PACKET_CONNECT,	// 다른 서버 접속용 Operation. - 2004.03.10.
	AGPMADMIN_PACKET_NUM_CURRENT_USER,
	AGPMADMIN_PACKET_SEARCH_CHARACTER,
	AGPMADMIN_PACKET_SEARCH_RESULT,
	AGPMADMIN_PACKET_SEARCH_CHARDATA,
	AGPMADMIN_PACKET_SEARCH_CHARITEM,
	AGPMADMIN_PACKET_SEARCH_CHARSKILL,
	AGPMADMIN_PACKET_SEARCH_CUSTOM,
	AGPMADMIN_PACKET_CHARMOVE,
	AGPMADMIN_PACKET_CHAREDIT,
	AGPMADMIN_PACKET_BAN,
	AGPMADMIN_PACKET_ITEM,
	AGPMADMIN_PACKET_SERVERINFO,
	AGPMADMIN_PACKET_GUILD,
	AGPMADMIN_PACKET_PING,			// 2007.07.30. steeple
	AGPMADMIN_PACKET_USER_POSITION_ANSWER,	
	AGPMADMIN_PACKET_USER_POSITION_REQUEST,
	AGPMADMIN_PACKET_ADMIN_CLIENT_LOGIN,
	AGPMADMIN_PACKET_TITLE,
	AGPMADMIN_PACKET_MAX,
} eAgpmAdminPacketType;

// callback function id
typedef enum _eAgpmAdminCB
{
	AGPMADMIN_CB_ADD_ADMIN = 0,
	AGPMADMIN_CB_REMOVE_ADMIN,
	AGPMADMIN_CB_CONNECT,	// 다른 서버 접속용 Callback. - 2004.03.10.
	AGPMADMIN_CB_ADD_CHARACTER,		// 현재 접속자 수 - 2004.03.25.
	AGPMADMIN_CB_REMOVE_CHARACTER,	// 현재 접속자 수 - 2004.03.25.
	AGPMADMIN_CB_NUM_CURRENT_USER,	// 현재 접속자 수 - 2004.03.24.
	AGPMADMIN_CB_SEARCH_CHARACTER,
	AGPMADMIN_CB_SEARCH_RESULT,
	AGPMADMIN_CB_SEARCH_CHARDATA,
	AGPMADMIN_CB_SEARCH_CHARITEM,
	AGPMADMIN_CB_SEARCH_CHARSKILL,
	AGPMADMIN_CB_SEARCH_CUSTOM,
	AGPMADMIN_CB_CHARMOVE,
	AGPMADMIN_CB_CHAREDIT,
	AGPMADMIN_CB_BAN,
	AGPMADMIN_CB_ITEM_CREATE,
	AGPMADMIN_CB_ITEM_RESULT,
	AGPMADMIN_CB_SERVERINFO,
	AGPMADMIN_CB_GUILD,
	AGPMADMIN_CB_PING,
	AGPMADMIN_CB_USER_POSITION,
	AGPMADMIN_CB_USER_POSITION_INFO,
	AGPMADMIN_CB_ADMIN_CLIENT_LOGIN,
	AGPMADMIN_CB_TITLE_EDIT,
	AGPMADMIN_CB_MAX,
} eAgpmAdminCB;

// 2005.05.06. steeple
typedef enum _eAgpmAdminCustomDataType
{
	AGPMADMIN_CUSTOM_DATA_TYPE_SEARCH_RESULT = 0,
	AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_BASIC,
	AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_STAT,
	AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_STATUS,
	AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_COMBAT,
	AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_MONEY,
	AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_SUB,
	AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_SKILL,
	AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_WC,
	AGPMADMIN_CUSTOM_DATA_TYPE_ITEM,
	AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_QUEST,
	AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_TITLE,
	AGPMADMIN_CUSTOM_DATA_TYPE_MAX,
} eAgpmAdminCustomDataType;

// 임시 Level 이다.
typedef enum _eAgpmAdminLevel
{
	AGPMADMIN_LEVEL_ADMIN = 88,
	AGPMADMIN_LEVEL_2,
	AGPMADMIN_LEVEL_3,
	AGPMADMIN_LEVEL_4,
	AGPMADMIN_LEVEL_5,
} eAgpmAdminLevel;

// 2004.03.24. 새로추가.
typedef enum _eAgpmAdminLoginOperation
{
	AGPMADMIN_LOGOUT = 0,
	AGPMADMIN_LOGIN,
} eAgpmAdminLoginOperation;

// Character Item Dlg 의 Position
typedef enum _eAgpmAdminItemPos
{
	AGPMADMIN_ITEM_POS_BODY = 0,
	AGPMADMIN_ITEM_POS_BAG1,
	AGPMADMIN_ITEM_POS_BAG2,
	AGPMADMIN_ITEM_POS_BAG3,
	AGPMADMIN_ITEM_POS_SALESBAG,
	AGPMADMIN_ITEM_POS_STORE,
	AGPMADMIN_ITEM_POS_BOARD,
} eAgpmAdminItemPos;

// Character Edit Field
typedef enum _eAgpmAdminCharEdit
{
	AGPMADMIN_CHAREDIT_HP = 0,
	AGPMADMIN_CHAREDIT_MP,
	AGPMADMIN_CHAREDIT_SP,
	AGPMADMIN_CHAREDIT_LEVEL,
	AGPMADMIN_CHAREDIT_EXP,
	AGPMADMIN_CHAREDIT_MONEY_INV,
	AGPMADMIN_CHAREDIT_MONEY_BANK,
	AGPMADMIN_CHAREDIT_SKILL_LEARN,
	AGPMADMIN_CHAREDIT_SKILL_LEVEL,
	AGPMADMIN_CHAREDIT_SKILL_INIT_ALL,
	AGPMADMIN_CHAREDIT_SKILL_INIT_SPECIFIC,
	AGPMADMIN_CHAREDIT_PRODUCT_EXP_CHANGE,
	AGPMADMIN_CHAREDIT_COMPOSE_LEARN,
	AGPMADMIN_CHAREDIT_COMPOSE_INIT_ALL,
	AGPMADMIN_CHAREDIT_COMPOSE_INIT_SPECIFIC,
	AGPMADMIN_CHAREDIT_CRIMINAL_POINT,
	AGPMADMIN_CHAREDIT_MURDERER_POINT,
	AGPMADMIN_CHAREDIT_CRIMINAL_TIME,
	AGPMADMIN_CHAREDIT_MURDERER_TIME,
	AGPMADMIN_CHAREDIT_WANTED_CRIMINAL,
	AGPMADMIN_CHAREDIT_DB,
	AGPMADMIN_CHAREDIT_DB2,
	AGPMADMIN_CHAREDIT_BANK_MONEY_DB,
	AGPMADMIN_CHAREDIT_CHARISMA_POINT,
	AGPMADMIN_CHAREDIT_CHARISMA_POINT_DB,
	AGPMADMIN_CHAREDIT_RESULT = 99,
} eAgpmAdminCharEdit;

// Help Operation
typedef enum _eAgpmAdminHelpOperation
{
	AGPMADMIN_HELP_LIST = 0,
	AGPMADMIN_HELP_ADD,
	AGPMADMIN_HELP_PROCESS,
	AGPMADMIN_HELP_COMPLETE,
	AGPMADMIN_HELP_REMOVE,
	AGPMADMIN_HELP_DEFER,
	AGPMADMIN_HELP_MEMO,
} eAgpmAdminHelpOperation;

// Help Status
typedef enum _eAgpmAdminHelpStatus
{
	AGPMADMIN_HELP_STATUS_FAILURE = -1,
	AGPMADMIN_HELP_STATUS_READY,
	AGPMADMIN_HELP_STATUS_PROCESS_READY,	// Admin 개인 진정 리스트에서 쓰인다.
	AGPMADMIN_HELP_STATUS_PROCESS_DEFER,	// Admin 개인 진정 리스트에서 쓰인다.
	AGPMADMIN_HELP_STATUS_COMPLETE,
	AGPMADMIN_HELP_STATUS_DEFER,
	AGPMADMIN_HELP_STATUS_DELIVER,
	AGPMADMIN_HELP_STATUS_REJECT,
	AGPMADMIN_HELP_STATUS_DELETE,
} eAgpmAdminHelpStatus;

// Ban Keep Time
typedef enum _eAgpmAdminBanKeepTime
{
	AGPMADMIN_BAN_KEEPTIME_ZERO = 0,
	AGPMADMIN_BAN_KEEPTIME_5H,
	AGPMADMIN_BAN_KEEPTIME_1D,
	AGPMADMIN_BAN_KEEPTIME_5D,
	AGPMADMIN_BAN_KEEPTIME_10D,
	AGPMADMIN_BAN_KEEPTIME_30D,
	AGPMADMIN_BAN_KEEPTIME_90D,
	AGPMADMIN_BAN_KEEPTIME_UNLIMIT,
} eAgpmAdminBanKeepTime;

// Ban Set Flag
typedef enum _eAgpmAdminBanFlag
{
	AGPMADMIN_BAN_FLAG_NO = 0,
	AGPMADMIN_BAN_FLAG_CHAT = 1,
	AGPMADMIN_BAN_FLAG_CHAR = 2,
	AGPMADMIN_BAN_FLAG_ACC = 4,
} eAgpmAdminBanFlag;

// Item Operation
typedef enum _eAgpmAdminItemOperation
{
	AGPMADMIN_ITEM_CREATE = 0, // C -> S
	AGPMADMIN_ITEM_RESULT,	// S -> C
	AGPMADMIN_ITEM_CONVERT,
	AGPMADMIN_ITEM_DELETE,
	AGPMADMIN_ITEM_OPTION_ADD,
	AGPMADMIN_ITEM_OPTION_REMOVE,
	AGPMADMIN_ITEM_UPDATE,
} eAgpmAdminItemOperation;

typedef enum _eAgpmAdminTitleOperation
{
	AGPMADMIN_TITLE_ADD = 0,
	AGPMADMIN_TITLE_USE,
	AGPMADMIN_TITLE_DELETE,
	AGPMADMIN_TITLE_QUEST_ADD,
	AGPMADMIN_TITLE_QUEST_CHECK,
	AGPMADMIN_TITLE_QUEST_DELETE,
	AGPMADMIN_TITLE_QUEST_COMPLETE,
} eAgpmAdminTitleOperation;

// Item Operation Result
typedef enum _eAgpmAdminItemOperationResult
{
	AGPMADMIN_ITEM_RESULT_SUCCESS_CREATE = 0,
	AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT,
	AGPMADMIN_ITEM_RESULT_FAIELD_INVALID_TID,
	AGPMADMIN_ITEM_RESULT_FAILED_INVEN_FULL,
	AGPMADMIN_ITEM_RESULT_FAILED_UNKNOWN,
	AGPMADMIN_ITEM_RESULT_FAILED_CONVERT,	// Item 개조에 실패했다.
	AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_PHY,
	AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_SOCKET,
	AGPMADMIN_ITEM_RESULT_SUCCESS_CONVERT_ADDITEM,
	AGPMADMIN_ITEM_RESULT_SUCCESS_DELETE,
	AGPMADMIN_ITEM_RESULT_FAILED_DELETE,
	AGPMADMIN_ITEM_RESULT_SUCCESS_OPTINO_ADD,
	AGPMADMIN_ITEM_RESULT_SUCCESS_OPTION_REMOVE,
	AGPMADMIN_ITEM_RESULT_FAILED_OPTION_ADD,
	AGPMADMIN_ITEM_RESULT_FAILED_OPTION_REMOVE,
	AGPMADMIN_ITEM_RESULT_SUCCESS_UPDATE,
	AGPMADMIN_ITEM_RESULT_FAILED_UPDATE,
} eAgpmAdminItemOperationResult;

// Guild Operation
typedef enum _eAgpmAdminGuildOperation
{
	AGPMADMIN_GUILD_SEARCH = 1,
	AGPMADMIN_GUILD_SEARCH_PASSWORD,
	AGPMADMIN_GUILD_TOTAL_INFO,
	AGPMADMIN_GUILD_LEAVE_MEMBER,
	AGPMADMIN_GUILD_DESTROY,
	AGPMADMIN_GUILD_CHANGE_MASTER,
	AGPMADMIN_GUILD_CHANGE_NOTICE,
	AGPMADMIN_GUILD_RESULT = 99,
} eAgpmAdminGuildOperation;

typedef enum _eAgpmAdminGuildOperationResult
{
	AGPMADMIN_GUILD_RESULT_SUCCESS_LEAVE_MEMBER = 1,
	AGPMADMIN_GUILD_RESULT_SUCCESS_DESTROY_GUILD,
	AGPMADMIN_GUILD_RESULT_SUCCESS_CHANGE_MASTER,
	AGPMADMIN_GUILD_RESULT_SUCCESS_CHANGE_NOTICE,
	AGPMADMIN_GUILD_RESULT_FAILED_LEAVE_MEMBER,
	AGPMADMIN_GUILD_RESULT_FAILED_DESTROY_GUILD,
	AGPMADMIN_GUILD_RESULT_FAILED_CHANGE_MASTER,
	AGPMADMIN_GUILD_RESULT_FAILED_CHANGE_NOTICE,
} eAgpmAdminGuildOperationResult;

typedef enum _eAgpmAdminItemDeleteReason
{
	AGPMADMIN_ITEM_DELETE_REASON_NONE = 0,
	AGPMADMIN_ITEM_DELETE_REASON_REPAY,
} eAgpmAdminItemDeleteReason;

typedef enum _eAgpmAdminItemCreateReason
{
	AGPMADMIN_ITEM_CREATE_REASON_NONE = 0,
	AGPMADMIN_ITEM_CREATE_REASON_REWARD,
} eAgpmAdminItemCreateReason;


//////////////////////////////////////////////////////////////////////////////////////
// Data Struct
#pragma pack (1)
typedef struct _stAgpdAdminInfo
{
	INT32 m_lCID;
	CHAR m_szAdminName[AGPACHARACTER_MAX_ID_STRING+1];
	INT16 m_lAdminLevel;

	INT16 m_lServerID;
	CHAR m_szAccName[AGPACHARACTER_MAX_ID_STRING+1];
} stAgpdAdminInfo;

typedef struct _stAgpdAdminPickingData
{
	INT32 m_eType;
	INT32 m_lCID;

	CHAR m_szName[AGPACHARACTER_MAX_ID_STRING+1];
} stAgpdAdminPickingData;

typedef struct _stAgpdAdminSearch
{
	INT8 m_iType;
	INT8 m_iField;

	INT32 m_lObjectCID;
	CHAR m_szSearchName[AGPACHARACTER_MAX_ID_STRING+1];
} stAgpdAdminSearch, *pstAgpdAdminSearch;

typedef struct _stAgpdAdminCharDataBasic
{
	INT32 m_lCID;
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 m_lTID;
	CHAR m_szServerName[AGPMADMIN_MAX_SERVERNAME_LENGTH+1];
} stAgpdAdminCharDataBasic, *pstAgpdAdminCharDataBasic;

typedef struct _stAgpdAdminCharDataStat
{
	INT32 m_lCON, m_lSTR, m_lINT, m_lDEX, m_lCHA, m_lWIS;
	INT32 m_lRace, m_lClass;
	
	//CHAR m_szRace[AGPACHARACTER_MAX_ID_STRING+1];
	//CHAR m_szGender[AGPACHARACTER_MAX_ID_STRING+1];
	//CHAR m_szClass[AGPACHARACTER_MAX_ID_STRING+1];
} stAgpdAdminCharDataStat, *pstAgpdAdminCharDataStat;

typedef struct _stAgpdAdminCharDataStatus
{
	INT32 m_lHP, m_lMP;
	INT32 m_lMaxHP, m_lMaxMP;

	INT64 m_llExp, m_llMaxExp;

	INT32 m_lLevel;
	AuPOS m_stPos;	// 현재 좌표

	INT32 m_lSkillPoint;

	BOOL m_bIsTransform;
	INT32 m_lOriginalTID;

	INT32 m_lCriminalPoint;
	INT32 m_lMurdererPoint;

	UINT32 m_lRemainedCriminalTime;
	UINT32 m_lRemainedMurdererTime;
} stAgpdAdminCharDataStatus, *pstAgpdAdminCharaDataStatus;

typedef struct _stAgpdAdminCharDataCombat
{
	// 0:Current Value	1:Original Value
	INT32 m_lNormalAttackDamage[2];
	INT32 m_lDefense[2];
	INT32 m_lSkillBlock[2];
	INT32 m_lPhysicalRes[2];
	INT32 m_lAttackRate[2];
	INT32 m_lAdditionalAR[2];
	INT32 m_lBlock[2];
	INT32 m_lAdditionalEvade[4];	// Evade[2], Dodge[2]
	INT32 m_lAttackSpeed[2];		// %
	INT32 m_lMoveSpeed[2];			// %

	// Attribute. 0:최소공격력 1:최대공격력 2:저항력
	INT32 m_lPhysical[3];
	INT32 m_lMagic[3];
	INT32 m_lFire[3];
	INT32 m_lAir[3];
	INT32 m_lWater[3];
	INT32 m_lEarth[3];
} stAgpdAdminCharDataCombat, *pstAgpdAdminCharDataCombat;

typedef struct _stAgpdAdminCharDataSub
{
	INT32 m_lCID;
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];	// 음... 2003.12.29.
	CHAR m_szAccName[AGPACHARACTER_MAX_ID_STRING+1];	// Basic 에서 이리로 옮겨옴.
	
	UINT32 m_ulConnectedTimeStamp;
	CHAR m_szIP[AGPMADMIN_MAX_IP_STRING+1];
	CHAR m_szLastIP[AGPMADMIN_MAX_IP_STRING+1];

	CHAR m_szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
} stAgpdAdminCharDataSub, *pstAgpdAdminCharDataSub;

// Money 정보만 따로 가진다.
typedef struct _stAgpdAdminCharDataMoney
{
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT64 m_llInvenMoney;
	INT64 m_llBankMoney;
} stAgpdAdminCharDataMoney, *pstAgpdAdminCharDataMoney;

// Move
typedef struct _stAgpdAdminCharDataPosition
{
	CHAR m_szName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szTargetName[AGPMADMIN_MAX_IP_STRING+1];
	FLOAT m_fX, m_fY, m_fZ;
} stAgpdAdminCharDataPosition, *pstAgpdAdminCharDataPosition;

typedef struct _stAgpdAdminCharData
{
	stAgpdAdminCharDataBasic m_stBasic;
	stAgpdAdminCharDataStat m_stStat;
	stAgpdAdminCharDataStatus m_stStatus;
	stAgpdAdminCharDataCombat m_stCombat;
	stAgpdAdminCharDataSub m_stSub;
	stAgpdAdminCharDataMoney m_stMoney;
} stAgpdAdminCharData, *pstAgpdAdminCharData;

typedef struct _stAgpdAdminSearchResult
{
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szAccName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szGuildID[AGPACHARACTER_MAX_ID_STRING+1];

	INT32 m_lTID;
	INT32 m_lLevel;

	CHAR m_szCreDate[AGPMADMIN_MAX_CREDATE_STRING+1];
	CHAR m_szIP[AGPMADMIN_MAX_IP_STRING+1];
	INT32 m_lStatus;

	CHAR m_szServerName[AGPMADMIN_MAX_SERVERNAME_LENGTH+1];
} stAgpdAdminSearchResult;

typedef struct _stAgpdAdminCharItem
{
	INT32 m_lCID;	// Owner CID (Player)
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT8 m_lPos;	// Search Position
} stAgpdAdminCharItem;

typedef struct _stAgpdAdminItemConvertHistory
{
	INT32 m_lNumMagicAttr;
	INT32 m_lNumWaterAttr;
	INT32 m_lNumFireAttr;
	INT32 m_lNumAirAttr;
	INT32 m_lNumEarthAttr;
} stAgpdAdminItemConvertHistory;

typedef struct _stAgpdAdminItemConvertWeaponAttrInfo
{
	INT32 m_lTotalDmgPhysical;
	INT32 m_lTotalAttackSpeed;
	INT32 m_lTotalAP;
	INT32 m_lTotalHPRecovery;
	INT32 m_lTotalMPRecovery;
	INT32 m_lTotalSPRecovery;

	INT32 m_lNumConvertMagic, m_lDmgMagicMin, m_lDmgMagicMax;
	INT32 m_lNumConvertFire, m_lDmgFireMin, m_lDmgFireMax;
	INT32 m_lNumConvertWater, m_lDmgWaterMin, m_lDmgWaterMax;
	INT32 m_lNumConvertAir, m_lDmgAirMin, m_lDmgAirMax;
	INT32 m_lNumConvertEarth, m_lDmgEarthMin, m_lDmgEarthMax;
} stAgpdAdminItemConvertWeaponAttrInfo;

typedef struct _stAgpdAdminItemConvertArmourAttrInfo
{
	INT32 m_lTotalDefPhysical;
	INT32 m_lTotalHPMax;
	INT32 m_lTotalMPMax;
	INT32 m_lTotalSPMax;

	INT32 m_lDefMagic;
	INT32 m_lDefFire;
	INT32 m_lDefWater;
	INT32 m_lDefAir;
	INT32 m_lDefEarth;
} stAgpdAdminItemConvertArmourAttrInfo;

typedef struct _stAgpdAdminItemConvertShieldAttrInfo
{
	INT32 m_lTotalDefPhysical;
	INT32 m_lTotalDefRatePhysical;
	INT32 m_lTotalHPRecovery;
	INT32 m_lTotalMPRecovery;
	INT32 m_lTotalSPRecovery;

	INT32 m_lDefMagic;
	INT32 m_lDefFire;
	INT32 m_lDefWater;
	INT32 m_lDefAir;
	INT32 m_lDefEarth;
} stAgpdAdminItemConvertShieldAttrInfo;

typedef struct _stAgpdAdminItemData
{
	// 소유주
	INT32 m_lOwnerCID;
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];

	// Basic
	INT16 m_lKind;	// Weapon, Armor, Shield
	INT16 m_lPos;
	INT16 m_lLayer, m_lRow, m_lCol;
	CHAR m_szItemName[AGPMITEM_MAX_ITEM_NAME+1];
	INT32 m_lItemID;
	INT32 m_lItemTID;
	INT32 m_lCount;	// 보통 1, Stackable 일때는 1 이상일 수 있다.

	// 공통 속성
	INT32 m_lPrice;
	INT32 m_lRank;
	INT32 m_lDurability;
	INT32 m_lMaxDurability;
	
	INT32 m_lNumPhysicalConvert;
	INT32 m_lNumConvertedSocket;
	INT32 m_lNumSocket;

	// Weapon 쪽 속성
	CHAR m_szWeaponType[AGPMITEM_MAX_ITEM_NAME+1];	// 한손, 두손.
	INT32 m_lAttackRange;
	INT32 m_lPhyMinDmg, m_lPhyMaxDmg;	// 물리 공격력
	INT32 m_lMagicMinDmg, m_lMagicMaxDmg;	// 마법 공격력
	INT32 m_lAttackSpeed;

	// Armor
	INT32 m_lPhyDefense;	// 물리 방어력

	// Shield
	INT32 m_lPhyDefenseRate;	// 블럭

	// Restrict
	INT32 m_lRace;
	INT32 m_lGender;
	INT32 m_lClass;
	CHAR m_szRaceName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szGenderName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szClassName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 m_lNeedLevel, m_lNeedStr, m_lNeedCon, m_lNeedInt, m_lNeedWis, m_lNeedDex;

	// 2004.04.06.
	stAgpdAdminItemConvertHistory m_stConvertHistory;
	stAgpdAdminItemConvertWeaponAttrInfo m_stConvertWeaponAttrInfo;
	stAgpdAdminItemConvertArmourAttrInfo m_stConvertArmourAttrInfo;
	stAgpdAdminItemConvertShieldAttrInfo m_stConvertShieldAttrInfo;
} stAgpdAdminItemData;

// 2005.03.03. steeple
// Convert Socket 정보를 담는다.
typedef struct _stAgpdAdminItemConvertSocketAttr
{
	BOOL m_bIsSpiritStone;
	INT32 m_lTID;
	//PVOID m_pcsItemTemplate;
} stAgpdAdminItemConvertSocketAttr;

// 2005.03.03. steeple
// 쓸모없는 것들은 다 빼버렸다.
typedef struct _stAgpdAdminItem
{
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szAccountName[AGPACHARACTER_MAX_ID_STRING+1];

	UINT64 m_ullDBID;
	INT32 m_lID;
	INT32 m_lTID;
	//PVOID m_pcsTemplate;

	INT16 m_lPos;
	INT16 m_lLayer, m_lRow, m_lCol;
	INT16 m_nPart;		// Equip 아이템일때 Part 를 저장한다. AgpmItemPart 와 같다.
	INT32 m_lCount;

	INT32 m_lPhysicalConvertLevel;
	INT32 m_lNumConvertedSocket;
	INT32 m_lNumSocket;

	stAgpdAdminItemConvertSocketAttr m_stSocketAttr[AGPDITEMCONVERT_MAX_WEAPON_SOCKET+1];	// 현재는 무기 소켓이 젤 많다.

	INT32 m_alOptionTID[AGPDITEM_OPTION_MAX_NUM];				// 서버에서는 UINT8 로 되어 있지만 분명 늘어날 것이얌. -0-
	INT32 m_alSkillPlusTID[AGPMITEM_MAX_SKILL_PLUS_EFFECT];		// 2007.03.19. steeple

	INT32 m_lDurability;		// 내구도. 2006.02.08. 추가
	INT32 m_lMaxDurability;		// 내구도 최대치. 2006.03.25. 추가

	INT8 m_nInUseItem;			//	사용중인지
	INT64 m_lRemainTime;		//	남은 사용기간, milli-seconds
	INT32 m_lExpireTime;		//	완료시간, seconds, time_t
	INT32 m_lCashItemUseCount;	//	사용횟수
	INT64 m_llStaminaRemainTime;//	An amount of remaining stamina

	CHAR m_szServerName[AGPMADMIN_MAX_SERVERNAME_LENGTH+1];
} stAgpdAdminItem;

typedef struct _stAgpdAdminCharSkill
{
	INT32 m_lCID;	// Owner CID (Player)
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT16 m_lType;	// Searcy Type
} stAgpdAdminCharSkill;

typedef struct _stAgpdAdminSkillMastery
{
	INT32 m_lOwnerCID;
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT16 m_lMastery;
	CHAR m_szMasteryName[AGPMSKILL_MAX_MASTERY_NAME+1];
	INT16 m_lUsedSP;
	INT16 m_lAquSkill;
	INT16 m_lSpecialization;
	INT16 m_lTotalSP;
	//CHAR m_szLastUpdate[AGPMADMIN_MAX_UPDATE_STRING+1];
} stAgpdAdminSkillMastery;

typedef struct _stAgpdAdminSkillDataDesc	// 2004.01.14.
{
	INT32 m_lNeedSP;

	INT32 m_lBasicAbilityType;
	INT32 m_lRestrictTimeType;
	INT32 m_lOtherAbilityType;
	INT32 m_lCostType;
	
	INT32 m_lBasicAbilityRange;
	INT32 m_lBasicAbilityArea;
	INT32 m_lBasicAbilityWaterDmg;
	INT32 m_lBasicAbilityFireDmg;
	INT32 m_lBasicAbilityEarthDmg;
	INT32 m_lBasicAbilityAirDmg;
	INT32 m_lBasicAbilityMagicDmg;
	INT32 m_lBasicAbilityDamage;
	
	INT32 m_lRestrictTimeDuration;	// 초단위
	INT32 m_lRestrictTimeDamage;
	INT32 m_lRestrictTimeDefense;
	INT32 m_lRestrictTimeHit;
	INT32 m_lRestrictTimeMoveSpeed;
	INT32 m_lRestrictTimeAttackSpeed;
	INT32 m_lRestrictTimeStr;
	INT32 m_lRestrictTimeCon;
	INT32 m_lRestrictTimeInt;
	INT32 m_lRestrictTimeWiz;
	INT32 m_lRestrictTimeDex;
	INT32 m_lRestrictTimeTransform;
	INT32 m_lRestrictTimeCha;
	INT32 m_lRestrictTimeHP;
	INT32 m_lRestrictTimeReflectDmg;
	
	INT32 m_lOtherAbilityArgo;
	INT32 m_lOtherAbilityStunProb;
	INT32 m_lOtherAbilityMoveTarget;
	INT32 m_lOtherAbilityRemedy;
	INT32 m_lOtherAbilityRecoveryHP;

	INT32 m_lCostHP;
	INT32 m_lCostMP;
	INT32 m_lCostSP;
} stAgpdAdminSkillDataDesc;

typedef struct _stAgpdAdminSkillData
{
	INT32 m_lOwnerCID;
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szSkillName[AGPMSKILL_MAX_SKILL_NAME+1];
	INT32 m_lAquLevel;	// 필요 마스터리 포인트
	INT32 m_lSP;
	UINT64 m_lSkillAttr;
	INT32 m_lSkillLevel;
	CHAR m_szMasteryName[AGPMSKILL_MAX_MASTERY_NAME+1];

	UINT64 m_lRequirementType;
	CHAR m_szSkillDescription[AGPMSKILL_MAX_SKILL_DESCRIPTION+1];

	stAgpdAdminSkillDataDesc m_stCurrentLevelDesc;	// 2004.01.14.
	stAgpdAdminSkillDataDesc m_stNextLevelDesc;	// 2004.01.14.
	
	CHAR m_szTextureName[AGPMADMIN_MAX_PATH+1];
} stAgpdAdminSkillData;

// 2005.02.26. steeple
// 필요없는 데이터들은 다 빼버림.
typedef struct _stAgpdAdminSkill
{
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 m_lTID;
	INT32 m_lLevel;
	PVOID m_pcsTemplate;
} stAgpdAdminSkill;

// 2006.05.16. steeple
// DB 조작용 스킬 스트링. 사이즈 겁나 크다. 하하하핫.
typedef struct _stAgpdAdminSkillString
{
	CHAR m_szCharName[AGPDCHARACTER_NAME_LENGTH + 1];
	CHAR m_szTreeNode[AGPMADMIN_MAX_SKILL_STRING_LENGTH + 1];
	CHAR m_szProduct[AGPMADMIN_MAX_SKILL_STRING_LENGTH + 1];
} stAgpdAdminSkillString;

typedef struct _stAgpdAdminCharPartyMember
{
	INT8 m_lLeader;	// 1 이면 Leader
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szAccName[AGPACHARACTER_MAX_ID_STRING+1];	// 아직 패킷엔 없다. - 2004.03.22.
	CHAR m_szName[AGPACHARACTER_MAX_ID_STRING+1];	// 아직 패킷엔 없다. - 2004.03.22.
	INT32 m_lRace;	// 아직 패킷엔 없다. - 2004.03.22.
	INT32 m_lClass;	// 아직 패킷엔 없다. - 2004.03.22.
	INT32 m_lLevel;
} stAgpdAdminCharPartyMember;

typedef struct _stAgpdAdminCharParty
{
	INT32 m_lCID;	// Owner CID (Player)
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT8 m_lType;	// Search Type
	stAgpdAdminCharPartyMember m_stMemberInfo;
} stAgpdAdminCharParty;

typedef struct _stAgpdAdminCharQuest
{
	INT32 lQuestID;
	INT32 lParam1;
	INT32 lParam2;
	CHAR szNPCName[AGPACHARACTER_MAX_ID_STRING + 1];
} stAgpdAdminCharQuest, *pstAgpdAdminCharQuest;

typedef struct _stAgpdAdminCharTitle
{
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 lTitleTid;
	BOOL bIsTitle;
	BOOL bUseTitle;
	INT32 CheckValue[AGPDTITLE_MAX_TITLE_CHECK];
} stAgpdAdminCharTitle, *psstAgpdAdminCharTitle;

typedef struct _stApgdAdminChatData
{
	INT8 m_lType;
	INT32 m_lSenderID;
	CHAR m_szSenderName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 m_lTargetID;
	CHAR m_szTargetName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szMessage[AGPMADMIN_MAX_CHAT_STRING+1];
	INT32 m_lMessageLength;
} stAgpdAdminChatData, *pstAgpdAdminChatData;

typedef struct _stAgpdAdminCharEdit	// 2003.12.26.
{
	INT32 m_lCID;
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	
	INT16 m_lEditField;
	INT8 m_lEditResult;
	INT32 m_lNewValueINT;
	FLOAT m_lNewValueFloat;
	INT64 m_llNewValueINT64;
	CHAR m_szNewValueChar[AGPMADMIN_MAX_CHAT_STRING+1];	// 뭐 일단 크게 잡는다.
	INT16 m_lNewValueCharLength;
	INT64 m_llNewValueExp;		// Exp 는 따로 간다. ㅜㅜ 매번 이런식으로 예외 추가는 조치안타. 2006.05.04. steeple
} stAgpdAdminCharEdit, *pstAgpdAdminCharEdit;

typedef struct _stAgpdAdminBan
{
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szAccName[AGPACHARACTER_MAX_ID_STRING+1];

	INT8 m_lLogout;	// True 로 세팅 되면, 해당 캐릭터를 Logout 시킨다.

	INT8 m_lBanFlag;	// eAgpmAdminBanFlag

	INT32 m_lChatBanStartTime;
	INT8 m_lChatBanKeepTime;

	INT32 m_lCharBanStartTime;
	INT8 m_lCharBanKeepTime;

	INT32 m_lAccBanStartTime;
	INT8 m_lAccBanKeepTime;
} stAgpdAdminBan, *pstAgpdAdminBan;

typedef struct _stAgpdAdminItemOperation
{
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];	// 대상 캐릭터. 운영자가 될수도 있고 User 가 될수도 있다.
	CHAR m_szAccName[AGPACHARACTER_MAX_ID_STRING+1];	// 2005.05.18. steeple. DB 에 쓰려니 Account 필요해서 넣었음.

	INT8 m_cOperation;
	INT8 m_cReason;
	UINT64 m_ullDBID;
	INT32 m_lID;
	INT32 m_lTID;
	INT8 m_cResult;
	INT32 m_lCount;

	// 개조, 삭제할 때 Base 가 되는 아이템. Equip 아이템은 Inven 으로 내리고 개조한다.
	INT16 m_lPos;
	INT16 m_lLayer, m_lRow, m_lColumn;
	INT16 m_nPart;		// Equip 일 때 위치. AgpmItemPart 와 같다.

	// 2005.03.03. steeple
	INT8 m_cPhysicalConvertLevel;
	INT8 m_cSocket;
	INT32 m_lUsableItemTID;

	INT32 m_alOptionTID[AGPDITEM_OPTION_MAX_NUM];		// 2005.04.11. steeple
	INT32 m_alSkillPlusTID[AGPMITEM_MAX_SKILL_PLUS_EFFECT];

	INT32 m_lDurability;			//	Durability
	INT32 m_lMaxDurability;		// 내구도 최대치. 2006.03.25. 추가

	INT8 m_nInUseItem;				//	사용중인지
	INT64 m_lRemainTime;			//	남은 사용기간, milli-seconds
	UINT32 m_lExpireTime;			//	완료시간, seconds, time_t
	//INT32 m_lCashItemUseCount;	//	사용횟수		// 여기선 필요 없다.
	INT64 m_llStaminaRemainTime;	//	An amount of remaining stamina

	stAgpdAdminItem m_stOriginalItem;
} stAgpdAdminItemOperation;

typedef struct _stAgpdAdminItemTemplate
{
	INT32 m_lTID;
	CHAR m_szItemName[AGPMITEM_MAX_ITEM_NAME+1];

	INT16 m_nType;
	INT16 m_nUsableItemType;

	CHAR m_szTexturePath[AGPMADMIN_MAX_PATH+1];
} stAgpdAdminItemTemplate, *pstAgpdAdminItemTemplate;

typedef struct _stAgpdAdminServerInfo
{
	CHAR m_szServerIP[AGPMADMIN_MAX_IP_STRING+1];
	INT16 m_lPort;
	
	// No Packet Element
	INT32 m_lNID;	
} stAgpdAdminServerInfo, *pstAgpdAdminServerInfo;

// 2007.07.30. steeple
typedef struct _stAgpdAdminPing
{
	UINT32 m_ulTimeStamp;
} stAgpdAdminPing;

typedef struct _stAgpdAdminTitleOperation
{
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR m_szAccName[AGPACHARACTER_MAX_ID_STRING+1];

	INT8 m_cOperation;
	INT32 lTitleTid;
	BOOL bIsTitle;
	BOOL bUseTitle;
	INT32 CheckValue[AGPDTITLE_MAX_TITLE_CHECK];
} stAgpdAdminTitleOperation, *pstAgpdAdminTitleOperation;

#pragma pack ()


/////////////////////////////////////////////////////////////////////////////////////
// AgpdAdmin
class AgpdAdmin
{
public:
	INT32 m_lAdminCID;
	INT16 m_lAdminLevel;
	INT8 m_lLoginCheck;	// 로그인 했는지 안했는지
	
	UINT32 m_ulNID;	// 따로 연결한 접속의 NID 를 저장한다.
};

#endif // _AGPDADMIN_H_
