#ifndef __HanGameInfoForClient__
#define __HanGameInfoForClient__

#ifdef HANGAMEINFOFORCLIENT_EXPORTS
#define HANGAMEINFO_API __declspec(dllexport)
#else
#define HANGAMEINFO_API __declspec(dllimport)
#ifdef _DEBUG
#pragma comment(lib, "HanGameInfoForClientD.lib") 
#else
#pragma comment(lib, "HanGameInfoForClient.lib") 
#endif
#endif

#define SERVICE_NATION  0x000000FF
#define SERVICE_KOR		0x00000001
#define SERVICE_USA		0x00000002
#define SERVICE_JPN		0x00000003
#define SERVICE_CHN		0x00000004

#define SERVICE_TYPE    0x00000F00
#define SERVICE_ALPHA	0x00000100
#define SERVICE_REAL	0x00000200
#define SERVICE_BETA	0x00000300

#define SERVICE_SITE	0x0000F000
#define SERVICE_HANGAME 0x00001000
#define SERVICE_ASOBLOG 0x00002000

/*
Return value Info:
0 : Ok
- : fault from  function / this system 
-1 : general fault.
+ : fault from logging server
*/

#define HAN_GAMEINFO_OK									0
#define HAN_GAMEINFO_ARGUMENT_INVALID					-1
#define HAN_GAMEINFO_INITED_ALREADY						-2
#define HAN_GAMEINFO_INITED_NOT							-3
#define HAN_GAMEINFO_INITED_FAIL						-4
#define HAN_GAMEINFO_HTTP_INITFAIL						-5
#define HAN_GAMEINFO_HTTP_CONNECTFAIL					-6
#define HAN_GAMEINFO_HTTP_OPENFAIL						-7
#define HAN_GAMEINFO_HTTP_SENDFAIL						-8
#define HAN_GAMEINFO_GAMESTRING_SERVICECODEINVALID		-9
#define HAN_GAMEINFO_GAMESTRING_GAMEIDINVALID			-10
#define HAN_GAMEINFO_GAMESTRING_MEMBERIDINVALID			-11
#define HAN_GAMEINFO_DLL_UNEXPECTED						-100

// For games that have no gamestring
HANGAMEINFO_API int __stdcall HanGameInfoInit(/*IN*/ char* szGameId,											  
											  /*IN*/ char *szMemberId,
											  /*IN*/ int   nServiceCode);

// For games that have gamestring
HANGAMEINFO_API int __stdcall HanGameInfoInitGameString(/*IN*/ char* szGameString);



HANGAMEINFO_API int __stdcall HanGameInfoEnterLobby();

HANGAMEINFO_API int __stdcall HanGameInfoExitLobby();

HANGAMEINFO_API int __stdcall HanGameInfoStartGame();

HANGAMEINFO_API int __stdcall HanGameInfoEndGame();

#endif	// __HanGameInfoForClient__