#include "AgcmUIControl.h"
#include "AgcmUIManager2.h"
#include "AgcmEventNPCDialog.h"

#define RW_TEXTURE_DESTROY( p )				{ if( p ) {	RwTextureDestroy( p ); ( p ) = NULL; } }

// ----------------- CUIOpenCloseSystem ------------------
CUIOpenCloseSystem::CUIOpenCloseSystem( void )
{
}

CUIOpenCloseSystem::~CUIOpenCloseSystem( void )
{
}

void CUIOpenCloseSystem::Init( AgcmUIManager2* pUIMgr, CHAR* szUIName )
{
	m_pcsAgcmUIManager2 = pUIMgr;
	strncpy( m_szUIName, szUIName, MAX_PATH );
}

BOOL CUIOpenCloseSystem::IsOpen( void )
{
	AgcdUI* pUI = m_pcsAgcmUIManager2->GetUI( m_szUIName );
	if( !pUI ) return FALSE;

	return AGCDUI_STATUS_OPENED == pUI->m_eStatus ? TRUE : FALSE;
}

BOOL CUIOpenCloseSystem::Open( void )
{
	AgcdUI* pUI = m_pcsAgcmUIManager2->GetUI( m_szUIName );
	if( !pUI ) return FALSE;

	m_pcsAgcmUIManager2->OpenUI( pUI );
	return TRUE;
}

BOOL CUIOpenCloseSystem::Close( void )
{
	AgcdUI* pUI = m_pcsAgcmUIManager2->GetUI( m_szUIName );
	if( !pUI ) return FALSE;

	m_pcsAgcmUIManager2->CloseUI( pUI );
	return TRUE;
}

// ----------------- AgcmUIControl ------------------
AgcmUIControl::AgcmUIControl( void )
{
	SetModuleName("AgcmUIControl");

	m_pComboBackTexture	= NULL;
	m_pcsAgpmGrid		= NULL;
	m_pBaseTexture		= NULL;

	for( INT32 nToolTipTextureCount = 0 ; nToolTipTextureCount < ACUITOOLTIP_TTTEXTURE_NUM ; ++nToolTipTextureCount )
	{
		m_pToolTipTexture[ nToolTipTextureCount ] = NULL;
	}

	m_szButtonClickSound[ 0 ]		= NULL;
	m_lItemGridTextureADDataIndex	= -1;

	m_pConvertBase					= NULL;
	m_pConvertDisable				= NULL;
	m_pConvertOutline				= NULL;

	m_pPvPFree						= NULL;
	m_pPvPDanger					= NULL;

	m_pCancelTransform				= NULL;
	m_pCancelEvolution				= NULL;

	m_pDurabilityZero				= NULL;

	m_pCatapultUse					= NULL;
	m_pCatapultRepair				= NULL;

	m_pAtkResTowerUse				= NULL;
	m_pAtkResTowerRepair			= NULL;

	m_pCastleOwnerHuman				= NULL;
	m_pCastleOwnerOrc				= NULL;
	m_pCastleOwnerMoonelf			= NULL;
	m_pCastleOwnerDragonScion		= NULL;
	m_pCastleOwnerArchlord			= NULL;

	m_pArchlordGuildMark			= NULL;
	m_pGoMark						= NULL;
	m_pPetState						= NULL;

	m_pTextureHangameS				= NULL;
	m_pTextureHangameT				= NULL;
}

AgcmUIControl::~AgcmUIControl( void )
{
	if( m_clWaitingDialog.pParent )
	{
		AgcWindow* pcsParent = m_clWaitingDialog.pParent;
		pcsParent->DeleteChild( &m_clWaitingDialog, FALSE );
		pcsParent->WindowListUpdate();
	}

	if( m_clMessageDialog.pParent )
	{
		AgcWindow* pcsParent = m_clMessageDialog.pParent;
		pcsParent->DeleteChild( &m_clMessageDialog, FALSE );
		pcsParent->WindowListUpdate();
	}

	if( m_clOkCancelDialog.pParent )
	{
		AgcWindow* pcsParent = m_clOkCancelDialog.pParent;
		pcsParent->DeleteChild( &m_clOkCancelDialog, FALSE );
		pcsParent->WindowListUpdate();
	}

	if( m_clEditOKDialog.pParent )
	{
		AgcWindow* pcsParent = m_clEditOKDialog.pParent;
		pcsParent->DeleteChild( &m_clEditOKDialog, FALSE );
		pcsParent->WindowListUpdate();
	}
}

