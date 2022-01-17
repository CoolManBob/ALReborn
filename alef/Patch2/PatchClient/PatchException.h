#pragma once

#include "./AuXCrashReport/ExceptionHandler.h"

class CPatchException
{
public:
	CPatchException		( VOID );
	~CPatchException	( VOID );

	static	LONG	__stdcall	ExceptionHandler		( EXCEPTION_POINTERS* pExPtrs );
	static	const char*			AddExceptionHandler		( EXCEPTION_POINTERS* pExPtrs );

private:

	static const DWORD						s_dwOption;
	static LPTOP_LEVEL_EXCEPTION_FILTER		s_pExceptionFilter;

};
