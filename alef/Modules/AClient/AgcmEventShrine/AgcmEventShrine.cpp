/******************************************************************************
Module:  AgcmEventShrine.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 25
******************************************************************************/

#include "AgcmEventShrine.h"

AgcmEventShrine::AgcmEventShrine()
{
	SetModuleName("AgcmEventShrine");
}

AgcmEventShrine::~AgcmEventShrine()
{
}

BOOL AgcmEventShrine::OnAddModule		(						)
{
	m_pcsAgpmShrine			= (AgpmShrine *)		GetModule("AgpmShrine");
	m_pcsAgcmEventManager	= (AgcmEventManager *)	GetModule("AgcmEventManager");

	if (!m_pcsAgpmShrine || !m_pcsAgcmEventManager)
		return FALSE;

	return TRUE;
}

BOOL			AgcmEventShrine::OnInit			(						)
{
	return TRUE;
}

BOOL			AgcmEventShrine::OnDestroy		(						)
{
	return TRUE;
}

BOOL		AgcmEventShrine::CBActiveEvent	( PVOID pData, PVOID pClass, PVOID pCustData	)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventShrine	*pThis			= (AgcmEventShrine *)	pClass;
	ApdEvent		*pcsEvent		= (ApdEvent *)			pData;
	ApBase			*pcsGenerator	= (ApBase *)			pCustData;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = pThis->m_pcsAgpmShrine->MakePacketGenerateShrineEvent(pcsEvent, pcsGenerator, &nPacketLength);

	if (!pvPacket || !nPacketLength)
		return FALSE;

	BOOL	bResult = TRUE;

	bResult = pThis->SendPacket(pvPacket, nPacketLength);

	pThis->m_pcsAgpmShrine->m_csPacket.FreePacket(pvPacket);

	return bResult;
}