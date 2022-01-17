//#include "stdafx.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "MagDebug.h"
#include "ApDefine.h"
#include "DbgHelp.h"
// 마고자 (2004-03-23 오후 5:03:47) : 
// Float To Int 퍼포먼스 개선용 글로벌 변수.
INTORFLOAT	g_unionBiasForFtoI;
INTORFLOAT	g_unionTempForFtoI;

#ifndef _DEBUG
#pragma warning ( disable : 4710 )
#endif
#pragma warning( push, 3 )
#include <vector>
using namespace std ;
#pragma warning( pop )

/*//////////////////////////////////////////////////////////////////////
                          File Scope Typedefs
//////////////////////////////////////////////////////////////////////*/
// The size of buffer that DiagAssert will use.  If you want to see more
// stack trace, make this a bigger number.
#define DIAGASSERT_BUFFSIZE 4096

/*//////////////////////////////////////////////////////////////////////
                          File Scope Typedefs
//////////////////////////////////////////////////////////////////////*/
// The typedef for the list of HMODULES that can possibly hold message
// resources.
typedef vector<HINSTANCE> HINSTVECTOR ;
// The address typedef.
typedef vector<ULONG> ADDRVECTOR ;

/*//////////////////////////////////////////////////////////////////////
                           File Scope Globals
//////////////////////////////////////////////////////////////////////*/
// The HMODULE vector.
static HINSTVECTOR g_HMODVector ;

// The DiagAssert display options.
static DWORD g_DiagAssertOptions	= MD_SHOWMSGBOX | MD_SHOWODS ;
BOOL g_bUseTrace			= TRUE ;

// The handle for assertion file output.
static HANDLE g_hAssertFile = INVALID_HANDLE_VALUE ;

// The handle for tracing file output.
static HANDLE g_hTraceFile = INVALID_HANDLE_VALUE ;

static BOOL	g_bCrashed	= FALSE;

static HWND g_hMessageBoxParentWnd = NULL;

//@{ Jaewon 20050525
// A dirty hack for printing when exporting 
typedef void (*OutputExportLog)(const char *);
OutputExportLog _outputExportLog = NULL;
//@} Jaewon

// 마고자 (2004-06-25 오후 4:18:56) : 크래시 처리용 ..
BOOL	MagDebug_IsCrashed()
{
	return g_bCrashed;
}

void	MagDebug_SetCrash( BOOL bCrash )
{
	g_bCrashed = bCrash;
}

void	MD_SetMainWindowHandle( HWND hWnd )
{
	g_hMessageBoxParentWnd	= hWnd;
}


/*//////////////////////////////////////////////////////////////////////
                         File Scope Prototypes
//////////////////////////////////////////////////////////////////////*/
// Handles doing the stack trace for DiagAssert.
void DoStackTrace ( LPTSTR szString  ,
                    DWORD  dwSize     ) ;

// The function that does the real assertions.
BOOL __stdcall RealAssert  ( DWORD  dwOverrideOpts  ,
                             LPCSTR szMsg           ,
                             BOOL   bAllowHalts      ) ;

/*//////////////////////////////////////////////////////////////////////
                            CODE STARTS HERE
//////////////////////////////////////////////////////////////////////*/

DWORD __stdcall
    SetAssertOptions ( DWORD dwOpts )
{
    /*if ( MD_DEFAULT == dwOpts )
    {
        return ( MD_DEFAULT ) ;
    }*/
    DWORD dwOld = g_DiagAssertOptions ;
    g_DiagAssertOptions = dwOpts ;
    return ( dwOld ) ;
}

HANDLE __stdcall
    SetAssertFile ( HANDLE hFile )
{
    HANDLE hRet = g_hAssertFile ;
    g_hAssertFile = hFile ;
    return ( hRet ) ;
}

BOOL __stdcall
    DiagAssertA ( DWORD  dwOverrideOpts ,
                  LPCSTR szMsg          ,
                  LPCSTR szFile         ,
                  DWORD  dwLine          )
{
    // First, save off the last error value.
    DWORD dwLastError = GetLastError ( ) ;

    // Format the C/C++ message.
    char szBuff [ 2048 ] ;
	// 소스폴더가 길경우 이 512 버퍼를 초과하는 경우가 생겨서 2048 로 넉넉하게 변경.

    sprintf ( szBuff               ,
               "File : %s\n"
               "Line : %d\n"
               "Expression : %s"    ,
               szFile               ,
               dwLine               ,
               szMsg                 ) ;

    // Set the error back and call the function that does all the work.
    SetLastError ( dwLastError ) ;
    return ( RealAssert ( dwOverrideOpts , szBuff , TRUE ) ) ;
}

