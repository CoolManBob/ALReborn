#include "AgcmGrass.h"

#include "AgcEngine.h"

#include "AcuSinTbl.h"
#include "ApModuleStream.h"

USING_ACUMATH;


#include "ApMemoryTracker.h"

extern AgcEngine*	g_pEngine;

void	GrassGroup::InitVertOrder()
{
	VertDrawOrder[0].x =BS.center.x - BS.radius;
	VertDrawOrder[0].y =MaxY + AGCM_GRASS_HEIGHT_OFFSET;
	VertDrawOrder[0].z =BS.center.z - BS.radius;

	VertDrawOrder[1].x =BS.center.x + BS.radius;
	VertDrawOrder[1].y =MaxY + AGCM_GRASS_HEIGHT_OFFSET;
	VertDrawOrder[1].z =BS.center.z - BS.radius;

	VertDrawOrder[2].x =BS.center.x + BS.radius;
	VertDrawOrder[2].y =MaxY + AGCM_GRASS_HEIGHT_OFFSET;
	VertDrawOrder[2].z =BS.center.z + BS.radius;

	VertDrawOrder[3].x =BS.center.x - BS.radius;
	VertDrawOrder[3].y =MaxY + AGCM_GRASS_HEIGHT_OFFSET;
	VertDrawOrder[3].z =BS.center.z + BS.radius;
}

BOOL	SectorGrassRoot::DoContainThis( RwV3d * pV3d )
{
	static ApmMap *pcsApmMap = NULL;
	if( NULL == pcsApmMap )
	{
		pcsApmMap = (ApmMap*) AGCMMAP_THIS->GetModule("ApmMap");
	}

	ASSERT( NULL != pV3d );

	if( pcsApmMap )
	{
		ApWorldSector * pSector = pcsApmMap->GetSectorByArrayIndex( six , siz );
		if( pSector )
		{
			if( pSector->GetXStart() <= pV3d->x && pV3d->x < pSector->GetXEnd() &&
				pSector->GetZStart() <= pV3d->z && pV3d->z < pSector->GetZEnd()	)
				return TRUE	;
			else
				return FALSE;
		}
		else
		{
			// 섹터가 로딩돼지 않았네.. 아에 에러인가
			return FALSE;
		}
	}
	else
	{
		// 여러모로 인생꼬였음
		return FALSE;
	}
}

AgcmGrass::AgcmGrass()
{
	SetModuleName("AgcmGrass");

	EnableIdle(TRUE);

	m_pcsAgcmRender	= NULL;
	m_pcsApmMap		= NULL;
	m_pcsAgcmOcTree	= NULL;
	m_pcsApmOcTree	= NULL;
	m_pcsAgcmMap	= NULL;
	m_pcsAgcmEventNature = NULL;
	m_pcsAgcmShadow2= NULL;

	m_pCurDevice	= NULL;
	m_bDrawGrass	= TRUE;
	m_eDetail		= GRASS_DETAIL_MEDIUM;

	m_iDrawGrassNum			= 0;
	m_iDrawGrassCountNear	= 0;
	m_iDrawGrassCount		= 0;

	m_pViewMatrixAt = NULL;

	m_uiLastTick	= 0;
	m_uiCurTickDiff	= 0;

	m_iGrassInfoNum = 0;

	m_iTextureNum = 0;
	m_listGrassRoot = NULL;

	m_pJobQueue			= NULL;
	m_iJobQueueCount	= 0;
	m_iJobQueueCount2	= 0;
	m_iJobQueueMaxCount	= 0;

	strcpy(m_szDirectory,"world\\grass");

	m_csCSection.Init();

	m_iDrawCameraIndex	= 3;
	m_iSectorRange		= 2;

	Grass::SetEnable(FALSE);
}

AgcmGrass::~AgcmGrass()
{

}

BOOL	AgcmGrass::OnAddModule()
{
	m_pcsAgcmRender			= (AgcmRender*) GetModule("AgcmRender");
	m_pcsApmMap				= (ApmMap*) GetModule("ApmMap");
	m_pcsAgcmOcTree			= (AgcmOcTree*) GetModule("AgcmOcTree");
	m_pcsApmOcTree			= (ApmOcTree*) GetModule("ApmOcTree");
	m_pcsAgcmMap			= (AgcmMap*) GetModule("AgcmMap");
	m_pcsAgcmEventNature	= (AgcmEventNature*) GetModule("AgcmEventNature");
	m_pcsAgcmShadow2		= (AgcmShadow2*)GetModule("AgcmShadow2");
	
	m_pcsAgcmRender->SetCallbackOctreeIDSet(CB_OCTREE_ID_SET,this);
	m_pcsApmMap->SetCallbackGetMinHeight(CB_GET_MIN_HEIGHT,this);


	if(m_pcsAgcmMap)
	{
		m_pcsAgcmMap->SetCallbackLoadMap(CB_LOAD_MAP,this);
		m_pcsAgcmMap->SetCallbackUnLoadMap(CB_UNLOAD_MAP,this);
	}

	if(m_pcsAgcmEventNature)
		m_pcsAgcmEventNature->SetCallbackLightUpdate(CB_LIGHT_VALUE_UPDATE,this);

	if(m_pcsAgcmShadow2)
		m_pcsAgcmShadow2->SetCallbackTextureUpdate(CB_SHADOW_UPDATE,this);

	return TRUE;
}

BOOL	AgcmGrass::OnInit()
{
	m_pCurDevice = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice ();

	SetGrassDetail(GRASS_DETAIL_MEDIUM);

	m_iJobQueueMaxCount = 1000;		
	m_pJobQueue = new GrassJobQueue[m_iJobQueueMaxCount];
	
	return TRUE;
}

void	AgcmGrass::RemoveAll()
{
	SectorGrassRoot*	cur_root = m_listGrassRoot;
	SectorGrassRoot*	remove_root;

	GrassGroup			*cur_group,*remove_group;

	while(cur_root)
	{
		cur_root->pLockMutex.Lock();

		cur_group = cur_root->listGrassGroup;
		while(cur_group)
		{
			remove_group = cur_group;
			cur_group = cur_group->next;

			GRASSLISTITER	Iter  = remove_group->ListGrass.begin();
			for( Iter ; Iter != remove_group->ListGrass.end() ; ++Iter )
			{
				DEF_SAFEDELETE( (*Iter) );
			}

			remove_group->ListGrass.clear();
			delete remove_group;
		}

		cur_root->pLockMutex.Unlock();

		remove_root = cur_root;
		cur_root = cur_root->next;

		delete remove_root;
	}

	m_listGrassRoot = NULL;

	int nGrassTextureCount = m_vecGrassTexture.GetSize();
	for( int nCount = 0 ; nCount < nGrassTextureCount ; nCount++ )
	{
		stGrassTextureEntry* pEntry = m_vecGrassTexture.Get( nCount );
		if( pEntry && pEntry->m_pTexture )
		{
			RwTextureDestroy( pEntry->m_pTexture );
			pEntry->m_pTexture = NULL;
		}
	}

	m_vecGrassTexture.Clear();

	m_csCSection.Lock();

	m_iJobQueueCount = 0;
	
	m_csCSection.Unlock();
}

BOOL	AgcmGrass::OnDestroy()
{
	INT32	i;

	for(i=0;i<static_cast<int>( m_astGrassInfo.size() ) ;++i)
	{
		if(m_astGrassInfo[i].pOriginalVB)
			delete[] m_astGrassInfo[i].pOriginalVB;

		if(m_astGrassInfo[i].pOriginalDarkVB)
			delete[] m_astGrassInfo[i].pOriginalDarkVB;
	}

	//JobQueue처리
	for(i=0;i<m_iJobQueueCount;++i)
	{
		//new한 객체 release
		if(m_pJobQueue[i].type == GRASS_JOB_ADD_ROOT)
		{
			SectorGrassRoot*	pRoot = (SectorGrassRoot*) m_pJobQueue[i].data1;
			DeleteAll(pRoot);
			delete pRoot;
		}
	}
	delete			[]m_pJobQueue;

	RemoveAll();

	return TRUE;
}

BOOL	AgcmGrass::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmGrass::OnIdle");

	m_uiCurTickDiff = ulClockCount - m_uiLastTick;
	m_uiLastTick = ulClockCount;

	m_pViewMatrixAt = m_pcsAgcmRender->m_pViewMatrixAt;

	m_iDrawGrassCount = 0;
	m_iDrawGrassCountNear = 0;

	// Job Queue 처리 !!
	m_csCSection.Lock();
	SectorGrassRoot*		pRoot;
	GrassGroup*				cur_group;
	SectorGrassRoot			*cur_root,*before_root;
	int	i;
	
	for(i=0;i<m_iJobQueueCount;++i)
	{
		if(m_pJobQueue[i].type == GRASS_JOB_REMOVED)
			continue;

		pRoot = (SectorGrassRoot*) m_pJobQueue[i].data1;

		if(m_pJobQueue[i].type == GRASS_JOB_ADD_ROOT)
		{
			pRoot->next = m_listGrassRoot;
			m_listGrassRoot = pRoot;

			m_pJobQueue[i].type = GRASS_JOB_REMOVED;
		}
		else if(m_pJobQueue[i].type == GRASS_JOB_ADD_RENDER)
		{
			if( pRoot->bAddRender == 1 ) continue;	// 이미 add되었다.

			OcCustomDataList	stSetParam;
			stSetParam.pData2 = NULL;
			stSetParam.iAppearanceDistance = m_iSectorRange;
			stSetParam.pClass = this;
			stSetParam.pRenderCB = CB_GRASS_RENDER;
			stSetParam.pUpdateCB = CB_GRASS_UPDATE;

			//@{ 2006/11/17 burumal
			stSetParam.pDistCorrectCB = CB_GRASS_DISTCORRECT;
			//@}

			cur_group = pRoot->listGrassGroup;
			while(cur_group)
			{
				stSetParam.pData1 = cur_group;
				stSetParam.BS.center = cur_group->BS.center;
				stSetParam.BS.radius = cur_group->BS.radius;
				stSetParam.piCameraZIndex = &cur_group->iCameraZIndex; 

				stSetParam.TopVerts[0].x = cur_group->VertDrawOrder[0].x;
				stSetParam.TopVerts[0].y = cur_group->VertDrawOrder[0].y;
				stSetParam.TopVerts[0].z = cur_group->VertDrawOrder[0].z;

				stSetParam.TopVerts[1].x = cur_group->VertDrawOrder[1].x;
				stSetParam.TopVerts[1].y = cur_group->VertDrawOrder[1].y;
				stSetParam.TopVerts[1].z = cur_group->VertDrawOrder[1].z;

				stSetParam.TopVerts[2].x = cur_group->VertDrawOrder[2].x;
				stSetParam.TopVerts[2].y = cur_group->VertDrawOrder[2].y;
				stSetParam.TopVerts[2].z = cur_group->VertDrawOrder[2].z;

				stSetParam.TopVerts[3].x = cur_group->VertDrawOrder[3].x;
				stSetParam.TopVerts[3].y = cur_group->VertDrawOrder[3].y;
				stSetParam.TopVerts[3].z = cur_group->VertDrawOrder[3].z;

				if(m_pcsApmOcTree->m_bOcTreeEnable)
				{
					m_pcsAgcmOcTree->AddCustomRenderDataToOcTree(&cur_group->stOctreeID,&stSetParam);
				}
				else
				{
					m_pcsAgcmRender->AddCustomRenderToSector(this,&cur_group->BS,CB_GRASS_UPDATE,CB_GRASS_RENDER,
															(PVOID)cur_group,NULL);
				}

				cur_group = cur_group->next;
			}

			pRoot->bAddRender = 1;

			m_pJobQueue[i].type = GRASS_JOB_REMOVED;
		}
		else if(m_pJobQueue[i].type == GRASS_JOB_CLEAR_ROOT)
		{
			cur_root = m_listGrassRoot;
			while(cur_root)
			{
				DeleteAll(cur_root);
				cur_root = cur_root->next;
			}

			for(++i;i<m_iJobQueueCount;++i)
			{
				if(m_pJobQueue[i].type == GRASS_JOB_ADD_ROOT)
				{
					pRoot = (SectorGrassRoot*) m_pJobQueue[i].data1;
					DeleteAll(pRoot);

					pRoot->next = m_listGrassRoot;
					m_listGrassRoot = pRoot;
				}
				
				m_pJobQueue[i].type = GRASS_JOB_REMOVED;
			}

			Grass::SetEnable(FALSE);

			m_pJobQueue[i].type = GRASS_JOB_REMOVED;

			break;
		}
		else if(m_pJobQueue[i].type == GRASS_JOB_REMOVE_ROOT)
		{
			// 뒤로 미룸..
			m_pJobQueue[m_iJobQueueCount2++].data1 = m_pJobQueue[i].data1;
		}
	}

	for(i=0;i<m_iJobQueueCount2;++i)
	{
		pRoot = (SectorGrassRoot*) m_pJobQueue[i].data1;
		DeleteAll(pRoot);

		cur_root = m_listGrassRoot;
		while(cur_root)
		{
			if(cur_root == pRoot)
			{
				if(cur_root == m_listGrassRoot)
				{
					m_listGrassRoot = cur_root->next;
				}
				else
				{
					before_root->next = cur_root->next;
				}
				
				delete cur_root;
				break;
			}

			before_root = cur_root;
			cur_root = cur_root->next;
		}
	}

	m_iJobQueueCount2 = 0;
	m_iJobQueueCount = 0;
	m_csCSection.Unlock();

