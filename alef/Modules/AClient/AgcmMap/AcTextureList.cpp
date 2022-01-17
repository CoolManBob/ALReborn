// AlefTextureList.cpp: implementation of the AcTextureList class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "ApmMap.h"
#include "AgcmMap.h"
#include "AcTextureList.h"
#include "rwcore.h"
#include "rpworld.h"
#include "rtpitexd.h"

#include "ApUtil.h"
#include "bmp.h"

#include "MagDebug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcTextureList::AcTextureList()
{
	m_strTextureDirectory = NULL;

	m_strTextureDictionary[0] = 0;
	m_pTexDict = NULL;

	m_bEdited		= FALSE;
}

AcTextureList::~AcTextureList()
{

}

int		AcTextureList::LoadScript			( char *filename	)	// 스크립트를 로딩한다.
	// 파일 을 기준으로 텍스쳐를 로딩하고 
	// Material을 생성하여 가지고 있는다.
{
	// File Format
	// Category|Index|Type|Comment|FileName

	// 0x00XXYY00 XX타입 , YY인덱스
	// 로 메모리에 넣는다.
	// 없는 녀석이라면 

	// 텍스쳐는 바로 로딩하지 않고 , 있는지만 가지고 있다.
	// Material Index는 가지고 있어야한다.

	FILE	*pFile = fopen( filename , "rt" );
	if( pFile == NULL )
	{
		// 파일오느 실패.
		return FALSE;
	}

	CGetArg2		arg;
	char			strbuffer[	1024	];
	unsigned int	nRead = 0;
	char			drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

	TextureInfo		ti;

	int count = 0;
	int	dimension , category , index , type;
	while( fgets( strbuffer , 1024 , pFile ) )
	{
		// 읽었으면..

		arg.SetParam( strbuffer , "|\n" );

		if( arg.GetArgCount() < 5 )
		{
			// 갯수 이상
			continue;
		}

		category	= atoi( arg.GetParam( 0 ) );
		index		= atoi( arg.GetParam( 1 ) );
		type		= atoi( arg.GetParam( 2 ) );

		strncpy		( ti.strComment		, arg.GetParam( 3 ) , 256 );

		// 파일이름 조절..
		strncpy		( ti.strFilename	, arg.GetParam( 4 ) , 256 );
		_splitpath	( ti.strFilename	, drive, dir, fname, ext );
		wsprintf	( ti.strFilename	, "Map\\Tile\\%s%s" , fname , ext );

		if( arg.GetArgCount() >= 6 )
			dimension	= atoi( arg.GetParam( 5 ) )	;
		else
		{
			switch( type )
			{
			case	TT_FLOORTILE	:	dimension	=	TT_FLOOR_DEPTH	;	break;
			case	TT_UPPERTILE	:	dimension	=	TT_UPPER_DEPTH	;	break;
			case	TT_ONE			:	dimension	=	TT_ONE_DEPTH	;	break;
			case	TT_ALPHATILE	:	dimension	=	TT_ALPHA_DEPTH	;	break;
			default:
				ASSERT( !"-__-!" );
				dimension	= TEXTURE_DIMENTION_DEFAULT	;					
				break;
			}
		}

		// 마고자 (2004-08-24 오후 3:50:12) : 타일 바닥 속성 추가..
		if( arg.GetArgCount() >= 7 )
			ti.nProperty	= atoi( arg.GetParam( 6 ) )	;
		else
		{
			ti.nProperty	= APMMAP_MATERIAL_SOIL;
		}

		ti.nIndex	= MAKE_TEXTURE_INDEX( dimension , category , index , type , 0x00 );

		TextureList.AddTail( ti );
		++count;
	}

	fclose( pFile );

	// InitialTempFiles();
	// 텍스쳐 등록할때 하게 해야함..

	//DisplayMessage( "텍스쳐 리스트 파일을 로디잉 하였습니다." );

	ClearEdited();
	return count;
}

RwTexture *	AcTextureList::GetTexture			( int textureindex	)	// 택스쳐포인터를 얻어내는 함수.
{
	TextureInfo 		* pTi;

	pTi = GetTextureInfo( textureindex );
	
	if( pTi )
	{
		// Ref Count 에 요주의!..
		return LoadTexture( pTi );
	}

	// 없으면 없는대로 -_-;;
	return NULL;
}

