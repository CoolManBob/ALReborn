// AgcmUIManager2.cpp: implementation of the AgcmUIManager2 class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmUIManager2.h"

#include "ApMemoryTracker.h"

#include "AgcmCamera2.h"

#include "AgcmUIEventNPCDialog.h"
#include "AgcmUIConsole.h"


const UINT32 g_ulNotificationMinDelay = 7000;
const FLOAT fProcessingTime = 300.0f;

AgcdUIMode AgcmUIManager2::m_eMode = AGCDUI_MODE_1024_768;

AgcmUICursor::AgcmUICursor( void )
{
	m_lCursor			= 0;
	m_lCursorCurrent	= 0;

	m_hCursorHandle = NULL;
	memset( m_apstCursorInfo, 0, sizeof( AgcdUICursor* ) * AGCMUIMANAGER2_MAX_CURSOR );

	m_pcsLockCursorModule = NULL;
	m_bShowCursor = TRUE;
}

AgcmUICursor::~AgcmUICursor( void )
{
}

INT32 AgcmUICursor::AddCursor( HCURSOR hCursor , INT32 lSpeed )
{
	if( !hCursor || m_lCursor >= AGCMUIMANAGER2_MAX_CURSOR ) return -1;

	if( m_apstCursorInfo[ m_lCursor ] )
	{
		delete m_apstCursorInfo[ m_lCursor ];
		m_apstCursorInfo[ m_lCursor ] = NULL;
	}

	m_apstCursorInfo[ m_lCursor ] = new AgcdUICursor;
	m_apstCursorInfo[ m_lCursor ]->m_hCursor[ 0 ] = hCursor;
	m_apstCursorInfo[ m_lCursor ]->m_lSpeed = lSpeed;
	m_apstCursorInfo[ m_lCursor ]->m_lTotalFrame = 1;
	m_apstCursorInfo[ m_lCursor ]->m_lCurrentFrame = 0;
	m_apstCursorInfo[ m_lCursor ]->m_lTick = 0;
	
	++m_lCursor;
	return m_lCursor - 1;
}

BOOL AgcmUICursor::AddCursorFrame( INT32 lCursorID , HCURSOR hCursor , INT32 lFrameIndex )
{
	if( !hCursor || lCursorID < 0 || lCursorID >= m_lCursor || lFrameIndex <= 0 || lFrameIndex >= AGCDUIMANAGER2_MAX_CURSOR_FRAME )	return FALSE;
	if( !m_apstCursorInfo[ lCursorID ] ) return FALSE;

	m_apstCursorInfo[ lCursorID ]->m_hCursor[ lFrameIndex ] = hCursor;
	if( lFrameIndex > m_apstCursorInfo[ lCursorID ]->m_lTotalFrame )
	{
		m_apstCursorInfo[ lCursorID ]->m_lTotalFrame = lFrameIndex;
	}

	return TRUE;
}

VOID AgcmUICursor::RemoveAllCursor( void )
{
	while( m_lCursor > 0 )
	{
		--m_lCursor;
		if( m_apstCursorInfo[ m_lCursor ] )
		{
			delete m_apstCursorInfo[ m_lCursor ];
			m_apstCursorInfo[ m_lCursor ] = NULL;
		}
	}
}

VOID AgcmUICursor::UpdateCursor( UINT32 nPrevTick, UINT32 nClockCount )
{
	if( m_apstCursorInfo[ m_lCursorCurrent ] )
	{
		if( nPrevTick != 0 )
		{
			m_apstCursorInfo[ m_lCursorCurrent ]->m_lTick += nClockCount - nPrevTick;
		}

		if( m_apstCursorInfo[ m_lCursorCurrent ]->m_lTick >= m_apstCursorInfo[ m_lCursorCurrent ]->m_lSpeed )
		{
			m_apstCursorInfo[ m_lCursorCurrent ]->m_lTick -= m_apstCursorInfo[ m_lCursorCurrent ]->m_lSpeed;
			++m_apstCursorInfo[ m_lCursorCurrent ]->m_lCurrentFrame;

			if( m_apstCursorInfo[ m_lCursorCurrent ]->m_lCurrentFrame >= m_apstCursorInfo[ m_lCursorCurrent ]->m_lTotalFrame )
			{
				m_apstCursorInfo[ m_lCursorCurrent ]->m_lCurrentFrame = 0;
			}
		}
	}
}

AgcdUICursor* AgcmUICursor::GetCursor( INT32 lCursorID )
{
	if( lCursorID < 0 || lCursorID >= m_lCursor ) return NULL;
	return m_apstCursorInfo[ lCursorID ];
}

INT32 AgcmUICursor::GetCurrentCursor( void )
{
	return m_lCursorCurrent;
}

BOOL AgcmUICursor::ChangeCursor( INT32 lCursorID, ApModule *pcsChangeModule )
{
	if( lCursorID < 0 || lCursorID >= m_lCursor || !m_apstCursorInfo[ lCursorID ] || !pcsChangeModule )	return FALSE;
	if( m_pcsLockCursorModule && m_pcsLockCursorModule != pcsChangeModule ) return FALSE;

	m_lCursorCurrent = lCursorID;
	m_hCursorHandle = m_apstCursorInfo[ m_lCursorCurrent ]->m_hCursor[ m_apstCursorInfo[ m_lCursorCurrent ]->m_lCurrentFrame ];

	if( m_hCursorHandle && m_bShowCursor )
	{
		::SetCursor( m_hCursorHandle );
	}
	else
	{
		::SetCursor( NULL );
	}

	return TRUE;
}

VOID AgcmUICursor::ShowCursor( BOOL bShow )
{
	m_bShowCursor = bShow;
	::ShowCursor( m_bShowCursor );

	if( m_bShowCursor )
	{
		::SetCursor( GetCurrentCursorHandle() );
	}
	else
	{
		::SetCursor( NULL );
	}
}

BOOL AgcmUICursor::LockCursor( ApModule *pcsLockModule )
{
	if( m_pcsLockCursorModule && m_pcsLockCursorModule != pcsLockModule ) return FALSE;
	m_pcsLockCursorModule = pcsLockModule;
	return TRUE;
}

BOOL AgcmUICursor::UnLockCursor( ApModule *pcsLockModule )
{
	if( !pcsLockModule || m_pcsLockCursorModule != pcsLockModule ) return FALSE;
	m_pcsLockCursorModule	= NULL;
	return TRUE;
}






















//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

INT32 AgcmUIManager2::m_alControlMessages[ AcUIBase::TYPE_COUNT ] =
{
	UICM_BASE_MAX_MESSAGE,
	UICM_BASE_MAX_MESSAGE,
	UICM_EDIT_MAX_MESSAGE,
	UICM_BUTTON_MAX_MESSAGE,
	UICM_CHECKBOX_MAX_MESSAGE,
	UICM_COMBO_MAX_MESSAGE,
	0,										//	UICM_NUMBER_MAX_MESSAGE 매크로 제거
	UICM_TOOLTIP_MAX_MESSAGE,
	UICM_GRID_MAX_MESSAGE,
	UICM_BAR_MAX_MESSAGE,
	UICM_LIST_MAX_MESSAGE,
	UICM_LISTITEM_MAX_MESSAGE,
	UICM_SKILLTREE_MAX_MESSAGE,
	UICM_GRIDITEM_MAX_MESSAGE,
	UICM_SCROLL_MAX_MESSAGE,
	UICM_SCROLLBUTTON_MAX_MESSAGE,
	UICM_BASE_MAX_MESSAGE,
	UICM_LIST_MAX_MESSAGE,
	UICM_LISTITEM_MAX_MESSAGE,
	UICM_CLOCK_MAX_MESSAGE,
};

#pragma warning( push )
#pragma warning( disable : 4355 )

AgcmUIManager2::AgcmUIManager2( void )
:	m_csEffectWindow( this ), m_csEffectBGMWindow( this ), m_csEffectGuildBattleWindow1( this ),
	m_csEffectGuildBattleWindow2( this ), m_csEffectGuildBattleWindow3( this ), m_csEffectGuildBattleWindow4( this ),
	m_csEffectGuildBattleWindow5( this ), m_csEffectGuildBattleWindow6( this ),	m_csEffectGuildBattleWindow7( this ),
	m_nEventAccessorySound(0), m_nEventEatSound(0), m_nEventToggleUIOpen(0), m_nEventToggleUIClose(0), m_nEventTabUIOpen(0),
	m_nEventClickSound(0), m_nEventWeaponEquipSound(0), m_nEventArmourEquipSound(0), m_nEventTextSound(0), 
	m_nEventServerSelectMouseOverSound(0), m_nEventDeadSound(0), m_nEventCriticalSound(0), m_nEventMissSound(0), m_nEventBlockSound(0)
{
	SetModuleName( "AgcmUIManager2" );
	m_csManagerWindow.SetManager( this );

	SetModuleData( sizeof( AgcdUI ), AGCMUIMANAGER2_DATA_TYPE_UI );
	SetModuleData( sizeof( AgcdUIControl ), AGCMUIMANAGER2_DATA_TYPE_CONTROL );

	EnableIdle( TRUE );

	m_lMaxUIID = 0;
	m_lMaxUIWindows		= AGCMUIMANAGER2_MAX_UI_WINDOWS;
	m_lMaxUIFunctions	= AGCMUIMANAGER2_MAX_UI_FUNCTIONS;
	m_lMaxUIUserData	= AGCDUIMANAGER2_MAX_UI_USER_DATA;
	m_lMaxUIDisplays	= AGCDUIMANAGER2_MAX_UI_DISPLAYS;
	m_lMaxUIBooleans	= AGCDUIMANAGER2_MAX_UI_BOOLEANS;
	m_lMaxUIEvents		= AGCDUIMANAGER2_MAX_UI_EVENTS;
	m_lMaxUICControls	= AGCDUIMANAGER2_MAX_UI_CUSTOM_CONTROL;

	m_lMaxUIFunctionID	= 0;
	m_lMaxUIUserDataID	= 0;
	m_lMaxUIDisplayID	= 0;
	m_lMaxUIBooleanID	= 0;
	m_lMaxUIEventID		= 0;
	m_lMaxUICControlID	= 0;
	m_lRefreshUserData	= 0;
	m_ulPrevTick = 0;

	memset( m_apstRefreshUserData, 0, sizeof( AgcdUIUserData* ) * AGCMUIMANAGER2_MAX_REFRESH_USER_DATA );
	memset( m_apstHotkeyTable, 0, sizeof( AgcdUIHotkey* ) * AGCDUI_MAX_HOTKEY_TYPE * 256 );
	memset( m_vtBoxFan, 0, sizeof( My2DVertex ) * 4 );

	m_fRecipZ = 0;

	m_pstCamera = NULL;
	m_pcsTarget = NULL;
	m_pcsAgcmFont = NULL;
	m_pcsAgcmRender = NULL;

	memset( &m_v2dCurMousePos, 0, sizeof( RwV2d ) );

	m_lWindowWidth = 0;
	m_lWindowHeight = 0;

	m_bOnCameraSizing = FALSE;
	m_bUseAbsoluteOnly = FALSE;

	m_pcsMainWindow = NULL;
	m_szSoundPath[ 0 ] = 0;
	m_eMode = AGCDUI_MODE_1024_768;

	ZeroMemory( m_szLineDelimiter, sizeof( CHAR ) * AGCMUIMANAGER2_MAX_LINE_DELIMITER );
	m_aszMessageData	= NULL;
	m_pcsMouseOnUI		= NULL;
	m_pcsMouseOnUIPrev	= NULL;

	m_bBeingDestroyed	= FALSE;
	m_bSmoothUI			= TRUE;
	m_bCameraControl	= TRUE;
	m_bUIFocusProcessed	= FALSE;

	m_szTexturePath[ 0 ] = 0;
	m_pcsUIOpenedPopup = NULL;
}

AgcmUIManager2::~AgcmUIManager2( void )
{
	RemoveAllUI();
	RemoveAllFunction();
	RemoveAllUserData();
	RemoveAllDisplay();
	RemoveAllBoolean();
	RemoveAllEvent();
	RemoveAllHotkey();
	RemoveAllCControl();

	DestroyUI( m_pcsUIDummy );
	m_csManagerWindow.DeleteChild( &m_csTooltip, TRUE, FALSE );
	m_csManagerWindow.WindowListUpdate();

	if( m_pcsMainWindow )
	{
		m_pcsMainWindow->WindowListUpdate();

		m_pcsMainWindow->DeleteChild( &m_csManagerWindow,				TRUE, FALSE );
		m_pcsMainWindow->DeleteChild( &m_csEffectWindow,				TRUE, FALSE );
		m_pcsMainWindow->DeleteChild( &m_csEffectBGMWindow,				TRUE, FALSE );
		m_pcsMainWindow->DeleteChild( &m_csEffectGuildBattleWindow1, 	TRUE, FALSE );
		m_pcsMainWindow->DeleteChild( &m_csEffectGuildBattleWindow2, 	TRUE, FALSE );
		m_pcsMainWindow->DeleteChild( &m_csEffectGuildBattleWindow3, 	TRUE, FALSE );
		m_pcsMainWindow->DeleteChild( &m_csEffectGuildBattleWindow4, 	TRUE, FALSE );
		m_pcsMainWindow->DeleteChild( &m_csEffectGuildBattleWindow5, 	TRUE, FALSE );
		m_pcsMainWindow->DeleteChild( &m_csEffectGuildBattleWindow6, 	TRUE, FALSE );
		m_pcsMainWindow->DeleteChild( &m_csEffectGuildBattleWindow7, 	TRUE, FALSE );

		m_pcsMainWindow->WindowListUpdate();
	}

	if( m_aszMessageData )
	{
		delete[] m_aszMessageData;
		m_aszMessageData = NULL;
	}

	MapUIMessageIter	Iter		=	m_mapAdminUIMsg.begin();
	for( ; Iter != m_mapAdminUIMsg.end() ; ++Iter )
	{
		DEF_SAFEDELETE( Iter->second );
	}
	m_mapAdminUIMsg.clear();
}

#pragma warning( pop )

BOOL AgcmUIManager2::OnAddModule( void )
{
	m_pcsAgcmFont			= ( AgcmFont*			)GetModule( "AgcmFont" );
	m_pcsAgcmRender			= ( AgcmRender*			)GetModule( "AgcmRender" );
	m_pcsAgcmSound			= ( AgcmSound*			)GetModule( "AgcmSound" );
	m_pcsAgcmResourceLoader = ( AgcmResourceLoader* )GetModule( "AgcmResourceLoader" );
	m_pcsAgcmUIControl 		= ( AgcmUIControl*		)GetModule( "AgcmUIControl" );
	m_pcsAgcmCharacter 		= ( AgcmCharacter*		)GetModule( "AgcmCharacter" );

	AgpmCharacter* pcsAgpmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	if( !m_pcsAgcmFont || !m_pcsAgcmUIControl )	return FALSE;
	if( !AddStreamCallback( AGCMUIMANAGER2_DATA_TYPE_UI, StreamReadCB, StreamWriteCB, this ) ) return FALSE;

	pcsAgpmCharacter->SetCallbackUpdateActionStatus( CBUpdateActionState , this );

	m_csUIFunctions.InitializeObject(	sizeof( AgcdUIFunction*	), m_lMaxUIFunctions );
	m_csUIUserData.InitializeObject(	sizeof( AgcdUIUserData*	), m_lMaxUIUserData );
	m_csUIDisplays.InitializeObject(	sizeof( AgcdUIDisplay*	), m_lMaxUIDisplays );
	m_csUIBooleans.InitializeObject(	sizeof( AgcdUIBoolean*	), m_lMaxUIBooleans );
	m_csUIEvents.InitializeObject(		sizeof( AgcdUIEventMap*	), m_lMaxUIEvents );
	m_csUICControls.InitializeObject(	sizeof( AgcdUICControl*	), m_lMaxUICControls );

	// Get AgcmCamera Module Pointer
	AgcmCamera2* pAgcmCamera2 = ( AgcmCamera2* )GetModule( "AgcmCamera2" );
	m_csManagerWindow.SetAgcmCameraModule( pAgcmCamera2 );	
	m_csManagerWindow.SetAgcmCharacterModule( ( AgcmCharacter* )GetModule( "AgcmCharacter" ) );

	SetCamera( GetCamera() );
	if( m_pcsAgcmCharacter )
	{
		if( !m_pcsAgcmCharacter->SetCallbackIsCameraMoving( CBIsCameraMoving, this ) ) return FALSE;
	}

	return TRUE;
}

