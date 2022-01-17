// AgpdPvP.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2004. 12. 16.

#ifndef _AGPDPVP_H_
#define _AGPDPVP_H_

#include "ApBase.h"
#include "AgpdCharacter.h"
#include "AgpdGuild.h"

#define AGPMPVP_MAX_FRIEND_COUNT			50				// 그냥 내 맘대루 정함
#define AGPMPVP_MAX_ENEMY_COUNT				50				// 그냥 내 맘대루 정함

typedef enum _eAgpdPvPMode
{
	AGPDPVP_MODE_NONE = 0,
	AGPDPVP_MODE_SAFE,
	AGPDPVP_MODE_FREE,
	AGPDPVP_MODE_COMBAT,
	AGPDPVP_MODE_BATTLE,			// 얘는 Character Attached Data 엔 들어가지 않고, 각 PvPInfo 에만 들어간다. 2005.04.17. steeple
} eAgpdPvPMode;

typedef enum _eAgpdPvPStatus
{
	AGPDPVP_STATUS_NONE = 0x00,
	AGPDPVP_STATUS_NOWPVP = 0x01,
	AGPDPVP_STATUS_INVINCIBLE =0x02,
} eAgpdPvPStatus;

class AgpmPvPArray {
private:
	ApSafeArray<INT32, AGPMPVP_MAX_FRIEND_COUNT>	m_csArrayID;
	CHAR	m_csArrayName[AGPMPVP_MAX_FRIEND_COUNT][AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];

	ApSafeArray<PVOID, AGPMPVP_MAX_FRIEND_COUNT>	m_csArrayData;

	INT32	lObjectCount;

public:
	AgpmPvPArray();
	virtual ~AgpmPvPArray();

	BOOL	Initialize();

	PVOID	AddObject(PVOID pObject, INT32 nKey);
	PVOID	AddObject(PVOID pObject, CHAR *szName);

	BOOL	RemoveObject(INT32 nKey);
	BOOL	RemoveObject(CHAR *szName);

	PVOID	GetObject(INT32 nKey);
	PVOID	GetObject(CHAR *szName);

	PVOID	GetObjectSequence(INT32* plIndex);

	INT32	GetObjectCount();
};

//////////////////////////////////////////////////////////////////////////
// Enemy Info
struct AgpdPvPCharInfo
{
	INT32 m_lCID;				// Target CID
	eAgpdPvPMode m_ePvPMode;	// Target 과의 PvPMode
	UINT32 m_ulLastCombatClock;	// 마지막으로 아무나 상대를 때린 시간

	bool operator == (INT32 lCID)
	{
		if(m_lCID == lCID)
			return true;

		return false;
	}

	AgpdPvPCharInfo() : m_lCID(0), m_ePvPMode(AGPDPVP_MODE_NONE), m_ulLastCombatClock(0) {;}
};

struct AgpdPvPGuildInfo
{
	CHAR m_szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	eAgpdPvPMode m_ePvPMode;
	UINT32 m_ulLastCombatClock;

	bool operator == (const CHAR* szGuildID)
	{
		if(!szGuildID)
			return false;

		return !_tcscmp(m_szGuildID, szGuildID);
	}

	AgpdPvPGuildInfo() : m_ePvPMode(AGPDPVP_MODE_NONE), m_ulLastCombatClock(0)
	{
		memset(m_szGuildID, 0, sizeof(m_szGuildID));
	}
};

typedef std::vector<AgpdPvPCharInfo>				PvPCharVector;
typedef std::vector<AgpdPvPCharInfo>::iterator		PvPCharIter;
typedef std::vector<AgpdPvPGuildInfo>				PvPGuildVector;
typedef std::vector<AgpdPvPGuildInfo>::iterator		PvPGuildIter;

//////////////////////////////////////////////////////////////////////////
// Attach Character Data
typedef struct _stAgpdPvPADChar
{
	BOOL m_bInit;

	INT32 m_lWin;
	INT32 m_lLose;

	eAgpdPvPMode m_ePvPMode;		// 현재 어떤 PvP 모드인지 저장한다. 주로 지역에 의해 좌우된다.
	INT8 m_cPvPStatus;				// 현재 PvP 상태를 저장한다. Bit 연산 해아한다.

	BOOL m_bDeadInCombatArea;		// Combat Area 안에서 죽었다. 고로 부활하게 됨. 모두 리셋해야하는 경우

	/*
	ApAdmin* m_pFriendList;
	ApAdmin* m_pEnemyList;

	ApAdmin* m_pFriendGuildList;	// 길드는 따로 관리한다.
	ApAdmin* m_pEnemyGuildList;		// 길드는 따로 관리한다.
	*/

	//AgpmPvPArray*	m_pFriendList;
	//AgpmPvPArray*	m_pEnemyList;

	//AgpmPvPArray*	m_pFriendGuildList;
	//AgpmPvPArray*	m_pEnemyGuildList;

	PvPCharVector*	m_pFriendVector;
	PvPCharVector*	m_pEnemyVector;

	PvPGuildVector* m_pFriendGuildVector;
	PvPGuildVector* m_pEnemyGuildVector;

	INT8 m_cDeadType;

	_stAgpdPvPADChar()
		:m_pFriendVector(NULL)
		,m_pEnemyVector(NULL)
		,m_pFriendGuildVector(NULL)
		,m_pEnemyGuildVector(NULL)
	{}
} AgpdPvPADChar;

//////////////////////////////////////////////////////////////////////////
// Item Drop Probabiliy
#define AGPDPVP_DROP_KIND_LENGTH		32

typedef struct _stAgpdPvPAreaDrop
{
	CHAR m_szKind[AGPDPVP_DROP_KIND_LENGTH+1];
	eAgpdPvPMode m_ePvPMode;
	INT16 m_nAreaProbability;
	INT16 m_nEquipProbability;
	INT16 m_nInvenProbability;
} AgpdPvPAreaDrop;

typedef struct _stAgpdPvPItemDrop
{
	CHAR m_szKind[AGPDPVP_DROP_KIND_LENGTH+1];
	INT32 m_lIndex;
	INT16 m_nEquipProbability;
	INT16 m_nInvenProbability;
} AgpdPvPItemDrop;

typedef struct _stAgpdPvPSkullDrop
{
	INT32 m_lLevelGap;
	INT32 m_lSkullLevel;
	INT32 m_lSkullProbability;
} AgpdPvPSkullDrop;

//////////////////////////////////////////////////////////////////////////
// System Message
typedef struct _stAgpdPvPSystemMessage
{
	INT32 m_lCode;
	CHAR* m_aszData[2];
	INT32 m_alData[2];
} AgpdPvPSystemMessage;

#endif //_AGPDPVP_H_
