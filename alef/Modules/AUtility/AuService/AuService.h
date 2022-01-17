/*====================================================================

	AuService.h

====================================================================*/

#ifndef _LAKI_EXTRA_LIBRARY_SERVICE_H_
	#define _LAKI_EXTRA_LIBRARY_SERVICE_H_

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AuServiceD")
#else
	#pragma comment(lib, "AuService")
#endif
#endif
	

#define AUSERVICE_CONTROL_USER					128
#define AUSERVICE_MAX_NAME_LENGTH				256
#define AUSERVICE_ARG_INSTALL					_T("-i")
#define AUSERVICE_ARG_UNINSTALL					_T("-u")
#define AUSERVICE_ARG_RUN						_T("-r")

#include "ApBase.h"
#include <tchar.h>
#include <stdio.h>

/********************************************************/
/*		The Definition of the Service base class		*/
/********************************************************/
//
class AuService
	{
	public:
		const enum CONTROL_CODE
			{
			scStart = 0,
			scStop,
			scPause,
			scContinue
			};
		const enum STATUS_CODE
			{
			ssFail = 0,
			ssStopped,
			ssStarting,
			ssStopping,
			ssRunning,
			ssContinuing,
			ssPausing,
			ssPaused
			};

	protected:
		static	AuService	*m_pInstance;
		
		// win32 service related
		SC_HANDLE				m_hSCM;
		SC_HANDLE				m_hService;
		SERVICE_STATUS			m_ServiceStatus; 
		SERVICE_STATUS_HANDLE	m_hServiceStatusHandle; 

		// info
		TCHAR	m_szName[AUSERVICE_MAX_NAME_LENGTH];
		TCHAR	m_szDisplayName[AUSERVICE_MAX_NAME_LENGTH];
		DWORD	m_dwStartType;
		DWORD	m_dwStatus;
		BOOL	m_fIsRunning;
		BOOL	m_fIsPaused;

		// if ya want file-based log, set this varaible on OnInit()
		// or you can override Notify() method.
		TCHAR	m_szNotifyPath[AUSERVICE_MAX_NAME_LENGTH];

	protected:
		// event notify
		virtual void Notify(LPCTSTR pszFormat, ...);

		// service main
		static VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *pszArgv);

		// control handler
	  #ifdef _WIN2K_COMPAT
		static DWORD WINAPI HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID pEventData, LPVOID pContext);
	  #else
		static VOID WINAPI Handler(DWORD dwControl);
	  #endif

	public:
		AuService(LPTSTR pszName, LPTSTR pszDisplayName, DWORD dwStart=SERVICE_DEMAND_START);
		virtual ~AuService();

		// info
		LPCTSTR	Name();
		LPCTSTR	DisplayName();
		BOOL	IsInstalled();

		// installation
		BOOL	Install();
		BOOL	Uninstall();

		BOOL	Start();
		BOOL	ParseArguments(int argc, char *argv[]);
		void	SetUserAndPassword(LPTSTR pszUser = NULL, LPTSTR pszPwd = NULL);

	protected:
		void	SetStatus(DWORD dwControl);

		// virtual
		virtual void Run();						// main processing code of service goes here
		virtual void OnInstall();				// after install
		virtual void OnUninstall();				// after uninstall
		virtual BOOL OnInit(DWORD *pdwStatus);	// before run, FALSE if ya want not to run
		virtual void OnStop();					// before stop. methods below, same entry point
		virtual void OnPause();					
		virtual void OnContinue();
		virtual BOOL OnUserControl(DWORD dwControl);
		virtual void OnParamChange();
		virtual void OnNetBindAdd();
		virtual void OnNetBindRemove();
		virtual void OnNetBindEnable();
		virtual void OnNetBindDisable();
		virtual void OnSetArguments(int argc, LPTSTR *argv);
	  #ifdef _WIN2K_COMPAT
		virtual void OnDeviceEvent(DWORD dwEventType, LPVOID pvEventData);
		virtual void OnPowerEvent(DWORD dwEventType, LPVOID pvEventData);
		virtual void OnHardwareProfileChange(DWORD dwEventType);
	  #endif

	public:
		// static control & query status method
		static BOOL			ControlService(LPTSTR lpszServiceName, CONTROL_CODE sc, LPTSTR psz, DWORD &dwLength);
		static STATUS_CODE	QueryService(LPCTSTR pszName, LPTSTR psz, DWORD &dwLength);
	};

inline LPCTSTR AuService::Name()
	{
	return m_szName;
	}

inline LPCTSTR AuService::DisplayName()
	{
	return m_szDisplayName;
	}

#endif		//_LAKI_EXTRA_LIBRARY_SERVICE_H_