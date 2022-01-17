#include "stdafx.h"
#include "maptool.h"
#include "Plugin_Grass.h"

#include "AlefMapDocument.h"
#include "MainWindow.h"
#include "MainFrm.h"

#include "ApModule.h"
#include "GrassSetDlg.h"

#include "ApModuleStream.h"

/////////////////////////////////////////////////////////////////////////////
// CPlugin_Grass

extern	MainWindow			g_MainWindow;
extern	AgcmGrass			*g_pcsAgcmGrass;

CPlugin_Grass::CPlugin_Grass()
{
	m_rectAddGrass.		SetRect( 0		, 30 , 150 , 100 );
	m_rectSelectGrass.	SetRect( 150	, 30 , 300 , 100 );

	m_iCurWorkState = 0;

	m_bDrawGrass	= TRUE	; 
	m_iGrassDensity	= 3		;
	m_iGrassRotMin	= -5	;
	m_iGrassRotMax	= 5		;
	m_iGrassRotYMin	= 0		;
	m_iGrassRotYMax	= 360	;
	m_fGrassScaleMin = 0.8f	;
	m_fGrassScaleMax = 1.0f	;

	m_iCurSelectGrassID = -1;
	m_strCurSelectGrass.Empty();

	m_bValidPosition = FALSE;
	m_ptSelectPos.x = m_ptSelectPos.y = m_ptSelectPos.z = 0.0f;
	m_fSelectRadius = 0.0f;

	m_iCurSectorGrassNum = 0;

	m_strShortName = "Grass";
}

CPlugin_Grass::~CPlugin_Grass()
{
}


BEGIN_MESSAGE_MAP(CPlugin_Grass, CWnd)
	//{{AFX_MSG_MAP(CPlugin_Grass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_PLUGIN_GRASS_LIST, OnSelChangedTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlugin_Grass message handlers

int CPlugin_Grass::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect	rect;
	rect.SetRect(10,110,260,450);
	m_TreeGrass.Create(WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS ,rect,this,IDC_PLUGIN_GRASS_LIST);

	ApModuleStream	csStream;
	const CHAR		*szValueName = NULL;
	CHAR			szValue[256];

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(".\\Ini\\GrassTemplate.ini");

	csStream.ReadSectionName(1);
	csStream.SetValueID(-1);
	INT32			lID;
	char			Name[50];

	TV_INSERTSTRUCT		tvstruct;
	HTREEITEM			parent = 0;
	HTREEITEM			item = 0;

	while(csStream.ReadNextValue())
	{
		szValueName = csStream.GetValueName();

		if(!strcmp(szValueName, AGCMGRASS_INI_NAME_GROUP_NAME))
		{
			csStream.GetValue(szValue, 256);
			sscanf(szValue, "%s", &Name ); 

			tvstruct.hParent= 0;
			tvstruct.hInsertAfter = TVI_LAST;
			tvstruct.item.pszText = Name;
			tvstruct.item.mask = TVIF_TEXT;

			parent = m_TreeGrass.InsertItem(&tvstruct);
			m_TreeGrass.SetItemData(parent,-1);	
		}
		else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_GRASS_INFO_ID))
		{
			csStream.GetValue(szValue, 256);
			sscanf(szValue, "%d", &lID);
		}
		else if(!strcmp(szValueName, AGCMGRASS_INI_NAME_GRASS_NAME))
		{
			csStream.GetValue(szValue, 256);
			sscanf(szValue, "%s", &Name ); 

			// TreeList에 항목 추가
			tvstruct.hParent= parent;
			tvstruct.hInsertAfter = TVI_LAST;
			tvstruct.item.pszText = Name;
			tvstruct.item.mask = TVIF_TEXT;
			
			item = m_TreeGrass.InsertItem(&tvstruct);
			
			m_TreeGrass.SetItemData(item,lID);		
		}
	}

	rect.SetRect(10,470,250,490);
	m_buttonGrassSet.Create("Grass Set",WS_VISIBLE | BS_PUSHBUTTON,rect,this,IDC_PLUGIN_GRASS_SET);
	
	return 1;
}

