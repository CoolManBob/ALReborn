// AcUIButton.cpp: implementation of the AcUIButton class.
//
//////////////////////////////////////////////////////////////////////
#include "rwcore.h"
#include "rpworld.h"

#include <skeleton.h>
#include <menu.h>
#include <camera.h>

#include "AgcModule.h"
#include "AgcEngine.h"
#include "AcUIButton.h"

char* AcUIButton::m_pszButtonClickSound = NULL;

extern AgcEngine *	g_pEngine;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcUIButton::AcUIButton( void )
{
	m_nType = TYPE_BUTTON;

	m_lButtonTextureID[ 0 ] 			= ACUIBUTTON_INIT_INDEX;
	m_lButtonTextureID[ 1 ] 			= ACUIBUTTON_INIT_INDEX;
	m_lButtonTextureID[ 2 ] 			= ACUIBUTTON_INIT_INDEX;
	m_lButtonTextureID[ 3 ] 			= ACUIBUTTON_INIT_INDEX;

	m_fTextureWidth						= 0.0f;
	m_fTextureHeight					= 0.0f;

	m_stProperty.m_bPushButton			= FALSE;
	m_stProperty.m_bStartOnClickStatus	= FALSE;
	m_stProperty.m_bClickOnButtonDown	= FALSE;

	m_pcsDragDropGridItem				= NULL;

	m_lButtonDownStringOffsetX			= 0;
	m_lButtonDownStringOffsetY			= 0;
	m_lDisableTextColor					= 0xff8f8f8f;

	SetButtonMode( ACUIBUTTON_MODE_NORMAL );
}

AcUIButton::~AcUIButton( void )
{
}

