/*====================================================================

	AuService.cpp
		- Extra Library for Service

====================================================================*/

//#include "StdAfx.h"
#include "AuService.h"

#ifdef _ctextend
	#undef _ctextend
#endif

#define _ctextend		_T('\0')

#ifdef UNUSED_PARA
	#undef UNUSED_PARA
#endif

#define UNUSED_PARA(p)		p

/************************************************************/
/*		The Implementation of the Service base class		*/
/************************************************************/
//

//	static instance ptr.
//==========================================
//
AuService* AuService::m_pInstance = NULL;

//	constructor/destructor
//==========================================
//
AuService::AuService(LPTSTR pszName, LPTSTR pszDisplayName, DWORD dwStartType)
	{
	m_szNotifyPath[0] = _ctextend;
	_tcsncpy(m_szName, pszName, AUSERVICE_MAX_NAME_LENGTH);
	_tcsncpy(m_szDisplayName, pszDisplayName, AUSERVICE_MAX_NAME_LENGTH);
	m_dwStartType = dwStartType;

	m_hSCM = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
	if (NULL == m_hSCM)
		{
		Notify(_T("%s : service control manager open failed. (error=%d)"), GetLastError());
		}

    m_hService = 0;
	m_pInstance = this;
	m_fIsRunning = FALSE;
	m_fIsPaused = FALSE;

	m_ServiceStatus.dwServiceType				= SERVICE_WIN32_OWN_PROCESS; 
	m_ServiceStatus.dwCurrentState				= SERVICE_START_PENDING; 
	m_ServiceStatus.dwControlsAccepted			= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE; 
	m_ServiceStatus.dwWin32ExitCode				= 0; 
	m_ServiceStatus.dwServiceSpecificExitCode	= 0; 
	m_ServiceStatus.dwCheckPoint				= 0; 
	m_ServiceStatus.dwWaitHint					= 0; 
	}

AuService::~AuService()
	{
	if (0 != m_hSCM)
		{
		::CloseServiceHandle(m_hSCM);
		}
    if (0 != m_hService)
		{
		::CloseServiceHandle(m_hService);
		}
	}

//	ParseArguments
//=======================================
//
BOOL AuService::ParseArguments(int argc, TCHAR **argv)
	{
	BOOL fResult = FALSE;
	if (argc>1 && !(_tcsnicmp(argv[1], AUSERVICE_ARG_INSTALL, 2)))
		{
        if ((Install()))
			{
			Notify(_T("%s : installed successfully."), Name());
			}
		else
			{
			Notify(_T("%s : install failed. last error %d"), Name(), GetLastError());
			}
		fResult = TRUE;
		}
	else if (argc>1 && !(_tcsnicmp(argv[1], AUSERVICE_ARG_UNINSTALL, 2)))
		{
		if ((Uninstall()))
			{
			Notify(_T("%s : uninstalled successfully."), Name());
			}
		else
			{
			Notify(_T("%s : uninstall failed. (error=%d)"), Name(), GetLastError());
			}
        fResult = TRUE;
		}
	return fResult;
	}


void AuService::Notify(LPCTSTR pszFormat, ...)
	{
	va_list args;
	va_start(args, pszFormat);

	// if no file name, console out
	if (_ctextend == *m_szNotifyPath)
		{
		_vtprintf(pszFormat, args);
		_tprintf(_T("\n"));
		}

	else
		{
		FILE* pLog = fopen(m_szNotifyPath, "at");
		if (pLog)
			{
			_vftprintf(pLog, pszFormat, args);
			_ftprintf(pLog, _T("\n"));
			fclose(pLog);
			}
		}

	va_end(args);
	return;
	}

