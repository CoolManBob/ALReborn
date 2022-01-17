// ModelToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "ModelToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CModelToolDlg *g_pcsModelToolDlg = NULL;

/////////////////////////////////////////////////////////////////////////////
// CModelToolDlg dialog
CModelToolDlg *CModelToolDlg::GetInstance()
{
	return g_pcsModelToolDlg;
}

CModelToolDlg::CModelToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelToolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_pcsModelToolDlg = this;

	InitializeMember();
}

void CModelToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelToolDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModelToolDlg, CDialog)
	//{{AFX_MSG_MAP(CModelToolDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelToolDlg message handlers

BOOL CModelToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	if (!CreateMember())
	{
		return FALSE;
	}

	GetClientRect( m_rtToolDlg );

	// Intensive Crash
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CModelToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CModelToolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

VOID CModelToolDlg::InitializeMember()
{
	m_hBlackBrush			= NULL;
	m_hWhiteBrush			= NULL;

	m_pcsRenderForm			= NULL;
	m_pcsTitleForm			= NULL;
	m_pcsMenuForm			= NULL;
	m_pcsPropertyForm		= NULL;
	m_pcsToolForm			= NULL;
	m_pcsResourceForm		= NULL;

	m_pcsEditEquipmentsDlg	= NULL;
	m_pcsAnimationDlg		= NULL;
	m_pcsAttachFaceDlg		= NULL;
	m_pcsAttachHairDlg		= NULL;

	m_pcsCustomizePreviewDlg= NULL;

	m_pcsCharRideDlg		= NULL;
}

BOOL CModelToolDlg::CreateMember()
{
	// 배경 브러쉬를 맹근당...
	m_hWhiteBrush		= CreateSolidBrush((RGB(255, 255, 255)));
	m_hBlackBrush		= CreateSolidBrush((RGB(0, 0, 0)));

	// Window Size( 5, 5, 970, 640 )
	m_rtMenuForm		= CRect( 5, 5, (5 + 180), (5 + 30) );
	m_rtPropertyForm	= CRect( 5, 40, (5 + 180), (40 + 555) );
	m_rtTitleForm		= CRect( 190, 5, (190 + 575), (5 + 30) );
	m_rtRenderForm		= CRect( 190, 40, (190 + 575), (40 + 555) );
	m_rtToolForm		= CRect( 5, 600, (210 + 555), (600 + 40) );
	m_rtResourceForm	= CRect( 770, 5, (770 + 200), (5 + 635) );
	
	// 부모 : this
	m_pcsMenuForm		= new CMenuForm( this, m_rtMenuForm);
	m_pcsPropertyForm	= new CPropertyForm( this, m_rtPropertyForm);
	m_pcsTitleForm		= new CTitleForm( this,	m_rtTitleForm);
	m_pcsRenderForm		= new CRenderForm( this, m_rtRenderForm);
	m_pcsToolForm		= new CToolForm( this, m_rtToolForm);
	m_pcsResourceForm	= new CResourceForm( this, m_rtResourceForm);

	return TRUE;
}

VOID CModelToolDlg::ReleaseAll()
{
	if(m_hWhiteBrush)
	{
		DeleteObject(m_hWhiteBrush);
		m_hWhiteBrush = NULL;
	}

	if(m_hBlackBrush)
	{
		DeleteObject(m_hBlackBrush);
		m_hBlackBrush = NULL;
	}

	if(m_pcsRenderForm)
	{
		m_pcsRenderForm->DestroyWindow();
//		delete m_pcsRenderForm;
		m_pcsRenderForm = NULL;
	}

	if(m_pcsTitleForm)
	{
		m_pcsTitleForm->DestroyWindow();
//		delete m_pcsTitleForm;
		m_pcsTitleForm = NULL;
	}

	if(m_pcsMenuForm)
	{
		m_pcsMenuForm->DestroyWindow();
//		delete m_pcsMenuForm;
		m_pcsMenuForm = NULL;
	}

	if(m_pcsPropertyForm)
	{
		m_pcsPropertyForm->DestroyWindow();
//		delete m_pcsPropertyForm;
		m_pcsPropertyForm = NULL;
	}

	if(m_pcsToolForm)
	{
		m_pcsToolForm->DestroyWindow();
//		delete m_pcsToolForm;
		m_pcsToolForm = NULL;
	}

	if(m_pcsResourceForm)
	{
		m_pcsResourceForm->DestroyWindow();
//		delete m_pcsResourceForm;
		m_pcsResourceForm = NULL;
	}
}

