/******************************************************************************
Module:  AgsEngine.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 10
******************************************************************************/

#include "ServerEngine.h"
#include "magdebug.h"
#include "AuPacket.h"


ServerEngine::ServerEngine(GUID guidApp)
{
	m_pcsAgsmPatchServer2 = NULL;
	m_pcsAgsmServerStatus = NULL;

	m_hEventStop = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_AsIOCPServer.SetDefaultSocketType(SOCKET_TYPE_PATCH);
	m_DPClientModule.m_SocketManager.SetDefaultSocketType(SOCKET_TYPE_PATCH);
}

ServerEngine::~ServerEngine()
{
	CloseHandle(m_hEventStop);
}

BOOL ServerEngine::OnRegisterModule()
{
	g_AuCircularBuffer.Init(100 * 1024 * 1024);

	REGISTER_MODULE(m_pcsAgsmPatchServer2, AgsmPatchServer2);
	REGISTER_MODULE(m_pcsAgsmServerStatus, AgsmServerStatus);

	m_pcsAgsmServerStatus->SetServerTypePatchServer();
	m_pcsAgsmServerStatus->m_pServerMainClass = this;
	m_pcsAgsmServerStatus->m_pfServerShutdown = NULL;

	// module start
	if (!Initialize())
		return FALSE;

	if (!m_pcsAgsmPatchServer2->LoadPatchCode("PatchCode.dat"))
		return FALSE;

	if (!m_pcsAgsmPatchServer2->m_cAuPackingManager.LoadResFile( "Patch.res" ))
		return FALSE;

	if (!m_pcsAgsmPatchServer2->LoadDownlodINI("PatchDownload.ini"))
		return FALSE;

	m_pcsAgsmPatchServer2->m_iVersion = m_pcsAgsmPatchServer2->m_cAuPackingManager.GetLastVersion();
	m_pcsAgsmServerStatus->SetPatchVersion(m_pcsAgsmPatchServer2->m_iVersion);

	m_pcsAgsmServerStatus->Start();

	return TRUE;
}

BOOL ServerEngine::Shutdown(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (!pClass)
		return FALSE;

	ServerEngine	*pThis		= (ServerEngine *)	pClass;

#ifdef	_ALEF_SERVICE_MODE
	::SetEvent(pThis->m_hEventStop);
#else
	::ExitProcess(0);
#endif	//_ALEF_SERVICE_MODE

	return TRUE;
}