//	m_listDraw = NULL;

	return TRUE;
}

BOOL	AgcmGrass::LoadGrassInfoFromINI(char*	szFile, BOOL bDecryption)
{
	if (!szFile)
		return FALSE;

	ApModuleStream	csStream;
	const CHAR		*szValueName = NULL;
	CHAR			szValue[256];

	CHAR			szFileName[256];
	INT32			index;
		
	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	RwTextureSetAutoMipmapping(TRUE);
	RwTextureSetMipmapping(TRUE);

	// szFile을 읽는다.
	VERIFY(csStream.Open(szFile, 0 , bDecryption) && "Grass INI파일을 읽지 못했습니다");

	{
		csStream.ReadSectionName(0);
		csStream.SetValueID(-1);

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();

			if(!strcmp(szValueName, AGCMGRASS_INI_NAME_TEXTURE_NAME))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue,"%d:%s",&index,szFileName);

				stGrassTextureEntry NewEntry;
				NewEntry.m_pTexture = RwTextureRead( szFileName, NULL );
				NewEntry.m_nIndex = index;

				m_vecGrassTexture.Add( NewEntry );
				++m_iTextureNum;
			}
		}
	}

	{
		csStream.ReadSectionName(1);
		csStream.SetValueID(-1);
		INT32			lID;

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();

			if(!strcmp(szValueName, AGCMGRASS_INI_NAME_GRASS_INFO_ID))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &lID);

				if(lID > m_iGrassInfoNum) 
					m_iGrassInfoNum = lID;


				if( static_cast< int >( m_astGrassInfo.size() ) <= lID )
				{
					m_astGrassInfo.resize( lID + 1 );
				}
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_GRASS_NAME))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%s", &m_astGrassInfo[lID].Name ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_TEXTURE_ID))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iTextureID ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_SHAPE_TYPE))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iShapeType ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_GRASS_WIDTH))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iWidth ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_GRASS_HEIGHT))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iHeight ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_GRASS_WIDTH_RANDOM))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iWidth_random ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_GRASS_HEIGHT_RANDOM))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iHeight_random ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_ANIMATION_TYPE))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iAnimationType ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_ANIMATION_AMOUNT))
			{
				int		iTemp;
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &iTemp);

				m_astGrassInfo[lID].fAnimAmount = (FLOAT)iTemp;
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_ANIMATION_SPEED))
			{
				int		iTemp;
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &iTemp);
					
				m_astGrassInfo[lID].fAnimSpeed = (FLOAT)iTemp / 40.0f;
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_IMAGE_START_X))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iImageX ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_IMAGE_START_Y))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iImageY ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_IMAGE_WIDTH))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iImageW ); 
			}
			else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_IMAGE_HEIGHT))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_astGrassInfo[lID].iImageH ); 

				// 풀 폴리곤 생성
				m_astGrassInfo[lID].numTris = MakeGrassVerts(&m_astGrassInfo[lID]);
			}
		}
	}

	++m_iGrassInfoNum;

	return TRUE;
}

BOOL	AgcmGrass::SaveGrassInfoToINI(char*		szFileName, BOOL bEncryption )
{
	CHAR szValue[255];

	ApModuleStream csStream;
	if(!csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE))
		return FALSE;
	
	if(!csStream.SetSection("Textures"))
		return FALSE;

	int i;
	for(i=0;i<m_iTextureNum;++i)
	{
		stGrassTextureEntry* pEntry = m_vecGrassTexture.Get( i );
		if( pEntry && pEntry->m_pTexture )
		{
			sprintf(szValue, "%d:%s", i, RwTextureGetName(pEntry->m_pTexture));
			if(!csStream.WriteValue(AGCMGRASS_INI_NAME_TEXTURE_NAME, szValue))
				return FALSE;
		}
	}

	if(!csStream.SetSection("GRASSINFO"))
		return FALSE;

	for(i=0;i<m_iGrassInfoNum;++i)
	{
		sprintf(szValue, "\n%d", i);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_GRASS_INFO_ID, szValue))
		return FALSE;

		sprintf(szValue, "%s", m_astGrassInfo[i].Name);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_GRASS_NAME, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_astGrassInfo[i].iTextureID);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_TEXTURE_ID, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_astGrassInfo[i].iShapeType);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_SHAPE_TYPE, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_astGrassInfo[i].iWidth);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_GRASS_WIDTH, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_astGrassInfo[i].iHeight);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_GRASS_HEIGHT, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_astGrassInfo[i].iWidth_random);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_GRASS_WIDTH_RANDOM, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_astGrassInfo[i].iHeight_random);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_GRASS_HEIGHT_RANDOM, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_astGrassInfo[i].iAnimationType);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_ANIMATION_TYPE, szValue))
		return FALSE;

		int		iTemp;
		iTemp = (int)m_astGrassInfo[i].fAnimAmount;
		sprintf(szValue, "%d", iTemp);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_ANIMATION_AMOUNT, szValue))
		return FALSE;

		iTemp = (int)(m_astGrassInfo[i].fAnimSpeed * 40.0f);
		sprintf(szValue, "%d", iTemp );
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_ANIMATION_SPEED, szValue))
		return FALSE;

		print_compact_format(szValue, "%f", m_astGrassInfo[i].iImageX);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_IMAGE_START_X, szValue))
		return FALSE;
		
		sprintf(szValue, "%d", m_astGrassInfo[i].iImageY);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_IMAGE_START_Y, szValue))
		return FALSE;

		print_compact_format(szValue, "%f", m_astGrassInfo[i].iImageW);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_IMAGE_WIDTH, szValue))
		return FALSE;

		print_compact_format(szValue, "%f", m_astGrassInfo[i].iImageH);
		if(!csStream.WriteValue(AGCMGRASS_INI_NAME_IMAGE_HEIGHT, szValue))
		return FALSE;
	}
	
	return csStream.Write(szFileName, 0 , bEncryption);
}

