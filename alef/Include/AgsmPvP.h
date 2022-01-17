// AgsmPvP.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 12. 26.

#ifndef _AGSMPVP_H_
#define _AGSMPVP_H_

#include "AuPacket.h"

#include "ApmMap.h"
#include "AgpmCharacter.h"
#include "AgpmFactors.h"
#include "AgpmParty.h"
#include "AgpmGuild.h"
#include "AgpmGrid.h"
#include "AgpmItem.h"
#include "AgpmEventTeleport.h"
#include "AgpmSummons.h"
#include "AgpmSkill.h"
#include "AgpmPvP.h"
#include "AgpmLog.h"
#include "AgpmConfig.h"
#include "AgpmSiegeWar.h"

#include "AgsmAOIFilter.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmCombat.h"
#include "AgsmDeath.h"
#include "AgsmSummons.h"
#include "AgsmSkill.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmGuild.h"
#include "AgsmTitle.h"

#include "AuRandomNumber.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgsmPvPD.lib")
#else
	#pragma comment(lib, "AgsmPvP.lib")
#endif
#endif

class AgsmAI2;
class AgpmBattleGround;
class AgsmBattleGround;

enum eAGSMPVP_CHAR_STATUS
	{
	AGSMPVP_CHAR_STATUS_NORMAL = 0,
	AGSMPVP_CHAR_STATUS_ASSAILANT,
	AGSMPVP_CHAR_STATUS_MURDERER1,
	AGSMPVP_CHAR_STATUS_MURDERER2,
	AGSMPVP_CHAR_STATUS_MURDERER3,
	AGSMPVP_CHAR_STATUS_MAX,
	};


enum eAGSMPVP_DROP_EQUIP_PART
	{
	AGSMPVP_DROP_EQUIP_PART_HEAD = 0,
	AGSMPVP_DROP_EQUIP_PART_BREAST,
	AGSMPVP_DROP_EQUIP_PART_LEG,
	AGSMPVP_DROP_EQUIP_PART_SLEEVE,
	AGSMPVP_DROP_EQUIP_PART_FOOT,
	AGSMPVP_DROP_EQUIP_PART_SHIELD,
	AGSMPVP_DROP_EQUIP_PART_WEAPON,
	AGSMPVP_DROP_EQUIP_PART_MAX,
	};


enum eAGSMPVP_EXCEL_COLUM
	{
	AGSMPVP_EXCEL_COLUM_TYPE = 0,
	AGSMPVP_EXCEL_COLUM_EQUIP_DROP,
	AGSMPVP_EXCEL_COLUM_BAG_DROP,
	AGSMPVP_EXCEL_COLUM_EQUIP_DROP_RATE_START,
	AGSMPVP_EXCEL_COLUM_EQUIP_BAG_RATE_START = (AGSMPVP_EXCEL_COLUM_EQUIP_DROP_RATE_START + AGSMPVP_DROP_EQUIP_PART_MAX),
	};


enum eAGSMPVP_CB
	{
	AGSMPVP_CB_PVPDEAD = 0,
	AGSMPVP_CB_CHECK_CRIMINAL,
	};


class AgsdPvPItemDropRate
	{
	public:
		INT32	m_lDropEquip;
		INT32	m_lDropBag;
		
		ApSafeArray<INT32, AGSMPVP_DROP_EQUIP_PART_MAX>		m_lRateEquip;
		ApSafeArray<INT32, AGPMITEM_MAX_INVENTORY>			m_lRateBag;
	};


class AgsdPvPCharismaPoint
	{
	public:
		INT32	m_lDiff;
		INT32	m_lNormalPoint;
		INT32	m_lRaceBattlePoint;
		INT32	m_lKillableCount;
	
	public:
		AgsdPvPCharismaPoint()
			{
			m_lDiff = 0;
			m_lNormalPoint = 0;
			m_lRaceBattlePoint = 0;
			m_lKillableCount = 1;
			}
	};


