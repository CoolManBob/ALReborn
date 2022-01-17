// -----------------------------------------------------------------------------
//  _____                     _      _ _                          
// |_   _|                   | |    (_) |                         
//   | |  _ __ ___  _ __ ___ | |     _| |__       ___ _ __  _ __  
//   | | | '_ ` _ \| '_ ` _ \| |    | | '_ \     / __| '_ \| '_ \ 
//  _| |_| | | | | | | | | | | |____| | |_) | _ | (__| |_) | |_) |
// |_____|_| |_| |_|_| |_| |_|______|_|_.__/ (_) \___| .__/| .__/ 
//                                                   | |   | |    
//                                                   |_|   |_|    
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 07/27/2005 by daeyoung Cha
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------
#define _CRT_SECURE_NO_DEPRECATE
#include "ImmLib.h"

HINSTANCE		CImmLib::m_hDllImm32;	// IMM32 DLL handle
HINSTANCE		CImmLib::m_hDllIme;		
HINSTANCE		CImmLib::m_hDllVer;		// Version DLL handle
HIMC			CImmLib::m_hImcDef;		// Default input context
HKL				CImmLib::m_hkl;

//IMM32
FPTR_ImmLockIMC					CImmLib::_ImmLockIMC				= NULL;
FPTR_ImmUnlockIMC				CImmLib::_ImmUnlockIMC				= NULL;
FPTR_ImmLockIMCC				CImmLib::_ImmLockIMCC				= NULL;
FPTR_ImmUnlockIMCC				CImmLib::_ImmUnlockIMCC				= NULL;
FPTR_ImmGetProperty				CImmLib::_ImmGetProperty			= NULL;
FPTR_ImmGetContext				CImmLib::_ImmGetContext				= NULL;
FPTR_ImmReleaseContext			CImmLib::_ImmReleaseContext			= NULL;
FPTR_ImmGetOpenStatus			CImmLib::_ImmGetOpenStatus			= NULL;
FPTR_ImmSetOpenStatus			CImmLib::_ImmSetOpenStatus			= NULL;
FPTR_ImmGetIMEFileNameA			CImmLib::_ImmGetIMEFileNameA		= NULL;
FPTR_ImmGetCompositionStringW	CImmLib::_ImmGetCompositionStringW	= NULL;
FPTR_ImmGetConversionStatus		CImmLib::_ImmGetConversionStatus	= NULL;
FPTR_ImmGetCandidateListW		CImmLib::_ImmGetCandidateListW		= NULL;

// Traditional Chinese IME 
FPTR_GetReadingString			CImmLib::_GetReadingString			= NULL;
FPTR_ShowReadingWindow			CImmLib::_ShowReadingWindow			= NULL;

// Verion library imports
FPTR_VerQueryValueA				CImmLib::_VerQueryValueA			= NULL;
FPTR_GetFileVersionInfoA		CImmLib::_GetFileVersionInfoA		= NULL;
FPTR_GetFileVersionInfoSizeA	CImmLib::_GetFileVersionInfoSizeA	= NULL;

// -----------------------------------------------------------------------------
CImmLib::CImmLib() 
{
}

// -----------------------------------------------------------------------------
CImmLib::~CImmLib()
{ 
	if(m_hDllImm32)
	{
		FreeLibrary(m_hDllImm32);
	}
}

// -----------------------------------------------------------------------------
// dll 로딩후 Imm_ 함수포인터 바인딩
void	CImmLib::Init()
{
	FARPROC farProc;
    char szPath[MAX_PATH+1];
    
	if( GetSystemDirectoryA( szPath, MAX_PATH+1 ) ) 
	{
		strcat( szPath, IMM32_DLLNAME );
		m_hDllImm32 = LoadLibraryA( szPath );

		if( m_hDllImm32 )
		{
			GETPROCADDRESS( m_hDllImm32, ImmLockIMC, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmUnlockIMC, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmLockIMCC, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmUnlockIMCC, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmGetProperty, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmGetContext, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmReleaseContext, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmGetOpenStatus, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmSetOpenStatus, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmGetIMEFileNameA, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmGetCompositionStringW, farProc );
			GETPROCADDRESS( m_hDllImm32, ImmGetConversionStatus, farProc);
			GETPROCADDRESS( m_hDllImm32, ImmGetCandidateListW, farProc);
		}
	}
}


void	CImmLib::SetupImeApi(HWND hWnd)
{
    char szImeFile[MAX_PATH + 1];

	if( _ImmGetIMEFileNameA( m_hkl, szImeFile, sizeof(szImeFile)/sizeof(szImeFile[0]) - 1 ) != 0 ) {

		if( m_hDllIme ) FreeLibrary( m_hDllIme );
	    
		m_hDllIme = LoadLibraryA( szImeFile );

		if ( m_hDllIme ) 
		{
			_GetReadingString = (UINT (WINAPI*)(HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT))
				( GetProcAddress( m_hDllIme, "GetReadingString" ) );
			_ShowReadingWindow =(BOOL (WINAPI*)(HIMC, BOOL))
				( GetProcAddress( m_hDllIme, "ShowReadingWindow" ) );

			if( _ShowReadingWindow ) 
			{
				HIMC hImc = _ImmGetContext(hWnd);
				if(hImc) 
				{
					_ShowReadingWindow( hImc, false );
					_ImmReleaseContext(hWnd, hImc);
				}
			}
		}
	}

}


// -----------------------------------------------------------------------------
// ImmLib.cpp - End of file
// -----------------------------------------------------------------------------