BOOL CPlugin_Grass::OnSelectedPlugin		()
{

	return TRUE;
}

BOOL CPlugin_Grass::OnDeSelectedPlugin		()
{

	return TRUE;
}

BOOL CPlugin_Grass::OnLButtonDownGeometry	( RwV3d * pPos )
{
	PositionValidation( pPos );

	if(m_iCurWorkState == 0)			// 추가 상태
	{
		SetSaveData();
		// SaveSetChangeObjectList();

		if(m_iGrassDensity == 1 || ISBUTTONDOWN( VK_CONTROL ) || ISBUTTONDOWN( VK_MENU ) )
		{
			FLOAT x	=  pPos->x;
			FLOAT z	=  pPos->z;

			FLOAT fXRot	=	m_iGrassRotMin	+ ( m_iGrassRotMax	- m_iGrassRotMin	) * frand();
			FLOAT fYRot	=	m_iGrassRotYMin	+ ( m_iGrassRotYMax	- m_iGrassRotYMin	) * frand();

			FLOAT fScale =	m_fGrassScaleMin	+ ( m_fGrassScaleMax	- m_fGrassScaleMin	) * frand();

			g_pcsAgcmGrass->AddGrass(m_iCurSelectGrassID,x,z,fXRot,fYRot,fScale);
		}
		else
		{
			FLOAT	r1 , r2;
			FLOAT	x,z,fXRot,fYRot,fScale;

			/*
			// 마고자 (2005-11-30 오전 11:56:43) : 
			// m_iGrassDensity 는 밀도라는건 거짓말이고
			// 한 브러시 크기에 들어갈 풀의 갯수일 뿐이다.
			// 즉 브러시 크기에 따라 밀도가 달라진다는 이야긴데.
			// 이걸 진짜 밀도로 바굴라고 그랬는데 작업자들이 너무 익숙해진거 같아서
			// 코드만 남기고 방치.
			double	fGrassCount = ( double ) m_iGrassDensity * 3.141572 * m_fSelectRadius * m_fSelectRadius /
																	( MAP_STEPSIZE * MAP_STEPSIZE );

			int	nGrassCount = ( int ) fGrassCount;

			for(int i=0;i<nGrassCount;++i)
			*/

			for(int i=0;i<m_iGrassDensity;++i)
			{
				r1	= frand();
				r2	= frand();

				x	= ( pPos->x - m_fSelectRadius ) + 2.0f * m_fSelectRadius * r1	;
				z	= ( pPos->z - m_fSelectRadius ) + 2.0f * m_fSelectRadius * r2	;

				fXRot	=	m_iGrassRotMin	+ ( m_iGrassRotMax	- m_iGrassRotMin	) * frand();
				fYRot	=	m_iGrassRotYMin	+ ( m_iGrassRotYMax	- m_iGrassRotYMin	) * frand();

				fScale =	m_fGrassScaleMin	+ ( m_fGrassScaleMax	- m_fGrassScaleMin	) * frand();

				g_pcsAgcmGrass->AddGrass(m_iCurSelectGrassID,x,z,fXRot,fYRot,fScale);
			}
		}
	}

	return TRUE;
}

BOOL CPlugin_Grass::OnLButtonUpGeometry	( RwV3d * pPos )
{

	return TRUE;
}

