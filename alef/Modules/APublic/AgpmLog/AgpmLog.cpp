/*==============================================================================

	AgpmLog.cpp
		(C) NHN - ArchLord Development Team
		steeple, 2004. 04. 26.

==============================================================================*/


#include "AgpmLog.h"
#include <time.h>
#include "AuTimeStamp.h"


/************************************************/
/*		The Implementation of AgpdLog class		*/
/************************************************/
//
//	==========	Base	==========
//
static const TCHAR s_szLog[AGPDLOG_CATEGORY_MAX][10] =
	{
	"PLAY",
	"ITEM",
	"ETC",
	"GHELD",
	"CASH"
	};


AgpdLog::AgpdLog()
	{
	Init();
	}


AgpdLog::~AgpdLog()
	{
	}


inline void AgpdLog::Init()
	{
	SetType(-1);
	m_lTimeStamp = 0;
	m_lServerID = 0;
	ZeroMemory(m_szIP, sizeof(m_szIP));
	ZeroMemory(m_szAccountID, sizeof(m_szAccountID));
	ZeroMemory(m_szWorld, sizeof(m_szWorld));
	ZeroMemory(m_szCharID, sizeof(m_szCharID));
	m_lCharTID = 0;
	m_lCharLevel = 0;
	m_llCharExp = 0;
	m_llGheldInven = 0;
	m_llGheldBank = 0;
	SetTimeStamp();
	}


inline void AgpdLog::SetType(INT8 cType)
	{
	m_cLogType = cType;
	}


inline INT32 AgpdLog::SetTimeStamp(INT32 lTimeStamp)
	{
	if (0 != lTimeStamp)
		m_lTimeStamp = lTimeStamp;
	else
		{
		// get time
		time_t timeval;
		(void)time(&timeval);
		
		m_lTimeStamp = (INT32)timeval;
		}

	return m_lTimeStamp;
	}


inline void AgpdLog::SetBasicData(AgpdLog *pAgpdLog)
	{
	if (!pAgpdLog)
		return;

	SetType(pAgpdLog->m_cLogType);
	m_lTimeStamp = pAgpdLog->m_lTimeStamp;
	m_lServerID = pAgpdLog->m_lServerID;
	strncpy(m_szIP, pAgpdLog->m_szIP, AGPDLOG_MAX_IPADDR_STRING);
	strncpy(m_szAccountID, pAgpdLog->m_szAccountID, AGPDLOG_MAX_ACCOUNTID);
	strncpy(m_szWorld, pAgpdLog->m_szWorld, AGPDLOG_MAX_WORLD_NAME);
	strncpy(m_szCharID, pAgpdLog->m_szCharID, AGPDLOG_MAX_STR_ID);
	m_lCharTID = pAgpdLog->m_lCharTID;
	m_lCharLevel = pAgpdLog->m_lCharLevel;
	m_llCharExp = pAgpdLog->m_llCharExp;
	m_llGheldInven = pAgpdLog->m_llGheldInven;
	m_llGheldBank = pAgpdLog->m_llGheldBank;
	}


inline void AgpdLog::SetBasicData(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld,
							CHAR *pszCharID, INT32 lCharTID, INT32 lCharLevel,
							INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank)
	{
	SetTimeStamp(lTimeStamp);
	strncpy(m_szIP, pszIP ? pszIP : "", AGPDLOG_MAX_IPADDR_STRING);
	strncpy(m_szAccountID, pszAccountID ? pszAccountID : "", AGPDLOG_MAX_ACCOUNTID);
	strncpy(m_szWorld, pszWorld ? pszWorld : "", AGPDLOG_MAX_WORLD_NAME);
	strncpy(m_szCharID, pszCharID ? pszCharID : "", AGPDLOG_MAX_STR_ID);
	m_lCharTID = lCharTID;
	m_lCharLevel = lCharLevel;
	m_llCharExp = llCharExp;
	m_llGheldInven = llGheldInven;
	m_llGheldBank = llGheldBank;
	}


//
//	==========	Play	==========
//
CHAR AgpdLog_Play::m_szAct[AGPDLOGTYPE_PLAY_MAX][AGPDLOG_MAX_ACTCODE+1] =
	{
	"CHGIN",			// AGPDLOGTYPE_PLAY_GAMEIN
	"CHGOUT",			// AGPDLOGTYPE_PLAY_GAMEOUT
	"CHTICK",			// AGPDLOGTYPE_PLAY_TICK,
	"CHLVUP",			// AGPDLOGTYPE_PLAY_LEVELUP,
	"GM-CHLV",			// AGPDLOGTYPE_PLAY_GMEDITLEVEL,
	"GM-CHEXP",			// AGPDLOGTYPE_PLAY_GMEDITEXP,
	"GM-EDITDB"			// AGPDLOGTYPE_PLAY_GMEDITDB
	};


AgpdLog_Play::AgpdLog_Play()
	{
	Init();
	}

AgpdLog_Play::~AgpdLog_Play()
	{
	}

inline void AgpdLog_Play::Init()
	{
	AgpdLog::Init();

	SetType(AGPDLOG_CATEGORY_PLAY);
	m_cFlag = -1;
	m_lCharLevelFrom = 0;
	m_lPartyTime = 0;
	m_lSoloTime = 0;
	m_lKillPC = 0;
	m_lKillMonster = 0;
	m_lDeadByPC = 0;
	m_lDeadByMonster = 0;
	}


