#include "CUiCharacterProperty.h"
#include "CharacterPropertyCallBack.h"

#include "AgpmCharacter.h"
#include "AgpmEventSkillMaster.h"
#include "AgcmCharacter.h"
#include "AgcmItem.h"
#include "AgcmUIManager2.h"

#include "AuXmlParser\TinyXML\TinyXML.h"


#define MAX_VALUE_CONTROL_COUNT		22


CUiCharacterProperty::CUiCharacterProperty( void )
{
	m_eCurrentPropertyType = ePropertyType_UnKnown;
	m_pcsAgpmCharacter = 0;
}

CUiCharacterProperty::~CUiCharacterProperty( void )
{
}

BOOL CUiCharacterProperty::OnInitializeProperty( void )
{
	if( !_InitPropertyCallBack() ) return FALSE;
	if( !_LoadPropertyView( "INI\\CharacterPropertyView.xml", TRUE ) ) return FALSE;
	return TRUE;
}

BOOL CUiCharacterProperty::OnClearProperty( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "Character_Status" );
	if( !pcdUI ) return FALSE;

	for( int nCount = 0 ; nCount < MAX_VALUE_CONTROL_COUNT ; nCount++ )
	{
		char strControlName[ 64 ] = { 0, };
		sprintf_s( strControlName, sizeof(strControlName), "Edit_ValueName_%02d", nCount + 1 );

		AgcdUIControl* pcdControlName = pcmUIManager->GetControl( pcdUI, strControlName );
		if( pcdControlName )
		{
			AcUIEdit* pcdEdit = ( AcUIEdit* )pcdControlName->m_pcsBase;
			if( pcdEdit )
			{
				pcdEdit->ClearText();
			}
		}

		char strControlValue[ 64 ] = { 0, };
		sprintf_s( strControlValue, sizeof(strControlValue), "Edit_Value_%02d", nCount + 1 );

		AgcdUIControl* pcdControlValue = pcmUIManager->GetControl( pcdUI, strControlValue );
		if( pcdControlValue )
		{
			AcUIEdit* pcdEdit = ( AcUIEdit* )pcdControlValue->m_pcsBase;
			if( pcdEdit )
			{
				pcdEdit->ClearText();
			}
		}
	}

	return TRUE;
}

BOOL CUiCharacterProperty::OnUpdateProperty( void )
{
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "Character_Status" );
	if( !pcdUI ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	ContainerMap< int, stCharacterPropertyEntry >* pEntries = NULL;
	switch( m_eCurrentPropertyType )
	{
	case ePropertyType_Attack :		pEntries = &m_mapCharacterPropertyAttack;		break;
	case ePropertyType_Defense :	pEntries = &m_mapCharacterPropertyDefense;		break;
	case ePropertyType_Attribute :	pEntries = &m_mapCharacterPropertyAttribute;	break;
	case ePropertyType_PVP :		pEntries = &m_mapCharacterPropertyPVP;			break;
	default :						return FALSE;									break;
	}

	if( !pEntries ) return FALSE;

	int nMaxCount = pEntries->GetSize();
	for( int nCount = 0 ; nCount < nMaxCount ; nCount++ )
	{
		stCharacterPropertyEntry* pEntry = pEntries->GetByIndex( nCount );
		if( pEntry && pEntry->m_fnCallBack )
		{
			pEntry->m_fnCallBack( pEntry, ppdCharacter );
		}
	}

	return TRUE;
}

BOOL CUiCharacterProperty::OnChangeProperty( void )
{
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "Character_Status" );
	if( !pcdUI ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	ContainerMap< int, stCharacterPropertyEntry >* pEntries = NULL;
	switch( m_eCurrentPropertyType )
	{
	case ePropertyType_Attack :		pEntries = &m_mapCharacterPropertyAttack;		break;
	case ePropertyType_Defense :	pEntries = &m_mapCharacterPropertyDefense;		break;
	case ePropertyType_Attribute :	pEntries = &m_mapCharacterPropertyAttribute;	break;
	case ePropertyType_PVP :		pEntries = &m_mapCharacterPropertyPVP;			break;
	default :						return FALSE;									break;
	}

	if( !pEntries ) return FALSE;

	OnClearProperty();
	_ChangeCurrentPropertyTabBtn( m_eCurrentPropertyType );

	int nMaxCount = pEntries->GetSize();
	for( int nCount = 0 ; nCount < nMaxCount ; nCount++ )
	{
		stCharacterPropertyEntry* pEntry = pEntries->GetByIndex( nCount );
		if( pEntry )
		{
			AcUIEdit* pEdit = ( AcUIEdit* )pEntry->m_pEditName;
			if( pEdit )
			{
				pEdit->SetText( pEntry->m_strName, pEntry->m_dwNameColor );
			}
		}
	}

	return TRUE;
}

BOOL CUiCharacterProperty::OnSelectProperty( eCharacterPropertyType eType )
{
	if( eType == m_eCurrentPropertyType ) return TRUE;
	m_eCurrentPropertyType = eType;
	return OnChangeProperty();	
}

BOOL CUiCharacterProperty::RegisterPropertyCallBack( char* pCallBackName, fnCharacterPropertyCallBack fnCallBack )
{
	if( !pCallBackName || strlen( pCallBackName ) <= 0 ) return FALSE;
	
	stCharacterPropertyCallBackEntry* pEntry = m_mapCharacterPropertyCallBack.Get( pCallBackName );
	if( !pEntry )
	{
		stCharacterPropertyCallBackEntry NewEntry;

		strcpy_s( NewEntry.m_strCallBackName, sizeof( char ) * 64, pCallBackName );
		NewEntry.m_fnCallBack = fnCallBack;

		m_mapCharacterPropertyCallBack.Add( pCallBackName, NewEntry );
	}
	else
	{
		pEntry->m_fnCallBack = fnCallBack; 
	}

	return TRUE;
}