void		AcTextureList::DeleteAll			( void				)
{
	// 모든 노드를 삭제.

	AuNode< TextureInfo > * pNode;
	pNode = TextureList.GetHeadNode();

	while( pNode )
	{
		Delete( pNode );
		pNode = TextureList.GetHeadNode();
	}
}

void		AcTextureList::Delete			( AuNode< TextureInfo > * pNode , BOOL bDeleteGraphicFile)
{
	// 노드 하나를 삭제.

	TextureInfo *	pTi = & pNode->GetData();

	if( bDeleteGraphicFile )
	{
		// 파일 삭제...
		char	fname[ 256 ];
		wsprintf( fname , "%s\\%s" , m_strTextureDirectory , pTi->strFilename );
		DeleteFile( fname );
	}
	
	TextureList.RemoveNode( pNode );

	SetEdited();
}

RwTexture *		AcTextureList::LoadTexture			( TextureInfo * textureinfo , BOOL bForceUpdate )
{
	// 텍스쳐 정보를 보고 텍스쳐를 로딩한다.
    RwTexture *				texture		= NULL;
	char					drive[ 256 ] , dir [ 256 ] , f_name [ 256 ] , ext[ 256 ];

	char	strExportName[ 256 ];
	_splitpath( textureinfo->strFilename	, drive,	dir,	f_name,	ext		);

	//wsprintf( strExportName , ALEF_TILE_TEXTURENAME_NAME , textureinfo->nIndex );
	GetExportFileName( strExportName , textureinfo->nIndex );

	char	*pAlphaName = NULL;
	switch( GET_TEXTURE_TYPE( textureinfo->nIndex ) )
	{
	case TT_ALPHATILE:
		{
			pAlphaName = NULL;
			// 마고자 (2004-04-22 오후 6:59:17) : 밉맵끄기.
			RwTextureSetMipmapping		( FALSE );
			RwTextureSetAutoMipmapping	( FALSE );
		}
		break;
	case TT_UPPERTILE:
		{
			int		textlen = ( int ) strlen( ext );
			for( int i = 0 ; i < textlen ; ++i )
				ext[ i ] = toupper( ext[ i ] );

			if( !strncmp( ext , ".PNG" , 4 ) )
				pAlphaName = NULL;
			else
				pAlphaName = strExportName;
		}
		break;
	default:
		pAlphaName = NULL;
		break;
	}

	AGCMMAP_THIS->m_pcsAgcmResourceLoader->EnableEncryption( FALSE );

	// 마고자 (2004-05-30 오후 7:09:25) : 딕셔너리 설정..
	AGCMMAP_THIS->m_pcsAgcmResourceLoader->SetDefaultTexDict();
	if( bForceUpdate )
	{
		// do nothing..
		texture	= NULL;
	}
	else
	{
		// 마고자 (2005-04-22 오후 2:33:06) : 
		// 익스포트된 타일은 절대 읽지 않는다.
		texture = RwTextureRead( strExportName , pAlphaName );
		//texture = NULL;
	}

	if( texture )
	{
		// 성공
		RwTextureSetFilterMode( texture, rwFILTERLINEARMIPLINEAR );
	}
	else
	{
		// 실패.
		// 오리지널 읽음.

		ASSERT( NULL != m_strTextureDirectory );
		char	fname1[ 256 ];
		wsprintf( fname1 , "%s\\%s" , m_strTextureDirectory , textureinfo->strFilename );

		switch( GET_TEXTURE_TYPE( textureinfo->nIndex ) )
		{
		case TT_ALPHATILE:
			{
				pAlphaName = NULL;
				// 마고자 (2004-04-22 오후 6:59:17) : 밉맵끄기.
				RwTextureSetMipmapping		( FALSE );
				RwTextureSetAutoMipmapping	( FALSE );
			}
			break;
		case TT_UPPERTILE:
			{
				int textlen = ( int ) strlen( ext );
				for( int i = 0 ; i < textlen ; ++i )
					ext[ i ] = toupper( ext[ i ] );

				if( !strncmp( ext , ".PNG" , 4 ) )
					pAlphaName = NULL;
				else
					pAlphaName = fname1;
			}
		default:
			pAlphaName = NULL;
			break;
		}

		texture = RwTextureRead( RWSTRING ( fname1 ) , RWSTRING ( pAlphaName ) );
	}
	
	switch( GET_TEXTURE_TYPE( textureinfo->nIndex ) )
	{
	case TT_ALPHATILE:
		{
			RwTextureSetMipmapping		( TRUE );
			RwTextureSetAutoMipmapping	( TRUE );
		}
		break;
	default:
		// do nothing
		break;
	}

	if( texture )
	{
		// 성공
		switch( GET_TEXTURE_TYPE( textureinfo->nIndex ) )
		{
		case TT_ALPHATILE:
			{
				RwTextureSetFilterMode	( texture, rwFILTERNEAREST 	);
			}
			break;
		default:
			RwTextureSetFilterMode	( texture, rwFILTERLINEARMIPLINEAR	);
			break;
		}

		if( bForceUpdate )
		{
			RwTexture * pExist;
			if( pAlphaName )
			{
				pExist = RwTextureRead( strExportName , strExportName );
			}
			else
			{
				pExist = RwTextureRead( strExportName , NULL );
			}

			if( pExist )
			{
				// 이미 존재한다면... 레스터 바꿔치기 들어간다..
				RwRaster * pRaster = pExist->raster	;
				pExist->raster	= texture->raster;
				texture->raster	= pRaster;

				// 로드한 텍스쳐 디스트로이.
				RwTextureDestroy( texture );
				texture = NULL;
				texture = pExist;

				// 레퍼런스 카운트 떨굼.
				pExist->refCount--;
			}
		}
		else
		{
			RwTextureSetName		( texture , strExportName			);

			if( pAlphaName )
				RwTextureSetMaskName( texture , strExportName			);
		}
	}
	else
	{
		TRACE( "텍스쳐 읽기 실패 (%s)" , textureinfo->strFilename );
	}

	AGCMMAP_THIS->m_pcsAgcmResourceLoader->EnableEncryption( TRUE );

	return texture;
}

