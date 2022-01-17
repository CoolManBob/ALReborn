#include "AgpmSiegeWar.h"
#include "AgpmGuild.h"
#include "AuTimeStamp.h"

static AgpmSiegeWar		*g_pcsAgpmSiegeWar	= NULL;
static AgpmGuild		*g_pcsAgpmGuild		= NULL;

AgpmSiegeWar::AgpmSiegeWar()
{
	SetModuleName("AgpmSiegeWar");

	m_pcsAgpmCharacter	= NULL;

	m_lIndexADCharacter	= (-1);
	m_lIndexADCharTemplate	= (-1);

	m_Mutex.Init();

	SetPacketType(AGPMSIEGEWAR_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
						AUTYPE_INT8,			1,				// packet operation
						AUTYPE_INT8,			1,				// result
						AUTYPE_PACKET,			1,				// castle information packet
						AUTYPE_INT32,			1,				// operator cid
						AUTYPE_PACKET,			1,				// guild list packet
						AUTYPE_INT32,			1,				// target cid
						AUTYPE_PACKET,			1,				// attack object information packet
						AUTYPE_INT32,			1,				// item id
						AUTYPE_PACKET,			1,				// status information packet
						AUTYPE_UINT32,			1,				// current time stamp : AuTimeStamp::GetCurrentTimeStamp()
						AUTYPE_PACKET,			1,				// sync message
						AUTYPE_END,				0
						);

	m_csPacketCastleInfo.SetFlagLength(sizeof(INT16));
	m_csPacketCastleInfo.SetFieldType(
						AUTYPE_MEMORY_BLOCK,	1,				// 성이름
						AUTYPE_MEMORY_BLOCK,	1,				// 성주이름
						AUTYPE_MEMORY_BLOCK,	1,				// 성주 길드이름
						AUTYPE_UINT8,			1,				// 길드 인원
						AUTYPE_UINT64,			1,				// 마지막 공성전이 벌어진 시간
						AUTYPE_UINT64,			1,				// 다음 공성전 시간
						AUTYPE_UINT8,			1,				// 공성전 상태
						AUTYPE_UINT8,			1,				// 공성 부활 가능 플래그
						AUTYPE_UINT8,			1,				// 수성 부활 가능 플래그
						AUTYPE_UINT8,			1,				// 공성/수성 상태 (입장)
						AUTYPE_MEMORY_BLOCK,	1,				// 각인 길드이름
						AUTYPE_END,				0
						);

	m_csPacketGuildList.SetFlagLength(sizeof(INT8));
	m_csPacketGuildList.SetFieldType(
						AUTYPE_INT16,			1,				// page 번호
						AUTYPE_INT16,			1,				// total page 번호
						AUTYPE_PACKET,			AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE,		// 길드 정보 패킷
						AUTYPE_END,				0
						);

	m_csPacketGuildInfo.SetFlagLength(sizeof(INT8));
	m_csPacketGuildInfo.SetFieldType(
						AUTYPE_MEMORY_BLOCK,	1,				// 길드 이름
						AUTYPE_MEMORY_BLOCK,	1,				// 길마 이름
						AUTYPE_UINT32,			1,				// 길드 포인트
						AUTYPE_END,				0
						);

	m_csPacketAttackObjectInfo.SetFlagLength(sizeof(INT16));
	m_csPacketAttackObjectInfo.SetFieldType(
						AUTYPE_UINT8,			1,				// 수리횟수
						AUTYPE_INT32,			1,				// 수리비용
						AUTYPE_UINT8,			1,				// 수리시간 (분단위)
						AUTYPE_UINT8,			1,				// 수리된 횟수
						AUTYPE_INT32,			1,				// 아이템1
						AUTYPE_INT32,			1,				// 아이템2
						AUTYPE_INT32,			1,				// 아이템3
						AUTYPE_INT32,			1,				// 아이템4
						AUTYPE_INT32,			1,				// 아이템5
						AUTYPE_INT32,			1,				// 아이템6
						AUTYPE_INT32,			1,				// 아이템7
						AUTYPE_INT32,			1,				// 아이템8
						AUTYPE_UINT8,			1,				// 수리 남은 시간 (초단위)
						AUTYPE_MEMORY_BLOCK,	1,				// 사용하고 있는 유저 이름
						AUTYPE_END,				0
						);

	m_csPacketStatusInfo.SetFlagLength(sizeof(INT8));
	m_csPacketStatusInfo.SetFieldType(
						AUTYPE_UINT8,			1,				// information type
						AUTYPE_MEMORY_BLOCK,	1,				// 성이름
						AUTYPE_MEMORY_BLOCK,	1,				// 길드이름
						AUTYPE_MEMORY_BLOCK,	1,				// 길마, 성주 이름
						AUTYPE_UINT8,			1,				// Object type
						AUTYPE_UINT32,			1,				// 시간 (초)
						AUTYPE_UINT8,			1,				// 활성화 갯수
						AUTYPE_END,				0
						);

	m_csPacketSyncMessage.SetFlagLength(sizeof(INT8));
	m_csPacketSyncMessage.SetFieldType(
						AUTYPE_UINT8,			1,				// message type
						AUTYPE_UINT16,			1,				// time (second)
						AUTYPE_END,				0
						);

	ZeroMemory(m_acsSiegeWarObjectInfo, sizeof(m_acsSiegeWarObjectInfo));
	
	m_bIsDayOfSiegeWar = FALSE;
	
	m_pcsAgpmEventBinding = NULL;
}

AgpmSiegeWar::~AgpmSiegeWar()
{
	m_Mutex.Destroy();
}

BOOL AgpmSiegeWar::OnAddModule()
{
	m_pcsApmMap			= (ApmMap *)			GetModule("ApmMap");
	m_pcsAgpmCharacter	= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmGuild		= (AgpmGuild *)			GetModule("AgpmGuild");
	m_pcsAgpmAdmin		= (AgpmAdmin *)			GetModule("AgpmAdmin");
	m_pcsAgpmEventSpawn	= (AgpmEventSpawn *)	GetModule("AgpmEventSpawn");
	m_pcsAgpmGrid		= (AgpmGrid *)			GetModule("AgpmGrid");
	m_pcsAgpmItem		= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmFactors	= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmPvP		= (AgpmPvP *)			GetModule("AgpmPvP");
	m_pcsAgpmEventBinding	= (AgpmEventBinding *) GetModule("AgpmEventBinding");	// 클라엔 없다 널첵 패스

	m_pcsAgpmConfig		= (AgpmConfig *)		GetModule("AgpmConfig");

	g_pcsAgpmSiegeWar	= this;
	g_pcsAgpmGuild		= m_pcsAgpmGuild;

	if (!m_pcsApmMap ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmGuild ||
		!m_pcsAgpmGrid ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmFactors)
		return FALSE;

	m_lIndexADCharacter		= m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdSiegeWarADCharacter), NULL, DesCharAttachData);
	if (m_lIndexADCharacter < 0)
		return FALSE;
	m_lIndexADCharTemplate	= m_pcsAgpmCharacter->AttachCharacterTemplateData(this, sizeof(AgpdSiegeWarADCharTemplate), NULL, NULL);
	if (m_lIndexADCharTemplate < 0)
		return FALSE;

	m_lIndexADGuild	= m_pcsAgpmGuild->AttachGuildData(this, sizeof(AgpdGuildAttachData), NULL, NULL);
	if (m_lIndexADGuild < 0)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackStreamReadImportData(CharacterImportDataReadCB, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackInitChar(CBInitCharacter, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackIsStaticCharacter(CBIsStaticCharacter, this))
		return FALSE;
	if (m_pcsAgpmPvP && !m_pcsAgpmPvP->SetCallbackCheckNPCAttackableTarget(CBCheckNPCAttackableTarget, this))
		return FALSE;
	if (m_pcsAgpmPvP && !m_pcsAgpmPvP->SetCallbackIsAttackableTime(CBIsAttackableTime, this))
		return FALSE;
	if (m_pcsAgpmPvP && !m_pcsAgpmPvP->SetCallbackIsInSiegeWarIngArea(CBIsInSiegeWarIngArea, this))
		return FALSE;
		

	if (!m_pcsAgpmGuild->SetCallbackGuildCheckDestroy(CBCheckDestroy, this))
		return FALSE;
	if (!m_pcsAgpmGuild->SetCallbackGuildDestroyPreProcess(CBDestroyGuild, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmSiegeWar::DesCharAttachData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmSiegeWar	*pThis			= (AgpmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdSiegeWarADCharacter	*pcsAttachData	= pThis->GetAttachCharacterData(pcsCharacter);
	if (pcsAttachData->m_pcsAttackObjectGrid)
	{
		pThis->m_pcsAgpmGrid->Remove(pcsAttachData->m_pcsAttackObjectGrid);

		delete pcsAttachData->m_pcsAttackObjectGrid;

		pcsAttachData->m_pcsAttackObjectGrid	= NULL;
	}

	return TRUE;
}

UINT32 AgpmSiegeWar::GetGuildWarPoint(AgpdGuild *pcsGuild)
{
	if (!pcsGuild)
		return 0;

	return pcsGuild->m_lGuildPoint;
}

UINT32 AgpmSiegeWar::GetGuildDuration(AgpdGuild *pcsGuild)
{
	if (!pcsGuild)
		return 0;

	return AuTimeStamp::GetCurrentTimeStamp() - pcsGuild->m_lCreationDate;
}

UINT32 AgpmSiegeWar::GetGuildArchon(AgpdGuild *pcsGuild)
{
	if (!pcsGuild)
		return 0;

	return pcsGuild->m_ulArchonScrollCount;
}

UINT32 AgpmSiegeWar::GetGuildMasterLevel(AgpdGuild *pcsGuild)
{
	if (!pcsGuild || !g_pcsAgpmGuild)
		return 0;

	AgpdGuildMember	*pcsGuildMaster	= g_pcsAgpmGuild->GetMaster(pcsGuild);
	if (!pcsGuildMaster)
		return 0;

	return pcsGuildMaster->m_lLevel;
}

UINT32 AgpmSiegeWar::GetGuildTotalPoint(AgpdGuild *pcsGuild)
{
	if (!pcsGuild || !g_pcsAgpmSiegeWar)	return 0;

	// 적당한 방법으로 포인트를 구해낸다.
	AgpdGuildAttachData	*pcsAttachData	= g_pcsAgpmSiegeWar->GetAttachGuildData(pcsGuild);
	if (!pcsAttachData)	return 0;

	return pcsAttachData->m_ulGuildTotalPoint;
}

AgpdSiegeWarADCharacter* AgpmSiegeWar::GetAttachCharacterData(AgpdCharacter *pcsCharacter)
{
	if (m_pcsAgpmCharacter)
		return (AgpdSiegeWarADCharacter *) m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexADCharacter, (PVOID) pcsCharacter);

	return NULL;
}

AgpdSiegeWarADCharTemplate* AgpmSiegeWar::GetAttachTemplateData(AgpdCharacterTemplate *pcsCharacterTemplate)
{
	if (m_pcsAgpmCharacter)
		return (AgpdSiegeWarADCharTemplate *) m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexADCharTemplate, (PVOID) pcsCharacterTemplate);

	return NULL;
}

AgpdGuildAttachData* AgpmSiegeWar::GetAttachGuildData(AgpdGuild *pcsGuild)
{
	if (m_pcsAgpmGuild)
		return (AgpdGuildAttachData *) m_pcsAgpmGuild->GetAttachedModuleData(m_lIndexADGuild, (PVOID) pcsGuild);

	return NULL;
}

AgpdSiegeWarMonsterType AgpmSiegeWar::GetSiegeWarMonsterType(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return AGPD_SIEGE_MONSTER_TYPE_NONE;

	return GetSiegeWarMonsterType(pcsCharacter->m_pcsCharacterTemplate);
}

AgpdSiegeWarMonsterType AgpmSiegeWar::GetSiegeWarMonsterType(AgpdCharacterTemplate *pcsCharacterTemplate)
{
	if (!pcsCharacterTemplate)
		return AGPD_SIEGE_MONSTER_TYPE_NONE;

	AgpdSiegeWarADCharTemplate	*pcsAttachTemplateData	= GetAttachTemplateData(pcsCharacterTemplate);
	if (!pcsAttachTemplateData)
		return AGPD_SIEGE_MONSTER_TYPE_NONE;

	if (pcsAttachTemplateData->m_eSiegeWarMonsterType < AGPD_SIEGE_MONSTER_TYPE_NONE ||
		pcsAttachTemplateData->m_eSiegeWarMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return AGPD_SIEGE_MONSTER_TYPE_NONE;

	return pcsAttachTemplateData->m_eSiegeWarMonsterType;
}

BOOL AgpmSiegeWar::SetSiegeWarMonsterType(AgpdCharacterTemplate *pcsCharacterTemplate, AgpdSiegeWarMonsterType eType)
{
	if (!pcsCharacterTemplate ||
		eType < AGPD_SIEGE_MONSTER_TYPE_NONE ||
		eType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return FALSE;

	AgpdSiegeWarADCharTemplate	*pcsAttachTemplateData	= GetAttachTemplateData(pcsCharacterTemplate);
	if (!pcsAttachTemplateData)
		return FALSE;

	pcsAttachTemplateData->m_eSiegeWarMonsterType	= eType;

	return TRUE;
}

BOOL AgpmSiegeWar::SetCurrentStatus(INT32 lSiegeWarInfoIndex, AgpdSiegeWarStatus eStatus, BOOL bCheckCondition)
{
	AgpdSiegeWar	*pcsSiegeWarData	= GetSiegeWarInfo(lSiegeWarInfoIndex);
	if (!pcsSiegeWarData)
		return FALSE;

	switch (eStatus) {
		case AGPD_SIEGE_WAR_STATUS_OFF:
			return SetStatusOff(pcsSiegeWarData, bCheckCondition);
			break;

		case AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR:
			return SetStatusProclaimWar(pcsSiegeWarData, bCheckCondition);
			break;

		case AGPD_SIEGE_WAR_STATUS_READY:
			return SetStatusReady(pcsSiegeWarData, bCheckCondition);
			break;

		case AGPD_SIEGE_WAR_STATUS_START:
			return SetStatusStart(pcsSiegeWarData, bCheckCondition);
			break;

		case AGPD_SIEGE_WAR_STATUS_BREAK_A_SEAL:
			return SetStatusBreakASeal(pcsSiegeWarData, bCheckCondition);
			break;

		case AGPD_SIEGE_WAR_STATUS_OPEN_EYES:
			return SetStatusOpenEyes(pcsSiegeWarData, bCheckCondition);
			break;

		case AGPD_SIEGE_WAR_STATUS_TIME_OVER:
			return SetStatusTimeOver(pcsSiegeWarData, bCheckCondition);
			break;

		case AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE:
			return SetStatusArchlordBattle(pcsSiegeWarData, bCheckCondition);
			break;

		case AGPD_SIEGE_WAR_STATUS_TIME_OVER_ARCHLORD_BATTLE:
			return SetStatusTimeOverArchlordBattle(pcsSiegeWarData, bCheckCondition);
			break;

		default:
			return FALSE;
			break;
	}

	return FALSE;
}

BOOL AgpmSiegeWar::SetStatusOff(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	pcsSiegeWarData->m_eCurrentStatus	= AGPD_SIEGE_WAR_STATUS_OFF;

	EnumCallback(AGPMSIEGEWAR_CB_UPDATE_STATUS, pcsSiegeWarData, NULL);

	return TRUE;
}

BOOL AgpmSiegeWar::SetStatusProclaimWar(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition)
{
	AuAutoLock	lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	if (bCheckCondition && 
		pcsSiegeWarData->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_OFF)
		return FALSE;

	pcsSiegeWarData->m_eCurrentStatus	= AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR;
	
	ClearAttackGuild(pcsSiegeWarData);
	ClearDefenseGuild(pcsSiegeWarData);
	ClearAttackApplGuild(pcsSiegeWarData);
	ClearDefenseApplGuild(pcsSiegeWarData);

	EnumCallback(AGPMSIEGEWAR_CB_UPDATE_STATUS, pcsSiegeWarData, NULL);

	return TRUE;
}

BOOL AgpmSiegeWar::SetStatusReady(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	if (bCheckCondition &&
		pcsSiegeWarData->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR)
		return FALSE;

	pcsSiegeWarData->m_eCurrentStatus	= AGPD_SIEGE_WAR_STATUS_READY;

	EnumCallback(AGPMSIEGEWAR_CB_UPDATE_STATUS, pcsSiegeWarData, NULL);

	return TRUE;
}

BOOL AgpmSiegeWar::SetStatusStart(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	if (bCheckCondition &&
		pcsSiegeWarData->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_READY)
		return FALSE;

	pcsSiegeWarData->m_eCurrentStatus	= AGPD_SIEGE_WAR_STATUS_START;

	EnumCallback(AGPMSIEGEWAR_CB_UPDATE_STATUS, pcsSiegeWarData, NULL);

	return TRUE;
}

BOOL AgpmSiegeWar::SetStatusBreakASeal(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	if (bCheckCondition &&
		pcsSiegeWarData->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_START)
		return FALSE;

	pcsSiegeWarData->m_eCurrentStatus	= AGPD_SIEGE_WAR_STATUS_BREAK_A_SEAL;

	EnumCallback(AGPMSIEGEWAR_CB_UPDATE_STATUS, pcsSiegeWarData, NULL);

	return TRUE;
}

BOOL AgpmSiegeWar::SetStatusOpenEyes(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	if (bCheckCondition &&
		pcsSiegeWarData->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_BREAK_A_SEAL)
		return FALSE;

	pcsSiegeWarData->m_eCurrentStatus	= AGPD_SIEGE_WAR_STATUS_OPEN_EYES;

	EnumCallback(AGPMSIEGEWAR_CB_UPDATE_STATUS, pcsSiegeWarData, NULL);

	return TRUE;
}

BOOL AgpmSiegeWar::SetStatusTimeOver(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	pcsSiegeWarData->m_eCurrentStatus	= AGPD_SIEGE_WAR_STATUS_TIME_OVER;

	EnumCallback(AGPMSIEGEWAR_CB_UPDATE_STATUS, pcsSiegeWarData, NULL);

	return TRUE;
}

BOOL AgpmSiegeWar::SetStatusArchlordBattle(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	if (bCheckCondition &&
		pcsSiegeWarData->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_TIME_OVER)
		return FALSE;

	pcsSiegeWarData->m_eCurrentStatus	= AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE;

	EnumCallback(AGPMSIEGEWAR_CB_UPDATE_STATUS, pcsSiegeWarData, NULL);

	return TRUE;
}

BOOL AgpmSiegeWar::SetStatusTimeOverArchlordBattle(AgpdSiegeWar *pcsSiegeWarData, BOOL bCheckCondition)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	if (bCheckCondition &&
		pcsSiegeWarData->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE)
		return FALSE;

	pcsSiegeWarData->m_eCurrentStatus	= AGPD_SIEGE_WAR_STATUS_TIME_OVER_ARCHLORD_BATTLE;

	EnumCallback(AGPMSIEGEWAR_CB_UPDATE_STATUS, pcsSiegeWarData, NULL);

	return TRUE;
}

BOOL AgpmSiegeWar::SetCallbackUpdateStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_UPDATE_STATUS, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackUpdateCarveAGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_UPDATE_CARVE_A_GUILD, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackSetNewOwner(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_SET_NEW_OWNER, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackUpdateCastleInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_UPDATE_CASTLE_INFO, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackSetNextSiegeWarTime(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_SET_NEXT_SIEGEWAR_TIME, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackAddDefenseApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_ADD_DEFENSE_APPLICATION, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackRemoveDefenseApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_REMOVE_DEFENSE_APPLICATION, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackAddAttackApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_ADD_ATTACK_APPLICATION, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackRemoveAttackApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_REMOVE_ATTACK_APPLICATION, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackReceiveResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RECEIVE_RESULT, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackRequestAttackApplGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_REQUEST_ATTACK_APPL_GUILD_LIST, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackRequestDefenseApplGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_REQUEST_DEFENSE_APPL_GUILD_LIST, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackReceiveAttackApplGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RECEIVE_ATTACK_APPL_GUILD_LIST, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackReceiveDefenseApplGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RECEIVE_DEFENSE_APPL_GUILD_LIST, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackAddAttackGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_ADD_ATTACK_GUILD, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackAddDefenseGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_ADD_DEFENSE_GUILD, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackRequestAttackGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_REQUEST_ATTACK_GUILD_LIST, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackRequestDefenseGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_REQUEST_DEFENSE_GUILD_LIST, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackReceiveAttackGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RECEIVE_ATTACK_GUILD_LIST, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackReceiveDefenseGuildList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RECEIVE_DEFENSE_GUILD_LIST, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackReceiveOpenAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RECEIVE_OPEN_ATTACK_OBJECT, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackResponseOpenAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RESPONSE_OPEN_ATTACK_OBJECT, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackReceiveUseAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RECEIVE_USE_ATTACK_OBJECT, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackResponseUseAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RESPONSE_USE_ATTACK_OBJECT, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackReceiveRepairAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RECEIVE_REPAIR_ATTACK_OBJECT, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackResponseRepairAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_RESPONSE_REPAIR_ATTACK_OBJECT, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackRemoveItemToAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_REMOVE_ITEM_TO_ATTACK_OBJECT, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackAddItemToAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_ADD_ITEM_TO_ATTACK_OBJECT, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackAddItemResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_ADD_ITEM_RESULT, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackCarveASeal(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_CARVE_A_SEAL, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackStatusInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_STATUS_INFO, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackUpdateReverseAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_UPDATE_REVERSE_ATTACK, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackSyncMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_SYNC_MESSAGE, pfCallback, pClass);
}

BOOL AgpmSiegeWar::SetCallbackAddArchlordCastleAttackGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSIEGEWAR_CB_ADD_ARCHLORD_CASTLE_ATTACK_GUILD, pfCallback, pClass);
}

AgpdSiegeWarType AgpmSiegeWar::GetCurrentSiegeWarType(INT32 lSiegeWarInfoIndex)
{
	AgpdSiegeWar	*pcsSiegeWarData	= GetSiegeWarInfo(lSiegeWarInfoIndex);
	if (!pcsSiegeWarData)
		return AGPD_SIEGE_WAR_TYPE_NONE;

	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return AGPD_SIEGE_WAR_TYPE_NONE;

	return pcsSiegeWarData->m_eSiegeWarType;
}

VOID AgpmSiegeWar::SetCurrentSiegeWarType(INT32 lSiegeWarInfoIndex, AgpdSiegeWarType eSiegeWarType)
{
	AgpdSiegeWar	*pcsSiegeWarData	= GetSiegeWarInfo(lSiegeWarInfoIndex);
	if (!pcsSiegeWarData)
		return;

	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return;

	pcsSiegeWarData->m_eSiegeWarType	= eSiegeWarType;
}

AgpdSiegeWarStatus AgpmSiegeWar::GetCurrentStatus(INT32 lSiegeWarInfoIndex)
{
	AgpdSiegeWar	*pcsSiegeWarData	= GetSiegeWarInfo(lSiegeWarInfoIndex);
	if (!pcsSiegeWarData)
		return AGPD_SIEGE_WAR_STATUS_MAX;

	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return AGPD_SIEGE_WAR_STATUS_MAX;

	return pcsSiegeWarData->m_eCurrentStatus;
}

BOOL AgpmSiegeWar::IsGuildSiegeWarEntry( const CHAR* szGuildID )
{
	AgpdSiegeWar*		pcsSiegeWarData	=	NULL;
	AgpdSiegeWarStatus	eSiegeWarStatus	=	AGPD_SIEGE_WAR_STATUS_MAX;

	for( INT i = 0 ; TRUE ; ++i )
	{
		// i번째 공성전의 상태를 얻어온다
		eSiegeWarStatus	=	GetCurrentStatus( i );

		// 마지막까지 검색했더니 공성전 하지 않고 있는 길드다
		if( eSiegeWarStatus	== AGPD_SIEGE_WAR_STATUS_MAX )	break;

		// 공성전이 진행 중이라면 참여중인 길드 인지 확인한다
		if( eSiegeWarStatus != AGPD_SIEGE_WAR_STATUS_OFF )
		{
			pcsSiegeWarData		=	GetSiegeWarInfo( i );	// 공성전 정보를 얻어온다
			ASSERT( pcsSiegeWarData );
			
			// 길드이름으로 Object를 얻어 왔다면 공성중인 길드다 !!!
			if( pcsSiegeWarData->m_csAttackGuild.GetObject( (CHAR*)szGuildID ) )	return TRUE;

			pcsSiegeWarData		=	NULL;
		}
		
	}

	return FALSE;
}

INT32 AgpmSiegeWar::GetSiegeWarInfoIndex(AuPOS stPos)
{
	return GetSiegeWarInfoIndex(m_pcsApmMap->GetTemplate(m_pcsApmMap->GetRegion(stPos.x, stPos.z)));
}

INT32 AgpmSiegeWar::GetSiegeWarInfoIndex(ApmMap::RegionTemplate *pcsRegionTemplate)
{
	if (!pcsRegionTemplate)
		return (-1);

	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		if (m_csSiegeWarInfo[i].m_apcsRegionTemplate[0] == pcsRegionTemplate ||
			m_csSiegeWarInfo[i].m_apcsRegionTemplate[1] == pcsRegionTemplate)
		{
			return i;
		}
	}

	return (-1);
}

