// ServerEngine.cpp: implementation of the ServerEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "ServerEngine.h"

// Global 변수 선언

AgsmServerStatus* g_pcsAgsmServerStatus;
AgpmConfig* g_pcsAgpmConfig;
ApmMap* g_pcsApmMap;
AgpmFactors* g_pcsAgpmFactors;
AgpmCharacter* g_pcsAgpmCharacter;
AgpmBillInfo* g_pcsAgpmBillInfo;
AgpmGrid* g_pcsAgpmGrid;
AgpmItem* g_pcsAgpmItem;
AgpmAdmin* g_pcsAgpmAdmin;
AgpmLog* g_pcsAgpmLog;
AgsmAOIFilter* g_pcsAgsmAOIFilter;
AgsmServerManager2* g_pcsAgsmServerManager;
AgsmAdmin* g_pcsAgsmAdmin;
AgsmInterServerLink* g_pcsAgsmInterServerLink;
AgsmLog* g_pcsAgsmLog;
AgsmDatabaseConfig* g_pcsAgsmDatabaseConfig;
AgsmDatabasePool* g_pcsAgsmDatabasePool;
AgsmRelay2* g_pcsAgsmRelay;
AgsmAuctionRelay* g_pscAgsmAuctionRelay;
AgsmConfig* g_pcsAgsmConfig;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ServerEngine::ServerEngine()
{

}

ServerEngine::~ServerEngine()
{

}

BOOL ServerEngine::OnRegisterModule()
{
	g_AuCircularBuffer.Init(100 * 1024 * 1024);

	//g_iServerClientType = ASSM_SERVER_TYPE_RELAY_SERVER;

	REGISTER_MODULE(g_pcsAgsmServerStatus, AgsmServerStatus);
	REGISTER_MODULE(g_pcsAgpmConfig, AgpmConfig);
	REGISTER_MODULE(g_pcsApmMap, ApmMap);
	REGISTER_MODULE(g_pcsAgpmFactors, AgpmFactors);
	REGISTER_MODULE(g_pcsAgpmCharacter, AgpmCharacter);
	REGISTER_MODULE(g_pcsAgpmBillInfo, AgpmBillInfo);
	REGISTER_MODULE(g_pcsAgpmGrid, AgpmGrid);
	REGISTER_MODULE(g_pcsAgpmItem, AgpmItem);
	REGISTER_MODULE(g_pcsAgpmAdmin, AgpmAdmin);
	REGISTER_MODULE(g_pcsAgpmLog, AgpmLog);
	REGISTER_MODULE(g_pcsAgsmAOIFilter, AgsmAOIFilter);
	REGISTER_MODULE(g_pcsAgsmServerManager, AgsmServerManager2);
	REGISTER_MODULE(g_pcsAgsmAdmin, AgsmAdmin);
	REGISTER_MODULE(g_pcsAgsmInterServerLink, AgsmInterServerLink);
	REGISTER_MODULE(g_pcsAgsmLog, AgsmLog);
	REGISTER_MODULE(g_pcsAgsmDatabaseConfig, AgsmDatabaseConfig);
	REGISTER_MODULE(g_pcsAgsmDatabasePool, AgsmDatabasePool);
	REGISTER_MODULE(g_pcsAgsmRelay, AgsmRelay2);
	REGISTER_MODULE(g_pscAgsmAuctionRelay, AgsmAuctionRelay);
	REGISTER_MODULE(g_pcsAgsmConfig, AgsmConfig);

	g_pcsAgsmServerStatus->SetServerTypeRelayServer();
	g_pcsAgsmServerStatus->m_pServerMainClass = this;
	g_pcsAgsmServerStatus->Start();

	g_pcsAgsmServerManager->SetMaxServer(AGSMSERVER_MAX_NUM_SERVER);
	g_pcsAgsmServerManager->SetMaxIdleEvent2(100);

	if (!g_pcsAgsmRelay->SetCallbackOperation())
	{
		return FALSE;
	}

	if (!g_pscAgsmAuctionRelay->SetCallbackForAuctionServer())
	{
		return FALSE;
	}

	g_pcsAgsmServerManager->SetMaxIdleEvent(100);
	g_pcsAgsmInterServerLink->SetMaxIdleEvent2(15);

	if (!Initialize())
		return FALSE;

	if (!g_pcsAgsmServerManager->ReadServerInfo())
	{
		ASSERT(!"g_csAgsmServerManager StreamReadServerInfo 실패");
		printf_s("Acquire db account info failed\n");
		return FALSE;
	}

	if (!g_pcsAgsmServerManager->InitThisServer())
	{
		cout << "InitThisServer Failed" << endl;
		ASSERT(!"g_csAgsmServerManager InitThisServer 실패");
		return FALSE;
	}

	if (!LoadTemplateData())
		return FALSE;

	g_pcsAgsmDatabasePool->Create();

	return TRUE;
}

BOOL ServerEngine::LoadTemplateData()
{
	//Need to check for MSSQL or Oracle, this assumes MSSQL for now.
	if(!g_pcsAgsmDatabaseConfig->ReadConfig("Ini\\Database.ini", "Ini\\DBQuery_MS.ini"))
		return FALSE;
	else
		return TRUE;
}

BOOL ServerEngine::OnTerminate()
{
	// Echo Server Stop - 2004.05.16. steeple
	/*if(g_pcsEchoServer)
	{
		g_pcsEchoServer->Stop();
		delete g_pcsEchoServer;
		g_pcsEchoServer = NULL;
	}*/

	return TRUE;
}