BOOL AuService::Install()
	{
	TCHAR szPath[MAX_PATH];
	TCHAR szBuffer[MAX_PATH+2];
	BOOL fResult = TRUE;  // Default to success

	GetModuleFileName(GetModuleHandle(NULL), szPath, MAX_PATH);
    // Why we do this?  The path may contain spaces.
    // if it does, the path must be quoted.
	_tcscpy(szBuffer, _T("\""));
	_tcscat(szBuffer, szPath);
	_tcscat(szBuffer, _T("\""));
	Notify(_T("%s : installing... (path=%s)"), Name(), szPath);

	m_hService = CreateService(m_hSCM, m_szName, m_szDisplayName,
							   SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
							   m_dwStartType, SERVICE_ERROR_NORMAL, szBuffer,
							   NULL, NULL, NULL, NULL, NULL);
	if (NULL == m_hService)
		{
		DWORD dwError;
		dwError = GetLastError();
		if (1073 == dwError) // Service already installed
			{
			Notify(_T("%s : install failed. service already installed(1073)"), Name());
			}
		else
			{
			Notify(_T("%s : install failed. (error = %d)"), Name(), dwError);
			}
		fResult = FALSE;
		}
	OnInstall();
	return fResult;
	}

BOOL AuService::Start()
	{
	SERVICE_TABLE_ENTRY DispatchTable[] =
		{
			{m_szName, AuService::ServiceMain},
			{NULL, NULL}
		};

	BOOL f = ::StartServiceCtrlDispatcher(DispatchTable);
	if (!f)
		{
		if ( ERROR_FAILED_SERVICE_CONTROLLER_CONNECT == GetLastError() )
			printf( "This program is service. But you run console mode.\n" );
		}
	return f;
	}

BOOL AuService::Uninstall()
	{
    m_hService = OpenService(m_hSCM, m_szName, SERVICE_ALL_ACCESS);
	Notify(_T("%s : uninstalling ..."), Name());
    if (NULL != m_hService)
		{
		DeleteService(m_hService);
		CloseServiceHandle(m_hService);
		m_hService = NULL;
		OnUninstall();
		return TRUE;
		}
	Notify("%s : cannot open service(Error=%d)", Name(), GetLastError());
	return FALSE;
	}


void AuService::SetStatus(DWORD dwControl)
	{
    m_ServiceStatus.dwCurrentState       = dwControl; 
    m_ServiceStatus.dwCheckPoint         = 0; 
    m_ServiceStatus.dwWaitHint           = 0; 
 
    if (!SetServiceStatus(m_hServiceStatusHandle, &m_ServiceStatus)) 
		{ 
		DWORD dwError = GetLastError(); 
		Notify(_T("%s : cannot set service status to %d. (error %d)"),
			   Name(), dwControl, dwError);
		} 
	}

VOID WINAPI AuService::ServiceMain(DWORD argc, LPTSTR *argv) 
	{
	UNUSED_PARA(argc);
	UNUSED_PARA(argv);
	ASSERT(NULL != m_pInstance);

	BOOL fInit; 
	DWORD dwError;
	AuService *thisPtr = m_pInstance;

  #ifdef _WIN2K_COMPAT
	TCHAR szContext[AUSERVICE_MAX_NAME_LENGTH];
    strCpy(szContext, thisPtr->Name());
    thisPtr->ServiceStatusHandle = RegisterServiceCtrlHandlerEx(thisPtr->m_szName,
																HandlerEx,
																&szContext);
  #else
    thisPtr->m_hServiceStatusHandle=RegisterServiceCtrlHandler(thisPtr->m_szName, Handler); 
  #endif 

    if ((SERVICE_STATUS_HANDLE)0 == thisPtr->m_hServiceStatusHandle) 
		{ 
		dwError = GetLastError();
		thisPtr->Notify(_T("%s : RegisterServiceCtrlHandler failed. (error=%d)"), thisPtr->Name(), dwError);
		return; 
		}

    thisPtr->SetStatus(SERVICE_START_PENDING);

	thisPtr->OnSetArguments(argc, argv);

	fInit=thisPtr->OnInit(&dwError); 
    if (!fInit)
		{ 
		thisPtr->SetStatus(SERVICE_STOPPED); 
		thisPtr->Notify(_T("%s : Initialization failed. (error=%d)"), thisPtr->Name(), dwError);
		return; 
		}

	thisPtr->SetStatus(SERVICE_RUNNING);
	thisPtr->m_fIsRunning = TRUE;
	thisPtr->Notify(_T("%s : Before running."), thisPtr->Name());
 
    thisPtr->Run();

    // Send current status. 
    thisPtr->SetStatus(SERVICE_STOPPED);
    return; 
	} 

