// AgcmAIDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#include "AgcmAIDlg.h"
#include "AgcmAIDialog.h"
#include "AgcmAITemplateDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AgcmAIDialog *			g_pcsAIDialog;
AgcmAITemplateDialog *	g_pcsAITemplateDialog;

static AFX_EXTENSION_MODULE AgcmAIDlgDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("AGCMAIDLG.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(AgcmAIDlgDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(AgcmAIDlgDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("AGCMAIDLG.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(AgcmAIDlgDLL);
	}
	return 1;   // ok
}

AgcmAIDlg::AgcmAIDlg()
{
	SetModuleName("AgcmAIDlg");

	g_pcsAIDialog = NULL;
	g_pcsAITemplateDialog = NULL;
}

AgcmAIDlg::~AgcmAIDlg()
{
	if (g_pcsAIDialog)
		delete g_pcsAIDialog;

	if (g_pcsAITemplateDialog)
		delete g_pcsAITemplateDialog;
}

BOOL	AgcmAIDlg::OnAddModule()
{
	m_pcsAgpmAI = (AgpmAI *) GetModule("AgpmAI");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgcmFactorsDlg = (AgcmFactorsDlg *) GetModule("AgcmFactorsDlg");

	if (!m_pcsAgpmAI || !m_pcsAgpmCharacter)
		return FALSE;

	return TRUE;
}

BOOL	AgcmAIDlg::OpenTemplates(BOOL bSelect)
{
	/*
	if (!g_pcsAITemplateDialog)
	{
		g_pcsAITemplateDialog = new AgcmAITemplateDialog;
		g_pcsAITemplateDialog->Create();
		g_pcsAITemplateDialog->Init();
		g_pcsAITemplateDialog->SetModule(m_pcsAgpmAI, this);
	}

	if (!g_pcsAITemplateDialog->InitData(bSelect))
		return FALSE;

	g_pcsAITemplateDialog->ShowWindow(SW_SHOW);
	*/
	AgcmAITemplateDialog	dlgAITemplates;

	dlgAITemplates.Init();
	dlgAITemplates.SetModule(m_pcsAgpmAI, this);
	dlgAITemplates.InitData(bSelect);

	if (dlgAITemplates.DoModal() == IDOK)
	{
	}

	return TRUE;
}

BOOL	AgcmAIDlg::OpenTemplate(AgpdAITemplate *pstTemplate)
{
	AgcmAIDialog	dlgAI;

	dlgAI.Init();
	dlgAI.SetModule(m_pcsAgpmAI, this, m_pcsAgpmCharacter, m_pcsAgpmItem );
	dlgAI.InitData(pstTemplate);

	if (dlgAI.DoModal() == IDOK)
	{
	}

	return TRUE;
}

void	AgcmAIDlg::Close()
{
	g_pcsAIDialog->CloseWindow();
}

BOOL	AgcmAIDlg::SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(0, pfCallback, pClass);
}
