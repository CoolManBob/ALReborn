#pragma once

#include "AgcmUIManager2.h"
#include "AgcmUIControl.h"
#include "AgcmGuild.h"

#define GUILD_BATTLE_MEMBER_MAX	100	//최대 100개 되서 이렇게 해줘야 한단다..

class CGuildBattleNode
{
public:
	CGuildBattleNode( const string& name, INT32 level )
	{
		strName = name;
		lLevel	= level;
	}

	//operator를 넣어줘야 하는뎅 아까 실패했잖아 ㅡㅡ
	CGuildBattleNode& operator = ( const CGuildBattleNode& node )
	{
		strName = node.strName;
		lLevel	= node.lLevel;
		return *this;
	}

	string	strName;
	INT32	lLevel;
};

typedef std::list< CGuildBattleNode >		GuildBattleNodeList;
typedef GuildBattleNodeList::iterator		GuildBattleNodeListItr;

// ----------------- CGuildBattleUnit ------------------
class CGuildBattleUnit
{
public:
	CGuildBattleUnit( INT32 lMax = GUILD_BATTLE_MEMBER_MAX );
	~CGuildBattleUnit();

	void	Clear();

	BOOL	Insert( const string& strName, INT32 lLevel );
	BOOL	Delete( const string& strName );
	BOOL	Delete( INT32 nPos );
	void	DeleteAll();
	BOOL	Swap( INT32 nPos, BOOL bUp = TRUE );
	BOOL	Swap( const string& strName, BOOL bUp = TRUE );
	INT32	Size()					{	return (INT32)m_listBattleChar.size();	}

	INT32	GetPos( const string& strName );
	GuildBattleNodeListItr	GetItr( INT32 nPos );

	GuildBattleNodeList&	GetStringList()		{	return m_listBattleChar;	}

private:
	INT32				m_lListMax;
	GuildBattleNodeList		m_listBattleChar;
};

// ----------------- AgcmUIGuildBattleMember ------------------
class AgcmUIGuild;
class AgcmUIGuildBattleMember : public CGuildBattleUnit, public CUIOpenCloseSystem
{
public:
	enum eOpenType {
		eOpenRequest	= 0,
		eOpenAccept		= 1,
		eOpenMax
	};

public:
	AgcmUIGuildBattleMember();
	~AgcmUIGuildBattleMember();

	BOOL	AddModule( AgcmUIManager2* pUIMgr, AgcmUIGuild* pUIGuild );

	static BOOL CBBattleListUp(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBBattleListDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBBattleListRandomSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBBattleListDelete(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBBattleListDeleteAll(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBBattleListDone(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBBattleListCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBBattleListSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBDisplayGuildBattleListSelect( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl );
	static BOOL CBDisplayGuildBattleMemberNumber( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL CBDisplayGuildBattleMemberName( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL CBDisplayGuildBattleMemberLevel( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL CBDisplayGuildBattleMemberMax( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	BOOL	Open( AgpdGuild* pGuild );
	BOOL	PushList( CHAR* szMember, INT32 lLevel );
	void	RefreshList();
	void	SetOpenType( eOpenType eType )	{	m_eOpenType = eType;	}

	//Util Function
	char*	GetSelectMember();
	void	SetRandomGuildMember();

private:
	AgcmUIManager2*		m_pcsAgcmUIManager2;
	AgcmUIGuild*		m_pcsAgcmUIGuild;

	eOpenType			m_eOpenType;
	AgcdUIUserData*		m_pBattleMemberList;
	INT32				m_aBattleMemberListIndex[GUILD_BATTLE_MEMBER_MAX];
	AgcdUIUserData*		m_pBattleMemberMax;
	INT32				m_lBattleMemberMax;
	INT32				m_lSelectMember;
};