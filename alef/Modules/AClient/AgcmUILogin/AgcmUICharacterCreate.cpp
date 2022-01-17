#include "AgcmUICharacterCreate.h"
#include "AgcmUIManager2.h"
#include "AgcmUILogin.h"
#include "AgcmUICharacter.h"
#include "AgpmEventCharCustomize.h"
#include "AuStrTable.h"





void* AgcmUICharacterCreate::g_pcmUILogin = NULL;
void* AgcmUICharacterCreate::g_pcmUIManager = NULL;
void* AgcmUICharacterCreate::g_pcmUICharacter = NULL;
void* AgcmUICharacterCreate::g_ppmCharacter = NULL;
void* AgcmUICharacterCreate::g_pcmCharacter = NULL;
void* AgcmUICharacterCreate::g_pcmLogin = NULL;
void* AgcmUICharacterCreate::g_ppmFactors = NULL;
void* AgcmUICharacterCreate::g_ppmEventCharCustomize = NULL;





CHAR* GetClassName( AuRaceType eRaceType, AuCharClassType eClassType )
{
	if( !AgcmUICharacterCreate::g_pcmUIManager ) return NULL;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )AgcmUICharacterCreate::g_pcmUIManager;

	CHAR* pUIMsgKeyword = NULL;

	switch( eRaceType )
	{
	case AURACE_TYPE_HUMAN :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	pUIMsgKeyword = UI_MESSAGE_ID_LG_CLASS_KNIGHT;			break;
			case AUCHARCLASS_TYPE_RANGER :	pUIMsgKeyword = UI_MESSAGE_ID_LG_CLASS_ARCHER;			break;
			case AUCHARCLASS_TYPE_MAGE :	pUIMsgKeyword = UI_MESSAGE_ID_LG_CLASS_MAGE;			break;
			default	:						return NULL;											break;
			}
		}
		break;

	case AURACE_TYPE_ORC :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	pUIMsgKeyword = UI_MESSAGE_ID_LG_CLASS_BERSERKER;		break;
			case AUCHARCLASS_TYPE_RANGER :	pUIMsgKeyword = UI_MESSAGE_ID_LG_CLASS_HUNTER;			break;
			case AUCHARCLASS_TYPE_MAGE :	pUIMsgKeyword = UI_MESSAGE_ID_LG_CLASS_SORCERER;		break;
			default	:						return NULL;											break;
			}
		}
		break;

	case AURACE_TYPE_MOONELF :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	return ClientStr().GetStr( STI_SWASHBUKLER );			break;
			case AUCHARCLASS_TYPE_RANGER :	pUIMsgKeyword = UI_MESSAGE_ID_LG_CLASS_RANGER;			break;
			case AUCHARCLASS_TYPE_MAGE :	pUIMsgKeyword = UI_MESSAGE_ID_LG_CLASS_ELEMENTALIST;	break;
			default	:						return NULL;											break;
			}
		}
		break;

	case AURACE_TYPE_DRAGONSCION :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_SCION:	pUIMsgKeyword = UI_MESSAGE_ID_LG_CLASS_SCION;			break;
			default	:						return NULL;											break;
			}
		}
		break;

	default :								return NULL;
	}

	return pcmUIManager->GetUIMessage( pUIMsgKeyword );
}

CHAR* GetRaceDescription( AuRaceType eRaceType )
{
	if( !AgcmUICharacterCreate::g_pcmUIManager ) return NULL;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )AgcmUICharacterCreate::g_pcmUIManager;

	CHAR* pUIMsgKeyword = NULL;

	switch( eRaceType )
	{
	case AURACE_TYPE_HUMAN :		pUIMsgKeyword = UI_MESSAGE_ID_RACE_DESC_HUMAN;		break;
	case AURACE_TYPE_ORC :			pUIMsgKeyword = UI_MESSAGE_ID_RACE_DESC_ORC;		break;
	case AURACE_TYPE_MOONELF :		pUIMsgKeyword = UI_MESSAGE_ID_RACE_DESC_MOONELF;	break;
	case AURACE_TYPE_DRAGONSCION :	pUIMsgKeyword = UI_MESSAGE_ID_RACE_DESC_DRAGONSCION;break;
	}

	return pUIMsgKeyword ? pcmUIManager->GetUIMessage( pUIMsgKeyword ) : NULL;
}

CHAR* GetClassDescription( AuRaceType eRaceType, AuCharClassType eClassType )
{
	if( !AgcmUICharacterCreate::g_pcmUIManager ) return NULL;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )AgcmUICharacterCreate::g_pcmUIManager;

	CHAR* pUIMsgKeyword = NULL;

	switch( eRaceType )
	{
	case AURACE_TYPE_HUMAN :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	pUIMsgKeyword = UI_MESSAGE_ID_CHAR_DESC_HUMAN_KNIGHT;	break;
			case AUCHARCLASS_TYPE_RANGER :	pUIMsgKeyword = UI_MESSAGE_ID_CHAR_DESC_HUMAN_ARCHER;	break;
			case AUCHARCLASS_TYPE_MAGE :	pUIMsgKeyword = UI_MESSAGE_ID_CHAR_DESC_HUMAN_WIZARD;	break;
			default	:						return NULL;											break;
			}
		}
		break;

	case AURACE_TYPE_ORC :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	pUIMsgKeyword = UI_MESSAGE_ID_CHAR_DESC_ORC_BERSERKER;	break;
			case AUCHARCLASS_TYPE_RANGER :	pUIMsgKeyword = UI_MESSAGE_ID_CHAR_DESC_ORC_HUNTER;		break;
			case AUCHARCLASS_TYPE_MAGE :	pUIMsgKeyword = UI_MESSAGE_ID_CHAR_DESC_ORC_SORCERER;	break;
			default	:						return NULL;											break;
			}
		}
		break;

	case AURACE_TYPE_MOONELF :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :	pUIMsgKeyword = UI_MESSAGE_ID_CHAR_DESC_MOONELF_KNIGHT;	break;
			case AUCHARCLASS_TYPE_RANGER :	pUIMsgKeyword = UI_MESSAGE_ID_CHAR_DESC_MOONELF_ARCHER;	break;
			case AUCHARCLASS_TYPE_MAGE :	pUIMsgKeyword = UI_MESSAGE_ID_CHAR_DESC_MOONELF_WIZARD;	break;
			default	:						return NULL;											break;
			}
		}
		break;

	case AURACE_TYPE_DRAGONSCION :
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_SCION:	pUIMsgKeyword = UI_MESSAGE_ID_CHAR_DESC_DRAGONSCION_SCION;	break;
			default	:						return NULL;												break;
			}
		}

	}

	if( !pUIMsgKeyword ) return NULL;
	return pcmUIManager->GetUIMessage( pUIMsgKeyword );
}

