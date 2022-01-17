// RTMenuRegion.cpp : implementation file
//

#include "stdafx.h"
#include "regiontool.h"
#include "RegionToolDlg.h"
#include "RegionMenuDlg.h"
#include "RTMenuRegion.h"

#include "TemplateEditDlg.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString	g_strLastLockOwnerName;

static const char g_strINIRegionTemplateSection		[]	= "%d"			;
static const char g_strINIRegionTemplateKeyName		[]	= "Name"		;
static const char g_strINIRegionTemplateKeyPriority	[]	= "Priority"	;
static const char g_strINIRegionTemplateKeyType		[]	= "Type"		;
static const char g_strINIRegionTemplateKeyComment	[]	= "Comment"		;

static const char g_strINIRegionElementSection		[]	= "%d"			;
static const char g_strINIRegionElementIndex		[]	= "Index"		;
static const char g_strINIRegionElementKeyKind		[]	= "Kind"		;
static const char g_strINIRegionElementKeyStartX	[]	= "StartX"		;
static const char g_strINIRegionElementKeyStartZ	[]	= "StartZ"		;
static const char g_strINIRegionElementKeyEndX		[]	= "EndX"		;
static const char g_strINIRegionElementKeyEndZ		[]	= "EndZ"		;

/////////////////////////////////////////////////////////////////////////////
// CRTMenuRegion

char *	CRTMenuRegion::GetMenuName()
{
	static char _strName[] = "Region";
	return ( char * ) _strName;
}


CRTMenuRegion::CRTMenuRegion()
{
	m_nCurrentTemplate	= REGION_NO_TEMPLATE	;
	m_pSelectedElement	= NULL	;
	m_nRangeDragMode	= MODE_NONE	;

	m_bChangedRegion	= FALSE;
	m_bChangedTemplate	= FALSE;
}

CRTMenuRegion::~CRTMenuRegion()
{
}

BOOL CRTMenuRegion::CBProgress( char * pStr , int nPos , int nMax , void * pData )
{
	CProgressDlg	*pDlg = ( CProgressDlg * ) pData;

	pDlg->SetProgress	( nPos );
	pDlg->SetTarget		( nMax );

	return TRUE;
}

BOOL CRTMenuRegion::OnLoadData		()
{
	g_pcsApmMap->LoadTemplate( "Ini\\" REGIONTEMPLATEFILE );

	UpdateList();

	// 마고자 (2005-06-07 오후 5:19:53) : 
	// 텔레포트 전용이면 컴팩트 데이타를 로딩함.
	if( g_bTeleportOnlyMode )
	{
		// 컴팩트 데이타 로딩.
		g_pcsApmMap->SetLoadingMode( TRUE , FALSE );
		g_pcsApmMap->Init();	// 데이타 초기화.

		CProgressDlg	dlg;
		dlg.StartProgress( "컴팩트 데이타 로딩중" , 100 , this );
		g_pcsApmMap->LoadAll( TRUE , FALSE , CBProgress , this );
		dlg.EndProgress();
	}
	else
	{
		// 리젼 정보 로딩..
		Load( "RegionTool\\" REGIONFILE );
	}

	m_bChangedTemplate	= FALSE;

	// 월드맵 인포
	g_pcsApmMap->LoadWorldMap( "Ini\\" WORLDMAPTEMPLATE );

	if( g_pcsAgpmItem && !g_pcsAgpmItem->StreamReadTemplate("Ini\\ItemTemplate.ini") )
	{
		::MessageBox( NULL , "아이템 템플릿 읽기에 실패했습니다. 템플릿이 제대로 로드돼지 않았을 수 있습니다." , "맵툴" , MB_ICONERROR | MB_OK );
	}

	UpdateList();

	return TRUE;
}

BOOL CRTMenuRegion::OnSaveData		()
{
	if( m_bChangedTemplate )
	{
		g_pcsApmMap->SaveTemplate( "Ini\\" REGIONTEMPLATEFILE );
		m_bChangedTemplate = FALSE;
	}

	Save( "RegionTool\\" REGIONFILE );

	return TRUE;
}

BOOL	CRTMenuRegion::Create( CRect * pRect , CWnd * pParent )
{
	return CWnd::Create( NULL , NULL , WS_CHILD | WS_VISIBLE , *pRect , pParent , 1346 );
}