AgpdSiegeWar* AgpmSiegeWar::GetSiegeWarByOwner(CHAR *szOwnerID)
{
	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		if (COMPARE_EQUAL == m_csSiegeWarInfo[i].m_strOwnerGuildMasterName.Compare(szOwnerID))
			return &m_csSiegeWarInfo[i];
	}

	return NULL;
}

AgpdSiegeWar* AgpmSiegeWar::GetSiegeWarInfo(INT32 lSiegeWarInfoIndex)
{
	if (lSiegeWarInfoIndex < 0 ||
		lSiegeWarInfoIndex >= AGPMSIEGEWAR_MAX_CASTLE)
		return NULL;

	return &m_csSiegeWarInfo[lSiegeWarInfoIndex];
}

AgpdSiegeWar* AgpmSiegeWar::GetSiegeWarInfo(CHAR *pszCastleName)
{
	if (!pszCastleName || !pszCastleName[0])
		return NULL;

	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		if (m_csSiegeWarInfo[i].m_strCastleName.Compare(pszCastleName) == COMPARE_EQUAL)
		{
			return &m_csSiegeWarInfo[i];
		}
	}

	return NULL;
}

AgpdSiegeWar* AgpmSiegeWar::GetSiegeWarInfo(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	return GetSiegeWarInfo(GetSiegeWarInfoIndex(pcsCharacter->m_stPos));
}

AgpdSiegeWar* AgpmSiegeWar::GetSiegeWarInfoOfGuild(AgpdGuild *pcsGuild, BOOL *pbOffense)
{
	if (!pcsGuild)
		return NULL;

	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		if (m_csSiegeWarInfo[i].m_csAttackGuild.GetObject(pcsGuild->m_szID))
		{
			if (pbOffense)
				*pbOffense = TRUE;
		
			return &m_csSiegeWarInfo[i];
		}
		else if (m_csSiegeWarInfo[i].m_csDefenseGuild.GetObject(pcsGuild->m_szID))
		{
			if (pbOffense)
				*pbOffense = FALSE;
			
			return &m_csSiegeWarInfo[i];
		}
	}

	return NULL;
}

AgpdSiegeWar* AgpmSiegeWar::GetSiegeWarInfoOfCharacter(AgpdCharacter *pcsCharacter, BOOL *pbOffense)
{
	if (!pcsCharacter)
		return NULL;
	
	return GetSiegeWarInfoOfGuild(m_pcsAgpmGuild->GetGuild(pcsCharacter), pbOffense);
}

AgpdSiegeWar* AgpmSiegeWar::GetSiegeWarInfo(PVOID pvPacketCastleInfo)
{
	if (!pvPacketCastleInfo)
		return NULL;

	CHAR	*pszCastleName			= NULL;
	CHAR	*pszGuildMasterName		= NULL;
	CHAR	*pszOwnerGuildName		= NULL;

	UINT16	unCastleNameLength		= 0;
	UINT16	unGuildMasterNameLength	= 0;
	UINT16	unOwnerGuildNameLength	= 0;

	CHAR	szCastleName[AGPMSIEGEWAR_MAX_CASTLE_NAME + 1];

	ZeroMemory(szCastleName, sizeof(szCastleName));

	UINT8	ucGuildMemberCount		= 0;
	UINT64	ullPrevSiegeWarTimeDate	= 0;
	UINT64	ullNextSiegeWarTimeDate	= 0;
	UINT8	ucSiegeWarStatus		= 0;
	BOOL	bIsActiveAttackResurrection = FALSE;
	BOOL	bIsActiveDefenseResurrection = FALSE;
	BOOL	bIsReverseAttack	= FALSE;

	CHAR	*pszCarveGuildName		= NULL;
	UINT16	unCarveGuildNameLength	= 0;

	m_csPacketCastleInfo.GetField(FALSE, pvPacketCastleInfo, 0,
									&pszCastleName, &unCastleNameLength,
									&pszGuildMasterName, &unGuildMasterNameLength,
									&pszOwnerGuildName, &unOwnerGuildNameLength,
									&ucGuildMemberCount,
									&ullPrevSiegeWarTimeDate,
									&ullNextSiegeWarTimeDate,
									&ucSiegeWarStatus,
									&bIsActiveAttackResurrection,
									&bIsActiveDefenseResurrection,
									&bIsReverseAttack,
									&pszCarveGuildName, &unCastleNameLength);

	if (unCastleNameLength > 0 &&
		unCastleNameLength <= AGPMSIEGEWAR_MAX_CASTLE_NAME)
		CopyMemory(szCastleName, pszCastleName, sizeof(CHAR) * unCastleNameLength);

	return GetSiegeWarInfo(szCastleName);
}

BOOL AgpmSiegeWar::UpdateCarveAGuild(INT32 lSiegeWarInfoIndex, AgpdGuild *pcsGuild)
{
	if (lSiegeWarInfoIndex < 0 ||
		lSiegeWarInfoIndex >= AGPMSIEGEWAR_MAX_CASTLE ||
		!pcsGuild)
		return FALSE;

	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	AgpdGuild	*pcsPrevCarveGuild	= m_pcsAgpmGuild->GetGuild(m_csSiegeWarInfo[lSiegeWarInfoIndex].m_strCarveGuildName.GetBuffer());
	if (pcsPrevCarveGuild == pcsGuild)
		return TRUE;

	m_csSiegeWarInfo[lSiegeWarInfoIndex].m_strCarveGuildName.SetText(pcsGuild->m_szID);

	if (m_csSiegeWarInfo[lSiegeWarInfoIndex].m_csAttackGuild.GetObject(pcsGuild->m_szID))
		UpdateReverseAttack(&m_csSiegeWarInfo[lSiegeWarInfoIndex], TRUE);
	else
		UpdateReverseAttack(&m_csSiegeWarInfo[lSiegeWarInfoIndex], FALSE);

	return EnumCallback(AGPMSIEGEWAR_CB_UPDATE_CARVE_A_GUILD, &lSiegeWarInfoIndex, NULL);
}

BOOL AgpmSiegeWar::SetNewOwner(AgpdSiegeWar *pcsSiegeWar, AgpdGuild *pcsGuild)
{
	if (!pcsSiegeWar)
		return FALSE;

	AgpdGuild	*pcsPrevGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strOwnerGuildName);

	if (pcsGuild)
	{
		pcsSiegeWar->m_strOwnerGuildName.SetText(pcsGuild->m_szID);
		pcsSiegeWar->m_strOwnerGuildMasterName.SetText(pcsGuild->m_szMasterID);
	}
	else
	{
		pcsSiegeWar->m_strOwnerGuildName.SetText("");
		pcsSiegeWar->m_strOwnerGuildMasterName.SetText("");
	}

	return EnumCallback(AGPMSIEGEWAR_CB_SET_NEW_OWNER, pcsSiegeWar, pcsPrevGuild);
}

BOOL AgpmSiegeWar::ReadSiegeInfo(CHAR *pszFileName, BOOL bDecryption)
{
	if (!pszFileName || !pszFileName[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(pszFileName, TRUE, bDecryption))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();
	INT16			nColumn			= csExcelTxtLib.GetColumn();

	if (nRow <= 1)
		return FALSE;

	AuAutoLock	lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		m_csSiegeWarInfo[i].m_lArrayIndex	= i;

		for (int j = 0; j < nColumn; ++j)
		{
			CHAR	*pszColumnName	= csExcelTxtLib.GetData(j, 0);
			if (!pszColumnName || !pszColumnName[0])
				break;

			CHAR	*pszValue	= csExcelTxtLib.GetData(j, i+1);
			if (!pszValue || !pszValue[0])
				break;

			if (strcmp(pszColumnName, AGPMSIEGEWAR_STREAM_CASTLE_NAME) == 0)
				m_csSiegeWarInfo[i].m_strCastleName.SetText(pszValue);

			else if (strcmp(pszColumnName, AGPMSIEGEWAR_STREAM_REGION_NAME) == 0)
			{
				CHAR	szRegionName1[128];
				CHAR	szRegionName2[128];

				ZeroMemory(szRegionName1, sizeof(szRegionName1));
				ZeroMemory(szRegionName2, sizeof(szRegionName2));

				int nStrIndex = 0;

				for (nStrIndex = 0; nStrIndex < (int) strlen(pszValue); ++nStrIndex)
				{
					if (pszValue[nStrIndex] == '/')
						break;
				}

				CopyMemory(szRegionName1, pszValue, sizeof(CHAR) * nStrIndex);
				CopyMemory(szRegionName2, pszValue + nStrIndex + 1, sizeof(CHAR) * (strlen(pszValue) - nStrIndex - 1));

				m_csSiegeWarInfo[i].m_apcsRegionTemplate[0]	= m_pcsApmMap->GetTemplate(szRegionName1);
				m_csSiegeWarInfo[i].m_apcsRegionTemplate[1]	= m_pcsApmMap->GetTemplate(szRegionName2);
			}

			else if (strcmp(pszColumnName, AGPMSIEGEWAR_STREAM_SIEGEWAR_PROGRESS_TIME) == 0)
				m_csSiegeWarInfo[i].m_csTemplate.m_ulSiegeWarDurationMSec	= (UINT32) atoi(pszValue);

			else if (strcmp(pszColumnName, AGPMSIEGEWAR_STREAM_OPEN_ARCHON_EYE_TIME) == 0)
				m_csSiegeWarInfo[i].m_csTemplate.m_ulOpenAllEyesDurationMSec	= (UINT32) atoi(pszValue);

			else if (strcmp(pszColumnName, AGPMSIEGEWAR_STREAM_CARVE_A_THRONE) == 0)
				m_csSiegeWarInfo[i].m_csTemplate.m_ulNeedTimeForCarveMSec	= (UINT32) atoi(pszValue);

			else if (strcmp(pszColumnName, AGSMSIEGEWAR_STREAM_PROCLAIM_TIME) == 0)
				m_csSiegeWarInfo[i].m_csTemplate.m_ulProclaimTimeHour	= (UINT32) atoi(pszValue);

			else if (strcmp(pszColumnName, AGSMSIEGEWAR_STREAM_APPLICATION_TIME) == 0)
				m_csSiegeWarInfo[i].m_csTemplate.m_ulApplicationTimeHour	= (UINT32) atoi(pszValue);

			else if (strcmp(pszColumnName, AGSMSIEGEWAR_STREAM_SIEGEWAR_INTERVAL) == 0)
				m_csSiegeWarInfo[i].m_csTemplate.m_ulSiegeWarIntervalDay	= (UINT32) atoi(pszValue);

			else if ( strcmp(pszColumnName, AGSMSIEGEWAR_STREAM_BEGIN_TIME ) == 0 )
			{
				UINT32 beginTime = (UINT32) atoi( pszValue );
				if ( beginTime > 24 ) beginTime = AGPMSIEGEWAR_DEFAULT_BEGIN_TIME;

				m_csSiegeWarInfo[i].m_csTemplate.m_ulSiegeWarBeginTime = beginTime;
			}
		}
	}

	return TRUE;
}

BOOL AgpmSiegeWar::CharacterImportDataReadCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmSiegeWar			*pThis						= (AgpmSiegeWar *)			pClass;
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate	= (AgpdCharacterTemplate *)	pData;
	PVOID					*ppvBuffer					= (PVOID *)					pCustData;

	AuExcelLib				*pcsExcelTxtLib				= (AuExcelLib *)			ppvBuffer[0];
	INT32					nRow						= PtrToInt(ppvBuffer[1]);
	INT32					nCol						= PtrToInt(ppvBuffer[2]);

	if (!pcsAgpdCharacterTemplate || !pcsExcelTxtLib)
		return FALSE;

	// key row is zero
	//////////////////////////////////////////////////////////

	CHAR					*pszData				= pcsExcelTxtLib->GetData(nCol, 0);
	if(!pszData)
		return FALSE;

	if(!strcmp(pszData, AGPMSIEGEWAR_IMPORT_SIEGEWAR_CHAR_TYPE))
	{
		CHAR	*pszValue = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszValue)
		{
			AgpdSiegeWarADCharTemplate	*pcsAttachTemplateData	= pThis->GetAttachTemplateData(pcsAgpdCharacterTemplate);
			pcsAttachTemplateData->m_eSiegeWarMonsterType	= (AgpdSiegeWarMonsterType)	atoi(pszValue);
		}
	}

	return TRUE;
}

BOOL AgpmSiegeWar::CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmSiegeWar	*pThis	= (AgpmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdSiegeWarMonsterType	eMonsterType	= pThis->GetSiegeWarMonsterType(pcsCharacter);

	if (eMonsterType == AGPD_SIEGE_MONSTER_CATAPULT ||
		eMonsterType == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER)
	{
		AgpdSiegeWarADCharacter	*pcsAttachData	= pThis->GetAttachCharacterData(pcsCharacter);

		pcsAttachData->m_pcsAttackObjectGrid	= new (AgpdGrid);
		if (pcsAttachData->m_pcsAttackObjectGrid)
		{
			pThis->m_pcsAgpmGrid->Init(pcsAttachData->m_pcsAttackObjectGrid, 1, 2, 4);
		}
	}

	return TRUE;
}

BOOL AgpmSiegeWar::CBIsStaticCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmSiegeWar	*pThis					= (AgpmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter			= (AgpdCharacter *)	pData;
	BOOL			*pbIsStaticCharacter	= (BOOL *)			pCustData;

	*pbIsStaticCharacter	= pThis->IsSiegeWarMonster(pcsCharacter);

	return TRUE;
}

BOOL AgpmSiegeWar::IsRespawnMonster(AgpdCharacter* pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (GetSiegeWarMonsterType(pcsCharacter) != AGPD_SIEGE_MONSTER_TYPE_NONE ||
		pcsCharacter->m_lTID1 == 1366)
		return FALSE;

	return TRUE;
}

