/*===========================================================================

	AgsmLoginClient.cpp

===========================================================================*/


#include "AgsmLoginClient.h"
#include "ApModuleStream.h"
#include "AuStrTable.h"
#include "AuStringConv.h"

#ifdef _AREA_JAPAN_
#include "AuJapaneseClientAuth.h"
#endif

#define LOGIN_ERROR_FILE_NAME		"LOG\\LoginError.log"


CHAR	*g_pszCharName[AUCHARCLASS_TYPE_MAX * (AURACE_TYPE_MAX - 1)]	=
	{	
	NULL,
	};
 
INT32 g_lCharTID[AUCHARCLASS_TYPE_MAX * 3]	=
	{
	0,
	};

/********************************************************/
/*		The Implementation of AgsmLoginClient Class		*/
/********************************************************/
//
AgsmLoginClient::AgsmLoginClient()
	{
	SetModuleName(_T("AgsmLoginClient"));
	SetModuleType(APMODULE_TYPE_SERVER);
	SetPacketType(AGPMLOGIN_PACKET_TYPE);

	SetModuleData(sizeof(AgsmBaseCharacterPos), AGSMLOGIN_DATA_TYPE_BASE_CHAR_POS);

	ZeroMemory(m_lRaceBaseCharacter, sizeof(INT32) * AURACE_TYPE_MAX * (AUCHARCLASS_TYPE_MAX * 3));
	ZeroMemory(m_RaceBaseCharacterPos, sizeof(AgsmBaseCharacterPos) * AURACE_TYPE_MAX * (AUCHARCLASS_TYPE_MAX * 3));
	
	m_lMaxProcessUserCount			= g_lMaxProcessUserCount;
	m_lMaxCreateProcessUserCount	= g_lMaxCreateProcessUserCount;

	ZeroMemory(g_lCharTID, sizeof(INT32) * (AUCHARCLASS_TYPE_MAX * 3));

	g_pszCharName[0] = ServerStr().GetStr(STI_KNIGHT);
	g_pszCharName[1] = ServerStr().GetStr(STI_ARCHER);
	g_pszCharName[2] = ServerStr().GetStr(STI_MAGE);
	g_pszCharName[3] = ServerStr().GetStr(STI_BERSERKER);
	g_pszCharName[4] = ServerStr().GetStr(STI_HUNTER);
	g_pszCharName[5] = ServerStr().GetStr(STI_SORCERER);
	g_pszCharName[6] = ServerStr().GetStr(STI_ELEMETALIST);
	g_pszCharName[7] = ServerStr().GetStr(STI_RANGER);
	g_pszCharName[8] = ServerStr().GetStr(STI_SWASHBUKLER);
	g_pszCharName[9] = ServerStr().GetStr(STI_SCION);

	g_lCharTID[0] = 96; //휴먼 남전사
	g_lCharTID[1] = 1;  //휴먼 궁수
	g_lCharTID[2] = 6;  //휴먼 여자 법사.
	g_lCharTID[3] = 4;  //오크 남전사
	g_lCharTID[4] = 8;  //오크 여궁수
	g_lCharTID[5] = 3;  //오크 마법사.
	g_lCharTID[6] = 9;  // 문엘프 여자 법사
	g_lCharTID[7] = 460;// 문엘프 여자 궁수
	g_lCharTID[8] = 377;// 문엘프 여자 전사
	g_lCharTID[9] = 1724;// 드레곤시온 시온
	}


AgsmLoginClient::~AgsmLoginClient()
	{
	}


BOOL AgsmLoginClient::OnInit()
{
	return TRUE;
}


