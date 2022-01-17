// ToolForm.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "ToolForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolForm

IMPLEMENT_DYNCREATE(CToolForm, CFormView)

CToolForm::CToolForm()
	: CFormView(CToolForm::IDD)
{
	InitializeMember();
	//{{AFX_DATA_INIT(CToolForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CToolForm::CToolForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor)
	: CFormView(CToolForm::IDD)
{
	InitializeMember();
	m_hBackBrush = hBackColor;

	Create(NULL, NULL, (WS_CHILD | WS_VISIBLE), stInitRect, pcsParent, NULL, NULL);

	CreateMember();
}

CToolForm::~CToolForm()
{
}

void CToolForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolForm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolForm, CFormView)
	//{{AFX_MSG_MAP(CToolForm)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolForm diagnostics

#ifdef _DEBUG
void CToolForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CToolForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CToolForm message handlers

VOID CToolForm::InitializeMember()
{
	m_nXTemp			= 0;
	m_nYTemp			= 0;
	m_nWidth			= 0;
	m_nHeight			= 0;
	m_nToolButtonIndex	= 0;
	m_pcsOffset			= NULL;
	m_pcsRenderOption	= NULL;
	m_pcsAnimation		= NULL;
	m_pcsResetView		= NULL;
}

BOOL CToolForm::CreateMember()
{
	m_pcsOffset			= new CEventButton(this, CRect(0, 0, 10, 10), TFTB_NAME_SET_OFFSET,			TRUE, PressSetOffsetButton);
	m_pcsRenderOption	= new CEventButton(this, CRect(0, 0, 10, 10), TFTB_NAME_SET_RENDER_OPTION,	TRUE, PressSetRenderOptionButton);
	m_pcsAnimation		= new CEventButton(this, CRect(0, 0, 10, 10), TFTB_NAME_SET_ANIMATION,		TRUE, PressSetAnimationButton);
	m_pcsResetView		= new CEventButton(this, CRect(0, 0, 10, 10), TFTB_NAME_RESET_VIEW,			TRUE, PressResetViewButton);

	HideAllToolButton();

	return TRUE;
}

VOID CToolForm::InitToolButton(INT16 nXTemp, INT16 nYTemp, INT16 nWidth, INT16 nHeight)
{
	HideAllToolButton();

	m_nXTemp			= nXTemp;
	m_nYTemp			= nYTemp;
	m_nWidth			= nWidth;
	m_nHeight			= nHeight;

	m_nToolButtonIndex	= 0;
}

VOID CToolForm::SetPropertyButton(INT32 lFlag)
{
	if(lFlag & TFTB_SET_OFFSET)
	{
		AddToolButton(m_pcsOffset);
	}

	if(lFlag & TFTB_SET_RENDER_OPTION)
	{
		AddToolButton(m_pcsRenderOption);
	}

	if(lFlag & TFTB_SET_ANIMATION)
	{
		AddToolButton(m_pcsAnimation);
	}

	if(lFlag & TFTB_RESET_VIEW)
	{
		AddToolButton(m_pcsResetView);
	}
}

void CToolForm::OnDestroy() 
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_pcsOffset)
	{
		delete m_pcsOffset;
		m_pcsOffset = NULL;
	}

	if(m_pcsRenderOption)
	{
		delete m_pcsRenderOption;
		m_pcsRenderOption = NULL;
	}

	if(m_pcsAnimation)
	{
		delete m_pcsAnimation;
		m_pcsAnimation = NULL;
	}

	if(m_pcsResetView)
	{
		delete m_pcsResetView;
		m_pcsResetView = NULL;
	}
}

VOID CToolForm::AddToolButton(CEventButton *pcsButton)
{
	RECT rtRect = {	((m_nXTemp * (m_nToolButtonIndex + 1)) + (m_nWidth * m_nToolButtonIndex)),
					(m_nYTemp),
					((m_nXTemp * (m_nToolButtonIndex + 1)) + (m_nWidth * (m_nToolButtonIndex + 1))),
					(m_nYTemp + m_nHeight)														};

	pcsButton->MoveWindow(&rtRect);
	pcsButton->_ShowButton();

	++m_nToolButtonIndex;
}

VOID CToolForm::HideAllToolButton()
{
	m_pcsOffset->_HideButton();
	m_pcsRenderOption->_HideButton();
	m_pcsAnimation->_HideButton();
	m_pcsResetView->_HideButton();
}

BOOL CToolForm::PressSetOffsetButton(PVOID pClass)
{
	if(!CModelToolApp::GetInstance())
	{
		OutputDebugString("!\n!\nERROR <CToolForm::PressSetOffsetButton() - 초기화할 수 없습니다.>\n!\n!\n");
		return FALSE;
	}

	if(!CModelToolApp::GetInstance()->F_SetOffset())
	{
		OutputDebugString("!\n!\nERROR <CToolForm::PressSetOffsetButton() - F_SetOffset() 실패>\n!\n!\n");
		CModelToolDlg::GetInstance()->MessageBox("Offset을 설정할 수 없습니다.", "ERROR", MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL CToolForm::PressSetRenderOptionButton(PVOID pClass)
{
	if(!CModelToolApp::GetInstance())
	{
		OutputDebugString("!\n!\nERROR <CToolForm::PressSetRenderOptionButton() - 초기화할 수 없습니다.>\n!\n!\n");
		return FALSE;
	}

	if(!CModelToolApp::GetInstance()->F_SetRenderOption())
	{
		OutputDebugString("!\n!\nERROR <CToolForm::PressSetRenderOptionButton() - F_SetRenderOption() 실패>\n!\n!\n");
		CModelToolDlg::GetInstance()->MessageBox("RenderOption을 설정할 수 없습니다.", "ERROR", MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL CToolForm::PressSetAnimationButton(PVOID pClass)
{
	if(!CModelToolApp::GetInstance())
	{
		OutputDebugString("!\n!\nERROR <CToolForm::PressSetAnimationButton() - 초기화할 수 없습니다.>\n!\n!\n");
		return FALSE;
	}

	if(!CModelToolApp::GetInstance()->F_SetAnimationOption())
	{
		OutputDebugString("!\n!\nERROR <CToolForm::PressSetAnimationButton() - F_SetAnimationOption() 실패>\n!\n!\n");
		CModelToolDlg::GetInstance()->MessageBox("Animation을 설정할 수 없습니다.", "ERROR", MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL CToolForm::PressResetViewButton(PVOID pClass)
{
	if(!CModelToolApp::GetInstance())
	{
		OutputDebugString("!\n!\nERROR <CToolForm::PressResetViewButton() - 초기화할 수 없습니다.>\n!\n!\n");
		return FALSE;
	}

	if(!CModelToolApp::GetInstance()->F_ResetView())
	{
		OutputDebugString("!\n!\nERROR <CToolForm::PressResetViewButton() - F_ResetView() 실패>\n!\n!\n");
		CModelToolDlg::GetInstance()->MessageBox("Animation을 설정할 수 없습니다.", "ERROR", MB_OK);
		return FALSE;
	}

	return TRUE;
}
