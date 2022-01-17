#include "AcuTextureList.h"
#include "AgcmResourceLoader.h"
#include "rtpng.h"



AcuTextureList::AcuTextureList( void )
{
	m_lAddImageIndex = 1;
}

AcuTextureList::~AcuTextureList( void )
{
	DestroyTextureList();
}

INT32 AcuTextureList::AddImage( char * filename, BOOL bRead	)
{
	ASSERT( NULL != filename );

	stTextureListInfo TextureListInfo;

	strcpy( TextureListInfo.szName, filename );
	TextureListInfo.pTexture = NULL;
	TextureListInfo.lTextureListID = m_lAddImageIndex;

	if( bRead )
	{
		RwTexture* pTexture = LoadTexture( filename );

		ASSERT( NULL != pTexture && "파일로딩체크" );
		ASSERT( NULL != pTexture->raster && "파일로딩체크" );

		INT32 index = m_listTexture.GetCount();
		TextureListInfo.pTexture = pTexture					;
	}

	m_listTexture.AddTail( TextureListInfo );
	m_lAddImageIndex++;

	return ( m_lAddImageIndex - 1 );
}

RwTexture* AcuTextureList::GetImage_ID( INT32 lID )
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{
		stTextureListInfo& TextureListInfo = m_listTexture.GetNext( pListNode );

		if( TextureListInfo.lTextureListID == lID )
		{
			return TextureListInfo.pTexture;
		}
		else if( TextureListInfo.lTextureListID > lID )
		{
			return NULL;
		}
	}

	return NULL;
}

CHAR* AcuTextureList::GetImageName_ID( INT32 lID )
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{
		stTextureListInfo& TextureListInfo = m_listTexture.GetNext( pListNode );
		if( TextureListInfo.lTextureListID == lID )
		{
			return TextureListInfo.szName;
		}
		else if( TextureListInfo.lTextureListID > lID )
		{
			return NULL;
		}
	}

	return NULL;
}

RwTexture* AcuTextureList::GetImage_Index( INT32 lIndex, INT32 *plID )
{
	if( m_listTexture.GetCount() <= lIndex ) return NULL;
	if( 0 == m_listTexture.GetCount() ) return NULL;

	// plID 입력 
	if( plID )
	{
		*plID = m_listTexture[ lIndex ].lTextureListID;
	}

	return m_listTexture[ lIndex ].pTexture;		
}

BOOL AcuTextureList::SetImage_Index( INT32 lIndex, char *filename,	BOOL bRead )
{
	if( m_listTexture.GetCount() <= lIndex ) return FALSE;

	// bRead 가 TRUE 인 경우 이미지가 이미 로드되어 있어도 다시 로딩한다.
	if( bRead )
	{
		// 이미 로드되어 있으면 일단 해제하고..
		if( m_listTexture[ lIndex ].pTexture )
		{
			_SafeReleaseTexture( m_listTexture[ lIndex ].pTexture );
			m_listTexture[ lIndex ].pTexture = NULL;
		}

		// 이미지 로딩..
		m_listTexture[ lIndex ].pTexture = LoadTexture( filename );
	}

	// 이미지를 로드하든 안하든 이름은 저장해준다.
	strcpy( m_listTexture[ lIndex ].szName, filename );
	return TRUE;
}

BOOL AcuTextureList::SetImageName_Index( INT32 lIndex, CHAR *szName	)
{
	if( m_listTexture.GetCount() <= lIndex ) return FALSE;
	strcpy( m_listTexture[ lIndex ].szName, szName );
	return TRUE;
}

BOOL AcuTextureList::SetImage_ID( INT32 lID, char *filename, BOOL bRead )
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{
		stTextureListInfo& TextureListInfo = m_listTexture.GetNext( pListNode );
		if ( lID == TextureListInfo.lTextureListID ) 
		{
			// bRead 가 TRUE 면 이미지를 해제한다.
			if( TextureListInfo.pTexture )
			{
				_SafeReleaseTexture( TextureListInfo.pTexture );
				TextureListInfo.pTexture = NULL;
			}

			// 이미지 로딩
			TextureListInfo.pTexture = LoadTexture( filename );

			// 파일명 저장
			strcpy( TextureListInfo.szName, filename );
			return TRUE;

		}
		else if( lID < TextureListInfo.lTextureListID )
		{
			return FALSE;
		}
	}

	return FALSE;		
}

