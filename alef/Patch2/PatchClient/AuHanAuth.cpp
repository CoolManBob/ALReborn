#include "stdafx.h"

#if defined( _AREA_KOREA_ ) || defined( _AREA_JAPAN_ )

#ifdef _AREA_KOREA_
	#include "HanAuthForClientKor.h"
#endif

#ifdef _AREA_JAPAN_
	#include "HanAuthForClient.h"
#endif

#include "AuHanAuth.h"
#include "AutoDetectMemoryLeak.h"

CAuHangAuth g_cHanAuthKor;

const char* CAuHangAuth::szNonExcuteFile = "NoExcuteHanAuth.arc";

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

bool	CAuHangAuth::Initialize( bool bExcute, char* szCmdLine, eHangAuthType eType )
{
	m_eType		= eType;
	
	if( !bExcute )
	{
		m_bExcute	= bExcute;
		return true;
	}
	
	FILE* fp = fopen( szNonExcuteFile, "r" );
	if( fp  )
	{
		m_bExcute	= false;
		fclose( fp );
		return	true;
	}

	char* szPos = strstr( szCmdLine, " " );
	//MessageBox( NULL, szCmdLine, "AuHangAuth", MB_OK );
	strcpy( m_szGameString, szPos );
	int nResult = HAN_AUTHCLI_OK;
	switch( m_eType )
	{
	case eHangAuthPatcher:
		{
			nResult = HanAuthInitGameString( m_szGameString );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			nResult = HanAuthForClientGameString( m_szGameString );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			//nResult = HanAuthGetId( m_szGameString, m_szMemberID, 64 );
			//if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

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

			nResult = GetAuthString( m_szAuthString, SIZE_AUTHSTRING );
			if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			//HanAuthGetId( m_szGameString, m_szMemberID, 64 );
			//if( HAN_AUTHCLI_OK != nResult  )	goto FAIL;

			//m_nMemberNumber = atoi( m_szMemberID );
		}
		break;
	default:
		//assert(0);
		return false;
	}

	return true;

FAIL:
	//MessageBox( NULL, ErrorCode( nResult ), "Hangame Authentication", MB_OK );
	return false;
}

void	CAuHangAuth::Destory()
{
}

void	CAuHangAuth::Refresh()
{
	if( eHangAuthPatcher == m_eType )
	{
		HanAuthRefresh( m_szMemberID );
		UpdateGamestring( m_szGameString );
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