// AgsmAdmin.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 04.

#ifndef _AGSMADMIN_H_
#define _AGSMADMIN_H_

#include "AuPacket.h"

#include "AgpmConfig.h"
#include "ApmEventManager.h"
#include "AgpmCharacter.h"
#include "AgpmChatting.h"
#include "AgpmGuild.h"
#include "AgpmItemConvert.h"
#include "AgpmAdmin.h"
#include "AgpmAI2.h"
#include "AgpmCashMall.h"
#include "AgpmProduct.h"
#include "AgpmLog.h"
#include "AgpmSummons.h"
#include "AgpmSiegeWar.h"
#include "AgpmWantedCriminal.h"
#include "AgpmMailBox.h"

#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmChatting.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmSkill.h"
#include "AgsmEventSkillMaster.h"
#include "AgsmParty.h"
#include "AgsmDeath.h"
#include "AgsmEventSpawn.h"
#include "AgsmGuild.h"
//#include "AgsmAdminDB.h"
//#include "AgpmItemLog.h"
#include "AgsmServerManager2.h"
#include "AgsmRide.h"
#include "AgsmQuest.h"
#include "AgsmSummons.h"
#include "AgsmRemission.h"
#include "AgsmInterServerLink.h"
#include "AgsmSystemMessage.h"
#include "AgsmWantedCriminal.h"
#include "AgsmEventNPCTrade.h"
#include "AgsmEventBank.h"
#include "AgsmAreaChatting.h"

#include "ApServerMonitorPacket.h"

#include <vector>
#include <set>

#include "AgpmScript.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgsmAdminD.lib")
#else
	#pragma comment(lib, "AgsmAdmin.lib")
#endif
#endif

#define AGSMADMIN_HELP_RECV_MSG									"진정 접수가 되었습니다. 순서를 기다려 주시기 바랍니다."

#define AGSMADMIN_TIMEINTERVAL_CURRENT_USERCOUNT				300000	// ms 단위 - 즉, 5분임
#define AGSMADMIN_TIMEINTERVAL_CURRENT_USERCOUNT_FOR_EXCEL		30000	// ms 단위 - 30 초마다 한번씩 체크해본다. 남기는 건 30분 단위
#define AGSMADMIN_TIMEINTERVAL_PING								1000	// ms 단위. 다른 서버와의 Ping 주기
#define AGSMADMIN_DISCONNECT_LIMIT_TIME							5 * 60	// s  단위. 5분 동안 Ping 이 없으면 접속을 끊어버린다.

#define AGSMADMIN_FILENAME_CURRENT_USERCOUNT					"current_usercount_"
#define AGSMADMIN_FILENAME_CURRENT_USERCOUNT_FOR_EXCEL			"current_usercount_for_excel_"

#define AGSMADMIN_MAX_PING_QUEUE_COUNT							20

#define	AGSMADMIN_COMMAND_LOG_FILE								"log\\AdminCommand.log"

// Packet Operation
typedef enum _eAgsmAdminPacketType
{
	AGSMADMIN_PACKET_HELP_PRIVATE_LIST_REQUEST = 0,
	AGSMADMIN_PACKET_HELP_PRIVATE_ADD,
	AGSMADMIN_PACKET_HELP_PRIVATE_COUNT,
	AGSMADMIN_PACKET_HELP_PRIVATE_INSERT_DB,
	AGSMADMIN_PACKET_HELP_PRIVATE_UPDATE_DB,
	AGSMADMIN_PACKET_CONNECTABLE_ACCOUNT_LEVEL,
	AGSMADMIN_PACKET_MAX,
} eAgsmAdminPackeType;