INT32	AgcmGrass::MakeGrassVerts(GrassInfo*	pGrass)
{
	if( !pGrass ) return 0;

	stGrassTextureEntry* pEntry = m_vecGrassTexture.Get( pGrass->iTextureID );
	if( !pEntry || !pEntry->m_pTexture ) return 0;

	RwTexture*		pTex = pEntry->m_pTexture;
	if(!pTex) return 0;

	RwRaster*		pRas = RwTextureGetRaster(pTex);

	INT32			tw = RwRasterGetWidth(pRas);
	INT32			th = RwRasterGetHeight(pRas);

	FLOAT			u1 = (FLOAT)pGrass->iImageX / (FLOAT)tw;
	FLOAT			u2 = (FLOAT)(pGrass->iImageX + pGrass->iImageW) / (FLOAT)tw;
	FLOAT			v1 = (FLOAT)pGrass->iImageY / (FLOAT)th;
	FLOAT			v2 = (FLOAT)(pGrass->iImageY + pGrass->iImageH) / (FLOAT)th;

	FLOAT			Gw,Gh,Hw,Shear;

	if(pGrass->iShapeType == GRASS_SHAPE_CROSS)
	{
		GRASS_VERTEX			verts[12];

		Gw = static_cast< FLOAT > ( pGrass->iWidth + (rand() % (pGrass->iWidth_random * 2)) - pGrass->iWidth_random );
		Gh = static_cast< FLOAT > ( pGrass->iHeight + (rand() % (pGrass->iHeight_random * 2)) - pGrass->iHeight_random );

		Shear = Gw * 0.05f;
		Hw = Gw * 0.5f;

		verts[0].x = Hw ; verts[0].y = 0.0f ; verts[0].z = -Shear ;
		verts[1].x = -Hw ; verts[1].y = 0.0f ; verts[1].z = 0.0f ;
		verts[2].x = Hw ; verts[2].y = Gh ; verts[2].z = 0.0f ;
		verts[3].x = Hw ; verts[3].y = Gh ; verts[3].z = 0.0f ;
		verts[4].x = -Hw ; verts[4].y = 0.0f; verts[4].z = 0.0f;
		verts[5].x = -Hw ; verts[5].y = Gh; verts[5].z = Shear;

		verts[0].u = u2;		verts[0].v = v2;
		verts[1].u = u1;		verts[1].v = v2;
		verts[2].u = u2;		verts[2].v = v1;
		verts[3].u = u2;		verts[3].v = v1;
		verts[4].u = u1;		verts[4].v = v2;
		verts[5].u = u1;		verts[5].v = v1;
				
		Gw = static_cast< FLOAT > ( pGrass->iWidth + (rand() % (pGrass->iWidth_random * 2)) - pGrass->iWidth_random );
		Gh = static_cast< FLOAT > ( pGrass->iHeight + (rand() % (pGrass->iHeight_random * 2)) - pGrass->iHeight_random );

		Shear = Gw * 0.05f;
		Hw = Gw * 0.5f;

		verts[6].x	= -Shear;	verts[6].y	= 0.0f;		verts[6].z	= Hw;
		verts[7].x	= 0.0f;		verts[7].y	= 0.0f;		verts[7].z	= -Hw;
		verts[8].x	= 0.0f;		verts[8].y	= Gh;		verts[8].z	= Hw;
		verts[9].x	= 0.0f;		verts[9].y	= Gh;		verts[9].z	= Hw;
		verts[10].x = 0.0f;		verts[10].y = 0.0f;		verts[10].z = -Hw;
		verts[11].x = Shear;	verts[11].y = Gh;		verts[11].z = -Hw;
		
		verts[6].u = u2;		verts[6].v = v2;
		verts[7].u = u1;		verts[7].v = v2;
		verts[8].u = u2;		verts[8].v = v1;
		verts[9].u = u2;		verts[9].v = v1;
		verts[10].u = u1;		verts[10].v = v2;
		verts[11].u = u1;		verts[11].v = v1;
		
		for(int i=0;i<12;++i)
		{
			verts[i].color = 0xffffffff;
			verts[i].spec = 0xffffffff;
		}

		//. Direct3D9 Debug mode에 의거하여 MANEGED로 생성 static vertex buffer를 runtime에 lock/unlock하는 것은
		//. 프레임저하를 유발하므로 시스템메모리를 사용하여 DrawPrimitiveUP로 render한다.
		pGrass->pOriginalVB = new GRASS_VERTEX[12];
		pGrass->pOriginalDarkVB = new GRASS_VERTEX[12];

		memcpy( pGrass->pOriginalVB, verts, sizeof(verts) );
		memcpy( pGrass->pOriginalDarkVB, verts, sizeof(verts) );

		return 4;
	}
	else if(pGrass->iShapeType == GRASS_SHAPE_PERPENDICULAR)
	{
		GRASS_VERTEX			verts[12];
		
		Gw = static_cast< FLOAT > ( pGrass->iWidth + (rand() % (pGrass->iWidth_random * 2)) - pGrass->iWidth_random );
		Gh = static_cast< FLOAT > ( pGrass->iHeight + (rand() % (pGrass->iHeight_random * 2)) - pGrass->iHeight_random );

		Shear = Gw * 0.2f;
		Hw = Gw * 0.5f;

		FLOAT		offset = Gw * 0.05f;
		FLOAT		offset2 = Gw * 0.15f;

		verts[0].x = Hw ; verts[0].y = 0.0f ; verts[0].z = -Shear ;
		verts[1].x = -Hw ; verts[1].y = 0.0f ; verts[1].z = 0.0f ;
		verts[2].x = Hw ; verts[2].y = Gh ; verts[2].z = 0.0f ;
		verts[3].x = Hw ; verts[3].y = Gh ; verts[3].z = 0.0f ;
		verts[4].x = -Hw ; verts[4].y = 0.0f; verts[4].z = 0.0f;
		verts[5].x = -Hw ; verts[5].y = Gh; verts[5].z = Shear;

		verts[0].u = u2;		verts[0].v = v2;
		verts[1].u = u1;		verts[1].v = v2;
		verts[2].u = u2;		verts[2].v = v1;
		verts[3].u = u2;		verts[3].v = v1;
		verts[4].u = u1;		verts[4].v = v2;
		verts[5].u = u1;		verts[5].v = v1;
		
		// 작은 부분(ㄱ 자 모양 풀의..)
		Gw = (pGrass->iWidth + (rand() % (pGrass->iWidth_random * 2)) - pGrass->iWidth_random ) * 0.9f;
		Gh = (pGrass->iHeight + (rand() % (pGrass->iHeight_random * 2)) - pGrass->iHeight_random) * 0.8f;

		Shear = Gw * 0.05f;
		Hw = Gw * 0.5f;

		verts[6].x = Shear + offset; verts[6].y = 0.0f ; verts[6].z = Hw + offset2;
		verts[7].x = 0.0f + offset; verts[7].y = 0.0f ; verts[7].z = -Hw + offset2;
		verts[8].x = 0.0f + offset; verts[8].y = Gh ; verts[8].z = Hw + offset2;
		verts[9].x = 0.0f + offset; verts[9].y = Gh ; verts[9].z = Hw + offset2;
		verts[10].x = 0.0f + offset; verts[10].y = 0.0f; verts[10].z = -Hw + offset2;
		verts[11].x = -Shear + offset; verts[11].y = Gh; verts[11].z = -Hw + offset2;
		
		verts[6].u = u2;		verts[6].v = v2;
		verts[7].u = u1;		verts[7].v = v2;
		verts[8].u = u2;		verts[8].v = v1;
		verts[9].u = u2;		verts[9].v = v1;
		verts[10].u = u1;		verts[10].v = v2;
		verts[11].u = u1;		verts[11].v = v1;

		for(int i=0;i<12;++i)
		{
			verts[i].color = 0xffffffff;
			verts[i].spec = 0xffffffff;
		}
		
		pGrass->pOriginalVB		= new GRASS_VERTEX[12];
		pGrass->pOriginalDarkVB = new GRASS_VERTEX[12];

		memcpy( pGrass->pOriginalVB, verts, sizeof(verts) );
		memcpy( pGrass->pOriginalDarkVB, verts, sizeof(verts) );

		return 4;
	}

	return 0;
}

// 풀은 라이팅 처리가 안먹으므로 버텍스 칼라를 바꿔준다.
BOOL	AgcmGrass::CB_LIGHT_VALUE_UPDATE ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmGrass*	pThis = (AgcmGrass*) pClass;
	GrassInfo*	pInfo;

	if ( pData == NULL || pCustData == NULL )
		return FALSE;

	RwRGBAReal*		direct = (RwRGBAReal*)pData;
	RwRGBAReal*		ambient = (RwRGBAReal*)pCustData;
	
	FLOAT		fr = direct->red * 0.5f + ambient->red;
	FLOAT		fg = direct->green * 0.5f + ambient->green;
	FLOAT		fb = direct->blue * 0.5f + ambient->blue;

	if(fr > 1.0f) fr = 1.0f;
	if(fg > 1.0f) fg = 1.0f;
	if(fb > 1.0f) fb = 1.0f;

	UINT8		gr = (UINT8)(255.0f * fr);
	UINT8		gg = (UINT8)(255.0f * fg);
	UINT8		gb = (UINT8)(255.0f * fb);

	DWORD		final_spec = 0xff000000 | (gr << 16) | (gg << 8) | gb ;

	UINT8		hr = (UINT8)(255.0f * (fr * 0.7f));
	UINT8		hg = (UINT8)(255.0f * (fg * 0.7f));
	UINT8		hb = (UINT8)(255.0f * (fb * 0.7f));

	DWORD		final_half_spec = 0xff000000 | (hr << 16) | (hg << 8) | hb ;
	INT32		num_vert,j;

	for(vector< GrassInfo >::iterator iter = pThis->m_astGrassInfo.begin()	;
		iter != pThis->m_astGrassInfo.end()									;
		iter++																)
	{
		pInfo = &( *iter );

		if( NULL == pInfo->pOriginalVB		||
			NULL == pInfo->pOriginalDarkVB	) continue;


		num_vert = pInfo->numTris * 3;

		//. runtime에 더이상 vertex buffer를 lock/unlock하지 않는다.
		for(j=0;j<num_vert;++j)
		{
			pInfo->pOriginalVB[j].color = 0xff000000;//final_diffuse;
			pInfo->pOriginalVB[j].spec = final_spec;
			
			pInfo->pOriginalDarkVB[j].color = 0xff000000;//final_half_diffuse;
			pInfo->pOriginalDarkVB[j].spec = final_half_spec;
		}
	}

	return TRUE;
}

BOOL	AgcmGrass::InitGrass(Grass*		pGrass)
{
	if( m_astGrassInfo.size() <= pGrass->iGrassID )
	{
		char	str[ 256 ];
		sprintf( str , "풀 ID 범위 초과?..(%d까지 인데 %d임)" , m_astGrassInfo.size() - 1 , pGrass->iGrassID );
		MD_SetErrorMessage( str );

		return FALSE;
	}
	if( pGrass->fRotX > 360 )
	{
		MD_SetErrorMessage( "각도 범위 초과X" );
		return FALSE;
	}

	if( pGrass->fRotY > 360 )
	{
		MD_SetErrorMessage( "각도 범위 초과Y" );
		return FALSE;
	}

	#ifdef _DEBUG
	try
	#endif
	{
		//. 2006. 3. 4. Nonstopdj
		//. LPDIRECT3DVERTEXBUFFER9를 사용하지 않고 힙에 할당된 buffer를 사용.
		//pGrass->pvVB = m_astGrassInfo[pGrass->iGrassID].pvOriginalVB;
		pGrass->pVB = m_astGrassInfo[pGrass->iGrassID].pOriginalVB;

		stGrassTextureEntry* pEntry = m_vecGrassTexture.Get( m_astGrassInfo[ pGrass->iGrassID ].iTextureID );
		pGrass->pTexture = pEntry ? pEntry->m_pTexture : NULL;
		pGrass->iVertexOffset = m_astGrassInfo[pGrass->iGrassID].iVertexOffset;
			
		pGrass->fAnimParam1 = (FLOAT)(rand() % 360);
		pGrass->fAnimParam2 = 0.0f;
		pGrass->iShadow = 0;
			
		RwV3d	ScaleFactor;
		ScaleFactor.x = ((FLOAT)(rand() % 20) / 100.0f + 0.9f) * pGrass->fScale;
		ScaleFactor.y = ((FLOAT)(rand() % 40) / 100.0f + 0.8f) * pGrass->fScale;
		ScaleFactor.z = ((FLOAT)(rand() % 20) / 100.0f + 0.9f) * pGrass->fScale;

		RwV3d	XAxis = {1.0f,0.0f,0.0f};
		RwV3d	YAxis = {0.0f,1.0f,0.0f};

		D3DXMATRIX		temp1,temp2,temp3;
		D3DXMatrixIdentity(&pGrass->matTransform);
		D3DXMatrixScaling(&temp1,ScaleFactor.x,ScaleFactor.y,ScaleFactor.z);
		D3DXMatrixRotationX(&temp2,pGrass->fRotX * AU_DEG2RAD);
		D3DXMatrixRotationY(&temp3,pGrass->fRotY * AU_DEG2RAD);

		D3DXMatrixMultiply(&pGrass->matTransform,&pGrass->matTransform,&temp1);
		D3DXMatrixMultiply(&pGrass->matTransform,&pGrass->matTransform,&temp2);
		D3DXMatrixMultiply(&pGrass->matTransform,&pGrass->matTransform,&temp3);
		return TRUE;
	}
	#ifdef _DEBUG
	catch( ... )
	{
		MD_SetErrorMessage( "메모리 오류" );
		return FALSE;
	}
	#endif
}

void	AgcmGrass::GrassMatrixSet(D3DXMATRIX*	pMat,Grass*	pGrass,INT32	iCamIndex)
{
	(*pMat) = pGrass->matTransform;

	if(iCamIndex < 1)
	{
		D3DXMATRIX		matRot,matTrans;

		D3DXMatrixRotationX(&matRot,pGrass->fAnimParam2 * AU_DEG2RAD);
		D3DXMatrixTranslation(&matTrans,pGrass->vPos.x,pGrass->vPos.y,pGrass->vPos.z);

		D3DXMatrixMultiply(pMat,pMat,&matRot);
		D3DXMatrixMultiply(pMat,pMat,&matTrans);
	}
	else if(iCamIndex < m_iDrawCameraIndex)
	{
		D3DXMATRIX		matTrans;
		D3DXMatrixTranslation(&matTrans,pGrass->vPos.x,pGrass->vPos.y,pGrass->vPos.z);
		D3DXMatrixMultiply(pMat,pMat,&matTrans);
	}
}

