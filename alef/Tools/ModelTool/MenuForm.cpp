// MenuForm.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "MenuForm.h"
#include "EditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMenuForm

IMPLEMENT_DYNCREATE(CMenuForm, CFormView)

CMenuForm::CMenuForm() : CFormView(CMenuForm::IDD)
{
	//{{AFX_DATA_INIT(CMenuForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMenuForm::CMenuForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor) : CFormView(CMenuForm::IDD)
{
	InitializeMember();
	m_hBackBrush = hBackColor;

	Create(NULL, NULL, (WS_CHILD | WS_VISIBLE), stInitRect, pcsParent, NULL, NULL);
	CreateMember();
}

CMenuForm::~CMenuForm()
{
}

void CMenuForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMenuForm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMenuForm, CFormView)
	//{{AFX_MSG_MAP(CMenuForm)
	ON_WM_DESTROY()
	ON_COMMAND(ID_AMT_MENU_TOOLBAR_SAVE, OnCommandPressSaveButton)
	ON_COMMAND(ID_AMT_MENU_TOOLBAR_TEST, OnCommandPressColorButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMenuForm diagnostics

#ifdef _DEBUG
void CMenuForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CMenuForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMenuForm message handlers

VOID CMenuForm::InitializeMember()
{
	m_pcsMenuToolBar		= NULL;
	m_pcsMenuToolBarBitmap	= NULL;
	m_pcsMenuToolTip		= NULL;
}

BOOL CMenuForm::CreateMember()
{
	m_pcsMenuToolBar		= new CToolBarCtrl();
	m_pcsMenuToolBarBitmap	= new CBitmap();
	m_pcsMenuToolTip		= new CToolTipCtrl();

	if( !m_pcsMenuToolBar->Create( WS_CHILD | WS_VISIBLE | TBSTYLE_TRANSPARENT | TBSTYLE_FLAT, CRect(0, 0, 100, 100), this, NULL ) )
		return FALSE;
	if( !m_pcsMenuToolTip->Create(m_pcsMenuToolBar) )
		return FALSE;

	TBBUTTON button = { NULL, 0, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, NULL };
	button.iBitmap = 0;
	m_pcsMenuToolBar->InsertButton(0, &button);
	button.iBitmap = 1;
	m_pcsMenuToolBar->InsertButton(1, &button);

	m_pcsMenuToolBar->SetCmdID(0, ID_AMT_MENU_TOOLBAR_SAVE);
	m_pcsMenuToolBar->SetCmdID(1, ID_AMT_MENU_TOOLBAR_TEST);

	m_pcsMenuToolBarBitmap->LoadBitmap( IDB_MENU_TOOLBAR2 );
	m_pcsMenuToolBar->AddBitmap( 5, m_pcsMenuToolBarBitmap );

	m_pcsMenuToolTip->AddTool( m_pcsMenuToolBar, AMT_MENU_TOOL_TIP_TEXT_SAVE, CRect(0, 0, 16, 16), ID_AMT_MENU_TOOLBAR_SAVE) ;
	m_pcsMenuToolTip->AddTool( m_pcsMenuToolBar, "ColorDlg", CRect(0, 0, 16, 16), ID_AMT_MENU_TOOLBAR_TEST );

	m_pcsMenuToolBar->SetToolTips(m_pcsMenuToolTip);

	return TRUE;
}

void CMenuForm::OnDestroy() 
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_pcsMenuToolBarBitmap)
	{
		delete m_pcsMenuToolBarBitmap;
		m_pcsMenuToolBarBitmap = NULL;
	}

	if(m_pcsMenuToolBar)
	{
		delete m_pcsMenuToolBar;
		m_pcsMenuToolBar = NULL;
	}

	if(m_pcsMenuToolTip)
	{
		delete m_pcsMenuToolTip;
		m_pcsMenuToolTip = NULL;
	}
}

void CMenuForm::OnCommandPressSaveButton()
{
	if( !CModelToolApp::GetInstance()->SaveData() )
	{
		OutputDebugString("!\n!\nERROR <CMenuForm::OnCommandPressSaveButton() - 저장할 수 없습니다.>\n!\n!\n");
		MessageBox( "저장 실패!", "ERROR", MB_OK );
	}
}

void CMenuForm::OnCommandPressColorButton()
{
	CRenderWare* pRenderWare = CModelToolDlg::GetInstance()->GetRenderForm()->GetRenderWare();

	CColorDialog	cDlg;
	//cDlg.m_cc.lpCustColors	= m_CustomColors;
	cDlg.m_cc.Flags			|= CC_RGBINIT | CC_FULLOPEN;
	cDlg.m_cc.rgbResult		= RGB( pRenderWare->GetColor().red, pRenderWare->GetColor().green, pRenderWare->GetColor().blue );

	if( IDOK == cDlg.DoModal()  )
	{
		COLORREF color = cDlg.GetColor();
		pRenderWare->SetClearColor( GetRValue( color ), GetGValue( color ), GetBValue( color ) );
	}
}