BOOL __stdcall
    DiagAssertW ( DWORD     dwOverrideOpts  ,
                  LPCWSTR   szMsg           ,
                  LPCSTR    szFile          ,
                  DWORD     dwLine           )
{
    // First, save off the last error value.
    DWORD dwLastError = GetLastError ( ) ;

    // Format the C/C++ message.
    char szBuff [ 512 ] ;

    sprintf ( szBuff               ,
               "File : %s\n"
               "Line : %d\n"
               "Expression : %lS"   ,
               szFile               ,
               dwLine               ,
               szMsg                 ) ;

    // Set the error back and call the function that does all the work.
    SetLastError ( dwLastError ) ;
    return ( RealAssert ( dwOverrideOpts , szBuff , TRUE ) ) ;
}

// Turn off unreachable code error after ExitProcess.
#pragma warning ( disable : 4702 )

// The code that does the real assertion work.
BOOL __stdcall RealAssert  ( DWORD  dwOverrideOpts  ,
                             LPCSTR szMsg           ,
                             BOOL   bAllowHalts      )
{
	if( MagDebug_IsCrashed() )
	{
		/*
		#ifdef _DEBUG
		DebugBreak();
		#endif
		*/

		return FALSE;
	}
	MagDebug_SetCrash( TRUE );

    // The buffer used for the final message text.
    static char  szBuff [ DIAGASSERT_BUFFSIZE ] ;
    // The current position in szBuff ;
    LPSTR  pCurrPos = szBuff ;
    // The module name.
    char   szModName[ MAX_PATH + 1 ] ;
    // The decoded message from FormatMessage
    LPSTR  szFmtMsg = NULL ;
    // The options.
    DWORD  dwOpts = dwOverrideOpts ;
    // The last error value.  (Which is preserved across the call).
    DWORD  dwLastErr = GetLastError ( ) ;


    if ( MD_DEFAULT == dwOverrideOpts )
    {
        dwOpts = g_DiagAssertOptions ;
    }

    // Look in any specified modules for the code.
    HINSTVECTOR::iterator loop ;
    for ( loop =  g_HMODVector.begin ( ) ;
          loop != g_HMODVector.end ( )   ;
          loop++                          )
    {
        if ( 0 != FormatMessageA ( FORMAT_MESSAGE_ALLOCATE_BUFFER    |
                                     FORMAT_MESSAGE_IGNORE_INSERTS   |
                                     FORMAT_MESSAGE_FROM_HMODULE      ,
                                   *loop                              ,
                                   dwLastErr                          ,
                                   0                                  ,
                                   (LPSTR)&szFmtMsg                   ,
                                   0                                  ,
                                   NULL                               ))
        {
            break ;
        }
    }

    // If the message was not translated, just look in the system.
    if ( NULL == szFmtMsg )
    {
        FormatMessageA ( FORMAT_MESSAGE_ALLOCATE_BUFFER    |
                           FORMAT_MESSAGE_IGNORE_INSERTS   |
                           FORMAT_MESSAGE_FROM_SYSTEM        ,
                         NULL                                ,
                         dwLastErr                           ,
                         0                                   ,
                         (LPSTR)&szFmtMsg                    ,
                         0                                   ,
                         NULL                                 ) ;
    }

    // Make sure the message got translated into something.
    LPSTR szRealLastErr ;
    if ( NULL != szFmtMsg )
    {
        szRealLastErr = szFmtMsg ;
    }
    else
    {
        szRealLastErr = "**Last error code does not exist!!!!" ;
    }

    // Get the module name.
    if ( 0 == GetModuleFileNameA ( NULL , szModName , MAX_PATH ) )
    {
        strncpy ( szModName , "<unknown application>" , MAX_PATH ) ;
    }

    // Build the message.
    pCurrPos += (wsprintfA ( szBuff                         ,
                             "Debug Assertion Failed!\n\n"
                             "Program : %s\n"
                             "%s\n"
                             "Last Error (0x%08X) : %s\n"   
							 "------- Message -------\n%s\n",
                             szModName                      ,
                             szMsg                          ,
                             dwLastErr                      ,
                             szFmtMsg                       ,
							 MD_GetErrorMessage()			) ) ;

    // Get rid of the allocated memory from FormatMessage.
    if ( NULL != szFmtMsg )
    {
        LocalFree ( (LPVOID)szFmtMsg ) ;
    }

    // If the file handle is something, write to it.  I do not do any
    // error checking on purpose.
    if ( INVALID_HANDLE_VALUE != g_hAssertFile )
    {
        DWORD dwWritten ;
        WriteFile ( g_hAssertFile       ,
                    szBuff              ,
                    lstrlenA ( szBuff ) ,
                    &dwWritten          ,
                    NULL                 ) ;
    }

    // Check out the kind of buttons I am supposed to do.
    UINT uiType = MB_TASKMODAL | MB_SETFOREGROUND | MB_ICONERROR ;
    if ( TRUE == bAllowHalts )
    {
        uiType |= MB_ABORTRETRYIGNORE ;
    }
    else
    {
        uiType |= MB_OK ;
    }

    // By default, treat the return as an IGNORE.  This works best in
    // the case the user does not want the MessageBox.
    int iRet = IDIGNORE ;
    if ( MD_SHOWMSGBOX == ( MD_SHOWMSGBOX & dwOpts ) )
    {
        HWND hWndParent = GetActiveWindow ( ) ;
        if ( NULL != hWndParent )
        {
            hWndParent = GetLastActivePopup ( hWndParent ) ;
        }
        iRet = MessageBoxA ( NULL             ,
                            szBuff                  ,
                            "ASSERTION FAILURE..."  ,
                            uiType                   ) ;
    }

    // Put the incoming last error back.
    SetLastError ( dwLastErr ) ;

    // Figure out what to do on the return.
    if ( ( IDIGNORE == iRet ) || ( IDOK == iRet ) )
    {
		MagDebug_SetCrash( FALSE );
        return ( FALSE ) ;
    }
    if ( IDRETRY == iRet )
    {
        // This will trigger DebugBreak!!
		MagDebug_SetCrash( FALSE );
        return ( TRUE ) ;
    }

    // The return has to be Abort....
	MagDebug_SetCrash( FALSE );
    ExitProcess ( (UINT)-1 ) ;
    return ( TRUE ) ;
}
// Turn on unreachable code error
#pragma warning ( default : 4702 )

