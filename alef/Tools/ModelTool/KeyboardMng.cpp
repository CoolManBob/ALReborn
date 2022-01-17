#include "stdafx.h"
#include "KeyBoardMng.h"

CKeyboardMng::CKeyboardMng(void)
{
	memset( m_bSystem, 0, sizeof(BYTE)*eKeyStateMax );
	memset( m_eState, eKeyboardNone, sizeof(eKeyboardState)*KEYBOARD_STATE_MAX );
}

CKeyboardMng::~CKeyboardMng(void)
{
	Clear();
}

void	CKeyboardMng::UpdateKeyboard()
{
	PreUpdate();

	for( KeyStateListItr Itr = begin(); Itr != end(); ++Itr )
		Execute( (*Itr) );

	PostUpdate();
}

void	CKeyboardMng::PreUpdate()
{
	for( BYTE by = 0; by<KEYBOARD_STATE_MAX; ++by )
	{
		SHORT sKeyState = GetAsyncKeyState( by );

		BOOL bPressing	= sKeyState & 0x0001 ? TRUE : FALSE;
		BOOL bPress		= sKeyState & 0x8000 ? TRUE : FALSE;

		if( bPressing || bPress )
			m_eState[by] = eKeyboardPress;
		//else if( bPress	)
		//	m_eState[by] = eKeyboardDown;
		else if( eKeyboardPress == m_eState[by] /*|| eKeyboardDown == m_eState[by]*/ )
			m_eState[by] = eKeyboardUp;
		else
			m_eState[by] = eKeyboardNone;
	}
	
	if( IsShift() )		m_bSystem[eKeyStateShift]	= TRUE;	
	if( IsCtrl() )		m_bSystem[eKeyStateCtrl]	= TRUE;
	if( IsAlt() )		m_bSystem[eKeyStateAlt]		= TRUE;
}

void	CKeyboardMng::PostUpdate()
{
	memset( m_bSystem, 0, sizeof(BYTE)*eKeyStateMax );
	memset( m_eState, eKeyboardNone, sizeof(eKeyboardState)*KEYBOARD_STATE_MAX );
}

void	CKeyboardMng::Execute( LPKeyStateNode pKeyStateNode )
{
	if( !pKeyStateNode )	return;
	if( pKeyStateNode->m_eState != m_eState[ pKeyStateNode->m_byKey ] )	return;
	for( int i=0; i<eKeyStateMax; ++i )
		if( pKeyStateNode->m_bSystem[i] != m_bSystem[i] )				return;

	pKeyStateNode->m_funcKeyboard( pKeyStateNode->m_pThis );
}

BOOL	CKeyboardMng::IsShift()
{
	if( m_eState[VK_SHIFT] || m_eState[VK_LSHIFT] || m_eState[VK_RSHIFT] )
		return TRUE;

	return FALSE;
}

BOOL	CKeyboardMng::IsCtrl()
{
	if( m_eState[VK_CONTROL] || m_eState[VK_LCONTROL] || m_eState[VK_RCONTROL] )
		return TRUE;

	return FALSE;
}

BOOL	CKeyboardMng::IsAlt()
{
	if( m_eState[VK_MENU] || m_eState[VK_LMENU] || m_eState[VK_RMENU] )
		return TRUE;

	return FALSE;
}