void* FindCreateCharacterModel( AuRaceType eRaceType, AuCharClassType eClassType, INT32* pCharacterID )
{
	if( !AgcmUICharacterCreate::g_ppmCharacter ) return NULL;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )AgcmUICharacterCreate::g_ppmCharacter;

	if( !AgcmUICharacterCreate::g_ppmFactors ) return NULL;
	AgpmFactors* ppmFactors = ( AgpmFactors* )AgcmUICharacterCreate::g_ppmFactors;

	if( !AgcmUICharacterCreate::g_pcmUICharacter ) return NULL;
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )AgcmUICharacterCreate::g_pcmUICharacter;

	INT32 nIndex = 0;
	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharSequence( &nIndex );
	if( !ppdCharacter ) return NULL;

	if( eRaceType == AURACE_TYPE_NONE || eClassType == AUCHARCLASS_TYPE_NONE ) return NULL;
	
	while( ppdCharacter )
	{
		AgpdFactor* ppdFactor = &ppdCharacter->m_pcsCharacterTemplate->m_csFactor;
		if( ppmFactors->GetRace( ppdFactor ) == eRaceType && ppmFactors->GetClass( ppdFactor ) == eClassType )
		{
			if( ppdCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD )
			{
				pcmUICharacter->SetCharacter( ppdCharacter );

				if( pCharacterID )
				{
					*pCharacterID = ppdCharacter->m_lID;
				}

				return ppdCharacter;
			}
		}

		ppdCharacter = ppmCharacter->GetCharSequence( &nIndex );
	}

	return NULL;
};






void stSelectableClass::SetClassType( AuRaceType eRaceType, AuCharClassType eClassType )
{
	m_eRaceType = eRaceType;
	m_eClassType = eClassType;

	CHAR* pCharName = GetClassName( m_eRaceType, m_eClassType );
	if( !pCharName ) return;
	strcpy( m_strClassName, pCharName );
}

void stSelectableClass::SelectHeadType( INT32 nHeadType )
{
	if( m_mapSelectableHeadType.Find( nHeadType ) )
	{
		m_nCurrentSelectHeadType = nHeadType;
	}
}

void stSelectableClass::SelectFaceType( INT32 nFaceType )
{
	if( m_mapSelectableFaceType.Find( nFaceType ) )
	{
		m_nCurrentSelectFaceType = nFaceType;
	}
}

INT32 stSelectableClass::GetSelectHeadIndex( void )
{
	stSelectableParts* pHead = m_mapSelectableHeadType.Get( m_nCurrentSelectHeadType );
	if( !pHead ) return -1;

	return pHead->m_nIndex;
}

INT32 stSelectableClass::GetSelectFaceIndex( void )
{
	stSelectableParts* pFace = m_mapSelectableFaceType.Get( m_nCurrentSelectFaceType );
	if( !pFace ) return -1;

	return pFace->m_nIndex;
}

CHAR* stSelectableClass::GetCurrentHeadName( void )
{
	stSelectableParts* pHead = m_mapSelectableHeadType.Get( m_nCurrentSelectHeadType );
	if( !pHead ) return NULL;

	return pHead->m_strPartsName;
}

CHAR* stSelectableClass::GetCurrentFaceName( void )
{
	stSelectableParts* pFace = m_mapSelectableFaceType.Get( m_nCurrentSelectFaceType );
	if( !pFace ) return NULL;

	return pFace->m_strPartsName;
}

void stSelectableClass::LoadHeadTypes( void* pCharacter )
{
	if( !pCharacter ) return;
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;

	if( !AgcmUICharacterCreate::g_ppmEventCharCustomize ) return;
	AgpmEventCharCustomize* ppmEventCharCustomize = ( AgpmEventCharCustomize* )AgcmUICharacterCreate::g_ppmEventCharCustomize;

	if( !AgcmUICharacterCreate::g_ppmFactors ) return;
	AgpmFactors* ppmFactors = ( AgpmFactors* )AgcmUICharacterCreate::g_ppmFactors;

	m_mapSelectableHeadType.Clear();

	for( INT32 nCount = 0 ; nCount < CHARCUSTOMIZE_MAX_LIST ; ++nCount )
	{
		if( !ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_eType ) break;

		CharCustomizeType eType = ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_eType;
		CharCustomizeCase eCase = ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_eCase;

		if( ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_pcsCharacterTemplate )
		{
			AgpdFactor* ppdFactor = &ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_pcsCharacterTemplate->m_csFactor;

			if( eType == CHARCUSTOMIZE_TYPE_HAIR &&
				( eCase == CHARCUSTOMIZE_CASE_ALL || eCase == CHARCUSTOMIZE_CHAR_LOGIN ) &&
				ppmFactors->GetRace( ppdFactor ) == m_eRaceType &&
				ppmFactors->GetClass( ppdFactor ) == m_eClassType )
			{
				stSelectableParts NewParts;

				NewParts.m_nIndex = ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_lNumber;
				strcpy( NewParts.m_strPartsName, ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_szName );

				m_mapSelectableHeadType.Add( m_mapSelectableHeadType.GetSize(), NewParts );
			}
		}
	}
}

void stSelectableClass::LoadFaceTypes( void* pCharacter )
{
	if( !pCharacter ) return;
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;

	if( !AgcmUICharacterCreate::g_ppmEventCharCustomize ) return;
	AgpmEventCharCustomize* ppmEventCharCustomize = ( AgpmEventCharCustomize* )AgcmUICharacterCreate::g_ppmEventCharCustomize;

	if( !AgcmUICharacterCreate::g_ppmFactors ) return;
	AgpmFactors* ppmFactors = ( AgpmFactors* )AgcmUICharacterCreate::g_ppmFactors;

	m_mapSelectableFaceType.Clear();

	for( INT32 nCount = 0 ; nCount < CHARCUSTOMIZE_MAX_LIST ; ++nCount )
	{
		if( !ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_eType ) break;

		CharCustomizeType eType = ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_eType;
		CharCustomizeCase eCase = ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_eCase;

		if( ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_pcsCharacterTemplate )
		{
			AgpdFactor* ppdFactor = &ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_pcsCharacterTemplate->m_csFactor;

			if( eType == CHARCUSTOMIZE_TYPE_FACE &&
				( eCase == CHARCUSTOMIZE_CASE_ALL || eCase == CHARCUSTOMIZE_CHAR_LOGIN ) &&
				ppmFactors->GetRace( ppdFactor ) == m_eRaceType &&
				ppmFactors->GetClass( ppdFactor ) == m_eClassType )
			{
				stSelectableParts NewParts;

				NewParts.m_nIndex = ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_lNumber;
				strcpy( NewParts.m_strPartsName, ppmEventCharCustomize->m_astCustomizeList[ nCount ].m_szName );

				m_mapSelectableFaceType.Add( m_mapSelectableFaceType.GetSize(), NewParts );
			}
		}
	}
}










void stSelectableRace::SetRaceType( AuRaceType eRaceType )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )AgcmUICharacterCreate::g_pcmUIManager;
	if( !pcmUIManager ) return;

	m_eRaceType = eRaceType;
	CHAR* pRaceName = NULL;
	switch( m_eRaceType )
	{
	case AURACE_TYPE_HUMAN :
		pRaceName = pcmUIManager->GetUIMessage( UI_MESSAGE_ID_LG_RACE_HUMAN );
		break;
	case AURACE_TYPE_ORC :
		pRaceName = pcmUIManager->GetUIMessage( UI_MESSAGE_ID_LG_RACE_ORC );
		break;
	case AURACE_TYPE_MOONELF :
		pRaceName = pcmUIManager->GetUIMessage( UI_MESSAGE_ID_LG_RACE_MOONELF );
		break;
	case AURACE_TYPE_DRAGONSCION :
		pRaceName = pcmUIManager->GetUIMessage( UI_MESSAGE_ID_LG_RACE_DRAGONSCION );		
		break;

	}

	if( !pRaceName ) return;

	strcpy( m_strRaceName, pRaceName );
	_LoadClassTypes();
}

