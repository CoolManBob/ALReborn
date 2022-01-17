// XTPSkinManagerApiHook.cpp: implementation of the CXTPSkinManagerApiHook class.
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

#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")

#include "Common/XTPVC80Helpers.h"

#include "XTPSkinManager.h"
#include "XTPSkinObjectFrame.h"
#include "XTPSkinDrawTools.h"

#include "XTPSkinManagerApiHook.h"
#include "XTPSkinManagerModuleList.h"
#include "XTPSkinManagerResource.h"

#ifdef _UNICODE
	#define lpszSystemParametersInfo "SystemParametersInfoW"
#else
	#define lpszSystemParametersInfo "SystemParametersInfoA"
#endif


int XTPCompareStringNoCase(const char* dst, const char* src)
{
	int f, l;
	do
	{
		f = (unsigned char)(*(dst++));
		if ((f >= 'A') && (f <= 'Z'))
			f -= ('A' - 'a');

		l = (unsigned char)(*(src++));
		if ((l >= 'A') && (l <= 'Z'))
			l -= ('A' - 'a');
	}
	while ( f && (f == l) );

	return (f - l);
}

CXTPSkinManagerApiHook::CXTPSkinManagerApiHook()
{
	m_bInitialized = FALSE;
	ZeroMemory(&m_arrFunctions, sizeof(m_arrFunctions));

	ExcludeModule(_T("SHLWAPI.DLL"), TRUE);
	ExcludeModule(_T("COMCTL32.DLL"), TRUE);
	ExcludeModule(_T("KERNEL32.DLL"), FALSE);
	ExcludeModule(_T("USER32.DLL"), FALSE);
	ExcludeModule(_T("GDI32.DLL"), FALSE);
}

CXTPSkinManagerApiHook::~CXTPSkinManagerApiHook()
{
	FinalizeHookManagement();
}


CXTPSkinManagerApiHook* CXTPSkinManagerApiHook::GetInstance()
{
	static CXTPSkinManagerApiHook instance;
	return &instance;
}

void CXTPSkinManagerApiHook::ExcludeModule(LPCTSTR lpszModule, BOOL bWin9x)
{
	EXCLUDEDMODULE em;
	em.bWin9x = bWin9x;
	em.strModule = lpszModule;
	em.hModule = 0;
	m_arrExcludedModules.Add(em);
}

BOOL CXTPSkinManagerApiHook::IsModuleExcluded(HMODULE hModule, BOOL bUseHandle) const
{
	BOOL bWin9x = XTPSkinManager()->IsWin9x();

	for (int i = 0; i < (int)m_arrExcludedModules.GetSize(); i++)
	{
		const EXCLUDEDMODULE& em = m_arrExcludedModules[i];

		if (em.bWin9x && !bWin9x)
			continue;

		if (bUseHandle)
		{
			if (em.hModule == hModule)
				return TRUE;
		}
		else
		{
			if (GetModuleHandle(em.strModule) == hModule)
				return TRUE;
		}
	}
	return FALSE;
}


void CXTPSkinManagerApiHook::UnhookAllFunctions()
{
	for (int i = 0; i < XTP_SKIN_APIHOOKCOUNT; i++)
	{
		CXTPSkinManagerApiFunction* pHook = m_arrFunctions[i];

		if (pHook)
		{
			pHook->UnhookImport();
			delete pHook;

			m_arrFunctions[i] = NULL;
		}
	}

	ZeroMemory(&m_arrFunctions, sizeof(m_arrFunctions));

}

CXTPSkinManagerApiFunction* CXTPSkinManagerApiHook::HookImport(XTPSkinFrameworkApiFunctionIndex nIndex, LPCSTR pszCalleeModName, LPCSTR pszFuncName, PROC  pfnHook)
{
	CXTPSkinManagerApiFunction* pResult = NULL;

	try
	{
		if (!GetHookedFunction(pszCalleeModName, pszFuncName))
		{
			if (XTPCompareStringNoCase(pszCalleeModName, "UXTHEME.DLL") == 0)
			{
				return AddHook(nIndex, pszCalleeModName, pszFuncName, NULL, pfnHook);
			}

			PROC pfnOrig = GetProcAddressWindows(::GetModuleHandleA(pszCalleeModName), pszFuncName);

			if (NULL == pfnOrig)
			{
				HMODULE hmod = ::LoadLibraryA(pszCalleeModName);
				if (hmod)
				{
					pfnOrig = GetProcAddressWindows(::GetModuleHandleA(pszCalleeModName), pszFuncName);
				}
			}

			if (pfnOrig)
			{
				pResult = AddHook(nIndex, pszCalleeModName, pszFuncName, pfnOrig,pfnHook);
			}
		}
	}
	catch(...)
	{

	}

	return pResult;
}

CXTPSkinManagerApiFunction* CXTPSkinManagerApiHook::AddHook(XTPSkinFrameworkApiFunctionIndex nIndex, LPCSTR pszCalleeModName, LPCSTR pszFuncName, PROC  pfnOrig, PROC  pfnHook)
{
	ASSERT(!GetHookedFunction(pszCalleeModName, pszFuncName));
	ASSERT(!GetHookedFunction(nIndex));
	ASSERT(nIndex < XTP_SKIN_APIHOOKCOUNT);

	CXTPSkinManagerApiFunction* pHook = new CXTPSkinManagerApiFunction(this, pszCalleeModName, pszFuncName, pfnOrig, pfnHook);
	m_arrFunctions[nIndex] = pHook;
	pHook->HookImport();

	return pHook;
}

FARPROC WINAPI CXTPSkinManagerApiHook::GetProcAddressWindows(HMODULE hModule, LPCSTR pszProcName)
{
	typedef FARPROC (WINAPI* LPFNGETPROCADDRESS)(HMODULE hModule, LPCSTR pszProcName);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiGetProcAddress);

	if (pfnOrig)
		return ((LPFNGETPROCADDRESS)pfnOrig)(hModule, pszProcName);

	return ::GetProcAddress(hModule, pszProcName);
}

