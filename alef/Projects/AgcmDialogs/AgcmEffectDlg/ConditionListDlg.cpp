// ConditionListDlg.cpp : implementation file
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

static CConditionListDlg	*g_pcsConditionListDlgInst		= NULL;
static CSSConditionListDlg	*g_pcsSSConditionListDlgInst	= NULL;

/////////////////////////////////////////////////////////////////////////////
// CConditionListDlg dialog
CConditionListDlg *CConditionListDlg::GetInstance()
{
	return g_pcsConditionListDlgInst;
}

CConditionListDlg::CConditionListDlg(UINT32 *plCondition, UINT32 *plSSCondition, CWnd* pParent /*=NULL*/)
	: CDialog(CConditionListDlg::IDD, pParent)
	, m_bSpiritStone(FALSE)
	, m_bCharAnimCast(FALSE)
{
	//{{AFX_DATA_INIT(CConditionListDlg)
	m_unCondition = 0;
	m_bCharAnimAttack = FALSE;
	m_bCharAnimDead = FALSE;
	m_bCharAnimRun = FALSE;
	m_bCharAnimStruck = FALSE;
	m_bCharAnimWait = FALSE;
	m_bCharAnimWalk = FALSE;
	m_bTargetStruck = FALSE;
	m_bLevelUP = FALSE;
	m_bCommonInit = FALSE;
	m_bItemEquip = FALSE;
	m_bObjectWork = FALSE;
	m_bSkillTarget = FALSE;
	m_bItemSS = FALSE;
	m_bDestroy = FALSE;
	m_bCharResurrection = FALSE;
	m_bCharCreate = FALSE;
	m_bItemUse = FALSE;
	m_bTransform = FALSE;
	m_bCheckTime = FALSE;
	//}}AFX_DATA_INIT

	m_plSSCondition				= plSSCondition;
	m_plCondition				= plCondition;
	g_pcsConditionListDlgInst	= this;

	UpdateConditionButton(FALSE);
}


void CConditionListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConditionListDlg)
	DDX_Text(pDX, IDC_EDIT_CONDITION, m_unCondition);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_ATTACK, m_bCharAnimAttack);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_DEAD, m_bCharAnimDead);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_RUN, m_bCharAnimRun);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_STRUCK, m_bCharAnimStruck);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_WAIT, m_bCharAnimWait);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_WALK, m_bCharAnimWalk);
	DDX_Check(pDX, IDC_CHECK_CHAR_TARGET_STRUCK, m_bTargetStruck);
	DDX_Check(pDX, IDC_CHECK_CHARACTER_LEVEL_UP, m_bLevelUP);
	DDX_Check(pDX, IDC_CHECK_COMMON_INIT, m_bCommonInit);
	DDX_Check(pDX, IDC_CHECK_ITEM_EQUIP, m_bItemEquip);
	DDX_Check(pDX, IDC_CHECK_OBJECT_WORK, m_bObjectWork);
	DDX_Check(pDX, IDC_CHECK_SKILL_TARGET, m_bSkillTarget);
	DDX_Check(pDX, IDC_CHECK_ITEM_SPIRIT_STONE, m_bItemSS);
	DDX_Check(pDX, IDC_CHECK_COMMON_DESTROY, m_bDestroy);
	DDX_Check(pDX, IDC_CHECK_CHAR_RESURRECTION, m_bCharResurrection);
	DDX_Check(pDX, IDC_CHECK_CHARACTER_CREATE, m_bCharCreate);
	DDX_Check(pDX, IDC_CHECK_ITEM_USE, m_bItemUse);
	DDX_Check(pDX, IDC_CHECK_CHARACTER_TRANSFORM, m_bTransform);
	DDX_Check(pDX, IDC_CHECK_COMMON_CHECK_TIME, m_bCheckTime);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_COMMON_SPIRIT_STONE, m_bSpiritStone);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_CAST, m_bCharAnimCast);
}


BEGIN_MESSAGE_MAP(CConditionListDlg, CDialog)
	//{{AFX_MSG_MAP(CConditionListDlg)
	ON_BN_CLICKED(IDC_BUTTON_INPUT, OnButtonInput)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT, OnButtonOutput)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_SS_CONDITION, OnButtonOpenSsCondition)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConditionListDlg message handlers

