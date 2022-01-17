#ifdef _AREA_JAPAN_
#include <windows.h>
#include <stdio.h>
#include "AuJapaneseClientAuth.h"
#include "HanAuthForClient.h"

CJapaneseClientAuth g_jAuth;

CJapaneseClientAuth::CJapaneseClientAuth()
:	m_autoLogin( true )
{
	m_gamestring.resize( SIZE_GAMESTRING );
	m_memberID.resize( 64 );
	m_authString.resize( SIZE_AUTHSTRING );
	ZeroMemory(m_BillNo,32);
}

bool CJapaneseClientAuth::Init( char* cmdLine )
{
	_snprintf_s( &m_gamestring[0], SIZE_GAMESTRING, _TRUNCATE, "%s", GetSecondParam( cmdLine ) );

	if ( !strncmp( &m_gamestring[0], "no_auto_login", 32 ) )
		m_autoLogin = false;
	else
	{
		int authResult = HanAuthInitGameString( &m_gamestring[0] );

		if ( HAN_AUTHCLI_OK == authResult )
		{
			authResult = HanAuthForClientGameString( &m_gamestring[0] );

			if ( HAN_AUTHCLI_OK == authResult )
			{
				authResult = HanAuthGetId( &m_gamestring[0], &m_memberID[0], 128 );
			}
		}

		if ( HAN_AUTHCLI_OK != authResult )
		{
			MessageBox( NULL, HanAuthErrorPrint( authResult ), "Hangame Authentication", MB_OK );
			return false;
		}
	}
	return true;
}

char* CJapaneseClientAuth::HanAuthErrorPrint( int errorCode )
{
	switch ( errorCode )
	{
		case HAN_AUTHCLI_ARGUMENT_INVALID:
			return "?達された因子が正しくありません。";
		case HAN_AUTHCLI_INITED_ALREADY:
			return "クライアントモデュ?ルがすでに初期化されています。";
		case HAN_AUTHCLI_INITED_NOT:
			return "クライアントモデュ?ルが初期化されていません。";
		case HAN_AUTHCLI_INITED_FAIL:
			return "モデュ?ル初期化に失敗しました。";
		case HAN_AUTHCLI_AUTHHTTP_INITFAIL:
			return "HTTP初期化に失敗しました。";
		case HAN_AUTHCLI_AUTHHTTP_CONNFAIL:
			return "HTTPアクセスに失敗しました。";
		case HAN_AUTHCLI_REFRESHHTTP_INITFAIL:
			return "HTTP初期化(refresh)に失敗しました。";
		case HAN_AUTHCLI_REFRESHHTTP_CONNFAIL:
			return "HTTPアクセス(refresh)に失敗しました。";
		case HAN_AUTHCLI_NOT_IMPLEMENTED:
			return "まだ実装されてない??です。";
		case HAN_AUTHCLI_AUTHHTTP_OPENREQFAIL:
			return "HTTPリクエストオ?プンに失敗しました。";
		case HAN_AUTHCLI_AUTHHTTP_SENDREQFAIL:
			return "リクエストSendに失敗しました。";
		case HAN_AUTHCLI_COOKIE_SETFAIL:
			return "クッキ?（cookie）セッティングに失敗しました。";
		case HAN_AUTHCLI_GAMESTRING_IDINVALID:
			return "?達されたゲ??ストリングが正しくありません。";
		case HAN_AUTHCLI_GAMESTRING_USNINVALID:
			return "USNが正しくありません。";
		case HAN_AUTHCLI_GAMESTRING_GAMEIDINVALID:
			return "GAMEIDが正しくありません。";
		case HAN_AUTHCLI_GAMESTRING_SERVICECODEINVALID:
			return "servicecodeが正しくありません。";
		//case HAN_AUTHCLI_GAMESTRING_VALUENOTEXIST:
		//	return "gamestringが存在しません。";
		case HAN_AUTHCLI_DLL_UNEXPECTED:
			return "?想されていないエラ?が発生しました。";
		case HAN_AUTHCLI_PARAM_INVALID:
			return "ウェブサ?バ?に?達された因子が適切ではありません。";
		case HAN_AUTHCLI_IPADDR_INVALID:
			return "IPアドレスが間違っています。";
		case HAN_AUTHCLI_MEMBERID_INVALID:
			return "使用者IDが間違っています。";
		case HAN_AUTHCLI_PASSWORD_INCORRECT:
			return "使用者パスワ?ドが間違っています。";
		case HAN_AUTHCLI_PASSWORD_MISMATCHOVER:
			return "パスワ?ド入力?ス回数制限をオ?バ?しました。";
		case HAN_AUTHCLI_MEMBERID_NOTEXIST:
			return "存在しない使用者です。";
		case HAN_AUTHCLI_SYSTEM_ERROR:
			return "System Error.";
		case HAN_AUTHCLI_COOKIE_SETERROR:
			return "Wrong Cookie";
		default:
			return "?想されていないエラ?が発生しました。";
	}
}

char* CJapaneseClientAuth::GetSecondParam( char* szCmdLine, int length )
{
	bool quote = false;
	int  index = 0;

	int loop = std::min<int>( length, (int) strlen(szCmdLine) );

	for ( ; index < length; ++index )
	{
		if ( szCmdLine[index] == 0 )
			break;

		if ( szCmdLine[index] == '\"' )
			quote = !quote;

		if ( !quote && szCmdLine[index] == ' ' )
			break;
	}

	if ( length > index )
		return ( szCmdLine + index + 1 );
	else
		return 0;
}

void CJapaneseClientAuth::Refresh()
{
	HanAuthRefresh( &m_memberID[0] );
}

bool CJapaneseClientAuth::UpdateGamestring( char* newGamestring )
{
	int authResult = UpdateGameString( &m_gamestring[0], newGamestring, SIZE_GAMESTRING );

	if ( HAN_AUTHCLI_OK == authResult ) 
		return true;
	
	HanAuthErrorPrint( authResult );

	return false;
}

const bool CJapaneseClientAuth::IsRealService( void )
{
	bool bResult = true;

	int serviceType = 0;

	HanAuthGetServiceTypeCode( &m_gamestring[0], &serviceType );

	bResult = (serviceType == SERVICE_REAL);

	return bResult;
}

const char* CJapaneseClientAuth::GetAuthString( void )
{
	::GetAuthString( &m_authString[0], static_cast<int>(m_authString.size()) );
	return &m_authString[0];
}
#endif //_AREA_JAPAN_