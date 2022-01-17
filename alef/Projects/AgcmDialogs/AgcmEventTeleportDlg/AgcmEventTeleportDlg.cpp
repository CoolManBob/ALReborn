// AgcmEventTeleportDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "AgcmEventTeleportDlg.h"
#include "AgcmTeleportTarget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//AgcmTeleportTargetList *	g_pcsTargetListDialog;


AgcmEventTeleportDlg::AgcmEventTeleportDlg()
{
	SetModuleName("AgcmEventTeleportDlg");

	//g_pcsTargetListDialog = NULL;
}

AgcmEventTeleportDlg::~AgcmEventTeleportDlg()
{
	/*
	if (g_pcsTargetListDialog)
		delete g_pcsTargetListDialog;
	*/
}

BOOL	AgcmEventTeleportDlg::OnAddModule()
{
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsApmEventManagerDlg = (ApmEventManagerDlg *) GetModule("ApmEventManagerDlg");
	m_pcsAgpmEventTeleport = (AgpmEventTeleport *) GetModule("AgpmEventTeleport");

	if (!m_pcsApmEventManager || !m_pcsApmEventManagerDlg || !m_pcsAgpmEventTeleport)
		return FALSE;

	if (!m_pcsApmEventManagerDlg->AddEventDialog(this, APDEVENT_FUNCTION_TELEPORT))
		return FALSE;

	return TRUE;
}

BOOL	AgcmEventTeleportDlg::Open(ApdEvent *pstEvent)
{
	AgcmTeleportTarget	dlgTarget(m_pcsAgpmEventTeleport);

	dlgTarget.InitData(pstEvent, ((AgpdTeleportAttach *) pstEvent->m_pvData)->m_pcsPoint);

	dlgTarget.DoModal();

	return TRUE;
}

void	AgcmEventTeleportDlg::Close()
{
	//g_pcsTargetListDialog->CloseWindow();
}