#ifdef _WIN2K_COMPAT	// windows 2000 compatable
DWORD WINAPI AuService::HandlerEx(DWORD dwControl, DWORD dwEventType,
								  LPVOID pEventData, LPVOID pContext)
	{
	ASSERT(NULL != m_pInstance);
	DWORD dwStatus = SERVICE_RUNNING; 
	AuService *thisPtr = m_pInstance;
 
	TRACE(_T("%s : Into service Control..."), thisPtr->Name());
    switch (dwControl) 
		{
		case SERVICE_CONTROL_PAUSE: 
			TRACE(_T("%s : pausing..."), thisPtr->Name());
            if (!(thisPtr->m_fIsPaused))
	            {
                thisPtr->m_fIsPaused = TRUE;
                thisPtr->OnPause();
                dwStatus=SERVICE_PAUSED; 
		        }
            break; 
 
        case SERVICE_CONTROL_CONTINUE: 
			TRACE(_T("%s : continuing..."), thisPtr->Name());
            if (thisPtr->m_fIsPaused)
				{
                thisPtr->OnContinue();
                thisPtr->m_fIsPaused = FALSE;
                dwStatus=SERVICE_RUNNING; 
				}
            break; 
 
        case SERVICE_CONTROL_STOP: 
			TRACE(_T("%s : stopping..."), thisPtr->Name());
			if (thisPtr->m_fIsRunning)
				{
				thisPtr->OnStop();
				dwStatus=SERVICE_STOP_PENDING; 
				thisPtr->m_fIsRunning = FALSE;
				}
			break;

        case SERVICE_CONTROL_PARAMCHANGE:
            thisPtr->OnParamChange();
            break;

        case SERVICE_CONTROL_NETBINDADD:
            thisPtr->OnNetBindAdd();
            break

        case SERVICE_CONTROL_NETBINDDELETE:
            thisPtr->OnNetBindDelete();
            break

        case SERVICE_CONTROL_NETBINDREMOVE:
            thisPtr->OnNetBindRemove();
            break

        case SERVICE_CONTROL_NETBINDENABLE:
            thisPtr->OnNetBindEnable();
            break

        case SERVICE_CONTROL_NETBINDDISABLE:
            thisPtr->OnNetBindDisable();
            break

        case SERVICE_CONTROL_DEVICEEVENT:
            thisPtr->OnDeviceEvent(dwEventType, pEventData);
            break;

        case SERVICE_CONTROL_POWEREVENT:
            thisPtr->OnPowerEvent(dwEventType, pEventData);
            break;

        case SERVICE_CONTROL_HARDWAREPROFILECHANGE:
            thisPtr->OnHardwareProfileChange(dwEventType);
            break;

        case SERVICE_CONTROL_INTERROGATE: 
            break;
 
        default: 
            if (dwControl >= _AuService_ControlUser) 
				{
                if (!thisPtr->OnUserControl(dwControl))
					{
                    thisPtr->Notify(_T("%s : unhandled control code(%d)."), thisPtr->Name(), dwControl); 
					}
				}
            else 
				{
				thisPtr->Notify(_T("%s : unrecognized control code(%d)"), thisPtr->Name(), dwControl); 
				}
			break;
		}
 
    // Send current status. 
	thisPtr->SetStatus(dwStatus);
	return 0;
	}

#else		// not windows 2000 compatable

