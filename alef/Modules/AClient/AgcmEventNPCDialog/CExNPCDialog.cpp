#include "CExNPCDialog.h"
#include "AgcmUIManager2.h"
#include "AgcmEventNPCDialog.h"
#include "AgcmUIEventNPCDialog.h"
#include "AgppEventNPCDialog.h"



CExNPCDialogButton::CExNPCDialogButton( void )
{
	AcUIButton::AcUIButton();
	m_fnCallBack_Click = NULL;
}

CExNPCDialogButton::~CExNPCDialogButton( void )
{
	m_fnCallBack_Click = NULL;
}

BOOL CExNPCDialogButton::OnLButtonDown( RsMouseStatus* MouseStatus )
{
	AcUIButton::OnLButtonDown( MouseStatus );
	if( m_fnCallBack_Click )
	{
		m_fnCallBack_Click( m_pCallBackClass, this );
	}

	return TRUE;
}



CExNPCDialog::CExNPCDialog( void )
: m_LastDialogMenu( 0, 0 )
{
	m_pModule = NULL;
	m_pUIManager = NULL;
	m_pExNPCUI = NULL;
	m_pExNPCDialog = NULL;

	m_bIsOpen = FALSE;
	m_bIsResourceReady = FALSE;
}

CExNPCDialog::~CExNPCDialog( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )m_pUIManager;

	// 기존의 버튼들은 날려버리고
	_DeleteExNPCDialogButton();

	pcmUIManager->RemoveUI( ( AgcdUI* )m_pExNPCUI );
	pcmUIManager->DestroyUI( ( AgcdUI* )m_pExNPCUI );

	m_pUIManager = NULL;
	m_pExNPCUI = NULL;
	m_pExNPCDialog = NULL;
	m_pModule = NULL;

	m_bIsOpen = FALSE;
}

void CExNPCDialog::OnInitNPCDialog( void* pUIManager )
{
	// 이 작업은 한번만 하면 된다..
	if( !m_pUIManager && !m_pExNPCUI && !m_pExNPCDialog )
	{
		m_pUIManager = pUIManager;
		if( !m_pUIManager ) return;

		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )m_pUIManager;
		m_pExNPCUI = pcmUIManager->CreateUI();
		if( !m_pExNPCUI ) return;

		m_pExNPCDialog = ( ( AgcdUI* )m_pExNPCUI )->m_pcsUIWindow;
		if( !m_pExNPCDialog ) return;

		_MakeExNPCDialogBase();
		_MakeExNPCDialogBtnMenu();
	}
}

void CExNPCDialog::OnShowNPCDialog( void* pPacketData )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )m_pUIManager;
	if( !pcmUIManager ) return;

	AgcUIWindow* pUIWindow = ( AgcUIWindow* )m_pExNPCDialog;
	if( !pUIWindow ) return;

	AgcdUI* pcdUI = ( AgcdUI* )m_pExNPCUI;
	if( !pcdUI ) return;

	if( pPacketData )
	{
		memcpy( &m_LastDialogMenu, pPacketData, sizeof( PACKET_EVENTNPCDIALOG_GRANT_MENU ) );
		OnUpdateNPCDialog();
	}

	pcmUIManager->AddUI( ( AgcdUI* )m_pExNPCUI );
	pcmUIManager->OpenUI( pcdUI, FALSE, FALSE, FALSE );
	pUIWindow->ShowWindow( TRUE );
	pUIWindow->SetFocus();

	m_bIsOpen = TRUE;
}

BOOL CExNPCDialog::OnHideNPCDialog( void )
{
	if( !m_bIsOpen ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )m_pUIManager;
	if( !pcmUIManager ) return FALSE;

	AgcUIWindow* pUIWindow = ( AgcUIWindow* )m_pExNPCDialog;
	if( !pUIWindow ) return FALSE;

	pUIWindow->ShowWindow( FALSE );
	pcmUIManager->CloseUI( ( AgcdUI* )m_pExNPCUI, FALSE, FALSE, FALSE, FALSE );
	pcmUIManager->RemoveUI( ( AgcdUI* )m_pExNPCUI );

	m_bIsOpen = FALSE;
	m_bIsResourceReady = FALSE;
	return TRUE;
}

void CExNPCDialog::OnSencNPCDialog( void* pBtn )
{
	CExNPCDialogButton* pNPCDialogButton = ( CExNPCDialogButton* )pBtn;
	if( !pNPCDialogButton ) return;

	int nEventID = _GetEventIDByButton( pNPCDialogButton );
	int nEventStep = _GetEventStepByButton( pNPCDialogButton );
	if( nEventID < 0 || nEventStep < 0 ) return;

	PACKET_EVENTNPCDIALOG_REQUEST_MENU pPacket( m_LastDialogMenu.CID, m_LastDialogMenu.NID, nEventID, nEventStep );
	g_pEngine->SendPacket( pPacket );
}