// Callback ID
typedef enum _eAgsmAdminCallback
{
	AGSMADMIN_CB_BAN_UPDATE_DB = 0,
	AGSMADMIN_CB_SEARCH_ACCOUNT,
	AGSMADMIN_CB_CONCURRENT_USER,
	AGSMADMIN_CB_PING_RELAY_SERVER,

	AGSMADMIN_CB_RELAY_SEARCH_ACCOUNT,
	AGSMADMIN_CB_RELAY_SEARCH_ACCOUNT_BANK,		// 2005.09.14. steeple 추가
	AGSMADMIN_CB_RELAY_SEARCH_CHARACTER,
	AGSMADMIN_CB_RELAY_UPDATE_CHARACTER,
	AGSMADMIN_CB_RELAY_UPDATE_CHARACTER2,
	AGSMADMIN_CB_RELAY_UPDATE_BANK_MONEY,
	AGSMADMIN_CB_RELAY_UPDATE_CHARISMA_POINT,
	AGSMADMIN_CB_RELAY_ITEM_INSERT,
	AGSMADMIN_CB_RELAY_ITEM_DELETE,
	AGSMADMIN_CB_RELAY_ITEM_CONVERT_UPDATE,
	AGSMADMIN_CB_RELAY_ITEM_CONVERT_UPDATE2,
	AGSMADMIN_CB_RELAY_ITEM_UPDATE,
	AGSMADMIN_CB_RELAY_SKILL_UPDATE,

	AGSMADMIN_CB_START_SIEGEWAR,
	AGSMADMIN_CB_END_SIEGEWAR,
	AGSMADMIN_CB_END_ARCHLORD_BATTLE,

	AGSMADMIN_CB_ARCHLORD_SET_GUARD,
	AGSMADMIN_CB_ARCHLORD_CANCEL_GUARD,
	AGSMADMIN_CB_ARCHLORD_GUARD_INFO,
	AGSMADMIN_CB_ARCHLORD_SET_ARCHLORD,
	AGSMADMIN_CB_ARCHLORD_CANCEL_ARCHLORD,
	AGSMADMIN_CB_ARCHLORD_START,
	AGSMADMIN_CB_ARCHLORD_END,
	AGSMADMIN_CB_ARCHLORD_NEXT_STEP,
	AGSMADMIN_CB_SET_CASTLE_OWNER,
	AGSMADMIN_CB_CANCEL_CASTLE_OWNER,

	AGSMADMIN_CB_MAX,
} eAgsmAdminCallback;

typedef enum _eAgsmAdminCharTID
{
	AGSMADMIN_CHARTID_HMK = 0,
	AGSMADMIN_CHARTID_HMA,
	AGSMADMIN_CHARTID_HWS,
	AGSMADMIN_CHARTID_OMK,
	AGSMADMIN_CHARTID_OMS,
	AGSMADMIN_CHARTID_OWA,
	AGSMADMIN_CHARTID_MER,
	AGSMADMIN_CHARTID_MEE,
	AGSMADMIN_CHARTID_MEK,
	AGSMADMIN_CHARTID_MAX,
};

const int AGSDADMIN_CHARTID[AGSMADMIN_CHARTID_MAX] = {96, 1, 6, 4, 3, 8, 460, 9, 377};

typedef struct _stAgsdAdminCurrentUser
{
	INT32 m_lServerID;
	INT32 m_lTotalUserCount;
	INT32 m_lHighestUserCount;
	INT32 m_lUserCountTID[AGSMADMIN_CHARTID_MAX];
	//INT32 m_lHumanUserCount;
	//INT32 m_lOrcUserCount;
} stAgsdAdminCurrentUser;

//////////////////////////////////////////////////////////////////////////
//
enum _eAgsmAdminCommands
{
	ACOMMAND_cha_up,
	ACOMMAND_battleground,
	ACOMMAND_bg_start,
	ACOMMAND_ebg_start,
	ACOMMAND_bg_end,
	ACOMMAND_ebg_end,
	ACOMMAND_expup,
	ACOMMAND_reload,
	ACOMMAND_crash,
	ACOMMAND_enable_pvp,
	ACOMMAND_disable_pvp,
	ACOMMAND_cast_skill,
	ACOMMAND_disable_chat,
	ACOMMAND_product_skill_up,
	ACOMMAND_murderpoint_up,
	ACOMMAND_cashmall,
	ACOMMAND_WinGuild,
	ACOMMAND_MAX
};

enum AgsmAdminCastSkill
{
	AGSM_ADMIN_CAST_SKILL_NONE	 = 0,
	AGSM_ADMIN_CAST_SKILL_NOCHAT = 1359		// 우수스의 경고 (Disable Chatting)
};

