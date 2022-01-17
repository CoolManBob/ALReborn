// AgcmProcessInfoDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "resource.h"

#include "AgcmProcessInfoDlg.h"
#include "ProcessInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE AgcmProcessInfoDlgDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("AGCMPROCESSINFODLG.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(AgcmProcessInfoDlgDLL, hInstance))
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

		new CDynLinkLibrary(AgcmProcessInfoDlgDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("AGCMPROCESSINFODLG.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(AgcmProcessInfoDlgDLL);
	}
	return 1;   // ok
}

static AgcmProcessInfoDlg	*g_pcsAgcmProcessInfoDlgInst	= NULL;
static CProcessInfoDlg		*g_pcsProcessInfoDlg			= NULL;

AgcmProcessInfoDlg::AgcmProcessInfoDlg()
{
	g_pcsAgcmProcessInfoDlgInst = this;
}

AgcmProcessInfoDlg::~AgcmProcessInfoDlg()
{
}

AgcmProcessInfoDlg *AgcmProcessInfoDlg::GetInstance()
{
	return g_pcsAgcmProcessInfoDlgInst;
}

BOOL AgcmProcessInfoDlg::OpenProcessInfoDlg(CHAR *szInitInfo)
{
	if(g_pcsProcessInfoDlg)
		return FALSE;

	g_pcsProcessInfoDlg = new CProcessInfoDlg();

	g_pcsProcessInfoDlg->Create(szInitInfo);
	g_pcsProcessInfoDlg->ShowWindow(SW_SHOW);

	return TRUE;
}

BOOL AgcmProcessInfoDlg::CloseProcessInfoDlg()
{
	if(!g_pcsProcessInfoDlg)
		return FALSE;

	g_pcsProcessInfoDlg->ShowWindow(SW_HIDE);
	g_pcsProcessInfoDlg->DestroyWindow();

	delete g_pcsProcessInfoDlg;
	g_pcsProcessInfoDlg = NULL;

	return TRUE;
}

BOOL AgcmProcessInfoDlg::IsOpenProcessInfoDlg()
{
	return (g_pcsProcessInfoDlg) ? (TRUE) : (FALSE);
}

BOOL AgcmProcessInfoDlg::SetProcessInfoDlg(CHAR *szInfo)
{
	if(!g_pcsProcessInfoDlg)
		return FALSE;

	g_pcsProcessInfoDlg->SetProcessInfo(szInfo);

	return TRUE;
}

BOOL AgcmProcessInfoDlg::SetEmptyProcessInfoDlg(INT32 lMax)
{
	if(!g_pcsProcessInfoDlg)
		return FALSE;

	for(INT32 lCount = 0; lCount < lMax; ++lCount)
		g_pcsProcessInfoDlg->SetProcessInfo("");

	return TRUE;
}

BOOL AgcmProcessInfoDlg::SaveInfo(CHAR *szName)
{
	if(!g_pcsProcessInfoDlg)
		return FALSE;

	FILE *outfile = fopen(szName, "wt");
	if(!outfile)
		return FALSE;

	fprintf(outfile, "%s", (LPSTR)(LPCSTR)(g_pcsProcessInfoDlg->m_strInfo));

	fclose(outfile);

	return TRUE;
}