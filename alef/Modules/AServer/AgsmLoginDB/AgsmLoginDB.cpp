/*========================================================================

	AgsmLoginDB.cpp

========================================================================*/

#include <stdio.h>
#include "AgsmLoginDB.h"
#include "ApLockManager.h"
#include "AgsmTitle.h"

#define _AGSMLOGINDB_LOG_					_T("LOG\\AgsmLoginDB.log")
#define _AGSMLOGINDB_COUPON_LOG_			_T("LOG\\ProcessEventError.txt")

#define LOGIN_ERROR_FILE_NAME		"LOG\\LoginError.log"
#ifdef _WEBZEN_AUTH_
#define LOGIN_AUTH_FILE_NAME		_T("LOG\\Auth.log")
#endif
// -========================================================================-
// 한국, 일본 로긴 모듈
// -========================================================================-
#ifdef _AREA_KOREA_
#include "AuGameEnv.h"

#ifdef _HANGAME_
#include "HanServerAuth.h"
#include "HanAuthForSvr.h"

	int ChangeHanAuthErrorToDefault( int jcode )
	{
		switch ( jcode )
		{
		case HAN_AUTHSVR_OK:
			return AGSMLOGINDB_AUTH_RESULT_SUCCESS;

		case HAN_AUTHSVR_MEMBERID_INVALID:
			return AGSMLOGINDB_AUTH_RESULT_NOT_MEMBER;

		case HAN_AUTHSVR_PASSWORD_INCORRECT:
			return AGSMLOGINDB_AUTH_RESULT_PWD_FAIL;

		case HAN_AUTHSVR_PASSWORD_MISMATCHOVER:
			return AGSMLOGINDB_AUTH_RESULT_PWD_FAIL;

		case HAN_AUTHSVR_MEMBERID_NOTEXIST:
			return AGSMLOGINDB_AUTH_RESULT_NOT_MEMBER;

		default:
			return AGSMLOGINDB_AUTH_RESULT_NOT_CONNECT;
		}
	}
#endif // _HANGAME_
#endif //_AREA_KOREA_

// -========================================================================-
// 일본 로긴 모듈
// -========================================================================-
#ifdef _AREA_JAPAN_
#include "HanServerAuth.h"
#include "HanAuthForSvr.h"
#include "AuGameEnv.h"

	int ChangeHanAuthErrorToDefault( int jcode )
	{
		switch ( jcode )
		{
		case HAN_AUTHSVR_OK:
			return AGSMLOGINDB_AUTH_RESULT_SUCCESS;

		case HAN_AUTHSVR_MEMBERID_INVALID:
			return AGSMLOGINDB_AUTH_RESULT_NOT_MEMBER;

		case HAN_AUTHSVR_PASSWORD_INCORRECT:
			return AGSMLOGINDB_AUTH_RESULT_PWD_FAIL;

		case HAN_AUTHSVR_PASSWORD_MISMATCHOVER:
			return AGSMLOGINDB_AUTH_RESULT_PWD_FAIL;

		case HAN_AUTHSVR_MEMBERID_NOTEXIST:
			return AGSMLOGINDB_AUTH_RESULT_NOT_MEMBER;

		default:
			return AGSMLOGINDB_AUTH_RESULT_NOT_CONNECT;
		}
	}
#endif //_AREA_KOREA_

// -========================================================================-
// 중국 로긴 모듈
// -========================================================================-
#ifdef _AREA_CHINA_
	#include "AgsmSessionMgr.h"

	#ifdef _DEBUG
		#pragma comment(lib, "PTSDKDLL_D.lib")
	#else
		#pragma comment(lib, "PTSDKDLL.lib")
	#endif

	AgsmSessionMgr			g_smgr;
	AgsmClientInterface		g_csClientInterface;
#endif


// -========================================================================-
// 코드 시작



INT32	g_alEventArmour[AURACE_TYPE_MAX][AUCHARCLASS_TYPE_MAX][4] =
{
	{
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }
	},

	{
		{ 0, 0, 0, 0 },
		{ 70, 73, 74, 75 },
		{ 348, 351, 352, 353 },
		{ 0, 0, 0, 0 },
		{ 672, 675, 676, 677 }
	},

	{
		{ 0, 0, 0, 0 },
		{ 178, 181, 182, 183 },
		{ 696, 699, 700, 701 },
		{ 0, 0, 0, 0 },
		{ 378, 381, 382, 383 }
	},

	{
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 2908, 2910, 2911, 2912 },
		{ 0, 0, 0, 0 },
		{ 2853, 2855, 2856, 2857 }
	},
};

/****************************************************/
/*		The Implementation of HTTP Helper class		*/
/****************************************************/
//
ApHttp::ApHttp() 
	{
	m_hOpen = NULL;
	m_hHttp = NULL;
	ZeroMemory(m_URLBuffer, sizeof(m_URLBuffer));
	ZeroMemory(m_Result, sizeof(m_Result));
	}


ApHttp::~ApHttp()
	{
	Close();
	}


BOOL ApHttp::Open()
	{
	m_hOpen = ::InternetOpen(_T("Archlord"), NULL, NULL, NULL, 0);
	if (NULL == m_hOpen)
		{
		DWORD dwError = ::GetLastError();
		return FALSE;
		}

	m_hHttp = InternetConnect(m_hOpen, _T("archlordid.naver.com"),
								INTERNET_DEFAULT_HTTP_PORT,
								_T(""),
								_T(""),
								INTERNET_SERVICE_HTTP,
								0,
								NULL
								);
	if (NULL == m_hHttp)
		{
		DWORD dwError = ::GetLastError();
		return FALSE;
		}
	
	return TRUE;
	}


BOOL ApHttp::IsOpen()
	{
	return (NULL != m_hOpen && NULL != m_hHttp);
	}


INT32 ApHttp::SendRequest(const TCHAR *pszName, const TCHAR *pszPassword)
	{
	if (!IsOpen())
		{
		return -1;
		}

	TCHAR szObj[100];
	_stprintf(szObj, _T("nidlogin.login?svctype=2048&id=%s&pw=%s"), pszName, pszPassword);
	HANDLE hRequest = HttpOpenRequest(m_hHttp, _T("GET"), szObj, NULL, NULL,
									  NULL, INTERNET_FLAG_KEEP_CONNECTION, NULL);
	if (NULL == hRequest)
		{
		DWORD dwError = ::GetLastError();
		return -1;
		}	

	if (!HttpSendRequest(hRequest, NULL, 0, NULL, 0))
		{
		DWORD dwError = ::GetLastError();
		return -1;
		}

	ZeroMemory( m_Result, sizeof(m_Result) );
	if (!::InternetReadFile(hRequest, m_Result, ResultBufferSize, &m_dwReceiveBytes))
		{
		DWORD dwError = ::GetLastError();
		return -1;
		}

	ASSERT(ResultBufferSize > m_dwReceiveBytes);

	if (NULL != hRequest)
		::InternetCloseHandle(hRequest);

	return _ttoi(m_Result);
	}


void ApHttp::Close()
	{
	if (NULL != m_hHttp)
		::InternetCloseHandle(m_hHttp);
	if (NULL != m_hOpen)
		::InternetCloseHandle(m_hOpen);
	}

/************************************************/
/*		The Implementation of Worker class		*/
/************************************************/
//
AgsmLoginWorker::AgsmLoginWorker()
	{
	m_pThis = NULL;
	m_bStop = FALSE;
	}


AgsmLoginWorker::~AgsmLoginWorker()
	{
	}

void AgsmLoginWorker::Set(AgsmLoginDB *pThis, INT16 nIndex)
	{
	m_pThis = pThis;
	m_nIndex = nIndex;
	}


void AgsmLoginWorker::Stop()
	{
	m_bStop = TRUE;
	}


DWORD AgsmLoginWorker::Do()
	{
	PVOID pvQuery = NULL;
	AgsmLoginQueueInfo	*pQueryInfo = NULL;

//// 한국 인증 프로세스
//#if _AREA_KOREA_
//	if (!m_Http.Open())
//		return 10;
//#endif

	while (!m_bStop)
		{
		ASSERT(NULL != m_pThis->m_pAgsmLoginServer);
		pvQuery = m_pThis->m_pAgsmLoginServer->PopFromQueue();
		if (pvQuery)
			{
			pQueryInfo = (AgsmLoginQueueInfo *) pvQuery;

			m_pThis->ProcessOperation(pQueryInfo);

//			// 한국만 Http 개체가 필요하다
//#ifdef _AREA_KOREA_
//			m_pThis->ProcessOperation(pQueryInfo, &m_Http);
//#else
//			m_pThis->ProcessOperation(pQueryInfo);
//#endif

			if (ApLockManager::Instance()->IsExistData())
				{
				ApLockManager::Instance()->AllUnlock();
				_tprintf(_T("ApLockManager::Instance()->AllUnlock(). Operation : %d\n"), pQueryInfo->m_nOperation);
				}

#ifdef _AREA_CHINA_
			if (AGSMLOGIN_OPERATION_CHECK_ACCOUNT != pQueryInfo->m_nOperation)
				delete pQueryInfo;
#else
			delete pQueryInfo;
#endif
				
			pQueryInfo = NULL;
			}
		else
			{
			Sleep(1);
			}
		}
//#ifdef _AREA_KOREA_
//	m_Http.Close();
//#endif
	
	return 0;
	}




/********************************************************/
/*		The Implementation of Game DB Group class		*/
/********************************************************/
//
AgsmLoginGameDBGroup::AgsmLoginGameDBGroup()
	{
	m_pManager = NULL;
	m_pDatabases = NULL;
	m_nConnection = 0;
	}


AgsmLoginGameDBGroup::~AgsmLoginGameDBGroup()
	{
	if (NULL != m_pDatabases)
		{
		delete [] m_pDatabases;
		m_pDatabases = NULL;
		}
	m_nConnection =0;
	m_pManager = NULL;
	}


BOOL AgsmLoginGameDBGroup::Connect(AuDatabaseManager *pManager, TCHAR *pszUser, TCHAR *pszPwd, TCHAR *pszName, INT16 nConnection)
	{
	if (NULL == pManager || 0 > nConnection)
		return FALSE;
	
	m_pManager = pManager;
	m_nConnection = nConnection;
	
	m_pDatabases = new AuDatabase2[m_nConnection];
	if (NULL == m_pDatabases)
		return FALSE;
	
	for (INT16 i=0; i<m_nConnection; ++i)
		{
		if (FALSE == m_pDatabases[i].Initialize(m_pManager)
			|| FALSE == m_pDatabases[i].Connect(pszUser, pszPwd, pszName)
			)
			{
			for (INT j = 0; j < i; j++)
				m_pDatabases[j].Terminate();
			
			delete [] m_pDatabases;
			m_pDatabases = NULL;
			return FALSE;
			}
		
		m_pDatabases[i].InitializeSemaphore();
		m_Semaphores.Add(m_pDatabases[i].m_hSemaphore);
		}
	
	return TRUE;
	}


AuDatabase2* AgsmLoginGameDBGroup::GetIdle()
	{
	AuDatabase2 *pDatabase = NULL;

	if (NULL == m_pDatabases)
		return pDatabase;

	DWORD dwWait = m_Semaphores.Wait(FALSE, AGSMLOGINDB_DB_WAIT_TIME);
	
	if (dwWait >= WAIT_OBJECT_0 && dwWait < WAIT_OBJECT_0 + m_nConnection)
		{
		pDatabase = &m_pDatabases[dwWait-WAIT_OBJECT_0];
		}
	
	return pDatabase;
	}



/****************************************************/
/*		The Implementation of AgsmLoginDB class		*/
/****************************************************/
//
AgsmLoginDB::AgsmLoginDB()
	{
	SetModuleName(_T("AgsmLoginDB"));
	
	m_pWorkers = NULL;
	m_nWorker = 0;
	
	m_pMasterDatabases		= NULL;	
	m_pCompenDatabases		= NULL;

	m_pAgsdExpeditionList	= NULL;
	m_lNumExpeditionList	= 0;

#ifdef _AREA_WESTERN_
	//m_pCogpas = 0;
#endif
	}


AgsmLoginDB::~AgsmLoginDB()
	{
	RemoveAllQuery();

	if (m_pAgsdExpeditionList)
		{
		delete [] m_pAgsdExpeditionList;
		m_pAgsdExpeditionList	= NULL;
		}
	}




//	ApModule inherited
//====================================
//
BOOL AgsmLoginDB::OnAddModule()
	{
	m_pAgpmLogin = (AgpmLogin *) GetModule(_T("AgpmLogin"));
	m_pAgpmGrid = (AgpmGrid *) GetModule(_T("AgpmGrid"));
	m_pAgpmFactors = (AgpmFactors *) GetModule(_T("AgpmFactors"));
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmItem = (AgpmItem *) GetModule(_T("AgpmItem"));
	m_pAgpmSkill = (AgpmSkill *) GetModule(_T("AgpmSkill"));
	m_pAgpmAdmin = (AgpmAdmin*) GetModule(_T("AgpmAdmin"));
	m_pAgpmUIStatus = (AgpmUIStatus *) GetModule(_T("AgpmUIStatus"));
	m_pAgsmAccountManager = (AgsmAccountManager *) GetModule(_T("AgsmAccountManager"));
	m_pAgsmServerManager = (AgsmServerManager *) GetModule(_T("AgsmServerManager2"));
	m_pAgsmFactors = (AgsmFactors *) GetModule(_T("AgsmFactors"));
	m_pAgsmCharacter = (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	m_pAgsmCharManager = (AgsmCharManager *) GetModule(_T("AgsmCharManager"));
	m_pAgsmItem = (AgsmItem *) GetModule(_T("AgsmItem"));
	m_pAgsmItemManager = (AgsmItemManager *) GetModule(_T("AgsmItemManager"));
	m_pAgsmItemConvert = (AgsmItemConvert *) GetModule(_T("AgsmItemConvert"));
	m_pAgsmSkill = (AgsmSkill *) GetModule(_T("AgsmSkill"));
	m_pAgsmSkillManager = (AgsmSkillManager *) GetModule(_T("AgsmSkillManager"));
	m_pAgsmQuest = (AgsmQuest *) GetModule(_T("AgsmQuest"));
	m_pAgsmLoginServer = (AgsmLoginServer *) GetModule(_T("AgsmLoginServer"));
	m_pAgsmLoginClient = (AgsmLoginClient *) GetModule(_T("AgsmLoginClient"));
	m_pAgsmEventSkillMaster = (AgsmEventSkillMaster *) GetModule(_T("AgsmEventSkillMaster"));
	m_pAgsmUIStatus = (AgsmUIStatus *) GetModule (_T("AgsmUIStatus"));
	m_pAgsmAuction = (AgsmAuction *) GetModule(_T("AgsmAuction"));
	m_pAgsmGuild = (AgsmGuild *) GetModule(_T("AgsmGuild"));
	m_pAgsmGK = (AgsmGK *) GetModule(_T("AgsmGK"));
	m_pAgpmPvP = (AgpmPvP *) GetModule(_T("AgpmPvP"));
	m_pAgpmWorld = (AgpmWorld *) GetModule(_T("AgpmWorld"));
	m_pAgsmWorld = (AgsmWorld *) GetModule(_T("AgsmWorld"));
	m_pAgpmConfig = (AgpmConfig *) GetModule(_T("AgpmConfig"));
	m_pAgsmReturnToLogin = (AgsmReturnToLogin *) GetModule(_T("AgsmReturnToLogin"));
	m_pAgsmLogin = (AgsmLogin *) GetModule(_T("AgsmLogin"));//JK_중복로그인

	if( !m_pAgpmGrid || !m_pAgpmFactors || !m_pAgpmCharacter || !m_pAgpmItem || !m_pAgsmServerManager ||
		!m_pAgsmFactors || !m_pAgsmCharacter || !m_pAgsmCharManager || !m_pAgsmItem || !m_pAgsmItemManager || !m_pAgsmItemConvert ||
		!m_pAgsmSkill || !m_pAgsmSkillManager || !m_pAgsmLoginServer || !m_pAgsmLoginClient || !m_pAgpmSkill || !m_pAgpmAdmin ||
		!m_pAgsmAccountManager || !m_pAgsmQuest ||
		!m_pAgpmUIStatus || !m_pAgsmUIStatus || !m_pAgsmEventSkillMaster || !m_pAgsmAuction || !m_pAgsmGuild ||
		!m_pAgsmGK || !m_pAgpmPvP || !m_pAgpmWorld || !m_pAgsmWorld ||
		!m_pAgpmConfig || !m_pAgsmReturnToLogin || !m_pAgsmLogin)
		{
		return FALSE;
		}

	if (!m_pAgsmGK->SetCallbackReceiveBillAuthResult(CBReceiveBillAuthResult, this))
		return FALSE;

	if (!m_pAgpmWorld->SetCallbackGetCharacterCount(CBGetCharacterCount, this))
		return FALSE;

	if (!m_pAgsmReturnToLogin->SetCallbackReconnectClient(CBReconnectClient, this))
		return FALSE;
	
	if (FALSE == m_Map.SetCount(100)
		|| FALSE == m_Map.InitializeObject(sizeof(AgsmLoginGameDBGroup *), m_Map.GetCount())
		)
		return FALSE;

#ifdef _AREA_KOREA_
#ifdef _HANGAME_
	if(g_SvrAuth.Init(CHanServerAuth::HAN_SERVER_AREA_KOREA) == false) return FALSE;
#endif
#endif

#ifdef _AREA_JAPAN_
	if ( g_SvrAuth.Init(CHanServerAuth::HAN_SERVER_AREA_JAPAN) == false ) return FALSE;
#endif

#ifdef _AREA_CHINA_
	g_csClientInterface.InitPT();
	g_csClientInterface.m_pAgsmLoginDB		= this;
	g_csClientInterface.m_pAgsmLoginClient	= m_pAgsmLoginClient;
#endif

#ifdef _AREA_WESTERN_
	/*m_pCogpas = new CCogpas;
	if (!m_pCogpas->LoadDLL())
		return FALSE;*/
#endif

	return TRUE;
	}

BOOL AgsmLoginDB::OnInit()
{
	m_pAgsmTitle = (AgsmTitle *) GetModule(_T("AgsmTitle"));

	if(!m_pAgsmTitle)
		return FALSE;

	return TRUE;
}


//	Worker
//==========================================
//
BOOL AgsmLoginDB::StartWorker(INT16 nWorker)
	{
	if (nWorker <=0 || nWorker > 30)
		return FALSE;

	if(g_lLocalServer == 0)
		m_nWorker = nWorker;
	else
		m_nWorker = 3;
	
	if (FALSE == ConnectDatabase())
		{
		DisconnectDatabase();
		return FALSE;
		}
	
	// start worker
	m_pWorkers = new AgsmLoginWorker[m_nWorker];
	
	for (INT16 i=0; i<m_nWorker; ++i)
		{
		m_pWorkers[i].Set(this, i);
		if (!m_pWorkers[i].Start())
			{
			delete [] m_pWorkers;
			m_nWorker = 0;
			return FALSE;
			}
		}
	
	return TRUE;
	}


void AgsmLoginDB::StopWorker()
	{
	zzWaitableCollection Workers;
	for (INT16 i=0; i<m_nWorker; ++i)
		{
		Workers.Add(m_pWorkers[i]);
		m_pWorkers[i].Stop();
		}

	Workers.Wait(TRUE, 3000);

	delete [] m_pWorkers;
	
	DisconnectDatabase();
	}




//	Database
//========================================
//
BOOL AgsmLoginDB::ConnectDatabase()
	{
	AuDatabaseBuffer::Initialize(8);
	m_DatabaseManagerOracle.Initialize(g_bNotUseOCI ? AUDB_API_ODBC : AUDB_API_OCI, AUDB_VENDER_ORACLE);
	m_DatabaseManagerMSSQL.Initialize(AUDB_API_ODBC, AUDB_VENDER_MSSQL);

	AuDatabaseManager *pDatabaseManager = NULL;
	AgsdServer *pAgsdServer = NULL;

	// connect master database first
	m_pMasterDatabases = new AuDatabase2[m_nWorker];
	pAgsdServer = m_pAgsmServerManager->GetMasterDBServer();

	if(!pAgsdServer)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginDB::ConnectDatabase() There is no master server.\n");
		AuLogFile_s("LOG\\StartError.log", strCharBuff);
		
		return FALSE;
	}

	for (INT16 i=0; i<m_nWorker; ++i)
		{
		if (0 == _tcslen(pAgsdServer->m_szDBVender) || 0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("ORACLE")))
			{
			pDatabaseManager = &m_DatabaseManagerOracle;
			}
		else if (0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("MSSQL")))
			{
			pDatabaseManager = &m_DatabaseManagerMSSQL;
			}
		else
			return FALSE;
		
		if (FALSE == m_pMasterDatabases[i].Initialize(pDatabaseManager)
			||FALSE == m_pMasterDatabases[i].Connect(pAgsdServer->m_szDBUser, pAgsdServer->m_szDBPwd, pAgsdServer->m_szDBDSN)
			)
			{
			_tprintf(_T("!!! Error : [%2d][%s] Failed to connect\n"), i, pAgsdServer->m_szDBUser);
			return FALSE;
			}

		m_pMasterDatabases[i].InitializeSemaphore();
		m_MasterSema.Add(m_pMasterDatabases[i].m_hSemaphore);		
		}
	
	_tprintf(_T("Info : [%d X %s DB] Connection succeeded\n"), m_nWorker, pAgsdServer->m_szDBUser);

	// connect game database
	// for all server
	INT16 nConnect = 10;			// Game DB Connection, hard-coded
	if(g_lLocalServer == 1)
		nConnect = 2;

	INT16 nIndex = 0;

	while (NULL != (pAgsdServer = m_pAgsmServerManager->GetGameServers(&nIndex)))
		{
		if (FALSE == pAgsdServer->m_bEnable)
			continue;

		AgsdServer *pcsRelayServer	= m_pAgsmServerManager->GetRelayServer(pAgsdServer);
		if(NULL == pcsRelayServer)
			continue;

		if (0 == _tcslen(pcsRelayServer->m_szDBVender) || 0 == _tcsicmp(pcsRelayServer->m_szDBVender, _T("ORACLE")))
			pDatabaseManager = &m_DatabaseManagerOracle;
		else if (0 == _tcsicmp(pcsRelayServer->m_szDBVender, _T("MSSQL")))
			pDatabaseManager = &m_DatabaseManagerMSSQL;
		else
			return FALSE;
		
		AgsmLoginGameDBGroup *pAgsmLoginGameDBGroup = new AgsmLoginGameDBGroup;
		if (NULL == pAgsmLoginGameDBGroup)
			{
			_tprintf(_T("!!! Error : [%2d][%s] Failed to connect\n"), nIndex, pcsRelayServer->m_szDBUser);
			return FALSE;
			}

		// ignore if connected or not
		BOOL bConnect = pAgsmLoginGameDBGroup->Connect(pDatabaseManager,
													   pcsRelayServer->m_szDBUser,
													   pcsRelayServer->m_szDBPwd,
													   pcsRelayServer->m_szDBDSN,
													   nConnect);

		if (!m_Map.AddObject(&pAgsmLoginGameDBGroup, pAgsdServer->m_szWorld))
			return FALSE;
		
		if (bConnect)
			_tprintf(_T("Info : [%d X %s.%s] DB Connection succeeded\n"), nConnect, pcsRelayServer->m_szDBUser, pcsRelayServer->m_szWorld);
		else
			_tprintf(_T("!!! Error : [%2d][%s] Failed to connect\n"), nIndex, pcsRelayServer->m_szDBUser);
		}

	// compensation database
	pAgsdServer = m_pAgsmServerManager->GetCompenDBServer();
	if(pAgsdServer)
		{
		m_pCompenDatabases = new AuDatabase2[m_nWorker];
		for (INT16 i=0; i<m_nWorker; ++i)
			{
			if (0 == _tcslen(pAgsdServer->m_szDBVender) || 0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("ORACLE")))
				pDatabaseManager = &m_DatabaseManagerOracle;
			else if (0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("MSSQL")))
				pDatabaseManager = &m_DatabaseManagerMSSQL;
			else
				{
				delete [] m_pCompenDatabases;
				m_pCompenDatabases = NULL;
				return TRUE;
				}
			
			if (FALSE == m_pCompenDatabases[i].Initialize(pDatabaseManager)
				||FALSE == m_pCompenDatabases[i].Connect(pAgsdServer->m_szDBUser, pAgsdServer->m_szDBPwd, pAgsdServer->m_szDBDSN)
				)
				{
				_tprintf(_T("!!! Error : [%2d][%s] Failed to connect compensation database\n"), i, pAgsdServer->m_szDBUser);
				delete [] m_pCompenDatabases;
				m_pCompenDatabases = NULL;
				return TRUE;
				}

			m_pCompenDatabases[i].InitializeSemaphore();
			m_CompenSema.Add(m_pCompenDatabases[i].m_hSemaphore);
			}
		}
	
	return TRUE;
	}


