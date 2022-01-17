#include "AgcmOcTree.h"

#include "ApMemoryTracker.h"
#include "cslog.h"

#include <vector>

AgcmOcTree * AgcmOcTree::m_pThis = NULL;

AgcmOcTree::AgcmOcTree()
{
	SetModuleName("AgcmOcTree");
	EnableIdle(TRUE);

	AgcmOcTree::m_pThis		=	this;

	m_iRootDataIndex	= 0;
	m_iNodeDataIndex	= 0;

	m_iAllocTempSize	= 0;
	m_listFindID		= NULL;

	m_bDrawOcTree = FALSE;
	m_pSelectedClump = NULL;

	m_ulCurTick = 0;
	m_pWorld	=	NULL;

	m_pfWorldIntersection = NULL;

	#ifdef USE_MFC
	m_pSelectedSector = NULL;
	#endif // USE_MFC
}

AgcmOcTree::~AgcmOcTree()
{

}

BOOL      AgcmOcTree::OnInit()
{
	return TRUE;
}

BOOL      AgcmOcTree::OnAddModule()
{
	m_pApmOcTree = (ApmOcTree*)GetModule("ApmOcTree");
	m_pApmMap = (ApmMap*)GetModule("ApmMap");

	m_iRootDataIndex = m_pApmOcTree->AttachOcTreeRootData (this,sizeof(OcRootData),NULL,CBRemoveRootData);
	m_iNodeDataIndex = m_pApmOcTree->AttachOcTreeNodeData (this,sizeof(OcNodeData),CBInitNodeData,CBRemoveNodeData);

	m_pApmOcTree->SetCallBackSave(CBSaveRootData);
	m_pApmOcTree->SetCallBackLoad(CBLoadRootData);

	m_pApmOcTree->SetCallbackInitRoot(CBInitRootData,this);

	return TRUE;
}

BOOL      AgcmOcTree::OnIdle(UINT32 ulClockCount)
{
	m_ulCurTick = ulClockCount;
	
	return TRUE;
}

BOOL      AgcmOcTree::OnDestroy()
{

	return TRUE;
}	

BOOL	AgcmOcTree::CBInitRootData ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmOcTree*			pThis = (AgcmOcTree*) pClass;
	OcTreeRoot*			pRoot = (OcTreeRoot*) pData;

	OcRootData*		pRootData = pThis->GetRootData(pRoot);

	pRootData->terrain = NULL;

	int		qsize = (int)MAP_SECTOR_WIDTH/4;
	int		hsize = (int)MAP_SECTOR_WIDTH/2;
	for(int i=0;i<4;++i)
	{
		pRootData->child[i] = new OcRootQuadTreeNode;
		pRootData->child[i]->level = 0;
		pRootData->child[i]->hsize = qsize;

		if(i == AGCMQTREE_LEFT_BACK)
		{
			pRootData->child[i]->worldcenter.x = pRoot->centerX - qsize;
			pRootData->child[i]->worldcenter.z = pRoot->centerZ - qsize;
		}
		else if(i == AGCMQTREE_LEFT_FRONT)
		{
			pRootData->child[i]->worldcenter.x = pRoot->centerX - qsize;
			pRootData->child[i]->worldcenter.z = pRoot->centerZ + qsize;
		}
		else if(i == AGCMQTREE_RIGHT_BACK)
		{
			pRootData->child[i]->worldcenter.x = pRoot->centerX + qsize;
			pRootData->child[i]->worldcenter.z = pRoot->centerZ - qsize;
		}
		else if(i == AGCMQTREE_RIGHT_FRONT)
		{
			pRootData->child[i]->worldcenter.x = pRoot->centerX + qsize;
			pRootData->child[i]->worldcenter.z = pRoot->centerZ + qsize;
		}

		pRootData->child[i]->topVerts[AGCMQTREE_LEFT_BACK].x = pRootData->child[i]->worldcenter.x - qsize;
		pRootData->child[i]->topVerts[AGCMQTREE_LEFT_BACK].y = 0.0f;
		pRootData->child[i]->topVerts[AGCMQTREE_LEFT_BACK].z = pRootData->child[i]->worldcenter.z - qsize;

		pRootData->child[i]->topVerts[AGCMQTREE_LEFT_FRONT].x = pRootData->child[i]->worldcenter.x - qsize;
		pRootData->child[i]->topVerts[AGCMQTREE_LEFT_FRONT].y = 0.0f;
		pRootData->child[i]->topVerts[AGCMQTREE_LEFT_FRONT].z = pRootData->child[i]->worldcenter.z + qsize;

		pRootData->child[i]->topVerts[AGCMQTREE_RIGHT_BACK].x = pRootData->child[i]->worldcenter.x + qsize;
		pRootData->child[i]->topVerts[AGCMQTREE_RIGHT_BACK].y = 0.0f;
		pRootData->child[i]->topVerts[AGCMQTREE_RIGHT_BACK].z = pRootData->child[i]->worldcenter.z - qsize;

		pRootData->child[i]->topVerts[AGCMQTREE_RIGHT_FRONT].x = pRootData->child[i]->worldcenter.x + qsize;
		pRootData->child[i]->topVerts[AGCMQTREE_RIGHT_FRONT].y = 0.0f;
		pRootData->child[i]->topVerts[AGCMQTREE_RIGHT_FRONT].z = pRootData->child[i]->worldcenter.z + qsize;
		
		pRootData->child[i]->isleaf = FALSE;
		pThis->DivideRootQuadTree(pRootData->child[i],1);
	}

	return TRUE;
}

BOOL	AgcmOcTree::CBRemoveRootData ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmOcTree*			pThis = (AgcmOcTree*) pClass;
	OcTreeRoot*			pRoot = (OcTreeRoot*) pData;

	OcRootData*		pRootData = pThis->GetRootData(pRoot);

	for(int i=0;i<4;++i)
	{
		pThis->RemoveRootQuadTree(pRootData->child[i]);
	}

	return TRUE;
}

BOOL	AgcmOcTree::CBInitNodeData ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmOcTree*			pThis = (AgcmOcTree*) pClass;
	OcTreeNode*			pNode = (OcTreeNode*) pData;

	OcNodeData*			pNodeData = pThis->GetNodeData(pNode);

	pNodeData->clump_list = NULL;
	pNodeData->custom_data_list = NULL;

	return TRUE;
}

BOOL	AgcmOcTree::CBRemoveNodeData ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmOcTree*			pThis = (AgcmOcTree*) pClass;
	OcTreeNode*			pNode = (OcTreeNode*) pData;

	OcNodeData*			pNodeData = pThis->GetNodeData(pNode);

	OcClumpList*		cur_node = pNodeData->clump_list;
	OcClumpList*		remove_node;

	//OcClumpAtomicList*		cur_listnode;
	//OcClumpAtomicList*		remove_listnode;

	while(cur_node)
	{
		//@{ Jaewon 20050608
		// Just use clump->atomicList.
		//if(cur_node->atomiclist)
		//{
		//	cur_listnode = cur_node->atomiclist;
		//	while(cur_listnode)
		//	{
		//		remove_listnode = cur_listnode;
		//		cur_listnode = cur_listnode->next;

		//		delete remove_listnode;
		//	}

		//	cur_node->atomiclist = NULL;
		//}
		//@} Jaewon

		remove_node = cur_node;
		cur_node = cur_node->next;

		delete remove_node;
	}

	pNodeData->clump_list = NULL;

	OcCustomDataList*		cur_cd = pNodeData->custom_data_list;
	OcCustomDataList*		remove_cd;

	while(cur_cd)
	{
		remove_cd = cur_cd;
		cur_cd = cur_cd->next;

		delete remove_cd->iRenderTick;
		delete remove_cd;
	}

	pNodeData->custom_data_list = NULL;
	
	return TRUE;
}

