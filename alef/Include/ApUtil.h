#pragma once

// 독립적으로 사용할수 있는 유틸리티 펑션 & 클래스를 모아두는 헤더.
// 필요할경우 따로 Include 시켜서 사용하세요
//
// 마고자 (2005-08-10 오후 2:03:48)


#include "MagDebug.h"

class IgnoreErr{};
template <class T>
class ForcedErrChk
{
	mutable bool read_;
	T			 code_;

public:
	ForcedErrChk(const T& code)
		: read_(false), code_(code)	{};
	ForcedErrChk(const ForcedErrChk& cpy)
		: read_( cpy.read_), code_(cpy.code_)	{
		cpy.read_ = true;
	};

	operator T() {
		read_ = true;
		return code_;
	};

	operator IgnoreErr() {
		read_ = true;
		return IgnoreErr();
	};

	~ForcedErrChk()	{
		ASSERT( "check the errcode, please!" && read_ );
	};
};

//// ex : ForcedErrChk
//////////////////////////////////////////
//	ForcedErrChk<int>	func();
//
//	int res = func();				//good
//	if( func() ){}				//good
//	func();						//assert!
//
//	// there is no assert but don't use like this..
//	!func();
//	(int)func();
//	int dummy = func();
//	// use like this for ignore the errcode...
//	(IgnoreErr)func();

#define	DEFAULT_PARAMETER_SIZE	1024
#include "AuList.h"


// 마고자 (2002-05-07 오후 1:44:35) : CGetArg2 ... 훨씬 빠르다. .. 훨씬 간단하다...
class CGetArg2  
{
private:
	AuList< char * >	list;
public:
	int		SetParam( char * pa	, char * deli = NULL	);// 파라미터를 새로 설정한다.
	int		GetParam( int num , char * buf				);// 파라미터를 얻어낸다. num은 0 based
	char *	GetParam( int num							);

	int		GetArgCount();// 파라미터의 개수 리턴 

	void	Clear();

	CGetArg2( char * pa , char * de = NULL );
	CGetArg2();
	virtual ~CGetArg2();
};


namespace profile
{
	class	Timer
	{
	protected:
		UINT32	uStartTime			;
		UINT32	uLastCheckTime		;
		UINT32	uCurrentCheckTime	;

	public:
		Timer():uStartTime( GetTickCount() ) , uLastCheckTime ( uStartTime ) , uCurrentCheckTime ( uStartTime ) {}

		void	Log( const char * pFile , int nLine )
		{
			char	strMessage[ 1024 ];

			uCurrentCheckTime	= GetTickCount();
			UINT32	uDeltaTime			= uCurrentCheckTime - uLastCheckTime;
			sprintf( strMessage , "%s(%d): %.1f초 ( 전체 %.1f초 )\n" , pFile , nLine , ( FLOAT ) uDeltaTime / 1000.0f , ( FLOAT ) ( uCurrentCheckTime - uStartTime ) / 1000.0f );
			OutputDebugString( strMessage );
			uLastCheckTime	= uCurrentCheckTime;
		}
	};
};

namespace memory
{
	class	InstanceMemoryBlock;

	class	InstanceFrameMemory
	{
	public:
		static InstanceMemoryBlock	* _pCurrentMemoryBlock;

		static void * operator new( size_t size );
		static void operator delete( void *p , size_t size )
		{// do nothing
		}

		InstanceFrameMemory(){}
		virtual ~InstanceFrameMemory(){}
	};

	class	InstanceMemoryBlock
	{
	public:
		InstanceMemoryBlock():_pMemoryBlock( NULL ),_pCurrent(NULL), _uSize( 0 )
		{
			InstanceFrameMemory::_pCurrentMemoryBlock	= this;
		}

		virtual ~InstanceMemoryBlock()
		{
			ReleaseMemoryBlock();
			InstanceFrameMemory::_pCurrentMemoryBlock	= NULL;
		}

		bool	CreateMemoryBlock( size_t	uSize )
		{
			ReleaseMemoryBlock();

			_pMemoryBlock	= new	BYTE[ uSize ];

			if( !_pMemoryBlock )
			{
				// 메모리 확보 실패.
				return false;
			}

			_pCurrent		= _pMemoryBlock;
			_uSize			= uSize;
			return true;
		}

		void	ReleaseMemoryBlock()
		{
			if( _pMemoryBlock )	delete [] _pMemoryBlock;
			_pMemoryBlock	= NULL;
			_pCurrent		= NULL;
			_uSize			= 0;
		}

		bool	IsValid( BYTE * pPointer )
		{
			if( _pMemoryBlock <= pPointer &&
				pPointer < _pMemoryBlock + _uSize ) return true;
			else return false;
		}
		
		void	*	GetMemory( size_t uSize )
		{
			BYTE * pReturn = NULL;

			#ifdef _DEBUG
			if( _pCurrent + uSize >= _pMemoryBlock + _uSize )
				return NULL; // memory over
			#endif

			pReturn	= _pCurrent;
			_pCurrent += uSize;
			return ( void * ) pReturn;
		}
	protected:
		
		BYTE	* _pMemoryBlock;
		BYTE	* _pCurrent;
		size_t	_uSize;
	};
};

// 2008/05/13
// 포인터 임시 관리를 위한 오토 포인터.
// 변수의 Scope동안 new한 데이타를 유지해 주기 위한 녀석.
template< typename T>
class AuAutoPtr
{
public:
	AuAutoPtr( T * pPtr = NULL ) : _pPointer( pPtr ) {}

	~AuAutoPtr(){ SafeDestroy(); }

	void	operator=( T * pPtr ){ SafeDestroy(); _pPointer = pPtr; }
	T * operator->() { return _pPointer; }

	bool	operator==( T * pPtr ){ return _pPointer == pPtr ? true : false; }

protected:
	T	*_pPointer;

	void	SafeDestroy() { if( _pPointer ) delete _pPointer; }
};