LRESULT WINAPI CXTPSkinManagerApiHook::CallWindowProcOrig(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	typedef LRESULT (WINAPI* LPFNCALLWINDOWPROC)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#ifdef _UNICODE
	PROC pfnOrig = GetOriginalProc(xtpSkinApiCallWindowProcW);
#else
	PROC pfnOrig = GetOriginalProc(xtpSkinApiCallWindowProcA);
#endif

	if (pfnOrig)
		return ((LPFNCALLWINDOWPROC)pfnOrig)(lpPrevWndFunc, hWnd, Msg, wParam, lParam);

	return ::CallWindowProc(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

CXTPSkinManagerApiFunction* CXTPSkinManagerApiHook::GetHookedFunction(XTPSkinFrameworkApiFunctionIndex nIndex)
{
	ASSERT(nIndex < XTP_SKIN_APIHOOKCOUNT);
	if (nIndex < 0 || nIndex >= XTP_SKIN_APIHOOKCOUNT)
		return NULL;

	return m_arrFunctions[nIndex];
}

PROC AFX_CDECL CXTPSkinManagerApiHook::GetOriginalProc(XTPSkinFrameworkApiFunctionIndex nIndex)
{
	CXTPSkinManagerApiFunction* pFunction = GetInstance()->GetHookedFunction(nIndex);
	if (!pFunction)
		return NULL;

	return pFunction->m_pfnOrig;
}


CXTPSkinManagerApiFunction* CXTPSkinManagerApiHook::GetHookedFunction(LPCSTR pszCalleeModName, LPCSTR pszFuncName)
{
	for (int i = 0; i < XTP_SKIN_APIHOOKCOUNT; i++)
	{
		CXTPSkinManagerApiFunction* pHook = m_arrFunctions[i];

		if (!pHook)
		{
			continue;
		}

		if (XTPCompareStringNoCase(pHook->m_szCalleeModName, pszCalleeModName) == 0 &&
			XTPCompareStringNoCase(pHook->m_szFuncName, pszFuncName) == 0)
		{
			return pHook;
		}
	}

	return NULL;
}

//
// The PUSH opcode on x86 platforms
//
const BYTE cPushOpCode = 0x68;

PVOID CXTPSkinManagerApiFunction::sm_pvMaxAppAddr = NULL;
PVOID CXTPSkinManagerApiFunction::sm_pfnAfxWndProc = NULL;

//////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerApiFunction

CXTPSkinManagerApiFunction::CXTPSkinManagerApiFunction( CXTPSkinManagerApiHook* pApiHook,
	LPCSTR pszCalleeModName, LPCSTR pszFuncName, PROC pfnOrig, PROC pfnHook)
:
	m_pApiHook(pApiHook),
	m_bHooked(FALSE),
	m_pfnOrig(pfnOrig),
	m_pfnHook(pfnHook)
{
#if (_MSC_VER > 1310) // VS2005
	strcpy_s(m_szCalleeModName, 20, pszCalleeModName);
	strcpy_s(m_szFuncName, 30, pszFuncName);
#else
	strcpy(m_szCalleeModName, pszCalleeModName);
	strcpy(m_szFuncName, pszFuncName);
#endif

	if (sm_pfnAfxWndProc == NULL)
	{
		sm_pfnAfxWndProc = (FARPROC)AfxGetAfxWndProc();

	}

	if (sm_pvMaxAppAddr == NULL)
	{
		//
		// Functions with address above lpMaximumApplicationAddress require
		// special processing (Windows 9x only)
		//
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		sm_pvMaxAppAddr = si.lpMaximumApplicationAddress;
	}

	if (m_pfnOrig > sm_pvMaxAppAddr)
	{
		//
		// The address is in a shared DLL; the address needs fixing up
		//
		PBYTE pb = (PBYTE) m_pfnOrig;
		if (pb[0] == cPushOpCode)
		{
			//
			// Skip over the PUSH op code and grab the real address
			//
			PVOID pv = * (PVOID*) &pb[1];
			m_pfnOrig = (PROC) pv;
		}
	}

}


CXTPSkinManagerApiFunction::~CXTPSkinManagerApiFunction()
{
	UnhookImport();
}

BOOL CXTPSkinManagerApiFunction::HookImport()
{
	return m_bHooked = ReplaceInAllModules(m_szCalleeModName, m_pfnOrig, m_pfnHook);
}

BOOL CXTPSkinManagerApiFunction::UnhookImport()
{
	if (m_bHooked)
	{
		ReplaceInAllModules(m_szCalleeModName, m_pfnHook, m_pfnOrig);
		m_bHooked = FALSE;
	}
	return !m_bHooked;
}

BOOL CXTPSkinManagerApiFunction::ReplaceInAllModules(LPCSTR pszCalleeModName,
	PROC pfnCurrent, PROC pfnNew)
{
	BOOL bResult = FALSE;

	if ((NULL != pfnCurrent) && (NULL != pfnNew))
	{

		CXTPSkinManagerModuleList moduleList(::GetCurrentProcessId());

		HMODULE hModule = moduleList.GetFirstModule();

		while (hModule)
		{
#ifdef _DEBUG
			TCHAR fn[MAX_PATH];
			GetModuleFileName(hModule, fn, MAX_PATH);
#endif
			if ((hModule != AfxGetInstanceHandle()) && !m_pApiHook->IsModuleExcluded(hModule, TRUE))
			{
				if (ReplaceInOneModule(pszCalleeModName, pfnCurrent, pfnNew, hModule))
				{
					bResult = TRUE;
				}
			}

			hModule = moduleList.GetNextModule();
		}


		if (ReplaceInOneModule(pszCalleeModName, pfnCurrent, pfnNew, AfxGetInstanceHandle()))
		{
			bResult = TRUE;
		}
	}
	return bResult;
}

BOOL CXTPSkinManagerApiFunction::ReplaceInOneModule(LPCSTR   pszCalleeModName, PROC pfnCurrent,
	PROC pfnNew, HMODULE hmodCaller)
{

	try
	{
		ULONG ulSize;
		// Get the address of the module's import section
		PIMAGE_IMPORT_DESCRIPTOR pImportDesc =
			(PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
			hmodCaller,
			TRUE,
			IMAGE_DIRECTORY_ENTRY_IMPORT,
			&ulSize
			);
		// Does this module has import section ?
		if (pImportDesc == NULL)
			return FALSE;

		while (pImportDesc != NULL)
		{

			// Loop through all descriptors and
			// find the import descriptor containing references to callee's functions
			while (pImportDesc->Name)
			{
				LPCSTR lpszName = ((LPCSTR)((PBYTE) hmodCaller + pImportDesc->Name));
				if (XTPCompareStringNoCase(lpszName, pszCalleeModName) == 0)
					break;   // Found
				pImportDesc++;
			} // while
			// Does this module import any functions from this callee ?
			if (pImportDesc->Name == 0)
				return FALSE;

			PIMAGE_THUNK_DATA pThunk =
				(PIMAGE_THUNK_DATA)( (PBYTE) hmodCaller + (UINT_PTR)pImportDesc->FirstThunk );

			while (pThunk->u1.Function)
			{
				PROC* ppfn = (PROC*) &pThunk->u1.Function;

				BOOL bFound = (*ppfn == pfnCurrent);

				if (!bFound && (*ppfn > sm_pvMaxAppAddr))
				{
					PBYTE pbInFunc = (PBYTE) *ppfn;
					// Is this a wrapper (debug thunk) represented by PUSH instruction?
					if (pbInFunc[0] == cPushOpCode)
					{
						ppfn = (PROC*) &pbInFunc[1];
						bFound = (*ppfn == pfnCurrent);
					}
				}


				if (bFound)
				{
					MEMORY_BASIC_INFORMATION mbi;
					::VirtualQuery(ppfn, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
					// In order to provide writable access to this part of the
					// memory we need to change the memory protection
					if (!::VirtualProtect(mbi.BaseAddress, mbi.RegionSize,
						PAGE_READWRITE, &mbi.Protect))
					{
						return FALSE;
					}

					// Hook the function.
					*ppfn = *pfnNew;

					// Restore the protection back
					DWORD dwOldProtect;
					::VirtualProtect(mbi.BaseAddress, mbi.RegionSize,
						mbi.Protect, &dwOldProtect);

					return TRUE;
				}
				pThunk++;
			}
			pImportDesc++;

		}
	}
	catch(...)
	{
		// do nothing
	}

	return FALSE;
}

void CXTPSkinManagerApiHook::InitializeHookManagement()
{
	if (m_bInitialized)
		return;

	for (int i = 0; i < (int)m_arrExcludedModules.GetSize(); i++)
	{
		m_arrExcludedModules[i].hModule = GetModuleHandle(m_arrExcludedModules[i].strModule);
	}


	#define ADDHOOK(Dll, ProcName) \
		HookImport(xtpSkinApi##ProcName, Dll, #ProcName, (PROC)&CXTPSkinManagerApiHook::OnHook##ProcName)

	ADDHOOK("KERNEL32.DLL", LoadLibraryA);
	ADDHOOK("KERNEL32.DLL", LoadLibraryW);
	ADDHOOK("KERNEL32.DLL", LoadLibraryExA);
	ADDHOOK("KERNEL32.DLL", LoadLibraryExW);
	ADDHOOK("KERNEL32.DLL", GetProcAddress);
	ADDHOOK("KERNEL32.DLL", GetModuleHandleA);
	ADDHOOK("KERNEL32.DLL", CreateThread);

	ADDHOOK("USER32.DLL", RegisterClassA);
	ADDHOOK("USER32.DLL", RegisterClassW);

	if (XTPSkinManager()->GetApplyOptions() & xtpSkinApplyColors)
	{
		ADDHOOK("USER32.DLL", GetSysColor);
		ADDHOOK("USER32.DLL", GetSysColorBrush);
		ADDHOOK("USER32.DLL", DrawEdge);
		ADDHOOK("USER32.DLL", FillRect);
		ADDHOOK("GDI32.DLL", DeleteObject);

		ADDHOOK("UXTHEME.DLL", OpenThemeData);
		ADDHOOK("UXTHEME.DLL", DrawThemeBackground);
		ADDHOOK("UXTHEME.DLL", CloseThemeData);
		ADDHOOK("UXTHEME.DLL", GetThemeColor);
		ADDHOOK("UXTHEME.DLL", IsAppThemed);
		ADDHOOK("UXTHEME.DLL", IsThemeActive);
		ADDHOOK("UXTHEME.DLL", GetCurrentThemeName);
		ADDHOOK("UXTHEME.DLL", GetThemeSysBool);
		ADDHOOK("UXTHEME.DLL", GetThemeSysColor);
		ADDHOOK("UXTHEME.DLL", GetThemePartSize);
		
		ADDHOOK("USER32.DLL", DrawFrameControl);
	}


	if (XTPSkinManager()->GetApplyOptions() & xtpSkinApplyFrame)
	{
		ADDHOOK("USER32.DLL", SetScrollInfo);
		ADDHOOK("USER32.DLL", SetScrollPos);
		ADDHOOK("USER32.DLL", GetScrollInfo);
		ADDHOOK("USER32.DLL", EnableScrollBar);
	}

	if (XTPSkinManager()->GetApplyOptions() & xtpSkinApplyMetrics)
	{
		HookImport(xtpSkinApiSystemParametersInfo, "USER32.DLL", lpszSystemParametersInfo, (PROC)&CXTPSkinManagerApiHook::OnHookSystemParametersInfo);
	}

	ADDHOOK("USER32.DLL", DefWindowProcA);
	ADDHOOK("USER32.DLL", DefWindowProcW);
	ADDHOOK("USER32.DLL", DefFrameProcA);
	ADDHOOK("USER32.DLL", DefFrameProcW);
	ADDHOOK("USER32.DLL", DefDlgProcA);
	ADDHOOK("USER32.DLL", DefDlgProcW);
	ADDHOOK("USER32.DLL", DefMDIChildProcA);
	ADDHOOK("USER32.DLL", DefMDIChildProcW);

	ADDHOOK("USER32.DLL", CallWindowProcA);
	ADDHOOK("USER32.DLL", CallWindowProcW);

	m_bInitialized = TRUE;
}

void CXTPSkinManagerApiHook::FinalizeHookManagement()
{
	if (!m_bInitialized)
		return;

	UnhookAllFunctions();

	m_bInitialized = FALSE;
}

DWORD WINAPI CXTPSkinManagerApiHook::OnHookGetSysColor(int nIndex)
{
	typedef int(WINAPI* LPFNGETSYSCOLOR)(int);

	CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();

	if (XTPSkinManager()->IsEnabled() && nIndex < XTP_SKINMETRICS_COLORTABLESIZE && pMetrics->m_clrTheme[nIndex] != -1)
		return pMetrics->m_clrTheme[nIndex];

	PROC pfnOrig = GetOriginalProc(xtpSkinApiGetSysColor);

	DWORD nResult = pfnOrig ? ((LPFNGETSYSCOLOR)pfnOrig)(nIndex) : GetSysColor(nIndex);

	return nResult;

}

BOOL WINAPI CXTPSkinManagerApiHook::OnHookDeleteObject(HGDIOBJ hObject)
{
	typedef BOOL (WINAPI* LPFNDELETEOBJECT)(HGDIOBJ);

	CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();
	if (pMetrics->IsMetricObject(hObject))
		return FALSE;

	PROC pfnOrig = GetOriginalProc(xtpSkinApiDeleteObject);

	return pfnOrig ? ((LPFNDELETEOBJECT)pfnOrig)(hObject) : DeleteObject(hObject);
}

HBRUSH WINAPI CXTPSkinManagerApiHook::OnHookGetSysColorBrush(  int nIndex)
{
	typedef HBRUSH (WINAPI* LPFNGETSYSCOLORBRUSH)(int);

	CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();

	if (XTPSkinManager()->IsEnabled() && nIndex > 0 && nIndex < XTP_SKINMETRICS_COLORTABLESIZE && pMetrics->m_brTheme[nIndex])
	{
		return pMetrics->m_brTheme[nIndex];
	}

	PROC pfnOrig = GetOriginalProc(xtpSkinApiGetSysColorBrush);

	HBRUSH nResult = pfnOrig ? ((LPFNGETSYSCOLORBRUSH)pfnOrig)(nIndex) : GetSysColorBrush(nIndex);

	return nResult;

}

int WINAPI CXTPSkinManagerApiHook::OnHookSetScrollPos( HWND hwnd,  int nBar,   int nPos,  BOOL redraw)
{

	typedef int(WINAPI* LPFNSETSCROLLPOS)(  HWND hWnd,  int nBar,  int nPos,  BOOL bRedraw);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiSetScrollPos);
	if (!pfnOrig)
		return SetScrollPos(hwnd, nBar, nPos, redraw);

	CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)XTPSkinManager()->Lookup(hwnd);

	if (!pFrame)
		return pfnOrig ? ((LPFNSETSCROLLPOS)pfnOrig)(hwnd, nBar, nPos, redraw) : 0;

	int nResult = pfnOrig ? ((LPFNSETSCROLLPOS)pfnOrig)(hwnd, nBar, nPos, FALSE) : 0;

	if (redraw)
	{
		if (nBar == SB_CTL) InvalidateRect(hwnd, NULL, FALSE);
		else pFrame->RedrawScrollBar(nBar);
	}


	return nResult;

}


int WINAPI CXTPSkinManagerApiHook::OnHookSetScrollInfo( HWND hwnd,  int nBar,  LPCSCROLLINFO lpsi,  BOOL redraw)
{
	typedef int(WINAPI* LPFNSETSCROLLINFO)(HWND hwnd, int fnBar, LPCSCROLLINFO lpsi, BOOL fRedraw);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiSetScrollInfo);
	if (!pfnOrig)
		return SetScrollInfo(hwnd, nBar, lpsi, redraw);

	CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)XTPSkinManager()->Lookup(hwnd);

	if (!pFrame || (nBar == SB_CTL) || !redraw)
		return ((LPFNSETSCROLLINFO)pfnOrig)(hwnd, nBar, lpsi, redraw);

	int nResult = ((LPFNSETSCROLLINFO)pfnOrig)(hwnd, nBar, lpsi, FALSE);
	pFrame->RedrawScrollBar(nBar);

	return nResult;
}

