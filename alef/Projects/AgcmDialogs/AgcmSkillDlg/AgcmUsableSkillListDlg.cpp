// AgcmUsableSkillListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmSkillDlg.h"
#include "AgcmUsableSkillListDlg.h"
#include "../EditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmUsableSkillListDlg dialog


AgcmUsableSkillListDlg::AgcmUsableSkillListDlg(AgpdSkillTemplateAttachData	*pcsAgpdSkillData, CHAR *pszDest, CWnd* pParent /*=NULL*/)
	: CDialog(AgcmUsableSkillListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmUsableSkillListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pszDest			= pszDest;
	m_pcsAgpdSkillData	= pcsAgpdSkillData;
}


void AgcmUsableSkillListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmUsableSkillListDlg)
	DDX_Control(pDX, IDC_LIST_USABLE_SKILL, m_csUsableSkillList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmUsableSkillListDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmUsableSkillListDlg)
	ON_BN_CLICKED(IDC_BUTTON_FIND_USABLE_SKILL, OnButtonFindUsableSkill)
	ON_LBN_DBLCLK(IDC_LIST_USABLE_SKILL, OnDblclkListUsableSkill)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmUsableSkillListDlg message handlers
VOID AgcmUsableSkillListDlg::_ToUpper(CHAR *szDest, CHAR *szSrc)
{
	strcpy(szDest, szSrc);
	INT16 nMax = strlen(szSrc);
	for(INT16 nCount = 0; nCount < nMax; szDest[nCount] = toupper(szSrc[nCount]), ++nCount);
}

BOOL AgcmUsableSkillListDlg::IsLowEqual(CHAR *szCmp1, CHAR *szCmp2)
{
	CHAR szTemp1[256], szTemp2[256];

	_ToUpper(szTemp1, szCmp1);
	_ToUpper(szTemp2, szCmp2);

	INT16 nMax = strlen(szTemp2);
	if(nMax > (INT16)(strlen(szTemp1)))
		return FALSE;

	for(INT16 nCount = 0; nCount < nMax; ++nCount)
	{
		if(szTemp1[nCount] != szTemp2[nCount])
			return FALSE;
	}

	return TRUE;
}

void AgcmUsableSkillListDlg::OnButtonFindUsableSkill() 
{
	// TODO: Add your control notification handler code here
	CHAR szSelected[256];

	CEditDlg dlg(szSelected);
	if(dlg.DoModal() == IDOK)
	{
		CHAR	szCurList[256];
		INT16	nMax = m_csUsableSkillList.GetCount();
		for(INT16 nCount = 0; nCount < nMax; ++nCount)
		{
			m_csUsableSkillList.GetText(nCount, szCurList);

			if(IsLowEqual(szCurList, szSelected))
			{
				m_csUsableSkillList.SetCurSel(nCount);
				break;
			}
		}
	}
}

BOOL AgcmUsableSkillListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	BOOL bSet = FALSE;

	for(INT16 nIndex = 0; nIndex < AGPMSKILL_MAX_SKILL_OWN; ++nIndex)
	{
		if(m_pcsAgpdSkillData->m_aszUsableSkillTName[nIndex][0] != NULL)
		{
			m_csUsableSkillList.AddString(m_pcsAgpdSkillData->m_aszUsableSkillTName[nIndex]);
			bSet = TRUE;
		}
	}

	if(bSet)
		m_csUsableSkillList.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmUsableSkillListDlg::OnDblclkListUsableSkill() 
{
	// TODO: Add your control notification handler code here
	m_csUsableSkillList.GetText(m_csUsableSkillList.GetCurSel(), m_pszDest);

	EndDialog(IDOK);
}
