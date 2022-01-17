#include "stdafx.h"
#include "EffTool.h"
#include ".\dlgnewdpnd.h"

CDlgList_BaseDpnd::CDlgList_BaseDpnd(LPEFFSET pEffSet, BOOL bParent, CWnd* pParent) : CDlgList(pParent),
 m_pEffSet(pEffSet),
 m_bParent(bParent),
 m_nSelectIndex(-1),
 m_parentDlg(NULL),
 m_fptrNotify(NULL)
{
}

BEGIN_MESSAGE_MAP(CDlgList_BaseDpnd, CDlgList)
	//{{AFX_MSG_MAP(CDlgListEffSet)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnItemchangedList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgList_BaseDpnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	vInitColum();
	vUpload();
	bSetParent( m_bParent );

	return (m_bInit	= TRUE);
}

void CDlgList_BaseDpnd::OnSize(UINT nType, int cx, int cy)
{
	CDlgList::OnSize( nType, cx, cy );
}

void CDlgList_BaseDpnd::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	if( !m_bInit )		return;
		

	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here

	m_nSelectIndex = m_ctrlListData.GetItemData( phdn->iItem );
	if( m_nSelectIndex > -1 && m_nSelectIndex < m_pEffSet->bGetVarSizeInfo().m_nNumOfBase )
	{
		CString	str;
		str.Format( m_bParent ? "parent : %d" : "chid : %d", this->m_nSelectIndex );
		m_ctrlEditInfo.SetWindowText( str );

		const AgcdEffBase* pBase = m_pEffSet->bGetPtrEffBase( m_nSelectIndex );
		ASSERT( pBase );
		if( pBase && m_fptrNotify )
			(*m_fptrNotify)( m_parentDlg, pBase, m_bParent );
	}
}

void CDlgList_BaseDpnd::bSetParent(BOOL bParent/*=TRUE*/)
{
	m_bParent = bParent;

	CString	str;
	if( m_bParent )	str.Format( "parent : %d", this->m_nSelectIndex );
	else			str.Format( "chid : %d", this->m_nSelectIndex );
		
	m_ctrlEditInfo.SetWindowText( str );
};

INT CDlgList_BaseDpnd::vInsert(INT nIndex, LPEFFBASE pEffBase)
{
	if( !m_ctrlListData )		return 0;

	LVITEM lvitem;
	memset( &lvitem, 0, sizeof( LVITEM ) );

	lvitem.mask		= LVIF_PARAM;
	lvitem.lParam	= (LPARAM)nIndex;
	lvitem.iSubItem	= 0;
	lvitem.iItem	= m_ctrlListData.GetItemCount();

	m_ctrlListData.InsertItem( &lvitem );
	char buff[MAX_PATH] = "";
	::itoa( nIndex, buff, 10 );
	m_ctrlListData.SetItemText( lvitem.iItem, 0, buff );

	const char* BASETYPE[AgcdEffBase::E_EFFBASE_NUM] = 
	{
		"BOARD",
		"PSYS",
		"PSYS_SIMPLEBLACKHOLE",
		"TAIL",
		"OBJECT",
		"LIGHT",
		"SOUND",
		"MOVINGFRAME",
		"TERRAINBOARD",
	};
	m_ctrlListData.SetItemText( lvitem.iItem, 1, BASETYPE[pEffBase->bGetBaseType()] );
	m_ctrlListData.SetItemText( lvitem.iItem, 2, pEffBase->bGetPtrTitle() );

	UpdateData(FALSE);
	return 0;
}

INT CDlgList_BaseDpnd::vUpload()
{
	ASSERT( m_pEffSet );
	if( !m_pEffSet )		return -1;
	if( !m_ctrlListData )	return 0;
		
	for( int i=0; i<m_pEffSet->m_stVarSizeInfo.m_nNumOfBase; ++i )
	{
		LPEFFBASE pEffBase = m_pEffSet->m_vecLPEffectBase[i];
		if( !pEffBase )		continue;
		if( pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_SOUND )		continue;

		if( m_bParent )
		{
			if( pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_TAIL	||
				pEffBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_MOVINGFRAME	)
				continue;
		}
		else
		{
			if( pEffBase->bFlagChk( FLAG_EFFBASE_MISSILE ) )
				continue;
		}

		vInsert( i, pEffBase );
	}

	return 0;
}
INT CDlgList_BaseDpnd::vInitColum()
{
	if( !m_ctrlListData )
		return 0;

	CRect	rc(0,0,0,0);
	m_ctrlListData.GetClientRect( &rc );
	INT		nWidth	= rc.Width();


	LV_COLUMN	lvColumn;
	memset( &lvColumn, 0, sizeof( LV_COLUMN ) );
	lvColumn.mask	= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt	= LVCFMT_LEFT;

	//index
	lvColumn.cx			= nWidth > 10 ? 10 : nWidth/3;
	lvColumn.pszText	= "index";
	lvColumn.iSubItem	= 0;
	m_ctrlListData.InsertColumn(lvColumn.iSubItem, &lvColumn );
	nWidth	= nWidth - lvColumn.cx;

	//type
	lvColumn.cx			= nWidth > 80 ? 80 : nWidth/3;
	lvColumn.pszText	= "type";
	lvColumn.iSubItem	= 1;
	m_ctrlListData.InsertColumn( lvColumn.iSubItem, &lvColumn );
	nWidth	= nWidth - lvColumn.cx;

	//title
	lvColumn.cx			= nWidth;
	lvColumn.pszText	= "title";
	lvColumn.iSubItem	= 2;
	m_ctrlListData.InsertColumn( lvColumn.iSubItem, &lvColumn );
	nWidth	= nWidth - lvColumn.cx;

	m_ctrlListData.SetExtendedStyle( m_ctrlListData.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_TWOCLICKACTIVATE );
	m_ctrlListData.SetBkColor( RGB(150, 150, 150) );
	m_ctrlListData.SetTextBkColor( RGB(150, 150, 150) );
	m_ctrlListData.SetTextColor( RGB(0, 0, 0) );

	return 0;
}