/*****************************************************************
*   Function : AddButtonImage
*   Comment  : Add Button Image
*   Date&Time : 2003-08-13, 오전 11:14
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIButton::AddButtonImage( char* filename, UINT8 cButtonMode )
{
	if( ACUIBUTTON_MODE_NORMAL <= cButtonMode && ACUIBUTTON_MODE_DISABLE >= cButtonMode )
	{
		m_lButtonTextureID[ cButtonMode ] = m_csTextureList.AddImage( filename );
		OnAddImage( m_csTextureList.GetImage_ID( m_lButtonTextureID[ cButtonMode ] ) );
	}

	SetButtonMode( m_cButtonMode );
}

void AcUIButton::SetButtonImage( INT32 lImageID, INT8 cButtonMode )
{
	if( ACUIBUTTON_MODE_NORMAL <= cButtonMode && ACUIBUTTON_MODE_DISABLE >= cButtonMode )
	{
		m_lButtonTextureID[ cButtonMode ] = lImageID;
		OnAddImage( m_csTextureList.GetImage_ID( m_lButtonTextureID[ cButtonMode ] ) );
	}

	SetButtonMode( m_cButtonMode );
}

INT32 AcUIButton::GetButtonImage( INT8 cButtonMode )
{
	if( ACUIBUTTON_MODE_NORMAL <= cButtonMode && ACUIBUTTON_MODE_DISABLE >= cButtonMode )
	{
		return m_lButtonTextureID[ cButtonMode ];
	}

	return -1;
}

/*****************************************************************
*   Function : OnAddImage
*   Comment  : virtual OnAddImage function Overriding
*   Date&Time : 2002-11-11, 오후 5:40
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIButton::OnAddImage( RwTexture* pTexture )
{
	// Texture 크기 Set
	if( pTexture != NULL && pTexture->raster != NULL )
	{
		m_fTextureWidth		= ( float )RwRasterGetWidth( pTexture->raster );
		m_fTextureHeight	= ( float )RwRasterGetHeight( pTexture->raster );
	}

	return;
}

/*****************************************************************
*   Function : OnPostRender
*   Comment  : virtual OnPostRender function Overriding
*   Date&Time : 2002-11-11, 오후 6:03
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIButton::OnPostRender( RwRaster *raster	)
{
}

/*****************************************************************
*   Function : OnWindowRender
*   Comment  : OnWindowRender
*   Date&Time : 2003-06-24, 오후 2:05
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIButton::OnWindowRender( void )
{
	PROFILE("AcUIButton::OnWindowRender");	
	DWORD lColor = m_lColor;

	// 버튼 비활성 상태인 경우 컬러값 조정
	if( GetButtonMode() == ACUIBUTTON_MODE_DISABLE )
	{
		m_lColor = m_lDisableTextColor;
	}

	AcUIBase::OnWindowRender();
	m_lColor = lColor;
}

/*****************************************************************
*   Function : OnLButtonDown
*   Comment  : virtual OnLButtonDown function Overriding
*   Date&Time : 2002-11-11, 오후 6:44
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL AcUIButton::OnLButtonDown( RsMouseStatus *ms )
{
	// 먼저 AcUIBase에서 처리.. 이후에 To Do..
	if( AcUIBase::OnLButtonDown( ms ) ) return TRUE;

	// 드래그중이면 넘어가고..
	if( g_pEngine->m_bDraging ) return FALSE;

	//비활성일 경우 여기까지..
	if( m_cButtonMode == ACUIBUTTON_MODE_DISABLE ) return TRUE;	

	if( m_stProperty.m_bPushButton )
	{
		if( m_cButtonMode == ACUIBUTTON_MODE_NORMAL || m_cButtonMode == ACUIBUTTON_MODE_ONMOUSE )
		{
			SetButtonMode( ACUIBUTTON_MODE_CLICK, TRUE );
		}
		else if ( m_cButtonMode == ACUIBUTTON_MODE_CLICK )
		{
			SetButtonMode( ACUIBUTTON_MODE_NORMAL, TRUE );
		}
	}
	else
	{
		SetButtonMode( ACUIBUTTON_MODE_CLICK, m_stProperty.m_bClickOnButtonDown ); //m_cButtonMode = 2;

		if( pParent )
		{
			pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BUTTON_MESSAGE_PUSHDOWN, ( PVOID )&m_lControlID );
		}
	}
	
	return TRUE;
}

/*****************************************************************
*   Function : 
*   Comment  : 
*   Date&Time : 2002-11-11, 오후 7:15
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL AcUIButton::OnLButtonUp( RsMouseStatus *ms )
{
	// 먼저 AcUIBase에서 처리.. 이후에 To Do..
	if( AcUIBase::OnLButtonUp( ms ) ) return FALSE;

	// 드래그중이면 넘어가고..
	if( g_pEngine->m_bDraging )	return FALSE;

	// 비활성일 경우 여기까지..
	if( m_cButtonMode == ACUIBUTTON_MODE_DISABLE ) return FALSE;	
	
	if (!m_stProperty.m_bPushButton)
	{
		// 버튼 다운시에서 Up에서 버튼 처리 해줄꺼다
		if( m_cButtonMode != ACUIBUTTON_MODE_CLICK ) return FALSE;

		SetButtonMode( ACUIBUTTON_MODE_ONMOUSE ) ; //m_cButtonMode = 1;

		if( pParent )
		{
			pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BUTTON_MESSAGE_PUSHUP, ( PVOID )&m_lControlID );
		}

		// 만약 ButtonDown일때 메세지 전달 하는게 아니라면
		if( !m_stProperty.m_bClickOnButtonDown )
		{
			// 이곳에 버튼이 눌렸을때 일어나는 메세지 전달
			if( pParent )
			{
				pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BUTTON_MESSAGE_CLICK, ( PVOID )&m_lControlID );
			}
		}
		
		// 메세지 전달과 함께 Button Click Sound
		if( m_pszButtonClickSound )
		{
			PlayUISound( m_pszButtonClickSound );
		}
	}
	
	return TRUE;
}

BOOL AcUIButton::OnDragDrop( PVOID pParam1, PVOID pParam2 )
{
	AcUIGridItem* pcsGridItem = ( AcUIGridItem* )pParam2;
	if( !pcsGridItem ||	!pcsGridItem->m_ppdGridItem ) return TRUE;

	m_pcsDragDropGridItem = pcsGridItem;

	if( pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BUTTON_MESSAGE_DRAG_DROP, ( PVOID )&m_lControlID );
	}

	return TRUE;
}

AcUIGridItem* AcUIButton::GetDragDropGridItem( void )
{
	return m_pcsDragDropGridItem;
}

/*****************************************************************
*   Function : SetButtonEnable
*   Comment  : 버튼의 Enable여부를 제어한다. 
*   Date&Time : 2002-11-11, 오후 8:51
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIButton::SetButtonEnable( BOOL bEnable )
{	
	if( !bEnable )
	{
		SetButtonMode( ACUIBUTTON_MODE_DISABLE );	//m_cButtonMode = 3;
	}
	else
	{
		SetButtonMode( ACUIBUTTON_MODE_NORMAL ); //m_cButtonMode = 0;
	}
}

void AcUIButton::OnSetFocus( void )
{
	// AcUIBase 선행처리
	AcUIBase::OnSetFocus();

	// To Do ..
	if( g_pEngine->m_bDraging ) return;

	//비활성일 경우 먼저 처리 
	if( m_cButtonMode == ACUIBUTTON_MODE_DISABLE ) return;	

	/* 이곳에 GetFocus 메세지 전달*/
	if( !m_stProperty.m_bPushButton || GetButtonMode() != ACUIBUTTON_MODE_CLICK )
	{
		SetButtonMode( ACUIBUTTON_MODE_ONMOUSE );	//m_cButtonMode = 1;
	}

	return;
}

