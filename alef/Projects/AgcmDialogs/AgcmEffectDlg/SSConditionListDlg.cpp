// SSConditionListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeffectdlg.h"
#include "../resource.h"
#include "ConditionListDlg.h"
#include "SSConditionListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSSConditionListDlg dialog


CSSConditionListDlg::CSSConditionListDlg(UINT32 *plCondition, CWnd* pParent /*=NULL*/)
	: CDialog(CSSConditionListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSSConditionListDlg)
	m_unCondition = 0;
	m_bSSAir = FALSE;
	m_bSSAirTarget = FALSE;
	m_bSSEarth = FALSE;
	m_bSSEarthTarget = FALSE;
	m_bSSFire = FALSE;
	m_bSSFireTarget = FALSE;
	m_bSSIce = FALSE;
	m_bSSIceTarget = FALSE;
	m_bSSLightening = FALSE;
	m_bSSLighteningTarget = FALSE;
	m_bSSMagic = FALSE;
	m_bSSMagicTarget = FALSE;
	m_bSSPoison = FALSE;
	m_bSSPoisonTarget = FALSE;
	m_bSSWater = FALSE;
	m_bSSWaterTarget = FALSE;
	//}}AFX_DATA_INIT

	m_plCondition	= plCondition;

	UpdateConditionButton(FALSE);
}


void CSSConditionListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSSConditionListDlg)
	DDX_Text(pDX, IDC_EDIT_CONDITION, m_unCondition);
	DDX_Check(pDX, IDC_CHECK_SS_AIR, m_bSSAir);
	DDX_Check(pDX, IDC_CHECK_SS_AIR_TARGET, m_bSSAirTarget);
	DDX_Check(pDX, IDC_CHECK_SS_EARTH, m_bSSEarth);
	DDX_Check(pDX, IDC_CHECK_SS_EARTH_TARGET, m_bSSEarthTarget);
	DDX_Check(pDX, IDC_CHECK_SS_FIRE, m_bSSFire);
	DDX_Check(pDX, IDC_CHECK_SS_FIRE_TARGET, m_bSSFireTarget);
	DDX_Check(pDX, IDC_CHECK_SS_ICE, m_bSSIce);
	DDX_Check(pDX, IDC_CHECK_SS_ICE_TARGET, m_bSSIceTarget);
	DDX_Check(pDX, IDC_CHECK_SS_LIGHTENING, m_bSSLightening);
	DDX_Check(pDX, IDC_CHECK_SS_LIGHTENING_TARGET, m_bSSLighteningTarget);
	DDX_Check(pDX, IDC_CHECK_SS_MAGIC, m_bSSMagic);
	DDX_Check(pDX, IDC_CHECK_SS_MAGIC_TARGET, m_bSSMagicTarget);
	DDX_Check(pDX, IDC_CHECK_SS_POISON, m_bSSPoison);
	DDX_Check(pDX, IDC_CHECK_SS_POISON_TARGET, m_bSSPoisonTarget);
	DDX_Check(pDX, IDC_CHECK_SS_WATER, m_bSSWater);
	DDX_Check(pDX, IDC_CHECK_SS_WATER_TARGET, m_bSSWaterTarget);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSSConditionListDlg, CDialog)
	//{{AFX_MSG_MAP(CSSConditionListDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_INPUT, OnButtonInput)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT, OnButtonOutput)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSSConditionListDlg message handlers

BOOL CSSConditionListDlg::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void CSSConditionListDlg::OnDestroy() 
{
//	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CSSConditionListDlg::OnOK() 
{
	// TODO: Add extra validation here
	if (CConditionListDlg::GetInstance())
		CConditionListDlg::GetInstance()->CloseSSConditionDlg();
//	CDialog::OnOK();
}

void CSSConditionListDlg::OnButtonInput() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	InitializeConditionButton();

/*	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_FIRE)
		m_bSSFire = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_WATER)
		m_bSSWater = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_AIR)
		m_bSSAir = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_EARTH)
		m_bSSEarth = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_MAGIC)
		m_bSSMagic = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_LIGHTENING)
		m_bSSLightening = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_POISON)
		m_bSSPoison = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_ICE)
		m_bSSIce = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_FIRE_STRUCK)
		m_bSSFireTarget = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_WATER_STRUCK)
		m_bSSWaterTarget = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_AIR_STRUCK)
		m_bSSAirTarget = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_EARTH_STRUCK)
		m_bSSEarthTarget = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_MAGIC_STRUCK)
		m_bSSMagicTarget = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_LIGHTENING_STRUCK)
		m_bSSLighteningTarget = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_POISON_STRUCK)
		m_bSSPoisonTarget = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_ICE_STRUCK)
		m_bSSIceTarget = TRUE;*/

	UpdateData(FALSE);
}