void CPlugin_Grass::OnPaint()
{
	CAlefMapDocument*	doc = &((CMainFrame*)AfxGetMainWnd())->m_Document;
	m_fSelectRadius = doc->m_fBrushRadius;

	CPaintDC dc(this); // device context for painting

	CRect	rect;
	GetClientRect( rect );
	rect.bottom = 30;
	dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );
	dc.SetTextColor( RGB( 255 , 255 , 255 ) );

	CString	str;
	dc.SetTextColor( 0 );
	dc.FillSolidRect( m_rectAddGrass , RGB( 64 , 128 , 64 ) );
	dc.DrawText( "풀 추가" , m_rectAddGrass , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	dc.FillSolidRect( m_rectSelectGrass , RGB( 128 , 128 , 128 ) );
	dc.DrawText( "풀 선택" , m_rectSelectGrass , DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB( 255 , 255 , 255 ) );

	if(m_iCurWorkState == 0)
		dc.TextOut( 0 , 10	, "추가 모드");
	else
		dc.TextOut( 0 , 10	, "선택 모드(Del키로 삭제)");

	dc.LineTo( 0, 28);
	dc.LineTo( 300, 28);

	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB( 255 , 255 , 255 ) );
	if(m_bDrawGrass)
		dc.TextOut( 0 , 500	, "Grass Draw On");
	else 
		dc.TextOut( 0 , 500	, "Grass Draw Off");

	int	nOffset = 500;
	#define DISPLAY_TEXT( str ) dc.TextOut( 0 , nOffset	+= 20 , str )

	str.Format("Density : %d",m_iGrassDensity);
	DISPLAY_TEXT( str );
	str.Format("Rot Min : %d , Max : %d",m_iGrassRotMin , m_iGrassRotMax );
	DISPLAY_TEXT( str );
	str.Format("Rot(Y) Min : %d , Max : %d",m_iGrassRotYMin, m_iGrassRotYMax );
	DISPLAY_TEXT( str );
	str.Format("Scale Min : %.3f",m_fGrassScaleMin );
	DISPLAY_TEXT( str );
	str.Format("Scale Max : %.3f",m_fGrassScaleMax );
	DISPLAY_TEXT( str );

	str.Format("Current Grass ID : %d , Name : %s",m_iCurSelectGrassID,m_strCurSelectGrass );
	DISPLAY_TEXT( str );
	str.Format("Sector Grass Count : %d ",m_iCurSectorGrassNum);
	DISPLAY_TEXT( str );
	str.Format("Draw Grass Count : %d ",g_pcsAgcmGrass->m_iDrawGrassCountNear);
	DISPLAY_TEXT( str );

	DISPLAY_TEXT( "-" );
	DISPLAY_TEXT( "Ctrl 누르면 풀하나씩 찍음" );
	DISPLAY_TEXT( "< 와 > 버튼으로 밀도 증감" );
}

LRESULT CPlugin_Grass::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case IDC_PLUGIN_GRASS_SET:
			switch(HIWORD(wParam))
			{
				case BN_CLICKED:
					ChangeGrassSet();
				break;
			}
			break;
		}
		break;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

void	CPlugin_Grass::ChangeGrassSet()
{
	CGrassSetDlg	dlg;

	dlg.m_bGrassDraw	= m_bDrawGrass		;
	dlg.m_iDensity		= m_iGrassDensity	;
	dlg.m_iRotMin		= m_iGrassRotMin	;
	dlg.m_iRotMax		= m_iGrassRotMax	;
	dlg.m_iRotYMin		= m_iGrassRotYMin	;
	dlg.m_iRotYMax		= m_iGrassRotYMax	;
	dlg.m_fScaleMin		= m_fGrassScaleMin	;
	dlg.m_fScaleMax		= m_fGrassScaleMax	;	

	AgcmGrass*	pThis = g_pcsAgcmGrass;
	pThis->UpdateGrassHeight();

	if(IDOK == dlg.DoModal())
	{
		m_bDrawGrass		= dlg.m_bGrassDraw	;
		m_iGrassDensity		= dlg.m_iDensity	;
		m_iGrassRotMin		= dlg.m_iRotMin		;
		m_iGrassRotMax		= dlg.m_iRotMax		;
		m_iGrassRotYMin		= dlg.m_iRotYMin	;
		m_iGrassRotYMax		= dlg.m_iRotYMax	;
		m_fGrassScaleMin	= dlg.m_fScaleMin	;
		m_fGrassScaleMax	= dlg.m_fScaleMax	;		

		g_pcsAgcmGrass->m_bDrawGrass = m_bDrawGrass;
	}

	GetParent()->InvalidateRect(NULL);
}

void	CPlugin_Grass::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_rectAddGrass.PtInRect( point ) )
	{
		m_iCurWorkState = 0;
	}
	else if( m_rectSelectGrass.PtInRect( point ) )
	{
		m_iCurWorkState	= 1;
	}

	GetParent()->InvalidateRect(NULL);
	
	CWnd::OnLButtonDown(nFlags, point);
}

