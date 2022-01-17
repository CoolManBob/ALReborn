#include "AgcmUICharacterSelect.h"
#include "AgpdCharacter.h"
#include "AgcmUILoginSetting.h"
#include "AgcmUILogin.h"
#include "AgcmUIManager2.h"
#include "AgcmUICharacter.h"

#ifndef USE_MFC
#ifdef _AREA_KOREA_
	#include "CWebzenAuth.h"
#endif
	#include "AuJapaneseClientAuth.h"
#endif




BOOL AgcmUICharacterSelect::stCharacterSlot::SetCharacter( CHAR* pName, void* ppdCharacter, RwV3d vPos, float fRotate )
{
	if( !ppdCharacter || !pName ) return FALSE;
	m_ppdCharacter = ppdCharacter;

	strcpy( m_strCharacterName, pName );

	m_vCharacterPos.x = vPos.x;
	m_vCharacterPos.y = vPos.y;
	m_vCharacterPos.z = vPos.z;

	m_fCharacterRotate = fRotate;
	return TRUE;
}





AgcmUICharacterSelect::AgcmUICharacterSelect( void )
: m_nCurrSlot( -1 ), m_nPrevSlot( -1 ), m_pcmUILogin( NULL ), m_nCurrentCharacterID( 0 )
{
}

AgcmUICharacterSelect::~AgcmUICharacterSelect( void )
{
	m_mapCharacterSlot.Clear();
	m_nCurrSlot = -1;
	m_nPrevSlot = -1;
}

