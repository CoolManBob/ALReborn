#include "AgsmSiegeWar.h"
#include "AuTimeStamp.h"
#include "AuMath.h"

const UINT32 AGSMSIEGEWAR_IDLE = 20 * 1000;	// seconds

const UINT32 DayBySecond  = 86400;			// 24 * 60 * 60
const UINT32 HourBySecond = 3600;			// 60 * 60

//#define	__TEST_SERVER_ONLY__	1

INT32	g_alDamageRateFromAttackType[AGPD_SIEGE_MONSTER_TYPE_MAX][3] =
{	//	원거리,	근거리,	공성전용
	{	0,		0,		0	},		// AGPD_SIEGE_MONSTER_TYPE_NONE
	{	30,		70,		100	},		// AGPD_SIEGE_MONSTER_GUARD_TOWER
	{	30,		70,		100	},		// AGPD_SIEGE_MONSTER_INNER_GATE
	{	30,		70,		100	},		// AGPD_SIEGE_MONSTER_OUTER_GATE
	{	30,		70,		100	},		// AGPD_SIEGE_MONSTER_LIFE_TOWER
	{	30,		70,		100	},		// AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER
	{	30,		70,		100	},		// AGPD_SIEGE_MONSTER_THRONE
	{	30,		70,		100	},		// AGPD_SIEGE_MONSTER_ARCHON_EYES
	{	0,		0,		0	},		// AGPD_SIEGE_MONSTER_ARCHON_EYES_TOWER
	{	30,		70,		100	},		// AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER
};

UINT32	g_aulSyncTimeSec[19]	=
{
	50 * 60,
	40 * 60,
	30 * 60,
	20 * 60,
	10 * 60,
	 9 * 60,
	 8 * 60,
	 7 * 60,
	 6 * 60,
	 5 * 60,
	 4 * 60,
	 3 * 60,
	 2 * 60,
	 1 * 60,
	     50,
		 40,
		 30,
		 20,
		 10
};

bool IsTestServerArchlordSiegeDay( SYSTEMTIME& systime )
{
	if ( systime.wDayOfWeek == 2 || systime.wDayOfWeek == 4 || systime.wDayOfWeek == 6 )
		return true;
	return false;
}

AgsmSiegeWar::AgsmSiegeWar()
{
	SetModuleName("AgsmSiegeWar");
	
	EnableIdle(TRUE);

	ZeroMemory(m_aulLastDBUpdateTime, sizeof(m_aulLastDBUpdateTime));
	
	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		m_acsSiegeWarRegionAdmin[i].InitializeObject(sizeof(UINT32), 4000, NULL, NULL, this);
	}
	
	m_bLoaded = FALSE;
	m_ulLoadedClockCount	= 0;
	m_ulTickCount4SiegeWarDay	= 0;
	m_ulLatestConnOfArchlord	= 0;
}

AgsmSiegeWar::~AgsmSiegeWar()
{
	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		m_acsSiegeWarRegionAdmin[i].RemoveObjectAll(FALSE);
	}

	CloseHandle(m_hEventDBLoad);
}

BOOL AgsmSiegeWar::OnAddModule()
{
	m_pcsAgpmConfig			= (AgpmConfig *)		GetModule("AgpmConfig");
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pcsApmMap				= (ApmMap *)			GetModule("ApmMap");
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgsmFactors		= (AgsmFactors *)		GetModule("AgsmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgsmCharacter		= (AgsmCharacter *)		GetModule("AgsmCharacter");
	m_pcsAgpmSiegeWar		= (AgpmSiegeWar *)		GetModule("AgpmSiegeWar");
	m_pcsAgpmGuild			= (AgpmGuild *)			GetModule("AgpmGuild");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmSkill			= (AgpmSkill *)			GetModule("AgpmSkill");
	m_pcsAgpmPvP			= (AgpmPvP *)			GetModule("AgpmPvP");
	m_pcsAgpmEventBinding	= (AgpmEventBinding *)	GetModule("AgpmEventBinding");
	m_pcsAgsmCombat			= (AgsmCombat *)		GetModule("AgsmCombat");
	m_pcsAgsmDeath			= (AgsmDeath *)			GetModule("AgsmDeath");
	m_pcsAgpmEventSpawn		= (AgpmEventSpawn *)	GetModule("AgpmEventSpawn");
	m_pcsAgsmEventSpawn		= (AgsmEventSpawn *)	GetModule("AgsmEventSpawn");
	m_pcsAgsmAOIFilter		= (AgsmAOIFilter *)		GetModule("AgsmAOIFilter");
	m_pcsAgsmSkill			= (AgsmSkill *)			GetModule("AgsmSkill");
	m_pcsAgsmAI2			= (AgsmAI2 *)			GetModule("AgsmAI2");
	m_pcsAgsmCharManager	= (AgsmCharManager *)	GetModule("AgsmCharManager");
	m_pcsAgsmItem			= (AgsmItem *)			GetModule("AgsmItem");
	m_pcsAgsmInterServerLink	= (AgsmInterServerLink *) GetModule("AgsmInterServerLink");
	m_pcsAgsmServerManager	= (AgsmServerManager *) GetModule("AgsmServerManager2");
	m_pcsAgsmAdmin			= (AgsmAdmin *)			GetModule("AgsmAdmin");
	m_pcsAgsmArchlord		= (AgsmArchlord *)		GetModule("AgsmArchlord");
	m_pcsAgpmArchlord		= (AgpmArchlord *)		GetModule("AgpmArchlord");
	m_pcsAgsmGuild			= (AgsmGuild *)			GetModule("AgsmGuild");
	m_pcsAgsmRide			= (AgsmRide *)			GetModule("AgsmRide");
	
	if (!m_pcsAgpmConfig ||
		!m_pcsApmEventManager ||
		!m_pcsApmMap ||
		!m_pcsAgpmFactors ||
		!m_pcsAgsmFactors ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgpmSiegeWar ||
		!m_pcsAgpmGuild ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmSkill ||
		!m_pcsAgpmPvP ||
		!m_pcsAgpmEventBinding ||
		!m_pcsAgsmCombat ||
		!m_pcsAgsmDeath ||
		!m_pcsAgpmEventSpawn ||
		!m_pcsAgsmEventSpawn ||
		!m_pcsAgsmAOIFilter ||
		!m_pcsAgsmSkill ||
		!m_pcsAgsmAI2 ||
		!m_pcsAgsmCharManager ||
		!m_pcsAgsmItem ||
		!m_pcsAgsmInterServerLink ||
		!m_pcsAgsmServerManager ||
		!m_pcsAgsmAdmin ||
		!m_pcsAgsmArchlord)
		return FALSE;

	if(!m_pcsAgsmInterServerLink->SetCallbackConnect(CBServerConnect, this))
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCallbackUpdateStatus(CBUpdateSiegeWarStatus, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackUpdateCarveAGuild(CBUpdateCarveAGuild, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackSetNewOwner(CBSetNewOwner, this))
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCallbackSetNextSiegeWarTime(CBSetNextSiegeWarTime, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackAddDefenseApplication(CBAddDefenseApplication, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackRemoveDefenseApplication(CBRemoveDefenseApplication, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackAddAttackApplication(CBAddAttackApplication, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackRemoveAttackApplication(CBRemoveAttackApplication, this))
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCallbackRequestAttackApplGuildList(CBRequestAttackApplGuildList, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackRequestDefenseApplGuildList(CBRequestDefenseApplGuildList, this))
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCallbackRequestAttackGuildList(CBRequestAttackGuildList, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackRequestDefenseGuildList(CBRequestDefenseGuildList, this))
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCallbackAddAttackGuild(CBAddAttackGuild, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackAddDefenseGuild(CBAddDefenseGuild, this))
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCallbackReceiveOpenAttackObject(CBReceiveOpenAttackObject, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackReceiveUseAttackObject(CBReceiveUseAttackObject, this))
		return FALSE;
	if (!m_pcsAgpmSiegeWar->SetCallbackReceiveRepairAttackObject(CBReceiveRepairAttackObject, this))
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCallbackAddItemToAttackObject(CBAddItemToAttackObject, this))
		return FALSE;

	//if (!m_pcsAgpmSiegeWar->SetCallbackRemoveItemToAttackObject(CBRemoveItemToAttackObject, this))
	//	return FALSE;
	
	if (!m_pcsAgpmSiegeWar->SetCallbackCarveASeal(CBCarveASeal, this))
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCallbackUpdateReverseAttack(CBUpdateReverseAttack, this))
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCallbackAddArchlordCastleAttackGuild(CBAddArchlordCastleAttackGuild, this))
		return FALSE;

	if (!m_pcsAgsmDeath->SetCallbackPreProcessCharacterDead(CBDeath, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackActionUseSiegeWarAttackObject(CBUseAttackObject, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackActionCarveASeal(CBActionMoveCarveASeal, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackPayActionCost(CBPayActionCost,this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateSpecialStatus(CBUpdateSpecialStatus, this))
		return FALSE;

	if (!m_pcsAgsmCombat->SetCallbackDamageAdjustSiegeWar(CBNormalDamageAdjustSiegeWar, this))
		return FALSE;
	if (!m_pcsAgsmSkill->SetCallbackDamageAdjustSiegeWar(CBSkillDamageAdjustSiegeWar, this))
		return FALSE;

	if (!m_pcsAgpmPvP->SetCallbackIsFriendSiegeWar(CBIsFriendGuild, this))
		return FALSE;
	if (!m_pcsAgpmPvP->SetCallbackIsEnemySiegeWar(CBIsEnemyGuild, this))
		return FALSE;

	if (!m_pcsAgsmEventSpawn->SetCallbackSpawn(CBSpawnCharacter, this))
		return FALSE;
	if (!m_pcsAgsmEventSpawn->SetCallbackInitReusedCharacter(CBSpawnCharacter, this))
		return FALSE;

	if (!m_pcsAgsmAI2->SetCallbackGetTarget(CBGetTarget, this))
		return FALSE;

	if (!m_pcsAgsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this))
		return FALSE;
	if (!m_pcsAgsmCharManager->SetCallbackSetCharacterGameData(CBSetCharacterGameData, this))
		return FALSE;
	if (!m_pcsAgsmCharManager->SetCallbackCheckLoginPosition(CBCheckLoginPosition, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackMoveChar(CBCharacterCancelCarving, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackActionAttack(CBCharacterCancelCarving, this))
		return FALSE;
	if (!m_pcsAgsmItem->SetCallbackUseTeleportScroll(CBCharacterCancelCarving, this))
		return FALSE;
	if (!m_pcsAgsmItem->SetCallbackUseItemTransform(CBCharacterCancelCarving, this))
		return FALSE;
	if (!m_pcsAgsmSkill->SetCallbackCastSkill(CBCharacterCancelCarving, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackUpdatePosition(CBCharacterCancelCarving, this))
		return FALSE;

	if (!m_pcsAgsmItem->SetCallbackUseItemSkillBook(CBItemCancelCarving, this))
		return FALSE;
	if (!m_pcsAgsmItem->SetCallbackUseItemSkillScroll(CBItemCancelCarving, this))
		return FALSE;
	if (!m_pcsAgsmItem->SetCallbackCheckReturnPosition(CBCheckReturnPosition, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackBindingRegionChange(CBRegionChange, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackAddCharacterToMap(CBAddCharacterToMap, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackRemoveCharacterFromMap(CBRemoveCharacterFromMap, this))
		return FALSE;

	if (!m_pcsAgsmCharacter->SetCallbackSendCharView(CBSendPacketCharacterView, this))
		return FALSE;
	if (!m_pcsAgsmCharacter->SetCallbackSendCharacterAllInfo(CBSendPacketCharacterAllInfo, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackStartSiegeWar(CBStartSiegeWarCommand, this))
		return FALSE;
	if (!m_pcsAgsmAdmin->SetCallbackEndSiegeWar(CBEndSiegeWarCommand, this))
		return FALSE;
	if (!m_pcsAgsmAdmin->SetCallbackEndArchlordBattle(CBEndArchlordBattleCommand, this))
		return FALSE;
	if (!m_pcsAgsmAdmin->SetCallbackSetCastleOwner(CBSetCastleOwner, this))
		return FALSE;
	if (!m_pcsAgsmAdmin->SetCallbackCancelCastleOwner(CBCancelCastleOwner, this))
		return FALSE;

	if (!m_pcsAgpmArchlord->SetCallbackSetArchlord(CBSetArchlord, this))
		return FALSE;
	if (!m_pcsAgpmArchlord->SetCallbackCancelArchlord(CBCancelArchlord, this))
		return FALSE;
	if (!m_pcsAgsmArchlord->SetCallbackStartArchlordSiegeWar(CBStartArchlordSiegeWar, this))
		return FALSE;
	if (!m_pcsAgsmArchlord->SetCallbackEndArchlordSiegeWar(CBEndArchlordSiegeWar, this))
		return FALSE;
	
	return TRUE;
}

BOOL AgsmSiegeWar::OnIdle(UINT32 ulClockCount)
{
	if (!m_bLoaded ||
		m_ulLoadedClockCount == 0 ||
		m_ulLoadedClockCount + 10000 > ulClockCount)
		return TRUE;

	if(m_pcsAgpmConfig->IsEventServer()) return TRUE;//JK_특성화서버

	UINT64	ullCurrentTimeDate	= AuTimeStamp2::GetCurrentTimeStamp();

	//STOPWATCH2(GetModuleName(), _T("OnIdle"));

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

// 	if (0 == (m_ulTickCount4SiegeWarDay % 6) &&			// 1 minute
// 		!m_pcsAgpmSiegeWar->IsDayOfSiegeWar())
// 	{
// 		m_pcsAgpmSiegeWar->SetDayOfSiegeWar();
// 		m_ulTickCount4SiegeWarDay = 0;
// 	}
//	m_ulTickCount4SiegeWarDay++;

	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_OFF)
		{
			if (m_pcsAgpmSiegeWar->IsArchlordCastle(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]))
				SetNextArchlordSiegeWarTime(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]);
			else
				SetNextSiegeWarTime(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]);
		}

		// 공성전이 진행되는 동안 실행되어야 하는 루틴들은 여기서 처리한다. (AGSMSIEGEWAR_IDLE 시간 마다)
		if (m_pcsAgpmSiegeWar->IsStarted(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]))
		{
			SyncEndTime(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i], ulClockCount);

			if ((m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ulIdleTimeData + AGSMSIEGEWAR_IDLE) < ulClockCount)
			{
				m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ulIdleTimeData = ulClockCount;

				// 주인이 없는 성이면 NPC Guild가 소환 된다.
				if (m_pcsAgpmSiegeWar->IsArchlordCastle(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]))
				{
					if (!CheckExistArchlord(ulClockCount))
					{
						DepriveArchlord(&(m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]));
					}
				
					if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_strOwnerGuildName.IsEmpty() &&
						m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_strCarveGuildName.IsEmpty())
						SpawnSiegeWarMonster(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i], AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_NPC_GUILD);
				}
				else
				{
					if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_strOwnerGuildName.IsEmpty())
						SpawnSiegeWarMonster(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i], AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_NPC_GUILD);
				}
			}
		}

		if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_BREAK_A_SEAL &&
			m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_csEyeStatus.m_bOpenEye &&
			m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_csEyeStatus.m_ulOpenTimeMSec > 0 &&
			m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_csEyeStatus.m_ulOpenTimeMSec < ulClockCount)
		{
			// 정해진 시간안에 나머지 눈을 못뜨게 했다.
			// 둘다 감긴걸로 다시 세팅한다.
			////////////////////////////////////////////////////////////////

			m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_csEyeStatus.m_bOpenEye			= FALSE;
			m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_csEyeStatus.m_ulOpenTimeMSec	= 0;

			for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
			{
				if (!m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][j]) break;
				m_pcsAgsmDeath->ResurrectNow(m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][j], TRUE, FALSE);
			}

			// 다시 다 감긴다고 알려준다.
			SendPacketStatusInfo(
					AGPMSIEGE_STATUS_FAILED_ACTIVE_ARCHON_EYE,
					&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i],
					NULL,
					NULL,
					AGPD_SIEGE_MONSTER_TYPE_NONE,
					0,
					0);
		}
		else if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_eCurrentStatus >= AGPD_SIEGE_WAR_STATUS_START &&
			m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_eCurrentStatus <= AGPD_SIEGE_WAR_STATUS_OPEN_EYES)
		{
			UINT32	ulDuration	= m_pcsAgpmSiegeWar->GetDurationMSec(&(m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]));

			if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ulSiegeWarStartTimeMSec + ulDuration < ulClockCount)
			{
				// timeout 시간이 되었다.
				// 공성전을 끝낸다.
				m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ulIdleTimeData = 0;

				TimeOver(i);

				if (!m_pcsAgpmSiegeWar->IsArchlordCastle(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]))
				{
					ResetSiegeWar(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]);
					SetNextSiegeWarTime(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]);
				}
				
			}
		}
		else if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE)
		{
			if (!CheckExistArchlord(ulClockCount))
			{
				DepriveArchlord(&(m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]));
				SpawnBossMob(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]);
			}
		
			UINT32	ulDuration	= m_pcsAgpmSiegeWar->GetDurationMSec(&(m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]));
			if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ulSiegeWarStartTimeMSec + ulDuration < ulClockCount)
			{
				// timeout 시간이 되었다.
				// 아크로드 결정전을 끝낸다.
				m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ulIdleTimeData = 0;

				AuLogFile_s("LOG\\SiegeWarLog", "Archlord Battle Time is over!!");

				ProcessFailedArchlordBattle();

				ResetSiegeWar(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]);

				SetNextArchlordSiegeWarTime(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]);
			}
		}

		// 주기적으로(현재 5분) 공성 상태를 DB에 저장한다.
		if (m_aulLastDBUpdateTime[i] + AGSMSIEGEWAR_DB_UPDATE_INTERVAL < ulClockCount)
		{
			UpdateToDB(i);
		}

		// 공성이 끝난 이후 다음 공성 세팅을 한다.
		if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_OFF &&
			m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ullProclaimTimeDate > 0)
		{
			if (ullCurrentTimeDate >= m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ullProclaimTimeDate)
			{
				// 공성 선포 시간이 지났다.
				// 성주가 선포를 안한 경우다. 임의로 시간을 확정해버린다.
				ConfirmSiegeWarTime(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]);
			}
		}
		else if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR &&
			m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ullApplicationTimeDate > 0)
		{
			if (ullCurrentTimeDate >= m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ullApplicationTimeDate)
			{
				// 신청 시간이 지났다.

				// 공성, 수성 길드를 확정한다.
				ConfirmGuild(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]);
			}
		}
		else if (m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_READY &&
			m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ullNextSiegeWarTimeDate > 0)
		{
			SyncStartTime(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i], ullCurrentTimeDate);

			if (ullCurrentTimeDate >= m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ullNextSiegeWarTimeDate)
			{
				// idle 시간 설정
				m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_ulIdleTimeData = ulClockCount;
				
				// 2007.02.12 laki 간혹 성주길드가 수성길드에 안들어간 경우가 있다. 우선 땜빵으로... -_-;
				AgpdGuild *pcsGuild	= m_pcsAgpmGuild->GetGuild(m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_strOwnerGuildName.GetBuffer());
				if (pcsGuild)
					m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_csDefenseGuild.AddObject((PVOID) &pcsGuild, pcsGuild->m_szID);
				
				// 이제 공성 시작이다.
				StartSiegeWar(m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_apcsRegionTemplate[0]);
			}
		}

		// 각인하고 있는 놈이 있다면, 시간체크해서 각인 완료해준다.
		if ((i == AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX && m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_START ||
			 i != AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX && m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_OPEN_EYES) &&
			m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0])
		{
			AgpdSiegeWarADCharacter	*pcsAttachData	= m_pcsAgpmSiegeWar->GetAttachCharacterData(m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]);

			if (pcsAttachData->m_strCarveGuildName.GetLength() > 0 && pcsAttachData->m_ulStartCarveTimeMSec > 0)
			{
				AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pcsAttachData->m_strCarveGuildName.GetBuffer());
				if(pcsGuild)
				{
					// 각인중 길드 마스터가 접속이 끊겼다.
					AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacter(pcsGuild->m_szMasterID);
					if(!pcsCharacter)
					{
						AgpdSiegeWar *pcsSiegeWar = &m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i];

						m_pcsAgpmSiegeWar->CancelCarveASeal(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]);

						//SendPacketCarveASeal(pcsSiegeWar, AGPMSIEGE_RESULT_CANCEL_CARVE_A_SEAL, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

						SendPacketStatusInfo(AGPMSIEGE_STATUS_CANCEL_CARVE_GUILD,
							pcsSiegeWar,
							pcsGuild->m_szID,
							pcsGuild->m_szMasterID,
							AGPD_SIEGE_MONSTER_TYPE_NONE,
							0,
							0);

						return TRUE;
					}

					if(pcsAttachData->m_ulStartCarveTimeMSec + m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_csTemplate.m_ulNeedTimeForCarveMSec < ulClockCount)
					{
						// 각인 완료를 알려준다.
						AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacter(pcsGuild->m_szMasterID);
						if (pcsCharacter)
						{
							m_pcsAgpmSiegeWar->EndCarveASeal(pcsCharacter, m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]);
							CarveASeal(pcsCharacter);

							SendPacketCarveASeal(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i], AGPMSIEGE_RESULT_END_CARVE_A_SEAL, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
						}
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL AgsmSiegeWar::SyncStartTime(AgpdSiegeWar *pcsSiegeWar, UINT64 ullCurrentTimeDate)
{
	if (!pcsSiegeWar || pcsSiegeWar->m_ullNextSiegeWarTimeDate == 0)
		return FALSE;

	UINT64	ullRemainTimeSec	= (pcsSiegeWar->m_ullNextSiegeWarTimeDate - ullCurrentTimeDate) / 10000000; // 초단위로 구한다.

	int i = 0;
	for ( ; i < 19 ; ++i)
	{
		if (g_aulSyncTimeSec[i] < ullRemainTimeSec)
			break;
	}

	if (i <= 0)
		return TRUE;

	if (g_aulSyncTimeSec[i - 1] == pcsSiegeWar->m_ulLastSyncStartTimeSec)
		return TRUE;

	pcsSiegeWar->m_ulLastSyncStartTimeSec	= g_aulSyncTimeSec[i - 1];

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketSyncMessage(AGPMSIEGE_MESSAGE_SYNC_START_TIME, (UINT16) g_aulSyncTimeSec[i - 1], &nPacketLength);
	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
	{
		SendPacketToSiegeWar(pvPacket, nPacketLength, pcsSiegeWar);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::SyncEndTime(AgpdSiegeWar *pcsSiegeWar, UINT32 ulClockCount)
{
	if (!pcsSiegeWar)
		return FALSE;

	UINT32	ulDuration		= m_pcsAgpmSiegeWar->GetDurationMSec(pcsSiegeWar);
	UINT32	ulRemainTimeSec	= ((pcsSiegeWar->m_ulSiegeWarStartTimeMSec + ulDuration) - ulClockCount) / 1000;

	int i = 0;
	for ( ; i < 19 ; ++i)
	{
		if (g_aulSyncTimeSec[i] < ulRemainTimeSec)
			break;
	}

	if (i <= 0)
		return TRUE;

	if (g_aulSyncTimeSec[i - 1] == pcsSiegeWar->m_ulLastSyncEndTimeSec)
		return TRUE;

	pcsSiegeWar->m_ulLastSyncEndTimeSec	= g_aulSyncTimeSec[i - 1];

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketSyncMessage(AGPMSIEGE_MESSAGE_SYNC_END_TIME, (UINT16) g_aulSyncTimeSec[i - 1], &nPacketLength);
	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
	{
		SendPacketToSiegeWar(pvPacket, nPacketLength, pcsSiegeWar);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CBServerConnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return TRUE;

	AgsmSiegeWar *pThis = (AgsmSiegeWar *) pClass;
	AgsdServer *pcsServer = (AgsdServer *) pData;

	//AgsdServer *pcsServerRelay = pThis->m_pcsAgsmServerManager->GetRelayServer();
	//if (!pcsServerRelay)
	//	return FALSE;

	//if (pcsServer == pcsServerRelay)
	//	pThis->LoadFromDB();

	return TRUE;
}

BOOL AgsmSiegeWar::CBUpdateSpecialStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	//UINT64			*pulOldStatus	= (UINT64 *)		pCustData;

	PVOID *pStatus		= (PVOID *) pCustData;

	UINT64* pulOldStatus = (UINT64 *)pStatus[0];
	INT32*  pReserved    = (INT32 *)pStatus[1];

	if ((*pulOldStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE) &&
		!(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE))
	{
		switch (pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter)) {
			case AGPD_SIEGE_MONSTER_LIFE_TOWER:
				pThis->ActiveLifeTower(pcsCharacter);
				break;

			case AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER:
				pThis->ActiveDefenseResurrectionTower(pcsCharacter);
				break;

			case AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER:
				pThis->ActiveAttackResurrectionTower(pcsCharacter);
				break;
		}
	}
	else if (!(*pulOldStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE) &&
		(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE))
	{
		switch (pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter)) {
			case AGPD_SIEGE_MONSTER_LIFE_TOWER:
				pThis->DisableLifeTower(pcsCharacter);
				break;

			case AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER:
				pThis->DisableDefenseResurrectionTower(pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter));
				break;

			case AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER:
				pThis->DisableAttackResurrectionTower(pcsCharacter);
				break;
		}
	}

	return TRUE;
}



