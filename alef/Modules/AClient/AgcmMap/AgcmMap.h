// AgcmMap.h: interface for the AgcmMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMMAP_H__91149629_2429_47C8_9C28_5ECF4DCC6645__INCLUDED_)
#define AFX_AGCMMAP_H__91149629_2429_47C8_9C28_5ECF4DCC6645__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ApBase.h>
#include <rwcore.h>
#include <rpworld.h>

#include <ApmMap.h>
//#include <ApmObject.h>
//#include <AgcmObject.h>

#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"
#include "AcTextureList.h"
#include "AcMaterialList.h"
#include "AcMapLoader.h"
#include "AgcmResourceLoader.h"

#include "AgcmRender.h"
#include "MagImAtomic.h"

#include "AgcmOcTree.h"
#include "ApmOcTree.h"

// 맵 클라이언트 모듈로서
// 특별히 하는일은 케릭터 기준점을 입력 받고
// 해당 기준점을 중심으로 맵을 로딩하고
// 사용하지 않는 맵은 메모리에서 제거한다.

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmMapD" )
#else
#pragma comment ( lib , "AgcmMap" )
#endif
#endif


#define	OBJECT_FILE_FORMAT				"%d,%d.aof"
#define	ALEF_SECTOR_DUMMY_DEPTH			1
#define	SECTOR_DFF_FILE_FORMAT_ROUGH	"R%d,%d.dff"
#define	SECTOR_ROUGH_TEXTURE			"R%d,%d"
#define	SECTOR_DFF_FILE_FORMAT_DETAIL	"D%d,%d.dff"
#define	SECTOR_TEMP_DIRECTORY			"$$TEMP$$"

typedef enum
{
	AGCM_CB_ID_ONLOAD_MAP		= 0	,
	AGCM_CB_ID_ONUNLOAD_MAP			,
	AGCM_CB_ID_MAP_LOAD_END			,	// Map Load End Callback : 조건부로 EnumCallback 이 불린다 ( AgcmMapMapLoadEndCBMode 참고 )
	AGCM_CB_ID_MAP_SECTOR_CHANGE_PRE,	// Sector Change가 되기 전에.
	AGCM_CB_ID_MAP_SECTOR_CHANGE	,	// Sector Change가 된 후에. ObjectShadow용 sector전환시에 재계산을 위해....(gemani)
	AGCM_CB_ID_ONGETHEIGHT			,	// 높이 얻어내기 콜백. 인자는..stCollisionAtomicInfo * 
	AGCM_CB_ID_SETAUTOLOADCHANGE		// 오토로딩 거리 변경시 호출되는 콜백.
};

// Map Load End Callback 에 사용될 조건 Mode
enum	AgcmMapMapLoadEndCBMode
{
	AGCMMAP_MAPLOADEND_CB_MODE_OFF	=	0	,		// 이상태에서는 Map Load End Callback 이 작동하지 않는다 
	AGCMMAP_MAPLOADEND_CB_MODE_ON			,		// On으로 바뀌면 MyCharacterPositionChange()함수를 지나기를 기다린다  
	AGCMMAP_MAPLOADEND_CB_MODE_LOAD_START			// MyCharacterPositionCharacter() 함수를 지나면 Load Start가 되고 이 상태에서 m_listSectorQueue == 0 이면 EnumCallback!
};

// DWSector
	#define rwVENDORID_NHN			(0xfffff0L)
	#define rwID_DWSECTOR_NHN		(0x03)
	#define rwID_DWSECTOR_DATA_NHN	(0x04)

	#define rwID_DWSECTOR			MAKECHUNKID(rwVENDORID_NHN, rwID_DWSECTOR_NHN)
	#define rwID_DWSECTOR_DATA		MAKECHUNKID(rwVENDORID_NHN, rwID_DWSECTOR_DATA_NHN)

	typedef enum RpDWSectorFields
	{
		rpDWSECTOR_FIELD_NONE			= 0x0000,
		rpDWSECTOR_FIELD_VERTICES		= 0x0100,
		rpDWSECTOR_FIELD_POLYGONS		= 0x0200,
		rpDWSECTOR_FIELD_NORMALS		= 0x0400,
		rpDWSECTOR_FIELD_PRELITLUM		= 0x0800,
		rpDWSECTOR_FIELD_FLAG_TEXCOORDS	= 0x00ff
	} RpDWSectorFields;

	typedef struct RpDWSector RpDWSector;
	struct RpDWSector
	{
		RpGeometry			*	geometry	;			// terrain geometry
		RpAtomic			*	atomic		;			// terrain atomic
		RwTexDictionary		*	texdict		;			// texture dictionary
	};
	
enum RoughPolygonType
{
	R_LEFTTOP		,
	R_TOP			,
	R_RIGHTTOP		,
	R_LEFT			,
	R_CENTER		,
	R_RIGHT			,
	R_LEFTBOTTOM	,
	R_BOTTOM		,
	R_RIGHTBOTTOM	,

	R_TYPETOTAL
};	

#include "DWSector.h"
	
struct	rsDWSectorInfo
{
	RpDWSector				*pDWSector		;
	int						nDetail			;	// Current Detail
	RpMorphTarget			*pMorphTarget	;
	RpTriangle				*polygons		;
	RwV3d					*vertices		;
	RwV3d					*normals		;
	RwTexCoords				*texCoords[rwMAXTEXTURECOORDS]	;
	RwRGBA					*preLitLum		;

	int						numPolygons		;
	int						numVertices		;
};

struct	stFindMapInfo
{
	BOOL			bSuccess				;
	ApDetailSegment	*pSegment				;	// 세그먼트 포인터.
	ApWorldSector	*pSector				;	// 월드섹터.
	int				nSegmentX				;
	int				nSegmentZ				;
	
	BOOL			bEdgeTurn				;
	int				nFirstVertexIndex		;
	int				nPolygonIndexFirst		;	// 폴리건 처음 인덱스..

	// 알파 블랜딩 블럭 선택..
	BOOL			bSelected				;

	int				nCurrentDetail			;

	void	Empty()
	{
		bSuccess				= FALSE	;
		nPolygonIndexFirst		= 0		;
		pSector					= NULL	;
		pSegment				= NULL	;
		nSegmentX				= 0		;
		nSegmentZ				= 0		;
		bEdgeTurn				= FALSE	;
		nFirstVertexIndex		= 0		;
		bSelected				= FALSE	;
		nCurrentDetail			= SECTOR_EMPTY;
	}
};

struct	stCollisionAtomicInfo
{
	FLOAT			fX						;
	FLOAT			fZ						;
	FLOAT			fHeight					;	// 맵모듈에서 얻어낸 높이
	FLOAT			fCharacterHeight		;	// 케릭터 높이
	INT32			nObject					;	// 오브제트 카운트
	INT32 *			aObject					;
	UINT8			uRidableType			;	// 올라간 오브젝트 인덱스..

	stCollisionAtomicInfo():nObject( 0 ),aObject( NULL ),uRidableType( APMMAP_MATERIAL_SOIL ){}
};

class AgcmMap : public AgcModule  
{
public:
	DECLARE_SINGLETON( AgcmMap )

