// StatusListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeffectdlg.h"
#include "../resource.h"
#include "StatusListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusListDlg dialog


CStatusListDlg::CStatusListDlg(UINT32 *plStatus, CWnd* pParent /*=NULL*/)
	: CDialog(CStatusListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatusListDlg)
	m_bDirection = FALSE;
	m_bHitEffect = FALSE;
	m_bOnlyTarget = FALSE;
	m_unStatus = 0;
	m_bLoop = FALSE;
	m_bAnimLoop = FALSE;
	m_bFirstTarget = FALSE;
	m_bCalcDist = FALSE;
	//}}AFX_DATA_INIT

	m_plStatus = plStatus;
	UpdateStatusButton(FALSE);
}


void CStatusListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatusListDlg)
	DDX_Check(pDX, IDC_CHECK_ES_SET_DIRECTION, m_bDirection);
	DDX_Check(pDX, IDC_CHECK_ES_SET_HIT_EFFECT, m_bHitEffect);
	DDX_Check(pDX, IDC_CHECK_ES_SET_ONLY_TARGET, m_bOnlyTarget);
	DDX_Text(pDX, IDC_EDIT_STATUS, m_unStatus);
	DDX_Check(pDX, IDC_CHECK_ES_SET_LOOP, m_bLoop);
	DDX_Check(pDX, IDC_CHECK_ES_SET_ANIM_LOOP, m_bAnimLoop);
	DDX_Check(pDX, IDC_CHECK_ES_SET_FIRST_TARGET, m_bFirstTarget);
	DDX_Check(pDX, IDC_CHECK_ES_SET_CALC_DIST, m_bCalcDist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatusListDlg, CDialog)
	//{{AFX_MSG_MAP(CStatusListDlg)
	ON_BN_CLICKED(IDC_BUTTON_INPUT, OnButtonInput)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT, OnButtonOutput)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusListDlg message handlers

void CStatusListDlg::OnButtonInput() 
{
	// TODO: Add your control notification handler code here
/*	UpdateData(TRUE);
	InitializeStatusButton();

	if (m_unStatus & AGCDEVENTEFFECT_OPTION_FLAG_DIRECTION)
		m_bDirection = TRUE;
	if (m_unStatus & AGCDEVENTEFFECT_OPTION_FLAG_HIT_EFFECT)
		m_bHitEffect = TRUE;
	if (m_unStatus & AGCDEVENTEFFECT_OPTION_FLAG_ONLY_TARGET)
		m_bOnlyTarget = TRUE;
	if (m_unStatus & AGCDEVENTEFFECT_OPTION_FLAG_LOOP)
		m_bLoop = TRUE;
	if (m_unStatus & AGCDEVENTEFFECT_OPTION_FLAG_ANIM_LOOP)
		m_bAnimLoop = TRUE;
	if (m_unStatus & AGCDEVENTEFFECT_OPTION_FLAG_FIRST_TARGET)
		m_bFirstTarget = TRUE;
	if (m_unStatus & AGCDEVENTEFFECT_OPTION_FLAG_CALC_DISTANCE)
		m_bCalcDist = TRUE;

	UpdateData(FALSE);*/
}

void CStatusListDlg::OnButtonOutput() 
{
	// TODO: Add your control notification handler code here
/*	UpdateData(TRUE);
	m_unStatus = 0;

	if (m_bDirection)
		m_unStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_DIRECTION;

	if (m_bHitEffect)
		m_unStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_HIT_EFFECT;

	if (m_bOnlyTarget)
		m_unStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_ONLY_TARGET;

	if (m_bLoop)
		m_unStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_LOOP;

	if (m_bAnimLoop)
		m_unStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_ANIM_LOOP;

	if (m_bFirstTarget)
		m_unStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_FIRST_TARGET;

	if (m_bCalcDist)
		m_unStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_CALC_DISTANCE;

	UpdateData(FALSE);*/
}

void CStatusListDlg::OnOK() 
{
	// TODO: Add extra validation here
	if (AgcmEffectDlg::GetInstance())
		AgcmEffectDlg::GetInstance()->CloseEffectStatusList();

//	CDialog::OnOK();
}

BOOL CStatusListDlg::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void CStatusListDlg::OnDestroy() 
{
//	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

VOID CStatusListDlg::InitializeStatusButton()
{
	m_bDirection = FALSE;
	m_bHitEffect = FALSE;
	m_bOnlyTarget = FALSE;
	m_bLoop = FALSE;
	m_bAnimLoop = FALSE;
	m_bFirstTarget = FALSE;
	m_bCalcDist = FALSE;
}

void CStatusListDlg::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	if (!AgcmEffectDlg::GetInstance())
		return;

	UpdateStatus();

	AgcmEffectDlg::GetInstance()->UpdateEffectScript();
//	AgcmEffectDlg::GetInstance()->_EnumCallback(AGCM_EFFECTDLG_CB_ID_APPLY_STATUS, NULL, NULL);
}

VOID CStatusListDlg::UpdateStatusButton(BOOL bUpdateData)
{
/*	InitializeStatusButton();

	if (*m_plStatus & AGCDEVENTEFFECT_OPTION_FLAG_DIRECTION)
		m_bDirection = TRUE;
	if (*m_plStatus & AGCDEVENTEFFECT_OPTION_FLAG_HIT_EFFECT)
		m_bHitEffect = TRUE;
	if (*m_plStatus & AGCDEVENTEFFECT_OPTION_FLAG_ONLY_TARGET)
		m_bOnlyTarget = TRUE;
	if (*m_plStatus & AGCDEVENTEFFECT_OPTION_FLAG_LOOP)
		m_bLoop = TRUE;
	if (*m_plStatus & AGCDEVENTEFFECT_OPTION_FLAG_ANIM_LOOP)
		m_bAnimLoop = TRUE;
	if (*m_plStatus & AGCDEVENTEFFECT_OPTION_FLAG_FIRST_TARGET)
		m_bFirstTarget = TRUE;
	if (*m_plStatus & AGCDEVENTEFFECT_OPTION_FLAG_CALC_DISTANCE)
		m_bCalcDist = TRUE;

	if (bUpdateData)
		UpdateData(FALSE);*/
}

VOID CStatusListDlg::UpdateStatus()
{
/*	UpdateData(TRUE);
	*m_plStatus = 0;

	if (m_bDirection)
		*m_plStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_DIRECTION;

	if (m_bHitEffect)
		*m_plStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_HIT_EFFECT;

	if (m_bOnlyTarget)
		*m_plStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_ONLY_TARGET;

	if (m_bLoop)
		*m_plStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_LOOP;

	if (m_bAnimLoop)
		*m_plStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_ANIM_LOOP;

	if (m_bFirstTarget)
		*m_plStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_FIRST_TARGET;

	if (m_bCalcDist)
		*m_plStatus			|= AGCDEVENTEFFECT_OPTION_FLAG_CALC_DISTANCE;	*/
}

VOID CStatusListDlg::UpdateStatusDlg(UINT32 *plStatus)
{
	m_plStatus = plStatus;

	UpdateStatusButton();
}

VOID CStatusListDlg::EnableApplyButton(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(bEnable);
}

void CStatusListDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialog::OnCancel();
}
