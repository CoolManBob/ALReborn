// AgsmGuild.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 15.

#ifndef _AGSMGUILD_H_
#define _AGSMGUILD_H_

#include "AuPacket.h"

#include "AgpmCharacter.h"
#include "AgpmFactors.h"
#include "AgpmGuild.h"
#include "AgpmLog.h"
#include "AgpmItem.h"
#include "AgsmServerManager2.h"
#include "AgsmInterServerLink.h"
#include "AgsmAOIFilter.h"
#include "AgsmCharacter.h"
#include "AgsmChatting.h"
#include "AgsmCharManager.h"
#include "AgsmLogin.h"
#include "AgsdGuild.h"
#include "AuDatabase2.h"
#include "AgsdRelay2.h"
#include <AuXmlParser.h>

#define WINNER_GUILD_NAME "WinnerGuild.txt"

typedef enum _eAgsmGuildPacketType
{
	AGSMGUILD_PACKET_GUILD_LOAD = 0,
	AGSMGUILD_PACKET_MEMBER_LOAD,

	AGSMGUILD_PACKET_SYNC_CREATE,
	AGSMGUILD_PACKET_SYNC_JOIN,
	AGSMGUILD_PACKET_SYNC_LEAVE,
	AGSMGUILD_PACKET_SYNC_FORCED_LEAVE,
	AGSMGUILD_PACKET_SYNC_DESTROY,
	AGSMGUILD_PACKET_SYNC_UPDATE_MAX_MEMBER_COUNT,
	AGSMGUILD_PACKET_SYNC_UPDATE_NOTICE,

	AGSMGUILD_PACKET_MAX,
} eAgsmGuildPacketType;

typedef enum _eAgsmGuildCallback
{
	AGSMGUILD_CB_DB_GUILD_INSERT = 0,
	AGSMGUILD_CB_DB_GUILD_UPDATE,
	AGSMGUILD_CB_DB_GUILD_DELETE,
	AGSMGUILD_CB_DB_MEMBER_INSERT,
	AGSMGUILD_CB_DB_MEMBER_UPDATE,
	AGSMGUILD_CB_DB_MEMBER_DELETE,

	AGSMGUILD_CB_DB_GUILD_SELECT,
	AGSMGUILD_CB_DB_MEMBER_SELECT,
	AGSMGUILD_CB_DB_MASTER_CHECK,

	AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY,

	AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_BATTLE,			// For AgsmPvP
	AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_BATTLE,		// For AgsmPvP

	AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_SURVIVE_BATTLE,			// For AgsmPvP
	AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_SURVIVE_BATTLE,		// For AgsmPvP

	AGSMGUILD_CB_DB_GUILD_RENAME,
	
	AGSMGUILD_CB_GUILD_LOAD,
	
	AGSMGUILD_CB_ENTER_GAME,
	
	AGSMGUILD_CB_SEND_GUILD_INFO,

	AGSMGUILD_CB_DB_JOINT_INSERT,
	AGSMGUILD_CB_DB_JOINT_UPDATE,
	AGSMGUILD_CB_DB_JOINT_DELETE,
	AGSMGUILD_CB_DB_HOSTILE_INSERT,
	AGSMGUILD_CB_DB_HOSTILE_DELETE,

	AGSMGUILD_CB_MAX,

} AgsmGuildCallback;

typedef enum _eAgsmGuildModuleDataType
{
	AGSMGUILD_DATA_TYPE_MEMBER = 0,
	AGSMGUILD_DATA_TYPE_BATTLE_PROCESS,
	AGSMGUILD_DATA_TYPE_NUM,
} AgsmGuildModuleDataType;

#define AGSMGUILD_LOAD_COMPLETE_STRING			"!★Loading Complete★!"
#define	AGSMGUILD_IDLE_TIME_INTERVAL			1000 * 60		//	1분

class AgpmSiegeWar;
class AgpmArchlord;
class AgpmBattleGround;
class AgsmSystemMessage;
class ApmMap;

