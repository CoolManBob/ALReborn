#include "CFlagImoticon.h"

#include "rwcore.h"
#include "rwplcore.h"

#include "AuMD5Encrypt.h"


CFlagImoticon::CFlagImoticon( void )
{
}

CFlagImoticon::~CFlagImoticon( void )
{
}

BOOL CFlagImoticon::LoadFlagFromFile( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	TiXmlDocument Doc;
	if( !_LoadXmlFile( &Doc, pFileName, TRUE ) ) return FALSE;

	TiXmlNode* pNodeRoot = Doc.FirstChild( "FlagImoticon" );
	if( !pNodeRoot ) return FALSE;

	TiXmlNode* pNodeFlagList = pNodeRoot->FirstChild( "FlagList" );
	if( !pNodeFlagList ) return FALSE;

	TiXmlNode* pNodeFlag = pNodeFlagList->FirstChild( "Flag" );
	while( pNodeFlag )
	{
		stFlagEntry NewEntry;

		const char* pName = pNodeFlag->ToElement()->Attribute( "Name" );
		if( pName && strlen( pName ) > 0 )
		{
			strcpy_s( NewEntry.m_strName, sizeof( char ) * STRING_LENGTH_NAME, pName );

			const char* pImgFileName = pNodeFlag->ToElement()->Attribute( "FileName" );
			if( pImgFileName && strlen( pImgFileName ) > 0 )
			{
				strcpy_s( NewEntry.m_strImageFileName, sizeof( char ) * STRING_LENGTH_NAME, pImgFileName );

				const char* pEventCode = pNodeFlag->ToElement()->Attribute( "EventCode" );
				if( pEventCode && strlen( pEventCode ) > 0 )
				{
					NewEntry.m_nEventCode = atoi( pEventCode );
				}

				void* pTexture = RwTextureRead( pImgFileName, NULL );
				if( pTexture )
				{
					NewEntry.m_pTexture = pTexture;
					m_mapFlag.Add( NewEntry.m_strName, NewEntry );
				}
			}
		}

		pNodeFlag = pNodeFlag->NextSibling();
	}

	TiXmlNode* pNodePositionList = pNodeRoot->FirstChild( "PositionList" );
	if( !pNodePositionList ) return FALSE;

	TiXmlNode* pNodePosition = pNodePositionList->FirstChild( "Position" );
	while( pNodePosition )
	{
		stOffsetEntry NewEntry;

		const char* pName = pNodePosition->ToElement()->Attribute( "Name" );
		if( pName && strlen( pName ) > 0 )
		{
			strcpy_s( NewEntry.m_strName, sizeof( char ) * STRING_LENGTH_NAME, pName );

			const char* pOffsetX = pNodePosition->ToElement()->Attribute( "OffsetX" );
			if( pOffsetX && strlen( pOffsetX ) > 0 )
			{
				NewEntry.m_fDefaultOffsetX = ( float )atof( pOffsetX );
			}

			const char* pOffsetY = pNodePosition->ToElement()->Attribute( "OffsetY" );
			if( pOffsetY && strlen( pOffsetY ) > 0 )
			{
				NewEntry.m_fDefaultOffsetY = ( float )atof( pOffsetY );
			}

			m_mapOffset.Add( NewEntry.m_strName, NewEntry );
		}

		pNodePosition = pNodePosition->NextSibling();
	}

	return TRUE;
}

BOOL CFlagImoticon::ReloadFlag( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	if( !ClearAllFlag() ) return FALSE;
	if( !LoadFlagFromFile( pFileName ) ) return FALSE;

	return TRUE;
}

BOOL CFlagImoticon::ClearAllFlag( void )
{
	int nFlagCount = m_mapFlag.GetSize();
	for( int nCount = 0 ; nCount < nFlagCount ; nCount++ )
	{
		stFlagEntry* pEntry = m_mapFlag.GetByIndex( nCount );
		if( pEntry && pEntry->m_pTexture )
		{
			RwTextureDestroy( ( RwTexture* )pEntry->m_pTexture );
			pEntry->m_pTexture = NULL;
		}
	}

	m_mapFlag.Clear();
	m_mapOffset.Clear();
	return TRUE;
}
 