BOOL AgsmSiegeWar::CBDeath(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	pThis->CancelCarving(pcsCharacter);

	// pcsCharacter 가 공성 관련 몬스터 중 한놈인지 보고 그렇다면 해당 몬스터 타입에 따라 공성 진행 상태를 변경한다.
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);

	BOOL	bResult	= TRUE;

	AgpdSiegeWarMonsterType	eMonsterType = pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter);
	switch (eMonsterType)	{
		case AGPD_SIEGE_MONSTER_LIFE_TOWER:
			// 생명의 탑이 부서졌다.
			bResult	= pThis->DisableLifeTower(pcsCharacter);
			pThis->SendPacketStatusInfo(
						AGPMSIEGE_STATUS_DESTROY_OBJECT,
						pcsSiegeWar,
						NULL,
						NULL,
						AGPD_SIEGE_MONSTER_LIFE_TOWER,
						0,
						0);
			break;

		case AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER:
			// 부활의 탑이 부서졌다.
			bResult	= pThis->DisableDefenseResurrectionTower(pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter));
			pThis->SendPacketStatusInfo(
						AGPMSIEGE_STATUS_DESTROY_OBJECT,
						pcsSiegeWar,
						NULL,
						NULL,
						AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER,
						0,
						0);
			break;

		case AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER:
			//bResult	= pThis->DisableAttackResurrectionTower(pcsCharacter);
			pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISABLE);
			pThis->m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_USE);

			pThis->SendPacketStatusInfo(
						AGPMSIEGE_STATUS_DESTROY_OBJECT,
						pcsSiegeWar,
						NULL,
						NULL,
						AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER,
						0,
						0);
			break;

		case AGPD_SIEGE_MONSTER_THRONE:
			// 봉인성좌의 봉인이 풀렸다.
			bResult	= pThis->BreakASeal(pThis->m_pcsApmMap->GetTemplate(pThis->m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z)));
			break;

		case AGPD_SIEGE_MONSTER_ARCHON_EYES:
			// 아콘의 눈이 떠졌다.
			bResult	= pThis->OpenEyes(pThis->m_pcsApmMap->GetTemplate(pThis->m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z)), pcsCharacter);
			break;

		case AGPD_SIEGE_MONSTER_INNER_GATE:
		case AGPD_SIEGE_MONSTER_OUTER_GATE:
			pThis->SendPacketStatusInfo(
				AGPMSIEGE_STATUS_DESTROY_OBJECT,
				pcsSiegeWar,
				NULL,
				NULL,
				eMonsterType,
				0,
				0);
			break;

		case AGPD_SIEGE_MONSTER_CATAPULT:
			pThis->m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_USE);
			pThis->SendPacketStatusInfo(
						AGPMSIEGE_STATUS_DESTROY_OBJECT,
						pcsSiegeWar,
						NULL,
						NULL,
						eMonsterType,
						0,
						0);
			break;

		case AGPD_SIEGE_MONSTER_TYPE_NONE:
			{
				// 아크로드나 디카인, 아크로드 공성 길드마스터가 죽은 경우 처리한다.
				AgpdCharacter	*pcsArchlord	= pThis->m_pcsAgpmSiegeWar->GetArchlord();
				if (pcsArchlord == pcsCharacter)
				{
					// 일단 어디서든 죽고나면 아크로드가 아니다.
					pThis->DepriveArchlord(pcsSiegeWar);

					// 공성중에 죽었다면, 아크로드 결정전이 끝나고 새로운 아크로드가 탄생한 것이다.
					// 처리해준다.
					if (pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar))
					{
						// 2 step의 경우 그냥 아크로드 박탈만하고 진행한다.(디카인 출현)
						if ( pcsSiegeWar->m_eCurrentStatus >= AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE)
						{
							pThis->ProcessNewArchlord(pcsSiegeWar);
							pThis->ResetSiegeWar(pcsSiegeWar);
							pThis->SetNextArchlordSiegeWarTime(pcsSiegeWar);
							pThis->m_pcsAgsmArchlord->SetStepTime(pThis->GetClockCount() - AGSMARCHLORD_STEP_ARCHLORD_TIME);
						}
					}
				}
				else if (pcsCharacter->m_lTID1 == 1366 &&
					pThis->m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar) &&
					pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar))
				{
					// 디카인이 아크로드 성에서 공성중에 죽었다.
					// 아크로드 결정전이 끝나고 새로운 아크로드가 탄생했다.
					pThis->ProcessNewArchlord(pcsSiegeWar);
					pThis->ResetSiegeWar(pcsSiegeWar);
					pThis->SetNextArchlordSiegeWarTime(pcsSiegeWar);
					pThis->m_pcsAgsmArchlord->SetStepTime(pThis->GetClockCount() - AGSMARCHLORD_STEP_ARCHLORD_TIME);
				}
				// 아크로드 결정전 중 공성 길드마스터가 죽은 경우 처리한다.
				// 이런 경우는 아크로드 결정전에 실패한 것이다.
				else if (pThis->m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar) &&
					pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar))
				{
					AgpdGuild	*pcsCarveGuild	= pThis->m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strCarveGuildName);

					if ( pcsCarveGuild )
					{
						if( pcsCarveGuild == pThis->m_pcsAgpmGuild->GetGuild(pcsCharacter) )
						{
							if(pThis->m_pcsAgpmGuild->IsMaster(pcsCarveGuild, pcsCharacter->m_szID))
							{
								pThis->ProcessFailedArchlordBattle();
								pThis->ResetSiegeWar(pcsSiegeWar);
								pThis->SetNextArchlordSiegeWarTime(pcsSiegeWar);
							}
						}
						pThis->m_pcsAgsmArchlord->SetStepTime(pThis->GetClockCount() - AGSMARCHLORD_STEP_ARCHLORD_TIME);
					}
				}
			}
			break;

		default:
			break;
	}

	if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE || eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return TRUE;

	INT32	lObjectIndex	= (-1);

	for (int i = 0; i < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++i)
	{
		if (pcsSiegeWar && pcsSiegeWar->m_apcsObjectChar[eMonsterType][i] == pcsCharacter)
		{
			lObjectIndex	= eMonsterType * 1000 + i;
			break;
		}
	}

	if (lObjectIndex > (-1))
		pThis->UpdateSiegeWarObjectToDB(pcsSiegeWar, pcsCharacter, lObjectIndex);

	return bResult;
}

BOOL AgsmSiegeWar::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
	if (pcsSiegeWar && 
		pcsSiegeWar->m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_OPEN_EYES &&
		pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0])
	{
		AgpdSiegeWarADCharacter	*pcsAttachData	= pThis->m_pcsAgpmSiegeWar->GetAttachCharacterData(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]);

		if (pcsAttachData->m_strCarveGuildName.GetLength() > 0 &&
			pcsAttachData->m_ulStartCarveTimeMSec > 0)
		{
			AgpdGuild	*pcsGuild	= pThis->m_pcsAgpmGuild->GetGuild(pcsCharacter);
			if (pcsGuild &&
				strcmp(pcsGuild->m_szID, pcsAttachData->m_strCarveGuildName.GetBuffer()) == 0)
			{
				pThis->CancelCarving(pcsCharacter);
			}
		}
	}


	pThis->RemoveSiegeWarObjectChar(pcsCharacter);

	return TRUE;
}

BOOL AgsmSiegeWar::CBUseAttackObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgpdCharacterAction	*pstAction	= (AgpdCharacterAction *) pCustData;

	AgpdCharacter	*pcsTarget		= pThis->m_pcsAgpmCharacter->GetCharacterLock(pstAction->m_csTargetBase.m_lID);
	if (!pcsTarget)
		return FALSE;

	// 그동안 딴놈이 사용중인지모르니 함 체크해본다.
	AgpmSiegeResult	eResult	= pThis->m_pcsAgpmSiegeWar->CheckOpenAttackObjectEvent(pcsCharacter, pcsTarget);

	pThis->SendResponseUseAttackObject(pcsCharacter, pcsTarget, eResult, TRUE);

	pcsTarget->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmSiegeWar::CBActionMoveCarveASeal(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgpdCharacterAction	*pstAction	= (AgpdCharacterAction *) pCustData;

	AgpdCharacter	*pcsTarget		= pThis->m_pcsAgpmCharacter->GetCharacterLock(pstAction->m_csTargetBase.m_lID);
	if (!pcsTarget)
		return FALSE;

	pThis->ProcessCarveASeal(pcsCharacter, pcsTarget);

	pcsTarget->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmSiegeWar::CBPayActionCost(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	INT_PTR			*pnActionType	= (INT_PTR *)	pCustData;

	if (pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter) != AGPD_SIEGE_MONSTER_CATAPULT)
		return TRUE;

	if (*((INT16 *) pnActionType) != AGPDCHAR_ACTION_TYPE_ATTACK)
		return TRUE;

	pThis->m_pcsAgpmSiegeWar->PayCostForUse(pcsCharacter);

	return TRUE;
}

BOOL AgsmSiegeWar::ActiveLifeTower(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	// 생명탑을 활성화 시킨다.
	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsCharacter->m_stPos));
	if (!pcsSiegeWar)
		return FALSE;

	pcsSiegeWar->m_bIsActiveLifeTower	= TRUE;

	// 해당 범위 스킬을 시작시킨다.





	return TRUE;
}

BOOL AgsmSiegeWar::ActiveAttackResurrectionTower(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	// 부활탑을 활성화 시킨다.
	// 움,,, 암튼 활성화 시킨다.

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsCharacter->m_stPos));
	if (!pcsSiegeWar)
		return FALSE;

	pcsSiegeWar->m_bIsActiveAttackResurrection	= TRUE;

	if (pcsSiegeWar->m_eCurrentStatus >= AGPD_SIEGE_WAR_STATUS_START &&
		pcsSiegeWar->m_eCurrentStatus <= AGPD_SIEGE_WAR_STATUS_OPEN_EYES)
	{
		SendPacketCastleInfoToAll(pcsSiegeWar);

		if (pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0] &&
			pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0] &&
			pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][1])
		{
			BOOL	bLeft	= AuMath::ConsiderLeftPoint(
														pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]->m_stPos,
														pcsCharacter->m_stPos,
														(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0] == pcsCharacter) ? pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][1]->m_stPos : pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0]->m_stPos);

			SendPacketStatusInfo(
					(bLeft) ? AGPMSIEGE_STATUS_ACTIVE_LEFT_ATTACK_RES_TOWER : AGPMSIEGE_STATUS_ACTIVE_RIGHT_ATTACK_RES_TOWER,
					pcsSiegeWar,
					NULL,
					NULL,
					AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER,
					0,
					0);
		}

		m_pcsAgpmSiegeWar->PayCostForUse(pcsCharacter);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::ActiveDefenseResurrectionTower(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	// 부활탑을 활성화 시킨다.
	// 움,,, 암튼 활성화 시킨다.

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsCharacter->m_stPos));
	if (!pcsSiegeWar)
		return FALSE;

	pcsSiegeWar->m_bIsActiveDefenseResurrection	= TRUE;

	SendPacketCastleInfoToAll(pcsSiegeWar);

	return TRUE;
}

BOOL AgsmSiegeWar::DisableLifeTower(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsCharacter->m_stPos));
	if (!pcsSiegeWar)
		return FALSE;

	pcsSiegeWar->m_bIsActiveLifeTower	= FALSE;

	// 생명탑의 스킬을 중지 시킨다.




	return TRUE;
}

BOOL AgsmSiegeWar::DisableAttackResurrectionTower(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	// 부활탑이 깨졌다.
	// 부활 기능을 정지시킨다.

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsCharacter->m_stPos));
	if (!pcsSiegeWar)
		return FALSE;

	if (!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0] ||
		!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0] ||
		!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][1])
		return FALSE;

	if (pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0]->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE &&
		pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][1]->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE)
		pcsSiegeWar->m_bIsActiveAttackResurrection	= FALSE;

	if (pcsSiegeWar->m_eCurrentStatus >= AGPD_SIEGE_WAR_STATUS_START &&
		pcsSiegeWar->m_eCurrentStatus <= AGPD_SIEGE_WAR_STATUS_OPEN_EYES)
	{
		SendPacketCastleInfoToAll(pcsSiegeWar);

		if (pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0] &&
			pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0] &&
			pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][1])
		{
			BOOL	bLeft	= AuMath::ConsiderLeftPoint(
														pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]->m_stPos,
														pcsCharacter->m_stPos,
														(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0] == pcsCharacter) ? pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][1]->m_stPos : pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0]->m_stPos);

			SendPacketStatusInfo(
					(bLeft) ? AGPMSIEGE_STATUS_DISABLE_LEFT_ATTACK_RES_TOWER : AGPMSIEGE_STATUS_DISABLE_RIGHT_ATTACK_RES_TOWER,
					pcsSiegeWar,
					NULL,
					NULL,
					AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER,
					0,
					0);
		}
	}

	return TRUE;
}

BOOL AgsmSiegeWar::DisableDefenseResurrectionTower(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	// 부활탑이 깨졌다.
	// 부활 기능을 정지시킨다.

	pcsSiegeWar->m_bIsActiveDefenseResurrection	= FALSE;

	SendPacketCastleInfoToAll(pcsSiegeWar);

	return TRUE;
}

BOOL AgsmSiegeWar::CBUpdateSiegeWarStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmSiegeWar	*pThis				= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWarData	= (AgpdSiegeWar *)	pData;

	BOOL	bResult	= TRUE;

	switch (pcsSiegeWarData->m_eCurrentStatus) {
		case AGPD_SIEGE_WAR_STATUS_OFF:
			pcsSiegeWarData->m_ullProclaimTimeDate		= pcsSiegeWarData->m_ullPrevSiegeWarTimeDate + (UINT64) pcsSiegeWarData->m_csTemplate.m_ulProclaimTimeHour * 60 * 60 * 10000000;
			pcsSiegeWarData->m_ullApplicationTimeDate	= 0;
			pcsSiegeWarData->m_ullNextSiegeWarTimeDate	= 0;
			break;

		case AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR:
			break;

		case AGPD_SIEGE_WAR_STATUS_READY:
			{
				pcsSiegeWarData->m_ulLastSyncStartTimeSec	= 0;

				INT16	nTotalPage		= (pcsSiegeWarData->m_csAttackGuild.GetObjectCount() / AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE) + 1;

				for(int i=0; i < nTotalPage; i++)
				{
					INT16	nPacketLength	= 0;
					PVOID	pvPacket		= NULL;

					pvPacket				= pThis->m_pcsAgpmSiegeWar->MakePacketAttackGuildList(pcsSiegeWarData, i, &nPacketLength);

					if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
						pThis->SendPacketAllUser(pvPacket, nPacketLength, PACKET_PRIORITY_2);
				}

				INT16 nPacketLength	= 0;
				PVOID pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketDefenseGuildList(pcsSiegeWarData, &nPacketLength);

				if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
					pThis->SendPacketAllUser(pvPacket, nPacketLength, PACKET_PRIORITY_2);
			}
			break;

		case AGPD_SIEGE_WAR_STATUS_START:
			bResult	= pThis->ProcessStatusStart(pcsSiegeWarData);
			pThis->SendPacketStatusInfo(
					AGPMSIEGE_STATUS_START,
					pcsSiegeWarData,
					NULL,
					NULL,
					AGPD_SIEGE_MONSTER_TYPE_NONE,
					0,
					0);
			break;

		case AGPD_SIEGE_WAR_STATUS_BREAK_A_SEAL:
			bResult = pThis->ProcessStatusBreakASeal(pcsSiegeWarData);
			pThis->SendPacketStatusInfo(
					AGPMSIEGE_STATUS_RELEASE_THRONE,
					pcsSiegeWarData,
					NULL,
					NULL,
					AGPD_SIEGE_MONSTER_TYPE_NONE,
					0,
					0);
			break;

		case AGPD_SIEGE_WAR_STATUS_OPEN_EYES:
			bResult = pThis->ProcessStatusOpenEyes(pcsSiegeWarData);
			pThis->SendPacketStatusInfo(
					AGPMSIEGE_STATUS_ACTIVE_ARCHON_EYE,
					pcsSiegeWarData,
					NULL,
					NULL,
					AGPD_SIEGE_MONSTER_TYPE_NONE,
					0,
					2);
			break;

		case AGPD_SIEGE_WAR_STATUS_TIME_OVER:
			bResult	= pThis->ProcessStatusTimeOver(pcsSiegeWarData);
			pThis->SendPacketStatusInfo(
					AGPMSIEGE_STATUS_TERMINATE,
					pcsSiegeWarData,
					NULL,
					NULL,
					AGPD_SIEGE_MONSTER_TYPE_NONE,
					0,
					0);
			break;

		case AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE:
			bResult	= pThis->ProcessStatusArchlordBattle(pcsSiegeWarData);
			break;

		case AGPD_SIEGE_WAR_STATUS_TIME_OVER_ARCHLORD_BATTLE:
			bResult	= pThis->ProcessStatusTimeOverArchlordBattle(pcsSiegeWarData);
			break;
	}

	// 월드내 모든 유저들에게 스테이터스 정보를 보낸다.
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketSiegeWarStatus(pcsSiegeWarData, &nPacketLength);

	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
	{
		pThis->SendPacketAllUser(pvPacket, nPacketLength, PACKET_PRIORITY_2);
	}

	if (pcsSiegeWarData->m_bIsNeedInsertDB)
		pThis->UpdateToDB(pcsSiegeWarData);

	return bResult;
}

BOOL AgsmSiegeWar::CBUpdateCarveAGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	INT32			lSiegeWarInfoIndex	= *(INT32 *)	pData;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(lSiegeWarInfoIndex);
	if (!pcsSiegeWar)
		return FALSE;

	AgpdGuild		*pcsGuild		= pThis->m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strCarveGuildName.GetBuffer());
	if (!pcsGuild)
		return FALSE;

	// 각인 길드 정보를 보내준다.
	pThis->SendPacketStatusInfo(
					AGPMSIEGE_STATUS_NEW_CARVE_GUILD,
					pcsSiegeWar,
					pcsSiegeWar->m_strCarveGuildName.GetBuffer(),
					pcsGuild->m_szMasterID,
					AGPD_SIEGE_MONSTER_TYPE_NONE,
					0,
					0);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketCarveGuildInfo(pcsSiegeWar, &nPacketLength);

	if (pvPacket && nPacketLength >= sizeof(PACKET_HEADER))
	{
		pThis->SendPacketAllUser(pvPacket, nPacketLength);
	}

	pThis->UpdateToDB(lSiegeWarInfoIndex);

	return TRUE;
}

BOOL AgsmSiegeWar::CBSetNewOwner(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pData;

	// 2007.01.18. steeple
	pThis->SendPacketCastleInfoToAll(pcsSiegeWar);

	pThis->UpdateToDB(pcsSiegeWar);

	return TRUE;
}

BOOL AgsmSiegeWar::CBSetNextSiegeWarTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pData;
	
	PVOID			*ppvBuffer		= (PVOID *)	pCustData;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	AgpmSiegeResult	eResult			= (AgpmSiegeResult) PtrToInt(ppvBuffer[1]);

	if (eResult == AGPMSIEGE_RESULT_SET_NEXT_SIEGE_WAR_TIME_SUCCESS)
	{
		pThis->ConfirmSiegeWarTime(pcsSiegeWar);
		pThis->UpdateToDB(pcsSiegeWar);
	}

	return pThis->SendResult(pcsSiegeWar, pcsCharacter, eResult);
}

BOOL AgsmSiegeWar::CBAddDefenseApplication(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pData;
	
	PVOID			*ppvBuffer		= (PVOID *)	pCustData;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	AgpmSiegeResult	eResult			= (AgpmSiegeResult)	PtrToInt(ppvBuffer[1]);

	if (AGPMSIEGE_RESULT_DEFENSE_APPL_SUCCESS == eResult)
	{
		// DB
		CHAR *pszCastle = pcsSiegeWar->m_strCastleName.GetBuffer();		// CASTLEID
		AgpdGuild *pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(pcsCharacter);
		CHAR *pszGuild = pcsGuild ? pcsGuild->m_szID : NULL;	// GUILDID
		UINT32	ulSiegeID	= pcsSiegeWar->m_ulSiegeDBID;
		INT32	lSide		= 2;						// SIDE		
		UINT64	ullApplyDate = 0;
		AgpdGuildAttachData	*pcsAttachGuildData	= pThis->m_pcsAgpmSiegeWar->GetAttachGuildData(pcsGuild);
		if (pcsAttachGuildData)
			ullApplyDate = pcsAttachGuildData->m_ullApplDate;				// APPLYDATE
		INT32	lAdopted	= 0;						// ADOPTED		(1 : 선택된 길드)
			
		PVOID pvBuffer[5];
		pvBuffer[0] = pszGuild;
		pvBuffer[1] = &ulSiegeID;
		pvBuffer[2] = &lAdopted;
		pvBuffer[3] = &lSide;
		pvBuffer[4] = &ullApplyDate;

		// insert
		pThis->EnumCallback(AGSMSIEGEWAR_CB_DB_INSERT_SIEGE_APPLICATION, pszCastle, pvBuffer);
	}
	
	return pThis->SendResult(pcsSiegeWar, pcsCharacter, eResult);
}

BOOL AgsmSiegeWar::CBRemoveDefenseApplication(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pData;
	
	PVOID			*ppvBuffer		= (PVOID *)	pCustData;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	AgpmSiegeResult	eResult			= (AgpmSiegeResult)	PtrToInt(ppvBuffer[1]);

	return pThis->SendResult(pcsSiegeWar, pcsCharacter, eResult);
}

BOOL AgsmSiegeWar::CBAddAttackApplication(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pData;
	
	PVOID			*ppvBuffer		= (PVOID *)	pCustData;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	AgpmSiegeResult	eResult			= (AgpmSiegeResult)	PtrToInt(ppvBuffer[1]);

	if (AGPMSIEGE_RESULT_ATTACK_APPL_SUCCESS == eResult)
	{
		// DB
		CHAR *pszCastle = pcsSiegeWar->m_strCastleName.GetBuffer();		// CASTLEID
		AgpdGuild *pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(pcsCharacter);
		CHAR *pszGuild = pcsGuild ? pcsGuild->m_szID : NULL;	// GUILDID
		UINT32	ulSiegeID	= pcsSiegeWar->m_ulSiegeDBID;
		INT32	lSide		= 1;						// SIDE		
		UINT64	ullApplyDate = 0;
		AgpdGuildAttachData	*pcsAttachGuildData	= pThis->m_pcsAgpmSiegeWar->GetAttachGuildData(pcsGuild);
		if (pcsAttachGuildData)
			ullApplyDate = pcsAttachGuildData->m_ullApplDate;				// APPLYDATE
		INT32	lAdopted	= 0;						// ADOPTED		(1 : 선택된 길드)
			
		PVOID pvBuffer[5];
		pvBuffer[0] = pszGuild;
		pvBuffer[1] = &ulSiegeID;
		pvBuffer[2] = &lAdopted;
		pvBuffer[3] = &lSide;
		pvBuffer[4] = &ullApplyDate;

		// insert
		pThis->EnumCallback(AGSMSIEGEWAR_CB_DB_INSERT_SIEGE_APPLICATION, pszCastle, pvBuffer);
	}

	pThis->SendAddAttackApplicationResult(pcsSiegeWar, pcsCharacter);

	return pThis->SendResult(pcsSiegeWar, pcsCharacter, eResult);
}

