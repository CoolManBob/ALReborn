// XTPSkinManagerModuleList.cpp: implementation of the CXTPSkinManagerModuleList class.
//
// This file is a part of the XTREME SKINFRAMEWORK MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tlhelp32.h>

#include "XTPSkinManagerModuleList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CModuleEnumerator

class CXTPSkinManagerModuleList::CModuleEnumerator
{
public:
	CModuleEnumerator(DWORD dwProcessId);
	virtual ~CModuleEnumerator();

	virtual HMODULE GetFirstModule() = 0;
	virtual HMODULE GetNextModule() = 0;

protected:
	int m_dwProcessId;

};

CXTPSkinManagerModuleList::CModuleEnumerator::CModuleEnumerator(DWORD dwProcessId)
	: m_dwProcessId(dwProcessId)
{

}

CXTPSkinManagerModuleList::CModuleEnumerator::~CModuleEnumerator()
{

}

//////////////////////////////////////////////////////////////////////////
// CPsapiModuleEnumerator

class CXTPSkinManagerModuleList::CPsapiModuleEnumerator : public CModuleEnumerator
{
	typedef BOOL (WINAPI* PFNENUMPROCESSMODULES) ( HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded );

public:
	CPsapiModuleEnumerator(DWORD dwProcessId, HMODULE hModPSAPI);
	~CPsapiModuleEnumerator();

	virtual HMODULE GetFirstModule();
	virtual HMODULE GetNextModule();

protected:
	PFNENUMPROCESSMODULES m_pfnEnumProcessModules;
	HMODULE m_hModPSAPI;

	HMODULE* m_pModules;
	int m_nIndex;
	int m_nCount;
};

CXTPSkinManagerModuleList::CPsapiModuleEnumerator::CPsapiModuleEnumerator(DWORD dwProcessId, HMODULE hModPSAPI)
	: CModuleEnumerator(dwProcessId)
{
	m_hModPSAPI = hModPSAPI;
	ASSERT(m_hModPSAPI);

	m_pfnEnumProcessModules = (PFNENUMPROCESSMODULES)
		::GetProcAddress(m_hModPSAPI, "EnumProcessModules");

	m_pModules = NULL;
	m_nIndex = 0;
	m_nCount = 0;
}

CXTPSkinManagerModuleList::CPsapiModuleEnumerator::~CPsapiModuleEnumerator()
{
	if (m_pModules)
	{
		delete[] m_pModules;
	}
}

HMODULE CXTPSkinManagerModuleList::CPsapiModuleEnumerator::GetFirstModule()
{
	if (!m_pfnEnumProcessModules)
		return NULL;

	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_dwProcessId);

	if (!hProcess)
		return NULL;

	DWORD cbNeeded = 0;
	HMODULE hModule;

	if (!m_pfnEnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded))
	{
		::CloseHandle(hProcess);
		return NULL;
	}

	if (cbNeeded == 0)
	{
		::CloseHandle(hProcess);
		return NULL;
	}

	m_nCount = cbNeeded / sizeof(HMODULE);

	m_pModules = new HMODULE[m_nCount];

	if (!m_pfnEnumProcessModules(hProcess, m_pModules, cbNeeded, &cbNeeded))
	{
		::CloseHandle(hProcess);
		return NULL;
	}

	m_nIndex = 0;

	::CloseHandle(hProcess);

	return m_pModules[0];
}

HMODULE CXTPSkinManagerModuleList::CPsapiModuleEnumerator::GetNextModule()
{
	ASSERT(m_pModules != NULL);
	if (m_pModules == NULL)
		return NULL;

	m_nIndex++;

	if (m_nIndex >= m_nCount)
		return NULL;

	return m_pModules[m_nIndex];
}

//////////////////////////////////////////////////////////////////////////
// ToolHelp


class CXTPSkinManagerModuleList::CToolHelpModuleEnumerator : public CModuleEnumerator
{
	typedef BOOL (WINAPI* PFNMODULE32FIRST) ( HANDLE hSnapshot, LPMODULEENTRY32 lpme );
	typedef BOOL (WINAPI* PFNMODULE32NEXT) ( HANDLE hSnapshot, LPMODULEENTRY32 lpme );
	typedef HANDLE (WINAPI* PFNCREATETOOLHELP32SNAPSHOT) ( DWORD dwFlags, DWORD th32ProcessID );

public:
	CToolHelpModuleEnumerator(DWORD dwProcessId);
	~CToolHelpModuleEnumerator();

	virtual HMODULE GetFirstModule();
	virtual HMODULE GetNextModule();

protected:
	HANDLE m_hSnapshot;

	PFNCREATETOOLHELP32SNAPSHOT m_pfnCreateToolhelp32Snapshot;
	PFNMODULE32NEXT m_pfnModule32Next;
	PFNMODULE32FIRST m_pfnModule32First;

