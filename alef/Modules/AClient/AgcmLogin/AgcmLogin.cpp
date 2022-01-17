#include "AgcmLogin.h"
#include "ApMemoryTracker.h"
#include "AgcmRegistryManager.h"

#ifndef USE_MFC
#ifdef _AREA_KOREA_
	#include "CWebzenAuth.h"
#endif
#endif

#ifdef _AREA_JAPAN_
#include "AuJapaneseClientAuth.h"
#include "AuGameInfoJpn.h"
#endif
AgcmLogin::AgcmLogin()
{
	EnableIdle(TRUE);

	SetModuleName(_T("AgcmLogin"));
	SetPacketType(AGPMLOGIN_PACKET_TYPE);

	ZeroMemory(m_szEncryptCode, sizeof(m_szEncryptCode));
	m_lTotalCharacter	= 0;
	m_pMainWindow		= NULL;
	m_lLoginMode		= AGCMLOGIN_MODE_PRE_LOGIN;
	m_nNID				= 0;
	ZeroMemory(m_szIPAddress, sizeof(m_szIPAddress));
	ZeroMemory(m_szAccount, sizeof(m_szAccount));
	ZeroMemory(m_szPassword, sizeof(m_szPassword));
	ZeroMemory(m_szConfirmPassword, sizeof(m_szConfirmPassword));
	m_lTID				= 0;
	m_hLoginDlg			= NULL;
	m_bIsConnectedLoginServer	= FALSE;

	ZeroMemory(m_szWorldName, sizeof(m_szWorldName));

	m_bCustomLoginInfo	= FALSE;
	m_lCharacterSelected= 0;

	m_lIsLimited		= 0;
	m_lIsProtected		= 0;
}

AgcmLogin::~AgcmLogin()
{
}

BOOL AgcmLogin::OnAddModule()
{
	m_pAgpmLogin			= (AgpmLogin *) GetModule(_T("AgpmLogin"));
	m_pAgcmCharacter		= (AgcmCharacter *) GetModule(_T("AgcmCharacter"));
	m_pAgcmConnectManager	= (AgcmConnectManager *) GetModule(_T("AgcmConnectManager"));
	m_pAgpmCharacter		= (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmResourceInfo		= (AgpmResourceInfo *) GetModule(_T("AgpmResourceInfo"));

	if (!m_pAgpmLogin || !m_pAgcmCharacter || !m_pAgcmConnectManager || !m_pAgpmCharacter)
		return FALSE;

	return TRUE;
}

BOOL AgcmLogin::OnIdle(UINT32 ulClockCount)
{
	if( m_cRequestConnect.Update( ulClockCount ) )
	{
		m_cRequestConnect.Stop();

		EnumCallback(AGCMLOGIN_CB_CONNECT, NULL, NULL);
	}

	return TRUE;
}


BOOL AgcmLogin::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	BOOL	bResult = FALSE;
	INT8	nOperation = 0;
	TCHAR	*pszEncryptCode = NULL;
	TCHAR	*pszAccount = NULL;
	TCHAR	*pszPassword = NULL;
	INT32	lCID = AP_INVALID_CID;
	PVOID	pvCharDetailInfo = NULL;
	PVOID	pvDetailServerInfo = NULL;
	INT32	lResult = 0;
	PVOID	pvPacketVersionInfo = NULL;
	CHAR	*pszChallengeNum = 0;
	INT32	lIsLimited = 0;
	INT32	lIsProtected = 0;

	m_nNID	= (INT16) ulNID;

	m_pAgpmLogin->m_csPacket.GetField(TRUE, pvPacket, nSize,
		&nOperation,
		&pszEncryptCode,
		&pszAccount,
		NULL,					// account length
		&pszPassword,
		NULL,					// password length
		&lCID,
		NULL,					// world name
		&pvCharDetailInfo,
		&pvDetailServerInfo,
		&lResult,
		&pvPacketVersionInfo,
		NULL,					// ekey
		&pszChallengeNum,		// challenge number for ekey
		&lIsLimited,
		&lIsProtected
		);

	switch (nOperation)
	{
		// Game Login
	case AGPMLOGIN_ENCRYPT_CODE :
		memcpy(m_szEncryptCode, pszEncryptCode, ENCRYPT_STRING_SIZE);
		EnumCallback(AGCMLOGIN_CB_ENCRYPT_CODE, m_szEncryptCode, NULL);
		break;

	case AGPMLOGIN_INVALID_CLIENT_VERSION :
		EnumCallback(AGCMLOGIN_CB_INVALID_CLIENT_VERSION, NULL, NULL);
		break;

	case AGPMLOGIN_SIGN_ON :	// id, pwd check result
		// 중문일경우에 PT account와 digit account 두개가 있다. 실제 디비에 기록은
		// PT account가 저장되므로 digit account를 이용했을경우에는 PT account로
		// 계정이름을 변경해야한다.
		if (pszAccount && strncmp(pszAccount, m_szAccount, AGCMLOGIN_ID_MAX_LENGTH))
		{
			strncpy(m_szAccount, pszAccount, AGCMLOGIN_ID_MAX_LENGTH);
			m_szAccount[AGCMLOGIN_ID_MAX_LENGTH] = 0;
		}

		// 2007.07.06. steeple
		m_lIsLimited = lIsLimited;
		m_lIsProtected = lIsProtected;

		EnumCallback(AGCMLOGIN_CB_SIGN_ON, (PVOID) ulNID, NULL);
		break;

	case AGPMLOGIN_UNION_INFO : //AGPMLOGIN_GET_UNION :
		bResult = OnUnionInfo(pvCharDetailInfo);
		break;

	case AGPMLOGIN_EKEY:
		EnumCallback(AGCMLOGIN_CB_EKEY_ACTIVE, pszChallengeNum, NULL);
		break;

	case AGPMLOGIN_CHARACTER_NAME : //AGPMLOGIN_GET_CHARACTERS :
		bResult = OnCharacterName(pvCharDetailInfo);
		break;

	case AGPMLOGIN_CHARACTER_NAME_FINISH : // AGPMLOGIN_GET_CHARACTERS_FINISH :
		EnumCallback(AGCMLOGIN_CB_CHARACTER_NAME_FINISH, (PVOID) pszAccount, (PVOID) ulNID);
		break;

	case AGPMLOGIN_CHARACTER_INFO_FINISH :	// AGPMLOGIN_SEND_CHARACTER_FINISH
		{
			m_lCharacterSelected = 0;
			PVOID pvBuffer[2];	// 0 : CID, 1 : AccountID
			pvBuffer[0] = (PVOID) lCID;
			pvBuffer[1] = (PVOID) pszAccount;
			EnumCallback(AGCMLOGIN_CB_CHARACTER_INFO_FINISH, (PVOID) pvBuffer, (PVOID) ulNID);
		}
		break;

	case AGPMLOGIN_ENTER_GAME :
		// UI 제어권을 AgcmUIManager로 넘겨준다
		EnumCallback(AGCMLOGIN_CB_ENTER_GAME, NULL, NULL);
		bResult = OnEnterGame(lCID, pvCharDetailInfo, pvDetailServerInfo);
		break;

		// Create Character
	case AGPMLOGIN_RACE_BASE :
		EnumCallback(AGCMLOGIN_CB_RACE_BASE, NULL, NULL);
		break;

	case AGPMLOGIN_NEW_CHARACTER_NAME :	// AGPMLOGIN_ADD_CREATED_CHARACTER
		bResult = OnCharacterName(pvCharDetailInfo, TRUE);
		break;		

	case AGPMLOGIN_NEW_CHARACTER_INFO_FINISH :	// AGPMLOGIN_SEND_CREATE_CHARACTER_FINISH
		EnumCallback(AGCMLOGIN_CB_NEW_CHARACTER_INFO_FINISH, NULL, NULL);
		break;			

		// Remove Character
	case AGPMLOGIN_REMOVE_CHARACTER :
		bResult = OnRemoveCharacter(pvCharDetailInfo);
		break;

	case AGPMLOGIN_COMPENSATION_INFO :
		bResult = OnCompensationInfo(pszAccount, pvCharDetailInfo);
		break;

	case AGPMLOGIN_LOGIN_RESULT :
		bResult = OnLoginResult(pvCharDetailInfo, lResult, ulNID);
		break;

	default :
		break;
	}

	return bResult;
}


