// AgpmConfig.h
// (C) NHN Games - ArchLord Development Team
// kelovon, 20051006

#ifndef _AGPM_CONFIG_H_
#define _AGPM_CONFIG_H_

#include "AuPacket.h"
#include "ApModule.h"

typedef	ApString<64>	ApDirString;
typedef ApString<32>	ApEncString;

// 주의: AgsdServer2.h의 AGSMSERVER_EXTRA_KEY_XXX와 일치해야 한다.
#define AGPM_CONFIG_DEFAULT_VALUE_EXP_ADJUSTMENT_RATIO		1.0f
#define AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO	1.0f
#define AGPM_CONFIG_DEFAULT_VALUE_CHARISMA_ADJUST_RATIO		1.0f
#define AGPM_CONFIG_DEFAULT_VALUE_IS_ADULT_SERVER			FALSE
#define AGPM_CONFIG_DEFAULT_VALUE_ADM_CMD_TO_ALL			FALSE
#define AGPM_CONFIG_DEFAULT_VALUE_IS_NEW_SERVER				FALSE			// 신서버 여부
#define AGPM_CONFIG_DEFAULT_VALUE_PC_DROP_ITEM				FALSE			// PC가 죽었을 때 Item Drop 여부
#define AGPM_CONFIG_DEFAULT_VALUE_INI_DIR					"Ini"			// INI 디렉토리 명
#define	AGPM_CONFIG_VALUE_NO_ACCOUNT_AUTH					_T('n')
#define	AGPM_CONFIG_VALUE_WEB_ACCOUNT_AUTH					_T('w')
#define	AGPM_CONFIG_VALUE_DB_ACCOUNT_AUTH					_T('d')
#define	AGPM_CONFIG_VALUE_IGNORE_ACCOUNT_AUTH				_T('i')
#define	AGPM_CONFIG_DEFAULT_VALUE_ACCOUNT_AUTH				AGPM_CONFIG_VALUE_WEB_ACCOUNT_AUTH
#define	AGPM_CONFIG_DEFAULT_VALUE_IGNORE_LOG_FAIL			FALSE
#define AGPM_CONFIG_DEFAULT_VALUE_FILE_LOG					FALSE
#define AGPM_CONFIG_DEFAULT_VALUE_EXP_PENALTY				TRUE			// PC가 죽었을 때 EXP Down 여부
#define AGPM_CONFIG_DEFAULT_VALUE_START_LEVEL				1				// 케릭터 생성시 레벨
#define AGPM_CONFIG_DEFAULT_VALUE_START_GHELD				0				// 케릭터 생성시 소지 겔드
#define AGPM_CONFIG_DEFAULT_VALUE_ENABLE_AUCTION			TRUE			// Aution 가능여부
#define AGPM_CONFIG_DEFAULT_VALUE_ENABLE_EVENT_ITEM_EFFECT	TRUE			// EventItemEffect 효과 여부 
#define AGPM_CONFIG_DEFAULT_VALUE_ENABLE_PVP				TRUE			// PVP 가능여부

#define AGPM_CONFIG_DEFAULT_VALUE_LIMIT_LEVEL				0				// 제한레벨..세팅이 되어있을때만 사용 한다..//JK_특성화서버
#define AGPM_CONFIG_DEFAULT_VALUE_LIMIT_UNDERAGE			0				// 심야샷다운 제한 나이...세팅이 되어있을때만 사용 한다..//JK_특성화서버


// 서버 이벤트 진행 관련 Flag들
#define	AGPM_CONFIG_DEFAULT_VALUE_EVENT_CHATTING			FALSE			// 330관련 채팅창 이벤트
#define	AGPM_CONFIG_DEFAULT_VALUE_EVENT_RESURRECT			FALSE			// 사망 시 부활 패널티 적용 안하는 이벤트
#define	AGPM_CONFIG_DEFAULT_VALUE_EVENT_ITEMDROP			FALSE

