// AlefAdminAPI.h
// (C) NHN - ArchLord Development Team
// steeple, 2005. 02. 16.

// 다른 프로젝트에서 참조하기 때문에..
// 대략 API 비스므리한 것이 온다.

#ifdef _ADMIN_EXPORT_
	#define ADMIN_DLL extern "C" __declspec(dllexport)
#else
	#define ADMIN_DLL extern "C" __declspec(dllimport)
#endif

// Callback Functions Define
typedef BOOL (*ADMIN_CB)(PVOID pData, PVOID pClass, PVOID pCustData);

// Common
ADMIN_DLL BOOL ADMIN_Common_SetAdminModule(PVOID pModule);
ADMIN_DLL BOOL ADMIN_Common_DeleteInstance();

// Main
ADMIN_DLL BOOL ADMIN_Main_OnOpenDlg();
ADMIN_DLL BOOL ADMIN_Main_OnDlgMessageProc( MSG* pMsg );
ADMIN_DLL BOOL ADMIN_Main_OnIdle();