HANDLE __stdcall
    SetDiagOutputFile ( HANDLE hFile )
{
    HANDLE hRet = g_hTraceFile ;
    g_hTraceFile = hFile ;
    return ( hRet ) ;
}

void 
    DiagOutputA ( LPCSTR szFmt , ... )
{
	if( g_bUseTrace )
	{
		// Never corrupt the last error value.
		DWORD dwLastError = GetLastError ( ) ;

		static char szOutBuff [ 1024 ] ;

		va_list  args ;

		va_start ( args , szFmt ) ;

		wvsprintfA ( szOutBuff , szFmt , args ) ;

		OutputDebugStringA ( szOutBuff ) ;

		if ( INVALID_HANDLE_VALUE != g_hTraceFile )
		{
			DWORD dwWritten ;
			WriteFile ( g_hTraceFile           ,
						szOutBuff              ,
						lstrlenA ( szOutBuff ) ,
						&dwWritten             ,
						NULL                    ) ;
		}

		va_end ( args ) ;

		SetLastError ( dwLastError ) ;
	}
}

void 
    DiagOutputW ( LPCWSTR szFmt , ... )
{
	if( g_bUseTrace )
	{
		// Never corrupt the last error value.
		DWORD dwLastError = GetLastError ( ) ;

		static wchar_t szOutBuff [ 1024 ] ;

		va_list  args ;

		va_start ( args , szFmt ) ;

		wvsprintfW ( szOutBuff , szFmt , args ) ;

		OutputDebugStringW ( szOutBuff ) ;

		if ( INVALID_HANDLE_VALUE != g_hTraceFile )
		{
			DWORD dwWritten ;
			WriteFile ( g_hTraceFile           ,
						szOutBuff              ,
						lstrlenW ( szOutBuff ) ,
						&dwWritten             ,
						NULL                    ) ;
		}

		va_end ( args ) ;

		SetLastError ( dwLastError ) ;
	}
}