BOOL AgcmUIManager2::CBUpdateActionState( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIManager2* pThis = ( AgcmUIManager2* )pClass;
	AgpdCharacter* pcsAgpdCharacter	= ( AgpdCharacter* )pData;
	INT16* pnOldStatus = ( INT16* )pCustData;

	if( !pThis || !pcsAgpdCharacter)
		return FALSE;

	if( pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
	{
		//pThis->ThrowEvent( pThis->m_nEventDeadSound );
		AgcdCharacter * pcdChar = pThis->m_pcsAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
		if( pcdChar )
		{
			pThis->m_pcsAgcmSound->Play3DSound( "Sound\\UI\\UI_Dead_Effect_00.WAV", 1, 1.0f,
				TRUE, pcdChar->m_pClump, 1.0f, TRUE );
		}
	}

	if(	pThis->m_pcsAgcmCharacter && pThis->m_pcsAgcmCharacter->GetSelfCharacter() == pcsAgpdCharacter &&
		*pnOldStatus != AGPDCHAR_STATUS_DEAD && pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
	{
		// 케릭터가 죽을때
		// UI를 싸그리 닫는다.
		pThis->CloseAllUIExceptMainUI();
	}

	return TRUE;
}

BOOL AgcmUIManager2::OnInit( void )
{
	m_csUIWindows.InitializeObject( sizeof( AgcdUI* ), m_lMaxUIWindows );

	m_csTooltip.m_Property.bTopmost = TRUE;
	m_csManagerWindow.AddChild( &m_csNotification );
	m_csNotification.ShowWindow( FALSE );
	m_csTooltip.ShowWindow( FALSE );
	m_csTooltip.m_Property.bUseInput = FALSE;
	m_csTooltip.m_bDrawTooltipBox = FALSE;

	m_csManagerWindow.AddChild( &m_csTooltip );
	if( m_pcsMainWindow )
	{
		m_pcsMainWindow->AddChild( &m_csManagerWindow );
		m_pcsMainWindow->AddChild( &m_csEffectWindow );
		m_pcsMainWindow->AddChild( &m_csEffectBGMWindow );
		m_pcsMainWindow->AddChild( &m_csEffectGuildBattleWindow1 );
		m_pcsMainWindow->AddChild( &m_csEffectGuildBattleWindow2 );
		m_pcsMainWindow->AddChild( &m_csEffectGuildBattleWindow3 );
		m_pcsMainWindow->AddChild( &m_csEffectGuildBattleWindow4 );
		m_pcsMainWindow->AddChild( &m_csEffectGuildBattleWindow5 );
		m_pcsMainWindow->AddChild( &m_csEffectGuildBattleWindow6 );
		m_pcsMainWindow->AddChild( &m_csEffectGuildBattleWindow7 );
	}

	// Set Callback : Set Cursor Mouse Position 
	g_pEngine->SetCallbackSetCursorMousePosition( CBSetCursorMousePosition, this );

	m_pcsUIDummy = CreateUI();
	if( !m_pcsUIDummy ) return FALSE;

	AS_REGISTER_TYPE_BEGIN( AgcmUIManager2, AgcmUIManager2 );
		AS_REGISTER_METHOD0( void, ShowUIName );
		AS_REGISTER_METHOD1( void, UIClose, string &in );
		AS_REGISTER_VARIABLE( string, m_strMouseOnUI );
	AS_REGISTER_TYPE_END;

	m_nEventEatSound					= AddEvent("EatSoundEvent");
	m_nEventToggleUIOpen				= AddEvent( "ToggleOpenUISoundEvent" );
	m_nEventToggleUIClose				= AddEvent( "ToggleCloseUISoundEvent" );
	m_nEventTabUIOpen					= AddEvent( "TabUIOpenSoundEvent" );
	m_nEventClickSound					= AddEvent( "ClickSoundEvent" );
	m_nEventWeaponEquipSound			= AddEvent( "WeaponEquipSoundEvent" );
	m_nEventArmourEquipSound			= AddEvent( "ArmourEquipSoundEvent" );
	m_nEventAccessorySound				= AddEvent( "AccessorySoundEvent" );
	m_nEventTextSound					= AddEvent( "WriteSoundEvent" );
	m_nEventServerSelectMouseOverSound	= AddEvent( "ServerSelectMouseOverSoundEvent" );
	m_nEventDeadSound					= AddEvent( "DeadSoundEvent" );
	m_nEventCriticalSound				= AddEvent( "CriticalSoundEvent" );
	m_nEventMissSound					= AddEvent( "MissSoundEvent" );
	m_nEventBlockSound					= AddEvent( "BlockSoundEvent" );

	return TRUE;
}

//@{ Jaewon 20050928
#ifdef _DEBUG
#include <sstream>
#endif
//@} Jaewon

BOOL AgcmUIManager2::OnIdle( UINT32 ulClockCount )
{
	PROFILE( "AgcmUIManager2::OnIdle" );

	INT32 lIndex = 0;

	AuNode< AgcdUI* >* pNode = NULL;
	AuNode< AgcdUI* >* pNodeNext = NULL;
	AgcdUI*	pcsUI = NULL;

	UINT32 ulDeltaTime = 0;
	BOOL bRemoveUIFromProcessingList = FALSE;

	if( m_pcsMouseOnUIPrev != m_pcsMouseOnUI )
	{
		if( m_pcsMouseOnUIPrev && m_pcsMouseOnUIPrev->m_pstKillFocusFunction && m_pcsMouseOnUIPrev->m_pstKillFocusFunction->m_fnCallback )
		{
			m_pcsMouseOnUIPrev->m_pstKillFocusFunction->m_fnCallback( m_pcsMouseOnUIPrev->m_pstKillFocusFunction->m_pClass, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
		}

		if( m_pcsMouseOnUI && m_pcsMouseOnUI->m_pstSetFocusFunction && m_pcsMouseOnUI->m_pstSetFocusFunction->m_fnCallback )
		{
			m_pcsMouseOnUI->m_pstSetFocusFunction->m_fnCallback( m_pcsMouseOnUI->m_pstSetFocusFunction->m_pClass, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
		}

		m_pcsMouseOnUIPrev = m_pcsMouseOnUI;
	}

	m_bUIFocusProcessed = TRUE;

	AuNode< AgcdUIActionBuffer* >* pstNode = m_listActionBuffers.GetHeadNode();
	AgcdUIActionBuffer* pstActionBuffer = NULL;

	while( pstNode )
	{
		pstActionBuffer = pstNode->GetData();
		m_listActionBuffers.RemoveNode( pstNode );

		ProcessAction( &pstActionBuffer->m_listActions, pstActionBuffer->m_pcsControl, pstActionBuffer->m_pcsEvent, pstActionBuffer->m_lDataIndex, TRUE, pstActionBuffer->m_pcsOwnerBase );
		delete pstActionBuffer;
		pstNode = m_listActionBuffers.GetHeadNode();
	}

	m_listActionBuffers.RemoveAll();
	for( lIndex = 0 ; lIndex < m_lRefreshUserData ; ++lIndex )
	{
		if( m_apstRefreshUserData[ lIndex ] )
		{
			RefreshUserData( m_apstRefreshUserData[ lIndex ] );
		}
	}

	m_lRefreshUserData = 0;
	UpdateCursor( m_ulPrevTick, ulClockCount );

	if( m_ulNotificationCloseTime <= ulClockCount )
	{
		m_csNotification.ShowWindow( FALSE );
	}

	pNode = m_listUIProcessing.GetHeadNode();
	while( pNode )
	{
		bRemoveUIFromProcessingList = FALSE;

		pcsUI = pNode->GetData();
		pNodeNext = pNode->GetNextNode();

		ulDeltaTime = ulClockCount - pcsUI->m_ulProcessingStartTime;

		switch( pcsUI->m_eStatus )
		{
		case AGCDUI_STATUS_OPENING :
			{
				if( ulDeltaTime >= ( UINT32 )fProcessingTime )
				{
					pcsUI->m_fAlpha = 1.0f;
					pcsUI->m_eStatus = AGCDUI_STATUS_OPENED;
					bRemoveUIFromProcessingList = TRUE;
				}
				else
				{
					pcsUI->m_fAlpha = ( float )( ulDeltaTime / ( UINT32 )fProcessingTime );
				}
			}
			break;

		case AGCDUI_STATUS_CLOSING :
			{
				if( ulDeltaTime >= (UINT32) fProcessingTime )
				{
					pcsUI->m_fAlpha = 0.0f;
					pcsUI->m_eStatus = AGCDUI_STATUS_OPENED;
					bRemoveUIFromProcessingList = TRUE;

					CloseUI( pcsUI, FALSE, FALSE, TRUE, FALSE );
				}
				else
				{
					pcsUI->m_fAlpha = 1.0f - ( float ) ulDeltaTime / fProcessingTime;
				}
			}
			break;

		default :
			{
				bRemoveUIFromProcessingList = TRUE;
			}
			break;
		}

		if( bRemoveUIFromProcessingList )
		{
			m_listUIProcessing.RemoveNode( pNode );
		}

		pNode = pNodeNext;
	}

	//@{ Jaewon 20050928
	// These are codes for debugging the problem that some textures are not drawn in the login UI.
#ifdef _DEBUG
	/**
	static bool slayTheBug = false;
	if(slayTheBug)
	{
		INT32 lIndex = 0, lIndexControl, lIndexTexture;
		AgcdUI *pcsUI;
		AgcdUIControl *pcsControl;
		std::stringstream output;
		CHAR name[64];
		name[0] = '\0';

		for(pcsUI = GetSequenceUI(&lIndex); pcsUI; pcsUI = GetSequenceUI(&lIndex))
		{
			output.clear();
			output << "UI " << pcsUI->m_fAlpha << " " << pcsUI->m_eStatus << std::endl;
			OutputDebugString(output.str().c_str());

			lIndexControl = 0;
			for(pcsControl = GetSequenceControl(pcsUI, &lIndexControl); pcsControl; pcsControl = GetSequenceControl(pcsUI, &lIndexControl))
			{
				output.clear();
				output << "CONTROL " << pcsControl->m_lType << " " << pcsControl->m_szName << std::endl;
				OutputDebugString(output.str().c_str());
				
				for(lIndexTexture = 0; lIndexTexture < pcsControl->m_pcsBase->m_csTextureList.GetCount(); ++lIndexTexture)
				{
					INT32 lImageID;
					CHAR *szTextureName = pcsControl->m_pcsBase->m_csTextureList.GetImageName_Index(lIndexTexture, &lImageID);
					RwTexture *pTexture = pcsControl->m_pcsBase->m_csTextureList.GetImage_Index(lIndexTexture);

					output.clear();
					output << "TEXTURE " << szTextureName << " " << lImageID << " " <<  pTexture << std::endl;
					OutputDebugString(output.str().c_str());

					if(!szTextureName || !strcmp(name, szTextureName))
						ASSERT(0);
				}
			}
			
		}
	}
	*/
#endif
	//@} Jaewon

	m_ulPrevTick = ulClockCount;
	return TRUE;
}

BOOL AgcmUIManager2::OnDestroy( void )
{
	AuNode< AgcdUIActionBuffer* >* pstNode = m_listActionBuffers.GetHeadNode();
	AgcdUIActionBuffer* pstActionBuffer = NULL;

	m_bBeingDestroyed = TRUE;
	while( pstNode )
	{
		pstActionBuffer = pstNode->GetData();
		m_listActionBuffers.RemoveNode( pstNode );

		delete pstActionBuffer;
		pstNode = m_listActionBuffers.GetHeadNode();
	}

	m_listActionBuffers.RemoveAll();
	m_csManagerWindow.DeleteChild( &m_csNotification );

	RemoveAllCursor();

	AcUIBase::m_pAgcmFont = NULL;
	AcUIBase::m_pAgcmSound = NULL;
	return TRUE;
}

INT16 AgcmUIManager2::AttachUIData( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor )
{
	return SetAttachedModuleData( pClass, AGCMUIMANAGER2_DATA_TYPE_UI, nDataSize, pfConstructor, pfDestructor );
}

INT16 AgcmUIManager2::AttachControlData( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor )
{
	return SetAttachedModuleData( pClass, AGCMUIMANAGER2_DATA_TYPE_CONTROL, nDataSize, pfConstructor, pfDestructor );
}

VOID AgcmUIManager2::SetMaxUI( INT32 lMaxCount )
{
	m_lMaxUIWindows = lMaxCount;
}

BOOL AgcmUIManager2::SetMaxUIMessage( INT32 lCount )
{
	return TRUE;
}

VOID AgcmUIManager2::SetMainWindow( AgcWindow *pcsUI )
{
	m_pcsMainWindow = pcsUI;
	EnumCallback( AGCMUIMANAGER2_CB_ID_SET_MAIN_WINDOW, NULL, NULL );
}

VOID AgcmUIManager2::ResizeManagerWindow( INT32 x, INT32 y, INT32 w, INT32 h )
{
	m_csManagerWindow.MoveWindow( x, y, w, h );
	m_csEffectWindow.MoveWindow( x, y, w, h );
	m_csEffectBGMWindow.MoveWindow( x, y, w, h );
	m_csEffectGuildBattleWindow1.MoveWindow( x, y, w, h );
	m_csEffectGuildBattleWindow2.MoveWindow( x, y, w, h );
	m_csEffectGuildBattleWindow3.MoveWindow( x, y, w, h );
	m_csEffectGuildBattleWindow4.MoveWindow( x, y, w, h );
	m_csEffectGuildBattleWindow5.MoveWindow( x, y, w, h );
	m_csEffectGuildBattleWindow6.MoveWindow( x, y, w, h );
	m_csEffectGuildBattleWindow7.MoveWindow( x, y, w, h );
}

VOID AgcmUIManager2::SetMaxMaps( INT32 lFunction, INT32 lUserData, INT32 lDisplay, INT32 lEvents, INT32 lBooleans, INT32 lCControls )
{
	m_lMaxUIFunctions = lFunction;
	m_lMaxUIUserData = lUserData;
	m_lMaxUIDisplays = lDisplay;
	m_lMaxUIEvents = lEvents;
	m_lMaxUIBooleans = lBooleans;
	m_lMaxUICControls = lCControls;
}

BOOL AgcmUIManager2::MakeValidName( CHAR *szName )
{
	if( !szName ) return FALSE;

	INT32 lIndex;
	INT32 lSize = strlen( szName );

	for( lIndex = 0 ; lIndex < lSize ; ++lIndex )
	{
		if( szName[ lIndex ] == ' ' || szName[ lIndex ] == '\t' || szName[ lIndex ] == '\r' || szName[ lIndex ] == '\n' )
		{
			szName[ lIndex ] = '_';
		}
	}

	return TRUE;
}

AgcdUI* AgcmUIManager2::CreateUI( void )
{
	AgcdUI* pcsUI = ( AgcdUI* )CreateModuleData( AGCMUIMANAGER2_DATA_TYPE_UI );
	if( !pcsUI ) return NULL;

	memset( pcsUI->m_alX, 0, sizeof( INT32 ) * AGCDUI_MAX_MODE );
	memset( pcsUI->m_alY, 0, sizeof( INT32 ) * AGCDUI_MAX_MODE );

	pcsUI->m_ulModeFlag				= 0;
	pcsUI->m_szParentUIName[ 0 ]	= 0;
	pcsUI->m_bUseParentPosition		= FALSE;
	pcsUI->m_pcsCurrentChildUI		= NULL;
	pcsUI->m_bMainUI				= FALSE;
	pcsUI->m_bEventUI				= FALSE;
	pcsUI->m_bTransparent			= FALSE;
	pcsUI->m_fAlpha					= 1.0f;
	pcsUI->m_ulProcessingStartTime	= 0;
	pcsUI->m_pcsUIAttach			= NULL;
	pcsUI->m_eAttachType			= AGCDUI_ATTACH_TYPE_LEFT;
	pcsUI->m_eAttachTypeToolTip		= AGCDUI_ATTACH_TYPE_LEFT;
	pcsUI->m_pcsTooltipAttach		= NULL;
	pcsUI->m_pstSetFocusFunction	= NULL;
	pcsUI->m_pstKillFocusFunction	= NULL;
	pcsUI->m_eType					= APBASE_TYPE_UI;
	pcsUI->m_eStatus				= AGCDUI_STATUS_NONE;
	pcsUI->m_eCoordSystem			= AGCDUI_COORDSYSTEM_ABSOLUTE;

	pcsUI->m_pcsUIWindow = new AgcUIWindow( this, pcsUI );
	if( !pcsUI->m_pcsUIWindow )	return NULL;

	pcsUI->m_lID = ++m_lMaxUIID;
	pcsUI->m_pcsUIWindow->m_pfAlpha = &pcsUI->m_fAlpha;
	pcsUI->m_pcsUIWindow->MoveWindow( 0, 0, 300, 300 );
	pcsUI->m_lMaxChildID = 0;
	pcsUI->m_ulModeFlag = 0xffffffff;
	pcsUI->m_bAutoClose	= TRUE;

	return pcsUI;
}

BOOL AgcmUIManager2::DestroyUI( AgcdUI *pcsUI )
{
	// 일단, Leak을 잡기 위해서 열라 지저분하게 코딩함... -_-; 나중에 시간나면 고치자... (Parn, 04/10/19 15:17)
	// 과연 시간이 날까?
	INT32 lIndex = 0;
	AgcdUIControl* pcsControl = NULL;
	AgcdUI* pcsUIChild;

	ASSERT( pcsUI );
	if( !pcsUI ) return FALSE;

	TRACE( "Destroy UI : %s (%x)\n", pcsUI->m_szUIName, pcsUI );

	if( m_pcsMouseOnUIPrev == pcsUI )
	{
		m_pcsMouseOnUIPrev = NULL;
	}

	// List를 먼저 처리한다. Dynamic Control들은 여기서 제거되어야 함.
	for( pcsControl = GetSequenceControl( pcsUI, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex ) )
	{
		if( pcsControl->m_lType == AcUIBase::TYPE_LIST || pcsControl->m_lType == AcUIBase::TYPE_TREE )
		{
			// lIndex 를 0 으로 초기화 함으로서 컨트롤을 중복검사하는 일이 생긴다.. 왜 이렇게 되어 있을까..
			// 순차증가하는 인덱스이니.. 걍 1 빼주면 될거 같은데.. 흠..
			RemoveControl( pcsControl );
			lIndex = 0;
		}
	}

	lIndex = 0;
	while( ( pcsControl = GetSequenceControl( pcsUI, &lIndex ) ) )
	{
		// 여기도 그러네.. 뭐 확실하게 지울수는 있겠지만서도;;
		RemoveControl( pcsControl );
		lIndex = 0;
	}

	// 흐음.. 위의 두 코드를 보자면.. 리스트나 트리타입의 컨트롤을 먼저 지우더니..
	// 아래 내려와서는 몽땅 다 지운다.. 이유가 뭐지?;; 그냥 첨부터 몽땅 다 지우지 않는 이유는?

	lIndex = 0;
	for( pcsUIChild = GetSequenceUI( &lIndex ) ; pcsUIChild ; pcsUIChild = GetSequenceUI( &lIndex ) )
	{
		if( pcsUIChild->m_pcsParentUI == pcsUI )
		{
			pcsUIChild->m_pcsParentUI = NULL;
		}
	}

	if( pcsUI->m_pcsUIWindow )
	{
		pcsUI->m_pcsUIWindow->UpdateChildWindow();
	}

	m_csManagerWindow.UpdateChildWindow();

	if( !m_bBeingDestroyed )
	{
		RemoveFromActions( pcsUI, NULL );
	}

	switch( pcsUI->m_eStatus )
	{
	case AGCDUI_STATUS_ADDED :			RemoveUI( pcsUI );								break;
	case AGCDUI_STATUS_NONE :
		{
			RemoveUI( pcsUI );
			m_csManagerWindow.DeleteChild( pcsUI->m_pcsUIWindow );

			if( pcsUI->m_pcsUIWindow )
			{
				delete pcsUI->m_pcsUIWindow;
				pcsUI->m_pcsUIWindow = NULL;
			}
		}
		break;

	case AGCDUI_STATUS_OPENED	:
		{
			CloseUI( pcsUI, TRUE, FALSE, FALSE, FALSE );
			RemoveUI( pcsUI );
		}
		break;
	}

	m_csManagerWindow.UpdateChildWindow();
	DestroyModuleData( pcsUI, AGCMUIMANAGER2_DATA_TYPE_UI );
	return TRUE;
}

AgcdUI* AgcmUIManager2::AddUI( AgcdUI *pcsUI )
{
	if( pcsUI->m_eStatus == AGCDUI_STATUS_ADDED || pcsUI->m_eStatus == AGCDUI_STATUS_OPENED ) return NULL;
	if( !MakeValidName( pcsUI->m_szUIName ) ) return NULL;

	TRACE( "Added UI : %s (%x,%x)\n", pcsUI->m_szUIName, pcsUI, pcsUI->m_pcsUIWindow );

	if( m_csUIWindows.AddUI( pcsUI ) )
	{
		pcsUI->m_eStatus = AGCDUI_STATUS_ADDED;
		return pcsUI;
	}

	return NULL;
}

AgcdUI* AgcmUIManager2::GetUI( CHAR *szName )
{
	return m_csUIWindows.GetUI( szName );
}

AgcdUI* AgcmUIManager2::GetUI( INT32 lID )
{
	return m_csUIWindows.GetUI( lID );
}

BOOL AgcmUIManager2::RemoveUI( AgcdUI *pcsUI )
{
	if( !pcsUI ) return FALSE;
	if( pcsUI->m_eStatus == AGCDUI_STATUS_OPENED )
	{
		CloseUI( pcsUI, m_bBeingDestroyed, FALSE, FALSE, FALSE );
	}

	if( m_csUIWindows.RemoveUI( pcsUI->m_lID, pcsUI->m_szUIName ) )
	{
		pcsUI->m_eStatus = AGCDUI_STATUS_NONE;
		if( m_pcsMouseOnUIPrev == pcsUI )
		{
			m_pcsMouseOnUIPrev = NULL;
		}
		return TRUE;
	}

	return FALSE;
}

VOID AgcmUIManager2::RemoveAllUI( void )
{
	INT32 lIndex = 0;
	AgcdUI*	pcsUI;

	for( pcsUI = GetSequenceUI( &lIndex ) ; pcsUI ; pcsUI = GetSequenceUI( &lIndex ) )
	{
		DestroyUI( pcsUI );
	}

	m_csUIWindows.RemoveObjectAll();
}

BOOL AgcmUIManager2::ChangeUIName( AgcdUI *pcsUI, CHAR *szName )
{
	if( !m_csUIWindows.UpdateKey( pcsUI->m_lID, szName ) ) return FALSE;
	
	strncpy( pcsUI->m_szUIName, szName, AGCDUIMANAGER2_MAX_NAME );
	pcsUI->m_szUIName[ AGCDUIMANAGER2_MAX_NAME - 1 ] = '\0';

	return TRUE;
}

AgcdUI* AgcmUIManager2::GetSequenceUI( INT32 *plIndex )
{
	AgcdUI** ppcsUI = ( AgcdUI** )m_csUIWindows.GetObjectSequence( plIndex );
	if( !ppcsUI ) return NULL;
	return *ppcsUI;
}

BOOL AgcmUIManager2::OpenUI( AgcdUI *pcsUI, BOOL bGroup, BOOL bEditMode, BOOL bSmooth )
{
	if( !pcsUI ) return FALSE;
	if( !bEditMode && !( pcsUI->m_ulModeFlag & ( 1 << m_eMode ) ) )	return TRUE;
	if( pcsUI->m_pcsUIWindow->m_bOpened ) return TRUE;

	TRACE( "\n!!!!!Open UI(%s) Start!!!!!!\n\n", pcsUI->m_szUIName );

	if( bGroup )
	{
		AgcdUI* pcsUITemp = pcsUI;
		while( pcsUITemp->m_pcsParentUI )
		{
			pcsUITemp = pcsUITemp->m_pcsParentUI;
		}

		while( pcsUITemp->m_pcsCurrentChildUI )
		{
			pcsUITemp = pcsUITemp->m_pcsCurrentChildUI;
		}

		return OpenUI( pcsUITemp, FALSE, bEditMode, bSmooth );
	}

	if( pcsUI->m_eStatus == AGCDUI_STATUS_CLOSING )
	{
		UINT32	ulClockCount = GetClockCount();

		pcsUI->m_ulProcessingStartTime = ulClockCount - ( ( UINT32 )fProcessingTime - ( ulClockCount - pcsUI->m_ulProcessingStartTime ) );
		pcsUI->m_eStatus = AGCDUI_STATUS_OPENING;

		{
			// 닫히는 중에 다시 열리면 포커스 주기.
			INT32 lIndex;
			AgcdUIControl* pcsControl = NULL;
			BOOL bFirstEditControl = TRUE;

			lIndex = 0;
			for( pcsControl = GetSequenceControl( pcsUI, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex ) )
			{
				if( pcsControl->m_pcsBase->m_nType == AcUIBase::TYPE_EDIT )
				{
					AcUIEdit* pcsEdit = ( AcUIEdit* )pcsControl->m_pcsBase;
					if( bFirstEditControl && !pcsEdit->m_bReadOnly )
					{
						// 마고자 (2006-05-12 오후 6:22:52) : 
						// 최초의 에디트 컨트롤에 포커스를 넣음.
						bFirstEditControl = FALSE;
						pcsEdit->SetMeActiveEdit();
					}
				}
			}
		}

		return TRUE;
	}

	if( pcsUI->m_eStatus != AGCDUI_STATUS_ADDED ) return FALSE;

	ASSERT( pcsUI && pcsUI->m_pcsUIWindow );
	ASSERT( pcsUI->m_lID );

	if( !pcsUI || !pcsUI->m_pcsUIWindow ) return FALSE;

	// 이젠 Coordination System에 따라 위치 조절
	if( pcsUI->m_eCoordSystem == AGCDUI_COORDSYSTEM_CURSOR )
	{
		INT32 lX = ( INT32 )m_v2dCurMousePos.x;
		INT32 lY = ( INT32 )m_v2dCurMousePos.y;

		if( lX + pcsUI->m_pcsUIWindow->w > m_csManagerWindow.w )
		{
			lX = lX - pcsUI->m_pcsUIWindow->w;
		}

		if( lY + pcsUI->m_pcsUIWindow->h > m_csManagerWindow.h )
		{
			lY = lY - pcsUI->m_pcsUIWindow->h;
		}

		pcsUI->m_pcsUIWindow->MoveWindow( lX, lY, pcsUI->m_pcsUIWindow->w, pcsUI->m_pcsUIWindow->h );
	}

	m_csManagerWindow.AddChild( pcsUI->m_pcsUIWindow, pcsUI->m_lID, FALSE );
	m_csManagerWindow.UpdateChildWindow();

	pcsUI->m_pcsCurrentChildUI = NULL;
	if( pcsUI->m_pcsParentUI )
	{
		pcsUI->m_pcsParentUI->m_pcsCurrentChildUI = pcsUI;
		if( pcsUI->m_bUseParentPosition )
		{
			AgcdUI*	pcsParentUI = pcsUI;
			while( pcsParentUI->m_pcsParentUI )
			{
				pcsParentUI = pcsParentUI->m_pcsParentUI;
			}

			pcsUI->m_pcsUIWindow->MoveWindow( pcsParentUI->m_pcsUIWindow->x, pcsParentUI->m_pcsUIWindow->y, pcsUI->m_pcsUIWindow->w, pcsUI->m_pcsUIWindow->h );
		}
	}

	if( m_pcsAgcmResourceLoader )
	{
		m_pcsAgcmResourceLoader->SetTexturePath( m_szTexturePath );
	}

	// 동적으로 UI Image를 읽어들인다.
	pcsUI->m_pcsUIWindow->m_csTextureList.LoadTextures();

	INT32 lIndex = 0;
	AgcdUIControl* pcsControl = NULL;
	INT32 lCoordX = -1000;
	INT32 lCoordY = 0;
	BOOL bFirstEditControl = TRUE;

	for( pcsControl = GetSequenceControl( pcsUI, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex ) )
	{
		pcsControl->m_pcsBase->m_csTextureList.LoadTextures();
		if( lCoordX == -1000 && pcsUI->m_eUIType != AGCDUI_TYPE_NORMAL && pcsControl->m_bAutoAlign )
		{
			lCoordX = pcsControl->m_pcsBase->x;
			lCoordY = pcsControl->m_pcsBase->y;
		}

		if( pcsUI->m_bInitControlStatus )
		{
			pcsControl->m_pcsBase->SetStatus( pcsControl->m_pcsBase->GetDefaultStatus(), FALSE );
			if( pcsControl->m_lType == AcUIBase::TYPE_BUTTON )
			{
				AcUIButton* pcsButton = ( AcUIButton* )pcsControl->m_pcsBase;

				if( pcsButton && pcsButton->m_stProperty.m_bStartOnClickStatus )
				{
					pcsButton->SetButtonMode( ACUIBUTTON_MODE_CLICK );
				}
			}
			else if( pcsControl->m_lType == AcUIBase::TYPE_EDIT )
			{
				AcUIEdit* pcsEdit = ( AcUIEdit* )pcsControl->m_pcsBase;
				pcsEdit->ClearText();
			}
		}

		if( pcsControl->m_pcsBase->m_nType == AcUIBase::TYPE_EDIT )
		{
			AcUIEdit* pcsEdit = ( AcUIEdit* )pcsControl->m_pcsBase;
			if( bFirstEditControl && !pcsEdit->m_bReadOnly )
			{
				// 마고자 (2006-05-12 오후 6:22:52) : 
				// 최초의 에디트 컨트롤에 포커스를 넣음.
				bFirstEditControl = FALSE;
				pcsEdit->SetMeActiveEdit();
			}
		}

		if( !pcsControl->m_bDynamic )
		{
			if( !bEditMode )
			{
				if( pcsControl->m_pstShowCB && pcsControl->m_pstShowCB->m_fnCallback )
				{
					if( !pcsControl->m_pstShowCB->m_fnCallback( pcsControl->m_pstShowCB->m_pvClass,	pcsControl->m_pstShowUD ? pcsControl->m_pstShowUD->m_stUserData.m_pvData : NULL,
																pcsControl->m_pstShowUD ? pcsControl->m_pstShowUD->m_eType : AGCDUI_USERDATA_TYPE_NONE,	pcsControl ) )
					{
						pcsControl->m_pcsBase->m_Property.bVisible = FALSE;
					}
					else
					{
						pcsControl->m_pcsBase->m_Property.bVisible = TRUE;
					}
				}
			}
			else
			{
				pcsControl->m_pcsBase->m_Property.bVisible = TRUE;
			}
		}

		if( !pcsControl->m_pcsParentControl && pcsControl->m_bAutoAlign && pcsControl->m_pcsBase->m_Property.bVisible )
		{
			if( pcsUI->m_eUIType == AGCDUI_TYPE_POPUP_V )
			{
				pcsControl->m_pcsBase->MoveWindow( lCoordX, lCoordY, pcsControl->m_pcsBase->w, pcsControl->m_pcsBase->h );
				lCoordY += pcsControl->m_pcsBase->h;
			}
			else if( pcsUI->m_eUIType == AGCDUI_TYPE_POPUP_H )
			{
				pcsControl->m_pcsBase->MoveWindow( lCoordX, lCoordY, pcsControl->m_pcsBase->w, pcsControl->m_pcsBase->h );
				lCoordX += pcsControl->m_pcsBase->w;
			}
		}
	}

	if( !m_listUIProcessing.Find( &pcsUI ) && m_bSmoothUI && bSmooth )
	{
		if( m_listUIProcessing.AddTail( pcsUI ) )
		{
			pcsUI->m_ulProcessingStartTime = GetClockCount();
			pcsUI->m_fAlpha = 0.0f;
			pcsUI->m_eStatus = AGCDUI_STATUS_OPENING;
		}
	}
	else
	{
		pcsUI->m_fAlpha = 1.0f;
		pcsUI->m_eStatus = AGCDUI_STATUS_OPENED;
	}

	if( pcsUI->m_eUIType == AGCDUI_TYPE_POPUP_V || pcsUI->m_eUIType == AGCDUI_TYPE_POPUP_H )
	{
		if( m_pcsUIOpenedPopup )
		{
			CloseUI( m_pcsUIOpenedPopup );
		}

		m_pcsUIOpenedPopup = pcsUI;
	}

	if( pcsUI->m_pcsUIWindow->m_Property.bModal )
	{
		pcsUI->m_pcsUIWindow->SetModal();
	}

	TRACE( "\n!!!!!Open UI(%s) End!!!!!!\n\n", pcsUI->m_szUIName );
	return TRUE;
}

BOOL AgcmUIManager2::CloseUI( AgcdUI *pcsUI, BOOL bDeleteMemory, BOOL bGroup, BOOL bForce, BOOL bSmooth )
{
	if( !pcsUI ) return FALSE;
	TRACE( "\n!!!!!Close UI(%s) Start!!!!!!\n\n", pcsUI->m_szUIName );
	if( !pcsUI->m_pcsUIWindow->m_bOpened ) return TRUE;

	AuNode< AgcdUI* >* pcsNode;

	if( bGroup )
	{
		AgcdUI* pcsUITemp = pcsUI;

		do
		{
			if( pcsUITemp->m_eStatus == AGCDUI_STATUS_OPENED )
			{
				CloseUI( pcsUITemp, bDeleteMemory, FALSE, bForce, bSmooth );
			}

			pcsUITemp = pcsUITemp->m_pcsParentUI;
		} while( pcsUITemp );

		pcsUITemp = pcsUI;

		do
		{
			if( pcsUITemp->m_eStatus == AGCDUI_STATUS_OPENED )
			{
				CloseUI( pcsUITemp, bDeleteMemory, FALSE, bForce, bSmooth );
			}

			pcsUITemp = pcsUITemp->m_pcsCurrentChildUI;
		} while( pcsUITemp );

		CloseUI( pcsUI, bDeleteMemory, FALSE, bForce, bSmooth );
		return TRUE;
	}

	if( pcsUI->m_pcsUIWindow && pcsUI->m_pcsUIWindow->m_Property.bModal )
	{
		pcsUI->m_pcsUIWindow->ReleaseModal();
	}

	if( pcsUI->m_eStatus == AGCDUI_STATUS_OPENING )
	{
		UINT32	ulClockCount = GetClockCount();
		pcsUI->m_ulProcessingStartTime = ulClockCount - ( ( UINT32 )fProcessingTime - ( ulClockCount - pcsUI->m_ulProcessingStartTime ) );
		pcsUI->m_eStatus = AGCDUI_STATUS_CLOSING;
		return TRUE;
	}

	// NPC Dialog인 경우 잘가라는 Sound Play (이런 코드 딱 싫다.. ㅠㅠ);
	if( m_eModuleStatus != APMODULE_STATUS_DESTROYED )
	{
		AgcmUIEventNPCDialog* pcsAgcmUIEventNPCDialog = ( AgcmUIEventNPCDialog* )GetModule( "AgcmUIEventDialog" );
		if( pcsAgcmUIEventNPCDialog	&& !strncmp( pcsUI->m_szUIName, "UI_NPCDialog", 12 ) &&	!pcsAgcmUIEventNPCDialog->IsNoNeedNPCCloseSound() )
		{
			// NPC Dialog라면 사운드 띄워야함..
			pcsAgcmUIEventNPCDialog->CloseNPCDialog();
		}
	}

	if( bSmooth && m_bSmoothUI && !bForce && pcsUI->m_eStatus == AGCDUI_STATUS_OPENED )
	{
		if( !m_listUIProcessing.Find( &pcsUI ) )
		{
			if( m_listUIProcessing.AddTail( pcsUI ) )
			{
				pcsUI->m_ulProcessingStartTime = GetClockCount();
				pcsUI->m_fAlpha = 1.0f;
				pcsUI->m_eStatus = AGCDUI_STATUS_CLOSING;
			}

			return TRUE;
		}
	}

	if( pcsUI->m_eStatus != AGCDUI_STATUS_OPENED ) return FALSE;
	ASSERT( pcsUI );
	ASSERT( pcsUI->m_lID );
	if( !pcsUI ) return FALSE;

	// 동적으로 UI의 Image를 Unload한다.
	if( pcsUI->m_pcsUIWindow )
	{
		pcsUI->m_pcsUIWindow->m_csTextureList.UnloadTextures();

		// 해당 영역의 Tooltip을 닫는다.
		{
			INT32 lX = 0;
			INT32 lY = 0;
			pcsUI->m_pcsUIWindow->ClientToScreen( &lX, &lY );

			CloseTooltip( lX, lY, pcsUI->m_pcsUIWindow->w, pcsUI->m_pcsUIWindow->h, pcsUI->m_pcsUIWindow );
		}

		pcsUI->m_pcsUIWindow->CloseUI();

		m_csManagerWindow.DeleteChild( pcsUI->m_pcsUIWindow, FALSE, bDeleteMemory );
		m_csManagerWindow.UpdateChildWindow();
	}

	if( bDeleteMemory )
	{
		pcsUI->m_pcsUIWindow = NULL;
	}

	pcsUI->m_eStatus = AGCDUI_STATUS_ADDED;

	INT32 lIndex = 0;
	AgcdUIControl* pcsControl;
	INT32 lCoordX = -1000;
	INT32 lCoordY = 0;

	for( pcsControl = GetSequenceControl( pcsUI, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex ) )
	{
		pcsControl->m_pcsBase->m_csTextureList.UnloadTextures();
	}

	if( pcsUI->m_pcsParentUI )
	{
		pcsUI->m_pcsParentUI->m_pcsCurrentChildUI = pcsUI;
		if( pcsUI->m_bUseParentPosition )
		{
			// 이상하다.. ㅡ.ㅡ.. 머지 이거.. 부모의 위치정보를 따라가라는거 같은디..
			// 부모가 여러개는 아닐텐데 왠 while 문이지;; 재귀호출도 아니고..
			AgcdUI* pcsParentUI = pcsUI;
			while( pcsParentUI->m_pcsParentUI )
			{
				pcsParentUI = pcsParentUI->m_pcsParentUI;
			}

			if( pcsParentUI->m_pcsUIWindow )
			{
				pcsParentUI->m_pcsUIWindow->MoveWindow( pcsUI->m_pcsUIWindow->x, pcsUI->m_pcsUIWindow->y, pcsParentUI->m_pcsUIWindow->w, pcsParentUI->m_pcsUIWindow->h );
			}
		}
	}

	DetachUI( pcsUI );
	while( TRUE )
	{
		pcsNode = pcsUI->m_listAttached.GetHeadNode();
		if( !pcsNode ) break;

		DetachUI( pcsNode->GetData() );
	}

	if( m_pcsUIOpenedPopup == pcsUI )
	{
		m_pcsUIOpenedPopup = NULL;
	}

	TRACE( "\n!!!!!Close UI(%s) End!!!!!!\n\n", pcsUI->m_szUIName );
	return TRUE;
}

BOOL AgcmUIManager2::AttachUI( AgcdUI *pcsUI, AgcdUI *pcsUITarget, AgcdUIAttachType eType )
{
	if( !pcsUI || !pcsUITarget ) return FALSE;

	if( pcsUI->m_eStatus == AGCDUI_STATUS_NONE ||
		pcsUI->m_eStatus == AGCDUI_STATUS_ADDED ||
		pcsUI->m_eStatus == AGCDUI_STATUS_CLOSING ||
		pcsUITarget->m_eStatus == AGCDUI_STATUS_NONE ||
		pcsUITarget->m_eStatus == AGCDUI_STATUS_ADDED ||
		pcsUITarget->m_eStatus == AGCDUI_STATUS_CLOSING ) return FALSE;

	if( pcsUI->m_pcsUIAttach )
	{
		DetachUI( pcsUI );
	}

	pcsUI->m_pcsUIAttach = pcsUITarget;
	pcsUI->m_eAttachType = eType;
	pcsUITarget->m_listAttached.AddTail( pcsUI );

	pcsUITarget->m_pcsUIWindow->MoveWindow( pcsUITarget->m_pcsUIWindow->x, pcsUITarget->m_pcsUIWindow->y,
											pcsUITarget->m_pcsUIWindow->w, pcsUITarget->m_pcsUIWindow->h);

	return TRUE;
}

BOOL AgcmUIManager2::DetachUI( AgcdUI *pcsUI )
{
	if( !pcsUI ) return FALSE;
	if( !pcsUI->m_pcsUIAttach ) return FALSE;

	pcsUI->m_pcsUIAttach->m_listAttached.RemoveData( pcsUI );
	pcsUI->m_pcsUIAttach = NULL;

	return TRUE;
}

BOOL AgcmUIManager2::AttachTooltip( AgcdUI *pcsUI, AcUIToolTip *pcsTooltip, AgcdUIAttachType eType )
{
	if( !pcsUI || !pcsTooltip )	return FALSE;
	if( pcsUI->m_eStatus == AGCDUI_STATUS_NONE ||
		pcsUI->m_eStatus == AGCDUI_STATUS_ADDED ||
		pcsUI->m_eStatus == AGCDUI_STATUS_CLOSING ) return FALSE;

	pcsUI->m_pcsTooltipAttach = pcsTooltip;
	pcsUI->m_eAttachTypeToolTip = eType;
	return TRUE;
}

BOOL AgcmUIManager2::DetachTooltip( AgcdUI *pcsUI )
{
	if( !pcsUI ) return FALSE;
	pcsUI->m_pcsTooltipAttach = NULL;
	return TRUE;
}

BOOL AgcmUIManager2::OpenMainUI( void )
{
	INT32 lIndex = 0;
	AgcdUI*	pcsUI;

	for( pcsUI = GetSequenceUI( &lIndex ) ; pcsUI ; pcsUI = GetSequenceUI( &lIndex ) )
	{
		if( pcsUI->m_bMainUI && pcsUI->m_eStatus == AGCDUI_STATUS_ADDED )
		{
			OpenUI( pcsUI );
		}
	}

	EnumCallback( AGCMUIMANAGER2_CB_ID_OPEN_MAIN_UI, NULL, NULL );
	return TRUE;
}

BOOL AgcmUIManager2::CloseAllUI( void )
{
	INT32 lIndex = 0;
	AgcdUI* pcsUI;

	for( pcsUI = GetSequenceUI( &lIndex ) ; pcsUI ; pcsUI = GetSequenceUI( &lIndex ) )
	{
		CloseUI( pcsUI );
	}

	EnumCallback( AGCMUIMANAGER2_CB_ID_CLOSE_ALL_UI, NULL, NULL );
	return TRUE;
}

BOOL AgcmUIManager2::CloseAllUIExceptMainUI( BOOL *pbIsCloseWindow )
{
	INT32 lIndex = 0;
	AgcdUI*	pcsUI;

	if( pbIsCloseWindow )
	{
		*pbIsCloseWindow = FALSE;
	}

	for( pcsUI = GetSequenceUI( &lIndex ) ; pcsUI ; pcsUI = GetSequenceUI( &lIndex ) )
	{
		if( !pcsUI->m_bMainUI && pcsUI->m_bAutoClose && pcsUI->m_pcsUIWindow->m_bOpened )
		{
			if( CloseUI( pcsUI ) && pbIsCloseWindow )
			{
				*pbIsCloseWindow = TRUE;
			}
		}
	}

	return TRUE;
}

BOOL AgcmUIManager2::CloseAllEventUI( void )
{
	INT32 lIndex = 0;
	AgcdUI*	pcsUI;

	for( pcsUI = GetSequenceUI( &lIndex ) ; pcsUI ; pcsUI = GetSequenceUI( &lIndex ) )
	{
		if( pcsUI->m_bEventUI )
		{
			CloseUI( pcsUI );
		}
	}

	return TRUE;
}

AgcdUI*	AgcmUIManager2::CopyUI( AgcdUI *pcsUI, CHAR *szName, BOOL bTransControl )
{
	if( !pcsUI ) return FALSE;

	AgcdUI*					pcsUIClone;
	AgcdUIControl*			pcsControl;
	AgcdUIControl*			pcsControlClone;
	AgcdUIArg*				pcsArg;
	AgcdUIActionUI*			pstActionUI;
	AgcdUIActionControl*	pstActionControl;

	INT32 lIndex;
	INT32 lIndex2;
	INT32 lIndex3;
	INT32 lIndex4;

	pcsUIClone = CreateUI();
	if( !pcsUIClone ) return NULL;

	strcpy( pcsUIClone->m_szUIName, szName );
	strcpy( pcsUIClone->m_szParentUIName, pcsUI->m_szParentUIName );

	memcpy( pcsUIClone->m_alX, pcsUI->m_alX, sizeof( INT32 ) * AGCDUI_MAX_MODE );
	memcpy( pcsUIClone->m_alY, pcsUI->m_alY, sizeof( INT32 ) * AGCDUI_MAX_MODE );

	pcsUIClone->m_bAutoClose			= pcsUI->m_bAutoClose;
	pcsUIClone->m_bEventUI				= pcsUI->m_bEventUI;
	pcsUIClone->m_bInitControlStatus	= pcsUI->m_bInitControlStatus;
	pcsUIClone->m_bMainUI				= pcsUI->m_bMainUI;
	pcsUIClone->m_bTransparent			= pcsUI->m_bTransparent;
	pcsUIClone->m_bUseParentPosition	= pcsUI->m_bUseParentPosition;
	pcsUIClone->m_eAttachType			= pcsUI->m_eAttachType;
	pcsUIClone->m_eAttachTypeToolTip	= pcsUI->m_eAttachTypeToolTip;
	pcsUIClone->m_eCoordSystem			= pcsUI->m_eCoordSystem;
	pcsUIClone->m_eType					= pcsUI->m_eType;
	pcsUIClone->m_eUIType				= pcsUI->m_eUIType;
	pcsUIClone->m_fAlpha				= pcsUI->m_fAlpha;
	pcsUIClone->m_lMaxChildID			= pcsUI->m_lMaxChildID;
	pcsUIClone->m_pcsParentUI			= pcsUI->m_pcsParentUI;
	pcsUIClone->m_pstKillFocusFunction	= pcsUI->m_pstKillFocusFunction;
	pcsUIClone->m_pstSetFocusFunction	= pcsUI->m_pstSetFocusFunction;
	pcsUIClone->m_ulModeFlag			= pcsUI->m_ulModeFlag;
	pcsUIClone->m_pcsTooltipAttach		= pcsUI->m_pcsTooltipAttach;
	pcsUIClone->m_pcsUIAttach			= pcsUI->m_pcsUIAttach;

	CopyBaseProperty( pcsUI->m_pcsUIWindow, pcsUIClone->m_pcsUIWindow );

	lIndex = 0;
	for( pcsControl = GetSequenceControl( pcsUI, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex ) )
	{
		if( !pcsControl->m_pcsParentControl )
		{
			pcsControlClone = CopyControl( pcsControl, pcsUIClone );
		}
	}
	
	lIndex = 0;
	for( pcsControl = GetSequenceControl( pcsUIClone, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUIClone, &lIndex ) )
	{
		switch( pcsControl->m_lType )
		{
		case AcUIBase::TYPE_LIST_ITEM:		pcsControl->m_pcsParentControl->m_uoData.m_stList.m_pcsListItemFormat = pcsControl;		break;
		case AcUIBase::TYPE_TREE_ITEM:		pcsControl->m_pcsParentControl->m_uoData.m_stTree.m_pcsListItemFormat = pcsControl;		break;
		}

		if( bTransControl )
		{
			for( lIndex2 = 0; lIndex2 < pcsControl->m_lMessageCount; ++lIndex2 )
			{
				for( lIndex3 = 0; lIndex3 < pcsControl->m_pstMessageMaps[ lIndex2 ].m_listAction.GetCount(); ++lIndex3 )
				{
					if( pcsControl->m_pstMessageMaps[ lIndex2 ].m_listAction[ lIndex3 ].m_eType == AGCDUI_ACTION_FUNCTION )
					{
						for( lIndex4 = 0; lIndex4 < AGCDUIMANAGER2_MAX_ARGS; ++lIndex4 )
						{
							pcsArg = pcsControl->m_pstMessageMaps[ lIndex2 ].m_listAction[ lIndex3 ].m_uoAction.m_stFunction.m_astArgs + lIndex4;
							if( pcsArg->m_pcsUI == pcsUI )
							{
								pcsArg->m_pcsUI = pcsUIClone;
								if( pcsArg->m_pstControl )
								{
									pcsArg->m_pstControl = GetControl( pcsUIClone, pcsArg->m_pstControl->m_lID );
								}
							}
						}
					}
					else if( pcsControl->m_pstMessageMaps[ lIndex2 ].m_listAction[ lIndex3 ].m_eType == AGCDUI_ACTION_UI )
					{
						pstActionUI = &pcsControl->m_pstMessageMaps[ lIndex2 ].m_listAction[ lIndex3 ].m_uoAction.m_stUI;
						if( pstActionUI->m_pcsUI == pcsUI )
						{
							pstActionUI->m_pcsUI = pcsUIClone;
						}
					}
					else if( pcsControl->m_pstMessageMaps[ lIndex2 ].m_listAction[ lIndex3 ].m_eType == AGCDUI_ACTION_CONTROL )
					{
						pstActionControl = &pcsControl->m_pstMessageMaps[ lIndex2 ].m_listAction[ lIndex3 ].m_uoAction.m_stControl;
						if( pstActionControl->m_pcsUI == pcsUI )
						{
							pstActionControl->m_pcsUI = pcsUIClone;
							pstActionControl->m_pcsControl = GetControl( pcsUIClone, pstActionControl->m_pcsControl->m_lID );
						}
					}
				}
			}
		}
	}

	return pcsUIClone;
}

VOID AgcmUIManager2::SetUIData( AgcdUI *pcsUI, INT32 lUIData )
{
	if( pcsUI )
	{
		pcsUI->m_lUIData = lUIData;
	}
}

INT32 AgcmUIManager2::GetUIData( AgcdUI *pcsUI )
{
	if( pcsUI )
	{
		return pcsUI->m_lUIData;
	}

	return 0;
}

AgcdUIControl* AgcmUIManager2::AddControl(AgcdUI *pcsUI, INT32 lType, INT32 lX, INT32 lY, AgcWindow *pcsWindow, AcUIBase *pcsBase, AgcdUICControl *pcsCustomControl)
{
	AcUIBase* pcsControlBase = NULL;
	AgcdUIControl* pcsControl = NULL;
	AgcdUIControl* pcsParent = NULL;

	if( !pcsUI )
	{
		pcsUI = m_pcsUIDummy;
	}

	if( pcsBase )
	{
		lType = pcsBase->m_nType;
		pcsControlBase = pcsBase;
	}
	else
	{
		switch( lType )
		{
		case AcUIBase::TYPE_BASE :			pcsControlBase = ( AcUIBase* )new AcUIBase;			break;
		case AcUIBase::TYPE_EDIT :
			{
				pcsControlBase = ( AcUIBase* )new AcUIEdit;
				( ( AcUIEdit* )pcsControlBase )->SetTextMaxLength( 0 );
			}
			break;

		case AcUIBase::TYPE_BUTTON :		pcsControlBase = ( AcUIBase* )new AcUIButton;		break;
		case AcUIBase::TYPE_CHECKBOX :		pcsControlBase = ( AcUIBase* )new AcUICheckBox;		break;
		case AcUIBase::TYPE_COMBO :			pcsControlBase = ( AcUIBase* )new AcUICombo;		break;
		case AcUIBase::TYPE_GRID :			pcsControlBase = ( AcUIBase* )new AcUIGrid;			break;
		case AcUIBase::TYPE_SKILL_TREE :	pcsControlBase = ( AcUIBase* )new AcUISkillTree;	break;
		case AcUIBase::TYPE_BAR :			pcsControlBase = ( AcUIBase* )new AcUIBar;			break;
		case AcUIBase::TYPE_LIST :
			{
				pcsControlBase = ( AcUIBase* )new AcUIList;
				( ( AcUIList* )pcsControlBase )->SetCallbackConstructor( CBListItemConstructor, this, pcsUI );
				( ( AcUIList* )pcsControlBase )->SetCallbackDestructor( CBListItemDestructor, this, pcsUI );
				( ( AcUIList* )pcsControlBase )->SetCallbackRefresh( CBListItemRefresh, this, pcsUI );
			}
			break;

		case AcUIBase::TYPE_TREE :
			{
				pcsControlBase = ( AcUIBase* )new AcUITree;
				( ( AcUITree* )pcsControlBase )->SetCallbackConstructor( CBListItemConstructor, this, pcsUI );
				( ( AcUITree* )pcsControlBase )->SetCallbackDestructor( CBListItemDestructor, this, pcsUI );
				( ( AcUITree* )pcsControlBase )->SetCallbackRefresh( CBListItemRefresh, this, pcsUI );
			}
			break;

		case AcUIBase::TYPE_LIST_ITEM :		pcsControlBase = ( AcUIBase* )new AcUIListItem;		break;
		case AcUIBase::TYPE_TREE_ITEM :		pcsControlBase = ( AcUIBase* )new AcUITreeItem; 	break;
		case AcUIBase::TYPE_SCROLL :		pcsControlBase = ( AcUIBase* )new AcUIScroll;		break;
		case AcUIBase::TYPE_SCROLL_BUTTON :	pcsControlBase = ( AcUIBase* )new AcUIScrollButton;	break;
		case AcUIBase::TYPE_CLOCK :			pcsControlBase = ( AcUIBase* )new AcUIClock;		break;
		case AcUIBase::TYPE_CUSTOM :
			{
				if( !pcsCustomControl )
				{
					ASSERT( pcsCustomControl && "Custom Control not exist!!!" );
					pcsControlBase = ( AcUIBase* )new AcUIBase;
					return FALSE;
				}
				else
				{
					pcsControlBase = ( AcUIBase* )pcsCustomControl->m_pcsBase;
				}
			}
			break;

		default:							return NULL;										break;
		}
	}

	if( !pcsControlBase ) return NULL;
	TRACE( "ControlBase %x allocated (Parent %x)\n", pcsControlBase, pcsWindow );

	pcsControl = ( AgcdUIControl* )CreateModuleData( AGCMUIMANAGER2_DATA_TYPE_CONTROL );
	if( !pcsControl )
	{
		delete pcsControlBase;
		return NULL;
	}

	pcsControl->m_eType = APBASE_TYPE_UI_CONTROL;

	if( pcsBase )
	{
		pcsControl->m_bCustomBase = TRUE;
	}

	pcsControl->m_lType = lType;
	pcsControl->m_pcsBase = pcsControlBase;
	pcsControl->m_lMessageCount = m_alControlMessages[ pcsControl->m_pcsBase->m_nType ];

	if( pcsControl->m_lMessageCount )
	{
		pcsControl->m_pstMessageMaps = new AgcdUIMessageMap[ pcsControl->m_lMessageCount ];
	}

	pcsControl->m_stDisplayMap.m_stFont.m_ulColor = 0xffffffff;
	pcsControl->m_stDisplayMap.m_stFont.m_fScale = 1.0f;
	pcsControl->m_stDisplayMap.m_stFont.m_bShadow = true;

	pcsControl->m_pcsBase->MoveWindow( lX, lY, 100, 30 );
	pcsControl->m_pstUserData = NULL;
	pcsControl->m_pcsParentUI = pcsUI;
	++( pcsUI->m_lMaxChildID );

	pcsControl->m_lID = ( INT32 )pcsControlBase;
	pcsControlBase->m_lControlID = ( INT32 )pcsControlBase;

	pcsUI->m_listChild.AddTail( pcsControl );
	if( pcsWindow )
	{
		pcsWindow->AddChild( pcsControl->m_pcsBase, pcsControlBase->m_lControlID );
	}
	else
	{
		pcsUI->m_pcsUIWindow->AddChild( pcsControl->m_pcsBase, pcsControlBase->m_lControlID );
	}

	if( pcsWindow )
	{
		pcsParent = GetControl( pcsUI, ( AcUIBase* )pcsWindow );
	}

	pcsControl->m_pcsParentControl = pcsParent;
	pcsControl->m_pstShowCB = NULL;
	pcsControl->m_pstShowUD = NULL;

	if( pcsParent )
	{
		pcsParent->m_listChild.AddTail( pcsControl );
		if( pcsControl->m_lType == AcUIBase::TYPE_SCROLL )
		{
			switch( pcsParent->m_lType )
			{
			case AcUIBase::TYPE_LIST :
			case AcUIBase::TYPE_TREE :	( ( AcUIList* )pcsParent->m_pcsBase )->SetListScroll( ( AcUIScroll* )pcsControlBase );	break;
			case AcUIBase::TYPE_EDIT :	( ( AcUIEdit* )pcsParent->m_pcsBase )->SetVScroll( ( AcUIScroll* )pcsControlBase );		break;
			case AcUIBase::TYPE_GRID :	( ( AcUIGrid* )pcsParent->m_pcsBase )->SetScroll( ( AcUIScroll* )pcsControlBase );		break;
			}
		}
	}

	if( lType == AcUIBase::TYPE_CUSTOM )
	{
		pcsControl->m_uoData.m_stCustom.m_pstCustomControl = pcsCustomControl;
	}

	SetControlAlpha( pcsControl->m_pcsBase, &pcsUI->m_fAlpha );
	return pcsControl;
}

VOID AgcmUIManager2::SetControlAlpha(AgcWindow *pcsWindow, FLOAT *pfAlpha)
{
	if( pcsWindow->m_bUIWindow )
	{
		( ( AcUIBase* )pcsWindow )->m_pfAlpha = pfAlpha;
	}

	AgcWindowNode* cur_node = pcsWindow->m_listChild.head;
	while( cur_node )
	{
		SetControlAlpha( cur_node->pWindow, pfAlpha );
		cur_node = cur_node->next;
	}
}

AgcdUIControl* AgcmUIManager2::GetControl( AgcdUI *pcsUI, AcUIBase *pcsUIBase )
{
	if( !pcsUI ) return NULL;

	INT32 lIndex = 0;
	AgcdUIControl* pcsControl;

	for( pcsControl = GetSequenceControl( pcsUI, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex ) )
	{
		if( pcsControl->m_pcsBase == pcsUIBase )
		{
			return pcsControl;
		}
	}

	return NULL;
}

AgcdUIControl* AgcmUIManager2::GetControl( AgcdUI *pcsUI, CHAR *szName )
{
	if( !pcsUI ) return NULL;

	INT32 lIndex = 0;
	AgcdUIControl *	pcsControl;

	for( pcsControl = GetSequenceControl( pcsUI, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex ) )
	{
		if( !strcmp( szName, pcsControl->m_szName ) )
		{
			return pcsControl;
		}
	}

	return NULL;
}

AgcdUIControl* AgcmUIManager2::GetControl( AgcdUI *pcsUI, INT32 lID )
{
	if( !pcsUI ) return NULL;

	INT32 lIndex = 0;
	AgcdUIControl *	pcsControl;

	for( pcsControl = GetSequenceControl( pcsUI, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex ) )
	{
		if( lID == pcsControl->m_lID )
		{
			return pcsControl;
		}
	}

	return NULL;
}

AgcdUIControl* AgcmUIManager2::GetControl( AgcdUIControl *pcsParentControl, AcUIBase *pcsUIBase )
{
	if( !pcsParentControl )	return NULL;

	INT32 lIndex = 0;
	AgcdUIControl *	pcsControl;

	for( pcsControl = GetSequenceControl( pcsParentControl, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsParentControl, &lIndex ) )
	{
		if( pcsControl->m_pcsBase == pcsUIBase ) return pcsControl;
	
		pcsControl = GetControl( pcsControl, pcsUIBase );
		if( pcsControl ) return pcsControl;
	}

	return NULL;
}

AgcdUIControl* AgcmUIManager2::GetControl( AgcdUIControl *pcsParentControl, CHAR *szName )
{
	if( !pcsParentControl ) return NULL;

	INT32 lIndex = 0;
	AgcdUIControl *	pcsControl;

	for( pcsControl = GetSequenceControl( pcsParentControl, &lIndex) ;  pcsControl ; pcsControl = GetSequenceControl( pcsParentControl, &lIndex ) )
	{
		if( !strcmp( szName, pcsControl->m_szName ) ) return pcsControl;

		pcsControl = GetControl( pcsControl, szName );
		if( pcsControl ) return pcsControl;
	}

	return NULL;
}

AgcdUIControl *	AgcmUIManager2::GetControl(AgcdUIControl *pcsParentControl, INT32 lID)
{
	if( !pcsParentControl )	return NULL;

	INT32 lIndex = 0;
	AgcdUIControl *	pcsControl;

	for( pcsControl = GetSequenceControl( pcsParentControl, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsParentControl, &lIndex ) )
	{
		if( lID == pcsControl->m_lID ) return pcsControl;

		pcsControl = GetControl( pcsControl, lID );
		if( pcsControl ) return pcsControl;
	}

	return NULL;
}

BOOL AgcmUIManager2::RemoveControl( AgcdUIControl *pcsControl, BOOL bTraverse, BOOL bChildOnly )
{
	PROFILE( "AgcmUIManager2::RemoveControl1" );

	INT32 lIndex = 0;
	AgcdUIControl* pcsChildControl;

	// List는 Streaming할때 FormatControl을 실제 Window Hierarchy에는 저장하지 않는다. 그렇기 때문에, Remove를 위해서는 Window Hierarchy에 추가한다.
	if( ( pcsControl->m_lType == AcUIBase::TYPE_LIST || pcsControl->m_lType == AcUIBase::TYPE_TREE ) && pcsControl->m_uoData.m_stList.m_pcsListItemFormat )
	{
		pcsControl->m_pcsBase->AddChild( pcsControl->m_uoData.m_stList.m_pcsListItemFormat->m_pcsBase );
		pcsControl->m_pcsBase->UpdateChildWindow();
	}

	if( bTraverse )
	{
		while( TRUE )
		{
			lIndex = 0;
			pcsChildControl = GetSequenceControl( pcsControl, &lIndex );
			if( !pcsChildControl ) break;

			RemoveControl( pcsChildControl, bTraverse, bChildOnly );
		}
	}
	else
	{
		lIndex = 0;
		for( pcsChildControl = GetSequenceControl( pcsControl, &lIndex ) ; pcsChildControl ; pcsChildControl = GetSequenceControl( pcsControl, &lIndex ) )
		{
			pcsChildControl->m_pcsParentControl = NULL;
			pcsChildControl->m_pcsBase->pParent = NULL;
		}

		pcsControl->m_listChild.RemoveAll();
	}

	if( !bChildOnly )
	{
		if( !pcsControl->m_bDynamic && !m_bBeingDestroyed )
		{
			RemoveFromActions( NULL, pcsControl );
		}

		if( !m_bBeingDestroyed )
		{
			CountUserData( pcsControl, FALSE, TRUE );
		}

		if( pcsControl->m_stDisplayMap.m_eType == AGCDUI_DISPLAYMAP_TYPE_STATIC && pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic )
		{
			delete[] pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic;
			pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic = NULL;
		}

		if( pcsControl->m_pstMessageMaps )
		{
			delete[] pcsControl->m_pstMessageMaps;
			pcsControl->m_pstMessageMaps = NULL;
		}

		if( pcsControl->m_szTooltip )
		{
			delete[] pcsControl->m_szTooltip;
			pcsControl->m_szTooltip = NULL;
		}

		pcsControl->m_pcsParentUI->m_listChild.RemoveData( pcsControl );

		if( pcsControl->m_pcsParentControl )
		{
			pcsControl->m_pcsParentControl->m_listChild.RemoveData( pcsControl );
		}

		if( pcsControl->m_pcsBase )
		{
			pcsControl->m_pcsBase->m_Property.bVisible = TRUE;
			if( pcsControl->m_pcsBase->pParent )
			{
				AcUIBase* pcsBase = ( AcUIBase* )pcsControl->m_pcsBase->pParent;
				if( pcsControl->m_lType == AcUIBase::TYPE_CUSTOM || pcsControl->m_bCustomBase )
				{
					pcsBase->DeleteChild( pcsControl->m_pcsBase, TRUE, FALSE );
				}
				else
				{
					pcsBase->DeleteChild( pcsControl->m_pcsBase, TRUE, TRUE );
					pcsControl->m_pcsBase = NULL;
				}
			}
		}

		{
			AuNode< AgcdUIActionBuffer* >* pstNode = m_listActionBuffers.GetHeadNode();
			AgcdUIActionBuffer* pstActionBuffer;

			while( pstNode )
			{
				pstActionBuffer = m_listActionBuffers.GetNext( pstNode );
				if( pstActionBuffer->m_pcsControl == pcsControl )
				{
					m_listActionBuffers.RemoveData( pstActionBuffer );
					delete pstActionBuffer;
					pstActionBuffer = NULL;
				}
			}
		}

		DestroyModuleData( pcsControl, AGCMUIMANAGER2_DATA_TYPE_CONTROL );
	}

	return TRUE;
}

BOOL AgcmUIManager2::RemoveControl( AgcdUI *pcsUI, AcUIBase *pcsUIBase, BOOL bTraverse )
{
	PROFILE( "AgcmUIManager2::RemoveControl2" );

	AgcdUIControl* pcsControl = GetControl( pcsUI, pcsUIBase );
	if( !pcsControl ) return FALSE;

	return RemoveControl( pcsControl, bTraverse );
}

AgcdUIControl* AgcmUIManager2::GetSequenceControl( AgcdUI *pcsUI, INT32 *plIndex )
{
	if( !pcsUI )
	{
		pcsUI = m_pcsUIDummy;
	}

	if( *plIndex == -1 ) return NULL;

	AuNode< AgcdUIControl* >* pNode;
	AgcdUIControl* pcsControl;

	if( !*plIndex )
	{
		pNode = pcsUI->m_listChild.GetHeadNode();
	}
	else
	{
		pNode = ( AuNode< AgcdUIControl* >* )*plIndex;
	}

	if( !pNode ) return NULL;

	pcsControl = pNode->GetData();
	pNode = pNode->GetNextNode();
	if( !pNode )
	{
		*plIndex = -1;
	}
	else
	{
		*plIndex = ( INT32 )pNode;
	}

	return pcsControl;
}

AgcdUIControl* AgcmUIManager2::GetSequenceControl( AgcdUIControl *pcsParentControl, INT32 *plIndex )
{
	if( !pcsParentControl )	return NULL;
	if( *plIndex == -1 ) return NULL;

	AuNode< AgcdUIControl* >* pNode;
	AgcdUIControl* pcsControl;

	if( !*plIndex )
	{
		pNode = pcsParentControl->m_listChild.GetHeadNode();
	}
	else
	{
		pNode = ( AuNode< AgcdUIControl* >* )*plIndex;
	}

	if( !pNode ) return NULL;

	pcsControl = pNode->GetData();
	pNode = pNode->GetNextNode();

	if( !pNode )
	{
		*plIndex = -1;
	}
	else
	{
		*plIndex = ( INT32 )pNode;
	}

	return pcsControl;
}

VOID AgcmUIManager2::CopyBaseProperty( AcUIBase *pcsSource, AcUIBase *pcsTarget )
{
	if( !pcsSource || !pcsTarget || pcsSource->m_nType != pcsTarget->m_nType ) return;

	pcsTarget->m_Property			= pcsSource->m_Property;
	pcsTarget->m_csTextureList		= pcsSource->m_csTextureList;
	pcsTarget->m_stRenderInfo		= pcsSource->m_stRenderInfo;
	pcsTarget->m_clProperty			= pcsSource->m_clProperty;
	pcsTarget->m_bAutoFitString		= pcsSource->m_bAutoFitString;
	pcsTarget->m_bClipImage			= pcsSource->m_bClipImage;
	pcsTarget->m_bStringNumberComma	= pcsSource->m_bStringNumberComma;
	pcsTarget->m_bShadow			= pcsSource->m_bShadow;
	pcsTarget->x					= pcsSource->x;
	pcsTarget->y					= pcsSource->y;
	pcsTarget->w					= pcsSource->w;
	pcsTarget->h					= pcsSource->h;
	pcsTarget->m_eHAlign			= pcsSource->m_eHAlign;
	pcsTarget->m_eVAlign			= pcsSource->m_eVAlign;

	// Copy Animation
	AcUIBaseAnimation* pcsAnim;
	if( pcsTarget->m_pAnimationData )
	{
		pcsTarget->RemoveAllAnimation();
	}

	for( pcsAnim = pcsSource->m_pAnimationData ; pcsAnim ; pcsAnim = pcsAnim->m_pNext )
	{
		pcsTarget->AddAnimation( pcsAnim->m_lTextureID, pcsAnim->m_ulClockCount, pcsAnim->m_bRenderString, pcsAnim->m_bRenderBaseTexture );
	}

	pcsTarget->m_bAnimation	= pcsSource->m_bAnimation;
	pcsTarget->m_bAnimationDefault = pcsSource->m_bAnimationDefault;

	if( pcsTarget->m_bAnimationDefault )
	{
		pcsTarget->StartAnimation();
	}

	// Status Copy
	memcpy( pcsTarget->m_astStatus, pcsSource->m_astStatus, sizeof( stStatusInfo ) * ACUIBASE_STATUS_MAX_NUM );
	switch( pcsSource->m_nType )
	{
	case AcUIBase::TYPE_BAR :
		{
			( ( AcUIBar* )pcsTarget )->SetBodyImageID( ( ( AcUIBar* )pcsSource )->GetBodyImageID() );
			( ( AcUIBar* )pcsTarget )->SetEdgeImageID( ( ( AcUIBar* )pcsSource )->GetEdgeImageID() );
		}
		break;

	case AcUIBase::TYPE_BUTTON :
	case AcUIBase::TYPE_SCROLL_BUTTON :
		{
			( ( AcUIButton* )pcsTarget )->SetButtonImage( ( ( AcUIButton* )pcsSource )->GetButtonImage( ACUIBUTTON_MODE_NORMAL ), ACUIBUTTON_MODE_NORMAL );
			( ( AcUIButton* )pcsTarget )->SetButtonImage( ( ( AcUIButton* )pcsSource )->GetButtonImage( ACUIBUTTON_MODE_ONMOUSE ), ACUIBUTTON_MODE_ONMOUSE );
			( ( AcUIButton* )pcsTarget )->SetButtonImage( ( ( AcUIButton* )pcsSource )->GetButtonImage( ACUIBUTTON_MODE_CLICK ), ACUIBUTTON_MODE_CLICK );
			( ( AcUIButton* )pcsTarget )->SetButtonImage( ( ( AcUIButton* )pcsSource )->GetButtonImage( ACUIBUTTON_MODE_DISABLE ), ACUIBUTTON_MODE_DISABLE );

			( ( AcUIButton* )pcsTarget )->m_stProperty = ( ( AcUIButton* )pcsSource )->m_stProperty;
			( ( AcUIButton* )pcsTarget )->m_lButtonDownStringOffsetX = ( ( AcUIButton* )pcsSource )->m_lButtonDownStringOffsetX;
			( ( AcUIButton* )pcsTarget )->m_lButtonDownStringOffsetY = ( ( AcUIButton* )pcsSource )->m_lButtonDownStringOffsetY;

			// Scroll에 딸린 버튼이라면... 처리
			if( pcsSource->pParent && ( ( AcUIBase* )pcsSource->pParent )->m_nType == AcUIBase::TYPE_SCROLL &&
				pcsTarget->pParent && ( ( AcUIBase* )pcsTarget->pParent )->m_nType == AcUIBase::TYPE_SCROLL )
			{
				AcUIScroll*	pcsScrollSource = ( AcUIScroll* )pcsSource->pParent;
				AcUIScroll*	pcsScrollTarget = ( AcUIScroll* )pcsTarget->pParent;

				if( pcsScrollSource->GetScrollUpButton() == pcsSource )
				{
					pcsScrollTarget->SetScrollUpButton( ( AcUIButton* )pcsTarget );
				}
				else if( pcsScrollSource->GetScrollDownButton() == pcsSource )
				{
					pcsScrollTarget->SetScrollDownButton( ( AcUIButton* )pcsTarget );
				}
				else if( pcsSource->m_nType == AcUIBase::TYPE_SCROLL_BUTTON )
				{
					pcsScrollTarget->SetScrollButton( ( AcUIScrollButton* )pcsTarget );
				}
			}
		}
		break;

	case AcUIBase::TYPE_SCROLL :
		{
			( ( AcUIScroll* )pcsTarget )->SetScrollButtonInfo(	( ( AcUIScroll* )pcsSource )->m_bVScroll,
																( ( AcUIScroll* )pcsSource )->m_lMinPosition,
																( ( AcUIScroll* )pcsSource )->m_lMaxPosition,
																( ( AcUIScroll* )pcsSource )->m_fScrollUnit,
																( ( AcUIScroll* )pcsSource )->m_bMoveByUnit );
		}
		break;
		
	case AcUIBase::TYPE_EDIT :
		{
			( ( AcUIEdit* )pcsTarget )->m_bAutoLF				= ( ( AcUIEdit* )pcsSource )->m_bAutoLF;
			( ( AcUIEdit* )pcsTarget )->m_bMultiLine			= ( ( AcUIEdit* )pcsSource )->m_bMultiLine;
			( ( AcUIEdit* )pcsTarget )->m_bReadOnly				= ( ( AcUIEdit* )pcsSource )->m_bReadOnly;
			( ( AcUIEdit* )pcsTarget )->m_bEnableTag			= ( ( AcUIEdit* )pcsSource )->m_bEnableTag;		//. 2005. 11. 29. Nonstopdj Tag옵션 copy추가.
			( ( AcUIEdit* )pcsTarget )->m_bPasswordEdit			= ( ( AcUIEdit* )pcsSource )->m_bPasswordEdit;
			( ( AcUIEdit* )pcsTarget )->m_bReleaseEditInputEnd	= ( ( AcUIEdit* )pcsSource )->m_bReleaseEditInputEnd;
			( ( AcUIEdit* )pcsTarget )->m_bForHotkey			= ( ( AcUIEdit* )pcsSource )->m_bForHotkey;

			( ( AcUIEdit* )pcsTarget )->SetTextMaxLength( ( ( AcUIEdit* )pcsSource )->GetTextMaxLength() );			
		}
		break;

	case AcUIBase::TYPE_LIST :	//. 2005. 11. 29. Nonstopdj. List의 Property 복사.
		{
			( ( AcUIList* )pcsTarget )->SetListItemColumn( ( ( AcUIList* )pcsSource )->m_lItemColumn );
			( ( AcUIList* )pcsTarget )->SetListItemWindowMoveInfo(	( ( AcUIList* )pcsSource )->m_lListItemWidth,
																	( ( AcUIList* )pcsSource )->m_lListItemHeight,
																	( ( AcUIList* )pcsSource )->m_lListItemStartX,
																	( ( AcUIList* )pcsSource )->m_lListItemStartY );

			( ( AcUIList* )pcsTarget )->SetListItemWindowVisibleRow( ( ( AcUIList* )pcsSource )->m_lVisibleListItemRow );
		}
		break;

	case AcUIBase::TYPE_SKILL_TREE :
		{
			memcpy( ( ( AcUISkillTree* )pcsTarget )->m_alSkillBackImageID, ( ( AcUISkillTree* )pcsSource )->m_alSkillBackImageID, sizeof( INT32 ) * ACUI_SKILLTREE_MAX_TYPE );
		}
	case AcUIBase::TYPE_GRID :
		{
			( ( AcUIGrid* )pcsTarget )->SetGridItemDrawInfo(	( ( AcUIGrid* )pcsSource )->m_lGridItemStart_x,
																( ( AcUIGrid* )pcsSource )->m_lGridItemStart_y,
																( ( AcUIGrid* )pcsSource )->m_lGridItemGap_x,
																( ( AcUIGrid* )pcsSource )->m_lGridItemGap_y,
																( ( AcUIGrid* )pcsSource )->m_lGridItemWidth,
																( ( AcUIGrid* )pcsSource )->m_lGridItemHeight );

			// 잘나오나 Test를 위해서 넣는다.
			( ( AcUIGrid* )pcsTarget )->SetDrawAreas( TRUE );

			( ( AcUIGrid* )pcsTarget )->SetGridItemMovable(				( ( AcUIGrid* )pcsSource )->GetGridItemMovable() );
			( ( AcUIGrid* )pcsTarget )->SetGridItemBottomCountWrite(	( ( AcUIGrid* )pcsSource )->GetGridItemBottomCountWrite() );
			( ( AcUIGrid* )pcsTarget )->SetGridItemMoveItemCopy(		( ( AcUIGrid* )pcsSource )->GetGridItemMoveItemCopy() );
			( ( AcUIGrid* )pcsTarget )->SetGridItemDrawImageForeground(	( ( AcUIGrid* )pcsSource )->GetGridItemDrawImageForeground() );
			( ( AcUIGrid* )pcsTarget )->SetReusableDisplayImage(		( ( AcUIGrid* )pcsSource )->GetReusableDisplayImage() );
			break;
		}

	case AcUIBase::TYPE_CLOCK:
		{
			( ( AcUIClock* )pcsTarget )->SetAlphaImageID( ( ( AcUIClock* )pcsSource )->GetAlphaImageID() );
		}
		break;
	}
}

AgcdUIControl *	AgcmUIManager2::CopyControl(AgcdUIControl *pcsControl, AgcdUI *pcsUI, AgcWindow *pcsParentWindow, BOOL bCopyChild, INT32 lDataIndex, AgcdUIControl *pcsDstControl, BOOL bDynamic)
{
	INT32 lIndex = 0;
	if( !pcsUI )
	{
		pcsUI = m_pcsUIDummy;
	}

	if( !pcsDstControl )
	{
		pcsDstControl = AddControl( pcsUI, pcsControl->m_lType, 0, 0, pcsParentWindow );
		if( !pcsDstControl ) return NULL;
	}

	TRACE( "ControlBase %x Copied (%d)\n", pcsDstControl->m_pcsBase, bDynamic );
	if( pcsDstControl->m_lType != pcsControl->m_lType ) return NULL;

	// Copy UIBase Properties
	CopyBaseProperty( pcsControl->m_pcsBase, pcsDstControl->m_pcsBase );
	if( pcsControl->m_pcsParentUI != pcsUI )
	{
		pcsDstControl->m_lID					= pcsControl->m_lID;
		pcsDstControl->m_pcsBase->m_lControlID	= pcsControl->m_pcsBase->m_lControlID;
	}

	if( pcsControl->m_pcsParentControl == pcsDstControl->m_pcsParentControl )
	{
		pcsDstControl->m_pcsBase->MoveWindow( pcsDstControl->m_pcsBase->x, pcsDstControl->m_pcsBase->y, pcsControl->m_pcsBase->w, pcsControl->m_pcsBase->h );
	}
	else
	{
		pcsDstControl->m_pcsBase->MoveWindow( pcsControl->m_pcsBase->x, pcsControl->m_pcsBase->y, pcsControl->m_pcsBase->w, pcsControl->m_pcsBase->h );
	}

	if( lDataIndex == -1 )
	{
		pcsDstControl->m_lUserDataIndex	= pcsControl->m_lUserDataIndex;
	}
	else
	{
		pcsDstControl->m_lUserDataIndex = lDataIndex;
	}

	pcsDstControl->m_uoData	= pcsControl->m_uoData;
	if( pcsControl->m_stDisplayMap.m_eType == AGCDUI_DISPLAYMAP_TYPE_STATIC )
	{
		pcsDstControl->m_stDisplayMap.m_stFont = pcsControl->m_stDisplayMap.m_stFont;
		SetControlDisplayMap( pcsDstControl, pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic );
	}
	else
	{
		pcsDstControl->m_stDisplayMap = pcsControl->m_stDisplayMap;
	}

	pcsDstControl->m_pcsFormatControl	= pcsControl;
	pcsDstControl->m_pstShowCB			= pcsControl->m_pstShowCB;
	pcsDstControl->m_pstShowUD			= pcsControl->m_pstShowUD;

	if( pcsDstControl->m_szTooltip )
	{
		delete[] pcsDstControl->m_szTooltip;
		pcsControl->m_szTooltip = NULL;
	}
	if( pcsControl->m_szTooltip )
	{
		pcsDstControl->m_szTooltip = new CHAR[ strlen( pcsControl->m_szTooltip ) + 1 ];
		strcpy( pcsDstControl->m_szTooltip, pcsControl->m_szTooltip );
	}

	pcsDstControl->m_bDynamic = bDynamic;
	switch( pcsDstControl->m_lType )
	{
	case AcUIBase::TYPE_GRID :
		{
			SetControlGrid( pcsDstControl, pcsControl->m_uoData.m_stGrid.m_pstGrid, pcsControl->m_uoData.m_stGrid.m_bControlUserData );
		}
		break;
	}

	for( lIndex = 0 ; lIndex < pcsControl->m_lMessageCount ; ++lIndex )
	{
		pcsDstControl->m_pstMessageMaps[ lIndex ].m_listAction = pcsControl->m_pstMessageMaps[ lIndex ].m_listAction;
	}

	memcpy( pcsDstControl->m_apstUserData, pcsControl->m_apstUserData, sizeof( AgcdUIUserData* ) * ACUIBASE_STATUS_MAX_NUM );

	pcsDstControl->m_bUseParentUserData	= pcsControl->m_bUseParentUserData;
	pcsDstControl->m_pstUserData		= GetControlUserData( pcsControl );

	CountUserData( pcsDstControl, FALSE );
	RefreshControl( pcsDstControl, TRUE, FALSE );

	if( bCopyChild )
	{
		AgcdUIControl* pcsControlChild;
 
		lIndex = 0;
		for( pcsControlChild = GetSequenceControl( pcsControl, &lIndex ) ; pcsControlChild ; pcsControlChild = GetSequenceControl( pcsControl, &lIndex ) )
		{
			if( !pcsControlChild->m_bDynamic )
			{
				CopyControl( pcsControlChild, pcsUI, pcsDstControl->m_pcsBase, bCopyChild, lDataIndex, NULL, bDynamic );
			}
		}
	}

	return pcsDstControl;
}

VOID AgcmUIManager2::RefreshUI( AgcdUI *pcsUI, BOOL bForce )
{
	INT32 lIndex = 0;
	AgcdUIControl* pcsControl;

	for( pcsControl = GetSequenceControl( pcsUI, &lIndex ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex ) )
	{
		RefreshControl( pcsControl, bForce, FALSE );
	}
}

VOID AgcmUIManager2::RefreshControl( AgcdUIControl *pcsControl, BOOL bForce, BOOL bTraverse, BOOL bUpdateList, INT32 lUserDataIndex )
{
	if( !pcsControl || pcsControl->m_lType == AcUIBase::TYPE_NONE || ( !pcsControl->m_bNeedRefresh && !bForce ) ) return;
	ASSERT( ( PVOID )pcsControl != ( PVOID )0xcdcdcdcd );

	INT32 lIndex = 0;
	INT32 lUserDataCount = 0;

	AgcdUIControl* pcsChildControl;
	AgcdUIUserData* pstUserData = GetControlUserData( pcsControl );

	if( lUserDataIndex != -1 )
	{
		pcsControl->m_lUserDataIndex = lUserDataIndex;
	}

	// Tree Control에서 Width를 맘대로 줄이기 때문에 일단 Refresh할때마다 원래 크기로 돌린다.
	if( pcsControl->m_pcsFormatControl )
	{
		pcsControl->m_pcsBase->w = pcsControl->m_pcsFormatControl->m_pcsBase->w;
	}

	SetControlDisplayFont( pcsControl, 
							pcsControl->m_stDisplayMap.m_stFont.m_lType, 
							pcsControl->m_stDisplayMap.m_stFont.m_ulColor,
							pcsControl->m_stDisplayMap.m_stFont.m_fScale,
							pcsControl->m_stDisplayMap.m_stFont.m_bShadow,
							pcsControl->m_stDisplayMap.m_stFont.m_eHAlign,
							pcsControl->m_stDisplayMap.m_stFont.m_eVAlign,
							pcsControl->m_stDisplayMap.m_stFont.m_bImageNumber );

	if( pcsControl->m_stDisplayMap.m_eType == AGCDUI_DISPLAYMAP_TYPE_VARIABLE )
	{
		SetControlDisplayMap( pcsControl,
								pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_pstUserData,
								pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_pstDisplay,
								pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_bControlUserData );
	}
	else if( pcsControl->m_stDisplayMap.m_eType == AGCDUI_DISPLAYMAP_TYPE_STATIC )
	{
		SetControlDisplayMap( pcsControl, NULL );
	}

	if( pstUserData )
	{
		lUserDataCount = pstUserData->m_stUserData.m_lCount;
	}

	switch( pcsControl->m_lType )
	{
	case AcUIBase::TYPE_TREE :
	case AcUIBase::TYPE_LIST :
		{
			if( bUpdateList )
			{
				AcUIList* pcsUIList = ( AcUIList* )pcsControl->m_pcsBase;
				pcsUIList->SetListItemWindowTotalNum( pstUserData ? pstUserData->m_stUserData.m_lCount : 0 );

				if( pstUserData && pstUserData->m_lStartIndex != -1 )
				{
					pcsUIList->SetListItemWindowStartRow( pstUserData->m_lStartIndex / pcsUIList->m_lItemColumn, TRUE );
				}

				if( pcsControl->m_lType == AcUIBase::TYPE_TREE && pstUserData && pcsControl->m_uoData.m_stTree.m_pstDepthUserData && pcsControl->m_uoData.m_stTree.m_pstDepthUserData->m_eType == AGCDUI_USERDATA_TYPE_INT32 )
				{
					for( lIndex = 0 ; lIndex < lUserDataCount ; ++lIndex )
					{
						if( lIndex > pcsControl->m_uoData.m_stTree.m_pstDepthUserData->m_stUserData.m_lCount ) break;
						( ( AcUITree* )pcsControl->m_pcsBase )->SetItemDepth( lIndex, ( ( INT32* )pcsControl->m_uoData.m_stTree.m_pstDepthUserData->m_stUserData.m_pvData )[ lIndex ] );
					}
				}

				pcsUIList->SetListItemWindowStartRow( pcsUIList->m_lListItemStartRow, TRUE );
				if( pstUserData )
				{
					pcsUIList->SelectItem( pstUserData->m_lSelectedIndex );
				}
			}
			break;
		}

	case AcUIBase::TYPE_GRID :
	case AcUIBase::TYPE_SKILL_TREE :
		{
			AgcdUIUserData* pstUDGrid = NULL;
			if( pcsControl->m_uoData.m_stGrid.m_bControlUserData )
			{
				pstUDGrid = GetControlUserData( pcsControl );
			}
			else if( pcsControl->m_uoData.m_stGrid.m_pstGrid )
			{
				pstUDGrid = pcsControl->m_uoData.m_stGrid.m_pstGrid;
			}

			SetControlGrid( pcsControl, pstUDGrid, pcsControl->m_uoData.m_stGrid.m_bControlUserData );
			if( pstUDGrid && pstUDGrid->m_lSelectedIndex >= 0 )
			{
				( ( AcUIGrid* )pcsControl->m_pcsBase )->SetNowLayer( pstUDGrid->m_lSelectedIndex );
			}

			break;
		}

	case AcUIBase::TYPE_BAR :
		{
			if( !pcsControl->m_uoData.m_stBar.m_pstMax || !pcsControl->m_uoData.m_stBar.m_pstCurrent ||
				!pcsControl->m_uoData.m_stBar.m_pstDisplayMax || !pcsControl->m_uoData.m_stBar.m_pstDisplayCurrent )
			{
				( ( AcUIBar* )pcsControl->m_pcsBase )->SetPointInfo( 1, 0 );
			}
			else
			{
				INT32 lCurrent = 0;
				INT32 lMax = 1;

				CHAR szTemp[ 128 ] = { 0, };

				EnumDisplayCallback( pcsControl->m_uoData.m_stBar.m_pstDisplayMax, pcsControl->m_uoData.m_stBar.m_pstMax,
										szTemp, &lMax, pcsControl->m_lUserDataIndex, pcsControl );
				EnumDisplayCallback( pcsControl->m_uoData.m_stBar.m_pstDisplayCurrent, pcsControl->m_uoData.m_stBar.m_pstCurrent,
										szTemp,	&lCurrent, pcsControl->m_lUserDataIndex, pcsControl );

				( ( AcUIBar* )pcsControl->m_pcsBase )->SetPointInfo( lMax, lCurrent );
			}

			break;
		}

	case AcUIBase::TYPE_SCROLL :
		{
			if( !pcsControl->m_uoData.m_stScroll.m_pstMax || !pcsControl->m_uoData.m_stScroll.m_pstCurrent ||
				!pcsControl->m_uoData.m_stScroll.m_pstDisplayMax || !pcsControl->m_uoData.m_stScroll.m_pstDisplayCurrent )
				;
			else
			{
				INT32 lCurrent = 0;
				INT32 lMax = 1;

				CHAR szTemp[ 128 ] = { 0, };

				EnumDisplayCallback( pcsControl->m_uoData.m_stScroll.m_pstDisplayMax, pcsControl->m_uoData.m_stScroll.m_pstMax,
										szTemp,	&lMax, pcsControl->m_lUserDataIndex, pcsControl );
				EnumDisplayCallback( pcsControl->m_uoData.m_stScroll.m_pstDisplayCurrent, pcsControl->m_uoData.m_stScroll.m_pstCurrent,
										szTemp,	&lCurrent, pcsControl->m_lUserDataIndex, pcsControl );

				( ( AcUIScroll* )pcsControl->m_pcsBase )->SetScrollValue( lCurrent / ( FLOAT )lMax );
			}
			break;
		}

	case AcUIBase::TYPE_BUTTON :
		{
			AgcdUIBoolean* pstBoolean = pcsControl->m_uoData.m_stButton.m_pstEnableBoolean;
			AgcdUIUserData* pstUserData;
			PVOID pvData;

			INT32 lIndex = pcsControl->m_lUserDataIndex;
			if( pstBoolean )
			{
				pvData = NULL;
				pstUserData = pcsControl->m_uoData.m_stButton.m_pstEnableUserData;

				if( pstUserData )
				{
					if( lIndex > pstUserData->m_stUserData.m_lCount )
					{
						pvData = pstUserData->m_stUserData.m_pvData;
					}
					else
					{
						pvData = ( ( CHAR* )pstUserData->m_stUserData.m_pvData ) + pstUserData->m_stUserData.m_lDataSize * lIndex;
					}
				}

				if( pstBoolean->m_fnCallback( pstBoolean->m_pvClass, pvData, pstUserData ? pstUserData->m_eType : AGCDUI_USERDATA_TYPE_NONE, pcsControl ) )
				{
					( ( AcUIButton* )pcsControl->m_pcsBase )->SetButtonMode( ACUIBUTTON_MODE_NORMAL );
				}
				else
				{
					( ( AcUIButton* )pcsControl->m_pcsBase )->SetButtonMode( ACUIBUTTON_MODE_DISABLE );
				}
			}

			pstBoolean = pcsControl->m_uoData.m_stButton.m_pstCheckBoolean;

			if( ( ( AcUIButton* )pcsControl->m_pcsBase )->GetButtonMode() != ACUIBUTTON_MODE_DISABLE )
			{
				if( pstBoolean )
				{
					pvData = NULL;
					pstUserData = pcsControl->m_uoData.m_stButton.m_pstCheckUserData;

					if( pstUserData )
					{
						if( lIndex > pstUserData->m_stUserData.m_lCount )
						{
							pvData = pstUserData->m_stUserData.m_pvData;
						}
						else
						{
							pvData = ( ( CHAR* )pstUserData->m_stUserData.m_pvData ) + pstUserData->m_stUserData.m_lDataSize * lIndex;
						}
					}

					if( pstBoolean->m_fnCallback( pstBoolean->m_pvClass, pvData, pstUserData ? pstUserData->m_eType : AGCDUI_USERDATA_TYPE_NONE, pcsControl ) )
					{
						( ( AcUIButton* )pcsControl->m_pcsBase )->SetButtonMode( ACUIBUTTON_MODE_CLICK, FALSE );
					}
					else
					{
						( ( AcUIButton* )pcsControl->m_pcsBase )->SetButtonMode( ACUIBUTTON_MODE_NORMAL, FALSE );
					}
				}
				else if( pcsControl->m_uoData.m_stButton.m_pstCheckUserData )
				{
					if( pcsControl->m_uoData.m_stButton.m_pstCheckUserData->m_lSelectedIndex == pcsControl->m_lUserDataIndex )
					{
						( ( AcUIButton* )pcsControl->m_pcsBase )->SetButtonMode( ACUIBUTTON_MODE_CLICK, FALSE );
					}
					else
					{
						( ( AcUIButton* )pcsControl->m_pcsBase )->SetButtonMode( ACUIBUTTON_MODE_NORMAL, FALSE );
					}
				}
			}
			break;
		}

	case AcUIBase::TYPE_COMBO :
		{
			if( pcsControl->m_stDisplayMap.m_eType == AGCDUI_DISPLAYMAP_TYPE_VARIABLE )
			{
				AcUICombo* pcsCombo	= ( AcUICombo* )pcsControl->m_pcsBase;
				AgcdUIUserData* pstUserData	= pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_pstUserData;
				AgcdUIDisplay* pstDisplay = pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_pstDisplay;

				INT32 lIndex = 0;
				INT32 lValue;
				CHAR szDisplay[ 128 ] = { 0, };				

				if( pstUserData )
				{
					for( lIndex = 0 ; lIndex < pstUserData->m_stUserData.m_lCount ; ++lIndex )
					{
						if( EnumDisplayCallback( pstDisplay, pstUserData, szDisplay, &lValue, lIndex, pcsControl ) )
						{
							pcsCombo->AddString( szDisplay, lIndex, lIndex == 0 );
						}
					}

					pcsCombo->SelectIndex( pstUserData->m_lSelectedIndex );
				}
			}

			break;
		}

	case AcUIBase::TYPE_CLOCK :
		{
			if( !pcsControl->m_uoData.m_stClock.m_pstMax || !pcsControl->m_uoData.m_stClock.m_pstCurrent ||
				!pcsControl->m_uoData.m_stClock.m_pstDisplayMax || !pcsControl->m_uoData.m_stClock.m_pstDisplayCurrent )
			{
				( ( AcUIClock* )pcsControl->m_pcsBase )->SetPointInfo( 1, 0 );
			}
			else
			{
				INT32 lCurrent = 0;
				INT32 lMax = 1;
				CHAR szTemp[ 128 ] = { 0, };

				EnumDisplayCallback( pcsControl->m_uoData.m_stClock.m_pstDisplayMax, pcsControl->m_uoData.m_stClock.m_pstMax,
										szTemp,	&lMax, pcsControl->m_lUserDataIndex, pcsControl );
				EnumDisplayCallback( pcsControl->m_uoData.m_stClock.m_pstDisplayCurrent, pcsControl->m_uoData.m_stClock.m_pstCurrent,
										szTemp, &lCurrent, pcsControl->m_lUserDataIndex, pcsControl );

				( ( AcUIClock* )pcsControl->m_pcsBase )->SetPointInfo( lMax, lCurrent );
			}

			break;
		}
	}

	if( pcsControl->m_pstShowCB && pcsControl->m_pstShowCB->m_fnCallback )
	{
		if( !pcsControl->m_pstShowCB->m_fnCallback( pcsControl->m_pstShowCB->m_pvClass,
													pcsControl->m_pstShowUD ? pcsControl->m_pstShowUD->m_stUserData.m_pvData : NULL,
													pcsControl->m_pstShowUD ? pcsControl->m_pstShowUD->m_eType : AGCDUI_USERDATA_TYPE_NONE,
													pcsControl ) )
		{
			pcsControl->m_pcsBase->m_Property.bVisible = FALSE;
		}
		else
		{
			pcsControl->m_pcsBase->m_Property.bVisible = TRUE;
		}
	}

	// 여기에  UpdateChildWindow() 넣었더니, 리스트 꼬이더라.
	pcsControl->m_pcsBase->m_bNeedWindowListUpdate = true;
	pcsControl->m_pcsBase->UpdateChildWindow();
	pcsControl->m_bNeedRefresh = FALSE;

	if( bTraverse )
	{
		lIndex = 0;
		for( pcsChildControl = GetSequenceControl( pcsControl, &lIndex ) ; pcsChildControl ; pcsChildControl = GetSequenceControl( pcsControl, &lIndex ) )
		{
			RefreshControl( pcsChildControl, bForce, bTraverse, bUpdateList, lUserDataIndex );
		}
	}
}

//========================================================

AgcdUIFunction*	AgcmUIManager2::AddFunction( PVOID pvClass, CHAR *szName, AgcUICallBack fnCallBack, INT32 lArgNum, CHAR *szArgDesc1, CHAR *szArgDesc2, CHAR *szArgDesc3, CHAR *szArgDesc4, CHAR *szArgDesc5 )
{
	AgcdUIFunction* pstFunction = CreateFunction();
	if( !pstFunction ) return NULL;

	pstFunction->m_pClass = pvClass;
	pstFunction->m_szName = szName;
	pstFunction->m_fnCallback = fnCallBack;
	pstFunction->m_lNumData = lArgNum;
	pstFunction->m_aszData[ 0 ] = szArgDesc1;
	pstFunction->m_aszData[ 1 ] = szArgDesc2;
	pstFunction->m_aszData[ 2 ] = szArgDesc3;
	pstFunction->m_aszData[ 3 ] = szArgDesc4;
	pstFunction->m_aszData[ 4 ] = szArgDesc5;

	if( AddFunction( pstFunction ) < 0 )
	{
		DestroyFunction( pstFunction );
		return NULL;
	}

	return pstFunction;
}

AgcdUIUserData* AgcmUIManager2::AddUserData( CHAR *szName, PVOID pvData, INT32 lDataSize, INT32 lDataCount, AgcdUIDataType eDataType )
{
	AgcdUIUserData* pstUserData = CreateUserData();
	if( !pstUserData ) return NULL;

	pstUserData->m_szName = szName;
	pstUserData->m_stUserData.m_pvData = pvData;
	pstUserData->m_stUserData.m_lDataSize = lDataSize;
	pstUserData->m_stUserData.m_lCount = lDataCount;
	pstUserData->m_eType = eDataType;

	if( AddUserData( pstUserData ) < 0 )
	{
		DestroyUserData( pstUserData );
		return NULL;
	}

	return pstUserData;
}

AgcdUIDisplay* AgcmUIManager2::AddDisplay( PVOID pvClass, CHAR *szName, INT32 lID, AgcUIDisplayCB fnCallBack, UINT32 ulDataType )
{
	AgcdUIDisplay* pstDisplay = CreateDisplay();
	if( !pstDisplay ) return NULL;

	pstDisplay->m_pvClass = pvClass;
	pstDisplay->m_szName = szName;
	pstDisplay->m_lID = lID;
	pstDisplay->m_fnCallback = fnCallBack;
	pstDisplay->m_fnOldCallback = NULL;
	pstDisplay->m_ulDataType = ulDataType;

	if( AddDisplay( pstDisplay ) < 0 )
	{
		DestroyDisplay( pstDisplay );
		return NULL;
	}

	return pstDisplay;
}

AgcdUIDisplay* AgcmUIManager2::AddDisplay( PVOID pvClass, CHAR *szName, INT32 lID, AgcUIDisplayOldCB fnCallBack, UINT32 ulDataType )
{
	AgcdUIDisplay* pstDisplay = CreateDisplay();
	if( !pstDisplay ) return NULL;

	pstDisplay->m_pvClass = pvClass;
	pstDisplay->m_szName = szName;
	pstDisplay->m_lID = lID;
	pstDisplay->m_fnCallback = NULL;
	pstDisplay->m_fnOldCallback = fnCallBack;
	pstDisplay->m_ulDataType = ulDataType;

	if( AddDisplay(pstDisplay) < 0 )
	{
		DestroyDisplay( pstDisplay );
		return NULL;
	}

	return pstDisplay;
}

AgcdUIBoolean* AgcmUIManager2::AddBoolean( PVOID pvClass, CHAR *szName, AgcUIBooleanCB fnCallBack, UINT32 ulDataType )
{
	AgcdUIBoolean* pstBoolean = CreateBoolean();
	if( !pstBoolean ) return NULL;

	pstBoolean->m_pvClass = pvClass;
	pstBoolean->m_szName = szName;
	pstBoolean->m_fnCallback = fnCallBack;
	pstBoolean->m_ulDataType = ulDataType;

	if( AddBoolean(pstBoolean) < 0 )
	{
		DestroyBoolean( pstBoolean );
		return NULL;
	}

	return pstBoolean;
}

AgcdUICControl* AgcmUIManager2::AddCustomControl( CHAR *szName, AcUIBase *pcsBase )
{
	if( m_lMaxUICControlID >= m_lMaxUICControls ) return NULL;
	if( !szName[ 0 ] ) return NULL;

	AgcdUICControl* pstCControl = new AgcdUICControl;
	if( !pstCControl ) return NULL;

	pstCControl->m_szName = szName;
	pstCControl->m_pcsBase = pcsBase;

	++m_lMaxUICControlID;
	if( !m_csUICControls.AddObject( &pstCControl, m_lMaxUICControlID, pstCControl->m_szName ) )
	{
		delete pstCControl;
		return NULL;
	}

	return pstCControl;
}

AgcdUIFunction* AgcmUIManager2::CreateFunction( void )
{
	AgcdUIFunction* pstFunction = new AgcdUIFunction;
	if( !pstFunction ) return NULL;
	return pstFunction;
}

BOOL AgcmUIManager2::DestroyFunction( AgcdUIFunction *pstFunction )
{
	if( !pstFunction ) return FALSE;
	delete pstFunction;
	return TRUE;
}

INT32 AgcmUIManager2::AddFunction( AgcdUIFunction *pstFunction )
{
	if( m_lMaxUIFunctionID >= m_lMaxUIFunctions ) return -1;
	if( !pstFunction->m_szName[ 0 ] ) return -1;

	++m_lMaxUIFunctionID;
	if( !m_csUIFunctions.AddObject( &pstFunction, m_lMaxUIFunctionID, pstFunction->m_szName ) )	return -1;
	return m_lMaxUIFunctionID;
}

AgcdUIFunction* AgcmUIManager2::GetFunction( INT32 lIndex )
{
	if( lIndex < 0 || lIndex >= m_lMaxUIFunctions ) return NULL;

	AgcdUIFunction **ppstFunction = ( AgcdUIFunction** )m_csUIFunctions.GetObject( lIndex );
	if( !ppstFunction ) return NULL;
	return *ppstFunction;
}

AgcdUIFunction* AgcmUIManager2::GetFunction( CHAR *szName )
{
	AgcdUIFunction** ppstFunction = ( AgcdUIFunction** )m_csUIFunctions.GetObject( szName );
	if( !ppstFunction ) return NULL;
	return *ppstFunction;
}

VOID AgcmUIManager2::RemoveAllFunction( void )
{
	INT32 lIndex = 0;
	AgcdUIFunction* pstFunction;

	for( lIndex = 1 ; lIndex <= m_lMaxUIFunctions ; ++lIndex )
	{
		pstFunction = GetFunction( lIndex );
		if( pstFunction )
		{
			DestroyFunction( pstFunction );
		}
	}

	m_csUIFunctions.RemoveObjectAll();
	m_lMaxUIFunctionID = 0;
}

AgcdUIUserData* AgcmUIManager2::CreateUserData( void )
{
	AgcdUIUserData* pstUserData = new AgcdUIUserData;
	if( !pstUserData ) return NULL;

	pstUserData->m_lSelectedIndex = -1;
	pstUserData->m_lStartIndex = -1;

	return pstUserData;
}

BOOL AgcmUIManager2::DestroyUserData( AgcdUIUserData *pstUserData )
{
	if( !pstUserData ) return FALSE;
	delete pstUserData;
	return TRUE;
}

INT32 AgcmUIManager2::AddUserData( AgcdUIUserData *pstUserData )
{
	if( m_lMaxUIUserDataID >= m_lMaxUIUserData ) return -1;
	if( !pstUserData->m_szName[ 0 ] ) return -1;

	++m_lMaxUIUserDataID;
	if( !m_csUIUserData.AddObject( &pstUserData, m_lMaxUIUserDataID, pstUserData->m_szName ) ) return -1;
	return m_lMaxUIUserData;
}

AgcdUIUserData* AgcmUIManager2::GetUserData( INT32 lIndex )
{
	if( lIndex < 0 || lIndex >= m_lMaxUIUserData ) return NULL;

	AgcdUIUserData** ppstUserData = ( AgcdUIUserData**)m_csUIUserData.GetObject( lIndex );
	if( !ppstUserData ) return NULL;
	return *ppstUserData;
}

AgcdUIUserData* AgcmUIManager2::GetUserData( CHAR *szName )
{
	AgcdUIUserData** ppstUserData = ( AgcdUIUserData** )m_csUIUserData.GetObject( szName );
	if( !ppstUserData )	return NULL;
	return *ppstUserData;
}

VOID AgcmUIManager2::RemoveAllUserData( void )
{
	INT32 lIndex = 0;
	AgcdUIUserData* pstUserData;

	for( lIndex = 1 ; lIndex <= m_lMaxUIUserData ; ++lIndex )
	{
		pstUserData = GetUserData( lIndex );
		if( pstUserData )
		{
			DestroyUserData( pstUserData );
		}
	}

	m_csUIUserData.RemoveObjectAll();
	m_lMaxUIUserData = 0;
}

AgcdUIUserData* AgcmUIManager2::GetControlUserData( AgcdUIControl *pcsControl )
{
	if( !pcsControl ) return NULL;

	while( pcsControl->m_bUseParentUserData && pcsControl->m_pcsParentControl )
	{
		pcsControl = pcsControl->m_pcsParentControl;
	}

	return pcsControl->m_pstUserData;
}

VOID AgcmUIManager2::CountUserData( void )
{
	INT32 lIndex;
	INT32 lIndex2;
	AgcdUI* pcsUI;
	AgcdUIControl* pcsControl;
	AgcdUIUserData* pstUserData;

	for( lIndex = 1 ; lIndex <= GetUserDataCount() ; ++lIndex )
	{
		pstUserData = GetUserData( lIndex );
		if( !pstUserData ) continue;

		if( pstUserData->m_ppControls )
		{
			free( pstUserData->m_ppControls );

			pstUserData->m_lControl = 0;
			pstUserData->m_ppControls = NULL;
		}
	}

	lIndex = 0;
	for( pcsUI = GetSequenceUI( &lIndex ) ; pcsUI ; pcsUI = GetSequenceUI( &lIndex ) )
	{
		lIndex2 = 0;
		for( pcsControl = GetSequenceControl( pcsUI, &lIndex2 ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex2 ) )
		{
			CountUserData( pcsControl, FALSE );
		}
	}
}

VOID AgcmUIManager2::CountUserData( AgcdUIUserData *pstUserData, AgcdUIControl *pcsControl, BOOL bUncount )
{
	if( pstUserData )
	{
		INT32 lIndex;
		if( bUncount )
		{
			for( lIndex = 0 ; lIndex < pstUserData->m_lControl ; ++lIndex )
			{
				if( pstUserData->m_ppControls[ lIndex ] == pcsControl )
				{
					pstUserData->m_ppControls[ lIndex ] = NULL;
					break;
				}
			}
		}
		else
		{
			if( !pstUserData->m_ppControls )
			{
				++( pstUserData->m_lControl );
				pstUserData->m_ppControls = ( AgcdUIControl** )malloc( sizeof( AgcdUIControl* ) * pstUserData->m_lControl );

				memset( pstUserData->m_ppControls, 0, sizeof( AgcdUIControl* ) * pstUserData->m_lControl );
				pstUserData->m_ppControls[ 0 ] = pcsControl;
			}
			else
			{
				for( lIndex = 0; lIndex < pstUserData->m_lControl; ++lIndex )
				{
					// 같은 Control이 이미 있으면, break
					if( pstUserData->m_ppControls[ lIndex ] == pcsControl ) break;
				}

				// 같은 Control이 없다. 그러면, 빈곳을 찾자.
				if( lIndex == pstUserData->m_lControl )
				{
					for( lIndex = 0 ; lIndex < pstUserData->m_lControl ; ++lIndex )
					{
						// 빈곳이 있으면 여기에 Insert
						if( !pstUserData->m_ppControls[ lIndex ] )
						{
							pstUserData->m_ppControls[ lIndex ] = pcsControl;
							break;
						}
					}

					// 빈곳이 없다. 그러면, realloc
					if( lIndex == pstUserData->m_lControl )
					{
						++( pstUserData->m_lControl );
						pstUserData->m_ppControls = ( AgcdUIControl** )realloc( pstUserData->m_ppControls, sizeof( AgcdUIControl* ) * pstUserData->m_lControl );

						if( pstUserData->m_ppControls )
						{
							pstUserData->m_ppControls[ pstUserData->m_lControl - 1 ] = pcsControl;
						}
					}
				}
			}
		}
	}
}

VOID AgcmUIManager2::CountUserData( AgcdUIControl *pcsControl, BOOL bTraverse, BOOL bUncount )
{
	INT32 lIndex;
	AgcdUIControl* pcsChildControl;

	if( GetControlUserData( pcsControl ) )
	{
		CountUserData( GetControlUserData( pcsControl ), pcsControl, bUncount );
	}

	if( pcsControl->m_stDisplayMap.m_eType == AGCDUI_DISPLAYMAP_TYPE_VARIABLE )
	{
		CountUserData( pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_pstUserData, pcsControl, bUncount );
	}

	CountUserData( pcsControl->m_pstShowUD, pcsControl, bUncount );

	switch( pcsControl->m_lType )
	{
	case AcUIBase::TYPE_BAR :
		{
			CountUserData( pcsControl->m_uoData.m_stBar.m_pstMax, pcsControl, bUncount );
			CountUserData( pcsControl->m_uoData.m_stBar.m_pstCurrent, pcsControl, bUncount );
			break;
		}

	case AcUIBase::TYPE_GRID :
	case AcUIBase::TYPE_SKILL_TREE :
		{
			CountUserData( pcsControl->m_uoData.m_stGrid.m_pstGrid, pcsControl, bUncount );
			break;
		}

	case AcUIBase::TYPE_SCROLL :
		{
			CountUserData( pcsControl->m_uoData.m_stScroll.m_pstMax, pcsControl, bUncount );
			CountUserData( pcsControl->m_uoData.m_stScroll.m_pstCurrent, pcsControl, bUncount );
			break;
		}

	case AcUIBase::TYPE_BUTTON :
		{
			CountUserData( pcsControl->m_uoData.m_stButton.m_pstCheckUserData, pcsControl, bUncount );
			CountUserData( pcsControl->m_uoData.m_stButton.m_pstEnableUserData, pcsControl, bUncount );
			break;
		}

	case AcUIBase::TYPE_CLOCK :
		{
			CountUserData( pcsControl->m_uoData.m_stClock.m_pstMax, pcsControl, bUncount );
			CountUserData( pcsControl->m_uoData.m_stClock.m_pstCurrent, pcsControl, bUncount );
			break;
		}
	}

	for( lIndex = 0 ; lIndex < ACUIBASE_STATUS_MAX_NUM ; ++lIndex )
	{
		if( pcsControl->m_apstUserData[ lIndex ] )
		{
			CountUserData( pcsControl->m_apstUserData[ lIndex ], pcsControl, bUncount );
		}
	}

	if( bTraverse )
	{
		lIndex = 0;
		for( pcsChildControl = GetSequenceControl( pcsControl, &lIndex ) ; pcsChildControl ; pcsChildControl = GetSequenceControl( pcsControl, &lIndex ) )
		{
			CountUserData( pcsChildControl, bTraverse, bUncount );
		}
	}
}

BOOL AgcmUIManager2::SetUserDataRefresh( AgcdUIUserData *pstUserData, BOOL bUpdateList )
{
	if( m_eModuleStatus != APMODULE_STATUS_READY ) return TRUE;

	// Refresh도 바로 하게 해보자.... 문제 생기면 그때 빼도 된다.
	pstUserData->m_bUpdateList |= bUpdateList;
	RefreshUserData( pstUserData, TRUE );

	// 킁;; 이게 먼가.. ㅡ.ㅡ;; 여기서 리턴이면 아래 코드는;;;..
	// 아래 코드는 어차피 실행되지 않을 코드니 일단 주석처리해두자..
	// 확인해보고 이상없으면... 아래 코드는 지우든가..
	return TRUE;

	//if (!pstUserData || m_lRefreshUserData >= AGCMUIMANAGER2_MAX_REFRESH_USER_DATA)
	//	return FALSE;

	//if (pstUserData->m_bNeedRefresh)
	//{
	//	pstUserData->m_bUpdateList |= bUpdateList;

	//	return TRUE;
	//}

	//INT32	lIndex;

	//m_apstRefreshUserData[m_lRefreshUserData] = pstUserData;
	//pstUserData->m_bNeedRefresh = TRUE;
	//pstUserData->m_bUpdateList	= bUpdateList;

	//++m_lRefreshUserData;

	//if (pstUserData->m_ppControls)
	//{
	//	for (lIndex = 0; lIndex < pstUserData->m_lControl; ++lIndex)
	//	{
	//		if (pstUserData->m_ppControls[lIndex])
	//			pstUserData->m_ppControls[lIndex]->m_bNeedRefresh = TRUE;
	//	}
	//}

	//return TRUE;
}

VOID AgcmUIManager2::RefreshUserData( AgcdUIUserData *pstUserData, BOOL bForce )
{
	if( !pstUserData || ( !pstUserData->m_bNeedRefresh && !bForce ) ) return;

	for( INT32 lIndex = 0 ; lIndex < pstUserData->m_lControl ; ++lIndex )
	{
		RefreshControl( pstUserData->m_ppControls[ lIndex ], bForce, TRUE, pstUserData->m_bUpdateList );
	}

	pstUserData->m_bNeedRefresh = FALSE;
}

AgcdUIDisplay* AgcmUIManager2::CreateDisplay( void )
{
	AgcdUIDisplay* pstDisplay = new AgcdUIDisplay;
	if( !pstDisplay ) return NULL;
	return pstDisplay;
}

BOOL AgcmUIManager2::DestroyDisplay( AgcdUIDisplay *pstDisplay )
{
	if( !pstDisplay ) return FALSE;
	delete pstDisplay;
	return TRUE;
}

INT32 AgcmUIManager2::AddDisplay( AgcdUIDisplay *pstDisplay )
{
	if( m_lMaxUIDisplayID >= m_lMaxUIDisplays ) return -1;
	if( !pstDisplay->m_szName[ 0 ] ) return -1;

	++m_lMaxUIDisplayID;
	if( !m_csUIDisplays.AddObject( &pstDisplay, m_lMaxUIDisplayID, pstDisplay->m_szName ) )	return -1;
	return m_lMaxUIDisplayID;
}

AgcdUIDisplay *AgcmUIManager2::GetDisplay( INT32 lIndex )
{
	if( lIndex < 0 || lIndex >= m_lMaxUIDisplays ) return NULL;

	AgcdUIDisplay** ppstDisplay = ( AgcdUIDisplay** )m_csUIDisplays.GetObject( lIndex );
	if( !ppstDisplay ) return NULL;

	return *ppstDisplay;
}

AgcdUIDisplay *AgcmUIManager2::GetDisplay( CHAR *szName )
{
	AgcdUIDisplay** ppstDisplay = ( AgcdUIDisplay** )m_csUIDisplays.GetObject( szName );
	if( !ppstDisplay ) return NULL;

	return *ppstDisplay;
}

VOID AgcmUIManager2::RemoveAllDisplay( void )
{
	for( INT32 lIndex = 1 ; lIndex <= m_lMaxUIDisplays ; ++lIndex )
	{
		AgcdUIDisplay* pstDisplay = GetDisplay( lIndex );
		if( pstDisplay )
		{
			DestroyDisplay( pstDisplay );
		}
	}

	m_csUIDisplays.RemoveObjectAll();
	m_lMaxUIDisplays = 0;
}

AgcdUIBoolean* AgcmUIManager2::CreateBoolean( void )
{
	AgcdUIBoolean* pstBoolean = new AgcdUIBoolean;
	if( !pstBoolean ) return NULL;
	return pstBoolean;
}

BOOL AgcmUIManager2::DestroyBoolean( AgcdUIBoolean *pstBoolean )
{
	if( !pstBoolean ) return FALSE;
	delete pstBoolean;
	return TRUE;
}

INT32 AgcmUIManager2::AddBoolean( AgcdUIBoolean *pstBoolean )
{
	if( m_lMaxUIBooleanID >= m_lMaxUIBooleans ) return -1;
	if( !pstBoolean->m_szName[ 0 ] ) return -1;

	++m_lMaxUIBooleanID;
	if( !m_csUIBooleans.AddObject( &pstBoolean, m_lMaxUIBooleanID, pstBoolean->m_szName ) )	return -1;
	return m_lMaxUIBooleanID;
}

AgcdUIBoolean* AgcmUIManager2::GetBoolean( INT32 lIndex )
{
	if( lIndex < 0 || lIndex >= m_lMaxUIBooleans ) return NULL;

	AgcdUIBoolean** ppstBoolean = ( AgcdUIBoolean** )m_csUIBooleans.GetObject( lIndex );
	if( !ppstBoolean ) return NULL;

	return *ppstBoolean;
}

AgcdUIBoolean* AgcmUIManager2::GetBoolean( CHAR *szName )
{
	AgcdUIBoolean** ppstBoolean = ( AgcdUIBoolean** )m_csUIBooleans.GetObject( szName );
	if( !ppstBoolean ) return NULL;
	return *ppstBoolean;
}

VOID AgcmUIManager2::RemoveAllBoolean( void )
{
	for( INT32 lIndex = 1 ; lIndex <= m_lMaxUIBooleans ; ++lIndex )
	{
		AgcdUIBoolean* pstBoolean = GetBoolean( lIndex );
		if( pstBoolean )
		{
			DestroyBoolean( pstBoolean );
		}
	}

	m_csUIBooleans.RemoveObjectAll();
	m_lMaxUIBooleans = 0;
}

AgcdUICControl* AgcmUIManager2::GetCControl( INT32 lIndex )
{
	if( lIndex < 0 || lIndex >= m_lMaxUICControls )	return NULL;

	AgcdUICControl** ppstCControl = ( AgcdUICControl** )m_csUICControls.GetObject( lIndex );
	if( !ppstCControl )	return NULL;

	return *ppstCControl;
}

AgcdUICControl* AgcmUIManager2::GetCControl( CHAR *szName )
{
	AgcdUICControl** ppstCControl = ( AgcdUICControl** )m_csUICControls.GetObject( szName );
	if( !ppstCControl ) return NULL;
	return *ppstCControl;
}

VOID AgcmUIManager2::RemoveAllCControl( void )
{
	for( INT32 lIndex = 1 ; lIndex <= m_lMaxUICControls ; ++lIndex )
	{
		AgcdUICControl* pstCControl = GetCControl( lIndex );
		if( pstCControl )
		{
			delete pstCControl;
			pstCControl = NULL;
		}
	}

	m_csUICControls.RemoveObjectAll();
	m_lMaxUICControls = 0;
}

BOOL AgcmUIManager2::EnumDisplayCallback( AgcdUIDisplay *pstDisplay, AgcdUIUserData *pstUserData, CHAR *szDisplay, INT32 *plValue, INT32 lIndex, AgcdUIControl *pcsSourceControl )
{
	if( !pstDisplay ) return FALSE;
	if( !pstUserData )
	{
		if( pstDisplay->m_fnCallback )
		{
			return pstDisplay->m_fnCallback( pstDisplay->m_pvClass, NULL, AGCDUI_USERDATA_TYPE_NONE, pstDisplay->m_lID,
												szDisplay, plValue, pcsSourceControl );
		}
		else if( pstDisplay->m_fnOldCallback )
		{
			return pstDisplay->m_fnOldCallback( pstDisplay->m_pvClass, NULL, AGCDUI_USERDATA_TYPE_NONE, pstDisplay->m_lID,
												szDisplay, plValue );
		}
	}

	if( !( pstDisplay->m_ulDataType & pstUserData->m_eType ) ) return FALSE;

	PVOID pvData = NULL;
	if( lIndex > pstUserData->m_stUserData.m_lCount )
	{
		pvData = pstUserData->m_stUserData.m_pvData;
	}
	else
	{
		pvData = ( ( CHAR* )pstUserData->m_stUserData.m_pvData ) + pstUserData->m_stUserData.m_lDataSize * lIndex;
	}

	if( pstDisplay->m_fnCallback )
	{
		return pstDisplay->m_fnCallback( pstDisplay->m_pvClass, pvData, 
											pstUserData ? pstUserData->m_eType : AGCDUI_USERDATA_TYPE_NONE,
											pstDisplay->m_lID, szDisplay, plValue, pcsSourceControl );
	}
	else if( pstDisplay->m_fnOldCallback )
	{
		return pstDisplay->m_fnOldCallback( pstDisplay->m_pvClass, pvData, 
											pstUserData ? pstUserData->m_eType : AGCDUI_USERDATA_TYPE_NONE,
											pstDisplay->m_lID, szDisplay, plValue );
	}

	return TRUE;
}

BOOL AgcmUIManager2::ResetControlDisplayMap( AgcdUIControl *pcsControl )
{
	if( pcsControl->m_stDisplayMap.m_eType == AGCDUI_DISPLAYMAP_TYPE_STATIC && pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic )
	{
		delete[] pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic;
		pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic = NULL;
	}

	pcsControl->m_stDisplayMap.m_eType = AGCDUI_DISPLAYMAP_TYPE_NONE;
	pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_pstUserData = NULL;
	pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_pstDisplay = NULL;

	pcsControl->m_pcsBase->SetStaticString( "" );
	return TRUE;
}

BOOL AgcmUIManager2::SetControlDisplayMap( AgcdUIControl *pcsControl, CHAR *szDisplay )
{
	if( szDisplay )
	{
		ResetControlDisplayMap( pcsControl );
		pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic = new CHAR[ strlen( szDisplay ) + 1 ];
		strcpy( pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic, szDisplay );
	}

	pcsControl->m_stDisplayMap.m_eType = AGCDUI_DISPLAYMAP_TYPE_STATIC;
	if( pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic )
	{
		pcsControl->m_pcsBase->SetStaticStringExt(	pcsControl->m_stDisplayMap.m_uoDisplay.m_szStatic,
													pcsControl->m_stDisplayMap.m_stFont.m_fScale,
													pcsControl->m_stDisplayMap.m_stFont.m_lType,
													pcsControl->m_stDisplayMap.m_stFont.m_ulColor,
													pcsControl->m_stDisplayMap.m_stFont.m_bShadow,
													pcsControl->m_stDisplayMap.m_stFont.m_eHAlign,
													pcsControl->m_stDisplayMap.m_stFont.m_eVAlign,
													pcsControl->m_stDisplayMap.m_stFont.m_bImageNumber );
	}
	else
	{
		pcsControl->m_pcsBase->SetStaticString( "" );
	}

	return TRUE;
}

BOOL AgcmUIManager2::SetControlDisplayMap( AgcdUIControl *pcsControl, AgcdUIUserData *pstUserData, AgcdUIDisplay *pstDisplay, BOOL bControlUserData )
{
	if( IsBadReadPtr( pcsControl, sizeof( AgcdUIControl ) ) )
	{
		_asm nop;
	}

	INT32 lValue = 0;
	CHAR szDisplay[ 256 ] = { 0, };
	ZeroMemory( szDisplay, sizeof( CHAR ) * 256 );

	ResetControlDisplayMap( pcsControl );
	pcsControl->m_stDisplayMap.m_eType = AGCDUI_DISPLAYMAP_TYPE_VARIABLE;

	if( bControlUserData )
	{
		pstUserData = GetControlUserData( pcsControl );
	}

	pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_bControlUserData = bControlUserData;
	pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_pstUserData = pstUserData;
	pcsControl->m_stDisplayMap.m_uoDisplay.m_stVariable.m_pstDisplay = pstDisplay;

	if( !pstUserData )
	{
		pcsControl->m_pcsBase->SetUserData( NULL, 0, 0 );
	}
	else
	{
		pcsControl->m_pcsBase->SetUserData( pstUserData->m_stUserData.m_pvData, pstUserData->m_stUserData.m_lDataSize, pstUserData->m_stUserData.m_lCount );
	}

	if( EnumDisplayCallback( pstDisplay, pstUserData, szDisplay, &lValue, pcsControl->m_lUserDataIndex, pcsControl ) )
	{
		pcsControl->m_pcsBase->SetStaticStringExt( szDisplay,
													pcsControl->m_stDisplayMap.m_stFont.m_fScale,
													pcsControl->m_stDisplayMap.m_stFont.m_lType,
													pcsControl->m_stDisplayMap.m_stFont.m_ulColor,
													pcsControl->m_stDisplayMap.m_stFont.m_bShadow,
													pcsControl->m_stDisplayMap.m_stFont.m_eHAlign,
													pcsControl->m_stDisplayMap.m_stFont.m_eVAlign,
													pcsControl->m_stDisplayMap.m_stFont.m_bImageNumber );
	}
	else
	{
		pcsControl->m_pcsBase->SetStaticStringExt( NULL,
													pcsControl->m_stDisplayMap.m_stFont.m_fScale,
													pcsControl->m_stDisplayMap.m_stFont.m_lType,
													pcsControl->m_stDisplayMap.m_stFont.m_ulColor,
													pcsControl->m_stDisplayMap.m_stFont.m_bShadow,
													pcsControl->m_stDisplayMap.m_stFont.m_eHAlign,
													pcsControl->m_stDisplayMap.m_stFont.m_eVAlign,
													pcsControl->m_stDisplayMap.m_stFont.m_bImageNumber );
	}

	return TRUE;
}
 
BOOL AgcmUIManager2::SetControlDisplayFont( AgcdUIControl *pcsControl, INT32 lType, UINT32 ulColor, FLOAT fScale, bool bShadow, AcUIBaseHAlign eHAlign, AcUIBaseVAlign eVAlign, BOOL bImageNumber )
{
	if( !pcsControl ) return FALSE;

	pcsControl->m_stDisplayMap.m_stFont.m_lType			= lType;
	pcsControl->m_stDisplayMap.m_stFont.m_ulColor		= ulColor;
	pcsControl->m_stDisplayMap.m_stFont.m_fScale		= fScale;
	pcsControl->m_stDisplayMap.m_stFont.m_bShadow		= bShadow;
	pcsControl->m_stDisplayMap.m_stFont.m_eHAlign		= eHAlign;
	pcsControl->m_stDisplayMap.m_stFont.m_eVAlign		= eVAlign;
	pcsControl->m_stDisplayMap.m_stFont.m_bImageNumber	= bImageNumber;

	pcsControl->m_pcsBase->SetStaticStringExt( NULL, fScale, lType, ulColor, bShadow, eHAlign, eVAlign,	bImageNumber );
	return TRUE;
}

INT32 AgcmUIManager2::AddEvent( CHAR *szName, AgcUIEventReturnCB fnReturnCB, PVOID pClass )
{
	if( m_lMaxUIEventID >= m_lMaxUIEvents )	return -1;
	if( fnReturnCB && !pClass )	return -1;

	AgcdUIEventMap* pstEventMap = new AgcdUIEventMap;
	if( !pstEventMap ) return -1;

	pstEventMap->m_szName		= szName;
	pstEventMap->m_fnReturnCB	= fnReturnCB;
	pstEventMap->m_pCBClass		= pClass;

	++m_lMaxUIEventID;
	if( !m_csUIEvents.AddObject( &pstEventMap, m_lMaxUIEventID, szName ) ) return -1;
	return m_lMaxUIEventID;
}

AgcdUIEventMap* AgcmUIManager2::GetEventMap( INT32 lIndex )
{
	if( lIndex >= m_lMaxUIEvents ) return NULL;

	AgcdUIEventMap** ppstEventMap = ( AgcdUIEventMap** )m_csUIEvents.GetObject( lIndex );
	if( !ppstEventMap )	return FALSE;

	return *ppstEventMap;
}

AgcdUIEventMap* AgcmUIManager2::GetEventMap( CHAR *szName )
{
	if( !szName ) return NULL;

	AgcdUIEventMap** ppstEventMap = ( AgcdUIEventMap** )m_csUIEvents.GetObject( szName );
	if( !ppstEventMap ) return FALSE;

	return *ppstEventMap;
}

BOOL AgcmUIManager2::ThrowEvent( INT32 lEventID, INT32 lDataIndex, BOOL bForce, ApBase *pcsOwnerBase )
{
	AgcdUIEventMap* pstEvent = GetEventMap( lEventID );
	if( !pstEvent )	return FALSE;

	return ProcessAction( &pstEvent->m_listAction, NULL, pstEvent, lDataIndex, bForce, pcsOwnerBase );
}

VOID AgcmUIManager2::RemoveAllEvent( void )
{
	AgcdUIEventMap* pstEvent;
	RemoveAllEventActions();

	for( INT32 lIndex = 1 ; lIndex <= m_lMaxUIEventID ; ++lIndex )
	{
		pstEvent = GetEventMap( lIndex );
		if( !pstEvent ) continue;

		delete pstEvent;
		pstEvent = NULL;
	}

	m_csUIEvents.RemoveObjectAll();
	m_lMaxUIEvents = 0;
}

VOID AgcmUIManager2::RemoveAllEventActions( void )
{
	AgcdUIEventMap* pstEvent;

	for( INT32 lIndex = 1 ; lIndex <= m_lMaxUIEventID ; ++lIndex )
	{
		pstEvent = GetEventMap( lIndex );
		if( !pstEvent )	continue;

		pstEvent->m_listAction.RemoveAll();
	}
}

AgcdUIHotkey* AgcmUIManager2::AddHotkey( AgcdUIHotkeyType eType, INT32 lKeyCode )
{
	if( eType < 0 || eType >= AGCDUI_MAX_HOTKEY_TYPE ) return NULL;
	if( lKeyCode < 0 || lKeyCode >= 256 ) return NULL;

	if( !m_apstHotkeyTable[ eType ][ lKeyCode ] )
	{
		m_apstHotkeyTable[ eType ][ lKeyCode ] = new AgcdUIHotkey;
		if( !m_apstHotkeyTable[ eType ][ lKeyCode ] ) return NULL;
	}

	m_apstHotkeyTable[ eType ][ lKeyCode ]->m_eType = eType;
	m_apstHotkeyTable[ eType ][ lKeyCode ]->m_cKeyCode = lKeyCode;
	return m_apstHotkeyTable[ eType ][ lKeyCode ];
}

AgcdUIHotkey* AgcmUIManager2::GetHotkey( AgcdUIHotkeyType eType, INT32 lKeyCode )
{
	if( eType < 0 || eType >= AGCDUI_MAX_HOTKEY_TYPE ) return NULL;
	if( lKeyCode < 0 || lKeyCode >= 256 ) return NULL;
	return m_apstHotkeyTable[ eType ][ lKeyCode ];
}


//@{ 2006/07/03 burumal
VOID AgcmUIManager2::RemoveHotkey( AgcdUIHotkeyType eType, INT32 lKeyCode )
{
	if( eType < 0 || eType >= AGCDUI_MAX_HOTKEY_TYPE ) return;
	if( lKeyCode < 0 || lKeyCode >= 256 ) return;

	if( m_apstHotkeyTable[ eType ][ lKeyCode ] )
	{
		m_apstHotkeyTable[ eType ][ lKeyCode ]->m_listAction.RemoveAll();
		if( m_apstHotkeyTable[ eType ][ lKeyCode ]->m_szDescription )
		{
			delete[] m_apstHotkeyTable[ eType ][ lKeyCode ]->m_szDescription;
			m_apstHotkeyTable[ eType ][ lKeyCode ]->m_szDescription = NULL;
		}
		
		delete m_apstHotkeyTable[ eType ][ lKeyCode ];
		m_apstHotkeyTable[ eType ][ lKeyCode ] = NULL;
	}
}

VOID AgcmUIManager2::RemoveHotkey( AgcdUIHotkey* pHotkey )
{
	if( !pHotkey ) return;
	if( pHotkey->m_szDescription )
	{
		delete[] pHotkey->m_szDescription;
		pHotkey->m_szDescription = NULL;
	}

	delete pHotkey;
	pHotkey = NULL;
}

BOOL AgcmUIManager2::CopyHotkey( AgcdUIHotkey* pDest, AgcdUIHotkey* pSrc )
{
	if( !pDest || !pSrc ) return FALSE;

	ASSERT(pDest != pSrc);
	if( pDest == pSrc )	return TRUE;

	pDest->m_bCustomizable	= pSrc->m_bCustomizable;
	pDest->m_listAction		= pSrc->m_listAction;
	pDest->m_eType			= pSrc->m_eType;
	pDest->m_cKeyCode		= pSrc->m_cKeyCode;

	if( pDest->m_szDescription )
	{
		delete[] pDest->m_szDescription;
		pDest->m_szDescription = NULL;
	}	
	
	if( pSrc->m_szDescription )
	{
		pDest->m_szDescription = new CHAR[ strlen( pSrc->m_szDescription ) + 1 ];
		strcpy( pDest->m_szDescription, pSrc->m_szDescription );
	}

	return TRUE;
}
//@}

VOID AgcmUIManager2::RemoveAllHotkey( void )
{
	INT32 lIndex1;
	INT32 lIndex2;

	for( lIndex1 = AGCDUI_HOTKEY_TYPE_NORMAL ; lIndex1 < AGCDUI_MAX_HOTKEY_TYPE ; ++lIndex1 )
	{
		for( lIndex2 = 0 ; lIndex2 < 256 ; ++lIndex2 )
		{
			if( m_apstHotkeyTable[ lIndex1 ][ lIndex2 ] )
			{
				m_apstHotkeyTable[ lIndex1 ][ lIndex2 ]->m_listAction.RemoveAll();
				if( m_apstHotkeyTable[ lIndex1 ][ lIndex2 ]->m_szDescription )
				{
					delete[] m_apstHotkeyTable[ lIndex1 ][ lIndex2 ]->m_szDescription;
					m_apstHotkeyTable[ lIndex1 ][ lIndex2 ]->m_szDescription = NULL;
				}

				delete m_apstHotkeyTable[ lIndex1 ][ lIndex2 ];
				m_apstHotkeyTable[ lIndex1 ][ lIndex2 ] = NULL;
			}
		}
	}
}

VOID AgcmUIManager2::SetHotkeyDescription( AgcdUIHotkey *pcsHotkey, CHAR *szDescription )
{
	if( !pcsHotkey ) return;
	if( pcsHotkey->m_szDescription )
	{
		delete[] pcsHotkey->m_szDescription;
		pcsHotkey->m_szDescription = NULL;
	}

	if( szDescription )
	{
		pcsHotkey->m_szDescription = new CHAR[ strlen( szDescription ) + 1 ];
		strcpy( pcsHotkey->m_szDescription, szDescription );
	}
}

BOOL AgcmUIManager2::ResetControlGrid( AgcdUIControl *pcsControl )
{
	if( pcsControl->m_lType != AcUIBase::TYPE_GRID && pcsControl->m_lType != AcUIBase::TYPE_SKILL_TREE ) return FALSE;

	pcsControl->m_uoData.m_stGrid.m_bControlUserData = FALSE;
	pcsControl->m_uoData.m_stGrid.m_pstGrid = NULL;

	return TRUE;
}

BOOL AgcmUIManager2::SetControlGrid( AgcdUIControl *pcsControl, AgcdUIUserData *pstUserData, BOOL bControlUserData )
{
	if( pcsControl->m_lType != AcUIBase::TYPE_GRID && pcsControl->m_lType != AcUIBase::TYPE_SKILL_TREE ) return FALSE;

	AcUIGrid* pcsUIGrid = ( AcUIGrid* )pcsControl->m_pcsBase;
	AgpdGrid* pstGrid;

	if( bControlUserData )
	{
		pstUserData = GetControlUserData( pcsControl );
	}

	pcsControl->m_uoData.m_stGrid.m_bControlUserData = bControlUserData;
	pcsControl->m_uoData.m_stGrid.m_pstGrid = pstUserData;

	if( !pstUserData )
	{
		pstGrid = NULL;
	}
	else
	{
		if( !pstUserData->m_stUserData.m_pvData || pstUserData->m_eType != AGCDUI_USERDATA_TYPE_GRID )
		{
			pstGrid = NULL;
		}
		else if( pstUserData->m_stUserData.m_lCount < pcsControl->m_lUserDataIndex )
		{
			pstGrid = ( AgpdGrid* )pstUserData->m_stUserData.m_pvData;
		}
		else
		{
			pstGrid = ( AgpdGrid* )( ( ( CHAR* )pstUserData->m_stUserData.m_pvData ) + pstUserData->m_stUserData.m_lDataSize * pcsControl->m_lUserDataIndex );
		}
	}

	if( pcsUIGrid->GetAgpdGrid() == pstGrid )
	{
		pcsUIGrid->UpdateUIGrid();
	}
	else
	{
		pcsUIGrid->SetAgpdGridPointer( pstGrid );
	}

	return TRUE;
}

AgpdGrid* AgcmUIManager2::GetControlGrid( AgcdUIControl *pcsControl )
{
	if( !pcsControl || ( pcsControl->m_lType != AcUIBase::TYPE_GRID && pcsControl->m_lType != AcUIBase::TYPE_SKILL_TREE ) )	return NULL;

	AgcdUIUserData* pstUserData;
	if( pcsControl->m_uoData.m_stGrid.m_bControlUserData )
	{
		pstUserData = GetControlUserData( pcsControl );
	}
	else
	{
		pstUserData = pcsControl->m_uoData.m_stGrid.m_pstGrid;
	}

	if( !pstUserData || pstUserData->m_eType != AGCDUI_USERDATA_TYPE_GRID )	return NULL;
	return ( AgpdGrid* )( ( CHAR* )pstUserData->m_stUserData.m_pvData + pstUserData->m_stUserData.m_lDataSize * pcsControl->m_lUserDataIndex );
}

BOOL AgcmUIManager2::ProcessAction( AuList< AgcdUIAction >* plistActions, AgcdUIControl *pcsControl, AgcdUIEventMap *pcsEvent, INT32 lDataIndex, BOOL bForce, ApBase *pcsOwnerBase )
{
	if( m_eModuleStatus != APMODULE_STATUS_READY ) return TRUE;

	// ProcessAction도 바로 하게 해보자.... 문제 생기면 그때 빼도 된다.
	bForce = TRUE;
	if( m_bBeingDestroyed )	return TRUE;
	if( !plistActions->GetCount() ) return FALSE;

	if( !bForce )
	{
		AgcdUIActionBuffer* pstActionBuffer = new AgcdUIActionBuffer;

		pstActionBuffer->m_listActions	= *plistActions;
		pstActionBuffer->m_lDataIndex	= lDataIndex;
		pstActionBuffer->m_pcsControl	= pcsControl;
		pstActionBuffer->m_pcsEvent		= pcsEvent;
		pstActionBuffer->m_pcsOwnerBase	= pcsOwnerBase;

		m_listActionBuffers.AddTail( pstActionBuffer );
	}
	else
	{
		INT32 lIndex;
		AgcdUIAction* pstAction;
		INT32 lReturn;

		for( lIndex = 0; lIndex < plistActions->GetCount(); ++lIndex )
		{
			pstAction = &( ( *plistActions )[ lIndex ] );
			switch( pstAction->m_eType )
			{
			case AGCDUI_ACTION_UI :
				{
					AgcdUIActionUI*	pstActionUI = &pstAction->m_uoAction.m_stUI;
					if( !pstActionUI->m_pcsUI )	return FALSE;

					switch( pstActionUI->m_eType )
					{
					case AGCDUI_ACTION_UI_OPEN :
						{
							OpenUI( pstActionUI->m_pcsUI, pstActionUI->m_bGroupAction, FALSE, pstActionUI->m_bSmooth );
							pstActionUI->m_pcsUI->m_pcsUIWindow->SetOwnerBase( pcsOwnerBase );
						}
						break;

					case AGCDUI_ACTION_UI_CLOSE :
						{
							CloseUI( pstActionUI->m_pcsUI, FALSE, pstActionUI->m_bGroupAction, FALSE, pstActionUI->m_bSmooth );
						}
						break;

					case AGCDUI_ACTION_UI_TOGGLE :
						{
							if( pstActionUI->m_bGroupAction && pstActionUI->m_pcsUI->m_pcsCurrentChildUI )
							{
								if( pstActionUI->m_pcsUI->m_eStatus == AGCDUI_STATUS_OPENED )
								{
									CloseUI( pstActionUI->m_pcsUI, FALSE, FALSE, FALSE, TRUE );
									pstActionUI->m_pcsUI->m_pcsCurrentChildUI = NULL;
								}
								else
								{
									if( pstActionUI->m_pcsUI->m_pcsCurrentChildUI->m_eStatus == AGCDUI_STATUS_OPENED )
									{
										CloseUI( pstActionUI->m_pcsUI, FALSE, pstActionUI->m_bGroupAction, FALSE, pstActionUI->m_bSmooth );
									}
									else if( pstActionUI->m_pcsUI->m_pcsCurrentChildUI->m_eStatus == AGCDUI_STATUS_ADDED )
									{
										OpenUI( pstActionUI->m_pcsUI, pstActionUI->m_bGroupAction, FALSE, pstActionUI->m_bSmooth );
										pstActionUI->m_pcsUI->m_pcsUIWindow->SetOwnerBase( pcsOwnerBase );
									}
								}
							}
							else
							{
								if( pstActionUI->m_pcsUI->m_eStatus == AGCDUI_STATUS_OPENED )
								{
									CloseUI( pstActionUI->m_pcsUI, FALSE, pstActionUI->m_bGroupAction, FALSE, pstActionUI->m_bSmooth );
								}
								else if( pstActionUI->m_pcsUI->m_eStatus == AGCDUI_STATUS_ADDED )
								{
									OpenUI( pstActionUI->m_pcsUI, pstActionUI->m_bGroupAction, FALSE, pstActionUI->m_bSmooth );
									pstActionUI->m_pcsUI->m_pcsUIWindow->SetOwnerBase( pcsOwnerBase );
								}
							}
						}
						break;

					case AGCDUI_ACTION_UI_MOVE :
						{
							pstActionUI->m_pcsUI->m_pcsUIWindow->MoveWindow( pstActionUI->m_lPosX, pstActionUI->m_lPosY,
																				pstActionUI->m_pcsUI->m_pcsUIWindow->w,
																				pstActionUI->m_pcsUI->m_pcsUIWindow->h );
						}
						break;

					case AGCDUI_ACTION_UI_CLOSE_EVENTUI :
						{
							CloseAllEventUI();
						}
						break;

					case AGCDUI_ACTION_UI_ATTACH_LEFT :
					case AGCDUI_ACTION_UI_ATTACH_RIGHT :
					case AGCDUI_ACTION_UI_ATTACH_TOP :
					case AGCDUI_ACTION_UI_ATTACH_BOTTOM :
						{
							AttachUI( pstActionUI->m_pcsUI, pstActionUI->m_pcsAttachUI, ( AgcdUIAttachType )( pstActionUI->m_eType - AGCDUI_ACTION_UI_ATTACH_LEFT ) );
						}
						break;

					case AGCDUI_ACTION_UI_DETACH :
						{
							DetachUI( pstActionUI->m_pcsUI );
						}
						break;
					}
				}
				break;

			case AGCDUI_ACTION_CONTROL :
				{
					AgcdUIActionControl* pstActionControl = &pstAction->m_uoAction.m_stControl;
					AgcdUIControl* pcsTargetControl	= pstActionControl->m_pcsControl;
					AgcdUIControl* pcsControlTemp;

					INT32 lCurrentStatus;
					INT32 lIndex2;

					if( !pcsTargetControl )	return FALSE;

					// 만약 Parent Control이 ListItem 이라면, TargetControl을 동적으로 생성된 놈으로 해줘야 된다.
					pcsControlTemp = pcsTargetControl->m_pcsParentControl;
					while( pcsControlTemp )
					{
						if( pcsControlTemp->m_lType == AcUIBase::TYPE_LIST_ITEM || pcsControlTemp->m_lType == AcUIBase::TYPE_TREE_ITEM )
						{
							AgcdUI* pcsUI = pcsTargetControl->m_pcsParentUI;
							lIndex2 = 0;

							for( pcsControlTemp = GetSequenceControl( pcsUI, &lIndex2 ) ; pcsControlTemp ; pcsControlTemp = GetSequenceControl( pcsUI, &lIndex2 ) )
							{
								if( pcsControlTemp->m_lUserDataIndex == lDataIndex && pcsControlTemp->m_pcsFormatControl == pcsTargetControl )
								{
									pcsTargetControl = pcsControlTemp;
									break;
								}
							}

							break;
						}

						pcsControlTemp = pcsControlTemp->m_pcsParentControl;
					}

					lCurrentStatus = pcsTargetControl->m_pcsBase->m_lCurrentStatusID;
					for( lIndex2 = 0 ; lIndex2 < pstActionControl->m_lStatusNum ; ++lIndex2 )
					{
						if( pstActionControl->m_astStatus[ lIndex2 ].m_lCurrentStatus == -1 ||
							lCurrentStatus == pstActionControl->m_astStatus[ lIndex2 ].m_lCurrentStatus )
						{
							pcsTargetControl->m_pcsBase->SetStatus( pstActionControl->m_astStatus[ lIndex2 ].m_lTargetStatus, pstActionControl->m_bSaveCurrent );
							pcsTargetControl->m_pstUserData = pcsTargetControl->m_apstUserData[pstActionControl->m_astStatus[ lIndex2 ].m_lTargetStatus & ( ACUIBASE_STATUS_ID_BIT_CONSTANT - 1 ) ];

							RefreshControl( pcsTargetControl, TRUE, TRUE );
							break;
						}
					}
				}
				break;

			case AGCDUI_ACTION_FUNCTION :
				{
					AgcdUIActionFunction* pstActionFunction = &pstAction->m_uoAction.m_stFunction;
					AgcdUIArg* pstArgs = pstActionFunction->m_astArgs;

					if( pstActionFunction->m_pstFunction && pstActionFunction->m_pstFunction->m_fnCallback )
					{
						// 아직 테스트 코드입니다. (Target을 넣어야됨)
						pstActionFunction->m_pstFunction->m_fnCallback( pstActionFunction->m_pstFunction->m_pClass,
							pstArgs[ 0 ].m_eType == AGCDUI_ARG_TYPE_UI ? ( PVOID )pstArgs[ 0 ].m_pcsUI : ( PVOID )pstArgs[0].m_pstControl,
							pstArgs[ 1 ].m_eType == AGCDUI_ARG_TYPE_UI ? ( PVOID )pstArgs[ 1 ].m_pcsUI : ( PVOID )pstArgs[1].m_pstControl,
							pstArgs[ 2 ].m_eType == AGCDUI_ARG_TYPE_UI ? ( PVOID )pstArgs[ 2 ].m_pcsUI : ( PVOID )pstArgs[2].m_pstControl,
							pstArgs[ 3 ].m_eType == AGCDUI_ARG_TYPE_UI ? ( PVOID )pstArgs[ 3 ].m_pcsUI : ( PVOID )pstArgs[3].m_pstControl,
							pstArgs[ 4 ].m_eType == AGCDUI_ARG_TYPE_UI ? ( PVOID )pstArgs[ 4 ].m_pcsUI : ( PVOID )pstArgs[4].m_pstControl,
							m_pcsTarget, pcsControl	);
					}
				}
				break;

			case AGCDUI_ACTION_MESSAGE :
				{
					AgcdUIActionMessage* pstActionMessage = &pstAction->m_uoAction.m_stMessage;

					// Action Message 를 날려주자!
					if( pstActionMessage )
					{
						CHAR szMessage[ AGCDUIMANAGER2_MAX_ACTION_MESSAGE ] = { 0, };
						if( !ParseMessage( szMessage, pstActionMessage->m_szMessage ) ) break;

						switch( pstActionMessage->m_eMessageType )
						{
						case AGCDUI_ACTION_MESSAGE_OK_DIALOG :
							{
								ActionMessageOKDialog( szMessage );
								if( pcsEvent && pcsEvent->m_fnReturnCB && pcsEvent->m_pCBClass )
								{
									pcsEvent->m_fnReturnCB( pcsEvent->m_pCBClass, 0, NULL );
								}
							}
							break;

						case AGCDUI_ACTION_MESSAGE_OKCANCEL_DIALOG :
							{
								lReturn = ActionMessageOKCancelDialog( szMessage );
								if( pcsEvent && pcsEvent->m_fnReturnCB && pcsEvent->m_pCBClass )
								{
									pcsEvent->m_fnReturnCB( pcsEvent->m_pCBClass, lReturn, NULL );
								}
							}
							break;

						case AGCDUI_ACTION_MESSAGE_CHATTING_WINDOW :
							{
								EnumCallback( AGCMUIMANAGER2_CB_ID_ACTION_MESSAGE_CHAT, ( PVOID )( szMessage ), ( PVOID )&( pstActionMessage->m_lColor ) );
							}
							break;
						}
					}
				}
				break;

			case AGCDUI_ACTION_SOUND :
				{
					AgcdUIActionSound* pstActionSound = &pstAction->m_uoAction.m_stSound;

					if( pstActionSound && pstActionSound->m_szSound && m_pcsAgcmSound )
					{
						CHAR szTemp[ AGCDUIMANAGER2_MAX_NAME * 2 ] = { 0, };
						sprintf( szTemp, "%s%s", m_szSoundPath, pstActionSound->m_szSound );
						m_pcsAgcmSound->PlaySampleSound( szTemp );
					}
				}
				break;
			}
		}
	}

	return TRUE;
}

BOOL AgcmUIManager2::ParseMessage( CHAR *szParsedMessage, CHAR *szMessage )
{
	INT32 lLen = 0;
	CHAR* szVariable;
	CHAR szName[ AGCDUIMANAGER2_MAX_NAME ] = { 0, }; 

	AgcdUIUserData*	pstUserData;
	AgcdUIDisplay* pstDisplay;
	INT32 lTemp;

	szParsedMessage[ 0 ] = 0;
	szVariable = ( CHAR* )strstr( ( CHAR const* )szMessage, AGCMUIMANAGER2_VARIABLE_START );

	while( szVariable )
	{
		// '[['부터 '.'까지 Parsing해서 UserData 가져온다.
		lLen = szVariable - szMessage;
		strncat( szParsedMessage, szMessage, lLen );

		szParsedMessage += lLen;
		szMessage = szVariable + strlen( AGCMUIMANAGER2_VARIABLE_START );

		szVariable = ( CHAR* )strstr( ( CHAR const* )szMessage, AGCMUIMANAGER2_VARIABLE_DELIMITER );
		if( !szVariable ) return FALSE;

		szName[ 0 ] = 0;
		strncat( szName, szMessage, szVariable - szMessage );
		pstUserData = GetUserData( szName );

		// '.'부터 ']]'까지 Parsing해서 Display 가져온다.
		szMessage = szVariable + strlen( AGCMUIMANAGER2_VARIABLE_DELIMITER );
		szVariable = ( CHAR* )strstr( ( CHAR const* )szMessage, AGCMUIMANAGER2_VARIABLE_END );
		if( !szVariable ) return FALSE;

		szName[ 0 ] = 0;
		strncat( szName, szMessage, szVariable - szMessage );
		pstDisplay = GetDisplay( szName );
		if( !pstDisplay ) return FALSE;

		// DisplayCallback으로 실제 Data를 만들어낸다.
		if( !EnumDisplayCallback( pstDisplay, pstUserData, szParsedMessage, &lTemp ) ) return FALSE;

		szParsedMessage += strlen( szParsedMessage );
		szMessage = szVariable + strlen( AGCMUIMANAGER2_VARIABLE_END );
		szVariable = ( CHAR* )strstr( ( CHAR const* )szMessage, AGCMUIMANAGER2_VARIABLE_START );
	}

	strcat( szParsedMessage, szMessage );
	return TRUE;
}

VOID AgcmUIManager2::SetupActions( void )
{
	INT32 lIndex;
	INT32 lIndex2;

	AgcdUIEventMap*	pstEventMap;
	AgcdUI* pcsUI;
	AgcdUIControl* pcsControl;

	for( lIndex = 1 ; lIndex <= GetEventCount() ; ++lIndex )
	{
		pstEventMap = GetEventMap( lIndex );
		if( pstEventMap )
		{
			SetupAction( &pstEventMap->m_listAction );
		}
	}

	lIndex = 0;
	for( pcsUI = GetSequenceUI( &lIndex ) ; pcsUI ; pcsUI = GetSequenceUI( &lIndex ) )
	{
		if( pcsUI->m_szParentUIName[ 0 ] )
		{
			pcsUI->m_pcsParentUI = GetUI( pcsUI->m_szParentUIName );
		}

		lIndex2 = 0;
		for( pcsControl = GetSequenceControl( pcsUI, &lIndex2 ) ; pcsControl ; pcsControl = GetSequenceControl( pcsUI, &lIndex2 ) )
		{
			SetupActions( pcsControl, FALSE );
		}
	}
}

VOID AgcmUIManager2::SetupActions( AgcdUIControl* pcsControl, BOOL bTraverse )
{
	INT32 lIndex;
	INT32 lIndex2;

	AgcdUIControl* pcsChildControl;
	AgcdUIMessageMap* pstMessageMap;
	AgcdUIHotkey* pstHotkey;

	for( lIndex = 0 ; lIndex < pcsControl->m_lMessageCount ; ++lIndex )
	{		
		pstMessageMap = pcsControl->m_pstMessageMaps + lIndex;
		SetupAction( &pstMessageMap->m_listAction );
	}

	for( lIndex = 0; lIndex < 256; ++lIndex )
	{
		for( lIndex2 = AGCDUI_HOTKEY_TYPE_NORMAL ; lIndex2 < AGCDUI_MAX_HOTKEY_TYPE ; ++lIndex2 )
		{
			pstHotkey = GetHotkey( ( AgcdUIHotkeyType )lIndex2, lIndex );
			if( !pstHotkey ) continue;

			SetupAction( &pstHotkey->m_listAction );
		}
	}

	if( bTraverse )
	{
		lIndex = 0;
		for( pcsChildControl = GetSequenceControl( pcsControl, &lIndex ) ; pcsChildControl ; pcsChildControl = GetSequenceControl( pcsControl, &lIndex ) )
		{
			SetupActions( pcsChildControl );
		}
	}
}

BOOL AgcmUIManager2::SetupAction( AuList <AgcdUIAction> *plistActions )
{
	INT32 lIndex;
	AgcdUI* pcsUI;
	AgcdUIControl* pcsControl;
	BOOL bRetVal = TRUE;
	AgcdUIAction* pstAction;

	for( lIndex = 0; lIndex < plistActions->GetCount(); ++lIndex )
	{
		pstAction = &( ( *plistActions )[ lIndex ] );
		switch( pstAction->m_eType )
		{
		case AGCDUI_ACTION_UI :
			{
				AgcdUIActionUI*	pstActionUI = &pstAction->m_uoAction.m_stUI;
				if( !pstActionUI->m_pcsUI )
				{
					pcsUI = GetUI( pstActionUI->m_szUIName );
					if( !pcsUI )
					{
						bRetVal = FALSE;
					}

					pstActionUI->m_pcsUI = pcsUI;
				}

				if( !pstActionUI->m_pcsAttachUI && pstActionUI->m_szAttachUIName[ 0 ] )
				{
					pcsUI = GetUI( pstActionUI->m_szAttachUIName );
					if( !pcsUI )
					{
						bRetVal = FALSE;
					}

					pstActionUI->m_pcsAttachUI = pcsUI;
				}

				break;
			}

		case AGCDUI_ACTION_CONTROL:
			{
				AgcdUIActionControl* pstActionControl = &pstAction->m_uoAction.m_stControl;
				if( !pstActionControl->m_pcsUI )
				{
					pcsUI = GetUI( pstActionControl->m_szUIName );
					if( !pcsUI )
					{
						bRetVal = FALSE;
					}

					pstActionControl->m_pcsUI = GetUI( pstActionControl->m_szUIName );
				}

				if( pstActionControl->m_pcsUI && !pstActionControl->m_pcsControl )
				{
					pcsControl = GetControl( pstActionControl->m_pcsUI, pstActionControl->m_szControlName );
					if( !pcsControl )
					{
						bRetVal = FALSE;
					}

					pstActionControl->m_pcsControl = pcsControl;
				}

				break;
			}

		case AGCDUI_ACTION_FUNCTION :
			{
				INT32 lIndex2;
				AgcdUIActionFunction* pstActionFunction = &pstAction->m_uoAction.m_stFunction;

				for( lIndex2 = 0; lIndex2 < AGCDUIMANAGER2_MAX_ARGS ; ++lIndex2 )
				{
					if( !pstActionFunction->m_astArgs[ lIndex2 ].m_pcsUI )
					{
						pstActionFunction->m_astArgs[ lIndex2 ].m_pcsUI = GetUI( pstActionFunction->m_astArgs[ lIndex2 ].m_szUIName );
						if( !pstActionFunction->m_astArgs[ lIndex2 ].m_pcsUI )
						{
							bRetVal = FALSE;
						}
					}

					if( pstActionFunction->m_astArgs[ lIndex2 ].m_pcsUI && !pstActionFunction->m_astArgs[ lIndex2 ].m_pstControl &&
						pstActionFunction->m_astArgs[ lIndex2 ].m_eType == AGCDUI_ARG_TYPE_CONTROL )
					{
						pcsControl = GetControl( pstActionFunction->m_astArgs[ lIndex2 ].m_pcsUI, pstActionFunction->m_astArgs[ lIndex2 ].m_szControlName );
						if( !pcsControl )
						{
							bRetVal = FALSE;
						}

						pstActionFunction->m_astArgs[ lIndex2 ].m_pstControl = pcsControl;
					}
				}
				break;
			}
		}

		++pstAction;
	}

	return bRetVal;
}

VOID AgcmUIManager2::RemoveFromActions( AgcdUI *pcsUI, AgcdUIControl *pcsControl )
{
	PROFILE( "AgcmUIManager2::RemoveFromActions1" );

	INT32 lIndex;
	INT32 lIndex2;
	AgcdUIEventMap* pstEventMap;
	AgcdUI* pcsSourceUI;
	AgcdUIControl* pcsSourceControl;

	for( lIndex = 1; lIndex <= GetEventCount(); ++lIndex )
	{
		pstEventMap = GetEventMap( lIndex );
		if( pstEventMap )
		{
			RemoveFromAction( pcsUI, pcsControl, &pstEventMap->m_listAction );
		}
	}

	lIndex = 0;
	for( pcsSourceUI = GetSequenceUI( &lIndex ); pcsSourceUI; pcsSourceUI = GetSequenceUI( &lIndex ) )
	{
		if( pcsSourceUI->m_pcsParentUI == pcsUI )
		{
			pcsSourceUI->m_pcsParentUI = NULL;
		}

		lIndex2 = 0;
		for( pcsSourceControl = GetSequenceControl( pcsSourceUI, &lIndex2 ); pcsSourceControl; pcsSourceControl = GetSequenceControl( pcsSourceUI, &lIndex2 ) )
		{
			if( pcsSourceControl->m_pcsParentControl == pcsControl )
			{
				pcsSourceControl->m_pcsParentControl = NULL;
			}

			RemoveFromActions( pcsUI, pcsControl, pcsSourceControl, FALSE );
		}
	}
}

VOID AgcmUIManager2::RemoveFromActions( AgcdUI *pcsUI, AgcdUIControl *pcsControl, AgcdUIControl *pcsSourceControl, BOOL bTraverse )
{
	PROFILE( "AgcmUIManager2::RemoveFromActions2" );

	INT32 lIndex;
	INT32 lIndex2;

	AgcdUIControl* pcsChildControl;
	AgcdUIMessageMap* pstMessageMap;
	AgcdUIHotkey* pstHotkey;

	for( lIndex = 0; lIndex < pcsSourceControl->m_lMessageCount; ++lIndex )
	{		
		pstMessageMap = pcsSourceControl->m_pstMessageMaps + lIndex;
		RemoveFromAction( pcsUI, pcsControl, &pstMessageMap->m_listAction );
	}

	for( lIndex = 0; lIndex < 256; ++lIndex )
	{
		for( lIndex2 = AGCDUI_HOTKEY_TYPE_NORMAL; lIndex2 < AGCDUI_MAX_HOTKEY_TYPE; ++lIndex2 )
		{
			pstHotkey = GetHotkey( ( AgcdUIHotkeyType )lIndex2, lIndex );
			if( !pstHotkey ) continue;

			RemoveFromAction( pcsUI, pcsControl, &pstHotkey->m_listAction );
		}
	}

	if( bTraverse )
	{
		lIndex = 0;
		for( pcsChildControl = GetSequenceControl( pcsSourceControl, &lIndex ); pcsChildControl; pcsChildControl = GetSequenceControl( pcsSourceControl, &lIndex ) )
		{
			RemoveFromActions( pcsUI, pcsControl, pcsChildControl );
		}
	}
}

BOOL AgcmUIManager2::RemoveFromAction( AgcdUI *pcsUI, AgcdUIControl *pcsControl, AuList< AgcdUIAction >* plistActions )
{
	PROFILE( "AgcmUIManager2::RemoveFromActions3" );

	INT32 lIndex;
	BOOL bRetVal = TRUE;
	AgcdUIAction* pstAction;

	for( lIndex = 0; lIndex < plistActions->GetCount(); ++lIndex )
	{
		pstAction = &( ( *plistActions )[ lIndex ] );
		switch( pstAction->m_eType )
		{
		case AGCDUI_ACTION_UI :
			{
				AgcdUIActionUI*	pstActionUI = &pstAction->m_uoAction.m_stUI;
				if( pstActionUI->m_pcsUI == pcsUI )
				{
					pstActionUI->m_pcsUI = NULL;
				}

				break;
			}
		case AGCDUI_ACTION_CONTROL :
			{
				AgcdUIActionControl* pstActionControl = &pstAction->m_uoAction.m_stControl;
				if( pstActionControl->m_pcsUI == pcsUI )
				{
					pstActionControl->m_pcsUI = NULL;
				}

				if( pstActionControl->m_pcsControl == pcsControl )
				{
					pstActionControl->m_pcsControl = NULL;
				}

				break;
			}
		case AGCDUI_ACTION_FUNCTION :
			{
				INT32 lIndex2;
				AgcdUIActionFunction* pstActionFunction = &pstAction->m_uoAction.m_stFunction;

				for( lIndex2 = 0; lIndex2 < AGCDUIMANAGER2_MAX_ARGS; ++lIndex2 )
				{
					if( pstActionFunction->m_astArgs[ lIndex2 ].m_pcsUI == pcsUI )
					{
						pstActionFunction->m_astArgs[ lIndex2 ].m_pcsUI = NULL;
					}

					if( pstActionFunction->m_astArgs[ lIndex2 ].m_pstControl == pcsControl )
					{
						pstActionFunction->m_astArgs[ lIndex2 ].m_pstControl = NULL;
					}
				}

				break;
			}
		}
	}

	return bRetVal;
}

BOOL AgcmUIManager2::CBListItemConstructor( AcUIList* pUIList, AcUIListItem* pUIListItem, PVOID pClass, PVOID pData, INT32 *plHeight )
{
	PROFILE( "AgcmUIManager2::CBListItemConstructor" );

	AgcmUIManager2*	pThis = ( AgcmUIManager2* )pClass;
	AgcdUI* pcsUI = ( AgcdUI* )pData;
	AgcdUIControl* pcsListControl = pThis->GetControl( pcsUI, pUIList );
	AgcdUIControl* pcsItemControl;
	AgcdUIControl* pcsFormatControl;

	if( !pcsListControl ) return FALSE;

	pcsFormatControl = pcsListControl->m_uoData.m_stList.m_pcsListItemFormat;
	if( !pcsFormatControl )	return FALSE;

	pcsItemControl = pThis->AddControl( pcsUI, pUIListItem->m_nType, 0, 0, pUIList, pUIListItem );
	if( !pcsItemControl ) return FALSE;
	if( pThis->CopyControl( pcsFormatControl, pcsUI, pUIList, TRUE, pUIListItem->m_lItemIndex, pcsItemControl, TRUE ) != pcsItemControl ) return FALSE;

	// Tree Control인 경우에 Open/Close Button Setting
	if( pcsListControl->m_lType == AcUIBase::TYPE_TREE && pcsItemControl->m_lType == AcUIBase::TYPE_TREE_ITEM )
	{
		INT32 lIndex = 0;
		AgcdUIControl*	pcsControl;
		AcUIButton*	pcsButtonClose = NULL;
		AcUIButton*	pcsButtonOpen = NULL;

		for( pcsControl = pThis->GetSequenceControl( pcsItemControl, &lIndex ); pcsControl; pcsControl = pThis->GetSequenceControl( pcsItemControl, &lIndex ) )
		{
			if( pcsControl->m_lType == AcUIBase::TYPE_BUTTON )
			{
				if( pcsControl->m_uoData.m_stButton.m_eButtonType == AGCDUI_BUTTON_TYPE_TREE_CLOSE )
				{
					( ( AcUITreeItem* )pUIListItem )->SetTreeButtonClose( ( AcUIButton* )pcsControl->m_pcsBase );
				}

				if( pcsControl->m_uoData.m_stButton.m_eButtonType == AGCDUI_BUTTON_TYPE_TREE_OPEN )
				{
					( ( AcUITreeItem* )pUIListItem )->SetTreeButtonOpen( ( AcUIButton* )pcsControl->m_pcsBase );
				}
			}
		}
	}

	pcsUI->m_pcsUIWindow->UpdateChildWindow();
	return TRUE;
}

BOOL AgcmUIManager2::CBListItemDestructor( AcUIList* pUIList, AcUIListItem* pUIListItem, PVOID pClass, PVOID pData, INT32 *plHeight )
{
	PROFILE( "AgcmUIManager2::CBListItemDestructor" );

	AgcmUIManager2* pThis = ( AgcmUIManager2* )pClass;
	AgcdUI* pcsUI = ( AgcdUI* )pData;
	AgcdUIControl* pcsListControl = pThis->GetControl( pcsUI, pUIList );
	AgcdUIControl* pcsItemControl;

	if( !pcsListControl ) return FALSE;

	pcsItemControl = pThis->GetControl( pcsListControl, pUIListItem );
	if( !pcsItemControl ) return FALSE;

	pThis->RemoveControl( pcsItemControl );
	return TRUE;
}

BOOL AgcmUIManager2::CBListItemRefresh( AcUIList* pUIList, AcUIListItem* pUIListItem, PVOID pClass, PVOID pData, INT32 *plHeight )
{
	PROFILE( "AgcmUIManager2::CBListItemDestructor" );

	AgcmUIManager2*	pThis = ( AgcmUIManager2* )pClass;
	AgcdUI* pcsUI = ( AgcdUI* )pData;
	AgcdUIControl* pcsListControl = pThis->GetControl( pcsUI, pUIList );
	AgcdUIControl* pcsItemControl = pThis->GetControl( pcsUI, pUIListItem );
	if( !pcsListControl ) return FALSE;

	pThis->RefreshControl( pcsItemControl, TRUE, TRUE, TRUE, pUIListItem->m_lItemIndex );
	return TRUE;
}

BOOL AgcmUIManager2::CB_POST_RENDER( PVOID pData, PVOID pClass, PVOID pCustData )
{
	return TRUE;
}

BOOL AgcmUIManager2::CBIsCameraMoving( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pData || !pClass ) return FALSE;

	AgcmUIManager2* pThis = ( AgcmUIManager2* )pClass;
	BOOL* pbIsCameraMoving = ( BOOL* )pData;

	*pbIsCameraMoving = pThis->m_csManagerWindow.m_bRButtonClicked;
	return TRUE;
}

void AgcmUIManager2::OnCameraStateChange( CAMERASTATECHANGETYPE )
{
	CameraStatusChange();
}

void AgcmUIManager2::CameraStatusChange( void )
{
	RwCamera * pCamera = GetCamera();
	if( !m_pstCamera || !pCamera || m_pstCamera != pCamera ) return;

	for( int i = 0 ; i < 4 ; ++i )
	{
		m_vtBoxFan[ i ].rhw = 1.0f;
		m_vtBoxFan[ i ].color = 0xffffffff;
		m_vtBoxFan[ i ].z = 0.0f;
	}

	m_vtBoxFan[ 0 ].u = 0.0f;
	m_vtBoxFan[ 0 ].v = 0.0f;

	m_vtBoxFan[ 1 ].u = 1.0f;
	m_vtBoxFan[ 1 ].v = 0.0f;

	m_vtBoxFan[ 2 ].u = 1.0f;
	m_vtBoxFan[ 2 ].v = 1.0f;

	m_vtBoxFan[ 3 ].u = 0.0f;
	m_vtBoxFan[ 3 ].v = 1.0f;

	RwRaster* pstRaster	= RwCameraGetRaster( pCamera );
	if( !pstRaster ) return ;

	INT32 lWidth = RwRasterGetWidth( pstRaster );
	INT32 lHeight = RwRasterGetHeight( pstRaster );

	if( m_lWindowHeight != lHeight || m_lWindowWidth != lWidth )
	{
		INT32 lIndex;
		
		ResizeManagerWindow( 0, 0, lWidth, lHeight );
		m_bOnCameraSizing = TRUE;
		lIndex = 0;

		m_lWindowWidth = lWidth;
		m_lWindowHeight = lHeight;
		m_bOnCameraSizing = FALSE;
		m_pcsAgcmUIControl->SetMessageDialogPosition( m_lWindowWidth / 2 - AGCMUICONTROL_MESSAGE_WIDTH / 2,
														m_lWindowHeight / 2 - AGCMUICONTROL_MESSAGE_HEIGHT / 2 );
	}
}

VOID AgcmUIManager2::SetCamera( RwCamera *pstCamera )
{
	m_pstCamera = pstCamera; 
	CameraStatusChange();
}

BOOL AgcmUIManager2::AddWindow( AgcWindow* pWindow )
{
	if( m_pcsMainWindow )
	{
		m_csManagerWindow.AddChild( pWindow );
		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIManager2::RemoveWindow( AgcWindow* pWindow )
{
	if( m_pcsMainWindow )
	{
		m_csManagerWindow.DeleteChild( pWindow );
		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIManager2::SetCallbackOpenMainUI( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMUIMANAGER2_CB_ID_OPEN_MAIN_UI, pfCallback, pClass );
}

BOOL AgcmUIManager2::SetCallbackCloseAllUI( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMUIMANAGER2_CB_ID_CLOSE_ALL_UI, pfCallback, pClass );
}

BOOL AgcmUIManager2::SetCallbackSetMainWindow( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMUIMANAGER2_CB_ID_SET_MAIN_WINDOW, pfCallback, pClass );
}

BOOL AgcmUIManager2::SetCallbackActionMessageChat( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMUIMANAGER2_CB_ID_ACTION_MESSAGE_CHAT, pfCallback, pClass );
}

BOOL AgcmUIManager2::IsMainWindow( AgcWindow *pWindow )
{
	if( &m_csManagerWindow == pWindow )	return TRUE;
	return FALSE;
}

BOOL AgcmUIManager2::CBSetCursorMousePosition( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIManager2*	pThis = ( AgcmUIManager2* )pClass;
	if( NULL == pThis ) return FALSE;

	pThis->m_v2dCurMousePos = ( ( RsMouseStatus* )pData )->pos;
	return TRUE;
}

INT32 AgcmUIManager2::ActionMessageOKDialog( CHAR* szMessage )
{
	m_pcsAgcmUIControl->SetMessageDialogPosition( m_lWindowWidth / 2 - AGCMUICONTROL_MESSAGE_WIDTH / 2,
													m_lWindowHeight / 2 - AGCMUICONTROL_MESSAGE_HEIGHT / 2 );

	return g_pEngine->MessageDialog( NULL, szMessage );
}

INT32 AgcmUIManager2::ActionMessageOKDialog2( CHAR* szMessage )
{
	m_pcsAgcmUIControl->SetMessageDialogPosition( m_lWindowWidth / 2 - AGCMUICONTROL_MESSAGE_WIDTH / 2,
													m_lWindowHeight / 2 - AGCMUICONTROL_MESSAGE_HEIGHT / 2 );

	return g_pEngine->MessageDialog2( this , szMessage );
}

INT32 AgcmUIManager2::ActionMessageOKCancelDialog( CHAR* szMessage )
{
	m_pcsAgcmUIControl->SetMessageDialogPosition( m_lWindowWidth / 2 - AGCMUICONTROL_MESSAGE_WIDTH / 2,
													m_lWindowHeight / 2 - AGCMUICONTROL_MESSAGE_HEIGHT / 2 );

	return g_pEngine->OkCancelDialog( NULL, szMessage );
}

INT32 AgcmUIManager2::ActionMessageOKCancelDialog2( CHAR* szMessage )
{
	m_pcsAgcmUIControl->SetMessageDialogPosition( m_lWindowWidth / 2 - AGCMUICONTROL_MESSAGE_WIDTH / 2,
													m_lWindowHeight / 2 - AGCMUICONTROL_MESSAGE_HEIGHT / 2 );

	return g_pEngine->OkCancelDialog2( NULL, szMessage );
}

INT32 AgcmUIManager2::ActionMessageEditOKDialog( CHAR* szMessage, CHAR* pEditString )
{
	m_pcsAgcmUIControl->SetMessageDialogPosition( m_lWindowWidth / 2 - AGCMUICONTROL_MESSAGE_WIDTH / 2,
													m_lWindowHeight / 2 - AGCMUICONTROL_MESSAGE_HEIGHT / 2 );

	return g_pEngine->EditMessageDialog( NULL, szMessage, pEditString );
}

INT32 AgcmUIManager2::ActionMessageEditOKDialog2( CHAR* szMessage, CHAR* pEditString )
{
	m_pcsAgcmUIControl->SetMessageDialogPosition( m_lWindowWidth / 2 - AGCMUICONTROL_MESSAGE_WIDTH / 2,
													m_lWindowHeight / 2 - AGCMUICONTROL_MESSAGE_HEIGHT / 2 );

	return g_pEngine->EditMessageDialog2( NULL, szMessage, pEditString );
}

#ifdef _AREA_GLOBAL_
INT32 AgcmUIManager2::ActionMessageThreeBtnDialog( CHAR* szMessage ,CHAR* szBtn1Name ,CHAR* szBtn2Name ,CHAR* szBtn3Name)
{
	m_pcsAgcmUIControl->SetMessageDialogPosition( m_lWindowWidth / 2 - AGCMUICONTROL_MESSAGE_WIDTH / 2,
		m_lWindowHeight / 2 - AGCMUICONTROL_MESSAGE_HEIGHT / 2 );

	/*m_pcsAgcmUIControl->m_clThreeBtnDialog.m_clOneBtn.SetStaticString( szBtn1Name );
	m_pcsAgcmUIControl->m_clThreeBtnDialog.m_clTwoBtn.SetStaticString( szBtn2Name );
	m_pcsAgcmUIControl->m_clThreeBtnDialog.m_clOK.SetStaticString( szBtn3Name );
	return g_pEngine->ThreeBtnDialog(NULL, szMessage);*/
	return 0;
}
#endif

VOID AgcmUIManager2::SortPopupUI( AgcdUI *pcsUI )
{
	if( pcsUI->m_eType != AGCDUI_TYPE_POPUP_V && pcsUI->m_eType != AGCDUI_TYPE_POPUP_H ) return;

	AuList< AgcdUIControl* > listNewChild;
	AuNode< AgcdUIControl* >* pNode;
	AuNode< AgcdUIControl* >* pNodeNext;
	AgcdUIControl* pcsControl;
	AgcdUIControl* pcsControlNext;
	INT32 lCoord = -1000;

	pNode = pcsUI->m_listChild.GetHeadNode();
	while( pNode )
	{
		pNodeNext = pNode->GetNextNode();
		pcsControl = pNode->GetData();

		if( pcsControl->m_pcsParentControl )
		{
			listNewChild.AddHead( pcsControl );
			pcsUI->m_listChild.RemoveNode( pNode );
		}

		pNode = pNodeNext;
	}

	while( pcsUI->m_listChild.GetCount() )
	{
		lCoord = -1000;
		pNode = pcsUI->m_listChild.GetHeadNode();

		while( pNode )
		{
			pcsControl = pNode->GetData();
			if( pcsUI->m_eType == AGCDUI_TYPE_POPUP_V )
			{
				if( lCoord < pcsControl->m_pcsBase->y )
				{
					pcsControlNext = pcsControl;
					lCoord = pcsControl->m_pcsBase->y;
					pNodeNext = pNode;
				}
			}
			else if( pcsUI->m_eType == AGCDUI_TYPE_POPUP_H )
			{
				if( lCoord < pcsControl->m_pcsBase->x )
				{
					pcsControlNext = pcsControl;
					lCoord = pcsControl->m_pcsBase->x;
					pNodeNext = pNode;
				}
			}

			pNode = pNode->GetNextNode();
		}

		listNewChild.AddHead( pcsControlNext );
		pcsUI->m_listChild.RemoveData( pcsControlNext );
	}

	pcsUI->m_listChild.RemoveAll();
	pcsUI->m_listChild = listNewChild;
}

VOID AgcmUIManager2::SetSoundPath( CHAR *szPath )
{
	strcpy( m_szSoundPath, szPath );
}

VOID AgcmUIManager2::SetTexturePath( CHAR *szPath )
{
	strcpy( m_szTexturePath, szPath );
}

VOID AgcmUIManager2::SetUIMode( AgcdUIMode eMode )
{
	if( eMode >= AGCDUI_MAX_MODE ) return;
	if( eMode == m_eMode ) return;

	INT32 lIndex;
	AgcdUI*	pcsUI;
	m_eMode = eMode;
	lIndex = 0;

	for( pcsUI = GetSequenceUI( &lIndex ); pcsUI; pcsUI = GetSequenceUI( &lIndex ) )
	{
		pcsUI->m_pcsUIWindow->MoveWindow( pcsUI->m_alX[ eMode ], pcsUI->m_alY[ eMode ], pcsUI->m_pcsUIWindow->w, pcsUI->m_pcsUIWindow->h );
	}
}

CHAR* AgcmUIManager2::GetLineDelimiter( void )
{
	return m_szLineDelimiter;
}

CHAR* AgcmUIManager2::GetUIMessage( CHAR *szMessageID )
{
	if( !szMessageID || !szMessageID[ 0 ] ) return NULL;

	INT32 nEntryCount = m_mapAdminUIMsg.size();

	if( nEntryCount < 1 )
	{
		char tmp[1024] = {0,};
		sprintf_s(tmp, sizeof(tmp), "Ini\\UIMessage.txt");
#ifdef _BIN_EXEC_
		bool decrypt = false;
#else
		bool decrypt = true;
#endif
		StreamReadUIMessage(tmp, decrypt);
	}

	MapUIMessageIter	itr = m_mapAdminUIMsg.find(szMessageID);

	if(itr != m_mapAdminUIMsg.end())
	{
		stAdminUIMsgEntry*	pMsgEntry	=	(*itr).second;

		if( pMsgEntry )
			return (CHAR*)pMsgEntry->m_strMsg.c_str();
	}

	return 0;
}

BOOL AgcmUIManager2::GetSeparateLine( CHAR *szMessageID, INT32 lLineIndex, CHAR *szBuffer, INT32 lBufferSize )
{
	if( !szMessageID || lLineIndex < 1 || !szBuffer || lBufferSize < 1 ) return FALSE;

	INT32 lMessageLength = strlen( szMessageID );
	INT32 lLineNumber = 1;

	CHAR* szLineStartPos = szMessageID;
	INT32 lLineStartNumber = 0;

	for( int i = 0; i < lMessageLength; ++i )
	{
		if( strncmp( szMessageID + i, m_szLineDelimiter, strlen( m_szLineDelimiter ) )  == 0 )
		{
			if( lLineNumber == lLineIndex )
			{
				strncpy( szBuffer, szLineStartPos, i - lLineStartNumber );
				return TRUE;
			}

			++lLineNumber;
			i += strlen( m_szLineDelimiter );

			szLineStartPos = szMessageID + i;
			lLineStartNumber = i;
		}
	}

	if( lLineNumber == lLineIndex )
	{
		strncpy( szBuffer, szLineStartPos, strlen( szMessageID ) );
		return TRUE;
	}

	return FALSE;
}

//@{ 2006/09/26 burumal
VOID AgcmUIManager2::CloseTooltip( INT32 x, INT32 y, INT32 w, INT32 h, AgcUIWindow* pcsParentUIWindow )
//@}
{
	AgcWindowNode* cur_node = m_csManagerWindow.m_listChild.head;
	AgcWindow* pWindow;

	while( cur_node )
	{
		pWindow = cur_node->pWindow;
		
		// 만약 영역내에 들어오는 Tooltip이면 없앤다.
		if( pWindow->m_bUIWindow && ( ( AcUIBase* )pWindow )->m_nType == AcUIBase::TYPE_TOOLTIP &&
			pWindow->x >= x && pWindow->y >= y && pWindow->x <= x + w && pWindow->y <= y + h )
		{
			pWindow->ShowWindow( FALSE );
		}
		
		cur_node = cur_node->next;
	}

	//@{ 2006/09/26 burumal
	if ( m_csTooltip.GetParentUIWindow() && m_csTooltip.GetParentUIWindow() == pcsParentUIWindow )
	{
		m_csTooltip.ShowWindow( FALSE );
	}
	//@}
}

VOID AgcmUIManager2::CloseTooltip( void )
{
	m_csTooltip.ShowWindow( FALSE );

	//@{ 2006/09/26 burumal
	m_csTooltip.SetParentUIWindow( NULL );
	//@}
}

//@{ 2006/09/26 burumal
VOID AgcmUIManager2::OpenTooltip( CHAR *szTooltip, AgcWindow *pcsWindow, AgcUIWindow* pcsParentUIWindow )
//@}
{
	INT32 lX = ( INT32 )m_v2dCurMousePos.x;
	INT32 lY = ( INT32 )m_v2dCurMousePos.y;

	m_csTooltip.DeleteAllStringInfo();
	m_csTooltip.AddString( szTooltip );

	if( pcsWindow && pcsWindow->m_bUIWindow && ( ( AcUIBase* )pcsWindow )->m_bTooltipForFitString )
	{
		AcUIBase* pcsBase = ( AcUIBase* )pcsWindow;

		lX = pcsBase->m_lAbsolute_x + pcsBase->m_lStringX + pcsBase->m_lStringOffsetX;
		lY = pcsBase->m_lAbsolute_y + pcsBase->m_lStringY + pcsBase->m_lStringOffsetY;
	}
	else
	{
		lY = lY + 32;
		if( lX + m_csTooltip.w > m_csManagerWindow.w )
		{
			lX = lX - m_csTooltip.w;
		}

		if( lY + m_csTooltip.h > m_csManagerWindow.h )
		{
			if( pcsWindow )
			{
				lY = 0;
				pcsWindow->ClientToScreen( NULL, &lY );
				lY = lY - m_csTooltip.h;
			}
			else
			{
				lY = ( INT32 )( m_v2dCurMousePos.y - m_csTooltip.h - 3 );
			}
		}
	}

	m_csTooltip.MoveWindow( lX, lY, m_csTooltip.w, m_csTooltip.h );
	m_csTooltip.ShowWindow( TRUE );

	//@{ 2006/09/26 burumal
	m_csTooltip.SetParentUIWindow( pcsParentUIWindow );
	//@}
}

VOID AgcmUIManager2::SetControlTooltip( AgcdUIControl *pcsControl, CHAR *szTooltip )
{
	if( pcsControl->m_szTooltip )
	{
		delete[] pcsControl->m_szTooltip;
		pcsControl->m_szTooltip = NULL;
	}

	pcsControl->m_szTooltip = new CHAR[ strlen( szTooltip ) + 1 ];
	strcpy( pcsControl->m_szTooltip, szTooltip );
}

BOOL AgcmUIManager2::Notice( CHAR *szMessage, DWORD dwColor, CHAR *szSubMessage, DWORD dwSubColor, INT32 lDuration, FLOAT fRateY )
{
	if( !szMessage ) return FALSE;

	m_csNotification.DeleteAllStringInfo();
	m_csNotification.AddString( szMessage, 10, dwColor );
	m_csNotification.AddNewLine( 14 );

	if( szSubMessage )
	{
		m_csNotification.AddString( szSubMessage, 10, dwSubColor );
		m_csNotification.AddNewLine( 14 );
	}
	m_csNotification.ShowWindow();
	m_csNotification.SetFocus();

	if( lDuration == -1 )
	{
		UINT32 ulNotificationDelay = strlen( szMessage ) * 200;

		if( ulNotificationDelay < g_ulNotificationMinDelay )
		{
			ulNotificationDelay = g_ulNotificationMinDelay;
		}

		m_ulNotificationCloseTime = GetClockCount() + ulNotificationDelay;
	}
	else
	{
		m_ulNotificationCloseTime = GetClockCount() + lDuration;
	}

	m_csNotification.MoveWindow( ( m_csManagerWindow.w - m_csNotification.w ) / 2, ( INT32 )( m_csManagerWindow.h * fRateY ), m_csNotification.w, m_csNotification.h );
	return TRUE;
}

VOID AgcmUIManager2::SetMouseOnUI( AgcdUI *pcsUI )
{
	if( m_bUIFocusProcessed )
	{
		m_pcsMouseOnUI = pcsUI;
		if( pcsUI )
		{
			m_bUIFocusProcessed = FALSE;
			m_strMouseOnUI = pcsUI->m_szUIName;
		}
	}
}

VOID AgcmUIManager2::UIClose( std::string strUIName )
{
	AgcdUI* pcsUI = GetUI( ( CHAR* )strUIName.c_str() );
	if( !pcsUI ) return;

	CloseUI( pcsUI );
}

VOID AgcmUIManager2::ShowUIName( void )
{
	AgcmUIConsole* pcsAgcmUIConsole = ( AgcmUIConsole* )GetModule( "AgcmUIConsole" );
	if( pcsAgcmUIConsole )
	{
		pcsAgcmUIConsole->getConsole().registerWatch( 0, "AgcmUIManager2.m_strMouseOnUI", 0xffffff00 );
	}
}

//@{ 2006/07/27 burumal
VOID AgcmUIManager2::SetUIMgrProcessHotkeys( void )
{
	m_csManagerWindow.SetCameraViewProcessHotkeys();
}
//@}

BOOL AgcmUIManager2::IsOpenModalWindow( void )
{
	// 현재 사용되고 있는 모달윈도우는 우선 메세지 박스류가 있다.
	AgcModalWindow* pMsgBoxWait = g_pEngine->m_pWaitingDialog;
	BOOL bIsOpenMsgBoxWait = FALSE;
	if( pMsgBoxWait && pMsgBoxWait->m_bOpened )
	{
		bIsOpenMsgBoxWait = TRUE;
	}

	AgcModalWindow* pMsgBoxOK = g_pEngine->m_pMessageDialog;
	BOOL bIsOpenMsgBoxOK = FALSE;
	if( pMsgBoxOK && pMsgBoxOK->m_bOpened )
	{
		bIsOpenMsgBoxOK = TRUE;
	}

	AgcModalWindow* pMsgBoxOK2 = g_pEngine->m_pMessageDialog2;
	BOOL bIsOpenMsgBoxOK2 = FALSE;
	if( pMsgBoxOK2 && pMsgBoxOK2->m_bOpened )
	{
		bIsOpenMsgBoxOK2 = TRUE;
	}

	AgcModalWindow* pMsgBoxOKCancel = g_pEngine->m_pOkCancelDialog;
	BOOL bIsOpenMsgBoxOKCancel = FALSE;
	if( pMsgBoxOKCancel && pMsgBoxOKCancel->m_bOpened )
	{
		bIsOpenMsgBoxOKCancel = TRUE;
	}

	AgcModalWindow* pMsgBoxEditOK = g_pEngine->m_pEditOkDialog;
	BOOL bIsOpenMsgBoxEditOK = FALSE;
	if( pMsgBoxEditOK && pMsgBoxEditOK->m_bOpened )
	{
		bIsOpenMsgBoxEditOK = TRUE;
	}

	if( bIsOpenMsgBoxWait || bIsOpenMsgBoxOK || bIsOpenMsgBoxOK2 || bIsOpenMsgBoxOKCancel || bIsOpenMsgBoxEditOK )
	{
		return TRUE;
	}

	AgcmEventNPCDialog* pcmEventNPCDialog = ( AgcmEventNPCDialog* )GetModule( "AgcmEventNPCDialog" );
	if( pcmEventNPCDialog )
	{
		CExNPC* pExNPC = pcmEventNPCDialog->GetExNPC();
		if( pExNPC )
		{
			if( pExNPC->IsOpenModalWindow() )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

float AgcmUIManager2::GetModeWidth()
{
	float result = 1024.f;

	switch( GetUIMode() )
	{
	case AGCDUI_MODE_1024_768:
		result = 1024.f;
		break;
	case AGCDUI_MODE_1280_1024:
		result = 1280.f;
		break;
	case AGCDUI_MODE_1600_1200:
		result = 1600.f;
		break;
	}

	return result;
}

float AgcmUIManager2::GetModeHeight()
{
	float result = 768.f;

	switch( GetUIMode() )
	{
	case AGCDUI_MODE_1024_768:
		result = 768.f;
		break;
	case AGCDUI_MODE_1280_1024:
		result = 1024.f;
		break;
	case AGCDUI_MODE_1600_1200:
		result = 1200.f;
		break;
	}

	return result;
}