BOOL CFlagImoticon::_LoadXmlFile( TiXmlDocument* pDoc, char* pFileName, BOOL bIsEncrypt )
{
	if( !pDoc || !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	BOOL bIsResult = FALSE;
	if( bIsEncrypt )
	{
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
#ifdef _AREA_CHINA_
			if( Cryptor.DecryptString( MD5_HASH_KEY_STRING, pBuffer, dwReadByte ) )
#else
			if( Cryptor.DecryptString( "1111", pBuffer, dwReadByte ) )
#endif
			{
				pDoc->Parse( pBuffer );
				if( !pDoc->Error() )
				{
					bIsResult = TRUE;
				}
			}
		}

		delete[] pBuffer;
		pBuffer = NULL;

		::CloseHandle( hFile );
		hFile = NULL;
	}
	else
	{
		if( pDoc->LoadFile( pFileName ) )
		{
			bIsResult = TRUE;
		}
	}

	return bIsResult;
}

void* CFlagImoticon::GetFlagTexture( char* pFlagName )
{
	if( !pFlagName || strlen( pFlagName ) <= 0 ) return NULL;

	stFlagEntry* pEntry = m_mapFlag.Get( pFlagName );
	if( !pEntry ) return NULL;

	return pEntry->m_pTexture;
}

void* CFlagImoticon::GetFlagTexture( int nEventCode )
{
	int nFlagCount = m_mapFlag.GetSize();
	for( int nCount = 0 ; nCount < nFlagCount ; nCount++ )
	{
		stFlagEntry* pEntry = m_mapFlag.GetByIndex( nCount );
		if( pEntry && pEntry->m_nEventCode == nEventCode  )
		{
			return pEntry->m_pTexture;
		}
	}

	return NULL;
}

float CFlagImoticon::GetFlagOffsetX( char* pStateName )
{
	if( !pStateName || strlen( pStateName ) <= 0 ) return 0.0f;

	stOffsetEntry* pEntry = m_mapOffset.Get( pStateName );
	if( !pEntry ) return 0.0f;

	return pEntry->m_fDefaultOffsetX;
}

float CFlagImoticon::GetFlagOffsetY( char* pStateName )
{
	if( !pStateName || strlen( pStateName ) <= 0 ) return 0.0f;

	stOffsetEntry* pEntry = m_mapOffset.Get( pStateName );
	if( !pEntry ) return 0.0f;

	return pEntry->m_fDefaultOffsetY;
}

float CFlagImoticon::GetFlagOffsetX( BOOL bHaveGuild, BOOL bHaveCharisma )
{
	if( !bHaveGuild && !bHaveCharisma ) return GetFlagOffsetX( "Normal" );
	if( bHaveGuild && !bHaveCharisma ) return GetFlagOffsetX( "HaveGuild" );
	if( !bHaveGuild && bHaveCharisma ) return GetFlagOffsetX( "HaveCharisma" );
	if( bHaveGuild && bHaveCharisma ) return GetFlagOffsetX( "HaveGuildAndCharisma" );
	return 0.0f;
}

float CFlagImoticon::GetFlagOffsetY( BOOL bHaveGuild, BOOL bHaveCharisma )
{
	if( !bHaveGuild && !bHaveCharisma ) return GetFlagOffsetY( "Normal" );
	if( bHaveGuild && !bHaveCharisma ) return GetFlagOffsetY( "HaveGuild" );
	if( !bHaveGuild && bHaveCharisma ) return GetFlagOffsetY( "HaveCharisma" );
	if( bHaveGuild && bHaveCharisma ) return GetFlagOffsetY( "HaveGuildAndCharisma" );
	return 0.0f;
}


