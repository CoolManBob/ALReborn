#ifndef _AGCM_OCTREE_H_
#define _AGCM_OCTREE_H_

#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"
#include "rtintsec.h"
#include "rtpick.h"
#include "RtRay.h"

#include "AgcModule.h"
#include "ApBase.h"
#include "AcuObject.h"

#include "ApmOcTree.h"
#include "ApmMap.h"

#include "AcuFrameMemory.h"
#include "AcuIMDraw.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmOcTreeD" )
#else
#pragma comment ( lib , "AgcmOcTree" )
#endif
#endif

// 마고자 (2004-04-07 오후 11:22:17) : DLL 컴파일이 안돼서 강제 삽입.
#pragma comment ( lib , "rtintsec" )
#pragma comment ( lib , "rpcollis" )
#pragma comment ( lib , "rpworld" )
#pragma comment ( lib , "RtRay" )

// note:
// bbox대신에 4개의 점 list를 갖자. 아래쪽은 사실 체크 할 필요가 없다. 지평선 알고리즘 이므로
// 시점에서 가까운 순으로 차폐검사와 draw를 행한다. ###
// 차폐 검사를 할때 octree 노드가 들어오는지 검사한다. 하부 노드를 전부 제외시킬수 잇다.

// 모든 노드는 정렬 .. 카메라에서 far plane 까지
// 4점의 최소 y 그리고 최대,최소 x로 차폐 영역을 구성한다.

// render에서 octree구성하는 함수를 둔다.
// 저장은 apmoctree에서 하구 agcmoctree에서 저장할 지형 bbox는 streamcallback 받아서 한다.

// 지형 minbox는 quadtree 자료형태를 가집니다. 그를 위한 enum형
enum AGCMQTREE_NODES
{
	AGCMQTREE_LEFT_BACK,
	AGCMQTREE_LEFT_FRONT,
	AGCMQTREE_RIGHT_BACK,
	AGCMQTREE_RIGHT_FRONT
};

#define		OC_CLUMP_ATOMIC_LIST_SIZE		12
struct OcClumpAtomicList:public ApMemory<OcClumpAtomicList, 40000>
{
	RpAtomic*					atomic;	
	//@{ Jaewon 20050602
	// Just use atomic->renderCallBack.
	//RpAtomicCallBackRender		renderCB;
	//@} Jaewon
	OcClumpAtomicList*			next;
};

struct OcClumpList:public ApMemory<OcClumpList, 20000>
{
	RpClump*					clump;
	RwSphere					BS;						// 이동이 없다고 가정.. 이동 하는 것은 월드에 들어갓!^^

	//@{ Jaewon 20050608
	// Just use clump->atomicList.
	//OcClumpAtomicList*			atomiclist;				// 해당 clump의 atomic을 list로 갖는다.
	//@} Jaewon

	OcClumpList*				next;
	OcClumpList*				prev;
};

typedef struct tag_OcCustomDataList
{
	RwSphere					BS;						// 이동이 없다고 가정.. 이동 하는 것은 월드에 들어갓!^^
	RwV3d						TopVerts[4];
	
	ApModuleDefaultCallBack		pRenderCB;
	ApModuleDefaultCallBack		pUpdateCB;

	PVOID						pClass;
	PVOID						pData1;
	PVOID						pData2;

	INT32						iAppearanceDistance;
	//@{ 2006/11/17 burumal
	ApModuleDefaultCallBack		pDistCorrectCB;
	//@}

	INT32*						piCameraZIndex;		// grass구조체나 effect구조체에 int변수를 두고 주소를 넘긴다.
													// 그럼 render시에 camera z index를 이 포인터에 set해준다.(0 - 10, 0 - near , 9 - far)
													// 가까이 있는 것만 update하려 할때 유용하다.^^
	
// 위 Data set해서 AddCustomRenderDataToOcTree 함수에 넘긴다.
	INT32*						iRenderTick;			// 중복 rendering 방지

	tag_OcCustomDataList*		next;
	tag_OcCustomDataList*		prev;
}OcCustomDataList;

