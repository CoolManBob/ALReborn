#include "AgcmUILoginSettingCamera.h"


void stViewSet::MoveTo( RwV3d vTargetPos )
{
	RwMatrixTranslate( &m_matTM, &vTargetPos, rwCOMBINEREPLACE );
	RwMatrixUpdate( &m_matTM );
}

void stViewSet::MoveForward( float fDistance )
{
	RwV3d vAt = *RwMatrixGetAt( &m_matTM );
	RwV3d vMove = { 0.0f, 0.0f, 0.0f };
	RwV3dScale( &vMove, &vAt, fDistance );
	_MoveBy( vMove );
}

void stViewSet::MoveBackward( float fDistance )
{
	RwV3d vAt = *RwMatrixGetAt( &m_matTM );
	RwV3d vMove = { 0.0f, 0.0f, 0.0f };
	RwV3dScale( &vMove, &vAt, -fDistance );
	_MoveBy( vMove );
}

void stViewSet::MoveLeft( float fDistance )
{
	RwV3d vRight = *RwMatrixGetRight( &m_matTM );
	RwV3d vMove = { 0.0f, 0.0f, 0.0f };
	RwV3dScale( &vMove, &vRight, -fDistance );
	_MoveBy( vMove );
}

void stViewSet::MoveRight( float fDistance )
{
	RwV3d vRight = *RwMatrixGetRight( &m_matTM );
	RwV3d vMove = { 0.0f, 0.0f, 0.0f };
	RwV3dScale( &vMove, &vRight, fDistance );
	_MoveBy( vMove );
}

void stViewSet::MoveUp( float fDistance )
{
	RwV3d vUp = *RwMatrixGetUp( &m_matTM );
	RwV3d vMove = { 0.0f, 0.0f, 0.0f };
	RwV3dScale( &vMove, &vUp, fDistance );
	_MoveBy( vMove );
}

void stViewSet::MoveDown( float fDistance )
{
	RwV3d vUp = *RwMatrixGetUp( &m_matTM );
	RwV3d vMove = { 0.0f, 0.0f, 0.0f };
	RwV3dScale( &vMove, &vUp, -fDistance );
	_MoveBy( vMove );
}

void stViewSet::RotateFront( float fAngle )
{
	RwV3d vAxis = *RwMatrixGetRight( &m_matTM );
	_RotateBy( vAxis, fAngle );
}

void stViewSet::RotateBack( float fAngle )
{
	RwV3d vAxis = *RwMatrixGetRight( &m_matTM );
	_RotateBy( vAxis, -fAngle );
}

void stViewSet::RotateLeft( float fAngle )
{
	RwV3d vAxis = *RwMatrixGetUp( &m_matTM );
	_RotateBy( vAxis, -fAngle );
}

void stViewSet::RotateRight( float fAngle )
{
	RwV3d vAxis = *RwMatrixGetUp( &m_matTM );
	_RotateBy( vAxis, fAngle );
}

void stViewSet::LookAt( RwV3d vTargetPos )
{
	RwV3d vCurrPos = *RwMatrixGetPos( &m_matTM );
	RwV3d vNewAt = { 0.0f, 0.0f, 0.0f };
	
	RwV3dSub( &vNewAt, &vTargetPos, &vCurrPos );
	vNewAt = _NormalizeVector( vNewAt );

	RwV3d vTemp = vNewAt;
	vTemp.y += 1.0f;

	RwV3d vNewRight = { 0.0f, 0.0f, 0.0f };
	RwV3dCrossProduct( &vNewRight, &vNewAt, &vTemp );
	vNewRight = _NormalizeVector( vNewRight );

	RwV3d vNewUp = { 0.0f, 0.0f, 0.0f };
	RwV3dCrossProduct( &vNewUp, &vNewAt, &vNewRight );
	vNewUp = _NormalizeVector( vNewUp );

	// in basically, the up vector must be direct to sky
	if( vNewUp.y < 0.0f )
	{
		RwV3dScale( &vNewUp, &vNewUp, -1.0f );
		RwV3dScale( &vNewRight, &vNewRight, -1.0f );
	}

	m_matTM.at = vNewAt;
	m_matTM.right = vNewRight;
	m_matTM.up = vNewUp;

	RwMatrixUpdate( &m_matTM );
}

void stViewSet::CopyFrom( RwMatrix matTM, float fPerspective )
{
	Initialize();

	RwMatrixCopy( &m_matTM, &matTM );
	m_fPerspective = fPerspective;
}

void stViewSet::_MoveBy( RwV3d vMoveDistance )
{
	RwV3d vCurrPos = *RwMatrixGetPos( &m_matTM );
	RwV3d vNewPos = { 0.0f, 0.0f, 0.0f };

	RwV3dAdd( &vNewPos, &vCurrPos, &vMoveDistance );

	RwMatrixTranslate( &m_matTM, &vNewPos, rwCOMBINEPOSTCONCAT );
	RwMatrixUpdate( &m_matTM );
}

void stViewSet::_RotateBy( RwV3d vAxis, float fAngle )
{
	RwMatrixRotate( &m_matTM, &vAxis, fAngle, rwCOMBINEPOSTCONCAT );
	RwMatrixUpdate( &m_matTM );
}

