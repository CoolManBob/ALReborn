#include "CGameFramework_Utility.h"
#include "shellapi.h"



char** FrameworkUtil::CommandLineToArgv( int* pCommandCount )
{
	if( !pCommandCount ) return NULL;

	LPWSTR pCommandLine = ::GetCommandLineW();
	LPWSTR* ppArgvWBCS = ::CommandLineToArgvW( pCommandLine, pCommandCount );
	if( !ppArgvWBCS || *pCommandCount <= 0 ) return NULL;

	char** ppArgvMBCS = ( char** )new char*[ *pCommandCount ];
	for( int nCount = 0 ; nCount < *pCommandCount ; nCount++ )
	{
		ppArgvMBCS[ nCount ] = new char[ 256 ];
		memset( ppArgvMBCS[ nCount ], 0, sizeof( char ) * 256 );
		::WideCharToMultiByte( CP_ACP, 0, ppArgvWBCS[ nCount ], -1, ppArgvMBCS[ nCount ], sizeof( char ) * 256, NULL, NULL );
	}

	return ppArgvMBCS;
}

void FrameworkUtil::DeleteCommandLine( char** ppArgv, int nCommandCount )
{
	if( !ppArgv || nCommandCount <= 0 ) return;

	for( int nCount = 0 ; nCount < nCommandCount ; nCount++ )
	{
		delete[] ppArgv[ nCount ];
		ppArgv[ nCount ] = NULL;
	}

	delete[] ppArgv;
	ppArgv = NULL;
}