void CExNPCDialog::OnSetDialogTitle( char* pText )
{
	if( m_pExNPCUI )
	{
		AgcdUI* pcdUI = ( AgcdUI* )m_pExNPCUI;

		if( !pText || strlen( pText ) <= 0 )
		{
			strcpy( pcdUI->m_szUIName, "NoTitle" );
			m_EditTitleBar.SetText( "NoTitle" );
		}
		else
		{
			strcpy( pcdUI->m_szUIName, pText );
			m_EditTitleBar.SetText( pText );
		}
	}
}

void CExNPCDialog::OnSetDialogMessage( char* pText )
{
	// NPC 다이얼로그의 메세지 텍스트
	if( pText && strlen( pText ) > 0 )
	{
		m_EditDialogText.SetText( pText );
	}
	else
	{
		m_EditDialogText.SetText( "NoText" );
	}
}

void CExNPCDialog::OnSetDialogButton( int nIndex, char* pText )
{
}

void CExNPCDialog::OnUpdateNPCDialog( void )
{
	OnSetDialogTitle( m_LastDialogMenu.strTitle );
	OnSetDialogMessage( m_LastDialogMenu.strDialog );

	_ClearExNPCDialogButton();

	for( int nCount = 0 ; nCount < MAX_BUTTON_COUNT ; nCount++ )
	{
		PACKET_EVENTNPCDIALOG_GRANT_MENU::stButton* pBtnData = &m_LastDialogMenu.ButtonArray[ nCount ];
		if( pBtnData )
		{
			_SetButtonEventInfo( nCount, pBtnData->EID, pBtnData->STEP, pBtnData->ItemTID );
			void* pBtn = _GetNPCDialogBtn( nCount );
			_UpdateNPCDialogBtn( pBtn, pBtnData );
		}
	}
}

void CExNPCDialog::_MakeExNPCDialogBase( void )
{
	AgcUIWindow* pUIWindow = ( AgcUIWindow* )m_pExNPCDialog;
	if( !pUIWindow ) return;

	// 윈도우 배경
	pUIWindow->MoveWindow( 107, 76, 236, 500 );
	pUIWindow->AddImage( "NPC_Dialog_Base.png" );
	pUIWindow->m_Property.bMovable = TRUE;
	pUIWindow->m_Property.bUseInput = TRUE;
	
	// 타이틀바
	_MakeExNPCDialogTitleBar();

	// 중단 텍스트박스
	_MakeExNPCDIalogTextBox();

	// X 버튼
	_MakeExNPCDIalogBtnClose();

	// KillFocus 정보를 초기화 해준다.
	AgcdUI* pcdUI = ( AgcdUI* )m_pExNPCUI;
	if( !pcdUI ) return;

	pcdUI->m_pstKillFocusFunction = NULL;
}

void CExNPCDialog::_MakeExNPCDialogTitleBar( void )
{
	AgcUIWindow* pUIWindow = ( AgcUIWindow* )m_pExNPCDialog;
	if( !pUIWindow ) return;

	m_EditTitleBar.MoveWindow( 28, 3, 150, 16 );
	m_EditTitleBar.ShowWindow( TRUE );
	m_EditTitleBar.SetStaticString( "" );
	m_EditTitleBar.SetTextMaxLength( 256 );
	m_EditTitleBar.m_bAutoLF = TRUE;
	m_EditTitleBar.m_bReadOnly = TRUE;
	m_EditTitleBar.SetStaticStringExt( "", 1.0, 0, 0xFFFFFFFF, true, ACUIBASE_HALIGN_LEFT, ACUIBASE_VALIGN_CENTER );
	m_EditTitleBar.m_Property.bUseInput = FALSE;
	pUIWindow->AddChild( &m_EditTitleBar );
}

void CExNPCDialog::_MakeExNPCDIalogTextBox( void )
{
	AgcUIWindow* pUIWindow = ( AgcUIWindow* )m_pExNPCDialog;
	if( !pUIWindow ) return;

	m_EditDialogText.MoveWindow( 15, 38, 200, 231 );
	m_EditDialogText.ShowWindow( TRUE );
	m_EditDialogText.SetStaticString( "" );
	m_EditDialogText.SetTextMaxLength( 256 );
	m_EditDialogText.m_bAutoLF = TRUE;
	m_EditDialogText.m_bReadOnly = TRUE;
	m_EditDialogText.SetLineDelimiter( "\n" );
	m_EditDialogText.SetStaticStringExt( "", 1.0, 0, 0xFFFFFFFF, true, ACUIBASE_HALIGN_LEFT, ACUIBASE_VALIGN_CENTER );
	pUIWindow->AddChild( &m_EditDialogText );
}

