// AlefChattingServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "ServerEngine.h"

int _tmain(int argc, _TCHAR* argv[])
{
  	int	i	= _set_SSE2_enable(1);

	if (i)
		printf("SSE2 enabled. \n");
	else
		printf("SSE2 not enabled; processor does not supoort SSE2.\n");

  // Init COM so we can use CoCreateInstance
    HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );

	ServerEngine serverEngine;

	serverEngine.SetCrashReportExceptionHandler();

	serverEngine.SetClockInterval(200);
	serverEngine.SetNumInitGameThreads(2);

	SYSTEM_INFO		stSystemInfo;
	GetSystemInfo(&stSystemInfo);

	if (!serverEngine.StartServer(11007, stSystemInfo.dwNumberOfProcessors, 5000, NULL, NULL, 50))
	{
		CoUninitialize();
		return 0;
	}

	if (!serverEngine.OnRegisterModule())
	{
		CoUninitialize();
		return 0;
	}

	if (!serverEngine.StartProcess())
	{
		CoUninitialize();
		return 0;
	}

	serverEngine.SetServerStatus(GF_SERVER_START);

	printf("AlefChattingServer is started\n");

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

	serverEngine.OnTerminate();
	
    CoUninitialize();

	return 0;
}