BOOL AcuTextureList::SetImageID_Name( INT32 lID, char *filename, BOOL bRead	)
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{
		stTextureListInfo &	TextureListInfo = m_listTexture.GetNext( pListNode );
		if( !strcmp( filename, TextureListInfo.szName ) )
		{
			TextureListInfo.lTextureListID = lID;

			// 이름이 같고 Texture 가 살아있으면 이미 로드된 것인데..
			// bRead 가 TRUE 면 해제하고 다시 로드, 아니면 걍 넘어간다.
			if( bRead )
			{
				// 이미지가 이미 로딩되어 있으면 해제
				if( TextureListInfo.pTexture )
				{
					_SafeReleaseTexture( TextureListInfo.pTexture );
					TextureListInfo.pTexture = NULL;
				}

				// 이미지 로딩
				TextureListInfo.pTexture = LoadTexture( filename );

				// 파일명 저장
				strcpy( TextureListInfo.szName, filename );
			}

			return TRUE;
		}
	}

	return FALSE;
}

INT32 AcuTextureList::AddOnlyThisImage( char* filename, BOOL bLoad )
{
	DestroyTextureList();
	ASSERT( NULL != filename );

	stTextureListInfo TextureListInfo;

	TextureListInfo.pTexture = NULL;
	TextureListInfo.lTextureListID = m_lAddImageIndex;

	if( bLoad )
	{
		RwTexture* pTexture = LoadTexture( filename );

		ASSERT( NULL != pTexture && "파일로딩체크" );
		ASSERT( NULL != pTexture->raster && "파일로딩체크" );

		TextureListInfo.pTexture = pTexture;
	}

	m_listTexture.AddTail( TextureListInfo );
	m_lAddImageIndex++;

	return ( m_lAddImageIndex - 1 );
}

void AcuTextureList::DestroyTextureList( void )
{
	AuNode< stTextureListInfo >* pNode = NULL	;
	stTextureListInfo TextureListInfo;

	while( pNode = m_listTexture.GetHeadNode() )
	{
		TextureListInfo = pNode->GetData();
		if( TextureListInfo.pTexture )
		{
			_SafeReleaseTexture( TextureListInfo.pTexture );
			TextureListInfo.pTexture = NULL;
		}

		m_listTexture.RemoveHead();
	}		
}

INT32 AcuTextureList::GetCount( void )
{
	return m_listTexture.GetCount();
}

stTextureListInfo* AcuTextureList::GetHeadTextureListInfo( void )
{
	return &m_listTexture.GetHead();
}

BOOL AcuTextureList::DeleteImage_ID( INT32 lID, BOOL bDestroyTexture )
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();
	stTextureListInfo TextureListInfo;

	while( pListNode )
	{
		TextureListInfo = m_listTexture.GetNext( pListNode );
		if( lID == TextureListInfo.lTextureListID ) 
		{
			// 이제 찾았으니 지우자!
			if ( NULL != TextureListInfo.pTexture && bDestroyTexture ) 
			{
				_SafeReleaseTexture( TextureListInfo.pTexture );
				TextureListInfo.pTexture = NULL;
			}

			m_listTexture.RemoveData( TextureListInfo );
			return TRUE;
		}
		else if( lID < TextureListInfo.lTextureListID )
		{
			return FALSE;
		}
	}

	return FALSE;		
}

BOOL AcuTextureList::DeleteImage_Index( INT32 lIndex )
{
	if ( m_listTexture.GetCount() <= lIndex ) return FALSE;

	stTextureListInfo TextureListInfo = m_listTexture[ lIndex ];

	if( TextureListInfo.pTexture )
	{
		_SafeReleaseTexture( TextureListInfo.pTexture );
		TextureListInfo.pTexture = NULL;
	}
	
	m_listTexture.RemoveData( m_listTexture[ lIndex ] );
	return TRUE;
}

AcuTextureList& AcuTextureList::operator=( const AcuTextureList& csTextureList ) 
{
	AuList< stTextureListInfo >	listTemp;

	listTemp = m_listTexture;
	m_listTexture = csTextureList.m_listTexture;
	m_lAddImageIndex = csTextureList.m_lAddImageIndex;
	
	// Add Reference Count
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	stTextureListInfo TextureListInfo;
	while( pListNode )
	{
		TextureListInfo = m_listTexture.GetNext( pListNode );
		if( TextureListInfo.pTexture )
		{
			// Add Reference Count 
			RwTextureAddRef( TextureListInfo.pTexture );
		}
	}

	pListNode = listTemp.GetHeadNode();
	while( pListNode )
	{
		TextureListInfo = listTemp.GetNext( pListNode );
		if( TextureListInfo.pTexture )
		{
			_SafeReleaseTexture( TextureListInfo.pTexture );
			TextureListInfo.pTexture = NULL;
		}
	}

	listTemp.RemoveAll();
	return *this;
}

