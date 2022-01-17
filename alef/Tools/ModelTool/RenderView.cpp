// RenderView.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "RenderView.h"
#include ".\renderview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenderView

IMPLEMENT_DYNCREATE(CRenderView, CView)

CRenderView::CRenderView()
{		
	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
	m_bMButtonDown = FALSE;
}

CRenderView::CRenderView(CWnd *pcsParent, RECT &stInitRect)
{
	InitializeMember();
	Create(NULL, NULL, (WS_CHILD | WS_VISIBLE | WS_GROUP), stInitRect, pcsParent, NULL, NULL);

	ASSERT( CModelToolApp::GetInstance() );
	ASSERT( CModelToolApp::GetInstance()->GetEngine() );
}

CRenderView::~CRenderView()
{
}

BEGIN_MESSAGE_MAP(CRenderView, CView)
	//{{AFX_MSG_MAP(CRenderView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP	
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenderView drawing

void CRenderView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CRenderView diagnostics

#ifdef _DEBUG
void CRenderView::AssertValid() const
{
	CView::AssertValid();
}

void CRenderView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRenderView message handlers

void CRenderView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();

	m_bLButtonDown = TRUE;
	SetCapture();

	RwCamera* pstCamera = CModelToolApp::GetInstance()->GetRenderWare()->GetCamera();
	RpWorld	* pstWorld = CModelToolApp::GetInstance()->GetRenderWare()->GetWorld();
	if( !pstCamera || !pstWorld )
	{
		CView::OnLButtonDown( nFlags, point );
		return;
	}

	RwV2d pixel	= { (float)point.x, (float)point.y };
	RpAtomic* pPickAtomic	= CModelToolApp::GetInstance()->RenderWorldForAllIntersections( &pixel );
	if( pPickAtomic )
	{
		if( CModelToolDlg::GetInstance()->EditEquipmentsDlgIsPickMode() )
		{
			if (!CModelToolApp::GetInstance()->CheckArmour(pPickAtomic))
			{
				if (CModelToolApp::GetInstance()->GetPickedAtomic() != pPickAtomic)
				{
					CModelToolApp::GetInstance()->SetPickedAtomic(pPickAtomic);
					CModelToolApp::GetInstance()->SetInitTransform();
				}
			}
		}
		else if (CModelToolApp::GetInstance()->GetEngine()->GetAgcmUDADlgModule()->IsOpenMainUDADlg())
		{
			if( !CModelToolApp::GetInstance()->CheckItemArmour() )
			{
				if (CModelToolApp::GetInstance()->GetPickedAtomic() != pPickAtomic)
				{
					CModelToolApp::GetInstance()->SetUDAData(pPickAtomic);
				}
			}
		}
		else if ( CModelToolApp::GetInstance()->GetEngine()->GetAgcmSetFaceDlgModule()->IsOpenMainFaceDlg() )
		{
			if ( !CModelToolApp::GetInstance()->CheckItemArmour() )
			{
				if (CModelToolApp::GetInstance()->GetPickedAtomic() != pPickAtomic)
				{
					CModelToolApp::GetInstance()->SetFaceAtomic(pPickAtomic);
				}
			}
		}
	}
	else
	{
		if( CModelToolApp::GetInstance()->CheckItemArmour() )
		{
			pPickAtomic = RwUtilPickWorldNearestAtomic( pstCamera, pstWorld, &pixel );
			if( pPickAtomic )
			{
				if( CModelToolApp::GetInstance()->GetPickedAtomic() != pPickAtomic )
					CModelToolApp::GetInstance()->SetUDAData( pPickAtomic );
			}
		}
	}


	CView::OnLButtonDown(nFlags, point);
}

void CRenderView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_bLButtonDown)
	{
		m_bLButtonDown = FALSE;
		ReleaseCapture();
	}	
	
	CView::OnLButtonUp(nFlags, point);
}

void CRenderView::OnMouseMove( UINT nFlags, CPoint point ) 
{
	//SetFocus();

	// TODO: Add your message handler code here and/or call default	
	RwV2d v2dDelta = CountDelta(point);
	if( m_bLButtonDown )
	{
		CModelToolApp::GetInstance()->RotateCamera( &v2dDelta );
	}
	else if( m_bMButtonDown )
	{
		CModelToolApp::GetInstance()->TranslateCamera( &v2dDelta );
	}
	else if( m_bRButtonDown )
	{
		AgcEngineChild* pEngine = CModelToolApp::GetInstance()->GetEngine();
		if( pEngine->GetAgcmBlockingDlgModule()->IsOpenEditBlocking() )
		{
			RwCamera* pCamera = CModelToolApp::GetInstance()->GetRenderWare()->GetCamera();
			pEngine->GetAgcmBlockingDlgModule()->UpdateBlock( v2dDelta, pCamera );
		}

		else if( CModelToolDlg::GetInstance()->EditEquipmentsDlgIsMouseMode() )
		{
			CModelToolApp::GetInstance()->TransformPickedAtomic( v2dDelta );
		}
	}

	CView::OnMouseMove( nFlags, point );
}

RwV2d CRenderView::CountDelta(CPoint point, float fFactor /* = 0.5f*/ )
{
	static CPoint ptOld = CPoint(0, 0);
	
	RwV2d delta = {fFactor * (float)(point.x - ptOld.x), fFactor * (float)(point.y - ptOld.y)};
	
	ptOld = point;

	return delta;
}

VOID CRenderView::InitializeMember()
{
	m_bLButtonDown	= FALSE;
	m_bRButtonDown	= FALSE;
	m_bMButtonDown = FALSE;
}

void CRenderView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();

	m_bRButtonDown = TRUE;
	SetCapture();
	
	CView::OnRButtonDown(nFlags, point);
}

void CRenderView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_bRButtonDown)
	{
		m_bRButtonDown = FALSE;
		ReleaseCapture();
	}
	
	CView::OnRButtonUp(nFlags, point);
}

void CRenderView::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	m_bMButtonDown = TRUE;
	SetCapture();

	CView::OnMButtonDown(nFlags, point);
}

void CRenderView::OnMButtonUp(UINT nFlags, CPoint point)
{
	if ( m_bMButtonDown )
	{
		m_bMButtonDown = FALSE;
		ReleaseCapture();
	}

	CView::OnMButtonUp(nFlags, point);
}

BOOL CRenderView::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	CModelToolApp::GetInstance()->ZoomCamera( (float)zDelta );

	return CView::OnMouseWheel( nFlags, zDelta, pt );
}

BOOL CRenderView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	TRACE( "CRenderView::OnEraseBkgnd" );
	return FALSE;//CView::OnEraseBkgnd(pDC);
}

BOOL CRenderView::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
		KeyDown( pMsg->wParam );

	return CView::PreTranslateMessage(pMsg);
}

void CRenderView::KeyDown( WPARAM wParam )
{
	RwV2d	vCamera = { 0.f, 0.f };
	switch( wParam )
	{
	case VK_LEFT:
		vCamera.x -= 10.f;
		break;
	case VK_UP:
		vCamera.y -= 10.f;
		break;
	case VK_RIGHT:
		vCamera.x += 10.f;
		break;
	case VK_DOWN:
		vCamera.y += 10.f;
		break;
	}

	CModelToolApp::GetInstance()->RotateCamera( &vCamera );
}