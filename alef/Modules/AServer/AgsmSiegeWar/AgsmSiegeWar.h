#pragma once

#include "AgpmConfig.h"
#include "AgpmSiegeWar.h"
#include "AgpmGuild.h"
#include "AgpmPvP.h"
#include "AgpmEventBinding.h"

#include "AgsmDeath.h"
#include "AgsmAI2.h"
#include "AgsmEventSpawn.h"
#include "AgsmAdmin.h"
#include "AgpmArchlord.h"
#include "AgsmArchlord.h"
#include "AgsmGuild.h"
#include "AgsmRide.h"

#include "AgsdBuddy.h"					// for rowset... but informal

#define	AGSMSIEGEWAR_DB_UPDATE_INTERVAL			60000

typedef enum _AttackType {
	AGSMSIEGEWAR_ATTACK_TYPE_RANGE			= 0,
	AGSMSIEGEWAR_ATTACK_TYPE_MELEE,
	AGSMSIEGEWAR_ATTACK_TYPE_SIEGE_WEAPON,
	AGSMSIEGEWAR_ATTACK_TYPE_NONE,
} AttackType;

typedef enum {
	AGSMSIEGEWAR_CB_DB_UPDATE_CASTLE	= 0,
	AGSMSIEGEWAR_CB_DB_SELECT_CASTLE,
	AGSMSIEGEWAR_CB_DB_SELECT_CASTLE_END,
	AGSMSIEGEWAR_CB_DB_INSERT_SIEGE,
	AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE,
	AGSMSIEGEWAR_CB_DB_SELECT_SIEGE,
	AGSMSIEGEWAR_CB_DB_INSERT_SIEGE_APPLICATION,
	AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_APPLICATION,
	AGSMSIEGEWAR_CB_DB_SELECT_SIEGE_APPLICATION,
	AGSMSIEGEWAR_CB_DB_INSERT_SIEGE_OBJECT,
	AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_OBJECT,
	AGSMSIEGEWAR_CB_DB_DELETE_SIEGE_OBJECT,
	AGSMSIEGEWAR_CB_DB_SELECT_SIEGE_OBJECT,
} AgsmSiegeWarCBID;


class AgsmSiegeWar : public AgsModule
{
public:
	AgsmSiegeWar();
	virtual ~AgsmSiegeWar();

	BOOL	OnAddModule();
	BOOL	OnIdle(UINT32 ulClockCount);

	HANDLE	m_hEventDBLoad;
	BOOL	m_bLoaded;

	UINT32	m_ulLoadedClockCount;
	UINT32	m_ulTickCount4SiegeWarDay;
	UINT32	m_ulLatestConnOfArchlord;

private:
	AgpmConfig			*m_pcsAgpmConfig;
	ApmEventManager		*m_pcsApmEventManager;
	ApmMap				*m_pcsApmMap;
	AgpmFactors			*m_pcsAgpmFactors;
	AgsmFactors			*m_pcsAgsmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgsmCharacter		*m_pcsAgsmCharacter;
	AgpmSiegeWar		*m_pcsAgpmSiegeWar;
	AgpmGuild			*m_pcsAgpmGuild;
	AgpmItem			*m_pcsAgpmItem;
	AgpmSkill			*m_pcsAgpmSkill;
	AgpmPvP				*m_pcsAgpmPvP;
	AgsmCombat			*m_pcsAgsmCombat;
	AgsmDeath			*m_pcsAgsmDeath;
	AgpmEventSpawn		*m_pcsAgpmEventSpawn;
	AgsmEventSpawn		*m_pcsAgsmEventSpawn;
	AgsmAOIFilter		*m_pcsAgsmAOIFilter;
	AgsmSkill			*m_pcsAgsmSkill;
	AgsmAI2				*m_pcsAgsmAI2;
	AgsmCharManager		*m_pcsAgsmCharManager;
	AgsmItem			*m_pcsAgsmItem;
	AgsmInterServerLink	*m_pcsAgsmInterServerLink;
	AgsmServerManager2	*m_pcsAgsmServerManager;
	AgsmAdmin			*m_pcsAgsmAdmin;
	AgpmEventBinding	*m_pcsAgpmEventBinding;
	AgsmArchlord		*m_pcsAgsmArchlord;
	AgpmArchlord		*m_pcsAgpmArchlord;
	AgsmGuild			*m_pcsAgsmGuild;
	AgsmRide			*m_pcsAgsmRide;

	UINT32				m_aulLastDBUpdateTime[AGPMSIEGEWAR_MAX_CASTLE];
	
	BOOL	ProcessStatusStart(AgpdSiegeWar *pcsSiegeWarData);
	BOOL	ProcessStatusBreakASeal(AgpdSiegeWar *pcsSiegeWarData);
	BOOL	ProcessStatusOpenEyes(AgpdSiegeWar *pcsSiegeWarData);
	BOOL	ProcessStatusTimeOver(AgpdSiegeWar *pcsSiegeWarData);
	BOOL	ProcessStatusArchlordBattle(AgpdSiegeWar *pcsSiegeWarData);
	BOOL	ProcessStatusTimeOverArchlordBattle(AgpdSiegeWar *pcsSiegeWarData);

