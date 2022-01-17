//#include "stdafx.h"
#include "ApBase.h"	
#include <string>

namespace myfirewall
{
	HANDLE hPermit = NULL;

	void On()
	{
		TCHAR szName[ MAX_PATH + 1 ] = {'\0',};

		if ( ! GetModuleFileName(NULL, szName, MAX_PATH) ) 
			return;
		
		for ( int i = 0; szName[i] != '\0'; ++i )
		{
			if ( szName[i] == '\\' )
				szName[i] = '_';
			//else
			//	szName[i] = toupper( szName[i] );
		}

		hPermit = CreateEvent( NULL, FALSE, FALSE, (LPCTSTR)szName );
		ASSERT( hPermit );

		TRACE( "myfirewall :: event ON ( %s )\n", szName );
	}

	void Off()
	{
		if ( hPermit )
		{
			CloseHandle( hPermit );
			hPermit = NULL;
		}

		TRACE( "myfirewall :: event OFF\n" );
	}
};