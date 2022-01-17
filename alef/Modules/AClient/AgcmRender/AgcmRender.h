#ifndef	_AGCMRENDER_H_
#define	_AGCMRENDER_H_

#include "ApModuleStream.h"
#include "rwcore.h"
#include "rpworld.h"
#include "rpskin.h"

#include "rptoon.h"
#include "rttoon.h"

#include "ApBase.h"
#include "AcuObject.h"
#include "AcuFrameMemory.h"
#include "AgcmRenderType.h"

#include "ApmMap.h"
#include "ApmOcTree.h"
#include "AgcmOcTree.h"
#include "AgcmPostFX.h"
#include "AcDefine.h"
#include "effect.h"
#include "ContainerUtil.h"

#include <set>
#include <list>
#include <map>
using namespace std;

#include <d3dx9.h>

// SetDistance관련 주의!! rendercallback에서 SetDistance를 안해준다.(카메라에 들어온다음 set하면 clump의 첫번째 atomic은
// 항상 전프레임의 거리를 가져서 SetDistance 호출 장소 이동이 필요했습니다.

#define				R_HASH_SIZE						7		// 소수일 것.. 이제부터 alpha가진 객체들은 hash를 안가진다(텍스쳐 별로 묶는게 의미가 없으므로)

// 일단 0번만 sort한다
#define				R_SORT_BLEND_MODE				9
#define				R_NOT_SORT_BLEND_MODE			6

// LOD가 없을 경우 최대Sector 거리로 set
#define				R_MAX_SECTOR_DISTANCE			99

#define				OCTREE_Z_SCALE					18

// 추가시에
enum enumAlphaOption
{
	CHECK_ALPHA,		//Alpha여부를 Check한다 
	ONLY_NONALPHA,		//무조건 Non Alpha로 분류한다: nonalpha는 알파가 없거나 매우 작은 부분에 사용되기 때문에 순서가 크게 상관 없을때
	ONLY_ALPHA,			//무조건 Alpha로 분류한다 : alpha는 많은 부분에 알파가 쓰여서 소팅을 해줘야 할때
	ONLY_BLEND,			//ZWrite를 안하는 Alpha객체들(effect같은 것들)
	ONLY_AMBIENT,		//무조건 AMBIENT LIGHT변경 .. NonAlpha이다
	ONLY_ALPHAFUNC,
	ONLY_LASTZWRITE		//마지막으로 ZWrite하며 그리는 것들 (normal water.. )
};

// 내부 핸들링용 
enum enumRenderType
{
	R_DEFAULT,
	R_NONALPHA,
	R_ALPHA,
	R_BLEND_SORT,
	R_BLEND_NSORT,
	R_AMBIENT,
	R_ALPHAFUNC,
	R_SKIN,
	R_LASTZWRITE,
	R_RENDER_TYPE_NUM
};

enum enumRenderBMode
{
	R_NONE = -1,
	R_BLENDSRCAINVA_ADD,		// sort 필요
	R_BLENDSRCAINVA_REVSUB,
	R_BLENDSRCAINVA_SUB,
	R_TEXTURE_STAGE_ADD,		
	R_TEXTURE_STAGE_REVSUB,
	R_TEXTURE_STAGE_SUB,
	R_BLENDSRCCINVC_ADD,
	R_BLENDSRCCINVC_REVSUB,
	R_BLENDSRCCINVC_SUB,
	R_BLEND11_ADD,				// 여기서부턴 sort불필요
	R_BLEND11_REVSUB,
	R_BLEND11_SUB,
	R_BLENDSRCA1_ADD,
	R_BLENDSRCA1_REVSUB,
	R_BLENDSRCA1_SUB,
	R_BELND_MODE_NUM
};

enum enumAddOption
{
	AGCMRENDER_ADD_NONE		= 0x00,
	AGCMRENDER_ADD_FADEIN	= 0x01
};

enum AgcmRenderCallbackPoint
{
	AGCMRENDER_CB_ID_PRERENDER		= 0,
	AGCMRENDER_CB_ID_POSTRENDER,
	AGCMRENDER_CB_ID_POSTRENDER2,
	AGCMRENDER_CB_ID_POSTRENDER_OCTREE_CLEAR,			//octree 지평선 체크 list 지우고 특정 camera로 지평선 체크 다시 하기 위해..(물 반사 텍스쳐 렌더링시 쓰임..)
	AGCMRENDER_CB_ID_REMOVEATOMIC,
	AGCMRENDER_CB_ID_REMOVECLUMP,
	AGCMRENDER_CB_ID_ADDATOMIC,
	AGCMRENDER_CB_ID_ADDCLUMP,
	AGCMRENDER_CB_OCTREE_UDA_SET,
	AGCMRENDER_CB_OCTREE_ID_SET,
	AGCMRENDER_CB_MTEXTURE_RENDER,
	AGCMRENDER_CB_CUSTOM_RENDER,		 //.edge rendering을 위해 설정.

};

enum enumLightingType
{
	LIGHT_NONE			=0,
	LIGHT_LOGIN_SELECT,
	LIGHT_IN_SHADOW,
	LIGHT_QUEST_OBJECT,
	LIGHT_STATUS_STUN,
	LIGHT_QUEST_FREEZE,			//. special status color 추가.
	LIGHT_QUEST_POISON
};

//These are bit flags which can be set in an atomic's stRenderInfo.backupCB3 and is used for various temporary special renderings
enum eSpecialRenderMode
{
	eSpecialRenderFade		= 0x00000001,
	eSpecialRenderUVAni		= 0x00000002,
	eSpecialRenderInvisible	= 0x00000004,
};

enum	VSHADER_TYPE		// VShader Atomics
{
	VS_NONE				= 0,
	VS_FADEIN_A			= 1,
	VS_FADEOUT_A		= 11,
	VS_FADEINOUT_A		= 21,
	VS_JUST_TRANSPARENT	= 31
};

enum	BLEND_SORT_TYPE		// Blend Atomics
{
	BS_FADEINOUT	= 30,
	BS_CUSTOM		= 50,
	BS_LASTDRAWING	= 70
};

struct CustomRenderList : public ApMemory<CustomRenderList, 500>
{
	ApModuleDefaultCallBack		CustUpdateCB;
	ApModuleDefaultCallBack		CustRenderCB;

	RwSphere*					BS;
	
	PVOID						data1;		// ApModuleDefaultCallBack 의 pData로 보내진다 
	PVOID						pClass;		// ApModuleDefaultCallBack 의 class로 보내진다
	PVOID						data2;		// ApModuleDefaultCallBack 의 pCustData로 보내진다 

	FLOAT						height;
	BOOL						bCheck;

	CustomRenderList*			next;
};

// World는 이제 퇴출이다 .. intersection도 render에서 해준다.
struct WorldAddClumpGroup : public ApMemory<WorldAddClumpGroup, 3000>
{
	RpClump*					pClump;
	// OcClumpAtomicList -> RpAtomic
	// This is valid only when pClump is NULL.
	RpAtomic*					Atomiclist;		

	RwSphere					BS , WorldBS ;
	RwV3d						MaxBBox[4];

	WorldAddClumpGroup*			next;
	WorldAddClumpGroup*			prev;
};

// Texture Sort(기존 Nonalpha,Ambient,AlphaFunc,NotSortBlend)와 SimpleAtoimc(그외의 것들로 구조체형 통합
#define		TEXTURE_SORT_ATOMIC_SIZE				20
struct	TextureSortAtomic
{
	RpAtomic*		atomic;
	DWORD			texaddr;
	int				billboard;			// 0 - none,1-빌보드,2-y축 빌보드

	TextureSortAtomic*		sibling;
	TextureSortAtomic*		next;
};

struct	TextureSortAtomicList
{
	TextureSortAtomic*	hash_table[R_HASH_SIZE];
};

struct NotSortBlendAtomicList
{
	TextureSortAtomic*	hash_table[R_NOT_SORT_BLEND_MODE][3];	// 해쉬 사이즈 적게 쓰자
};