//
//	==========	Item	==========
//
CHAR AgpdLog_Item::m_szAct[AGPDLOGTYPE_ITEM_MAX][AGPDLOG_MAX_ACTCODE+1] = 
	{
	"ITNPCBUY",			// AGPDLOGTYPE_ITEM_NPC_BUY
	"ITNPCSELL",		// AGPDLOGTYPE_ITEM_NPC_SELL,
	"ITPTGIVE",			// AGPDLOGTYPE_ITEM_TRADE_GIVE,		// 개인 거래 준거
	"ITPTTAKE",			// AGPDLOGTYPE_ITEM_TRADE_TAKE,		// 개인 거래 받은거
	"ITSBSELL",			// AGPDLOGTYPE_ITEM_BOARD_SELL,		// 거래 게시판 등록
	"ITSBBUY",			// AGPDLOGTYPE_ITEM_BOARD_BUY,			// 거래 게시판 구매
	"ITSBSOLD",			// AGPDLOGTYPE_ITEM_BOARD_SOLD,		// 거래 게시판 구매 더미
	"ITSBCAN",			// AGPDLOGTYPE_ITEM_BOARD_CANCEL,		// 거래 게시판 취소
	"ITSBCON",			// AGPDLOGTYPE_ITEM_BOARD_CONFIRM,		// 거래 게시판 확인
	"ITPICK",			// AGPDLOGTYPE_ITEM_PICK,
	"ITDEST",			// AGPDLOGTYPE_ITEM_DEST,
	"ITUSE",			// AGPDLOGTYPE_ITEM_USE,
	"ITREPAIR",			// AGPDLOGTYPE_ITEM_REPAIR,			// 수리
	"ITSM1_SC",			// AGPDLOGTYPE_ITEM_SM1_SC,			// 강화 성공
	"ITSM1_NC",			// AGPDLOGTYPE_ITEM_SM1_NC,			// 강화 실패(변화 없음)
	"ITSM1_IN",			// AGPDLOGTYPE_ITEM_SM1_IN,			// 강화 초기화
	"ITSM1_DT",			// AGPDLOGTYPE_ITEM_SM1_DT,			// 강화 아이템 파괴
	"ITSM2_SC",			// AGPDLOGTYPE_ITEM_SM2_SC,			// 정령석 성공
	"ITSM2_FL",			// AGPDLOGTYPE_ITEM_SM2_FL,			// 정령석 실패
	"ITSM3_SC",			// AGPDLOGTYPE_ITEM_SM3_SC,			// 기원석 성공
	"ITSM3_NC",			// AGPDLOGTYPE_ITEM_SM3_NC,			// 기원석 실패(변화 없음)
	"ITSM3_IN",			// AGPDLOGTYPE_ITEM_SM3_IN,			// 기원석 소켓 초기화
	"ITSM3_II",			// AGPDLOGTYPE_ITEM_SM3_II,			// 기원석 아이템 초기화
	"ITSM3_DT",			// AGPDLOGTYPE_ITEM_SM3_DT,			// 기원석 아이템 파괴
	"ITQSTRE",			// AGPDLOGTYPE_ITEM_QUEST_REWARD,		// 퀘스트 보상(받았다)
	"ITQSTPAY",			// AGPDLOGTYPE_ITEM_QUEST_PAY,		
	"ITPDRE",			// AGPDLOGTYPE_ITEM_PDSKILL_REWARD,	// 생산스킬 보상
	"ITPDPAY",			// AGPDLOGTYPE_ITEM_PDSKILL_SOURCE,	// 생산 재료
	"ITREFRE",			// AGPDLOGTYPE_ITEM_REFINE_REWARD,		// 정제 결과
	"ITREFPAY",			// AGPDLOGTYPE_ITEM_REFINE_SOURCE,		// 정제 재료
	"ITBANKIN",			// AGPDLOGTYPE_ITEM_BANK_IN,			// 뱅크에 넣었다.
	"ITBANKOUT",		// AGPDLOGTYPE_ITEM_BANK_OUT,			// 뱅크에서 뺐다.
	"ITBANKLO",			// AGPDLOGTYPE_ITEM_BANK_LOGOUT
	"ITPTDIST",			// AGPDLOGTYPE_ITEM_PARTY_DISTRIBUTE
	"ITEVGIVE",			// AGPDLOGTYPE_ITEM_EVENT_GIVE,		// 이벤트 아이템 증정
	"GM-ITMAKE",		// AGPDLOGTYPE_ITEM_GM_MAKE,			// GM이 생성.
	"GM-ITDEST",		// AGPDLOGTYPE_ITEM_GM_DESTROY,		// GM이 삭제
	"GM-ITSM1",			// AGPDLOGTYPE_ITEM_GM_ITSM1,			// GM 축성제강화
	"GM-ITSM2",			// AGPDLOGTYPE_ITEM_GM_ITSM2,			// GM 정령석강화
	"GM-ITSM3",			// AGPDLOGTYPE_ITEM_GM_ITSM3,			// GM 기원석강화
	"GM-ITSM4",			// AGPDLOGTYPE_ITEM_GM_ITSM4,			// GM 고리추가
	"GM-ITSM5",			// AGPDLOGTYPE_ITEM_GM_ITSM5,			// GM 옵션추가
	"ITMAILAT",			// AGPDLOGTYPE_ITEM_MAIL_ATTACH,		// 메일에 첨부
	"ITMAILRD",			// AGPDLOGTYPE_ITEM_MAIL_READ,			// 메일에 첨부된거 확인
	"ITSTART",			// AGPDLOGTYPE_ITEM_USE_START
	"ITDROP",			// AGPDLOGTYPE_ITEM_DROP
	"ITGD_IN",			// AGPDLOGTYPE_ITEM_GWARE_IN,
	"ITGD_OUT",			// AGPDLOGTYPE_ITEM_GWARE_OUT,
	"ITSOCKIN",			// AGPDLOGTYPE_ITEM_SOCKET_INIT,				// 소켓 초기화
	"NPCGIVE",			// AGPDLOGTYPE_ITEM_NPC_GIVE,		// NPC Manager에게 받았다
	"NPCROB",			// AGPDLOGTYPE_ITEM_NPC_ROB,			// NPC Manager에게 주었다
	"ITSTOP",			// AGPDLOGTYPE_ITEM_USE_STOP
	"ITEND",			// AGPDLOGTYPE_ITEM_USE_END
	};


