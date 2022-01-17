// AgcmGuild.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 15.

#ifndef _AGCMGUILD_H_
#define _AGCMGUILD_H_

#include "AgpmFactors.h"
#include "AgpmGuild.h"

#include "AgpmCharacter.h"
#include "AgcModule.h"
#include "AgcmCharacter.h"
#include "AgcmUIControl.h"
#include "AgcmResourceLoader.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
#pragma comment(lib, "AgcmGuildD.lib")
#else
#pragma comment(lib, "AgcmGuild.lib")
#endif
#endif

class AgpdGuildMember;
class AgcmTextBoardMng;

struct AgcdGuildMember
{
	CHAR	m_szMemberID[AGPACHARACTER_MAX_ID_STRING+1];
	INT32	m_lLevel;
	INT32	m_lRank;
	INT8	m_cStatus;
	INT32	m_lTID;

	AgcdGuildMember( AgpdGuildMember& cMember )
	{
		*this = cMember;
	}

	AgcdGuildMember& operator = ( const AgpdGuildMember& cMember )
	{
		strcpy( m_szMemberID, cMember.m_szID );
		m_lLevel	= cMember.m_lLevel;
		m_lRank		= cMember.m_lRank;
		m_cStatus	= cMember.m_cStatus;
		m_lTID		= cMember.m_lTID;

		return *this;
	}

	static bool LevelLess( const AgcdGuildMember& lhs, const AgcdGuildMember& rhs );
};

inline bool operator == ( const AgcdGuildMember& cMember, const string& strMember )
{
	return strMember == cMember.m_szMemberID ? true : false;
}

inline bool	operator == ( const AgcdGuildMember& lhs, const AgcdGuildMember& rhs )
{
	return !strcmp( lhs.m_szMemberID, rhs.m_szMemberID ) ? true : false;
}

inline bool	operator < ( const AgcdGuildMember& lhs, const AgcdGuildMember& rhs )
{
	return lhs.m_lLevel < rhs.m_lLevel;
}

inline bool	operator > ( const AgcdGuildMember& lhs, const AgcdGuildMember& rhs )
{
	return lhs.m_lLevel > rhs.m_lLevel;
}

typedef list< AgcdGuildMember >			AgcdGuildMemberList;
typedef AgcdGuildMemberList::iterator	AgcdGuildMemberListItr;

struct AgcdGuildList
{
	CHAR	m_szID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR	m_szMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR	m_szSubMasterID[ AGPACHARACTER_MAX_ID_STRING+1 ];

	INT32	m_lLevel;
	INT32	m_lWinPoint;

	INT32	m_lMemberCount;
	INT32	m_lMaxMemberCount;

	INT32	m_lGuildBattle;

	INT32	m_lGuildMarkTID;
	INT32	m_lGuildMarkColor;

};

// 2006.07.26. steeple
typedef vector<AgcdGuildList>				GuildDetailVector;
typedef vector<AgcdGuildList>::iterator		GuildDetailIter;

struct FindGuildDetail
{
public:
	FindGuildDetail( const CHAR* szGuildID ) : m_szGuildID(szGuildID) {;}

	bool operator() (AgcdGuildList& unit)
	{
		return m_szGuildID ? !_tcscmp(unit.m_szID, m_szGuildID) : FALSE;
	}

private:
	FindGuildDetail()		{		}

private:
	const CHAR* m_szGuildID;
};

struct AgcdGuildMarkTemplate
{
	CHAR*		m_szTextureName;
	RwTexture*	m_pGuildMarkTexture;

	CHAR*		m_szSmallTextureName;
	RwTexture*	m_pSmallGuildMarkTexture;
};

class AgcmGuild : public AgcModule
{
private:
	AgpmFactors*		m_pcsAgpmFactors;
	AgpmGuild*			m_pcsAgpmGuild;

	AgpmCharacter*		m_pcsAgpmCharacter;
	AgcmCharacter*		m_pcsAgcmCharacter;
	AgcmTextBoardMng*	m_pcsAgcmTextBoardMng;
	AgcmUIControl*		m_pcsAgcmUIControl;
	AgcmResourceLoader*	m_pcsAgcmResourceLoader;

public:
	AgcmGuild();
	virtual ~AgcmGuild();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	//////////////////////////////////////////////////////////////////////////
	// Packet Send
	BOOL SendCreateGuild(CHAR* szGuildName, CHAR* szPassword);
	BOOL SendJoinRequest(CHAR* szCharID);
	BOOL SendJoinReject(CHAR* szGuildID, CHAR* szMasterID);
	BOOL SendJoin(CHAR* szGuildID);
	BOOL SendLeave();
	BOOL SendForcedLeave(CHAR* szCharID);
	BOOL SendDestroy(CHAR* szPassword);
	BOOL SendNotice(CHAR* szNotice);

