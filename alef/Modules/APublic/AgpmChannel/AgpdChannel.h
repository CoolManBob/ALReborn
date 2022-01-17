#pragma once

#include "ApAdmin.h"
#include "AgpdCharacter.h"
#include "AgpdGuild.h"
#include <list>

const INT32 AGPDCHANNEL_MAX_JOIN	= 5;

const INT32 AGPDCHANNEL_MEMBER_PUBLIC = 15;
const INT32 AGPDCHANNEL_MEMBER_GUILD = 100;
const INT32 AGPDCHANNEL_MEMBER_PRIVATE = 15;

const INT32 AGPDCHANNEL_CHANNEL_REMAINTIME = 5 * 60 * 1000;	// 5분

typedef ApString<40>	ApStrChannelName;
typedef ApString<8>		ApStrChannelPassword;
typedef ApString<120>	ApStrChannelChatMsg;

enum EnumChannelType
{
	AGPDCHANNEL_TYPE_NONE	= 0,
	AGPDCHANNEL_TYPE_PUBLIC,	// 공개 대화방
	AGPDCHANNEL_TYPE_PRIVATE,	// 비공개 대화방
	AGPDCHANNEL_TYPE_GUILD		// 길드 대화방
};

class AgpdChannel
{
private:
	INT32 m_lMaxUser;
	EnumChannelType m_eChannelType;
	UINT32 m_lTickCount;

public:
	ApStrChannelName m_strName;
	ApStrChannelPassword m_strPassword;
	ApCriticalSection m_CriticalSection;
	ApStrGuildName m_strGuildName;

	typedef std::list<ApStrCharacterName>::iterator iterChannelUser;
	std::list<ApStrCharacterName> m_UserList;

public:
	AgpdChannel()
	{
		m_eChannelType = AGPDCHANNEL_TYPE_NONE;
		m_lMaxUser = 0;
		m_lTickCount = 0;
		m_UserList.clear();
	}

	void Initialize(EnumChannelType eChannelType)
	{
		m_eChannelType = eChannelType;

		switch (eChannelType)
		{
		case AGPDCHANNEL_TYPE_PUBLIC:		m_lMaxUser = AGPDCHANNEL_MEMBER_PUBLIC;		break;
		case AGPDCHANNEL_TYPE_PRIVATE:		m_lMaxUser = AGPDCHANNEL_MEMBER_PRIVATE;	break;
		case AGPDCHANNEL_TYPE_GUILD:		m_lMaxUser = AGPDCHANNEL_MEMBER_GUILD;		break;
		default:							ASSERT(!"Invalid Channel Type");
		};
	}

	INT32 GetMaxUser() {return m_lMaxUser;}
	INT32 GetCurrentUser() {return (INT32)m_UserList.size();}
	BOOL IsFull()	{return (GetMaxUser() == GetCurrentUser());}
	EnumChannelType GetChannelType() {return m_eChannelType;};

	BOOL AddUser(AgpdCharacter *pcsCharacter)
	{
		ASSERT(NULL != pcsCharacter);
		ASSERT(AGPDCHANNEL_TYPE_NONE != m_eChannelType);

		AuAutoLock lock(m_CriticalSection);
		if (!lock.Result()) return FALSE;

		if (GetCurrentUser() > GetMaxUser())
			return FALSE;

		if (NULL != FindUser(pcsCharacter->m_szID))
			return FALSE;

		m_UserList.push_back(pcsCharacter->m_szID);

		return TRUE;
	}

	void RemoveUser(AgpdCharacter *pcsCharacter)
	{
		ASSERT(NULL != pcsCharacter);
		ASSERT(AGPDCHANNEL_TYPE_NONE != m_eChannelType);

		AuAutoLock lock(m_CriticalSection);
		if (!lock.Result()) return;

		m_UserList.remove(pcsCharacter->m_szID);
	}

	BOOL FindUser(TCHAR *szName)
	{
		ASSERT(NULL != szName);
		ASSERT(AGPDCHANNEL_TYPE_NONE != m_eChannelType);

		AuAutoLock lock(m_CriticalSection);
		if (!lock.Result()) return FALSE;

		ApString<AGPDCHARACTER_NAME_LENGTH> strName(szName);

		for (iterChannelUser iter = m_UserList.begin(); iter != m_UserList.end(); ++iter)
		{
			if (COMPARE_EQUAL == strName.CompareNoCase(iter->GetBuffer()))
				return TRUE;
		}

		return FALSE;
	}

	void SetTickCount(UINT32 lTickCount)
	{
		m_lTickCount = lTickCount;
	}

	UINT32 GetTickCount() {return m_lTickCount;}
};

// 현재 사용자가 참여하고 있는 방들의 리스트
class AgpdJoinChannelADChar
{
private:
	BOOL m_bCreatedGuildChannel;

public:
	ApVector<ApStrChannelName, AGPDCHANNEL_MAX_JOIN> m_vtJoinRooms;
	typedef ApVector<ApStrChannelName, AGPDCHANNEL_MAX_JOIN>::iterator iterChannelName;
	
public:
	AgpdJoinChannelADChar() : m_bCreatedGuildChannel(FALSE) {}

	BOOL GetCreatedGuildChannel() {return m_bCreatedGuildChannel;}
	void SetCreatedGuildChannel(BOOL bCreated) {m_bCreatedGuildChannel = bCreated;}
};