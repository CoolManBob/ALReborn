#include "stdafx.h"

#include "EffTool.h"

#include "EffToolDoc.h"
#include "EffToolView.h"

#include "MainFrm.h"

#include "ToolOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
	const float	BOUNDINGBOX_INF	= -10.f;
	const float BOUNDINGBOX_SUP	=  10.f;
};

/////////////////////////////////////////////////////////////////////////////
// CEffToolView

IMPLEMENT_DYNCREATE(CEffToolView, CView)

BEGIN_MESSAGE_MAP(CEffToolView, CView)
	//{{AFX_MSG_MAP(CEffToolView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffToolView construction/destruction

CEffToolView::CEffToolView()
{
	// TODO: add construction code here

}

CEffToolView::~CEffToolView()
{
}

BOOL CEffToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CEffToolView drawing

void CEffToolView::OnDraw(CDC* pDC)
{
	CEffToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CEffToolView printing

BOOL CEffToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEffToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CEffToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CEffToolView diagnostics

#ifdef _DEBUG
void CEffToolView::AssertValid() const
{
	CView::AssertValid();
}

void CEffToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEffToolDoc* CEffToolView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEffToolDoc)));
	return (CEffToolDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEffToolView message handlers

void CEffToolView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
	RwBBox	bbox	= {
		{BOUNDINGBOX_INF, BOUNDINGBOX_INF, BOUNDINGBOX_INF},
		{BOUNDINGBOX_SUP, BOUNDINGBOX_SUP, BOUNDINGBOX_SUP}
	};

	CRenderWare::GetInst().Initialise( GetSafeHwnd() );
	
	CGlobalVar::bGetInst().bSetInitEngin(TRUE);
}

void CEffToolView::OnDestroy() 
{
	CRenderWare::GetInst().Destroy();

	CView::OnDestroy();
	
	// TODO: Add your message handler code here
}

void CEffToolView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect	rc;
	GetClientRect(&rc);

	CRenderWare::GetInst().SetCameraViewSize( rc.Width(), rc.Height() );	
}

void CEffToolView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	CCamera::bGetInst().bSetActiveWnd( TRUE );
}

void CEffToolView::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	CCamera::bGetInst().bSetActiveWnd( FALSE );
}

void CEffToolView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_cptLButtonDown	= point;

	switch( CToolOption::bGetInst().bGetLBtnAct() )
	{
		case CToolOption::e_lbtn_camrot:
		case CToolOption::e_lbtn_movefrmOnVirtualPlane:
		case CToolOption::e_lbtn_movefrmOnField:
		case CToolOption::e_lbtn_frmrot:
			g_MyEngine.bPickShowFrm( point );
			break;
	}
	
	CView::OnLButtonDown(nFlags, point);
}

void CEffToolView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	g_MyEngine.bLBtnUp();

	CView::OnLButtonUp(nFlags, point);
}

void CEffToolView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_cptMButtonDown	= point;
	
	CView::OnMButtonDown(nFlags, point);
}

void CEffToolView::OnMButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_cptMButtonDown	= point;

	CView::OnMButtonUp(nFlags, point);
}

void CEffToolView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( nFlags & MK_MBUTTON )
	{
		CCamera::bGetInst().bCameraWorkWithMouse( 
			  FALSE
			, float( point.x - m_cptMButtonDown.x )
			, float( point.y - m_cptMButtonDown.y ) );
		m_cptMButtonDown	= point;
	}
	else if( nFlags & MK_LBUTTON )
	{
		switch( CToolOption::bGetInst().bGetLBtnAct() )
		{
		case CToolOption::e_lbtn_camrot:
			{
				CCamera::bGetInst().bCameraWorkWithMouse( 
					  TRUE
					, float( point.x - m_cptLButtonDown.x )
					, float( point.y - m_cptLButtonDown.y ) );
			}
			break;
		case CToolOption::e_lbtn_movefrmOnVirtualPlane:
			{
				g_MyEngine.bMoveShowFrm( point );
			}
			break;
		case CToolOption::e_lbtn_movefrmOnField:
			{
				g_MyEngine.bMoveShowFrm( point );
			}
			break;
		case CToolOption::e_lbtn_frmrot:
			{
				g_MyEngine.bRotShowFrm( 
					  ( point.x - m_cptLButtonDown.x )
					, ( point.y - m_cptLButtonDown.y ) );
			}
			break;
		}

		m_cptLButtonDown	= point;
	}
	else if( nFlags & MK_RBUTTON )
	{
		CCamera::bGetInst().bCameraWorkWithMouse( 
			  FALSE
			, float( point.x - m_cptRButtonDown.x )
			, float( point.y - m_cptRButtonDown.y ) );
		m_cptRButtonDown	= point;
	}
	
	CView::OnMouseMove(nFlags, point);
}

BOOL CEffToolView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	CCamera::bGetInst().bZoom(zDelta*2.5f);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CEffToolView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_cptRButtonDown	= point;

	//ApMemoryManager::GetInstance().ReportMemoryInfo();
	
	CView::OnRButtonDown(nFlags, point);
}

void CEffToolView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnRButtonUp(nFlags, point);
}

void CEffToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	ToWnd(Eff2Ut_FmtMsg("CEffToolView::OnChar -- nChar : %d, nRepCnt : %d, nFlags : %08x, <%c>\n", nChar, nRepCnt, nFlags, nChar));
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CEffToolView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default	
	ToWnd(Eff2Ut_FmtMsg("CEffToolView::OnChar -- nChar : %d, nRepCnt : %d, nFlags : %08x, <%c>\n", nChar, nRepCnt, nFlags, nChar));
	CView::OnChar(nChar, nRepCnt, nFlags);
}
