// ApWorldSector.h: interface for the ApWorldSector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APWORLDSECTOR_H__FA496200_24F6_4F97_8D80_4AAA2462BEA1__INCLUDED_)
#define AFX_APWORLDSECTOR_H__FA496200_24F6_4F97_8D80_4AAA2462BEA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "MagList.h"
#include "ApmMap.h"
#include "ApDefine.h"
#include "ApMapBlocking.h"
#include "AuOTree.h"
#include "AuList.h"
#include "ApRWLock.h"
#include <vector>

//#ifdef DEBUG_NEW
//#undef DEBUG_NEW

// 상수 정의.
#define	ALEF_MAPFILE				0x0001
#define	MAPSTRUCTURE_FILE_VERSION	0x0002

#define	ALEF_GEOMETRYFILE			0x0002

//#define GEOMETRY_FILE_VERSION		0x0003
#define GEOMETRY_FILE_VERSION		0x0004

#define	ALEF_MOONIEFILE				0x0003
//#define MOONEE_FILE_VERSION		0x0003
#define MOONEE_FILE_VERSION			0x0004
// 마고자 (2005-03-15 오후 3:27:56) : 버젼 업.. 지오메트리 이펙트 정보 추가

#define ALEF_COMPACT_FILE_VERSION	0x1000
#define ALEF_COMPACT_FILE_VERSION2	0x1001
	// 마고자 (2005-07-19 오전 11:26:02) : 시작시 지형 옵션 추가..
#define ALEF_COMPACT_FILE_VERSION3	0x1002
	// 마고자 (2005-12-13 오후 4:45:27) : 높이값 FLOAT로 변경 , 리젼값 UINT8 -> UINT16 , 리절브 필드 삭제.
#define ALEF_COMPACT_FILE_VERSION4	0x1003
	// 마고자 (2007-07-03) : 타일에 오브젝트 블러킹 4비트를 NoLayer 옵션과 Reserved로 변경.
#define ALEF_SERVER_DATA_VERSION	0x2001

#define	MAPSTRUCTURE_FILE_HEADER	( ( ALEF_MAPFILE		<< 16 ) | MAPSTRUCTURE_FILE_VERSION	)
#define	ALEF_GEOMETRYFILE_HEADER	( ( ALEF_GEOMETRYFILE	<< 16 ) | GEOMETRY_FILE_VERSION		)
#define	ALEF_MOONIEFILE_HEADER		( ( ALEF_MOONIEFILE		<< 16 ) | MOONEE_FILE_VERSION		)

#define SERVER_DATA_VERSION			0x10011001

// 마고자 (2005-03-15 오후 3:29:18) : 
// 이전 버젼과 호환용..
#define	ALEF_MOONIEFILE_HEADER_COMPATIBLE	( ( ALEF_MOONIEFILE		<< 16 ) | 0x003	)


#define	ALEF_GetFileType( x )		( x >> 16 )
#define	ALEF_GetFileVersion( x )	( x & 0x0000ffff )
#define	ALEF_SECTOR_DEFAULT_TILE_INDEX		( 0x00010380 )		// 디폴트로 들어가게 돼는 타일 번호
#define	ALEF_SECTOR_DEFAULT_ROUGHTILE_INDEX	( 0x00010200 )		// 디폴트로 들어가게 돼는 타일 번호
#define	ALEF_DEFAULT_ALPHA					( 0x000001c0 )		// 디폴트 텍스쳐의 알파 인덱스.
#define	ALEF_TEXTURE_NO_TEXTURE				( 0 )
#define	ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT	1.0f	// 세그먼트가 만들어질때 디폴트 높이

#define	ALEF_MAX_HEIGHT					10000.0f
#define	ALEF_MIN_HEIGHT					0.0f

#define	ALEF_COMPACT_DATA_DEFAULT_DEPTH	16

#define	MAX_BLOCKCOUNT				20
#define	MAX_USER_COUNT_IN_SECTOR	200
#define	MAX_ITEM_COUNT_IN_SECTOR	200
#define	MAX_OBJECT_COUNT_IN_SECTOR	200
#define	MAP_INITIAL_HEIGHT			(5.0f)

enum TextureDepth
{
	TD_FIRST	,	// 베이스 텍스쳐
	TD_SECOND	,	// 알파 텍스쳐
	TD_THIRD	,	// 알파가 적용됄 베이스 텍스쳐/
	TD_FOURTH	,	// 3섹터 알파용..
	TD_FIFTH	,	// 3섹터 알파용..
	TD_SIXTH	,	// 위에 그려질 텍스쳐.

	// Reserved
	TD_SEVENTH	,
	TD_EIGHTTH	,
	TD_DEPTH
};

#define	SECTOR_COMPACT_BLOCK_FILE		"c%04d.mpf"
#define	SECTOR_GEOMETRY_BLOCK_FILE		"g%04d.mpf"
#define	SECTOR_MOONIE_BLOCK_FILE		"m%04d.mpf"
#define	SECTOR_SERVERDATA_FILE			"sd%04d.bin"

#define	SECTOR_MAX_COLLISION_OBJECT_COUNT	10

// 텍스쳐 인덱스 정리.
// 11111111 | 11111111 | 11111111 | 11111111
//    안씀    Category    Index    앞에 두비트는 Type , 뒤에 6개는 Offset.
//

// Tile Blocking Flag..
// INT32 에서 블러킹..
// 각각 char형의 데이타에 비트로 설정한다.
// 33333333222222221111111100000000 으로 처리한다.
#define	TBF_NONE		0
#define	TBF_GEOMETRY	1
#define	TBF_OBJECT		2

enum	TILE_POSITION
{
	TP_LEFTTOP		,
	TP_RIGHTTOP		,
	TP_LEFTBOTTOM	,
	TP_RIGHTBOTTOM	,
	TP_MAX
};