int CUiCharacterProperty::CalcCurrentZeroBaseProperty( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType )
{
	if( !ppdCharacter ) return 0;

	INT32 nBase = _GetZeroBaseProperty( ppdCharacter, eType );
	INT32 nPassive = 0;
	INT32 nBuff = 0;
	INT32 nItem = 0;

	switch( eType )
	{
	case eZeroBaseProperty_AttackSpeed :
		{
			// 공속의 경우 무기착용시에만 계산된다.
			AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
			if( ppmItem && ppmItem->IsEquipWeapon( ppdCharacter ) )
			{
				nPassive = _GetZeroBasePropertyByPassive( ppdCharacter, eType );
				nBuff = _GetZeroBasePropertyByBuff( ppdCharacter, eType );
				nItem = _GetZeroBasePropertyByItem( ppdCharacter, eType );	
			}
		}
		break;

	default :
		{
			nPassive = _GetZeroBasePropertyByPassive( ppdCharacter, eType );
			nBuff = _GetZeroBasePropertyByBuff( ppdCharacter, eType );
			nItem = _GetZeroBasePropertyByItem( ppdCharacter, eType );	
		}
		break;
	}

	return nBase + nPassive + nBuff + nItem;
}

int CUiCharacterProperty::CalcCurrentItemProperty( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType )
{
	INT32 nItem = _GetZeroBasePropertyByItem( ppdCharacter, eType );
	return nItem;
}

