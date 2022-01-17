// AgpmConfig.cpp
// (C) NHN Games - ArchLord Development Team
// kelovon, 20051006

#include "AgpmConfig.h"
#include "AuGameEnv.h"

#define AGPMCONFIG_MAX_EXP_RATIO			5.0f
#define AGPMCONFIG_MAX_DROP_RATIO			5.0f
#define AGPMCONFIG_MAX_GHELD_DROP_RATIO		5.0f
#define AGPMCONFIG_MAX_CHARISM_DROP_RATIO	5.0f

AgpmConfig::AgpmConfig()
{
	SetModuleName("AgpmConfig");
	SetModuleType(APMODULE_TYPE_PUBLIC);

	EnableIdle2(FALSE);

	m_stAgpdConfig.Reset();

	SetPacketType(AGPMCONFIG_PACKET_TYPE);

	// flag length = 16bits
	m_csPacket.SetFlagLength(sizeof(INT32));
	m_csPacket.SetFieldType(
							AUTYPE_INT32,	1,		// bPCDropItemOnDeath
							AUTYPE_INT32,	1,		// bExpPenaltyOnDeath
							AUTYPE_INT8,	1,		// bIsTestServer
							AUTYPE_END, 0
							);
}

AgpmConfig::~AgpmConfig()
{
}

BOOL AgpmConfig::OnAddModule()
{
	AuGameEnv().InitEnvironment();
	return TRUE;
}

BOOL AgpmConfig::OnInit()
{
	return TRUE;
}

BOOL AgpmConfig::OnIdle2(UINT32 ulClockCount)
{
	return TRUE;
}
BOOL AgpmConfig::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgpmCharacter::OnReceive");

	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	// m_csPacket
	{
		PROFILE("AgpmConfig::OnReceive GetField");

		m_csPacket.GetField(TRUE, pvPacket, nSize, 
							&m_stAgpdConfig.bPCDropItemOnDeath, 
							&m_stAgpdConfig.bExpPenaltyOnDeath,
							&m_stAgpdConfig.bIsTestServer
							);
	}

	if( m_stAgpdConfig.bIsTestServer )
	{
		m_stAgpdConfig.bIsAllAdmin = TRUE;	
	}

	return TRUE;
}

void AgpmConfig::SendConfigPacket(UINT32 ulNID)
{
	PVOID	pvPacket;
	INT16	nPacketLength;

	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCONFIG_PACKET_TYPE,
									 &m_stAgpdConfig.bPCDropItemOnDeath,
									 &m_stAgpdConfig.bExpPenaltyOnDeath,
									 &m_stAgpdConfig.bIsTestServer
									 );

	if (!pvPacket)
		return;

	SendPacket(pvPacket, nPacketLength, ulNID);
}

BOOL AgpmConfig::OnDestroy()
{
	return TRUE;
}

void AgpmConfig::SetEventNumber(INT32 lEventNumber /*= 0*/)
{
	m_stAgpdConfig.lEventNumber = lEventNumber;
}

INT32 AgpmConfig::GetEventNumber()
{
	return m_stAgpdConfig.lEventNumber;
}

void AgpmConfig::SetExpAdjustmentRatio(float fRatio)
{
	if(AuGameEnv().IsAlpha() != FALSE &&
		IsTestServer() == FALSE)
	{
		if(fRatio > AGPMCONFIG_MAX_EXP_RATIO)
			fRatio = AGPMCONFIG_MAX_EXP_RATIO;
	}

	m_stAgpdConfig.fExpAdjustmentRatio = fRatio;
}

float AgpmConfig::GetExpAdjustmentRatio(BOOL bTPackUser)
{
	FLOAT fTPackBenefit = 0.0f;

	if(bTPackUser)
		fTPackBenefit = static_cast<FLOAT>((GetTPackExpRatio() / 100.0f));

	return m_stAgpdConfig.fExpAdjustmentRatio + fTPackBenefit;
}

void AgpmConfig::SetDropAdjustmentRatio(float fRatio)
{
	if(AuGameEnv().IsAlpha() != FALSE &&
		IsTestServer() == FALSE)
	{
		if(fRatio > AGPMCONFIG_MAX_DROP_RATIO)
			fRatio = AGPMCONFIG_MAX_DROP_RATIO;
	}

	m_stAgpdConfig.fDropAdjustmentRatio = fRatio;
}

float AgpmConfig::GetDropAdjustmentRatio(BOOL bTPackUser)
{
	FLOAT fTPackBenefit = 0.0f;

	if(bTPackUser)
		fTPackBenefit = static_cast<FLOAT>((GetTPackDropRatio() / 100.0f));

	return m_stAgpdConfig.fDropAdjustmentRatio + fTPackBenefit;
}

