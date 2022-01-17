////////////////////////////////////////////////////////////////////
// Created  : 마고자 (2002-04-23 오후 6:22:14)
// Note     : 
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////

#if !defined(AFX_APMMAP_H__D09E90BC_BF4C_482A_AE9C_E19838224685__INCLUDED_)
#define AFX_APMMAP_H__D09E90BC_BF4C_482A_AE9C_E19838224685__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApModule.h"
#include "ApAdmin.h"
//#include "MagList.h"
#include "ApMapBlocking.h"
#include "ApWorldSector.h"
#include "MagUnpackManager.h"
#include "ApMemoryPool.h"
#include <AuXmlParser.h>
#include <AuLua.h>

#include <vector>
#include <map>

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "ApmMapD" )
#else
#pragma comment ( lib , "ApmMap" )
#endif
#endif
//@} Jaewon

#define	MAP_WORLD_INDEX_WIDTH	800			// 전체맵 크기의 넓이.	x축 
#define	MAP_WORLD_INDEX_HEIGHT	800			// 전체맵 크기의 높이.	z축
#define	MAP_SECTOR_WIDTH		(6400.0f)	// 한섹터의 너비.
#define	MAP_SECTOR_HEIGHT		(6400.0f)	// 한섹터의 높이.
#define MAP_DEFAULT_DEPTH		16
#define MAP_STEPSIZE			(400.0f)
#define	MAP_UNPACK_MANAGER_BUF	9

#define INVALID_HEIGHT					(-20000.0f)

#define	ALEF_MAX_LOADING_SECTOR_BUFFER	65535

// 최대높이 최저높이.. 제한을 검.
#define	SECTOR_MAX_HEIGHT				100000.0f
#define	SECTOR_MIN_HEIGHT				INVALID_HEIGHT

#define MAKEDWORD(lo,hi)	( (DWORD) ( ( (hi) << 32 ) | (lo) ) )

// Utility Inline Functions..
inline	INT32 ArrayIndexToSectorIndexX	( INT32 arrayindex_X		) { return  arrayindex_X	- ( INT32 ) ( MAP_WORLD_INDEX_WIDTH		>> 1 ); }
inline	INT32 ArrayIndexToSectorIndexZ	( INT32 arrayindex_Z		) { return  arrayindex_Z	- ( INT32 ) ( MAP_WORLD_INDEX_HEIGHT	>> 1 ); }
inline	INT32 SectorIndexToArrayIndexX	( INT32 sectorindex_X		) { return  sectorindex_X	+ ( INT32 ) ( MAP_WORLD_INDEX_WIDTH		>> 1 ); }
inline	INT32 SectorIndexToArrayIndexZ	( INT32 sectorindex_Z		) { return  sectorindex_Z	+ ( INT32 ) ( MAP_WORLD_INDEX_HEIGHT	>> 1 ); }

#define REGIONTEMPLATEFILE	"regiontemplate.ini"
#define REGIONFILE			"region.ini"
#define WORLDMAPTEMPLATE	"worldmap.ini"

#define	DIMENSION_NAME_LENGTH		32
#define	WORLDMAP_FILENAME_LENGTH	32
#define	WORLDMAP_COMMENT_LENGTH		256

#define APMMAP_REGIONPECULIARITY_DISABLE_ALL_SKILL		0
#define APMMAP_REGIONPECULIARITY_DISABLE_ALL_ITEM		0

enum AgpmMapRestrictPlace
{
	AGPMMAP_RESTRICT_CASTLE_ARCHLORD = 81,
	AGPMMAP_RESTRICT_CONSULT_OFFICE	 = 177,
	AGPMMAP_RESTRICT_EVENT_BATTLE_GROUND = 178,
	AGPMMAP_RESTRICT_EVENT_BATTLE_GROUND_TOWN = 179,
};

typedef enum ApmMapCallbackPoint			// Callback ID in Map Module 
{
	APMMAP_CB_ID_ADDCHAR			= 0,
	APMMAP_CB_ID_REMOVECHAR				,
	APMMAP_CB_ID_MOVECHAR				,
	APMMAP_CB_ID_ADDITEM				,
	APMMAP_CB_ID_REMOVEITEM				,
	APMMAP_CB_ID_LOAD_SECOTR			,
	APMMAP_CB_ID_CLEAR_SECTOR			,
	APMMAP_CB_ID_SAVEGEOMETRY			,
	APMMAP_CB_ID_LOADGEOMETRY			,
	APMMAP_CB_ID_SAVEMOONEE				,
	APMMAP_CB_ID_LOADMOONEE				,
	APMMAP_CB_ID_GET_MIN_HEIGHT			
} ApmMapCallbackPoint;

enum AgpmMapRegionPeculiarity
{
	APMMAP_PECULIARITY_NONE = 0,
	APMMAP_PECULIARITY_PARTY,
	APMMAP_PECULIARITY_CHATTING,
	APMMAP_PECULIARITY_ITEM_DROP,
	APMMAP_PECULIARITY_SKUL_DROP,
	APMMAP_PECULIARITY_REMOTE_BUFF,
	APMMAP_PECULIARITY_SKILL,
	APMMAP_PECULIARITY_SHOWNAME,
	APMMAP_PECULIARITY_PRESERVE_BUFF,
	APMMAP_PECULIARITY_DROP_EXP,
	APMMAP_PECULIARITY_CRIMINAL_RULE,
	APMMAP_PECULIARITY_USE_ITEM,
	APMMAP_PECULIARITY_GUILD_MESSAGE,
	APMMAP_PECULIARITY_GUILD_BUFF,
	APMMAP_PECULIARITY_GUILD_PVP,
	APMMAP_PECULIARITY_DROP_SKUL_TID,
	APMMAP_PECULIARITY_DROP_SKUL_RESTRICTION,
	APMMAP_PECULIARITY_DROP_SKUL_RESTRICTION_TIME,
	APMMAP_PECULIARITY_IS_ALLOW_LOGIN_PLACE,
	APMMAP_PECULIARITY__MOB_CHARISMA_DROP,
	APMMAP_PECULIARITY__PVP_CHARISMA_DROP,
	APMMAP_PECULIARITY_MAX
};

enum AgpmMapRegionPeculiarityReturn
{
	APMMAP_PECULIARITY_RETURN_INVALID_REGIONINDEX = 0,
	APMMAP_PECULIARITY_RETURN_NO_PECULIARITY,
	APMMAP_PECULIARITY_RETURN_ENABLE_USE,
	APMMAP_PECULIARITY_RETURN_DISABLE_USE,
	APMMAP_PECULIARITY_RETURN_ENABLE_USE_UNION
};

enum AgpmMapRegionPeculiarityShowName
{
	APMMAP_PECULIARITY_SHOW_NAME_ALL	= 0x00000001,
	APMMAP_PECULIARITY_SHOW_NAME_UNION	= 0x00000002,
	APMMAP_PECULIARITY_SHOW_NAME_ALL_NO = 0x00000004,
};