// 블러킹 변수에 값을 설정함.
// 섹터에서 블러킹 정보를 기록할때 사용한다.
// 블러킹 정보는 한 그리드를 2x2로 쪼개어 저장한다.
// nPosition은 TILE_POSITION 값..
// nFlag 값을 무조건 대입하므로, 원래 값을 유지 하면서 플래그 추가를 위해서는
// _GetBlocking_Unit 함수와 같이 사용하여야 한다.
inline	void	_SetBlocking_Unit( INT32 * pBlocking , INT32 nPosition , INT32 nFlag	)
{
	INT32 mask = ~( 0xff << ( nPosition * 8 ) );
	*pBlocking &= mask;
	*pBlocking |= nFlag << ( nPosition * 8 );
}

// 해당 위치의 값을 얻어냄..
// nPosition은 TILE_POSITION 값..
inline	INT32	_GetBlocking_Unit( INT32 * pBlocking , INT32 nPosition					)
{
	return ( *pBlocking & ( 0x000000ff << ( nPosition * 8 ) ) ) >> ( nPosition * 8 );
}



// 지형의 높이를 저장하기 위한 스트럭쳐.

class	ApmMap						;

// Vertex Color를 저장하기 위한 스트럭쳐
struct	ApRGBA
{
	UINT8	red		;
	UINT8	green	;
	UINT8	blue	;
	UINT8	alpha	;	
};

// 섹터 디테일 레벨.
enum	SECTOR_DETAIL
{
	SECTOR_EMPTY		,
	SECTOR_LOWDETAIL	,
	SECTOR_HIGHDETAIL	,
	SECTOR_DETAILDEPTH	,
	SECTOR_ERROR		// 에러의 경우!
};

enum	TILE_DIRECTION
{
	TD_NORTH	,
	TD_EAST		,
	TD_SOUTH	,
	TD_WEST		,
	TD_DIRECTIONCOUNT
};

inline	void	_SetFlag( INT32 * pBlocking , int offset )
{
	int data = 0x01 << offset	;
	int mask = ~data			;

}

inline	void	_UnSetFlag( INT32 * pBlocking , int offset )
{
	int data = 0x01 << offset	;
	int mask = ~data			;

	*pBlocking &= mask			;
}

struct	ApTileInfo
{
	enum
	{
		BLOCKNONE		=	0x00	,
		BLOCKGEOMETRY	=	0x01	,
		BLOCKOBJECT		=	0x02	,
		BLOCKSKY		=	0x04
	};

	enum GEOMETRYBLOCK
	{
		GB_NONE		= 0x00,
		GB_GROUND	= 0x01,
		GB_SKY		= 0x02
	};

	UINT8	bEdgeTurn	  :1;	// 엣지 턴 정보.
	UINT8	tiletype	  :7;	// 타일의 러프한 타입을 정의한다.
	UINT8	geometryblock :4;	// 지형 블러킹.. 0x01 은 땅 0x02는 하늘 , 0x04,0x08은 예약필드.

	// UINT8	objectblock	  :4;	// 오브젝트 블러킹 . 4개의 위치를 비트 플래그로 적용.
	// 사용하지 않기때문에 삭제함.

	UINT8	bNoLayer	  :1;	// 복층을 사용하지 않음을표시 ,즉 모든경우 Ridable Object를 위로 올려버린다.
	UINT8	reserved	  :3;	// 훗날을 위한 예비..

	ApTileInfo()
	{
		bEdgeTurn		= 0;
		tiletype		= 0;
		geometryblock	= 0;
		//objectblock		= 0;

		bNoLayer		= 0;
		reserved		= 0;
	}

	/*
	// Zero Base
	void	SetObjectBlocking( INT32 nPosition , BOOL bBlock )
	{
		UINT8 data = 0x01 << nPosition	;
		UINT8 mask = ~data				;

		objectblock	 &= mask	;

		if( bBlock )
			objectblock |= data	;
	}

	BOOL	GetObjectBlocking( INT32 nPosition )
	{
		if( objectblock & ( 0x01 << nPosition ) )	return TRUE		;
		else										return FALSE	;
	}
	*/

	void	SetNoLayer( BOOL bNoLayer )
	{
		this->bNoLayer = bNoLayer ? TRUE : FALSE;
	}

	BOOL	IsNoLayer() { return ( BOOL ) bNoLayer; }

	void	SetGeometryBlocking( BOOL bBlock )
	{
		UINT8 data = GB_GROUND;
		UINT8 mask = ~data			;

		geometryblock	 &= mask	;
		if( bBlock )
			geometryblock |= data	;
	}

	void	SetSkyBlocking( BOOL bBlock )
	{
		UINT8 data = GB_SKY;
		UINT8 mask = ~data			;

		geometryblock	 &= mask	;
		if( bBlock )
			geometryblock |= data	;
	}

	BOOL	GetGeometryBlocking()
	{
		return ( geometryblock & GB_GROUND ) ? TRUE : FALSE;
	}

	BOOL	GetSkyBlocking()
	{
		return ( geometryblock & GB_SKY ) ? TRUE : FALSE;
	}

	UINT8	GetBlocking( INT32 nPosition )
	{
		// 마고자 (2004-05-03 오후 1:08:01) : 리턴 방식 변경..
		// 어떤 블러킹에 걸렸는지 플래그로 남김..
		// 체크는 ApTileInfo::BLOCKGEOMETRY 이런 녀석들을 이요하면 ok
		UINT8	nRet = BLOCKNONE;
		if( GetGeometryBlocking()			) nRet |= BLOCKGEOMETRY	;
		//if( GetObjectBlocking( nPosition )	) nRet |= BLOCKOBJECT	;
		return nRet;
	}

	// 마고자 (2004-06-15 오전 10:42:13) : 엣지턴 정보..
	BOOL	SetEdgeTurn( BOOL bEnable )
	{
		if( bEnable )	bEdgeTurn = TRUE	;
		else			bEdgeTurn = FALSE	;
		return bEdgeTurn;
	}
	BOOL	GetEdgeTurn() { return ( BOOL ) bEdgeTurn; }
};

#define APCOMPACTSEGMENTSIZE			4
#define APCOMPACTSEGMENTSIZE_20040917	6
#define APCOMPACTSEGMENTSIZE_20051213	8