BEGIN_MESSAGE_MAP(CRTMenuRegion, CWnd)
	//{{AFX_MSG_MAP(CRTMenuRegion)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRTMenuRegion message handlers

void CRTMenuRegion::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );
		
	// Do not call CWnd::OnPaint() for painting messages
}

int CRTMenuRegion::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect	rect;
	GetClientRect( rect );

	CRect	rectList , rectButton;
	const int nButtonHeight = 20;

	rectList = rect;
	rectList.bottom -= ( nButtonHeight + 3 ) * 4 ;

	m_ctlTemplate.Create( WS_CHILD | WS_VISIBLE | LBS_SORT | WS_VSCROLL , rectList , this , IDC_MENUREGION_LIST );

	rectButton = rect;

	rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 0;
	rectButton.bottom	= rectButton.top + nButtonHeight;
	m_wndButtonEdit.Create( "Edit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rectButton , this,IDC_MENUREGION_BUTTON_EDIT);
	
	rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 1;
	rectButton.bottom	= rectButton.top + nButtonHeight;
	m_wndButtonAdd.Create( "Add", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rectButton , this,IDC_MENUREGION_BUTTON_ADD);
	
	rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 2;
	rectButton.bottom	= rectButton.top + nButtonHeight;
	m_wndButtonRemove.Create( "Remove", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rectButton , this,IDC_MENUREGION_BUTTON_REMOVE);
	
	rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 3;
	rectButton.bottom	= rectButton.top + nButtonHeight;
	m_wndButtonDelete.Create( "Delete Region", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rectButton , this,IDC_MENUREGION_BUTTON_DELETE);
	
	// 데이타설정..

	UpdateList();
	
	return 0;
}