CHAR* AcuTextureList::GetImageName_Index( INT32 lIndex, INT32 *plID	)
{
	if ( m_listTexture.GetCount() <= lIndex ) return NULL;
	if ( !m_listTexture.GetCount() ) return NULL;

	// plID 입력 
	if( NULL != plID )
	{
		*plID = m_listTexture[ lIndex ].lTextureListID;
	}

	return m_listTexture[ lIndex ].szName;		
}

void AcuTextureList::LoadTextures( void	)
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{
		stTextureListInfo& TextureListInfo = m_listTexture.GetNext( pListNode );
		if( TextureListInfo.pTexture == NULL )
		{
			if ( TextureListInfo.szName == NULL || TextureListInfo.szName[ 0 ] == NULL ) continue;

			TextureListInfo.pTexture = LoadTexture( TextureListInfo.szName );
			if( !TextureListInfo.pTexture )
			{
				// 읽기실패시 한번더 시도.. RwTextureRead 가 가끔 실패할수도 있댄다.
				TextureListInfo.pTexture = LoadTexture( TextureListInfo.szName );
			}
		}
	}
}

RwTexture* AcuTextureList::LoadTexture( char* pFileName )
{
	if( pFileName == NULL || strlen( pFileName ) == 0 ) return NULL;

	// Texture ReadCallBack 은 AgcmResourceLoader::CBTextureRead() 로 변경되어 있다.
	// 위의 콜백을 통해서 로딩을 시도해본다.
	RwTexture* pTexture = RwTextureRead( pFileName, NULL );
	if( !pTexture )
	{
		// 로딩이 실패했다면 Renderware 의 DefaultReadCallBack 을 통해 직접 로딩시도한다.
		RwTextureCallBackRead fnReadCallBack = AgcmResourceLoader::GetDefaultCBReadTexture();
		if( !fnReadCallBack )
		{
#ifdef _DEBUG
			CHAR strDebug[ 256 ] = { 0, };
			sprintf( strDebug, "Texture Load Fail : %s ( Cannot Find Default ReadCallBack )\n", pFileName );
			OutputDebugString( strDebug );
			return NULL;
#endif
		}

		// 파일명에 확장자가 존재하면 확장자를 제거한다.
		CHAR strFileName[ 256 ] = { 0, };
		CHAR strExtension[ 16 ] = { 0, };
		_ParseTextureFileName( pFileName, strFileName, strExtension );

		pTexture = fnReadCallBack( strFileName, NULL );
		if( !pTexture )
		{
#ifdef _DEBUG
			CHAR strDebug[ 256 ] = { 0, };
			sprintf( strDebug, "Texture Load Fail : %s ( Use Renderware DefaultCallBack )\n", pFileName );
			OutputDebugString( strDebug );
#endif
			return NULL;
		}
	}

	RwTextureSetFilterMode( pTexture, rwFILTERNEAREST );
	RwTextureSetAddressing( pTexture, rwTEXTUREADDRESSCLAMP );
	return pTexture;
}

void AcuTextureList::UnloadTextures( void )
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{		
		stTextureListInfo& TextureListInfo = m_listTexture.GetNext( pListNode );
		if( TextureListInfo.pTexture != NULL )
		{
			_SafeReleaseTexture( TextureListInfo.pTexture );
			TextureListInfo.pTexture = NULL;
		}
	}
}

INT32 AcuTextureList::AddTexture( RwTexture* pstTexture )
{
	stTextureListInfo TextureListInfo;

	TextureListInfo.szName[ 0 ] = 0;
	TextureListInfo.pTexture = pstTexture;
	TextureListInfo.lTextureListID = m_lAddImageIndex;

	m_listTexture.AddTail( TextureListInfo );
	m_lAddImageIndex++;

	return ( m_lAddImageIndex - 1 );
}

BOOL AcuTextureList::_SafeReleaseTexture( RwTexture* pTexture )
{
	try
	{
		RwTextureDestroy( pTexture );
		pTexture = NULL;
	}
	catch( ... )
	{
		return FALSE;
	}

	return TRUE;
}

void AcuTextureList::_ParseTextureFileName( char* pFullFileName, char* pFileName, char* pExtension )
{
	if( !pFullFileName ) return;
	if( !pFileName ) return;
	if( !pExtension ) return;	

	INT32 nIndex = 0;
	INT32 nExtCount = 0;
	BOOL bIsExtension = FALSE;

	while( 1 )
	{
		if( pFullFileName[ nIndex ] == '\0' ) return;
		if( pFullFileName[ nIndex ] == '.' )
		{
			bIsExtension = TRUE;
		}
		else
		{
			if( !bIsExtension )
			{
				pFileName[ nIndex ] = pFullFileName[ nIndex ];
			}
			else
			{
				pExtension[ nExtCount ] = pFullFileName[ nIndex ];
				++nExtCount;
			}
		}

		++nIndex;
	}
}