AgpdLog_Item::AgpdLog_Item()
	{
	Init();
	}


AgpdLog_Item::~AgpdLog_Item()
	{
	}


inline void AgpdLog_Item::Init()
	{
	AgpdLog::Init();
	
	SetType(AGPDLOG_CATEGORY_ITEM);
	m_cFlag = (-1);
	m_ullItemDBID = 0;
	m_lItemTID = 0;
	m_lItemQty = 0;
	ZeroMemory(m_szConvert, sizeof(m_szConvert));
	ZeroMemory(m_szOption, sizeof(m_szOption));
	m_lGheld = 0;
	ZeroMemory(m_szCharID2, sizeof(m_szCharID2));
	m_lInUse = 0;
	m_lUseCount = 0;
	m_lRemainTime = 0;
	m_lExpireDate = 0;
	m_llStaminaRemainTime = 0;
	ZeroMemory(m_szDescription, sizeof(m_szDescription));
	}


//
//	==========	ETC	==========
//
CHAR AgpdLog_ETC::m_szAct[AGPDLOGTYPE_ETC_MAX][AGPDLOG_MAX_ACTCODE+1] =
	{
	"SKBUY",		// AGPDLOGTYPE_ETC_SKILL_BUY
	"PDBUY",		// AGPDLOGTYPE_ETC_PDSKILL_BUY,
	"SKUP",			// AGPDLOGTYPE_ETC_SKILL_UPGRADE,
	"QSTACP",		// AGPDLOGTYPE_ETC_QUEST_ACCEPT,
	"QSTCAN",		// AGPDLOGTYPE_ETC_QUEST_CANCEL,
	"QSTCOM",		// AGPDLOGTYPE_ETC_QUEST_COMPLETE,
	"GLDCR",		// AGPDLOGTYPE_ETC_GUILD_CREATE,
	"GLDRM",		// AGPDLOGTYPE_ETC_GUILD_REMOVE,
	"GLDEN",		// AGPDLOGTYPE_ETC_GUILD_ENTRANCE,
	"GLDOUT",		// AGPDLOGTYPE_ETC_GUILD_OUT,
	"GLDBREQ",		// AGPDLOGTYPE_ETC_GUILD_BATTLE_REQ,
	"GLDBACP",		// AGPDLOGTYPE_ETC_GUILD_BATTLE_ACP,
	"SKINIT",		// AGPDLOGTYPE_ETC_SKILL_INIT,
	"CHCUST",		// AGPDLOGTYPE_ETC_CHAR_CUSTOMIZE,
	"BANKEX",		// AGPDLOGTYPE_ETC_BANK_EXPANSION,
	"CHDEATH",		// AGPDLOGTYPE_ETC_DEATH
	"GLDEX",		// AGPDLOGTYPE_ETC_GUILD_EXPANSION,
	"GLDREN",		// AGPDLOGTYPE_ETC_GUILD_RENAME,
	"TELEPT",		// AGPDLOGTYPE_ETC_TELEPORT
	"GLDBMK",		// AGPDLOGTYPE_ETC_GUILD_BUY_MARK
	"REMIT",		// AGPDLOGTYPE_ETC_REMISSION
	"MURDUP",		// AGPDLOGTYPE_ETC_MURDERER_UP
	"WCKILL",		// AGPDLOGTYPE_ETC_WC_KILL,
	"WCDEAD",		// AGPDLOGTYPE_ETC_WC_DEAD,
	"WCARNG",		// AGPDLOGTYPE_ETC_WC_ARRANGE
	"CHKILL",		// AGPDLOGTYPE_ETC_KILL
	"TAXTRAN",		// AGPDLOGTYPE_ETC_TAX_TRANSFER
	"TAXUPD",		// AGPDLOGTYPE_ETC_TAX_UPDATE
	"CHARISUP",		// AGPDLOGTYPE_ETC_CHARISMA_UP
	"TAXBK",		// AGPDLOGTYPE_ETC_TAX_BACKUP
	"RENAME",		// AGPDLOGTYPE_ETC_RENAME
	"SEAL",			// AGPDLOGTYPE_ETC_SEAL
	"UNSEAL",		// AGPDLOGTYPE_ETC_RELEASESEAL
	"CHNOCHAT",		// AGPDLOGTYPE_ETC_NOCHAT		
	"TLTQADD",		// AGPDLOGTYPE_ETC_TITLEQUEST_ADD
	"TLTQCPT",		// AGPDLOGTYPE_ETC_TITLEQUEST_COMPLETE
	"TLTADD",		// AGPDLOGTYPE_ETC_TITLE_ADD
	"TLTUSE",		// AGPDLOGTYPE_ETC_TITLE_USE
	"TLTDEL",		// AGPDLOGTYPE_ETC_TITLE_DELETE
	"GMTLTADD",		// AGPDLOGTYPE_ETC_TITLE_ADD_BY_GM
	"GMTLTDEL",		// AGPDLOGTYPE_ETC_TITLE_DELETE_BY_GM
	"GMTLTEDIT",	// AGPDLOGTYPE_ETC_TITLE_EDIT_BY_GM
	"GMTLTQADD",	// AGPDLOGTYPE_ETC_TITLEQUEST_ADD_BY_GM
	"GMTLTQEDIT",	// AGPDLOGTYPE_ETC_TITLEQUEST_EDIT_BY_GM
	"GMTLTQDEL",	// AGPDLOGTYPE_ETC_TITLEQUEST_DELETE_BY_GM
	"GMTLTQCPT",	// AGPDLOGTYPE_ETC_TITLEQUEST_COMPLETE_BY_GM
	"GMCHARISUP",	// AGPDLOGTYPE_ETC_CHARISMA_UP_BY_GM
	"MAILWRITE",	// AGPDLOGTYPE_ETC_MAIL_WRITE
	"MAILDEL",		// AGPDLOGTYPE_ETC_MAIL_DELETE
	"BGTELEPT",		// AGPDLOGTYPE_ETC_BATTLEGROUND_TELEPORT
	};


