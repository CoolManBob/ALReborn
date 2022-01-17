#pragma once

#include "ApBase.h"
#include "AgpmGuild.h"

#define	AGPMSIEGEWAR_MAX_CASTLE_NAME		128
#define AGPMSIEGEWAR_DEFAULT_BEGIN_TIME		21

enum AgpmSiegeWarCastleIndex
{
	AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX		= 0,
	AGPMSIEGEWAR_HUMAN_CASTLE_INDEX			= 1,
	AGPMSIEGEWAR_ORC_CASTLE_INDEX			= 2,
	AGPMSIEGEWAR_MOONELF_CASTLE_INDEX		= 3,
	AGPMSIEGEWAR_DRAGONSCION_CASTLE_INDEX	= 4,
	AGPMSIEGEWAR_MAX_CASTLE					= 5,
};

const int	AGPMSIEGEWAR_MAX_ATTACK_APPLICATION_GUILD		= 500;
const int	AGPMSIEGEWAR_MAX_DEFENSE_APPLICATION_GUILD		= 500;

const int	AGPMSIEGEWAR_GUILD_POINT_MEMBER_COUNT			= 25;
const int	AGPMSIEGEWAR_GUILD_POINT_BATTLE					= 10;
const int	AGPMSIEGEWAR_GUILD_POINT_DURATION				= 15;
const int	AGPMSIEGEWAR_GUILD_POINT_ARCHON_SCROLL_COUNT	= 30;
const int	AGPMSIEGEWAR_GUILD_POINT_MASTER_LEVEL			= 20;

const int	AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER			= 5;

typedef enum {
	AGPMSIEGE_GUILD_POINT_TYPE_NONE							= 0,
	AGPMSIEGE_GUILD_POINT_TYPE_MEMBER_COUNT,
	AGPMSIEGE_GUILD_POINT_TYPE_BATTLE,
	AGPMSIEGE_GUILD_POINT_TYPE_DURATION,
	AGPMSIEGE_GUILD_POINT_TYPE_ARCHON_SCROLL_COUNT,
	AGPMSIEGE_GUILD_POINT_TYPE_MASTER_LEVEL,
	AGPMSIEGE_GUILD_POINT_TYPE_MAX
} AgpmSiegeGuildPointType;

typedef enum _AgpdSiegeWarStatus {
	AGPD_SIEGE_WAR_STATUS_OFF			= 1,
	AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR,
	AGPD_SIEGE_WAR_STATUS_READY,
	AGPD_SIEGE_WAR_STATUS_START,
	AGPD_SIEGE_WAR_STATUS_BREAK_A_SEAL,
	AGPD_SIEGE_WAR_STATUS_OPEN_EYES,
	AGPD_SIEGE_WAR_STATUS_TIME_OVER,
	AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE,
	AGPD_SIEGE_WAR_STATUS_TIME_OVER_ARCHLORD_BATTLE,
	AGPD_SIEGE_WAR_STATUS_MAX,
} AgpdSiegeWarStatus;

typedef enum _AgpdSiegeWarMonsterType {
	AGPD_SIEGE_MONSTER_TYPE_NONE		= 0,
	AGPD_SIEGE_MONSTER_GUARD_TOWER,
	AGPD_SIEGE_MONSTER_INNER_GATE,
	AGPD_SIEGE_MONSTER_OUTER_GATE,
	AGPD_SIEGE_MONSTER_LIFE_TOWER,
	AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER,
	AGPD_SIEGE_MONSTER_THRONE,
	AGPD_SIEGE_MONSTER_ARCHON_EYES,
	AGPD_SIEGE_MONSTER_ARCHON_EYES_TOWER,
	AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER,
	AGPD_SIEGE_MONSTER_CATAPULT,
	AGPD_SIEGE_MONSTER_SUMMON,
	AGPD_SIEGE_MONSTER_TYPE_MAX,
} AgpdSiegeWarMonsterType;

typedef enum {
	AGPD_SIEGE_WAR_TYPE_NONE			= 0,
	AGPD_SIEGE_WAR_TYPE_NORMAL			= 1,
	AGPD_SIEGE_WAR_TYPE_NPC_GUILD,
} AgpdSiegeWarType;

