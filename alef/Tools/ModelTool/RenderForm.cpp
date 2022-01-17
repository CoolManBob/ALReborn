// RenderForm.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "RenderForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenderForm

IMPLEMENT_DYNCREATE(CRenderForm, CFormView)

CRenderForm::CRenderForm()
	: CFormView(CRenderForm::IDD)
{
	//{{AFX_DATA_INIT(CRenderForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CRenderForm::CRenderForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor)
	: CFormView(CRenderForm::IDD)
{
	InitializeMember();
	m_hBackBrush = hBackColor;

	Create(NULL, NULL, (WS_CHILD | WS_VISIBLE), stInitRect, pcsParent, NULL, NULL);

	CreateMember();
}

CRenderForm::~CRenderForm()
{
}

void CRenderForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenderForm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRenderForm, CFormView)
	//{{AFX_MSG_MAP(CRenderForm)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenderForm diagnostics

#ifdef _DEBUG
void CRenderForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CRenderForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRenderForm message handlers

BOOL CRenderForm::CreateMember()
{
	CRect	rc;
	GetWindowRect( rc );

	m_pcsRenderView	= new CRenderView( this, CRect( 5, 5, rc.Width() - 5, rc.Height() - 5 ) );

	// 랜더웨어를 초기화한당...
	m_pcsRenderWare	= new CRenderWare;
	if( !m_pcsRenderWare->Initialize( m_pcsRenderView->m_hWnd ) )
	{
		OutputDebugString("!\n!\nERROR <CRenderForm::OnInitialUpdate() - 랜더웨어 초기화 실패>\n!\n!\n");
		return FALSE;
	}

	m_pcsRenderView->GetWindowRect( rc );
	RwRect	rcNew = { 0, 0, rc.Width(), rc.Height() };
	m_pcsRenderWare->SetCameraSize( &rcNew );

	RwImageSetPath("Texture\\Etc\\;Texture\\World\\;Texture\\Character\\;Texture\\Object\\;Texture\\Object\\;Texture\\Effect\\;Texture\\UI\\;Texture\\SKILL\\;Texture\\Item\\;Texture\\UI\\BASE\\;Texture\\NotPacked\\");
	RpMaterialD3DFxSetSearchPath(".\\Effect\\");

	return TRUE;
}

void CRenderForm::OnDestroy() 
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_pcsRenderWare)
	{
		delete m_pcsRenderWare;
		m_pcsRenderWare = NULL;
	}

	if(m_pcsRenderView)
	{
		m_pcsRenderView->DestroyWindow();
		m_pcsRenderView = NULL;
	}
}

VOID CRenderForm::InitializeMember()
{
	m_pcsRenderView		= NULL;
	m_pcsRenderWare		= NULL;
}

BOOL CRenderForm::UpdateRenderForm()
{
	if( m_pcsRenderView )
		m_pcsRenderView->Invalidate(FALSE);

	return TRUE;
}

void CRenderForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(CModelToolApp::GetInstance())
	{
		if(CModelToolApp::GetInstance()->m_pMainWnd)
		{
			CRect rc;
			GetClientRect( rc );

			rc.left		+= 5;
			rc.top		+= 5;
			rc.right	-= 5;
			rc.bottom	-= 5;

			m_pcsRenderView->MoveWindow( rc );

			RwRect rcCamera = { 0, 0, rc.Width(), rc.Height() };
			m_pcsRenderWare->SetCameraSize( &rcCamera );

			/*
			RwRect r;

			r.x = 0		;
			r.y = 0		;
			r.w = cx	;
			r.h = cy	;
			AppEventHandler(rsCAMERASIZE, &r);
			*/

			// 독자적인 구조에 의한 강제적인 초기화 설정 -_-;;
			CModelToolApp	*	pModelToolApp	= CModelToolApp::GetInstance();
			AgcEngineChild	*	pEngine			= pModelToolApp->GetEngine();
			pEngine->GetAgcmRenderModule()->SetWorldCamera( m_pcsRenderWare->GetWorld() , m_pcsRenderWare->GetCamera() ); 
			pEngine->GetAgcmFont()->OnCameraStateChange(CSC_RESIZE);

		}
	}
}
