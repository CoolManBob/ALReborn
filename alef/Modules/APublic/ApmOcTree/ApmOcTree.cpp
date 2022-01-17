#include "MagDebug.h"
#include "ApmOcTree.h"
#include "ApMemoryTracker.h"

#define	APMOCTREE_OCTREE_FILE_VERSION	1

ApmOcTree::ApmOcTree()
{
	SetModuleName("ApmOcTree");

	m_pfSaveCB = NULL;
	m_pfLoadCB = NULL;

	for(INT32	i=0;i<MAP_WORLD_INDEX_WIDTH;++i)
	{
		for(INT32 j=0;j<MAP_WORLD_INDEX_HEIGHT;++j)
		{
			m_pOcTreeRoots[i][j] = NULL;
			m_aeOcTreeRootStatus[i][j] = APMOCTREE_ROOT_STATUS_EMPTY;
		}
	}

	m_bOcTreeEnable = FALSE;

	m_pCenterSector = NULL;
	m_iLoadRange = 0;

	m_iCurLoadVersion = 0;

	//m_listFO = NULL;
	//m_iFOCount = 0;
}

ApmOcTree::~ApmOcTree()
{

}

BOOL	ApmOcTree::OnAddModule	()
{
	m_pApmMap = (ApmMap*)GetModule("ApmMap");

	SetModuleData( sizeof( OcTreeNode	),	OCTREE_NODE_DATA	);
	SetModuleData( sizeof( OcTreeRoot	),	OCTREE_ROOT_DATA	);
	
	return TRUE;
}

BOOL	ApmOcTree::OnInit		()
{
	m_uiIndexMask[0] = 0x00000000;
	m_uiIndexMask[1] = 0x000001C0;
	m_uiIndexMask[2] = 0x00000E00;
	m_uiIndexMask[3] = 0x00007000;
	m_uiIndexMask[4] = 0x00038000;
	m_uiIndexMask[5] = 0x001C0000;
	m_uiIndexMask[6] = 0x00E00000;
	m_uiIndexMask[7] = 0x07000000;

	m_uiIndexShift[0] = 0;
	m_uiIndexShift[1] = 6;
	m_uiIndexShift[2] = 9;
	m_uiIndexShift[3] = 12;
	m_uiIndexShift[4] = 15;
	m_uiIndexShift[5] = 18;
	m_uiIndexShift[6] = 21;
	m_uiIndexShift[7] = 24;

	if(m_bOcTreeEnable && m_pApmMap)
	{
		// 어차피 Object가 Add될때 OcTree Data를 읽기 때문에 Main Thread에서만 처리할 수 있다.
		m_pApmMap->SetCallbackLoadSector(CB_LoadSector,this);
		m_pApmMap->SetCallbackClearSector(CB_ClearSector,this);
	}
	
	return TRUE;
}

void	ApmOcTree::DestroyAll()
{
	for(INT32	i=0;i<MAP_WORLD_INDEX_WIDTH;++i)
	{
		for(INT32 j=0;j<MAP_WORLD_INDEX_HEIGHT;++j)
		{
			if(m_pOcTreeRoots[ i ][ j ])
			{
				DestroyTree( m_pOcTreeRoots[ i ][ j ] );

				m_pOcTreeRoots[ i ][ j ] = NULL;
			}
		}
	}
}

BOOL	ApmOcTree::OnDestroy	()
{
	m_csCSection.Lock();

	DestroyAll();
	
	/*OcTreeFOList*	cur_FO = m_listFO;
	OcTreeFOList*	remove_FO;
	while(cur_FO)
	{
		remove_FO = cur_FO;
		cur_FO = cur_FO->next;

		CloseHandle(remove_FO->fd);
		delete remove_FO;
	}

	m_listFO = NULL;
	*/

	m_csCSection.Unlock();
	
	return TRUE;
}

void	ApmOcTree::DestroyTree(OcTreeRoot*	root)
{
	PROFILE("ApmOcTree::DestroyTree");

	if (!root)
		return;

	OcTreeRootList*		cur_root = root->roots;
	OcTreeRootList*		remove_root;

	//m_csCSection.Lock();

	for(int i=0;i<root->rootnum;++i)
	{
		DestroyChilds(cur_root->node);
		remove_root = cur_root;
		
		cur_root = cur_root->next;
		delete remove_root;
	}

	root->rootnum = 0;
	root->roots = NULL;

	root->m_csCSection.Destroy();

	DestroyModuleData(root,OCTREE_ROOT_DATA);

	//m_csCSection.Unlock();
}

void	ApmOcTree::DestroyChilds(OcTreeNode*	node)
{
	//m_csCSection.Lock();

	for(int i=0;i<8;++i)
	{
		if(node->child[i])
			DestroyChilds(node->child[i]);
	}

	DestroyModuleData(node,OCTREE_NODE_DATA);

	//m_csCSection.Unlock();
}

// sector index를 넘겨서 root를 가져온다.
OcTreeRoot*		ApmOcTree::GetRoot(INT32	sectorix,	INT32	sectoriz)
{
	if(sectorix < 0 || sectorix >= MAP_WORLD_INDEX_WIDTH || sectoriz < 0 || sectoriz >= MAP_WORLD_INDEX_HEIGHT)
		return NULL;

	return	m_pOcTreeRoots[sectorix][sectoriz];
}

BOOL			ApmOcTree::SetRoot(INT32	sectorix,	INT32	sectoriz,	OcTreeRoot*	root)
{
	if(sectorix < 0 || sectorix >= MAP_WORLD_INDEX_WIDTH || sectoriz < 0 || sectoriz >= MAP_WORLD_INDEX_HEIGHT)
		return NULL;

	if (m_pOcTreeRoots[sectorix][sectoriz] && m_pOcTreeRoots[sectorix][sectoriz] != root)
		return FALSE;

	m_pOcTreeRoots[sectorix][sectoriz] = root;

	return TRUE;
}

// world x,z를 넘겨서 root를 가져온다.
OcTreeRoot*		ApmOcTree::GetRootByWorldPos(FLOAT	world_x, FLOAT world_z, BOOL bLoad)
{
	// BackThread로 LoadFromFiles를 다 뺐기 때문에 여기서는 load해주지 않는다.
	bLoad = FALSE;

	INT32	six = PosToSectorIndexX(world_x);
	INT32	siz = PosToSectorIndexZ(world_z);

	six = SectorIndexToArrayIndexX(six);
	siz = SectorIndexToArrayIndexZ(siz);

	if(six < 0 || six >= MAP_WORLD_INDEX_WIDTH || siz < 0 || siz >= MAP_WORLD_INDEX_HEIGHT)
		return NULL;

	if (!m_pOcTreeRoots[six][siz] && bLoad)
	{
		m_pOcTreeRoots[six][siz] = LoadFromFiles(six,siz);
	}

	return m_pOcTreeRoots[six][siz];
}

OcTreeNode*		ApmOcTree::GetNode(OcTreeRoot*		root,UINT32 ID)
{
	INT32		root_index = CalcRootIndex(ID);
	OcTreeRootList*		cur_rootnode = root->roots;
	while(cur_rootnode)
	{
		if(cur_rootnode->rootindex == root_index)
		{
			return GetNode(cur_rootnode->node,ID);
		}

		cur_rootnode = cur_rootnode->next;
	}

	return NULL;
}

