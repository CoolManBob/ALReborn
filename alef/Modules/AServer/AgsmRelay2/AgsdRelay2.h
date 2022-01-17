/*======================================================================

	AgsdRelay2.h

======================================================================*/

#ifndef _AGSD_RELAY2_H_
	#define _AGSD_RELAY2_H_


#include "ApBase.h"
#include "AgpdLog.h"
#include "AgsdDatabase.h"
#include "AgsdAccount.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include "AgpdGuild.h"
#include "AgpdMailBox.h"
#include "AgpdQuestFlag.h"
#include "AgpdAdmin.h"
#include "AgsdSkill.h"
#include "AgsdItem.h"
#include "AgpdTax.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
typedef enum _eAgsmRelay2Param
	{
	AGSMRELAY_PARAM_BASE = AGSMDATABASE_PARAM_RELAY,		// 1
	AGSMRELAY_PARAM_CHARACTER,								// 2
	AGSMRELAY_PARAM_ITEM,									// 3
	AGSMRELAY_PARAM_ITEM_CONVERT_HISTORY,					// 4
	AGSMRELAY_PARAM_SKILL,									// 5
	AGSMRELAY_PARAM_LOGIN,									// 6
	AGSMRELAY_PARAM_UI_STATUS,								// 7
	AGSMRELAY_PARAM_GUILD_MASTER,							// 8
	AGSMRELAY_PARAM_GUILD_MEMBER,							// 9
	AGSMRELAY_PARAM_ACCOUNT_WORLD,							// 10
	AGSMRELAY_PARAM_QUEST,									// 11
	AGSMRELAY_PARAM_QUEST_FLAG,								// 12
	AGSMRELAY_PARAM_REVERSEORB,								// 13
	AGSMRELAY_PARAM_GUILD_MASTER_CHECK,						// 14
	AGSMRELAY_PARAM_CONCURRENT_USER,						// 15
	AGSMRELAY_PARAM_TRANSFORM,								// 16
	AGSMRELAY_PARAM_CHARGAMEIN,								// 17
	AGSMRELAY_PARAM_CHARGAMEOUT,							// 18
	AGSMRELAY_PARAM_GUILD_BATTLE_HISTORY,					// 19 - 2005.04.17. steeple
	AGSMRELAY_PARAM_GUILD_RENAME,							// 20
	AGSMRELAY_PARAM_BUDDY,									// 21
	AGSMRELAY_PARAM_MAIL,									// 22
	AGSMRELAY_PARAM_MAIL_ITEM,								// 23
	AGSMRELAY_PARAM_CASHITEM,								// 24
	AGSMRELAY_PARAM_CASHITEMBUYLIST,						// 25
	AGSMRELAY_PARAM_CASHITEMBUYLIST2,						// 26
	AGSMRELAY_PARAM_WANTEDCRIMINAL,							// 27
	AGSMRELAY_PARAM_GUILD_WAREHOUSE_MONEY,					// 28
	AGSMRELAY_PARAM_GUILD_WAREHOUSE_ITEM,					// 29
	//============================================================
	AGSMRELAY_PARAM_LOG_PLAY = 30,							// 30
	AGSMRELAY_PARAM_LOG_ITEM,								// 31
	AGSMRELAY_PARAM_LOG_ETC,								// 32
	AGSMRELAY_PARAM_LOG_GHELD,								// 33
	AGSMRELAY_PARAM_LOG_CASHITEM,							// 34
	AGSMRELAY_PARAM_LOG_PCROOM,								// 35
	
	AGSMRELAY_PARAM_CASTLE = 50,							// 50								
	AGSMRELAY_PARAM_SIEGE,									// 51
	AGSMRELAY_PARAM_SIEGE_APPLICATION,						// 52
	AGSMRELAY_PARAM_SIEGE_OBJECT,							// 53
	AGSMRELAY_PARAM_TAX,									// 54
	AGSMRELAY_PARAM_ARCHLORD,								// 55
	AGSMRELAY_PARAM_LORDGUARD,								// 56

	AGSMRELAY_PARAM_PING = 60,								// 60
	AGSMRELAY_PARAM_ADMIN,									// 61. Query 는 없고 Callback 용으로만 쓰인다.
	AGSMRELAY_PARAM_ADMIN_ACCOUNT,							// 62
	AGSMRELAY_PARAM_ADMIN_CHAR,								// 63
	AGSMRELAY_PARAM_ADMIN_CHAR2,							// 64
	AGSMRELAY_PARAM_ADMIN_CHAR_ITEM,						// 65
	AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_BANK,					// 66
	AGSMRELAY_PARAM_ADMIN_CHAR_ITEM_CASH,					// 67

	AGSMRELAY_PARAM_NOTIFY_SAVE_ALL,						// 67. Query 는 없고 Callback 용으로만 쓰인다.
	AGSMRELAY_PARAM_GUILD_RELATION,							// 69
	AGSMRELAY_PARAM_GUILD_MEMBER_NAVER,						// 70. naver_charmaster join query

	AGSMRELAY_PARAM_SKILL_SAVE,						// 71 save skills - arycoat 2008.7
	AGSMRELAY_PARAM_WORLD_CHAMPIONSHIP,						// 72 World Championship - arycoat 2008.8
	
	//////////////////////////////////////////////////////////////////////////
	//
	AGSMRELAY_PARAM_ATTENDANCE,								// 출석 체크 시스템 - arycoat 2009.2
	AGSMRELAY_PARAM_EVENT_ITEM,								// 아이템부여 시스템 - absenty 2009.3
	AGSMRELAY_PARAM_CHANGENAME,								// 캐릭명 변경 시스템 - arycoat 2009.4.

	AGSMRELAY_PARAM_CASHITEMTIMEEXTEND,						// 기간제 Cashitem 시간연장 - 박경도 2009.4.

	AGSMRELAY_PARAM_CERARIUMORB,							// 캐릭터 봉인 시스템 - arycoat 2009.4.

	AGSMRELAY_PARAM_ADMIN_EDIT_BANK_MONEY,					// 창고 겔드 수정.

	AGSMRELAY_PARAM_TITLE,

	AGSMRELAY_PARAM_ADMIN_CHAR_TITLE,						// 80. 어드민 클라, 타이틀 조회

	AGSMRELAY_PARAM_CHARACTER_CREATION_DATE,				// 81. 캐릭터 생성 시간 체크

	AGSMRELAY_PARAM_SERVERMOVE,								// 특성화 서버 서버이동 //JK_특성화서버

	} eAgsmRelay2Param;


