#include "CExNPC.h"
#include "AgppEventNPCDialog.h"
#include "AgcmEventNPCDialog.h"
#include "AgcmUIManager2.h"



CExNPC::CExNPC( void )
: m_LastMsgBoxPacket( -1, -1, -1, -1 )
{
}

CExNPC::~CExNPC( void )
{
}

void CExNPC::OnRegisterCallBack( void )
{
	AgcmEventNPCDialog* pcmNPC = ( AgcmEventNPCDialog* )m_pModule;
	if( !pcmNPC ) return;

	AgpmEventNPCDialog* ppmEventNPCDialog = ( AgpmEventNPCDialog* )pcmNPC->GetModule( "AgpmEventNPCDialog" );
	if( !ppmEventNPCDialog ) return;

	ppmEventNPCDialog->SetCallback( AGPMEVENT_NPCDIALOG_CB_GRANT_MESSAGEBOX, CBShowExNPCMsgBox, this );
	ppmEventNPCDialog->SetCallback( AGPMEVENT_NPCDIALOG_CB_GRANT_MENU, CBShowExNPCDialog, this );

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmNPC->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return;

	pcmCharacter->SetCallbackSelfUpdatePosition( CBAutoCloseByCharacterMove, this );
}

void CExNPC::OnMakeResource( void )
{
	if( !m_bIsResourceReady )
	{
		AgcmEventNPCDialog* pcmNPC = ( AgcmEventNPCDialog* )m_pModule;
		if( !pcmNPC ) return;

		AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )pcmNPC->GetModule( "AgcmResourceLoader" );
		if( !pcmResourceLoader ) return;

		pcmResourceLoader->SetTexturePath( "Texture\\UI\\" );

		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmNPC->GetModule( "AgcmUIManager2" );
		if( !pcmUIManager ) return;

		// NPC 다이얼로그 미리 생성
		OnInitNPCDialog( pcmUIManager );

		// NPC 메세지박스 미리 생성
		OnInitNPCMsgBox( pcmUIManager );

		m_bIsResourceReady = TRUE;
	}
}

void CExNPC::OnInitNPCMsgBox( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return;

	m_MsgBoxOK.OnInit();
	m_MsgBoxOK.ShowWindow( FALSE );

	m_MsgBoxYesNo.OnInit();
	m_MsgBoxYesNo.ShowWindow( FALSE );

	m_MsgBoxEditOK.OnInit();
	m_MsgBoxEditOK.ShowWindow( FALSE );

	m_MsgBoxGridItem.OnInit();
	m_MsgBoxGridItem.ShowWindow( FALSE );

	m_MsgBoxWait.OnInit();
	m_MsgBoxWait.ShowWindow( FALSE );

	int nPosX = ( pcmUIManager->m_lWindowWidth / 2 ) - ( AGCMUICONTROL_MESSAGE_WIDTH / 2 );
	int nPosY = ( pcmUIManager->m_lWindowHeight / 2 ) - ( AGCMUICONTROL_MESSAGE_HEIGHT / 2 );

	m_MsgBoxOK.MoveWindow( nPosX, nPosY, AGCMUICONTROL_MESSAGE_WIDTH, AGCMUICONTROL_MESSAGE_HEIGHT );
	m_MsgBoxYesNo.MoveWindow( nPosX, nPosY, AGCMUICONTROL_MESSAGE_WIDTH, AGCMUICONTROL_MESSAGE_HEIGHT );
	m_MsgBoxEditOK.MoveWindow( nPosX, nPosY, AGCMUICONTROL_MESSAGE_WIDTH, AGCMUICONTROL_MESSAGE_HEIGHT );
	m_MsgBoxGridItem.MoveWindow( nPosX, nPosY, AGCMUICONTROL_MESSAGE_WIDTH, AGCMUICONTROL_MESSAGE_HEIGHT );
	m_MsgBoxWait.MoveWindow( nPosX, nPosY, AGCMUICONTROL_MESSAGE_WIDTH, AGCMUICONTROL_MESSAGE_HEIGHT );

	// 모달리스 메세지박스는 콜백이 필요하다.
	m_MsgBoxGridItem.SetReturnCallBack( this, CBMsgBoxGridReturnItem );
	m_MsgBoxGridItem.SetCancelCallBack( this, CBMsgBoxGridCancelItem );
}