AgpdLog_ETC::AgpdLog_ETC()
	{
	Init();
	}


AgpdLog_ETC::~AgpdLog_ETC()
	{
	}


inline void AgpdLog_ETC::Init()
	{
	AgpdLog::Init();
	
	SetType(AGPDLOG_CATEGORY_ETC);
	m_cFlag = -1;
	m_lNumID = 0;
	ZeroMemory(m_szStrID, sizeof(m_szStrID));
	ZeroMemory(m_szDescription, sizeof(m_szDescription));
	m_lGheld = 0;
	ZeroMemory(m_szCharID2, sizeof(m_szCharID2));
	}


//
//	==========	Gheld	==========
//
CHAR AgpdLog_Gheld::m_szAct[AGPDLOGTYPE_GHELD_MAX][AGPDLOG_MAX_ACTCODE+1] =
	{
	"GHPICK",		// AGPDLOGTYPE_GHELD_PICK,
	"GHBANKIN",		// AGPDLOGTYPE_GHELD_BANK_IN,
	"GHBANKOUT",	// AGPDLOGTYPE_GHELD_BANK_OUT,
	"GHPTGIVE",		// AGPDLOGTYPE_GHELD_TRADE_GIVE,
	"GHPTTAKE",		// AGPDLOGTYPE_GHELD_TRADE_TAKE,
	"GHREFPAY",		// AGPDLOGTYPE_GHELD_REFINE_PAY,
	"GHQSTRE",		// AGPDLOGTYPE_GHELD_QUEST_REWARD
	"GHEVGIVE",		// AGPDLOGTYPE_GHELD_EVENT_GIVE
	"GM-GHEDIT",	// AGPDLOGTYPE_GHELD_GMEDIT
	"GHGD_IN",		// AGPDLOGTYPE_GHELD_GWARE_IN,
	"GHGD_OUT"		// AGPDLOGTYPE_GHELD_GWARE_OUT,
	};

AgpdLog_Gheld::AgpdLog_Gheld()
	{
	Init();
	}


AgpdLog_Gheld::~AgpdLog_Gheld()
	{
	}


inline void AgpdLog_Gheld::Init()
	{
	AgpdLog::Init();
	
	SetType(AGPDLOG_CATEGORY_GHELD);
	m_cFlag = -1;
	m_lGheldChange = 0;
	ZeroMemory(m_szCharID2, sizeof(m_szCharID2));
	}


AgpdLog_PCRoom::AgpdLog_PCRoom()
{
	Init();
}

AgpdLog_PCRoom::~AgpdLog_PCRoom()
{
}

inline void AgpdLog_PCRoom::Init()
{
	AgpdLog::Init();

	SetType(AGPDLOG_CATEGORY_PCROOM);

	ZeroMemory(m_szGameID, sizeof(m_szGameID));
	m_ulLoginTimeStamp = 0;
	m_ulLogoutTimeStamp = 0;
	m_ulPlayTime = 0;
	m_lLoginLevel = 0;
	ZeroMemory(m_szCRMCode, sizeof(m_szCRMCode));
	ZeroMemory(m_szGrade, sizeof(m_szGrade));
}




/************************************************/
/*		The Implementation of AgpmLog class		*/
/************************************************/
//
AgpmLog::AgpmLog()
	{
	SetModuleName("AgpmLog");

	EnableIdle(FALSE);
	
	SetModuleType(APMODULE_TYPE_PUBLIC);

	if(g_eServiceArea != AP_SERVICE_AREA_CHINA)
		m_bWriteChattingLog = FALSE;
	else
		m_bWriteChattingLog = TRUE;
	}


AgpmLog::~AgpmLog()
	{
	}




//	ApModule Inherited
//===========================================
//
BOOL AgpmLog::OnAddModule()
	{
	m_PlayFilter.MemSetAll();
	m_EtcFilter.MemSetAll();
	m_GheldFilter.MemSetAll();
	m_ItemFilter.MemSetAll();	
	
	return TRUE;
	}


BOOL AgpmLog::OnDestroy()
	{
	for (INT32 i=0; i<AGPDLOGTYPE_ITEM_MAX; ++i)
		{
		if (NULL != m_ItemFilter[i])
			{
			delete m_ItemFilter[i];
			m_ItemFilter[i] = NULL;
			}
		}
	
	return TRUE;
	}




