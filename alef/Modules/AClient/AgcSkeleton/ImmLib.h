// -----------------------------------------------------------------------------
//  _____                     _      _ _         _     
// |_   _|                   | |    (_) |       | |    
//   | |  _ __ ___  _ __ ___ | |     _| |__     | |__  
//   | | | '_ ` _ \| '_ ` _ \| |    | | '_ \    | '_ \ 
//  _| |_| | | | | | | | | | | |____| | |_) | _ | | | |
// |_____|_| |_| |_|_| |_| |_|______|_|_.__/ (_)|_| |_|
//                                                     
//                                                     
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 07/27/2005 by daeyoung Cha
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_IMMLIB
#define _H_IMMLIB

#define WIN32_LEAN_AND_MEAN								// Exclude rarely-used stuff from Windows headers

// function pointer binding macro	..by dxsdk9.0 summer 2004
#define GETPROCADDRESS( Module, APIName, Temp ) \
    Temp = GetProcAddress( Module, #APIName ); \
    if( Temp ) \
        *(FARPROC*)&_##APIName = Temp

#define IMM32_DLLNAME "\\imm32.dll"
#define VER_DLLNAME "\\version.dll"

#pragma comment (lib , "Version.lib")					// Registry functions
#pragma comment (lib , "Imm32.lib")

// Windows Header Files:
#include <windows.h>

/* IME Header files */ 
#include <imm.h>
#include <objbase.h>
#include <dimm.h>

// type define function pointers: IMM32
typedef	INPUTCONTEXT*	(WINAPI * FPTR_ImmLockIMC)( HIMC );
typedef	BOOL			(WINAPI * FPTR_ImmUnlockIMC)( HIMC );
typedef	LPVOID			(WINAPI * FPTR_ImmLockIMCC)( HIMCC );
typedef	BOOL			(WINAPI * FPTR_ImmUnlockIMCC)( HIMCC );
typedef	DWORD			(WINAPI * FPTR_ImmGetProperty)( HKL, DWORD );
typedef	HIMC			(WINAPI * FPTR_ImmGetContext)( HWND );
typedef	BOOL			(WINAPI * FPTR_ImmReleaseContext)( HWND, HIMC );
typedef BOOL			(WINAPI * FPTR_ImmGetOpenStatus)( HIMC );
typedef BOOL			(WINAPI * FPTR_ImmSetOpenStatus)( HIMC , BOOL );
typedef	UINT			(WINAPI * FPTR_ImmGetIMEFileNameA)( HKL, LPSTR, UINT );
typedef	LONG			(WINAPI * FPTR_ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD );
typedef BOOL			(WINAPI * FPTR_ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD );
typedef DWORD			(WINAPI * FPTR_ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD );


// type define function pointers: Traditional Chinese IME
typedef	UINT			(WINAPI * FPTR_GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
typedef	BOOL			(WINAPI * FPTR_ShowReadingWindow)( HIMC, BOOL );

// type define function pointers: Verion library imports
typedef	BOOL			(APIENTRY * FPTR_VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT );
typedef	BOOL			(APIENTRY * FPTR_GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID );
typedef	DWORD			(APIENTRY * FPTR_GetFileVersionInfoSizeA)( LPSTR, LPDWORD );

// -----------------------------------------------------------------------------
// "imm32.dll"를 로드한 후 Imm함수포인터 바인딩한다.
// gpgstudy.com 의 IME 예제 참고..
class	CImmLib
{
	friend class AuInputComposer;

	// Construction/Destruction
public:
	CImmLib();
	virtual	~CImmLib();

	void	Init();
	void	SetupImeApi(HWND hWnd);

	// accessor
	void	SetHKL(LPARAM lParam)	{ m_hkl = (HKL)lParam; }
	HKL&	GetHKL()				{ return m_hkl;}
	
protected:

	// Data members
	static HINSTANCE				m_hDllImm32;	// IMM32 DLL handle
	static HINSTANCE				m_hDllIme;		// IMM DLL handle
    static HINSTANCE				m_hDllVer;		// Version DLL handle
    static HIMC						m_hImcDef;		// Default input context
	static HKL						m_hkl;			// Input locale identifier. #typedef HANDLE HKL

	//IMM32
	static FPTR_ImmLockIMC					_ImmLockIMC;
	static FPTR_ImmUnlockIMC				_ImmUnlockIMC;
	static FPTR_ImmLockIMCC					_ImmLockIMCC;
	static FPTR_ImmUnlockIMCC				_ImmUnlockIMCC;
	static FPTR_ImmGetProperty				_ImmGetProperty;
	static FPTR_ImmGetContext				_ImmGetContext;
	static FPTR_ImmReleaseContext			_ImmReleaseContext;
	static FPTR_ImmGetOpenStatus			_ImmGetOpenStatus;
	static FPTR_ImmSetOpenStatus			_ImmSetOpenStatus;
	static FPTR_ImmGetIMEFileNameA			_ImmGetIMEFileNameA;
	static FPTR_ImmGetCompositionStringW	_ImmGetCompositionStringW;
	static FPTR_ImmGetConversionStatus		_ImmGetConversionStatus;
	static FPTR_ImmGetCandidateListW		_ImmGetCandidateListW;

	// Traditional Chinese IME 
	static FPTR_GetReadingString			_GetReadingString;
	static FPTR_ShowReadingWindow			_ShowReadingWindow;

	// Verion library imports
	static FPTR_VerQueryValueA				_VerQueryValueA;
	static FPTR_GetFileVersionInfoA			_GetFileVersionInfoA;
	static FPTR_GetFileVersionInfoSizeA		_GetFileVersionInfoSizeA;


};

#endif // _H_IMMLIB
// -----------------------------------------------------------------------------
// ImmLib.h - End of file
// -----------------------------------------------------------------------------