int CExNPC::OnShowNPCMsgBox( int nMsgBoxType, char* pText, char* pEditString )
{
	if( !pText ) return -1;

	AgcmEventNPCDialog* pcmNPC = ( AgcmEventNPCDialog* )m_pModule;
	if( !pcmNPC ) return -1;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmNPC->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return -1;

#ifdef _DEBUG
	if( strlen( pText ) <= 0 )
	{
		strcpy( pText, "문자열에 내용이 없어요!\n" );
	}
#endif

	return _ShowMsgBox( nMsgBoxType, pText, pEditString );
}

void CExNPC::OnSendNPCMsgBox( void* pMsgBoxData, int nResult, char* pEditText )
{
	PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX* pData = ( PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX* )pMsgBoxData;
	if( !pData || nResult < 0 ) return;

	PACKET_EVENTNPCDIALOG_REQUEST_MESSAGEBOX pPacket( pData->CID, pData->NID, pData->lEvnetIndex, pData->lStep );

	switch( nResult )
	{
	case 0 :	pPacket.bBtnIndex = FALSE;	break;	// No 버튼을 눌렀다.
	case 1 :	pPacket.bBtnIndex = TRUE;	break;	// OK 버튼 혹은 Yes 버튼을 눌렀다.
	default	:	return;
	}

	// 에디트박스가 있을 경우 거기에 입력된 문자열 복사
	if( pEditText && strlen( pEditText ) > 0 )
	{
		strcpy_s( pPacket.strString, sizeof( char ) * MESSAGE_MAX_SIZE, pEditText );
	}

	g_pEngine->SendPacket(pPacket);
}

void CExNPC::OnSendNPCMsgBoxItem( int nItemID, int nItemTID )
{
	PACKET_EVENTNPCDIALOG_REQUEST_MESSAGEBOX Packet( m_LastMsgBoxPacket.CID, m_LastMsgBoxPacket.NID, m_LastMsgBoxPacket.lEvnetIndex, m_LastMsgBoxPacket.lStep );

	Packet.nItemID = nItemID;
	Packet.nItemTID = nItemTID;
	Packet.bBtnIndex = TRUE;

	g_pEngine->SendPacket( Packet );

	AgcmEventNPCDialog* pcmNPC = ( AgcmEventNPCDialog* )m_pModule;
	if( !pcmNPC ) return;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmNPC->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return;

	m_MsgBoxGridItem.OnClearGridItem();
	m_MsgBoxGridItem.ShowWindow( FALSE );
	m_MsgBoxGridItem.EndDialog( 1 );
}

BOOL CExNPC::OnHideNPCMsgBox( void )
{
	_HideMsgBox( 1 );	// MsgBoxOK
	_HideMsgBox( 2 );	// MsgBoxYesNo
	_HideMsgBox( 3 );	// MsgBoxEditOK
	_HideMsgBox( 4 );	// MsgBoxGridItem
	_HideMsgBox( 5 );	// MsgBoxWait
	return TRUE;
}

void CExNPC::AddMsgBoxImage( char* pImageName, int nBtnMode )
{
	if( nBtnMode == -1 )
	{
		m_MsgBoxOK.AddImage( pImageName );
		m_MsgBoxYesNo.AddImage( pImageName );
		m_MsgBoxEditOK.AddImage( pImageName );
		m_MsgBoxGridItem.AddImage( pImageName );
		m_MsgBoxWait.AddImage( pImageName );
	}
	else
	{
		m_MsgBoxOK.m_clOK.AddButtonImage( pImageName, ( UINT8 )nBtnMode );
		m_MsgBoxYesNo.m_clOK.AddButtonImage( pImageName, ( UINT8 )nBtnMode );
		m_MsgBoxYesNo.m_clCancel.AddButtonImage( pImageName, ( UINT8 )nBtnMode );
		m_MsgBoxEditOK.m_clOK.AddButtonImage( pImageName, ( UINT8 )nBtnMode );
		m_MsgBoxGridItem.m_clOK.AddButtonImage( pImageName, ( UINT8 )nBtnMode );
		m_MsgBoxGridItem.m_clCancel.AddButtonImage( pImageName, ( UINT8 )nBtnMode );
		m_MsgBoxWait.m_clClose.AddButtonImage( pImageName, ( UINT8 )nBtnMode );
	}
}