//	Login operations
//======================================================
//
INT16 AgcmLogin::ConnectLoginServer()
{
	if (m_bIsConnectedLoginServer)
	{
		EnumCallback( AGCMLOGIN_CB_ENCRYPT_CODE, NULL, NULL );
		return m_nNID;
	}

	// 아직까지 접속할 로그인서버가 설정되어 있지 않으면 레지스트리로부터 읽는다.
	if( strlen( m_szIPAddress ) <= 0 )
	{
		if( !InitRegistry() ) return FALSE;
	}
	
	m_cRequestConnect.Start( GetClockCount() );
	INT16 nNID  = m_pAgcmConnectManager->Connect( m_szIPAddress, ACDP_SERVER_TYPE_LOGINSERVER, this, CBSocketConnect, CBSocketDisConnect, CBSocketError );
	if( nNID >= 0 )
		m_nNID = nNID;

	return nNID;
}


INT16	AgcmLogin::ConnectLoginServer(CONST CHAR* szIpAddress)
{
	ZeroMemory( m_szIPAddress , 64 );
	sprintf_s( m_szIPAddress , 64 , "%s" , szIpAddress );

	return ConnectLoginServer();
}

BOOL AgcmLogin::DisconnectLoginServer()
{
	if (m_pAgcmConnectManager->Disconnect(m_nNID))
	{
		m_cRequestConnect.Stop();

		m_bIsConnectedLoginServer = FALSE;
		m_nNID = 0;
		return TRUE;
	}

	return FALSE;
}