BOOL WINAPI CXTPSkinManagerApiHook::OnHookEnableScrollBar(HWND hWnd, UINT wSBflags, UINT wArrows)
{
	typedef int(WINAPI* LPFNENABLESCROLLBAR)(HWND hWnd, UINT wSBflags, UINT wArrows);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiEnableScrollBar);
	if (!pfnOrig)
		return EnableScrollBar(hWnd, wSBflags, wArrows);

	CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)XTPSkinManager()->Lookup(hWnd);

	if (!pFrame)
		return pfnOrig ? ((LPFNENABLESCROLLBAR)pfnOrig)(hWnd, wSBflags, wArrows) : 0;

	int nResult = pfnOrig ? ((LPFNENABLESCROLLBAR)pfnOrig)(hWnd, wSBflags, wArrows) : 0;

	if (wSBflags == SB_BOTH || wSBflags == SB_HORZ)
		pFrame->RedrawScrollBar(SB_HORZ);

	if (wSBflags == SB_BOTH || wSBflags == SB_VERT)
		pFrame->RedrawScrollBar(SB_VERT);


	return nResult;
}

BOOL WINAPI CXTPSkinManagerApiHook::OnHookGetScrollInfo(HWND hWnd, int nBar, LPSCROLLINFO lpsi)
{

	typedef BOOL(WINAPI* LPFNGETSCROLLINFO)(HWND hwnd, int nBar, LPCSCROLLINFO lpsi);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiGetScrollInfo);
	if (!pfnOrig)
		return GetScrollInfo(hWnd, nBar, lpsi);

	BOOL nResult = pfnOrig ? ((LPFNGETSCROLLINFO)pfnOrig)(hWnd, nBar, lpsi) : FALSE;

	if (lpsi && (lpsi->fMask & SIF_TRACKPOS))
	{
		CXTPSkinObject* pSink = XTPSkinManager()->Lookup(hWnd);

		if (pSink)
		{
			XTP_SKINSCROLLBARTRACKINFO* pSBTrack = ((CXTPSkinObjectFrame*)pSink)->GetScrollBarTrackInfo();

			if (pSBTrack && (pSBTrack->nBar == nBar))
			{
				// posNew is in the context of psbiSB's window and bar code
				lpsi->nTrackPos = pSBTrack->posNew;
			}
		}

	}

	return nResult;
}