// 클라이언트에서 사용되는 압축된 지형정보를 저장하기 위한 세그먼트 클래스..
struct	ApCompactSegment
{
	// 마고자 (2005-12-19 오전 10:45:57) : 쓰지 않으니 빼버림.따로 복층데이타로 통합.
	// FLOAT		fHeight	;	// 높이정보 , 배율은 따로 정해야함..
	ApTileInfo	stTileInfo	;
	
	UINT16		uRegioninfo		;	// 리전 정보..

	ApCompactSegment():uRegioninfo(0)
	{
	}

	// 마고자 (2004-09-17 오전 11:46:38) : 
	// 리전 설정..
	UINT16	GetRegion()					{ return uRegioninfo; }
	UINT16	SetRegion( UINT16 uRegion)	{ return uRegioninfo = uRegion; }
};

struct	ApCompactSegment_20040917
{
	UINT16		fHeight			;	// 높이정보 , 배율은 따로 정해야함..
	ApTileInfo	stTileInfo		;
	UINT8		uRegioninfo		;	// 리전 정보..
	UINT8		uReserved		;
};


// 맵툴에서 사용하는 세부적인 정보가 들어 있는 
struct	ApDetailSegment
{
	enum	FLAG
	{
		USEFLATSHADING	,
		FLAGCOUNT		,
	};
	
	FLOAT		height				;
	UINT32		pIndex[ TD_DEPTH ]	; // 타일번호. ALEF_TILE_DO_NOT_USE 이면 사용하지 않음.
	ApRGBA		vertexcolor			;
	ApTileInfo	stTileInfo			; // 블러킹 정보 저장용.. 이녀석 전용 수정 펑션을 이용한다.. 2x2의 블러킹 정보가 들어간다.
	UINT16		nPadding			;

	// 데이타 정의 필드..
	
	INT32	nFlags					;	// 비트 플래그 32개를 저장함..
	UINT16	uCheckTime				;	// 툴에서 사용하는 체크타임..

	void	SetDefault( FLOAT fDefaultHeight = 1.0f )
	{
		height					= fDefaultHeight			;

		// 텍스쳐 초기화.
		pIndex[ TD_FIRST ]	= ALEF_SECTOR_DEFAULT_TILE_INDEX;
		for( int t = 1 ; t < TD_DEPTH ; ++t )
			pIndex[ t ]		= ALEF_TEXTURE_NO_TEXTURE	;

		// 버텍스 칼라 초기화.
		vertexcolor	.alpha		= 255						;
		vertexcolor	.red		= 255						;
		vertexcolor	.green		= 255						;
		vertexcolor	.blue		= 255						;

		// Reserved
		nFlags					= 0							;
		nPadding				= 0							;
	}

	//void	SetBlocking( INT32 nPosition , INT32 nFlag	) { stTileInfo.SetGeometryBlocking( nFlag ); }
	//INT32	GetBlocking( INT32 nPosition				) { return stTileInfo.GetGeometryBlocking(); }

	void	SetFlag		( INT32 nFlagIndex , BOOL bData	)
	{
		ASSERT( nFlagIndex >= 0 && nFlagIndex < FLAGCOUNT );

		int	mask	= 0x00000001 << nFlagIndex				;
		int	bitflag	= ( ( bData ) ? 1 : 0 ) << nFlagIndex	;

		nFlags = ( nFlags & ~mask ) | bitflag				;
	}

	BOOL	GetFlag		( INT32 nFlagIndex				)
	{
		ASSERT( nFlagIndex >= 0 && nFlagIndex < FLAGCOUNT );
		return nFlags & ( 0x00000001 << nFlagIndex )		;
	}
};

struct	ApCompactSectorInfo
{
	ApCompactSegment *	m_pCompactSegmentArray		;
	INT32				m_nCompactSegmentDepth		;

	ApCompactSectorInfo()
	{
		m_nCompactSegmentDepth = 0		;
		m_pCompactSegmentArray = NULL	;
	}

	~ApCompactSectorInfo()
	{
		Free();
	}

	BOOL	Alloc( int nDepth )
	{
		Free();
		m_nCompactSegmentDepth = nDepth;
		m_pCompactSegmentArray = new ApCompactSegment[ nDepth * nDepth ];
		return TRUE;
	}

	BOOL	Free()
	{
		if( m_pCompactSegmentArray )
		{
			delete [] m_pCompactSegmentArray;
			m_pCompactSegmentArray = NULL;
		}
		m_nCompactSegmentDepth = 0;

		return TRUE;
	}

	ApCompactSegment * GetSegment( INT32 x , INT32 z )
	{
		if( m_pCompactSegmentArray )
			return &m_pCompactSegmentArray[ x + z * m_nCompactSegmentDepth ];
		else
			return NULL;
	}

	BOOL	IsLoaded() { if( m_pCompactSegmentArray ) return TRUE; else return FALSE; }
};

struct	ApDetailSectorInfo
{
	ApDetailSegment *						m_pSegment		[ SECTOR_DETAILDEPTH ];
	int										m_nDepth		[ SECTOR_DETAILDEPTH ]; // 디테일

	// Reserved parameter;
	int		m_nAlignment;	// 텍스쳐 사용 타입.
	int		m_nReserved1;
	int		m_nReserved2;
	int		m_nReserved3;
	int		m_nReserved4;
};

// 마고자 (2005-12-14 오후 3:10:20) : 
// 서버 폴리건 정보를 저장하기 위한 데이타.
struct ApServerSectorInfo
{
protected:
	AuPOS	* pcsVector		;

	INT32	nTotal			;
	INT32	nBlockingCount	;
	INT32	nRidableCount	;

public:
	AuPOS	* GetVector() { return pcsVector		; }

	INT32	GetTotal		() { return nTotal			; }
	INT32	GetBlockingCount() { return nBlockingCount	; }
	INT32	GetRidableCount	() { return nRidableCount	; }
	
	ApServerSectorInfo():pcsVector( NULL ),nTotal( 0 ),nBlockingCount( 0 ),nRidableCount( 0 ) {}
	~ApServerSectorInfo(){ FreeVector(); }

	BOOL	AllocVector( int nCount )
	{
		FreeVector();

		if( nCount > 0 )
		{
			pcsVector		= new AuPOS[ nCount ];
			nTotal			= nCount;
			nBlockingCount	= nCount;
			nRidableCount	= nCount;
			return TRUE;
		}
		else
			return FALSE;
	}