void CConditionListDlg::OnButtonInput() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	InitializeConditionButton();

	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK)
		m_bCharAnimAttack = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_DEAD)
		m_bCharAnimDead = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_RUN)
		m_bCharAnimRun = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_STRUCK)
		m_bCharAnimStruck = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK)
		m_bTargetStruck = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT)
		m_bCharAnimWait = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WALK)
		m_bCharAnimWalk = TRUE;
//	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LEVELUP)
//		m_bLevelUP = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT)
		m_bCommonInit = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_EQUIP)
		m_bItemEquip = TRUE;
//	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_SPIRIT_STONE)
//		m_bItemSS = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_OBJECT_OP_WORK)
		m_bObjectWork = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET)
		m_bSkillTarget = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_DESTROY)
		m_bDestroy = TRUE;
//	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_CREATE)
//		m_bCharCreate = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_USE)
		m_bItemUse = TRUE;
//	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_RESURRECTION)
//		m_bCharResurrection = TRUE;
//	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_TRANSFORM)
//		m_bTransform = TRUE;
	if (m_unCondition & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_CHECK_TIME)
		m_bCheckTime = TRUE;

	UpdateData(FALSE);
}

void CConditionListDlg::OnButtonOutput() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_unCondition = 0;

	if (m_bCharAnimAttack)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK;

	if (m_bCharAnimDead)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_DEAD;

	if (m_bCharAnimRun)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_RUN;

	if (m_bCharAnimStruck)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_STRUCK;

	if (m_bTargetStruck)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK;

	if (m_bCharAnimWait)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT;

	if (m_bCharAnimWalk)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WALK;

//	if (m_bLevelUP)
//		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LEVELUP;

	if (m_bCommonInit)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT;

	if (m_bItemEquip)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_EQUIP;

//	if (m_bItemSS)
//		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_SPIRIT_STONE;

	if (m_bObjectWork)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_OBJECT_OP_WORK;

	if (m_bSkillTarget)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET;

	if (m_bDestroy)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_DESTROY;

//	if (m_bCharCreate)
//		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_CREATE;

	if (m_bItemUse)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_USE;

//	if (m_bCharResurrection)
//		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_RESURRECTION;

//	if (m_bTransform)
//		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_TRANSFORM;

	if (m_bCheckTime)
		m_unCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_CHECK_TIME;

	UpdateData(FALSE);
}

VOID CConditionListDlg::InitializeConditionButton()
{
	m_bCharAnimAttack			= FALSE;
	m_bCharAnimDead				= FALSE;
	m_bCharAnimRun				= FALSE;
	m_bCharAnimStruck			= FALSE;
	m_bCharAnimWait				= FALSE;
	m_bCharAnimWalk				= FALSE;
	m_bTargetStruck				= FALSE;
	m_bLevelUP					= FALSE;
	m_bCommonInit				= FALSE;
	m_bItemEquip				= FALSE;
	m_bObjectWork				= FALSE;
	m_bSkillTarget				= FALSE;
	m_bDestroy					= FALSE;
	m_bItemSS					= FALSE;
	m_bCharCreate				= FALSE;
	m_bItemUse					= FALSE;
	m_bTransform				= FALSE;
	m_bCharResurrection			= FALSE;
	m_bCheckTime				= FALSE;
}

BOOL CConditionListDlg::Create(CWnd* pParentWnd)
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void CConditionListDlg::OnOK() 
{
	// TODO: Add extra validation here
	CloseSSConditionDlg();

	if (AgcmEffectDlg::GetInstance())
		AgcmEffectDlg::GetInstance()->CloseEffectConditionList();
//	CDialog::OnOK();
}

void CConditionListDlg::OnDestroy() 
{
//	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
}

void CConditionListDlg::OnButtonOpenSsCondition() 
{
	// TODO: Add your control notification handler code here
	if (!g_pcsSSConditionListDlgInst)
	{
		g_pcsSSConditionListDlgInst = new CSSConditionListDlg(m_plSSCondition);

		g_pcsSSConditionListDlgInst->Create(NULL);
		g_pcsSSConditionListDlgInst->ShowWindow(SW_SHOW);
	}
}

VOID CConditionListDlg::CloseSSConditionDlg()
{
	if (g_pcsSSConditionListDlgInst)
	{
		g_pcsSSConditionListDlgInst->ShowWindow(SW_HIDE);
		g_pcsSSConditionListDlgInst->DestroyWindow();

		delete g_pcsSSConditionListDlgInst;
		g_pcsSSConditionListDlgInst = NULL;
	}
}

