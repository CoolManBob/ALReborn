// AuTickCounter.cpp: implementation of the AuTickCounter class.
//
//////////////////////////////////////////////////////////////////////

#include "ApBase.h"

#include <stdio.h>
#include "ApBase.h"
#include "AuTickCounter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AuTickCounter::AuTickCounter(): m_ullTickPerMillisecond(0),m_ullLastElapsedTime(0)
{
// 	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &m_ullTickPerMillisecond))
// 		m_ullTickPerMillisecond	= 0;
// 
// 	m_hThread				= NULL	;
}

AuTickCounter::~AuTickCounter()
{
// 	if( m_hThread )
// 	{
// 		WaitForSingleObject( m_hThread , INFINITE );
// 
// 		// TerminateThread( m_hThread , 0 );
// //		CloseHandle( m_hThread );
// 	}
}

//Rdtsc로 얻어오는것을 인텔CPU가 아닌 타 플랫폼일경우엔 문제가 있을 소지가 있어 바꿈.
//불필요한 스레드생성도 제거
void AuTickCounter::Initialize()
{
	/*
	LARGE_INTEGER qwTickPerSec, qwTime;

	if( 0 != QueryPerformanceFrequency( &qwTickPerSec ) )
	{
		m_ullTickPerMillisecond	= qwTickPerSec.QuadPart;
		m_ullLastElapsedTime = QueryPerformanceCounter( &qwTime );
	}
	*/

	m_ullLastElapsedTime = ::timeGetTime();


// 	UINT dwThreadID;
// 
// 	if (!m_ullTickPerMillisecond)
// 	{
// 		m_hThread = (HANDLE)_beginthreadex(NULL, 0, AuTickCounter::CalcTPM, &m_ullTickPerMillisecond, 0, &dwThreadID);
// 		SetThreadName(dwThreadID, "AuTickCounter");
// 	}
}
UINT32 AuTickCounter::GetTickCount()
{
	/*
	UINT64 ullTick;
	QueryPerformanceCounter( (LARGE_INTEGER*)&ullTick );

	//이미 코드에서 32비트로 처리중이라 어쩔수 없이 (UINT32)로 형변환함 ...짤릴듯한데 ...쩝 ㅡㅡ;;; supertj@20100208
	return (UINT32)( ullTick * 1000  / m_ullTickPerMillisecond ); 
	*/
	return ::timeGetTime();
}

float AuTickCounter::GetElapsedTime()
{
	/*
	LARGE_INTEGER ullTick;
	QueryPerformanceCounter( &ullTick );

	float fElapsedTime = (float) ( (double)(ullTick.QuadPart - m_ullLastElapsedTime ) / (double)m_ullTickPerMillisecond );

	m_ullLastElapsedTime = ullTick.QuadPart;

	return fElapsedTime; 
	*/

	INT32 curTick = ::timeGetTime();

	float delta = (float)(curTick - m_ullLastElapsedTime);

	m_ullLastElapsedTime = curTick;

	return delta;
}


// _inline UINT64 AuTickCounter::Rdtsc()
// {
// #ifdef _M_X64
// 	// ....................
// 	// ....................
// 	// How can I solve this problem in AMD x64 architecture?
// 	//
// 	//
// 	return 0L;
// #else
// //     _asm    _emit 0x0F
// //     _asm    _emit 0x31
// 	_asm	rdtsc
// #endif
// }
// 
// UINT WINAPI AuTickCounter::CalcTPM(LPVOID lpParameter)
// {
// 	UINT32 *	pulTPM = (UINT32 *) lpParameter;
// 	UINT64		ulTPS;
// 	UINT32		ulTPM;
// 	INT32		lIndex;
// 
// 	*pulTPM = -1;
// 
// 	for (lIndex = 0; lIndex < 5; ++lIndex)
// 	{
// 		ulTPS = AuTickCounter::Rdtsc();
// 		Sleep(1000);
// 		ulTPS = (AuTickCounter::Rdtsc() - ulTPS);
// 
// 		ulTPM = (UINT32) ( ulTPS / 1000000 ) * 1000;
// 
// 		if (*pulTPM > ulTPM)
// 			*pulTPM = ulTPM;
// 	}
// 
// 	ExitThread( 0 );
// 	return 0;
// }
