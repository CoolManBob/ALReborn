#ifndef _AGSM_BATTLE_GROUND_H
#define _AGSM_BATTLE_GROUND_H

#include <AgsEngine.h>
#include <AgpmBattleGround.h>

#include <AuLua.h>
#include <AuRandomNumber.h>
#include <AgpdEventSpawn.h>
#include <AuXmlParser.h>
#include "AgpmLog.h"
#include <ApmMap.h>

#include <vector>

class AgpmEventSpawn;
class AgsmEventSpawn;
class ApmEventManager;
class AgsmPvP;
class AgsmCharacter;
class AgpmLog;
class AgpmEventBinding;
class AgpmConfig;
class AgsmSystemMessage;
class AgpmSkill;
class AgsmSkill;
class AgsmDeath;
class AgsmServerStatus;
class AgpmEventTeleport;

class AgpdSkill;

enum eBattleGroundState
{
	BATTLEGROUND_STATE_NONE = 0,
	BATTLEGROUND_STATE_NORMAL,
	BATTLEGROUND_STATE_BEFORE,
	BATTLEGROUND_STATE_ING,
	BATTLEGROUND_STATE_AFTER,
};

template<class _Ty>
class CSafeVector : public std::vector<_Ty>
{
	ApCriticalSection m_CriticalSection;

public:
	using std::vector<_Ty>::push_back;

	typedef typename vector<_Ty> _Base;
	typedef typename vector<_Ty>::iterator iterator;

	void push_back(_Ty& _v) 
	{
		AuAutoLock pLock(m_CriticalSection);
		_Base::push_back(_v);
	};

	BOOL erase(iterator __pos)
	{
		AuAutoLock pLock(m_CriticalSection);
		_Base::erase(__pos);
		return true;
	}
};

class AgsmBattleGround : public AgsModule
{
	static AgsmBattleGround*	m_pInstance;
	ApMutualEx m_Mutex;
	ApMutualEx m_MutexAbilityXml;

private:
	UINT32				m_ulClockCount;
	MTRand				m_csRandom;

	AgpmBattleGround*	m_pagpmBattleGround;
	AgpmCharacter*		m_pagpmCharacter;
	AgsmCharacter*		m_pagsmCharacter;
	AgpmEventSpawn*		m_pcsAgpmEventSpawn;
	AgsmEventSpawn*		m_pcsAgsmEventSpawn;
	ApmEventManager*	m_pcsApmEventManager;
	AgpmConfig*			m_pcsAgpmConfig;
	AgsmPvP*			m_pagsmPvP;
	AgpmLog*			m_pagpmLog;
	AgpmEventBinding*	m_pagpmEventBinding;
	AgsmSystemMessage*	m_pagsmSystemMessage;
	AgpmSkill*			m_pagpmSkill;
	AgsmSkill*			m_pagsmSkill;
	AgsmDeath*			m_pagsmDeath;
	AgsmServerStatus*	m_pagsmServerStatus;
	AgpmEventTeleport*	m_pagpmEventTeleport;

	ApmMap*				m_papmMap;
	
	lua_State*			pLuaState;
	CLuaStreamPack		pluaPack;
	eBattleGroundState	m_pBattleGroundState;
	
	AuXmlDocument		m_XmlData;
	AuXmlNode*			m_pAbilityNode;
	AuXmlNode*			m_pSpawnListNode;
	
	struct _CURRENT_SPAWN
	{
		std::string strSpawnGroupName;
		AuXmlElement* pElem;

		_CURRENT_SPAWN()
			: strSpawnGroupName(), pElem(0)
		{
		}
	};

	struct _ADDSKILL
	{
		INT32 SkillTID;
		INT32 SkillTID_RaceCast;
	};

	struct _BATTLEGROUND_RESULT
	{
		_ADDSKILL Winner;
		_ADDSKILL Looser;
	};

	_BATTLEGROUND_RESULT m_BattleGroundResult;

	typedef CSafeVector<_CURRENT_SPAWN> CSpawnVector;
	CSpawnVector m_pSpawnVector;
	CSpawnVector m_pBossVector;

	//////////////////////////////////////////////////////////////////////////
	// 어빌리티 몬스터 통제 시스템~
	typedef CSafeVector<INT32> ACharacterVector;
	ACharacterVector	m_AMonsterIndex;
	BOOL				m_bIsEventBattleGround;

	AuXmlElement* FindSkillCondition(AuXmlNode* node, string strGroup, int TID);
	BOOL AbilityMonsterPush(AgpdCharacter* pcsCharacter, AgpdSkill *pcsSkill);
	void AbilityMonsterRemove(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	// ~어빌리티 몬스터 통제 시스템

public:
	AgsmBattleGround();
	virtual ~AgsmBattleGround();

	static AgsmBattleGround* GetInstance()
	{
		return AgsmBattleGround::m_pInstance;
	}

	AuXmlNode* GetEpicNoticeNode();

	BOOL OnDead(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	BOOL CheckControlSkill(AgpdCharacter* pcsCharacter, AgpdSkill *pcsSkill);
	BOOL ProcessCharismaPoint( AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget );
	void SetBattleGroundTime(BOOL bUse, INT32 nDay, INT32 nHour, INT32 nMin, INT32 nDuring, BOOL bEvent);
	int GetSpawnGroupCharCount(char* strGroupName);

	eBattleGroundState GetBattleGroundState();
	BOOL OnTimer(UINT32 ulClockCount);

	BOOL IsEventBattleGroud();

	BOOL WriteBattleGroundLog(eAGPDLOGTYPE_ETC eType, AgpdCharacter *pcsKillCharacter, AgpdCharacter *pcsTarget);
	BOOL SendBattleGroundNotice(CHAR* szNotice, INT32 lNoticeType);
	BOOL SendBattleGroundNotice(AgpdCharacter *pcsCharacter, CHAR* szNotice, INT32 lNoticeType, CHAR* szDirectNotice = NULL);

	static BOOL CBGetBattleGroundState(PVOID pData, PVOID pClass, PVOID pCustData);

private:
	BOOL OnInit();
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	AuXmlNode* GetNoticeNode();

	typedef map< AuRace, INT32 > CRaceMap;

	BOOL ProcessSpawn(char *pstrGroupName);
	static int GetBattleGroundState(lua_State* L);
	static int SetBattleGroundState(lua_State* L);
	static int BattleGroundNotice(lua_State* L);
	static int RemoveMonsters(lua_State* L);
	static int SpawnAbilityMonster(lua_State* L);
	static int SpawnCharacters(lua_State* L);
	static int BattleGroundStatistic(lua_State* L);
};

#endif // _AGSM_BATTLE_GROUND_H