	BOOL SendBattlePerson(INT16 lPerson, CHAR* pPersonList, INT16 lSize);
	BOOL SendBattleRequest(CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 lPerson );
	BOOL SendBattleAccept(CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 lPerson );
	BOOL SendBattleReject(CHAR* szEnemyGuildID);
	BOOL SendBattleCancelRequest(CHAR* szEnemyGuildID);
	BOOL SendBattleCancelAccept(CHAR* szEnemyGuildID);
	BOOL SendBattleCancelReject(CHAR* szEnemyGuildID);
	BOOL SendBattleWithdraw(CHAR* szEnemyGuildID);

	BOOL SendGuildListRequest(INT16 nPage);
	BOOL SendGuildListRequestFind(CHAR *szGuildID);			//	2005.07.15 By SungHoon

	BOOL SendJointRequest(CHAR* szGuildID);
	BOOL SendJointReject(CHAR* szGuildID);
	BOOL SendJoint(CHAR* szGuildID);
	BOOL SendJointLeave();
	BOOL SendHostileRequest(CHAR* szGuildID);
	BOOL SendHostileReject(CHAR* szGuildID);
	BOOL SendHostile(CHAR* szGuildID);
	BOOL SendHostileLeaveRequest(CHAR* szGuildID);
	BOOL SendHostileLeaveReject(CHAR* szGuildID);
	BOOL SendHostileLeave(CHAR* szGuildID);

	BOOL SendMaxMemberCount(CHAR *szGuildID, INT32 lCID, INT32 lMaxMemberCount);
	BOOL SendSelfJoin(CHAR* szGuildID);
	BOOL SendJoinAllow(CHAR* szMemberID);
	BOOL SendLeaveAllow(CHAR* szMemberID);
	BOOL SendGuildRenameGuildID(INT32 lCID, CHAR *szGuildID, CHAR *szMemberID);		//	2005.08.24. By SungHoon
	BOOL SendGuildBuyGuildMark(INT32 lGuildMarkTID, INT32 lGuildMarkColor, BOOL bForce);

	BOOL SendWorldChampionshipRequest();
	BOOL SendWorldChampionshipEnter();

	// 부마스터 임명 & 해제 패킷	2009. 09. 05.
	BOOL SendAppointRequest( CHAR* szMasterID , CHAR* szTarget , INT32 Rank );
	BOOL SendAppointAnswer ( CHAR* szMasterID , CHAR* szTarget , INT32 Rank , INT32 Answer );

	// 길드 승계 패킷		2009. 09. 05
	BOOL SendSuccessionRequest	( CHAR* szMasterID , CHAR* szTarget );
	BOOL SendSuccessionAnswer	( CHAR* szMasterID , CHAR* szTarget , INT32 Answer );


	//////////////////////////////////////////////////////////////////////////
	// Callback
	static BOOL CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildUpdateMaxMemberCount(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildRenameGuildID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildRenameCharID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildMarktemplateLoad(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGetGuildMarkTexture(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGetGuildMarkSmallTexture(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL ConAgcdGuildMarkTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesAgcdGuildMarkTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCheckGuildAttackable(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CB_OnResultWorldChampionShipRequest(PVOID pData, PVOID pClass, PVOID pCustData);	

public:
	CHAR*		GetSelfGuildID();
	AgpdGuild*	GetSelfGuildLock();

	INT32 GetSelfCharLevel();
	INT32 GetMemberList(INT32 lIndex, AgcdGuildMemberList& listAgcdGuildMember, INT32 lMaxCount);
	INT32 SearchGuildMember( AgpdGuild *pcsGuild, CHAR *szMemberID);

	BOOL IsSelfGuildOperation(CHAR* szGuildID);
	BOOL IsSelfGuildMaster();
	BOOL IsGuildBattle( AgpdCharacter* pMy );
	BOOL IsGuildBattleMember( AgpdCharacter* pMy, AgpdCharacter* pTarget );

	BOOL RenameGuild(INT32 lCID, CHAR *szGuildID, CHAR *szMemberID);
	BOOL RenameCharID(CHAR *szGuildID, CHAR *szMemberID);

	BOOL SetGridGuildAttachedTexture( AgpdGuildMarkTemplate * pcsAgpdGuildMarkTemplate );		
	BOOL SetGridGuildAttachedSmallTexture( AgpdGuildMarkTemplate * pcsAgpdGuildMarkTemplate );	

	VOID ReleaseAgcdGuildMarkTemplate( AgcdGuildMarkTemplate *pcsAgcdGuildMarkTemplate);
	VOID SetTexturePath(CHAR *szPath);		

	AgcdGuildMarkTemplate* GetAttachDataGuildMarkTemplate(PVOID pData);

private:
	INT32	m_nIndexAttachDataGuildMarkTemplate;
	CHAR	m_szTexturePath[_MAX_PATH+1];
};

#endif	//_AGCMGUILD_H_