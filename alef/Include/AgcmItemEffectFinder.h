#ifndef __CLASS_ITEM_EFFECT_FINDER_H__
#define __CLASS_ITEM_EFFECT_FINDER_H__


#include "ContainerUtil.h"


enum AuEECommonEffectTypeCloak
{
	None_Cloak = 0,
	Cloak_Air,
	Cloak_Fire,
	Cloak_Magic,
	Cloak_Ground,
	Cloak_Water,
	Cloak_Soul,
	Cloak_Poison,
	Cloak_Soul2,
	EndOfEnumCloak
};

enum AuEECommonEffectTypeWing
{
	None_Wing = 0,
	Wing_King,
	Wing_Angel,
	Wing_Bat,
	Wing_Devil,
	Wing_15,
	Wing_Judge,
	Wing_Messiah,
	EndOfEnumWing
};


struct stEECommonCharEntryClass
{
	int													m_nClassType;
	int													m_nEffectTypeID;

	stEECommonCharEntryClass( void )
	{
		m_nClassType = -1;
		m_nEffectTypeID = -1;
	}
};

struct stEECommonCharEntryRace
{
	int													m_nRaceType;
	ContainerMap< int, stEECommonCharEntryClass >		m_mapEntry;

	stEECommonCharEntryRace( void )
	{
		m_nRaceType = -1;
		m_mapEntry.Clear();
	}

	void				AddEffectTypeID					( int nClassType, int nEffectTypeID );
	int					GetEffectTypeID					( int nClassType, int nEffectTypeID );
	bool				IsMyEffectTypeID				( int nEffectTypeID );
};

struct stEECommonCharEntry
{
	int													m_nEffectBaseType;
	ContainerMap< int, stEECommonCharEntryRace >		m_mapEntry;

	stEECommonCharEntry( void )
	{
		m_mapEntry.Clear();
	}

	void				AddEffectTypeID					( int nRaceType, int nClassType, int nEffectTypeID );
	int					GetEffectTypeID					( int nRaceType, int nClassType, int nEffectTypeID );
	bool				IsMyEffectTypeID				( int nEffectTypeID );
};

class AgcmItemEffectFinder
{
private :
	ContainerMap< int, stEECommonCharEntry >			m_mapEECommonCharCloak;
	ContainerMap< int, stEECommonCharEntry >			m_mapEECommonCharWing;

public :
	AgcmItemEffectFinder( void );
	~AgcmItemEffectFinder( void );

public :
	void				AddCloakEffectID				( AuEECommonEffectTypeCloak eType, int nRaceType, int nClassType, int nEffectTypeID );
	void				AddWingEffectID					( AuEECommonEffectTypeWing eType, int nRaceType, int nClassType, int nEffectTypeID );

	int					FindEffectIDCloak				( AuEECommonEffectTypeCloak eType, int nRaceType, int nClassType, int nEffectTypeID );
	int					FindEffectIDWing				( AuEECommonEffectTypeWing eType, int nRaceType, int nClassType, int nEffectTypeID );

	int					GetEffectIDCloak				( int nEffectTypeID, int nRaceType, int nClassType );
	int					GetEffectIDWing					( int nEffectTypeID, int nRaceType, int nClassType );

	int					FindEffectTypeCloak				( int nEffectTypeID );
	int					FindEffectTypeWing				( int nEffectTypeID );

	void				LoadCloakEffectID				( void );
	void				LoadWingEffectID				( void );
};



#endif