#include "AgcmUILoginSetting.h"
#include "AuMD5Encrypt.h"
#include "rwplcore.h"


#ifndef USE_MFC
#ifdef _AREA_KOREA_
	#include "CWebzenAuth.h"
#endif
	#include "AuJapaneseClientAuth.h"
#endif

#define HASH_KEY_STRING "1111"


// string stream loader

void String_stream::Empty( void )
{
	if( pString )
	{
		delete[] pString;
		pString = NULL;
	}

	pCurrent = NULL;
	nBufferSize	= 0;
}

bool String_stream::readfile( const char* pFilename, bool bDecryption, char* pReadType )
{
	Empty();

	FILE* pFile	= NULL;
	fopen_s( &pFile, pFilename , pReadType );
	if( !pFile )
	{
		return false;
	}

	fseek( pFile, 0 , SEEK_END );
	nBufferSize = ftell( pFile );
	fseek( pFile, 0 , SEEK_SET );

	pString = new char[ nBufferSize ];
	memset( pString, 0, nBufferSize );
	size_t lReadBytes = fread( pString, sizeof( char ), nBufferSize, pFile );

	fclose( pFile );

	if( bDecryption )
	{
		AuMD5Encrypt MD5;

#ifdef _AREA_CHINA_
		if( !MD5.DecryptString( MD5_HASH_KEY_STRING, pString, ( unsigned long )nBufferSize ) )
#else
		if( !MD5.DecryptString( HASH_KEY_STRING, pString, ( unsigned long )nBufferSize ) )
#endif
		{
			Empty();
			return false;
		}
	}

	pCurrent = pString;
	return true;
}

char* String_stream::sgetf( char* pBuffer )
{
	char* pNext = strstr( pCurrent, "\n" );
	if( !pNext )
	{
		strcpy( pBuffer, pCurrent );
		pCurrent = NULL;
	}
	else
	{
		int	nSize = pNext - pCurrent;
		strncpy( pBuffer, pCurrent , pNext - pCurrent );
		pBuffer[ nSize ] = '\0';
		pCurrent = pNext + 1 ;
	}

	return pCurrent;
}

bool String_stream::seof( void )
{
	if( ( size_t )( pCurrent - pString ) >= nBufferSize || pCurrent == NULL ) return true;
	else return false;
}	





static AgcmUILoginSetting* g_pagcmUILoginSetting = NULL;
AgcmUILoginSetting* GetUILoginSetting( void )
{
	if( !g_pagcmUILoginSetting )
	{
		g_pagcmUILoginSetting = new AgcmUILoginSetting;
	}

	return g_pagcmUILoginSetting;
}

void DestroyLoginSettingData( void )
{
	if( g_pagcmUILoginSetting )
	{
		delete g_pagcmUILoginSetting;
		g_pagcmUILoginSetting = NULL;
	}
}



BOOL AgcmUILoginSetting::LoadSettingFromFile( CHAR* pFileName, BOOL bDescript )
{
	_ClearData();
	_RegisterDataHeaders();	

	String_stream Stream;
	if( !Stream.readfile( pFileName, bDescript ? true : false, "rt" ) ) return FALSE;

	CHAR strBufferLine[ MAX_PATH ] = { 0, };
	CHAR strBufferWord[ MAX_PATH ] = { 0, };

	LoginSettingDataType eDataType = NoData;

	while( !Stream.seof() )
	{
		memset( strBufferLine, 0, sizeof( CHAR ) * MAX_PATH );
		if( !Stream.sgetf( strBufferLine ) )
		{
			Stream.Empty();
			return FALSE;
		}

		int nLength = strlen( strBufferLine );
		if( strBufferLine[ nLength - 1 ] == '\n' )
		{
			strBufferLine[ nLength - 1 ] = '\0';
		}

		// if line is comment, pass this line
		if( strBufferLine[ 0 ] == '/' && strBufferLine[ 1 ] == '/' ) continue;

		// if line has no characters, pass this line
		if( strBufferLine[ 0 ] == '\0' ) continue;

		// if line is data's header
		if( strBufferLine[ 0 ] == '[' )
		{
			eDataType = _ParseHeaderDataType( strBufferLine );

			switch( eDataType )
			{
			case LoginDivision :				_ReadLoginDivisionNumber( &Stream );		break;
			case CharacterSelect_Position :		_AddCharacterSelectPosition( &Stream );		break;
			case CharacterSelect_MoveOffSet :	_AddCharacterSelectMoveOffset( &Stream );	break;
			case CharacterCreate_Position :		_AddCharacterCreatePosition( &Stream );		break;
			case CameraTM_Login :				_AddCameraLoginTM( &Stream );				break;
			case CameraTM_CharacterSelect :		_AddCameraCharacterSelectTM( &Stream );		break;
			case CameraTM_CharacterCreate :		_AddCameraCharacterCreateTM( &Stream );		break;
			case CameraTM_CharacterCreateZoom :	_AddCameraCharacterZoomTM( &Stream );		break;
			}
		}
	}

	Stream.Empty();
	return TRUE;
}