void AgsmLoginDB::DisconnectDatabase()
	{
	if (NULL != m_pMasterDatabases)
		{
		delete [] m_pMasterDatabases;
		m_pMasterDatabases = NULL;
		}

	if (NULL != m_pCompenDatabases)
		{
		delete [] m_pCompenDatabases;
		m_pCompenDatabases = NULL;
		}
	
	// for all game db group
	INT32 lIndex = 0;
	AgsmLoginGameDBGroup **ppGroup = NULL;
	
	while (NULL != (ppGroup = (AgsmLoginGameDBGroup **) m_Map.GetObjectSequence(&lIndex)))
		{
		if (*ppGroup)
			delete (*ppGroup);
		}
	
	m_Map.Reset();
	}


AuDatabase2* AgsmLoginDB::GetIdleMasterDatabase()
	{
	AuDatabase2 *pDatabase = NULL;

	if (NULL == m_pMasterDatabases)
		return pDatabase;

	DWORD dwWait = m_MasterSema.Wait(FALSE, AGSMLOGINDB_DB_WAIT_TIME);
	
	if (dwWait >= WAIT_OBJECT_0 && dwWait < WAIT_OBJECT_0 + m_nWorker)
		{
		pDatabase = &m_pMasterDatabases[dwWait-WAIT_OBJECT_0];
		}
	
	return pDatabase;
	}


AuDatabase2* AgsmLoginDB::GetIdleGameDatabase(TCHAR *pszName)
	{
	AgsmLoginGameDBGroup **ppGroup = (AgsmLoginGameDBGroup **) m_Map.GetObject(pszName);
	
	if (ppGroup && *ppGroup)
		return (*ppGroup)->GetIdle();
	
	return NULL;
	}

AuDatabase2* AgsmLoginDB::GetIdleGameDatabase(INT32 *plIndex)
	{
	AgsmLoginGameDBGroup **ppGroup = (AgsmLoginGameDBGroup **) m_Map.GetObjectSequence(plIndex);

	if (ppGroup && *ppGroup)
		return (*ppGroup)->GetIdle();
	
	return NULL;
	}


AuDatabase2* AgsmLoginDB::GetIdleCompenDatabase()
	{
	AuDatabase2 *pDatabase = NULL;

	if (NULL == m_pCompenDatabases)
		return pDatabase;

	DWORD dwWait = m_CompenSema.Wait(FALSE, AGSMLOGINDB_DB_WAIT_TIME);
	
	if (dwWait >= WAIT_OBJECT_0 && dwWait < WAIT_OBJECT_0 + m_nWorker)
		{
		pDatabase = &m_pCompenDatabases[dwWait-WAIT_OBJECT_0];
		}
	
	return pDatabase;
	}




//	Main
//===================================
//

// 중국과 유럽의 경우에는 pHttp == NULL 이다.
BOOL AgsmLoginDB::ProcessOperation(AgsmLoginQueueInfo *pQueryInfo, ApHttp *pHttp /*= NULL*/)
	{
	BOOL bResult = FALSE;

	switch (pQueryInfo->m_nOperation)
		{
		case AGSMLOGIN_OPERATION_CHECK_ACCOUNT : // check account id, pwd, ...
			bResult = ProcessCheckAccount1(pQueryInfo, pHttp);
			break;

		case AGSMLOGIN_OPERATION_GET_UNION :
			bResult = ProcessGetUnion(pQueryInfo);
			break;

		case AGSMLOGIN_OPERATION_GET_EKEY:
			bResult = ProcessGetEKey(pQueryInfo);
			break;

		case AGSMLOGIN_OPERATION_GET_CHARACTERS : // get character of given account
			bResult = ProcessGetCharacters(pQueryInfo);
			break;
	
		case AGSMLOGIN_OPERATION_SELECT_CHARACTER : // when select character
			bResult = ProcessSelectCharacter(pQueryInfo);
			break;

		case AGSMLOGIN_OPERATION_RESET_LOGINSATUS_BY_DISCONNECT_FROM_GAMESERVER :	// login status
			bResult = ProcessResetLoginStatusByDisconnectFromGameServer(pQueryInfo);
			break;
			
		case AGSMLOGIN_OPERATION_RESET_LOGINSATUS_BY_DISCONNECT_FROM_LOGINSERVER :
			bResult = ProcessResetLoginStatusByDisconnectFromLoginServer(pQueryInfo);
			break;

		case AGSMLOGIN_OPERATION_SET_LOGINSATUS_IN_GAMESERVER :
			bResult = ProcessSetLoginStatusInGameServer(pQueryInfo);
			break;
			
		case AGSMLOGIN_OPERATION_CREATE_CHARACTER : // create character
			bResult = ProcessCreateCharacter(pQueryInfo);
			break;
		
		case AGSMLOGIN_OPERATION_REMOVE_CHARACTER : // remove character
			bResult = ProcessRemoveCharacter(pQueryInfo);
			break;
		
		case AGSMLOGIN_OPERATION_RENAME_CHARACTER : // update character(rename character)
			bResult = ProcessRenameCharacter(pQueryInfo);
			break;

		default :
			break;
		}

	return bResult;
	}




//	Login step
//=======================================
//
BOOL AgsmLoginDB::ProcessCheckAccount1(AgsmLoginQueueInfo *pQueryInfo, ApHttp* pHttp)
	{
	INT32 lResult = 0;
	INT32 lResult2 = 0;

	_tprintf(_T("ProcessCheckAccount1 - 1\n"));
	for (int i = 0; i < AGPACHARACTER_MAX_ID_STRING; ++i)
	{
		if (pQueryInfo->m_szAccountID[i] == ' ')
		{
			pQueryInfo->m_szAccountID[i] = '\0';
			break;
		}
		else if (pQueryInfo->m_szAccountID[i] == '\0')
			break;
	}

	// 혹시 모르니 NULL로 스트링을 막아두자
	pQueryInfo->m_szAccountID[AGPACHARACTER_MAX_ID_STRING] = 0;


	_tprintf(_T("ProcessCheckAccount1 - 2\n"));
	//
	if (m_pAgpmConfig->IsIgnoreAccountAuth())
	{
		_tprintf(_T("ProcessCheckAccount1 - 3\n"));
		lResult = AGSMLOGINDB_AUTH_RESULT_SUCCESS;
	}
	else
	{
#ifdef _AREA_CHINA_
		// for localauth, PT가 연결이 끊어졌을때는 로컬의 MySQL로 직접 질의한다. 그를 위한것
		g_smgr.NewAccountToQi(pQueryInfo->m_szAccountID, pQueryInfo);

		// PT Authen
		CSessionPtr ptr;	// PT SDK에서 내부적으로 사용하는 session

		g_csClientInterface.m_pAS->NewSession(ptr);				// 새 session을 만들고
		ptr->PutValue("ArchInput", pQueryInfo->m_szAccountID);	// session에 AccountID를 저장해둔다

		// 실제 인증 시도
		g_csClientInterface.m_pAS->PTAuthen(ptr, pQueryInfo->m_szAccountID, pQueryInfo->m_szPassword);

		// sessionID와 QueryInfo*를 저장해둔다
		g_smgr.NewSessionToQi(ptr->GetSessionId(), pQueryInfo);

		lResult = AGSMLOGINDB_AUTH_RESULT_WAIT;	// 인증 결과를 기다리고 있다
#endif

#ifdef _AREA_KOREA_
		_tprintf(_T("ProcessCheckAccount1 - 4\n"));
		if ( false == GetGameEnv().IsDebugTest() )
		{
			//if(m_pAgsmLoginClient->IsReturnToLogin(pQueryInfo->m_ulCharNID))
			//{
			//	_tprintf(_T("ProcessCheckAccount1 - 5 - ReturnToLogin is TRUE\n"));
			//	lResult = AGSMLOGINDB_AUTH_RESULT_SUCCESS;
			//}
			//else
			//{
#ifdef _HANGAME_
			_tprintf(_T("ProcessCheckAccount1 - 5\n"));
			int iAge = 0;
			lResult2 = g_SvrAuth.Auth( pQueryInfo->m_szAccountID, pQueryInfo->m_szHanAuthString, &iAge );
			lResult = ChangeHanAuthErrorToDefault( lResult2 );

			// 리턴값이 어찌되던 간에 iAge 를 저장한다. 2007.11.20. steeple
			m_pAgsmLoginClient->SetAccountAge(pQueryInfo->m_ulCharNID, pQueryInfo->m_szAccountID, iAge);
			//}
#endif

#ifdef _WEBZEN_AUTH_
			if( m_pAgsmLoginClient->IsCertificatedNID(pQueryInfo->m_ulCharNID) )
			{
				//std::cout << "Global Authentication : ID = ["<< pQueryInfo->m_szAccountID << "] return to Game server." << std::endl;

				char szLog[256]={0,};

				sprintf_s(szLog,sizeof(szLog),"Auth #1 [%s] [%s]",pQueryInfo->m_szAccountID , pQueryInfo->m_szGBAuthString);

				AuLogFile_s(LOGIN_AUTH_FILE_NAME, szLog);

				lResult = AGSMLOGINDB_AUTH_RESULT_SUCCESS;

				GetWebzenAuthInfoFromDB(pQueryInfo);
		
				m_pAgsmLoginClient->SetAccountAge(pQueryInfo->m_ulCharNID, pQueryInfo->m_szAccountID, pQueryInfo->m_dwAge);//나이정보 저장
			}
			else
			{
				//std::cout << "Global Authentication : ID = ["<< pQueryInfo->m_szAccountID << "] AuthString = [" << pQueryInfo->m_szGBAuthString << "]" << std::endl;
				INT32 iAge = 0;

				int iQueryResult = WebzenAuthFromDB( pQueryInfo, iAge );

				if(1 == iQueryResult)
				{	
					lResult = AGSMLOGINDB_AUTH_RESULT_SUCCESS;
					m_pAgsmLoginClient->SetAccountAge(pQueryInfo->m_ulCharNID, pQueryInfo->m_szAccountID, pQueryInfo->m_dwAge);//나이정보 저장

				}
				else
					lResult = AGSMLOGINDB_AUTH_RESULT_NOT_MEMBER;

				char szLog[256]={0,};

				sprintf_s(szLog,sizeof(szLog),"Auth #2 [%s] [%s] - [%d]",pQueryInfo->m_szAccountID , pQueryInfo->m_szGBAuthString , iQueryResult);

				AuLogFile_s(LOGIN_AUTH_FILE_NAME, szLog);
			}
#endif

		}
		else
		{
			_tprintf(_T("ProcessCheckAccount1 - 6\n"));
			if (m_pAgpmConfig->IsNoAccountAuth())
			{
				_tprintf(_T("ProcessCheckAccount1 - 7\n"));
				lResult	= AGSMLOGINDB_AUTH_RESULT_SUCCESS;
			}
			else if (m_pAgpmConfig->IsDBAccountAuth())
			{
				_tprintf(_T("ProcessCheckAccount1 - 8\n"));
				lResult = ProcessCheckAccountFromDB(pQueryInfo);
			}
			else
			{
				_tprintf(_T("ProcessCheckAccount1 - 9\n"));
				lResult	= pHttp->SendRequest(pQueryInfo->m_szAccountID, pQueryInfo->m_szPassword);
			}
		}
#endif

#ifdef _AREA_JAPAN_
		_tprintf(_T("ProcessCheckAccount1 - 4\n"));
		if ( false == GetGameEnv().IsDebugTest() )
		{
			//if(m_pAgsmLoginClient->IsReturnToLogin(pQueryInfo->m_ulCharNID))
			//{
			//	_tprintf(_T("ProcessCheckAccount1 - 5 - ReturnToLogin is TRUE\n"));
			//	lResult = AGSMLOGINDB_AUTH_RESULT_SUCCESS;
			//}
			//else
			//{
				_tprintf(_T("ProcessCheckAccount1 - 5\n"));
				int iAge = 0;
				lResult2 = g_SvrAuth.Auth( pQueryInfo->m_szAccountID, pQueryInfo->m_szHanAuthString, &iAge );
				lResult = ChangeHanAuthErrorToDefault( lResult2 );

				// 리턴값이 어찌되던 간에 iAge 를 저장한다. 2007.11.20. steeple
				m_pAgsmLoginClient->SetAccountAge(pQueryInfo->m_ulCharNID, pQueryInfo->m_szAccountID, iAge);
			//}
		}
		else
		{
			_tprintf(_T("ProcessCheckAccount1 - 6\n"));
			if (m_pAgpmConfig->IsNoAccountAuth())
			{
			_tprintf(_T("ProcessCheckAccount1 - 7\n"));
				lResult	= AGSMLOGINDB_AUTH_RESULT_SUCCESS;
			}
			else if (m_pAgpmConfig->IsDBAccountAuth())
			{
				_tprintf(_T("ProcessCheckAccount1 - 8\n"));
				lResult = ProcessCheckAccountFromDB(pQueryInfo);
			}
			else
			{
				_tprintf(_T("ProcessCheckAccount1 - 9\n"));
				lResult	= pHttp->SendRequest(pQueryInfo->m_szAccountID, pQueryInfo->m_szPassword);
			}
		}
#endif

#ifdef _AREA_WESTERN_	// 영국의 경우 blocking으로 인증을 처리한다
		
			_tprintf(_T("ProcessCheckAccount1 - 4\n"));
			if (m_pAgpmConfig->IsNoAccountAuth())
			{
				_tprintf(_T("ProcessCheckAccount1 - 5\n"));
				lResult = AGSMLOGINDB_AUTH_RESULT_SUCCESS;
			}
			else if (m_pAgpmConfig->IsDBAccountAuth())
			{
				_tprintf(_T("ProcessCheckAccount1 - 6\n"));
				lResult = ProcessCheckAccountFromDB(pQueryInfo);
			}

#endif
	}
	

	_tprintf(_T("ProcessCheckAccount1 - 10 : Result:%d\n"), lResult);
	//AuLogFile(LOGIN_ERROR_FILE_NAME, "AgsmLoginDB::ProcessCheckAccount1 - 1 : Result2:%d\n", lResult2);

	BOOL bResult = FALSE;
	switch (lResult)
		{
		case AGSMLOGINDB_AUTH_RESULT_SUCCESS :		// success
			{
				printf("Test!!!\n");
			// check archlord db.
			return ProcessCheckAccount2(pQueryInfo);
			}
			break;
		case AGSMLOGINDB_AUTH_RESULT_NOT_MEMBER :		// not member
			{
			bResult = m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_INVALID_ACCOUNT, NULL, pQueryInfo->m_ulCharNID);
			return FALSE;
			}
			break;
		case AGSMLOGINDB_AUTH_RESULT_PWD_FAIL :		// password fail
			{
			// increase failure count. if limit exceed, destroy
			if (m_pAgsmLoginClient->AddInvalidPasswordLog(pQueryInfo->m_ulCharNID))
				bResult = m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_INVALID_PASSWORD, NULL, pQueryInfo->m_ulCharNID);
			else
				{
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_INVALID_PASSWORD_LIMIT_EXCEED, NULL, pQueryInfo->m_ulCharNID);
				DestroyClient(pQueryInfo->m_ulCharNID);
				}
			}
			break;
		case AGSMLOGINDB_AUTH_RESULT_WAIT:				// 로긴서버에 쿼리를 했음
			bResult = TRUE;
			break;

		case AGSMLOGINDB_AUTH_RESULT_FOREIGN :			// 대기 상태의 외국인
		case AGSMLOGINDB_AUTH_RESULT_NEED_NAME :		// 실명전환이 필요한 회원
		case AGSMLOGINDB_AUTH_RESULT_IN_PENALTY :		// 경고
		case AGSMLOGINDB_AUTH_RESULT_IN_STOP :			// 이용정지
		case AGSMLOGINDB_AUTH_RESULT_ABUSE_MAIL :		// e-mail 도용
		case AGSMLOGINDB_AUTH_RESULT_PERM_STOP :		// 영구 이용 정지
		case AGSMLOGINDB_AUTH_RESULT_NEED_PA :			// 부모 동의가 필요한 회원
		case AGSMLOGINDB_AUTH_RESULT_BLOCKED_IP :		// 블록된 IP에서 접근
		case AGSMLOGINDB_AUTH_RESULT_NOT_CONNECT:		// 로그인 DB에 접근할수 없음
		default :
			DestroyClient(pQueryInfo->m_ulCharNID);
			break;
		}

	_tprintf(_T("ProcessCheckAccount1 - 11\n"));
	return bResult;
	}


BOOL AgsmLoginDB::ProcessCheckAccount2(AgsmLoginQueueInfo *pQueryInfo)
	{
	AuDatabase2		*pDatabase = NULL;
	UINT32			ulClientNID = pQueryInfo->m_ulCharNID;
	TCHAR			*pszAccountID = pQueryInfo->m_szAccountID;
	TCHAR			*pszPassword = pQueryInfo->m_szPassword;

	TCHAR *pszQuery	= NULL;

	_tprintf(_T("ProcessCheckAccount2 - 1\n"));

	// find db
	pDatabase = GetIdleMasterDatabase();
	if (!pDatabase)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginDB::ProcessCheckAccount2 - 1\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

		return FALSE;
	}

	_tprintf(_T("ProcessCheckAccount2 - 2\n"));

	if (m_pAgpmConfig->IsNoAccountAuth())
	{
		pszQuery = GetQuery(_T("ACCOUNT_NOAUTH_SELECT"), pDatabase->GetVender());
		_tprintf(_T("ProcessCheckAccount2 - 3\n"));
	}
	else
	{
		pszQuery = GetQuery(_T("ACCOUNT_GENERAL_SELECT"), pDatabase->GetVender());
		_tprintf(_T("ProcessCheckAccount2 - 4\n"));
	}

	_tprintf(_T("ProcessCheckAccount2 - 4.1 : Query:%s, Vender:%d\n"),
							pszQuery ? pszQuery : _T("null"),
							(INT32)pDatabase->GetVender());

	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;
	
	_tprintf(_T("ProcessCheckAccount2 - 5\n"));

	AuAutoReleaseDS Auto(pDatabase);

	// query
	pDatabase->StartTranaction();

	if (!pDatabase->SetQueryText(pszQuery) ||
		!pDatabase->SetParam(0, pQueryInfo->m_szAccountID, sizeof(pQueryInfo->m_szAccountID))
		)
		{
		_tprintf(_T("ProcessCheckAccount2 - 6\n"));
		pDatabase->EndQuery();
		pDatabase->AbortTransaction();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginDB::ProcessCheckAccount2 - 2\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

		return FALSE;
		}
	
	_tprintf(_T("ProcessCheckAccount2 - 7\n"));

	/*if (GetGameEnv().IsDebugTest())
	{
#ifdef _WEBZEN_AUTH_
		if (AUDATABASE2_QR_SUCCESS != pDatabase->ExecuteQuery())
		{
			_tprintf(_T("ProcessCheckAccount2 - 8\n"));
			pDatabase->EndQuery();
			pDatabase->AbortTransaction();

			return FALSE;
		}
#endif
	}
	else
	{*/
		// account
		if (AUDATABASE2_QR_SUCCESS != pDatabase->ExecuteQuery()&& !m_pAgpmConfig->IsNoAccountAuth())
			{
			_tprintf(_T("ProcessCheckAccount2 - 8\n"));
			pDatabase->EndQuery();
			pDatabase->AbortTransaction();
			
			// 아크로드 이용 동의가 필요하다.
			return m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_NOT_APPLIED, NULL, ulClientNID);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginDB::ProcessCheckAccount2 - 3\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
			}
	//}

	INT32	lAccountLV = 0;
	TCHAR	szPenalty[6];
	BOOL	bPenaltyFinished = TRUE;
	BOOL	bStatusExist = FALSE;


	// check account level and penalty
	ZeroMemory(szPenalty, sizeof(szPenalty));
	TCHAR *pszPenalty = pDatabase->GetQueryResult(1);
	if (pszPenalty)
		_tcsncpy(szPenalty, pszPenalty, 5);
		
	_tprintf(_T("ProcessCheckAccount2 - 9\n"));
	TCHAR *pszFinished = pDatabase->GetQueryResult(2);
	if (pszFinished)
		{
		if (_T('n') ==  *pszFinished || _T('N') ==  *pszFinished)
			bPenaltyFinished = FALSE;
		}
		
	TCHAR *pszExist = pDatabase->GetQueryResult(3);
	if (pszExist)
		{
		if (_T('y') ==  *pszExist || _T('Y') ==  *pszExist)
			bStatusExist = TRUE;
		}
		
	if (pDatabase->GetQueryResult(4))
		{
		lAccountLV = atoi(pDatabase->GetQueryResult(4));
		}
		
	_tprintf(_T("ProcessCheckAccount2 - 10\n"));

	// 한국에서만 하자 -ㅂ-;
	if (AP_SERVICE_AREA_KOREA == g_eServiceArea && /*false == GetGameEnv().IsDebugTest() &&*/ false == m_pAgpmConfig->IsNoAccountAuth())
	{
		// 2007.11.20. steeple
		// 나이체크가 한게임 모듈로 변경.
		BOOL bReturnToLogin = TRUE;
		bReturnToLogin = m_pAgsmLoginClient->IsReturnToLogin(ulClientNID);
		INT32 nLimitUnderAge = m_pAgpmConfig->GetLimitUnderAge();//JK_심야샷다운

		INT32 lAge = m_pAgsmLoginClient->GetAccountAge(ulClientNID, pQueryInfo->m_szAccountID);
		if(lAge < 15 /*&& bReturnToLogin == FALSE*/)
		{
			pDatabase->EndQuery();
			pDatabase->AbortTransaction();

			m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_NOT_ENOUGH_AGE, NULL, ulClientNID);
			DestroyClient(ulClientNID);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginDB::ProcessCheckAccount2 - 4\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

			return FALSE;
		}
		else if (nLimitUnderAge != 0 && lAge < nLimitUnderAge && m_pAgsmServerManager->CheckMidnightShutDownTime() == 0)//JK_심야샷다운
		{
			pDatabase->EndQuery();
			pDatabase->AbortTransaction();

			m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_MIDNIGHTSHUTDOWN_UNDERAGE, NULL, ulClientNID);
			DestroyClient(ulClientNID);
			return FALSE;
		}

		//if (FALSE == _CheckSocialNumber(pszSocialNo, 15))
		//	{
		//	pDatabase->EndQuery();
		//	pDatabase->AbortTransaction();
		//	m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_NOT_ENOUGH_AGE, NULL, ulClientNID);
		//	//DestroyClient(ulClientNID);
		//	return FALSE;
		//	}
	}

	_tprintf(_T("ProcessCheckAccount2 - 12\n"));
	// account level
	if (m_pAgpmAdmin->m_lConnectableAccountLevel > 0 && m_pAgpmAdmin->m_lConnectableAccountLevel > lAccountLV)
		{
		_tprintf(_T("ProcessCheckAccount2 - 13\n"));
		pDatabase->EndQuery();
		pDatabase->AbortTransaction();
		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_NOT_BETA_TESTER, NULL, ulClientNID);
		DestroyClient(ulClientNID);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginDB::ProcessCheckAccount2 - 5\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
		
		return FALSE;
		}

	_tprintf(_T("ProcessCheckAccount2 - 14\n"));
	// block
	if (!bPenaltyFinished && (0 == _tcsnicmp(szPenalty, _T("AC"), 2)))
		{
		CHAR szPauseDate[11];		// YYYY-MM-DD
		ZeroMemory(szPauseDate, sizeof(szPauseDate));
		if (m_pAgpmConfig->IsWebAccountAuth())
			strncpy(szPauseDate, pDatabase->GetQueryResult(6), 10);
		else
			strncpy(szPauseDate, pDatabase->GetQueryResult(5), 10);	
		
		_tprintf(_T("ProcessCheckAccount2 - 15\n"));
		pDatabase->EndQuery();
		pDatabase->AbortTransaction();
		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_ACCOUNT_BLOCKED, szPauseDate, ulClientNID);
		DestroyClient(ulClientNID);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginDB::ProcessCheckAccount2 - 6\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
		
		return FALSE;
		}

#ifdef _WEBZEN_AUTH_
	//////////////////////////////////////////////////////////////////////////
	// global guid
	TCHAR* szAccountGUID = pDatabase->GetQueryResult(7);
	_tprintf(_T("ProcessCheckAccount2 - 15-1 - szAccountGUID : %s\n"), szAccountGUID);
	if( szAccountGUID )
		pQueryInfo->m_dwAccountGUID = atoi(szAccountGUID);
#endif

	// success
	pDatabase->EndQuery();
	pDatabase->CommitTransaction();

	_tprintf(_T("ProcessCheckAccount2 - 16\n"));
	PlayerData	*pPlayerData	= GetPlayerDataForID(ulClientNID);
	if (pPlayerData)
		pPlayerData->lCustomData	= (INT32) bStatusExist;

	if (!ProcessCheckBillInfo(pQueryInfo))
		{
		_tprintf(_T("ProcessCheckAccount2 - 17\n"));
		// 결제 확인이 실패했다. 접속을 끊는다.
		DestroyClient(ulClientNID);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginDB::ProcessCheckAccount2 - 7\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
		
		return FALSE;
		}

	_tprintf(_T("ProcessCheckAccount2 - 18\n"));
	return TRUE;
	}


