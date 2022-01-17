#pragma once

#include "AgpmConfig.h"
#include "AgpmCharacter.h"
#include "AgpmGuild.h"
#include "AgpmAdmin.h"
#include "AgpmEventSpawn.h"
#include "AgpmPvP.h"
#include "AgpmEventBinding.h"

#include "AgpdSiegeWar.h"

typedef enum {
	AGPMSIEGEWAR_CB_UPDATE_STATUS	= 0,
	AGPMSIEGEWAR_CB_UPDATE_CARVE_A_GUILD,
	AGPMSIEGEWAR_CB_SET_NEW_OWNER,
	AGPMSIEGEWAR_CB_UPDATE_CASTLE_INFO,
	AGPMSIEGEWAR_CB_SET_NEXT_SIEGEWAR_TIME,
	AGPMSIEGEWAR_CB_ADD_DEFENSE_APPLICATION,
	AGPMSIEGEWAR_CB_REMOVE_DEFENSE_APPLICATION,
	AGPMSIEGEWAR_CB_ADD_ATTACK_APPLICATION,
	AGPMSIEGEWAR_CB_REMOVE_ATTACK_APPLICATION,
	AGPMSIEGEWAR_CB_RECEIVE_RESULT,
	AGPMSIEGEWAR_CB_REQUEST_ATTACK_APPL_GUILD_LIST,
	AGPMSIEGEWAR_CB_REQUEST_DEFENSE_APPL_GUILD_LIST,
	AGPMSIEGEWAR_CB_RECEIVE_ATTACK_APPL_GUILD_LIST,
	AGPMSIEGEWAR_CB_RECEIVE_DEFENSE_APPL_GUILD_LIST,
	AGPMSIEGEWAR_CB_ADD_ATTACK_GUILD,
	AGPMSIEGEWAR_CB_ADD_DEFENSE_GUILD,
	AGPMSIEGEWAR_CB_REQUEST_ATTACK_GUILD_LIST,
	AGPMSIEGEWAR_CB_REQUEST_DEFENSE_GUILD_LIST,
	AGPMSIEGEWAR_CB_RECEIVE_ATTACK_GUILD_LIST,
	AGPMSIEGEWAR_CB_RECEIVE_DEFENSE_GUILD_LIST,
	AGPMSIEGEWAR_CB_RECEIVE_OPEN_ATTACK_OBJECT,
	AGPMSIEGEWAR_CB_RESPONSE_OPEN_ATTACK_OBJECT,
	AGPMSIEGEWAR_CB_RECEIVE_USE_ATTACK_OBJECT,
	AGPMSIEGEWAR_CB_RESPONSE_USE_ATTACK_OBJECT,
	AGPMSIEGEWAR_CB_RECEIVE_REPAIR_ATTACK_OBJECT,
	AGPMSIEGEWAR_CB_RESPONSE_REPAIR_ATTACK_OBJECT,
	AGPMSIEGEWAR_CB_ADD_ITEM_TO_ATTACK_OBJECT,
	AGPMSIEGEWAR_CB_REMOVE_ITEM_TO_ATTACK_OBJECT,
	AGPMSIEGEWAR_CB_ADD_ITEM_RESULT,

	AGPMSIEGEWAR_CB_CARVE_A_SEAL,
	AGPMSIEGEWAR_CB_STATUS_INFO,

	AGPMSIEGEWAR_CB_UPDATE_REVERSE_ATTACK,

	AGPMSIEGEWAR_CB_SYNC_MESSAGE,

	AGPMSIEGEWAR_CB_ADD_ARCHLORD_CASTLE_ATTACK_GUILD,
} AgpmSiegeWarCBID;

const int	AGPMSIEGEWAR_TOTAL_ATTACK_GUILD					= 15;
const int	AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD				= 3;

const int	AGPMSIEGEWAR_GUILD_MEMBER_COUNT_REQUIREMENT		= 30;
const int	AGPMSIEGEWAR_GUILD_CREATE_TIME_REQUIREMENT		= 60 * 60 * 24 * 14;		// 60초 * 60분 * 24시간 * 14일
// const int	AGPMSIEGEWAR_GUILD_MEMBER_COUNT_REQUIREMENT		= 1;						// 1명 테스트용
// const int	AGPMSIEGEWAR_GUILD_CREATE_TIME_REQUIREMENT		= 1;						// 1초 테스트용
const int	AGPMSIEGEWAR_ARCHLORD_2STEP_WAIT_INTERVAL		= 5 * 60 * 1000;			// 5 minutes

const int	AGPMSIEGEWAR_START_WAR_TIME						= 8;