	void	SetBlockingVertexCount( int nCount )
	{
		nBlockingCount = nCount;
	}
	void	SetRidableVertexCount( int nCount )
	{
		nRidableCount = nCount;
	}

	void	FreeVector()
	{
		if( pcsVector ) delete [] pcsVector;
		pcsVector		= NULL;
		nTotal			= 0;
		nBlockingCount	= 0;
		nRidableCount	= 0;
	}
};
		
	//typedef struct IdPos IdPos;
// 월드 섹터 정의..
class ApWorldSector  
{
public:
	// 맵퍼블릭 모듈 프랜드선언..
	friend class ApmMap;

	typedef enum
	{
		STATUS_INIT		= 0x01,	// 섹터가 메모리잡히교 콜백이 불려져 있는 상태..
		STATUS_LOADED	= 0x02,	// 데이타 로드 펑션이 실행돼어 섹터에이타가 존재하고 있는경우.
		STATUS_DELETED	= 0x04, // 
	} Status;

	typedef struct	IdPos
	{
		AuPOS	pos		;
		INT32	range	;
		INT_PTR	id		;	// pCharacter 
		INT_PTR	id2		;	// CID
		IdPos *	pNext	;
		IdPos *	pPrev	;
	} IdPos;

	enum HEIGHTINFO
	{
		NOHEIGHTINFO			= 0x00,
		HEIGHTWITHRIDABLEOBJECT	= 0x01,
		HEIGHTWITHGEOMETRYONLY	= 0x02,
		HEIGHTINFO_MAX			= 0x03
	};
	struct	HeightPool
	{
		UINT8	uDataAvailable	;
		FLOAT	fHeight			;
		FLOAT	fGeometryHeight	;

		HeightPool():uDataAvailable( NOHEIGHTINFO ),fHeight(0.0f), fGeometryHeight( 0.0f ) {}
	};

	// 인덱스 저장을 위한 녀석..
	struct	IndexStruct
	{
		INT32	nCount	;
		INT32	pArray[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];

		IndexStruct():nCount(0){}
	};

	struct	IndexArray
	{
		IndexStruct	*	pIndexStruct	;
		INT32			nDepth			;

		IndexArray(): pIndexStruct( NULL ) , nDepth( 0 ) {}
	};

	struct	AuLineBlock	: public AuLine
	{
	public:
		UINT32 uSerial; // 블러킹 정보의 인덱스.. 중복시 제거 확인용.

		AuLineBlock() : uSerial( 0 ) {}

		void	FromAuLine( AuLine &stParam )
		{
			this->start	= stParam.start	;
			this->end	= stParam.end	;
		}
	};

	enum	INDEX_TYPE
	{
		AWS_COLLISIONOBJECT	= 0	,	// 콜리젼 오브젝트..
		AWS_SKYOBJECT			,	// 스카이 정보를 가진 오브젝트..
		AWS_GRASS				,	// 풀정보를 가진 오브젝트..
		AWS_RIDABLEOBJECT		,
		AWS_COUNT			
	};

	// 마고자 (2005-03-15 오후 3:24:10) : 
	// 지오메트리 옵션들..
	enum	SECTOR_OPTIONS
	{
		OP_NONE					= 0x0000,
		OP_GEOMETRYEFFECTENABLE	= 0x0001,
		OP_DONOTLOADSECTOR		= 0x0002,	// 지형데이타를 클라이언트에서 읽어들이지 않는다.
		OP_HEIGHTUPDATEAFTERLOAD= 0x0004,	// 지형 읽어들인 후에 케릭터 높이를 갱신한다. ( 클라이언트전용 )
	};

	enum	LOADING_FLAG
	{
		LF_NONE				= 0x0000,
		LF_OBJECT_LOADED	= 0x0001
	};
	
protected:
	INT32			m_lStatus		; // WorldSector의 현재 상태 바꿀때는 반드시 Lock하고 바꾸시오.

	BOOL			m_bInitialized	;

	// 섹터가 커버하는 범위.
	FLOAT			m_fxStart		; // 전체지형에서의 X좌표 스타트
	FLOAT			m_fxEnd			;
	FLOAT			m_fyStart		; // 전체지형에서의 Y좌표 스타트
	FLOAT			m_fyEnd			;
	FLOAT			m_fzStart		; // 전체지형에서의 Z좌표 스타트
	FLOAT			m_fzEnd			;

	//FLOAT			m_fStepSize		; // 격자형 스텝 사이즈..

	// 맵에서의 섹터 인덱스 .
	INT32			m_nIndexX		;
	INT32			m_nIndexZ		;

	INT32			m_nCurrentDetail;

	// 마고자 (2005-03-15 오후 3:24:33) : 
	DWORD			m_uFlag			; // 옵션들..
	DWORD			m_uLoadingFlag	;

public:
	// 섹터 에 저장돼는 정보..

	// 계 스트럭쳐..
	// 이정보가 계 기준으로 생성이 됀다..

	struct	Dimension
	{	
		INT32	nIndex		;	// 생성된 계 인덱스 . ApmMap에서 부여해줌.		

		IdPos *	pUsers		;
		IdPos *	pNPCs		;
		IdPos *	pMonsters	;
		IdPos *	pItems		;
		IdPos *	pObjects	;

		// 각각 락을 추가.. 조심해서 사용하자.
		// 좀더 괜찮은 구조가 있으면 바꾸고 싶지만
		// 생각하기가 귀찮으므로 노가다로 진행.

		ApRWLock	lockUsers	;
		ApRWLock	lockNPCs	;
		ApRWLock	lockMonsters;
		ApRWLock	lockItems	;
		ApRWLock	lockObjects	;

		Dimension() :
			nIndex		( 0	   ),
			pUsers		( NULL ), 
			pNPCs		( NULL ), 
			pMonsters	( NULL ), 
			pItems		( NULL ), 
			pObjects	( NULL )
		{
			// do nothing..
		}