void AgcmLogin::SelectCharacter(INT32 lCID, BOOL &bRename)
{
	long result = InterlockedIncrement( &m_lCharacterSelected );
	if ( result > 1 ) return;

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(lCID);

	if (NULL == pAgpdCharacter)
	{
		InterlockedDecrement( &m_lCharacterSelected );
		return;
	}

	TCHAR szCharID[AGPACHARACTER_MAX_ID_STRING + 1];
	ZeroMemory(szCharID, sizeof(szCharID));
	_tcscpy(szCharID, pAgpdCharacter->m_szID);

	if (m_pAgpmLogin->IsDuplicatedCharacterOfMigration(szCharID))
	{
		bRename = TRUE;
		InterlockedDecrement( &m_lCharacterSelected );
		return;
	}

	// 2. Select Info 보냄 
	if ( FALSE == SendEnterGame(m_szWorldName, m_szAccount, 0, szCharID) )
	{
		BOOL isNormalExit = FALSE;
		EnumCallback( AGCMLOGIN_CB_DISCONNECT, NULL, &isNormalExit );
	}

	InterlockedDecrement( &m_lCharacterSelected );
}


BOOL AgcmLogin::RenameCharacter(INT32 lCID, TCHAR *pszNewCharID)
{
	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(lCID);
	if (NULL == pAgpdCharacter)
		return FALSE;

	TCHAR szCharID[AGPACHARACTER_MAX_ID_STRING + 1];
	ZeroMemory(szCharID, sizeof(szCharID));
	_tcscpy(szCharID, pAgpdCharacter->m_szID);

	if (FALSE == m_pAgpmLogin->IsDuplicatedCharacterOfMigration(szCharID))
		return FALSE;

	return SendRenameCharacter(m_szWorldName, m_szAccount, 0, szCharID, pszNewCharID);
}




//
//======================================================
//
BOOL AgcmLogin::OnUnionInfo(PVOID pvCharDetailInfo)
{
	if (NULL == pvCharDetailInfo)
		return FALSE;

	INT32 lUnion = AUUNION_TYPE_NONE;

	m_pAgpmLogin->m_csPacketCharInfo.GetField(FALSE, pvCharDetailInfo, 0,
		NULL,			//TID
		NULL,			//CharID
		NULL,			//MaxRegisterChars
		NULL,			//Slot Index
		&lUnion,		//Union Info
		NULL,
		NULL,
		NULL,
		NULL
		);

	EnumCallback(AGCMLOGIN_CB_UNION_INFO, (PVOID) &lUnion, NULL);

	return TRUE;
}


BOOL AgcmLogin::OnCharacterName(PVOID pvCharDetailInfo, BOOL bNew)
{
	if (NULL == pvCharDetailInfo)
		return FALSE;

	TCHAR *pszCharID = NULL;
	INT32 lTotalCount = 0;
	INT32 lSlotIndex = 0;

	m_pAgpmLogin->m_csPacketCharInfo.GetField(FALSE, pvCharDetailInfo, 0,
		NULL,					//TID
		&pszCharID,				//CharID
		&lTotalCount,			//Total
		&lSlotIndex,			//Slot Index
		NULL,					//Union Info
		NULL,
		NULL,
		NULL,
		NULL
		);

	m_lTotalCharacter = lTotalCount;
	if (bNew)
		EnumCallback(AGCMLOGIN_CB_NEW_CHARACTER_NAME, (PVOID) &lSlotIndex, pszCharID);
	else
		EnumCallback(AGCMLOGIN_CB_CHARACTER_NAME, (PVOID) &lSlotIndex, pszCharID);

	return TRUE;
}


