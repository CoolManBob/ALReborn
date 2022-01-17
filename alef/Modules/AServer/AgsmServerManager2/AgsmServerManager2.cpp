/*=============================================================================

	AgsmServerManager2.cpp

==============================================================================*/		

#include <stdio.h>
#include <time.h>
#include "AgsmServerManager2.h"
#include "AgsModuleDBStream.h"
#include "ApMemoryTracker.h"
#include "AuGameEnv.h"
#include <AuXmlParser.h>


/****************************************************/
/*		The Implementation of AgsdServer class		*/
/****************************************************/
//
AgsdServer2::AgsdServer2()
	{
	}


AgsdServer2::~AgsdServer2()
	{
	}


void AgsdServer2::Initialize()
	{
	m_Mutex.Init();

	m_lServerID		= AP_INVALID_SERVERID;
	ZeroMemory(m_szIP, sizeof(m_szIP));
	memset(m_szPrivateIP, 0, sizeof(m_szPrivateIP));
	m_cType			= AGSMSERVER_TYPE_NONE;
	ZeroMemory(m_szWorld, sizeof(m_szWorld));
	ZeroMemory(m_szGroupName, sizeof(m_szGroupName));
	m_bEnable		= TRUE;

	ZeroMemory(m_szDBUser, sizeof(m_szDBUser));
	ZeroMemory(m_szDBPwd, sizeof(m_szDBPwd));
	ZeroMemory(m_szDBDSN, sizeof(m_szDBDSN));
	ZeroMemory(m_szLogDBUser, sizeof(m_szLogDBUser));
	ZeroMemory(m_szLogDBPwd, sizeof(m_szLogDBPwd));
	ZeroMemory(m_szLogDBDSN, sizeof(m_szLogDBDSN));

	m_SubWorld.MemSetAll();
	m_nSubWorld = 0;

	m_dpnidServer		= AGSMSERVER_INVALID_NID;
	m_bIsConnected		= FALSE;
	m_bIsReplyConnect	= FALSE;

	m_nControlArea	= 0;
	m_csControlArea.MemSetAll();
	
	m_nStatus		= GF_SERVER_STOP;
	m_nNumPlayers	= 0;

	m_pExtraMap = NULL;

	m_lMaxUserCount	= 3700;

	m_bIsTestServer = FALSE;

	m_lStartLevel = 1;
	m_llStartGheld = 0;
	
	m_lOrder		= 0;
	//JK_½É¾ß¼¦´Ù¿î
	m_bMidNightShutDown = FALSE;
	}


void AgsdServer2::Cleanup()
	{
	CHAR **ppsz = NULL;
	INT32 lIndex = 0;
	
	if (m_pExtraMap)
		{
		while (NULL != (ppsz = (CHAR **) m_pExtraMap->GetObjectSequence(&lIndex)))
			{
			if (*ppsz)
				delete [] *ppsz;
			}
		
		delete 	m_pExtraMap;
		m_pExtraMap = NULL;
		}
	
	m_Mutex.Destroy();
	}


BOOL AgsdServer2::AddWorld(CHAR *pszWorld, BOOL bShow)
	{
	if (m_nSubWorld >= AGSMSERVER_MAX_SUBWORLD)
		return FALSE;
	
	_tcsncpy(m_SubWorld[m_nSubWorld].m_szName, pszWorld, AGSM_MAX_SERVER_NAME);
	m_SubWorld[m_nSubWorld].m_bShow = bShow;
	m_nSubWorld++;
	
	return TRUE;
	}


