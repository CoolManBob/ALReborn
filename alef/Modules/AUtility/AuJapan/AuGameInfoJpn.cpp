#include "AuGameInfoJpn.h"
#include <stdio.h>

#ifdef _AREA_JAPAN_
#include "HanGameInfoForClient.h"
#include "HanReportForClient.h"

	#ifdef _DEBUG
	#pragma comment( lib, "HanReportForClientD.lib" )
	#else
	#pragma comment( lib, "HanReportForClient.lib" )
	#endif

#endif

AuGameInfoJpn g_jGameInfo;

bool AuGameInfoJpn::Init( char* gamestring, bool real )
{
	int result = 0;
#ifdef _AREA_JAPAN_
	result = HanGameInfoInitGameString( gamestring );
	if ( real )
		result |= HanReportInit( "J_ARCHLORD", SERVICE_JPN | SERVICE_REAL, 0 );
	else
		result |= HanReportInit( "J_ARCHLORD", SERVICE_JPN | SERVICE_ALPHA, 0 );
#endif
	return (result == 0);
}

void AuGameInfoJpn::StartGame( const char* account )
{
#ifdef _AREA_JAPAN_

	HanGameInfoEnterLobby();
	HanGameInfoStartGame();

	char encryptID[33]  = {0, };
	char reportStr[128] = {0, };

	HanReportConvertMD5( const_cast<char*>(account), encryptID );
	_snprintf_s( reportStr, 128, _TRUNCATE, "userid=%s&eventcode=300", encryptID );
	HanReportSendPosLog( reportStr );
#endif
}

void AuGameInfoJpn::ExitGame( const char* account )
{
#ifdef _AREA_JAPAN_
	HanGameInfoEndGame();
	HanGameInfoExitLobby();

	char encryptID[33]  = {0, };
	char reportStr[128] = {0, };

	HanReportConvertMD5( const_cast<char*>(account), encryptID );
	_snprintf_s( reportStr, 128, _TRUNCATE, "userid=%s&eventcode=301", encryptID );
	HanReportSendPosLog( reportStr );
#endif
}