// 애니메이션 업데이트
BOOL	AgcmGrass::CB_GRASS_UPDATE(PVOID pData,PVOID	pClass,PVOID	pCustData)
{
	PROFILE("AgcmGrass::GrassUpdate");
	AgcmGrass*	pThis = (AgcmGrass*)pClass;

	if(!pThis->m_bDrawGrass)	
		return TRUE;

	GrassGroup*		pGrassGroup = (GrassGroup*)pData;
	GRASSLISTITER	Iter		=	pGrassGroup->ListGrass.begin();

	D3DXMATRIX		matRot,matTrans;

	if(pGrassGroup->iCameraZIndex < 1)
	{
		RwV3d	XAxis = {1.0f,0.0f,0.0f};

		for( ; Iter != pGrassGroup->ListGrass.end() ; ++Iter )
		{
			Grass*		pGrass		=	(*Iter);

			pGrass->fAnimParam1 += pThis->m_astGrassInfo[pGrass->iGrassID].fAnimSpeed * (float)pThis->m_uiCurTickDiff;

			if( pGrass->fAnimParam1 >= 360.0f )
				pGrass->fAnimParam1	=	0.0f;

			pGrass->fAnimParam2 =	(AcuSinTbl::Sin(pGrass->fAnimParam1)) * pThis->m_astGrassInfo[pGrass->iGrassID].fAnimAmount;
		}

		pThis->m_iDrawGrassCountNear += pGrassGroup->iGrassNum;
	}
	else if(pGrassGroup->iCameraZIndex < pThis->m_iDrawCameraIndex)
	{
		pThis->m_iDrawGrassCount += pGrassGroup->iGrassNum;
	}

	return TRUE;
}

BOOL	AgcmGrass::CB_GRASS_RENDER(PVOID pData,PVOID	pClass,PVOID	pCustData)
{
	PROFILE("AgcmGrass::RenderGrass");
	AgcmGrass*	pThis = (AgcmGrass*)pClass;

	if(!pThis->m_bDrawGrass)	
		return TRUE;		

	GrassGroup*		pGrassGroup = (GrassGroup*)pData;

	// disable the programmable pipeline.
	RwD3D9SetVertexShader(NULL);
	RwD3D9SetPixelShader(NULL);
	RwD3D9SetFVF( D3DFVF_GRASS_VERTEX );

	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
	//RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1	,	D3DTA_TEXTURE		);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP	,   D3DTOP_SELECTARG1	);
	RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1	,	D3DTA_TEXTURE		);
	
	// Enable lighting 
    RwD3D9SetRenderState(D3DRS_LIGHTING, TRUE);
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	RwD3D9SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	RwD3D9SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	RwD3D9SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);

	// prelight false
	RwD3D9SetRenderState(D3DRS_COLORVERTEX, TRUE);
	RwD3D9SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);	
	RwD3D9SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR2);	
	RwD3D9SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);	

	D3DXMATRIX		matFinal;
	INT32			numTris = 4;		// 이건 그냥 일단 쓰자.. 다 4개짜리이다. 현재.
	
	if(pGrassGroup->iCameraZIndex < pThis->m_iDrawCameraIndex)
	{
		if(pThis->m_iDrawGrassCount > pThis->m_iDrawGrassNum)
		{
			pThis->m_iDrawGrassCount -= pGrassGroup->iGrassNum;
		}

		if(pGrassGroup->iGrassNum == 1)
		{
			GRASSLISTITER	Iter	=	pGrassGroup->ListGrass.begin();

			for( ; Iter != pGrassGroup->ListGrass.end() ; ++Iter )
			{
				Grass* pGrass	=	(*Iter);

				if( pGrass->pTexture )
				{
					RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *) RwTextureGetRaster(pGrass->pTexture));
					pThis->GrassMatrixSet(&matFinal, pGrass ,pGrassGroup->iCameraZIndex);
					RwD3D9SetTransform( D3DTS_WORLD, &matFinal );
					RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLELIST, numTris, pGrass->pVB, sizeof(GRASS_VERTEX));
				}
			}
		}
		else
		{
			INT32		before_ID		= -1;
			INT32		before_iShadow	= -1;
			RwV3d*		pViewAt			= pThis->m_pViewMatrixAt;
			RwTexture*	before_Tex		= NULL;
			RwV3d		ScreenPos;


			mmapDrawGrass	SortDrawGrass;

			GRASSLISTITER	GrassIter	=	pGrassGroup->ListGrass.begin();
			for( ; GrassIter != pGrassGroup->ListGrass.end() ; ++GrassIter )
			{
				Grass*	pGrass		=	(*GrassIter);

				pThis->m_pcsAgcmRender->GetWorldPosToScreenPos( &pGrass->vPos , &ScreenPos );
				SortDrawGrass.insert( make_pair( ScreenPos.z , pGrass ) );
			}

			mmapDrawGrassIter	Iter	=	SortDrawGrass.begin();

			for( ; Iter != SortDrawGrass.end()  ; ++Iter )
			{
				Grass*	pGrass	=	Iter->second;
				if( !pGrass )
					continue;

				if(pGrass->pTexture)
				{
					if(pGrass->pTexture != before_Tex)
					{
						before_Tex = pGrass->pTexture;
						RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *) RwTextureGetRaster(pGrass->pTexture));
					}

					pThis->GrassMatrixSet(&matFinal,pGrass,pGrassGroup->iCameraZIndex);
					RwD3D9SetTransform( D3DTS_WORLD, &matFinal );

					if(before_ID != pGrass->iGrassID || before_iShadow != pGrass->iShadow)
					{
						before_ID		= pGrass->iGrassID; 
						before_iShadow	= pGrass->iShadow;
					}

					RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLELIST, numTris, pGrass->pVB, sizeof(GRASS_VERTEX));
				}
			}
		}
	}
	
	return TRUE;
}

BOOL	AgcmGrass::CB_SHADOW_UPDATE(PVOID pData,PVOID pClass,PVOID pCustData)
{
	((AgcmGrass*)pClass)->CheckAndUpdateInShadow();

	return TRUE;
}

void	AgcmGrass::DeleteAll(SectorGrassRoot*	pRoot)
{
	if(!pRoot) return;

	pRoot->pLockMutex.Lock();

	GrassGroup			*cur_group , *remove_group;

	cur_group = pRoot->listGrassGroup;
	while(cur_group)
	{
		if(m_pcsApmOcTree->m_bOcTreeEnable)
		{
			m_pcsAgcmOcTree->RemoveCustomRenderDataFromOcTree(&cur_group->stOctreeID,(PVOID)this,(PVOID)cur_group,NULL);
		}
		else
		{
			m_pcsAgcmRender->RemoveCustomRenderFromSector(this,&cur_group->BS,(PVOID)cur_group,NULL);
		}

		GRASSLISTITER	Iter	=	cur_group->ListGrass.begin();

		for( ; Iter != cur_group->ListGrass.end() ; ++Iter )
		{
			DEF_SAFEDELETE( (*Iter) );
		}

		cur_group->ListGrass.clear();

		remove_group = cur_group;
		cur_group = cur_group->next;

		delete remove_group;
	}

	pRoot->listGrassGroup = NULL;
	pRoot->iGrassGroupCount = 0;
	pRoot->iTotalGrassCount = 0;

	pRoot->bLoad = 0;
	pRoot->bAddRender = 0;

	pRoot->pLockMutex.Unlock();
}

void	AgcmGrass::JobQueueAdd(enumGrassJobType		type,PVOID data1,PVOID data2)
{
	m_csCSection.Lock();

	BOOL	bFind = FALSE;
	for(int i=0;i<m_iJobQueueCount;++i)
	{
		if(m_pJobQueue[i].type	== type && m_pJobQueue[i].data1 == data1 && m_pJobQueue[i].data2 == data2)
		{
			bFind = TRUE;
			break;
		}
	}

	if(!bFind)
	{
		ASSERT(m_iJobQueueCount < m_iJobQueueMaxCount);

		if(m_iJobQueueCount >= m_iJobQueueMaxCount)		// 초과시 버퍼 늘림..
		{
			GrassJobQueue*	temp = new GrassJobQueue[m_iJobQueueMaxCount];
			int				count = m_iJobQueueMaxCount;
			memcpy(temp,m_pJobQueue,sizeof(GrassJobQueue) * count);
			delete	[]m_pJobQueue;

            m_iJobQueueMaxCount *= 2;		
			m_pJobQueue = new GrassJobQueue[m_iJobQueueMaxCount];
			memcpy(m_pJobQueue,temp,sizeof(GrassJobQueue) * count);

			delete	[]temp;
		}
				
		m_pJobQueue[m_iJobQueueCount].type		= type;
		m_pJobQueue[m_iJobQueueCount].data1		= data1;
		m_pJobQueue[m_iJobQueueCount++].data2	= data2;
	}

	m_csCSection.Unlock();
}

void	AgcmGrass::JobQueueRemove(INT32	six,INT32	siz)
{
	SectorGrassRoot*	pRoot;

	m_csCSection.Lock();

	for(int i=0;i<m_iJobQueueCount;++i)
	{
		if(m_pJobQueue[i].type == GRASS_JOB_REMOVED || m_pJobQueue[i].type == GRASS_JOB_CLEAR_ROOT) continue;

		pRoot = (SectorGrassRoot*)m_pJobQueue[i].data1;
		
		if(pRoot->six == six && pRoot->siz == siz)
		{
			if(m_pJobQueue[i].type == GRASS_JOB_ADD_RENDER)
			{
				m_pJobQueue[i].type = GRASS_JOB_REMOVED;
			}
			else if(m_pJobQueue[i].type == GRASS_JOB_ADD_ROOT)			// new 한 객체 release
			{
				DeleteAll(pRoot);
				delete pRoot;
				m_pJobQueue[i].type = GRASS_JOB_REMOVED;
			}
		}
	}

	m_csCSection.Unlock();
}

inline DWORD * __GetBufferOffset( DWORD * pBuffer , int nOffset , int nLoadSize )
{
	static DWORD _sDummy = -1;
	if( ( nOffset + 1 ) * ( int ) sizeof( INT32 ) <= nLoadSize )
	{
		return &pBuffer[ ( nOffset ) ];
	}
	else
	{
		return &_sDummy;
	}
}