BOOL AgsmLoginDB::ProcessCheckAccountFromDB(AgsmLoginQueueInfo *pQueryInfo)
	{
	AuDatabase2		*pDatabase = NULL;

	// find db
	pDatabase = GetIdleMasterDatabase();
	if (!pDatabase)
		return AGSMLOGINDB_AUTH_RESULT_UNKNOWN;

	TCHAR *pszQuery	= GetQuery(_T("ACCOUNT_DB_SELECT"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;

	AuAutoReleaseDS Auto(pDatabase);

	if (!pDatabase->SetQueryText(pszQuery)
		|| !pDatabase->SetParam(0, pQueryInfo->m_szAccountID, sizeof(pQueryInfo->m_szAccountID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery()
		)
		{
		pDatabase->EndQuery();
		return AGSMLOGINDB_AUTH_RESULT_UNKNOWN;
		}
	
	if (0 >= pDatabase->GetReadRows())
		{
		pDatabase->EndQuery();
		return AGSMLOGINDB_AUTH_RESULT_NOT_MEMBER;
		}

	TCHAR *pszPassword = pDatabase->GetQueryResult(0);
	if (NULL == pszPassword || 0 != _tcscmp(pQueryInfo->m_szPassword, pszPassword))
		{
		pDatabase->EndQuery();
		return AGSMLOGINDB_AUTH_RESULT_PWD_FAIL;
		}

	pDatabase->EndQuery();

	return AGSMLOGINDB_AUTH_RESULT_SUCCESS;
	}


BOOL AgsmLoginDB::ProcessCheckBillInfo(AgsmLoginQueueInfo *pQueryInfo)
{
	if (!pQueryInfo)
		return FALSE;

	TCHAR			*pszAccountID	= pQueryInfo->m_szAccountID;
	UINT32			ulClientNID		= pQueryInfo->m_ulCharNID;

	if (!pszAccountID || !pszAccountID[0] || ulClientNID == 0)
		return FALSE;

	// check bill information
	///////////////////////////////////////////////////////



	// check pc-room ip
	///////////////////////////////////////////////////////
	SetAccountNameToPlayerContext(pszAccountID, ulClientNID);

	if (m_pAgsmGK->GetLKServer())
	{
		TCHAR	szAccountID[64];
		ZeroMemory(szAccountID, sizeof(szAccountID));

		_stprintf(szAccountID, _T("%5d:%s"), ulClientNID, pszAccountID);

		if (!m_pAgsmGK->SendPacketBillAuth(szAccountID, ulClientNID))
			return FALSE;

		AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
		if (!pModuleManager)
			return FALSE;

		if (!pModuleManager->m_AsIOCPServer.m_csTimer.AddTimer(10000, ulClientNID, (PVOID) this, CBCheckBillAuthResponse, NULL))
			return FALSE;
	}
	else
	{
		AgpdBillInfo	csBillInfo;
		ZeroMemory(&csBillInfo, sizeof(AgpdBillInfo));

		csBillInfo.m_bIsPCRoom	= FALSE;
#ifdef _WEBZEN_AUTH_
		csBillInfo.m_CashInfoGlobal.m_dwAccountGUID = pQueryInfo->m_dwAccountGUID;
		csBillInfo.m_dwPCRoomGuid					= pQueryInfo->m_dwPCRoomGuid;
		csBillInfo.m_dwClientCnt					= pQueryInfo->m_dwClientCnt;
		csBillInfo.m_dwAge							= pQueryInfo->m_dwAge; //JK_심야샷다운
#endif


		SetBillAuthResponse(pszAccountID, ulClientNID, csBillInfo.m_bIsPCRoom, FALSE, &csBillInfo, pQueryInfo->m_isLimited, pQueryInfo->m_isProtected);
	}

	return TRUE;
}


BOOL AgsmLoginDB::CBCheckBillAuthResponse(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
	{
	if (0 == lCID || !pClass)
		return FALSE;

	AgsmLoginDB	*pThis			= (AgsmLoginDB *)	pClass;

	PlayerData	*pPlayerData	= pThis->GetPlayerDataForID(lCID);
	if (!pPlayerData)
		return FALSE;

	// 아직도 ip check 요청에 대한 응답을 못받은 경우는 문제가 있으니 잠시후에 시도하라고 알려주고 끊어버린다.
	////////////////////////////////////////////////////////////////
	AgpdLoginStep	eCurrentStep	= pThis->m_pAgsmLoginClient->GetLoginStep(lCID);

	if (eCurrentStep <= AGPMLOGIN_STEP_NONE ||
		eCurrentStep >= AGPMLOGIN_STEP_UNKNOWN ||
		eCurrentStep == AGPMLOGIN_STEP_CONNECT)
		{
		pThis->DestroyClient(lCID);
		return FALSE;
		}

	return TRUE;
	}


BOOL AgsmLoginDB::CBReceiveBillAuthResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmLoginDB	*pThis	= (AgsmLoginDB *)	pClass;
	TCHAR		*pszAccountID	= (TCHAR *)	pData;
	PVOID		*ppvBuffer		= (PVOID *)	pCustData;

	UINT32		ulResult		= PtrToUint(ppvBuffer[0]);

	// ulResult 를 분석해 PC방인지, 가맹된 PC방인지를 알아낸다.
	////////////////////////////////////////////////////////////////

	AgpdBillInfo	csBillInfo;
	ZeroMemory(&csBillInfo, sizeof(AgpdBillInfo));

	if (ulResult == 1)
		csBillInfo.m_bIsPCRoom	= TRUE;

	TCHAR	szBuffer[6];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	CopyMemory(szBuffer, pszAccountID, sizeof(TCHAR) * 5);

	UINT32	ulNID	= atoi(szBuffer);

	return pThis->SetBillAuthResponse(pszAccountID + 6, ulNID, csBillInfo.m_bIsPCRoom, FALSE, &csBillInfo);
	}


BOOL AgsmLoginDB::CBGetCharacterCount(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;
	
	AgsmLoginDB *pThis = (AgsmLoginDB *) pClass;
	TCHAR *pszAccount = (TCHAR *) pData;
	UINT32 ulNID = *((INT32 *) pCustData);
	
	AuDatabase2 *pMaster = pThis->GetIdleMasterDatabase();
	if (!pMaster)
		return FALSE;
	
	AuAutoReleaseDS Auto(pMaster);
	
	TCHAR *pszQuery = pThis->GetQuery(_T("WORLD_MAPPING_SELECT"), pMaster->GetVender());
	ASSERT(NULL != pszQuery);
	
	if (NULL == pszQuery)
		return FALSE;

	// transaction doesn't required, but
	pMaster->StartTranaction();

	if (!pMaster->SetQueryText(pszQuery)
		|| !pMaster->SetParam(0, pszAccount, sizeof(TCHAR) * ((INT32)_tcslen(pszAccount) + 1))
		|| AUDATABASE2_QR_FAIL == pMaster->ExecuteQuery()
		)
		{
		pMaster->EndQuery();
		pMaster->AbortTransaction();
		return FALSE;
		}

	TCHAR szEncoded[AGPMWORLD_MAX_ENCODED_LENGTH];
	ZeroMemory(szEncoded, sizeof(szEncoded));
	TCHAR *psz = szEncoded;

	if (pMaster->GetReadRows() > 0)
		{
		do	{
			TCHAR *pszWorld = pMaster->GetQueryResult(0);
			INT32 lCount = _ttoi(pMaster->GetQueryResult(1));
			
			if (pszWorld && _T('\0') != *pszWorld && 0 != lCount)
				{
				psz += _stprintf(psz, _T("%s%c%d%c"), pszWorld, AGPMWORLD_DELIM1, lCount, AGPMWORLD_DELIM2);
				}
			} while(pMaster->GetNextRow());
		}
	
	pMaster->EndQuery();
	pMaster->CommitTransaction();

	if (_tcslen(szEncoded) > 0)
		{
		pThis->m_pAgsmWorld->SendPacketCharCount(pszAccount, szEncoded, ulNID);
		}
	
	return TRUE;
	}

BOOL AgsmLoginDB::CBReconnectClient(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmLoginDB *	pThis = (AgsmLoginDB *) pClass;

	return pThis->SetReconnectClient(*(UINT32 *) pData, (TCHAR *) pCustData);
}

BOOL AgsmLoginDB::SetBillAuthResponse(TCHAR *pszAccountID, UINT32 ulNID, BOOL IsPCRoom, BOOL IsAuthPCRoom, AgpdBillInfo *pcsBillInfo, int isLimited, int isProtected)
	{
	PlayerData	*pPlayerData	= GetPlayerDataForID(ulNID);
	if (!pPlayerData)
		return FALSE;

	// find db
	AuDatabase2 *pDatabase = GetIdleMasterDatabase();
	if (!pDatabase)
		return FALSE;
	
	AuAutoReleaseDS Auto(pDatabase);

	//
	UINT64 ullKey = _GetKey();

	if (!_WriteLoginStatus(pDatabase, (BOOL) pPlayerData->lCustomData, pszAccountID,
						   ullKey, TRUE))
		{
		DestroyClient(ulNID);
		return FALSE;
		}

	//JK_중복로그인
	//일단주석처리
	/*
	INT16 lIndex = 0;
	AgsdServer *pAgsdServer = NULL;

	while((pAgsdServer = m_pAgsmServerManager->GetGameServers(&lIndex)) != NULL)
	{
		if (NULL != pAgsdServer && pAgsdServer->m_bIsConnected)
		{
			m_pAgsmLogin->SendRemoveDuplicateAccountForLoginServer(pszAccountID, 0, pAgsdServer->m_dpnidServer);
			break;
		}

	}
	*/

	if (m_pAgsmLoginClient->SetCertificatedNID(ulNID, pszAccountID, ullKey))
	{
		AgsdAccount *pAgsdAccount = m_pAgsmAccountManager->AddAccount(pszAccountID, ulNID);
		if (pAgsdAccount && ulNID != pAgsdAccount->m_ulNID)
		{
			pAgsdAccount->m_ulNID = ulNID;
		}
		
		if (AP_SERVICE_AREA_KOREA == g_eServiceArea)
			GetEventCouponInfo(pDatabase, pAgsdAccount);

		m_pAgsmAccountManager->SetBillInfo(pszAccountID, pcsBillInfo);

		AgsmLoginEncryptInfo	*pEncryptInfo = m_pAgsmLoginClient->GetCertificatedNID(ulNID);
		if (pEncryptInfo && pcsBillInfo)
			pEncryptInfo->m_bIsPCBangIP	= pcsBillInfo->m_bIsPCRoom;

		m_pAgsmLoginClient->SetLoginStep(ulNID, AGPMLOGIN_STEP_PASSWORD_CHECK_OK);
		m_pAgsmLoginClient->SendSignOnSuccess(ulNID, pszAccountID, isLimited, isProtected);
	}

	return TRUE;
	}


BOOL AgsmLoginDB::SetReconnectClient(UINT32 ulNID, TCHAR *pszAccountName)
	{
	if (!pszAccountName || !pszAccountName[0])
		return FALSE;

	if (!m_pAgsmLoginClient->ProcessEncryptCode(NULL, ulNID, FALSE))
		return FALSE;

	UINT64 ullKey = _GetKey();

	if (m_pAgsmLoginClient->SetCertificatedNID(ulNID, pszAccountName, ullKey))
		{
		//#####################################
		//JK_중복로그인
		/*
		AgsdAccount *pAgsdAccount = m_pAgsmAccountManager->AddAccount(pszAccountName, ulNID);
		if (pAgsdAccount && ulNID != pAgsdAccount->m_ulNID)
			{
			//m_pAgsmLoginClient->SendResult(AGPMLOGIN_RESULT_DISCONNECTED_BY_DUPLICATE_ACCOUNT, NULL, pAgsdAccount->m_ulNID);
			//DestroyClient(pAgsdAccount->m_ulNID);
			pAgsdAccount->m_ulNID = ulNID;

			//if (pszSocialNo)
			//	strncpy(pAgsdAccount->m_szSocialNumber, pszSocialNo, 13);
			}
		*/
		AgsmLoginEncryptInfo* pLoginEncryptInfo = m_pAgsmLoginClient->GetCertificatedNID(ulNID);
		if(pLoginEncryptInfo)
			pLoginEncryptInfo->m_bReturnToLogin = TRUE;

		m_pAgsmLoginClient->SetLoginStep(ulNID, AGPMLOGIN_STEP_PASSWORD_CHECK_OK);
		
		AuDatabase2 *pDatabase = GetIdleMasterDatabase();
		AuAutoReleaseDS Auto1(pDatabase);
		
		_WriteLoginStatus(pDatabase, TRUE, pszAccountName, ullKey, TRUE);
		}
	else
		return FALSE;

	return TRUE;
	}

BOOL AgsmLoginDB::ProcessGetEKey(AgsmLoginQueueInfo* pQueryInfo)
{
#ifdef _AREA_CHINA_

	CSessionPtr* ptr = g_smgr.GetSessionByNid(pQueryInfo->m_ulCharNID);
	if (ptr)
	{
		g_csClientInterface.m_pAS->GiveEkey(*ptr, pQueryInfo->m_szPassword);
		delete ptr;
	}
#endif

	return TRUE;
}

BOOL AgsmLoginDB::ProcessGetUnion(AgsmLoginQueueInfo *pQueryInfo)
	{
	AuDatabase2		*pMaster = NULL;
	AuDatabase2		*pDatabase = NULL;
	UINT32	ulClientNID = pQueryInfo->m_ulCharNID;
	TCHAR	*pszQuery = NULL;

	// find db
	pMaster = GetIdleMasterDatabase();
	if (NULL == pMaster)
		return FALSE;
	
	AuAutoReleaseDS Auto1(pMaster);
	
	pDatabase = GetIdleGameDatabase(pQueryInfo->m_szDBName);
	if (NULL == pDatabase)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Cannot get db set of[%s]\n", pQueryInfo->m_szDBName);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return FALSE;
		}
	
	AuAutoReleaseDS Auto2(pDatabase);
	
	TCHAR	*pszChar;
	INT32	lSlot;

	// validate user connection
	if (!_CheckLoginStatus(pMaster, ulClientNID, pQueryInfo->m_szAccountID))
		{
		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_DISCONNECTED_BY_DUPLICATE_ACCOUNT, NULL, ulClientNID);
		DestroyClient(ulClientNID);
		return FALSE;
		}

	// get world
	AgpdWorld *pAgpdWorld = m_pAgpmWorld->GetWorld(pQueryInfo->m_szServerName);
	if (NULL == pAgpdWorld)
		return FALSE;
	
	// if NC17 server, check social no.

	if (g_eServiceArea == AP_SERVICE_AREA_KOREA)// && false == GetGameEnv().IsDebugTest())
		{
		if (pAgpdWorld->IsNC17())
			{
			BOOL bReturnToLogin = TRUE;
			bReturnToLogin = m_pAgsmLoginClient->IsReturnToLogin(ulClientNID);

			// 2007.11.20. steeple
			// 나이체크가 한게임 모듈로 변경. 
			INT32 lAge = m_pAgsmLoginClient->GetAccountAge(ulClientNID, pQueryInfo->m_szAccountID);
			if(lAge < 18 /*&& bReturnToLogin == FALSE*/)
			{
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_NOT_ENOUGH_AGE, NULL, ulClientNID);
				//DestroyClient(ulClientNID);
				return FALSE;
			}

			//pszQuery = GetQuery(_T("ACCOUNT_SSN_SELECT"), pMaster->GetVender());
			//ASSERT(NULL != pszQuery);
			//if (NULL == pszQuery)
			//	return FALSE;
			//
			//if (!pMaster->SetQueryText(pszQuery)
			//	|| !pMaster->SetParam(0, pQueryInfo->m_szAccountID, sizeof(pQueryInfo->m_szAccountID))
			//	|| AUDATABASE2_QR_FAIL == pMaster->ExecuteQuery()
			//	)
			//	{
			//	pMaster->EndQuery();
			//	pMaster->AbortTransaction();
			//	return FALSE;
			//	}

			//TCHAR *pszSocialNo = pMaster->GetQueryResult(0);
			//if (!pszSocialNo)
			//	{
			//	pMaster->EndQuery();
			//	pMaster->AbortTransaction();
			//	return FALSE;			
			//	}

			//if (FALSE == _CheckSocialNumber(pszSocialNo, 18))
			//	{
			//	pMaster->EndQuery();
			//	pMaster->AbortTransaction();
			//	m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_NOT_ENOUGH_AGE, NULL, ulClientNID);
			//	//DestroyClient(ulClientNID);
			//	return FALSE;
			//	}
			}
		}

	pszQuery = GetQuery(_T("CHARMASTER_ID_SLOT_SELECT"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;
	
	//pDatabase->StartTranaction();

	if (!pDatabase->SetQueryText(pszQuery) ||
		!pDatabase->SetParam(0, pQueryInfo->m_szAccountID, sizeof(pQueryInfo->m_szAccountID)) ||
		!pDatabase->SetParam(1, pQueryInfo->m_szServerName, sizeof(pQueryInfo->m_szServerName)) ||
		AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		pDatabase->EndQuery();
		//pDatabase->AbortTransaction();
		return FALSE;
		}

	//유니온이 없어져서 무조건 Human으로 보낸다.
	m_pAgsmLoginClient->SendUnionInfo(AUUNION_TYPE_RED_HEART, ulClientNID);
	
	UINT32 ulTotal = 0;
	if (0 < (ulTotal = pDatabase->GetReadRows()))
		{
		do	{
			pszChar = pDatabase->GetQueryResult(0);
			lSlot = atoi(pDatabase->GetQueryResult(1));
			
			if (pszChar && _T('\0') != *pszChar)
				m_pAgsmLoginClient->SendCharacterName(pszChar, ulTotal, lSlot, ulClientNID);
			
			} while(pDatabase->GetNextRow());
		}

	m_pAgsmLoginClient->SendCharacterNameFinish(pQueryInfo->m_szAccountID, ulClientNID);
	pDatabase->EndQuery();
	//pDatabase->CommitTransaction();
	
	return TRUE;
	}


BOOL AgsmLoginDB::ProcessGetCharacters(AgsmLoginQueueInfo *pQueryInfo)
	{
	AuDatabase2		*pMaster = NULL;
	AuDatabase2		*pDatabase = NULL;	
	UINT32	ulClientNID = pQueryInfo->m_ulCharNID;

	// find db
	pMaster = GetIdleMasterDatabase();
	if (NULL == pMaster)
		return FALSE;

	AuAutoReleaseDS Auto1(pMaster);
	
	pDatabase = GetIdleGameDatabase(pQueryInfo->m_szDBName);
	if (NULL == pDatabase)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Cannot get db set of[%s]\n", pQueryInfo->m_szDBName);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return FALSE;
		}

	TCHAR *pszQuery = GetQuery(_T("CHARMASTER_ID_SELECT"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;

	AuAutoReleaseDS Auto2(pDatabase);

	if (!_CheckLoginStatus(pMaster, ulClientNID, pQueryInfo->m_szAccountID))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _CheckLoginStatus [%s]\n", pQueryInfo->m_szAccountID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_DISCONNECTED_BY_DUPLICATE_ACCOUNT, NULL, ulClientNID);
		DestroyClient(ulClientNID);
		return FALSE;
		}

	pDatabase->StartTranaction();

	// character id select
	if (!pDatabase->SetQueryText(pszQuery) ||
		!pDatabase->SetParam(0, pQueryInfo->m_szAccountID, sizeof(pQueryInfo->m_szAccountID)) ||
		!pDatabase->SetParam(1, pQueryInfo->m_szServerName, sizeof(pQueryInfo->m_szServerName)) ||
		AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		pDatabase->EndQuery();
		pDatabase->AbortTransaction();
		return FALSE;
		}

	// character exists
	INT32 lFirstCID = 0;
	TCHAR szCharacter[5][AGSMACCOUNT_MAX_ACCOUNT_NAME + 1];
	UINT32 ulCount = 0;
	//BOOL bCompenChecked = FALSE;

	if (pDatabase->GetReadRows() > 0)
		{
		do	{
			TCHAR *pszCharID = pDatabase->GetQueryResult(0);
			if (!pszCharID)
				continue;
			
			_tcscpy(szCharacter[ulCount++], pszCharID);
			} while (pDatabase->GetNextRow());
		}
	
	for (UINT32 x=0; x<ulCount; ++x)
	{	
		INT32 lCID = _SelectCharacterView(pDatabase, szCharacter[x], NULL, ulClientNID);
		if (0 == lCID)
		{
			pDatabase->EndQuery();
			pDatabase->AbortTransaction();
			DestroyClient(ulClientNID);
			return FALSE;
		}
				
		AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacterLock(lCID);
		if (pAgpdCharacter)
		{
			//////////////////////////////////////////////////////////////////////////
			//
			m_pAgsmSkill->EnterGameWorld(pAgpdCharacter);
			m_pAgsmSkill->RecastSaveEvolutionSkill(pAgpdCharacter);

			m_pAgsmAccountManager->AddCharacterToAccount(pQueryInfo->m_szAccountID, pAgpdCharacter->m_lID, pAgpdCharacter->m_szID);
			m_pAgsmCharacter->SendCharacterAllInfo(pAgpdCharacter, ulClientNID);
			m_pAgpmCharacter->UpdateStatus(pAgpdCharacter, AGPDCHAR_STATUS_IN_LOGIN_PROCESS);
				
			if (0 == lFirstCID)
				lFirstCID = lCID;
			
			if (/*!bCompenChecked && */CheckCompensation(pQueryInfo, szCharacter[x]))
			{
				//bCompenChecked = TRUE;
			}
				
			pAgpdCharacter->m_Mutex.Release();
		}
	}
		
			
	m_pAgsmLoginClient->SendCharacterInfoFinish(lFirstCID, pQueryInfo->m_szAccountID, ulClientNID);

	pDatabase->EndQuery();
	pDatabase->CommitTransaction();

	m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);

	return TRUE;
	}


BOOL AgsmLoginDB::ProcessSelectCharacter(AgsmLoginQueueInfo *pQueryInfo)
	{
	AuDatabase2		*pDatabase = NULL;
	AuDatabase2		*pMaster = NULL;
	UINT32			ulGameServerNID = pQueryInfo->m_ulGameServerNID;
	UINT32			ulClientNID = pQueryInfo->m_ulCharNID;

	AgsmLoginEncryptInfo *pLoginEncryptInfo = m_pAgsmLoginClient->GetCertificatedNID(ulClientNID);
	if (NULL == pLoginEncryptInfo || 0 == pLoginEncryptInfo->m_ullKey)
	{
		return FALSE;	
	}

	// find db
	pMaster = GetIdleMasterDatabase();
	if (NULL == pMaster)
	{
		return FALSE;
	}
	
	AuAutoReleaseDS Auto1(pMaster);
	
	pDatabase = GetIdleGameDatabase(pQueryInfo->m_szDBName);
	if (NULL == pDatabase)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Cannot get db set of[%s]\n", pQueryInfo->m_szDBName);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
	
		return FALSE;
		}
	
	AuAutoReleaseDS Auto2(pDatabase);	

	if (!_CheckLoginStatus(pMaster, ulClientNID, pQueryInfo->m_szAccountID))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _CheckLoginStatus [%s]\n", pQueryInfo->m_szAccountID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_DISCONNECTED_BY_DUPLICATE_ACCOUNT, NULL, ulClientNID);
		pLoginEncryptInfo->m_lCompenID = 0;
		ZeroMemory(pLoginEncryptInfo->m_szChar4Compen, sizeof(pLoginEncryptInfo->m_szChar4Compen));
		DestroyClient(ulClientNID);
		return FALSE;
		}

	CHAR	szNewName[AGPDCHARACTER_MAX_ID_LENGTH * 2];
	ZeroMemory(szNewName, sizeof(szNewName));

	m_pAgsmCharacter->AddServerNameToCharName(pQueryInfo->m_szCharacterID, pQueryInfo->m_szServerName, szNewName);

	INT32 lCID = m_pAgsmAccountManager->GetCIDFromAccount(pQueryInfo->m_szAccountID,
														  szNewName);
	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(lCID);
	if (!pAgpdCharacter)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : No Selected Character [%d]\n", lCID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);
		return FALSE;
		}

	if (m_pAgpmLogin->IsDuplicatedCharacterOfMigration(m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID)))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : IsDuplicatedCharacterOfMigration [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);
		return FALSE;
		}

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;

	pMaster->StartTranaction();
	pDatabase->StartTranaction();


	TCHAR *pszQuery = GetQuery(_T("CHARMASTER_ID_SELECT"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;

	// character id select
	if (!pDatabase->SetQueryText(pszQuery) ||
		!pDatabase->SetParam(0, pQueryInfo->m_szAccountID, sizeof(pQueryInfo->m_szAccountID)) ||
		!pDatabase->SetParam(1, pQueryInfo->m_szServerName, sizeof(pQueryInfo->m_szServerName)) ||
		AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Cannot Get Characters in SelectCharacter [%s][%s]\n", pQueryInfo->m_szAccountID, pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);
		m_pAgpmCharacter->RemoveCharacter(pAgpdCharacter->m_lID, FALSE, FALSE);
		pLoginEncryptInfo->m_lCompenID = 0;
		ZeroMemory(pLoginEncryptInfo->m_szChar4Compen, sizeof(pLoginEncryptInfo->m_szChar4Compen));
		DestroyClient(ulClientNID);
		goto ps_fail;
		}

	BOOL bFound = FALSE;
	if (pDatabase->GetReadRows() > 0)
		{
		do	{
			TCHAR *pszCharID = pDatabase->GetQueryResult(0);
			if (!pszCharID && 0 >= _tcslen(pszCharID))
				continue;
			
			if (0 == _tcscmp(pszCharID, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID)))
				{
				bFound = TRUE;
				break;
				}
			} while (pDatabase->GetNextRow());
		}

	if (!bFound)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Can't find char[%s] of account[%s]\n", pAgpdCharacter->m_szID, pQueryInfo->m_szAccountID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);

		m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);
		m_pAgpmCharacter->RemoveCharacter(pAgpdCharacter->m_lID, FALSE, FALSE);
		pLoginEncryptInfo->m_lCompenID = 0;
		ZeroMemory(pLoginEncryptInfo->m_szChar4Compen, sizeof(pLoginEncryptInfo->m_szChar4Compen));
		DestroyClient(ulClientNID);
		goto ps_fail;		
		}

	pDatabase->EndQuery();

	// if game server is not connected, 
	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pQueryInfo->m_szServerName);
	if (!pAgsdServer || FALSE == pAgsdServer->m_bIsConnected)	// no server to enter
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Game Server disconnected [%s]\n", pQueryInfo->m_szServerName);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
	
		m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);
		m_pAgpmCharacter->RemoveCharacter(pAgpdCharacter->m_lID, FALSE, FALSE);
		pLoginEncryptInfo->m_lCompenID = 0;
		ZeroMemory(pLoginEncryptInfo->m_szChar4Compen, sizeof(pLoginEncryptInfo->m_szChar4Compen));
		DestroyClient(ulClientNID);
		goto ps_fail;
		}

	/*if (FALSE == _SelectCharacterViewExceptMaster(pDatabase, pAgpdCharacter))
		{
		AuLogFile(_AGSMLOGINDB_LOG_, _T("!!! Error : _SelectCharacterViewExceptMaster [%s]\n"), pAgpdCharacter->m_szID);
		m_pAgpmCharacter->RemoveCharacter(pAgpdCharacter->m_lID, FALSE, FALSE);
		pLoginEncryptInfo->m_lCompenID = 0;
		ZeroMemory(pLoginEncryptInfo->m_szChar4Compen, sizeof(pLoginEncryptInfo->m_szChar4Compen));
		DestroyClient(ulClientNID);
		goto ps_fail;		
		}*/

	AgsdCharacter *pAgsdCharacter	= m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	
	if (FALSE == pAgsdCharacter->m_bIsCreatedChar)
		{
		if (FALSE == _SelectCharacterExceptView(pDatabase, pAgpdCharacter, pQueryInfo->m_szServerName))
			{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCharacterExceptView [%s]\n", pAgpdCharacter->m_szID);
			AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
			
			m_pAgpmCharacter->RemoveCharacter(pAgpdCharacter->m_lID, FALSE, FALSE);
			pLoginEncryptInfo->m_lCompenID = 0;
			ZeroMemory(pLoginEncryptInfo->m_szChar4Compen, sizeof(pLoginEncryptInfo->m_szChar4Compen));
			DestroyClient(ulClientNID);
			goto ps_fail;
			}
		}
	else
		{
		if (FALSE == _SelectCashItems(pDatabase, pAgpdCharacter)	// 생성된 캐릭터도 쿠폰등에 의해 생길 수 있다.
			|| FALSE == _SelectAccountWorld(pDatabase, pAgpdCharacter, pQueryInfo->m_szServerName)
			|| FALSE == _SelectBankItems(pDatabase, pAgpdCharacter, pQueryInfo->m_szServerName)
			|| FALSE == _SelectTitle(pDatabase, pAgpdCharacter))
			{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectETC [%s]\n", pAgpdCharacter->m_szID);
			AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
			
			m_pAgpmCharacter->RemoveCharacter(pAgpdCharacter->m_lID, FALSE, FALSE);
			pLoginEncryptInfo->m_lCompenID = 0;
			ZeroMemory(pLoginEncryptInfo->m_szChar4Compen, sizeof(pLoginEncryptInfo->m_szChar4Compen));
			DestroyClient(ulClientNID);
			goto ps_fail;
			}
		}
	
	pszQuery = GetQuery(_T("LOGIN_STATUS_LEVEL_SELECT"), pMaster->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		{
		pMaster->EndQuery();
		goto ps_fail;
		}

	if (!pMaster->SetQueryText(pszQuery) ||
		!pMaster->SetParam(0, pQueryInfo->m_szAccountID, sizeof(pQueryInfo->m_szAccountID)) ||
		AUDATABASE2_QR_FAIL == pMaster->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : ExecuteQuery [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pMaster->EndQuery();
		goto ps_fail;
		}
	
	INT16	nLevel = 0;
	if (pMaster->GetReadRows() > 0)
		{
		nLevel = (INT16) atoi(pMaster->GetQueryResult(0));
		}
	pMaster->EndQuery();

	pAgsdCharacter->m_dpnidCharacter = ulClientNID;

	if (0 != pLoginEncryptInfo->m_lCompenID &&
		0 == _tcscmp(pLoginEncryptInfo->m_szChar4Compen, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID)))
		{
		AgpdLoginCompenMaster *pCompenMaster = NULL;
		INT32 lCount = 0;
		if (GetCompenMaster(&pCompenMaster, lCount, pLoginEncryptInfo->m_lCompenID))
			{
			if (1 == lCount)
				PayCompensation(pQueryInfo->m_szDBName, pAgpdCharacter, pAgsdServer->m_szWorld, &(pCompenMaster[0]));
			}
		delete [] pCompenMaster;
		}

	if (AP_SERVICE_AREA_KOREA == g_eServiceArea)
	{
		// process coupon
		ProcessCoupon(pQueryInfo->m_szDBName, pAgpdCharacter, pQueryInfo->m_szServerName);
		
		//ProcessEventCoupon(pMaster, pDatabase, pAgpdCharacter, pQueryInfo->m_szServerName);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	if(pAgpdCharacter->m_bIsEvolution)
	{
		m_pAgsmSkill->RemoveBuffedSkillEvolution(pAgpdCharacter);
	}

	//////////////////////////////////////////////////////////////////////////
	// 게임서버에 들어간다.
	_WriteLoginStatus(pMaster, TRUE, pQueryInfo->m_szAccountID, 
					pLoginEncryptInfo->m_ullKey, pAgsdServer->m_szIP,
					m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), pAgpdCharacter->m_lID, FALSE);

	m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_ENTER_GAME_WORLD);
	ulGameServerNID = pAgsdServer->m_dpnidServer;
	//pAgsdCharacter->m_dpnidCharacter = ulClientNID;
	pAgsdCharacter->m_ulServerID = pAgsdServer->m_lServerID;
	ZeroMemory(pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID));
	_tcsncpy(pAgsdCharacter->m_szAccountID, pQueryInfo->m_szAccountID, _tcslen(pQueryInfo->m_szAccountID));
	pAgsdCharacter->m_lMemberBillingNum = pQueryInfo->m_lCID;
	m_pAgsmAccountManager->RemoveCharacterExceptOne(pQueryInfo->m_szAccountID, pAgpdCharacter->m_szID);
	m_pAgsmCharacter->GenerateAuthKey(pAgpdCharacter);
	m_pAgsmCharacter->SetWaitOperation(pAgpdCharacter, AGSMCHARACTER_WAIT_OPERATION_REQUEST_NEW_CID);
	m_pAgsmCharacter->SendPacketRequestNewCID(pAgpdCharacter->m_lID, ulGameServerNID);

	// admin level setting
	m_pAgpmAdmin->SetAdminCharacter(pAgpdCharacter, nLevel);
	
	pLoginEncryptInfo->m_lCompenID = 0;
	ZeroMemory(pLoginEncryptInfo->m_szChar4Compen, sizeof(pLoginEncryptInfo->m_szChar4Compen));

	// transaction
	pMaster->CommitTransaction();
	pDatabase->CommitTransaction();

	Sleep(50);

	return TRUE;
	
  ps_fail:
	pMaster->AbortTransaction();
	pDatabase->AbortTransaction();
	pLoginEncryptInfo->m_lCompenID = 0;
	ZeroMemory(pLoginEncryptInfo->m_szChar4Compen, sizeof(pLoginEncryptInfo->m_szChar4Compen));
	return FALSE;
	}




