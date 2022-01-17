// AgcmEventSpawnDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "AgcmEventSpawnDlg.h"
#include "AgcmEventSpawnSpawnDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AgcmEventSpawnSpawnDlg *	g_pcsSpawnDialog;

AgcmEventSpawnDlg::AgcmEventSpawnDlg()
{
	SetModuleName("AgcmEventSpawnDlg");

	g_pcsSpawnDialog = NULL;

	m_pcsApmEventManager		=	NULL;
	m_pcsApmEventManagerDlg		=	NULL;
	m_pcsAgpmEventSpawn			=	NULL;
	m_pcsAgpmCharacter			=	NULL;
	m_pcsAgpmItem				=	NULL;
	m_pcsAgpmSkill				=	NULL;
	m_pcsAgpmDropItem			=	NULL;
	m_pcsAgcmFactorsDlg			=	NULL;
	m_pcsAgpmAI2				=	NULL;
}

AgcmEventSpawnDlg::~AgcmEventSpawnDlg()
{
	DEF_SAFEDELETE( g_pcsSpawnDialog );
}

BOOL	AgcmEventSpawnDlg::OnAddModule()
{
	m_pcsApmEventManager	= static_cast< ApmEventManager		* >(	GetModule("ApmEventManager")		);
	m_pcsApmEventManagerDlg = static_cast< ApmEventManagerDlg	* >(	GetModule("ApmEventManagerDlg")		);
	m_pcsAgpmEventSpawn		= static_cast< AgpmEventSpawn		* >(	GetModule("AgpmEventSpawn")			);
	m_pcsAgpmCharacter		= static_cast< AgpmCharacter		* >(	GetModule("AgpmCharacter")			);
	m_pcsAgpmItem			= static_cast< AgpmItem				* >(	GetModule("AgpmItem")				);
	m_pcsAgpmSkill			= static_cast< AgpmSkill			* >(	GetModule("AgpmSkill")				);
	m_pcsAgpmDropItem		= static_cast< AgpmDropItem			* >(	GetModule("AgpmDropItem")			);
	m_pcsAgcmFactorsDlg		= static_cast< AgcmFactorsDlg		* >(	GetModule("AgcmFactorsDlg")			);

	//m_pcsAgpmAI = (AgpmAI *) GetModule("AgpmAI");
	//m_pcsAgcmAIDlg = (AgcmAIDlg *) GetModule("AgcmAIDlg");

	m_pcsAgpmAI2	= (AgpmAI2 *) GetModule("AgpmAI2");

	if (!m_pcsApmEventManager || !m_pcsApmEventManagerDlg || !m_pcsAgpmEventSpawn || !m_pcsAgpmCharacter || 
		!m_pcsAgpmEventSpawn || !m_pcsAgpmItem || !m_pcsAgpmSkill || !m_pcsAgpmDropItem || //!m_pcsAgpmAI || !m_pcsAgcmAIDlg || 
		!m_pcsAgpmAI2 )
		return FALSE;

	if (!m_pcsApmEventManagerDlg->AddEventDialog(this, APDEVENT_FUNCTION_SPAWN))
		return FALSE;

	return TRUE;
}

BOOL	AgcmEventSpawnDlg::Open(ApdEvent *pstEvent)
{
	AgcmEventSpawnSpawnDlg	dlgSpawn;

	if (!pstEvent)								return FALSE;
	if (!pstEvent || !pstEvent->m_pvData)		return FALSE;
	/*
	if (!g_pcsSpawnDialog)
	{
		g_pcsSpawnDialog = new AgcmEventSpawnSpawnDlg;
		g_pcsSpawnDialog->Create();
		g_pcsSpawnDialog->SetModule(this);
		g_pcsSpawnDialog->Init();
	}

	if (!g_pcsSpawnDialog->InitData(pstEvent))
		return FALSE;

	g_pcsSpawnDialog->ShowWindow(SW_SHOW);
	*/

	dlgSpawn.SetModule(this, m_pcsAgpmCharacter, m_pcsAgpmItem, m_pcsAgpmSkill, m_pcsAgpmDropItem, m_pcsAgpmAI2 );
	dlgSpawn.Init();
	dlgSpawn.InitData(pstEvent);

	if (dlgSpawn.DoModal() == IDOK)
	{
	}

	return TRUE;
}

void	AgcmEventSpawnDlg::Close()
{
	g_pcsSpawnDialog->CloseWindow();
}
