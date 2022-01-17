#ifndef _APMOCTREE_H_
#define _APMOCTREE_H_

#include "ApModule.h"
#include "ApmMap.h"
#include "ApMemory.h"

#include "AuPackingManager.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "ApmOcTreeD" )
#else
#pragma comment ( lib , "ApmOcTree" )
#endif
#endif
//@} Jaewon

//참고: RootNode의 BBOX size는 맵 월드 width,height와 같다 . 그러므로 y는 -size/2 에서 size/2 의 범위를 갖는다.
// 지형은 RootNode에만 존재하고 RootNode엔 지형을 분할하여 일정 단위 BBOX를 QuadTree로 갖는다.
// leaf가 아니더라도 clump의 list를 갖는다.
// 즉,가장 fit한 node에 들어가게 된다.
// 예를 들어 8개의 자식에 모두 속할 경우 자식 노드엔 없고 부모 노드 한군데만 들어가게 된다.. 최소 순회를 위해

// 대각선 거리를 구하기 위해 1.732 (root3)곱해줌.. 약간 큰 1.75f 곱해주자.
//#define		OCTREE_SPHEREVAL		1.732f
#define		OCTREE_SPHEREVAL		1.75f

// Octree구성 최대 depth level
#define		OCTREE_MAXDEPTH			2
// octree구성 정보는 ApmOcTree에서 저장하고 읽는다.
#define		OCTREE_MAX_ROOT_COUNT	4

typedef struct tag_OcTreeNode
{
	// 레벨별 사이즈
	// 0 - 3200 , 1 - 1600 , 2 - 800 , 3 - 400 , 4 - 200 , 5 - 100 , 6 - 50 
	// 주: ID에 대해(bit구조)	- level7 까지 갖는다.(0 base)(6400 size기준으로 50 size) .. root에서 childs는 초기화시에 자동 분할
	// Ln = Level n의 Index, Depth = 현재 node의 depth
	// IsLeaf = 현재 node가 leaf인지 아닌지..(Tree에만 붙는다,object는 신경 안씀) ,Root Index = root list의 index
	// dummy(5):L7(3):L6(3):L5(3):L4(3):L3(3):L2(3):L1(3):Depth(3):IsLeaf(1):RootIndex(2)
	UINT32				ID;				
	tag_OcTreeNode*		parent;
	
	UINT16				objectnum;		// 현재 노드의 object 개수
	UINT8				bHasChild;		// FALSE이면 leaf
	UINT8				level;			// tree 깊이 레벨

	UINT32				hsize;			// half-size

	tag_OcTreeNode*		child[8];

	AuSPHERE			BS;				// 해당 node의 BS(Frustum check& 충돌 체크용)
}OcTreeNode;

//	세로로 여러개의 root가 올수 있다.(최대 4개) 높은 산 같은 경우
typedef struct tag_OcTreeRootList
{
	OcTreeNode*				node;
	INT32					rootindex;
	tag_OcTreeRootList*		next;
}OcTreeRootList;

typedef struct tag_OcTreeRoot
{
	INT8				sectorX;			// index( 16개의 섹터가 올수 있다,, 그러므로 INT8로 잡자 )
	INT8				sectorZ;
	INT8				rootnum;
	INT8				pad;

	FLOAT				centerX;
	FLOAT				centerZ;
	
	OcTreeRootList*		roots;				// 산같은 경우엔 세로로 여러개의 정육면체(6400size)가 올수 있다. 그러므로 list

	ApCriticalSection	m_csCSection;
}OcTreeRoot;

// 오브젝트에 붙는 octree id list ..  ApmObject에서 Streamming 한다.
struct OcTreeIDList: public ApMemory<OcTreeIDList, 12000>
{
	INT16			six;					// 현재 800까지 잡고 있다.. 그러므로 16bit할당
	INT16			siz;
	UINT32			ID;

	OcTreeNode*		pAddedNode;
	PVOID			pAddedClumpListNode;	// clump리스트에서 검색 빠르게 하기 위해 미리 노드 저장.. customdata listnode도 이걸 쓴다..
	OcTreeRoot*		pAddedRoot;

	OcTreeIDList*	next;

	//@{ kday 20050628
	// ;)
	OcTreeIDList() : six(0),siz(0),ID(0LU),pAddedNode(NULL),pAddedClumpListNode(NULL),pAddedRoot(NULL),next(NULL){};
	//@} kday
};

// custom data형식이 미리 ID를 저장시(현재는 풀만) 사용되는 구조체,, 역시 해당 모듈에서 Streamming 된다..
struct	OcTreeCustomID
{
	INT16			six;					// 현재 800까지 잡고 있다.. 그러므로 16bit할당
	INT16			siz;
	UINT32			ID;

