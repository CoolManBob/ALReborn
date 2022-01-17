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

				//일본의 빌링모듈 변경으로 게임서버에 접속할때 받는것으로 변경 supertj@090115
				/*
				if ( HAN_AUTHCLI_OK == authResult )
				{
					char userNo[32] = "0";
					authResult = HanAuthGetBillUserNo( &m_gamestring[0], userNo, 32 );
					
					m_memberNo = atoi( userNo );
				}
				*/
			}
		}

		if ( HAN_AUTHCLI_OK != authResult 
			&& HAN_AUTHCLI_INVALID_BILLINGNO != authResult )
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
			return "?묪궠귢궫덓럔궕맫궢궘궇귟귏궧귪갃";
		case HAN_AUTHCLI_INITED_ALREADY:
			return "긏깋귽귺깛긣긾긢깄?깑궕궥궳궸룊딖돸궠귢궲궋귏궥갃";
		case HAN_AUTHCLI_INITED_NOT:
			return "긏깋귽귺깛긣긾긢깄?깑궕룊딖돸궠귢궲궋귏궧귪갃";
		case HAN_AUTHCLI_INITED_FAIL:
			return "긾긢깄?깑룊딖돸궸렪봲궢귏궢궫갃";
		case HAN_AUTHCLI_AUTHHTTP_INITFAIL:
			return "HTTP룊딖돸궸렪봲궢귏궢궫갃";
		case HAN_AUTHCLI_AUTHHTTP_CONNFAIL:
			return "HTTP귺긏긜긚궸렪봲궢귏궢궫갃";
		case HAN_AUTHCLI_REFRESHHTTP_INITFAIL:
			return "HTTP룊딖돸(refresh)궸렪봲궢귏궢궫갃";
		case HAN_AUTHCLI_REFRESHHTTP_CONNFAIL:
			return "HTTP귺긏긜긚(refresh)궸렪봲궢귏궢궫갃";
		case HAN_AUTHCLI_NOT_IMPLEMENTED:
			return "귏궬렳몧궠귢궲궶궋??궳궥갃";
		case HAN_AUTHCLI_AUTHHTTP_OPENREQFAIL:
			return "HTTP깏긏긄긚긣긆?긵깛궸렪봲궢귏궢궫갃";
		case HAN_AUTHCLI_AUTHHTTP_SENDREQFAIL:
			return "깏긏긄긚긣Send궸렪봲궢귏궢궫갃";
		case HAN_AUTHCLI_COOKIE_SETFAIL:
			return "긏긞긌?걁cookie걂긜긞긡귻깛긐궸렪봲궢귏궢궫갃";
		case HAN_AUTHCLI_GAMESTRING_IDINVALID:
			return "?묪궠귢궫긒??긚긣깏깛긐궕맫궢궘궇귟귏궧귪갃";
		case HAN_AUTHCLI_GAMESTRING_USNINVALID:
			return "USN궕맫궢궘궇귟귏궧귪갃";
		case HAN_AUTHCLI_GAMESTRING_GAMEIDINVALID:
			return "GAMEID궕맫궢궘궇귟귏궧귪갃";
		case HAN_AUTHCLI_GAMESTRING_SERVICECODEINVALID:
			return "servicecode궕맫궢궘궇귟귏궧귪갃";
		//case HAN_AUTHCLI_GAMESTRING_VALUENOTEXIST:
		//	return "gamestring궕뫔띪궢귏궧귪갃";
		case HAN_AUTHCLI_DLL_UNEXPECTED:
			return "?몒궠귢궲궋궶궋긄깋?궕뵯맯궢귏궢궫갃";
		case HAN_AUTHCLI_PARAM_INVALID:
			return "긂긃긳긖?긫?궸?묪궠귢궫덓럔궕밙먛궳궼궇귟귏궧귪갃";
		case HAN_AUTHCLI_IPADDR_INVALID:
			return "IP귺긤깒긚궕듩댾궯궲궋귏궥갃";
		case HAN_AUTHCLI_MEMBERID_INVALID:
			return "럊뾭롌ID궕듩댾궯궲궋귏궥갃";
		case HAN_AUTHCLI_PASSWORD_INCORRECT:
			return "럊뾭롌긬긚깗?긤궕듩댾궯궲궋귏궥갃";
		case HAN_AUTHCLI_PASSWORD_MISMATCHOVER:
			return "긬긚깗?긤볺쀍?긚됷릶맕뙽귩긆?긫?궢귏궢궫갃";
		case HAN_AUTHCLI_MEMBERID_NOTEXIST:
			return "뫔띪궢궶궋럊뾭롌궳궥갃";
		case HAN_AUTHCLI_SYSTEM_ERROR:
			return "System Error.";
		case HAN_AUTHCLI_COOKIE_SETERROR:
			return "Wrong Cookie";
		default:
			return "?몒궠귢궲궋궶궋긄깋?궕뵯맯궢귏궢궫갃";
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