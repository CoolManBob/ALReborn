#include "CUiExNPCMsgBoxGrid.h"
#include "AgcmUIControl.h"


CUiExNPCMsgBoxGrid::CUiExNPCMsgBoxGrid( void )
{
	m_fnCallBackOK = NULL;
	m_pCallerClassOK = NULL;

	m_fnCallBackNo = NULL;
	m_pCallerClassNo = NULL;
}

CUiExNPCMsgBoxGrid::~CUiExNPCMsgBoxGrid( void )
{
}

BOOL CUiExNPCMsgBoxGrid::OnInit( void )
{
	AcUIOKCancelDialog::OnInit();
	AddChild( &m_ItemGrid );

	_LoadResource();
	m_ItemGrid.Initialize( AGCMUICONTROL_MESSAGE_WIDTH, AGCMUICONTROL_MESSAGE_HEIGHT );
	return TRUE;
}

void CUiExNPCMsgBoxGrid::OnWindowRender( void )
{
	PROFILE("AcUIMessageDialog::OnWindowRender");
	if( m_listTexture.GetCount() <= 0 ) return;
	
	INT32 nX = 0 , nY = 0;
	ClientToScreen( &nX, &nY );

	RwTexture* pTexture = m_listTexture.GetHead();
	if( pTexture && pTexture->raster )
	{
		g_pEngine->DrawRwTexture( pTexture, nX, nY );
	}

	if( m_pAgcmFont )
	{
		m_pAgcmFont->FontDrawStart( 0 );

		for( INT32 lIndex = 0; lIndex < m_lLine; ++lIndex )
		{
			float fX = MagIToF( nX + ACUIMESSAGEDIALOG_WIDTH / 2 - m_pAgcmFont->GetTextExtent( 0, m_aszMessageLine[ lIndex ], strlen( m_aszMessageLine[ lIndex ] ) ) / 2 );
			float fY = MagIToF( nY + 40 - ( m_lLine - 1 ) * ( ACUI_MESSAGE_FONT_HEIGHT / 2 ) + lIndex * ACUI_MESSAGE_FONT_HEIGHT );
			m_pAgcmFont->DrawTextIM2D( fX, fY, m_aszMessageLine[ lIndex ], 0 );
		}

		m_pAgcmFont->FontDrawEnd();
	}
}

BOOL CUiExNPCMsgBoxGrid::OnCommand( INT32 nID, void* pParam )
{
	if( *( INT32* )pParam == m_clOK.m_lControlID )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			if( m_fnCallBackOK )
			{
				m_fnCallBackOK( m_pCallerClassOK, m_ItemGrid.GetItemID(), m_ItemGrid.GetItemTID() );
			}

			EndDialog( 1 );
			return TRUE;
		}
	}
	else if( *( INT32* )pParam == m_clCancel.m_lControlID )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			if( m_fnCallBackNo )
			{
				m_fnCallBackNo( m_pCallerClassNo );
			}

			EndDialog( 0 );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CUiExNPCMsgBoxGrid::OnKeyDown( RsKeyStatus* pKeyState )
{
	if( rsENTER == pKeyState->keyCharCode || ' ' == pKeyState->keyCharCode )
	{
		if( m_fnCallBackOK )
		{
			m_fnCallBackOK( m_pCallerClassOK, m_ItemGrid.GetItemID(), m_ItemGrid.GetItemTID() );
		}

		EndDialog( 1 );
		return TRUE;
	}

	if( rsESC == pKeyState->keyCharCode )
	{
		if( m_fnCallBackNo )
		{
			m_fnCallBackNo( m_pCallerClassNo );
		}

		EndDialog( 0 );
		return TRUE;
	}

	return FALSE;
}

void CUiExNPCMsgBoxGrid::OnClearGridItem( void )
{
	m_ItemGrid.OnClear();
}

void CUiExNPCMsgBoxGrid::SetButtonName( char* pBtnTextOK, char* pBtnTextNo )
{
	memset( m_szNameOK, 0, sizeof( char ) * ACUI_MESSAGE_BUTTONG_LENGTH );
	memset( m_szNameCancel, 0, sizeof( char ) * ACUI_MESSAGE_BUTTONG_LENGTH );

	if( !pBtnTextOK || strlen( pBtnTextOK ) <= 0 ) return;
	if( !pBtnTextNo || strlen( pBtnTextNo ) <= 0 ) return;
	
	strcpy_s( m_szNameOK, sizeof( char ) * ACUI_MESSAGE_BUTTONG_LENGTH, pBtnTextOK );
	strcpy_s( m_szNameCancel, sizeof( char ) * ACUI_MESSAGE_BUTTONG_LENGTH, pBtnTextNo );
}

void CUiExNPCMsgBoxGrid::_LoadResource( void )
{
	AddImage( "Common_Message_Base2.png" );

	m_clOK.AddButtonImage( "Common_Button_Small_A.png", ACUIBUTTON_MODE_NORMAL );
	m_clOK.AddButtonImage( "Common_Button_Small_B.png", ACUIBUTTON_MODE_ONMOUSE );
	m_clOK.AddButtonImage( "Common_Button_Small_C.png", ACUIBUTTON_MODE_CLICK );
	m_clOK.AddButtonImage( "Common_Button_Small_D.png", ACUIBUTTON_MODE_DISABLE );

	m_clCancel.AddButtonImage( "Common_Button_Small_A.png", ACUIBUTTON_MODE_NORMAL );
	m_clCancel.AddButtonImage( "Common_Button_Small_B.png", ACUIBUTTON_MODE_ONMOUSE );
	m_clCancel.AddButtonImage( "Common_Button_Small_C.png", ACUIBUTTON_MODE_CLICK );
	m_clCancel.AddButtonImage( "Common_Button_Small_D.png", ACUIBUTTON_MODE_DISABLE );
}