BOOL AgpmSiegeWar::CBCheckNPCAttackableTarget(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmSiegeWar	*pThis	= (AgpmSiegeWar *)	pClass;
	PVOID			*ppvBuffer	= (PVOID *)	pData;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	AgpdCharacter	*pcsTarget		= (AgpdCharacter *)	ppvBuffer[1];
	BOOL			*pbIsSiegeWarObject	= (BOOL *)	ppvBuffer[2];
	BOOL			*pbCheckResult	= (BOOL *)	ppvBuffer[3];

	AgpdSiegeWarMonsterType	eMonsterType	= pThis->GetSiegeWarMonsterType(pcsTarget);
	if (eMonsterType == AGPD_SIEGE_MONSTER_TYPE_NONE)
	{
		*pbIsSiegeWarObject	= FALSE;
		return TRUE;
	}

	*pbIsSiegeWarObject	= TRUE;
	*pbCheckResult		= FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->GetSiegeWarInfo(pcsCharacter);
	if (!pcsSiegeWar)
		return TRUE;

	AuAutoLock	lock(pThis->m_Mutex);
	if (!lock.Result()) return FALSE;

	if (pThis->IsAttackGuild(pcsCharacter, pcsSiegeWar))
	{
		if (eMonsterType == AGPD_SIEGE_MONSTER_GUARD_TOWER ||
			eMonsterType == AGPD_SIEGE_MONSTER_OUTER_GATE)
		{
			*pbCheckResult	= TRUE;
		}
		else if (eMonsterType == AGPD_SIEGE_MONSTER_INNER_GATE ||
			eMonsterType == AGPD_SIEGE_MONSTER_DEFENSE_RESURRECTION_TOWER)
		{
			if (!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_OUTER_GATE][0] &&
				!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_GUARD_TOWER][0])
				*pbCheckResult	= TRUE;
			else
			{
				// 외성문이 열려야 부술 수 있다.
				for (int i = 0; i < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++i)
				{
					if (!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_OUTER_GATE][i])
						break;

					if (pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_OUTER_GATE][i]->IsDead())
					{
						*pbCheckResult	= TRUE;
						break;
					}
				}

				if (*pbCheckResult == FALSE)
				{
					for (int i = 0; i < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++i)
					{
						if (!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_GUARD_TOWER][i])
							break;

						if (pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_GUARD_TOWER][i]->IsDead())
						{
							*pbCheckResult	= TRUE;
							break;
						}
					}
				}
			}
		}
		else if (eMonsterType == AGPD_SIEGE_MONSTER_LIFE_TOWER ||
			eMonsterType == AGPD_SIEGE_MONSTER_THRONE ||
			eMonsterType == AGPD_SIEGE_MONSTER_ARCHON_EYES)
		{
			if (eMonsterType == AGPD_SIEGE_MONSTER_THRONE &&
				pThis->IsArchlordCastle(pcsSiegeWar))
			{
				*pbCheckResult	= FALSE;
			}
			else if (!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_INNER_GATE][0])
			{
				*pbCheckResult	= TRUE;
			}
			else
			{
				// 내성문이 열려야 부술 수 있다.
				for (int i = 0; i < AGPMSIEGEWAR_GUILD_MAX_OBJECT_CHARACTER; ++i)
				{
					if (!pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_INNER_GATE][i])
						break;

					if (pcsSiegeWar->m_apcsObjectChar[AGPD_SIEGE_MONSTER_INNER_GATE][i]->IsDead())
					{
						*pbCheckResult	= TRUE;
						break;
					}
				}
			}
		}
	}
	else if (pThis->IsDefenseGuild(pcsCharacter, pcsSiegeWar))
	{
		if (eMonsterType == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER ||
			eMonsterType == AGPD_SIEGE_MONSTER_CATAPULT)
		{
			*pbCheckResult	= TRUE;
		}
	}

	return TRUE;
}

BOOL AgpmSiegeWar::CBIsAttackableTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmSiegeWar	*pThis			= (AgpmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgpdCharacter	*pcsTarget		= (AgpdCharacter *)	pCustData;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->GetSiegeWarInfo(pcsCharacter);
	if (pThis->IsEndArchlordSiegeWar(pcsSiegeWar))
		return FALSE;

	pcsSiegeWar	= pThis->GetSiegeWarInfo(pcsTarget);
	if (pThis->IsEndArchlordSiegeWar(pcsSiegeWar))
		return FALSE;

	return TRUE;
}

BOOL AgpmSiegeWar::CBIsInSiegeWarIngArea(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmSiegeWar	*pThis			= (AgpmSiegeWar *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID			*ppvBuffer		= (PVOID *) pCustData;
	BOOL *pbIsInSiegeWarIngArea		= (BOOL *) ppvBuffer[0];
	BOOL *pbIsParticipantSiegeWar	= (BOOL *) ppvBuffer[1];

	if (pbIsInSiegeWarIngArea)
	{
		AgpdSiegeWar	*pcsSiegeWar1	= pThis->GetSiegeWarInfo(pcsCharacter);
		if (pcsSiegeWar1 && pThis->IsStarted(pcsSiegeWar1))
			*pbIsInSiegeWarIngArea = TRUE;
	}
	
	if (pbIsParticipantSiegeWar)
	{
		AgpdSiegeWar	*pcsSiegeWar2 = pThis->GetSiegeWarInfoOfCharacter(pcsCharacter);
		if (pcsSiegeWar2 && pThis->IsStarted(pcsSiegeWar2))
			*pbIsParticipantSiegeWar = TRUE;
	}

	return TRUE;
}

BOOL AgpmSiegeWar::CBCheckDestroy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmSiegeWar	*pThis			= (AgpmSiegeWar *)	pClass;
	AgpdGuild		*pcsGuild		= (AgpdGuild *)		pData;
	BOOL			*pbCheckResult	= (BOOL *)			pCustData;

	if (!*pbCheckResult)
		return TRUE;

	AuAutoLock	lock(pThis->m_Mutex);
	if (!lock.Result()) return FALSE;

	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		if (pThis->m_csSiegeWarInfo[i].m_csAttackGuild.GetObject(pcsGuild->m_szID) ||
			pThis->m_csSiegeWarInfo[i].m_csDefenseGuild.GetObject(pcsGuild->m_szID))
		{
			*pbCheckResult	= FALSE;
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AgpmSiegeWar::CBDestroyGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmSiegeWar	*pThis		= (AgpmSiegeWar *)	pClass;
	AgpdGuild		*pcsGuild	= (AgpdGuild *)		pData;

	AuAutoLock	lock(pThis->m_Mutex);
	if (!lock.Result()) return FALSE;

	for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		pThis->RemoveAttackApplicationGuild(&pThis->m_csSiegeWarInfo[i], pcsGuild);
		pThis->RemoveDefenseApplicationGuild(&pThis->m_csSiegeWarInfo[i], pcsGuild);
	}

	return TRUE;
}

BOOL AgpmSiegeWar::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || !pstCheckArg || nSize < sizeof(PACKET_HEADER))
		return FALSE;

	INT8	cOperation			= (-1);
	INT8	cResult				= (-1);
	PVOID	pvPacketCastleInfo	= NULL;
	INT32	lCID				= AP_INVALID_CID;
	PVOID	pvPacketGuildList	= NULL;
	INT32	lTargetCID			= AP_INVALID_CID;
	PVOID	pvPacketAttackObjectInfo	= NULL;
	INT32	lItemID				= AP_INVALID_IID;
	PVOID	pvPacketStatusInfo	= NULL;
	UINT32	ulCurrentTimeStamp	= 0;
	PVOID	pvPacketSyncMessage	= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&cResult,
						&pvPacketCastleInfo,
						&lCID,
						&pvPacketGuildList,
						&lTargetCID,
						&pvPacketAttackObjectInfo,
						&lItemID,
						&pvPacketStatusInfo,
						&ulCurrentTimeStamp,
						&pvPacketSyncMessage);

	if (!pstCheckArg->bReceivedFromServer &&
		pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
		lCID != pstCheckArg->lSocketOwnerID)
		return FALSE;

	AuAutoLock	lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	BOOL	bReturn	= TRUE;

	switch (cOperation)
	{
	case AGPMSIEGE_OPERATION_UPDATE:
		bReturn	= OnOperationUpdate(pvPacketCastleInfo, pstCheckArg);
		break;

	case AGPMSIEGE_OPERATION_SET_NEXT_SIEGEWAR_TIME:
		bReturn	= OnOperationSetNextSiegeWarTime(lCID, pvPacketCastleInfo, pstCheckArg);
		break;

	case AGPMSIEGE_OPERATION_DEFENSE_APPLICATION:
		bReturn	= OnOperationDefenseApplication(lCID, pvPacketCastleInfo, pstCheckArg);
		break;

	case AGPMSIEGE_OPERATION_CANCEL_DEFENSE_APPLICATION:
		bReturn	= OnOperationCancelDefenseApplication(lCID, pvPacketCastleInfo, pstCheckArg);
		break;

	case AGPMSIEGE_OPERATION_ATTACK_APPLICATION:
		bReturn	= OnOperationAttackApplication(lCID, pvPacketCastleInfo, pstCheckArg);
		break;

	case AGPMSIEGE_OPERATION_CANCEL_ATTACK_APPLICATION:
		bReturn	= OnOperationCancelAttackApplication(lCID, pvPacketCastleInfo, pstCheckArg);
		break;

	case AGPMSIEGE_OPERATION_RESULT:
		bReturn	= OnOperationResult(cResult, pvPacketCastleInfo, pstCheckArg);
		break;

	case AGPMSIEGE_OPERATION_REQUEST_ATTACK_APPL_GUILD_LIST:
		bReturn	= OnOperationRequestAttackApplGuildList(pvPacketCastleInfo, lCID, pvPacketGuildList);
		break;

	case AGPMSIEGE_OPERATION_REQUEST_DEFENSE_APPL_GUILD_LIST:
		bReturn	= OnOperationRequestDefenseApplGuildList(pvPacketCastleInfo, lCID, pvPacketGuildList);
		break;

	case AGPMSIEGE_OPERATION_ATTACK_APPL_GUILD_LIST:
		bReturn	= OnOperationAttackApplGuildList(pvPacketGuildList);
		break;

	case AGPMSIEGE_OPERATION_DEFENSE_APPL_GUILD_LIST:
		bReturn	= OnOperationDefenseApplGuildList(pvPacketGuildList);
		break;

	case AGPMSIEGE_OPERATION_SELECT_DEFENSE_GUILD:
		bReturn = OnOperationSelectDefenseGuild(lCID, pvPacketCastleInfo);
		break;

	case AGPMSIEGE_OPERATION_REQUEST_ATTACK_GUILD_LIST:
		bReturn	= OnOperationRequestAttackGuildList(pvPacketCastleInfo, lCID);
		break;

	case AGPMSIEGE_OPERATION_REQUEST_DEFENSE_GUILD_LIST:
		bReturn	= OnOperationRequestDefenseGuildList(pvPacketCastleInfo, lCID);
		break;

	case AGPMSIEGE_OPERATION_ATTACK_GUILD_LIST:
		bReturn	= OnOperationAttackGuildList(pvPacketCastleInfo, pvPacketGuildList);
		break;

	case AGPMSIEGE_OPERATION_DEFENSE_GUILD_LIST:
		bReturn	= OnOperationDefenseGuildList(pvPacketCastleInfo, pvPacketGuildList);
		break;

	case AGPMSIEGE_OPERATION_UPDATE_ATTACK_OBJECT:
		bReturn	= OnOperationUpdateAttackObject(lCID, pvPacketAttackObjectInfo);
		break;

	case AGPMSIEGE_OPERATION_REQUEST_OPEN_ATTACK_OBJECT:
		bReturn	= OnOperationRequestOpenAttackObject(lCID, lTargetCID);
		break;

	case AGPMSIEGE_OPERATION_RESPONSE_OPEN_ATTACK_OBJECT:
		bReturn	= OnOperationResponseOpenAttackObject(lCID, lTargetCID, cResult, pvPacketAttackObjectInfo);
		break;

	case AGPMSIEGE_OPERATION_REQUEST_USE_ATTACK_OBJECT:
		bReturn	= OnOperationRequestUseAttackObject(lCID, lTargetCID);
		break;

	case AGPMSIEGE_OPERATION_RESPONSE_USE_ATTACK_OBJECT:
		bReturn	= OnOperationResponseUseAttackObject(lCID, lTargetCID, cResult, pvPacketAttackObjectInfo);
		break;

	case AGPMSIEGE_OPERATION_REQUEST_REPAIR_ATTACK_OBJECT:
		bReturn	= OnOperationRequestRepairAttackObject(lCID, lTargetCID);
		break;

	case AGPMSIEGE_OPERATION_RESPONSE_REPAIR_ATTACK_OBJECT:
		bReturn	= OnOperationResponseRepairAttackObject(lCID, lTargetCID, cResult);
		break;

	case AGPMSIEGE_OPERATION_ADD_ITEM_TO_ATTACK_OBJECT:
		bReturn	= OnOperationAddItemToAttackObject(lCID, lTargetCID, lItemID);
		break;

	case AGPMSIEGE_OPERATION_REMOVE_ITEM_TO_ATTACK_OBJECT:
		bReturn = OnOperationRemoveItemToAttackObject(lCID, lTargetCID, lItemID);
		break;

	case AGPMSIEGE_OPERATION_ADD_ITEM_RESULT:
		bReturn	= OnOperationAddItemResult(lCID, lTargetCID, lItemID);
		break;

	case AGPMSIEGE_OPERATION_CARVE_A_SEAL:
		bReturn	= OnOperationCarveASeal(lCID, lTargetCID);
		break;

	case AGPMSIEGE_OPERATION_STATUS_INFO:
		bReturn = OnOperationStatusInfo(pvPacketStatusInfo);
		break;

	case AGPMSIEGE_OPERATION_MESSAGE:
		bReturn	= OnOperationMessage(pvPacketSyncMessage);
		break;
	}

	return bReturn;
}

