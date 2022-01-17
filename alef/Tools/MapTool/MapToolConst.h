#pragma once

// 맵툴에사 서용하는 콘스턴트 모음집. 
#include "ConstManager.h"
#include "ApDefine.h"
#include <ApBase.h>

#define	ALEF_LOADING_MODE					( g_Const.m_nGlobalMode					)
#define	ALEF_PREVIEW_MAP_SELECT_SIZE		( g_Const.m_nPreviewMapSelectSize		)
#define	ALEF_PREVIEW_MAP_STEP_SIZE			( 16									)

#define	ALEF_SECTOR_WIDTH					( g_Const.m_fSectorWidth				)	// 한섹터의 가로세로 길이.100.0f
#define	ALEF_MAX_HORI_SECTOR				( g_Const.m_nHoriSectorCount			)
#define	ALEF_MAX_VERT_SECTOR				( g_Const.m_nVertSectorCount			)		// 가로세로 섹터의 최대 좌표.
#define	ALEF_SECTOR_DEFAULT_DEPTH			( g_Const.m_nSectorDepth				)		// 

#define	ALEF_CURRENT_DIRECTORY				( g_Const.m_strCurrentDirectory			)

// 로딩 범위. Array Coordinate! -_-+
#define	ALEF_LOAD_RANGE_X1					( g_Const.m_nLoading_range_x1			)
#define	ALEF_LOAD_RANGE_Y1					( g_Const.m_nLoading_range_y1			)
#define	ALEF_LOAD_RANGE_X2					( g_Const.m_nLoading_range_x2			)
#define	ALEF_LOAD_RANGE_Y2					( g_Const.m_nLoading_range_y2			)

#define	ALEF_USE_EFFECT						( g_Const.m_bUseEffect					)

// Lock Mode
#define	ALEF_GEOMETRY_LOCK_MODE				( ( RwInt32 ) g_Const.m_nGeometryLockMode		)
#define	ALEF_VERTEXCOLOR_LOCK_MODE			( ( RwInt32 ) g_Const.m_nVertexColorLockMode	)

#define	ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT	1.0f	// 세그먼트가 만들어질때 디폴트 높이
#define	ALEF_SECTOR_DEFAULT_TILE_INDEX		( 0x00010380 )		// 디폴트로 들어가게 돼는 타일 번호
#define	ALEF_SECTOR_DEFAULT_ROUGHTILE_INDEX	( 0x00010200 )		// 디폴트로 들어가게 돼는 타일 번호
#define	ALEF_DEFAULT_ALPHA					( 0x000001c0 )		// 디폴트 텍스쳐의 알파 인덱스.
// 디테일에 1/5 로 처리한다..
#define	ALEF_SECTOR_ROUGH_DEPTH				( 4 )

#define	ALEF_SECTOR_DUMMY_DEPTH				1

// 타일관계
#define	ALEF_TILE_DO_NOT_USE				( 0x00000000 )

// 파일 포멧 기록
#define	ALEF_GetFileType( x )				( x >> 16 )
#define	ALEF_GetFileVersion( x )			( x & 0x0000ffff )

#define	ALEF_MAPFILE						0x0001

#define	ALEF_MATERIAL_DEFAULT_COUNT			(5)


#define	KEYFORWARD		'w'
#define	KEYBACKWARD		's'
#define	KEYSTEPLEFT		'a'
#define	KEYSTEPRIGHT	'd'
#define	KEYLIFTUP		VK_SPACE
#define	KEYLIFTDOWN		'z'
#define	KEYROTATECCW	'e'
#define KEYROTATECW		'q'

// 프로그램 작업 모드들 정리함.
enum	TILEWINDOWTYPE
{
	EDITMODE_GEOMETRY	,
	EDITMODE_TILE		,
	EDITMODE_OBJECT		,
	EDITMODE_OTHERS		,	//서브 디비젼 추가.
	EDITMODE_TYPECOUNT
};

// 글로벌 에러메시지 칼라링
// Alef Error Message colors...
#define	AEM_WARNNING	RGB( 180 , 255 , 180	)
#define	AEM_ERROR		RGB( 255 , 0 , 0		)
#define	AEM_NOTIFY		RGB( 38 , 38 , 255		)
#define	AEM_NORMAL		RGB( 38 , 38 , 38		)