#define MAX_BUF	655350
//
//int	AuLogFileEx(char *fname, char *fmt, ...)
//{
//	int		bufcnt;
//	FILE*	fp;
//	int		tmp_len;
//
//	char	strErrorMessageBuf[MAX_BUF];
//	//char*	tmpfile;
//	//char	fileext[8];
//	//int		result;
//	char	filename[128];
//	//SYSTEMTIME	timefile;
//	//char	strTimeInfo[16];
//
//	va_list         ap;
//
//	if(fname == NULL)
//	{
//		return -1;
//	}
//
//	tmp_len = (int)strlen(fname);
//
//	if(tmp_len <= 0)
//	{
//		// 에러메시지가 없다.
//		return -1;
//	}
//
//	strncpy(filename, fname, 128 - 1);
//
//	/*
//	GetLocalTime(&timefile);
//
//	sprintf(strTimeInfo, "-%d%02d%02d", timefile.wYear, timefile.wMonth, timefile.wDay);
//
//	tmpfile = strchr(filename, '.');
//	strcpy(fileext, tmpfile);
//	result = tmpfile - filename;
//	filename[result] = '\0';
//	strcat(filename, strTimeInfo);
//	strcat(filename, fileext);
//	*/
//
//	//. 2006. 5. 4. nonstopdj
//	//. append가 아닌 기능의 log파일~
//	fp = fopen(filename, "w");
//	if ( fp == NULL) 
//	    return 0 ;  
//
//	::ZeroMemory(strErrorMessageBuf, sizeof(char) * MAX_BUF);
//
//	va_start(ap, fmt);
//	bufcnt = vsprintf(strErrorMessageBuf, fmt, ap);
//	va_end(ap);
//
//	if (bufcnt >= MAX_BUF)
//	{
//		fclose(fp);
//		return 0;
//	}
//
//	if ( strErrorMessageBuf[ strlen( strErrorMessageBuf ) -1 ] != '\n' )
//	{
//		int tmp = (int)strlen( strErrorMessageBuf ) ;
//		strErrorMessageBuf[ tmp ] = '\n';
//		strErrorMessageBuf[ tmp + 1] = NULL;
//	}
//
//	fprintf(fp, strErrorMessageBuf );
//
//	if( g_bUseTrace )
//		OutputDebugString( strErrorMessageBuf );
//
//	fclose(fp);
//
//	return 0;
//}

#include "ApMutualEx.h"

//int	AuLogFile(char *fname, char *fmt, ...)
//{
//	int		bufcnt;
//	FILE*	fp;
//	int		tmp_len;
//	char	strErrorMessageBuf[4*1024];
//	char	strErrorMessageBuf2[4*1024];
//	char*	tmpfile;
//	char	fileext[8];
//	int		result;
//	char	filename[128];
//	SYSTEMTIME	timefile;
//	char	strTimeInfo[16];
//
//	va_list         ap;
//
//	if(fname == NULL)
//	{
//		return -1;
//	}
//
//	tmp_len = (int)strlen(fname);
//
//	if(tmp_len <= 0)
//	{
//		// 에러메시지가 없다.
//		return -1;
//	}
//
//	ApMutualEx m_Mutex;
//	AuAutoLock pLock(m_Mutex);
//	if(!pLock.Result())
//		return -1;
//
//	strncpy(filename, fname, 128 - 1);
//
//	GetLocalTime(&timefile);
//
//	sprintf(strTimeInfo, "-%d%02d%02d", timefile.wYear, timefile.wMonth, timefile.wDay);
//
//	tmpfile = strchr(filename, '.');
//	strcpy(fileext, tmpfile);
//	result = tmpfile - filename;
//	filename[result] = '\0';
//	strcat(filename, strTimeInfo);
//	strcat(filename, fileext);
//
//	fp = fopen(filename, "a");
//	if ( fp == NULL) 
//	    return 0 ;  
//
//	::ZeroMemory(strErrorMessageBuf, sizeof(strErrorMessageBuf));
//	::ZeroMemory(strErrorMessageBuf2, sizeof(strErrorMessageBuf2));
//
//	va_start(ap, fmt);
//	bufcnt = vsprintf(strErrorMessageBuf, fmt, ap);
//	va_end(ap);
//
//	if (bufcnt >= 4*1024)
//	{
//		fclose(fp);
//		return 0;
//	}
//
//	if ( strErrorMessageBuf[ strlen( strErrorMessageBuf ) -1 ] != '\n' )
//	{
//		int tmp = (int)strlen( strErrorMessageBuf ) ;
//		strErrorMessageBuf[ tmp ] = '\n';
//		strErrorMessageBuf[ tmp + 1] = NULL;
//	}
//	
//	sprintf(strErrorMessageBuf2, "[%02d:%02d:%02d] %s", timefile.wHour, timefile.wMinute, timefile.wSecond, strErrorMessageBuf);
//
//	fprintf(fp, strErrorMessageBuf2 );
//
//	if( g_bUseTrace )
//		OutputDebugString( strErrorMessageBuf2 );
//
//	fclose(fp);
//
//	return 0;
//}