BOOL AgpmSiegeWar::OnOperationUpdate(PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg)
{
	if (!pstCheckArg || !pstCheckArg->bReceivedFromServer)
		return FALSE;

	if (pvPacketCastleInfo)
	{
		CHAR	*pszCastleName			= NULL;
		CHAR	*pszGuildMasterName		= NULL;
		CHAR	*pszOwnerGuildName		= NULL;
		CHAR	*pszCarveGuildName		= NULL;

		UINT16	unCastleNameLength		= 0;
		UINT16	unGuildMasterNameLength	= 0;
		UINT16	unOwnerGuildNameLength	= 0;
		UINT16	unCarveGuildNameLength	= 0;

		UINT8	ucGuildMemberCount		= 0;
		UINT64	ullPrevSiegeWarTimeDate	= 0;
		UINT64	ullNextSiegeWarTimeDate	= 0;

		UINT8	ucSiegeWarStatus		= 0;

		UINT8	ucIsActiveAttackResurrection	= 2;
		UINT8	ucIsActiveDefenseResurrection	= 2;
		UINT8	ucIsReverseAttack				= 2;

		m_csPacketCastleInfo.GetField(FALSE, pvPacketCastleInfo, 0,
									  &pszCastleName, &unCastleNameLength,
									  &pszGuildMasterName, &unGuildMasterNameLength,
									  &pszOwnerGuildName, &unOwnerGuildNameLength,
									  &ucGuildMemberCount,
									  &ullPrevSiegeWarTimeDate,
									  &ullNextSiegeWarTimeDate,
									  &ucSiegeWarStatus,
									  &ucIsActiveAttackResurrection,
									  &ucIsActiveDefenseResurrection,
									  &ucIsReverseAttack,
									  &pszCarveGuildName, &unCarveGuildNameLength);

		CHAR	szCastleName[AGPMSIEGEWAR_MAX_CASTLE_NAME + 1];
		CHAR	szGuildMasterName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
		CHAR	szOwnerGuildName[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
		CHAR	szCarveGuildName[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];

		ZeroMemory(szCastleName, sizeof(szCastleName));
		ZeroMemory(szGuildMasterName, sizeof(szGuildMasterName));
		ZeroMemory(szOwnerGuildName, sizeof(szOwnerGuildName));
		ZeroMemory(szCarveGuildName, sizeof(szCarveGuildName));

		if (pszCastleName && unCastleNameLength > 0)
		{
			if (unCastleNameLength > AGPMSIEGEWAR_MAX_CASTLE_NAME)
				unCastleNameLength	= AGPMSIEGEWAR_MAX_CASTLE_NAME;

			CopyMemory(szCastleName, pszCastleName, sizeof(CHAR) * unCastleNameLength);
		}

		if (pszGuildMasterName && unGuildMasterNameLength > 0)
		{
			if (unGuildMasterNameLength > AGPDCHARACTER_MAX_ID_LENGTH)
				unGuildMasterNameLength	= AGPDCHARACTER_MAX_ID_LENGTH;

			CopyMemory(szGuildMasterName, pszGuildMasterName, sizeof(CHAR) * unGuildMasterNameLength);
		}

		if (pszOwnerGuildName && unOwnerGuildNameLength > 0)
		{
			if (unOwnerGuildNameLength > AGPMGUILD_MAX_GUILD_ID_LENGTH)
				unOwnerGuildNameLength	= AGPMGUILD_MAX_GUILD_ID_LENGTH;

			CopyMemory(szOwnerGuildName, pszOwnerGuildName, sizeof(CHAR) * unOwnerGuildNameLength);
		}

		if (pszCarveGuildName && unCarveGuildNameLength > 0)
		{
			if (unCarveGuildNameLength > AGPMGUILD_MAX_GUILD_ID_LENGTH)
				unCarveGuildNameLength	= AGPMGUILD_MAX_GUILD_ID_LENGTH;

			CopyMemory(szCarveGuildName, pszCarveGuildName, sizeof(CHAR) * unCarveGuildNameLength);
		}

		AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(szCastleName);
		if (pcsSiegeWar)
		{
			if (strlen(szOwnerGuildName) > 0)
				pcsSiegeWar->m_strOwnerGuildName.SetText(szOwnerGuildName);
			if (strlen(szGuildMasterName) > 0)
				pcsSiegeWar->m_strOwnerGuildMasterName.SetText(szGuildMasterName);
			if (strlen(szCarveGuildName) > 0)
				pcsSiegeWar->m_strCarveGuildName.SetText(szCarveGuildName);

			if (ucGuildMemberCount > 0)
				pcsSiegeWar->m_ucOwnerGuildMemberCount	= ucGuildMemberCount;

			if (ullPrevSiegeWarTimeDate > 0)
				pcsSiegeWar->m_ullPrevSiegeWarTimeDate	= ullPrevSiegeWarTimeDate;
			if (ullNextSiegeWarTimeDate > 0)
				pcsSiegeWar->m_ullNextSiegeWarTimeDate	= ullNextSiegeWarTimeDate;

			INT32	lOldStatus	= (INT32) pcsSiegeWar->m_eCurrentStatus;

			if (ucSiegeWarStatus > 0)
				pcsSiegeWar->m_eCurrentStatus			= (AgpdSiegeWarStatus) ucSiegeWarStatus;

			if (ucIsActiveAttackResurrection < 2)
				pcsSiegeWar->m_bIsActiveAttackResurrection	= (BOOL)	ucIsActiveAttackResurrection;
			if (ucIsActiveDefenseResurrection < 2)
				pcsSiegeWar->m_bIsActiveDefenseResurrection	= (BOOL)	ucIsActiveDefenseResurrection;

			if (ucIsReverseAttack < 2)
				pcsSiegeWar->m_bIsReverseAttack	= (BOOL)	ucIsReverseAttack;

			EnumCallback(AGPMSIEGEWAR_CB_UPDATE_CASTLE_INFO, pcsSiegeWar, &lOldStatus);
		}
	}

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationSetNextSiegeWarTime(INT32 lCID, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg)
{
	if (!pvPacketCastleInfo || !pstCheckArg)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	CHAR	*pszCastleName			= NULL;
	CHAR	*pszGuildMasterName		= NULL;
	CHAR	*pszOwnerGuildName		= NULL;

	UINT16	unCastleNameLength		= 0;
	UINT16	unGuildMasterNameLength	= 0;
	UINT16	unOwnerGuildNameLength	= 0;

	CHAR	szCastleName[AGPMSIEGEWAR_MAX_CASTLE_NAME + 1];
	//CHAR	szGuildMasterName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	//CHAR	szOwnerGuildName[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];

	ZeroMemory(szCastleName, sizeof(szCastleName));
	//ZeroMemory(szGuildMasterName, sizeof(szGuildMasterName));
	//ZeroMemory(szOwnerGuildName, sizeof(szOwnerGuildName));

	UINT8	ucGuildMemberCount		= 0;
	UINT64	ullPrevSiegeWarTimeDate	= 0;
	UINT64	ullNextSiegeWarTimeDate	= 0;
	UINT8	ucSiegeWarStatus		= 0;

	m_csPacketCastleInfo.GetField(FALSE, pvPacketCastleInfo, 0,
									&pszCastleName, &unCastleNameLength,
									&pszGuildMasterName, &unGuildMasterNameLength,
									&pszOwnerGuildName, &unOwnerGuildNameLength,
									&ucGuildMemberCount,
									&ullPrevSiegeWarTimeDate,
									&ullNextSiegeWarTimeDate,
									&ucSiegeWarStatus,
									NULL,
									NULL,
									NULL,
									NULL);

	if (unCastleNameLength > 0 &&
		unCastleNameLength <= AGPMSIEGEWAR_MAX_CASTLE_NAME)
		CopyMemory(szCastleName, pszCastleName, sizeof(CHAR) * unCastleNameLength);
	//if (unGuildMasterNameLength > 0 &&
	//	unGuildMasterNameLength <= AGPDCHARACTER_MAX_ID_LENGTH)
	//	CopyMemory(szGuildMasterName, pszGuildMasterName, sizeof(CHAR) * unGuildMasterNameLength);
	//if (unOwnerGuildNameLength > 0 &&
	//	unOwnerGuildNameLength <=AGPMGUILD_MAX_GUILD_ID_LENGTH)
	//	CopyMemory(szOwnerGuildName, pszOwnerGuildName, sizeof(CHAR) * unOwnerGuildNameLength);

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(szCastleName);
	if (!pcsSiegeWar ||
		strcmp(pcsSiegeWar->m_strOwnerGuildMasterName, pcsCharacter->m_szID) != 0 &&
		m_pcsAgpmAdmin && m_pcsAgpmAdmin->GetAdminLevel(pcsCharacter) == 0)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	AgpmSiegeResult	eResult	= SetNextSiegeWarTime(pcsSiegeWar, ullNextSiegeWarTimeDate);

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID)	pcsCharacter;
	pvBuffer[1]	= (PVOID)	eResult;

	EnumCallback(AGPMSIEGEWAR_CB_SET_NEXT_SIEGEWAR_TIME, pcsSiegeWar, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationDefenseApplication(INT32 lCID, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg)
{
	if (!pvPacketCastleInfo)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter ||
		!CheckApplicationRequirement(pcsCharacter))
		return FALSE;

	AgpmSiegeResult	eResult	= AddDefenseApplicationGuild(pcsSiegeWar, m_pcsAgpmGuild->GetGuild(pcsCharacter));

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID)	pcsCharacter;
	pvBuffer[1]	= (PVOID)	eResult;

	EnumCallback(AGPMSIEGEWAR_CB_ADD_DEFENSE_APPLICATION, pcsSiegeWar, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationCancelDefenseApplication(INT32 lCID, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg)
{
	if (!pvPacketCastleInfo)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter ||
		!m_pcsAgpmGuild->IsMaster(m_pcsAgpmGuild->GetGuild(pcsCharacter), pcsCharacter->m_szID))
		return FALSE;

	AgpmSiegeResult	eResult	= RemoveDefenseApplicationGuild(pcsSiegeWar, m_pcsAgpmGuild->GetGuild(pcsCharacter));

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID)	pcsCharacter;
	pvBuffer[1]	= (PVOID)	eResult;

	EnumCallback(AGPMSIEGEWAR_CB_REMOVE_DEFENSE_APPLICATION, pcsSiegeWar, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationAttackApplication(INT32 lCID, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg)
{
	if (!pvPacketCastleInfo)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter ||
		!CheckApplicationRequirement(pcsCharacter))
		return FALSE;

	AgpmSiegeResult	eResult	= AddAttackApplicationGuild(pcsSiegeWar, m_pcsAgpmGuild->GetGuild(pcsCharacter));

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID)	pcsCharacter;
	pvBuffer[1]	= (PVOID)	eResult;

	EnumCallback(AGPMSIEGEWAR_CB_ADD_ATTACK_APPLICATION, pcsSiegeWar, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationCancelAttackApplication(INT32 lCID, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg)
{
	if (!pvPacketCastleInfo)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter ||
		!m_pcsAgpmGuild->IsMaster(m_pcsAgpmGuild->GetGuild(pcsCharacter), pcsCharacter->m_szID))
		return FALSE;

	AgpmSiegeResult	eResult	= RemoveAttackApplicationGuild(pcsSiegeWar, m_pcsAgpmGuild->GetGuild(pcsCharacter));

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID)	pcsCharacter;
	pvBuffer[1]	= (PVOID)	eResult;

	EnumCallback(AGPMSIEGEWAR_CB_REMOVE_ATTACK_APPLICATION, pcsSiegeWar, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationRequestAttackApplGuildList(PVOID pvPacketCastleInfo, INT32 lCID, PVOID pvPacketGuildList)
{
	if (!pvPacketCastleInfo || !pvPacketGuildList)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPage				= (-1);
	INT16	nTotalPage			= (-1);
	PVOID	pvGuildListPacket	= NULL;

	m_csPacketGuildList.GetField(FALSE, pvPacketGuildList, 0,
								 &nPage,
								 &nTotalPage,
								 &pvGuildListPacket);

	if (nPage < 0)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID)	pcsCharacter;
	pvBuffer[1]	= (PVOID)	pcsSiegeWar;

	EnumCallback(AGPMSIEGEWAR_CB_REQUEST_ATTACK_APPL_GUILD_LIST, &nPage, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationRequestDefenseApplGuildList(PVOID pvPacketCastleInfo, INT32 lCID, PVOID pvPacketGuildList)
{
	if (!pvPacketCastleInfo || !pvPacketGuildList)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPage				= (-1);
	INT16	nTotalPage			= (-1);
	PVOID	pvGuildListPacket	= NULL;

	m_csPacketGuildList.GetField(FALSE, pvPacketGuildList, 0,
								 &nPage,
								 &nTotalPage,
								 &pvGuildListPacket);

	if (nPage < 0)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID)	pcsCharacter;
	pvBuffer[1]	= (PVOID)	pcsSiegeWar;

	EnumCallback(AGPMSIEGEWAR_CB_REQUEST_DEFENSE_APPL_GUILD_LIST, &nPage, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationAttackApplGuildList(PVOID pvPacketGuildList)
{
	if (!pvPacketGuildList)
		return FALSE;

	INT16	nPage				= 0;
	INT16	nTotalPage			= 0;
	PVOID	ppvGuildListPacket[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];
	ZeroMemory(ppvGuildListPacket, sizeof(ppvGuildListPacket));

	m_csPacketGuildList.GetField(FALSE, pvPacketGuildList, 0,
								 &nPage,
								 &nTotalPage,
								 ppvGuildListPacket);

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID) &nPage;
	pvBuffer[1]	= (PVOID) &nTotalPage;

	return EnumCallback(AGPMSIEGEWAR_CB_RECEIVE_ATTACK_APPL_GUILD_LIST, ppvGuildListPacket, pvBuffer);
}

BOOL AgpmSiegeWar::OnOperationDefenseApplGuildList(PVOID pvPacketGuildList)
{
	if (!pvPacketGuildList)
		return FALSE;

	INT16	nPage				= 0;
	INT16	nTotalPage			= 0;
	PVOID	ppvGuildListPacket[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];
	ZeroMemory(ppvGuildListPacket, sizeof(ppvGuildListPacket));

	m_csPacketGuildList.GetField(FALSE, pvPacketGuildList, 0,
								 &nPage,
								 &nTotalPage,
								 ppvGuildListPacket);

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID) &nPage;
	pvBuffer[1]	= (PVOID) &nTotalPage;

	return EnumCallback(AGPMSIEGEWAR_CB_RECEIVE_DEFENSE_APPL_GUILD_LIST, ppvGuildListPacket, pvBuffer);
}

BOOL AgpmSiegeWar::OnOperationResult(INT8 cResult, PVOID pvPacketCastleInfo, DispatchArg *pstCheckArg)
{
	if (!pvPacketCastleInfo || !pstCheckArg)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	return EnumCallback(AGPMSIEGEWAR_CB_RECEIVE_RESULT, pcsSiegeWar, &cResult);
}

BOOL AgpmSiegeWar::OnOperationSelectDefenseGuild(INT32 lCID, PVOID pvPacketCastleInfo)
{
	if (!pvPacketCastleInfo)
		return FALSE;

	CHAR	*pszCastleName			= NULL;
	CHAR	*pszGuildMasterName		= NULL;
	CHAR	*pszOwnerGuildName		= NULL;

	UINT16	unCastleNameLength		= 0;
	UINT16	unGuildMasterNameLength	= 0;
	UINT16	unOwnerGuildNameLength	= 0;

	CHAR	szCastleName[AGPMSIEGEWAR_MAX_CASTLE_NAME + 1];
	//CHAR	szGuildMasterName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	CHAR	szOwnerGuildName[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];

	ZeroMemory(szCastleName, sizeof(szCastleName));
	//ZeroMemory(szGuildMasterName, sizeof(szGuildMasterName));
	ZeroMemory(szOwnerGuildName, sizeof(szOwnerGuildName));

	UINT8	ucGuildMemberCount		= 0;
	UINT64	ullPrevSiegeWarTimeDate	= 0;
	UINT64	ullNextSiegeWarTimeDate	= 0;
	UINT8	ucSiegeWarStatus		= 0;

	m_csPacketCastleInfo.GetField(FALSE, pvPacketCastleInfo, 0,
									&pszCastleName, &unCastleNameLength,
									&pszGuildMasterName, &unGuildMasterNameLength,
									&pszOwnerGuildName, &unOwnerGuildNameLength,
									&ucGuildMemberCount,
									&ullPrevSiegeWarTimeDate,
									&ullNextSiegeWarTimeDate,
									&ucSiegeWarStatus,
									NULL,
									NULL,
									NULL,
									NULL);

	if (unCastleNameLength > 0 &&
		unCastleNameLength <= AGPMSIEGEWAR_MAX_CASTLE_NAME)
		CopyMemory(szCastleName, pszCastleName, sizeof(CHAR) * unCastleNameLength);
	//if (unGuildMasterNameLength > 0 &&
	//	unGuildMasterNameLength <= AGPDCHARACTER_MAX_ID_LENGTH)
	//	CopyMemory(szGuildMasterName, pszGuildMasterName, sizeof(CHAR) * unGuildMasterNameLength);
	if (unOwnerGuildNameLength > 0 &&
		unOwnerGuildNameLength <=AGPMGUILD_MAX_GUILD_ID_LENGTH)
		CopyMemory(szOwnerGuildName, pszOwnerGuildName, sizeof(CHAR) * unOwnerGuildNameLength);

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(szCastleName);
	if (!pcsSiegeWar || pcsSiegeWar->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR)
		return FALSE;

	AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(szOwnerGuildName);
	if (!pcsGuild)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	// 성주인지 확인한다.
	if (!IsCastleOwner(pcsSiegeWar, pcsCharacter->m_szID))
	{
		if (!(pcsSiegeWar->m_strOwnerGuildMasterName.GetLength() == 0 &&
			  m_pcsAgpmAdmin && m_pcsAgpmAdmin->GetAdminLevel(pcsCharacter) > 0))
		{
			pcsCharacter->m_Mutex.Release();
			return FALSE;
		}
	}

	// 수성 길드리스트에 추가한다.
	AgpmSiegeResult	eResult	= SetDefenseGuild(pcsSiegeWar, pcsGuild);

	// 결과를 보내준다.
	PVOID	pvBuffer[3];
	pvBuffer[0]	= (PVOID)	pcsCharacter;
	pvBuffer[1]	= (PVOID)	eResult;
	pvBuffer[2]	= (PVOID)	pcsGuild;
	EnumCallback(AGPMSIEGEWAR_CB_ADD_DEFENSE_GUILD, pcsSiegeWar, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationRequestAttackGuildList(PVOID pvPacketCastleInfo, INT32 lCID)
{
	if (!pvPacketCastleInfo || lCID == AP_INVALID_CID)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	EnumCallback(AGPMSIEGEWAR_CB_REQUEST_ATTACK_GUILD_LIST, pcsSiegeWar, pcsCharacter);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationRequestDefenseGuildList(PVOID pvPacketCastleInfo, INT32 lCID)
{
	if (!pvPacketCastleInfo || lCID == AP_INVALID_CID)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	EnumCallback(AGPMSIEGEWAR_CB_REQUEST_DEFENSE_GUILD_LIST, pcsSiegeWar, pcsCharacter);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationAttackGuildList(PVOID pvPacketCastleInfo, PVOID pvPacketGuildList)
{
	if (!pvPacketCastleInfo || !pvPacketGuildList)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPage				= 0;
	INT16	nTotalPage			= 0;

	PVOID	ppvGuildListPacket[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];

	ZeroMemory(ppvGuildListPacket, sizeof(ppvGuildListPacket));

	m_csPacketGuildList.GetField(FALSE, pvPacketGuildList, 0,
								 &nPage,
								 &nTotalPage,
								 ppvGuildListPacket);

	for (int i = 0; i < AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE; ++i)
	{
		if (!ppvGuildListPacket[i])
			break;

		CHAR	*pszGuildName			= NULL;
		CHAR	*pszGuildMasterID		= NULL;

		UINT16	unGuildNameLength		= 0;
		UINT16	unGuildMasterIDLength	= 0;
		UINT32	ulGuildPoint			= 0;

		m_csPacketGuildInfo.GetField(FALSE, ppvGuildListPacket[i], 0,
									&pszGuildName, &unGuildNameLength,
									&pszGuildMasterID, &unGuildMasterIDLength,
									&ulGuildPoint);

		CHAR	szGuildName[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];

		ZeroMemory(szGuildName, sizeof(szGuildName));

		if (pszGuildName && unGuildNameLength > 0 && unGuildNameLength <= AGPMGUILD_MAX_GUILD_ID_LENGTH)
			CopyMemory(szGuildName, pszGuildName, sizeof(CHAR) * unGuildNameLength);

		INT_PTR	DummyData	= 0;

		if (strlen(szGuildName) > 0)
			pcsSiegeWar->m_csAttackGuild.AddObject((PVOID) &DummyData, szGuildName);
	}

	return EnumCallback(AGPMSIEGEWAR_CB_RECEIVE_ATTACK_GUILD_LIST, ppvGuildListPacket, pcsSiegeWar);
}

BOOL AgpmSiegeWar::OnOperationDefenseGuildList(PVOID pvPacketCastleInfo, PVOID pvPacketGuildList)
{
	if (!pvPacketCastleInfo || !pvPacketGuildList)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pvPacketCastleInfo);
	if (!pcsSiegeWar)
		return FALSE;

	INT16	nPage				= 0;
	INT16	nTotalPage			= 0;
	PVOID	ppvGuildListPacket[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];
	ZeroMemory(ppvGuildListPacket, sizeof(ppvGuildListPacket));

	m_csPacketGuildList.GetField(FALSE, pvPacketGuildList, 0,
								 &nPage,
								 &nTotalPage,
								 ppvGuildListPacket);

	for (int i = 0; i < AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE; ++i)
	{
		if (!ppvGuildListPacket[i])
			break;

		CHAR	*pszGuildName			= NULL;
		CHAR	*pszGuildMasterID		= NULL;

		UINT16	unGuildNameLength		= 0;
		UINT16	unGuildMasterIDLength	= 0;
		UINT32	ulGuildPoint			= 0;

		m_csPacketGuildInfo.GetField(FALSE, ppvGuildListPacket[i], 0,
									&pszGuildName, &unGuildNameLength,
									&pszGuildMasterID, &unGuildMasterIDLength,
									&ulGuildPoint);

		CHAR	szGuildName[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];

		ZeroMemory(szGuildName, sizeof(szGuildName));

		if (pszGuildName && unGuildNameLength > 0 && unGuildNameLength <= AGPMGUILD_MAX_GUILD_ID_LENGTH)
			CopyMemory(szGuildName, pszGuildName, sizeof(CHAR) * unGuildNameLength);

		INT_PTR	DummyData	= 0;

		if (strlen(szGuildName) > 0)
			pcsSiegeWar->m_csDefenseGuild.AddObject((PVOID) &DummyData, szGuildName);
	}

	return EnumCallback(AGPMSIEGEWAR_CB_RECEIVE_DEFENSE_GUILD_LIST, ppvGuildListPacket, pcsSiegeWar);
}

BOOL AgpmSiegeWar::OnOperationUpdateAttackObject(INT32 lCID, PVOID pvPacketAttackObjectInfo)
{
	AgpdCharacter	*pcsCharacter		= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
	{
		return FALSE;
	}

	if (pvPacketAttackObjectInfo)
	{
		AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsCharacter);

		//pcsAttachData->ucRepairCount			= 0;
		//pcsAttachData->lRepairCost			= 0;
		//pcsAttachData->ucRepairDuration		= 0;

		INT32	alItemID[8];
		for (int i = 0; i < 8; ++i)
			alItemID[i]	= (-1);

		CHAR	*pszUserName		= NULL;
		UINT16	unUserNameLength	= 0;

		UINT8	ucRepairElapsedSec	= 0;

		m_csPacketAttackObjectInfo.GetField(FALSE, pvPacketAttackObjectInfo, 0,
			&pcsAttachData->ucRepairCount,
			&pcsAttachData->lRepairCost,
			&pcsAttachData->ucRepairDuration,
			&pcsAttachData->m_ucRepairedCount,
			&alItemID[0],
			&alItemID[1],
			&alItemID[2],
			&alItemID[3],
			&alItemID[4],
			&alItemID[5],
			&alItemID[6],
			&alItemID[7],
			&ucRepairElapsedSec,

			&pszUserName, &unUserNameLength);

		pcsAttachData->m_ulStartRepairTimeMSec	= GetClockCount() - ucRepairElapsedSec * 1000;

		if (pszUserName && unUserNameLength <= AGPDCHARACTER_MAX_ID_LENGTH)
		{
			CHAR	szUserName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
			ZeroMemory(szUserName, sizeof(szUserName));

			CopyMemory(szUserName, pszUserName, unUserNameLength);

			pcsAttachData->m_strUseCharacterName.SetText(szUserName);
		}

		if (pcsAttachData->m_pcsAttackObjectGrid)
		{
			for (int i = 0; i < 8; ++i)
			{
				if (alItemID[i] == (-1))
					continue;

				AgpdItem	*pcsItem	= m_pcsAgpmItem->GetItem(alItemID[i]);
				if (pcsItem)
				{
					m_pcsAgpmGrid->Add(pcsAttachData->m_pcsAttackObjectGrid, 0, i / 4, i % 4, pcsItem->m_pcsGridItem, 1, 1);
				}
				else
				{
					m_pcsAgpmGrid->Clear(pcsAttachData->m_pcsAttackObjectGrid, 0, i / 4, i % 4, 1, 1);
				}
			}
		}
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationRequestOpenAttackObject(INT32 lCID, INT32 lTargetCID)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacterLock(lTargetCID);
	if (!pcsTarget)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	EnumCallback(AGPMSIEGEWAR_CB_RECEIVE_OPEN_ATTACK_OBJECT, pcsCharacter, pcsTarget);

	pcsTarget->m_Mutex.Release();
	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationResponseOpenAttackObject(INT32 lCID, INT32 lTargetCID, INT8 cResult, PVOID pvPacketAttackObjectInfo)
{
	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacterLock(lTargetCID);
	if (!pcsTarget)
	{
		return FALSE;
	}

	if (pvPacketAttackObjectInfo)
	{
		OnOperationUpdateAttackObject(lTargetCID, pvPacketAttackObjectInfo);
	}

	EnumCallback(AGPMSIEGEWAR_CB_RESPONSE_OPEN_ATTACK_OBJECT, &pcsTarget, &cResult);

	pcsTarget->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationRequestUseAttackObject(INT32 lCID, INT32 lTargetCID)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacterLock(lTargetCID);
	if (!pcsTarget)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	EnumCallback(AGPMSIEGEWAR_CB_RECEIVE_USE_ATTACK_OBJECT, pcsCharacter, pcsTarget);

	pcsTarget->m_Mutex.Release();
	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationResponseUseAttackObject(INT32 lCID, INT32 lTargetCID, INT8 cResult, PVOID pvPacketAttackObjectInfo)
{
	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacterLock(lTargetCID);
	if (!pcsTarget)
	{
		return FALSE;
	}

	if (pvPacketAttackObjectInfo)
	{
		OnOperationUpdateAttackObject(lTargetCID, pvPacketAttackObjectInfo);
	}

	EnumCallback(AGPMSIEGEWAR_CB_RESPONSE_USE_ATTACK_OBJECT, pcsTarget, &cResult);

	pcsTarget->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationRequestRepairAttackObject(INT32 lCID, INT32 lTargetCID)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacterLock(lTargetCID);
	if (!pcsTarget)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	EnumCallback(AGPMSIEGEWAR_CB_RECEIVE_REPAIR_ATTACK_OBJECT, pcsCharacter, pcsTarget);

	pcsTarget->m_Mutex.Release();
	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationResponseRepairAttackObject(INT32 lCID, INT32 lTargetCID, INT8 cResult)
{
	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacterLock(lTargetCID);
	if (!pcsTarget)
		return FALSE;

	EnumCallback(AGPMSIEGEWAR_CB_RESPONSE_REPAIR_ATTACK_OBJECT, pcsTarget, &cResult);

	pcsTarget->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationRemoveItemToAttackObject(INT32 lCID, INT32 lTargetCID, INT32 lItemID)
{
	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacter *pcsTarget = m_pcsAgpmCharacter->GetCharacterLock(lTargetCID);
	if (!pcsTarget)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	BOOL bResult = RemoveItemToAttackObject(pcsCharacter, pcsTarget, m_pcsAgpmItem->GetItem(lItemID));

	pcsTarget->m_Mutex.Release();
	pcsCharacter->m_Mutex.Release();

	return bResult;
}

BOOL AgpmSiegeWar::OnOperationAddItemToAttackObject(INT32 lCID, INT32 lTargetCID, INT32 lItemID)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacterLock(lTargetCID);
	if (!pcsTarget)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	BOOL	bResult	= AddItemToAttackObject(pcsCharacter, pcsTarget, m_pcsAgpmItem->GetItem(lItemID));

	pcsTarget->m_Mutex.Release();
	pcsCharacter->m_Mutex.Release();

	return bResult;
}

BOOL AgpmSiegeWar::OnOperationAddItemResult(INT32 lCID, INT32 lTargetCID, INT32 lItemID)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacterLock(lTargetCID);
	if (!pcsTarget)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsTarget);
	if (!pcsAttachData->m_pcsAttackObjectGrid)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	AgpdItem	*pcsItem	= m_pcsAgpmItem->GetItem(lItemID);
	if (!pcsItem)
	{
		pcsTarget->m_Mutex.Release();
		pcsCharacter->m_Mutex.Release();

		return FALSE;
	}

	m_pcsAgpmGrid->Add(pcsAttachData->m_pcsAttackObjectGrid, pcsItem->m_pcsGridItem, 1, 1);
	m_pcsAgpmGrid->AddItem(pcsAttachData->m_pcsAttackObjectGrid, pcsItem->m_pcsGridItem);


	PVOID	pvBuffer[3];
	pvBuffer[0]	= pcsCharacter;
	pvBuffer[1]	= pcsTarget;
	pvBuffer[2]	= pcsItem;

	EnumCallback(AGPMSIEGEWAR_CB_ADD_ITEM_RESULT, pvBuffer, NULL);

	pcsTarget->m_Mutex.Release();
	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationCarveASeal(INT32 lCID, INT32 lTargetCID)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacter	*pcsTarget		= m_pcsAgpmCharacter->GetCharacterLock(lTargetCID);
	if (!pcsTarget)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	EnumCallback(AGPMSIEGEWAR_CB_CARVE_A_SEAL, pcsCharacter, pcsTarget);

	pcsTarget->m_Mutex.Release();
	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmSiegeWar::OnOperationStatusInfo(PVOID pvPacketStatusInfo)
{
	if (!pvPacketStatusInfo)
		return FALSE;

	return EnumCallback(AGPMSIEGEWAR_CB_STATUS_INFO, pvPacketStatusInfo, NULL);
}

BOOL AgpmSiegeWar::OnOperationMessage(PVOID pvPacketSyncMessage)
{
	if (!pvPacketSyncMessage)
		return FALSE;

	return EnumCallback(AGPMSIEGEWAR_CB_SYNC_MESSAGE, pvPacketSyncMessage, NULL);
}

BOOL AgpmSiegeWar::RemoveItemToAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, AgpdItem *pcsItem)
{
	if (!pcsCharacter ||
		!pcsTarget ||
		!pcsItem)
		return FALSE;

	if (pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsTarget);
	if (!pcsAttachData->m_pcsAttackObjectGrid)
		return FALSE;

	if (!m_pcsAgpmGrid->RemoveItem(pcsAttachData->m_pcsAttackObjectGrid, pcsItem->m_pcsGridItem))
		return FALSE;

	return TRUE;

	//PVOID	pvBuffer[3];
	//pvBuffer[0]	= (PVOID) pcsCharacter;
	//pvBuffer[1]	= (PVOID) pcsTarget;
	//pvBuffer[2]	= (PVOID) pcsItem;

	//EnumCallback(AGPMSIEGEWAR_CB_REMOVE_ITEM_TO_ATTACK_OBJECT, pvBuffer, NULL);

	//return TRUE;
}

BOOL AgpmSiegeWar::AddItemToAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, AgpdItem *pcsItem)
{
	if (!pcsCharacter ||
		!pcsTarget ||
		!pcsItem)
		return FALSE;

	if (pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY)
		return FALSE;

	AgpdSiegeWarMonsterType	eMonsterType	= GetSiegeWarMonsterType(pcsTarget);
	if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE ||
		eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return FALSE;

	// 여기에 넣고 사용할 수 있는 아이템인지 본다.
	int i;
	for (i = 0; i < 8; ++i)
	{
		if (m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i] == pcsItem->m_pcsItemTemplate->m_lID)
			break;
	}

	if (i == 8)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsTarget);
	if (!pcsAttachData->m_pcsAttackObjectGrid)
		return FALSE;

	if (!m_pcsAgpmGrid->Add(pcsAttachData->m_pcsAttackObjectGrid, pcsItem->m_pcsGridItem, 1, 1))
		return FALSE;

	m_pcsAgpmItem->RemoveItemFromInventory(pcsCharacter, pcsItem);

	pcsItem->m_eStatus	= AGPDITEM_STATUS_SIEGEWAR_OBJECT;

	PVOID	pvBuffer[3];
	pvBuffer[0]	= (PVOID) pcsCharacter;
	pvBuffer[1]	= (PVOID) pcsTarget;
	pvBuffer[2]	= (PVOID) pcsItem;

	EnumCallback(AGPMSIEGEWAR_CB_ADD_ITEM_TO_ATTACK_OBJECT, pvBuffer, NULL);

	return TRUE;
}

AgpmSiegeResult AgpmSiegeWar::CheckOpenAttackObjectEvent(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget)
{
	if (!pcsCharacter || !pcsTarget)
		return AGPMSIEGE_RESULT_FAILED;

	if (m_pcsAgpmCharacter->IsActionBlockCondition(pcsCharacter))
		return AGPMSIEGE_RESULT_FAILED;

	// 투석기, 공성부활탑만 사용할 수 있다.
	AgpdSiegeWarMonsterType	eMonsterType	= GetSiegeWarMonsterType(pcsTarget);
	if (eMonsterType != AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER &&
		eMonsterType != AGPD_SIEGE_MONSTER_CATAPULT)
		return AGPMSIEGE_RESULT_FAILED;

	AgpdSiegeWar *pcsSiegeWar	= GetSiegeWarInfo(GetSiegeWarInfoIndex(pcsTarget->m_stPos));
	if (!pcsSiegeWar)
		return AGPMSIEGE_RESULT_FAILED;

	// 공성 중인지 본다.
	if (!IsStarted(pcsSiegeWar))
		return AGPMSIEGE_RESULT_FAILED;

	// 이미 사용중인지 본다.
	if (m_pcsAgpmCharacter->IsUseCharacter(pcsTarget))
	{
		// 사용하던 놈인지 본다.
		AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsTarget);

		if (strlen(pcsAttachData->m_strUseCharacterName.GetBuffer()) > 0 &&
			strcmp(pcsAttachData->m_strUseCharacterName.GetBuffer(), pcsCharacter->m_szID) != 0)
			return AGPMSIEGE_RESULT_ALREADY_USED_ATTACK_OBJECT;
	}
	if (m_pcsAgpmCharacter->IsRepairCharacter(pcsTarget))
	{
		// 수리하던 놈인지 본다.
		AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsTarget);

		if (strlen(pcsAttachData->m_strUseCharacterName.GetBuffer()) > 0 &&
			strcmp(pcsAttachData->m_strUseCharacterName.GetBuffer(), pcsCharacter->m_szID) != 0)
			return AGPMSIEGE_RESULT_ALREADY_REPAIRED_ATTACK_OBJECT;
	}

	// 공성측 길원인지 본다.
	if (!IsAttackGuild(pcsCharacter, pcsSiegeWar))
		return AGPMSIEGE_RESULT_FAILED;

	// 부활탑인 경우 길마인지 본다.
	if (eMonsterType == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER)
	{
		if (!m_pcsAgpmGuild->IsMaster(m_pcsAgpmGuild->GetGuild(pcsCharacter), pcsCharacter->m_szID))
			return AGPMSIEGE_RESULT_NEED_GUILD_MASTER;
	}

	return AGPMSIEGE_RESULT_OPEN_ATTACK_OBJECT_EVENT;
}

AgpmSiegeResult AgpmSiegeWar::CheckValidUseAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget)
{
	if (!pcsCharacter || !pcsTarget || !IsSiegeWarMonster(pcsTarget))
		return AGPMSIEGE_RESULT_FAILED;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsTarget);
	if (!pcsAttachData->m_pcsAttackObjectGrid)
		return AGPMSIEGE_RESULT_FAILED;

	if (m_pcsAgpmCharacter->IsUseCharacter(pcsTarget))
	{
		if (GetSiegeWarMonsterType(pcsTarget) == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER)
			return AGPMSIEGE_RESULT_ALREADY_ACTIVE_OBJECT;

		// 사용하던 놈인지 본다.
		if (strlen(pcsAttachData->m_strUseCharacterName.GetBuffer()) > 0 &&
			strcmp(pcsAttachData->m_strUseCharacterName.GetBuffer(), pcsCharacter->m_szID) != 0)
			return AGPMSIEGE_RESULT_ALREADY_USED_ATTACK_OBJECT;
	}

	if (m_pcsAgpmCharacter->IsRepairCharacter(pcsTarget))
		return AGPMSIEGE_RESULT_ALREADY_REPAIRED_ATTACK_OBJECT;

	// 공성측 길원인지 본다.
	AgpdSiegeWar *pcsSiegeWar	= GetSiegeWarInfo(GetSiegeWarInfoIndex(pcsTarget->m_stPos));
	if (!pcsSiegeWar)
		return AGPMSIEGE_RESULT_FAILED;

	if (!IsAttackGuild(pcsCharacter, pcsSiegeWar))
		return AGPMSIEGE_RESULT_FAILED;

	/*
	// 혹시 HP가 0인지 본다. 0이면 수리를 해야 한다.
	INT32	lHP	= m_pcsAgpmFactors->GetHP((AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
	if (lHP <= 0)
		return AGPMSIEGE_RESULT_FAILED;
	*/

	// 죽은놈은 살려야 한다. 수리가 필요한지 본다.
	if (pcsTarget->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return AGPMSIEGE_RESULT_FAILED;

	// 사용할 때 필요한 아이템이 올라가 있는지 본다.
	if (!CheckNeedItemForUse(pcsTarget))
		return AGPMSIEGE_RESULT_FAILED;

	return AGPMSIEGE_RESULT_SUCCESS;
}

AgpmSiegeResult AgpmSiegeWar::CheckValidRepairAttackObject(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget)
{
	if (!pcsCharacter || !pcsTarget)
		return AGPMSIEGE_RESULT_FAILED;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsTarget);

	if (m_pcsAgpmCharacter->IsUseCharacter(pcsTarget))
		return AGPMSIEGE_RESULT_FAILED;

	if (m_pcsAgpmCharacter->IsRepairCharacter(pcsTarget))
	{
		//if (strlen(pcsAttachData->m_strUseCharacterName.GetBuffer()) > 0 &&
		//	strcmp(pcsAttachData->m_strUseCharacterName.GetBuffer(), pcsCharacter->m_szID) != 0)
			return AGPMSIEGE_RESULT_ALREADY_REPAIRED_ATTACK_OBJECT;
	}

	// 공성측 길원인지 본다.
	AgpdSiegeWar *pcsSiegeWar	= GetSiegeWarInfo(GetSiegeWarInfoIndex(pcsTarget->m_stPos));
	if (!pcsSiegeWar)
		return AGPMSIEGE_RESULT_FAILED;

	if (!IsAttackGuild(pcsCharacter, pcsSiegeWar))
		return AGPMSIEGE_RESULT_FAILED;

	/*
	// 혹시 HP가 0보다 큰지 본다. 0보다 크면 수리할 수 없다.
	INT32	lHP	= m_pcsAgpmFactors->GetHP((AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
	if (lHP > 0)
		return AGPMSIEGE_RESULT_FAILED;
	*/

	// 죽은놈인지 본다. 살아있는 놈은 수리가 필요치안다.
	if (pcsTarget->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
		return AGPMSIEGE_RESULT_FAILED;

	// 수리비용이 충분하게 있나 본다.
	INT32	lRepairCost		= GetRepairCost(pcsTarget);

	if (pcsCharacter->m_llMoney < (INT64) lRepairCost)
		return AGPMSIEGE_RESULT_NOTENOUGH_MONEY;

	// 현재 남아있는 수리 횟수가 몇번인지 본다.
	if (pcsAttachData->m_ucRepairedCount >= pcsAttachData->ucRepairCount)
		return AGPMSIEGE_RESULT_NO_MORE_REPAIR;

	return AGPMSIEGE_RESULT_SUCCESS;
}

BOOL AgpmSiegeWar::CheckStartCondition()
{
	AuAutoLock	Lock(m_Mutex);
	if (!Lock.Result())
		return FALSE;

	if (m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX].m_csAttackApplicationGuild.GetObjectCount() < 1)
		return FALSE;

	INT32	lIndex	= 0;
	AgpdGuild	**ppcsAttackGuild	= (AgpdGuild **) m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX].m_csAttackApplicationGuild.GetObjectSequence(&lIndex);
	while (ppcsAttackGuild && *ppcsAttackGuild)
	{
		if (GetSiegeWarInfo(m_pcsAgpmCharacter->GetCharacter((*ppcsAttackGuild)->m_szMasterID)) == &m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX])
			return TRUE;

		ppcsAttackGuild	= (AgpdGuild **) m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX].m_csAttackApplicationGuild.GetObjectSequence(&lIndex);
	}

	return FALSE;
}