void	AgcmOcTree::DivideRootQuadTree( OcRootQuadTreeNode*	node, int level)
{
	int		qsize = node->hsize/4;
	int		hsize = node->hsize/2;
	for(int i=0;i<4;++i)
	{
		node->child[i] = new OcRootQuadTreeNode;
		node->child[i]->level = level;
		node->child[i]->hsize = hsize;

		if(i == AGCMQTREE_LEFT_BACK)
		{
			node->child[i]->worldcenter.x = node->worldcenter.x - hsize;
			node->child[i]->worldcenter.z = node->worldcenter.z - hsize;
		}
		else if(i == AGCMQTREE_LEFT_FRONT)
		{
			node->child[i]->worldcenter.x = node->worldcenter.x - hsize;
			node->child[i]->worldcenter.z = node->worldcenter.z + hsize;
		}
		else if(i == AGCMQTREE_RIGHT_BACK)
		{
			node->child[i]->worldcenter.x = node->worldcenter.x + hsize;
			node->child[i]->worldcenter.z = node->worldcenter.z - hsize;
		}
		else if(i == AGCMQTREE_RIGHT_FRONT)
		{
			node->child[i]->worldcenter.x = node->worldcenter.x + hsize;
			node->child[i]->worldcenter.z = node->worldcenter.z + hsize;
		}

		node->child[i]->topVerts[AGCMQTREE_LEFT_BACK].x = node->child[i]->worldcenter.x - hsize;
		node->child[i]->topVerts[AGCMQTREE_LEFT_BACK].y = 0.0f;
		node->child[i]->topVerts[AGCMQTREE_LEFT_BACK].z = node->child[i]->worldcenter.z - hsize;

		node->child[i]->topVerts[AGCMQTREE_LEFT_FRONT].x = node->child[i]->worldcenter.x - hsize;
		node->child[i]->topVerts[AGCMQTREE_LEFT_FRONT].y = 0.0f;
		node->child[i]->topVerts[AGCMQTREE_LEFT_FRONT].z = node->child[i]->worldcenter.z + hsize;

		node->child[i]->topVerts[AGCMQTREE_RIGHT_BACK].x = node->child[i]->worldcenter.x + hsize;
		node->child[i]->topVerts[AGCMQTREE_RIGHT_BACK].y = 0.0f;
		node->child[i]->topVerts[AGCMQTREE_RIGHT_BACK].z = node->child[i]->worldcenter.z - hsize;

		node->child[i]->topVerts[AGCMQTREE_RIGHT_FRONT].x = node->child[i]->worldcenter.x + hsize;
		node->child[i]->topVerts[AGCMQTREE_RIGHT_FRONT].y = 0.0f;
		node->child[i]->topVerts[AGCMQTREE_RIGHT_FRONT].z = node->child[i]->worldcenter.z + hsize;
		
		if(level == QUAD_MAX_DEPTH)
		{
			node->child[i]->isleaf = TRUE;
			node->child[i]->child[AGCMQTREE_LEFT_BACK] = NULL;
			node->child[i]->child[AGCMQTREE_LEFT_FRONT] = NULL;
			node->child[i]->child[AGCMQTREE_RIGHT_BACK] = NULL;
			node->child[i]->child[AGCMQTREE_RIGHT_FRONT] = NULL;
		}
		else 
		{
			node->child[i]->isleaf = FALSE;
			DivideRootQuadTree(node->child[i],level+1);
		}
	}
}

void	AgcmOcTree::RemoveRootQuadTree(OcRootQuadTreeNode*		node)
{
	for(int i=0;i<4;++i)
	{
		if(node->child[i])
		{
			RemoveRootQuadTree(node->child[i]);
			node->child[i] = NULL;
		}
	}

	delete node;
	node = NULL;
}

// 지형을 root에 삽입하고 quadtree로 bbox를 만들어준다.
void	AgcmOcTree::CreateRootByTerrain(RpAtomic*	atomic, ApWorldSector * pSector )
{
	int	i, j;

	ASSERT( NULL != pSector );

	INT32	six = pSector->GetArrayIndexX();
	INT32	siz = pSector->GetArrayIndexZ();

	RpGeometry*		pGeom = RpAtomicGetGeometry(atomic);
	RpMorphTarget*	pMorph = RpGeometryGetMorphTarget(pGeom,0);
	
	RwV3d*			pVerts = RpMorphTargetGetVertices(pMorph);

	RwReal			fMaxY,fMinY,fTemp;
	RwInt32			iRootMakeNum = 1;
	RwReal			fMapWidth = MAP_SECTOR_WIDTH;

	fMaxY = pVerts->y;
	fMinY = pVerts->y;

	INT32	num_vert = RpGeometryGetNumVertices (pGeom);
	
	for(i=0;i<num_vert;++i)
	{
		if(pVerts->y > fMaxY) fMaxY = pVerts->y;
		if(pVerts->y < fMinY) fMinY = pVerts->y;
		
		++pVerts;
	}

	fTemp = fMaxY;

	// 마고자 (2005-11-08 오전 11:15:26) : 
	// fMin 값에 적응돼지 않는 Grass 높이가 있다..
	fMinY = this->m_pApmMap->GetMinHeight( pSector );

	while(fTemp - fMinY > fMapWidth)
	{
		fTemp -= fMapWidth;
		++iRootMakeNum;
	}

	ASSERT( iRootMakeNum <= OCTREE_MAX_ROOT_COUNT );

	OcTreeRoot*		pRoot = m_pApmOcTree->GetRoot(six,siz);

	if(pRoot == NULL)
	{
		for(i=0;i<iRootMakeNum;++i)
		{
			// m_pApmOcTree->CreateRoot(six,siz,fMaxY - (fMapWidth*i), &pRoot);
			m_pApmOcTree->CreateRoot(six,siz,fMinY + (MAP_SECTOR_WIDTH/2.0f) + (fMapWidth*i), &pRoot);
			m_pApmOcTree->SetRoot(six,siz, pRoot);
		}

		pRoot = m_pApmOcTree->GetRoot(six,siz);
	}
	
	OcRootData*		pRootData = GetRootData(pRoot);
	pRootData->terrain = atomic;

	// 최대 값으로 set
	SetAllQuadY(pRootData,999999.0f);

	pVerts = RpMorphTargetGetVertices(pMorph);		
	WhereQuadNodes		stWQN;

	for(i=0;i<num_vert;++i)
	{
		stWQN.num = 0;
		for(j=0;j<4;++j)
		{
			GetQuadNodes(pVerts->x,pVerts->z,&stWQN,pRootData->child[j]);
		}

		for(j=0;j<stWQN.num;++j)
		{
			if(stWQN.quads[j]->topVerts[0].y > pVerts->y)
			{
				stWQN.quads[j]->topVerts[0].y =  pVerts->y;
				stWQN.quads[j]->topVerts[1].y =  pVerts->y;
				stWQN.quads[j]->topVerts[2].y =  pVerts->y;
				stWQN.quads[j]->topVerts[3].y =  pVerts->y;
			}
		}

		++pVerts;
	}
}

void 	AgcmOcTree::GetQuadNodes(FLOAT	x,FLOAT		z,WhereQuadNodes*	pStruct,OcRootQuadTreeNode*	qNode)
{
	FLOAT	dx,dz;

	if(qNode->isleaf)
	{
		if(qNode->worldcenter.x > x)
			dx = qNode->worldcenter.x - x;
		else
			dx = x - qNode->worldcenter.x;

		if(qNode->worldcenter.z > z)
			dz = qNode->worldcenter.z - z;
		else 
			dz = z - qNode->worldcenter.z;

		if(dx <= qNode->hsize && dz <= qNode->hsize)
		{
			pStruct->quads[pStruct->num] = qNode;
			++pStruct->num;
		}
	}
	else
	{
		if(qNode->worldcenter.x > x)
			dx = qNode->worldcenter.x - x;
		else
			dx = x - qNode->worldcenter.x;

		if(qNode->worldcenter.z > z)
			dz = qNode->worldcenter.z - z;
		else 
			dz = z - qNode->worldcenter.z;

		if(dx <= qNode->hsize && dz <= qNode->hsize)
		{
			for(int i=0;i<4;++i)
			{
				GetQuadNodes(x,z,pStruct,qNode->child[i]);
			}
		}
	}
}

void	AgcmOcTree::SetAllQuadY(OcRootData*	root,FLOAT y)
{
	for(int i=0;i<4;++i)
	{
		SetAllQuadY(root->child[i],y);
	}
}

void	AgcmOcTree::SetAllQuadY(OcRootQuadTreeNode*	node,FLOAT y)
{
	if(node->isleaf == FALSE)
	{
		for(int i=0;i<4;++i)
			SetAllQuadY(node->child[i],y);
	}

	node->topVerts[0].y = y;
	node->topVerts[1].y = y;
	node->topVerts[2].y = y;
	node->topVerts[3].y = y;
}