BOOL CModelToolDlg::Create(CWnd* pParentWnd)
{
	return CDialog::Create(IDD, pParentWnd);
}

void CModelToolDlg::OnClose() 
{
	CModelToolApp::GetInstance()->Fin();
//	CDialog::OnClose();
}

BOOL CModelToolDlg::UpdateAll()
{
	if(m_pcsRenderForm)
		m_pcsRenderForm->UpdateRenderForm();

	if( IsOpenCustomizePreviewDlg() )
		GetCustomizePreviewDlg()->OnIdle();

	return TRUE;
}

void CModelToolDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(CModelToolApp::GetInstance())
	{
		if(CModelToolApp::GetInstance()->m_pMainWnd)
		{
			//CRect rcApp;
			//GetClientRect( rcApp );

			INT16 nCX = cx - m_rtToolDlg.Width();
			INT16 nCY = cy - m_rtToolDlg.Height();

			m_rtRenderForm.right	+= nCX;
			m_rtRenderForm.bottom	+= nCY;

			m_rtPropertyForm.bottom	+= nCY;

			m_rtToolForm.top		+= nCY;
			m_rtToolForm.right		+= nCX;
			m_rtToolForm.bottom		+= nCY;

			m_rtTitleForm.right		+= nCX;

			m_rtResourceForm.left	+= nCX;
			m_rtResourceForm.right	+= nCX;
			m_rtResourceForm.bottom	+= nCY;

			m_pcsRenderForm->MoveWindow( CRect( m_rtRenderForm.left, m_rtRenderForm.top, m_rtRenderForm.right, m_rtRenderForm.bottom ) );
			m_pcsPropertyForm->MoveWindow( CRect( m_rtPropertyForm.left, m_rtPropertyForm.top, m_rtPropertyForm.right, m_rtPropertyForm.bottom ) );
			m_pcsToolForm->MoveWindow( CRect(m_rtToolForm.left, m_rtToolForm.top, m_rtToolForm.right, m_rtToolForm.bottom ) );
			m_pcsTitleForm->MoveWindow( CRect( m_rtTitleForm.left, m_rtTitleForm.top, m_rtTitleForm.right, m_rtTitleForm.bottom ) );
			m_pcsResourceForm->MoveWindow( CRect( m_rtResourceForm.left, m_rtResourceForm.top, m_rtResourceForm.right, m_rtResourceForm.bottom ) );

			m_rtToolDlg.right	+= nCX;
			m_rtToolDlg.bottom	+= nCY;
		}

		CModelToolApp::GetInstance()->m_bActiveWindows = SIZE_MINIMIZED == nType ? FALSE : TRUE;
	}
}

