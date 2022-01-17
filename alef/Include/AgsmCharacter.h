/******************************************************************************
Module:  AgsmCharacter.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 02
******************************************************************************/

#if !defined(__AGSMCHARACTER_H__)
#define __AGSMCHARACTER_H__

#include "ApBase.h"
#include "AsDefine.h"
#include "AgsEngine.h"
#include "ApAdmin.h"
#include "ApHeap.h"
#include "AgpmFactors.h"
#include "AgpdCharacter.h"

#include "AgpmCharacter.h"
#include "AgpmParty.h"
#include "AgpmSkill.h"
#include "AgpmAdmin.h"
#include "AgpmUIStatus.h"
#include "AgpmLog.h"
#include "AgpmOptimizedPacket2.h"
#include "AgpmEventBinding.h"
#include "AgpmItemConvert.h"

#include "AgsmSystemMessage.h"
#include "AgsmFactors.h"
#include "AgsmAOIFilter.h"
#include "AgsmServerManager2.h"
#include "AgsmAccountManager.h"
#include "AgsmMap.h"
#include "AgpmEventTeleport.h"
#include "AgpmSummons.h"

#include "AgpmScript.h"

#include "AgsdCharacter.h"

#include "AuGenerateID.h"
#include "AuPacket.h"

#include "AgsmAccountManager.h"
#include "ApIOCPDispatcher.h"

#include "AuDatabase2.h"

#include "AgpmPvP.h"
#include "AgpmGuild.h"

#include "AgsmInterServerLink.h"

#if defined (_AREA_GLOBAL_) || defined(_AREA_KOREA_)
#include "AuGameGuard.h"
#endif

class AgpmArchlord;

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmCharacterD" )
#else
#pragma comment ( lib , "AgsmCharacter" )
#endif
#endif

//#define	AGSMCHARACTER_PACKET_TYPE	0x1B

#define AGSMCHARACTER_DEFAULT_SEND_MOVE_DELAY			500
#define AGSMCHARACTER_DEFAULT_HISTORY_REFRESH_DELAY		2000

const int	AGSMCHARACTER_UPDATE_MUKZA_INTERVAL			= 3600000;

const int	AGSMCHARACTER_SAVE_CHARACTER_DB_INTERVAL	= 60000;		// 1분

const int	AGSMCHARACTER_SYNC_MOVE_FOLLOW_INTERVAL		= 4000;
const int	AGSMCHARACTER_SYNC_MOVE_INTERVAL			= 5000;

const int	AGSMCHARACTER_PROCESS_IDLE_TIME_INTERVAL	= 2000;			// 2초

const int	AGSMCHAR_DEFAULT_INVINCIBLE_TIME			= 30000;		// 기본 무적 시간 30초. 2006.01.09. steeple
const int	AGSMCHAR_RESSURECTION_ORB_INVINCIBLE_TIME	= 10000;		// 레저럭션 오브 사용시 무적 시간 10초. 2006.01.09. steeple

#define AGSM_CHARACTER_PLAY_LOG_TICK_TERM					10			//AGSMCHARACTER_SAVE_CHARACTER_DB_INTERVAL * 10 마다 로깅(10분)

const int	AUTOPICKUPON	= 1;
const int	AUTOPICKUPOFF	= 0;

typedef enum _eAgsmCharacterCB {
	AGSMCHARACTER_CB_SENDADDCHAR	= 0,
	AGSMCHARACTER_CB_SENDEQUIPITEM,

	AGSMCHARACTER_CB_UPDATE_CONTROLSERVER,

	AGSMCHARACTER_CB_REMOVE_ITEM_ONLY,			// 캐릭터 데이타는 걍 냅두고 아이템 데이타만 삭제한다.

	AGSMCHARACTER_CB_RECALC_FACTOR,
	AGSMCHARACTER_CB_RECALC_RESULT_FACTOR,

	AGSMCHARACTER_CB_UPDATE_LEVEL,
	AGSMCHARACTER_CB_UPDATE_HP,
	AGSMCHARACTER_CB_UPDATE_MAX_HP,

	AGSMCHARACTER_CB_SEND_CHARACTER_ALL_INFO,
	AGSMCHARACTER_CB_SEND_CHARACTER_NEW_ID,
	AGSMCHARACTER_CB_SEND_CHARACTER_ALL_SERVER_INFO,

	AGSMCHARACTER_CB_GET_NEW_CID,

	AGSMCHARACTER_CB_SEND_CELL_INFO,
	AGSMCHARACTER_CB_SEND_CELL_REMOVE_INFO,

	AGSMCHARACTER_CB_ENTER_GAMEWORLD,

	AGSMCHARACTER_CB_REMOVE_WAIT_OPERATION,
	AGSMCHARACTER_CB_COMPLETE_SEND_CHARACTER_INFO,

	AGSMCHARACTER_CB_RECEIVE_CHARACTER_DATA,
	AGSMCHARACTER_CB_DISCONNECT_FROM_GAMESERVER,

	AGSMCHARACTER_CB_SYNC_CHARACTER_ACTIONS,

	AGSMCHARACTER_CB_UPDATE_CHARACTER_TO_DB,
	AGSMCHARACTER_CB_UPDATE_ALL_TO_DB,

	AGSMCHARACTER_CB_RECEIVE_NEW_CID,

	AGSMCHARACTER_CB_COMPLETE_RECV_CHARDATA_FROM_LOGINSERVER,

	AGSMCHARACTER_CB_ADJUST_TRANSFORM_FACTOR,
	AGSMCHARACTER_CB_APPLY_BONUS_FACTOR,

	AGSMCHARACTER_CB_SEND_AUTH_KEY,
	AGSMCHARACTER_CB_SEND_CHAR_VIEW,	// 2005.01.18. steeple

	AGSMCHARACTER_CB_IS_GUILD_MASTER,		//	2005.06.01. By SungHoon

	AGSMCHARACTER_CB_EXIT_GAME_WORLD,
	
	AGSMCHARACTER_CB_UPDATE_BANK_TO_DB,		// laki

} eAgsmCharacterCB;

