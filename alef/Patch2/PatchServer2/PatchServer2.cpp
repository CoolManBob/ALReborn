/******************************************************************************
Module:  PatchServer2.cpp
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
#include "ApMemoryTracker.h"


//#define _ALEF_SERVICE_MODE

int DoConsole();

int main(int argc, char* argv[])
{
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
 
	return 0;
}
	
int DoConsole()
{
	int	i	= _set_SSE2_enable(1);

	if (i)
		printf("SSE2 enabled. \n");
	else
		printf("SSE2 not enabled; processor does not supoort SSE2.\n");

    // Init COM so we can use CoCreateInstance
    HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );

	OpenLog(AS_LOG_CONSOLE);

	ServerEngine serverEngine(g_guidApp);

	serverEngine.SetCrashReportExceptionHandler();

	serverEngine.SetClockInterval(200);
	serverEngine.SetNumInitGameThreads(2);

	SYSTEM_INFO		stSystemInfo;
	GetSystemInfo(&stSystemInfo);

	if (!serverEngine.StartServer(PATCHSERVER_PORT, stSystemInfo.dwNumberOfProcessors, MAX_CONNECTION_COUNT, NULL, NULL, 100))
	{
		CoUninitialize();
		return 0;
	}

	if (!serverEngine.StartProcess())
	{
		CoUninitialize();
		return 0;
	}

	if (!serverEngine.OnRegisterModule())
	{
		CoUninitialize();
		return 0;
	}

	printf("----------------------------------------> Complete Register Module");

	serverEngine.SetServerStatus(GF_SERVER_START);

	printf("PatchServer2 is started\n");

	while (char c = getchar())
	{
		BOOL	bExitLoop	= FALSE;

		switch(c)
		{
		case 'z':
			{
				bExitLoop	= TRUE;
			}
			break;
		}

		if (bExitLoop)
			break;
	}

	CloseLog();
	
    CoUninitialize();

	return 0;
}