#define		ALPHA_ATOMIC_SIZE						16
struct AlphaAtomic
{
	RpAtomic*		atomic;
	float			SortFactor;
	int				billboard;						// 0 - none,1-빌보드,2-y축 빌보드

};

#define		mmapAlphaAtomic			std::multimap< FLOAT , AlphaAtomic , greater<FLOAT> >
#define		mmapAlphaAtomicIter		std::multimap< FLOAT , AlphaAtomic , greater<FLOAT> >::iterator

#define		SIMPLE_ATOMIC_SIZE						8
struct SimpleAtomic
{
	RpAtomic*				atomic;

	SimpleAtomic			*next;			
};

#define		VSHADER_FADE_ATOMIC_SIZE				12
struct	VShaderFadeAtomic
{
	INT32						shader_type;
	float						alpha;

	BOOL						bHook;
};

#define		BLEND_ATOMIC_SIZE						28

struct stBlendAtomic
{
	RpAtomic*												m_pAtomic;

	float													m_fSortFactor;
	int														m_nBillBoardType;

	void*													m_pClass;
	void*													m_pData1;
	void*													m_pData2;

	stBlendAtomic( void )
	{
		m_pAtomic = NULL;

		m_fSortFactor = 0.0f;
		m_nBillBoardType = 0;

		m_pClass = NULL;
		m_pData1 = NULL;
		m_pData2 = NULL;
	}

	void						Copy						( stBlendAtomic* pOriginal )
	{
		if( !pOriginal ) return;

		m_pAtomic = pOriginal->m_pAtomic;

		m_fSortFactor = pOriginal->m_fSortFactor;
		m_nBillBoardType = pOriginal->m_nBillBoardType;

		m_pClass = pOriginal->m_pClass;
		m_pData1 = pOriginal->m_pData1;
		m_pData2 = pOriginal->m_pData2;
	}
};

struct stBlendAtomicInfo
{
	enumRenderBMode											m_eBlendMode;
	typedef multimap< FLOAT , stBlendAtomic , greater< FLOAT > > Atomics;
	Atomics atomics_;

	stBlendAtomicInfo( void )
	{
		m_eBlendMode = R_NONE;
	}

	void						Add							( stBlendAtomic & BlendAtomic );
	void						_SortAtomicsBySortFactor	( void );
	FLOAT						_AtomicGetCameraDistance	( RpAtomic *pAtomic );

	INT32						GetCount					( void ) { return atomics_.size(); }
	/*
	stBlendAtomic*				GetAtomicByIndex			( int nIndex ) 
	{ 
		multimap< FLOAT , stBlendAtomic >::iterator		Iter	=	m_mapAtomics.begin();

		for(  INT i = 0 ; Iter != m_mapAtomics.end()  ; ++Iter , ++i )
		{
			if( i == nIndex )
			{
				return &Iter->second;
			}
		}

		return NULL;
	}*/

private:
	stBlendAtomicInfo( stBlendAtomicInfo const & other );
	void operator=( stBlendAtomicInfo const & other );
};

struct stBlendAtomicEntry
{
	ContainerMap< enumRenderBMode, stBlendAtomicInfo* >		m_mapBlendAtomics;
	stBlendAtomicInfo										m_LastBlendAtomic;
	
	stBlendAtomicEntry( void )
	{
		Clear();
	}

	void						Clear						( void );
	int							GetCount					( void ) { return m_mapBlendAtomics.GetSize(); }
	stBlendAtomicInfo*			GetBlendAtomicByRenderMode	( enumRenderBMode eRenderMode ) { stBlendAtomicInfo ** result = m_mapBlendAtomics.Get( eRenderMode ); return result && *result ? *result : 0; }
	void						Add							( enumRenderBMode eRenderMode, stBlendAtomic BlendAtomic );
	void						AddLast						( stBlendAtomic BlendAtomic ) {	m_LastBlendAtomic.Add( BlendAtomic ); }
};

#define		LIGHTING_ATOMIC_SIZE					16
struct LightingAtomic
{
	RpAtomic*	atomic;
	DWORD		texaddr;
	
	LightingAtomic*		sibling;										// 같은 texture들 연결
	LightingAtomic*		next;				
};	

struct LightingAtomicList
{
	LightingAtomic*			hash_table[R_HASH_SIZE];
};	

#define		SKINNING_ATOMIC_SIZE			8
struct SkinningAtomic
{
	RpAtomic*				atomic;	
	SkinningAtomic*		next;
};

#define		SKINNING_TSORT_ATOMIC_SIZE		16
struct SkinningTSortAtomic
{
	RpAtomic*	atomic;
	DWORD		texaddr;

	SkinningTSortAtomic*	sibling;
	SkinningTSortAtomic*	next;
};

struct SkinningAtomicList
{
	SkinningTSortAtomic*		hash_table[R_HASH_SIZE];
};

// UV Animation (Texture Change) Atomics -----------
#define		UVANIM_ATOMIC_SIZE				8
struct UVAnimAtomic
{
	RpAtomic*				atomic;
	UVAnimAtomic*		next;
};

// shadow pass 타기 위해 묶어둔다(uda 변경)
#define SHADOW2_TERRAIN_LIST_SIZE		16
struct Shadow2TerrainList
{
	RpAtomic*					atomic;
	BOOL						bShadow;									// shadow2 range안에 드는지..
	int							sectordist;
	Shadow2TerrainList*		next;
};

//----------------------  clump와 atomic에 붙는 update node --------------------------- 
// 
struct UpdateCallbackList	: public ApMemory<UpdateCallbackList, 40000>
{
	ApModuleDefaultCallBack		pRenderCB;
	ApModuleDefaultCallBack		UpdateCB;
	ApModuleDefaultCallBack		DestructCB;

	PVOID						data1;										// ApModuleDefaultCallBack 의 pData로 보내진다 
	PVOID						pClass;										// ApModuleDefaultCallBack 의 class로 보내진다
	PVOID						data2;										// ApModuleDefaultCallBack 의 pCustData로 보내진다 
	RwSphere*					pSphere;									// Bounding Sphere

};

typedef std::list< UpdateCallbackList* >					ListUpdateCallback;
typedef std::list< UpdateCallbackList* >::iterator		ListUpdateCallbackIter;

//----------------------  OCTREE --------------------------- 
enum	RENDER_PRERENDER_TEST_TYPE
{
	PRE_TEST_NORMAL		= 0,	// 프러스텀 체크 해주는 평범한 것들
	PRE_TEST_NOT_TEST,			// 지형 같이 프러스텀 체크가 필요 없는 것들 
	PRE_TEST_CUSTOM	,			// atomic 단위로 추가 되지 않은 effect나 풀들
	PRE_TEST_CUSTOM_NOT_TEST	// 비나 눈..  지평선 체크 X
};

// PreRenderList -> std::priority_queue<PreRender>
#include <queue>
class PreRender		// 카메라near에서부터 정렬
{
public:
	// Effect, 풀에선 아래 4개의 data가 다음과 같이 쓰입니다. 숙지!!..customrender
	// atomiclist = pClass, isOccluder = UpdateCB , reserve1 = RenderCB, nOccluder = pData1 , arrayOccluder = pData2 
	// OcClumpAtomicList -> RpAtomic
	RpAtomic*					atomicList;			// terrain이나 world에 추가된것들은 1개, object는 atomiclist시작 위치
	BOOL						isOccluder;
	INT32						nOccluder;
	FLOAT*						arrayOccluder;		// index+0 - left, index+1 - right, index+2 - top, index+3 - z

	PVOID						reserve1;			

	INT32						TestType;			// RENDER_PRERENDER_TEST_TYPE 을 가진다.
	FLOAT						maxBBox[4];			// 0 - left, 1 - right, 2 - top, 3 - z(가장 먼값, 이값으로 camera sort한다)
	RwSphere					Sphere;			

	FLOAT						dOnXZ;				// xz plane distance for sorting
};
struct lessPreRender : public binary_function<PreRender*, PreRender*, bool>
{
	result_type operator()(first_argument_type lhs, second_argument_type rhs) const
	{
		return rhs->dOnXZ < lhs->dOnXZ;
	}
};