VOID CModelToolDlg::ResizeResourceForm( DWORD dwWidth )
{
	if( !dwWidth )	return;

	DWORD dwCurWidth = m_rtResourceForm.right - m_rtResourceForm.left;
	INT32 dwOffset = dwWidth - dwCurWidth;
	if( !dwOffset )	return;

	m_rtTitleForm.right		-= dwOffset;
	m_rtRenderForm.right	-= dwOffset;
	m_rtToolForm.right		-= dwOffset;
	m_rtResourceForm.left	-= dwOffset;

	m_pcsRenderForm->MoveWindow( CRect( m_rtRenderForm.left, m_rtRenderForm.top, m_rtRenderForm.right, m_rtRenderForm.bottom ) );
	m_pcsPropertyForm->MoveWindow( CRect( m_rtPropertyForm.left, m_rtPropertyForm.top, m_rtPropertyForm.right, m_rtPropertyForm.bottom ) );
	m_pcsToolForm->MoveWindow( CRect(m_rtToolForm.left, m_rtToolForm.top, m_rtToolForm.right, m_rtToolForm.bottom ) );
	m_pcsTitleForm->MoveWindow( CRect( m_rtTitleForm.left, m_rtTitleForm.top, m_rtTitleForm.right, m_rtTitleForm.bottom ) );
	m_pcsResourceForm->MoveWindow( CRect( m_rtResourceForm.left, m_rtResourceForm.top, m_rtResourceForm.right, m_rtResourceForm.bottom ) );
}

VOID CModelToolDlg::OpenEditEquipmentsDlg()
{
	if(!m_pcsEditEquipmentsDlg)
	{
		m_pcsEditEquipmentsDlg = new CEditEquipmentsDlg();

		m_pcsEditEquipmentsDlg->Create();
		m_pcsEditEquipmentsDlg->ShowWindow(SW_SHOW);
		m_pcsEditEquipmentsDlg->SetFocus();
	}
}

VOID CModelToolDlg::CloseEditEquipmentsDlg()
{
	if(m_pcsEditEquipmentsDlg)
	{
		m_pcsEditEquipmentsDlg->ShowWindow(SW_HIDE);
		m_pcsEditEquipmentsDlg->DestroyWindow();

		delete m_pcsEditEquipmentsDlg;
		m_pcsEditEquipmentsDlg = NULL;
	}
}

BOOL CModelToolDlg::EditEquipmentsDlgIsPickMode()
{
	if(!IsOpenEditEquipmentsDlg())
		return FALSE;

	return m_pcsEditEquipmentsDlg->IsPickMode();
}

BOOL CModelToolDlg::EditEquipmentsDlgIsMouseMode()
{
	if(!IsOpenEditEquipmentsDlg())
		return FALSE;

	return m_pcsEditEquipmentsDlg->IsMouseMode();
}

INT32 CModelToolDlg::EditEquipmentsDlgGetMouseMode()
{
	if(!IsOpenEditEquipmentsDlg())
		return FALSE;

	return m_pcsEditEquipmentsDlg->GetMouseMode();
}

INT32 CModelToolDlg::EditEquipmentsDlgGetMouseModeAxis()
{
	if(!IsOpenEditEquipmentsDlg())
		return FALSE;

	return m_pcsEditEquipmentsDlg->GetMouseModeAxis();
}

BOOL CModelToolDlg::OpenAnimationOptionDlg()
{
	if( m_pcsAnimationDlg )
		return TRUE;

	m_pcsAnimationDlg = new CAnimationDlg();
	m_pcsAnimationDlg->Create();
	m_pcsAnimationDlg->ShowWindow(SW_SHOW);

	INT32	lAnimDuration = 0;
	if(CModelToolApp::GetInstance())
		lAnimDuration = CModelToolApp::GetInstance()->GetCharacterCurrentAnimDuration();

	if(lAnimDuration)
		SetAnimRange(0, lAnimDuration);

	return TRUE;
}

BOOL CModelToolDlg::CloseAnimationOptionDlg()
{
	if( m_pcsAnimationDlg )
	{
		m_pcsAnimationDlg->ShowWindow(SW_HIDE);
		m_pcsAnimationDlg->DestroyWindow();

		delete m_pcsAnimationDlg;
		m_pcsAnimationDlg = NULL;
	}

	return TRUE;
}

VOID CModelToolDlg::UpdateAnimTime(INT32 lTime)
{
	if( m_pcsAnimationDlg )
		m_pcsAnimationDlg->UpdateAnimTime(lTime);
}

