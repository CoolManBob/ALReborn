#ifndef __AGCMGRASS_H__
#define __AGCMGRASS_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmGrassD" )
#else
#pragma comment ( lib , "AgcmGrass" )
#endif
#endif

#include "AgcModule.h"
#include "rwcore.h"
#include "rpworld.h"

#include "AgcmRender.h"
#include "ApmOcTree.h"
#include "AgcmOcTree.h"
#include "ApmMap.h"
#include "AgcmMap.h"
#include "AgcmEventNature.h"
#include "AgcmShadow2.h"

#include "AuPackingManager.h"

#include "ApMemory.h"
#include <vector>
#include <list>

#include "ContainerUtil.h"

// Sector Destroy CB받아서 octree node나 sector data에 있는 풀 정보를 지우자!
// Sector Add CB 받아서 풀 로딩하자~ (water처럼)
#define AGCMGRASS_INI_NAME_TEXTURE_NAME				"TEXTURE_NAME"
#define AGCMGRASS_INI_NAME_TEXTURE_ID				"TEXTURE_ID"
#define AGCMGRASS_INI_NAME_GROUP_NAME				"GRASS_GROUP"
#define AGCMGRASS_INI_NAME_GRASS_INFO_ID			"GRASS_ID"
#define AGCMGRASS_INI_NAME_GRASS_NAME				"GRASS_NAME"
#define AGCMGRASS_INI_NAME_SHAPE_TYPE				"SHAPE_TYPE"
#define AGCMGRASS_INI_NAME_GRASS_WIDTH				"GRASS_WIDTH"
#define AGCMGRASS_INI_NAME_GRASS_HEIGHT				"GRASS_HEIGHT"
#define AGCMGRASS_INI_NAME_GRASS_WIDTH_RANDOM		"GRASS_WIDTH_RANDOM"
#define AGCMGRASS_INI_NAME_GRASS_HEIGHT_RANDOM		"GRASS_HEIGHT_RANDOM"
#define AGCMGRASS_INI_NAME_ANIMATION_TYPE			"ANIM_TYPE"
#define AGCMGRASS_INI_NAME_ANIMATION_AMOUNT			"ANIM_AMOUNT"
#define AGCMGRASS_INI_NAME_ANIMATION_SPEED			"ANIM_SPEED"
#define AGCMGRASS_INI_NAME_IMAGE_START_X			"IMAGE_START_X"
#define AGCMGRASS_INI_NAME_IMAGE_START_Y			"IMAGE_START_Y"
#define AGCMGRASS_INI_NAME_IMAGE_WIDTH				"IMAGE_WIDTH"
#define AGCMGRASS_INI_NAME_IMAGE_HEIGHT				"IMAGE_HEIGHT"

typedef enum
{
	GRASS_SHAPE_CROSS			= 0,
	GRASS_SHAPE_PERPENDICULAR	= 1
} enumGrassShapeType;

// octree의 leaf 너비가 400이고 대각 길이 대략 구하면 700
#define	AGCM_GRASS_SPHERE_RADIUS	700.0f
#define	AGCM_GRASS_HEIGHT_OFFSET	10.0f

#define AGCM_GRASS_SECTOR_APPEAR_DIST	2

#define		D3DFVF_GRASS_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
struct GRASS_VERTEX {
	float x, y, z;
	DWORD color;
	DWORD spec;	
	float u,v;
};

#define		GRASSLIST			std::list< Grass* >
#define		GRASSLISTITER		std::list< Grass* >::iterator

struct Grass: public ApMemory<Grass, 20000>
{
	WORD			iGrassID;
	WORD			iShadow;

	FLOAT			fRotX;
	FLOAT			fRotY;
	FLOAT			fScale;
	
	FLOAT			fAnimParam1;						// Animation Parameter1
	FLOAT			fAnimParam2;

	RwV3d			vPos;
	D3DXMATRIX		matTransform;
	
	GRASS_VERTEX*	pVB;

	INT32			iVertexOffset;
	RwTexture*		pTexture;
				
	VOID			Init()	{ iGrassID=0; iShadow=0; fRotX=fRotY=0.0f; fScale=1.0f; fAnimParam1=fAnimParam2=0.f; pVB=NULL; iVertexOffset=0; pTexture=NULL; }

	Grass(){ Init(); }
};

#define				DRAW_ORDER_SORT_COUNT		4

struct GrassGroup				// 이 단위로 grass 묶자.일정 범위 별로 묶는다.
{
	RwSphere		BS;
	
