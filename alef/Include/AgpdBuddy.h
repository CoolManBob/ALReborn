#pragma once

#include "ApBase.h"
#include "AgpdCharacter.h"

const INT32 AGPD_BUDDY_MAX				= 30;

typedef enum _eAgsmBuddyMentorStatus
{
	AGSMBUDDY_MENTOR_NONE		= 0,
	AGSMBUDDY_MENTOR_MENTOR		= 1,
	AGSMBUDDY_MENTOR_MENTEE		= 2,
}_eAgsmBuddyMentorStatus;

class AgpdBuddyElement
{
private:
	DWORD	m_dwOptions;

	enum EnumBuddyOption
	{
		// 1 ~ 19 : 옵션 설정, 20 ~ 24 클라이언트 데이터, 25 ~ 31 : 기타 데이터
		BUDDY_OPTION_NONE			= 0,
		BUDDY_OPTION_WHISPER		= (1 << 0),		// 귓속말
		BUDDY_OPTION_TRADE			= (1 << 1),		// 1:1 거래
		BUDDY_OPTION_INVITATION		= (1 << 2),		// 초대

		BUDDY_OPTION_ONLINE			= (1 << 20),	// Online 여부

		BUDDY_OPTION_FRIEND			= (1 << 25),	// 친구인가? 0:거부리스트, 1:친구리스트
	};

	void SetOption(EnumBuddyOption eOption, BOOL bSet)
	{
		if (bSet)
			m_dwOptions |= eOption;
		else
			m_dwOptions &= ~eOption;
	}

	BOOL GetOption(EnumBuddyOption eOption)
	{
		return (m_dwOptions & eOption);
	}

public:
	ApString<AGPDCHARACTER_NAME_LENGTH> m_szName;
	INT16	m_nMentorStatus;

public:
	AgpdBuddyElement()
	{
		m_dwOptions = 0;
		m_nMentorStatus = 0;
	}

	void Initialize()
	{
		m_dwOptions = 0;
		m_nMentorStatus = 0;
		m_szName.Clear();
		
		SetOnline(FALSE);
	}

	bool operator == (TCHAR* szName)
	{
		return (COMPARE_EQUAL == m_szName.CompareNoCase(szName));
	}

	BOOL SetValues(DWORD dwOptions, const TCHAR* szName)	
	{
		m_dwOptions = dwOptions;
		return m_szName.SetText(szName);
	}

	DWORD GetOptions()					{return m_dwOptions;}
	void SetOptions(DWORD dwOptions)	{m_dwOptions = dwOptions;}

	void SetBlockWhisper(BOOL bSet)		{SetOption(BUDDY_OPTION_WHISPER, bSet);}
	void SetBlockTrade(BOOL bSet)		{SetOption(BUDDY_OPTION_TRADE, bSet);}
	void SetBlockInvitation(BOOL bSet)	{SetOption(BUDDY_OPTION_INVITATION, bSet);}
	void SetFriend(BOOL bSet)			{SetOption(BUDDY_OPTION_FRIEND, bSet);}
	void SetOnline(BOOL bSet)			{SetOption(BUDDY_OPTION_ONLINE, bSet);}

	BOOL IsBlockWhisper()				{return GetOption(BUDDY_OPTION_WHISPER);}
	BOOL IsBlockTrade()					{return GetOption(BUDDY_OPTION_TRADE);}
	BOOL IsBlockInvitation()			{return GetOption(BUDDY_OPTION_INVITATION);}

	BOOL IsFriend()						{return GetOption(BUDDY_OPTION_FRIEND);}
	BOOL IsOnline()						{return GetOption(BUDDY_OPTION_ONLINE);}

	INT16 GetMentorStatus()				{return m_nMentorStatus;}
	void SetMentorStatus(INT16 MentorStatus){m_nMentorStatus = MentorStatus;}

};

struct AgpdBuddyRegion
{
	AgpdBuddyElement	&m_BuddyElement;
	INT32				m_lRegionIndex;

	AgpdBuddyRegion(AgpdBuddyElement &rBuddyElement, INT32 lRegionIndex) :
	m_BuddyElement(rBuddyElement), m_lRegionIndex(lRegionIndex) {}
};

typedef ApVector<AgpdBuddyElement, AGPD_BUDDY_MAX> ApVectorBuddy;

struct AgpdBuddyADChar
{
	ApVectorBuddy	m_vtFriend;
	ApVectorBuddy	m_vtBan;
	INT32			m_nCurrentMenteeNumber;

	ApString<AGPDCHARACTER_NAME_LENGTH> m_szLastRequestBuddy;
};

struct AgpdBuddyConstrict
{
	INT32			m_nMaxMenteeNumber;
	INT32			m_nLimitMentorLevel;
	INT32			m_nLimitMenteeLevel;

	AgpdBuddyConstrict()
	{
		m_nMaxMenteeNumber = 0;
		m_nLimitMentorLevel = 200;
		m_nLimitMenteeLevel = 0;
	}

	AgpdBuddyConstrict(INT32 MaxMenteeNumber, INT32 LimitMentorLevel, INT32 LimitMenteeLevel)
	{
		m_nMaxMenteeNumber = MaxMenteeNumber;
		m_nLimitMentorLevel = LimitMentorLevel;
		m_nLimitMenteeLevel = LimitMenteeLevel;
	}
};