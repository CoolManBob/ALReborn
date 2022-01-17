/*==============================================================================

	AgpmLog.h
		(C) NHN - ArchLord Development Team
		steeple, 2004. 04. 26.

==============================================================================*/


#ifndef _AGPMLOG_H_
	#define _AGPMLOG_H_


#include "ApModule.h"
#include "AgpdLog.h"
#include "ApAdmin.h"
#include "AuExcelTxtLib.h"
#include <unordered_map>

using namespace std;
//using namespace stdext;


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGPMLOG_CB
	{
	AGPMLOG_CB_WRITE = 0,
	};


typedef pair<INT32, INT32>					ItemFilterPair;
typedef unordered_map<INT32, LONG>::iterator		ItemFilterIter;
typedef unordered_map<INT32, LONG>				ItemFilterMap;


const enum eAGPMLOG_EXCEL_COLUMN
	{
	AGPMLOG_EXCEL_COLUMN_TYPE		= 0,
	AGPMLOG_EXCEL_COLUMN_ACTION,
	AGPMLOG_EXCEL_COLUMN_ITEM_TID
	};


#define AGPMLOG_CHATTING_FILE_NAME			"Chatting.log"



/********************************************/
/*		The Definition of AgpmLog class		*/
/********************************************/
//
class AgpmLog : public ApModule
	{
	private:
		// log filter
		ApSafeArray<BOOL, AGPDLOGTYPE_PLAY_MAX>				m_PlayFilter;
		ApSafeArray<BOOL, AGPDLOGTYPE_ETC_MAX>				m_EtcFilter;
		ApSafeArray<BOOL, AGPDLOGTYPE_GHELD_MAX>			m_GheldFilter;
		ApSafeArray<ItemFilterMap *, AGPDLOGTYPE_ITEM_MAX>	m_ItemFilter;

	public:
		volatile	BOOL									m_bWriteChattingLog;

		AgpmLog();
		virtual ~AgpmLog();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnDestroy();
		
		//	Stream
		BOOL	StreamReadFilter(CHAR *pszFile, BOOL bDecryption);

		//	Base		
		inline BOOL	WriteLog(PVOID pvLog, INT16 nSize = sizeof(AgpdLog));
		BOOL	WriteLog_Play(eAGPDLOGTYPE_PLAY eType, INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							INT32 lCharLevelFrom, INT32 lPartyTime, INT32 lSoloTime, INT32 lKillPC, INT32 lKillMon, INT32 lDeadByPC, INT32 lDeadByMon);
		BOOL	WriteLog_Item(BOOL bIsCashItem, eAGPDLOGTYPE_ITEM eType, INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							UINT64 ullItemDBID, INT32 lItemTID, INT32 lItemQty, CHAR *pszConvert, CHAR *pszOption,
							INT32 lGheld, CHAR *pszCharID2, INT32 lInUse, INT32 lUseCount, INT64 lRemainTime, INT32 lExpireDate, INT64 llStaminaRemainTime,
							INT32 lDurability, INT32 lMaxDurability,
							CHAR* pszPosition);
		BOOL	WriteLog_ETC(eAGPDLOGTYPE_ETC eType, INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							INT32 lNumID, CHAR *pszStrID, CHAR *pszDesc, INT32 lGheld, CHAR *pszCharID2 = NULL);
		BOOL	WriteLog_Gheld(eAGPDLOGTYPE_GHELD eType, INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							INT32 lGheld, CHAR *pszCharID2);

		// Play
		BOOL	WriteLog_Login(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank);
		BOOL	WriteLog_Logout(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lPartyTime, INT32 lSoloTime, INT32 lKillPC, INT32 lKillMon, INT32 lDeadByPC, INT32 lDeadByMon);
		BOOL	WriteLog_LevelUp(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lCharLevelFrom, INT32 lPartyTime, INT32 lSoloTime, INT32 lKillPC, INT32 lKillMon, INT32 lDeadByPC, INT32 lDeadByMon);
		BOOL	WriteLog_Tick(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lPartyTime, INT32 lSoloTime, INT32 lKillPC, INT32 lKillMon, INT32 lDeadByPC, INT32 lDeadByMon);
		
		// ETC
		BOOL	WriteLog_SkillBuy(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc, INT32 lGheld);
		BOOL	WriteLog_SkillUp(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc, INT32 lGheld);
		BOOL	WriteLog_SkillInit(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR *pszDesc, INT32 lGheld);
		BOOL	WriteLog_PDSkillBuy(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lPDSkillTID, CHAR *pszDesc, INT32 lGheld);
		BOOL	WriteLog_QuestAccept(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lQuestTID, CHAR *pszDesc);	
		BOOL	WriteLog_QuestCancel(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lQuestTID, CHAR *pszDesc);	
		BOOL	WriteLog_QuestComplete(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lQuestTID, CHAR *pszDesc);
		BOOL	WriteLog_GuildCreate(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszGuildID, CHAR *pszDesc, INT32 lGheld);
		BOOL	WriteLog_GuildRemove(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszGuildID, CHAR *pszDesc);
		BOOL	WriteLog_GuildIn(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszGuildID, CHAR *pszDesc);
		BOOL	WriteLog_GuildOut(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszGuildID, CHAR *pszDesc);
		BOOL	WriteLog_GuildBattleRequest(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszGuildID, CHAR *pszEnemyGuildID);
		BOOL	WriteLog_GuildBattleAccept(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszGuildID, CHAR *pszEnemyGuildID);
		BOOL	WriteLog_CharCustomize(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR *pszDesc, INT32 lGheld);
		BOOL	WriteLog_BankExpansion(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									INT32 lBankSize, INT32 lGheld);
		BOOL	WriteLog_CharDeath(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR *pszEnemy, CHAR *pszDesc, CHAR *pszGuild);
		BOOL	WriteLog_CharKill(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR *pszEnemy, CHAR *pszDesc, CHAR *pszGuild);
		BOOL	WriteLog_GuildExpansion(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszGuildID, INT32 lGheld, INT32 lMaxMember);
		BOOL	WriteLog_GuildRename(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszGuildID, CHAR *pszOldName);
		BOOL	WriteLog_Teleport(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszFrom, CHAR *pszTo, INT32 lGheld);
		BOOL	WriteLog_BattleGroundTeleport(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszFrom, CHAR *pszTo, INT32 lGheld);
		BOOL	WriteLog_GuildBuyMark(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									CHAR* pszGuildID, CHAR *pszDesc, INT32 lGheld);
		BOOL	WriteLog_Remission(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									INT32 lPoint, CHAR *pszDesc);
		BOOL	WriteLog_MurdererUp(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									INT32 lPoint, INT32 lTotalPoint, CHAR* pszID);
		BOOL	WriteLog_CharismaUp(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									INT32 lPoint, INT32 lTotalPoint, CHAR* pszID);

		BOOL	WriteLog_Chatting(INT32 lTimeStamp, CHAR* szSender, CHAR* szMessage, INT32 lMessageLength);

		BOOL	WriteLog_NoChat(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc, INT32 lGheld, CHAR* pszExcutor);

		BOOL	WriteLog_TitleQuestAdd(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleQuestComplete(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleAdd(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleUse(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleDelete(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleAddByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleDeleteByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleEditByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleQuestAddByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleQuestEditByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleQuestDeleteByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_TitleQuestCompleteByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc);

		BOOL	WriteLog_CharismaUpByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
			INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
			INT32 lSkillTID, CHAR *pszDesc);


		// PC Room
		BOOL	WriteLog_PCRoom(UINT32 ulTimeStamp, const CHAR* szGameID, CHAR* szServerID, CHAR* szAccountID, CHAR* szWorldID,
								CHAR* szCharID, UINT32 ulLoginTimeStamp, CHAR* szIP, INT32 lLoginLV, INT32 lLogoutLV,
								INT64 llGheldInven, INT64 llGheldBank, CHAR* szCRMCode, CHAR* szGrade);

		//	Filter check
		BOOL	CheckItemFilter(INT32 lItemTID, eAGPDLOGTYPE_ITEM eFlag);
		BOOL	CheckPlayFilter(eAGPDLOGTYPE_PLAY eAction);
		BOOL	CheckEtcFilter(eAGPDLOGTYPE_ETC eAction);
		BOOL	CheckGheldFilter(eAGPDLOGTYPE_GHELD eAction);

		//	Callback setting
		BOOL	SetCallbackWriteLog(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	};


#endif // _AGPMLOG_H_