BOOL AgsdServer2::DecodeExtra(CHAR *pszExtra)
{
	if (NULL == pszExtra || _T('\0') == *pszExtra)
		return FALSE;

	m_pExtraMap = new ApAdmin;
	if (NULL == m_pExtraMap)
		return FALSE;
	
	CHAR szKey[AGSMSERVER_MAX_EXTRA_VALUE+1];
	CHAR szValue[AGSMSERVER_MAX_EXTRA_VALUE+1];

	CHAR *psz = pszExtra;
	CHAR *pszBuf = NULL;

	if (FALSE == m_pExtraMap->SetCount(100)
		|| FALSE == m_pExtraMap->InitializeObject(sizeof(CHAR *), m_pExtraMap->GetCount())
		)
		return FALSE;

	while (_T('\0') != *psz)
	{
		ZeroMemory(szKey, sizeof(szKey));
		ZeroMemory(szValue, sizeof(szValue));

		// key
		pszBuf = szKey;
		while (_T('\0') != *psz && AGSMSERVER_EXTRA_KEY_DELIM != *psz)
			*pszBuf++ = *psz++;
		if (AGSMSERVER_EXTRA_KEY_DELIM == *psz)
			psz++; // skip _AGSMDATABASECONFIG_DELIM1
		*pszBuf = _T('\0');
	
		// value
		pszBuf = szValue;
		while (_T('\0') != *psz && AGSMSERVER_EXTRA_VALUE_DELIM != *psz)
			*pszBuf++ = *psz++;
		if (AGSMSERVER_EXTRA_VALUE_DELIM == *psz)
			psz++; // skip 
		*pszBuf = _T('\0');
		
		CHAR *pszValue = new CHAR[AGSMSERVER_MAX_EXTRA_VALUE+1];
		
		if (_tcslen(szKey) > 0 && _tcslen(szValue) > 0)
		{
			_tcscpy(pszValue, szValue);
			_tcslwr(szKey);
			m_pExtraMap->AddObject(&pszValue, szKey);
		}
	}
	
	return TRUE;
}


CHAR* AgsdServer2::GetExtraValue(CHAR *pszKey)
{
	ApString<AGSMSERVER_MAX_EXTRA_VALUE> szTmpKey(pszKey);	szTmpKey.MakeLower();
	if (m_pExtraMap)
		{
		CHAR **ppsz = (CHAR **) m_pExtraMap->GetObject(szTmpKey);
	
		if (ppsz)
			return *ppsz;
		}
	
	return NULL;
}




/************************************************************/
/*		The Implementation of AgsmServerManager2 class		*/
/************************************************************/
//
AgsmServerManager2::AgsmServerManager2()
	{
	SetModuleName("AgsmServerManager2");
	SetModuleType(APMODULE_TYPE_SERVER);

	m_pcsDealServer		= NULL;
	m_pcsRecruitServer	= NULL;
	m_pcsRelayServer	= NULL;
	m_pcsMasterDBServer	= NULL;
	m_pcsCompenDBServer = NULL;
	m_pcsAimEventServer	= NULL;
	m_pcsGameServers.MemSetAll();
	m_nNumGameServers = 0;
	}


AgsmServerManager2::~AgsmServerManager2()
	{
	}




//	IsInArea
//======================================================
//
BOOL AgsmServerManager2::_IsInArea(AuAREA csArea, AuPOS csPos)
	{
	if (csArea.posLeftTop.x <= csPos.x &&
		csArea.posLeftTop.z <= csPos.z &&
		csArea.posRightBottom.x >= csPos.x &&
		csArea.posRightBottom.z >= csPos.z)
		return TRUE;

	return FALSE;
	}




//	Module data
//==============================================
//
AgsdServer2* AgsmServerManager2::_CreateServer()
{
	AgsdServer2 *pcsServer = new AgsdServer2;

	if (pcsServer)
		pcsServer->Initialize();

	return pcsServer;
}


BOOL AgsmServerManager2::_DestroyServer(AgsdServer2 *pcsServer)
{
	pcsServer->Cleanup();

	delete pcsServer;

	return TRUE;
}




//	ApModule Inherited
//======================================================
//
BOOL AgsmServerManager2::OnAddModule()
	{
	// get related module
	m_pAgsmAOIFilter	= (AgsmAOIFilter *)	GetModule("AgsmAOIFilter");
	if (!m_pAgsmAOIFilter)
		return FALSE;

	return TRUE;
	}	


BOOL AgsmServerManager2::OnInit()
	{
	if (!m_csServerAdmin.InitializeObject(sizeof(AgsdServer2 *), m_csServerAdmin.GetCount(), NULL, NULL, this))
		return FALSE;

	if (!m_csWorldAdmin.InitializeObject(sizeof(AgsdServer2 *), m_csWorldAdmin.GetCount(), NULL, NULL, this))
		return FALSE;

	return TRUE;
	}


