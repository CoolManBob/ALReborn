#include "AgcuUIControlCharacter.h"
#include "AgpdCharacter.h"
#include "AgcdCharacter.h"
#include "AgcmLogin.h"
#include "AgcmEventEffect.h"
#include "rpworld.h"



AgcuUIControlCharacter::stCharacterController::stCharacterController( void )
: m_nCharacterID( 0 ), m_eCharacterState( NoState ), m_ppdCharacter( NULL ), m_pcdCharacter( NULL )
{	
}

AgcuUIControlCharacter::stCharacterController::stCharacterController( INT32 nCharacterID, void* ppdCharacter, void* pcdCharacter, RwV3d vSelectedPos, RwV3d vPos )
: m_nCharacterID( nCharacterID ), m_eCharacterState( Ready ), m_ppdCharacter( ppdCharacter ), m_pcdCharacter( pcdCharacter )
{
	m_vSelectPos = vSelectedPos;
	m_vCreatePos = vPos;
}

AgcuUIControlCharacter::stCharacterController::stCharacterController( const stCharacterController& rSource )
: m_nCharacterID( rSource.m_nCharacterID ), m_eCharacterState( rSource.m_eCharacterState ),
	m_ppdCharacter( rSource.m_ppdCharacter ), m_pcdCharacter( rSource.m_pcdCharacter )
{
	m_vSelectPos = rSource.m_vSelectPos;
	m_vCreatePos = rSource.m_vCreatePos;
}

bool AgcuUIControlCharacter::stCharacterController::IsMyCharacterID( INT32 nCharacterID )
{
	if( m_eCharacterState == NoState ) return false;
	return m_nCharacterID == nCharacterID ? true : false;
}

bool AgcuUIControlCharacter::stCharacterController::IsMyCharacterName( CHAR* pCharacterName )
{
	if( !pCharacterName ) return false;
	if( !strcmp( pCharacterName, "" ) ) return false;
	if( m_eCharacterState == NoState ) return false;
	if( !m_ppdCharacter ) return false;

	return strcmp( ( ( AgpdCharacter* )m_ppdCharacter )->m_szID, pCharacterName ) == 0 ? true : false;
}

void AgcuUIControlCharacter::stCharacterController::ChangeState( AuCharacterState eNewState )
{
	m_eCharacterState = eNewState;
}

RwFrame* AgcuUIControlCharacter::stCharacterController::GetFrame( void )
{
	if( !m_pcdCharacter ) return NULL;

	RpClump* pClump = ( ( AgcdCharacter* )m_pcdCharacter )->m_pClump;
	if( !pClump ) return NULL;

	return RpClumpGetFrame( pClump );
}

INT32 AgcuUIControlCharacter::stCharacterController::MoveForward( void* ppmCharacter, float fElapsed )
{
	if( !ppmCharacter || !m_ppdCharacter || !m_pcdCharacter ) return 0;
	AgpmCharacter* pagpmCharacter = ( AgpmCharacter* )ppmCharacter;

	if( m_eCharacterState == Selected )
	{
		pagpmCharacter->UpdatePosition( ( AgpdCharacter* )m_ppdCharacter, ( AuPOS* )&m_vSelectPos, FALSE );
		m_eCharacterState = Ready;
	}
	return 0;
}

INT32 AgcuUIControlCharacter::stCharacterController::MoveBackward( void* ppmCharacter, float fElapsed )
{
	if( !ppmCharacter || !m_ppdCharacter || !m_pcdCharacter ) return 0;
	AgpmCharacter* pagpmCharacter = ( AgpmCharacter* )ppmCharacter;

	if( m_eCharacterState == UnSelected )
	{
		pagpmCharacter->UpdatePosition( ( AgpdCharacter* )m_ppdCharacter, ( AuPOS* )&m_vCreatePos, FALSE );
		m_eCharacterState = Ready;
	}
	return 0;
}









AgcuUIControlCharacter* AgcuUIControlCharacter::m_pSingletonPtr = NULL;
AgcuUIControlCharacter::AgcuUIControlCharacter( void )
: m_pcmCharacter( NULL ), m_ppmCharacter( NULL ), m_pcmEventEffect( NULL ),
	m_nLoginMode( AGCMLOGIN_MODE_PRE_LOGIN ), m_eRotateState( NoRotate ), m_nCurrSelectCharacterID( 0 )
{
	m_pSingletonPtr = this;
}

INT32 AgcuUIControlCharacter::OnUpdate( RwReal fElapsed )
{
	switch( m_nLoginMode )
	{
	case AGCMLOGIN_MODE_CHARACTER_CREATE :		_UpdateModeCharacterCreate( fElapsed );			break;
	}

	return 0;
}