//	ApModule inherited
//=========================================================
//
BOOL AgsmLoginClient::OnAddModule()
	{
	m_csCertificatedNID.InitializeObject(sizeof(AgsmLoginEncryptInfo*), AGPMLOGIN_MAX_CONNECTION);

	m_pAgpmLogin = (AgpmLogin *) GetModule(_T("AgpmLogin"));
	m_pAgpmFactors = (AgpmFactors *) GetModule(_T("AgpmFactors"));
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmItem = (AgpmItem *) GetModule(_T("AgpmItem"));
	m_pAgpmAdmin = (AgpmAdmin *) GetModule(_T("AgpmAdmin"));
	m_pAgsmAccountManager = (AgsmAccountManager *) GetModule(_T("AgsmAccountManager"));
	m_pAgsmCharacter = (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	m_pAgsmCharManager = (AgsmCharManager *) GetModule(_T("AgsmCharManager"));
	m_pAgsmItem = (AgsmItem *) GetModule(_T("AgsmItem"));
	m_pAgsmItemManager = (AgsmItemManager *) GetModule(_T("AgsmItemManager"));
	m_pAgsmServerManager = (AgsmServerManager *) GetModule(_T("AgsmServerManager2"));
	m_pAgsmLoginServer = (AgsmLoginServer *) GetModule(_T("AgsmLoginServer"));
	m_pAgpmResourceInfo = (AgpmResourceInfo *) GetModule(_T("AgpmResourceInfo"));
	m_pAgsmFactors = (AgsmFactors *) GetModule(_T("AgsmFactors"));
	m_pAgsmGK = (AgsmGK *) GetModule(_T("AgsmGK"));
	m_pAgpmEventCharCustomize = (AgpmEventCharCustomize *) GetModule(_T("AgpmEventCharCustomize"));
	m_pAgpmStartupEncryption = (AgpmStartupEncryption *) GetModule(_T("AgpmStartupEncryption"));
	m_pAgpmConfig = (AgpmConfig*)GetModule(_T("AgpmConfig"));//JK_심야샷다운

	if (!m_pAgpmLogin || !m_pAgpmFactors || !m_pAgpmCharacter || !m_pAgsmCharacter || !m_pAgsmCharManager || 
		!m_pAgpmItem || !m_pAgsmItem || !m_pAgsmItemManager || !m_pAgsmServerManager ||
		!m_pAgsmLoginServer || !m_pAgsmAccountManager || !m_pAgpmAdmin || !m_pAgsmFactors || !m_pAgsmGK || !m_pAgpmEventCharCustomize || !m_pAgpmConfig
		)
		{
		return FALSE;
		}

	// client disconnect callback
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	pModuleManager->SetCBLoginDisconnect(this, CBDisconnect);

	if (!m_pAgsmCharacter->SetCallbackReceiveNewCID(CBReceiveNewCID, this))
		return FALSE;
	if (!m_pAgsmCharacter->SetCallbackCompleteSendCharacterInfo(CBCompleteSendCharacterInfo, this))
		return FALSE;

	if (!AddStreamCallback(AGSMLOGIN_DATA_TYPE_BASE_CHAR_POS, BaseCharacterPosReadCB, NULL, this))
		return FALSE;

	return TRUE;
	}


BOOL AgsmLoginClient::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	BOOL bResult = FALSE;

	if (FALSE == m_pAgsmLoginServer->CheckQueueCount())
		{
		_tprintf("!!! Warning : Queue FULL in AgsmLoginClient::OnReceive\n");
		return bResult;
		}
	
	INT8	cOperation;
	CHAR	*pszEncryptCode = NULL;
	CHAR	*pszAccountID = NULL;
	CHAR	*pszPassword = NULL;
	INT8	cAccountLen = 0;
	INT8	cPasswordLen = 0;
	INT32	lCID = AP_INVALID_CID;
	CHAR	*pszServerName = NULL;
	PVOID	pvCharDetailInfo = NULL;
	PVOID	pvDetailServerInfo = NULL;
	INT32	lResult;
	PVOID	pvPacketVersionInfo	= NULL;
	CHAR	*pszExtraForForeign = 0;		// ekey for china, authstring for japan.
	CHAR	*pszChallenge = 0;
	INT32	lIsLimited = 0;
	INT32	lIsProtected = 0;

	m_pAgpmLogin->m_csPacket.GetField(TRUE, pvPacket, nSize,
								&cOperation,
								&pszEncryptCode,
								&pszAccountID,
								&cAccountLen,
								&pszPassword,
								&cPasswordLen,
								&lCID,
								&pszServerName,
								&pvCharDetailInfo,
								&pvDetailServerInfo,
								&lResult,
								&pvPacketVersionInfo,
								&pszExtraForForeign,
								&pszChallenge,
								&lIsLimited,
								&lIsProtected
								);

	
	if (AGPMLOGIN_ENCRYPT_CODE == cOperation)
		{
		_tprintf(_T("AGPMLOGIN_GET_ENCRYPT_CODE in AgsmLoginClient::OnReceive()\n"));
		bResult = ProcessEncryptCode(pvPacketVersionInfo, ulNID);
		if (!bResult)
			DestroyClient(ulNID);
		}
	else if (AGPMLOGIN_SIGN_ON == cOperation)
		{
		_tprintf(_T("AGPMLOGIN_SIGN_ON in AgsmLoginClient::OnReceive()\n"));
		bResult = ProcessSignOn(pszAccountID, cAccountLen, pszPassword, cPasswordLen, ulNID, pszExtraForForeign);
		}
	else if (AGPMLOGIN_EKEY == cOperation)
	{
		printf( "AGPMLOGIN_EKEY in AgsmLoginClient::OnReceive()\n");
		bResult = ProcessEKey(pszExtraForForeign, ulNID);
	}
	else
		{
		if (FALSE == IsCertificatedNID(ulNID))
			return FALSE;

		AgsmLoginEncryptInfo *	pEncryptInfo = GetCertificatedNID(ulNID);
		if (!pEncryptInfo)
			return FALSE;

		if (_tcslen(pEncryptInfo->m_szAccountID) <= 0
			|| _tcscmp(pEncryptInfo->m_szAccountID, pszAccountID) != 0
			)
			return FALSE;

		if (cOperation != AGPMLOGIN_UNION_INFO)
			pszServerName = pEncryptInfo->m_szServerName;

		AgsdServer *			pcsServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
		CHAR *					pszDBName = NULL;

		if (pcsServer)
			pszDBName = pcsServer->m_szWorld;
		
		switch (cOperation)
			{
			case AGPMLOGIN_UNION_INFO :
				if (NULL == pszDBName)
					return FALSE;
				_tprintf(_T("AGPMLOGIN_UNION_INFO in AgsmLoginClient::OnReceive()\n"));
				bResult = ProcessGetUnion(pszAccountID, pszServerName, pszDBName, ulNID);

				_tcscpy(pEncryptInfo->m_szServerName, pszServerName);

				break;

			case AGPMLOGIN_CHARACTER_INFO :
				if (NULL == pszDBName)
					return FALSE;
				_tprintf(_T("AGPMLOGIN_CHARACTER_INFO in AgsmLoginClient::OnReceive\n"));
				// reset compensation
				pEncryptInfo->m_lCompenID = 0;
				ZeroMemory(pEncryptInfo->m_szChar4Compen, sizeof(pEncryptInfo->m_szChar4Compen));
				bResult = ProcessGetCharacters(pszAccountID, pszServerName, pszDBName, ulNID);
				break;

			case AGPMLOGIN_ENTER_GAME :		
				if (NULL == pszDBName)
					return FALSE;
				_tprintf(_T("AGPMLOGIN_ENTER_GAME(Select Character) in AgsmLoginClient::OnReceive\n"));
				bResult = ProcessSelectCharacter(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID, lCID);
				break;

			case AGPMLOGIN_RETURN_TO_SELECT_WORLD :
				_tprintf(_T("AGPMLOGIN_RETURN_TO_SELECT_WORLD in AgsmLoginClient::OnReceive()\n"));
				bResult = ProcessReturnToSelectWorld(pszAccountID, ulNID);
				break;

			// Create Character
			case AGPMLOGIN_RACE_BASE :
				_tprintf(_T("AGPMLOGIN_RACE_BASE in AgsmLoginClient::OnReceive()\n"));
				bResult = ProcessBaseCharacterOfRace(pvCharDetailInfo, ulNID);
				break;

			case AGPMLOGIN_NEW_CHARACTER_NAME :
				if (NULL == pszDBName)
					return FALSE;
				_tprintf(_T("AGPMLOGIN_NEW_CHARACTER_NAME(Create Character) in AgsmLoginClient::OnReceive()\n"));
				bResult = ProcessCreateCharacter(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID);
				break;

			// Rename Character
			case AGPMLOGIN_RENAME_CHARACTER :
				if (NULL == pszDBName)
					return FALSE;
				_tprintf(_T("AGPMLOGIN_RENAME_CHARACTER in AgsmLoginClient::OnReceive()\n"));
				bResult = ProcessRenameCharacter(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID);
				break;			
			
			// Remove Character
			case AGPMLOGIN_REMOVE_CHARACTER :
				if (NULL == pszDBName)
					return FALSE;
				_tprintf(_T("AGPMLOGIN_REMOVE_CHARACTER in AgsmLoginClient::OnReceive()\n"));
				//JK_케릭삭제패스워드확인
				//bResult = ProcessRemoveCharacter(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID);
				bResult = ProcessRemoveCharacter(pszAccountID, pszPassword, pszServerName, pszDBName, pvCharDetailInfo, ulNID);
				break;
				
			case AGPMLOGIN_COMPENSATION_CHARACTER_SELECT :
				if (NULL == pszDBName)
					return FALSE;
				_tprintf(_T("AGPMLOGIN_COMPENSATION_CHARACTER_SELECT in AgsmLoginClient::OnReceive()\n"));
				bResult = ProcessCharacter4Compensation(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID, TRUE);
				break;			
			
			case AGPMLOGIN_COMPENSATION_CHARACTER_CANCEL :
				if (NULL == pszDBName)
					return FALSE;
				_tprintf(_T("AGPMLOGIN_COMPENSATION_CHARACTER_CANCEL in AgsmLoginClient::OnReceive()\n"));
				bResult = ProcessCharacter4Compensation(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID, FALSE);
				break;
				
			default :
				break;
			}
		}
	
	return bResult;
	}




//	Admin
//==========================================
//
AgsmLoginEncryptInfo* AgsmLoginClient::GetCertificatedNID( UINT32 ulNID )
	{
	AgsmLoginEncryptInfo **ppAgsmLoginEncryptInfo;

	ppAgsmLoginEncryptInfo = (AgsmLoginEncryptInfo **)m_csCertificatedNID.GetObject( ulNID );

	if (ppAgsmLoginEncryptInfo)
		return *ppAgsmLoginEncryptInfo;
		
	return NULL;
	}


BOOL AgsmLoginClient::IsCertificatedNID( UINT32 ulNID )
	{
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL != pAgsmLoginEncryptInfo)
		{
		return pAgsmLoginEncryptInfo->m_bLogin;
		}
		
	return FALSE;
	}


BOOL AgsmLoginClient::SetCertificatedNID(UINT32 ulNID, CHAR *pszAccount, UINT64 ullKey)
	{
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL != pAgsmLoginEncryptInfo)
		{
		pAgsmLoginEncryptInfo->m_bLogin = TRUE;
		ZeroMemory(pAgsmLoginEncryptInfo->m_szAccountID, sizeof(pAgsmLoginEncryptInfo->m_szAccountID));
		strncpy(pAgsmLoginEncryptInfo->m_szAccountID, pszAccount, AGPACHARACTER_MAX_ID_STRING-1);
		pAgsmLoginEncryptInfo->m_ullKey = ullKey;
		
		return TRUE;
		}
	
	return FALSE;
	}


BOOL AgsmLoginClient::SetLoginStep(UINT32 ulNID, AgpdLoginStep	eLoginStep)
	{
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL != pAgsmLoginEncryptInfo)
		{
		pAgsmLoginEncryptInfo->m_eLoginStep = eLoginStep;
		return TRUE;
		}
	
	return FALSE;
	}


AgpdLoginStep AgsmLoginClient::GetLoginStep(UINT32 ulNID)
	{
	AgpdLoginStep eLoginStep = AGPMLOGIN_STEP_NONE;
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL != pAgsmLoginEncryptInfo)
		{
		eLoginStep = pAgsmLoginEncryptInfo->m_eLoginStep;
		}
	
	return eLoginStep;
	}