// move callback 인 경우 pData에 이 구조체 포인터를 넘겨준다.
typedef struct _stApmMapMoveSector {
	ApWorldSector	*pOldSector;			// 이동하기 전에 있던 섹터
	ApWorldSector	*pNewSector;			// 이동한 섹터
	AuPOS			*pstCurrentPos;			// 이동후 현재 포지션
} stApmMapMoveSector, *pstApmMapMoveSector;

struct	BindElement
{
	INT32		nIndex	;	// 해당 인덱스.
	
	AuCYLINDER	stData	;
	// stData.height 가 0일경우 박스로 처리함..
	// 즉.. stData.center 가 lefttop ,
	// radius 하 width , height 가 height 로..
};

typedef enum _ApmMapCharType {
	APMMAP_CHAR_TYPE_PC				= 0x01,
	APMMAP_CHAR_TYPE_NPC			= 0x02,
	APMMAP_CHAR_TYPE_MONSTER		= 0x04,
} ApmMapCharType;

typedef	AuList< BindElement >	BindList		;
typedef AuList< BindList >		BindTypeList	;

// Bob님 추가(071203)
typedef enum _eApmMapMaterial
{
	APMMAP_MATERIAL_SOIL		,
	APMMAP_MATERIAL_SWAMP		,
	APMMAP_MATERIAL_GRASS		,
	APMMAP_MATERIAL_SAND		,
	APMMAP_MATERIAL_LEAF		,
	APMMAP_MATERIAL_SNOW		,
	APMMAP_MATERIAL_WATER		,
	APMMAP_MATERIAL_STONE		,
	APMMAP_MATERIAL_WOOD		,
	APMMAP_MATERIAL_METAL		,
	APMMAP_MATERIAL_BONE		,
	APMMAP_MATERIAL_MUD			,	// 진흙
	APMMAP_MATERIAL_SOILGRASS	,	// 풀 + 흙
	APMMAP_MATERIAL_SOLIDSOIL	,	// 돌 + 흙
	APMMAP_MATERIAL_SPORE		,	// 포자
	APMMAP_MATERIAL_MOSS		,	// 이끼
	APMMAP_MATERIAL_GRANITE		,	// 화강암.
	APMMAP_MATERIAL_NUM
} eApmMapMaterial;

// 마고자 (2004-06-30 오후 4:03:37) : 
// 사실상 오브젝트 정보를 저장하기 위한 데이타 스트럭쳐..
struct stLoadedDivision
{
public:
	enum
	{
		DATA_NODATA		= 0x00,
		DATA_OBJECT		= 0x01,
		DATA_MAX
	};

protected:
	INT32	nDivisionIndex	;
	UINT32	uRefCount		;
	UINT32	uSetUnSavedFlag	;	// 저장이 될 것이 있는지..
	UINT32	uLoadDataFlag	;	// 데이타라 로딩 되어있는지..

public:
	stLoadedDivision():nDivisionIndex( 0 ) , uRefCount( 0 ) , uSetUnSavedFlag( DATA_NODATA ) , uLoadDataFlag( DATA_NODATA ) {}

	INT32	GetDivisionIndex() { return nDivisionIndex;	}
	INT32	SetDivisionIndex( INT32 nDivision ) { return nDivisionIndex = nDivision;	}

	// 마고자 (2004-06-30 오후 4:08:26) : 레퍼런스 카운트 관련..
	UINT32	GetRefCount	()	{ return uRefCount; }
	void	AddRef		()	{ uRefCount++;	}
	BOOL	DecreaseRef	()	{ uRefCount--; return TRUE;	}

	UINT32	GetUnSavedFlag()				{ return uSetUnSavedFlag; }
	UINT32	SetUnSavedFlag( UINT32 uFlag )	{ return uSetUnSavedFlag |= uFlag; }

	UINT32	GetLoadDataFlag()				{ return uLoadDataFlag; }
	UINT32	SetLoadDataFlag( UINT32 uFlag )	{ return uLoadDataFlag |= uFlag; }
};

struct DivisionInfo;

//모듈 기능 분리.
//
//ApmMap - 맵 퍼블릭 모듈
//- 데이타 이동.
//- 블러킹 처리
//- 리스트 관리
//
//AgcmMap - 맵 클라이언트 모듈
//- 맵 로딩
//
//AgsmMap - 맵 서버 모듈
//- 맵 로딩

//@{ kday 20050823
// ;)
typedef void (* FPTR_SkillDbg_Ready)(const ApWorldSector*, const AuMATRIX*, const AuBOX*, const AuPOS* );
typedef void (* FPTR_SkillDbg_PushBox)(const AuPOS&, const AuPOS& );
typedef void (* FPTR_SkillDbg_PushPos)(const AuPOS&);
//@} kday

// Special Rule On Region
struct RegionPerculiarity
{	
	struct  Perculiarity
	{
		BYTE	m_bParty			: 1;
		BYTE	m_bItemDrop			: 1;
		BYTE	m_bSkulDrop			: 1;
		BYTE	m_bRemoteBuff		: 1;
		BYTE	m_bEnableChat		: 1;

		BYTE	m_bShowName			: 3;
		BYTE	m_bPreserveBuff		: 1;

		BYTE	m_bDropExp			: 1;
		BYTE	m_bCriminalRule		: 1;
		BYTE	m_bGuildMessage		: 1;
		BYTE	m_bGuildBuff		: 1;
		BYTE	m_bGuildPVP			: 1;
		BYTE	m_bSkullRestriction	: 1;
		BYTE	m_bAllowLoginPlace	: 1;
		BYTE	m_bMobCharismaDrop	: 1;
		BYTE	m_bPVPCharismaDrop	: 1;

		INT32	m_lSkulTid;
		INT32	m_ulRestrictionTime;

		Perculiarity()
		{
			m_bParty		= TRUE;
			m_bItemDrop		= TRUE;
			m_bSkulDrop		= TRUE;
			m_bRemoteBuff	= TRUE;
			m_bEnableChat	= TRUE;
			m_bPreserveBuff = TRUE;
			m_bDropExp		= TRUE;
			m_bCriminalRule	= TRUE;
			m_bGuildMessage	= TRUE;
			m_bGuildBuff	= TRUE;
			m_bGuildPVP		= TRUE;
			m_bAllowLoginPlace	= TRUE;
			m_bMobCharismaDrop	= FALSE;
			m_bPVPCharismaDrop	= FALSE;
			m_bShowName		= APMMAP_PECULIARITY_SHOW_NAME_ALL;
			m_lSkulTid		= 0;

			m_bSkullRestriction	= TRUE;
			m_ulRestrictionTime	= 0;
		}
	};

	INT32			m_lRegionIndex;
	BOOL			m_bUsePerculiarity;
	Perculiarity	m_stPerculiarity;
	vector<INT32>	m_vDisableSkillList;
	vector<INT32>	m_vDisableUseItemList;

	RegionPerculiarity()
	{
		m_lRegionIndex = 0;
		m_bUsePerculiarity = FALSE;
		m_vDisableSkillList.clear();
		m_vDisableUseItemList.clear();
	}
};

