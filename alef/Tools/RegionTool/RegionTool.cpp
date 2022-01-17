// RegionTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "RegionToolDlg.h"
#include "BugSlay/CrashHandler.h"
#include "MagDebug.h"
#include "dbghelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define	DEFAULT_EXCEPTION_FILENAME	"RegionTool%02d%02d-%02d%02d.txt"

extern MINIDUMP_TYPE	g_eMiniDumpType	;//

DWORD dwOpts = 
	//GSTSO_PARAMS |
	GSTSO_MODULE |
	GSTSO_SYMBOL |
	GSTSO_SRCLINE;

LONG __stdcall ExcepCallBack ( EXCEPTION_POINTERS * pExPtrs )
{
	char	strOutputFile[ 1024 ];
	//DWORD	dwCurrentTime = ::time(NULL);
	::MessageBox( NULL , "프로그램 죽었어요 , 로그 남깁니다~ 시간이 좀 걸릴 수 있어요~" , "리젼툴" , MB_OK | MB_ICONERROR );

	time_t long_time;
	time( &long_time );                /* Get time as long integer. */

	tm * pLocalTime;
	pLocalTime = localtime(  &long_time );

	wsprintf( strOutputFile , DEFAULT_EXCEPTION_FILENAME ,
		pLocalTime->tm_mon + 1	, pLocalTime->tm_mday ,
		pLocalTime->tm_hour	, pLocalTime->tm_min	);

	FILE	* pFile = fopen( strOutputFile , "wt" );
	if( pFile )
	{
		fprintf ( pFile ,  "%s\n" , GetFaultReason ( pExPtrs ) ) ;
		fprintf ( pFile , "%s\n" , GetRegisterString ( pExPtrs ) ) ;
		const char * szBuff = GetFirstStackTraceString ( dwOpts  ,
			pExPtrs  ) ;
		do
		{
			fprintf ( pFile , "%s\n" , szBuff ) ;
			fflush ( pFile ) ;
			szBuff = GetNextStackTraceString ( dwOpts , pExPtrs ) ;
		}
		while ( NULL != szBuff ) ;

		fclose( pFile );
	}

	::MessageBox( NULL , "프로그램 죽었어요 , 로그 남겼습니다~" , "리젼툴" , MB_OK | MB_ICONERROR );
	
	ExitProcess( 1 );
	return ( EXCEPTION_CONTINUE_SEARCH ) ;
}

/////////////////////////////////////////////////////////////////////////////
// CRegionToolApp

BEGIN_MESSAGE_MAP(CRegionToolApp, CWinApp)
	//{{AFX_MSG_MAP(CRegionToolApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionToolApp construction

CRegionToolApp::CRegionToolApp()
{
	// TODO: add construction code here,
	g_eMiniDumpType = MiniDumpNormal;

#ifdef _DEBUG	
	int	nErrorCode = 0;
	if( nErrorCode )
		_CrtSetBreakAlloc( nErrorCode );
#endif

	if( ::CreateMutex( NULL , TRUE , "RegionTool" ) )
	{
		if( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			ExitProcess( 1 );
		}
	}
	else
	{
		// 뭔가 이상~
		ExitProcess( 1 );
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRegionToolApp object

CRegionToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRegionToolApp initialization

BOOL CRegionToolApp::InitInstance()
{
	if ( ( int ) TRUE != SetCrashHandlerFilter ( ExcepCallBack ) )
	{
		TRACE( "크래시 핸들러 설정 실패 -_-;;;\n" );
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CRegionToolDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