BOOL AgcmLogin::OnEnterGame(INT32 lCID, PVOID pvCharDetailInfo, PVOID pvDetailServerInfo)
{
	if (NULL == pvCharDetailInfo || NULL == pvDetailServerInfo)
		return FALSE;

	TCHAR *pszCharID = NULL;
	TCHAR *pszIPAddress = NULL;
	INT32 lTID = 0;

	m_pAgpmLogin->m_csPacketCharInfo.GetField(FALSE, pvCharDetailInfo, 0,
		&lTID,				//TID
		&pszCharID,			//CharID
		NULL,				//MaxRegisterChars
		NULL,				//Slot Index
		NULL,				//Union Info
		NULL,
		NULL,
		NULL,
		NULL
		);

	m_pAgpmLogin->m_csPacketServerInfo.GetField(FALSE, pvDetailServerInfo, 0,
		&pszIPAddress
		);

	if (NULL == pszCharID || NULL == pszIPAddress)
		return FALSE;

	TRACE("Login End !!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	return m_pAgcmCharacter->SendEnterGameWorld(pszCharID, pszIPAddress, lCID);

	/*
	//게임 서버와 연결되었으니 로그인 서버와는 이제 더이상 볼일이 없군. 접속 해제!!
	if(bResult)
	{
	//m_pcsAgcmConnectManager->Disconnect(m_pcsAgcmConnectManager->GetLoginServerNID());
	}
	//게임 서버와 연결실패. 로그인 화면에서 에러 메시지를 발생시키고 상황에 따른 대처를 한다.
	else //if(bResult == false)
	{
	}
	*/

}


BOOL AgcmLogin::OnRemoveCharacter(PVOID pvCharDetailInfo)
{
	if (NULL == pvCharDetailInfo)
		return FALSE;

	TCHAR *pszCharID = NULL;

	m_pAgpmLogin->m_csPacketCharInfo.GetField(FALSE, pvCharDetailInfo, 0,
		NULL,			//TID
		&pszCharID,		//CharID
		NULL,			//MaxRegisterChars
		NULL,			//Slot Index
		NULL,			//Union Info
		NULL,
		NULL,
		NULL,
		NULL
		);

	if (NULL == pszCharID)
		return FALSE;

	EnumCallback(AGCMLOGIN_CB_REMOVE_CHARACTER, pszCharID, NULL);

	// remove from map
	m_pAgpmCharacter->RemoveCharacterFromMap(m_pAgpmCharacter->GetCharacter(pszCharID));

	// Public Character Module 에서 삭제 
	return m_pAgpmCharacter->RemoveCharacter(pszCharID);
}


BOOL AgcmLogin::OnCompensationInfo(TCHAR *pszAccount, PVOID pvCharDetailInfo)
{
	if (NULL == pszAccount || NULL == pvCharDetailInfo)
		return FALSE;

	AgcdLoginCompenMaster CompenMaster;
	TCHAR *pszCharID = NULL;
	TCHAR *pszItemDetails = NULL;
	TCHAR *pszDescription = NULL;
	INT16 nDescriptionLength = 0;

	m_pAgpmLogin->m_csPacketCompenInfo.GetField(FALSE, pvCharDetailInfo, 0,
		&CompenMaster.m_lCompenID,
		&CompenMaster.m_eType,
		&pszCharID,
		&pszItemDetails,
		&pszDescription,
		&nDescriptionLength
		);

	_tcsncpy(CompenMaster.m_szCharID, pszCharID ? pszCharID : _T(""), AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(CompenMaster.m_szItemDetails, pszItemDetails ? pszItemDetails : _T(""), 100);
	_tcsncpy(CompenMaster.m_szDescription, pszDescription ? pszDescription : _T(""), nDescriptionLength);

	EnumCallback(AGCMLOGIN_CB_COMPENSATION_INFO, pszAccount, &CompenMaster);

	return TRUE;
}


BOOL AgcmLogin::OnLoginResult(PVOID pvCharDetailInfo, INT32 lResult, INT32 ulNID)
{
	TCHAR *pszCharID = NULL;
	m_pAgpmLogin->m_csPacketCharInfo.GetField(FALSE, pvCharDetailInfo, 0,
		NULL,				//TID
		&pszCharID,			//CharName
		NULL,				//MaxRegisterChars
		NULL,				//Slot Index
		NULL,				//Union Info
		NULL,
		NULL,
		NULL,
		NULL
		);

	return EnumCallback(AGCMLOGIN_CB_LOGIN_RESULT, (PVOID) &lResult, pszCharID);
}




//	Helper
//==============================================
//
BOOL AgcmLogin::InitRegistry()
{
	HKEY hRegKey;
	TCHAR strBuffer[ 256 ] = { 0, };

	switch( g_eServiceArea )
	{
	case AP_SERVICE_AREA_KOREA :	// 아크로드 이관하면 요건 분리되어야겠지..
		{
			_stprintf( strBuffer, AgcmRegistryManager::RegPath() );
			if( RegOpenKeyEx( HKEY_CURRENT_USER, strBuffer, 0, KEY_READ, &hRegKey ) != ERROR_SUCCESS )	return FALSE;
		}
		break;

	case AP_SERVICE_AREA_JAPAN :
	case AP_SERVICE_AREA_CHINA :
		{
			_stprintf( strBuffer, _T( "SOFTWARE\\ArchLord" ) );
			if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, strBuffer, 0, KEY_READ, &hRegKey ) != ERROR_SUCCESS )	return FALSE;
		}
		break;

	case AP_SERVICE_AREA_WESTERN :
		{
			_stprintf( strBuffer, _T( "SOFTWARE\\Webzen\\ArchLord" ) );
			if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, strBuffer, 0, KEY_READ, &hRegKey ) != ERROR_SUCCESS )	return FALSE;
		}
		break;
	};

	TCHAR szTempIP[256];
	INT32 lTempPort;
	DWORD dwType;
	DWORD dwLength;

	// read IP
	dwLength = sizeof(szTempIP);
	if (ERROR_SUCCESS != RegQueryValueEx(hRegKey, _T("LoginServerIP"), 0, &dwType, (LPBYTE) szTempIP, &dwLength))
		return FALSE;

	// read port
	dwLength = sizeof(lTempPort);
	if (ERROR_SUCCESS != RegQueryValueEx(hRegKey, _T("LoginServerPort"), 0, &dwType, (LPBYTE) &lTempPort, &dwLength))
		return FALSE;

	ZeroMemory(m_szIPAddress, sizeof(m_szIPAddress));
	_stprintf(m_szIPAddress, _T("%s:%d"), szTempIP, lTempPort);

	RegCloseKey(hRegKey); 

	return TRUE;
}


VOID AgcmLogin::SetMainWindow(AgcWindow *pcsWindow)
{
	m_pMainWindow = pcsWindow;
}


