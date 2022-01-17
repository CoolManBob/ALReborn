// AuTickCounter.h: interface for the AuTickCounter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTICKCOUNTER_H__8326B3FC_C6C3_403E_9644_9F66835158DF__INCLUDED_)
#define AFX_AUTICKCOUNTER_H__8326B3FC_C6C3_403E_9644_9F66835158DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AuTickCounterD" )
#else
#pragma comment ( lib , "AuTickCounter" )
#endif
#endif


class AuTickCounter  
{
private:
//	HANDLE	m_hThread				;
	UINT64	m_ullTickPerMillisecond	;
	//UINT64	m_ullLastElapsedTime;
	UINT32 m_ullLastElapsedTime;

// 	static	UINT WINAPI CalcTPM(void *lpParameter);
// 	static	_inline UINT64 Rdtsc();
public:
	float GetElapsedTime();
	UINT32 GetTickCount();
	void Initialize();
	AuTickCounter();
	virtual ~AuTickCounter();

};

#endif // !defined(AFX_AUTICKCOUNTER_H__8326B3FC_C6C3_403E_9644_9F66835158DF__INCLUDED_)