void AgcmUILoginSetting::_ClearData( void )
{
	m_mapHeader.Clear();
	m_mapRaceType.Clear();
	m_mapClassType.Clear();

	m_nLoginDivision = 0;

	ClearCameraData();
	ClearCharacterData();
}

void AgcmUILoginSetting::_RegisterDataHeaders( void )
{
	m_mapHeader.Add( "[ NoData ]",							NoData );
	m_mapHeader.Add( "[ LoginDivision ]",					LoginDivision );
	m_mapHeader.Add( "[ CharacterSelect_Position ]",		CharacterSelect_Position );
	m_mapHeader.Add( "[ CharacterSelect_MoveOffSet ]",		CharacterSelect_MoveOffSet );
	m_mapHeader.Add( "[ CharacterCreate_Position ]",		CharacterCreate_Position );
	m_mapHeader.Add( "[ CameraTM_Login ]",					CameraTM_Login );
	m_mapHeader.Add( "[ CameraTM_CharacterSelect ]", 		CameraTM_CharacterSelect );
	m_mapHeader.Add( "[ CameraTM_CharacterCreate ]", 		CameraTM_CharacterCreate );
	m_mapHeader.Add( "[ CameraTM_CharacterCreateZoom ]",	CameraTM_CharacterCreateZoom );

	m_mapRaceType.Add( "Human",			AURACE_TYPE_HUMAN );
	m_mapRaceType.Add( "Orc",			AURACE_TYPE_ORC );
	m_mapRaceType.Add( "MoonElf",		AURACE_TYPE_MOONELF );
	m_mapRaceType.Add( "DragonScion",	AURACE_TYPE_DRAGONSCION );

	m_mapClassType.Add( "Knight",		AUCHARCLASS_TYPE_KNIGHT );
	m_mapClassType.Add( "Ranger",		AUCHARCLASS_TYPE_RANGER );
	m_mapClassType.Add( "Scion",		AUCHARCLASS_TYPE_SCION );
	m_mapClassType.Add( "Mage",			AUCHARCLASS_TYPE_MAGE );	
}

LoginSettingDataType AgcmUILoginSetting::_ParseHeaderDataType( CHAR* pString )
{
	if( !pString ) return NoData;

	LoginSettingDataType* peDataType = m_mapHeader.Get( pString );
	if( !peDataType ) return NoData;

	return *peDataType;
}

AuRaceType AgcmUILoginSetting::_ParseRaceName( CHAR* pString )
{
	if( !pString ) return AURACE_TYPE_NONE;

	AuRaceType* peRaceType = m_mapRaceType.Get( pString );
	if( !peRaceType ) return AURACE_TYPE_NONE;

	return *peRaceType;
}

AuCharClassType AgcmUILoginSetting::_ParseClassName( CHAR* pString )
{
	if( !pString ) return AUCHARCLASS_TYPE_NONE;

	AuCharClassType* peClassType = m_mapClassType.Get( pString );
	if( !peClassType ) return AUCHARCLASS_TYPE_NONE;

	return *peClassType;
}

void AgcmUILoginSetting::_ReadLoginDivisionNumber( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer[ MAX_PATH ] = { 0, };
	if( !pData->sgetf( strBuffer ) ) return;

	sscanf( strBuffer, "DivisionNumber=%d", &m_nLoginDivision );
}

