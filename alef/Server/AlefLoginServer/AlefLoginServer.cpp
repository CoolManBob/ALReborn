/******************************************************************************
Module:  AlefAuctionServer.cpp
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 02. 17
******************************************************************************/

#include "ApBase.h"

#include <stdio.h>
#include "objbase.h"
#include "ServerEngine.h"
#include "ServiceEngine.h"
#include "ApMemory.h"

#include "ApMemoryTracker.h"

char *	g_szIPBlockFileName = "Ini\\IPBlockData.txt";
LPCTSTR g_szOptionIPBlock = "IPBlock";

extern AgsmAdmin* g_pcsAgsmAdmin;
extern AgsmLoginClient* g_pcsAgsmLoginClient;

//#define _ALEF_SERVICE_MODE

int DoConsole();

int main(int argc, char* argv[])
	{

	// Check Process is Running(Allow only one Server) 
	if(::IsProcessRun("LoginServer"))
	{
		printf("This Program is already Running!!!");
		exit(1);
	}

  // Console(Non-service) mode
  #ifndef		_ALEF_SERVICE_MODE
	DoConsole();

  // Service mode
  #else		

	ServiceEngine AlefService(g_szServiceEngineName, g_szServiceEngineDisplayName);
	if (FALSE == AlefService.ParseArguments(argc,argv))
		{
		AlefService.Start();
		}	

  #endif
  
#ifdef	_AP_MEMORY_TRACKER_
	ApMemoryTracker::GetInstance().ReportLeaks(REPORT_MODE_FILE);
	ApMemoryTracker::DestroyInstance();
#endif	//_AP_MEMORY_TRACKER_
 
	return 0;
 }

int DoConsole()
{

#ifndef _M_X64
    // Init COM so we can use CoCreateInstance
    CoInitializeEx( NULL, COINIT_MULTITHREADED );
#endif

	//OpenLog(AS_LOG_FILE);
	OpenLog(AS_LOG_CONSOLE);

	ServerEngine serverEngine(g_guidApp);

	serverEngine.SetCrashReportExceptionHandler();

	// (port, connect timeout, connect retries, timeout until keep alive)
//	if (!serverEngine.StartDP(9991, 15000, 2, 15000)) //LoginServer라 9991이다.



	if (!serverEngine.StartServer( 11002, 4, 3000, NULL, NULL, 50 ))
	{
		CoUninitialize();
		return 0;
	}

	if (!serverEngine.OnRegisterModule())
	{
		//WriteLog(AS_LOG_RELEASE, "AlefAuctionServer : failed OnRegisterModule()");

		CoUninitialize();
		return 0;
	}

	if (!serverEngine.StartProcess())
	{
		//WriteLog(AS_LOG_RELEASE, "AlefAuctionServer : failed StartProcess()");

		CoUninitialize();
		return 0;
	}

	if (!serverEngine.ConnectServers())
	{
		CoUninitialize();
		return 0;
	}

	//스레드 생성을 위해 기다려준다. 
	Sleep( 1000 );

	serverEngine.SetServerStatus(GF_SERVER_START);

	fprintf(stdout, "AlefLoginServer is started\n");
	fflush(stdout);

	while (char c = getchar())
	{
		BOOL	bExitLoop	= FALSE;

		switch(c)
		{
		case 'm':
			{
				ApMemoryManager::GetInstance().ReportMemoryInfo();
				printf("ApMemory에서 사용중인 내용들을 AlefLoginServerD.exe_ApMemory.log에 출력했습니다.");
			}
			break;

		case 'o':	// Set Connectable Account Level
			{
				INT32 lLevel = 0;
				printf("Current connectable account level :: %d\n", g_pcsAgsmAdmin->GetConnectableAccountLevel());
				printf("Input new connectable account level. ex) 0 (Tester), 88 (Admin) : ");
				scanf("%d", &lLevel);

				g_pcsAgsmAdmin->SetConnectableAccountLevel(lLevel);
			}
			break;

		case 'c':
			{
				INT32 lMaxProcessUserCount = 0;
				printf("Current max user process count :: %d\n", g_pcsAgsmLoginClient->m_lMaxProcessUserCount);
				printf("Enter new max process count :: ");
				scanf("%d", &lMaxProcessUserCount);

				if (lMaxProcessUserCount > 0 &&
					lMaxProcessUserCount < 10000)
					g_pcsAgsmLoginClient->m_lMaxProcessUserCount	= lMaxProcessUserCount;
			}
			break;

		case 'z':
			{
				bExitLoop	= TRUE;
			}
			break;
		}

		if (bExitLoop)
			break;
	}

	serverEngine.SetServerStatus(GF_SERVER_STOP);

	serverEngine.DisconnectServers();

	//모든쓰레드가 죽을때까지 기다려준다.
	serverEngine.WaitDBThreads();

	serverEngine.StopProcess();



	serverEngine.OnTerminate();

	serverEngine.Stop();

	CloseLog();
	
    CoUninitialize();

	return 0;
}

