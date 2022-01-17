#ifndef  __WBANetwork_H
#define  __WBANetwork_H

#include <crtdbg.h>

#pragma		comment(lib, "ws2_32.lib")
#pragma		comment(lib, "netapi32")
#pragma		comment(lib, "mpr.lib")

#define  __WBANetwork_Ver		0x0000


#ifdef  _CRT_DEBUG
	//  Detection memory leak
	#define  _new_dbg_	new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
	#define  _new_dbg_	new
#endif


//  Declares dll exports
#define  WBANETWORT_API

#if defined(__WBANETWORT_DLLIMPORT)

	#undef   WBANETWORT_API
	#define  WBANETWORT_API		__declspec(dllimport)

#elif defined(_USRDLL)

	#undef   WBANETWORT_API
	#define  WBANETWORT_API		__declspec(dllexport)

#endif



#if  defined( _MSC_VER ) && ( _MSC_VER == 1200 )		//  for vs 6.0 or minor version...
	#pragma  warning(disable:4786)
	#pragma  warning(disable:4251)
#elif  defined( _MSC_VER ) && ( _MSC_VER > 1200 )		//  for vs 7.0 or higher version...
	#pragma  warning(disable:4251)
#endif

#if !defined( _MT )
	#error You must select multi-threaded libraries if you want to use WBANetwork Network Library.
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////
//  Definition IfDebug
#ifndef  IfDebug
	#ifdef  _DEBUG
		#define  IfDebug()		if( true )
	#else
		#define  IfDebug()		if( false )
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
//  Definition IfNotDebug
#ifndef  IfNotDebug
	#ifdef  _DEBUG
		#define  IfNotDebug()	if( false )
	#else
		#define  IfNotDebug()	if( true )
	#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
//  Definition SafeDelete
#ifndef  SafeDelete
	#define  SafeDelete( expr )	\
	{							\
		if( ( expr ) != 0 )		\
			delete ( expr );	\
		expr = 0;				\
	}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
//  Definition Swap
#ifndef  Swap
	#define  Swap( left, right )	( (left) ^= ( (right) ^= ( (left) ^= (right) ) ) )
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
//  Definition CompileTimeNote
#ifndef  CompileTimeNote
	#define  __CTN_Line( x )			#x
	#define  CompileTimeNote( msg )		message( __FILE__ "(" __CTN_Line( __LINE__ ) "): [NOTE] " #msg )
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
//  Definition Singleton
#ifndef  Singleton
#define  Singleton( _T )\
	private:\
				_T();\
		virtual	~_T();\
\
	public:\
	__declspec(noinline)	static	_T*	Instance()\
	{\
		static	_T	selfInstance;\
		\
		return &selfInstance;\
	}
#endif

namespace WBANetwork
{
	//  Error Handling
	typedef	void	( *LPFN_ErrorHandler )( DWORD lastError, TCHAR* desc );
			void	CallbackErrorHandler( DWORD lastError, TCHAR* desc );

			bool	Initialize( TCHAR* mutexName = 0, LPFN_ErrorHandler handler = 0 );
			DWORD	StringToHash( char* target );
};

// WBANetwork error define
#define EXT_ERROR_BASE					WSABASEERR + 10000
#define EXT_ERROR_ZERO_BYTE_RECEIVE		EXT_ERROR_BASE + 1

#endif