AcTextureList::TextureInfo * AcTextureList::GetTextureInfo	( int textureindex	)	// 텍스쳐 검색.... 검색 온리.
{
	// 무조건 같은것이 있는지만을 검사함. 
	// textureindex = GET_TEXTURE_OFFSET_MASKING_OUT( textureindex );
	int	category , index;

	category	= GET_TEXTURE_CATEGORY	( textureindex );
	index		= GET_TEXTURE_INDEX		( textureindex );

	AcTextureList::TextureInfo 				*pTi;
	AuNode< TextureInfo >	*pNode = TextureList.GetHeadNode();
	while( pNode )
	{
		pTi = & pNode->GetData();

		if( category	== GET_TEXTURE_CATEGORY	( pTi->nIndex )	&&
			index		== GET_TEXTURE_INDEX	( pTi->nIndex )	)
		{
			return pTi;
		}
		pNode = pNode->GetNextNode();
	}

	return NULL;
}

BOOL		AcTextureList::DeleteTexture		( int textureindex , BOOL bDeleteGraphicFile	)	// 텍스쳐의 정보를 리스트에서 제거한다.
{
	// 파일까지 삭제..
	TextureInfo * textureInfo = GetTextureInfo( textureindex );

	AuNode< TextureInfo > * pNode;
	pNode = TextureList.GetHeadNode();

	while( pNode )
	{
		if( &pNode->GetData() == textureInfo )
		{
			Delete( pNode , bDeleteGraphicFile );
			SetEdited();
			return TRUE;
		}
		TextureList.GetNext( pNode );
	}	

	// 못찾음!
	return FALSE;
}