void CConditionListDlg::OnButtonApply() 
{
	// TODO: Add your control notification handler code here
	if (AgcmEffectDlg::GetInstance())
	{
		UpdateCondition();
		AgcmEffectDlg::GetInstance()->UpdateEffectScript();
//		AgcmEffectDlg::GetInstance()->_EnumCallback(AGCM_EFFECTDLG_CB_ID_APPLY_CONDITION, NULL, NULL);
	}
}

VOID CConditionListDlg::UpdateConditionButton(BOOL bUpdateData)
{
	InitializeConditionButton();

	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK)
		m_bCharAnimAttack = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_DEAD)
		m_bCharAnimDead = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_RUN)
		m_bCharAnimRun = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_STRUCK)
		m_bCharAnimStruck = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK)
		m_bTargetStruck = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT)
		m_bCharAnimWait = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WALK)
		m_bCharAnimWalk = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_CAST)
		m_bCharAnimCast = TRUE;
//	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LEVELUP)
//		m_bLevelUP = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT)
		m_bCommonInit = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_EQUIP)
		m_bItemEquip = TRUE;
//	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_SPIRIT_STONE)
//		m_bItemSS = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_OBJECT_OP_WORK)
		m_bObjectWork = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET)
		m_bSkillTarget = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_DESTROY)
		m_bDestroy = TRUE;
//	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_CREATE)
//		m_bCharCreate = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_USE)
		m_bItemUse = TRUE;
//	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_RESURRECTION)
//		m_bCharResurrection = TRUE;
//	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_TRANSFORM)
//		m_bTransform = TRUE;
	if (*m_plCondition & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_CHECK_TIME)
		m_bCheckTime = TRUE;

	if (bUpdateData)
		UpdateData(FALSE);
}

VOID CConditionListDlg::UpdateCondition()
{
	UpdateData(TRUE);
	*m_plCondition = 0;

	if (m_bCharAnimAttack)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK;

	if (m_bCharAnimDead)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_DEAD;

	if (m_bCharAnimRun)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_RUN;

	if (m_bCharAnimStruck)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_STRUCK;

	if (m_bTargetStruck)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK;

	if (m_bCharAnimWait)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT;

	if (m_bCharAnimWalk)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WALK;

	if (m_bCharAnimCast)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_CAST;

//	if (m_bLevelUP)
//		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LEVELUP;

	if (m_bCommonInit)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT;

	if (m_bItemEquip)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_EQUIP;

//	if (m_bItemSS)
//		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_SPIRIT_STONE;

	if (m_bObjectWork)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_OBJECT_OP_WORK;

	if (m_bSkillTarget)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET;

	if (m_bDestroy)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_DESTROY;

//	if (m_bCharCreate)
//		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_CREATE;

	if (m_bItemUse)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_USE;

//	if (m_bCharResurrection)
//		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_RESURRECTION;

//	if (m_bTransform)
//		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_TRANSFORM;

	if (m_bCheckTime)
		*m_plCondition			|= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_CHECK_TIME;
}

VOID CConditionListDlg::ApplySSCondition()
{
	if (!AgcmEffectDlg::GetInstance())
		return;

	AgcmEffectDlg::GetInstance()->UpdateEffectScript();
//	AgcmEffectDlg::GetInstance()->_EnumCallback(AGCM_EFFECTDLG_CB_ID_APPLY_SS_CONDITION, NULL, NULL);
}

VOID CConditionListDlg::UpdateConditionDlg(UINT32 *plCondition, UINT32 *plSSCondition)
{
	m_plCondition		= plCondition;
	m_plSSCondition		= plSSCondition;

	UpdateConditionButton();

	if (g_pcsSSConditionListDlgInst)
		g_pcsSSConditionListDlgInst->UpdateConditionDlg(m_plSSCondition);
}

VOID CConditionListDlg::EnableApplyButton(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(bEnable);

	if (g_pcsSSConditionListDlgInst)
		g_pcsSSConditionListDlgInst->EnableApplyButton(bEnable);
}

BOOL CConditionListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_CHECK_CHAR_RESURRECTION)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_CHARACTER_LEVEL_UP)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_CHARACTER_CREATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_CHARACTER_TRANSFORM)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConditionListDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialog::OnCancel();
}