BOOL AgsmSiegeWar::CBRemoveAttackApplication(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pData;
	
	PVOID			*ppvBuffer		= (PVOID *)	pCustData;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	AgpmSiegeResult	eResult			= (AgpmSiegeResult)	PtrToInt(ppvBuffer[1]);

	return pThis->SendResult(pcsSiegeWar, pcsCharacter, eResult);
}

BOOL AgsmSiegeWar::CBAddArchlordCastleAttackGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis		= (AgsmSiegeWar *)	pClass;
	AgpdGuild		*pcsGuild	= (AgpdGuild *)		pData;
	
	// DB
	CHAR *pszCastle = pThis->m_pcsAgpmSiegeWar->GetArchlordCastle()->m_strCastleName.GetBuffer();		// CASTLEID
	UINT32	ulSiegeID	= pThis->m_pcsAgpmSiegeWar->GetArchlordCastle()->m_ulSiegeDBID;
	INT32	lSide		= 1;						// SIDE		
	UINT64	ullApplyDate = 0;
	AgpdGuildAttachData	*pcsAttachGuildData	= pThis->m_pcsAgpmSiegeWar->GetAttachGuildData(pcsGuild);
	if (pcsAttachGuildData)
		ullApplyDate = pcsAttachGuildData->m_ullApplDate;				// APPLYDATE
	INT32	lAdopted	= 1;						// ADOPTED		(1 : 선택된 길드)

	PVOID pvBuffer[5];
	pvBuffer[0] = pcsGuild->m_szID;
	pvBuffer[1] = &ulSiegeID;
	pvBuffer[2] = &lAdopted;
	pvBuffer[3] = &lSide;
	pvBuffer[4] = &ullApplyDate;

	// insert
	pThis->EnumCallback(AGSMSIEGEWAR_CB_DB_INSERT_SIEGE_APPLICATION, pszCastle, pvBuffer);

	return TRUE;
}

BOOL AgsmSiegeWar::CBRequestAttackApplGuildList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	INT16			nPage	= *(INT16 *)	pData;
	PVOID			*ppvBuffer	= (PVOID *)	pCustData;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	ppvBuffer[1];

	if (!pcsCharacter || !pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketAttackApplGuildList(pcsSiegeWar, nPage, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	pThis->m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmSiegeWar::CBRequestDefenseApplGuildList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	INT16			nPage	= *(INT16 *)	pData;
	PVOID			*ppvBuffer	= (PVOID *)	pCustData;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	ppvBuffer[1];

	if (!pcsCharacter || !pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketDefenseApplGuildList(pcsSiegeWar, nPage, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	pThis->m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmSiegeWar::CBRequestAttackGuildList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pCustData;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pData;

	if (!pcsCharacter || !pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;

	INT16	nTotalPage		= (pcsSiegeWar->m_csAttackGuild.GetObjectCount() / AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE) + 1;
	for(int i=0; i < nTotalPage; i++)
	{
		PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketAttackGuildList(pcsSiegeWar, i, &nPacketLength);

		if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
			return FALSE;

		pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

		pThis->m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);
	}

	return	TRUE;
}

BOOL AgsmSiegeWar::CBRequestDefenseGuildList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pCustData;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pData;

	if (!pcsCharacter || !pcsSiegeWar)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketDefenseGuildList(pcsSiegeWar, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	pThis->m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

AttackType AgsmSiegeWar::GetAttackType(AgpdCharacter *pcsAttacker)
{
	if (!pcsAttacker)
		return AGSMSIEGEWAR_ATTACK_TYPE_NONE;

	// 원거리, 근거리, 공성전용무기 공격인지 알아낸다.
	//	원거리			: AGSMSIEGEWAR_ATTACK_TYPE_RANGE
	//	근거리			: AGSMSIEGEWAR_ATTACK_TYPE_MELEE
	//	공성전용무기	: AGSMSIEGEWAR_ATTACK_TYPE_SIEGE_WEAPON		(투석기, 공성무기, 공성몬스터)

	AgpdItem	*pcsWeapon	= m_pcsAgpmItem->GetEquipWeapon(pcsAttacker);
	if (!pcsWeapon)
		return AGSMSIEGEWAR_ATTACK_TYPE_MELEE;	// 맨손이니 근거리

	if (pcsWeapon->m_pcsItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP)
		return AGSMSIEGEWAR_ATTACK_TYPE_NONE;

	if (((AgpdItemTemplateEquip *) pcsWeapon->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_WEAPON)
		return AGSMSIEGEWAR_ATTACK_TYPE_NONE;

	switch (((AgpdItemTemplateEquipWeapon *) pcsWeapon->m_pcsItemTemplate)->m_nWeaponType) {
		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW:
		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW:
		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STAFF:
		case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_TROPHY:
		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WAND:
		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_KATARIYA:
		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHAKRAM:
			return AGSMSIEGEWAR_ATTACK_TYPE_RANGE;
			break;

		default:
			return AGSMSIEGEWAR_ATTACK_TYPE_MELEE;
			break;
	}

	return AGSMSIEGEWAR_ATTACK_TYPE_SIEGE_WEAPON;
}

AttackType AgsmSiegeWar::GetAttackType(AgpdSkill *pcsSkill)
{
	if (!pcsSkill)
		return AGSMSIEGEWAR_ATTACK_TYPE_NONE;

	// 원거리, 근거리, 공성전용무기 공격인지 알아낸다.
	//	원거리			: AGSMSIEGEWAR_ATTACK_TYPE_RANGE
	//	근거리			: AGSMSIEGEWAR_ATTACK_TYPE_MELEE
	//	공성전용무기	: AGSMSIEGEWAR_ATTACK_TYPE_SIEGE_WEAPON		(투석기, 공성무기, 공성몬스터)

	INT32	lSkillLevel	= m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
	if (lSkillLevel < 1 || lSkillLevel >= AGPMSKILL_MAX_SKILL_CAP)
		return AGSMSIEGEWAR_ATTACK_TYPE_NONE;

	if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_TYPE2][lSkillLevel] != 1)
		return AGSMSIEGEWAR_ATTACK_TYPE_NONE;

	if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_RANGE][lSkillLevel] <= 500)
		return AGSMSIEGEWAR_ATTACK_TYPE_MELEE;
	else
		return AGSMSIEGEWAR_ATTACK_TYPE_RANGE;

	return AGSMSIEGEWAR_ATTACK_TYPE_SIEGE_WEAPON;
}

/*
BOOL AgsmSiegeWar::UseAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget)
{
	if (!pcsCharacter || !pcsTarget)
		return FALSE;

	// 사용한다~!!!!!
	//
	//
	//
	//
	//


	m_pcsAgpmCharacter->UpdateSpecialStatus(pcsTarget, AGPDCHAR_SPECIAL_STATUS_USE);
	m_pcsAgpmSiegeWar->SetUseCharacterName(pcsTarget, pcsCharacter);

	return TRUE;
}
*/

BOOL AgsmSiegeWar::CBNormalDamageAdjustSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar				*pThis				= (AgsmSiegeWar *)				pClass;
	stAgsmCombatAttackResult	*pstAttackResult	= (stAgsmCombatAttackResult *)	pData;

	if (!pstAttackResult->pAttackChar ||
		!pstAttackResult->pTargetChar ||
		pstAttackResult->nDamage <= 0)
		return TRUE;

	// 공격받은 놈이 공성 몬스터인지 본다.
	AgpdSiegeWarMonsterType	eMonsterType	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pstAttackResult->pTargetChar);

	if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE ||
		eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return TRUE;

	// 원거리, 근거리, 공성전용무기 공격인지 알아낸다.
	AttackType	eAttackType	= pThis->GetAttackType(pstAttackResult->pAttackChar);
	if (eAttackType < AGSMSIEGEWAR_ATTACK_TYPE_RANGE ||
		eAttackType >= AGSMSIEGEWAR_ATTACK_TYPE_NONE)
		return TRUE;

	// 공격 타입에 따른 데미지 비율을 적용한다.
	pstAttackResult->nDamage	= (INT32) ((FLOAT) pstAttackResult->nDamage * (FLOAT) g_alDamageRateFromAttackType[eMonsterType][eAttackType] / 100.0f);

	return TRUE;
}

BOOL AgsmSiegeWar::CBSkillDamageAdjustSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar				*pThis				= (AgsmSiegeWar *)				pClass;
	stAgsmCombatAttackResult	*pstAttackResult	= (stAgsmCombatAttackResult *)	pData;
	AgpdSkill					*pcsSkill			= (AgpdSkill *)					pCustData;

	if (!pstAttackResult->pAttackChar ||
		!pstAttackResult->pTargetChar ||
		pstAttackResult->nDamage <= 0)
		return TRUE;

	// 공격받은 놈이 공성 몬스터인지 본다.
	AgpdSiegeWarMonsterType	eMonsterType	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pstAttackResult->pTargetChar);

	if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE ||
		eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return TRUE;

	// 원거리, 근거리, 공성전용무기 공격인지 알아낸다.
	AttackType	eAttackType	= pThis->GetAttackType(pcsSkill);
	if (eAttackType < AGSMSIEGEWAR_ATTACK_TYPE_RANGE ||
		eAttackType >= AGSMSIEGEWAR_ATTACK_TYPE_NONE)
		return TRUE;

	// 공격 타입에 따른 데미지 비율을 적용한다.
	pstAttackResult->nDamage	= (INT32) ((FLOAT) pstAttackResult->nDamage * (FLOAT) g_alDamageRateFromAttackType[eMonsterType][eAttackType] / 100.0f);

	return TRUE;
}

BOOL AgsmSiegeWar::CBSpawnCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	pThis->AddSiegeWarObjectChar(pcsCharacter);

	return TRUE;
}

BOOL AgsmSiegeWar::CBAddAttackGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	return TRUE;
}

BOOL AgsmSiegeWar::CBAddDefenseGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pData;
	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	AgpmSiegeResult	eResult			= (AgpmSiegeResult)	PtrToInt(ppvBuffer[1]);
	AgpdGuild		*pcsGuild		= (AgpdGuild *)		ppvBuffer[2];

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketDefenseGuildList(pcsSiegeWar, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	pThis->SendResult(pcsSiegeWar, pcsCharacter, eResult);

	if (pcsGuild)
	{
		UINT32	ulSiegeID	= pcsSiegeWar->m_ulSiegeDBID;
		INT32	lAdopted	= 2;

		PVOID pvBuffer[4];
		pvBuffer[0] = pcsGuild->m_szID;
		pvBuffer[1] = &ulSiegeID;
		pvBuffer[2] = &lAdopted;

		// update
		pThis->EnumCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_APPLICATION, pcsSiegeWar->m_strCastleName.GetBuffer(), pvBuffer);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CBReceiveOpenAttackObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgpdCharacter	*pcsTarget		= (AgpdCharacter *)	pCustData;

	AgpmSiegeResult	eResult	= pThis->m_pcsAgpmSiegeWar->CheckOpenAttackObjectEvent(pcsCharacter, pcsTarget);
	if (eResult != AGPMSIEGE_RESULT_OPEN_ATTACK_OBJECT_EVENT)
		return pThis->SendResponseUseAttackObject(pcsCharacter, pcsTarget, eResult);

	// 사용할 수 있는 놈이니 거리 체크해서 바로 결과 알려주던, 뛰게 하던 해준다.

	AuPOS	stTargetPos	= {0,0,0};

	if (pThis->m_pcsAgpmFactors->IsInRange(&pcsCharacter->m_stPos, &pcsTarget->m_stPos, 1100, 0, &stTargetPos))
	{
		// 성공~ 이제 사용해도 된다.
		// UI 열라고 알려준다.
		return pThis->SendResponseUseAttackObject(pcsCharacter, pcsTarget, eResult, TRUE);
	}
	else
	{
		// 이놈은 뛰어야겠네...

		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_USE_SIEGEWAR_ATTACK_OBJECT;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsTarget->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsTarget->m_lID;

		return pThis->m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CBReceiveUseAttackObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgpdCharacter	*pcsTarget		= (AgpdCharacter *)	pCustData;

	AgpmSiegeResult	eResult	= pThis->m_pcsAgpmSiegeWar->CheckValidUseAttackObject(pcsCharacter, pcsTarget);
	if (eResult != AGPMSIEGE_RESULT_SUCCESS)
		return pThis->SendResponseUseAttackObject(pcsCharacter, pcsTarget, eResult);

	// 사용할 수 있는 놈이니 거리 체크해서 바로 결과 알려주던, 뛰게 하던 해준다.

	AuPOS	stTargetPos	= {0,0,0};

	if (!pThis->m_pcsAgpmFactors->IsInRange(&pcsCharacter->m_stPos, &pcsTarget->m_stPos, 1100, 0, &stTargetPos))
		return FALSE;

	// 성공~ 이제 사용해도 된다.
	pThis->SendResponseUseAttackObject(pcsCharacter, pcsTarget, eResult);

	// 공격을 하던,, 부활을 시키던,, 암튼,, 활성화 시킨다.
	pThis->m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISABLE);
	pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsTarget, AGPDCHAR_SPECIAL_STATUS_USE);
	pThis->m_pcsAgpmSiegeWar->SetUseCharacterName(pcsTarget, pcsCharacter);

	pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsTarget, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);

	return TRUE;
}

BOOL AgsmSiegeWar::CBReceiveRepairAttackObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgpdCharacter	*pcsTarget		= (AgpdCharacter *)	pCustData;

	AgpmSiegeResult	eResult	= pThis->m_pcsAgpmSiegeWar->CheckValidRepairAttackObject(pcsCharacter, pcsTarget);
	if (eResult != AGPMSIEGE_RESULT_SUCCESS)
		return pThis->SendResponseUseAttackObject(pcsCharacter, pcsTarget, eResult);

	// 사용할 수 있는 놈이니 거리 체크해서 바로 결과 알려주던, 뛰게 하던 해준다.

	AuPOS	stTargetPos	= {0,0,0};

	if (!pThis->m_pcsAgpmFactors->IsInRange(&pcsCharacter->m_stPos, &pcsTarget->m_stPos, 1100, 0, &stTargetPos))
		return FALSE;

	// 성공~ 이제 수리해도 된다.
	pThis->SendResponseUseAttackObject(pcsCharacter, pcsTarget, eResult);

	pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsTarget, AGPDCHAR_SPECIAL_STATUS_REPAIR);
	pThis->m_pcsAgpmSiegeWar->SetUseCharacterName(pcsTarget, pcsCharacter);

	// 일단 돈을 깐다.
	pThis->m_pcsAgpmCharacter->SubMoney(pcsCharacter, pThis->m_pcsAgpmSiegeWar->GetRepairCost(pcsTarget));

	// IDLE 이벤트에 수리 시간 만큼 등록한다.
	pThis->AddTimer(pThis->m_pcsAgpmSiegeWar->GetRepairDuration(pcsTarget) * 60 * 1000, pcsTarget->m_lID, pThis, RepairAttackObject, pcsTarget);
	pThis->m_pcsAgpmSiegeWar->SetRepairStartTime(pcsTarget);

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsTarget);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketUpdateAttackObject(pcsTarget, &nPacketLength, FALSE);

	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
	{
		pThis->SendPacketToSiegeWar(pvPacket, nPacketLength, pcsSiegeWar);
	}

	// 수리가 완료되면 수리된 횟수를 증가시킨다.

	if (pcsSiegeWar && pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsTarget) == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER)
	{
		if (pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0] &&
			pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0] &&
			pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][1])
		{
			BOOL	bLeft	= AuMath::ConsiderLeftPoint(
														pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]->m_stPos,
														pcsTarget->m_stPos,
														(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0] == pcsTarget) ? pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][1]->m_stPos : pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0]->m_stPos);

			pThis->SendPacketStatusInfo(
					(bLeft) ? AGPMSIEGE_STATUS_REPAIR_START_LEFT_ATTACK_RES_TOWER : AGPMSIEGE_STATUS_REPAIR_START_RIGHT_ATTACK_RES_TOWER,
					pcsSiegeWar,
					NULL,
					NULL,
					AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER,
					0,
					0);
		}
	}

	return TRUE;
}

//BOOL AgsmSiegeWar::CBRemoveItemToAttackObject(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (!pData || !pClass)
//		return FALSE;
//
//	AgsmSiegeWar	*pThis		= (AgsmSiegeWar *)	pClass;
//	PVOID			*ppvBuffer	= (PVOID *)			pData;
//
//	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
//	AgpdCharacter	*pcsTarget		= (AgpdCharacter *)	ppvBuffer[1];
//	AgpdItem		*pcsItem		= (AgpdItem *)		ppvBuffer[2];
//
//	if (!pcsCharacter || !pcsTarget || !pcsItem)
//		return FALSE;
//
//	INT16	nPacketLength	= 0;
//	PVOID	pvPacket		= pThis->m_pcsAgsmItem->MakePacketItemUpdateStatus(pcsItem, &nPacketLength);
//
//	if (nPacketLength > sizeof(PACKET_HEADER) && pvPacket)
//	{
//		pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
//	}
//
//	return TRUE;
//}


BOOL AgsmSiegeWar::CBAddItemToAttackObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis		= (AgsmSiegeWar *)	pClass;
	PVOID			*ppvBuffer	= (PVOID *)			pData;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	AgpdCharacter	*pcsTarget		= (AgpdCharacter *)	ppvBuffer[1];
	AgpdItem		*pcsItem		= (AgpdItem *)		ppvBuffer[2];

	if (!pcsCharacter || !pcsTarget || !pcsItem)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgsmItem->MakePacketItemUpdateStatus(pcsItem, &nPacketLength);

	if (nPacketLength > sizeof(PACKET_HEADER) && pvPacket)
	{
		pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
	}

	// 추가되었다는 정보를 pcsCharacter로 보내준다.

	nPacketLength	= 0;
	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketAddItemResult(pcsCharacter->m_lID, pcsTarget->m_lID, pcsItem->m_lID, &nPacketLength);

	if (nPacketLength < sizeof(PACKET_HEADER) ||
		!pvPacket)
		return FALSE;

	return pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
}

