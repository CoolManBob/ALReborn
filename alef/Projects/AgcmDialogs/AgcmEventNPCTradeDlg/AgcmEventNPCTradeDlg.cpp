// AgcmEventNPCTradeDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "../Resource.h"
#include "AgcmEventSelectNPCTradeTemplateDlg.h"
#include "AgcmEventNPCTradeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AgcmEventNPCTradeDlg::AgcmEventNPCTradeDlg()
{
	SetModuleName("AgcmEventNPCTradeDlg");

	//g_pcsTargetListDialog = NULL;
}

AgcmEventNPCTradeDlg::~AgcmEventNPCTradeDlg()
{
	/*
	if (g_pcsTargetListDialog)
		delete g_pcsTargetListDialog;
	*/
}

BOOL	AgcmEventNPCTradeDlg::OnAddModule()
{
	m_pcsApmObject = (ApmObject *)GetModule ("ApmObject");
	m_pcsAgpmCharacter = (AgpmCharacter *)GetModule( "AgpmCharacter" );
	m_pcsAgpmEventNPCTrade = (AgpmEventNPCTrade *) GetModule("AgpmEventNPCTrade");
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsApmEventManagerDlg = (ApmEventManagerDlg *) GetModule("ApmEventManagerDlg");

	if ( !m_pcsApmObject || !m_pcsAgpmCharacter || !m_pcsAgpmEventNPCTrade || !m_pcsApmEventManager || !m_pcsApmEventManagerDlg)
		return FALSE;

	if (!m_pcsApmEventManagerDlg->AddEventDialog(this, APDEVENT_FUNCTION_NPCTRADE))
		return FALSE;

	return TRUE;
}

BOOL	AgcmEventNPCTradeDlg::Open(ApdEvent *pstEvent)
{
	AgcmEventSelectNPCTradeTemplateDlg	 g_csEventNPCTradeTemplateDlg;

	g_csEventNPCTradeTemplateDlg.InitData( m_pcsApmObject, m_pcsAgpmCharacter, m_pcsAgpmEventNPCTrade, (AgpdEventNPCTradeData *)pstEvent->m_pvData );

	g_csEventNPCTradeTemplateDlg.DoModal();

	return TRUE;
}

void	AgcmEventNPCTradeDlg::Close()
{
}
