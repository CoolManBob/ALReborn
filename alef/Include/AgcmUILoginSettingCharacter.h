#ifndef __AGCM_UI_LOGIN_SETTING_CHARACTER_H__
#define __AGCM_UI_LOGIN_SETTING_CHARACTER_H__



#include "rwcore.h"
#include "ApDefine.h"
#include "ContainerUtil.h"



#define MAX_CHARACTER_COUNT_PER_ACCOUNT				3



struct stForwardOffset
{
	float											m_fOffsetKnight;
	float											m_fOffsetRanger;
	float											m_fOffsetMonk;
	float											m_fOffsetMage;

	stForwardOffset( void )
		: m_fOffsetKnight( 0.0f ),  m_fOffsetRanger( 0.0f ), m_fOffsetMonk( 0.0f ), m_fOffsetMage( 0.0f )
	{
	}

	float		GetOffSet							( AuCharClassType eClassType )
	{
		switch( eClassType )
		{
		case AUCHARCLASS_TYPE_KNIGHT :				return m_fOffsetKnight;			break;
		case AUCHARCLASS_TYPE_RANGER :				return m_fOffsetRanger;			break;
		case AUCHARCLASS_TYPE_SCION :				return m_fOffsetMonk;			break;
		case AUCHARCLASS_TYPE_MAGE :				return m_fOffsetMage;			break;
		}

		return 0.0f;
	}

	void		SetOffSet							( AuCharClassType eClassType, float fOffSet )
	{
		switch( eClassType )
		{
		case AUCHARCLASS_TYPE_KNIGHT :				m_fOffsetKnight = fOffSet;		break;
		case AUCHARCLASS_TYPE_RANGER :				m_fOffsetRanger = fOffSet;		break;
		case AUCHARCLASS_TYPE_SCION :				m_fOffsetMonk = fOffSet;		break;
		case AUCHARCLASS_TYPE_MAGE :				m_fOffsetMage = fOffSet;		break;
		}
	}
};


struct stPositionSet
{
	RwV3d											m_vPos;
	RwReal											m_rRotate;

	stPositionSet( void )
	{
		m_vPos.x = 0.0f;
		m_vPos.y = 0.0f;
		m_vPos.z = 0.0f;
		m_rRotate = 0.0f;
	}
};


class AgcmUILoginSettingCharacter
{
private :
	// Character's postions in Character Select mode
	ContainerMap< int, stPositionSet >				m_mapSelectPosition;

	// Character's create position  in Character Create Mode
	ContainerMap< AuRaceType, stPositionSet >		m_mapCreatePosition;

	// Character's offset distance for position setting
	ContainerMap< AuRaceType, stForwardOffset >		m_mapOffSet;

public :
	AgcmUILoginSettingCharacter( void )				{ 	}
	~AgcmUILoginSettingCharacter( void )			{	}

public :
	void			ClearCharacterData				( void );

	void			AddSelectPosition				( int nIndex, RwV3d vPos, RwReal rRotate );
	stPositionSet*	GetSelectPosition				( int nIndex );

	void			AddCreatePosition				( AuRaceType eRaceType, RwV3d vPos, RwReal rRotate );
	stPositionSet*	GetCreatePosition				( AuRaceType eRaceType );

	void			AddMoveOffset					( AuRaceType eRaceType, AuCharClassType eClassType, float fOffSet );
	float			GetMoveOffset					( AuRaceType eRaceType, AuCharClassType eClassType );
};



#endif