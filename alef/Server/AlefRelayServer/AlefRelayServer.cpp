/******************************************************************************
Module:  AlefRelayServer.cpp
Notices: Copyright (c) NHN Studio 2004 
Purpose: 
Last Update: 2004. 02. 03
******************************************************************************/

#include "ApBase.h"
#include <stdio.h>
#include <objbase.h>
#include "ServerEngine.h"

int main(int argc, char* argv[])
{
  // Init COM so we can use CoCreateInstance
    HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );

	//OpenLog(AS_LOG_FILE);
//	OpenLog(AS_LOG_CONSOLE);

	ServerEngine serverEngine;	

	if (!serverEngine.OnRegisterModule())
	{
		WriteLog(AS_LOG_RELEASE, "AlefAuctionServer : failed OnRegisterModule()");

		CoUninitialize();
		return 0;
	}

	if (!serverEngine.StartServer(11004, 4, 3000, "Ini\\IPBlockData.txt", 1 ))
	{
		CoUninitialize();
		return 0;
	}
	
	if (!serverEngine.InitServerManager())
	{
		CoUninitialize();
		return 0;
	}

	if (!serverEngine.StartProcess())
	{
		WriteLog(AS_LOG_RELEASE, "AlefAuctionServer : failed StartProcess()");

		CoUninitialize();
		return 0;
	}

	//스레드 생성을 위해 기다려준다. 
	Sleep( 1000 );

	/*
	if (!serverEngine.ConnectServers())
	{
		WriteLog(AS_LOG_RELEASE, "AlefAuctionServer : failed ConnectServers()");

		CoUninitialize();
		return 0;
	}

	INT16	nServerStatus = serverEngine.GetServerStatus();
	while (nServerStatus != GF_SERVER_COMPLETE_CONNECTION && nServerStatus != GF_SERVER_START)
	{
		Sleep(1000);
		nServerStatus = serverEngine.GetServerStatus();
	}
	*/

	serverEngine.CreateDBPool();
	serverEngine.StartDBProcess();
	serverEngine.SetServerStatus(GF_SERVER_START);

	fprintf(stdout, "AlefRelay is started\n");
	fflush(stdout);

	while (char c = getchar())
	{
		BOOL	bExitLoop	= FALSE;

		switch(c)
		{
		case 'm':
			{
				//MemoryManager().ReportMemoryInfo();
				ApMemoryManager::GetInstance().ReportMemoryInfo();
				printf("ApMemory에서 사용중인 내용들을 AlefRelayServerD.exe_ApMemory.log에 출력했습니다.");
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

	serverEngine.StopProcess();
	serverEngine.StopDBProcess();

	serverEngine.OnTerminate();

	serverEngine.Stop();

	CloseLog();
	
    CoUninitialize();

	return 0;
}