typedef map<INT32, RegionPerculiarity>	RegionPerculiarityMap;
typedef pair<INT32, RegionPerculiarity> RegionPerculiarityPair;
typedef RegionPerculiarityMap::iterator	RegionPerculiarityIter;	
class ApmMap : public ApModule  
{
//@{ kday 20050823
// ;)
private:
	FPTR_SkillDbg_Ready		m_fptrSkillDbg_Ready;
	FPTR_SkillDbg_PushBox	m_fptrSkillDbg_PushBox;
	FPTR_SkillDbg_PushPos	m_fptrSkillDbg_PushPos;
public:
	VOID	SetSkillDbgCB( FPTR_SkillDbg_Ready		fptrSkillDbg_Ready
		, FPTR_SkillDbg_PushBox	fptrSkillDbg_PushBox
		, FPTR_SkillDbg_PushPos	fptrSkillDbg_PushPos);
//@} kday

protected:
	BOOL	m_bInitialized				;
	char	*m_strMapGeometryDirectory	;	// Detail Data , Geometry가 있는 위치..
	char	*m_strMapMooneeDirectory	;	// Detail Data , Moonee가 있는 위치..
	char	*m_strMapCompactDirectory	;	// Compact Data가 풀리는 위치..
	char	*m_strWorldDirectory		;	// 컴팩트 데이타가 압축돼어있는위치..

	// Attributes
	// ApWorldSector 데이타를 지역별로 관리할수 있는 데이타 구조.
	ApWorldSector * m_ppSector[ MAP_WORLD_INDEX_WIDTH ][ MAP_WORLD_INDEX_HEIGHT ];
		// [x][y]
		// 섹터 데이타를 관리하기 위한 포인터의 이차원 배열. 
		// 배열에서 인덱스가 맵의 인덱스인것은 아니다.

	BOOL	m_bLoadCompactData				;
	BOOL	m_bLoadDetailData				;
	BOOL	m_bAutoLoadData					;	// 지형 자동 로딩.. 툴에서 쓸땐 OFF

	ApAdmin	m_csAdminSectorRemove			;
	INT32	m_lSectorRemoveDelay			;
	INT32	m_lSectorRemovePool				;
	UINT32	m_ulPrevRemoveClockCount		;

	// 섹터로드/디스트로이 콜백..
	void	( *	m_pfSectorLoadCallback		)( ApWorldSector * pSector );
	void	( *	m_pfSectorDestroyCallback	)( ApWorldSector * pSector );


protected:
	// 로딩되어있는 섹터를 모아두는 버퍼.
	ApWorldSector *	m_pCurrentLoadedSectors		[ ALEF_MAX_LOADING_SECTOR_BUFFER ];
	UINT32			m_nCurrentLoadedSectorCount	;
	ApMutualEx		m_csMutexSectorList;

public:
	ApMemoryPool	m_csMemoryPool;
	ApWorldSector **GetCurrentLoadedSectors		() { return m_pCurrentLoadedSectors		; }
	UINT32			GetCurrentLoadedSectorCount	() { return m_nCurrentLoadedSectorCount	; }

	void			CLS_AddSector		( ApWorldSector * pSector );
	void			CLS_RemoveSector	( ApWorldSector * pSector );
	
public:
	// 모듈 데이타 인덱스
	enum APMMAP_DATA_INDEX
	{
		SECTOR_DATA,
		SEGMENT_DATA
	};

	ApmMap();
	virtual ~ApmMap();

	inline BOOL		IsAutoLoadData			(					) { return m_bAutoLoadData			;	}
	inline void		SetAutoLoadData			( BOOL bAuto		) { m_bAutoLoadData			= bAuto	;	}

	inline	BOOL	IsLoadingCompactData	() { return m_bLoadCompactData	;	}
	inline	BOOL	IsLoadingDetailData		() { return m_bLoadDetailData	;	}
	inline	void	SetLoadingMode			( BOOL bLoadCompactData , BOOL bLoadDetailData )
	{ m_bLoadCompactData = bLoadCompactData ; m_bLoadDetailData = bLoadDetailData ;	}
	inline	void	SetSectorCallback		( void	( *	pLoadCallback )( ApWorldSector * pSector ) , void ( * pDestroyCallback )( ApWorldSector * pSector ) )
	{ m_pfSectorLoadCallback = pLoadCallback; m_pfSectorDestroyCallback = pDestroyCallback; }

	// 맵툴 데이타를 로딩하고싶을때 , 데이타 읽기전에 셋을 해주어야 한다.
	// 둘중에 하나만 읽게돼며 , 디테일이 있을경우 컴팩트는 무시된다.

	// Operations
	// 해당 플레이어가 있는 월드 섹터로 콜을 넘겨주는 루틴들.

	char *	GetMapGeometryDirectory			() { return m_strMapGeometryDirectory	; }
	char *	GetMapMooneeDirectory			() { return m_strMapMooneeDirectory		; }
	char *	GetMapCompactDirectory			() { return m_strMapCompactDirectory	; }
	char *	GetWorldDirectory				() { return m_strWorldDirectory			; }

	BOOL	Init					(	char *strGeometryDir	= NULL ,
										char *strMooneeDir		= NULL ,
										char *strCompactDir		= NULL ,
										char *strWorldDir		= NULL );	// 데이타 초기화.
									// "C:\Alef\Map" 형식으로 지정해주어야함.

	enum	BLOCKINGTYPE
	{
		GROUND		,	// 보통땅.
		SKY			,	// 하늘.. 날으는 탈것
		UNDERGROUND	,	// 땅속.. 아마 안쓰지 않을까
		GHOST			// 유령.. 블로킹을 무시한다.
	};

	BOOL	GetValidDestination				( AuPOS * pStart , AuPOS * pDest , AuPOS * pValid , FLOAT fRadius , BLOCKINGTYPE eType );
	BOOL	GetValidDestination_LineBlock	( AuPOS * pStart , AuPOS * pDest , AuPOS * pValid , FLOAT fRadius );

	BOOL	CheckBlockingInfo		( AuPOS pos , BLOCKINGTYPE eType );	// TRUE 못가는것 FALSE 가는것.
protected:
	INT32	CheckBlockingInfo		( INT32 x , INT32 z , BLOCKINGTYPE eType );
		// 월드에서 (0,0) 을 중심으로 옵셋으로 값을 입력하며 , 
		// 내부적으로 섹터를 얻어내서 값을 찾아준다.
		// 리턴값은 다음 enum과 같다.
public:
	enum MapBlocking
	{
		NO_BLOCKING			= 0x00	,
		GEOMETRY_BLOCKING	= 0x01	,
		OBJECT_BLOCKING		= 0x02	
	};
	
	BOOL	IsPassThis				( AuPOS pos1 , AuPOS pos2 , AuPOS *pCollisionPoint  = NULL );

	// Serialization Functions..