typedef enum _AgpmConfigServerStatusFlag {
	AGPM_CONFIG_FLAG_NORMAL						= 0x00000000,
	AGPM_CONFIG_FLAG_SIEGE_WAR					= 0x00000001,
	AGPM_CONFIG_FLAG_ADMIN_EVENT_ITEM			= 0x00000002,
} AgpmConfigServerStatusFlag;

struct AgpdTPack
{
	FLOAT		fTPackExpRatio;
	FLOAT		fTPackDropRatio;
	FLOAT		fTPackGheldRatio;
	FLOAT		fTPackConvertRatio;
	FLOAT		fTPackCharismaRatio;

public:
	AgpdTPack()
		: fTPackExpRatio(0.0f), fTPackDropRatio(0.0f), fTPackGheldRatio(0.0f), fTPackConvertRatio(0.0f), fTPackCharismaRatio(0.0f)
	{

	}

	~AgpdTPack()
	{
		Init();
	}

	void		Init()
	{
		fTPackExpRatio		= 0.0f;
		fTPackDropRatio		= 0.0f;
		fTPackGheldRatio	= 0.0f;
		fTPackConvertRatio	= 0.0f;
		fTPackCharismaRatio	= 0.0f;
	}
};

struct AgpdConfig
{
	INT32		lEventNumber;					//더블데이류의 이벤트 진행시 이벤트 구분 아뒤.
	float		fExpAdjustmentRatio;
	float		fDropAdjustmentRatio;
	float		fDrop2AdjustmentRatio;
	float		fGheldDropAdjustmentRatio;
	float		fCharismaDropAdjustmentRatio;
	BOOL		bIsAdultServer;
	BOOL		bIsAllAdmin;					// 모든 사용자가 admin command 사용 가능, 개발용으로만 사용
	BOOL		bIsNewServer;
	BOOL		bIsEventServer;
	BOOL		bIsAimEventServer;
	BOOL		bPCDropItemOnDeath;
	ApDirString	szIniDir;
	TCHAR		cAccountAuth;
	BOOL		bIgnoreLogFail;
	BOOL		bFileLog;
	INT32		lMaxUserCount;
	BOOL		bExpPenaltyOnDeath;

	BOOL		bIsEventChatting;
	BOOL		bIsEventResurrect;
	BOOL		bIsEventItemDrop;

	BOOL		bIsTestServer;

	BOOL		bIsEnableAuction;

	BOOL		bIsEventItemEffect;
	BOOL		bIsAblePvP;

	INT32		lStartLevel;
	INT64		llStartGheld;

	INT32		lLimitLevel;	//JK_특성화서버
	INT32		lLimitAge;		//JK_심야샷다운

	// 서버 상태 관련 플래그 (진행중인 이벤트라던가,,,)
	UINT32		ulServerStatus;

	ApEncString szEncPublic;					// 2006.06.14. steeple. 사용하는 알고리즘.
	ApEncString szEncPrivate;					// 2006.06.14. steeple. 사용하는 알고리즘.

	AgpdConfig()
	{
		Reset();
	}