//	Stream
//======================================
//
BOOL AgpmLog::StreamReadFilter(CHAR *pszFile, BOOL bDecryption)
	{
	if (!pszFile || !strlen(pszFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
		return FALSE;

	eAGPDLOG_CATEGORY eLog = AGPDLOG_CATEGORY_MAX;
	CHAR *pszPrevAction = NULL;
	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)
		{
		CHAR *psz = NULL;
		
		// log type
		psz = csExcelTxtLib.GetData(AGPMLOG_EXCEL_COLUMN_TYPE, lRow);
		for (INT32 k =0; k<AGPDLOG_CATEGORY_MAX; ++k)
			{
			if (psz && 0 == stricmp(psz, s_szLog[k]))
				{
				eLog = (eAGPDLOG_CATEGORY) k;
				break;
				}
			}
		
		if (AGPDLOG_CATEGORY_MAX == eLog)
			return FALSE;

		// action code
		psz = csExcelTxtLib.GetData(AGPMLOG_EXCEL_COLUMN_ACTION, lRow);
		if (NULL == psz || 0 >= strlen(psz))
			psz = pszPrevAction;
		else
			pszPrevAction = psz;

		switch (eLog)
			{
			case AGPDLOG_CATEGORY_PLAY :
				{
				eAGPDLOGTYPE_PLAY eAction = AGPDLOGTYPE_PLAY_MAX;
				for (INT32 i=0; i<AGPDLOGTYPE_PLAY_MAX; ++i)
					{
					if (psz && 0 == stricmp(psz, AgpdLog_Play::m_szAct[i]))
						{
						eAction = (eAGPDLOGTYPE_PLAY) i;
						break;
						}
					}
				
				if (AGPDLOGTYPE_PLAY_MAX == eAction)
					continue;
				
				m_PlayFilter[eAction] = TRUE;
				}
				break;
			
			case AGPDLOG_CATEGORY_ITEM :
			case AGPDLOG_CATEGORY_CASHITEM :
				{
				eAGPDLOGTYPE_ITEM eAction = AGPDLOGTYPE_ITEM_MAX;
				INT32 lItemTID = 0;
				
				for (INT32 i=0; i<AGPDLOGTYPE_ITEM_MAX; ++i)
					{
					if (psz && 0 == stricmp(psz, AgpdLog_Item::m_szAct[i]))
						{
						eAction = (eAGPDLOGTYPE_ITEM) i;
						break;
						}
					}
				
				if (AGPDLOGTYPE_ITEM_MAX == eAction)
					continue;
				
				// item tid
				psz = csExcelTxtLib.GetData(AGPMLOG_EXCEL_COLUMN_ITEM_TID, lRow);
				if (NULL == psz || '\0' == *psz)
					continue;
				lItemTID = atoi(psz);
				if (0 == lItemTID)
					continue;

				if (NULL == m_ItemFilter[eAction])
					m_ItemFilter[eAction] = new ItemFilterMap;

				m_ItemFilter[eAction]->insert(ItemFilterPair(lItemTID, 0));
				}
				break;
			
			case AGPDLOG_CATEGORY_ETC :
				{
				eAGPDLOGTYPE_ETC eAction = AGPDLOGTYPE_ETC_MAX;
				for (INT32 i=0; i<AGPDLOGTYPE_ETC_MAX; ++i)
					{
					if (psz && 0 == stricmp(psz, AgpdLog_ETC::m_szAct[i]))
						{
						eAction = (eAGPDLOGTYPE_ETC) i;
						break;
						}
					}
				
				if (AGPDLOGTYPE_ETC_MAX == eAction)
					continue;
				
				m_EtcFilter[eAction] = TRUE;
				}
				break;
			
			case AGPDLOG_CATEGORY_GHELD :
				{
				eAGPDLOGTYPE_GHELD eAction = AGPDLOGTYPE_GHELD_MAX;
				for (INT32 i=0; i<AGPDLOGTYPE_GHELD_MAX; ++i)
					{
					if (psz && 0 == stricmp(psz, AgpdLog_Gheld::m_szAct[i]))
						{
						eAction = (eAGPDLOGTYPE_GHELD) i;
						break;
						}
					}
				
				if (AGPDLOGTYPE_GHELD_MAX == eAction)
					continue;
				
				m_GheldFilter[eAction] = TRUE;
				}
				break;
			
			default :
				break;
			}
		}
		
	return TRUE;
	}




//	Base
//============================================
//
inline BOOL AgpmLog::WriteLog(PVOID pvLog, INT16 nSize)
	{
	if(!pvLog)
		return FALSE;

	return EnumCallback(AGPMLOG_CB_WRITE, pvLog, &nSize);
	}


BOOL AgpmLog::WriteLog_Play(eAGPDLOGTYPE_PLAY eType, INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lCharLevelFrom, INT32 lPartyTime, INT32 lSoloTime, INT32 lKillPC, INT32 lKillMon, INT32 lDeadByPC, INT32 lDeadByMon)
	{
	if (CheckPlayFilter(eType))
		return TRUE;

	AgpdLog_Play cLog;
	cLog.SetBasicData(lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID, lCharLevel, llCharExp, llGheldInven, llGheldBank);
	cLog.m_cFlag = eType;

	cLog.m_lCharLevelFrom = lCharLevelFrom;
	cLog.m_lPartyTime = lPartyTime;
	cLog.m_lSoloTime = lSoloTime;
	cLog.m_lKillPC = lKillPC;
	cLog.m_lKillMonster = lKillMon;
	cLog.m_lDeadByPC = lDeadByPC;
	cLog.m_lDeadByMonster = lDeadByMon;
	
	return WriteLog(&cLog, sizeof(cLog));
	}


BOOL AgpmLog::WriteLog_Item(BOOL bIsCashItem, eAGPDLOGTYPE_ITEM eType, INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						UINT64 ullItemDBID, INT32 lItemTID, INT32 lItemQty, CHAR *pszConvert, CHAR *pszOption,
						INT32 lGheld, CHAR *pszCharID2, INT32 lInUse, INT32 lUseCount, INT64 lRemainTime, INT32 lExpireDate, INT64 llStaminaRemainTime,
						INT32 lDurability, INT32 lMaxDurability, CHAR* pszPosition)
	{
	// DB 부하 때문에 이부분 줄였다. 2008.01.04. steeple
	if (eType == AGPDLOGTYPE_ITEM_BANK_LOGOUT)
		return TRUE;

	if (CheckItemFilter(lItemTID, eType))
		return TRUE;


	AgpdLog_Item cLog;
	cLog.SetBasicData(lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID, lCharLevel, llCharExp, llGheldInven, llGheldBank);
	cLog.m_cFlag = eType;
	
	if (bIsCashItem)
		cLog.m_cLogType = AGPDLOG_CATEGORY_CASHITEM;

	cLog.m_ullItemDBID = ullItemDBID;
	cLog.m_lItemTID = lItemTID;
	cLog.m_lItemQty = lItemQty;
	strncpy(cLog.m_szConvert, pszConvert ? pszConvert : "", AGPDLOG_MAX_ITEM_CONVERT_STRING);
	strncpy(cLog.m_szOption, pszOption ? pszOption : "", AGPDLOG_MAX_ITEM_FULL_OPTION);
	cLog.m_lGheld = lGheld;
	strncpy(cLog.m_szCharID2, pszCharID2 ? pszCharID2 : "", AGPDLOG_MAX_STR_ID);
	cLog.m_lInUse = lInUse;
	cLog.m_lUseCount = lUseCount;
	cLog.m_lRemainTime = lRemainTime;
	cLog.m_lExpireDate = lExpireDate;
	cLog.m_llStaminaRemainTime = llStaminaRemainTime;
	if(!pszPosition)
		sprintf(cLog.m_szDescription, "%d/%d", lDurability, lMaxDurability);
	else
	{
		sprintf(cLog.m_szDescription, "%d/%d [%s]", lDurability, lMaxDurability, pszPosition);
	}
	
	return WriteLog(&cLog, sizeof(cLog));
	}


BOOL AgpmLog::WriteLog_ETC(eAGPDLOGTYPE_ETC eType, INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lNumID, CHAR *pszStrID, CHAR *pszDesc, INT32 lGheld, CHAR *pszCharID2)
	{
	if (CheckEtcFilter(eType))
		return TRUE;
		
	AgpdLog_ETC cLog;
	cLog.SetBasicData(lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID, lCharLevel, llCharExp, llGheldInven, llGheldBank);
	cLog.m_cFlag = eType;

	cLog.m_lNumID = lNumID;
	strncpy(cLog.m_szStrID, pszStrID ? pszStrID : "", AGPDLOG_MAX_STR_ID);
	strncpy(cLog.m_szDescription, pszDesc ? pszDesc : "", AGPDLOG_MAX_DESCRIPTION);
	cLog.m_lGheld = lGheld;
	strncpy(cLog.m_szCharID2, pszCharID2 ? pszCharID2 : "", AGPDLOG_MAX_STR_ID);
	
	return WriteLog(&cLog, sizeof(cLog));
	}


BOOL AgpmLog::WriteLog_Gheld(eAGPDLOGTYPE_GHELD eType, INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lGheld, CHAR *pszCharID2)
	{
	if (CheckGheldFilter(eType))
		return TRUE;

	AgpdLog_Gheld cLog;
	cLog.SetBasicData(lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID, lCharLevel, llCharExp, llGheldInven, llGheldBank);
	cLog.m_cFlag = eType;

	cLog.m_lGheldChange = lGheld;
	strncpy(cLog.m_szCharID2, pszCharID2 ? pszCharID2 : "", AGPDLOG_MAX_STR_ID);
	
	return WriteLog(&cLog, sizeof(cLog));
	}




//	Play
//===================================
//
BOOL AgpmLog::WriteLog_Login(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank)
	{
	return WriteLog_Play(AGPDLOGTYPE_PLAY_GAMEIN, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
						lCharLevel, llCharExp, llGheldInven, llGheldBank,
						lCharLevel, 0, 0, 0, 0, 0, 0
						);
	}


BOOL AgpmLog::WriteLog_Logout(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									INT32 lPartyTime, INT32 lSoloTime, INT32 lKillPC, INT32 lKillMon, INT32 lDeadByPC, INT32 lDeadByMon)
	{
	return WriteLog_Play(AGPDLOGTYPE_PLAY_GAMEOUT, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
						lCharLevel, llCharExp, llGheldInven, llGheldBank,
						lCharLevel, lPartyTime, lSoloTime, lKillPC, lKillMon, lDeadByPC, lDeadByMon
						);
	}


BOOL AgpmLog::WriteLog_LevelUp(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							INT32 lCharLevelFrom, INT32 lPartyTime, INT32 lSoloTime, INT32 lKillPC, INT32 lKillMon, INT32 lDeadByPC, INT32 lDeadByMon)
	{
	return WriteLog_Play(AGPDLOGTYPE_PLAY_LEVELUP, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
						lCharLevel, llCharExp, llGheldInven, llGheldBank,
						lCharLevelFrom, lPartyTime, lSoloTime, lKillPC, lKillMon, lDeadByPC, lDeadByMon
						);
	}


BOOL AgpmLog::WriteLog_Tick(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
									INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
									INT32 lPartyTime, INT32 lSoloTime, INT32 lKillPC, INT32 lKillMon, INT32 lDeadByPC, INT32 lDeadByMon)
	{
	return WriteLog_Play(AGPDLOGTYPE_PLAY_TICK, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
						lCharLevel, llCharExp, llGheldInven, llGheldBank,
						lCharLevel, lPartyTime, lSoloTime, lKillPC, lKillMon, lDeadByPC, lDeadByMon
						);
	}




//	ETC
//=======================================
//
BOOL AgpmLog::WriteLog_SkillBuy(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							INT32 lSkillTID, CHAR *pszDesc, INT32 lGheld)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_SKILL_BUY, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, lGheld
				);
	}