BOOL CUiCharacterProperty::_InitPropertyCallBack( void )
{
	RegisterPropertyCallBack( "fn_CharacterProperty_PhysicalDamage",			fn_CharacterProperty_PhysicalDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_HeroicDamage",				fn_CharacterProperty_HeroicDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_RateAttack",				fn_CharacterProperty_RateAttack );
	RegisterPropertyCallBack( "fn_CharacterProperty_RateHit",					fn_CharacterProperty_RateHit );
	RegisterPropertyCallBack( "fn_CharacterProperty_DurationCastSkill",			fn_CharacterProperty_DurationCastSkill );
	RegisterPropertyCallBack( "fn_CharacterProperty_DurationSkillCoolTime",		fn_CharacterProperty_DurationSkillCoolTime );
	RegisterPropertyCallBack( "fn_CharacterProperty_SpeedMove",					fn_CharacterProperty_SpeedMove );
	RegisterPropertyCallBack( "fn_CharacterProperty_SpeedAttack",				fn_CharacterProperty_SpeedAttack );
	RegisterPropertyCallBack( "fn_CharacterProperty_RateCritical",				fn_CharacterProperty_RateCritical );
	RegisterPropertyCallBack( "fn_CharacterProperty_CriticalDamage",			fn_CharacterProperty_CriticalDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_IgnoreTargetRegistance",	fn_CharacterProperty_IgnoreTargetRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_IgnoreTargetDefense",		fn_CharacterProperty_IgnoreTargetDefense );
	RegisterPropertyCallBack( "fn_CharacterProperty_AttackRange",				fn_CharacterProperty_AttackRange );

	RegisterPropertyCallBack( "fn_CharacterProperty_PhysicalDefense",			fn_CharacterProperty_PhysicalDefense );
	RegisterPropertyCallBack( "fn_CharacterProperty_HeroicDefense",				fn_CharacterProperty_HeroicDefense );
	RegisterPropertyCallBack( "fn_CharacterProperty_DefenseRate",				fn_CharacterProperty_DefenseRate );
	RegisterPropertyCallBack( "fn_CharacterProperty_PhysicalRegistance",		fn_CharacterProperty_PhysicalRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_BlockRate",					fn_CharacterProperty_BlockRate );
	RegisterPropertyCallBack( "fn_CharacterProperty_SkillBlockRate",			fn_CharacterProperty_SkillBlockRate );
	RegisterPropertyCallBack( "fn_CharacterProperty_EvadeRate", 				fn_CharacterProperty_EvadeRate );
	RegisterPropertyCallBack( "fn_CharacterProperty_DodgeRate", 				fn_CharacterProperty_DodgeRate );
	RegisterPropertyCallBack( "fn_CharacterProperty_CriticalRegistance",		fn_CharacterProperty_CriticalRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_StunRegistance",			fn_CharacterProperty_StunRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_ReflectDamageRate",			fn_CharacterProperty_ReflectDamageRate );
	RegisterPropertyCallBack( "fn_CharacterProperty_RefectDamage",				fn_CharacterProperty_RefectDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_HeroicMeleeRegistance",		fn_CharacterProperty_HeroicMeleeRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_HeroicMissileRegistance",	fn_CharacterProperty_HeroicMissileRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_HeroicMagicRegistance",		fn_CharacterProperty_HeroicMagicRegistance );

	RegisterPropertyCallBack( "fn_CharacterProperty_MagicDamage",				fn_CharacterProperty_MagicDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_MagicRegistance",			fn_CharacterProperty_MagicRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_FireDamage",				fn_CharacterProperty_FireDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_FireRegistance",			fn_CharacterProperty_FireRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_AirDamage",					fn_CharacterProperty_AirDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_AirRegistance",				fn_CharacterProperty_AirRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_WaterDamage",				fn_CharacterProperty_WaterDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_WaterRegistance",			fn_CharacterProperty_WaterRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_EarthDamage",				fn_CharacterProperty_EarthDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_EarthRegistance",			fn_CharacterProperty_EarthRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_IceDamage",					fn_CharacterProperty_IceDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_IceRegistance",				fn_CharacterProperty_IceRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_ThunderDamage",				fn_CharacterProperty_ThunderDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_ThunderRegistance",			fn_CharacterProperty_ThunderRegistance );
	RegisterPropertyCallBack( "fn_CharacterProperty_PoisonDamage",				fn_CharacterProperty_PoisonDamage );
	RegisterPropertyCallBack( "fn_CharacterProperty_PoisonRegistance",			fn_CharacterProperty_PoisonRegistance );

	RegisterPropertyCallBack( "fn_CharacterProperty_RemainTimeAttacker",		fn_CharacterProperty_RemainTimeAttacker );
	RegisterPropertyCallBack( "fn_CharacterProperty_MurdererPoint",				fn_CharacterProperty_MurdererPoint );
	RegisterPropertyCallBack( "fn_CharacterProperty_RemainTimeRemission",		fn_CharacterProperty_RemainTimeRemission );
	RegisterPropertyCallBack( "fn_CharacterProperty_MurdererLevel",				fn_CharacterProperty_MurdererLevel );
	return TRUE;
}

BOOL CUiCharacterProperty::_ChangeCurrentPropertyTabBtn( eCharacterPropertyType eType )
{
	int nTabCount = m_mapCharacterPropertyTabs.GetSize();
	for( int nCount = 0 ; nCount < nTabCount ; nCount++ )
	{
		stCharacterPropertyTab* pTab = m_mapCharacterPropertyTabs.GetByIndex( nCount );
		if( pTab && pTab->m_pControl )
		{
			AcUIButton* pButton = ( AcUIButton* )pTab->m_pControl->m_pcsBase;
			if( pButton )
			{
				int nBtnMode = pTab->m_ePropertyType == eType ? 2 : 1;
				pButton->SetButtonMode( nBtnMode );
			}
		}
	}

	return TRUE;
}

BOOL CUiCharacterProperty::_LoadPropertyView( char* pFileName, BOOL bIsEncript )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	TiXmlDocument Doc;

	if( !bIsEncript )
	{
		if( !Doc.LoadFile( pFileName ) ) return FALSE;
	}
	else
	{
		BOOL bIsResult = FALSE;

		HANDLE hFile = ::CreateFile( pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile == INVALID_HANDLE_VALUE ) return FALSE;

		// 마지막에 NULL 문자열을 추가해야 하니까 파일사이즈 + 1 해서 초기화한다.
		DWORD dwBufferSize = ::GetFileSize( hFile, NULL ) + 1;
		char* pBuffer = new char[ dwBufferSize ];
		memset( pBuffer, 0, sizeof( char ) * dwBufferSize );

		DWORD dwReadByte = 0;
		if( ::ReadFile( hFile, pBuffer, dwBufferSize, &dwReadByte, NULL ) )
		{
			AuMD5Encrypt Cryptor;
			if( Cryptor.DecryptString( "1111", pBuffer, dwReadByte ) )
			{
				Doc.Parse( pBuffer );
				if( !Doc.Error() )
				{
					bIsResult = TRUE;
				}
			}
		}

		delete[] pBuffer;
		pBuffer = NULL;

		::CloseHandle( hFile );
		hFile = NULL;

		if( !bIsResult ) return FALSE;
	}

	TiXmlNode* pNodeTabs = Doc.FirstChild( "CharacterPropertyTabs" );
	if( !_LoadPropertyTabs( pNodeTabs ) ) return FALSE;

	TiXmlNode* pNodeRoot = Doc.FirstChild( "CharacterPropertyView" );
	if( !pNodeRoot ) return FALSE;

	TiXmlNode* pNodeAttack = pNodeRoot->FirstChild( "CharacterPropertyAttack" );
	if( !_LoadProperty( pNodeAttack, &m_mapCharacterPropertyAttack ) ) return FALSE;

	TiXmlNode* pNodeDefense = pNodeRoot->FirstChild( "CharacterPropertyDefense" );
	if( !_LoadProperty( pNodeDefense, &m_mapCharacterPropertyDefense ) ) return FALSE;

	TiXmlNode* pNodeAttribute = pNodeRoot->FirstChild( "CharacterPropertyAttribute" );
	if( !_LoadProperty( pNodeAttribute, &m_mapCharacterPropertyAttribute ) ) return FALSE;

	TiXmlNode* pNodePVP = pNodeRoot->FirstChild( "CharacterPropertyPVP" );
	if( !_LoadProperty( pNodePVP, &m_mapCharacterPropertyPVP ) ) return FALSE;

	return TRUE;
}

BOOL CUiCharacterProperty::_LoadPropertyTabs( void* pNode )
{
	if( !pNode ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "Character_Status" );
	if( !pcdUI ) return FALSE;

	m_mapCharacterPropertyTabs.Clear();

	TiXmlNode* pPropertyTabs = ( TiXmlNode* )pNode;
	TiXmlNode* pPropertyTab = pPropertyTabs->FirstChild( "TabButton" );

	while( pPropertyTab )
	{
		stCharacterPropertyTab NewTab;

		const char* pProtpertyType = pPropertyTab->ToElement()->Attribute( "PropertyType" );
		if( pProtpertyType && strlen( pProtpertyType ) > 0 )
		{
			NewTab.m_ePropertyType = ( eCharacterPropertyType )atoi( pProtpertyType );
		}

		const char* pControlName = pPropertyTab->ToElement()->Attribute( "ControlName" );
		if( pControlName && strlen( pControlName ) > 0 )
		{
			strcpy_s( NewTab.m_strTabControlName, sizeof( char ) * 64, pControlName );
			NewTab.m_pControl = pcmUIManager->GetControl( pcdUI, NewTab.m_strTabControlName );
		}

		const char* pButtonText = pPropertyTab->ToElement()->Attribute( "ButtonText" );
		if( pButtonText && strlen( pButtonText ) > 0 )
		{
			strcpy_s( NewTab.m_strTabName, sizeof( char ) * 32, pButtonText );
		}

		m_mapCharacterPropertyTabs.Add( NewTab.m_ePropertyType, NewTab );
		pPropertyTab = pPropertyTab->NextSibling();
	}

	return TRUE;
}

BOOL CUiCharacterProperty::_LoadProperty( void* pNode, void* pMap )
{
	if( !pNode || !pMap ) return FALSE;

	AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )g_pEngine->GetModule( "AgcmResourceLoader" );
	if( !pcmResourceLoader ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "Character_Status" );
	if( !pcdUI ) return FALSE;

	TiXmlNode* pXmlNode = ( TiXmlNode* )pNode;
	ContainerMap< int, stCharacterPropertyEntry >* pPropertyMap = ( ContainerMap< int, stCharacterPropertyEntry >* )pMap;

	pPropertyMap->Clear();

	TiXmlNode* pNodeProperty = pXmlNode->FirstChild( "Property" );
	while( pNodeProperty )
	{
		stCharacterPropertyEntry NewEntry;

		char* pControlEditName = ( char* )pNodeProperty->ToElement()->Attribute( "NameControl" );
		AgcdUIControl* pControlName = pcmUIManager->GetControl( pcdUI, pControlEditName );
		if( pControlName )
		{
			NewEntry.m_pEditName = pControlName->m_pcsBase;
		}

		char* pControlEditValue = ( char* )pNodeProperty->ToElement()->Attribute( "ValueControl" );
		AgcdUIControl* pControlValue = pcmUIManager->GetControl( pcdUI, pControlEditValue );
		if( pControlValue )
		{
			NewEntry.m_pEditValue = pControlValue->m_pcsBase;
		}

		TiXmlNode* pPropertyName = pNodeProperty->FirstChild( "PropertyName" );
		if( pPropertyName && NewEntry.m_pEditName )
		{
			AcUIEdit* pEdit = ( AcUIEdit* )NewEntry.m_pEditName;

			char* pTextName = ( char* )pPropertyName->ToElement()->Attribute( "String" );
			char* pTextColor = ( char* )pPropertyName->ToElement()->Attribute( "Color" );

			char* pUiMsg = ( char* )pcmUIManager->GetUIMessage( pTextName );
			DWORD dwColor = pcmResourceLoader->GetColor( pTextColor );

			pEdit->ClearText();

			if( pUiMsg && strlen( pUiMsg ) > 0 )
			{
				pEdit->SetText( pUiMsg, dwColor );
				strcpy_s( NewEntry.m_strName, sizeof( char ) * 64, pUiMsg );
			}

			NewEntry.m_dwNameColor = dwColor;
		}

		TiXmlNode* pPropertyValue = pNodeProperty->FirstChild( "PropertyValue" );
		if( pPropertyValue )
		{
			char* pTextFormatType = ( char* )pPropertyValue->ToElement()->Attribute( "FormatType" );
			if( !pTextFormatType || strlen( pTextFormatType ) <= 0 )
			{
				pTextFormatType = "Normal";
			}

			char* pTextFormat = ( char* )pPropertyValue->ToElement()->Attribute( "Format" );
			if( pTextFormat && strlen( pTextFormat ) > 0 )
			{
				if( strcmp( pTextFormatType, "Normal" ) == 0 )
				{
					strcpy_s( NewEntry.m_strValueFormat, sizeof( char ) * 64, pTextFormat );
				}
				else if( strcmp( pTextFormatType, "UiMsg" ) == 0 )
				{
					char* pText = pcmUIManager->GetUIMessage( pTextFormat );
					if( pText && strlen( pTextFormat ) > 0 )
					{
						strcpy_s( NewEntry.m_strValueFormat, sizeof( char ) * 64, pText );
					}
				}
			}

			char* pTextColorNormal = ( char* )pPropertyValue->ToElement()->Attribute( "Color" );
			NewEntry.m_dwValueColor = pcmResourceLoader->GetColor( pTextColorNormal );

			char* pTextColorUpper = ( char* )pPropertyValue->ToElement()->Attribute( "UpperColor" );
			NewEntry.m_dwValueColorUpper = pcmResourceLoader->GetColor( pTextColorUpper );

			char* pTextColorLower = ( char* )pPropertyValue->ToElement()->Attribute( "LowerColor" );
			NewEntry.m_dwValueColorLower = pcmResourceLoader->GetColor( pTextColorLower );
		}

		TiXmlNode* pPropertyCallBack = pNodeProperty->FirstChild( "PropertyCallBack" );
		if( pPropertyCallBack )
		{
			char* pCallBackName = ( char* )pPropertyCallBack->ToElement()->Attribute( "Function" );
			NewEntry.m_fnCallBack = _GetPropertyCallBack( pCallBackName );
		}

		int nIndex = pPropertyMap->GetSize();
		pPropertyMap->Add( nIndex, NewEntry );

		pNodeProperty = pNodeProperty->NextSibling();
	}

	return TRUE;
}

fnCharacterPropertyCallBack CUiCharacterProperty::_GetPropertyCallBack( char* pCallBackName )
{
	if( !pCallBackName || strlen( pCallBackName ) <= 0 ) return NULL;

	stCharacterPropertyCallBackEntry* pEntry = m_mapCharacterPropertyCallBack.Get( pCallBackName );
	if( !pEntry ) return NULL;

	return pEntry->m_fnCallBack;
}

int CUiCharacterProperty::_GetZeroBaseProperty( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType )
{
	switch( eType )
	{
	case eZeroBaseProperty_AttackSpeed :				return 100;		break;
	case eZeroBaseProperty_StunRegistance :				return 0;		break;
	}

	return 0;
}

int CUiCharacterProperty::_GetZeroBasePropertyByPassive( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType )
{
	if( !ppdCharacter ) return 0;
	INT32 nCurrentValue = 0;

	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmEventSkillMaster* ppmEventSkillMaster = ( AgpmEventSkillMaster* )g_pEngine->GetModule( "AgpmEventSkillMaster" );
	if( !ppmSkill || !ppmEventSkillMaster ) return 0;

	// 현재 적용된 모든 패시브스킬을 뒤진다.
	AgpdSkillAttachData* ppdSkillAttachData = ppmSkill->GetAttachSkillData( ( ApBase* )ppdCharacter );
	if( !ppdSkillAttachData ) return 0;

	for( INT32 nCount = 0 ; nCount < AGPMSKILL_MAX_SKILL_USE ; ++nCount )
	{
		AgpdSkill* ppdSkill = ppdSkillAttachData->m_apcsUsePassiveSkill[ nCount ];
		if( ppdSkill )
		{
			AgpdSkillTemplate* ppdSkillTemplate = ( AgpdSkillTemplate* )ppdSkill->m_pcsTemplate;
			INT32 nSkillLevel = ppmSkill->GetSkillLevel( ppdSkill );
			int nCurrentSkillModifiedLevel = ppmSkill->GetModifiedSkillLevel( ( ApBase* )ppdCharacter );

			switch( eType )
			{
			case eZeroBaseProperty_AttackSpeed :
				{
					int nValue = ( int )ppdSkillTemplate->m_fUsedConstFactor[ AGPMSKILL_CONST_POINT_ATKSPEED ][ nSkillLevel + nCurrentSkillModifiedLevel ];
					nCurrentValue += nValue;

					nValue = ( int )ppdSkillTemplate->m_fUsedConstFactor[ AGPMSKILL_CONST_PERCENT_ATKSPEED ][ nSkillLevel + nCurrentSkillModifiedLevel ];
					nCurrentValue += nValue;						

					nValue = ( int )ppdSkillTemplate->m_fUsedConstFactor2[ AGPMSKILL_CONST_POINT_ATKSPEED ][ nSkillLevel + nCurrentSkillModifiedLevel ];
					nCurrentValue += nValue;

					nValue = ( int )ppdSkillTemplate->m_fUsedConstFactor2[ AGPMSKILL_CONST_PERCENT_ATKSPEED ][ nSkillLevel + nCurrentSkillModifiedLevel ];
					nCurrentValue += nValue;						
				}
				break;

			case eZeroBaseProperty_StunRegistance :
				{
					if( ppdSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_STUN_PROTECT )
					{
						int nValue = ( int )ppdSkillTemplate->m_fUsedConstFactor[ AGPMSKILL_CONST_SKILL_RATE ][ nSkillLevel + nCurrentSkillModifiedLevel ];
						nCurrentValue += nValue;
					}
				}
				break;
			}

		}
	}

	return nCurrentValue;
}

int CUiCharacterProperty::_GetZeroBasePropertyByBuff( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType )
{
	if( !ppdCharacter ) return 0;
	INT32 nCurrentValue = 0;

	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmEventSkillMaster* ppmEventSkillMaster = ( AgpmEventSkillMaster* )g_pEngine->GetModule( "AgpmEventSkillMaster" );
	if( !ppmSkill || !ppmEventSkillMaster ) return 0;

	// 현재 활성화된 모든 버프스킬을 뒤진다.
	AgpdSkillAttachData* ppdSkillAttachData = ppmSkill->GetAttachSkillData( ( ApBase* )ppdCharacter );
	if( !ppdSkillAttachData ) return 0;

	for( INT32 nCount = 0 ; nCount < AGPMSKILL_MAX_SKILL_BUFF ; ++nCount )
	{
		if( ppdSkillAttachData->m_astBuffSkillList[ nCount ].pcsSkillTemplate )
		{
			char* pSkillName = ppdSkillAttachData->m_astBuffSkillList[ nCount ].pcsSkillTemplate->m_szName;
			if( pSkillName )
			{
				INT32 nSkillTID = ppdSkillAttachData->m_astBuffSkillList[ nCount ].lSkillTID;
				INT32 nSkillLevel = 0;
				int nCurrentSkillModifiedLevel = 0;

				AgpdSkill* ppdSkill = ppmSkill->GetSkill( ( ApBase* )ppdCharacter, pSkillName );
				AgpdSkillTemplate* ppdSkillTemplate = ppmSkill->GetSkillTemplate( nSkillTID );

				if( ppdSkillTemplate && ppdSkill )
				{
					nSkillLevel = ppmSkill->GetSkillLevel( ppdSkill );
					nCurrentSkillModifiedLevel = ppmSkill->GetModifiedSkillLevel( ( ApBase* )ppdCharacter );
				}
				else if( ppdSkillTemplate )
				{					
					AgpdEventSkillAttachTemplateData* ppdEventAttachData = ppmEventSkillMaster->GetSkillAttachTemplateData( ppdSkillTemplate );

					if( ppdEventAttachData )
					{
						nSkillLevel = ppdEventAttachData->lMaxUpgradeLevel;
						nCurrentSkillModifiedLevel = 0;
					}
				}

				switch( eType )
				{
				case eZeroBaseProperty_AttackSpeed :
					{
						int nValue = ( int )ppdSkillTemplate->m_fUsedConstFactor[ AGPMSKILL_CONST_POINT_ATKSPEED ][ nSkillLevel + nCurrentSkillModifiedLevel ];
						nCurrentValue += nValue;

						nValue = ( int )ppdSkillTemplate->m_fUsedConstFactor[ AGPMSKILL_CONST_PERCENT_ATKSPEED ][ nSkillLevel + nCurrentSkillModifiedLevel ];
						nCurrentValue += nValue;						

						nValue = ( int )ppdSkillTemplate->m_fUsedConstFactor2[ AGPMSKILL_CONST_POINT_ATKSPEED ][ nSkillLevel + nCurrentSkillModifiedLevel ];
						nCurrentValue += nValue;

						nValue = ( int )ppdSkillTemplate->m_fUsedConstFactor2[ AGPMSKILL_CONST_PERCENT_ATKSPEED ][ nSkillLevel + nCurrentSkillModifiedLevel ];
						nCurrentValue += nValue;						
					}
					break;

				case eZeroBaseProperty_StunRegistance :
					{
						if( ppdSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_STUN_PROTECT )
						{
							int nValue = ( int )ppdSkillTemplate->m_fUsedConstFactor[ AGPMSKILL_CONST_SKILL_RATE ][ nSkillLevel + nCurrentSkillModifiedLevel ];
							nCurrentValue += nValue;
						}
					}
					break;
				}

			}
		}
	}

	return nCurrentValue;
}

int CUiCharacterProperty::_GetZeroBasePropertyByItem( AgpdCharacter* ppdCharacter, eCharacterZeroBaseProperty eType )
{
	if( !ppdCharacter ) return 0;
	int nCurrentValue = 0;

	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	AgcmItem* pcmItem = ( AgcmItem* )g_pEngine->GetModule( "AgcmItem" );
	AgpmGrid* ppmGrid = ( AgpmGrid* )g_pEngine->GetModule( "AgpmGrid" );	
	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmItem || !ppmGrid || !pcmItem || !ppmFactor ) return FALSE;

	AgpdItemADChar* ppdItemADChar = ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid* ppdEquipGrid = &ppdItemADChar->m_csEquipGrid;

	if( !m_pcsAgpmCharacter )
		m_pcsAgpmCharacter = (AgpmCharacter*)g_pEngine->GetModule( "AgpmCharacter" );

	if( !m_pcsAgpmCharacter )
		return 0;

	int nCharacterLevel = m_pcsAgpmCharacter->GetLevel( ppdCharacter );
	//ppmFactor->GetValue( &ppdCharacter->m_csFactor, &nCharacterLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL );

	int nSequencialIndex = 0;	
	AgpdGridItem* ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		AgpdItem* ppdItem = ppmItem->GetItem( ppdGridItem->m_lItemID );

		if( !ppdGridItem->IsDurabilityZero() && ppdItem )
		{
			switch( eType )
			{
			case eZeroBaseProperty_AttackSpeed :
				{
					int nValue = _GetItemOptionValuePoint( ppdItem, nCharacterLevel, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED );
					nCurrentValue += nValue;
				}
				break;

			case eZeroBaseProperty_RateCritical :
			case eZeroBaseProperty_CriticalDamage :
			case eZeroBaseProperty_IgnoreTargetRegistance :
			case eZeroBaseProperty_IgnoreTargetDefense :
			case eZeroBaseProperty_CriticalRegistance :
			case eZeroBaseProperty_StunRegistance :
			case eZeroBaseProperty_ReflectDamageRate :
			case eZeroBaseProperty_ReflectDamage :
				{
					int nValue = _GetItemOptionValueSkill( ppdItem, eType, nCharacterLevel );
					nCurrentValue += nValue;
				}
				break;
			}
		}

		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return nCurrentValue;
}

