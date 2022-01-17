// FILE		: AuKbHook.h
// AUTHOR	: burumal
// DATE		: 2006/08/09

#ifndef __AU_KBHOOK_H__
#define __AU_KBHOOK_H__

#ifdef AUKBHOOK_EXPORTS
	#define __DECLSPEC __declspec(dllexport)
#else
	#define __DECLSPEC __declspec(dllimport)
#endif

extern "C" __DECLSPEC bool DisableWindowsWinKey(bool bVal, HINSTANCE hInstance);
extern "C" __DECLSPEC void DisableWindowsStickKeys(bool bEnable);
extern "C" __DECLSPEC void DisableWindowsFilterKeys(bool bEnable);

#endif // __AU_KBHOOK_H__