// 디테일 스트링
#define	SECTORDETAIL_STRING_EMPTY		"Dummy Detail"
#define	SECTORDETAIL_STRING_LOWDETAIL	"Rough Detail"
#define	SECTORDETAIL_STRING_HIGHDETAIL	"High Detail"

// 디테일 표시 칼라
#define	SECTORDETAIL_BACK_COLOR			RGB( 40 , 40 , 40 )
#define	SECTORDETAIL_TEXT_COLOR_EMPTY	RGB( 40 , 40 , 255 )
#define	SECTORDETAIL_TEXT_COLOR_LOW		RGB( 40 , 255 , 40 )
#define	SECTORDETAIL_TEXT_COLOR_HIGH	RGB( 255 , 40 , 40 )

#define	DEFAULT_OBJECT_BRUSH_WIDTH		5000


// 타일쪽...
	#define	TILECATEGORY_FILE			"map\\tile\\categorylist.txt"
	#define	TILECATEGORY_ERROR_BITMAP	"errorbmp.bmp"
	#define	TILECATEGORY_WIDTH			75
	#define	TILECATEGORY_HEIGHT			20
	// 라인당 타일수.. 이것으로 계산함.
	#define	TILECATEGORY_COUNTPERLINE	3
	#define	TILECATEGORY_TILEWMARGIN	12
	#define	TILECATEGORY_TILEHMARGIN	2
	#define	TILECATEGORY_TILEWIDTH		50
	#define	TILECATEGORY_TILEHEIGHT		50

	// 레이어 표시 크기..
	// 레이어 없엠..
	// #define	TILELAYER_HEIGHT			20
	#define	TILELAYER_HEIGHT			0

	#define	TILELAYERSTRING_BACKGROUND	"배경찍기"
	#define	TILELAYERSTRING_ALPHATILE	"Alpha Tiling 작업"
	#define	TILELAYERSTRING_UPPER		"각각 알파이미지 올려 놓기"
	#define	TILELAYERCOLOR_BACKTEXT		RGB( 224 , 244 , 50 )
	#define	TILELAYERCOLOR_BACKBACK		RGB( 35 , 23 , 68 )
	#define	TILELAYERCOLOR_ALPHATEXT	RGB( 224 , 100 , 100 )
	#define	TILELAYERCOLOR_ALPHABACK	RGB( 224 , 244 , 50 )
	#define	TILELAYERCOLOR_UPPERTEXT	RGB( 100 , 123 , 200 )
	#define	TILELAYERCOLOR_UPPERBACK	RGB( 154 , 54 , 20 )

	// Color Table
	#define	TILECATEGORY_TILEINFOTEXTCOLOR_USED		RGB( 255	, 255	, 255	)
	#define	TILECATEGORY_TILEINFOTEXTCOLOR_UNUSED	RGB( 255	, 154	, 145	)
	#define	TILECATEGORY_TILEINFOTEXTBKCOLOR		RGB( 0		, 0		, 0		)
	#define TILECATEGORY_BACKGROUNDCOLOR			RGB( 64		, 64	, 64	)
	#define	TILECATEGORY_COLOR_SELECTEDBOX			RGB( 0		, 0		, 0		)
	#define	TILECATEGORY_COLOR_SELECTEDTEXT			RGB( 255	, 128	, 128	)
	#define	TILECATEGORY_COLOR_DEFAULTBOX			RGB( 64		, 64	, 64	)
	#define	TILECATEGORY_COLOR_DEFAULTTEXT			RGB( 200	, 200	, 200	)

	enum	TILELAYER
	{
		TILELAYER_BACKGROUND	,
		TILELAYER_TONGMAP		,
		TILELAYER_ALPHATILEING	,
		TILELAYER_UPPER			,
		TILELAYER_COUNT
	};

// Geometry 에디트 쪽 창.
// UI Geomtry
// 메뉴아래에 브러시 그림들어감.
enum UIGM_BRUSH_TYPE
{
	BRUSHTYPE_GEOMETRY		,		// 올라라
	BRUSHTYPE_SMOOTHER		,		// 반들반들
	BRUSHTYPE_VERTEXCOLOR	,		// 버택스칼라..
	BRUSHTYPE_CHANGENORMAL	,
	BRUSHTYPE_EDGETURN		,
	BRUSHTYPE_COUNT
};

#define	UIGM_COLOR_BACKGROUND	RGB( 35 , 23 , 68 )
#define	UIGM_COLOR_CIRCLE		RGB( 255 , 255 , 255 )
#define	UIGM_COLOR_TEXT			RGB( 231 , 128 , 52 )

