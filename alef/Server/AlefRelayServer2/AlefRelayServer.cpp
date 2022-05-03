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

int DoConsole();

int main(int argc, char* argv[])
{
	// Check Process is Running(Allow only one Server) 
	if (::IsProcessRun("RelayServer"))

	{
		printf("This Program is already Running!!!");
		exit(1);
	}

	DoConsole();

	return 0;
}

int DoConsole()
{

#ifndef _M_X64
	// Init COM so we can use CoCreateInstance
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif

	//OpenLog(AS_LOG_FILE);
	OpenLog(AS_LOG_CONSOLE);

	ServerEngine serverEngine;

	serverEngine.SetCrashReportExceptionHandler();


	if (!serverEngine.StartServer(11004, 4, 3000, NULL, NULL, 50))
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

	/*if (!serverEngine.ConnectServers())
	{
		CoUninitialize();
		return 0;
	}*/

	//스레드 생성을 위해 기다려준다. 
	Sleep(1000);

	serverEngine.SetServerStatus(GF_SERVER_START);

	fprintf(stdout, "AlefRelayServer is started\n");
	fflush(stdout);

	while (char c = getchar())
	{
		BOOL	bExitLoop = FALSE;

		switch (c)
		{
		case 'm':
		{
			ApMemoryManager::GetInstance().ReportMemoryInfo();
			printf("ApMemory에서 사용중인 내용들을 AlefRelayServer2D.exe_ApMemory.log에 출력했습니다.");
		}
		break;

		case 'z':
		{
			bExitLoop = TRUE;
		}
		break;
		}

		if (bExitLoop)
			break;
	}

	serverEngine.SetServerStatus(GF_SERVER_STOP);

	serverEngine.StopProcess();

	serverEngine.OnTerminate();

	serverEngine.Stop();

	CloseLog();

	CoUninitialize();

	return 0;
}