RwV3d stViewSet::_NormalizeVector( RwV3d vVector )
{
	RwReal flNormal;
	flNormal = ( RwReal )sqrt( vVector.x * vVector.x + vVector.y * vVector.y + vVector.z * vVector.z );

	if( flNormal <= 0.0001f )
	{
		flNormal = 1.0f;
	}

	vVector.x /= flNormal;
	vVector.y /= flNormal;
	vVector.z /= flNormal;

	if( fabs( vVector.x ) < 0.0001f )
	{
		vVector.x = 0.0f;
	}

	if( fabs( vVector.y ) < 0.0001f )
	{
		vVector.y = 0.0f;
	}

	if( fabs( vVector.z ) < 0.0001f )
	{
		vVector.z = 0.0f;
	}

	return vVector;
}




void AgcmUILoginSettingCamera::ClearCameraData( void )
{
	m_CamSetLogin.Initialize();
	m_CamSetCharacterSelect.Initialize();

	m_CamSetCharacterCreate.Clear();

	m_CamSetForHuman.Clear();
	m_CamSetForOrc.Clear();
	m_CamSetForMoonElf.Clear();
	m_CamSetForDragonScion.Clear();
}

void AgcmUILoginSettingCamera::SetLoginTM( RwMatrix matTM, float fPerspective )
{
	m_CamSetLogin.CopyFrom( matTM, fPerspective );
}

void AgcmUILoginSettingCamera::SetSelectTM( RwMatrix matTM, float fPerspective )
{
	m_CamSetCharacterSelect.CopyFrom( matTM, fPerspective );
}

void AgcmUILoginSettingCamera::AddCreateTM( AuRaceType eRace, RwMatrix matTM, float fPerspective )
{
	if( m_CamSetCharacterCreate.Find( eRace ) ) return;

	stViewSet NewSet;
	NewSet.CopyFrom( matTM, fPerspective );

	m_CamSetCharacterCreate.Add( eRace, NewSet );
}

void AgcmUILoginSettingCamera::AddZoomTM( AuRaceType eRace, AuCharClassType eClass, RwMatrix matTM, float fPerspective )
{
	stViewSet NewSet;
	NewSet.CopyFrom( matTM, fPerspective );

	switch( eRace )
	{
	case AURACE_TYPE_HUMAN :
		{
			if( m_CamSetForHuman.Find( eClass ) ) return;
			m_CamSetForHuman.Add( eClass, NewSet );
		}
		break;

	case AURACE_TYPE_ORC :
		{
			if( m_CamSetForOrc.Find( eClass ) ) return;
			m_CamSetForOrc.Add( eClass, NewSet );
		}
		break;

	case AURACE_TYPE_MOONELF :
		{
			if( m_CamSetForMoonElf.Find( eClass ) ) return;
			m_CamSetForMoonElf.Add( eClass, NewSet );
		}
		break;

	case AURACE_TYPE_DRAGONSCION :
		{
			if( m_CamSetForDragonScion.Find( eClass ) ) return;
			m_CamSetForDragonScion.Add( eClass, NewSet );
		}
		break;
	}
}

RwMatrix* AgcmUILoginSettingCamera::GetLoginTM( void )
{
	return &m_CamSetLogin.m_matTM;
}

RwMatrix* AgcmUILoginSettingCamera::GetSelectTM( void )
{
	return &m_CamSetCharacterSelect.m_matTM;
}

RwMatrix* AgcmUILoginSettingCamera::GetCreateTM( AuRaceType eRace )
{
	stViewSet* pViewSet = m_CamSetCharacterCreate.Get( eRace );
	if( !pViewSet ) return NULL;

	return &pViewSet->m_matTM;
}

RwMatrix* AgcmUILoginSettingCamera::GetZoomTM( AuRaceType eRace, AuCharClassType eClass )
{
	stViewSet* pViewSet = NULL;

	switch( eRace )
	{
	case AURACE_TYPE_HUMAN :		pViewSet = m_CamSetForHuman.Get( eClass );		break;
	case AURACE_TYPE_ORC :			pViewSet = m_CamSetForOrc.Get( eClass );		break;
	case AURACE_TYPE_MOONELF :		pViewSet = m_CamSetForMoonElf.Get( eClass );	break;
	case AURACE_TYPE_DRAGONSCION :	pViewSet = m_CamSetForDragonScion.Get( eClass );	break;
	}

	if( !pViewSet ) return NULL;
	return &pViewSet->m_matTM;
}

float AgcmUILoginSettingCamera::GetLoginPerspective( void )
{
	return m_CamSetLogin.m_fPerspective;
}

float AgcmUILoginSettingCamera::GetSelectPerspective( void )
{
	return m_CamSetCharacterSelect.m_fPerspective;
}

float AgcmUILoginSettingCamera::GetCreatePerspective( AuRaceType eRace )
{
	stViewSet* pViewSet = m_CamSetCharacterCreate.Get( eRace );
	if( !pViewSet ) return 1.0f;
	return pViewSet->m_fPerspective;
}

float AgcmUILoginSettingCamera::GetZoomPerspective( AuRaceType eRace, AuCharClassType eClass )
{
	stViewSet* pViewSet = NULL;

	switch( eRace )
	{
	case AURACE_TYPE_HUMAN :		pViewSet = m_CamSetForHuman.Get( eClass );		break;
	case AURACE_TYPE_ORC :			pViewSet = m_CamSetForOrc.Get( eClass );		break;
	case AURACE_TYPE_MOONELF :		pViewSet = m_CamSetForMoonElf.Get( eClass );	break;
	case AURACE_TYPE_DRAGONSCION :	pViewSet = m_CamSetForDragonScion.Get( eClass );	break;
	}

	if( !pViewSet ) return 1.0f;
	return pViewSet->m_fPerspective;
}

