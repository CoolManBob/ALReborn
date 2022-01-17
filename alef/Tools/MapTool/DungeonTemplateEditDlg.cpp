// DungeonTemplateEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "MyEngine.h"
#include "MainFrm.h"
#include "ApUtil.h"
#include "DungeonTemplateEditDlg.h"
#include "DungeonWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *		g_pMainFrame;
/////////////////////////////////////////////////////////////////////////////
// CDungeonTemplateEditDlg dialog

#define LAST_CONTROL	IDC_DOME1_2


CDungeonTemplateEditDlg::CDungeonTemplateEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDungeonTemplateEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDungeonTemplateEditDlg)
	m_strTemplateName = _T("");
	//}}AFX_DATA_INIT

	for( int i = 0 ; i < 12 ; i ++ )
		m_afHeight[ i ] = 0.0f;

	m_pTemplate	= NULL;
}


void CDungeonTemplateEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDungeonTemplateEditDlg)
	DDX_Control(pDX, IDC_OBJECTLIST, m_ctlTree);
	DDX_Text(pDX, IDC_TEMPLATE_NAME, m_strTemplateName);
	DDX_Text(pDX, IDC_HEIGHT_01, m_afHeight[ 0	] );
	DDX_Text(pDX, IDC_HEIGHT_02, m_afHeight[ 1	] );
	DDX_Text(pDX, IDC_HEIGHT_03, m_afHeight[ 2	] );
	DDX_Text(pDX, IDC_HEIGHT_04, m_afHeight[ 3	] );
	DDX_Text(pDX, IDC_HEIGHT_05, m_afHeight[ 4	] );
	DDX_Text(pDX, IDC_HEIGHT_06, m_afHeight[ 5	] );
	DDX_Text(pDX, IDC_HEIGHT_07, m_afHeight[ 6	] );
	DDX_Text(pDX, IDC_HEIGHT_08, m_afHeight[ 7	] );
	DDX_Text(pDX, IDC_HEIGHT_09, m_afHeight[ 8	] );
	DDX_Text(pDX, IDC_HEIGHT_10, m_afHeight[ 9	] );
	DDX_Text(pDX, IDC_HEIGHT_11, m_afHeight[ 10	] );
	DDX_Text(pDX, IDC_HEIGHT_12, m_afHeight[ 11	] );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDungeonTemplateEditDlg, CDialog)
	//{{AFX_MSG_MAP(CDungeonTemplateEditDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDungeonTemplateEditDlg message handlers

BOOL CDungeonTemplateEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( !LoadCategory() )
	{
		MD_SetErrorMessage( "카테고리파일 로드 실패" );
	}

	CopyTree( &g_pMainFrame->m_pTileList->m_pObjectWnd->m_wndTreeCtrl , TVI_ROOT , 0 );

	UpdateControls();

	m_strTemplateName = m_pTemplate->strName;

	for( int i = 0 ; i < 12 ; i ++ )
	{
		m_afHeight[ i ] = m_pTemplate->afSampleHeight[ i ] / 100.0f;
	}

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDungeonTemplateEditDlg::CopyTree( CTreeCtrl * pTree , HTREEITEM root, int level)
{
	HTREEITEM child;
	stObjectEntry *pObject;

	if( root != TVI_ROOT )
	{
		pObject = (stObjectEntry *) pTree->GetItemData( root );

		ApdObjectTemplate *			pstApdObjectTemplate	= g_pcsApmObject->GetObjectTemplate(pObject->tid);

		if( pstApdObjectTemplate )
		{
			AgcdObjectTemplate *		pstAgcdObjectTemplate	= g_pcsAgcmObject->GetTemplateData(pstApdObjectTemplate);

			HTREEITEM category;
			category = SearchItemText( pstAgcdObjectTemplate->m_szCategory );
			if( !category )
				category = TVI_ROOT;
			
			HTREEITEM item;
			item = m_ctlTree.InsertItem( pObject->name , category);
			m_ctlTree.SetItemData(item, (DWORD) pObject);
		}
	}

	for( child = pTree->GetChildItem( root ) ; child ; child = pTree->GetNextSiblingItem( child ) )
	{
		CopyTree( pTree , child, level + 1 );
	}

	return TRUE;
}