BOOL stSelectableRace::SelectClassType( AuCharClassType eClassType )
{
	BOOL bIsCurrentClass = TRUE;

	if( m_mapSelectableClassType.Find( eClassType ) )
	{
		if( m_eCurrentSelectClassType != eClassType )
		{
			bIsCurrentClass = FALSE;
		}

		m_eCurrentSelectClassType = eClassType;
	}

	return bIsCurrentClass;
}

stSelectableClass* stSelectableRace::GetSelectClass( void )
{
	return m_mapSelectableClassType.Get( m_eCurrentSelectClassType );
}

stSelectableClass* stSelectableRace::GetClass( AuCharClassType eClassType )
{
	return m_mapSelectableClassType.Get( eClassType );
}

void stSelectableRace::_LoadClassTypes( void )
{
	m_mapSelectableClassType.Clear();

	_LoadClassType( AUCHARCLASS_TYPE_KNIGHT );
	_LoadClassType( AUCHARCLASS_TYPE_RANGER );
	_LoadClassType( AUCHARCLASS_TYPE_MAGE );
	_LoadClassType( AUCHARCLASS_TYPE_SCION );
}

void stSelectableRace::_LoadClassType( AuCharClassType eClassType )
{
	stSelectableClass NewClassType;
	NewClassType.SetClassType( m_eRaceType, eClassType );

	NewClassType.SelectHeadType( 0 );
	NewClassType.SelectFaceType( 0 );

	m_mapSelectableClassType.Add( eClassType, NewClassType );
}












AgcmUICharacterCreate::AgcmUICharacterCreate( void )
: m_eCurrentSelectRaceType( AURACE_TYPE_NONE ), m_bIsZoomIn( FALSE ), m_ppdCurrentCharacter( NULL )
{
	m_pcdUIUserDataHeadName = NULL;
	m_pcdUIUserDataFaceName = NULL;
	m_pcdUIUserDataRaceDescTitle = NULL;
	m_pcdUIUserDataClassDescTitle = NULL;

	m_pBtnSelectClassKnight = NULL;
	m_pBtnSelectClassArcher = NULL;
	m_pBtnSelectClassMage = NULL;
	m_pBtnSelectClassBerserker = NULL;
	m_pBtnSelectClassHunter = NULL;
	m_pBtnSelectClassSocerer = NULL;
	m_pBtnSelectClassSwashBuckler = NULL;
	m_pBtnSelectClassRanger = NULL;
	m_pBtnSelectClassElementalist = NULL;
	m_pBtnSelectClassScion = NULL;

	m_pEditRaceDesc = NULL;
	m_pEditClassDesc = NULL;

	m_nEventGetBtnClassKnight = -1;
	m_nEventGetBtnClassArcher = -1;
	m_nEventGetBtnClassMage = -1;
	m_nEventGetBtnClassBerserker = -1;
	m_nEventGetBtnClassHunter = -1;
	m_nEventGetBtnClassSocerer = -1;
	m_nEventGetBtnClassSwashBuckler = -1;
	m_nEventGetBtnClassRanger = -1;
	m_nEventGetBtnClassElementalist = -1;
	m_nEventGetBtnClassScion = -1;

	m_nEventGetEditRaceDesc = -1;
	m_nEventGetEditClassDesc = -1;
}

AgcmUICharacterCreate::~AgcmUICharacterCreate( void )
{
	OnClear();
}