const int	AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE			= 15;
/*const int	AGPMSIEGEWAR_CONFIRMED_GUILD_LIST_MAX_CNT		= (AGPMSIEGEWAR_TOTAL_ATTACK_GUILD > AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD) ?	// 공성이든 수성이든 성 하나당 선정된 길드리스트의 최대 수치
																AGPMSIEGEWAR_TOTAL_ATTACK_GUILD : AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD;		// 현재 30*/

#define	AGPMSIEGEWAR_STREAM_CASTLE_NAME				"Castle"
#define	AGPMSIEGEWAR_STREAM_REGION_NAME				"Region"
#define	AGPMSIEGEWAR_STREAM_SIEGEWAR_PROGRESS_TIME	"SWPlayingTime"
#define	AGPMSIEGEWAR_STREAM_OPEN_ARCHON_EYE_TIME	"ArchonEyeOpenTime"
#define	AGPMSIEGEWAR_STREAM_CARVE_A_THRONE			"StampingTime"
#define	AGSMSIEGEWAR_STREAM_PROCLAIM_TIME			"NextSWDeclaring(Hours)"
#define	AGSMSIEGEWAR_STREAM_APPLICATION_TIME		"SelectGuildForSW(Hours)"
#define	AGSMSIEGEWAR_STREAM_SIEGEWAR_INTERVAL		"Duration4nextSW(Days)"
#define AGSMSIEGEWAR_STREAM_BEGIN_TIME				"SWBeginTime"

#define	AGSMSIEGEWAR_STREAM_MONSTER_TYPE			"타입"
#define	AGSMSIEGEWAR_STREAM_REPAIR_COUNT			"수리횟수"
#define	AGSMSIEGEWAR_STREAM_REPAIR_COST				"수리비용"
#define	AGSMSIEGEWAR_STREAM_REPAIR_DURATION			"수리시간(분)"
#define	AGSMSIEGEWAR_STREAM_NEED_ITEMTID_FOR_ACTIVATE	"필요아이템(TID)"
#define	AGSMSIEGEWAR_STREAM_NEED_ITEMCOUNT_FOR_ACTIVATE	"필요아이템(개수)"

#define	AGPMSIEGEWAR_IMPORT_SIEGEWAR_CHAR_TYPE		"SiegeWarCharType"

typedef enum {
	AGPMSIEGE_OPERATION_UPDATE						= 0,
	AGPMSIEGE_OPERATION_SET_NEXT_SIEGEWAR_TIME,
	AGPMSIEGE_OPERATION_RESULT,
	AGPMSIEGE_OPERATION_DEFENSE_APPLICATION,
	AGPMSIEGE_OPERATION_CANCEL_DEFENSE_APPLICATION,
	AGPMSIEGE_OPERATION_ATTACK_APPLICATION,
	AGPMSIEGE_OPERATION_CANCEL_ATTACK_APPLICATION,
	AGPMSIEGE_OPERATION_REQUEST_ATTACK_APPL_GUILD_LIST,
	AGPMSIEGE_OPERATION_REQUEST_DEFENSE_APPL_GUILD_LIST,
	AGPMSIEGE_OPERATION_ATTACK_APPL_GUILD_LIST,
	AGPMSIEGE_OPERATION_DEFENSE_APPL_GUILD_LIST,
	AGPMSIEGE_OPERATION_SELECT_DEFENSE_GUILD,
	AGPMSIEGE_OPERATION_REQUEST_ATTACK_GUILD_LIST,
	AGPMSIEGE_OPERATION_REQUEST_DEFENSE_GUILD_LIST,
	AGPMSIEGE_OPERATION_ATTACK_GUILD_LIST,
	AGPMSIEGE_OPERATION_DEFENSE_GUILD_LIST,
	AGPMSIEGE_OPERATION_REQUEST_OPEN_ATTACK_OBJECT,
	AGPMSIEGE_OPERATION_RESPONSE_OPEN_ATTACK_OBJECT,
	AGPMSIEGE_OPERATION_REQUEST_USE_ATTACK_OBJECT,
	AGPMSIEGE_OPERATION_RESPONSE_USE_ATTACK_OBJECT,
	AGPMSIEGE_OPERATION_REQUEST_REPAIR_ATTACK_OBJECT,
	AGPMSIEGE_OPERATION_RESPONSE_REPAIR_ATTACK_OBJECT,
	AGPMSIEGE_OPERATION_ADD_ITEM_TO_ATTACK_OBJECT,
	AGPMSIEGE_OPERATION_REMOVE_ITEM_TO_ATTACK_OBJECT,
	AGPMSIEGE_OPERATION_ADD_ITEM_RESULT,
	AGPMSIEGE_OEPRATION_UPDATE_SIEGEWAR_STATUS,
	AGPMSIEGE_OPERATION_UPDATE_ATTACK_OBJECT,

	AGPMSIEGE_OPERATION_CARVE_A_SEAL,
	AGPMSIEGE_OPERATION_STATUS_INFO,

	AGPMSIEGE_OPERATION_MESSAGE,
} AgpmSiegePacketOperation;

