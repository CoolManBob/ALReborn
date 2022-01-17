#include <windows.h>
#include <stdio.h>
#include "AuNProtect.h"

AuNProtect g_nProtect;


AuNProtect::AuNProtect( void )
{
}

AuNProtect::~AuNProtect( void )
{
}

bool AuNProtect::Init( void )
{
	return true;
}

bool AuNProtect::CreateAuthObject(CCSAuth2 **p)
{
	return false;
}

bool AuNProtect::ServerToClient(CCSAuth2* pAuth2, _GG_AUTH_DATA* ggData)
{
	return false;
}

bool AuNProtect::ClientToServer(CCSAuth2* pAuth2, _GG_AUTH_DATA* ggData)
{
	return false;
}