BOOL AgcmUICharacterCreate::OnInitialize( void* pcmUILogin )
{
	if( !pcmUILogin ) return FALSE;
	g_pcmUILogin = pcmUILogin;

	OnClear();

	if( !g_pcmUIManager )			g_pcmUIManager			= ( ( AgcmUILogin* )g_pcmUILogin )->GetModule( "AgcmUIManager2" );
	if( !g_pcmUICharacter )			g_pcmUICharacter		= ( ( AgcmUILogin* )g_pcmUILogin )->GetModule( "AgcmUICharacter" );
	if( !g_ppmCharacter )			g_ppmCharacter 			= ( ( AgcmUILogin* )g_pcmUILogin )->GetModule( "AgpmCharacter" );
	if( !g_pcmCharacter )			g_pcmCharacter 			= ( ( AgcmUILogin* )g_pcmUILogin )->GetModule( "AgcmCharacter" );
	if( !g_pcmLogin )				g_pcmLogin				= ( ( AgcmUILogin* )g_pcmUILogin )->GetModule( "AgcmLogin" );
	if( !g_ppmFactors )				g_ppmFactors			= ( ( AgcmUILogin* )g_pcmUILogin )->GetModule( "AgpmFactors" );
	if( !g_ppmEventCharCustomize )	g_ppmEventCharCustomize = ( ( AgcmUILogin* )g_pcmUILogin )->GetModule( "AgpmEventCharCustomize" );

	memset( m_strUserDataHeadName, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
	memset( m_strUserDataFaceName, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
	memset( m_strUserDataRaceDescTitle, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
	memset( m_strUserDataClassDescTitle, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );

	m_bIsZoomIn = FALSE;

	_LoadSelectableRace( AURACE_TYPE_HUMAN );
	_LoadSelectableRace( AURACE_TYPE_ORC );
	_LoadSelectableRace( AURACE_TYPE_MOONELF );
	_LoadSelectableRace( AURACE_TYPE_DRAGONSCION );
	return TRUE;
}

BOOL AgcmUICharacterCreate::OnGetBtnControls( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->ThrowEvent( m_nEventGetBtnClassKnight );
	pcmUIManager->ThrowEvent( m_nEventGetBtnClassArcher );
	pcmUIManager->ThrowEvent( m_nEventGetBtnClassMage );
	pcmUIManager->ThrowEvent( m_nEventGetBtnClassBerserker );
	pcmUIManager->ThrowEvent( m_nEventGetBtnClassHunter );
	pcmUIManager->ThrowEvent( m_nEventGetBtnClassSocerer );
	pcmUIManager->ThrowEvent( m_nEventGetBtnClassSwashBuckler );
	pcmUIManager->ThrowEvent( m_nEventGetBtnClassRanger );
	pcmUIManager->ThrowEvent( m_nEventGetBtnClassElementalist );
	pcmUIManager->ThrowEvent( m_nEventGetBtnClassScion );

	pcmUIManager->ThrowEvent( m_nEventGetEditRaceDesc );
	pcmUIManager->ThrowEvent( m_nEventGetEditClassDesc );
	return TRUE;
}

BOOL AgcmUICharacterCreate::OnAddEvent( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	m_nEventGetBtnClassKnight = pcmUIManager->AddEvent( "CreateCharacter_OnGetBtnClassKnight" );
	if( m_nEventGetBtnClassKnight < 0 ) return FALSE;

	m_nEventGetBtnClassArcher = pcmUIManager->AddEvent( "CreateCharacter_OnGetBtnClassArcher" );
	if( m_nEventGetBtnClassArcher < 0 ) return FALSE;

	m_nEventGetBtnClassMage = pcmUIManager->AddEvent( "CreateCharacter_OnGetBtnClassMage" );
	if( m_nEventGetBtnClassMage < 0 ) return FALSE;

	m_nEventGetBtnClassBerserker = pcmUIManager->AddEvent( "CreateCharacter_OnGetBtnClassBerserker" );
	if( m_nEventGetBtnClassBerserker < 0 ) return FALSE;

	m_nEventGetBtnClassHunter = pcmUIManager->AddEvent( "CreateCharacter_OnGetBtnClassHunter" );
	if( m_nEventGetBtnClassHunter < 0 ) return FALSE;

	m_nEventGetBtnClassSocerer = pcmUIManager->AddEvent( "CreateCharacter_OnGetBtnClassSocerer" );
	if( m_nEventGetBtnClassSocerer < 0 ) return FALSE;

	m_nEventGetBtnClassSwashBuckler = pcmUIManager->AddEvent( "CreateCharacter_OnGetBtnClassSwashBuckler" );
	if( m_nEventGetBtnClassSwashBuckler < 0 ) return FALSE;

	m_nEventGetBtnClassRanger = pcmUIManager->AddEvent( "CreateCharacter_OnGetBtnClassRanger" );
	if( m_nEventGetBtnClassRanger < 0 ) return FALSE;

	m_nEventGetBtnClassElementalist = pcmUIManager->AddEvent( "CreateCharacter_OnGetBtnClassElementalist" );
	if( m_nEventGetBtnClassElementalist < 0 ) return FALSE;

	m_nEventGetBtnClassScion = pcmUIManager->AddEvent( "CreateCharacter_OnGetBtnClassScion" );
	if( m_nEventGetBtnClassScion < 0 ) return FALSE;

	m_nEventGetEditRaceDesc = pcmUIManager->AddEvent( "CreateCharacter_OnGetEditRaceDesc" );
	if( m_nEventGetEditRaceDesc < 0 ) return FALSE;

	m_nEventGetEditClassDesc = pcmUIManager->AddEvent( "CreateCharacter_OnGetEditClassDesc" );
	if( m_nEventGetEditClassDesc < 0 ) return FALSE;
	return TRUE;
}

BOOL AgcmUICharacterCreate::OnAddUserData( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	m_pcdUIUserDataHeadName			= pcmUIManager->AddUserData( "CreateCharacter_UDHeadName",			m_strUserDataHeadName,			sizeof( CHAR ), AGCMUILOGIN_MAX_NAME_LENGTH,	AGCDUI_USERDATA_TYPE_STRING );
	m_pcdUIUserDataFaceName			= pcmUIManager->AddUserData( "CreateCharacter_UDFaceName",			m_strUserDataFaceName,			sizeof( CHAR ), AGCMUILOGIN_MAX_NAME_LENGTH,	AGCDUI_USERDATA_TYPE_STRING );
	m_pcdUIUserDataRaceDescTitle	= pcmUIManager->AddUserData( "CreateCharacter_UDRaceDescTitle",		m_strUserDataRaceDescTitle,		sizeof( CHAR ), AGCMUILOGIN_MAX_NAME_LENGTH,	AGCDUI_USERDATA_TYPE_STRING );
	m_pcdUIUserDataClassDescTitle	= pcmUIManager->AddUserData( "CreateCharacter_UDClassDescTitle",	m_strUserDataClassDescTitle,	sizeof( CHAR ), AGCMUILOGIN_MAX_NAME_LENGTH,	AGCDUI_USERDATA_TYPE_STRING );

	return TRUE;
}

BOOL AgcmUICharacterCreate::OnAddFunction( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->AddFunction( this, "CreateCharacter_OnCheckNickName",			CB_OnBtnCheckNickName, 0 );

	pcmUIManager->AddFunction( this, "CreateCharacter_OnSelectRaceHuman",		CB_OnSelectRaceHuman, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnSelectRaceOrc",			CB_OnSelectRaceOrc, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnSelectRaceMoonElf",		CB_OnSelectRaceMoonElf, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnSelectRaceDragonScion",	CB_OnSelectRaceDragonScion, 0 );

	pcmUIManager->AddFunction( this, "CreateCharacter_OnSelectClassKnight",	CB_OnSelectClassKnight, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnSelectClassArcher",	CB_OnSelectClassArcher, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnSelectClassMage",	CB_OnSelectClassMage, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnSelectClassMonk",	CB_OnSelectClassMonk, 0 );

	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetBtnClassKnight",			CB_OnGetBtnClassKnight, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetBtnClassArcher",			CB_OnGetBtnClassArcher, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetBtnClassMage",			CB_OnGetBtnClassMage, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetBtnClassBerserker",		CB_OnGetBtnClassBerserker, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetBtnClassHunter",			CB_OnGetBtnClassHunter, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetBtnClassSocerer",		CB_OnGetBtnClassSocerer, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetBtnClassSwashBuckler",	CB_OnGetBtnClassSwashBuckler, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetBtnClassRanger",			CB_OnGetBtnClassRanger, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetBtnClassElementalist",	CB_OnGetBtnClassElementalist, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetBtnClassScion",			CB_OnGetBtnClassScion, 0 );

	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetEditRaceDesc",	CB_OnGetEditRaceDesc, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnGetEditClassDesc",	CB_OnGetEditClassDesc, 0 );

	pcmUIManager->AddFunction( this, "CreateCharacter_OnBtnSelectHeadPrev",	CB_OnBtnSelectHeadPrev, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnBtnSelectHeadNext",	CB_OnBtnSelectHeadNext, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnBtnSelectFacePrev",	CB_OnBtnSelectFacePrev, 0 );
	pcmUIManager->AddFunction( this, "CreateCharacter_OnBtnSelectFaceNext",	CB_OnBtnSelectFaceNext, 0 );

	pcmUIManager->AddFunction( this, "LG_RotateLeftStart",				CB_OnBtnRotateLeft, 0 );
	pcmUIManager->AddFunction( this, "LG_RotateRightStart",				CB_OnBtnRotateRight, 0 );
	pcmUIManager->AddFunction( this, "LG_RotateStop",					CB_OnBtnRotateEnd, 0 );
	pcmUIManager->AddFunction( this, "LG_ToggleZoom",					CB_OnBtnToggleZoom, 0 );
	pcmUIManager->AddFunction( this, "LG_CharacterCreateOK",			CB_OnCharacterCreateOK, 1, "Name_Edit" );
	pcmUIManager->AddFunction( this, "LG_CharacterCreateCancel",		CB_OnCharacterCreateNo, 0 );

	return TRUE;
}

BOOL AgcmUICharacterCreate::OnAddDisplay( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->AddDisplay( this, "CreateCharacter_HeadName",			0,	CB_OnDisplayHeadName,		AGCDUI_USERDATA_TYPE_INT32 );
	pcmUIManager->AddDisplay( this, "CreateCharacter_FaceName",			0,	CB_OnDisplayFaceName,		AGCDUI_USERDATA_TYPE_INT32 );
	pcmUIManager->AddDisplay( this, "CreateCharacter_RaceDescTitle",	0,	CB_OnDisplayRaceDescTitle,	AGCDUI_USERDATA_TYPE_INT32 );
	pcmUIManager->AddDisplay( this, "CreateCharacter_ClassDescTitle",	0,	CB_OnDisplayClassDescTitle,	AGCDUI_USERDATA_TYPE_INT32 );
	pcmUIManager->AddDisplay( this, "LoginZoomButton",					0,	CB_OnDisplayBtnToggleZoom,	AGCDUI_USERDATA_TYPE_INT32 );

	return TRUE;
}

BOOL AgcmUICharacterCreate::OnAddCallBack( void* pLogin )
{
	AgcmLogin* pcmLogin = ( AgcmLogin* )pLogin;
	if( !pcmLogin ) return FALSE;

	if( !pcmLogin->SetCallbackRaceBase( CB_OnGetBaseCharacterOfRace, this ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUICharacterCreate::OnClear( void )
{
	m_mapSelectableRaceTypes.Clear();
	m_eCurrentSelectRaceType = AURACE_TYPE_NONE;
	return TRUE;
}

BOOL AgcmUICharacterCreate::OnCheckNickName( TCHAR* pNickName )
{
	if( !pNickName || strlen( pNickName ) == 0 ) return FALSE;

	return TRUE;
}

BOOL AgcmUICharacterCreate::OnSelectRace( INT32 nSelectRaceType )
{
	if( m_mapSelectableRaceTypes.Find( ( AuRaceType )nSelectRaceType ) )
	{
		BOOL bIsCurrRaceType = _IsCurrentRaceType( ( AuRaceType )nSelectRaceType );

		AuCharClassType eClassType = AURACE_TYPE_DRAGONSCION != nSelectRaceType ?  AUCHARCLASS_TYPE_KNIGHT : AUCHARCLASS_TYPE_SCION;
		BOOL bIsCurrClassType = _IsCurrentClassType( eClassType );
		if( bIsCurrRaceType && bIsCurrClassType ) return TRUE;

		stSelectableClass* pClass = _GetCreatableClass( ( AuRaceType )nSelectRaceType, eClassType );
		AgcmUILogin* pcmUILogin = ( AgcmUILogin* )g_pcmUILogin;
		if( !pClass || !pcmUILogin ) return FALSE;

		AgpdCharacter* ppdCharacter = ( AgpdCharacter* )FindCreateCharacterModel( ( AuRaceType )nSelectRaceType,
			eClassType, &pClass->m_nCharacterID );
		if( !ppdCharacter ) return FALSE;

		pClass->LoadHeadTypes( ppdCharacter );
		pClass->LoadFaceTypes( ppdCharacter );

		pcmUILogin->CreateCharacter( ppdCharacter, pClass->m_nCharacterID,
			( AuRaceType )nSelectRaceType, eClassType );

		m_ppdCurrentCharacter = ppdCharacter;
		m_nCreateCharacterID = pClass->m_nCharacterID;
		m_eCurrentSelectRaceType = ( AuRaceType )nSelectRaceType;

		OnSelectClass( eClassType );
		OnSelectHead( 0 );
		OnSelectFace( 0 );

		_ChangeBtnSetSelectClass( m_eCurrentSelectRaceType );
		_UpdateCharacterCreateUI( Race );
	}

	return TRUE;
}

BOOL AgcmUICharacterCreate::OnSelectClass( INT32 nSelectClassType )
{
	stSelectableRace* pCurrRace = _GetSelectRace();
	if( !pCurrRace ) return FALSE;
	if( pCurrRace->m_eCurrentSelectClassType == ( AuCharClassType )nSelectClassType ) return TRUE;

	stSelectableClass* pClass = pCurrRace->GetClass( ( AuCharClassType )nSelectClassType );
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )g_pcmUILogin;
	if( !pClass || !pcmUILogin ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )FindCreateCharacterModel( m_eCurrentSelectRaceType,
		( AuCharClassType )nSelectClassType, &pClass->m_nCharacterID );
	if( !ppdCharacter ) return FALSE;

	pClass->LoadHeadTypes( ppdCharacter );
	pClass->LoadFaceTypes( ppdCharacter );

	pcmUILogin->CreateCharacter( ppdCharacter, pClass->m_nCharacterID,
		m_eCurrentSelectRaceType, ( AuCharClassType )nSelectClassType );

	pCurrRace->SelectClassType( ( AuCharClassType )nSelectClassType );
	m_ppdCurrentCharacter = ppdCharacter;
	m_nCreateCharacterID = pClass->m_nCharacterID;

	OnSelectHead( 0 );
	OnSelectFace( 0 );

	_UpdateCharacterCreateUI( Class );
	return TRUE;
}

BOOL AgcmUICharacterCreate::OnSelectHead( INT32 nSelectHeadType )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return FALSE;

	pCurrClass->SelectHeadType( nSelectHeadType );
	_UpdateCharacterCreateUI( Head );

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pcmCharacter;
	if( pcmCharacter )
	{
		AgpdCharacter* ppdCharacter = ( AgpdCharacter* )FindCreateCharacterModel( _GetSelectRaceType(), _GetSelectClassType(), NULL );
		INT32 nHeadIndex = pCurrClass->GetSelectHeadIndex();

		if( ppdCharacter && nHeadIndex >= 0 )
		{
			pcmCharacter->SetHair( pcmCharacter->GetCharacterData( ppdCharacter ), nHeadIndex );
		}
	}

	return TRUE;
}

BOOL AgcmUICharacterCreate::OnSelectPrevHead( void )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return FALSE;
	return OnSelectHead( pCurrClass->m_nCurrentSelectHeadType - 1 );
}

BOOL AgcmUICharacterCreate::OnSelectNextHead( void )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return FALSE;
	return OnSelectHead( pCurrClass->m_nCurrentSelectHeadType + 1 );
}

BOOL AgcmUICharacterCreate::OnSelectFace( INT32 nSelectFaceType )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return FALSE;

	pCurrClass->SelectFaceType( nSelectFaceType );
	_UpdateCharacterCreateUI( Face );

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pcmCharacter;
	if( pcmCharacter )
	{
		AgpdCharacter* ppdCharacter = ( AgpdCharacter* )FindCreateCharacterModel( _GetSelectRaceType(), _GetSelectClassType(), NULL );
		INT32 nFaceIndex = pCurrClass->GetSelectFaceIndex();

		if( ppdCharacter && nFaceIndex >= 0 )
		{
			pcmCharacter->SetFace( pcmCharacter->GetCharacterData( ppdCharacter ), nFaceIndex );
		}
	}

	return TRUE;
}

BOOL AgcmUICharacterCreate::OnSelectPrevFace( void )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return FALSE;
	return OnSelectFace( pCurrClass->m_nCurrentSelectFaceType - 1 );
}