	MODULEENTRY32 m_me;
};

CXTPSkinManagerModuleList::CToolHelpModuleEnumerator::CToolHelpModuleEnumerator(DWORD dwProcessId)
	: CModuleEnumerator(dwProcessId)
{

	HMODULE hInstLib = GetModuleHandle(_T("KERNEL32"));
	ASSERT(hInstLib);
	//
	// We must link to these functions of Kernel32.DLL explicitly. Otherwise
	// a module using this code would fail to load under Windows NT, which does not
	// have the Toolhelp32 functions in the Kernel32.
	//
	m_pfnCreateToolhelp32Snapshot = (PFNCREATETOOLHELP32SNAPSHOT)
		::GetProcAddress(hInstLib, "CreateToolhelp32Snapshot");

#ifdef UNICODE
	m_pfnModule32First = (PFNMODULE32FIRST)
		::GetProcAddress(hInstLib, "Module32FirstW");
	m_pfnModule32Next = (PFNMODULE32NEXT)
		::GetProcAddress(hInstLib, "Module32NextW");
#else
	m_pfnModule32First = (PFNMODULE32FIRST)
		::GetProcAddress(hInstLib, "Module32First");
	m_pfnModule32Next = (PFNMODULE32NEXT)
		::GetProcAddress(hInstLib, "Module32Next");
#endif

	m_hSnapshot = INVALID_HANDLE_VALUE;
	m_me.dwSize = sizeof(MODULEENTRY32);
}

CXTPSkinManagerModuleList::CToolHelpModuleEnumerator::~CToolHelpModuleEnumerator()
{
	if (m_hSnapshot != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hSnapshot);
	}
}

HMODULE CXTPSkinManagerModuleList::CToolHelpModuleEnumerator::GetFirstModule()
{
	if (m_pfnCreateToolhelp32Snapshot == NULL ||
		m_pfnModule32First == NULL || m_pfnModule32Next == NULL)
	{
		return NULL;
	}

	m_hSnapshot = m_pfnCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwProcessId);

	if (m_hSnapshot == INVALID_HANDLE_VALUE)
		return NULL;

	if (m_pfnModule32First(m_hSnapshot, &m_me))
	{
		return m_me.hModule;
	}

	return NULL;
}

HMODULE CXTPSkinManagerModuleList::CToolHelpModuleEnumerator::GetNextModule()
{
	if (m_hSnapshot == INVALID_HANDLE_VALUE)
		return NULL;

	if (m_pfnModule32Next(m_hSnapshot, &m_me))
	{
		return m_me.hModule;
	}

	return NULL;
}


//////////////////////////////////////////////////////////////////////
// CXTPSkinManagerModuleList


CXTPSkinManagerModuleList::CSharedData::CSharedData()
{
	m_hPsapiDll = ::LoadLibraryA("PSAPI.DLL");

	m_bExists = m_hPsapiDll != NULL;

	if (!m_bExists)
	{
		HMODULE hModule = ::GetModuleHandle(_T("KERNEL32.DLL"));
		if (hModule)
		{
			if (::GetProcAddress(hModule, "CreateToolhelp32Snapshot") != NULL)
			{
				m_bExists = TRUE;
			}
		}

	}
}

CXTPSkinManagerModuleList::CSharedData::~CSharedData()
{
	if (m_hPsapiDll)
	{
		//FreeLibrary(m_hPsapiDll); Dangerous to call FreeLibrary in destructors of static objects.
	}
}

CXTPSkinManagerModuleList::CSharedData& AFX_CDECL CXTPSkinManagerModuleList::GetSharedData()
{
	static CSharedData sData;
	return sData;
}

BOOL AFX_CDECL CXTPSkinManagerModuleList::IsEnumeratorExists()
{
	return GetSharedData().m_bExists;

}

CXTPSkinManagerModuleList::CXTPSkinManagerModuleList(DWORD dwProcessId)
{
	m_pEnumerator = NULL;

	if (GetSharedData().m_hPsapiDll)
	{
		m_pEnumerator = new CPsapiModuleEnumerator(dwProcessId, GetSharedData().m_hPsapiDll);
	}
	else if (GetSharedData().m_bExists)
	{
		m_pEnumerator = new CToolHelpModuleEnumerator(dwProcessId);
	}

	ASSERT(m_pEnumerator);
}

CXTPSkinManagerModuleList::~CXTPSkinManagerModuleList()
{
	SAFE_DELETE(m_pEnumerator);
}

HMODULE CXTPSkinManagerModuleList::GetFirstModule()
{
	if (m_pEnumerator)
	{
		return m_pEnumerator->GetFirstModule();
	}
	return NULL;
}

HMODULE CXTPSkinManagerModuleList::GetNextModule()
{
	if (m_pEnumerator)
	{
		return m_pEnumerator->GetNextModule();
	}
	return NULL;
}