AcTextureList::TextureInfo * AcTextureList::AddTexture		( int dimension , int category , int index , int type , char *filename , char *comment )
{
	TextureInfo		ti;

	if( dimension == TEXTURE_DIMENTION_DEFAULT )
	{
		switch( type )
		{
		case	TT_FLOORTILE	:	dimension	=	TT_FLOOR_DEPTH	;	break;
		case	TT_UPPERTILE	:	dimension	=	TT_UPPER_DEPTH	;	break;
		case	TT_ONE			:	dimension	=	TT_ONE_DEPTH	;	break;
		case	TT_ALPHATILE	:	dimension	=	TT_ALPHA_DEPTH	;	break;
		default:
			ASSERT( !"-__-!" );
			dimension	= TEXTURE_DIMENTION_DEFAULT	;					
			break;
		}
	}

	ti.nIndex	= MAKE_TEXTURE_INDEX( dimension , category , index , type , 0x00 );
	strncpy( ti.strFilename, filename , 256 );
	if( comment == NULL )
		strncpy( ti.strComment , filename , 256 );	// 파일이름으로 채워 버린다.
	else
		strncpy( ti.strComment , comment , 256 );

	// 템포러리 비트맵 생성.
	// 파일이 있는지도 검사함..
	if( MakeTempBitmap( &ti ) )
	{
		TextureList.AddTail( ti );
		TextureInfo * pTi = &TextureList.GetTailNode()->GetData();
		SetEdited();
		return pTi;
	}
	else
	{
		return NULL;
	}
}

int			AcTextureList::SaveScript			( char *filename	)
{
	// File Format
	// Type|Index|Comment|FileName

	// 0x00XXYY00 XX타입 , YY인덱스
	// 로 메모리에 넣는다.
	// 없는 녀석이라면 

	// 텍스쳐는 바로 로딩하지 않고 , 있는지만 가지고 있다.
	// Material Index는 가지고 있어야한다.

	FILE	*pFile = fopen( filename , "wt" );
	if( pFile == NULL )
	{
		// 파일오느 실패.
		return FALSE;
	}

	CGetArg2		arg;
	char			strbuffer[	1024	];

	TextureInfo		*pTi , *pTiLowest;

	int count = 0;
	int dimension , category , index , type ;

	AuNode< TextureInfo > * pNode		;
	AuNode< TextureInfo > * pNodeTmp	;
	
	////////////////////////////////////////////////////////////////////
	// 소트한다..
	////////////////////////////////////////////////////////////////////
	AuList< TextureInfo > listTmp	= TextureList;
	AuList< TextureInfo > listSorted;

	while( listTmp.GetHeadNode() )
	{
		pNode		= listTmp.GetHeadNode()	;
		pNodeTmp	= pNode					;
		pTiLowest	= &pNode->GetData()		;

		while( pNode )
		{
			// 가장 작은 것을 찾아서 인서트/리무부
			pTi		= &pNode->GetData();

			if( ( pTi->nIndex & 0x00ffffff ) < ( pTiLowest->nIndex & 0x00ffffff ) )
			{
				// 오오 작은거 발견.;;
				pNodeTmp	= pNode	;
				pTiLowest	= pTi	;
			}

			pNode = pNode->GetNextNode();
		}

		listSorted.AddTail( *pTiLowest	);
		listTmp.RemoveNode( pNodeTmp	);
	}
	////////////////////////////////////////////////////////////////////
	
	pNode = listSorted.GetHeadNode();
	while( pNode )
	{
		pTi = &pNode->GetData();

		dimension	= GET_TEXTURE_DIMENSION	( pTi->nIndex	);
		category	= GET_TEXTURE_CATEGORY	( pTi->nIndex	);
		index		= GET_TEXTURE_INDEX		( pTi->nIndex	);
		type		= GET_TEXTURE_TYPE		( pTi->nIndex	);

		// Type|Index|Comment|FileName

		wsprintf( strbuffer , "%d|%d|%d|%s|%s|%d|%d\n" ,
			category , index , type ,  pTi->strComment , pTi->strFilename , dimension , pTi->nProperty );

		fputs( strbuffer , pFile );

		count ++;
		listSorted.GetNext( pNode );
	}

	fclose( pFile );

	//DisplayMessage( "텍스쳐 리스트 파일을 저장하였습니다." );
	ClearEdited();
	return count;
}

