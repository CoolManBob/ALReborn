#include "AgsmArchlord.h"

#include "AgsmGuild.h"
#include "AgpmItem.h"
#include "AgsmItemManager.h"
#include "AgpmFactors.h"
#include "AgsmAdmin.h"
#include "AgsmCharacter.h"
#include "AgpmEventSpawn.h"
#include "ApmEventManager.h"
#include "AgsmEventSpawn.h"
#include "AgpmEventTeleport.h"
#include "AgpmEventBinding.h"

const INT32 ARCHLORD_TID = 1276;

AgsmArchlord::AgsmArchlord()
{
	m_pcsAgpmArchlord = NULL;
	m_pcsAgsmGuild = NULL;
	m_pcsAgpmItem = NULL;
	m_pcsAgpmGrid = NULL;
	m_pcsAgsmItemManager = NULL;
	m_pcsAgpmFactors = NULL;
	m_pcsAgsmAdmin = NULL;
	m_pcsAgpmCharacter = NULL;
	m_pcsAgsmCharacter = NULL;
	m_pcsAgpmEventSpawn = NULL;
	m_pcsApmEventManager = NULL;
	m_pcsAgsmEventSpawn = NULL;
	m_pcsAgpmEventTeleport = NULL;
	m_pcsAgpmSiegeWar = NULL;
	m_pcsAgsmRide = NULL;
	m_pcsAgsmDeath = NULL;
	m_pcsAgpmEventBinding = NULL;

	m_lLastTick = 0;
	m_lStepTime = 0;

	EnableIdle(TRUE);
	SetModuleName("AgsmArchlord");

	InitGuardItem();
}

AgsmArchlord::~AgsmArchlord()
{
}

