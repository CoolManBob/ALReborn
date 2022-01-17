/******************************************************************************
Module:  AlefRecruitServer.cpp
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 02. 10
******************************************************************************/

#include <stdio.h>
#include "objbase.h"
#include "ServerEngine.h"

GUID			g_guidApp = { 0xede9493e, 0x6ac8, 0x4f15, { 0x8d, 0x1, 0x8b, 0x16, 0x32, 0x0, 0xb9, 0x66 } };

int main(int argc, char* argv[])
{
    // Init COM so we can use CoCreateInstance
    CoInitializeEx( NULL, COINIT_MULTITHREADED );

	//OpenLog(AS_LOG_FILE);
	OpenLog(AS_LOG_CONSOLE);

	ServerEngine serverEngine(g_guidApp);

	if (!serverEngine.StartDP(9992))
	{
		CoUninitialize();
		return 0;
	}

	if (!serverEngine.OnRegisterModule())
	{
		WriteLog(AS_LOG_RELEASE, "AlefRecruitServer : failed OnRegisterModule()");

		CoUninitialize();
		return 0;
	}

	if (!serverEngine.StartProcess())
	{
		WriteLog(AS_LOG_RELEASE, "AlefRecruitServer : failed StartProcess()");

		CoUninitialize();
		return 0;
	}

	if (!serverEngine.ConnectServers())
	{
		WriteLog(AS_LOG_RELEASE, "AlefRecruitServer : failed ConnectServers()");

		CoUninitialize();
		return 0;
	}

	INT16	nServerStatus = serverEngine.GetServerStatus();
	while (nServerStatus != GF_SERVER_COMPLETE_CONNECTION && nServerStatus != GF_SERVER_START)
	{
		Sleep(1000);
		nServerStatus = serverEngine.GetServerStatus();
	}

	serverEngine.SetServerStatus(GF_SERVER_START);

	fprintf(stdout, "AlefRecruitServer is started\n");
	fflush(stdout);

	while (char c = getchar())
	{
		if (c == 'z')
			break;
	}

	//DB처리 관련 쓰레드를 죽인다.
	serverEngine.SetDBThreadStatus( DBTHREAD_DEACTIVE );

	//모든쓰레드가 죽을때까지 기다려준다.
	serverEngine.WaitDBThreads();

	serverEngine.SetServerStatus(GF_SERVER_STOP);

	serverEngine.DisconnectServers();

	serverEngine.StopProcess();

	serverEngine.OnTerminate();

	serverEngine.StopDP();

	CloseLog();
	
    CoUninitialize();

	return 0;
}