void	AgcmGrass::LoadGrass(SectorGrassRoot*	pRoot)
{
	if(pRoot->bLoad == 1) return;			// 이미 읽었다..

	INT32	six = pRoot->six;
	INT32	siz = pRoot->siz;

	INT32 lsix = six / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;				 // MAP_DEFAULT_DEPTH 단위로 증가하기 위해
	INT32 lsiz = siz / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;

	if(six < 0 && six % MAP_DEFAULT_DEPTH != 0) lsix -= MAP_DEFAULT_DEPTH;
	if(siz < 0 && siz % MAP_DEFAULT_DEPTH != 0) lsiz -= MAP_DEFAULT_DEPTH;

	INT32 loffx = six - lsix;
	INT32 loffz = siz - lsiz;
	
	INT32	x_arrayindex,z_arrayindex,division_index;
	x_arrayindex = SectorIndexToArrayIndexX(lsix);
	z_arrayindex = SectorIndexToArrayIndexZ(lsiz);
	division_index = GetDivisionIndex( x_arrayindex , z_arrayindex );
	char		fstr[100];
	memset(fstr,'\0',100);

	#ifdef USE_MFC
	sprintf(fstr,"map\\data\\grass\\GR%d.dat",division_index);
	#else
	sprintf(fstr,"world\\grass\\GR%d.dat",division_index);
	#endif
	
	ApdFile				csApdFile;
	AuPackingManager*	pPackingManager = AuPackingManager::GetSingletonPtr();
	if(!pPackingManager->OpenFile(fstr,&csApdFile))
	{
		ASSERT(!"파일을 읽는데 실패하였습니다.GRASS");
		return;
	}

	// Version History
	// 1 -  fileoffset미리 계산해서 저장
	// 2 -  grass group의 BS.radius값 늘림

	DWORD	version = 0;
	pPackingManager->ReadFile( &version, 4, &csApdFile );

#ifdef USE_MFC
	if(version != 1 && version != 2)			// version 체크
	{
		MD_SetErrorMessage("그래스 파일 버전이 틀립니다!");
		pRoot = NULL;
		return;
	}
#endif

	INT32	foffset = 0;
	INT32	iLoadSize = 0;
	foffset = loffz * MAP_DEFAULT_DEPTH * 8 + loffx * 8 + 4;		// version 정보 추가
	pPackingManager->SetPos(foffset,&csApdFile);
	pPackingManager->ReadFile( &foffset, 4, &csApdFile );
	pPackingManager->ReadFile( &iLoadSize, 4, &csApdFile );

	DWORD*		loadBuffer = new DWORD[iLoadSize/4];
	INT32		iLoadIndex = 0;
	INT32		root_num = 0;
	
	pPackingManager->SetPos(foffset,&csApdFile);
	INT32	LoadFileSize = pPackingManager->ReadFile( loadBuffer, iLoadSize, &csApdFile );
	if (!LoadFileSize)
	{
		delete	[]loadBuffer;
		pPackingManager->CloseFile(&csApdFile);
		return;
	}

	pPackingManager->CloseFile(&csApdFile);
	
	DWORD	sDummy = -1;
	//BOOL	bBreak = FALSE;

	#define	GetBufferINT32( nOffset ) *( ( INT32 * )__GetBufferOffset( loadBuffer , ( nOffset ) , LoadFileSize ) )
	#define	GetBufferFLOAT( nOffset ) *( ( float * )__GetBufferOffset( loadBuffer , ( nOffset ) , LoadFileSize ) )

	INT32	group_num;
	group_num = GetBufferINT32(iLoadIndex++);

	if(group_num == 0)
	{
		delete []loadBuffer;
		return;
	}

	GrassGroup*		nw_group;
	Grass*			nw_grass;
	INT32			iTemp;
	INT32			gsix,gsiz;

	pRoot->iGrassGroupCount = group_num;
	
	for(int i=0;i<group_num;++i)
	{
		nw_group = new GrassGroup;

		nw_group->iCameraZIndex = 0;
		nw_group->iGrassNum =  GetBufferINT32( iLoadIndex++ );//((INT32*)loadBuffer)[iLoadIndex++];
		pRoot->iTotalGrassCount += nw_group->iGrassNum;

		gsix = GetBufferINT32(  iLoadIndex++ );
		gsiz = GetBufferINT32(  iLoadIndex++ );
		nw_group->stOctreeID.ID = GetBufferINT32(  iLoadIndex++ );

		nw_group->stOctreeID.six = gsix;
		nw_group->stOctreeID.siz = gsiz;
		
		nw_group->ListGrass.clear();

		nw_group->BS.center.x = GetBufferFLOAT(  iLoadIndex++ );	
		nw_group->BS.center.y = GetBufferFLOAT(  iLoadIndex++ );	
		nw_group->BS.center.z = GetBufferFLOAT(  iLoadIndex++ );	
		nw_group->BS.radius = GetBufferFLOAT(  iLoadIndex++ );	
		if(version == 1) nw_group->BS.radius = AGCM_GRASS_SPHERE_RADIUS;		 

		nw_group->MaxY = GetBufferFLOAT(  iLoadIndex++ );	
			
		int iGrassNumberEditted = nw_group->iGrassNum;
		for(int j=0;j<nw_group->iGrassNum;++j)
		{
			if( ( iLoadIndex + 7 ) * ( int ) sizeof( INT32 ) < LoadFileSize )
			{
				nw_grass = new Grass;
				nw_grass->Init();

				if(!nw_grass)
				{
					nw_group->iGrassNum = j;
					break; // ApMemory꽉 참..
				}

				iTemp = GetBufferINT32(  iLoadIndex++ );
				nw_grass->iGrassID = iTemp;

				nw_grass->vPos.x = GetBufferFLOAT(  iLoadIndex++ );	
				nw_grass->vPos.y = GetBufferFLOAT(  iLoadIndex++ );	
				nw_grass->vPos.z = GetBufferFLOAT(  iLoadIndex++ );	

				nw_grass->fRotX = GetBufferFLOAT(  iLoadIndex++ );	
				nw_grass->fRotY = GetBufferFLOAT(  iLoadIndex++ );	
				nw_grass->fScale = GetBufferFLOAT(  iLoadIndex++ );	
							
				if( InitGrass(nw_grass) )
				{
					nw_group->ListGrass.push_front( nw_grass );
				}
				else
				{
					iGrassNumberEditted--;
				}
			}
		}

		nw_group->iGrassNum = iGrassNumberEditted;

		if( nw_group->iGrassNum > 0 )
		{
			nw_group->next = pRoot->listGrassGroup;
			pRoot->listGrassGroup = nw_group;

			nw_group->InitVertOrder();
		}
		else
		{
			// 그룹 삭제..
			group_num--;		// 풀 그룹 개수
		}
	}

	pRoot->iGrassGroupCount = group_num;		// 풀 그룹 개수

	delete []loadBuffer;
		
	pRoot->bLoad = 1;
}

BOOL	AgcmGrass::CB_LOAD_MAP(PVOID pData,PVOID pClass,PVOID pCustData)
{
	AgcmGrass*		pThis = (AgcmGrass*)pClass;
	ApWorldSector*	pSector = (ApWorldSector*)pData;
	INT32			nDetail = (INT32)pCustData;

	if (nDetail != SECTOR_HIGHDETAIL)	return TRUE;		// 풀은 디테일 영역에서만 출연하므로..

	INT32	six = pSector->GetIndexX();
	INT32	siz = pSector->GetIndexZ();

	pThis->m_csCSection.Lock();
	SectorGrassRoot*	pRoot = pThis->m_listGrassRoot;
	BOOL				bFind = FALSE;
	SectorGrassRoot*	nw_root;

	while(pRoot)
	{
		if(pRoot->six == six && pRoot->siz == siz)
		{
			bFind = TRUE;
			nw_root = pRoot;
			break;
		}

		pRoot = pRoot->next;
	}

	pThis->JobQueueRemove(six,siz);

	pThis->m_csCSection.Unlock();

	if(!bFind)
	{
		SectorGrassRoot*	nw_root = new SectorGrassRoot;
		nw_root->listGrassGroup = NULL;
		nw_root->iGrassGroupCount = 0;
		nw_root->six = six;
		nw_root->siz = siz;

		nw_root->iTotalGrassCount = 0;

		nw_root->bAddRender = 0;
		nw_root->bLoad = 0;

		nw_root->pLockMutex.Init();
		nw_root->pLockMutex.Lock();

		if( pThis->m_eDetail != GRASS_DETAIL_OFF )		
		{
			pThis->LoadGrass(nw_root);
			pThis->JobQueueAdd(GRASS_JOB_ADD_RENDER, (PVOID)nw_root,NULL);
			pThis->JobQueueAdd(GRASS_JOB_ADD_ROOT, (PVOID)nw_root,NULL);
		}
		else
		{
			pThis->JobQueueAdd(GRASS_JOB_ADD_ROOT, (PVOID)nw_root,NULL);
		}

		nw_root->pLockMutex.Unlock();
	}
	else
	{
		nw_root->pLockMutex.Lock();

		if( pThis->m_eDetail != GRASS_DETAIL_OFF )		
		{
			pThis->LoadGrass(nw_root);
			pThis->JobQueueAdd(GRASS_JOB_ADD_RENDER, (PVOID)nw_root,NULL);
		}

		nw_root->pLockMutex.Unlock();
	}
	/*
		
	SectorGrassRoot*	pRoot = pThis->m_listGrassRoot;
	SectorGrassRoot*	find_root = NULL;
	BOOL				bFind = FALSE;
	SectorGrassRoot*	nw_root;

	while(pRoot)
	{
		if(pRoot->six == six && pRoot->siz == siz)
		{
			bFind = TRUE;
			nw_root = pRoot;
			break;
		}

		pRoot = pRoot->next;
	}

	if(!bFind)
	{
		SectorGrassRoot*	nw_root = new SectorGrassRoot;
		nw_root->listGrassGroup = NULL;
		nw_root->iGrassGroupCount = 0;
		nw_root->six = six;
		nw_root->siz = siz;

		nw_root->iTotalGrassCount = 0;
		pThis->JobQueueAdd(GRASS_JOB_ADD_ROOT,nw_root,NULL);
	}

	if( pThis->m_eDetail != GRASS_DETAIL_OFF )		
	{
		INT32 lsix = six / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;				 // MAP_DEFAULT_DEPTH 단위로 증가하기 위해
		INT32 lsiz = siz / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;

		if(six < 0 && six % MAP_DEFAULT_DEPTH != 0) lsix -= MAP_DEFAULT_DEPTH;
		if(siz < 0 && siz % MAP_DEFAULT_DEPTH != 0) lsiz -= MAP_DEFAULT_DEPTH;

		INT32 loffx = six - lsix;
		INT32 loffz = siz - lsiz;
		
		HANDLE fd =	pThis->GetHandle(lsix,lsiz);
		if(fd == INVALID_HANDLE_VALUE)
		{
			ASSERT(!"파일을 읽는데 실패하였습니다.GRASS");
			return TRUE;
		}

		DWORD FP;
		INT32 foffset = 0;

		foffset = loffz * MAP_DEFAULT_DEPTH * 4  + loffx * 4;

		SetFilePointer(fd,foffset,NULL,FILE_BEGIN);
		ReadFile(fd,&foffset,sizeof(foffset),&FP,NULL);
		SetFilePointer(fd,foffset,NULL,FILE_BEGIN);

		INT32	group_num;
		ReadFile(fd,&group_num,sizeof(INT32),&FP,NULL);		// 그룹 개수

		if(group_num == 0)
		{
			return TRUE;
		}

		GrassGroup*		nw_group;
		Grass*			nw_grass;
		INT32			iTemp;

		nw_root->iGrassGroupCount = group_num;
		
		for(int i=0;i<group_num;++i)
		{
			nw_group = new GrassGroup;

			nw_group->iCameraZIndex = 0;

			ReadFile(fd,&nw_group->iGrassNum,sizeof(INT32),&FP,NULL);

			nw_root->iTotalGrassCount += nw_group->iGrassNum;

			ReadFile(fd,&nw_group->OcTreeID.six,sizeof(INT32),&FP,NULL);
			ReadFile(fd,&nw_group->OcTreeID.siz,sizeof(INT32),&FP,NULL);
			ReadFile(fd,&nw_group->OcTreeID.ID,sizeof(INT32),&FP,NULL);
			nw_group->OcTreeID.next = NULL;

			nw_group->listGrass = NULL;
			
			ReadFile(fd,&nw_group->BS.center.x,sizeof(FLOAT),&FP,NULL);
			ReadFile(fd,&nw_group->BS.center.y,sizeof(FLOAT),&FP,NULL);
			ReadFile(fd,&nw_group->BS.center.z,sizeof(FLOAT),&FP,NULL);
			ReadFile(fd,&nw_group->BS.radius,sizeof(FLOAT),&FP,NULL);

			ReadFile(fd,&nw_group->MaxY,sizeof(FLOAT),&FP,NULL);

			for(int j=0;j<nw_group->iGrassNum;++j)
			{
				nw_grass = new Grass;

				if(!nw_grass)
				{
					nw_group->iGrassNum = j;
					break; // ApMemory꽉 참..
				}

				ReadFile(fd,&iTemp,sizeof(INT32),&FP,NULL);	
				nw_grass->iGrassID = iTemp;

				ReadFile(fd,&nw_grass->vPos.x,sizeof(FLOAT),&FP,NULL);
				ReadFile(fd,&nw_grass->vPos.y,sizeof(FLOAT),&FP,NULL);
				ReadFile(fd,&nw_grass->vPos.z,sizeof(FLOAT),&FP,NULL);

				ReadFile(fd,&nw_grass->fRotX,sizeof(FLOAT),&FP,NULL);
				ReadFile(fd,&nw_grass->fRotY,sizeof(FLOAT),&FP,NULL);
				ReadFile(fd,&nw_grass->fScale,sizeof(FLOAT),&FP,NULL);

				pThis->InitGrass(nw_grass);

				nw_grass->next = nw_group->listGrass;
				nw_group->listGrass = nw_grass;
			}

			nw_group->next = nw_root->listGrassGroup;
			nw_root->listGrassGroup = nw_group;

			nw_group->VertDrawOrder[0].x = nw_group->BS.center.x - nw_group->BS.radius;
			nw_group->VertDrawOrder[0].y = nw_group->MaxY + AGCM_GRASS_HEIGHT_OFFSET;
			nw_group->VertDrawOrder[0].z = nw_group->BS.center.z - nw_group->BS.radius;

			nw_group->VertDrawOrder[1].x = nw_group->BS.center.x + nw_group->BS.radius;
			nw_group->VertDrawOrder[1].y = nw_group->MaxY + AGCM_GRASS_HEIGHT_OFFSET;
			nw_group->VertDrawOrder[1].z = nw_group->BS.center.z - nw_group->BS.radius;

			nw_group->VertDrawOrder[2].x = nw_group->BS.center.x + nw_group->BS.radius;
			nw_group->VertDrawOrder[2].y = nw_group->MaxY + AGCM_GRASS_HEIGHT_OFFSET;
			nw_group->VertDrawOrder[2].z = nw_group->BS.center.z + nw_group->BS.radius;

			nw_group->VertDrawOrder[3].x = nw_group->BS.center.x - nw_group->BS.radius;
			nw_group->VertDrawOrder[3].y = nw_group->MaxY + AGCM_GRASS_HEIGHT_OFFSET;
			nw_group->VertDrawOrder[3].z = nw_group->BS.center.z + nw_group->BS.radius;
			
			for(int k=0;k<4;++k)
			{
				nw_group->listDrawOrder[k] = NULL;
			}
			
			pThis->SortDrawOrder(nw_group);
		}
	}*/

	return TRUE;
}