void AgsmArchlord::InitGuardItem()
{
	::ZeroMemory(&m_GuardItem, sizeof(m_GuardItem));

	/*
		휴먼궁수
		[3941]H_Arc_M_Set_9thBody
		[3945]H_Arc_M_Set_9thFoot
		[3943]H_Arc_M_Set_9thHands
		[3946]H_Arc_M_Set_head_9thHead
		[3944]H_Arc_M_Set_9thLegs 
	*/
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_BODY] = 3941;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_FOOT] = 3945;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HAND] = 3943;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HEAD] = 3946;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_LEGS] = 3944;

	/*
		휴먼전사
		[3975]H_Kni_M_Set_9thBody
		[3979]H_Kni_M_Set_9thFoot
		[3977]H_Kni_M_Set_9thHands
		[3980]H_Kni_M_Set_head_9thHead
		[3978]H_Kni_M_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_BODY] = 3975;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_FOOT] = 3979;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HAND] = 3977;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HEAD] = 3980;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_LEGS] = 3978;

	/*
		휴먼법사
		[4093]H_wiz_W_Set_9thBody
		[4097]H_wiz_W_Set_9thFoot
		[4095]H_wiz_W_Set_9thHands
		[4098]H_wiz_W_Set_head_9thHead
		[4096]H_wiz_W_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_BODY] = 4093;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_FOOT] = 4097;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HAND] = 4095;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HEAD] = 4098;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_LEGS] = 4096;

	/*
		문엘프전사
		[5751]H_Kni_M_Set_9thBody
		[5754]H_Kni_M_Set_9thFoot
		[5752]H_Kni_M_Set_9thHands
		[5758]H_Kni_M_Set_head_9thHead
		[5753]H_Kni_M_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_BODY] = 5751;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_FOOT] = 5754;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HAND] = 5752;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HEAD] = 5758;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_LEGS] = 5753;

	/*
		문엘프궁수
		[4423]M_Hun_W_Set_9thBody
		[4427]M_Hun_W_Set_9thFoot 
		[4425]M_Hun_W_Set_9thHands
		[4432]M_Hun_W_Set_head_9thHead
		[4426]M_Hun_W_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_BODY] = 4423;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_FOOT] = 4427;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HAND] = 4425;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HEAD] = 4432;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_LEGS] = 4426;

	/*
		문엘프법사
		[4453]M_Sha_W_Set_9thBody
		[4457]M_Sha_W_Set_9thFoot
		[4455]M_Sha_W_Set_9thHands
		[4552]M_Sha_W_Set_head_9thHead
		[4456]M_Sha_W_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_BODY] = 4453;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_FOOT] = 4457;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HAND] = 4455;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HEAD] = 4552;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_LEGS] = 4456;

	/*
		오크궁수
		[4482]O_Hun_W_Set_9thBody
		[4486]O_Hun_W_Set_9thFoot 
		[4484]O_Hun_W_Set_9thHands
		[4491]O_Hun_W_Set_head_9thHead
		[4485]O_Hun_W_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_BODY] = 4482;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_FOOT] = 4486;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HAND] = 4484;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HEAD] = 4491;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_LEGS] = 4485;

	/*
		오크법사
		[4512]O_Sha_M_Set_9thBody
		[4516]O_Sha_M_Set_9thFoot 
		[4514]O_Sha_M_Set_9thHands
		[4521]O_Sha_M_Set_Head_9thHead
		[4515]O_Sha_M_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_BODY] = 4512;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_FOOT] = 4516;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HAND] = 4514;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HEAD] = 4521;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_LEGS] = 4515;

	/*
		오크전사
		[4542]O_War_M_Set_9thBody
		[4546]O_War_M_Set_9thFoot
		[4544]O_War_M_Set_9thHands
		[4551]O_War_M_Set_Head_9thHead
		[4545]O_War_M_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_BODY] = 4542;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_FOOT] = 4546;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HAND] = 4544;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HEAD] = 4551;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_LEGS] = 4545;


	// DragonScion
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_BODY] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_FOOT] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HAND] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HEAD] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_LEGS] = 1;

	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_BODY] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_FOOT] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HAND] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HEAD] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_LEGS] = 1;

	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_SCION].ItemTID[EGIP_BODY] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_SCION].ItemTID[EGIP_FOOT] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_SCION].ItemTID[EGIP_HAND] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_SCION].ItemTID[EGIP_HEAD] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_SCION].ItemTID[EGIP_LEGS] = 1;

	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_BODY] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_FOOT] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HAND] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HEAD] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_LEGS] = 1;
}

BOOL AgsmArchlord::OnAddModule()
{
	m_pcsAgpmArchlord = (AgpmArchlord*)GetModule("AgpmArchlord");
	m_pcsAgsmGuild = (AgsmGuild*)GetModule("AgsmGuild");
	m_pcsAgpmItem = (AgpmItem*)GetModule("AgpmItem");
	m_pcsAgpmGrid = (AgpmGrid*)GetModule("AgpmGrid");
	m_pcsAgsmItemManager = (AgsmItemManager*)GetModule("AgsmItemManager");
	m_pcsAgpmFactors = (AgpmFactors*)GetModule("AgpmFactors");
	m_pcsAgsmAdmin = (AgsmAdmin*)GetModule("AgsmAdmin");
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pcsAgsmCharManager = (AgsmCharManager*)GetModule("AgsmCharManager");
	m_pcsAgpmEventSpawn = (AgpmEventSpawn*)GetModule("AgpmEventSpawn");
	m_pcsApmEventManager = (ApmEventManager*)GetModule("ApmEventManager");
	m_pcsAgsmEventSpawn = (AgsmEventSpawn*)GetModule("AgsmEventSpawn");
	m_pcsAgpmEventTeleport = (AgpmEventTeleport*)GetModule("AgpmEventTeleport");
	m_pcsAgpmSiegeWar = (AgpmSiegeWar*)GetModule("AgpmSiegeWar");
	m_pcsAgsmRide = (AgsmRide*)GetModule("AgsmRide");
	m_pcsAgsmDeath = (AgsmDeath*)GetModule("AgsmDeath");
	m_pcsAgpmEventBinding = (AgpmEventBinding*)GetModule("AgpmEventBinding");
	
	if (!m_pcsAgpmArchlord || !m_pcsAgsmGuild || !m_pcsAgpmItem || !m_pcsAgpmGrid || 
		!m_pcsAgsmItemManager || !m_pcsAgpmFactors || !m_pcsAgsmAdmin || !m_pcsAgpmCharacter ||
		!m_pcsAgsmCharacter || !m_pcsAgsmCharManager || !m_pcsAgpmEventSpawn || !m_pcsApmEventManager ||
		!m_pcsAgsmEventSpawn || !m_pcsAgpmEventTeleport || !m_pcsAgpmSiegeWar || !m_pcsAgsmRide ||
		!m_pcsAgsmDeath || !m_pcsAgpmEventBinding)
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordSetArchlord(CBSetArchlord, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordSetGuard(CBSetGuard, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordCancelGuard(CBCancelGuard, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordGuardInfo(CBGuardInfo, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordCancelArchlord(CBCancelArchlord, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordStart(CBArchlordStart, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordNextStep(CBArchlordNextStep, this))
		return FALSE;

	if (!m_pcsAgsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordEnd(CBArchlordEnd, this))
		return FALSE;

	if (!m_pcsAgpmEventTeleport->SetCallbackArchlordMessageId(CBTeleportMessageId, this))
		return FALSE;

	if (!m_pcsAgpmArchlord->SetCallbackEventRequest(CBEventRequest, this))
		return FALSE;

	if (!m_pcsAgsmDeath->SetCallbackCharacterDead(CBCharacterDead, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmArchlord::OnInit()
{
	return TRUE;
}

BOOL AgsmArchlord::OnDestroy()
{
	return TRUE;
}

BOOL AgsmArchlord::OnIdle(UINT32 ulClockCount)
{
	// 30초 마다 확인
	if (ulClockCount < (m_lLastTick + (30* 1000)))
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("OnIdle"));

	m_lLastTick = ulClockCount;
	

	switch (m_pcsAgpmArchlord->GetCurrentStep())
	{
	case AGPMARCHLORD_STEP_NONE:		IdleStepNone(ulClockCount);		break;
	case AGPMARCHLORD_STEP_DUNGEON:		IdleStepDungeon(ulClockCount);	break;
	case AGPMARCHLORD_STEP_SIEGEWAR:	IdleStepSiegewar(ulClockCount);	break;
	case AGPMARCHLORD_STEP_ARCHLORD:	IdleStepArchlord(ulClockCount);	break;
	default:	ASSERT(!"AgsmArchlord::OnIdle() error");
	};

	return TRUE;
}

void AgsmArchlord::StartArchlord(UINT32 ulClockCorrection)
{
// 아크로드 시스템 시작

	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_DUNGEON);
	SetStepTime(GetClockCount() - ulClockCorrection);

	SendPacketCurrentStep();

	// 비밀의 던젼이 성주 전용 던젼이 되면서 아크로드 결정전에서 빠짐. 2009. 09. 29 박경도
	//SpawnDungeonMonster();
	//m_pcsAgpmEventTeleport->ActiveTeleportCastleToDungeon();
	//아크로드 결정전이 시작되면 비밀의 던젼에서 모든 유저를 내쫓는다.
	ExpelAllUsersFromSecretDungeon();
}

void AgsmArchlord::NextStep()
{
	SetStepTime(m_lStepTime - (60 * 60 * 1000));
}

void AgsmArchlord::ArchlordEnd()
{
	SetStepTime(0);

	m_pcsAgpmSiegeWar->SetCurrentStatus(AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX, AGPD_SIEGE_WAR_STATUS_TIME_OVER_ARCHLORD_BATTLE);

	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_END);
	SendPacketCurrentStep();
	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_NONE);
}

void AgsmArchlord::IdleStepNone(UINT32 ulClockCount)
{
	// 날짜 확인
	if (FALSE == IsArchlordStartDay())
		return;

	// 시간 확인
	SYSTEMTIME localTime;
	::GetLocalTime(&localTime);

	// 오후 8시 30분에 시작 -> 아크로드 결정전 리뉴얼되면서 오후 9시로 변경 supertj@20100204
	if ( 21 == localTime.wHour)
	{
		UINT32 ulCorrection = 0;
		// 5분 이상 지난경우 남은 시간 보정.
		if (localTime.wMinute >= 35)
			ulCorrection = (UINT32) (60 * 1000 * (localTime.wMinute - (30 + 3))); 		// 약 3분의 추가시간
	
		StartArchlord(ulCorrection);
	}
}

void AgsmArchlord::IdleStepDungeon(UINT32 ulClockCount)
{
	// 30분이 지나면 다음 단계 --> 10분으로 수정 2009. 09. 29 박경도
	if (ulClockCount < m_lStepTime + AGSMARCHLORD_STEP_DUNGEON_TIME)
		return;

	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_SIEGEWAR);
	SetStepTime(ulClockCount);
	SendPacketCurrentStep();

	// 비밀의 던젼이 성주 전용 던젼이 되면서 아크로드 결정전에서 빠짐. 2009. 09. 29 박경도
	//// 던젼 몬스터 삭제
	//RemoveDungeonMonster();
	//// 던젼 텔레포트를 정지시킨다.
	//m_pcsAgpmEventTeleport->DungeonTeleportDisable();

	//// 던젼에 남아 있는 모든 유저들을 쫓아 낸다.
	//ExpelAllUsersFromSecretDungeon();

	if (m_pcsAgpmSiegeWar->CheckStartCondition())
		EnumCallback(AGSMARCHLORD_CB_START_ARCHLORD_SIEGEWAR, NULL, NULL);
	else
	{	
		EnumCallback(AGSMARCHLORD_CB_END_ARCHLORD_SIEGEWAR, NULL, NULL);
		//아크로드 공성을 시작할 수 없다.
		ArchlordEnd();
	}
}

void AgsmArchlord::IdleStepSiegewar(UINT32 ulClockCount)
{
	// 1시간이 지나면 다음 단계
	if (ulClockCount < m_lStepTime + AGSMARCHLORD_STEP_SIEGEWAR_TIME)
		return;

	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_ARCHLORD);
	SetStepTime(ulClockCount);
	SendPacketCurrentStep();
}

void AgsmArchlord::IdleStepArchlord(UINT32 ulClockCount)
{
	// 1시간이 지나면 다음 단계
	if (ulClockCount < m_lStepTime + AGSMARCHLORD_STEP_ARCHLORD_TIME)
		return;

	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_NONE);
	SetStepTime(ulClockCount);
	SendPacketCurrentStep();
}

BOOL AgsmArchlord::IsArchlordStartDay()
{
	// 현재 시간 구하기
	SYSTEMTIME localTime;
	::GetLocalTime(&localTime);

	// 토요일(6)에만 아크로드 시스템이 진행된다.
	if (6 != localTime.wDayOfWeek)
		return FALSE;

	// 아크로드결정전은 마지막주가 아닌 4번째 주에 진행이 된다.
	// 4번째 주 토요일은 어떤달이건 22일부터 28일 사이에 위치하게 된다.
	if (localTime.wDay >= 22 && localTime.wDay <= 28)
		return TRUE;

	return FALSE;
}

BOOL AgsmArchlord::CBSetArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsTarget = (AgpdCharacter*)pData;

	return pThis->SetArachlord(pcsTarget);
}

BOOL AgsmArchlord::CBCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsTarget = (AgpdCharacter*)pData;

	return pThis->CancelArchlord(pcsTarget->m_szID);
}

BOOL AgsmArchlord::CBSetGuard(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsArchlord = (AgpdCharacter*)pData;
	AgpdCharacter *pcsTarget = (AgpdCharacter*)pCustData;

	return pThis->SetGuard(pcsArchlord, pcsTarget);
}

BOOL AgsmArchlord::CBCancelGuard(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsArchlord = (AgpdCharacter*)pData;
	AgpdCharacter *pcsTarget = (AgpdCharacter*)pCustData;

	return pThis->CancelGuard(pcsArchlord, pcsTarget);
}

BOOL AgsmArchlord::CBGuardInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsArchlord = (AgpdCharacter*)pData;

	return pThis->GuardInfo(pcsArchlord);
}

BOOL AgsmArchlord::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmArchlord* pThis = (AgsmArchlord*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBEnterGameWorld"));

	if (FALSE == pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	if (pThis->m_pcsAgpmCharacter->IsNPC(pcsCharacter))
		return TRUE;

	if (pThis->m_pcsAgpmCharacter->IsGuard(pcsCharacter))
		return TRUE;

	// 모든 접속자에게 아크로드의 ID를 전송해준다.
	pThis->SendPacketArchlordID(pcsCharacter);

	// 아크로드인지 확인후 작업 진행
	if (FALSE == pThis->TransformArchlord(pcsCharacter))
		// 아크로드가 아니라면 호위대 인지 확인 후 작업 진행
		pThis->CheckGuard(pcsCharacter);

	return TRUE;
}

BOOL AgsmArchlord::CBArchlordStart(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBArchlordStart"));

	pThis->StartArchlord();
	return TRUE;
}

BOOL AgsmArchlord::CBArchlordNextStep(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBArchlordNextStep"));

	pThis->NextStep();

	return TRUE;
}

BOOL AgsmArchlord::CBArchlordEnd(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBArchlordEnd"));

	pThis->ArchlordEnd();

	return TRUE;
}

BOOL AgsmArchlord::CBTeleportMessageId(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;

	pThis->SendPacketMessageId(pcsCharacter, AGPMARCHLORD_MESSAGE_NEED_ARCHON_ITEM);

	return TRUE;
}

BOOL AgsmArchlord::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pData;
	ApdEvent *pApdEvent = (ApdEvent *) pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmArchlord->MakePacketEventGrant(&nPacketLength, pApdEvent, pcsCharacter->m_lID);

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	pThis->m_pcsAgpmArchlord->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmArchlord::TransformArchlord(AgpdCharacter *pcsCharacter)
{
	// 아크로드이면 변신 시켜준다.
	if (FALSE == m_pcsAgpmArchlord->IsArchlord(pcsCharacter->m_szID))
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("TransformArchlord"));

	AuAutoLock Guard(pcsCharacter->m_Mutex);
	if (!Guard.Result()) return FALSE;

	// 아크로드로 변신
	AgpdCharacterTemplate *pcsTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(ARCHLORD_TID);
	ASSERT(pcsTemplate);
	if (!pcsTemplate) 
		return FALSE;

	// 2006.11.23. steeple
	// 탈것을 타 있으면 내려준다.
	if(pcsCharacter->m_bRidable)
	{
		m_pcsAgsmRide->DismountReq(pcsCharacter, 1);
		//AgpdGridItem* pcsAgpdGridItem = m_pcsAgpmItem->GetEquipItem(pcsCharacter, AGPMITEM_PART_RIDE);
		//if(pcsAgpdGridItem)
		//{
		//	AgpdItem* pcsItem = m_pcsAgpmItem->GetItem(pcsAgpdGridItem);
		//	if(pcsItem)
		//	{
		//		m_pcsAgpmItem->UnEquipItem(pcsCharacter, pcsItem);
		//		//m_pcsAgsmRide->DismountReq(pcsCharacter, pcsItem->m_lID);
		//	}
		//}
	}

	INT64	lMaxExp		= m_pcsAgpmFactors->GetMaxExp(&pcsCharacter->m_csFactor);
	INT64	lCurrentExp	= m_pcsAgpmCharacter->GetExp(pcsCharacter);

	// 2006.12.14. steeple
	// 겉모양 만 바뀌게끔 변경한다.
	// 하지만 추가적으로 더해줘야 하는 팩터가 있는 지 체크해야 한다.
	//m_pcsAgpmCharacter->TransformCharacter(pcsCharacter, AGPMCHAR_TRANSFORM_TYPE_APPEAR_STATUS_ALL, pcsTemplate, &pcsTemplate->m_csEffectFactor);
	m_pcsAgpmCharacter->TransformCharacter(pcsCharacter, AGPMCHAR_TRANSFORM_TYPE_APPEAR_ONLY, pcsTemplate);

	//// 2006.11.07. steeple
	//// 팩터를 복사한다.
	//m_pcsAgpmCharacter->TransformCharacter(pcsCharacter, AGPMCHAR_TRANSFORM_TYPE_APPEAR_STATUS_ALL, pcsTemplate, &pcsTemplate->m_csFactor, TRUE);

	m_pcsAgpmFactors->SetMaxExp(&pcsCharacter->m_csFactor, lMaxExp);
	//m_pcsAgpmFactors->SetMaxExp((AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT), lMaxExp);

	m_pcsAgpmFactors->SetExp(&pcsCharacter->m_csFactor, lCurrentExp);
	//m_pcsAgpmFactors->SetExp((AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT), lCurrentExp);

	// 에너지를 풀로 채워준다.
	m_pcsAgsmCharacter->SetHPFull(pcsCharacter);

	m_pcsAgsmCharacter->ReCalcCharacterResultFactors(pcsCharacter);

	return TRUE;
}

BOOL AgsmArchlord::CheckGuard(AgpdCharacter *pcsCharacter)
{
	// 접속할 때 호위대가 아닌데 호위대 아이템을 가지고 있으면 강제로 삭제한다.

	ASSERT(pcsCharacter);
	if (NULL == pcsCharacter)
		return FALSE;

	AuAutoLock Guard(m_lock);
	if (!Guard.Result()) return FALSE;

	AuAutoLock Guard2(pcsCharacter->m_Mutex);
	if (!Guard2.Result()) return FALSE;

	vtIterator iter = ::find(m_vtGuard.begin(), m_vtGuard.end(), string(pcsCharacter->m_szID));
	if (m_vtGuard.end() == iter)
	{
		DeleteGuardItem(pcsCharacter);
	}

	return TRUE;
}

BOOL AgsmArchlord::SetGuard(AgpdCharacter *pcsArchlord, AgpdCharacter *pcsTarget)
{
	//STOPWATCH2(GetModuleName(), _T("SetGuard"));

	AuAutoLock Guard(m_lock);
	if (!Guard.Result()) return FALSE;

	// 명령을 내린 사람이 아크로드 인지 확인
	if (FALSE == m_pcsAgpmArchlord->IsArchlord(pcsArchlord->m_szID))
		return FALSE;

	// 같은 길드인지 확인
	if (FALSE == m_pcsAgsmGuild->IsSameGuild(pcsArchlord, pcsTarget))
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_SET_GUARD);
		return FALSE;
	}

	// 설정된 호위대의 최대 숫자를 초과 하는지 확인
	if (GUARD_MAX < m_vtGuard.size())
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_SET_GUARD);
		return FALSE;
	}

	// 이미 호위대로 설정 되어 있는 사람인지 확인
	vtIterator iter = ::find(m_vtGuard.begin(), m_vtGuard.end(), string(pcsTarget->m_szID));
	if (m_vtGuard.end() != iter)
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_SET_GUARD);
		return FALSE;
	}

	// 호위대 아이템 생성
	if (FALSE == CreateGuardItem(pcsTarget))
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_SET_GUARD);
		return FALSE;
	}

	// 호위대 설정
	m_vtGuard.push_back(string(pcsTarget->m_szID));
	
	// DB
	EnumCallback(AGSMARCHLORD_CB_DBINSERT_GUARD, pcsTarget, NULL);

	return TRUE;
}

BOOL AgsmArchlord::CancelGuard(AgpdCharacter *pcsArchlord, AgpdCharacter *pcsTarget)
{
	//STOPWATCH2(GetModuleName(), _T("CancelGuard"));

	ASSERT(pcsArchlord);
	ASSERT(pcsTarget);

	AuAutoLock Guard(m_lock);
	if (!Guard.Result()) return FALSE;

	// 명령을 내린 사람이 아크로드 인지 확인
	if (FALSE == m_pcsAgpmArchlord->IsArchlord(pcsArchlord->m_szID))
		return FALSE;

	// 같은 길드인지 확인
	if (FALSE == m_pcsAgsmGuild->IsSameGuild(pcsArchlord, pcsTarget))
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_CANCEL_GUARD);
		return FALSE;
	}

	// 호위대가 맞는지 확인
	vtIterator iter = ::find(m_vtGuard.begin(), m_vtGuard.end(), string(pcsTarget->m_szID));

	if (iter == m_vtGuard.end())
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_CANCEL_GUARD);
		return FALSE;
	}

	// 호위대 아이템 삭제
	DeleteGuardItem(pcsTarget);

	// 호위대 해제
	m_vtGuard.erase(iter);

	// DB
	EnumCallback(AGSMARCHLORD_CB_DBDELETE_GUARD, pcsTarget, NULL);

	return TRUE;
}

// 아크로드 설정
BOOL AgsmArchlord::SetArachlord(AgpdCharacter *pcsCharacter)
{
	//STOPWATCH2(GetModuleName(), _T("SetArchlord"));

	ASSERT(pcsCharacter);
	AuAutoLock Guard(m_lock);
	if (!Guard.Result()) return FALSE;

	// 아크로드가 그대로이면 호위대의 아이템을 그대로 유지한다.
	if (COMPARE_EQUAL != m_pcsAgpmArchlord->m_szArchlord.Compare(pcsCharacter->m_szID))
	{
		// 다르면 기존의 아크로드가 가지고 있던 혜택들을 삭제한다.
		AgpdCharacter *pcsGuard = NULL;
		if (m_vtGuard.size())
		{
			for (vtIterator iter = m_vtGuard.begin(); iter == m_vtGuard.end(); ++iter)
			{
				pcsGuard = m_pcsAgpmCharacter->GetCharacter(const_cast<char*>(iter->c_str()));
				if (NULL == pcsGuard)
					continue;

				DeleteGuardItem(pcsGuard);
			}

			m_vtGuard.clear();
		}
	}

	// 
	if (FALSE == m_pcsAgpmArchlord->m_szArchlord.IsEmpty())
	{
		CancelArchlord(m_pcsAgpmArchlord->m_szArchlord.GetBuffer());
	}

	// 새로운 아크로드를 설정한다.
	m_pcsAgpmArchlord->SetArchlord(pcsCharacter->m_szID);
	
	// DB
	UINT32 ulSiegeID = 0;
	EnumCallback(AGSMARCHLORD_CB_DBINSERT_ARCHLORD, pcsCharacter, &ulSiegeID);

	// 아크로드가 선정된 사실을 모든 플레이어에게 패킷 전송
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketSetArchlord(&nPacketLength, pcsCharacter->m_szID);
	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacketAllUser(pvPacket, nPacketLength);

	TransformArchlord(pcsCharacter);

	return TRUE;
}

BOOL AgsmArchlord::CancelArchlord(CHAR* szArchlord)
{
	//STOPWATCH2(GetModuleName(), _T("CancelArchlord"));

	AuAutoLock Guard(m_lock);
	if (!Guard.Result()) return FALSE;

	// 다르면 기존의 아크로드가 가지고 있던 혜택들을 삭제한다.
	AgpdCharacter *pcsGuard = NULL;
	if (m_vtGuard.size())
	{
		for (vtIterator iter = m_vtGuard.begin(); iter == m_vtGuard.end(); ++iter)
		{
			pcsGuard = m_pcsAgpmCharacter->GetCharacter(const_cast<char*>(iter->c_str()));
			if (NULL == pcsGuard)
				continue;

			DeleteGuardItem(pcsGuard);
		}

		m_vtGuard.clear();
	}

	// 아크로드가 해제된 사실을 모든 플레이어에게 패킷 전송
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketCancelArchlord(&nPacketLength, m_pcsAgpmArchlord->m_szArchlord.GetBuffer());
	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacketAllUser(pvPacket, nPacketLength);

	// DB
	// 만료일까정 알려주려고 했는데 일단 안해...
	//UINT32 ulSiegeID = 0;
	//EnumCallback(AGSMARCHLORD_CB_DBUPDATE_ARCHLORD, pcsCharacter, &ulSiegeID);

	// 온라인 중이면 아크로드로 변신 취소
	AgpdCharacter *pcsArchlord = m_pcsAgpmCharacter->GetCharacter(m_pcsAgpmArchlord->m_szArchlord.GetBuffer());
	if (NULL != pcsArchlord)
	{
		// 탈 거 타고 있으면 내려주고
		if(pcsArchlord->m_bRidable)
		{
			INT32 lItemID = 1295;		// Arcadia
			m_pcsAgsmRide->DismountReq(pcsArchlord, lItemID);
		}

		m_pcsAgpmCharacter->RestoreTransformCharacter(pcsArchlord);

		// 2006.11.23. steeple
		// 혹시 탈것 타고 있었다면 다시 체크해주기.
		m_pcsAgsmRide->RegionChange(pcsArchlord, pcsArchlord->m_nBindingRegionIndex);
	}

	m_pcsAgpmArchlord->CancelArchlord(szArchlord);

	return TRUE;	
}

UINT32 AgsmArchlord::SetStepTime(UINT32 ulTime)
{
	m_lStepTime = ulTime;

	return m_lStepTime;
}

BOOL AgsmArchlord::GuardInfo(AgpdCharacter *pcsCharacter)
{
	if (FALSE == m_pcsAgpmArchlord->IsArchlord(pcsCharacter->m_szID))
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketGuardInfo(&nPacketLength, (INT32) m_vtGuard.size(), GUARD_MAX);
	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));

	return TRUE;
}

BOOL AgsmArchlord::CreateGuardItem(AgpdCharacter *pcsCharacter)
{
	//STOPWATCH2(GetModuleName(), _T("CreateGuardItem"));

	// 인벤토리에 아이템 추가
	AuAutoLock Guard2(pcsCharacter->m_Mutex);
	if (!Guard2.Result()) return FALSE;

	if (FALSE == IsValidInventory(pcsCharacter))
		return FALSE;

	INT32 Race = m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
	INT32 Class = m_pcsAgpmFactors->GetClass(&pcsCharacter->m_csFactor);

	AgpdItem* pNewItem = NULL;

	for (INT32 i = 0; i < EGIP_MAX; ++i)
	{
		// 아이템 생성
		ASSERT(m_GuardItem[Race][Class].ItemTID[i]);
		pNewItem = m_pcsAgsmItemManager->CreateItem(m_GuardItem[Race][Class].ItemTID[i], pcsCharacter);
		if (!pNewItem) 
			break;

		if (FALSE == m_pcsAgpmItem->AddItemToInventory(pcsCharacter, pNewItem))
		{
			m_pcsAgpmItem->RemoveItem(pNewItem, TRUE);
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL AgsmArchlord::DeleteGuardItem(AgpdCharacter *pcsCharacter)
{
	//STOPWATCH2(GetModuleName(), _T("DeleteGuardItem"));

	INT32 Race = m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
	INT32 Class = m_pcsAgpmFactors->GetClass(&pcsCharacter->m_csFactor);

	AgpdGrid *pcsInventory = m_pcsAgpmItem->GetInventory(pcsCharacter);
	if (NULL == pcsInventory)
		return FALSE;

	AgpdGrid *pcsEquip = m_pcsAgpmItem->GetEquipGrid(pcsCharacter);
	if (NULL == pcsEquip)
		return FALSE;

	AgpdGridItem *pcsGridItem = NULL;

	for (INT32 i = 0; i < EGIP_MAX; ++i)
	{
		// 인벤토리에서 찾기
		ASSERT(m_GuardItem[Race][Class].ItemTID[i]);
		pcsGridItem = m_pcsAgpmGrid->GetItemByTemplate(pcsInventory, AGPDGRID_ITEM_TYPE_ITEM, 
						m_GuardItem[Race][Class].ItemTID[i]);

		// 인벤토리에 없으면 장착중인지 확인
		if (NULL == pcsGridItem)
		{
			pcsGridItem = m_pcsAgpmGrid->GetItemByTemplate(pcsEquip, AGPDGRID_ITEM_TYPE_ITEM, 
							m_GuardItem[Race][Class].ItemTID[i]);
		}

		if (NULL == pcsGridItem)
			continue;

		AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
		if (pcsItem)
		{
			// Equip 이면 UnEquip 부터 해준다. 2007.05.15. steeple
			if (pcsItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
			{
				m_pcsAgpmItem->UnEquipItem(pcsCharacter, pcsItem);
			}

			m_pcsAgpmItem->RemoveItem(pcsItem, TRUE);
		}
	}

	return TRUE;
}

BOOL AgsmArchlord::IsValidInventory(AgpdCharacter *pcsCharacter)
{
	// 호위대의 전용 아이템을 넣어 줄 수 있는 인벤토리 공간이 필요한지 확인
	AgpdGrid *pcsInventory = m_pcsAgpmItem->GetInventory(pcsCharacter);
	if (NULL == pcsInventory)
		return FALSE;

	INT32 ItemCount = m_pcsAgpmGrid->GetItemCount(pcsInventory);

	if (EGIP_MAX > pcsInventory->m_lGridCount - ItemCount )
	{
		SendPacketMessageId(pcsCharacter, AGPMARCHLORD_MESSAGE_NOT_ENOUGH_INVENTORY);
		return FALSE;
	}

	return TRUE;
}

// 비밀의 던젼에서 몬스터 스폰
void AgsmArchlord::SpawnDungeonMonster()
{
	//STOPWATCH2(GetModuleName(), _T("SpawnDungeonMonster"));

	// 2007.03.23. steeple
	// EventSpawn Thread 로 돌린다.
	m_pcsAgsmEventSpawn->InsertAdditionalSpawn(AGSMSPAWN_ADDITIONAL_TYPE_SECRET_DUNGEON,
												this,
												CBSpawnDungeonMonster,
												0,
												(INT32)AGPDSPAWN_ARCHLORD_TYPE_SECRET_DUNGEON);
}

BOOL AgsmArchlord::CBSpawnDungeonMonster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmArchlord* pThis = static_cast<AgsmArchlord*>(pClass);
	AdditionalSpawnData* pstSpawnData = static_cast<AdditionalSpawnData*>(pData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBSpawnDungeonMonster"));

	AgpdSpawnGroup *	pstGroup;
	INT32				lIndex = 0;

	UINT32				ulClockCount	= pThis->GetClockCount();

	for (pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex); pstGroup; pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		for (ApdEvent	*pstEvent = pstGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn *) pstEvent->m_pvData)->m_pstNext)
		{
			if (!pstEvent->m_pvData)
				break;

			if (pstSpawnData->m_lData2 == ((AgpdSpawn *) pstEvent->m_pvData)->m_lSiegeWarType)
			{
				AuMATRIX	*pstDirection	= NULL;;
				AuPOS		stSpawnPos	= *pThis->m_pcsApmEventManager->GetBasePos(pstEvent->m_pcsSource, &pstDirection);

				pThis->m_pcsAgsmEventSpawn->ProcessSpawn(pstEvent, ulClockCount);
			}
		}
	}

	return TRUE;
}

void AgsmArchlord::RemoveDungeonMonster()
{
	//STOPWATCH2(GetModuleName(), _T("RemoveDungeonMonster"));

	// 2007.03.23. steeple
	// EventSpawn Thread 로 돌린다.
	m_pcsAgsmEventSpawn->InsertAdditionalSpawn(AGSMSPAWN_ADDITIONAL_TYPE_SECRET_DUNGEON,
												this,
												CBRemoveDungeonMonster,
												0,
												(INT32)AGPDSPAWN_ARCHLORD_TYPE_SECRET_DUNGEON);
}

BOOL AgsmArchlord::CBRemoveDungeonMonster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmArchlord* pThis = static_cast<AgsmArchlord*>(pClass);
	AdditionalSpawnData* pstSpawnData = static_cast<AdditionalSpawnData*>(pData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveDungeonMonster"));

	AgpdSpawnGroup *	pstGroup;
	INT32				lIndex = 0;

	for (pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex); pstGroup; pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		for (ApdEvent	*pstEvent = pstGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn *) pstEvent->m_pvData)->m_pstNext)
		{
			if (!pstEvent->m_pvData)
				break;

			if (pstSpawnData->m_lData2 == ((AgpdSpawn *) pstEvent->m_pvData)->m_lSiegeWarType)
			{
				pThis->m_pcsAgsmEventSpawn->RemoveSpawnCharacter(pstEvent);
			}
		}
	}

	return TRUE;
}

BOOL AgsmArchlord::CBCharacterDead(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmArchlord* pThis = static_cast<AgsmArchlord*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);

	if(pThis->m_pcsAgpmArchlord->IsArchlord(pcsCharacter->m_szID))
	{
		// 아크로드 해제 시킨다.
		pThis->CancelArchlord(pcsCharacter->m_szID);
	}

	return TRUE;
}

void AgsmArchlord::SendPacketCurrentStep()
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketCurrentStep(&nPacketLength, m_pcsAgpmArchlord->GetCurrentStep());

	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacketAllUser(pvPacket, nPacketLength);
}

void AgsmArchlord::SendPacketMessageId(AgpdCharacter *pcsCharacter, AgpmArchlordMessageId eMessageID)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakepacketMessageID(&nPacketLength, eMessageID);

	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
}

void AgsmArchlord::SendPacketArchlordID(AgpdCharacter *pcsCharacter)
{
	// 아크로드 아이디 전송
	if (m_pcsAgpmArchlord->m_szArchlord.IsEmpty())
		return;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketSetArchlord(&nPacketLength, m_pcsAgpmArchlord->m_szArchlord.GetBuffer());
	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
}

// 역대 아크로드 리스트
BOOL AgsmArchlord::OnSelectResultArchlord(AgpdCharacter *pcsCharacter, stBuddyRowset *pRowset, BOOL bEnd)
{
	if (NULL == pRowset && FALSE == bEnd)
		return FALSE;

	if (bEnd)
		return TRUE;
		
	return TRUE;
}

// 호위대 DB에서 읽어옴
BOOL AgsmArchlord::LoadGuardFromDB()
{
	EnumCallback(AGSMARCHLORD_CB_DBSELECT_GUARD, NULL, NULL);
	
	return TRUE;
}

BOOL AgsmArchlord::OnSelectResultLordGuard(stBuddyRowset *pRowset, BOOL bEnd)
{
	if (NULL == pRowset && FALSE == bEnd)
		return FALSE;

	if (bEnd)
	{
		// 여기서 실제 로딩이 다 끝난것임.
		return TRUE;
	}
	
	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
	{
		CHAR	*pszCharID = pRowset->Get(ul, 0);
		// 읽어왔으니 알아서...
		// 여기서 다 끝난것이 아님. 위를 보삼.
		m_vtGuard.push_back(string(pszCharID));
	}

	return TRUE;
}

BOOL AgsmArchlord::SetCallbackDBSelectArclord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBSELECT_ARCHLORD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackDBUpdateArclord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBUPDATE_ARCHLORD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackDBInsertArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBINSERT_ARCHLORD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackDBSelectLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBSELECT_GUARD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackDBInsertLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBINSERT_GUARD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackDBDeleteLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBDELETE_GUARD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackStartArchlordSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_START_ARCHLORD_SIEGEWAR, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackEndArchlordSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_END_ARCHLORD_SIEGEWAR, pfCallback, pClass);
}
// 비밀 던전 시간이 종료됐을때
// 던전안에 위치한 모든 유저를 이전 마을로 보낸다.
void AgsmArchlord::ExpelAllUsersFromSecretDungeon()
{
	INT32 lIndex = 0;
	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	while (pcsCharacter)
	{
		if (m_pcsAgpmCharacter->IsPC(pcsCharacter) && m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter) != 0)
		{
			if (pcsCharacter->m_Mutex.WLock())
			{
				if (m_pcsAgpmSiegeWar->IsInSecretDungeon(pcsCharacter))
				{
					AuPOS stNewPosition;
					if (!m_pcsAgpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &stNewPosition))
						stNewPosition = pcsCharacter->m_stPos;
					
					m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
					m_pcsAgpmCharacter->UpdatePosition(pcsCharacter, &stNewPosition, 0, TRUE);
				}

				pcsCharacter->m_Mutex.Release();
			}
		}

		pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	}
}
