/******************************************************************************
Module:  AlefMigrateDB.cpp
Notices: Copyright (c) NHN Studio 2004 By Netong
Purpose: 
******************************************************************************/

#include "ApBase.h"

#include <stdio.h>
#include "objbase.h"
#include "ServerEngine.h"

GUID			g_guidApp = { 0xede9493e, 0x6ac8, 0x4f15, { 0x8d, 0x1, 0x8b, 0x16, 0x32, 0x0, 0xb9, 0x66 } };

int main(int argc, char* argv[])
{
    // Init COM so we can use CoCreateInstance
    CoInitializeEx( NULL, COINIT_MULTITHREADED );

	ServerEngine serverEngine(g_guidApp);

	if (!serverEngine.OnRegisterModule())
	{
		WriteLog(AS_LOG_RELEASE, "AlefMigrateDB : failed OnRegisterModule()");

		CoUninitialize();
		return 0;
	}

	if (!serverEngine.MigrateDB())
	{
		printf("\nMigrateDB Fail\n");
		return 0;
	}

	printf("\nMigrateDB Success\n");

	serverEngine.OnTerminate();

	//DB처리 관련 쓰레드를 죽인다.
	serverEngine.SetDBThreadStatus( DBTHREAD_DEACTIVE );

	//모든쓰레드가 죽을때까지 기다려준다.
	serverEngine.WaitDBThreads();

	serverEngine.Stop();

    CoUninitialize();

	return 0;
}