		Dimension( const Dimension &stParam )
		{
			this->nIndex	= stParam.nIndex	;
			this->pUsers	= stParam.pUsers	;
			this->pNPCs		= stParam.pNPCs		;
			this->pMonsters	= stParam.pMonsters	;
			this->pItems	= stParam.pItems	;
			this->pObjects	= stParam.pObjects	;
		}
	};

	// 디멘젼 처리..
	std::vector< Dimension >		m_arrayDimension;	// 계배열을 저장
	std::vector< AuLineBlock >		m_vecBlockLine;	// 라인 블러킹 정보..

	BOOL	AddLineBlock( AuLineBlock & stParam );
	BOOL	ClearLineBlock();
	INT32	GetLineBlockCount();

	Dimension *	GetDimensionArray() { return &m_arrayDimension[ 0 ]; }
	INT32		GetDimensionCount() { return (INT32)m_arrayDimension.size(); }
	Dimension *	GetDimension( INT32 nCreatedIndex );

	BOOL	CreateDimension( INT32 nCreatedIndex );
	BOOL	DeleteDimension( INT32 nCreatedIndex );

	//ApCriticalSection	m_Mutex						;
	ApRWLock			m_RWLock					;
	AuOTree				m_csOTree					;
	
	// AgcmMap 에서 사용하는 큐 옵셋 저장용.
	UINT32				m_uQueueOffset				;

	UINT32				m_ulRemoveTimeMSec			;

	// Compact Segment Data...
	ApCompactSectorInfo	*	m_pCompactSectorInfo	;
	ApDetailSectorInfo	*	m_pDetailSectorInfo		;
	ApServerSectorInfo		m_ServerSectorInfo		;	// 포인터사용하지않음..

	BOOL	AllocDetailSectorInfo	();
	BOOL	FreeDetailSectorInfo	();

	BOOL	AllocCompactSectorInfo	();
	BOOL	FreeCompactSectorInfo	();

	bool	LockSector		(); // 전체에 대한 Write Lock
	void	ReleaseSector	(); // Write Lock 한거 릴리즈..

protected:

	// Detail Segment Data...

	// Height Pool
	HeightPool	*		m_pHeightPool			;

	IndexArray			m_aIndexArray[ AWS_COUNT ]	;

	void				CreateCollisionObjectArray(){ CreateIndexArray( AWS_COLLISIONOBJECT ); }
	void				DeleteCollisionObjectArray(){ DeleteIndexArray( AWS_COLLISIONOBJECT ); }

public:
	void				CreateIndexArray( INT32 nIndexType );
	void				DeleteIndexArray( INT32 nIndexType );

	void				DeleteAllIndexArray()
	{
		for( int i = 0 ; i < AWS_COUNT ; ++ i ) DeleteIndexArray( i );
	}
	BOOL				AddObjectCollisionID( int x , int z , INT32 oid )
	{
		return			AddIndex( AWS_COLLISIONOBJECT , x , z , oid );
	}
	INT32				GetObjectCollisionID( int x , int z , INT32 * pArrayID , INT32 nMax )	// Return is its count.
	{
		return			GetIndex( AWS_COLLISIONOBJECT , x , z , pArrayID , nMax );
	}

	BOOL				AddIndex		( INT32	nIndexType , int x , int z , INT32 oid			, BOOL bOverlappingTest = FALSE );
	INT32				GetIndex		( INT32	nIndexType , int x , int z , INT32 * pArrayID	, INT32 nMax );
	BOOL				DeleteIndex		( INT32	nIndexType , int x , int z );	// 해당세그먼트의 인덱스를 삭제한다.
	BOOL				DeleteIndex		( INT32	nIndexType , int x , int z , INT32 nIndex		);	// 해당세그먼트의 인덱스를 삭제한다.

	BOOL				AddSkyObjectTemplateID	( int x , int z , INT32 nTemplate , FLOAT fDistance );
	INT32				GetSkyObjectTemplateID	( int x , int z , INT32 * pArrayID , INT32 nMax );	// Return is its count.

	INT32				GetStatus()	{ return m_lStatus; }

public:
	BOOL	AllocHeightPool			();
	BOOL	FreeHeightPool			();

	BOOL	HP_SetHeight			( int x , int z , FLOAT fHeight );
	FLOAT *	HP_GetHeight			( int x , int z );
	BOOL	HP_SetHeightGeometryOnly( int x , int z , FLOAT fHeight );
	FLOAT *	HP_GetHeightGeometryOnly( int x , int z );

// Operations
		// Get Functions..
		INT32	GetIndexX	() const	{ return m_nIndexX	; }
		INT32	GetIndexZ	() const	{ return m_nIndexZ	; }

		INT32	GetArrayIndexX() const;
		INT32	GetArrayIndexZ() const;

		int	GetArrayIndexDWORD	() const;
		inline int	GetCurrentDetail	() { return m_nCurrentDetail; }

		FLOAT	GetXStart	() const	{ return m_fxStart	; }
		FLOAT	GetXEnd		() const	{ return m_fxEnd	; }
		FLOAT	GetYStart	() const	{ return m_fyStart	; }
		FLOAT	GetYEnd		() const	{ return m_fyEnd	; }
		FLOAT	GetZStart	() const	{ return m_fzStart	; }
		FLOAT	GetZEnd		() const	{ return m_fzEnd	; }

		FLOAT	GetStepSizeX() ;
		FLOAT	GetStepSizeZ() ;
		FLOAT	GetStepSizeX( int nDetail ) ;
		FLOAT	GetStepSizeZ( int nDetail ) ;

		UINT	GetDivisionOffset()
		{
			return ( UINT ) ( (GetArrayIndexX() % 16) + (GetArrayIndexZ() % 16) * 16 );
		}

		// 마고자 (2005-03-15 오후 3:40:33) : 플래그 설정..
		DWORD	GetFlag		() const	{ return m_uFlag	; }
		void	SetFlag		( DWORD uFlag )	{ m_uFlag = uFlag;}

		void	SetLoadingFlag	( LOADING_FLAG eFlagIndex , BOOL bData	)
		{
			int	mask	= 0x00000001 << eFlagIndex				;
			int	bitflag	= ( ( bData ) ? 1 : 0 ) << eFlagIndex	;

			m_uLoadingFlag = ( m_uLoadingFlag & ~mask ) | bitflag;
		}

