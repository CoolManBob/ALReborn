#pragma once

#include "ExceptionHandler.h"
#include <winsock2.h>
#include <windows.h>

class MyEngine;
class CMyException
{
public:
	CMyException( MyEngine* pEngine );
	~CMyException();

	static LONG __stdcall ExceptionHandler( EXCEPTION_POINTERS* pExPtrs );
	static const char* AddExceptionHandler( EXCEPTION_POINTERS* pExPtrs );

public:
	static const DWORD s_dwOption;

private:
	static MyEngine* m_pEngine;
	static LPTOP_LEVEL_EXCEPTION_FILTER	m_pExceptionFilter;
};