	// 한node에만 들어간다.
	OcTreeCustomID	stOctreeID;				
	
	INT32			iGrassNum;
	GRASSLIST		ListGrass;

	FLOAT			MaxY;					// 그룹내 풀의 최대 Y값(나중에 클라이언트에서 최대 BBOX구할때 참고)
	
	INT32			iCameraZIndex;			// client에 쓰임(octree mode용) .. maptool에선 save flag로 쓰임

	RwV3d			VertDrawOrder[DRAW_ORDER_SORT_COUNT];

	GrassGroup*		next;

	GrassGroup():iGrassNum( 0 ) , MaxY( 0.0f ),next( NULL )
	{
	}
	~GrassGroup(){  }

	void			InitVertOrder();
};

struct GrassInfo
{
	char			Name[30];

	INT32			iTextureID;
	INT32			iShapeType;
		
	INT32			iVertexOffset;
	INT32			iVertexOffset_Dark;
	INT32			numTris;

	//. RwD3D9DrawPrimitiveUP 사용으로 튜닝.
	GRASS_VERTEX*	pOriginalVB;
	GRASS_VERTEX*	pOriginalDarkVB;

	INT32			iAnimationType;
	FLOAT			fAnimAmount;
	FLOAT			fAnimSpeed;

	INT32			iWidth;
	INT32			iHeight;
	INT32			iWidth_random;
	INT32			iHeight_random;

	INT32			iImageX;
	INT32			iImageY;
	INT32			iImageW;
	INT32			iImageH;

	GrassInfo() : /*pvOriginalVB( NULL ), 
				pvOriginalVB_Dark( NULL ), */
				pOriginalVB(NULL), 
				pOriginalDarkVB(NULL),
				iVertexOffset(0){}
};

//풀들이 로딩된 섹터 정보들(중복 로딩 방지..)
struct	SectorGrassRoot
{
	ApCriticalSection	pLockMutex;

	INT16			six;
	INT16			siz;

	GrassGroup*		listGrassGroup;
	INT32			iGrassGroupCount;

	UINT32			iTotalGrassCount;					// maptool 작업시 참고용 .. 최종엔 빠져도 된다..
	INT16			bLoad;
	INT16			bAddRender;

	SectorGrassRoot*	next;

	BOOL			DoContainThis( RwV3d * pV3d );
};

typedef enum
{
	GRASS_DETAIL_HIGH			= 0,
	GRASS_DETAIL_MEDIUM			= 1,
	GRASS_DETAIL_LOW			= 2,
	GRASS_DETAIL_OFF
} enumGrassDetail;

typedef enum
{
	GRASS_JOB_ADD_ROOT = 0 ,
	GRASS_JOB_ADD_RENDER,
	GRASS_JOB_REMOVE_ROOT,
	GRASS_JOB_CLEAR_ROOT,				// option에서 off시 단지 root만 남기고 clear
	
	GRASS_JOB_REMOVED
} enumGrassJobType;

struct GrassJobQueue
{
	enumGrassJobType		type;
	void*					data1;							// 왠만하면 sector 저장 (sector release 검사용)
	void*					data2;
};

struct stGrassTextureEntry
{
	RwTexture*				m_pTexture;
	int						m_nIndex;

	stGrassTextureEntry( void )
	{
		m_pTexture = NULL;
		m_nIndex = -1;
	}
};