// octree mode client용
BOOL	AgcmGrass::CB_UNLOAD_MAP(PVOID pData,PVOID pClass,PVOID pCustData)
{
	AgcmGrass*		pThis = (AgcmGrass*)pClass;
	ApWorldSector*	pSector = (ApWorldSector*)pData;
	
	//ApmOcTree*		pApmOcTree = pThis->m_pcsApmOcTree;

	//if(!pApmOcTree->m_pCenterSector)
	//	return TRUE;

	//INT32	ix = pSector->GetIndexX();
	//INT32	iz = pSector->GetIndexZ();
	//INT32	ix2 = pApmOcTree->m_pCenterSector->GetIndexX();
	//INT32	iz2 = pApmOcTree->m_pCenterSector->GetIndexZ();
	//INT32	dx,dz;

	//if(ix > ix2) dx = ix - ix2;
	//else dx = ix2 - ix;
	//if(iz > iz2) dz = iz - iz2;
	//else dz = iz2 - iz;

	//if(dx <= AGCM_GRASS_LOAD_SECTOR_RANGE || dz <= AGCM_GRASS_LOAD_SECTOR_RANGE)	
	//{
		//return TRUE;
	//}
	
	INT32	six = pSector->GetIndexX();
	INT32	siz = pSector->GetIndexZ();

	SectorGrassRoot*	cur_root = pThis->m_listGrassRoot;

	while(cur_root)
	{
		if(cur_root->six == six && cur_root->siz == siz)
		{
			pThis->JobQueueRemove(six,siz);
			// 삭제
			pThis->JobQueueAdd(GRASS_JOB_REMOVE_ROOT,cur_root,NULL);
			return TRUE;
		}

		cur_root = cur_root->next;
	}

	return TRUE;
}

void	AgcmGrass::CheckAndUpdateInShadow()
{
	if(m_eDetail == GRASS_DETAIL_OFF) return;

	SectorGrassRoot*		cur_root = m_listGrassRoot;
	GrassGroup*				cur_group;
	BOOL					res;
	
	while(cur_root)
	{
		cur_group = cur_root->listGrassGroup;
		while(cur_group)
		{
			GRASSLISTITER	Iter	=	cur_group->ListGrass.begin();
			for( ; Iter != cur_group->ListGrass.end() ; ++Iter )
			{
				Grass*		pGrass		=	(*Iter);

				res = m_pcsAgcmShadow2->GetShadowBuffer(&pGrass->vPos);

				if(res)
				{
					//cur_grass->pvVB = m_astGrassInfo[cur_grass->iGrassID].pvOriginalVB_Dark;
					pGrass->pVB = m_astGrassInfo[pGrass->iGrassID].pOriginalDarkVB;

					pGrass->iVertexOffset = m_astGrassInfo[pGrass->iGrassID].iVertexOffset_Dark; 
					pGrass->iShadow = 1;				// on
				}
				else
				{
					//cur_grass->pvVB = m_astGrassInfo[cur_grass->iGrassID].pvOriginalVB;
					pGrass->pVB = m_astGrassInfo[pGrass->iGrassID].pOriginalVB;

					pGrass->iVertexOffset = m_astGrassInfo[pGrass->iGrassID].iVertexOffset;
					pGrass->iShadow = 0;				// off
				}
			}

			cur_group = cur_group->next;
		}
			
		cur_root = cur_root->next;
	}
}

// MapTool용 ///////////////////////////////////////////////////////////////////////////////////////////
void	AgcmGrass::AddGrass(INT32 iGrassID,FLOAT x,FLOAT z,FLOAT fRotX,FLOAT fRotY,FLOAT fScale)
{
	if(iGrassID < 0) return;

	FLOAT y = m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(x,z);

	Grass*		nw_grass = new Grass;
	nw_grass->Init();

	nw_grass->fScale = fScale;
	nw_grass->fRotX = fRotX;
	nw_grass->fRotY = fRotY;
	
	nw_grass->vPos.x = x;
	nw_grass->vPos.y = y;
	nw_grass->vPos.z = z;

	nw_grass->iGrassID = iGrassID;

	InitGrass(nw_grass);

	GrassGroup*		nw_group = new GrassGroup;

	nw_group->iCameraZIndex = 0;
	nw_group->MaxY = y;
	nw_group->iGrassNum = 1;
	
	nw_group->BS.center.x = x;
	nw_group->BS.center.y = y;
	nw_group->BS.center.z = z;
	nw_group->BS.radius = AGCM_GRASS_SPHERE_RADIUS;

	nw_group->ListGrass.push_front( nw_grass );

	int		six,siz;
	six = PosToSectorIndexX(x);
	siz = PosToSectorIndexZ(z); 

	six = SectorIndexToArrayIndexX(six);
	siz = SectorIndexToArrayIndexZ(siz);
	
	SectorGrassRoot*		cur_find = GetGrassRoot( six , siz );
	if( !cur_find )
	{
		SectorGrassRoot*	nw_root = new SectorGrassRoot;
		
		nw_root->six = six;
		nw_root->siz = siz;

		nw_group->next = NULL;
		nw_root->listGrassGroup = nw_group;

		nw_root->next = m_listGrassRoot;
		m_listGrassRoot = nw_root;

		nw_root->iTotalGrassCount = 1;
		nw_root->iGrassGroupCount = 1;

		nw_root->bAddRender = 1;
		nw_root->bLoad = 1;
	}
	else
	{
		nw_group->next = cur_find->listGrassGroup;
		cur_find->listGrassGroup = nw_group;

		++cur_find->iGrassGroupCount;
		++cur_find->iTotalGrassCount;
	}

	// Draw Order Set
	nw_group->VertDrawOrder[0].x = nw_group->BS.center.x - nw_group->BS.radius;
	nw_group->VertDrawOrder[0].y = nw_group->BS.center.y + nw_group->BS.radius;
	nw_group->VertDrawOrder[0].z = nw_group->BS.center.z - nw_group->BS.radius;

	nw_group->VertDrawOrder[1].x = nw_group->BS.center.x + nw_group->BS.radius;
	nw_group->VertDrawOrder[1].y = nw_group->BS.center.y + nw_group->BS.radius;
	nw_group->VertDrawOrder[1].z = nw_group->BS.center.z - nw_group->BS.radius;

	nw_group->VertDrawOrder[2].x = nw_group->BS.center.x - nw_group->BS.radius;;
	nw_group->VertDrawOrder[2].y = nw_group->BS.center.y + nw_group->BS.radius;
	nw_group->VertDrawOrder[2].z = nw_group->BS.center.z + nw_group->BS.radius;

	nw_group->VertDrawOrder[3].x = nw_group->BS.center.x + nw_group->BS.radius;;
	nw_group->VertDrawOrder[3].y = nw_group->BS.center.y + nw_group->BS.radius;
	nw_group->VertDrawOrder[3].z = nw_group->BS.center.z + nw_group->BS.radius;

	m_pcsAgcmRender->AddCustomRenderToSector(this,&nw_group->BS,CB_GRASS_UPDATE,CB_GRASS_RENDER,(PVOID)nw_group,NULL,AGCM_GRASS_SECTOR_APPEAR_DIST);
}