#include <fstream>
int AuLogFile_s(char* strFileName, char* pData)
{
	CTime cur = CTime::GetCurrentTime();

	std::ofstream m_LogFile;
	char FileName[FILENAME_MAX] = { 0, };
	sprintf_s(FileName, sizeof(FileName), "%s-%04d%02d%02d.txt", strFileName, cur.GetYear(), cur.GetMonth(), cur.GetDay());
	m_LogFile.open( FileName, std::ios_base::out | std::ios_base::app);

	char strLog[1024] = { 0, };
	sprintf_s(strLog, sizeof(strLog), "[%02d:%02d:%02d] %s\n", cur.GetHour(), cur.GetMinute(), cur.GetSecond(), pData);

	m_LogFile << strLog;
	m_LogFile.close();

	return 0;
}

// 파일 이름 뒤에 날짜 붙는 거 뺀 버전
// 2004.01.16. 김태희
//int	AuLogFile2(char *fname, char *fmt, ...)
//{
//	int		bufcnt;
//	FILE*	fp;
//	int		tmp_len;
//	char	strErrorMessageBuf[MAX_BUF];
//	//char*	tmpfile;
//	//char	fileext[8];
//	//int		result;
//	char	filename[128];
//	//SYSTEMTIME	timefile;
//	//char	strTimeInfo[16];
//
//	va_list         ap;
//
//	if(fname == NULL)
//	{
//		return -1;
//	}
//
//	tmp_len = (int)strlen(fname);
//
//	if(tmp_len <= 0)
//	{
//		// 에러메시지가 없다.
//		return -1;
//	}
//
//	ZeroMemory(filename, sizeof(char) * 128);
//	strncpy(filename, fname, 127);
//
//	fp = fopen(filename, "a");
//	if ( fp == NULL) 
//	    return 0 ;  
//
//	va_start(ap, fmt);
//	bufcnt = vsprintf(strErrorMessageBuf, fmt, ap);
//	va_end(ap);
//
//	if (bufcnt >= MAX_BUF)
//	{
//		fclose(fp);
//		return 0;
//	}
//
//	if ( strErrorMessageBuf[ strlen( strErrorMessageBuf ) -1 ] != '\n' )
//	{
//		int tmp = (int)strlen( strErrorMessageBuf ) ;
//		strErrorMessageBuf[ tmp ] = '\n';
//		strErrorMessageBuf[ tmp + 1] = NULL;
//	}
//
//	fprintf(fp, strErrorMessageBuf );
//	if( g_bUseTrace )
//		OutputDebugString( strErrorMessageBuf );
//
//	fclose(fp);
//
//	return 0;
//}

//////////////////////////////////////////////////////////////////////////
// 마고자 (2004-09-30 오후 5:58:21) : 
// 에러 로깅 시스템..
//////////////////////////////////////////////////////////////////////////

//@{ Jaewon 20050701
// 1024 -> 5120
#define MD_MAX_ERROR_BUFFER_SIZE	5120
//@} Jaewon

static char	g_str_MD_Error_Buffer[ MD_MAX_ERROR_BUFFER_SIZE ] = "";
static int	g_n_MD_Error_Count = 0;

