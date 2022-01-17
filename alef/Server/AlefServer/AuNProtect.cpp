#include <windows.h>
#include <stdio.h>

#ifdef _AREA_JAPAN_

#include "AuNProtect.h"

#include "ggsrv25.h"


#ifdef WIN64
	#pragma comment(lib, "ggsrvlib25_win64_MT.lib" )
#else
	#pragma comment(lib, "ggsrvlib25_MT.lib")
#endif

AuNProtect g_nProtect;

AuNProtect::AuNProtect( void )
{
}

AuNProtect::~AuNProtect( void )
{
	CleanupGameguardAuth();

}

bool AuNProtect::Init( void )
{
	DWORD ret = InitGameguardAuth("", 5, true, 0x03);
	if ( ERROR_SUCCESS != ret )
	{
		printf( "Fail[%d] InitGameguardAuth()\n", ret );
		return false;
	}

	SetUpdateCondition( 30, 50 );
	return true;
}

bool AuNProtect::CreateAuthObject(CCSAuth2 **p)
{
	*p = new CCSAuth2;
	return NULL != *p;
}

bool AuNProtect::ServerToClient(CCSAuth2* pAuth2, _GG_AUTH_DATA* ggData)
{
	if ( ERROR_SUCCESS == pAuth2->GetAuthQuery() )
	{
		memcpy( ggData, &pAuth2->m_AuthQuery, sizeof(GG_AUTH_DATA) );
		return true;
	}
}

bool AuNProtect::ClientToServer(CCSAuth2* pAuth2, _GG_AUTH_DATA* ggData)
{
	memcpy( &pAuth2->m_AuthAnswer, ggData, sizeof(GG_AUTH_DATA) );
	return pAuth2->CheckAuthAnswer() == ERROR_SUCCESS;
}

// callback
GGAUTHS_API void NpLog(int mode, char* msg)
{
}

GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report)
{
}

#endif //_AREA_JAPAN_