BOOL AgsmSiegeWar::CBCarveASeal(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgpdCharacter	*pcsTarget		= (AgpdCharacter *)	pCustData;

	// 거리를 체크한다.
	AuPOS	stTargetPos	= {0,0,0};

	if (pThis->m_pcsAgpmFactors->IsInRange(&pcsCharacter->m_stPos, &pcsTarget->m_stPos, 1100, 0, &stTargetPos))
	{
		return pThis->ProcessCarveASeal(pcsCharacter, pcsTarget);
	}
	else
	{
		// 이놈은 뛰어야겠네...

		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_CARVE_A_SEAL;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsTarget->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsTarget->m_lID;

		return pThis->m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CBUpdateReverseAttack(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pData;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketUpdateReverseAttack(pcsSiegeWar, &nPacketLength);
	
	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return pThis->SendPacketAllUser(pvPacket, nPacketLength);
}

BOOL AgsmSiegeWar::ProcessCarveASeal(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget)
{
	if (!pcsCharacter || !pcsTarget)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsTarget);
	if (!pcsSiegeWar)
		return FALSE;

	AgpdGuild		*pcsGuild		= m_pcsAgpmGuild->GetGuild(pcsCharacter);
	if (!pcsGuild)
		return FALSE;

	if (!m_pcsAgpmSiegeWar->CheckCarveASeal(pcsCharacter, pcsTarget))
		return FALSE;

	if (!m_pcsAgpmSiegeWar->StartCarveASeal(pcsCharacter, pcsTarget))
		return FALSE;

	// 각인이 시작되었다고 알려준다.
	SendPacketCarveASeal(pcsSiegeWar, AGPMSIEGE_RESULT_START_CARVE_A_SEAL, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	SendPacketStatusInfo(AGPMSIEGE_STATUS_START_CARVE_GUILD,
						 pcsSiegeWar,
						 pcsGuild->m_szID,
						 pcsGuild->m_szMasterID,
						 AGPD_SIEGE_MONSTER_TYPE_NONE,
						 0,
						 0);

	return TRUE;
}

BOOL AgsmSiegeWar::SendResult(AgpdSiegeWar *pcsSiegeWar, AgpdCharacter *pcsCharacter, AgpmSiegeResult eResult)
{
	if (!pcsSiegeWar || !pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketResult(pcsSiegeWar, eResult, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmSiegeWar::SendResponseUseAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, AgpmSiegeResult eResult, BOOL bSendObjectInfo)
{
	if (!pcsTarget || !pcsCharacter)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= m_pcsAgpmSiegeWar->GetAttachCharacterData(pcsTarget);

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketResponseUseAttackObject(pcsCharacter->m_lID,
																				   pcsTarget->m_lID,
																				   eResult,
																				   (bSendObjectInfo) ? m_pcsAgpmSiegeWar->GetRepairCount(pcsTarget) : 0,
																				   (bSendObjectInfo) ? pcsAttachData->m_ucRepairedCount : 0,
																				   (bSendObjectInfo) ? m_pcsAgpmSiegeWar->GetRepairCost(pcsTarget) : 0,
																				   (bSendObjectInfo) ? m_pcsAgpmSiegeWar->GetRepairDuration(pcsTarget) : 0,
																				   0,
																				   (bSendObjectInfo) ? pcsAttachData->m_strUseCharacterName.GetBuffer() : NULL,
																				   (bSendObjectInfo) ? pcsAttachData->m_pcsAttackObjectGrid : NULL,
																				   &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmSiegeWar::ProcessStatusStart(AgpdSiegeWar *pcsSiegeWarData)
{
	if (m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWarData))
	{
		m_ulLatestConnOfArchlord = 0;
		pcsSiegeWarData->m_bIsActiveDefenseResurrection	= TRUE;
		pcsSiegeWarData->m_bIsActiveAttackResurrection	= TRUE;

		SendPacketCastleInfoToAll(pcsSiegeWarData);

		// 2007.04.24. steeple
		// 탈것에 타고 있으면 내린다.
		AgpdCharacter* pcsArchlord = m_pcsAgpmCharacter->GetCharacterLock(pcsSiegeWarData->m_strOwnerGuildMasterName);
		if(pcsArchlord && pcsArchlord->m_bRidable && m_pcsAgsmRide)
		{
			m_pcsAgsmRide->DismountReq(pcsArchlord, 1);
		}
	}

	if(!m_bLoaded)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("ProcessStatusStart"));

	// 2005.03.15. steeple
	// 공성 시작할 때 CarveGuildName 초기화 해준다.
	pcsSiegeWarData->m_strCarveGuildName.Clear();

	// 2007.03.21. laki
	// 섭다후 다시뜨면 여기서 초기화된다. -_-; AgsmSiegeWar::StartSiegeWar()로 이동한다.
	//pcsSiegeWarData->m_ulSiegeWarStartTimeMSec	= GetClockCount();
	//pcsSiegeWarData->m_ulLastSyncEndTimeSec	= 0;

	// 이제 싸우기 시작이다.
	// 공성 관련 길드끼리 쌈박질을 시작하게 한다.
	//
	//
	//
	//
	RemoveSiegeWarMonster(pcsSiegeWarData, AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_OBJECT);

	if (!SpawnSiegeWarMonster(pcsSiegeWarData, AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_OBJECT))
		return FALSE;

	if (pcsSiegeWarData->m_strOwnerGuildName.IsEmpty())
	{
		if (!SpawnSiegeWarMonster(pcsSiegeWarData, AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_NPC_GUILD))
			return FALSE;
	}

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	// 2007.03.15. steeple
	// Spawn Thread 를 타고 오게끔 변경.
	ActiveObject(pcsSiegeWarData);

	if (m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWarData))
		SetPositionForArchlordBattle();

	MoveAttackGuildMember(pcsSiegeWarData);
	MoveDefenseGuildMember(pcsSiegeWarData);

	//공성 시작시 성주 사냥터에 있는 유저들을 전부 쫓아낸다.
	m_pcsAgsmArchlord->ExpelAllUsersFromSecretDungeon();

	return TRUE;
}

BOOL AgsmSiegeWar::ProcessStatusBreakASeal(AgpdSiegeWar *pcsSiegeWarData)
{
	if (!pcsSiegeWarData)
		return FALSE;

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	// disable throne
	for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
	{
		if (!pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][j])
			break;

		m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][j], AGPDCHAR_SPECIAL_STATUS_DISABLE);
	}

	// active archon eyes & tower
	for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
	{
		if (!pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][j])
			break;

		m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][j], AGPDCHAR_SPECIAL_STATUS_DISABLE);
		m_pcsAgsmCharacter->SetIdleInterval(pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][j], AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
	}

	for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
	{
		if (!pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES_TOWER][j])
			break;

		m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES_TOWER][j], AGPDCHAR_SPECIAL_STATUS_DISABLE);
		m_pcsAgsmCharacter->SetIdleInterval(pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES_TOWER][j], AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::ProcessStatusOpenEyes(AgpdSiegeWar *pcsSiegeWarData)
{
	if (!pcsSiegeWarData)
		return FALSE;

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	// disable archon eyes & tower
	for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
	{
		if (!pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][j])
			break;

		m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][j], AGPDCHAR_SPECIAL_STATUS_DISABLE);
	}

	for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
	{
		if (!pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES_TOWER][j])
			break;

		m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES_TOWER][j], AGPDCHAR_SPECIAL_STATUS_DISABLE);
	}

	// active throne
	for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
	{
		if (!pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][j])
			break;

		m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][j], AGPDCHAR_SPECIAL_STATUS_DISABLE);
		m_pcsAgsmCharacter->SetIdleInterval(pcsSiegeWarData->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][j], AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::ProcessStatusTimeOver(AgpdSiegeWar *pcsSiegeWarData)
{
	//STOPWATCH2(GetModuleName(), _T("ProcessStatusTimeOver"));

	// 공성전이 끝났다. 남아 있는것들 모두 정리한다.
	pcsSiegeWarData->m_ullPrevSiegeWarTimeDate	= pcsSiegeWarData->m_ullNextSiegeWarTimeDate;

	// 서로 못싸우게 한다.
	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	for (int i = 0; i < AGPD_SIEGE_MONSTER_TYPE_MAX; ++i)
	{
		for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
		{
			if (!pcsSiegeWarData->m_apcsObjectChar[i][j])
				break;

			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsSiegeWarData->m_apcsObjectChar[i][j], AGPDCHAR_SPECIAL_STATUS_DISABLE);
		}
	}

	if (!m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWarData))
	{
		// 2007.07.24. steeple
		// 모두 30초로 통일.
		INT32 lTimer = 30 * 1000;
		AddTimer(lTimer, 0, this, ResetSiegeWarObject, pcsSiegeWarData);

		// 마지막으로 각인한 길드가 있다면 그 길드가 새로운 주인이 되는거다.
		AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWarData->m_strCarveGuildName.GetBuffer());

		// 2007.01.15. steeple
		// 새 길드가 있고 이전과 다를 때만 한다.
		if(pcsGuild &&
			pcsSiegeWarData->m_strCarveGuildName.Compare(pcsSiegeWarData->m_strOwnerGuildMasterName.GetBuffer()) != COMPARE_EQUAL)
		{
			AgpdCharacter	*pcsPrevOwner	= m_pcsAgpmCharacter->GetCharacterLock(pcsSiegeWarData->m_strOwnerGuildMasterName.GetBuffer());
			if (pcsPrevOwner)
			{
				if (pcsSiegeWarData->m_lArrayIndex == AGPMSIEGEWAR_HUMAN_CASTLE_INDEX)
					m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsPrevOwner, AGPDCHAR_SPECIAL_STATUS_HUMAN_CASTLE_OWNER);
				else if (pcsSiegeWarData->m_lArrayIndex == AGPMSIEGEWAR_ORC_CASTLE_INDEX)
					m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsPrevOwner, AGPDCHAR_SPECIAL_STATUS_ORC_CASTLE_OWNER);
				else if (pcsSiegeWarData->m_lArrayIndex == AGPMSIEGEWAR_MOONELF_CASTLE_INDEX)
					m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsPrevOwner, AGPDCHAR_SPECIAL_STATUS_MOONELF_CASTLE_OWNER);
				else if (pcsSiegeWarData->m_lArrayIndex == AGPMSIEGEWAR_DRAGONSCION_CASTLE_INDEX)
					m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsPrevOwner, AGPDCHAR_SPECIAL_STATUS_DRAGONSCION_CASTLE_OWNER);

				pcsPrevOwner->m_Mutex.Release();
			}

			m_pcsAgpmSiegeWar->SetNewOwner(pcsSiegeWarData, pcsGuild);

			AgpdCharacter	*pcsNewOwner	= m_pcsAgpmCharacter->GetCharacterLock(pcsSiegeWarData->m_strOwnerGuildMasterName.GetBuffer());
			if (pcsNewOwner)
			{
				if (pcsSiegeWarData->m_lArrayIndex == AGPMSIEGEWAR_HUMAN_CASTLE_INDEX)
					m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsNewOwner, AGPDCHAR_SPECIAL_STATUS_HUMAN_CASTLE_OWNER);
				else if (pcsSiegeWarData->m_lArrayIndex == AGPMSIEGEWAR_ORC_CASTLE_INDEX)
					m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsNewOwner, AGPDCHAR_SPECIAL_STATUS_ORC_CASTLE_OWNER);
				else if (pcsSiegeWarData->m_lArrayIndex == AGPMSIEGEWAR_MOONELF_CASTLE_INDEX)
					m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsNewOwner, AGPDCHAR_SPECIAL_STATUS_MOONELF_CASTLE_OWNER);
				else if (pcsSiegeWarData->m_lArrayIndex == AGPMSIEGEWAR_DRAGONSCION_CASTLE_INDEX)
					m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsNewOwner, AGPDCHAR_SPECIAL_STATUS_DRAGONSCION_CASTLE_OWNER);

				pcsNewOwner->m_Mutex.Release();
			}
	
			// 2007.01.18. steeple
			SendPacketCastleInfoToAll(pcsSiegeWarData);
		}

		RemoveSiegeWarMonster(pcsSiegeWarData, AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_NPC_GUILD);

		m_pcsAgpmSiegeWar->ClearAttackGuild(pcsSiegeWarData);
		m_pcsAgpmSiegeWar->ClearDefenseGuild(pcsSiegeWarData);
		m_pcsAgpmSiegeWar->ClearAttackApplGuild(pcsSiegeWarData);
		m_pcsAgpmSiegeWar->ClearDefenseApplGuild(pcsSiegeWarData);
	}
	else
	{
		m_pcsAgpmSiegeWar->SetCurrentStatus(pcsSiegeWarData->m_lArrayIndex, AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::ProcessStatusArchlordBattle(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	// 각인 길드가 없으면 뭐 대략 ...
	AgpdGuild	*pcsCarveGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strCarveGuildName);
	if (!pcsCarveGuild)
		return TRUE;

	// 디카인을 소환한다.
	SpawnBossMob(pcsSiegeWar);
	
	// start time을 현재 시간으로 조정.
	pcsSiegeWar->m_ulSiegeWarStartTimeMSec = GetClockCount();
	
	// 렌스피어 내성문 제거.
	if (pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_INNER_GATE][0])
		m_pcsAgpmCharacter->RemoveCharacter(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_INNER_GATE][0]->m_lID);
	
	return TRUE;
}

BOOL AgsmSiegeWar::ProcessStatusTimeOverArchlordBattle(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	RemoveBossMob(pcsSiegeWar);

	RemoveSiegeWarMonster(pcsSiegeWar, AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_NPC_GUILD);

	// 아크로드 결정전에 성공하지 못했다면, 해당 길드를 일정 시간 후에 모두 마을로 보내버린다.
	// 10분 후에서 1초 후로 변경.
	AddTimer(1000, 0, this, TeleportArchlordBattle, pcsSiegeWar);

	m_pcsAgpmSiegeWar->ClearAttackGuild(pcsSiegeWar);
	m_pcsAgpmSiegeWar->ClearDefenseGuild(pcsSiegeWar);
	m_pcsAgpmSiegeWar->ClearAttackApplGuild(pcsSiegeWar);
	m_pcsAgpmSiegeWar->ClearDefenseApplGuild(pcsSiegeWar);

	return TRUE;
}

//BOOL AgsmSiegeWar::SetPositionForArchlordBattle()
//{
//	//STOPWATCH2(GetModuleName(), _T("SetPositionForArchlordBattle"));
//
//	AgpdSiegeWar	*pcsSiegeWar	= &m_pcsAgpmSiegeWar->m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX];
//
//	AgpdGuild	*pcsCarveGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strCarveGuildName);
//	if (pcsCarveGuild)
//	{
//		// 내성 포탈을 막는다.
//		// 내성에 존재하는 아크로드 길드원 (아크로드 제외)을 마을로 보내버린다.
//		//
//		//
//		//
//		//
//		//
//
//		// 각인된 길드원(공성지역에 있는)과 아크로드를 내성으로 소환시킨다.
//		
//		// 아크로드가 존재한다면 아크로드부터 소환시킨다.
//		AgpdCharacter	*pcsArchlord	= m_pcsAgpmCharacter->GetCharacterLock(pcsSiegeWar->m_strOwnerGuildMasterName);
//		if (pcsArchlord)
//		{
//			AuPOS	stDestPos;
//			if (m_pcsAgpmEventBinding->GetBindingForArchlord(&stDestPos, pcsSiegeWar->m_apcsRegionTemplate[0]->pStrName) ||
//				m_pcsAgpmEventBinding->GetBindingForArchlord(&stDestPos, pcsSiegeWar->m_apcsRegionTemplate[1]->pStrName))
//			{
//				m_pcsAgpmCharacter->StopCharacter(pcsArchlord, NULL);
//				m_pcsAgpmCharacter->UpdatePosition(pcsArchlord, &stDestPos, FALSE, TRUE);
//			}
//			else
//			{
//				// 이를 어쩐다(?)
//				// 이런일은 일어나면 안되는디,,, 걍 서버를 죽일까?
//				CHAR	*pszNULL	= NULL;
//				*pszNULL	= 'x';
//			}
//
//			pcsArchlord->m_Mutex.Release();
//
//			AgpdGuild	*pcsArchlordGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strOwnerGuildName);
//
//			if (pcsArchlordGuild && pcsArchlordGuild->m_Mutex.WLock())
//			{
//				INT32 lIndex = 0;
//				for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**) pcsArchlordGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
//																ppcsMember = (AgpdGuildMember**) pcsArchlordGuild->m_pMemberList->GetObjectSequence(&lIndex))
//				{
//					if(!ppcsMember || !*ppcsMember)
//						break;
//
//					AgpdCharacter	*pcsMember	= m_pcsAgpmCharacter->GetCharacterLock((*ppcsMember)->m_szID);
//					if (pcsMember)
//					{
//						if (pcsMember != pcsArchlord)
//						{
//							AgpdSiegeWar	*pcsCurrentSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsMember);
//							if (pcsSiegeWar == pcsCurrentSiegeWar)
//							{
//								AuPOS	stNewTargetPos	= pcsMember->m_stPos;
//								if (m_pcsAgpmEventBinding->GetBindingForArchlord(&stNewTargetPos, pcsSiegeWar->m_apcsRegionTemplate[0]->pStrName) ||
//									m_pcsAgpmEventBinding->GetBindingForArchlord(&stNewTargetPos, pcsSiegeWar->m_apcsRegionTemplate[1]->pStrName))
//								{
//									m_pcsAgpmCharacter->StopCharacter(pcsMember, NULL);
//									m_pcsAgpmCharacter->UpdatePosition(pcsMember, &stNewTargetPos, FALSE, TRUE);
//								}
//							}
//						}
//
//						pcsMember->m_Mutex.Release();
//					}
//				}
//
//				pcsArchlordGuild->m_Mutex.Release();
//			}
//		}
//		else
//		{
//			// 디카인을 소환한다.
//			SpawnBossMob(pcsSiegeWar);
//		}
//
//		// 각인 길드를 소환시킨다.
//		if (pcsCarveGuild->m_Mutex.WLock())
//		{
//			INT32 lIndex = 0;
//			for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**) pcsCarveGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
//															ppcsMember = (AgpdGuildMember**)pcsCarveGuild->m_pMemberList->GetObjectSequence(&lIndex))
//			{
//				if(!ppcsMember || !*ppcsMember)
//					break;
//
//				AgpdCharacter	*pcsMember	= m_pcsAgpmCharacter->GetCharacterLock((*ppcsMember)->m_szID);
//				if (pcsMember)
//				{
//					AgpdSiegeWar	*pcsCurrentSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsMember);
//					if (pcsSiegeWar == pcsCurrentSiegeWar)
//					{
//						AuPOS	stNewTargetPos	= pcsMember->m_stPos;
//						if (m_pcsAgpmEventBinding->GetBindingForArchlordAttacker(&stNewTargetPos, pcsSiegeWar->m_apcsRegionTemplate[0]->pStrName) ||
//							m_pcsAgpmEventBinding->GetBindingForArchlordAttacker(&stNewTargetPos, pcsSiegeWar->m_apcsRegionTemplate[1]->pStrName))
//						{
//							m_pcsAgpmCharacter->StopCharacter(pcsMember, NULL);
//							m_pcsAgpmCharacter->UpdatePosition(pcsMember, &stNewTargetPos, FALSE, TRUE);
//						}
//					}
//
//					pcsMember->m_Mutex.Release();
//				}
//			}
//
//			pcsCarveGuild->m_Mutex.Release();
//		}
//	}
//	else
//	{
//		// 각인 길드가 없다면 아크로드건 누구건 굳이 소환할 필요가 없다.
//	}
//
//	return TRUE;
//}

BOOL AgsmSiegeWar::SetPositionForArchlordBattle()
{
	//STOPWATCH2(GetModuleName(), _T("SetPositionForArchlordBattle"));

	AgpdSiegeWar	*pcsSiegeWar	= &m_pcsAgpmSiegeWar->m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX];

	// 아크로드가 존재한다면 아크로드부터 소환시킨다.
	AgpdCharacter	*pcsArchlord	= m_pcsAgpmCharacter->GetCharacterLock(pcsSiegeWar->m_strOwnerGuildMasterName);
	if (pcsArchlord)
	{
		AuPOS	stDestPos;
		if (m_pcsAgpmEventBinding->GetBindingForArchlord(&stDestPos, pcsSiegeWar->m_apcsRegionTemplate[0]->pStrName) ||
			m_pcsAgpmEventBinding->GetBindingForArchlord(&stDestPos, pcsSiegeWar->m_apcsRegionTemplate[1]->pStrName))
		{
			m_pcsAgpmCharacter->StopCharacter(pcsArchlord, NULL);
			m_pcsAgpmCharacter->UpdatePosition(pcsArchlord, &stDestPos, FALSE, TRUE);
		}
		else
		{
			// 이를 어쩐다(?)
			// 이런일은 일어나면 안되는디,,, 걍 서버를 죽일까?
			CHAR	*pszNULL	= NULL;
			*pszNULL	= 'x';
		}

		pcsArchlord->m_Mutex.Release();
	}

	AgpdGuild	*pcsArchlordGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strOwnerGuildName);

	if (pcsArchlordGuild && pcsArchlordGuild->m_Mutex.WLock())
	{
		INT32 lIndex = 0;
		for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**) pcsArchlordGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
														ppcsMember = (AgpdGuildMember**) pcsArchlordGuild->m_pMemberList->GetObjectSequence(&lIndex))
		{
			if(!ppcsMember || !*ppcsMember)
				break;

			AgpdCharacter	*pcsMember	= m_pcsAgpmCharacter->GetCharacterLock((*ppcsMember)->m_szID);
			if (pcsMember)
			{
				if (pcsMember != pcsArchlord)
				{
					// 2007.04.25. laki 어디 있던 일단 땡겨온다.
					AuPOS	stNewTargetPos	= pcsMember->m_stPos;
					if (m_pcsAgpmEventBinding->GetBindingForArchlord(&stNewTargetPos, pcsSiegeWar->m_apcsRegionTemplate[0]->pStrName) ||
						m_pcsAgpmEventBinding->GetBindingForArchlord(&stNewTargetPos, pcsSiegeWar->m_apcsRegionTemplate[1]->pStrName))
					{
						m_pcsAgpmCharacter->StopCharacter(pcsMember, NULL);
						m_pcsAgpmCharacter->UpdatePosition(pcsMember, &stNewTargetPos, FALSE, TRUE);
					}
				}

				pcsMember->m_Mutex.Release();
			}
		}

		pcsArchlordGuild->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmSiegeWar::ProcessNewArchlord(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	// 시스템 메시지 먼저 뿌린다. 2007.02.21. steeple
	ProcessSystemMessageNewArchlord(pcsSiegeWar);

	// 새로운 아크로드의 탄생이다.
	AgpdGuild	*pcsCarveGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strCarveGuildName);
	if (!pcsCarveGuild)
		return FALSE;

	AgpdGuildMember		*pcsGuildMaster	= m_pcsAgpmGuild->GetMaster(pcsCarveGuild);
	if (!pcsGuildMaster)
		return FALSE;

	AgpdCharacter	*pcsNewArchlord	= m_pcsAgpmCharacter->GetCharacterLock(pcsGuildMaster->m_szID);
	if (!pcsNewArchlord)
		return FALSE;

	// 새로운 아크로드를 탄생시킨다.
	m_pcsAgsmArchlord->SetArachlord(pcsNewArchlord);

	// 일단 아크로드 결정전을 종료한다.
	m_pcsAgpmSiegeWar->SetCurrentStatus(AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX, AGPD_SIEGE_WAR_STATUS_TIME_OVER_ARCHLORD_BATTLE);

	// 죽은 아크로드는 마을 부활밖에 할 수 없다. 굳이 보낼 필요도 없다.

	pcsNewArchlord->m_Mutex.Release();

	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
		if (!m_pcsAgpmSiegeWar->IsArchlordCastle(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i]) && m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_strOwnerGuildName.Compare(pcsSiegeWar->m_strCarveGuildName) == COMPARE_EQUAL)
			m_pcsAgpmSiegeWar->SetNewOwner(&m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i], NULL);

	return TRUE;
}

BOOL AgsmSiegeWar::ProcessFailedArchlordBattle()
{
	AgpdSiegeWar	*pcsSiegeWar	= &m_pcsAgpmSiegeWar->m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX];

	// 시스템 메시지 먼저 뿌린다. 2007.02.21. steeple
	ProcessSystemMessageFailedArchlordBattle(pcsSiegeWar);

	// 아크로드 결정전을 종료한다.
	// 상태를 종료시키고, 아크로드 결정전에 참여했던 공격 길드를 모두 마지막 거친 마을로 보내버린다.

	m_pcsAgpmSiegeWar->SetCurrentStatus(AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX, AGPD_SIEGE_WAR_STATUS_TIME_OVER_ARCHLORD_BATTLE);

	return TRUE;
}

// 2007.02.21. steeple
BOOL AgsmSiegeWar::ProcessSystemMessageNewArchlord(AgpdSiegeWar* pcsSiegeWar)
{
	if(!pcsSiegeWar)
		return FALSE;

	// 기존 아크로드 길드
	AgpdGuild* pcsPrevGuild = m_pcsAgpmGuild->GetGuildLock(pcsSiegeWar->m_strOwnerGuildName.GetBuffer());
	if(pcsPrevGuild)
	{
		m_pcsAgsmGuild->SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_ARCHLORD_DEFENSE_FAILURE, pcsPrevGuild);
		pcsPrevGuild->m_Mutex.Release();
	}

	// 새 아크로드 길드
	AgpdGuild* pcsNewGuild = m_pcsAgpmGuild->GetGuildLock(pcsSiegeWar->m_strCarveGuildName.GetBuffer());
	if(pcsNewGuild)
	{
		m_pcsAgsmGuild->SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_ARCHLORD_ATTACK_SUCCESS, pcsPrevGuild);
		pcsNewGuild->m_Mutex.Release();
	}

	return TRUE;
}

// 2007.02.21. steeple
BOOL AgsmSiegeWar::ProcessSystemMessageFailedArchlordBattle(AgpdSiegeWar* pcsSiegeWar)
{
	if(!pcsSiegeWar)
		return FALSE;

	// 기존 아크로드 길드
	AgpdGuild* pcsPrevGuild = m_pcsAgpmGuild->GetGuildLock(pcsSiegeWar->m_strOwnerGuildName.GetBuffer());
	if(pcsPrevGuild)
	{
		m_pcsAgsmGuild->SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_ARCHLORD_DEFENSE_SUCCESS, pcsPrevGuild);
		pcsPrevGuild->m_Mutex.Release();
	}

	// 새 아크로드 길드
	AgpdGuild* pcsNewGuild = m_pcsAgpmGuild->GetGuildLock(pcsSiegeWar->m_strCarveGuildName.GetBuffer());
	if(pcsNewGuild)
	{
		m_pcsAgsmGuild->SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_ARCHLORD_ATTACK_FAILURE, pcsPrevGuild);
		pcsNewGuild->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmSiegeWar::SpawnSiegeWarMonster(AgpdSiegeWar *pcsSiegeWarData, const AgpdSpawnSiegeWarType eSpawnSiegeWarType)
{
	//STOPWATCH2(GetModuleName(), _T("SpawnSiegeWarMonster"));


	// 2007.03.15. steeple
	// EventSpawn Thread 로 돌린다.
	return m_pcsAgsmEventSpawn->InsertAdditionalSpawn(AGSMSPAWN_ADDITIONAL_TYPE_SIEGEWAR,
														this,
														CBSpawnSiegeWarMonster,
														pcsSiegeWarData->m_lArrayIndex,
														(INT32)eSpawnSiegeWarType);
}

BOOL AgsmSiegeWar::SpawnSiegeWarMonsterDirect(AgpdSiegeWar *pcsSiegeWarData, const AgpdSpawnSiegeWarType eSpawnSiegeWarType)
{
	//STOPWATCH2(GetModuleName(), _T("SpawnSiegeWarMonsterDirect"));

	// spawn siege war monster

	AgpdSpawnGroup *	pstGroup;
	INT32				lIndex = 0;

	UINT32				ulClockCount	= GetClockCount();

	for (pstGroup = m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex);
		pstGroup;
		pstGroup = m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		for (ApdEvent	*pstEvent = pstGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn *) pstEvent->m_pvData)->m_pstNext)
		{
			if (!pstEvent->m_pvData)
				break;

			// 공성 Object, 몬스터 소환
			if (eSpawnSiegeWarType == ((AgpdSpawn *) pstEvent->m_pvData)->m_lSiegeWarType)
			{
				AuMATRIX	*pstDirection	= NULL;;
				AuPOS		stSpawnPos	= *m_pcsApmEventManager->GetBasePos(pstEvent->m_pcsSource, &pstDirection);

				if (pcsSiegeWarData == m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(stSpawnPos)))
					m_pcsAgsmEventSpawn->ProcessSpawn(pstEvent, ulClockCount);
			}
		}
	}

	return TRUE;
}

BOOL AgsmSiegeWar::RemoveSiegeWarMonster(AgpdSiegeWar *pcsSiegeWarData, const AgpdSpawnSiegeWarType eSpawnSiegeWarType)
{
	//STOPWATCH2(GetModuleName(), _T("RemoveSiegeWarMonster"));

	// 2007.03.15. steeple
	// EventSpawn Thread 로 돌린다.
	return m_pcsAgsmEventSpawn->InsertAdditionalSpawn(AGSMSPAWN_ADDITIONAL_TYPE_SIEGEWAR,
														this,
														CBRemoveSiegeWarMonster,
														pcsSiegeWarData->m_lArrayIndex,
														(INT32)eSpawnSiegeWarType);
}