	// 내부적으로만 사용함..
	// 맵모듈 포인터..
	ApmMap *		m_pcsApmMap					;
	AgcmRender *	m_pcsAgcmRender				;
	AgcmResourceLoader *	m_pcsAgcmResourceLoader	;
	AgcmOcTree*		m_pcsAgcmOcTree				;
	ApmOcTree*		m_pcsApmOcTree				;

	INT32					m_lLoaderChangeID		;
	INT32					m_lLoaderDestroyID		;

	// 섹터로드/디스트로이 콜백..
	void		( *	m_pfSectorWaterLoadCallback		)( ApWorldSector * pSector , int nDetail	);
	void		( *	m_pfSectorWaterDestroyCallback	)( ApWorldSector * pSector , int nDetail	);
	RpAtomic *	( *	m_pfGetCollisionAtomicCallback	)( RpClump * pClump , PVOID pvData			);
	RpAtomic *	( *	m_pfGetCollisionAtomicCallbackFromAtomic	)( RpAtomic * pClump , PVOID pvData			);

	void		( * m_pfOnFirstMyCharacterPositionCallback	)();

	inline	void	SetCollisionAtomicCallback	( RpAtomic * ( * pCallback	)( RpClump * pClump , PVOID pvData ) )
	{	m_pfGetCollisionAtomicCallback	= pCallback;	}
	inline	void	SetCollisionAtomicCallbackFromAtomic	( RpAtomic * ( * pCallback	)( RpAtomic * pAtomic , PVOID pvData ) )
	{	m_pfGetCollisionAtomicCallbackFromAtomic	= pCallback;	}
	inline	void	SetSectorWaterCallback		( void	( *	pLoadCallback )( ApWorldSector * pSector , int nDetail ) , void ( * pDestroyCallback )( ApWorldSector * pSector , int nDetail ) )
	{ m_pfSectorWaterLoadCallback = pLoadCallback; m_pfSectorWaterDestroyCallback = pDestroyCallback; }
	inline	void	SetFirstMapLoadCallback		( void	( *	pCallback )() )
	{ m_pfOnFirstMyCharacterPositionCallback = pCallback; }

	// 렌더웨어 섹터 포인터..
	struct	AgcmMapSectorData
	{
		DWSectorData	m_DWSector		;
		AcMaterialList	m_MaterialList[ SECTOR_DETAILDEPTH	];

		#ifdef USE_MFC
		BOOL			m_bValidGrid;
		RwIm3DVertex	m_pImVertex[ 70 ];	// 테두리 버텍스 정보.

		void *			m_pLoadedData1; // Rough 정보
		void *			m_pLoadedData2;	// Detail 정보
		#endif // USE_MFC
	};

	AcTextureList	m_TextureList				;

	BOOL			m_bInited					;
	AuPOS			m_posCurrentAvatarPosition	;	// 마지막에 움직인 케릭터 위치..
	BOOL			m_bAvatarPositionSet		;	// Avatar Position이 Setting되었는가 (외부에서 MyCharacterChangePosition 을 불렀는가?)

	char			m_strToolImagePath[ 256 ]	;	// 툴에서 사용하는 이미지가 들어있는 디렉토리 위치.

	BOOL			m_bFirstPosition			;
	void			DeleteMyCharacterCheck()	;

	AcMapLoader		m_csMapLoader				;	// Map Loading Thread

	RwCamera *		m_pstCamera					;
	UINT32			m_ulPrevLoadTime			;

	BOOL			m_bUseTexDict				;

	CHAR			m_szTexturePath[ 128 ]		;	// Texture Path를 지정해준다. ResourceLoader에 추가된 기능 (Parn)

	/************************ 2003_11_14 98pastel : Map Load End CB 관련 ******************************/
	INT32			m_lMapLoadEndCBMode			;	// Map Load End CB Mode : AgcmMapMapLoadEndCBMode를 참고 
	void			SetMapLoadEndCBActive() { m_lMapLoadEndCBMode = AGCMMAP_MAPLOADEND_CB_MODE_ON; }
	BOOL			SetCallbackMapLoadEnd(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) 
						{ return SetCallback(AGCM_CB_ID_MAP_LOAD_END, pfnCallback, pvClass); }
	/************************ 2003_11_14 98pastel : Map Load End CB 관련 ******************************/
	

	INT32			EnumLoadedDivision		( DivisionCallback pCallback , PVOID pData );
	// 리턴은 디비전 카운트..

	static const int	EnumDivisionExpandValue = 2;
	INT32			EnumLoadedDivisionObject( DivisionCallback pCallback , PVOID pData );
	// 여분의 오브젝트를 로딩하기 위해 상하좌우 디비젼을 추가로 Enum한다.

protected:
	RtWorldImport *	CreateWorldImport()			;

	BOOL			m_bInitialized				;
	void			SetInitialize(	BOOL bInit = TRUE ){ m_bInitialized = bInit; }
	BOOL			IsInitialized(){ if ( m_bInitialized ) return TRUE ; else return FALSE; }

	INT16				m_nSectorAttachedDataOffset	;

	// Load Range
	BOOL			m_bRangeApplied	;
	INT32			m_nLoadRangeX1	;
	INT32			m_nLoadRangeZ1	;
	INT32			m_nLoadRangeX2	;
	INT32			m_nLoadRangeZ2	;

	RwInt32			m_nGeometryLockMode		;
	RwInt32			m_nVertexColorLockMode	;

	RpWorld		*	m_pWorld				;

	// 로딩 레인지 설정..
	INT32			m_nMapDataLoadRadius	;
	INT32			m_nMapDataReleaseRadius	;
	INT32			m_nRoughLoadRadius		;	// 맵상거리..러프맵이 로딩돼는 범위.
	INT32			m_nDetailLoadRadius		;	// 맵상거리..디테일 맵이 로딩돼는 범위.
	INT32			m_nRoughReleaseRadius	;	// 맵상거리..러프맵이 메모리에서 제거돼는 범위
	INT32			m_nDetailReleaseRadius	;	// 맵상거리..디테일맵이 메모리에서 해제돼는 범위.

	INT32			m_lSectorsToLoad		;	// Load해야할 Sector의 개수
					
public:
	AgcmMapSectorData * GetAgcmAttachedData( ApWorldSector * pSector )
	{
		return ( AgcmMapSectorData * )m_pcsApmMap->GetAttachedModuleData( m_nSectorAttachedDataOffset , ( PVOID ) pSector );
	}

	inline INT32	GetMapDataLoadRadius	()	{ return m_nMapDataLoadRadius	; }
	inline INT32	GetRoughLoadRadius		()	{ return m_nRoughLoadRadius		; }
	inline INT32	GetDetailLoadRadius		()	{ return m_nDetailLoadRadius	; }
	inline INT32	GetMapDataReleaseRadius	()	{ return m_nMapDataReleaseRadius; }
	inline INT32	GetRoughReleaseRadius	()	{ return m_nRoughReleaseRadius	; }
	inline INT32	GetDetailReleaseRadius	()	{ return m_nDetailReleaseRadius	; }

