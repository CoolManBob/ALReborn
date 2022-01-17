#include "AuGameGuard.h"

#ifdef _USE_NPROTECT_GAMEGUARD_

GGAUTHS_API void NpLog( int mode, char* msg )
{
	//if(mode & (NPLOG_DEBUG | NPLOG_ERROR)) //Select log mode.
	//	printf("%s\n", msg);
}

GGAUTHS_API void GGAuthUpdateCallback( PGG_UPREPORT report )
{
	//printf("GGAuth version update [%s] : [%ld] -> [%ld] \n", 
	//	(report->nType == 1) ? "GameGuard Ver" : "Protocol Num", 
	//	report->dwBefore, report->dwNext);
}

AuGGCSAuth::AuGGCSAuth()
{

}

AuGGCSAuth::~AuGGCSAuth()
{
	CleanupGameguardAuth();
}

BOOL AuGGCSAuth::Init()
{
	DWORD ret = InitGameguardAuth("", 5, false, 0x03);
	if ( ERROR_SUCCESS != ret )
	{
		printf( "Fail[%d] InitGameguardAuth()\n", ret );
		return FALSE;
	}

	SetUpdateCondition( 30, 50 );

	return TRUE;
}

#endif