void AgcmUILoginSetting::_AddCharacterSelectPosition( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	// read 3 lines for 3 characters
	CHAR strBuffer1[ MAX_PATH ] = { 0, };
	CHAR strBuffer2[ MAX_PATH ] = { 0, };
	CHAR strBuffer3[ MAX_PATH ] = { 0, };

	if( !pData->sgetf( strBuffer1 ) ) return;
	if( !pData->sgetf( strBuffer2 ) ) return;
	if( !pData->sgetf( strBuffer3 ) ) return;

	RwV3d vPos1 = { 0.0f, 0.0f, 0.0f };
	float fRotate1 = 0.0f;
	sscanf( strBuffer1, "Pos : X=%f, Y=%f, Z=%f, Rotate : %f", &vPos1.x, &vPos1.y, &vPos1.z, &fRotate1 );

	RwV3d vPos2 = { 0.0f, 0.0f, 0.0f };
	float fRotate2 = 0.0f;
	sscanf( strBuffer2, "Pos : X=%f, Y=%f, Z=%f, Rotate : %f", &vPos2.x, &vPos2.y, &vPos2.z, &fRotate2 );

	RwV3d vPos3 = { 0.0f, 0.0f, 0.0f };
	float fRotate3 = 0.0f;
	sscanf( strBuffer3, "Pos : X=%f, Y=%f, Z=%f, Rotate : %f", &vPos3.x, &vPos3.y, &vPos3.z, &fRotate3 );

	AddSelectPosition( 0, vPos1, fRotate1 );
	AddSelectPosition( 1, vPos2, fRotate2 );
	AddSelectPosition( 2, vPos3, fRotate3 );
}

void AgcmUILoginSetting::_AddCharacterSelectMoveOffset( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer[ MAX_PATH ] = { 0, };
	if( !pData->sgetf( strBuffer ) ) return;

	while( strBuffer[ 0 ] == 'R' )
	{
		CHAR strRaceName[ 16 ] = { 0, };
		CHAR strClassName[ 16 ] = { 0, };
		float fOffSet = 0.0f;

		sscanf( strBuffer, "Race=%s Class=%s OffSet=%f", strRaceName, strClassName, &fOffSet );

		AuRaceType eRaceType = _ParseRaceName( strRaceName );
		AuCharClassType eClassType = _ParseClassName( strClassName );

		AddMoveOffset( eRaceType, eClassType, fOffSet );
		if( !pData->sgetf( strBuffer ) ) return;
	}
}

void AgcmUILoginSetting::_AddCharacterCreatePosition( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer[ MAX_PATH ] = { 0, };
	if( !pData->sgetf( strBuffer ) ) return;

	while( strBuffer[ 0 ] == 'R' )
	{
		CHAR strRaceName[ 16 ] = { 0, };
		RwV3d vPos = { 0.0f, 0.0f, 0.0f };
		RwReal rRotate = 0.0f;

		sscanf( strBuffer, "Race=%s X=%f Y=%f Z=%f Rotate=%f", strRaceName, &vPos.x, &vPos.y, &vPos.z, &rRotate );

		AuRaceType eRaceType = _ParseRaceName( strRaceName );
		AddCreatePosition( eRaceType, vPos, rRotate );
		if( !pData->sgetf( strBuffer ) ) return;
	}
}

void AgcmUILoginSetting::_AddCameraLoginTM( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer1[ MAX_PATH ] = { 0, };
	CHAR strBuffer2[ MAX_PATH ] = { 0, };
	CHAR strBuffer3[ MAX_PATH ] = { 0, };

	if( !pData->sgetf( strBuffer1 ) ) return;
	if( !pData->sgetf( strBuffer2 ) ) return;
	if( !pData->sgetf( strBuffer3 ) ) return;

	RwV3d vPos = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer1, "Pos : X=%f, Y=%f, Z=%f", &vPos.x, &vPos.y, &vPos.z );

	RwV3d vLookAt = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer2, "LookAt : X=%f, Y=%f, Z=%f", &vLookAt.x, &vLookAt.y, &vLookAt.z );

	float fPerspective = 1.0f;
	sscanf( strBuffer3, "Perspective=%f", &fPerspective );

	stViewSet ViewSet;

	ViewSet.Initialize();
	ViewSet.MoveTo( vPos );
	ViewSet.LookAt( vLookAt );

	SetLoginTM( ViewSet.m_matTM, fPerspective );
}

