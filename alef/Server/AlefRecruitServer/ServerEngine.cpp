#include <stdio.h>
#include "ServerEngine.h"

AuOLEDBManager			g_csAuOLEDBManager;
AgsmServerManager		g_csAgsmServerManager;
AgsmConnectionSetupGame	g_csAgsmConnectionSetupGame;
AgsmRecruitServer		g_csAgsmRecruitServer;
AgsmRecruitClient		g_csAgsmRecruitClient;
AgsmRecruitServerDB		g_csAgsmRecruitServerDB;

ServerEngine::ServerEngine(GUID guidApp)
{
	SetGUID(guidApp);
}

ServerEngine::~ServerEngine()
{
}

BOOL ServerEngine::OnRegisterModule()
{

	if (!g_csAuOLEDBManager.	AddModule(this))
	{
		ASSERT( !"g_csAuOLEDBManager 등록실패" );
		return FALSE;
	}
	
	if (!g_csAgsmServerManager.	AddModule(this))
	{
		ASSERT( !"g_csAgsmServerManager 등록실패" );
		return FALSE;
	}

	if (!g_csAgsmConnectionSetupGame.	AddModule(this))
	{
		ASSERT( !"g_csAgsmConnectionSetupGame 등록실패" );
		return FALSE;
	}

	if (!g_csAgsmRecruitServer.	AddModule(this))
	{
		ASSERT( !"g_csAgsmRecruitServer 등록실패" );
		return FALSE;
	}

	if (!g_csAgsmRecruitClient.	AddModule(this))
	{
		ASSERT( !"g_csAgsmRecruitClient 등록실패" );
		return FALSE;
	}

	if (!g_csAgsmRecruitServerDB.AddModule(this))
	{
		ASSERT( !"g_csAgsmRecruitServerDB 등록실패" );
		return FALSE;
	}

	g_csAgsmServerManager.	SetMaxServer	( 20		);
	g_csAgsmServerManager.	SetMaxTemplate	( 100		);
	g_csAgsmServerManager.	SetMaxIdleEvent	( 100		);

	// module start
	if (!Initialize())
		return FALSE;

	//OLEDBManager를 초기화해준다.
	g_csAuOLEDBManager.Init( 10, 10000 );

	g_csAuOLEDBManager.SetStatus( DBTHREAD_ACTIVE );

	//Recruit DB 쿼리를 실행해줄 쓰레드를 띄운다. 우선 10개~ 내맘대로. ㅎㅎ
	for( int i=0; i<10; ++i )
	{
		g_csAuOLEDBManager.RegisterThreadFunction( AgsmRecruitServerDB::RecruitDBProcessThread, &g_csAgsmRecruitServerDB, i );
	}

	if (!g_csAgsmServerManager.	StreamReadTemplate("Ini\\ServerTemplate.ini"		)	)
	{
		ASSERT( !"g_csAgsmServerManager StreamReadTemplate 실패" );
		return FALSE;
	}

	if (!g_csAgsmServerManager.InitServerData())
	{
		ASSERT( !"g_csAgsmServerManager InitServerData 실패" );
		return FALSE;
	}

	// 아뒤 앞에 붙일 서버플래그를 가져와서 필요 모듈들에 세팅한다.
	INT16	nServerFlag = 0;
	INT16	nFlagSize	= 0;
	if (!g_csAgsmServerManager.	GetServerFlag(&nServerFlag, &nFlagSize))
	{
		ASSERT( !"g_csAgsmServerManager.GetServerFlag() 실패");
		return FALSE;
	}

	WriteLog(AS_LOG_RELEASE, "ServerEngine : module is started");
	//MessageBox(NULL, "Module is started", "AlefRecruitServer", 1);

	return TRUE;
}

BOOL ServerEngine::OnTerminate()
{
	return Destroy();
	//return TRUE;
}

BOOL ServerEngine::ConnectServers()
{
	return g_csAgsmConnectionSetupGame.ConnectGameServers();
	//return g_csAgsmServerManager.SetupConnection();
}

BOOL ServerEngine::DisconnectServers()
{
	return g_csAgsmServerManager.DisconnectAllServers();
}

//DB쓰레드의 상태를 바꾼다. DB처리중에 끝나지 않도록.....
BOOL ServerEngine::SetDBThreadStatus( int iStatus )
{
	g_csAuOLEDBManager.SetStatus( iStatus );

	return TRUE;
}

//DB 모든 쓰레드가 끝나길 기다린다.
BOOL ServerEngine::WaitDBThreads()
{
	g_csAuOLEDBManager.WaitThreads();

	return TRUE;
}