BOOL AgpmSiegeWar::CheckNeedItemForUse(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdSiegeWarMonsterType	eMonsterType	= GetSiegeWarMonsterType(pcsCharacter);
	if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE ||
		eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsCharacter);

	// 사용할 때 필요한 아이템이 올라가 있는지 본다.
	for (int i = 0; i < 8; ++i)
	{
		if (m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i] != AP_INVALID_IID)
		{
			if (m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemCountforActive[i] > 0)
			{
				INT32	lCount	= m_pcsAgpmGrid->GetCountByTemplate(pcsAttachData->m_pcsAttackObjectGrid, AGPDGRID_ITEM_TYPE_ITEM, m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i]);

				if (lCount < m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemCountforActive[i])
					return FALSE;
			}
			else
			{
				if (!m_pcsAgpmGrid->GetItemByTemplate(pcsAttachData->m_pcsAttackObjectGrid, AGPDGRID_ITEM_TYPE_ITEM, m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i]))
					return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL AgpmSiegeWar::PayCostForUse(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdSiegeWarMonsterType	eMonsterType	= GetSiegeWarMonsterType(pcsCharacter);
	if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE ||
		eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsCharacter);

	// 사용할 때 필요한 아이템이 올라가 있는지 본다.
	for (int i = 0; i < 8; ++i)
	{
		if (m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i] != AP_INVALID_IID)
		{
			if (m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemCountforActive[i] > 0)
			{
				INT32	lIndex	= 0;
				INT32	lCount	= m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemCountforActive[i];

				AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItemByTemplate(lIndex, pcsAttachData->m_pcsAttackObjectGrid, AGPDGRID_ITEM_TYPE_ITEM, m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i]);

				while (pcsGridItem)
				{
					AgpdItem		*pcsItem		= m_pcsAgpmItem->GetItem(pcsGridItem);
					if (!pcsItem)
						return FALSE;

					INT32	lItemCount	= pcsItem->m_nCount;
					if (lItemCount <= 0)
						lItemCount	= 1;

					if (lItemCount <= lCount)
					{
						lCount -= lItemCount;
						m_pcsAgpmGrid->RemoveItem(pcsAttachData->m_pcsAttackObjectGrid, pcsGridItem);
						// cause server crash, init character pointer
						pcsItem->m_pcsCharacter = NULL;
						m_pcsAgpmItem->RemoveItem(pcsItem, TRUE);
					}
					else
					{
						m_pcsAgpmItem->SubItemStackCount(pcsItem, lCount);
						lCount	= 0;
					}

					if (lCount <= 0)
						break;

					pcsGridItem	= m_pcsAgpmGrid->GetItemByTemplate(lIndex, pcsAttachData->m_pcsAttackObjectGrid, AGPDGRID_ITEM_TYPE_ITEM, m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i]);
				}
			}
			else
			{
				AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItemByTemplate(pcsAttachData->m_pcsAttackObjectGrid, AGPDGRID_ITEM_TYPE_ITEM, m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i]);
				AgpdItem		*pcsItem		= m_pcsAgpmItem->GetItem(pcsGridItem);

				if (!pcsItem)
					return FALSE;

				m_pcsAgpmGrid->RemoveItem(pcsAttachData->m_pcsAttackObjectGrid, pcsGridItem);
				m_pcsAgpmItem->RemoveItem(pcsItem, TRUE);
			}
		}
	}

	return TRUE;
}

AgpmSiegeResult AgpmSiegeWar::SetNextSiegeWarTime(AgpdSiegeWar *pcsSiegeWar, UINT64 ullNextSiegeWarTimeDate)
{
	if (pcsSiegeWar->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_OFF)
		return AGPMSIEGE_RESULT_FAILED;

	if (!IsValidSiegeWarTime(pcsSiegeWar, ullNextSiegeWarTimeDate))
		return AGPMSIEGE_RESULT_INVALID_TIME;

	pcsSiegeWar->m_ullNextSiegeWarTimeDate	= ullNextSiegeWarTimeDate;
	pcsSiegeWar->m_ullApplicationTimeDate	= AuTimeStamp2::GetCurrentTimeStamp() + (UINT64) pcsSiegeWar->m_csTemplate.m_ulApplicationTimeHour * 60 * 60 * 10000000;

	return AGPMSIEGE_RESULT_SET_NEXT_SIEGE_WAR_TIME_SUCCESS;
}

BOOL AgpmSiegeWar::IsValidSiegeWarTime(AgpdSiegeWar *pcsSiegeWar, UINT64 ullNextSiegeWarTimeDate)
{
	if (!pcsSiegeWar || ullNextSiegeWarTimeDate == 0)
		return FALSE;

	if (pcsSiegeWar->m_ullPrevSiegeWarTimeDate == 0)
		return TRUE;

	UINT64	ullBaseSiegeWarTimeDate	= pcsSiegeWar->m_ullPrevSiegeWarTimeDate + 60 * 60 * 24 * (UINT64) pcsSiegeWar->m_csTemplate.m_ulSiegeWarIntervalDay * 10000000;

	FILETIME	file_time;
	SYSTEMTIME	system_time;

	CopyMemory(&file_time, &ullBaseSiegeWarTimeDate, sizeof(UINT64));
	if (::FileTimeToSystemTime(&file_time, &system_time) == 0)
		return FALSE;

	if (system_time.wDayOfWeek == 0)	// sunday
	{
		ullBaseSiegeWarTimeDate	-= (UINT64) 60 * 60 * 24 * 10000000;
	}

	ullBaseSiegeWarTimeDate	-= ((system_time.wHour - (UINT64) (12 + AGPMSIEGEWAR_START_WAR_TIME))) * 60 * 60 * 10000000;

	if (ullNextSiegeWarTimeDate < ullBaseSiegeWarTimeDate ||
		ullNextSiegeWarTimeDate > ullBaseSiegeWarTimeDate + (UINT64) 60 * 60 * 26 * 10000000)
		return FALSE;

	return TRUE;
}