BOOL AgcmLogin::SetIDPassword(TCHAR *pszAccount, TCHAR *pszPassword)
{
	ZeroMemory(m_szAccount, sizeof(m_szAccount));
	ZeroMemory(m_szPassword, sizeof(m_szPassword));

	_snprintf_s( m_szAccount, AGCMLOGIN_ID_MAX_LENGTH, _TRUNCATE, _tcslwr(pszAccount) );
	_snprintf_s( m_szPassword, AGCMLOGIN_PASSWORD_MAX_LENGTH, _TRUNCATE, pszPassword );

	return TRUE;
}




//	Packet
//==================================================
//
BOOL AgcmLogin::SendGetEncryptCode()
{
	BOOL bResult = FALSE;

	INT8	cOperation = AGPMLOGIN_ENCRYPT_CODE;
	INT16	nPacketLength;

	INT32	lMajorVersion = 0;
	INT32	lMinorVersion = 0;

	if (m_pAgpmResourceInfo)
	{
		lMajorVersion	= m_pAgpmResourceInfo->GetMajorVersion();
		lMinorVersion	= m_pAgpmResourceInfo->GetMinorVersion();
	}

	PVOID pvPacketVersionInfo = m_pAgpmLogin->m_csPacketVersionInfo.MakePacket(FALSE, &nPacketLength, 0,
		&lMajorVersion,
		&lMinorVersion
		);
	if (!pvPacketVersionInfo)
		return bResult;

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&cOperation,		// cOperation
		NULL,				// EncryptCode
		NULL,				// AccountID
		NULL,				// AccountID Length
		NULL,				// AccountPassword
		NULL,				// AccountPassword Length
		NULL,				// lCID
		NULL,				// World Name;
		NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
		NULL,				// pvDetailServerInfo; 서버군 정보.
		NULL,				// lResult
		pvPacketVersionInfo,// version info
		NULL,				// gamestring
		NULL,				// challenge number for ekey
		NULL,				// isLimited
		NULL				// isProtected
		);

	m_pAgpmLogin->m_csPacketVersionInfo.FreePacket(pvPacketVersionInfo);

	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);

		if ( FALSE == bResult )
		{
			BOOL isNormalExit = FALSE;
			EnumCallback( AGCMLOGIN_CB_DISCONNECT, NULL, &isNormalExit );
		}
	}

	return bResult;
}

BOOL AgcmLogin::SendEKey(char *pszEKey, INT16 nNID)
{
	if (0 == pszEKey || strlen(pszEKey) > AGPMLOGIN_EKEYSIZE) return FALSE;

	INT8	cOperation = AGPMLOGIN_EKEY;
	INT16	nPacketLength;
	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&cOperation,		// Operation
		NULL,				// EncryptCode
		NULL, //szTempAccount,		// AccountID
		NULL, //&cAccountLen,		// AccountID Length
		NULL, //szTempPassword,		// AccountPassword
		NULL, //&cPasswordLen,		// Password Length
		NULL,				// lCID
		NULL,				// World Name;
		NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
		NULL,				// pvDetailServerInfo; 서버군 정보.
		NULL,				// lResult
		NULL,				// version info
		pszEKey,			// ekey
		NULL,
		NULL,				// isLimited
		NULL				// isProtected
		);

	BOOL bResult = FALSE;
	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);

		if ( FALSE == bResult )
		{
			BOOL isNormalExit = FALSE;
			EnumCallback( AGCMLOGIN_CB_DISCONNECT, NULL, &isNormalExit );
		}

	}

	return bResult;
}

BOOL AgcmLogin::SendAccount(TCHAR *pszAccount, TCHAR *pszPassword, INT16 nNID)
{
	if (NULL == pszAccount || NULL == pszPassword)
		return FALSE;

	// encrypt
	TCHAR szTempAccount[AGPACHARACTER_MAX_ID_STRING+1];
	TCHAR szTempPassword[AGPACHARACTER_MAX_PW_STRING+1];
	UINT32 ulAccountLen = _tcslen(pszAccount);
	UINT32 ulPasswordLen = _tcslen(pszPassword);

	if (0 >= ulAccountLen || 0 >= ulPasswordLen
		|| ulAccountLen > AGPACHARACTER_MAX_ID_STRING
		|| ulPasswordLen > AGPACHARACTER_MAX_PW_STRING
		)
		return FALSE;

	ZeroMemory(szTempAccount, sizeof(szTempAccount));
	ZeroMemory(szTempPassword, sizeof(szTempPassword));

	_tcscpy(szTempAccount, pszAccount);
	_tcscpy(szTempPassword, pszPassword);

	if (false == m_csMD5Encrypt.EncryptString(m_szEncryptCode, szTempAccount, ulAccountLen))
		return FALSE;

	if (false == m_csMD5Encrypt.EncryptString(m_szEncryptCode, szTempPassword, ulPasswordLen))
		return FALSE;

	INT8 cAccountLen = (INT32) ulAccountLen;
	INT8 cPasswordLen = (INT32) ulPasswordLen;

	INT8	cOperation = AGPMLOGIN_SIGN_ON;
	INT16	nPacketLength;

	const char* gamestring = NULL;

#ifdef _AREA_JAPAN_
	// 일본 서비스 그중에서도 자동 로그인을 사용할때만 gamestring을 사용한다.
	if ( g_jAuth.GetAutoLogin() )
		gamestring = g_jAuth.GetAuthString();
#endif

#if defined( _AREA_KOREA_ ) && !defined( USE_MFC )
	CWebzenAuth* pWebzenAuth = CWebzenAuth::GetInstance();
	if( pWebzenAuth && pWebzenAuth->IsAutoLogin() )
	{
		gamestring = pWebzenAuth->GetGameString();
	}

	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, sizeof( char ) * 256, "-- Send LoginPacket( AGPMLOGIN_SIGN_ON ) : Account = %s, Password = %s, GameString = %s\n", pszAccount, pszPassword, gamestring );
	OutputDebugString( strDebug );