// Node에 붙는 attachdata
typedef struct tag_OcNodeData
{
	OcClumpList*		clump_list;
	OcCustomDataList*	custom_data_list;				// 풀이나 effect같은 것들
}OcNodeData;

// level2 (800 size)까지 분할된다 .. 그럼 root당 leaf 노드 총개수는 64개이다. leaf만 occluder로 추가
typedef struct tag_OcRootQuadTreeNode
{
	INT8						level;
	INT8						isleaf;
	INT16						hsize;

	tag_OcRootQuadTreeNode*		child[4];
	RwV3d						topVerts[4];
	RwV3d						worldcenter;
}OcRootQuadTreeNode;

// Root에 붙는 attachdata
typedef struct tag_OcRootData
{
	RpAtomic*					terrain;
	OcRootQuadTreeNode*			child[4];
}OcRootData;

// OcTree 구성시 필요한 구조체들(격자 중심에 잇음 4개의 노드가 리턴될수 있다).. 저장시 탐색용.
typedef struct tag_WhereQuadNodes
{
	int							num;
	OcRootQuadTreeNode*			quads[4];
}WhereQuadNodes;

#define			OCID_DATA_SIZE	24
// clump가 어느 octree node에 속하는지 알아내기 위한 구조체
typedef struct tag_WhereOcIDs
{
	INT32						six;
	INT32						siz;
	INT32						ID;

	BOOL						bSearch;
	INT32						cur_level;

	tag_WhereOcIDs*				next;
}WhereOcIDs;

// Render에서 쓰임.. AgcdObject에서 멤버로 갖고 있음
typedef struct tag_OcTreeRenderData
{
	RwV3d		topVerts_MAX[4];		// 최대 BBOX

	INT8		bMove;					// 이동 가능 객체일 경우엔 local로 verts값을 갖고 매번 구해준다.. 아니면 world space 변환후 고정
	INT8		isOccluder;
	INT8		nOccluderBox;
	INT8		pad;

	RwV3d*		pTopVerts;				// 최소 BBOX(Occluder일 경우만)
	
	//OcTreeIDList*	ID;					// ApdObject에 있는 IDlist의 시작지점을 가리킨다.
}OcTreeRenderData;

// Item,Character같이 occluder로 작용할 일이 없는 것을 위한 구조체(메모리 낭비 방지)..AgcdItemTemplate,AgcdCharacterTemplate이 멤버로 가짐
typedef struct tag_OcTreeRenderData2
{
	RwV3d		topVerts_MAX[4];		// 최대 BBOX
}OcTreeRenderData2;

// IntersectionLine에 걸린 sector들을 위한 리스트
#define			INTERSECTION_SECTORS_SIZE	8
typedef struct tag_IntersectionSectors
{
	ApWorldSector*				pSector;
	tag_IntersectionSectors*	next;
}IntersectionSectors;

typedef void	(*AgcmOcTreeIntersectionCBFunc) (RpIntersection*	intersection,
							 	RpIntersectionCallBackAtomic    callBack,    void *    data);

#define QUAD_MAX_DEPTH	2

class AgcmOcTree : public AgcModule
{
public:
	static AgcmOcTree *			m_pThis;
	
public:
	ApmOcTree*		m_pApmOcTree;
	ApmMap*			m_pApmMap;

	RpWorld*		m_pWorld;

	// ApmOcTree attach data index
	int				m_iRootDataIndex;
	int				m_iNodeDataIndex;

	// octree 추가 검사용(저장시용)
	int				m_iAllocTempSize;			// AcuFrameMemory 해제시 data size
	WhereOcIDs*		m_listFindID;

	// octree draw용(맵툴에서 디버깅 화면 표시용)
	BOOL			m_bDrawOcTree;
	RpClump*		m_pSelectedClump;

