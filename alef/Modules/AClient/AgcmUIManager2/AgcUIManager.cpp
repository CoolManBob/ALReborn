// AgcUIManager.cpp: implementation of the AgcUIManager class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcUIManager.h"
#include "AgcmUIManager2.h"

//@{ kday 20050130
#include "AgcuCamMode.h"
//@} kday

#include "AgcmCharacter.h"
#include "AgcmTargeting.h"
#include "AgcmUIHotkey.h"

static AgcmTargeting *g_pAgcmTargeting = NULL;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcUIManager::AgcUIManager( AgcmUIManager2* pcsAgcmUIManager2 )
{
	m_pcsAgcmUIManager2 = pcsAgcmUIManager2;
	m_Property.bTopmost = TRUE;

	m_pAgcmCamera2		= NULL;
	m_bLoginMode		= TRUE;
	m_bRButtonClicked	= FALSE;

	m_eKeyStatus = AGCDUI_HOTKEY_TYPE_NORMAL;
	memset( &m_PrevMouseStatus, 0, sizeof( m_PrevMouseStatus ) );

	m_bCameraMoveMode	= FALSE;
	m_ulPrevClockCount	= 0;
	m_lZoomStatus		= 0;
	m_lRotateStatus		= 0;
	m_lPanStatus		= 0;
	m_bUseModalMessage	= TRUE;

	m_pAgcmUIHotkey		= NULL;
	m_pHkMinus			= NULL;
	m_pHkPlus			= NULL;
	m_pHkLeft			= NULL;
	m_pHkRight			= NULL;
	m_pHkUp				= NULL;
	m_pHkDown			= NULL;	
	m_pHkF10			= NULL;
	m_pHkF11			= NULL;
	m_pHkF12			= NULL;
}

AgcUIManager::~AgcUIManager( void )
{	
}	

BOOL AgcUIManager::OnKeyDown( RsKeyStatus *ks )
{
	if( !m_bLoginMode )
	{
		// 우선순위는 Shift/Ctrl/Alt
		if( g_pEngine->IsShiftDown() )
		{
			m_eKeyStatus = AGCDUI_HOTKEY_TYPE_SHIFT;
		}
		else if( g_pEngine->IsCtrlDown() )
		{
			m_eKeyStatus = AGCDUI_HOTKEY_TYPE_CONTROL;
		}
		else if( g_pEngine->IsAltDown() )
		{
			m_eKeyStatus = AGCDUI_HOTKEY_TYPE_ALT;
		}
		else
		{
			m_eKeyStatus = AGCDUI_HOTKEY_TYPE_NORMAL;
		}

		AgcdUIHotkey* pstHotkey = m_pcsAgcmUIManager2->GetHotkey( m_eKeyStatus, ks->keyScanCode );
		if( !pstHotkey ) return FALSE;

		return m_pcsAgcmUIManager2->ProcessAction( &pstHotkey->m_listAction );
	}

	return FALSE;
}

BOOL AgcUIManager::OnKeyUp( RsKeyStatus *ks	)
{
	return FALSE;
}

BOOL AgcUIManager::OnLButtonDown( RsMouseStatus *ms	)
{
	// popup UI가 있으면 닫자.
	if( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup )
	{
		m_pcsAgcmUIManager2->CloseUI( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup );
	}

	AgcdUIHotkey* pstHotkey = m_pcsAgcmUIManager2->GetHotkey( m_eKeyStatus, AGCDUI_HOTKEY_MOUSE_LCLICK );
	if( !pstHotkey ) return FALSE;

	return m_pcsAgcmUIManager2->ProcessAction( &pstHotkey->m_listAction );
}

BOOL AgcUIManager::OnRButtonDown( RsMouseStatus *ms	)
{
	// popup UI가 있으면 닫자.
	if( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup )
	{
		m_pcsAgcmUIManager2->CloseUI( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup );
	}

	m_bRButtonClicked = TRUE;
	m_ulRClickStart = m_pcsAgcmUIManager2->GetClockCount();
	m_stRClickStartStatus = *ms;
	m_bCameraMoveMode =	FALSE;

	::GetCursorPos( &m_stCursorPos );
	return FALSE;
}

BOOL AgcUIManager::OnMButtonDown( RsMouseStatus *ms )
{
	// popup UI가 있으면 닫자.
	if( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup )
	{
		m_pcsAgcmUIManager2->CloseUI( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup );
	}

	AgcdUIHotkey* pstHotkey = m_pcsAgcmUIManager2->GetHotkey( m_eKeyStatus, AGCDUI_HOTKEY_MOUSE_MCLICK );
	if( pstHotkey )
	{
		return m_pcsAgcmUIManager2->ProcessAction( &pstHotkey->m_listAction );
	}

	if( m_pAgcmCamera2 && !m_bLoginMode )
	{
		m_pAgcmCamera2->bTurnWY_180();
	}

	return TRUE;
}