// 처음 OcTree구성용
BOOL	AgcmOcTree::TestClumpOctrees(RpClump*	pClump)
{
	INT32*		uda_oid = AcuObject::GetOcTreeID(pClump);

	if(uda_oid[0] > 0) return TRUE;		// 이미 계산된 clump이다.

	WhereOcIDs*		ids = NULL;

	m_iAllocTempSize = 0;
			
	if( !RpClumpForAllAtomics(pClump,CallBackSetOcTreeIDs,(void*)&ids) ) return FALSE;		// check해보자 ids에 재대로 값이 리턴하는지

	WhereOcIDs*		cur_data;

	// dummy(5):L7(3):L6(3):L5(3):L4(3):L3(3):L2(3):L1(3):Depth(3):IsLeaf(1):RootIndex(2)
	UINT32		mask_parents[7] =
	{0x00FFFFC3,0x001FFFC3,0x0003FFC3,0x00007FC3,0x00000FC3,0x000001C3,0x00000003};
	UINT32		mask_index[7] = 
	{0x07000000,0x00E00000,0x001C0000,0x00038000,0x00007000,0x00000E00,0x000001C0};
	UINT32		mask_index_shift[7] =
	{24,21,18,15,12,9,6};

	INT32		find_val;
	INT32		find_six,find_siz;

	BOOL		bAllSearch;
	char		find[8];
	INT32		find_level = OCTREE_MAXDEPTH;

	// Optimize
	// OCTREE_MAXDEPTH ~ level0<6400> 까지 통합검사 
	for(int i=7-OCTREE_MAXDEPTH;i<7;++i)
	{
		bAllSearch = FALSE;

		while(!bAllSearch)
		{
			for(int j=0;j<8;++j)
			{
				find[j] = 0;
			}

			// 첫번째 val 찾기
			cur_data = ids;
			while(cur_data)
			{
				if(cur_data->bSearch == FALSE && cur_data->cur_level == find_level)
				{
					find_val = cur_data->ID & mask_parents[i];
					find_six = cur_data->six;
					find_siz = cur_data->siz;

					cur_data->bSearch = TRUE;

					find[(cur_data->ID & mask_index[i]) >> mask_index_shift[i]] = 1;
					
					cur_data = cur_data->next;
					break;
				}

				cur_data = cur_data->next;
			}

			// 같은 상위레벨 찾기 
			while(cur_data)
			{
				if(cur_data->bSearch == FALSE)
				{
					if(find_val == (cur_data->ID & mask_parents[i]) && find_six == cur_data->six &&
					find_siz == cur_data->siz && cur_data->cur_level == find_level)
					{
						find[(cur_data->ID & mask_index[i]) >> mask_index_shift[i]] = 1;
						cur_data->bSearch = TRUE;
					}
				}

				cur_data = cur_data->next;
			}

			// 통합 여부 검사 and 통합
			if(find[0] && find[1] && find[2] && find[3] && find[4] && find[5] && find[6] && find[7])
			{
				WhereOcIDs*		before_data;

				cur_data = ids;
				while(cur_data)
				{
					if(find_val == (cur_data->ID & mask_parents[i]) && find_six == cur_data->six &&
					find_siz == cur_data->siz && cur_data->cur_level == find_level)
					{
						if(cur_data == ids)
						{
							ids = cur_data->next;
						}
						else
						{
							before_data->next = cur_data->next;
						}

						break;
					}
					else
					{
						before_data = cur_data;
						cur_data = cur_data->next;
					}
				}

				// 통합된 노드 생성
				WhereOcIDs*		nw_data = (WhereOcIDs*) AcuFrameMemory::AllocFrameMemory(sizeof(WhereOcIDs));
				if( nw_data )
				{
					m_iAllocTempSize += OCID_DATA_SIZE;

					nw_data->ID = find_val;
					nw_data->six = find_six;
					nw_data->siz = find_siz;
					nw_data->bSearch = FALSE;
					nw_data->cur_level = find_level-1;

					nw_data->next = ids;
					ids = nw_data;
				}
			}

			// 전부 검사 했는지 ..
			bAllSearch = TRUE;
			cur_data = ids;
			while(cur_data)
			{
				if(!cur_data->bSearch && cur_data->cur_level == find_level)
				{
					bAllSearch = FALSE;
					break;
				}

				cur_data = cur_data->next;
			}
		}

		find_level -= 1;
	}

	// Add
	cur_data = ids;
	OcTreeNode*		pTreeNode;
//	OcClumpList*	pClumpList;
//	OcClumpList*	nw_ClumpList;

	OcTreeIDList**		pIDList = (OcTreeIDList**)pClump->stType.pOcTreeIDList;
//	OcTreeIDList*		nw_ID ;
	
	int				index_oid = 0;
	while(cur_data)
	{
		pTreeNode = m_pApmOcTree->GetNodeForInsert(cur_data->six,cur_data->siz,cur_data->ID,TRUE);
		if(pTreeNode)
		{
			++pTreeNode->objectnum;
		}

		if(3 + index_oid < ALLOC_OCTREE_ID_TEST_UDA_NUM)
		{
			// userdata set ( save용 )
			++uda_oid[0];
			// sector ix
			uda_oid[1+index_oid] = cur_data->six;
			// sector iz
			uda_oid[2+index_oid] = cur_data->siz;
			// octree id
			uda_oid[3+index_oid] = cur_data->ID;

			index_oid += 3;
		}
		else
		{
			ASSERT(!"OcTreeIDSet개수 초과!");
		}

		cur_data = cur_data->next;
	}

	return TRUE;
}

RpAtomic*	AgcmOcTree::CallBackSetOcTreeIDs(RpAtomic*		atomic, void* data)
{
	WhereOcIDs**		pIDs = (WhereOcIDs**) data;

	RpLODAtomicSetCurrentLOD(atomic,0);		// max lod level set

	if( m_pThis->TestAtomicOctreesByClump(atomic,pIDs) )
		return atomic;
	else
		return NULL;
}

BOOL	AgcmOcTree::TestAtomicOctreesByClump(RpAtomic*		atomic,WhereOcIDs**	pIDs)
{
	RpGeometry*		pGeom = RpAtomicGetGeometry(atomic);
	RpMorphTarget*	pMorph = RpGeometryGetMorphTarget(pGeom,0);

	RwV3d*			pVerts = RpMorphTargetGetVertices(pMorph);
	
	RwV3d			PrevWorldVert;
	vector< RwV3d > vWorldVert;
	RwV3d			CalcVert;
	FLOAT			Length;

	INT32			six,siz;
	bool			bFind;

	WhereOcIDs*		cur_data;
	WhereOcIDs*		bef_data;

	INT32			num_vert = RpGeometryGetNumVertices(pGeom);
	UINT32			res_ID;

	RwFrame*		frame = RpAtomicGetFrame(atomic);
	LockFrame();
	RwMatrix*		matrix = RwFrameGetLTM(frame);
	UnlockFrame();

	int j;

	RwV3d			vTmp;

	for(int i=0;i<num_vert;++i)
	{
		vWorldVert.clear();
		RwV3dTransformPoint( &vTmp , pVerts , matrix);
		vWorldVert.push_back( vTmp );

		if(i>0)
		{
			RwV3dSub(&CalcVert,&PrevWorldVert,&vWorldVert[0]);
			Length = RwV3dLength(&CalcVert);

			RwV3dNormalize(&CalcVert,&CalcVert);
			RwV3dScale(&CalcVert,&CalcVert,800.0f);

			if(Length > 800.0f)
			{
				for(j = ( int ) ( Length - 800.0f ); j > 0 ; j -= 800 )
				{
					RwV3dAdd( &vTmp ,&vWorldVert[ vWorldVert.size() - 1 ],&CalcVert);
					vWorldVert.push_back( vTmp );

					//if(num_TestVert == 10)
					//	break;
				}
			}
		}

		for( j = 0 ; j < ( int ) vWorldVert.size() ; ++j )
		{
			res_ID = m_pApmOcTree->GetLeafID((AuPOS*)&vWorldVert[j]);

			if(res_ID == 0xffffffff)		// error
			{
				continue;
			}

			six = PosToSectorIndexX(vWorldVert[j].x);
			siz = PosToSectorIndexZ(vWorldVert[j].z); 

			six = SectorIndexToArrayIndexX(six);
			siz = SectorIndexToArrayIndexZ(siz);

			bFind = false;

			cur_data = (*pIDs);
			while(cur_data)
			{
				if(res_ID == cur_data->ID && six == cur_data->six && siz == cur_data->siz)
				{
					bFind = true;
					break;
				}

				bef_data = cur_data;
				cur_data = cur_data->next ;
			}

			if(!bFind)
			{
				WhereOcIDs*		nw_data = (WhereOcIDs*) AcuFrameMemory::AllocFrameMemory(sizeof(WhereOcIDs));

				if( NULL == nw_data )
					// 필히 메모리 부족
					return FALSE;

				m_iAllocTempSize += OCID_DATA_SIZE;

				nw_data->next = NULL;
				nw_data->ID = res_ID;
				nw_data->six = six;
				nw_data->siz = siz;
				nw_data->bSearch = FALSE;
				nw_data->cur_level = OCTREE_MAXDEPTH;

				if((*pIDs) == cur_data)
				{
					nw_data->next = (*pIDs);
					(*pIDs) = nw_data;	
				}
				else 
				{
					bef_data->next = nw_data;
					nw_data->next = cur_data;
				}
			}
		}

		PrevWorldVert = vWorldVert[0];

		++pVerts;
	}

	return TRUE;
}

void	AgcmOcTree::DivideToLeafTree(INT32	six,INT32 siz)
{
	OcTreeRoot*	pRoot = m_pApmOcTree->GetRoot(six,siz);

	m_pApmOcTree->DivideAllTree(pRoot);
}

void	AgcmOcTree::CBSaveRootData (HANDLE fd,DWORD* fp,INT32* foffset,OcTreeRoot*	root)
{
	OcRootData*		root_data = m_pThis->GetRootData(root);

	for(int i=0;i<4;++i)
	{
		m_pThis->SaveQuadTreeBoxes(root_data->child[i],fd,fp,foffset);
	}
}

