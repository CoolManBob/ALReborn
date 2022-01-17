/*===================================================

	ServiceEngine.cpp :

===================================================*/

#include "ServiceEngine.h"

/********************************************/
/*		The Implementation of Global		*/
/********************************************/
//
GUID	g_guidApp = { 0xede9493e, 0x6ac8, 0x4f15, { 0x8d, 0x1, 0x8b, 0x16, 0x32, 0x0, 0xb9, 0x66 } };
LPTSTR	g_szServiceEngineName = _T("AlefLogin");
LPTSTR	g_szServiceEngineDisplayName  = _T("Archlord Login Server");

extern char *	g_szIPBlockFileName;
extern LPCTSTR g_szOptionIPBlock;

/****************************************************/
/*		The Implementation of ServiceEngine class		*/
/****************************************************/
//
ServiceEngine::ServiceEngine(LPTSTR pszName, LPTSTR pszDisplay)
	:AuService(pszName, pszDisplay), m_csEngine(g_guidApp), m_hEventStop(NULL), m_hEventStopConfirm(NULL)
	{
	AuRegistry::ModuleInitialize(_T("ArchLord"));
	m_szIPBlockFileName = NULL;
	}

ServiceEngine::~ServiceEngine()
	{
	}

void ServiceEngine::OnSetArguments(int argc, LPTSTR *argv)
	{
		int i;

		for (i = 0; i < argc; ++i)
		{
			if (!stricmp(argv[i], g_szOptionIPBlock))
			{
				m_szIPBlockFileName = g_szIPBlockFileName;
				m_csEngine.m_bIPBlock = TRUE;

			}
		}
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

	// 30초 기다려도 안죽으면 자살
	DWORD dwWait = WaitForSingleObject(m_hEventStopConfirm, 30 * 1000);

	if (WAIT_TIMEOUT == dwWait)
		ExitProcess(99);
	
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

#ifndef _M_X64
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
#endif

	CHAR szPath[_MAX_PATH] = {0, };
	m_Registry.Open(g_szServiceEngineName, FALSE);
	UINT32 ulSize = _MAX_PATH;
	m_Registry.ReadString(_T("Path"), szPath, ulSize);
	SetCurrentDirectory(szPath);

	m_hEventStop = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventStopConfirm = CreateEvent(NULL, FALSE, FALSE, NULL);

	//OpenLog(AS_LOG_FILE);
	OpenLog(AS_LOG_CONSOLE);

	m_csEngine.SetCrashReportExceptionHandler();

	// (port, connect timeout, connect retries, timeout until keep alive)
//	if (!m_csEngine.StartDP(9991, 15000, 2, 15000)) //LoginServer라 9991이다.

	/*
	char			*pstrIPBlockResFileName;
	char			*pstrPCRoomIPResFileName;
	bool			bUseIPBlock;
	bool			bOpenPCRoom;

	pstrIPBlockResFileName = NULL;
	pstrPCRoomIPResFileName = NULL;
	bUseIPBlock = false;
	bOpenPCRoom = false;

	m_csEngine.OpenIPInfo( bUseIPBlock, bOpenPCRoom );

	if( bUseIPBlock )
	{
		pstrIPBlockResFileName = "Ini\\IPBlockData.txt";
	}
	if( bOpenPCRoom )
	{
		pstrPCRoomIPResFileName = "Ini\\PCRoomIPData.txt";
	}
	*/

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "Before OnRegisterModule\n");
	AuLogFile_s("LOG\\Startup.log", strCharBuff);

	if (!m_csEngine.OnRegisterModule())
	{
		//WriteLog(AS_LOG_RELEASE, "AlefAuctionServer : failed OnRegisterModule()");

		CoUninitialize();
		return;
	}

	char strCharBuff2[256] = { 0, };
	sprintf_s(strCharBuff2, sizeof(strCharBuff2), "Before StartServer\n");
	AuLogFile_s("LOG\\Startup.log", strCharBuff2);

	//if (!m_csEngine.StartServer( 11002, 4, 3000, pstrIPBlockResFileName, pstrPCRoomIPResFileName, 100 ))
	if (!m_csEngine.StartServer( 11002, 4, 3000, m_szIPBlockFileName, NULL, 50 ))
	{
		CoUninitialize();
		return;
	}

	char strCharBuff3[256] = { 0, };
	sprintf_s(strCharBuff3, sizeof(strCharBuff3), "Before InitServerManager\n");
	AuLogFile_s("LOG\\Startup.log", strCharBuff3);

	if (!m_csEngine.InitServerManager())
		{
			CoUninitialize();
			return;
		}

	char strCharBuff4[256] = { 0, };
	sprintf_s(strCharBuff4, sizeof(strCharBuff4), "Before StartProcess\n");
	AuLogFile_s("LOG\\Startup.log", strCharBuff4);

	if (!m_csEngine.StartProcess())
	{
		//WriteLog(AS_LOG_RELEASE, "AlefAuctionServer : failed StartProcess()");

		CoUninitialize();
		return;
	}

	char strCharBuff5[256] = { 0, };
	sprintf_s(strCharBuff5, sizeof(strCharBuff5), "Before ConnectServers\n");
	AuLogFile_s("LOG\\Startup.log", strCharBuff5);

	if (!m_csEngine.ConnectServers())
	{
		CoUninitialize();
		return;
	}

	//스레드 생성을 위해 기다려준다. 
	Sleep( 1000 );


	m_csEngine.SetServerStatus(GF_SERVER_START);

	char strCharBuff6[256] = { 0, };
	sprintf_s(strCharBuff6, sizeof(strCharBuff6), "Before WaitForSingleObject\n");
	AuLogFile_s("LOG\\Startup.log", strCharBuff6);

	WaitForSingleObject(m_hEventStop, INFINITE);

	m_csEngine.SetServerStatus(GF_SERVER_STOP);

	m_csEngine.DisconnectServers();

	m_csEngine.StopProcess();

	m_csEngine.OnTerminate();

	//모든쓰레드가 죽을때까지 기다려준다.
	m_csEngine.WaitDBThreads();

	m_csEngine.Stop();

	CloseLog();
	
    CoUninitialize();

	SetEvent(m_hEventStopConfirm);
	
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