BOOL	MD_SetErrorMessage( char *fmt , ... )
{
	// 메시지 필요할경우 다시 넣자.
	return TRUE;

	// 에러 메시지 설정함..
	int		bufcnt;
	char	strErrorMessageBuf[MAX_BUF];

	va_list         ap;

	::ZeroMemory(strErrorMessageBuf, sizeof(char) * MAX_BUF);

	va_start(ap, fmt);
	bufcnt = vsprintf(strErrorMessageBuf, fmt, ap);
	va_end(ap);

	//@{ Jaewon 20050610
	// MAX_BUF -> MAX_BUF-2
	if (bufcnt >= MAX_BUF-2)
	{
		return FALSE;
	}
	//@} Jaewon

	// 맨끝줄 엔터 추가..
	//@{ Jaewon 20050610
	// \n -> \r\n
	if ( strlen( strErrorMessageBuf ) > 1 && 
		(strErrorMessageBuf[ strlen( strErrorMessageBuf ) -2 ] != '\r'
		|| strErrorMessageBuf[ strlen( strErrorMessageBuf ) -1 ] != '\n') )
	{
		int tmp = (int)strlen( strErrorMessageBuf ) ;
		if(strErrorMessageBuf[tmp-1] == '\n')
			--tmp;
		strErrorMessageBuf[ tmp ] = '\r';
		strErrorMessageBuf[ tmp + 1] = '\n';
		strErrorMessageBuf[ tmp + 2] = NULL;
	}
	//@} Jaewon

	// 아웃풋 디버그 창에 표시.
	#ifdef _DEBUG
	if( g_bUseTrace )
		OutputDebugString( strErrorMessageBuf );
	#endif

	// 에러로깅 버퍼에 저장해둠..

	int	nSizeLogged			= (int)strlen( g_str_MD_Error_Buffer	);
	int nSizeNeedtobeAdd	= (int)strlen( strErrorMessageBuf	);

	if( nSizeLogged + nSizeNeedtobeAdd >= MD_MAX_ERROR_BUFFER_SIZE )
	{
		int	nOverLapped =( nSizeLogged + nSizeNeedtobeAdd ) - MD_MAX_ERROR_BUFFER_SIZE + 5;

		for( int i = 0 ; i < MD_MAX_ERROR_BUFFER_SIZE - nOverLapped ; ++ i )
		{
			g_str_MD_Error_Buffer[ i ] = g_str_MD_Error_Buffer[ i + nOverLapped ];
		}
	}

	strcat( g_str_MD_Error_Buffer , strErrorMessageBuf );
	ASSERT( strlen(g_str_MD_Error_Buffer) < sizeof(g_str_MD_Error_Buffer));

	// 에러 카운트 증가.
	g_n_MD_Error_Count++;

	return 0;
}
char *	MD_GetErrorMessage()
{
	// 에러 메시지 얻어냄..
	return g_str_MD_Error_Buffer;
}
int		MD_GetErrorMessageCount()
{
	// 에러메시지가 몇개나 발생했는지점검.. 
	return g_n_MD_Error_Count;
}
BOOL	MD_FlushErrorMessage()
{
	// 에러로그를 비움.
	g_n_MD_Error_Count = 0;
	strncpy( g_str_MD_Error_Buffer , "" , MD_MAX_ERROR_BUFFER_SIZE );
	return TRUE;
}

void	MD_ErrorCheck()
{
	static BOOL _sbShowError = TRUE;
	static BOOL _bMessageBoxOn = FALSE;

	if( !_bMessageBoxOn && _sbShowError && MD_GetErrorMessageCount() )
	{
		_bMessageBoxOn = TRUE;
		// 마고자 (2004-12-02 오후 6:55:41) : 별로 의미없는거 같아서 그냥 해버림..
		// if( IDYES == MessageBox( g_hMessageBoxParentWnd , "에러메시지가 발생했어요. 확인할래요?" , "맵툴 디버그 정보" , MB_YESNO ) )
		{
			MessageBox( g_hMessageBoxParentWnd ,  MD_GetErrorMessage() , "에러메시지" , MB_OK );

			MD_FlushErrorMessage();
		}
//		else
//		{
//			// 에러 표시하지 않음..
//			// 날려버림..
//			MD_FlushErrorMessage();
//		}

		_bMessageBoxOn = FALSE;
	}
}

int	stl_printf( std::string str , const char * pFormat , ... )
{
	int		len;
	char	strMessage[MAX_BUF];

	va_list         ap;

	va_start(ap, pFormat);
	len = vsprintf(strMessage, pFormat, ap);
	va_end(ap);

	str = strMessage;

	return len;
}

