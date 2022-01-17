#pragma once

#include "AgcmUIManager2.h"
#include "AgcmUIControl.h"
#include "AgcmGuild.h"

#define GUILD_BATTLE_RESULT_MAX		100

struct GuildBattleUnit
{
	string		strName;
	UINT32		ulKO;
	UINT32		ulKill;
	UINT32		ulDeath;
	UINT32		ulSequence;

	static UINT32 ulBattleMode;
	
	GuildBattleUnit( char* szName, UINT32 ulKO, UINT32 ulKill, UINT32 ulDeath, UINT32 ulSequence )
	{
		strName = szName ? szName : "Empty";
		this->ulKO		= ulKO;
		this->ulKill	= ulKill;
		this->ulDeath	= ulDeath;
		this->ulSequence= ulSequence;
	}

	bool operator == ( CHAR* szName )
	{
		return strName == szName ? true : false;
	}

	static bool UnitSort( const GuildBattleUnit& lhs, const GuildBattleUnit& rhs )
	{
		switch( ulBattleMode )
		{
		case eGuildBattlePrivateSurvive:
			return lhs.ulSequence <= rhs.ulSequence;
		default:
			return lhs.ulKO > rhs.ulKO;
		}
	}
};
typedef vector< GuildBattleUnit >		GuildResultUnitVec;
typedef GuildResultUnitVec::iterator	GuildResultUnitVecItr;

struct GuildBattleTeamInfo
{
	string				strGuildName;
	string				strMasterName;
	GuildResultUnitVec	vecGuildResultUnit;
	UINT32				lTotal;

	void	Clear()
	{
		strGuildName.clear();
		strMasterName.clear();
		vecGuildResultUnit.clear();
		lTotal	= 0;
	}

	void	SetInfo( char* szGuildName, char* szMasterName, INT32 lTotal )
	{
		if( szGuildName )	strGuildName	= szGuildName;
		if( szMasterName )	strMasterName	= szMasterName; 
		this->lTotal	= lTotal;
	}

	void	PushUnit( char* szName, UINT32 ulKO, UINT32 ulKill, UINT32 ulDeath, UINT32 ulSequence )
	{
		vecGuildResultUnit.push_back( GuildBattleUnit( szName, ulKO, ulKill, ulDeath, ulSequence ) );
	}

	void	Update( char* szName, UINT32 ulKO, UINT32 ulKill, UINT32 ulDeath, UINT32 ulSequence )
	{
		GuildResultUnitVecItr Itr = find( vecGuildResultUnit.begin(), vecGuildResultUnit.end(), szName );
		if( Itr != vecGuildResultUnit.end() )
		{
			(*Itr).ulKO			= ulKO;
			(*Itr).ulKill		= ulKill;
			(*Itr).ulDeath		= ulDeath;
			if( ulSequence != -1  )
				(*Itr).ulSequence = ulSequence;
		}
		else
			PushUnit( szName, ulKO, ulKill, ulDeath, ulSequence );
	}

	void	Sort()		//맴버 점수별로 소팅..
	{
		sort( vecGuildResultUnit.begin(), vecGuildResultUnit.end(), GuildBattleUnit::UnitSort );
	}
};

//0이 나.. 1이. 적
struct GuildBattleResultInfo
{
	AgpmGuildBattleResult	eResult;
	eGuildBattleType		eType;
	UINT32					ulTime;
	GuildBattleTeamInfo		cTeam[2];

	void	Clear()
	{
		eResult = AGPMGUILD_BATTLE_RESULT_NONE;
		eType	= eGuildBattlePointMatch;
		ulTime	= 0;
		cTeam[0].Clear();
		cTeam[1].Clear();
	}

	void	SetInfo( AgpdGuild* pGuild );
	void	SetMemberInfo( char* szName, UINT32 ulKO, UINT32 ulKill, UINT32 ulDeath, UINT32 ulSequence, BOOL bMy );
	void	SetMemberListInfo( INT32 lMyCount, AgpdGuildMemberBattleInfo* pMyInfo, INT32 lEnemyCount, AgpdGuildMemberBattleInfo* pEnemyInfo );
	BOOL	IsMember( char* szName, BOOL bMy );
	BOOL	IsCurRoundMember( char* szName, UINT32 ulRound, BOOL bMy );
	void	GetRoundName( UINT32 ulRound, string& strMyTeam, string& strYourTeam );
};

class AgcmUIGuild;
class AgcmUIGuildBattleResult : public GuildBattleResultInfo, public CUIOpenCloseSystem
{
public:
	AgcmUIGuildBattleResult();
	~AgcmUIGuildBattleResult();

	void	OpenResult();
	BOOL	AddModule( AgcmUIManager2* pUIMgr, AgcmUIGuild* pUIGuild );

	static BOOL CBDisplayGuildBattleInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL CBDisplayGuildNameLeft(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildResultNameLeft(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildNameRight(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildResultNameRight(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildBattleLeftTotal(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildBattleRightTotal(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL CBDisplayGuildBattleWin(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildBattleLose(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildBattleWinDetail1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildBattleWinDetail2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildBattleLoseDetail1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildBattleLoseDetail2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL CBDisplayGuildBattleMemberNameLeft( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildBattleMemberPointLeft( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildBattleMemberNameRight( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL CBDisplayGuildBattleMemberPointRight( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL IsBattleRound(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	void	RefreshUserData();
	
	UINT32	GetGPPoint( BOOL bMy );
	CHAR*	GetResultPointString();
	CHAR*	GetTimePointString();

	void	GetResultPrivateString( UINT32 ulRound , CHAR* szText, BOOL bMy );

private:
	void	SetString();

public:
	BOOL					m_bResult;
	UINT32					m_ulRound;

private:
	AgcmUIManager2*			m_pcsAgcmUIManager2;
	AgcmUIGuild*			m_pcsAgcmUIGuild;

	AgcdUIUserData*			m_pstBattleResult;
	INT32					m_lBattleResult[GUILD_BATTLE_RESULT_MAX];

	AgcdUIUserData*			m_pstBattleInfo;
	INT32					m_lBattleInfo;

	AgcdUIUserData*			m_pstWinLose[12];
	INT32					m_lWinLose[12];

	int						m_nResultRound;

	CHAR					m_szGuildBattleResultPointArray[eGuildBattleMax][256];
};