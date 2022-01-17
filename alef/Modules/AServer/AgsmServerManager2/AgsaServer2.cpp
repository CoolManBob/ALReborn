/*========================================================================

	AgsaServer2.cpp

========================================================================*/

#include "AgsaServer2.h"
#include <algorithm>

/****************************************************/
/*		The Implementation of AgsaServer2 class		*/
/****************************************************/
//
AgsaServer2::AgsaServer2()
	{
	m_nNumServers		= 0;
	m_nNumGameServers	= 0;
	m_nNumLoginServers	= 0;

	m_pcsGameServers.MemSetAll();
	m_pcsLoginServers.MemSetAll();
	}


AgsaServer2::~AgsaServer2()
	{
	}

int AgsaServer2::SortByOrder(const void * p1, const void * p2)
{
	if(!p1 || !p2) return FALSE;
	
	AgsdServer2* a = *(AgsdServer2**)p1;
	AgsdServer2* b = *(AgsdServer2**)p2;
	
	return -(a->m_lOrder - b->m_lOrder);
}



//	AddServer
//=========================================================================
//
//	Add server to map. key1 = lServerID, key2=m_szIP, value = AgsdServer2*
//	Add game server, login server to array
//
AgsdServer2* AgsaServer2::AddServer(AgsdServer2 *pcsServer)
	{
	if (!pcsServer || !AddObject((PVOID) &pcsServer, pcsServer->m_lServerID, pcsServer->m_szIP))
		return NULL;

	m_nNumServers++;

	switch (pcsServer->m_cType)
		{
		case AGSMSERVER_TYPE_GAME_SERVER:
			ASSERT(m_nNumGameServers < AGSMSERVER_MAX_GAME_SERVER);
			m_pcsGameServers[m_nNumGameServers++] = pcsServer;
			break;

		case AGSMSERVER_TYPE_LOGIN_SERVER:
			ASSERT(m_nNumLoginServers < AGSMSERVER_MAX_LOGIN_SERVER);
			m_pcsLoginServers[m_nNumLoginServers++] = pcsServer;
			break;

		default:
			break;
		}

	return pcsServer;
	}

void AgsaServer2::SortGameServers()
{
	qsort(m_pcsGameServers.begin(), m_nNumGameServers, sizeof(AgsdServer2 *), AgsaServer2::SortByOrder);
}



//	RemoveServer
//=================================================
//
//	Remove server of given id from map and array
//
BOOL AgsaServer2::RemoveServer(INT32 lServerID)
	{
	AgsdServer2 *pcsServer = GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	switch (pcsServer->m_cType)
		{
		case AGSMSERVER_TYPE_GAME_SERVER:
			{
			if (0 == m_nNumGameServers)
				break;
	
			for (int i = 0; i < m_nNumGameServers; i++)
				{
				if (m_pcsGameServers[i] != pcsServer)
					continue;

				m_pcsGameServers.MemCopy(i, &m_pcsGameServers[i + 1], m_nNumGameServers - i - 1);
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

				m_pcsLoginServers.MemCopy(i, &m_pcsLoginServers[i + 1], m_nNumLoginServers - i - 1);
				m_nNumLoginServers--;
				break;
				}
			break;
			}
		}

	m_nNumServers--;
	return RemoveObject(lServerID);
	}




//	GetServer
//=================================================
//
//	Get server of given id from map
//	Get server of given IP address(string) from map
//
AgsdServer2* AgsaServer2::GetServer(INT32 lServerID)
	{
	AgsdServer2 **ppServer = (AgsdServer2 **) GetObject(lServerID);
	if (!ppServer)
		return NULL;

	return *ppServer;
	}


AgsdServer2* AgsaServer2::GetServer(CHAR *pszIP)
	{
	AgsdServer2 **ppServer = (AgsdServer2 **) GetObject(pszIP);
	if (!ppServer)
		return NULL;

	return *ppServer;
	}




//	GetLoginServers
//====================================================
//
AgsdServer2* AgsaServer2::GetLoginServers(INT16 *pnIndex)
	{
	if (!pnIndex || *pnIndex == m_nNumLoginServers)
		return NULL;

	return m_pcsLoginServers[(*pnIndex)++];
	}




//	GetGameServers
//====================================================
//		
AgsdServer2* AgsaServer2::GetGameServers(INT16 *pnIndex)
	{
	if (!pnIndex || *pnIndex == m_nNumGameServers)
		return NULL;

	return m_pcsGameServers[(*pnIndex)++];
	}




/********************************************************/
/*		The Implementation of AgsaServerWorld class		*/
/********************************************************/
//
AgsaServerWorld::AgsaServerWorld()
	{
	}


AgsaServerWorld::~AgsaServerWorld()
	{
	}



//	Add/Remove
//====================================
//
AgsdServer2* AgsaServerWorld::Add(CHAR *pszWorld, AgsdServer2 *pcsServer)
	{
	if (!pszWorld || !pcsServer || !AddObject((PVOID) &pcsServer, pszWorld))
		return NULL;

	return pcsServer;
	}


BOOL AgsaServerWorld::Remove(CHAR *pszWorld)
	{
	return RemoveObject(pszWorld);
	}




//	Get
//=================================================
//
AgsdServer2* AgsaServerWorld::Get(CHAR *pszWorld)
	{
	AgsdServer2 **ppServer = (AgsdServer2 **) GetObject(pszWorld);
	if (!ppServer)
		return NULL;

	return *ppServer;
	}