void CExNPCDialog::_MakeExNPCDIalogBtnClose( void )
{
	AgcUIWindow* pUIWindow = ( AgcUIWindow* )m_pExNPCDialog;
	if( !pUIWindow ) return;

	// 상단 타이틀바 X버튼
	m_BtnClose.MoveWindow( 203, 3, 16, 16 );
	m_BtnClose.ShowWindow( TRUE );
	m_BtnClose.AddButtonImage( "Common_Button_X_A.png", ACUIBUTTON_MODE_NORMAL );
	m_BtnClose.AddButtonImage( "Common_Button_X_B.png", ACUIBUTTON_MODE_ONMOUSE );
	m_BtnClose.AddButtonImage( "Common_Button_X_C.png", ACUIBUTTON_MODE_CLICK );
	m_BtnClose.AddButtonImage( "Common_Button_X_D.png", ACUIBUTTON_MODE_DISABLE );
	m_BtnClose.OnRegisterCallBack( this, CBClickCloseButton );
	pUIWindow->AddChild( &m_BtnClose );
}

void CExNPCDialog::_MakeExNPCDialogBtnMenu( void )
{
	AgcUIWindow* pUIWindow = ( AgcUIWindow* )m_pExNPCDialog;
	if( !pUIWindow ) return;

	for( int nCount = 0 ; nCount < MAX_BUTTON_COUNT ; nCount++ )
	{
		CExNPCDialogButton* pBtn = new CExNPCDialogButton;
		if( !pBtn ) return;

		pBtn->AddButtonImage( "Common_Button_Large_A.png", ACUIBUTTON_MODE_NORMAL );
		pBtn->AddButtonImage( "Common_Button_Large_B.png", ACUIBUTTON_MODE_ONMOUSE );
		pBtn->AddButtonImage( "Common_Button_Large_C.png", ACUIBUTTON_MODE_CLICK );
		pBtn->AddButtonImage( "Common_Button_Large_D.png", ACUIBUTTON_MODE_DISABLE );

		int nWidth = 158;
		int nHeight = 17;
		int nOffSet = 3;
		int nPosX = 37;
		int nPosY = 293 + ( ( nHeight + nOffSet ) * nCount );

		pBtn->MoveWindow( nPosX, nPosY, nWidth, nHeight );
		pBtn->m_lButtonDownStringOffsetY = 1;
		pBtn->ShowWindow( FALSE );

		// 버튼에 클릭이벤트 처리기 장착
		pBtn->OnRegisterCallBack( this, CBClickMenuButton );
		pUIWindow->AddChild( pBtn );

		stExNPCButtonEntry NewButton;
		NewButton.m_pBtn = pBtn;
		m_mapDialogButtons.Add( m_mapDialogButtons.GetSize(), NewButton );
	}
}

void CExNPCDialog::_ClearExNPCDialogButton( void )
{
	AgcUIWindow* pUIWindow = ( AgcUIWindow* )m_pExNPCDialog;
	if( !pUIWindow ) return;
	int nBtnCount = m_mapDialogButtons.GetSize();

	for( int nCount = 0 ; nCount < nBtnCount ; nCount++ )
	{
		stExNPCButtonEntry* pEntry = m_mapDialogButtons.GetByIndex( nCount );
		if( pEntry )
		{
			CExNPCDialogButton* pBtn = ( CExNPCDialogButton* )pEntry->m_pBtn;
			if( pBtn )
			{
				pBtn->ShowWindow( FALSE );
			}

			pEntry->ReSet();
		}
	}
}

void CExNPCDialog::_DeleteExNPCDialogButton( void )
{
	AgcUIWindow* pUIWindow = ( AgcUIWindow* )m_pExNPCDialog;
	if( !pUIWindow ) return;
	int nBtnCount = m_mapDialogButtons.GetSize();

	for( int nCount = 0 ; nCount < nBtnCount ; nCount++ )
	{
		stExNPCButtonEntry* pEntry = m_mapDialogButtons.GetByIndex( nCount );
		if( pEntry )
		{
			pEntry->ReSet();
			if( pEntry->m_pBtn )
			{
				pUIWindow->DeleteChild( ( CExNPCDialogButton* )pEntry->m_pBtn );

				delete pEntry->m_pBtn;
				pEntry->m_pBtn = NULL;
			}
		}
	}

	m_mapDialogButtons.Clear();
}