class AgsdAdminPing
{
public:
	DWORD m_dwLastPingTickCount;
	DWORD m_dwLastReceivePingTickCount;
	ApSafeArray<DWORD, AGSMADMIN_MAX_PING_QUEUE_COUNT> m_arrPingSendQueue;	// Ping 보낸 시간을 저장해 놓는다.
	INT32 m_lPingCurrentQueueCount;
	INT32 m_lPingTime;		// Ping 응답속도
	INT32 m_lPingSendCount;
	INT32 m_lPingReceiveCount;

public:
	AgsdAdminPing() { init(); }
	~AgsdAdminPing() {;}

	void init()
	{
		m_dwLastPingTickCount = 0;
		m_dwLastReceivePingTickCount = 0;
		m_arrPingSendQueue.MemSetAll();
		m_lPingCurrentQueueCount = 0;
		m_lPingTime = m_lPingSendCount = m_lPingReceiveCount = 0;
	}
};

struct AgsdAdminEventItem
{
	INT32 m_lTID;
	INT32 m_lCount;
	INT32 m_ISet;

	AgsdAdminEventItem() : m_lTID(0), m_lCount(0), m_ISet(0) {}
};

class AgsmBattleGround;
class AgsmConfig;
class AgsmNpcManager;
class AgpmBuddy;

class AgpmPvP;

class AgpmEpicZone;
class AgsmEpicZone;
class AgpmTitle;
class AgsmTitle;

class AgsmAdmin : public AgsModule
{
public:
	static INT32	s_lCharTID[AGSMADMIN_CHARTID_MAX];
	
private:
	AgpmLog* m_pagpmLog;
	

	ApmMap*					m_papmMap;
	ApmEventManager* m_papmEventManager;
	AgpmFactors* m_pagpmFactors;
	AgpmCharacter* m_pagpmCharacter;
	AgpmChatting* m_pagpmChatting;
	AgpmItem* m_pagpmItem;
	AgpmGrid* m_pagpmGrid;
	AgpmItemConvert* m_pagpmItemConvert;
	AgpmParty* m_pagpmParty;
	AgpmGuild* m_pagpmGuild;
	//AgpmItemLog* m_pagpmItemLog;
	AgpmSkill* m_pagpmSkill;
	AgpmEventSkillMaster* m_pagpmEventSkillMaster;
	AgpmEventSpawn* m_pagpmEventSpawn;
	AgpmAdmin* m_pagpmAdmin;
	AgpmAI2* m_pagpmAI2;
	AgpmScript* m_pagpmScript;
	AgpmCashMall* m_pagpmCashMall;
	AgpmProduct* m_pagpmProduct;
	AgpmSiegeWar* m_pagpmSiegeWar;
	AgpmSummons* m_pagpmSummons;
	AgpmWantedCriminal* m_pagpmWantedCriminal;
	AgpmMailBox* m_pagpmMailBox;

	AgsmAOIFilter* m_pagsmAOIFilter;
	AgsmCharManager* m_pagsmCharManager;
	AgsmCharacter* m_pagsmCharacter;
	AgsmChatting* m_pagsmChatting;
	AgsmFactors* m_pagsmFactors;
	AgsmItem* m_pagsmItem;
	AgsmItemManager* m_pagsmItemManager;
	AgsmSkill* m_pagsmSkill;
	AgsmEventSkillMaster* m_pagsmEventSkillMaster;
	AgsmParty* m_pagsmParty;
	AgsmDeath* m_pagsmDeath;
	AgsmEventSpawn* m_pagsmEventSpawn;
	AgsmGuild* m_pagsmGuild;
	AgsmRide*	m_pagsmRide;
	AgsmQuest*	m_pagsmQuest;
	AgsmSummons* m_pagsmSummons;
	AgsmRemission* m_pagsmRemission;
	AgsmSystemMessage* m_pagsmSystemMessage;
	AgsmWantedCriminal* m_pagsmWantedCriminal;
	AgsmEventNPCTrade* m_pagsmEventNPCTrade;
	AgsmEventBank* m_pagsmEventBank;
	AgpmBattleGround* m_pagpmBattleGround;
	AgsmBattleGround* m_pagsmBattleGround;
	
	AgsmServerManager* m_pAgsmServerManager;
	AgsmInterServerLink* m_pAgsmInterServerLink;

