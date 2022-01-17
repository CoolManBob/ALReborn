// XTPSkinManagerApiHook.h: interface for the CXTPSkinManagerApiHook class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSKINMANAGERAPIHOOK_H__)
#define __XTPSKINMANAGERAPIHOOK_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObject.h"

//{{AFX_CODEJOCK_PRIVATE

enum XTPSkinFrameworkApiFunctionIndex
{
	xtpSkinApiLoadLibraryA,
	xtpSkinApiLoadLibraryW,
	xtpSkinApiLoadLibraryExA,
	xtpSkinApiLoadLibraryExW,
	xtpSkinApiGetProcAddress,
	xtpSkinApiGetModuleHandleA,
	xtpSkinApiRegisterClassA,
	xtpSkinApiRegisterClassW,
	xtpSkinApiGetSysColor,
	xtpSkinApiGetSysColorBrush,
	xtpSkinApiDrawEdge,
	xtpSkinApiDrawFrameControl,
	xtpSkinApiSetScrollInfo,
	xtpSkinApiSetScrollPos,
	xtpSkinApiGetScrollInfo,
	xtpSkinApiEnableScrollBar,
	xtpSkinApiOpenThemeData,
	xtpSkinApiDrawThemeBackground,
	xtpSkinApiCloseThemeData,
	xtpSkinApiGetThemeColor,
	xtpSkinApiIsAppThemed,
	xtpSkinApiIsThemeActive,
	xtpSkinApiGetCurrentThemeName,
	xtpSkinApiGetThemeSysBool,
	xtpSkinApiGetThemeSysColor,
	xtpSkinApiGetThemePartSize,
	xtpSkinApiSystemParametersInfo,
	xtpSkinApiDefWindowProcA,
	xtpSkinApiDefWindowProcW,
	xtpSkinApiDefMDIChildProcA,
	xtpSkinApiDefMDIChildProcW,
	xtpSkinApiDefFrameProcA,
	xtpSkinApiDefFrameProcW,
	xtpSkinApiDefDlgProcA,
	xtpSkinApiDefDlgProcW,
	xtpSkinApiCallWindowProcA,
	xtpSkinApiCallWindowProcW,
	xtpSkinApiCreateThread,
	xtpSkinApiFillRect,
	xtpSkinApiDeleteObject,
	XTP_SKIN_APIHOOKCOUNT
};


class CXTPSkinManagerApiHook;

class CXTPSkinManagerApiFunction
{
public:
	CXTPSkinManagerApiFunction(CXTPSkinManagerApiHook* pApiHook,
		LPCSTR  pszCalleeModName, LPCSTR pszFuncName, PROC pfnOrig, PROC pfnHook);

	virtual ~CXTPSkinManagerApiFunction();

public:
	BOOL HookImport();
	BOOL UnhookImport();

private:
	BOOL ReplaceInOneModule(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller);
	BOOL ReplaceInAllModules(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew);

private:
	CXTPSkinManagerApiHook* m_pApiHook;
	BOOL              m_bHooked;
	char              m_szCalleeModName[20];
	char              m_szFuncName[30];
	PROC              m_pfnOrig;
	PROC              m_pfnHook;

	static PVOID          sm_pvMaxAppAddr;
	static PVOID          sm_pfnAfxWndProc;

	friend class CXTPSkinManagerApiHook;
	friend class CXTPSkinObject;
};