		BOOL	GetLoadingFlag		( LOADING_FLAG eFlagIndex				)
		{
			return m_uLoadingFlag & ( 0x00000001 << eFlagIndex )		;
		}

		ApCompactSectorInfo *	GetCompactSectorInfo()	{ return m_pCompactSectorInfo	; }
		ApDetailSectorInfo *	GetDetailSectorInfo()	{ return m_pDetailSectorInfo	; }

	// 공통 펑션들
protected:
		// 다음은 맵 퍼블릭 모듈에서만 호출이 가능하다. ( 콜백 지정때문에 );
		// 섹터의 정보를 처음으로 생성함..
		BOOL	SetupSector		( INT32 indexX , INT32 indexY , INT32 indexZ			);
		BOOL	LoadSector		( BOOL bLoadCompact = TRUE , BOOL bLoadDetail = TRUE	);
		BOOL	LoadSectorServer();
		//BOOL	_LoadSectorOld	( INT32 indexX , INT32 indexY , INT32 indexZ			);
		// 섹터의 모든 데이타를 날림..
		BOOL	RemoveAllData	();

public:

		// OTree : Parn 작업, OTree 생성하기 위해서 Init만들다.
		BOOL	Init			( FLOAT width	, FLOAT unitsize , BOOL bBlock = TRUE	);
		// OTree : AddObject로 들어간 Blocking 정보를 가지고 OTree를 Optimize한다.
		void	OptimizeBlocking();


		ApWorldSector *	GetNearSector	( int direction );	// 주위 섹터의 포인터를 구함.

		// 좌표가 이 섹터 안에 존재하는가..
		// 가로 좌표만을 검사한다.
		inline bool		IsInSector		( AuPOS pos	) { return IsInSector( pos.x , pos.y , pos.z ); }
		inline bool		IsInSector		( FLOAT x , FLOAT y , FLOAT z )
		{
			if( x <		m_fxStart	||	z <	m_fzStart	||
				x >=	m_fxEnd		||	z >= m_fzEnd	)	return false	;
			else											return true		;
		}
		BOOL	IsInSectorRadius( float x , float z , float radius );
		BOOL	IsInSectorRadiusBBox( float x1 , float z1 , float x2 , float z2 );

		// 블러킹 체크 함수.
		FLOAT	GetHeight		( FLOAT x , FLOAT y , FLOAT z	);	// 해당 좌표의 이론적 높이를 계산해낸다.
		UINT8	GetType			( FLOAT x , FLOAT y , FLOAT z	);	// 바닥의 타입을 얻어낸다.
		UINT8	GetBlocking		( FLOAT x , FLOAT y , FLOAT z	, INT32 eType );
			// 인덱스로 넘어온다..
			// 지형블러킹과 오브젝트 블러킹이 구분돼어 플래그로 넘어와서 체크한다.

		// 이건 50x50 좌표..
		//UINT8	SetBlocking		( INT32 x , INT32 z , INT32 nBlocking	);
			// nBlocking 은 Bit 플래그로 조합하여 들어가야 한다.
			// 바로 대입하는것이기 때문에 주의를 요한다.
		//UINT8	AddBlocking		( INT32 x , INT32 z , INT32 nBlockType	)
		//	{ return SetBlocking( x , z , GetBlocking( x , z , 0x00 ) | nBlockType ); }	// 0x00 은 GROUND BLOCK
		//UINT8	AddBlocking		( AuBLOCKING * pBlocking  , INT32 count	, INT32 nBlockType );
			// 이녀석은 타입만 넣는다. 해당 블러킹을 추가한다.
			// 원래 데이타에 Or 연산으로 값이 추가됀다.
		// void	ClearObjectBlocking();
		UINT8	GetBlocking		( INT32 x , INT32 z	, INT32 eType );
	
		FLOAT	GetHeight		( AuPOS pos ) { return GetHeight	( pos.x , pos.y , pos.z ); }	// 해당 좌표의 이론적 높이를 계산해낸다.
		UINT8	GetType			( AuPOS pos ) { return GetType		( pos.x , pos.y , pos.z ); }	// 바닥의 타입을 얻어낸다.
		UINT8	GetBlocking		( AuPOS pos , INT32 eType ) { return GetBlocking	( pos.x , pos.y , pos.z , eType ); }

		BOOL	IsPassThis		( AuPOS pos1 , AuPOS pos2 , AuPOS *pCollisionPoint  = NULL		);