PVOID AgpmSiegeWar::MakePacketCastleInfo(AgpdSiegeWar *pcsSiegeWar, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	PVOID	pvPacketCastleInfo	= MakePacketCastleInfoPacket(pcsSiegeWar);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_UPDATE;

	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
												&cOperation,
												NULL,
												pvPacketCastleInfo,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return	pvPacket;
}

PVOID AgpmSiegeWar::MakePacketCastleInfoPacket(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return NULL;

	AgpdGuild		*pcsGuild		= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strOwnerGuildName.GetBuffer());
	AgpdGuildMember	*pcsGuildMember	= NULL;

	if (pcsGuild)
		pcsGuildMember	= m_pcsAgpmGuild->GetMaster(pcsGuild);

	UINT16	unCastleNameLength		= (UINT16) pcsSiegeWar->m_strCastleName.GetLength();
	UINT16	unOwnerGuildNameLength	= (UINT16) pcsSiegeWar->m_strOwnerGuildName.GetLength();
	UINT16	unCarveGuildNameLength	= (UINT16) pcsSiegeWar->m_strCarveGuildName.GetLength();

	UINT16	unGuildMasterNameLength	= 0;
	UINT8	ucGuildMemberCount		= 0;
	if (pcsGuild && pcsGuildMember)
	{
		unGuildMasterNameLength	= (UINT16) strlen(pcsGuildMember->m_szID);
		ucGuildMemberCount		= (UINT8) m_pcsAgpmGuild->GetMemberCount(pcsGuild);
	}
	UINT8	ucSiegeWarStatus		= (UINT8) pcsSiegeWar->m_eCurrentStatus;

	if (pcsGuild)
	{
		return	m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
												pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
												((pcsGuildMember) ? pcsGuildMember->m_szID : _T("")), &unGuildMasterNameLength,
												pcsSiegeWar->m_strOwnerGuildName.GetBuffer(), &unOwnerGuildNameLength,
												&ucGuildMemberCount,
												&pcsSiegeWar->m_ullPrevSiegeWarTimeDate,
												&pcsSiegeWar->m_ullNextSiegeWarTimeDate,
												&ucSiegeWarStatus,
												&pcsSiegeWar->m_bIsActiveAttackResurrection,
												&pcsSiegeWar->m_bIsActiveDefenseResurrection,
												&pcsSiegeWar->m_bIsReverseAttack,
												pcsSiegeWar->m_strCarveGuildName.GetBuffer(), &unCarveGuildNameLength);
	}
	else
	{
		return	m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
												pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
												NULL,
												pcsSiegeWar->m_strOwnerGuildName.GetBuffer(), &unOwnerGuildNameLength,
												&ucGuildMemberCount,
												&pcsSiegeWar->m_ullPrevSiegeWarTimeDate,
												&pcsSiegeWar->m_ullNextSiegeWarTimeDate,
												&ucSiegeWarStatus,
												&pcsSiegeWar->m_bIsActiveAttackResurrection,
												&pcsSiegeWar->m_bIsActiveDefenseResurrection,
												&pcsSiegeWar->m_bIsReverseAttack,
												pcsSiegeWar->m_strCarveGuildName.GetBuffer(), &unCarveGuildNameLength);
	}

	return NULL;
}

PVOID AgpmSiegeWar::MakePacketCarveGuildInfo(AgpdSiegeWar *pcsSiegeWar, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength		= (UINT16) pcsSiegeWar->m_strCastleName.GetLength();
	UINT16	unCarveGuildNameLength	= (UINT16) pcsSiegeWar->m_strCarveGuildName.GetLength();

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											pcsSiegeWar->m_strCarveGuildName.GetBuffer(), &unCarveGuildNameLength);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_UPDATE;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return pvPacket;
}

PVOID AgpmSiegeWar::MakePacketSetNextSiegeWarTime(AgpdSiegeWar *pcsSiegeWar, UINT64 ullNextSetTime, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											&ullNextSetTime,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_SET_NEXT_SIEGEWAR_TIME;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return pvPacket;
}

PVOID AgpmSiegeWar::MakePacketResult(AgpdSiegeWar *pcsSiegeWar, AgpmSiegeResult eResult, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= (INT8)	AGPMSIEGE_OPERATION_RESULT;
	INT8	cResult		= (INT8)	eResult;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											&cResult,
											pvPacketCastleInfo,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return pvPacket;
}

PVOID AgpmSiegeWar::MakePacketSiegeWarStatus(AgpdSiegeWar *pcsSiegeWar, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength		= (UINT16) strlen(pcsSiegeWar->m_strCastleName.GetBuffer());
	UINT8	ucSiegeWarStatus		= (UINT8) pcsSiegeWar->m_eCurrentStatus;

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											&pcsSiegeWar->m_ullPrevSiegeWarTimeDate,
											&pcsSiegeWar->m_ullNextSiegeWarTimeDate,
											&ucSiegeWarStatus,
											NULL,
											NULL,
											NULL,
											NULL);

	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= (INT8)	AGPMSIEGE_OPERATION_UPDATE;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return pvPacket;
}


PVOID AgpmSiegeWar::MakePacketDefenseApplication(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_DEFENSE_APPLICATION;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return pvPacket;
}

PVOID AgpmSiegeWar::MakePacketCancelDefenseApplication(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_CANCEL_DEFENSE_APPLICATION;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return pvPacket;
}

PVOID AgpmSiegeWar::MakePacketAttackApplication(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_ATTACK_APPLICATION;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return pvPacket;
}

PVOID AgpmSiegeWar::MakePacketDefenseApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 nPage, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	INT16	nTotalPage	= (pcsSiegeWar->m_csDefenseApplicationGuild.GetObjectCount() / AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE) + 1;

	if (nPage < 1 || nPage > nTotalPage)
		return NULL;

	INT32	lIndex	= 1;

	AuAutoLock Lock(pcsSiegeWar->m_ApDefenseGuildSortList.m_Mutex);
	if (!Lock.Result()) return NULL;

	AgpdGuild	*pcsGuild	= pcsSiegeWar->m_ApDefenseGuildSortList.GetHead();

	while (pcsGuild && !pcsSiegeWar->m_ApDefenseGuildSortList.IsEnd())
	{
		if (lIndex > (nPage - 1) * AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE)
			break;

		pcsGuild	= pcsSiegeWar->m_ApDefenseGuildSortList.GetNext();

		++lIndex;
	}

	if (pcsSiegeWar->m_ApDefenseGuildSortList.IsEnd())
		return NULL;

	PVOID	pvPacketGuildInfo[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];
	ZeroMemory(pvPacketGuildInfo, sizeof(pvPacketGuildInfo));

	for (int i = 0; i < AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE; ++i)
	{
		pvPacketGuildInfo[i]	= MakePacketGuildInfo(pcsGuild);

		pcsGuild	= pcsSiegeWar->m_ApDefenseGuildSortList.GetNext();
		if (!pcsGuild || pcsSiegeWar->m_ApDefenseGuildSortList.IsEnd())
			break;
	}

	PVOID	pvPacketGuildList	= m_csPacketGuildList.MakePacket(FALSE, NULL, 0,
		&nPage,
		&nTotalPage,
		pvPacketGuildInfo);

	if (!pvPacketGuildList)
		return NULL;

	INT8	cOperation			= AGPMSIEGE_OPERATION_DEFENSE_APPL_GUILD_LIST;
	PVOID	pvPacket			= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
		&cOperation,
		NULL,
		NULL,
		NULL,
		pvPacketGuildList,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	m_csPacketGuildList.FreePacket(pvPacketGuildList);

	return	pvPacket;
}

PVOID AgpmSiegeWar::MakePacketCancelAttackApplication(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_CANCEL_ATTACK_APPLICATION;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return pvPacket;
}

PVOID AgpmSiegeWar::MakePacketRequestAttackApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 nPage, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	PVOID	pvPacketGuildList	= m_csPacketGuildList.MakePacket(FALSE, NULL, 0,
											&nPage,
											NULL,
											NULL);
	if (!pvPacketGuildList)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation			= AGPMSIEGE_OPERATION_REQUEST_ATTACK_APPL_GUILD_LIST;
	PVOID	pvPacket			= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											&lCID,
											pvPacketGuildList,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketGuildList.FreePacket(pvPacketGuildList);
	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return	pvPacket;
}

PVOID AgpmSiegeWar::MakePacketRequestDefenseApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 nPage, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	PVOID	pvPacketGuildList	= m_csPacketGuildList.MakePacket(FALSE, NULL, 0,
											&nPage,
											NULL,
											NULL);
	if (!pvPacketGuildList)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation			= AGPMSIEGE_OPERATION_REQUEST_DEFENSE_APPL_GUILD_LIST;
	PVOID	pvPacket			= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											&lCID,
											pvPacketGuildList,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketGuildList.FreePacket(pvPacketGuildList);
	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return	pvPacket;
}

PVOID AgpmSiegeWar::MakePacketAttackApplGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 nPage, INT16 *pnPacketLength)
{
	if (pcsSiegeWar->m_eCurrentStatus <= AGPD_SIEGE_WAR_STATUS_PROCLAIM_WAR)
	{
		INT16	nTotalPage	= (pcsSiegeWar->m_csAttackApplicationGuild.GetObjectCount() / AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE) + 1;

		if (nPage < 1 || nPage > nTotalPage)
			return NULL;

		INT32	lIndex	= 1;

		AuAutoLock Lock(pcsSiegeWar->m_ApAttackGuildSortList.m_Mutex);
		if (!Lock.Result()) return NULL;

		AgpdGuild	*pcsGuild	= pcsSiegeWar->m_ApAttackGuildSortList.GetHead();

		while (pcsGuild && !pcsSiegeWar->m_ApAttackGuildSortList.IsEnd())
		{
			if (lIndex > (nPage - 1) * AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE)
				break;

			pcsGuild	= pcsSiegeWar->m_ApAttackGuildSortList.GetNext();

			++lIndex;
		}

		PVOID	pvPacketGuildInfo[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];
		ZeroMemory(pvPacketGuildInfo, sizeof(pvPacketGuildInfo));

		if (!pcsSiegeWar->m_ApAttackGuildSortList.IsEnd() &&
			pcsGuild)
		{
			for (int i = 0; i < AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE; ++i)
			{
				pvPacketGuildInfo[i]	= MakePacketGuildInfo(pcsGuild);

				pcsGuild	= pcsSiegeWar->m_ApAttackGuildSortList.GetNext();
				if (!pcsGuild || pcsSiegeWar->m_ApAttackGuildSortList.IsEnd())
					break;
			}
		}

		PVOID	pvPacketGuildList	= m_csPacketGuildList.MakePacket(FALSE, NULL, 0,
			&nPage,
			&nTotalPage,
			pvPacketGuildInfo);

		if (!pvPacketGuildList)
			return NULL;

		INT8	cOperation			= AGPMSIEGE_OPERATION_ATTACK_APPL_GUILD_LIST;
		PVOID	pvPacket			= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
			&cOperation,
			NULL,
			NULL,
			NULL,
			pvPacketGuildList,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);

		m_csPacketGuildList.FreePacket(pvPacketGuildList);

		return pvPacket;
	}
	else
	{
		INT16	nTotalPage	= (pcsSiegeWar->m_csAttackGuild.GetObjectCount() / AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE) + 1;

		if (nPage < 1 || nPage > nTotalPage)
			return NULL;

		INT32	lIndex	= 0;
		INT32	lPacketIndex = 0;

		AuAutoLock Lock(pcsSiegeWar->m_csAttackGuild.m_Mutex);
		if (!Lock.Result()) return NULL;

		AgpdGuild **ppcsGuild = NULL;


		PVOID	pvPacketGuildInfo[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];
		ZeroMemory(pvPacketGuildInfo, sizeof(pvPacketGuildInfo));

		if(nPage > 1)
		{
			INT32 nCount = 0;

			for (ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lIndex); ppcsGuild && *ppcsGuild; ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lIndex))
			{
				if (nCount + 1 >= (nPage - 1) * AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE)
					break;

				nCount++;
			}

			// 그 다음 인덱스부터 다시 패킷 제조

			for (ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lIndex); ppcsGuild && *ppcsGuild; ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lIndex))
			{
				if (lPacketIndex >= AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE)
					break;

				pvPacketGuildInfo[lPacketIndex++]	= MakePacketGuildInfo(*ppcsGuild);
			}
		}
		else
		{
			for (ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lIndex); ppcsGuild && *ppcsGuild; ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lIndex))
			{
				if (lPacketIndex >= AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE)
					break;

				pvPacketGuildInfo[lPacketIndex++]	= MakePacketGuildInfo(*ppcsGuild);
			}
		}

		PVOID	pvPacketGuildList	= m_csPacketGuildList.MakePacket(FALSE, NULL, 0,
			&nPage,
			&nTotalPage,
			pvPacketGuildInfo);

		if (!pvPacketGuildList)
			return NULL;

		INT8	cOperation			= AGPMSIEGE_OPERATION_ATTACK_APPL_GUILD_LIST;
		PVOID	pvPacket			= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
			&cOperation,
			NULL,
			NULL,
			NULL,
			pvPacketGuildList,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);

		m_csPacketGuildList.FreePacket(pvPacketGuildList);

		return pvPacket;	
	}

	return	NULL;
}

PVOID AgpmSiegeWar::MakePacketRequestAttackGuildList(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar ||
		lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation			= AGPMSIEGE_OPERATION_REQUEST_ATTACK_GUILD_LIST;
	PVOID	pvPacket			= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return	pvPacket;
}

PVOID AgpmSiegeWar::MakePacketRequestDefenseGuildList(AgpdSiegeWar *pcsSiegeWar, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar ||
		lCID == AP_INVALID_CID ||
		!pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation			= AGPMSIEGE_OPERATION_REQUEST_DEFENSE_GUILD_LIST;
	PVOID	pvPacket			= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return	pvPacket;
}

PVOID AgpmSiegeWar::MakePacketAttackGuildList(AgpdSiegeWar *pcsSiegeWar,  INT16 nPage, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	PVOID	pvPacketGuildInfo[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];
	ZeroMemory(pvPacketGuildInfo, sizeof(pvPacketGuildInfo));

	INT16	nTotalPage	= (pcsSiegeWar->m_csAttackGuild.GetObjectCount() / AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE) + 1;

	if (nPage < 0 || nPage > nTotalPage)
		return NULL;

	INT32	lIndex			= 0;
	INT32	lPacketIndex	= 0;
	
	AuAutoLock Lock(pcsSiegeWar->m_csAttackGuild.m_Mutex);
	if (!Lock.Result()) return NULL;

	INT32 nCount = 0;

	AgpdGuild **ppcsGuild	= NULL;

	if(nPage > 0)
	{
		for (ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lIndex); ppcsGuild && *ppcsGuild; ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lIndex))
		{
			if (nCount + 1 >= (nPage - 1) * AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE)
				break;

			nCount++;
		}
	}

	// 그 다음 인덱스부터 다시 패킷 제조

	for (ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lIndex); ppcsGuild && *ppcsGuild; ppcsGuild = (AgpdGuild **) pcsSiegeWar->m_csAttackGuild.GetObjectSequence(&lIndex))
	{
		if (lPacketIndex >= AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE)
			break;

		pvPacketGuildInfo[lPacketIndex++]	= MakePacketGuildInfo(*ppcsGuild);
	}

	PVOID	pvPacketGuildList	= m_csPacketGuildList.MakePacket(FALSE, NULL, 0,
											&nPage,
											&nTotalPage,
											pvPacketGuildInfo);

	if (!pvPacketGuildList)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	INT8	cOperation			= AGPMSIEGE_OPERATION_ATTACK_GUILD_LIST;
	PVOID	pvPacket			= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											NULL,
											pvPacketGuildList,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketGuildList.FreePacket(pvPacketGuildList);

	return	pvPacket;
}

PVOID AgpmSiegeWar::MakePacketDefenseGuildList(AgpdSiegeWar *pcsSiegeWar, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	PVOID	pvPacketGuildInfo[AGPMSIEGEWAR_MAX_GUILD_LIST_PER_PAGE];
	ZeroMemory(pvPacketGuildInfo, sizeof(pvPacketGuildInfo));

	int	i = 0;

	if (pcsSiegeWar->m_strOwnerGuildName.GetLength() > 0)
	{
		AgpdGuild	*pcsOwnerGuild	= m_pcsAgpmGuild->GetGuild(pcsSiegeWar->m_strOwnerGuildName.GetBuffer());
		if (pcsOwnerGuild)
			pvPacketGuildInfo[i++]	= MakePacketGuildInfo(pcsOwnerGuild);
	}

	INT32	lIndex	= 0;
	for ( ; i < AGPMSIEGEWAR_TOTAL_DEFENSE_GUILD; ++i)
	{
		AgpdGuild	**ppcsGuild	= (AgpdGuild **) pcsSiegeWar->m_csDefenseGuild.GetObjectSequence(&lIndex);
		if (ppcsGuild && *ppcsGuild)
		{
			pvPacketGuildInfo[i]	= MakePacketGuildInfo(*ppcsGuild);
		}
		else
			break;
	}

	PVOID	pvPacketGuildList	= m_csPacketGuildList.MakePacket(FALSE, NULL, 0,
											NULL,
											NULL,
											pvPacketGuildInfo);

	if (!pvPacketGuildList)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16)strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	INT8	cOperation			= AGPMSIEGE_OPERATION_DEFENSE_GUILD_LIST;
	PVOID	pvPacket			= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											NULL,
											pvPacketGuildList,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketGuildList.FreePacket(pvPacketGuildList);

	return	pvPacket;
}

PVOID AgpmSiegeWar::MakePacketGuildInfo(AgpdGuild *pcsGuild)
{
	if (!pcsGuild)
		return NULL;

	UINT16	unGuildNameLength	= (UINT16)strlen(pcsGuild->m_szID);
	UINT16	unGuildMasterLength	= (UINT16)strlen(pcsGuild->m_szMasterID);
	UINT32	ulGuildPoint		= GetGuildTotalPoint(pcsGuild);

	return	m_csPacketGuildInfo.MakePacket(FALSE, NULL, 0,
										   pcsGuild->m_szID, &unGuildNameLength,
										   pcsGuild->m_szMasterID, &unGuildMasterLength,
										   &ulGuildPoint);
}

PVOID AgpmSiegeWar::MakePacketSelectDefenseGuild(AgpdSiegeWar *pcsSiegeWar, CHAR *szGuildName, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !szGuildName || !szGuildName[0] || !pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16) strlen(pcsSiegeWar->m_strCastleName.GetBuffer());
	UINT16	unGuildNameLength	= (UINT16) strlen(szGuildName);

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											szGuildName, &unGuildNameLength,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_SELECT_DEFENSE_GUILD;

	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											pvPacketCastleInfo,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return	pvPacket;
}

PVOID AgpmSiegeWar::MakePacketRequestOpenAttackObject(INT32 lCID, INT32 lTargetCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength ||
		lCID == AP_INVALID_CID ||
		lTargetCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_REQUEST_OPEN_ATTACK_OBJECT;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											&lCID,
											NULL,
											&lTargetCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
}

PVOID AgpmSiegeWar::MakePacketRequestUseAttackObject(INT32 lCID, INT32 lTargetCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength ||
		lCID == AP_INVALID_CID ||
		lTargetCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_REQUEST_USE_ATTACK_OBJECT;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											&lCID,
											NULL,
											&lTargetCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
}

