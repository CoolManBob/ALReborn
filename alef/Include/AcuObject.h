// AcuObject.h: interface for the AcuObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUOBJECT_H__1801AA47_8F43_40BC_9B0C_2C7F1FD02115__INCLUDED_)
#define AFX_ACUOBJECT_H__1801AA47_8F43_40BC_9B0C_2C7F1FD02115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwCore.h"
#include "RpWorld.h"
#include "RpUsrdat.h"
#include "RpLodAtm.h"
#include "RpHAnim.h"

#include "ApBase.h"

//@{ kday 20051014
// ;)
#include <vector>
//@} kday

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuObjectD" )
#else
#pragma comment ( lib , "AcuObject" )
#endif
#endif

//#define			DINFONAME				"DI"
//#define			RENDERINFONAME			"REI"
#define			BACKFRAMEINFONAME		"BFI"
#define			SECTORMODEBACKUPNAME	"SEB"
#define			OCTREEID				"OT"

#define			TYPEINFOSLOT		0

#define			GEOM_TYPESLOT		0

enum ObjectType
{
	TYPE_ETYPE						= 0		,
	TYPE_LID						= 1		,
	TYPE_POBJECT					= 2		,
	TYPE_PTEMPLATE					= 3		,
	TYPE_PCUSTDATA					= 4		,
	TYPE_PICKATOMIC					= 5		,	// 피킹 계산용 아토믹..
	TYPE_OCTREEDATA					= 6		,	// octree renderdata 추가^^
	TYPE_OCTREEIDLIST				= 7		,	// ApdObject의 OcTreeIDList
	TYPE_BSX						= 8		,
	TYPE_BSY						= 9		,
	TYPE_BSZ						= 10	,
	TYPE_BSRADIUS					= 11	,
	TYPE_UPDATELIST					= 12	,
	TYPE_UPDATETICK					= 13	,
	// Clump가 있을 경우 clump의 bounding sphere frustum check를 공유하기 위해(for performance)
	TYPE_RENDERFRUSTUMTESTTICK		= 14	,
	TYPE_RENDERFRUSTUMTESTRESULT	= 15	,
	// 해당 오브젝트가 보여지는 sector거리
	TYPE_VIEWSECTORDISTANCE			= 16	,
	TYPE_COLLISIONATOMIC			= 17	,	// 콜리전 전용 아토믹.
	TYPE_ALLOC_NUM					= 18	
};

// 0x00ff	타입이 들어감.
// 0xff00	속성이 플래그로 조합되어 들어감.

/*	//atomic에 AgcdRenderInfo로 붙습니다. 이제 AcuObject에서 퇴출..(2004.11.3)
enum ObjectRenderIndex
{
	RI_RTYPE			= 0	,
	RI_CB				= 1	,
	RI_BMODE			= 2	,
	RI_ISBILL			= 3	,
	// 0이 아님 shader모드임
	// RI_SHADER - 지형 atomic에선 0 - default path, 1 - shadow2 path로 쓰인다.(AcuRpMTexture에서 씀)
	RI_SHADER			= 4	,	
	RI_BEFORELOD		= 5	,
	RI_SCOUNT			= 6	,
	RI_SCOUNTEND		= 7	,
	RI_LEV1				= 8	,		// FadeIn LOD Level
	RI_LEV2				= 9	,		// FadeOut LOD Level
	RI_CB2				= 10,

	RI_CURTICK			= 14,
	RI_INTERSECTTICK	= 15,
	// Billboard로 진입시에 1 , 나올시에 0
	// 처음 추가됬을 때 tick
//	RI_ADDTICK			= 16,
	RI_ISBILLBOARDLEV	= 17,

	RI_ALLOC_NUM		= 18
};*/

// 이거 따로 만들어서 갖고 올수 있게 하자.. 그래서 새로 만들었다 ..SECTORMODEBACKUPNAME 
//RI_IT_SECTORX: 추가시의 sectorx(worldpos)
//RI_IT_SECTORZ: 추가시의 sectorz(worldpos)
//RI_IT_RADIUS: 추가시의 radius
	