	ApWorldSector *	GetSector		( AuPOS pos , INT32 *px = NULL , INT32 *py = NULL , INT32 *pz = NULL	);	// 좌표가 속해있는 인덱스를 알아낸다.
	ApWorldSector *	GetSector		( INT32 wx			, INT32 wy			, INT32 wz				);	// 월드 인덱스.
	inline ApWorldSector *	GetSector( INT32 wx	, INT32 wz ) { return GetSector( wx , 0 , wz ); }
	inline ApWorldSector *	GetSectorByArrayIndex( INT32 ix, INT32 iz )
	{
		if ( ix < 0 || iz < 0 || ix >= MAP_WORLD_INDEX_WIDTH || iz >= MAP_WORLD_INDEX_HEIGHT)
			return NULL;

		return m_ppSector[ ix ][ iz ];
	}
	inline ApWorldSector *	GetSector( FLOAT fx	, FLOAT fz )
	{	
		AuPOS pos;
		pos.x = fx ; pos.y = 0.0f ; pos.z = fz;
		return GetSector( pos );
	}

	INT32	GetSectorList		( AuPOS pos , float range , ApWorldSector * apSectors[], INT32 size);

	ApWorldSector * GetSectorByArray( INT32 arrayindexX	, INT32 arrayindexY	, INT32 arrayindexZ		);	// ppSector 배열 인덱스

	INT32	GetItemList			( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size ); // return 얻어진 갯수;

	INT32	GetCharList			( INT32	nDimensionIndex , INT32	lCharType ,	AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 = NULL , INT32 size2 = 0 ); // 원 안에서 구함. return 얻어진 갯수;
	INT32	GetCharList			( INT32	nDimensionIndex , INT32	lCharType ,	AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 = NULL , INT32 size2 = 0 ); // 원 안의 특정 각에서. return 얻어진 갯수;
	INT32	GetCharList			( INT32	nDimensionIndex , INT32	lCharType ,	AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 = NULL , INT32 size2 = 0, BOOL bDbgSkill=FALSE ); // 사각형 안에서. return 얻어진 갯수;