PVOID AgpmSiegeWar::MakePacketResponseUseAttackObject(INT32 lCID, INT32 lTargetCID, AgpmSiegeResult eResult, UINT8 ucRepairCount, UINT8 ucRepairedCount, INT32 lRepairCost, UINT8 ucRepairDuration, UINT8 ucRepairElapsed, CHAR *pszUserName, AgpdGrid *pcsGrid, INT16 *pnPacketLength)
{
	if (!pnPacketLength ||
		lCID == AP_INVALID_CID ||
		lTargetCID == AP_INVALID_CID)
		return NULL;

	INT32	alGridItemID[8];
	ZeroMemory(alGridItemID, sizeof(alGridItemID));

	if (pcsGrid)
	{
		AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
		if (pcsGridItem) alGridItemID[0]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 1);
		if (pcsGridItem) alGridItemID[1]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 2);
		if (pcsGridItem) alGridItemID[2]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 3);
		if (pcsGridItem) alGridItemID[3]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsGrid, 0, 1, 0);
		if (pcsGridItem) alGridItemID[4]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsGrid, 0, 1, 1);
		if (pcsGridItem) alGridItemID[5]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsGrid, 0, 1, 2);
		if (pcsGridItem) alGridItemID[6]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsGrid, 0, 1, 3);
		if (pcsGridItem) alGridItemID[7]	= pcsGridItem->m_lItemID;
	}

	UINT16	unUserNameLength	= 0;
	if (pszUserName)
		unUserNameLength	= (UINT16) strlen(pszUserName);

	PVOID	pvPacketAttackObjectInfo	= m_csPacketAttackObjectInfo.MakePacket(FALSE, NULL, 0,
											(ucRepairCount > 0) ? &ucRepairCount : NULL,
											(lRepairCost > 0) ? &lRepairCost : NULL,
											(ucRepairDuration > 0) ? &ucRepairDuration : NULL,
											&ucRepairedCount,
											(alGridItemID[0] != AP_INVALID_IID) ? &alGridItemID[0] : NULL,
											(alGridItemID[1] != AP_INVALID_IID) ? &alGridItemID[1] : NULL,
											(alGridItemID[2] != AP_INVALID_IID) ? &alGridItemID[2] : NULL,
											(alGridItemID[3] != AP_INVALID_IID) ? &alGridItemID[3] : NULL,
											(alGridItemID[4] != AP_INVALID_IID) ? &alGridItemID[4] : NULL,
											(alGridItemID[5] != AP_INVALID_IID) ? &alGridItemID[5] : NULL,
											(alGridItemID[6] != AP_INVALID_IID) ? &alGridItemID[6] : NULL,
											(alGridItemID[7] != AP_INVALID_IID) ? &alGridItemID[7] : NULL,
											(ucRepairElapsed > 0) ? &ucRepairElapsed : NULL,
											pszUserName, &unUserNameLength);

	if (!pvPacketAttackObjectInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_RESPONSE_USE_ATTACK_OBJECT;
	INT8	cResult		= (INT8)	eResult;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											&cResult,
											NULL,
											&lCID,
											NULL,
											&lTargetCID,
											pvPacketAttackObjectInfo,
											NULL,
											NULL,
											NULL,
											NULL);
}

PVOID AgpmSiegeWar::MakePacketRequestRepairAttackObject(INT32 lCID, INT32 lTargetCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength ||
		lCID == AP_INVALID_CID ||
		lTargetCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_REQUEST_REPAIR_ATTACK_OBJECT;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											&lCID,
											NULL,
											&lTargetCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
}

PVOID AgpmSiegeWar::MakePacketAddItemToAttackObject(INT32 lCID, INT32 lTargetCID, INT32 lItemID, INT16 *pnPacketLength)
{
	if (!pnPacketLength ||
		lCID == AP_INVALID_CID ||
		lTargetCID == AP_INVALID_CID || 
		lItemID == AP_INVALID_IID)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_ADD_ITEM_TO_ATTACK_OBJECT;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											&lCID,
											NULL,
											&lTargetCID,
											NULL,
											&lItemID,
											NULL,
											NULL,
											NULL);
}

PVOID AgpmSiegeWar::MakePacketRemoveItemToAttackObject(INT32 lCID, INT32 lTargetCID, INT32 lItemID, INT16 *pnPacketLength)
{
	if (!pnPacketLength ||
		lCID == AP_INVALID_CID ||
		lTargetCID == AP_INVALID_CID || 
		lItemID == AP_INVALID_IID)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_REMOVE_ITEM_TO_ATTACK_OBJECT;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											&lCID,
											NULL,
											&lTargetCID,
											NULL,
											&lItemID,
											NULL,
											NULL,
											NULL);
}

PVOID AgpmSiegeWar::MakePacketAddItemResult(INT32 lCID, INT32 lTargetCID, INT32 lItemID, INT16 *pnPacketLength)
{
	if (!pnPacketLength ||
		lCID == AP_INVALID_CID ||
		lTargetCID == AP_INVALID_CID || 
		lItemID == AP_INVALID_IID)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_ADD_ITEM_RESULT;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											&lCID,
											NULL,
											&lTargetCID,
											NULL,
											&lItemID,
											NULL,
											NULL,
											NULL);
}

PVOID AgpmSiegeWar::MakePacketCarveASeal(INT32 lCID, INT32 lTargetCID, INT16 *pnPacketLength)
{
	if (!pnPacketLength ||
		lCID == AP_INVALID_CID ||
		lTargetCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_CARVE_A_SEAL;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											&lCID,
											NULL,
											&lTargetCID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL);
}

PVOID AgpmSiegeWar::MakePacketStatusInfo(AgpmSiegeStatusType eStatusType, AgpdSiegeWar *pcsSiegeWar, CHAR *pszGuildName, CHAR *pszGuildMasterName, AgpdSiegeWarMonsterType eMonsterType, UINT32 ulTimeSec, UINT8 ucActiveCount, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	UINT8	ucStatusType		= (UINT8)	eStatusType;
	UINT16	unCastleNameLength	= (UINT16)	strlen(pcsSiegeWar->m_strCastleName.GetBuffer());
	UINT8	ucObjectType		= (UINT8)	eMonsterType;

	PVOID	pvPacketStatusInfo	= NULL;

	if (pszGuildName && strlen(pszGuildName) > 0)
	{
		UINT16	unGuildNameLength		= (UINT16) strlen(pszGuildName);
		UINT16	unGuildMasterNameLength	= (UINT16) strlen(pszGuildMasterName);

		pvPacketStatusInfo	= m_csPacketStatusInfo.MakePacket(FALSE, NULL, 0,
											&ucStatusType,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											pszGuildName, &unGuildNameLength,
											pszGuildMasterName, &unGuildMasterNameLength,
											(ucObjectType > AGPD_SIEGE_MONSTER_TYPE_NONE) ? &ucObjectType : NULL,
											(ulTimeSec > 0) ? &ulTimeSec : NULL,
											(ucActiveCount > 0) ? &ucActiveCount : NULL);
	}
	else
	{
		pvPacketStatusInfo	= m_csPacketStatusInfo.MakePacket(FALSE, NULL, 0,
											&ucStatusType,
											pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
											NULL,
											NULL,
											(ucObjectType > AGPD_SIEGE_MONSTER_TYPE_NONE) ? &ucObjectType : NULL,
											(ulTimeSec > 0) ? &ulTimeSec : NULL,
											(ucActiveCount > 0) ? &ucActiveCount : NULL);
	}

	if (!pvPacketStatusInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_STATUS_INFO;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL,
											pvPacketStatusInfo,
											NULL,
											NULL);
}

PVOID AgpmSiegeWar::MakePacketUpdateAttackObject(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, BOOL bSendGridInfo)
{
	if (!pnPacketLength ||
		!pcsCharacter)
		return NULL;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsCharacter);

	INT32	alGridItemID[8];
	ZeroMemory(alGridItemID, sizeof(alGridItemID));

	if (bSendGridInfo && pcsAttachData->m_pcsAttackObjectGrid)
	{
		AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsAttachData->m_pcsAttackObjectGrid, 0, 0, 0);
		if (pcsGridItem) alGridItemID[0]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsAttachData->m_pcsAttackObjectGrid, 0, 0, 1);
		if (pcsGridItem) alGridItemID[1]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsAttachData->m_pcsAttackObjectGrid, 0, 0, 2);
		if (pcsGridItem) alGridItemID[2]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsAttachData->m_pcsAttackObjectGrid, 0, 0, 3);
		if (pcsGridItem) alGridItemID[3]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsAttachData->m_pcsAttackObjectGrid, 0, 1, 0);
		if (pcsGridItem) alGridItemID[4]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsAttachData->m_pcsAttackObjectGrid, 0, 1, 1);
		if (pcsGridItem) alGridItemID[5]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsAttachData->m_pcsAttackObjectGrid, 0, 1, 2);
		if (pcsGridItem) alGridItemID[6]	= pcsGridItem->m_lItemID;

		pcsGridItem	= m_pcsAgpmGrid->GetItem(pcsAttachData->m_pcsAttackObjectGrid, 0, 1, 3);
		if (pcsGridItem) alGridItemID[7]	= pcsGridItem->m_lItemID;
	}

	UINT16	unUserNameLength	= 0;

	if (m_pcsAgpmCharacter->IsUseCharacter(pcsCharacter) ||
		m_pcsAgpmCharacter->IsRepairCharacter(pcsCharacter))
		unUserNameLength	= (UINT16) strlen(pcsAttachData->m_strUseCharacterName.GetBuffer());

	UINT8	ucRepairElapsedSec	= (UINT8) ((GetClockCount() - pcsAttachData->m_ulStartRepairTimeMSec) / 1000);
	pcsAttachData->ucRepairDuration	= GetRepairDuration(pcsCharacter);

	PVOID	pvPacketAttackObjectInfo	= m_csPacketAttackObjectInfo.MakePacket(FALSE, NULL, 0,
		(pcsAttachData->ucRepairCount > 0) ? &pcsAttachData->ucRepairCount : NULL,
		(pcsAttachData->lRepairCost > 0) ? &pcsAttachData->lRepairCost : NULL,
		(pcsAttachData->ucRepairDuration > 0) ? &pcsAttachData->ucRepairDuration : NULL,
		&pcsAttachData->m_ucRepairedCount,
		(alGridItemID[0] != AP_INVALID_IID) ? &alGridItemID[0] : NULL,
		(alGridItemID[1] != AP_INVALID_IID) ? &alGridItemID[1] : NULL,
		(alGridItemID[2] != AP_INVALID_IID) ? &alGridItemID[2] : NULL,
		(alGridItemID[3] != AP_INVALID_IID) ? &alGridItemID[3] : NULL,
		(alGridItemID[4] != AP_INVALID_IID) ? &alGridItemID[4] : NULL,
		(alGridItemID[5] != AP_INVALID_IID) ? &alGridItemID[5] : NULL,
		(alGridItemID[6] != AP_INVALID_IID) ? &alGridItemID[6] : NULL,
		(alGridItemID[7] != AP_INVALID_IID) ? &alGridItemID[7] : NULL,
		(ucRepairElapsedSec > 0) ? &ucRepairElapsedSec : NULL,
		pcsAttachData->m_strUseCharacterName.GetBuffer(), &unUserNameLength);

	if (!pvPacketAttackObjectInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_UPDATE_ATTACK_OBJECT;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
		&cOperation,
		NULL,
		NULL,
		&pcsCharacter->m_lID,
		NULL,
		NULL,
		pvPacketAttackObjectInfo,
		NULL,
		NULL,
		NULL,
		NULL);
}

PVOID AgpmSiegeWar::MakePacketUpdateReverseAttack(AgpdSiegeWar *pcsSiegeWar, INT16 *pnPacketLength)
{
	if (!pcsSiegeWar || !pnPacketLength)
		return NULL;

	UINT16	unCastleNameLength	= (UINT16) strlen(pcsSiegeWar->m_strCastleName.GetBuffer());

	PVOID	pvPacketCastleInfo	= m_csPacketCastleInfo.MakePacket(FALSE, NULL, 0,
		pcsSiegeWar->m_strCastleName.GetBuffer(), &unCastleNameLength,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&pcsSiegeWar->m_bIsReverseAttack,
		NULL);

	if (!pvPacketCastleInfo)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_UPDATE;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
		&cOperation,
		NULL,
		pvPacketCastleInfo,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	m_csPacketCastleInfo.FreePacket(pvPacketCastleInfo);

	return pvPacket;
}

PVOID AgpmSiegeWar::MakePacketSyncMessage(AgpmSiegeMessage eMessageType, UINT16 unTimeSec, INT16 *pnPacketLength)
{
	if (!pnPacketLength)
		return NULL;

	INT8	cMessageType	= (INT8) eMessageType;

	PVOID	pvPacketMessage	= m_csPacketSyncMessage.MakePacket(FALSE, NULL, 0,
								&cMessageType,
								(unTimeSec > 0) ? &unTimeSec : NULL);
	if (!pvPacketMessage)
		return NULL;

	INT8	cOperation	= AGPMSIEGE_OPERATION_MESSAGE;
	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSIEGEWAR_PACKET_TYPE,
								&cOperation,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								pvPacketMessage);

	return pvPacket;
}

UINT64 AgpmSiegeWar::GetNextSiegeWarFirstTime(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return 0;

	UINT64	ullNextSiegeWarTimeDate	= pcsSiegeWar->m_ullPrevSiegeWarTimeDate + 60 * 60 * 24 * (UINT64) pcsSiegeWar->m_csTemplate.m_ulSiegeWarIntervalDay * 10000000;

	FILETIME	file_time;
	SYSTEMTIME	system_time;

	CopyMemory(&file_time, &ullNextSiegeWarTimeDate, sizeof(UINT64));
	if (::FileTimeToSystemTime(&file_time, &system_time) == 0)
		return 0;

	if (system_time.wDayOfWeek == 0)	// sunday
	{
		ullNextSiegeWarTimeDate	-= (UINT64) 60 * 60 * 24 * 10000000;
	}

	return ullNextSiegeWarTimeDate;
}

UINT64 AgpmSiegeWar::GetFirstSiegeWarTime(AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsSiegeWar)
		return 0;

	UINT64	ullCurrentTime	= AuTimeStamp2::GetCurrentTimeStamp();

	UINT64	ullFirstTime	= ullCurrentTime + 60 * 60 * 24 * ((UINT64) (pcsSiegeWar->m_csTemplate.m_ulSiegeWarIntervalDay - 7)) * 10000000;

	FILETIME	file_time;
	SYSTEMTIME	system_time;

	CopyMemory(&file_time, &ullFirstTime, sizeof(UINT64));
	if (::FileTimeToSystemTime(&file_time, &system_time) == 0)
		return 0;

	ullFirstTime	+= (6 - (UINT64) system_time.wDayOfWeek) * 60 * 60 * 24 * 10000000;

	return ullFirstTime;
}

BOOL AgpmSiegeWar::IsCastleOwner(AgpdSiegeWar *pcsSiegeWar, CHAR *szCharID)
{
	if (!pcsSiegeWar || !szCharID || !szCharID[0])
		return FALSE;

	// 성주인지 확인해본다.
	if (pcsSiegeWar->m_strOwnerGuildMasterName.Compare(szCharID) != COMPARE_EQUAL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgpmSiegeWar::IsResurrectableInCastle(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;
	
	// get siege war info of character's position
	AgpdSiegeWar *pcsSiegeWar = GetSiegeWarInfo(pcsCharacter);
	if (!pcsSiegeWar)
		return FALSE;
	
	// in siege war event?
	if (!IsStarted(pcsSiegeWar))
		return FALSE;

	// if archlord castle and archlord determination battle started, can't resurrect
	//if (IsArchlordCastle(pcsSiegeWar) && AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE <= pcsSiegeWar->m_eCurrentStatus)
	if ( IsArchlordCastle(pcsSiegeWar) )
	{
		if( AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE == pcsSiegeWar->m_eCurrentStatus)
			return FALSE;
		else
			return TRUE;
	}
	BOOL bOffense = FALSE;
	BOOL bDefense = FALSE;


	bOffense	= IsAttackGuild(pcsCharacter, pcsSiegeWar);
	bDefense	= IsDefenseGuild(pcsCharacter, pcsSiegeWar);

	// offence guild and point is active?
	if (bOffense && pcsSiegeWar->m_bIsActiveAttackResurrection)
		return TRUE;

	// defence guild and point is active?
	if (bDefense && pcsSiegeWar->m_bIsActiveDefenseResurrection)
		return TRUE;

	// character doesn't engaged in siege war
	return FALSE;
}

AgpdSiegeWar*	AgpmSiegeWar::GetSiegeWar(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsCharacter);
	if (!pcsAttachData)
		return NULL;

	return pcsAttachData->m_pcsSiegeWar;
}

BOOL	AgpmSiegeWar::SetSiegeWar(AgpdCharacter *pcsCharacter, AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsCharacter);
	if (!pcsAttachData)
		return FALSE;

	pcsAttachData->m_pcsSiegeWar	= pcsSiegeWar;

	return TRUE;
}

BOOL	AgpmSiegeWar::IsDefenseGuild(AgpdCharacter *pcsCharacter, AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsCharacter || !pcsSiegeWar)
		return FALSE;

	//AgpdSiegeWar	*pcsCharacterSiegeWar	= GetSiegeWarInfo(pcsCharacter);

	//if (pcsCharacterSiegeWar != pcsSiegeWar)
	//	return FALSE;

	BOOL	bIsDefenseGuild	= FALSE;

	if (pcsSiegeWar->m_csDefenseGuild.GetObject(m_pcsAgpmGuild->GetJoinedGuildID(pcsCharacter)))
		bIsDefenseGuild	= TRUE;

	//if (pcsSiegeWar->m_bIsReverseAttack)
	//	return (!bIsDefenseGuild);

	return bIsDefenseGuild;
}

BOOL	AgpmSiegeWar::IsAttackGuild(AgpdCharacter *pcsCharacter, AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsCharacter || !pcsSiegeWar)
		return FALSE;

	//AgpdSiegeWar	*pcsCharacterSiegeWar	= GetSiegeWarInfo(pcsCharacter);

	//if (pcsCharacterSiegeWar != pcsSiegeWar)
	//	return FALSE;

	BOOL	bIsAttackGuild	= FALSE;

	if (pcsSiegeWar->m_csAttackGuild.GetObject(m_pcsAgpmGuild->GetJoinedGuildID(pcsCharacter)))
		bIsAttackGuild	= TRUE;

	//if (pcsSiegeWar->m_bIsReverseAttack)
	//	return (!bIsAttackGuild);

	return bIsAttackGuild;
}

AgpdSiegeWar* AgpmSiegeWar::IsOwnerGuild(const char* guildID)
{
	for (INT32 i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
	{
		if (m_csSiegeWarInfo[i].m_strOwnerGuildName.Compare(guildID) == COMPARE_EQUAL)
			return &m_csSiegeWarInfo[i];
	}

	return NULL;
}

BOOL AgpmSiegeWar::IsOwnerGuild(AgpdCharacter *pcsCharacter, AgpdSiegeWar *pcsSiegeWar)
{
	if (!pcsCharacter || !pcsSiegeWar)
		return FALSE;

	if (pcsSiegeWar->m_strOwnerGuildName.GetLength() > 0 &&
		pcsSiegeWar->m_strOwnerGuildName.Compare(m_pcsAgpmGuild->GetJoinedGuildID(pcsCharacter)) == COMPARE_EQUAL)
		return TRUE;

	return FALSE;
}

UINT8 AgpmSiegeWar::GetRepairCount(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	return m_acsSiegeWarObjectInfo[GetSiegeWarMonsterType(pcsCharacter)].m_ucRepairCount;
}

INT32 AgpmSiegeWar::GetRepairCost(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	return m_acsSiegeWarObjectInfo[GetSiegeWarMonsterType(pcsCharacter)].m_lRepairCost;
}

UINT8 AgpmSiegeWar::GetRepairDuration(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	return m_acsSiegeWarObjectInfo[GetSiegeWarMonsterType(pcsCharacter)].m_ucRepairDuration;
}

BOOL AgpmSiegeWar::SetRepairStartTime(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsCharacter);

	pcsAttachData->m_ulStartRepairTimeMSec	= GetClockCount();

	return TRUE;
}