BOOL		AcTextureList::MakeTempBitmap		( TextureInfo * pTi				)	// 임시 비트맵 생성함.
{
	RwImage	* pImage;

	char	str[1024];
	ASSERT( NULL != m_strTextureDirectory );
	wsprintf( str , "%s\\%s" , m_strTextureDirectory , pTi->strFilename );
	pImage = RwImageRead ( str );

	if( pImage == NULL )
	{
		// 디렉토리 변경에 대한.. 호완코드
		// 마고자 (2002-08-30 오후 6:43:36) : 
		wsprintf( str , "%s\\Map\\%s" , m_strTextureDirectory , pTi->strFilename );
		pImage = RwImageRead ( str );
	}

	// 어쩔수 없다.. 에러 이미지 출력
	if( NULL == pImage )
	{
		TRACE( "파일이 없어서 임의로 생성(%s)\n" , pTi->strFilename );
		CBmp	bmp;
		HBRUSH	hBrush;

		bmp.Create( 50 , 50 );
		VERIFY( hBrush = CreateSolidBrush( RGB( 255 , 0 , 0 ) ) );

		RECT	rect;
		SetRect( &rect , 0 , 0 , 50 , 50 );
		::FillRect( bmp.GetDC() , &rect , hBrush );
		::SetTextColor( bmp.GetDC() , RGB( 0 , 0 , 0 ) );
		::SetBkMode( bmp.GetDC() , TRANSPARENT );
		::TextOut( bmp.GetDC() , 0 , 0 , "ERROR" , 5 );

		char 	strFilename[ 1024 ];
		wsprintf( strFilename , "%s\\Map\\Temp\\%d,%d.bmp" , m_strTextureDirectory , GET_TEXTURE_CATEGORY( pTi->nIndex ) , GET_TEXTURE_INDEX( pTi->nIndex ) );

		VERIFY( bmp.Save( strFilename ) );

		DeleteObject( hBrush );
		return TRUE;
	}

	if( pImage == NULL )
	{
		// -_-;;;
		// 나보고 어쩌라고 - -;;
//		CString	str;
//		str.Format( "텍스쳐 리스트에 등록된 이미지( \"%s\" )가 존재하지 않습니다." , pTi->strComment );
//		DisplayMessage( AEM_ERROR , str );
//		ASSERT( !"Bitmap 생성 실패!\n" );
		return FALSE;
	}
	else
	{
		switch( GET_TEXTURE_TYPE( pTi->nIndex ) )
		{
		case	TT_FLOORTILE	:
			{
				// 생각없이 한개만 생성.
				// 로딩성공.
				char strFilenameOrigin[ 1024 ];
				wsprintf( strFilenameOrigin , "%s\\Map\\Temp\\Origin_%x.bmp" , m_strTextureDirectory , pTi->nIndex );

				RwImage	* pReSample;
				pReSample = RwImageCreateResample( pImage , 100 , 100 );

				ASSERT( NULL != pReSample );

				VERIFY( RwImageWrite( pReSample , strFilenameOrigin ) );

				RwImageDestroy( pImage		);
				RwImageDestroy( pReSample	);
				
				
				// 대표 타일 한개만 생성.
				// 로딩성공.

				CBmp	bmp,bmpPicked;
				bmp.Load( strFilenameOrigin , NULL );

				bmpPicked.Create( 50 , 50 );
				bmp.Draw( bmpPicked.GetDC() , 0 , 0 , 50 , 50 );

				char 	strFilename[ 1024 ];
				wsprintf( strFilename , "%s\\Map\\Temp\\%d,%d.bmp" , m_strTextureDirectory , GET_TEXTURE_CATEGORY( pTi->nIndex ) , GET_TEXTURE_INDEX( pTi->nIndex ) );

				VERIFY( bmpPicked.Save( strFilename ) );

				DeleteFile		( strFilenameOrigin );

				return TRUE;
			}
			break;
		case	TT_ALPHATILE	:
			{
				// 생각없이 한개만 생성.
				// 로딩성공.
				char strFilenameOrigin[ 1024 ];
				wsprintf( strFilenameOrigin , "%s\\Map\\Temp\\Origin_%x.bmp" , m_strTextureDirectory , pTi->nIndex );

				RwImage	* pReSample;
				pReSample = RwImageCreateResample( pImage , 200 , 200 );

				ASSERT( NULL != pReSample );
				VERIFY( RwImageWrite( pReSample , strFilenameOrigin ) );

				RwImageDestroy( pImage		);
				RwImageDestroy( pReSample	);
				
				
				// 대표 타일 한개만 생성.
				// 로딩성공.

				CBmp	bmp,bmpPicked;
				bmp.Load( strFilenameOrigin , NULL );

				bmpPicked.Create( 50 , 50 );
				bmp.Draw( bmpPicked.GetDC() , 0 , 0 , 50 , 50 );

				char strFilename[ 1024 ];
				wsprintf( strFilename , "%s\\Map\\Temp\\%d,%d.bmp" , m_strTextureDirectory , GET_TEXTURE_CATEGORY( pTi->nIndex ) , GET_TEXTURE_INDEX( pTi->nIndex ) );

				VERIFY( bmpPicked.Save( (LPSTR)(LPCTSTR) strFilename ) );

				DeleteFile		( strFilenameOrigin );

				return TRUE;
			}
			break;
		case	TT_UPPERTILE	:
			{
//				// 생각없이 한개만 생성.
//				// 로딩성공.
//				char strFilenameOrigin[ 1024 ];
//				wsprintf( strFilenameOrigin , "%s\\Map\\Temp\\Origin_%x.bmp" , m_strTextureDirectory , pTi->nIndex );
//
//				RwImage	* pReSample;
//
//				pReSample = RwImageCreateResample( pImage , 200 , 200 );
//
//				ASSERT( NULL != pReSample );
//
//				VERIFY( RwImageWrite( pReSample , strFilenameOrigin ) );
//
//				RwImageDestroy( pImage		);
//				RwImageDestroy( pReSample	);
//				
//				
//				// 대표 타일 한개만 생성.
//				// 로딩성공.
//
//				CBmp	bmp,bmpPicked;
//				bmp.Load( strFilenameOrigin );
//
//				bmpPicked.Create( 50 , 50 );
//
//				char strFilename[ 1024 ];
//
//				for( int y = 0 ; y < 4 ; ++y )
//				{
//					for( int x = 0 ; x < 4 ; ++x )
//					{
//						wsprintf( strFilename , "%s\\Map\\Temp\\%d,%d.bmp" , m_strTextureDirectory , GET_TEXTURE_CATEGORY( pTi->nIndex ) , GET_TEXTURE_INDEX( pTi->nIndex ) );
//						wsprintf( strFilename , "%s\\Map\\Temp\\%x.bmp" , m_strTextureDirectory , pTi->nIndex + y * 4 + x );
//
//						bmp.Draw( bmpPicked.GetDC() , 0 , 0 , 50 , 50 , x * 50 , y * 50 );
//							
//						VERIFY( bmpPicked.Save( strFilename ) );
//					}
//				}
//
//				DeleteFile		( strFilenameOrigin );
//
				return TRUE;
			}
			break;
		case	TT_ONE			:
			{
				// 생각없이 한개만 생성.
				// 로딩성공.
				char strFilename[ 1024 ];
				wsprintf( strFilename , "%s\\Map\\Temp\\%d,%d.bmp" , m_strTextureDirectory , GET_TEXTURE_CATEGORY( pTi->nIndex ) , GET_TEXTURE_INDEX( pTi->nIndex ) );

				RwImage	* pReSample;
				pReSample = RwImageCreateResample( pImage , 50 , 50 );

				ASSERT( NULL != pReSample );

				VERIFY( RwImageWrite( pReSample , (LPCTSTR) strFilename ) );

				RwImageDestroy( pImage		);
				RwImageDestroy( pReSample	);
				return TRUE;
			}
			break;
		default					:
			{
				// 한개 생성.

			}
			break;
		}

//		// 프리뷰 이미지 생성부분.
//
//		// 로딩성공.
//		CString	filename;
//		filename.Format( "%s\\Map\\Temp\\%d.bmp" , m_strTextureDirectory , pTi->nIndex );
//
//		RwImage	* pReSample;
//		pReSample = RwImageCreateResample( pImage , 50 , 50 );
//
//		if( RwImageWrite( pReSample , (LPCTSTR) filename ) )
//		{
//			// 아싸
//		}
//		else
//		{
//			CString	str;
//			str.Format( "텍스쳐 리스트에 등록된 이미지( \"%s\" ) 임시 파일 생성 실패" , pTi->strComment );
//			DisplayMessage( AEM_ERROR , str );
//		}
//
//		RwImageDestroy( pImage		);
//		RwImageDestroy( pReSample	);

		return FALSE;
	}
}

