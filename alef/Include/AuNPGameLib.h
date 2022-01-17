#ifndef _AUNPGAMELIB_H
#define _AUNPGAMELIB_H

#ifdef _AREA_JAPAN_

#include <windows.h>

#if defined(_AREA_JAPAN_) && !defined(_DEBUG)
//#define NO_GAMEGUARD
#include "NPGameLib.h"
#endif

struct _GG_AUTH_DATA;

class AuNPWrapper
{
public:
	AuNPWrapper();

	DWORD Init();
	void  SetWndHandle(HWND hWnd);
	void  Auth2(_GG_AUTH_DATA* pggAuthData);
	DWORD Check();
	DWORD Send(LPCTSTR lpszUserId);
	LPCSTR GetInfo();

private:
#if defined(_AREA_JAPAN_) && !defined(_DEBUG)
	CNPGameLib m_npgame;
#endif
};

extern AuNPWrapper npgl;

#endif //_AREA_JAPAN_


#if defined(_AREA_GLOBAL) || defined(_AREA_KOREA_)
//////////////////////////////////////////////////////////////////////////
//
// nProtect GameGuard Lib
//     - GameGuard client module
//
#include <ApDefine.h>
#include "nProtect/NPGameLib.h"

class AuNPBase
{
public:
	AuNPBase()
	{
	};
	
	virtual ~AuNPBase()
	{
	};

	virtual BOOL Init()
	{
		return TRUE;
	};

	virtual void SetWndHandle( HWND hWnd )
	{

	};

	virtual void Auth( PVOID pggAuthData )
	{

	};

	virtual BOOL Check()
	{
		return TRUE;
	};

	virtual DWORD Send( LPCTSTR lpszUserId )
	{
		return 0;
	};

	virtual LPCSTR GetInfo()
	{
		return NULL;
	};

	virtual CHAR* GetMessage( DWORD dwResult )
	{
		return NULL;
	};

	virtual BOOL NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
	{
		return TRUE;
	};
};

class CNPGameLib;
class AuNPGameLib : public AuNPBase
{
	CNPGameLib* m_pNPGameLib;
	HWND m_hWnd;

public:
	AuNPGameLib();
	virtual ~AuNPGameLib();

	BOOL Init();
	void SetWndHandle( HWND hWnd );
	CHAR* GetMessage(DWORD dwResult);
	BOOL NPGameMonCallback(DWORD dwMsg, DWORD dwArg);
	BOOL Check();
	void Auth( PVOID pggAuthData );
	DWORD Send( LPCTSTR lpszUserId );
};

//////////////////////////////////////////////////////////////////////////
//
class AuNPWrapper
{
	AuNPBase* m_pNPGameLib;

public:
	AuNPWrapper();
	virtual ~AuNPWrapper();

	BOOL Init();
	void SetWndHandle( HWND hWnd );
	BOOL NPGameMonCallback(DWORD dwMsg, DWORD dwArg);
	BOOL Check();
	DWORD Send(LPCTSTR lpszUserId);
	void  Auth(PVOID pggAuthData);
	CHAR* GetMessage(DWORD dwResult);
};

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg);
BOOL CALLBACK NPGameMonSend(DWORD dwMsg);

//extern AuNPWrapper npgl;

#endif //defined(_AREA_GLOBAL) || defined(_AREA_KOREA_)

#endif //_AUNPGAMELIB_H