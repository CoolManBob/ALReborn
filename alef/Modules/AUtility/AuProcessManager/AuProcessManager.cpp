#include ".\auprocessmanager.h"
#include <string>
#include <Tlhelp32.h>

using namespace std;

HANDLE AuProcessManager::GetProcessHandleByName(const char *i_szProcessName)
{
   PROCESSENTRY32	stProcessEntry32 = {0};
    BOOL            bIsOK;
	HANDLE			hProcess = NULL;

    HANDLE hProcessHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, FALSE);
    if (INVALID_HANDLE_VALUE == hProcessHandle)
        return FALSE;

    stProcessEntry32.dwSize = sizeof (PROCESSENTRY32);
    bIsOK=::Process32First(hProcessHandle, &stProcessEntry32);

    while (bIsOK)
    {
        if (0 == lstrcmpi(i_szProcessName, stProcessEntry32.szExeFile))
        {
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, stProcessEntry32.th32ProcessID);
            if (NULL != hProcess)
				break;
        }

        bIsOK = ::Process32Next(hProcessHandle, &stProcessEntry32);
    }

	return hProcess;
}

BOOL AuProcessManager::StartProcess(const char *i_szPath, const char *i_szProcessName, const char *i_szStartParameters)
{
	STARTUPINFO			si;		// startup info
	PROCESS_INFORMATION	pi;		// process info

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	string szCmdLine = string(i_szPath) + string(i_szProcessName) + string(i_szStartParameters);

	// Start the child process. 
	if( !CreateProcess( NULL, // No module name (use command line). 
		(LPSTR)szCmdLine.c_str(), // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
	) 
	{
		//printf("CreateProcess failed.\r\n");
		return FALSE;
	}

	// 생성된 process와 thread에 대한 usage count를 내리기 위해 바로 CloseHandle 호출함
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return TRUE;
}

BOOL AuProcessManager::StopProcess(const char *i_szProcessName)
{
	///////////////////////////////////////////////////////////////////////////
	// find process
	///////////////////////////////////////////////////////////////////////////

	// Get the list of process identifiers.
	HANDLE hProcess = GetProcessHandleByName(i_szProcessName);

	///////////////////////////////////////////////////////////////////////////
	// kill process
	///////////////////////////////////////////////////////////////////////////
	if (hProcess != NULL)
	{
		BOOL bRet = TerminateProcess(hProcess, 0);

		if (!bRet)
		{
			// Close process handles. 
			CloseHandle(hProcess);

			return FALSE;
		}

		// Wait until child process exits.
		if (WaitForSingleObject(hProcess, INFINITE) == WAIT_OBJECT_0)
		{
			//printf("Process Terminated!\r\n");
		}
		else
		{
			// ???
			DWORD dwErr = GetLastError();

			LPVOID lpMsgBuf;
			if (FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dwErr,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL ))
			{
				// Display the string.
				MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

				// Free the buffer.
				LocalFree( lpMsgBuf );
			}
		}

		// Close process handles. 
		CloseHandle(hProcess);

		return TRUE;
	}

	return FALSE;
}

//BOOL AuProcessManager::IsRunning(const char *i_szProcessName, SYSTEMTIME *pStartSystemTime)
//{
//	///////////////////////////////////////////////////////////////////////////
//	// find process
//	///////////////////////////////////////////////////////////////////////////
//
//	// Get the list of process identifiers.
//	DWORD aProcesses[1024], cbNeeded, cProcesses;
//	unsigned int i;
//	
//	HANDLE hProcess = GetProcessHandleByName(i_szProcessName);
//
//	// Print the name and process identifier for each process.
//	// Get the process name.
//	if (NULL != hProcess)
//	{
//		HMODULE hMod;
//		DWORD cbNeeded;
//
//		if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
//		{
//			GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName) );
//
//			if (strncmp(szProcessName, i_szProcessName, MAX_PATH) == 0)
//			{
//				// FOUND!!!
//				if (NULL != pStartSystemTime)
//				{
//					FILETIME CreationTime;
//					FILETIME ExitTime;
//					FILETIME KernelTime;
//					FILETIME UserTime;
//					GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime);
//					FILETIME tmpFileTime;
//					FileTimeToLocalFileTime(&CreationTime, &tmpFileTime);
//					FileTimeToSystemTime(&tmpFileTime, pStartSystemTime);
//				}
//
//				// Close process handles. 
// 				CloseHandle(hProcess);
//
//				return TRUE;
//			}
//		}
//
//		// Close process handles. 
//		CloseHandle(hProcess);
//
//		hProcess = NULL;
//	}
//
//	return FALSE;
//}

//int AuProcessManager::PrintProcessInfo(const char *i_szProcessName)
//{
//	// Get the list of process identifiers.
//	DWORD aProcesses[1024], cbNeeded, cProcesses;
//	unsigned int i;
//
//	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
//	{
//		return 0;
//	}
//
//	// Calculate how many process identifiers were returned.
//
//	cProcesses = cbNeeded / sizeof(DWORD);
//
//	// Print the name and process identifier for each process.
//
//	int nProcCount = 0;
//	for ( i = 0; i < cProcesses; i++ )
//	{
//		DWORD processID = aProcesses[i];
//
//		char szProcessName[MAX_PATH] = "unknown";
//
//		// Get a handle to the process.
//
//		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
//			PROCESS_VM_READ,
//			FALSE, processID );
//
//		// Get the process name.
//		if (NULL != hProcess )
//		{
//			HMODULE hMod;
//			DWORD cbNeeded;
//
//			if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
//			{
//				GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName) );
//
//				if (strncmp(szProcessName, i_szProcessName, MAX_PATH) == 0)
//				{
//					// increase process count
//					nProcCount++;
//
//					// Print the process name and identifier.
//					printf( "%s (Process ID: %u)\n", szProcessName, processID );
//				}
//			}
//		}
//
//		CloseHandle( hProcess );
//	}
//
//	printf("Total %d process(es)...\r\n", nProcCount);
//
//	return nProcCount;
//}

//BOOL AuProcessManager::GetFileVersionTime(const char *i_szPath, SYSTEMTIME *o_pSystemTime)
//{
//	HANDLE hFile; 
//
//	hFile = CreateFile(i_szPath,				// file to open 
//						GENERIC_READ,			// open for reading 
//						FILE_SHARE_READ,		// share for reading 
//						NULL,					// default security 
//						OPEN_EXISTING,			// existing file only 
//						FILE_ATTRIBUTE_NORMAL,	// normal file 
//						NULL);					// no attr. template 
//
//	if (hFile == INVALID_HANDLE_VALUE) 
//	{ 
//		printf("Could not open file (error %d)\n", GetLastError());
//		return FALSE;
//	}
//
//	FILETIME ftCreate, ftAccess, ftWrite;
//	SYSTEMTIME stUTC;
//
//	// Retrieve the file times for the file.
//	if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
//		return FALSE;
//
//	// Convert the last-write time to local time.
//	FileTimeToSystemTime(&ftWrite, &stUTC);
//	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, o_pSystemTime);
//
//	CloseHandle(hFile);
//
//	return TRUE;
//}