class AgsdPvPKilledChar
	{
	private :
		INT32		m_lDayOfYear;
		INT32		m_lKilledCount;
	
	public:
		AgsdPvPKilledChar()
			{
			m_lDayOfYear = -1;
			m_lKilledCount = 0;
			}
		
		INT32	AddKilledCount(INT32 lDayOfYear)
			{
			if (m_lDayOfYear != lDayOfYear)
				{
				m_lDayOfYear = lDayOfYear;
				m_lKilledCount = 0;
				}
			
			m_lKilledCount++;
			
			return m_lKilledCount;
			}
	};


typedef hash_map<string, AgsdPvPKilledChar>					KilledCharInfoMap;
typedef pair<string, AgsdPvPKilledChar>						KilledCharInfoMapPair;
typedef KilledCharInfoMap::iterator							KilledCharInfoMapIter;

typedef hash_map<string, KilledCharInfoMap>					KilledCharMap;
typedef pair<string, KilledCharInfoMap>						KilledCharMapPair;
typedef KilledCharMap::iterator								KilledCharMapIter;

const UINT32 AGSMPVP_IDLE_CLOCK_INTERVAL		=	500;		// 0.5 sec

#define AGSMPVP_INI_SECTION_NAME_RACEBATTLE		"RaceBattle"
#define AGSMPVP_INI_RACE_BATTLE_STATUS			"Status"
#define AGSMPVP_INI_RACE_BATTLE_HUMAN_SCORE		"Human"
#define AGSMPVP_INI_RACE_BATTLE_ORC_SCORE		"Orc"
#define AGSMPVP_INI_RACE_BATTLE_MOONELF_SCORE	"Moonelf"

/************************************************/
/*		The Definition of Relay Sender Thread	*/
/************************************************/
//
class RaceBattlePacketSender : public zzThread
	{
	protected:
		CHAR				m_szName[20];
		AuCircularBuffer	m_csBuffer;
		AgsmAOIFilter		*m_pAgsmAOIFilter;
		BOOL				m_bStop;

		HANDLE				m_hSendEvent;
		BOOL				m_bInit;
		ApMutualEx			m_csLock;
		deque<PVOID>		m_QueueItem;
		deque<INT16>		m_QueueLength;
		
	protected:
		//	zzThread inherited
		BOOL		OnCreate();
		void		OnTerminate();
		DWORD		Do();

		//	Internal
		PVOID	Get(INT16 &nLength);
		void	Remove(PVOID pv);
		BOOL	Send();

	public:
		RaceBattlePacketSender();
		virtual ~RaceBattlePacketSender();

		//	Stop
		void	Stop();

		//	Set(Init)
		BOOL	Set(INT32 lBufferSize, AgsmAOIFilter *pAgsmAOIFilter, CHAR *pszName);
		
		//	Push(to send)
		BOOL Push(PVOID pvPacket, INT16 nPacketLength);
	};




class AgsmPvP : public AgsModule
{
public:
	static const TCHAR	s_szCharStatus[AGSMPVP_CHAR_STATUS_MAX][30];
	static const INT32	s_lDropEquipItem[AGSMPVP_DROP_EQUIP_PART_MAX];
	static const INT32	s_lNextEvalEquip[AGSMPVP_DROP_EQUIP_PART_MAX];
	static const TCHAR	s_szRaceBattleStatus[AGPMPVP_RACE_BATTLE_STATUS_MAX][20];

	typedef vector<INT32>				TIDVector;
	typedef vector<INT32>::iterator		TIDIter;

private:
	ApmMap* m_papmMap;
	ApmEventManager* m_papmEventManager;
	AgpmCharacter* m_pagpmCharacter;
	AgpmFactors* m_pagpmFactors;
	AgpmParty* m_pagpmParty;
	AgpmGuild* m_pagpmGuild;
	AgpmGrid* m_pagpmGrid;
	AgpmItem* m_pagpmItem;
	AgpmEventTeleport* m_pagpmEventTeleport;
	AgpmSummons* m_pagpmSummons;
	AgpmSkill* m_pagpmSkill;
	AgpmPvP* m_pagpmPvP;
	AgpmLog* m_pagpmLog;
	AgpmConfig* m_pagpmConfig;
	AgpmSiegeWar* m_pagpmSiegeWar;
	AgpmBattleGround		*m_pagpmBattleGround;