BOOL AgcmUICharacterCreate::OnSelectNextFace( void )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return FALSE;
	return OnSelectFace( pCurrClass->m_nCurrentSelectFaceType + 1 );
}

BOOL AgcmUICharacterCreate::OnToggleZoom( void )
{
	m_bIsZoomIn = !m_bIsZoomIn;
	return TRUE;
}

//BOOL AgcmUICharacterCreate::OnBtnRaceDragonSion( void )
//{
//	if( !g_pcmUIManager ) return FALSE;
//	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pcmUIManager;
//
//	//if( m_pBtnSelectClassKnight )		m_pBtnSelectClassKnight->m_pcsBase->SetStatus( 1 );
//	//if( m_pBtnSelectClassArcher )		m_pBtnSelectClassArcher->m_pcsBase->SetStatus( 1 );
//	//if( m_pBtnSelectClassMage )			m_pBtnSelectClassMage->m_pcsBase->SetStatus( 1 );
//	//if( m_pBtnSelectClassBerserker )	m_pBtnSelectClassBerserker->m_pcsBase->SetStatus( 1 );
//	//if( m_pBtnSelectClassHunter )		m_pBtnSelectClassHunter->m_pcsBase->SetStatus( 1 );
//	//if( m_pBtnSelectClassSocerer )		m_pBtnSelectClassSocerer->m_pcsBase->SetStatus( 1 );
//	//if( m_pBtnSelectClassSwashBuckler )	m_pBtnSelectClassSwashBuckler->m_pcsBase->SetStatus( 1 );
//	//if( m_pBtnSelectClassRanger )		m_pBtnSelectClassRanger->m_pcsBase->SetStatus( 1 );
//	//if( m_pBtnSelectClassElementalist )	m_pBtnSelectClassElementalist->m_pcsBase->SetStatus( 1 );
//	if( m_pBtnSelectClassScion )		m_pBtnSelectClassScion->m_pcsBase->SetStatus( 1 );
//
//	//memset( m_strUserDataRaceDescTitle, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
//	//sprintf( m_strUserDataRaceDescTitle, "%s - %s",
//	//	ClientStr().GetStr( STI_STATICTEXT_RACE ), ClientStr().GetStr( STI_STATICTEXT_UNKNOWN ) );
//
//	//if( m_pEditRaceDesc )// && pRaceDesc )
//	//{
//	//	m_pEditRaceDesc->ClearText();
//		//m_pEditRaceDesc->SetText( pRaceDesc );
//	//}
//
//	//memset( m_strUserDataClassDescTitle, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
//	//if( m_pEditClassDesc )
//	//{
//	//	m_pEditClassDesc->ClearText();
//	//}
//
//	//AgcmUILogin* pcmUILogin = ( AgcmUILogin* )g_pcmUILogin;
//	//pcmUIManager->SetUserDataRefresh( pcmUILogin->m_pstUserDataDummy );
//
//	CHAR* pMsg = pcmUIManager->GetUIMessage( UI_MESSAGE_ID_ALEART_NOT_SERVICE );
//	if( !pMsg ) return FALSE;
//	return pcmUIManager->ActionMessageOKDialog( pMsg );
//}