	INT32	GetCharList			( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // 원 안에서 구함. return 얻어진 갯수;
	INT32	GetCharList			( INT32	nDimensionIndex , AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // 원 안의 특정 각에서. return 얻어진 갯수;
	INT32	GetCharList			( INT32	nDimensionIndex , AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // 사각형 안에서. return 얻어진 갯수;

	INT32	GetNPCList			( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // 원 안에서 구함. return 얻어진 갯수;
	INT32	GetNPCList			( INT32	nDimensionIndex , AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // 원 안의 특정 각에서. return 얻어진 갯수;
	INT32	GetNPCList			( INT32	nDimensionIndex , AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // 사각형 안에서. return 얻어진 갯수;

	INT32	GetMonsterList		( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // 원 안에서 구함. return 얻어진 갯수;
	INT32	GetMonsterList		( INT32	nDimensionIndex , AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // 원 안의 특정 각에서. return 얻어진 갯수;
	INT32	GetMonsterList		( INT32	nDimensionIndex , AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2, BOOL bDbgSkill=FALSE ); // 사각형 안에서. return 얻어진 갯수;

	INT32	GetObjectList		( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size ); // return 얻어진 갯수;

	/*
	BOOL	GetSectorDataChar	( AuPOS pos , INT32 ** pArray , INT32 * pSize		);
	BOOL	GetSectorDataItem	( AuPOS pos , INT32 ** pArray , INT32 * pSize		);
	BOOL	GetSectorDataObject	( AuPOS pos , INT32 ** pArray , INT32 * pSize		);

	BOOL	GetSectorDataChar	( INT32 wx , INT32 wy , INT32 wz , INT32 ** pArray , INT32 * pSize		); // z요소는 항상0 .. 예약된필드.
	BOOL	GetSectorDataItem	( INT32 wx , INT32 wy , INT32 wz , INT32 ** pArray , INT32 * pSize		);
	BOOL	GetSectorDataObject	( INT32 wx , INT32 wy , INT32 wz , INT32 ** pArray , INT32 * pSize		);
	*/

	FLOAT	GetHeight			( FLOAT x , FLOAT z );
	UINT8	GetType				( FLOAT x , FLOAT z	);	// 바닥의 타입을 얻어낸다.
	

	// 속도 개선을 위해서..
	CMagUnpackManager * GetUnpackManagerDivisionCompact		( INT32 nDivisionIndex );
	CMagUnpackManager * GetUnpackManagerDivisionGeometry	( INT32 nDivisionIndex );
	CMagUnpackManager * GetUnpackManagerDivisionMoonee		( INT32 nDivisionIndex );

	void				FlushUnpackManagerDivision();

	struct	CompactDataUnpackManagerBufferElement
	{
		CMagUnpackManager * pUnpack			;
		INT32				nDivisionIndex	;
		UINT32				uLastAccessTime	;
	};

	CompactDataUnpackManagerBufferElement	m_ArrayUnpackBufferCompact	[ MAP_UNPACK_MANAGER_BUF ];
	CompactDataUnpackManagerBufferElement	m_ArrayUnpackBufferGeometry	[ MAP_UNPACK_MANAGER_BUF ];
	CompactDataUnpackManagerBufferElement	m_ArrayUnpackBufferMoonee	[ MAP_UNPACK_MANAGER_BUF ];
	
public:
	// 해당 섹터에서의 연산을 수행한다.

	// 섹터의 정보를 메모리에 잡느냐 안잡느냐.를 결정하는 두 함수이다.
	// 섹터의 내용과는 다른 것임..
	ApWorldSector *
			SetupSector		( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	);
		// 섹터를 준비함.. 섹터를 로딩하지는 않음.
		// 섹터 메모리를 할당하고 , 로딩할 준비를 함.  이 순간부터 add char 등 관리 펑션은 사용할 수 있음.
	
	BOOL	DeleteSector	( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	);
	BOOL	DeleteSector	( ApWorldSector	* pSector );// RemovePool에 넣는다.
	BOOL	DeleteSectorReal( ApWorldSector	* pSector );// 메모리에서 제거함.
		// Setup Sector에 반대돼는 함수 , 섹터자체를 메모리에서 제거함..
		// 제거함과 동시에 섹터 데이타까지 같이 날림.

	void	FlushDeleteSectors();
		// 마고자 (2004-12-03 오후 12:38:27) : 리스트에 올라간거처리함..


	// 데이타 로딩 펑션..
	// 섹터는 셋업이 돼어 있어야함..
	BOOL			LoadSector		( ApWorldSector	* pSector , BOOL bServer = FALSE );

	// 인덱스가 잘못 들어갈경우 뻑이날 가능성이 있음..
	inline	BOOL	LoadSector		( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z , BOOL bServer = FALSE )
	{
		return LoadSector	( m_ppSector[ SectorIndexToArrayIndexX( sectorindex_x ) ][ SectorIndexToArrayIndexZ( sectorindex_z ) ] , bServer );
	}
		// 섹터의 지형 정보를 메모리에 할당하고 , 하드에서 로딩함.
		// 섹터는 Setup 돼어있어야함..
		// 로드 콜백이 호출됨,.

	BOOL			ClearSector		( ApWorldSector	* pSector											);
	inline	BOOL	ClearSector		( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	)
	{
		return ClearSector	( m_ppSector[ SectorIndexToArrayIndexX( sectorindex_x ) ][ SectorIndexToArrayIndexZ( sectorindex_z ) ] );
	}
		// 로딩 돼어있던 지형정보만을 메모리에서 제거함.
		// 디스트로이 콜백이 호출됨..

	// 옛날꺼.. 안씀..
	//BOOL	_LoadSectorOld	( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	);

	// 전체 로딩..
	BOOL	LoadAll			( BOOL bLoadData = TRUE , BOOL bServerData = FALSE , ProgressCallback pfCallback = NULL , void * pData = NULL );
	BOOL	SaveAll			();
	void	DeleteAllSector	();
	void	DeleteRealAllSector	();
	void	ClearAllSector	();

	BOOL	LoadDivision	( UINT32 uDivision , BOOL bLoadData , BOOL bServerData );

	BOOL	AddChar			( INT32	nDimensionIndex , ApmMapCharType eCharType,		AuPOS pos,		INT_PTR cid,		INT_PTR cid2,			INT32 range	= 0);
	BOOL	UpdateChar		( INT32	nDimensionIndex , ApmMapCharType eCharType,		AuPOS posPrev ,	AuPOS posCurrent ,	INT_PTR cid,			INT_PTR cid2	);
	BOOL	DeleteChar		( INT32	nDimensionIndex , ApmMapCharType eCharType,		AuPOS pos ,		INT_PTR cid						);
	BOOL	DeleteChars		( INT32	nDimensionIndex , ApmMapCharType eCharType,		ApWorldSector * pSector							);
	BOOL	MoveChar		( INT32	nDimensionIndex , ApmMapCharType eCharType,		ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,		INT_PTR cid2,		AuPOS posCurrent	);

	BOOL	AddChar			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2,			INT32 range = 0		);
	BOOL	UpdateChar		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent ,	INT_PTR cid,			INT_PTR cid2	);
	BOOL	DeleteChar		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid						);
	BOOL	DeleteChars		( INT32	nDimensionIndex , ApWorldSector * pSector							);
	BOOL	MoveChar		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,		INT_PTR cid2,		AuPOS posCurrent	);

	BOOL	AddNPC			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2,			INT32 range = 0		);
	BOOL	UpdateNPC		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent ,	INT_PTR cid,			INT_PTR cid2	);
	BOOL	DeleteNPC		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid						);
	BOOL	DeleteNPCs		( INT32	nDimensionIndex , ApWorldSector * pSector							);
	BOOL	MoveNPC			( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,		INT_PTR cid2,		AuPOS posCurrent	);

	BOOL	AddMonster		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2,			INT32 range = 0		);
	BOOL	UpdateMonster	( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent ,	INT_PTR cid,			INT_PTR cid2	);
	BOOL	DeleteMonster	( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid						);
	BOOL	DeleteMonsters	( INT32	nDimensionIndex , ApWorldSector * pSector							);
	BOOL	MoveMonster		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,		INT_PTR cid2,		AuPOS posCurrent	);

	BOOL	AddItem			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid						);
	BOOL	UpdateItem		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent , INT_PTR iid	);
	BOOL	DeleteItem		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid						);
	BOOL	DeleteItems		( INT32	nDimensionIndex , ApWorldSector * pSector							);
	BOOL	MoveItem		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR iid,	AuPOS posCurrent	);

	// Parn 작업 Blocking 정보 추가
	BOOL	AddObject		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid ,						AuBLOCKING *pstBlocking = NULL);
	BOOL	UpdateObject	( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent , INT_PTR oid	);
	BOOL	DeleteObject	( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid						);
	BOOL	DeleteObjects	( INT32	nDimensionIndex , ApWorldSector * pSector							);
	BOOL	MoveObject		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR oid,	AuPOS posCurrent	);


	// 지형 블러킹 추가 펑션들..
	void	AddObjectBlocking		( AuBLOCKING * pBlocking  , INT32 count );
	void	ClearAllObjectBlocking	();	// 오브젝트 블러킹 정보 수정후 이걸 호출해서 클리어한 후 다시 작성한다.
	void	ClearAllHeightPool		();

	// Remove되는 Sector들 처리 관련 펑션들..
	void	SetRemoveSector	( INT32 lRemoveDelay ,	INT32 lRemovePool			) { m_lSectorRemoveDelay = lRemoveDelay; m_lSectorRemovePool = lRemovePool; }
	void	ProcessRemove	( UINT32 ulClockCount								);

	// 바인딩 요소 추가 펑션. 
	// 마고자 (2004-03-08 오전 11:54:03) : 
	BindTypeList	m_listBind;

	INT32	AddBindType			(); 
	void	ClearAllBindData	();
	BindList	* GetBindList	( INT32 nBindIndex );

	// return Binding Type 
	// 바인딩 요소 추가할 공간을 할당하고 해당 요소의 인덱스를 리턴한다. 
	BOOL	AddBindElementRect	( INT32 nBindIndex , INT32 nData , AuPOS pos , FLOAT width , FLOAT height	); 
	BOOL	AddBindElementCircle( INT32 nBindIndex , INT32 nData , AuPOS pos , FLOAT radius					); 

	BOOL	IsChangedBind		( INT32 nBindIndex , AuPOS prev , AuPOS next	);  
	INT32	GetBindData			( INT32 nBindIndex , AuPOS pos					); 

	// 바인딩 걸리는게 없으면 0 리턴.

	FLOAT	GetMinHeight		( ApWorldSector * pSector );

public:
	// 모듈데이타 추가 인터페이스 펑션
	INT16	AttachSectorData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
	INT16	AttachSegmentData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