INT32 AgcuUIControlCharacter::OnChangeMode( INT32 nLoginMode )
{
	switch( nLoginMode )
	{
	case AGCMLOGIN_MODE_CHARACTER_CREATE :
	case AGCMLOGIN_MODE_CHARACTER_SELECT :		m_nLoginMode = nLoginMode;						break;
	}

	return 0;
}

INT32 AgcuUIControlCharacter::OnClearCharacter( void )
{
	m_mapSelectCharacters.Clear();
	return 0;
}

INT32 AgcuUIControlCharacter::OnAddCharacter( INT32 nCharacterID, void* ppdCharacter, void* pcdCharacter, RwV3d vSelectedPos, RwV3d vCreatePos )
{
	stCharacterController* pCharacter = m_mapSelectCharacters.Get( nCharacterID );
	if( pCharacter )
	{
		pCharacter->m_eCharacterState = Ready;
		pCharacter->m_ppdCharacter = ppdCharacter;
		pCharacter->m_pcdCharacter = pcdCharacter;
		pCharacter->m_vSelectPos = vSelectedPos;
		pCharacter->m_vCreatePos = vCreatePos;
	}
	else
	{
		stCharacterController NewCharacter( nCharacterID, ppdCharacter, pcdCharacter, vSelectedPos, vCreatePos );
		m_mapSelectCharacters.Add( nCharacterID, NewCharacter );
	}

	return 0;
}

INT32 AgcuUIControlCharacter::OnDeleteCharacter( INT32 nCharacterID )
{
	m_mapSelectCharacters.Delete( nCharacterID );
	return 0;
}

INT32 AgcuUIControlCharacter::OnDeleteCharacter( CHAR* pCharacterName )
{
	int nCharacterCount = m_mapSelectCharacters.GetSize();

	for( int nCount = 0 ; nCount < nCharacterCount ; ++nCount )
	{
		stCharacterController* pCharacter = m_mapSelectCharacters.GetByIndex( nCount );
		if( pCharacter->IsMyCharacterName( pCharacterName ) )
		{
			m_mapSelectCharacters.Delete( pCharacter->m_nCharacterID );
			return 0;
		}
	}

	return 0;
}

INT32 AgcuUIControlCharacter::OnDoubleClickCharacter( INT32 nCharacterID )
{
	if( m_nCurrSelectCharacterID == nCharacterID )
	{
		stCharacterController* pSelectCharacter = m_mapSelectCharacters.Get( nCharacterID );
		if( pSelectCharacter )
		{
			if( m_pcmCharacter )
			{
				AgcmCharacter* pagcmCharacter = ( AgcmCharacter* )m_pcmCharacter;
				pagcmCharacter->SetNextAnimation( ( AgpdCharacter* )pSelectCharacter->m_ppdCharacter,
					( AgcdCharacter* )pSelectCharacter->m_pcdCharacter, AGCMCHAR_ANIM_TYPE_ATTACK, FALSE );
			}
		}
	}
	else
	{
		_UnSelectCharacter();
		m_nCurrSelectCharacterID = nCharacterID;
		_SelectCharacter();
	}

	return 0;
}

INT32 AgcuUIControlCharacter::OnRotateStartLeft( void )
{
	m_eRotateState = RotateLeft;
	return 0;
}

INT32 AgcuUIControlCharacter::OnRotateStartRight( void )
{
	m_eRotateState = RotateRight;
	return 0;
}

INT32 AgcuUIControlCharacter::OnRotateEnd( void )
{
	m_eRotateState = NoRotate;
	return 0;
}

INT32 AgcuUIControlCharacter::InitializeCreateCharacter( INT32 nCharacterID, void* ppdCharacter, void* pcdCharacter, RwV3d vCreatePos, float fMoveOffset )
{
	// if exist current character, move it to zero point
	if( m_CreateCharacter.m_ppdCharacter && m_CreateCharacter.m_nCharacterID != nCharacterID )
	{
		AuPOS vZeroPoint = { 0.0f, 0.0f, 0.0f };
		if( m_ppmCharacter )
		{
			AgpmCharacter* pagpmCharacter = ( AgpmCharacter* )m_ppmCharacter;
			pagpmCharacter->UpdatePosition( ( AgpdCharacter* )m_CreateCharacter.m_ppdCharacter, &vZeroPoint, FALSE );
		}
	}

	m_CreateCharacter.m_nCharacterID = nCharacterID;
	m_CreateCharacter.m_eCharacterState = Ready;
	m_CreateCharacter.m_ppdCharacter = ppdCharacter;
	m_CreateCharacter.m_pcdCharacter = pcdCharacter;

	RwV3d vSelectPos = vCreatePos;
	RwFrame* pFrame = m_CreateCharacter.GetFrame();
	if( pFrame )
	{
		RwMatrix* pmatTM = RwFrameGetLTM( pFrame );
		if( pmatTM )
		{
			RwV3d vAt = pmatTM->at;
			RwV3dScale( &vAt, &vAt, fMoveOffset );
			RwV3dAdd( &vSelectPos, &vCreatePos, &vAt );
		}
	}

	m_CreateCharacter.m_vSelectPos = vSelectPos;
	m_CreateCharacter.m_vCreatePos = vCreatePos;

	return 0;
}