	inline RwInt32	GetGeometryLockMode		(					) { return m_nGeometryLockMode		;				}
	inline RwInt32	SetGeometryLockMode		( RwInt32 nLockMode	) { return m_nGeometryLockMode		= nLockMode;	}
	inline RwInt32	GetVertexColorLockMode	(					) { return m_nVertexColorLockMode	;				}
	inline RwInt32	SetVertexColorLockMode	( RwInt32 nLockMode	) { return m_nVertexColorLockMode	= nLockMode;	}

	// 메시지 헨들러.
	BOOL	OnAddModule	(						)	;
	BOOL	OnInit		(						)	;
	BOOL	OnDestroy	(						)	;
	BOOL	OnIdle		( UINT32 ulClockCount	)	;

	virtual	BOOL	OnLoadSector	( ApWorldSector * pSector );	// 섹터가 로딩돼기 전!
	virtual	BOOL	OnDestorySector	( ApWorldSector * pSector );	// 섹터가 없어지기 전!

	// 맵 클라이언트 모듈 이니트..
	// 이 안에서 페얼런트를 이니셜라이즈 하므로 , 따로 페어런트 펑션은 호출하지 않도록 한다.
	// - 보시다 시피 init는 , 월드( 러프로 만들어진것.. ) 를 로딩한 다음에 호출돼어야함.
	// - 월드 섹터 데이타에 RpWorldSector * 데이타를 추가함.
	// - 현재 월드를 이누머레이팅해서 , 포인터 정리.. <- 방법이 묘연함 , 아마도 월드 섹터 포인터만 따로 가지고 있어야할듯함.
	// 호출돼는시점 -> 월드가 생성됀 직후.

	virtual	BOOL	OnLoadRough		( ApWorldSector * pSector );	// 러프 로딩을 요청 하기전.
	virtual	BOOL	OnLoadDetail	( ApWorldSector * pSector );	// 디테일 로딩을 요청하기전. 
	virtual	BOOL	OnDestroyRough	( ApWorldSector * pSector );	// 러프 로딩을 요청 하기전.
	virtual	BOOL	OnDestroyDetail	( ApWorldSector * pSector );	// 디테일 로딩을 요청하기전. 

	BOOL	SetSectorIndexInsert( int indexType , ApWorldSector * pSector , INT32 nObjectIndex , RwSphere * pSphere );
	BOOL	SetSectorIndexInsertBBox( int indexType , ApWorldSector * pSector , INT32 nObjectIndex , RwBBox * pBBox );
	// 주변에까지 인덱스가 삽입된경우 , 리턴 츠루

	VOID	SetTexturePath		( CHAR *szTexturePath				);

	// Texture Dictionary 이름 결정
	BOOL	SetTexDict			( char *pTexDictFileName			);
	BOOL	SetCamera			( RwCamera * pCamera				);
	BOOL	SetDataPath			( char * pInitToolImagePath			);
	BOOL	InitMapClientModule	( RpWorld * pWorld					);
	BOOL	InitMapToolData		( char *pTextureScriptFileName , char *pObjectScripFileName = NULL	);
	void	InitMapToolLoadRange( int x1 , int z1 , int x2 , int z2									);

	BOOL	SetAutoLoadRange		( INT32 nMapdata , INT32 nRough , INT32 nDetail );
	BOOL	__SetAutoLoadRange		( INT32 nMapdata , INT32 nRough , INT32 nDetail , INT32 nViewOption = -1 );
			// 섹터의 거리를 생각해서 반지름(섹터단위..)을 지정해준다.
			// nViewOption은 옵션창에서 설정한 거리 레벨..

	// 전체 로딩..

	// 마고자 (2004-04-30 오전 11:45:56) : 
	// 아래것과 거의 같은 함수인데..
	// 로긴할때 미리 지형을 읽어두기 위해 사용함.
	BOOL	LoadTargetPosition	( FLOAT fX , FLOAT fZ , ProgressCallback pfCallback = NULL , void * pData = NULL );

	// 마고자 (2004-04-29 오후 5:23:41) : 
	// fX , fZ 점을 기준으로 섹터를 얻은 후 , 그쎅터에서 -nRange , + nRange 만큼 읽어 들인다.
	// 즉 , nRange 가 0 이면 해당 섹터만 로딩함.
	// 로딩이 끝나면 리턴 , 필요할경우 Progress 콜벡을 사용하면 좋다고 생각함.
	BOOL	LoadAll			( FLOAT	fX , FLOAT fZ , INT32 nRange , ProgressCallback pfCallback = NULL , void * pData = NULL );

	// 마고자 (2004-07-02 오후 12:44:14) : bCreate는 생성할건지 로딩할건지 체크..
	BOOL	LoadAll			( BOOL bCreate , ProgressCallback pfCallback = NULL , void * pData = NULL );
	BOOL	SaveAll			( BOOL bGeometry = TRUE , BOOL bTile = TRUE , BOOL bCompact = TRUE	, ProgressCallback pfCallback = NULL , void * pData = NULL	, char * pBackupDirectory = NULL );
	BOOL	SaveAll			( char * pTarget , BOOL bGeometry = TRUE , BOOL bTile = TRUE , BOOL bCompact = TRUE	, ProgressCallback pfCallback = NULL , void * pData = NULL	);
							// pTarget 은 "c:\\maptool\\subdata" 이런식으로 돼어야함.

	BOOL	SaveGeometryBlocking( INT32 nDivisionIndex , char * pFileName );
	BOOL	LoadGeometryBlocking( INT32 nDivisionIndex , char * pFileName );
	BOOL	LoadSkyBlocking		( INT32 nDivisionIndex , char * pFileName );

	// 툴용 펑션..
	BOOL	CreateAllCollisionAtomic	( ProgressCallback pfCallback = NULL , void * pData = NULL );
	BOOL	DestroyAllCollisionAtomic	( ProgressCallback pfCallback = NULL , void * pData = NULL );

	// 외부에서 사용될 자동 함수들
	// 자동 로드 온오프시 처리함.. ApWorldSector 는 무조건 널이어야한다.
	//ApWorldSector *	LoadSector		( ApWorldSector * pSector											);
	ApWorldSector *	LoadSector		( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	);
	BOOL			ReleaseSector	( ApWorldSector * pSector											);
	BOOL			ReleaseSector	( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	);

	void			FlushLoadSector	( ApWorldSector * pNextSector );

	// 익스포트 함수

	BOOL			BspExport		( BOOL bDetail , BOOL bRough , BOOL bCompact , BOOL bServer ,
									char * pDestinationDirectory = NULL ,
									ProgressCallback pfCallback = NULL , void * pData = NULL );

	inline	BOOL	IsRangeApplied() { return m_bRangeApplied	;	}
	inline	INT32	GetLoadRangeX1() { return m_nLoadRangeX1	;	}
	inline	INT32	GetLoadRangeZ1() { return m_nLoadRangeZ1	;	}
	inline	INT32	GetLoadRangeX2() { return m_nLoadRangeX2	;	}
	inline	INT32	GetLoadRangeZ2() { return m_nLoadRangeZ2	;	}