#define OCCLUDER_LINE_LIST_SIZE			16
struct OccluderLineList
{
	INT32				startx , endx , y;
	OccluderLineList*	next;
};

// Spot, Point Light 관리!
struct LocalLight
{
	RpLight*		pLight;
	RwSphere		BS;								// local space

	BOOL			bMove;
	RwSphere		WorldBS;

	INT32			iCameraIndex;

	LocalLight*		next;
};

#define PROCESS_LIGHT_SIZE			8
struct ProcessLight
{
	LocalLight*			pLight;
	ProcessLight*		next;
};

#define PROCESS_SORT_LIGHT_SIZE			12
struct ProcessSortLight
{
	RpLight*			pLight;
	FLOAT				distFactor;
	ProcessSortLight*	next;
};

typedef void (*AgcmRenderTestRenderAtomicList) (RpAtomic* pList);		// OcClumpAtomicList -> RpAtomic
typedef void (*AgcmRenderTestRenderAtomic) (RpAtomic* atomic);
typedef void (*AgcmRenderTestRenderTerrain) (RpAtomic* atomic, INT32 SectorDist);

//----------------------  Sector Attach Data --------------------------- 
enum eAddSectorMode
{
	eAddSectorInSectorStatic,
	eAddSectorInSectorMove,
	eAddSectorNoInSectorStatic,
	eAddSectorNoInSectorMove,
	eAddSectorMax
};

typedef std::list< RpAtomic* >				RpAtomicList;
typedef RpAtomicList::iterator				RpAtomicListItr;
typedef RpAtomicList::const_iterator		RpAtomicListCItr;

struct WorldSphere
{
	WorldSphere( RpAtomic* p ) : atomic(p)	{
		RwV3dTransformPoint( &cSphere.center, &atomic->boundingSphere.center, RwFrameGetLTM( RpAtomicGetFrame(atomic) ) );
		cSphere.radius = atomic->boundingSphere.radius;
	}

	RpAtomic*	atomic;
	RwSphere	cSphere;

	bool	operator == ( RpAtomic* pAtomic )	{
		return memcmp( atomic, pAtomic, sizeof( RpAtomic ) ) ? false : true;
	}
};
typedef std::list< WorldSphere >			WorldSphereList;
typedef WorldSphereList::iterator			WorldSphereListItr;
typedef WorldSphereList::const_iterator		WorldSphereListCItr;

typedef std::list< LocalLight* >			LocalLightList;
typedef LocalLightList::iterator			LocalLightListItr;
typedef LocalLightList::const_iterator		LocalLightListCItr;

struct CustomRenderSector			// Sector모드용
{
	CustomRenderSector( ApModuleDefaultCallBack update, ApModuleDefaultCallBack render, RwSphere* sphere, PVOID pClass, PVOID pData1, PVOID pData2, INT32 sectorDist )
	{
		CustUpdateCB	= update;
		CustRenderCB	= render;
		BS				= sphere;
		this->pClass	= pClass;
		data1			= pData1;
		data2			= pData2;
		iSectorDistance	= sectorDist;
	}

	ApModuleDefaultCallBack		CustUpdateCB;
	ApModuleDefaultCallBack		CustRenderCB;
	RwSphere*	BS;
	PVOID		pClass;
	PVOID		data1;
	PVOID		data2;	
	INT32		iSectorDistance;	// 출현 섹터 
};
typedef std::list< CustomRenderSector >			CustomRenderSectorList;
typedef CustomRenderSectorList::iterator		CustomRenderSectorListItr;
typedef CustomRenderSectorList::const_iterator	CustomRenderSectorListCItr;

struct	SectorRenderList
{
	RpAtomic*				terrain;									// sector atomic 포인터(NULL이면 없음)

	WorldSphereList*		listInStatic;
	RpAtomicList*			listInMove;
	WorldSphereList*		listNotInStatic;
	RpAtomicList*			listNotInMove;
	LocalLightList*			listLocalLight;		// A list of local lights 
	CustomRenderSectorList*	listCustomRenderSector;

	void	Init()
	{
		terrain			= NULL;
		listInStatic	= new WorldSphereList;
		listInStatic->clear();
		listInMove		= new RpAtomicList;
		listInMove->clear();
		listNotInStatic	= new WorldSphereList;
		listNotInStatic->clear();
		listNotInMove	= new RpAtomicList;
		listNotInMove->clear();
		listLocalLight	= new LocalLightList;
		listLocalLight->clear();
		listCustomRenderSector = new CustomRenderSectorList;
		listCustomRenderSector->clear();
	}

	void	Clear()
	{
		terrain		= NULL;

		if( listInStatic )
		{
			listInStatic->clear();
			delete listInStatic;
			listInStatic = NULL;
		}

		if( listInMove )
		{
			listInMove->clear();
			delete listInMove;
			listInMove = NULL;
		}

		if( listNotInStatic )
		{
			listNotInStatic->clear();
			delete listNotInStatic;
			listNotInStatic = NULL;
		}

		if( listNotInMove )
		{
			listNotInMove->clear();
			delete listNotInMove;
			listNotInMove = NULL;
		}

		if( listLocalLight )
		{
			listLocalLight->clear();
			delete listLocalLight;
			listLocalLight = NULL;
		}

		if( listCustomRenderSector )
		{
			listCustomRenderSector->clear();
			delete listCustomRenderSector;
			listCustomRenderSector = NULL;
		}
	}

	void	Add( eAddSectorMode eMode, RpAtomic* atomic )
	{
		switch( eMode )
		{
		case eAddSectorInSectorStatic:
			listInStatic->push_back( WorldSphere( atomic ) );
			break;
		case eAddSectorNoInSectorStatic:
			listNotInStatic->push_back( WorldSphere( atomic ) );
			break;
		case eAddSectorInSectorMove:
			listInMove->push_back( atomic );
			break;
		case eAddSectorNoInSectorMove:
			listNotInMove->push_back( atomic );
			break;
		}
	}

	void	Delete( eAddSectorMode eMode, RpAtomic* atomic )
	{
		switch( eMode )
		{
		case eAddSectorInSectorStatic:
			{
				WorldSphereListItr find = std::find( listInStatic->begin(), listInStatic->end(), atomic );
				if( find != listInStatic->end() )
					listInStatic->erase( find );
			}
			break;
		case eAddSectorInSectorMove:
			{
				RpAtomicListItr find = std::find( listInMove->begin(), listInMove->end(), atomic );
				if( find != listInMove->end() )
					listInMove->erase( find );
			}
			break;
		case eAddSectorNoInSectorStatic:
			{
				WorldSphereListItr find = std::find( listNotInStatic->begin(), listNotInStatic->end(), atomic );
				if( find != listNotInStatic->end() )
					listNotInStatic->erase( find );
			}
			break;
		case eAddSectorNoInSectorMove:
			{
				RpAtomicListItr find = std::find( listNotInMove->begin(), listNotInMove->end(), atomic );
				if( find != listNotInMove->end() )
					listNotInMove->erase( find );
			}
			break;
		}
	}
};

struct SectorData
{
	LocalLightList*		listLocalLight;
};

//---------------- CRenderFadeInOut ------------------
class CRenderFadeInOut
{
public:
	CRenderFadeInOut()					{		}
	virtual  ~CRenderFadeInOut()		{		}

	void	StartFadeInOut( RpAtomic* atomic, int level1, int level2, UINT32 endtick = 3000 );
	void	StartFadeIn( RpAtomic* atomic, int level, UINT32 endtick = 3000 );
	void	StartFadeOut( RpAtomic* atomic, int level, UINT32 endtick = 3000 );

	void	StartFadeInClump( RpClump* pClump, int level, UINT32 endtick = 3000 );		// clump단위로 set(캐릭터 갑옷중에 lod없는 것이 있어 fadein안타는 것이 있다..)
	void	StartFadeOutClump( RpClump* pClump, int level, UINT32 endtick = 3000 );