#endif

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
															&cOperation,		// Operation
															NULL,				// EncryptCode
															szTempAccount,		// AccountID
															&cAccountLen,		// AccountID Length
															szTempPassword,		// AccountPassword
															&cPasswordLen,		// Password Length
															NULL,				// lCID
															NULL,				// World Name;
															NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
															NULL,				// pvDetailServerInfo; 서버군 정보.
															NULL,				// lResult
															NULL,				// version info
															gamestring,			// gamestring
															NULL,
															NULL,				// isLimited
															NULL				// isProtected
															);

	BOOL bResult = FALSE;
	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);

		if ( FALSE == bResult )
		{
			BOOL isNormalExit = FALSE;
			EnumCallback( AGCMLOGIN_CB_DISCONNECT, NULL, &isNormalExit );
		}
	}

	return bResult;
}


BOOL AgcmLogin::SendGetUnion(TCHAR *pszWorld, TCHAR *pszAccount)
{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMLOGIN_UNION_INFO;
	INT16	nPacketLength;

	if (NULL == pszWorld)
		pszWorld = m_szWorldName;

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&cOperation,		// cOperation
		NULL,				// EncryptCode
		pszAccount,			// AccountID
		NULL,				// AccountID Len
		NULL,				// AccountPassword
		NULL,				// AccountPassword Len
		NULL,				// lCID
		pszWorld,			// World Name;
		NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
		NULL,				// pvDetailServerInfo; 서버군 정보.
		NULL,				// lResult
		NULL,				// version info
		NULL,
		NULL,
		NULL,				// isLimited
		NULL				// isProtected
		);

	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
	}

	return bResult;
}


BOOL AgcmLogin::SendGetCharacters(TCHAR *pszWorld, TCHAR *pszAccount)
{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMLOGIN_CHARACTER_INFO;
	INT16	nPacketLength;

	if (NULL == pszWorld)
		pszWorld = m_szWorldName;

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&cOperation,		// cOperation
		NULL,				// EncryptCode
		pszAccount,			// AccountID
		NULL,				// AccountID Len
		NULL,				// AccountPassword
		NULL,				// AccountPassword Len
		NULL,				// lCID
		pszWorld,			// World Name;
		NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
		NULL,				// pvDetailServerInfo; 서버군 정보.
		NULL,				// lResult
		NULL,				// version info
		NULL,				// gamestring
		NULL,				// quest ekey
		NULL,				// isLimited
		NULL				// isProtected
		);

	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);

		if ( FALSE == bResult )
		{
			BOOL isNormalExit = FALSE;
			EnumCallback( AGCMLOGIN_CB_DISCONNECT, NULL, &isNormalExit );
		}
	}

	return bResult;
}


BOOL AgcmLogin::SendEnterGame(TCHAR *pszWorld, TCHAR *pszAccount, INT32 lTID, TCHAR *pszCharID)
{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMLOGIN_ENTER_GAME;
	INT16	nPacketLength;

	if (NULL == pszWorld)
		pszWorld = m_szWorldName;

	PVOID	pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
																			&lTID,				//TID
																			pszCharID,			//CharID
																			NULL,				//MaxRegisterChars
																			NULL,				//Slot Index
																			NULL,				//Union Info
																			NULL,
																			NULL,
																			NULL,
																			NULL
																			);
	if( !pvDetailCharInfo)		return bResult;

	INT32 memberBillingNum = 0;
	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket( TRUE, 
														&nPacketLength, 
														AGPMLOGIN_PACKET_TYPE,
														&cOperation,		// cOperation
														NULL,				// EncryptCode
														pszAccount,			// AccountID
														NULL,				// AccountID Len
														NULL,				// AccountPassword
														NULL,				// AccountPassword Len
														&memberBillingNum,	// lCID, 일본의 경우 billingNum으로 사용하자.
														pszWorld,// World Name;
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

	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		if ( FALSE == bResult )
		{
			BOOL isNormalExit = FALSE;
			EnumCallback( AGCMLOGIN_CB_DISCONNECT, NULL, &isNormalExit );
		}
	}

	return bResult;
}


BOOL AgcmLogin::SendReturnToSelectServer(TCHAR *pszAccount)
{
	if (NULL == pszAccount)
		return FALSE;

	INT8	cOperation		= AGPMLOGIN_RETURN_TO_SELECT_WORLD;
	INT16	nPacketLength	= 0;

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&cOperation,
		NULL,
		pszAccount,
		NULL,
		NULL,				// Password
		NULL,				// Password Len
		NULL,				// lCID
		NULL,				// World Name;
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
	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		if ( FALSE == bResult )
		{
			BOOL isNormalExit = FALSE;
			EnumCallback( AGCMLOGIN_CB_DISCONNECT, NULL, &isNormalExit );
		}
	}

	return bResult;
}