BOOL AgcmUICharacterCreate::_LoadSelectableRace( AuRaceType eRaceType )
{
	if( !g_pcmUIManager ) return FALSE;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pcmUIManager;

	stSelectableRace NewRaceType;

	NewRaceType.SetRaceType( eRaceType );
	NewRaceType.SelectClassType( AURACE_TYPE_DRAGONSCION != eRaceType ? AUCHARCLASS_TYPE_KNIGHT : AUCHARCLASS_TYPE_SCION );

	m_mapSelectableRaceTypes.Add( eRaceType, NewRaceType );
	return TRUE;
}

BOOL AgcmUICharacterCreate::_ChangeBtnSetSelectClass( AuRaceType eRaceType )
{
	if( !g_pcmUIManager ) return FALSE;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pcmUIManager;

	switch( eRaceType )
	{
	case AURACE_TYPE_HUMAN :
		{
			if( m_pBtnSelectClassKnight )		m_pBtnSelectClassKnight->m_pcsBase->SetStatus( 0 );
			if( m_pBtnSelectClassArcher )		m_pBtnSelectClassArcher->m_pcsBase->SetStatus( 0 );
			if( m_pBtnSelectClassMage )			m_pBtnSelectClassMage->m_pcsBase->SetStatus( 0 );
			if( m_pBtnSelectClassBerserker )	m_pBtnSelectClassBerserker->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassHunter )		m_pBtnSelectClassHunter->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassSocerer )		m_pBtnSelectClassSocerer->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassSwashBuckler )	m_pBtnSelectClassSwashBuckler->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassRanger )		m_pBtnSelectClassRanger->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassElementalist )	m_pBtnSelectClassElementalist->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassScion )		m_pBtnSelectClassScion->m_pcsBase->SetStatus( 1 );
		}
		break;

	case AURACE_TYPE_ORC :
		{
			if( m_pBtnSelectClassKnight )		m_pBtnSelectClassKnight->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassArcher )		m_pBtnSelectClassArcher->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassMage )			m_pBtnSelectClassMage->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassBerserker )	m_pBtnSelectClassBerserker->m_pcsBase->SetStatus( 0 );
			if( m_pBtnSelectClassHunter )		m_pBtnSelectClassHunter->m_pcsBase->SetStatus( 0 );
			if( m_pBtnSelectClassSocerer )		m_pBtnSelectClassSocerer->m_pcsBase->SetStatus( 0 );
			if( m_pBtnSelectClassSwashBuckler )	m_pBtnSelectClassSwashBuckler->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassRanger )		m_pBtnSelectClassRanger->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassElementalist )	m_pBtnSelectClassElementalist->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassScion )		m_pBtnSelectClassScion->m_pcsBase->SetStatus( 1 );
		}
		break;

	case AURACE_TYPE_MOONELF :
		{
			if( m_pBtnSelectClassKnight )		m_pBtnSelectClassKnight->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassArcher )		m_pBtnSelectClassArcher->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassMage )			m_pBtnSelectClassMage->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassBerserker )	m_pBtnSelectClassBerserker->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassHunter )		m_pBtnSelectClassHunter->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassSocerer )		m_pBtnSelectClassSocerer->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassSwashBuckler )	m_pBtnSelectClassSwashBuckler->m_pcsBase->SetStatus( 0 );
			if( m_pBtnSelectClassRanger )		m_pBtnSelectClassRanger->m_pcsBase->SetStatus( 0 );
			if( m_pBtnSelectClassElementalist )	m_pBtnSelectClassElementalist->m_pcsBase->SetStatus( 0 );
			if( m_pBtnSelectClassScion )		m_pBtnSelectClassScion->m_pcsBase->SetStatus( 1 );
		}
		break;

	case AURACE_TYPE_DRAGONSCION :
		{
			if( m_pBtnSelectClassKnight )		m_pBtnSelectClassKnight->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassArcher )		m_pBtnSelectClassArcher->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassMage )			m_pBtnSelectClassMage->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassBerserker )	m_pBtnSelectClassBerserker->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassHunter )		m_pBtnSelectClassHunter->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassSocerer )		m_pBtnSelectClassSocerer->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassSwashBuckler )	m_pBtnSelectClassSwashBuckler->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassRanger )		m_pBtnSelectClassRanger->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassElementalist )	m_pBtnSelectClassElementalist->m_pcsBase->SetStatus( 1 );
			if( m_pBtnSelectClassScion )		m_pBtnSelectClassScion->m_pcsBase->SetStatus( 0 );
		}
		break;

	}

	return TRUE;
}

stSelectableRace* AgcmUICharacterCreate::_GetSelectRace( void )
{
	return m_mapSelectableRaceTypes.Get( m_eCurrentSelectRaceType );
}

stSelectableClass* AgcmUICharacterCreate::_GetSelectClass( void )
{
	stSelectableRace* pCurrRace = _GetSelectRace();
	if( !pCurrRace ) return NULL;

	return pCurrRace->GetSelectClass();
}

AuRaceType AgcmUICharacterCreate::_GetSelectRaceType( void )
{
	return m_eCurrentSelectRaceType;
}

AuCharClassType	AgcmUICharacterCreate::_GetSelectClassType( void )
{
	stSelectableRace* pCurrRace = _GetSelectRace();
	if( !pCurrRace ) return AUCHARCLASS_TYPE_NONE;

	return pCurrRace->m_eCurrentSelectClassType;
}

stSelectableRace* AgcmUICharacterCreate::_GetCreatableRace( AuRaceType eRaceType )
{
	return m_mapSelectableRaceTypes.Get( eRaceType );
}

stSelectableClass* AgcmUICharacterCreate::_GetCreatableClass( AuRaceType eRaceType, AuCharClassType eClassType )
{
	stSelectableRace* pRace = _GetCreatableRace( eRaceType );
	if( !pRace ) return NULL;

	return pRace->GetClass( eClassType );
}