BOOL AgsmSiegeWar::SpawnBossMob(AgpdSiegeWar *pcsSiegeWar)
{
	//STOPWATCH2(GetModuleName(), _T("SpawnBossMob"));

	if (!pcsSiegeWar)
		return FALSE;

	// 현재는 아크로드 성에만 보스몹은 디카인이 존재한다.
	// 아크로드 성일 경우에도 아크로드가 존재하지 않을 경우만 디카인이 존재한다.

	if (!m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar))	// 아크로드 성이 아니다.
		return FALSE;

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	if (!pcsSiegeWar->m_strOwnerGuildMasterName.IsEmpty())	// 아크로드가 존재하는 성이다.
		return FALSE;

	if (pcsSiegeWar->m_pcsBossMob)		// 이미 스폰이 되어있다. 넘어간다.
		return TRUE;

	INT32	lIndex	= 0;
	UINT32	ulClockCount	= GetClockCount();

	// 스폰을 뒤져 아크로드 성의 보스인 디카인을 찾아 스폰시킨다.
	for (AgpdSpawnGroup *pstGroup = m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex); pstGroup; pstGroup = m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		for (ApdEvent *pstEvent = pstGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn *) pstEvent->m_pvData)->m_pstNext)
		{
			if (!pstEvent->m_pvData)
				break;

			AgpdSpawn *			pstSpawn	= (AgpdSpawn *) pstEvent->m_pvData;

			for (INT32 lIndex1 = 0; lIndex1 < pstSpawn->m_lTotalCharacter; ++lIndex1)
			{
				if (pstSpawn->m_stSpawnConfig[lIndex1].m_lTID != 1366)
					continue;

				AuPOS	*pstBasePos	= m_pcsApmEventManager->GetBasePos(pstEvent->m_pcsSource, NULL);
				if (pstBasePos && pcsSiegeWar == m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(*pstBasePos)))
					return m_pcsAgsmEventSpawn->SpawnCharacter(	pstSpawn->m_stSpawnConfig[lIndex1].m_lTID, pstSpawn->m_stSpawnConfig[lIndex1].m_lAITID, *pstBasePos, pstSpawn);
			}
		}
	}

	return FALSE;
}

BOOL AgsmSiegeWar::RemoveBossMob(AgpdSiegeWar *pcsSiegeWar)
{
	//STOPWATCH2(GetModuleName(), _T("RemoveBossMob"));
	if (!pcsSiegeWar)
		return FALSE;

	// 스폰되어 있는 보스몹을 제거해버린다.

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	if (pcsSiegeWar->m_pcsBossMob)
		m_pcsAgpmCharacter->RemoveCharacter(pcsSiegeWar->m_pcsBossMob->m_lID);

	pcsSiegeWar->m_pcsBossMob	= NULL;

	return TRUE;
}

// 2007.03.15. steeple
// EventSpawn Thread 에서 불린다. 실제 스폰은 여기서 해야 한다.
BOOL AgsmSiegeWar::CBSpawnSiegeWarMonster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmSiegeWar* pThis = static_cast<AgsmSiegeWar*>(pClass);
	AdditionalSpawnData* pstSpawnData = static_cast<AdditionalSpawnData*>(pData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBSpawnSiegeWarMonster"));

	AgpdSiegeWar* pcsSiegeWarData = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pstSpawnData->m_lData1);
	if(!pcsSiegeWarData)
		return FALSE;

	pThis->SpawnSiegeWarMonsterDirect(pcsSiegeWarData, (AgpdSpawnSiegeWarType)pstSpawnData->m_lData2);
	return TRUE;
}

// 2007.03.15. steeple
// EventSpawn Thread 에서 불린다.
BOOL AgsmSiegeWar::CBRemoveSiegeWarMonster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmSiegeWar* pThis = static_cast<AgsmSiegeWar*>(pClass);
	AdditionalSpawnData* pstSpawnData = static_cast<AdditionalSpawnData*>(pData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveSiegeWarMonster"));

	AgpdSiegeWar* pcsSiegeWarData = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pstSpawnData->m_lData1);
	if(!pcsSiegeWarData)
		return FALSE;

	AgpdSpawnSiegeWarType eSpawnSiegeWarType = (AgpdSpawnSiegeWarType)pstSpawnData->m_lData2;

	// remove siege war monster

	AgpdSpawnGroup *	pstGroup;
	INT32				lIndex = 0;

	for (pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex);
		pstGroup;
		pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		for (ApdEvent	*pstEvent = pstGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn *) pstEvent->m_pvData)->m_pstNext)
		{
			if (!pstEvent->m_pvData)
				break;

			if (eSpawnSiegeWarType == ((AgpdSpawn *) pstEvent->m_pvData)->m_lSiegeWarType)
			{
				AuMATRIX	*pstDirection	= NULL;;
				AuPOS		stSpawnPos	= *pThis->m_pcsApmEventManager->GetBasePos(pstEvent->m_pcsSource, &pstDirection);

				if (pcsSiegeWarData == pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(stSpawnPos)))
					pThis->m_pcsAgsmEventSpawn->RemoveSpawnCharacter(pstEvent);
			}
		}
	}

	return TRUE;
}

// 2007.03.15. steeple
// Spawn 을 다른 쓰레드에서 하므로 얘도 그 쪽 큐를 따라가야 한다.
BOOL AgsmSiegeWar::ActiveObject(AgpdSiegeWar* pcsSiegeWarData)
{
	if(!pcsSiegeWarData)
		return FALSE;

	return m_pcsAgsmEventSpawn->InsertAdditionalSpawn(AGSMSPAWN_ADDITIONAL_TYPE_SIEGEWAR,
														this,
														CBActiveObject,
														pcsSiegeWarData->m_lArrayIndex,
														0);
}

// 2007.03.15. steeple
// Spawn 다 된 후에 Object 를 Active 시킨다.
BOOL AgsmSiegeWar::CBActiveObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmSiegeWar* pThis = static_cast<AgsmSiegeWar*>(pClass);
	AdditionalSpawnData* pstSpawnData = static_cast<AdditionalSpawnData*>(pData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBActiveObject"));

	AgpdSiegeWar* pcsSiegeWarData = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pstSpawnData->m_lData1);
	if(!pcsSiegeWarData)
		return FALSE;

	AuAutoLock lock(pThis->m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	for (int i = 0; i < AGPD_SIEGE_MONSTER_TYPE_MAX; ++i)
	{
		// 렌스피어성의 내성문은 activate하지 않는다.
		if (AGPD_SIEGE_MONSTER_INNER_GATE == i && pThis->m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWarData))
			continue;
	
		for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
		{
			if (!pcsSiegeWarData->m_apcsObjectChar[i][j])
				break;

			PVOID pvPacketFactor	= NULL;
			if (!pThis->m_pcsAgsmFactors->Resurrection(&pcsSiegeWarData->m_apcsObjectChar[i][j]->m_csFactor, &pvPacketFactor))
				return FALSE;

			// 다시 살아났다는 패킷을 보낸다.
			pThis->m_pcsAgsmCharacter->SendPacketFactor(pvPacketFactor, pcsSiegeWarData->m_apcsObjectChar[i][j], PACKET_PRIORITY_3);

			// update character status
			if (pcsSiegeWarData->m_apcsObjectChar[i][j]->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
				pThis->m_pcsAgpmCharacter->UpdateActionStatus(pcsSiegeWarData->m_apcsObjectChar[i][j], AGPDCHAR_STATUS_NORMAL);

			if (i != AGPD_SIEGE_MONSTER_ARCHON_EYES &&
				i != AGPD_SIEGE_MONSTER_ARCHON_EYES_TOWER &&
				i != AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER &&
				i != AGPD_SIEGE_MONSTER_CATAPULT)
			{
				pThis->m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsSiegeWarData->m_apcsObjectChar[i][j], AGPDCHAR_SPECIAL_STATUS_DISABLE);
				pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsSiegeWarData->m_apcsObjectChar[i][j], AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
			}

			if (i == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER ||
				i == AGPD_SIEGE_MONSTER_CATAPULT)
			{
				AgpdSiegeWarADCharacter	*pcsAttachData	= pThis->m_pcsAgpmSiegeWar->GetAttachCharacterData(pcsSiegeWarData->m_apcsObjectChar[i][j]);

				pcsAttachData->ucRepairCount	= pThis->m_pcsAgpmSiegeWar->GetRepairCount(pcsSiegeWarData->m_apcsObjectChar[i][j]);
			}

			if (pcsSiegeWarData->m_bIsNeedInsertDB)
			{
				// DB Insert	
				CHAR *pszCastle = pcsSiegeWarData->m_strCastleName.GetBuffer();		// CASTLEID
				INT32 lObjectID = i * 1000 + j;
				PVOID pvBuffer[2];
				pvBuffer[0] = pcsSiegeWarData->m_apcsObjectChar[i][j];				// Object(Character)
				pvBuffer[1] = &lObjectID;											// OBJECTID
				
				pThis->EnumCallback(AGSMSIEGEWAR_CB_DB_INSERT_SIEGE_OBJECT, pszCastle, pvBuffer);
			}
		}
	}

	if (pThis->m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWarData))
	{
		pcsSiegeWarData->m_bIsActiveDefenseResurrection	= TRUE;
		pcsSiegeWarData->m_bIsActiveAttackResurrection	= TRUE;

		pThis->SendPacketCastleInfoToAll(pcsSiegeWarData);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::ProclaimSiegeWar(ApmMap::RegionTemplate *pcsRegionTemplate)
{
	if (!pcsRegionTemplate)
		return FALSE;

	// DB Insert
	AgpdSiegeWar *pcsSiegeWar = m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsRegionTemplate));
	if (!pcsSiegeWar)
		return FALSE;
	
	time_t ulSiegeID;
	time(&ulSiegeID);		// SIEGEID
	pcsSiegeWar->m_ulSiegeDBID = (UINT32) ulSiegeID;
	UINT32	ulApplyElapsed = 0;
	UINT32	ulSiegeElapsed = 0;
	UINT64	ullSiegeEndDate = 0;
	PVOID pvBuffer[3];
	pvBuffer[0] = &ulApplyElapsed;
	pvBuffer[1] = &ulSiegeElapsed;
	pvBuffer[2] = &ullSiegeEndDate;

	pcsSiegeWar->m_bIsNeedInsertDB	= FALSE;
	m_pcsAgpmSiegeWar->SetCurrentStatus(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsRegionTemplate), AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR);
	pcsSiegeWar->m_bIsNeedInsertDB	= TRUE;

	return EnumCallback(AGSMSIEGEWAR_CB_DB_INSERT_SIEGE, pcsSiegeWar, pvBuffer);
}

BOOL AgsmSiegeWar::ReadySiegeWar(ApmMap::RegionTemplate *pcsRegionTemplate)
{
	if (!pcsRegionTemplate)
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCurrentStatus(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsRegionTemplate), AGPD_SIEGE_WAR_STATUS_READY))
		return FALSE;

	return TRUE;
}

BOOL AgsmSiegeWar::StartSiegeWar(ApmMap::RegionTemplate *pcsRegionTemplate, BOOL bCheckCondition)
{
	if (!pcsRegionTemplate)
		return FALSE;

	// 2007.03.21. laki.
	// idle()이나 command로 시작되는 경우는 여기밖에 없다.
	// DB 로딩시 ProcessStatusStart()에서 시간이 초기화되므로 여기로 옮긴다.
	INT32 lSiegeWarInfoIndex = m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsRegionTemplate);
	AgpdSiegeWar *pcsSiegeWarData = m_pcsAgpmSiegeWar->GetSiegeWarInfo(lSiegeWarInfoIndex);
	if (!pcsSiegeWarData)
		return FALSE;

	pcsSiegeWarData->m_ulSiegeWarStartTimeMSec = GetClockCount();
	pcsSiegeWarData->m_ulLastSyncEndTimeSec	= 0;

	if (!m_pcsAgpmSiegeWar->SetCurrentStatus(lSiegeWarInfoIndex, AGPD_SIEGE_WAR_STATUS_START, bCheckCondition))
	{
		pcsSiegeWarData->m_ulSiegeWarStartTimeMSec = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmSiegeWar::BreakASeal(ApmMap::RegionTemplate *pcsRegionTemplate)
{
	if (!pcsRegionTemplate)
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCurrentStatus(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsRegionTemplate), AGPD_SIEGE_WAR_STATUS_BREAK_A_SEAL))
		return FALSE;

	return TRUE;
}

BOOL AgsmSiegeWar::OpenEyes(ApmMap::RegionTemplate *pcsRegionTemplate, AgpdCharacter *pcsCharacter)
{
	if (!pcsRegionTemplate || !pcsCharacter)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsRegionTemplate));
	if (!pcsSiegeWar)
		return FALSE;

	AuAutoLock	lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	// 양쪽 두눈이 다 떠졌는지 본다.
	if (pcsSiegeWar->m_csEyeStatus.m_bOpenEye)
	{
		// 이미 한쪽이 떠져 있는 상태이다.
		pcsSiegeWar->m_csEyeStatus.m_bOpenEye		= FALSE;
		pcsSiegeWar->m_csEyeStatus.m_ulOpenTimeMSec	= 0;

		// 양쪽이 다 떠졌다면 이제 각인을 하면 된다.
		if (!m_pcsAgpmSiegeWar->SetCurrentStatus(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsRegionTemplate), AGPD_SIEGE_WAR_STATUS_OPEN_EYES))
			return FALSE;
	}
	else
	{
		// 한쪽만 떠졌다면 다른 한쪽이 눈떠야하는 제한시간을 세팅한다.
		pcsSiegeWar->m_csEyeStatus.m_bOpenEye		= TRUE;
		pcsSiegeWar->m_csEyeStatus.m_ulOpenTimeMSec	= GetClockCount() + pcsSiegeWar->m_csTemplate.m_ulOpenAllEyesDurationMSec;

		if (pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0] &&
			pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][0] &&
			pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][1])
		{
			BOOL	bLeft	= AuMath::ConsiderLeftPoint(
														pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]->m_stPos,
														pcsCharacter->m_stPos,
														(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][0] == pcsCharacter) ? pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][1]->m_stPos : pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ARCHON_EYES][0]->m_stPos);

			// 한쪽 눈이 떠졌다는 메시지를 날려준다.
			SendPacketStatusInfo(AGPMSIEGE_STATUS_ACTIVE_ARCHON_EYE, pcsSiegeWar, NULL, NULL, AGPD_SIEGE_MONSTER_TYPE_NONE, 0, bLeft);
		}
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CarveASeal(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	ApmMap::RegionTemplate	*pcsRegionTemplate	= m_pcsApmMap->GetTemplate(m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z));
	if (!pcsRegionTemplate)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsRegionTemplate));
	if (!pcsSiegeWar)
		return FALSE;

	AuAutoLock	lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	if (m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar))
	{
		if (pcsSiegeWar->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_START)
			return FALSE;
	}
	else
	{
		if (pcsSiegeWar->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_OPEN_EYES)
			return FALSE;
	}

	AgpdGuildADChar	*pcsGuildAttachData	= m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
	if (!pcsGuildAttachData)
		return FALSE;

	m_pcsAgpmSiegeWar->UpdateCarveAGuild(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsRegionTemplate), m_pcsAgpmGuild->GetGuild(pcsGuildAttachData->m_szGuildID));

	if (m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar))
	{
		// 아크로드 공성이라면 각인 길드가 나온 후 더이상 싸우지 못한다.

		// 각인된 길드를 제외한 모든 길드원을 마지막으로 거쳤던 마을로 보내버린다.
		// 상황 파악을 위해 30초 정도 공지를 날려준 후 보낸다.
		AgpdGuild	*pcsCarveGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strCarveGuildName.GetBuffer());

		INT32	lGuildIndex	= 0;
		for (AgpdGuild **ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lGuildIndex); ppcsGuild && *ppcsGuild; ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lGuildIndex))
		{
			if (pcsCarveGuild == (*ppcsGuild))
				continue;

			if ((*ppcsGuild)->m_Mutex.WLock())
			{
				AddTimer(30 * 1000, 0, this, TeleportGuildMember, (*ppcsGuild));
				(*ppcsGuild)->m_Mutex.Release();
			}
		}
		// 스폰된 몬스터를 날려버린다.
		RemoveSiegeWarMonster(pcsSiegeWar, AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_NPC_GUILD);

		// 5분 후 최종전 시작. 경과 시간을 조정할 수 없으니 시작 시간을 조정한다.
		pcsSiegeWar->m_ulSiegeWarStartTimeMSec = GetClockCount() - m_pcsAgpmSiegeWar->GetDurationMSec(pcsSiegeWar) + AGPMSIEGEWAR_ARCHLORD_2STEP_WAIT_INTERVAL;
		m_pcsAgsmArchlord->SetStepTime(GetClockCount() + AGPMSIEGEWAR_ARCHLORD_2STEP_WAIT_INTERVAL - AGSMARCHLORD_STEP_SIEGEWAR_TIME);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::TimeOver(INT32 lSiegeWarInfoIndex)
{
	if (lSiegeWarInfoIndex < 0 ||
		lSiegeWarInfoIndex >= AGPMSIEGEWAR_MAX_CASTLE)
		return FALSE;

	if (!m_pcsAgpmSiegeWar->SetCurrentStatus(lSiegeWarInfoIndex, AGPD_SIEGE_WAR_STATUS_TIME_OVER))
		return FALSE;

	// remove all objects
	AgpdSiegeWar *pcsSiegeWar = m_pcsAgpmSiegeWar->GetSiegeWarInfo(lSiegeWarInfoIndex);
	if (pcsSiegeWar)
	{
		CHAR *pszCastle = pcsSiegeWar->m_strCastleName.GetBuffer();
		EnumCallback(AGSMSIEGEWAR_CB_DB_DELETE_SIEGE_OBJECT, pszCastle, NULL);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::ResetSiegeWar(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	if (m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar))
		m_ulLatestConnOfArchlord = 0;

	return m_pcsAgpmSiegeWar->SetCurrentStatus(pcsSiegeWar->m_lArrayIndex, AGPD_SIEGE_WAR_STATUS_OFF);
}

BOOL AgsmSiegeWar::UpdateToDB(INT32 lSiegeWarInfoIndex)
{
	return UpdateToDB(m_pcsAgpmSiegeWar->GetSiegeWarInfo(lSiegeWarInfoIndex));
}

BOOL AgsmSiegeWar::UpdateToDB(AgpdSiegeWar *pcsSiegeWar)
{
	if (!m_bLoaded)
		return FALSE;

	if (!pcsSiegeWar)
		return FALSE;

	AuAutoLock	lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	// DB에 해당 공성전 내용을 저장한다.
	UpdateCastleToDB(pcsSiegeWar);
	UpdateSiegeToDB(pcsSiegeWar);
	UpdateAllSiegeWarObjectToDB(pcsSiegeWar);

	m_aulLastDBUpdateTime[pcsSiegeWar->m_lArrayIndex]	= GetClockCount();

	return TRUE;
}

BOOL AgsmSiegeWar::UpdateCastleToDB(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	// CASTLEID, OWNERGUILDID
	CHAR *pszCastle = pcsSiegeWar->m_strCastleName.GetBuffer();		// CASTLEID
	CHAR *pszGuild = pcsSiegeWar->m_strOwnerGuildName.GetBuffer();	// OWNERGUILDID
	
	// update
	////////////////////////////////////////////////////////////////////////
	EnumCallback(AGSMSIEGEWAR_CB_DB_UPDATE_CASTLE, pszCastle, pszGuild);

	return TRUE;
}

BOOL AgsmSiegeWar::UpdateSiegeToDB(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	//	CASTLEID, SIEGEID,APPLYBEGINDATE,APPLYELAPSED,SIEGEBEGINDATE,SIEGEELAPSED,
	//	SIEGEENDDATE,PREVENDDATE,STATUS,GUILDID
	UINT32	ulCurrentClockCount	= GetClockCount();
	UINT64	ullCurrentTimeDate	= AuTimeStamp2::GetCurrentTimeStamp();

	UINT32	ulApplyElapsed	= 0;					// APPLYELAPSED
	if (pcsSiegeWar->m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR)
	{
		ulApplyElapsed	= (UINT32) (ullCurrentTimeDate / 10000000 - pcsSiegeWar->m_ullProclaimTimeDate / 10000000);
	}

	UINT32	ulSiegeElapsed	= 0;					// SIEGEELAPSED
	if (m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar))
		ulSiegeElapsed	= (UINT32) ((ulCurrentClockCount - pcsSiegeWar->m_ulSiegeWarStartTimeMSec) / 1000.0f);

	UINT64 ullSiegeEndDate = ullCurrentTimeDate + (UINT64) ((pcsSiegeWar->m_ulSiegeWarStartTimeMSec + m_pcsAgpmSiegeWar->GetDurationMSec(pcsSiegeWar) - ulCurrentClockCount) / 1000.0f * (UINT64) 10000000);	// SIEGEENDDATE
	
	PVOID pvBuffer[3];
	pvBuffer[0] = &ulApplyElapsed;
	pvBuffer[1] = &ulSiegeElapsed;
	pvBuffer[2] = &ullSiegeEndDate;

	EnumCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE, pcsSiegeWar, pvBuffer);

	return TRUE;
}

BOOL AgsmSiegeWar::UpdateAllSiegeWarObjectToDB(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	AuAutoLock	lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	// 공성 관련 오브젝트들을 저장한다.
	if (!m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar))
		return TRUE;

	for (int i = 0; i < AGPD_SIEGE_MONSTER_TYPE_MAX; ++i)
	{
		for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
		{
			if (!pcsSiegeWar->m_apcsObjectChar[i][j])
				break;

			UpdateSiegeWarObjectToDB(pcsSiegeWar, pcsSiegeWar->m_apcsObjectChar[i][j], i * 1000 + j);
		}
	}

	return TRUE;
}

BOOL AgsmSiegeWar::UpdateSiegeWarObjectToDB(AgpdSiegeWar *pcsSiegeWar, AgpdCharacter *pcsCharacter, INT32 lObjectIndex)
{
	if (!pcsSiegeWar || !pcsCharacter)
		return FALSE;

	// DB Update
	PVOID pvBuffer[2];
	pvBuffer[0] = pcsCharacter;													// Object(Character)
	pvBuffer[1] = &lObjectIndex;												// OBJECTID
	
	EnumCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_OBJECT, pcsSiegeWar->m_strCastleName.GetBuffer(), pvBuffer);

	return TRUE;
}