BOOL CXTPSkinManagerApiHook::IsSystemWindowModule(WNDPROC lpWndProc, BOOL* pbAvail)
{
	MEMORY_BASIC_INFORMATION mbi;
	BOOL bAvail = (BOOL)VirtualQuery(lpWndProc, &mbi, sizeof(mbi));

	if (pbAvail) *pbAvail = bAvail;

	if (bAvail)
	{
		if ((HMODULE)mbi.AllocationBase == GetModuleHandle(_T("COMCTL32.DLL")))
			return TRUE;

		if ((HMODULE)mbi.AllocationBase == GetModuleHandle(_T("USER32.DLL")))
			return TRUE;
	}
	return FALSE;
}

BOOL CXTPSkinManagerApiHook::CallHookDefWindowProc(HWND hWnd, PROC pfnOrig, XTPSkinDefaultProc defProc, LPVOID lpPrev, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult)
{
	if (!XTPSkinManager()->IsEnabled())
		return FALSE;

	CXTPSkinObject* pSkinObject = XTPSkinManager()->Lookup(hWnd);

	if (!pSkinObject || pSkinObject->m_bCustomDraw)
		return FALSE;

	if (nMessage == pSkinObject->GetHeadMessage())
		return FALSE;

	if (defProc == xtpSkinDefaultCallWindowProc && pSkinObject->IsDefWindowProcAvail(nMessage))
	{
		BOOL bAvail = FALSE;
		WNDPROC lpWndProc = (WNDPROC)lpPrev;

		if (IsSystemWindowModule(lpWndProc, &bAvail) || !bAvail)
		{
			lpWndProc = 0;
		}

		if (lpWndProc != 0 && (!XTPSkinManager()->IsWin9x() ||
			((DWORD_PTR)lpWndProc < (DWORD_PTR)0x70000000)))
		{
			return FALSE;
		}
	}

	MSG& curMsg = AfxGetThreadState()->m_lastSentMsg;
	MSG  oldMsg = curMsg;
	curMsg.hwnd = hWnd;
	curMsg.message = nMessage;
	curMsg.wParam  = wParam;
	curMsg.lParam  = lParam;

	pSkinObject->OnBeginHook(nMessage, defProc, pfnOrig, lpPrev);

	BOOL bResult = pSkinObject->OnHookDefWindowProc(nMessage, wParam ,lParam, lResult);

	pSkinObject->OnEndHook();

	curMsg = oldMsg;

	if (bResult)
		return TRUE;

	return FALSE;
}