void AgcuUIControlCharacter::SetExternalClassPointers( void* pcmCharacter, void* ppmCharacter, void* pcmEventEffect )
{
	m_pcmCharacter = pcmCharacter;
	m_ppmCharacter = ppmCharacter;
	m_pcmEventEffect = pcmEventEffect;
}

AgcuUIControlCharacter::stCharacterController* AgcuUIControlCharacter::GetCurrSelectCharacter( void )
{
	return m_mapSelectCharacters.Get( m_nCurrSelectCharacterID );
}

AgcuUIControlCharacter::stCharacterController* AgcuUIControlCharacter::GetCharacterController( CHAR* pCharacterName )
{
	int nCharacterCount = m_mapSelectCharacters.GetSize();

	for( int nCount = 0 ; nCount < nCharacterCount ; ++nCount )
	{
		stCharacterController* pCharacter = m_mapSelectCharacters.GetByIndex( nCount );
		if( pCharacter->IsMyCharacterName( pCharacterName ) )
		{
			return pCharacter;
		}
	}

	return NULL;
}

INT32 AgcuUIControlCharacter::GetSelectAnimationType( void* ppdCharacter, void* pcdCharacter, void* pFactorModule, void* pItemModule, BOOL bIsReturnToBack )
{
	INT32 nSelectAnimType = -1;

	AgpdCharacter* ppdChar = ( AgpdCharacter* )ppdCharacter;
	AgcdCharacter* pcdChar = ( AgcdCharacter* )pcdCharacter;
	AgpmFactors* ppmFactor = ( AgpmFactors* )pFactorModule;
	AgcmItem* pcmItem = ( AgcmItem* )pItemModule;
	if( !ppdChar || !pcdChar || !ppmFactor ) return nSelectAnimType;

	AuRaceType eRaceType = ( AuRaceType )ppmFactor->GetRace( &ppdChar->m_csFactor );
	AuCharClassType eClassType = ( AuCharClassType )ppmFactor->GetClass( &ppdChar->m_csFactor );

	switch( eRaceType )
	{
	case AURACE_TYPE_HUMAN :
	case AURACE_TYPE_ORC :
	case AURACE_TYPE_MOONELF :
		{
			// 기존방식 : 선택이냐 선택해제냐에 따라 맨손 애니메이션을 지정한후 캐릭터에 저장된 숫자를 더해준다.
			nSelectAnimType = bIsReturnToBack ? AGPDCHAR_SOCIAL_TYPE_SELECT1_BACK : AGPDCHAR_SOCIAL_TYPE_SELECT1;
			nSelectAnimType += pcdChar->m_lCurAnimType2;
		}
		break;

	case AURACE_TYPE_DRAGONSCION :
		{
			// 시온의 경우.. 장착중인 무기가 한손제논이면 특정 애니를.. 아니면 걍 일반 애니를 적용한다. ( ㅡ.ㅡ.. 뭐이 이런;;; )
			AgpdItem* ppdIEquipWeapon = pcmItem->GetCurrentEquipWeapon( ppdChar );
			if( ppdIEquipWeapon )
			{
				AgpdItemTemplateEquipWeapon* ppdWeaponTemplate = ( AgpdItemTemplateEquipWeapon* )ppdIEquipWeapon->m_pcsItemTemplate;
				if( ppdWeaponTemplate )
				{
					// 착용중인 무기가 있는 경우
					switch( eClassType )
					{
					case AUCHARCLASS_TYPE_KNIGHT :	nSelectAnimType = bIsReturnToBack ? AGPDCHAR_SOCIAL_TYPE_SELECT9_BACK : AGPDCHAR_SOCIAL_TYPE_SELECT9;	break;
					case AUCHARCLASS_TYPE_RANGER :	
					case AUCHARCLASS_TYPE_SCION :	nSelectAnimType = bIsReturnToBack ? AGPDCHAR_SOCIAL_TYPE_SELECT8_BACK : AGPDCHAR_SOCIAL_TYPE_SELECT8;	break;
					case AUCHARCLASS_TYPE_MAGE :	nSelectAnimType = bIsReturnToBack ? AGPDCHAR_SOCIAL_TYPE_SELECT1_BACK : AGPDCHAR_SOCIAL_TYPE_SELECT1;	break;
					}
				}
			}

			// 착용중인 무기를 못찾았거나 아니면 찾았는데 제논이 아니거나 뭐 이지랄하면 맨손 애니메이션으로 지정
			if( nSelectAnimType < 0 )
			{
				nSelectAnimType = bIsReturnToBack ? AGPDCHAR_SOCIAL_TYPE_SELECT1_BACK : AGPDCHAR_SOCIAL_TYPE_SELECT1;
			}
		}
		break;
	}

	return nSelectAnimType;
}