VOID WINAPI AuService::Handler(DWORD dwControl)
	{
	DWORD dwStatus = SERVICE_RUNNING; 
	AuService *thisPtr;

	thisPtr = m_pInstance;
 
	TRACE(_T("%s : Into service Control..."), thisPtr->Name());
    switch (dwControl) 
		{
		case SERVICE_CONTROL_PAUSE:
			TRACE(_T("%s : pausing..."), thisPtr->Name());
            if (!(thisPtr->m_fIsPaused))
				{
				thisPtr->m_fIsPaused = TRUE;
				thisPtr->OnPause();
				dwStatus=SERVICE_PAUSED; 
				}
            break; 
 
        case SERVICE_CONTROL_CONTINUE: 
			TRACE(_T("%s : continuing..."), thisPtr->Name());
            if (thisPtr->m_fIsPaused)
				{
				thisPtr->OnContinue();
				thisPtr->m_fIsPaused = FALSE;
				dwStatus = SERVICE_RUNNING; 
				}
			break; 
 
        case SERVICE_CONTROL_STOP: 
			TRACE(_T("%s : stopping..."), thisPtr->Name());
			if (thisPtr->m_fIsRunning)
				{
				thisPtr->OnStop();
				dwStatus=SERVICE_STOP_PENDING; 
				thisPtr->m_fIsRunning = FALSE;
				}
			break;
 
        case SERVICE_CONTROL_PARAMCHANGE:
            thisPtr->OnParamChange();
            break;

        case SERVICE_CONTROL_NETBINDADD:
            thisPtr->OnNetBindAdd();
            break;

        case SERVICE_CONTROL_NETBINDDISABLE:
            thisPtr->OnNetBindDisable();
            break;

        case SERVICE_CONTROL_NETBINDREMOVE:
            thisPtr->OnNetBindRemove();
            break;

        case SERVICE_CONTROL_NETBINDENABLE:
            thisPtr->OnNetBindEnable();
            break;

        case SERVICE_CONTROL_INTERROGATE: 
            break; 
 
        default: 
			if (dwControl >= AUSERVICE_CONTROL_USER) 
				{
				if (!thisPtr->OnUserControl(dwControl)) 
					{
					thisPtr->Notify(_T("%s : unhandled control code(%d)"),	thisPtr->Name(), dwControl); 
					}
				}
			else 
				{
				thisPtr->Notify(_T("%s : unrecognized control code(%d)"),  thisPtr->Name(), dwControl);
				}
			break;
		}
 
 
	// Send current status. 
	thisPtr->SetStatus(dwStatus);
	return; 
	}
#endif		// windows 2000 compatable



BOOL AuService::OnUserControl(DWORD dwControl)
	{
	UNUSED_PARA(dwControl);
	Notify(_T("%s : OnUserControl\n"), Name());
	return FALSE;
	}

void AuService::OnContinue()
	{
	Notify(_T("%s : OnContinue\n"), Name());
	return;
	}

void AuService::OnPause()
	{
	Notify(_T("%s : OnPause\n"), Name());
	return;
	}

void AuService::OnStop()
	{
	Notify(_T("%s : OnStop\n"), Name());
	return;
	}

void AuService::OnInstall()
	{
	Notify(_T("%s : OnInstall\n"), Name());
	return;
	}

void AuService::OnUninstall()
	{
	Notify(_T("%s : OnUninstall\n"), Name());
	return;
	}

BOOL AuService::OnInit(DWORD *pdwStatus)
	{
	Notify(_T("%s : OnInit\n"), Name());
	*pdwStatus = 0;
	return TRUE;
	}

void AuService::OnParamChange()
	{
	Notify(_T("%s : OnParamChange\n"), Name());
	return;
	}

void AuService::OnNetBindAdd()
	{
	Notify(_T("%s : OnNetBindAdd\n"), Name());
	return;
	}

void AuService::OnNetBindRemove()
	{
	Notify(_T("%s : OnNetBindRemove\n"), Name());
	return;
	}

void AuService::OnNetBindEnable()
	{
	Notify(_T("%s : OnNetBindEnable\n"), Name());
	return;
	}

void AuService::OnNetBindDisable()
	{
	Notify(_T("%s : OnNetBindDisable\n"), Name());
	return;
	}

void AuService::Run()
	{
	while (m_fIsRunning)
		{
        Sleep(1000);
		}
	return;
	}

void AuService::OnSetArguments(int argc, LPTSTR *argv)
	{
	Notify(_T("%s : OnSetArguments\n"), Name());
	return;
	}

#ifdef _WIN2K_COMPAT

