// AgcmEventAuctionDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "../Resource.h"
#include "AgcmEventAuctionDlgDialog.h"
#include "AgcmEventAuctionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


AgcmEventAuctionDlg::AgcmEventAuctionDlg()
{
	SetModuleName("AgcmEventAuctionDlg");

	//g_pcsTargetListDialog = NULL;
}

AgcmEventAuctionDlg::~AgcmEventAuctionDlg()
{
	/*
	if (g_pcsTargetListDialog)
		delete g_pcsTargetListDialog;
	*/
}

BOOL	AgcmEventAuctionDlg::OnAddModule()
{
	m_pcsApmObject = (ApmObject *)GetModule ("ApmObject");
	m_pcsAgpmCharacter = (AgpmCharacter *)GetModule( "AgpmCharacter" );
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsApmEventManagerDlg = (ApmEventManagerDlg *) GetModule("ApmEventManagerDlg");

	if ( !m_pcsApmObject || !m_pcsAgpmCharacter || !m_pcsApmEventManager || !m_pcsApmEventManagerDlg)
		return FALSE;

	if (!m_pcsApmEventManagerDlg->AddEventDialog(this, APDEVENT_FUNCTION_AUCTION))
		return FALSE;

	return TRUE;
}

BOOL	AgcmEventAuctionDlg::Open(ApdEvent *pstEvent)
{
	CAgcmEventAuctionDlgDialog	 csEventAuctionDlgDialog;

	csEventAuctionDlgDialog.InitData( m_pcsApmObject, m_pcsAgpmCharacter, (AgpdEventAuction *)pstEvent->m_pvData );

	csEventAuctionDlgDialog.DoModal();

	return TRUE;
}

void	AgcmEventAuctionDlg::Close()
{
}
