#include "StdAfx.h"
#include "PatchException.h"
#include "MagDebug.h"

#include "./BugSlay/CrashHandler.h"


const DWORD						CPatchException::s_dwOption			=	GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE;
LPTOP_LEVEL_EXCEPTION_FILTER	CPatchException::s_pExceptionFilter	=	NULL;

CPatchException::CPatchException( VOID )
{
	s_pExceptionFilter		=	SetUnhandledExceptionFilter( ExceptionHandler );
}

CPatchException::~CPatchException( VOID )
{
	if( s_pExceptionFilter )
	{
		SetUnhandledExceptionFilter( s_pExceptionFilter );
		s_pExceptionFilter		=	NULL;
	}
}

LONG	__stdcall	CPatchException::ExceptionHandler( EXCEPTION_POINTERS* pExPtrs )
{
	return	RecordExceptionInfo( pExPtrs , "PatchClient" , "PatchClient" , &AddExceptionHandler );
}

const char*		CPatchException::AddExceptionHandler( EXCEPTION_POINTERS* pExPtrs )
{
	// additional stack trace, but this needs the release of pdb
	static char buf[10240];
	const UINT32 limit = sizeof(buf)/sizeof(buf[0])-1;
	buf[limit] = '\0';
	strcpy(buf, "Debug log:\r\n");
	strncat(buf, MD_GetErrorMessage(), limit - strlen(buf));
	strncat(buf, "\r\n", limit-strlen(buf));

	char tmp[1025];
	tmp[1024] = '\0';

	strncat(buf, "\r\n", limit - strlen(buf));
	sprintf(tmp, "Crash Address:\r\n");
	strncat(buf, tmp, limit - strlen(buf));
	sprintf(tmp, "%08x\r\n\r\n" , pExPtrs->ExceptionRecord->ExceptionAddress );
	strncat(buf, tmp, limit - strlen(buf));

	sprintf(tmp, "Stack trace:\r\n");
	strncat(buf, tmp, limit - strlen(buf));

	const char *szBuff = GetFirstStackTraceString( s_dwOption, pExPtrs );
	do
	{
		_snprintf( tmp, sizeof(tmp)/sizeof(tmp[0])-1, "%s\r\n", szBuff );
		strncat( buf, tmp, limit-strlen(buf) );
		szBuff = GetNextStackTraceString( s_dwOption, pExPtrs );
	}
	while(szBuff);


	return buf;
}