BOOL AgpmLog::WriteLog_SkillUp(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							INT32 lSkillTID, CHAR *pszDesc, INT32 lGheld)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_SKILL_UPGRADE, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, lGheld
				);
	}


BOOL AgpmLog::WriteLog_SkillInit(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							CHAR *pszDesc, INT32 lGheld)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_SKILL_INIT, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, NULL, pszDesc, lGheld
				);
	}


BOOL AgpmLog::WriteLog_PDSkillBuy(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							INT32 lPDSkillTID, CHAR *pszDesc, INT32 lGheld)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_PDSKILL_BUY, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lPDSkillTID, NULL, pszDesc, lGheld
				);
	}


BOOL AgpmLog::WriteLog_QuestAccept(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							INT32 lQuestTID, CHAR *pszDesc)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_QUEST_ACCEPT, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lQuestTID, NULL, pszDesc, 0
				);
	}


BOOL AgpmLog::WriteLog_QuestCancel(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							INT32 lQuestTID, CHAR *pszDesc)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_QUEST_CANCEL, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lQuestTID, NULL, pszDesc, 0
				);
	}


BOOL AgpmLog::WriteLog_QuestComplete(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
							INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
							INT32 lQuestTID, CHAR *pszDesc)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_QUEST_COMPLETE, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lQuestTID, NULL, pszDesc, 0
				);
	}