LRESULT WINAPI CXTPSkinManagerApiHook::OnHookCallWindowProcW(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PROC pfnOrig = GetOriginalProc(xtpSkinApiCallWindowProcW);
	if (!pfnOrig)
		return CallWindowProcW(lpPrevWndFunc, hWnd, Msg, wParam, lParam);

	LRESULT lResult = 0;
	if (lpPrevWndFunc && GetInstance()->CallHookDefWindowProc(hWnd, pfnOrig, xtpSkinDefaultCallWindowProc, lpPrevWndFunc, Msg, wParam ,lParam, lResult))
	{
		return lResult;
	}

	typedef LRESULT (WINAPI* LPFNCALLWINDOWPROC)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT bResult = ((LPFNCALLWINDOWPROC)pfnOrig)(lpPrevWndFunc, hWnd, Msg, wParam, lParam);

	return bResult;
}

LRESULT WINAPI CXTPSkinManagerApiHook::OnHookCallWindowProcA(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PROC pfnOrig = GetOriginalProc(xtpSkinApiCallWindowProcA);
	if (!pfnOrig)
		return CallWindowProcA(lpPrevWndFunc, hWnd, Msg, wParam, lParam);

	LRESULT lResult = 0;
	if (lpPrevWndFunc && GetInstance()->CallHookDefWindowProc(hWnd, pfnOrig, xtpSkinDefaultCallWindowProc, lpPrevWndFunc, Msg, wParam ,lParam, lResult))
	{
		return lResult;
	}

	typedef LRESULT (WINAPI* LPFNCALLWINDOWPROC)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	LRESULT bResult = ((LPFNCALLWINDOWPROC)pfnOrig)(lpPrevWndFunc, hWnd, Msg, wParam, lParam);

	return bResult;
}

LRESULT WINAPI CXTPSkinManagerApiHook::OnHookDefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PROC pfnOrig = GetOriginalProc(xtpSkinApiDefWindowProcA);
	if (!pfnOrig)
		return DefWindowProcA(hWnd, Msg, wParam, lParam);

	LRESULT lResult = 0;
	if (GetInstance()->CallHookDefWindowProc(hWnd, pfnOrig, xtpSkinDefaultDefWindowProc, NULL, Msg, wParam ,lParam, lResult))
	{
		return lResult;
	}

	typedef LRESULT (WINAPI* LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	return ((LPFNDEFWINDOWPROC)pfnOrig)(hWnd, Msg, wParam, lParam);
}

LRESULT WINAPI CXTPSkinManagerApiHook::OnHookDefWindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PROC pfnOrig = GetOriginalProc(xtpSkinApiDefWindowProcW);
	if (!pfnOrig)
		return DefWindowProcW(hWnd, Msg, wParam, lParam);

	LRESULT lResult = 0;
	if (GetInstance()->CallHookDefWindowProc(hWnd, pfnOrig, xtpSkinDefaultDefWindowProc, NULL, Msg, wParam ,lParam, lResult))
	{
		return lResult;
	}

	typedef LRESULT (WINAPI* LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	return ((LPFNDEFWINDOWPROC)pfnOrig)(hWnd, Msg, wParam, lParam);
}

LRESULT WINAPI CXTPSkinManagerApiHook::OnHookDefFrameProcA(HWND hWnd, HWND hWndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PROC pfnOrig = GetOriginalProc(xtpSkinApiDefFrameProcA);
	if (!pfnOrig)
		return DefFrameProcA(hWnd, hWndMDIClient, Msg, wParam, lParam);

	LRESULT lResult = 0;
	if (GetInstance()->CallHookDefWindowProc(hWnd, pfnOrig, xtpSkinDefaultDefFrameProc, (LPVOID)hWndMDIClient, Msg, wParam ,lParam, lResult))
	{
		return lResult;
	}

	typedef LRESULT (WINAPI* LPFNDEFWINDOWPROC)(HWND hWnd, HWND hWndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam);
	return ((LPFNDEFWINDOWPROC)pfnOrig)(hWnd, hWndMDIClient, Msg, wParam, lParam);
}

LRESULT WINAPI CXTPSkinManagerApiHook::OnHookDefFrameProcW(HWND hWnd, HWND hWndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PROC pfnOrig = GetOriginalProc(xtpSkinApiDefFrameProcW);
	if (!pfnOrig)
		return DefFrameProcW(hWnd, hWndMDIClient, Msg, wParam, lParam);

	LRESULT lResult = 0;
	if (GetInstance()->CallHookDefWindowProc(hWnd, pfnOrig, xtpSkinDefaultDefFrameProc, (LPVOID)hWndMDIClient, Msg, wParam ,lParam, lResult))
	{
		return lResult;
	}

	typedef LRESULT (WINAPI* LPFNDEFWINDOWPROC)(HWND hWnd, HWND hWndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam);
	return ((LPFNDEFWINDOWPROC)pfnOrig)(hWnd, hWndMDIClient, Msg, wParam, lParam);
}

LRESULT WINAPI CXTPSkinManagerApiHook::OnHookDefDlgProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PROC pfnOrig = GetOriginalProc(xtpSkinApiDefDlgProcA);
	if (!pfnOrig)
		return DefDlgProcA(hWnd, Msg, wParam, lParam);

	LRESULT lResult = 0;
	if (GetInstance()->CallHookDefWindowProc(hWnd, pfnOrig, xtpSkinDefaultDefDlgProc, NULL, Msg, wParam ,lParam, lResult))
	{
		return lResult;
	}

	typedef LRESULT (WINAPI* LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	return ((LPFNDEFWINDOWPROC)pfnOrig)(hWnd, Msg, wParam, lParam);
}

