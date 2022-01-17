// AgcmEventGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 13.

#include "AgcmEventGuild.h"

#include "ApMemoryTracker.h"

AgcmEventGuild::AgcmEventGuild()
{
	SetModuleName("AgcmEventGuild");
	
	m_pcsAgpmCharacter = NULL;
	m_pcsAgpmEventGuild = NULL;
	m_pcsAgcmCharacter = NULL;
	m_pcsAgcmEventManager = NULL;
}

AgcmEventGuild::~AgcmEventGuild()
{
}

BOOL AgcmEventGuild::OnAddModule()
{
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgpmEventGuild = (AgpmEventGuild*)GetModule("AgpmEventGuild");

	m_pcsAgcmCharacter = (AgcmCharacter*)GetModule("AgcmCharacter");
	m_pcsAgcmEventManager = (AgcmEventManager*)GetModule("AgcmEventManager");

	if(!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventGuild ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmEventManager)
		return FALSE;

	if(!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_GUILD, CBEventRequest, this))
		return FALSE;
	
	if(!m_pcsAgpmEventGuild->SetCallbackGrant(CBEventGrant, this))
		return FALSE;

	if(!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_GUILD_WAREHOUSE, CBEventRequestWarehouse, this))
		return FALSE;
	
	if(!m_pcsAgpmEventGuild->SetCallbackGrantWarehouse(CBEventGrantWarehouse, this))
		return FALSE;

	if(!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_WORLD_CHAMPIONSHIP, CBEventRequestWorldChampionship, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmEventGuild::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMEVENTGUILD_CB_GRANT, pfCallback, pClass);
}

BOOL AgcmEventGuild::SetCallbackGrantWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMEVENTGUILD_CB_GRANT_WAREHOUSE, pfCallback, pClass);
}

BOOL AgcmEventGuild::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgcmEventGuild* pThis = (AgcmEventGuild*)pClass;
	ApBase* pcsGenerator = (ApBase*)pCustData;

	if(pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventGuild->MakePacketEventRequest(pcsEvent, pcsGenerator->m_lID, &nPacketLength);
	
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength);
	pThis->m_pcsAgpmEventGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmEventGuild::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgcmEventGuild* pThis = (AgcmEventGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	if(pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return FALSE;

	if(pcsEvent->m_eFunction != APDEVENT_FUNCTION_GUILD)
		return FALSE;
	
	pThis->EnumCallback(AGCMEVENTGUILD_CB_GRANT, pcsCharacter, pcsEvent);

	return TRUE;
}

BOOL AgcmEventGuild::CBEventRequestWarehouse(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgcmEventGuild* pThis = (AgcmEventGuild*)pClass;
	ApBase* pcsGenerator = (ApBase*)pCustData;

	if(pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventGuild->MakePacketEventRequestWarehouse(pcsEvent, pcsGenerator->m_lID, &nPacketLength);
	
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength);
	pThis->m_pcsAgpmEventGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmEventGuild::CBEventGrantWarehouse(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgcmEventGuild* pThis = (AgcmEventGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	if(pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return FALSE;

	if(pcsEvent->m_eFunction != APDEVENT_FUNCTION_GUILD_WAREHOUSE)
		return FALSE;
	
	pThis->EnumCallback(AGCMEVENTGUILD_CB_GRANT_WAREHOUSE, pcsCharacter, pcsEvent);

	return TRUE;
}

BOOL AgcmEventGuild::CBEventRequestWorldChampionship(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgcmEventGuild* pThis = (AgcmEventGuild*)pClass;
	ApBase* pcsGenerator = (ApBase*)pCustData;

	if(pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmEventGuild->MakePacketEventRequestWorldChampionship(pcsEvent, pcsGenerator->m_lID, &nPacketLength);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	pThis->SendPacket(pvPacket, nPacketLength);
	pThis->m_pcsAgpmEventGuild->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmEventGuild::CBEventGrantWorldChampionship(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgcmEventGuild* pThis = (AgcmEventGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	if(pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return FALSE;

	if(pcsEvent->m_eFunction != APDEVENT_FUNCTION_WORLD_CHAMPIONSHIP)
		return FALSE;

	pThis->EnumCallback(AGCMEVENTGUILD_CB_GRANT_WORLD_CHAMPIONSHIP, pcsCharacter, pcsEvent);

	return TRUE;
}