class AgpdSiegeWarADCharTemplate
{
public:
	AgpdSiegeWarMonsterType		m_eSiegeWarMonsterType;

	AgpdSiegeWarADCharTemplate()
	{
		m_eSiegeWarMonsterType	= AGPD_SIEGE_MONSTER_TYPE_NONE;
	}
};

class AgpdSiegeWarEye {
public:
	BOOL				m_bOpenEye;
	UINT32				m_ulOpenTimeMSec;

	AgpdSiegeWarEye()
	{
		m_bOpenEye	= FALSE;
		m_ulOpenTimeMSec	= 0;
	}
};

class AgpdSiegeWarTemplate {
public:
	UINT32				m_ulSiegeWarDurationMSec;
	UINT32				m_ulOpenAllEyesDurationMSec;
	UINT32				m_ulNeedTimeForCarveMSec;

	UINT32				m_ulProclaimTimeHour;			// 다음공성선포마감시한(시간)
	UINT32				m_ulApplicationTimeHour;		// 공성길드선정마감시한(시간)
	UINT32				m_ulSiegeWarIntervalDay;		// 공성간간격(일)
	UINT32				m_ulSiegeWarBeginTime;			// 공성시작시간

	AgpdSiegeWarTemplate()
	{
		m_ulSiegeWarDurationMSec	= 0;
		m_ulOpenAllEyesDurationMSec	= 0;
		m_ulNeedTimeForCarveMSec	= 0;

		m_ulProclaimTimeHour		= 0;
		m_ulApplicationTimeHour		= 0;
		m_ulSiegeWarIntervalDay		= 0;
		m_ulSiegeWarBeginTime		= 26;
	}
};

class AgpdSiegeWar {
public:
	ApString<AGPMSIEGEWAR_MAX_CASTLE_NAME + 1>		m_strCastleName;

	ApmMap::RegionTemplate	*m_apcsRegionTemplate[2];

	ApString<AGPMGUILD_MAX_GUILD_ID_LENGTH + 1>		m_strOwnerGuildName;
	ApString<AGPDCHARACTER_MAX_ID_LENGTH + 1>		m_strOwnerGuildMasterName;
	UINT8				m_ucOwnerGuildMemberCount;

	ApString<AGPMGUILD_MAX_GUILD_ID_LENGTH + 1>		m_strCarveGuildName;

	AgpdSiegeWarTemplate	m_csTemplate;

	UINT32				m_ulSiegeWarStartTimeMSec;

	AgpdSiegeWarType	m_eSiegeWarType;
	AgpdSiegeWarStatus	m_eCurrentStatus;

	AgpdSiegeWarEye		m_csEyeStatus;

	INT32				m_lArrayIndex;

	UINT64				m_ullPrevSiegeWarTimeDate;
	UINT64				m_ullProclaimTimeDate;
	UINT64				m_ullApplicationTimeDate;
	UINT64				m_ullNextSiegeWarTimeDate;
	UINT64				m_ulIdleTimeData;

	ApAdmin				m_csAttackGuild;
	ApAdmin				m_csDefenseGuild;

	ApAdmin				m_csAttackApplicationGuild;
	ApAdmin				m_csDefenseApplicationGuild;

	ApAdmin				m_csSWMonsters;

	AgpdCharacter*		m_apcsObjectChar[AGPD_SIEGE_MONSTER_TYPE_MAX][AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER];
	AgpdCharacter*		m_pcsBossMob;	// 디카인

	ApSort <AgpdGuild *>	m_ApAttackGuildSortList;
	ApSort <AgpdGuild *>	m_ApDefenseGuildSortList;

	BOOL				m_bIsActiveLifeTower;
	BOOL				m_bIsActiveAttackResurrection;
	BOOL				m_bIsActiveDefenseResurrection;

	UINT32				m_ulSiegeDBID;

	BOOL				m_bIsNeedInsertDB;

	BOOL				m_bIsReverseAttack;

