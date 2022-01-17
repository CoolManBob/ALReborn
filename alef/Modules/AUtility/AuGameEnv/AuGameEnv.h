//
// 2007.11.19. steeple
// 기존에 있던 거 수정.


#pragma once

#include <string>

class AuGameEnv
{
public:
	AuGameEnv( void );
	void InitEnvironment( void );

	bool IsAlpha    ( void ) { return m_isAlpha; }
	bool IsDebugTest( void ) { return m_isDebugTest; }
	bool IsAdmin	( void ) { return m_isAdmin; }

private:
	void	CheckOption( std::string& token );
	bool	m_isAlpha;		// 알파 환경
	bool	m_isDebugTest;	// 실제 환경을 다 무시하고 무조건 실행된다.
	bool	m_isAdmin;		// admin만 접속가능.
};

AuGameEnv& GetGameEnv( void );