#ifndef __CLASS_UI_CHARACTER_PROPERTY_H__
#define __CLASS_UI_CHARACTER_PROPERTY_H__




#include "ContainerUtil.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include "AgcdUIManager2.h"


enum eCharacterPropertyType
{
	ePropertyType_UnKnown = 0,
	ePropertyType_Attack,
	ePropertyType_Defense,
	ePropertyType_Attribute,
	ePropertyType_PVP,
};

enum eCharacterZeroBaseProperty
{
	eZeroBaseProperty_UnKnown = 0,
	eZeroBaseProperty_AttackSpeed,
	eZeroBaseProperty_RateCritical,
	eZeroBaseProperty_CriticalDamage,
	eZeroBaseProperty_IgnoreTargetRegistance,
	eZeroBaseProperty_IgnoreTargetDefense,
	eZeroBaseProperty_CriticalRegistance,
	eZeroBaseProperty_StunRegistance,
	eZeroBaseProperty_ReflectDamageRate,
	eZeroBaseProperty_ReflectDamage,
};

typedef BOOL ( *fnCharacterPropertyCallBack )( void* pData, void* pCharacter );

struct stCharacterPropertyTab
{
	eCharacterPropertyType												m_ePropertyType;

	char																m_strTabName[ 32 ];
	char																m_strTabControlName[ 64 ];

	AgcdUIControl*														m_pControl;

	stCharacterPropertyTab( void )
	{
		m_ePropertyType = ePropertyType_UnKnown;
		memset( m_strTabName, 0, sizeof( char ) * 32 );
		memset( m_strTabControlName, 0, sizeof( char ) * 64 );
		m_pControl = NULL;
	}
};

struct stCharacterPropertyEntry
{
	void*																m_pEditName;
	void*																m_pEditValue;

	char																m_strName[ 64 ];
	DWORD																m_dwNameColor;

	char																m_strValueFormat[ 64 ];
	DWORD																m_dwValueColor;
	DWORD																m_dwValueColorUpper;
	DWORD																m_dwValueColorLower;

	fnCharacterPropertyCallBack											m_fnCallBack;

	stCharacterPropertyEntry( void )
	{
		m_pEditName = NULL;
		m_pEditValue = NULL;

		memset( m_strName, 0, sizeof( char ) * 64 );
		m_dwNameColor = 0xFFFFFFFF;

		memset( m_strValueFormat, 0, sizeof( char ) * 64 );
		m_dwValueColor = 0xFFFFFFFF;
		m_dwValueColorUpper = 0xFFFFFFFF;
		m_dwValueColorLower = 0xFFFFFFFF;

		m_fnCallBack = NULL;
	};
};

struct stCharacterPropertyCallBackEntry
{
	char																m_strCallBackName[ 64 ];
	fnCharacterPropertyCallBack											m_fnCallBack;

	stCharacterPropertyCallBackEntry( void )
	{
		memset( m_strCallBackName, 0, sizeof( char ) * 64 );
		m_fnCallBack = NULL;
	};
};

class CUiCharacterProperty
{
private :
	ContainerMap< eCharacterPropertyType, stCharacterPropertyTab >		m_mapCharacterPropertyTabs;

	ContainerMap< int, stCharacterPropertyEntry >						m_mapCharacterPropertyAttack;
	ContainerMap< int, stCharacterPropertyEntry >						m_mapCharacterPropertyDefense;
	ContainerMap< int, stCharacterPropertyEntry >						m_mapCharacterPropertyAttribute;
	ContainerMap< int, stCharacterPropertyEntry >						m_mapCharacterPropertyPVP;

	eCharacterPropertyType												m_eCurrentPropertyType;

	ContainerMap< std::string, stCharacterPropertyCallBackEntry >		m_mapCharacterPropertyCallBack;

	AgpmCharacter *														m_pcsAgpmCharacter;

public :
	CUiCharacterProperty( void );
	virtual ~CUiCharacterProperty( void );

public :
	BOOL							OnInitializeProperty				( void );
	BOOL							OnUpdateProperty					( void );

	BOOL							OnClearProperty						( void );
	BOOL							OnSelectProperty					( eCharacterPropertyType eType );
	BOOL							OnChangeProperty					( void );

public :
	BOOL							RegisterPropertyCallBack			( char* pCallBackName, fnCharacterPropertyCallBack fnCallBack );

public :
	int								CalcCurrentZeroBaseProperty			( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType );
	int								CalcCurrentItemProperty				( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType );

private :
	BOOL							_InitPropertyCallBack				( void );
	BOOL							_ChangeCurrentPropertyTabBtn		( eCharacterPropertyType eType );

	BOOL							_LoadPropertyView					( char* pFileName, BOOL bIsEncript );
	BOOL							_LoadPropertyTabs					( void* pNode );
	BOOL							_LoadProperty						( void* pNode, void* pMap );

	fnCharacterPropertyCallBack		_GetPropertyCallBack				( char* pCallBackName );

	int								_GetZeroBaseProperty				( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType );
	int								_GetZeroBasePropertyByPassive		( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType );
	int								_GetZeroBasePropertyByBuff			( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType );
	int								_GetZeroBasePropertyByItem			( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType );

	int								_GetItemOptionValuePoint			( AgpdItem* ppdItem, int nCharacterLevel, eAgpdFactorsType eFactorType, int nSubType );
	int								_GetItemOptionValueSkill			( AgpdItem* ppdItem, eCharacterZeroBaseProperty eType, int nCharacterLevel );

	int								_GetItemOptionValueRateCritical		( AgpdItem* ppdItem );
	int								_GetItemOptionValueCriticalDamage	( AgpdItem* ppdItem );
	int								_GetFactorItemOptionValue			( AgpdItemOptionTemplate* ppdItemOptionTemplate, eAgpdFactorsType eFactorType, int nSubType );
};



#endif