typedef enum  AgsmCharacterPacketOperation {
	AGSMCHAR_PACKET_OPERATION_UPDATE					= 10,
	AGSMCHAR_PACKET_OPERATION_CHECK_RECV_CHAR,
	AGSMCHAR_PACKET_OPERATION_CHECK_RECV_CHAR_SUCCESS,
	AGSMCHAR_PACKET_OPERATION_CHECK_RECV_CHAR_FAIL,
	AGSMCHAR_PACKET_OPERATION_COMPLETE_SEND_CHAR_INFO,
	AGSMCHAR_PACKET_OPERATION_BAN_DATA,
	AGSMCHAR_PACKET_OPERATION_REQUEST_NEW_CID,
	AGSMCHAR_PACKET_OPERATION_RESPONSE_NEW_CID,
} AgsmCharacterPacketOperation;

struct CheckChatacterValue
{
	INT32 bUse;

	INT32 MaxHP;		// 최대 HP
	INT32 MaxMP;		// 최대 MP
	INT32 fAttackMaxDmg;		// 물리공격력
	INT32 AttackSpeed;	// 공격속도
	//INT32 fPhysicalDefense;	// 물리방어력
	INT32 PhysicalResistance; // 물리저항력
	INT32 lMoveSpeed;		// 이동속도
};


class AgsmLogin;
class AgsmTitle;
class AgpmAI2;

class AgsmCharacter : public AgsModule {
private:
	ApmMap*				m_papmMap;
	AgpmFactors*		m_pagpmFactors;
	AgpmCharacter*		m_pcsAgpmCharacter;
	AgpmSkill*			m_pcsAgpmSkill;
	AgpmParty*			m_pcsAgpmParty;
	AgpmAdmin*			m_pagpmAdmin;
	AgpmUIStatus*		m_pagpmUIStatus;
	AgpmLog*			m_pagpmLog;
	AgpmItem*			m_pagpmItem;
	//AgpmOptimizedPacket*	m_pagpmOptimizedPacket;
	AgpmOptimizedPacket2*	m_pagpmOptimizedPacket2;
	AgpmEventBinding*	m_pcsAgpmEventBinding;
	AgpmEventTeleport*	m_pcsAgpmEventTeleport;
	AgpmPvP*			m_pcsAgpmPvP;
	AgpmScript*			m_pcsAgpmScript;
	AgpmSummons*		m_pcsAgpmSummons;
	AgpmGuild*			m_pcsAgpmGuild;
	AgpmItemConvert*	m_pcsAgpmItemConvert;

	AgsmSystemMessage*	m_pagsmSystemMessage;	// 20051212, kelovon

	//AgsmDBStream*		m_pagsmDBStream;
	AgsmFactors*		m_pagsmFactors;
	AgsmAOIFilter*		m_pagsmAOIFilter;
	AgsmServerManager2*	m_pAgsmServerManager2;
	AgsmAccountManager*	m_pagsmAccountManager;
	//AgsmCharManager*	m_pagsmCharManager;
	AgsmMap*			m_pagsmMap;
	AgsmInterServerLink *	m_pagsmInterServerLink;
	AgpmArchlord*		m_pcsAgpmArchlord;
	AgsmLogin*			m_pagsmLogin;
	AgsmTitle*			m_pagsmTitle;
	AgpmAI2*			m_pagpmAI2;

	ApAdmin				m_csAdminCheckRecvChar;
	ApAdmin				m_csAdminWaitForRemoveChar;
	ApAdmin				m_csAdminConnectedAccount;

	AgpmBillInfo*		m_pcsAgpmBillInfo;//JK_WISM연동

	INT16				m_nIndexADCharacter;

	INT16				m_nMaxHitHistory;

	//AuGenerateID		m_csGenerateID;

	UINT32				m_ulSendMoveDelay;
	UINT32				m_ulHistoryRefreshDelay;

	UINT32				m_ulPrevHistoryRefreshTime;

	UINT32				m_ulNextSaveDBTime;

	UINT32				m_ulNextProcessIdleTime;
	UINT32				m_ulNextProcessIdleTime3;
#ifdef _USE_NPROTECT_GAMEGUARD_
	CTime				m_ulNextProcessIdleTimeGameGuard;
#endif

	ApIOCPDispatcher	m_AuIOCPDispatcher;
	BOOL				m_bUseIOCPDispatcher;
	UINT32				m_ulPreviousClock;

	MTRand				m_csRandom;

	INT32				m_lFactorTypeIndex;

	ApMutualEx			m_csMutexNumOfPlayers;
	INT32				m_lNumOfPlayers;
	INT32				m_lNumOfPCRoomPlayers;//JK_WISM연동
	INT32				m_lNumOfPremiumPCRoomPlayers;//JK_WISM연동



public:
	LONG				m_alNumPlayerByClass[AURACE_TYPE_MAX][AUCHARCLASS_TYPE_MAX];

public:
	ApAdmin				m_csCertificatedAccountID;
	ApHeap				m_csHeap;

	//BOOL				GetForwardSector(ApWorldSector *pOldSector, ApWorldSector *pNewSector, ApWorldSector **ppForwardBuffer, ApWorldSector **ppBackBuffer);
	BOOL				GetForwardCell(AgsmAOICell *pcsOldCell, AgsmAOICell *pcsNewCell, AgsmAOICell **ppForwardBuffer, AgsmAOICell **ppBackBuffer, INT32 lForwardRange, INT32 lBackwardRange);
	ApWorldSector*		GetSectorBySectorIndex(int arrayindexX , int arrayindexY , int arrayindexZ);

private:
	BOOL				OnOperationUpdate(INT32 lCID, PVOID pvCharServerData);
	BOOL				OnOperationBanData(INT32 lCID, PVOID pvBanData);