void	CPlugin_Grass::OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	m_iCurSelectGrassID = m_TreeGrass.GetItemData(pNMTreeView->itemNew.hItem);
	m_strCurSelectGrass = m_TreeGrass.GetItemText(pNMTreeView->itemNew.hItem);

	//AfxGetMainWnd()->InvalidateRect(NULL);

	GetParent()->InvalidateRect(NULL);
}

BOOL	CPlugin_Grass::OnMouseMoveGeometry	( RwV3d * pPos )
{
	PositionValidation( pPos );

	static int temp_count = 0;

	if( NULL == pPos )
	{
		m_bValidPosition	= FALSE;
	}
	else
	{
		m_bValidPosition	= TRUE;

		m_ptSelectPos = *pPos;

		// 풀 개수 표시용
		int		six,siz;
		six = PosToSectorIndexX(pPos->x);
		siz = PosToSectorIndexZ(pPos->z); 

		six = SectorIndexToArrayIndexX(six);
		siz = SectorIndexToArrayIndexZ(siz);
	
		SectorGrassRoot*		cur_find = g_pcsAgcmGrass->m_listGrassRoot;
		while(cur_find)
		{
			if(cur_find->six == six && cur_find->siz == siz)
			{
				m_iCurSectorGrassNum = cur_find->iTotalGrassCount;

				if(++temp_count == 10)
				{
					GetParent()->InvalidateRect(NULL);
					temp_count = 0;
				}

				break;
			}
			cur_find = cur_find->next;
		}

	}

	return TRUE;
}

BOOL	CPlugin_Grass::OnWindowRender()
{
	CAlefMapDocument*	doc = &((CMainFrame*)AfxGetMainWnd())->m_Document;
		
	g_MainWindow.DrawAreaSphere(m_ptSelectPos.x,m_ptSelectPos.y,m_ptSelectPos.z,
		doc->m_fBrushRadius  );

	return TRUE;
}

void CPlugin_Grass::PositionValidation		( RwV3d	* pPos )	// 로딩 번위 벗어난것을 체크한다.
{
	if( NULL == pPos )
	{
		// do nothing..
	}
	else
	{
		if( pPos->x < GetSectorStartX	( ArrayIndexToSectorIndexX(ALEF_LOAD_RANGE_X1) ) )
			pPos->x = GetSectorStartX	( ArrayIndexToSectorIndexX(ALEF_LOAD_RANGE_X1) );

		if( pPos->x > GetSectorEndX		( ArrayIndexToSectorIndexX(ALEF_LOAD_RANGE_X2-1) ) )
			pPos->x = GetSectorEndX		( ArrayIndexToSectorIndexX(ALEF_LOAD_RANGE_X2-1) );

		if( pPos->z < GetSectorStartZ	( ArrayIndexToSectorIndexZ(ALEF_LOAD_RANGE_Y1) ) )
			pPos->z = GetSectorStartZ	( ArrayIndexToSectorIndexZ(ALEF_LOAD_RANGE_Y1) );

		if( pPos->z > GetSectorEndZ		( ArrayIndexToSectorIndexZ(ALEF_LOAD_RANGE_Y2-1) ) )
			pPos->z = GetSectorEndZ		( ArrayIndexToSectorIndexZ(ALEF_LOAD_RANGE_Y2-1) );
	}
}

BOOL CPlugin_Grass::Window_OnKeyDown		( RsKeyStatus *ks	)
{
    switch( ks->keyCharCode )
    {
	case rsDEL:
		{
			SetSaveData();
			// SaveSetChangeObjectList();

			RwSphere	sphere;

			sphere.center = m_ptSelectPos;
			sphere.radius = m_fSelectRadius;
			// 선택한 풀 삭제..
			g_pcsAgcmGrass->RemoveGrass(&sphere);
		}
		break;
	case ',':
	case '<':
		{
			// 밀도 감소
			if( m_iGrassDensity > 1 ) m_iGrassDensity--;
			Invalidate( TRUE );
		}
		break;

	case '.':
	case '>':
		{
			// 밀도 증가.
			m_iGrassDensity++;
			Invalidate( TRUE );
		}
		break;
	default:
		break;
	}

	return TRUE;
}