void	AgcmOcTree::CBLoadRootData (DWORD* pLoadBuffer,INT32* pLoadIndex,OcTreeRoot*	root)
{
	OcRootData*		root_data = m_pThis->GetRootData(root);

	for(int i=0;i<4;++i)
	{
		m_pThis->LoadQuadTreeBoxes(root_data->child[i],pLoadBuffer,pLoadIndex);
	}
}

void	AgcmOcTree::SaveQuadTreeBoxes(OcRootQuadTreeNode*	qnode,HANDLE fd,DWORD* fp,INT32* foffset)
{
	// quadtree node data save
	if(qnode->isleaf)
	{
		WriteFile(fd,&qnode->topVerts[0].x,sizeof(float),fp,NULL);
		WriteFile(fd,&qnode->topVerts[0].y,sizeof(float),fp,NULL);
		WriteFile(fd,&qnode->topVerts[0].z,sizeof(float),fp,NULL);

		WriteFile(fd,&qnode->topVerts[1].x,sizeof(float),fp,NULL);
		WriteFile(fd,&qnode->topVerts[1].y,sizeof(float),fp,NULL);
		WriteFile(fd,&qnode->topVerts[1].z,sizeof(float),fp,NULL);

		WriteFile(fd,&qnode->topVerts[2].x,sizeof(float),fp,NULL);
		WriteFile(fd,&qnode->topVerts[2].y,sizeof(float),fp,NULL);
		WriteFile(fd,&qnode->topVerts[2].z,sizeof(float),fp,NULL);

		WriteFile(fd,&qnode->topVerts[3].x,sizeof(float),fp,NULL);
		WriteFile(fd,&qnode->topVerts[3].y,sizeof(float),fp,NULL);
		WriteFile(fd,&qnode->topVerts[3].z,sizeof(float),fp,NULL);

		(*foffset) += 48;
	}
	else
	{
		for(int i=0;i<4;++i)
		{
			SaveQuadTreeBoxes(qnode->child[i],fd,fp,foffset);
		}
	}
}

void	AgcmOcTree::LoadQuadTreeBoxes(OcRootQuadTreeNode*	qnode,DWORD*	pLoadBuffer,INT32* pLoadIndex)
{
	// quadtree node data load
	if(qnode->isleaf)
	{
		qnode->topVerts[0].x = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
		qnode->topVerts[0].y = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
		qnode->topVerts[0].z = ((float*)pLoadBuffer)[(*pLoadIndex)++];	

		qnode->topVerts[1].x = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
		qnode->topVerts[1].y = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
		qnode->topVerts[1].z = ((float*)pLoadBuffer)[(*pLoadIndex)++];	

		qnode->topVerts[2].x = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
		qnode->topVerts[2].y = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
		qnode->topVerts[2].z = ((float*)pLoadBuffer)[(*pLoadIndex)++];	

		qnode->topVerts[3].x = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
		qnode->topVerts[3].y = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
		qnode->topVerts[3].z = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
	}
	else
	{
		for(int i=0;i<4;++i)
		{
			LoadQuadTreeBoxes(qnode->child[i],pLoadBuffer,pLoadIndex);
		}
	}
}

// Object 만 추가됬으리라 본다..
BOOL	AgcmOcTree::AddClumpToOcTree(RpClump*	pClump)
{
	PROFILE("AgcmOcTree::AddClumpToOcTree");

	OcTreeIDList*		cur_ID = *((OcTreeIDList**)pClump->stType.pOcTreeIDList);

	OcTreeRoot*			oct_root;
	OcTreeNode*			oct_node;
	OcNodeData*			pNodeData;
	OcClumpList*		add_clump;

	RwFrame*			frame;
	RwMatrix*			LTM;

	//m_pThis->m_pApmOcTree->m_csCSection.Lock();

	while(cur_ID)
	{
		//@{ kday 20050628
		// ;)
		if( cur_ID->pAddedClumpListNode )
		{
			cur_ID = cur_ID->next;
			continue;
		}
		//@} kday
		oct_root =  m_pApmOcTree->GetRoot (cur_ID->six,cur_ID->siz);
		if(!oct_root)
		{
			cur_ID->pAddedRoot = NULL;

			cur_ID = cur_ID->next;
			continue;
		}

		oct_node =	m_pApmOcTree->GetNodeForInsert(cur_ID->six,cur_ID->siz,cur_ID->ID,FALSE);
		if(!oct_node) 
		{
			cur_ID->pAddedRoot = NULL;

			cur_ID = cur_ID->next;
			continue;
		}

		++oct_node->objectnum;		// object개수 추가

		pNodeData = GetNodeData(oct_node);

		add_clump = new OcClumpList;

		add_clump->BS = pClump->stType.boundingSphere;
		
		frame = RpClumpGetFrame(pClump);
	
		LockFrame();
		LTM = RwFrameGetLTM(frame);
		UnlockFrame();

		RwV3dTransformPoint(&add_clump->BS.center, &add_clump->BS.center, LTM);
		
		add_clump->clump = pClump;

		//@{ Jaewon 20050608
		// Just use clump->atomicList.
		//add_clump->atomiclist = NULL;
		//@} Jaewon
		add_clump->prev = NULL;

		//@{ Jaewon 20050608
		// Just use clump->atomicList.
		//SetAtomicList(pClump,add_clump);
		//@} Jaewon

		add_clump->next = pNodeData->clump_list;
		if(pNodeData->clump_list) pNodeData->clump_list->prev = add_clump;
		pNodeData->clump_list = add_clump;

		// 해당 ID관련 Node 미리 저장 .. 삭제시 빠르게 찾기 위해
		cur_ID->pAddedNode = oct_node;

		// clump리스트에서 빠르게 지우기 위해 노드 미리 저장
		cur_ID->pAddedClumpListNode = add_clump;

		// 나중에 지울시 root가 add될시 root인지 보장하기 위해
		cur_ID->pAddedRoot = oct_root;
		
		cur_ID = cur_ID->next;
	}

	//m_pThis->m_pApmOcTree->m_csCSection.Unlock();

	return TRUE;
}

BOOL	AgcmOcTree::RemoveClumpFromOcTree(RpClump*	pClump)
{
	OcTreeIDList*		cur_ID = *((OcTreeIDList**)pClump->stType.pOcTreeIDList);
		
	OcTreeRoot*			oct_root;
	OcTreeNode*			oct_node;
	OcNodeData*			pNodeData;
		
	OcClumpList*		cur_clump;

	//OcClumpAtomicList*	cur_atomic;
	//OcClumpAtomicList*	remove_atomic;

	//m_pThis->m_pApmOcTree->m_csCSection.Lock();

	while(cur_ID)
	{
		// root를 지웠을 경우도 있다.
		oct_root = m_pApmOcTree->GetRoot(cur_ID->six,cur_ID->siz);
		//@{ kday 20050628
		// ;)
		if( !oct_root
		 || !cur_ID->pAddedRoot)
		{
			cur_ID = cur_ID->next;
			continue;
		}
		ASSERT( cur_ID->pAddedRoot == oct_root );
		//@{ kday 20050822
		// ;)
		ASSERT( cur_ID->pAddedClumpListNode );
		//@} kday

		//@{ kday 20050701
		// ;)
		if(cur_ID->pAddedRoot != oct_root)
		{
			cur_ID = cur_ID->next;
			continue;
		}
		//@} kday

		//if(!oct_root) 
		//{
		//	cur_ID = cur_ID->next;
		//	continue;
		//}

		//if(cur_ID->pAddedRoot == NULL || cur_ID->pAddedRoot != oct_root)		// add시의 root와 틀리다 
		//{
		//	cur_ID = cur_ID->next;
		//	continue;
		//}
		//@} kday

		oct_node = cur_ID->pAddedNode;
		--oct_node->objectnum;		// object개수 감소

		pNodeData = GetNodeData(oct_node);

		cur_clump = (OcClumpList*)cur_ID->pAddedClumpListNode;
		//@{ kday 20050822
		// ;)
		cur_ID->pAddedClumpListNode = NULL;
		//@} kday

		//@{ Jaewon 20050608
		// Just use clump->atomicList.
		//cur_atomic = cur_clump->atomiclist;
		//while(cur_atomic)
		//{
		//	remove_atomic = cur_atomic;
		//	cur_atomic = cur_atomic->next;

		//	delete remove_atomic;
		//}
		//cur_clump->atomiclist = NULL;
		//@} Jaewon

		if(cur_clump == pNodeData->clump_list)
		{
			pNodeData->clump_list = cur_clump->next;
			if(pNodeData->clump_list) pNodeData->clump_list->prev = NULL;
		}
		else 
		{
			//. 2006. 5. 2. Nonstopdj
			//. Seize crash report.
			//. cur_clump->prev is null???
			//. check out. magoja bros.~
			cur_clump->prev->next = cur_clump->next;
			if(cur_clump->next) cur_clump->next->prev = cur_clump->prev;
		}

		delete cur_clump;

		cur_ID->pAddedRoot = NULL;				// 나중에 또 삭제 요청시 대비..

		cur_ID = cur_ID->next;
	}

	//m_pThis->m_pApmOcTree->m_csCSection.Unlock();
	
	return TRUE;
}