OcTreeNode*		ApmOcTree::GetNode(OcTreeNode*	start,UINT32 ID)
{
	OcTreeNode*		res_node = start;

	int depth = CalcDepth(ID);

	for(int i=0;i<depth;++i)
	{
		if(res_node)
		{
			res_node = res_node->child[CalcIndex(ID,i+1)];
		}
		else return NULL;
	}

	return res_node;
}

// world x,y,z 를 넘겨서 시작 node를 가져온다.
OcTreeNode*		ApmOcTree::GetStartNode(FLOAT x,FLOAT y,FLOAT z)
{
	INT32	six = PosToSectorIndexX(x);
	INT32	siz = PosToSectorIndexZ(z);

	six = SectorIndexToArrayIndexX(six);
	siz = SectorIndexToArrayIndexZ(siz);

	OcTreeRoot*		root = GetRoot(six,siz);
	if(!root)
		return NULL;

	OcTreeRootList*		cur_node = root->roots;
	AuPOS	pos = {x,y,z};

	AuPOS	center;
	FLOAT	size;

	while(cur_node)
	{
		if(cur_node->node)
		{
			center = cur_node->node->BS.center;
			size = (FLOAT)cur_node->node->hsize;  

			if(pos.x < center.x + size && pos.x >= center.x - size &&
				pos.y < center.y + size && pos.y >= center.y - size &&
				pos.z < center.z + size && pos.z >= center.z - size)
			{
				return cur_node->node;
			}
		}
		
		cur_node = cur_node->next;
	}

	// root가 없었으므로 root생성 리턴(현재 y가 현재 rootlist에서 없을수 있음)
	OcTreeNode*	node = CreateRoot(six,siz,y, &root);

	if (!GetRoot(six, siz))
		SetRoot(six, siz, root);

	return node;
}

// 시작 node를 가져온다. 현재 node로..
OcTreeNode*		ApmOcTree::GetStartNode(OcTreeNode*		node)
{
	OcTreeNode*	cur_node = node;
	if(cur_node)
	{
		while(cur_node->parent)
		{
			cur_node = cur_node->parent;
		}
		return cur_node;
	}
	else 
		return 	NULL;
}

OcTreeNode*		ApmOcTree::GetStartNode(OcTreeRoot*		root,INT32	rootindex)
{
	if( root == NULL )
		return NULL;

	OcTreeRootList*		cur_root = root->roots ;
	for(int i=0;i<rootindex;++i)
	{
		if(cur_root)
		{
			cur_root = cur_root->next;
		}
		else
		{
			return NULL;
		}
	}

	if(cur_root)	return cur_root->node;
	else return NULL;
}

UINT32			ApmOcTree::GetNodeID(OcTreeNode*		node)
{
	return node->ID;
}

BOOL	ApmOcTree::TestBoxPos(AuPOS*	worldpos,AuPOS*	boxcenter,FLOAT	boxsize)
{
	if(worldpos->x < boxcenter->x + boxsize && worldpos->x >= boxcenter->x - boxsize &&
		worldpos->y < boxcenter->y + boxsize && worldpos->y >= boxcenter->y - boxsize &&
		worldpos->z < boxcenter->z + boxsize && worldpos->z >= boxcenter->z - boxsize)
	{
		return TRUE;
	}
	else return FALSE;
}

// 현재 노드를 8개로 분할(현재 노드는 leaf 이어야 함)
void	ApmOcTree::DivideNode(OcTreeNode*	node)
{
	if(node->bHasChild == TRUE || node->level >= OCTREE_MAXDEPTH) return;
	
	node->bHasChild = TRUE;
	node->ID = SetIsLeaf(node->ID,FALSE);
		
	for(int i=0;i<8;++i)
	{
		node->child[i] = MakeChild(node,(APMOCTREE_NODES)i);
	}
}

// 현재 노드의 자식들을 통합
void	ApmOcTree::CombineNodeChilds(OcTreeNode*	node)
{
	if(node->level >= OCTREE_MAXDEPTH || !node->bHasChild ) return;	// 7은 자식이 없다.

	node->bHasChild = FALSE;
	node->ID = SetIsLeaf(node->ID,TRUE);

	for(int i=0;i<8;++i)
	{
		if(node->child[i])
		{
			CombineNodeChilds(node->child[i]);
			DestroyModuleData(node->child[i],OCTREE_NODE_DATA);
			node->child[i] = NULL;
		}
	}
}

OcTreeNode*		ApmOcTree::MakeChild(OcTreeNode*	node,APMOCTREE_NODES	pos)
{
	//m_csCSection.Lock();

	OcTreeNode*		nw_node = (OcTreeNode*)CreateModuleData(OCTREE_NODE_DATA);
	int nwlev = node->level + 1;

	nw_node->parent = node;
	//nw_node->hsize = node->hsize/2;
	nw_node->hsize = node->hsize>>1;
	
	nw_node->level = nwlev;
	
	for(int i=0;i<8;++i)
		nw_node->child[i] = NULL;

	nw_node->bHasChild = FALSE;
	nw_node->ID = node->ID;

	nw_node->ID |= ((pos & 0x7) << m_uiIndexShift[nwlev]);
	nw_node->ID = SetDepth(nw_node->ID,nwlev);
	nw_node->ID = SetIsLeaf(nw_node->ID,TRUE);

	nw_node->objectnum = 0;
	
	nw_node->BS.radius = (float)nw_node->hsize * OCTREE_SPHEREVAL;

	switch(pos)
	{
	case APMOCTREE_TOP_LEFT_FRONT:
		nw_node->BS.center.x = node->BS.center.x - nw_node->hsize;
		nw_node->BS.center.y = node->BS.center.y + nw_node->hsize;
		nw_node->BS.center.z = node->BS.center.z + nw_node->hsize;
	break;

	case APMOCTREE_TOP_LEFT_BACK:
		nw_node->BS.center.x = node->BS.center.x - nw_node->hsize;
		nw_node->BS.center.y = node->BS.center.y + nw_node->hsize;
		nw_node->BS.center.z = node->BS.center.z - nw_node->hsize;
	break;

	case APMOCTREE_TOP_RIGHT_BACK:
		nw_node->BS.center.x = node->BS.center.x + nw_node->hsize;
		nw_node->BS.center.y = node->BS.center.y + nw_node->hsize;
		nw_node->BS.center.z = node->BS.center.z - nw_node->hsize;
	break;

	case APMOCTREE_TOP_RIGHT_FRONT:
		nw_node->BS.center.x = node->BS.center.x + nw_node->hsize;
		nw_node->BS.center.y = node->BS.center.y + nw_node->hsize;
		nw_node->BS.center.z = node->BS.center.z + nw_node->hsize;
	break;

	case APMOCTREE_BOTTOM_LEFT_FRONT:
		nw_node->BS.center.x = node->BS.center.x - nw_node->hsize;
		nw_node->BS.center.y = node->BS.center.y - nw_node->hsize;
		nw_node->BS.center.z = node->BS.center.z + nw_node->hsize;
	break;

	case APMOCTREE_BOTTOM_LEFT_BACK:
		nw_node->BS.center.x = node->BS.center.x - nw_node->hsize;
		nw_node->BS.center.y = node->BS.center.y - nw_node->hsize;
		nw_node->BS.center.z = node->BS.center.z - nw_node->hsize;
	break;

	case APMOCTREE_BOTTOM_RIGHT_BACK:
		nw_node->BS.center.x = node->BS.center.x + nw_node->hsize;
		nw_node->BS.center.y = node->BS.center.y - nw_node->hsize;
		nw_node->BS.center.z = node->BS.center.z - nw_node->hsize;
	break;

	case APMOCTREE_BOTTOM_RIGHT_FRONT:
		nw_node->BS.center.x = node->BS.center.x + nw_node->hsize;
		nw_node->BS.center.y = node->BS.center.y - nw_node->hsize;
		nw_node->BS.center.z = node->BS.center.z + nw_node->hsize;
	break;
	}

	//m_csCSection.Unlock();

	return nw_node;
}