BOOL		AcTextureList::InitialTempFiles	( void				)	// 타일 선택 윈도우에서 사용할 비트맵 파일 생성.
{
	// 리스트에 있는 이미지 파일들을 모두 bmp파일로 생성함.
	// 
	AcTextureList::TextureInfo 			*	pTi		;
	int										count	= 0;
	AuNode< TextureInfo >	*pNode = TextureList.GetHeadNode();
	while( pNode )
	{
		pTi = & pNode->GetData();
		if( MakeTempBitmap( pTi ) )
		{
			// 성공
		}
		else
		{
			// 실패.
		}
		pNode = pNode->GetNextNode();
	}

	return TRUE;
}


BOOL	AcTextureList::ExportTile	( char *destination , void ( *callback ) ( void * pData ) )
{
	// 등록된 타일을 다 익스포트한다..
	ASSERT( NULL != m_strTextureDirectory );

	AuNode< TextureInfo >	* pNode = TextureList.GetHeadNode();
	TextureInfo	*	pTi			;

	//RwImage	*		pReSample	;

	char		str[1024]			;
	char		dstfullname[ 1024 ]	;
	char		dstShortFilename[ 1024 ];
	char		drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

//	CProgressDlg	dlg;
//	dlg.StartProgress( "월드 텍스쳐 익스포트중..." , TextureList.GetCount() , g_pMainFrame );
	int	count = 0;

	while( pNode )
	{
		pTi = & pNode->GetData();

		// Texture Dictionary 생성을 위해서 다 로딩한다.
		if( m_strTextureDictionary[0] )
			LoadTexture(pTi);

		// 밉맵데이타 생성...

		// 우선 이미지 로딩.
		wsprintf( str , "%s\\%s" , m_strTextureDirectory , pTi->strFilename );

		// 3단계로 밉맵 파일을 생성함.
		// full 1/2 1/4 로 생성.

		// 파일이름 생성
		// 데스티네이션 디렉토리에 파일 생성..

		_splitpath( pTi->strFilename , drive, dir, fname, ext );

		// Full Size..

		//wsprintf( dstShortFilename , ALEF_TILE_EXPORT_FILE_NAME , pTi->nIndex , ext );
		GetExportFileName( dstShortFilename , pTi->nIndex , ext );
		wsprintf( dstfullname , "%s\\Texture\\World\\%s" ,	m_strTextureDirectory , dstShortFilename);


		if( CopyFile( str , dstfullname , FALSE ) )
		{
			// 성공

		}
		else
		{
			TRACE( "실패!\n" );
		}


		TextureList.GetNext( pNode );

		++count;
//		dlg.SetProgress( count );
	}

	// Texture Dictionary가 있으면, StreamWrite한다. Platform Independent Data로
	if( m_pTexDict && m_strTextureDictionary[0] )
	{
		RwStream *		stream;

		stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMWRITE, m_strTextureDictionary );
		if( stream )
		{
			RtPITexDictionaryStreamWrite( m_pTexDict , stream );
   
			RwStreamClose(stream, NULL);
		}
	}

