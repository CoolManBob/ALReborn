#ifndef _AUGAMEGUARD_H
#define _AUGAMEGUARD_H

#include "ApDefine.h"

//////////////////////////////////////////////////////////////////////////
//
// nProtect GameGuard CS Authenticaltion
//     - GameGuard server module
//

/*
#ifdef _WIN64
#pragma comment(lib, "ggsrvlib25" )
#else
#pragma comment(lib, "ggsrvlib25_Win32_MT" )
#endif

#define _USE_NPROTECT_GAMEGUARD_
*/

/*#if defined(_WIN64) || defined(WIN64)

	#include "nProtect/ggsrv25.h"
	//#define _USE_NPROTECT_GAMEGUARD_//JK_게임가드주석처리
	#pragma comment(lib, "ggsrvlib25" )

	// callback
	GGAUTHS_API void NpLog(int mode, char* msg);
	GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report);

	class AuGGCSAuth
	{
	public:
		AuGGCSAuth();
		virtual ~AuGGCSAuth();

		BOOL Init();
	};

#endif*/

#endif //_AUGAMEGUARD_H