//	Operations
//=========================================================
//
BOOL AgsmLoginClient::ProcessEncryptCode(PVOID pvPacketVersionInfo, UINT32 ulNID, BOOL bCheckVersion)
	{
	if (m_pAgpmStartupEncryption)
		{
		if(!m_pAgpmStartupEncryption->CheckCompleteStartupEncryption(ulNID))
			{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessEncryptCode - 1\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
			return FALSE;
			}
		}

	if (TRUE == bCheckVersion)
		{
		if (NULL == pvPacketVersionInfo)
			{
			_tprintf(_T("!!! Warning : Invalid Client Version\n"));
			SendInvalidClientVersion(ulNID);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessEncryptCode - 2\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
			return FALSE;
			}

		INT32	lMajorVersion	= 0;
		INT32	lMinorVersion	= 0;

		// check client version
		m_pAgpmLogin->m_csPacketVersionInfo.GetField(FALSE, pvPacketVersionInfo, 0, &lMajorVersion, &lMinorVersion);

		if (FALSE == m_pAgpmResourceInfo->CheckValidVersion(lMajorVersion, lMinorVersion))
			{
			_tprintf(_T("!!! Warning : Invalid Client Version\n"));
			SendInvalidClientVersion(ulNID);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessEncryptCode - 3\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

			return FALSE;
			}
		}

	BOOL bResult = FALSE;
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = new AgsmLoginEncryptInfo;

	if (NULL != m_csCertificatedNID.GetObject(ulNID))
		{
		m_pAgsmCharManager->DestroyClient( ulNID );

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessEncryptCode - 4\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
		}
	else
		{
		if (NULL != m_csCertificatedNID.AddObject((PVOID *)&pAgsmLoginEncryptInfo, ulNID))
			{
			CHAR *pszEncryptCode = m_csMD5Encrypt.GetRandomHashString(ENCRYPT_STRING_SIZE);
			if (NULL != pszEncryptCode)
				{
				//EncryptData 세팅.
				pAgsmLoginEncryptInfo->m_pszEncryptData = pszEncryptCode;
				bResult = SendEncryptCode(pszEncryptCode, ulNID);
				}
			}
		else
			{
			m_pAgsmCharManager->DestroyClient(ulNID);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessEncryptCode - 5\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
			}
		}

	return bResult;
	}

BOOL AgsmLoginClient::ProcessSignOn(CHAR *pszAccount, INT8 cAccountLen, CHAR *pszPassword, INT8 cPasswordLen, UINT32 ulNID, CHAR* pszExtraForForeign)
	{
	if (NULL == pszAccount || NULL == pszPassword)
	{
		printf("Error: Account or Password is NULL\n");

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessSignOn - 1\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
		return FALSE;
	}

	UINT32 ulAccountLen = (UINT32)cAccountLen;
	UINT32 ulPasswordLen = (UINT32)cPasswordLen;
	
	if (0 >= ulAccountLen || 0 >= ulPasswordLen
		|| ulAccountLen > AGPACHARACTER_MAX_ID_STRING
		|| ulPasswordLen > AGPACHARACTER_MAX_PW_STRING
		)
	{
		printf("Error: Account or Password length is wrong\n");

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessSignOn - 2\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

		return FALSE;
	}
		
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pAgsmLoginEncryptInfo)
	{
		printf("Error: Encryption failed.\n");

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessSignOn - 3\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
		
		return FALSE;
	}

	if (false == m_csMD5Encrypt.DecryptString(pAgsmLoginEncryptInfo->m_pszEncryptData, pszAccount, ulAccountLen)
		|| false == m_csMD5Encrypt.DecryptString(pAgsmLoginEncryptInfo->m_pszEncryptData, pszPassword, ulPasswordLen)
		)
	{
		printf("Error: Decrypt failed.\n");

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessSignOn - 4\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

		return FALSE;
	}

	ZeroMemory(pAgsmLoginEncryptInfo->m_szAccountID, sizeof(pAgsmLoginEncryptInfo->m_szAccountID));
	strncpy_s(pAgsmLoginEncryptInfo->m_szAccountID, AGPACHARACTER_MAX_ID_STRING, pszAccount, AGPACHARACTER_MAX_ID_STRING);

	//해당 ID, Password를 DBQuery로 걸어본다.
	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_CHECK_ACCOUNT;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;

	ZeroMemory(pAgsmLoginQueueInfo->m_szAccountID, sizeof(pAgsmLoginQueueInfo->m_szAccountID));
	strncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccount, AGPACHARACTER_MAX_ID_STRING);
	ZeroMemory(pAgsmLoginQueueInfo->m_szPassword, sizeof(pAgsmLoginQueueInfo->m_szPassword));
	strncpy(pAgsmLoginQueueInfo->m_szPassword, pszPassword, AGPACHARACTER_MAX_PW_STRING);

#ifdef _AREA_KOREA_
#ifdef _HANGAME_
	if ( (AP_SERVICE_AREA_KOREA == g_eServiceArea || AP_SERVICE_AREA_JAPAN == g_eServiceArea) && pszExtraForForeign != NULL )
	{
		strncpy_s( pAgsmLoginQueueInfo->m_szHanAuthString,
			sizeof(pAgsmLoginQueueInfo->m_szHanAuthString),
			pszExtraForForeign,
			_TRUNCATE );
	}
#endif

#ifdef _WEBZEN_AUTH_
	if(AP_SERVICE_AREA_KOREA == g_eServiceArea && pszExtraForForeign != NULL)
	{
		strncpy_s( pAgsmLoginQueueInfo->m_szGBAuthString,
		sizeof(pAgsmLoginQueueInfo->m_szGBAuthString),
		pszExtraForForeign,
		_TRUNCATE );
	}
#endif

#elif _AREA_JAPAN_ // 일본 인증 스트링
	if ( (AP_SERVICE_AREA_KOREA == g_eServiceArea || AP_SERVICE_AREA_JAPAN == g_eServiceArea) && pszExtraForForeign != NULL )
	{
		strncpy_s( pAgsmLoginQueueInfo->m_szHanAuthString,
			sizeof(pAgsmLoginQueueInfo->m_szHanAuthString),
			pszExtraForForeign,
			_TRUNCATE );
	}
#elif _AREA_GLOBAL_
	/*if (AP_SERVICE_AREA_GLOBAL == g_eServiceArea && pszExtraForForeign != NULL)
	{
		strncpy_s( pAgsmLoginQueueInfo->m_szGBAuthString,
		sizeof(pAgsmLoginQueueInfo->m_szGBAuthString),
		pszExtraForForeign,
		_TRUNCATE );
	}*/

#endif

	m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);

	return TRUE;
	}

BOOL AgsmLoginClient::ProcessEKey(CHAR *pszEKey, UINT32 ulNID)
{
	if ( !pszEKey || strlen(pszEKey) > AGPMLOGIN_EKEYSIZE ) return FALSE;
	
		
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pAgsmLoginEncryptInfo)
		return FALSE;

	//해당 ekey, nid를 DBQuery로 걸어본다.
	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;

	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_GET_EKEY;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;

	ZeroMemory(pAgsmLoginQueueInfo->m_szAccountID, sizeof(pAgsmLoginQueueInfo->m_szAccountID));
	//strncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccount, AGPACHARACTER_MAX_ID_STRING);
	ZeroMemory(pAgsmLoginQueueInfo->m_szPassword, sizeof(pAgsmLoginQueueInfo->m_szPassword));
	strncpy(pAgsmLoginQueueInfo->m_szPassword, pszEKey, AGPACHARACTER_MAX_PW_STRING);

	m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);

	return TRUE;
}


BOOL AgsmLoginClient::ProcessGetUnion(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, UINT32 ulNID)
	{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName)
		return FALSE;
	
	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		return FALSE;
	
	if (pAgsdServer->m_nNumPlayers > pAgsdServer->m_lMaxUserCount)
		{
		SendLoginResult(AGPMLOGIN_RESULT_GAMESERVER_FULL, NULL, ulNID);
		return FALSE;
		}

	// push to queue
	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_GET_UNION;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);

	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}


