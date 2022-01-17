#pragma once

#include "AgsEngine.h"
#include "ApDefine.h"
#include "AgpdCharacter.h"
#include "AgpmArchlord.h"
#include <string>
#include "AgsdBuddy.h"					// for rowset... but informal

class AgsmGuild;
class AgpmItem;
class AgpmGrid;
class AgsmItemManager;
class AgpmFactors;
class AgsmAdmin;
class AgpmCharacter;
class AgsmCharacter;
class AgsmCharManager;
class AgpmEventSpawn;
class ApmEventManager;
class AgsmEventSpawn;
class AgpmEventTeleport;
class AgpmSiegeWar;
class AgsmRide;
class AgsmDeath;
class AgpmEventBinding;

const INT32 GUARD_MAX = 5;

const UINT32 AGSMARCHLORD_STEP_DUNGEON_TIME			= 10 * 60 * 1000;		// 아크로드 결정전 준비 시간 10분.
const UINT32 AGSMARCHLORD_STEP_SIEGEWAR_TIME		= 60 * 60 * 1000;		// 랜스피어 각인 시간 1시간.
const UINT32 AGSMARCHLORD_STEP_ARCHLORD_TIME		= 60 * 60 * 1000;		// 아크로드 결정전 시간 1시간.

enum eGuardItemPart
{
	EGIP_BODY = 0,
	EGIP_FOOT,
	EGIP_HAND,
	EGIP_HEAD,
	EGIP_LEGS,

	EGIP_MAX
};

const enum eAGSMARCHLORD_CB
{
	AGSMARCHLORD_CB_DBSELECT_ARCHLORD = 0,
	AGSMARCHLORD_CB_DBUPDATE_ARCHLORD,
	AGSMARCHLORD_CB_DBINSERT_ARCHLORD,
	AGSMARCHLORD_CB_DBSELECT_GUARD,
	AGSMARCHLORD_CB_DBINSERT_GUARD,
	AGSMARCHLORD_CB_DBDELETE_GUARD,
	AGSMARCHLORD_CB_START_ARCHLORD_SIEGEWAR,
	AGSMARCHLORD_CB_END_ARCHLORD_SIEGEWAR,
};

struct AgsdArchlordGuardItem
{
	INT32 ItemTID[EGIP_MAX];
};

class AgsmArchlord : public AgsModule 
{
private:
	typedef vector<string>::iterator vtIterator;

	AgpmArchlord		*m_pcsAgpmArchlord;
	AgsmGuild			*m_pcsAgsmGuild;
	AgpmItem			*m_pcsAgpmItem;
	AgpmGrid			*m_pcsAgpmGrid;
	AgsmItemManager		 *m_pcsAgsmItemManager;
	AgpmFactors			*m_pcsAgpmFactors;
	AgsmAdmin			*m_pcsAgsmAdmin;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmCharManager		*m_pcsAgsmCharManager;
	AgpmEventSpawn		*m_pcsAgpmEventSpawn;
	ApmEventManager		*m_pcsApmEventManager;
	AgsmEventSpawn		*m_pcsAgsmEventSpawn;
	AgpmEventTeleport	*m_pcsAgpmEventTeleport;
	AgpmSiegeWar		*m_pcsAgpmSiegeWar;
	AgsmRide			*m_pcsAgsmRide;
	AgsmDeath			*m_pcsAgsmDeath;
	AgpmEventBinding	*m_pcsAgpmEventBinding;

	AgsdArchlordGuardItem m_GuardItem[AURACE_TYPE_MAX][AUCHARCLASS_TYPE_MAX];
	vector<string>	m_vtGuard;
	ApMutualEx		m_lock;
	UINT32			m_lLastTick;
	UINT32			m_lStepTime;

private:
	BOOL IsValidInventory(AgpdCharacter *pcsCharacter);
	BOOL CreateGuardItem(AgpdCharacter *pcsCharacter);
	BOOL DeleteGuardItem(AgpdCharacter *pcsCharacter);

	void StartArchlord(UINT32 ulClockCorrection = 0);
	void NextStep();
	void ArchlordEnd();

	BOOL TransformArchlord(AgpdCharacter *pcsCharacter);
	BOOL CheckGuard(AgpdCharacter *pcsCharacter);

public:
	AgsmArchlord();
	virtual ~AgsmArchlord();

	void InitGuardItem();

	static BOOL CBSetArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSetGuard(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBCancelGuard(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBGuardInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBArchlordStart(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBArchlordNextStep(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBArchlordEnd(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBTeleportMessageId(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnDestroy();
	virtual BOOL OnIdle(UINT32 ulClockCount);

	BOOL SetGuard(AgpdCharacter *pcsArchlord, AgpdCharacter *pcsTarget);
	BOOL CancelGuard(AgpdCharacter *pcsArchlord, AgpdCharacter *pcsTarget);
	BOOL SetArachlord(AgpdCharacter *pcsCharacter);
	BOOL CancelArchlord(CHAR* szArchlord);
	BOOL GuardInfo(AgpdCharacter *pcsCharacter);

	UINT32 SetStepTime(UINT32 ulTime);

	void IdleStepNone(UINT32 ulClockCount);
	void IdleStepDungeon(UINT32 ulClockCount);
	void IdleStepSiegewar(UINT32 ulClockCount);
	void IdleStepArchlord(UINT32 ulClockCount);

	void ExpelAllUsersFromSecretDungeon();

	BOOL IsArchlordStartDay();

	void SpawnDungeonMonster();
	void RemoveDungeonMonster();
	void SendPacketCurrentStep();
	void SendPacketMessageId(AgpdCharacter *pcsCharacter, AgpmArchlordMessageId eMessageID);
	void SendPacketArchlordID(AgpdCharacter *pcsCharacter);

	static BOOL CBSpawnDungeonMonster(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveDungeonMonster(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCharacterDead(PVOID pData, PVOID pClass, PVOID pCustData);

	// DB Result
	BOOL OnSelectResultArchlord(AgpdCharacter *pcsCharacter, stBuddyRowset *pRowset, BOOL bEnd = FALSE);
	BOOL LoadGuardFromDB();
	BOOL OnSelectResultLordGuard(stBuddyRowset *pRowset, BOOL bEnd = FALSE);
	
	BOOL SetCallbackDBSelectArclord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBUpdateArclord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBInsertArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBSelectLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBInsertLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBDeleteLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackStartArchlordSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pcClass);
	BOOL SetCallbackEndArchlordSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pcClass);
};