BOOL AgsmServerManager2::OnDestroy()
	{
	INT32	lIndex = 0;
	AgsdServer2	**ppcsServer = (AgsdServer2 **) m_csServerAdmin.GetObjectSequence(&lIndex);
	while (ppcsServer)
		{
		_DestroyServer(*ppcsServer);
		ppcsServer = (AgsdServer2 **) m_csServerAdmin.GetObjectSequence(&lIndex);
		}

	m_csServerAdmin.Reset();

	m_csWorldAdmin.Reset();

	return TRUE;
	}

BOOL AgsmServerManager2::ReadServerInfo()
{
	AuXmlDocument		m_XmlData;

	if(!m_XmlData.LoadFile("config\\ServerInfo.xml"))
		return FALSE;

	//m_XmlData.Print();

	AuXmlNode* pServerInfoNode = m_XmlData.FirstChild("SERVERINFO");
	if(!pServerInfoNode)
		return FALSE;

	for (AuXmlNode* pServerNode = pServerInfoNode->FirstChild("Server"); pServerNode; pServerNode = pServerNode->NextSibling("Server"))
	{
		AgsdServer2* pcsServer = _CreateServer();
		if (!pcsServer)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmServerManager2::StreamReadServerInfo() Can't create server\n");
			AuLogFile_s("LOG\\StartError.log", strCharBuff);

			return FALSE;
		}

		AuXmlElement* pElemIndex = pServerNode->FirstChildElement("INDEX");
		if(pElemIndex)
			pcsServer->m_lServerID = atoi(pElemIndex->GetText());

		AuXmlElement* pElemGroupName = pServerNode->FirstChildElement("GroupName");
		if(pElemGroupName)
			strcpy(pcsServer->m_szGroupName, pElemGroupName->GetText());

		AuXmlElement* pElemWorldName = pServerNode->FirstChildElement("WorldName");
		if(pElemWorldName)
			strcpy(pcsServer->m_szWorld, pElemWorldName->GetText());
			
		AuXmlElement* pElemType = pServerNode->FirstChildElement("TYPE");
		if(pElemType)
		{
			switch (atoi(pElemType->GetText()))
			{
				case 1:
					pcsServer->m_cType = AGSMSERVER_TYPE_MASTERDB_SERVER;
					break;

				case 15:
					pcsServer->m_cType = AGSMSERVER_TYPE_COMPENDB_SERVER;
					break;
					
				case 2:
					pcsServer->m_cType = AGSMSERVER_TYPE_LOGIN_SERVER;
					break;

				case 8:
					pcsServer->m_cType = AGSMSERVER_TYPE_GAME_SERVER;
					break;

				case 4:
					pcsServer->m_cType = AGSMSERVER_TYPE_RELAY_SERVER;
					break;

				case 10:
					pcsServer->m_cType = AGSMSERVER_TYPE_LK_SERVER;
					break;

				case 11:
					pcsServer->m_cType = AGSMSERVER_TYPE_RECRUIT_SERVER;
					break;
				
				case 12:
					pcsServer->m_cType = AGSMSERVER_TYPE_BILLING_SERVER;
					break;
			}
		}

		AuXmlElement* pElemAddress = pServerNode->FirstChildElement("Address");
		if(pElemAddress)
		{
			AuXmlElement* pElemPublicIP = pElemAddress->FirstChildElement("Public");
			if(pElemPublicIP)
				strcpy(pcsServer->m_szIP, pElemPublicIP->GetText());

			AuXmlElement* pElemPrivateIP = pElemAddress->FirstChildElement("Private");
			if(pElemPrivateIP)
				strcpy(pcsServer->m_szPrivateIP, pElemPrivateIP->GetText());
		}

		AuXmlElement* pElemControlArea = pServerNode->FirstChildElement("ControlArea");
		if(pElemControlArea)
		{
			if (pcsServer->m_nControlArea >= AGSMSERVER_MAX_CONTROL_AREA)
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmServerManager2::ServerInfoReadCB Overflow Control Area\n");
				AuLogFile_s("LOG\\StartError.log", strCharBuff);
				
				return FALSE;
			}

			CHAR* szControlArea = (CHAR*)pElemControlArea->GetText();

			sscanf(szControlArea, "%f,%f,%f,%f", &pcsServer->m_csControlArea[pcsServer->m_nControlArea].m_stControlArea.posLeftTop.x,
										&pcsServer->m_csControlArea[pcsServer->m_nControlArea].m_stControlArea.posLeftTop.z,
										&pcsServer->m_csControlArea[pcsServer->m_nControlArea].m_stControlArea.posRightBottom.x,
										&pcsServer->m_csControlArea[pcsServer->m_nControlArea].m_stControlArea.posRightBottom.z);
			
			pcsServer->m_nControlArea++;
		}

		AuXmlElement* pElemDataBase = pServerNode->FirstChildElement("DATABASE");
		if(pElemDataBase)
		{
			AuXmlElement* pElemDBVender = pElemDataBase->FirstChildElement("Vender");
			if(pElemDBVender)
				strcpy(pcsServer->m_szDBVender, pElemDBVender->GetText());

			AuXmlElement* pElemDBUSER = pElemDataBase->FirstChildElement("DBUSER");
			if(pElemDBUSER)
				strcpy(pcsServer->m_szDBUser, pElemDBUSER->GetText());

			AuXmlElement* pElemDBPWD = pElemDataBase->FirstChildElement("DBPWD");
			if(pElemDBPWD)
				strcpy(pcsServer->m_szDBPwd, pElemDBPWD->GetText());

			AuXmlElement* pElemDBDSN = pElemDataBase->FirstChildElement("DBDSN");
			if(pElemDBDSN)
				strcpy(pcsServer->m_szDBDSN, pElemDBDSN->GetText());

			AuXmlElement* pElemLOGDBUSER = pElemDataBase->FirstChildElement("LOGDBUSER");
			if(pElemLOGDBUSER)
				strcpy(pcsServer->m_szLogDBUser, pElemLOGDBUSER->GetText());

			AuXmlElement* pElemLOGDBPWD = pElemDataBase->FirstChildElement("LOGDBPWD");
			if(pElemLOGDBPWD)
				strcpy(pcsServer->m_szLogDBPwd, pElemLOGDBPWD->GetText());

			AuXmlElement* pElemLOGDBDSN = pElemDataBase->FirstChildElement("LOGDBDSN");
			if(pElemLOGDBDSN)
				strcpy(pcsServer->m_szLogDBDSN, pElemLOGDBDSN->GetText());
		}

		AuXmlElement* pElemShow = pServerNode->FirstChildElement("Show");
		if(pElemShow)
		{
			CHAR* szShow = (CHAR*)pElemShow->GetText();
			if(szShow && !strcmp(szShow, "F"))
				pcsServer->m_bEnable = FALSE;
		}

		AuXmlElement* pElemExtra = pServerNode->FirstChildElement("Extra");
		if(pElemExtra)
		{
			CHAR* szExtra = (CHAR*)pElemExtra->GetText();
			pcsServer->DecodeExtra(szExtra);
		}

		if (!AddServer(pcsServer))
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmServerManager2::StreamReadServerInfo() Enum Stream CB\n");
			AuLogFile_s("LOG\\StartError.log", strCharBuff);

			return FALSE;
		}
	}

	return TRUE;
}