BOOL AgcmUIControl::OnAddModule( void )						
{
	AcUIBase::m_pAgcmUIControl		= this;

	AcUIBase::m_pAgcmSound			= ( AgcmSound* )GetModule( "AgcmSound" );
	AcUIModalImage::m_pAgcmSound	= ( AgcmSound* )GetModule( "AgcmSound" );

	AcUIBase::m_pAgcmFont			= ( AgcmFont* )GetModule( "AgcmFont" );
	AcUIModalImage::m_pAgcmFont		= ( AgcmFont* )GetModule( "AgcmFont" );

	m_pcsAgpmGrid					= ( AgpmGrid* )GetModule( "AgpmGrid" );
	m_lItemGridTextureADDataIndex	= m_pcsAgpmGrid->AttachGridItemTextureData( this, sizeof( RwTexture* ), ItemGridTextureConstructor, ItemGridTextureDestructor );
	
	for( UINT32 nToolTipTextureCount = 0 ; nToolTipTextureCount < 9 ; ++nToolTipTextureCount )
	{
		if( AcUIToolTip::m_pTTTexture[ nToolTipTextureCount ] )
		{
			RwTextureSetFilterMode( AcUIToolTip::m_pTTTexture[ nToolTipTextureCount ], rwFILTERLINEAR );
		}
	}

	return TRUE;
}

BOOL AgcmUIControl::OnInit( void )
{
	m_clWaitingDialog.OnInit();
	m_clWaitingDialog.ShowWindow( FALSE );
	g_pEngine->SetWaitingDialog( ( AgcModalWindow* )&m_clWaitingDialog );

	m_clMessageDialog.OnInit();
	m_clMessageDialog.ShowWindow( FALSE );
	g_pEngine->SetMessageDialog( ( AgcModalWindow* )&m_clMessageDialog );

	m_clOkCancelDialog.OnInit();
	m_clOkCancelDialog.ShowWindow( FALSE );
	g_pEngine->SetOkCancelDialog( ( AgcModalWindow* )&m_clOkCancelDialog );	

	m_clEditOKDialog.OnInit();
	m_clEditOKDialog.ShowWindow( FALSE );
	g_pEngine->SetEditOkDialog( ( AgcModalWindow* )&m_clEditOKDialog );	

	return TRUE;
}

void AgcmUIControl::OnClose( void )
{
}

BOOL AgcmUIControl::UIIniRead( CHAR* szName, BOOL bDecryption )
{
	AuIniManagerA clIniManager;
	clIniManager.SetPath( szName );

	if( !clIniManager.ReadFile( 0, bDecryption ) )
	{
		TRACE( "AgcmUIManager - UI Ini Read실패\n" );
		return FALSE;
	}
	
	INT32 lNumSection = clIniManager.GetNumSection();
	if( lNumSection < 1 )
	{
		TRACE( "AgcmUIManager - UI Ini Read실패\n");
		return FALSE;
	}
	else if( lNumSection == 1 )
	{
		return FALSE;
	}

	_LoadTextureFromINI( &clIniManager );
	_LoadSoundFromINI( &clIniManager );
	return TRUE;
}

BOOL	AgcmUIControl::OnDestroy()						
{
	for ( int i = 0 ; i < ACUITOOLTIP_TTTEXTURE_NUM ; ++i )
	{
		RW_TEXTURE_DESTROY( m_pToolTipTexture[i] );
		AcUIToolTip::m_pTTTexture[i] = NULL;
	}

	RW_TEXTURE_DESTROY( m_pBaseTexture );
	AcUIBase::m_pBaseTexture = NULL;
	
	RW_TEXTURE_DESTROY( m_pComboBackTexture );
	RW_TEXTURE_DESTROY( m_pConvertBase );
	RW_TEXTURE_DESTROY( m_pConvertDisable );
	RW_TEXTURE_DESTROY( m_pConvertOutline );

	RW_TEXTURE_DESTROY( m_pCastleOwnerHuman );
	RW_TEXTURE_DESTROY( m_pCastleOwnerOrc );
	RW_TEXTURE_DESTROY( m_pCastleOwnerMoonelf );
	RW_TEXTURE_DESTROY( m_pCastleOwnerDragonScion );
	RW_TEXTURE_DESTROY( m_pCastleOwnerArchlord );

	m_clWaitingDialog.OnClose();
	m_clWaitingDialog.m_clClose.OnClose();

	m_clMessageDialog.OnClose();
	m_clMessageDialog.m_clOK.OnClose();

	m_clOkCancelDialog.OnClose();
	m_clOkCancelDialog.m_clOK.OnClose();
	m_clOkCancelDialog.m_clCancel.OnClose();

	m_clEditOKDialog.OnClose();
	m_clEditOKDialog.m_clOK.OnClose();
	m_clEditOKDialog.m_clEdit.OnClose();


	return TRUE;
}