BOOL AgcUIManager::OnLButtonUp( RsMouseStatus *ms )
{
	return FALSE;
}

BOOL AgcUIManager::OnRButtonUp( RsMouseStatus *ms )
{
	AgcdUIHotkey* pstHotkey = m_pcsAgcmUIManager2->GetHotkey( m_eKeyStatus, AGCDUI_HOTKEY_MOUSE_RCLICK );
	if( !pstHotkey ) return FALSE;
	return m_pcsAgcmUIManager2->ProcessAction( &pstHotkey->m_listAction );
}

BOOL AgcUIManager::OnMButtonUp( RsMouseStatus *ms )
{
	return FALSE;
}

BOOL AgcUIManager::PreTranslateInputMessage( RsEvent event, PVOID param )
{
	RsMouseStatus* pstMouseStatus = ( RsMouseStatus* )param;

	switch( event )
	{
	case rsRIGHTBUTTONDOWN:										break;
	case rsRIGHTBUTTONUP:
		{
			m_bRButtonClicked = FALSE;
			if( m_bCameraMoveMode )
			{
				m_bCameraMoveMode = FALSE;
				m_pcsAgcmUIManager2->ShowCursor( TRUE );
				ReleaseModal();
				return TRUE;
			}
		}
		break;

	case rsMOUSEMOVE:
		{
			m_pcsAgcmUIManager2->SetMouseOnUI( NULL );
			if( m_bRButtonClicked && !m_bCameraMoveMode )
			{
				SetModal();
				m_pcsAgcmUIManager2->ShowCursor( FALSE );
				m_bCameraMoveMode = TRUE;
				
				if( !g_pAgcmTargeting )
				{
					g_pAgcmTargeting = ( AgcmTargeting* )( m_pcsAgcmUIManager2->GetModule( "AgcmTargeting" ) );
				}

				if( m_pAgcmCharacter && ( g_pAgcmTargeting->m_iTargetMode == AGCMTARGETING_TARGET_MODE_NONE || 
						g_pAgcmTargeting->m_iTargetMode == AGCMTARGETING_TARGET_MODE_GROUNDCLICK ||
						g_pAgcmTargeting->m_iTargetMode == AGCMTARGETING_TARGET_MODE_AREATARGETING ) )
				{
					m_pAgcmCharacter->SetPlayerLookAtFactor( 1.0 );
				}
			}
		}
		break;
	}

	return FALSE;
}

BOOL AgcUIManager::OnMouseWheel( INT32 lDelta )
{
	// Camera Control여부 
	if( m_bLoginMode ) return TRUE;

	//{@ kday 20041227
	if( m_pAgcmCamera2 )
	{
		if( g_pEngine->GetDebugFlag() & AgcEngine::CAMERA_ANGLE_CHANGE && g_pEngine->IsLCtrlDown() )
		{
			FLOAT fProjection = g_pEngine->GetProjection();

			// 5%씩..변화..
			if( lDelta > 0 )
			{
				fProjection	= fProjection + fProjection * 0.05f;
			}
			else
			{ 
				fProjection	= fProjection - fProjection * 0.05f;
			}

			g_pEngine->SetProjection( fProjection );

#ifdef _DEBUG
			char str[ 256 ];
			sprintf( str , "Projection %.4f" , fProjection );
			g_pEngine->LuaErrorMessage( str );
#endif
		}
		else
		{
			m_pAgcmCamera2->bZoom( ( float )lDelta );
		}
	}

	return TRUE;
}

BOOL AgcUIManager::OnMouseMove( RsMouseStatus *ms )
{
	// Camera Control 여부 
	if( m_bLoginMode ) return FALSE;

	if( ( m_bCameraMoveMode ) && m_pAgcmCamera2 )
	{
		POINT stCursorPos;
		::GetCursorPos( &stCursorPos );

		if( stCursorPos.x != m_stCursorPos.x || stCursorPos.y != m_stCursorPos.y )
		{
			if( ms->pos.x != m_stRClickStartStatus.pos.x )
			{
				m_pAgcmCamera2->bRotWY( ( float )( m_stCursorPos.x - stCursorPos.x ) );
			}

			if( ms->pos.y != m_stRClickStartStatus.pos.y )
			{
				m_pAgcmCamera2->bRotCX( ( float )( stCursorPos.y - m_stCursorPos.y ) );
			}

			::SetCursorPos( m_stCursorPos.x, m_stCursorPos.y );
		}
	}

	m_PrevMouseStatus = *ms;
	return FALSE;
}

