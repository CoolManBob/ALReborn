// PatchClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PatchClient.h"
#include "PatchClientDlg.h"
#include "AuSingleProcessChecker.h"
#include "AuProcessManager.h"
#include "LangControl.h"

#include ".//BugSlay/CrashHandler.h"

#ifdef _JPN
#include "AuJapaneseClientAuth.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "XmlLogger/XmlLogger.h"
#include "AutoDetectMemoryLeak.h"

CXmlLogger* g_pLogger = NULL;

void DeleteOldFiles(void);

/////////////////////////////////////////////////////////////////////////////
// CPatchClientApp

BEGIN_MESSAGE_MAP(CPatchClientApp, CWinApp)
	//{{AFX_MSG_MAP(CPatchClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchClientApp construction

CPatchClientApp::CPatchClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	//_CrtSetBreakAlloc(224182);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPatchClientApp object

CPatchClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPatchClientApp initialization

BOOL CPatchClientApp::InitInstance()
{
	//SetCrashHandlerFilter( ExcepCallBack );


#ifdef _DEBUG
	AuSingleProcessChecker csSingleProc("ArchlordD.exe", "Archlord Patch Client");
#else
	AuSingleProcessChecker csSingleProc("Archlord.exe", "Archlord Patch Client");
#endif
	
	if (!csSingleProc.IsAlreadyRunning())
	{
		if (!csSingleProc.InitChecker())
			return FALSE;
	}
	else
	{
		csSingleProc.ActivateFirstApplication();
		return FALSE;
	}

	//AuProcessManager::StopProcess(PATCHCLIENT_GAMECLIENT_FILE);

	AfxEnableControlContainer();

	// 리소스 핸들.
	g_resInst = ::AfxGetApp()->m_hInstance;
	g_lt = GetLanguageType();

	// Option 확인
	LPTSTR	szCmdLine = GetCommandLine();

#ifdef _ENG
	SetResource();
	DeleteOldFiles();
	
#endif

#ifdef _TWI
	SetResource();
	DeleteOldFiles();
#endif
	
	

#ifdef _JPN
	if ( !g_jAuth.Init( szCmdLine ) )
	{
		return FALSE;
	}
#endif

#ifdef _ENG
	CHAR	Seps[]				=	".";
	CHAR	Sep2[]				=	" ";
	LPSTR	lpCommandLine		=	::GetCommandLine();
	CHAR*	Token				=	NULL;
	CHAR*	Token2				=	NULL;
	CHAR	strToken[ 4096 ] = { 0, };
	strcpy_s( strToken , 4096 , lpCommandLine );

	Token = strtok( strToken, Seps ); 
	Token = strtok( NULL, Seps ); 

	Token2 = strtok( Token , Sep2 ); 
	Token2 = strtok( NULL , Sep2 );

	if( !Token2 )
	{
		::ShellExecute( NULL, "open", "http://archlord.webzen.com/", NULL, NULL, SW_SHOW );
		return FALSE;
	}
#endif

	CPatchClientDlg dlg;

	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	HRESULT	dwError		=	GetLastError();

	return FALSE;
}

void CPatchClientApp::SetResource()
{
	HINSTANCE resModule = NULL;

	if (g_lt == LT_FRENCH)
	{
		SetThreadLocale(MAKELCID(MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH), SORT_DEFAULT));
		resModule = LoadLibrary(".\\French.dll");
	}
	else if (g_lt == LT_GERMAN)
	{
		SetThreadLocale(MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), SORT_DEFAULT));
		resModule = LoadLibrary(".\\German.dll");
	}

	if (g_lt != LT_ENGLISH)
	{
		g_resInst = resModule;
		AfxSetResourceHandle(resModule);
	}
}

void DeleteOldFiles()
{
}