RwTexture** AgcmUIControl::GetAttachGridItemTextureData(AgpdGridItem *pcsGridItem)
{
	if ( !pcsGridItem )		return NULL;

	return (RwTexture**) m_pcsAgpmGrid->GetAttachedModuleData( m_lItemGridTextureADDataIndex, pcsGridItem );
}

RwTexture*	AgcmUIControl::GetGridItemTexture( AgpdGridItem* pstGridItem )
{
	if( !pstGridItem )		return NULL;

	RwTexture** ppRwTexture = GetAttachGridItemTextureData( pstGridItem );
	return ppRwTexture ? *ppRwTexture : NULL;
}

RwTexture*	AgcmUIControl::GetGridItemTexture( AgpdGrid* pstGrid, INT32 lLayer, INT32 lRow, INT32 lColumn )
{
	return GetGridItemTexture( m_pcsAgpmGrid->GetItem( pstGrid, (INT16)lLayer, (INT16)lRow, (INT16)lColumn ) );
}

AgpdGridItem*	AgcmUIControl::GetGridItemPointer( AgpdGrid* pstGrid, INT32 lLayer, INT32 lRow, INT32 lColumn )
{
	return m_pcsAgpmGrid->GetItem( pstGrid, (INT16)lLayer, (INT16)lRow, (INT16)lColumn );
}	

BOOL	AgcmUIControl::SetMessageDialogPosition(INT32 lPosX, INT32 lPosY)
{
	m_clWaitingDialog.MoveWindow( lPosX, lPosY, AGCMUICONTROL_MESSAGE_WIDTH, AGCMUICONTROL_MESSAGE_HEIGHT );
	m_clMessageDialog.MoveWindow( lPosX, lPosY, AGCMUICONTROL_MESSAGE_WIDTH, AGCMUICONTROL_MESSAGE_HEIGHT );
	m_clOkCancelDialog.MoveWindow( lPosX, lPosY, AGCMUICONTROL_MESSAGE_WIDTH, AGCMUICONTROL_MESSAGE_HEIGHT );
	m_clEditOKDialog.MoveWindow( lPosX, lPosY, AGCMUICONTROL_MESSAGE_WIDTH, AGCMUICONTROL_MESSAGE_HEIGHT );

	return TRUE;
}

VOID	AgcmUIControl::SetButtonName(CHAR *szOK, CHAR *szCancel)
{
	m_clMessageDialog.SetButtonName(szOK);
	m_clOkCancelDialog.SetButtonName(szOK, szCancel);
	m_clEditOKDialog.SetButtonName( szOK );
}

CHAR *	AgcmUIControl::GetCustomTexture(CHAR *szName)
{
	for ( CustomTextureItr Itr = m_vCustomTexture.begin(); Itr != m_vCustomTexture.end(); ++Itr )
		if( !_tcsicmp( (*Itr).szName, szName ) )
			return (*Itr).szTextureName;

	return NULL;
}