void AgcmUILoginSetting::_AddCameraCharacterSelectTM( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer1[ MAX_PATH ] = { 0, };
	CHAR strBuffer2[ MAX_PATH ] = { 0, };
	CHAR strBuffer3[ MAX_PATH ] = { 0, };

	if( !pData->sgetf( strBuffer1 ) ) return;
	if( !pData->sgetf( strBuffer2 ) ) return;
	if( !pData->sgetf( strBuffer3 ) ) return;

	RwV3d vPos = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer1, "Pos : X=%f, Y=%f, Z=%f", &vPos.x, &vPos.y, &vPos.z );

	RwV3d vLookAt = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer2, "LookAt : X=%f, Y=%f, Z=%f", &vLookAt.x, &vLookAt.y, &vLookAt.z );

	float fPerspective = 1.0f;
	sscanf( strBuffer3, "Perspective=%f", &fPerspective );

	stViewSet ViewSet;

	ViewSet.Initialize();
	ViewSet.MoveTo( vPos );
	ViewSet.LookAt( vLookAt );

	SetSelectTM( ViewSet.m_matTM, fPerspective );
}

void AgcmUILoginSetting::_AddCameraCharacterCreateTM( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer1[ MAX_PATH ] = { 0, };
	CHAR strBuffer2[ MAX_PATH ] = { 0, };
	CHAR strBuffer3[ MAX_PATH ] = { 0, };
	CHAR strBuffer4[ MAX_PATH ] = { 0, };

	if( !pData->sgetf( strBuffer1 ) ) return;
	if( !pData->sgetf( strBuffer2 ) ) return;
	if( !pData->sgetf( strBuffer3 ) ) return;
	if( !pData->sgetf( strBuffer4 ) ) return;

	CHAR strRaceName[ 32 ] = { 0, };
	sscanf( strBuffer1, "Race=%s", strRaceName );
	AuRaceType eRace = _ParseRaceName( strRaceName );

	RwV3d vPos = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer2, "Pos : X=%f, Y=%f, Z=%f", &vPos.x, &vPos.y, &vPos.z );

	RwV3d vLookAt = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer3, "LookAt : X=%f, Y=%f, Z=%f", &vLookAt.x, &vLookAt.y, &vLookAt.z );

	float fPerspective = 1.0f;
	sscanf( strBuffer4, "Perspective=%f", &fPerspective );

	stViewSet ViewSet;

	ViewSet.Initialize();
	ViewSet.MoveTo( vPos );
	ViewSet.LookAt( vLookAt );

	AddCreateTM( eRace, ViewSet.m_matTM, fPerspective );
}

void AgcmUILoginSetting::_AddCameraCharacterZoomTM( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer1[ MAX_PATH ] = { 0, };
	CHAR strBuffer2[ MAX_PATH ] = { 0, };
	CHAR strBuffer3[ MAX_PATH ] = { 0, };
	CHAR strBuffer4[ MAX_PATH ] = { 0, };
	CHAR strBuffer5[ MAX_PATH ] = { 0, };

	if( !pData->sgetf( strBuffer1 ) ) return;
	if( !pData->sgetf( strBuffer2 ) ) return;
	if( !pData->sgetf( strBuffer3 ) ) return;
	if( !pData->sgetf( strBuffer4 ) ) return;
	if( !pData->sgetf( strBuffer5 ) ) return;

	CHAR strRaceName[ 32 ] = { 0, };
	sscanf( strBuffer1, "Race=%s", strRaceName );
	AuRaceType eRace = _ParseRaceName( strRaceName );

	CHAR strClassName[ 32 ] = { 0, };
	sscanf( strBuffer2, "Class=%s", strClassName );
	AuCharClassType eClass = _ParseClassName( strClassName );

	RwV3d vPos = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer3, "Pos : X=%f, Y=%f, Z=%f", &vPos.x, &vPos.y, &vPos.z );

	RwV3d vLookAt = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer4, "LookAt : X=%f, Y=%f, Z=%f", &vLookAt.x, &vLookAt.y, &vLookAt.z );

	float fPerspective = 1.0f;
	sscanf( strBuffer5, "Perspective=%f", &fPerspective );

	stViewSet ViewSet;

	ViewSet.Initialize();
	ViewSet.MoveTo( vPos );
	ViewSet.LookAt( vLookAt );

	AddZoomTM( eRace, eClass, ViewSet.m_matTM, fPerspective );
}
