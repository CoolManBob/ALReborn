#include "AcUIMessageDialog.h"
#include "AgcmUIManager2.h"


extern AgcEngine *	g_pEngine;


//------------------------- AcUIWaitingDialog ------------------------- 
const DWORD AcUIWaitingDialog::s_dwWaitMaxTick = 1000 * 60 * 1;	//1분


AcUIWaitingDialog::AcUIWaitingDialog( void )
{
	memset( m_szButtonName, 0, sizeof( m_szButtonName ) );
}

AcUIWaitingDialog::~AcUIWaitingDialog( void )
{
}

BOOL AcUIWaitingDialog::OnInit( void )
{
	m_clClose.MoveWindow( ACUIMESSAGEDIALOG_OK_BUTTON_X, ACUIMESSAGEDIALOG_OK_BUTTON_Y, ACUIMESSAGEDIALOG_OK_BUTTON_WIDTH, ACUIMESSAGEDIALOG_OK_BUTTON_HEIGHT );
	AddChild( &m_clClose, ACUIMESSAGEDIALOG_OK_BUTTON );

	m_clClose.ShowWindow( FALSE );
	m_clClose.SetStaticStringExt( m_szButtonName, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clClose.m_lButtonDownStringOffsetY = 1;
	return TRUE;
}

void  AcUIWaitingDialog::SetMessage( char* szMessage )
{
	AgcModalWindow::SetMessage( szMessage );
	if( !szMessage ) return;

	INT32 lIndex;
	INT32 lLen;

	m_lLine = 0;

	while( TRUE )
	{
		lLen = m_pAgcmFont->GetTextExtent( 0, szMessage,strlen( szMessage ) );
		if( !lLen )	break;

		++m_lLine;
		if( m_lLine > ACUI_MESSAGE_MAX_LINE ) break;

		// 남은 String의 길이가 한 라인에 들어가면 마지막 라인에 Copy하고 끝.
		if( lLen < ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE )
		{
			strncpy( &m_aszMessageLine[ m_lLine - 1 ][ 0 ], szMessage, lLen );
			m_aszMessageLine[ m_lLine - 1 ][ lLen ] = 0;
			break;
		}

		// ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE 이하의 길이를 갖는 char array의 index를 찾는다.
		// 걍 뒤에서부터 무식허게 찾는다. binary search를 하면 더 좋을듯... 근디 싫다. 메렁~~~
		INT32 lMaxIndex = strlen( szMessage );
		while( TRUE )
		{
			INT32 l = m_pAgcmFont->GetTextExtent( 0, szMessage, lMaxIndex );
			if( l < ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE ) break;

			lMaxIndex--;
		}

		// 라인 끝에서부터 공백을 꺼구로 찾아들어간다.
		for( lIndex = lMaxIndex - 1 ; lIndex > 0 ; --lIndex )
		{
			if( szMessage[ lIndex ] == ' ' ) break;
		}

		// 공백이 없을 경우는 그냥 중간을 끊는다.
		if( lIndex == 0 )
		{
			lIndex = lMaxIndex - 1;
		}

		// 라인을 Copy
		strncpy( &m_aszMessageLine[ m_lLine - 1 ][ 0 ], szMessage, lIndex );
		m_aszMessageLine[ m_lLine - 1 ][ lIndex ] = 0;
		szMessage += lIndex;

		while( szMessage[ 0 ] == ' ' )
		{
			++szMessage;
		}
	}

	m_clClose.ShowWindow( FALSE );
	m_dwOpenTick = GetTickCount();
}

void AcUIWaitingDialog::OnWindowRender( void )	
{
	PROFILE( "AcUIWaitingDialog::OnWindowRender" );
	if( m_listTexture.GetCount() <= 0 ) return;

	//이곳에서... 활성화를 시켜줘야 할지 말아야할지...
	if( !m_clClose.m_Property.bVisible )
	{
		DWORD dwDiff = GetTickCount() - m_dwOpenTick;
		if( dwDiff > s_dwWaitMaxTick )
		{
			AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );

			memset( m_szButtonName, 0, sizeof( m_szButtonName ) );

			char* pMsg = pcmUIManager ? pcmUIManager->GetUIMessage( "UI_Name_OK" ) : "Exit";
			if( pMsg && strlen( pMsg ) > 0 )
			{
				strcpy( m_szButtonName, pMsg );
			}

			m_clClose.SetStaticStringExt( m_szButtonName, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
			m_clClose.m_lButtonDownStringOffsetY = 1;

			memset( m_szMessage, 0, sizeof( m_szMessage ) );
			pMsg = pcmUIManager ? pcmUIManager->GetUIMessage( "LOGIN_WAITING_FAILE" ) : "Connection Failed.";
			if( pMsg && strlen( pMsg ) > 0 )
			{
				SetMessage( pMsg );
			}			

			m_clClose.ShowWindow( TRUE );
		}
	}
	
	INT32 nX = 0, nY = 0;
	ClientToScreen( &nX, &nY );

	RwTexture* pTexture = m_listTexture.GetHead();
	if( pTexture && pTexture->raster )
	{
		g_pEngine->DrawRwTexture( pTexture, nX, nY );
	}

	if( m_pAgcmFont )
	{
		m_pAgcmFont->FontDrawStart( 0 );
		for( INT32 lIndex = 0 ; lIndex < m_lLine ; ++lIndex )
		{
			float fX = MagIToF( nX + ACUIMESSAGEDIALOG_WIDTH / 2 - m_pAgcmFont->GetTextExtent( 0, m_aszMessageLine[ lIndex ], strlen( m_aszMessageLine[ lIndex ] ) ) / 2 );
			float fY = MagIToF( nY + 70 - ( m_lLine - 1 ) * ( ACUI_MESSAGE_FONT_HEIGHT / 2 ) + lIndex * ACUI_MESSAGE_FONT_HEIGHT );
			m_pAgcmFont->DrawTextIM2D( fX, fY, m_aszMessageLine[ lIndex ], 0 );
		}

		m_pAgcmFont->FontDrawEnd();
	}
}

BOOL AcUIWaitingDialog::OnCommand( INT32 nID, PVOID pParam )
{
	AgcWindow* pcsWindow = ( AgcWindow* )pParam;
	AcUIBase* pcsBase = ( AcUIBase* )pParam;

	if( *( INT32* )pParam == m_clClose.m_lControlID )
	{
		if( nID == UICM_BUTTON_MESSAGE_CLICK )
		{
			RsEventHandler( rsQUITAPP, NULL );
			return TRUE;
		}
	}

	return TRUE;
}


//------------------------- AcUIMessageDialog ------------------------- 
AcUIMessageDialog::AcUIMessageDialog( void )
{
	memset( m_szMessage, 0, sizeof( m_szMessage ) );
	strcpy( m_szNameOK, "OK");
}

AcUIMessageDialog::~AcUIMessageDialog( void )
{
}

BOOL AcUIMessageDialog::OnInit( void )
{
	m_clOK.MoveWindow( ACUIMESSAGEDIALOG_OK_BUTTON_X, ACUIMESSAGEDIALOG_OK_BUTTON_Y, ACUIMESSAGEDIALOG_OK_BUTTON_WIDTH, ACUIMESSAGEDIALOG_OK_BUTTON_HEIGHT );
	AddChild( &m_clOK, ACUIMESSAGEDIALOG_OK_BUTTON );

	m_clOK.ShowWindow( TRUE );
	m_clOK.SetStaticStringExt( m_szNameOK, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clOK.m_lButtonDownStringOffsetY = 1;
	return TRUE;
}

void  AcUIMessageDialog::SetMessage( char* szMessage )
{
	AgcModalWindow::SetMessage( szMessage );

	INT32 lIndex;
	INT32 lLen;

	m_lLine = 0;
	while( TRUE )
	{
		// clear memory
		ZeroMemory( m_aszMessageLine[ m_lLine ], ACUI_MESSAGE_MAX_STRING_PER_LINE + 1 );

		lLen = m_pAgcmFont->GetTextExtent( 0, szMessage, strlen( szMessage ) );
		if( !lLen ) break;

		++m_lLine;
		if( m_lLine > ACUI_MESSAGE_MAX_LINE ) break;

		// '___' line feed가 한줄을 차지할 상황 - ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE보다
		// 작다면 한라인으로 만든다
		INT32 newLine = strcspn( szMessage, "___" );
		if( newLine != strlen( szMessage ) && m_pAgcmFont->GetTextExtent( 0, szMessage, newLine ) < ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE )
		{
			lIndex = newLine;

			// 라인을 Copy
			strncpy( &m_aszMessageLine[ m_lLine - 1 ][ 0 ], szMessage, lIndex );
			m_aszMessageLine[ m_lLine - 1 ][ lIndex ] = 0;

			// "___"만큼 뒤로
			lIndex += 3;
		}
		else
		{
			// 남은 String의 길이가 한 라인에 들어가면 마지막 라인에 Copy하고 끝.
			if( lLen < ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE )
			{
				strncpy( &m_aszMessageLine[ m_lLine - 1 ][ 0 ], szMessage, lLen );
				m_aszMessageLine[ m_lLine - 1 ][ lLen ] = 0;
				break;
			}

			// ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE 이하의 길이를 갖는 char array의 index를 찾는다.
			// 걍 뒤에서부터 무식허게 찾는다. binary search를 하면 더 좋을듯... 근디 싫다. 메렁~~~
			INT32 lMaxIndex = strlen( szMessage );
			while( TRUE )
			{
				INT32 l = m_pAgcmFont->GetTextExtent( 0, szMessage, lMaxIndex );
				if( l < ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE ) break;
				--lMaxIndex;
			}

			// 라인 끝에서부터 공백을 꺼구로 찾아들어간다.
			for( lIndex = lMaxIndex - 1; lIndex > 0; --lIndex )
			{
				if( szMessage[ lIndex ] == ' ' ) break;
			}

			// 공백이 없을 경우는 그냥 중간을 끊는다.
			if (lIndex == 0)
			{
				if( lMaxIndex > 0 && isleadbyte( szMessage[ lMaxIndex - 1 ] ) )
				{
					lIndex = lMaxIndex - 1;
				}
				else
				{
					lIndex = lMaxIndex - 2;
				}
			}

			// 라인을 Copy
			strncpy( &m_aszMessageLine[ m_lLine - 1 ][ 0 ], szMessage, lIndex );
			m_aszMessageLine[ m_lLine - 1 ][ lIndex ] = 0;

		}

		szMessage += lIndex;
		while( szMessage[ 0 ] == ' ')
		{
			++szMessage;
		}
	}
}

void AcUIMessageDialog::OnWindowRender( void )
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
			float fY = MagIToF( nY + 70 - ( m_lLine - 1 ) * ( ACUI_MESSAGE_FONT_HEIGHT / 2 ) + lIndex * ACUI_MESSAGE_FONT_HEIGHT );
			m_pAgcmFont->DrawTextIM2D( fX, fY, m_aszMessageLine[ lIndex ], 0 );
		}

		m_pAgcmFont->FontDrawEnd();
	}
}

