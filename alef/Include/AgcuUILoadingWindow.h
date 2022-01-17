#ifndef __AGCU_UI_LOADING_WINDOW_H__
#define __AGCU_UI_LOADING_WINDOW_H__



#include "ApDefine.h"



class AgcuUILoadingWindow
{
private :
	INT32									m_nMax;
	INT32									m_nCurrent;

	BOOL									m_bIsLoadingWindowOpen;
	BOOL									m_bIsPostLoadingWindow;

	void*									m_pUserDataLoadingMax;
	void*									m_pUserDataLoadingCurrent;

	float									m_fSoundVolume;

public :
	AgcuUILoadingWindow( void );
	virtual ~AgcuUILoadingWindow( void )	{	}

public :
	BOOL			OnInitialize			( void* pUILogin );
	BOOL			OnAddUserData			( void* pUIManager );
	BOOL			OnRefresh				( void* pUIManager, INT32 nCurrent, INT32 nMax = -1 );

	BOOL			OnOpen					( void* pUIManager, INT32 nStart = 0 );
	BOOL			OnUpdateAdd				( void* pUIManager, INT32 nAddCount = 1 );
	BOOL			OnClose					( void* pUIManager );

public :
	void			SetIsPostLoadingWindow	( BOOL bIsPost ) { m_bIsPostLoadingWindow = bIsPost; }
	BOOL			GetIsPostLoadingWindow	( void ) { return m_bIsPostLoadingWindow; }

	void			SetIsLoadingWindowOpen	( BOOL bIsOpen ) { m_bIsLoadingWindowOpen = bIsOpen; }
	BOOL			GetIsLoadingWindowOpen	( void ) { return m_bIsLoadingWindowOpen; }

	INT32			GetMaxLoadingCount		( void ) { return m_nMax; }
	INT32			GetCurrentLoadingCount	( void ) { return m_nCurrent; }
};



#endif