//------------------------------- CDlgNewDpnd ------------------------
IMPLEMENT_DYNAMIC(CDlgNewDpnd, CDialog)
CDlgNewDpnd::CDlgNewDpnd(LPEFFSET pEffSet, CWnd* pParent /*=NULL*/): CDialog(CDlgNewDpnd::IDD, pParent),
 m_pEffSet(pEffSet),
 m_strState(_T("")),
 m_nNodeIndex(0),
 m_dlgParent(pEffSet, TRUE, &m_staticParent),
 m_dlgChild(pEffSet, FALSE, &m_dlgChild)
{
	m_vecDependancy.clear();
}

CDlgNewDpnd::~CDlgNewDpnd()
{
	m_vecDependancy.clear();
}

void CDlgNewDpnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PARENT, m_staticParent);
	DDX_Control(pDX, IDC_STATIC_CHILD, m_staticChild);
	DDX_Text(pDX, IDC_EDIT_STATE, m_strState);
	DDX_Text(pDX, IDC_EDIT_NODE, m_nNodeIndex);
}


BEGIN_MESSAGE_MAP(CDlgNewDpnd, CDialog)
END_MESSAGE_MAP()


// CDlgNewDpnd 메시지 처리기입니다.

void CDlgNewDpnd::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	UpdateData( TRUE );

	m_vecDependancy.clear();

	POSITION posChild = m_dlgChild.m_ctrlListData.GetFirstSelectedItemPosition();
	while( posChild )
	{
		INT32 nItem = m_dlgChild.m_ctrlListData.GetNextSelectedItem( posChild );

		CString strText = m_dlgChild.m_ctrlListData.GetItemText( nItem, 0 );
		INT32 nIndexChild = atoi( strText.GetBuffer() );

		AgcdEffSet::stBaseDependancy temp;
		temp.SetChildIndex( nIndexChild );
		if( m_pEffSet->bForTool_FindDependancy( temp ) == -1 )
		{
			AgcdEffSet::stBaseDependancy	dependancy;

			if( m_nNodeIndex )
			{
				LPEFFBASE pEffBase = m_pEffSet->bGetPtrEffBase( m_dlgParent.bGetIndex() );
				if( !dynamic_cast<AgcdEffObj*>(pEffBase) || dynamic_cast<AgcdEffObj*>(pEffBase)->bFindNodeIndex(m_nNodeIndex) < 0 )
				{
					ToWnd( "해당 부모 배이스가 오브젝트 타입이 아니거나 해당 노드가 없음!\r\n" );
					continue;
				}
			
				dependancy.FlagOn( FLAG_EFFBASE_DEPENDANCY_HASNODE );
				dependancy.SetParentNodeID( m_nNodeIndex );
			}

			dependancy.SetParentIndex( m_dlgParent.bGetIndex() );
			dependancy.SetChildIndex( nIndexChild );

			m_vecDependancy.push_back( dependancy );
		}
	}

	CDialog::OnOK();
}

void CDlgNewDpnd::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnCancel();
}

BOOL CDlgNewDpnd::OnInitDialog()
{
	CDialog::OnInitDialog();	

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_dlgParent.Create( CDlgList::IDD, &m_staticParent );
	m_dlgParent.bSetCB( this, CDlgNewDpnd::CB_SelChanged );
	m_dlgParent.ShowWindow( SW_SHOWMAXIMIZED );
	m_dlgParent.bSetParent();

	m_dlgChild.Create( CDlgList::IDD, &m_staticChild );
	m_dlgChild.bSetCB( this, CDlgNewDpnd::CB_SelChanged );
	m_dlgChild.ShowWindow( SW_SHOWMAXIMIZED );
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgNewDpnd::CB_SelChanged(CDlgNewDpnd* pThis, const AgcdEffBase* pSelectedBase, BOOL bParent)
{
	if( !pThis )		return;

	pThis->m_strState.Format( bParent ? "parent : %s" : "child : %s", pSelectedBase->bGetPtrTitle() );
	pThis->UpdateData( FALSE );
}