class AgsmGuild : public AgsModule
{
private:
	HANDLE m_hEventGuildLoadComplete;

	AgpmCharacter* m_pagpmCharacter;
	AgpmFactors* m_pagpmFactors;
	AgpmPvP* m_pagpmPvP;
	AgpmLog* m_pagpmLog;
	AgpmItem* m_pagpmItem;
	AgsmServerManager* m_pagsmServerManager;
	AgsmInterServerLink* m_pagsmInterServerLink;
	AgsmAOIFilter* m_pagsmAOIFilter;
	AgsmCharacter* m_pagsmCharacter;
	AgsmChatting* m_pagsmChatting;
	AgsmCharManager* m_pagsmCharManager;
	AgsmLogin *m_pagsmLogin;
	AgsmItem *m_pagsmItem;
	AgpmSiegeWar *m_pagpmSiegeWar;
	AgpmArchlord *m_pagpmArchlord;
	AgpmBattleGround*		m_pagpmBattleGround;
	AgsmSystemMessage*		m_pagsmSystemMessage;
	ApmMap*					m_papmMap;

	INT16 m_nIndexADMember;

	ApMutualEx m_csBattleProcessLock;
	GuildBattleList		m_listBattleProcess;

	UINT32 m_ulBattleReadyTime;
	UINT32 m_ulBattleCancelEnableTime;

	INT32 m_lBattleNeedMemberCount;
	INT32 m_lBattleNeedLevelSum;

	UINT32 m_ulLastProcessBattleClock;
	UINT32 m_ulLastProcessLeaveGuildClock;

	ApMutualEx	m_Mutex;
	ApSafeArray<ApString<32>, WINNER_GUILD_MAX> szWinnerGuildName;

public:
	AgpmGuild* m_pagpmGuild;
	AuPacket m_csPacket;
	
public:
	AgsmGuild();
	virtual ~AgsmGuild();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle(UINT32 ulClockCount);
	BOOL OnIdle2(UINT32 ulClockCount);
	BOOL OnDestroy();

	BOOL SetEventGuildLoadComplete(HANDLE hEvent);

