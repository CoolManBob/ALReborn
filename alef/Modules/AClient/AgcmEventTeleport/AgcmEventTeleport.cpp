// AgcmEventTeleport.cpp: implementation of the AgcmEventTeleport class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmEventTeleport.h"
#include "AgpmArchlord.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmEventTeleport::AgcmEventTeleport()
{
	SetModuleName("AgcmEventTeleport");

	m_pcsEvent			= NULL;
	m_pcsGenerateBase	= NULL;
}

AgcmEventTeleport::~AgcmEventTeleport()
{

}

BOOL			AgcmEventTeleport::OnAddModule()
{
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgcmCharacter = (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgcmEventManager = (AgcmEventManager *) GetModule("AgcmEventManager");
	m_pcsAgpmEventTeleport = (AgpmEventTeleport *) GetModule("AgpmEventTeleport");
	m_pcsAgpmArchlord = (AgpmArchlord *) GetModule("AgpmArchlord");

	if (!m_pcsAgpmCharacter || !m_pcsAgcmCharacter || !m_pcsApmEventManager || !m_pcsAgcmEventManager || !m_pcsAgpmEventTeleport ||
		!m_pcsAgpmArchlord)
		return FALSE;

	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_TELEPORT, CBExecuteEvent, this))
		return FALSE;

	if (m_pcsAgpmEventTeleport)
	{
		if (!m_pcsAgpmEventTeleport->SetCallbackReceivedRequestResult(CBReceivedRequestResult, this))
			return FALSE;
		if (!m_pcsAgpmEventTeleport->SetCallbackTeleportLoading(CBReceiveTeleportLoading, this))
			return FALSE;
	}

	return TRUE;
}

BOOL			AgcmEventTeleport::OnInit()
{
	return TRUE;
}

BOOL			AgcmEventTeleport::OnDestroy()
{
	return TRUE;
}

BOOL			AgcmEventTeleport::CBExecuteEvent(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgcmEventTeleport *	pThis = (AgcmEventTeleport *) pvClass;
	ApdEvent *			pstEvent = (ApdEvent *) pvData;
	ApBase *			pcsGenerator = (ApBase *) pvCustData;

	if (!pstEvent || !pstEvent->m_pvData || !pcsGenerator)
		return FALSE;

	pThis->m_pcsEvent			= pstEvent;
	pThis->m_pcsGenerateBase	= pcsGenerator;

	if (pcsGenerator->m_lID == pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter->m_lID)
	{
		/*
		// 거리 체크를 한다.
		if (!pThis->m_pcsAgpmEventTeleport->CheckValidRange(pstEvent, &((AgpdCharacter *) pcsGenerator)->m_stPos))
			return TRUE;

		// UI 상에서 하나를 고르라고 한다.
		return pThis->EnumCallback(AGCMTELEPORT_CB_SELECT_TELEPORT_POINT, pstEvent->m_pvData, pstEvent);
		*/

		if (pThis->m_pcsAgpmCharacter->IsActionBlockCondition(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
			return FALSE;
		if (pThis->m_pcsAgpmCharacter->IsCombatMode(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter))
		{
			pThis->EnumCallback(AGCMTELEPORT_CB_CANNOT_USE_FOR_COMBATMODE, pcsGenerator, NULL);
			return FALSE;
		}

		// 변신중일때는 암것도 못한다. 일단...
		if (pcsGenerator->m_eType == APBASE_TYPE_CHARACTER)
		{
			if (FALSE == pThis->m_pcsAgpmArchlord->IsArchlord(pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter->m_szID))
			{
				if (((AgpdCharacter *) pcsGenerator)->m_bIsTrasform)
				{
					pThis->EnumCallback(AGCMTELEPORT_CB_CANNOT_USE_FOR_TRANSFORMING, pcsGenerator, pstEvent);
					return TRUE;
				}
			}
		}

		return pThis->m_pcsAgpmEventTeleport->SendRequestTeleport(pstEvent, &pcsGenerator->m_lID);
	}

	return TRUE;
}

BOOL AgcmEventTeleport::CBReceivedRequestResult(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	if (!pvData || !pvClass || !pvCustData)
		return FALSE;

	AgcmEventTeleport	*pThis				= (AgcmEventTeleport *)	pvClass;
	ApdEvent			*pstEvent			= (ApdEvent *)			pvData;

	INT32				lCID				= (INT32)				((PVOID *) pvCustData)[0];
	BOOL				bResult				= (BOOL)				((PVOID *) pvCustData)[1];

	AgpdCharacter		*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter || pcsSelfCharacter->m_lID != lCID)
		return FALSE;

	if (bResult)
	{
		// UI 상에서 하나를 고르라고 한다.
		return pThis->EnumCallback(AGCMTELEPORT_CB_SELECT_TELEPORT_POINT, pvCustData, pstEvent);
	}
	else
	{
	}

	return FALSE;
}

BOOL AgcmEventTeleport::CBReceiveTeleportLoading(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	if (!pvData || !pvClass)
		return FALSE;

	AgcmEventTeleport	*pThis	= (AgcmEventTeleport *)	pvClass;
	INT32				lCID	= *(INT32 *)	pvData;

	return pThis->m_pcsAgpmEventTeleport->SendTeleportLoading(lCID);
}

BOOL AgcmEventTeleport::RequestTeleport(CHAR *szPointName)
{
	if (!m_pcsEvent || !m_pcsGenerateBase || !szPointName || !szPointName[0])
		return FALSE;

	return m_pcsAgpmEventTeleport->SendTeleportInfo(m_pcsEvent, &m_pcsGenerateBase->m_lID, szPointName, NULL);
}

BOOL AgcmEventTeleport::RequestTeleportReturnOnly()
{
	if (!m_pcsEvent || !m_pcsGenerateBase)
		return FALSE;

	return m_pcsAgpmEventTeleport->SendTeleportReturn(m_pcsEvent, &m_pcsGenerateBase->m_lID);
}

BOOL AgcmEventTeleport::SetCallbackSelectTeleportPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMTELEPORT_CB_SELECT_TELEPORT_POINT, pfCallback, pClass);
}

BOOL AgcmEventTeleport::SetCallbackCannotUseForTransforming(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMTELEPORT_CB_CANNOT_USE_FOR_TRANSFORMING, pfCallback, pClass);
}

BOOL AgcmEventTeleport::SetCallbackCannotUseForCombatMode(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMTELEPORT_CB_CANNOT_USE_FOR_COMBATMODE, pfCallback, pClass);
}