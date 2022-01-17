// AgcmEventProductDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "AgcmEventProductDlg.h"
#include "EventProduct.h"

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
// AgcmEventProductDlg construction

AgcmEventProductDlg::AgcmEventProductDlg()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	SetModuleName("AgcmEventProductDlg");
}

AgcmEventProductDlg::~AgcmEventProductDlg()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

BOOL	AgcmEventProductDlg::OnAddModule()
{
	m_pApmEventManager		= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pApmEventManagerDlg	= (ApmEventManagerDlg *)	GetModule("ApmEventManagerDlg");
	m_pAgpmProduct			= (AgpmProduct *)			GetModule("AgpmProduct");

	if (!m_pApmEventManager || !m_pApmEventManagerDlg || !m_pAgpmProduct)
	{
		MD_SetErrorMessage( "AgcmEventProductDlg::OnAddModule() 필요 모듈이 등록되어있지 않아용" );
		return FALSE;
	}

	// register event
	if (!m_pApmEventManagerDlg->AddEventDialog(this, APDEVENT_FUNCTION_PRODUCT))
	{
		MD_SetErrorMessage( "AgcmEventProductDlg::OnAddModule() 이벤트 등록 실패" );
		return FALSE;
	}


	return TRUE;
}

BOOL	AgcmEventProductDlg::Open(ApdEvent *pApdEvent)
{
	// open
	CEventProduct dlg;

	dlg.m_pAgpmProduct = m_pAgpmProduct;
	dlg.m_pApdEvent = pApdEvent;
	dlg.DoModal();

	return TRUE;
}

void	AgcmEventProductDlg::Close()
{
}
