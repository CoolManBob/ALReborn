#include "CWebzenUtility.h"
#include "stdio.h"
#include "time.h"



CWebzenUtility::CWebzenUtility( void )
{
	m_fnMsgCallBack = NULL;
	::InitializeCriticalSection( &m_cs );
}

CWebzenUtility::~CWebzenUtility( void )
{
	::DeleteCriticalSection( &m_cs );
}

void CWebzenUtility::WriteFileLog( char* pMsg, char* pFileName )
{
	if( !pMsg || strlen( pMsg ) <= 0 ) return;
	if( !pFileName || strlen( pFileName ) <= 0 ) return;

	::EnterCriticalSection( &m_cs );

	DWORD dwCurrentTickCount = ::GetTickCount();

	struct tm tLogTime;
	memset( &tLogTime, 0, sizeof( tm ) );
	_localtime32_s( &tLogTime, ( __time32_t* )&dwCurrentTickCount );
	
	char strLog[ 2048 ] = { 0, };
	sprintf_s( strLog, sizeof( char ) * 2048, "[ %Y-%m-%d %H:%M:%S ] %s",
		tLogTime.tm_year + 1900, tLogTime.tm_mon + 1, tLogTime.tm_mday, tLogTime.tm_hour, tLogTime.tm_min, tLogTime.tm_sec, pMsg );

	int nLogLength = ( int )strlen( strLog );
	if( nLogLength <= 0 ) return;

#ifdef _DEBUG
	::OutputDebugString( strLog );
#endif

	HANDLE hFile = CreateFile( pFileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );  //File »ý¼º 
	if( hFile != INVALID_HANDLE_VALUE )
	{ 
		DWORD dwPos = SetFilePointer( hFile, 0, NULL, FILE_END );
		LockFile( hFile, dwPos, 0, dwPos + nLogLength, 0 ); 

		DWORD dwBytesWritten = 0;  
		WriteFile( hFile, strLog, nLogLength, &dwBytesWritten, NULL ); 

		UnlockFile( hFile, dwPos, 0, dwPos + nLogLength, 0 );
		CloseHandle( hFile );
	}  	

	::LeaveCriticalSection( &m_cs );
}

void CWebzenUtility::WriteFormattedLog( char* pFormat, ... )
{
	if( !pFormat || strlen( pFormat ) <= 0 ) return;

	char strBuffer[ 2048 ] = { 0, };

	va_list arg;
	va_start( arg, pFormat );
	vsprintf( strBuffer, pFormat, arg );
	va_end( arg );

#ifdef _DEBUG
	::OutputDebugString( strBuffer );
#endif

	if( m_fnMsgCallBack )
	{
		m_fnMsgCallBack( strBuffer );
	}
}

BOOL CWebzenUtility::ConvertWideToMBCS( WCHAR* pWideString, char* pBuffer, int nBufferSize )
{
	if( !pWideString || !pBuffer ) return FALSE;

	int nLengthRequire = ::WideCharToMultiByte( CP_ACP, 0, pWideString, -1, 0, 0, 0, 0 );
	if( nLengthRequire > nBufferSize ) return FALSE;

	::WideCharToMultiByte( CP_ACP, 0, pWideString, nLengthRequire, pBuffer, nBufferSize, 0, 0 );
	return TRUE;
}

BOOL CWebzenUtility::ConvertMBCSToWide( char* pMBCSString, WCHAR* pBuffer, int nBufferSize )
{
	if( !pMBCSString || !pBuffer ) return FALSE;

	int nLengtthMBCS = ( int )strlen( pMBCSString );
	if( nLengtthMBCS <= 0 ) return FALSE;

	int nLengthRequire = ::MultiByteToWideChar( CP_ACP, 0, pMBCSString, nLengtthMBCS, NULL, 0 ); 
	if( nLengthRequire > nBufferSize ) return FALSE;

	::MultiByteToWideChar( CP_ACP, 0, pMBCSString, nLengtthMBCS, pBuffer, nBufferSize );
	return TRUE;
}


