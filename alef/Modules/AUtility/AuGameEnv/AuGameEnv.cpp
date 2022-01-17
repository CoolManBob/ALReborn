#include "AuGameEnv.h"
#include <sstream>
#include <windows.h>
#include <vector>

AuGameEnv& GetGameEnv( void )
{
	static AuGameEnv singleton;
	return singleton;
}

// class AuGameEnv

AuGameEnv::AuGameEnv( void )
:	m_isAlpha( false ),
	m_isDebugTest( false ),
	m_isAdmin( false )
{
}

void AuGameEnv::InitEnvironment()
{
	std::vector<char> buffer(2048);
	strncpy_s( &buffer[0], 2048, GetCommandLine(), _TRUNCATE );

	_strlwr_s( &buffer[0], 2048 );

	std::stringstream cmdLine( &buffer[0] );

	std::string token;

	// 1st parameter
	cmdLine >> token;
	token = "";

	// 2nd parameter
	cmdLine >> token;
	CheckOption( token );


	// 3rd parameter
	token = "";
	cmdLine >> token;
	CheckOption( token );

	// 4th parameter
	token = "";
	cmdLine >> token;
	CheckOption( token );
}

void AuGameEnv::CheckOption( std::string& token )
{
	if ( token == "alpha" )
		m_isAlpha = true;

	if ( token == "debugtest" )
		m_isDebugTest = true;

	if( token == "admin" )
		m_isAdmin = true;
}