void AgcuUIControlCharacter::_UpdateModeCharacterCreate( RwReal fElapsed )
{
	if( !m_CreateCharacter.m_pcdCharacter ) return;
	if( !m_CreateCharacter.GetFrame() ) return;

	const RwReal rCoef = 44.0f * 1.5f;
	const RwV3d vAxisAbsoluteY = { 0.0f, 1.0f, 0.0f };

	switch( m_eRotateState )
	{
	case RotateLeft :
		{
			float fSpeed = fElapsed * rCoef * -1.0f;
			RwFrameRotate( m_CreateCharacter.GetFrame(), &vAxisAbsoluteY, fSpeed, rwCOMBINEPRECONCAT );
		}
		break;

	case RotateRight :
		{
			float fSpeed = fElapsed * rCoef;
			RwFrameRotate( m_CreateCharacter.GetFrame(), &vAxisAbsoluteY, fSpeed, rwCOMBINEPRECONCAT );
		}
		break;
	}

	if( g_pEngine )
	{
		if( !g_pEngine->IsMouseLeftBtnDown() )
		{
			m_eRotateState = NoRotate;
		}
	}
}

void AgcuUIControlCharacter::_SelectCharacter( void )
{
	stCharacterController* pSelectCharacter = m_mapSelectCharacters.Get( m_nCurrSelectCharacterID );
	if( pSelectCharacter )
	{
		pSelectCharacter->ChangeState( Selected );
		if( m_pcmEventEffect )
		{
			AgcmEventEffect* pagcmEventEffect = ( AgcmEventEffect* )m_pcmEventEffect;
			BOOL bCheck = pagcmEventEffect->StartSelectionAnimation( ( AgpdCharacter* )pSelectCharacter->m_ppdCharacter,
				( AgcdCharacter* )pSelectCharacter->m_pcdCharacter, CallBackSelectAnimationEnd, FALSE );
		}
	}
}

void AgcuUIControlCharacter::_UnSelectCharacter( void )
{
	stCharacterController* pUnSelectCharacter = m_mapSelectCharacters.Get( m_nCurrSelectCharacterID );
	if( pUnSelectCharacter )
	{
		pUnSelectCharacter->ChangeState( UnSelected );
		pUnSelectCharacter->MoveBackward( m_ppmCharacter, 0.0f );

		if( m_pcmEventEffect )
		{
			AgcmEventEffect* pagcmEventEffect = ( AgcmEventEffect* )m_pcmEventEffect;
			BOOL bCheck = pagcmEventEffect->StartSelectionAnimation( ( AgpdCharacter* )pUnSelectCharacter->m_ppdCharacter,
				( AgcdCharacter* )pUnSelectCharacter->m_pcdCharacter, CallBackSelectAnimationEnd, TRUE );
		}
	}

	m_nCurrSelectCharacterID = 0;
}


 




void* AgcuUIControlCharacter::CallBackSelectAnimationEnd( void* pData1, void* pData2, void* pData3 )
{
	if( m_pSingletonPtr )
	{
		if( m_pSingletonPtr->m_pcmCharacter )
		{
			AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pData2;
			AgcdCharacter* pcdCharacter = ( AgcdCharacter* )pData3;
			if( !ppdCharacter || !pcdCharacter ) return NULL;

			AgcmCharacter* pcmCharacter = ( AgcmCharacter* )m_pSingletonPtr->m_pcmCharacter;
			pcmCharacter->SetNextAnimation( ppdCharacter,	pcdCharacter, AGCMCHAR_ANIM_TYPE_WAIT, TRUE );

			CHAR* pCharacterName = ppdCharacter->m_szID;
			stCharacterController* pController = m_pSingletonPtr->GetCharacterController( pCharacterName );
			if( pController )
			{
				switch( pController->m_eCharacterState )
				{
				case Selected :
					{
						pController->MoveForward( m_pSingletonPtr->m_ppmCharacter, 0.0f );
					}
					break;
				}
			}
		}
	}

	return NULL;
}

void AgcuUIControlCharacter::UnSelectCharacter( void )
{
	_UnSelectCharacter();
}



