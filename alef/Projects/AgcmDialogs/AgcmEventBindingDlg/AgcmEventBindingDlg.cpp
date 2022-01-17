// AgcmEventBindingDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "AgcmEventBindingDlg.h"
#include "AgcmEventBindingPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AgcmEventBindingDlg::AgcmEventBindingDlg()
{
	SetModuleName("AgcmEventBindingDlg");
}

AgcmEventBindingDlg::~AgcmEventBindingDlg()
{
}

BOOL	AgcmEventBindingDlg::OnAddModule()
{
	m_pcsApmEventManager		= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pcsApmEventManagerDlg		= (ApmEventManagerDlg *)	GetModule("ApmEventManagerDlg");
	m_pcsAgpmEventBinding		= (AgpmEventBinding *)		GetModule("AgpmEventBinding");
	m_pcsAgpmFactors			= (AgpmFactors *)			GetModule("AgpmFactors");
	m_pcsApmMap					= (ApmMap *)				GetModule("ApmMap");

	if (!m_pcsApmEventManager ||
		!m_pcsApmEventManagerDlg ||
		!m_pcsAgpmEventBinding ||
		!m_pcsAgpmFactors ||
		!m_pcsApmMap)
		return FALSE;

	if (!m_pcsApmEventManagerDlg->AddEventDialog(this, APDEVENT_FUNCTION_BINDING))
		return FALSE;

	return TRUE;
}

BOOL	AgcmEventBindingDlg::Open(ApdEvent *pstEvent)
{
	AgcmEventBindingPoint	dlgBindingPoint(m_pcsApmEventManager, m_pcsAgpmEventBinding, m_pcsAgpmFactors, m_pcsApmMap, pstEvent);


	dlgBindingPoint.DoModal();

	return TRUE;
}

void	AgcmEventBindingDlg::Close()
{
}