	void Reset()
	{
		lEventNumber				= 0;
		fExpAdjustmentRatio			= AGPM_CONFIG_DEFAULT_VALUE_EXP_ADJUSTMENT_RATIO;
		fDropAdjustmentRatio		= AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO;
		fDrop2AdjustmentRatio		= AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO;
		fGheldDropAdjustmentRatio	= AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO;
		fCharismaDropAdjustmentRatio= AGPM_CONFIG_DEFAULT_VALUE_CHARISMA_ADJUST_RATIO;
		bIsAdultServer				= AGPM_CONFIG_DEFAULT_VALUE_IS_ADULT_SERVER;
		bIsAllAdmin					= AGPM_CONFIG_DEFAULT_VALUE_ADM_CMD_TO_ALL;
		bIsNewServer				= AGPM_CONFIG_DEFAULT_VALUE_IS_NEW_SERVER;
		bPCDropItemOnDeath			= AGPM_CONFIG_DEFAULT_VALUE_PC_DROP_ITEM;
		szIniDir					= AGPM_CONFIG_DEFAULT_VALUE_INI_DIR;
		cAccountAuth				= AGPM_CONFIG_DEFAULT_VALUE_ACCOUNT_AUTH;
		bIgnoreLogFail				= AGPM_CONFIG_DEFAULT_VALUE_IGNORE_LOG_FAIL;
		bFileLog					= AGPM_CONFIG_DEFAULT_VALUE_FILE_LOG;
		lMaxUserCount				= 0;
		bExpPenaltyOnDeath			= AGPM_CONFIG_DEFAULT_VALUE_EXP_PENALTY;

		bIsEventChatting			= AGPM_CONFIG_DEFAULT_VALUE_EVENT_CHATTING;
		bIsEventResurrect			= AGPM_CONFIG_DEFAULT_VALUE_EVENT_RESURRECT;
		bIsEventItemDrop			= AGPM_CONFIG_DEFAULT_VALUE_EVENT_ITEMDROP;

		ulServerStatus				= AGPM_CONFIG_FLAG_NORMAL;

		bIsTestServer				= FALSE;
		bIsEventServer				= FALSE;
		bIsAimEventServer			= FALSE;
		bIsEnableAuction			= TRUE;
		bIsEventItemEffect			= TRUE;
		bIsAblePvP					= TRUE;

		lStartLevel					= AGPM_CONFIG_DEFAULT_VALUE_START_LEVEL;
		llStartGheld				= AGPM_CONFIG_DEFAULT_VALUE_START_GHELD;

		lLimitLevel					= AGPM_CONFIG_DEFAULT_VALUE_LIMIT_LEVEL; //JK_특성화서버
		lLimitAge					= AGPM_CONFIG_DEFAULT_VALUE_LIMIT_UNDERAGE; //JK_심야샷다운

		szEncPublic.Clear();
		szEncPrivate.Clear();
	};
};

class AgpmConfig : public ApModule
{
public:
	AgpmConfig();
	virtual ~AgpmConfig();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle2(UINT32 ulClockCount);
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL OnDestroy();

	void ResetConfig() { m_stAgpdConfig.Reset(); }

	// 이벤트 번호
	void SetEventNumber(INT32 lEventNumber = 0);
	INT32 GetEventNumber();

	// 경험치 조정율
	void SetExpAdjustmentRatio(float fRatio = AGPM_CONFIG_DEFAULT_VALUE_EXP_ADJUSTMENT_RATIO);
	float GetExpAdjustmentRatio(BOOL bTPackUser = FALSE);

	// 드랍율 조정율
	void SetDropAdjustmentRatio(float fRatio = AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO);
	float GetDropAdjustmentRatio(BOOL bTPackUser = FALSE);

	// 드랍율2 조정율
	void SetDrop2AdjustmentRatio(float fRatio = AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO);
	float GetDrop2AdjustmentRatio(BOOL bTPackUser = FALSE);

	// 겔드 드랍 조정
	void SetGheldDropAdjustmentRatio(float fRatio = AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO);
	float GetGheldDropAdjustmentRatio(BOOL bTPackUser = FALSE);

	// 카리스마 드랍 조정
	void SetCharismaDropAdjustmentRatio(float fRatio = AGPM_CONFIG_DEFAULT_VALUE_CHARISMA_ADJUST_RATIO);
	float GetCharismaDropAdjustmentRatio(BOOL bTPackUser = FALSE);

	// 성인 서버 여부
	void SetAdultServer(BOOL bIsAdultServer = AGPM_CONFIG_DEFAULT_VALUE_IS_ADULT_SERVER);
	BOOL IsAdultServer();