BOOL AgcmUICharacterCreate::_UpdateCharacterCreateUI( UpdateUILevel eUpdateLevel )
{
	switch( eUpdateLevel )
	{
	case Race :
	case Class :
		{
			_UpdateHead();
			_UpdateFace();
			_UpdateDescription();
		}
		break;

	case Head :	_UpdateHead();	break;
	case Face :	_UpdateFace();	break;
	}

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )g_pcmUILogin;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pcmUIManager;
	if( !pcmUILogin || !pcmUIManager ) return FALSE;

	return pcmUIManager->SetUserDataRefresh( pcmUILogin->m_pstUserDataDummy );
}

BOOL AgcmUICharacterCreate::_UpdateHead( void )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return FALSE;

	memset( m_strUserDataHeadName, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
	strcpy( m_strUserDataHeadName, pCurrClass->GetCurrentHeadName() );
	return TRUE;
}

BOOL AgcmUICharacterCreate::_UpdateFace( void )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return FALSE;

	memset( m_strUserDataFaceName, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
	strcpy( m_strUserDataFaceName, pCurrClass->GetCurrentFaceName() );
	return TRUE;
}

BOOL AgcmUICharacterCreate::_UpdateDescription( void )
{
	stSelectableRace* pCurrentRace = _GetSelectRace();
	if( !pCurrentRace ) return FALSE;

	stSelectableClass* pCurrentClass = _GetSelectClass();
	if( !pCurrentClass ) return FALSE;

	memset( m_strUserDataRaceDescTitle, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
	sprintf( m_strUserDataRaceDescTitle, "%s - %s",
		ClientStr().GetStr( STI_STATICTEXT_RACE ), pCurrentRace->m_strRaceName );

	memset( m_strUserDataClassDescTitle, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
	sprintf( m_strUserDataClassDescTitle, "%s - %s",
		ClientStr().GetStr( STI_STATICTEXT_CLASS ), pCurrentClass->m_strClassName );

	AuRaceType eRace = _GetSelectRaceType();
	AuCharClassType eClass = _GetSelectClassType();

	CHAR* pRaceDesc = GetRaceDescription( eRace );
	CHAR* pClassDesc = GetClassDescription( eRace, eClass );

	if( m_pEditRaceDesc && pRaceDesc )
	{
		m_pEditRaceDesc->ClearText();
		m_pEditRaceDesc->SetText( pRaceDesc );
	}

	if( m_pEditClassDesc && pClassDesc )
	{
		m_pEditClassDesc->ClearText();
		m_pEditClassDesc->SetText( pClassDesc );
	}

	return TRUE;
}

BOOL AgcmUICharacterCreate::_IsCurrentRaceType( AuRaceType eRaceType )
{
	return m_eCurrentSelectRaceType == eRaceType ? TRUE : FALSE;
}

BOOL AgcmUICharacterCreate::_IsCurrentClassType( AuCharClassType eClassType )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return FALSE;

	return pCurrClass->m_eClassType == eClassType ? TRUE : FALSE;
}

INT32 AgcmUICharacterCreate::GetSelectHair( void )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return 0;

	return pCurrClass->GetSelectHeadIndex();
}

INT32 AgcmUICharacterCreate::GetSelectFace( void )
{
	stSelectableClass* pCurrClass = _GetSelectClass();
	if( !pCurrClass ) return 0;

	return pCurrClass->GetSelectFaceIndex();
}

CHAR* AgcmUICharacterCreate::GetBtnToggleZoomText( void )
{
	if( !g_pcmUIManager ) return NULL;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pcmUIManager;

	CHAR* pKeyString = m_bIsZoomIn ? UI_MESSAGE_ID_LOGIN_ZOOM_OUT : UI_MESSAGE_ID_LOGIN_ZOOM_IN;
	return pcmUIManager->GetUIMessage( pKeyString );
}