// 0 level 생성
OcTreeNode*		ApmOcTree::CreateRoot(INT32 six,INT32 siz,FLOAT cy, OcTreeRoot**	ppRoot)
{
	PROFILE("ApmOcTree::CreateRoot");

	if(six < 0 || six >= MAP_WORLD_INDEX_WIDTH || siz < 0 || siz >= MAP_WORLD_INDEX_HEIGHT)
		return NULL;

	//m_csCSection.Lock();

	OcTreeNode*	nw_node =  (OcTreeNode*)CreateModuleData(OCTREE_NODE_DATA);

	OcTreeRootList*	nw_list = new OcTreeRootList;
	OcTreeRoot*		root = *ppRoot;

	nw_list->node = nw_node;
	nw_list->next = NULL;

	FLOAT	cyNew = cy;
	ApWorldSector * pSector = m_pApmMap->GetSectorByArrayIndex( six , siz );
	FLOAT	fMinHeight;

	if( NULL == pSector )
		fMinHeight = SECTOR_MIN_HEIGHT;
	else
		fMinHeight = this->m_pApmMap->GetMinHeight( pSector );

	// 마고자 (2005-12-06 오전 10:51:47) : 
	// Min Height에 걸린 녀석이 있는경우 계산에 문제가 생겨 상수값을 곱함.
	FLOAT	fMinHeightOffset = ( MAP_SECTOR_WIDTH / 2.0f ) * 0.95f;

	if( cyNew - fMinHeightOffset < fMinHeight ) 
	{
		cyNew = fMinHeight + fMinHeightOffset;
	}

	if(root == NULL)
	{
		nw_list->rootindex = 0;

		root = (OcTreeRoot*)CreateModuleData(OCTREE_ROOT_DATA);

		root->m_csCSection.Init();

		root->rootnum = 1;
		root->sectorX = six;
		root->sectorZ = siz;

		int		sector_x = ArrayIndexToSectorIndexX(six);
		int		sector_z = ArrayIndexToSectorIndexZ(siz);

		root->centerX = (GetSectorStartX(sector_x) + GetSectorEndX(sector_x)) * 0.5f;
		root->centerZ = (GetSectorStartZ(sector_z) + GetSectorEndZ(sector_z)) * 0.5f;
		
		root->roots = nw_list;

		EnumCallback(APMOCTREE_CB_ID_INIT_ROOT,(PVOID)root,NULL);
	}
	else
	{
		BOOL				bFind = FALSE;

		OcTreeRootList*		cur_rootlist = root->roots;
		while(cur_rootlist)
		{
			AuPOS	pos;
			pos.x = root->centerX;
			pos.y = cyNew;
			pos.z = root->centerZ;

			if(TestBoxPos(&pos,&cur_rootlist->node->BS.center,(FLOAT)cur_rootlist->node->hsize))
			{
				bFind = TRUE;
				break;
			}
			
			cur_rootlist = cur_rootlist->next;
		}

		if(!bFind)
		{
			if( root->rootnum < OCTREE_MAX_ROOT_COUNT )
			{
				nw_list->rootindex = root->rootnum;

				OcTreeRootList*		find_end_ptr = root->roots;
				while(find_end_ptr && find_end_ptr->next)
				{
					find_end_ptr = find_end_ptr->next;
				}
				find_end_ptr->next = nw_list;
				
				++root->rootnum;

				// 마고자 (2005-09-06 오후 1:14:36) : 
				// 새로 생설할때..
				{

					// 마고자 (2005-09-06 오전 10:37:53) : 
					// 박스 높이 보정
					{
						OcTreeRootList*		pRoot = root->roots;
						while(pRoot && pRoot->next)
						{
							// 혹시 박스가 겹치나?
							if( pRoot->node->BS.center.y - pRoot->node->hsize > cyNew + pRoot->node->hsize &&
								cyNew > pRoot->node->BS.center.y - pRoot->node->hsize * 2 )
							{
								cyNew = pRoot->node->BS.center.y - pRoot->node->hsize * 2;
							}
							else
							if( pRoot->node->BS.center.y + pRoot->node->hsize > cyNew - pRoot->node->hsize &&
								cyNew < pRoot->node->BS.center.y + pRoot->node->hsize * 2 )
							{
								cyNew = pRoot->node->BS.center.y + pRoot->node->hsize * 2;
							}

							pRoot = pRoot->next;
						}
					}
				}
			}
			else 
			{
				// 마고자 (2005-09-07 오후 4:22:45) : 
				// 최대 루트 갯수 오버.
				// 이경우 해당 지역에 옥트리 정보가 생성돼지 않을 수 있음.

				delete nw_list;	
				DestroyModuleData((PVOID)nw_node,OCTREE_NODE_DATA);

				//m_csCSection.Unlock();

				return	NULL;
			}
		}
	}

	nw_node->bHasChild = FALSE;
	//nw_node->hsize = (INT32)MAP_SECTOR_WIDTH/2;
	nw_node->hsize = (INT32)MAP_SECTOR_WIDTH>>1;

	nw_node->BS.center.x = root->centerX;
	nw_node->BS.center.y = cyNew;
	nw_node->BS.center.z = root->centerZ;

	nw_node->BS.radius = nw_node->hsize * OCTREE_SPHEREVAL;

	nw_node->level = 0;
	nw_node->objectnum = 0;
	nw_node->parent = NULL;
	nw_node->ID = SetRootIndex(0x00000000,(root->rootnum-1));
	nw_node->ID = SetDepth(nw_node->ID,0);
	nw_node->ID = SetIsLeaf(nw_node->ID,TRUE);

	for(int i=0;i<8;++i)
	{
		nw_node->child[i] = NULL;
	}

	*ppRoot = root;

	//m_csCSection.Unlock();
	
//	for(int i=0;i<8;++i)
//	{
//		nw_node->child[i] = MakeChild(nw_node->child[i],(APMOCTREE_NODES)i);
//	}

	return nw_node;
}

INT32		ApmOcTree::AttachOcTreeNodeData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, OCTREE_NODE_DATA, nDataSize, pfConstructor, pfDestructor);	
}

INT32		ApmOcTree::AttachOcTreeRootData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, OCTREE_ROOT_DATA, nDataSize, pfConstructor, pfDestructor);	
}

BOOL		ApmOcTree::SetCallbackInitRoot	( ApModuleDefaultCallBack pfCallback, PVOID pClass	)
{
	return SetCallback(APMOCTREE_CB_ID_INIT_ROOT, pfCallback, pClass);
}