BOOL	AgcmOcTree::AddCustomRenderDataToOcTree(FLOAT x,FLOAT y,FLOAT z,OcCustomDataList* pStruct)
{
	AuPOS		pos;
	pos.x = x; pos.y = y; pos.z = z;
	
	INT32		id = m_pApmOcTree->GetLeafID(&pos);

	if(id != 0xffffffff)		// error
	{
		int		six,siz;
		six = PosToSectorIndexX(x);
		siz = PosToSectorIndexZ(z); 

		six = SectorIndexToArrayIndexX(six);
		siz = SectorIndexToArrayIndexZ(siz);

		OcTreeCustomID		oid;
		oid.six = six;
		oid.siz = siz;
		oid.ID = id;

		AddCustomRenderDataToOcTree( &oid , pStruct);
	}
	
	return TRUE;
}

BOOL	AgcmOcTree::AddCustomRenderDataToOcTree( OcTreeCustomID* cur_ID, OcCustomDataList* pStruct)
{
//	m_pApmOcTree->m_csCSection.Lock();

	OcTreeRoot*		oct_root =  m_pApmOcTree->GetRoot (cur_ID->six,cur_ID->siz);
	if(!oct_root)
	{
		cur_ID->pAddedRoot = NULL;
		return FALSE;
	}
    
	OcTreeNode*		oct_node =	m_pApmOcTree->GetNodeForInsert(cur_ID->six,cur_ID->siz,cur_ID->ID,FALSE);
	if(!oct_node) 
	{
		cur_ID->pAddedRoot = NULL;
		return FALSE;
	}

	++oct_node->objectnum;		// object개수 추가

	OcNodeData*		pNodeData = GetNodeData(oct_node);

	OcCustomDataList*	add_custom = new OcCustomDataList;

	add_custom->BS.center.x = pStruct->BS.center.x;
	add_custom->BS.center.y = pStruct->BS.center.y;
	add_custom->BS.center.z = pStruct->BS.center.z;
	add_custom->BS.radius = pStruct->BS.radius;

	for(int i=0;i<4;++i)
	{
		add_custom->TopVerts[i] = pStruct->TopVerts[i];
	}

	add_custom->iAppearanceDistance = pStruct->iAppearanceDistance;
	add_custom->pRenderCB = pStruct->pRenderCB;
	add_custom->pUpdateCB = pStruct->pUpdateCB;
	//@{ 2006/11/17 burumal
	add_custom->pDistCorrectCB = pStruct->pDistCorrectCB;
	//@}

	add_custom->pClass = pStruct->pClass;
	add_custom->pData1 = pStruct->pData1;
	add_custom->pData2 = pStruct->pData2;

	add_custom->piCameraZIndex = pStruct->piCameraZIndex;

	add_custom->iRenderTick = new INT32;
	*(add_custom->iRenderTick) = 0;

	add_custom->prev = NULL;

	add_custom->next = pNodeData->custom_data_list;
	if(pNodeData->custom_data_list) pNodeData->custom_data_list->prev = add_custom;
	pNodeData->custom_data_list = add_custom;

	// 해당 ID관련 Node 미리 저장 .. 삭제시 빠르게 찾기 위해
	cur_ID->pAddedNode = oct_node;

	// custom data 리스트에서 빠르게 지우기 위해 노드 미리 저장
	cur_ID->pAddedClumpListNode = add_custom;

	// 나중에 지울시 root가 add될시 root인지 보장하기 위해
	cur_ID->pAddedRoot = oct_root;

	//	m_pApmOcTree->m_csCSection.Unlock();

	return TRUE;
}

BOOL	AgcmOcTree::RemoveCustomRenderDataFromOcTree(FLOAT x,FLOAT y,FLOAT z,PVOID pClass, PVOID pData1, PVOID pData2)
{
	CCSLog	stLog( 3001 );

	AuPOS		pos;
	pos.x = x; pos.y = y; pos.z = z;

	INT32		id = m_pApmOcTree->GetLeafID(&pos);

	//	m_pApmOcTree->m_csCSection.Lock();

	if(id != 0xffffffff)		// error
	{
		int		six,siz;
		six = PosToSectorIndexX(x);
		siz = PosToSectorIndexZ(z); 

		six = SectorIndexToArrayIndexX(six);
		siz = SectorIndexToArrayIndexZ(siz);

		// 이건 따로 처리하자.. 추가 노드를 미리 저장하고 있다 ㅜㅜ..
		OcTreeRoot*	oct_root = m_pApmOcTree->GetRoot(six,siz);
		if(!oct_root)
		{
			//	m_pApmOcTree->m_csCSection.Unlock();
			return FALSE;
		}

		OcTreeNode*	oct_node =	m_pApmOcTree->GetNode(oct_root,id);
		if(!oct_node)
		{
			//	m_pApmOcTree->m_csCSection.Unlock();
			return FALSE;
		}

		--oct_node->objectnum;		// object개수 감소
	
		OcNodeData*				pNodeData = GetNodeData(oct_node);;
		OcCustomDataList*		cur_cd = pNodeData->custom_data_list;

		while(cur_cd)
		{
			if(cur_cd->pClass == pClass && cur_cd->pData1 == pData1 && cur_cd->pData2 == pData2)
			{
				if(cur_cd == pNodeData->custom_data_list)
				{
					pNodeData->custom_data_list = cur_cd->next;
					if(pNodeData->custom_data_list) pNodeData->custom_data_list->prev = NULL;
				}
				else 
				{
					cur_cd->prev->next = cur_cd->next;
					if(cur_cd->next) cur_cd->next->prev = cur_cd->prev;
				}
				
				delete cur_cd->iRenderTick;
				delete cur_cd;
				break;
			}

			cur_cd = cur_cd->next;
		}
	}

	//	m_pApmOcTree->m_csCSection.Unlock();
	
	return TRUE;
}

BOOL	AgcmOcTree::RemoveCustomRenderDataFromOcTree( OcTreeCustomID*	cur_ID, PVOID pClass, PVOID pData1, PVOID pData2 )
{
	CCSLog	stLog( 3002 );

//	m_pApmOcTree->m_csCSection.Lock();

	OcTreeRoot*			oct_root = m_pApmOcTree->GetRoot(cur_ID->six,cur_ID->siz);
	if(!oct_root) 
	{
		return FALSE;
	}

	if(cur_ID->pAddedRoot == NULL || cur_ID->pAddedRoot != oct_root)		// add시의 root와 틀리다 
	{
		return FALSE;
	}

	OcTreeNode*			oct_node = cur_ID->pAddedNode;
	--oct_node->objectnum;		// object개수 감소

	OcNodeData*			pNodeData = GetNodeData(oct_node);

	OcCustomDataList*	cur_cd = (OcCustomDataList*)cur_ID->pAddedClumpListNode;

	if(cur_cd == pNodeData->custom_data_list)
	{
		pNodeData->custom_data_list = cur_cd->next;
		if(pNodeData->custom_data_list) pNodeData->custom_data_list->prev = NULL;
	}
	else 
	{
		cur_cd->prev->next = cur_cd->next;
		if(cur_cd->next) cur_cd->next->prev = cur_cd->prev;
	}

	delete cur_cd->iRenderTick;
	delete cur_cd;

	cur_ID->pAddedRoot = NULL;			// 나중에 또 삭제 요청시 대비..


	//m_pApmOcTree->m_csCSection.Unlock();
	return TRUE;
}

//@{ Jaewon 20050608
// Just use clump->atomicList.
//void	AgcmOcTree::SetAtomicList(RpClump*	pClump,OcClumpList*	clump_list)
//{
//	RpClumpForAllAtomics(pClump,SetAtomicListCB,(PVOID)clump_list);
//}
//
//RpAtomic*	AgcmOcTree::SetAtomicListCB( RpAtomic *atomic, void *data )
//{
//	OcClumpList*	pList = (OcClumpList*) data;
//
//	OcClumpAtomicList*	add_atomic = new OcClumpAtomicList;
//
//	add_atomic->atomic = atomic;
//	//@{ Jaewon 20050602
//	// Just use atomic->renderCallBack.
//	//add_atomic->renderCB = RpAtomicGetRenderCallBack(atomic);
//	//@} Jaewon
//
//	add_atomic->next = pList->atomiclist;
//	pList->atomiclist = add_atomic;
//
//	return atomic;
//}
//@} Jaewon

