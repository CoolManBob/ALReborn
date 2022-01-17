// AgcmCharacterSkillDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"

#include "AgcmSkillDlg.h"
#include "AgcmCharacterSkillDlg.h"
#include "AgcmFileListDlg.h"
#include "AgcmEffectDlg.h"
#include "AgcmSetDefSkillDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmCharacterSkillDlg dialog

/*
AgcmCharacterSkillDlg::AgcmCharacterSkillDlg(AgpdSkillTemplateAttachData *pcsAgpdSkillData, AgcdSkillAttachTemplateData *pcsgcdSkillData, CHAR *szFindAnimPathName1, CHAR *szFindAnimPathName2, CWnd* pParent )
	: CDialog(AgcmCharacterSkillDlg::IDD, pParent)
{
	CString strSkillDataAnimationName = "";

	if(	(pcsAgcdSkillData->m_astSkillVisualInfo->m_pastAnim[0]) &&
		(pcsAgcdSkillData->m_astSkillVisualInfo->m_pastAnim[0]->m_ppastAnimData[0]) &&
		(pcsAgcdSkillData->m_astSkillVisualInfo->m_paszAnimName[0])					)
	{
		strSkillDataAnimationName = pcsAgcdSkillData->m_astSkillVisualInfo->m_paszAnimName[0];
	}

	//{{AFX_DATA_INIT(AgcmCharacterSkillDlg)
	m_nSkillDataIndex			= -1;
	m_strSkillDataAnimationName	= strSkillDataAnimationName;
	m_strSkillName				= _T((pcsAgpdSkillData->m_aszUsableSkillTName[0]) ? (pcsAgpdSkillData->m_aszUsableSkillTName[0]) : (""));
	m_nEquipType = -1;
	//}}AFX_DATA_INIT

	m_pcsAgcdSkillData = pcsAgcdSkillData;
	memcpy(&m_csAgcdTempSkillData, pcsAgcdSkillData, sizeof(AgcdSkillAttachTemplateData));

	m_pcsAgpdSkillData = pcsAgpdSkillData;
	memcpy(&m_csAgpdTempSkillData, pcsAgpdSkillData, sizeof(AgpdSkillTemplateAttachData));

	strcpy(m_szFindAnimPathName1, szFindAnimPathName1);
	strcpy(m_szFindAnimPathName2, szFindAnimPathName2);
}


void AgcmCharacterSkillDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmCharacterSkillDlg)
	DDX_CBIndex(pDX, IDC_SKILL_DATA_INDEX, m_nSkillDataIndex);
	DDX_Text(pDX, IDC_EDIT_SKILL_DATA_ANIMATION_NAME, m_strSkillDataAnimationName);
	DDX_Text(pDX, IDC_EDIT_SKILL_DATA_SKILL_NAME, m_strSkillName);
	DDX_CBIndex(pDX, IDC_COMBO_EQUIP_TYPE, m_nEquipType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmCharacterSkillDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmCharacterSkillDlg)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_DATA_SET_ANIMATION, OnButtonSkillDataSetAnimation)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_DATA_SET_EFFECT, OnButtonSkillDataSetEffect)
	ON_CBN_SELCHANGE(IDC_SKILL_DATA_INDEX, OnSelchangeSkillDataIndex)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_DATA_SET_SKILL_NAME, OnButtonSkillDataSetSkillName)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_DATA_SET_DEF_SKILL, OnButtonSkillDataSetDefSkill)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_SKILL, OnButtonApplySkill)
	ON_BN_CLICKED(IDC_BUTTON_START_ANIMATION, OnButtonStartAnimation)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SKILL_DATA_REMOVE, OnButtonSkillDataRemove)

	// 마고자 2004/12/21 존재 하지 않는 인덱스.
	//ON_CBN_SELCHANGE(IDC_SKILL_DATA_LEVEL_INDEX, OnSelchangeSkillDataLevelIndex)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_DATA_SET_ANIM_PROPERTY, OnButtonSkillDataSetAnimProperty)
	ON_BN_CLICKED(IDC_BUTTON_START_EFFECT, OnButtonStartEffect)
	ON_CBN_SELCHANGE(IDC_COMBO_EQUIP_TYPE, OnSelchangeComboEquipType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmCharacterSkillDlg message handlers

void AgcmCharacterSkillDlg::OnButtonSkillDataSetAnimation() 
{
	// TODO: Add your control notification handler code here
	CHAR			*pszFileName;
	AgcmFileListDlg	dlg;

	pszFileName = dlg.OpenFileList(m_szFindAnimPathName1, m_szFindAnimPathName2, NULL);
	if(pszFileName)
	{
		UpdateData(TRUE);

		AgcmSkill				*pcsAgcmSkill				= AgcmSkillDlg::GetInstance()->GetAgcmSkill();
		AgpdCharacterTemplate	*pcsAgpdCharacterTemplate	= pcsAgcmSkill->GetCharacterTemplate(m_pcsAgcdSkillData);

		AgcmSkillDlgGetAnimationSet csSet;
		strcpy(csSet.m_szAnimName, pszFileName);
		csSet.m_ppcsAgcdAnimation	= &m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType];
		csSet.m_lTID				= pcsAgpdCharacterTemplate->m_lID;
		csSet.m_lDataIndex			= m_nSkillDataIndex;
		csSet.m_lEquipType			= m_nEquipType;

		// callback
		AgcmSkillDlg::GetInstance()->EnumCallbackGetAnimation(&csSet);

		if(	(!m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType]) ||
			(!m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType]->m_ppastAnimData[0])	)
		{
			MessageBox("Animation을 설정할 수 없습니다!", "ERROR");
			return;
		}

		if(!m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType])
			m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType] = (CHAR *)(malloc(AGCDSKILL_ANIMATION_NAME_LENGTH));

		strcpy(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType], pszFileName);
		m_strSkillDataAnimationName = pszFileName;

		GetDlgItem(IDC_BUTTON_SKILL_DATA_SET_ANIM_PROPERTY)->EnableWindow(TRUE);

		UpdateData(FALSE);
	}
}

void AgcmCharacterSkillDlg::OnButtonSkillDataSetEffect() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
//	for(INT16 nCount = 0; nCount < EFFECT_USE_EFFECT_SET_MAX_NUM; ++nCount)
//	{
//		m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect[m_nSkillLevelIndex].m_astData[nCount].m_lConditionFlag |= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_SKILL;
//	}

	AgcmEffectDlg *pcsEffectDlg = AgcmSkillDlg::GetInstance()->GetAgcmEffectDlg();
	if(pcsEffectDlg)
	{
		pcsEffectDlg->OpenUseEffectSet(&m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect[m_nEquipType]);
	}
}

VOID AgcmCharacterSkillDlg::ApplyData()
{
	UpdateData(TRUE);

	memcpy(m_pcsAgcdSkillData, &m_csAgcdTempSkillData, sizeof(AgcdSkillAttachTemplateData));
	memcpy(m_pcsAgpdSkillData, &m_csAgpdTempSkillData, sizeof(AgpdSkillTemplateAttachData));
}

void AgcmCharacterSkillDlg::OnOK() 
{
	// TODO: Add extra validation here
	ApplyData();

	AgcmSkillDlg::GetInstance()->CloseCharacterSkillDlg();
//	CDialog::OnOK();
}

void AgcmCharacterSkillDlg::OnSelchangeSkillDataIndex() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_strSkillName				=
		(m_csAgpdTempSkillData.m_aszUsableSkillTName[m_nSkillDataIndex])	?
		(m_csAgpdTempSkillData.m_aszUsableSkillTName[m_nSkillDataIndex])	:
		("")																;
		

	if(	(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType]) &&
		(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType]->m_ppastAnimData[0]) &&
		(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType])					)
	{
		m_strSkillDataAnimationName = m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType];

		GetDlgItem(IDC_BUTTON_SKILL_DATA_SET_ANIM_PROPERTY)->EnableWindow(TRUE);
	}
	else
	{
		m_strSkillDataAnimationName = "";

		GetDlgItem(IDC_BUTTON_SKILL_DATA_SET_ANIM_PROPERTY)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}

BOOL AgcmCharacterSkillDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CHAR		szTemp[256];
	CComboBox	*pcsComboSkillIndex		= (CComboBox *)(GetDlgItem(IDC_SKILL_DATA_INDEX));

	for(INT16 nIndex = 0; nIndex < AGPMSKILL_MAX_SKILL_OWN; ++nIndex)
	{
		sprintf(szTemp, "%d", nIndex);
		pcsComboSkillIndex->InsertString(nIndex, szTemp);
	}

	if(nIndex > 0)
		pcsComboSkillIndex->SetCurSel(0);

	CComboBox *pcsEquipType = (CComboBox *)(GetDlgItem(IDC_COMBO_EQUIP_TYPE));
	for(INT32 lEquipType = 0; lEquipType < AGCDITEM_EQUIP_ANIMATION_TYPE_NUM; ++lEquipType)
	{
		pcsEquipType->InsertString(lEquipType, AgcmSkillDlg::GetInstance()->GetAgcmItem()->GetItemEquipAnimationName(lEquipType));
	}

	if(lEquipType > 0)
		pcsEquipType->SetCurSel(0);

	UpdateData(TRUE);

	if(	(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType]) &&
		(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType]->m_ppastAnimData[0]) &&
		(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType])					)
	{
		GetDlgItem(IDC_BUTTON_SKILL_DATA_SET_ANIM_PROPERTY)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_SKILL_DATA_SET_ANIM_PROPERTY)->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmCharacterSkillDlg::OnButtonSkillDataSetSkillName() 
{
	// TODO: Add your control notification handler code here
//	CHAR szSkillTemplateName[256];

	CHAR	*pszSkillTemplateName;
	if(AgcmSkillDlg::GetInstance()->OpenSkillTemplateList(&pszSkillTemplateName))
	{
		UpdateData(TRUE);
		m_csAgpdTempSkillData.m_aszUsableSkillTName[m_nSkillDataIndex]	= pszSkillTemplateName;

		m_strSkillName = pszSkillTemplateName;
		UpdateData(FALSE);
	}
}

void AgcmCharacterSkillDlg::OnButtonSkillDataSetDefSkill() 
{
	// TODO: Add your control notification handler code here
	//AgcmSetDefSkillDlg dlg(&m_csAgpdTempSkillData);
	//dlg.DoModal();
}

void AgcmCharacterSkillDlg::OnButtonApplySkill() 
{
	// TODO: Add your control notification handler code here
	ApplyData();
}

void AgcmCharacterSkillDlg::OnButtonStartAnimation() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(m_pcsAgcdSkillData->m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType])
	{
		AgcmSkillDlgStartAnimationSet csSet;
		csSet.m_pstAgcdAnimation	= m_pcsAgcdSkillData->m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType];
		csSet.m_lSkillUsableIndex	= m_nSkillDataIndex;
		csSet.m_lEquipType			= m_nEquipType;

		AgcmSkillDlg::GetInstance()->EnumCallbackStartSkillAnimation(&csSet);
	}
	else
		MessageBox("Animation값이 적용되지 않았습니다.\nApply버튼을 눌러보세요.", "Error");
}

BOOL AgcmCharacterSkillDlg::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

void AgcmCharacterSkillDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	AgcmSkillDlg::GetInstance()->CloseCharacterSkillDlg();
//	CDialog::OnCancel();
}

void AgcmCharacterSkillDlg::OnDestroy() 
{
//	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void AgcmCharacterSkillDlg::OnButtonSkillDataRemove() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType] = NULL;
	if(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType])
		free(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType]);

//	for(INT16 nEffectIndex = 0; nEffectIndex < D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM; ++nEffectIndex)
//	{
//		if(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect[m_nEquipType].m_astData[nEffectIndex])
//		{
//			memset(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect[m_nEquipType].m_astData[nEffectIndex], 0, sizeof(AgcdUseEffectSetData));
//			m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect[m_nEquipType].m_astData[nEffectIndex]->m_fScale	= 1.0f;
////			m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect[m_nEquipType].m_astData[nEffectIndex]->m_lCustData = -1;
//		}
//	}

	AgcmSkillDlg::GetInstance()->GetAgcmEventEffect()->GetAgcaEffectData2()->DeleteAll(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect + m_nEquipType);

	m_csAgpdTempSkillData.m_aszDefaultTName[m_nSkillDataIndex][0]		= NULL;
	m_csAgpdTempSkillData.m_aszUsableSkillTName[m_nSkillDataIndex]		= NULL;

	m_strSkillDataAnimationName = "";
	m_strSkillName				= "";

	UpdateData(FALSE);
}

void AgcmCharacterSkillDlg::OnSelchangeSkillDataLevelIndex() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	if(	(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType]) &&
		(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType]->m_ppastAnimData[0]) &&
		(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType])					)
	{
		m_strSkillDataAnimationName = m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType];

		GetDlgItem(IDC_BUTTON_SKILL_DATA_SET_ANIM_PROPERTY)->EnableWindow(TRUE);
	}
	else
	{
		m_strSkillDataAnimationName = "";

		GetDlgItem(IDC_BUTTON_SKILL_DATA_SET_ANIM_PROPERTY)->EnableWindow(FALSE);
	}


	UpdateData(FALSE);
}

void AgcmCharacterSkillDlg::OnButtonSkillDataSetAnimProperty() 
{
	// TODO: Add your control notification handler code here
	AgcmAnimationDlg *pcsAgcmAnimationDlg = AgcmSkillDlg::GetInstance()->GetAgcmAnimationDlg();
	if(pcsAgcmAnimationDlg)
	{
		UpdateData(TRUE);

		if(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType])
		{
			AgcdCharacterAnimationAttachedData *pcsData	= (AgcdCharacterAnimationAttachedData *)(
			AgcmSkillDlg::GetInstance()->GetAgcmSkill()->GetAgcmCharacter()->GetAnimationAttachedData(
				m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType],
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY												)	);

			pcsAgcmAnimationDlg->OpenAnimProperty(pcsData, 0);
		}
	}
}

void AgcmCharacterSkillDlg::OnButtonStartEffect() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

//	if(	(m_pcsAgcdSkillData->m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect[m_nEquipType].m_astData[0]) &&
//		(m_pcsAgcdSkillData->m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect[m_nEquipType].m_astData[0]->m_ulEID)	)
	if (	(m_pcsAgcdSkillData->m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect[m_nEquipType].m_pcsHead) &&
			(m_pcsAgcdSkillData->m_astSkillVisualInfo[m_nSkillDataIndex].m_astEffect[m_nEquipType].m_pcsHead->m_csData.m_ulEID)	)
	{
		AgcmSkillDlg::GetInstance()->EnumCallbackStartSkillEffect(m_nSkillDataIndex, m_nEquipType);
	}
	else
		MessageBox("Effect가 적용되지 않았습니다.\nApply버튼을 눌러보세요.", "Error");
}

void AgcmCharacterSkillDlg::OnSelchangeComboEquipType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(	(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType]) &&
		(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_pastAnim[m_nEquipType]->m_ppastAnimData[0]) &&
		(m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType])					)
	{
		m_strSkillDataAnimationName = m_csAgcdTempSkillData.m_astSkillVisualInfo[m_nSkillDataIndex].m_paszAnimName[m_nEquipType];

		GetDlgItem(IDC_BUTTON_SKILL_DATA_SET_ANIM_PROPERTY)->EnableWindow(TRUE);
	}
	else
	{
		m_strSkillDataAnimationName = "";

		GetDlgItem(IDC_BUTTON_SKILL_DATA_SET_ANIM_PROPERTY)->EnableWindow(FALSE);
	}

	UpdateData(FALSE);
}
*/