#define		_MAX_CHARNAME_LENGTH			AGPDCHARACTER_NAME_LENGTH
#define		_MAX_TITLE_LENGTH				AGPACHARACTER_MAX_CHARACTER_SKILLINIT
#define		_MAX_NICKNAME_LENGTH			AGPACHARACTER_MAX_CHARACTER_NICKNAME
#define		_MAX_REASON_LENGTH				AGPMITEM_MAX_DELETE_REASON
#define		_MAX_GUILDID_LENGTH				AGPMGUILD_MAX_GUILD_ID_LENGTH
#define		_MAX_GUILDPW_LENGTH				AGPMGUILD_MAX_PASSWORD_LENGTH
#define		_MAX_DATETIME_LENGTH			32
#define		_MAX_POSITION_LENGTH			32
#define		_MAX_TREENODE_LENGTH			AGSMSKILL_MAX_SKILLTREE_LENGTH
#define		_MAX_CONVERT_HISTORY_LENGTH		AGPDLOG_MAX_ITEM_CONVERT_STRING
#define		_MAX_QUICKBELT_LENGTH			512
#define		_MAX_COOLDOWN_LENGTH			512
#define		_I64_STR_LENGTH					20
#define		_MAX_SERVERNAME_LENGTH			32
#define		_MAX_OPTION_LENGTH				32
#define		_ctextend						'\0'




/********************************************/
/*		The Definition of Parameters		*/
/********************************************/
//
//	==========		Base		==========
//
//	class AgsdDBParam
//	class AgsdDBParamCustom
//			See AgsdDatabase.h of AgsmDatabsePool