BOOL AgsmSiegeWar::UpdateApplGuildToDB(AgpdSiegeWar *pcsSiegeWar)
{
	//STOPWATCH2(GetModuleName(), _T("UpdateApplGuildToDB"));

	if (!pcsSiegeWar)
		return FALSE;

	AuAutoLock	lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	UINT32 ulSiegeID = pcsSiegeWar->m_ulSiegeDBID;

	// 먼저 공성 신청 길드
	INT32	lIndex	= 0;
	AgpdGuild	**ppcsGuild	= (AgpdGuild **)	pcsSiegeWar->m_csAttackApplicationGuild.GetObjectSequence(&lIndex);
	while (ppcsGuild)
	{
		if (*ppcsGuild)
		{
			CHAR *pszCastle = pcsSiegeWar->m_strCastleName.GetBuffer();		// CASTLEID
			CHAR *pszGuild = (*ppcsGuild)->m_szID;							// GUILDID
			INT32	lSide		= 1;						// SIDE

			AgpdGuildAttachData	*pcsAttachGuildData	= m_pcsAgpmSiegeWar->GetAttachGuildData(*ppcsGuild);
			pcsAttachGuildData->m_ullApplDate;				// APPLYDATE

			INT32	lAdopted	= 0;						// ADOPTED		(1 : 선택된 길드)
			if (pcsSiegeWar->m_csAttackGuild.GetObject((*ppcsGuild)->m_lID))
			{
				// 이놈은 공성 신청하고, 선택도 된놈.
				lAdopted	= 1;
			}
			
			PVOID pvBuffer[4];
			pvBuffer[0] = pszGuild;
			pvBuffer[1] = &ulSiegeID;
			pvBuffer[2] = &lAdopted;

			// update
			EnumCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_APPLICATION, pszCastle, pvBuffer);
		}

		ppcsGuild	= (AgpdGuild **)	pcsSiegeWar->m_csAttackApplicationGuild.GetObjectSequence(&lIndex);
	}

	// 이젠 수성 신청 길드
	lIndex	= 0;
	ppcsGuild	= (AgpdGuild **)	pcsSiegeWar->m_csDefenseApplicationGuild.GetObjectSequence(&lIndex);
	while (ppcsGuild)
	{
		if (*ppcsGuild)
		{
			CHAR *pszCastle = pcsSiegeWar->m_strCastleName.GetBuffer();		// CASTLEID
			CHAR *pszGuild = (*ppcsGuild)->m_szID;							// GUILDID
			INT32	lSide		= 2;						// SIDE

			AgpdGuildAttachData	*pcsAttachGuildData	= m_pcsAgpmSiegeWar->GetAttachGuildData(*ppcsGuild);
			pcsAttachGuildData->m_ullApplDate;				// APPLYDATE

			INT32	lAdopted	= 0;						// ADOPTED		(1 : 선택된 길드)
			if (pcsSiegeWar->m_csDefenseGuild.GetObject((*ppcsGuild)->m_lID))
			{
				// 이놈은 수성 신청하고, 선택도 된놈.
				lAdopted	= 1;
			}

			PVOID pvBuffer[4];
			pvBuffer[0] = pszGuild;
			pvBuffer[1] = &ulSiegeID;
			pvBuffer[2] = &lAdopted;

			// update
			EnumCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_APPLICATION, pszCastle, pvBuffer);
		}

		ppcsGuild	= (AgpdGuild **)	pcsSiegeWar->m_csDefenseApplicationGuild.GetObjectSequence(&lIndex);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::SendPacketCastleInfo(AgpdSiegeWar *pcsSiegeWar, UINT32 ulNID)
{
	if (!pcsSiegeWar || ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketCastleInfo(pcsSiegeWar, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmSiegeWar::SendPacketCastleInfoToAll(AgpdSiegeWar *pcsSiegeWar)
{
	//STOPWATCH2(GetModuleName(), _T("SendPacketCastleInfoToAll"));

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketCastleInfo(pcsSiegeWar, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacketToSiegeWar(pvPacket, nPacketLength, pcsSiegeWar);
}

BOOL AgsmSiegeWar::SendPacketCarveASeal(AgpdSiegeWar *pcsSiegeWar, AgpmSiegeResult eResult, UINT32 ulNID)
{
	if (!pcsSiegeWar || ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketResult(pcsSiegeWar, eResult, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmSiegeWar->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmSiegeWar::ConfirmSiegeWarTime(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	if (pcsSiegeWar->m_ullNextSiegeWarTimeDate == 0)
	{
		// 성주가 공성 선포를 안한 경우.
		// 임의로 날을 잡아버린다. (날짜는 토요일, 시간은 이전 공성전 시간으로 확정해버린다.)
		if (!m_pcsAgpmSiegeWar->SetNextSiegeWarTime(pcsSiegeWar, m_pcsAgpmSiegeWar->GetNextSiegeWarFirstTime(pcsSiegeWar)))
			return FALSE;
	}
	else
	{
		// 성주가 잡아놓은 날로 확정한다.
		// 날이 잡혀있으니 굳이 따로 할건 없다.
	}

	return ProclaimSiegeWar(pcsSiegeWar->m_apcsRegionTemplate[0]);
}

BOOL AgsmSiegeWar::ConfirmGuild(AgpdSiegeWar *pcsSiegeWar)
{
	//STOPWATCH2(GetModuleName(), _T("ConfirmGuild"));

	if (!pcsSiegeWar)
		return FALSE;

	{
		AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strOwnerGuildName.GetBuffer());
		if (pcsGuild)
		{
			pcsSiegeWar->m_csDefenseGuild.AddObject((PVOID) &pcsGuild, pcsGuild->m_szID);

			INT32	lAdopted	= 1;

			PVOID pvBuffer[4];
			pvBuffer[0] = pcsGuild->m_szID;
			pvBuffer[1] = &pcsSiegeWar->m_ulSiegeDBID;
			pvBuffer[2] = &lAdopted;

			// update
			EnumCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_APPLICATION, pcsSiegeWar->m_strCastleName.GetBuffer(), pvBuffer);
		}
	}

	// 수성 길드를 확정한다.
	if (pcsSiegeWar->m_strOwnerGuildName.GetLength() > 0)
	{
		if (pcsSiegeWar->m_csDefenseGuild.GetObjectCount() == AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD - 1)
		{
			// 성주가 확정한 경우 이대로 수성 길드가 확정된다.
		}
		else
		{
			//pcsSiegeWar->m_csDefenseGuild.RemoveObjectAll();

			// 성주가 정하지 않은 경우, 상위 길드들이 확정된다.

			INT32	lIndex	= pcsSiegeWar->m_csDefenseGuild.GetObjectCount();

			AgpdGuild	*pcsGuild	= pcsSiegeWar->m_ApDefenseGuildSortList.GetHead();
			while (pcsGuild && !pcsSiegeWar->m_ApDefenseGuildSortList.IsEnd())
			{
				if (lIndex >= AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD)
					break;

				pcsSiegeWar->m_csDefenseGuild.AddObject((PVOID) &pcsGuild, pcsGuild->m_szID);

				INT32	lAdopted	= 1;

				PVOID pvBuffer[4];
				pvBuffer[0] = pcsGuild->m_szID;
				pvBuffer[1] = &pcsSiegeWar->m_ulSiegeDBID;
				pvBuffer[2] = &lAdopted;

				// update
				EnumCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_APPLICATION, pcsSiegeWar->m_strCastleName.GetBuffer(), pvBuffer);

				++lIndex;

				pcsGuild	= pcsSiegeWar->m_ApDefenseGuildSortList.GetNext();
			}
		}
	}

	// 공성 길드를 확정한다.
	// 상위 두개 길드가 공성 길드가 된다.
	{
		pcsSiegeWar->m_csAttackGuild.RemoveObjectAll();

		AgpdGuild	*pcsGuild	= pcsSiegeWar->m_ApAttackGuildSortList.GetHead();
		INT32	lIndex	= 0;
		while (pcsGuild && !pcsSiegeWar->m_ApAttackGuildSortList.IsEnd())
		{
			if (lIndex >= AGPMSIEGEWAR_TOTAL_ATTACK_GUILD)
				break;

			pcsSiegeWar->m_csAttackGuild.AddObject((PVOID) &pcsGuild, pcsGuild->m_szID);

			INT32	lAdopted	= 1;

			PVOID pvBuffer[4];
			pvBuffer[0] = pcsGuild->m_szID;
			pvBuffer[1] = &pcsSiegeWar->m_ulSiegeDBID;
			pvBuffer[2] = &lAdopted;

			char szSiegeWarLog[256]={0,};
			sprintf_s(szSiegeWarLog, sizeof(szSiegeWarLog), "[%s] Guild is Added AttackGuild. MasterID[%s] Point[%d] Rank[%d]",pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsGuild->m_lGuildPoint, pcsGuild->m_lRank);
			AuLogFile_s("LOG\\SiegeWarLog", szSiegeWarLog);

			// update
			EnumCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_APPLICATION, pcsSiegeWar->m_strCastleName.GetBuffer(), pvBuffer);

			++lIndex;

			pcsGuild	= pcsSiegeWar->m_ApAttackGuildSortList.GetNext();
		}
	}

	// 신청했던 길드 리스트를 모두 날려버린다.
	m_pcsAgpmSiegeWar->ClearAttackApplGuild(pcsSiegeWar);
	m_pcsAgpmSiegeWar->ClearDefenseApplGuild(pcsSiegeWar);

	return ReadySiegeWar(pcsSiegeWar->m_apcsRegionTemplate[0]);
}

BOOL AgsmSiegeWar::LoadFromDB()
{
	if (m_bLoaded)
		return TRUE;

	m_ulLoadedClockCount	= 0;

	m_hEventDBLoad	= CreateEvent(NULL, TRUE, FALSE, NULL);

	// load from db. 1st - castle
	EnumCallback(AGSMSIEGEWAR_CB_DB_SELECT_CASTLE, NULL, NULL);

	if (WaitForSingleObject(m_hEventDBLoad, INFINITE) == WAIT_FAILED)
		return FALSE;

	m_ulLoadedClockCount	= GetClockCount();

	return TRUE;
}

BOOL AgsmSiegeWar::OnSelectResultCastle(stBuddyRowset *pRowset, BOOL bEnd)
{
	if (NULL == pRowset && FALSE == bEnd)
	{
		m_bLoaded = TRUE;
		SetEvent(m_hEventDBLoad);
		return TRUE;
	}

	if (bEnd)		// castle load ended
		{
		EnumCallback(AGSMSIEGEWAR_CB_DB_SELECT_CASTLE_END, NULL, &bEnd);
		return EnumCallback(AGSMSIEGEWAR_CB_DB_SELECT_SIEGE, NULL, NULL);	// 2nd - siege
		}

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);	
	if (!lock.Result()) return FALSE;

	// SELECT CASTLEID, OWNERGUILDID FROM CASTLE
	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
	{
		UINT32	ulIndex	= 0;

		CHAR	*pszCastleName		= pRowset->Get(ul, ulIndex++);
		CHAR	*pszOwnerGuildID	= pRowset->Get(ul, ulIndex++);

		AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(pszCastleName);
		AgpdGuild		*pcsOwnerGuild	= m_pcsAgpmGuild->GetGuild(pszOwnerGuildID);
		if (pcsSiegeWar && pcsOwnerGuild)
		{
			pcsSiegeWar->m_strOwnerGuildName.SetText(pszOwnerGuildID);
			pcsSiegeWar->m_strOwnerGuildMasterName.SetText(pcsOwnerGuild->m_szMasterID);
			pcsSiegeWar->m_ucOwnerGuildMemberCount	= (UINT8) m_pcsAgpmGuild->GetMemberCount(pcsOwnerGuild);
		}
		else
		{
		}
	}
	
	EnumCallback(AGSMSIEGEWAR_CB_DB_SELECT_CASTLE_END, pRowset, &bEnd);

	m_pcsAgpmArchlord->SetArchlord(m_pcsAgpmSiegeWar->m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX].m_strOwnerGuildMasterName);
	
	return TRUE;
}

BOOL AgsmSiegeWar::OnSelectResultSiege(stBuddyRowset *pRowset, BOOL bEnd)
{
	if (NULL == pRowset && FALSE == bEnd)
		return FALSE;

	if (bEnd)
		return EnumCallback(AGSMSIEGEWAR_CB_DB_SELECT_SIEGE_APPLICATION, NULL, NULL);	// 3rd - siege application for all castle

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);	
	if (!lock.Result()) return FALSE;

	// SELECT
	//		CASTLEID, SIEGEID,APPLYBEGINDATE,APPLYELAPSED,SIEGEBEGINDATE,SIEGEELAPSED,
	//		SIEGEENDDATE,PREVENDDATE,STATUS,GUILDID FROM SIEGE
	//		WHERE (CASTLEID, SIEGEID) IN (SELECT CASTLEID, MAX(SIEGEID) FROM SIEGE GROUP BY CASTLEID)
	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
	{
		UINT32	ulIndex	= 0;

		CHAR	*pszCastleName		= pRowset->Get(ul, ulIndex++);
		CHAR	*pszSiegeID			= pRowset->Get(ul, ulIndex++);

		CHAR	*pszApplBeginDate	= pRowset->Get(ul, ulIndex++);
		CHAR	*pszApplElapsed		= pRowset->Get(ul, ulIndex++);
		CHAR	*pszSiegeBeginDate	= pRowset->Get(ul, ulIndex++);
		CHAR	*pszSiegeElapsed	= pRowset->Get(ul, ulIndex++);
		CHAR	*pszSiegeEndDate	= pRowset->Get(ul, ulIndex++);
		CHAR	*pszPrevEndDate		= pRowset->Get(ul, ulIndex++);
		CHAR	*pszStatus			= pRowset->Get(ul, ulIndex++);
		CHAR	*pszGuildID			= pRowset->Get(ul, ulIndex++);

		AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(pszCastleName);
		if (pcsSiegeWar)
		{
			pcsSiegeWar->m_ulSiegeDBID				= atoi(pszSiegeID);
			pcsSiegeWar->m_ullApplicationTimeDate	= AuTimeStamp2::ConvertOracleTimeToTimeStamp(pszApplBeginDate);
			pcsSiegeWar->m_ullNextSiegeWarTimeDate	= AuTimeStamp2::ConvertOracleTimeToTimeStamp(pszSiegeBeginDate);
			pcsSiegeWar->m_ullPrevSiegeWarTimeDate	= AuTimeStamp2::ConvertOracleTimeToTimeStamp(pszSiegeEndDate) - (INT32) ((float) m_pcsAgpmSiegeWar->GetDurationMSec(pcsSiegeWar) * 10000);
			pcsSiegeWar->m_ullProclaimTimeDate		= AuTimeStamp2::ConvertOracleTimeToTimeStamp(pszSiegeEndDate) + (UINT64) pcsSiegeWar->m_csTemplate.m_ulProclaimTimeHour * 60 * 60 * 10000000;

			pcsSiegeWar->m_eCurrentStatus			= (AgpdSiegeWarStatus) atoi(pszStatus);
			if (m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar))
			{
				pcsSiegeWar->m_ulSiegeWarStartTimeMSec	= GetClockCount() - atoi(pszSiegeElapsed) * 1000;

				if (!pcsSiegeWar->m_strOwnerGuildName.IsEmpty())
				{
					AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strOwnerGuildName.GetBuffer());
					m_pcsAgpmSiegeWar->SetDefenseGuild(pcsSiegeWar, pcsGuild, FALSE);
				}

				pcsSiegeWar->m_strCarveGuildName.SetText(pszGuildID);
			}
			else
				pcsSiegeWar->m_ulSiegeWarStartTimeMSec	= 0;

			pcsSiegeWar->m_bIsNeedInsertDB	= FALSE;
			m_pcsAgpmSiegeWar->SetCurrentStatus(pcsSiegeWar->m_lArrayIndex, pcsSiegeWar->m_eCurrentStatus, FALSE);
			pcsSiegeWar->m_bIsNeedInsertDB	= TRUE;
		}
		else
		{
		}
	}
	
	return TRUE;
}

BOOL AgsmSiegeWar::OnSelectResultSiegeApplication(stBuddyRowset *pRowset, BOOL bEnd)
{
	if (NULL == pRowset && FALSE == bEnd)
		return FALSE;

	if (bEnd)
		return EnumCallback(AGSMSIEGEWAR_CB_DB_SELECT_SIEGE_OBJECT, NULL, NULL);	// 4th - siege object	

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	// SELECT CASTLEID,SIEGEID,GUILDID,SIDE,APPLYDATE,ADOPTED FROM SIEGEAPPLICATION
	// WHERE (CASTLEID, SIEGEID) IN (SELECT CASTLEID, MAX(SIEGEID) FROM SIEGE GROUP BY CASTLEID)
	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
	{
		UINT32	ulIndex	= 0;

		CHAR	*pszCastleName	= pRowset->Get(ul, ulIndex++);
		CHAR	*pszSiegeID		= pRowset->Get(ul, ulIndex++);

		CHAR	*pszGuildID		= pRowset->Get(ul, ulIndex++);
		CHAR	*pszSide		= pRowset->Get(ul, ulIndex++);
		CHAR	*pszApplDate	= pRowset->Get(ul, ulIndex++);
		CHAR	*pszAdopted		= pRowset->Get(ul, ulIndex++);

		AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(pszCastleName);
		if (pcsSiegeWar)
		{
			AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pszGuildID);
			if (!pcsGuild)
				continue;

			AgpdGuildAttachData	*pcsAttachGuildData	= m_pcsAgpmSiegeWar->GetAttachGuildData(pcsGuild);
			if (pcsAttachGuildData)
				pcsAttachGuildData->m_ullApplDate	= AuTimeStamp2::ConvertOracleTimeToTimeStamp(pszApplDate);

			// side
			//		1 : 공성
			//		2 : 수성

			switch (atoi(pszSide)) {
				case 1:
					m_pcsAgpmSiegeWar->AddAttackApplicationGuild(pcsSiegeWar, pcsGuild);
					if (atoi(pszAdopted) == 1)
						m_pcsAgpmSiegeWar->SetAttackGuild(pcsSiegeWar, pcsGuild, FALSE);
					break;
				case 2:
					m_pcsAgpmSiegeWar->AddDefenseApplicationGuild(pcsSiegeWar, pcsGuild);
					if (atoi(pszAdopted) == 1 || atoi(pszAdopted) == 2)
						m_pcsAgpmSiegeWar->SetDefenseGuild(pcsSiegeWar, pcsGuild, FALSE);
					break;
			}
		}
		else
		{
		}
	}
	
	return TRUE;
}

BOOL AgsmSiegeWar::OnSelectResultSiegeObject(stBuddyRowset *pRowset, BOOL bEnd)
{
	if (NULL == pRowset && FALSE == bEnd)
		return FALSE;

	printf("Loading Saved Siege Object Info...\n");
	AuLogFile_s("log\\SiegeWarLog","Loading Saved Siege Object Info...");

	if (bEnd)		// db load ended
	{
		// 2007.01.18. steeple
		// 스폰하지 않은 오브젝트들 스폰해준다.
		SpawnAllSiegeWarObjectExceptIng();

		m_bLoaded = TRUE;
		SetEvent(m_hEventDBLoad);
		return TRUE;
	}

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);	
	if (!lock.Result()) return FALSE;

	//공성이 진행 중일때만 디비에 row 컬럼이 생긴다. row값이 0이라는 애기는 공성이 진행중이지 않다는 말임 supertj@20100326
	if( 0 == pRowset->m_ulRows )
	{
		printf_s("Siege war is not going on !!!\n");
		AuLogFile_s("log\\SiegeWarLog","Siege war is not going on !!!");
		return TRUE;
	}

	// SELECT CASTLEID,OBJECTNO,TID,HP,POSITION,STATUS FROM SIEGEOBJECT
	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
	{
		UINT32	ulIndex	= 0;

		CHAR	*pszCastleName		= pRowset->Get(ul, ulIndex++);
		CHAR	*pszObjectNo		= pRowset->Get(ul, ulIndex++);
		CHAR	*pszTID				= pRowset->Get(ul, ulIndex++);
		CHAR	*pszHP				= pRowset->Get(ul, ulIndex++);
		CHAR	*pszPos				= pRowset->Get(ul, ulIndex++);
		CHAR	*pszStatus			= pRowset->Get(ul, ulIndex++);
		CHAR	*pszSpecialStatus	= pRowset->Get(ul, ulIndex++);

		if (!pszObjectNo || !pszObjectNo[0] ||
			!pszTID || !pszTID[0])
			continue;

		AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(pszCastleName);
		if (m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar))
		{
			AgpdCharacterTemplate	*pcsCharacterTemplate	= m_pcsAgpmCharacter->GetCharacterTemplate(atoi(pszTID));
			if (!pcsCharacterTemplate)
				continue;

			AgpdSiegeWarMonsterType	eMonsterType	= m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacterTemplate);
			if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE || eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
				continue;

			INT32	lMonsterType	= atoi(pszObjectNo) / 1000;
			INT32	lArrayIndex		= atoi(pszObjectNo) % 1000;

			if ((AgpdSiegeWarMonsterType) lMonsterType != eMonsterType ||
				lArrayIndex < 0 || lArrayIndex >= AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER)
				continue;

			SpawnSiegeWarMonsterDirect(pcsSiegeWar, AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_OBJECT);

			AgpdCharacter	*pcsCharacter	= pcsSiegeWar->m_apcsObjectChar[lMonsterType][lArrayIndex];
			if (!pcsCharacter)
				continue;

			if (pszHP)
			{
				m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, atoi(pszHP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
				m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, atoi(pszHP), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
			}

			// update position
			//////////////////////////////////////////////////////

			if (pszStatus)
				m_pcsAgpmCharacter->UpdateActionStatus(pcsCharacter, atoi(pszStatus));
			if (pszSpecialStatus)
			{
				m_pcsAgpmCharacter->UpdateSpecialStatus(pcsCharacter, (UINT64)_atoi64(pszSpecialStatus));

				if (m_pcsAgpmCharacter->IsRepairCharacter(pcsCharacter))
				{
					AddTimer(m_pcsAgpmSiegeWar->GetRepairDuration(pcsCharacter) * 60 * 1000, pcsCharacter->m_lID, this, RepairAttackObject, pcsCharacter);
					m_pcsAgpmSiegeWar->SetRepairStartTime(pcsCharacter);
				}
			}

			if (eMonsterType == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER ||
				eMonsterType == AGPD_SIEGE_MONSTER_CATAPULT)
			{
				AgpdSiegeWarADCharacter	*pcsAttachData	= m_pcsAgpmSiegeWar->GetAttachCharacterData(pcsCharacter);
				pcsAttachData->ucRepairCount	= m_pcsAgpmSiegeWar->GetRepairCount(pcsCharacter);
			}
			
			// 결정전 3단계에선 내성문을 없애준다.
			if (m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar) &&
				AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE == pcsSiegeWar->m_eCurrentStatus && 
				pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_INNER_GATE][0])
				m_pcsAgpmCharacter->RemoveCharacter(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_INNER_GATE][0]->m_lID);
			
			// 공성정보 로깅
			char a_log[256]={0,};
			sprintf_s(a_log,"Loaded Siege Object info castleIndex[%d] ObjectTID[%d] ObjectStatus[%d]", pcsSiegeWar->m_lArrayIndex, atoi(pszTID), atoi(pszStatus));
			AuLogFile_s("log\\SiegeWarLog", a_log);
			printf_s("Loaded Siege Object info castleIndex[%d] ObjectTID[%d] ObjectStatus[%d]\n", pcsSiegeWar->m_lArrayIndex, atoi(pszTID), atoi(pszStatus));
		}
	}
	
	return TRUE;
}

BOOL AgsmSiegeWar::SetCallbackDBUpdateCastle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_UPDATE_CASTLE, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBSelectCastle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_SELECT_CASTLE, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBSelectCastleEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_SELECT_CASTLE_END, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBInsertSiege(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_INSERT_SIEGE, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBUpdateSiege(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBSelectSiege(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_SELECT_SIEGE, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBInsertSiegeApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_INSERT_SIEGE_APPLICATION, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBUpdateSiegeApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_APPLICATION, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBSelectSiegeApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_SELECT_SIEGE_APPLICATION, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBInsertSiegeObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_INSERT_SIEGE_OBJECT, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBUpdateSiegeObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_UPDATE_SIEGE_OBJECT, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBDeleteSiegeObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_DELETE_SIEGE_OBJECT, pfCallback, pClass);
}

BOOL AgsmSiegeWar::SetCallbackDBSelectSiegeObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMSIEGEWAR_CB_DB_SELECT_SIEGE_OBJECT, pfCallback, pClass);
}

BOOL AgsmSiegeWar::AddSiegeWarObjectChar(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsCharacter->m_stPos));
	if (pcsSiegeWar)
	{
		// monster이면 NPC Guild이다. 길드 이름을 설정해준다.
		//
		// 2006.09.20. steeple
		// 소환수는 붙이지 않는다.
		if (m_pcsAgpmCharacter->IsMonster(pcsCharacter) && !m_pcsAgpmCharacter->IsStatusSummoner(pcsCharacter))
		{
			AgpdGuildADChar* pcsAttachedGuild = this->m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
			if (!pcsAttachedGuild) return FALSE;

			ZeroMemory(pcsAttachedGuild->m_szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
			strncpy(pcsAttachedGuild->m_szGuildID, pcsSiegeWar->m_strCastleName, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		}
	}
	else return FALSE;


	AgpdSiegeWarMonsterType	eMonsterType	= m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter);
	if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE ||
		eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX ||
		eMonsterType == AGPD_SIEGE_MONSTER_SUMMON)
		return FALSE;

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	int i = 0;
	for ( ; i < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++i)
		if (!pcsSiegeWar->m_apcsObjectChar[eMonsterType][i])
			break;

	if (i == AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER)
		return FALSE;

	if (eMonsterType >= AGPD_SIEGE_MONSTER_GUARD_TOWER || eMonsterType <= AGPD_SIEGE_MONSTER_CATAPULT)
	{
		ASSERT(1);
	}