//	Create/Remove
//=============================================
//
BOOL AgsmLoginDB::ProcessCreateCharacter(AgsmLoginQueueInfo *pQueryInfo)
	{
	AuDatabase2		*pMaster = NULL;
	AuDatabase2		*pDatabase = NULL;
	TCHAR			*pszAccountID = pQueryInfo->m_szAccountID;
	BOOL			bExist = FALSE;
	INT32			lSlotCount = 0;
	INT8			cSlotNext = 0;
	UINT32			ulClientNID = pQueryInfo->m_ulCharNID;
	AgpdCharacter	*pAgpdCharacter = m_pAgpmCharacter->GetCharacter(pQueryInfo->m_lCID);
	AgsdCharacter	*pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);

	// find db
	pMaster = GetIdleMasterDatabase();
	if (NULL == pMaster)
		return FALSE;
	
	AuAutoReleaseDS Auto1(pMaster);
	
	pDatabase = GetIdleGameDatabase(pQueryInfo->m_szDBName);
	if (NULL == pDatabase)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Cannot get db set of[%s]\n", pQueryInfo->m_szDBName);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return FALSE;
		}
	
	AuAutoReleaseDS Auto2(pDatabase);	

	if (!pAgpdCharacter || !pAgsdCharacter)
		{
		m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);
		return FALSE;
		}

	// validate current connection from database
	if (FALSE == _CheckLoginStatus(pMaster, ulClientNID, pszAccountID))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : ProcessCreateCharacter _CheckLoginStatus [%s]\n", pszAccountID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_DISCONNECTED_BY_DUPLICATE_ACCOUNT, NULL, ulClientNID);
		DestroyClient(ulClientNID);
		return FALSE;
		}
	
	// let's go
	pMaster->StartTranaction();
	pDatabase->StartTranaction();

	//////////////////////////////////////////////////////////////////////////
	//
	TCHAR *pszQuery = GetQuery(_T("CHARMASTER_ID_ON_CREATE"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);

	INT32 lCode = 0;
	if (NULL == pszQuery
		|| !pDatabase->SetQueryText(pszQuery)
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| !pDatabase->SetParam(1, &lCode, TRUE)
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
	{
		goto pc_fail;
	}

	if (lCode > 0) // character id already exist
	{
		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_CHAR_NAME_ALREADY_EXIST, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), ulClientNID);
		bExist = TRUE;
		goto pc_fail;
	}

	pDatabase->EndQuery();

	//////////////////////////////////////////////////////////////////////////
	//
	pszQuery = GetQuery(_T("CHARMASTER_SLOT_ON_CREATE"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		{
		goto pc_fail;
		}

	INT8 cQueryResult;
	if (!pDatabase->SetQueryText(pszQuery)
		|| !pDatabase->SetParam(0, pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID))
		|| !pDatabase->SetParam(1, pQueryInfo->m_szServerName, sizeof(pQueryInfo->m_szServerName))
		|| AUDATABASE2_QR_FAIL == (cQueryResult = pDatabase->ExecuteQuery()))
		{
		goto pc_fail;
		}

	// if created character exist
	if (AUDATABASE2_QR_SUCCESS == cQueryResult)
		{
		ApSafeArray<INT8, 10> Slots;
		Slots.MemSetAll();
		do {
			Slots[lSlotCount++] = atoi(pDatabase->GetQueryResult(0));
			} while(pDatabase->GetNextRow());
		
		if (lSlotCount >= AGPMLOGIN_MAX_CHAR_PER_ACCOUNT)	// max 3 character
			{
			m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_FULL_SLOT, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), ulClientNID);
			goto pc_fail;
			}
		
		BOOL bFind = FALSE;
		for (INT32 l=0; l<lSlotCount; ++l)
			{
			if (Slots[l] > l)
				{
				cSlotNext = (INT8) l;
				bFind = TRUE;
				break;
				}
			}
		
		if (!bFind)
			cSlotNext = (INT8) lSlotCount;

		}
	pDatabase->EndQuery();


	// insert
	if (!_InsertCharacter(pDatabase, pAgpdCharacter, cSlotNext, pQueryInfo->m_szServerName) ||
		!_InsertItem(pDatabase, pAgpdCharacter) ||
		!_InsertSkill(pDatabase, pAgpdCharacter))
		{
		goto pc_fail;
		}

	// insert world mapping table
	if (!_WriteWorldMapping(pMaster, pAgsdCharacter->m_szAccountID, pQueryInfo->m_szServerName, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID)))
		goto pc_fail;

	// success, commit and send to client
	pMaster->EndQuery();
	pDatabase->EndQuery();
	pMaster->CommitTransaction();
	pDatabase->CommitTransaction();
	
	m_pAgsmLoginClient->SendNewCharacterName(m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), lSlotCount, cSlotNext, ulClientNID);
	m_pAgsmCharacter->SendCharacterAllInfo(pAgpdCharacter, ulClientNID);
	m_pAgsmLoginClient->SendNewCharacterInfoFinish(pAgpdCharacter->m_lID, ulClientNID);
	m_pAgsmAccountManager->AddCharacterToAccount(pAgsdCharacter->m_szAccountID, pAgpdCharacter->m_lID, pAgpdCharacter->m_szID);
	m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);

	return TRUE;

  pc_fail:		// fail
	pMaster->EndQuery();
	pDatabase->EndQuery();
	pMaster->AbortTransaction();
	pDatabase->AbortTransaction();
	m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);
	if (pAgpdCharacter)
		m_pAgpmCharacter->RemoveCharacter(pAgpdCharacter->m_lID);

	return FALSE;
	}


BOOL AgsmLoginDB::ProcessRemoveCharacter(AgsmLoginQueueInfo *pQueryInfo)
	{
	AuDatabase2		*pMaster = NULL;
	AuDatabase2		*pDatabase = NULL;	
	BOOL			bResult = FALSE;
	UINT32			ulClientNID = pQueryInfo->m_ulCharNID;

	// find db
	pMaster = GetIdleMasterDatabase();
	if (NULL == pMaster)
		return FALSE;
	
	AuAutoReleaseDS Auto1(pMaster);
	
	pDatabase = GetIdleGameDatabase(pQueryInfo->m_szDBName);
	if (NULL == pDatabase)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Cannot get db set of[%s]\n", pQueryInfo->m_szDBName);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return FALSE;
		}
	
	AuAutoReleaseDS Auto2(pDatabase);	

	if (!_CheckLoginStatus(pMaster, ulClientNID, pQueryInfo->m_szAccountID))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : ProcessRemoveCharacter _CheckLoginStatus [%s]\n", pQueryInfo->m_szAccountID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_DISCONNECTED_BY_DUPLICATE_ACCOUNT, NULL, ulClientNID);
		DestroyClient(ulClientNID);
		return FALSE;		
		}
	//적용전까지 주석처리
/*
	//JK_케릭삭제비번확인
	if (!_CheckAccountPassword(pMaster, ulClientNID, pQueryInfo->m_szAccountID, pQueryInfo->m_szPassword))
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : ProcessRemoveCharacter _CheckAccountPassword [%s]\n", pQueryInfo->m_szAccountID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);

		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_INVALID_PASSWORD, NULL, ulClientNID);

		m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);

		return bResult;
	}
*/	

	CHAR	szNewName[AGPDCHARACTER_MAX_ID_LENGTH * 2];
	ZeroMemory(szNewName, sizeof(szNewName));
	m_pAgsmCharacter->AddServerNameToCharName(pQueryInfo->m_szCharacterID, pQueryInfo->m_szServerName, szNewName);

	if (0 == m_pAgsmAccountManager->GetCIDFromAccount(pQueryInfo->m_szAccountID, szNewName))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : ProcessRemoveCharacter GetCIDFromAccount [%s][%s]\n", pQueryInfo->m_szAccountID, szNewName);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		DestroyClient(ulClientNID);
		return FALSE;
		}

	pMaster->StartTranaction();
	pDatabase->StartTranaction();

	INT32 lResult = 0;
	if (!pDatabase->SetQueryText(GetQuery(_T("CHARACTER_DELETE"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, pQueryInfo->m_szCharacterID, sizeof(pQueryInfo->m_szCharacterID))
		|| !pDatabase->SetParam(1, &lResult, TRUE)
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		pMaster->EndQuery();
		pDatabase->EndQuery();
		pMaster->AbortTransaction();
		pDatabase->AbortTransaction();
		}
	else
		{
		// remove world mapping
		if (0 != lResult
			|| !_WriteWorldMapping(pMaster, pQueryInfo->m_szAccountID, pQueryInfo->m_szServerName, m_pAgsmCharacter->GetRealCharName(pQueryInfo->m_szCharacterID), TRUE))
			{
			pMaster->EndQuery();
			pDatabase->EndQuery();
			pMaster->AbortTransaction();
			pDatabase->AbortTransaction();

			if (1 == lResult)
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_CANT_DELETE_CHAR_B4_1DAY, NULL, ulClientNID);
			else if( lResult == 2 )
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_CANT_DELETE_CHAR_GUILD_MASTER, NULL, ulClientNID);
			}
		else
			{
			// success
			pMaster->EndQuery();
			pDatabase->EndQuery();
			pMaster->CommitTransaction();
			pDatabase->CommitTransaction();
			bResult = TRUE;
			
			m_pAgsmLoginClient->SendCharacterRemoved(pQueryInfo->m_szAccountID, m_pAgsmCharacter->GetRealCharName(pQueryInfo->m_szCharacterID), ulClientNID);
			m_pAgsmAccountManager->RemoveCharacterFromAccount(pQueryInfo->m_szAccountID, szNewName);
			m_pAgpmCharacter->RemoveCharacter(szNewName);

			// notify to game server
			AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pQueryInfo->m_szServerName);
			if (pAgsdServer)
				m_pAgsmCharManager->SendDeleteCompletePacket(pQueryInfo->m_szCharacterID, pAgsdServer->m_dpnidServer);
			}
		}

	m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);

	return bResult;
	}


BOOL AgsmLoginDB::ProcessRenameCharacter(AgsmLoginQueueInfo *pQueryInfo)
	{
	AuDatabase2		*pMaster = NULL;
	AuDatabase2		*pDatabase = NULL;	
	BOOL			bResult = FALSE;
	UINT32			ulClientNID = pQueryInfo->m_ulCharNID;

	// find db
	pMaster = GetIdleMasterDatabase();
	if (NULL == pMaster)
		return FALSE;
	
	AuAutoReleaseDS Auto1(pMaster);
	
	pDatabase = GetIdleGameDatabase(pQueryInfo->m_szDBName);
	if (NULL == pDatabase)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Cannot get db set of[%s]\n", pQueryInfo->m_szDBName);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return FALSE;
		}
	
	AuAutoReleaseDS Auto2(pDatabase);	

	if (!_CheckLoginStatus(pMaster, ulClientNID, pQueryInfo->m_szAccountID))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : ProcessRenameCharacter _CheckLoginStatus [%s]\n", pQueryInfo->m_szAccountID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_DISCONNECTED_BY_DUPLICATE_ACCOUNT, NULL, ulClientNID);
		DestroyClient(ulClientNID);
		return FALSE;		
		}

	CHAR	szNewName[AGPDCHARACTER_MAX_ID_LENGTH * 2];
	ZeroMemory(szNewName, sizeof(szNewName));

	m_pAgsmCharacter->AddServerNameToCharName(pQueryInfo->m_szCharacterID, pQueryInfo->m_szServerName, szNewName);

	INT32 lCID = m_pAgsmAccountManager->GetCIDFromAccount(pQueryInfo->m_szAccountID, szNewName);
	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(lCID);
	if (!pAgpdCharacter)
		{
		m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);
		return FALSE;
		}

	if (FALSE == m_pAgpmLogin->IsDuplicatedCharacterOfMigration(pQueryInfo->m_szCharacterID))
		{
		m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);
		return FALSE;
		}

	TCHAR *pszQuery	= GetQuery(_T("CHARACTER_RENAME"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		{
		m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);
		return FALSE;
		}

	pDatabase->StartTranaction();
	INT32 lCode = AGSMLOGINDB_RENAME_RESULT_FAIL;

	if (!pDatabase->SetQueryText(pszQuery)
		|| !pDatabase->SetParam(0, pQueryInfo->m_szCharacterID, sizeof(pQueryInfo->m_szCharacterID))
		|| !pDatabase->SetParam(1, pQueryInfo->m_szNewCharacterID, sizeof(pQueryInfo->m_szNewCharacterID))
		|| !pDatabase->SetParam(2, &lCode, TRUE)
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		pDatabase->EndQuery();
		pDatabase->AbortTransaction();
		
		m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_RENAME_FAIL, NULL, ulClientNID);
		}
	else
		{
		// success	
		pDatabase->EndQuery();
		pDatabase->CommitTransaction();

		switch (lCode)
			{
			case AGSMLOGINDB_RENAME_RESULT_SUCCESS :
				{
				// 2008.07.09. steeple
				// 
				// Character Rename World mapping 처리 부분이 Procedure 에 묶어서 처리한다.
				//

				//pDatabase->StartTranaction();
				
				// send rename result succeede
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_RENAME_SUCCESS, NULL, ulClientNID);
				
				// send character removed
				m_pAgsmLoginClient->SendCharacterRemoved(pQueryInfo->m_szAccountID, pQueryInfo->m_szCharacterID, ulClientNID);
				m_pAgsmAccountManager->RemoveCharacterFromAccount(pQueryInfo->m_szAccountID, szNewName);
				m_pAgpmCharacter->RemoveCharacter(szNewName);

				//// remove world mapping
				//pMaster->StartTranaction();
				//if (_WriteWorldMapping(pMaster, pQueryInfo->m_szAccountID, pQueryInfo->m_szServerName, m_pAgsmCharacter->GetRealCharName(pQueryInfo->m_szCharacterID), TRUE))
				//	pMaster->CommitTransaction();
				//else
				//	pMaster->AbortTransaction();
				//pMaster->EndQuery();

				// notify to game server
				AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pQueryInfo->m_szServerName);
				if (pAgsdServer)
					{
					m_pAgsmLoginServer->SendRenameCharacter(pQueryInfo->m_szNewCharacterID, m_pAgsmCharacter->GetRealCharName(pQueryInfo->m_szCharacterID), pAgsdServer->m_dpnidServer);
					m_pAgsmCharManager->SendDeleteCompletePacket(m_pAgsmCharacter->GetRealCharName(pQueryInfo->m_szCharacterID), pAgsdServer->m_dpnidServer);
					}

				// send character added
				INT8 cSlot = 0;
				INT32 lCID = _SelectCharacterView(pDatabase, pQueryInfo->m_szNewCharacterID, &cSlot);
				if (0 == lCID)
					{
					//pDatabase->EndQuery();
					//pDatabase->AbortTransaction();
					DestroyClient(ulClientNID);
					return FALSE;
					}

				AgpdCharacter *pAgpdCharacter_New = m_pAgpmCharacter->GetCharacterLock(lCID);
				
				if (pAgpdCharacter_New)
					{
					m_pAgsmLoginClient->SendNewCharacterName(m_pAgsmCharacter->GetRealCharName(pAgpdCharacter_New->m_szID), 0, cSlot, ulClientNID);
					m_pAgsmCharacter->SendCharacterAllInfo(pAgpdCharacter_New, ulClientNID);
					m_pAgsmLoginClient->SendNewCharacterInfoFinish(pAgpdCharacter_New->m_lID, ulClientNID);
					m_pAgsmAccountManager->AddCharacterToAccount(pQueryInfo->m_szAccountID, pAgpdCharacter_New->m_lID, pAgpdCharacter_New->m_szID);
					
					pAgpdCharacter_New->m_Mutex.Release();

					//// add world mapping
					//pMaster->StartTranaction();
					//if (_WriteWorldMapping(pMaster, pQueryInfo->m_szAccountID, pQueryInfo->m_szServerName, pQueryInfo->m_szNewCharacterID))
					//	pMaster->CommitTransaction();
					//else
					//	pMaster->AbortTransaction();
					//pMaster->EndQuery();
					}
				
				//pDatabase->EndQuery();
				//pDatabase->CommitTransaction();
				}
				break;

			case AGSMLOGINDB_RENAME_RESULT_SAME_OLD_NEW_ID :
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_RENAME_SAME_OLD_NEW_ID, NULL, ulClientNID);
				break;
				
			case AGSMLOGINDB_RENAME_RESULT_NOT_EXIST_ID :
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_RENAME_FAIL, NULL, ulClientNID);
				break;
				
			case AGSMLOGINDB_RENAME_RESULT_DUP_NEW_ID :
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_CHAR_NAME_ALREADY_EXIST, NULL, ulClientNID);
				break;

			case AGSMLOGINDB_RENAME_RESULT_FAIL :
			default :
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_RENAME_FAIL, NULL, ulClientNID);
				break;	
			}
		}

	m_pAgsmLoginClient->SetLoginStep(ulClientNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);

	return bResult;
	}




