// AlefTextureList.h: interface for the AcTextureList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALEFTEXTURELIST_H__3B84C638_E220_4F07_8A25_5C6BFBC4657E__INCLUDED_)
#define AFX_ALEFTEXTURELIST_H__3B84C638_E220_4F07_8A25_5C6BFBC4657E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rwcore.h"
#include "AuList.h"

#define	ALEF_TILE_LIST_FILE_NAME	"map/tile/tilelist.txt"
#define	ALEF_TILE_EXPORT_FILE_NAME	"a%06xt%s"
// EXT가 없는 녀석으로..
#define	ALEF_TILE_TEXTURENAME_NAME	"a%06xt"

// 텍스쳐 없음을 나타내는 인식자.
#define	ALEF_TEXTURE_NO_TEXTURE	( 0 )

class AcTextureList  
{
public:
	BOOL				m_bEdited					;
	void				SetEdited	()				{ m_bEdited = TRUE	; }
	void				ClearEdited	()				{ m_bEdited = FALSE	; }
	BOOL				IsEdited	()				{ return m_bEdited	; }

	char *				m_strTextureDirectory		;

	char				m_strTextureDictionary[256]	;
	RwTexDictionary *	m_pTexDict					;	// NULL이면 아직 안읽은것

	struct	TextureInfo
	{
		RwImage		*	pImage				;	// 새로 쓰는 녀석,따로 메모리 관리 안함..
		int				nIndex				;	// 텍스쳐 인덱스.
		int				nProperty			;	// 타일 속성 저장..
		char			strComment	[ 256 ]	;	// 코멘트.
		char			strFilename	[ 256 ]	;	// 파일이름 저장해둠.

		TextureInfo() : pImage( NULL ) , nIndex( -1 ) , nProperty( APMMAP_MATERIAL_SOIL ) {}
	};

	AuList< TextureInfo >		TextureList;

	BOOL		IsEmpty				() { if( TextureList.GetCount() == 0 ) return TRUE;else return FALSE; }

	int			LoadScript			( char *filename	);	// 스크립트를 로딩한다.
	int			SaveScript			( char *filename	);
	BOOL		ExportTile			( char *destination , void ( *callback ) ( void * pData ) = NULL );
			// 파일 을 기준으로 텍스쳐를 로딩하고 
			// Material을 생성하여 가지고 있는다.
	
	BOOL		GetExportFileName	( char *filename , int nIndex	, char * ext = NULL	);

	RwTexture *		GetTexture		( int textureindex	);	// 택스쳐포인터를 얻어내는 함수.
	TextureInfo *	GetTextureInfo	( int textureindex	);	// 텍스쳐 검색.... 검색 온리.

	void		DeleteAll			( void				);	// 등록되어있는 텍스쳐 리스트 모두를 제거함.

	// Ref Count에 신경을 쓰자.
	RwTexture *	LoadTexture			( TextureInfo * textureinfo		, BOOL bForceUpdate = FALSE );
	RwRaster *	LoadRaster			( int			textureindex	);
	RwImage *	LoadImage			( int			textureindex	);


	// 맵툴 관련..
	TextureInfo * AddTexture		( int dimension , int category , int index , int type , char *filename , char *comment = NULL );
	BOOL		DeleteTexture		( int textureindex	, BOOL bDeleteGraphicFile = FALSE );	// 텍스쳐의 정보를 리스트에서 제거한다.
	AcTextureList();
	virtual ~AcTextureList();

	BOOL		MakeTempBitmap		( TextureInfo * pTi				);	// 임시 비트맵 생성함. 클라이언트 데이타도 만듬.

	int			CleanUpUnusedTile	();
protected:
	BOOL		InitialTempFiles	( void							);	// 타일 선택 윈도우에서 사용할 비트맵 파일 생성.
	void		Delete				( AuNode< TextureInfo > * pNode , BOOL bDeleteGraphicFile = FALSE );

};

#endif // !defined(AFX_ALEFTEXTURELIST_H__3B84C638_E220_4F07_8A25_5C6BFBC4657E__INCLUDED_)