	//ApCriticalSection	m_csCSection;
	
public:
	AgcmOcTree();
	virtual ~AgcmOcTree();

	#ifdef USE_MFC
	protected:
		ApWorldSector * m_pSelectedSector;
	public:
		void SetOctreeDebugSector( ApWorldSector * pSector = NULL );
		void DrawOctreeDebugSectorBox();
	#endif // USE_MFC

	BOOL                OnInit();
	BOOL                OnAddModule();
	BOOL                OnIdle(UINT32 ulClockCount);	
	BOOL                OnDestroy();

	OcRootData *	GetRootData(OcTreeRoot *pRoot)	{ return (OcRootData*) m_pApmOcTree->GetAttachedModuleData(m_iRootDataIndex,(PVOID)pRoot);}
	OcNodeData *	GetNodeData(OcTreeNode *pNode)	{ return (OcNodeData*) m_pApmOcTree->GetAttachedModuleData(m_iNodeDataIndex,(PVOID)pNode);}

	inline	void	SetRpWorld(RpWorld*		pWorld) { m_pWorld = pWorld; }

	void	DivideRootQuadTree( OcRootQuadTreeNode*		node, int level);
	void	RemoveRootQuadTree( OcRootQuadTreeNode*		node);

	BOOL	AddClumpToOcTree( RpClump*	pClump );
	BOOL	RemoveClumpFromOcTree( RpClump*	pClump );

	//OcCustomDataList 구조체에 data를 set한후 넘긴다.		
	BOOL	AddCustomRenderDataToOcTree( FLOAT x, FLOAT y,FLOAT z, OcCustomDataList* pStruct);
	BOOL	RemoveCustomRenderDataFromOcTree( FLOAT x,FLOAT y,FLOAT z,PVOID pClass, PVOID pData1, PVOID pData2);

	BOOL	AddCustomRenderDataToOcTree( OcTreeCustomID* IDList, OcCustomDataList* pStruct);		// octree ID미리 setting된경우
	BOOL	RemoveCustomRenderDataFromOcTree( OcTreeCustomID*	IDList, PVOID pClass, PVOID pData1, PVOID pData2);
	
	//@{ Jaewon 20050608
	// Just use clump->atomicList.
	//BOOL	AddAtomicToOcTree( RpAtomic*	pAtomic);		// UDA를 통해 add되는 나무 등을 처리하기 위해.. 지금은 안쓰인다..2005.4.18 gemani
	//@} Jaewon

	//@{ Jaewon 20050608
	// Just use clump->atomicList.
	//void	SetAtomicList(RpClump*	pClump,OcClumpList*	clump_list);
	//static	RpAtomic*	SetAtomicListCB( RpAtomic *atomic, void *data );
	//@} Jaewon
	
	// octree구성용 
	void	CreateRootByTerrain( RpAtomic*	atomic, ApWorldSector * pSector );
	void	DivideToLeafTree(INT32 six, INT32 siz);

	// clump를 해당 apmoctree의 node에 삽입하고 OcClumpList에 set한다.
	BOOL	TestClumpOctrees(RpClump*	pClump);
	static	RpAtomic*	CallBackSetOcTreeIDs(RpAtomic*		atomic, void* data);
	BOOL	TestAtomicOctreesByClump(RpAtomic*		pAtomic,WhereOcIDs**	pIDs);

	void	TestAtomicOctrees(RpAtomic*	pAtomic);
	
	void 	GetQuadNodes(FLOAT x,FLOAT z,WhereQuadNodes*	pData,OcRootQuadTreeNode*	pQuadNode);
	
	void	SetAllQuadY(OcRootData*		pRoot,FLOAT y);
	void	SetAllQuadY(OcRootQuadTreeNode*		pNode,FLOAT y);