//	Status Update
//=============================================
//
BOOL AgsmLoginDB::ProcessResetLoginStatusByDisconnectFromGameServer(AgsmLoginQueueInfo *pQueryInfo)
	{
	AuDatabase2 *pMaster = GetIdleMasterDatabase();
	if (NULL == pMaster)
		return FALSE;
	
	AuAutoReleaseDS Auto1(pMaster);
	
	return _WriteLoginStatus(pMaster, TRUE, 
							 pQueryInfo->m_szAccountID, 0, _T(""), _T(""), 0, TRUE);
	}


BOOL AgsmLoginDB::ProcessResetLoginStatusByDisconnectFromLoginServer(AgsmLoginQueueInfo *pQueryInfo)
	{
	return m_pAgsmAccountManager->RemoveAccount(pQueryInfo->m_szAccountID, TRUE);
	}


BOOL AgsmLoginDB::ProcessSetLoginStatusInGameServer(AgsmLoginQueueInfo *pQueryInfo)
	{
	return m_pAgsmAccountManager->RemoveAccount(pQueryInfo->m_szAccountID, FALSE);
	}


BOOL AgsmLoginDB::ProcessReturnToLoginServer(AgsmLoginQueueInfo *pQueryInfo)
	{
	return TRUE;
	}

//	Insert Helper
//=============================================
//
BOOL AgsmLoginDB::_InsertCharacter(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter, INT8 cSlot, TCHAR *pszServerName)
	{
	// charmaster table
	if (!pDatabase->SetQueryText(GetQuery(_T("CHARMASTER_INSERT"), pDatabase->GetVender()))
		|| !m_pAgsmCharacter->SetParamMasterInsertQuery5(pDatabase, pAgpdCharacter, cSlot, pszServerName))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _InsertCharacter CHARMASTER_INSERT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);

		pDatabase->EndQuery();
		return FALSE;
		}
	pDatabase->EndQuery();

	// chardetail table
	TCHAR szQuickBelt[AGPMUISTATUS_MAX_QUICKBELT_STRING + 1];
	ZeroMemory(szQuickBelt, sizeof(szQuickBelt));
	m_pAgsmUIStatus->EncodingDefaultQBeltString(pAgpdCharacter, szQuickBelt, AGPMUISTATUS_MAX_QUICKBELT_STRING);
	m_pAgpmUIStatus->SetQBeltEncodedString(pAgpdCharacter, szQuickBelt, (INT32)_tcslen(szQuickBelt));
		
	if (!pDatabase->SetQueryText(GetQuery(_T("CHARDETAIL_INSERT"), pDatabase->GetVender()))
		|| !m_pAgsmCharacter->SetParamDetailInsertQuery5(pDatabase, pAgpdCharacter, szQuickBelt))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _InsertCharacter CHARDETAIL_INSERT [%s]\n", m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID));
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
		}

	pDatabase->EndQuery();
	return TRUE;
	}


BOOL AgsmLoginDB::_InsertItem(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter)
	{
	// equip item
	AgpdGridItem	*pAgpdGridItem;

	for (INT32 i=0; i<AGPMITEM_PART_NUM; ++i)
		{
		pAgpdGridItem = m_pAgpmItem->GetEquipItem(pAgpdCharacter, i);
		if (pAgpdGridItem)
			{
			AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
			if (!pDatabase->SetQueryText(GetQuery(_T("ITEM_INSERT"), pDatabase->GetVender()))
				|| !m_pAgsmItem->SetParamInsertQuery5(pDatabase, pAgpdItem)
				)
				{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _InsertItem EQUIP_ITEM_INSERT [%s]\n", pAgpdCharacter->m_szID);
				AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
				
				pDatabase->EndQuery();
				return FALSE;
				}
			pDatabase->EndQuery();
			}
		}

	// inventory item
	AgpdGrid *pInventoryGrid = m_pAgpmItem->GetInventory(pAgpdCharacter);
	if (pInventoryGrid)
		{
		INT32			lIndex	= 0;
		AgpdGridItem	*pAgpdGridItem = NULL;

		for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(pInventoryGrid, &lIndex);
			 pAgpdGridItem;
			 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(pInventoryGrid, &lIndex))
			{
			if (pAgpdGridItem)
				{
				AgpdItem	*pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
				if (!pDatabase->SetQueryText(GetQuery(_T("ITEM_INSERT"), pDatabase->GetVender()))
					|| !m_pAgsmItem->SetParamInsertQuery5(pDatabase, pAgpdItem)
					)
					{
					char strCharBuff[256] = { 0, };
					sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _InsertItem INVEN_ITEM_INSERT [%s]\n", pAgpdCharacter->m_szID);
					AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
				
					pDatabase->EndQuery();
					return FALSE;
					}
				pDatabase->EndQuery();
				}
			}
		}
	
	return TRUE;
	}


BOOL AgsmLoginDB::_InsertSkill(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter)
	{
	TCHAR	szSkillSeq[AGSMSKILL_MAX_SKILLTREE_LENGTH + 1];
	ZeroMemory(szSkillSeq, sizeof(szSkillSeq));
	m_pAgsmEventSkillMaster->EncodingDefaultSkillList(pAgpdCharacter->m_pcsCharacterTemplate, szSkillSeq, sizeof(szSkillSeq));

	if (!pDatabase->SetQueryText(GetQuery(_T("SKILL_INSERT"), pDatabase->GetVender()))
		|| !m_pAgsmSkill->SetParamInsertQuery5(pDatabase, pAgpdCharacter, szSkillSeq)
		)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _InsertSkill SKILL_INSERT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
		}

	pDatabase->EndQuery();
						
	AgsdSkillADBase	*pAgsdSkillADBase = m_pAgsmSkill->GetADBase((ApBase *) pAgpdCharacter);
	pAgsdSkillADBase->m_pstMasteryInfo = new stMasteryInfo;
	if (!pAgsdSkillADBase->m_pstMasteryInfo)
		return FALSE;

	ZeroMemory(pAgsdSkillADBase->m_pstMasteryInfo->szSkillTree, sizeof(pAgsdSkillADBase->m_pstMasteryInfo->szSkillTree));
	ZeroMemory(pAgsdSkillADBase->m_pstMasteryInfo->szSpecialize, sizeof(pAgsdSkillADBase->m_pstMasteryInfo->szSpecialize));
	ZeroMemory(pAgsdSkillADBase->m_pstMasteryInfo->szProductCompose, sizeof(pAgsdSkillADBase->m_pstMasteryInfo->szProductCompose));
	_tcsncpy(pAgsdSkillADBase->m_pstMasteryInfo->szSkillTree, szSkillSeq, AGSMSKILL_MAX_SKILLTREE_LENGTH);

	return TRUE;
	}


BOOL AgsmLoginDB::_InsertAccountWorld(AuDatabase2 *pDatabase, AgsdCharacter *pAgsdCharacter, TCHAR *pszServerName)
	{
	BOOL bResult = TRUE;
	if (!pDatabase->SetQueryText(GetQuery(_T("ACCOUNTWORLD_INSERT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID))
		|| !pDatabase->SetParam(1, pszServerName, sizeof(TCHAR) * ((INT32)_tcslen(pszServerName) + 1))
		|| AUDATABASE2_QR_SUCCESS != pDatabase->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _InsertAccountWorld ACCOUNTWORLD_INSERT [%s]\n", pAgsdCharacter->m_szAccountID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		bResult = FALSE;
		}
	
	pDatabase->EndQuery();
	return bResult;
	}


BOOL AgsmLoginDB::_WriteWorldMapping(AuDatabase2 *pDatabase, TCHAR *pszAccountID, TCHAR *pszServerName, TCHAR *pszCharID, BOOL bRemove)
	{
	TCHAR *pszQuery = NULL;
	if (bRemove)
		pszQuery = GetQuery(_T("WORLD_MAPPING_DELETE"), pDatabase->GetVender());
	else
		pszQuery = GetQuery(_T("WORLD_MAPPING_INSERT"), pDatabase->GetVender());
	
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;
	
	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		return FALSE;
	
	TCHAR szSection[11];
	ZeroMemory(szSection, sizeof(szSection));
	itoa(pAgsdServer->m_lServerID, szSection, 10);
	
	if (!pDatabase->SetQueryText(pszQuery)
		|| !pDatabase->SetParam(0, pszAccountID, sizeof(TCHAR) * ((INT32)_tcslen(pszAccountID)+1) )
		|| !pDatabase->SetParam(1, szSection, sizeof(szSection))
		|| !pDatabase->SetParam(2, pszServerName, sizeof(TCHAR) * ((INT32)_tcslen(pszServerName)+1) )
		|| !pDatabase->SetParam(3, pszCharID, sizeof(TCHAR) * ((INT32)_tcslen(pszCharID)+1) )
		|| AUDATABASE2_QR_SUCCESS != pDatabase->ExecuteQuery()
		)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _WriteWorldMapping [bRemove:%d] [Acc%s] [Server:%s] [Char:%s]\n",
											bRemove,
											pszAccountID,
											pszServerName,
											pszCharID);
			AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
			
		pDatabase->EndQuery();
		return FALSE;
		}

	pDatabase->EndQuery();	
	return TRUE;
	}




//	Select Helper
//=============================================
//
INT32 AgsmLoginDB::_SelectCharacterView(AuDatabase2 *pDatabase, TCHAR *pszChar, INT8 *pcSlot, UINT32 ulNID)
	{
	if (!pDatabase || !pszChar)
		return AP_INVALID_CID;

	AgpdCharacter	*pAgpdCharacter = NULL;
	BOOL			bResult = FALSE;
	INT32			lCID = AP_INVALID_CID;

	// read character
	if (!pDatabase->SetQueryText(GetQuery(_T("CHARACTER_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, pszChar, (INT32)(sizeof(TCHAR) * (_tcslen(pszChar)+1)))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCharacterView CHARACTER_SELECT [%s]\n", pszChar);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return AP_INVALID_CID;
	}

	if (0 == pDatabase->GetReadRows())
		{
		return AP_INVALID_CID;
		}

	// set master
	INT8 cSlot = 0;
	lCID = m_pAgsmCharacter->GetMasterSelectResult5(pDatabase, cSlot);
	if (0 == lCID)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCharacterView GetMasterSelectResult5 [%s]\n", pszChar);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return AP_INVALID_CID;
	}

	if (pcSlot)
		*pcSlot = cSlot;

	// set detail
	pAgpdCharacter = m_pAgpmCharacter->GetCharacter(lCID);
	AgsdCharacter *	pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result())
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCharacterView AuAutoLock [%s]\n", pszChar);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgpmCharacter->RemoveCharacter(lCID);
		pDatabase->EndQuery();
		return AP_INVALID_CID;
	}

	pcsAgsdCharacter->m_dpnidCharacter = ulNID;
	if (!pAgpdCharacter || !m_pAgsmCharacter->GetDetailSelectResult5(pAgpdCharacter, pDatabase))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCharacterView GetDetailSelectResult5 [%s]\n", pszChar);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgpmCharacter->RemoveCharacter(lCID);
		pDatabase->EndQuery();
		return AP_INVALID_CID;
		}

	if (IsExistExpeditionList(pcsAgsdCharacter->m_szServerName, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID)))
	{
		pAgpdCharacter->m_unEventStatusFlag |= AGPM_CHAR_BIT_FLAG_EXPEDITION;
	}

	// check if archlord or archlord guard
	//BOOL bArchlord = _IsArchlord(pDatabase, pAgpdCharacter->m_szID);
	//BOOL bArchlordGuard = _IsArchlordGuard(pDatabase, pAgpdCharacter->m_szID);
	//ApSafeArray<ApgdItem *pAgpdItem, 30> RemoveItems;
	//RemoveItems.MemSetAll();
	//INT32 lRemoveItem = 0;


	// get equip items
	if (!pDatabase->SetQueryText(GetQuery(_T("EQUIP_ITEM_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, pszChar, (INT32)(sizeof(TCHAR) * (_tcslen(pszChar)+1)))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		// if fail, just return character
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCharacterView EQUIP_ITEM_SELECT [%s]\n", pszChar);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return lCID;
		}
	
	// for all result
	do	{
		AgpdItem *pAgpdItem = m_pAgsmItemManager->GetSelectResult5(pAgpdCharacter, pDatabase);
		if (pAgpdItem)
			{
			AgsdItem *pAgsdItem	= m_pAgsmItem->GetADItem(pAgpdItem);
			pAgsdItem->m_bIsNeedInsertDB = FALSE;
			AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
			pAgpdItem->m_eStatus = AGPDITEM_STATUS_NOTSETTING;
			
			if (!m_pAgpmItem->EquipItem(pAgpdCharacter, pAgpdItemADChar, pAgpdItem, TRUE, TRUE))	// equip
				{
				if (!m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem))		// 1st fail, to inven
					m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);							// fail, remove
				}
			}
		} while(pDatabase->GetNextRow());

	if (pDatabase->GetReadRows() == 0 && pcsAgsdCharacter->m_bNeedReinitialize)
		{
		m_pAgsmCharManager->EnumCallbackCreateCharacter(pAgpdCharacter);
		_InsertItem(pDatabase, pAgpdCharacter);
		}

	if( _SelectGuild(pDatabase, pAgpdCharacter) == FALSE		// 길드 정보 - arycoat 2008.09.
		|| _SelectSkill(pDatabase, pAgpdCharacter) == FALSE		// 장착스킬 정보(진화스킬로딩) - arycoat 2008.10.
		)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : AgsmLoginDB::_SelectCharacterView::_SelectGuild [%s]\n", pszChar);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgpmCharacter->RemoveCharacter(lCID);
		pDatabase->EndQuery();
		return AP_INVALID_CID;
	}

	pDatabase->EndQuery();

	return lCID;
	}


BOOL AgsmLoginDB::_SelectCharacterViewExceptMaster(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter)
	{
	if (!pDatabase || !pAgpdCharacter)
		return FALSE;

	// read character
	if (!pDatabase->SetQueryText(GetQuery(_T("CHARACTER_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCharacterViewExceptMaster CHARACTER_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
		}

	if (0 == pDatabase->GetReadRows())
		{
		return FALSE;
		}

	// set detail
	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCharacterViewExceptMaster AuAutoLock [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgpmCharacter->RemoveCharacter(pAgpdCharacter->m_lID);
		pDatabase->EndQuery();
		return FALSE;
		}

	if (!m_pAgsmCharacter->GetDetailSelectResult5(pAgpdCharacter, pDatabase))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCharacterViewExceptMaster GetDetailSelectResult5 [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		m_pAgpmCharacter->RemoveCharacter(pAgpdCharacter->m_lID);
		pDatabase->EndQuery();
		return FALSE;
		}

	// remove equip items
	AgpdItemADChar *pAgpdItemADChar =  m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	
	for (INT32 i = 0; i < pAgpdItemADChar->m_csEquipGrid.m_lGridCount; ++i)
		{
		if (pAgpdItemADChar->m_csEquipGrid.m_ppcGridData[i] == NULL)
			continue;

		m_pAgpmItem->RemoveItem(pAgpdItemADChar->m_csEquipGrid.m_ppcGridData[i]->m_lItemID);
		}
	
	for (INT32 i = 0; i < pAgpdItemADChar->m_csInventoryGrid.m_lGridCount; ++i)
		{
		if (pAgpdItemADChar->m_csInventoryGrid.m_ppcGridData[i] == NULL)
			continue;

		m_pAgpmItem->RemoveItem(pAgpdItemADChar->m_csInventoryGrid.m_ppcGridData[i]->m_lItemID);
		}	


	// get equip items
	if (!pDatabase->SetQueryText(GetQuery(_T("EQUIP_ITEM_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		// if fail, just return character
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCharacterViewExceptMaster EQUIP_ITEM_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return TRUE;
		}
	
	AgsdCharacter	*pcsAgsdCharacter	= m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);

	// for all result
	do	{
		AgpdItem *pAgpdItem = m_pAgsmItemManager->GetSelectResult5(pAgpdCharacter, pDatabase);
		if (pAgpdItem)
			{
			AgsdItem *pAgsdItem	= m_pAgsmItem->GetADItem(pAgpdItem);
			pAgsdItem->m_bIsNeedInsertDB = FALSE;
			AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
			pAgpdItem->m_eStatus	= AGPDITEM_STATUS_NOTSETTING;
			if (!m_pAgpmItem->EquipItem(pAgpdCharacter, pAgpdItemADChar, pAgpdItem, TRUE, TRUE))	// equip
				{
				if (!m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem))		// 1st fail, to inven
					m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);							// fail, remove
				}
			}

		} while(pDatabase->GetNextRow());

	pDatabase->EndQuery();

	return TRUE;
	}


BOOL AgsmLoginDB::_SelectCharacterExceptView(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter, TCHAR *pszServerName)
	{
	if (!pDatabase || !pAgpdCharacter)
		return FALSE;

	if (!_SelectNonEquipItems(pDatabase, pAgpdCharacter)
		|| !_SelectCashItems(pDatabase, pAgpdCharacter)
		|| !_SelectAccountWorld(pDatabase, pAgpdCharacter, pszServerName)
		|| !_SelectBankItems(pDatabase, pAgpdCharacter, pszServerName)
		//|| !_SelectSkill(pDatabase, pAgpdCharacter)
		|| !_SelectQuest(pDatabase, pAgpdCharacter)
		|| !_SelectGuild(pDatabase, pAgpdCharacter)
		|| !_SelectTitle(pDatabase, pAgpdCharacter))
		{
		return FALSE;
		}
	
	return TRUE;
	}


BOOL AgsmLoginDB::_SelectNonEquipItems(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter)
	{
	// all items except equip
	if (!pDatabase->SetQueryText(GetQuery(_T("NONEQUIP_ITEM_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectNonEquipItems NONEQUIP_ITEM_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
		}

	if (0 == pDatabase->GetReadRows())
		{
		pDatabase->EndQuery();
		return TRUE;
		}
	
	AgsdCharacter	*pcsAgsdCharacter	= m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);

	AgpdItem *pAgpdItem = NULL;
	do	{
		pAgpdItem = m_pAgsmItemManager->GetSelectResult5(pAgpdCharacter, pDatabase);
		if (pAgpdItem)
			{
			AgsdItem *pAgsdItem	= m_pAgsmItem->GetADItem(pAgpdItem);
			pAgsdItem->m_bIsNeedInsertDB = FALSE;
			if (AGPDITEM_STATUS_BANK == pAgpdItem->m_eStatus)
				{
				pAgpdItem->m_eStatus	= AGPDITEM_STATUS_NOTSETTING;

				if (!m_pAgpmItem->AddItemToBank(pAgpdCharacter, pAgpdItem, pAgpdItem->m_anGridPos[0], pAgpdItem->m_anGridPos[1], pAgpdItem->m_anGridPos[2], FALSE))
					{
					m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);
					}
				}
			else if (AGPDITEM_STATUS_SUB_INVENTORY == pAgpdItem->m_eStatus)
				{
				pAgpdItem->m_eStatus	= AGPDITEM_STATUS_NOTSETTING;
				AgpdGrid *pAgpdGrid = m_pAgpmItem->GetSubInventory(pAgpdCharacter);
				if (pAgpdGrid)
					{
					if (!m_pAgpmItem->AddItemToSubInventory(pAgpdCharacter, pAgpdItem, pAgpdItem->m_anGridPos[0], pAgpdItem->m_anGridPos[1], pAgpdItem->m_anGridPos[2], FALSE))
						{
						if (!m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem, FALSE))
							m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);
						}
					}
				}
			else if (AGPDITEM_STATUS_SALESBOX_GRID == pAgpdItem->m_eStatus)
				{
				m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);
				}
			else if (AGPDITEM_STATUS_CASH_INVENTORY == pAgpdItem->m_eStatus)
			{
				ASSERT(0);	// 들어오면 아니됨, 문제 없으면 이 부분 지우기, 20051208, kelovon
			}
			else if (pAgpdItem->m_eStatus != 0)
				{
				pAgpdItem->m_eStatus	= AGPDITEM_STATUS_NOTSETTING;

				AgpdGrid *pAgpdGrid = m_pAgpmItem->GetInventory(pAgpdCharacter);
				if (pAgpdGrid)
					{
					if (!m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem, pAgpdItem->m_anGridPos[0], pAgpdItem->m_anGridPos[1], pAgpdItem->m_anGridPos[2], FALSE))
						{
						if (!m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem, FALSE))
							m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);
						}
					}
				}
			else
				m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);
			}
		} while(pDatabase->GetNextRow());

	pDatabase->EndQuery();

	return TRUE;
	}