// 1 - high(주인공) 2 - medium , 3 - low
/*enum WhatIsDI
{
	DI_SHADOWLEVEL			= 0,
	DI_DISTANCE				= 1,
	DI_DISTANCETICK			= 2,
	DI_MAXLODLEVEL			= 3,
	// 0 - none , 1 - mouse on , 2 - lock ...
	DI_TARGETINGSTATUS		= 4,
	DI_CAMERA_Z_INDEX		= 5,

	DI_ALLOC_NUM			= 6
};*/

typedef enum AcuObjectType
{
	// Type
	ACUOBJECT_TYPE_NONE					=	0x0000	,
	ACUOBJECT_TYPE_OBJECT				=	0x0001	,
	ACUOBJECT_TYPE_CHARACTER			=	0x0002	,
	ACUOBJECT_TYPE_ITEM					=	0x0003	,
	ACUOBJECT_TYPE_WORLDSECTOR			=	0x0004	,

	ACUOBJECT_TYPE_TYPEFILEDMASK		=	0x000f	,

	// 마고자 (2005-04-11 오후 4:50:40) : 지형에 M텍스쳐 파이프라인을 안타게 하는 플레그.
	ACUOBJECT_TYPE_NO_MTEXTURE			=	0x0010	,
	ACUOBJECT_TYPE_SECTOR_ROUGHMAP		=	0x0020	,	// 러프맵의 경우..

	ACUOBJECT_TYPE_RESERVED2			=	0x0040	,
	ACUOBJECT_TYPE_RESERVED3			=	0x0080	,
	ACUOBJECT_TYPE_OPTIONMASK			=	0x00f0	,

	// 포합가능한 플래그.. ( Property )
	ACUOBJECT_TYPE_BLOCKING				=	0x00000100	,
	ACUOBJECT_TYPE_RIDABLE				=	0x00000200	,
	ACUOBJECT_TYPE_NO_CAMERA_ALPHA		=	0x00000400	,
	ACUOBJECT_TYPE_USE_ALPHA			=	0x00000800	,
	ACUOBJECT_TYPE_USE_AMBIENT			=	0x00001000	,
	ACUOBJECT_TYPE_USE_LIGHT			=	0x00002000	,
	ACUOBJECT_TYPE_USE_PRE_LIGHT		=	0x00004000	,
	ACUOBJECT_TYPE_USE_FADE_IN_OUT		=	0x00008000	,
	ACUOBJECT_TYPE_IS_SYSTEM_OBJECT		=	0x00010000	,
	ACUOBJECT_TYPE_MOVE_INSECTOR		=	0x00020000	,
	ACUOBJECT_TYPE_NO_INTERSECTION		=	0x00040000	,
	ACUOBJECT_TYPE_WORLDADD				=	0x00080000	,
	ACUOBJECT_TYPE_OBJECTSHADOW			=	0x00100000	,
	ACUOBJECT_TYPE_RENDER_UDA			=	0x00200000	,
	ACUOBJECT_TYPE_USE_ALPHAFUNC		=	0x00400000	,
	ACUOBJECT_TYPE_OCCLUDER				=	0x00800000	,
	ACUOBJECT_TYPE_DUNGEON_STRUCTURE	=	0x01000000	,
	ACUOBJECT_TYPE_DUNGEON_DOME			=	0x02000000	,	// 던젼 천장
	ACUOBJECT_TYPE_CAM_ZOOM				=	0x04000000	,	// 카메라충돌 카메라줌.
	ACUOBJECT_TYPE_CAM_ALPHA			=	0x08000000	,	// 카메라충돌 오브젝트 알파처리.
	//@{ Jaewon 20050912
	// For the 'invisible' skill
	ACUOBJECT_TYPE_INVISIBLE			=   0x10000000	,
	//@} Jaewon
	//@{ 2006/09/08 burumal
	ACUOBJECT_TYPE_FORCED_RENDER_EFFECT	=   0x20000000	,
	//@}
	ACUOBJECT_TYPE_DONOT_CULL			=	0x40000000	,	// 무조건 렌더한다.. 사라지는 녀석 대비용..
	ACUOBJECT_TYPE_PROPERTY_FILTER		=	0xffffff00
} AcuObjectType;

// ACUOBJECT_TYPE_MOVEINSECTOR : sector내에서만 이동이 일어나는 것(ex. 파도) 다시 말해 RwFrame의 position이 계속 변경되는것
// ACUOBJECT_TYPE_NO_INTERSECTION : Ray intersection에서 걸릴 필요가 없는 객체들(ex.풀,파도)
// ACUOBJECT_TYPE_WORLDADD : RpWorld에 추가되어야 하는 이동 객체들