void AuService::OnDeviceEvent(DWORD dwEventType, LPVOID pvEventData)
	{
	Notify(_T("%s : OnDeviceEvent\n"), Name());
	return;
	}

void AuService::OnPowerEvent(DWORD dwEventType, LPVOID pvEventData)
	{
	Notify(_T("%s : OnPowerEvent\n"), Name());
	return;
	}

void AuService::OnHardwareProfileChange(DWORD dwEventType)
	{
	Notify(_T("%s : OnHardwareProfileChange\n"), Name());
	return;
	}

#endif


BOOL AuService::ControlService(LPTSTR pszName, CONTROL_CODE sc, LPTSTR psz, DWORD& dwLength)
	{
	DWORD dwError;
	DWORD dwControl;
	SERVICE_STATUS Status;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    SC_HANDLE hService = ::OpenService(hSCM, pszName, SERVICE_ALL_ACCESS);  
    if (NULL == hService)
	    {
		dwError = GetLastError();
		//exceptionFromAPI(sz, dwError);
        //sz.Format("%s : can't open service. (error=%d)", pszName, dwError);
        return FALSE;
		}

	switch (sc)
		{
		case scStart:
			{
			if (!::StartService(hService, 0, NULL))
				{
				dwError = GetLastError();
				//sz.Format("%s : failed to start. (error=%d)", pszName, dwError);
				return FALSE;
				}
			}
			break;

		case scPause:
			{
			dwControl = SERVICE_CONTROL_PAUSE;
			if (!::ControlService(hService, dwControl, &Status))
				{
				dwError = GetLastError();
				//sz.Format("%s : failed to pause. (error=%d)", pszName, dwError);
				return FALSE;
				}
			}
			break;

		case scStop:
			{
			dwControl = SERVICE_CONTROL_STOP;
			if (!::ControlService(hService, dwControl, &Status))
				{
				dwError = GetLastError();
				//sz.Format("%s : failed to stop. (error=%d)", pszName, dwError);
				return FALSE;
				}
			}
			break;

		case scContinue:
			{
			dwControl = SERVICE_CONTROL_CONTINUE;
			if (!::ControlService(hService, dwControl, &Status))
				{
				dwError = GetLastError();
				//sz.Format("%s : failed to continue. (error=%d)", pszName, dwError);
				return FALSE;
				}
			}
			break;

		default:
			{
			//sz.Format("%s : unknown control code.", pszName);
			return FALSE;
			}
		}

	::CloseServiceHandle(hService); 
	::CloseServiceHandle(hSCM);

	return TRUE;
	}

AuService::STATUS_CODE AuService::QueryService(LPCTSTR pszName, LPTSTR psz, DWORD &dwLength)
	{
	DWORD dwError;
	SERVICE_STATUS Status;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    SC_HANDLE hService = ::OpenService(hSCM, pszName, SERVICE_ALL_ACCESS);  
    if (NULL == hService)
	    {
		dwError = GetLastError();
		//sz.Format("%s : can't open service. (error=%d)", pszName, dwError);
        return ssFail;
		}

    if (!QueryServiceStatus(hService, &Status))
		{
		dwError = GetLastError();
		//sz.Format("%s : can't query service status. (error=%d)", pszName, dwError);
		return ssFail;
		}

	STATUS_CODE ss = ssFail;

	switch (Status.dwCurrentState)
		{
		case SERVICE_STOPPED:
			ss = ssStopped;
			break;
		case SERVICE_START_PENDING:
			ss = ssStarting;
			break;
		case SERVICE_STOP_PENDING:
			ss = ssStopping;
			break;
		case SERVICE_RUNNING:
			ss = ssRunning;
			break;
		case SERVICE_CONTINUE_PENDING:
			ss = ssContinuing;
			break;
		case SERVICE_PAUSE_PENDING:
			ss = ssPausing;
			break;
		case SERVICE_PAUSED:
			ss = ssPaused;
			break;
		default:
			ss = ssFail;
			break;			
		}

	::CloseServiceHandle(hService); 
	::CloseServiceHandle(hSCM);

	return ss;
	}  