// level8까지 강제 분할
void	ApmOcTree::DivideAllTree(OcTreeRoot*	pRoot)
{
	OcTreeRootList*		cur_root = pRoot->roots;
	for(int i=0;i<pRoot->rootnum ;++i)
	{
		DivideAllChilds(cur_root->node);

		cur_root = cur_root->next;
	}
}

void	ApmOcTree::DivideAllChilds(OcTreeNode*	pNode)
{
	if(pNode->level >= OCTREE_MAXDEPTH) return;

	pNode->bHasChild = TRUE;
	
	pNode->ID = SetIsLeaf(pNode->ID,FALSE);
	
	for(int i=0;i<8;++i)
	{
		if(!pNode->child[i])
			pNode->child[i] = MakeChild(pNode,(APMOCTREE_NODES)i);
	}

	for(int i=0;i<8;++i)
	{
		DivideAllChilds(pNode->child[i]);
	}
}

OcTreeNode*		ApmOcTree::GetNode(AuPOS*	pos)								// pos를 넘겨서 leaf를 가져온다.
{
	INT32	six = PosToSectorIndexX(pos->x);
	INT32	siz = PosToSectorIndexZ(pos->z);

	six = SectorIndexToArrayIndexX(six);
	siz = SectorIndexToArrayIndexZ(siz);

	OcTreeRoot*		pRoot = GetRoot(six,siz);

	if(!pRoot) return NULL;
	
	if(pRoot->rootnum == 1)
	{
		return GetNode(pRoot->roots->node,pos);
	}
	else 
	{
		OcTreeRootList*		cur_root = pRoot->roots;
		for(int i=0;pRoot->rootnum;++i)
		{
			if(pos->y < cur_root->node->BS.center.y + cur_root->node->hsize && 
				pos->y >= cur_root->node->BS.center.y - cur_root->node->hsize)
			{		// 포함
				return GetNode(cur_root->node,pos);
				break;
			}

			cur_root = cur_root->next;
		}
	}

	return NULL;
}

OcTreeNode*		ApmOcTree::GetNode(OcTreeNode*	node,AuPOS*	pos)
{
	AuPOS	center;
	FLOAT	size;

	if(node->bHasChild == FALSE) return node;
	else
	{
		for(int i=0;i<8;++i)
		{
			center = node->child[i]->BS.center;
			size = (FLOAT)node->child[i]->hsize;

			if(pos->x < center.x + size && pos->x >= center.x - size &&
				pos->y < center.y + size && pos->y >= center.y - size &&
				pos->z < center.z + size && pos->z >= center.z - size)
			{
				return GetNode(node->child[i],pos);
				break;
			}
		}
	}

	return NULL;
}

OcTreeNode*		ApmOcTree::GetNodeForInsert(INT32 six,INT32 siz,UINT32	octreeID,BOOL bDivide)
{
	OcTreeRoot*	root = GetRoot(six,siz);	

	if(root == NULL) 
	{
		//버벅거림 때문에 뺏다 .. 기존에 이걸 왜 했을까 ㅜㅜ 2005.2.14 gemani
		//if(bLoad)
		//{
		//	root = LoadFromFiles(six,siz);
		//	if(!root)
		//		return NULL;
		//}
		//else
		//{
		//	return NULL;
		//}

		return NULL;
	}

	int		rootindex = CalcRootIndex(octreeID);

	OcTreeNode*		node = GetStartNode(root,rootindex);
	if(!node) 
		return NULL;

	int		depth = CalcDepth(octreeID);
	int		index;
	for(int i=0;i<depth;++i)
	{
		index = CalcIndex(octreeID,i+1);

		if(node->child[index] == NULL)
		{
			if(bDivide)
			{
				DivideNode(node);
				node = node->child[index];
			}
			else
				return NULL;
		}
		else
		{
			node = node->child[index];
		}
	}

	return node;
}

void	ApmOcTree::OptimizeTree(INT32	six,INT32 siz)
{
	OcTreeRoot*	root = GetRoot(six,siz);	
	if(root == NULL) return;

	OcTreeRootList*		cur_list = root->roots;
	for(int i=0;i<root->rootnum ;++i)
	{
		OptimizeChild(cur_list->node);

		cur_list = cur_list->next;
	}
}

// 하위 node들에 object가 없으면 하위 node 통합
int 	ApmOcTree::OptimizeChild(OcTreeNode*	node)
{
	if(node->bHasChild)
	{
		BOOL	bUnity = TRUE;
		int i =0 ;
		for(;i<8;++i)
		{
			if(OptimizeChild(node->child[i]))
			{
				bUnity = FALSE;
			}
		}

		if(bUnity == TRUE)
		{
			node->bHasChild = FALSE;

			for(i=0;i<8;++i)
			{
				DestroyModuleData((PVOID)node->child[i],OCTREE_NODE_DATA);

				node->child[i] = NULL;
			}

			// 현재 노드 ID leaf로 변경 ㅋㅋ.
			node->ID = SetIsLeaf(node->ID,TRUE);
		}

		return node->objectnum;	
	}
	else
	{
		return node->objectnum;
	}
}