void AgcUIManager::SetAgcmCameraModule( AgcmCamera2* pAgcmCamera2 )
{
	m_pAgcmCamera2 = pAgcmCamera2;
}

void AgcUIManager::SetAgcmCharacterModule( AgcmCharacter *pAgcmCharacter )
{
	m_pAgcmCharacter = pAgcmCharacter;
}

BOOL AgcUIManager::OnRButtonDblClk( RsMouseStatus *ms )
{
	// popup UI가 있으면 닫자.
	if( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup )
	{
		m_pcsAgcmUIManager2->CloseUI( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup );
	}
	
	return AgcWindow::OnRButtonDblClk( ms );
}

BOOL AgcUIManager::OnLButtonDblClk( RsMouseStatus *ms )
{
	// popup UI가 있으면 닫자.
	if( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup )
	{
		m_pcsAgcmUIManager2->CloseUI( m_pcsAgcmUIManager2->m_pcsUIOpenedPopup );
	}

	return AgcWindow::OnLButtonDblClk( ms );
}

BOOL AgcUIManager::OnIdle( UINT32 ulClockCount )
{
	PROFILE( "AgcUIManager::OnIdle" );
	if( !m_pcsAgcmUIManager2->m_bCameraControl ) return TRUE;

	INT32 lDeltaClock = ulClockCount - m_ulPrevClockCount;

	if( m_pAgcmCamera2 )
	{		
		UpdateCameraViewProcess( lDeltaClock );
	}

	//@{ Jaewon 20051213
	// Fade the look-at motion out if the current target mode is NONE or GROUNDCLICK.
	if( !g_pAgcmTargeting )
	{
		g_pAgcmTargeting = ( AgcmTargeting* )( m_pcsAgcmUIManager2->GetModule( "AgcmTargeting" ) );
	}

	if( m_pAgcmCharacter && m_pAgcmCharacter->GetPlayerLookAtFactor() > 0.0f && m_bRButtonClicked == FALSE &&
		(	g_pAgcmTargeting->m_iTargetMode == AGCMTARGETING_TARGET_MODE_NONE || 
			g_pAgcmTargeting->m_iTargetMode == AGCMTARGETING_TARGET_MODE_GROUNDCLICK || 
			g_pAgcmTargeting->m_iTargetMode == AGCMTARGETING_TARGET_MODE_AREATARGETING ) )
	{
		m_pAgcmCharacter->SetPlayerLookAtFactor( m_pAgcmCharacter->GetPlayerLookAtFactor() - ( RwReal )lDeltaClock / 2000.0f );
	}
	//@} Jaewon

	m_ulPrevClockCount = ulClockCount;
	return TRUE;
}

BOOL AgcUIManager::OnChar( CHAR* pChar, UINT lParam )
{
	if( !m_bLoginMode )
	{
		RsKeyCodes eKeyCode = rsNULL;
		switch( *pChar )
		{
		case VK_RETURN :		eKeyCode = rsENTER;			break;
		}

		if( eKeyCode )
		{
			AgcdUIHotkey* pstHotkey = m_pcsAgcmUIManager2->GetHotkey( m_eKeyStatus, eKeyCode );
			if( !pstHotkey ) return FALSE;

			return m_pcsAgcmUIManager2->ProcessAction( &pstHotkey->m_listAction );
		}
	}

	return FALSE; // m_bLoginMode가 True인 경우 Return값이 가비지 값으로 추정되어 추가했음. 2004.10.20 =Ashulam=
}

//@{ 2006/07/27 burumal
BOOL AgcUIManager::SetCameraViewProcessHotkeys( void )
{
	m_pHkMinus	= m_pAgcmUIHotkey->FindCurrentHotkeyByDefault( AGCDUI_HOTKEY_TYPE_NORMAL, 74 );		// 단축키 -		(g_aszHotkeyNameTable참고)	
	m_pHkPlus	= m_pAgcmUIHotkey->FindCurrentHotkeyByDefault( AGCDUI_HOTKEY_TYPE_NORMAL, 78 );		// 단축키 +		(g_aszHotkeyNameTable참고)
	m_pHkLeft	= m_pAgcmUIHotkey->FindCurrentHotkeyByDefault( AGCDUI_HOTKEY_TYPE_NORMAL, 203 );	// 단축키 Left	(g_aszHotkeyNameTable참고)
	m_pHkRight	= m_pAgcmUIHotkey->FindCurrentHotkeyByDefault( AGCDUI_HOTKEY_TYPE_NORMAL, 205 );	// 단축키 Right	(g_aszHotkeyNameTable참고)
	m_pHkUp		= m_pAgcmUIHotkey->FindCurrentHotkeyByDefault( AGCDUI_HOTKEY_TYPE_NORMAL, 200 );	// 단축키 Up	(g_aszHotkeyNameTable참고)
	m_pHkDown	= m_pAgcmUIHotkey->FindCurrentHotkeyByDefault( AGCDUI_HOTKEY_TYPE_NORMAL, 208 );	// 단축키 Down	(g_aszHotkeyNameTable참고)	
	m_pHkF10	= m_pAgcmUIHotkey->FindCurrentHotkeyByDefault( AGCDUI_HOTKEY_TYPE_NORMAL, 68 );		// 단축키 F10	(g_aszHotkeyNameTable참고)
	m_pHkF11	= m_pAgcmUIHotkey->FindCurrentHotkeyByDefault( AGCDUI_HOTKEY_TYPE_NORMAL, 87 );		// 단축키 F11	(g_aszHotkeyNameTable참고)
	m_pHkF12	= m_pAgcmUIHotkey->FindCurrentHotkeyByDefault( AGCDUI_HOTKEY_TYPE_NORMAL, 88 );		// 단축키 F12	(g_aszHotkeyNameTable참고)

	if( m_pHkMinus && m_pHkPlus && m_pHkLeft && m_pHkRight && m_pHkUp && m_pHkDown && m_pHkF10 && m_pHkF11 && m_pHkF12 ) return TRUE;
	return FALSE;
}