	AgsmAOIFilter* m_pagsmAOIFilter;
	AgsmCharacter* m_pagsmCharacter;
	AgsmCharManager* m_pagsmCharManager;
	AgsmCombat* m_pagsmCombat;
	AgsmDeath* m_pagsmDeath;
	AgsmSummons* m_pagsmSummons;
	AgsmSkill* m_pagsmSkill;
	AgsmItem* m_pagsmItem;
	AgsmItemManager* m_pagsmItemManager;
	AgsmGuild* m_pagsmGuild;
	AgsmAI2* m_pagsmAI2;
	AgsmBattleGround*		m_pagsmBattleGround;
	AgsmTitle*				m_pagsmTitle;

	MTRand m_csRandom;

	// 최종 evaluation rate로 실제 떨굴지 말지를 정한다.
	// 최초 evaluation rate로 어느 곳에 있는걸 떨굴지를 정한다.
	ApSafeArray<INT32, AGSMPVP_DROP_EQUIP_PART_MAX>				m_lEquipDropRate;
	ApSafeArray<INT32, AGPMITEM_MAX_INVENTORY>					m_lBagDropRate;
	ApSafeArray<AgsdPvPItemDropRate, AGSMPVP_CHAR_STATUS_MAX>	m_ItemDropRate;
	
	ApSafeArray<AgsdPvPCharismaPoint, 50>	m_CharismaPoint;
	INT32									m_lTotalCharismaPoint;

	TIDVector m_vcPCTID;
	TIDVector m_vcSummonsTID;
	
	RaceBattlePacketSender		m_PacketSender;
	ApCriticalSection			m_RaceBattlePointLock;
	CHAR						m_szIniFile[_MAX_PATH];
	ApSafeArray<UINT32, AGPMPVP_RACE_BATTLE_STATUS_MAX>		m_ulRaceBattleTimeOfSec;
	UINT32						m_ulTickCount;
	INT32						m_lRemained;
	INT32						m_lSiegeDayOfYear;
	INT32						m_lNotSiegeDayOfYear;
	KilledCharMap				m_KilledCharMap;
	ApCriticalSection			m_KilledCharMapLock;

	UINT32						m_ulPreviousClock;

public:
	AgsmPvP();
	virtual ~AgsmPvP();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle(UINT32 ulClockCount);
	BOOL OnDestroy();

	// Operation
	BOOL OnAttack(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	BOOL OnSkill(AgpdSkill* pcsSkill, AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	BOOL OnDead(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bDisconnect = FALSE);
	BOOL UpdateCharacter(AgpdCharacter* pcsCharacter, UINT32 ulClockCount);
	BOOL IsAttackable(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bForce);
	BOOL IsSkillEnable(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget);

	BOOL ProcessExceptionNonExistFinalAttackerOnDead(AgpdCharacter* pcsCharacter);

	BOOL OnAttackSummons(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);

	INT32 CopyFriendEnemy(AgpdCharacter* pcsSource, AgpdCharacter* pcsTarget, BOOL bPacket = TRUE);
	INT32 NotifyEnterCombatAreaToFriend(AgpdCharacter* pcsCharacter);
	INT32 NotifyEnterCombatAreaToEnemy(AgpdCharacter* pcsCharacter);
	INT32 NotifyLeaveCombatAreaToFriend(AgpdCharacter* pcsCharacter);
	INT32 NotifyLeaveCombatAreaToEnemy(AgpdCharacter* pcsCharacter);
	INT32 ConvertEnemyGuildMemberToEnemy(AgpdCharacter* pcsCharacter, INT8 cNewPvPMode = -1);
	
	INT32 NotifyDisconnectToFriend(AgpdCharacter* pcsCharacter);
	INT32 NotifyDisconnectToFriendGuildMembers(AgpdCharacter* pcsCharacter);
	INT32 NotifyDisconnectToEnemy(AgpdCharacter* pcsCharacter);
	INT32 NotifyDisconnectToEnemyGuildMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsMostDamager = NULL, BOOL bLeaveCombatArea = FALSE);

	INT32 ProcessEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, UINT32 ulClockCount = 0);
	INT32 ProcessEnemyGuild(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, UINT32 ulClockCount = 0);