void	AgcmGrass::RemoveGrass(RwSphere*	sphere)
{
	GrassGroup		*cur_group		= NULL,
					*before_group	= NULL,
					*remove_group	= NULL;
	FLOAT			dx,dz,dist;

	SectorGrassRoot*	cur_root = m_listGrassRoot;
	ApWorldSector	*	pSector;
	
	while(cur_root)
	{
		pSector = this->m_pcsApmMap->GetSectorByArrayIndex( cur_root->six , cur_root->siz );
		if( pSector && pSector->IsInSectorRadius( sphere->center.x , sphere->center.z , sphere->radius ) )
		{
			cur_group = cur_root->listGrassGroup;	
			while(cur_group)
			{
				// 각 녀석들을 검사한다.
				GRASSLISTITER	Iter	=	cur_group->ListGrass.begin();

				BOOL	bDeleted = FALSE;

				for( ; Iter != cur_group->ListGrass.end() ;  )
				{
					Grass*		pGrass		=	(*Iter);

					dx = pGrass->vPos.x - sphere->center.x;
					dz = pGrass->vPos.z - sphere->center.z;

					dist = sqrt( dx * dx + dz * dz );

					if( dist < sphere->radius )
					{
						// 이 풀 삭제.
						DEF_SAFEDELETE( (*Iter) );
						cur_group->ListGrass.erase( Iter++ );
						cur_group->iGrassNum --;
						cur_root->iTotalGrassCount --;
						ASSERT( cur_group->iGrassNum >= 0 );

						bDeleted = TRUE;
					}
					else
					{
						++Iter;
					}
				}

				if( cur_group->iGrassNum == 0 )
				{
					m_pcsAgcmRender->RemoveCustomRenderFromSector(this,&cur_group->BS,(PVOID)cur_group,NULL);

					// 그룹 제거..
					if(cur_group == cur_root->listGrassGroup)
					{
						cur_root->listGrassGroup = cur_group->next;
					}
					else
					{
						ASSERT( NULL != before_group );
						before_group->next = cur_group->next;
					}

					remove_group = cur_group;
					cur_group = cur_group->next;

					--cur_root->iGrassGroupCount;
					delete remove_group;
				}
				else
				{
					if( bDeleted )
					{

					}

					before_group = cur_group;
					cur_group = cur_group->next;
				}
			}
		}

		cur_root = cur_root->next;
	}
}

void	AgcmGrass::UpdateGrassHeight()
{
	GrassGroup		*cur_group;

	SectorGrassRoot*	cur_root = m_listGrassRoot;
	while(cur_root)
	{
		cur_group = cur_root->listGrassGroup;

		if( cur_group )
		{
			cur_group->BS.center.y = m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(cur_group->BS.center.x,cur_group->BS.center.z);

			while(cur_group)
			{

				GRASSLISTITER	Iter	=	cur_group->ListGrass.begin();
				for( ; Iter != cur_group->ListGrass.end() ; ++Iter )
				{
					Grass*	pGrass		=	(*Iter);
					pGrass->vPos.y = m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly( pGrass->vPos.x , pGrass->vPos.z );
				}

				cur_group = cur_group->next;
			}
		}

		cur_root = cur_root->next;
	}
}

void	AgcmGrass::UpdateGrassHeight(ApWorldSector*	pSector)
{
	GrassGroup		*cur_group;
	
	SectorGrassRoot*	cur_root = m_listGrassRoot;
	while(cur_root)
	{
		if(cur_root->six == pSector->GetIndexX() && cur_root->siz == pSector->GetIndexZ())
		{
			cur_group = cur_root->listGrassGroup;

			if( cur_group )
			{
				cur_group->BS.center.y = m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(cur_group->BS.center.x,cur_group->BS.center.z);

				while(cur_group)
				{

					GRASSLISTITER		Iter		=	cur_group->ListGrass.begin();
					for( ; Iter != cur_group->ListGrass.end() ; ++Iter )
					{
						Grass* pGrass		=	(*Iter);
						pGrass->vPos.y = m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly( pGrass->vPos.x , pGrass->vPos.z );
					}

					cur_group = cur_group->next;
				}
			}
		}

		cur_root = cur_root->next;
	}
}

// Grass들로 octree id test!
BOOL	AgcmGrass::CB_OCTREE_ID_SET(PVOID pData,PVOID pClass,PVOID pCustData)
{
	AgcmGrass*	pThis = (AgcmGrass*)pClass;

	RwV3d		check_vert;

	int		res_ID,six,siz;
	OcTreeNode*		pTreeNode;

	SectorGrassRoot*	pRoot = pThis->m_listGrassRoot;
	GrassGroup*			pGrassGroup;
	GrassGroup*			pBeforeGrassGroup;

	while(pRoot)
	{
		pGrassGroup = pRoot->listGrassGroup;

		while(pGrassGroup)
		{
			check_vert.x = pGrassGroup->BS.center.x;
			check_vert.y = pGrassGroup->BS.center.y;	
			check_vert.z = pGrassGroup->BS.center.z;

			res_ID = pThis->m_pcsApmOcTree->GetLeafID((AuPOS*)&check_vert);

			if(res_ID != 0xffffffff)		// error
			{
				six = PosToSectorIndexX(check_vert.x);
				siz = PosToSectorIndexZ(check_vert.z); 

				six = SectorIndexToArrayIndexX(six);
				siz = SectorIndexToArrayIndexZ(siz);

				if(pRoot->six == six && pRoot->siz == siz)			// 풀은 한섹터에만 추가하자 ^^
				{
					pGrassGroup->stOctreeID.six = six;
					pGrassGroup->stOctreeID.siz = siz;
					pGrassGroup->stOctreeID.ID = res_ID;

					pTreeNode = pThis->m_pcsApmOcTree->GetNodeForInsert(six,siz,res_ID);
					if(pTreeNode)
					{
						++pTreeNode->objectnum;
					}
				}

				pBeforeGrassGroup = pGrassGroup;
			}
			else
			{
				six = PosToSectorIndexX(check_vert.x);
				siz = PosToSectorIndexZ(check_vert.z); 

				six = SectorIndexToArrayIndexX(six);
				siz = SectorIndexToArrayIndexZ(siz);

				if( pThis->m_pcsAgcmMap->GetLoadRangeX1() <= six	&&
					six < pThis->m_pcsAgcmMap->GetLoadRangeX2()		&&
					pThis->m_pcsAgcmMap->GetLoadRangeZ1() <= siz	&&
					siz < pThis->m_pcsAgcmMap->GetLoadRangeZ2()		)
				{
					MD_SetErrorMessage( "AgcmGrass::CB_OCTREE_ID_SET 옥트리 아이디 설정 실패!(%d,%d)섹터" , six , siz );
				}
				else
				{

				}
			}
			pGrassGroup = pGrassGroup->next;
		}// while(pGrassGroup)

		pRoot = pRoot->next;
	}			// while(pRoot)

	// pThis->SaveToFiles( ( char * ) pData );		// Save!!

	return TRUE;
}

FLOAT	AgcmGrass::GetGrassMinHeight( ApWorldSector*	pSector , FLOAT fTopHeight )
{
	int		six,siz;

	SectorGrassRoot*	pRoot = m_listGrassRoot;
	GrassGroup*			pGrassGroup;

	while(pRoot)
	{
		pGrassGroup = pRoot->listGrassGroup;

		while(pGrassGroup)
		{
			six = PosToSectorIndexX(pGrassGroup->BS.center.x);
			siz = PosToSectorIndexZ(pGrassGroup->BS.center.z); 

			six = SectorIndexToArrayIndexX(six);
			siz = SectorIndexToArrayIndexZ(siz);

			if( pSector->GetArrayIndexX() == six && pSector->GetArrayIndexZ() == siz)
			{
				if( pGrassGroup->BS.center.y < fTopHeight )
				{
					fTopHeight = pGrassGroup->BS.center.y;
				}
			}
			pGrassGroup = pGrassGroup->next;
		}

		pRoot = pRoot->next;
	}

	return fTopHeight;
}

BOOL	AgcmGrass::CB_GET_MIN_HEIGHT ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmGrass		* pThis		= ( AgcmGrass *		) pClass	;
	ApWorldSector	* pSector	= ( ApWorldSector *	) pData		;
	FLOAT			* pHeight	= ( FLOAT *			) pCustData	;

	#ifdef USE_MFC
	// 클라이언트에서 데이타가 꼬이는 경우가 있는것 같다.
	*pHeight = pThis->GetGrassMinHeight( pSector , *pHeight );
	#endif

	return TRUE;
}

BOOL	AgcmGrass::CheckGrassList		()
{
	// 체크할 요소.
	// 1, 갯수가 실제 갯수와 같은가
	// 2, 하위 풀들이 루트의 범위를 벗어나진 않는가.
	// 3, 실제 있는 풀 인덱스인가.

	SectorGrassRoot	*	pRoot		= m_listGrassRoot;
	GrassGroup		*	cur_group;

	int		nGrassCount_group	;
	int		nGrassCount_root	;

	while(pRoot)
	{
		cur_group = pRoot->listGrassGroup;

		nGrassCount_root	= 0;

		while(cur_group)
		{

			nGrassCount_group	= 0;
			GRASSLISTITER		Iter		=	cur_group->ListGrass.begin();

			for( ; Iter != cur_group->ListGrass.end() ; ++Iter )
			{
				Grass*		pGrass		=	(*Iter);
				if( pRoot->DoContainThis( &pGrass->vPos ) )
				{
					nGrassCount_group++;
				}
				else
				{
					MD_SetErrorMessage( "루트에 포함돼지 않는 풀이 존재한다." );
					return FALSE;
				}
			}

			if( cur_group->iGrassNum != nGrassCount_group )
			{
				MD_SetErrorMessage( "그룹에 풀의 갯수가 다르다." );
				return FALSE;
			}

			nGrassCount_root += nGrassCount_group;
			
			cur_group = cur_group->next;
		}

		if( pRoot->iTotalGrassCount != nGrassCount_root )
		{
			MD_SetErrorMessage( "루트에 풀의 갯수가 다르다." );
			return FALSE;
		}		

		pRoot = pRoot->next;
	}

	return TRUE;
}

void	AgcmGrass::OptimaizeGrassList	()
{

}

void	AgcmGrass::UnityForSave()
{
	SectorGrassRoot*		pRoot = m_listGrassRoot;
	GrassGroup*				cur_group;
	GrassGroup*				before_group;
	GrassGroup*				main_group;
	GrassGroup*				remove_group;

	// sort parameter clear
	while(pRoot)
	{
		cur_group = pRoot->listGrassGroup;
		while(cur_group)
		{
			cur_group->iCameraZIndex = 0;
			cur_group = cur_group->next;
		}
		
		pRoot = pRoot->next;
	}

	pRoot = m_listGrassRoot;
	BOOL	bSortRemain;
	
	while(pRoot)
	{
		while(1)
		{
			bSortRemain = FALSE;
			cur_group = pRoot->listGrassGroup;
			while(cur_group)
			{
				if(cur_group->iCameraZIndex == 0)
				{
					bSortRemain = TRUE;
					break;
				}

				before_group = cur_group;
				cur_group = cur_group->next;
			}

			if(!bSortRemain)			// sort 끝
			{
				break;		
			}
			else						// sort 할 기준 노드 지정 그리고 그 노드와 뭉쳐져야할 노드 뭉쳐라~~
			{
				cur_group->iCameraZIndex = 1;
				main_group = cur_group;

				before_group = cur_group;
				cur_group = cur_group->next;
				
				while(cur_group)
				{
					if(cur_group->iCameraZIndex == 0 
						&& cur_group->stOctreeID.ID == main_group->stOctreeID.ID)
					{
						// 그룹 헤체하고 기준 그룹으로 통합
						m_pcsAgcmRender->RemoveCustomRenderFromSector(this,&cur_group->BS,(PVOID)cur_group,NULL);
						before_group->next = cur_group->next;
	
						remove_group = cur_group;
						cur_group = cur_group->next;

						main_group->iGrassNum += remove_group->iGrassNum;
						--pRoot->iGrassGroupCount;

						copy( remove_group->ListGrass.begin() , remove_group->ListGrass.end() ,
								front_inserter( main_group->ListGrass ) );

						if(remove_group->MaxY > main_group->MaxY)
						{
							main_group->MaxY = remove_group->MaxY;
						}

						delete remove_group;
					}
					else
					{
						before_group = cur_group;
						cur_group = cur_group->next;
					}
				}
			}		// else(sort필요)
		}			// while(1)

		pRoot = pRoot->next;
	}

	pRoot = m_listGrassRoot;
	while(pRoot)
	{
		cur_group = pRoot->listGrassGroup;
		while(cur_group)
		{
			cur_group = cur_group->next;
		}

		pRoot = pRoot->next;
	}

}