BOOL AgsmLoginClient::ProcessGetCharacters(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, UINT32 ulNID)
	{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName)
		return FALSE;
	
	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_GET_CHARACTERS;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	
	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}


BOOL AgsmLoginClient::ProcessSelectCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID, INT32 lMemberBillingNum)
	{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName || NULL == pvCharDetailInfo)
	{
		return FALSE;
	}
	
	CHAR *pszChar = NULL;
	m_pAgpmLogin->m_csPacketCharInfo.GetField( FALSE, pvCharDetailInfo, 0, 
									NULL,				//TID
									&pszChar,			//CharName
									NULL,				//MaxRegisterChars
									NULL,				//Slot Index
									NULL,				//Union Info
									NULL,				//race info
									NULL,
									NULL,
									NULL
									);	
	
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pszChar || NULL == pAgsmLoginEncryptInfo ||
		AGSMLOGIN_FLOW_SELECT_CHARACTER == pAgsmLoginEncryptInfo->m_lFlow)
		{
		DestroyClient(ulNID);
		return FALSE;
		}

	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		{
		SendLoginResult(AGPMLOGIN_RESULT_GAMESERVER_NOT_READY, NULL, ulNID);
		return FALSE;
		}

	if (FALSE == IsValidLoginStep(ulNID, AGPMLOGIN_STEP_SELECT_CHARACTER))
		return FALSE;

	//JK_심야샷다운
	INT32 nLimitUnderAge = m_pAgpmConfig->GetLimitUnderAge();
	if(nLimitUnderAge != 0 && pAgsmLoginEncryptInfo->m_lAge < nLimitUnderAge && m_pAgsmServerManager->CheckMidnightShutDownTime() == 0)
	{
		//뭐라고 메세지라도....
		SendLoginResult(AGPMLOGIN_RESULT_MIDNIGHTSHUTDOWN_UNDERAGE, NULL, ulNID);
		DestroyClient(ulNID);
		return FALSE;
	}

	SetLoginStep(ulNID, AGPMLOGIN_STEP_SELECT_CHARACTER);

	pAgsmLoginEncryptInfo->m_lFlow = AGSMLOGIN_FLOW_SELECT_CHARACTER;

	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_SELECT_CHARACTER;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	pAgsmLoginQueueInfo->m_lCID = lMemberBillingNum;	// 일본: 결제 번호, 안쓰는 필드니 잠깐 훔쳐 쓰자.
	pAgsmLoginQueueInfo->m_ulGameServerNID = pAgsdServer->m_dpnidServer;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szCharacterID, pszChar, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szIPAddress, pAgsdServer->m_szIP, AGPMLOGIN_IPADDRSIZE);

	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}


BOOL AgsmLoginClient::ProcessReturnToSelectWorld(CHAR *pszAccountID, UINT32 ulNID)
	{
	// 이 계정의 캐릭터 정보들을 초기화 시킨다.
	m_pAgsmAccountManager->RemoveAllCharacters(pszAccountID);

	// reset login step
	SetLoginStep(ulNID, AGPMLOGIN_STEP_PASSWORD_CHECK_OK);

	return TRUE;
	}

BOOL AgsmLoginClient::ProcessBaseCharacterOfRace(PVOID pvCharDetailInfo, UINT32 ulNID)
{
	if (NULL == pvCharDetailInfo || 0 == ulNID)
		return FALSE;
	
	INT32	lRace = AURACE_TYPE_NONE;
	m_pAgpmLogin->m_csPacketCharInfo.GetField( FALSE, pvCharDetailInfo, 0, 
								NULL,		//TID
								NULL,		//CharName
								NULL,		//MaxRegisterChars
								NULL,		//Slot Index
								NULL,		//Union Info
								&lRace,		//race info
								NULL,
								NULL,
								NULL
								);

	if (lRace <= AURACE_TYPE_NONE || lRace >= AURACE_TYPE_MAX)
		return FALSE;

	/*
	// race base character가 생성이 안되어 있다면 생성한다.
	if (AP_INVALID_CID == m_lRaceBaseCharacter[lRace][0])
	{
		if (!CreateBaseCharacterOfRace(lRace))
			return FALSE;
	}
	*/

	// 모든 base race character의 정보를 ulNID로 보낸다.
	for (INT32 i = 0; i < AUCHARCLASS_TYPE_MAX * 3; ++i)
	{
		if (m_lRaceBaseCharacter[lRace][i] == AP_INVALID_CID)
			continue;

		AgpdCharacter* pcsCharacter = m_pAgpmCharacter->GetCharacter(m_lRaceBaseCharacter[lRace][i]);
		if(!pcsCharacter)
			continue;

		if (!m_pAgsmCharacter->SendCharacterAllInfo(pcsCharacter, ulNID, FALSE))
			break;
	}	

	return SendBaseCharacterOfRace(lRace, AUCHARCLASS_TYPE_MAX * 3, ulNID);
}