	BOOL	SpawnSiegeWarMonster(AgpdSiegeWar *pcsSiegeWarData, const AgpdSpawnSiegeWarType eSpawnSiegeWarType);
	BOOL	SpawnSiegeWarMonsterDirect(AgpdSiegeWar *pcsSiegeWarData, const AgpdSpawnSiegeWarType eSpawnSiegeWarType);
	BOOL	RemoveSiegeWarMonster(AgpdSiegeWar *pcsSiegeWarData, const AgpdSpawnSiegeWarType eSpawnSiegeWarType);
	BOOL	SpawnBossMob(AgpdSiegeWar *pcsSiegeWar);
	BOOL	RemoveBossMob(AgpdSiegeWar *pcsSiegeWar);

	static	BOOL CBSpawnSiegeWarMonster(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL CBRemoveSiegeWarMonster(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	ActiveObject(AgpdSiegeWar* pcsSiegeWarData);
	static	BOOL CBActiveObject(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	ActiveLifeTower(AgpdCharacter *pcsCharacter);
	BOOL	ActiveAttackResurrectionTower(AgpdCharacter *pcsCharacter);
	BOOL	ActiveDefenseResurrectionTower(AgpdCharacter *pcsCharacter);

	BOOL	DisableLifeTower(AgpdCharacter *pcsCharacter);
	BOOL	DisableAttackResurrectionTower(AgpdCharacter *pcsCharacter);
	BOOL	DisableDefenseResurrectionTower(AgpdSiegeWar *pcsSiegeWar);

	BOOL	SendResult(AgpdSiegeWar *pcsSiegeWar, AgpdCharacter *pcsCharacter, AgpmSiegeResult eResult);
	BOOL	SendResponseUseAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, AgpmSiegeResult eResult, BOOL bSendObjectInfo = FALSE);
	BOOL	SendResponseRepairAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, AgpmSiegeResult eResult);

	BOOL	ConfirmSiegeWarTime(AgpdSiegeWar *pcsSiegeWar);
	BOOL	ConfirmGuild(AgpdSiegeWar *pcsSiegeWar);

	AttackType	GetAttackType(AgpdCharacter	*pcsAttacker);
	AttackType	GetAttackType(AgpdSkill *pcsSkill);

	//BOOL	UseAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget);

	ApAdmin	m_acsSiegeWarRegionAdmin[AGPMSIEGEWAR_MAX_CASTLE];

public:
	static BOOL CBServerConnect(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBUpdateSpecialStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBDeath(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBUpdateSiegeWarStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUpdateCarveAGuild(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSetNewOwner(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBSetNextSiegeWarTime(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAddDefenseApplication(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRemoveDefenseApplication(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAddAttackApplication(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRemoveAttackApplication(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRequestAttackApplGuildList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRequestDefenseApplGuildList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAddAttackGuild(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAddDefenseGuild(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRequestAttackGuildList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRequestDefenseGuildList(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBReceiveOpenAttackObject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveUseAttackObject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveRepairAttackObject(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBNormalDamageAdjustSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSkillDamageAdjustSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBSpawnCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBIsFriendGuild(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBIsEnemyGuild(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUseAttackObject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBActionMoveCarveASeal(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAddItemToAttackObject(PVOID pData, PVOID pClass, PVOID pCustData);
	//static BOOL CBRemoveItemToAttackObject(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBGetTarget(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSetCharacterGameData(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCheckLoginPosition(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCheckReturnPosition(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBCarveASeal(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBCharacterCancelCarving(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBItemCancelCarving(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAddCharacterToMap(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveCharacterFromMap(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBSendPacketCharacterView(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSendPacketCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBStartSiegeWarCommand(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEndSiegeWarCommand(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEndArchlordBattleCommand(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBPayActionCost(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateReverseAttack(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBStartArchlordSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEndArchlordSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSetCastleOwner(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCancelCastleOwner(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAddArchlordCastleAttackGuild(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSetArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	ProcessCarveASeal(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget);

	BOOL	ProclaimSiegeWar(ApmMap::RegionTemplate *pcsRegionTemplate);
	BOOL	ReadySiegeWar(ApmMap::RegionTemplate *pcsRegionTemplate);
	BOOL	StartSiegeWar(ApmMap::RegionTemplate *pcsRegionTemplate, BOOL bCheckCondition = TRUE);
	BOOL	BreakASeal(ApmMap::RegionTemplate *pcsRegionTemplate);
	BOOL	OpenEyes(ApmMap::RegionTemplate *pcsRegionTemplate, AgpdCharacter *pcsCharacter);
	BOOL	CarveASeal(AgpdCharacter *pcsCharacter);
	BOOL	TimeOver(INT32 lSiegeWarInfoIndex);
	BOOL	ResetSiegeWar(AgpdSiegeWar *pcsSiegeWar);

	BOOL	AddSiegeWarObjectChar(AgpdCharacter *pcsCharacter);
	BOOL	RemoveSiegeWarObjectChar(AgpdCharacter *pcsCharacter);

	BOOL	UpdateToDB(INT32 lSiegeWarInfoIndex);
	BOOL	UpdateToDB(AgpdSiegeWar *pcsSiegeWar);
	BOOL	UpdateCastleToDB(AgpdSiegeWar *pcsSiegeWar);
	BOOL	UpdateSiegeToDB(AgpdSiegeWar *pcsSiegeWar);
	BOOL	UpdateSiegeWarObjectToDB(AgpdSiegeWar *pcsSiegeWar, AgpdCharacter *pcsCharacter, INT32 lObjectIndex);
	BOOL	UpdateAllSiegeWarObjectToDB(AgpdSiegeWar *pcsSiegeWar);
	BOOL	UpdateApplGuildToDB(AgpdSiegeWar *pcsSiegeWar);

	BOOL	LoadFromDB();

	BOOL	OnSelectResultCastle(stBuddyRowset *pRowset, BOOL bEnd = FALSE);
	BOOL	OnSelectResultSiege(stBuddyRowset *pRowset, BOOL bEnd = FALSE);
	BOOL	OnSelectResultSiegeApplication(stBuddyRowset *pRowset, BOOL bEnd = FALSE);
	BOOL	OnSelectResultSiegeObject(stBuddyRowset *pRowset, BOOL bEnd = FALSE);

	BOOL	SetCallbackDBUpdateCastle(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBSelectCastle(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBSelectCastleEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBInsertSiege(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBUpdateSiege(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBSelectSiege(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBInsertSiegeApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBUpdateSiegeApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBSelectSiegeApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBInsertSiegeObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBUpdateSiegeObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBDeleteSiegeObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDBSelectSiegeObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);

public:
	BOOL	SendPacketCastleInfo(AgpdSiegeWar *pcsSiegeWar, UINT32 ulNID);
	BOOL	SendPacketCastleInfoToAll(AgpdSiegeWar *pcsSiegeWar);
	BOOL	SendPacketCarveASeal(AgpdSiegeWar *pcsSiegeWar, AgpmSiegeResult eResult, UINT32 ulNID);

	static BOOL	RepairAttackObject(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);
	static BOOL	TeleportGuildMember(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);
	static BOOL	ResetSiegeWarObject(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);
	static BOOL	TeleportArchlordBattle(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);

	BOOL	SpawnAllSiegeWarObjectExceptIng();
	BOOL	RemoveAllSiegeWarObject(AgpdSiegeWar *pcsSiegeWar);

	BOOL	SetPositionForArchlordBattle();

	BOOL	ProcessNewArchlord(AgpdSiegeWar *pcsSiegeWar);
	BOOL	ProcessFailedArchlordBattle();

	BOOL	ProcessSystemMessageNewArchlord(AgpdSiegeWar* pcsSiegeWar);
	BOOL	ProcessSystemMessageFailedArchlordBattle(AgpdSiegeWar* pcsSiegeWar);

	BOOL	CancelCarving(AgpdCharacter *pcsCharacter);

	BOOL	SendPacketToSiegeWar(PVOID pvPacket, INT16 nPacketLength, AgpdSiegeWar *pcsSiegeWar, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4);
	BOOL	SendPacketStatusInfo(AgpmSiegeStatusType eStatusType, AgpdSiegeWar *pcsSiegeWar, CHAR *pszGuildName, CHAR *pszGuildMasterName, AgpdSiegeWarMonsterType eMonsterType, UINT32 ulTimeSec, UINT8 ucActiveCount);

	BOOL	SendAddAttackApplicationResult(AgpdSiegeWar *pcsSiegeWar, AgpdCharacter *pcsCharacter);
	
	BOOL	SyncStartTime(AgpdSiegeWar *pcsSiegeWar, UINT64 ullCurrentTimeDate);
	BOOL	SyncEndTime(AgpdSiegeWar *pcsSiegeWar, UINT32 ulClockCount);

	BOOL	SetTestServerNextSiegeWarTime(AgpdSiegeWar *pcsSiegeWar);
	BOOL	SetNextSiegeWarTime(AgpdSiegeWar *pcsSiegeWar);
	BOOL	SetNextArchlordSiegeWarTime(AgpdSiegeWar *pcsSiegeWar);

	BOOL	SetNotOwnerGuildTargetPosition(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos);

	BOOL	MoveAttackGuildMember(AgpdSiegeWar *pcsSiegeWar);
	BOOL	MoveDefenseGuildMember(AgpdSiegeWar *pcsSiegeWar);
	
	BOOL	CheckExistArchlord(UINT32 ulClockCount);
	BOOL	DepriveArchlord(AgpdSiegeWar *pcsSiegeWar);
};