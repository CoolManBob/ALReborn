#pragma once

class AuGameInfoJpn
{
public:
	bool Init( char* gamestring, bool real );
	
	void StartGame( const char* account );
	void ExitGame( const char* account );
};

extern AuGameInfoJpn g_jGameInfo;