BOOL AgsmLoginClient::ProcessCreateCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID)
	{
	if (NULL == pvCharDetailInfo || NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName)
		return FALSE;
	
	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		return FALSE;

	CHAR *pszChar = NULL;
	INT32 lTID = 0;
	INT32 lHairIndex	= 0;
	INT32 lFaceIndex	= 0;
	m_pAgpmLogin->m_csPacketCharInfo.GetField( FALSE, pvCharDetailInfo, 0, 
									&lTID,				//TID
									&pszChar,			//CharName
									NULL,				//MaxRegisterChars
									NULL,				//Slot Index
									NULL,				//Union Info
									NULL,				//Race Info
									&lHairIndex,
									&lFaceIndex,
									NULL
									);
	
	if (NULL == pszChar || 0 == lTID)
		return FALSE;

	ToLowerExceptFirst( pszChar, static_cast<int>( strlen( pszChar ) ) );

	if (FALSE == CheckCharacterName(pszChar))
		{
		return SendLoginResult(AGPMLOGIN_RESULT_UNMAKABLE_CHAR_NAME, NULL, ulNID);
		}

	AgpdCharacter	*pcsBaseCharacter	= NULL;

	// 로그인 서버에서 생성 가능한 놈인지 체크
	int i = 0;
	for (i = 0; i < AUCHARCLASS_TYPE_MAX * 3; ++i)
		{
		pcsBaseCharacter	= m_pAgpmCharacter->GetCharacter(m_lRaceBaseCharacter[1][i]);
		if (pcsBaseCharacter && pcsBaseCharacter->m_pcsCharacterTemplate->m_lID == lTID)
			break;
		}

	if (i == AUCHARCLASS_TYPE_MAX * 3)
		return FALSE;
	// 로그인 서버에서 생성 가능한 놈인지 체크

	CHAR	szNewName[AGPDCHARACTER_MAX_ID_LENGTH * 2];
	ZeroMemory(szNewName, sizeof(szNewName));

	m_pAgsmCharacter->AddServerNameToCharName(pszChar, pszServerName, szNewName);

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacterLock(szNewName);
	if (pAgpdCharacter)
		{
		AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
		if (m_pAgsmAccountManager->GetAccount(pAgsdCharacter->m_szAccountID))
			{
			SendLoginResult(AGPMLOGIN_RESULT_CHAR_NAME_ALREADY_EXIST, NULL, ulNID);
			return FALSE;
			}
		else
			{
			pAgpdCharacter->m_Mutex.Release();
			m_pAgpmCharacter->RemoveCharacter(szNewName);
			}
		}

	if (FALSE == IsValidLoginStep(ulNID, AGPMLOGIN_STEP_CREATE_CHARACTER))
		return FALSE;

	// create character
	pAgpdCharacter = m_pAgsmCharManager->CreateCharacter(pszAccountID, szNewName, lTID, ulNID, TRUE, pszServerName);
	if (NULL == pAgpdCharacter)
		return FALSE;

	if (pAgsdServer->m_lStartLevel > 1)
	{
		m_pAgsmCharacter->SetCharacterLevel(pAgpdCharacter, pAgsdServer->m_lStartLevel, FALSE);
		m_pAgpmFactors->CopyFactor(&pAgpdCharacter->m_csFactor, (AgpdFactor *) m_pAgpmFactors->GetFactor(&pAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT), TRUE, FALSE, AGPD_FACTORS_TYPE_CHAR_POINT);
	}

	if (pAgsdServer->m_llStartGheld > 0)
	{
		m_pAgpmCharacter->AddMoney(pAgpdCharacter, pAgsdServer->m_llStartGheld);
		m_pAgpmCharacter->UpdateMoney(pAgpdCharacter);
	}

	INT32	lRace	= m_pAgpmFactors->GetRace(&pcsBaseCharacter->m_csFactor);
	INT32	lClass	= m_pAgpmFactors->GetClass(&pcsBaseCharacter->m_csFactor);

	if (m_pAgpmEventCharCustomize->GetCustomizeCase(lRace, lClass, CHARCUSTOMIZE_TYPE_HAIR, lHairIndex) == CHARCUSTOMIZE_CASE_ALL ||
		m_pAgpmEventCharCustomize->GetCustomizeCase(lRace, lClass, CHARCUSTOMIZE_TYPE_HAIR, lHairIndex) == CHARCUSTOMIZE_CHAR_LOGIN)
		pAgpdCharacter->m_lHairIndex	= lHairIndex;
	
	if (m_pAgpmEventCharCustomize->GetCustomizeCase(lRace, lClass, CHARCUSTOMIZE_TYPE_FACE, lFaceIndex) == CHARCUSTOMIZE_CASE_ALL ||
		m_pAgpmEventCharCustomize->GetCustomizeCase(lRace, lClass, CHARCUSTOMIZE_TYPE_FACE, lFaceIndex) == CHARCUSTOMIZE_CHAR_LOGIN)
		pAgpdCharacter->m_lFaceIndex	= lFaceIndex;

	SetLoginStep(ulNID, AGPMLOGIN_STEP_CREATE_CHARACTER);

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	ZeroMemory(pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID));
	_tcsncpy(pAgsdCharacter->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	ZeroMemory(pAgsdCharacter->m_szServerName, sizeof(pAgsdCharacter->m_szServerName));
	_tcsncpy(pAgsdCharacter->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);

	// push to queue
	AgsmLoginQueueInfo	*pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_CREATE_CHARACTER;
	pAgsmLoginQueueInfo->m_lCID = pAgpdCharacter->m_lID;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	pAgsmLoginQueueInfo->m_ulGameServerNID = pAgsdServer->m_dpnidServer;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);

	std::cout << "AGSMLOGIN_OPERATION_CREATE_CHARACTER :" 
		<< '['<< pAgsmLoginQueueInfo->m_szAccountID   << ']' 
		<< '['<< pAgsmLoginQueueInfo->m_lCID   << ']' 
		<< '[' << pAgsmLoginQueueInfo->m_szServerName << ']' 
		<< '['<< pAgsmLoginQueueInfo->m_ulGameServerNID   << ']' 
		<< '[' << pAgsmLoginQueueInfo->m_szDBName <<']' << std::endl;
	//pAgpdCharacter->m_Mutex.Release();

	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID ) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}

//JK_케릭삭제패스워드확인
//BOOL AgsmLoginClient::ProcessRemoveCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID)
BOOL AgsmLoginClient::ProcessRemoveCharacter(CHAR *pszAccountID, CHAR* pszPassword, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID)
	{
	if (NULL == pszAccountID || NULL == pszServerName  || NULL == pszDBName || NULL == pvCharDetailInfo)
		return FALSE;

	CHAR *pszChar = NULL;

	m_pAgpmLogin->m_csPacketCharInfo.GetField( FALSE, pvCharDetailInfo, 0, 
									NULL,				//TID
									&pszChar,			//CharName
									NULL,				//MaxRegisterChars
									NULL,				//Slot Index
									NULL,				//Union Info
									NULL,				//Race Info
									NULL,
									NULL,
									NULL
									);

	if (NULL == pszChar)
		return FALSE;

	if (FALSE == IsValidLoginStep(ulNID, AGPMLOGIN_STEP_REMOVE_CHARACTER))
		return FALSE;

	//m_pAgsmAccountManager->SetLoginStep(pszAccountID, AGPMLOGIN_STEP_REMOVE_CHARACTER);
	SetLoginStep( ulNID, AGPMLOGIN_STEP_REMOVE_CHARACTER);

	// push to queue
	AgsmLoginQueueInfo	*pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_REMOVE_CHARACTER;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szCharacterID, pszChar, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szPassword, pszPassword ? pszPassword : _T(""), AGPACHARACTER_MAX_PW_STRING); //JK_케릭삭제패스워드확인


	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}


BOOL AgsmLoginClient::ProcessRenameCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID)
	{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName || NULL == pvCharDetailInfo)
		return FALSE;
	
	CHAR *pszChar = NULL;
	CHAR *pszNewChar = NULL;
	m_pAgpmLogin->m_csPacketCharInfo.GetField(FALSE, pvCharDetailInfo, 0,
								NULL,				// TID
								&pszChar,			// character id
								NULL,				// MaxRegisterChars
								NULL,				// Slot Index
								NULL,				// Union Info
								NULL,				// race info
								NULL,
								NULL,
								&pszNewChar
								);	

	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pszChar
		|| NULL == pAgsmLoginEncryptInfo
		)
		return FALSE;

	if (FALSE == CheckCharacterName(pszNewChar))
		{
		return SendLoginResult(AGPMLOGIN_RESULT_UNMAKABLE_CHAR_NAME, NULL, ulNID);
		}
	
	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		{
		DestroyClient(ulNID);
		return FALSE;
		}

	if (!IsValidLoginStep(ulNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER))
		return FALSE;

	SetLoginStep(ulNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);

	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_RENAME_CHARACTER;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	pAgsmLoginQueueInfo->m_ulGameServerNID = pAgsdServer->m_dpnidServer;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szCharacterID, pszChar, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szIPAddress, pAgsdServer->m_szIP, AGPMLOGIN_IPADDRSIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szNewCharacterID, pszNewChar, AGPACHARACTER_MAX_ID_STRING);

	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}


BOOL AgsmLoginClient::ProcessCharacter4Compensation(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID, BOOL bSelect)
	{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName || NULL == pvCharDetailInfo)
		return FALSE;
	
	CHAR *pszChar = NULL;
	m_pAgpmLogin->m_csPacketCharInfo.GetField(FALSE, pvCharDetailInfo, 0,
								NULL,				// TID
								&pszChar,			// character id
								NULL,				// MaxRegisterChars
								NULL,				// Slot Index
								NULL,				// Union Info
								NULL,				// race info
								NULL,
								NULL,
								NULL
								);	

	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pszChar
		|| NULL == pAgsmLoginEncryptInfo
		)
		return FALSE;

	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		{
		DestroyClient(ulNID);
		return FALSE;
		}

	if (!IsValidLoginStep(ulNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER))
		return FALSE;
	
	if (!bSelect)
		ZeroMemory(pAgsmLoginEncryptInfo->m_szChar4Compen, sizeof(pAgsmLoginEncryptInfo->m_szChar4Compen));
	else
		{
		if (0 != pAgsmLoginEncryptInfo->m_lCompenID)
			_tcscpy(pAgsmLoginEncryptInfo->m_szChar4Compen, pszChar);
		}

	return TRUE;
	}




//	Callbacks
//=================================================
//
BOOL AgsmLoginClient::CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (NULL == pClass)
		return FALSE;
	
	AgsmLoginClient *pThis = (AgsmLoginClient *) pClass;
	return pThis->OnDisconnect(0, PtrToUint(pCustData));
	}