	BOOL	IsInited			(){ return m_bInited; }

	void	MyCharacterPositionChange( AuPOS prevpos , AuPOS pos , BOOL bForceReload = FALSE );
		// 호출함..
		// My Character 가 이동할땐 케릭터 모듈에서 항상 호출해주어야 한다.
		// 이 함수 안에서 지형 로딩에 대한 루틴을 구한한다.

	static BOOL	IsInSectorRadius		( ApWorldSector * pSector , float x , float z , float radius );


	//////////////////////////////////////////////////////
	// 섹터 폴리건 정보 생성 관련..
	// 월드와 연결
	RpWorld	*		GetWorld		( void );

	FLOAT			GetHeight		( FLOAT x , FLOAT z , FLOAT y );//= SECTOR_MAX_HEIGHT );
	inline FLOAT	GetHeight		( AuPOS pos ) { return GetHeight( pos.x , pos.z , pos.y ); }
		// GetHeight는 , x,y값을 기준으로 타일 사이의 계산적인 위치값을 구해준다.
		// 높이 자체는 HP_GetHeight 로 얻어내서 , 각 포인트당 기울기를 구하여
		// 이론적인 높이를 구해낸다.
		// 일반적으론 이 함수를 쓰는것이 가장 옳다.

	FLOAT			GetHeight_Lowlevel				( FLOAT x , FLOAT z , FLOAT y = SECTOR_MAX_HEIGHT  , BOOL * pValidCash = NULL );
	FLOAT			GetHeight_Lowlevel_HeightOnly	( FLOAT x , FLOAT z , BOOL * pValidCash = NULL );				// 지형 높이만을 구함.

	FLOAT			GetHeight_Lowlevel_Collision	( FLOAT x , FLOAT z , FLOAT y = SECTOR_MAX_HEIGHT  , BOOL * pbRiddable = NULL );

		// HeightPool을 거치지 않고 , 직접 높이값을 구해낸다,
		// 지형 콜리전이 이용돼며 속도는 느린편이니 남용하지 말것.

	// Height Pool , 높이정보 버퍼..
	FLOAT			HP_GetHeight	( ApWorldSector * pSector , int x , int z , FLOAT fY = SECTOR_MAX_HEIGHT );
	FLOAT			HP_GetHeight	( FLOAT fX , FLOAT fZ , FLOAT fY = SECTOR_MAX_HEIGHT );

	FLOAT			HP_GetHeightGeometryOnly	( ApWorldSector * pSector , int x , int z , FLOAT fY = SECTOR_MAX_HEIGHT);
	FLOAT			HP_GetHeightGeometryOnly	( FLOAT fX , FLOAT fZ , FLOAT fY = SECTOR_MAX_HEIGHT );

	void			RemoveHPInfo	();
		// HP_GetHeight 는 , 높이정보를 캐싱해서 처리하는 함수다.
		// 즉 , 한번도 읽은적이 없는 위치라면 GetHeight_Lowlevel를 호출해서 정보를 만들고
		// 이전에 로딩한 적이 있으면 그 값을 리턴한다.

	enum
	{
		PICKINGOBJECTCLUMP				,
		PICKINGOBJECTCOLLISIONATOMIC	,
		PICKINGCHARACTER				
	};
	RpClump *		GetCursorClump	( RwCamera * pCamera , RwV2d * pScreenPos , INT32 nOption = PICKINGOBJECTCOLLISIONATOMIC );

	BOOL	GetMapPositionFromMousePosition( INT32 x , INT32 y , FLOAT *px = NULL , FLOAT *py = NULL , FLOAT *pz = NULL , INT32 *pnPolygonIndex = NULL , FLOAT * pFoundDistancRatio = NULL , FLOAT * pFoundDistance = NULL );
		// pFoundDistancRatio = 콜리젼 거리의 비율
		// pFoundDistance = 실제 카메라와의 거리.
	ApWorldSector *	GetMapPositionFromMousePosition_tool( INT32 x , INT32 y , FLOAT *px = NULL , FLOAT *py = NULL , FLOAT *pz = NULL , INT32 *pnPolygonIndex = NULL , FLOAT * pFoundDistance = NULL );
	
	// 안쓰이는것 같아 주석처리 by gemani
	//BOOL			CheckCollision( RwV3d *pos1, RwV3d *pos2 );			//두 벡터로 이루어진 Line과 지형과의 Collision여부를 Return - AgcmCamera에서 사용
		// 리턴이 NULL 일 경우 에러처리.
		// 그외의 경우는 참임. 그리고 해당 섹터의 포인터..를 리턴함.

	void				MakeDetailWorld			( ApWorldSector * pSector , int nDetail = SECTOR_HIGHDETAIL	);	// 해당섹터에 디테일 맵 정보를 생성한다.
	RtWorldImport	*	__MakeDetailWorldImport	( ApWorldSector * pSector , int nDetail = SECTOR_HIGHDETAIL	);
	RpAtomic		*	MakeDetailWorldAtomic	( ApWorldSector * pSector , int nDetail = SECTOR_HIGHDETAIL	);
	// 지형 데이타 수정..
	void		Update				( ApWorldSector * pSector , int detail = -1					);

	void		RecalcNormal		( ApWorldSector * pSector , INT32 nDetail , BOOL bReculsive = TRUE );
	// 마고자 (2004-06-28 오후 3:55:48) : 리컬시브 플래그 붙임.. 옆에 섹터도 같이 업데이트 됨..
	// 디테일에서만 의미 있음..
	
	void		CopyDetailNormalToRoughNormal
									( ApWorldSector * pSector									);
	void		RecalcBoundingBox	( ApWorldSector * pSector									);
	// 디테일 지형의 경계 맞춰주는 함수..

	//RpWorldSector	* GetWorldSector	( ApWorldSector * pSector );
	//RpWorldSector	* GetWorldSector	( INT32 arrayindexX	, INT32 arrayindexY	, INT32 arrayindexZ	);

	inline AcMaterialList	* GetMaterialList	( ApWorldSector * pSector )
	{
		AgcmMapSectorData	* pAgcmSectorData = GetAgcmAttachedData( pSector );
		if( pAgcmSectorData )	return pAgcmSectorData->m_MaterialList	;
		else					return NULL								;
	}

	//ApWorldSector *		GetSector		( RpWorldSector	* pWorldSector	);

	BOOL		GetWorldSectorInfo		( stFindMapInfo * pMapInfo , ApWorldSector * pSector , int x , int z , int detailedX = -1 , int detailedZ = -1 );
	BOOL		GetWorldSectorInfoOnly	( stFindMapInfo * pMapInfo , ApWorldSector * pSector , int x , int z , int detailedX = -1 , int detailedZ = -1 );
	void		SectorPreviwTextureSetup( stFindMapInfo * pMapInfo , int tileindex  );


	// 전체 적용..
	void		UpdateAll			(						);

	void		RecalcBoundingBox	(						);
	void		RecalcNormals		( BOOL bDetail = FALSE	, ProgressCallback pfCallback = NULL , void * pData = NULL );
	void		MakeDetailWorld		( ProgressCallback pfCallback = NULL , void * pData = NULL );	// 해당섹터에 디테일 맵 정보를 생성한다.