BOOL	CPlugin_Grass::LoadGrassOctreeFile()
{
	AgcmGrass*	pThis = g_pcsAgcmGrass;

	INT32	LoadRangeX1 = g_Const.m_nLoading_range_x1;
	INT32	LoadRangeX2 = g_Const.m_nLoading_range_x2;
	INT32	LoadRangeZ1 = g_Const.m_nLoading_range_y1;
	INT32	LoadRangeZ2 = g_Const.m_nLoading_range_y2;

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

	Grass*				nw_grass;
	GrassGroup*			nw_group;
	SectorGrassRoot*	nw_root;

	INT32				group_count;
	INT32				iTemp;

	INT32	division_index;
	
	for(INT32	i = lstartz; i < lendz; i += MAP_DEFAULT_DEPTH)
	{
	  for(INT32	j = lstartx; j < lendx; j+= MAP_DEFAULT_DEPTH)
	  {
		division_index = GetDivisionIndex( j , i );
	
		memset(fstr,'\0',100);

		#ifdef USE_MFC
		sprintf(fstr,"map\\data\\grass\\GR%d.dat",division_index);
		#else
		sprintf(fstr,"world\\grass\\GR%d.dat",division_index);
		#endif

		fd=CreateFile(fstr,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
		if(fd == INVALID_HANDLE_VALUE)
		{
			ASSERT(!"파일을 읽는데 실패하였습니다.GRASS");
			continue;
		}

		DWORD	version = 0;
		ReadFile(fd,&version,sizeof(version),&FP,NULL);

		if(version != 1 && version != 2)			// version 체크
		{
			MD_SetErrorMessage("Grass 파일 버전이 틀립니다!");
			return FALSE;
		}
		
		foffset = 0;
		
		for(INT32	k = 0; k<MAP_DEFAULT_DEPTH ; ++k)
		{
		  for(INT32	l = 0; l<MAP_DEFAULT_DEPTH; ++l)
		  {
			// file 시작위치 
			SetFilePointer(fd,k*8*MAP_DEFAULT_DEPTH  + l*8 + 4,NULL,FILE_BEGIN);
			ReadFile(fd,&foffset,sizeof(foffset),&FP,NULL);
			SetFilePointer(fd,foffset,NULL,FILE_BEGIN);

			ReadFile(fd,&group_count,sizeof(INT32),&FP,NULL);		// 풀 그룹 개수
			if(group_count == 0) continue;

			//pSector = g_pcsApmMap->GetSectorByArrayIndex(j+l,i+k);
			//if(!pSector) continue;

			nw_root = new SectorGrassRoot;
			nw_root->listGrassGroup = NULL;
			nw_root->six = j+l;//pSector->GetArrayIndexX();
			nw_root->siz = i+k;//pSector->GetArrayIndexZ();

			if( nw_root->six < 0 || nw_root->six >= MAP_WORLD_INDEX_WIDTH ||
				nw_root->siz < 0 || nw_root->siz >= MAP_WORLD_INDEX_HEIGHT )
			{
				// 마고자 (2005-11-29 오후 4:00:33) : 
				// 범위 초과..
				delete nw_root;
				continue;
			}

			nw_root->iTotalGrassCount = 0;

			nw_root->next = pThis->m_listGrassRoot;
			pThis->m_listGrassRoot = nw_root;

			nw_root->iGrassGroupCount = group_count;		// 풀 그룹 개수
			
			for(int m=0;m<nw_root->iGrassGroupCount;++m)
			{
				nw_group = new GrassGroup;

				nw_group->iCameraZIndex = 0;

				ReadFile(fd,&nw_group->iGrassNum,sizeof(INT32),&FP,NULL);

				INT32	gsix,gsiz;
                
				ReadFile(fd,&gsix,sizeof(INT32),&FP,NULL);
				ReadFile(fd,&gsiz,sizeof(INT32),&FP,NULL);
				ReadFile(fd,&nw_group->stOctreeID.ID,sizeof(INT32),&FP,NULL);

				nw_group->stOctreeID.six = gsix;
				nw_group->stOctreeID.siz = gsiz;

				nw_group->listGrass = NULL;
				
				ReadFile(fd,&nw_group->BS.center.x,sizeof(FLOAT),&FP,NULL);
				ReadFile(fd,&nw_group->BS.center.y,sizeof(FLOAT),&FP,NULL);
				ReadFile(fd,&nw_group->BS.center.z,sizeof(FLOAT),&FP,NULL);
				ReadFile(fd,&nw_group->BS.radius,sizeof(FLOAT),&FP,NULL);

				if(version == 1) nw_group->BS.radius = AGCM_GRASS_SPHERE_RADIUS;

				ReadFile(fd,&nw_group->MaxY,sizeof(FLOAT),&FP,NULL);

				int iGrassCountNew = 0;
				for(int n=0;n<nw_group->iGrassNum;++n)
				{
					nw_grass = new Grass;

					ReadFile(fd,&iTemp,sizeof(INT32),&FP,NULL);
					nw_grass->iGrassID = iTemp;

					ReadFile(fd,&nw_grass->vPos.x,sizeof(FLOAT),&FP,NULL);
					ReadFile(fd,&nw_grass->vPos.y,sizeof(FLOAT),&FP,NULL);
					ReadFile(fd,&nw_grass->vPos.z,sizeof(FLOAT),&FP,NULL);

					ReadFile(fd,&nw_grass->fRotX,sizeof(FLOAT),&FP,NULL);
					ReadFile(fd,&nw_grass->fRotY,sizeof(FLOAT),&FP,NULL);
					ReadFile(fd,&nw_grass->fScale,sizeof(FLOAT),&FP,NULL);

					if( nw_root->DoContainThis( &nw_grass->vPos ) &&
						pThis->InitGrass(nw_grass) )
					{
						nw_grass->next = nw_group->listGrass;
						nw_group->listGrass = nw_grass;

						iGrassCountNew++;
					}
					else
					{
						// 마고자 (2005-11-29 오후 4:12:30) : 
						// 뭔가 빠지면 바로 중단..
						break;
					}
				}

				if( nw_group->iGrassNum != iGrassCountNew )
				{
					MD_SetErrorMessage( "(%d,%d) 섹터에서 풀데이타가 오류가나서 삭제되었습니다. 확인하세요( %d개->%d개 )" ,
						nw_root->six , nw_root->siz , nw_group->iGrassNum , iGrassCountNew );
				}

				nw_group->iGrassNum = iGrassCountNew;

				if( nw_group->iGrassNum > 0 )
				{
					nw_group->next = nw_root->listGrassGroup;
					nw_root->listGrassGroup = nw_group;

					// Draw Order Set
					nw_group->InitVertOrder();
					pThis->SortDrawOrder(nw_group);

					pThis->m_pcsAgcmRender->AddCustomRenderToSector(pThis,&nw_group->BS,AgcmGrass::CB_GRASS_UPDATE,
													AgcmGrass::CB_GRASS_RENDER,(PVOID)nw_group,NULL,AGCM_GRASS_SECTOR_APPEAR_DIST);

					nw_root->iTotalGrassCount += nw_group->iGrassNum;
				}
				else
				{
					// 그룹 삭제..
					group_count--;		// 풀 그룹 개수
				}
			}
			nw_root->iGrassGroupCount = group_count;		// 풀 그룹 개수
		  }
		}
		
		CloseHandle(fd);
	  }
	}

	try
	{
		//pThis->UpdateGrassHeight();
	}
	catch( ... )
	{
		TRACE( "풀 업데이트하다 뻗음..\n" );
		// listGrassGroup이 널임.
	}

	return TRUE;
}

BOOL CPlugin_Grass::OnQuerySaveData		( char * pStr )
{
	#ifdef USE_NEW_GRASS_FORMAT
	strcpy( pStr , "풀데이타" );
	return TRUE;
	#else
	return FALSE;
	#endif
}


BOOL CPlugin_Grass::OnLoadData()				// 맵툴용
{
	#ifdef USE_NEW_GRASS_FORMAT
	// 새 데이타 읽기..
	if( AGCMMAP_THIS->EnumLoadedDivision( CPlugin_Grass::__DivisionLoadCallback , this ) )
	{
		// 합치기..
		// g_pcsAgcmGrass->UnityForSave();

		return TRUE;
	}
	else
	#endif
	{
		// 없는경우..풀을 다 날리고 원래 옥트리 파일을 읽어들인다.
		SetSaveData();
		g_pcsAgcmGrass->RemoveAll();
		return LoadGrassOctreeFile();
	}
}

BOOL CPlugin_Grass::OnSaveData()
{
	#ifdef USE_NEW_GRASS_FORMAT
	char	strSub[ 1024 ];
	GetSubDataDirectory( strSub );

	_CreateDirectory( strSub );
	_CreateDirectory( "%s\\MAP"					, strSub );
	_CreateDirectory( "%s\\MAP\\DATA"			, strSub );
	_CreateDirectory( "%s\\MAP\\DATA\\Object"	, strSub );
	_CreateDirectory( "%s\\MAP\\DATA\\GRASS"	, strSub );

	_CreateDirectory( "MAP\\DATA\\GRASS"		, strSub );

	return AGCMMAP_THIS->EnumLoadedDivision( CPlugin_Grass::__DivisionSaveCallback , this );
	#else
	return TRUE;
	#endif
}

BOOL	CPlugin_Grass::__DivisionLoadCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CPlugin_Grass *pPlugin = ( CPlugin_Grass * ) pData;

	return pPlugin->LoadGrass( pDivisionInfo );
}

