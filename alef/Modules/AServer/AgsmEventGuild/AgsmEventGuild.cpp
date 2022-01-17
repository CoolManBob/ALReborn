// AgsmEventGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 13.

#include "AgsmEventGuild.h"

AgsmEventGuild::AgsmEventGuild()
{
	SetModuleName("AgsmEventGuild");

	m_pagpmEventGuild = NULL;
	m_pagsmCharacter = NULL;
}

AgsmEventGuild::~AgsmEventGuild()
{
}

BOOL AgsmEventGuild::OnAddModule()
{
	m_pagpmEventGuild = (AgpmEventGuild*)GetModule("AgpmEventGuild");
	m_pagsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");

	if(!m_pagpmEventGuild || !m_pagsmCharacter)
		return FALSE;

	if(!m_pagpmEventGuild->SetCallbackRequest(CBGuildEventRequest, this))
		return FALSE;
	
	if(!m_pagpmEventGuild->SetCallbackRequestWarehouse(CBGuildEventRequestWarehouse, this))
		return FALSE;

	if(!m_pagpmEventGuild->SetCallbackRequestWorldChampionship(CBGuildEventRequestWorldChampionship, this))
		return FALSE;
			
	return TRUE;
}

BOOL AgsmEventGuild::CBGuildEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgsmEventGuild* pThis = (AgsmEventGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pagpmEventGuild->MakePacketEventGrant(pcsEvent, pcsCharacter->m_lID, &nPacketLength);

	BOOL bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	pThis->m_pagpmEventGuild->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmEventGuild::CBGuildEventRequestWarehouse(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgsmEventGuild* pThis = (AgsmEventGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pagpmEventGuild->MakePacketEventGrantWarehouse(pcsEvent, pcsCharacter->m_lID, &nPacketLength);

	BOOL bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	pThis->m_pagpmEventGuild->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmEventGuild::CBGuildEventRequestWorldChampionship(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent* pcsEvent = (ApdEvent*)pData;
	AgsmEventGuild* pThis = (AgsmEventGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pagpmEventGuild->MakePacketEventGrantWorldChampionship(pcsEvent, pcsCharacter->m_lID, &nPacketLength);

	BOOL bSendResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	pThis->m_pagpmEventGuild->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}