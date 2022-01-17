/******************************************************************************
Module:  ApBase.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: define base class include mutex object for public shared data
Last Update: 2002. 05. 27
******************************************************************************/

#if !defined(__APBASE_H__)
#define __APBASE_H__

#include "ApDefine.h"
#include "MagDebug.h"
#include "ApSafeArray.h"
#include "ApVector.h"
#include "ApString.h"
#include "ApMutualEx.h"
#include "ApRWLock.h"
#include "ApMemoryChecker.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "ApBaseD" )
#else
#pragma comment ( lib , "ApBase" )
#endif
#endif

// Dialog Message Func - 2004.01.12. steeple
typedef BOOL (*DIALOG_MSG_FUNC) (PVOID pMSG, PVOID pClass);

typedef enum
{
	APBASE_TYPE_NONE	= 0,
	APBASE_TYPE_OBJECT,
	APBASE_TYPE_OBJECT_TEMPLATE,
	APBASE_TYPE_CHARACTER,
	APBASE_TYPE_CHARACTER_TEMPLATE,
	APBASE_TYPE_ITEM,
	APBASE_TYPE_ITEM_TEMPLATE,
	APBASE_TYPE_SKILL,
	APBASE_TYPE_SKILL_TEMPLATE,
	APBASE_TYPE_SHRINE,
	APBASE_TYPE_SHRINE_TEMPLATE,
	APBASE_TYPE_PARTY,
	APBASE_TYPE_SERVER,
	APBASE_TYPE_SERVER_TEMPLATE,
	APBASE_TYPE_QUEST_TEMPLATE,
	APBASE_TYPE_UI,
	APBASE_TYPE_UI_CONTROL,
	APBASE_MAX_TYPE
} ApBaseType;

class ApBaseExLock {
public:
	ApBaseType	m_eType;
	INT32		m_lID;

	INT32		GetID() { return m_lID; }
};

class ApBase : public ApBaseExLock {
public:
	ApMutualEx	m_Mutex;
};

struct THREADNAME_INFO
{
	DWORD dwType;
	CHAR* szname;
	DWORD dwThreadID;
	DWORD dwFlags;
};

/* 프로세스 중복 실행 방지 by iluvs */
static BOOL	IsProcessRun(LPCSTR szProcessName)
{
	HANDLE hProcessMutex = CreateMutex(NULL, FALSE, szProcessName);

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hProcessMutex);
		return TRUE;
	}

	return FALSE;
}

// Setting Thread Name 
static void __stdcall SetThreadName(DWORD dwThreadID, CHAR *szThreadName)
{
	THREADNAME_INFO stInfo;
	stInfo.dwType		= 0x1000;
	stInfo.szname		= szThreadName;
	stInfo.dwThreadID	= dwThreadID;
	stInfo.dwFlags		= 0;

	__try
	{
		RaiseException(0x406D1388, 0, sizeof(THREADNAME_INFO) / sizeof(DWORD), (ULONG_PTR*)(&stInfo));
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

// Delete Object for container. 2006.04.18. steeple
struct DeletePtr
{
	template<typename T>
	void operator() (const T* ptr) const
	{
		if(ptr)
			delete ptr;
	}
};

#endif //__APBASE_H__