BOOL AgsmLoginClient::OnDisconnect(INT32 lCID, UINT32 ulNID)
	{
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pAgsmLoginEncryptInfo)
		return FALSE;
	
	if (NULL != pAgsmLoginEncryptInfo->m_szAccountID
		|| _T('\0') != pAgsmLoginEncryptInfo->m_szAccountID[0])
		{
		// reset login status
		AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
		pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_RESET_LOGINSATUS_BY_DISCONNECT_FROM_LOGINSERVER;
		pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
		strncpy(pAgsmLoginQueueInfo->m_szAccountID, pAgsmLoginEncryptInfo->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
	
		m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
		
		/*
		if (m_pAgsmGK->GetLKServer() &&
			pAgsmLoginEncryptInfo->m_bIsPCBangIP)
			{
			PlayerData	*pPlayerData	= GetPlayerDataForID(ulNID);
			if (pPlayerData && pPlayerData->lDisconnectReason != DISCONNNECT_REASON_CLIENT_NORMAL)
				{
				// LK Server 로 Logout 메시지를 날린다.
				// (pc방 이벤트를 위한 거다. 이벤트가 끝나면 이코드는 뺀다.)
				m_pAgsmGK->SendPacketLogout(pAgsmLoginQueueInfo->m_szAccountID, ulNID);
				}
			}
		*/
		}

	// destroy AgsmLoginEncryptInfo
	delete pAgsmLoginEncryptInfo;
	m_csCertificatedNID.RemoveObject(ulNID);

	return TRUE;
	}


BOOL AgsmLoginClient::CBReceiveNewCID(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (NULL == pData || NULL == pClass || NULL == pCustData)
		return FALSE;

	AgsmLoginClient	*pThis			= (AgsmLoginClient *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *) pData;
	PVOID			*ppvBuffer		= (PVOID *) pCustData;
	INT32			lNewCID			= PtrToInt(ppvBuffer[0]);
	UINT32			ulNID			= PtrToUint(ppvBuffer[1]);

	pThis->m_pAgsmCharacter->ResetWaitOperation(pAgpdCharacter, AGSMCHARACTER_WAIT_OPERATION_REQUEST_NEW_CID);
	pThis->m_pAgsmCharacter->SetWaitOperation(pAgpdCharacter, AGSMCHARACTER_WAIT_OPERATION_SEND_CHAR_INFO);

	AgsdCharacter	*pAgsdCharacter = pThis->m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	AgsdServer		*pAgsdServer = pThis->m_pAgsmServerManager->GetServer(pAgsdCharacter->m_ulServerID);

	if (!pAgsdServer)
		return FALSE;

	INT32	lOldCID = pAgpdCharacter->m_lID;
	pAgpdCharacter->m_lID = lNewCID;
	pAgsdCharacter->m_lOldCID = lOldCID;

	//성공적으로 캐릭터를 읽었으니~ 서버로 로그인할 캐릭터를 보내준다.
	pThis->m_pAgsmCharacter->SendCharacterAllInfo(pAgpdCharacter, pAgsdServer->m_dpnidServer,
													TRUE, FALSE, FALSE, lOldCID);

	return TRUE;
	}


BOOL AgsmLoginClient::CBCompleteSendCharacterInfo(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (NULL == pClass || NULL == pData)
		return FALSE;

	AgsmLoginClient *pThis			= (AgsmLoginClient *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	AgsdCharacter	*pAgsdCharacter = pThis->m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	AgsdServer		*pAgsdServer = pThis->m_pAgsmServerManager->GetServer(pAgsdCharacter->m_ulServerID);
	if (NULL == pAgsdCharacter || NULL == pAgsdServer)
		return FALSE;

	pThis->SendEnterGameEnd(pAgsdCharacter->m_szAccountID,
							pAgpdCharacter->m_lID,
							pAgpdCharacter->m_lTID1,
							pThis->m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID),
							pAgsdServer->m_szIP,
							pAgsdCharacter->m_dpnidCharacter
							);

	// push to queue
	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;

	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_SET_LOGINSATUS_IN_GAMESERVER;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szCharacterID, pAgpdCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);	
	//_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pAgsdServer->m_szGroup, AGPACHARACTER_MAX_ID_STRING);
	pAgsmLoginQueueInfo->m_lCID = pAgpdCharacter->m_lID;
	pAgsmLoginQueueInfo->m_lTID = pAgpdCharacter->m_lTID1;
	pAgsmLoginQueueInfo->m_ulCharNID = pAgsdCharacter->m_dpnidCharacter;
	pAgsmLoginQueueInfo->m_ulGameServerNID = pAgsdServer->m_dpnidServer;

	pThis->m_pAgsmLoginServer->CheckAndPushToQueue((PVOID)pAgsmLoginQueueInfo, __FILE__, __LINE__);

	return TRUE;
	}