	BOOL ProcessCriminalAttack(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTraget);
	BOOL ProcessMurderer(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTarget);
	BOOL ProcessCharisma(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTarget);

	INT32 AddEnemyToFriends(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode = -1, UINT32 ulClockCount = 0);
	INT32 AddEnemyToFriendGuildMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode = -1, UINT32 ulClockCount = 0);
	INT32 AddEnemyToCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode = -1, UINT32 ulClockCount = 0);
	INT32 AddEnemyFriendsToCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode = -1, UINT32 ulClockCount = 0);
	INT32 AddEnemyFriendGuildsToCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode = -1, UINT32 ulClockCount = 0);

	INT32 RemoveEnemyToCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy );

	INT32 UpdateEnemyToFriends(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode = -1, UINT32 ulClockCount = 0);
	INT32 UpdateEnemyToFriendGuildMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode = -1, UINT32 ulClockCount = 0);
	INT32 UpdateEnemyGuildToFriends(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode = -1, UINT32 ulClockCount = 0);
	INT32 UpdateEnemyGuildToFriendGuildMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cNewPvPMode = -1, UINT32 ulClockCount = 0);

	AgpdCharacter* GetOneGuildMemberInCombatArea(AgpdCharacter* pcsCharacter);
	AgpdCharacter* GetOnePartyMemberInCombatArea(AgpdCharacter* pcsCharacter);

	INT32 ConvertEnemyGuildMeToEnemyMe(AgpdCharacter* pcsCharacter, CHAR* szGuildID);

	// Guild Battle
	BOOL AddEnemyGuildByBattle(AgpdCharacter* pcsCharacter, CHAR* szEnemyGuildID);
	BOOL RemoveEnemyGuildByBattle(AgpdCharacter* pcsCharacter, CHAR* szEnemyGuild);

	// Dead (PvP Result)
	BOOL ProcessDeadInFreeArea(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	BOOL ProcessDeadInCombatArea(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	BOOL ProcessItemDrop(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	BOOL UseAntiDrop(AgpdCharacter *pAgpdCharacter, INT32 lTID);
	BOOL ProcessSkullDrop(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	BOOL AddItemToField(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, AgpdItem* pcsItem);
	AgpdItem* GetEquipItemByDropPart(AgpdCharacter* pcsCharacter, INT32 lPart);
	AgpdItem* GetInvenItemByDropPart(AgpdCharacter* pcsCharacter, INT32 lLayer, BOOL *pbExist = NULL);
	AgpdCharacter* GetMostAttackerInParty(AgpdParty* pcsParty, AgpdCharacter* pcsDeadCharacter);
	INT32 NotifyDeadToFriendGuildMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	INT32 NotifyDeadToPartyMembers(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);

	// Race War
	BOOL	StreamReadRaceBattleStatus(CHAR *pszFile);
	BOOL	StreamReadRaceBattleTime(CHAR *pszFile, BOOL bDecryption);
	BOOL	StreamWriteRaceBattleStatus();
	void	WriteLogRaceBattleResult();
	void	ProcessRaceBattle();
	BOOL	IsEnableCharismaPoint(CHAR *pszCharID, CHAR *pszKilledCharID, INT32 lDayOfYear, INT32 lCount);
	
	// Exception
	BOOL ProcessLeaveGuild(CHAR* szGuildID, CHAR* szCharID);
	BOOL ProcessLeaveParty(AgpdCharacter* pcsCharacter, AgpdParty* pcsParty);
	BOOL ProcessDestroyParty(AgpdParty* pcsParty);

	// PvP Status
	BOOL UpdatePvPStatus(AgpdCharacter* pcsCharacter);
	BOOL SetInvincible(AgpdCharacter* pcsCharacter, UINT32 ulClockCount = 0);
	BOOL UnsetInvincible(AgpdCharacter* pcsCharacter);
	BOOL SetPvPIdleInterval(AgpdCharacter* pcsCharacter);
	
	// Connect
	BOOL EnterGameWorld(AgpdCharacter* pcsCharacter);
	BOOL Disconnect(AgpdCharacter* pcsCharacter);
	
	// Area
	//BOOL RegionChange(AgpdCharacter* pcsCharacter, INT16 nPrevRegionIndex);
	BOOL EnterSafePvPArea(AgpdCharacter* pcsCharacter);
	BOOL EnterFreePvPArea(AgpdCharacter* pcsCharacter);
	BOOL EnterCombatPvPArea(AgpdCharacter* pcsCharacter);
	BOOL LeaveFreePvPArea(AgpdCharacter* pcsCharacter);
	BOOL LeaveCombatPvPArea(AgpdCharacter* pcsCharacter);

	// Log
	BOOL WriteDeathLog(AgpdCharacter *pAgpdCharacterAttack, AgpdCharacter *pAgpdCharacterDead, INT8 cDeadType);
	BOOL WriteKillLog(AgpdCharacter *pAgpdCharacterAttack, AgpdCharacter *pAgpdCharacterDead, INT8 cDeadType);

	// Packet Send
	BOOL SendPvPInfo(AgpdCharacter* pcsCharacter);
	BOOL SendPvPResult(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	BOOL SendPvPInfoToNear(AgpdCharacter* pcsCharacter);
	BOOL SendPvPResultToNear(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);

	BOOL SendAddFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID);
	BOOL SendAddEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID, INT8 cPvPMode = -1, UINT32 ulCombatClock = 0);
	BOOL SendRemoveFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID);
	BOOL SendRemoveEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID);
	BOOL SendInitFriend(AgpdCharacter* pcsCharacter);
	BOOL SendInitEnemy(AgpdCharacter* pcsCharacter);
	BOOL SendUpdateFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID);
	BOOL SendUpdateEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID, INT8 cPvPMode = -1, UINT32 ulCombatClock = 0);

	BOOL SendAddFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID);
	BOOL SendAddEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT8 cPvPMode = -1, UINT32 ulCombatClock = 0);
	BOOL SendRemoveFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID);
	BOOL SendRemoveEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID);
	BOOL SendInitFriendGuild(AgpdCharacter* pcsCharacter);
	BOOL SendInitEnemyGuild(AgpdCharacter* pcsCharacter);
	BOOL SendUpdateFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID);
	BOOL SendUpdateEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT8 cPvPMode = -1, UINT32 ulCombatClock = 0);

	BOOL SendSystemMessage(INT32 lCode, UINT32 ulNID, CHAR* szData1 = NULL, CHAR* szData2 = NULL, INT32 lData1 = 0, INT32 lData2 = 0);

	BOOL SendResponseDeadType(AgpdCharacter* pcsCharacter, INT32 lTargetCID, INT32 lDropExpRate, INT32 lTargetType);
	
	BOOL SendRaceBattleInfo(AgpdCharacter *pcsCharacter, INT8 cStatus);

	BOOL SetCallbackPvPDead(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCheckCriminal(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// Callback

	// From AgpmPvP
	static BOOL CBEnterSafePvPArea(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEnterFreePvPArea(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEnterCombatPvPArea(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBLeaveFreePvPArea(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBLeaveCombatPvPArea(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBProcessSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRequestDeadType(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBResponseDeadType(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// From AgsmAOIFilter
	static BOOL CBMoveCell(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// From AgpmCharacter
	static BOOL CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSendCharView(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgsmCharManager
	static BOOL CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgsmCharacter
	static BOOL CBOnAttack(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBIsAttackable(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgsmSkill
	static BOOL CBOnSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBIsSkillEnable(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgpmEventTeleport
	static BOOL CBIsUsableTeleport(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgsmDeath
	static BOOL CBOnDead(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgsmGuild
	static BOOL CBLeaveGuild(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAddEnemyGuildByBattle(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveEnemyGuildByBattle(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAddEnemyPrivateGuildByBattle(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveEnemyPrivateGuildByBattle(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgpmParty
	static BOOL CBLeaveParty(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDestroyParty(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	
	BOOL	StreamReadItemDropRate(CHAR *pszFile, BOOL bDecryption);
	BOOL	StreamReadCharismaPoint(CHAR *pszFile, BOOL bDecryption);
};



#endif//_AGSMPVP_H_