LRESULT CRTMenuRegion::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	#define	CW_CLICKED_BUTTON( uID )	if( uID == LOWORD( wParam ) && BN_CLICKED == HIWORD( wParam ) )

	if( WM_COMMAND == message )
	{
		CW_CLICKED_BUTTON( IDC_MENUREGION_BUTTON_EDIT )
		{
			if( LockTemplate() )
			{
				// 선택 되어있는지 확인..
				OnSelchangeTemplate();

				if( REGION_NO_TEMPLATE == GetCurrentTemplate() )
				{
					MessageBox( "템플릿 선택하고 눌러요!" );
				}
				else
				{
					ApmMap::RegionTemplate * pTemplate = g_pcsApmMap->GetTemplate( GetCurrentTemplate() );

					ASSERT( NULL != pTemplate );

					CTemplateEditDlg	dlg;
					dlg.SetTemplate( pTemplate );
					
					if( IDOK == dlg.DoModal() )
					{
						m_bChangedTemplate	= TRUE;
				
						UpdateList();
					}
				}
			}
			else
			{
				MessageBox( "RegionTemplate.ini 파일을 누가 물고있어요!" );
			}
		}
		CW_CLICKED_BUTTON( IDC_MENUREGION_BUTTON_ADD )
		{
			if( LockTemplate() )
			{
				CTemplateEditDlg	dlg;

				// 빈 템플릿 인덱스 생성..
				int nLast = 0;
				int i;
				for( i = 0 ; i < 256 ; ++ i )
				{
					if( NULL == g_pcsApmMap->GetTemplate( i ) )
					{
						nLast = i;
						break;
					}
				}

				if( i == 256 )
				{
					MessageBox( "템플릿이 꽉차서 더이상 넣을 수 없어요. 마고자와 상의하세요 -_-;;" );
				}
				else
				{

					ApmMap::RegionTemplate	stTemplate;
					stTemplate.nIndex = nLast;
					dlg.SetTemplate( &stTemplate );

					if( IDOK == dlg.DoModal() )
					{
						// 템플릿 추가..		
						while( !g_pcsApmMap->AddTemplate( & stTemplate ) )
						{
							MessageBox( "데이타가 오류가 있어여 잘고쳐봐요( 특히 인덱스 )" );

							dlg.SetTemplate( &stTemplate );
							if( IDOK == dlg.DoModal() )
							{

							}
							else
							{
								break;
							}
						}

						UpdateList();
						m_bChangedTemplate = TRUE;
					}
				}
			}
			else
			{
				MessageBox( "RegionTemplate.ini 파일을 누가 물고있어요!" );
			}

		}
		CW_CLICKED_BUTTON( IDC_MENUREGION_BUTTON_REMOVE )
		{
			if( LockTemplate() )
			{
				OnSelchangeTemplate();
				
				if( REGION_NO_TEMPLATE == GetCurrentTemplate() )
				{
					MessageBox( "템플릿 선택하고 눌러요!" );
				}
				else
				{
					ApmMap::RegionTemplate * pTemplate = g_pcsApmMap->GetTemplate( GetCurrentTemplate() );

					ASSERT( NULL != pTemplate );

					CString	str;
					str.Format( "리젼 템플릿'%s' 를 정말 지울래요?" , pTemplate->pStrName );
					if( IDYES == MessageBox( str , "리젼툴" , MB_YESNOCANCEL ) )
					{
						g_pcsApmMap->RemoveTempate( pTemplate->nIndex );
						UpdateList();
						m_bChangedTemplate = TRUE;
					}
				}
			}
			else
			{
				MessageBox( "RegionTemplate.ini 파일을 누가 물고있어요!" );
			}
		}
		CW_CLICKED_BUTTON( IDC_MENUREGION_BUTTON_DELETE )
		{
			OnDeleteKeyDown();			
		}
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

void	CRTMenuRegion::UpdateList()
{
	m_ctlTemplate.ResetContent();

	AuNode< ApmMap::RegionTemplate > * pNode = g_pcsApmMap->m_listTemplate.GetHeadNode();
	ApmMap::RegionTemplate * pTemplateInList;

	CString	str;

	while( pNode )
	{
		pTemplateInList	= & pNode->GetData();

		str.Format( SKY_TEMPLATE_FORMAT , pTemplateInList->nIndex , pTemplateInList->pStrName );

		m_ctlTemplate.AddString( str );

		pNode = pNode->GetNextNode();
	}
}

void CRTMenuRegion::OnSelchangeTemplate() 
{
	// TODO: Add your control notification handler code here
	int nSelection = m_ctlTemplate.GetCurSel();

	if( nSelection == LB_ERR ) return;

	CString	str;

	m_ctlTemplate.GetText( nSelection , str );

	int	nIndex = atoi( (LPCTSTR) str );

	SetCurrentTemplate( nIndex );
}

BOOL CRTMenuRegion::OnDeleteKeyDown()
{
	RemoveSelected();
	return TRUE;
}

void CRTMenuRegion::OnPaintOnMap		( CDC * pDC )
{

	BOOL	bControlState = FALSE;
	if( GetAsyncKeyState( VK_CONTROL ) < 0 )
		bControlState = TRUE;

	CRect	rect , rectMap;
	this->m_pParent->m_pMainDlg->m_ctlRegion.GetClientRect( rect );

	if( g_bTeleportOnlyMode )
	{

	}
	else
	{
		// 범위 표시..
		AuNode< ApmMap::RegionElement > * pNode = m_listElement.GetHeadNode();
		ApmMap::RegionElement * pElement;

		CPen	penRegion;
		penRegion.CreatePen( PS_SOLID , 2 , RGB( 87 , 25 , 255 )	);
		pDC->SelectObject( penRegion );
		pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );

		pDC->SetBkColor( RGB( 255 , 255 , 255 ) );
		while( pNode )
		{
			pElement	= & pNode->GetData();

			//if( pElement->nIndex < MAX_COLOR_SAMPLE )
			{
				penRegion.DeleteObject();
				penRegion.CreatePen( PS_SOLID , 2 , __GetColor( pElement->nIndex ) 	);
				pDC->SelectObject( penRegion );
			}

			/*else
			{
				penRegion.DeleteObject();
				penRegion.CreatePen( PS_SOLID , 2 , RGB( 87 , 25 , 255 )	);
				pDC->SelectObject( penRegion );
			}
			*/

			rectMap.SetRect(
				( INT32 ) ( pElement->nStartX * m_fScale) + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX,
				( INT32 ) ( pElement->nStartZ * m_fScale) + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ,
				( INT32 ) ( pElement->nEndX * m_fScale	) + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX ,
				( INT32 ) ( pElement->nEndZ * m_fScale	) + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ);

			if ( rect.right > rectMap.left && rectMap.right > rect.left)
			{
				if (rect.bottom > rectMap.top && rectMap.bottom > rect.top )
				{
					// 화면에 포함된다..
					// 

					if( bControlState )
					{
						pDC->FillSolidRect( rectMap , __GetColor( pElement->nIndex ) );
					}
					else
					{
						if( pNode == m_pSelectedElement )
							pDC->FillSolidRect( rectMap , RGB( 255 , 255 , 255 ) );
						else
							pDC->Rectangle( rectMap );
					}

					CString	str;
					
					ApmMap::RegionTemplate * pTemplate = g_pcsApmMap->GetTemplate( pElement->nIndex );
					if( pTemplate )
					{
						str.Format( "%d , %s" , pTemplate->nIndex , pTemplate->pStrName );
						pDC->DrawText( str , rectMap , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
					}
				}
			}

			pNode = pNode->GetNextNode();
		}


		// Selection 계산용..
		if( m_nRangeDragMode == MODE_RANGESELECTED )
		{
			CPen	penSelection;
			penSelection.CreatePen( PS_SOLID , 2 , RGB( 255, 128 , 128 )	);
			pDC->SelectObject( penSelection );
			pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );

			pDC->Rectangle(
				m_SelectedPosX1 + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX ,
				m_SelectedPosZ1 + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ ,
				m_SelectedPosX2 + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX ,
				m_SelectedPosZ2 + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ );
		}
	}
}