	AgpmConfig*	m_pagpmConfig;
	AgsmConfig* m_pagsmConfig;
	AgsmNpcManager* m_pagsmNpcManager;

	AgpmPvP* m_pagpmPvP;
	AgsmAreaChatting* m_pagsmAreaChatting;
	AgpmBuddy* m_pagpmBuddy;

	AgpmEpicZone* m_pagpmEpicZone;
	AgsmEpicZone* m_pagsmEpicZone;

	AgpmTitle* m_pagpmTitle;
	AgsmTitle* m_pagsmTitle;

	AuGenerateID m_csGenerateHelpCount;

	// Current User 관련
	ApMutualEx		m_csUserLock;
	stAgsdAdminCurrentUser m_stTotalUser;
	//ApAdmin m_csCurrentUserAdmin;
	DWORD m_dwLastWriteLogCurrentUserTickCount;
	INT32 m_lThisServerUserCount;	// 2004.05.21.
	
	DWORD m_dwLastWriteLogCurrentUserTickCountForExcel;
	INT32 m_lLastMinuteWriteLogCurrentUserForExcel;

	// Account Level 관련
	INT32 m_lConnectableAccountLevel;

	// Ping Relay
	ApMutualEx m_csPingRelayLock;
	AgsdAdminPing m_csPingRelay;

	// Ping Client
	UINT32 m_ulLastSendPingClient;
	std::set<UINT32> m_setServerNID;

	// Notice Repeat
	AssmNoticeVector	m_vtNotice;
	ApCriticalSection	m_csNoticeCS;

	// Event Items
	typedef std::vector<AgsdAdminEventItem>		AdminEventItems;
	typedef AdminEventItems::iterator			AdminEventItemIter;

	AdminEventItems		m_vtEventItems;

	typedef std::map<std::string, _eAgsmAdminCommands> AdminCommandsMap;
	AdminCommandsMap m_AdminCommandString;

	typedef std::vector<string>	m_csAllowedIPList; 
	m_csAllowedIPList m_csAllowedAdminIPList;	// admin client 접속 가능 ip 리스트.

	void InitCommandString();

public:
	AuPacket m_csPacket;

public:
	AgsmAdmin();
	virtual ~AgsmAdmin();

	// Virtual Function
	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle(UINT32 ulClockCount);
	BOOL OnDestroy();

	BOOL InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag);
	void PrintCurrentUserCount(FILE* pfFile = stdout);

protected:
	INT32 GetThisServerID();
	INT32 GetRelayServerID();
	UINT32 GetGameServerNID(INT32 nGameServerID);
	UINT32 GetRelayServerNID();
	UINT32 GetLoginServerNID(INT16 nIndex = 0);
	BOOL IsGameServer();
	BOOL IsRelayServer();
	BOOL IsThisServerCharacter(AgpdCharacter* pcsAgpdCharacter);

	UINT32 GetCharacterNID(AgpdCharacter* pcsAgpdCharacter);
	INT16 GetCharacterServerID(AgpdCharacter* pcsAgpdCharacter);

	void UserLock();
	void UserUnlock();

