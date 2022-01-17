#include "stdafx.h"
#include "../EffTool.h"
#include "DlgListEffSet.h"
#include "DlgTreeEffSet.h"

#include "../MainFrm.h"
#include "../EffToolDoc.h"

#include "../ToolOption.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CDlgListEffSet::CDlgListEffSet(CWnd* pParent) : CDlgList(pParent), 
 m_nColumn(0L),
 m_bSortedByID(FALSE),
 m_bSortedByTitle(FALSE),
 m_bSortedByBaseNum(FALSE),
 m_bSortedByBaseLife(FALSE),
 m_pEffSet(NULL)
{
}

BEGIN_MESSAGE_MAP(CDlgListEffSet, CDlgList)
	//{{AFX_MSG_MAP(CDlgListEffSet)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DATA, OnColumnclickList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnItemchangedList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DATA, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDlgListEffSet message handlers
BOOL CDlgListEffSet::OnInitDialog() 
{
	CDlgList::OnInitDialog();
	
	// TODO: Add extra initialization here

	vInitColum();

	if( CGlobalVar::bGetInst().bIsInitEngin() )
		vUpload();

	return ( m_bInit = TRUE ); 
}

void CDlgListEffSet::OnSize(UINT nType, int cx, int cy) 
{
	CDlgList::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( !m_bInit )		return;

	CDlgList::OnSize(nType, cx, cy);
	CRect rc(0,0,0,0);

	m_ctrlListData.GetClientRect( &rc );
	INT	nWidth	= rc.Width();

	m_ctrlListData.SetColumnWidth( 0, ( nWidth > 80 ) ? 80 : nWidth/3 );
	nWidth -= m_ctrlListData.GetColumnWidth(0);
	
	m_ctrlListData.SetColumnWidth( 1, nWidth*3/4 );
	nWidth -= m_ctrlListData.GetColumnWidth(1);
	
	m_ctrlListData.SetColumnWidth( 2, nWidth/2 );
	nWidth -= m_ctrlListData.GetColumnWidth(2);
	
	m_ctrlListData.SetColumnWidth( 3, nWidth );
	nWidth -= m_ctrlListData.GetColumnWidth(2);

	UpdateWindow();
}

void CDlgListEffSet::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// TODO: Add your control notification handler code here
	CDlgList::ListCtrlAndSubItem st(&m_ctrlListData,pNMListView->iSubItem);

	switch( pNMListView->iSubItem )
	{
	case 0:
		m_ctrlListData.SortItems( m_bSortedByID ? CB_CmpByInt_Up : CB_CmpByInt_Down, (LPARAM)&st );
		m_bSortedByID		= !m_bSortedByID;
		break;
	case 1:
		m_ctrlListData.SortItems( m_bSortedByTitle ? CB_CmpByString_Up : CB_CmpByString_Down, (LPARAM)&st );
		m_bSortedByTitle	= !m_bSortedByTitle;
		break;
	case 2:
		m_ctrlListData.SortItems( m_bSortedByBaseNum ? CB_CmpByInt_Up : CB_CmpByInt_Down, (LPARAM)&st );
		m_bSortedByBaseNum	= !m_bSortedByBaseNum;
		break;
	case 3:
		m_ctrlListData.SortItems( m_bSortedByBaseLife ? CB_CmpByInt_Up : CB_CmpByInt_Down, (LPARAM)&st );
		m_bSortedByBaseLife	= !m_bSortedByBaseLife;
		break;
	}
	
	*pResult = 0;
}

void CDlgListEffSet::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	
	DWORD dwr = m_ctrlListData.GetItemData( phdn->iItem );

	CHAR	buff[MAX_PATH]	= "";
	m_ctrlListData.GetItemText( phdn->iItem, 0, buff, MAX_PATH );
	m_ctrlEditInfo.SetWindowText( buff );
	
	*pResult = 0;
}

void CDlgListEffSet::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int nIndex	= m_ctrlListData.GetSelectionMark();
	if( T_ISMINUS4( nIndex ) )		return;

	DWORD dwr =	m_ctrlListData.GetItemData( nIndex );

	CHAR	buff[MAX_PATH]	= "¼±ÅÃµÊ : ";
	m_ctrlListData.GetItemText( nIndex, 1, &buff[strlen(buff)], MAX_PATH );
	m_ctrlEditInfo.SetWindowText( buff );
	
	if( AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2() )
	{
		m_pEffSet = AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_FindEffSet( dwr );
		if( m_pEffSet )
			((CMainFrame*)::AfxGetMainWnd())->bInsEffSetFromListToTree( m_pEffSet );
	}

	*pResult = 0;
}