//	Packet send
//===========================================
//
BOOL AgsmLoginClient::SendEncryptCode(CHAR *pszEncryptCode, UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_ENCRYPT_CODE;
	INT16	nPacketLength = 0;

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													pszEncryptCode,		// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,				// gamestring
													NULL,				// challenge number for ekey
													NULL,				// isLimited
													NULL				// isProtected
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendInvalidClientVersion(UINT32 ulNID)
	{
	if (0 == ulNID)
		return FALSE;

	INT8	cOperation		= AGPMLOGIN_INVALID_CLIENT_VERSION;
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}

BOOL AgsmLoginClient::SendSignOnSuccess(UINT32 ulNID, CHAR* pszAccountID, int isLimited, int isProtected)	//SendLoginSucceeded(UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_SIGN_ON;
	INT16	nPacketLength;
	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													pszAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,				// gamestring for Japan
													NULL,				// challenge number for ekey(china)
													&isLimited,			// for china
													&isProtected		// for china
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendUnionInfo(INT32 lUnionType, UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_UNION_INFO;
	INT16	nPacketLength;
	PVOID	pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														NULL,			//CharName
														NULL,			//MaxRegisterChars
														NULL,			//Slot Index
														&lUnionType,	//Union Info
														NULL,			//race info
														NULL,
														NULL,
														NULL
														);

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendCharacterName(CHAR *pszCharName, INT32 lMaxChars, INT32 lIndex, UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_CHARACTER_NAME;
	INT16	nPacketLength;
	PVOID	pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														pszCharName,	//CharName
														&lMaxChars,		//MaxRegisterChars
														&lIndex,		//Slot Index
														NULL,			//Union Info
														NULL,			//race info
														NULL,
														NULL,
														NULL
														);

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendCharacterNameFinish(CHAR *pszAccountID, UINT32 ulNID)
	{
	if (NULL == pszAccountID)
		return FALSE;
	
	INT8	cOperation = AGPMLOGIN_CHARACTER_NAME_FINISH;
	INT16	nPacketLength;
	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													pszAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		_tprintf(_T("Send Character Name Finished\n"));
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendCharacterInfoFinish(UINT32 lCID, CHAR *pszAccountID, UINT32 ulNID)
	{
	if (NULL == pszAccountID)
		return FALSE;
		
	INT8	cOperation = AGPMLOGIN_CHARACTER_INFO_FINISH;
	INT16	nPacketLength;
	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													pszAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													&lCID,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		_tprintf(_T("Send Character Info Finished\n"));
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendBaseCharacterOfRace(INT8 cRace, INT32 lNumChar, UINT32 ulNID)
	{
	if (0 == ulNID)
		return FALSE;

	INT8 cOperation = AGPMLOGIN_RACE_BASE;
	INT16 nPacketLength;
	INT32 lRace = (INT32) cRace;
	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														NULL,			//CharName
														&lNumChar,		//MaxRegisterChars
														NULL,			//Slot Index
														NULL,			//UnionInfo
														&lRace,			//race info
														NULL,
														NULL,
														NULL
														);

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	if (pvDetailCharInfo)
		m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendNewCharacterName(CHAR *pszCharName, INT32 lCharCount, INT32 lSlotIndex, UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_NEW_CHARACTER_NAME;
	INT16	nPacketLength;
	PVOID	pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														pszCharName,	//CharName
														&lCharCount,	//MaxRegisterChars
														&lSlotIndex,	//Slot Index
														NULL,			//Union Info
														NULL,			//race info
														NULL,
														NULL,
														NULL
														);

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendNewCharacterInfoFinish(INT32 lCID, UINT32 ulNID)
	{
	if (AP_INVALID_CID == lCID)
		{
		DestroyClient(ulNID);
		return FALSE;
		}

	INT8	cOperation = AGPMLOGIN_NEW_CHARACTER_INFO_FINISH;
	INT16	nPacketLength;
	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													&lCID,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendCharacterRemoved(CHAR *pszAccountID, CHAR *pszCharName, UINT32 ulNID)
	{
	if (NULL == pszAccountID)
		return FALSE;
	
	INT8 cOperation = AGPMLOGIN_REMOVE_CHARACTER;
	INT16 nPacketLength;
	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,
														pszCharName,	//CharName
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL
														);

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													pszAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		_tprintf(_T("Send Character Removed\n"));
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}

BOOL AgsmLoginClient::SendEKeyChallengeNum(const char* pszChallengeNum, UINT32 ulNID)
{
	INT cOperation = AGPMLOGIN_EKEY;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													NULL,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,			// lResult
													NULL,				// version info
													NULL,				// ekey
													pszChallengeNum,	// challenge number for ekey
													NULL,				// isLimited
													NULL				// isProtected
													);
	BOOL bResult = FALSE;
	if (pvPacket)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
	}

	return bResult;
}

BOOL AgsmLoginClient::SendLoginResult(INT32 lResult, CHAR *pszCharName, UINT32 ulNID)
	{
	INT8 cOperation = AGPMLOGIN_LOGIN_RESULT;
	INT16 nPacketLength;
	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														pszCharName,	//CharName
														NULL,			//MaxRegisterChars
														NULL,			//Slot Index
														NULL,			//UnionInfo
														NULL,			//race info
														NULL,
														NULL,
														NULL
														);

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													&lResult,			// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);
	if (pvDetailCharInfo)
		m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);
	
	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendEnterGameEnd(CHAR *pszAccountID, INT32 lCID, INT32 lTID, CHAR *pszCharName, CHAR *pszIPAddress, UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_ENTER_GAME;
	INT16	nPacketLength;
	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														&lTID,			//TID
														pszCharName,	//CharName
														NULL,			//MaxRegisterChars
														NULL,			//Slot Index
														NULL,			//UnionInfo
														NULL,			// race info
														NULL,
														NULL,
														NULL
														);

	PVOID pvDetailServerInfo = m_pAgpmLogin->m_csPacketServerInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													pszIPAddress		// IPAddress
													);

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													&lCID,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													pvDetailServerInfo, // pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);
	m_pAgpmLogin->m_csPacketServerInfo.FreePacket(pvDetailServerInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		printf("SendLoginComplete\n");
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendCompensationExist(CHAR *pszAccount, INT32 lCompenID, INT32 lCompenType, CHAR *pszCharID, CHAR *pszItemDetails, CHAR *pszDescription, UINT32 ulNID)
	{
	if (NULL == pszAccount)
		return FALSE;
	
	INT16 nDescriptionLength = pszDescription ? (INT16) strlen(pszDescription) : 0;
	
	INT8 cOperation = AGPMLOGIN_COMPENSATION_INFO;
	INT16 nPacketLength;
	PVOID pvCompenInfo = m_pAgpmLogin->m_csPacketCompenInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														&lCompenID,
														&lCompenType,
														pszCharID,
														pszItemDetails,
														pszDescription,
														&nDescriptionLength
														);

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													pszAccount,			// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													pvCompenInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCompenInfo.FreePacket(pvCompenInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		_tprintf(_T("Send Compensation Info\n"));
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}




//	ETC
//================================================
//
BOOL AgsmLoginClient::AddInvalidPasswordLog(UINT32 ulNID)
	{
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL != pAgsmLoginEncryptInfo)
		{
		pAgsmLoginEncryptInfo->m_lInvalidPasswordCount++;
		if (pAgsmLoginEncryptInfo->m_lInvalidPasswordCount < 3)
			return TRUE;
		}
	
	return FALSE;
	}


BOOL AgsmLoginClient::CheckCharacterName(CHAR *pszCharName)
	{
	const int tempSize = 128;
	std::vector<CHAR> tempName( tempSize );
	ConvertToLower( pszCharName, &tempName[0], tempSize );
	
	INT32 lLen = (INT32)_tcslen(&tempName[0]);
	if (lLen < 4 || lLen > 16)
		return FALSE;

	if (FALSE == this->m_pAgpmCharacter->CheckCharName( &tempName[0] ))
		return FALSE;

	// 욕설인지 확인한다. 2005.07.01. steeple
	if (FALSE == m_pAgpmCharacter->CheckFilterText(&tempName[0]))
		return FALSE;

	// 금지단어가 문자열에 포함되어 있는지 확인한다.
	if (TRUE == SearchContainBanWord(&tempName[0]))
		return FALSE;

	//캐릭터 템플릿에 있는 이름인지 확인한다.
	AgpdCharacterTemplate *pcsTemplate = m_pAgpmCharacter->GetCharacterTemplate( pszCharName );
	if (pcsTemplate)
		return FALSE;

	// 캐릭 생성을 위해 만들어 놓은 캐릭터들과 이름이 같은지 본다.
	for (int i = 0; i < AUCHARCLASS_TYPE_MAX * 3; ++i)
		{
		if (g_pszCharName[i] && strcmp(g_pszCharName[i], pszCharName) == 0)
			return FALSE;
		}

	return TRUE;
	}


BOOL AgsmLoginClient::SearchContainBanWord(CHAR* pszCharName)
{
	string szTemp(pszCharName);

	for ( vector<string>::iterator iter = m_vstrBanWord.begin(); iter != m_vstrBanWord.end(); iter++)
	{
		if (string::npos == szTemp.find( (*iter).c_str() ) )
			continue;

		return TRUE;
	}

	return FALSE;
}


BOOL AgsmLoginClient::CreateBaseCharacterOfRace(INT8 cRace)
	{
	if (cRace <= AURACE_TYPE_NONE || cRace >= AURACE_TYPE_MAX)
		return FALSE;

	for (INT32 i = 0; i < AUCHARCLASS_TYPE_MAX * 3; ++i)
		{
		if (g_lCharTID[i] == AP_INVALID_CID)
			continue;

		/*
		if (pcsCharacterTemplate->m_lID != 1 &&
			pcsCharacterTemplate->m_lID != 5)
			continue;
		*/

		AgpdCharacter *pcsCharacter = m_pAgsmCharManager->CreateCharacter(NULL, g_pszCharName[i], g_lCharTID[i], 0, FALSE);
		if (pcsCharacter)
			{
			// 여기서 클래스별 포지션을 세팅한다.
			pcsCharacter->m_stPos	= m_RaceBaseCharacterPos[cRace][i].stPos;
			pcsCharacter->m_fTurnX	= m_RaceBaseCharacterPos[cRace][i].fDegreeX;
			pcsCharacter->m_fTurnY	= m_RaceBaseCharacterPos[cRace][i].fDegreeY;

			//로그인에서 그냥 보이기만 하면 되기때문에 벨리데이션 체크는 안한다.
			if (!m_pAgsmCharManager->EnterGameWorld(pcsCharacter, FALSE))
				{
				m_pAgpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
				return FALSE;
				}
			}
		else
			{
			pcsCharacter = m_pAgpmCharacter->GetCharacter(g_pszCharName[i]);
			if (!pcsCharacter)
				return FALSE;
			}

		m_pAgsmCharacter->ReCalcCharacterFactors(pcsCharacter, FALSE);
		m_lRaceBaseCharacter[cRace][i] = pcsCharacter->m_lID;
		}

	return TRUE;
	}