void AgcUIManager::UpdateCameraViewProcess( INT32 lDeltaClock )
{		
	if( !m_pAgcmUIHotkey )
	{
		m_pAgcmUIHotkey = ( AgcmUIHotkey* )g_pEngine->GetModule( "AgcmUIHotkey" );
		ASSERT( m_pAgcmUIHotkey );
	}

	if( !m_pAgcmUIHotkey ) return;
	static BOOL bIsFirst = TRUE;

	if( bIsFirst )
	{		
		if( SetCameraViewProcessHotkeys() )
		{
			bIsFirst = FALSE;
		}
	}

	if( m_bLoginMode ) return;

	// 액티브 모드 인지 점검..
	if( g_pEngine->GetActivate() != AgcEngine::AGCENGINE_NORMAL ) return;
	if( g_pEngine->GetCharCheckState() ) return;
	
	// zoom...
	m_lZoomStatus = 0;	
	if( m_pAgcmUIHotkey->GetAsyncHotkeyState( m_pHkMinus ) )
	{
		m_lZoomStatus =	-1;
	}

	if( m_pAgcmUIHotkey->GetAsyncHotkeyState( m_pHkPlus ) )
	{
		m_lZoomStatus = 1;
	}

	if ( m_lZoomStatus != 0 )
	{
		m_pAgcmCamera2->bZoom( ( RwReal )( lDeltaClock * m_lZoomStatus ) );
	}


	// rotate...(yaw)
	m_lRotateStatus = 0;
	if( m_pAgcmUIHotkey->GetAsyncHotkeyState( m_pHkLeft ) )
	{
		m_lRotateStatus	= 1;
	}

	if( m_pAgcmUIHotkey->GetAsyncHotkeyState( m_pHkRight ) )
	{
		m_lRotateStatus = -1;
	}

	if( m_lRotateStatus != 0 )
	{
		m_pAgcmCamera2->bRotWY( ( RwReal )( lDeltaClock * m_lRotateStatus ) );
	}
		
	// rotate...(pitch)
	m_lPanStatus = 0;
	if( m_pAgcmUIHotkey->GetAsyncHotkeyState( m_pHkUp ) )
	{
		m_lPanStatus = 1;
	}
	
	if( m_pAgcmUIHotkey->GetAsyncHotkeyState( m_pHkDown ) )
	{
		m_lPanStatus = -1;
	}

	if( m_lPanStatus != 0 )
	{
		m_pAgcmCamera2->bRotCX( ( RwReal )( lDeltaClock * m_lPanStatus ) );
	}

	// F10 (camera top-view mode)
	if( m_pAgcmCamera2 && m_pAgcmUIHotkey->GetAsyncHotkeyState( m_pHkF10 ) )
	{
		AgcuCamMode::bGetInst().bChageMode( *m_pAgcmCamera2, AgcuCamMode::eMode_Top );
	}

	// F11 (camera quater-view mode)
	if( m_pAgcmCamera2 && m_pAgcmUIHotkey->GetAsyncHotkeyState( m_pHkF11 ) )
	{
		AgcuCamMode::bGetInst().bChageMode( *m_pAgcmCamera2, AgcuCamMode::eMode_Quart );
	}

	// F12 (camera free-view mode)
	if( m_pAgcmCamera2 && m_pAgcmUIHotkey->GetAsyncHotkeyState( m_pHkF12 ) )
	{
		AgcuCamMode::bGetInst().bChageMode( *m_pAgcmCamera2, AgcuCamMode::eMode_Free );
	}
}
//@}