	// Attached Data
	AgsdGuildMember* GetADMember(AgpdGuildMember* pData);
	static BOOL ConAgsdGuildMember(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesAgsdGuildMember(PVOID pData, PVOID pClass, PVOID pCustData);
	
	//////////////////////////////////////////////////////////////////////////
	// Operation

	// From Other GameServer(s)
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL OnReceiveSyncCreate(AgpdGuild* pcsGuild);
	BOOL OnReceiveSyncJoin(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember);
	BOOL OnReceiveSyncLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember);
	BOOL OnReceiveSyncForcedLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember);
	BOOL OnReceiveSyncDestroy(AgpdGuild* pcsGuild);
	BOOL OnReceiveSyncUpdateMaxMemberCount(AgpdGuild* pcsGuild);
	BOOL OnReceiveSyncUpdateNotice(AgpdGuild* pcsGuild, CHAR* szNotice, INT16 nNoticeLength);

	BOOL CreateGuildCheck(AgpdGuild* pcsGuild, CHAR* szMasterID);
	BOOL CreateGuildCheckEnable(CHAR* szGuildID, CHAR* szMasterID);
	BOOL CreateGuildSuccess(CHAR* szGuildID, CHAR* szMasterID);
	BOOL CreateGuildFail(CHAR* szGuildID, CHAR* szMasterID);
	
	BOOL CheckGuildID(CHAR* szGuildID);

	BOOL JoinRequest(CHAR* szGuildID, CHAR* szMasterID, AgpdCharacter* pcsCharacter);
	BOOL JoinCheckEnable(CHAR* szGuildID, CHAR* szCharID, BOOL bSelfRequest);
	BOOL JoinReject(CHAR* szGuildID, CHAR* szCharID);
	BOOL JoinMember(CHAR* szGuildID, CHAR* szMemberID);

	BOOL Leave(CHAR* szGuildID, CHAR* szCharID);
	BOOL ForcedLeave(CHAR* szGuildID, CHAR* szCharID);
	BOOL LeaveChar(CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID);

	BOOL DestroyGuild(CHAR* szGuildID, CHAR* szMasterID, INT32 lMemberCount, CHAR** pszMemberID, BOOL bSaveDB = TRUE, BOOL bWriteLog = TRUE);
	BOOL DestroyGuildFail(CHAR* szGuildID, CHAR* szMasterID);
	BOOL CleanUpRelation(CHAR* szGuildID, BOOL bSaveDB = TRUE);

	INT32 CalcMaxMemberCount(AgpdCharacter* pcsCharacter);
	BOOL UpdateMaxMemberCount(AgpdGuild* pcsGuild, INT32 lNewMaxMember, BOOL bSaveDB = TRUE);
	BOOL CheckPassword(AgpdGuild* pcsGuild, CHAR* szPassword);

	BOOL UpdateAgsdGuildMember(CHAR* szGuildID, CHAR* szMemberID, INT32 lServerID, UINT32 ulNID, AgpdCharacter* pcsCharacter = NULL);
	BOOL UpdateAgsdGuildMember(AgsdGuildMember* pcsAgsdMember, INT32 lServerID, UINT32 ulNID, AgpdCharacter* pcsCharacter = NULL);

	BOOL CharacterLevelUp(AgpdCharacter* pcsCharacter, INT32 lLevelUpNum);

	BOOL EnterGameWorld(AgpdCharacter* pcsCharacter);
	BOOL Disconnect(AgpdCharacter* pcsCharacter);
	BOOL CharacterDeleteComplete(CHAR* szCharID);
	BOOL IsGuildMaster(AgpdCharacter *pcsCharacter);
	BOOL IsSameGuild(AgpdCharacter *pcsChar1, AgpdCharacter *pcsChar2);
	BOOL GuildMessage(AgpdCharacter* pcsCharacter, CHAR* szMessage, INT32 lLength);
	BOOL GuildJointMessage(AgpdCharacter* pcsCharacter, CHAR* szMessage, INT32 lLength);

	UINT32 GetMemberNID(AgpdGuildMember* pcsMember);

	BOOL ProcessRefreshAllGuildMemberStatus(UINT32 ulClockCount);

	// Battle
	BOOL SetCustomBattleTime(UINT32 ulReadyTime = 0, UINT32 ulDurationMin = 0, UINT32 ulDurationMax = 0, UINT32 ulCancelEnableTime = 0,
							INT32 lBattleNeedMemberCount = 0, INT32 lBattleNeedLevelSum = 0);

	INT32 CheckBattleEnable(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild);

	BOOL BattleRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 ulPerson);
	BOOL BattleAccept(CHAR* szGuildID, CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 ulPerson);
	BOOL BattleReject(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);

	BOOL BattleCancelRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	BOOL BattleCancelAccept(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);
	BOOL BattleCancelReject(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID);

	BOOL AddBattleProcess(CHAR* szGuildID1, CHAR* szGuildID2, INT8 cStatus, UINT32 ulAcceptTime, UINT32 ulReadyTime, UINT32 ulStartTime, INT32 lType, UINT32 ulDuration, UINT32 ulPerson);
	BOOL RemoveBattleProcess(AgsdGuildBattleProcess* pcsGuildBattleProcess);
	BOOL RemoveBattleProcess(CHAR* szGuildID1, CHAR* szGuildID2);
	BOOL RemoveBattleProcessList(CHAR* szGuildID1, CHAR* szGuildID2);
	AgsdGuildBattleProcess* GetBattleProcess(CHAR* szGuildID1, CHAR* szGuildID2);

	BOOL ProcessAllBattle(UINT32 ulClockCount);
	BOOL ProcessBattle(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime);
	BOOL ProcessBattleException(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime);

	void BattleListLock() { m_csBattleProcessLock.WLock(); }
	void BattleListUnlock() { m_csBattleProcessLock.Release(); }

	BOOL IsBattleStart(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime);
	BOOL IsBattleEnd(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime);
	BOOL BattleStart(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime);
	BOOL BattleEnd(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime);
	BOOL BattleEndProcess(AgpdGuild* pcsWinGuild, AgpdGuild* pcsLoseGuild);
	BOOL BattleUpdateTime(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime);
	BOOL ClearBattleProcessEachGuild(AgsdGuildBattleProcess* pcsBattleProcess);

	BOOL AddEnemyGuildToAllMembers(AgpdGuild* pcsGuild, CHAR* szEnemyGuildID);
	BOOL RemoveEnemyGuildToAllMembers(AgpdGuild* pcsGuild, CHAR* szEnemyGuildID);
	BOOL AddEnemyGuildToBattleAllMembers(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild);
	BOOL RemoveEnemyGuildToBattleAllMembers(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild);
	BOOL AddEnemyGuildToBattleMembers(CHAR* szMyID, CHAR* szEnemyID);
	BOOL RemoveEnemyGuildToBattleMembers(CHAR* szMyID, CHAR* szEnemyID);
	
	BOOL BattleUpdatePrivateMode(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2);

	// Battle Score
	BOOL CalcBattleScore(AgpdCharacter* pcsAttacker, AgpdCharacter* pcsDead);
	BOOL CalcBattleScoreByDisconnect(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsMostDamager = NULL);
	BOOL CalcBattleScoreProcess(AgpdGuild* pcsGuild1, AgpdCharacter* pcsWinner, AgpdGuild* pcsGuild2, AgpdCharacter* pcsLoser);


	// Withdraw
	BOOL BattleWithdraw(AgpdGuild* pcsGuild, AgpdCharacter* pcsCharacter);
	BOOL BattleWithdraw(AgpdGuild* pcsGuild, CHAR* szMasterID);


	// Joint, Hostile
	BOOL JointRequest(CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szMasterID);
	BOOL JointReject(CHAR* szGuildID, CHAR* szJointGuildID);
	BOOL Joint(CHAR* szGuildID, AgpdGuildRelationUnit* pUnit);
	BOOL JointLeave(INT32 lCID, CHAR* szGuildID, BOOL bForce = FALSE);
	BOOL JointLeave(AgpdGuild* pcsGuild, CHAR* szGuildID, AgpdGuildRelationUnit& stNewLeaderUnit);
	BOOL HostileRequest(CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID);
	BOOL HostileReject(CHAR* szGuildID, CHAR* szHostileGuildID);
	BOOL Hostile(CHAR* szGuildID, AgpdGuildRelationUnit* pUnit);
	BOOL HostileLeaveRequest(CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID);
	BOOL HostileLeaveReject(CHAR* szGuildID, CHAR* szHostileGuildID);
	BOOL HostileLeave(CHAR* szGuildID, CHAR* szHostileGuildID, BOOL bForce = FALSE);

	INT32 CheckJointEnable(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2);
	INT32 CheckHostileEnable(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2);
	INT32 CheckHostileLeaveEnable(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2);
	
	BOOL CheckValidationForRelation();




	// DB Load
	BOOL LoadAllGuildInfo();
	BOOL ClearAllGuildInfo();

	// From AgsmRelay. DB Select
	BOOL GuildLoadFromDB(AgsdRelay2GuildMaster* pAgsdRelay);
	BOOL MemberLoadFromDB(CHAR* szMemberID, CHAR* szGuildID, INT32 lRank, INT32 lJoinDate, INT32 lLevel, INT32 lTID);
	BOOL GuildIDCheckFromDB(BOOL bResult, CHAR* szGuildID, CHAR* szMasterID, CHAR* szPassword);
	BOOL RelationLoadFromDB(CHAR* szGuildID, CHAR* szRelationGuildID, INT8 cRelation, UINT32 ulDate);

	// Get Char Guild ID
	BOOL GetSelectQueryCharGuildID(CHAR* szQuery, INT32 lQueryLength);
	BOOL GetSelectQueryCharGuildID(CHAR* szCharID, CHAR* szWorldDBName, CHAR* szQuery, INT32 lQueryLength);
	//BOOL GetSelectResultCharGuildID(COLEDB* pcOLEDB, AgpdCharacter* pcsCharacter);

	BOOL GetSelectResultCharGuildID5(AuDatabase2 *pDatabase, AgpdCharacter* pcsCharacter);

	// 우승 길드
	BOOL SetWinnerGuild(CHAR* szGuildName, INT32 lBRRank);



	//////////////////////////////////////////////////////////////////////////
	// Packet Send
	BOOL SendSystemMessage(INT32 lCode, UINT32 ulNID, CHAR* szData1 = NULL, CHAR* szData2 = NULL, INT32 lData1 = 0, INT32 lData2 = 0);
	BOOL SendSystemMessageToAllMembers(INT32 lCode, AgpdGuild* pcsGuild, CHAR* szData1 = NULL, CHAR* szData2 = NULL, INT32 lData1 = 0, INT32 lData2 = 0);

	BOOL SendCreateGuild(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID);
	BOOL SendJoinRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID, INT32 lCID, UINT32 ulNID);
	BOOL SendJoinMember(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember, INT32 lCID, UINT32 ulNID);
	BOOL SendSelfJoinMember(CHAR *szGuldID, AgpdGuildRequestMember *pcsMember, INT32 lCID, UINT32 ulNID);	//	2005.08.01. By SungHoon
	BOOL SendLeaveAllowMember(AgpdGuild* pcsGuild, CHAR* szCharID, INT32 lCID, UINT32 ulNID);
	BOOL SendLeaveMember(AgpdGuild* pcsGuild, CHAR* szCharID, INT32 lCID, UINT32 ulNID);		//	2005.08.19. By SungHoon
	BOOL SendDestroyGuild(CHAR* szGuildID, CHAR* szMasterID, INT32 lCID, UINT32 ulNID);
	BOOL SendMaxMemberCount(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID);
	BOOL SendCharGuildData(CHAR* szGuildID, CHAR* szCharID, AgpdGuildMember* pcsMember, INT32 lCID, UINT32 ulNID);
	BOOL SendCharGuildData(AgpdGuildRequestMember* pcsMember, INT32 lCID, UINT32 ulNID);	//	2005.08.17. By SungHoon

	BOOL SendNotice(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID);
	BOOL SendNoticeToAllMember(AgpdGuild* pcsGuild);

	BOOL SendNewMemberJoinToOtherMembers(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember);
	BOOL SendNewMemberSelfJoinToOtherMembers(CHAR *szGuildID, AgpdGuildRequestMember *pcsMember, INT32 lCID);		//	2005.08.01. By SungHoon
	BOOL SendAllMemberJoin(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID);
	BOOL SendOtherMemberMemberJoin(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID, CHAR *szMemberID);		//	2005.08.17. By SungHoon
	AgpdGuildRequestMember *GetJoinRequestMember(AgpdGuild *pcsGuild, CHAR *szID);		//	2005.08.18. By SungHoon

	BOOL SendAllMemberSelfJoin(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID);		//	2005.08.01. By SungHoon

	BOOL SendLeaveToOtherMembers(AgpdGuild* pcsGuild, CHAR* szCharID);
	BOOL SendLeaveAllowToOtherMembers(AgpdGuild* pcsGuild, CHAR* szCharID);
	BOOL SendForcedLeaveToOtherMembers(AgpdGuild* pcsGuild, CHAR* szCharID);

	BOOL SendMaxMemberCountToAllMembers(AgpdGuild* pcsGuild);

	BOOL SendMemberUpdateToMembers(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember, BOOL bIncludeMe = TRUE);
	BOOL SendRequestJoinMemberUpdateToMembers(AgpdGuild* pcsGuild, AgpdGuildRequestMember* pcsMember, BOOL bIncludeMe = TRUE);

	BOOL SendCharGuildIDToNear(CHAR* szGuildID, CHAR* szCharID, AuPOS stPos, INT32 lGuildMarkTID, INT32 lGuildMarkColor, INT32 lBRRanking, UINT32 ulNID);
	BOOL SendCharGuildID(CHAR* szGuildID, CHAR* szCharID, AuPOS stPos, INT32 lGuildMarkTID, INT32 lGuildMarkColor, INT32 lBRRanking, UINT32 ulNID);



	// Battle
	BOOL SendBattleRequest(CHAR* szGuildID, CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 lPerson, UINT32 ulNID);
	BOOL SendBattleAcceptToAllMembers(AgpdGuild* pcsGuild);
	BOOL SendBattleCancelRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, UINT32 ulNID);
	BOOL SendBattleCancelAcceptToAllMembers(AgpdGuild* pcsGuild);

	BOOL SendBattleStartToAllMembers(AgpdGuild* pcsGuild);
	BOOL SendBattleEndToAllMembers(AgpdGuild* pcsGuild);
	BOOL SendBattleUpdateTimeToAllMembers(AgpdGuild* pcsGuild);
	BOOL SendBattleUpdateScoreToAllMembers(AgpdGuild* pcsGuild);
	BOOL SendBattleRoundToAllMembers(AgpdGuild* pcsGuild);
	BOOL SendBattleMemberInfoAllMembers(AgpdGuild* pcsGuild, CHAR* szGuildID, AgpdGuildMember* pMember);
	BOOL SendBattleMemberListInfoAllMembers(AgpdGuild* pcsMyGuild, AgpdGuild* pcsEnemyGuild);

	BOOL SendBattleInfoToAllMembers(AgpdGuild* pcsGuild);
	BOOL SendBattleInfo(AgpdGuild* pcsGuild, UINT32 ulNID);



	// Relation - Joint, Hostile
	BOOL SendJointRequest(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szMasterID, UINT32 ulNID);
	BOOL SendJoint(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulDate, INT8 cRelation, UINT32 ulNID = 0);
	BOOL SendJointLeave(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szNewLeader, UINT32 ulNID = 0);

	BOOL SendHostileRequest(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szMasterID, UINT32 ulNID);
	BOOL SendHostile(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulDate, UINT32 ulNID = 0);
	BOOL SendHostileLeaveRequest(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szMasterID, UINT32 ulNID);
	BOOL SendHostileLeave(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulNID = 0);

	BOOL SendJointAll(AgpdGuild* pcsGuild, UINT32 ulNID = 0);
	BOOL SendHostileAll(AgpdGuild* pcsGuild, UINT32 ulNID = 0);
	BOOL SendJointDetail(AgpdGuild* pcsGuild, AgpdGuild* pcsJoint, UINT32 ulNID = 0);
	BOOL SendHostileDetail(AgpdGuild* pcsGuild, AgpdGuild* pcsHostile, UINT32 ulNID = 0);



	// 해당 패킷을 길드원 전원에게 보낸다.
	BOOL SendPacketToAllMembers(AgpdGuild* pcsGuild, PVOID pvPacket, INT16 nPacketLength, BOOL bBattle = FALSE );
	BOOL SendPacketToOtherMembers(AgpdGuild* pcsGuild, CHAR *szMemberID, PVOID pvPacket, INT16 nPacketLength);	//	2005.08.16. By SungHoon
	BOOL SendPacketToAllJoinRequestMembers(AgpdGuild* pcsGuild, PVOID pvPacket, INT16 nPacketLength);	//	2005.09.05. By SungHoon

	BOOL SendPacketToAllMembers(AgpdGuild* pcsGuild, PACKET_HEADER& pvPacket);

	// 승리길드에 대한 정보를 전송한다.
	BOOL SendPacketWinnerInfo(AgpdGuild* pcsGuild);
	void LoadWinnerGuild();
	void SaveWinnerGuild();



	// Send To Other Game Servers
	BOOL SendSyncCreate(AgpdGuild* pcsGuild);
	BOOL SendSyncJoin(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember);
	BOOL SendSyncLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember);
	BOOL SendSyncForcedLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember);
	BOOL SendSyncDestroy(AgpdGuild* pcsGuild);
	BOOL SendSyncUpdateMaxMemberCount(AgpdGuild* pcsGuild);
	BOOL SendSyncUpdateNotice(AgpdGuild* pcsGuild);

	BOOL SendSyncPacketToOtherGameServers(PVOID pvPacket, INT16 nPacketLength);

	PVOID MakeSyncPacketGuild(INT16* pnPacketLength, INT8 cOperation, AgpdGuild* pcsGuild);
	PVOID MakeSyncPacketMember(INT16* pnPacketLength, INT8 cOperation, AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember);
	PVOID MakeSyncPacketNotice(INT16* pnPacketLength, INT8 cOperation, AgpdGuild* pcsGuild);

	//////////////////////////////////////////////////////////////////////////
	// Callback

	// From AgpmGuild
	static BOOL CBCreateGuildCheck(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCreateGuildCheckEnable(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCreateGuildSuccess(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCreateGuildFail(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBJoinRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBJoinCheckEnable(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBJoinReject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBJoin(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBJoinFail(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBLeave(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBForcedLeave(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDestroyGuild(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDestroyGuildFail(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDestroyGuildCheckTime(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCheckPassword(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateNotice(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBBattlePerson(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBBattleRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBBattleAccept(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBBattleReject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBBattleCancelRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBBattleCancelAccept(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBBattleCancelReject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBBattleWithdraw(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBReceiveGuildListInfo(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.07.08. By SungHoon

	static BOOL CBReceiveJoinRequestSelf(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.08.01. By SungHoon
	static BOOL CBReceiveLeaveRequestSelf(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.07.28. By SungHoon

	static BOOL CBGuildUpdateMaxMemberCount(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.07.13. By SungHoon

	// 2006.07.13. steeple
	static BOOL CBJointRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBJointReject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBJoint(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBJointLeave(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBHostileRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBHostileReject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBHostile(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBHostileLeaveRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBHostileLeaveReject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBHostileLeave(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgpmCharacter
	static BOOL CBCharacterLevelUp(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgsmInterServerLink
	static BOOL CBServerConnect(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBServerSetFlag(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// From AgsmCharManager
	static BOOL CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDeleteComplete(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// From AgsmAOIFilter
	static BOOL CBMoveCell(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// From AgsmChararcter
	static BOOL CBSendCharView(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSendCharacterNewID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBIsGuildMaster(PVOID pData, PVOID pClass, PVOID pCustData);	//	2005.06.01. By SungHoon

	// From AgsmChatting
	static BOOL	CBGuildMemberInviteByCommand(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildLeaveByCommand(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildJointMessage(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgpmItem
	static BOOL CBItemPickupCheck(PVOID pData, PVOID pClass, PVOID pCustData);

	// For Callback Registration
	BOOL SetCallbackDBGuildInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBGuildUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBGuildDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBMemberInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBMemberUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBMemberDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBGuildInsertBattleHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBRename(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildLoad(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSendGuildInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBJointInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBJointUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBJointDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBHostileInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBHostileDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	// For AgsmPvP
	BOOL SetCallbackAddEnemyGuildByBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveEnemyGuildByBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddEnemyGuildBySurviveBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveEnemyGuildBySurviveBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// DB Load
	BOOL SetCallbackDBGuildSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBMemberSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBGuildIDCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);

protected:
	BOOL JoinRequestReject(CHAR *szGuildID, CHAR *szMemberID);		//	2005.08.02. By SungHoon

	VOID CheckLeaveGuildList(UINT32 ulClockCount);

	BOOL RequestLeaveMemberSelf(CHAR *szGuildID, CHAR  *szMemberID);
	BOOL RequestJoinMemberSelf(CHAR* szGuildID, CHAR* szCharID, INT32 lMemberRank,INT32 lJoinDate,INT32 lLevel, INT32 lTID);
	BOOL InsertLeaveMemberSelf(CHAR *szGuildID, CHAR *szMemberID , INT32 lLeaveRequestTime);

	BOOL AllowJoinMember(CHAR *szGuildID, CHAR  *szMemberID);
	BOOL RemoveLeaveMember(CHAR *szMemberID);
	VOID GuildJoinSuccess(AgpdGuild *pcsGuild, AgpdGuildMember *pcsGuildMember, AgpdCharacter *pcsCharacter, BOOL bCreate);

	BOOL RenameGuild(INT32 lCID, CHAR *szGuildID, CHAR *szMemberID);		//	2005.08.24. By SungHoon

	BOOL ProcessAllMemberRenameGuildID(INT32 lCID, CHAR *szOldGuildID, CHAR *szMemberID, AgpdGuild *pcsGuild, PVOID pvRenamePacket , INT16 nPacketLength );
	BOOL ProcessAllRequestJoinMemberRenameGuildID(INT32 lCID, CHAR *szOldGuildID, CHAR *szMemberID, AgpdGuild *pcsGuild, PVOID pvRenamePacket, INT16 nPacketLength );
protected:
	ApSort < AgpdGuildRequestMember * > m_ApGuildLeaveList;
	ApAdmin m_csSelfJoinRequestList;
public:
	static BOOL CBGuildRenameGuildID(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.08.24. By SungHoon
	static BOOL CBGuildRenameCharacterID(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.09.05. By SungHoon
	static BOOL CBGuildBuyGuildMark(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.10.19. By SungHoon
	static BOOL CBGuildBuyGuildMarkForce(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.10.20. By SungHoon

	BOOL GuildBuyGuildMark(INT32 lCID, CHAR *szGuildID, INT32 lGuildMarkTID, INT32 lGuildMarkColor, BOOL bForce);		//	2005.10.19. By SungHoon
private:
	BOOL GuildRenameCharacterID(AgpdGuild *pcsGuild, CHAR *szOldID, CHAR *szNewID);

	BOOL ProcessAllMemberBuyGuildMark(AgpdGuild *pcsGuild, PVOID pvBuyPacket, INT16 nPacketLength );		//	2005.10.19. By SungHoon
	BOOL ProcessAllRequestJoinMemberBuyGuildMark(AgpdGuild *pcsGuild, PVOID pvBuyPacket, INT16 nPacketLength );	//	2005.10.19. By SungHoon

	INT32 SubItemBuyGuildMark(INT32 lGuildMarkTID, AgpdCharacter *pcsCharacter);	//	2005.10.21. By SungHoon

	BOOL CheckGuildKorID(CHAR* szGuildID, int idLength);
	BOOL CheckGuildCnID(CHAR* szGuildID, int idLength);

public:
	static BOOL OnOperationWorldChampionshipRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL OnOperationWorldChampionshipEnter(PVOID pData, PVOID pClass, PVOID pCustData);
	AuXmlNode* GetNoticeNodeWorldChampionship();

	static BOOL CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	AuXmlNode* GetNoticeNodeCSAppointment();
	static BOOL OnOperationCSAppointmentRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL OnOperationCSAppointmentAnswer(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL GuildMemberRankUpdate(AgpdCharacter* pcsCharacter, INT32 nRank);
	BOOL GuildMemberRankUpdate(AgpdGuild* pcsGuild, AgpdGuildMember* pcsGuildMember, INT32 nRank);
	BOOL GuildMemberAppointmentAsk(CHAR *strMaster, CHAR *strMember, INT32 nRank);
	BOOL GuildMemberDismissal(CHAR *strMaster, CHAR *strMember, INT32 nRank);

	static BOOL OnOperationCSSuccessionRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL OnOperationCSSuccessionAnswer(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif //_AGSMGUILD_H_