typedef struct
{
	INT32			eType			;
	INT32			lID				;
	PVOID			pvObject		;
	PVOID			pvTemplate		;
	PVOID			pvCustData		;
	PVOID			pvPickAtomic	;
} AcuObjectTypeData;

#define				BILLARRAYNUM	10

typedef struct tag_IsBill
{
	int				num;
	int				isbill[BILLARRAYNUM];
}IsBillData;

#define				DATABACKUPSIZE	1572
typedef struct tag_UserDataBackup
{
	PVOID				data[64];
	char				strdata[25][50];
	INT32				numelements;
	RpUserDataFormat	format;
	char				name[52];

	tag_UserDataBackup*		next;
}UserDataBackup;

#define				ALLOC_FRAME_MEM_ACUOBJECT		6000000

#define				ALLOC_OCTREE_ID_TEST_UDA_NUM	1501

class AcuObject  
{
private:
	// 2005.4.12 gemani 이제 메모리 alloc할일 없다.. 
	//static void*			AllocFrameMemory(INT32 nBytes);
	//static void				DeallocFrameMemory(INT32	nBytes);	// 생존기간이 block내 인경우 block끝나는 시점에서 해제해주자

	//static INT32			m_iByteAllocated;			// debug 버전에서만 check
	//static INT32			m_iCurAllocated;
	//static UINT8*			m_pMemoryBlock;
	//static UINT8*			m_pFrame;

public:
	//static ApCriticalSection	m_csMutexFrameMemory;

	AcuObject();
	virtual ~AcuObject();

	static CHAR *			m_szName;
	static RwCamera *		m_pstCamera;

	// 인덱스 분석하는 펑션..
	static INT32			GetType			( int index );
	static INT32			GetProperty		( int index );

	// Atomic의 Type과 ID를 Get/Set 한다.
	static VOID				SetAtomicType	(
		RpAtomic *pstAtomic			,
		INT32 eType					,
		INT32 lID					,
		PVOID pvObject = NULL		,
		PVOID pvTemplate = NULL		,
		PVOID pvCustData = NULL		,
		PVOID pvPickAtomic = NULL	,
		RwSphere*	sphere = NULL	,
		PVOID pvOcTreeData = NULL	,
		PVOID pvOcTreeIDList = NULL
		);
	
	static INT32			GetAtomicType	( RpAtomic *pstAtomic	, INT32 *plID			, PVOID *ppvObject = NULL	, PVOID *ppvTemplate = NULL		, PVOID *ppvCustData	= NULL	, PVOID *ppvPickAtomic	= NULL);

	static RpAtomic *		GetAtomicGetCollisionAtomic	( RpAtomic * pstAtomic );
	static RpAtomic *		GetAtomicGetPickingAtomic	( RpAtomic * pstAtomic );

	// Clump의 Type과 ID를 Get/Set 한다. (CustData는 LODDistance를 가져올 때 쓰인다.)
	static INT32			GetClumpType	(
		RpClump* pstClump,
		INT32* plID,
		PVOID* ppvObject = NULL,
		PVOID* ppvTemplate = NULL,
		PVOID* ppvCustData = NULL,
		PVOID* ppvPickAtomic = NULL);

	// 각각 정보를 리턴하는 펑션..
	//static RpAtomic *		GetClumpGetCollisionAtomic	( RpClump * pstClump );		//clump는 직접 AgcdType직접 참조..
	//static RpAtomic *		GetClumpGetPickingAtomic	( RpClump * pstClump );		//clump는 직접 AgcdType직접 참조..

	static VOID				SetClumpType	(
		RpClump *pstClump							,
		INT32 eType									, 
		INT32 lID									,
		PVOID			pvObject			= NULL	,
		PVOID			pvTemplate			= NULL	,
		PVOID			pvCustData			= NULL	,
		RpAtomic *		pvPickAtomic		= NULL	,
		RwSphere *		pvSphere			= NULL	,
		RpAtomic *		pvCollisionAtomic	= NULL	,
		PVOID			pvOctreeData		= NULL	,
		PVOID			pvOcTreeIDList		= NULL );