	void	StartTransparentClump( RpClump* pClump, int alpha);
	void	ChangeTransparentValue( RpClump* pClump, int alpha );
	void	EndTransparentClump( RpClump* pClump );
	
	void	InvisibleFxBegin( RpClump* pClump, RwUInt8 alpha, RwUInt16 fadeOutTiming = 0 );
	void	InvisibleFxEnd( RpClump* pClump );

	static	RpAtomic* CallBackSetFadeIn( RpAtomic* atomic, void* data );
	static	RpAtomic* CallBackSetFadeOut (RpAtomic* atomic, void* data );
	static	RpAtomic* CallBackSetStartTransparent( RpAtomic* atomic,void* data );
	static	RpAtomic* CallBackSetEndTransparent( RpAtomic* atomic,void* data );
	static	RpAtomic* CallBackChangeTransparent( RpAtomic* atomic,void* data );
	static	RpAtomic* CallBackInvisibleFxBegin( RpAtomic* atomic,void* data );
	static	RpAtomic* CallBackInvisibleFxEnd( RpAtomic* atomic,void* data );
};

//---------------- CUsableRwFrameTable ------------------
class CUsableRwFrameTable
{
public:
	CUsableRwFrameTable()			{	m_UsableRwFrameTable.clear();	}
	virtual ~CUsableRwFrameTable()	{	m_UsableRwFrameTable.clear();	}

	void	OnInit()
	{
		RwFrameSetRemoveCallback( EnumRemoveRwFrameCallback, (void*)this );
	}

	BOOL	AddToRwFrameMapTable(const RwFrame* pFrame)
	{
		if( !pFrame )		return FALSE;

		AuAutoLock Lock(m_FrameTableMutex);		
		if ( !Lock.Result() )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		m_UsableRwFrameTable.insert((DWORD)pFrame);

		return TRUE;
	}

	BOOL	RemoveFromRwFrameMapTable(const RwFrame* pFrame)
	{
		if ( !pFrame )		return FALSE;

		AuAutoLock Lock(m_FrameTableMutex);
		if ( !Lock.Result() )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		m_UsableRwFrameTable.erase( (DWORD)pFrame );

		return TRUE;
	}

	BOOL	FindFromRwFrameMapTable(const RwFrame* pFrame)
	{
		if( !pFrame )		return FALSE;
			
		AuAutoLock Lock(m_FrameTableMutex);
		if ( !Lock.Result() )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		std::set<DWORD>::iterator Itr = m_UsableRwFrameTable.find((DWORD)pFrame);
		return Itr != m_UsableRwFrameTable.end() ? TRUE : FALSE;
	}

	static RwFrame* EnumRemoveRwFrameCallback(RwFrame* frame, void* pData);

	ApMutualEx			m_FrameTableMutex;
	std::set<DWORD>		m_UsableRwFrameTable;		//. 유효한 RwFrame 포인터인지 전역 적으로 관리가능한 stl map을 이용.
};

//---------------- CD3DFxMng ------------------
class AgcmMap;
class CD3DFxMng
{
public:
	CD3DFxMng();
	~CD3DFxMng()		{			}

	BOOL	OnInit( ApmMap* pcApmMap, ApmOcTree* pcApmOcTree, AgcmOcTree* pcAgcmOcTree );

	void		DisableMatD3DFx()				{	m_bDisableMatD3DFx = TRUE;	}
	void		EnableMatD3DFx()				{	m_bDisableMatD3DFx = FALSE;	}
	BOOL		IsMatD3DFxDisabled() const		{	return m_bDisableMatD3DFx;	}

	void		AtomicFxCheck(RpAtomic* atomic);
	void		AtomicFxOn(RpAtomic* atomic);
	void		AtomicFxOff(RpAtomic* atomic);

	static		RpMaterial*		FxCheckMaterialCallBack( RpMaterial* pMat, void* data);
	static		RpGeometry*		FxCheckGeometryCallBack( RpGeometry* pGeom, void* data);
	static		RpClump*		FxAllCheckClumpCallBack( RpClump* pClump, void*	data);
	static		RpAtomic*		FxAllCheckAtomicCallBack (RpAtomic* pAtomic, void* data);

	void		AllAtomicFxCheck();			// option에서 변경시

protected:
	BOOL		m_bDisableMatD3DFx;
	ApmMap*		m_pcApmMap;
	AgcmMap*	m_pcAgcmMap;
	ApmOcTree*	m_pApmOcTree;
	AgcmOcTree*	m_pAgcmOcTree;
};

//. Edge rendering을 위한 RpToonInk
//---------------- CEdgeRender ------------------
class CEdgeRender
{
public:
	CEdgeRender();
	~CEdgeRender();

	BOOL	OnInit();
	void	SetToonInkColor( int red, int green, int blue);

	static RpAtomic*	CreateToonGeometry( RpAtomic *a, void * data __RWUNUSED__ );
	static RpAtomic*	SilhouetteRender( RpAtomic *a, void * data __RWUNUSED__ );
	static RpAtomic*	RpSkinResEntryRelease( RpAtomic *atomic );
	static void			Toonify( RpToonGeo *toonGeo );
	static void			SetSilThickness( float fSilThickness);

	static RpToonInk*	m_pSilhouetteInk;
	RwReal				m_fSilThickness;
	RwRGBA				m_EdgeColor;
};

//Map의 섹터에 관련된 클래스
//------------------------ CSectorData ------------------------
class CSectorData
{
public:
	CSectorData();
	~CSectorData();

private:

};

//---------------- AgcmRender ------------------
class AgcmRender : public AgcModule, public CRenderFadeInOut, public CUsableRwFrameTable, public CD3DFxMng, public CEdgeRender
{
public:
	static AgcmRender*				m_pThisAgcmRender;
	static RpAtomicCallBackRender	OriginalDefaultAtomicRenderCallback;			//Default Render Callback For Atomic 
	
	AgcmRender	( VOID );
	~AgcmRender	( VOID );

	BOOL	OnAddModule				( VOID );
	BOOL	OnInit					( VOID );
	BOOL	OnDestroy				( VOID );
	BOOL	OnIdle					( UINT32 ulClockCount );

	void	OnCameraStateChange		( CAMERASTATECHANGETYPE	ctype );
	void	OnLuaInitialize			( AuLua * pLua );

	ApmMap*							m_pcApmMap;
	ApmOcTree*						m_pApmOcTree;
	AgcmOcTree*						m_pAgcmOcTree;
	AgcmPostFX*						m_pcsAgcmPostFX;

	float							m_fRotAngle;				// 카메라의 Y축 회전값(같은 값으로 유지).. 축빌보드용 카메라에서 매프레임마다 set
	float							m_fPanAngle;
	int								m_iShadow2Range;			// Shadow2 onAddModule에서 설정

	FLOAT							m_fFogEnd;	

	FLOAT							SetDirectionalLightAdjust( FLOAT fval );
	FLOAT							SetAmbientLightAdjust( FLOAT fval );
	
	RwRGBAReal						m_colorOriginalAmbient, m_colorOriginalDirect, m_colorBlack;
	FLOAT							m_fDirectionalLightAdjust , m_fAmbientLightAdjust;
	RwRGBAReal						m_stAdjustedDirectionalLight , m_stAdjustedAmbientLight;
	RwV3d							*m_pViewMatrixAt , *m_pCameraPos;
	
	WorldAddClumpGroup*				m_listWorldClumps;		//World에 추가되는 clump그룹( 13개의 포인터 배열 .. 해시 테이블 )

	RwFrame*						m_pFrame;				// 거리계산 기준(캐릭터 or 카메라 프레임)
	RwV3d							m_vYAxis;				// y축

	UINT32							m_ulLastTick, m_ulCurTick, m_ulCurTickDiff;
		
	RpWorld*						m_pWorld;
	RwCamera*						m_pCamera;
	RpLight*						m_pAmbientLight;
	RpLight*						m_pDirectLight;
	
	BOOL							m_bDrawShadow2;
	BOOL							m_bVertexShaderEnable;		// 하드웨어 지원 여부 검사
	BOOL							m_bActiveClass;
	BOOL							m_bWBufferEnable;										// 하드웨어 지원 여부 검사
	BOOL							m_bDisableMatD3DFx;

