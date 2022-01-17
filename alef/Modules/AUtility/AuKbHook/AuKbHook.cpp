// FILE		: AuKbHook.cpp
// AUTHOR	: burumal
// DATE		: 2006/08/09

#include "stdafx.h"
#include "AuKbHook.h"

HHOOK g_hTheKeyHook = 0;

DWORD g_dwStickyKeysFlag = 0;
DWORD g_dwFilterKeysFlag = 0;

UINT g_nPrevSysParamInfo;

LRESULT CALLBACK _LowLevelKeyboardProc(INT nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(ul_reason_for_call);
	UNREFERENCED_PARAMETER(lpReserved);

    return TRUE;
}

bool DisableWindowsWinKey(bool bVal, HINSTANCE hInstance)
{
	if ( hInstance == NULL )
		return false;
	
	if ( bVal )
	{
		OSVERSIONINFO osVersionInfo;
		osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osVersionInfo);	

		if ( osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
		{
			if ( !g_hTheKeyHook )
				g_hTheKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, _LowLevelKeyboardProc, hInstance,0);

			if ( g_hTheKeyHook != NULL )
				return true;
		}
		else
		//if ( osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
		{
			// 95/98/ME
			SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, TRUE, &g_nPrevSysParamInfo, 0);

		#ifdef _DEBUG
			::MessageBox(NULL, "AuKbHook", "TRUE", MB_OK);
		#endif
		}
	}
	else
	{	
		UnhookWindowsHookEx(g_hTheKeyHook);
		
		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, FALSE, &g_nPrevSysParamInfo, 0);

	#ifdef _DEBUG
		::MessageBox(NULL, "AuKbHook", "FALSE", MB_OK);
	#endif

		return true;
	}

	return false;
}

LRESULT CALLBACK _LowLevelKeyboardProc(INT nCode, WPARAM wParam, LPARAM lParam)
{	
	if ( nCode < 0 || nCode != HC_ACTION )
		return CallNextHookEx( g_hTheKeyHook, nCode, wParam, lParam); 

	bool bEatKeystroke = false;

	switch ( wParam )
	{
	case WM_KEYDOWN :
	case WM_KEYUP :
		{
			KBDLLHOOKSTRUCT* pkbhs = (KBDLLHOOKSTRUCT*) lParam;
			bEatKeystroke = (pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN);
			break;
		}
	}

	if ( bEatKeystroke )
		return 1;

	return CallNextHookEx(g_hTheKeyHook, nCode, wParam, lParam);
} 

void DisableWindowsStickKeys(bool bEnable)
{		
	STICKYKEYS sStickKeys;

	if ( bEnable )
	{		
		if ( g_dwStickyKeysFlag != 0 )
			return;

		// 고정키 막기

		// 백업
		sStickKeys.cbSize = sizeof(STICKYKEYS);
		SystemParametersInfo( SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &sStickKeys, 0 );
		g_dwStickyKeysFlag = sStickKeys.dwFlags;

		// 설정
		sStickKeys.dwFlags &= ~(SKF_AVAILABLE|SKF_HOTKEYACTIVE);
		SystemParametersInfo( SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &sStickKeys, 0 );	
	}
	else
	{
		if ( g_dwStickyKeysFlag == 0 )
			return;

		// 고정키 복구

		STICKYKEYS sStickKeys;
		sStickKeys.cbSize = sizeof(STICKYKEYS);
		sStickKeys.dwFlags = g_dwStickyKeysFlag;
		SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &sStickKeys, 0);
	}
}

void DisableWindowsFilterKeys(bool bEnable)
{
	FILTERKEYS sFilterKeys;

	if ( bEnable )
	{
		if ( g_dwFilterKeysFlag != 0 )
			return;

		sFilterKeys.cbSize = sizeof(FILTERKEYS);
		SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &sFilterKeys, 0 );

		g_dwFilterKeysFlag = sFilterKeys.dwFlags;
		sFilterKeys.dwFlags &= ~(FKF_AVAILABLE | FKF_HOTKEYACTIVE);
		SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &sFilterKeys, 0);
	}
	else
	{
		sFilterKeys.cbSize = sizeof(FILTERKEYS);
		sFilterKeys.dwFlags = g_dwFilterKeysFlag;
		SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &sFilterKeys, 0);
	}
}
