// AgcmEventNPCDialogDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "../Resource.h"
#include "AgcmEventNPCDialogSelectTemplateDlg.h"
#include "AgcmEventNPCDialogDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


AgcmEventNPCDialogDlg::AgcmEventNPCDialogDlg()
{
	SetModuleName("AgcmEventNPCDialogDlg");

	//g_pcsTargetListDialog = NULL;
}

AgcmEventNPCDialogDlg::~AgcmEventNPCDialogDlg()
{
	/*
	if (g_pcsTargetListDialog)
		delete g_pcsTargetListDialog;
	*/
}

BOOL	AgcmEventNPCDialogDlg::OnAddModule()
{
	m_pcsApmObject = (ApmObject *)GetModule ("ApmObject");
	m_pcsAgpmEventNPCDialog = (AgpmEventNPCDialog *) GetModule("AgpmEventNPCDialog");
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsApmEventManagerDlg = (ApmEventManagerDlg *) GetModule("ApmEventManagerDlg");

	if ( !m_pcsApmObject || !m_pcsAgpmEventNPCDialog || !m_pcsApmEventManager || !m_pcsApmEventManagerDlg)
		return FALSE;

	if (!m_pcsApmEventManagerDlg->AddEventDialog(this, APDEVENT_FUNCTION_NPCDAILOG))
		return FALSE;

	return TRUE;
}

BOOL	AgcmEventNPCDialogDlg::Open(ApdEvent *pstEvent)
{
	AgcmEventNPCDialogSelectTemplateDlg	 g_csEventNPCDialogTemplateDlg;

	g_csEventNPCDialogTemplateDlg.InitData( m_pcsApmObject, m_pcsAgpmEventNPCDialog, (AgpdEventNPCDialogData *)pstEvent->m_pvData );

	g_csEventNPCDialogTemplateDlg.DoModal();

	return TRUE;
}

void	AgcmEventNPCDialogDlg::Close()
{
}
