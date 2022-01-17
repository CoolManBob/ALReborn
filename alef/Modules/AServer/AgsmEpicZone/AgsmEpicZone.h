#ifndef _AGSM_EPIC_ZONE_H
#define _AGSM_EPIC_ZONE_H

#include <AgsEngine.h>
#include <AuXmlParser.h>
#include <AuLua.h>
#include <AgsmBattleGround.h>

#include <map>

#define EPICBOSSZONEINTERVAL	300000

enum eEpicBossZoneState
{
	EPICBOSSZONE_STATE_NONE = 0,
	EPICBOSSZONE_STATE_START,
	EPICBOSSZONE_STATE_ING,
	EPICBOSSZONE_STATE_END,
};

struct _CURRENT_SPAWN
{
	std::string strSpawnGroupName;
	AuXmlElement* pElem;
			
	_CURRENT_SPAWN()
		: strSpawnGroupName(), pElem(0)
	{
	}
};

typedef vector<_CURRENT_SPAWN> CSpawnVector;
typedef vector<AgpdCharacter*> EntryVector;
struct EpicBossZone
{
	CHAR				m_szLuaFileName[MAX_PATH+1];
	UINT32				m_ulPlayTime;
	UINT32				m_ulStartTime;
	UINT32				m_ulEndTime;
	UINT16				m_ulBossZoneID;
	UINT16				m_ulRegionIndex;
	eEpicBossZoneState	m_eBossZoneState;

	CSpawnVector		m_pSpawnVector;
	CSpawnVector		m_pBossVector;

	EpicBossZone()
		: m_ulPlayTime(0), m_ulStartTime(0), m_ulBossZoneID(0), m_ulRegionIndex(0), m_ulEndTime(0)
	{
		ZeroMemory(m_szLuaFileName, sizeof(m_szLuaFileName));
		m_eBossZoneState = EPICBOSSZONE_STATE_NONE;

		m_pSpawnVector.clear();
		m_pBossVector.clear();
	}
};

//////////////////////////////////////////////////////////////////////
// typedef
typedef map<INT32, EpicBossZone>	EpicBossZoneMap;
typedef pair<INT32, EpicBossZone>	EpicBossZonePair;
typedef EpicBossZoneMap::iterator	EpicBossZoneIter;

class AgpmEventSpawn;
class AgsmEventSpawn;
class ApmEventManager;
class AgsmBattleGround;
class AgpmCharacter;
class AgpmEpicZone;
class AgsmCharacter;
class ApmMap;
class AgpmParty;

class AgsmEpicZone : public AgsModule
{
	static AgsmEpicZone*	m_pInstance;
	ApMutualEx				m_Mutex;

private:
	UINT32					m_ulClockCount;

	AgpmEventSpawn*			m_pcsAgpmEventSpawn;
	ApmEventManager*		m_pcsApmEventManager;
	AgsmEventSpawn*			m_pcsAgsmEventSpawn;
	AgsmBattleGround*		m_pcsAgsmBattleGroud;
	AgpmCharacter*			m_pcsAgpmCharacter;
	AgsmCharacter*			m_pcsAgsmCharacter;
	AgpmEpicZone*			m_pcsAgpmEpicZone;
	ApmMap*					m_pcsApmMap;
	AgpmParty*				m_pcsAgpmParty;
			
	lua_State*				m_pLuaState;
	CLuaStreamPack			m_pLuaPack;

	AuXmlDocument			m_XmlData;
	AuXmlNode*				m_pLuaFileNode;
	AuXmlNode*				m_pSpawnListNode;

	EpicBossZoneMap			m_mapBossZone;

private:
	BOOL	OnInit();

public:
	AgsmEpicZone();
	virtual ~AgsmEpicZone(void);

	static AgsmEpicZone* GetInstance()
	{
		return m_pInstance;
	}

	BOOL		LoadXml();
	BOOL		OnTimer(UINT32 ulClockCount);

	BOOL		SetEpicBossZoneStartTime(UINT32 lBossZoneID, UINT32 lClockCount);	
	BOOL		IsEnableEntrance(UINT32 lBossZoneID);
	BOOL		IsEpicBossZoneTimeOver(UINT32 lBossZoneID);
	BOOL		InitEpicBossZone(UINT32 lBossZoneID);
	BOOL		LoadSpawnData(EpicBossZone *pstEpicBossZone);
	BOOL		LoadSpawnData(INT32 lBossZoneID);
	BOOL		ProcessSpawn(CHAR *pstrGroupName);
	BOOL		RemoveMonster(INT32 lBossZoneID);
	BOOL		ReturnToEntry(INT32 lBossZoneID, FLOAT PosX, FLOAT PosZ);
	BOOL		OnDeadProcess(AgpdCharacter *pcsDeadCharacter, AgpdCharacter *pcsAttackCharacter=NULL);
	BOOL		SpawnEpicZoneMonster(INT32 lBossZoneID);
	BOOL		RemoveMonsterFromSpawnGroup(CHAR* szSpawnGroupName, INT32 lRegionIndex);
//	BOOL		RegisterEntry(INT32 lBossZoneID, AgpdCharacter* pcsPartyLeader);
	BOOL		SendEpicZoneNotice(CHAR* szNotice, INT32 lNoticeType, EpicBossZone *pstEpicBossZone, CHAR* szDirectNotice = NULL);

	INT32		GetRemainTime(INT32 lBossZoneID);
	BOOL		SendRemainTime(EpicBossZone *pstEpicBossZone, INT32 lRemainTime);

	////////////////////////////////////////////////////////////////////////////

private:
	BOOL LuaRegister(lua_State* pLuaState);

	static int GetEpicBossZoneState(lua_State* L);
	static int SetEpicBossZoneState(lua_State* L);
	static int IsEpicBossZoneTimeOver(lua_State* L);
	static int SpawnEpicZoneMonster(lua_State* L);
	static int RemoveMonster(lua_State* L);
	static int ReturnToEntry(lua_State* L);
		
};

#endif