BOOL AgcmLogin::SendGetBaseCharacterOfRace(TCHAR *pszAccount, INT32 lRace)
{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMLOGIN_RACE_BASE;
	INT16	nPacketLength;

	PVOID	pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		NULL,			//TID
		NULL,			//CharID
		NULL,			//MaxRegisterChars
		NULL,			//Slot Index
		NULL,			//Union Info
		&lRace,
		NULL,
		NULL,
		NULL
		);
	if (!pvDetailCharInfo)
		return FALSE;

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&cOperation,		// cOperation
		NULL,				// EncryptCode
		pszAccount,			// AccountID
		NULL,				// AccountID Len
		NULL,				// AccountPassword
		NULL,				// AccountPassword Len
		NULL,				// lCID
		NULL,				// World Name;
		pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
		NULL,				// pvDetailServerInfo; 서버군 정보.
		NULL,				// lResult
		NULL,				// version info
		NULL,
		NULL,
		NULL,				// isLimited
		NULL				// isProtected
		);

	m_pAgpmLogin->m_csPacket.FreePacket(pvDetailCharInfo);

	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
	}

	return bResult;
}


BOOL AgcmLogin::SendCreateCharacter(TCHAR *pszAccount, TCHAR *pszWorld, INT32 lTID, TCHAR *pszCharID, INT32 lIndex, INT32 lUnion, INT32 lHairIndex, INT32 lFaceIndex)
{
	if (NULL == pszCharID)
		return FALSE;

	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMLOGIN_NEW_CHARACTER_NAME;
	INT16	nPacketLength;

	if (NULL == pszWorld)
		pszWorld = m_szWorldName;

	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&lTID,			//TID
		pszCharID,		//CharID
		NULL,			//MaxRegisterChars
		&lIndex,		//Slot Index
		&lUnion,			//Union Info
		NULL,
		&lHairIndex,
		&lFaceIndex,
		NULL
		);
	if (NULL == pvDetailCharInfo)
		return FALSE;

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&cOperation,		// cOperation
		NULL,				// EncryptCode
		pszAccount,			// AccountID
		NULL,				// AccountID Len
		NULL,				// AccountPassword
		NULL,				// AccountPassword Len
		NULL,				// lCID
		pszWorld,			// pszWorld;
		pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
		NULL,				// pvDetailServerInfo; 서버군 정보.
		NULL,				// lResult
		NULL,				// version info
		NULL,
		NULL,
		NULL,				// isLimited
		NULL				// isProtected
		);

	m_pAgpmLogin->m_csPacket.FreePacket(pvDetailCharInfo);

	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);

		if ( FALSE == bResult )
		{
			BOOL isNormalExit = FALSE;
			EnumCallback( AGCMLOGIN_CB_DISCONNECT, NULL, &isNormalExit );
		}
	}

	return bResult;
}


BOOL AgcmLogin::SendRemoveCharacter( TCHAR *pszAccount, TCHAR *pszWorld, TCHAR *pszCharID, TCHAR *pszPassword /*= NULL*/ )
{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMLOGIN_REMOVE_CHARACTER;
	INT16	nPacketLength;

	if (NULL == pszWorld)
		pszWorld = m_szWorldName;

	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		NULL,			//TID
		pszCharID,		//CharID
		NULL,			//MaxRegisterChars
		NULL,			//Slot Index
		NULL,			//Union Info
		NULL,
		NULL,
		NULL,
		NULL
		);
	if (NULL == pvDetailCharInfo)
		return FALSE;

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&cOperation,		// cOperation
		NULL,				// EncryptCode
		pszAccount,			// AccountID
		NULL,				// AccountID Len
		pszPassword,		// AccountPassword
		NULL,				// AccountPassword Len
		NULL,				// lCID
		pszWorld,			// World Name;
		pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
		NULL,				// pvDetailServerInfo; 서버군 정보.
		NULL,				// lResult
		NULL,				// version info
		NULL,
		NULL,
		NULL,				// isLimited
		NULL				// isProtected
		);

	m_pAgpmLogin->m_csPacket.FreePacket(pvDetailCharInfo);

	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);

		if ( FALSE == bResult )
		{
			BOOL isNormalExit = FALSE;
			EnumCallback( AGCMLOGIN_CB_DISCONNECT, NULL, &isNormalExit );
		}

	}

	return bResult;
}


BOOL AgcmLogin::SendRenameCharacter(TCHAR *pszWorld, TCHAR *pszAccount, INT32 lTID, TCHAR *pszCharID, TCHAR *pszNewCharID)
{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMLOGIN_RENAME_CHARACTER;
	INT16	nPacketLength;

	if (NULL == pszWorld)
		pszWorld = m_szWorldName;

	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&lTID,				//TID
		pszCharID,			//CharID
		NULL,				//MaxRegisterChars
		NULL,				//Slot Index
		NULL,				//Union Info
		NULL,
		NULL,
		NULL,
		pszNewCharID
		);
	if (NULL == pvDetailCharInfo)
		return FALSE;

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&cOperation,		// cOperation
		NULL,				// EncryptCode
		pszAccount,			// AccountID
		NULL,				// AccountID Len
		NULL,				// AccountPassword
		NULL,				// AccountPassword Len
		NULL,				// lCID
		pszWorld,			// World Name;
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

	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
	}

	return bResult;
}


