#ifdef _AREA_JAPAN_

#include "AuNPGameLib.h"

AuNPWrapper npgl;

AuNPWrapper::AuNPWrapper()
#if defined(_AREA_JAPAN_) && !defined(_DEBUG)
:	m_npgame("ArchlordJP")
#endif
{
}

DWORD AuNPWrapper::Init( void )
{
#if defined(_AREA_JAPAN_) && !defined(_DEBUG)
	return m_npgame.Init();
#else
	return 0;
#endif
}

void AuNPWrapper::SetWndHandle(HWND hWnd)
{
#if defined(_AREA_JAPAN_) && !defined(_DEBUG)
	m_npgame.SetHwnd(hWnd);
#else
	UNREFERENCED_PARAMETER( hWnd );
#endif
}

void AuNPWrapper::Auth2(_GG_AUTH_DATA* pggAuthData)
{
#if defined(_AREA_JAPAN_) && !defined(_DEBUG)
	m_npgame.Auth2( pggAuthData );
#else
	UNREFERENCED_PARAMETER( pggAuthData );
#endif
}

DWORD AuNPWrapper::Check()
{
#if defined(_AREA_JAPAN_) && !defined(_DEBUG)
	return m_npgame.Check();
#else
	return 0;
#endif
}

DWORD AuNPWrapper::Send(LPCTSTR lpszUserId)
{
#if defined(_AREA_JAPAN_) && !defined(_DEBUG)
	return m_npgame.Send( lpszUserId );
#else
	return 0;
#endif
}

LPCSTR AuNPWrapper::GetInfo()
{
#if defined(_AREA_JAPAN_) && !defined(_DEBUG)
	return m_npgame.GetInfo();
#else
	return "";
#endif
}

#endif //_AREA_JAPAN_

#if defined(_AREA_GLOBAL) || defined(_AREA_KOREA_)


#include "AuNPGameLib.h"
#include "MagDebug.h"
#include "AuIgnoringHelper.h"

AuNPWrapper npgl;

//////////////////////////////////////////////////////////////////////////
//
AuNPWrapper::AuNPWrapper()
	: m_pNPGameLib(NULL)
{
}

AuNPWrapper::~AuNPWrapper()
{
	if(m_pNPGameLib)
		delete m_pNPGameLib;
}

BOOL AuNPWrapper::Init()
{
#ifdef _DEBUG
	m_pNPGameLib = new AuNPBase();
#else

	if( AuIgnoringHelper::IgnoreToFile( "NotCheckGameGuard.arc" ) ) // "NotCheckGameGuard.arc" 파일이 있으면 게임가드 사용안함
	{
		m_pNPGameLib = new AuNPBase();
	}
	else
	{
		m_pNPGameLib = new AuNPGameLib();
	}
#endif      

	if(!m_pNPGameLib->Init())
		return FALSE;

	return TRUE;
}

DWORD AuNPWrapper::Send(LPCTSTR lpszUserId)
{
	return m_pNPGameLib->Send( lpszUserId );
}

CHAR* AuNPWrapper::GetMessage(DWORD dwResult)
{
	return m_pNPGameLib->GetMessage(dwResult);
}

BOOL AuNPWrapper::NPGameMonCallback( DWORD dwMsg, DWORD dwArg )
{
	return m_pNPGameLib->NPGameMonCallback(dwMsg, dwArg);
}

BOOL AuNPWrapper::Check()
{
	return m_pNPGameLib->Check();
}

void AuNPWrapper::SetWndHandle( HWND hWnd )
{
	m_pNPGameLib->SetWndHandle(hWnd);
}

void AuNPWrapper::Auth( PVOID pggAuthData )
{
	m_pNPGameLib->Auth(pggAuthData);
}

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	//{
	//	char strCharBuff[256] = { 0, };
	//	sprintf_s(strCharBuff, sizeof(strCharBuff), "NPGameMonCallback - [%d][%d]\n", dwMsg, dwArg);
	//	//AuLogFile_s("GameGuard.txt", strCharBuff);
	//	OutputDebugString(strCharBuff);
	//}

	return npgl.NPGameMonCallback(dwMsg, dwArg);
}