// pos로 단지 가상의 leafID를 계산해서 return 준다.
UINT32	ApmOcTree::GetLeafID(AuPOS*	pos)
{
/*	OcTreeNode*		start_node = GetStartNode(pos->x,pos->y,pos->z);
	if(!start_node)		return 0xffffffff;		// error!

	UINT32		res_ID = start_node->ID;

	res_ID = SetDepth(res_ID,OCTREE_MAXDEPTH);
	res_ID = SetIsLeaf(res_ID,TRUE);

	AuSPHERE	BS;
	BS = start_node->BS;
	AuSPHERE	calcBS;
	float		dist = (float)(start_node->hsize/2);
	float		dx,dy,dz,d1,d2;
	
	for(int i=0;i<OCTREE_MAXDEPTH;++i)
	{
		calcBS.radius = dist * OCTREE_SPHEREVAL;
		for(int j=0;j<8;++j)
		{
			if((APMOCTREE_NODES)j == APMOCTREE_TOP_LEFT_FRONT)
			{
				calcBS.center.x = BS.center.x - dist;calcBS.center.y = BS.center.y + dist;calcBS.center.z = BS.center.z + dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_TOP_LEFT_BACK)
			{
				calcBS.center.x = BS.center.x - dist;calcBS.center.y = BS.center.y + dist;calcBS.center.z = BS.center.z - dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_TOP_RIGHT_BACK)
			{
				calcBS.center.x = BS.center.x + dist;calcBS.center.y = BS.center.y + dist;calcBS.center.z = BS.center.z - dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_TOP_RIGHT_FRONT)
			{
				calcBS.center.x = BS.center.x + dist;calcBS.center.y = BS.center.y + dist;calcBS.center.z = BS.center.z + dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_BOTTOM_LEFT_FRONT)
			{
				calcBS.center.x = BS.center.x - dist;calcBS.center.y = BS.center.y - dist;calcBS.center.z = BS.center.z + dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_BOTTOM_LEFT_BACK)
			{
				calcBS.center.x = BS.center.x - dist;calcBS.center.y = BS.center.y - dist;calcBS.center.z = BS.center.z - dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_BOTTOM_RIGHT_BACK)
			{
				calcBS.center.x = BS.center.x + dist;calcBS.center.y = BS.center.y - dist;calcBS.center.z = BS.center.z - dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_BOTTOM_RIGHT_FRONT)
			{
				calcBS.center.x = BS.center.x + dist;calcBS.center.y = BS.center.y - dist;calcBS.center.z = BS.center.z + dist;
			}

			dx = calcBS.center.x - pos->x;
			dy = calcBS.center.y - pos->y;
			dz = calcBS.center.z - pos->z;

			d1 = dx*dx + dy*dy + dz*dz;
			d2 = (float)sqrt(d1);

			if(d2 < calcBS.radius)
			{
				BS = calcBS;

				// level i+1의 ID Set
				res_ID |= ((j & 0x7) << m_uiIndexShift[i+1]);
				
				break;
			}
		}

		if(j==8)		// sphere안에 없었다.. check
		{
			return 0xffffffff;
		}

		dist *= 0.5f;
	}

	return res_ID;*/

	OcTreeNode*		start_node = GetStartNode(pos->x,pos->y,pos->z);
	if(!start_node)		return 0xffffffff;		// error!

	UINT32		res_ID = start_node->ID;

	res_ID = SetDepth(res_ID,OCTREE_MAXDEPTH);
	res_ID = SetIsLeaf(res_ID,TRUE);

	AuSPHERE	BS;
	BS = start_node->BS;
	AuSPHERE	calcBS;
	//float		dist = (float)(start_node->hsize/2);
	float		dist = (float)(start_node->hsize>>1);
		
	for(int i=0;i<OCTREE_MAXDEPTH;++i)
	{
		int j = 0;
		for(;j<8;++j)
		{
			if((APMOCTREE_NODES)j == APMOCTREE_TOP_LEFT_FRONT)
			{
				calcBS.center.x = BS.center.x - dist;calcBS.center.y = BS.center.y + dist;calcBS.center.z = BS.center.z + dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_TOP_LEFT_BACK)
			{
				calcBS.center.x = BS.center.x - dist;calcBS.center.y = BS.center.y + dist;calcBS.center.z = BS.center.z - dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_TOP_RIGHT_BACK)
			{
				calcBS.center.x = BS.center.x + dist;calcBS.center.y = BS.center.y + dist;calcBS.center.z = BS.center.z - dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_TOP_RIGHT_FRONT)
			{
				calcBS.center.x = BS.center.x + dist;calcBS.center.y = BS.center.y + dist;calcBS.center.z = BS.center.z + dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_BOTTOM_LEFT_FRONT)
			{
				calcBS.center.x = BS.center.x - dist;calcBS.center.y = BS.center.y - dist;calcBS.center.z = BS.center.z + dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_BOTTOM_LEFT_BACK)
			{
				calcBS.center.x = BS.center.x - dist;calcBS.center.y = BS.center.y - dist;calcBS.center.z = BS.center.z - dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_BOTTOM_RIGHT_BACK)
			{
				calcBS.center.x = BS.center.x + dist;calcBS.center.y = BS.center.y - dist;calcBS.center.z = BS.center.z - dist;
			}
			else if((APMOCTREE_NODES)j == APMOCTREE_BOTTOM_RIGHT_FRONT)
			{
				calcBS.center.x = BS.center.x + dist;calcBS.center.y = BS.center.y - dist;calcBS.center.z = BS.center.z + dist;
			}

			if( pos->x >= calcBS.center.x - dist 
			&&  pos->x < calcBS.center.x + dist 
			&&  pos->y >= calcBS.center.y - dist 
			&&  pos->y < calcBS.center.y + dist 
			&&  pos->z >= calcBS.center.z - dist 
			&&  pos->z < calcBS.center.z + dist )
			{
				BS = calcBS;

				// level i+1의 ID Set
				res_ID |= ((j & 0x7) << m_uiIndexShift[i+1]);
				
				break;
			}
		}

		if(j==8)		// sphere안에 없었다.. check
		{
			return 0xffffffff;
		}

		dist *= 0.5f;
	}

	return res_ID;
}