void	AgcmGrass::SaveToFiles( char * szDir )
{
	INT32	LoadRangeX1 = m_pcsAgcmRender->m_iLoadRangeX1;
	INT32	LoadRangeX2 = m_pcsAgcmRender->m_iLoadRangeX2;
	INT32	LoadRangeZ1 = m_pcsAgcmRender->m_iLoadRangeZ1;
	INT32	LoadRangeZ2 = m_pcsAgcmRender->m_iLoadRangeZ2;

	INT32 lstartx = LoadRangeX1 / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;
	INT32 lstartz = LoadRangeZ1 / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;
	
	if(LoadRangeX1 < 0 && LoadRangeX1 % MAP_DEFAULT_DEPTH != 0) lstartx -= MAP_DEFAULT_DEPTH;
	if(LoadRangeZ1 < 0 && LoadRangeZ1 % MAP_DEFAULT_DEPTH != 0) lstartz -= MAP_DEFAULT_DEPTH;

	INT32 lendx = LoadRangeX2 / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH ;
	INT32 lendz = LoadRangeZ2 / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH ;
	
	if(LoadRangeX2 < 0 && LoadRangeX2 % MAP_DEFAULT_DEPTH != 0) lendx -= MAP_DEFAULT_DEPTH;
	if(LoadRangeZ2 < 0 && LoadRangeZ2 % MAP_DEFAULT_DEPTH != 0) lendz -= MAP_DEFAULT_DEPTH;

	char				fstr[100];
	HANDLE				fd;
	DWORD				FP;
	INT32				foffset;
	
	SectorGrassRoot*	cur_root;
	BOOL				bFindRoot;
	
	INT32				group_num;
		
	GrassGroup*			cur_group;
		
	UnityForSave();

	//INT32	x_arrayindex,z_arrayindex;
	INT32	division_index;
	
	for(INT32	i = lstartz; i < lendz; i += MAP_DEFAULT_DEPTH)
	{
	  for(INT32	j = lstartx; j < lendx; j+= MAP_DEFAULT_DEPTH)
	  {
        memset(fstr,'\0',100);

		division_index = GetDivisionIndex( j , i );

		if(!szDir)
		{
			sprintf(fstr,"%s\\gr%d.dat",m_szDirectory,division_index);
		}
		else
		{
			sprintf(fstr,"%s\\grass\\gr%d.dat",szDir,division_index);
		}	

		fd=CreateFile(fstr,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(fd == INVALID_HANDLE_VALUE)
		{
			CreateDirectory(m_szDirectory,NULL);

			fd=CreateFile(fstr,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		}

		DWORD	version = 2;
		WriteFile(fd,&version,sizeof(version),&FP,NULL);
		
		foffset = MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH * 8 + 4;	// 버전 정보 추가
		INT32	before_offset;
		INT32	fsize = 0;

		for(INT32	k = 0; k<MAP_DEFAULT_DEPTH ; ++k)
		{
		  for(INT32	l = 0; l<MAP_DEFAULT_DEPTH; ++l)
		  {
			SetFilePointer(fd,foffset,NULL,FILE_BEGIN);
			before_offset = foffset;

			bFindRoot = FALSE;

			cur_root = GetGrassRoot( j+l , i+k );

			if(!cur_root || cur_root->iGrassGroupCount == 0)
			{
				group_num = 0;
				WriteFile(fd,&group_num,sizeof(INT32),&FP,NULL);			
				foffset += 4;

				fsize = foffset - before_offset;
				SetFilePointer(fd,k*8*MAP_DEFAULT_DEPTH  + l*8 + 4,NULL,FILE_BEGIN);
				WriteFile(fd,&before_offset,sizeof(INT32),&FP,NULL);
				WriteFile(fd,&fsize,sizeof(INT32),&FP,NULL);

				continue;
			}

			WriteFile(fd,&cur_root->iGrassGroupCount,sizeof(INT32),&FP,NULL);			
			foffset += 4;

			cur_group = cur_root->listGrassGroup;
			for(int n=0;n<cur_root->iGrassGroupCount;++n)
			{
				ASSERT( cur_group->iGrassNum > 0 );
				WriteFile(fd,&cur_group->iGrassNum,sizeof(INT32),&FP,NULL);			
				foffset += 4;

				INT32	gsix,gsiz;
				gsix = cur_group->stOctreeID.six;
				gsiz = cur_group->stOctreeID.siz;
								
				WriteFile(fd,&gsix,sizeof(INT32),&FP,NULL);
				WriteFile(fd,&gsiz,sizeof(INT32),&FP,NULL);
				WriteFile(fd,&cur_group->stOctreeID.ID,sizeof(INT32),&FP,NULL);
				foffset += 12;

				OcTreeNode*		pTreeNode;
				pTreeNode = m_pcsApmOcTree->GetNodeForInsert(cur_group->stOctreeID.six,cur_group->stOctreeID.siz
															,cur_group->stOctreeID.ID);

				WriteFile(fd,&pTreeNode->BS.center.x,sizeof(FLOAT),&FP,NULL);
				WriteFile(fd,&pTreeNode->BS.center.y,sizeof(FLOAT),&FP,NULL);
				WriteFile(fd,&pTreeNode->BS.center.z,sizeof(FLOAT),&FP,NULL);
				WriteFile(fd,&pTreeNode->BS.radius,sizeof(FLOAT),&FP,NULL);
				foffset += 16;

				WriteFile(fd,&cur_group->MaxY,sizeof(FLOAT),&FP,NULL);
				foffset += 4;

				GRASSLISTITER		Iter		=	cur_group->ListGrass.begin();
				for(int m=0;m<cur_group->iGrassNum;++m ,++Iter)			// 같은 octree id 같는것까지 저장(maptool에선 group이 풀하나하나로 간주)
				{
					Grass*		pGrass		=	(*Iter);

					INT32	iTemp = pGrass->iGrassID;
					WriteFile(fd,&iTemp,sizeof(INT32),&FP,NULL);

					WriteFile(fd,&pGrass->vPos.x,sizeof(FLOAT),&FP,NULL);
					WriteFile(fd,&pGrass->vPos.y,sizeof(FLOAT),&FP,NULL);
					WriteFile(fd,&pGrass->vPos.z,sizeof(FLOAT),&FP,NULL);

					WriteFile(fd,&pGrass->fRotX,sizeof(FLOAT),&FP,NULL);
					WriteFile(fd,&pGrass->fRotY,sizeof(FLOAT),&FP,NULL);
					WriteFile(fd,&pGrass->fScale,sizeof(FLOAT),&FP,NULL);

					foffset += 28;
				}

				cur_group->iCameraZIndex = 0;			// save parameter clear
				cur_group = cur_group->next;
			}

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

	SectorGrassRoot*		pRoot = m_listGrassRoot;
	// sort parameter clear
	while(pRoot)
	{
		cur_group = pRoot->listGrassGroup;
		while(cur_group)
		{
			cur_group->iCameraZIndex = 0;
			cur_group = cur_group->next;
		}
		
		pRoot = pRoot->next;
	}
}

void	AgcmGrass::SetGrassDetail(enumGrassDetail	detail)
{
	if(detail != m_eDetail)
	{
		if(m_eDetail == GRASS_DETAIL_OFF)				// loading
		{
			Grass::SetEnable(TRUE);

			m_eDetail = detail;

			SectorGrassRoot			*cur_root;
			cur_root = m_listGrassRoot;
			ApWorldSector*	pSector;
			INT32			saix,saiz;
			while(cur_root)
			{
				saix = SectorIndexToArrayIndexX(cur_root->six);
				saiz = SectorIndexToArrayIndexZ(cur_root->siz);
				pSector = m_pcsApmMap->GetSectorByArrayIndex(saix,saiz);

				CB_LOAD_MAP((PVOID)pSector,(PVOID)this,(PVOID) SECTOR_HIGHDETAIL);

				cur_root = cur_root->next;
			}
		}
		else if(detail == GRASS_DETAIL_OFF)				// memory에서 clear
		{
			SectorGrassRoot			*cur_root = m_listGrassRoot;
			while(cur_root)
			{
				JobQueueRemove(cur_root->six,cur_root->siz);
				cur_root = cur_root->next;
			}
			
			JobQueueAdd(GRASS_JOB_CLEAR_ROOT,NULL,NULL);
		}
	}

	m_eDetail = detail;
	if(detail == GRASS_DETAIL_HIGH) 
	{
		m_bDrawGrass = TRUE;
		m_iDrawGrassNum = 3000;
		m_iDrawCameraIndex = 4;
		m_iSectorRange = 3;
	}
	else if(detail == GRASS_DETAIL_MEDIUM)
	{
		m_bDrawGrass = TRUE;
		m_iDrawGrassNum = 1500;
		m_iDrawCameraIndex = 3;
		m_iSectorRange = 2;
	}
	else if(detail == GRASS_DETAIL_LOW)
	{
		m_bDrawGrass = TRUE;
		m_iDrawGrassNum = 500;
		m_iDrawCameraIndex = 3;
		m_iSectorRange = 2;
	}
	else if(detail == GRASS_DETAIL_OFF)
	{
		m_bDrawGrass = FALSE;
	}
}

void	AgcmGrass::SetDirectoryPath(char*	szDir)
{
	strcpy(m_szDirectory,szDir);
}

SectorGrassRoot * AgcmGrass::GetGrassRoot( INT32 nIndexX , INT32 nIndexZ )
{
	SectorGrassRoot*		cur_find = m_listGrassRoot;
	while(cur_find)
	{
		if(cur_find->six == nIndexX && cur_find->siz == nIndexZ )
		{
			return cur_find;
		}

		cur_find = cur_find->next;
	}
	return NULL;
}

#define fAPPEARANCE_DISTANCE_CORRECTION_VALUE_FOR_GRASS		(3000.0f)

#ifdef _DEBUG
FLOAT g_fGrassDistCorrectionValue = fAPPEARANCE_DISTANCE_CORRECTION_VALUE_FOR_GRASS;
#endif

BOOL	AgcmGrass::CB_GRASS_DISTCORRECT( PVOID pDistFloat, PVOID pNull1, PVOID pNull2 )
{	
	pNull1;
	pNull2;

	if ( pDistFloat == NULL )
		return FALSE;
	
#ifdef _DEBUG
		*((FLOAT*) pDistFloat) = *((FLOAT*) pDistFloat) + g_fGrassDistCorrectionValue;
#else
		*((FLOAT*) pDistFloat) = *((FLOAT*) pDistFloat) + fAPPEARANCE_DISTANCE_CORRECTION_VALUE_FOR_GRASS;
#endif

	return TRUE;
}