BOOL AgcmLogin::SendCharacter4Compensation(TCHAR *pszAccount, TCHAR *pszWorld, TCHAR *pszCharID, BOOL bSelect)
{
	BOOL	bResult = FALSE;
	INT8	cOperation = bSelect ? AGPMLOGIN_COMPENSATION_CHARACTER_SELECT : AGPMLOGIN_COMPENSATION_CHARACTER_CANCEL;
	INT16	nPacketLength;

	if (NULL == pszWorld)
		pszWorld = m_szWorldName;

	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		NULL,			//TID
		pszCharID,		//CharID
		NULL,			//MaxRegisterChars
		NULL,			//Slot Index
		NULL,			//Union Info
		NULL,
		NULL,
		NULL,
		NULL
		);
	if (NULL == pvDetailCharInfo)
		return FALSE;

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
		&cOperation,		// cOperation
		NULL,				// EncryptCode
		pszAccount,			// AccountID
		NULL,				// AccountID Len
		NULL,				// AccountPassword
		NULL,				// AccountPassword Len
		NULL,				// lCID
		pszWorld,			// World Name;
		pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
		NULL,				// pvDetailServerInfo; 서버군 정보.
		NULL,				// lResult
		NULL,				// version info
		NULL,
		NULL,
		NULL,				// isLimited
		NULL				// isProtected
		);

	m_pAgpmLogin->m_csPacket.FreePacket(pvDetailCharInfo);

	if (pvPacket && nPacketLength > 0)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, m_nNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
	}

	return bResult;
}




//	Callback
//==============================================================
//
BOOL AgcmLogin::CBSocketConnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmLogin *pThis = (AgcmLogin *) pClass;

	pThis->m_cRequestConnect.Stop();
	pThis->m_bIsConnectedLoginServer	= TRUE;
	pThis->EnumCallback(AGCMLOGIN_CB_CONNECT, &pThis->m_nNID, NULL);

	// 2006.04.11. steeple
	// 아래 코드는 AgcmStartupEncryption 의 단계가 끝난 후 불리게 된다.
	//pThis->SendGetEncryptCode();

	return TRUE;
}


BOOL AgcmLogin::CBSocketDisConnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmLogin *pThis = (AgcmLogin *) pClass;

	pThis->EnumCallback(AGCMLOGIN_CB_DISCONNECT, NULL, pCustData);

	return TRUE;
}


BOOL AgcmLogin::CBSocketError(PVOID pData, PVOID pClass, PVOID pCustData					)
{
	if (!pClass)
		return FALSE;

	AgcmLogin *pThis = (AgcmLogin *) pClass;

	pThis->EnumCallback(AGCMLOGIN_CB_CONNECT, NULL, NULL);

	AcClientSocket *pSocket = (AcClientSocket *) pData;

	if (!pSocket)
		return FALSE;

	pSocket->Close();

	pThis->m_bIsConnectedLoginServer = FALSE;

	return TRUE;
}




//	Callback Setting
//===================================================
//
BOOL AgcmLogin::SetCallbackConnectLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_CONNECT, pfCallback, pClass);
}

BOOL AgcmLogin::SetCallbackDisconnectLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_DISCONNECT, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackEncryptCodeSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_ENCRYPT_CODE, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackInvalidClientVersion(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_INVALID_CLIENT_VERSION, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackSignOnSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_SIGN_ON, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackUnionInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_UNION_INFO, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackCharacterName(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_CHARACTER_NAME, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackCharacterNameFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_CHARACTER_NAME_FINISH, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackCharacterInfoFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_CHARACTER_INFO_FINISH, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackEnterGameEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_ENTER_GAME, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackRaceBase(ApModuleDefaultCallBack pfCallback,PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_RACE_BASE, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackNewCharacterName(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_NEW_CHARACTER_NAME, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackNewCharacterInfoFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_NEW_CHARACTER_INFO_FINISH, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_REMOVE_CHARACTER, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackLoginResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_LOGIN_RESULT, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackEKeyActive(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_EKEY_ACTIVE, pfCallback, pClass);
}


BOOL AgcmLogin::SetCallbackCompensationInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLOGIN_CB_COMPENSATION_INFO, pfCallback, pClass);
}


VOID AgcmLogin::SetLoginServer(TCHAR *szAddress)
{
	if (strlen(szAddress) < 64)
	{
		m_bCustomLoginInfo = TRUE;
		strcpy(m_szIPAddress, szAddress);
	}
}

BOOL AgcmLogin::_IsExistLoginSelectFile( void )
{
	FILE* pFile = fopen( ".\\Archlordtest.ini", "r" );
	return pFile ? TRUE : FALSE;
}