void AcUIButton::OnKillFocus( void )
{
	// AcUIBase 선행처리
	AcUIBase::OnKillFocus();

	// To Do ..
	if( g_pEngine->m_bDraging ) return;

	// 비활성일 경우 먼저 처리 
	if( m_cButtonMode == ACUIBUTTON_MODE_DISABLE ) return;	

	/* 이곳에 LostFocus 메세지 전달*/
	if( !m_stProperty.m_bPushButton || GetButtonMode() != ACUIBUTTON_MODE_CLICK )
	{
		SetButtonMode( ACUIBUTTON_MODE_NORMAL );		//m_cButtonMode = 0;
	}

	return;
}

BOOL AcUIButton::OnLButtonDblClk( RsMouseStatus *ms	)
{
	// AcUIBase의 MouseMove처리
	if( AcUIBase::OnLButtonDblClk( ms ) ) return TRUE;
	if( g_pEngine->m_bDraging ) return FALSE;

	//return OnLButtonDown( ms );
	return TRUE;
}

BOOL AcUIButton::OnMouseMove( RsMouseStatus *ms )
{
	// AcUIBase의 MouseMove처리
	if( AcUIBase::OnMouseMove( ms ) ) return TRUE;
	if( g_pEngine->m_bDraging ) return FALSE;
	return FALSE;
}

/*****************************************************************
*   Function : SetButtonMode
*   Comment  : SetButtonMode
*   Date&Time : 2003-06-24, 오전 11:38
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUIButton::SetButtonMode( UINT8 cMode, BOOL bSendMessage )
{
	if( cMode < ACUIBUTTON_MODE_NORMAL || cMode >= ACUIBUTTON_MAX_MODE ) return;
	if( m_cButtonMode == cMode )
	{
		SetRednerTexture( m_lButtonTextureID[ cMode ] );
		return;
	}

	// Disable로 바뀔 경우 이전 모드 저장
	if( cMode == ACUIBUTTON_MODE_DISABLE )
	{
		if( m_cButtonMode == ACUIBUTTON_MODE_ONMOUSE )
		{
			m_cButtonMode = ACUIBUTTON_MODE_NORMAL;
		}

		m_cPrevButtonMode = m_cButtonMode;
	}
	// Disable에서 바뀔 경우 이전 모드 Load
	else if( m_cButtonMode == ACUIBUTTON_MODE_DISABLE )
	{
		m_cButtonMode = m_cPrevButtonMode;
	}

	if( m_cButtonMode != cMode )
	{
		if( cMode == ACUIBUTTON_MODE_CLICK )
		{
			m_lStringOffsetX = m_lButtonDownStringOffsetX;
			m_lStringOffsetY = m_lButtonDownStringOffsetY;
		}
		else
		{
			m_lStringOffsetX = 0;
			m_lStringOffsetY = 0;
		}

		m_cButtonMode = cMode;

		if( bSendMessage && pParent )
		{
			if( cMode == ACUIBUTTON_MODE_CLICK )
			{
				if( m_stProperty.m_bPushButton )
				{
					pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BUTTON_MESSAGE_PUSHDOWN, ( PVOID )&m_lControlID );
				}

				pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BUTTON_MESSAGE_CLICK, ( PVOID )&m_lControlID );
			}
			else if( cMode == ACUIBUTTON_MODE_NORMAL && m_cButtonMode != ACUIBUTTON_MODE_ONMOUSE )
			{
				if( m_stProperty.m_bPushButton )
				{
					pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BUTTON_MESSAGE_PUSHUP, ( PVOID )&m_lControlID );
					pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_BUTTON_MESSAGE_CLICK, ( PVOID )&m_lControlID );
				}
			}
		}
	}

	SetRednerTexture( m_lButtonTextureID[ m_cButtonMode ] );
}

UINT8 AcUIButton::GetButtonMode( void )
{
	return m_cButtonMode;
}