LRESULT WINAPI CXTPSkinManagerApiHook::OnHookDefDlgProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PROC pfnOrig = GetOriginalProc(xtpSkinApiDefDlgProcW);
	if (!pfnOrig)
		return DefDlgProcW(hWnd, Msg, wParam, lParam);

	LRESULT lResult = 0;
	if (GetInstance()->CallHookDefWindowProc(hWnd, pfnOrig, xtpSkinDefaultDefDlgProc, NULL, Msg, wParam ,lParam, lResult))
	{
		return lResult;
	}

	typedef LRESULT (WINAPI* LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	return ((LPFNDEFWINDOWPROC)pfnOrig)(hWnd, Msg, wParam, lParam);
}

LRESULT WINAPI CXTPSkinManagerApiHook::OnHookDefMDIChildProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PROC pfnOrig = GetOriginalProc(xtpSkinApiDefMDIChildProcA);
	if (!pfnOrig)
		return DefMDIChildProcA(hWnd, Msg, wParam, lParam);

	LRESULT lResult = 0;
	if (GetInstance()->CallHookDefWindowProc(hWnd, pfnOrig, xtpSkinDefaultDefMDIChildProc, NULL, Msg, wParam ,lParam, lResult))
	{
		return lResult;
	}

	typedef LRESULT (WINAPI* LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	return ((LPFNDEFWINDOWPROC)pfnOrig)(hWnd, Msg, wParam, lParam);
}

LRESULT WINAPI CXTPSkinManagerApiHook::OnHookDefMDIChildProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PROC pfnOrig = GetOriginalProc(xtpSkinApiDefMDIChildProcW);
	if (!pfnOrig)
		return DefMDIChildProcW(hWnd, Msg, wParam, lParam);

	LRESULT lResult = 0;
	if (GetInstance()->CallHookDefWindowProc(hWnd, pfnOrig, xtpSkinDefaultDefMDIChildProc, NULL, Msg, wParam ,lParam, lResult))
	{
		return lResult;
	}

	typedef LRESULT (WINAPI* LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	return ((LPFNDEFWINDOWPROC)pfnOrig)(hWnd, Msg, wParam, lParam);
}





struct XTP_SKINFRAMEWORK_THREADPROCPARAMETER
{
	LPTHREAD_START_ROUTINE lpStartAddress;
	LPVOID lpParameter;
};

DWORD WINAPI CXTPSkinManagerApiHook::ThreadProcHook(LPVOID lpThreadData)
{

	XTP_SKINFRAMEWORK_THREADPROCPARAMETER* parameter = (XTP_SKINFRAMEWORK_THREADPROCPARAMETER*)lpThreadData;
	LPTHREAD_START_ROUTINE lpStartAddress = parameter->lpStartAddress;
	LPVOID lpParam = parameter->lpParameter;
	delete lpThreadData;

	XTPSkinManager()->EnableCurrentThread();

	return (lpStartAddress)(lpParam);
}

HANDLE STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookCreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
	UINT_PTR dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
	DWORD dwCreationFlags, LPDWORD lpThreadId)
{
	typedef HANDLE (WINAPI* LPFNCREATETHREAD)(LPSECURITY_ATTRIBUTES lpThreadAttributes,
		UINT_PTR dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
		DWORD dwCreationFlags, LPDWORD lpThreadId);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiCreateThread);
	if (!pfnOrig)
		return CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);

	if (XTPSkinManager()->IsEnabled() && XTPSkinManager()->GetAutoApplyNewThreads())
	{

		XTP_SKINFRAMEWORK_THREADPROCPARAMETER* pParam = new XTP_SKINFRAMEWORK_THREADPROCPARAMETER;
		pParam->lpParameter = lpParameter;
		pParam->lpStartAddress = lpStartAddress;

		return ((LPFNCREATETHREAD)pfnOrig)(lpThreadAttributes,
			dwStackSize, ThreadProcHook, pParam,
			dwCreationFlags, lpThreadId);
	}

	return ((LPFNCREATETHREAD)pfnOrig)(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
}


ATOM WINAPI CXTPSkinManagerApiHook::OnHookRegisterClassA ( const WNDCLASSA *lpWndClass)
{
	typedef ATOM(WINAPI* LPFNREGISTERCLASSA)(const WNDCLASSA* lpWndClass);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiRegisterClassA);
	if (!pfnOrig)
		return RegisterClassA(lpWndClass);

	CXTPSkinManagerApiFunction* pDefWindowFunction = NULL;
	if (lpWndClass && lpWndClass->lpfnWndProc == (WNDPROC)&CXTPSkinManagerApiHook::OnHookDefWindowProcA)
	{
		pDefWindowFunction = GetInstance()->GetHookedFunction(xtpSkinApiDefWindowProcA);
	}
	if (lpWndClass && lpWndClass->lpfnWndProc == (WNDPROC)&CXTPSkinManagerApiHook::OnHookDefFrameProcA)
	{
		pDefWindowFunction = GetInstance()->GetHookedFunction(xtpSkinApiDefFrameProcA);
	}
	if (lpWndClass && lpWndClass->lpfnWndProc == (WNDPROC)&CXTPSkinManagerApiHook::OnHookDefMDIChildProcA)
	{
		pDefWindowFunction = GetInstance()->GetHookedFunction(xtpSkinApiDefMDIChildProcA);
	}
	if (lpWndClass && lpWndClass->lpfnWndProc == (WNDPROC)&CXTPSkinManagerApiHook::OnHookDefDlgProcA)
	{
		pDefWindowFunction = GetInstance()->GetHookedFunction(xtpSkinApiDefDlgProcA);
	}

	if (pDefWindowFunction)
	{
		WNDCLASSA wc;
		wc = *lpWndClass;
		wc.lpfnWndProc = (WNDPROC)pDefWindowFunction->m_pfnOrig;

		return ((LPFNREGISTERCLASSA)pfnOrig)(&wc);
	}

	return ((LPFNREGISTERCLASSA)pfnOrig)(lpWndClass);
}

ATOM WINAPI CXTPSkinManagerApiHook::OnHookRegisterClassW ( const WNDCLASSW *lpWndClass)
{
	typedef ATOM(WINAPI* LPFNREGISTERCLASSW)(const WNDCLASSW* lpWndClass);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiRegisterClassW);
	if (!pfnOrig)
		return RegisterClassW(lpWndClass);

	CXTPSkinManagerApiFunction* pDefWindowFunction = NULL;
	if (lpWndClass && lpWndClass->lpfnWndProc == (WNDPROC)&CXTPSkinManagerApiHook::OnHookDefWindowProcW)
	{
		pDefWindowFunction = GetInstance()->GetHookedFunction(xtpSkinApiDefWindowProcW);
	}
	if (lpWndClass && lpWndClass->lpfnWndProc == (WNDPROC)&CXTPSkinManagerApiHook::OnHookDefFrameProcW)
	{
		pDefWindowFunction = GetInstance()->GetHookedFunction(xtpSkinApiDefFrameProcW);
	}
	if (lpWndClass && lpWndClass->lpfnWndProc == (WNDPROC)&CXTPSkinManagerApiHook::OnHookDefMDIChildProcW)
	{
		pDefWindowFunction = GetInstance()->GetHookedFunction(xtpSkinApiDefMDIChildProcW);
	}
	if (lpWndClass && lpWndClass->lpfnWndProc == (WNDPROC)&CXTPSkinManagerApiHook::OnHookDefDlgProcW)
	{
		pDefWindowFunction = GetInstance()->GetHookedFunction(xtpSkinApiDefDlgProcW);
	}

	if (pDefWindowFunction)
	{
		WNDCLASSW wc;
		wc = *lpWndClass;
		wc.lpfnWndProc = (WNDPROC)pDefWindowFunction->m_pfnOrig;

		return ((LPFNREGISTERCLASSW)pfnOrig)(&wc);
	}

	return ((LPFNREGISTERCLASSW)pfnOrig)(lpWndClass);

}