//////////////////////////////////////////////////////////////////////////
//
#ifndef _DEBUG
//#ifdef _WIN64
#pragma comment(lib, "NPGameLib.lib" )
//#else
//#pragma comment(lib, "NPGameLib_78.lib")
//#endif
#include "AuStrTable.h"

AuNPGameLib::AuNPGameLib()
	: m_pNPGameLib(NULL)
	, m_hWnd(0)
{
}

AuNPGameLib::~AuNPGameLib()
{
	if( m_pNPGameLib )
		delete m_pNPGameLib;
}

BOOL AuNPGameLib::Init()
{
#ifdef _AREA_JAPAN_
	m_pNPGameLib = new CNPGameLib("ArchlordJP");
#endif

#if defined ( _AREA_GLOBAL_ )
	CHAR	szFileName[ MAX_PATH ];
	FILE*	pFile	=	fopen( "ArchlordGBTest.ini" , "rb" );
	if( pFile )
		sprintf_s( szFileName , MAX_PATH , "ArchlordGBTest" );
	else
		sprintf_s( szFileName , MAX_PATH , "ArchlordGB" );

	if( pFile )
		fclose( pFile );
#elif defined ( _AREA_KOREA_ )
	CHAR	szFileName[ MAX_PATH ];
	FILE*	pFile	=	fopen( "ArchlordKRTest.ini" , "rb" );
	if( pFile )
		sprintf_s( szFileName , MAX_PATH , "ArchlordKRTest" );
	else
		sprintf_s( szFileName , MAX_PATH , "ArchlordKR" );

	if( pFile )
		fclose( pFile );

	m_pNPGameLib = new CNPGameLib( szFileName );
#endif

	if(!m_pNPGameLib)
		return FALSE;

	DWORD dwResult = m_pNPGameLib->Init();
	if( dwResult != NPGAMEMON_SUCCESS)
	{
		CHAR strMessage[256] = { 0, };
		sprintf_s( strMessage, sizeof(strMessage), "%s", GetMessage(dwResult));
		MessageBox(NULL, strMessage, "GameGuard Error", MB_OK);
		return FALSE;
	}

	return TRUE;
}

CHAR* AuNPGameLib::GetMessage(DWORD dwResult)
{
	char* pMsg = 0;

	switch (dwResult)
	{
	case NPGAMEMON_ERROR_EXIST:
		pMsg = ClientStr().GetStr(STI_NP_EXIST);
		break;
	case NPGAMEMON_ERROR_GAME_EXIST:
		pMsg = ClientStr().GetStr(STI_NP_GAME_EXIST);
		break;
	case NPGAMEMON_ERROR_INIT:
		pMsg = ClientStr().GetStr(STI_NP_ERROR_INIT);
		break;
	case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
	case NPGAMEMON_ERROR_NFOUND_GG:
	case NPGAMEMON_ERROR_AUTH_INI:
	case NPGAMEMON_ERROR_NFOUND_INI:
		pMsg = ClientStr().GetStr(STI_NP_NOT_FOUND);
		break;
	case NPGAMEMON_ERROR_CRYPTOAPI:
		pMsg = ClientStr().GetStr(STI_NP_CRYPTOAPI);
		break;
	case NPGAMEMON_ERROR_EXECUTE:
		pMsg = ClientStr().GetStr(STI_NP_EXECUTE_FAIL);
		break;
	case NPGAMEMON_ERROR_ILLEGAL_PRG:
		pMsg = ClientStr().GetStr(STI_NP_ILLEGAL_PRO);
		break;
	case NPGMUP_ERROR_ABORT:
		pMsg = ClientStr().GetStr(STI_NP_ABORT_UPDATE);
		break;
	case NPGMUP_ERROR_CONNECT:
		pMsg = ClientStr().GetStr(STI_NP_CONNECT_FAIL);
		break;
	//case HOOK_TIMEOUT:
	//	pMsg = ClientStr().GetStr(STI_NP_TIMEOUT);
	//	break;
	case NPGAMEMON_ERROR_GAMEGUARD:
		pMsg = ClientStr().GetStr(STI_NP_GAMEGUARD);
		break;
	//case NPGMUP_ERROR_PARAM:
	//	pMsg = ClientStr().GetStr(STI_NP_NO_INI);
	//	break;
	//case NPGMUP_ERROR_INIT:
	//	pMsg = ClientStr().GetStr(STI_NP_NPGMUP);
	//	break;
	case NPGMUP_ERROR_DOWNCFG:
		pMsg = ClientStr().GetStr(STI_NP_DOWNCFG);
		break;
	case NPGMUP_ERROR_AUTH:
		pMsg = ClientStr().GetStr(STI_NP_AUTH_FAIL);
		break;
	case NPGAMEMON_ERROR_NPSCAN:
		pMsg = ClientStr().GetStr(STI_NP_NPSCAN_FAIL);
		break;
	default:
		pMsg = ClientStr().GetStr(STI_NP_DEFAULT);
		break;
	}

	return pMsg;
}

