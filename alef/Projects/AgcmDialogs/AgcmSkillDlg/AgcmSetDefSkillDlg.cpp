// AgcmSetDefSkillDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmSkillDlg.h"
#include "AgcmSetDefSkillDlg.h"
#include "AgcmUsableSkillListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmSetDefSkillDlg dialog


AgcmSetDefSkillDlg::AgcmSetDefSkillDlg(AgpdSkillTemplateAttachData	*pcsAgpdSkillData, CWnd* pParent /*=NULL*/)
	: CDialog(AgcmSetDefSkillDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmSetDefSkillDlg)
	m_strDefSkillName = _T( (pcsAgpdSkillData->m_aszDefaultTName[0][0] == NULL) ? ("") : (pcsAgpdSkillData->m_aszDefaultTName[0]) );
	m_nDefSkillIndex = -1;
	//}}AFX_DATA_INIT

	m_pcsAgpdSkillData = pcsAgpdSkillData;
	memcpy(&m_csAgpdTempSkillData, pcsAgpdSkillData, sizeof(AgpdSkillTemplateAttachData));
}


void AgcmSetDefSkillDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmSetDefSkillDlg)
	DDX_Text(pDX, IDC_EDIT_DEFAULT_SKILL_NAME, m_strDefSkillName);
	DDX_CBIndex(pDX, IDC_COMBO_DEF_SKILL_INDEX, m_nDefSkillIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmSetDefSkillDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmSetDefSkillDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_DEF_SKILL_INDEX, OnSelchangeComboDefSkillIndex)
	ON_BN_CLICKED(IDC_BUTTON_SKILL_DATA_SET_DEFAULT_SKILL_NAME, OnButtonSkillDataSetDefaultSkillName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmSetDefSkillDlg message handlers

void AgcmSetDefSkillDlg::OnSelchangeComboDefSkillIndex() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_strDefSkillName = m_csAgpdTempSkillData.m_aszDefaultTName[m_nDefSkillIndex];

	UpdateData(FALSE);
}

BOOL AgcmSetDefSkillDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CHAR		szTemp[256];
	CComboBox	*pcsCombo		= (CComboBox *)(GetDlgItem(IDC_COMBO_DEF_SKILL_INDEX));

	INT16 nIndex;

	for(nIndex = 0; nIndex < AGPMSKILL_MAX_DEFAULT_SKILL; ++nIndex)
	{
		sprintf(szTemp, "%d", nIndex);
		pcsCombo->InsertString(nIndex, szTemp);
	}

	if(nIndex > 0)
		pcsCombo->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmSetDefSkillDlg::OnButtonSkillDataSetDefaultSkillName() 
{
	// TODO: Add your control notification handler code here
	CHAR szTemp[256];

	AgcmUsableSkillListDlg dlg(&m_csAgpdTempSkillData, szTemp);
	if(dlg.DoModal() == IDOK)
	{
		UpdateData(TRUE);
		strcpy(m_csAgpdTempSkillData.m_aszDefaultTName[m_nDefSkillIndex], szTemp);

		m_strDefSkillName = szTemp;
		UpdateData(FALSE);
	}
}

void AgcmSetDefSkillDlg::OnOK() 
{
	// TODO: Add extra validation here
	memcpy(	m_pcsAgpdSkillData->m_aszDefaultTName,
			m_csAgpdTempSkillData.m_aszDefaultTName,
			AGPMSKILL_MAX_DEFAULT_SKILL * (AGPMSKILL_MAX_SKILL_NAME + 1));

	CDialog::OnOK();
}