//#ifdef _DEBUG
//	switch(eMonsterType)
//	{
//		case AGPD_SIEGE_MONSTER_GUARD_TOWER:
//		{
//			ASSERT(i < 2);
//			break;
//		}
//
//		case AGPD_SIEGE_MONSTER_INNER_GATE:
//		{
//			ASSERT(i < 1);
//			break;
//		}
//
//		case AGPD_SIEGE_MONSTER_OUTER_GATE:
//		{
//			ASSERT(i < 2);
//			break;
//		}
//
//		case AGPD_SIEGE_MONSTER_LIFE_TOWER:
//		{
//			ASSERT(i < 1);
//			break;
//		}
//
//		case AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER:
//		{
//			ASSERT(i < 1);
//			break;
//		}
//
//		case AGPD_SIEGE_MONSTER_THRONE:
//		{
//			ASSERT(i < 1);
//			break;
//		}
//
//		case AGPD_SIEGE_MONSTER_ARCHON_EYES:
//		{
//			ASSERT(i < 2);
//			break;
//		}
//
//		case AGPD_SIEGE_MONSTER_ARCHON_EYES_TOWER:
//		{
//			ASSERT(i < 4);
//			break;
//		}
//	}
//#endif
//
	pcsSiegeWar->m_apcsObjectChar[eMonsterType][i]	= pcsCharacter;

	// disable character
	m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISABLE);

	return TRUE;
}

BOOL AgsmSiegeWar::RemoveSiegeWarObjectChar(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdSiegeWarMonsterType	eMonsterType	= m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter);
	if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE ||
		eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pcsCharacter->m_stPos));
	if (!pcsSiegeWar)
		return FALSE;

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	for (int i = 0 ; i < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++i)
	{
		if (pcsSiegeWar->m_apcsObjectChar[eMonsterType][i] == pcsCharacter)
		{
			pcsSiegeWar->m_apcsObjectChar[eMonsterType][i]	= NULL;

			return TRUE;
		}
	}

	return FALSE;
}

// 2006.06.20. steeple
// pcsCharacter 의 길드가 공성전 중이고, pcsTarget 의 길드가 같은 편일때만 return TRUE
// 아니면 return FALSE
BOOL AgsmSiegeWar::CBIsFriendGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar*	pThis = static_cast<AgsmSiegeWar*>(pClass);
	AgpdCharacter*	pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AgpdCharacter*	pcsTarget = static_cast<AgpdCharacter*>(pCustData);

	BOOL bCharacterIsAttack = FALSE;
	BOOL bTargetIsAttack = FALSE;

	AgpdSiegeWar* pcsSiegeWar1 = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfoOfCharacter(pcsCharacter, &bCharacterIsAttack);
	AgpdSiegeWar* pcsSiegeWar2 = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfoOfCharacter(pcsTarget, &bTargetIsAttack);

	if (pcsSiegeWar1 && pcsSiegeWar2 && pcsSiegeWar1 == pcsSiegeWar2 &&
		pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar1))
	{
		// 둘 다 참이거나 둘다 거짓일 때 return TRUE; !(XOR)
		if(!(bCharacterIsAttack ^ bTargetIsAttack))
			return TRUE;
	}	

	return FALSE;
}

// 2006.06.20. steeple
// pcsCharacter 의 길드가 공성전 중이고, pcsTarget 의 길드가 적일때만 return TRUE
// 아니면 return FALSE
BOOL AgsmSiegeWar::CBIsEnemyGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar*	pThis = static_cast<AgsmSiegeWar*>(pClass);
	AgpdCharacter*	pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AgpdCharacter*	pcsTarget = static_cast<AgpdCharacter*>(pCustData);

	BOOL bCharacterIsAttack = FALSE;
	BOOL bTargetIsAttack = FALSE;

	AgpdSiegeWar* pcsSiegeWar1 = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfoOfCharacter(pcsCharacter, &bCharacterIsAttack);
	AgpdSiegeWar* pcsSiegeWar2 = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfoOfCharacter(pcsTarget, &bTargetIsAttack);

	if (pcsSiegeWar1 && pcsSiegeWar2 && pcsSiegeWar1 == pcsSiegeWar2 &&
		pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar1))
	{
		if (pThis->m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar1))
		{
			// 결정전시 길드가 다르면 적이다.
			if (pThis->m_pcsAgpmGuild->GetGuild(pcsCharacter) != pThis->m_pcsAgpmGuild->GetGuild(pcsTarget))
				return TRUE;
		}

		// 둘이 서로 다르면 return TRUE; (XOR)
		if(bCharacterIsAttack ^ bTargetIsAttack)
				return TRUE;
	}	

	return FALSE;
}

BOOL AgsmSiegeWar::RepairAttackObject(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (!pClass || !pvData)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;

	// 수리한 lCID하고 pvData로 넘어온 AgpdCharacter 하고 같은 놈인지 본다.

	AgpdCharacter	*pcsCharacter	= pThis->m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter ||
		pcsCharacter != (AgpdCharacter *) pvData)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
	if (!pcsSiegeWar)
		return FALSE;

	if (!pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar))
		return FALSE;

	// 수리 가능한 공성몹인지 본다.

	// 현재 HP는 0이어야 한다.
	INT32	lHP	= pThis->m_pcsAgpmFactors->GetHP((AgpdFactor *) pThis->m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
	if (lHP > 0)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	// special status는 repair 상태이어야 함.
	if (!pThis->m_pcsAgpmCharacter->IsRepairCharacter(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	// HP를 채워준다.
	PVOID pvPacketFactor	= NULL;

	if (!pThis->m_pcsAgsmFactors->Resurrection(&pcsCharacter->m_csFactor, &pvPacketFactor))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if (pvPacketFactor)
	{
		// 다시 살아났다는 패킷을 보낸다.
		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= pThis->m_pcsAgsmCharacter->MakePacketFactor(pcsCharacter, pvPacketFactor, &nPacketLength);

		if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
			pThis->m_pcsAgsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_3);
	}

	// update character status
	pThis->m_pcsAgpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_NORMAL);
	pThis->m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_REPAIR);
	if (pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter) == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER)
		pThis->m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISABLE);
	pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_USE);

	// 수리된 횟수를 증가시킨다.
	AgpdSiegeWarADCharacter	*pcsAttachData	= pThis->m_pcsAgpmSiegeWar->GetAttachCharacterData(pcsCharacter);
	pcsAttachData->m_ucRepairedCount++;

	pcsCharacter->m_Mutex.Release();

	if (pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter) == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER)
	{
		if (pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0] &&
			pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0] &&
			pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][1])
		{
			BOOL	bLeft	= AuMath::ConsiderLeftPoint(
														pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]->m_stPos,
														pcsCharacter->m_stPos,
														(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0] == pcsCharacter) ? pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][1]->m_stPos : pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER][0]->m_stPos);

			pThis->SendPacketStatusInfo(
					(bLeft) ? AGPMSIEGE_STATUS_REPAIR_END_LEFT_ATTACK_RES_TOWER : AGPMSIEGE_STATUS_REPAIR_END_RIGHT_ATTACK_RES_TOWER,
					pcsSiegeWar,
					NULL,
					NULL,
					AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER,
					0,
					0);
		}
	}

	return TRUE;
}

BOOL AgsmSiegeWar::TeleportGuildMember(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (!pClass || !pvData)
		return FALSE;

	AgsmSiegeWar	*pThis		= (AgsmSiegeWar *)	pClass;
	AgpdGuild		*pcsGuild	= (AgpdGuild *)		pvData;

	// 아크로드 공성지역에 있는 해당 길드원을 모두 마지막으로 거친 마을로 보내버린다.
	AgpdSiegeWar	*pcsSiegeWar	= &pThis->m_pcsAgpmSiegeWar->m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX];

	if (pcsGuild->m_Mutex.WLock())
	{
		INT32 lIndex = 0;
		for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**) pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
														ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
		{
			if(!ppcsMember || !*ppcsMember)
				break;

			AgpdCharacter	*pcsMember	= pThis->m_pcsAgpmCharacter->GetCharacterLock((*ppcsMember)->m_szID);
			if (pcsMember)
			{
				AgpdSiegeWar	*pcsCurrentSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsMember);
				if (pcsSiegeWar == pcsCurrentSiegeWar)
				{
					AuPOS	stNewTargetPos	= pcsMember->m_stPos;
					pThis->m_pcsAgpmEventBinding->GetBindingPositionForResurrection(pcsMember, &stNewTargetPos);

					pThis->m_pcsAgpmCharacter->StopCharacter(pcsMember, NULL);
					pThis->m_pcsAgpmCharacter->UpdatePosition(pcsMember, &stNewTargetPos, FALSE, TRUE);
				}

				pcsMember->m_Mutex.Release();
			}
		}

		pcsGuild->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmSiegeWar::ResetSiegeWarObject(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (!pClass || !pvData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pvData;

	//STOPWATCH2(pThis->GetModuleName(), _T("ResetSiegeWarObject"));

	// 성주 길드를 제외한 모든 공성지역 내에 있는 유저들을 공성 부활탑 근처로 소환시킨다.
	INT32			lIndex	= 0;
	AgpdCharacter	*pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	while (pcsCharacter)
	{
		if (pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter) && pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter) != 0)
		{
			if (pcsCharacter->m_Mutex.WLock())
			{
				AgpdSiegeWar	*pcsCurrentSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
				if (pcsCurrentSiegeWar == pcsSiegeWar &&
					!pThis->m_pcsAgpmSiegeWar->IsOwnerGuild(pcsCharacter, pcsSiegeWar))
				{
					AuPOS stNewPosition;
					if (!pThis->m_pcsAgpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &stNewPosition))
						stNewPosition = pcsCharacter->m_stPos;
					
					pThis->m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
					pThis->m_pcsAgpmCharacter->UpdatePosition(pcsCharacter, &stNewPosition, 0, TRUE);
				}

				pcsCharacter->m_Mutex.Release();
			}
		}

		pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	}

	pThis->RemoveAllSiegeWarObject(pcsSiegeWar);

	// 2007.01.18. steeple
	// 이제 EventSpawn Idle 에서는 스폰안하고 여기서 직접해주는 걸로 바꿧다.
	pThis->SpawnSiegeWarMonster(pcsSiegeWar, AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_OBJECT);
	return TRUE;
}

BOOL AgsmSiegeWar::TeleportArchlordBattle(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (!pClass || !pvData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= (AgpdSiegeWar *)	pvData;

	//STOPWATCH2(pThis->GetModuleName(), _T("TeleportArchlordBattle"));

	// 성주(아크로드)를 제외한 모든 유저를 마지막으로 방문한 마을로 보내버린다.
	INT32			lIndex	= 0;
	AgpdCharacter	*pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	while (pcsCharacter)
	{
		if (pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter) && pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter) != 0)
		{
			if (pcsCharacter->m_Mutex.WLock())
			{
				AgpdSiegeWar	*pcsCurrentSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
				if (pcsCurrentSiegeWar == pcsSiegeWar &&
					!pThis->m_pcsAgpmSiegeWar->IsOwnerGuild(pcsCharacter, pcsSiegeWar))
				{
					AuPOS	stNewTargetPos	= pcsCharacter->m_stPos;
					pThis->m_pcsAgpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &stNewTargetPos);

					pThis->m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
					pThis->m_pcsAgpmCharacter->UpdatePosition(pcsCharacter, &stNewTargetPos, FALSE, TRUE);
				}

				pcsCharacter->m_Mutex.Release();
			}
		}

		pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	}

	return pThis->RemoveAllSiegeWarObject(pcsSiegeWar);
}

// 2007.01.18. steeple
// Object 로딩이 끝난 후 공성진행중이지 않은 성의 Object 를 스폰시켜준다.
BOOL AgsmSiegeWar::SpawnAllSiegeWarObjectExceptIng()
{
	//STOPWATCH2(GetModuleName(), _T("SpawnAllSiegeWarObjectExceptIng"));

	for(int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		AgpdSiegeWar* pcsSiegeWarData = &m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i];
		if(!pcsSiegeWarData)
			continue;

		if(m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWarData) == FALSE)
		{
			SpawnSiegeWarMonster(pcsSiegeWarData, AGPDSPAWN_SIEGEWAR_TYPE_SIEGEWAR_OBJECT);

			// HP 가 0 으로 된 경우도 있다. 채워준다. 2007.02.21. steeple
			for(int j = 0; j < AGPD_SIEGE_MONSTER_TYPE_MAX; ++j)
			{
				for(int k = 0; k < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++k)
				{
					if (!pcsSiegeWarData->m_apcsObjectChar[j][k])
						break;

					PVOID pvPacketFactor	= NULL;
					if(!m_pcsAgsmFactors->Resurrection(&pcsSiegeWarData->m_apcsObjectChar[j][k]->m_csFactor, &pvPacketFactor))
						return FALSE;

					// 다시 살아났다는 패킷을 보낸다.
					m_pcsAgsmCharacter->SendPacketFactor(pvPacketFactor, pcsSiegeWarData->m_apcsObjectChar[j][k], PACKET_PRIORITY_3);
				}
			}
		}
	}

	return TRUE;
}

BOOL AgsmSiegeWar::RemoveAllSiegeWarObject(AgpdSiegeWar *pcsSiegeWar)
{
	//STOPWATCH2(GetModuleName(), _T("RemoveAllSiegeWarObject"));

	if (!pcsSiegeWar)
		return FALSE;

	AuAutoLock lock(m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	for (int i = 0; i < AGPD_SIEGE_MONSTER_TYPE_MAX; ++i)
	{
		for (int j = 0; j < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++j)
		{
			if (!pcsSiegeWar->m_apcsObjectChar[i][j])
				break;

			// 모두 지운다. 지우고 나면 AgsmEventSpawn에서 바로 다시 스폰 시킨다.
			m_pcsAgpmCharacter->RemoveCharacter(pcsSiegeWar->m_apcsObjectChar[i][j]->m_lID, FALSE, FALSE);
		}
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CBGetTarget(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	PVOID	*ppvBuffer	= (PVOID *)	pData;

	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)	ppvBuffer[0];
	INT_PTR			**apcsTargetChar	= (INT_PTR **)		ppvBuffer[1];
	INT32			*plNumCID			= (INT32 *)		ppvBuffer[2];

	if (!pThis->m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsCharacter))
		return FALSE;

	AgpdSiegeWarMonsterType	eMonsterType	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter);

	if (eMonsterType == AGPD_SIEGE_MONSTER_CATAPULT)
	{
		// 투석기 포탄이 있는지 본다.
		if (!pThis->m_pcsAgpmSiegeWar->CheckNeedItemForUse(pcsCharacter))
		{
			// 이 투석기는 포탄이 없다. 사용중 상태를 해제시킨다.
			pThis->m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_USE);
			pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISABLE);
			return FALSE;
		}

		// 투석기인 경우, 같은 공성 지역내에 있는 성벽, 성문들중 제일 가까운 놈을 때린다.
		AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
		if (pcsSiegeWar)
		{
			AgpdCharacter	*pcsNearTarget	= NULL;
			FLOAT			fNearDistance	= (-1.0f);

			for (int i = 0; i < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++i)
			{
				if (!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_GUARD_TOWER][i])
					break;

				FLOAT	fDistance	= abs(pcsCharacter->m_stPos.x - pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_GUARD_TOWER][i]->m_stPos.x);

				if (fNearDistance == (-1.0f) ||
					fNearDistance > fDistance)
				{
					fNearDistance	= fDistance;
					pcsNearTarget	= pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_GUARD_TOWER][i];
				}
			}

			for (int i = 0; i < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++i)
			{
				if (!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_OUTER_GATE][i])
					break;

				FLOAT	fDistance	= abs(pcsCharacter->m_stPos.x - pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_OUTER_GATE][i]->m_stPos.x);

				if (fNearDistance == (-1.0f) ||
					fNearDistance > fDistance)
				{
					fNearDistance	= fDistance;
					pcsNearTarget	= pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_OUTER_GATE][i];
				}
			}

			*plNumCID	= 1;
			apcsTargetChar[0]	= (INT_PTR *) pcsNearTarget;
		}
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter) || _tcslen(pcsCharacter->m_szID) == 0)
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBEnterGameWorld"));

	// 공성과 관련된 정보를 몽땅 보내준다.

	AuAutoLock lock(pThis->m_pcsAgpmSiegeWar->m_Mutex);
	if (!lock.Result()) return FALSE;

	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		pThis->SendPacketCastleInfo(&pThis->m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i], pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

		// AttackGuild
		INT16	nTotalPage		= (pThis->m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i].m_csAttackGuild.GetObjectCount() / AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE) + 1;
		for(int j=0; j < nTotalPage; j++)
		{
			INT16	nPacketLength	= 0;
			PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketAttackGuildList(&pThis->m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i], j, &nPacketLength);

			if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
				pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
		}

		// DefenseGuild
		{
			INT16 nPacketLength	= 0;
			PVOID pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketDefenseGuildList(&pThis->m_pcsAgpmSiegeWar->m_csSiegeWarInfo[i], &nPacketLength);

			if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
				pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
		}
	}

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
	if (pcsSiegeWar && pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar))
	{
		UINT32	ulRemainTimeSec	= ((pcsSiegeWar->m_ulSiegeWarStartTimeMSec + pThis->m_pcsAgpmSiegeWar->GetDurationMSec(pcsSiegeWar)) - pThis->GetClockCount()) / 1000;

		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketSyncMessage(AGPMSIEGE_MESSAGE_SYNC_END_TIME, (UINT16) ulRemainTimeSec, &nPacketLength);

		if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
			pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
	}

	// 아크로드이고 결정전이 진행중인 상태면...
	if (pThis->m_pcsAgpmArchlord->IsArchlord(pcsCharacter->m_szID)
		&& pThis->m_pcsAgpmSiegeWar->IsStarted(pThis->m_pcsAgpmSiegeWar->GetArchlordCastle())
		)
	{
		// 접시 업뎃!
		pThis->m_ulLatestConnOfArchlord = pThis->GetClockCount();
	}

	return FALSE;
}

BOOL AgsmSiegeWar::CBSetCharacterGameData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	// 성주인지 본다.
	if (pThis->m_pcsAgpmSiegeWar->IsCastleOwner(&pThis->m_pcsAgpmSiegeWar->m_csSiegeWarInfo[AGPMSIEGEWAR_HUMAN_CASTLE_INDEX], pcsCharacter->m_szID))
		pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_HUMAN_CASTLE_OWNER);
	else if (pThis->m_pcsAgpmSiegeWar->IsCastleOwner(&pThis->m_pcsAgpmSiegeWar->m_csSiegeWarInfo[AGPMSIEGEWAR_ORC_CASTLE_INDEX], pcsCharacter->m_szID))
		pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_ORC_CASTLE_OWNER);
	else if (pThis->m_pcsAgpmSiegeWar->IsCastleOwner(&pThis->m_pcsAgpmSiegeWar->m_csSiegeWarInfo[AGPMSIEGEWAR_MOONELF_CASTLE_INDEX], pcsCharacter->m_szID))
		pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_MOONELF_CASTLE_OWNER);
	else if (pThis->m_pcsAgpmSiegeWar->IsCastleOwner(&pThis->m_pcsAgpmSiegeWar->m_csSiegeWarInfo[AGPMSIEGEWAR_DRAGONSCION_CASTLE_INDEX], pcsCharacter->m_szID))
		pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DRAGONSCION_CASTLE_OWNER);

	return TRUE;
}

BOOL AgsmSiegeWar::CBCheckLoginPosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (!pThis->m_pcsAgpmSiegeWar->SetAttackGuildTargetPosition(pcsCharacter, &pcsCharacter->m_stPos) &&
		!pThis->m_pcsAgpmSiegeWar->SetDefenseGuildTargetPosition(pcsCharacter, &pcsCharacter->m_stPos))
		pThis->SetNotOwnerGuildTargetPosition(pcsCharacter, &pcsCharacter->m_stPos);

	return TRUE;
}

BOOL AgsmSiegeWar::CBCheckReturnPosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AuPOS			*pstReturnPos	= (AuPOS *)			pCustData;

	if (!pThis->m_pcsAgpmSiegeWar->SetAttackGuildTargetPosition(pcsCharacter, pstReturnPos) &&
		!pThis->m_pcsAgpmSiegeWar->SetDefenseGuildTargetPosition(pcsCharacter, pstReturnPos))
		pThis->SetNotOwnerGuildTargetPosition(pcsCharacter, pstReturnPos);

	return TRUE;
}

BOOL AgsmSiegeWar::CBCharacterCancelCarving(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	return pThis->CancelCarving(pcsCharacter);
}

BOOL AgsmSiegeWar::CBItemCancelCarving(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis	= (AgsmSiegeWar *)	pClass;
	AgpdItem		*pcsItem	= (AgpdItem *)	pData;

	return pThis->CancelCarving(pcsItem->m_pcsCharacter);
}

BOOL AgsmSiegeWar::CancelCarving(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= m_pcsAgpmSiegeWar->GetAttachCharacterData(pcsCharacter);
	if (pcsAttachData->m_bIsCarving)
	{
		AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
		if (!pcsSiegeWar || !pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0])
		{
			pcsAttachData->m_bIsCarving	= FALSE;
			return FALSE;
		}

		AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pcsCharacter);
		if (!pcsGuild)
		{
			pcsAttachData->m_bIsCarving	= FALSE;
			return FALSE;
		}

		AgpdSiegeWarADCharacter	*pcsThroneAttachData	= m_pcsAgpmSiegeWar->GetAttachCharacterData(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]);

		if (pcsThroneAttachData->m_strCarveGuildName.GetLength() > 0 &&
			strcmp(pcsThroneAttachData->m_strCarveGuildName.GetBuffer(), pcsGuild->m_szID) == 0)
		{
			m_pcsAgpmSiegeWar->CancelCarveASeal(pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_THRONE][0]);

			SendPacketCarveASeal(pcsSiegeWar, AGPMSIEGE_RESULT_CANCEL_CARVE_A_SEAL, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

			SendPacketStatusInfo(AGPMSIEGE_STATUS_CANCEL_CARVE_GUILD,
				pcsSiegeWar,
				pcsGuild->m_szID,
				pcsGuild->m_szMasterID,
				AGPD_SIEGE_MONSTER_TYPE_NONE,
				0,
				0);
		}

		pcsAttachData->m_bIsCarving	= FALSE;
	}

	return TRUE;
}