int CUiCharacterProperty::_GetItemOptionValuePoint( AgpdItem* ppdItem, int nCharacterLevel, eAgpdFactorsType eFactorType, int nSubType )
{
	if( !ppdItem || !ppdItem->m_pcsItemTemplate ) return 0;

	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmFactors* ppmFactors = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	AgpmItemConvert* ppmItemConvert = ( AgpmItemConvert* )g_pEngine->GetModule( "AgpmItemConvert" );
	if( !ppmItem || !ppmFactors || !ppmItemConvert ) return 0;

	int nItemOptionValue = 0;
	AgpdItemOptionTemplate* ppdItemOptionTemplate = NULL;

	// Step 1. LinkTID 를 뒤진다.
	for( int nCount = 0 ; nCount < AGPDITEM_LINK_MAX_NUM ; nCount++ )
	{
		int nLinkID = ppdItem->m_pcsItemTemplate->m_alLinkID[ nCount ];
		if( nLinkID > 0 )
		{
			ppdItemOptionTemplate = ppmItem->GetFitLinkTemplate( nLinkID, nCharacterLevel );
			if( ppdItemOptionTemplate )
			{
				nItemOptionValue += _GetFactorItemOptionValue( ppdItemOptionTemplate, eFactorType, nSubType );
			}
		}
	}

	// Step 2. OptionTID 를 뒤진다.
	for( int nCount = 0 ; nCount < AGPDITEM_OPTION_MAX_NUM ; nCount++ )
	{
		ppdItemOptionTemplate = ppmItem->GetItemOptionTemplate( ppdItem->m_aunOptionTID[ nCount ] );
		if( ppdItemOptionTemplate )
		{
			nItemOptionValue += _GetFactorItemOptionValue( ppdItemOptionTemplate, eFactorType, nSubType );
		}
	}

	// Step 3. OptionTIDRune 을 뒤진다.
	for( int nCount = 0 ; nCount < AGPDITEM_OPTION_RUNE_MAX_NUM ; nCount++ )
	{
		// OptionTIDRune 에는 특정 정령석이나 기원석의 TID가 있다. 그러므로 TID에 해당하는 템플릿을 가져와서..
		AgpdItemTemplate* ppdItemTemplate = ppmItem->GetItemTemplate( ppdItem->m_aunOptionTIDRune[ nCount ] );
		if( ppdItemTemplate )
		{
			for( int nCount = 0 ; nCount < AGPDITEM_OPTION_MAX_NUM ; nCount++ )
			{
				// 템플릿의 옵션TID 를 뒤진다.
				AgpdItemOptionTemplate* ppdItemOptionTemplate = ppmItem->GetItemOptionTemplate( ppdItemTemplate->m_alOptionTID[ nCount ] );
				if( ppdItemOptionTemplate )
				{
					nItemOptionValue += _GetFactorItemOptionValue( ppdItemOptionTemplate, eFactorType, nSubType );
				}
			}
		}
	}

	// Step 4. 소켓에 박혀있는 기원석이나 정령석을 뒤진다.
	int nSocketCount = ppmItemConvert->GetNumConvertedSocket( ppdItem );
	AgpdItemConvertADItem* ppdItemConvertData = ppmItemConvert->GetADItem( ppdItem );
	if( !ppdItemConvertData ) return nItemOptionValue;

	for( int nCount = 0 ; nCount < nSocketCount ; nCount++ )
	{
		AgpdItemTemplate* ppdSocketItemTemplate = ppdItemConvertData->m_stSocketAttr[ nCount ].pcsItemTemplate;
		if( ppdSocketItemTemplate )
		{
			for( int nCount = 0 ; nCount < AGPDITEM_OPTION_MAX_NUM ; nCount++ )
			{
				// 소켓에 박힌 기원석이나 정령석 템플릿의 옵션TID 를 뒤진다.
				AgpdItemOptionTemplate* ppdItemOptionTemplate = ppmItem->GetItemOptionTemplate( ppdSocketItemTemplate->m_alOptionTID[ nCount ] );
				if( ppdItemOptionTemplate )
				{
					nItemOptionValue += _GetFactorItemOptionValue( ppdItemOptionTemplate, eFactorType, nSubType );
				}
			}
		}
	}

	return nItemOptionValue;
}