typedef enum {
	AGPMSIEGE_RESULT_SUCCESS						= 0,
	AGPMSIEGE_RESULT_FAILED,
	AGPMSIEGE_RESULT_INVALID_TIME,
	AGPMSIEGE_RESULT_NEED_CASTLE_MASTER,
	AGPMSIEGE_RESULT_NEED_GUILD_MASTER,
	AGPMSIEGE_RESULT_NOTENOUGH_GUILD_CONDITION,
	AGPMSIEGE_RESULT_ALREADY_APPLICATION,
	AGPMSIEGE_RESULT_ALREADY_SELECTED,
	AGPMSIEGE_RESULT_DEFENSE_APPL_SUCCESS,
	AGPMSIEGE_RESULT_ATTACK_APPL_SUCCESS,
	AGPMSIEGE_RESULT_SET_DEFENSE_GUILD_SUCCESS,
	AGPMSIEGE_RESULT_SET_NEXT_SIEGE_WAR_TIME_SUCCESS,
	AGPMSIEGE_RESULT_ALREADY_USED_ATTACK_OBJECT,
	AGPMSIEGE_RESULT_ALREADY_REPAIRED_ATTACK_OBJECT,
	AGPMSIEGE_RESULT_OPEN_ATTACK_OBJECT_EVENT,
	AGPMSIEGE_RESULT_NOTENOUGH_MONEY,
	AGPMSIEGE_RESULT_START_CARVE_A_SEAL,
	AGPMSIEGE_RESULT_CANCEL_CARVE_A_SEAL,
	AGPMSIEGE_RESULT_END_CARVE_A_SEAL,
	AGPMSIEGE_RESULT_ALREADY_ACTIVE_OBJECT,
	AGPMSIEGE_RESULT_NO_MORE_REPAIR,
	AGPMSIEGE_RESULT_REGISTRATION_EXCEEDED,
} AgpmSiegeResult;

typedef enum {
	AGPMSIEGE_STATUS_START							= 1,
	AGPMSIEGE_STATUS_REMAIN_TIME_TO_START,
	AGPMSIEGE_STATUS_REMAIN_TIME_TO_TERMINATE,
	AGPMSIEGE_STATUS_DESTROY_OBJECT,
	AGPMSIEGE_STATUS_RELEASE_THRONE,
	AGPMSIEGE_STATUS_ACTIVE_ARCHON_EYE,
	AGPMSIEGE_STATUS_FAILED_ACTIVE_ARCHON_EYE,
	AGPMSIEGE_STATUS_START_CARVE_GUILD,
	AGPMSIEGE_STATUS_CANCEL_CARVE_GUILD,
	AGPMSIEGE_STATUS_NEW_CARVE_GUILD,
	AGPMSIEGE_STATUS_TERMINATE,
	AGPMSIEGE_STATUS_ACTIVE_LEFT_ATTACK_RES_TOWER,
	AGPMSIEGE_STATUS_ACTIVE_RIGHT_ATTACK_RES_TOWER,
	AGPMSIEGE_STATUS_DISABLE_LEFT_ATTACK_RES_TOWER,
	AGPMSIEGE_STATUS_DISABLE_RIGHT_ATTACK_RES_TOWER,
	AGPMSIEGE_STATUS_REPAIR_START_LEFT_ATTACK_RES_TOWER,
	AGPMSIEGE_STATUS_REPAIR_START_RIGHT_ATTACK_RES_TOWER,
	AGPMSIEGE_STATUS_REPAIR_END_LEFT_ATTACK_RES_TOWER,
	AGPMSIEGE_STATUS_REPAIR_END_RIGHT_ATTACK_RES_TOWER,
} AgpmSiegeStatusType;

typedef enum {
	AGPMSIEGE_MESSAGE_SYNC_START_TIME				= 1,
	AGPMSIEGE_MESSAGE_SYNC_END_TIME,
} AgpmSiegeMessage;

class AgpmSiegeWar : public ApModule
{
public:
	AgpmSiegeWar();
	virtual ~AgpmSiegeWar();