// 맵툴에선 풀 하나하나가 group으로 간주되고 client에선 octree node하나가 풀 group하나이다.
class AgcmGrass : public AgcModule
{
private:
	typedef	std::multimap< FLOAT ,  Grass* , greater< FLOAT > >				mmapDrawGrass;
	typedef	std::multimap< FLOAT ,	Grass* , greater< FLOAT > >::iterator	mmapDrawGrassIter;

public:
	AgcmGrass();
	~AgcmGrass();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);		

	void	DeleteAll(SectorGrassRoot*	root);

	void	SetGrassDetail(enumGrassDetail	detail);

	BOOL	LoadGrassInfoFromINI(char*	szFileName , BOOL bDecryption);
	BOOL	SaveGrassInfoToINI(char*	szFileName , BOOL bEncryption);

	INT32	MakeGrassVerts(GrassInfo*	pGrass);

	BOOL	InitGrass(Grass*	pGrass);
		// 풀 폴리건과 에니메이션 정보를 설정한다.

	void	CheckAndUpdateInShadow();		// 풀들의 list에서 shadow영역인지 아닌지 체크하고 해당VB set(normal or shadow)

	void	JobQueueAdd(enumGrassJobType	type,PVOID data1,PVOID data2);		// 동일 data검색해서 skip처리
	void	JobQueueRemove(INT32	six,INT32	siz);

	void	GrassMatrixSet(D3DXMATRIX*	pMat,Grass*	pGrass,INT32	iCamIndex);
	
	static	BOOL	CB_GRASS_UPDATE ( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CB_GRASS_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData );

	//@{ 2006/11/17 burumal
	static	BOOL	CB_GRASS_DISTCORRECT( PVOID pDistFloat, PVOID pNull1, PVOID pNull2 );
	//@}
	
	//void	RenderGrass();

	void	LoadGrass(SectorGrassRoot*	pRoot);
	
	static	BOOL	CB_LOAD_MAP ( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CB_UNLOAD_MAP ( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL	CB_LIGHT_VALUE_UPDATE ( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CB_SHADOW_UPDATE ( PVOID pData, PVOID pClass, PVOID pCustData );

	// MapTool용 
	static	BOOL	CB_OCTREE_ID_SET ( PVOID pData, PVOID pClass, PVOID pCustData );

	void	SetDirectoryPath(char*	szDir);

	SectorGrassRoot * GetGrassRoot( ApWorldSector * pSector ) { return GetGrassRoot( pSector->GetArrayIndexX() , pSector->GetArrayIndexZ() ); }
	SectorGrassRoot * GetGrassRoot( INT32 nIndexX , INT32 nIndexZ );

	void	AddGrass(INT32	iGrassID, FLOAT x, FLOAT z,FLOAT fRotX,FLOAT fRotY,FLOAT fScale);
	void	RemoveGrass(RwSphere*	sphere);			// 구 범위 안에 드는 풀들 삭제..
	void	SaveToFiles( char * szDir );
	void	RemoveAll();

	void	UnityForSave();								// 같은 octree id끼리 root별로 통합한다. ^^ 

	BOOL	CheckGrassList		();	// 데이타가 온전한지 확인..
	void	OptimaizeGrassList	();	// 불온전한 데이타를 제거함.

	void	UpdateGrassHeight();							// 모든 풀 list높이를 갱신
	void	UpdateGrassHeight(ApWorldSector*	pSector);	// 해당 섹터 풀list높이를 갱신

	FLOAT	GetGrassMinHeight( ApWorldSector*	pSector , FLOAT fTopHeight = SECTOR_MAX_HEIGHT );
	static	BOOL	CB_GET_MIN_HEIGHT ( PVOID pData, PVOID pClass, PVOID pCustData );
	
// data
	AgcmRender*			m_pcsAgcmRender;
	ApmMap*				m_pcsApmMap;
	AgcmMap*			m_pcsAgcmMap;
	AgcmOcTree*			m_pcsAgcmOcTree;
	ApmOcTree*			m_pcsApmOcTree;
	AgcmEventNature*	m_pcsAgcmEventNature;
	AgcmShadow2*		m_pcsAgcmShadow2;

	LPDIRECT3DDEVICE9	m_pCurDevice;

	BOOL				m_bDrawGrass;
	enumGrassDetail		m_eDetail;
					
	INT32				m_iDrawGrassNum;
	INT32				m_iDrawGrassCountNear;			// 현재 프레임에서 그려진 풀개수(가까이 있는거나 maptool)
	INT32				m_iDrawGrassCount;				// 현재 프레임에서 그려진 풀개수(profile용)

	RwV3d*				m_pViewMatrixAt;

	UINT32				m_uiLastTick;
	UINT32				m_uiCurTickDiff;
	
	INT32				m_iGrassInfoNum;
	vector< GrassInfo >	m_astGrassInfo;
	
	INT32				m_iTextureNum;
	ContainerVector< stGrassTextureEntry >		m_vecGrassTexture;

	SectorGrassRoot*	m_listGrassRoot;
	//DrawGrass*		m_listDraw;
	
	ApCriticalSection	m_csCSection;
	GrassJobQueue*		m_pJobQueue;
	INT32				m_iJobQueueCount;
	INT32				m_iJobQueueCount2;
	INT32				m_iJobQueueMaxCount;

	char				m_szDirectory[48];

	INT16				m_iDrawCameraIndex;
	INT16				m_iSectorRange;
};

#endif