		// 섹터안에 위치하는 케릭터 ,아이템 등의 정보를 저장,삭제하기 위한 함수들..
		BOOL	AddChar			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2 ,		INT32 range = 0		);
		BOOL	AddChar			( INT32	nDimensionIndex , IdPos *pUser											);
		BOOL	UpdateChar		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid							);
		IdPos *	GetChar			( INT32	nDimensionIndex , INT_PTR cid	,	BOOL bIsNeedLock = TRUE				);
		BOOL	DeleteChar		( INT32	nDimensionIndex , INT_PTR cid												);
		VOID	RemoveChar		( INT32	nDimensionIndex , IdPos *pUser ,	BOOL bIsNeedLock = FALSE			);

		BOOL	AddNPC			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2 ,		INT32 range = 0		);
		BOOL	AddNPC			( INT32	nDimensionIndex , IdPos *pNPC											);
		BOOL	UpdateNPC		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid							);
		IdPos *	GetNPC			( INT32	nDimensionIndex , INT_PTR cid	,	BOOL bIsNeedLock = TRUE				);
		BOOL	DeleteNPC		( INT32	nDimensionIndex , INT_PTR cid												);
		VOID	RemoveNPC		( INT32	nDimensionIndex , IdPos *pNPC ,		BOOL bIsNeedLock = FALSE			);

		BOOL	AddMonster		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2 ,		INT32 range = 0		);
		BOOL	AddMonster		( INT32	nDimensionIndex , IdPos *pMonster										);
		BOOL	UpdateMonster	( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid							);
		IdPos *	GetMonster		( INT32	nDimensionIndex , INT_PTR cid	,	BOOL bIsNeedLock = TRUE				);
		BOOL	DeleteMonster	( INT32	nDimensionIndex , INT_PTR cid												);
		VOID	RemoveMonster	( INT32	nDimensionIndex , IdPos *pMonster , BOOL bIsNeedLock = FALSE			);

		BOOL	AddItem			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid							);
		BOOL	AddItem			( INT32	nDimensionIndex , IdPos *pItem											);
		BOOL	UpdateItem		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid							);
		IdPos *	GetItem			( INT32	nDimensionIndex , INT_PTR iid	,	BOOL bIsNeedLock = TRUE				);
		BOOL	DeleteItem		( INT32	nDimensionIndex , INT_PTR iid												);
		VOID	RemoveItem		( INT32	nDimensionIndex , IdPos *pItem ,	BOOL bIsNeedLock = FALSE			);

		// Parn 작업 Blocking 정보 추가
		BOOL	AddObject		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid ,	AuBLOCKING *pstBlocking = NULL	);
		BOOL	AddObject		( INT32	nDimensionIndex , IdPos *pObject			  ,		AuBLOCKING *pstBlocking = NULL	);
		BOOL	UpdateObject	( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid									);
		IdPos *	GetObject		( INT32	nDimensionIndex , INT_PTR oid	,		BOOL bIsNeedLock = TRUE						);
		BOOL	DeleteObject	( INT32	nDimensionIndex , INT_PTR oid														);
		VOID	RemoveObject	( INT32	nDimensionIndex , IdPos *pObject ,	BOOL bIsNeedLock = FALSE					);

	// Compact Segment Funcitons...

		// 로딩 관련..
		BOOL	IsLoadedCompactData				() { if( m_pCompactSectorInfo && m_pCompactSectorInfo->IsLoaded() ) return TRUE ; else return FALSE; }
		BOOL	LoadCompactData					();	//	Compact 데이타를 읽어들임.
		//BOOL	_LoadCompactData_Old			();	//	Compact 데이타를 읽어들임.
		BOOL	_LoadCompactData_FromBuffer		( BYTE * pBuffer , UINT uSize );	//	Compact 데이타를 읽어들임.
		
		BOOL	LoadServerData					();
			// 마고자 (2005-12-14 오후 3:31:47) : 
			// 서버정보 로딩할때 쓰는 용도..
			// 서버 폴리건정보를 읽어들임.
		
		BOOL	SaveCompactData					( BOOL bServer = FALSE );	//	현재 로드됀 컴펙트 데이타를 저장함.

		BOOL	CreateCompactDataFromDetailInfo	( int nTargetDetail = SECTOR_HIGHDETAIL , int nDepth = ALEF_COMPACT_DATA_DEFAULT_DEPTH );	//	디테일 데이타에서 컴펙트 데이타를 생성함.

		BOOL	RemoveCompactData				();	//	컴펙트 데이타 메모리에서 제거..
		BOOL	RemoveCompactDataFile			();	//	컴펙트 데이타 파일 제거.

		FLOAT	C_GetHeight						( FLOAT x , FLOAT y , FLOAT z	);	// 해당 좌표의 이론적 높이를 계산해낸다.
		FLOAT	C_GetHeight						( INT32 nX , INT32 nZ			);	// 해당 좌표의 이론적 높이를 계산해낸다.
		UINT8	C_GetType						( FLOAT x , FLOAT y , FLOAT z	);	// 바닥의 타입을 얻어낸다.
		UINT8	C_GetBlocking					( FLOAT x , FLOAT y , FLOAT z , INT32 eType );	// 바닥의 타입을 얻어낸다.

		ApCompactSegment	* C_GetSegment		( FLOAT x , FLOAT y , FLOAT z );
		ApCompactSegment	* C_GetSegment		( INT32 x , INT32 z );

		UINT8	GetTileCompactType				( INT32 tileindex				);

	// Detail Segment Functions....

		// 로딩 관련..
		BOOL	IsLoadedDetailData		() const { if( m_pDetailSectorInfo ) return TRUE ; else return FALSE; }
		BOOL	IsLoadedDetailData		( INT32 nTargetDetail );

		BOOL	LoadDetailData			();	// 디테일 데이타 로딩.
		BOOL	SaveDetailData			();	// 디테일 데이타 저장.

		// 지형 먼저. 타일을 후에..
		BOOL	LoadDetailDataGeometry	();	// 높이정보
		BOOL	LoadDetailDataTile		();	// 타일+버텍스 칼라
		BOOL	_LoadDetailDataGeometry	( char * pfilename = NULL );	// 높이정보
		BOOL	_LoadDetailDataTile		( char * pfilename = NULL );	// 타일+버텍스 칼라
		BOOL	SaveDetailDataGeometry	();
		BOOL	SaveDetailDataTile		();

		inline	void	SetCurrentDetail( BOOL bDetail ) { m_nCurrentDetail = bDetail; }
		
		BOOL	CreateDetailData		( INT32 nTargetDetail , INT32 nDepth , FLOAT fDefaultHeight = 1.0f );	// 해당 디테일 정보 생성..
		BOOL	GenerateRoughMap		( INT32 nTargetDetail , INT32 sourcedetail , FLOAT offset ,
										BOOL bHeight = TRUE , BOOL bTile = FALSE , BOOL bVertexColor = FALSE );

		BOOL	RemoveAllDetailData		();	// 디테일 데이타 메모리에서 제거.
		BOOL	RemoveDetailData		( INT32 nTargetDetail );	// 디테일 데이타 메모리에서 제거. 해당 디테일만.
		BOOL	RemoveDetailDataFile	();	// 데이타 파일까지 제거함..

		INT32	D_GetDepth				( INT32 nTargetDetail );
		FLOAT	D_GetStepSize			( INT32 nTargetDetail )
		{
			if( m_pDetailSectorInfo )
				return ( ( m_fxEnd - m_fxStart ) / ( ( float ) m_pDetailSectorInfo->m_nDepth[ nTargetDetail ] ) );
			else
			{
				switch( nTargetDetail )
				{
				case SECTOR_LOWDETAIL	:	return 1600.0f	;	// 마고자 (2003-11-03 오전 12:39:59) : 수정 필요함.
				case SECTOR_HIGHDETAIL	:	return 400.0f	;
				default					:	return 1.0f		;
				}
			}
		}

		// 섹터 편지 펑션들..
		ApDetailSegment * D_GetSegment	( INT32 nTargetDetail , INT32 x , INT32 z ,	FLOAT *pfPosX = NULL,	FLOAT *pfPosZ = NULL	);
		ApDetailSegment	* D_GetSegment	( INT32 nTargetDetail , FLOAT x , FLOAT z ,	INT32 *pPosX = NULL ,	INT32 *pPosZ = NULL		);

		FLOAT	D_GetHeight2			( INT32 x , INT32 z );
		FLOAT	D_GetHeight				( INT32 nTargetDetail , FLOAT x , FLOAT z );	// 해당 좌표의 이론적 높이를 계산해낸다.
		ApRGBA	D_GetValue				( INT32 nTargetDetail , FLOAT x , FLOAT z );
		UINT32 *D_GetTile				( INT32 nTargetDetail , FLOAT x , FLOAT z );
		FLOAT	D_GetRoughHeight		( INT32 nTargetDetail , INT32 x , INT32 z );	// 해당 좌표의 이론적 높이를 계산해낸다.

		FLOAT	D_SetHeight				( INT32 nTargetDetail , INT32 x , INT32 z , FLOAT height	);
		ApRGBA	D_SetValue				( INT32 nTargetDetail , INT32 x , INT32 z , ApRGBA value	);
		UINT32 *D_SetTile				( INT32 nTargetDetail , INT32 x , INT32 z , UINT32 firsttexture ,
										UINT32 secondtexture	= ALEF_TEXTURE_NO_TEXTURE	,
										UINT32 thirdtexture		= ALEF_TEXTURE_NO_TEXTURE	,
										UINT32 fourthtexture	= ALEF_TEXTURE_NO_TEXTURE	,
										UINT32 fifthtexture		= ALEF_TEXTURE_NO_TEXTURE	,
										UINT32 sixthtexture		= ALEF_TEXTURE_NO_TEXTURE	);
		
		INT32	D_SetAllTile			( INT32 nTargetDetail , INT32 tileindex					);

		FLOAT	D_GetMinHeight			();

		//////////////////////////////////////////////////////////
		// Inline Functions..
		//////////////////////////////////////////////////////////

		inline BOOL	CreateDetailData		( INT32 nDepth , FLOAT fDefaultHeight = 1.0f	)	{ return CreateDetailData( GetCurrentDetail() , nDepth , fDefaultHeight );	}
		inline BOOL	GenerateRoughMap		( INT32 sourcedetail , FLOAT offset				)	{ return GenerateRoughMap( GetCurrentDetail() , sourcedetail , offset ); }

		inline BOOL	RemoveDetailData		() { return RemoveDetailData( GetCurrentDetail() ); }

		inline INT32	D_GetDepth				() { return D_GetDepth( GetCurrentDetail() ); }
		inline FLOAT	D_GetStepSize			() { return D_GetStepSize( GetCurrentDetail() ); }

		// 섹터 편지 펑션들..
		inline ApDetailSegment * D_GetSegment	( INT32 x , INT32 z ,	FLOAT *pfPosX = NULL,	FLOAT *pfPosZ = NULL	) { return D_GetSegment( GetCurrentDetail() , x , z , pfPosX , pfPosZ ); }
		inline ApDetailSegment * D_GetSegment	( FLOAT x , FLOAT z ,	INT32 *pPosX = NULL ,	INT32 *pPosZ = NULL		) { return D_GetSegment( GetCurrentDetail() , x , z , pPosX , pPosZ); }

		inline FLOAT	D_GetHeight				( FLOAT x , FLOAT z ) { return D_GetHeight		( GetCurrentDetail() , x , z ); }
		inline ApRGBA	D_GetValue				( FLOAT x , FLOAT z ) { return D_GetValue		( GetCurrentDetail() , x , z ); }
		inline FLOAT	D_GetRoughHeight		( INT32 x , INT32 z ) { return D_GetRoughHeight	( GetCurrentDetail() , x , z ); }

		inline FLOAT	D_SetHeight				( INT32 x , INT32 z , FLOAT height	) { return D_SetHeight( GetCurrentDetail() , x , z , height ); }
		inline ApRGBA	D_SetValue				( INT32 x , INT32 z , ApRGBA value	) { return D_SetValue( GetCurrentDetail() , x , z , value ); }
		inline INT32	D_SetAllTile			( INT32 tileindex					) { return D_SetAllTile( GetCurrentDetail() , tileindex ); }

	// 컨스트럭터/디스트럭터. 인생에 도움이 안됨.
	ApWorldSector();
	virtual ~ApWorldSector();