	UINT32				m_ulLastSyncStartTimeSec;
	UINT32				m_ulLastSyncEndTimeSec;

public:
	AgpdSiegeWar()
	{
		m_apcsRegionTemplate[0]			= NULL;
		m_apcsRegionTemplate[1]			= NULL;

		m_ucOwnerGuildMemberCount		= 0;

		m_ulSiegeWarStartTimeMSec		= 0;
		m_eSiegeWarType					= AGPD_SIEGE_WAR_TYPE_NONE;
		m_eCurrentStatus				= AGPD_SIEGE_WAR_STATUS_OFF;

		m_lArrayIndex					= 0;

		m_ullPrevSiegeWarTimeDate		= 0;
		m_ullProclaimTimeDate			= 0;
		m_ullApplicationTimeDate		= 0;
		m_ullNextSiegeWarTimeDate		= 0;
		m_ulIdleTimeData				= 0;

		m_bIsNeedInsertDB				= TRUE;

		m_csAttackGuild.InitializeObject(sizeof(AgpdGuild *), AGPMSIEGEWAR_MAX_ATTACK_APPLICATION_GUILD);
		m_csDefenseGuild.InitializeObject(sizeof(AgpdGuild *), AGPMSIEGEWAR_MAX_DEFENSE_APPLICATION_GUILD);

		m_csAttackApplicationGuild.InitializeObject(sizeof(AgpdGuild *), AGPMSIEGEWAR_MAX_ATTACK_APPLICATION_GUILD);
		m_csDefenseApplicationGuild.InitializeObject(sizeof(AgpdGuild *), AGPMSIEGEWAR_MAX_DEFENSE_APPLICATION_GUILD);

		m_csSWMonsters.InitializeObject(sizeof(AgpdCharacter *), 1000);

		ZeroMemory(m_apcsObjectChar, sizeof(m_apcsObjectChar));
		m_pcsBossMob					= NULL;

		m_bIsActiveAttackResurrection	= FALSE;
		m_bIsActiveDefenseResurrection	= FALSE;
		
		m_ulSiegeDBID		= 0;

		m_bIsReverseAttack	= FALSE;

		m_ulLastSyncStartTimeSec	= 0;
		m_ulLastSyncEndTimeSec		= 0;
	}

	~AgpdSiegeWar()
	{
		m_csAttackGuild.RemoveObjectAll();
		m_csDefenseGuild.RemoveObjectAll();

		m_csAttackApplicationGuild.RemoveObjectAll();
		m_csDefenseApplicationGuild.RemoveObjectAll();

		m_csSWMonsters.RemoveObjectAll();
	}

	BOOL IsActiveLifeTower() { return m_bIsActiveLifeTower; };
	BOOL IsActiveAttackResurrection() { return m_bIsActiveAttackResurrection; };
	BOOL IsActiveDefenseResurrection() { return m_bIsActiveDefenseResurrection; };
};

class AgpdGuildAttachData {
public:
	UINT32	m_ulGuildTotalPoint;
	UINT64	m_ullApplDate;
};

class AgpdSiegeWarADCharacter {
public:
	AgpdSiegeWar	*m_pcsSiegeWar;

	AgpdGrid		*m_pcsAttackObjectGrid;
	UINT8			ucRepairCount;
	INT32			lRepairCost;
	UINT8			ucRepairDuration;

	UINT8			m_ucRepairedCount;

	ApString<AGPMGUILD_MAX_GUILD_ID_LENGTH + 1>		m_strCarveGuildName;
	UINT32			m_ulStartCarveTimeMSec;

	BOOL			m_bIsCarving;

	ApString<AGPDCHARACTER_MAX_ID_LENGTH + 1>		m_strUseCharacterName;

	UINT32			m_ulStartRepairTimeMSec;
};

class AgpdSiegeWarObject {
public:
	UINT8		m_ucRepairCount;
	INT32		m_lRepairCost;
	UINT8		m_ucRepairDuration;

	INT32		m_lNeedItemTIDforActive[8];
	INT32		m_lNeedItemCountforActive[8];
};