	BOOL				OnOperationRequestNewCID(INT32 lOldCID, UINT32 ulNID);
	BOOL				OnOperationResponseNewCID(AgpdCharacter *pcsCharacter, INT32 lNewCID, UINT32 ulNID);

	BOOL				ParsingHistoryPacket(AgpdCharacter *pcsCharacter, PVOID pvPacketHistory, UINT32 ulCurrentClockCount, UINT32 ulReceivedClockCount);

public:
	AuPacket			m_csPacket;
	AuPacket			m_csPacketServerData;
	AuPacket			m_csPacketHistory;
	AuPacket			m_csPacketHistoryEntry;
	AuPacket			m_csPacketBase;
	AuPacket			m_csPacketBanData;

public:
	AgsmCharacter();
	~AgsmCharacter();

	BOOL OnAddModule();

	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 lClock);
	BOOL OnIdle2(UINT32 lClock);
	BOOL OnIdle3(UINT32 lClock);

	BOOL LoadCheckCharacterLua();
	BOOL m_bReloadLua;
	CheckChatacterValue	m_CheckCharacterValue;

	//BOOL InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag);
	//INT32 GenerateCID();

	static UINT WINAPI AgsmCharacterDispatcher(PVOID pvParam);
	BOOL InitIOCPDispatcher(INT32 lThreadCount);
	void SetIOCPDispatcherUse(BOOL bUse);

	void SetSendMoveDelay(UINT32 ulDelay);
	void SetHistoryRefreshDelay(UINT32 ulDelay);

	BOOL OnValid(CHAR* szData, INT16 nSize);

	BOOL OnPreReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL OnDisconnect(INT32 lCID);
	
	static BOOL ConAgsdCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesAgsdCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	AgsdCharacter* GetADCharacter(AgpdCharacter *pData);

	BOOL SetServerID(INT32 lCID, INT32 ulServerID);
	BOOL SetServerID(AgpdCharacter *pCharacter, INT32 ulServerID);

	DPNID GetCharDPNID(INT32 lCID);
	DPNID GetCharDPNID(AgpdCharacter *pCharacter);

	CHAR* GetAccountID(AgpdCharacter *pCharacter);
	CHAR* GetServerName(AgpdCharacter *pCharacter);

	/*
	static BOOL CBAddSector(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveSector(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBMoveSector(PVOID pData, PVOID pClass, PVOID pCustData);
	*/

	static BOOL CBAddCell(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveCell(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBMoveCell(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBMoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBStopCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateMoney(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateBankMoney(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateCash(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBMoveBankMoney(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateSpecialStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateCriminalStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateFactor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateLevel(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBPayActionCost(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBCheckActionTargetAttack(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBTransformAppear(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBTransformStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRestoreTransform(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCancelTransform(PVOID pData, PVOID pClass, PVOID pCustData);
	
	static BOOL CBEvolution(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRestoreEvolution(PVOID pData, PVOID pClass, PVOID pCustData);

	//static BOOL CBMapAddCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBGetDPNID(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateMurdererPoint(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateMukzaPoint(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateCharismaPoint(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAddAttackerToList(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBRemoveChar(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateFactorView(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateFactorMovement(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBBackupCharacterData(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBResetNID(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBIsPlayerCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateRegionIndex(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBSocialAnimation(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAccountRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBReleaseActionMove(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateCustomizeIndex(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateOptionFlag( PVOID pData, PVOID pClass, PVOID pCustData );	//	2005.05.31. By SungHoon

	static BOOL CBUpdateBankSize(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBBlockByPenalty(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBCheckAllBlock(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBGetCurrentUser(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAddStaticCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveProtectedNPC(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBGetBonusDropRate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGetBonusDropRate2(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGetBonusMoneyRate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGetBonusCharismaRate(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBChangeAutoPickItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRenameCharacterID(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdCharacter *EnterGameWorld(AgpdCharacter *pCharacter, BOOL bZoning = FALSE);

	BOOL SetCallbackSendAddCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSendEquipItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	BOOL SetCallbackUpdateControlServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackRemoveItemOnly(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackReCalcFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackReCalcResultFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackUpdateLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateHP(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateMaxHP(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackSendCharacterAllInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSendCharacterNewID(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSendCharacterAllServerInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	/*
	BOOL SetCallbackSendSectorInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSendSectorRemoveInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	*/

	BOOL SetCallbackSendCellInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSendCellRemoveInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackEnterGameworld(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackRemoveWaitOperation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCompleteSendCharacterInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//BOOL SetCallbackSendAllDBData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackReceiveCharacterData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	/*
	BOOL SetCallbackDBStreamInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBStreamDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBStreamSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBStreamUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	*/

	BOOL SetCallbackGetNewCID(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackDisconnectFromGameServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackSyncCharacterActions(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackReceiveNewCID(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackCompleteRecvCharDataFromLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackAdjustTransformFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackApplyBonusFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackSendAuthKey(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackIsGuildMaster(ApModuleDefaultCallBack pfCallback, PVOID pClass);	//	2005.06.01 By SungHoon
	BOOL SetCallbackExitGameWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//static BOOL SyncNewChar(PVOID pData, PVOID pClass);

	BOOL RemoveItemOnly(AgpdCharacter *pCharacter);

	BOOL ReCalcCharacterFactors(INT32 lCID, BOOL bSendPacket = TRUE);
	BOOL ReCalcCharacterFactors(AgpdCharacter *pcsCharacter, BOOL bSendPacket = TRUE);
	BOOL ReCalcCharacterResultFactors(AgpdCharacter *pcsCharacter, BOOL bSendPacket = TRUE);

	PVOID MakePacket(INT16* pnPacketLength, INT8* pcOperation, INT32* plCID, INT8* pcStatus, CHAR* szZoneServerAddress,
								INT32* plZoneServerID, PVOID pvServerCharData, PVOID pvBanData, INT32 *plNewCID = NULL);
	PVOID MakePacketAddCharacter(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, PVOID pvPacketFactor);
	PVOID MakePacketAgsdCharacter(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);
	PVOID MakePacketAgsdCharacterLogin(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);
	PVOID MakePacketHistory(AgpdCharacter *pcsCharacter);
	PVOID MakePacketBase(ApBase *pcsBase);
	PVOID MakePacketBanData(AgpdCharacter* pcsCharacter, INT16* pnPacketLength);
	PVOID MakePacketRequestNewCID(INT32 lOldCID, INT16 *pnPacketLength);
	PVOID MakePacketResponseNewCID(INT32 lOldCID, INT32 lNewCID, INT16 *pnPacketLength);

	PVOID MakePacketCharRemove(INT32 lCID, INT16 *pnPacketLength);

	PVOID MakePacketUpdateCriminalStatus(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	PVOID MakePacketAgpdDBData(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	PVOID MakePacketFactor(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 *pnPacketLength);

	PVOID MakePacketUpdateCustomize(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	PVOID MakePacketUpdateBankSize(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	//BOOL SendPacketDBData(AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID = FALSE);

	//BOOL SendPacketAllDBData(AgpdCharacter *pcsCharacter, DPNID dpnid);

	BOOL SendPacketCharBasicData(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bGroupNID);

	BOOL SendPacketFactor(PVOID pvPacketFactor, AgpdCharacter *pcsCharacter, PACKET_PRIORITY ePriority);
	BOOL SendPacketFactor(PVOID pvPacketFactor, AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID = FALSE, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4);

	BOOL SendPacketCharStatus(AgpdCharacter *pcsCharacter);
	BOOL SendPacketCharStatus(AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID = FALSE);

	BOOL SendPacketCharActionStatus(AgpdCharacter *pcsCharacter);
	BOOL SendPacketCharActionStatus(AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID = FALSE);

	BOOL SendPacketCharCriminalStatus(AgpdCharacter *pcsCharacter);
	BOOL SendPacketCharCriminalStatus(AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID = FALSE);
	BOOL SendPacketNearCharCriminalStatus(AgpdCharacter *pcsCharacter);

	BOOL SendPacketCharPosition(AgpdCharacter *pcsCharacter);
	BOOL SendPacketCharPosition(AgpdCharacter *pcsCharacter, DPNID dpnid, BOOL bGroupNID = FALSE);

	BOOL SendPacketTransformAppear(AgpdCharacter *pcsCharacter, INT32 lTID);
	BOOL SendPacketRestoreTransformAppear(AgpdCharacter *pcsCharacter);

	BOOL SendPacketEvolutionAppear(AgpdCharacter *pcsCharacter, INT32 lTID);
	BOOL SendPacketRestoreEvolutionAppear(AgpdCharacter *pcsCharacter, INT32 lTID);

	BOOL SendPacketRidableAppear(AgpdCharacter *pcsCharacter);
	BOOL SendPacketRestoreRidable(AgpdCharacter *pcsCharacter);

	BOOL SendPacketLevelUp(AgpdCharacter *pcsCharacter, PVOID pvPacketFactor = NULL);

	BOOL SendPacketMurdererPoint(AgpdCharacter *pcsCharacter);
	BOOL SendPacketMukzaPoint(AgpdCharacter *pcsCharacter);
	BOOL SendPacketCharismaPoint(AgpdCharacter *pcsCharacter);
	BOOL SendPacketNickName(AgpdCharacter *pcsCharacter);

	/*
	BOOL SendSectorToChar(ApWorldSector *pSector, AgpdCharacter *pCharacter);
	BOOL SendCharToSector(AgpdCharacter *pCharacter, ApWorldSector *pSector, BOOL bIsNewChar = FALSE);

	BOOL SendSectorRemoveToChar(ApWorldSector *pSector, AgpdCharacter *pCharacter);
	BOOL SendCharRemoveToSector(AgpdCharacter *pCharacter, ApWorldSector *pSector, BOOL bIsForView = TRUE);
	*/

	BOOL SendCellToChar(AgsmAOICell *pcsCell, AgpdCharacter *pCharacter);
	BOOL SendCharToCell(AgpdCharacter *pCharacter, AgsmAOICell *pcsCell, BOOL bIsNewChar = FALSE);

	BOOL SendCellRemoveToChar(AgsmAOICell *pcsCell, AgpdCharacter *pCharacter);
	BOOL SendCharRemoveToCell(AgpdCharacter *pCharacter, AgsmAOICell *pcsCell, BOOL bIsForView = TRUE);

	BOOL SendCellNPCToChar(AgsmAOICell *pcsCell, AgpdCharacter *pCharacter);
	BOOL SendCellNPCRemoveToChar(AgsmAOICell *pcsCell, AgpdCharacter *pCharacter);

	// 해당 Region의 모든 NPC 전송 2005.06.24 by kelovon
	BOOL SendRegionNPCToChar(INT16 nRegionIndex, AgpdCharacter *pCharacter);
	BOOL SendRegionNPCRemoveToChar(INT16 nRegionIndex, AgpdCharacter *pCharacter);

	BOOL SendPacketChar(AgpdCharacter *pcsCharacter, UINT_PTR ulNID, BOOL bGroupNID);
	BOOL SendPacketCharView(AgpdCharacter *pCharacter, UINT_PTR ulNID, BOOL bGroupNID, BOOL bIsNewChar = FALSE, BOOL bIsExceptSelf = FALSE);
	
	BOOL SendPacketCharRemove(AgpdCharacter *pCharacter, UINT_PTR ulNID, BOOL bGroupNID);
	BOOL SendPacketCharRemove(INT32 lCID, UINT_PTR ulNID, BOOL bGroupNID);

	BOOL SendPacketCharRemoveForView(AgpdCharacter *pCharacter, UINT_PTR ulNID, BOOL bGroupNID);
	BOOL SendPacketCharRemoveForView(INT32 lCID, UINT_PTR ulNID, BOOL bGroupNID);

	BOOL SendPacketUpdateFactorView(AgpdCharacter *pCharacter);

	BOOL SendPacketUpdateRegionIndex(AgpdCharacter *pCharacter);
	BOOL SendPacketUpdateEventStatusFlag(AgpdCharacter* pcsCharacter);
	
	BOOL SendPacketCharAdminInfo(AgpdCharacter* pCharacter, UINT32 ulNID);	// 2004.03.05 steeple
	BOOL SendPacketBanData(AgpdCharacter* pCharacter, UINT32 ulNID);

	BOOL SendCharacterAllInfo(AgpdCharacter *pCharacter, UINT32 ulNID, BOOL bSyncSend = FALSE, BOOL bSendServerInfo = FALSE, BOOL bLogin = FALSE, INT32 lOldCID = AP_INVALID_CID);
	BOOL SendCharacterAllServerInfo(AgpdCharacter *pCharacter, UINT32 ulNID, BOOL bLogin = TRUE);

	BOOL SendAddAttackerID(AgpdCharacter *pCharacter, INT32 lAttackerID, UINT32 ulNID);

	BOOL SendPacketRequestNewCID(INT32 lOldCID, UINT32 ulNID);

	BOOL SendDisconnectByAnotherUser( UINT32 ulNID );

	BOOL SendPacketSocialAnimation(AgpdCharacter *pcsCharacter, UINT8 ucSocialAnimation);

	BOOL SendPacketUpdateBankSize(AgpdCharacter *pcsCharacter, UINT32 ulNID);
	
	BOOL SendPakcetBlockByPenalty(AgpdCharacter *pcsCharacter, INT32 lPenalty, UINT32 ulNID);

	BOOL SendPacketCharUseEffect(AgpdCharacter *pcsCharacter, AgpdCharacterAdditionalEffect eEffect, INT32 lExtraType = 0);

	BOOL SendPacketSkillInitString(AgpdCharacter* pcsCharacter);

	BOOL SendPacketResurrectionByOther(AgpdCharacter* pcsCharacter, CHAR* szName, INT32 lFlag);

	BOOL SendPacketEventEffectID(INT32 lCID, INT32 lEventID, UINT32 ulNID);

	INT32 GetGiveSkillPoint(AgpdCharacter *pcsCharacter);
	BOOL GiveSkillPoint(AgpdCharacter *pcsCharacter);
	BOOL GiveSkillPoint(AgpdCharacter *pcsCharacter, INT32 lSkillPoint);

	BOOL	SetCharacterLevel(AgpdCharacter *pcsCharacter, INT32 lLevel, BOOL bCopyOnlyAttack);
	BOOL	IsAuctionBlocked(AgpdCharacter* pcsCharacter);

	//Agro관련
	//각종 어그로를 다양하게 증가시키는 함수.
	BOOL						AddAgroPoint( AgpdCharacter *pcsCharacter, ApBase *pcsSource, eAgpdFactorAgroType eAgroType, INT32 lAgroPoint );
	//EXP와 직접 관련된 어그로 증가함수.
	BOOL						AddAgroPoint( AgpdCharacter *pcsCharacter, ApBase *pcsSource, AgpdFactor *pcsFactor );
	//각종 어그로를 줄여주는 함수.
	BOOL						DecreaseHistory( AgpdCharacter *pcsCharacter, eAgpdFactorAgroType eAgroType, INT32 lDecreasePoint );

	BOOL						AddHistory(AgpdCharacter *pcsCharacter, ApBase *pcsSource, AgpdFactor *pcsFactor, UINT32 ulTime);
	BOOL						AddHistoryEntry(AgsdCharacterHistory *pstHistory, ApBase *pcsSource, AgpdFactor *pcsFactor, UINT32 ulTime, INT32 lPartyID = AP_INVALID_PARTYID);
	BOOL						RemoveHistory(AgpdCharacter *pcsCharacter, ApBase *pcsSource);
	BOOL						RemovePartyHistory(AgpdCharacter *pcsCharacter, ApBase *pcsSource);
	AgsdCharacterHistoryEntry *	GetHistoryEntry(AgpdCharacter *pcsCharacter, ApBase *pcsSource);
	AgsdCharacterHistory *		GetHistory(AgpdCharacter *pcsCharacter);
	AgsdCharacterHistory *		GetPartyHistory(AgpdCharacter *pcsCharacter);
	BOOL						InitHistory(AgpdCharacter *pcsCharacter);
	BOOL						InitPartyHistory(AgpdCharacter *pcsCharacter);	// 2005.04.26. steeple

	AgsdCharacterHistoryEntry *	GetMostAgroChar(AgpdCharacter *pcsCharacter);
	
	AgsdCharacterHistoryEntry *	GetMostDamager(AgpdCharacter *pcsCharacter);
	AgsdCharacterHistoryEntry * GetMostDamagerInParty(AgpdCharacter *pcsCharacter, INT32 lPartyID);
	AgsdCharacterHistoryEntry * GetFinalAttacker(AgpdCharacter* pcsCharacter);
	AgsdCharacterHistoryEntry * GetFinalAttackerInParty(AgpdCharacter* pcsCharacter);
	BOOL						IsDamager( AgpdCharacter *pcsCharacter, INT32 lTargetCID );
	BOOL						IsNoAgro( AgpdCharacter *pcsCharacter );
	INT32						GetAgroFromEntry( AgpdCharacter *pcsCharacter, INT32 lTargetCID );
	INT32						GetMobListFromHistoryEntry( AgpdCharacter *pcsCharacter, INT32 *alMobList, INT32 lArraryCount );

	/*
	// get DB Query String.
	BOOL				GetInsertQuery( AgpdCharacter *pcsCharacter, char *pstrQuery );
	BOOL				GetInsertQuery( AgpdCharacter *pcsCharacter, INT32 lSlotIndex, char *pstrQuery );
	BOOL				GetSelectQuery( char *szCharName, char *pstrQuery );
	BOOL				GetSelectAllQuery( char *szAccountID, char *pstrQuery );
	BOOL				GetUpdateQuery( AgpdCharacter *pcsCharacter, char *pstrQuery );
	BOOL				GetDeleteQuery( char *szCharName, char *pstrQuery );

	BOOL				GetUpdateLevelQuery( AgpdCharacter *pcsCharacter, char *pstrQuery );
	BOOL				GetUpdateSkillPointQuery( AgpdCharacter *pcsCharacter, char *pstrQuery );
	BOOL				GetUpdateMajorDataQuery( AgpdCharacter *pcsCharacter, char *pstrQuery );

	// Get Query Result;
	INT32				GetSelectResult( COLEDB	*pcOLEDB );

	// db operation
	BOOL				StreamInsertDB(AgpdCharacter *pcsCharacter, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamSelectDB(CHAR *szCharName, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamSelectByAccountDB(CHAR *szAccountName, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamUpdateDB(AgpdCharacter *pcsCharacter, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamDeleteDB(AgpdCharacter *pcsCharacter, ApModuleDefaultCallBack pfCallback, PVOID pClass);

//	BOOL				StreamUpdateDBLevel(AgpdCharacter *pcsCharacter, ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL				StreamUpdateDBSkillPoint(AgpdCharacter *pcsCharacter, ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL				StreamUpdateDBMajorData(AgpdCharacter *pcsCharacter, ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// get DB Query String2
	BOOL				GetInsertQuery2( AgpdCharacter *pcsCharacter, char *pstrQuery );
	BOOL				GetInsertQuery2( AgpdCharacter *pcsCharacter, INT32 lSlotIndex, char *pstrQuery );
	BOOL				GetSelectQuery2( char *szCharName, char *pstrQuery );
	BOOL				GetSelectAllQuery2( char *szAccountID, char *pstrQuery );
	BOOL				GetUpdateQuery2( AgpdCharacter *pcsCharacter, char *pstrQuery );
	BOOL				GetDeleteQuery2( char *szCharName, char *pstrQuery );

	BOOL				GetUpdateLevelQuery2( AgpdCharacter *pcsCharacter, char *pstrQuery );
	BOOL				GetUpdateSkillPointQuery2( AgpdCharacter *pcsCharacter, char *pstrQuery );
	BOOL				GetUpdateMajorDataQuery2( AgpdCharacter *pcsCharacter, char *pstrQuery );

	// Get Query Result;
	INT32				GetSelectResult2( COLEDB *pcOLEDB );
	*/

	// get DB Query String3
	/*
	BOOL				GetMasterInsertQuery3( AgpdCharacter *pcsCharacter, char *pstrWorldDBName, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetMasterInsertQuery3( AgpdCharacter *pcsCharacter, char *pstrWorldDBName, INT32 lSlotIndex, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetMasterSelectQuery3( char *szCharName, char *pstrWorldDBName, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetMasterSelectAllQuery3( char *szAccountID, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetMasterDeleteQuery3( char *szCharName, char *pstrQuery, INT32 lQueryLength );

	BOOL				GetDetailInsertQuery3( AgpdCharacter *pcsCharacter, char *pstrWorldDBName, char *pstrQBeltString, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetDetailSelectQuery3( char *szCharName, char *pstrWorldDBName, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetDetailDeleteQuery3( char *szCharName, char *pstrQuery, INT32 lQueryLength );

	BOOL				GetAccountWorldInsertQuery( char *szAccountID, char *pstrWorldDBName, INT64 llBankMoney, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetAccountWorldSelectQuery( char *szAccountID, char *pstrWorldDBName, char *pstrQuery, INT32 lQueryLength);

	BOOL				GetCharacterDeleteQueyr3( char *szCharName, char *pstrWorldDBName, char *pstrQuery, INT32 lQueryLength );


	BOOL				GetMasterInsertQuery4( char *pstrQuery, INT32 lQueryLength );
	BOOL				SetParamMasterInsertQuery4( COLEDB *pcOLEDB, AgpdCharacter *pcsCharacter, INT32 lSlotIndex );
	BOOL				GetMasterSelectQuery4( char *pstrQuery, INT32 lQueryLength );
	BOOL				GetMasterSelectAllQuery4( char *pstrQuery, INT32 lQueryLength );
	BOOL				GetMasterDeleteQuery4( char *pstrQuery, INT32 lQueryLength );

	BOOL				GetDetailInsertQuery4( char *pstrQuery, INT32 lQueryLength );
	BOOL				SetParamDetailInsertQuery4( COLEDB *pcOLEDB, AgpdCharacter *pcsCharacter, char *pstrQBeltString );
	BOOL				GetDetailSelectQuery4( char *pstrQuery, INT32 lQueryLength );
	BOOL				GetDetailDeleteQuery4( char *pstrQuery, INT32 lQueryLength );

	BOOL				GetAccountWorldInsertQuery4( INT64 llBankMoney, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetAccountWorldSelectQuery4( char *pstrQuery, INT32 lQueryLength);

	BOOL				GetCharacterDeleteQuery4( char *pstrQuery, INT32 lQueryLength );


	// Get Query Result;
	INT32				GetMasterSelectResult3( COLEDB *pcOLEDB );
	BOOL				GetDetailSelectResult3( AgpdCharacter *pcsCharacter, COLEDB *pcOLEDB );
	BOOL				GetAccountWorldSelectResult3( AgpdCharacter *pcsCharacter, COLEDB *pcOLEDB );
	*/

	// Insert	
	BOOL				SetParamMasterInsertQuery5(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter, INT8 cSlotIndex, CHAR *pszServerName);
	BOOL				SetParamDetailInsertQuery5(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter, CHAR *pszQuickBelt);

	// Select
	INT32				GetMasterSelectResult5(AuDatabase2 *pDatabase, INT8 &cSlot);
	BOOL				GetDetailSelectResult5(AgpdCharacter *pcsCharacter, AuDatabase2 *pDatabase);

	// AccountWorld
	BOOL				GetAccountWorldSelectResult5(AgpdCharacter *pcsCharacter, AuDatabase2 *pDatabase);

	
	
	
	
	// 각종 저장 함수들
	/////////////////////////////////////////////////////////////////////
	BOOL				BackupCharacterData(AgpdCharacter *pcsCharacter);
	BOOL				UpdateBankToDB(AgpdCharacter *pcsCharacter);

	/*
	BOOL				SaveCharacterDB(AgpdCharacter *pcsCharacter);
		// 특정 이벤트가 발생했을때 DB에 저장 (level up, update skill point etc)
	BOOL				SaveCharacterLevel(AgpdCharacter *pcsCharacter);
	BOOL				SaveCharacterSkillPoint(AgpdCharacter *pcsCharacter);
		// 주기적으로 DB에 저장할때 사용 (exp, union rank etc)
	BOOL				SaveCharacterMajorData(AgpdCharacter *pcsCharacter);

	BOOL				SetCallbackSaveCharacterDB(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	*/

	//static BOOL			CBStreamDB(PVOID pData, PVOID pClass, PVOID pCustData);

	// functions for send character all information
	/////////////////////////////////////////////////////////////////////
	BOOL				SetMaxCheckRecvChar(UINT32 lMaxCheckRecvChar);

	BOOL				SendCheckRecvChar(INT32 lCID, UINT32 ulDPNID, INT32 lOldCID);
	BOOL				SendCheckRecvCharResult(INT32 lCID, UINT32 ulDPNID, BOOL bResult);
	BOOL				SendCompleteSendCharInfo(INT32 lCID, UINT32 ulDPNID);

	BOOL				IsRecvChar(INT32 lCID);

	BOOL				CheckRecvChar(INT32 lCID, INT32 lServerID, INT32 lOldCID);

	BOOL				CheckRecvCharList(INT32 lCID);
	AgsdServer*			GetRecvCharServer(AgpdCharacter* pcsCharacter);

	// wait operation for remove character
	/////////////////////////////////////////////////////////////////////
	BOOL				SetMaxWaitForRemoveChar(UINT32 lCount);

	BOOL				SetWaitOperation(AgpdCharacter *pcsCharacter, AgsmCharacterWaitOperation eWaitOperation);
	BOOL				ResetWaitOperation(AgpdCharacter *pcsCharacter, AgsmCharacterWaitOperation eWaitOperation);

	BOOL				IsWaitOperation(AgpdCharacter *pcsCharacter);
	BOOL				IsWaitTimeout(AgpdCharacter *pcsCharacter, UINT32 ulClockCount);

	BOOL				AddListRemoveChar(AgpdCharacter *pcsCharacter);
	BOOL				RemoveListRemoveChar(AgpdCharacter *pcsCharacter);

	// Relay Server Operation
	// Callback 함수
	BOOL				SetCallbackCharacterUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackBankUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackAllUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				EncodingPosition(AuPOS *pstPos, CHAR *szBuffer, INT32 lBufferSize);
	BOOL				DecodingPosition(CHAR *szPositionBuffer, AuPOS *pstPos);

	BOOL				SaveAllCharacterData();

	// Logging 관련 함수들
	// 시간은 모두 milisecond 단위임...
	BOOL				StartPlayTime(AgpdCharacter *pcsCharacter);

	BOOL				StartPartyPlay(AgpdCharacter *pcsCharacter);
	BOOL				StopPartyPlay(AgpdCharacter *pcsCharacter);

	BOOL				AddKillMonCount(AgpdCharacter *pcsCharacter);
	BOOL				AddKillPCCount(AgpdCharacter *pcsCharacter);
	BOOL				AddDeadByMonCount(AgpdCharacter *pcsCharacter);
	BOOL				AddDeadByPCCount(AgpdCharacter *pcsCharacter);

	UINT32				GetPartyPlayTimeMSec(AgpdCharacter *pcsCharacter);
	UINT32				GetSoloPlayTimeMSec(AgpdCharacter *pcsCharacter);
	UINT32				GetKillMonCount(AgpdCharacter *pcsCharacter);
	UINT32				GetKillPCCount(AgpdCharacter *pcsCharacter);
	UINT32				GetDeadByMonCount(AgpdCharacter *pcsCharacter);
	UINT32				GetDeadByPCCount(AgpdCharacter *pcsCharacter);

	BOOL				ResetPlayLogData(AgpdCharacter *pcsCharacter);

	// 2004.05.18. steeple
	BOOL				WritePlayLog(AgpdCharacter *pcsCharacter, INT32 lLevelUp);
	BOOL				WriteGheldLog(eAGPDLOGTYPE_GHELD eType, AgpdCharacter *pcsCharacter, INT32 lGheld);
	BOOL				WriteBankExpLog(AgpdCharacter *pcsCharacter);

	BOOL				SetTransformTimeout(AgpdCharacter *pcsCharacter, UINT32 ulDurationTMsec);
	BOOL				ResetTransformTimeout(AgpdCharacter *pcsCharacter);

	BOOL				AdjustTransformFactor(AgpdCharacter *pcsCharacter, AgpdFactor *pcsUpdateFactor);

	BOOL				ApplyBonusFactor(AgpdCharacter *pcsCharacter, AgpdFactor *pcsUpdateFactor);

	BOOL				SetIdleInterval(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType, AgsdCharacterIdleInterval eInterval);
	BOOL				IsIdleProcessTime(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType, UINT32 ulClockCount);
	BOOL				ResetIdleInterval(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType);
	BOOL				SetProcessTime(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType, UINT32 ulClockCount);
	UINT32				GetLastIdleProcessTime(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType);
	UINT32				GetElapsedTimeFromLastIdleProcess(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType, UINT32 ulClockCount);

	BOOL				IsProcessIdle(AgpdCharacter *pcsCharacter, UINT32 ulClockCount);
	BOOL				ProcessIdle(AgpdCharacter *pcsCharacter, UINT32 ulClockCount);
	
	BOOL				SetSpecialStatusTime(AgpdCharacter* pcsCharacter, UINT64 ulSpecialStatus, UINT32 ulDuration, INT32 lValue = 0);
	BOOL				CheckSpecialStatusIdleTime(AgpdCharacter* pcsCharacter, UINT32 ulClockCount);
	BOOL				RestoreSpecialStatusTime(AgpdCharacter* pcsCharacter, UINT64 ulOldSpecialStatus);

	static BOOL			CBCheckProcessIdle(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				AddReservedTimeForDestroy(AgpdCharacter *pcsCharacter, UINT32 ulAddReservedTimeMSec);
	UINT32				GetReservedTimeForDestory(AgpdCharacter *pcsCharacter);

	BOOL				GenerateAuthKey(AgpdCharacter *pcsCharacter);

	BOOL				SetCallbackSendCharView(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				CheckProcessIdle(AgpdCharacter *pcsCharacter, UINT32 ulClockCount);

	BOOL				ApplySpecialStatusFactor(AgpdCharacter* pcsCharacter);

	BOOL				AddTargetInfo(AgpdCharacter* pcsAttacker, INT32 lTargetCID, UINT32 ulClockCount = 0);
	BOOL				RemoveTargetInfo(AgpdCharacter* pcsAttacker, INT32 lTargetCID);
	BOOL				InitTargetInfoArray(AgpdCharacter* pcsCharacter);
	INT32				CheckTargetInfoIdle(AgpdCharacter* pcsCharacter, UINT32 ulClockCount);

	BOOL				SetDefaultInvincible(AgpdCharacter* pcsCharacter);			// 2006.01.09. steeple
	BOOL				SetResurrectionOrbInvincible(AgpdCharacter* pcsCharacter);	// 2006.01.09. steeple

	BOOL				AddExp(AgpdCharacter *pcsCharacter, INT64 llAddExp);
	BOOL				SubExp(AgpdCharacter *pcsCharacter, INT64 llSubExp);

	BOOL				SetHPFull(AgpdCharacter* pcsCharacter);
	BOOL				DecreaseHP(AgpdCharacter* pcsCharacter, INT32 lDrceaseHP);
	BOOL				AdjustExceedPoint(AgpdCharacter* pcsCharacter);
	BOOL				SyncHP(AgpdCharacter* pcsCharacter);

	INT32				GetNumOfPlayers();
	INT32				GetNumOfPremiumPCRoomPlayers();//JK_WISM연동
	INT32				GetNumOfPCRoomPlayers();
	
	BOOL				SetCash(AgpdCharacter *pcsCharacter, INT64 llCash);
	INT64				SubCash(AgpdCharacter *pcsCharacter, INT64 llCash);

	BOOL				ProcessReleaseSpecialStatus(AgpdCharacter *pcsCharacter);

	// seongkyeong cho
	//BOOL				SetPaidTimeout(AgpdCharacter *pcsCharacter, UINT32 ulDurationTMsec);
	//BOOL				ResetPaidTimeout(AgpdCharacter *pcsCharacter);
	//BOOL				GGSendInitialPacket(AgpdCharacter* pcsCharacter);
	//BOOL				GGSendServerAuth(AgpdCharacter* pcsCharacter);


private:
	vector<string>		m_vectorStaticCharacterName;

public:
	static BOOL			CheckStaticCharacter(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);

	void	AddServerNameToCharName(CHAR *pszOriginalName, CHAR *pszServerName, CHAR *pszNewNameBuffer);
	CHAR	*GetRealCharName(CHAR *pszCharName);

	BOOL	IsAutoPickup(AgpdCharacter *pcsCharacter);
	VOID	SetAutoPickup(AgpdCharacter *pcsCharacter, BOOL bSetValue);

	BOOL	SetCharacterLevelLimit		( AgpdCharacter * pcsCharacter , INT32 nLevel );
	BOOL	ReleaseCharacterLevelLimit	( AgpdCharacter * pcsCharacter );

	BOOL	CheckLoginPosition(AgpdCharacter* pcsCharacter);

	BOOL	IsNotLogoutStatus(AgpdCharacter* pcsCharacter);
	BOOL	IsResurrectingNow(AgpdCharacter* pcsCharacter);
	static BOOL CBGameGuardAuth(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	MoveCharacterPosition(AgpdCharacter* pcsCharacter, FLOAT PosX, FLOAT PosZ);
	BOOL	MoveCharacterIncludeParty(AgpdCharacter* pcsCharacter, FLOAT PosX, FLOAT PosZ);
};

#endif //__AGSMCHARACTER_H__