BOOL CRTMenuRegion::OnLButtonDownMap	( CPoint &pointOriginal , int x , int z )
{
	OnSelchangeTemplate();
	
	AuNode< ApmMap::RegionElement > * pNode = GetRegion( x , z , GetCurrentTemplate() );

	if( LockRegion() )
	{
		if( pNode )
		{
			// 선택..
			m_pSelectedElement	= pNode;

			// 드래그모드..
			int nOffsetX , nOffsetZ;

			nOffsetX = x;
			nOffsetZ = z;
			m_nRangeDragMode = MODE_REGIONMOVE;
			m_pointLastPress	= pointOriginal ;
		
			m_pParent->InvalidateRegionView();
		}
		else
		{
			if( GetCurrentTemplate() != REGION_NO_TEMPLATE )
			{
				m_pParent->SetCaptureRegionView();
			
				int nOffsetX , nOffsetZ;

				nOffsetX = x;
				nOffsetZ = z;

				// 드래그모드..
				m_nRangeDragMode = MODE_RANGESELECTED;
				m_SelectedPosX1	 = m_SelectedPosX2 = nOffsetX;
				m_SelectedPosZ1	 = m_SelectedPosZ2 = nOffsetZ;
				m_pParent->InvalidateRegionView();
			}
			else
			{
				// do nothing..
			}
		}
	}
	else
	{
		CString	str;
		str.Format( "'%s'아저씨가 Region.ini 를 물고있어요" , g_strLastLockOwnerName );
		MessageBox( str );
	}

	return TRUE;
}

BOOL CRTMenuRegion::OnLButtonUpMap		( CPoint &pointOriginal , int x , int z )
{
	m_pParent->ReleaseCaptureRegionView();

	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		// 마고자 (2004-09-10 오후 6:04:32) : 
		// 버튼을 뗄때 처리하는 녀석들..
		int nX1 , nX2 , nZ1 , nZ2;

		// 범위 표시줄..
		if( m_SelectedPosX1 < m_SelectedPosX2 )
		{
			nX1 = m_SelectedPosX1;
			nX2 = m_SelectedPosX2;
		}
		else									
		{
			nX1 = m_SelectedPosX2;
			nX2 = m_SelectedPosX1;
		}
		
		if( m_SelectedPosZ1 < m_SelectedPosZ2 )
		{
			nZ1 = m_SelectedPosZ1;
			nZ2 = m_SelectedPosZ2;
		}
		else									
		{
			nZ1 = m_SelectedPosZ2;
			nZ2 = m_SelectedPosZ1;
		}

		// 범위추가..
		if( GetCurrentTemplate() != REGION_NO_TEMPLATE )
		{
			if( LockRegion() )
			{
				AddRegion( GetCurrentTemplate() , 
					( INT32 ) ( nX1 / m_fScale ), 
					( INT32 ) ( nZ1 / m_fScale ), 
					( INT32 ) ( nX2 / m_fScale ), 
					( INT32 ) ( nZ2 / m_fScale ) );
				m_pParent->InvalidateRegionView();
			}
			else
			{
				CString	str;
				str.Format( "'%s'아저씨가 Region.ini 를 물고있어요" , g_strLastLockOwnerName );
				MessageBox( str );
				return FALSE;
			}
		}
		else
		{
			// .
			// 추가돼지 않음..

			MessageBox( "템플릿에러~." );
		}

		m_nRangeDragMode = MODE_NONE;
	}

	if( m_nRangeDragMode == MODE_REGIONMOVE)
	{
		m_nRangeDragMode = MODE_NONE;	

		// 해당 엘리먼트를 리스트 끝으로 이동..
		if( LockRegion() )
		{
			if( m_pSelectedElement )
			{
				ApmMap::RegionElement stElement = m_pSelectedElement->GetData();
				m_listElement.RemoveNode( m_pSelectedElement );
				m_listElement.AddTail( stElement );
				m_pSelectedElement = m_listElement.GetTailNode();
				m_pParent->InvalidateRegionView();
			}
		}
	}

	return TRUE;
}