	// 기타 함수들..
	BOOL		GenerateRoughMap	( INT32 nTargetDetail , INT32 nSourceDetail , int alphatexture , FLOAT offset	,
									BOOL bHeight = TRUE , BOOL bTile = FALSE , BOOL bVertexColor = FALSE , BOOL bApplyAlpha = FALSE );

	void		UpdateNormal2		( ApWorldSector * pSector , float fx , float fz );
	
	BOOL		ApplyAlpha			( stFindMapInfo *pMapInfo, int basetile , int maskedindex , int alphaindex , BOOL bReculsive = FALSE , BOOL bFirst = TRUE);
	BOOL		ApplyAlphaToSector	( ApWorldSector * pSector , int alphatexture , int nDetail = -1 );

	// 지형 에디팅 함수들..
	// 포릴건 정보도 같이 변경함..

	BOOL		D_SetHeight		( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z , FLOAT height , BOOL bReculsive = TRUE , BOOL bUpdateNormal = TRUE	);
	BOOL		D_SetValue		( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z , ApRGBA value	);
	BOOL		D_SetTile		( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z ,
									UINT32 firsttexture ,
									UINT32 secondtexture	= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 thirdtexture		= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 fourthtexture	= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 fifthtexture		= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 sixthtexture		= ALEF_TEXTURE_NO_TEXTURE	);
	BOOL		D_SetEdgeTurn	( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z , BOOL bEdge	);
	BOOL		D_GetEdgeTurn	( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z );
	BOOL		D_GetEdgeTurn	( FLOAT fX , FLOAT fZ );

	// 마고자 (2005-04-25 오후 5:39:44) : 
	// 클라이언트용 땅 타입 읽어냄..
	UINT8		GetType			( FLOAT fX , FLOAT fY , FLOAT fZ );
	BOOL		GetOnObject		( FLOAT fX , FLOAT fY , FLOAT fZ );

	// DWSector 관련...
	BOOL					IsPolygonDataLoaded	( ApWorldSector * pSector );
	inline DWSectorData	*	_GetDWSectorData	( ApWorldSector * pSector )
	{
		AgcmMapSectorData	* pAgcmSectorData = GetAgcmAttachedData( pSector );
		if( pAgcmSectorData )	return &pAgcmSectorData->m_DWSector	;
		else					return NULL							;
	}

		// 맵툴..
		RpDWSector *	CreateDWSector		( ApWorldSector * pSector ,	int detail , int nov , int not , int tex = 4 );
		void			DestroyDWSector		( ApWorldSector * pSector ,	int detail );

		// 클라이언트..
		BOOL			LoadDWSector		( ApWorldSector * pSector				);
		BOOL			LoadDWSector		( ApWorldSector * pSector , int nDetail	);
		BOOL			ReleaseDWSector		( ApWorldSector * pSector				);

		RwTexture *		LoadRoughTexture	( RwStream *stream						);

		RpAtomic *		LoadRoughAtomic		( INT32 wx , INT32 wz , FLOAT fSrcX , FLOAT fSrcZ , FLOAT fHeightOffset , FLOAT fDstX , FLOAT fDstZ );


	rsDWSectorInfo *	GetDWSector			( ApWorldSector * pSector ,	int detail ,  rsDWSectorInfo * pDWSectorInfo );
	rsDWSectorInfo *	GetDWSector			( ApWorldSector * pSector ,	rsDWSectorInfo * pDWSectorInfo ) { return GetDWSector( pSector , GetCurrentDetail( pSector ) , pDWSectorInfo ); }

	// 디테일 변경..
	void				SetCurrentDetail	( int detail , BOOL bForce = FALSE	);
	void				SetCurrentDetail	( ApWorldSector * pSector , int detail , BOOL bForce = FALSE );
	inline void			SetCurrentDetail	( INT32 arrayindexX	, INT32 arrayindexY , INT32 arrayindexZ , int detail	){ SetCurrentDetail( m_pcsApmMap->GetSectorByArray( arrayindexX , arrayindexY , arrayindexZ ) , detail ); }
	int					GetCurrentDetail	( ApWorldSector * pSector	);
	inline int			GetCurrentDetail	( INT32 arrayindexX	, INT32 arrayindexY , INT32 arrayindexZ ){ return GetCurrentDetail( m_pcsApmMap->GetSectorByArray( arrayindexX , arrayindexY , arrayindexZ ) ); }

	ApWorldSector *		GetSector			( RpAtomic		* pAtomic		);	// 아토믹으로 부터.. 섹터 정보 얻어내기.
	BOOL				IsGeometry			( RpAtomic		* pAtomic	, INT32 * pIndex = NULL	);

	// 로딩 콜백..
	static	void		CallBackMapDataLoad		( ApWorldSector * pSector );
	static	void		CallBackMapDataDestroy	( ApWorldSector * pSector );

	// Texture Dictionary 관련 함수들
	BOOL					DumpTexDict();

	INT32				GetSectorsToLoad	()				{ return m_lSectorsToLoad	;	}
	VOID				SetSectorsToLoad	(INT32 lCount)	{ m_lSectorsToLoad = lCount	;	}

	void	RenderSectorGrid	( ApWorldSector * pSector );
	BOOL	AllocSectorGridInfo	( ApWorldSector * pSector );

	virtual ~AgcmMap();

public:
	// DWSector 관련 평션들..
	
	/*
	 * Initialize DWSector Object
	 * dwSector : DWSector must be fetched by RpDWSectorGetDetail()
	 * numVertices : Number of vertices
	 * numPolygons : Number of Polygons
	 * numTexCoords : Number of Texture Coordinates
	 */
	RpDWSector *		RpDWSectorInit				( RpDWSector *dwSector, RwUInt32 numVertices, RwUInt32 numPolygons, RwUInt8 numTexCoords);
	/* Get detail object (0 base)*/
	RpDWSector *		RpDWSectorGetDetail			( ApWorldSector * pWorldSector ,  RwUInt8 numDetail );
	INT32				RpDWSectorGetCurrentDetail	( ApWorldSector * pWorldSector );
	/* Set current detail index (0 base, one sector) */
	RpAtomic *			RpDWSectorSetCurrentDetail	( ApWorldSector * pWorldSector ,  RwUInt8 numDetail , BOOL bForce = FALSE );
	/* Build world sector collision data */
	ApWorldSector *		RpDWSectorBuildAtomicCollision( ApWorldSector *pWorldSector );
	ApWorldSector *		RpDWSectorDestroyDetail		( ApWorldSector * pWorldSector , RwUInt8 numDetail	);
	ApWorldSector *		RpDWSectorDestroyDetail		( ApWorldSector * pWorldSector);

	BOOL				RpDWSectorDestroy			( RpDWSector *dwSector	);

	BOOL				IsLoadedDetail				( ApWorldSector * pWorldSector , INT32 nDetail );