BOOL WINAPI CXTPSkinManagerApiHook::OnHookSystemParametersInfo (UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
	typedef BOOL(WINAPI* LPFNSYSTEMPARAMETERSINFO )(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiSystemParametersInfo);
	if (!pfnOrig)
		return SystemParametersInfo(uiAction, uiParam, pvParam, fWinIni);


	BOOL bResult = pfnOrig ? ((LPFNSYSTEMPARAMETERSINFO)pfnOrig)(uiAction, uiParam, pvParam, fWinIni) : FALSE;

	if (uiAction == SPI_GETICONTITLELOGFONT && uiParam == sizeof(LOGFONT))
	{
		LOGFONT* pLogFont = (LOGFONT*)pvParam;

		CXTPSkinManagerSchema* pSchema = XTPSkinManager()->GetSchema();
		UINT nSysMetrics = pSchema->GetClassCode(_T("SYSMETRICS"));

		pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_ICONTITLEFONT, *pLogFont);
	}

	if (uiAction == SPI_GETNONCLIENTMETRICS && uiParam == sizeof(NONCLIENTMETRICS))
	{
		NONCLIENTMETRICS* pncm = (NONCLIENTMETRICS*)pvParam;

		CXTPSkinManagerSchema* pSchema = XTPSkinManager()->GetSchema();
		UINT nSysMetrics = pSchema->GetClassCode(_T("SYSMETRICS"));

		pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_MENUFONT, pncm->lfMenuFont);
		pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_STATUSFONT, pncm->lfStatusFont);
		pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_CAPTIONFONT, pncm->lfCaptionFont);

	}

	return bResult;
}


int WINAPI CXTPSkinManagerApiHook::OnHookFillRect(HDC hDC, CONST RECT *lprc, HBRUSH hbr)
{
	typedef int (WINAPI* LPFNFILLRECT)(HDC hDC, CONST RECT *lprc, HBRUSH hbr);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiFillRect);
	if (!pfnOrig)
		return ::FillRect(hDC, lprc, hbr);

	if ((DWORD_PTR)hbr > 0 && (DWORD_PTR)hbr < XTP_SKINMETRICS_COLORTABLESIZE)
	{
		hbr = XTPSkinManager()->GetMetrics()->m_brTheme[(DWORD_PTR)hbr - 1];
	}

	if (pfnOrig)
	{
		return ((LPFNFILLRECT)pfnOrig)(hDC, lprc, hbr);
	}

	return 0;
}

BOOL WINAPI CXTPSkinManagerApiHook::OnHookDrawEdge (HDC hdc, LPRECT lprc, UINT edge, UINT flags)
{
	return XTRSkinFrameworkDrawEdge(hdc, lprc, edge, flags);
}

BOOL WINAPI CXTPSkinManagerApiHook::OnHookDrawFrameControl(HDC hdc, LPRECT lprc, UINT uType, UINT uState)
{
	if (XTRSkinFrameworkDrawFrameControl(hdc, lprc, uType, uState))
		return TRUE;

	typedef BOOL (WINAPI* LPFNDRAWFRAMECONTROL)(HDC, LPRECT, UINT, UINT);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiDrawFrameControl);
	if (!pfnOrig)
		return DrawFrameControl(hdc, lprc, uType, uState);

	return ((LPFNDRAWFRAMECONTROL)pfnOrig)(hdc, lprc, uType, uState);
}


void CXTPSkinManagerApiHook::HackModuleOnLoad(HMODULE hmod, DWORD dwFlags)
{
	if (IsModuleExcluded(hmod, FALSE))
		return;

	if ((hmod != NULL) && ((dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0))
	{
		for (int i = 0; i < XTP_SKIN_APIHOOKCOUNT; i++)
		{
			CXTPSkinManagerApiFunction* pHook = m_arrFunctions[i];

			if (pHook)
			{
				if (pHook->ReplaceInOneModule(pHook->m_szCalleeModName, pHook->m_pfnOrig, pHook->m_pfnHook, hmod))
				{
					pHook->m_bHooked = TRUE;
				}
			}
		}
	}
}

HMODULE WINAPI CXTPSkinManagerApiHook::OnHookLoadLibraryA(LPCSTR pszModuleName)
{
	typedef HMODULE (WINAPI* LPFNLOADLIBRARYA)(LPCSTR pszModuleName);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiLoadLibraryA);
	if (!pfnOrig)
		return ::LoadLibraryA(pszModuleName);

	HMODULE hmod = ((LPFNLOADLIBRARYA)pfnOrig)(pszModuleName);

	GetInstance()->HackModuleOnLoad(hmod, 0);

	return hmod;
}

HMODULE WINAPI CXTPSkinManagerApiHook::OnHookLoadLibraryW(PCWSTR pszModuleName)
{
	typedef HMODULE (WINAPI* LPFNLOADLIBRARYW)(PCWSTR pszModuleName);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiLoadLibraryW);
	if (!pfnOrig)
		return ::LoadLibraryW(pszModuleName);

	HMODULE hmod = ((LPFNLOADLIBRARYW)pfnOrig)(pszModuleName);

	GetInstance()->HackModuleOnLoad(hmod, 0);

	return hmod;
}

HMODULE WINAPI CXTPSkinManagerApiHook::OnHookLoadLibraryExA(LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	typedef HMODULE (WINAPI* LPFNLOADLIBRARYEXA)(LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiLoadLibraryExA);
	if (!pfnOrig)
		return ::LoadLibraryExA(pszModuleName, hFile, dwFlags);

	HMODULE hmod = ((LPFNLOADLIBRARYEXA)pfnOrig)(pszModuleName, hFile, dwFlags);


	GetInstance()->HackModuleOnLoad(hmod, 0);

	return hmod;
}