	INT32							m_iCameraRasterWidth, m_iCameraRasterHeight;

	D3DXVECTOR4						m_fFogConstant;											// fog 계산용 (Vertex Shader)
	
	IDirect3DDevice9*				m_pCurD3D9Device;
	
	AgcmRenderType					m_csRenderType;

	int								m_iRenderCount;					// Ui Modal시 2번 render() 불릴수 있다. skip용

	ApRWLock						lockRender; // 렌더락..

	int								m_bDrawOcLineDebug;

	// Dynamic Render List ========================================================================
	TextureSortAtomicList			m_listNonAlpha;
	TextureSortAtomicList			m_listAmbient;							
	TextureSortAtomicList			m_listAlphaFunc;
	
	mmapAlphaAtomic					m_mapAlpha;
	
	//BlendAtomicList				m_listBlend;
	stBlendAtomicEntry				m_BlendAtomics;

	NotSortBlendAtomicList			m_listBlendNSort;
	int								m_iNumBlendNSort[R_NOT_SORT_BLEND_MODE];
		
	SkinningAtomic*					m_listSkinningBlend[R_BELND_MODE_NUM];					// blend모드 쓰는 것들(effect같은 것들)
	
	SkinningAtomicList				m_listSkinning;
	SkinningAtomic*					m_listSkinningAlpha;
	SkinningAtomicList				m_listSkinningAlphaFunc;
	
	UVAnimAtomic*					m_listUVAnim;											// UVAnim 중 Skin없는거
	UVAnimAtomic*					m_listUVAnimSkin;										// UVAnim 중 Skin
			
	std::vector<RpAtomic*>			m_listInvisible;		// A list of "invisible.fx" applied atomics.

	Shadow2TerrainList*				m_listShadow2Terrain;
	Shadow2TerrainList*				m_pShadow2Terrain_Own;									// 주인공이 서잇는 지점
	Shadow2TerrainList*				m_listTerrainNoMTexture;								// 지형중 RpMTexture 파이프 안타는 것들

	CustomRenderList*				m_listCustomRender;

	LocalLight*						m_listLocalLight;
	ProcessLight*					m_pTableLight[10];										// m_listPreRender의 index와 동일.index당 10개 light
	INT8							m_iTableLightNum[12];									// Light개수 저장 .. 2개 이상일 경우 가까운거 리턴위해 거리 계산 ㅜㅜ

	SimpleAtomic*					m_listLastZWrite;
	// ============================================================================================

public:
	FLOAT		SetFogEnd( FLOAT fFogEnd )		{	return m_fFogEnd = fFogEnd;	}	// 포그 파클립 저장용. 카메라 업데이트에서 포그 값을 강제로 변경하기 때문에. 렌더에서 강제로 설정한다..

	void		DisableMatD3DFx()				{	m_bDisableMatD3DFx = TRUE;	}
	void		EnableMatD3DFx()				{	m_bDisableMatD3DFx = FALSE;	}
	BOOL		IsMatD3DFxDisabled() const		{	return m_bDisableMatD3DFx;	}

	void		SetMainFrame(RwFrame* pFrame);
	void		SetActive(bool	bset)			{	m_bActiveClass = bset;	}
	RwCamera*	GetCamera()						{	return m_pCamera;	}
	BOOL		GetVertexShaderEnable()			{	return m_bVertexShaderEnable;	}

	AgcmRenderType*	GetRenderType()				{	return &m_csRenderType;		}

	CHAR*		GetRenderTypeName(INT32 lIndex);
	CHAR*		GetRenderBlendModeName(INT32 lIndex);
	
	void		SetWorldCamera( RpWorld* pWorld, RwCamera* pCamera );	//꼭 초기화 해주어야 함!!!
	void		SetLight( RpLight* pAmbient, RpLight* pDirect );
	void		LightValueUpdate();	// 라이트 변경 시에 콜해줘야함.
	void		Render();

	bool		AddClumpToWorld( RpClump* pClump,enumAddOption option = AGCMRENDER_ADD_NONE);
	bool		AddAlphaBModeClumpToWorld( RpClump* pClump, enumRenderBMode eBMode, BOOL bBillboard, enumAddOption option = AGCMRENDER_ADD_NONE);
	bool		AddClumpFromUDA( RpClump* pClump);
	bool		RemoveClumpFromWorld( RpClump* pClump );

	bool		AddAtomicToWorld( RpAtomic* pAtomic, enumAlphaOption eAlphaOption = CHECK_ALPHA, enumAddOption option = AGCMRENDER_ADD_NONE,bool bCheckRenderUDA = false);
	bool		AddAlphaBModeAtomicToWorld( RpAtomic* pAtomic, enumRenderBMode eBMode, BOOL bBillboard, enumAddOption option = AGCMRENDER_ADD_NONE);
	bool		AddBillAtomicToWorld( RpAtomic* pAtomic, enumAlphaOption eAlphaOption = CHECK_ALPHA, enumAddOption option = AGCMRENDER_ADD_NONE);
	bool		RemoveAtomicFromWorld( RpAtomic* pAtomic );

	// RpWorld에 추가하는 함수 대체^^
	bool		AddClumpToRenderWorld( RpClump*	pClump );
	bool		RemoveClumpFromRenderWorld( RpClump* pClump );
	bool		AddAtomicToRenderWorld( RpAtomic* pAtomic, RpClump*	pParentClump );
	bool		RemoveAtomicFromRenderWorld ( RpAtomic*	pAtomic, RpClump*	pParentClump );

	// 이제 Light도 Render에서 관리~
	bool		AddLightToRenderWorld( RpLight*	pLight , BOOL bMove = FALSE	);
	bool		RemoveLightFromRenderWorld ( RpLight*	pLight , BOOL bMove = FALSE );
	
	INT32		GetLightByAtomic( RpAtomic*	pAtomic,RpLight *pLights[]);		// Max number of local lights : 2 -> 8

	// 기존 RpWorld에 추가된것들 중에 대해 rendercallback을 변경후에는 이함수를 꼭 호출해주자.! (list를 뒤져 rendercallback정보를 갱신)
	void		UpdateRenderCallBack( RpAtomic*	pAtomic, RpClump* pParentClump );		// atomic 단위
	void		UpdateRenderCallBack( RpClump* pClump );								// clump 단위(하위atomic모두 갱신)

	void		UpdateDistanceForTool();												// tool에서 lod거리 세팅용

	// 기존 RpWorld에 추가된것들 중에 BoundingSphere변경 하고 변경반영 호출
	void		UpdateBoundingSphere( RpClump*	pClump );

	void		SetUVAnimRenderCB( RpAtomic*	pAtomic);
	void		ReturnUVAnimRenderCB( RpAtomic*	pAtomic);

	// Do refactoring of lighting render pipes.
	void		CustomizeLighting(RpClump *pClump, enumLightingType lightingType) const;
	void		ResetLighting(RpClump *pClump) const;

	void		RenderWorldForAllIntersections(RpIntersection *    intersection,   RpIntersectionCallBackAtomic    callBack, void *    data );

	BOOL		IsSkinedPipe( RpAtomic* atomic )	{		// skin pipeline이 붙어 있는지 검사한다!
		RpGeometry* pGeom = RpAtomicGetGeometry(atomic);
		return pGeom && RpSkinGeometryGetSkin( pGeom ) ? TRUE : FALSE;
	}

	RwBool		TextureTestAlpha( RwTexture *texture );
	RwBool		GeometryPreLightsTestAlpha( RpGeometry *geometry );

