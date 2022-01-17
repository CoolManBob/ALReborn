#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminManager.h"
#include "AuStrTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CAlefAdminDLLApp, CWinApp)
END_MESSAGE_MAP()

CAlefAdminDLLApp::CAlefAdminDLLApp()
{
}

CAlefAdminDLLApp theApp;

BOOL CAlefAdminDLLApp::InitInstance()
{
	CWinApp::InitInstance();
	g_AuCircularBuffer.Init(200 * 1024);
	return TRUE;
}

ADMIN_DLL BOOL ADMIN_Common_SetAdminModule(PVOID pModule)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	theApp.InitInstance();
	AlefAdminManager::Instance()->OnInit();

	return AlefAdminManager::Instance()->SetAdminModule(pModule);
}

ADMIN_DLL BOOL ADMIN_Common_DeleteInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AlefAdminManager::Instance()->DeleteInstance();
	return TRUE;
}

ADMIN_DLL BOOL ADMIN_Main_OnOpenDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AlefAdminManager::Instance()->GetAdminData()->OpenMainDlg();
}

ADMIN_DLL BOOL ADMIN_Main_OnDlgMessageProc( MSG* pMsg )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AlefAdminManager::Instance()->IsDialogMessage( pMsg );
}

ADMIN_DLL BOOL ADMIN_Main_OnIdle()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AlefAdminManager::Instance()->OnIdle();
}