//
//	==========		Character		==========
//
class AgsdRelay2Character : public AgsdDBParam, public ApMemory<AgsdRelay2Character, 30000>
	{
	public:
		CHAR	m_szName[_MAX_CHARNAME_LENGTH + 1];
		CHAR	m_szSkillInit[_MAX_TITLE_LENGTH + 1];
		CHAR	m_szNickName[_MAX_NICKNAME_LENGTH + 1];
		CHAR	m_szPosition[_MAX_POSITION_LENGTH + 1];
		INT8	m_cCriminalStatus;
		INT32	m_lMurderPoint;
		CHAR	m_szInventoryMoney[_I64_STR_LENGTH + 1];
		UINT32	m_ulRemainCriminalTime;
		UINT32	m_ulRemainMurderTime;
		INT32	m_lHP;
		INT32	m_lMP;
		INT32	m_lSP;
		CHAR	m_szExp[_I64_STR_LENGTH + 1];
		INT64	m_llExp;
		INT32	m_lLevel;
		INT32	m_lSkillPoint;
		UINT8	m_ucRegionIndex;
		INT8	m_ucActionStatus;
		INT32	m_lHair;
		INT32	m_lFace;
		INT8	m_cDeadType;
		INT32	m_lCharismaPoint;
		INT32	m_lHeroicPoint;
		UINT32	m_ulBattleSquareLastKilledTime;
		
	protected:
		BOOL	SetParamUpdate(AuStatement *pStatement);

	public:
		AgsdRelay2Character();
		virtual ~AgsdRelay2Character()	{}
		virtual void Release()	{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


//
//	==========		Item		==========
//
class AgsdRelay2Item : public AgsdDBParam, public ApMemory<AgsdRelay2Item, 30000>
	{
	public:
		CHAR	m_szDBID[_I64_STR_LENGTH + 1];
		CHAR	m_szAccount[_MAX_CHARNAME_LENGTH + 1];
		CHAR	m_szServerName[_MAX_SERVERNAME_LENGTH + 1];
		CHAR	m_szCharName[_MAX_CHARNAME_LENGTH + 1];
		INT32	m_lTemplateID;
		INT32	m_lStackCount;
		INT16	m_nStatus;
		CHAR	m_szPosition[_MAX_POSITION_LENGTH + 1];
		INT32	m_lNeedLevel;
		INT32	m_lDurability;
		INT32	m_lMaxDurability;
		CHAR	m_szDeleteReason[_MAX_REASON_LENGTH + 1];
		INT32	m_lStatusFlag;
		CHAR	m_szOption[_MAX_OPTION_LENGTH + 1];
		CHAR	m_szSkillPlus[_MAX_OPTION_LENGTH + 1];
		INT16	m_nInUse;
		INT32	m_lUseCount;
		INT64	m_lRemainTime;
		UINT32	m_lExpireTime;
		INT64	m_llStaminaRemainTime;

		CHAR	m_szConvertHistory[_MAX_CONVERT_HISTORY_LENGTH + 1];
		
		// dummy
		CHAR	m_szExpireTime[_MAX_DATETIME_LENGTH + 1];
		CHAR	m_szConvertDummy[_MAX_CONVERT_HISTORY_LENGTH + 1];

	protected:
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamDelete(AuStatement *pStatement);

	public:
		AgsdRelay2Item();
		virtual ~AgsdRelay2Item()	{}
		virtual void Release()	{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


//
//	==========		Item Convert		==========
//
class AgsdRelay2ItemConvert : public AgsdDBParam, public ApMemory<AgsdRelay2ItemConvert, 20000>
	{
	public:
		CHAR	m_szDBID[_I64_STR_LENGTH + 1];
		CHAR	m_szConvertHistory[_MAX_CONVERT_HISTORY_LENGTH + 1];

	protected:
		BOOL	SetParamUpdate(AuStatement *pStatement);

	public:
		AgsdRelay2ItemConvert();
		virtual ~AgsdRelay2ItemConvert()	{}
		virtual void Release()	{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


//
//	==========		Skill		==========
//
class AgsdRelay2Skill : public AgsdDBParam, public ApMemory<AgsdRelay2Skill, 20000>
	{
	public:
		CHAR	m_szCharName[_MAX_CHARNAME_LENGTH + 1];
		CHAR	m_szTreeNode[_MAX_TREENODE_LENGTH + 1];
		CHAR	m_szProduct[_MAX_TREENODE_LENGTH + 1];

	protected:
		BOOL	SetParamUpdate(AuStatement *pStatement);

	public:
		AgsdRelay2Skill();
		virtual ~AgsdRelay2Skill()	{}
		virtual void Release()	{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


//
//	==========		UI Status		==========
//
class AgsdRelay2UIStatus : public AgsdDBParam, public ApMemory<AgsdRelay2UIStatus, 20000>
	{
	public:
		CHAR	m_szCharName[_MAX_CHARNAME_LENGTH + 1];
		CHAR	m_szQuickBelt[_MAX_QUICKBELT_LENGTH + 1];
		CHAR	m_szCooldown[_MAX_COOLDOWN_LENGTH + 1];

	protected:
		BOOL	SetParamUpdate(AuStatement *pStatement);

	public:
		AgsdRelay2UIStatus();
		virtual ~AgsdRelay2UIStatus()	{}
		virtual void Release()	{ delete this; }
	};


//
//	==========		Guild Master		==========
//
class AgsdRelay2GuildMaster : public AgsdDBParam, public ApMemory<AgsdRelay2GuildMaster, 10000>
	{
	public:
		CHAR	m_szGuildID[_MAX_GUILDID_LENGTH + 1];
		CHAR	m_szMasterID[_MAX_CHARNAME_LENGTH + 1];
		INT32	m_lGuildTID;
		INT32	m_lGuildRank;
		INT32	m_lCreationDate;
		INT32	m_lMaxMemberCount;
		INT32	m_lUnionID;
		CHAR	m_szPassword[_MAX_GUILDPW_LENGTH + 1];
		
		// 2005.04.17. steeple
		INT8	m_cStatus;
		INT32	m_lWin;
		INT32	m_lDraw;
		INT32	m_lLose;
		CHAR	m_szEnemyGuildID[_MAX_GUILDID_LENGTH + 1];
		CHAR	m_szBattleStartTime[AGPMGUILD_MAX_DATE_LENGTH + 1];
		UINT32	m_ulBattleDuration;
		INT32	m_lMyScore;
		INT32	m_lEnemyScore;

		BOOL	m_bCheckSelect;
		BOOL	m_bBattleHistoryInsert;

		//	2005.10.19. By SungHoon
		INT32	m_lGuildMarkTID;
		INT32	m_lGuildMarkColor;
		
		INT64	m_llMoney;
		CHAR	m_szMoney[_I64_STR_LENGTH + 1];
		INT32	m_lSlot;

		INT32	m_lGuildPoint;

	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamDelete(AuStatement *pStatement);

	public:
		AgsdRelay2GuildMaster();
		virtual ~AgsdRelay2GuildMaster()	{}
		virtual void Release()	{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


//
//	==========		Guild Member		==========
//
class AgsdRelay2GuildMember : public AgsdDBParam, public ApMemory<AgsdRelay2GuildMember, 10000>
	{
	public:
		CHAR	m_szMemberID[_MAX_CHARNAME_LENGTH + 1];
		CHAR	m_szGuildID[_MAX_GUILDID_LENGTH + 1];
		INT32	m_lMemberRank;
		INT32	m_lJoinDate;
		INT32	m_lLevel;
		INT32	m_lTID;
		CHAR	m_szJoinDate[AGPMGUILD_MAX_DATE_LENGTH + 1];			//	2005.07.28. SungHoon

	protected:
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamDelete(AuStatement *pStatement);

	public:
		AgsdRelay2GuildMember();
		virtual ~AgsdRelay2GuildMember()	{}
		virtual void Release()	{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


//
//	==========		Quest		==========
//
class AgsdRelay2Quest : public AgsdDBParam, public ApMemory<AgsdRelay2Quest, 20000>
	{
	public:
		CHAR	m_szCharID[_MAX_CHARNAME_LENGTH + 1];
		INT32	m_lQuestTID;
		//CHAR	m_szStartDate[AUDB_MAX_DATETIME_SIZE];
		//CHAR	m_szEndDate[AUDB_MAX_DATETIME_SIZE];
		INT32	m_lArg1;
		INT32	m_lArg2;
		CHAR	m_szNPC[_MAX_CHARNAME_LENGTH + 1];

	protected:
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamDelete(AuStatement *pStatement);

	public:
		AgsdRelay2Quest();
		virtual ~AgsdRelay2Quest()	{}
		virtual void Release()	{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


class AgsdRelay2QuestFlag : public AgsdDBParam, public ApMemory<AgsdRelay2QuestFlag, 20000>
	{
	public:
		CHAR	m_szCharID[_MAX_CHARNAME_LENGTH + 1];
		CHAR	m_szQuestBlock1[AGSDQUEST_MAX_FLAG + 1];
		CHAR	m_szQuestBlock2[AGSDQUEST_MAX_FLAG + 1];
		CHAR	m_szQuestBlock3[AGSDQUEST_MAX_FLAG + 1];
	protected:
		BOOL	SetParamUpdate(AuStatement *pStatement);
	
	public:
		AgsdRelay2QuestFlag();
		virtual ~AgsdRelay2QuestFlag()	{}
		virtual void Release()		{ delete this; }

	};


//
//	==========		Buddy		==========
//
class AgsdRelay2Buddy : public AgsdDBParam, public ApMemory<AgsdRelay2Buddy, 20000>
	{
	public:
		ApString<AGPDCHARACTER_NAME_LENGTH> m_szCharID;
		ApString<AGPDCHARACTER_NAME_LENGTH> m_szBuddyID;
		DWORD m_dwOptions;
		INT32 m_nMentorStatus;
		PVOID pvRowsetPacket;

	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamDelete(AuStatement *pStatement);

	public:
		AgsdRelay2Buddy();
		virtual ~AgsdRelay2Buddy()	{}
		virtual void Release()		{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


//
//	==========		Mail		==========
//
class AgsdRelay2Mail : public AgsdDBParam, public ApMemory<AgsdRelay2Mail, 20000>
	{
	public:
		INT32		m_lCID;
		PVOID		m_pvPacketEmb;
		AgpdMail	m_AgpdMail;
		INT32		m_lItemID;
		INT16		m_nStatus;

		// dummy
		CHAR		m_szDate[_MAX_DATETIME_LENGTH + 1];
		CHAR		m_szDBID[_I64_STR_LENGTH + 1];
		CHAR		m_szItemSeq[_I64_STR_LENGTH + 1];
		
		// result code
		INT32		m_lResult;

	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamDelete(AuStatement *pStatement);

	public:
		AgsdRelay2Mail();
		virtual ~AgsdRelay2Mail()	{}
		virtual void Release()		{ delete this; }

		virtual void	Dump(CHAR *pszOp);	
	};


//
//	==========		Mail Item		==========
//
class AgsdRelay2MailItem : public AgsdDBParam, public ApMemory<AgsdRelay2MailItem, 20000>
	{
	public:
		INT32		m_lCID;
		INT32		m_lMailID;
		
		// in
		UINT64		m_ullDBID;
		UINT64		m_ullItemSeq;
		INT32		m_lItemTID;
		INT32		m_lItemQty;
		CHAR		m_szCharID[_MAX_CHARNAME_LENGTH + 1];
		INT16		m_nStatus;

		// out
		INT32		m_lNeedLevel;
		CHAR		m_szConvert[_MAX_CONVERT_HISTORY_LENGTH + 1];
		INT32		m_lDurability;
		INT32		m_lMaxDurability;
		INT32		m_lFlag;
		CHAR		m_szOption[_MAX_OPTION_LENGTH + 1];
		CHAR		m_szSkillPlus[_MAX_OPTION_LENGTH + 1];
		
		INT32		m_lInUse;
		INT32		m_lUseCount;
		INT64		m_lRemainTime;
		CHAR		m_szExpireDate[_MAX_DATETIME_LENGTH + 1];
		INT64		m_llStaminaRemainTime;
		
		// dummy
		CHAR		m_szDBID[_I64_STR_LENGTH + 1];
		CHAR		m_szItemSeq[_I64_STR_LENGTH + 1];
		
		// result code
		INT32		m_lResult;

	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);

	public:
		AgsdRelay2MailItem();
		virtual ~AgsdRelay2MailItem()	{}
		virtual void Release()		{ delete this; }

		virtual void	Dump(CHAR *pszOp);	
	};


//
//	==========		Cash Item Buy List		==========
//
class AgsdRelay2CashItemBuyList : public AgsdDBParam, public ApMemory<AgsdRelay2CashItemBuyList, 20000>
	{
	public:
		stCashItemBuyList	m_stList;
		CHAR	m_szAccountID[AGSMACCOUNT_MAX_ACCOUNT_NAME + 1];
		CHAR	m_szCharID[_MAX_CHARNAME_LENGTH + 1];
		INT32	m_lCharTID;
		INT32	m_lCharLevel;
		CHAR	m_szIP[15];

		// dummy
		CHAR	m_szBuySeq[_I64_STR_LENGTH + 1];
		CHAR	m_szItemSeq[_I64_STR_LENGTH + 1];
		CHAR	m_szMoney[_I64_STR_LENGTH + 1];
		CHAR	m_szOrderNo[_I64_STR_LENGTH + 1];
		CHAR	m_szStatus[11];

	protected:
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamInsert(AuStatement *pStatement);
	
	public:
		AgsdRelay2CashItemBuyList();
		virtual ~AgsdRelay2CashItemBuyList()	{}
		virtual void Release()	{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


//
//	==========		Wanted Criminal		==========
//
class AgsdRelay2WantedCriminal : public AgsdDBParam, public ApMemory<AgsdRelay2WantedCriminal, 20000>
	{
	public:
		CHAR	m_szCharID[_MAX_CHARNAME_LENGTH + 1];
		INT64	m_llMoney;
		INT32	m_lCharTID;
		INT32	m_lCharLevel;
		PVOID	m_pvPacketEmb;		
	
		// dummy
		CHAR	m_szMoney[_I64_STR_LENGTH + 1];
		
	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamDelete(AuStatement *pStatement);
		BOOL	SetParamExecute(AuStatement *pStatement);
	
	public:
		AgsdRelay2WantedCriminal();
		virtual ~AgsdRelay2WantedCriminal()	{}
		virtual void Release()	{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


//
//	==========		Account World		==========
//
class AgsdRelay2AccountWorld : public AgsdDBParam, public ApMemory<AgsdRelay2AccountWorld, 20000>
	{
	public:
		CHAR	m_szAccountID[AGSMACCOUNT_MAX_ACCOUNT_NAME + 1];
		CHAR	m_szServerName[_MAX_SERVERNAME_LENGTH + 1];
		CHAR	m_szBankMoney[_I64_STR_LENGTH + 1];
		INT8	m_cBankSize;

	protected:
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamInsert(AuStatement *pStatement);

	public:
		AgsdRelay2AccountWorld();
		virtual ~AgsdRelay2AccountWorld()	{}
		virtual void Release()	{ delete this; }

		virtual void	Dump(CHAR *pszOp);
	};


//
//	==========		Castle		==========
//
class AgsdRelay2Castle : public AgsdDBParam, public ApMemory<AgsdRelay2Castle, 1000>
	{
	public :
		CHAR	m_szCastleID[AGPMSIEGEWAR_MAX_CASTLE_NAME + 1];
		CHAR	m_szOwnerGuildID[_MAX_GUILDID_LENGTH + 1];

		INT32	m_lInd;					// for select result(rowset)
		PVOID	m_pvPacketEmb;

	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);

	public:
		AgsdRelay2Castle();
		virtual ~AgsdRelay2Castle()	{}
		virtual void Release()	{ delete this; }	
	};


//
//	==========		Siege		==========
//
class AgsdRelay2Siege : public AgsdDBParam, public ApMemory<AgsdRelay2Siege, 1000>
	{
	public :
		CHAR	m_szCastleID[AGPMSIEGEWAR_MAX_CASTLE_NAME + 1];
		UINT32	m_ulSiegeID;
		UINT64	m_ullProclaimDate;
		UINT64	m_ullApplyBeginDate;
		UINT32	m_ulApplyElapsed;
		UINT64	m_ullSiegeBeginDate;
		UINT32	m_ulSiegeElapsed;
		UINT64	m_ullSiegeEndDate;
		UINT64	m_ullPrevEndDate;
		INT32	m_lStatus;
		CHAR	m_szLastCarveGuildID[_MAX_GUILDID_LENGTH + 1];

		INT32	m_lInd;					// for select result(rowset)
		PVOID	m_pvPacketEmb;
		
		CHAR	m_szProclaimDate[_MAX_DATETIME_LENGTH + 1];		// dummy
		CHAR	m_szApplyBeginDate[_MAX_DATETIME_LENGTH + 1];
		CHAR	m_szSiegeBeginDate[_MAX_DATETIME_LENGTH + 1];
		CHAR	m_szSiegeEndDate[_MAX_DATETIME_LENGTH + 1];
		CHAR	m_szPrevEndDate[_MAX_DATETIME_LENGTH + 1];

	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);

	public:
		AgsdRelay2Siege();
		virtual ~AgsdRelay2Siege()	{}
		virtual void Release()	{ delete this; }	
	};


//
//	==========		Siege Application		==========
//
class AgsdRelay2SiegeApplication : public AgsdDBParam, public ApMemory<AgsdRelay2SiegeApplication, 1000>
	{
	public :
		CHAR	m_szCastleID[AGPMSIEGEWAR_MAX_CASTLE_NAME + 1];
		UINT32	m_ulSiegeID;
		CHAR	m_szGuildID[_MAX_GUILDID_LENGTH + 1];
		INT32	m_lSide;
		UINT64	m_ullApplyDate;
		INT32	m_lAdopted;

		INT32	m_lInd;					// for select result(rowset)
		PVOID	m_pvPacketEmb;

		CHAR	m_szApplyDate[_MAX_DATETIME_LENGTH + 1];	// dummy

	protected:
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamSelect(AuStatement *pStatement);

	public:
		AgsdRelay2SiegeApplication();
		virtual ~AgsdRelay2SiegeApplication()	{}
		virtual void Release()	{ delete this; }	
	};


//
//	==========		Siege Object		==========
//
class AgsdRelay2SiegeObject : public AgsdDBParam, public ApMemory<AgsdRelay2SiegeObject, 10000>
	{
	public :
		CHAR	m_szCastleID[AGPMSIEGEWAR_MAX_CASTLE_NAME + 1];
		INT32	m_lObjectID;
		INT32	m_lTID;
		INT32	m_lHP;
		CHAR	m_szPosition[_MAX_POSITION_LENGTH + 1];
		INT32	m_lStatus;
		UINT64	m_ulSpecialStatus;

		INT32	m_lInd;					// for select result(rowset)
		PVOID	m_pvPacketEmb;

		// dummy
		CHAR	m_szSpecialStatus[_I64_STR_LENGTH + 1];

	protected:
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamDelete(AuStatement *pStatement);
		BOOL	SetParamSelect(AuStatement *pStatement);

	public:
		AgsdRelay2SiegeObject();
		virtual ~AgsdRelay2SiegeObject()	{}
		virtual void Release()	{ delete this; }	
	};


//
//	==========		Concurrent User		==========
//
class AgsdRelay2ConcurrentUser : public AgsdDBParam, public ApMemory<AgsdRelay2ConcurrentUser, 20000>
	{
	public:
		CHAR	m_szServer[21];
		CHAR	m_szType[21];
		INT32	m_lCurrent;
		INT32	m_lMax;

	protected:
		BOOL	SetParamUpdate(AuStatement *pStatement);

	public:
		AgsdRelay2ConcurrentUser();
		virtual ~AgsdRelay2ConcurrentUser()	{}
		virtual void Release()	{ delete this; }
	};


//
//	==========		Tax		==========
//
class AgsdRelay2Tax : public AgsdDBParam, public ApMemory<AgsdRelay2Tax, 10>
	{
	public :
		CHAR	m_szCastleID[AGPMSIEGEWAR_MAX_CASTLE_NAME + 1];
		INT64	m_llTotalIncome;
		CHAR	m_szTotalIncome[_I64_STR_LENGTH];
		UINT32	m_ulLatestTransferDate;
		UINT32	m_ulLatestModifyDate;
		
		CHAR	m_szRegionNames[AGPDTAX_REGION_MAX][_MAX_GUILDID_LENGTH + 1];
		INT32	m_lRegionRatios[AGPDTAX_REGION_MAX];
		INT64	m_llRegionIncomes[AGPDTAX_REGION_MAX];
		CHAR	m_szRegionIncomes[AGPDTAX_REGION_MAX][_I64_STR_LENGTH];
		
		// dummy
		CHAR	m_szLatestTransferDate[_MAX_DATETIME_LENGTH + 1];
		CHAR	m_szLatestModifyDate[_MAX_DATETIME_LENGTH + 1];

	protected:
		BOOL	SetParamUpdate(AuStatement *pStatement);

	public:
		AgsdRelay2Tax();
		virtual ~AgsdRelay2Tax()	{}
		virtual void Release()	{ delete this; }
	};


//
//	==========		Guild Item		==========
//
class AgsdRelay2GuildItem : public AgsdDBParam, public ApMemory<AgsdRelay2GuildItem, 10>
	{
	public :
		CHAR	m_szGuildID[_MAX_GUILDID_LENGTH + 1];
		INT32	m_lCID;
		PVOID	m_pvPacketEmb;
		INT32	m_lInd;
		
	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);

	public:
		AgsdRelay2GuildItem();
		virtual ~AgsdRelay2GuildItem()	{}
		virtual void Release()	{ delete this; }
	};


//
//	==========		Archlord		==========
//
class AgsdRelay2Archlord : public AgsdDBParam, public ApMemory<AgsdRelay2Archlord, 10>
	{
	public :
		INT32	m_lCID;
		CHAR	m_szCharID[_MAX_CHARNAME_LENGTH + 1];
		CHAR	m_szGuildID[_MAX_GUILDID_LENGTH + 1];
		INT32	m_ulSiegeID;
		PVOID	m_pvPacketEmb;
		INT32	m_lInd;
		
	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);
		BOOL	SetParamUpdate(AuStatement *pStatement);
		BOOL	SetParamInsert(AuStatement *pStatement);

	public:
		AgsdRelay2Archlord();
		virtual ~AgsdRelay2Archlord()	{}
		virtual void Release()	{ delete this; }
	};


//
//	==========		Lord Guard		==========
//
class AgsdRelay2LordGuard : public AgsdDBParam, public ApMemory<AgsdRelay2LordGuard, 10>
	{
	public :
		CHAR	m_szCharID[_MAX_CHARNAME_LENGTH + 1];
		PVOID	m_pvPacketEmb;
		INT32	m_lInd;
		
	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);
		BOOL	SetParamInsert(AuStatement *pStatement);
		BOOL	SetParamDelete(AuStatement *pStatement);

	public:
		AgsdRelay2LordGuard();
		virtual ~AgsdRelay2LordGuard()	{}
		virtual void Release()	{ delete this; }
	};


//
//	==========		Log Base		==========
//
class AgsdRelay2Log : public AgsdDBParam
	{
	public:
		CHAR		m_szDate[23];
		CHAR		m_szGheldInven[_I64_STR_LENGTH + 1];
		CHAR		m_szGheldBank[_I64_STR_LENGTH + 1];
		CHAR		m_szExp[_I64_STR_LENGTH + 1];

	public:
		AgsdRelay2Log()
			{
			ZeroMemory(m_szDate, sizeof(m_szDate));
			ZeroMemory(m_szGheldInven, sizeof(m_szGheldInven));
			ZeroMemory(m_szGheldBank, sizeof(m_szGheldBank));
			ZeroMemory(m_szExp, sizeof(m_szExp));
			}
		~AgsdRelay2Log()	{}
		
		virtual	BOOL	WriteToFile()	{	return FALSE;	}
	};


//
//	==========		Log Play		==========
//
class AgsdRelay2Log_Play : public AgsdRelay2Log, public ApMemory<AgsdRelay2Log_Play, 30000>
	{
	public:
		AgpdLog_Play	m_csLog;

	protected:
		BOOL	SetParamInsert(AuStatement *pStatement);

	public:
		AgsdRelay2Log_Play()	{ m_eOperation = AGSMDATABASE_OPERATION_INSERT; }
		AgsdRelay2Log_Play(AgpdLog* pLog);
		virtual ~AgsdRelay2Log_Play()	{}

		virtual void	Release()	{ delete this; }

		virtual void	SetStaticQuery(CHAR *pszDest, CHAR *pszFmt);
		virtual	BOOL	WriteToFile();
	};


//
//	==========		Log Item		==========
//
class AgsdRelay2Log_Item : public AgsdRelay2Log, public ApMemory<AgsdRelay2Log_Item, 70000>
	{
	public:
		AgpdLog_Item		m_csLog;
		CHAR				m_szItemSeq[_I64_STR_LENGTH + 1];
		CHAR				m_szExpireDate[23];

	protected:
		BOOL	SetParamInsert(AuStatement *pStatement);

	public:
		AgsdRelay2Log_Item()
			{
			m_eOperation = AGSMDATABASE_OPERATION_INSERT;
			ZeroMemory(m_szItemSeq, sizeof(m_szItemSeq));
			ZeroMemory(m_szExpireDate, sizeof(m_szExpireDate));
			}
		AgsdRelay2Log_Item(AgpdLog* pLog);
		virtual ~AgsdRelay2Log_Item()	{}

		virtual void	Release()	{ delete this; }

		virtual void	SetStaticQuery(CHAR *pszDest, CHAR *pszFmt);
		virtual	BOOL	WriteToFile();
	};


//
//	==========		Log ETC		==========
//
class AgsdRelay2Log_ETC : public AgsdRelay2Log, public ApMemory<AgsdRelay2Log_ETC, 20000>
	{
	public:
		AgpdLog_ETC			m_csLog;

	protected:
		BOOL	SetParamInsert(AuStatement *pStatement);

	public:
		AgsdRelay2Log_ETC()	{ m_eOperation = AGSMDATABASE_OPERATION_INSERT; }
		AgsdRelay2Log_ETC(AgpdLog* pLog);
		virtual ~AgsdRelay2Log_ETC()	{}

		virtual void	Release()	{ delete this; }

		virtual void	SetStaticQuery(CHAR *pszDest, CHAR *pszFmt);
		virtual	BOOL	WriteToFile();
	};


//
//	==========		Log Gheld		==========
//
class AgsdRelay2Log_Gheld : public AgsdRelay2Log, public ApMemory<AgsdRelay2Log_Gheld, 30000>
	{
	public:
		AgpdLog_Gheld		m_csLog;

	protected:
		BOOL	SetParamInsert(AuStatement *pStatement);

	public:
		AgsdRelay2Log_Gheld()	{ m_eOperation = AGSMDATABASE_OPERATION_INSERT; }
		AgsdRelay2Log_Gheld(AgpdLog* pLog);
		virtual ~AgsdRelay2Log_Gheld()	{}

		virtual void	Release()	{ delete this; }

		virtual void	SetStaticQuery(CHAR *pszDest, CHAR *pszFmt);
		virtual	BOOL	WriteToFile();
	};


//
//	==========		Log PCRoom		==========
//
class AgsdRelay2Log_PCRoom : public AgsdRelay2Log, public ApMemory<AgsdRelay2Log_PCRoom, 10000>
	{
	public:
		AgpdLog_PCRoom		m_csLog;

		CHAR				m_szLoginDate[23];
		UINT32				m_ulStayTime;

	protected:
		BOOL	SetParamInsert(AuStatement *pStatement);

	public:
		AgsdRelay2Log_PCRoom()
		{
			m_eOperation = AGSMDATABASE_OPERATION_INSERT;
			memset(m_szLoginDate, 0, sizeof(m_szLoginDate));
		}
		AgsdRelay2Log_PCRoom(AgpdLog* pLog);
		virtual ~AgsdRelay2Log_PCRoom()	{}

		virtual void	Release()	{ delete this; }

		virtual void	SetStaticQuery(CHAR *pszDest, CHAR *pszFmt);
		virtual	BOOL	WriteToFile();
	};


//
//	==========		Ping		==========
//	2005.04.07. steeple
class AgsdRelay2Ping : public AgsdDBParam, public ApMemory<AgsdRelay2Ping, 1000>
	{
	public:
		DWORD				m_dwPingSendTickCount;

	protected:
		//BOOL	SetParamSelect(AuStatement *pStatement);

	public:
		AgsdRelay2Ping()	{ m_eOperation = AGSMDATABASE_OPERATION_SELECT; m_dwPingSendTickCount = 0; }
		virtual ~AgsdRelay2Ping()	{}

		virtual void	Release()	{ delete this; }
	};


//
//	==========		Admin		==========
//	2005.05.02. steeple
class AgsdRelay2Admin : public AgsdDBParam, public ApMemory<AgsdRelay2Admin, 1000>
	{
	public:
		CHAR				m_szAdminCharName[_MAX_CHARNAME_LENGTH + 1];		// Operation 을 요청한 Admin

        INT16				m_nAdminOperation;									// Operation 종류
		CHAR				m_szTargetName[_MAX_CHARNAME_LENGTH + 1];			// AccountName 또는 CharName 이 된다.

		stAgpdAdminSearchResult				m_stSearchResult;
		stAgpdAdminCharDataBasic			m_stBasic;
		stAgpdAdminCharDataStatus			m_stStatus;
		stAgpdAdminCharDataStat				m_stStat;
		stAgpdAdminCharDataMoney			m_stMoney;
		stAgpdAdminItem						m_stItem;
		stAgpdAdminCharTitle				m_stTitle;

		CHAR				m_szDBID[_I64_STR_LENGTH + 1];
		CHAR				m_szExp[_I64_STR_LENGTH + 1];
		CHAR				m_szTreeNode[_MAX_TREENODE_LENGTH + 1];
		CHAR				m_szProduct[_MAX_TREENODE_LENGTH + 1];

	protected:
		BOOL				SetParamSelect(AuStatement *pStatement);
		BOOL				SetParamInsert(AuStatement *pStatement);
		BOOL				SetParamUpdate(AuStatement *pStatement);

	public:
		AgsdRelay2Admin();
		virtual ~AgsdRelay2Admin() {}

		virtual void	Release() { delete this; }
	};




/********************************************/
/*		The Definition of Parameters		*/
/********************************************/
//
class AgsdRelay2AutoPtr
	{
	private :
		AgsdDBParam		*m_pAgsdDBParam;
	
	public :
		AgsdRelay2AutoPtr(AgsdDBParam *pAgsdDBParam)
			{
			m_pAgsdDBParam = pAgsdDBParam;
			}
		~AgsdRelay2AutoPtr()
			{
			if (NULL != m_pAgsdDBParam)
				{
				m_pAgsdDBParam->Release();
				m_pAgsdDBParam = NULL;
				}
			}
	};

#include "AgspGuild.h"
class AgsdRelay2WorldChampionship :	public AgsdDBParam, public ApMemory<AgsdRelay2WorldChampionship, 1000>
{
public:
	CHAR	strCharName[AGPDCHARACTER_NAME_LENGTH+1];
	CHAR	strGuildName[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR	m_szWorld[AGSM_MAX_SERVER_NAME+1];
	INT16 nOperation;
	INT16 m_nCode;
	INT32 m_lServerID;

	AgsdRelay2WorldChampionship(UINT32 ulNID, INT16 Operation)
		: m_nCode(-1), m_lServerID(0)
	{
		memset(strCharName, 0, sizeof(strCharName));
		memset(strGuildName, 0, sizeof(strGuildName));
		memset(m_szWorld, 0, sizeof(m_szWorld));
		m_ulNID = ulNID;
		nOperation = Operation;
		m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;
	};

	virtual ~AgsdRelay2WorldChampionship()
	{

	};

	void Release()
	{ 
		delete this;
	};

	BOOL SetParamExecute(AuStatement* pStatement)
	{
		switch(nOperation)
		{
		case AGSMRELAY_PARAM_WORLD_CHAMPIONSHIP_REQUEST:
			{
				// in
				pStatement->SetParam( 0, strGuildName, sizeof(strGuildName));
				pStatement->SetParam( 1, strCharName, sizeof(strCharName));

				// out
				pStatement->SetParam( 2, &m_nCode, TRUE);
			} break;
		case AGSMRELAY_PARAM_WORLD_CHAMPIONSHIP_ENTER:
			{
				int i = 0;
				// in
				pStatement->SetParam( i++, strCharName, sizeof(strCharName));
				pStatement->SetParam( i++, &m_lServerID);
				pStatement->SetParam( i++, m_szWorld, sizeof(m_szWorld));

				// out
				pStatement->SetParam( i++, &m_nCode, TRUE);
			} break;
		}

		return TRUE;
	};
};

class AgsdRelay2Attendance : public AgsdDBParam, public ApMemory<AgsdRelay2Attendance, 10>
{
public:
	INT16 m_nCode;
	INT16 nOperation;
	
	CHAR strCharName[AGPDCHARACTER_NAME_LENGTH+1];
	CHAR strAccountName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR strWorldName[AGSM_MAX_SERVER_NAME+1];
	INT32 CID, NID, EID, STEP, Type;

	AgsdRelay2Attendance(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2Attendance();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2EventUser : public AgsdDBParam, public ApMemory<AgsdRelay2EventUser, 1000>
{
public:
	INT16 m_nCode;
	INT16 nOperation;

	CHAR strCharName[AGPDCHARACTER_NAME_LENGTH+1];
	CHAR strAccountName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR strWorldName[AGSM_MAX_SERVER_NAME+1];
	INT32 CID, NID, EID, STEP, Type;

	AgsdRelay2EventUser(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2EventUser();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};


class AgsdRelay2EventUserFlagUpdate : public AgsdDBParam, public ApMemory<AgsdRelay2EventUserFlagUpdate, 1000>
{
public:
	INT16 m_nCode;
	INT16 nOperation;

	CHAR strCharName[AGPDCHARACTER_NAME_LENGTH+1];
	CHAR strAccountName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR strWorldName[AGSM_MAX_SERVER_NAME+1];
	INT32 CID, NID, EID, STEP, Type, Flag;

	AgsdRelay2EventUserFlagUpdate(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2EventUserFlagUpdate();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

//////////////////////////////////////////////////////////////////////////
// 이름 변경 시스템 - arycoat 2009.4.
class AgsdRelay2ChangeName : public AgsdDBParam, public ApMemory<AgsdRelay2ChangeName, 10>
{
public:
	INT16 m_nCode;
	INT16 nOperation;

	CHAR strOldName[AGPDCHARACTER_NAME_LENGTH+1];
	CHAR strNewName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 CID, NID, EID, STEP;

	AgsdRelay2ChangeName(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2ChangeName();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

// 캐쉬 아이템 시간 연장 - 박경도 2009. 4.
class AgsdRelay2CashItemTimeExtend : public AgsdDBParam, public ApMemory<AgsdRelay2CashItemTimeExtend, 10>
{
public:
	INT16 m_nCode;
	INT16 nOperation;

	UINT32 ulExtendTime;
	UINT32 ulExtendTimeResult;

	AgsdRelay2CashItemTimeExtend(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2CashItemTimeExtend();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

//////////////////////////////////////////////////////////////////////////
// 캐릭터 봉인 시스템 - arycoat 2009.4.
class AgsdRelay2CerariumOrb : public AgsdDBParam, public ApMemory<AgsdRelay2CerariumOrb, 10>
{
public:
	INT16 m_nCode;
	INT16 nOperation;

	CHAR strWorldName[AGPDCHARACTER_NAME_LENGTH+1];
	CHAR strAccountName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR strCharName[AGPDCHARACTER_NAME_LENGTH+1];
	CHAR szDBID[_I64_STR_LENGTH+1];
	INT32 CID, NID, EID, STEP;
	INT32 Slot;
	INT32 ItemID;
	UINT64 ullDBIID;

	AgsdRelay2CerariumOrb(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2CerariumOrb();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2BankMoneyEdit : public AgsdDBParam, public ApMemory<AgsdRelay2BankMoneyEdit, 10>
{
public:
	INT16 nOperation;

	CHAR	strServerName[AGSM_MAX_SERVER_NAME+1];
	CHAR	strAccountName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR	strBankMoney[_I64_STR_LENGTH + 1];

	AgsdRelay2BankMoneyEdit(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2BankMoneyEdit();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2CharismaPointEdit : public AgsdDBParam, public ApMemory<AgsdRelay2CharismaPointEdit, 10>
{
public:
	INT16 nOperation;

	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32	nCharismaPoint;

	AgsdRelay2CharismaPointEdit(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2CharismaPointEdit();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2TitleAdd : public AgsdDBParam, public ApMemory<AgsdRelay2TitleAdd, 10>
{
public:
	INT16 nOperation;

	BOOL	bCompleteQuest;
	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR	strDate[_MAX_DATETIME_LENGTH + 1];
	INT32	nTitleTid;
	BOOL	bUse;

	AgsdRelay2TitleAdd(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2TitleAdd();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2TitleUse : public AgsdDBParam, public ApMemory<AgsdRelay2TitleUse, 10>
{
public:
	INT16 nOperation;

	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR	strDate[_MAX_DATETIME_LENGTH + 1];
	INT32	nTitleTid;
	INT32	nNextTitleTid;
	BOOL	bUse;

	AgsdRelay2TitleUse(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2TitleUse();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2TitleList : public AgsdDBParam, public ApMemory<AgsdRelay2TitleList, 10>
{
public:
	INT16 nOperation;

	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	BOOL	bIsTitle;

	AgsdRelay2TitleList(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2TitleList();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2TitleQuestRequest : public AgsdDBParam, public ApMemory<AgsdRelay2TitleQuestRequest, 10>
{
public:
	INT16 nOperation;

	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32	nTitleTid;
	CHAR	strDate[_MAX_DATETIME_LENGTH + 1];

	AgsdRelay2TitleQuestRequest(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2TitleQuestRequest();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2TitleQuestCheck : public AgsdDBParam, public ApMemory<AgsdRelay2TitleQuestCheck, 10>
{
public:
	INT16 nOperation;

	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32	nTitleTid;
	INT32	nTitleCheckValue[AGPDTITLE_MAX_TITLE_CHECK];

	AgsdRelay2TitleQuestCheck(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2TitleQuestCheck();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2TitleQuestComplete : public AgsdDBParam, public ApMemory<AgsdRelay2TitleQuestComplete, 10>
{
public:
	INT16 nOperation;

	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR	strDate[_MAX_DATETIME_LENGTH + 1];
	INT32	nTitleTid;
	BOOL	bComplete;

	AgsdRelay2TitleQuestComplete(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2TitleQuestComplete();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2TitleQuestList : public AgsdDBParam, public ApMemory<AgsdRelay2TitleQuestList, 10>
{
public:
	INT16 nOperation;

	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];

	AgsdRelay2TitleQuestList(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2TitleQuestList();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2TitleDelete : public AgsdDBParam, public ApMemory<AgsdRelay2TitleDelete, 10>
{
public:
	INT16 nOperation;

	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32	nTitleTid;

	AgsdRelay2TitleDelete(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2TitleDelete();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

class AgsdRelay2CharacterCreationDate : public AgsdDBParam, public ApMemory<AgsdRelay2CharacterCreationDate, 10>
{
public:
	INT16 nOperation;
	INT32 CID, NID, EID, STEP;

	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR	strStartDate[_MAX_DATETIME_LENGTH+1];
	CHAR	strEndDate[_MAX_DATETIME_LENGTH+1];

	AgsdRelay2CharacterCreationDate(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2CharacterCreationDate();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};

//JK_특성화서버
//////////////////////////////////////////////////////////////////////////
// 특성화 서버 이동 시스템 - 
class AgsdRelay2ServerMove : public AgsdDBParam, public ApMemory<AgsdRelay2ServerMove, 10>
{
public:
	INT16 m_nCode;
	INT16 nOperation;

	CHAR strCharName[AGPDCHARACTER_NAME_LENGTH+1];
	CHAR strServerName[AGSM_MAX_SERVER_NAME+1];
	
	INT32 CID, NID, EID, STEP;

	AgsdRelay2ServerMove(UINT32 ulNID, INT16 Operation);
	virtual ~AgsdRelay2ServerMove();

	void Release();
	BOOL SetParamExecute(AuStatement* pStatement);
};
#endif	// _AGSD_RELAY2_H_