	// 모든 사용자가 admin command 사용 가능한지 여부, 개발용으로만 사용
	void SetAllAdmin(BOOL bIsAllAdmin = AGPM_CONFIG_DEFAULT_VALUE_ADM_CMD_TO_ALL);
	BOOL IsAllAdmin();

	// 신서버 여부
	void SetNewServer(BOOL bIsNewServer = AGPM_CONFIG_DEFAULT_VALUE_IS_NEW_SERVER);
	BOOL IsNewServer();

	// PC가 죽었을 때 Item Drop 여부
	void SetPCDropItemOnDeath(BOOL bPCDropItemOnDeath = AGPM_CONFIG_DEFAULT_VALUE_PC_DROP_ITEM);
	BOOL DoesPCDropItemOnDeath();

	// PC가 죽었을 때 경험치 Penalty 여부
	void SetExpPenaltyOnDeath(BOOL bExpPenaltyOnDeath = AGPM_CONFIG_DEFAULT_VALUE_EXP_PENALTY);
	BOOL DoesExpPenaltyOnDeath();

	// INI 디렉토리 명
	void SetIniDir(LPCTSTR szIniDir = AGPM_CONFIG_DEFAULT_VALUE_INI_DIR);
	LPCTSTR GetIniDir();

	// Auction 가능여부
	void SetEnableAuction(BOOL bEnableAuction = AGPM_CONFIG_DEFAULT_VALUE_ENABLE_AUCTION);
	BOOL IsEnableAuction();

	// EventItemEffect 효과 여부
	void SetEnableEventItemEffect(BOOL bEnableEventItemEffect = AGPM_CONFIG_DEFAULT_VALUE_ENABLE_EVENT_ITEM_EFFECT);
	BOOL IsEnableEventItemEffect();

	// PVP 가능여부
	void SetEnablePvP(BOOL bEnalbePvP = AGPM_CONFIG_DEFAULT_VALUE_ENABLE_PVP);
	BOOL IsEnablePvP();

	void SetAccountAuth(CHAR cAccountAuth = AGPM_CONFIG_DEFAULT_VALUE_ACCOUNT_AUTH);
	BOOL IsNoAccountAuth();
	BOOL IsWebAccountAuth();
	BOOL IsDBAccountAuth();
	BOOL IsIgnoreAccountAuth();
	
	void SetIgnoreLogFail(BOOL bIgnore = AGPM_CONFIG_DEFAULT_VALUE_IGNORE_LOG_FAIL);
	BOOL IsIgnoreLogFail();

	void SetFileLog(BOOL bFile = AGPM_CONFIG_DEFAULT_VALUE_FILE_LOG);
	BOOL IsFileLog();	

	void SetEventChatting(BOOL bIsEventChatting = AGPM_CONFIG_DEFAULT_VALUE_EVENT_CHATTING);
	BOOL IsEventChatting();

	void SetEventResurrect(BOOL bIsEventResurrect = AGPM_CONFIG_DEFAULT_VALUE_EVENT_RESURRECT);
	BOOL IsEventResurrect();

	void SetEventItemDrop(BOOL bIsEventItemDrop = AGPM_CONFIG_DEFAULT_VALUE_EVENT_ITEMDROP);
	BOOL IsEventItemDrop();

	void SetServerStatusFlag(AgpmConfigServerStatusFlag eFlag);
	void ResetServerStatusFlag(AgpmConfigServerStatusFlag eFlag);
	BOOL IsSetServerStatusFlag(AgpmConfigServerStatusFlag eFlag);

	void SetMaxUserCount(INT32 lMaxUserCount = 0);
	INT32 GetMaxUserCount();

	void SendConfigPacket(UINT32 ulNID);

	void SetEncPublic(LPCSTR szEncAlgorithm) { if(szEncAlgorithm) m_stAgpdConfig.szEncPublic.SetText(szEncAlgorithm); }
	LPCTSTR GetEncPublic() { return (LPCTSTR)m_stAgpdConfig.szEncPublic; }
	void SetEncPrivate(LPCSTR szEncAlgorithm) { if(szEncAlgorithm) m_stAgpdConfig.szEncPrivate.SetText(szEncAlgorithm); }
	LPCTSTR GetEncPrivate() { return (LPCTSTR)m_stAgpdConfig.szEncPrivate; }