class CXTPSkinManagerApiHook
{
	struct EXCLUDEDMODULE
	{
		HMODULE hModule;
		CString strModule;
		BOOL bWin9x;
	};

private:
	CXTPSkinManagerApiHook();

public:
	static CXTPSkinManagerApiHook* AFX_CDECL GetInstance();
	virtual ~CXTPSkinManagerApiHook();

public:
	void ExcludeModule(LPCTSTR lpszModule, BOOL bWin9x);
	BOOL IsModuleExcluded(HMODULE hModule, BOOL bUseHandle) const;

public:
	void InitializeHookManagement();
	void FinalizeHookManagement();

public:
	CXTPSkinManagerApiFunction* GetHookedFunction(LPCSTR pszCalleeModName, LPCSTR pszFuncName);
	CXTPSkinManagerApiFunction* GetHookedFunction(XTPSkinFrameworkApiFunctionIndex nIndex);
	static PROC AFX_CDECL GetOriginalProc(XTPSkinFrameworkApiFunctionIndex nIndex);

public:
	static FARPROC WINAPI GetProcAddressWindows(HMODULE hmod, LPCSTR   pszProcName);
	static LRESULT WINAPI CallWindowProcOrig(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	static BOOL AFX_CDECL IsSystemWindowModule(WNDPROC lpWndProc, BOOL* pbAvail = NULL);

protected:
	void UnhookAllFunctions();
	CXTPSkinManagerApiFunction* HookImport(XTPSkinFrameworkApiFunctionIndex nIndex, LPCSTR pszCalleeModName, LPCSTR pszFuncName, PROC  pfnHook);


private:
	static HMODULE WINAPI OnHookGetModuleHandleA(LPCSTR lpModuleName);

	CXTPSkinManagerApiFunction* AddHook(XTPSkinFrameworkApiFunctionIndex nIndex, LPCSTR  pszCalleeModName, LPCSTR  pszFuncName, PROC pfnOrig, PROC pfnHook);

	BOOL RemoveHook(LPCSTR pszCalleeModName, LPCSTR pszFuncName);

	void HackModuleOnLoad(HMODULE hmod, DWORD dwFlags);

	static HMODULE WINAPI OnHookLoadLibraryW(PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryExA(LPCSTR  pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW(PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static FARPROC WINAPI OnHookGetProcAddress(HMODULE hmod, PCSTR pszProcName);
	static HMODULE WINAPI OnHookLoadLibraryA(LPCSTR pszModuleName);

	static ATOM WINAPI OnHookRegisterClassA( const WNDCLASSA *lpWndClass);
	static ATOM WINAPI OnHookRegisterClassW( const WNDCLASSW *lpWndClass);
	static int WINAPI OnHookSetScrollInfo(HWND hwnd, int fnBar, LPCSCROLLINFO lpsi, BOOL fRedraw);
	static int WINAPI OnHookSetScrollPos(HWND hwnd,  int nBar,   int nPos,  BOOL redraw);
	static BOOL WINAPI OnHookEnableScrollBar(HWND hWnd, UINT wSBflags, UINT wArrows);
	static BOOL WINAPI OnHookGetScrollInfo(HWND hwnd, int nBar, LPSCROLLINFO lpsi);
	static DWORD WINAPI OnHookGetSysColor(int nIndex);
	static BOOL WINAPI OnHookDeleteObject(HGDIOBJ hObject);
	static BOOL WINAPI OnHookDrawEdge (HDC hdc, LPRECT qrc, UINT edge, UINT grfFlags);
	static BOOL WINAPI OnHookDrawFrameControl(HDC hDC, LPRECT rect, UINT, UINT);
	static HBRUSH WINAPI OnHookGetSysColorBrush(int nIndex);
	static BOOL WINAPI OnHookSystemParametersInfo (UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni);
	static LRESULT WINAPI OnHookDefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefWindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefFrameProcA(HWND hWnd, HWND hWndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefFrameProcW(HWND hWnd, HWND hWndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefDlgProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefDlgProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefMDIChildProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefMDIChildProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookCallWindowProcA(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookCallWindowProcW(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static int WINAPI OnHookFillRect( HDC hDC, CONST RECT *lprc, HBRUSH hbr);

	static HRESULT STDAPICALLTYPE OnHookGetCurrentThemeName(LPWSTR pszThemeFileName, int dwMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR /*pszSizeBuff*/, int /*cchMaxSizeChars*/);
	static BOOL STDAPICALLTYPE OnHookGetThemeSysBool(HTHEME /*hTheme*/, int iBoolId);
	static COLORREF STDAPICALLTYPE OnHookGetThemeSysColor(HTHEME /*hTheme*/, int iColorId);
	static HRESULT STDAPICALLTYPE OnHookGetThemePartSize(HTHEME hTheme, HDC, int iPartId, int iStateId, RECT *pRect, THEMESIZE eSize, SIZE* pSize);
	static BOOL STDAPICALLTYPE OnHookIsAppThemed();
	static BOOL STDAPICALLTYPE OnHookIsThemeActive();
	static HRESULT STDAPICALLTYPE OnHookGetThemeColor(HTHEME hTheme, int iPartId, int iStateId, int iPropID, COLORREF *pColor);
	static HRESULT STDAPICALLTYPE OnHookCloseThemeData(HTHEME);
	static HRESULT STDAPICALLTYPE OnHookDrawThemeBackground(HTHEME hTheme, HDC hDC, int iPartId, int iStateId, const RECT* pRect, const RECT*);
	static HTHEME STDAPICALLTYPE OnHookOpenThemeData(HWND /*hWnd*/, LPCWSTR pszClassList);
	static HANDLE STDAPICALLTYPE OnHookCreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
		UINT_PTR dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
		DWORD dwCreationFlags, LPDWORD lpThreadId);
	static DWORD WINAPI ThreadProcHook(LPVOID lpParameter);

	BOOL CallHookDefWindowProc(HWND hWnd, PROC pfnOrig, XTPSkinDefaultProc defProc, LPVOID lpPrev, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

protected:
	CXTPSkinManagerApiFunction* m_arrFunctions[XTP_SKIN_APIHOOKCOUNT];
	BOOL m_bInitialized;
	CArray<EXCLUDEDMODULE, EXCLUDEDMODULE&> m_arrExcludedModules;

	friend class CXTPSkinManagerApiFunction;
};

//}}AFX_CODEJOCK_PRIVATE

#endif // !defined(__XTPSKINMANAGERAPIHOOK_H__)
