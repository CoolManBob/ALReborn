#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <windows.h>
#ifdef _AREA_JAPAN_
#include "HanAuthForSvr_JPN.h"
#else
#include "HanAuthForSvr.h"
#endif
#include "HanServerAuth.h"
#include "AuGameEnv.h"

CHanServerAuth g_SvrAuth;

// local only function
namespace {

	char* GetSecondParam( char* str, int length )
	{
		bool quote = false;
		int  index = 0;

		int loop = std::min<int>( length, (int)strlen(str) );

		for ( ; index < length; ++index )
		{
			if ( str[index] == 0 )
				break;

			if ( str[index] == '\"' )
				quote = !quote;

			if ( !quote && str[index] == ' ' )
				break;
		}

		if ( length > index )
			return ( str + index + 1 );
		else
			return 0;
	}
}

//
// class CHanServerAuth
//

CHanServerAuth::CHanServerAuth() : m_iArea(HAN_SERVER_AREA_KOREA)
{
}

bool CHanServerAuth::Init( int iArea )
{
	m_iArea = iArea;
	GetGameEnv().InitEnvironment();

	unsigned code = GetGameEnv().IsAlpha() ? SERVICE_ALPHA : SERVICE_REAL;
	printf( "Hangame server auth: %s\n", code == SERVICE_REAL ? "Real" : "Alpha" );

	char* szGameString = NULL;
	switch(m_iArea)
	{
		case HAN_SERVER_AREA_KOREA:
			szGameString = "K_ARCHLORD";
			code |= SERVICE_KOR;
			break;

		case HAN_SERVER_AREA_JAPAN:
			szGameString = "J_ARCHLORD";
			code |= SERVICE_JPN;
			break;

		default:
			szGameString = "K_ARCHLORD";
			code |= SERVICE_KOR;
			break;
	}

	int result = HanAuthInit( szGameString, code, 5 );
	if ( HAN_AUTHSVR_OK == result )
	{
		printf( "Hangame server auth initialized successfully\n" );
		return true;
	}

	printf( "%s\n", GetError( result ) );
	return false;
}

int CHanServerAuth::Auth( char* account, char* authstring, int* piAge )
{
	int lResult = HAN_AUTHSVR_MEMBERID_INVALID;

	if ( strnlen( account, 49 ) <= 48 && strnlen( authstring, SIZE_AUTHSTRING + 1 ) <= SIZE_AUTHSTRING )
	{
		char authOutBuffer[2048];
		lResult = HanAuthForSvr( account, authstring, authOutBuffer, sizeof(authOutBuffer) );

		if(lResult == HAN_AUTHSVR_OK && piAge)
			*piAge = HanAuthGetUserAge(authOutBuffer);
	}

	return lResult;
}

char* CHanServerAuth::GetError(int errorCode)
{
	static char errmsg[32];

	switch( errorCode )
	{
	case HAN_AUTHSVR_PARAM_INVALID:
		return "Parameter invalid";
	case HAN_AUTHSVR_IP_INVALID:
		return "IP invalid";
	case HAN_AUTHSVR_MEMBERID_INVALID:
		return "Member ID invalid";
	case HAN_AUTHSVR_PASSWORD_INCORRECT:
		return "Password incorrect";
	case HAN_AUTHSVR_PASSWORD_MISMATCHOVER:
		return "Password mismatch over";
	case HAN_AUTHSVR_MEMBERID_NOTEXIST:
		return "ID not exist";
	case HAN_AUTHSVR_SYSTEM_ERROR:
		return "System error";
	case HAN_AUTHSVR_COOKIE_SETTINGERR:
		return "Cookie setting error";
	case HAN_AUTHSVR_COOKIE_NOTEXIST:
		return "Cookie not exist";
	case HAN_AUTHSVR_COOKIE_TIMEOUT:
		return "Cookie timeout";
	case HAN_AUTHSVR_ARGUMENT_INVALID:
		return "Argument invalid";
	case HAN_AUTHSVR_INITED_ALREADY:
		return "Inited already";
	case HAN_AUTHSVR_INITED_NOT:
		return "Not inited";
	case HAN_AUTHSVR_INITED_FAIL:
		return "Init fail";
	case HAN_AUTHSVR_CONNPOOL_CREATEFAIL:
		return "Connection pool creation fail";
	case HAN_AUTHSVR_GETCONNECTION_FAIL:
		return "Getting connection pool fail";
	case HAN_AUTHSVR_OPENCONNECTION_FAIL:
		return "Opening connection pool fail";
	case HAN_AUTHSVR_SENDCONNECTION_FAIL:
		return "Send connection fail";
	case HAN_AUTHSVR_RECVCONNECTION_FAIL:
		return "Recv connection fail";
	case HAN_AUTHSVR_RETURNVALUE_INVALID:
		return "Return value invalid";
	case HAN_AUTHSVR_AUTHSTRING_INVALID:
		return "Authstring invalid";
	case HAN_AUTHSVR_DLL_UNEXPECTED:
		return "unexpected";
	default:
		_snprintf_s( errmsg, 32, _TRUNCATE, "Unknown: %d", errorCode );
		return errmsg;
	}
}