BOOL AcUIMessageDialog::OnCommand( INT32 nID, PVOID pParam )
{
	AgcWindow* pcsWindow = ( AgcWindow* )pParam;
	AcUIBase* pcsBase = ( AcUIBase* )pParam;

	if( *( INT32* )pParam == m_clOK.m_lControlID )
	{
		if( nID == UICM_BUTTON_MESSAGE_CLICK )
		{
			EndDialog( 1 );
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AcUIMessageDialog::OnKeyDown( RsKeyStatus *ks	)
{
	if( rsENTER == ks->keyCharCode || ' ' == ks->keyCharCode )
	{
		EndDialog( 1 );
	}

	return TRUE;
}

VOID AcUIMessageDialog::SetButtonName( CHAR* szOK )
{
	if( szOK )
	{
		strcpy( m_szNameOK, szOK );
	}
	else
	{
		m_szNameOK[ 0 ] = NULL;
	}
}

//------------------------- AcUIOKCancelDialog ------------------------- 
AcUIOKCancelDialog::AcUIOKCancelDialog( void )
{
	memset( m_szMessage, 0, sizeof( m_szMessage ) );
	m_lOKCancelMode	= 0;

	strcpy( m_szNameOK, "OK" );
	strcpy( m_szNameCancel, "Cancel" );
}

AcUIOKCancelDialog::~AcUIOKCancelDialog( void )
{
}

BOOL AcUIOKCancelDialog::OnInit( void )
{
	m_clOK.MoveWindow( ACUIOKCANCELDIALOG_OK_BUTTON_X, ACUIOKCANCELDIALOG_OK_BUTTON_Y, ACUIOKCANCELDIALOG_BUTTON_WIDTH, ACUIOKCANCELDIALOG_BUTTON_HEIGHT );
	AddChild( &m_clOK, ACUIOKCANCELDIALOG_OK_BUTTON );
	m_clOK.ShowWindow( TRUE );

	m_clCancel.MoveWindow( ACUIOKCANCELDIALOG_CANCEL_BUTTON_X, ACUIOKCANCELDIALOG_CANCEL_BUTTON_Y, ACUIOKCANCELDIALOG_BUTTON_WIDTH, ACUIOKCANCELDIALOG_BUTTON_HEIGHT );
	AddChild( &m_clCancel, ACUIOKCANCELDIALOG_CANCEL_BUTTON );
	m_clCancel.ShowWindow( TRUE );
	
	m_clOK.SetStaticStringExt( m_szNameOK, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clCancel.SetStaticStringExt( m_szNameCancel, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );

	m_clOK.m_lButtonDownStringOffsetY = 1;
	m_clCancel.m_lButtonDownStringOffsetY = 1;
	return TRUE;
}

BOOL AcUIOKCancelDialog::OnCommand( INT32 nID, PVOID pParam )
{
	if( *( INT32* )pParam == m_clOK.m_lControlID )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			EndDialog( 1 );
			return TRUE;
		}
	}
	else if( *( INT32* )pParam == m_clCancel.m_lControlID )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			EndDialog( 0 );
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AcUIOKCancelDialog::OnKeyDown( RsKeyStatus *ks	)
{
	if( rsENTER == ks->keyCharCode || ' ' == ks->keyCharCode )
	{
		EndDialog( 1 );
	}
	else if( rsESC == ks->keyCharCode )
	{
		EndDialog( 0 );
	}

	return TRUE;	
}

VOID AcUIOKCancelDialog::SetButtonName( CHAR* szOK, CHAR* szCancel )
{
	if( szOK )
	{
		strcpy( m_szNameOK, szOK );
	}
	else
	{
		m_szNameOK[ 0 ] = NULL;
	}

	if( szOK )
	{
		strcpy( m_szNameCancel, szCancel );
	}
	else
	{
		m_szNameCancel[ 0 ] = NULL;
	}
}


//---------------------- AcUIEditOKDialog -------------------------
AcUIEditOKDialog::AcUIEditOKDialog( void )
{
	memset( m_szMessage, 0, sizeof( m_szMessage ) );
	strcpy( m_szNameOK, "OK");
}

AcUIEditOKDialog::~AcUIEditOKDialog( void )
{
}

BOOL AcUIEditOKDialog::OnInit( void )
{
	m_clOK.MoveWindow( ACUIMESSAGEDIALOG_OK_BUTTON_X, ACUIMESSAGEDIALOG_OK_BUTTON_Y, ACUIMESSAGEDIALOG_OK_BUTTON_WIDTH, ACUIMESSAGEDIALOG_OK_BUTTON_HEIGHT );
	AddChild( &m_clOK, ACUIMESSAGEDIALOG_OK_BUTTON );

	m_clOK.ShowWindow( TRUE );
	m_clOK.SetStaticStringExt( m_szNameOK, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clOK.m_lButtonDownStringOffsetY = 1;

	m_clEdit.MoveWindow( 60, 105, 200, 30 );
	AddChild( &m_clEdit, 3 );

	m_clEdit.ShowWindow( TRUE );
	m_clEdit.SetStaticStringExt( "", 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clEdit.SetTextMaxLength( 256 );
	m_clEdit.SetFocus();
	return TRUE;
}

void AcUIEditOKDialog::OnWindowRender( void )
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
			float fY = MagIToF( nY + 70 - ( m_lLine - 1 ) * ( ACUI_MESSAGE_FONT_HEIGHT / 2 ) + lIndex * ACUI_MESSAGE_FONT_HEIGHT );
			m_pAgcmFont->DrawTextIM2D( fX, fY, m_aszMessageLine[ lIndex ], 0 );
		}

		m_pAgcmFont->FontDrawEnd();
	}

	m_clEdit.OnWindowRender();
}

BOOL AcUIEditOKDialog::OnCommand( INT32 nID, PVOID pParam )
{
	AgcWindow* pcsWindow = ( AgcWindow* )pParam;
	AcUIBase* pcsBase = ( AcUIBase* )pParam;

	if( *( INT32* )pParam == m_clOK.m_lControlID )
	{
		if( nID == UICM_BUTTON_MESSAGE_CLICK )
		{
			EndDialog( 1 );
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AcUIEditOKDialog::OnKeyDown( RsKeyStatus* ks )
{
	if( m_clEdit.IsFocused() )
	{
		m_clEdit.OnKeyDown( ks );
	}

	return TRUE;
}

VOID AcUIEditOKDialog::SetButtonName( CHAR* szOK )
{
	if( szOK )
	{
		strcpy( m_szNameOK, szOK );
	}
	else
	{
		m_szNameOK[ 0 ] = NULL;
	}
}