	BOOL	OnAddModule();

private:
	ApmMap				*m_pcsApmMap;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmGuild			*m_pcsAgpmGuild;
	AgpmAdmin			*m_pcsAgpmAdmin;
	AgpmEventSpawn		*m_pcsAgpmEventSpawn;
	AgpmGrid			*m_pcsAgpmGrid;
	AgpmItem			*m_pcsAgpmItem;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmPvP				*m_pcsAgpmPvP;
	AgpmEventBinding	*m_pcsAgpmEventBinding;

	AgpmConfig			*m_pcsAgpmConfig;

	INT32				m_lIndexADCharacter;
	INT32				m_lIndexADCharTemplate;
	INT32				m_lIndexADGuild;
	
	BOOL				m_bIsDayOfSiegeWar;

public:
	ApMutualEx			m_Mutex;

	AgpdSiegeWar		m_csSiegeWarInfo[AGPMSIEGEWAR_MAX_CASTLE];


// 패킷 관련 함수들
public:
	AuPacket			m_csPacket;
	AuPacket			m_csPacketCastleInfo;

	AuPacket			m_csPacketGuildList;
	AuPacket			m_csPacketGuildInfo;

	AuPacket			m_csPacketAttackObjectInfo;

	AuPacket			m_csPacketStatusInfo;
	AuPacket			m_csPacketSyncMessage;

public:
	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL				OnOperationUpdate(PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg);
	BOOL				OnOperationSetNextSiegeWarTime(INT32 lCID, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg);

	BOOL				OnOperationDefenseApplication(INT32 lCID, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg);
	BOOL				OnOperationCancelDefenseApplication(INT32 lCID, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg);

	BOOL				OnOperationAttackApplication(INT32 lCID, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg);
	BOOL				OnOperationCancelAttackApplication(INT32 lCID, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg);

	BOOL				OnOperationResult(INT8 cResult, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg);

	BOOL				OnOperationRequestAttackApplGuildList(PVOID pvPacketCastleInfo, INT32 lCID, PVOID pvPacketGuildList);
	BOOL				OnOperationRequestDefenseApplGuildList(PVOID pvPacketCastleInfo, INT32 lCID, PVOID pvPacketGuildList);
	BOOL				OnOperationAttackApplGuildList(PVOID pvPacketGuildList);
	BOOL				OnOperationDefenseApplGuildList(PVOID pvPacketGuildList);

	BOOL				OnOperationSelectDefenseGuild(INT32 lCID, PVOID pvPacketCastleInfo);

	BOOL				OnOperationRequestAttackGuildList(PVOID pvPacketCastleInfo, INT32 lCID);
	BOOL				OnOperationRequestDefenseGuildList(PVOID pvPacketCastleInfo, INT32 lCID);
	BOOL				OnOperationAttackGuildList(PVOID pvPacketCastleInfo, PVOID pvPacketGuildList);
	BOOL				OnOperationDefenseGuildList(PVOID pvPacketCastleInfo, PVOID pvPacketGuildList);

	BOOL				OnOperationUpdateAttackObject(INT32 lCID, PVOID pvPacketAttackObjectInfo);

	BOOL				OnOperationRequestOpenAttackObject(INT32 lCID, INT32 lTargetCID);
	BOOL				OnOperationResponseOpenAttackObject(INT32 lCID, INT32 lTargetCID, INT8 cResult, PVOID pvPacketAttackObjectInfo);

	BOOL				OnOperationRequestUseAttackObject(INT32 lCID, INT32 lTargetCID);
	BOOL				OnOperationResponseUseAttackObject(INT32 lCID, INT32 lTargetCID, INT8 cResult, PVOID pvPacketAttackObjectInfo);

	BOOL				OnOperationRequestRepairAttackObject(INT32 lCID, INT32 lTargetCID);
	BOOL				OnOperationResponseRepairAttackObject(INT32 lCID, INT32 lTargetCID, INT8 cResult);

	BOOL				OnOperationAddItemToAttackObject(INT32 lCID, INT32 lTargetCID, INT32 lItemID);
	BOOL				OnOperationAddItemResult(INT32 lCID, INT32 lTargetCID, INT32 lItemID);

	BOOL				OnOperationRemoveItemToAttackObject(INT32 lCID, INT32 lTargetCID, INT32 lItemID);

	BOOL				OnOperationCarveASeal(INT32 lCID, INT32 lTargetCID);

	BOOL				OnOperationStatusInfo(PVOID pvPacketStatusInfo);

	BOOL				OnOperationMessage(PVOID pvPacketSyncMessage);

	PVOID				MakePacketCastleInfo(AgpdSiegeWar *pcsSiegeWar, INT16 *pnPacketLength);

	PVOID				MakePacketCastleInfoPacket(AgpdSiegeWar *pcsSiegeWar);

	PVOID				MakePacketCarveGuildInfo(AgpdSiegeWar *pcsSiegeWar, INT16 *pnPacketLength);