//	dlg.EndProgress();
	
	return TRUE;
}

BOOL		AcTextureList::GetExportFileName	( char *filename , int nIndex , char * ext )
{
	int	nIndexExport;

	nIndexExport	=	GET_TEXTURE_DIMENTION_MASKING_OUT( GET_TEXTURE_OFFSET_MASKING_OUT( nIndex ) );

	wsprintf( filename , ALEF_TILE_EXPORT_FILE_NAME , nIndexExport , ext );
	return TRUE;
}

RwRaster *	AcTextureList::LoadRaster			( int			textureindex	)
{
	TextureInfo 		* pTi;

	pTi = GetTextureInfo( textureindex );

	ASSERT( NULL != m_strTextureDirectory );
	char	fname1[ 256 ];
	wsprintf( fname1 , "%s\\%s" , m_strTextureDirectory , pTi->strFilename );

	return RwRasterRead( fname1 );
}

RwImage *	AcTextureList::LoadImage			( int			textureindex	)
{
	TextureInfo 		* pTi;

	pTi = GetTextureInfo( textureindex );

	if( pTi->pImage )
	{
		// 이미 로드돼어있음..
		return pTi->pImage;
	}
	else
	{
		ASSERT( NULL != m_strTextureDirectory );
		char	fname1[ 256 ];
		wsprintf( fname1 , "%s\\%s" , m_strTextureDirectory , pTi->strFilename );

		return pTi->pImage = RwImageRead( fname1 );
	}
}