	OcTreeNode*		pAddedNode;
	PVOID			pAddedClumpListNode;	// clump리스트에서 검색 빠르게 하기 위해 미리 노드 저장.. customdata listnode도 이걸 쓴다..
	OcTreeRoot*		pAddedRoot;
};

// 모듈 데이타 인덱스
enum APMOCTREE_DATA_INDEX
{
	OCTREE_NODE_DATA,
	OCTREE_ROOT_DATA
};

enum APMOCTREE_NODES
{
	APMOCTREE_TOP_LEFT_BACK,
	APMOCTREE_TOP_LEFT_FRONT,
	APMOCTREE_TOP_RIGHT_BACK,
	APMOCTREE_TOP_RIGHT_FRONT,
	APMOCTREE_BOTTOM_LEFT_BACK,
	APMOCTREE_BOTTOM_LEFT_FRONT,
	APMOCTREE_BOTTOM_RIGHT_BACK,
	APMOCTREE_BOTTOM_RIGHT_FRONT
};

// 메모리 아끼기 위해서 char형으로 쓴다.. 그러기 위해서 enum형 없앰.. 2005.3.29 gemani
#define		APMOCTREE_ROOT_STATUS_EMPTY		0
#define		APMOCTREE_ROOT_STATUS_LOADING	1
#define		APMOCTREE_ROOT_STATUS_LOADED	2
#define		APMOCTREE_ROOT_STATUS_REMOVED	3

typedef enum ApmOcTreeCallbackPoint	
{
	APMOCTREE_CB_ID_INIT_ROOT		= 0	
} ApmOcTreeCallbackPoint;

typedef void	(*ApmOcTreeSaveCallBackFunc) (HANDLE fd,DWORD* fp,INT32* foffset,OcTreeRoot*	root);	// agcmoctree에서 추가 정보를 저장하기 위해
typedef void	(*ApmOcTreeLoadCallBackFunc) (DWORD* pLoadBuffer,INT32* pLoadIndex,OcTreeRoot*	root); // agcmoctree에서 추가 정보를 로드하기 위해

/*#define			OCTREE_FO_NUM		8
// 최대 개수까지 LRU 알고리즘에 의해 유지한다. fileopen을 줄이기 위해서 handle저장 .. FOList == FileOpenedList ^^..
typedef struct tag_OcTreeFOList
{
	INT32					lx;
	INT32					lz;

	HANDLE					fd;
	tag_OcTreeFOList*		next;
}OcTreeFOList;*/

class ApmOcTree : public ApModule  
{
	OcTreeNode*	MakeChild(OcTreeNode*	node,APMOCTREE_NODES	pos);

public:
	ApmOcTree();
	~ApmOcTree();

	BOOL	OnAddModule	();
	BOOL	OnInit		();
	BOOL	OnDestroy	();

	INT32		AttachOcTreeNodeData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
	INT32		AttachOcTreeRootData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	BOOL		SetCallBackSave(ApmOcTreeSaveCallBackFunc	pFunc);
	BOOL		SetCallBackLoad(ApmOcTreeLoadCallBackFunc	pFunc);

	// octree시작 노드를 리턴 
	OcTreeNode*		CreateRoot(INT32	sectorix,INT32	sectoriz,FLOAT cy, OcTreeRoot**	root);	// world y를 넘긴다. root가 존재하면 리스트 추가
	void			DivideNode(OcTreeNode*	node);				// 현재 노드를 8개로 분할(현재 노드는 leaf 이어야 함)
	
	void			CombineNodeChilds(OcTreeNode*	node);		// 현재 노드의 자식들을 통합
	
	OcTreeRoot*		GetRoot(INT32	sectorix,	INT32	sectoriz);		// sector index를 넘겨서 root를 가져온다.
	OcTreeRoot*		GetRootByWorldPos(FLOAT	world_x, FLOAT world_z, BOOL bLoad = FALSE);			// world x,z를 넘겨서 root를 가져온다.

	BOOL			SetRoot(INT32	sectorix,	INT32	sectoriz,	OcTreeRoot*	root);
		
	OcTreeNode*		GetNode(OcTreeRoot*		root,UINT32 ID);
	OcTreeNode*		GetNode(OcTreeNode*		start,UINT32 ID);

	OcTreeNode*		GetNode(AuPOS*	pos);								// pos를 넘겨서 leaf를 가져온다.
	OcTreeNode*		GetNode(OcTreeNode*		start,AuPOS*	pos);
	