BOOL AgpmSiegeWar::ReadSiegeWarObjectInfo(CHAR *pszFileName, BOOL bDecryption)
{
	if (!pszFileName || !pszFileName[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(pszFileName, TRUE, bDecryption))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();
	INT16			nColumn			= csExcelTxtLib.GetColumn();

	if (nRow <= 1)
		return FALSE;

	AuAutoLock	lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	for (int i = 1; i < nRow; ++i)
	{
		CHAR	*pszObjectType	= csExcelTxtLib.GetData(0, i);
		if (!pszObjectType || !pszObjectType[0])
			continue;

		AgpdSiegeWarMonsterType	eMonsterType	= (AgpdSiegeWarMonsterType) atoi(pszObjectType);
		if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE ||
			eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
			continue;

		CHAR	*pszColumnName	= NULL;
		CHAR	*pszValue		= NULL;

		for (int j = 2; j < 5; ++j)
		{
			pszColumnName	= csExcelTxtLib.GetData(j, 0);
			if (!pszColumnName || !pszColumnName[0])
				break;

			pszValue	= csExcelTxtLib.GetData(j, i);
			if (!pszValue || !pszValue[0])
				break;

			if (strcmp(pszColumnName, AGSMSIEGEWAR_STREAM_REPAIR_COUNT) == 0)
				m_acsSiegeWarObjectInfo[eMonsterType].m_ucRepairCount		= atoi(pszValue);

			else if (strcmp(pszColumnName, AGSMSIEGEWAR_STREAM_REPAIR_COST) == 0)
				m_acsSiegeWarObjectInfo[eMonsterType].m_lRepairCost			= atoi(pszValue);

			else if (strcmp(pszColumnName, AGSMSIEGEWAR_STREAM_REPAIR_DURATION) == 0)
				m_acsSiegeWarObjectInfo[eMonsterType].m_ucRepairDuration	= atoi(pszValue);
		}

		INT32	lCount	= 0;
		for (int j = 5; j < nColumn; ++j)
		{
			pszValue	= csExcelTxtLib.GetData(j, i);
			if (!pszValue || !pszValue[0])
				break;

			m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[lCount]	= atoi(pszValue);

			++j;
			pszValue	= csExcelTxtLib.GetData(j, i);
			if (pszValue && pszValue[0])
				m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemCountforActive[lCount]	= atoi(pszValue);

			++lCount;
		}
	}

	return TRUE;
}

BOOL AgpmSiegeWar::CheckActiveCondition(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter ||
		!IsSiegeWarMonster(pcsCharacter))
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsCharacter);
	if (!pcsAttachData->m_pcsAttackObjectGrid)
		return FALSE;

	AgpdSiegeWarMonsterType	eMonsterType	= GetSiegeWarMonsterType(pcsCharacter);

	for (int i = 0; i < 8; ++i)
	{
		if (m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i] != AP_INVALID_IID)
		{
			if (m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemCountforActive[i] > 0)
			{
				INT32	lCount	= m_pcsAgpmGrid->GetCountByTemplate(pcsAttachData->m_pcsAttackObjectGrid, AGPDGRID_ITEM_TYPE_ITEM, m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i]);

				if (lCount < m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemCountforActive[i])
					return FALSE;
			}
			else
			{
				if (!m_pcsAgpmGrid->GetItemByTemplate(pcsAttachData->m_pcsAttackObjectGrid, AGPDGRID_ITEM_TYPE_ITEM, m_acsSiegeWarObjectInfo[eMonsterType].m_lNeedItemTIDforActive[i]))
					return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL AgpmSiegeWar::IsSiegeWarMonster(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdSiegeWarMonsterType	eMonsterType	= GetSiegeWarMonsterType(pcsCharacter);
	if (eMonsterType <= AGPD_SIEGE_MONSTER_TYPE_NONE ||
		eMonsterType >= AGPD_SIEGE_MONSTER_TYPE_MAX)
		return FALSE;

	return TRUE;
}

BOOL AgpmSiegeWar::CheckCarveASeal(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsThrone)
{
	if (!pcsCharacter || !pcsThrone)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pcsThrone);
	if (!pcsSiegeWar)
		return FALSE;

	if (!IsAttackGuild(pcsCharacter, pcsSiegeWar) &&
		!IsDefenseGuild(pcsCharacter, pcsSiegeWar))
		return FALSE;

	// check siegewar status
	if (IsArchlordCastle(pcsSiegeWar))
	{
		if (pcsSiegeWar->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_START ||
			pcsSiegeWar->m_strCarveGuildName.GetLength() > 0)
			return FALSE;
	}
	else
	{
		if (pcsSiegeWar->m_eCurrentStatus != AGPD_SIEGE_WAR_STATUS_OPEN_EYES)
			return FALSE;
	}

	// pcsThrone이 봉인성좌인지 본다.
	if (GetSiegeWarMonsterType(pcsThrone) != AGPD_SIEGE_MONSTER_THRONE)
		return FALSE;

	AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pcsCharacter);
	if (!pcsGuild)
		return FALSE;

	if (!m_pcsAgpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID))
		return FALSE;

	// 봉인성좌의 보호막이 없어져야 한다. 즉, HP는 0이어야 한다.
	/*
	INT32	lHP	= m_pcsAgpmFactors->GetHP((AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsThrone->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
	if (lHP > 0)
		return FALSE;
	*/

	if (!IsArchlordCastle(pcsSiegeWar) && pcsThrone->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
		return FALSE;

	// 다른사람이 각인중인지 본다.
	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsThrone);
	if (pcsAttachData->m_strCarveGuildName.GetLength() > 0)
		return FALSE;

	// 공격하는 측의 길드마스터이어야 한다.
	// 즉, 현재 성주 (혹은 각인한 길마)와 적대 길드의 길마가 각인을 할 수 있다.
	BOOL	bCarveAttackGuild	= FALSE;

	if (pcsSiegeWar->m_strCarveGuildName.GetLength() > 0)
	{
		if (pcsSiegeWar->m_strCarveGuildName.Compare(pcsGuild->m_szID) == COMPARE_EQUAL)
			return FALSE;
	}
	else if (pcsSiegeWar->m_strOwnerGuildName.GetLength() > 0)
	{
		if (pcsSiegeWar->m_strOwnerGuildName.Compare(pcsGuild->m_szID) == COMPARE_EQUAL)
			return FALSE;

		// 처음 각인은 공성 길드여야 한다.
		if (!IsAttackGuild(pcsCharacter, pcsSiegeWar))
			return FALSE;
	}

	// pcsCharacter의 block condition을 체크한다.
	if (m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	return TRUE;
}

BOOL AgpmSiegeWar::StartCarveASeal(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsThrone)
{
	if (!pcsCharacter || !pcsThrone)
		return FALSE;

	AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pcsCharacter);
	if (!pcsGuild)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsThrone);

	pcsAttachData->m_strCarveGuildName.SetText(pcsGuild->m_szID);
	pcsAttachData->m_ulStartCarveTimeMSec	= GetClockCount();

	pcsAttachData	= GetAttachCharacterData(pcsCharacter);
	
	pcsAttachData->m_bIsCarving	= TRUE;

	return TRUE;
}

BOOL AgpmSiegeWar::CancelCarveASeal(AgpdCharacter *pcsThrone)
{
	if (!pcsThrone)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsThrone);

	pcsAttachData->m_strCarveGuildName.Clear();
	pcsAttachData->m_ulStartCarveTimeMSec	= 0;

	return TRUE;
}

BOOL AgpmSiegeWar::EndCarveASeal(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsThrone)
{
	if (!pcsCharacter || !pcsThrone)
		return FALSE;

	AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pcsCharacter);
	if (!pcsGuild)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsThrone);

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pcsThrone);
	if (!pcsSiegeWar)
		return FALSE;

	pcsAttachData->m_strCarveGuildName.Clear();
	pcsAttachData->m_ulStartCarveTimeMSec	= 0;

	pcsAttachData	= GetAttachCharacterData(pcsCharacter);
	
	pcsAttachData->m_bIsCarving	= FALSE;

	return TRUE;
}

BOOL AgpmSiegeWar::SetUseCharacterName(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsUser)
{
	if (!pcsCharacter || !pcsUser)
		return FALSE;

	AgpdSiegeWarADCharacter	*pcsAttachData	= GetAttachCharacterData(pcsCharacter);

	pcsAttachData->m_strUseCharacterName.SetText(pcsUser->m_szID);

	return TRUE;
}

BOOL AgpmSiegeWar::IsDoingSiegeWar(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdSiegeWar *pcsSiegeWar	= GetSiegeWarInfo(pcsCharacter);
	if (!pcsSiegeWar)
		return FALSE;

	// 공성 중인지 본다.
	if (!IsStarted(pcsSiegeWar))
		return FALSE;

	return TRUE;
}

BOOL AgpmSiegeWar::UpdateReverseAttack(AgpdSiegeWar *pcsSiegeWar, BOOL bIsReverseAttack)
{
	if (!pcsSiegeWar)
		return FALSE;

	//if (pcsSiegeWar->m_bIsReverseAttack != bIsReverseAttack)
	//{
	//	pcsSiegeWar->m_bIsReverseAttack	= bIsReverseAttack;
	//
	//	EnumCallback(AGPMSIEGEWAR_CB_UPDATE_REVERSE_ATTACK, pcsSiegeWar, NULL);
	//}

	return TRUE;
}

BOOL AgpmSiegeWar::IsEndArchlordSiegeWar(AgpdSiegeWar *pcsSiegeWar)
{
	if (!IsArchlordCastle(pcsSiegeWar))
		return FALSE;

	if (pcsSiegeWar->m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_START &&
		pcsSiegeWar->m_strCarveGuildName.GetLength() > 0)
		return TRUE;

	return FALSE;
}

AgpdCharacter* AgpmSiegeWar::GetArchlord()
{
	return m_pcsAgpmCharacter->GetCharacter(m_csSiegeWarInfo[AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX].m_strOwnerGuildMasterName);
}

BOOL AgpmSiegeWar::AddArchlordCastleAttackGuild(AgpdGuild *pcsGuild)
{
	if (!pcsGuild)
		return FALSE;

	if (AGPMSIEGE_RESULT_ATTACK_APPL_SUCCESS != AddAttackApplicationGuild(GetArchlordCastle(), pcsGuild))
		return FALSE;
	
	// 2007.04.25. laki. 신청길드뿐만 아니라 공성길드에도 넣는다.
	SetAttackGuild(GetArchlordCastle(), pcsGuild, FALSE);

	EnumCallback(AGPMSIEGEWAR_CB_ADD_ARCHLORD_CASTLE_ATTACK_GUILD, pcsGuild, NULL);

	return TRUE;
}

void AgpmSiegeWar::SetDayOfSiegeWar()
{
#ifdef _SIEGETIMER
	time_t CurrentTime;
	time(&CurrentTime);
	
	tm *pCurrentTime = localtime(&CurrentTime);
	if (!pCurrentTime)
		return;

	for (INT32 i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; i++)
	{
		AgpdSiegeWar *pAgpdSiegeWar = GetSiegeWarInfo(i);
		if (!pAgpdSiegeWar || 0 == pAgpdSiegeWar->m_ullNextSiegeWarTimeDate)
			continue;
		
		// compare day of year
		SYSTEMTIME	system_time;
		FILETIME	file_time;

		CopyMemory(&file_time, &pAgpdSiegeWar->m_ullNextSiegeWarTimeDate, sizeof(UINT64));
		if (0 == ::FileTimeToSystemTime(&file_time, &system_time))
			continue;
		
		if ((system_time.wYear == pCurrentTime->tm_year)
			&& (system_time.wMonth == pCurrentTime->tm_mon)
			&& (system_time.wDay == pCurrentTime->tm_mday)
			)		// found
		{
			m_bIsDayOfSiegeWar = TRUE;
			return;
		}
	}
#else
	SYSTEMTIME	curr_time;
	::GetLocalTime(&curr_time);

	for (INT32 i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; i++)
	{
		AgpdSiegeWar *pAgpdSiegeWar = GetSiegeWarInfo(i);
		if (!pAgpdSiegeWar || 0 == pAgpdSiegeWar->m_ullNextSiegeWarTimeDate)
			continue;

		// compare day of year
		SYSTEMTIME	system_time;
		FILETIME	file_time;

		CopyMemory(&file_time, &pAgpdSiegeWar->m_ullNextSiegeWarTimeDate, sizeof(UINT64));
		if (0 == ::FileTimeToSystemTime(&file_time, &system_time))
			continue;

		if ((system_time.wYear == curr_time.wYear)
			&& (system_time.wMonth == curr_time.wMonth)
			&& (system_time.wDay == curr_time.wDay)
			)		// found
		{
			m_bIsDayOfSiegeWar = TRUE;
			return;
		}
	}
#endif	
	m_bIsDayOfSiegeWar = FALSE;
}

BOOL AgpmSiegeWar::IsDayOfSiegeWar()
{
	return m_bIsDayOfSiegeWar;
}


BOOL AgpmSiegeWar::SetGuildTargetPosition(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos, BOOL bInner)
{
	if (!pcsCharacter || !pstTargetPos)
		return FALSE;
		
	if (IsAttackGuild(pcsCharacter, GetSiegeWarInfo(pcsCharacter)))
		return SetAttackGuildTargetPosition(pcsCharacter, pstTargetPos, bInner);
	else if (IsDefenseGuild(pcsCharacter, GetSiegeWarInfo(pcsCharacter)))
		return SetDefenseGuildTargetPosition(pcsCharacter, pstTargetPos, bInner);

	return FALSE;
}


BOOL AgpmSiegeWar::SetAttackGuildTargetPosition(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos, BOOL bInner)
{
	if (!m_pcsAgpmEventBinding)
		return FALSE;

	if (!pcsCharacter || !pstTargetPos)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pcsCharacter);
	if (!pcsSiegeWar)
		return FALSE;

	// 2007.04.25. 렌스피어는 시작체크 안한다. 어차피 아래 공격길드에서 까질테니...
	if (!IsStarted(pcsSiegeWar) && !IsArchlordCastle(pcsSiegeWar))
		return FALSE;

	if (!IsAttackGuild(pcsCharacter, pcsSiegeWar))
		return FALSE;

	AuPOS	stNewPosition;

	if (IsArchlordCastle(pcsSiegeWar))
	{
		AgpdGuild	*pcsGuild	= m_pcsAgpmGuild->GetGuild(pcsCharacter);
		if (!pcsGuild)
			return FALSE;

		INT32	lIndex	= 0;

		if (m_csSiegeWarInfo[AGPMSIEGEWAR_HUMAN_CASTLE_INDEX].m_strOwnerGuildName.Compare(pcsGuild->m_szID) == COMPARE_EQUAL)
		{
			lIndex	= 2;
		}
		else if (m_csSiegeWarInfo[AGPMSIEGEWAR_ORC_CASTLE_INDEX].m_strOwnerGuildName.Compare(pcsGuild->m_szID) == COMPARE_EQUAL)
		{
			lIndex	= 0;
		}
		else if (m_csSiegeWarInfo[AGPMSIEGEWAR_MOONELF_CASTLE_INDEX].m_strOwnerGuildName.Compare(pcsGuild->m_szID) == COMPARE_EQUAL)
		{
			lIndex	= 1;
		}
		else if (m_csSiegeWarInfo[AGPMSIEGEWAR_DRAGONSCION_CASTLE_INDEX].m_strOwnerGuildName.Compare(pcsGuild->m_szID) == COMPARE_EQUAL)
		{
			lIndex  = 3;
		}
		else
		{
			return FALSE;
		}

		if (!(m_pcsAgpmEventBinding->GetBindingPositionForSiegeWarResurrection(pcsCharacter, &stNewPosition, pcsSiegeWar->m_apcsRegionTemplate[0]->pStrName, TRUE, bInner, lIndex) ||
			  m_pcsAgpmEventBinding->GetBindingPositionForSiegeWarResurrection(pcsCharacter, &stNewPosition, pcsSiegeWar->m_apcsRegionTemplate[1]->pStrName, TRUE, bInner, lIndex)))
			return FALSE;
	}
	else
	{
		if (!(m_pcsAgpmEventBinding->GetBindingPositionForSiegeWarResurrection(pcsCharacter, &stNewPosition, pcsSiegeWar->m_apcsRegionTemplate[0]->pStrName, TRUE, bInner) ||
			  m_pcsAgpmEventBinding->GetBindingPositionForSiegeWarResurrection(pcsCharacter, &stNewPosition, pcsSiegeWar->m_apcsRegionTemplate[1]->pStrName, TRUE, bInner)))
			return FALSE;
	}

	*pstTargetPos	= stNewPosition;

	return TRUE;
}

BOOL AgpmSiegeWar::SetDefenseGuildTargetPosition(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos, BOOL bInner)
{
	if (!m_pcsAgpmEventBinding)
		return FALSE;

	if (!pcsCharacter || !pstTargetPos)
		return FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= GetSiegeWarInfo(pcsCharacter);
	if (!pcsSiegeWar)
		return FALSE;

	if (!IsStarted(pcsSiegeWar))
		return FALSE;

	if (!IsDefenseGuild(pcsCharacter, pcsSiegeWar))
		return FALSE;

	AuPOS	stNewPosition;
	if (IsArchlordCastle(pcsSiegeWar))
	{
		if (!(m_pcsAgpmEventBinding->GetBindingForArchlordAttacker(&stNewPosition, pcsSiegeWar->m_apcsRegionTemplate[0]->pStrName) ||
			  m_pcsAgpmEventBinding->GetBindingForArchlordAttacker(&stNewPosition, pcsSiegeWar->m_apcsRegionTemplate[1]->pStrName)))
			return FALSE;
	}
	else
	{
		if (!(m_pcsAgpmEventBinding->GetBindingPositionForSiegeWarResurrection(pcsCharacter, &stNewPosition, pcsSiegeWar->m_apcsRegionTemplate[0]->pStrName, FALSE, bInner) ||
			  m_pcsAgpmEventBinding->GetBindingPositionForSiegeWarResurrection(pcsCharacter, &stNewPosition, pcsSiegeWar->m_apcsRegionTemplate[1]->pStrName, FALSE, bInner)))
			return FALSE;
	}

	*pstTargetPos	= stNewPosition;

	return TRUE;
}

UINT32 AgpmSiegeWar::GetDurationMSec(AgpdSiegeWar *pcsSiegeWar)
{
	UINT32 ulDuration = pcsSiegeWar->m_csTemplate.m_ulSiegeWarDurationMSec;
	if (pcsSiegeWar->m_eCurrentStatus == AGPD_SIEGE_WAR_STATUS_ARCHLORD_BATTLE)		// 결정전 3단계 시간조정.
		ulDuration	= (UINT32) (ulDuration / 2);
	
	return ulDuration;
}

// 2008.01.03. steeple
// 해당 캐릭터가 비밀의 던젼에 있는 지 확인한다.
BOOL AgpmSiegeWar::IsInSecretDungeon(AgpdCharacter* pcsCharacter)
{
	BOOL bResult = FALSE;
	if(pcsCharacter)
	{
		switch(pcsCharacter->m_nBindingRegionIndex) {
		case 95:	// 인플레임 성주 사냥터
		case 96:	// 인플레임 성주 사냥터 입구
		case 98:	// 아르시안 성주 사냥터
		case 99:	// 아르시안 성주 사냥터 입구
		case 101:	// 트라쿠아 성주 사냥터
		case 102:	// 트라쿠아 성주 사냥터 입구
		case 172:	// 드래이거 성주 사냥터
		case 173:	// 드래이거 성주 사냥터 입구
		case 186:	// 랜스피어 성주 사냥터
		case 187:	// 랜스피어 성주 사냥터 입구
			bResult = TRUE;
			break;

		default:
			bResult = FALSE;
		}
	}

	return bResult;
}

// 해당 캐릭터가 성주 길드인지 확인하다.
BOOL AgpmSiegeWar::IsCastleOwnerGuildMember(AgpdCharacter* pcsCharacter)
{
	if ( AgpdGuild* pGuild = m_pcsAgpmGuild->GetGuild(pcsCharacter) )
	{
		if ( GetSiegeWarByOwner(pGuild->m_szMasterID) )
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmSiegeWar::IsThisRegionCastleOwnerGuildMember(AgpdCharacter* pcsCharacter)
{
	AgpdGuild* pGuild = m_pcsAgpmGuild->GetGuild(pcsCharacter);
	if(!pGuild)
		return FALSE;

	AgpdSiegeWar *pcsSiegeWar = GetSiegeWarInfo(pcsCharacter);
	if(!pcsSiegeWar)
		return FALSE;

	if(COMPARE_EQUAL != pcsSiegeWar->m_strOwnerGuildMasterName.Compare(pGuild->m_szMasterID))
		return FALSE;

	return TRUE;
}

BOOL AgpmSiegeWar::IsThisRegionCastleInSiegeWar(AgpdCharacter* pcsCharacter)
{
	AgpdSiegeWar *pcsSiegeWar = GetSiegeWarInfo(pcsCharacter);
	if(!pcsSiegeWar)
		return FALSE;

	if(pcsSiegeWar->m_eCurrentStatus <= AGPD_SIEGE_WAR_STATUS_READY)
		return FALSE;

	return TRUE;
}