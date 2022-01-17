#ifdef _AREA_KOREA_
#include "HanAuthForClientKor.h"
#include "AuHanAuth.h"
#include <vector>
#include <string>
#include <algorithm>

CAuHangAuth g_cHanAuthKor;

const char* CAuHangAuth::szNonExecuteFile		= "NoExcuteHanAuth.arc";
const char* CAuHangAuth::szNonExecuteCommand	= "no_auto_login";
const DWORD CAuHangAuth::s_dwTickMax			= 1000 * 60 * 3;

CAuHangAuth::CAuHangAuth() : m_eType(eHangAuthMax), m_bExcute(TRUE), m_nMemberNumber(0)
{
	memset( m_szErrorCode, 0, sizeof(char) * 512 );
	memset( m_szMemberID, 0, sizeof(char) * 64 );

	m_szAuthString = new char[SIZE_AUTHSTRING];
	memset( m_szAuthString, 0, sizeof(char) * SIZE_AUTHSTRING );

	m_szGameString = new char[SIZE_GAMESTRING];
	memset( m_szGameString, 0, sizeof(char) * SIZE_GAMESTRING );
}

CAuHangAuth::~CAuHangAuth()
{
	if( m_szAuthString )
	{
		delete [] m_szAuthString;
		m_szAuthString = NULL;
	}

	if( m_szGameString )
	{
		delete [] m_szGameString;
		m_szGameString = NULL;
	}
}

inline BOOL IsFileExist( const char* path )
{
    DWORD dwAttr = GetFileAttributes( path );
    return dwAttr != INVALID_FILE_ATTRIBUTES && ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) == 0;
}

char ToLower(char c)
{
	return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
}

char toUpper(char c)
{
	return c >= 'a' && c <= 'z' ? c - ('a' - 'A') : c;
}

inline BOOL IsCommand( const char* command, const char* noExecute )
{
	if( !command || !noExecute )	return FALSE;

	std::string	strCommand( command );
	std::transform( strCommand.begin(), strCommand.end(), strCommand.begin(), ToLower );
	return std::string::npos != strCommand.find( noExecute ) ? TRUE : FALSE;
}

bool	CAuHangAuth::Initialize( bool bExcute, char* szCmdLine, eHangAuthType eType )
{
	m_eType		= eType;
	m_bExcute	= bExcute;
	
	if( !m_bExcute )	return true;

	//find condition that hanauth execute.
	if( IsFileExist( szNonExecuteFile ) || IsCommand( szCmdLine, szNonExecuteCommand ) )
	{
		m_bExcute	= false;
		return	true;
	}

	char* szSpace = strstr( szCmdLine, " " );
	strncpy_s( m_szGameString, SIZE_GAMESTRING, szSpace, strlen(szSpace) );	

	int nResult = HAN_AUTHCLI_OK;
	switch( m_eType )
	{
	case eHangAuthPatcher:
		{
			nResult = HanAuthInitGameString( m_szGameString );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			nResult = HanAuthForClientGameString( m_szGameString );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			nResult = HanAuthGetId( m_szGameString, m_szMemberID, 64 );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			nResult = HanAuthRefresh( m_szMemberID );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			nResult = UpdateGamestring( m_szGameString );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;
		}
		break;
	case eHangAuthClient:
		{
			nResult = HanAuthInitGameString( m_szGameString );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			nResult = HanAuthForClientGameString( m_szGameString );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			nResult = HanAuthGetId( m_szGameString, m_szMemberID, 64 );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			nResult = HanAuthRefresh( m_szMemberID );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			nResult = GetAuthString( m_szAuthString, SIZE_AUTHSTRING );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;
		}
		break;
	default:
		//assert(0);
		return false;
	}

	HanAuthMiniExplorer( false );

	m_dwTick = GetTickCount();

	return true;

FAIL:
	//MessageBox( NULL, ErrorCode( nResult ), "Hangame Authentication", MB_OK );
	return false;
}

void	CAuHangAuth::Destory()
{
	if( m_bExcute )
		HanAuthMiniExplorer( true );
}

void	CAuHangAuth::Refresh()
{
	if( !m_bExcute )	return;

	if( eHangAuthPatcher == m_eType )
		HanAuthRefresh( m_szMemberID );
	else if( eHangAuthClient == m_eType )
	{
		DWORD dwCurrentTick = GetTickCount();
		if( dwCurrentTick - m_dwTick > s_dwTickMax )
		{
			//char szText[256];
			//sprintf( szText, "%dºÐ HanAuth Refresh\n", ( dwCurrentTick - m_dwTick ) / 1000 );
			//OutputDebugString( szText );

			int nResult = 0;
			nResult = GetAuthString( m_szAuthString, SIZE_AUTHSTRING );
			if( 0 != nResult )
				MessageBox( NULL, ErrorCode( nResult ), "GetAuthStringError", MB_OK );

#ifdef SERVICE_KOR
			nResult = HanForceAuthRefresh( m_szMemberID );
			if( 0 != nResult )
				MessageBox( NULL, ErrorCode( nResult ), "HanForceAuthRefresh", MB_OK );
#else
			//nResult = HanForceAuthRefresh( m_szMemberID );
			//if( 0 != nResult )
			//	MessageBox( NULL, ErrorCode( nResult ), "HanForceAuthRefresh", MB_OK );
#endif
			//nResult = HanAuthRefresh( m_szMemberID );
			//if( 0 != nResult )
			//	MessageBox( NULL, ErrorCode( nResult ), "HanAuthRefreshError", MB_OK );

			m_dwTick = dwCurrentTick;
		}
	}
}