BOOL	CPlugin_Grass::__DivisionSaveCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CPlugin_Grass *pPlugin = ( CPlugin_Grass * ) pData;

	BOOL bRet = pPlugin->SaveGrass( pDivisionInfo );

	char	strSub[ 1024 ];
	GetSubDataDirectory( strSub );
	pPlugin->SaveGrass( pDivisionInfo , strSub );

	return bRet;
}

BOOL	CPlugin_Grass::LoadGrass( DivisionInfo * pDivisionInfo )
{
	char	strFilename[ 256 ];

	// 파일 여기서 처리함..
	FILE	* pFile;
	wsprintf( strFilename , GRASS_FILE_NAME , pDivisionInfo->nIndex );
	pFile = fopen( strFilename , "rb" );

	// 풀 포멧..

	// 4 Byte : 풀파일 고유 번호 ( GRASS_FILE_HEADER )
	// 4 Byte : 풀 파일 버젼	 ( GRASS_FILE_VERSION )
	// 4 Byte : 총 풀의 갯수 INT32
	// sizeof stGrassSaveData : 풀데이타.

	if( pFile )
	{
		DWORD	uHeader = GRASS_FILE_HEADER;
		fread( ( void * ) &uHeader , sizeof DWORD , 1 , pFile );

		if( uHeader != GRASS_FILE_HEADER )
		{
			MD_SetErrorMessage( "풀파일이 아님둥" );
			return FALSE;
		}

		DWORD	uVersion = GRASS_FILE_VERSION;
		fread( ( void * ) &uVersion , sizeof DWORD , 1 , pFile );

		if( uVersion != GRASS_FILE_VERSION )
		{
			MD_SetErrorMessage( "풀파일 버젼이 다름둥" );
			return FALSE;
		}

		stGrassSaveData	stGrass;

		// 갯수확인
		INT32 nTotalCount = 0;
		// 갯수 기록..
		fread( ( void * ) &nTotalCount , sizeof INT32 , 1 , pFile );

		for( int i = 0 ; i < nTotalCount ; i ++ )
		{
			fread( ( void * ) &stGrass , sizeof stGrass , 1 , pFile );
			// Add 작업..

			g_pcsAgcmGrass->AddGrass( stGrass.nGrassID , stGrass.fX , stGrass.fZ , stGrass.fXRot , stGrass.fYRot , stGrass.fScale );
		}

		fclose( pFile );
		return TRUE;
	}
	else
	{
		MD_SetErrorMessage( "%s 파일을 쓸수없습니다!" , strFilename );
		return FALSE;
	}

	return TRUE;

}

