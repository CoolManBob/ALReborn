/////////////////////////////////////////////////////////////////////////////
// MagDebug			: 디버그용 라이브러리 펑션들.
// 작성				: 마고자
// 일자				: 2002/11/01
/////////////////////////////////////////////////////////////////////////////

// 사용법
//---------------------------------------------------------------------------
#ifndef MAGDEBUG_H
#define MAGDEBUG_H

// 2006.02.16. steeple
// ignore deprecated warning message in VS2005
#if _MSC_VER >= 1400
#pragma warning (disable : 4996)
#endif

//#include <crtdbg.h>
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

// Debug Info FileName
#define ALEF_ERROR_FILENAME "LOG\\_ALEF_ERROR_.log"
#define ALEF_STACKWALK_FILENAME "LOG\\AlefClientD.exe.exp.log"

// 로딩 펑션의 사용..

// 디버그에서만 사용할경우 TRACEFILE( "filename.txt" , fmt , arg1 , arg2 , ... );
// 릴리즈에서도 사용할 경우 AuLogFile( "filename.txt" , fmt , arg1 , arg2 , ... );

enum	MAGDEBUG_OPTION
{
	MD_DEFAULT			= 0x0000,
	MD_SHOWMSGBOX		= 0x0001,	// Use Message Box Output	, by Default
	MD_SHOWODS			= 0x0002,	// Use OutputDebugString	, by Default
	MD_SHOWSTACKTRACE	= 0x0004	// Show Stack Trace
};

DWORD	__stdcall SetAssertOptions	( DWORD dwOpts ) ;	// Set Options..
HANDLE	__stdcall SetAssertFile		( HANDLE hFile ) ;	// Set Assert Output File
HANDLE	__stdcall SetOutputFile		( HANDLE hFile ) ;	// Set Trace Output File

// Assert Funtions.... you do not call this directly
BOOL __stdcall	DiagAssertA ( DWORD dwOverrideOpts , LPCSTR	szMsg	, LPCSTR szFile , DWORD dwLine ) ;
BOOL __stdcall	DiagAssertW ( DWORD dwOverrideOpts , LPCWSTR szMsg	, LPCSTR szFile , DWORD dwLine ) ;

//int	AuLogFile(char *fname, char *fmt, ...);
int AuLogFile_s(char* strFileName, char* pData);
//int	AuLogFile2(char *fname, char *fmt, ...);
//int	AuLogFileEx(char *fname, char *fmt, ...);

// 마고자 (2004-06-25 오후 4:17:09) : 어설트창이 떠있는 중인가.. 설정..
BOOL	MagDebug_IsCrashed();

// 마고자 (2004-09-30 오후 5:54:49) : 
// 디버그 정보 시스템...

BOOL	MD_SetErrorMessage( char *fmt , ... );
	// 에러 메시지 설정함..
char *	MD_GetErrorMessage();
	// 에러 메시지 얻어냄..
int		MD_GetErrorMessageCount();
	// 에러메시지가 몇개나 발생했는지점검.. 
BOOL	MD_FlushErrorMessage();
	// 에러로그를 비움.
void	MD_ErrorCheck();
	// 에러체크해서 있는경우 메시지박스를 띄움
void	MD_SetMainWindowHandle( HWND hWnd );
	// 메시지 박스의 페어런트 지정.

void	MD_Dump( char * pFileName = NULL);

#ifdef UNICODE
#define DiagAssert  DiagAssertW
#else
#define DiagAssert  DiagAssertA
#endif

// Trace Functions..
void DiagOutputA	( LPCSTR szFmt	, ... );
void DiagOutputW	( LPCWSTR szFmt	, ... );

#ifdef UNICODE
#define DiagOutput  DiagOutputW
#else
#define DiagOutput  DiagOutputA
#endif

#ifdef ASSERT
#undef ASSERT
#endif

#ifdef assert
#undef assert
#endif

#ifdef VERIFY
#undef VERIFY
#endif
#ifdef TRACE
#undef TRACE
#endif

#ifdef TRACE0
#undef TRACE0
#endif
#ifdef TRACE1
#undef TRACE1
#endif
#ifdef TRACE2
#undef TRACE2
#endif
#ifdef TRACE3
#undef TRACE3
#endif

#ifdef	ASSERT_VALID_POINTER
#undef	ASSERT_VALID_POINTER
#endif