BOOL AgpmLog::WriteLog_GuildCreate(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszGuildID, CHAR *pszDesc, INT32 lGheld)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_GUILD_CREATE, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszGuildID, pszDesc, lGheld
				);
	}


BOOL AgpmLog::WriteLog_GuildRemove(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszGuildID, CHAR *pszDesc)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_GUILD_REMOVE, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszGuildID, pszDesc, 0
				);
	}


BOOL AgpmLog::WriteLog_GuildIn(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszGuildID, CHAR *pszDesc)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_GUILD_IN, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszGuildID, pszDesc, 0
				);
	}


BOOL AgpmLog::WriteLog_GuildOut(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszGuildID, CHAR *pszDesc)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_GUILD_OUT, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszGuildID, pszDesc, 0
				);
	}


BOOL AgpmLog::WriteLog_GuildBattleRequest(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszGuildID, CHAR *pszEnemyGuildID)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_GUILD_BATTLE_REQ, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszGuildID, pszEnemyGuildID, 0
				);
	}


BOOL AgpmLog::WriteLog_GuildBattleAccept(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszGuildID, CHAR *pszEnemyGuildID)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_GUILD_BATTLE_ACP, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszGuildID, pszEnemyGuildID, 0
				);
	}


BOOL AgpmLog::WriteLog_CharCustomize(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR *pszDesc, INT32 lGheld)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_CHAR_CUSTOMIZE, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, NULL, pszDesc, lGheld
				);
	}


BOOL AgpmLog::WriteLog_BankExpansion(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lBankSize, INT32 lGheld)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_BANK_EXPANSION, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lBankSize, NULL, NULL, lGheld
				);
	}


BOOL AgpmLog::WriteLog_CharDeath(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR *pszEnemy, CHAR *pszDesc, CHAR *pszGuild)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_DEATH, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszGuild, pszDesc, 0, pszEnemy
				);
	}


BOOL AgpmLog::WriteLog_CharKill(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR *pszEnemy, CHAR *pszDesc, CHAR *pszGuild)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_KILL, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszGuild, pszDesc, 0, pszEnemy
				);
	}


BOOL AgpmLog::WriteLog_GuildExpansion(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszGuildID, INT32 lGheld, INT32 lMaxMember)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_GUILD_EXPANSION, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lMaxMember, pszGuildID, NULL, lGheld
				);
	}


BOOL AgpmLog::WriteLog_GuildRename(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszGuildID, CHAR *pszOldName)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_GUILD_RENAME, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszGuildID, pszOldName, 0
				);
	}


BOOL AgpmLog::WriteLog_Teleport(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszFrom, CHAR *pszTo, INT32 lGheld)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TELEPORT, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszFrom, pszTo, lGheld
				);
	}

BOOL AgpmLog::WriteLog_BattleGroundTeleport(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszFrom, CHAR *pszTo, INT32 lGheld)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_BATTLEGROUND_TELEPORT, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
		lCharLevel, llCharExp, llGheldInven, llGheldBank,
		0, pszFrom, pszTo, lGheld
		);
}


BOOL AgpmLog::WriteLog_GuildBuyMark(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								CHAR* pszGuildID, CHAR *pszDesc, INT32 lGheld)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_GUILD_BUY_MARK, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				0, pszGuildID, pszDesc, lGheld
				);
	}


BOOL AgpmLog::WriteLog_Remission(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lPoint, CHAR *pszDesc)
	{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_REMISSION, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lPoint, NULL, pszDesc, 0
				);
	}


BOOL AgpmLog::WriteLog_MurdererUp(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lPoint, INT32 lTotalPoint, CHAR* pszID)
	{
	TCHAR sz[AGPDLOG_MAX_DESCRIPTION+1];
	ZeroMemory(sz, sizeof(sz));
	sprintf(sz, "%d/%d", lPoint, lTotalPoint);

	return WriteLog_ETC(AGPDLOGTYPE_ETC_MURDERER_UP, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lPoint, NULL, sz, 0, pszID
				);
	}

BOOL AgpmLog::WriteLog_CharismaUp(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lPoint, INT32 lTotalPoint, CHAR* pszID)
	{
	TCHAR sz[AGPDLOG_MAX_DESCRIPTION+1];
	ZeroMemory(sz, sizeof(sz));
	sprintf(sz, "%d/%d", lPoint, lTotalPoint);

	return WriteLog_ETC(AGPDLOGTYPE_ETC_CHARISMA_UP, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lPoint, NULL, sz, 0, pszID
				);
	}

BOOL AgpmLog::WriteLog_NoChat(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID, 
							  INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank, 
							  INT32 lSkillTID, CHAR *pszDesc, INT32 lGheld, CHAR* pszExcutor)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_NOCHAT, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
						lCharLevel, llCharExp, llGheldInven, llGheldBank,
						lSkillTID, NULL, pszDesc, lGheld, pszExcutor);
}

