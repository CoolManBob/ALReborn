// UIListView.cpp : implementation file
//

#include "stdafx.h"
#include "Casper.h"
#include "UserListView.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCasperApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUserListView

IMPLEMENT_DYNCREATE(CUserListView, CListView)

CUserListView::CUserListView()
{
}

CUserListView::~CUserListView()
{
}


BEGIN_MESSAGE_MAP(CUserListView, CListView)
	//{{AFX_MSG_MAP(CUserListView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserListView drawing

void CUserListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CUserListView diagnostics

#ifdef _DEBUG
void CUserListView::AssertValid() const
{
	CListView::AssertValid();
}

void CUserListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUserListView message handlers

void CUserListView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();

	CListCtrl &	csList = GetListCtrl();

	csList.ModifyStyle(0, LVS_REPORT);
	csList.InsertColumn(1, "ID List", LVCFMT_LEFT, 180);
}

void CUserListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW *	pNMListView	= (NM_LISTVIEW*)pNMHDR;
	CListCtrl &		csList		= GetListCtrl();

	CString strName = csList.GetItemText(pNMListView->iItem, 0);
	AgpdCharacter *pcsAgpdCharacter = g_pcsAgpmCharacter->GetCharacter((CHAR*)strName.operator LPCTSTR());

	*pResult = 0;
}

void CUserListView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
}

LRESULT CUserListView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// 엔진으로 메시지 포워드..
	MSG messagestruct;
	messagestruct.hwnd		= GetSafeHwnd();
	messagestruct.message	= message;
	messagestruct.wParam	= wParam;
	messagestruct.lParam	= lParam;

	if (message == WM_CHAR || message == WM_KEYDOWN || message == WM_KEYUP)
		if (theApp.GetRenderWare().MessageProc( & messagestruct ))
			return TRUE;
	
	return CListView::WindowProc(message, wParam, lParam);
}