//	Admin
//=========================================
//+
//
BOOL AgsmServerManager2::SetMaxServer(INT16 nCount)
	{
	m_csServerAdmin.SetCount(nCount);
	
	m_csWorldAdmin.SetCount(nCount);
	
	return TRUE;
	}


#define _WOLRD_DELIM		_T(':')
BOOL AgsmServerManager2::AddServer(AgsdServer2* pcsServer)
	{
	if (!pcsServer)
		return FALSE;

	if (!m_csServerAdmin.AddServer(pcsServer))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmServerManager2::AddServer() ServerAdmin Add Error\n");
		AuLogFile_s("LOG\\StartError.log", strCharBuff);
		
		_DestroyServer(pcsServer);
		return FALSE;
		}

	if (AGSMSERVER_TYPE_GAME_SERVER == pcsServer->m_cType)
		{
		if (NULL == m_csWorldAdmin.Add(pcsServer->m_szWorld, pcsServer))
			{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmServerManager2::AddServer() WorldAdmin Add Error\n");
			AuLogFile_s("LOG\\StartError.log", strCharBuff);

			_DestroyServer(pcsServer);
			return FALSE;
			}
		
		CHAR szWorld[AGSM_MAX_SERVER_NAME+1];
		CHAR *psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_SUB_WORLD);
		CHAR *pszBuf = NULL;
		
		if (NULL != psz)
			{
			ZeroMemory(szWorld, sizeof(szWorld));
			pszBuf = szWorld;
			
			while (_T('\0') != *psz)
				{
				if (_WOLRD_DELIM == *psz)
					{
					*pszBuf = _T('\0');
					if (_T('\0') != szWorld[0])
						{
						if (FALSE == pcsServer->AddWorld(szWorld, TRUE)
							|| NULL == m_csWorldAdmin.Add(szWorld, pcsServer))
							{
							char strCharBuff[256] = { 0, };
							sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmServerManager2::AddServer() SubWorld Add Error\n");
							AuLogFile_s("LOG\\StartError.log", strCharBuff);
							
							_DestroyServer(pcsServer);
							return FALSE;
							}				
						}
					psz++;
					ZeroMemory(szWorld, sizeof(szWorld));
					pszBuf = szWorld;
					}
				else
					*pszBuf++ = *psz++;
				}
			
			*pszBuf = _T('\0');
			if (_T('\0') != szWorld[0])
				{
				if (FALSE == pcsServer->AddWorld(szWorld, TRUE)
					|| NULL == m_csWorldAdmin.Add(szWorld, pcsServer))
					{
						char strCharBuff[256] = { 0, };
						sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmServerManager2::AddServer() WorldAdmin Add Error\n");
						AuLogFile_s("LOG\\StartError.log", strCharBuff);
					
					_DestroyServer(pcsServer);
					return FALSE;
					}				
				}
			}

		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_MAX_USER_COUNT);
		if (psz)
			{
			pcsServer->m_lMaxUserCount	= atoi(psz);
			}

		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_TEST_SERVER);
		if (psz)
			{
			pcsServer->m_bIsTestServer	= (strnicmp("y", psz, 1)==0) ? TRUE : FALSE;
			}

		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_START_LEVEL);
		if (psz)
			{
			pcsServer->m_lStartLevel	= atoi(psz);
			}

		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_START_GHELD);
		if (psz)
			{
			pcsServer->m_llStartGheld	= _atoi64(psz);
			}
			
		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_ORDER);
		if (psz)
			pcsServer->m_lOrder	= atoi(psz);

		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_AIM_EVENT_SERVER);
		if (psz)
			pcsServer->m_bIsAimEventServer = TRUE;

		m_csServerAdmin.SortGameServers();
		}

	EnumCallback(AGSMSERVER_CB_ADD_SERVER, pcsServer, NULL);

	return TRUE;
	}