	void SetTestServer(BOOL bIsTestServer) { m_stAgpdConfig.bIsTestServer = bIsTestServer; }
	BOOL IsTestServer() { return m_stAgpdConfig.bIsTestServer; }

	void SetStartLevel(INT32 lStartLevel) { m_stAgpdConfig.lStartLevel = lStartLevel; }
	INT32 GetStartLevel() { return m_stAgpdConfig.lStartLevel; }

	void SetStartGheld(INT64 llStartGheld) { m_stAgpdConfig.llStartGheld = llStartGheld; }
	INT64 GetStartGheld() { return m_stAgpdConfig.llStartGheld; }

	BOOL IsEventServer();
	void SetEventServer(BOOL bIsEventServer);

	BOOL IsAimEventServer();
	void SetAimEventServer(BOOL bIsAimEventServer);
	//JK_특성화서버
	void SetLimitLevel(INT32 lLimitLevel) { m_stAgpdConfig.lLimitLevel = lLimitLevel; }
	INT32 GetLimitLevel() { return m_stAgpdConfig.lLimitLevel; }
	//JK_심야샷다운
	void SetLimitUnderAge(INT32 lLimitAge) { m_stAgpdConfig.lLimitAge = lLimitAge; }
	INT32 GetLimitUnderAge() { return m_stAgpdConfig.lLimitAge; }


	void SetIsAdminEventItem(BOOL bSet)
	{
		if(bSet)
			m_stAgpdConfig.ulServerStatus |= AGPM_CONFIG_FLAG_ADMIN_EVENT_ITEM;
		else
			m_stAgpdConfig.ulServerStatus &= ~AGPM_CONFIG_FLAG_ADMIN_EVENT_ITEM;
	}
	BOOL GetIsAdminEventItem()
	{
		if(m_stAgpdConfig.ulServerStatus & AGPM_CONFIG_FLAG_ADMIN_EVENT_ITEM)
			return TRUE;
		else
			return FALSE;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TPack 관련

	VOID	InitTPack() { m_stAgpdTPack.Init(); }

	VOID	SetTPackExpRatio(FLOAT fExpRatio)			{ m_stAgpdTPack.fTPackExpRatio = fExpRatio; }
	VOID	SetTPackDropRatio(FLOAT fDropRatio)			{ m_stAgpdTPack.fTPackDropRatio = fDropRatio; }
	VOID	SetTPackGheldRatio(FLOAT fGheldRatio)		{ m_stAgpdTPack.fTPackGheldRatio = fGheldRatio; }
	VOID	SetTPackConvertRatio(FLOAT fConvertRatio)	{ m_stAgpdTPack.fTPackConvertRatio = fConvertRatio; }
	VOID	SetTPackCharismaRatio(FLOAT fCharismaRatio)	{ m_stAgpdTPack.fTPackCharismaRatio = fCharismaRatio; }

	FLOAT	GetTPackExpRatio()							{ return m_stAgpdTPack.fTPackExpRatio; }
	FLOAT	GetTPackDropRatio()							{ return m_stAgpdTPack.fTPackDropRatio; }
	FLOAT	GetTPackGheldRatio()						{ return m_stAgpdTPack.fTPackGheldRatio; }
	FLOAT	GetTPackConvertRatio()						{ return m_stAgpdTPack.fTPackConvertRatio; }
	FLOAT	GetTPackCharismaRatio()						{ return m_stAgpdTPack.fTPackCharismaRatio; }

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
	AgpdConfig	m_stAgpdConfig;
	AgpdTPack	m_stAgpdTPack;
	AuPacket	m_csPacket;
};

#endif // _AGPM_CONFIG_H_