void AgcmUIControl::_LoadTextureFromINI( AuIniManagerA* pINIManager )
{
	if( !pINIManager ) return;

	// [TEXTURE] Setcion 검색 - 첫번째 Section이 UI Texture Name 
	INT32 lNumKey_Section0 = pINIManager->GetNumKeys( 0 );

	// Tool Tip에서 쓰일 Texture Read하기 
	for( INT32 nKeyCount = 0 ; nKeyCount < lNumKey_Section0 ; ++nKeyCount )
	{
		const CHAR* szKeyName =	pINIManager->GetKeyName( 0, nKeyCount );
		CHAR* szKeyValue = pINIManager->GetValue( 0, nKeyCount );

		if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_TT_BACK ) )
		{
			m_pToolTipTexture[ 0 ] = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pToolTipTexture[ 0 ] );
			AcUIToolTip::m_pTTTexture[ 0 ] = m_pToolTipTexture[ 0 ];
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_TT_TLINE ) )
		{
			m_pToolTipTexture[ 1 ] = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pToolTipTexture[ 1 ] );
			AcUIToolTip::m_pTTTexture[ 1 ] = m_pToolTipTexture[ 1 ];
		}
		else if (!strcmp(szKeyName, AGCMUICONTROL_KEY_NAME_TT_BLINE))
		{
			m_pToolTipTexture[ 2 ] = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pToolTipTexture[ 2 ] );
			AcUIToolTip::m_pTTTexture[ 2 ] = m_pToolTipTexture[ 2 ];
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_TT_LLINE ) )
		{
			m_pToolTipTexture[ 3 ] = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pToolTipTexture[ 3 ] );
			AcUIToolTip::m_pTTTexture[ 3 ] = m_pToolTipTexture[ 3 ];
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_TT_RLINE ) )
		{
			m_pToolTipTexture[ 4 ] = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pToolTipTexture[ 4 ] );
			AcUIToolTip::m_pTTTexture[ 4 ] = m_pToolTipTexture[ 4 ];
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_TT_TLCORNER ) )
		{
			m_pToolTipTexture[ 5 ] = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pToolTipTexture[ 5 ] );
			AcUIToolTip::m_pTTTexture[ 5 ] = m_pToolTipTexture[ 5 ];
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_TT_TRCORNER ) )
		{
			m_pToolTipTexture[ 6 ] = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pToolTipTexture[ 6 ] );
			AcUIToolTip::m_pTTTexture[ 6 ] = m_pToolTipTexture[ 6 ];
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_TT_BLCORNER ) )
		{
			m_pToolTipTexture[ 7 ] = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pToolTipTexture[ 7 ] );
			AcUIToolTip::m_pTTTexture[ 7 ] = m_pToolTipTexture[ 7 ];
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_TT_BRCORNER ) )
		{
			m_pToolTipTexture[ 8 ] = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pToolTipTexture[ 8 ] );
			AcUIToolTip::m_pTTTexture[ 8 ] = m_pToolTipTexture[ 8 ];
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_BASE ) )
		{
			m_pBaseTexture = RwTextureRead( szKeyValue, NULL );
			if( m_pBaseTexture )
			{
				AcUIBase::m_pBaseTexture = m_pBaseTexture;
			}
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_MESSAGE_BACK ) )
		{
			m_clWaitingDialog.AddImage( szKeyValue );
			m_clMessageDialog.AddImage( szKeyValue );
			m_clOkCancelDialog.AddImage( szKeyValue );
			m_clEditOKDialog.AddImage( szKeyValue );

			AgcmEventNPCDialog* pcmEventNPCDialog = ( AgcmEventNPCDialog* )GetModule( "AgcmEventNPCDialog" );
			if( pcmEventNPCDialog )
			{
				CExNPC* pExNPC = pcmEventNPCDialog->GetExNPC();
				if( pExNPC )
				{
					pExNPC->AddMsgBoxImage( szKeyValue );
				}
			}
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_MESSAGE_BUTTON0 ) )
		{
			m_clWaitingDialog.m_clClose.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_NORMAL );
			m_clMessageDialog.m_clOK.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_NORMAL	);
			m_clOkCancelDialog.m_clOK.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_NORMAL );
			m_clOkCancelDialog.m_clCancel.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_NORMAL );
			m_clEditOKDialog.m_clOK.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_NORMAL );

			AgcmEventNPCDialog* pcmEventNPCDialog = ( AgcmEventNPCDialog* )GetModule( "AgcmEventNPCDialog" );
			if( pcmEventNPCDialog )
			{
				CExNPC* pExNPC = pcmEventNPCDialog->GetExNPC();
				if( pExNPC )
				{
					pExNPC->AddMsgBoxImage( szKeyValue, ACUIBUTTON_MODE_NORMAL );
				}
			}
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_MESSAGE_BUTTON1 ) )
		{
			m_clWaitingDialog.m_clClose.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_ONMOUSE	);
			m_clMessageDialog.m_clOK.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_ONMOUSE );
			m_clOkCancelDialog.m_clOK.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_ONMOUSE );
			m_clOkCancelDialog.m_clCancel.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_ONMOUSE );
			m_clEditOKDialog.m_clOK.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_ONMOUSE );

			AgcmEventNPCDialog* pcmEventNPCDialog = ( AgcmEventNPCDialog* )GetModule( "AgcmEventNPCDialog" );
			if( pcmEventNPCDialog )
			{
				CExNPC* pExNPC = pcmEventNPCDialog->GetExNPC();
				if( pExNPC )
				{
					pExNPC->AddMsgBoxImage( szKeyValue, ACUIBUTTON_MODE_ONMOUSE );
				}
			}
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_MESSAGE_BUTTON2 ) )
		{
			m_clWaitingDialog.m_clClose.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_CLICK );
			m_clMessageDialog.m_clOK.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_CLICK );
			m_clOkCancelDialog.m_clOK.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_CLICK );
			m_clOkCancelDialog.m_clCancel.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_CLICK );
			m_clEditOKDialog.m_clOK.AddButtonImage( szKeyValue, ACUIBUTTON_MODE_CLICK );

			AgcmEventNPCDialog* pcmEventNPCDialog = ( AgcmEventNPCDialog* )GetModule( "AgcmEventNPCDialog" );
			if( pcmEventNPCDialog )
			{
				CExNPC* pExNPC = pcmEventNPCDialog->GetExNPC();
				if( pExNPC )
				{
					pExNPC->AddMsgBoxImage( szKeyValue, ACUIBUTTON_MODE_CLICK );
				}
			}
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CONVERT_BASE ) )
		{
			m_pConvertBase = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pConvertBase );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CONVERT_DISABLE ) )
		{
			m_pConvertDisable = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pConvertDisable );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CONVERT_OUTLINE ) )
		{
			m_pConvertOutline = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pConvertOutline );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_PVP_FREE ) )
		{
			m_pPvPFree = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pPvPFree );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_PVP_DANGER ) )
		{
			m_pPvPDanger = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pPvPDanger );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CANCEL_TRANSFORM ) )
		{
			m_pCancelTransform = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pCancelTransform );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CANCEL_EVOLUTION ) )
		{
			m_pCancelEvolution = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pCancelEvolution );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_DURABILITY_ZERO ) )
		{
			m_pDurabilityZero = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pDurabilityZero );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CATAPULT_USE ) )
		{
			m_pCatapultUse = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pCatapultUse );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CATAPULT_REPAIR ) )
		{
			m_pCatapultRepair = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pCatapultRepair );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_ATKRESTOWER_USE ) )
		{
			m_pAtkResTowerUse = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pAtkResTowerUse );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_ATKRESTOWER_REPAIR ) )
		{
			m_pAtkResTowerRepair = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pAtkResTowerRepair );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CASTLE_OWNER_HUMAN ) )
		{
			m_pCastleOwnerHuman	= RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pCastleOwnerHuman );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CASTLE_OWNER_ORC ) )
		{
			m_pCastleOwnerOrc = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pCastleOwnerOrc );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CASTLE_OWNER_MOONELF ) )
		{
			m_pCastleOwnerMoonelf = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pCastleOwnerMoonelf );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CASTLE_OWNER_DRAGONSCION ) )
		{
			m_pCastleOwnerDragonScion = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pCastleOwnerDragonScion );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_CASTLE_OWNER_ARCHLORD ) )
		{
			m_pCastleOwnerArchlord = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pCastleOwnerArchlord );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_ARCHLORD_GUILD_MARK ) )
		{
			m_pArchlordGuildMark = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pArchlordGuildMark );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_GO_MARK ) )
		{
			m_pGoMark = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pGoMark );
		}
		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_PET_STATE ) )
		{
			m_pPetState = RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pPetState );
		}

		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_HANGAME_S ) )
		{
			m_pTextureHangameS	=	RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pTextureHangameS );
		}

		else if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_HANGAME_T ) )
		{
			m_pTextureHangameT	=	RwTextureRead( szKeyValue, NULL );
			ASSERT( NULL != m_pTextureHangameT );
		}

		else
		{
			AgcdUICustomTexture	stCTexture;
			_tcsncpy( stCTexture.szName, szKeyName, AGCMUICONTROL_TEXTURE_LENGTH - 1 );
			_tcsncpy( stCTexture.szTextureName, szKeyValue, AGCMUICONTROL_TEXTURE_LENGTH - 1 );
			m_vCustomTexture.push_back( stCTexture );
		}
	}
}

void AgcmUIControl::_LoadSoundFromINI( AuIniManagerA* pINIManager )
{
	if( !pINIManager ) return;
	
	// [Sound] Section 검색 - 두번째 Section이 Sound
	INT32 lNumKey_Section1 = pINIManager->GetNumKeys( 1 );	//두번째 Section의 Key Num
	
	for ( INT32 nKeyCount = 0 ; nKeyCount < lNumKey_Section1 ; ++nKeyCount )
	{
		const CHAR* szKeyName =	pINIManager->GetKeyName( 1, nKeyCount );
		CHAR* szKeyValue = pINIManager->GetValue( 1, nKeyCount );

		if( !strcmp( szKeyName, AGCMUICONTROL_KEY_NAME_BUTTON_CLICK_SOUND ) )
		{
			strncpy( m_szButtonClickSound, szKeyValue, AGCMSOUND_SOUND_FILENAME_LENGTH );
			AcUIButton::m_pszButtonClickSound = m_szButtonClickSound;
		}
	}
}