BOOL	CPlugin_Grass::SaveGrass( DivisionInfo * pDivisionInfo , char * pDestFolder )
{
	//char	strSub[ 1024 ];
	//GetSubDataDirectory( strSub );

	char	strFilename[ 256 ];

	// 파일 여기서 처리함..
	FILE	* pFile;

	wsprintf( strFilename , GRASS_FILE_NAME , pDivisionInfo->nIndex );
	if( pDestFolder )
	{
		char	strFileName2[ 256 ];
		strcpy( strFileName2 , strFilename );
		wsprintf( strFilename , "%s\\%s", pDestFolder , strFileName2 );
	}
	else
		wsprintf( strFilename , GRASS_FILE_NAME , pDivisionInfo->nIndex );

	pFile = fopen( strFilename , "wb" );

	// 풀 포멧..

	// 4 Byte : 풀파일 고유 번호 ( GRASS_FILE_HEADER )
	// 4 Byte : 풀 파일 버젼	 ( GRASS_FILE_VERSION )
	// 4 Byte : 총 풀의 갯수 INT32
	// sizeof stGrassSaveData : 풀데이타.
	DWORD	uHeader = GRASS_FILE_HEADER;
	fwrite( ( void * ) &uHeader , sizeof DWORD , 1 , pFile );
	DWORD	uVersion = GRASS_FILE_VERSION;
	fwrite( ( void * ) &uVersion , sizeof DWORD , 1 , pFile );

	if( pFile )
	{
		ApWorldSector * pSector;
		stGrassSaveData	stGrass;
		int x , z;

		// 갯수확인
		INT32 nTotalCount = 0;

		for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; z ++ )
		{
			for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; x ++ )
			{
				pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );
				if( NULL == pSector ) continue;

				SectorGrassRoot*	pRoot = g_pcsAgcmGrass->GetGrassRoot( pSector );
				if( NULL == pRoot ) continue;

				GrassGroup		*pGroup;
				Grass			*pGrass;
				
				pGroup = pRoot->listGrassGroup;
				while(pGroup)
				{
					pGrass = pGroup->listGrass;
					while(pGrass)
					{
						nTotalCount++;
						pGrass = pGrass->next;
					}
					pGroup = pGroup->next;
				}
			}
		}

		// 갯수 기록..
		fwrite( ( void * ) &nTotalCount , sizeof INT32 , 1 , pFile );

		for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; z ++ )
		{
			for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; x ++ )
			{
				pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );
				if( NULL == pSector ) continue;

				SectorGrassRoot*	pRoot = g_pcsAgcmGrass->GetGrassRoot( pSector );
				if( NULL == pRoot ) continue;

				GrassGroup		*pGroup;
				Grass			*pGrass;
				
				pGroup = pRoot->listGrassGroup;
				while(pGroup)
				{
					pGrass = pGroup->listGrass;
					while(pGrass)
					{
						stGrass.nGrassID	= pGrass->iGrassID	;
						stGrass.fX			= pGrass->vPos.x	;
						stGrass.fZ			= pGrass->vPos.z	;
						stGrass.fXRot		= pGrass->fRotX		;
						stGrass.fYRot		= pGrass->fRotY		;
						stGrass.fScale		= pGrass->fScale	;

						// 각각 저장..
						fwrite( ( void * ) &stGrass , sizeof stGrass , 1 , pFile );

						pGrass = pGrass->next;
					}
					pGroup = pGroup->next;
				}
			}
		}

		fclose( pFile );
		return TRUE;
	}
	else
	{
		MD_SetErrorMessage( "%s 파일을 쓸수없습니다!" , strFilename );
		return FALSE;
	}

	return TRUE;
}

void CPlugin_Grass::OnChangeSectorGeometry	( ApWorldSector * pSector )
{
	SetSaveData();
	// SaveSetChangeObjectList();
	g_pcsAgcmGrass->UpdateGrassHeight(pSector);
}