public:
	INT32 GetNumCurrentUser();

	////////////////////////////////////////////////////////////////////////////////////////
	// Operation
	BOOL AddAdmin(AgpdCharacter* pcsAgpdCharacter);	// 운영자 로그인 - AgpmAdmin 에서
	BOOL EnterGameWorld(AgpdCharacter* pcsAgpdCharacter);	// 운영자 로그인 - AgsmCharManager 에서 불린다. cpp 파일도 보자.
	BOOL RemoveAdmin(INT32 lRemoveCID);	// 운영자 로그아웃 - AgpmAdmin 에서
	BOOL DisconnectAdmin(AgpdCharacter* pcsAgpdCharacter);	// 운영자 로그아웃 - AgsmCharManager 에서 불린다.
	BOOL Connect(AgpdCharacter* pcsAgpdCharacter, UINT32 ulNID);	// 운영자 다른 서버에 접속.

	BOOL AddCharacter(AgpdCharacter* pcsAgpdCharacter);	// 접속자 수 관리를 위해. - 2004.03.25.
	BOOL RemoveCharacter(AgpdCharacter* pcsAgpdCharacter);	// 접속자 수 관리를 위해. - 2004.03.25.

	BOOL LoadAllowedAdminIPList();
	BOOL IsAllowedIP(const char *szIPAddress);

	void WriteLogCurrentUserCount();	// 현재 접속자 수의 로그를 남긴다. - 2004.04.12.
	void WriteLogCurrentUserCountForExcel();	// 현재 접속자 수를 액셀형식에 맞게 남긴다. - 2004.05.24.
	void MakeExcelTime(CHAR* szExcelTime, INT32 lExcelTimeLength, CHAR* szTimeDate);

	// 운영자 명령어를 파싱/처리 한다.
	BOOL ParseCommand(AgpdChatData * pstChatData, BOOL bCheckAdmin = TRUE);
	BOOL ProcessCommandNotice(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandNoticeRepeat(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandNoticeList(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandNoticeDelete(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandNoticeClear(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandCreate(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength, INT32 lCount);
//	BOOL ProcessCommandQCreate(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandOpen(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandGuild(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandMove(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandUpdateLevel(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandSpawn(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandRide(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandSummons(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandQuest(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandCreateItemAll(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);
	BOOL ProcessCommandCashItemTimeExtend(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength, BOOL bCheckAdmin);
	BOOL ProcessCommandClearInventory(AgpdCharacter* pcsAgpdCharacter, UINT32 ulInvenLayerNum = 0);
	BOOL ProcessCommandClearCashInventory(AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessCommandOpenLotteryBox(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength);

	INT32 ProcessStringToken(CHAR* szMessage, INT32 lMessageLength, CHAR** szToken, INT32 lTokenNum, CHAR cTokener);
	INT32 ProcessStringToken(CHAR* szMessage, CHAR** szToken, INT32 lTokenNum, CHAR cTokener);

	// Search
	BOOL SearchCharacter(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsApgdCharacter);
	BOOL SearchCharacterFromDB(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter);
	BOOL SearchCharacterResultDB(CHAR* szName, INT32 lCID);		// 2003.11.23. 김태희 AgsmAdminDB 에서 불리게 된다.
	BOOL SearchCharacterByAccount(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter);
	BOOL SearchCharacterByAccount2(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter);
	BOOL SearchCharacterDB(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter);
	BOOL SearchAccountBank(CHAR* szAccountName, AgpdCharacter* pcsAgpdCharacter);

	// Search From DB (Relay)
	BOOL ProcessSearchAccountDB(CHAR* szAdminCharName, CHAR* szAccountName, stAgpdAdminSearchResult* pstSearchResult);
	BOOL ProcessSearchCharacterDB(CHAR* szAdminCharName, CHAR* szCharName, stAgpdAdminSearchResult* pstSearchResult,
								stAgpdAdminCharDataBasic* pstBasic, stAgpdAdminCharDataStatus* pstStatus,
								stAgpdAdminCharDataStat* pstStat, stAgpdAdminCharDataMoney* pstMoney,
								CHAR* szTreeNode, CHAR* szProduct);
	BOOL ProcessSearchCharItemDB(CHAR* szAdminCharName, CHAR* szTargetName, stAgpdAdminItem* pstItem);
	BOOL ProcessSearchCharTitleDB(CHAR* szAdminCharName, CHAR* szTargetName, stAgpdAdminCharTitle* pstTitle);
	
	// Move
	BOOL ProcessCharacterMove(stAgpdAdminCharDataPosition* pstPosition, AgpdCharacter* pcsAgpdCharacter);

	// Edit
	BOOL ProcessCharacterEdit(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessCharacterEditToDB(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessCharacterEditToDB2(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessCharismaEditToDB(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessCharSkillEdit(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessWCEdit(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter);

	// Ban
	BOOL ProcessBan(stAgpdAdminBan* pstBan, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessBanApplyCharacter(stAgpdAdminBan* pstBan, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessBanGetData(stAgpdAdminBan* pstBan, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessBanReCalcTime(AgpdCharacter* pcsAgpdCharacter);
	INT32 ProcessBanGetKeepTime(INT8 cKeepTime);
	BOOL ProcessBanUpdateDB(stAgpdAdminBan* pstBan);
	
	// Item
	BOOL ProcessItemOperation(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessItemCreate(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter, BOOL bCheckAdmin = TRUE, BOOL bIncludeOffLine = TRUE, AgpdCharacter* pcsTarget = NULL);
	BOOL ProcessItemCreateToDB(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessItemConvert(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessItemConvertPhysical(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem, INT32 lPhysicalConvertLevel);
	BOOL ProcessItemConvertSocket(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem, INT32 lSocket);
	BOOL ProcessItemConvertAddItem(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem, INT32 lAddItemTID, AgpdCharacter *pcsAdmin);
	BOOL ProcessItemConvertToDB(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessItemDelete(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessItemDeleteToDB(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessItemOptionAdd(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessItemOptionRemove(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessItemUpdate(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessItemUpdateToDB(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);

	AgpdItem* FindItemByPosition(stAgpdAdminItemOperation* pstItemOperation, AgpdCharacter* pcsAgpdCharacter);
	BOOL ConvertItemData(AgpdItem* pcsItem, stAgpdAdminItem* pstItem);

	// Skill
	BOOL ProcessSkillLearn(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessSkillLevel(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessSkillInitAll(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessSkillInitSpecific(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessComposeLearn(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessSkillQuery(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter);

	BOOL ProcessTitleOperation(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessTitleOperationAdd(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessTitleOperationUse(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessTitleOperationDelete(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessTitleOperationQuestAdd(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessTitleOperationQuestEdit(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessTitleOperationQuestDelete(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessTitleOperationQuestComplete(stAgpdAdminTitleOperation* pstTitle, AgpdCharacter* pcsAgpdCharacter);

	// Server Info
	BOOL ProcessSendServerInfo(AgpdCharacter* pcsAgpdCharacter);
	BOOL GetIPAddressFromAgsdServer(stAgpdAdminServerInfo* pstServerInfo, AgsdServer* pcsAgsdServer);
	INT16 GetPortFromAgsdServer(AgsdServer* pcsAgsdServer);

	// Guild
	BOOL ProcessGuildOperation(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessGuildSearch(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessGuildSearchPassword(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessGuildTotalInfo(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessGuildMemberLeave(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessGuildDestroy(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessGuildChangeMaster(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter);
	BOOL ProcessGuildChangeNotice(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter);

	// Ping Relay - Check Alive
	void ProcessPingRelaySend();
	void ProcessPingRelayReceive(DWORD dwSendTickCount);
	void GetPingRelayInfo(AgsdAdminPing* pcsAgsdAdminPing);

	// Ping to Client - 2007.07.30. steeple
	void ProcessPingClient(UINT32 ulClockCount);
	void ProcessPingClientReceive(stAgpdAdminPing& stPing, UINT32 ulNID);

	// Notice Repeat
	void ProcessNoticeRepeat();
	void ClearNoticeRepeat();
	INT32 GetNoticeCount();
	BOOL EnumNotice(ApModuleDefaultCallBack pfCallback, PVOID pClass, PVOID pCustData);

	AssmNoticeVector *	LockNotice();
	VOID				UnlockNotice();

	// Event Items
	void LoadEventItem();
	BOOL ProcessEventItem(INT32 m_ISet);

	//////////////////////////////////////////////////////////////////////////
	// Packet Make
	PVOID MakePacket(INT16* pnPacketLength, INT8* pcOperation, INT32* plCID, INT32* plConnectedAccountLevel, PVOID pvHelp);

	////////////////////////////////////////////////////////////////////////////////////////
	// Packet Receive - Server Packet
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL OnReceiveConnectableAccountLevel(INT32 lCID, INT32 lLevel);

	////////////////////////////////////////////////////////////////////////////////////////
	// Packet Send - Server Packet
	BOOL SendConnectableAccountLevel(INT32 lLevel, INT32 lCID, UINT32 ulNID);


	///////////////////////////////////////////////////////////////////////////////////////////
	// Packet Send - Public Packet
	BOOL SendAddAdmin(stAgpdAdminInfo* pstAdminInfo, INT32 lCID, UINT32 ulNID);
	BOOL SendRemoveAdmin(stAgpdAdminInfo* pstAdminInfo, INT32 lCID, UINT32 ulNID);

	BOOL SendAddAdminToAllAdmin(stAgpdAdminInfo* pstAdminInfo, AgpdCharacter* pcsAgpdCharacter);
	BOOL SendRemoveAdminToAllAdmin(stAgpdAdminInfo* pstAdminInfo, AgpdCharacter* pcsAgpdCharacter);
	BOOL SendAdminList(AgpdCharacter* pcsAgpdCharacter);

	BOOL SendNumCurrentUser(INT32 lCurrentUser, INT32 lCID, UINT32 ulNID);	// 2004.03.24. 현재 접속자 수를 보낸다.

	BOOL SendSearchOtherServer(stAgpdAdminSearch* pstSearch, INT32 lCID, UINT32 ulNID);
	BOOL SendSearchResult(AgpdCharacter* pcsAgpdCharacter, INT32 lCID, UINT32 ulNID);
	BOOL SendSearchResult(CHAR* szAdminName, CHAR* szCharName, CHAR* szAccName, INT32 lLevel, INT32 lTID,
							INT32 lStatus, CHAR* szCreationDate);	// 대략 뒤의 정보들을 Admin 에게 보내준다. AgsmRelay 에서 부른다.
	BOOL SendSearchCharDataSub(AgpdCharacter* pcsAgpdCharacter, INT32 lCID, UINT32 ulNID);
	BOOL SendSearchCharMoney(AgpdCharacter* pcsAgpdCharacter, INT32 lCID, UINT32 ulNID);

	BOOL SendSearchCharItem(stAgpdAdminCharItem* pstItem, AgpdCharacter* pcsAgpdAdminCharacter);
	BOOL SendSearchCharItem(AgpdItem* pcsAgpdItem, INT32 lCID, UINT32 ulNID);
	
	BOOL SendCharAllItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID);
	BOOL SendCharEquipItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID);
	BOOL SendCharInventoryItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID);
	BOOL SendCharBankItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID);
	BOOL SendCharSalesBoxItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID);
	BOOL SendCharMailBoxItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID);
	BOOL SendCharCashItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID);
	BOOL SendCharPetInventoryItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID);
	BOOL SendGridItemUseCustom(AgpdGrid* pcsGrid, INT32 lCID, UINT32 ulNID);

	BOOL SendCharWantedCriminal(CHAR* szCharName, INT32 lCID, UINT32 ulNID);

	BOOL SendCharQuestInfo(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID);

	BOOL SendCharTitleInfo(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID);

	BOOL SendSearchCharSkill(stAgpdAdminCharSkill* pstSkill, AgpdCharacter* pcsAgpdAdminCharacter);
	
	BOOL SendSearchCharParty(stAgpdAdminCharParty* pstParty, AgpdCharacter* pcsAgpdAdminCharacter);
	BOOL SendSearchCharParty(AgpdCharacter* pcsSearchCharacter, AgpdCharacter* pcsAgpdCharacter);
	BOOL SendSearchCharParty(stAgpdAdminCharParty* pstParty, INT32 lCID, UINT32 ulNID);

	BOOL SendCustomPacket(INT32 lCustomType, PVOID pvCustomData, INT16 nDataLength, INT32 lCID, UINT32 ulNID);

	BOOL SendCharEditResult(stAgpdAdminCharEdit* pstCharEdit, INT32 lCID, UINT32 ulNID);

	// Ban
	BOOL SendBanData(stAgpdAdminBan* pstBan, INT32 lCID, UINT32 ulNID);

	// Item
	BOOL SendItemOperationResult(INT32 lTID, eAgpmAdminItemOperationResult eResult, INT32 lCount, INT32 lCID, UINT32 ulNID);
	BOOL SendItemOperationResult(stAgpdAdminItemOperation* pstItemOperation, INT32 lCID, UINT32 ulNID);

	// Server Info
	BOOL SendServerInfo(stAgpdAdminServerInfo* pstServerInfo, INT32 lCID, UINT32 ulNID);

	// Guild
	BOOL SendGuildOperation(stAgpdAdminCharEdit* pstCharEdit, INT32 lCID, UINT32 ulNID);

	// Ping
	BOOL SendPing(stAgpdAdminPing& stPing, UINT32 ulNID);

	BOOL SendAdminClientLoginOK(AgpdCharacter* pstCharacter, UINT32 ulNID);



	// Account Level
	BOOL SetConnectableAccountLevel(INT32 lLevel);
	INT32 GetConnectableAccountLevel();


	// Log
	BOOL WriteLog_Char(eAGPDLOGTYPE_PLAY eLogType, AgpdCharacter *pcsAdmin, AgpdCharacter *pcsTarget);
	BOOL WriteLog_Char(eAGPDLOGTYPE_PLAY eLogType, AgpdCharacter *pcsAdmin, CHAR *pszCharName, INT32 lTID, INT32 lLevel, INT64 llExp, INT64 llMoney);
	BOOL WriteLog_Item(eAGPDLOGTYPE_ITEM eLogType, AgpdCharacter *pcsAdmin, AgpdCharacter *pcsTarget, AgpdItem *pcsItem, INT32 lQuantity);
	BOOL WriteLog_Item(eAGPDLOGTYPE_ITEM eLogType, AgpdCharacter *pcsAdmin, CHAR *pszAccountID, CHAR *pszCharName, AgpdItem *pcsItem, INT32 lQuantity);
	BOOL WriteLog_Item(eAGPDLOGTYPE_ITEM eLogType, AgpdCharacter *pcsAdmin, CHAR *pszAccountID, CHAR *pszCharName, UINT64 ullItemSeq, CHAR *pszConvert, CHAR *pszOption, CHAR* pszPosition);
	BOOL WriteLog_Gheld(eAGPDLOGTYPE_GHELD eLogType, AgpdCharacter *pcsAdmin, AgpdCharacter *pcsTarget);

	//////////////////////////////////////////////////////////////////////////
	// Callback Registration Function
	// For AgsmMakeSQL
	BOOL SetCallbackUpdateCharacterBan(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// For AgsmRelay
	BOOL SetCallbackSearchAccount(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackPingSend(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelaySearchAccount(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelaySearchAccountBank(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelaySearchCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelayUpdateCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelayUpdateCharacter2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelayUpdateBankMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelayUpdateCharismaPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelayItemInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelayItemDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelayItemConvertUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelayItemConvertUpdate2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelayItemUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRelaySkillUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//
	BOOL SetCallbackConcurrentUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackStartSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEndSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEndArchlordBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackArchlordSetGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackArchlordCancelGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackArchlordGuardInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackArchlordSetArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackArchlordCancelArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackArchlordStart(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackArchlordEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackArchlordNextStep(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSetCastleOwner(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCancelCastleOwner(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	///////////////////////////////////////////////////////////////////////////////////////
	// CallBack Function
	// From AgpmChatting
	static BOOL CBRecvNormalMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBIsAdminCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// From AgpmAdmin
	static BOOL CBAddAdmin(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveAdmin(PVOID pData, PVOID pClass, PVOID pCustData);
	
	static BOOL CBDisconnectAdmin(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBConnect(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAddCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBNumCurrentUser(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUserPosition(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBSearchCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSearchCharItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSearchCharSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSearchCharParty(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCharMove(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCharEdit(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBBan(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBItemOperation(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBGuildOperation(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBPingOperation(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgsmChatting
	static BOOL CBCheckAdminCommand(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgsmAdminDB
	static BOOL CBSearchCharacterResultDB(PVOID pData, PVOID pClass, PVOID pCustData);

	// For AgsmLoginDB
	static BOOL CBIsConnectableAccountLevel(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// For AsEchoServer
	//static BOOL CBGetCurrentUserCount(PVOID pData, PVOID pClass, PVOID pCustData);

	// For AgsmRelay
	static BOOL CBPingRelaySend(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRelaySearchAccount(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRelaySearchCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	// for event
	static BOOL CBReceiveEventMessage(PVOID pData, PVOID pClass, PVOID pCustData);

	// For AgsmInterServerLink
	static BOOL CBOnConnectServer(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBOnDisconnectServer(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBOnAdminClientLogin(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBOnTitleEdit(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif //_AGSMADMIN_H_
