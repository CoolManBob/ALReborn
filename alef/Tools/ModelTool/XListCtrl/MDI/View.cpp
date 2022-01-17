// View.cpp : implementation of the CMDIView class
//

#include "stdafx.h"
#include "MDI.h"

#include "Doc.h"
#include "View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDIView

IMPLEMENT_DYNCREATE(CMDIView, CView)

BEGIN_MESSAGE_MAP(CMDIView, CView)
	//{{AFX_MSG_MAP(CMDIView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDIView construction/destruction

CMDIView::CMDIView() :
	m_pList(0)
{
}

CMDIView::~CMDIView()
{
	delete m_pList;
}

BOOL CMDIView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMDIView drawing

void CMDIView::OnDraw(CDC* /*pDC*/)
{
	CMDIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CMDIView printing

BOOL CMDIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMDIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CMDIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CMDIView diagnostics

#ifdef _DEBUG
void CMDIView::AssertValid() const
{
	CView::AssertValid();
}

void CMDIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMDIDoc* CMDIView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMDIDoc)));
	return (CMDIDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMDIView message handlers

int CMDIView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	DWORD dwStyle = LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | 
		WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP;

	CRect rect(0,0,100,100);

	m_pList = new CXListCtrl();
	ASSERT(m_pList);
	VERIFY(m_pList->CreateEx(WS_EX_CLIENTEDGE, _T("SysListView32"), _T(""),
						dwStyle, rect, this, 2000, NULL));
	CFont *pFont = GetFont();
	if (pFont)
		m_pList->SetFont(pFont);

	m_pList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return 0;
}

void CMDIView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	InitListCtrl(m_pList);
	FillListCtrl(m_pList);
	UpdateWindow();
}

void CMDIView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (m_pList && ::IsWindow(m_pList->m_hWnd))
	{
		m_pList->LockWindowUpdate();

		m_pList->MoveWindow(0, 0, cx, cy);

		CRect rect;
		GetClientRect(&rect);

		int w = cx - 2 - ::GetSystemMetrics(SM_CXVSCROLL);
		int total_cx = 0;
		int n = m_pList->GetColumns();
		int nPad = m_pList->GetCellPadding();

		// adjust columns
		for (int i = 0; i < n; i++)
		{
			int colwidth = (i == (n - 1)) ? w - total_cx - 2 : (w * m_nColWidths[i]) / 64;
			total_cx += colwidth;
			m_pList->SetColumnWidth(i, colwidth-2*nPad-2);
		}

		m_pList->UnlockWindowUpdate();
	}
}
