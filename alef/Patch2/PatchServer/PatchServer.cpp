#include "PatchServerIOCP.h"
#include "AuIniManager.h"
#include <signal.h>
#include <new.h>

unsigned short	g_lPatchServerPort = 11000; //아크로드용
unsigned short	g_lPatchSMPort = 11001; //모니터링용

struct ExceptionHandler
{
	LPTOP_LEVEL_EXCEPTION_FILTER org_exception_handler_;
	_invalid_parameter_handler org_invalid_handler_;
	_purecall_handler org_pure_handler_;

	ExceptionHandler::ExceptionHandler()
		: org_exception_handler_(0)
		, org_invalid_handler_(0)
	{
		Sleep(3000);

		org_exception_handler_ = SetUnhandledExceptionFilter( &ExceptionHandler::exception_handler );
		org_invalid_handler_ = _set_invalid_parameter_handler( &ExceptionHandler::invalid_param_handelr );
		org_pure_handler_ = _set_purecall_handler( &ExceptionHandler::pure_handler );

		// windows runtime library error 다이얼로그가 뜨지 않도록함
		signal( SIGABRT, sigabrt_handler );
		signal( SIGSEGV, sigabrt_handler );
		signal( SIGTERM, sigabrt_handler );
		signal( SIGSEGV, sigabrt_handler );
		_set_abort_behavior( 0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT );
		set_terminate( terminal_handler );
		_set_new_mode( 1 );
		_set_new_handler( new_handler );

		SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOOPENFILEERRORBOX );
	}

	ExceptionHandler::~ExceptionHandler( VOID )
	{
		if( org_exception_handler_ )
		{
			SetUnhandledExceptionFilter( org_exception_handler_ );
		}

		if( org_invalid_handler_ )
		{
			_set_invalid_parameter_handler( org_invalid_handler_ );
		}

		if( org_pure_handler_ )
		{
			_set_purecall_handler( org_pure_handler_ );
		}
	}

	static void re_run()
	{
		//system( "patchserver.exe" );
		//ExitProcess(0);
		//system( "re_run.exe patchserver.exe" );
		ShellExecute( 0, "Open", "re_run.exe", "PatchServer.exe", 0, SW_NORMAL );
		ExitProcess(0);
	}

	static LONG __stdcall exception_handler( EXCEPTION_POINTERS* pExPtrs )
	{
		re_run();

		return EXCEPTION_EXECUTE_HANDLER;
	}

	static void invalid_param_handelr( wchar_t const * expression, wchar_t const * function, wchar_t const * file, unsigned int line, uintptr_t pReserved )
	{
		re_run();
	}

	static void __cdecl pure_handler()
	{
		re_run();
	}

	static void __cdecl sigabrt_handler( int value )
	{
		re_run();
	}

	static void __cdecl terminal_handler()
	{
		//re_run();
		ExitProcess( 0 );
	}

	static int __cdecl new_handler( size_t )
	{
		re_run();
		return 0;
	}
};

void main()
{
	ExceptionHandler excpetionHandler;

	try
	{
		g_AuCircularBuffer.Init(300 * 1024 * 1024);
		g_AuCircularOutBuffer.Init(1024 * 1024);

		CIOCPNetworkModule		cIOCP;
		CIOCPNetworkModule		cIOCPSM;

		//PatchCheckCode를 확인한다.
		if( cIOCP.LoadPatchCode( "PatchCode.Dat" ) == false )
		{
			printf( "PatchCode.Dat not found !!!\n" );
			getchar();

			return;
		}

		// Patch Server FTP 주소를 읽어온다.
		AuIniManagerA	csIniManager;

		csIniManager.SetPath("PatchDownload.ini");
		if (csIniManager.ReadFile())
		{
			char *			szServerIP	= csIniManager.GetValue("DownloadServer", "IP");
			unsigned short	iServerPort	= csIniManager.GetValueI("DownloadServer", "Port");

			if (szServerIP)
			{
				printf( "Download Server : %s (%d)\n", szServerIP, iServerPort);
				cIOCP.SetDownloadServer(szServerIP, iServerPort);
			}
		}

		cIOCPSM.m_iVersion	= cIOCP.m_iVersion;

		//Create IOCP
		cIOCP.CreateIOCP( 0 );
		cIOCP.SetIPBlocking( false );

		cIOCP.m_cWinSockLib.startupWinsock( 0x0202 );

		cIOCPSM.CreateIOCP( 0 );
		cIOCPSM.SetIPBlocking( false );

		cIOCPSM.m_cWinSockLib.startupWinsock( 0x0202 );

		/*	char			strHostName[255];
		char			strIP[255];
		HOSTENT			*pcsHostEnt;

		gethostname( strHostName, sizeof(strHostName) );
		pcsHostEnt = gethostbyname( strHostName );

		sprintf( strIP, "%d.%d.%d.%d", (BYTE)pcsHostEnt->h_addr_list[0][0],
		(BYTE)pcsHostEnt->h_addr_list[0][1],
		(BYTE)pcsHostEnt->h_addr_list[0][2],
		(BYTE)pcsHostEnt->h_addr_list[0][3] );

		printf( "Local IP:%s, Port:%d\n", strIP, g_lPatchServerPort );*/

		if( cIOCP.m_cWinSockLib.initBind( true, NULL, g_lPatchServerPort, 0 ) == false )
		{
			printf( "Bind Error\n" );
			//getchar();
			//exit(1);
			ExceptionHandler::re_run();
		}

		if( cIOCPSM.m_cWinSockLib.initBind( true, NULL, g_lPatchSMPort, 0 ) == false )
		{
			printf( "Bind Error\n" );
			//getchar();
			//exit(1);
			ExceptionHandler::re_run();
		}

		//	cIOCP.m_cIPBlockingManager.LoadBlockIPFromFile( "IPBlockData.txt" );

		//소켓과 IOCP를 연결한다.
		cIOCP.AssociateIOCP( (HANDLE)cIOCP.m_cWinSockLib.m_hSocket, (void *)IOCP_ACCEPTEX );

		cIOCPSM.AssociateIOCP( (HANDLE)cIOCPSM.m_cWinSockLib.m_hSocket, (void *)IOCP_ACCEPTEX );

		//Sleep.... Thread구동 시간동안 기다려준다. 나중엔 Event 구조로 변경한다.

		//IOCP와 관련된 Thread를 등록한다.
		cIOCP.registerThread( &CIOCPNetworkModule::IOCPGQCSThread );
		cIOCP.registerThread( &CIOCPNetworkModule::IOCPSendThread );
		cIOCP.registerThread( &CIOCPNetworkModule::IOCPRecvThread );

		cIOCPSM.registerThread( &CIOCPNetworkModule::IOCPGQCSThreadSM );
		cIOCPSM.registerThread( &CIOCPNetworkModule::IOCPSendThread );

		//IOCP와 과련된 Thread를 실행한다.
		cIOCP.beginRegisteredThread();
		cIOCPSM.beginRegisteredThread();

		printf( "Waiting any Connection...\n" );

		cIOCP.postAccept();
		cIOCPSM.postAccept();

		while (char c = getchar())
		{
			if (c == 'z')
				break;
		}

		cIOCP.cleanup();
		cIOCPSM.cleanup();
	}
	catch(...)
	{
		ExceptionHandler::re_run();
	}
}