	BOOL				CreateImAtomic				( MagImAtomic * pAtomic , ApWorldSector * pWorldSector );
	RpAtomic *			CreateCollisionAtomic		( ApWorldSector * pWorldSector , INT32 nDetail = SECTOR_HIGHDETAIL );

	BOOL				SetPreLightForAllGeometry	( FLOAT fValue , ProgressCallback pfCallback = NULL , void * pData = NULL );
	// 0.0f ~ 2.0f , 1.0f는 현재 상황 유지..
	// 맵데이타의 프리라잇을 통채로 바꾼다.

	//◎◎◎◎◎◎◎◎◎ Intersection 용 ◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎ add by gemani(RpWorld 안쓸때)
	ApWorldSector*		m_pSelfCharacterSector;			// 주인공이 서있는 섹터
	bool				m_bUseCullMode;
	UINT32				m_ulCurTick;

	void				SectorForAllAtomicsIntersection( ApWorldSector * pWorldSector ,   RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data );

	IntersectionSectors*		LineGetIntersectionSectors(RwLine*	line); 

	// 현재 로딩된 모든 sector를 체크한다
	// ID는 중복 Intersection체크를 방지하기 위해 부여한 번호
	// AgcmTargeting - 0 , AgcmLensFlare - 1, AgcmCamera - 2,
	// AgcmMap::GetMapPositionFromMousePosition - 3,
	// AgcmMap::GetCursorClump - 4