//@{ Jaewon 20050608
// Just use clump->atomicList.
//BOOL	AgcmOcTree::AddAtomicToOcTree(RpAtomic*		pAtomic)
//{
//	RpClump*		pClump = RpAtomicGetClump(pAtomic);
//
//	AgcdType*		lpType = AcuObject::GetAtomicTypeStruct(pAtomic);
//	OcTreeIDList*		cur_ID = *((OcTreeIDList**)lpType->pOcTreeIDList);
//	
//	OcTreeNode*			oct_node;
//	OcNodeData*			pNodeData;
//	OcClumpList*		add_clump;
//	OcClumpList*		cur_clump;
//
//	OcClumpAtomicList*	add_atomic;
//
//	RwFrame*			frame;
//	RwMatrix*			LTM;
//	BOOL				bFind;
//
//	//m_pThis->m_pApmOcTree->m_csCSection.Lock();
//
//	while(cur_ID)
//	{
//		bFind = FALSE;
//
//		oct_node =	m_pApmOcTree->GetNodeForInsert(cur_ID->six,cur_ID->siz,cur_ID->ID,FALSE);
//
//		pNodeData = GetNodeData(oct_node);
//
//		cur_clump = pNodeData->clump_list;
//		while(cur_clump)
//		{
//			if(cur_clump->clump == pClump)
//			{
//				bFind = TRUE;
//				break;
//			}
//			cur_clump = cur_clump->next;
//		}
//
//		if(!bFind)
//		{
//			++oct_node->objectnum;		// object개수 추가
//			add_clump = new OcClumpList;
//
//			add_clump->BS = lpType->boundingSphere;
//
//			frame = RpClumpGetFrame(pClump);
//	
//			LockFrame();
//			LTM = RwFrameGetLTM(frame);
//			UnlockFrame();
//
//			RwV3dTransformPoint(&add_clump->BS.center, &add_clump->BS.center, LTM);
//		
//			add_clump->clump = pClump;
//
//			add_atomic = new OcClumpAtomicList;
//
//			add_atomic->atomic = pAtomic;
//			//@{ Jaewon 20050602
//			// Just use atomic->renderCallBack.
//			//add_atomic->renderCB = RpAtomicGetRenderCallBack(pAtomic);
//			//@} Jaewon
//
//			add_atomic->next = NULL;
//			add_clump->atomiclist = add_atomic;
//
//			add_clump->next = pNodeData->clump_list;
//			pNodeData->clump_list = add_clump;
//		}
//		else
//		{
//			add_atomic = new OcClumpAtomicList;
//
//			add_atomic->atomic = pAtomic;
//			//@{ Jaewon 20050602
//			// Just use atomic->renderCallBack.
//			//add_atomic->renderCB = RpAtomicGetRenderCallBack(pAtomic);
//			//@} Jaewon
//
//			add_atomic->next = cur_clump->atomiclist;
//			cur_clump->atomiclist = add_atomic;
//		}
//		
//		cur_ID = cur_ID->next;
//	}
//
//	//m_pThis->m_pApmOcTree->m_csCSection.Unlock();
//
//	return TRUE;
//}
//@} Jaewon

void	AgcmOcTree::SetClumpAndDrawStart(RpClump*	pClump)
{
	m_pSelectedClump = pClump;
	m_bDrawOcTree = TRUE;
}

void	AgcmOcTree::SetDrawEnd()
{
	m_bDrawOcTree = FALSE;
}

void	AgcmOcTree::DrawDebugBoxes()
{
/*	if(!m_bDrawOcTree) return;

	RpClump*	pClump = m_pSelectedClump;
	INT32*	uda_type = AcuObject::GetClumpTypeArray(pClump);

	INT32	tindex = uda_type[TYPE_ETYPE];
	INT32	type = AcuObject::GetType(tindex);

	if(type != ACUOBJECT_TYPE_OBJECT)	return;

	OcTreeIDList*	cur_ID = *((OcTreeIDList**)uda_type[TYPE_OCTREEIDLIST]);
	OcTreeIDList*	myID = NULL;
	OcTreeIDList*	nwID;

	INT32	IDsize = sizeof(OcTreeIDList);
	INT32	mem_alloced = 0;

	INT32	find_sector[32][2];
	INT32	find_sector_num = 0;
	BOOL	bFind_sector;

	while(cur_ID)
	{
		nwID = (OcTreeIDList*)AcuFrameMemory::AllocFrameMemory(IDsize);
		mem_alloced += IDsize;

		bFind_sector = FALSE;

		for(int i=0;i<find_sector_num;++i)
		{
			if(find_sector[i][0] == cur_ID->six && find_sector[i][1] == cur_ID->siz)
			{
				bFind_sector = TRUE;
			}
		}

		if(!bFind_sector)
		{
			find_sector[find_sector_num][0] = cur_ID->six;
			find_sector[find_sector_num][1] = cur_ID->siz;

			++find_sector_num;
		}

		nwID->six = cur_ID->six;
		nwID->siz = cur_ID->siz;
		nwID->ID = cur_ID->ID;

		nwID->next = myID;
		myID = nwID;

		cur_ID = cur_ID->next;
	}

	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE	, ( void * ) FALSE	);
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE	, ( void * ) TRUE	);
	
	// Quad Box그리자. ^^
	OcTreeRoot*			pRoot;
	OcRootData*			pRootData;

	for(int i=0;i<find_sector_num;++i)
	{
		pRoot = m_pApmOcTree->GetRoot(find_sector[i][0],find_sector[i][1]);
		if(!pRoot)	continue;

		pRootData = GetRootData(pRoot);
		if(!pRootData)	continue;

		for(int j=0;j<4;++j)
		{
			DrawQuadNodes(pRootData->child[j]);
		}
	}

	AcuFrameMemory::DeallocFrameMemory(mem_alloced);
	return;*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(!m_bDrawOcTree) return;

	RpClump*	pClump = m_pSelectedClump;

	INT32	tindex = pClump->stType.eType;
	INT32	type = AcuObject::GetType(tindex);

	if(type != ACUOBJECT_TYPE_OBJECT)	return;

	OcTreeIDList*	cur_ID = *((OcTreeIDList**)pClump->stType.pOcTreeIDList);
	OcTreeIDList*	myID = NULL;
	OcTreeIDList*	nwID;

	INT32	IDsize = sizeof(OcTreeIDList);
	INT32	mem_alloced = 0;

	INT32	find_sector[32][2];
	INT32	find_sector_num = 0;
	BOOL	bFind_sector;

	while(cur_ID)
	{
		nwID = (OcTreeIDList*)AcuFrameMemory::AllocFrameMemory(IDsize);
		if( nwID )
		{
			mem_alloced += IDsize;

			bFind_sector = FALSE;

			for(int i=0;i<find_sector_num;++i)
			{
				if(find_sector[i][0] == cur_ID->six && find_sector[i][1] == cur_ID->siz)
				{
					bFind_sector = TRUE;
				}
			}

			if(!bFind_sector)
			{
				if( find_sector_num < 32 )
				{
					find_sector[find_sector_num][0] = cur_ID->six;
					find_sector[find_sector_num][1] = cur_ID->siz;

					++find_sector_num;
				}
			}

			nwID->six = cur_ID->six;
			nwID->siz = cur_ID->siz;
			nwID->ID = cur_ID->ID;

			nwID->next = myID;
			myID = nwID;
		}

		cur_ID = cur_ID->next;
	}

	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE	, ( void * ) FALSE	);
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE	, ( void * ) FALSE	);
	
	// OcTree Node Box그리자. ^^
	OcTreeRoot*			pRoot;
	OcTreeRootList*		pRootList;
	for(int i=0;i<find_sector_num;++i)
	{
		pRoot = m_pApmOcTree->GetRoot(find_sector[i][0],find_sector[i][1]);
		if(!pRoot)	continue;

		pRootList = pRoot->roots;
		while(pRootList)
		{
			DrawOcTreeNodes(pRootList->node,pRoot->sectorX,pRoot->sectorZ,&myID);
			pRootList = pRootList->next;
		}
	}

	AcuFrameMemory::DeallocFrameMemory(mem_alloced);
}

#ifdef USE_MFC
	void AgcmOcTree::SetOctreeDebugSector( ApWorldSector * pSector )
	{
		m_pSelectedSector = pSector;
	}
	void AgcmOcTree::DrawOctreeDebugSectorBox()
	{
		if( m_pSelectedSector )
		{
			if( GetAsyncKeyState( VK_SHIFT ) < 0 )
			{
				RwRenderStateSet( rwRENDERSTATEZWRITEENABLE	, ( void * ) FALSE	);
				RwRenderStateSet( rwRENDERSTATEZTESTENABLE	, ( void * ) FALSE	);
			}

			// OcTree Node Box그리자. ^^
			OcTreeRoot		*	pRoot		;
			OcTreeRootList	*	pRootList	;
			OcTreeNode		*	pNode		;
			DWORD				color		= 0x1fffffff;

			pRoot = m_pApmOcTree->GetRoot(
				m_pSelectedSector->GetArrayIndexX() ,
				m_pSelectedSector->GetArrayIndexZ() );

			int	nLevel = 0;

			if(pRoot)	
			{
				pRootList = pRoot->roots;
				while(pRootList)
				{
					pNode = pRootList->node;

					switch( nLevel++ )
					{
					case 0: color = 0x0000ff00;break;
					case 1: color = 0x00ff0000;break;
					case 2: color = 0x000000ff;break;
					case 3: color = 0x00ffff00;break;
					}

					UINT32	uTick = GetTickCount() / 2 ;
					if( uTick & 0x100 )
						color |= ( ( uTick ) % 0xff ) << 24;
					else
						color |= 0xff - ( ( ( uTick ) % 0xff ) << 24 );
	
					RwBBox	box;
					box.inf.x = pNode->BS.center.x - pNode->hsize;
					box.inf.y = pNode->BS.center.y - pNode->hsize;
					box.inf.z = pNode->BS.center.z - pNode->hsize;

					box.sup.x = pNode->BS.center.x + pNode->hsize;
					box.sup.y = pNode->BS.center.y + pNode->hsize;
					box.sup.z = pNode->BS.center.z + pNode->hsize;

					AcuIMDraw::DrawBox(&box,color);

					pRootList = pRootList->next;
				}
			}
		}
	}
#endif // USE_MFC



void	AgcmOcTree::DrawQuadNodes(OcRootQuadTreeNode*	qNode)
{
	if(qNode->isleaf)
	{
		AcuIMDraw::DrawLine(&qNode->topVerts[0],&qNode->topVerts[1]);
		AcuIMDraw::DrawLine(&qNode->topVerts[1],&qNode->topVerts[3]);
		AcuIMDraw::DrawLine(&qNode->topVerts[3],&qNode->topVerts[2]);
		AcuIMDraw::DrawLine(&qNode->topVerts[2],&qNode->topVerts[0]);
	}
	else
	{
		for(int i=0;i<4;++i)
		{
			DrawQuadNodes(qNode->child[i]);
		}
	}
}

void	AgcmOcTree::DrawOcTreeNodes(OcTreeNode*		node,INT32 six,INT32 siz,OcTreeIDList**		IdList)
{
	bool	bFind = false;		// clump의 ID list와 같은 것이 있는지 검사

	OcTreeIDList*	cur_ID = (*IdList);
	OcTreeIDList*	bef_ID = cur_ID;
	while(cur_ID)
	{
		if(cur_ID->six == six && cur_ID->siz == siz && cur_ID->ID == node->ID)
		{
			bFind = true;

			// remove ID node (찾았으므로)
			if(cur_ID == (*IdList))
			{
				(*IdList) = cur_ID->next;
			}
			else 
			{
				bef_ID->next = cur_ID->next;
			}
		}

		bef_ID = cur_ID;
		cur_ID = cur_ID->next;
	}

	DWORD color = 0x1fffffff;

	if(bFind == true)		// 색 틀리게 해서 그림
	{
		int temp = rand() % 5;

		switch(temp)
		{
		case 0:
			color = 0xff00ff00;
			break;
		case 1:
			color = 0xffff0000;
			break;
		case 2:
			color = 0xff0000ff;
			break;
		case 3:
			color = 0xffffff00;
			break;
		case 4:
			color = 0xff00ffff;
			break;

		}
		
	}

	RwBBox	box;
	box.inf.x = node->BS.center.x - node->hsize;
	box.inf.y = node->BS.center.y - node->hsize;
	box.inf.z = node->BS.center.z - node->hsize;

	box.sup.x = node->BS.center.x + node->hsize;
	box.sup.y = node->BS.center.y + node->hsize;
	box.sup.z = node->BS.center.z + node->hsize;

	AcuIMDraw::DrawBox(&box,color);

	if(node->bHasChild)
	{
		for(int i=0;i<8;++i)
		{
			DrawOcTreeNodes(node->child[i],six,siz,IdList);
		}
	}
}

BOOL	AgcmOcTree::OcTreeForAllClumps(INT32 six,INT32 siz,RpClumpCallBack fpCallBack,void*	pData, INT32* pCorruptCheckArray)
{
	OcTreeRoot*		pRoot = m_pApmOcTree->GetRoot(six,siz);
	if(!pRoot)	return FALSE;

	//@{ 2006/11/22 burumal
	/*
	INT32			CorruptCheckArray[65536];		// 0번 index는 개수로 쓰임
	memset(CorruptCheckArray,0,4*65536);
	*/
	if ( pCorruptCheckArray == NULL )
		return FALSE;
	pCorruptCheckArray[0] = 0;
	//@}

	OcTreeRootList*		cur_root = pRoot->roots;
	while(cur_root)
	{
		NodeForAllClumps(cur_root->node,fpCallBack,pData, pCorruptCheckArray);

		cur_root = cur_root->next;
	}

	return TRUE;
}

