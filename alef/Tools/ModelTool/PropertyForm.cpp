// PropertyForm.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "PropertyForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyForm

IMPLEMENT_DYNCREATE(CPropertyForm, CFormView)

CPropertyForm::CPropertyForm() : CFormView(CPropertyForm::IDD)
{
	//{{AFX_DATA_INIT(CPropertyForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyForm::CPropertyForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor) : CFormView(CPropertyForm::IDD)
{
	InitializeMember();
	m_hBackBrush = hBackColor;

	Create(NULL, NULL, (WS_CHILD | WS_VISIBLE), stInitRect, pcsParent, NULL, NULL);
	CreateMember();
}

CPropertyForm::~CPropertyForm()
{
}

void CPropertyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyForm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyForm, CFormView)
	//{{AFX_MSG_MAP(CPropertyForm)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyForm diagnostics

#ifdef _DEBUG
void CPropertyForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CPropertyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPropertyForm message handlers
VOID CPropertyForm::InitializeMember()
{
	m_hBackBrush		= NULL;
	m_pcsPropertyTree	= NULL;
}

BOOL CPropertyForm::CreateMember()
{
	m_pcsPropertyTree	= new CPropertyTree(this, CRect(0, 0, 180, 555));
	return TRUE;
}

void CPropertyForm::OnDestroy() 
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	if (m_pcsPropertyTree)
	{
		delete m_pcsPropertyTree;
		m_pcsPropertyTree = NULL;
	}
}

void CPropertyForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if ((CModelToolApp::GetInstance()) && (CModelToolApp::GetInstance()->m_pMainWnd))
	{
		if (m_pcsPropertyTree)
		{
			CRect rtTemp;
			GetClientRect(rtTemp);

			RECT rtTreeRect = {0, 0, rtTemp.Width(), rtTemp.Height()};
			m_pcsPropertyTree->MoveWindow(&rtTreeRect);
		}
	}
}