	PVOID				MakePacketSetNextSiegeWarTime(AgpdSiegeWar *pcsSiegeWar, UINT64 ullNextSetTime, INT32 lCID, INT16 *pnPacketLength);

	PVOID				MakePacketResult(AgpdSiegeWar *pcsSiegeWar, AgpmSiegeResult eResult, INT16 *pnPacketLength);

	PVOID				MakePacketSiegeWarStatus(AgpdSiegeWar *pcsSiegeWar, INT16 *pnPacketLength);

	PVOID				MakePacketDefenseApplication(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength);
	PVOID				MakePacketCancelDefenseApplication(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength);

	PVOID				MakePacketAttackApplication(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength);
	PVOID				MakePacketCancelAttackApplication(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength);

	PVOID				MakePacketRequestAttackApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 nPage, INT16 *pnPacketLength);
	PVOID				MakePacketRequestDefenseApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 nPage, INT16 *pnPacketLength);

	PVOID				MakePacketAttackApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 nPage, INT16 *pnPacketLength);
	PVOID				MakePacketDefenseApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 nPage, INT16 *pnPacketLength);

	PVOID				MakePacketRequestAttackGuildList(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength);
	PVOID				MakePacketRequestDefenseGuildList(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength);

	PVOID				MakePacketAttackGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 nPage, INT16 *pnPacketLength);
	PVOID				MakePacketDefenseGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 *pnPacketLength);

	PVOID				MakePacketGuildInfo(AgpdGuild *pcsGuild);

	PVOID				MakePacketSelectDefenseGuild(AgpdSiegeWar *pcsSiegeWar, CHAR *szGuildName, INT32 lCID, INT16 *pnPacketLength);

	PVOID				MakePacketRequestOpenAttackObject(INT32 lCID, INT32 lTargetCID, INT16 *pnPacketLength);

	PVOID				MakePacketRequestUseAttackObject(INT32 lCID, INT32 lTargetCID, INT16 *pnPacketLength);
	PVOID				MakePacketResponseUseAttackObject(INT32 lCID, INT32 lTargetCID, AgpmSiegeResult eResult, UINT8 ucRepairCount, UINT8 ucRepairedCount, INT32 lRepairCost, UINT8 ucRepairDuration, UINT8 ucRepairElapsed, CHAR *pszUserName, AgpdGrid *pcsGrid, INT16 *pnPacketLength);

	PVOID				MakePacketRequestRepairAttackObject(INT32 lCID, INT32 lTargetCID, INT16 *pnPacketLength);

	PVOID				MakePacketAddItemToAttackObject(INT32 lCID, INT32 lTargetCID, INT32 lItemID, INT16 *pnPacketLength);
	PVOID				MakePacketAddItemResult(INT32 lCID, INT32 lTargetCID, INT32 lItemID, INT16 *pnPacketLength);

	PVOID				MakePacketRemoveItemToAttackObject(INT32 lCID, INT32 lTargetCID, INT32 lItemID, INT16 *pnPacketLength);

	PVOID				MakePacketCarveASeal(INT32 lCID, INT32 lTargetCID, INT16 *pnPacketLength);

	PVOID				MakePacketStatusInfo(AgpmSiegeStatusType eStatusType, AgpdSiegeWar *pcsSiegeWar, CHAR *pszGuildName, CHAR *pszGuildMasterName, AgpdSiegeWarMonsterType eMonsterType, UINT32 ulTimeSec, UINT8 ucActiveCount, INT16 *pnPacketLength);

	PVOID				MakePacketUpdateAttackObject(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, BOOL bSendGridInfo = TRUE);

	PVOID				MakePacketUpdateReverseAttack(AgpdSiegeWar *pcsSiegeWar, INT16 *pnPacketLength);

	PVOID				MakePacketSyncMessage(AgpmSiegeMessage eMessageType, UINT16 unTimeSec, INT16 *pnPacketLength);