	// 지형 ,object,character 를 체크한다!!
	void				LoadingSectorsForAllAtomicsIntersection( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// 지형을 제외하고 world와 object 체크한다!!
	void				LoadingSectorsForAllAtomicsIntersection2( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// object만 체크한다!!(카메라에서 쓰임!)
	void				LoadingSectorsForAllAtomicsIntersection3( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// character만 체크한다!!(debug용)
	void				LoadingSectorsForAllAtomicsIntersection4( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// 지형하고만 체크한다!!(카메라에서 쓰임!)
	BOOL				LoadingSectorsForAllAtomicsIntersection5(RpIntersection *    intersection, 
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// 마고자 (2003-11-20 오후 3:17:52) : 클럼프 체크함수 추가.
	void				LoadingSectorsForAllClumps(		INT32				ID			,	// 아직 사용 안하나봄.. 현재 0 넣고 쓰는중..
												 		RpClumpCallBack		pCallBack	,
														void *				pData		,
														BOOL				bLock = TRUE );
	SectorRenderList *	GetSectorData(ApWorldSector *pSector)	
			{ return (SectorRenderList*) m_pcsApmMap->GetAttachedModuleData(m_pcsAgcmRender->m_iSectorDataIndex,(PVOID)pSector);}

	// gemani(2004-4-7) : 클럼프 체크함수(옥트리용) 추가.
	void				LoadingSectorsForAllClumps_OCTREE(INT32				ID			,	// 아직 사용 안하나봄.. 현재 0 넣고 쓰는중..
												 		RpClumpCallBack		pCallBack	,
														void *				pData		,
														BOOL				bLock = TRUE );

protected:

	RwV3d	* m_pVertexList		;
	RwRGBA	* m_pPreLightList	;

	struct	_MakeDetailDataSet
	{
		ApWorldSector *	pSector			;

		INT32			nWidth			;
		INT32			nHeight			;
		RwV3d			*pVertexList	;
		RwRGBA			*pPreLightList	;


		INT32			nVertexIndex	;

		RpMorphTarget	*pMorphTarget	;
		RpTriangle		*pPolygons		;
		RwV3d			*pVertices		;
		RwV3d			*pNormals		;
		RwTexCoords		*pTexCoords		[rwMAXTEXTURECOORDS];
		RwRGBA			*pPreLitLum		;

		RwV3d	*		Map		( int x , int y ) { return &pVertexList	[ ( y ) * ( nWidth + 1 ) + ( x ) ]; }
		RwRGBA	*		Prelight( int x , int y ) { return &pPreLightList[ ( y ) * ( nWidth + 1 ) + ( x ) ]; }

		BOOL			bValidNormal	;
		BOOL			bValidTexCoords	;

		_MakeDetailDataSet() :
			bValidNormal	( FALSE ),
			bValidTexCoords	( FALSE )
		{
		}
	};

	BOOL	MakeRoughPolygon		( INT32 nDetail , INT32 nType , INT32 x , INT32 z , FLOAT fStepSize , _MakeDetailDataSet * pDataSet , ApDetailSegment * pSegment , INT32 nMatIndex );
public:
		// Lock Master
	struct stLockSectorInfo
	{
		ApWorldSector *	pSector		;
		INT32			nDetail		;
		RwInt32			nLockMode	;
	};
	
	AuList< stLockSectorInfo >		m_listLockedSectorList;
	BOOL IsLockedSector			( ApWorldSector * pSector	, RwInt32 nLockMode	, INT32 nDetail = -1 );
	BOOL LockSector				( ApWorldSector * pSector	, RwInt32 nLockMode	, INT32 nDetail = -1 );	// 현재 섹터를 락함.
	BOOL UnlockSectors			( BOOL bUpdateCollision = FALSE , BOOL bUpdateRough = FALSE );	// 락한 모든 섹터를 언락함

public:
	static RpCollisionTriangle *		CollisionAtomicSectorCallback(
																		RpIntersection		*	pstIntersection	,
																		RpCollisionTriangle	*	pstCollTriangle	,
																		RwReal					fDistance		, 
																		void				*	pvData			);

	static RpAtomic *	_GetClumpCallback(
								 RpIntersection *	intersection	,
								 RpWorldSector *	sector			,
								 RpAtomic *			atomic			,
								 RwReal				distance		,
								 void *				data			);
	static RpAtomic *	_GetClumpPickingOnlyCallback(
								 RpIntersection *	intersection	,
								 RpWorldSector *	sector			,
								 RpAtomic *			atomic			,
								 RwReal				distance		,
								 void *				data			);
	static RpAtomic *	_GetHeightCallback(
								 RpIntersection *	intersection	,
								 RpWorldSector *	sector			,
								 RpAtomic *			atomic			,
								 RwReal				distance		,
								 void *				data			);
	static RpAtomic *	_GetHeightClientCallback(
								 RpIntersection *	intersection	,
								 RpWorldSector *	sector			,
								 RpAtomic *			atomic			,
								 RwReal				distance		,
								 void *				data			);
	static RpCollisionTriangle * _IntersectionCallBackFindDistance
												(RpIntersection *intersection, RpCollisionTriangle *collTriangle,
												RwReal distance, void *data);

	static	BOOL	_AgcmMapSectorDataConstructor	(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	_AgcmMapSectorDataDestructor	(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	SetCallbackLoadMap			(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_ONLOAD_MAP			, pfnCallback, pvClass); }
	BOOL	SetCallbackUnLoadMap		(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_ONUNLOAD_MAP		, pfnCallback, pvClass); }
	BOOL	SetCallbackSectorChangePre	(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_MAP_SECTOR_CHANGE_PRE	, pfnCallback, pvClass); }
	BOOL	SetCallbackSectorChange		(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_MAP_SECTOR_CHANGE	, pfnCallback, pvClass); }
	BOOL	SetCallbackGetHeight		(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_ONGETHEIGHT		, pfnCallback, pvClass); }
	BOOL	SetCallbackSetAutoLoadChange(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_SETAUTOLOADCHANGE		, pfnCallback, pvClass); }
	
	static BOOL	CBMyCharacterPositionChange(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBMyCharacterPositionChange2(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBDestroySector_Start	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBDestroySector_End		(PVOID pData, PVOID pClass, PVOID pCustData);

	RtWorldImport *	__CreateWorldCollisionImport		( ApWorldSector * pWorldSector , INT32 nDetail );
	
	BOOL	CreateCompactDataFromDetailInfo	( ApWorldSector * pSector );	//	디테일 데이타에서 컴펙트 데이타를 생성함.

	static BOOL	CBLoadGeometry	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSaveGeometry	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBLoadMoonee	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSaveMoonee	(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBMTextureRender(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	LoadZoneRoughBuffer( FLOAT fSrcX , FLOAT fSrcZ , FLOAT fHeight , FLOAT fDstX , FLOAT fDstZ , INT32 nRadius = 6 );
	void	FlushZoneRoughBuffer();
	vector<RpAtomic *>	m_vectorZoneRough;	// 존로딩 배경으로쓸 아토믹 모음

	// 마고자 (2005-08-24 오후 6:02:21) : 존로딩 처리..
	INT32	m_nZoneLoadDivision;
	BOOL	IsZoneLoading()
	{
		if( 0 == m_nZoneLoadDivision )	return FALSE;
		else							return TRUE	;
	}
	BOOL	CheckSelfCharacterRegionChange( AuPOS *pPrev , AuPOS *pNext );
	BOOL	ZoneLoadingDivision( INT32 nDivision , INT16 nRegion );
	void	ClearAllSectors();

	// 마고자 (2005-08-29 오후 12:08:42) : 
	// 디버그용 펑션
	void	ShowAll();
	void	ToggleZoneRough();
	BOOL	m_bShowZoneRough;

	INT32	m_nLoadLockCount;

	// 폴리모프를 위해 섹터 날리는걸 임시로 막는 일을 함..
	INT32	LoadLock	() { return ++m_nLoadLockCount; }
	INT32	ReleaseLock	() { return --m_nLoadLockCount; }
	INT32	IsLocked	() { return m_nLoadLockCount; }

	#ifdef USE_MFC
	void	SetChangeTile( UINT32 uTiled , UINT32 uReplace )
	{
		m_uTiled	= uTiled	;
		m_uReplace	= uReplace	;
	}

	// 타일 바꿔치기용 데이타.
	// m_uTiled == 0 이면 Disable
	UINT32	m_uTiled	;
	UINT32	m_uReplace	;
	#endif // USE_MFC


	// 섹터 얻어내기 펑션 개조..
	static RpAtomic * GetBBoxCallback(RpAtomic * pAtomic, void *data);
	INT32	GetSectorList		( RwBBox *pbbox , vector< ApWorldSector * > * pVector );
	inline	INT32 GetSectorList		( RpAtomic * pAtomic , vector< ApWorldSector * > * pVector )
	{
		RwBBox bbox;
		AgcmMap::GetBBoxCallback( pAtomic , ( void * ) &bbox );
		return GetSectorList( &bbox , pVector );
	}

	// 마고자 (2005-11-23 오전 10:39:54) : 
	// 툴용 펑션..

	struct stLoadedDivisionCheck
	{
		INT32	nDivision		;
		BOOL	bInLoadedRange	;

		stLoadedDivisionCheck():bInLoadedRange(FALSE){}
	};

	static	BOOL	CBLoadedDivisionCheck( DivisionInfo * pDivisionInfo , PVOID pData );

	BOOL	IsLoadedDivision( AuPOS * pPos );
	BOOL	IsLoadedDivision( INT32	nDivision );
};

// 텍스쳐 타입 정리..
enum TEXTURETYPE
{
	TT_FLOORTILE	,	// 바닥 타일.. 텍스쳐 4개가 하나로 돼어 알파타일로 제작돼어 옵셋으로 구분.	2x2
	TT_UPPERTILE	,	// 바닥 타일 위에 올라가는 알파 무늬를 말하며 , 따로 알파로 제작돼지 않고 옵셋으로 분류됀다.	4x4
	TT_ONE			,	// 통채로 하나의 텍스쳐를 쓴다. 아직 사용하지 않는다.
	TT_ALPHATILE	,	// 바닥타일의 알파 타일.. 4x4
	TT_MAX
};

#define	TT_FLOOR_DEPTH	2
#define	TT_UPPER_DEPTH	4
#define TT_ONE_DEPTH	1
#define	TT_ALPHA_DEPTH	4

// 이건 클라이언트 모듈 헤더에 들어간다아..
inline int GET_TEXTURE_DIMENSION( int index  )
{
	int dim = ( ( index & 0x0f000000 ) >> 24	);

	if( dim == TEXTURE_DIMENTION_DEFAULT )
	{
		switch( GET_TEXTURE_TYPE( index ) )
		{
		case	TT_FLOORTILE	:	return	TT_FLOOR_DEPTH	;
		case	TT_UPPERTILE	:	return	TT_UPPER_DEPTH	;
		case	TT_ONE			:	return	TT_ONE_DEPTH	;
		case	TT_ALPHATILE	:	return	TT_ALPHA_DEPTH	;
		default:
			ASSERT( !"-__-!" );
			return TEXTURE_DIMENTION_DEFAULT;
		}
	}
	else	return dim;
}

inline UINT32 SET_TEXTURE_DIMENSION( UINT32 nIndex , INT32 nDim  )
{
	// 마고자 (2004-05-28 오후 12:28:42) : 디멘죤 변경.
	//int nPrevDim = ( ( nIndex & 0x0f000000 ) >> 24	);

	return ( nIndex & 0xf0ffffff ) | ( nDim << 24 );
}

inline float GET_TEXTURE_U_START	( int index  )
{
	int	dimension = GET_TEXTURE_DIMENSION( index );
	switch( GET_TEXTURE_TYPE( index ) )
	{
	case TT_FLOORTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension ) / ( float ) dimension;
	case TT_UPPERTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension ) / ( float ) dimension;
	case TT_ALPHATILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension ) / ( float ) dimension;
	case TT_ONE			:
	default:
		return 0.0f;
	}
}
inline float GET_TEXTURE_V_START	( int index  )
{
	int	dimension = GET_TEXTURE_DIMENSION( index );
	switch( GET_TEXTURE_TYPE( index ) )
	{
	case TT_FLOORTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension ) / ( float ) dimension;
	case TT_UPPERTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension ) / ( float ) dimension;
	case TT_ALPHATILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension ) / ( float ) dimension;
	case TT_ONE			:
	default:
		return 0.0f;
	}
}
inline float GET_TEXTURE_U_END		( int index  )
{
	int	dimension = GET_TEXTURE_DIMENSION( index );
	switch( GET_TEXTURE_TYPE( index ) )
	{
	case TT_FLOORTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension + 1 ) / ( float ) dimension;
	case TT_UPPERTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension + 1 ) / ( float ) dimension;
	case TT_ALPHATILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension + 1 ) / ( float ) dimension;
	case TT_ONE			:
	default:
		return 1.0f;
	}
}
inline float GET_TEXTURE_V_END		( int index  )
{
	int	dimension = GET_TEXTURE_DIMENSION( index );
	switch( GET_TEXTURE_TYPE( index ) )
	{
	case TT_FLOORTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension + 1 ) / ( float ) dimension;
	case TT_UPPERTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension + 1 ) / ( float ) dimension;
	case TT_ALPHATILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension + 1 ) / ( float ) dimension;
	case TT_ONE			:
	default:
		return 1.0f;
	}
}