HMODULE WINAPI CXTPSkinManagerApiHook::OnHookLoadLibraryExW(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	typedef HMODULE (WINAPI* LPFNLOADLIBRARYEXW)(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiLoadLibraryExW);
	if (!pfnOrig)
		return ::LoadLibraryExW(pszModuleName, hFile, dwFlags);

	HMODULE hmod = ((LPFNLOADLIBRARYEXW)pfnOrig)(pszModuleName, hFile, dwFlags);

	GetInstance()->HackModuleOnLoad(hmod, dwFlags);

	return hmod;
}

HMODULE WINAPI CXTPSkinManagerApiHook::OnHookGetModuleHandleA(LPCSTR lpModuleName)
{
	if (lpModuleName && XTPCompareStringNoCase(lpModuleName, "UxTheme.dll") == 0)
	if (XTPSkinManager()->GetApplyOptions() & xtpSkinApplyColors)
		return XTP_UXTHEME_HANDLE;

	typedef HMODULE (WINAPI* LPFNGETMODULEHANDLEA)(LPCSTR lpModuleName);

	PROC pfnOrig = GetOriginalProc(xtpSkinApiGetModuleHandleA);

	if (pfnOrig)
		return ((LPFNGETMODULEHANDLEA)pfnOrig)(lpModuleName);

	return ::GetModuleHandleA(lpModuleName);
}

FARPROC WINAPI CXTPSkinManagerApiHook::OnHookGetProcAddress(HMODULE hmod, PCSTR pszProcName)
{
	if ((DWORD_PTR)pszProcName < 0xFFFF)
		return GetProcAddressWindows(hmod, pszProcName);

	CXTPSkinManagerApiFunction* pFuncHook = NULL;
	if (hmod == XTP_UXTHEME_HANDLE)
	{
		pFuncHook = GetInstance()->GetHookedFunction("UXTHEME.DLL", pszProcName);
	}
	else
	{
		char szFullFileName[MAX_PATH];
		::GetModuleFileNameA(hmod, szFullFileName, MAX_PATH);
		// We must extract only the name and the extension

		char* szFileName = strrchr(szFullFileName, '\\');
		if (szFileName == NULL)
			szFileName = szFullFileName;
		else
			szFileName = szFileName + 1;

		if (XTPCompareStringNoCase(szFileName, "UxTheme.dll") == 0)
			return GetProcAddressWindows(hmod, pszProcName);

		pFuncHook = GetInstance()->GetHookedFunction(szFileName, pszProcName);
	}


	if (pFuncHook)
		return pFuncHook->m_pfnHook;

	return GetProcAddressWindows(hmod, pszProcName);
}



HTHEME STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookOpenThemeData(HWND /*hWnd*/, LPCWSTR pszClassList)
{
	CXTPSkinManager* pManager = XTPSkinManager();

	if (!pManager->IsEnabled())
		return 0;

	CXTPSkinManagerClass* pClass = pManager->GetSkinClass(CString(pszClassList));
	return (HTHEME)pClass;
}

HRESULT STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookDrawThemeBackground(HTHEME hTheme, HDC hDC, int iPartId, int iStateId, const RECT* pRect, const RECT*)
{
	if (!hTheme)
		return E_FAIL;

	CXTPSkinManagerClass* pClass = XTPSkinManager()->FromHandle(hTheme);
	if (!pClass)
		return E_FAIL;

	if (!pClass->DrawThemeBackground(CDC::FromHandle(hDC), iPartId, iStateId, pRect))
		return E_FAIL;

	return S_OK;
}


HRESULT STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookCloseThemeData(HTHEME)
{
	return S_OK;
}

HRESULT STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookGetThemeColor(HTHEME hTheme, int iPartId, int iStateId, int iPropID, COLORREF *pColor)
{
	if (!hTheme)
		return E_FAIL;

	CXTPSkinManagerClass* pClass = XTPSkinManager()->FromHandle(hTheme);
	if (!pClass)
		return E_FAIL;

	COLORREF clr = pClass->GetThemeColor(iPartId, iStateId, iPropID);

	if (clr == COLORREF_NULL)
		return E_FAIL;

	*pColor = clr;
	return S_OK;
}

BOOL STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookIsThemeActive()
{
	return TRUE;
}

BOOL STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookIsAppThemed()
{
	return TRUE;
}

HRESULT STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookGetThemePartSize(HTHEME hTheme, HDC, int iPartId, int iStateId, RECT *pRect, THEMESIZE eSize, SIZE* pSize)
{
	if (!hTheme)
		return E_FAIL;

	CXTPSkinManagerClass* pClass = XTPSkinManager()->FromHandle(hTheme);
	if (!pClass)
		return E_FAIL;

	if (!pClass->GetThemePartSize(iPartId, iStateId, pRect, eSize, pSize))
		return E_FAIL;

	return S_OK;
}


BOOL STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookGetThemeSysBool(HTHEME /*hTheme*/, int iBoolId)
{
	return XTPSkinManager()->GetThemeSysBool(iBoolId);
}

COLORREF STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookGetThemeSysColor(HTHEME /*hTheme*/, int iColorId)
{
	return XTPSkinManager()->GetThemeSysColor(iColorId);
}


HRESULT STDAPICALLTYPE CXTPSkinManagerApiHook::OnHookGetCurrentThemeName(LPWSTR pszThemeFileName, int dwMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR /*pszSizeBuff*/, int /*cchMaxSizeChars*/)
{
	CXTPSkinManager* pManager = XTPSkinManager();

	if (!pManager->GetResourceFile())
		return E_FAIL;

	if (pszThemeFileName)
	{
		CString strThemeFileName = pManager->GetResourceFile()->GetResourcePath();
		int nLength = strThemeFileName.GetLength();

		if (nLength < dwMaxNameChars)
		{
			MBSTOWCS_S(pszThemeFileName, strThemeFileName, nLength + 1);
		}
	}

	if (pszColorBuff)
	{
		CString strColorBuff = pManager->GetResourceFile()->GetIniFileName();
		strColorBuff.MakeUpper();

		if (strColorBuff.Find(_T("METALLIC")) > 0) strColorBuff = _T("metallic");
		if (strColorBuff.Find(_T("BLUE"))  > 0) strColorBuff = _T("normalcolor");
		if (strColorBuff.Find(_T("HOMESTEAD"))  > 0) strColorBuff = _T("homestead");


		int nLength = strColorBuff.GetLength();

		if (nLength < cchMaxColorChars)
		{
			MBSTOWCS_S(pszColorBuff, strColorBuff, nLength + 1);
		}

	}
	return S_OK;
}
