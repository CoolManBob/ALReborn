/******************************************************************************
Module:  AlefServer.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 03
******************************************************************************/

#include "ApBase.h"
#include <stdio.h>
#include <math.h>
#include "objbase.h"
 
#include "ServerEngine.h"
#include "ServiceEngine.h"
#include "ApMemory.h"
#include "ApMemoryChecker.h"

#include "ApMemoryTracker.h"


//GUID			g_guidApp = { 0xede9493e, 0x6ac8, 0x4f15, { 0x8d, 0x1, 0x8b, 0x16, 0x32, 0x0, 0xb9, 0x66 } };

extern AgsmInterServerLink*	g_pcsAgsmInterServerLink;
extern AgsmCharManager*		g_pcsAgsmCharManager;
extern AgsmCharacter*		g_pcsAgsmCharacter;
extern AgpmLog*				g_pcsAgpmLog;

extern BOOL					g_bPrintNumCharacter;
extern BOOL					g_bPrintActiveCellCount;
extern BOOL					g_bPrintRemovePoolTID;

//extern	UINT64	g_ullTotalMakePacketSize;
//extern	UINT64	g_ullCompressedMakePacketSize;

//#define _ALEF_SERVICE_MODE

int DoConsole();

int main(int argc, char* argv[])
{
	// Check Process is Running(Allow only one Server) 
	if(::IsProcessRun("GameServer"))
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
	int	iSSE2Enable	= _set_SSE2_enable(1);
	if (iSSE2Enable)
		printf("SSE2 enabled. \n");
	else
		printf("SSE2 not enabled; processor does not supoort SSE2.\n");
#endif

    // Init COM so we can use CoCreateInstance
#ifndef _WIN64
    HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
#endif

	//OpenLog(AS_LOG_FILE);
	OpenLog(AS_LOG_CONSOLE);

	ServerEngine serverEngine;

	serverEngine.SetCrashReportExceptionHandler();

	serverEngine.SetClockInterval(200);
	serverEngine.SetNumInitGameThreads(2);

	SYSTEM_INFO		stSystemInfo;
	GetSystemInfo(&stSystemInfo);

	printf("serverEngine.StartServer()\n");
	if (!serverEngine.StartServer(11008, stSystemInfo.dwNumberOfProcessors, MAX_CONNECTION_COUNT, NULL, NULL, 100))
	{
		CoUninitialize();
		return 0;
	}

	//printf("serverEngine.StartProcess()\n");
	//if (!serverEngine.StartProcess())
	//{
	//	CoUninitialize();
	//	return 0;
	//}

	printf("serverEngine.OnRegisterModule()\n");
	if (!serverEngine.OnRegisterModule())
	{
		CoUninitialize();
		return 0;
	}

	printf("Complete Register Module\n");

	printf("serverEngine.StartProcess()\n");
	if (!serverEngine.StartProcess())
	{
		CoUninitialize();
		return 0;
	}

	if (!serverEngine.SetGuildLoadEvent())
	{
		CoUninitialize();
		return 0;
	}

	printf("Processing - 1000\n");

	if (!serverEngine.ConnectRelayServer())
	{
		CoUninitialize();
		return 0;
	}

	printf("Processing - 2000\n");

	if (!serverEngine.ConnectLKServer())
	{
		CoUninitialize();
		return 0;
	}

	printf("Processing - 3000\n");

	/*if (!serverEngine.ConnectBillingServer())
	{
		CoUninitialize();
		return 0;
	}*/ //cmb disabled billing server

	printf("Processing - 4000\n");

	if( !serverEngine.WaitForGuildLoadComplete())
	{
		CoUninitialize();
		return 0;
	}

	printf("Processing - 5000\n");
	
	if (!serverEngine.LoadSiegeInfoFromDB())
	{
		CoUninitialize();
		return 0;
	}

	printf("Processing - 6000\n");

	serverEngine.ConnectServers();

	printf("Processing - 7000\n");

	serverEngine.SetServerStatus(GF_SERVER_START);

	printf("AlefServer is started\n");

	while (char c = getchar())
	{
		BOOL	bExitLoop	= FALSE;

		switch(c)
		{
		case 'r':
			g_pcsAgsmInterServerLink->ConnectRelayServer(TRUE);
			break;

		case 'l':
			g_pcsAgsmInterServerLink->ConnectLoginServers(TRUE);
			break;

		case 'd':
			g_pcsAgsmInterServerLink->DisconnectLoginServers();
			break;

		case 'm':
			{
				ApMemoryManager::GetInstance().ReportMemoryInfo();
				printf("\nApMemory에서 사용중인 내용들을 AlefServerD.exe_ApMemory.log에 출력했습니다.");
			}
			break;
			
		case '1':	// Print Current User Count - 2004.03.25. steeple
			{
				serverEngine.PrintCurrentUserCount();
			}
			break;
			
		case '2':	// Print Guild Info - 2004.06.23. steeple
			{
				serverEngine.PrintGuildInfo();
			}
			break;

		case 'g':	// Load Guild Info From DB - 2004.07.06. steeple
			{
				serverEngine.LoadGuildInfoFromDB();
			}
			break;

		case 'z':
			{
				bExitLoop	= TRUE;
			}
			break;

		case 'c':
			{
				g_bPrintNumCharacter	= 1;
			}
			break;

		case 'a':
			{
				g_bPrintActiveCellCount	= 1;
			}
			break;

		case 'p':
			{
				g_bPrintRemovePoolTID	= 1;
			}
			break;

		case 's':
			break;

		case 't':
			{
				g_pcsAgpmLog->m_bWriteChattingLog = !g_pcsAgpmLog->m_bWriteChattingLog;
				if(g_pcsAgpmLog->m_bWriteChattingLog)
					printf(" -- Write Chatting Log\n");
				else
					printf(" -- Don't write Chatting Log\n");
			}
			break;

		}

		if (bExitLoop)
			break;
	}

	serverEngine.Shutdown();

	CloseLog();
	
    CoUninitialize();

	return 0;
}