public:
	BOOL						SetCurrentStatus(INT32 lSiegeWarInfoIndex, AgpdSiegeWarStatus eStatus, BOOL bCheckCondition = TRUE);
	AgpdSiegeWarStatus			GetCurrentStatus(INT32 lSiegeWarInfoIndex);

	BOOL						IsGuildSiegeWarEntry( const CHAR* szGuildID );	//	길드가 렌스피어공성 & 공성 중인지 확인

	AgpdSiegeWarADCharacter*	GetAttachCharacterData(AgpdCharacter *pcsCharacter);
	AgpdSiegeWarADCharTemplate*	GetAttachTemplateData(AgpdCharacterTemplate *pcsCharacterTemplate);
	AgpdGuildAttachData*		GetAttachGuildData(AgpdGuild *pcsGuild);

	AgpdSiegeWarMonsterType		GetSiegeWarMonsterType(AgpdCharacter *pcsCharacter);
	AgpdSiegeWarMonsterType		GetSiegeWarMonsterType(AgpdCharacterTemplate *pcsCharacterTemplate);

	BOOL						SetSiegeWarMonsterType(AgpdCharacterTemplate *pcsCharacterTemplate, AgpdSiegeWarMonsterType eType);

	AgpdSiegeWarType			GetCurrentSiegeWarType(INT32 lSiegeWarInfoIndex);
	VOID						SetCurrentSiegeWarType(INT32 lSiegeWarInfoIndex, AgpdSiegeWarType eSiegeWarType);

	INT32						GetSiegeWarInfoIndex(AuPOS stPos);
	INT32						GetSiegeWarInfoIndex(ApmMap::RegionTemplate *pcsRegionTemplate);
	AgpdSiegeWar*				GetSiegeWarInfo(INT32 lSiegeWarInfoIndex);
	AgpdSiegeWar*				GetSiegeWarInfo(CHAR *pszCastleName);
	AgpdSiegeWar*				GetSiegeWarInfo(AgpdCharacter *pcsCharacter);
	AgpdSiegeWar*				GetSiegeWarByOwner(CHAR *szOwnerID);

	AgpdSiegeWar*				GetSiegeWarInfoOfGuild(AgpdGuild *pcsGuild, BOOL *pbOffense = NULL);
	AgpdSiegeWar*				GetSiegeWarInfoOfCharacter(AgpdCharacter *pcsCharacter, BOOL *pbOffense = NULL);

	AgpdSiegeWar*				GetSiegeWarInfo(PVOID pvPacketCastleInfo);

	BOOL						UpdateCarveAGuild(INT32 lSiegeWarInfoIndex, AgpdGuild *pcsGuild);
	BOOL						SetNewOwner(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsGuild);

	BOOL						ReadSiegeInfo(CHAR *pszFileName, BOOL bDecryption);
	static BOOL					CharacterImportDataReadCB(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL						IsRespawnMonster(AgpdCharacter* pcsCharacter);

	static BOOL					DesCharAttachData(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL					CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL					CBCheckNPCAttackableTarget(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL					CBIsAttackableTime(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL					CBIsInSiegeWarIngArea(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL					CBCheckDestroy(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL					CBDestroyGuild(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL					CBIsStaticCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL						ClearAttackGuild(AgpdSiegeWar *pcsSiegeWar);
	BOOL						ClearDefenseGuild(AgpdSiegeWar *pcsSiegeWar);
	BOOL						ClearAttackApplGuild(AgpdSiegeWar *pcsSiegeWar);
	BOOL						ClearDefenseApplGuild(AgpdSiegeWar *pcsSiegeWar);
	AgpmSiegeResult				SetAttackGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsAttackGuild, BOOL bCheckCondition = TRUE);
	AgpmSiegeResult				SetDefenseGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsDefenseGuild, BOOL bCheckCondition = TRUE);

	BOOL						IsCastleOwner(AgpdSiegeWar *pcsSiegeWar, CHAR *szCharID);
	BOOL						IsAlreadyApplGuild(AgpdGuild *pcsGuild, BOOL bIsArchlordCastle = FALSE);
	BOOL						IsResurrectableInCastle(AgpdCharacter *pcsCharacter);
	BOOL						IsInSecretDungeon(AgpdCharacter* pcsCharacter);
	BOOL						IsCastleOwnerGuildMember(AgpdCharacter* pcsCharacter);
	BOOL						IsThisRegionCastleOwnerGuildMember(AgpdCharacter* pcsCharacter);
	BOOL						IsThisRegionCastleInSiegeWar(AgpdCharacter* pcsCharacter);

	AgpdCharacter*				GetArchlord();
	
	void						SetDayOfSiegeWar();
	BOOL						IsDayOfSiegeWar();

public:
	BOOL	SetCallbackUpdateStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateCarveAGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackSetNewOwner(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateCastleInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackSetNextSiegeWarTime(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackAddDefenseApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveDefenseApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackAddAttackApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveAttackApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReceiveResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRequestAttackApplGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRequestDefenseApplGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReceiveAttackApplGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReceiveDefenseApplGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackAddAttackGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackAddDefenseGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackRequestAttackGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRequestDefenseGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReceiveAttackGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReceiveDefenseGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackReceiveOpenAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackResponseOpenAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackReceiveUseAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackResponseUseAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackReceiveRepairAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackResponseRepairAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackRemoveItemToAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackAddItemToAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackAddItemResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackCarveASeal(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackStatusInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackUpdateReverseAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackSyncMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackAddArchlordCastleAttackGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);

private:
	BOOL	SetStatusOff(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition = TRUE);
	BOOL	SetStatusProclaimWar(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition = TRUE);
	BOOL	SetStatusReady(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition = TRUE);
	BOOL	SetStatusStart(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition = TRUE);
	BOOL	SetStatusBreakASeal(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition = TRUE);
	BOOL	SetStatusOpenEyes(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition = TRUE);
	BOOL	SetStatusTimeOver(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition = TRUE);
	BOOL	SetStatusArchlordBattle(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition = TRUE);
	BOOL	SetStatusTimeOverArchlordBattle(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition = TRUE);

	BOOL	AddItemToAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, AgpdItem *pcsItem);
	BOOL	RemoveItemToAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, AgpdItem *pcsItem);

public:
// 공성 신청 관련 내용
	BOOL	CheckApplicationRequirement(AgpdCharacter *pcsCharacter);
	BOOL	CheckGuildRequirement(AgpdGuild *pcsGuild);

	BOOL	SortAttackGuild(AgpdSiegeWar *pcsSiegeWar);
	BOOL	SortDefenseGuild(AgpdSiegeWar *pcsSiegeWar);

	BOOL	IsValidSiegeWarTime(AgpdSiegeWar *pcsSiegeWar, UINT64 ullNextSiegeWarTimeDate);
	AgpmSiegeResult	SetNextSiegeWarTime(AgpdSiegeWar *pcsSiegeWar, UINT64 ullNextSiegeWarTimeDate);
	UINT64	GetNextSiegeWarFirstTime(AgpdSiegeWar *pcsSiegeWar);
	UINT64	GetFirstSiegeWarTime(AgpdSiegeWar *pcsSiegeWar);


	AgpmSiegeResult	AddDefenseApplicationGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsGuild);
	AgpmSiegeResult	RemoveDefenseApplicationGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsGuild);
	AgpmSiegeResult	AddAttackApplicationGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsGuild);
	AgpmSiegeResult	RemoveAttackApplicationGuild(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsGuild);

public:
	struct CompareGuildMemberDesc
	{
		template < class _T_ >
		bool operator()(const _T_& a, const _T_& b)
		{
			return (a->m_pMemberList->GetObjectCount() > b->m_pMemberList->GetObjectCount());
		};
	};
	struct CompareGuildWarPointDesc
	{
		template < class _T_ >
		bool operator()(const _T_& a, const _T_& b)
		{
			return (GetGuildWarPoint(a) > GetGuildWarPoint(b));
		};
	};
	struct CompareGuildDurationDesc
	{
		template < class _T_ >
		bool operator()(const _T_& a, const _T_& b)
		{
			return (GetGuildDuration(a) > GetGuildDuration(b));
		};
	};
	struct CompareGuildArchonDesc
	{
		template < class _T_ >
		bool operator()(const _T_& a, const _T_& b)
		{
			return (GetGuildArchon(a) > GetGuildArchon(b));
		};
	};
	struct CompareGuildMasterLevelDesc
	{
		template < class _T_ >
		bool operator()(const _T_& a, const _T_& b)
		{
			return (GetGuildMasterLevel(a) > GetGuildMasterLevel(b));
		};
	};
	struct CompareGuildTotalPointDesc
	{
		template < class _T_ >
		bool operator()(const _T_& a, const _T_& b)
		{
			return (GetGuildTotalPoint(a) > GetGuildTotalPoint(b));
		};
	};

	static UINT32	GetGuildWarPoint(AgpdGuild *pcsGuild);
	static UINT32	GetGuildDuration(AgpdGuild *pcsGuild);
	static UINT32	GetGuildArchon(AgpdGuild *pcsGuild);
	static UINT32	GetGuildMasterLevel(AgpdGuild *pcsGuild);
	static UINT32	GetGuildTotalPoint(AgpdGuild *pcsGuild);

	INT32	GetGuildPoint(AgpmSiegeGuildPointType eGuildPointType, AgpdGuild *pcsGuild);
	BOOL	IsSamePoint(AgpmSiegeGuildPointType eGuildPointType, AgpdGuild *pcsGuildA, AgpdGuild *pcsGuildB);

	BOOL	CalcGuilPoint(AgpmSiegeGuildPointType eGuildPointType, INT32 lMaxPoint, ApSort <AgpdGuild *> *pcsSortList);

	BOOL	ResetAttackGuildTotalPoint(AgpdSiegeWar *pcsSiegeWar);
	BOOL	ResetDefenseGuildTotalPoint(AgpdSiegeWar *pcsSiegeWar);
	BOOL	ResetGuildTotalPoint(ApAdmin *pcsAdmin);

	AgpdSiegeWar*	GetSiegeWar(AgpdCharacter *pcsCharacter);
	BOOL			SetSiegeWar(AgpdCharacter *pcsCharacter, AgpdSiegeWar *pcsSiegeWar);

	BOOL			IsAttackGuild(AgpdCharacter *pcsCharacter, AgpdSiegeWar *pcsSiegeWar);
	BOOL			IsDefenseGuild(AgpdCharacter *pcsCharacter, AgpdSiegeWar *pcsSiegeWar);
	BOOL			IsOwnerGuild(AgpdCharacter *pcsCharacter, AgpdSiegeWar *pcsSiegeWar);

	AgpdSiegeWar*	IsOwnerGuild(const char* guildID);

	AgpmSiegeResult	CheckOpenAttackObjectEvent(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget);
	AgpmSiegeResult	CheckValidUseAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget);
	AgpmSiegeResult	CheckValidRepairAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget);

	BOOL			CheckStartCondition();

	BOOL			CheckNeedItemForUse(AgpdCharacter *pcsCharacter);
	BOOL			PayCostForUse(AgpdCharacter *pcsCharacter);

	UINT8			GetRepairCount(AgpdCharacter *pcsCharacter);
	INT32			GetRepairCost(AgpdCharacter *pcsCharacter);
	UINT8			GetRepairDuration(AgpdCharacter *pcsCharacter);

	BOOL			SetRepairStartTime(AgpdCharacter *pcsCharacter);