INT CDlgListEffSet::bInsItem(DWORD dwEffSetID, LPCTSTR szEffSetTitle, INT nBaseNum, UINT ulLife)
{
	LVITEM lvitem;
	memset( &lvitem, 0, sizeof( LVITEM ) );

	lvitem.mask		= LVIF_PARAM;
	lvitem.lParam	= (LPARAM)dwEffSetID;
	lvitem.iSubItem	= 0;
	lvitem.iItem	= m_ctrlListData.GetItemCount();
	
	INT	ir	= m_ctrlListData.InsertItem( &lvitem );
	if( T_ISMINUS4( ir ) )
	{
		ErrToWnd( "m_ctrlListData.InsertItem( &lvitem ) failed @ CDlgListEffSet::bInsItem" );
		return -1;
	}

	INT nItem	= ir;
	char	buff[MAX_PATH]	= "";
	::itoa( dwEffSetID, buff, 10 );
	ir = m_ctrlListData.SetItemText( lvitem.iItem, 0, buff );
	if( !ir )
	{
		ErrToWnd( "m_ctrlListData.SetItemText(dwEffSetID) failed @ CDlgListEffSet::bInsItem" );
		return -1;
	}

	ir = m_ctrlListData.SetItemText( lvitem.iItem, 1, szEffSetTitle );
	if( !ir )
	{
		ErrToWnd( "m_ctrlListData.SetItemText(szEffSetTitle) failed @ CDlgListEffSet::bInsItem" );
		return -1;
	}

	::itoa( nBaseNum, buff, 10 );
	ir = m_ctrlListData.SetItemText( lvitem.iItem, 2, buff );
	if( !ir )
	{
		ErrToWnd( "m_ctrlListData.SetItemText(nBaseNum) failed @ CDlgListEffSet::bInsItem" );
		return -1;
	}

	::itoa( ulLife, buff, 10 );
	ir = m_ctrlListData.SetItemText( lvitem.iItem, 3, buff );
	if( !ir )
	{
		ErrToWnd( "m_ctrlListData.SetItemText(ulLife) failed @ CDlgListEffSet::bInsItem" );
		return -1;
	}

	UpdateData(FALSE);
	
	return 0;
};

INT CDlgListEffSet::vUpload()
{
	const LPEffectSetMap& mapEffSet = g_pAgcmEff2->bForTool_GetEffSetMng();
	for( LPEffectSetMapCItr CItr = mapEffSet.begin(); CItr != mapEffSet.end(); ++CItr )
	{
		const EFFSET* pEffSet	= (*CItr).second;
		if( !pEffSet )
		{
			ErrToWnd("pEffSet == NULL @ CMainFrame::bShowAllEffSetToListWnd");
			continue;
		}

		bInsItem( pEffSet->bGetID(), pEffSet->bGetTitle(), pEffSet->bGetVarSizeInfo().m_nNumOfBase, pEffSet->bGetLife() );
	}

	return 0;
};

INT CDlgListEffSet::bReloadAllItem()
{
	BOOL bChk = m_ctrlListData.DeleteAllItems();
	ASSERT( bChk );

	bChk = g_pAgcmEff2->bForTool_ReloadAllEffect();
	ASSERT( bChk );

	return vUpload();
};

INT CDlgListEffSet::vInitColum()
{
	CRect	rc(0,0,0,0);
	m_ctrlListData.GetClientRect( &rc );
	INT		nWidth	= rc.Width();

	LV_COLUMN	lvColumn;
	memset( &lvColumn, 0, sizeof( LV_COLUMN ) );
	lvColumn.mask	= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt	= LVCFMT_LEFT;

	//EffSetID
	lvColumn.cx			= ( nWidth > 10 ) ? 10 : nWidth/3;
	lvColumn.pszText	= "EffSetID";
	lvColumn.iSubItem	= 0;

	m_ctrlListData.InsertColumn( lvColumn.iSubItem, &lvColumn );
	nWidth	= nWidth-lvColumn.cx;

	//EffSetTitle
	lvColumn.cx			= nWidth*3/4;
	lvColumn.pszText	= "title";
	lvColumn.iSubItem	= 1;

	m_ctrlListData.InsertColumn( lvColumn.iSubItem, &lvColumn );
	nWidth	= nWidth-lvColumn.cx;

	//EffSetInfo
	lvColumn.cx			= nWidth/2;
	lvColumn.pszText	= "BaseNum";
	lvColumn.iSubItem	= 2;

	m_ctrlListData.InsertColumn( lvColumn.iSubItem, &lvColumn );
	nWidth	= nWidth-lvColumn.cx;

	//EffLife
	lvColumn.cx			= nWidth;
	lvColumn.pszText	= "life";
	lvColumn.iSubItem	= 3;

	m_ctrlListData.InsertColumn( lvColumn.iSubItem, &lvColumn );
	nWidth	= nWidth-lvColumn.cx;

	m_ctrlListData.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_TWOCLICKACTIVATE );
	m_ctrlListData.SetBkColor( RGB(150, 150, 150) );
	m_ctrlListData.SetTextBkColor( RGB(150, 150, 150) );
	m_ctrlListData.SetTextColor( RGB(0, 0, 0) );

	return 0;
};