BOOL AgsmSiegeWar::SendPacketToSiegeWar(PVOID pvPacket, INT16 nPacketLength, AgpdSiegeWar *pcsSiegeWar, PACKET_PRIORITY ePriority)
{
	//STOPWATCH2(GetModuleName(), _T("SendPacketToSiegeWar"));

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER) || !pcsSiegeWar)
		return FALSE;

	INT32	lIndex	= 0;
	UINT32	*pulNID	= (UINT32 *) m_acsSiegeWarRegionAdmin[pcsSiegeWar->m_lArrayIndex].GetObjectSequence(&lIndex);

	while (pulNID)
	{
		SendPacket(pvPacket, nPacketLength, *pulNID, ePriority);

		pulNID	= (UINT32 *) m_acsSiegeWarRegionAdmin[pcsSiegeWar->m_lArrayIndex].GetObjectSequence(&lIndex);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar		*pThis				= (AgsmSiegeWar *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	INT16				nPrevRegion			= *(INT16 *)			pCustData;

	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRegionChange"));

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	AgpdSiegeWar	*pcsSiegeWar		= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
	AgpdSiegeWar	*pcsPrevSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(pThis->m_pcsApmMap->GetTemplate(nPrevRegion)));

	if (pcsSiegeWar && pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar) && !pcsPrevSiegeWar)
	{
		pThis->SendPacketCastleInfo(pcsSiegeWar, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

		UINT32	ulRemainTimeSec	= ((pcsSiegeWar->m_ulSiegeWarStartTimeMSec + pThis->m_pcsAgpmSiegeWar->GetDurationMSec(pcsSiegeWar)) - pThis->GetClockCount()) / 1000;

		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketSyncMessage(AGPMSIEGE_MESSAGE_SYNC_END_TIME, (UINT16) ulRemainTimeSec, &nPacketLength);

		if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
			pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
	}

	if (pcsSiegeWar != pcsPrevSiegeWar)
	{
		if (pcsPrevSiegeWar)
			pThis->m_acsSiegeWarRegionAdmin[pcsPrevSiegeWar->m_lArrayIndex].RemoveObject(pcsAgsdCharacter->m_dpnidCharacter);

		if (pcsSiegeWar)
			pThis->m_acsSiegeWarRegionAdmin[pcsSiegeWar->m_lArrayIndex].AddObject(&pcsAgsdCharacter->m_dpnidCharacter, pcsAgsdCharacter->m_dpnidCharacter);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CBAddCharacterToMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmSiegeWar		*pThis				= (AgsmSiegeWar *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
	if (pcsSiegeWar)
		pThis->m_acsSiegeWarRegionAdmin[pcsSiegeWar->m_lArrayIndex].AddObject(&pcsAgsdCharacter->m_dpnidCharacter, pcsAgsdCharacter->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmSiegeWar::CBRemoveCharacterFromMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmSiegeWar		*pThis				= (AgsmSiegeWar *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacterFromMap"));

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
	if (pcsSiegeWar)
		pThis->m_acsSiegeWarRegionAdmin[pcsSiegeWar->m_lArrayIndex].RemoveObject(pcsAgsdCharacter->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmSiegeWar::CBSendPacketCharacterView(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID			*ppvBuffer		= (PVOID *)	pCustData;

	UINT_PTR		ulNID			= UINT_PTR(ppvBuffer[0]);
	BOOL			bGroup			= PtrToInt(ppvBuffer[1]);
	BOOL			bIsExceptSelf	= PtrToInt(ppvBuffer[2]);

	AgpdSiegeWarMonsterType	eMonsterType	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter);
	if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE ||
		eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return TRUE;

	if (eMonsterType != AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER &&
		eMonsterType != AGPD_SIEGE_MONSTER_CATAPULT)
		return TRUE;

	if (!pThis->m_pcsAgpmCharacter->IsUseCharacter(pcsCharacter) &&
		!pThis->m_pcsAgpmCharacter->IsRepairCharacter(pcsCharacter))
		return TRUE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketUpdateAttackObject(pcsCharacter, &nPacketLength);

	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
	{
		if(bGroup)
		{
			if(bIsExceptSelf)
				pThis->m_pcsAgsmAOIFilter->SendPacketGroupExceptSelf(pvPacket, nPacketLength, ulNID, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_4);
			else
				pThis->m_pcsAgsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);
		}
		else
			pThis->SendPacket(pvPacket, nPacketLength, (UINT32) ulNID, PACKET_PRIORITY_4);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CBSendPacketCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			*pulNID			= (UINT32 *)		pCustData;

	AgpdSiegeWarMonsterType	eMonsterType	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter);
	if (eMonsterType != AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER &&
		eMonsterType != AGPD_SIEGE_MONSTER_CATAPULT)
		return TRUE;

	if (!pThis->m_pcsAgpmCharacter->IsUseCharacter(pcsCharacter) &&
		!pThis->m_pcsAgpmCharacter->IsRepairCharacter(pcsCharacter))
		return TRUE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmSiegeWar->MakePacketUpdateAttackObject(pcsCharacter, &nPacketLength);;

	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		pThis->SendPacket(pvPacket, nPacketLength, *pulNID, PACKET_PRIORITY_4);

	return TRUE;
}

BOOL AgsmSiegeWar::CBStartSiegeWarCommand(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);

	if (!pcsSiegeWar)
		return FALSE;

	pThis->ConfirmGuild(pcsSiegeWar);
	pThis->StartSiegeWar(pThis->m_pcsApmMap->GetTemplate(pThis->m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z)), FALSE);
	pcsSiegeWar->m_ulIdleTimeData = pThis->GetClockCount();

	return TRUE;
}

BOOL AgsmSiegeWar::CBEndSiegeWarCommand(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsCharacter);

	if (!pcsSiegeWar)
		return FALSE;

	// timeout 시간이 되었다.
	// 공성전을 끝낸다.
	pcsSiegeWar->m_ulIdleTimeData = 0;

	pThis->TimeOver(pcsSiegeWar->m_lArrayIndex);

	if (!pThis->m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar))
	{
		pThis->ResetSiegeWar(pcsSiegeWar);
		pThis->SetNextSiegeWarTime(pcsSiegeWar);
	}

	return TRUE;
}

BOOL AgsmSiegeWar::CBEndArchlordBattleCommand(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX);

	if (!pcsSiegeWar)
		return FALSE;

	pThis->ProcessFailedArchlordBattle();

	pThis->ResetSiegeWar(pcsSiegeWar);

	pThis->SetNextArchlordSiegeWarTime(pcsSiegeWar);

	return TRUE;
}

BOOL AgsmSiegeWar::CBSetArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	CHAR			*pszID			= (CHAR *)			pData;

	AgpdCharacter	*pcsArchlord	= pThis->m_pcsAgpmCharacter->GetCharacterLock(pszID);
	if (!pcsArchlord)
		return FALSE;

	AgpdGuild	*pcsGuild	= pThis->m_pcsAgpmGuild->GetGuildLock(pcsArchlord);
	if (!pcsGuild)
	{
		pcsArchlord->m_Mutex.Release();
		return FALSE;
	}

	BOOL	bResult	= pThis->m_pcsAgpmSiegeWar->SetNewOwner(pThis->m_pcsAgpmSiegeWar->GetArchlordCastle(), pcsGuild);

	pcsGuild->m_Mutex.Release();
	pcsArchlord->m_Mutex.Release();

	return bResult;
}

BOOL AgsmSiegeWar::CBCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;

	return pThis->m_pcsAgpmSiegeWar->SetNewOwner(pThis->m_pcsAgpmSiegeWar->GetArchlordCastle(), NULL);
}

BOOL AgsmSiegeWar::CBStartArchlordSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX);
	if (!pcsSiegeWar)
		return FALSE;

	pThis->ConfirmGuild(pcsSiegeWar);
	pThis->StartSiegeWar(pcsSiegeWar->m_apcsRegionTemplate[0], FALSE);
	pcsSiegeWar->m_ulIdleTimeData = pThis->GetClockCount();

	return TRUE;
}

BOOL AgsmSiegeWar::CBEndArchlordSiegeWar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmSiegeWar	*pThis			= (AgsmSiegeWar *)	pClass;
	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX);
	if (!pcsSiegeWar)
		return FALSE;

	pThis->SetNextArchlordSiegeWarTime(pcsSiegeWar);

	return TRUE;
}

BOOL AgsmSiegeWar::CBSetCastleOwner(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmSiegeWar *pThis = (AgsmSiegeWar *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	CHAR* szCastleName = (CHAR*)pCustData;//JK_성주 설정 오류 수정
	//INT32 iCastleIdx = atoi((char*)pCustData);


	// 기존것을 취소하고
	CBCancelCastleOwner(pData, pClass, pCustData);

	AgpdSiegeWar* pcsCastle = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(szCastleName);
	//AgpdSiegeWar* pcsCastle = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(iCastleIdx);

	if (!pcsCastle)
		return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(pcsCharacter);
	if (!pcsGuild)
		return FALSE;

	pThis->m_pcsAgpmSiegeWar->SetNewOwner(pcsCastle, pcsGuild);

	// 2007.02.21. steeple
	if(pcsCastle->m_lArrayIndex == AGPMSIEGEWAR_HUMAN_CASTLE_INDEX)
		pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_HUMAN_CASTLE_OWNER);
	else if (pcsCastle->m_lArrayIndex == AGPMSIEGEWAR_ORC_CASTLE_INDEX)
		pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_ORC_CASTLE_OWNER);
	else if (pcsCastle->m_lArrayIndex == AGPMSIEGEWAR_MOONELF_CASTLE_INDEX)
		pThis->m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_MOONELF_CASTLE_OWNER);
	else if (pcsCastle->m_lArrayIndex == AGPMSIEGEWAR_DRAGONSCION_CASTLE_INDEX)
		pThis->m_pcsAgpmCharacter->UpdateSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DRAGONSCION_CASTLE_OWNER);

	pThis->SendPacketCastleInfoToAll(pcsCastle);

	return TRUE;
}

BOOL AgsmSiegeWar::CBCancelCastleOwner(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmSiegeWar *pThis = (AgsmSiegeWar *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	CHAR* szCastleName = (CHAR*)pCustData;

	AgpdSiegeWar* pcsCastle = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(szCastleName);
	if (!pcsCastle)
		return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuild(pcsCastle->m_strOwnerGuildName.GetBuffer());
	if (!pcsGuild)
		return FALSE;

	// 2007.02.21. steeple
	// 길마가 있으면 박탈한다.
	AgpdCharacter* pcsPrevOwner = pThis->m_pcsAgpmCharacter->GetCharacterLock(pcsCastle->m_strOwnerGuildMasterName.GetBuffer());
	if(pcsPrevOwner)
	{
		if(pcsCastle->m_lArrayIndex == AGPMSIEGEWAR_HUMAN_CASTLE_INDEX)
			pThis->m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsPrevOwner, AGPDCHAR_SPECIAL_STATUS_HUMAN_CASTLE_OWNER);
		else if (pcsCastle->m_lArrayIndex == AGPMSIEGEWAR_ORC_CASTLE_INDEX)
			pThis->m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsPrevOwner, AGPDCHAR_SPECIAL_STATUS_ORC_CASTLE_OWNER);
		else if (pcsCastle->m_lArrayIndex == AGPMSIEGEWAR_MOONELF_CASTLE_INDEX)
			pThis->m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsPrevOwner, AGPDCHAR_SPECIAL_STATUS_MOONELF_CASTLE_OWNER);

		pcsPrevOwner->m_Mutex.Release();
	}

	pThis->m_pcsAgpmSiegeWar->SetNewOwner(pcsCastle, NULL);

	// 2007.02.21. steeple
	pThis->SendPacketCastleInfoToAll(pcsCastle);

	return TRUE;
}

BOOL AgsmSiegeWar::SendPacketStatusInfo(AgpmSiegeStatusType eStatusType, AgpdSiegeWar *pcsSiegeWar, CHAR *pszGuildName, CHAR *pszGuildMasterName, AgpdSiegeWarMonsterType eMonsterType, UINT32 ulTimeSec, UINT8 ucActiveCount)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketStatusInfo(
												eStatusType,
												pcsSiegeWar,
												pszGuildName,
												pszGuildMasterName,
												eMonsterType,
												ulTimeSec,
												ucActiveCount,
												&nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacketToSiegeWar(pvPacket, nPacketLength, pcsSiegeWar);
}

BOOL AgsmSiegeWar::SendAddAttackApplicationResult(AgpdSiegeWar *pcsSiegeWar, AgpdCharacter *pcsCharacter)
{
	if (!pcsSiegeWar || !pcsCharacter)
		return FALSE;

	AgpdGuild	*pcsAddGuild	= m_pcsAgpmGuild->GetGuild(pcsCharacter);
	if (!pcsAddGuild)
		return FALSE;

	AuAutoLock Lock(pcsSiegeWar->m_ApAttackGuildSortList.m_Mutex);
	if (!Lock.Result()) return FALSE;

	INT32	lIndex	= 0;
    AgpdGuild	*pcsGuild	= pcsSiegeWar->m_ApAttackGuildSortList.GetHead();

	while (pcsGuild && !pcsSiegeWar->m_ApAttackGuildSortList.IsEnd())
	{
		if (pcsGuild == pcsAddGuild)
			break;

		pcsGuild	= pcsSiegeWar->m_ApAttackGuildSortList.GetNext();

		++lIndex;
	}

	if (pcsSiegeWar->m_ApAttackGuildSortList.IsEnd() ||
		!pcsGuild)
		return FALSE;

	INT16	nPage	= (lIndex / AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE) + 1;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmSiegeWar->MakePacketAttackApplGuildList(pcsSiegeWar, nPage, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	return SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
}

BOOL AgsmSiegeWar::SetTestServerNextSiegeWarTime(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	INT32	lSiegeWarTime = 21;		// 공성시간 PM 9:00
	UINT64	ullCurrentTimeDate	= AuTimeStamp2::GetCurrentTimeStamp();

	FILETIME	file_time;
	SYSTEMTIME	system_time;

	CopyMemory(&file_time, &ullCurrentTimeDate, sizeof(UINT64));
	if (::FileTimeToSystemTime(&file_time, &system_time) == 0)
		return FALSE;

	INT32	lRestTimeSecond	= 0;			// 남은시간(초단위)
	if (system_time.wHour < lSiegeWarTime)
	{
		lRestTimeSecond	= (lSiegeWarTime - system_time.wHour - 1) * 60 * 60 + (60 - system_time.wMinute) * 60;
	}
	else
	{
		lRestTimeSecond = (24 - system_time.wHour - 1 + lSiegeWarTime) * 60 * 60 + (60 - system_time.wMinute) * 60;
	}

	UINT64	ullNextSiegeWarTimeDate	= ullCurrentTimeDate + (UINT64) lRestTimeSecond * (UINT64) 10000000;
	UINT64	ullApplicationTimeDate	= ullNextSiegeWarTimeDate - ((UINT64) 60 * 60 * 10000000);

	pcsSiegeWar->m_ullNextSiegeWarTimeDate	= ullNextSiegeWarTimeDate;
	pcsSiegeWar->m_ullApplicationTimeDate	= ullApplicationTimeDate;

	return ProclaimSiegeWar(pcsSiegeWar->m_apcsRegionTemplate[0]);
}

BOOL AgsmSiegeWar::SetNextSiegeWarTime(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	// 2007.08.01. steeple
	// 한국 테섭에선 테스트 서버 시간 설정으로 가게 한다.
	if (m_pcsAgpmConfig->IsTestServer() && g_eServiceArea == AP_SERVICE_AREA_KOREA)
		return SetTestServerNextSiegeWarTime(pcsSiegeWar);
 
	const INT32 lSiegeWarTime  = pcsSiegeWar->m_csTemplate.m_ulSiegeWarBeginTime;
	UINT64	ullCurrentTimeDate	= AuTimeStamp2::GetCurrentTimeStamp();

	SYSTEMTIME	system_time;
	::GetLocalTime( &system_time );

	INT32	lRestTimeSecond	= 0;

	if (system_time.wHour <= lSiegeWarTime - 1)
	{
		lRestTimeSecond	= (lSiegeWarTime - system_time.wHour - 1) * HourBySecond + (60 - system_time.wMinute) * 60;
		lRestTimeSecond	+= (6 - system_time.wDayOfWeek) * DayBySecond;
	}
	else
	{
		lRestTimeSecond = (24 - system_time.wHour - 1 + lSiegeWarTime) * HourBySecond + (60 - system_time.wMinute) * 60;

		if (6 - system_time.wDayOfWeek - 1 < 0)
			lRestTimeSecond	+= 6 * DayBySecond;
		else
			lRestTimeSecond	+= (6 - system_time.wDayOfWeek - 1) * DayBySecond;
	}

	// 아크로드 공성이 매주 토요일 4째주에 열린다. 4째주 토요일인 경우 한주 뒤에 열린다.
	INT32	lNextSaturday	= system_time.wDay;
	if (system_time.wDayOfWeek == 6)
		lNextSaturday += 7;
	else
		lNextSaturday += (6 - system_time.wDayOfWeek);

	if ( AP_SERVICE_AREA_WESTERN != g_eServiceArea )
	{
		if (lNextSaturday >= 22 && lNextSaturday <= 28)
			lRestTimeSecond	+= 7 * DayBySecond;
	}

	UINT64	ullNextSiegeWarTimeDate	= ullCurrentTimeDate + (UINT64) lRestTimeSecond * (UINT64) 10000000;
	UINT64	ullApplicationTimeDate	= ullCurrentTimeDate + pcsSiegeWar->m_csTemplate.m_ulApplicationTimeHour * (UINT64) 60 * (UINT64) 60 * (UINT64) 10000000;
#ifdef _DEBUG
	PrintTimeStamp("ullNextSiegeWarTimeDate",ullNextSiegeWarTimeDate);
	PrintTimeStamp("ullApplicationTimeDate",ullApplicationTimeDate);
#endif
	if (ullApplicationTimeDate > ullNextSiegeWarTimeDate - ((UINT64) 60 * 60 * 10000000))
		ullApplicationTimeDate	= ullNextSiegeWarTimeDate - ((UINT64) 60 * 60 * 10000000);

	pcsSiegeWar->m_ullNextSiegeWarTimeDate	= ullNextSiegeWarTimeDate;
	pcsSiegeWar->m_ullApplicationTimeDate	= ullApplicationTimeDate;

	return ProclaimSiegeWar(pcsSiegeWar->m_apcsRegionTemplate[0]);
}

BOOL AgsmSiegeWar::SetNextArchlordSiegeWarTime(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	// 공성 시작은 Archlord 모듈쪽에서 한다.
	// 여긴 고로,, 현재 시간부터 대략 한 두달정도? 후에 공성이 시작한다고 잡아버린다.
	// 실제론 한달 간격으로 시작되니 상관없다.

	UINT64	ullNextSiegeWarTimeDate	= AuTimeStamp2::GetCurrentTimeStamp() + 60 * 24 * 60 * 60 * (UINT64) 10000000;

	pcsSiegeWar->m_ullNextSiegeWarTimeDate	= ullNextSiegeWarTimeDate;
	pcsSiegeWar->m_ullApplicationTimeDate	= ullNextSiegeWarTimeDate;

	return ProclaimSiegeWar(pcsSiegeWar->m_apcsRegionTemplate[0]);
}

BOOL AgsmSiegeWar::SetNotOwnerGuildTargetPosition(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos)
{
	if (!pcsCharacter || !pstTargetPos)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= m_pcsAgpmSiegeWar->GetSiegeWarInfo(m_pcsAgpmSiegeWar->GetSiegeWarInfoIndex(*pstTargetPos));
	if (!pcsSiegeWar)
		return FALSE;

	if (m_pcsAgpmSiegeWar->IsOwnerGuild(pcsCharacter, pcsSiegeWar))
		return FALSE;

	AuPOS	stNewPosition;
	
	if (m_pcsAgpmSiegeWar->IsArchlordCastle(pcsSiegeWar))		// 결정전은 무조건 최종 마을로 보낸다. 원래 올 수 없으니까..
	{
		if (!m_pcsAgpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &stNewPosition))
			return FALSE;
	}
	else
	{
		if (!(m_pcsAgpmEventBinding->GetBindingPositionForSiegeWarResurrection(pcsCharacter, &stNewPosition, pcsSiegeWar->m_apcsRegionTemplate[0]->pStrName, TRUE, FALSE) ||
			  m_pcsAgpmEventBinding->GetBindingPositionForSiegeWarResurrection(pcsCharacter, &stNewPosition, pcsSiegeWar->m_apcsRegionTemplate[1]->pStrName, TRUE, FALSE)))
			return FALSE;
	}
	
	*pstTargetPos	= stNewPosition;

	return TRUE;
}

BOOL AgsmSiegeWar::MoveAttackGuildMember(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("MoveAttackGuildMember"));

	INT32	lGuildIndex	= 0;
	for (AgpdGuild **ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lGuildIndex); ppcsGuild && *ppcsGuild; ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lGuildIndex))
	{
		if ((*ppcsGuild)->m_Mutex.WLock())
		{
			AgpdGuild	*pcsGuild	= *ppcsGuild;
			INT32 lIndex = 0;
			for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**) pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
															ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
			{
				if(!ppcsMember || !*ppcsMember)
					break;

				AgpdCharacter	*pcsMember	= m_pcsAgpmCharacter->GetCharacterLock((*ppcsMember)->m_szID);
				if (pcsMember)
				{
					AuPOS	stNewTargetPos	= pcsMember->m_stPos;

					if (m_pcsAgpmSiegeWar->SetAttackGuildTargetPosition(pcsMember, &stNewTargetPos))
					{
						m_pcsAgpmCharacter->StopCharacter(pcsMember, NULL);
						m_pcsAgpmCharacter->UpdatePosition(pcsMember, &stNewTargetPos, FALSE, TRUE);
					}

					pcsMember->m_Mutex.Release();
				}
			}

			(*ppcsGuild)->m_Mutex.Release();
		}
	}

	return TRUE;
}

BOOL AgsmSiegeWar::MoveDefenseGuildMember(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("MoveDefenseGuildMember"));

	INT32	lGuildIndex	= 0;
	for (AgpdGuild **ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csDefenseGuild.GetObjectSequence(&lGuildIndex); ppcsGuild && *ppcsGuild; ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csDefenseGuild.GetObjectSequence(&lGuildIndex))
	{
		if ((*ppcsGuild)->m_Mutex.WLock())
		{
			AgpdGuild	*pcsGuild	= *ppcsGuild;
			INT32 lIndex = 0;
			for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**) pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
															ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
			{
				if(!ppcsMember || !*ppcsMember)
					break;

				AgpdCharacter	*pcsMember	= m_pcsAgpmCharacter->GetCharacterLock((*ppcsMember)->m_szID);
				if (pcsMember)
				{
					AuPOS	stNewTargetPos	= pcsMember->m_stPos;

					if (m_pcsAgpmSiegeWar->SetDefenseGuildTargetPosition(pcsMember, &stNewTargetPos))
					{
						m_pcsAgpmCharacter->StopCharacter(pcsMember, NULL);
						m_pcsAgpmCharacter->UpdatePosition(pcsMember, &stNewTargetPos, FALSE, TRUE);
					}

					pcsMember->m_Mutex.Release();
				}
			}

			(*ppcsGuild)->m_Mutex.Release();
		}
	}

	return TRUE;
}

const INT32 ARCHLORD_CANCEL_INTERVAL			= 1000 * 60 * 5;		// 5 minutes

BOOL AgsmSiegeWar::CheckExistArchlord(UINT32 ulClockCount)
{
	// 아크로드가 없으면 패스.
	if (m_pcsAgpmArchlord->m_szArchlord.IsEmpty())
		return TRUE;
	
	// 처음 타는거면 패스.
	if (0 == m_ulLatestConnOfArchlord)
	{
		m_ulLatestConnOfArchlord = ulClockCount;
		return TRUE;
	}
	
	// 접속중이면 패스.
	AgpdCharacter	*pcsArchlord	= m_pcsAgpmSiegeWar->GetArchlord();	
	if (pcsArchlord)
	{
		m_ulLatestConnOfArchlord = ulClockCount;
		return TRUE;
	}
	
	// 시간 안지났으면 패스.
	if (ARCHLORD_CANCEL_INTERVAL >= ulClockCount - m_ulLatestConnOfArchlord)
		return TRUE;
	
	// 넌 끝이야...
	m_ulLatestConnOfArchlord = 0;		// 초기화
	return FALSE;
}

BOOL AgsmSiegeWar::DepriveArchlord(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return FALSE;
	
	AgpdGuild *pcsGuild = m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strOwnerGuildName.GetBuffer());
	if (pcsGuild)
	{
		pcsGuild->m_Mutex.WLock();
		AddTimer(5 * 1000, 0, this, TeleportGuildMember, pcsGuild);
		pcsGuild->m_Mutex.Release();
	}
	m_pcsAgsmArchlord->CancelArchlord(m_pcsAgpmArchlord->m_szArchlord.GetBuffer());
	m_pcsAgpmSiegeWar->ClearDefenseGuild(pcsSiegeWar);
	DisableDefenseResurrectionTower(pcsSiegeWar);
	
	return TRUE;
}
