#include "AgsmTimer.h"

AgsmTimer::AgsmTimer()
{
	SetModuleName("AgsmTimer");
	SetModuleType(APMODULE_TYPE_SERVER);

	m_pcsAgpmTimer			= NULL;
}

AgsmTimer::~AgsmTimer()
{
}

BOOL AgsmTimer::OnAddModule()
{
	m_pcsAgpmTimer			= (AgpmTimer *)				GetModule("AgpmTimer");
	m_pcsAgpmEventNature	= (AgpmEventNature *)		GetModule("AgpmEventNature");
	m_pcsAgsmCharacter		= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pcsAgsmCharManager	= (AgsmCharManager *)		GetModule("AgsmCharManager");

	if (!m_pcsAgpmTimer ||
		!m_pcsAgpmEventNature ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmCharManager)
		return FALSE;

	if (!m_pcsAgsmCharManager->SetCallbackConnectedChar(CBConnectedClient, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmTimer::OnInit()
{
	return TRUE;
}

BOOL AgsmTimer::OnDestroy()
{
	return TRUE;
}

BOOL AgsmTimer::SendGameTime(UINT32 ulNID)
{
	if (ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmTimer->MakePacketTime(&nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmTimer->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL	AgsmTimer::CBConnectedClient(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmTimer		*pThis			= (AgsmTimer *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= PtrToUint(pCustData);

	return pThis->SendGameTime(ulNID);
}