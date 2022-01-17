/*===================================================

	ServiceEngine.cpp :

===================================================*/

#include "ServiceEngine.h"

/********************************************/
/*		The Implementation of Global		*/
/********************************************/
//
// {ECDEC312-5858-4b22-9B22-AEB676669BC5}
const GUID	g_guidApp = 
{ 0xecdec312, 0x5858, 0x4b22, { 0x9b, 0x22, 0xae, 0xb6, 0x76, 0x66, 0x9b, 0xc5 } };

LPTSTR	g_szServiceEngineName = _T("AlefGame");
LPTSTR	g_szServiceEngineDisplayName  = _T("Archlord Game Server");

/****************************************************/
/*		The Implementation of ServiceEngine class		*/
/****************************************************/
//
ServiceEngine::ServiceEngine(LPTSTR pszName, LPTSTR pszDisplay)
	:AuService(pszName, pszDisplay), m_csEngine(g_guidApp), m_hEventStop(NULL), m_hEventStopConfirm(NULL)
{
	AuRegistry::ModuleInitialize(_T("ArchLord"));
}

ServiceEngine::~ServiceEngine()
{
}

BOOL ServiceEngine::OnInit(DWORD *pdwStatus)
{
  #ifdef _DEBUG
	//DebugBreak();
  #endif

	*pdwStatus=0;
	_stprintf(m_szNotifyPath, _T("c:\\%s.log"), m_szName);

	return TRUE;
}

void ServiceEngine::OnStop()
{
	ASSERT(NULL != m_hEventStop);
	//PostMessage(m_hwnd, WM_QUIT, (WPARAM)0, NULL);
	SetEvent(m_hEventStop);

	// 20초 기다려도 안죽으면 자살
	DWORD dwWait = WaitForSingleObject(m_hEventStopConfirm, 20 * 1000);

	if (WAIT_TIMEOUT == dwWait)
		{
		CloseHandle(m_hEventStop);
		CloseHandle(m_hEventStopConfirm);	
		ExitProcess(99);
		}
	
	CloseHandle(m_hEventStop);
	CloseHandle(m_hEventStopConfirm);
}

void ServiceEngine::OnPause()
{
}

void ServiceEngine::OnContinue()
{
}

void ServiceEngine::Run()
{
	//AllocConsole();

	SYSTEMTIME st;
	GetLocalTime(&st);

	Notify("Service Start Time : %4d-%02d-%02d-%02d %02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	CHAR szPath[_MAX_PATH] = {0, };
	m_Registry.Open(g_szServiceEngineName, FALSE);
	UINT32 ulSize = _MAX_PATH;
	m_Registry.ReadString(_T("Path"), szPath, ulSize);
	SetCurrentDirectory(szPath);

	m_hEventStop = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventStopConfirm = CreateEvent(NULL, FALSE, FALSE, NULL);

	int	i	= _set_SSE2_enable(1);

	if (i)
		printf("SSE2 enabled. \n");
	else
		printf("SSE2 not enabled; processor does not supoort SSE2.\n");

	//OpenLog(AS_LOG_FILE);
	OpenLog(AS_LOG_CONSOLE);

	m_csEngine.SetCrashReportExceptionHandler();

	m_csEngine.SetClockInterval(200);
	m_csEngine.SetNumInitGameThreads(2);

	SYSTEM_INFO		stSystemInfo;
	GetSystemInfo(&stSystemInfo);

	if (!m_csEngine.StartServer(PATCHSERVER_PORT, stSystemInfo.dwNumberOfProcessors, 5000, NULL, NULL, 100))
	{
		CoUninitialize();
		return;
	}

	if (!m_csEngine.StartProcess())
	{
		CoUninitialize();
		return;
	}

	if (!m_csEngine.OnRegisterModule())
	{
		CoUninitialize();
		return;
	}

	m_csEngine.SetServerStatus(GF_SERVER_START);

	printf("PatchServer2 is started\n");

	HANDLE	m_ahEventStop[2];
	m_ahEventStop[0]	= m_hEventStop;
	m_ahEventStop[1]	= m_csEngine.m_hEventStop;

	WaitForMultipleObjects(2, m_ahEventStop, FALSE, INFINITE);

	CloseLog();

	CoUninitialize();
		
	return;
}

void ServiceEngine::OnInstall()
{
	CHAR szPath[_MAX_PATH] = {0, };
	GetCurrentDirectory(_MAX_PATH, szPath);
	m_Registry.Open(g_szServiceEngineName, TRUE);
	m_Registry.WriteString(_T("Path"), szPath);
}

void ServiceEngine::OnUninstall()
{
}