void	AgcmOcTree::NodeForAllClumps(OcTreeNode*	node,RpClumpCallBack	fpCallBack,void*	pData,void*	pCheckArray)
{
	INT32*		pCorruptCheckArray = (INT32*)pCheckArray;

	if(node->objectnum)
	{
		OcNodeData*	pNodeData = GetNodeData(node);
		BOOL			bExist = FALSE;

		OcClumpList*	cur_clump = pNodeData->clump_list;
		while(cur_clump)
		{
			for(int i=1;i < (pCorruptCheckArray[0]+1);++i)
			{
				if(pCorruptCheckArray[i] == (INT32)cur_clump->clump)
				{
					bExist = TRUE;
					break;
				}
			}

			if(!bExist)
			{
				++pCorruptCheckArray[0];
				pCorruptCheckArray[pCorruptCheckArray[0]] = (INT32)cur_clump->clump;

				fpCallBack( cur_clump->clump , pData );
			}

			cur_clump  = cur_clump->next;
		}	
	}

	if(node->bHasChild)
	{
		for(int i=0;i<8;++i)
		{
			NodeForAllClumps(node->child[i],fpCallBack,pData,pCheckArray);
		}
	}
}

//◎◎◎◎◎◎◎◎◎◎◎◎◎ Intersection 함수 ◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎◎ add by gemani
BOOL	AgcmOcTree::SetWorldIntersectionCallBack(AgcmOcTreeIntersectionCBFunc	pFunc)
{
	m_pfWorldIntersection = pFunc;
	return TRUE;
}

void	AgcmOcTree::OcTreeChildCheck(OcTreeNode*	node,RpIntersection*	intersection,
									 RpIntersectionCallBackAtomic	callBack, void*	data)
{
	RwLine		line = intersection->t.line;
	RwReal		dist;

	if (!node)
		return;

	if(RtLineSphereIntersectionTest(&line,(RwSphere*)&node->BS, &dist))
	{
		OcNodeData*				pNodeData = GetNodeData(node);

		OcClumpList*			cur_clump = pNodeData->clump_list;
		//@{ Jaewon 20050608
		// Just use clump->atomicList.
		// OcClumpAtomicList -> RpAtomic
		RpAtomic*		cur_atomic;
		//@} Jaewon
		while(cur_clump)
		{
			if(RtLineSphereIntersectionTest(&line, (RwSphere*)&cur_clump->BS, &dist))
			{
				//@{ Jaewon 20050608
				// Just use clump->atomicList.
				if(cur_clump->clump->atomicList)
				{
					cur_atomic = cur_clump->clump->atomicList;
					do
					{
						if(cur_atomic->stRenderInfo.intersectTick != (INT32)m_ulCurTick)		// 한번만 체크(GetCursorClump제외)
						{
							cur_atomic->stRenderInfo.intersectTick = m_ulCurTick;
							callBack (intersection, NULL , cur_atomic , dist , data);
						}
						cur_atomic = cur_atomic->next;
					}
					while(cur_atomic != cur_clump->clump->atomicList);
				}
				//@} Jaewon
			}

			cur_clump = cur_clump->next;
		}

		if(node->bHasChild)
		{
			for(int i=0;i<8;++i)
			{
				OcTreeChildCheck(node->child[i],intersection,callBack,data);
			}
		}
	}
}

void	AgcmOcTree::OneOcTreeForAllAtomicsIntersection( INT32 six , INT32 siz ,   RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data )
{
	if(intersection->type != rpINTERSECTLINE)		// 현재는 Line만 체크
		return;

	RwLine		line = intersection->t.line;
	RwReal		dist;

	//m_pApmOcTree->m_csCSection.Lock();

	OcTreeRoot*				pRoot = m_pApmOcTree->GetRoot(six,siz);
	if(!pRoot)
	{
		//m_pApmOcTree->m_csCSection.Unlock();
		return;
	}

	OcRootData*				pRootData = GetRootData(pRoot);
	if(!pRootData)
	{
		//m_pApmOcTree->m_csCSection.Unlock();
		return;
	}

	// 지형 체크
	if(pRootData->terrain)
	{
		if(RtLineSphereIntersectionTest ( &line, RpAtomicGetBoundingSphere(pRootData->terrain) , &dist))	// Intersection!!
		{
			callBack (intersection, NULL , pRootData->terrain , dist , data);
		}
	}

	// OcTree 체크
	OcTreeRootList*		cur_root = pRoot->roots;
	while(cur_root)
	{
		OcTreeChildCheck(cur_root->node,intersection,callBack,data);
	
		cur_root = cur_root->next;
	}

	//m_pApmOcTree->m_csCSection.Unlock();
		
	// World도 check하자 (character같은 경우는 world에 add)
	m_pfWorldIntersection(intersection,callBack,data);
}