BOOL AuNPGameLib::NPGameMonCallback( DWORD dwMsg, DWORD dwArg )
{
	CHAR hackMsg[1024] = {0, };
	BOOL bReturn = FALSE;

	switch (dwMsg)
	{
		case NPGAMEMON_COMM_ERROR:
		case NPGAMEMON_COMM_CLOSE:
			{
			} break;
		case NPGAMEMON_INIT_ERROR:
			{
				wsprintf(hackMsg, "%s : %lu", ClientStr().GetStr(STI_NP_INIT_ERROR), dwArg);				
			} break;
		case NPGAMEMON_SPEEDHACK:
			{
				wsprintf(hackMsg, ClientStr().GetStr(STI_NP_SPEEDHACK));
			} break;
		case NPGAMEMON_GAMEHACK_KILLED:
			{
				wsprintf(hackMsg, "%s\r\n%s", ClientStr().GetStr(STI_NP_HACK_KILLED), "" /*npgl.GetInfo()*/);
			} break;
		case NPGAMEMON_GAMEHACK_DETECT:
			{
				wsprintf(hackMsg, "%s\r\n%s", ClientStr().GetStr(STI_NP_HACK_DETECT), "" /*npgl.GetInfo()*/);
			} break;
		case NPGAMEMON_GAMEHACK_DOUBT:
			{
				wsprintf(hackMsg, "%s\r\n", ClientStr().GetStr(STI_NP_HACK_DOUBT));
			} break;
		case NPGAMEMON_GAMEHACK_REPORT:
			{
				wsprintf(hackMsg, "%s\r\n", ClientStr().GetStr(STI_NP_HACK_DOUBT));
				bReturn = TRUE;
			} break;
		case NPGAMEMON_CHECK_CSAUTH2:
			{
				NPGameMonSend(dwArg);
				bReturn = TRUE;
			} break;
	}

	if( bReturn != TRUE )
	{
		MessageBox(m_hWnd, hackMsg, "GameGuard", MB_OK);
		throw "detect gamehack";
	}

	return bReturn;
}

BOOL AuNPGameLib::Check()
{
	DWORD dwResult = m_pNPGameLib->Check();
	if(dwResult != NPGAMEMON_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

DWORD AuNPGameLib::Send( LPCTSTR lpszUserId )
{
	return m_pNPGameLib->Send(lpszUserId);
}

void AuNPGameLib::SetWndHandle( HWND hWnd )
{
	m_pNPGameLib->SetHwnd(hWnd);
	m_hWnd = hWnd;
}

void AuNPGameLib::Auth( PVOID pggAuthData )
{
	GG_AUTH_DATA ggData;
	memcpy(&ggData, pggAuthData, sizeof(GG_AUTH_DATA));
	m_pNPGameLib->Auth2(&ggData);
}
#endif // _DEBUG

#endif //defined(_AREA_GLOBAL) || defined(_AREA_KOREA_)
