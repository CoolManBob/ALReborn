#pragma once

#include "AgcmUIManager2.h"
#include "AgcmUIControl.h"
#include "AgcmGuild.h"
#include <list>
#include <string>
#include <algorithm>

enum eGuildBattleReadyMode
{
	eGuildBattleReadyRequest,
	eGuildBattleReadyAccept,
	eGuildBattleReadyMax,
};

extern char* g_szGuildBattleTime[eGuildBattleTimeModeMax];
extern char* g_szBattleTotalMan[eGuildBattleTotalModeMax];
extern char* g_szBattlePrivateMan[eGuildBattlePrivateModeMax];

// ----------------- AgcmUIGuildBattleOffer ------------------
class AgcmUIGuild;
class AgcmUIGuildBattleOffer : public CUIOpenCloseSystem
{
public:
	AgcmUIGuildBattleOffer();
	~AgcmUIGuildBattleOffer();

	BOOL	AddModule( AgcmUIManager2* pUIMgr, AgcmUIGuild* pUIGuild );

	void	Update();

	static BOOL CBGuildBattleType(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildBattleTime(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBGuildBattleMan(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBBattleRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBUpdateComboBattlePerson(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsBattlePerson(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	static BOOL CBMessageBattleRequestEnd(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );

	void	Clear();
	BOOL	IsGuildBattle();

	BOOL	Open( INT32 nType, UINT32 nTime, UINT32 nPerson, INT32 lEvent, BOOL bReceive = FALSE );
	BOOL	ComboClear();
	void	BattleStringClear();

	BOOL	IsPossiblePerson( INT32 lType, UINT32 ulPerson );
	
	INT32	GetBattleType()					{	return m_nBattleType;		}
	CHAR*	GetBattleTypeString()			{	return m_szBattleType;		}
	CHAR*	GetBattleTypeString( int nType ){	return m_szBattleTypeArray[nType];	}
	void	SetBattleType( INT32 nType );

	UINT32	GetBattleTime()					{	return m_nBattleTime;		}
	UINT32	GetBattleTimeSize()				{	return g_nGuildBattleTime[m_nBattleTime] * 60;	}
	CHAR*	GetBattleTimeString()			{	return m_szBattleTime;		}
	void	SetBattleTime( UINT32 nType, BOOL bReceive = FALSE );

	UINT32	GetBattlePerson()				{	return m_nBattlePerson;		}
	UINT32	GetBattlePersonSize();
	CHAR*	GetBattlePersonString()			{	return m_szBattlePerson;	}
	void	SetBattlePerson( UINT32 nType, BOOL bReceive = FALSE );

	BOOL	SendRequest();
	BOOL	SendAccept();
	BOOL	SendReject();

	BOOL	SendCancelRequest();
	
private:
	void	SetString();

public:
	INT32				m_lEventBattleRequestEnd;
	INT32				m_lEventUpdateComboBattlePerson;
	CHAR				m_szBattleTypeArray[eGuildBattleMax][128+1];

private:
	AgcmUIManager2*		m_pcsAgcmUIManager2;
	AgcmUIGuild*		m_pcsAgcmUIGuild;

	
	AgcdUIUserData*		m_pudBattleType;
	AgcdUIUserData*		m_pudBattleTime;
	AgcdUIUserData*		m_pudBattlePerson;

	INT32				m_nBattleType;
	UINT32				m_nBattleTime;
	UINT32				m_nBattlePerson;

	CHAR				m_szBattleType[128+1];
	CHAR				m_szBattleTime[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR				m_szBattlePerson[128+1];
};