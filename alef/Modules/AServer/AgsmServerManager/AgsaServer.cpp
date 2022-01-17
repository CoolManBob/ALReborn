/******************************************************************************
Module:  AgsaServer.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 12. 10
******************************************************************************/

#include "AgsaServer.h"

AgsaServerTemplate::AgsaServerTemplate()
{
}

AgsaServerTemplate::~AgsaServerTemplate()
{
}

AgsdServerTemplate* AgsaServerTemplate::AddServerTemplate(AgsdServerTemplate *pcsServerTemplate, INT32 lTID)
{
	if (AddObject((PVOID) &pcsServerTemplate, lTID))
		return pcsServerTemplate;

	return NULL;
}

AgsdServerTemplate* AgsaServerTemplate::GetServerTemplate(INT32 lTID)
{
	AgsdServerTemplate **ppTemplate = (AgsdServerTemplate **) GetObject(lTID);

	if (!ppTemplate)
		return NULL;

	return *ppTemplate;
}

BOOL AgsaServerTemplate::RemoveServerTemplate(INT32 lTID)
{
	return RemoveObject(lTID);
}

AgsaServer::AgsaServer()
{
	m_nNumServers		= 0;
	m_nNumGameServers	= 0;
	m_nNumLoginServers	= 0;

	ZeroMemory(m_pcsGameServers, sizeof(AgsdServer *) * AGSMSERVER_MAX_GAME_SERVER);
	ZeroMemory(m_pcsLoginServers, sizeof(AgsdServer *) * AGSMSERVER_MAX_LOGIN_SERVER);
}

AgsaServer::~AgsaServer()
{
}

AgsdServer* AgsaServer::AddServer(AgsdServer *pcsServer, INT32 lServerID, INT16 nServerType)
{
	if (!pcsServer)
		return NULL;

	if (!AddObject((PVOID) &pcsServer, lServerID, ((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_szName))
		return NULL;

	m_nNumServers++;

	switch (nServerType) {
	case AGSMSERVER_TYPE_GAME_SERVER:

		if (m_nNumGameServers == AGSMSERVER_MAX_GAME_SERVER)
			break;

		m_pcsGameServers[m_nNumGameServers] = pcsServer;

		m_nNumGameServers++;

		break;

	case AGSMSERVER_TYPE_LOGIN_SERVER:

		if (m_nNumLoginServers == AGSMSERVER_MAX_LOGIN_SERVER)
			break;

		m_pcsLoginServers[m_nNumLoginServers] = pcsServer;

		m_nNumLoginServers++;

		break;

	case AGSMSERVER_TYPE_DEAL_SERVER:

		m_pcsDealServer = pcsServer;

		break;

	default:
		break;
	}

	return pcsServer;
}

AgsdServer* AgsaServer::GetServer(INT32 lServerID)
{
	AgsdServer **ppServer = (AgsdServer **) GetObject(lServerID);
	if (!ppServer)
		return NULL;

	return *ppServer;
}

AgsdServer* AgsaServer::GetServer(CHAR *szServerName)
{
	AgsdServer **ppServer = (AgsdServer **) GetObject(szServerName);
	if (!ppServer)
		return NULL;

	return *ppServer;
}

BOOL AgsaServer::RemoveServer(INT32 lServerID)
{
	AgsdServer *pcsServer = GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	switch (((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType) {
	case AGSMSERVER_TYPE_GAME_SERVER:
		{
			if (!m_nNumGameServers)
				break;

			for (int i = 0; i < m_nNumGameServers; i++)
			{
				if (m_pcsGameServers[i] != pcsServer)
					continue;

				CopyMemory(m_pcsGameServers + i, m_pcsGameServers + i + 1, sizeof(AgsdServer *) * (m_nNumGameServers - i - 1));

				m_nNumGameServers--;

				break;
			}

			break;
		}

	case AGSMSERVER_TYPE_LOGIN_SERVER:
		{
			if (!m_nNumLoginServers)
				break;

			for (int i = 0; i < m_nNumLoginServers; i++)
			{
				if (m_pcsLoginServers[i] != pcsServer)
					continue;

				CopyMemory(m_pcsLoginServers + i, m_pcsLoginServers + i + 1, sizeof(AgsdServer *) * (m_nNumLoginServers - i - 1));

				m_nNumLoginServers--;

				break;
			}

			break;
		}
	}

	m_nNumServers--;

	return RemoveObject(lServerID);
}

AgsdServer* AgsaServer::GetLoginServers(INT16 *pnIndex)
{
	if (!pnIndex || *pnIndex == m_nNumLoginServers)
		return NULL;

	return m_pcsLoginServers[(*pnIndex)++];
}

AgsdServer* AgsaServer::GetDealServer()
{
	return m_pcsDealServer;
}

AgsdServer* AgsaServer::GetGameServers(INT16 *pnIndex)
{
	if (!pnIndex || *pnIndex == m_nNumGameServers)
		return NULL;

	return m_pcsGameServers[(*pnIndex)++];
}