int CUiCharacterProperty::_GetItemOptionValueSkill( AgpdItem* ppdItem, eCharacterZeroBaseProperty eType, int nCharacterLevel )
{
	if( !ppdItem || !ppdItem->m_pcsItemTemplate ) return 0;

	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	AgpmSkill* ppmSkill = ( AgpmSkill* )g_pEngine->GetModule( "AgpmSkill" );
	AgpmFactors* ppmFactors = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	AgpmItemConvert* ppmItemConvert = ( AgpmItemConvert* )g_pEngine->GetModule( "AgpmItemConvert" );
	if( !ppmItem || !ppmFactors || !ppmItemConvert ) return 0;

	int nItemOptionValue = 0;
	AgpdItemOptionTemplate* ppdItemOptionTemplate = NULL;

	// Step 1. LinkTID 를 뒤진다.
	for( int nCount = 0 ; nCount < AGPDITEM_LINK_MAX_NUM ; nCount++ )
	{
		int nLinkID = ppdItem->m_pcsItemTemplate->m_alLinkID[ nCount ];
		if( nLinkID > 0 )
		{
			ppdItemOptionTemplate = ppmItem->GetFitLinkTemplate( nLinkID, nCharacterLevel );
			if( ppdItemOptionTemplate )
			{
				switch( eType )
				{
				case eZeroBaseProperty_RateCritical :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCriticalRate;					break;
				case eZeroBaseProperty_CriticalDamage :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCritical;						break;
				case eZeroBaseProperty_IgnoreTargetRegistance :		nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lIgnoreAttributeDefenseRate;		break;
				case eZeroBaseProperty_IgnoreTargetDefense :		nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lIgnorePhysicalDefenseRate;		break;
				case eZeroBaseProperty_CriticalRegistance :			nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCriticalDefenseRate;			break;
				case eZeroBaseProperty_StunRegistance :																											break;
				case eZeroBaseProperty_ReflectDamageRate :			nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lDamageConvertHPRate;			break;
				case eZeroBaseProperty_ReflectDamage :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lDamageConvertHP;				break;
				}				
			}
		}
	}

	// Step 2. OptionTID 를 뒤진다.
	for( int nCount = 0 ; nCount < AGPDITEM_OPTION_MAX_NUM ; nCount++ )
	{
		ppdItemOptionTemplate = ppmItem->GetItemOptionTemplate( ppdItem->m_aunOptionTID[ nCount ] );
		if( ppdItemOptionTemplate )
		{
			switch( eType )
			{
			case eZeroBaseProperty_RateCritical :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCriticalRate;					break;
			case eZeroBaseProperty_CriticalDamage :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCritical;						break;
			case eZeroBaseProperty_IgnoreTargetRegistance :		nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lIgnoreAttributeDefenseRate;		break;
			case eZeroBaseProperty_IgnoreTargetDefense :		nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lIgnorePhysicalDefenseRate;		break;
			case eZeroBaseProperty_CriticalRegistance :			nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCriticalDefenseRate;			break;
			case eZeroBaseProperty_StunRegistance :																											break;
			case eZeroBaseProperty_ReflectDamageRate :			nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lDamageConvertHPRate;			break;
			case eZeroBaseProperty_ReflectDamage :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lDamageConvertHP;				break;
			}				
		}
	}

	// Step 3. OptionTIDRune 을 뒤진다.
	for( int nCount = 0 ; nCount < AGPDITEM_OPTION_RUNE_MAX_NUM ; nCount++ )
	{
		// OptionTIDRune 에는 특정 정령석이나 기원석의 TID가 있다. 그러므로 TID에 해당하는 템플릿을 가져와서..
		AgpdItemTemplate* ppdItemTemplate = ppmItem->GetItemTemplate( ppdItem->m_aunOptionTIDRune[ nCount ] );
		if( ppdItemTemplate )
		{
			for( int nCount = 0 ; nCount < AGPDITEM_OPTION_MAX_NUM ; nCount++ )
			{
				// 템플릿의 옵션TID 를 뒤진다.
				AgpdItemOptionTemplate* ppdItemOptionTemplate = ppmItem->GetItemOptionTemplate( ppdItemTemplate->m_alOptionTID[ nCount ] );
				if( ppdItemOptionTemplate )
				{
					switch( eType )
					{
					case eZeroBaseProperty_RateCritical :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCriticalRate;					break;
					case eZeroBaseProperty_CriticalDamage :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCritical;						break;
					case eZeroBaseProperty_IgnoreTargetRegistance :		nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lIgnoreAttributeDefenseRate;		break;
					case eZeroBaseProperty_IgnoreTargetDefense :		nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lIgnorePhysicalDefenseRate;		break;
					case eZeroBaseProperty_CriticalRegistance :			nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCriticalDefenseRate;			break;
					case eZeroBaseProperty_StunRegistance :																											break;
					case eZeroBaseProperty_ReflectDamageRate :			nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lDamageConvertHPRate;			break;
					case eZeroBaseProperty_ReflectDamage :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lDamageConvertHP;				break;
					}				
				}
			}
		}
	}

	// Step 4. 소켓에 박혀있는 기원석이나 정령석을 뒤진다.
	int nSocketCount = ppmItemConvert->GetNumConvertedSocket( ppdItem );
	AgpdItemConvertADItem* ppdItemConvertData = ppmItemConvert->GetADItem( ppdItem );
	if( !ppdItemConvertData ) return nItemOptionValue;

	for( int nCount = 0 ; nCount < nSocketCount ; nCount++ )
	{
		AgpdItemTemplate* ppdSocketItemTemplate = ppdItemConvertData->m_stSocketAttr[ nCount ].pcsItemTemplate;
		if( ppdSocketItemTemplate )
		{
			for( int nCount = 0 ; nCount < AGPDITEM_OPTION_MAX_NUM ; nCount++ )
			{
				// 소켓에 박힌 기원석이나 정령석 템플릿의 옵션TID 를 뒤진다.
				AgpdItemOptionTemplate* ppdItemOptionTemplate = ppmItem->GetItemOptionTemplate( ppdSocketItemTemplate->m_alOptionTID[ nCount ] );
				if( ppdItemOptionTemplate )
				{
					switch( eType )
					{
					case eZeroBaseProperty_RateCritical :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCriticalRate;					break;
					case eZeroBaseProperty_CriticalDamage :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCritical;						break;
					case eZeroBaseProperty_IgnoreTargetRegistance :		nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lIgnoreAttributeDefenseRate;		break;
					case eZeroBaseProperty_IgnoreTargetDefense :		nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lIgnorePhysicalDefenseRate;		break;
					case eZeroBaseProperty_CriticalRegistance :			nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lCriticalDefenseRate;			break;
					case eZeroBaseProperty_StunRegistance :																											break;
					case eZeroBaseProperty_ReflectDamageRate :			nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lDamageConvertHPRate;			break;
					case eZeroBaseProperty_ReflectDamage :				nItemOptionValue += ppdItemOptionTemplate->m_stSkillData.m_lDamageConvertHP;				break;
					}				
				}
			}
		}
	}

	return nItemOptionValue;
}

int CUiCharacterProperty::_GetFactorItemOptionValue( AgpdItemOptionTemplate* ppdItemOptionTemplate, eAgpdFactorsType eFactorType, int nSubType )
{
	if( !ppdItemOptionTemplate ) return 0;

	AgpmFactors* ppmFactors = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	if( !ppmFactors ) return 0;

	int nValue = 0;
	if( ppdItemOptionTemplate->m_lPointType == 0 )
	{
		ppmFactors->GetValue( &ppdItemOptionTemplate->m_csFactor, &nValue, eFactorType, nSubType );
	}
	else
	{
		float fValue = 0.0f;
		ppmFactors->GetValue( &ppdItemOptionTemplate->m_csFactorPercent, &fValue, eFactorType, nSubType );
		nValue = ( int )fValue;
	}

	return nValue;
}