#undef _WOLRD_DELIM



//	Status
//=============================================
//
BOOL AgsmServerManager2::UpdateServerStatus(AgsdServer2 *pcsServer, eSERVERSTATUS eNewStatus, INT16 nNumPlayers)
	{
	if (!pcsServer)
		return FALSE;

	pcsServer->m_Mutex.WLock();
	INT16	nOldStatus	= pcsServer->m_nStatus;
	pcsServer->m_nStatus = (INT16) eNewStatus;
	pcsServer->m_nNumPlayers = nNumPlayers;
	pcsServer->m_Mutex.Release();

	return EnumCallback(AGSMSERVER_CB_UPDATE_SERVER_STATUS, pcsServer, &nOldStatus);
	}


//	Callback setting
//====================================================
//
//
BOOL AgsmServerManager2::SetCallbackAddServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMSERVER_CB_ADD_SERVER, pfCallback, pClass);
	}


BOOL AgsmServerManager2::SetCallbackUpdateServerStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMSERVER_CB_UPDATE_SERVER_STATUS, pfCallback, pClass);
	}




//	Get Server
//======================================================
//
AgsdServer2* AgsmServerManager2::GetServer(INT32 lServerID)
	{
	return m_csServerAdmin.GetServer(lServerID);
	}


AgsdServer2* AgsmServerManager2::GetServer(CHAR *pszIP)
	{
	if (!pszIP || !strlen(pszIP))
		return NULL;

	return m_csServerAdmin.GetServer(pszIP);
	}