void	AgcmOcTree::OcTreesForAllAtomicsIntersection(INT32 ID, RpIntersection*	intersection,
											 	RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL	bLock )
{
	if(intersection->type != rpINTERSECTLINE)		// 현재는 Line만 체크
		return;

	++m_ulCurTick;

	RwLine		line = intersection->t.line;
	RwReal		dist;

	IntersectionSectors*	list = LineGetIntersectionSectors(&line);

	OcTreeRoot*				pRoot;
	OcRootData*				pRootData;

	//m_pApmOcTree->m_csCSection.Lock();

	while(list)
	{
		pRoot = m_pApmOcTree->GetRoot(list->pSector->GetArrayIndexX(),list->pSector->GetArrayIndexZ());
		if(!pRoot)		
		{
			list = list->next;
			continue;
		}

		pRootData = GetRootData(pRoot);
		if(!pRootData)
		{
			list = list->next;
			continue;
		}

		// 지형 체크
		if(pRootData->terrain)
		{
			if(RtLineSphereIntersectionTest ( &line, RpAtomicGetBoundingSphere(pRootData->terrain) , &dist))	// Intersection!!
			{
				callBack (intersection, NULL , pRootData->terrain , dist , data);
			}
		}

		// OcTree 체크
		OcTreeRootList*		cur_root = pRoot->roots;
		while(cur_root)
		{
			OcTreeChildCheck(cur_root->node,intersection,callBack,data);
		
			cur_root = cur_root->next;
		}
			
		list = list->next;
	}

	//m_pApmOcTree->m_csCSection.Unlock();
	
	// World도 check하자 (character같은 경우는 world에 add)
	m_pfWorldIntersection(intersection,callBack,data);
}

void	AgcmOcTree::OcTreesForAllAtomicsIntersection2(INT32 ID, RpIntersection*	intersection,
											 	RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL	bLock )
{
	if(intersection->type != rpINTERSECTLINE)		// 현재는 Line만 체크
		return;

	++m_ulCurTick;

	RwLine		line = intersection->t.line;

	IntersectionSectors*	list = LineGetIntersectionSectors(&line);

	OcTreeRoot*				pRoot;
	OcRootData*				pRootData;

	//m_pApmOcTree->m_csCSection.Lock();

	while(list)
	{
		pRoot = m_pApmOcTree->GetRoot(list->pSector->GetArrayIndexX(),list->pSector->GetArrayIndexZ());
		if(!pRoot)
		{
			list = list->next;
			continue;
		}

		pRootData = GetRootData(pRoot);
		if(!pRootData)
		{
			list = list->next;
			continue;
		}

		// OcTree 체크
		OcTreeRootList*		cur_root = pRoot->roots;
		while(cur_root)
		{
			OcTreeChildCheck(cur_root->node,intersection,callBack,data);
		
			cur_root = cur_root->next;
		}
			
		list = list->next;
	}

	//m_pApmOcTree->m_csCSection.Unlock();
	
	// World도 check하자 (character같은 경우는 world에 add)
	m_pfWorldIntersection(intersection,callBack,data);
}

void	AgcmOcTree::OcTreesForAllAtomicsIntersection3(INT32 ID, RpIntersection*	intersection,
											 	RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL	bLock )
{
	if(intersection->type != rpINTERSECTLINE)		// 현재는 Line만 체크
		return;

	++m_ulCurTick;

	RwLine		line = intersection->t.line;

	IntersectionSectors*	list = LineGetIntersectionSectors(&line);

	OcTreeRoot*				pRoot;
	OcRootData*				pRootData;

	//m_pApmOcTree->m_csCSection.Lock();

	while(list)
	{
		pRoot = m_pApmOcTree->GetRoot(list->pSector->GetArrayIndexX(),list->pSector->GetArrayIndexZ());
		if(!pRoot)
		{
			list = list->next;
			continue;
		}

		pRootData = GetRootData(pRoot);
		if(!pRootData)
		{
			list = list->next;
			continue;
		}

		// OcTree 체크
		OcTreeRootList*		cur_root = pRoot->roots;
		while(cur_root)
		{
			OcTreeChildCheck(cur_root->node,intersection,callBack,data);
		
			cur_root = cur_root->next;
		}
			
		list = list->next;
	}

	//m_pApmOcTree->m_csCSection.Unlock();
}

void	AgcmOcTree::OcTreesForAllAtomicsIntersection4(INT32 ID, RpIntersection*	intersection,
											 	RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL	bLock )
{
	// World도 check하자 (character같은 경우는 world에 add)
	m_pfWorldIntersection(intersection,callBack,data);
}

BOOL	AgcmOcTree::OcTreesForAllAtomicsIntersection5(RpIntersection*	intersection,	BOOL	bLock )
{
	if(intersection->type != rpINTERSECTLINE)		// 현재는 Line만 체크
		return	FALSE;

	++m_ulCurTick;

	RwLine		line = intersection->t.line;
	RwReal		dist;

	IntersectionSectors*	list = LineGetIntersectionSectors(&line);

	OcTreeRoot*				pRoot;
	OcRootData*				pRootData;

	//m_pApmOcTree->m_csCSection.Lock();

	while(list)
	{
		pRoot = m_pApmOcTree->GetRoot(list->pSector->GetArrayIndexX(),list->pSector->GetArrayIndexZ());
		if(!pRoot)
		{
			list = list->next;
			continue;
		}

		pRootData = GetRootData(pRoot);
		if(!pRootData)
		{
			list = list->next;
			continue;
		}

		// 지형 체크
		if(pRootData->terrain)
		{
			if(RtLineSphereIntersectionTest ( &line, RpAtomicGetBoundingSphere(pRootData->terrain) , &dist))	// Intersection!!
			{
				//m_pApmOcTree->m_csCSection.Unlock();
				return TRUE;
			}
		}

		list = list->next;
	}

	//m_pApmOcTree->m_csCSection.Unlock();

	return FALSE;
}

IntersectionSectors*	AgcmOcTree::LineGetIntersectionSectors(RwLine*	line)
{
	int		xdiff,zdiff;
	int		xunit,zunit;

	int		x1 = SectorIndexToArrayIndexX(PosToSectorIndexX(line->start.x));
	int		x2 = SectorIndexToArrayIndexX(PosToSectorIndexX(line->end.x));

	int		z1 = SectorIndexToArrayIndexZ(PosToSectorIndexZ(line->start.z));
	int		z2 = SectorIndexToArrayIndexZ(PosToSectorIndexZ(line->end.z));

	int		x = x1,z = z1;

	if(x1 > x2)
	{
		xdiff = x1 - x2;
		xunit = -1;
	}
	else 
	{
		xdiff = x2 - x1;
		xunit = 1;
	}

	if(z1 > z2)
	{
		zdiff = z1 - z2;
		zunit = -1;
	}
	else 
	{
		zdiff = z2 - z1;
		zunit = 1;
	}

	int		error_term = 0;
	
	ApWorldSector*	pSector;
	IntersectionSectors*	result = NULL;
	IntersectionSectors*	new_node;

	//m_pApmOcTree->m_csCSection.Lock();

	if(xdiff>zdiff)
	{
		for(int i=0; i<= xdiff; ++i)
		{
			pSector = m_pApmMap->GetSectorByArray(x,0,z);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}

			pSector = m_pApmMap->GetSectorByArray(x,0,z-1);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}

			pSector = m_pApmMap->GetSectorByArray(x,0,z+1);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}
			
			x += xunit;
			error_term += zdiff;
			if(error_term > xdiff)
			{
				error_term -= xdiff;
				z += zunit;
			}
		}
	}
	else 
	{
		for(int i=0; i<= zdiff; ++i)
		{
			pSector = m_pApmMap->GetSectorByArray(x,0,z);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}

			pSector = m_pApmMap->GetSectorByArray(x-1,0,z);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}

			pSector = m_pApmMap->GetSectorByArray(x+1,0,z);

			if(pSector)
			{
				new_node = (IntersectionSectors*)AcuFrameMemory::AllocFrameMemory(sizeof(IntersectionSectors));
				if( new_node )
				{
					new_node->pSector = pSector;
					new_node->next = result;
					result = new_node;
				}
			}
			
			z += zunit;
			error_term += xdiff;
			if(error_term > zdiff)
			{
				error_term -= zdiff;
				x += xunit;
			}
		}
	}

	//m_pApmOcTree->m_csCSection.Unlock();

	return result;
}