BOOL AgpmLog::WriteLog_TitleQuestAdd(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLEQUEST_ADD, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleQuestComplete(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLEQUEST_COMPLETE, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleAdd(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLE_ADD, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleUse(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLE_USE, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleDelete(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLE_DELETE, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleAddByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLE_ADD_BY_GM, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleDeleteByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLE_DELETE_BY_GM, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleEditByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLE_EDIT_BY_GM, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleQuestAddByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLEQUEST_ADD_BY_GM, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleQuestEditByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLEQUEST_EDIT_BY_GM, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleQuestDeleteByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLEQUEST_DELETE_BY_GM, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL AgpmLog::WriteLog_TitleQuestCompleteByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
						INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
						INT32 lSkillTID, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_TITLEQUEST_COMPLETE_BY_GM, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
				lCharLevel, llCharExp, llGheldInven, llGheldBank,
				lSkillTID, NULL, pszDesc, 0
				);
}

BOOL	AgpmLog::WriteLog_CharismaUpByGm(INT32 lTimeStamp, CHAR *pszIP, CHAR *pszAccountID, CHAR *pszWorld, CHAR *pszCharID, INT32 lCharTID,
								INT32 lCharLevel, INT64 llCharExp, INT64 llGheldInven, INT64 llGheldBank,
								INT32 lSkillTID /* Charisma Point 의미 */, CHAR *pszDesc)
{
	return WriteLog_ETC(AGPDLOGTYPE_ETC_CHARISMA_UP_BY_GM, lTimeStamp, pszIP, pszAccountID, pszWorld, pszCharID, lCharTID,
		lCharLevel, llCharExp, llGheldInven, llGheldBank,
		lSkillTID, NULL, pszDesc, 0
		);
}

// 2006.07.12. steeple
// 채팅 로그는 파일로 남긴다. 하하하하
BOOL AgpmLog::WriteLog_Chatting(INT32 lTimeStamp, CHAR* szSender, CHAR* szMessage, INT32 lMessageLength)
	{
	if(!m_bWriteChattingLog)
		return TRUE;

	if(lMessageLength > 400)
		return FALSE;

	TCHAR szTmp[512 + 1];
	TCHAR szTmp2[AUTIMESTAMP_SIZE_ORACLE_TIME_STRING + 1];

	memset(szTmp, 0, sizeof(szTmp));
	memset(szTmp2, 0, sizeof(szTmp2));

	memcpy(szTmp, szMessage, lMessageLength);
	szTmp[lMessageLength] = '\0';

	if(lTimeStamp == 0)
		lTimeStamp = AuTimeStamp::GetCurrentTimeStamp();

	AuTimeStamp::ConvertTimeStampToOracleTime(lTimeStamp, szTmp2, sizeof(szTmp2));
	
	HANDLE hFile = CreateFile(AGPMLOG_CHATTING_FILE_NAME,
								GENERIC_READ | GENERIC_WRITE, 0, NULL,
								OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	SetFilePointer(hFile, 0, 0, FILE_END);		// 얘 없어서 계속 덮어쓰고 있었다. -_-;

	CHAR pBuffer[512 + 1];
	sprintf(pBuffer, "%s (%s) : %s\r\n", szSender, szTmp2, szTmp);
	DWORD dwWritten = 0;
	WriteFile(hFile, pBuffer, (DWORD)strlen(pBuffer), &dwWritten, NULL);

	CloseHandle(hFile);
	return TRUE;
	}


// 2007.11.06. steeple
// PC Room
BOOL AgpmLog::WriteLog_PCRoom(UINT32 ulTimeStamp, const CHAR* szGameID, CHAR* szServerID, CHAR* szAccountID, CHAR* szWorldID,
								CHAR* szCharID, UINT32 ulLoginTimeStamp, CHAR* szIP, INT32 lLoginLV, INT32 lLogoutLV,
								INT64 llGheldInven, INT64 llGheldBank, CHAR* szCRMCode, CHAR* szGrade)
{
	AgpdLog_PCRoom cLog;
	cLog.SetBasicData(ulTimeStamp, szIP, szAccountID, szWorldID, szCharID, 0, lLogoutLV, 0, llGheldInven, llGheldBank);

	_tcsncpy(cLog.m_szGameID, szGameID, AGPDLOG_MAX_GAMEID);
	cLog.m_ulLoginTimeStamp = ulLoginTimeStamp;
	cLog.m_ulLogoutTimeStamp = ulTimeStamp;
	cLog.m_ulPlayTime = ulTimeStamp - ulLoginTimeStamp;
	cLog.m_lLoginLevel = lLoginLV;
	_tcsncpy(cLog.m_szCRMCode, szCRMCode, AGPDLOG_MAX_PCROOM_CRM_CODE);
	_tcsncpy(cLog.m_szGrade, szGrade, AGPDLOG_MAX_GRADE);
	
	return WriteLog(&cLog, sizeof(cLog));
}




//	Filter check
//===========================================
//
BOOL AgpmLog::CheckItemFilter(INT32 lItemTID, eAGPDLOGTYPE_ITEM eAction)
	{
	if (0 > eAction || eAction >= AGPDLOGTYPE_ITEM_MAX)
		return FALSE;

	// Gheld는 무조건 제거.	
	if (268 == lItemTID)
		return TRUE;
	
	if (NULL == m_ItemFilter[eAction])
		return FALSE;

	ItemFilterIter Iter;
	Iter = m_ItemFilter[eAction]->find(292513);	// when 292513 found, ignore all log of this action
	if (Iter != m_ItemFilter[eAction]->end())
		return TRUE;
	
	Iter = m_ItemFilter[eAction]->find(lItemTID);
	if (Iter == m_ItemFilter[eAction]->end())
		return FALSE;
	
	::InterlockedIncrement(&Iter->second);
	return TRUE;
	}


BOOL AgpmLog::CheckPlayFilter(eAGPDLOGTYPE_PLAY eAction)
	{
	if (0 > eAction || eAction >= AGPDLOGTYPE_PLAY_MAX)
		return TRUE;

	return m_PlayFilter[eAction];
	}


BOOL AgpmLog::CheckEtcFilter(eAGPDLOGTYPE_ETC eAction)
	{
	if (0 > eAction || eAction >= AGPDLOGTYPE_ETC_MAX)
		return TRUE;

	return m_EtcFilter[eAction];
	}


BOOL AgpmLog::CheckGheldFilter(eAGPDLOGTYPE_GHELD eAction)
	{
	if (0 > eAction || eAction >= AGPDLOGTYPE_GHELD_MAX)
		return TRUE;

	return m_GheldFilter[eAction];
	}




//	Callback setting
//============================================
//
BOOL AgpmLog::SetCallbackWriteLog(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMLOG_CB_WRITE, pfCallback, pClass);
	}