AgsdServer2* AgsmServerManager2::GetLoginServers(INT16 *pnIndex)
	{
	if (!pnIndex)
		return NULL;

	return m_csServerAdmin.GetLoginServers(pnIndex);
	}


AgsdServer2* AgsmServerManager2::GetGameServers(INT16 *pnIndex)
	{
	if (!pnIndex)
		return NULL;

	return m_csServerAdmin.GetGameServers(pnIndex);
	}


AgsdServer2* AgsmServerManager2::GetGameServerOfWorld(CHAR *pszWorld)
	{
	if (!pszWorld || _T('\0') == *pszWorld)
		return NULL;

	AgsdServer2 *pcsServer = m_csWorldAdmin.Get(pszWorld);

	return pcsServer;
	}


AgsdServer2* AgsmServerManager2::GetGameServersInGroup(INT16 *pnIndex)
	{
	if (!pnIndex || *pnIndex == m_nNumGameServers)
		return NULL;

	return m_pcsGameServers[(*pnIndex)++];
	}


CHAR* AgsmServerManager2::GetGameDBName(CHAR *pszWorld)
	{
	if (NULL == pszWorld || _T('\0') == *pszWorld)
		return NULL;

	AgsdServer2 *pcsServer = m_csWorldAdmin.Get(pszWorld);
	if (pcsServer)
		return pcsServer->m_szDBUser;

	return NULL;
	}


AgsdServer2* AgsmServerManager2::GetGameServerBySocketIndex(INT32 lIndex)
	{
	AgsdServer2* pcsGameServer = NULL;

	for(INT16 i = 0; i < AGSMSERVER_MAX_NUM_SERVER; i++)
		{
		if(m_pcsGameServers[i])
			{
			if(m_pcsGameServers[i]->m_dpnidServer == lIndex)
				{
				pcsGameServer = m_pcsGameServers[i];
				break;
				}
			}
		}

	return pcsGameServer;
	}


AgsdServer2* AgsmServerManager2::GetRelayServer()
	{
	return m_pcsRelayServer;
	}

AgsdServer2* AgsmServerManager2::GetRelayServer(AgsdServer2 *pcsGameServer)
{
	if(NULL == pcsGameServer)
		return NULL;

	if(pcsGameServer->m_cType != AGSMSERVER_TYPE_GAME_SERVER)
		return NULL;

	/////////////////////////////////////////////////////////////////////////////////
	// Get RelayServer fall under this GameServer
	INT32 lGameServerIndex		= pcsGameServer->m_lServerID;
	INT32 lRelayServerIndex		= lGameServerIndex + RELAY_SERVER_START_ID;

	AgsdServer *pcsRelayServer	= GetServer(lRelayServerIndex) ;
	if(NULL == pcsRelayServer)
		return NULL;

	return pcsRelayServer;
}


AgsdServer2* AgsmServerManager2::GetDealServer()
	{
	//  ×³ Ì¼ Ï¹Ç· Ì· Ø³.
	return m_pcsRelayServer;
	//return m_pcsDealServer;
	}


AgsdServer2* AgsmServerManager2::GetRecruitServer()
	{
	return m_pcsRecruitServer;
	}


AgsdServer2* AgsmServerManager2::GetMasterDBServer()
	{
	return m_pcsMasterDBServer;
	}


AgsdServer2* AgsmServerManager2::GetCompenDBServer()
	{
	return m_pcsCompenDBServer;
	}


UINT8 AgsmServerManager2::GetServerType(AgsdServer2 *pcsServer)
	{
	return pcsServer ? (pcsServer->m_cType) : AGSMSERVER_TYPE_NONE;
	}