void	MD_Dump( char * pFileName )
{
	/*

	일단 주석처리..
	당장스지 않으니까..

	char strDumpFileName[ MAX_PATH + 2 ] = "";
	if( !pFileName )
	{
		sprintf( strDumpFileName , "stack.dmp" );
		pFileName = strDumpFileName;
	}

	HANDLE hMiniDumpFile = CreateFile(
		strDumpFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);

	OutputDebugString(_T("writing minidump\r\n"));

	if( hMiniDumpFile )
	{
		MiniDumpWriteDump(
			GetCurrentProcess	()	,
			GetCurrentProcessId	()	,
			hMiniDumpFile			,
			MiniDumpNormal			,
			NULL					,
			NULL					,
			NULL					);
	}

	*/
}

// GetArg.cpp: implementation of the CGetArg class.
//
//////////////////////////////////////////////////////////////////////

#include "ApBase.h"
#include <stdio.h>
#include <string.h>
#include "ApUtil.h"

////////////////////////////////////////////////////////////////////
// Function : CGetArg2
// Return   :
// Created  : 마고자 (2002-05-07 오후 1:45:23)
// Parameter: 
// Note     : 
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////

int		CGetArg2::SetParam( char * pa	, char * deli )
{
	Clear();

	char	seps[]   = " ,\t\n";

	if( deli == NULL )
	{
		deli = seps;
	}

	char	str[ 1024 ];
	strncpy( str , pa , 1024 );

	char	* strToken;
	char	* strBuffer;

	strToken = strtok( str , deli );
	while( strToken )
	{
		// 메모리 할당 & 리스트 삽입.
		strBuffer	= new char [ strlen( strToken ) + 1 ];
		strcpy( strBuffer , strToken );
		list.AddTail( strBuffer );

		strToken = strtok( NULL, deli );
	}

	return GetArgCount();
}
int		CGetArg2::GetParam( int num , char * buf			)
{
	strcpy( buf , list[ num ] );
	return num;
}

char *	CGetArg2::GetParam( int num							)
{
	return list[ num ];
}

int		CGetArg2::GetArgCount()
{
	return list.GetCount();
}

CGetArg2::CGetArg2( char * pa , char * de )
{
	SetParam( pa , de );
}

CGetArg2::CGetArg2()
{
}

CGetArg2::~CGetArg2()
{
	Clear();
}

void CGetArg2::Clear()
{
	while( list.GetHeadNode() )
	{
		delete [] list.GetHeadNode()->GetData();
		list.RemoveHead();
	}
}

UINT32	MakeRGBToUINT32( FLOAT r , FLOAT g , FLOAT b , FLOAT a )
{
	// 실제로 게임에 쓰이는 0~1사이의 RGB값을 숫자 값으려 변화해서
	// 스트리밍의 시간을 조절한다.

	unsigned char	ur,ug,ub,ua;

	UINT32	uRGB;

	// BYTE 값으로 변경..
	ur	= ( unsigned char ) ( 255.0f * r );
	ug	= ( unsigned char ) ( 255.0f * g );
	ub	= ( unsigned char ) ( 255.0f * b );
	ua	= ( unsigned char ) ( 255.0f * a );

	uRGB = DEF_ARGB32( ua , ur , ug , ub );

	return uRGB;
}

bool	ReadUINT32ToRGB( UINT32 uRGB ,FLOAT *r , FLOAT *g , FLOAT *b , FLOAT *a )
{
	unsigned char	ur,ug,ub,ua;

	ur	= DEF_GET_RED	( uRGB );
	ug	= DEF_GET_GREEN	( uRGB );
	ub	= DEF_GET_BLUE	( uRGB );
	ua	= DEF_GET_ALPHA	( uRGB );

	if( r ) *r = ( FLOAT ) ur / 255.0f;
	if( g ) *g = ( FLOAT ) ug / 255.0f;
	if( b ) *b = ( FLOAT ) ub / 255.0f;
	if( a ) *a = ( FLOAT ) ua / 255.0f;

	return TRUE;
}


namespace memory
{
	InstanceMemoryBlock	* InstanceFrameMemory::_pCurrentMemoryBlock = NULL;

	void * InstanceFrameMemory::operator new( size_t size )
	{
		return _pCurrentMemoryBlock->GetMemory( size );
	}
};