inline void	_RemoveFullPath( char * fullpath , char * filename)
{
	char			drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath	( fullpath	, drive, dir, fname, ext );

	wsprintf( filename , "%s%s" , fname , ext );
}

void _PreviewTextureSetup( ApWorldSector * pSector , int indexX , int indexY , int tileindex , int depth , RwTexCoords * pCoord );
inline void _PreviewTextureSetup( ApWorldSector * pSector , int indexX , int indexY , int tileindex , RwTexCoords * pCoord )
{
	_PreviewTextureSetup( pSector , indexX , indexY , tileindex , pSector->GetDetailSectorInfo()->m_nDepth[ pSector->GetCurrentDetail() ] , pCoord );
}

inline	RwRGBA ApRGBAToRwRGBA( ApRGBA rgb )
{
	RwRGBA	rwrgba;
	rwrgba.alpha	= rgb.alpha	;
	rwrgba.red		= rgb.red	;
	rwrgba.green	= rgb.green	;
	rwrgba.blue		= rgb.blue	;
	return rwrgba;
}

inline	ApRGBA RwRGBAToApRGBA( RwRGBA rgb )
{
	ApRGBA	aprgba;
	aprgba.alpha	= rgb.alpha	;
	aprgba.red		= rgb.red	;
	aprgba.green	= rgb.green	;
	aprgba.blue		= rgb.blue	;
	return aprgba;
}

inline	INT32	GetSectorDistance( ApWorldSector * pSector1 , ApWorldSector * pSector2 )
{
	INT32 nDistanceX	= ( pSector1->GetIndexX() - pSector2->GetIndexX() );
	INT32 nDistanceZ	= ( pSector1->GetIndexZ() - pSector2->GetIndexZ() );

	nDistanceX	= ( nDistanceX < 0 ) ? -nDistanceX : nDistanceX;
	nDistanceZ	= ( nDistanceZ < 0 ) ? -nDistanceZ : nDistanceZ;
	// 마고자 (2004-03-24 오전 11:55:01) : 거리체크를 

	return	nDistanceX < nDistanceZ ? nDistanceZ : nDistanceX;
}

// 월드 섹터 포인터 뽑아내는 유틸리티 함수..
ApWorldSector * AcuGetWorldSectorFromAtomicPointer( RpAtomic * pAtomic );
BOOL RpDWSectorDestroy(RpDWSector *dwSector);

#define	ALEF_WORLD_DETAIL_SECTOR_FILE_NAME_FORMAT	"b%06dx.ma2"
#define	ALEF_WORLD_ROUGH_SECTOR_FILE_NAME_FORMAT	"a%06dx.ma2"

#define	ALEF_WORLD_OBJECT_FILE_NAME_FORMAT			"W%06d.off"

struct	_FindWorldSectorGeometryStruct
{
	_FindWorldSectorGeometryStruct()													{	}
	_FindWorldSectorGeometryStruct( float f, RpAtomic* p ) : distance(f), pAtomic(p)	{	}

	float		distance;
	RpAtomic*	pAtomic;
};

// 마고자 (2004-05-28 오후 2:33:11) : 같은 타일인지 .. 디멘젼 검사..
inline bool IsSameTile( int nIndex1 , int nIndex2 )
{
	int	category , index , dim;
	category	= GET_TEXTURE_CATEGORY	( nIndex1 );
	index		= GET_TEXTURE_INDEX		( nIndex1 );
	dim			= GET_TEXTURE_DIMENSION	( nIndex1 );
	

	if(	category	==	GET_TEXTURE_CATEGORY	( nIndex2 )	&&	
		index		==	GET_TEXTURE_INDEX		( nIndex2 )	&&
		dim			==	GET_TEXTURE_DIMENSION	( nIndex2 ) )
		return true;
	else
		return false;
}

// 마고자 (2004-05-28 오후 2:33:19) : 같은 텍스쳐를 사용하는 것 이라면..
inline bool IsSameTexture( int nIndex1 , int nIndex2 )
{
	int	category , index;
	category	= GET_TEXTURE_CATEGORY	( nIndex1 );
	index		= GET_TEXTURE_INDEX		( nIndex1 );

	if(	category	==	GET_TEXTURE_CATEGORY	( nIndex2 )	&&	
		index		==	GET_TEXTURE_INDEX		( nIndex2 )	)
		return true;
	else
		return false;
}

inline bool IsUsedThisTile( ApDetailSegment * pSegment , int tileindex )
{
	if( IsSameTile( tileindex , pSegment->pIndex[ TD_FIRST	] )	||
		IsSameTile( tileindex , pSegment->pIndex[ TD_THIRD	] )	)
		return true;
	else
		return false;
}

inline bool IsSingleBackTile( ApDetailSegment * pSegment )
{
	if( pSegment->pIndex[ TD_FIRST	] != ALEF_TEXTURE_NO_TEXTURE &&
		pSegment->pIndex[ TD_SECOND	] == ALEF_TEXTURE_NO_TEXTURE &&
		pSegment->pIndex[ TD_THIRD	] == ALEF_TEXTURE_NO_TEXTURE )
		// 4th 텍스쳐는 검사하지 않는다.
	{
		return true;
	}
	else
		return false;
}

inline	int	_GetRandTileOffset( int tileindex )
{
	switch( GET_TEXTURE_TYPE( tileindex ) )
	{
	case TT_FLOORTILE	:
		{
			// 랜덤하게 조합해서 사용.

			int	dimension	= GET_TEXTURE_DIMENSION( tileindex );
			int index;
			index = MAKE_TEXTURE_INDEX(
				GET_TEXTURE_DIMENSION	( tileindex ) ,
				GET_TEXTURE_CATEGORY	( tileindex ) ,
				GET_TEXTURE_INDEX		( tileindex ) ,
				GET_TEXTURE_TYPE		( tileindex ) ,
				rand() % (dimension * dimension )	);
			return index;
		}
		break;
	case TT_UPPERTILE	:
	case TT_ONE			:
	case TT_ALPHATILE	:
	default:
		return tileindex;
	}
}

extern INT32	g_nRidableCount		;
extern BOOL		g_bRidableChecked	;

#define AGCMMAP_THIS	( &AgcmMap::GetInstance() )

#endif // !defined(AFX_AGCMMAP_H__91149629_2429_47C8_9C28_5ECF4DCC6645__INCLUDED_)