#define	UIGM_MAX_CIRCLE_SIZE	( ALEF_SECTOR_WIDTH / 2.0f	)
#define	UIGM_MIN_CIRCLE_SIZE	( ALEF_SECTOR_WIDTH / 40.0f	)
#define	UIGM_DEFAULT_RADIUS		( ALEF_SECTOR_WIDTH / 20.0f	)
#define	UIGM_MENU_HEIGHT		50
#define	UIGM_DEFAULT_BRUSHTYPE	BRUSHTYPE_GEOMETRY

struct	stMenuItem
{
	// 메뉴 아이템 비트맵 타입..
	enum MIB_TYPE
	{
		MIB_SELECTED		,
		MIB_UNSELECT		,
		MIB_SELECTEDDOWN	,
		MIB_COUNT
	};

	INT32	nIndex	;
	CString	name	;	// 메뉴네임
	CRect	rect	;	// 선택랙트.
	CWnd *	pWnd	;	// 해당 윈도우의 핸들.
	CBitmap	abitmap[ MIB_COUNT ];

	BOOL	bEnable	;

	stMenuItem()
	{
		bEnable	 = TRUE;
	}
};

// 메인윈도우에서 사용하는 언두 오브젝트.;
	struct ObjectUndoInfo
	{
		INT32	oid			;	// Object ID
		AuPOS	stScale		;	// Scale Vector
		AuPOS	stPosition	;	// Base Position
		FLOAT	fDegreeX	;	// Rotation Degree
		FLOAT	fDegreeY	;
	};

// 오브젝트 팝업 매뉴 인덱스 스타트..
	#define	IDM_OBJECT_POPUP_OFFSET	19324

	enum DWSectorSetting
	{
		DWSECTOR_EACHEACH			,
		DWSECTOR_ALLROUGH			,
		DWSECTOR_ALLDETAIL			,
		DWSECTOR_ACCORDINGTOCURSOR	,
		DWSECTOR_SETTING_COUNT
	};



// 로딩모드
enum LOADINGMODE
{
	LOAD_NORMAL				,
	LOAD_EXPORT_DIVISION	,
	LOAD_EXPORT_LIST		
};

//inline int	GetSectorBlockIndex( int arrayx , int arrayy )
//{
//	// 14는 한라인 갯수..
//	return 1 + arrayx/10 + (arrayy/10) * 14;
//}

inline void	RemoveFullPath( char * fullpath , char * filename)
{
	char			drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath	( fullpath	, drive, dir, fname, ext );

	wsprintf( filename , "%s%s" , fname , ext );
}

enum GRID_MODE
{
	GRIDMODE_NONE			,	// No Grid
	GRIDMODE_D_GETHEIGHT	,	// 
	GRIDMODE_SECTOR_GRID	,
	GRIDMODE_HEIGHT_SECTOR	,	// 위에거 둘다.
	GRIDMODE_COUNT
};

inline	FLOAT	frand()
{
	return ( FLOAT ) ( rand() % RAND_MAX ) / ( FLOAT ) RAND_MAX;
}

#define ALEF_BRUSH_LOAD_NORMAL	( g_Const.m_brushLoadNormal )
#define ALEF_BRUSH_LOAD_EXPORT	( g_Const.m_brushLoadExport )

#define ALEF_BRUSH_BACKGROUND	( g_Const.m_nGlobalMode == LOAD_NORMAL ? g_Const.m_brushLoadNormal : g_Const.m_brushLoadExport )

// 마고자 (2005-04-06 오전 10:40:33) : 
// 세이브 플래그 설정..
void	SaveSetChangeGeometry	();
void	SaveSetChangeMoonee		();
void	SaveSetChangeTileList	();
void	SaveSetChangeObjectList	();

void	GetSubDataDirectory		( char * pStr );

void	ClearAllSaveFlags		();

// 마고자 (2005-06-16 오전 11:57:54) : 
// NPC와 스폰을 구분하기 위한 플래그..

#define	BOSSMOB_OFFSET	100000
#define	ISNPCID( id )	( id < BOSSMOB_OFFSET )
#define	ISBOSSID( id )	( id >= BOSSMOB_OFFSET )

#define ISBUTTONDOWN( key )	( GetAsyncKeyState( key ) < 0 )

#define USE_NEW_GRASS_FORMAT
