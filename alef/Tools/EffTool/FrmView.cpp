// FrmView.cpp : implementation file
//

#include "stdafx.h"
#include "EffTool.h"
#include "FrmView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrmView

IMPLEMENT_DYNCREATE(CFrmView, CFormView)

CFrmView::CFrmView()
	: CFormView(CFrmView::IDD)
{
	//{{AFX_DATA_INIT(CFrmView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CFrmView::~CFrmView()
{
}

void CFrmView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrmView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrmView, CFormView)
	//{{AFX_MSG_MAP(CFrmView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrmView diagnostics

#ifdef _DEBUG
void CFrmView::AssertValid() const
{
	CFormView::AssertValid();
}

void CFrmView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFrmView message handlers