BOOL AgcmUICharacterSelect::OnInitialize( void* pUILogin )
{
	m_pcmUILogin = pUILogin;
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnAddFunction( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	return TRUE;
}

BOOL AgcmUICharacterSelect::OnAddUserData( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	return TRUE;
}

BOOL AgcmUICharacterSelect::OnAddDisplay( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	return TRUE;
}

BOOL AgcmUICharacterSelect::OnAddCallBack( void* pLogin )
{
	AgcmLogin* pcmLogin = ( AgcmLogin* )pLogin;
	if( !pcmLogin ) return FALSE;

	return TRUE;
}

BOOL AgcmUICharacterSelect::OnSlotInitialize( void )
{
	stCharacterSlot SlotCenter;
	SlotCenter.m_nSlotIndex = 0;

	m_mapCharacterSlot.Add( 0, SlotCenter );

	stCharacterSlot SlotLeft;
	SlotCenter.m_nSlotIndex = 1;

	m_mapCharacterSlot.Add( 1, SlotCenter );

	stCharacterSlot SlotRight;
	SlotCenter.m_nSlotIndex = 2;

	m_mapCharacterSlot.Add( 2, SlotCenter );
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnSlotUpdate( INT32 nSlotIndex )
{
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnCharacterAddInfo( INT32 nSlotIndex, TCHAR* pCharaceterName )
{
	if( !pCharaceterName ) return FALSE;

	stCharacterSlot* pSlot = m_mapCharacterSlot.Get( nSlotIndex );
	if( !pSlot ) return FALSE;

	memset( pSlot->m_strCharacterName, 0, sizeof( CHAR ) * 64 );
	strcpy( pSlot->m_strCharacterName, pCharaceterName );
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnCharacterAdd( void* pUILogin, void* pCharacter )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return FALSE;

	stCharacterSlot* pSlot = _GetCharacterSlot( ppdCharacter->m_szID );
	if( !pSlot ) return FALSE;

	stPositionSet* pPosition = GetUILoginSetting()->GetSelectPosition( pSlot->m_nSlotIndex );
	if( pPosition ) return FALSE;

	pSlot->m_ppdCharacter = ppdCharacter;
	pSlot->m_vCharacterPos.x = pPosition->m_vPos.x;
	pSlot->m_vCharacterPos.y = pPosition->m_vPos.y;
	pSlot->m_vCharacterPos.z = pPosition->m_vPos.z;
	pSlot->m_fCharacterRotate = pPosition->m_rRotate;

	_MakeCharacter( pUILogin, pSlot );
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnCharacterSelect( INT32 nCharacterID )
{
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnCharacterUse( INT32 nCharacterID )
{
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnCharacterRemove( INT32 nSlotIndex )
{
	m_mapCharacterSlot.Delete( nSlotIndex );
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnCharacterRemove( TCHAR* pCharacterName )
{
	stCharacterSlot* pSlot = _GetCharacterSlot( pCharacterName );
	if( !pSlot ) return FALSE;

	m_mapCharacterSlot.Delete( pSlot->m_nSlotIndex );
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnBtnCharacterCreate( void* pUILogin )
{
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnBtnCharacterRemove( void* pUILogin )
{
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnBtnReturnToSelectServer( void* pUILogin )
{
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnBtnSlot( INT32 nControlNumber )
{
	if( nControlNumber < 0 ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )GetModule( "AgcmUILogin" );
	if( !pcmUILogin ) return FALSE;
	if( pcmUILogin->GetLoginMode() != AGCMLOGIN_MODE_CHARACTER_SELECT ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )GetModule( "AgcmUICharacter" );
	if( !pcmUICharacter ) return FALSE;

	AgpdCharacter* ppdCurrentCharacter = ( AgpdCharacter* )GetSlotCharacter( m_nCurrSlot );
	if( ppdCurrentCharacter )
	{
		AgcdCharacter* pcdCurrentCharacter = pcmCharacter->GetCharacterData( ppdCurrentCharacter );
		if( pcdCurrentCharacter && pcdCurrentCharacter->m_pClump )
		{
			pcmUIManager->m_pcsAgcmRender->ResetLighting( pcdCurrentCharacter->m_pClump );
		}
	}

	AgpdCharacter* ppdSelectCharacter = ( AgpdCharacter* )GetSlotCharacter( nControlNumber );
	if( !ppdSelectCharacter ) return FALSE;

	pcmUICharacter->SetCharacter( ppdSelectCharacter );
	AgcdCharacter* pcdSelectCharacter = pcmCharacter->GetCharacterData( ppdSelectCharacter );
	if( pcdSelectCharacter && pcdSelectCharacter->m_pClump )
	{
		pcmUIManager->m_pcsAgcmRender->CustomizeLighting( pcdSelectCharacter->m_pClump, LIGHT_LOGIN_SELECT );
	}

	pcmUILogin->m_ControlCharacter.OnDoubleClickCharacter( ppdSelectCharacter->m_lID );
	m_nCurrentCharacterID = ppdSelectCharacter->m_lID;

	pcmUILogin->UpdateCompenInfo();
	pcmUILogin->UpdateSlotCharInfo();

	pcmUIManager->SetUserDataRefresh( pcmUILogin->m_pstUserDataDummy );
	return TRUE;
}

BOOL AgcmUICharacterSelect::OnBtnCompensation( INT32 nControlNumber )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )GetModule( "AgcmUILogin" );
	if( !pcmUILogin ) return FALSE;
	if( pcmUILogin->GetLoginMode() != AGCMLOGIN_MODE_CHARACTER_SELECT ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )GetModule( "AgcmUICharacter" );
	if( !pcmUICharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )GetSlotCharacter( nControlNumber );
	if( !ppdCharacter ) return FALSE;

	CHAR* pFormat = NULL;
	BOOL bIsSelect = TRUE;

	if( _tcslen( m_cdLoginCompenMaster.m_szSelectedCharID ) > 0 )
	{
		if( _tcscmp( m_cdLoginCompenMaster.m_szSelectedCharID, ppdCharacter->m_szID ) == 0 )
		{
			pFormat = pcmUIManager->GetUIMessage( UI_MESSAGE_ID_COMPENSATION_CANCEL );
		}
	}
	else
	{
		pFormat = pcmUIManager->GetUIMessage( UI_MESSAGE_ID_COMPENSATION_CANCEL );
	}


	return TRUE;
}

BOOL AgcmUICharacterSelect::OnBtnExitGame( void* pUILogin )
{
	return TRUE;
}

void* AgcmUICharacterSelect::GetModule( CHAR* pModuleName )
{
	if( !pModuleName || strlen( pModuleName ) == 0 ) return NULL;
	if( !m_pcmUILogin ) return NULL;
	return ( ( AgcmUILogin* )m_pcmUILogin )->GetModule( pModuleName );
}

INT32 AgcmUICharacterSelect::GetControlNumber( AgcdUIControl* pControl )
{
	if( !pControl ) return -1;
	return 0;
}

void* AgcmUICharacterSelect::GetSlotCharacter( INT32 nSlotIndex )
{
	stCharacterSlot* pSlot = m_mapCharacterSlot.Get( nSlotIndex );
	if( !pSlot ) return NULL;
	return pSlot->m_ppdCharacter;
}

void* AgcmUICharacterSelect::GetSelectCharacter( void )
{
	stCharacterSlot* pSlot = m_mapCharacterSlot.Get( m_nCurrSlot );
	if( !pSlot ) return NULL;
	return pSlot->m_ppdCharacter;
}

BOOL AgcmUICharacterSelect::_MakeCharacter( void* pUILogin, stCharacterSlot* pSlot )
{
	if( !pSlot ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pSlot->m_ppdCharacter;
	if( !ppdCharacter ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pUILogin;
	if( !pcmUILogin ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pcmUILogin->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmUILogin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpmFactors* ppmFactors = ( AgpmFactors* )pcmUILogin->GetModule( "AgpmFactors" );
	if( !ppmFactors ) return FALSE;

	switch( pcmUILogin->GetLoginMode() )
	{
	case AGCMLOGIN_MODE_WAIT_MY_CHARACTER :
		{
			AgcdCharacter* pcdCharacter = pcmCharacter->GetCharacterData( ppdCharacter );
			if( !pcdCharacter ) return FALSE;

			AgpdFactor* ppdFactor = &ppdCharacter->m_pcsCharacterTemplate->m_csFactor;
			if( !ppdFactor ) return FALSE;

			RpClump* pClump = pcdCharacter->m_pClump;
			if( !pClump ) return FALSE;

			RwFrame* pFrame = RpClumpGetFrame( pClump );
			if( !pFrame ) return FALSE;

			ppdCharacter->m_stPos = pSlot->m_vCharacterPos;
			ppmCharacter->AddCharacterToMap( ppdCharacter );
			ppmCharacter->TurnCharacter( ppdCharacter, 0.0f, pSlot->m_fCharacterRotate );

			AuRaceType eRace = ( AuRaceType )ppmFactors->GetRace( ppdFactor );
			AuCharClassType eClass = ( AuCharClassType )ppmFactors->GetClass( ppdFactor );

			RwReal rDistance = GetUILoginSetting()->GetMoveOffset( eRace, eClass );
			RwV3d vOffset = *RwMatrixGetAt( RwFrameGetLTM( pFrame ) );

			RwV3dScale( &vOffset, &vOffset, rDistance );

			RwV3d vSelectPos = { 0.0f, 0.0f, 0.0f };
			RwV3dAdd( &vSelectPos, &vOffset, ( RwV3d* )( &pSlot->m_vCharacterPos ) );

			pcmUILogin->m_ControlCharacter.OnAddCharacter( ppdCharacter->ApBase::m_lID, ppdCharacter,
				pcdCharacter, vSelectPos, *( const RwV3d* )( &pSlot->m_vCharacterPos ) );
		}
		break;

	case AGCMLOGIN_MODE_WAIT_MY_NEW_CHARACTER :
		{
			AuPOS vWorldCenter = { 0.0f, 0.0f, 0.0f };
			ppmCharacter->UpdatePosition( ppdCharacter, &vWorldCenter, FALSE );
		}
		break;
	}

	return TRUE;
}

INT32 AgcmUICharacterSelect::_GetCharacterCount( void )
{
	return m_mapCharacterSlot.GetSize();
}

INT32 AgcmUICharacterSelect::_GetSlotIndexByName( CHAR* pName )
{
	stCharacterSlot* pSlot = _GetCharacterSlot( pName );
	if( !pSlot ) return -1;
	return pSlot->m_nSlotIndex;
}

AgcmUICharacterSelect::stCharacterSlot* AgcmUICharacterSelect::_GetCharacterSlot( INT32 nSlotIndex )
{
	return m_mapCharacterSlot.Get( nSlotIndex );
}

AgcmUICharacterSelect::stCharacterSlot* AgcmUICharacterSelect::_GetCharacterSlot( CHAR* pName )
{
	if( !pName ) return NULL;
	INT32 nSlotCount = m_mapCharacterSlot.GetSize();

	for( INT32 nCount = 0 ; nCount < nSlotCount ; ++nCount )
	{
		stCharacterSlot* pSlot = _GetCharacterSlot( nCount );
		if( pSlot )
		{
			if( strcmp( pSlot->m_strCharacterName, pName ) == 0 )
			{
				return pSlot;
			}
		}
	}

	return NULL;
}

AgcmUICharacterSelect::stCharacterSlot* AgcmUICharacterSelect::_GetCurrCharacterSlot( void )
{
	return m_mapCharacterSlot.Get( m_nCurrSlot );
}

AgcmUICharacterSelect::stCharacterSlot* AgcmUICharacterSelect::_GetPrevCharacterSlot( void )
{
	return m_mapCharacterSlot.Get( m_nPrevSlot );
}

BOOL AgcmUICharacterSelect::CB_OnBtnCharacterSelect( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUICharacterSelect* pThis = ( AgcmUICharacterSelect* )pClass;
	if( !pThis ) return FALSE;

	//if (AP_INVALID_CID != pThis->m_lLoginMainSelectCID)
	//{
	//	if (pThis->SelectCharacterAtLoginMain(pThis->m_lLoginMainSelectCID))
	//	{
	//		pThis->m_lLoginMainSelectCID = AP_INVALID_CID;
	//		g_pEngine->WaitingDialog( NULL, ClientStr().GetStr(STI_CONNECTING_SERVER) );

	//		AgcmEventPointLight* pcmEventPointLight = ( AgcmEventPointLight* )pThis->GetModule( "AgcmEventPointLight" );
	//		if( pcmEventPointLight )
	//		{
	//			pcmEventPointLight->m_bReturnLoginLobby = FALSE;
	//		}

	//		pThis->m_pcsPreSelectedCharacter	= NULL;
	//		pThis->m_lAddMyCharacterCount		= 0;
	//		pThis->m_lLoginMainSelectCID		= 0;

	//		ZeroMemory(pThis->m_szMyCharacterInfo, sizeof(pThis->m_szMyCharacterInfo));
	//		ZeroMemory(pThis->m_bMyCharacterFlag, sizeof(pThis->m_bMyCharacterFlag));
	//	}
	//}
	//else
	//{
	//	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	//	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_ENTER_GAME_NOT_SELECTED_CHAR]);
	//}

	return TRUE;
}

BOOL AgcmUICharacterSelect::CB_OnBtnCharacterDelete( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUICharacterSelect* pThis = ( AgcmUICharacterSelect* )pClass;
	if( !pThis ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgpdCharacter* ppdSelectCharacter = ( AgpdCharacter* )pThis->GetSelectCharacter();
	if( !ppdSelectCharacter ) return FALSE;

	if( pData1 )
	{
		AgcdUI* pcdUI = ( AgcdUI* )pData1;
		INT32 nIndex = 0;

		AgcdUIControl* pcdUIControl = pcmUIManager->GetSequenceControl( pcdUI, &nIndex );
		while( pcdUIControl )
		{
			if( pcdUIControl->m_pcsBase && pcdUIControl->m_pcsBase->m_nType == AcUIBase::TYPE_EDIT )
			{
				// 읽기전용인 에디트를 찾아 삭제 확인 메세지를 찍어준다..
				// 특정 UI를 찾는게 아니라 모든 읽기전용인 것들에 대한 작업이라.. 그닥..;;
				if( ( ( AcUIEdit* )pcdUIControl->m_pcsBase )->m_bReadOnly )
				{
					TCHAR strBuffer[ 512 ] = { 0, };

					_stprintf( strBuffer, _T( "%s %s" ), ppdSelectCharacter->m_szID,
						pcmUIManager->GetUIMessage( UI_MESSAGE_ID_LOGIN_CONFIRM_PASSWORD ) );

					( ( AcUIEdit* )pcdUIControl->m_pcsBase )->SetText( strBuffer );
				}
				// 읽기전용 아닌 것들은 클리어..
				else
				{
					( ( AcUIEdit* )pcdUIControl->m_pcsBase )->ClearText();
				}
			}
		}
	}

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->GetModule( "AgcmUILogin" );
	if( !pcmUILogin ) return FALSE;

#if defined( _AREA_KOREA_ ) && !defined( USE_MFC )
	CWebzenAuth* pWebzenAuth = CWebzenAuth::GetInstance();
	if( pWebzenAuth && pWebzenAuth->IsAutoLogin() )
	{
		pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_REMOVE_MYCHARACTER_QUESTION ] );
	}
#elif defined(_AREA_JAPAN_)
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_REMOVE_MYCHARACTER_QUESTION ] );
#else
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_CONFIRM_PASSWORD ] );
#endif
	return TRUE;
}

BOOL AgcmUICharacterSelect::CB_OnBtnSlotSelect( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUICharacterSelect* pThis = ( AgcmUICharacterSelect* )pClass;
	if( !pThis ) return FALSE;

	INT32 nControlNumber = pThis->GetControlNumber( pControl );
	return pThis->OnBtnSlot( nControlNumber );
}

BOOL AgcmUICharacterSelect::CB_OnBtnSlotInfoSelect( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUICharacterSelect* pThis = ( AgcmUICharacterSelect* )pClass;
	if( !pThis ) return FALSE;

	INT32 nControlNumber = pThis->GetControlNumber( pControl );
	return pThis->OnBtnCompensation( nControlNumber );
}

BOOL AgcmUICharacterSelect::CB_OnBtnSlotSetFocus( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUICharacterSelect* pThis = ( AgcmUICharacterSelect* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->GetModule( "AgcmUILogin" );
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	if( pcmUILogin->m_AgcdLoginCompenMaster.m_lCompenID == 0 ||
		_tcslen( pcmUILogin->m_AgcdLoginCompenMaster.m_szDescription ) <= 0 ) return FALSE;

	pcmUILogin->m_CompenTooltip.DeleteAllStringInfo();

	INT32 nPosX = ( INT32 )pcmUIManager->m_v2dCurMousePos.x + 30;
	INT32 nPosY = ( INT32 )pcmUIManager->m_v2dCurMousePos.y + 30;
	INT32 nWidth = ( INT32 )pcmUILogin->m_CompenTooltip.w;
	INT32 nHeight = ( INT32 )pcmUILogin->m_CompenTooltip.h;
	DWORD dwColor = 0xffffffff;
	CHAR* pString = pcmUILogin->m_AgcdLoginCompenMaster.m_szDescription;

	pcmUILogin->m_CompenTooltip.MoveWindow( nPosX, nPosY, nWidth, nHeight );
	pcmUILogin->m_CompenTooltip.AddString( pString, 14, dwColor );
	pcmUILogin->m_CompenTooltip.AddNewLine( 14 );
	pcmUILogin->m_CompenTooltip.ShowWindow( TRUE );
	return TRUE;
}

BOOL AgcmUICharacterSelect::CB_OnBtnSlotKillFocus( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUICharacterSelect* pThis = ( AgcmUICharacterSelect* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->GetModule( "AgcmUILogin" );
	if( !pcmUILogin ) return FALSE;

	pcmUILogin->m_CompenTooltip.ShowWindow( FALSE );
	pcmUILogin->m_CompenTooltip.DeleteAllStringInfo();
	return TRUE;
}
