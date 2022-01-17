#ifdef _AREA_JAPAN_
#pragma once

#include <vector>

class CJapaneseClientAuth
{
public:
	CJapaneseClientAuth();

	bool  Init( char* cmdLine );
	void  Refresh( void );
	bool  UpdateGamestring( char* newGamestring );
	char* HanAuthErrorPrint( int errorCode );

	// query
	const bool  GetAutoLogin ( void ) { return m_autoLogin; }
	const bool  IsRealService( void );
	
	const char* GetMemberID  ( void ) { return &m_memberID[0]; }
	const char* GetAuthString( void );
	char*       GetGameString( void ) { return &m_gamestring[0]; }
	
private:
	char* GetSecondParam( char* szCmdLine, int length = 4096 );

	// HanAuth와 붙이지 않고 일반 로긴을 사용해서 할 경우를 위해
	// 테스트 환경 때문에 어쩔수 없이 추가
	bool			  m_autoLogin;

	char		  m_BillNo[32];	//billing number
	std::vector<char> m_gamestring; //게임스트링
	std::vector<char> m_memberID; //계정아이디
	std::vector<char> m_authString;
};

extern CJapaneseClientAuth g_jAuth;

#endif //_AREA_JAPAN_