VOID CModelToolDlg::SetAnimRange(INT32 lMin, INT32 lMax)
{
	if(!m_pcsAnimationDlg)
		return;

	m_pcsAnimationDlg->SetAnimRange(lMin, lMax);
}

VOID CModelToolDlg::OpenAttachFaceDlg()
{
	if( !m_pcsAttachFaceDlg )
	{
		m_pcsAttachFaceDlg = new CAttachFaceDlg;
		m_pcsAttachFaceDlg->Create();
		m_pcsAttachFaceDlg->ShowWindow(SW_SHOW);
		m_pcsAttachFaceDlg->SetDefaultFace();
	}
}

VOID CModelToolDlg::CloseAttachFaceDlg()
{
	if( m_pcsAttachFaceDlg )
	{
		m_pcsAttachFaceDlg->ShowWindow(SW_HIDE);
		m_pcsAttachFaceDlg->DestroyWindow();

		delete m_pcsAttachFaceDlg;
		m_pcsAttachFaceDlg = NULL;
	}
}

VOID CModelToolDlg::OpenAttachHairDlg()
{
	if( !m_pcsAttachHairDlg )
	{
		m_pcsAttachHairDlg = new CAttachHairDlg;
		m_pcsAttachHairDlg->Create();
		m_pcsAttachHairDlg->ShowWindow(SW_SHOW);
		m_pcsAttachHairDlg->SetDefaultHair();
	}
}

VOID CModelToolDlg::CloseAttachHairDlg()
{
	if( m_pcsAttachHairDlg )
	{
		m_pcsAttachHairDlg->ShowWindow(SW_HIDE);
		m_pcsAttachHairDlg->DestroyWindow();

		delete m_pcsAttachHairDlg;
		m_pcsAttachHairDlg = NULL;
	}
}

VOID CModelToolDlg::OpenCustomizePreviewDlg()
{
	if( m_pcsCustomizePreviewDlg )		return;

	int nID = CModelToolApp::GetInstance()->GetCurrentID();

	AgpmCharacter* pAgpmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgpmCharacterModule();
	AgcmCharacter* pAgcmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule();

	AgpdCharacter *pcsAgpdCharacter	= pAgpmCharacter->GetCharacter( nID );
	if( !pcsAgpdCharacter )		return;
		
	AgcdCharacter *pcsAgcdCharacter = pAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter )		return;

	if( pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_vpFace.empty() )
		return;

	m_pcsCustomizePreviewDlg = new CCustomizePreviewDlg;
	m_pcsCustomizePreviewDlg->Create();
	m_pcsCustomizePreviewDlg->ShowWindow(SW_SHOW);
	m_pcsCustomizePreviewDlg->Start();
}

VOID CModelToolDlg::CloseCustomizePreviewDlg()
{
	if( m_pcsCustomizePreviewDlg )
	{
		m_pcsCustomizePreviewDlg->End();
		m_pcsCustomizePreviewDlg->ShowWindow(SW_HIDE);
		m_pcsCustomizePreviewDlg->DestroyWindow();

		delete m_pcsCustomizePreviewDlg;
		m_pcsCustomizePreviewDlg = NULL;
	}
}

VOID CModelToolDlg::OpenRideDlg()
{
	if( !m_pcsCharRideDlg )
	{
		m_pcsCharRideDlg = new CCharRideDlg;
		m_pcsCharRideDlg->Create();
		m_pcsCharRideDlg->ShowWindow(SW_SHOW);
	}
}

VOID CModelToolDlg::CloseRideDlg()
{
	if( m_pcsCharRideDlg )
	{
		m_pcsCharRideDlg->ShowWindow( SW_HIDE );
		m_pcsCharRideDlg->DestroyWindow();

		delete m_pcsCharRideDlg;
		m_pcsCharRideDlg = NULL;
	}
}