void CExNPC::SetMsgBoxButtonName( char* pBtnNameOK, char* pBtnNameNo )
{
	m_MsgBoxOK.SetButtonName( pBtnNameOK );
	m_MsgBoxOK.m_clOK.SetStaticString( pBtnNameOK );

	m_MsgBoxYesNo.SetButtonName( pBtnNameOK, pBtnNameNo );
	m_MsgBoxYesNo.m_clOK.SetStaticString( pBtnNameOK );
	m_MsgBoxYesNo.m_clCancel.SetStaticString( pBtnNameNo );

	m_MsgBoxEditOK.SetButtonName( pBtnNameOK );
	m_MsgBoxEditOK.m_clOK.SetStaticString( pBtnNameOK );

	m_MsgBoxGridItem.SetButtonName( pBtnNameOK, pBtnNameNo );
	m_MsgBoxGridItem.m_clOK.SetStaticString( pBtnNameOK );
	m_MsgBoxGridItem.m_clCancel.SetStaticString( pBtnNameNo );
}

void CExNPC::CopyLastReceiveMsgBoxPacket( void* pPacket )
{
	if( !pPacket ) return;
	memcpy( &m_LastMsgBoxPacket, pPacket, sizeof( PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX ) );
}

BOOL CExNPC::IsOpenModalWindow( void )
{
	BOOL bIsOpenMsgBoxOK = m_MsgBoxOK.m_bOpened;
	BOOL bIsOpenMsgBoxYesNo = m_MsgBoxYesNo.m_bOpened;
	BOOL bIsOpenMsgBoxEditOK = m_MsgBoxEditOK.m_bOpened;
	BOOL bIsOpenMsgBoxGrid = m_MsgBoxGridItem.m_bOpened;
	BOOL bIsOpenMsgBoxWait = m_MsgBoxWait.m_bOpened;

	if( bIsOpenMsgBoxOK || bIsOpenMsgBoxYesNo || bIsOpenMsgBoxEditOK || bIsOpenMsgBoxGrid || bIsOpenMsgBoxWait )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CExNPC::IsOpenWaitingMsgBox( void )
{
	return m_MsgBoxWait.m_bOpened;
}

int CExNPC::_ShowMsgBox( int nMsgBoxType, char* pText, char* pEditString )
{
	AgcmEventNPCDialog* pcmNPC = ( AgcmEventNPCDialog* )m_pModule;
	if( !pcmNPC ) return -1;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmNPC->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return -1;

	if( !g_pEngine->m_pCurrentFullUIModule ) return -1;

	AgcModalWindow* pMsgBox = NULL;
	switch( nMsgBoxType )
	{
	case 1 : pMsgBox = &m_MsgBoxOK;			break;
	case 2 : pMsgBox = &m_MsgBoxEditOK;		break;
	case 3 : pMsgBox = &m_MsgBoxYesNo;		break;
	case 4 : pMsgBox = &m_MsgBoxGridItem;	break;
	case 5 : pMsgBox = &m_MsgBoxWait;		break;
	}

	if( !pMsgBox ) return -1;

	pMsgBox->ShowWindow( TRUE );
	pMsgBox->SetMessage( pText );

	int nResult = -1;
	if( nMsgBoxType != 4 )
	{
		// 모달타입
		nResult = pMsgBox->DoModal( g_pEngine->m_pCurrentFullUIModule );
	}
	else
	{
		pcmUIManager->AddWindow( pMsgBox );
	}

	if( nMsgBoxType == 3 )
	{
		const CHAR* pEditBoxText = m_MsgBoxEditOK.m_clEdit.GetText();
		if( pEditBoxText && strlen( pEditBoxText ) > 0 )
		{
			strcpy_s( pEditString, sizeof( char ) * 256, m_MsgBoxEditOK.m_clEdit.GetText() );
			m_MsgBoxEditOK.m_clEdit.ClearText();
		}
	}

	return nResult;
}

void CExNPC::_HideMsgBox( int nMsgBoxType )
{
	AgcModalWindow* pMsgBox = NULL;
	switch( nMsgBoxType )
	{
	case 1 : pMsgBox = &m_MsgBoxOK;			break;
	case 2 : pMsgBox = &m_MsgBoxEditOK;		break;
	case 3 : pMsgBox = &m_MsgBoxYesNo;		break;
	case 4 : pMsgBox = &m_MsgBoxGridItem;	break;
	case 5 : pMsgBox = &m_MsgBoxWait;		break;
	}

	if( pMsgBox && pMsgBox->m_bOpened )
	{
		pMsgBox->EndDialog( 0 );
		pMsgBox->ShowWindow( FALSE );

		AgcmEventNPCDialog* pcmNPC = ( AgcmEventNPCDialog* )m_pModule;
		if( !pcmNPC ) return;

		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmNPC->GetModule( "AgcmUIManager2" );
		if( !pcmUIManager ) return;

		pcmUIManager->RemoveWindow( pMsgBox );
		m_bIsResourceReady = FALSE;
	}
}

BOOL CExNPC::CBShowExNPCMsgBox( void* pData, void* pClass, void* pCustData )
{
	CExNPC* pThis = ( CExNPC* )pClass;
	PACKET_EVENTNPCDIALOG_GRANT_EX* pPacket = ( PACKET_EVENTNPCDIALOG_GRANT_EX* )pData;

	if( !pThis || !pPacket ) return FALSE;
	if( pPacket->pcOperation2 != AGPMEVENTNPCDIALOG_PACKET_GRANT_MESSAGEBOX ) return FALSE;

	// 기존에 모달리스 메세지 박스가 떠 있다면 닫아준다. 패킷은 보내지 않고 걍 닫는다.
	pThis->OnHideNPCMsgBox();

	PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX* pMsgBoxData = ( PACKET_EVENTNPCDIALOG_GRANT_MESSAGEBOX* )pPacket;

	pThis->CopyLastReceiveMsgBoxPacket( pMsgBoxData );
	CHAR strEditString[ MESSAGE_MAX_SIZE ] = { 0, };

	pThis->OnMakeResource();
	int nResult = pThis->OnShowNPCMsgBox( pMsgBoxData->lMessageBoxType, pMsgBoxData->strMessage, strEditString );
	if( nResult >= 0 )
	{
		pThis->OnSendNPCMsgBox( pMsgBoxData, nResult, strEditString );
	}

	return TRUE;
}

BOOL CExNPC::CBShowExNPCDialog( void* pData, void* pClass, void* pCustData )
{
	CExNPC* pThis = ( CExNPC* )pClass;
	PACKET_EVENTNPCDIALOG_GRANT_EX* pPacket = ( PACKET_EVENTNPCDIALOG_GRANT_EX* )pData;

	if( !pThis || !pPacket ) return FALSE;
	if( pPacket->pcOperation2 != AGPMEVENTNPCDIALOG_PACKET_GRANT_MENU ) return FALSE;

	// 새로뜨는 창이라면 기존의 창을 끄고..
	pThis->OnHideNPCDialog();

	// 기본 다이얼로그를 만들고..
	pThis->OnMakeResource();

	// 패킷데이터를 가져와서..
	PACKET_EVENTNPCDIALOG_GRANT_MENU* pMenuData = ( PACKET_EVENTNPCDIALOG_GRANT_MENU* )pPacket;

	// 가져온 패킷데이터로 창을 띄운다.
	pThis->OnShowNPCDialog( pMenuData );

	return TRUE;
}

void CExNPC::CBMsgBoxGridReturnItem( void* pCallerClass, int nItemID, int nItemTID )
{
	CExNPC* pThis = ( CExNPC* )pCallerClass;
	if( !pThis ) return;

	pThis->OnSendNPCMsgBoxItem( nItemID, nItemTID );
}

void CExNPC::CBMsgBoxGridCancelItem( void* pCallerClass )
{
	CExNPC* pThis = ( CExNPC* )pCallerClass;
	if( !pThis ) return;

	pThis->OnHideNPCMsgBox();
}
