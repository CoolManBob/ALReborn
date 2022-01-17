#ifndef	_AGPDEVENTSPAWN_H_
#define _AGPDEVENTSPAWN_H_

#include "ApBase.h"
#include "AgpmCharacter.h"
#include "ApmEventManager.h"

#define AGPDSPAWN_MAX_CHAR_NUM		10
#define AGPDSPAWN_MAX_ITEM_NUM		10

#define	AGPDSPAWN_MAX_SPAWNED_CHAR	100

#define AGPDSPAWN_SPAWN_NAME_LENGTH	32

typedef struct AgpdSpawnGroup	AgpdSpawnGroup;

typedef enum
{
	AGPDSPAWN_SIEGEWAR_TYPE_NORMAL				= 0,
	AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_OBJECT,
	AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_NPC_GUILD,
	AGPDSPAWN_ARCHLORD_TYPE_SECRET_DUNGEON,
	AGPDSPAWN_ARCHLORD_TYPE_DEKAIN				= 4,
	AGPDSPAWN_BATTLEGROUND_NORMAL				= 10,
	AGPDSPAWN_BATTLEGROUND_ABILITY				= 11,
} AgpdSpawnSiegeWarType;

typedef struct
{
	INT32					m_lTID;							// Spawn될 Character Template ID
	INT32					m_lAITID;							// Spawn될 Character가 사용할 AI ID
	INT32					m_lSpawnRate;						// Spawn Rate 확률 (0 ~ 100000)
	INT32					m_lMaxMobCount;
} AgpdSpawnConfig;

typedef struct
{
	INT32					m_lID;							// Spawn될 Character Template ID
	CTime					m_ulRespawnTime;
} AgpdSpawnInfo;

class AgpdSpawn
{
	AgpdSpawn();
	virtual ~AgpdSpawn();

public:
	ApMutualEx			m_Mutex;
	AgpdSpawnGroup*		m_pstParent;
	ApdEvent*			m_pstNext;
	BOOL				m_bActive;
	ApdEventAttachData	m_stEvent;

	CHAR				m_szName[AGPDSPAWN_SPAWN_NAME_LENGTH];
	INT32				m_lGroupID;
	INT32				m_lSpawnInterval;
	AgpdSpawnSiegeWarType	m_lSiegeWarType;
	AgpdSpawnConfig		m_stSpawnConfig[AGPDSPAWN_MAX_CHAR_NUM];
	
	INT32				m_lTotalCharacter;
	AgpdSpawnInfo		m_stSpawnCharacters[AGPDSPAWN_MAX_SPAWNED_CHAR];
};

struct AgpdSpawnGroup
{
	CHAR		m_szName[AGPDSPAWN_SPAWN_NAME_LENGTH];
	INT32		m_lID;

	ApdEvent *	m_pstChild;
};

typedef struct
{
	ApdEvent *	m_pstEvent;
	AgpdSpawn *	m_pstSpawn;
} AgpdSpawnADChar;

// 2006.09.28. steeple
typedef struct _AgpdSpawnCustomData
{
	INT32		m_lTID;
	INT32		m_lCount;
	AuPOS		m_stPos;
	ApBase*		m_pBoss;
	BOOL		m_bAdmin;
} AgpdSpawnCustomData;

#endif //_AGPDEVENTSPAWN_H_