HTREEITEM CDungeonTemplateEditDlg::SearchItemText(const char *strName, HTREEITEM root)
{
	CCategory *pstCategory;
	for( int i = 0 ; i < ( int ) m_listCategory.size(); i ++ )
	{
		pstCategory = &m_listCategory[ i ];
		if( !strcmp( pstCategory->str , strName ) )
		{
			return pstCategory->pos;
		}
	}

	/*
	ASSERT( NULL != strName		);

	HTREEITEM child	;
	HTREEITEM item	;

	if( root != TVI_ROOT && !strcmp( strName, m_ctlTree.GetItemText(root) ) )
		return root;

	for( child = m_ctlTree.GetChildItem( root ) ; child ; child = m_ctlTree.GetNextSiblingItem( child ) )
	{
		item = SearchItemText( strName, child );
		if( item )
			return item;
	}
	*/

	return NULL;
}

BOOL	CDungeonTemplateEditDlg::LoadCategory()
{
	char	filename[ 1024 ];
	wsprintf( filename , "%s\\%s" , ALEF_CURRENT_DIRECTORY , OBJECT_FILE );
	FILE	*pFile = fopen( filename , "rt" );

	ASSERT( NULL != pFile && "오브젝트 스크립트 파일 오픈 실패" );

	if( pFile == NULL )
	{
		// 파일오느 실패.
		return FALSE;
	}

	CGetArg2		arg;

	char			strBuffer[	1024	];
	//unsigned int	nRead = 0;

	int				count = 0;
	int				level;
	HTREEITEM		current[10];
	CCategory		stCategory;

	current[0] = NULL;

	while( fgets( strBuffer , 1024 , pFile ) )
	{
		// 읽었으면..

		arg.SetParam( strBuffer , "|\n" );

		if( arg.GetArgCount() < 2 )
		{
			// 갯수 이상
			continue;
		}

		level				= atoi( arg.GetParam( 0 ) )	;
		if( level >= 10 )
		{
			continue;
		}

		{
			unsigned long tid = 0;
			const char *name = arg.GetParam( 1 );
			const char *file = arg.GetArgCount() > 2 ? arg.GetParam( 2 ):NULL;
			RpClump *pClump = NULL;
			HTREEITEM entry = level ? current[level - 1]:TVI_ROOT;
			RwRGBA *pstColor = NULL;

			stObjectEntry *pObject = new stObjectEntry;
			char strName[256];
			HTREEITEM item;

			ASSERT( NULL != pObject );

			pObject->tid = tid;

			strncpy( pObject->name, name, 256 );
			pObject->name[255] = 0;

			strcpy(strName, pObject->name);

			pObject->file[0] = 0;
			//pObject->pClump = pClump;
			if (pstColor)
				pObject->stPreLitLum = *pstColor;

			if( file )
			{
				strncpy( pObject->file, file, 256 );
				pObject->file[255] = 0;

				strcat(strName, " ( ");
				strcat(strName, pObject->file);
				strcat(strName, " )");
			}

			item = m_ctlTree.InsertItem(strName, entry);
			m_ctlTree.SetItemData(item, (DWORD) pObject);

			current[ level ] = item;

			// 마고자 (2005-04-11 오후 4:31:36) : 카테고리 저장해둠..
			stCategory.str	= strName;
			stCategory.pos	= current[ level ];
			m_listCategory.push_back( stCategory );

			m_listDelete.AddTail( ( void * ) pObject );
		}

		count ++;
	}

	fclose( pFile );

	return TRUE;
}

// 창띄우기.. 템플릿을 인자로 받음..
int		CDungeonTemplateEditDlg::DoModal( CDungeonTemplate * pTemplate )
{
	ASSERT( NULL != pTemplate );

	if( NULL == pTemplate )
		return IDCANCEL;

	// 템플릿 포인터 설정
	this->m_pTemplate = pTemplate;

	return CDialog::DoModal();
}

void CDungeonTemplateEditDlg::OnOK() 
{
	UpdateData( TRUE );
	m_pTemplate->strName	= m_strTemplateName;

	// 높이 소팅..
	float	afHeightTmp[ 12 ];
	float	fTmp;
	int i , j , nIndex ;

	for( i = 0 ; i < 12 ; i ++ )
	{
		afHeightTmp[ i ] = m_afHeight[ i ];
	}

	for( i = 0 ; i < 12 ; i ++ )
	{
		fTmp	= afHeightTmp[ 0 ]	; 
		nIndex	= i					;

		for( j = 1 ; j < 12 - i ; j ++ )
		{
			if( fTmp < afHeightTmp[ j ] )
			{
				fTmp	= afHeightTmp[ j ] ;
				nIndex	= j;
			}
		}
	
		m_afHeight[ i ] = fTmp;
		afHeightTmp[ nIndex ] = afHeightTmp[ 12 - i - 1 ];
	}

	for( i = 0 ; i < 12 ; i ++ )
	{
		m_pTemplate->afSampleHeight[ i ] = m_afHeight[ i ] * 100.0f;
	}
	
	CDialog::OnOK();
}

