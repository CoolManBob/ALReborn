// AgcmEventQuestDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "../resource.h"
#include "AgcmEventQuestDlg2.h"
#include "EventQuest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// AgcmEventQuestDlg construction

AgcmEventQuestDlg::AgcmEventQuestDlg()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	SetModuleName("AgcmEventQuestDlg");
}

AgcmEventQuestDlg::~AgcmEventQuestDlg()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

BOOL	AgcmEventQuestDlg::OnAddModule()
{
	m_pcsApmEventManager		= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pcsApmEventManagerDlg		= (ApmEventManagerDlg *)	GetModule("ApmEventManagerDlg");
	m_pcsAgpmQuest				= (AgpmQuest*)				GetModule("AgpmQuest");

	if (!m_pcsApmEventManager || !m_pcsApmEventManagerDlg || !m_pcsAgpmQuest)
	{
		MD_SetErrorMessage( "AgcmEventQuestDlg::OnAddModule() 필요 모듈이 등록되어있지 않아용" );
		return FALSE;
	}

	// 에빈트 등록..
	if (!m_pcsApmEventManagerDlg->AddEventDialog(this, APDEVENT_FUNCTION_QUEST))
	{
		MD_SetErrorMessage( "AgcmEventQuestDlg::OnAddModule() 이벤트 등록 실패" );
		return FALSE;
	}


	return TRUE;
}

BOOL	AgcmEventQuestDlg::Open(ApdEvent *pcsEvent)
{
	// 이벤트 오픈 처리..
	CEventQuest dlg3;
	dlg3.m_pcsAgpmQuest = m_pcsAgpmQuest;
	dlg3.m_pcsEvent = pcsEvent;
	dlg3.DoModal();

	return TRUE;
}

void	AgcmEventQuestDlg::Close()
{
}