#define TOLOWERSTRING( str )					\
{												\
	int tmp;									\
	tmp = strlen( str ) - 1;					\
	while( tmp > 0 )							\
	{											\
		str[ tmp ] = tolower( str[ tmp ] );		\
		tmp--;									\
	}											\
}

int		AcTextureList::CleanUpUnusedTile	()
{
	char	strPath[ 1024 ];
	wsprintf( strPath , "%s\\Map\\Tile\\*.*" ,	m_strTextureDirectory );

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	int count = 0;

	// path 의 파일을 찾아본다.
	hFind = FindFirstFile( strPath , & FindFileData );

	char	strFile[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( strPath , drive, dir, fname, ext );

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		OutputDebugString( "DeleteFiles : FindFile Handle Invalid\n" );
		return count;
	} 
	else 
	{
		char	drive2[ 256 ] , dir2 [ 256 ] , fname2 [ 256 ] , ext2[ 256 ];
		char	drive3[ 256 ] , dir3 [ 256 ] , fname3 [ 256 ] , ext3[ 256 ];

		AcTextureList::TextureInfo 	*pTi	;
		AuNode< TextureInfo >		*pNode	;
		_splitpath( FindFileData.cFileName	, drive2 , dir2 , fname2 , ext2 );
		wsprintf( strFile , "%s%s%s%s" , drive , dir , fname2 , ext2 );

		TOLOWERSTRING( fname2 );
		TOLOWERSTRING( ext2 );

		// 리스트에 있는지 검사..
		// 

		pNode = TextureList.GetHeadNode();
		while( pNode )
		{
			pTi = & pNode->GetData();

			_splitpath( pTi->strFilename	, drive3 , dir3 , fname3 , ext3 );
			
			TOLOWERSTRING( fname3 );
			TOLOWERSTRING( ext3 );

			if( !strcmp( fname2 , fname3 ) && !strcmp( ext2 , ext3 ) )
			{
				break;
			}
			pNode = pNode->GetNextNode();
		}

		if( NULL == pNode && strcmp( ext2 , ".txt" ) )
		{
			if( DeleteFile( strFile ) )
				count ++;
			else
			{
				// 딜리트 실패,.
				char strDebug[ 256 ];
				wsprintf( strDebug , "Cannot Delete '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
				OutputDebugString( strDebug );
			}
		}


		while( FindNextFile( hFind , & FindFileData ) )
		{
			_splitpath( FindFileData.cFileName	, drive2 , dir2 , fname2 , ext2 );
			wsprintf( strFile , "%s%s%s%s" , drive , dir , fname2 , ext2 );

			TOLOWERSTRING( fname2 );
			TOLOWERSTRING( ext2 );

			pNode = TextureList.GetHeadNode();
			while( pNode )
			{
				pTi = & pNode->GetData();

				_splitpath( pTi->strFilename	, drive3 , dir3 , fname3 , ext3 );
				
				TOLOWERSTRING( fname3 );
				TOLOWERSTRING( ext3 );

				if( !strcmp( fname2 , fname3 ) && !strcmp( ext2 , ext3 ) )
				{
					break;
				}
				pNode = pNode->GetNextNode();
			}

			if( NULL == pNode && strcmp( ext2 , ".txt" ) )
			{
				if( DeleteFile( strFile ) )
					count ++;
				else
				{
					// 딜리트 실패,.
					char strDebug[ 256 ];
					wsprintf( strDebug , "Cannot Delete '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
					OutputDebugString( strDebug );
				}
			}
		}

		// 처리 끝
	}
	

	return count;
}