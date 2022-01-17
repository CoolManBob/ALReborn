// AnimationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modeltool.h"
#include "AnimationDlg.h"
//#include "PropertyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimationDlg dialog
CAnimationDlg::CAnimationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnimationDlg)
	m_unCurTime = 0;
	m_nSliderAnimTime = 0;
	m_bAnimPlay = (CModelToolApp::GetInstance()) ? (CModelToolApp::GetInstance()->IsPlayCharacterAnim()) : (TRUE);
	m_bAnimStop = (CModelToolApp::GetInstance()) ? (!CModelToolApp::GetInstance()->IsPlayCharacterAnim()) : (FALSE);
	//}}AFX_DATA_INIT
}


void CAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnimationDlg)
	DDX_Text(pDX, IDC_EDIT_ANIM_CUR_TIME, m_unCurTime);
	DDX_Slider(pDX, IDC_SLIDER_ANIM_TIME, m_nSliderAnimTime);
	DDX_Check(pDX, IDC_CHECK_ANIM_PLAY, m_bAnimPlay);
	DDX_Check(pDX, IDC_CHECK_ANIM_STOP, m_bAnimStop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnimationDlg, CDialog)
	//{{AFX_MSG_MAP(CAnimationDlg)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_ANIM_PLAY, OnCheckAnimPlay)
	ON_BN_CLICKED(IDC_CHECK_ANIM_STOP, OnCheckAnimStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimationDlg message handlers
BOOL CAnimationDlg::Create(CWnd* pParentWnd/* = NULL */) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void CAnimationDlg::OnOK() 
{
	// TODO: Add extra validation here
	CModelToolDlg::GetInstance()->CloseAnimationOptionDlg();
//	CDialog::OnOK();
}

void CAnimationDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CModelToolDlg::GetInstance()->CloseAnimationOptionDlg();
//	CDialog::OnCancel();
}

void CAnimationDlg::OnDestroy() 
{
//	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

VOID CAnimationDlg::UpdateAnimTime(INT32 lTime)
{
	m_unCurTime			= lTime;
	m_nSliderAnimTime	= lTime;

	UpdateData(FALSE);
}

VOID CAnimationDlg::SetAnimRange(INT32 lMin, INT32 lMax)
{
	((CSliderCtrl *)(GetDlgItem(IDC_SLIDER_ANIM_TIME)))->SetRange(lMin, lMax);
}

void CAnimationDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default	
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	m_unCurTime			= ((CSliderCtrl *)(GetDlgItem(IDC_SLIDER_ANIM_TIME)))->GetPos();
	m_nSliderAnimTime	= m_unCurTime;
	UpdateData(FALSE);

	if(CModelToolApp::GetInstance())
	{
		CModelToolApp::GetInstance()->SetCharacterAnimTime(m_unCurTime);
	}
}


BOOL CAnimationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_SLIDER_ANIM_TIME)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnimationDlg::OnCheckAnimPlay() 
{
	// TODO: Add your control notification handler code here
	m_bAnimPlay = TRUE;
	m_bAnimStop = FALSE;

	GetDlgItem(IDC_SLIDER_ANIM_TIME)->EnableWindow(FALSE);
	CModelToolApp::GetInstance()->StopCharacterAnim(FALSE);

	UpdateData(FALSE);
}

void CAnimationDlg::OnCheckAnimStop() 
{
	// TODO: Add your control notification handler code here
	m_bAnimPlay = FALSE;
	m_bAnimStop = TRUE;

	CModelToolApp::GetInstance()->StopCharacterAnim(TRUE);
	GetDlgItem(IDC_SLIDER_ANIM_TIME)->EnableWindow(TRUE);

	UpdateData(FALSE);
}
