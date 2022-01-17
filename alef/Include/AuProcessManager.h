#pragma once

#include <windows.h>
#include <string>

using namespace std;

// process state
typedef		int		ProcState_t;

// PROCESS_STATE_XXX
const ProcState_t	PROCESS_STATE_NOT_EXECUTED	= 0;
const ProcState_t	PROCESS_STATE_NORMAL		= 1;
const ProcState_t	PROCESS_STATE_ABNORMAL		= 2;

class AuProcessManager
{
public:
	static BOOL StartProcess(const char *i_szPath, const char *i_szProcessName, const char *i_szStartParameters);
	static BOOL StopProcess(const char *i_szProcessName);
//	static BOOL IsRunning(const char *i_szProcessName, SYSTEMTIME *pSystemTime = NULL);
//	static BOOL GetFileVersionTime(const char *i_szPath, SYSTEMTIME *o_pSystemTime);

//	static int PrintProcessInfo(const char *i_szProcessName);
	static HANDLE GetProcessHandleByName(const char *i_szProcessName);
};
