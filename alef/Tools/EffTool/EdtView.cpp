// EdtView.cpp : implementation file
//

#include "stdafx.h"
#include "EffTool.h"
#include "EdtView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdtView

IMPLEMENT_DYNCREATE(CEdtView, CEditView)

CEdtView::CEdtView()
{
}

CEdtView::~CEdtView()
{
}


BEGIN_MESSAGE_MAP(CEdtView, CEditView)
	//{{AFX_MSG_MAP(CEdtView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEdtView drawing

void CEdtView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CEdtView diagnostics

#ifdef _DEBUG
void CEdtView::AssertValid() const
{
	CEditView::AssertValid();
}

void CEdtView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEdtView message handlers