BOOL CRTMenuRegion::OnMouseMoveMap		( CPoint &pointOriginal , int x , int z )
{
	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		// 범위 조절중..
		m_SelectedPosX2 = x;
		m_SelectedPosZ2 = z;
		m_pParent->InvalidateRegionView();
	}
	
	if( m_nRangeDragMode == MODE_REGIONMOVE )
	{
		// 범위 조절중..
		int	dx , dz;

		dx	= m_pointLastPress.x - pointOriginal.x ;
		dz	= m_pointLastPress.y - pointOriginal.y ;

		dx = ( INT32 ) ( dx / m_fScale );
		dz = ( INT32 ) ( dz / m_fScale );

		m_pointLastPress = pointOriginal;
		
		if( m_pSelectedElement )
		{
			ApmMap::RegionElement * pElement= & m_pSelectedElement->GetData();
			pElement->nStartX	-= dx;
			pElement->nStartZ	-= dz;
			pElement->nEndX		-= dx;
			pElement->nEndZ		-= dz;
		}

		m_pParent->InvalidateRegionView();
	}

	return TRUE;
}

BOOL	CRTMenuRegion::SetCurrentTemplate( int nIndex )
{
	if( !g_pcsApmMap->GetTemplate( nIndex ) )		return FALSE;
		
	m_nCurrentTemplate = nIndex;
	return TRUE;
}

BOOL						CRTMenuRegion::AddRegion( int nIndex , INT32 nStartX , INT32 nStartZ , INT32 nEndX , INT32 nEndZ , INT32 nKind )
{
	ApmMap::RegionElement	element;

	element.nIndex	= nIndex	;
	element.nStartX	= nStartX	;
	element.nStartZ	= nStartZ	;
	element.nEndX	= nEndX		;
	element.nEndZ	= nEndZ		;
	element.nKind	= nKind		;

	// 쏘팅해서 넣어야하는데..
	// 끄으응...

	m_listElement.AddTail( element );
	return TRUE;
}

AuNode< ApmMap::RegionElement > *	CRTMenuRegion::GetRegion( INT32 nOffsetX , INT32 nOffsetZ , INT32 nSelectedTemplate )
{
	AuNode< ApmMap::RegionElement > * pNode = m_listElement.GetHeadNode();
	ApmMap::RegionElement * pElement;

	nOffsetX = ( INT32 ) ( ( nOffsetX ) / m_fScale );
	nOffsetZ = ( INT32 ) ( ( nOffsetZ ) / m_fScale );

	while( pNode )
	{
		pElement	= & pNode->GetData();

		if( pElement->nStartX <= nOffsetX && nOffsetX <= pElement->nEndX	&&
			pElement->nStartZ <= nOffsetZ && nOffsetZ <= pElement->nEndZ	&&
			nSelectedTemplate == pElement->nIndex )
		{
			return pNode;
		}

		pNode = pNode->GetNextNode();
	}

	return NULL;
	// 그런거 없샤..
}