	OcTreeNode*		GetStartNode(OcTreeRoot*	root , INT32	rootindex);
	OcTreeNode*		GetStartNode(FLOAT x,FLOAT y,FLOAT z);		// world x,y,z 를 넘겨서 시작 node를 가져온다.
	OcTreeNode*		GetStartNode(OcTreeNode*		node);		// 시작 node를 가져온다. 현재 node로..
	UINT32			GetNodeID(OcTreeNode*			node);

	// 만약 octree가 해당ID만큼 분할되지 않았고 bDivide가 True이면 해당 IDlevel까지 분할하여 리턴한다. 
	OcTreeNode*		GetNodeForInsert(INT32 six,INT32 siz,UINT32	octreeID,BOOL	bDivide = TRUE);

	BOOL	TestBoxPos(AuPOS*	worldpos,AuPOS*	boxcenter,FLOAT	boxsize);

	void	DestroyTree(OcTreeRoot*		root);
	void	DestroyChilds(OcTreeNode*	node);

	// 마고자 (2005-11-24 오후 5:03:57) : 몽창 날린다.
	void	DestroyAll();

	// (맵툴용)
	// Octree test용 (먼저 모든 레벨까지 분할한후 node에 추가하고 최종적으로 optimizing한다.)
	void	DivideAllTree(OcTreeRoot*	root);
	void	DivideAllChilds(OcTreeNode*	node);

	// 자식이 모두 null이면 통합해주는 함수
	void	OptimizeTree(INT32 six,INT32 siz);
	int 	OptimizeChild(OcTreeNode*	node);

	// 단지 pos으로 leaf의 위치 ID를 계산해서 return(object용 - root가 없으면 현재 처리 X)
	// 0xffffffff를 리턴하면 error
	UINT32	GetLeafID(AuPOS*	pos);

	// File 입출력
	BOOL	SaveToFiles(char*	szDir,INT32	Loadx1,INT32	Loadx2,INT32	Loadz1,INT32	Loadz2);
	void	SaveNode(OcTreeNode*	node,INT32*		Foffset,HANDLE	fd,DWORD*	FP);

	OcTreeRoot*		LoadFromFiles(INT32	six,INT32 siz);
	void	LoadNode(OcTreeNode*	node,DWORD* pLoadBuffer,INT32*	pLoadIndex);

	BOOL	SetCallbackInitRoot	( ApModuleDefaultCallBack pfCallback, PVOID pClass	);

	static BOOL	CB_LoadSector			( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CB_ClearSector			( PVOID pData, PVOID pClass, PVOID pCustData );

	inline	UINT32	CalcRootIndex(UINT32	ID){return	(ID & 0x3); }
	inline	UINT32	SetRootIndex(UINT32	ID,UINT32 rootindex){return	((ID & 0xfffffffc) | (rootindex & 0x3)); }
	inline	UINT32	CalcDepth(UINT32	ID){return	((ID & 0x38) >> 3); }
	inline	UINT32	SetDepth(UINT32	ID,UINT32	depth){return ((ID & 0xffffffc7) | ((depth & 0x7) << 3)); }

	inline	UINT32	SetIsLeaf(UINT32	ID,BOOL	bLeaf) {return (ID | ((bLeaf & 0x1) << 2)); }

	// 1~7
	inline	UINT32	CalcIndex(UINT32	ID,INT32 lev){return	((ID & m_uiIndexMask[lev]) >> m_uiIndexShift[lev]); }

	inline	void	SetOcTreeEnable( BOOL	bEnable ) { m_bOcTreeEnable = bEnable; }
		
// Datas
	BOOL			m_bOcTreeEnable;
	ApmMap*			m_pApmMap;
	
	ApmOcTreeSaveCallBackFunc		m_pfSaveCB;
	ApmOcTreeLoadCallBackFunc		m_pfLoadCB;

	OcTreeRoot*		m_pOcTreeRoots[ MAP_WORLD_INDEX_WIDTH ][ MAP_WORLD_INDEX_HEIGHT ];
	char			m_aeOcTreeRootStatus[ MAP_WORLD_INDEX_WIDTH ][ MAP_WORLD_INDEX_HEIGHT ];

	UINT32			m_uiIndexMask[8];
	UINT32			m_uiIndexShift[8];

	ApCriticalSection	m_csCSection;

	ApWorldSector*		m_pCenterSector;			// 주인공이 서있는 섹터(release위해서..)
	INT16				m_iLoadRange;				// Release영역

	INT16				m_iCurLoadVersion;

	//OcTreeFOList*		m_listFO;
	//INT32				m_iFOCount;					

	//HANDLE	GetHandle(INT32 lx,INT32 lz);
};

#endif