LRESULT CDungeonTemplateEditDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	int	nID = LOWORD(wParam);

	if( WM_COMMAND == message			&&
		BN_CLICKED == HIWORD(wParam)	&&
		LOWORD(wParam) >= IDC_FLOOR_0	&&
		LAST_CONTROL >= LOWORD(wParam)	)
	{
		// 버튼 눌러졌을때..

		// 어느 타입인지 얻어냄..
		int nType = ( nID - IDC_FLOOR_0 ) / MAX_DUNGEON_INDEX_COUNT;

		ASSERT( nType < TBM_SELECT );
		
		// 선택된 컨트롤 확인..
		
		stObjectEntry	*pObject;
		ApdObjectTemplate	*	pstApdObjectTemplate	;
		HTREEITEM		item;

		item = m_ctlTree.GetSelectedItem();
		if( !item )
		{
			// 선택된 아이템이 없으면 해당 아이템 삭제..
			int i;
			for( i = 0 ; i < MAX_DUNGEON_INDEX_COUNT ; ++ i )
			{
				if( this->m_pTemplate->uTemplateIndex[ nType ][ i ].uIndex == 0 )
				{
					break;
				}
			}

			if( i > 0 )
				this->m_pTemplate->uTemplateIndex[ nType ][ i - 1 ].uIndex = 0;
		}
		else
		{
			// 추가 가 되는데...

			pObject = (stObjectEntry *) m_ctlTree.GetItemData( item );
			ASSERT( NULL != pObject );
			pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate(pObject->tid);

			if( NULL == pstApdObjectTemplate )
			{
				MessageBox( "해당 아이템에 템플릿 정보가 없어용" );
			}
			else
			{
				// 해당 타입에 겸치는게 있는지 검사..

				BOOL	bJungBok = FALSE;
				int		i;

				for( i = 0 ; i < MAX_DUNGEON_INDEX_COUNT ; ++ i )
				{
					if( pstApdObjectTemplate->m_lID == this->m_pTemplate->uTemplateIndex[ nType ][ i ].uIndex )
					{
						bJungBok = TRUE;
						break;
					}
				}

				if( !bJungBok )
				{
					// 중복이 아닌경우에만 추가함..
					
					// 빈칸이 있는 경우 거기에 넣음..
					for( i = 0 ; i < MAX_DUNGEON_INDEX_COUNT ; ++ i )
					{
						if( this->m_pTemplate->uTemplateIndex[ nType ][ i ].uIndex == 0 )
						{
							// 삽입!
							this->m_pTemplate->uTemplateIndex[ nType ][ i ].uIndex = pstApdObjectTemplate->m_lID;
							break;
						}
					}

					if( i == MAX_DUNGEON_INDEX_COUNT )
					{
						// 넣기 실패..
						MessageBox( "인덱스는 4개까지 들어가용." );
					}
				}
			}
		}

		// 컨트롤 업데이트..
		UpdateControls();

		m_ctlTree.SelectItem( NULL );
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}

void	CDungeonTemplateEditDlg::UpdateControls()
{
	CONTROL_UPDATE( IDC_FLOOR_0 );
	CONTROL_UPDATE( IDC_FLOOR_1 );
	CONTROL_UPDATE( IDC_FLOOR_2 );
	CONTROL_UPDATE( IDC_SLOPE_0 );
	CONTROL_UPDATE( IDC_SLOPE_1 );
	CONTROL_UPDATE( IDC_SLOPE_2 );
	CONTROL_UPDATE( IDC_STAIR_0 );
	CONTROL_UPDATE( IDC_STAIR_1 );
	CONTROL_UPDATE( IDC_STAIR_2 );
	CONTROL_UPDATE( IDC_WALL2_0	);
	CONTROL_UPDATE( IDC_WALL2_1	);
	CONTROL_UPDATE( IDC_WALL2_2	);
	CONTROL_UPDATE( IDC_WALL4_0	);
	CONTROL_UPDATE( IDC_WALL4_1	);
	CONTROL_UPDATE( IDC_WALL4_2 );
	CONTROL_UPDATE( IDC_FENCE_0	);
	CONTROL_UPDATE( IDC_FENCE_1 );
	CONTROL_UPDATE( IDC_FENCE_2 );
	CONTROL_UPDATE( IDC_PILLAR_0);
	CONTROL_UPDATE( IDC_PILLAR_1);
	CONTROL_UPDATE( IDC_PILLAR_2);

	CONTROL_UPDATE( IDC_DOME4_0	);
	CONTROL_UPDATE( IDC_DOME4_1	);
	CONTROL_UPDATE( IDC_DOME4_2	);

	CONTROL_UPDATE( IDC_DOME1_0	);
	CONTROL_UPDATE( IDC_DOME1_1	);
	CONTROL_UPDATE( IDC_DOME1_2	);

}

