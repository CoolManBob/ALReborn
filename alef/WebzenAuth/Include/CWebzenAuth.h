#ifndef __CLASS_WEBZEN_AUTH_H__
#define __CLASS_WEBZEN_AUTH_H__



#include <string>



class CWebzenAuth
{
private:
	BOOL									m_bIsAutoLogin;
	std::string								m_strGameString;
	std::string								m_strAccountGUID;
	std::string								m_strGameID;
	std::string								m_strGameAuthKey;

public :
	CWebzenAuth( void );
	~CWebzenAuth( void );

public :
	static CWebzenAuth*	GetInstance			( void );

public :
	BOOL				OnInitialize		( char* pCommandLine );

private :
	BOOL				_IsFileExist		( char* pFilePath );

public :
	const char*			GetGameString		( void ) { return m_strGameString.c_str(); }
	const char*			GetAccountGUID		( void ) { return m_strAccountGUID.c_str(); }
	const char*			GetGameID			( void ) { return m_strGameID.c_str(); }
	const char*			GetGameAuthKey		( void ) { return m_strGameAuthKey.c_str(); }
	BOOL				IsAutoLogin			( void ) { return m_bIsAutoLogin; }
};



#endif