	BOOL		SetCallbackCustomRender(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//EdgeRender callback.

	BOOL		SetCallbackPreRender(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackPostRender(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackPostRender2(ApModuleDefaultCallBack pfCallback, PVOID pClass);//	Blend 뒤에 맨마지막에 그린다.
	BOOL		SetCallbackPostRenderOcTreeClear(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL		SetCallbackAddAtomic(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackAddClump(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackRemoveAtomic(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackRemoveClump(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL		SetCallbackOctreeUDASet(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackOctreeIDSet(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL		SetCallbackMTextureRender(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback(AGCMRENDER_CB_MTEXTURE_RENDER, pfCallback, pClass);}	

	void		OriginalAtomicRender( RpAtomic* pAtomic );
	void		OriginalClumpRender( RpClump* pClump );

	INT32		StreamReadClumpRenderType(ApModuleStream *pcsStream, AgcdClumpRenderType *pcsType);
	VOID		StreamWriteClumpRenderType(ApModuleStream *pcsStream, AgcdClumpRenderType *pcsType);

	VOID		ClumpSetRenderType(RpClump *pstClump, AgcdClumpRenderType *pcsType);
	VOID		ClumpSetRenderTypeCheckCustData(RpClump *pstClump , AgcdClumpRenderType *pcsType, INT32 *plCustData);

	// 특정 카메라로 재렌더링 ^^ ex)물반사 렌더링
	void		RenderMyCamera( RwCamera* camera, BOOL bDrawWorldAdded, AgcmRenderTestRenderTerrain	TF = ImmediateTerrainRender,
								AgcmRenderTestRenderAtomicList ALF = ImmediateAtomicListTestRender, AgcmRenderTestRenderAtomic AF = ImmediateAtomicRender);

	void		TerrainDataSet();

private:
	RwBool		AlphaTestPalettizedTexture( RwRaster *raster );
	RwBool		AlphaTestTrueColorTexture( RwRaster *raster );
	void		CheckAlphaAtomicFromClump( RpClump *clump, enumAlphaOption eAlphaOption , BOOL bBillboard);	//Take Out Alpha Atomic From Clump and Add to List
	void		CheckAlphaAtomic( RpAtomic *atomic, enumAlphaOption eAlphaOption , BOOL bBillboard);	//Add to List if atomic is Alpha one

	RwReal		GetCameraDistance( RpAtomic *atomic );					//Get Distance From Atomic To Camera

	VOID		RenderAmbOcclMVLAtomics			( VOID );		// For the batch rendering of "AMBIENTOCCLUSIONMVL.FX"-applied atomics
	VOID		RenderAlphaAtomics				( VOID );
	VOID		RenderNonAlphaAtomics			( VOID );
	VOID		RenderAmbientAtomics			( VOID );
	VOID		RenderAlphaFuncAtomics			( VOID );
	VOID		RenderNotSortedBlendAtomics		( VOID );
	VOID		RenderSortedBlendAtomics		( VOID );
	VOID		RenderSkinPipeAtomics			( VOID );
	VOID		RenderSkinPipeAlphaAtomics		( VOID );
	VOID		RenderSkinPipeBlendAtomics		( VOID );
	VOID		RenderSkinPipeAlphaFuncAtomics	( VOID );
	VOID		RenderLastZWriteAtomics			( VOID );
	VOID		RenderInvisibleAtomics			( VOID );		// Render "invisible.fx" applied atomics.
	VOID		RenderUVAnimAtomics				( UVAnimAtomic*	start_node );

	void		ProcessFadeAtomic				( RpAtomic* atomic , VShaderFadeAtomic*	pData );
	void		ProcessBillboardDraw			( RpAtomic*	atomic , int billtype );		//Billboard 그린다

	RpAtomic*	GeometryMaterialFlagOff			( RpAtomic *atomic, void *data );
	RpAtomic*	GeometryMaterialFlagOn			( RpAtomic *atomic, void *data );

	INT32		GetHashKey						( DWORD val )	{	return val%R_HASH_SIZE;		}
	void		ClearLists						( VOID );

	void		RenderAtomic					( RpAtomic*	atomic , BOOL bBill );

	static	void	DefaultAtomicListTestRender(RpAtomic* pList);
	static	void	ImmediateAtomicListTestRender(RpAtomic*	pList);
	static	void	DefaultAtomicRender(RpAtomic* atomic);
	static	void	ImmediateAtomicRender(RpAtomic*	atomic);
	static	void	DefaultTerrainRender(RpAtomic*	atomic,INT32 SectorDist);
	static	void	ImmediateTerrainRender(RpAtomic* atomic,INT32 SectorDist);

	void		RenderWorldAtomics( RwCamera* pCheckCamera, AgcmRenderTestRenderAtomicList pRenderFunc );
	void		RenderCustomObjects();

	void		SetDistance( RpAtomic* atomic, INT32 CameraZindex = 0 );

	RwReal		GetCameraDistanceOnXZ(const RwV3d& pointInWorld) const;

	// // For the batch rendering of "AMBIENTOCCLUSIONMVL.FX"-applied atomics
	static RpAtomic *AtomicAmbOcclMVLRenderCallBack(RpAtomic *atomic);
	struct AmbOcclMVLAtomic
	{
		RpAtomic *pAtomic_;
		RpLight *pLight_[8];

		AmbOcclMVLAtomic() : pAtomic_(NULL)		{	memset( pLight_, 0, sizeof(pLight_) );	}
	};
	std::vector<AmbOcclMVLAtomic> m_ambOcclMVLAtomics[FX_LIGHTING_VARIATION];

	
	RpMaterial *m_pInvisibleMaterial[5];		// The material which will contain a fx for the invisible skill

public:
	static	BOOL		CB_LIGHT_UPDATE					( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL		CB_LIGHT_RENDER					( PVOID pData, PVOID pClass, PVOID pCustData );		// Show the bounding sphere of a light.
	static	void		CBWorldIntersection				( RpIntersection* intersection, RpIntersectionCallBackAtomic callBack, void* data);
	static	RpAtomic*	SetRenderCBFromUDACB			( RpAtomic *atomic, void *data );
	static	RpAtomic*	ReturnDefOriginalRenderCB		( RpAtomic *atomic, void *data );
	static	RpAtomic*	SetAtomicListCB					( RpAtomic *atomic, void *data );

	static	RpAtomic*	AtomicSetAlpha					( RpAtomic *atomic, void *data );		
	static	RpAtomic*	AtomicSetNonAlpha				( RpAtomic *atomic, void *data );		
	static	RpAtomic*	AtomicSetAmbient				( RpAtomic *atomic, void *data );		
	static	RpAtomic*	AtomicSetInfoBlendAlpha			( RpAtomic *atomic, void *data );
	static	RpAtomic*	AtomicSetAlphaFunc				( RpAtomic *atomic, void *data );
	static	RpAtomic*	AtomicSetLastZWrite				( RpAtomic *atomic, void *data );

	static	RpAtomic*	RenderCallbackForNonAlphaAtomic		( RpAtomic *atomic );		// Render Callback For NonAlphaAtomic
	static	RpAtomic*	RenderCallbackForAmbientAtomic		( RpAtomic *atomic );		// Render Callback For Alpha Func Atomic - NonAlpha - Ambient변경 
	static	RpAtomic*	RenderCallbackForAlphaFuncAtomic	( RpAtomic *atomic );
	static	RpAtomic*	RenderCallbackForAlphaAtomic		( RpAtomic *atomic );		// Render Callback For AlphaAtomic
	static	RpAtomic*	RenderCallbackForBlendAtomic		( RpAtomic *atomic );
	static	RpAtomic*	RenderCallbackForBlendAtomicNotSort	( RpAtomic *atomic );		// Render Callback For AlphaAtomic - Not Sort by Distance
	static	RpAtomic*	RenderCallbackForVSFadeAtomic		( RpAtomic *atomic );	
	static	RpAtomic*	RenderCallbackForSkinPipe			( RpAtomic *atomic );		// skinpipe
	static	RpAtomic*	RenderCallbackForSkinPipeAlpha		( RpAtomic *atomic );		// skinpipe Alpha
	static	RpAtomic*	RenderCallbackForSkinPipeBlend		( RpAtomic *atomic );		// skinpipe Blending
	static	RpAtomic*	RenderCallbackForSkinPipeAlphaFunc	( RpAtomic *atomic );		// skinpipe AlphaFunc
	static	RpAtomic*	RenderCallbackForUVAnim				( RpAtomic *atomic );
	static	RpAtomic*	RenderCallbackForUVAnimSkin			( RpAtomic *atomic );
	static	RpAtomic*	RenderCallbackForLastZWrite			( RpAtomic *atomic );
	static	RpAtomic*	RenderCallbackForNone				( RpAtomic *atomic );
	
	// Billboard포함 atomics
	static	RpAtomic*	RenderCallbackForNonAlphaAtomicBill		( RpAtomic *atomic );	//Render Callback For NonAlphaAtomic
	static	RpAtomic*	RenderCallbackForAmbientAtomicBill		( RpAtomic *atomic );		//Render Callback For Alpha Func Atomic - NonAlpha - Ambient변경 
	static	RpAtomic*	RenderCallbackForAlphaFuncAtomicBill	( RpAtomic *atomic );
	static	RpAtomic*	RenderCallbackForAlphaAtomicBill		( RpAtomic *atomic );		//Render Callback For AlphaAtomic
	static	RpAtomic*	RenderCallbackForBlendAtomicBill		( RpAtomic *atomic );		//Render Callback For AlphaAtomic - Not Sort by Distance
	static	RpAtomic*	RenderCallbackForBlendAtomicNotSortBill	( RpAtomic *atomic );//Render Callback For AlphaAtomic - Not Sort by Distance
			
	static	RpAtomic*	RenderCallbackForSpecialRendering		( RpAtomic *atomic );	// Universal render callback for various temporary special  renderings(a.k.a. fade-in/out, uv animation, invisible renderings).
	static  RpAtomic*	RenderClumpOriginalDefaultRender		( RpAtomic *atomic, void *data );	//Original Default Render Callback을 위한 static함수 

	// RenderType 유저 데이터 설정!(BOB, 030204)
	static	RpAtomic*	SetRenderTypeCB							( RpAtomic *atomic, void *data);
	static	RpAtomic*	SetRenderTypeCheckCustDataCB			( RpAtomic *atomic, void *data);
	static	RwUInt32	MyWorldGetAtomicLocalLightsCallBack		( RpAtomic *pAtomic, RpLight *pLights[]);		// Max number of local lights : 2 -> 8
	static	RwUInt32	MyWorldGetAtomicLocalLightsDummyCallBack( RpAtomic *pAtomic, RpLight *pLights[]);

	static	RpAtomic*	InitAtomicDatasCB						( RpAtomic *atomic, void *data );

	void	AddCustomToBlendRenderList							( PVOID pClass, ApModuleDefaultCallBack pCustRenderCB, PVOID pData1, PVOID pData2 ,  RwV3d*	pWorldPos = NULL );

	void	InitAtomicDatas										( RpAtomic* atomic);
	void	InitAtomicDatas										( RpClump* clump);
	
	void	InitAtomicSetRenderCallBack							( RpAtomic* atomic, RpAtomicCallBackRender pCB, enumRenderType	eRenderType, enumRenderBMode eRenderBMode, BOOL bBill );
										
	
//◆◆◆◆◆◆◆◆◆◆◆ Sector관련 처리 data ◆◆◆◆◆◆◆◆◆◆◆
//◆◆◆◆◆◆◆◆◆◆◆ Sector관련 처리 함수 ◆◆◆◆◆◆◆◆◆◆◆

	INT32		m_iSectorDataIndex;
	bool		m_bUseCullMode;
	bool		m_bUseLODBoundary;

	SectorData*			GetClientSectorData( ApWorldSector *pSector )		{	return (SectorData*) m_pcApmMap->GetAttachedModuleData(m_iSectorDataIndex, (PVOID)pSector);			}
	SectorRenderList*	GetSectorData( ApWorldSector *pSector )				{	return (SectorRenderList*) m_pcApmMap->GetAttachedModuleData(m_iSectorDataIndex,(PVOID)pSector);	}

	void	SetUseLODBoundary(bool val)		{	m_bUseLODBoundary = val;	}

	void	SetSectorDistanceC( RpClump* pClump );
	void	SetSectorDistanceA( RpAtomic* pAtomic );

	BOOL	MyWorldAddAtomic( RpAtomic* atomic );
	BOOL	MyWorldAddClump( RpClump* clump );
	BOOL	MyWorldRemoveAtomic( RpAtomic* atomic );
	BOOL	MyWorldRemoveClump( RpClump* clump );

	void	CalcWorldBoundingSphere( RwSphere* sphere, RpAtomic* atomic );

	void	SetBoundingSphereFromUDA( RpAtomic* atomic );
	void	SetBoundingSphereFromUDA( RpClump* clump );

	void	AddTerrainToSector( RpAtomic* atomic );
	void	AddAtomicToSector( RpAtomic* atomic, INT32 type );
	void	AddClumpToSector( RpClump* clump, INT32 type );
	
	void	AddCustomRenderToSector(PVOID pClass, RwSphere* sphere, ApModuleDefaultCallBack pCustUpdateCB, ApModuleDefaultCallBack	pCustRenderCB, PVOID pData1, PVOID pData2, INT32 SectorAppearanceDistance = 10 );
	void	RemoveCustomRenderFromSector( PVOID pClass, RwSphere* sphere, PVOID pData1, PVOID pData2 );

	void	RemoveAtomicFromSector( RpAtomic* atomic, INT32	type );
	void	RemoveClumpFromSector( RpClump* clump, INT32 type );

	static	RpAtomic*	CalcBoundingSphere(RpAtomic * atomic, void *data);
	static	RpAtomic*	CallBackForEnumAtomicforSetBSFromUDA(RpAtomic*		atomic,void* data);
	static	RpAtomic*	CallBackForEnumAtomicAddSector( RpAtomic* atomic, void* data );
	static	RpAtomic*	CallBackForEnumAtomicRemoveSector( RpAtomic* atomic, void* data );

	static	BOOL	CBInitSector(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	CBRemoveSector( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CBInitSectorData(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	CBRemoveSectorData( PVOID pData, PVOID pClass, PVOID pCustData );

	void	ResetTickCount()	{	--m_ulCurTick;	}					// 마고자 (2004-01-28 오후 9:16:18) : 한프레임 두번 렌더용 펑션.

	void	RenderGeometryOnly( int type = 0 );							// 0 - 지형, 1 - 그외 ,// 마고자 (2004-01-29 오후 9:06:29) : 맵툴용
	void	WorldToAtomicRender( RpAtomic* pAtomic, INT32 nDistSector, BOOL bUpdateFrustum = FALSE );
	void	WorldToAtomicFrustumRender( RpAtomic* pAtomic, RwSphere& cSphere, INT32 nDistSector, RwCamera* pCheckCamera, AgcmRenderTestRenderAtomic pRenderFunc );
	void	CullModeWorldRender( RwCamera* pCheckCamera, AgcmRenderTestRenderAtomic pRenderFunc, AgcmRenderTestRenderTerrain pTerrainFunc );	//현재 주인공이 서있는 sector를 기준으로 cull check하고 해당 atomic이 처음 그리는 거라면 rendercallback을 호출
	void	CullModeWorldRenderGeometryOnly( int type = 0 );
	
	//■■■■■■■■ UpdateList처리 DATA & Funtions ■■■■■■■
	// 2004.10.28 effect쪽은 render도 등록..

	void	AtomicPreRender( RpAtomic* atomic );					// atomic의 rendering되기 전에 처리해주는 함수

	bool	AddUpdateInfotoClump( RpClump* pClump, PVOID pThisClass, ApModuleDefaultCallBack pUpdate, ApModuleDefaultCallBack pDestruct, PVOID pData, PVOID pCustData, ApModuleDefaultCallBack pRender = NULL );
	bool	AddUpdateInfotoAtomic( RpAtomic* pAtomic, PVOID	 pThisClass, ApModuleDefaultCallBack pUpdate, ApModuleDefaultCallBack pDestruct, PVOID pData, PVOID pCustData, ApModuleDefaultCallBack pRender = NULL );
							
	// data까지 비교
	bool	RemoveUpdateInfoFromClump( RpClump* pClump, PVOID pThisClass, ApModuleDefaultCallBack pUpdate, ApModuleDefaultCallBack pDestruct, PVOID pData );
	bool	RemoveUpdateInfoFromAtomic( RpAtomic* pAtomic, PVOID pThisClass, ApModuleDefaultCallBack pUpdate, ApModuleDefaultCallBack pDestruct, PVOID pData );

	// 단지 callback과 this만을 비교
	bool	RemoveUpdateInfoFromClump2(RpClump*	pClump, PVOID pThisClass, ApModuleDefaultCallBack pUpdate, ApModuleDefaultCallBack	pDestruct);
	bool	RemoveUpdateInfoFromAtomic2(RpAtomic* pAtomic, PVOID pThisClass, ApModuleDefaultCallBack pUpdate, ApModuleDefaultCallBack pDestruct);

	//	Clump나 Atomic에 붙어 있는 UpdateList에서 인스턴스 삭제
	bool	RemoveUpdateInfoFromClump3( RpClump* pClump		, PVOID pThisClass );
	bool	RemoveUpdateinfoFromAtomic3( RpAtomic* pAtomic  , PVOID pThisClass );

	// Lighting처리 칼라 계산용
	BOOL							m_bLogin;

	FLOAT							m_fLightColorParam;						
	INT32							m_iLightColorStatus;			// 0 - 증가 , 1 - 감소

	//@{ Jaewon 20050913
	// Various computed light colors which may be used in RenderAtomic().
	// 0 - ambient, 1 - directional
	RwRGBAReal						m_lightColorInShadow[2];
	RwRGBAReal						m_lightColorAtLoginSelect[2];
	RwRGBAReal						m_lightColorOfQuestObject[2];
	//. 2006. 9. 14. Nonstopdj
	//. Special Status Color 추가.
	//. index 0 : Stun
	//. index 1 : Freeze
	RwRGBAReal						m_lighterColorBySpecialStatus[2];

	// Compute light colors in shadow, at login selection, of quest objects.
	void	ComputeLightColors();
	//@} Jaewon

	// 옥트리 구성용(맵툴)
	INT32			m_iLoadRangeX1, m_iLoadRangeX2, m_iLoadRangeZ1, m_iLoadRangeZ2, m_iDrawRange;

	BOOL	SaveAllOctrees( char* szDirectory );		// 이걸 호출하여 octree만들고 저장 
	BOOL	MakeOctrees();

	// ♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥ OCTREE ♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥♥
	// PreRenderList -> std::priority_queue<PreRender>
	std::priority_queue< PreRender*, vector<PreRender*>, lessPreRender >	m_priorityQueuePreRender;
	
	OccluderLineList*		m_listOccluderLine;			// 지평선 라인 리스트
	static const int		_ocLineMax = 32;
	OccluderLineList*		m_pStartOcLine[_ocLineMax];			// 탐색수를 줄이기 위해.. xval /12 한값중 젤 작은 값을 갖는 시작위치 저장
	INT32					m_iMaxHeightOcLine[_ocLineMax];		// 검사수를 줄이기 위해.. 먼저 maxheight와 검사한후 모두 작으면 검사 안한다.
	BOOL					m_bOcLineCheck;

	void	SetOcLineCheck(BOOL	bVal)		{	m_bOcLineCheck = bVal;	}	// 지평선 체크를 on/off한다 (default : on)
	
	void	InsertPreListOcTree( RpAtomic* atomiclist, RpClump* pClump );	// OcTree에 추가된 것들(지형,object)
	void	InsertPreListWorld( WorldAddClumpGroup*	pClumpGroup);			// World에 추가된 것들(character,Item)
	void	InsertPreListCustom( OcCustomDataList* customdata );

	void	AddCustomRenderObject( PVOID pClass, RwSphere* BoundingSphere,FLOAT height, ApModuleDefaultCallBack pCustUpdateCB, ApModuleDefaultCallBack	pCustRenderCB, PVOID pData1, PVOID pData2, BOOL bCheck = TRUE );	// bCheck = 지평선 체크 여부.. 비나 눈은 할 필요 없다.
	void	RemoveCustomRenderObject(PVOID pClass,PVOID pData1,PVOID pData2);		// 3 인자가 같은 node를 찾아 지운다.

	BOOL	GetWorldPosToScreenPos( RwV3d * pWorldPos, RwV3d* pScreenPos);

	void	UpdateOBufferAndDrawTest( AgcmRenderTestRenderAtomicList pRenderFunc,BOOL bCustProcess = TRUE );				// Occlusion Buffer를 update하고 draw test를 한다.
	void	UpdateOBufferAndDrawNotTest( AgcmRenderTestRenderAtomicList	pRenderFunc,BOOL bCustProcess = TRUE );

	BOOL	IsExcept(INT32 sx,INT32 ex,INT32 y);	// 현재 occluder영역에 의해 가려지는지 여부 return
	BOOL	IsExcept(float sx,float ex,float y)			{ return IsExcept( ( INT32 ) sx , ( INT32 ) ex , ( INT32 ) y ); }

	void	SetOccluderLine(INT32 sx,INT32 ex,INT32 y);
	void	SetOccluderLine(float sx,float ex,float y)	{ SetOccluderLine( ( INT32 ) sx , ( INT32 ) ex , ( INT32 ) y ); }

	void	SetMaxHeightTable(OccluderLineList*		node);		// 넘기는 노드에 해당하는 y값들을 갖는 table갱신

	void	OcTreeModeRender(RwCamera*	checkCamera,AgcmRenderTestRenderTerrain	pTerrainFunc,BOOL bWorldAdded);	// OcTree를 탐색하며 render한다.
	void	OcTreeChildRender(OcTreeNode*	node,BOOL	bCheck,INT32 nDistSector,RwCamera*	checkCamera);	// bCheck가 FALSE이면 그냥 그린다. parent가 inside일 경우

	void	SetTerrainOccluderBoxs(OcRootQuadTreeNode*	node,FLOAT*	fArray,INT32* index);
		
	static  RpAtomic*	RenderCallbackForAlphaAtomic2(RpAtomic*	atomic);
	static  RpAtomic*	RenderCallbackForBlendAtomic2(RpAtomic*	atomic);
	static  RpAtomic*	RenderCallbackForAlphaAtomicBill2(RpAtomic*	atomic);
	static  RpAtomic*	RenderCallbackForBlendAtomicBill2(RpAtomic*	atomic);
	static	RpAtomic*	RenderCallbackForLastBlendAtomic( RpAtomic *atomic );
	static	RpAtomic*	PreRenderCallbackForVSFadeAtomic( RpAtomic *atomic );	

public:
	BOOL	m_bDrawCollision_Terrain ,  m_bDrawCollision_Object	;

	// #define으로 정의된것을 이용하기때문에 , 다른데서 쓸때 꼬인다.
	void		SetDrawCollisionTerrain( BOOL bVal );
	void		SetDrawCollisionObject( BOOL bVal );
	BOOL		SetPreRenderAtomicCallback( BOOL ( *pCallback ) ( RpAtomic * , void * pData ) , void * pData );

	static BOOL	IsMaptoolBuild();

	// 경삼아저씨의 오브젝트 정보 추가 표시 기능 사용 온오프
	// 스피어 , 콜리젼 , 피킹 아토믹표시기능..
	// 맵툴 빌드에서만 작동함.
	BOOL	m_bUseAtomicCustomRender;
	BOOL	SetUseAtomicCustomRender( BOOL bUse )		{ return m_bUseAtomicCustomRender = bUse; }

private :
	bool	_RenderBlendAtomic_BillBoard( void );
	void	_RenderBlendAtomic_LastBlended( void );
	bool	_RenderBlendAtomic( void );
	void	_ChangeRenderStateForBlendAtomic( enumRenderBMode eRenderMode );
	void	_RestoreRenderStateForBlendAtomic( void );
	FLOAT	_AtomicGetCameraDistance( RpAtomic *pAtomic );
	FLOAT	_GetSortFactor			( RpAtomic *pAtomic );
};

#endif //_AGCMRENDER_H_