void	CDungeonTemplateEditDlg::CONTROL_UPDATE( UINT32 uID )
{
	CButton	* pButton;
	CString	str;
	int		nTemplate;
	ApdObjectTemplate	*	pstApdObjectTemplate	;

	pButton		= ( CButton * ) GetDlgItem( uID );
	nTemplate	= this->m_pTemplate->uTemplateIndex[ ( uID - IDC_FLOOR_0 ) / MAX_DUNGEON_INDEX_COUNT ][ ( uID - IDC_FLOOR_0 ) % MAX_DUNGEON_INDEX_COUNT ].uIndex;
	pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate( nTemplate );
	if( nTemplate == 0 ) str = "빈자리";
	else
		if( NULL == pstApdObjectTemplate )
		{
			MessageBox( "템플릿에 오류가 있어요." );
			str = "오류!";
		}
		else
		{
			str = pstApdObjectTemplate->m_szName;
		}
	pButton->SetWindowText( str );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDungeonTemplateSelectDlg dialog


CDungeonTemplateSelectDlg::CDungeonTemplateSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDungeonTemplateSelectDlg::IDD, pParent),
	m_bListUpdated( FALSE ),
	m_nTemplateID( 0 )
{
	//{{AFX_DATA_INIT(CDungeonTemplateSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pDungeonWnd	= g_pMainFrame->m_pDungeonWnd;
}


void CDungeonTemplateSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDungeonTemplateSelectDlg)
	DDX_Control(pDX, IDC_DUNGEONTEMPLATELIST, m_ctlTemplateList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDungeonTemplateSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CDungeonTemplateSelectDlg)
	ON_BN_CLICKED(IDC_EDITTEMPLATE, OnEdittemplate)
	ON_BN_CLICKED(IDC_ADDTEMPLATE, OnAddtemplate)
	ON_BN_CLICKED(IDC_DELETE_TEMPLATE, OnDeleteTemplate)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDungeonTemplateSelectDlg message handlers

void CDungeonTemplateSelectDlg::OnEdittemplate() 
{
	int	nIndex = m_ctlTemplateList.GetCurSel();
	if( LB_ERR == nIndex )
	{
		MessageBox( "아무것도 선택된게 없스염.." );
		return;
	}

	CDungeonTemplateEditDlg dlg;
	CDungeonTemplate	* pTemplate;

	pTemplate = ( CDungeonTemplate * ) m_ctlTemplateList.GetItemDataPtr( nIndex );

	if( IDOK == dlg.DoModal( pTemplate ) )
	{
		//////////////////////////////////////////////////////////////////////////
		// 따로 처리할건 없다..
		// 안에서 포인터 데이타를 바꾸기 때문에..
		// 리스트 변화 플래그만 체크해둔다.

		SetListUpdate();
	}
}

void CDungeonTemplateSelectDlg::OnAddtemplate() 
{
	// 템플릿 추가..
	CDungeonTemplate	* pTemplate;
	CDungeonTemplateEditDlg dlg;

	// 빈 템플릿 인덱스 얻기..
	int	nLastIndex = 0;
	int nCount = m_ctlTemplateList.GetCount();
	for( int i = 0 ; i < nCount ; ++ i )
	{
		pTemplate = ( CDungeonTemplate * ) m_ctlTemplateList.GetItemDataPtr( i );
		ASSERT( NULL != pTemplate );

		if( ( INT32 ) pTemplate->uTID > nLastIndex )
		{
			nLastIndex = pTemplate->uTID;
		}
	}	

	pTemplate		= new CDungeonTemplate;
	pTemplate->uTID	= nLastIndex + 1 ;	// 마지막 인덱스에 1을 더해서 넣어서 중복방지..

	if( IDOK == dlg.DoModal( pTemplate ) )
	{
		// 추가함..
		int nIndex = AddTemplate( pTemplate );
		ASSERT( nIndex != LB_ERR );

		m_ctlTemplateList.SetCurSel( nIndex );
		SetListUpdate();
	}
	else
	{
		// 취소 됐으니까.. 그냥 없에버림..
		delete pTemplate;
	}
}