	// 맵툴에서 화면 디버깅용
	void	SetClumpAndDrawStart(RpClump*	pClump);
	void	SetDrawEnd();
	void	DrawDebugBoxes();							// 해당 clump가 속해있는 sector들의 octree node들과 
														// clump가 속한 node를 다른 색으로 화면에 표시..
	void	DrawOcTreeNodes(OcTreeNode*		node,INT32 six,INT32 siz,OcTreeIDList**		IdList);
	void	DrawQuadNodes(OcRootQuadTreeNode*	qnode);

	void	SaveQuadTreeBoxes(OcRootQuadTreeNode*	qnode,HANDLE fd,DWORD* fp,INT32* foffset);
	void	LoadQuadTreeBoxes(OcRootQuadTreeNode*	qnode,DWORD* pLoadBuffer,INT32* pLoadIndex);

	static BOOL	CBInitRootData ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBRemoveRootData ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBInitNodeData ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBRemoveNodeData ( PVOID pData, PVOID pClass, PVOID pCustData );

	static void	CBSaveRootData (HANDLE fd,DWORD* fp,INT32* foffset,OcTreeRoot*	root);	
	static void	CBLoadRootData (DWORD* pLoadBuffer,INT32* pLoadIndex,OcTreeRoot*	root); 

	//RpWorldForAllClumps에 대응, 해당 root의 모든 클럼프에 callback을 호출
	BOOL	OcTreeForAllClumps(INT32 six,INT32 siz,RpClumpCallBack    fpCallBack, void *    pData, INT32* pCorruptCheckArray);

	void	NodeForAllClumps(OcTreeNode*	node,RpClumpCallBack    fpCallBack, void *    pData,void*	pCheckArray);

	//◎◎◎◎◎◎◎◎◎ Intersection 용 ◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎ add by gemani(RpWorld 안쓸때)
	AgcmOcTreeIntersectionCBFunc			m_pfWorldIntersection;

	BOOL				SetWorldIntersectionCallBack(AgcmOcTreeIntersectionCBFunc	pFunc);
	
	UINT32				m_ulCurTick;

	void				OcTreeChildCheck(OcTreeNode*	node,RpIntersection *    intersection,  
										RpIntersectionCallBackAtomic    callBack,    void *    data);

	// 해당 섹터 옥트리에 대해 인터섹션을 행한다..
	void				OneOcTreeForAllAtomicsIntersection( INT32	six, INT32 siz , RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data );

	// 해당 라인에 해당하는 루트 리스트를 가져온다
	IntersectionSectors*	LineGetIntersectionSectors(RwLine*	line);

	// 현재 로딩된 모든 octree node를 체크한다
	// ID는 중복 Intersection체크를 방지하기 위해 부여한 번호
	// AgcmTargeting - 0 , AgcmLensFlare - 1, AgcmCamera - 2,
	// AgcmMap::GetMapPositionFromMousePosition - 3,
	// AgcmMap::GetCursorClump - 4

	// 지형 ,object,character 를 체크한다!!
	void				OcTreesForAllAtomicsIntersection( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// 지형을 제외하고 world와 object 체크한다!!
	void				OcTreesForAllAtomicsIntersection2( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// object만 체크한다!!(카메라에서 쓰임!)
	void				OcTreesForAllAtomicsIntersection3( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// character만 체크한다!!(debug용)
	void				OcTreesForAllAtomicsIntersection4( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// 지형하고만 체크한다!!(카메라에서 쓰임!)
	BOOL				OcTreesForAllAtomicsIntersection5(RpIntersection *    intersection, BOOL bLock = TRUE );

	// 옥트리내 모든 클럼프에 대해 callback을 수행한다..
	// 마고자 (2003-11-20 오후 3:17:52) : 클럼프 체크함수 추가.
	void				OcTreesForAllClumps( INT32				ID			,	// 아직 사용 안하나봄.. 현재 0 넣고 쓰는중..
											RpClumpCallBack		pCallBack	,
											void *				pData		,
											BOOL				bLock = TRUE );
};

#endif