public:
	AgpdSiegeWarObject	m_acsSiegeWarObjectInfo[AGPD_SIEGE_MONSTER_TYPE_MAX];

public:
	BOOL			ReadSiegeWarObjectInfo(CHAR *pszFileName, BOOL bDecryption);

	BOOL			CheckActiveCondition(AgpdCharacter *pcsCharacter);
	BOOL			IsSiegeWarMonster(AgpdCharacter *pcsCharacter);

	BOOL			CheckCarveASeal(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsThrone);
	BOOL			StartCarveASeal(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsThrone);
	BOOL			CancelCarveASeal(AgpdCharacter *pcsThrone);
	BOOL			EndCarveASeal(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsThrone);

	BOOL			SetUseCharacterName(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsUser);

	BOOL			IsDoingSiegeWar(AgpdCharacter* pcsCharacter);

	BOOL			UpdateReverseAttack(AgpdSiegeWar *pcsSiegeWar, BOOL bIsReverseAttack);

	BOOL			IsStarted(AgpdSiegeWar *pcsSiegeWar)
	{
		if (!pcsSiegeWar)
			return FALSE;

		if (pcsSiegeWar->m_eCurrentStatus < AGPD_SIEGE_WAR_STATUS_START ||
			pcsSiegeWar->m_eCurrentStatus >= AGPD_SIEGE_WAR_STATUS_MAX)
			return FALSE;

		if (IsArchlordCastle(pcsSiegeWar) && pcsSiegeWar->m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_TIME_OVER_ARCHLORD_BATTLE)
			return FALSE;

		return TRUE;
	};

	BOOL			IsArchlordCastle(AgpdSiegeWar *pcsSiegeWar)
	{
		if (!pcsSiegeWar)
			return FALSE;

		if (pcsSiegeWar->m_lArrayIndex == AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX)
			return TRUE;

		return FALSE;
	};

	BOOL			IsInArchlordCastle(AgpdCharacter* pcsCharacter)
	{
		AgpdSiegeWar* pcsSiegeWar = GetSiegeWarInfo(pcsCharacter);
		if(pcsSiegeWar && pcsSiegeWar->m_lArrayIndex == AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX)
			return TRUE;

		return FALSE;
	}

	AgpdSiegeWar	*GetArchlordCastle()
	{
		return &m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX];
	};


	UINT32			GetDurationMSec(AgpdSiegeWar *pcsSiegeWar);

	BOOL			IsEndArchlordSiegeWar(AgpdSiegeWar *pcsSiegeWar);

	BOOL			AddArchlordCastleAttackGuild(AgpdGuild *pcsGuild);
	
	BOOL	SetGuildTargetPosition(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos, BOOL bInner);
	BOOL	SetAttackGuildTargetPosition(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos, BOOL bInner = FALSE);
	BOOL	SetDefenseGuildTargetPosition(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos, BOOL bInner = TRUE);
};