public:
	// 모듈 메니져로 오는 이벤트들.
	virtual BOOL	OnAddModule	();
	virtual BOOL	OnInit		();
	virtual BOOL	OnDestroy	();
	virtual BOOL	OnIdle(UINT32 ulClockCount);

	BOOL	SetCallbackAddChar		(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveChar	(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackMoveChar		(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackAddItem		(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveItem	(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackLoadSector	(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackClearSector	(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// 마고자 (2005-03-23 오후 3:32:18) : 
	// pData 가 FILE *
	// pCustom 이 ApWorldSector *
	BOOL	SetCallbackSaveGeometry	(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback( APMMAP_CB_ID_SAVEGEOMETRY	, pfCallback, pClass); }
	BOOL	SetCallbackLoadGeometry	(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback( APMMAP_CB_ID_LOADGEOMETRY	, pfCallback, pClass); }
	BOOL	SetCallbackSaveMoonee	(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback( APMMAP_CB_ID_SAVEMOONEE		, pfCallback, pClass); }
	BOOL	SetCallbackLoadMoonee	(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback( APMMAP_CB_ID_LOADMOONEE		, pfCallback, pClass); }
	BOOL	SetCallbackGetMinHeight	(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback( APMMAP_CB_ID_GET_MIN_HEIGHT	, pfCallback, pClass); }
	
	////////////////////////////////////////////////////////
	// 오브젝트 모듈용...
	////////////////////////////////////////////////////////
	AuList< stLoadedDivision >	m_listLoadedDivision;
	stLoadedDivision *	GetLoadedDivisionInfo	( UINT32 uDivisionIndex );
	BOOL				EnumLoadedDivision		( ApModuleDefaultCallBack pCallback , PVOID pClass );
												// pCustomData 는 사용하지 않음..
												// pClass = 입력 클래스..
												// pData = stLoadedDivision *

	// Low level function
	stLoadedDivision *	AddLoadedDivision		( UINT32 uDivisionIndex );
	BOOL				RemoveLoadedDivision	( UINT32 uDivisionIndex );

	//////////////////////////////////////////////
	// 리전 컨트롤..

	enum FieldType
	{
		FT_FIELD	= 0x00000000,
		FT_TOWN		= 0x00000001,
		FT_PVP		= 0x00000002
	};

	enum SafetyType
	{
		ST_SAFE		= 0x00000000,
		ST_FREE		= 0x00000001,
		ST_DANGER	= 0x00000002
	};

	struct RegionTemplate
	{
		INT32	nIndex					;
		char	pStrName[ 32 ]			;
		INT32	nParentIndex			;	// Region의 계층을 위한 상위 Region
		INT32	nWorldMapIndex			;	// 월드맵 번호 저장
		INT32	nSkyIndex				;
		FLOAT	fVisibleDistance		;	// 시야거리.
		FLOAT	fMaxCameraHeight		;	// Top View 카메라 최대 높이.
		INT32	nLevelLimit				;	// 레벨제한.. 0이면 무시.
		INT32	nLevelMin				;	// 레벨 미니멈.. 0이면 무시
		INT32	nUnableItemSectionNum	;	// 지역에서 제한되는 ItemSectionNumber
		AuPOS	stResurrectionPos		;	// 부활 지역

		struct PropertyList
		{
			BYTE	uFieldType		: 2;	// 마을 필드 던젼 
			BYTE	uSafetyType		: 2;	// 안전 자유전투 위험지역

			BYTE	bRidable		: 1;	// 탈것 가능
			BYTE	bPet			: 1;	// 펫 사용 가능..
			BYTE	bItemWangbok	: 1;	// 왕복문서 사용 가능.
			BYTE	bItemPotion		: 1;	// 회복 물약 사용..
			// 1Byte

			BYTE	bItemResurrect	: 1;	// 부활 아이템..
			BYTE	bDisableMinimap	: 1;	// 미니맵 켜지지 않게함
			BYTE	bJail			: 1;	// 감옥플래그.
			BYTE	bZoneLoadArea	: 1;	// 존로딩 지역
			BYTE	bCharacterBlock	: 1;	// 케릭터 블러킹.
			BYTE	bItemPotion2	: 1;	// 포션 타입2
			BYTE	bItemPotionGuild: 1;	// 길드전용 포션.
			BYTE	bUnderRoof		: 1;	// 천장이 있는곳.. 눈이나 비가 안오게하는곳..
			BYTE	bRecallItem		: 1;	// 소환오브 , 환영의문 사용 가능
			BYTE	bVitalPotion	: 1;	// 활력 포션 사용 여부
			BYTE	bCurePotion		: 1;	// 치료 포션 사용 여부
			BYTE	bRecoveryPotion : 1;	// 회복 포션 사용 여부

			// 1Byte
			BYTE	uReserved2		;		// 예약필드~
			// 1Byte
			BYTE	uReserved3		;		// 예약필드~
			// 1Byte
		};

		struct	ZoneLoadInfo
		{
			FLOAT	fSrcX			;	// 가져올 지역의 위치
			FLOAT	fSrcZ			;
			FLOAT	fHeightOffset	;	// 지형의 높이차 적용
			FLOAT	fDstX			;	// 목표지점 
			FLOAT	fDstZ			;	// 목표지점 
			INT32	nRadius			;	// 로딩할 범위.

			ZoneLoadInfo():fSrcX( 0.0f ),fSrcZ( 0.0f ),fHeightOffset( 0.0f ),fDstX( 0 ),fDstZ( 0 ),nRadius( 0 ){}
		};

		union TypeInfo
		{
			INT32			nType	;
			PropertyList	stType	;
		};

		TypeInfo		ti;
		ZoneLoadInfo	zi;

		char	pStrComment[ 128 ]	;

		RegionTemplate():
			nIndex			( -1	),
			nParentIndex	( -1	),
			nWorldMapIndex	( 0		),
			nSkyIndex		( 13	),
			fVisibleDistance( 1200.0f),	// 12미터.
			fMaxCameraHeight( 2750.0f),
			nLevelLimit		( 0 ),
			nLevelMin		( 0 ), // 최저 레벨
			nUnableItemSectionNum( 0 )
		{
			ti.nType		= 0		;

			strcpy( pStrName	, "" );
			strcpy( pStrComment	, "" );

			ti.stType.uFieldType		= FT_FIELD	;
			ti.stType.uSafetyType		= ST_SAFE	;
			ti.stType.bRidable			= TRUE		;	// 탈것 가능
			ti.stType.bPet				= TRUE		;	// 펫 사용 가능..
			ti.stType.bItemWangbok		= TRUE		;	// 왕복문서만 체크함..
			ti.stType.bItemPotion		= TRUE		;	// 회복 물약 사용..
			ti.stType.bItemResurrect	= TRUE		;	// 부활 아이템..
			ti.stType.bDisableMinimap	= FALSE		;	// 미니맵 꺼짐
			ti.stType.bJail				= FALSE		;	// 감옥 안임.
			ti.stType.bItemPotion2		= FALSE		;
			ti.stType.bItemPotionGuild	= FALSE		;
			ti.stType.bRecallItem		= TRUE		;
			ti.stType.bVitalPotion		= FALSE		;
			ti.stType.bCurePotion		= TRUE		;
			ti.stType.bRecoveryPotion	= TRUE		;

			stResurrectionPos.x			= 0			;
			stResurrectionPos.y			= 0			;
			stResurrectionPos.z			= 0			;

		}
	};

	AuList< RegionTemplate > m_listTemplate;
	BOOL				AddTemplate		( RegionTemplate * pTemplate );
	BOOL				RemoveTempate	( int nIndex );
	RegionTemplate *	GetTemplate		( int nIndex );
	RegionTemplate *	GetTemplate		( char * pstrRegionName );

	BOOL	SaveTemplate( char * pFileName , BOOL bEncryption = FALSE);
	BOOL	LoadTemplate( char * pFileName , BOOL bDecryption = FALSE);

	// 동일 Region인지 확인한다 
	BOOL	IsSameRegion		( RegionTemplate* pstBase, RegionTemplate* pstTarget);
	// 동일 Region인지 확인한다 (상위 인덱스여부까지 확인)
	BOOL	IsSameRegionInvolveParent( RegionTemplate* pstBase, RegionTemplate* pstTarget);

	enum REGION_KIND
	{
		RK_BOX		,
		RK_CIRCLE	,
		RK_MAX
	};

	struct RegionElement
	{
		INT32	nIndex	;
		INT32	nKind	;

		INT32	nStartX	;
		INT32	nStartZ	;
		INT32	nEndX	;
		INT32	nEndZ	;

		RegionElement()
		{
			// 원은 후에 추가.. 리저브드 필드.
			nKind = RK_BOX;
		}
	};

	AuList< RegionElement > m_listElement;
	
	BOOL						AddRegion( RegionElement * pElement )	{	return AddRegion( pElement->nIndex , pElement->nStartX , pElement->nStartZ , pElement->nEndX , pElement->nEndZ , pElement->nKind ); }
	BOOL						AddRegion( int nIndex , INT32 nStartX , INT32 nStartZ , INT32 nEndX , INT32 nEndZ , INT32 nKind = RK_BOX );
	AuNode< RegionElement > *	GetRegion( INT32 nOffsetX , INT32 nOffsetZ );
	BOOL						RemoveRegion( AuNode< RegionElement > * pNode );

	void						RemoveAllRegion();
	BOOL						SaveRegion( char * pFileName , BOOL bEncryption = FALSE);
	BOOL						LoadRegion( char * pFileName , BOOL bDecryption = FALSE);

	UINT16						GetRegion( FLOAT x , FLOAT z );

public:
	/////////////////////////////////////////
	// 계처리 관련 스트럭쳐..

	struct	DimensionTemplate
	{
		INT32	nTID	;
		char	strName[ DIMENSION_NAME_LENGTH ]	;	// 계이름..
		INT32	nStartX	;
		INT32	nStartZ	;
		INT32	nEndX	;
		INT32	nEndZ	;

		// property..
		std::vector< INT32 > arrayCreatedDimension;
	};

	// 템플릿 저장..
	std::vector< DimensionTemplate > m_arrayDimensionTemplate;

	BOOL	LoadDimensionTemplate( char * strFilename );
	BOOL	SaveDimensionTemplate( char * strFilename );

	BOOL	AddDimensionTemplate( DimensionTemplate * pTemplate );
	BOOL	RemoveDimensionTemplate( INT32 nTID );
	DimensionTemplate * GetDimensionTemplate( INT32 nTID );

	BOOL	CreateDimension	( INT32 nTID );
	BOOL	DestroyDimension( INT32 nTID , INT32 nIndex );

public:

	// 마고자 (2005-05-24 오후 2:29:02) : 
	// 월드맵 관련..
	// 다음코드는 리젼툴에서만 사용할 예정이므로.. 
	// 코드 편이상 여기에 배치.
	struct	WorldMap
	{
		INT32	nMID;	// Map ID, 파일이름은 wmap0001 형식으로 월드탭 아이디로 한다. 따로 카피해 주지않고 데이타가 준비돼어 있어야함.
		char	strComment[ WORLDMAP_COMMENT_LENGTH ];

		// 커버 범위
		FLOAT	xStart		;
		FLOAT	zStart		;
		FLOAT	xEnd		;
		FLOAT	zEnd		;

		INT32	nMapItemID	;	// -1 이면 NO ITEM . 아이템 없이도 표시됨.

		WorldMap() :	nMID ( 0 ) , xStart ( 0.0f ) , zStart ( 0.0f ) ,
						xEnd ( 0.0f ) , zEnd ( 0.0f ) , nMapItemID ( -1 )			
		{
			strcpy( strComment , "No Map" );
		}
	};

	std::vector< WorldMap > m_arrayWorldMap;
	BOOL	LoadWorldMap( char * pFileName , BOOL bDecryption = FALSE );
	BOOL	SaveWorldMap( char * pFileName , BOOL bEncryption = FALSE );

	WorldMap *	GetWorldMapInfo( INT32 nMID );

	// Clean Up
	void	RemoveAllWorldMap();

	inline INT32	GetTargetPositionLevelLimit( AuPOS &stPos )
	{
		RegionTemplate	* pTemplate = GetTemplate( GetRegion( stPos.x , stPos.z ) );
		return pTemplate ? pTemplate->nLevelLimit : 0 ;
	}

	RegionPerculiarityMap	m_RegionPerculiarityMap;
	AuXmlDocument			m_XmlData;
	AuXmlNode*				m_pRootRuleNode;

	BOOL							LoadRegionPerculiarity( BOOL bEncrypt = FALSE );
	AgpmMapRegionPeculiarityReturn	CheckRegionPerculiarity(INT32 lRegionIndex, AgpmMapRegionPeculiarity ePeculiarity, INT32 lResevedOption = 0);
	INT32							LoadRegioinPerculiaritySkulTid(INT32 lRegionIndex);
	INT32							LoadRegioinPerculiaritySkulRestrictionTime(INT32 lRegionIndex);
};

// 글로벌 위치로 섹터 인덱스 얻어내기..

// posZ += MAP_SECTOR_HEIGHT * ( float )( MAP_WORLD_INDEX_HEIGHT / 2 );
// return ( int ) ( ( posZ ) / MAP_SECTOR_HEIGHT  ) - MAP_WORLD_INDEX_HEIGHT / 2;

// 플로트 계산으 ㅣ오류가 있어서 , 인트로 바꾼후 계산한다.
inline INT32 PosToSectorIndexX			( FLOAT posX	) { return ( ( ( int ) posX + 2560000 ) / 6400 ) - 400; }
inline INT32 PosToSectorIndexX			( AuPOS pos		) { return PosToSectorIndexX( pos.x ); }
inline INT32 PosToSectorIndexZ			( FLOAT posZ	) { return ( ( ( int ) posZ + 2560000 ) / 6400 ) - 400; }
inline INT32 PosToSectorIndexZ			( AuPOS pos		) { return PosToSectorIndexZ( pos.z ); }

// 마고자 (2004-03-30 오후 2:29:15) : 아래 코드는 시험중..
//inline	INT32 PosToSectorIndexX			( FLOAT posX	)
//{
//	if( posX >= 0.0f )	return MagFToI ( posX / MAP_SECTOR_WIDTH  )							;
//	else				return MagFToI ( ( posX - MAP_SECTOR_WIDTH + 1.0f	) / MAP_SECTOR_WIDTH )	;
//}
//inline	INT32 PosToSectorIndexZ			( FLOAT posZ	)
//{
//	if( posZ >= 0.0f )	return MagFToI ( posZ / MAP_SECTOR_HEIGHT )							;
//	else				return MagFToI ( ( posZ - MAP_SECTOR_HEIGHT + 1.0f	) / MAP_SECTOR_HEIGHT )	;
//}

// 인덱싱 시스템 변경.. 
// 디멘존을 삽입한다.. 통맵용..

#define	TEXTURE_DIMENTION_DEFAULT	0

// 이건 클라이언트 모듈 헤더에 들어간다아..
//inline int GET_TEXTURE_DIMENSION( int index  )
//{
//	int dim = ( ( index & 0x0f000000 ) >> 24	);
//
//	if( dim == TEXTURE_DIMENTION_DEFAULT )
//	{
//		switch( GET_TEXTURE_TYPE( index ) )
//		{
//		case	TT_FLOORTILE	:	return	TT_FLOOR_DEPTH	;
//		case	TT_UPPERTILE	:	return	TT_UPPER_DEPTH	;
//		case	TT_ONE			:	return	TT_ONE_DEPTH	;
//		case	TT_ALPHATILE	:	return	TT_ALPHA_DEPTH	;
//		default:
//			ASSERT( !"-__-!" );
//			break;
//		}
//	}
//	else	return dim;
//}
inline int GET_TEXTURE_CATEGORY	( int index  ) { return ( ( index & 0x00ff0000 ) >> 16	); }
inline int GET_TEXTURE_INDEX	( int index  ) { return ( ( index & 0x0000ff00 ) >> 8	); }
inline int GET_TEXTURE_TYPE		( int index  ) { return ( ( index & 0x000000c0 ) >> 6	); }
inline int GET_TEXTURE_OFFSET	( int index  ) { return (   index & 0x0000003f			); }

inline int GET_TEXTURE_OFFSET_MASKING_OUT
								( int index  ) { return ( index & 0x0fffffc0			); }
inline int GET_TEXTURE_DIMENTION_MASKING_OUT
								( int index  ) { return ( index & 0x00ffffff			); }


inline int MAKE_TEXTURE_INDEX	( int dimension , int category , int index , int type , int offset )
{
#ifdef _DEBUG
	ASSERT( dimension	<=	8	);
	ASSERT( category	<	256	);
	ASSERT( index		<	256	);
	ASSERT( type		<	4	);
	ASSERT( offset		<	64	);
#endif

	return (
				( dimension	<< 24	) |
				( category	<< 16	) |
				( index		<< 8	) |
				( type		<< 6	) |
				( offset			) );
}

inline int UPDATE_TEXTURE_OFFSET( int index , int offset = 0 )
{
	return ( index & 0xffffffc0 ) | ( offset & 0x0000003f );
}

inline BOOL CHECK_MAP_ARRAY_INDEX( int arrayx , int arrayz )
{
	if( 0 <= arrayx && arrayx <= MAP_WORLD_INDEX_WIDTH &&
		0 <= arrayz && arrayz <= MAP_WORLD_INDEX_HEIGHT	)
		return TRUE	;
	else
		return FALSE;
}

inline BOOL CHECK_MAP_SECTOR_INDEX( int sectorx , int sectorz )
{
	if( 0 <= SectorIndexToArrayIndexX( sectorx ) && SectorIndexToArrayIndexX( sectorx ) <= MAP_WORLD_INDEX_WIDTH &&
		0 <= SectorIndexToArrayIndexZ( sectorz ) && SectorIndexToArrayIndexZ( sectorz ) <= MAP_WORLD_INDEX_HEIGHT	)
		return TRUE	;
	else
		return FALSE;
}

// 임시수정
/*inline FLOAT	GetSectorStartX	( int indexX )	{ 
	if( indexX >= 0 )	return ( float ) indexX			* MAP_SECTOR_WIDTH	;
	else				return ( float ) indexX			* MAP_SECTOR_WIDTH	- MAP_SECTOR_WIDTH;
}
inline FLOAT	GetSectorStartZ	( int indexZ )	{
	if( indexZ >= 0 )	return ( float ) indexZ			* MAP_SECTOR_HEIGHT	;
	else				return ( float ) indexZ			* MAP_SECTOR_HEIGHT	- MAP_SECTOR_HEIGHT;
}

inline FLOAT	GetSectorEndX	( int indexX )	{ return ( float ) GetSectorStartX(indexX+1); }
inline FLOAT	GetSectorEndZ	( int indexZ )	{ return ( float ) GetSectorStartZ(indexZ+1); }
*/

inline FLOAT	GetSectorStartX	( int indexX )  { return ( float ) indexX			* MAP_SECTOR_WIDTH	; }
inline FLOAT	GetSectorStartZ	( int indexZ )	{ return ( float ) indexZ			* MAP_SECTOR_HEIGHT	; }
inline FLOAT	GetSectorEndX	( int indexX )	{ return ( float ) ( indexX + 1 )	* MAP_SECTOR_WIDTH	; }
inline FLOAT	GetSectorEndZ	( int indexZ )	{ return ( float ) ( indexZ + 1 )	* MAP_SECTOR_HEIGHT	; }

inline INT32	MakeDivisionIndex( INT32 x , INT32 z ){ return ( x * 100 + z );		}

inline INT32	GetDivisionIndex( int sectorArrayIndexX , int sectorArrayIndexZ )
{
	INT32	x = sectorArrayIndexX / MAP_DEFAULT_DEPTH;
	INT32	z = sectorArrayIndexZ / MAP_DEFAULT_DEPTH;
	return MakeDivisionIndex( x , z );
}

inline INT32	GetDivisionIndexF( FLOAT fPosX , FLOAT fPosZ )
{
	INT32	nSectorX	=	PosToSectorIndexX( fPosX );
	INT32	nSectorZ	=	PosToSectorIndexZ( fPosZ );

	return GetDivisionIndex( 
		SectorIndexToArrayIndexX( nSectorX )	,
		SectorIndexToArrayIndexZ( nSectorZ )	);
}


inline INT32	GetDivisionXIndex( INT32 nDivision )	{ return ( nDivision / 100 );	}
inline INT32	GetDivisionZIndex( INT32 nDivision )	{ return ( nDivision % 100 );	}

// Return ArraySectorIndex
#define	GetFirstSectorXInDivision( nDivision )	( GetDivisionXIndex( nDivision ) * MAP_DEFAULT_DEPTH )
#define	GetFirstSectorZInDivision( nDivision )	( GetDivisionZIndex( nDivision ) * MAP_DEFAULT_DEPTH )

struct DivisionInfo
{
	INT32	nIndex	;
	FLOAT	fStartX	;
	FLOAT	fStartZ	;
	FLOAT	fWidth	;

	INT32	nX		;// Array Index
	INT32	nZ		;// Array Index
	INT32	nDepth	;

	void	GetDivisionInfo( INT32 nDivision )
	{
		nX		= GetFirstSectorXInDivision( nDivision );
		nZ		= GetFirstSectorZInDivision( nDivision );
		nDepth	= MAP_DEFAULT_DEPTH	;

		fStartX	= GetSectorStartX( ArrayIndexToSectorIndexX( nX ) );
		fStartZ	= GetSectorStartZ( ArrayIndexToSectorIndexX( nZ ) );
		fWidth	= MAP_SECTOR_WIDTH * nDepth;

		nIndex	= nDivision;
	}
};

typedef BOOL	(*DivisionCallback ) ( DivisionInfo * pDivisionInfo , PVOID pData );

#endif // !defined(AFX_APMMAP_H__D09E90BC_BF4C_482A_AE9C_E19838224685__INCLUDED_)