BOOL AgcmUICharacterCreate::IsCurrentUI( void* pControl )
{
	if( !pControl ) return FALSE;
	AgcdUIControl* pcdControl = ( AgcdUIControl* )pControl;

	AgcdUI* pcdUI = pcdControl->m_pcsParentUI;
	if( !pcdUI ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pcmUIManager;
	if( !pcmUIManager ) return FALSE;

	switch( pcmUIManager->GetUIMode() )
	{
	case AGCDUI_MODE_1024_768 :
		{
			return strcmp( pcdUI->m_szUIName, "UI_LG_CHARCREATE" ) == 0 ? TRUE : FALSE;
		}
		break;

	case AGCDUI_MODE_1280_1024 :
		{
			return strcmp( pcdUI->m_szUIName, "UI_LG_CHARCREATE_1280" ) == 0 ? TRUE : FALSE;
		}
		break;

	case AGCDUI_MODE_1600_1200 :
		{
			return strcmp( pcdUI->m_szUIName, "UI_LG_CHARCREATE_1600" ) == 0 ? TRUE : FALSE;
		}
		break;
	}

	return FALSE;
}

BOOL AgcmUICharacterCreate::CB_OnBtnCheckNickName( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	AgcdUIControl* pControlEditName = ( AgcdUIControl* )pData1;
	if( pControlEditName->m_lType != AcUIBase::TYPE_EDIT ) return FALSE;

	TCHAR strName[ 128 ] = { 0, };
	_tcscpy( strName, ( TCHAR* )( ( AcUIEdit* )( pControlEditName->m_pcsBase ) )->GetText() );

	return pThis->OnCheckNickName( strName );
}

BOOL AgcmUICharacterCreate::CB_OnSelectRaceHuman( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectRace( AURACE_TYPE_HUMAN );
}

BOOL AgcmUICharacterCreate::CB_OnSelectRaceOrc( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectRace( AURACE_TYPE_ORC );
}

BOOL AgcmUICharacterCreate::CB_OnSelectRaceMoonElf( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectRace( AURACE_TYPE_MOONELF );
}

BOOL AgcmUICharacterCreate::CB_OnSelectRaceDragonScion( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectRace( AURACE_TYPE_DRAGONSCION );
}

BOOL AgcmUICharacterCreate::CB_OnSelectClassKnight( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectClass( AUCHARCLASS_TYPE_KNIGHT );
}

BOOL AgcmUICharacterCreate::CB_OnSelectClassArcher( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectClass( AUCHARCLASS_TYPE_RANGER );
}

BOOL AgcmUICharacterCreate::CB_OnSelectClassMage( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectClass( AUCHARCLASS_TYPE_MAGE );
}

BOOL AgcmUICharacterCreate::CB_OnSelectClassMonk( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectClass( AUCHARCLASS_TYPE_SCION );
}

BOOL AgcmUICharacterCreate::CB_OnGetBtnClassKnight( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pBtnSelectClassKnight = ( AgcdUIControl* )pData1;
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetBtnClassArcher( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pBtnSelectClassArcher = ( AgcdUIControl* )pData1;
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetBtnClassMage( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pBtnSelectClassMage = ( AgcdUIControl* )pData1;
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetBtnClassBerserker( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pBtnSelectClassBerserker = ( AgcdUIControl* )pData1;
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetBtnClassHunter( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pBtnSelectClassHunter = ( AgcdUIControl* )pData1;
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetBtnClassSocerer( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pBtnSelectClassSocerer = ( AgcdUIControl* )pData1;
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetBtnClassSwashBuckler( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pBtnSelectClassSwashBuckler = ( AgcdUIControl* )pData1;
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetBtnClassRanger( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;
	
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pBtnSelectClassRanger = ( AgcdUIControl* )pData1;
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetBtnClassScion( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pBtnSelectClassScion = ( AgcdUIControl* )pData1;
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetBtnClassElementalist( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pBtnSelectClassElementalist = ( AgcdUIControl* )pData1;
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetEditRaceDesc( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pEditRaceDesc = ( AcUIEdit* )( ( ( AgcdUIControl* )pData1 )->m_pcsBase );
	pThis->m_pEditRaceDesc->SetTextMaxLength( AGCMUILOGIN_MAX_DESC_LENGTH );
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetEditClassDesc( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	if( !pThis->IsCurrentUI( pData1 ) ) return FALSE;

	pThis->m_pEditClassDesc = ( AcUIEdit* )( ( ( AgcdUIControl* )pData1 )->m_pcsBase );
	pThis->m_pEditClassDesc->SetTextMaxLength( AGCMUILOGIN_MAX_DESC_LENGTH );
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnBtnSelectHeadPrev( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectPrevHead();
}

BOOL AgcmUICharacterCreate::CB_OnBtnSelectHeadNext( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectNextHead();
}

BOOL AgcmUICharacterCreate::CB_OnBtnSelectFacePrev( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectPrevFace();
}

BOOL AgcmUICharacterCreate::CB_OnBtnSelectFaceNext( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	return pThis->OnSelectNextFace();
}

BOOL AgcmUICharacterCreate::CB_OnBtnRotateLeft( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;

	if( !AgcmUICharacterCreate::g_pcmUILogin ) return FALSE;
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )AgcmUICharacterCreate::g_pcmUILogin;

	pcmUILogin->m_ControlCharacter.OnRotateStartLeft();
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnBtnRotateRight( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;

	if( !AgcmUICharacterCreate::g_pcmUILogin ) return FALSE;
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )AgcmUICharacterCreate::g_pcmUILogin;

	pcmUILogin->m_ControlCharacter.OnRotateStartRight();
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnBtnRotateEnd( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;

	if( !AgcmUICharacterCreate::g_pcmUILogin ) return FALSE;
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )AgcmUICharacterCreate::g_pcmUILogin;

	pcmUILogin->m_ControlCharacter.OnRotateEnd();
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnBtnToggleZoom( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;

	if( !AgcmUICharacterCreate::g_pcmUILogin ) return FALSE;
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )AgcmUICharacterCreate::g_pcmUILogin;

	if( !AgcmUICharacterCreate::g_pcmUIManager ) return FALSE;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )AgcmUICharacterCreate::g_pcmUIManager;	

	pThis->OnToggleZoom();
	pcmUILogin->m_ControlCamera.OnClickBtnZoom( TRUE, pThis->IsZoomIn() );
	pcmUIManager->SetUserDataRefresh( pcmUILogin->m_pstUserDataDummy );

	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnCharacterCreateOK( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	AgcdUIControl* pControlEditName = ( AgcdUIControl* )pData1;
	if( pControlEditName->m_lType != AcUIBase::TYPE_EDIT ) return FALSE;
	
	if( !AgcmUICharacterCreate::g_pcmUIManager ) return FALSE;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )AgcmUICharacterCreate::g_pcmUIManager;	

	if( !AgcmUICharacterCreate::g_pcmUILogin ) return FALSE;
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )AgcmUICharacterCreate::g_pcmUILogin;

	if( !AgcmUICharacterCreate::g_pcmLogin ) return FALSE;
	AgcmLogin* pcmLogin = ( AgcmLogin* )AgcmUICharacterCreate::g_pcmLogin;

	if( !AgcmUICharacterCreate::g_ppmCharacter ) return FALSE;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )AgcmUICharacterCreate::g_ppmCharacter;

	if( !AgcmUICharacterCreate::g_pcmCharacter ) return FALSE;
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )AgcmUICharacterCreate::g_pcmCharacter;

	TCHAR strName[ 128 ] = { 0, };
	_tcscpy( strName, ( TCHAR* )( ( AcUIEdit* )( pControlEditName->m_pcsBase ) )->GetText() );

	if( _tcslen( strName ) <= 0 )
	{
		pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_INVALID_CHAR_NAME_INPUT ] );
		return FALSE;
	}
	
	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pThis->GetCreateCharacterID() );
	if( !ppdCharacter )
	{
		pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_CHARACTER_CREATE_NOT_SELECTED ] );
		return FALSE;
	}

	AgcdCharacterTemplate* pcdCharacterTemplate = pcmCharacter->GetTemplateData( ppdCharacter->m_pcsCharacterTemplate );
	pcmCharacter->LoadTemplateAnimation( ppdCharacter->m_pcsCharacterTemplate, pcdCharacterTemplate );

	pcmLogin->SendCreateCharacter( pcmLogin->m_szAccount, pcmLogin->m_szWorldName, ppdCharacter->m_lTID1,
		strName, 0, pcmUILogin->m_lUnion, pThis->GetSelectHair(), pThis->GetSelectFace() );

	pcmUILogin->SetLoginMode( AGCMLOGIN_MODE_WAIT_MY_CHARACTER );

	if( g_pEngine )
	{
		g_pEngine->WaitingDialog( NULL, pcmUIManager->GetUIMessage( "LOGIN_WAITING" ) );
	}
	
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnCharacterCreateNo( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass ) return FALSE;
	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;

	if( !AgcmUICharacterCreate::g_pcmUIManager ) return FALSE;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )AgcmUICharacterCreate::g_pcmUIManager;	
	
	if( !AgcmUICharacterCreate::g_pcmUILogin ) return FALSE;
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )AgcmUICharacterCreate::g_pcmUILogin;	

	if( !AgcmUICharacterCreate::g_pcmUICharacter ) return FALSE;
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )AgcmUICharacterCreate::g_pcmUICharacter;	

	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_CHARACTER_CREATE_CLOSE ] );
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_CHARACTER_CREATE_HELP_CLOSE ] );
	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_MAIN_LOGIN_BUTTONS_OPEN ] );

	pcmUILogin->SetLoginMode( AGCMLOGIN_MODE_CHARACTER_SELECT );
	pcmUICharacter->ReleaseCharacter();
	pcmUILogin->UpdateSlotCharInfo();

	pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_HIDE_COMPEN ] );
	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnDisplayHeadName( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	if( !pClass || !pDisplay ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	sprintf( pDisplay, "%s", pThis->GetCurrentHeadName() );

	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnDisplayFaceName( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	if( !pClass || !pDisplay ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	sprintf( pDisplay, "%s", pThis->GetCurrentFaceName() );

	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnDisplayRaceDescTitle( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	if( !pClass || !pDisplay ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	sprintf( pDisplay, "%s", pThis->GetRaceDescTitle() );

	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnDisplayClassDescTitle( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	if( !pClass || !pDisplay ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	sprintf( pDisplay, "%s", pThis->GetClassDescTitle() );

	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnDisplayBtnToggleZoom( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	if( !pClass || !pDisplay ) return FALSE;

	AgcmUICharacterCreate* pThis = ( AgcmUICharacterCreate* )pClass;
	sprintf( pDisplay, "%s", pThis->GetBtnToggleZoomText() );

	return TRUE;
}

BOOL AgcmUICharacterCreate::CB_OnGetBaseCharacterOfRace( void* pData, void* pClass, void* pCustomData )
{
	if( !AgcmUICharacterCreate::g_pcmUILogin ) return FALSE;
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )AgcmUICharacterCreate::g_pcmUILogin;	

	pcmUILogin->SetLoginMode( AGCMLOGIN_MODE_CHARACTER_CREATE );
	pcmUILogin->OnSelectRace( pcmUILogin->m_lUnion );
	return TRUE;
}
