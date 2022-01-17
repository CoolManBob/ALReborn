#include "AgcmUILoginSettingCharacter.h"



void AgcmUILoginSettingCharacter::ClearCharacterData( void )
{
	m_mapSelectPosition.Clear();
	m_mapCreatePosition.Clear();
	m_mapOffSet.Clear();
}

void AgcmUILoginSettingCharacter::AddSelectPosition( int nIndex, RwV3d vPos, RwReal rRotate )
{
	if( nIndex < 0 || nIndex >= MAX_CHARACTER_COUNT_PER_ACCOUNT ) return;

	stPositionSet* pPositionSet = m_mapSelectPosition.Get( nIndex );
	if( pPositionSet )
	{
		pPositionSet->m_vPos = vPos;
		pPositionSet->m_rRotate = rRotate;
	}
	else
	{
		stPositionSet NewPosition;

		NewPosition.m_vPos = vPos;
		NewPosition.m_rRotate = rRotate;

		m_mapSelectPosition.Add( nIndex, NewPosition );
	}
}

void AgcmUILoginSettingCharacter::AddCreatePosition( AuRaceType eRaceType, RwV3d vPos, RwReal rRotate )
{
	stPositionSet* pPositionSet = m_mapCreatePosition.Get( eRaceType );
	if( pPositionSet )
	{
		pPositionSet->m_vPos = vPos;
		pPositionSet->m_rRotate = rRotate;
	}
	else
	{
		stPositionSet NewPosition;

		NewPosition.m_vPos = vPos;
		NewPosition.m_rRotate = rRotate;

		m_mapCreatePosition.Add( eRaceType, NewPosition );
	}
}

void AgcmUILoginSettingCharacter::AddMoveOffset( AuRaceType eRaceType, AuCharClassType eClassType, float fOffSet )
{
	stForwardOffset* pOffSet = m_mapOffSet.Get( eRaceType );
	if( pOffSet )
	{
		// 있으면 찾아서 해당 내용 수정
		pOffSet->SetOffSet( eClassType, fOffSet );
	}
	else
	{
		// 없으면 새로 만들어서 넣어준다.
		stForwardOffset NewOffSet;
		NewOffSet.SetOffSet( eClassType, fOffSet );

		m_mapOffSet.Add( eRaceType, NewOffSet );
	}
}

stPositionSet* AgcmUILoginSettingCharacter::GetSelectPosition( int nIndex )
{
	return m_mapSelectPosition.Get( nIndex );
}

stPositionSet* AgcmUILoginSettingCharacter::GetCreatePosition( AuRaceType eRaceType )
{
	return m_mapCreatePosition.Get( eRaceType );
}

float AgcmUILoginSettingCharacter::GetMoveOffset( AuRaceType eRaceType, AuCharClassType eClassType )
{
	stForwardOffset* pOffSet = m_mapOffSet.Get( eRaceType );
	if( !pOffSet ) return 0.0f;
	return pOffSet->GetOffSet( eClassType );
}

