#ifndef __CLASS_GAME_FRAMEWORK_H__
#define __CLASS_GAME_FRAMEWORK_H__




#include "ContainerUtil.h"
#include "CRenderWindow.h"
#include "CRenderDevice.h"



class CGameFramework
{
public :
	stRenderDeivceSettings						m_DeviceSettings;

	CRenderWindow								m_RenderWindow;
	CRenderDevice*								m_pRenderDevice;

	HINSTANCE									m_hInstance;
	WNDPROC										m_fnWndProc;

public :
	CGameFramework( void );
	virtual ~CGameFramework( void );

public :
	virtual BOOL			OnGameStart			( HINSTANCE hInstacne, WNDPROC fnWndProc = NULL );
	virtual BOOL			OnGameLoop			( void );
	virtual BOOL			OnGameEnd			( void );

public :
	virtual BOOL			ParseCommandLine	( void );
	virtual BOOL			CheckPatchCode		( char* pPatchCode );

	virtual BOOL			CreateRenderWindow	( char* pSettingFileName );
	virtual BOOL			CreateRenderDevice	( char* pRendererName );

	virtual BOOL			DestroyRenderWindow	( void );
	virtual BOOL			DestroyRenderDevice	( void );

	virtual BOOL			LoadDeviceSettings	( char* pFileName );
	virtual BOOL			LoadGameResource	( void );

public :
	CRenderWindow*			GetRenderWindow		( void ) { return &m_RenderWindow; }
	CRenderDevice*			GetRenderDevice		( void ) { return m_pRenderDevice; }

public :
	static CGameFramework*	GetGameFramework	( char* pNationName = NULL );
};




#endif