void CExNPCDialog::_UpdateNPCDialogBtn( void* pBtn, void* pBtnData )
{
	CExNPCDialogButton* pMenuBtn = ( CExNPCDialogButton* )pBtn;
	PACKET_EVENTNPCDIALOG_GRANT_MENU::stButton* pBtnInfo = ( PACKET_EVENTNPCDIALOG_GRANT_MENU::stButton* )pBtnData;
	if( !pMenuBtn || !pBtnInfo ) return;

	if( pBtnInfo->EID < 0 ) return;
	if( pBtnInfo->STEP < 0 ) return;
	if( strlen( pBtnInfo->strText ) <= 0 ) return;

	pMenuBtn->SetStaticStringExt( pBtnInfo->strText, 1.0, 0, 0xFFFFFFFF, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	pMenuBtn->ShowWindow( TRUE );
}

void* CExNPCDialog::_GetNPCDialogBtn( int nIndex )
{
	stExNPCButtonEntry* pEntry = m_mapDialogButtons.Get( nIndex );
	if( !pEntry ) return NULL;
	return pEntry->m_pBtn;
}

void CExNPCDialog::_SetButtonEventInfo( int nIndex, int nEventID, int nStepID, int nItemTID )
{
	stExNPCButtonEntry* pEntry = m_mapDialogButtons.Get( nIndex );
	if( pEntry )
	{
		pEntry->m_nEventID = nEventID;
		pEntry->m_nStep = nStepID;
		pEntry->m_nItemTID = nItemTID;
	}
}

int CExNPCDialog::_GetEventIDByButton( CExNPCDialogButton* pBtn )
{
	if( !pBtn ) return -1;

	int nBtnCount = m_mapDialogButtons.GetSize();
	for( int nCount = 0 ; nCount < nBtnCount ; nCount++ )
	{
		stExNPCButtonEntry* pEntry = m_mapDialogButtons.GetByIndex( nCount );
		if( pEntry )
		{
			if( pEntry->m_pBtn == pBtn )
			{
				return pEntry->m_nEventID;
			}
		}
	}

	return -1;
}

int CExNPCDialog::_GetEventStepByButton( CExNPCDialogButton* pBtn )
{
	if( !pBtn ) return -1;

	int nBtnCount = m_mapDialogButtons.GetSize();
	for( int nCount = 0 ; nCount < nBtnCount ; nCount++ )
	{
		stExNPCButtonEntry* pEntry = m_mapDialogButtons.GetByIndex( nCount );
		if( pEntry )
		{
			if( pEntry->m_pBtn == pBtn )
			{
				return pEntry->m_nStep;
			}
		}
	}

	return -1;
}

void* CExNPCDialog::GetModule( char* pModuleName )
{
	if( !pModuleName || strlen( pModuleName ) <= 0 ) return NULL;

	AgcmEventNPCDialog* pcmNPC = ( AgcmEventNPCDialog* )m_pModule;
	if( !pcmNPC ) return NULL;

	return pcmNPC->GetModule( pModuleName );
}

BOOL CExNPCDialog::IsExNpcDialog( char* pUiName )
{
	if( !pUiName || strlen( pUiName ) <= 0 ) return FALSE;

	AgcdUI* pcdUI = ( AgcdUI* )m_pExNPCUI;
	if( !pcdUI ) return FALSE;

	return strcmp( pcdUI->m_szUIName, pUiName ) == 0 ? TRUE : FALSE;
}

BOOL CExNPCDialog::CBClickMenuButton( void* pClass, void* pControl )
{
	CExNPCDialog* pDialog = ( CExNPCDialog* )pClass;
	CExNPCDialogButton* pBtn = ( CExNPCDialogButton* )pControl;
	if( !pDialog || !pBtn ) return FALSE;

	pDialog->OnSencNPCDialog( pBtn );
	pDialog->OnHideNPCDialog();
	return TRUE;
}

BOOL CExNPCDialog::CBClickCloseButton( void* pClass, void* pControl )
{
	CExNPCDialog* pDialog = ( CExNPCDialog* )pClass;
	if( !pDialog ) return FALSE;

	pDialog->OnHideNPCDialog();
	return TRUE;
}

BOOL CExNPCDialog::CBAutoCloseByCharacterMove( void* pData, void* pClass, void* pCustData )
{
	CExNPCDialog* pThis = ( CExNPCDialog* )pClass;
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pData;
	if( !pThis || !ppdCharacter ) return FALSE;

	CExNPC* pExNPC = ( CExNPC* )pClass;
	if( pExNPC )
	{
		pExNPC->OnHideNPCDialog();
		pExNPC->OnHideNPCMsgBox();		
	}

	return TRUE;
}