int	CDungeonTemplateSelectDlg::AddTemplate( CDungeonTemplate * pTemplate )
{
	ASSERT( NULL != pTemplate );
	if( NULL == pTemplate ) return LB_ERR;

	// 하핫 에디트 박스에 등록함..

	int nIndex;
	nIndex = m_ctlTemplateList.AddString( pTemplate->strName );

	if( nIndex == LB_ERR )
	{
		MessageBox( "메시지박스 에드 실패" );
		return LB_ERR;
	}

	VERIFY( m_ctlTemplateList.SetItemDataPtr( nIndex , pTemplate ) );
	return nIndex;
}

void CDungeonTemplateSelectDlg::OnDeleteTemplate() 
{
	int	nIndex = m_ctlTemplateList.GetCurSel();
	if( LB_ERR == nIndex )
	{
		MessageBox( "아무것도 선택된게 없스염.." );
		return;
	}

	// 선택된 템플릿이 있으면 ..
	CDungeonTemplate	* pTemplate;
	pTemplate = ( CDungeonTemplate * ) m_ctlTemplateList.GetItemDataPtr( nIndex );

	if( IDYES == MessageBox( "정말 지울거예요?" , "템플릿 삭제 확인.." , MB_YESNO ) )
	{
		delete pTemplate;
		m_ctlTemplateList.DeleteString( nIndex );

		// 삭제 작업..
		SetListUpdate();
	}
	
}

BOOL CDungeonTemplateSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT( NULL != m_pDungeonWnd );
	if( NULL == m_pDungeonWnd )
		return FALSE;

	AuList< CDungeonTemplate > * pList = &m_pDungeonWnd->m_listTemplate	;
	AuNode< CDungeonTemplate > * pNode = pList->GetHeadNode()			;
	CDungeonTemplate * pTemplate;
	CDungeonTemplate * pTemplateCopy;
	INT32	nIndex;

	while( pNode )
	{
		pTemplate	= &pNode->GetData();

		pTemplateCopy = new CDungeonTemplate;

		// 복사..
		pTemplateCopy->copy( *pTemplate );

		// 템플릿을 리스트에 추가함.

		nIndex = m_ctlTemplateList.AddString( pTemplateCopy->strName );
		m_ctlTemplateList.SetItemDataPtr( nIndex , ( void * ) pTemplateCopy );

		pNode = pNode->GetNextNode();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDungeonTemplateSelectDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	// 마고자 (2004-10-07 오후 12:58:26) : 
	// 메모리 정리 작업..

	int nCount = m_ctlTemplateList.GetCount();
	for( int i = 0 ; i < nCount ; ++ i )
	{
		delete m_ctlTemplateList.GetItemDataPtr( i );
	}	
}

void CDungeonTemplateSelectDlg::OnOK() 
{
	// 선택 된녀석이 있는지 확인한다..
	int	nIndex = m_ctlTemplateList.GetCurSel();
	if( LB_ERR == nIndex )
	{
		MessageBox( "아무것도 선택된게 없스염.." );
		return;
	}

	// 선택된 템플릿이 있으면 ..
	CDungeonTemplate	* pTemplate;
	pTemplate = ( CDungeonTemplate * ) m_ctlTemplateList.GetItemDataPtr( nIndex );
	m_nTemplateID	= pTemplate->uTID;

	if( GetListUpdated() )
	{
		// 리스트 업데이트..

		ASSERT( NULL != g_pMainFrame->m_pDungeonWnd );

		g_pMainFrame->m_pDungeonWnd->m_listTemplate.RemoveAll();

		int nCount = m_ctlTemplateList.GetCount();
		for( int i = 0 ; i < nCount ; ++ i )
		{
			g_pMainFrame->m_pDungeonWnd->m_listTemplate.AddTail( 
				* ( ( CDungeonTemplate * ) m_ctlTemplateList.GetItemDataPtr( i ) ) );
		}
	}
	
	CDialog::OnOK();
}

void CDungeonTemplateEditDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// 마고자 (2004-10-08 오전 11:30:31) : 
	// 메모리 클린업..

	AuNode< void * > * pNode = m_listDelete.GetHeadNode();
	while( pNode )
	{
		delete pNode->GetData();

		pNode = pNode->GetNextNode();
	}
}