BOOL	CRTMenuRegion::RemoveRegion( AuNode< ApmMap::RegionElement > * pNode )
{
	ASSERT( NULL != pNode );

	if( NULL == pNode ) return FALSE;

	if( LockRegion() )
	{
		m_listElement.RemoveNode( pNode );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void	CRTMenuRegion::RemoveSelected()
{
	if( m_pSelectedElement )
	{
		RemoveRegion( m_pSelectedElement );
		m_pSelectedElement = NULL;

		m_pParent->InvalidateRegionView();
	}
}

BOOL	CRTMenuRegion::Save( char * pFileName , BOOL bEncryption )
{
	if( m_bChangedRegion )
	{
		AuIniManagerA	iniManager;
		iniManager.SetPath( pFileName );

		ApmMap::RegionElement * pElement;
		AuNode< ApmMap::RegionElement >	* pNode				= m_listElement.GetHeadNode();
		char	strSection [ 256 ];

		int	nSeq = 0;

		while( pNode )
		{
			pElement	= & pNode->GetData();

			wsprintf( strSection , g_strINIRegionElementSection , nSeq++ );

			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementIndex		, pElement->nIndex	);
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyKind		, pElement->nKind	);
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyStartX	, pElement->nStartX	);
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyStartZ	, pElement->nStartZ	);
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyEndX		, pElement->nEndX	);
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyEndZ		, pElement->nEndZ	);

			pNode = pNode->GetNextNode();
		}

		iniManager.WriteFile(0, bEncryption);

		m_bChangedRegion	= FALSE;

		/*
		// 리소스키퍼 파일 업데이트
		char strFilename[ FILENAME_MAX ];
		wsprintf( strFilename , RM_RK_DIRECTORY_BIN "%s" , pFileName );

		if( g_bTeleportOnlyMode )
		{
			return TRUE;
		}
		else
		{
			if( FileUpload( strFilename ) )
			{
				if( UnLock( strFilename ) )
				{
				}
				return TRUE;
			}
			else
			{
				CString	str;
				str.Format( "'%s' 파일 업로드 실패!" , pFileName );
				MessageBox( str , "Region Tool" , MB_ICONERROR | MB_OK );
				return FALSE;
			}
		}
		*/
	}

	return TRUE;
}

BOOL	CRTMenuRegion::Load( char * pFileName , BOOL bDecryption )
{
	AuIniManagerA	iniManager;

	iniManager.SetPath(	pFileName	);

	if( iniManager.ReadFile(0, bDecryption) )
	{
		int		nSectionCount	;

		int		nKeyIndex	;
		int		nKeyKind	;
		int		nKeyStartX	;
		int		nKeyStartZ	;
		int		nKeyEndX	;
		int		nKeyEndZ	;

		nSectionCount	= iniManager.GetNumSection();	

		// 테긋쳐 추가함..

		ApmMap::RegionElement	stElement;

		for( int i = 0 ; i < nSectionCount ; ++i  )
		{
			nKeyIndex	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementIndex		);
			nKeyKind	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyKind	);
			nKeyStartX	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyStartX	);
			nKeyStartZ	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyStartZ	);
			nKeyEndX	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyEndX	);
			nKeyEndZ	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyEndZ	);

			stElement.nIndex		= atoi( iniManager.GetValue	( i , nKeyIndex		) );
			stElement.nKind			= atoi( iniManager.GetValue	( i , nKeyKind		) );

			stElement.nStartX		= atoi( iniManager.GetValue	( i , nKeyStartX	) );
			stElement.nStartZ		= atoi( iniManager.GetValue	( i , nKeyStartZ	) );
			stElement.nEndX			= atoi( iniManager.GetValue	( i , nKeyEndX		) );
			stElement.nEndZ			= atoi( iniManager.GetValue	( i , nKeyEndZ		) );

			AddRegion( &stElement );
		}

		m_bChangedRegion	= FALSE;
		
		return TRUE;
	}
	else
	{
		TRACE( "템플릿 파일이 없심!.\n" );
		return FALSE;
	}
}