	static VOID				SetClumpType	( RpClump *pstClump		, INT32 eType );
	
	//static INT32*			GetClumpTypeArray ( RpClump *pClump );		//clump는 직접 AgcdType직접 참조..
	static AgcdType*		GetAtomicTypeStruct ( RpAtomic *pAtomic );

	//static VOID			GetClumpBSphere ( RpClump *pClump , RwSphere* sphere);	//clump는 직접 AgcdType직접 참조..
	static VOID				GetAtomicBSphere ( RpAtomic *pAtomic , RwSphere* sphere);

	// Add By gemani(03.06.26)
	static VOID				SetClumpDInfo_Distance	( RpClump *pstClump, float Distance, UINT32 UpdateTick);
	static VOID				SetClumpDInfo_Shadow	( RpClump *pstClump, int	ShadowLevel);
	static VOID				SetClumpDInfo_LODLevel	( RpClump *pstClump, int	LODLevel);
	//static INT32*			GetClumpDInfo	( RpClump *pstClump	);
	
	// Atomic과 Clump의 User Data를 제거한다.
	static RpAtomic			*DestroyAtomicDataCB(RpAtomic *atomic, void *data);
	static VOID				DestroyAtomicData	( RpAtomic *pstAtomic	);
	static VOID				DestroyClumpData	( RpClump *pstClump		);

	// Geometry의 Prelight를 setting한다.
	static RpGeometry		*SetGeometryPreLitLum(RpGeometry *pstGeometry, RwRGBA *pstPreLitLum); // 030303 Bob님 추가
	static RpGeometry *		CBSetGeometryPreLitLum	( RpGeometry *pstGeometry, PVOID pvData );
	static VOID				SetAtomicPreLitLim		( RpAtomic *pstAtomic	, RwRGBA *pstPreLitLum	);
	static RpAtomic *		CBSetAtomicPreLitLim	( RpAtomic *pstAtomic	, PVOID pvPreLitLum	);
	static VOID				SetClumpPreLitLim		( RpClump *pstClump		, RwRGBA *pstPreLitLum	);

	// Geometry를 Instancing한다.
	static BOOL				InstanceAtomic	( RpAtomic *pstAtomic	);
	static RpAtomic *		CBInstanceAtomic( RpAtomic *pstAtomic, PVOID pvData );
	static BOOL				InstanceClump	( RpClump *pstClump		);

	static VOID				SetCamera		(RwCamera *pstCamera	);

//	static VOID				SetAtomicInFrustumTime	(RpAtomic *pstAtomic, UINT32 ulInFrustumTime);
//	static UINT32			GetAtomicInFrustumTime	(RpAtomic *pstAtomic);

	// Add by TaiHyung 03.4.17
	static VOID				SetAtomicRenderUDA(RpAtomic *pstAtomic,UINT32 rendertype,
			RpAtomicCallBackRender cbRender,INT32	BMode,BOOL bBill,INT32 shadertype);

	/*static INT32*			GetAtomicRenderArray (RpAtomic *pstAtomic);		// 이걸루 주로 처리하자
	static INT32*			GetClumpRenderArray(RpClump *pstClump);		// Clump에 붙어있는 renderarray를 가져온다(fade in/out시 refcount계산시 쓰임)

	static RpAtomicCallBackRender	GetAtomicRenderCallBack(RpAtomic *pstAtomic);
*/
	static float*			MakeAtomicBackFrameInfo(RpAtomic *pstAtomic);		// billboard처리후 돌아갈때 쓸 frame
	static float*			GetAtomicBackFrameInfo(RpAtomic *pstAtomic);

	static float*			MakeAtomicSectorModeBackup(RpAtomic *pstAtomic);	//  render sectormode시 save용
	static float*			GetAtomicSectorModeBackup(RpAtomic *pstAtomic);

	static VOID				SetClumpRenderInFrustum(RpClump *pstClump, PVOID pvData);
	static RpAtomic*		CBSetRenderInFrustum(RpAtomic *pstAtomic	, PVOID pvData);
	static VOID				SetAtomicRenderInFrustum(RpAtomic *pstAtomic, PVOID pvData);

	// LODAtomic별로 Geometry에 Billboard 여부 Set 
	static VOID				SetClumpIsBillboard(RpClump *pstClump, PVOID pvData);
	static RpAtomic*		CBSetIsBillboard(RpAtomic* pstAtomic, PVOID pvData);

