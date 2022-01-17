// AgcmEventSkillMasterDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "../resource.h"		// main symbols

#include "AgcmEventSkillMasterDlg.h"
#include "AgcmEventSkillEventSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AgcmEventSkillMasterDlg::AgcmEventSkillMasterDlg()
{
	SetModuleName("AgcmEventSkillMasterDlg");

	m_pcsApmEventManager		= NULL;
	m_pcsApmEventManagerDlg		= NULL;
	m_pcsAgpmEventSkillMaster	= NULL;
}

AgcmEventSkillMasterDlg::~AgcmEventSkillMasterDlg()
{
}

BOOL	AgcmEventSkillMasterDlg::OnAddModule()
{
	m_pcsApmEventManager		= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pcsApmEventManagerDlg		= (ApmEventManagerDlg *)	GetModule("ApmEventManagerDlg");
	m_pcsAgpmEventSkillMaster	= (AgpmEventSkillMaster *)	GetModule("AgpmEventSkillMaster");

	if (!m_pcsApmEventManager || !m_pcsApmEventManagerDlg || !m_pcsAgpmEventSkillMaster)
		return FALSE;

	if (!m_pcsApmEventManagerDlg->AddEventDialog(this, APDEVENT_FUNCTION_SKILLMASTER))
		return FALSE;

	return TRUE;
}

BOOL	AgcmEventSkillMasterDlg::Open(ApdEvent *pcsEvent)
{
	AgcmEventSkillEventSettingDlg	dlgSkillMaster(m_pcsAgpmEventSkillMaster, pcsEvent);

	dlgSkillMaster.DoModal();

	return TRUE;
}

void	AgcmEventSkillMasterDlg::Close()
{
}


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
//
/////////////////////////////////////////////////////////////////////////////
// CAgcmEventSkillMasterDlgApp
//
//BEGIN_MESSAGE_MAP(CAgcmEventSkillMasterDlgApp, CWinApp)
//	//{{AFX_MSG_MAP(CAgcmEventSkillMasterDlgApp)
//		// NOTE - the ClassWizard will add and remove mapping macros here.
//		//    DO NOT EDIT what you see in these blocks of generated code!
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
/////////////////////////////////////////////////////////////////////////////
// CAgcmEventSkillMasterDlgApp construction
//
//CAgcmEventSkillMasterDlgApp::CAgcmEventSkillMasterDlgApp()
//{
//	// TODO: add construction code here,
//	// Place all significant initialization in InitInstance
//}
//
/////////////////////////////////////////////////////////////////////////////
// The one and only CAgcmEventSkillMasterDlgApp object
//
//CAgcmEventSkillMasterDlgApp theApp;
