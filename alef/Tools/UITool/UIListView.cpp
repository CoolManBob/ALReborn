// UIListView.cpp : implementation file
//

#include "stdafx.h"
#include "UITool.h"
#include "UIListView.h"

#include "MyEngine.h"
#include ".\uilistview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CUIToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUIListView

IMPLEMENT_DYNCREATE(CUIListView, CListView)

CUIListView::CUIListView()
{
}

CUIListView::~CUIListView()
{
}


BEGIN_MESSAGE_MAP(CUIListView, CListView)
	//{{AFX_MSG_MAP(CUIListView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUIListView drawing

void CUIListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CUIListView diagnostics

#ifdef _DEBUG
void CUIListView::AssertValid() const
{
	CListView::AssertValid();
}

void CUIListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUIListView message handlers

void CUIListView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();

	CListCtrl &	csList = GetListCtrl();

	csList.ModifyStyle(0, LVS_REPORT);
	csList.InsertColumn(1, "UI Name", LVCFMT_LEFT, 148);
}

void CUIListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW *	pNMListView	= (NM_LISTVIEW*)pNMHDR;
	CListCtrl &		csList		= GetListCtrl();
	POSITION		pos			= csList.GetFirstSelectedItemPosition();
	INT32			lListIndex;
	AgcdUI *		pcsUI;

	if (pos)
	{
		lListIndex = csList.GetNextSelectedItem(pos);
		pcsUI = (AgcdUI *) csList.GetItemData(lListIndex);
		if (!pcsUI)
			return;

		g_MainWindow.OpenUI(pcsUI);
	}
	
	*pResult = 0;
}

void CUIListView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
}

LRESULT CUIListView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CListView::WindowProc(message, wParam, lParam);
}

BOOL CUIListView::PreTranslateMessage(MSG* pMsg)
{
	/*
	// 엔진으로 메시지 포워드..
	MSG messagestruct;
	messagestruct.hwnd		= GetSafeHwnd();
	messagestruct.message	= pMsg->;
	messagestruct.wParam	= wParam;
	messagestruct.lParam	= lParam;
	*/

	if (pMsg->message == WM_CHAR || pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP || pMsg->message == WM_MOUSEWHEEL)
		if (theApp.GetRenderWare().MessageProc( pMsg ))
			return TRUE;

	CHAR	szTemp[128];

	sprintf(szTemp, "Message : %d\n", pMsg->message);
	OutputDebugString(szTemp);

	return CListView::PreTranslateMessage(pMsg);
}

// SortTextItems - 리스트를 컬럼 텍스트에 따라 정렬하는 함수
// Returns - 성공시 TRUE 리턴
// nCol - 소트할 문자열을 가지고 있는 컬럼번호
// bAscending - 소트순서 지정
// low - 조사 시작 줄 - 기본값은 0
// high - 조사 마지막줄 - -1은 마지막줄을 가리킵니다.
BOOL CUIListView::SortTextItems( int nCol, BOOL bAscending,int low /*= 0*/, int high /*= -1*/ )
{
	/*
	if( nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount() )
		return FALSE;

	if( high == -1 ) 
		high = GetItemCount() - 1;

	int lo = low;
	int hi = high;
	
	CString midItem;

	if( hi <= lo ) 
		return FALSE;

	midItem = GetItemText( (lo+hi)/2, nCol );

	// 인덱스들이 교차될때까지 리스트를 돕니다(Loop).
	while( lo <= hi )
	{
		// rowText 변수가 한줄에 대한 모든 컬럼문자열을 가지게 됩니다.
		CStringArray rowText;

		//왼쪽 인덱스부터 시작하여 구역 요소보다 크거나 같은 첫째 요소를 찾음.
		if( bAscending )
			while( ( lo < high ) && ( GetItemText(lo, nCol) < midItem ) )
				++lo;
		else
			while( ( lo < high ) && ( GetItemText(lo, nCol) > midItem ) )
				++lo;
		　
		//오른쪽 인덱스부터 시작하여 구역 요소보다 크거나 같은 요소를 찾음.
		if( bAscending )
			while( ( hi > low ) && ( GetItemText(hi, nCol) > midItem ) )
				--hi;
		else
			while( ( hi > low ) && ( GetItemText(hi, nCol) < midItem ) )
				--hi;
		
		// 만약 인덱스가 교차되지 않았다면 교환하고, 만약 아이템이 같지않다면,
		if( lo <= hi )
		{
			// 아이템이 같지 않을때만 교환한다.
			if( GetItemText(lo, nCol) != GetItemText(hi, nCol))
			{
				// 줄들을 교환한다.
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount =?
				((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
				int i;
				for( i=0; i<nColCount; i++)
					rowText[i] = GetItemText(lo, i);
				
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED |
				LVIS_FOCUSED | LVIS_SELECTED |
				LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;
				GetItem( &lvitemlo );
				GetItem( &lvitemhi );
				for( i=0; i<nColCount; i++)
					SetItemText(lo, i, GetItemText(hi, i));
				lvitemhi.iItem = lo;
				SetItem( &lvitemhi );
				for( i=0; i<nColCount; i++)
					SetItemText(hi, i, rowText[i]);
				lvitemlo.iItem = hi;
				SetItem( &lvitemlo );
			}
		++lo;
		--hi;
		}
	}

	// 만약 오른쪽 인덱스가 배열의 왼쪽 끝에 닿지 않았다면 왼쪽 구역을 
	// 정렬해야 한다.
	if( low < hi )
		SortTextItems( nCol, bAscending , low, hi);

	// 만약 왼쪽 인덱스가 배열의 오른쪽 끝에 닿지 않았다면 오른쪽 구역을
	// 정렬해야 한다.
	if( lo < high )
		SortTextItems( nCol, bAscending , lo, high );
	*/

	return TRUE;
}