BOOL AgsmLoginDB::_SelectCashItems(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter)
	{
	// all items except equip
	if (!pDatabase->SetQueryText(GetQuery(_T("CASH_ITEM_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectCashItems CASH_ITEM_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
		}

	if (0 == pDatabase->GetReadRows())
		{
		pDatabase->EndQuery();
		return TRUE;
		}

	AgsdCharacter	*pcsAgsdCharacter	= m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	
	AgpdItem *pAgpdItem = NULL;
	do	{
		pAgpdItem = m_pAgsmItemManager->GetSelectResult5(pAgpdCharacter, pDatabase);
		if (pAgpdItem)
			{
			AgsdItem *pAgsdItem	= m_pAgsmItem->GetADItem(pAgpdItem);
			pAgsdItem->m_bIsNeedInsertDB = FALSE;
			if (AGPDITEM_STATUS_CASH_INVENTORY == pAgpdItem->m_eStatus)
				{
				pAgpdItem->m_eStatus	= AGPDITEM_STATUS_NOTSETTING;
				
				if (!m_pAgpmItem->AddItemToCashInventory(pAgpdCharacter, pAgpdItem))
					{
					m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);
					}
				}
			else
				m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);
			}

		} while(pDatabase->GetNextRow());

	pDatabase->EndQuery();

	return TRUE;
	}


BOOL AgsmLoginDB::_SelectBankItems(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter, TCHAR *pszServerName)
	{
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	if (!pDatabase->SetQueryText(GetQuery(_T("BANK_ITEM_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID))
		|| !pDatabase->SetParam(1, pszServerName, sizeof(TCHAR) * ((INT32)_tcslen(pszServerName) + 1))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectBankItems BANK_ITEM_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
		}

	if (0 == pDatabase->GetReadRows())
		{
		pDatabase->EndQuery();
		return TRUE;
		}

	AgpdItem *pAgpdItem = NULL;
	do	{
		pAgpdItem = m_pAgsmItemManager->GetSelectBankResult5(pAgpdCharacter, pDatabase);
		if (pAgpdItem)
			{
			AgsdItem *pAgsdItem	= m_pAgsmItem->GetADItem(pAgpdItem);
			pAgsdItem->m_bIsNeedInsertDB = FALSE;
			if (AGPDITEM_STATUS_BANK == pAgpdItem->m_eStatus)
				{
				pAgpdItem->m_eStatus	= AGPDITEM_STATUS_NOTSETTING;

				if (!m_pAgpmItem->AddItemToBank(pAgpdCharacter, pAgpdItem, pAgpdItem->m_anGridPos[0], pAgpdItem->m_anGridPos[1], pAgpdItem->m_anGridPos[2], FALSE))
					{
					//if (!m_pAgpmItem->AddItemToBank(pAgpdCharacter, pAgpdItem))
						m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);
					}
				}
			else
				{
				m_pAgpmItem->RemoveItem(pAgpdItem->m_lID);
				}
			}
		} while (pDatabase->GetNextRow());

	pDatabase->EndQuery();

	return TRUE;
	}


BOOL AgsmLoginDB::_SelectAccountWorld(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter, TCHAR *pszServerName)
	{
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return FALSE;
	
	if (!pDatabase->SetQueryText(GetQuery(_T("ACCOUNTWORLD_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID))
		|| !pDatabase->SetParam(1, pszServerName, sizeof(TCHAR) * ((INT32)_tcslen(pszServerName) + 1))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectAccountWorld ACCOUNTWORLD_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
		}

	if (0 == pDatabase->GetReadRows())
		{
		pDatabase->EndQuery();
		return _InsertAccountWorld(pDatabase, pAgsdCharacter, pszServerName);
		}
	else
		{
		m_pAgsmCharacter->GetAccountWorldSelectResult5(pAgpdCharacter, pDatabase);
		pDatabase->EndQuery();
		}

	return TRUE;
	}


BOOL AgsmLoginDB::_SelectSkill(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter)
{
	if (!pDatabase->SetQueryText(GetQuery(_T("SKILL_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectSkill SKILL_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
	}
	
	if (0 == pDatabase->GetReadRows())
	{
		pDatabase->EndQuery();
		return _InsertSkill(pDatabase, pAgpdCharacter);
	}
	else
	{
		m_pAgsmSkillManager->GetSelectResult5(pDatabase, pAgpdCharacter);	
	}

	//////////////////////////////////////////////////////////////////////////
	// Get Equip Skills - arycoat 2008.7
	if (!pDatabase->SetQueryText(GetQuery(_T("EQUIP_SKILL_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectSkill EQUIP_SKILL_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
	}

	if (pDatabase->GetReadRows() > 0)
	{
		AgsdSkillADBase	*pcsSkillADBase	= m_pAgsmSkill->GetADBase((ApBase *)pAgpdCharacter);
		if(!pcsSkillADBase)
		{
			pDatabase->EndQuery();
			return FALSE;
		}

		pcsSkillADBase->m_csEquipSkillArray.clear();

		do {
			AgsdEquipSkill pEquipSkill;
			{
				pDatabase->GetQueryINT32( 0, &pEquipSkill.SkillTID);
				pDatabase->GetQueryUINT32( 1, &pEquipSkill.RemainTime);

				CHAR* szExp = pDatabase->GetQueryResult( 2 );
				pEquipSkill.ExpireDate = AuTimeStamp::ConvertOracleTimeToCTime(szExp);

				DWORD nAttribute;
				pDatabase->GetQueryDWORD( 3, &nAttribute);
				pEquipSkill.Attribute = (UINT64)nAttribute;
			}

			pcsSkillADBase->m_csEquipSkillArray.push_back(pEquipSkill);
		} while(pDatabase->GetNextRow());
	}
	
	pDatabase->EndQuery();

	return TRUE;
}


BOOL AgsmLoginDB::_SelectQuest(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter)
	{
	// current quest
	if (!pDatabase->SetQueryText(GetQuery(_T("QUEST_CURRENT_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectQuest QUEST_CURRENT_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
		}

	if (0 != pDatabase->GetReadRows())
		{
		do	{
			m_pAgsmQuest->GetSelectResultCurrentQuest5(pDatabase, pAgpdCharacter);
			} while (pDatabase->GetNextRow());
		}
		
	pDatabase->EndQuery();

	// quest flag
	if (!pDatabase->SetQueryText(GetQuery(_T("QUEST_FLAG_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectQuest QUEST_FLAG_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;
		}

	if (0 != pDatabase->GetReadRows())
		{
		m_pAgsmQuest->GetSelectResultFlag5(pDatabase, pAgpdCharacter);
		}
	
	pDatabase->EndQuery();
	return TRUE;
	}


BOOL AgsmLoginDB::_SelectGuild(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter)
	{
	if (!pDatabase->SetQueryText(GetQuery(_T("GUILD_MEMBER_SELECT"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())	
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectGuild GUILD_MEMBER_SELECT [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;	
		}
	
	
	if (0 != pDatabase->GetReadRows())
		{
		do	{
			m_pAgsmGuild->GetSelectResultCharGuildID5(pDatabase, pAgpdCharacter);
			} while (pDatabase->GetNextRow());
		}
		
	pDatabase->EndQuery();	
	
	return TRUE;
	}


BOOL AgsmLoginDB::_SelectTitle(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter)
{
	if(!m_pAgsmTitle)
		return FALSE;

	BOOL bTitle = TRUE;

	if(!pAgpdCharacter->m_csTitle)
		pAgpdCharacter->m_csTitle = new AgpdTitle();

	if(!pAgpdCharacter->m_csTitleQuest)
		pAgpdCharacter->m_csTitleQuest = new AgpdTitleQuest();

	if(!pDatabase->SetQueryText(GetQuery(_T("SELECT_TITLE"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| !pDatabase->SetParam(1, &bTitle)
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectTitle SELECT_TITLE [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;	
	}

	if(pDatabase->GetReadRows() != 0)
	{
		do
		{
			m_pAgsmTitle->GetSelectTitleResult(pDatabase, pAgpdCharacter);
		}while(pDatabase->GetNextRow());
	}
	pAgpdCharacter->m_csTitle->m_bLoadedTitle = TRUE;

	pDatabase->EndQuery();

	if(!pDatabase->SetQueryText(GetQuery(_T("SELECT_TITLE_QUEST"), pDatabase->GetVender()))
		|| !pDatabase->SetParam(0, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _SelectTitle SELECT_TITLE [%s]\n", pAgpdCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		return FALSE;	
	}

	if(pDatabase->GetReadRows() != 0)
	{
		do
		{
			m_pAgsmTitle->GetSelectTitleQuestResult(pDatabase, pAgpdCharacter);
		}while(pDatabase->GetNextRow());
	}
	pAgpdCharacter->m_csTitleQuest->m_bLoadedTitleQuest = TRUE;

	pDatabase->EndQuery();
	
	return TRUE;
}


//	Status Helper
//===========================================
//
BOOL AgsmLoginDB::_CheckLoginStatus(AuDatabase2 *pDatabase, UINT32 ulClientNID, TCHAR *pszAccountID)
	{
	if (!pDatabase || 0 == ulClientNID || !pszAccountID)
		return FALSE;

	AgsmLoginEncryptInfo *pLoginEncryptInfo = m_pAgsmLoginClient->GetCertificatedNID(ulClientNID);
	if (NULL == pLoginEncryptInfo)
		return FALSE;

	TCHAR *pszQuery = GetQuery(_T("LOGIN_STATUS_SELECT"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;
	
	pDatabase->StartTranaction();

	if (!pDatabase->SetQueryText(pszQuery) ||
		!pDatabase->SetParam(0, pLoginEncryptInfo->m_szAccountID, sizeof(pLoginEncryptInfo->m_szAccountID)) ||
		AUDATABASE2_QR_SUCCESS != pDatabase->ExecuteQuery())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _CheckLoginStatus LOGIN_STATUS_SELECT [%s]\n", pszAccountID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		pDatabase->AbortTransaction();
		return FALSE;
		}

	if (_atoi64(pDatabase->GetQueryResult(0)) != pLoginEncryptInfo->m_ullKey)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : _CheckLoginStatus pLoginEncryptInfo [%s]\n", pszAccountID);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		pDatabase->EndQuery();
		pDatabase->AbortTransaction();
		return FALSE;
		}

	TCHAR *psz = NULL;
	TCHAR szServerIP[40];
	TCHAR szCharID[AGPACHARACTER_MAX_ID_STRING + 1];
	
	// server
	psz = pDatabase->GetQueryResult(1);
	_tcscpy(szServerIP, psz ? psz : _T(""));
	
	// charid
	psz = pDatabase->GetQueryResult(2);
	_tcscpy(szCharID, psz ? psz : _T(""));
	
	if (_T('\0') != szServerIP[0] && _T('\0') != szCharID[0])
		{
		AgsdServer *pAgsdServer = m_pAgsmServerManager->GetServer(szServerIP);
		if (NULL != pAgsdServer)
			m_pAgsmLoginServer->SendRemoveDuplicateAccount(pszAccountID, 0, pAgsdServer->m_dpnidServer);
		}
	

	pDatabase->EndQuery();
	pDatabase->CommitTransaction();
	return TRUE;
	}

//JK_케릭삭제시패스워드확인
BOOL AgsmLoginDB::_CheckAccountPassword(AuDatabase2 *pDatabase, UINT32 ulClientNID, TCHAR *pszAccountID, TCHAR *pszPassword)
{
	if (!pDatabase)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "ERROR!! AgsmLoginDB::_CheckAccountPassword\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

		return FALSE;
	}
	//sp 호출

	TCHAR *pszQuery = GetQuery(_T("CHECKACCOUNTPASSWORD"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;

	AuAutoReleaseDS Auto(pDatabase);

	INT32 Result		= 0;

	if (!pDatabase->SetQueryText(pszQuery) ||
		!pDatabase->SetParam(0, pszAccountID, sizeof(pszAccountID)) ||
		!pDatabase->SetParam(1, pszPassword, sizeof(pszPassword) ) ||
		!pDatabase->SetParam(2, &Result, TRUE)
		)
	{
		pDatabase->EndQuery();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "_CheckAccountPassword - SetParm Error ( AccountID : %s)\n" ,pszAccountID);
		//AuLogFile_s(LOGIN_AUTH_FILE_NAME, strCharBuff);

		return FALSE;
	}

	if (AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
	{
		pDatabase->EndQuery();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "_CheckAccountPassword - ExecuteQuery Error\n");
		//AuLogFile_s(LOGIN_AUTH_FILE_NAME, strCharBuff);

		return FALSE;
	}

	pDatabase->EndQuery();

	if(Result != 0 ) return FALSE;

	return TRUE;
}

BOOL AgsmLoginDB::_WriteLoginStatus(AuDatabase2 *pDatabase, BOOL bUpdate, TCHAR *pszAccountID, UINT64 ullKey,
									TCHAR *pszServer, TCHAR *pszChar, INT32 lCID, BOOL bTran)
	{
	TCHAR	szAccountID[AGPACHARACTER_MAX_ID_STRING + 1];
	TCHAR	szChar[AGPACHARACTER_MAX_ID_STRING + 1];
	TCHAR	szKey[21];
	INT32	lCID2;
	TCHAR	szServer[41];

	TCHAR	*pszQuery = NULL;
	if (bUpdate)
		pszQuery = GetQuery(_T("LOGIN_STATUS_UPDATE"), pDatabase->GetVender());
	else
		pszQuery = GetQuery(_T("LOGIN_STATUS_INSERT"), pDatabase->GetVender());
	
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;
	
	if (bTran)
		pDatabase->StartTranaction();

	if (!pDatabase->SetQueryText(pszQuery))
		{
		pDatabase->EndQuery();
		if (bTran)
			pDatabase->AbortTransaction();
		return FALSE;
		}
	
	ZeroMemory(szAccountID, sizeof(szAccountID));
	_tcsncpy(szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);

	_i64toa(ullKey, szKey, 10);
	
	ZeroMemory(szServer, sizeof(szServer));
	_tcsncpy(szServer, pszServer ? pszServer : _T(""), 40);	

	ZeroMemory(szChar, sizeof(szChar));
	_tcsncpy(szChar, pszChar ? pszChar : _T(""), AGPACHARACTER_MAX_ID_STRING);

	lCID2 = lCID;
	
	INT16 i = 0;
	pDatabase->SetParam(i++, szKey, sizeof(szKey));
	pDatabase->SetParam(i++, szServer, sizeof(szServer));
	pDatabase->SetParam(i++, szChar, sizeof(szChar));
	pDatabase->SetParam(i++, &lCID2);
	pDatabase->SetParam(i++, szAccountID, sizeof(szAccountID));

	if (AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		pDatabase->EndQuery();
		if (bTran)
			pDatabase->AbortTransaction();
		return FALSE;
		}
	
	pDatabase->EndQuery();
	if (bTran)
		pDatabase->CommitTransaction();
	return TRUE;
	}


BOOL AgsmLoginDB::_WriteLoginStatus(AuDatabase2 *pDatabase, BOOL bUpdate, TCHAR *pszAccountID, UINT64 ullKey, BOOL bTran)
	{
	TCHAR	szAccountID[AGPACHARACTER_MAX_ID_STRING + 1];
	TCHAR	szKey[21];

	TCHAR	*pszQuery = NULL;
	if (bUpdate)
		pszQuery = GetQuery(_T("LOGIN_STATUS_UPDATE2"), pDatabase->GetVender());
	else
		pszQuery = GetQuery(_T("LOGIN_STATUS_INSERT2"), pDatabase->GetVender());
	
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;	
	
	if (bTran)
		pDatabase->StartTranaction();

	if (!pDatabase->SetQueryText(pszQuery))
		{
		pDatabase->EndQuery();
		if (bTran)
			pDatabase->AbortTransaction();
		return FALSE;
		}
	
	ZeroMemory(szAccountID, sizeof(szAccountID));
	_tcsncpy(szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);

	_i64toa(ullKey, szKey, 10);
	

	INT16 i = 0;
	pDatabase->SetParam(i++, szKey, sizeof(szKey));
	pDatabase->SetParam(i++, szAccountID, sizeof(szAccountID));

	if (AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
		pDatabase->EndQuery();
		if (bTran)
			pDatabase->AbortTransaction();
		return FALSE;
		}
	
	pDatabase->EndQuery();
	if (bTran)
		pDatabase->CommitTransaction();
	return TRUE;
	}


BOOL AgsmLoginDB::_CheckSocialNumber(TCHAR *pszSocialNo, INT32 lAge)
	{
	TCHAR	szBirthYear[5];
	ZeroMemory(szBirthYear, sizeof(szBirthYear));

	_tcsncpy(szBirthYear, pszSocialNo, 2);

	INT32	lBirthYear	= _ttoi(szBirthYear);

	ZeroMemory(szBirthYear, sizeof(szBirthYear));

	_tcsncpy(szBirthYear, pszSocialNo + 2, 4);

	INT32	lBirthMonthDay	= atoi(szBirthYear);

	SYSTEMTIME	stCurrent;
	::GetLocalTime(&stCurrent);

	INT32	lDiffYear	= (stCurrent.wYear % 100) - lBirthYear;
	if (lDiffYear < 0)
		lDiffYear += 100;

	if (stCurrent.wMonth * 100 + stCurrent.wDay - lBirthMonthDay < 0)
		--lDiffYear;

	if (lDiffYear < lAge)
		return FALSE;
		
	return TRUE;
	}


BOOL AgsmLoginDB::_IsArchlord(AuDatabase2 *pDatabase, TCHAR *pszChar)
	{
	BOOL bResult = FALSE;
	TCHAR szCastle[32];
		
	if (!pDatabase || !pszChar || _T('\0') == *pszChar)
		goto isa_fail;

	/* SELECT G.GMCHARID FROM CASTLE C, GUILDMASTER G WHERE
	G.GUILDID = C.OWNERGUILDID AND CASTLEID = :1 */
	TCHAR	*pszQuery = GetQuery(_T("ARCHLORD_CHECK"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		goto isa_fail;
	
	if (!pDatabase->SetQueryText(pszQuery)
		|| !pDatabase->SetParam(0, szCastle, sizeof(szCastle))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery()
		)
		goto isa_fail;

	if (0 >= pDatabase->GetReadRows())
		goto isa_fail;

	TCHAR *pszArchlord = pDatabase->GetQueryResult(0);
	if (!pszArchlord || _T('\0') == *pszArchlord)
		goto isa_fail;

	if (0 == _tcscmp(pszArchlord, pszChar))
		bResult = TRUE;

  isa_fail:
	pDatabase->EndQuery();
	return bResult;
	}


BOOL AgsmLoginDB::_IsArchlordGuard(AuDatabase2 *pDatabase, CHAR *pszChar)
	{
	BOOL bResult = FALSE;
	if (!pDatabase || !pszChar || _T('\0') == *pszChar)
		goto isg_fail;
	
	/* SELECT CHARID FROM LORDGUARD WHERE CHARID = :1 */
	TCHAR	*pszQuery = GetQuery(_T("ARCHLORD_GUARD_CHECK"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		goto isg_fail;
	
	if (!pDatabase->SetQueryText(pszQuery)
		|| !pDatabase->SetParam(0, pszChar, (_tcslen(pszChar) + 1) * sizeof(TCHAR))
		|| AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery()
		)
		goto isg_fail;

	if (0 >= pDatabase->GetReadRows())
		goto isg_fail;

	bResult = TRUE;
	
  isg_fail:
	pDatabase->EndQuery();
	return bResult;
	}




// Login Key
//==========================================
//
void AgsmLoginDB::SetKeyRange(INT32 ServerIndex)
{
	m_csGenerateID64.Initialize(ServerIndex);
}	


UINT64 AgsmLoginDB::_GetKey()
{
	return (UINT64)m_csGenerateID64.GetID();
}




//	Expedition
//=============================================
//
BOOL AgsmLoginDB::LoadExpeditionList()
	{
	TCHAR *pszQuery = _T("SELECT * from EXPEDITIONLIST");

	// find db
	AuDatabase2		*pDatabase = GetIdleMasterDatabase();
	if (!pDatabase)
		return FALSE;
	
	AuAutoReleaseDS Auto(pDatabase);

	// query
	pDatabase->StartTranaction();

	if (!pDatabase->SetQueryText(pszQuery) ||
		AUDATABASE2_QR_SUCCESS != pDatabase->ExecuteQuery())
		{
		pDatabase->EndQuery();
		pDatabase->AbortTransaction();
		return FALSE;
		}

	if (0 != pDatabase->GetReadRows())
		{
		m_lNumExpeditionList	= pDatabase->GetReadRows();

		m_pAgsdExpeditionList	= new AgsdLoginExpeditionList[m_lNumExpeditionList];
		if (!m_pAgsdExpeditionList)
			{
			pDatabase->EndQuery();
			pDatabase->AbortTransaction();
			return FALSE;
			}

		INT32	lIndex	= 0;

		do	{
			TCHAR	*pszWorldName	= pDatabase->GetQueryResult(1);
			TCHAR	*pszCharName	= pDatabase->GetQueryResult(2);

			if (!pszWorldName || !pszWorldName[0] || !pszCharName || !pszCharName[0])
				continue;

			_tcsncpy(m_pAgsdExpeditionList[lIndex].m_szWorldName, pszWorldName, AGPDWORLD_MAX_WORLD_NAME);
			_tcsncpy(m_pAgsdExpeditionList[lIndex].m_szCharName, pszCharName, AGPDCHARACTER_MAX_ID_LENGTH);

			lIndex++;
		
			} while (pDatabase->GetNextRow());
		}
	else
		{
		pDatabase->EndQuery();
		pDatabase->AbortTransaction();
		return FALSE;
		}

	// success
	pDatabase->EndQuery();
	pDatabase->CommitTransaction();

	return TRUE;
	}


BOOL	AgsmLoginDB::IsExistExpeditionList(TCHAR *pszWorldName, TCHAR *pszCharName)
	{
	if (!pszWorldName || !pszWorldName[0] || !pszCharName || !pszCharName[0])
		return FALSE;

	if (!m_pAgsdExpeditionList ||
		m_lNumExpeditionList < 1)
		return FALSE;

	for (int i = 0; i < m_lNumExpeditionList; ++i)
		{
		if (_tcscmp(m_pAgsdExpeditionList[i].m_szWorldName, pszWorldName) == 0 &&
			_tcscmp(m_pAgsdExpeditionList[i].m_szCharName, pszCharName) == 0)
			return TRUE;
		}

	return FALSE;
	}




//	Compensation
//==========================================================
//
BOOL AgsmLoginDB::CheckCompensation(AgsmLoginQueueInfo *pQueryInfo, TCHAR *pszChar)
	{
	if (!pQueryInfo || !pszChar)
		return FALSE;

	AgpdLoginCompenMaster *pCompenMaster = NULL;
	INT32 lCount = 0;
	GetCompenMaster(&pCompenMaster, lCount);
	
	for (INT32 i = 0; i < lCount; i++)
		{
		AgpdLoginCompenDetail CompenDetail;
		CompenDetail.m_lCompenID = pCompenMaster[i].m_lCompenID;
		_tcscpy(CompenDetail.m_szAccount, pQueryInfo->m_szAccountID);

		_tcscpy(CompenDetail.m_szWorld, pQueryInfo->m_szServerName);
		_tcscpy(CompenDetail.m_szChar, pszChar ? pszChar : _T(""));
		
		if (IsNotCompensated(NULL, pCompenMaster[i].m_eType, pCompenMaster[i].m_szDetailTable, &CompenDetail))
			{
			AgsmLoginEncryptInfo *pLoginEncryptInfo = m_pAgsmLoginClient->GetCertificatedNID(pQueryInfo->m_ulCharNID);
			if (pLoginEncryptInfo)
				{
				pLoginEncryptInfo->m_lCompenID = pCompenMaster[i].m_lCompenID;
				m_pAgsmLoginClient->SendCompensationExist(pQueryInfo->m_szAccountID,
														  pCompenMaster[i].m_lCompenID,
														  pCompenMaster[i].m_eType,
														  pszChar,
														  (_tcslen(CompenDetail.m_szItemDetails) > 0) ? CompenDetail.m_szItemDetails : pCompenMaster->m_szItemDetails,
														  pCompenMaster[i].m_szDescription,
														  pQueryInfo->m_ulCharNID
														  );
				delete [] pCompenMaster;
				return TRUE;
				}
			}
		}

	delete [] pCompenMaster;
	return FALSE;
	}


BOOL AgsmLoginDB::GetCompenMaster(AgpdLoginCompenMaster **ppCompenMaster, INT32 &lCount, INT32 lCompenID)
	{
	if (!ppCompenMaster)
		return FALSE;

	lCount = 0;
	
	// get databases
	AuDatabase2 *pCompenDatabase = GetIdleCompenDatabase();
	if (NULL == pCompenDatabase)
		return FALSE;
	AuAutoReleaseDS Auto1(pCompenDatabase);
	
	TCHAR *pszQuery = NULL;
	if (0 == lCompenID)
		pszQuery = GetQuery(_T("COMPEN_MASTER_SELECT"), pCompenDatabase->GetVender());	
	else
		pszQuery = GetQuery(_T("COMPEN_MASTER_SELECT_ID"), pCompenDatabase->GetVender());
	
	if (!pszQuery)
		return FALSE;
	
	if (!pCompenDatabase->SetQueryText(pszQuery))
		{
		pCompenDatabase->EndQuery();
		return FALSE;
		}
	if (0 != lCompenID &&
		!pCompenDatabase->SetParam(0, &lCompenID))
		{
		pCompenDatabase->EndQuery();
		return FALSE;
		}
	if (AUDATABASE2_QR_FAIL == pCompenDatabase->ExecuteQuery())
		{
		pCompenDatabase->EndQuery();
		return FALSE;
		}

	INT32 lTotal = pCompenDatabase->GetReadRows();
	INT32 lRead = 0;
	if (lTotal > 0)
		{
		*ppCompenMaster = new AgpdLoginCompenMaster[lTotal];
		if (NULL == (*ppCompenMaster))
			{
			pCompenDatabase->EndQuery();
			return FALSE;
			}
		
		do
			{
			TCHAR *pszID = pCompenDatabase->GetQueryResult(0);
			TCHAR *pszItemDetails = pCompenDatabase->GetQueryResult(1);
			TCHAR *pszType = pCompenDatabase->GetQueryResult(2);
			TCHAR *pszDetailTable = pCompenDatabase->GetQueryResult(3);
			TCHAR *pszDescription = pCompenDatabase->GetQueryResult(4);
			
			if (!pszID || 0 >= _tcslen(pszID)
				|| !pszItemDetails || 0 >= _tcslen(pszItemDetails)
				|| !pszType || 0 >= _tcslen(pszType)
				|| !pszDetailTable || 0 >= _tcslen(pszDetailTable))
				continue;
			
			(*ppCompenMaster)[lRead].m_lCompenID = _ttoi(pszID);
			_tcsncpy((*ppCompenMaster)[lRead].m_szItemDetails, pszItemDetails, 100);
			_stprintf((*ppCompenMaster)[lRead].m_szDetailTable, _T("COMPENDETAIL_%s"), pszDetailTable);
			_tcsncpy((*ppCompenMaster)[lRead].m_szDescription, pszDescription ? pszDescription : _T(""), 200);
			if (0 == _tcsicmp(pszType, _T("ALL")))
				(*ppCompenMaster)[lRead].m_eType = AGPMLOGINDB_COMPEN_TYPE_ALL;
			else if (0 == _tcsicmp(pszType, _T("ACC")))
				(*ppCompenMaster)[lRead].m_eType = AGPMLOGINDB_COMPEN_TYPE_ACCOUNT;
			else if (0 == _tcsicmp(pszType, _T("CHAR")))
				(*ppCompenMaster)[lRead].m_eType = AGPMLOGINDB_COMPEN_TYPE_CHAR;
			else if (0 == _tcsicmp(pszType, _T("ALLC")))
				(*ppCompenMaster)[lRead].m_eType = AGPMLOGINDB_COMPEN_TYPE_ALLC;
			else
				{
				(*ppCompenMaster)[lRead].Reset();
				continue;
				}
			
			lRead++;
			} while (pCompenDatabase->GetNextRow());
		}
	
	lCount = lRead;
	pCompenDatabase->EndQuery();
	return TRUE;
	}


BOOL AgsmLoginDB::IsNotCompensated(AuDatabase2 *pDatabase, eAGPMLOGINDB_COMPEN_TYPE eType, TCHAR *pszDetailTable, AgpdLoginCompenDetail *pCompenDetail)
	{
	TCHAR szQuery[1000];
	ZeroMemory(szQuery, sizeof(szQuery));

	// get databases
	AuDatabase2 *pCompenDatabase = NULL;
	
	if (NULL == pDatabase)
		{
		pCompenDatabase = GetIdleCompenDatabase();
		}
	else
		pCompenDatabase = pDatabase;

	if (NULL == pCompenDatabase)
		return FALSE;

	TCHAR *pszFormat = NULL;
	switch (eType)
		{
		case AGPMLOGINDB_COMPEN_TYPE_CHAR :
			pszFormat = GetQuery(_T("COMPEN_DETAIL_SELECT_CHAR"), pCompenDatabase->GetVender());
			break;
	
		case AGPMLOGINDB_COMPEN_TYPE_ACCOUNT :
			pszFormat = GetQuery(_T("COMPEN_DETAIL_SELECT_ACC"), pCompenDatabase->GetVender());
			break;

		case AGPMLOGINDB_COMPEN_TYPE_ALL :
			pszFormat = GetQuery(_T("COMPEN_DETAIL_SELECT_ALL"), pCompenDatabase->GetVender());
			break;

		case AGPMLOGINDB_COMPEN_TYPE_ALLC:
			pszFormat = GetQuery(_T("COMPEN_DETAIL_SELECT_ALLC"), pCompenDatabase->GetVender());
			break;

		default :
			break;
		}

	if (NULL == pszFormat)
		return FALSE;

	_stprintf(szQuery, pszFormat, pszDetailTable);

	if (!pCompenDatabase->SetQueryText(szQuery))
		{
		pCompenDatabase->EndQuery();
		if (NULL == pDatabase)
			pCompenDatabase->ReleaseSemaphore();
		return FALSE;
		}
	if (!pCompenDatabase->SetParam(0, &pCompenDetail->m_lCompenID)
		|| !pCompenDatabase->SetParam(1, pCompenDetail->m_szAccount, sizeof(pCompenDetail->m_szAccount)))
		{
		pCompenDatabase->EndQuery();
		if (NULL == pDatabase)
			pCompenDatabase->ReleaseSemaphore();
		return FALSE;
		}	
	if (AGPMLOGINDB_COMPEN_TYPE_CHAR == eType || AGPMLOGINDB_COMPEN_TYPE_ALLC == eType)
	{
		if (!pCompenDatabase->SetParam(2, pCompenDetail->m_szWorld, sizeof(pCompenDetail->m_szWorld)))
		{
			pCompenDatabase->EndQuery();
			if (NULL == pDatabase)
				pCompenDatabase->ReleaseSemaphore();
			return FALSE;
		}
	}
	if (AGPMLOGINDB_COMPEN_TYPE_CHAR == eType || AGPMLOGINDB_COMPEN_TYPE_ALLC == eType)
	{
		if (!pCompenDatabase->SetParam(3, pCompenDetail->m_szChar, sizeof(pCompenDetail->m_szChar)))
		{
			pCompenDatabase->EndQuery();
			if (NULL == pDatabase)
				pCompenDatabase->ReleaseSemaphore();
			return FALSE;
			}	
	}
	if (AUDATABASE2_QR_FAIL == pCompenDatabase->ExecuteQuery())
		{
		pCompenDatabase->EndQuery();
		if (NULL == pDatabase)
			pCompenDatabase->ReleaseSemaphore();
		return FALSE;
		}

	BOOL bExist = FALSE;
	if (pCompenDatabase->GetReadRows() > 0)
		{
		bExist = TRUE;
		if (AGPMLOGINDB_COMPEN_TYPE_ALL != eType)
			{
			TCHAR *pszCompenSeq = pCompenDatabase->GetQueryResult(0);
			if (!pszCompenSeq || 0 >= _tcslen(pszCompenSeq))
				{
				pCompenDatabase->EndQuery();
				if (NULL == pDatabase)
					pCompenDatabase->ReleaseSemaphore();
				return FALSE;
				}
			
			pCompenDetail->m_llCompenSeq = _ttoi64(pszCompenSeq);
			_tcscpy(pCompenDetail->m_szItemDetails, pCompenDatabase->GetQueryResult(1));
			}
		}

	pCompenDatabase->EndQuery();
	if (NULL == pDatabase)
		pCompenDatabase->ReleaseSemaphore();

	if (AGPMLOGINDB_COMPEN_TYPE_ALL == eType || AGPMLOGINDB_COMPEN_TYPE_ALLC == eType)
		return !bExist;
	else
		return bExist;
	}


BOOL AgsmLoginDB::PayCompensation(TCHAR *pszDBName, AgpdCharacter *pAgpdCharacter, TCHAR *pszWorld, AgpdLoginCompenMaster *pAgpdLoginCompenMaster)
	{
	ApSafeArray<AgpdItem *, 20> PaidItems;
	PaidItems.MemSetAll();
	INT32 lNumPaidItems = 0;

	if (!pszDBName || !pAgpdCharacter || !pszWorld)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	// get databases
	AuDatabase2 *pCompenDatabase = GetIdleCompenDatabase();
	if (NULL == pCompenDatabase)
		return FALSE;
	AuAutoReleaseDS Auto1(pCompenDatabase);
	
	AuDatabase2 *pGameDatabase = GetIdleGameDatabase(pszDBName);
	if (NULL == pGameDatabase)
		return FALSE;
	AuAutoReleaseDS Auto2(pGameDatabase);

	// query for compendetail table
	TCHAR *pszDetailModify = NULL;
	CHAR szDetailModify[500];
	ZeroMemory(szDetailModify, sizeof(szDetailModify));
	if (AGPMLOGINDB_COMPEN_TYPE_ALL == pAgpdLoginCompenMaster->m_eType || AGPMLOGINDB_COMPEN_TYPE_ALLC == pAgpdLoginCompenMaster->m_eType)
		{
		pszDetailModify = GetQuery(_T("COMPEN_DETAIL_INSERT"), pCompenDatabase->GetVender());
		}
	else
		{
		pszDetailModify = GetQuery(_T("COMPEN_DETAIL_UPDATE"), pCompenDatabase->GetVender());
		}
	if (NULL == pszDetailModify)
		return FALSE;
	_stprintf(szDetailModify, pszDetailModify, pAgpdLoginCompenMaster->m_szDetailTable);
	
	// query 4 cashitembuylist table
	TCHAR *pszCashItemBuyListInsert = GetQuery(_T("CASHITEM_LIST_INSERT"), pGameDatabase->GetVender());
	if (NULL == pszCashItemBuyListInsert)
		return FALSE;

	// compen detail
	AgpdLoginCompenDetail CompenDetail;
	CompenDetail.m_lCompenID = pAgpdLoginCompenMaster->m_lCompenID;
	_tcscpy(CompenDetail.m_szAccount, pAgsdCharacter->m_szAccountID);
	_tcscpy(CompenDetail.m_szWorld, pszWorld);
	_tcscpy(CompenDetail.m_szChar, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID));
	if (!IsNotCompensated(pCompenDatabase, pAgpdLoginCompenMaster->m_eType, pAgpdLoginCompenMaster->m_szDetailTable, &CompenDetail))
		return FALSE;
	
	// parse item
	TCHAR *pszItemDetails = NULL;
	if (_tcslen(CompenDetail.m_szItemDetails) > 0)
		pszItemDetails = CompenDetail.m_szItemDetails;
	else
		pszItemDetails = pAgpdLoginCompenMaster->m_szItemDetails;

	AgpdLoginCompenItemList ItemList;
	if (!ItemList.Parse(pszItemDetails))
		return FALSE;
	
	// start transaction
	pCompenDatabase->StartTranaction();
	pGameDatabase->StartTranaction();

	// pay
	for (INT32 j=0; j<ItemList.m_lCount; ++j)
		{
		if (0 == ItemList.m_Items[j].m_lItemTID || 0 == ItemList.m_Items[j].m_lItemQty)
			continue;
		
		INT32 lItemTID = ItemList.m_Items[j].m_lItemTID;
		INT32 lItemQty = ItemList.m_Items[j].m_lItemQty;
		AgpdItem *pAgpdItem = m_pAgsmItemManager->CreateItem(lItemTID, pAgpdCharacter, lItemQty);
		if (!pAgpdItem)
			goto compen_fail;

		PaidItems[lNumPaidItems++] = pAgpdItem;

		if (!m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem, FALSE))
			{
			pAgpdItem->m_eStatus = AGPDITEM_STATUS_INVENTORY;
			pAgpdItem->m_anGridPos[0] = 0;
			pAgpdItem->m_anGridPos[1] = 0;
			pAgpdItem->m_anGridPos[2] = 0;
			pAgpdItem->m_pcsCharacter = pAgpdCharacter;
			}

		TCHAR *pszItemInsertQuery = NULL;
		if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
			pszItemInsertQuery = GetQuery(_T("CASH_ITEM_INSERT"), pGameDatabase->GetVender());
		else
			pszItemInsertQuery = GetQuery(_T("ITEM_INSERT"), pGameDatabase->GetVender());
		
		if (!pGameDatabase->SetQueryText(pszItemInsertQuery)
			|| !m_pAgsmItem->SetParamInsertQuery5(pGameDatabase, pAgpdItem))
			goto compen_fail;

		// get item seq.
		TCHAR szItemSeq[21];
		ZeroMemory(szItemSeq, sizeof(szItemSeq));
		AgsdItem *pAgsdItem = m_pAgsmItem->GetADItem(pAgpdItem);
		_i64tot(pAgsdItem->m_ullDBIID, szItemSeq, 10);

		// insert cashitembuylist
		if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
			{
			TCHAR szBuySeq[21];
			ZeroMemory(szBuySeq, sizeof(szBuySeq));
			UINT64 ullBuySeq = m_pAgsmItemManager->GetDBID();
			_i64tot(ullBuySeq, szBuySeq, 10);
			
			INT32 lLevel = m_pAgpmCharacter->GetLevel(pAgpdCharacter);
			if (!pGameDatabase->SetQueryText(pszCashItemBuyListInsert)
				|| !pGameDatabase->SetParam(0, szBuySeq, sizeof(szBuySeq))
				|| !pGameDatabase->SetParam(1, pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID))
				|| !pGameDatabase->SetParam(2, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
				|| !pGameDatabase->SetParam(3, &pAgpdCharacter->m_pcsCharacterTemplate->m_lID)
				|| !pGameDatabase->SetParam(4, &lLevel)
				|| !pGameDatabase->SetParam(5, &lItemTID)
				|| !pGameDatabase->SetParam(6, &lItemQty)
				|| !pGameDatabase->SetParam(7, szItemSeq, sizeof(szItemSeq))
				|| !pGameDatabase->SetParam(8, &pAgsdCharacter->m_strIPAddress[0], sizeof(TCHAR) * (pAgsdCharacter->m_strIPAddress.GetLength() + 1))
				|| AUDATABASE2_QR_SUCCESS != pGameDatabase->ExecuteQuery())
				goto compen_fail;
				
			pGameDatabase->EndQuery();
			}

		// Log
		m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_EVENT_GIVE, pAgpdCharacter->m_lID, pAgpdItem, lItemQty);
		}

	INT16 i = 0;	
	if (!pCompenDatabase->SetQueryText(szDetailModify))
		goto compen_fail;
	if (AGPMLOGINDB_COMPEN_TYPE_ALL == pAgpdLoginCompenMaster->m_eType || AGPMLOGINDB_COMPEN_TYPE_ALLC == pAgpdLoginCompenMaster->m_eType)
		{
		if (!pCompenDatabase->SetParam(i++, &(CompenDetail.m_lCompenID))
			|| !pCompenDatabase->SetParam(i++, CompenDetail.m_szAccount, sizeof(CompenDetail.m_szAccount))
			|| !pCompenDatabase->SetParam(i++, CompenDetail.m_szWorld, sizeof(CompenDetail.m_szWorld))
			|| !pCompenDatabase->SetParam(i++, CompenDetail.m_szChar, sizeof(CompenDetail.m_szChar))
			|| AUDATABASE2_QR_SUCCESS != pCompenDatabase->ExecuteQuery())
			goto compen_fail;
		}
	else
		{
		TCHAR szSeq[21];
		_i64tot(CompenDetail.m_llCompenSeq, szSeq, 10);
		if (!pCompenDatabase->SetParam(i++, szSeq, sizeof(szSeq))
			|| AUDATABASE2_QR_SUCCESS != pCompenDatabase->ExecuteQuery())
			goto compen_fail;
		}

	pGameDatabase->CommitTransaction();	
	pCompenDatabase->EndQuery();
	pCompenDatabase->CommitTransaction();
	
	return TRUE;

  compen_fail:		// remove all paid items and abort transaction
	for (INT32 h = 0; h < lNumPaidItems; h++)
		{
		m_pAgpmItem->RemoveItem(PaidItems[h]);
		}

	pCompenDatabase->EndQuery();
	pCompenDatabase->AbortTransaction();
	pGameDatabase->EndQuery();
	pGameDatabase->AbortTransaction();
	return FALSE;
	}




//	Coupon
//===================================================
//
BOOL AgsmLoginDB::ProcessCoupon(TCHAR *pszDBName, AgpdCharacter *pAgpdCharacter, TCHAR *pszWorld)
{
	if (!pszDBName || !pAgpdCharacter || !pszWorld)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	// 쿠폰 지급은 로그인 프로세스와는 다른 트랜잭션 단위이다.
	// 그래서 DB를 새로 얻어온다.
	AuDatabase2		*pMaster = NULL;
	AuDatabase2		*pGame = NULL;

	// find db
	pMaster = GetIdleMasterDatabase();
	if (NULL == pMaster)
		return FALSE;
	
	AuAutoReleaseDS Auto1(pMaster);
	
	pGame= GetIdleGameDatabase(pszDBName);
	if (NULL == pGame)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Cannot get db set of[%s]\n", pszDBName);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return FALSE;
		}
	
	AuAutoReleaseDS Auto2(pGame);

	// select coupon	
	TCHAR *pszSelect = GetQuery(_T("COUPON_SELECT"), pMaster->GetVender());

	// update coupon
	TCHAR *pszUpdate = GetQuery(_T("COUPON_UPDATE"), pMaster->GetVender());

	// insert buy list
	TCHAR *pszInsert = GetQuery(_T("COUPON_INSERT"), pMaster->GetVender());

	if (NULL == pszSelect
		|| NULL == pszUpdate
		|| NULL == pszInsert
		)
		{
		ASSERT(FALSE);
		return FALSE;
		}

	if (!pMaster->SetQueryText(pszSelect)
		|| !pMaster->SetParam(0, pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID))
		|| !pMaster->SetParam(1, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
		|| !pMaster->SetParam(2, pszWorld, sizeof(TCHAR) * ((INT32)_tcslen(pszWorld) + 1))
		|| AUDATABASE2_QR_FAIL == pMaster->ExecuteQuery()
		)
		{
		pMaster->EndQuery();
		pGame->EndQuery();
		return FALSE;
		}

	if (0 >= pMaster->GetReadRows())
		{
		pMaster->EndQuery();
		pGame->EndQuery();
		return TRUE;
		}

	const INT AGSMLOGINDB_MAX_COUPON	= 100;
	ApSafeArray<AgsdLoginCoupon, AGSMLOGINDB_MAX_COUPON> CouponList;
	INT32 ulTotal = 0;	

	do	{
		INT32 lSeqMaster = atoi(pMaster->GetQueryResult(0));
		TCHAR *pszCouponNo = pMaster->GetQueryResult(1);
		TCHAR *pszCompen = pMaster->GetQueryResult(2);
		
		if (0 != lSeqMaster && NULL != pszCouponNo && NULL != pszCompen)
			{
			CouponList[ulTotal].m_lSeqMaster = lSeqMaster;
			_tcsncpy(CouponList[ulTotal].m_szCouponNo, pszCouponNo, 25);
			_tcsncpy(CouponList[ulTotal].m_szCompen, pszCompen, 50);

			ulTotal++;
			}
		
		//} while(pMaster->GetNextRow() && ulTotal < AGSMLOGINDB_MAX_COUPON);
		} while(pMaster->GetNextRow() && ulTotal < 10); // 30개 넘많아...10개씩만 - arycoat 2009.01.19

	pMaster->EndQuery();

	pMaster->StartTranaction();
	pGame->StartTranaction();
	
	for (INT32 i=0; i<ulTotal; ++i)
	{
		// parse compensate
		ApSafeArray<AgsdCouponItem, AGSMLOGINDB_MAX_COUPON> CouponItemList;
		CouponItemList.MemSetAll();
		INT32 lNumCouponItem = 0;
		TCHAR szItem[51];
		ZeroMemory(szItem, sizeof(szItem));
		TCHAR *pszCompen = CouponList[i].m_szCompen;
		TCHAR *pszItem = szItem;

		while (TRUE)
		{
			if (_T('\0') == *pszCompen)
				{
				if (_tcslen(szItem) > 0)
					{
					if (2 == _stscanf(szItem, _T("%d:%d"), &(CouponItemList[lNumCouponItem].m_lItemTID), &(CouponItemList[lNumCouponItem].m_lItemQty)))
						lNumCouponItem++;
					ZeroMemory(szItem, sizeof(szItem));
					}
				break;
				}

			if (_T(',') == *pszCompen)
				{
				if (2 == _stscanf(szItem, _T("%d:%d"), &(CouponItemList[lNumCouponItem].m_lItemTID), &(CouponItemList[lNumCouponItem].m_lItemQty)))
					lNumCouponItem++;
				ZeroMemory(szItem, sizeof(szItem));
				pszItem = szItem;
				pszCompen++;
				}
			
			*pszItem++ = *pszCompen++;
		}

		for (INT32 j=0; j<lNumCouponItem; ++j)
		{
			if (0 == CouponItemList[j].m_lItemTID || 0 == CouponItemList[j].m_lItemQty)
				continue;
			
			// check inventory
			//if ...
			// break;

			// give item
			INT32 lItemTID = CouponItemList[j].m_lItemTID;
			INT32 lItemQty = CouponItemList[j].m_lItemQty;
			AgpdItem *pAgpdItem = m_pAgsmItemManager->CreateItem(lItemTID, pAgpdCharacter, lItemQty);
			if (pAgpdItem)
			{
				if (!m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem, FALSE))
					{
					pAgpdItem->m_eStatus = AGPDITEM_STATUS_INVENTORY;
					pAgpdItem->m_anGridPos[0] = 0;
					pAgpdItem->m_anGridPos[1] = 0;
					pAgpdItem->m_anGridPos[2] = 0;
					pAgpdItem->m_pcsCharacter = pAgpdCharacter;
					}

				TCHAR *pszItemInsertQuery = NULL;
				if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
					pszItemInsertQuery = GetQuery(_T("CASH_ITEM_INSERT"), pGame->GetVender());
				else
					pszItemInsertQuery = GetQuery(_T("ITEM_INSERT"), pGame->GetVender());
				
				if (!pGame->SetQueryText(pszItemInsertQuery)
					|| !m_pAgsmItem->SetParamInsertQuery5(pGame, pAgpdItem)
					)
				{
					// 아이템을 지우고 rollback
					m_pAgpmItem->RemoveItem(pAgpdItem);
					pGame->EndQuery();
					pMaster->EndQuery();
					pGame->AbortTransaction();
					pMaster->AbortTransaction();
					
					return FALSE;
				}

				// get item seq.
				TCHAR szItemSeq[21];
				ZeroMemory(szItemSeq, sizeof(szItemSeq));
				AgsdItem *pAgsdItem = m_pAgsmItem->GetADItem(pAgpdItem);
				_i64tot(pAgsdItem->m_ullDBIID, szItemSeq, 10);

				// insert cashitembuylist
				if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
				{
					TCHAR szBuySeq[21];
					ZeroMemory(szBuySeq, sizeof(szBuySeq));
					UINT64 ullBuySeq = m_pAgsmItemManager->GetDBID();
					_i64tot(ullBuySeq, szBuySeq, 10);
					
					INT32 lLevel = m_pAgpmCharacter->GetLevel(pAgpdCharacter);
					if (!pGame->SetQueryText(pszInsert)
						|| !pGame->SetParam(0, szBuySeq, sizeof(szBuySeq))
						|| !pGame->SetParam(1, pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID))
						|| !pGame->SetParam(2, m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID), sizeof(pAgpdCharacter->m_szID))
						|| !pGame->SetParam(3, &pAgpdCharacter->m_pcsCharacterTemplate->m_lID)
						|| !pGame->SetParam(4, &lLevel)
						|| !pGame->SetParam(5, &lItemTID)
						|| !pGame->SetParam(6, &lItemQty)
						|| !pGame->SetParam(7, szItemSeq, sizeof(szItemSeq))
						|| !pGame->SetParam(8, &pAgsdCharacter->m_strIPAddress[0], sizeof(TCHAR) * (pAgsdCharacter->m_strIPAddress.GetLength() + 1))
						|| AUDATABASE2_QR_SUCCESS != pGame->ExecuteQuery()
						)
					{
						// 아이템을 지우고 rollback
						m_pAgpmItem->RemoveItem(pAgpdItem);
						pGame->EndQuery();
						pMaster->EndQuery();
						pGame->AbortTransaction();
						pMaster->AbortTransaction();

						return FALSE;
					}
				}

				// update amt_coupon_user
				if (!pMaster->SetQueryText(pszUpdate)
					|| !pMaster->SetParam(0, szItemSeq, sizeof(szItemSeq))
					|| !pMaster->SetParam(1, pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID))
					|| !pMaster->SetParam(2, &(CouponList[i].m_lSeqMaster))
					|| !pMaster->SetParam(3, CouponList[i].m_szCouponNo, sizeof(CouponList[i].m_szCouponNo))
					|| AUDATABASE2_QR_SUCCESS != pMaster->ExecuteQuery()
					)
				{
					// 아이템을 지우고 rollback
					m_pAgpmItem->RemoveItem(pAgpdItem);
					pGame->EndQuery();
					pMaster->EndQuery();
					pGame->AbortTransaction();
					pMaster->AbortTransaction();

					return FALSE;
				}
				
				pGame->EndQuery();
				pMaster->EndQuery();

				// Log
				m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_EVENT_GIVE, pAgpdCharacter->m_lID, pAgpdItem, lItemQty);
			}
		}
	}

	pGame->CommitTransaction();
	pMaster->CommitTransaction();

	return TRUE;
}


BOOL AgsmLoginDB::GetEventCouponInfo(AuDatabase2 *pDatabase, AgsdAccount *pcsAccount)
	{
	if (!pDatabase || !pcsAccount)
		return FALSE;

	pDatabase->StartTranaction();

	TCHAR *pszQuery = GetQuery(_T("EVENT_COUPON_SELECT"), pDatabase->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;

	if (!pDatabase->SetQueryText(pszQuery)
		|| !pDatabase->SetParam(0, pcsAccount->m_szName, sizeof(pcsAccount->m_szName))
		|| AUDATABASE2_QR_SUCCESS != pDatabase->ExecuteQuery()
		)
		{
		pDatabase->EndQuery();
		pDatabase->AbortTransaction();

		return FALSE;
		}

	if (pDatabase->GetReadRows() > 0)
		{
		TCHAR	*pszCouponNo	= pDatabase->GetQueryResult(0);
		TCHAR	*pszCharID		= pDatabase->GetQueryResult(1);
		TCHAR	*pszUseYN		= pDatabase->GetQueryResult(2);
		TCHAR	*pszWorld		= pDatabase->GetQueryResult(3);

		if (pszCouponNo && pszCouponNo[0] &&
			pszCharID && pszCharID[0] &&
			pszWorld && pszWorld[0])
			{
			CouponInfo	stCouponInfo;
			ZeroMemory(&stCouponInfo, sizeof(stCouponInfo));

			if (pszUseYN &&
				pszUseYN[0] == _T('Y'))
				stCouponInfo.m_bEnable	= FALSE;
			else
				stCouponInfo.m_bEnable	= TRUE;

			_tcsncpy(stCouponInfo.m_szCouponNo, pszCouponNo, AGSMACCOUNT_MAX_COUPONNO);
			_tcsncpy(stCouponInfo.m_szCharName, pszCharID, AGPACHARACTER_MAX_ID_STRING);
			_tcsncpy(stCouponInfo.m_szWorldName, pszWorld, AGPDWORLD_MAX_WORLD_NAME);

			m_pAgsmAccountManager->SetCouponNum(pcsAccount, 1);
			m_pAgsmAccountManager->AddCouponInfo(pcsAccount, &stCouponInfo);
			}
		}
	
	pDatabase->EndQuery();
	pDatabase->CommitTransaction();

	return TRUE;
	}


BOOL AgsmLoginDB::ProcessEventCoupon(AuDatabase2 *pMaster, AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter, TCHAR *pszWorld)
{
	if (!pMaster || !pDatabase || !pcsCharacter || !pszWorld || !pszWorld[0])
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= m_pAgsmCharacter->GetADCharacter(pcsCharacter);
	AgsdAccount		*pcsAccount			= m_pAgsmAccountManager->GetAccount(pcsAgsdCharacter->m_szAccountID);
	if (!pcsAccount)
		return FALSE;

	if (pcsAccount->m_csCoupon.m_lNumCoupon < 1 ||
		!pcsAccount->m_csCoupon.m_pcsCouponInfo)
		return TRUE;

	// 현재 쿠폰이란 하나밖에 없다.
	if (!pcsAccount->m_csCoupon.m_pcsCouponInfo[0].m_bEnable)
		return TRUE;

	if (_tcscmp(pcsAccount->m_csCoupon.m_pcsCouponInfo[0].m_szCharName, pcsCharacter->m_szID) != 0 ||
		_tcscmp(pcsAccount->m_csCoupon.m_pcsCouponInfo[0].m_szWorldName, pszWorld) != 0)
		return TRUE;

	// 쿠폰 번호가 A로 시작하는 놈들만 해당 사항이 있다. ==> 아니란다
	//if (pcsAccount->m_csCoupon.m_pcsCouponInfo[0].m_szCouponNo[0] != 'A')
	//	return TRUE;

	// 이제 쿠폰을 사용할거니 Disable 시킨다.
	TCHAR	szQuery[256];
	ZeroMemory(szQuery, sizeof(szQuery));

	TCHAR	*pszQuery = GetQuery(_T("EVENT_COUPON_UPDATE"), pMaster->GetVender());
	ASSERT(NULL != pszQuery);
	if (NULL == pszQuery)
		return FALSE;

	_stprintf(szQuery, pszQuery, pcsAccount->m_csCoupon.m_pcsCouponInfo[0].m_szCouponNo);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "쿠폰에 사용 플래그 세팅 : %s\n", pcsCharacter->m_szID);
	AuLogFile_s(_AGSMLOGINDB_COUPON_LOG_, strCharBuff);
	

	if (!pMaster->SetQueryText(szQuery) ||
		AUDATABASE2_QR_SUCCESS != pMaster->ExecuteQuery())
		{
		pMaster->EndQuery();

		char strCharBuff2[256] = { 0, };
		sprintf_s(strCharBuff2, sizeof(strCharBuff2), "쿠폰에 사용 플래그 세팅 실패 : %s\n", pcsCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_COUPON_LOG_, strCharBuff2);
		

		return FALSE;
		}

	pMaster->EndQuery();

	char strCharBuff2[256] = { 0, };
	sprintf_s(strCharBuff2, sizeof(strCharBuff2), "이벤트 상품 지급 : %s\n", pcsCharacter->m_szID);
	AuLogFile_s(_AGSMLOGINDB_COUPON_LOG_, strCharBuff2);
 
	// 오키 이놈한테 이벤트 상품을 준다.

	// 정착금 10만 겔드, 방어구 풀세트, 강화축성제 10개

	// 정착금 10만 겔드
	m_pAgpmCharacter->AddMoney(pcsCharacter, 100000);
	
	m_pAgsmCharacter->WriteGheldLog(AGPDLOGTYPE_GHELD_EVENT_GIVE, pcsCharacter, 100000);

	// 강화축성제 10개
	AgpdItem	*pcsItem	= m_pAgsmItemManager->CreateItem(486, pcsCharacter, 10);
	if (pcsItem)
		{
		if (!m_pAgpmItem->AddItemToInventory(pcsCharacter, pcsItem, FALSE))
			{
			pcsItem->m_eStatus	= AGPDITEM_STATUS_INVENTORY;

			pcsItem->m_anGridPos[0]	= 0;
			pcsItem->m_anGridPos[1]	= 0;
			pcsItem->m_anGridPos[2]	= 0;

			pcsItem->m_pcsCharacter	= pcsCharacter;
			}

		if (!pDatabase->SetQueryText(GetQuery(_T("ITEM_INSERT"), pDatabase->GetVender()))
			|| !m_pAgsmItem->SetParamInsertQuery5(pDatabase, pcsItem)
			)
			{
			char strCharBuff3[256] = { 0, };
			sprintf_s(strCharBuff3, sizeof(strCharBuff3), "강화축성제 Insert 실패 : %s\n", pcsCharacter->m_szID);
			AuLogFile_s(_AGSMLOGINDB_COUPON_LOG_, strCharBuff3);
			}
		else
			{
			m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_EVENT_GIVE, pcsCharacter->m_lID, pcsItem, pcsItem->m_nCount ? pcsItem->m_nCount : 1);
			}

		pDatabase->EndQuery();
		}
	else
		{
		char strCharBuff3[256] = { 0, };
		sprintf_s(strCharBuff3, sizeof(strCharBuff3), "강화축성제 생성 실패 : %s\n", pcsCharacter->m_szID);
		AuLogFile_s(_AGSMLOGINDB_COUPON_LOG_, strCharBuff3);
		}

	// 방어구 풀세트

	INT32	lRace	= m_pAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
	INT32	lClass	= m_pAgpmFactors->GetClass(&pcsCharacter->m_csFactor);

	if (lRace > AURACE_TYPE_NONE && lRace < AURACE_TYPE_MAX &&
		lClass > AUCHARCLASS_TYPE_NONE && lClass < AUCHARCLASS_TYPE_MAX)
		{
		for (int i = 0; i < 4; ++i)
			{
			if (g_alEventArmour[lRace][lClass][i] != AP_INVALID_IID)
				{
				pcsItem	= m_pAgsmItemManager->CreateItem(g_alEventArmour[lRace][lClass][i], pcsCharacter);
				if (pcsItem)
					{
					if (!m_pAgpmItem->AddItemToInventory(pcsCharacter, pcsItem, FALSE))
						{
						pcsItem->m_eStatus	= AGPDITEM_STATUS_INVENTORY;

						pcsItem->m_anGridPos[0]	= 0;
						pcsItem->m_anGridPos[1]	= 0;
						pcsItem->m_anGridPos[2]	= i;

						pcsItem->m_pcsCharacter	= pcsCharacter;
						}

					if (!pDatabase->SetQueryText(GetQuery(_T("ITEM_INSERT"), pDatabase->GetVender()))
						|| !m_pAgsmItem->SetParamInsertQuery5(pDatabase, pcsItem)
						) 
						{
						char strCharBuff3[256] = { 0, };
						sprintf_s(strCharBuff3, sizeof(strCharBuff3), "방어구 Insert 실패 : %s (item tid : %d)\n", pcsCharacter->m_szID, g_alEventArmour[lRace][lClass][i]);
						AuLogFile_s(_AGSMLOGINDB_COUPON_LOG_, strCharBuff3);
						}
					else
						{
						m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_EVENT_GIVE, pcsCharacter->m_lID, pcsItem, pcsItem->m_nCount ? pcsItem->m_nCount : 1);
						}

					pDatabase->EndQuery();
					}
				else
					{
					char strCharBuff3[256] = { 0, };
					sprintf_s(strCharBuff3, sizeof(strCharBuff3), "방어구 생성 실패 : %s (item tid : %d)\n", pcsCharacter->m_szID, g_alEventArmour[lRace][lClass][i]);
					AuLogFile_s(_AGSMLOGINDB_COUPON_LOG_, strCharBuff3);
					}
				}
			}
		}
	else
		{
		char strCharBuff3[256] = { 0, };
		sprintf_s(strCharBuff3, sizeof(strCharBuff3), "잘못된 캐릭터 클래스 : %s (race : %d, class : %d)\n", pcsCharacter->m_szID, lRace, lClass);
		AuLogFile_s(_AGSMLOGINDB_COUPON_LOG_, strCharBuff3);
		}

	char strCharBuff3[256] = { 0, };
	sprintf_s(strCharBuff3, sizeof(strCharBuff3), "이벤트 상품 지급 완료 : %s\n", pcsCharacter->m_szID);
	AuLogFile_s(_AGSMLOGINDB_COUPON_LOG_, strCharBuff3);

	return TRUE;
	}




#define _ctextend _T('\0')

BOOL AgsmLoginDB::LoadQuery(TCHAR *pszFile, eAUDB_VENDER eVender)
	{
	if (NULL == pszFile || _ctextend == *pszFile)
		return FALSE;

	if (AUDB_VENDER_ORACLE > eVender || eVender >= AUDB_VENDER_MAX)
		return FALSE;
	
	m_QueryMap[eVender].SetCount(200);
	if (!m_QueryMap[eVender].InitializeObject(sizeof (TCHAR *), m_QueryMap[eVender].GetCount()))
		return FALSE;
	
	// read ini
	TCHAR sz[2000];
	ZeroMemory(sz, sizeof(sz));
	GetPrivateProfileString(_T("QUERY"), NULL, _T(""), sz, sizeof(sz), pszFile);
	TCHAR *psz = sz;

	TCHAR szKey[128];
	TCHAR szValue[1024];
	TCHAR *pszBuf = NULL;

	while (_ctextend != *psz)
		{
		pszBuf = szKey;
		while (_ctextend != *psz)
			*pszBuf++ = *psz++;
		*pszBuf = _ctextend;
		GetPrivateProfileString("QUERY", szKey, "", szValue, sizeof(szValue), pszFile);
		
		TCHAR *pszQuery = new TCHAR[_tcslen(szValue) + 1];
		ZeroMemory(pszQuery, sizeof(TCHAR) * (_tcslen(szValue) + 1));
		_tcscpy(pszQuery, szValue);
		
		if (!m_QueryMap[eVender].AddObject(&pszQuery, szKey))
			{
			RemoveAllQuery();
			return FALSE;
			}

		psz++;
		}
	
	return TRUE;
	}

#undef _ctextend


void AgsmLoginDB::RemoveAllQuery()
	{
	for (INT32 i=0; i < AUDB_VENDER_MAX; i++)
		{
		INT32 lIndex = 0;
		TCHAR **ppszQuery = (TCHAR **) m_QueryMap[i].GetObjectSequence(&lIndex);
		while (ppszQuery)
			{
			if (*ppszQuery)
				delete [] *ppszQuery;
			
			ppszQuery = (TCHAR **) m_QueryMap[i].GetObjectSequence(&lIndex);
			}
		
		m_QueryMap[i].Reset();
		}
	}


TCHAR* AgsmLoginDB::GetQuery(TCHAR *pszKey, eAUDB_VENDER eVender)
	{
	//AuLogFile(_AGSMLOGINDB_LOG_, _T("GetQuery :: Key[%s], Vender[%d]\n"), pszKey, (INT32)eVender);

	if (AUDB_VENDER_ORACLE > eVender || eVender >= AUDB_VENDER_MAX)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Query is NULL [%s], Vender[%d]\n", pszKey, (INT32)eVender);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		
		return NULL;	
		}
	
	TCHAR **ppszQuery = (TCHAR **) m_QueryMap[eVender].GetObject(pszKey);
	if (NULL != ppszQuery)
		{
		if(!*ppszQuery)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Query is NULL [%s], Vender[%d]\n", pszKey, (INT32)eVender);
			AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
		}

		return *ppszQuery;
		}

	if(!ppszQuery || !*ppszQuery)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!! Error : Query is NULL [%s], Vender[%d]\n", pszKey, (INT32)eVender);
		AuLogFile_s(_AGSMLOGINDB_LOG_, strCharBuff);
	}

	return NULL;
	}

#ifdef _AREA_CHINA_

	AgsmClientInterface::AgsmClientInterface()
	{
	}

	AgsmClientInterface::~AgsmClientInterface()
	{
	}

	bool AgsmClientInterface::InitPT()
	{
		m_pAS->Init("ini/pt.ini");
		m_pAS->SetClientInterface(this);

		// PT 쓰레드가 접속을 시도하는게 보통 2~3초이후부터다.
		// 실제 접속이 되기를 최대 5초간 기다린다.
		//
		// 실제 서비스할때는 연결되지 않으면 로컬의 디비에서 인증과정을
		// 대체하기때문에 반드시 접속되어야하는건 아니다.
		printf("wait for PT connecting...\n");
		for (int i = 0; i < 5; ++i)
		{
			if (m_pAS->IsConnect())
			{
				printf("PT connected.\n");
				break;
			}
			Sleep(1000);
		}

		return m_pAS->IsConnect();
	}

	void AgsmClientInterface::QuestEKey(CSessionPtr &p_session, const char *challegeNum)
	{
		AgsmLoginQueueInfo* lqi = g_smgr.GetQiBySession(p_session->GetSessionId());
		if (lqi)
		{
			CSessionPtr* ptr = new CSessionPtr;
			*ptr = p_session;
			
			g_smgr.NewNidToSession(lqi->m_ulCharNID, ptr);

			m_pAgsmLoginClient->SendEKeyChallengeNum(challegeNum, lqi->m_ulCharNID);
		}
	}

	void AgsmClientInterface::OperateResult(CSessionPtr &p_session, int operation, int result)
	{
		// operation 쓸일이 없다.
		//switch (operation)
		//{
		//case authen:
		//	break;
		//case ekeyauth:
		//	break;
		//case regist:
		//	break;
		//case upgrade:
		//	break;
		//case checkid:
		//	break;
		//case changepwd:
		//	break;
		//default:
		//	break;
		//}

		// 정상적인 PT 계정 과정
		//AccountToQi::iterator iterATQ = m_atq.end();
		//SessionToQi::iterator iterSTQ = m_stq.find(p_session->GetSessionId());
		AgsmLoginQueueInfo* pQueryInfo = g_smgr.GetQiBySession(p_session->GetSessionId());
		if (0 == pQueryInfo)
			pQueryInfo = g_smgr.GetQiByAccount( p_session->GetValue("ArchInput") );

		if (0 == pQueryInfo) return;

		unsigned nid = pQueryInfo->m_ulCharNID;

		// 계정 이름을 임시로 저장하자
		char tempAccount[AGPACHARACTER_MAX_ID_STRING + 1] = {0, };
		
		strncpy(tempAccount, pQueryInfo->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
		tempAccount[AGPACHARACTER_MAX_ID_STRING] = 0;

		switch (result)
		{
		case failed:
		case miss_pwd:
			// increase failure count. if limit exceed, destroy
			if (m_pAgsmLoginClient->AddInvalidPasswordLog(nid))
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_INVALID_PASSWORD, NULL, nid);
			else
			{
				m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_INVALID_PASSWORD_LIMIT_EXCEED, NULL, nid);
				m_pAgsmLoginDB->DestroyClient(nid);
			}
			break;

		case success:
			{
				const char* pta = p_session->GetValue("PopTangId");

				// 로그인한 계정이 digit account일 경우에 PT account로 계정이름을 바꾼다.
				if (strncmp(pta, pQueryInfo->m_szAccountID, AGPACHARACTER_MAX_ID_STRING))
				{
					strncpy(pQueryInfo->m_szAccountID, pta, AGPACHARACTER_MAX_ID_STRING);
					pQueryInfo->m_szAccountID[AGPACHARACTER_MAX_ID_STRING] = 0;
				}

				pQueryInfo->m_isLimited = p_session->GetValueInt("IsLimited");
				pQueryInfo->m_isProtected = p_session->GetValueInt("IsProtected");

				m_pAgsmLoginDB->ProcessCheckAccount2(pQueryInfo);
			}
			break;
		case no_user:
			m_pAgsmLoginClient->SendLoginResult(AGPMLOGIN_RESULT_INVALID_ACCOUNT, NULL, nid);
			break;

		case refuse:
		case no_bind_ekey:
		case ekey_warp:
		case user_existed:
		case time_out:
		case need_reserve_pwd:
		default:
			m_pAgsmLoginDB->DestroyClient(nid);
			break;
		}

		// remove temp session info
		g_smgr.RemoveAccount(tempAccount);
		g_smgr.RemoveSession(p_session->GetSessionId());
		g_smgr.RemoveNid(pQueryInfo->m_ulCharNID);

		delete pQueryInfo;

	}

#endif

#ifdef _WEBZEN_AUTH_
//인증키를 비교하고 나이 정보를 받아온다.
int AgsmLoginDB::WebzenAuthFromDB(AgsmLoginQueueInfo *pQueryInfo, INT32 &nAge) 
	{
		AuDatabase2		*pDatabase = NULL;
		TCHAR *pszQuery	= NULL;

		pDatabase = GetIdleMasterDatabase();
		if (!pDatabase)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginDB::WebzenAuthFromDB - 1\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

			return FALSE;
		}
		//sp 호출

		pszQuery = GetQuery(_T("WEBZEN_AUTH"), pDatabase->GetVender());
		ASSERT(NULL != pszQuery);
		if (NULL == pszQuery)
			return FALSE;

		AuAutoReleaseDS Auto(pDatabase);

		INT32 AccountType	= 0;
		INT32 BlockState	= 0;
		INT32 Age			= 0;	// Age
		INT32 PCBangIdx		= 0;
		INT32 Result		= 0;

		//INT32 gameCode = 8; // 웹젠인증팀에서 게임 아이디 정해준거임 =  게임코드(1 : 뮤, 2 : 썬, 8 : 아크로드, 9 : R2 )
		char* gameCode = "08"; // 문자열로 바뀌었음...웹젠인증팀에서 게임 아이디 정해준거임 =  게임코드(1 : 뮤, 2 : 썬, 8 : 아크로드, 9 : R2 )
		INT32 dwAccountGUID;
		string szAuthKey;
		//string szIPAddress;
		INT32 dwClientCnt;
		

		ParseWebzenGamestring(pQueryInfo->m_szGBAuthString, dwAccountGUID, szAuthKey, dwClientCnt);

		char* authKey = const_cast<char*>(szAuthKey.c_str());
		char* ipadress = GetPlayerIPAddress(pQueryInfo->m_ulCharNID);
		

		//cout << "[Global Auth - Call Function] WebzenAuthFromDB()" << endl;
		//cout << "[Global Auth - Input Parameter] SetParam(1)" << "dwAccountGuid = " << dwAccountGUID << endl;
		//cout << "[Global Auth - Input Parameter] SetParam(2)" << "GameCode = " << gameCode << endl;
		//cout << "[Global Auth - Input Parameter] SetParam(3)" << "GBAuthString = " << authKey << endl;
		//cout << "[Global Auth - Input Parameter] SetParam(4)" << "IPAddress = " << ipadress << endl;
		

		if (!pDatabase->SetQueryText(pszQuery) ||
			!pDatabase->SetParam(0, &dwAccountGUID) ||
			!pDatabase->SetParam(1, gameCode, sizeof(gameCode)) || //Gamecode
			!pDatabase->SetParam(2, authKey, sizeof(authKey)) ||
			!pDatabase->SetParam(3, ipadress, sizeof(ipadress) ) ||
			!pDatabase->SetParam(4, &AccountType, TRUE) ||
			!pDatabase->SetParam(5, &BlockState, TRUE) ||
			!pDatabase->SetParam(6, &Age, TRUE) ||
			!pDatabase->SetParam(7, &PCBangIdx, TRUE) ||
			!pDatabase->SetParam(8, &Result, TRUE)
			)
		{
			pDatabase->EndQuery();

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "WebzenAuthFromDB - SetParm Error ( %d, %s, %s, %s)\n" ,dwAccountGUID, gameCode, authKey, ipadress);
			AuLogFile_s(LOGIN_AUTH_FILE_NAME, strCharBuff);

			return -1;
		}

		if (AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
			pDatabase->EndQuery();

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "WebzenAuthFromDB - ExecuteQuery Error\n");
			AuLogFile_s(LOGIN_AUTH_FILE_NAME, strCharBuff);

			return -2;
		}

		pDatabase->EndQuery();

		//cout << "[Global Auth - Call Function] GlobalAuthFromDB()" << endl;
		//cout << "[Global Auth - Output Parameter] Result(1)" << "AccountType = " << re1 << endl;
		//cout << "[Global Auth - Output Parameter] Result(2)" << "BlockState = " << re2 << endl;
		//cout << "[Global Auth - Output Parameter] Result(3)" << "Age = " << re3 << endl;
		//cout << "[Global Auth - Output Parameter] Result(4)" << "Result = " << re4 << endl;

		//result re4 값 여부에 따른 에러 처리 코드 작업 해야함.

		if(BlockState > 0)
			pQueryInfo->m_dwAccountType = AccountType;

		nAge = Age; //나이정보
		pQueryInfo->m_dwPCRoomGuid = PCBangIdx;//PC방 Guid
		pQueryInfo->m_dwAge		   = Age;
		pQueryInfo->m_dwClientCnt  = dwClientCnt;

		return Result;
	}

	bool AgsmLoginDB::ParseWebzenGamestring(  char* szGameString ,int &dwAccountID, string &szGameAuthKey, int &dwClientCnt)
	{

		if( NULL == szGameString )
			return FALSE;

		string tmp = szGameString;
		string::size_type idx = tmp.find('|');

		if( idx == string::npos )
			return FALSE;
		else
		{
			// Find gameid
			string szAccountId = tmp.substr( 0, idx );
			dwAccountID = atoi(szAccountId.c_str());

			tmp = tmp.substr( idx+1 );

			idx = tmp.find('|'); //Find mac address.

			if( idx == string::npos )
				szGameAuthKey = tmp; //Can't find Ip address info.
			else
			{
				szGameAuthKey = tmp.substr( 0, idx );
				string szClientCnt = tmp.substr( idx+1 );
				dwClientCnt = atoi(szClientCnt.c_str());
			}
		}

		//std::cout << "GameID = " << szGameId << " " << "GameAuthKey = " << szGameAuthKey << "MacAddress = " << szMacAddress << std::endl;

		return TRUE;
	}
//JK_웹젠인증 : 케릭터 선택하기로 돌아 왔을때 나이 정보와 PCGuid를 다시 받아온다.
int AgsmLoginDB::GetWebzenAuthInfoFromDB(AgsmLoginQueueInfo *pQueryInfo) 
	{
		AuDatabase2		*pDatabase = NULL;
		TCHAR *pszQuery	= NULL;

		pDatabase = GetIdleMasterDatabase();
		if (!pDatabase)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginDB::GetWebzenAuthInfoFromDB - 1\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

			return FALSE;
		}
		//sp 호출

		pszQuery = GetQuery(_T("GETWEBZEN_AUTHINFO"), pDatabase->GetVender());
		ASSERT(NULL != pszQuery);
		if (NULL == pszQuery)
			return FALSE;

		AuAutoReleaseDS Auto(pDatabase);

		INT32 Age			= 0;	// Age
		INT32 PCBangIdx		= 0;
		INT32 Result		= 0;

		char* gameCode = "08"; // 문자열로 바뀌었음...웹젠인증팀에서 게임 아이디 정해준거임 =  게임코드(1 : 뮤, 2 : 썬, 8 : 아크로드, 9 : R2 )
		INT32 dwAccountGUID;
		string szAuthKey;
		//string szIPAddress;
		INT32 dwClientCnt;


		ParseWebzenGamestring(pQueryInfo->m_szGBAuthString, dwAccountGUID, szAuthKey, dwClientCnt);

		char* authKey = const_cast<char*>(szAuthKey.c_str());
		char* ipadress = GetPlayerIPAddress(pQueryInfo->m_ulCharNID);


		if (!pDatabase->SetQueryText(pszQuery) ||
			!pDatabase->SetParam(0, &dwAccountGUID) ||
			!pDatabase->SetParam(1, ipadress, sizeof(ipadress) ) ||
			!pDatabase->SetParam(2, &Age, TRUE) ||
			!pDatabase->SetParam(3, &PCBangIdx, TRUE) ||
			!pDatabase->SetParam(4, &Result, TRUE)
			)
		{
			pDatabase->EndQuery();

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "GetWebzenAuthInfoFromDB - SetParm Error ( AccountGUId : %d)\n" ,dwAccountGUID);
			AuLogFile_s(LOGIN_AUTH_FILE_NAME, strCharBuff);

			return -1;
		}

		if (AUDATABASE2_QR_FAIL == pDatabase->ExecuteQuery())
		{
			pDatabase->EndQuery();

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "GetWebzenAuthInfoFromDB - ExecuteQuery Error\n");
			AuLogFile_s(LOGIN_AUTH_FILE_NAME, strCharBuff);

			return -2;
		}

		pDatabase->EndQuery();
		
		pQueryInfo->m_dwPCRoomGuid = PCBangIdx;//PC방 Guid
		pQueryInfo->m_dwAge		   = Age;
		pQueryInfo->m_dwClientCnt  = dwClientCnt;

		return Result;
	}



	bool AgsmLoginDB::ParseGlobalGamestring( char* szGameString ,string &szGameId, string &szGameAuthKey, string &szMacAddress)
	{

		if( NULL == szGameString )
			return FALSE;

		string tmp = szGameString;
		string::size_type idx = tmp.find('|');

		if( idx == string::npos )
			return FALSE;
		else
		{
			// Find gameid
			szGameId = tmp.substr( 0, idx );
			tmp = tmp.substr( idx+1 );

			idx = tmp.find('|'); //Find mac address.

			if( idx == string::npos )
				szGameAuthKey = tmp; //Can't find mac address info.
			else
			{
				szGameAuthKey = tmp.substr( 0, idx );
				szMacAddress = tmp.substr( idx+1 );
			}
		}

		//std::cout << "GameID = " << szGameId << " " << "GameAuthKey = " << szGameAuthKey << "MacAddress = " << szMacAddress << std::endl;

		return TRUE;
	}
#endif