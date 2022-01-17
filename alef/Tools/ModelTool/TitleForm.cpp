// TitleForm.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "TitleForm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitleForm

CTitleForm* CTitleForm::m_pThisTitleForm = NULL;

IMPLEMENT_DYNCREATE(CTitleForm, CFormView)

CTitleForm::CTitleForm()
	: CFormView(CTitleForm::IDD)
{
	//{{AFX_DATA_INIT(CTitleForm)
	//}}AFX_DATA_INIT
}

CTitleForm::CTitleForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor) : CFormView(CTitleForm::IDD)
{
	InitializeMember();
	m_hBackBrush = hBackColor;

	Create(NULL, NULL, (WS_CHILD | WS_VISIBLE), stInitRect, pcsParent, NULL, NULL);
	CreateMember();

	CTitleForm::m_pThisTitleForm = this;
}

CTitleForm::~CTitleForm()
{
}

void CTitleForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTitleForm)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTitleForm, CFormView)
	//{{AFX_MSG_MAP(CTitleForm)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTitleForm diagnostics

#ifdef _DEBUG
void CTitleForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CTitleForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTitleForm message handlers

VOID CTitleForm::InitializeMember()
{
	m_pcsCurTitle				= NULL;
}

void CTitleForm::OnDestroy() 
{
	CFormView::OnDestroy();
	
	if (m_pcsCurTitle)
	{
		delete m_pcsCurTitle;
		m_pcsCurTitle = NULL;
	}
}

VOID CTitleForm::CreateMember()
{
	CRect csParentRect;
	GetClientRect(csParentRect);
	m_rtCurTitle	= CRect(csParentRect.left + 108 + 2, 2, csParentRect.right - 30 - 2, csParentRect.bottom - 2);
	m_pcsCurTitle	= new CEdit();
	m_pcsCurTitle->Create( WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_READONLY, m_rtCurTitle, this, IDC_EDIT_RESOURCE_TITLE );
}

VOID CTitleForm::SetCurrentTitle(CHAR *szTitle)
{
	m_pcsCurTitle->SetWindowText( szTitle );
}

void CTitleForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( CModelToolApp::GetInstance() && CModelToolApp::GetInstance()->m_pMainWnd )
	{
		CRect csParentRect;
		GetClientRect( csParentRect );
		m_pcsCurTitle->MoveWindow( CRect( m_rtCurTitle.left, m_rtCurTitle.top, csParentRect.right - 30 - 2, m_rtCurTitle.bottom	) );
	}
}