	static VOID				SetGeometryIsBillboard(RpGeometry* pstGeom,int isBill); // 0 - false, 1 - true
	static int 				GetAtomicIsBillboard(RpAtomic* pstAtomic);

	static VOID				SetClumpMaterialAlphaGeometryFlagOff(RpClump *clump);
	static RpAtomic*		CBSetAtomicMaterialAlphaGeometryFlagOff( RpAtomic* atomic, void *data );		//2003_03_13 Seong Yon-jun	// Modify 2003_06_20 Bob.
	static RpGeometry*		CBSetGeometryMaterialAlphaFlagOff(RpGeometry *geom, void *data);
	static RpGeometry*		SetGeometryMaterialAlphaFlagOff(RpGeometry *geom);

	static void				DestroyAtomicUD(RpAtomic*	pAtomic);

	static	RwFrame*			CB_GetHeirarchy( RwFrame* lpFrm, void* lplpHierarch );
	static	RpHAnimHierarchy*	GetHierarchyFromFrame( RwFrame* pFrm );
	static	RpAtomic*			SetHierarchyForSkinAtomic( RpAtomic* atomic, void* data );

	// 이제 안쓰임..
	//	static RpGeometry *		CBSetGeometryData( RpGeometry *pstGeometry	, PVOID pvData		);
	//	static RpAtomic *		CBSetAtomicForGT	( RpAtomic *pstAtomic	, PVOID pvData			);

	//static void				SetDefaultClumpFrameUD(RwFrame*	frame);			// clump의 frame 
	//static void				SetDefaultAtomicGeometryUD(RpGeometry* pGeom,INT32	bClump);		// Clump가 있는지여부
	//static void				SetDefaultAtomicFrameUD(RwFrame* pFrame);
	//static void				RearrangeAtomicUserdata(RpAtomic*	atomic);
	//static void				RearrangeClumpUserdata(RpClump*		clump);
	//static	RpAtomic*		CBRearrangeUD(RpAtomic*	pAtomic,PVOID pvData);

	// 있으면 리턴 없으면 만들어서 리턴,, 맵툴 옥트리 id저장을 위해 임시생성 되는 userdata이다! (gemani)
	// 총 1201 크기의 정수 배열 .. 400개의 octreeid를 저장한다.
	// totalID 가 0이면 계산 안된것임..(최소 1)
	// 0번째 index는 totalID , 
	//<1> 1: sectoriX , 2: sectoriZ, 3: OctreeID	//<2> 4: sectoriX , 5: sectoriZ, 6: OctreeID 
	//<3> 7: sectoriX , 8: sectoriZ, 9: OctreeID	//<4> 10: sectoriX , 11: sectoriZ, 12: OctreeID
	//<5> 13: sectoriX , 14: sectoriZ, 15: OctreeID	//<6> 16: sectoriX , 17: sectoriZ, 18: OctreeID
	//<7> 19: sectoriX , 20: sectoriZ, 21: OctreeID	//<8> 22: sectoriX , 23: sectoriZ, 24: OctreeID
	//<9> 25: sectoriX , 26: sectoriZ, 27: OctreeID	//<10> 28: sectoriX , 29: sectoriZ, 30: OctreeID
	// ... 이하 생략
	static	INT32*			GetOcTreeID(RpClump*	clump);

	//@{ kday 20051013
	// ;)
	// 각 타일에 포함되는 컬리전&라이더블 오브젝트의 삼각형인덱스를 뽑아내려구 만듬.
	typedef std::vector<INT32>	CONTAINER_TRIINDEX;
	static INT32			GetAllTriIndexThatIsInTile( CONTAINER_TRIINDEX& container, RpAtomic* atom
													  , const RwV3d& leftTop, const RwV3d& rightBottom);
	static INT32			IntsctLineTriInAtomic( RpAtomic* atom
												, const CONTAINER_TRIINDEX& triindices
												, const RwLine& line
												, BOOL	nearest = FALSE
												, RwReal* dist = NULL
												);
	//@} kday
};

#endif // !defined(AFX_ACUOBJECT_H__1801AA47_8F43_40BC_9B0C_2C7F1FD02115__INCLUDED_)