AgsdServer2* AgsmServerManager2::GetControlServer(AuPOS posDestArea, AgsdControlArea *pstCurrentControlArea)
	{
	//    Ú´ ß¿ îº?..
	if (pstCurrentControlArea)
		{
		for (INT32 i = 0; i < pstCurrentControlArea->m_nNumAroundUnit; i++)
			{
			if (pstCurrentControlArea->m_pcsAroundUnit[i])
				{
				for (INT32 j = 0; j <  (pstCurrentControlArea->m_pcsAroundUnit[i])->m_nControlArea; j++)
					{
					if (_IsInArea((pstCurrentControlArea->m_pcsAroundUnit[i])->m_csControlArea[j].m_stControlArea, posDestArea))
						{
						return (AgsdServer2 *) pstCurrentControlArea->m_pcsAroundUnit[i];
						}
					}
				}
			}
		}
	else
		{
		INT16 nIndex = 0;
		AgsdServer2 *pcsServer = NULL;

		while (NULL != (pcsServer = m_csServerAdmin.GetGameServers(&nIndex)))
			{
			for (INT32 i = 0; i < pcsServer->m_nControlArea; i++)
				{
				if (_IsInArea(pcsServer->m_csControlArea[i].m_stControlArea, posDestArea))
					return pcsServer;
				}
			}
		}

	return NULL;
	}




//	This server
//========================================================
//
//
BOOL AgsmServerManager2::InitThisServer()
	{
	// Initialize Server Data
	INT32	lIndex = 0;

	// get local ip address
	CHAR	*pszLocalIP = NULL;
	INT16	nNumAddr;
	GetLocalIPAddr(&pszLocalIP, &nNumAddr);

	AgsdServer2 *pcsThisServer = NULL;
	AgsdServer2 **ppcsServer = NULL;

	// find this server
	for (int i = 0; i < nNumAddr; i++)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmServerManager2::InitThisServer() [%d] %s\n", i, pszLocalIP + 23 * i);
		AuLogFile_s("LOG\\IP.log", strCharBuff);
		
		pcsThisServer = GetServer(pszLocalIP + 23 * i);
		if (NULL != pcsThisServer)
			{
			m_lServerID = pcsThisServer->m_lServerID;
			//strncpy(m_szGroup, pcsThisServer->m_szGroup, AGSM_MAX_SERVER_NAME);
			SetSelfCID(m_lServerID);
			break;
			}
		}

	if (!pcsThisServer)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmServerManager2::InitThisServer() Cannot find this server\n");
		AuLogFile_s("LOG\\IP.log", strCharBuff);
		
		return FALSE;
		}

	// set servers in same group
	lIndex = 0;
	while (NULL != (ppcsServer = (AgsdServer2 **) m_csServerAdmin.GetObjectSequence(&lIndex)))
		{
		if (AGSMSERVER_TYPE_MASTERDB_SERVER == (*ppcsServer)->m_cType)
			{
			m_pcsMasterDBServer = *ppcsServer;
			continue;
			}
		
		if (AGSMSERVER_TYPE_COMPENDB_SERVER == (*ppcsServer)->m_cType)
			{
			m_pcsCompenDBServer = *ppcsServer;
			continue;
			}

		if((*ppcsServer)->m_bIsAimEventServer == TRUE)
		{
			m_pcsAimEventServer = *ppcsServer;
		}
		
		if (0 == _tcsncmp((*ppcsServer)->m_szWorld, pcsThisServer->m_szWorld, AGSM_MAX_SERVER_NAME))
			{
			switch ((*ppcsServer)->m_cType)
				{
				case AGSMSERVER_TYPE_GAME_SERVER:
					ASSERT(m_nNumGameServers < AGSMSERVER_MAX_NUM_SERVER);
					m_pcsGameServers[m_nNumGameServers++] = *ppcsServer;
					break;

				case AGSMSERVER_TYPE_RELAY_SERVER:
					m_pcsRelayServer = *ppcsServer;
					break;

				case AGSMSERVER_TYPE_RECRUIT_SERVER:
					m_pcsRecruitServer = *ppcsServer;
					break;
				}
			}
		}

	if (0 == _tcsicmp(pcsThisServer->m_szDBVender, _T("MSSQL")))
		g_eAgsDatabaseVender = AGS_DB_VENDER_MSSQL;
	else
		g_eAgsDatabaseVender = AGS_DB_VENDER_ORACLE;

	pcsThisServer->m_lTime = time(NULL);

	//5ÀÏ¶§¸¸ ¼º°ø
	int iResult = CheckThisServer();
	if( 5 !=  iResult )
	{
		char strBuff[256] = { 0, };
		sprintf_s(strBuff, sizeof(strBuff), "server start error - (%d)\n", iResult);
		AuLogFile_s("LOG\\IP.log", strBuff);

		printf("%s \n", strBuff);

		return FALSE;
	}

	return TRUE;
	}

