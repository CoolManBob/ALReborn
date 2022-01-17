#include "ServerEngine.h"

AgpmAreaChatting*		g_pcsAgpmAreaChatting;
AgsmAreaChatting*		g_pcsAgsmAreaChatting;

ServerEngine::ServerEngine()
{
}

ServerEngine::~ServerEngine()
{
}

BOOL ServerEngine::OnRegisterModule()
{
	g_AuCircularBuffer.Init(100 * 1024 * 1024);
	g_AuCircularOutBuffer.Init(1024 * 1024);

	REGISTER_MODULE(g_pcsAgpmAreaChatting, AgpmAreaChatting);
	REGISTER_MODULE(g_pcsAgsmAreaChatting, AgsmAreaChatting);

	g_pcsAgsmAreaChatting->SetMaxRegionChar(5000);
	g_pcsAgsmAreaChatting->SetMaxClient(5000);

	return Initialize();
}

BOOL ServerEngine::OnTerminate()
{
	return Destroy();
}