void AgpmConfig::SetDrop2AdjustmentRatio(float fRatio)
{
	if(AuGameEnv().IsAlpha() != FALSE &&
		IsTestServer() == FALSE)
	{
		if(fRatio > AGPMCONFIG_MAX_DROP_RATIO)
			fRatio = AGPMCONFIG_MAX_DROP_RATIO;
	}

	m_stAgpdConfig.fDrop2AdjustmentRatio = fRatio;
}

float AgpmConfig::GetDrop2AdjustmentRatio(BOOL bTpackUser)
{
	FLOAT fTPackBenefit = 0.0f;

	if(bTpackUser)
		fTPackBenefit = static_cast<FLOAT>((GetTPackDropRatio() / 100.0f));

	return m_stAgpdConfig.fDrop2AdjustmentRatio;
}

void AgpmConfig::SetGheldDropAdjustmentRatio(float fRatio)
{
	if(AuGameEnv().IsAlpha() != FALSE &&
		IsTestServer() == FALSE)
	{
		if(fRatio > AGPMCONFIG_MAX_GHELD_DROP_RATIO)
			fRatio = AGPMCONFIG_MAX_GHELD_DROP_RATIO;
	}

	m_stAgpdConfig.fGheldDropAdjustmentRatio = fRatio;
}

float AgpmConfig::GetGheldDropAdjustmentRatio(BOOL bTPackUser)
{
	FLOAT fTPackBenefit = 0.0f;

	if(bTPackUser)
		fTPackBenefit = static_cast<FLOAT>((GetTPackGheldRatio() / 100.0f));

	return m_stAgpdConfig.fGheldDropAdjustmentRatio + fTPackBenefit;
}

void AgpmConfig::SetCharismaDropAdjustmentRatio(float fRatio)
{
	if(AuGameEnv().IsAlpha() != FALSE &&
		IsTestServer() == FALSE)
	{
		if(fRatio > AGPMCONFIG_MAX_CHARISM_DROP_RATIO)
			fRatio = AGPMCONFIG_MAX_CHARISM_DROP_RATIO;
	}

	m_stAgpdConfig.fCharismaDropAdjustmentRatio = fRatio;
}

float AgpmConfig::GetCharismaDropAdjustmentRatio(BOOL bTPackUser)
{
	FLOAT fTPackBenefit = 0.0f;

	if(bTPackUser)
		fTPackBenefit = static_cast<FLOAT>((GetTPackCharismaRatio() / 100.0f));

	return m_stAgpdConfig.fCharismaDropAdjustmentRatio + fTPackBenefit;
}

void AgpmConfig::SetAdultServer(BOOL bIsAdultServer)
{
	m_stAgpdConfig.bIsAdultServer = bIsAdultServer;
}

BOOL AgpmConfig::IsAdultServer()
{
	return m_stAgpdConfig.bIsAdultServer;
}

void AgpmConfig::SetAllAdmin(BOOL bIsAllAdmin)
{
	m_stAgpdConfig.bIsAllAdmin = bIsAllAdmin;
}

BOOL AgpmConfig::IsAllAdmin()
{
	return m_stAgpdConfig.bIsAllAdmin;
}

void AgpmConfig::SetNewServer(BOOL bIsNewServer)
{
	m_stAgpdConfig.bIsNewServer = bIsNewServer;
}

BOOL AgpmConfig::IsNewServer()
{
	return m_stAgpdConfig.bIsNewServer;
}

void AgpmConfig::SetEventServer(BOOL bIsEventServer)
{
	m_stAgpdConfig.bIsEventServer = bIsEventServer;
}

BOOL AgpmConfig::IsEventServer()
{
	return m_stAgpdConfig.bIsEventServer;
}

void AgpmConfig::SetAimEventServer(BOOL bIsAimEventServer)
{
	m_stAgpdConfig.bIsAimEventServer = bIsAimEventServer;
}

BOOL AgpmConfig::IsAimEventServer()
{
	return m_stAgpdConfig.bIsAimEventServer;
}

void AgpmConfig::SetPCDropItemOnDeath(BOOL bPCDropItemOnDeath)
{
	m_stAgpdConfig.bPCDropItemOnDeath = bPCDropItemOnDeath;
}

BOOL AgpmConfig::DoesPCDropItemOnDeath()
{
	return m_stAgpdConfig.bPCDropItemOnDeath;
}

void AgpmConfig::SetExpPenaltyOnDeath(BOOL bExpPenaltyOnDeath)
{
	m_stAgpdConfig.bExpPenaltyOnDeath = bExpPenaltyOnDeath;
}

BOOL AgpmConfig::DoesExpPenaltyOnDeath()
{
	return m_stAgpdConfig.bExpPenaltyOnDeath;
}

void AgpmConfig::SetIniDir(LPCTSTR szIniDir)
{
	m_stAgpdConfig.szIniDir = szIniDir;
}