int		CAuHangAuth::UpdateGamestring( char* szGameString )
{
	char szGameStringOld[SIZE_GAMESTRING];
	strcpy( szGameStringOld, szGameString );
	return UpdateGameString( szGameStringOld, m_szGameString, SIZE_GAMESTRING );
}

char*	CAuHangAuth::ErrorCode( int nCode )
{
	memset( m_szErrorCode, 0, sizeof( char ) * 512 );

	switch( nCode )
	{
	case HAN_AUTHCLI_ARGUMENT_INVALID:		strcpy( m_szErrorCode, "HAN_AUTHCLI_ARGUMENT_INVALID" );	break;
	case HAN_AUTHCLI_INITED_ALREADY:		strcpy( m_szErrorCode, "HAN_AUTHCLI_INITED_ALREADY" );		break;
	case HAN_AUTHCLI_INITED_NOT:			strcpy( m_szErrorCode, "HAN_AUTHCLI_INITED_NOT" );			break;
	case HAN_AUTHCLI_INITED_FAIL:			strcpy( m_szErrorCode, "HAN_AUTHCLI_INITED_FAIL" );			break;
	case HAN_AUTHCLI_AUTHHTTP_INITFAIL:		strcpy( m_szErrorCode, "HAN_AUTHCLI_AUTHHTTP_INITFAIL" );	break;
	case HAN_AUTHCLI_AUTHHTTP_CONNFAIL:		strcpy( m_szErrorCode, "HAN_AUTHCLI_AUTHHTTP_CONNFAIL" );	break;
	case HAN_AUTHCLI_REFRESHHTTP_INITFAIL:	strcpy( m_szErrorCode, "HAN_AUTHCLI_REFRESHHTTP_INITFAIL" );break;
	case HAN_AUTHCLI_REFRESHHTTP_CONNFAIL:	strcpy( m_szErrorCode, "HAN_AUTHCLI_REFRESHHTTP_CONNFAIL" );break;
	case HAN_AUTHCLI_NOT_IMPLEMENTED:		strcpy( m_szErrorCode, "HAN_AUTHCLI_NOT_IMPLEMENTED" );		break;
	case HAN_AUTHCLI_AUTHHTTP_OPENREQFAIL:	strcpy( m_szErrorCode, "HAN_AUTHCLI_AUTHHTTP_OPENREQFAIL" );break;
	case HAN_AUTHCLI_AUTHHTTP_SENDREQFAIL:	strcpy( m_szErrorCode, "HAN_AUTHCLI_AUTHHTTP_SENDREQFAIL" );break;
	case HAN_AUTHCLI_COOKIE_SETFAIL:		strcpy( m_szErrorCode, "HAN_AUTHCLI_COOKIE_SETFAIL" );		break;
	case HAN_AUTHCLI_GAMESTRING_IDINVALID:	strcpy( m_szErrorCode, "HAN_AUTHCLI_GAMESTRING_IDINVALID" );break;
	case HAN_AUTHCLI_GAMESTRING_USNINVALID:	strcpy( m_szErrorCode, "HAN_AUTHCLI_GAMESTRING_USNINVALID" );break;
	case HAN_AUTHCLI_GAMESTRING_GAMEIDINVALID:		strcpy( m_szErrorCode, "HAN_AUTHCLI_GAMESTRING_GAMEIDINVALID" );		break;
	case HAN_AUTHCLI_GAMESTRING_SERVICECODEINVALID:	strcpy( m_szErrorCode, "HAN_AUTHCLI_GAMESTRING_SERVICECODEINVALID" );	break;
	case HAN_AUTHCLI_CREATEINTANCEFAIL:				strcpy( m_szErrorCode, "HAN_AUTHCLI_CREATEINTANCEFAIL" );				break;
	}

	return m_szErrorCode;
}

const bool CAuHangAuth::IsRealService()
{
	int nServiceType = 0;
	HanAuthGetServiceTypeCode( m_szGameString, &nServiceType );
	return nServiceType == SERVICE_REAL ? true : false;
}

char* CAuHangAuth::GetSecondParam( char* szCmdLine, int nLength )
{
	bool bQuote = false;
	int  nIndex = 0;
	for ( ; nIndex < nLength; ++nIndex )
	{
		if ( !szCmdLine[nIndex] )
			break;

		if ( szCmdLine[nIndex] == '\"' )
			bQuote = !bQuote;

		if ( !bQuote && szCmdLine[nIndex] == ' ' )
			break;
	}

	return nLength > nIndex ? ( szCmdLine + nIndex + 1 ) : 0;
}
#endif