#include "AuMD5Encrypt.h"
#define HASH_KEY_STRING_SERVERINFO "hashkeystring_serverinfo"

// result °¡ 5ÀÏ¶§ ¼º°ø
int AgsmServerManager2::CheckThisServer()
{
#ifndef _AREA_CHINA_
	return 5;//return TRUE;//JK_À¥Á¨ºô¸µ : ¸®ÅÏ°ªÀÌ ÀÌ»óÇÏ³×...
#endif

	//////////////////////////////////////////////////////////////////////////
	//
	int iResult = 0;
	fstream file;
	string filename;
	size_t size;
	char* buffer = NULL;

	filename = "serverinfo.dat";

	file.open(filename.c_str(), ios::in | ios::binary);
	if(!file) 
		return 1;

	file.seekg(0, ios::end);
	size = (size_t)file.tellg();
	buffer = new char[size+1];
	memset(buffer, 0, size+1);

	file.seekg(0, ios::beg);
	file.read(buffer, size);
	file.close();

	while(TRUE)
	{
		//////////////////////////////////////////////////////////////////////////
		//
		AuMD5Encrypt md5;
		if(!md5.DecryptString( HASH_KEY_STRING_SERVERINFO, buffer, size ))
		{
			iResult = 2;
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		//	
		AuXmlDocument pXml;
		pXml.Parse(buffer);
		if( pXml.Error() )
		{
			iResult = 3;
			break;
		}

		AuXmlNode* pRootNode = pXml.FirstChild("SERVERINFO");
		if(!pRootNode)
			break;

		AuXmlElement* pExpire = pRootNode->FirstChildElement("EXPIREDATE");
		if(!pExpire)
			break;

		CHAR* szExpire = (CHAR*)pExpire->GetText();
		if(!szExpire || strlen(szExpire) < 10)
			break;

		INT32 nYear, nMon, nDay;
		sscanf(szExpire, "%d/%d/%d", &nYear, &nMon, &nDay);
		CTime pExpireTime = CTime(nYear, nMon, nDay, 0, 0, 0);
		CTime pNow = CTime::GetCurrentTime();

		if(pNow > pExpireTime)
		{
			iResult = 4;
			break;
		}
		AgsdServer2* pServer = GetThisServer();
		if(!pServer)
			break;

		for(AuXmlElement* pElem = pRootNode->FirstChildElement("SERVER"); pElem; pElem = pElem->NextSiblingElement())
		{
			// IP
			CHAR* szIP = (CHAR*)pElem->Attribute("IP");
			if(!szIP || strncmp(szIP, pServer->m_szIP, sizeof(szIP)) != 0)
				continue;

			// TYPE
			CHAR* szType = (CHAR*)pElem->Attribute("TYPE");
			if(pServer->m_cType != atoi(szType))
				continue;

			// success
			iResult = 5;
			break; // exit for(...)
		}
	
		break; // exit while(...)
	}

	if(buffer)
		delete[] buffer;

	return iResult;
}

AgsdServer2* AgsmServerManager2::GetThisServer()
	{
	return GetServer(m_lServerID);
	}


UINT8 AgsmServerManager2::GetThisServerType()
	{
	return GetServerType(GetThisServer());
	}

//JK_½É¾ß¼¦´Ù¿î
INT32 AgsmServerManager2::CheckMidnightShutDownTime()
{
	
	// ÇöÀç ½Ã°£ ±¸ÇÏ±â
	SYSTEMTIME localTime;
	::GetLocalTime(&localTime);

	if(localTime.wHour >= 0 && localTime.wHour < 6) 
		return 0; //ÀÚÁ¤ºÎÅÍ »õº®6½Ã±îÁö´Â ¼¦´Ù¿î..

	INT32 leftMin = 0;
	//¼¦´Ù¿î°¡Áö ³²Àº ½Ã°£(ºÐ...)
	leftMin	= ( 23 - localTime.wHour ) * 60 + (59 - localTime.wMinute);
	return leftMin;

}


//	Packet
//==================================================
//
BOOL AgsmServerManager2::SendPacketGameServers(PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority)
	{
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	return TRUE;
	}
