// AgsdGuild.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 20.

#ifndef _AGSDGUILD_H_
#define _AGSDGUILD_H_

#include "ApBase.h"
#include "AsDefine.h"

class AgsdGuildMember
{
public:
	INT32 m_lServerID;
	UINT32 m_ulNID;

	AgpdCharacter* m_pcsCharacter;	// ulNID 가 가끔 뻘짓해서 캐릭터 포인터를 들고 있는 것으로 변경. 2005.04.27.
};

// 2005.04.17. steeple
// 배틀 상태와 각 시간을 관리한다. 점수 관리는 하지 않는다.
class AgsdGuildBattleProcess
{
public:
	eGuildBattleType	m_eType;
	UINT32				m_ulPerson;

	INT8				m_cStatus;

	UINT32				m_ulLastCheckTime;

	UINT32				m_ulAcceptTime;
	UINT32				m_ulReadyTime;
	UINT32				m_ulStartTime;
	UINT32				m_ulDuration;

	CHAR				m_szGuildID1[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
	CHAR				m_szGuildID2[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
};
typedef list<AgsdGuildBattleProcess*>	GuildBattleList;
typedef GuildBattleList::iterator		GuildBattleListItr;

#endif	//_AGSDGUILD_H_