void CSSConditionListDlg::OnButtonOutput() 
{
	// TODO: Add your control notification handler code here
/*	UpdateData(TRUE);
	m_unCondition = 0;

	if (m_bSSFire)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_FIRE;

	if (m_bSSWater)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_WATER;

	if (m_bSSAir)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_AIR;

	if (m_bSSEarth)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_EARTH;

	if (m_bSSMagic)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_MAGIC;

	if (m_bSSLightening)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_LIGHTENING;

	if (m_bSSPoison)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_POISON;

	if (m_bSSIce)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_ICE;

	if (m_bSSFireTarget)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_FIRE_STRUCK;

	if (m_bSSWaterTarget)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_WATER_STRUCK;

	if (m_bSSAirTarget)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_AIR_STRUCK;

	if (m_bSSEarthTarget)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_EARTH_STRUCK;

	if (m_bSSMagicTarget)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_MAGIC_STRUCK;

	if (m_bSSLighteningTarget)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_LIGHTENING_STRUCK;

	if (m_bSSPoisonTarget)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_POISON_STRUCK;

	if (m_bSSIceTarget)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_ICE_STRUCK;

	UpdateData(FALSE);*/
}

VOID CSSConditionListDlg::InitializeConditionButton() 
{
	m_bSSAir = FALSE;
	m_bSSAirTarget = FALSE;
	m_bSSEarth = FALSE;
	m_bSSEarthTarget = FALSE;
	m_bSSFire = FALSE;
	m_bSSFireTarget = FALSE;
	m_bSSIce = FALSE;
	m_bSSIceTarget = FALSE;
	m_bSSLightening = FALSE;
	m_bSSLighteningTarget = FALSE;
	m_bSSMagic = FALSE;
	m_bSSMagicTarget = FALSE;
	m_bSSPoison = FALSE;
	m_bSSPoisonTarget = FALSE;
	m_bSSWater = FALSE;
	m_bSSWaterTarget = FALSE;
}

VOID CSSConditionListDlg::UpdateConditionButton(BOOL bUpdateData)
{
/*	InitializeConditionButton();

	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_FIRE)
		m_bSSFire = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_WATER)
		m_bSSWater = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_AIR)
		m_bSSAir = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_EARTH)
		m_bSSEarth = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_MAGIC)
		m_bSSMagic = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_LIGHTENING)
		m_bSSLightening = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_POISON)
		m_bSSPoison = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_ICE)
		m_bSSIce = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_FIRE_STRUCK)
		m_bSSFireTarget = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_WATER_STRUCK)
		m_bSSWaterTarget = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_AIR_STRUCK)
		m_bSSAirTarget = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_EARTH_STRUCK)
		m_bSSEarthTarget = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_MAGIC_STRUCK)
		m_bSSMagicTarget = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_LIGHTENING_STRUCK)
		m_bSSLighteningTarget = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_POISON_STRUCK)
		m_bSSPoisonTarget = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_SS_FLAG_ICE_STRUCK)
		m_bSSIceTarget = TRUE;

	if (bUpdateData)
		UpdateData(FALSE);*/
}

VOID CSSConditionListDlg::UpdateCondition()
{
/*	UpdateData(TRUE);
	*m_plCondition				= 0;

	if (m_bSSFire)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_FIRE;

	if (m_bSSWater)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_WATER;

	if (m_bSSAir)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_AIR;

	if (m_bSSEarth)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_EARTH;

	if (m_bSSMagic)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_MAGIC;

	if (m_bSSLightening)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_LIGHTENING;

	if (m_bSSPoison)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_POISON;

	if (m_bSSIce)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_ICE;

	if (m_bSSFireTarget)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_FIRE_STRUCK;

	if (m_bSSWaterTarget)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_WATER_STRUCK;

	if (m_bSSAirTarget)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_AIR_STRUCK;

	if (m_bSSEarthTarget)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_EARTH_STRUCK;

	if (m_bSSMagicTarget)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_MAGIC_STRUCK;

	if (m_bSSLighteningTarget)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_LIGHTENING_STRUCK;

	if (m_bSSPoisonTarget)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_POISON_STRUCK;

	if (m_bSSIceTarget)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_SS_FLAG_ICE_STRUCK;*/
}

void CSSConditionListDlg::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	if (!CConditionListDlg::GetInstance())
		return;

	UpdateCondition();
	CConditionListDlg::GetInstance()->ApplySSCondition();
}

VOID CSSConditionListDlg::UpdateConditionDlg(UINT32 *plCondition)
{
	m_plCondition	= plCondition;
	UpdateConditionButton();
}

VOID CSSConditionListDlg::EnableApplyButton(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(bEnable);
}

void CSSConditionListDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialog::OnCancel();
}