BOOL	ApmOcTree::SaveToFiles(char*	szDir,INT32 startx,INT32 endx,INT32 startz,INT32 endz)
{
	INT32 lstartx = startx / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;
	INT32 lstartz = startz / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;
	
	if(startx < 0 && startx % MAP_DEFAULT_DEPTH != 0) lstartx -= MAP_DEFAULT_DEPTH;
	if(startz < 0 && startz % MAP_DEFAULT_DEPTH != 0) lstartz -= MAP_DEFAULT_DEPTH;

	INT32 lendx = endx / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH ;
	INT32 lendz = endz / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH ;
	
	if(endx < 0 && endx % MAP_DEFAULT_DEPTH != 0) lendx -= MAP_DEFAULT_DEPTH;
	if(endz < 0 && endz % MAP_DEFAULT_DEPTH != 0) lendz -= MAP_DEFAULT_DEPTH;

	OcTreeRootList*		cur_rootlist;
	char				fstr[150];
	HANDLE				fd;
	DWORD				FP;
	INT32				foffset;
	//INT32				temp[100];

	INT32				root_num;

	//INT32	x_arrayindex,z_arrayindex;
	INT32	division_index;
	
	for(INT32	i = lstartz; i < lendz; i += MAP_DEFAULT_DEPTH)
	{
	  for(INT32	j = lstartx; j < lendx; j+= MAP_DEFAULT_DEPTH)
	  {
        memset(fstr,'\0',150);

		//x_arrayindex = SectorIndexToArrayIndexX(j);
		//z_arrayindex = SectorIndexToArrayIndexZ(i);
		division_index = GetDivisionIndex( j , i );

		if(!szDir)
		{
			sprintf(fstr,"world\\octree\\ot%d.dat",division_index);
		}
		else
		{
			sprintf(fstr,"%s\\octree\\ot%d.dat",szDir,division_index);
		}
		
		fd=CreateFile(fstr,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(fd == INVALID_HANDLE_VALUE)
		{
			if(!szDir)
			{
				char	fdirc[100];
				memset(fdirc,'\0',100);
				 
				sprintf(fdirc,"world\\octree");
				CreateDirectory(fdirc,NULL);
			}
			else
			{	
				CreateDirectory(szDir,NULL);
			}

			fd=CreateFile(fstr,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		}

		if(fd == INVALID_HANDLE_VALUE)
		{
			DWORD	as = GetLastError();
			return FALSE;
		}

		// Vserion History:
		// 1 : file offset 미리 계산
		// 2 : 메모리 사이즈 & 파일 사이즈 다이어트

		DWORD	version = APMOCTREE_OCTREE_FILE_VERSION;
		WriteFile(fd,&version,sizeof(version),&FP,NULL);
		
		foffset = MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH * 8 + 4;	// version정보 추가
		INT32	before_offset;
		INT32	fsize;

		//memset(temp,0,400);
		//WriteFile(fd,&temp,400,&FP,NULL);

		for(INT32	k = 0; k<MAP_DEFAULT_DEPTH ; ++k)
		{
		  for(INT32	l = 0; l<MAP_DEFAULT_DEPTH; ++l)
		  {
			SetFilePointer(fd,foffset,NULL,FILE_BEGIN);
			before_offset = foffset;

			if(!m_pOcTreeRoots[j+l][i+k]) 
			{
				// root num 저장
				root_num = 0;
				WriteFile(fd,&root_num,sizeof(INT32),&FP,NULL);			
				foffset += 4;

				fsize = foffset - before_offset;
				SetFilePointer(fd,k*8*MAP_DEFAULT_DEPTH  + l*8 + 4,NULL,FILE_BEGIN);
				WriteFile(fd,&before_offset,sizeof(INT32),&FP,NULL);
				WriteFile(fd,&fsize,sizeof(INT32),&FP,NULL);
				continue;
			}
			
			// root num 저장
			root_num = m_pOcTreeRoots[j+l][i+k]->rootnum;
			WriteFile(fd,&root_num,sizeof(INT32),&FP,NULL);			
			foffset += 4;

			cur_rootlist = m_pOcTreeRoots[j+l][i+k]->roots;
			while(cur_rootlist)
			{
				// center y 저장
				WriteFile(fd,&cur_rootlist->node->BS.center.y ,sizeof(float),&FP,NULL);			
				foffset += 4;
				
				SaveNode(cur_rootlist->node,&foffset,fd,&FP);
				
				cur_rootlist = cur_rootlist->next;
			}
			
			if(m_pfSaveCB)	m_pfSaveCB(fd,&FP,&foffset,m_pOcTreeRoots[j+l][i+k]);		// agcmOcTree에서 지형 BBOX저장위해

			// file 시작 위치 저장
			fsize = foffset - before_offset;
			SetFilePointer(fd,k*8*MAP_DEFAULT_DEPTH  + l*8 + 4,NULL,FILE_BEGIN);
			WriteFile(fd,&before_offset,sizeof(INT32),&FP,NULL);
			WriteFile(fd,&fsize,sizeof(INT32),&FP,NULL);
		  }
		}
		
		CloseHandle(fd);
	  }
	}
	return TRUE;
}

void	ApmOcTree::SaveNode(OcTreeNode*	node,INT32*		Foffset,HANDLE	fd,DWORD*	FP)
{
	// node data save
	UINT32		uiObjectNum = node->objectnum;
	BOOL		bHasChild = node->bHasChild;
	UINT32		uiHSize = node->hsize;
	UINT32		uiLevel = node->level;
	WriteFile(fd,&node->ID,sizeof(UINT32),FP,NULL);
	WriteFile(fd,&uiObjectNum,sizeof(UINT32),FP,NULL);
	WriteFile(fd,&bHasChild,sizeof(BOOL),FP,NULL);
	WriteFile(fd,&node->BS.radius,sizeof(float),FP,NULL);
	WriteFile(fd,&node->BS.center.x,sizeof(float),FP,NULL);
	WriteFile(fd,&node->BS.center.y,sizeof(float),FP,NULL);
	WriteFile(fd,&node->BS.center.z,sizeof(float),FP,NULL);
	WriteFile(fd,&uiHSize,sizeof(UINT32),FP,NULL);
	WriteFile(fd,&uiLevel,sizeof(UINT32),FP,NULL);
	(*Foffset) += 36;
	
	if(node->bHasChild)
	{
		for(int i=0;i<8;++i)
		{
			SaveNode(node->child[i],Foffset,fd,FP);
		}
	}
}

OcTreeRoot*	ApmOcTree::LoadFromFiles(INT32	six,INT32 siz)
{
	PROFILE("ApmOcTree::LoadFromFiles");

	INT32 lsix = six / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH; // MAP_DEFAULT_DEPTH 단위로 증가하기 위해
	INT32 lsiz = siz / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;

	if(six < 0 && six % MAP_DEFAULT_DEPTH != 0) lsix -= MAP_DEFAULT_DEPTH;
	if(siz < 0 && siz % MAP_DEFAULT_DEPTH != 0) lsiz -= MAP_DEFAULT_DEPTH;

	// 만약 다른 Thread에서 읽고 있다면, return;
	m_csCSection.Lock();

	if (m_aeOcTreeRootStatus[six][siz] == APMOCTREE_ROOT_STATUS_LOADING || m_aeOcTreeRootStatus[six][siz] == APMOCTREE_ROOT_STATUS_LOADED)
	{
		m_csCSection.Unlock();
		return NULL;
	}

	m_aeOcTreeRootStatus[six][siz] = APMOCTREE_ROOT_STATUS_LOADING;

	m_csCSection.Unlock();

	INT32 loffx = six - lsix;
	INT32 loffz = siz - lsiz;

	INT32	division_index = GetDivisionIndex( lsix , lsiz );
	char		fstr[100];
	memset(fstr,'\0',100);
	sprintf(fstr,"world\\octree\\OT%d.dat",division_index);

	ApdFile				csApdFile;
	AuPackingManager*		pPackingManager = AuPackingManager::GetSingletonPtr();
	if(!pPackingManager->OpenFile(fstr,&csApdFile))
	{
		//@{ kday 20050705
		//잠시주석
		// TODO : AgcmCharacter::LoadTemplateClump() 잡고 주석 풀것
		//MD_SetErrorMessage( "파일을 읽는데 실패하였습니다.OcTree File \"%02d\"" , GetDivisionIndex( lsix , lsiz ) );
		//@} kday
		return NULL;
	}

	INT32	foffset = 0;
	INT32	iLoadSize = 0;
	INT32	version = 0;
	pPackingManager->ReadFile( &version, 4, &csApdFile );

	m_iCurLoadVersion = version;

	//ASSERT( version == APMOCTREE_OCTREE_FILE_VERSION );
	//if( version != APMOCTREE_OCTREE_FILE_VERSION )
	//{
		// 데이타 에러?..
	//	pPackingManager->CloseFile(&csApdFile);
	//	return NULL;
	//}

	//foffset = loffz * MAP_DEFAULT_DEPTH * 8 + loffx * 8 + 4;
	foffset = loffz * (MAP_DEFAULT_DEPTH << 3) + (loffx << 3) + 4;
	pPackingManager->SetPos(foffset,&csApdFile);
	pPackingManager->ReadFile( &foffset, 4, &csApdFile );
	pPackingManager->ReadFile( &iLoadSize, 4, &csApdFile );

	// 2005/01/16 에러처리 추가.
	if( iLoadSize < 0 )
	{
		// 데이타 에러?..
		pPackingManager->CloseFile(&csApdFile);
		return NULL;
	}

	/*DWORD	foffset = 0;
	DWORD	foffset2 = 0;
	INT32	iLoadSize = 0;

	foffset = loffz * MAP_DEFAULT_DEPTH * 4  + loffx * 4;

	pPackingManager->SetPos(foffset,&csApdFile);
	pPackingManager->ReadFile( &foffset, 4, &csApdFile );

	if(loffx+1 >= MAP_DEFAULT_DEPTH)
	{
		if(loffz+1 >= MAP_DEFAULT_DEPTH)
		{
			foffset2 = pPackingManager->GetFileSize( &csApdFile );
		}
		else
		{
			foffset2 = (loffz + 1) * MAP_DEFAULT_DEPTH * 4;

			pPackingManager->SetPos(foffset2,&csApdFile);
			pPackingManager->ReadFile( &foffset2, 4, &csApdFile );
		}
	}
	else
	{
		foffset2 = loffz * MAP_DEFAULT_DEPTH * 4 + (loffx + 1) * 4;

		pPackingManager->SetPos(foffset2,&csApdFile);
		pPackingManager->ReadFile( &foffset2, 4, &csApdFile );
	}

	iLoadSize = foffset2 - foffset;
	*/

	DWORD*		loadBuffer = new DWORD[iLoadSize/4];
	INT32		iLoadIndex = 0;
	INT32		root_num = 0;
	
	pPackingManager->SetPos(foffset,&csApdFile);
	INT32	LoadFileSize = pPackingManager->ReadFile( loadBuffer, iLoadSize, &csApdFile );
	if (!LoadFileSize)
	{
		delete	[]loadBuffer;
		pPackingManager->CloseFile(&csApdFile);
		return NULL;
	}
	
	pPackingManager->CloseFile(&csApdFile);

	root_num = loadBuffer[iLoadIndex++];

	float	cy;
	OcTreeNode*		start_node;
	OcTreeRoot*		root = GetRoot(six, siz);

	for(int i=0;i<root_num;++i)
	{
		// center y load
		//ReadFile(fd,&cy ,sizeof(float),&FP,NULL);
		cy = ((float*)loadBuffer)[iLoadIndex++];
				
		start_node = CreateRoot(six,siz,cy, &root);

		// 마고자 (2005-09-07 오전 10:40:40) : 
		// 한 섹터에 루트가 4개까지 존재가 가능하다.
		// 그 이상 초과돼는 녀석은 NULL이 떨어지게 돼는데
		// 이런경우는 대부분 데이타 오류일 가능성이 높다.
		ASSERT( i < OCTREE_MAX_ROOT_COUNT );

		if( start_node )
		{
			LoadNode(start_node,loadBuffer,&iLoadIndex);
		}
		else
		{
			ASSERT( !"루트 생성 갯수 초과? 데이타 오류일 가능성이 높음" );
			MD_SetErrorMessage( "ApmOcTree::LoadFromFiles 옥트리 데이타 이상?" );
			break;
		}
	}

	if(root)
	{
		if(m_pfLoadCB)
			m_pfLoadCB(loadBuffer,&iLoadIndex,root);		// agcmOcTree에서 지형 BBOX 로드위해

		m_csCSection.Lock();

		if (m_aeOcTreeRootStatus[six][siz] != APMOCTREE_ROOT_STATUS_LOADING)
		{
			m_pOcTreeRoots[six][siz] = NULL;
			
			DestroyTree(root);

			m_aeOcTreeRootStatus[six][siz] = APMOCTREE_ROOT_STATUS_EMPTY;

			m_csCSection.Unlock();

			delete []loadBuffer;

			return NULL;
		}

		m_aeOcTreeRootStatus[six][siz] = APMOCTREE_ROOT_STATUS_LOADED;

		SetRoot(six, siz, root);

		m_csCSection.Unlock();
	}
	else
	{
		m_aeOcTreeRootStatus[six][siz] = APMOCTREE_ROOT_STATUS_EMPTY;
	}

	delete []loadBuffer;

	return root;
}

void	ApmOcTree::LoadNode(OcTreeNode*	node,DWORD* pLoadBuffer,INT32*	pLoadIndex)
{
	// 마고자 (2005-09-07 오전 10:36:02) : 
	// 널이 들어오면 안됌..
	ASSERT( NULL != node		);
	ASSERT( NULL != pLoadBuffer	);
	ASSERT( NULL != pLoadIndex	);
	if( NULL == node ) return;
	
	node->ID = ((UINT32*)pLoadBuffer)[(*pLoadIndex)++];	
	node->objectnum = ((UINT32*)pLoadBuffer)[(*pLoadIndex)++];	
	node->bHasChild = ((BOOL*)pLoadBuffer)[(*pLoadIndex)++];	

	node->BS.radius = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
	node->BS.center.x = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
	node->BS.center.y = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
	node->BS.center.z = ((float*)pLoadBuffer)[(*pLoadIndex)++];	
	node->hsize = ((UINT32*)pLoadBuffer)[(*pLoadIndex)++];	
	node->level = ((UINT32*)pLoadBuffer)[(*pLoadIndex)++];	

	if(node->bHasChild)
	{
		for(int i=0;i<8;++i)
		{
			if(!node->child[i])
			{
				node->child[i] = (OcTreeNode*)CreateModuleData(OCTREE_NODE_DATA);
				node->child[i]->parent = node;
				for(int j=0;j<8;++j)
				{
					node->child[i]->child[j] = NULL;
				}
			}

			// 혹시나 생길지 모르는 리컬시브콜 방지.
			if( node != node->child[i] )
				LoadNode(node->child[i],pLoadBuffer,pLoadIndex);
		}
	}
}

BOOL	ApmOcTree::SetCallBackSave(ApmOcTreeSaveCallBackFunc	pFunc)
{
	m_pfSaveCB = pFunc;
	return TRUE;
}

BOOL	ApmOcTree::SetCallBackLoad(ApmOcTreeLoadCallBackFunc	pFunc)
{
	m_pfLoadCB = pFunc;
	return TRUE;
}

BOOL	ApmOcTree::CB_LoadSector			( PVOID pData, PVOID pClass, PVOID pCustData )
{
	ApmOcTree*	pThis = (ApmOcTree*)pClass;
	ApWorldSector*	pSector = (ApWorldSector*)pData;

	pThis->LoadFromFiles(pSector->GetArrayIndexX(),pSector->GetArrayIndexZ());
	//pThis->LoadFromFiles(pSector->GetIndexX(),pSector->GetIndexZ());

	return TRUE;
}

BOOL	ApmOcTree::CB_ClearSector			( PVOID pData, PVOID pClass, PVOID pCustData )
{
	ApmOcTree*	pThis = (ApmOcTree*)pClass;
	ApWorldSector*	pSector = (ApWorldSector*)pData;

	if(!pThis->m_pCenterSector)
		return TRUE;

	INT32	ix = pSector->GetArrayIndexX();
	INT32	iz = pSector->GetArrayIndexZ();
	INT32	ix2 = pThis->m_pCenterSector->GetArrayIndexX();
	INT32	iz2 = pThis->m_pCenterSector->GetArrayIndexZ();
	INT32	dx,dz;

	if(ix > ix2) dx = ix - ix2;
	else dx = ix2 - ix;
	if(iz > iz2) dz = iz - iz2;
	else dz = iz2 - iz;

	if(dx > pThis->m_iLoadRange && dz > pThis->m_iLoadRange)	
	{
		if(pThis->m_pOcTreeRoots[ix][iz])
		{
			OcTreeRoot*		pRoot = pThis->m_pOcTreeRoots[ix][iz];
			pThis->m_pOcTreeRoots[ix][iz] = NULL;
			
			pThis->DestroyTree(pRoot);
		}

		pThis->m_csCSection.Lock();

		if(pThis->m_aeOcTreeRootStatus[ix][iz] == APMOCTREE_ROOT_STATUS_LOADING)
			pThis->m_aeOcTreeRootStatus[ix][iz] = APMOCTREE_ROOT_STATUS_REMOVED;
		else if (pThis->m_aeOcTreeRootStatus[ix][iz] == APMOCTREE_ROOT_STATUS_LOADED)
			pThis->m_aeOcTreeRootStatus[ix][iz] = APMOCTREE_ROOT_STATUS_EMPTY;

		pThis->m_csCSection.Unlock();
	}

	// 이거 없어서 죽는다. (Parn)
	if (pThis->m_pCenterSector == pSector)
		pThis->m_pCenterSector = NULL;

	return TRUE;
}


/*HANDLE		ApmOcTree::GetHandle(INT32 lx,INT32 lz)
{
	//AuAutoLock	lock(m_csCSection);
	HANDLE		res;

	BOOL			bFind = FALSE;
	OcTreeFOList*	cur_FO = m_listFO;
	OcTreeFOList*	bef_FO;
	while(cur_FO)
	{
		if(cur_FO->lx == lx && cur_FO->lz == lz)
		{
			res = cur_FO->fd;
			bFind = TRUE;
			break;
		}

		bef_FO = cur_FO;
		cur_FO = cur_FO->next;
	}

	if(bFind)
	{
		if(cur_FO != m_listFO)
		{
			bef_FO->next = cur_FO->next;

			cur_FO->next = m_listFO;
			m_listFO = cur_FO;
		}
		return res;
	}

	//없었으므로 만든다.
	if(m_iFOCount >= OCTREE_FO_NUM)
	{
		// 맨뒤 원소 삭제
		cur_FO = m_listFO;
		while(cur_FO && cur_FO->next)
		{
			bef_FO = cur_FO;
			cur_FO = cur_FO->next;
		}

		// netong 수정했음. cur_FO가 NULL 이면 무조건 죽는다. cur_FO가 NULL 이 아니면 cur_FO->next는 무조건 NULL 이다.
		// 아래처럼 처리하면 되고, 왼만하면 앞으론 NULL 체크좀 잘 하장
		bef_FO->next	= NULL;

		//bef_FO->next = cur_FO->next;

		if (cur_FO)
		{
			CloseHandle(cur_FO->fd);
			delete cur_FO;
		}
	}
	else 
	{
		++m_iFOCount;
	}

	OcTreeFOList*		nw_FO = new OcTreeFOList;
	nw_FO->lx = lx;
	nw_FO->lz = lz;

	INT32	x_arrayindex,z_arrayindex,division_index;
	//x_arrayindex = SectorIndexToArrayIndexX(lx);
	//z_arrayindex = SectorIndexToArrayIndexZ(lz);
	division_index = GetDivisionIndex( lx , lz );
		
	char		fstr[100];
	memset(fstr,'\0',100);
	sprintf(fstr,"world\\octree\\OT%d.dat",division_index);

	
	//char		fstr[100];
	//memset(fstr,'\0',100);
	//sprintf(fstr,"world\\octree\\OT%dx%d.dat",lx,lz);
	

	// INVALID_HANDLE이래도 저장 ..
	res = CreateFile(fstr,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
	nw_FO->fd = res;

	nw_FO->next = m_listFO;
	m_listFO = nw_FO;

	return res;
}

// 읽으면서 생성한다.
OcTreeRoot*	ApmOcTree::LoadFromFiles(INT32	six,INT32 siz)
{
	PROFILE("ApmOcTree::LoadFromFiles");

	INT32 lsix = six / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH; // MAP_DEFAULT_DEPTH 단위로 증가하기 위해
	INT32 lsiz = siz / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;

	if(six < 0 && six % MAP_DEFAULT_DEPTH != 0) lsix -= MAP_DEFAULT_DEPTH;
	if(siz < 0 && siz % MAP_DEFAULT_DEPTH != 0) lsiz -= MAP_DEFAULT_DEPTH;

	// 만약 다른 Thread에서 읽고 있다면, return;
	m_csCSection.Lock();

	if (m_aeOcTreeRootStatus[six][siz] == APMOCTREE_ROOT_STATUS_LOADING || m_aeOcTreeRootStatus[six][siz] == APMOCTREE_ROOT_STATUS_LOADED)
	{
		m_csCSection.Unlock();
		return NULL;
	}

	m_aeOcTreeRootStatus[six][siz] = APMOCTREE_ROOT_STATUS_LOADING;

	m_csCSection.Unlock();

	INT32 loffx = six - lsix;
	INT32 loffz = siz - lsiz;

	HANDLE		fd = GetHandle(lsix,lsiz);
	if(fd == INVALID_HANDLE_VALUE)
	{
		MD_SetErrorMessage( "파일을 읽는데 실패하였습니다.OcTree File \"%02d\"" , GetDivisionIndex( lsix , lsiz ) );
		return NULL;
	}

	DWORD	FP = 0;
	DWORD	foffset = 0;
	DWORD	foffset2 = 0;
	INT32	iLoadSize = 0;

	foffset = loffz * MAP_DEFAULT_DEPTH * 4  + loffx * 4;

	SetFilePointer(fd,foffset,NULL,FILE_BEGIN);
	ReadFile(fd,&foffset,sizeof(foffset),&FP,NULL);

	if(loffx+1 >= MAP_DEFAULT_DEPTH)
	{
		if(loffz+1 >= MAP_DEFAULT_DEPTH)
		{
			foffset2 = GetFileSize(fd,NULL);
		}
		else
		{
			foffset2 = (loffz + 1) * MAP_DEFAULT_DEPTH * 4;

			SetFilePointer(fd,foffset2,NULL,FILE_BEGIN);
			ReadFile(fd,&foffset2,sizeof(foffset2),&FP,NULL);
		}
	}
	else
	{
		foffset2 = loffz * MAP_DEFAULT_DEPTH * 4 + (loffx + 1) * 4;

		SetFilePointer(fd,foffset2,NULL,FILE_BEGIN);
		ReadFile(fd,&foffset2,sizeof(foffset2),&FP,NULL);
	}

	iLoadSize = foffset2 - foffset;

	DWORD*		loadBuffer = new DWORD[iLoadSize/4];
	INT32		iLoadIndex = 0;
	INT32	root_num = 0;
	DWORD	dwError;

	SetFilePointer(fd,foffset,NULL,FILE_BEGIN);
	BOOL	bRead = ReadFile(fd,loadBuffer,iLoadSize,&FP,NULL);
	if (!bRead)
	{
		dwError = GetLastError();
	}

	root_num = loadBuffer[iLoadIndex++];

	float	cy;
	OcTreeNode*		start_node;
	OcTreeRoot*		root = GetRoot(six, siz);

	for(int i=0;i<root_num;++i)
	{
		// center y load
		//ReadFile(fd,&cy ,sizeof(float),&FP,NULL);
		cy = ((float*)loadBuffer)[iLoadIndex++];
				
		start_node = CreateRoot(six,siz,cy, &root);

		LoadNode(start_node,loadBuffer,&iLoadIndex);
	}

	if(root)
	{
		if(m_pfLoadCB)
			m_pfLoadCB(loadBuffer,&iLoadIndex,root);		// agcmOcTree에서 지형 BBOX 로드위해

		m_csCSection.Lock();

		if (m_aeOcTreeRootStatus[six][siz] != APMOCTREE_ROOT_STATUS_LOADING)
		{
			DestroyTree(root);

			m_aeOcTreeRootStatus[six][siz] = APMOCTREE_ROOT_STATUS_EMPTY;

			m_csCSection.Unlock();

			delete []loadBuffer;

			return NULL;
		}

		m_aeOcTreeRootStatus[six][siz] = APMOCTREE_ROOT_STATUS_LOADED;

		SetRoot(six, siz, root);

		m_csCSection.Unlock();
	}
	else
	{
		m_aeOcTreeRootStatus[six][siz] = APMOCTREE_ROOT_STATUS_EMPTY;
	}

	//CloseHandle(fd);

	delete []loadBuffer;

	return root;
}
*/