// Inline Functions...

	static ApmMap * m_pModuleMap;
};

// 리스트 내에서 사용할 비교 펑션..
bool operator==( ApWorldSector::Dimension &lParam , INT32 nCreatedIndex );

inline int	GetArrayIndexXDWORD	( int index	) { return ( index >> 16		);	}
inline int	GetArrayIndexZDWORD	( int index	) { return ( index &0x0000ffff	);	}

inline FLOAT GET_REAL_HEIGHT_FROM_COMPACT_HEIGHT( UINT16 compactheight )
{
	return ( ALEF_MAX_HEIGHT - ALEF_MIN_HEIGHT ) * ( FLOAT ) compactheight / 65536.0f + ALEF_MIN_HEIGHT;
}

//#define DEBUG_NEW
//#endif

#define	ALEF_WORLD_COMPACT_SECTOR_FILE_NAME_FORMAT		"a%06dx.ma1"
#define	ALEF_WORLD_COMPACT_SECTOR_FILE_WILDCARD_FORMAT	"a*.ma1"
#define COMPACT_SERVER_FORMAT	"c%04d.dat"
#define COMPACT_SERVER_FORMAT_WILDCARD					"server\\c*.dat"

#endif // !defined(AFX_APWORLDSECTOR_H__FA496200_24F6_4F97_8D80_4AAA2462BEA1__INCLUDED_)