BOOL AgsmLoginClient::IsValidLoginStep(CHAR *pszAccountID, AgpdLoginStep eLoginStep)
	{
	AgpdLoginStep eCurrentStep = m_pAgsmAccountManager->GetLoginStep(pszAccountID);

	if (eCurrentStep <= AGPMLOGIN_STEP_NONE ||
		eCurrentStep >= AGPMLOGIN_STEP_UNKNOWN)
		return FALSE;

	switch (eLoginStep)
		{
		case AGPMLOGIN_STEP_CREATE_CHARACTER:
			{
			if ((eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				&& (eCurrentStep != AGPMLOGIN_STEP_PASSWORD_CHECK_OK)
				)
				return FALSE;
			}
			break;

		case AGPMLOGIN_STEP_SELECT_CHARACTER:
			{
			if (eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				return FALSE;
			}
			break;

		case AGPMLOGIN_STEP_REMOVE_CHARACTER:
			{
			if (eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				return FALSE;
			}
			break;
		}

	return TRUE;
	}


BOOL AgsmLoginClient::IsValidLoginStep( UINT32 ulNID, AgpdLoginStep eLoginStep)
	{
	AgpdLoginStep eCurrentStep = GetLoginStep(ulNID);

	if (eCurrentStep <= AGPMLOGIN_STEP_NONE ||
		eCurrentStep >= AGPMLOGIN_STEP_UNKNOWN)
		return FALSE;

	switch (eLoginStep)
		{
		case AGPMLOGIN_STEP_CREATE_CHARACTER:
			{
			if ((eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				&& (eCurrentStep != AGPMLOGIN_STEP_PASSWORD_CHECK_OK)
				)
				return FALSE;
			}
			break;

		case AGPMLOGIN_STEP_SELECT_CHARACTER:
			{
			if (eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				return FALSE;
			}
			break;

		case AGPMLOGIN_STEP_REMOVE_CHARACTER:
			{
			if (eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				return FALSE;
			}
			break;
		}

	return TRUE;
	}


BOOL AgsmLoginClient::SetAccountAge(UINT32 ulNID, CHAR* szAccountID, INT32 lAge)
{
	if(!szAccountID)
		return FALSE;

	AgsmLoginEncryptInfo** ppAgsmLoginEncryptInfo = (AgsmLoginEncryptInfo**)m_csCertificatedNID.GetObject(ulNID);
	if(ppAgsmLoginEncryptInfo && *ppAgsmLoginEncryptInfo)
	{
		if(_tcscmp((*ppAgsmLoginEncryptInfo)->m_szAccountID, szAccountID) == 0)
		{
			(*ppAgsmLoginEncryptInfo)->m_lAge = lAge;
		}
	}

	return TRUE;
}

INT32 AgsmLoginClient::GetAccountAge(UINT32 ulNID, CHAR* szAccountID)
{
	if(!szAccountID)
		return FALSE;

	AgsmLoginEncryptInfo** ppAgsmLoginEncryptInfo = (AgsmLoginEncryptInfo**)m_csCertificatedNID.GetObject(ulNID);
	if(ppAgsmLoginEncryptInfo && *ppAgsmLoginEncryptInfo)
	{
		if(_tcscmp((*ppAgsmLoginEncryptInfo)->m_szAccountID, szAccountID) == 0)
		{
			return (*ppAgsmLoginEncryptInfo)->m_lAge;
		}
	}

	return 0;
}



//	Max user
//============================================
//
INT32 AgsmLoginClient::GetMaxProcessUserCount()
	{
	return m_lMaxProcessUserCount;
	}


BOOL AgsmLoginClient::SetMaxProcessUserCount(INT32 lMaxProcessUserCount)
	{
	if (lMaxProcessUserCount < 0)
		return FALSE;

	m_lMaxProcessUserCount	= lMaxProcessUserCount;

	return TRUE;
	}


INT32 AgsmLoginClient::GetMaxCreateProcessUserCount()
	{
	return m_lMaxProcessUserCount;
	}


BOOL AgsmLoginClient::SetMaxCreateProcessUserCount(INT32 lMaxCreateProcessUserCount)
	{
	if (lMaxCreateProcessUserCount < 0)
		return FALSE;

	m_lMaxCreateProcessUserCount	= lMaxCreateProcessUserCount;

	return TRUE;
	}




//	Stream
//==========================================
//
BOOL AgsmLoginClient::LoadCharNameFromExcel(CHAR *pszFile)
	{
	AuExcelTxtLib		csAuExcelTxtLib;

	//Excel파일에서 갯수를 얻고.
	if (FALSE == csAuExcelTxtLib.OpenExcelFile( pszFile, TRUE ))
	{
		_tprintf(_T("%s Excel파일을 읽지 못했습니다.\n"), pszFile);
		return FALSE;
	}

	for (INT32 iRow = 0; iRow < csAuExcelTxtLib.GetRow(); iRow++ )
	{
		for (INT32 iColumn = 0; iColumn < csAuExcelTxtLib.GetColumn(); iColumn++ )
		{
			CHAR* szBuffer = (CHAR*)csAuExcelTxtLib.GetData( iColumn, iRow );
			if(!szBuffer)
				continue;

			const string szData = szBuffer;
			m_vstrBanWord.push_back(szData);
		}
	}

	csAuExcelTxtLib.CloseFile();

	return TRUE;
	}


BOOL AgsmLoginClient::StreamReadBaseCharPos(CHAR *pszFile)
	{
	if (NULL == pszFile || _T('\0') == *pszFile)
		return FALSE;

	ApModuleStream			csStream;
	UINT16					nNumKeys;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	if (!csStream.Open(pszFile))
		return FALSE;

	nNumKeys = csStream.GetNumSections();

	for (INT32 i = 0; i < nNumKeys; i++)
		{
		csStream.ReadSectionName(i);

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGSMLOGIN_DATA_TYPE_BASE_CHAR_POS, NULL, this))
			return FALSE;
		}

	return TRUE;
	}


BOOL AgsmLoginClient::BaseCharacterPosReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
	{
	if (!pClass || !pStream)
		return FALSE;

	AgsmLoginClient	*pThis	= (AgsmLoginClient *)	pClass;

	const CHAR		*pszValueName;
	CHAR			szValue[128];
	ZeroMemory		(szValue, sizeof(szValue));

	if (!pStream->ReadNextValue())
		return FALSE;

	pszValueName = pStream->GetValueName();
	if (0 != _tcscmp(pszValueName, AGSMLOGIN_INI_RACE_NAME))
		return FALSE;

	pStream->GetValue(szValue, AGPACHARACTERT_MAX_TEMPLATE_NAME);

	INT32	lRace = pThis->m_pAgpmFactors->FindCharacterRaceIndex(szValue);
	if (lRace <= AURACE_TYPE_NONE || lRace >= AURACE_TYPE_MAX)
		return FALSE;

	INT32 i = 0;
	for (i = 0; i < AUCHARCLASS_TYPE_MAX * 3; ++i)
		{
		if (pThis->m_RaceBaseCharacterPos[lRace][i].stPos.x == 0 &&
			pThis->m_RaceBaseCharacterPos[lRace][i].fDegreeX == 0)
			break;
		}

	if (i == AUCHARCLASS_TYPE_MAX * 3)
		return FALSE;

	while (pStream->ReadNextValue())
		{
		pszValueName = pStream->GetValueName();

		if (0 == _tcscmp(pszValueName, AGSMLOGIN_INI_POSITION))
			{
			pStream->GetValue(&pThis->m_RaceBaseCharacterPos[lRace][i].stPos);
			}
		else if (0 == _tcscmp(pszValueName, AGSMLOGIN_INI_DEGREEX))
			{
			pStream->GetValue(&pThis->m_RaceBaseCharacterPos[lRace][i].fDegreeX);
			}
		else if (0 == _tcscmp(pszValueName, AGSMLOGIN_INI_DEGREEY))
			{
			pStream->GetValue(&pThis->m_RaceBaseCharacterPos[lRace][i].fDegreeY);
			}
		}

	return TRUE;
	}


BOOL AgsmLoginClient::IsReturnToLogin(UINT32 ulNID)
{
	if(!ulNID)
		return FALSE;

	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pAgsmLoginEncryptInfo)
		return FALSE;

	return pAgsmLoginEncryptInfo->m_bReturnToLogin;
}