BOOL	CRTMenuRegion::LockRegion()
{
	m_bChangedRegion = TRUE;		
	return TRUE;
	/*
	if( m_bChangedRegion || g_bTeleportOnlyMode ) return TRUE;

	char strFilename[ FILENAME_MAX ];
	wsprintf( strFilename , RM_RK_DIRECTORY_BIN "Regiontool\\Region.ini" );

	if( IsLock( strFilename ) )
	{
		CString	strOwner;
		GetLockOwner( strFilename , strOwner );
		g_strLastLockOwnerName = strOwner;

		if( strOwner == this->m_pParent->m_pMainDlg->m_strUserName )
		{
			// 이미 락하고 있는거.

			// 락 플레그 설정..
			m_bChangedRegion = TRUE;
			return TRUE;
		}
		else
		{
			// 누가 쓰고 있어.
			return FALSE;
		}
	}

	// 파일이 없으면 업로드 시킨다..
	if( IsExistFileToRemote( strFilename ) )
	{
		// 최신파일인지 확인..
		if( !IsLatestFile( strFilename ) )
		{
			if( FileDownLoad( strFilename ) )
			{
				// 파일 받기 성공..
			}
			else
			{
				MessageBox( "최신파일 받기 실패!" );
				return FALSE;
			}
		}

		if( Lock( strFilename ) )
		{
			// 락 플레그 설정..
			m_bChangedRegion = TRUE;
			return TRUE;
		}
		else
		{
			// 락 실패
			return FALSE;
		}
	}
	else
	{
		// 파일자체가 없으니까..
		// 그냥 락 설정.
		// 락 플레그 설정..
		m_bChangedRegion = TRUE;
		return TRUE;
	}
	*/
}

BOOL	CRTMenuRegion::LockTemplate()
{
	// 2004/12/31 잠시 블럭..
	return TRUE;

	/*
	if( m_bChangedTemplate ) return TRUE;

	char strFilename[ FILENAME_MAX ];
	wsprintf( strFilename , RM_RK_DIRECTORY_BIN "Regiontool\\ApmMap::RegionTemplate.ini" );

	if( IsLock( strFilename ) )
	{
		CString	strOwner;
		GetLockOwner( strFilename , strOwner );
		g_strLastLockOwnerName = strOwner;

		if( strOwner == this->m_pParent->m_pMainDlg->m_strUserName )
		{
			// 이미 락하고 있는거.

			// 락 플레그 설정..
			m_bChangedTemplate = TRUE;
			return TRUE;
		}
		else
		{
			// 누가 쓰고 있어.
			return FALSE;
		}
	}

	// 파일이 없으면 업로드 시킨다..
	if( IsExistFileToRemote( strFilename ) )
	{
		// 최신파일인지 확인..
		if( !IsLatestFile( strFilename ) )
		{
			if( FileDownLoad( strFilename ) )
			{
				// 파일 받기 성공..
			}
			else
			{
				MessageBox( "최신파일 받기 실패!" );
				return FALSE;
			}
		}

		if( Lock( strFilename ) )
		{
			// 락 플레그 설정..
			m_bChangedTemplate = TRUE;
			return TRUE;
		}
		else
		{
			// 락 실패
			return FALSE;
		}
	}
	else
	{
		// 파일자체가 없으니까..
		// 그냥 락 설정.
		// 락 플레그 설정..
		m_bChangedTemplate = TRUE;
		return TRUE;
	}
	*/
}

void	CRTMenuRegion::SetWindowControlPosition()
{
	if( m_ctlTemplate		.GetSafeHwnd() &&
		m_wndButtonEdit		.GetSafeHwnd() &&
		m_wndButtonAdd		.GetSafeHwnd() &&
		m_wndButtonRemove	.GetSafeHwnd() &&
		m_wndButtonDelete	.GetSafeHwnd() )
	{
		CRect	rect;
		GetClientRect( rect );

		CRect	rectList , rectButton;
		const int nButtonHeight = 20;

		rectList = rect;
		rectList.bottom -= ( nButtonHeight + 3 ) * 4 ;

		m_ctlTemplate.MoveWindow( rectList );

		rectButton = rect;

		rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 0;
		rectButton.bottom	= rectButton.top + nButtonHeight;
		m_wndButtonEdit.MoveWindow( rectButton );
		
		rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 1;
		rectButton.bottom	= rectButton.top + nButtonHeight;
		m_wndButtonAdd.MoveWindow( rectButton );
		
		rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 2;
		rectButton.bottom	= rectButton.top + nButtonHeight;
		m_wndButtonRemove.MoveWindow( rectButton );
		
		rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 3;
		rectButton.bottom	= rectButton.top + nButtonHeight;
		m_wndButtonDelete.MoveWindow( rectButton );
	}
}

void CRTMenuRegion::OnSize(UINT nType, int cx, int cy)
{
	CRTMenuBase::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( this->GetSafeHwnd() )
	{
		SetWindowControlPosition();
	}
}