#ifdef _DEBUG
#ifdef _M_X64
#define ASSERTMACRO(a,x)                                            \
    do                                                              \
    {                                                               \
        if ( !(x)                                               &&  \
             DiagAssert ( a , _T ( #x ) , __FILE__  , __LINE__)    )\
        {                                                           \
                __debugbreak();                                    \
        }                                                           \
    } while (0)
#else
#define ASSERTMACRO(a,x)                                            \
    do                                                              \
    {                                                               \
        if ( !(x)                                               &&  \
             DiagAssert ( a , _T ( #x ) , __FILE__  , __LINE__)    )\
        {                                                           \
                __asm int 3                                         \
        }                                                           \
    } while (0)
#endif

#define ASSERTE(x)		ASSERT(x)
#define ASSERT(x)		ASSERTMACRO ( MD_DEFAULT , x )
#define assert			ASSERT
#define VERIFY(x)		ASSERT(x)
#define SUPERASSERT(x)	ASSERTMACRO ( MD_SHOWSTACKTRACE | MD_SHOWMSGBOX | MD_SHOWODS , x )

#define	ASSERT_VALID_POINTER(x,classtype) ASSERT( !IsBadReadPtr( ( x ) , sizeof ( classtype ) ) );

#define	ASSERTONCE( x )	ASSERTONCEMACRO( _assert_ref_value_ , x )

#define	ASSERTONCEMACRO( assert_ref , x )	\
	do										\
    {										\
		static bool assert_ref = true;		\
		if( assert_ref && !( x ) )			\
		{									\
			assert_ref = false;				\
			ASSERT(x);						\
		}									\
    } while (0)



#define SETDIAGASSERTOPTIONS(x)	SetAssertOptions(x)

// The TRACE macros
#define TRACE					::DiagOutput
#define TRACE0(sz)              DiagOutput(_T("%s"), _T(sz))
#define TRACE1(sz, p1)          DiagOutput(_T(sz), p1)
#define TRACE2(sz, p1, p2)      DiagOutput(_T(sz), p1, p2)
#define TRACE3(sz, p1, p2, p3)  DiagOutput(_T(sz), p1, p2, p3)

//#define	TRACEFILE				::AuLogFile
//#define	TRACEFILE2				::AuLogFile2

#else   // !_DEBUG

#define ASSERTMACRO(a,x)
#define ASSERT(x)
#define ASSERTE(x)
#define VERIFY(x)   ((void)(x))
#define SUPERASSERT(x)
#define SETDIAGASSERTOPTIONS(x)
#define	ASSERT_VALID_POINTER(x,classtype)
#define	ASSERTONCE( x )	

#define TRACE   
#define TRACE0(fmt)
#define TRACE1(fmt,arg1)
#define TRACE2(fmt,arg1,arg2)
#define TRACE3(fmt,arg1,arg2,arg3)

#define	TRACEFILE				
#define	TRACEFILE2				

#endif	//!_DEBUG

inline	char *	GetFileNameOnly( char * pPathName )
{
	static char	strDestFile[ 256 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( pPathName , drive, dir, fname, ext );
	int		nlength	= sprintf( strDestFile , "%s.log" , fname );
	ASSERT(nlength < 256);

	return strDestFile;
}

#define	__FILENAME__	GetFileNameOnly( __FILE__ )

//@{ Jaewon 20051020
#include <ctime>
#include <sstream>
#include <string>
class AuStopWatch
{
public:
	AuStopWatch(const std::string& title) : currentTime_(0)
	{
		startTime_ = clock();
		OutputDebugString(title.c_str());
	}
	~AuStopWatch()
	{
		output_ << " || " << double(clock() - startTime_) / double(CLOCKS_PER_SEC) << std::endl;
		OutputDebugString(output_.str().c_str());
	}

	void start()
	{
		currentTime_ = clock();
	}
	void stop(const std::string& message)
	{
		output_ << " | " << message << double(clock() - currentTime_) / double(CLOCKS_PER_SEC);
	}
private:
	clock_t startTime_, currentTime_;
	std::stringstream output_;
};

#ifdef _DEBUG
#define STOPWATCH(title)			AuStopWatch _StopWatch_(title)
#define STOPWATCH_START				_StopWatch_.start()
#define STOPWATCH_STOP(message)		_StopWatch_.stop(message)
#else
#define STOPWATCH(title)
#define STOPWATCH_START
#define STOPWATCH_STOP(message)
#endif
//@} Jaewon



// 2007.02.28. steeple
// Release 에서 사용하게끔 만듬 stop watch. file 로 남긴다.
#include "AuTimeStamp.h"

class AuStopWatch2
{
public:
	AuStopWatch2(const std::string& filename, const std::string& title , clock_t boundarytime = 150 ) : currentTime_(0), hFile_(INVALID_HANDLE_VALUE) , boundarytime_ ( boundarytime )
	{
		startTime_ = clock();
		filename_ << "slowtick_" << filename << ".log";
		title_ = title;
	}

	~AuStopWatch2()
	{
		clock_t cElapsed = clock() - startTime_;
		if(cElapsed > boundarytime_)
		{
			output_ << currentDate() << " " << title_ << " elapsed tick : " << cElapsed << " ms" << "\r\n";;
			writeLog();
			closeFile();
		}
	}

	void start()
	{
		currentTime_ = clock();
	}

	void stop(const std::string& message)
	{
		clock_t cElapsed = clock() - currentTime_;
		if(cElapsed > (clock_t)boundarytime_)
		{
			output_ << currentDate() << " " << message << " elapsed tick : " << cElapsed << " ms" << "\r\n";;
			writeLog();
			closeFile();
			output_.clear();
		}			
	}

private:
	clock_t				startTime_, currentTime_ , boundarytime_;
	std::stringstream	output_;
	std::stringstream	filename_;
	std::string			title_;
	HANDLE				hFile_;

	const std::string currentDate()
	{
		TCHAR szTmp[32];
		memset(szTmp, 0, sizeof(szTmp));
		UINT64 ullTimeStamp = AuTimeStamp2::GetCurrentTimeStamp();
		if(AuTimeStamp2::ConvertTimeStampToOracleTime(ullTimeStamp, szTmp, 32) > 0)
			return szTmp;
		else
			return _T("");
	}

	void openFile()
	{
		hFile_ = CreateFile(filename_.str().c_str(),
							GENERIC_READ | GENERIC_WRITE, 0, NULL,
							OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile_ == INVALID_HANDLE_VALUE)
			return;
	}

	void closeFile()
	{
		if(hFile_ != INVALID_HANDLE_VALUE)
			CloseHandle(hFile_);

		hFile_ = INVALID_HANDLE_VALUE;
	}

	void writeLog()
	{
		if(hFile_ == INVALID_HANDLE_VALUE)
			openFile();

		if(hFile_ == INVALID_HANDLE_VALUE)
			return;

		SetFilePointer(hFile_, 0, 0, FILE_END);
		
		DWORD dwWritten = 0;
		WriteFile(hFile_, output_.str().c_str(), (DWORD)output_.str().length(), &dwWritten, NULL);
	}
};

//#define USE_STOPWATCH2

#ifdef USE_STOPWATCH2
	#define STOPWATCH2TIME(filename, title ,time)	AuStopWatch2 _StopWatch2_(filename, title , time)
	#define STOPWATCH2(filename, title)				AuStopWatch2 _StopWatch2_(filename, title)
	#define STOPWATCH2_START					_StopWatch2_.start()
	#define STOPWATCH2_STOP(message)			_StopWatch2_.stop(message)
#else
	#define STOPWATCH2TIME(filename, title ,time)
	#define STOPWATCH2(filename, title)		
	#define STOPWATCH2_START
	#define STOPWATCH2_STOP(message)
#endif

// DebugValue Logging System..
// 2007/06/11 Magoja

class DebugValue
{
public:
	virtual void LogString( char * pString ) = 0;
};

class DebugValueManager
{
public:
	vector< DebugValue * > vecValue;

	static DebugValueManager * GetInstance()
	{
		// Singleton
		static DebugValueManager stThis;
		return & stThis;
	}

	void	AddValue( DebugValue * pValue )
	{
		vecValue.push_back( pValue );
	}
};
class DebugValueINT32 : public DebugValue
{
public:
	DebugValueINT32( INT32 nInitValue , const char * pFmt )
	{
		nValue = nInitValue;
		strFmt = pFmt;

		// 메니져에 포인터 등록..
		DebugValueManager * pManager = DebugValueManager::GetInstance();
		pManager->AddValue( this );
	}

	string	strFmt;
	INT32	nValue;

	virtual void LogString( char * pString )
	{
		sprintf( pString , strFmt.c_str() , nValue );
	}

	void	operator=( INT32 nValueInput )
	{
		nValue = nValueInput;
	}
};

class DebugValueUINT32 : public DebugValue
{
public:
	DebugValueUINT32( UINT32 nInitValue , const char * pFmt )
	{
		nValue = nInitValue;
		strFmt = pFmt;

		// 메니져에 포인터 등록..
		DebugValueManager * pManager = DebugValueManager::GetInstance();
		pManager->AddValue( this );
	}

	string	strFmt;
	UINT32	nValue;

	virtual void LogString( char * pString )
	{
		sprintf( pString , strFmt.c_str() , nValue );
	}

	void	operator=( UINT32 nValueInput )
	{
		nValue = nValueInput;
	}
};

class DebugValueFloat : public DebugValue
{
public:
	DebugValueFloat( FLOAT fInitValue , const char * pFmt )
	{
		fValue = fInitValue;
		strFmt = pFmt;

		// 메니져에 포인터 등록..
		DebugValueManager * pManager = DebugValueManager::GetInstance();
		pManager->AddValue( this );
	}

	string	strFmt;
	FLOAT	fValue;

	virtual void LogString( char * pString )
	{
		sprintf( pString , strFmt.c_str() , fValue );
	}

	void	operator=( FLOAT fValueInput )
	{
		fValue = fValueInput;
	}
};

class DebugValueString : public DebugValue
{
public:
	DebugValueString( const char * pInitValue , const char * pFmt )
	{
		strValue	= pInitValue	;
		strFmt		= pFmt			;

		// 메니져에 포인터 등록..
		DebugValueManager * pManager = DebugValueManager::GetInstance();
		pManager->AddValue( this );
	}

	string	strFmt	;
	string	strValue;

	virtual void LogString( char * pString )
	{
		sprintf( pString , strFmt.c_str() , strValue.c_str() );
	}

	void	operator=( const char * pValueInput )
	{
		strValue = pValueInput;
	}
};

#endif // MAGDEBUG_H