LPCTSTR AgpmConfig::GetIniDir()
{
	return (LPCTSTR)m_stAgpdConfig.szIniDir;
}

void AgpmConfig::SetAccountAuth(TCHAR cAccountAuth)
{
	m_stAgpdConfig.cAccountAuth	= cAccountAuth;
}

BOOL AgpmConfig::IsNoAccountAuth()
{
	if (AGPM_CONFIG_VALUE_NO_ACCOUNT_AUTH == m_stAgpdConfig.cAccountAuth)
		return TRUE;

	if (AGPM_CONFIG_VALUE_IGNORE_ACCOUNT_AUTH == m_stAgpdConfig.cAccountAuth)
		return TRUE;

	return FALSE;
}

BOOL AgpmConfig::IsIgnoreAccountAuth()
{
	return (AGPM_CONFIG_VALUE_IGNORE_ACCOUNT_AUTH == m_stAgpdConfig.cAccountAuth);
}

BOOL AgpmConfig::IsWebAccountAuth()
{
	if (IsDBAccountAuth() || IsNoAccountAuth())
		return FALSE;
	return TRUE;
}

BOOL AgpmConfig::IsDBAccountAuth()
{
	return (AGPM_CONFIG_VALUE_DB_ACCOUNT_AUTH == m_stAgpdConfig.cAccountAuth);
}

void AgpmConfig::SetIgnoreLogFail(BOOL bIgnore)
{
	m_stAgpdConfig.bIgnoreLogFail = bIgnore;
}

BOOL AgpmConfig::IsIgnoreLogFail()
{
	return m_stAgpdConfig.bIgnoreLogFail;
}

void AgpmConfig::SetFileLog(BOOL bFile)
{
	m_stAgpdConfig.bFileLog = bFile;
}

BOOL AgpmConfig::IsFileLog()
{
	return m_stAgpdConfig.bFileLog;
}

void AgpmConfig::SetEventChatting(BOOL bIsEventChatting)
{
	m_stAgpdConfig.bIsEventChatting = bIsEventChatting;
}

BOOL AgpmConfig::IsEventChatting()
{
	return m_stAgpdConfig.bIsEventChatting;
}

void AgpmConfig::SetEventResurrect(BOOL bIsEventResurrect)
{
	m_stAgpdConfig.bIsEventResurrect = bIsEventResurrect;
}

BOOL AgpmConfig::IsEventResurrect()
{
	return m_stAgpdConfig.bIsEventResurrect;
}

void AgpmConfig::SetEventItemDrop(BOOL bIsEventItemDrop)
{
	m_stAgpdConfig.bIsEventItemDrop = bIsEventItemDrop;
}

BOOL AgpmConfig::IsEventItemDrop()
{
	return m_stAgpdConfig.bIsEventItemDrop;
}

void AgpmConfig::SetServerStatusFlag(AgpmConfigServerStatusFlag eFlag)
{
	m_stAgpdConfig.ulServerStatus |= eFlag;
}

void AgpmConfig::ResetServerStatusFlag(AgpmConfigServerStatusFlag eFlag)
{
	m_stAgpdConfig.ulServerStatus &= ~eFlag;
}

BOOL AgpmConfig::IsSetServerStatusFlag(AgpmConfigServerStatusFlag eFlag)
{
	return (m_stAgpdConfig.ulServerStatus & eFlag);
}

void AgpmConfig::SetMaxUserCount(INT32 lMaxUserCount)
{
	m_stAgpdConfig.lMaxUserCount	= lMaxUserCount;
}

INT32 AgpmConfig::GetMaxUserCount()
{
	return m_stAgpdConfig.lMaxUserCount;
}

void AgpmConfig::SetEnableAuction(BOOL bEnableAuction /* = AGPM_CONFIG_DEFAULT_VALUE_ENABLE_AUCTION */)
{
	m_stAgpdConfig.bIsEnableAuction = bEnableAuction;
}
	
BOOL AgpmConfig::IsEnableAuction()
{
	return m_stAgpdConfig.bIsEnableAuction;
}

void AgpmConfig::SetEnableEventItemEffect(BOOL bEnableEventItemEffect /* = AGPM_CONFIG_DEFAULT_VALUE_ENABLE_EVENT_ITEM_EFFECT */)
{
	m_stAgpdConfig.bIsEventItemEffect = bEnableEventItemEffect;
}

BOOL AgpmConfig::IsEnableEventItemEffect()
{
	return m_stAgpdConfig.bIsEventItemEffect;
}

void AgpmConfig::SetEnablePvP(BOOL bEnalbePvP /* = AGPM_CONFIG_DEFAULT_VALUE_ENABLE_PVP */)
{
	m_stAgpdConfig.bIsAblePvP = bEnalbePvP;
}

BOOL AgpmConfig::IsEnablePvP()
{
	return m_stAgpdConfig.bIsAblePvP;
}

